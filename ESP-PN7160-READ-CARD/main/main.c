#include <stdio.h>
#include "esp32_board_config.h"
extern void nfc_example(void);

void app_main(void) {
    printf("Running the NXP-NCI2.0 example (I2C interface)\n");
    nfc_example();
}