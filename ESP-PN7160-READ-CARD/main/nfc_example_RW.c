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
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <tool.h>
#include <Nfc.h>

/* Discovery loop configuration according to the targeted modes of operation */
unsigned char DiscoveryTechnologies[] = {
    MODE_POLL | TECH_PASSIVE_NFCA,
    MODE_POLL | TECH_PASSIVE_NFCB,
    MODE_POLL | TECH_PASSIVE_15693
};

void PCD_MIFARE_scenario (void)
{
    #define BLK_NB_MFC      4
    #define KEY_MFC         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	#define DATA_WRITE_MFC  0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff

    bool status;
    unsigned char Resp[256];
    unsigned char RespSize;
    /* Authenticate sector 1 with generic keys */
    unsigned char Auth[] = {0x40, BLK_NB_MFC/4, 0x10, KEY_MFC};
    /* Read block 4 */
    unsigned char Read[] = {0x10, 0x30, BLK_NB_MFC};
    /* Write block 4 */
    unsigned char WritePart1[] = {0x10, 0xA0, BLK_NB_MFC};
    unsigned char WritePart2[] = {0x10, DATA_WRITE_MFC};

    /* Authenticate */
    status = NxpNci_ReaderTagCmd(Auth, sizeof(Auth), Resp, &RespSize);
    if((status == NFC_ERROR) || (Resp[RespSize-1] != 0))
    {
        PRINTF(" Authenticate sector %d failed with error 0x%02x\n", Auth[1], Resp[RespSize-1]);
        return;
    }
    PRINTF(" Authenticate sector %d succeed\n", Auth[1]);

    /* Read block */
    status = NxpNci_ReaderTagCmd(Read, sizeof(Read), Resp, &RespSize);
    if((status == NFC_ERROR) || (Resp[RespSize-1] != 0))
    {
        PRINTF(" Read block %d failed with error 0x%02x\n", Read[2], Resp[RespSize-1]);
        return;
    }
    PRINTF(" Read block %d:", Read[2]); PRINT_BUF(" ", (Resp+1), RespSize-2);

    /* Write block */
    status = NxpNci_ReaderTagCmd(WritePart1, sizeof(WritePart1), Resp, &RespSize);
    if((status == NFC_ERROR) || (Resp[RespSize-1] != 0x14))
    {
        PRINTF(" Write block %d failed with error 0x%02x\n", WritePart1[2], Resp[RespSize-1]);
        return;
    }
    status = NxpNci_ReaderTagCmd(WritePart2, sizeof(WritePart2), Resp, &RespSize);
    if((status == NFC_ERROR) || (Resp[RespSize-1] != 0x14))
    {
        PRINTF(" Write block %d failed with error 0x%02x\n", WritePart1[2], Resp[RespSize-1]);
        return;
    }
    PRINTF(" Block %d written\n", WritePart1[2]);

    /* Read block */
    status = NxpNci_ReaderTagCmd(Read, sizeof(Read), Resp, &RespSize);
    if((status == NFC_ERROR) || (Resp[RespSize-1] != 0))
    {
        PRINTF(" Read failed with error 0x%02x\n", Resp[RespSize-1]);
        return;
    }
    PRINTF(" Read block %d:", Read[2]); PRINT_BUF(" ", (Resp+1), RespSize-2);
}

