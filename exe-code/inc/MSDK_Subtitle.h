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
* file MSDK_Subtitle.h
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
* $Revision: #10 $
* $Modtime:$
* $Log:$
*
*******************************************************************************/

#ifndef _MSDK_SUBTITLE_H_
#define _MSDK_SUBTITLE_H_

#include <MSDK_Defs.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup mediaInfo
 * @{
 */

/**
*    @brief get subtitle count
*
*    @param[in]      hSubtitleInfo  subtitle info handle
*    @param[out]   pu4Count GUINT32* stream count 
*
*    l    the count is stream count which in one file,
*        internal subtitle count is the subtitle stream count include subtext and subpicture which embed in the video file
*        external subtitle count  is the subtitle stream count in the subtext file or external subpicture file
*
*    @return    MRESULT  define in msdk_error.h
*               
*
*    @see     MediaGraph_QueryInterface()
*    @see     msdk_error.h
*
**/
MRESULT     SubtitleInfo_GetCount(HSUBTITLEINFO hSubtitleInfo, GUINT32 *pu4Count);

/**
*    @brief get specify subtitle stream type,internal external subtext sub picture
*
*    @param[in]     hSubtitleInfo  subtitle info handle
*    @param[in]       i4Index GUINT32  stream index
*    @param[out]  peType E_MSDK_SUBTITLE_TYPE* subtitle type
*
*    
*
*    @return    MRESULT  define in msdk_error.h
*               
*
*    @see     MediaGraph_QueryInterface()
*    @see     msdk_error.h
*    @see    E_MSDK_SUBTITLE_TYPE
*
*    @note index unused,now only load one type subtitle.all stream locate in external file or embed in video file
*
**/
MRESULT     SubtitleInfo_GetType(HSUBTITLEINFO hSubtitleInfo, GINT32 i4Index, 
                                 E_MSDK_SUBTITLE_TYPE *peType);

/**
*    @brief get specify subtitle stream codec
*
*    @param[in]     hSubtitleInfo  subtitle info handle
*    @param[in]       i4Index GUINT32  stream index
*    @param[out]  peCodecID E_MSDK_AVCODEC* subtitle stream codec
*
*    
*
*    @return    MRESULT  define in msdk_error.h
*               
*
*    @see     MediaGraph_QueryInterface()
*    @see     msdk_error.h
*    @see    E_MSDK_AVCODEC
*
**/
MRESULT     SubtitleInfo_GetCodec(HSUBTITLEINFO hSubtitleInfo, GINT32 i4Index, 
                                  E_MSDK_AVCODEC *peCodecID);

/**
*    @brief get specify subtitle stream language
*
*    @param[in]     hSubtitleInfo  subtitle info handle
*    @param[in]       i4Index GUINT32  stream index
*    @param[out]  pu4LangID GUINT32* subtitle stream language id
*
*    
*    @return    MRESULT  define in msdk_error.h
*               
*
*    @see     MediaGraph_QueryInterface()
*    @see     msdk_error.h
*
**/
MRESULT     SubtitleInfo_GetLanguage(HSUBTITLEINFO hSubtitleInfo, GINT32 i4Index,
                                     GUINT32 *pu4LangID);
/**
*@}
*/


/**
 * @addtogroup mediaCtrl
 * @{
 */

/**
*    @brief set alpha for subtitle
*
*    @param[in]     hSubtitleCtrl  subtitle control handle
*    @param[in]      uAlpha GUINT alpha value for subtitle 
*
*    
*    @return    MRESULT  define in msdk_error.h
*               
*
*    @see     MediaGraph_QueryInterface()
*    @see     msdk_error.h
*
**/
MRESULT     SubtitleCtrl_SetAlpha(HSUBTITLECTRL hSubtitleCtrl, GUINT8 uAlpha);


/**
*    @brief get current subtitle
*
*    @param[in]     hSubtitleCtrl  subtitle control handle
*    @param[in]      pi4Current GUINT32*  current subtitle index(1~steam number)
*
*    
*    @return    MRESULT  define in msdk_error.h
*               
*
*    @see     MediaGraph_QueryInterface()
*    @see    SubtitleCtrl_SetCurrentSubtitle()
*    @see    SubtitleCtrl_CloseSubtitle()
*    @see     msdk_error.h
*
*    @note    make sure that subtitle not be closed,that is have not use close method or set subtitle index zero
**/
MRESULT     SubtitleCtrl_GetCurrentSubtitle(HSUBTITLECTRL hSubtitleCtrl,
                                            GINT32 *pi4Current);

/**
*    @brief set current subtitle
*
*    @param[in]     hSubtitleCtrl  subtitle control handle
*    @param[in]      i4Index GUINT32  set the subtitle specified by index(1~steam number)
*
*    
*    @return    MRESULT  define in msdk_error.h
*               
*
*    @see     MediaGraph_QueryInterface()
*    @see     SubtitleCtrl_CloseSubtitle()
*    @see     msdk_error.h
*
*    @note      if index is zero,the subtitle is closed.if you want close subtitle,SubtitleCtrl_CloseSubtitle() method recommend
*            
**/
MRESULT     SubtitleCtrl_SetCurrentSubtitle(HSUBTITLECTRL hSubtitleCtrl, GINT32 i4Index);

/**
*    @brief close current subtitle
*
*    @param[in]     hSubtitleCtrl  subtitle control handle
*    
*    @return    MRESULT  define in msdk_error.h
*               
*
*    @see     MediaGraph_QueryInterface()
*    @see     msdk_error.h
*
**/
MRESULT     SubtitleCtrl_CloseSubtitle(HSUBTITLECTRL hSubtitleCtrl);

/**
*    @brief move subtitle to specified position
*
*    @param[in]     hSubtitleCtrl  subtitle control handle
*    @param[in]    i4X GUINT32 x coordinate value
*    @param[in]    i4Y GUINT32 y coordinate value
*    @return    MRESULT  define in msdk_error.h
*               
*
*    @see     MediaGraph_QueryInterface()
*    @see     msdk_error.h
*
*    @note   not implemented
*
**/
MRESULT     SubtitleCtrl_Move(HSUBTITLECTRL hSubtitleCtrl, GINT32 i4X, GINT32 i4Y);
/**
*@}
*/

#ifdef __cplusplus
}
#endif

#endif //_MSDK_SUBTITLE_H_
