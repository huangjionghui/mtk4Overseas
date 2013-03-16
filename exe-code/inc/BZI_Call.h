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
 * $Author: bin.xu $ 
 *
 * $Modtime: $  
 *
 * $Revision: #4 $
****************************************************************************/

#ifndef _BZI_CALL_H_
#define _BZI_CALL_H_


#include "BZI_Comm.h"

#include "BZS_Call.h"
#include "BZS_PBMgr.h"

#ifdef __cplusplus
extern "C" {
#endif

void BZ_AnswerCall();

void BZ_RejectCall();

void BZ_TerminateCall();

void BZ_Dial (wchar_t *wszTelNum, wchar_t *wszName = NULL);

void BZ_ReDial();

void BZ_SendDtmf (wchar_t *wszDtmf);

BOOL BZ_GetDtmfStatus();

void BZ_SetAudioOutput(BOOL fgPhone);

BOOL BZ_GetAudioOutput();

HRESULT BZ_GetCallTelNum(UINT32 u4Idx, wchar_t *wszTelNum);


//HRESULT BZ_GetCallName(DWORD dwIdx, wchar_t *wszName);
HRESULT BZ_GetCallName(UINT32 u4Idx, wchar_t *wszName);


HRESULT BZ_GetLastCallTelNum(wchar_t *wszTelNum);


HRESULT BZ_GetLastCallName(wchar_t *wszName);


HRESULT BZ_GetCallStatus(UINT32 u4Idx, E_BZMWCALLMANAGER_CALLSTATE_T &eCallStatus);


HRESULT BZ_GetCallInfo(UINT32 u4Idx, E_BT_CALLDIRECTION_T &eCallDir,
    E_BZMWCALLMANAGER_CALLSTATE_T &eCallStatus,
    wchar_t *wszTelNum = NULL, wchar_t *wszName= NULL );

HRESULT BZ_GetCallDir(UINT32 u4Idx, E_BT_CALLDIRECTION_T &eCallDir);

void BZ_EnableAutoAnswerCall (BOOL fgAuto, UINT32 u4Secs);

BOOL BZ_GetAutoAnswerCallState();


HRESULT BZ_GetSignalQuality(E_BT_PHONESIGHAL_LEVEL_T & eLvl);

HRESULT BZ_GetBatteryLevel(E_BT_PHONEBATTERY_LEVEL_T & eLvl);

E_BZMWCALLMANAGER_STATE_T BZ_GetIfInCallState();

HRESULT BZ_SetAPNCode(CHAR *code);


BOOL BZ_IsExistNewCall();

#if PBMGR_UPDATE_LOCAL_CALLHISTORY_SELF
HRESULT BZ_PBMGRAddLastCallToLocalContactHistory();
#endif

BOOL BZ_IsMicMuteEnable(VOID);
HRESULT BZ_EnableMicMute(BOOL fgEnable);

#ifdef __cplusplus
}
#endif

#endif //_BZI_CALL_H_


