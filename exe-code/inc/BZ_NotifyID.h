 /*****************************************************************************
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by copyright and the information contained
 *  herein is confidential. The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of MediaTek Inc.   2008 MediaTek Inc.
 *
 *  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 *  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
 *  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 *  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 *  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 *  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
 *  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
 *  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
 *  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 *  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 *  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
 *  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
 *  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
 *  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
 *  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
 *  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
 *
 *****************************************************************************/ 

/*****************************************************************************
 *
 * szFileName:
 * ---------
 *   $Workfile:  $ 
 *
 * Project:
 * --------
 *   MTKBluetooth
 *
 * Description:
 * ------------
 *   
 *   
 * Author:
 * -------
 *   mtk40144
 *
 * Last changed:
 * ------------- 
 * $Author: xiaozhou.huang $ 
 *
 * $Modtime: $  
 
 * $Revision: #5 $
****************************************************************************/


#ifndef _BZI_NOTIFYID_H_
#define _BZI_NOTIFYID_H_

#include "BZ_IOCTL.h"

typedef enum _tag_BTCALLBACK_MODULE
{
    BZ_MODULE_OCBT=1,
    BZ_MODULE_PAIRING,
    BZ_MODULE_CONNECTION,
    BZ_MODULE_PAIREDREC,
    BZ_MODULE_CALL,
    BZ_MODULE_MUSIC,
    BZ_MODULE_SPP,
    BZ_MODULE_DUN,
    BZ_MODULE_PBMGR,
    BZ_MODULE_SIGNAL_BATTERY,
    BZ_MODULE_FTP,
    BZ_MODULE_SMS,
    BZ_MODULE_SEARCH,
    MAX_MODULE_NUM
} E_BTEVTCALLBACK_MODULE_T;



#define BZIND_CODE( module, idx )               BZCTL_CODE( 1, module, idx )
#define BZ_MODULETYPE(dwindid)                  ((dwindid & 0x7f000000) >> 24)

//Open/Close Bluetooth Indication
#define BZ_IND_OPEN_BLUETOOTH_SUCCESS           BZIND_CODE( BZ_MODULE_OCBT, 1 )        
#define BZ_IND_OPEN_BLUETOOTH_FAIL              BZIND_CODE( BZ_MODULE_OCBT, 2 )
#define BZ_IND_ClOSE_BLUETOOTH_SUCCESS          BZIND_CODE( BZ_MODULE_OCBT, 3 )
#define BZ_IND_ClOSE_BLUETOOTH_FAIL             BZIND_CODE( BZ_MODULE_OCBT, 4 )
#define BZ_IND_AUTO_CONNECT                     BZIND_CODE( BZ_MODULE_OCBT, 5 )

// Indication related to Paring
#define BZ_IND_SETPIN_SUCCESS                   BZIND_CODE( BZ_MODULE_PAIRING, 1 )
#define BZ_IND_SETPIN_FAIL                      BZIND_CODE( BZ_MODULE_PAIRING, 2 )  
#define BZ_IND_PAIR_SUCCESS                     BZIND_CODE( BZ_MODULE_PAIRING, 3 )
#define BZ_IND_PAIR_FAIL                        BZIND_CODE( BZ_MODULE_PAIRING, 4 )  


#define BZ_IND_SET_LOCALDEVNAME_SUCCESS         BZIND_CODE( BZ_MODULE_PAIRING, 5 )   
#define BZ_IND_SET_LOCALDEVNAME_FAIL            BZIND_CODE( BZ_MODULE_PAIRING, 6 ) 
#define BZ_IND_START_PAIRING_SUCCESS            BZIND_CODE( BZ_MODULE_PAIRING, 7 ) 
#define BZ_IND_START_PAIRING_FAIL               BZIND_CODE( BZ_MODULE_PAIRING, 8 ) 

