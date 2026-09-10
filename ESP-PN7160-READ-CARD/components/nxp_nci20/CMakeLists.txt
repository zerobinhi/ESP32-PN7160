idf_component_register(
    SRCS "src/NxpNci20.c"
         "src/NdefLibrary/RW_NDEF.c"
         "src/NdefLibrary/RW_NDEF_T1T.c"
         "src/NdefLibrary/RW_NDEF_T2T.c"
         "src/NdefLibrary/RW_NDEF_T3T.c"
         "src/NdefLibrary/RW_NDEF_T4T.c"
         "src/NdefLibrary/RW_NDEF_T5T.c"
         "src/NdefLibrary/RW_NDEF_MIFARE.c"
         "src/NdefLibrary/T4T_NDEF_emu.c"
         "src/NdefLibrary/P2P_NDEF.c"
         "src/tml.c"
         "src/tool.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_driver_gpio esp_driver_i2c freertos
)