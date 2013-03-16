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
  * $Author: wenchao.liu $ 
  *
  * $Modtime: $  
  *
  * $Revision: #12 $
 ****************************************************************************/
#ifndef _BZI_BZMW_H_
#define _BZI_BZMW_H_
 
 
#include "BZS_Comm.h"
#include "BZS_Pairing.h"
#include "BZ_NotifyID.h"
 
 
#ifdef __cplusplus
 extern "C" {
#endif
 
 BOOL BZ_CheckLog(BOOL fgBZMW);
 /**
 * @brief Init Bluezone Middleware func and receive Message by callback function
 * @param fpIndCallback
 * @param filter
 */
 HRESULT BZ_Init(P_BTCALLBACK fpIndCallback, UINT32 u4ServiceMask, LPVOID param);
 
 /**
 * @brief deinit this blueman function
 */
 void BZ_Deinit();
 
 void BZ_RegBTCallBack(UINT32 u4ViewID, E_BTEVTCALLBACK_MODULE_T eModuleID, P_BTCALLBACK pfnCB);
 
 void BZ_UnRegBTCallBack(UINT32 u4ViewID, E_BTEVTCALLBACK_MODULE_T eModuleID);
 
 void BZ_HandleBTEventEntry();
 
 void BZ_OpenBlueTooth(BOOL fgWriteConfigFile);
 
 void BZ_CloseBlueTooth(BOOL fgWriteConfigFile);
 
 HRESULT BZ_GetBlueToothStatus(E_BT_STATUS_T &eState);
 
 /**
 * @bridef Set Bluetooth Audio/SCO Volume Level
 * @param eVolumeLvl  Auido/SCO volume level to set
 */
#if FIXED_VOLUME_LEVEL
 HRESULT BZ_SetVolume(E_BT_VOLUME_LEVEL_T eVolumeLvl);
#else
 HRESULT BZ_SetVolume(DWORD u4VolumeLvl);
#endif
 
 /**
 * @bridef Get Bluetooth Audio/SCO Volume Level
 * @param none
 */
#if FIXED_VOLUME_LEVEL
 E_BT_VOLUME_LEVEL_T BZ_GetVolume(VOID);
#else
 DWORD BZ_GetVolume(VOID);
#endif
 
 HRESULT BZ_Mute();
 HRESULT BZ_StopRing();
 VOID BZ_GetVersion(PVOID szVersion, UINT32 u4Length);
 DWORD BZ_GetMaxVol();
 VOID BZ_SetMaxVol(DWORD u4VolMaxLvl);
 HRESULT BZ_SetMode(BT_STATUS *peMode,BOOL fgSync = TRUE);
 HRESULT BZ_GetMode(BT_STATUS *peMode);
 HRESULT BZ_SetRingVolume(DWORD dwGain);
 
#ifdef __cplusplus
 }
#endif
 
#endif /* _BZI_BZMW_H_ */
 