// Indication related to Connection/Disconnection
#define BZ_IND_CREATE_CONNECT_SUCCESS           BZIND_CODE( BZ_MODULE_CONNECTION, 1 ) 
#define BZ_IND_CREATE_CONNECT_FAIL              BZIND_CODE( BZ_MODULE_CONNECTION, 2 ) 
#define BZ_IND_DISCONNECT_SUCCESS               BZIND_CODE( BZ_MODULE_CONNECTION, 3 ) 
#define BZ_IND_DISCONNECT_FAIL                  BZIND_CODE( BZ_MODULE_CONNECTION, 4 ) 
#define BZ_IND_SET_PROTOCOL_COMPLETE            BZIND_CODE( BZ_MODULE_CONNECTION, 5 )
#define BZ_IND_CONNECT_STATUS_CHANGED           BZIND_CODE( BZ_MODULE_CONNECTION, 6 )

// Indication related to the Paired Device Records list
#define BZ_IND_DELELTE_PAIREDREC_SUCCESS        BZIND_CODE( BZ_MODULE_PAIREDREC, 1 ) 
#define BZ_IND_DELELTE_PAIREDREC_FAIL           BZIND_CODE( BZ_MODULE_PAIREDREC, 2 ) 
#define BZ_IND_CLEAR_PAIREDRECS_SUCCESS         BZIND_CODE( BZ_MODULE_PAIREDREC, 3 ) 
#define BZ_IND_CLEAR_PAIREDRECS_FAIL            BZIND_CODE( BZ_MODULE_PAIREDREC, 4 ) 

// Indication related to Call
#define BZ_IND_CALL_OUTGOING                    BZIND_CODE( BZ_MODULE_CALL, 1 ) 
#define BZ_IND_CALL_INCOMING                    BZIND_CODE( BZ_MODULE_CALL, 2 )
#define BZ_IND_CALL_SPEAKING                    BZIND_CODE( BZ_MODULE_CALL, 3 )
#define BZ_IND_CALL_TERMINATED                  BZIND_CODE( BZ_MODULE_CALL, 4 )
#define BZ_IND_AUDIO_OUTPUT_INPHONE             BZIND_CODE( BZ_MODULE_CALL, 5 )
#define BZ_IND_AUDIO_OUTPUT_INLOCALDEV          BZIND_CODE( BZ_MODULE_CALL, 6 )
#define BZ_IND_DIAL_CALL_FAIL                   BZIND_CODE( BZ_MODULE_CALL, 7 )
#define BZ_IND_REDIAL_CALL_FAIL                 BZIND_CODE( BZ_MODULE_CALL, 8 )
#define BZ_IND_REJECT_CALL_FAIL                 BZIND_CODE( BZ_MODULE_CALL, 9 )
#define BZ_IND_TERMINATE_CALL_FAIL              BZIND_CODE( BZ_MODULE_CALL, 10 )
#define BZ_IND_ANSWER_CALL_FAIL                 BZIND_CODE( BZ_MODULE_CALL, 11 )
#define BZ_IND_ACCEPT_CALL                      BZIND_CODE( BZ_MODULE_CALL, 12 )
#define BZ_IND_HEADSET_AG_CONNECTED             BZIND_CODE( BZ_MODULE_CALL, 30 )
#define BZ_IND_HEADSET_AG_DISCONNECTED          BZIND_CODE( BZ_MODULE_CALL, 31 )
#define BZ_IND_HEADSET_AG_AUDIO_ON              BZIND_CODE( BZ_MODULE_CALL, 32 )
#define BZ_IND_HEADSET_AG_AUDIO_OFF             BZIND_CODE( BZ_MODULE_CALL, 33 )
#define BZ_IND_HEADSET_AG_BUTTON_PRESSED        BZIND_CODE( BZ_MODULE_CALL, 34 )
#define BZ_IND_HEADSET_HS_CONNECTED             BZIND_CODE( BZ_MODULE_CALL, 35 )
#define BZ_IND_HEADSET_HS_DISCONNECTED          BZIND_CODE( BZ_MODULE_CALL, 36 )
#define BZ_IND_HEADSET_HS_AUDIO_ON              BZIND_CODE( BZ_MODULE_CALL, 37 )
#define BZ_IND_HEADSET_HS_AUDIO_OFF             BZIND_CODE( BZ_MODULE_CALL, 38 )
#define BZ_IND_HEADSET_HS_RING                  BZIND_CODE( BZ_MODULE_CALL, 39 )

