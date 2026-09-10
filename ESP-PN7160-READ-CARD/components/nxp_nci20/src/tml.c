/*
* NXP-NCI2.0 Transport Middleware Layer - ESP-IDF port
*/

#include "tml.h"
#include "esp32_board_config.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

typedef enum { ERROR = 0, SUCCESS = !ERROR } Status;

static i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t dev_handle;
static bool isDwlMode = false;

#define HEADER_SZ (isDwlMode==true?1:2)
#define FOOTER_SZ (isDwlMode==true?2:0)

static void INTF_INIT(void)
{
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = BOARD_NXPNCI_I2C_PORT,
        .sda_io_num = BOARD_NXPNCI_I2C_SDA_PIN,
        .scl_io_num = BOARD_NXPNCI_I2C_SCL_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags.enable_internal_pullup = true,
    };
    i2c_new_master_bus(&bus_cfg, &bus_handle);

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = BOARD_NXPNCI_I2C_ADDR,
        .scl_speed_hz = BOARD_NXPNCI_I2C_BAUDRATE,
    };
    i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle);
}

static Status INTF_WRITE(uint8_t *pBuff, uint16_t buffLen)
{
    return (i2c_master_transmit(dev_handle, pBuff, buffLen, -1) == ESP_OK) ? SUCCESS : ERROR;
}

static Status INTF_READ(uint8_t *pBuff, uint16_t buffLen)
{
    return (i2c_master_receive(dev_handle, pBuff, buffLen, -1) == ESP_OK) ? SUCCESS : ERROR;
}

static Status tml_Init(void)
{
    /* VEN + DWL: output */
    gpio_config_t out_conf = {
        .pin_bit_mask = (1ULL << BOARD_NXPNCI_VEN_PIN) | (1ULL << BOARD_NXPNCI_DWL_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&out_conf);

    /* IRQ: input */
    gpio_config_t irq_conf = {
        .pin_bit_mask = (1ULL << BOARD_NXPNCI_IRQ_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&irq_conf);

    isDwlMode = false;
    INTF_INIT();

    return SUCCESS;
}

static Status tml_DeInit(void)
{
    gpio_set_level(BOARD_NXPNCI_VEN_PIN, 0);
    return SUCCESS;
}

static Status tml_Reset(void)
{
    gpio_set_level(BOARD_NXPNCI_VEN_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(BOARD_NXPNCI_VEN_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
    return SUCCESS;
}

static Status tml_Tx(uint8_t *pBuff, uint16_t buffLen)
{
    if (INTF_WRITE(pBuff, buffLen) != SUCCESS)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
        if (INTF_WRITE(pBuff, buffLen) != SUCCESS)
        {
            return ERROR;
        }
    }
    return SUCCESS;
}

static Status tml_Rx(uint8_t *pBuff, uint16_t buffLen, uint16_t *pBytesRead)
{
    if (INTF_READ(pBuff, HEADER_SZ + 1) == SUCCESS)
    {
        if ((pBuff[HEADER_SZ] + HEADER_SZ + 1) <= buffLen)
        {
            if (pBuff[HEADER_SZ] > 0)
            {
                if (INTF_READ(&pBuff[HEADER_SZ + 1], pBuff[HEADER_SZ] + FOOTER_SZ) == SUCCESS)
                {
                    *pBytesRead = pBuff[HEADER_SZ] + HEADER_SZ + 1;
                }
                else return ERROR;
            }
            else
            {
                *pBytesRead = HEADER_SZ + 1;
            }
        }
        else return ERROR;
    }
    else return ERROR;

    return SUCCESS;
}

static Status tml_WaitForRx(uint32_t timeout)
{
    if (timeout == 0)
    {
        while (gpio_get_level(BOARD_NXPNCI_IRQ_PIN) == 0)
        {
            vTaskDelay(1);
        }
    }
    else
    {
        int32_t to = timeout;
        while (gpio_get_level(BOARD_NXPNCI_IRQ_PIN) == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
            to -= 10;
            if (to <= 0) return ERROR;
        }
    }
    return SUCCESS;
}

void tml_Connect(void)
{
    tml_Init();
    tml_Reset();
}

void tml_Disconnect(void)
{
    tml_DeInit();
}

void tml_EnterDwlMode(void)
{
    isDwlMode = true;
    gpio_set_level(BOARD_NXPNCI_DWL_PIN, 1);
    tml_Reset();
}

void tml_LeaveDwlMode(void)
{
    isDwlMode = false;
    gpio_set_level(BOARD_NXPNCI_DWL_PIN, 0);
    tml_Reset();
}

void tml_Send(uint8_t *pBuffer, uint16_t BufferLen, uint16_t *pBytesSent)
{
    if (tml_Tx(pBuffer, BufferLen) == ERROR)
        *pBytesSent = 0;
    else
        *pBytesSent = BufferLen;
}

void tml_Receive(uint8_t *pBuffer, uint16_t BufferLen, uint16_t *pBytes, uint16_t timeout)
{
    if (tml_WaitForRx(timeout) == ERROR)
        *pBytes = 0;
    else
        tml_Rx(pBuffer, BufferLen, pBytes);
}