void PCD_ISO15693_scenario (void)
{
    #define BLK_NB_ISO15693     8
    #define DATA_WRITE_ISO15693 0x11, 0x22, 0x33, 0x44

    bool status;
    unsigned char Resp[256];
    unsigned char RespSize;
    unsigned char ReadBlock[] = {0x02, 0x20, BLK_NB_ISO15693};
    unsigned char WriteBlock[] = {0x02, 0x21, BLK_NB_ISO15693, DATA_WRITE_ISO15693};

    status = NxpNci_ReaderTagCmd(ReadBlock, sizeof(ReadBlock), Resp, &RespSize);
    if((status == NFC_ERROR) || (Resp[RespSize-1] != 0x00))
    {
        PRINTF(" Read block %d failed with error 0x%02x\n", ReadBlock[2], Resp[RespSize-1]);
        return;
    }
    PRINTF(" Read block %d:", ReadBlock[2]); PRINT_BUF(" ", (Resp+1), RespSize-2);

    /* Write */
    status = NxpNci_ReaderTagCmd(WriteBlock, sizeof(WriteBlock), Resp, &RespSize);
    if((status == NFC_ERROR) || (Resp[RespSize-1] != 0))
    {
        PRINTF(" Write block %d failed with error 0x%02x\n", WriteBlock[2], Resp[RespSize-1]);
        return;
    }
    PRINTF(" Block %d written\n", WriteBlock[2]);

    /* Read back */
    status = NxpNci_ReaderTagCmd(ReadBlock, sizeof(ReadBlock), Resp, &RespSize);
    if((status == NFC_ERROR) || (Resp[RespSize-1] != 0x00))
    {
        PRINTF(" Read block %d failed with error 0x%02x\n", ReadBlock[2], Resp[RespSize-1]);
        return;
    }
    PRINTF(" Read block %d:", ReadBlock[2]); PRINT_BUF(" ", (Resp+1), RespSize-2);
}

void PCD_ISO14443_3A_scenario (void)
{
    #define BLK_NB_ISO14443_3A      5
    #define DATA_WRITE_ISO14443_3A  0x11, 0x22, 0x33, 0x44

    bool status;
    unsigned char Resp[256];
    unsigned char RespSize;
    /* Read block */
    unsigned char Read[] = {0x30, BLK_NB_ISO14443_3A};
    /* Write block */
    unsigned char Write[] = {0xA2, BLK_NB_ISO14443_3A, DATA_WRITE_ISO14443_3A};
    
    /* Read */
    status = NxpNci_ReaderTagCmd(Read, sizeof(Read), Resp, &RespSize);
    if((status == NFC_ERROR) || (Resp[RespSize-1] != 0))
    {
        PRINTF(" Read block %d failed with error 0x%02x\n", Read[1], Resp[RespSize-1]);
        return;
    }
    PRINTF(" Read block %d:", Read[1]); PRINT_BUF(" ", Resp, 4);
    /* Write */
    status = NxpNci_ReaderTagCmd(Write, sizeof(Write), Resp, &RespSize);
    if((status == NFC_ERROR) || (Resp[RespSize-1] != 0x14))
    {
        PRINTF(" Write block %d failed with error 0x%02x\n", Write[1], Resp[RespSize-1]);
        return;
    }
    PRINTF(" Block %d written\n", Write[1]);

    /* Read back */
    status = NxpNci_ReaderTagCmd(Read, sizeof(Read), Resp, &RespSize);
    if((status == NFC_ERROR) || (Resp[RespSize-1] != 0))
    {
        PRINTF(" Read block %d failed with error 0x%02x\n", Read[1], Resp[RespSize-1]);
        return;
    }
    PRINTF(" Read block %d:", Read[1]); PRINT_BUF(" ", Resp, 4);
}

void PCD_ISO14443_4_scenario (void)
{
    bool status;
    unsigned char Resp[256];
    unsigned char RespSize;
    unsigned char SelectPPSE[] = {0x00, 0xA4, 0x04, 0x00, 0x0E, 0x32, 0x50, 0x41, 0x59, 0x2E, 0x53, 0x59, 0x53, 0x2E, 0x44, 0x44, 0x46, 0x30, 0x31, 0x00};

    status = NxpNci_ReaderTagCmd(SelectPPSE, sizeof(SelectPPSE), Resp, &RespSize);
    if((status == NFC_ERROR) || (Resp[RespSize-2] != 0x90) || (Resp[RespSize-1] != 0x00))
    {
        PRINTF(" Select PPSE failed with error %02x %02x\n", Resp[RespSize-2], Resp[RespSize-1]);
        return;
    }
    PRINTF(" Select PPSE Application succeed\n");
}