// Indication related to Music
#define BZ_IND_A2DP_CONNECT_SUCCESS             BZIND_CODE( BZ_MODULE_MUSIC, 1 )
#define BZ_IND_A2DP_CONNECT_FAIL                BZIND_CODE( BZ_MODULE_MUSIC, 2 )
#define BZ_IND_A2DP_DISCONNECT_SUCCESS          BZIND_CODE( BZ_MODULE_MUSIC, 3 )
#define BZ_IND_A2DP_DISCONNECT_FAIL             BZIND_CODE( BZ_MODULE_MUSIC, 4 )
#define BZ_IND_AVRCP_CONNECT_SUCCESS            BZIND_CODE( BZ_MODULE_MUSIC, 5 )
#define BZ_IND_AVRCP_CONNECT_FAIL               BZIND_CODE( BZ_MODULE_MUSIC, 6 )
#define BZ_IND_AVRCP_DISCONNECT_SUCCESS         BZIND_CODE( BZ_MODULE_MUSIC, 7 )
#define BZ_IND_AVRCP_DISCONNECT_FAIL            BZIND_CODE( BZ_MODULE_MUSIC, 8 )
#define BZ_IND_MUSIC_STOP                       BZIND_CODE( BZ_MODULE_MUSIC, 9 )
#define BZ_IND_MUSIC_PLAYING                    BZIND_CODE( BZ_MODULE_MUSIC, 10 )
#define BZ_IND_MUSIC_QUERY_PLAY                 BZIND_CODE( BZ_MODULE_MUSIC, 11 )


//SPP
#define BZ_IND_SPP_CONNECTED                    BZIND_CODE( BZ_MODULE_SPP, 1 )
#define BZ_IND_SPP_DISCONNECTED                 BZIND_CODE( BZ_MODULE_SPP, 2 )

//DUN
#define BZ_IND_DUN_CONNECTED                    BZIND_CODE( BZ_MODULE_DUN, 1 )
#define BZ_IND_DUN_DISCONNECTED                 BZIND_CODE( BZ_MODULE_DUN, 2 )
#define BZ_IND_DUN_DISCONNECTING                BZIND_CODE( BZ_MODULE_DUN, 3 )


// Indication related to the PBAP
#define BZ_IND_PBAP_NOT_SUPPORT                 BZIND_CODE( BZ_MODULE_PBMGR, 1 )
#define BZ_IND_PBAP_SYNC_COMPLETE               BZIND_CODE( BZ_MODULE_PBMGR, 2 )
#define BZ_IND_PBAP_SYNC_FAIL                   BZIND_CODE( BZ_MODULE_PBMGR, 3 )    
#define BZ_IND_PBAP_PBRECORDS                   BZIND_CODE( BZ_MODULE_PBMGR, 4 )
#define BZ_IND_PBAP_TIMEOUT                     BZIND_CODE( BZ_MODULE_PBMGR, 5 )
#define BZ_IND_PBAP_FILLERR                     BZIND_CODE( BZ_MODULE_PBMGR, 6 )

// Indication related to the Signal and Battery
#define BZ_IND_SIGNAL_LEVEL_CHANGED             BZIND_CODE( BZ_MODULE_SIGNAL_BATTERY, 1 )
#define BZ_IND_BATTERY_LEVEL_CHANGED            BZIND_CODE( BZ_MODULE_SIGNAL_BATTERY, 2 )

// Indication related to the SMS
#define BZ_IND_SMS_NOT_SUPPORT                  BZIND_CODE( BZ_MODULE_SMS, 1 )
#define BZ_IND_SMS_SYNC_COMPLETE                BZIND_CODE( BZ_MODULE_SMS, 2 )
#define BZ_IND_SMS_SYNC_FAIL                    BZIND_CODE( BZ_MODULE_SMS, 3 )    
#define BZ_IND_SMS_SMSRECORDS                   BZIND_CODE( BZ_MODULE_SMS, 4 )
#define BZ_IND_SMS_FILEERROR                    BZIND_CODE( BZ_MODULE_SMS, 5 )
#define BZ_IND_SMS_TIMEOUT                      BZIND_CODE( BZ_MODULE_SMS, 6 )

// Indication related to SEARCH
#define BZ_IND_DEVICESEARCH_SUCCESS             BZIND_CODE( BZ_MODULE_SEARCH, 1 )
#define BZ_IND_DEVICESEARCH_FINISHED            BZIND_CODE( BZ_MODULE_SEARCH, 2 )
#define BZ_IND_QUERYSERVICES_FINISH            BZIND_CODE( BZ_MODULE_SEARCH, 3 )

