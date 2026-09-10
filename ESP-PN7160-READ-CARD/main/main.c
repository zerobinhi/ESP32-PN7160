#include <stdio.h>
#include "esp32_board_config.h"
#include "nfc_examples.h"

/*
* Select which demo to run.
* Change EXAMPLE_xxx below and rebuild with: idf.py build flash
*
*   EXAMPLE_RW       - Reader/Writer: read & write MIFARE, T2T, T5T, ISODEP tags
*   EXAMPLE_CE       - Card Emulation: emulate a T4T NDEF tag (NFCEE_NDEF)
*   EXAMPLE_P2P      - P2P: SNEP NDEF exchange with a peer NFC device
*   EXAMPLE_RW_CE    - Reader/Writer + Card Emulation simultaneously
*/

#define EXAMPLE_RW
//#define EXAMPLE_CE
//#define EXAMPLE_P2P
//#define EXAMPLE_RW_CE

void app_main(void) {
#if defined(EXAMPLE_RW)
    printf("Running the NXP-NCI2.0 example: Reader/Writer (I2C interface)\n");
    nfc_example_RW();
#elif defined(EXAMPLE_CE)
    printf("Running the NXP-NCI2.0 example: Card Emulation (I2C interface)\n");
    nfc_example_CE();
#elif defined(EXAMPLE_P2P)
    printf("Running the NXP-NCI2.0 example: P2P (I2C interface)\n");
    nfc_example_P2P();
#elif defined(EXAMPLE_RW_CE)
    printf("Running the NXP-NCI2.0 example: Reader/Writer + Card Emulation (I2C interface)\n");
    nfc_example_RWandCE();
#else
    #error "No example selected. Define one of EXAMPLE_RW / EXAMPLE_CE / EXAMPLE_P2P / EXAMPLE_RW_CE in main.c"
#endif
}
