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
 
 * $Revision: #7 $
****************************************************************************/


#ifndef _BZS_PBMGR_H_
#define _BZS_PBMGR_H_
//
// BlueZone PBMGR Profile - Structure definition
//

#include "BZS_Comm.h"
#define _IN
#define _OUT
#define _OPTIONAL

#define PBMGR_UPDATE_LOCAL_CALLHISTORY_SELF 1 //打电话后在线维护，不重新下载通话记录
// 断点续传
#define PBMGR_ENABLE_PAUSE_SYNCING 	1
#define PBMGR_MAX_PATH_LEN 256
#define FAVORITE_MAX_CHAR_LEN 50

#define CONTACT_NAME_TEL_REF_FILE           L"PBNameTelRef"
#define CONTACT_NAME_TEL_FILE               L"PBNameTel"
#define PHONESELF_CONTACTS_FILE_NAME        L"PBPhone"
#define SIM_CONTACTS_FILE_NAME              L"PBSIM"
#define INCOMING_HIS_FILE_NAME              L"Incoming"
#define OUTGOING_HIS_FILE_NAME              L"Outgoing"
#define MISSED_HIS_FILE_NAME                L"Missed"
#define PHONESELF_CONTACTS_REF_FILE_NAME    L"PBPhoneRef"
#define SIM_CONTACTS_REF_FILE_NAME          L"PBSIMRef"
#define INCOMING_HIS_REF_FILE_NAME          L"IncomingRef"
#define OUTGOING_HIS_REF_FILE_NAME          L"OutgoingRef"
#define MISSED_HIS_REF_FILE_NAME            L"MissedRef"
#define PBAP_VCARD_FILE_NAME                L"PBAPVCardNameList"
#define FAVORITE_FILE_NAME                  L"PBFavorite"

#define MAX_CNT_OF_CONHIS_FILE_TYPE         5

typedef enum _tag_BT_PBAP_SYNC_PATH
{
    BT_PBAP_SYNC_PATH_PHONESELF_PHONEBOOK = 0,
    BT_PBAP_SYNC_PATH_SIM_PHONEBOOK,
    BT_PBAP_SYNC_PATH_ALL_PHONEBOOK,
    BT_PBAP_SYNC_PATH_RECEIVE_CALLHISTORY,
    BT_PBAP_SYNC_PATH_DIALED_CALLHISTORY,
    BT_PBAP_SYNC_PATH_MISSED_CALLHISTORY,
    BT_PBAP_SYNC_PATH_ALL_CALLHISTORY,
    BT_PBAP_SYNC_PATH_ALL_PHONEBOOK_CALLHISTORY,
    NUM_OF_BT_PBAP_SYNC_PATH,
} E_BT_PBAP_SYNC_PATH_T;

typedef enum _tag_BT_TEL_NUMBER_TYPE
{
    BT_TEL_NUMBER_TYPE_HOME = 0,
    BT_TEL_NUMBER_TYPE_WORK,
    BT_TEL_NUMBER_TYPE_MOBILE,
    BT_TEL_NUMBER_TYPE_FAX,
    BT_TEL_NUMBER_TYPE_OTHER,
    BT_TEL_NUMBER_TYPE_UNKNOWN
} E_BT_TEL_NUMBER_TYPE;

typedef enum _tag_PBMGR_SYNC_STATE_T
{
    PBMGR_SYNC_STATE_IDLE = 0,
    PBMGR_SYNC_STATE_GETCINDVAL,
    PBMGR_SYNC_STATE_PREPAREED,
    PBMGR_SYNC_STATE_SYNCING,
    PBMGR_SYNC_STATE_STOPING,
} E_PBMGR_SYNC_STATE_T;

typedef struct _tag_PBMGR_SYNC_RESULT_PARAM_T{
    E_BT_PBAP_SYNC_PATH_T ePath;
    UINT32 u4RecCnt;
    DWORD u4TotleCnt;
} PBMGR_SYNC_RESULT_PARAM_T, *P_PBMGR_SYNC_RESULT_PARAM_T;

typedef struct _tag_PBMGRRECORD
{
    struct _tag_PBMGRRECORD *next;
    unsigned short *pswName; 
    unsigned short *pswHandle; 
    unsigned short *pswNumber; 
    unsigned short *pswTime;
    E_BT_TEL_NUMBER_TYPE  eTelType;
    E_BT_PBAP_SYNC_PATH_T ePath;
} PBMGRRECORD_T, *P_PBMGRRECORD_T;

typedef enum PBMGR_TYPE_CONNECT_tag{
    PBMGR_UNKNOW_CONNECT,
    PBMGR_HFP_CONNECT,
    PBMGR_SPP_CONNECT,
    PBMGR_DUN_CONNECT,
    PBMGR_PBAP_CONNECT,
    PBMGR_SYNCML_CONNECT,
    MAX_OF_PBMGR_CONNECT_CNT
} PBMGR_TYPE_CONNECT;

typedef struct
{
    wchar_t szName[FAVORITE_MAX_CHAR_LEN];
    wchar_t szNumber[FAVORITE_MAX_CHAR_LEN];
}FAVORITE_INFO, *P_FAVORITE_INFO;

#endif
