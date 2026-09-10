idf_component_register(
    SRCS "main.c" "nfc_example_RW.c" "ndef_helper.c"
    INCLUDE_DIRS "."
    REQUIRES nxp_nci20
)

target_compile_definitions(${COMPONENT_LIB} PRIVATE
    REMOVE_FACTORYTEST_SUPPORT
    REMOVE_P2P_SUPPORT
    # REMOVE_CARDEMU_SUPPORT
    # REMOVE_NDEF_SUPPORT
)