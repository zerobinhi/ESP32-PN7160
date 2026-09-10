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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <tool.h>
#include <Nfc.h>
#include <ndef_helper.h>

/* Discovery loop configuration according to the targeted modes of operation */
static unsigned char DiscoveryTechnologies[] = {
    MODE_POLL | TECH_PASSIVE_NFCA,
    MODE_POLL | TECH_PASSIVE_NFCF,
    MODE_POLL | TECH_ACTIVE_NFC,
    MODE_LISTEN | TECH_PASSIVE_NFCA,
    MODE_LISTEN | TECH_PASSIVE_NFCF,
    MODE_LISTEN | TECH_ACTIVE_NFC,
};

static unsigned char gNdefBuffer[100];
static unsigned short gPendingNdefMsgReception = 0;

static void NdefPull_Cb(unsigned char *pNdefMessage, unsigned short ReceivedSize, unsigned int NdefMessageSize)
{
    unsigned char *pNdefRecord = pNdefMessage;
    NdefRecord_t NdefRecord;
    unsigned char save;

    if (pNdefMessage == NULL)
    {
        PRINTF("--- Provisioned buffer size too small or NDEF message empty \n");
        return;
    }

    if (gPendingNdefMsgReception)
    {
        memcpy(&gNdefBuffer[NdefMessageSize - gPendingNdefMsgReception], pNdefMessage, ReceivedSize);
        gPendingNdefMsgReception -= ReceivedSize;
        if (gPendingNdefMsgReception > 0)
        {
            PRINTF("--- Receiving Ndef fragmented message, pending reception of %d bytes\n", gPendingNdefMsgReception);
            return;
        }
        else
        {
            PRINTF("--- Received complete Ndef fragmented message\n");
            pNdefRecord = gNdefBuffer;
        }
    }
    else if(NdefMessageSize > ReceivedSize)
    {
        if (sizeof(gNdefBuffer) < NdefMessageSize)
        {
            PRINTF("--- Provisioned buffer size too small \n");
            return;
        }
        memcpy(gNdefBuffer, pNdefMessage, ReceivedSize);
        gPendingNdefMsgReception = NdefMessageSize - ReceivedSize;
        PRINTF("--- Receiving Ndef fragmented message (size = %d bytes), pending reception of %d bytes\n", NdefMessageSize, gPendingNdefMsgReception);
        return;
    }

    while (pNdefRecord != NULL)
    {
        NdefRecord = DetectNdefRecordType(pNdefRecord);

        PRINTF("--- NDEF record received:\n");

        switch(NdefRecord.recordType)
        {
        case MEDIA_VCARD:
            {
                save = NdefRecord.recordPayload[NdefRecord.recordPayloadSize];
                NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = '\0';
                PRINTF("   vCard:\n%s\n", NdefRecord.recordPayload);
                NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = save;
            }
            break;

        case WELL_KNOWN_SIMPLE_TEXT:
            {
                save = NdefRecord.recordPayload[NdefRecord.recordPayloadSize];
                NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = '\0';
                PRINTF("   Text record: %s\n", &NdefRecord.recordPayload[NdefRecord.recordPayload[0]+1]);
                NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = save;
            }
            break;

        case WELL_KNOWN_SIMPLE_URI:
            {
                save = NdefRecord.recordPayload[NdefRecord.recordPayloadSize];
                NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = '\0';
                PRINTF("   URI record: %s%s\n", ndef_helper_UriHead(NdefRecord.recordPayload[0]), &NdefRecord.recordPayload[1]);
                NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = save;
            }
            break;

        case MEDIA_HANDOVER_WIFI:
            {
                unsigned char index = 0, i;

                PRINTF ("--- Received WIFI credentials:\n");
                if ((NdefRecord.recordPayload[index] == 0x10) && (NdefRecord.recordPayload[index+1] == 0x0e)) index+= 4;
                while(index < NdefRecord.recordPayloadSize)
                {
                    if (NdefRecord.recordPayload[index] == 0x10)
                    {
                        if (NdefRecord.recordPayload[index+1] == 0x45) {PRINTF ("- SSID = "); for(i=0;i<NdefRecord.recordPayload[index+3];i++) PRINTF("%c", NdefRecord.recordPayload[index+4+i]); PRINTF ("\n");}
                        else if (NdefRecord.recordPayload[index+1] == 0x03) PRINTF ("- Authenticate Type = %s\n", ndef_helper_WifiAuth(NdefRecord.recordPayload[index+5]));
                        else if (NdefRecord.recordPayload[index+1] == 0x0f) PRINTF ("- Encryption Type = %s\n", ndef_helper_WifiEnc(NdefRecord.recordPayload[index+5]));
                        else if (NdefRecord.recordPayload[index+1] == 0x27) {PRINTF ("- Network key = "); for(i=0;i<NdefRecord.recordPayload[index+3];i++) PRINTF("#"); PRINTF ("\n");}
                        index += 4 + NdefRecord.recordPayload[index+3];
                    }
                    else continue;
                }
            }
            break;

        case WELL_KNOWN_HANDOVER_SELECT:
            PRINTF("   Handover select version %d.%d\n", NdefRecord.recordPayload[0] >> 4, NdefRecord.recordPayload[0] & 0xF);
            break;

        case WELL_KNOWN_HANDOVER_REQUEST:
            PRINTF("   Handover request version %d.%d\n", NdefRecord.recordPayload[0] >> 4, NdefRecord.recordPayload[0] & 0xF);
            break;

        case MEDIA_HANDOVER_BT:
            PRINT_BUF("   BT Handover payload = ", NdefRecord.recordPayload, NdefRecord.recordPayloadSize);
            break;

        case MEDIA_HANDOVER_BLE:
            PRINT_BUF("   BLE Handover payload = ", NdefRecord.recordPayload, NdefRecord.recordPayloadSize);
            break;

        case MEDIA_HANDOVER_BLE_SECURE:
            PRINT_BUF("   BLE secure Handover payload = ", NdefRecord.recordPayload, NdefRecord.recordPayloadSize);
            break;

        default:
            PRINTF("   Unsupported NDEF record, cannot parse\n");
            break;
        }
        pNdefRecord = GetNextRecord(pNdefRecord);
    }

    PRINTF("\n");
}

