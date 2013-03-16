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
 *
 * Modify by jian.wang (mtk40390)
 
 * $Revision: #3 $
****************************************************************************/

#ifndef _BZS_CALL_H_
#define _BZS_CALL_H_
//
// BlueZone HFP Profile - Structure definition
//

#include "BZS_Comm.h"

#include "BZS_Connect.h"




#define _IN
#define _OUT
#define _OPTIONAL

#define MAX_LEN_PHONE_NUMBER        64
#define MAX_LEN_DTMF_STRING            16
#define MAX_LEN_OPERATOR_NAME        16
#define MAX_NUM_SUBSCRIBER_NUMBER    3
#define MAX_NUM_CALL_LIST            3
#define MAX_NUM_INDICATOR            20
#define MAX_LEN_AT_CMD                64
#define MAX_LEN_PHONEREC_NAME       128
#define MAX_NUM_PHONE               2
#define TEL_NAME_UNKNOWN            L"UNKNOWN"
#define DEFAULT_AUTO_ANSWER_CALL_MILLSECS    5000
#define DEFAULT_INCONING_CALL_MILLSECS    6000





typedef enum {
    BT_SERVICE_TYPE_VIOCE = 4,
    BT_SERVICE_TYPE_FAX
} E_BT_SERVICETYPE_T;

typedef enum {
    BT_CALL_DIR_OUTGOING = 0,
    BT_CALL_DIR_INCOMING,
    BT_CALL_DIR_UNKNOWN,
} E_BT_CALLDIRECTION_T;

typedef enum _BTCPASVALUE
{
    BT_CPAS_STATUS_IDLE = 0,
    BT_CPAS_STATUS_ALERMING = 3,
    BT_CPAS_STATUS_CALLACTIVE = 4,

} E_BT_CPAS_STATUS_T;

typedef enum _tag_enumBTCallStatus
{
    BT_CALL_STATUS_ACTIVE = 0,
    BT_CALL_STATUS_HELD,           // one call in in helding
    BT_CALL_STATUS_DIALING,        // is in dialing: outgoing call is in setup
    BT_CALL_STATUS_ALERTING,       // is in alerting: outgoing calll  is in alerting
    BT_CALL_STATUS_INCOMING,       // is in incoming: incoming call is in setup
    BT_CALL_STATUS_WAITING,        // one call is in waiting
    BT_CALL_STATUS_IDLE ,          // No calls
    NUM_OF_BT_CALL_STATUS
} E_BT_CALL_STATUS_T;

typedef enum
{
    CALLMANAGER_CALLSTATE_IDLE,
    CALLMANAGER_CALLSTATE_INCOMING,
    CALLMANAGER_CALLSTATE_DIALING,
    CALLMANAGER_CALLSTATE_SPEAKING,
    NUM_OF_CALLMANAGER_CALLSTATE,
} E_BZMWCALLMANAGER_CALLSTATE_T;


typedef struct
{
    //E_BT_CALL_STATUS_T          eTelStatus;
    E_BZMWCALLMANAGER_CALLSTATE_T eTelStatus;
    E_BT_CALLDIRECTION_T      eTelDir;
    wchar_t                   wszTelNumber[MAX_LEN_PHONE_NUMBER];
    wchar_t                   wszTelName[MAX_LEN_PHONEREC_NAME];
} CALLINFO_T;
 
typedef enum _tag_BT_PhoneSignallevel
{
    BT_PHONESIGNAL_LEVEL_0,
    BT_PHONESIGNAL_LEVEL_1,
    BT_PHONESIGNAL_LEVEL_2,
    BT_PHONESIGNAL_LEVEL_3,
    BT_PHONESIGNAL_LEVEL_4,
    BT_PHONESIGNAL_LEVEL_5,
    BT_PHONESIGNAL_LEVEL_UNKNOWN,
    NUM_OF_BT_PHONESIGNAL_LEVEL
} E_BT_PHONESIGHAL_LEVEL_T;

typedef enum _tag_BT_PhoneBatterylevel
{
    BT_PHONEBATTERY_LEVEL_0,
    BT_PHONEBATTERY_LEVEL_1,
    BT_PHONEBATTERY_LEVEL_2,
    BT_PHONEBATTERY_LEVEL_3,
    BT_PHONEBATTERY_LEVEL_4,
    BT_PHONEBATTERY_LEVEL_5,
    BT_PHONEBATTERY_LEVEL_UNKNOWN,
    NUM_OF_BT_PHONEBATTERY_LEVEL
} E_BT_PHONEBATTERY_LEVEL_T;

typedef enum
{
    PHONEST_ACTIVE,
    PHONEST_INACTIVE
}E_BZMWCALLMANAGER_STATE_T;


typedef enum
{
    CALLCMD_CREATECONNECT,
    CALLCMD_REMOVECONNECT,
    CALLCMD_SETAUDIOOUTPUT,
    CALLCMD_ANSWERCALL,
    CALLCMD_REJECTCALL,
    CALLCMD_TERMINATECALL,
    CALLCMD_DIAL,
    CALLCMD_REDIAL,
    CALLCMD_SENDDTMF,
    NUM_OF_BZMWCALLCMD
} E_BZMWCALLCMD_T;





#endif /* _BZS_CALL_H_ */

