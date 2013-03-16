/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008 MediaTek Inc.
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

/*******************************************************************************
*
* Filename:
* ---------
* file MSDK_Global.h
*
* Project:
* --------
*   CNB
*
* Description:
* ------------
*
*
* Author:
* -------
*
*
*------------------------------------------------------------------------------
* $Revision: #8 $
* $Modtime:$
* $Log:$
*
*******************************************************************************/

#ifndef _MSDK_GLOBAL_H_
#define _MSDK_GLOBAL_H_


#include <MSDK_Defs.h>
#include "ddraw.h"
#define MSDK_FULLSCREEN 0

#ifdef __cplusplus
extern "C" {
#endif

/**
*  @defgroup PlaybackMechanism  PlaybackMechanism
*
*  @brief the control center of media playback,and the entry of msdk API.msdk initialize and unintialize must be called before enter or exit msdk module.
*  @{
*/

/**
*	@brief This method initializes msdk. 
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
*
*	@note	must be called before msdk api used
**/
#if MSDK_FULLSCREEN
MRESULT     MSDK_Init(HWND hWnd,LPDIRECTDRAWSURFACE pddPrimary);
#else
MRESULT     MSDK_Init(GVOID);
#endif

/**
*	@brief This method uninitializes msdk. 
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
*
*	@note	must be called after app exit.sure that all resource be released
**/
MRESULT     MSDK_Uninit(GVOID);

/**
*	@brief This method initializes thread context. 
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
*
*	@note	not implemented
**/
MRESULT     MSDK_InitThreadContext(GVOID);

/**
*	@brief This method uninitializes  thread context. 
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
*
*	@note	not implemented
**/
MRESULT     MSDK_UninitThreadContext(GVOID);

/**
*	@brief This method reset  msdk thread 
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
*
*	@note	not implemented
**/
MRESULT     MSDK_Reset(GVOID);

/**
*	@brief This method retrieves  msdk state. 
*	
*	@param[out]  peState msdk state type
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
*	@see	E_MSDK_STATE
**/
MRESULT     MSDK_GetState(E_MSDK_STATE *peState);

/**
*@}
*/
#ifdef __cplusplus
}
#endif

#endif //_MSDK_GLOBAL_H_
