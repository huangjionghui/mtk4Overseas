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
 *   mtk40027
 *
 * Last changed:
 * ------------- 
 * $Author: bin.xu $ 
 *
 * $Modtime: $  
 
 * $Revision: #2 $
****************************************************************************/


#ifndef _BZS_SMS_H_
#define _BZS_SMS_H_
//
// BlueZone SMSMGR Profile - Structure definition
//

#include "BZS_Comm.h"
#define _IN
#define _OUT
#define _OPTIONAL

#define SML_PARSER_INST_FILE_NAME L"SmlParserInstance.txt"
#define SML_SEND_INST_FILE_NAME   L"SmlSendCmdInstance.txt"
#define SMS_DATA_FILE_NAME        L"SMSData"
#define SMS_REF_DATA_FILE_NAME    L"SMSRefData"

typedef enum _tag_BT_SMSMGR_SYNC_PATH
{
    BT_SMSMGR_SYNC_PATH_ME,
    BT_SMSMGR_SYNC_PATH_SM,
    BT_SMSMGR_SYNC_PATH_ALL,
    NUM_OF_BT_SMSMGR_SYNC_PATH,
} E_BT_SMSMGR_SYNC_PATH_T;

typedef enum _tag_SMSMGR_SYNC_STATE_T
{
    SMSMGR_SYNC_STATE_IDLE = 0,
    SMSMGR_SYNC_STATE_PREPAREED,
    SMSMGR_SYNC_STATE_SYNCING,
    SMSMGR_SYNC_STATE_STOPING,
} E_SMSMGR_SYNC_STATE_T;

typedef struct _tag_SMSMGR_SYNC_RESULT_PARAM_T{
    E_BT_SMSMGR_SYNC_PATH_T ePath;
    UINT32 u4RecCnt;
} SMSMGR_SYNC_RESULT_PARAM_T, *P_SMSMGR_SYNC_RESULT_PARAM_T;

typedef enum btsmstype
{
    BT_SMS_REC_TYPE_UNREAD,
    BT_SMS_REC_TYPE_READED,
    BT_SMS_REC_TYPE_DRAFT,
    BT_SMS_REC_TYPE_SENDED,
    BT_SMS_REC_TYPE_ALL,
    MAX_OF_BT_SMS_REC_TYPE_CNT
} E_BT_SMS_REC_TYPE_T;

typedef struct _tag_SMSMGRRECORD
{
    struct _tag_SMSMGRRECORD *next;
    unsigned short *wszTime;
    unsigned short *wszPhone;
    unsigned short *wszRec;
    E_BT_SMS_REC_TYPE_T eSMSRectype;
    E_BT_SMSMGR_SYNC_PATH_T ePath;
} SMSMGRRECORD_T, *P_SMSMGRRECORD_T;

typedef enum btsmsmgrconnecttype
{
    BT_SMS_CONNECT_UNKNOWN,
    BT_SMS_CONNECT_HFP,
    BT_SMS_CONNECT_SPP,
    BT_SMS_CONNECT_DUN,
    BT_SMS_CONNECT_SYNCML,
    BT_SMS_CONNECT_FTP,
    BT_SMS_CONNECT_MAP,
    MAX_OF_BT_SMS_CONNECT_CNT
}E_BT_SMS_CONNECT_TYPE_T;

typedef enum _tag_SMSMGR_SYNC_RESULT_T
{
    BT_SMSMGR_SYNC_RST_SUCCESS,
    BT_SMSMGR_SYNC_RST_FAIL,
    BT_SMSMGR_SYNC_RST_TERMINATE,
    BT_SMSMGR_SYNC_RST_NOTSUPPORT,
    BT_SMSMGR_SYNC_RST_FILEERR,
    BT_SMSMGR_SYNC_RST_UNKNOWN,
    NUM_OF_BT_SMSMGR_SYNC_RST
} E_BT_SMSMGR_SYNC_RESULT_T;

typedef struct _tag_SMSMGR_IND_PARAM_T
{
    DWORD dwPath;
    DWORD dwCurRecCnt;
} SMSMGR_IND_PARAM_T, *P_SMSMGR_IND_PARAM_T;


#endif //_BZS_SMS_H_