static const char NDEF_MESSAGE[14] = {
    0xC1,                /* MB/ME/CF/1/IL/TNF */
    0x01,                /* TYPE LENGTH */
    0x07 >> 24,          /* PAYLOAD LENTGH MSB */
    0x07 >> 16,          /* PAYLOAD LENTGH */
    0x07 >> 8,           /* PAYLOAD LENTGH */
    0x07 & 0xFF,         /* PAYLOAD LENTGH LSB */
    'T',                 /* TYPE */
                         /* PAYLOAD */
    0x02,                /* Status */
    'e', 'n',            /* Language length */
    'T', 'e', 's', 't' };

static void NdefPush_Cb(unsigned char *pNdefRecord, unsigned short NdefRecordSize) {
    PRINTF("--- NDEF Record sent\n\n");
}

void nfc_example_P2P(void)
{
    NxpNci_RfIntf_t RfInterface;

    /* Register NDEF message to be sent to remote peer */
    P2P_NDEF_SetMessage((unsigned char *) NDEF_MESSAGE, sizeof(NDEF_MESSAGE), *NdefPush_Cb);
    /* Register callback for reception of NDEF message from remote peer */
    P2P_NDEF_RegisterPullCallback(*NdefPull_Cb);

    /* Open connection to NXPNCI device */
    if (NxpNci_Connect() == NFC_ERROR) {
        PRINTF("Error: cannot connect to NXPNCI device\n");
        return;
    }

    if (NxpNci_ConfigureSettings() == NFC_ERROR) {
        PRINTF("Error: cannot configure NXPNCI settings\n");
        return;
    }

    if (NxpNci_ConfigureMode(NXPNCI_MODE_P2P) == NFC_ERROR)
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

        /* Reset reception index */
        gPendingNdefMsgReception = false;

        /* Wait until a peer is discovered */
        while(NxpNci_WaitForDiscoveryNotification(&RfInterface) != NFC_SUCCESS);

        /* Is P2P discovery ? */
        if (RfInterface.Interface == INTF_NFCDEP)
        {
            if ((RfInterface.ModeTech & MODE_LISTEN) == MODE_LISTEN) PRINTF(" - P2P TARGET MODE: Activated from remote Initiator\n");
            else PRINTF(" - P2P INITIATOR MODE: Remote Target activated\n");

            /* Process with SNEP for NDEF exchange */
            NxpNci_ProcessP2pMode(RfInterface);

            PRINTF("PEER LOST\n");
        }
        else
        {
            if ((RfInterface.ModeTech & MODE_MASK) == MODE_POLL)
            {
                /* Restart discovery loop */
                NxpNci_StopDiscovery();
                while(NxpNci_StartDiscovery(DiscoveryTechnologies,sizeof(DiscoveryTechnologies)));
            }

            PRINTF("WRONG DISCOVERY\n");
        }
    }
}
