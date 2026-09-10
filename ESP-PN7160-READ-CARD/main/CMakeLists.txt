idf_component_register(
    SRCS "main.c"
         "nfc_example_RW.c"
         "nfc_example_CE.c"
         "nfc_example_P2P.c"
         "nfc_example_RWandCE.c"
         "ndef_helper.c"
    INCLUDE_DIRS "."
    REQUIRES nxp_nci20
)

target_compile_definitions(${COMPONENT_LIB} PRIVATE
    REMOVE_FACTORYTEST_SUPPORT
    # Enable P2P, CardEmu, and NDEF support for all demos
    # REMOVE_P2P_SUPPORT
    # REMOVE_CARDEMU_SUPPORT
    # REMOVE_NDEF_SUPPORT
)
