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
 *
 * $Revision: #2 $
****************************************************************************/

#ifndef _BZI_SMS_H_
#define _BZI_SMS_H_
//
// BlueZone SMS Profile - API definition
//

#include "BZS_Comm.h"
#include "BZS_SMS.h"

#ifdef __cplusplus
extern "C" {
#endif

HRESULT BZ_SMSMGRSetRemoteDevice(BT_ADDR btAddr);
HRESULT BZ_SMSMGRClearRemoteDevice();
void BZ_SMSMGRStartSync();
void BZ_SMSMGRStopSync();
HRESULT BZ_SMSMGRSetIndStep(UINT32 u4Step);
HRESULT BZ_SMSMGRGetRecCnt(E_BT_SMS_REC_TYPE_T eType,UINT32 &u4RecCnt);
HRESULT BZ_SMSMGRGetRecs(E_BT_SMS_REC_TYPE_T eType,UINT32 u4FirstIdx, UINT32 u4RecCnt, P_SMSMGRRECORD_T ppRecord[]);
HRESULT BZ_SMSMGRClearAllRec();
HRESULT BZ_SMSMGRSetMaxSyncCnt(UINT32 u4MaxCnt);
E_SMSMGR_SYNC_STATE_T BZ_SMSMGRGetState();
E_BT_SMS_CONNECT_TYPE_T BZ_SMSMGRGetConnectType();
BOOL BZ_SMSMGRIsInSyncing();

BOOL BZ_SMSMGRDataPathIsValid();

BOOL BZ_SMSMGRIsSptSync();

HRESULT BZ_SMSMGRSetTimeOutVal(UINT32 u4TimerOut);  //ms

#ifdef __cplusplus
}
#endif


#endif  /* _BZI_SMS_H_ */


