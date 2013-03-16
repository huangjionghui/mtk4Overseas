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
 * $Revision: #6 $
****************************************************************************/

#ifndef _BZI_PBMGR_H_
#define _BZI_PBMGR_H_
//
// BlueZone PBMGR Profile - API definition
//

//#include "BMSComm.h"
#include "BZS_PBMgr.h"


HRESULT BZ_PBMGRRelRecs(P_PBMGRRECORD_T *ppRecord, UINT32 u4RecCnt);
HRESULT BZ_PBMGRRelNames(wchar_t *awszName[], UINT32 u4NameCnt);
HRESULT BZ_PBMGRGetNameByTelExectly(wchar_t *wszTel, 
        UINT32 u4MaxFindCnt, UINT32 &u4FoundCnt, wchar_t *awszName[]);


#ifdef __cplusplus
extern "C" {
#endif


void BZ_PBMGRStartSync(E_BT_PBAP_SYNC_PATH_T ePath);

void BZ_PBMGRStopSync();

HRESULT BZ_PBMGRSetIndStep(UINT32 u4Step);

HRESULT BZ_PBMGRGetRecCnt(E_BT_PBAP_SYNC_PATH_T ePath, UINT32 &u4RecCnt);

HRESULT BZ_PBMGRGetRecs(E_BT_PBAP_SYNC_PATH_T ePath,
    UINT32 u4FirstIdx, UINT32 u4RecCnt, P_PBMGRRECORD_T ppRecord[]);

HRESULT BZ_PBMGRClearAllRec(E_BT_PBAP_SYNC_PATH_T ePath);

HRESULT BZ_PBMGRSetMaxSyncCnt(E_BT_PBAP_SYNC_PATH_T ePath, UINT32 u4MaxCnt);

BOOL BZ_PBMGRIsDownloading();

E_PBMGR_SYNC_STATE_T BZ_PBMGRGetState();

HRESULT BZ_PBMGRSetTimeOutVal(E_BT_PBAP_SYNC_PATH_T ePath, UINT32 u4TimerOut);  //ms

PBMGR_TYPE_CONNECT BZ_PBMGRGetConnectType(E_BT_PBAP_SYNC_PATH_T ePath);

BOOL BZ_PBMGRDataPathIsValid();

BOOL BZ_PBMGRSyncIsInterruped(E_BT_PBAP_SYNC_PATH_T ePath);

BOOL BZ_PBMGRIsSptSync(E_BT_PBAP_SYNC_PATH_T ePath);

void BZ_PBMGRContinueSync(E_BT_PBAP_SYNC_PATH_T ePath);

#if PBMGR_UPDATE_LOCAL_CALLHISTORY_SELF
HRESULT BZ_PBMGRAddLastCallToLocalContactHistory();
#endif
HRESULT BZ_InitFavorite(P_FAVORITE_INFO pFavoriteInfo, DWORD u4BuffSz, DWORD *pu4ExistRecord);

HRESULT BZ_DeleteFavoriteByIdx(DWORD u4Index);

HRESULT BZ_AddFavorite(P_FAVORITE_INFO pFavoriteInfo);

HRESULT BZ_ResetDownloadConnect();
#ifdef __cplusplus
}
#endif


#endif  /* _BZI_PBMGR_H_ */

