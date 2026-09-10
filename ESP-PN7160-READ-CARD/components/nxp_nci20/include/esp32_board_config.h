#ifndef ESP32_BOARD_CONFIG_H
#define ESP32_BOARD_CONFIG_H

#include <driver/gpio.h>
#include <driver/i2c_master.h>

/* 选择通信接口：二选一 */
#define BOARD_NXPNCI_INTERFACE_I2C
//#define BOARD_NXPNCI_INTERFACE_SPI

#define BOARD_NXPNCI_I2C_PORT       I2C_NUM_0
#define BOARD_NXPNCI_I2C_SDA_PIN    GPIO_NUM_21
#define BOARD_NXPNCI_I2C_SCL_PIN    GPIO_NUM_22
#define BOARD_NXPNCI_I2C_BAUDRATE   (100000)
#define BOARD_NXPNCI_I2C_ADDR       (0x28)

/* GPIO 引脚定义 */
#define BOARD_NXPNCI_IRQ_PIN        GPIO_NUM_4     // 输入
#define BOARD_NXPNCI_VEN_PIN        GPIO_NUM_14    // 输出(复位)
#define BOARD_NXPNCI_DWL_PIN        GPIO_NUM_13    // 输出(下载模式)

#endif /* ESP32_BOARD_CONFIG_H */
