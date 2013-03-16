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
 * $Revision: #5 $
****************************************************************************/

#ifndef _BZI_PAIRING_H_
#define _BZI_PAIRING_H_

#include "BZS_Pairing.h"

#ifdef __cplusplus
extern "C" {
#endif

void BZ_StartPairing();
void BZ_StopPairing();
HRESULT BZ_GetPairStatus(E_BT_PAIR_STATUS_T &eState);
void BZ_SetPinCode(wchar_t *wszPinCode);

HRESULT BZ_GetPinCode(wchar_t *wszPinCode,  UINT32 u4MaxSz);

void BZ_SetLocalDevName (wchar_t *wszDevName);
void BZ_GetLocalDevName (wchar_t *wszDevName);
HRESULT BZ_GetLocalDevAddr(wchar_t *wszAddr);
HRESULT BZ_GetLocalDevInfo(wchar_t *wszDevName, UINT32 u4MaxNameSz, wchar_t *wszAddr, UINT32 u4MaxAddrSz);

HRESULT BZ_GetPairedRecCnt(UINT32 &u4Count);

HRESULT BZ_GetPairedRec(UINT32 u4Idx, BT_PAIREDRECORDINFO_T  **prRec);

HRESULT BZ_GetPairedRecs(UINT32 u4FirstIdx, UINT32 u4RecCnt, P_BT_PAIREDRECORDINFO_T  prRecs[]);

void BZ_DeletePairedRec(UINT32 u4Idx = -1);

void BZ_ClearAllPairedRecs();

void BZ_ConnectByPairedRec(UINT32 u4Idx = -1);

void BZ_DisConnectByPairedRec(UINT32 u4Idx = -1);

BOOL BZ_IsCreateDunAfterDis(void);

void BZ_EnableCreateDunAfterDis(void);

HRESULT BZ_GetDevNameFrmSrch(wchar_t *wszDevName);

void BZ_ClrDevNameFrmSrch (void);

void BZ_ResetCurRemoteDev(void);

HRESULT BZ_QuerySuppSvc(BT_ADDR ba);

void BZ_RevokePin(void);
#ifdef __cplusplus
}
#endif


#endif /* _BZI_PAIRING_H_ */

