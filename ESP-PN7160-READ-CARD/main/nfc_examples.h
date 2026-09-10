/*
* NFC example entry points for ESP-IDF port of NXP-NCI2.0 demos.
*
* Each demo is self-contained with file-local (static) symbols.
* Select which demo runs in main.c via the EXAMPLE_xxx macro.
*/

#ifndef NFC_EXAMPLES_H
#define NFC_EXAMPLES_H

/* Reader/Writer demo: read & write various tag types (MIFARE, T2T, T5T, ISODEP) */
void nfc_example_RW(void);

/* Card Emulation demo: emulate a T4T NDEF tag (NFCEE_NDEF) */
void nfc_example_CE(void);

/* P2P demo: SNEP NDEF exchange with a peer NFC device */
void nfc_example_P2P(void);

/* Reader/Writer + Card Emulation demo: both modes simultaneously */
void nfc_example_RWandCE(void);

#endif /* NFC_EXAMPLES_H */
