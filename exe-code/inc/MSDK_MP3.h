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
* file MSDK_MP3.h
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
* $Revision: #9 $
* $Modtime:$
* $Log:$
*
*******************************************************************************/

#ifndef _MSDK_MP3_H_
#define _MSDK_MP3_H_

#include <MSDK_Defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup mediaInfo
 * @{
 */

/**
*	@brief get MP3 title information
*
*	@param[in] 	 hMP3Info  mp3info handle
*	@param[out]    szTitle GTCHAR*  pointer address of output title information 
*	@param[in]     u4MaxLen GUINT32 max length of szTitle
*
*	@return	   MRESULT  define in msdk_error.h
*			   
*
*	@see	 MediaGraph_QueryInterface()
*	@see	 msdk_error.h
*
**/
MRESULT     MP3Info_GetTitle(HMP3INFO hMP3Info, GTCHAR *szTitle, GUINT32 u4MaxLen);

/**
*	@brief get MP3 artist information
*
*	@param[in] 	 hMP3Info  mp3info handle
*	@param[out]    szArtist GTCHAR* address of output artist information 
*	@param[in]     u4MaxLen GUINT32 max length of szArtist
*
*	@return	   MRESULT  define in msdk_error.h
*			   
*
*	@see	 MediaGraph_QueryInterface()
*	@see	 msdk_error.h
*
**/
MRESULT     MP3Info_GetArtist(HMP3INFO hMP3Info, GTCHAR *szArtist, GUINT32 u4MaxLen);

/**
*	@brief get MP3 picture information
*
*	@param[in] 	 hMP3Info  mp3info handle
*	@param[out]    ppbData GBYTE** pointer to address of output picture data 
*	@param[out]     pu4DataSize GUINT32* size of picture data
*
*	@return	   MRESULT  define in msdk_error.h
*			   
*
*	@see	 MediaGraph_QueryInterface()
*	@see	 msdk_error.h
*
**/
MRESULT     MP3Info_GetAPictureData(HMP3INFO hMP3Info, GBYTE **ppbData, GUINT32 *pu4DataSize);

/**
*	@brief get MP3 album information
*
*	@param[in] 	 hMP3Info  mp3info handle
*	@param[out]    szAlbum GTCHAR*  address of output album information
*	@param[in]     u4MaxLen GUINT32 max length of szAlbum
*
*	@return	   MRESULT  define in msdk_error.h
*			   
*
*	@see	 MediaGraph_QueryInterface()
*	@see	 msdk_error.h
*
**/
MRESULT     MP3Info_GetAlbum(HMP3INFO hMP3Info, GTCHAR *szAlbum, GUINT32 u4MaxLen);

/**
*	@brief get MP3 genre information
*
*	@param[in] 	 hMP3Info  mp3info handle
*	@param[out]    szGenre GTCHAR*  address of output genre information
*	@param[in]     u4MaxLen GUINT32 max length of szGenre
*
*	@return	   MRESULT  define in msdk_error.h
*			   
*
*	@see	 MediaGraph_QueryInterface()
*	@see	 msdk_error.h
*
**/
MRESULT     MP3Info_GetGenre(HMP3INFO hMP3Info, GTCHAR *szGenre, GUINT32 u4MaxLen);

/**
*	@brief get MP3 date information
*
*	@param[in] 	 hMP3Info  mp3info handle
*	@param[out]    szDate GTCHAR*  address of output date information
*	@param[in]    u4MaxLen GUINT32 max length  of szDate
*
*	@return	   MRESULT  define in msdk_error.h
*			   
*
*	@see	 MediaGraph_QueryInterface()
*	@see	 msdk_error.h
*
**/
MRESULT     MP3Info_GetDate(HMP3INFO hMP3Info, GTCHAR *szDate, GUINT32 u4MaxLen);

/**
*	@brief get MP3 year information
*
*	@param[in] 	 hMP3Info  mp3info handle
*	@param[out]    szYear GTCHAR*  address of output year information
*	@param[in]    u4MaxLen GUINT32 max length  of szYear
*
*	@return	   MRESULT  define in msdk_error.h
*			   
*
*	@see	 MediaGraph_QueryInterface()
*	@see	 msdk_error.h
*
**/
MRESULT     MP3Info_GetYear(HMP3INFO hMP3Info, GTCHAR *szYear, GUINT32 u4MaxLen);

/**
*	@brief get MP3 lyric item count
*
*	@param[in] 	 hMP3Info  mp3info handle
*	@param[out]    pu4Count GUINT32*  item count
*
*	@return	   MRESULT  define in msdk_error.h
*			   
*
*	@see	 MediaGraph_QueryInterface()
*	@see	 msdk_error.h
*
**/
MRESULT     MP3Info_GetLyricItemCount(HMP3INFO hMP3Info, GUINT32 *pu4Count);

/**
*	@brief get MP3 lyric item pts
*
*	@param[in] 	 hMP3Info  mp3info handle
*	@param[in]     i4Index  item index
*     @param[out]   pu4Second  item pts
*
*	@return	   MRESULT  define in msdk_error.h
*			   
*
*	@see	 MediaGraph_QueryInterface()
*	@see	 msdk_error.h
*
**/
MRESULT     MP3Info_GetLyricItemPts(HMP3INFO hMP3Info, GINT32 i4Index, GUINT32 *pu4Second);

/**
*	@brief get MP3 lyric item which is specified by index
*
*	@param[in] 	hMP3Info  mp3info handle
*	@param[in]    i4Index GUINT32  item index
*	@param[out]	szLyric  GTCHAR* memory for lyric
*	@param[in] 	u4MaxLen GUINT32 max length of szLyric
*
*	@return	   MRESULT  define in msdk_error.h
*			   
*
*	@see	 MediaGraph_QueryInterface()
*	@see	 msdk_error.h
*
**/
MRESULT     MP3Info_GetLyricItemText(HMP3INFO hMP3Info, GINT32 i4Index,
                                     GTCHAR *szLyric, GUINT32 u4MaxLen);

/**
*	@brief initialized MP3 lyric,lyric Analysis and encode convert
*
*	@param[in] 	 pBLyrcBuff  GBYTE* raw data
*	@param[out]   pWLyrcBuff GWCHAR** wide char be return while pb2Unicode is true
*	@param[out]	 pb2Unicode GBOOL*	true is  converted unicode
*
*	@return	   MRESULT  define in msdk_error.h
*			   
*
*	@see	 MediaGraph_QueryInterface()
*	@see	 msdk_error.h
*
**/
MRESULT		MP3Info_InitLyric(GBYTE *pBLyrcBuff,GWCHAR **pWLyrcBuff,GBOOL *pb2Unicode);
/**
*@}
*/

#ifdef __cplusplus
}
#endif

#endif //_MSDK_MP3_H_