void displayCardInfo(NxpNci_RfIntf_t RfIntf)
{
    switch(RfIntf.Protocol){
    case PROT_T1T:
    case PROT_T2T:
    case PROT_T3T:
    case PROT_ISODEP:
        PRINTF(" - POLL MODE: Remote T%dT activated\n", RfIntf.Protocol);
        break;
    case PROT_T5T:
        PRINTF(" - POLL MODE: Remote ISO15693 card activated\n");
        break;
    case PROT_MIFARE:
        PRINTF(" - POLL MODE: Remote MIFARE card activated\n");
        break;
    default:
        PRINTF(" - POLL MODE: Undetermined target\n");
        return;
    }

    switch(RfIntf.ModeTech) {
    case (MODE_POLL | TECH_PASSIVE_NFCA):
        PRINTF("\tSENS_RES = 0x%.2x 0x%.2x\n", RfIntf.Info.NFC_APP.SensRes[0], RfIntf.Info.NFC_APP.SensRes[1]);
        PRINT_BUF("\tNFCID = ", RfIntf.Info.NFC_APP.NfcId, RfIntf.Info.NFC_APP.NfcIdLen);
        if(RfIntf.Info.NFC_APP.SelResLen != 0) PRINTF("\tSEL_RES = 0x%.2x\n", RfIntf.Info.NFC_APP.SelRes[0]);
    break;

    case (MODE_POLL | TECH_PASSIVE_NFCB):
        if(RfIntf.Info.NFC_BPP.SensResLen != 0) PRINT_BUF("\tSENS_RES = ", RfIntf.Info.NFC_BPP.SensRes, RfIntf.Info.NFC_BPP.SensResLen);
    break;

    case (MODE_POLL | TECH_PASSIVE_NFCF):
        PRINTF("\tBitrate = %s\n", (RfIntf.Info.NFC_FPP.BitRate==1)?"212":"424");
        if(RfIntf.Info.NFC_FPP.SensResLen != 0) PRINT_BUF("\tSENS_RES = ", RfIntf.Info.NFC_FPP.SensRes, RfIntf.Info.NFC_FPP.SensResLen);
    break;

    case (MODE_POLL | TECH_PASSIVE_15693):
        PRINT_BUF("\tID = ", RfIntf.Info.NFC_VPP.ID, sizeof(RfIntf.Info.NFC_VPP.ID));
        PRINTF("\tAFI = 0x%.2x\n", RfIntf.Info.NFC_VPP.AFI);
        PRINTF("\tDSFID = 0x%.2x\n", RfIntf.Info.NFC_VPP.DSFID);
    break;

    default:
        break;
    }
}

void nfc_example(void)
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

    if (NxpNci_ConfigureMode(NXPNCI_MODE_RW) == NFC_ERROR)
    {
        PRINTF("Error: cannot configure NXPNCI\n");
        return;
    }

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

        if ((RfInterface.ModeTech & MODE_MASK) == MODE_POLL)
        {
			/* For each discovered cards */
			while(1){
				/* Display detected card information */
				displayCardInfo(RfInterface);

				/* What's the detected card type ? */
				switch(RfInterface.Protocol) {
				case PROT_T2T:
					PCD_ISO14443_3A_scenario();
					break;
				case PROT_ISODEP:
					PCD_ISO14443_4_scenario();
					break;
				case PROT_T5T:
					PCD_ISO15693_scenario();
					break;
				case PROT_MIFARE:
					PCD_MIFARE_scenario();
					break;
				default:
					break;
				}

				/* If more cards (or multi-protocol card) were discovered (only same technology are supported) select next one */
				if(RfInterface.MoreTags) {
					if(NxpNci_ReaderActivateNext(&RfInterface) == NFC_ERROR) break;
				}
				/* Otherwise leave */
				else break;
			}

			/* Wait for card removal */
			NxpNci_ProcessReaderMode(RfInterface, PRESENCE_CHECK);

		    PRINTF("CARD REMOVED\n");

			/* Restart discovery loop */
			NxpNci_StopDiscovery();
			while(NxpNci_StartDiscovery(DiscoveryTechnologies,sizeof(DiscoveryTechnologies)));
		}
        else
        {
            PRINTF("WRONG DISCOVERY\n");
        }
	}
}
