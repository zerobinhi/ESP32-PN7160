#ifndef ESP32_BOARD_CONFIG_H
#define ESP32_BOARD_CONFIG_H

/* 选择通信接口：二选一 */
#define BOARD_NXPNCI_INTERFACE_I2C
//#define BOARD_NXPNCI_INTERFACE_SPI

#define BOARD_NXPNCI_I2C_PORT       0
#define BOARD_NXPNCI_I2C_SDA_PIN    21
#define BOARD_NXPNCI_I2C_SCL_PIN    22
#define BOARD_NXPNCI_I2C_BAUDRATE   (100000)
#define BOARD_NXPNCI_I2C_ADDR       (0x28)

/* GPIO 引脚定义 */
#define BOARD_NXPNCI_IRQ_PIN        4     // 输入
#define BOARD_NXPNCI_VEN_PIN        14    // 输出(复位)
#define BOARD_NXPNCI_DWL_PIN        13    // 输出(下载模式)

#endif /* ESP32_BOARD_CONFIG_H */
