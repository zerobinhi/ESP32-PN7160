/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*
* Ported to ESP-IDF: all file-local symbols made static to avoid
* linker conflicts when multiple demos are compiled together.
*/

#include <tool.h>
#include <Nfc.h>
#include <ndef_helper.h>

/* Discovery loop configuration according to the targeted modes of operation */
static unsigned char DiscoveryTechnologies[] = {
    MODE_LISTEN | TECH_PASSIVE_NFCA,
    MODE_LISTEN | TECH_PASSIVE_NFCB,
};

void nfc_example_CE(void)
{
    NxpNci_RfIntf_t RfInterface;

    /* Open connection to NXPNCI device */
    if (NxpNci_Connect() == NFC_ERROR) {
        PRINTF("Error: cannot connect to NXPNCI device\n");
        return;
    }

    if (NxpNci_ConfigureSettings() == NFC_ERROR) {
        PRINTF("Error: cannot configure NXPNCI settings\n");
        return;
    }

    if (NxpNci_ConfigureMode(NXPNCI_MODE_RW | NXPNCI_MODE_CARDEMU) == NFC_ERROR)
    {
        PRINTF("Error: cannot configure NXPNCI\n");
        return;
    }

    /* Configure NFCEE_NDEF */
    NFCEE_NDEF_Configuration();
    /* NFCEE_NDEF was configured */

    /* Before starting discovery, let's store NDEF message to NFCEE_NDEF */
    NFCEE_NDEF_DH_Write();
    /* NDEF was stored, let's start discovery now */

    /* Start Discovery */
    if (NxpNci_StartDiscovery(DiscoveryTechnologies,sizeof(DiscoveryTechnologies)) != NFC_SUCCESS)
    {
        PRINTF("Error: cannot start discovery\n");
        return;
    }

    while(1)
    {
        PRINTF("\nWAITING FOR DEVICE DISCOVERY\n");

        /* Wait until a peer is discovered */
        while(NxpNci_WaitForDiscoveryNotification(&RfInterface) != NFC_SUCCESS);

        /* Is activated from remote T4T ? */
        if ((RfInterface.Interface == INTF_ISODEP) && ((RfInterface.ModeTech & MODE_MASK) == MODE_LISTEN))
        {
            PRINTF(" - LISTEN MODE: Activated from remote Reader\n");

            /* Calling this function is needed only for TagInfo */
            NxpNci_ProcessCardMode(RfInterface);

            PRINTF("READER DISCONNECTED\n");
        }
        else
        {
            PRINTF("WRONG DISCOVERY\n");
        }
    }
}