#define BZ_BLUZEON_SHELL_MASK              (0x1)
#define BZ_BLUZEON_MIN_MASK                (0x7)
#define BZ_BLUZEON_DEFAULT_MASK            (0x7)
#define BZ_BLUZEON_ALL_MASK                (0xFFFFFFFF)
#define BZ_SDP_LOC_MASK                    (0x1 << 1)
#define BZ_SDP_REM_MASK                    (0x1 << 2)
#define BZ_SDP_MASK                        (BZ_SDP_LOC_MASK | BZ_SDP_REM_MASK)
#define BZ_HSP_HS_MASK                     (0x1 << 3)
#define BZ_HSP_AG_MASK                     (0x1 << 4)
#define BZ_HSP_MASK                        (BZ_HSP_HS_MASK | BZ_HSP_AG_MASK)
#define BZ_HFP_HF_MASK                     (0x1 << 5)
#define BZ_HFP_AG_MASK                     (0x1 << 6)
#define BZ_HFP_MASK                        (BZ_HFP_HF_MASK|BZ_HFP_AG_MASK)
#define BZ_A2DP_SINK_MASK                  (0x1 << 7)
#define BZ_A2DP_SRC_MASK                   (0x1 << 8)
#define BZ_A2DP_MASK                       (BZ_A2DP_SINK_MASK|BZ_A2DP_SRC_MASK)
#define BZ_SPP_A_MASK                      (0x1 << 9)
#define BZ_SPP_B_MASK                      (0x1 << 10)
#define BZ_SPP_MASK                        (BZ_SPP_A_MASK|BZ_SPP_B_MASK)
#define BZ_DUN_DT_MASK                     (0x1 << 11)
#define BZ_DUN_GW_MASK                     (0x1 << 12)
#define BZ_DUN_MASK                        (BZ_DUN_DT_MASK|BZ_DUN_GW_MASK)
#define BZ_OBEX_CLI_MASK                   (0x1 << 13)
#define BZ_OBEX_SRV_MASK                   (0x1 << 14)
#define BZ_OBEX_MASK                       (BZ_OBEX_CLI_MASK|BZ_OBEX_SRV_MASK)
#define BZ_PBAP_CLI_MASK                   (0x1 << 15)
#define BZ_PBAP_SRV_MASK                   (0x1 << 16)
#define BZ_PBAP_MASK                       (BZ_PBAP_CLI_MASK|BZ_PBAP_SRV_MASK)
#define BZ_FTP_CLI_MASK                    (0x1 << 17)
#define BZ_FTP_SRV_MASK                    (0x1 << 18)
#define BZ_FTP_MASK                        (BZ_FTP_CLI_MASK|BZ_FTP_SRV_MASK)
#define BZ_SYNCML_CLI_MASK                 (0x1 << 19)
#define BZ_SYNCML_SRV_MASK                 (0x1 << 20)
#define BZ_SYNCML_MASK                     (BZ_SYNCML_CLI_MASK|BZ_SYNCML_SRV_MASK)
#define BZ_AVRCP_CT_MASK                   (0x1 << 21)
#define BZ_AVRCP_TG_MASK                   (0x1 << 22)
#define BZ_AVRCP_MASK                      (BZ_AVRCP_CT_MASK|BZ_AVRCP_TG_MASK)

#define MAX_OF_BZ_VALID_SERVICE_MASK       22
#define MIN_OF_BZ_VALID_SERVICE_MASK       3

#define BZ_DEFAULT_SERVICE_MASK            (BZ_HSP_MASK        | \
                                            BZ_HFP_AG_MASK     | \
                                            BZ_A2DP_SRC_MASK   | \
                                            BZ_AVRCP_TG_MASK   | \
                                            BZ_SPP_A_MASK      | \
                                            BZ_DUN_DT_MASK     | \
                                            BZ_SYNCML_CLI_MASK | \
                                            BZ_PBAP_SRV_MASK)

// Indication related to FTP

// Indication related to DUNl

#endif /* _BZI_NOTIFYID_H_ */

