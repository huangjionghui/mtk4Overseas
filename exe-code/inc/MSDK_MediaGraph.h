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
* file MSDK_MediaGraph.h
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
* $Revision: #32 $
* $Modtime:$
* $Log:$
*
*******************************************************************************/


#ifndef _MSDK_MEDIAGRAPH_H_
#define _MSDK_MEDIAGRAPH_H_


#include <MSDK_Defs.h>



#ifdef __cplusplus
extern "C" {
#endif

/**
  *@addtogroup PlaybackMechanism 
 * @{
 *
 */

/**
*	@brief This method create media graph. 
*
*	@param[out] 	phMediaGraph pointer to media graph handle.
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	msdk_error.h
*	@see	MEDIAGRAPH_T
**/
MRESULT     MediaGraph_Create(HMEDIAGRAPH *phMediaGraph);

/**
*	@brief This method release media graph. 
*
*	@param[in] 	hMediaGraph media graph handle.
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
*
*	@note the method might be called while app exit.media graph will be destroyed,all inteface be released also.
**/
MRESULT     MediaGraph_Release(HMEDIAGRAPH hMediaGraph);

/**
*	@brief This method clear media graph. 
*
*	@param[in] 	hMediaGraph media graph handle.
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
*
*	@note the method might be called while stop play .mediagraph not be destroyed,but some members will be released .
**/
MRESULT 	MediaGraph_Clear(HMEDIAGRAPH hMediaGraph);

/**
*	@brief This method set media graph config.repeat play and notify frequency 
*
*	@param[in] 	hMediaGraph media graph handle.
*	@param[in]	prConfig config param for mediagraph
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
*	@see	MG_CONFIG_T
**/
MRESULT     MediaGraph_SetConfig(HMEDIAGRAPH hMediaGraph, MG_CONFIG_T *prConfig);

/**
*	@brief This method retrieves media graph config. 
*
*	@param[in] 	hMediaGraph media graph handle.
*	@param[out]	prConfig current mediagraph config
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
*	@see	MG_CONFIG_T
**/
MRESULT     MediaGraph_GetConfig(HMEDIAGRAPH hMediaGraph, MG_CONFIG_T *prConfig);

/**
*	@brief This method retrieves media graph state(play pause stop). 
*
*	@param[in] 	hMediaGraph media graph handle.
*	@param[out]	peState current media graph state
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
*	@see	E_MSDK_MGSTATE
**/
MRESULT     MediaGraph_GetState(HMEDIAGRAPH hMediaGraph, E_MSDK_MGSTATE *peState);

/**
*	@brief This method query interface by IID. 
*
*	@param[in] 	hMediaGraph media graph handle.
*	@param[in]	pIID MSDK_IID marked 
*	@param[out]	ppvInterface address of interface pointer
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
*	@see	MSDK_IID
**/
MRESULT     MediaGraph_QueryInterface(HMEDIAGRAPH hMediaGraph, const MSDK_IID *pIID,
                                      GVOID **ppvInterface);

/**
*	@brief This method set media graph  source file. 
*
*	@param[in] 	hMediaGraph media graph handle.
*	@param[in]	szFileName file path will be render
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
**/
MRESULT     MediaGraph_SetSourceFile(HMEDIAGRAPH hMediaGraph, GTCHAR *szFileName);

MRESULT     MediaGraph_SetSourceFileEx(HMEDIAGRAPH hMediaGraph, GTCHAR *szFileName);


/**
*	@brief This method set media graph  source file and paly. 
*
*	@param[in] 	hMediaGraph media graph handle.
*	@param[in]	szFileName file path will be render
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
**/


MRESULT MediaGraph_SetMediaFile(HMEDIAGRAPH hMediaGraph, GTCHAR *szFileName, GUINT64 u8Time);


/**
*	@brief This method set media graph  dvd volume source . 
*
*	@param[in] 	hMediaGraph media graph handle.
*	@param[in]	szDiscName dvd volum path
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
**/
MRESULT     MediaGraph_SetSourceDVDVolume(HMEDIAGRAPH hMediaGraph, GTCHAR *szDiscName);

/**
*	@brief This method set media graph  source stream. 
*
*	@param[in] 	hMediaGraph media graph handle.
*	@param[in]	eType source stream type
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
*	@see	E_MSDK_SOURCESTREAM_TYPE
*
*	@note not implement
**/
MRESULT     MediaGraph_SetSourceStream(HMEDIAGRAPH hMediaGraph, 
                                       E_MSDK_SOURCE_TYPE eType);

/**
*	@brief This method set media graph  source format. 
*
*	@param[in] 	hMediaGraph media graph handle.
*	@param[in]	peFormat new suorce format.
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
*	@see	E_MSDK_SOURCEFMT
**/
MRESULT     MediaGraph_GetSourceFormat(HMEDIAGRAPH hMediaGraph, E_MSDK_SOURCEFMT *peFormat);



/**
*	@brief This method set media graph data source. 
*
*	@param[in] 	hMediaGraph media graph handle.
*	@param[in]	eSourceType source type might file or line in.
*	@param[in]    eSinkType eSinkType video display target to front ,rear or both
*	@param[out]  param extend parameter if file source,the parameter is file path
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
**/
MRESULT		MediaGraph_SetDataSource(HMEDIAGRAPH hMediaGraph,E_MSDK_SOURCE_TYPE eSourceType,E_MSDK_AVIN_AINDEX eAIndex, E_MSDK_AVIN_VINDEX eVIndex, E_MSDK_SINK_TYPE eSinkType,GVOID* param);

/**
*	@brief This method force media progress stop. 
*
*	@param[in] 	hMediaGraph media graph handle.
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
**/
MRESULT     MediaGraph_TerminateLoad(HMEDIAGRAPH hMediaGraph);
MRESULT     MediaGraph_TerminateLoadAvin(HMEDIAGRAPH hMediaGraph);
MRESULT     MediaGraph_GetVStmCnt(HMEDIAGRAPH hMediaGraph, UINT8 *u1VideoStreamCnt);
MRESULT     MediaGraph_GetAVInfoLimitation(HMEDIAGRAPH hMediaGraph, E_MSDK_VIDEO_INFO_LIMITATION_T *pVInfo, E_MSDK_AUDIO_INFO_LIMITATION_T *pAInfo);


MRESULT     MediaGraph_GetIsSignalReady(HMEDIAGRAPH hMediaGraph, BOOL *fgIsSignalReady);

MRESULT MediaGraph_SetVideoInSource(HMEDIAGRAPH hMediaGraph, E_MSDK_AVIN_VINDEX eVIndex,
                                    MSDK_AVIN_SINK_INFO_T *prSinkInfo);
MRESULT MediaGraph_SetAudioInSource(HMEDIAGRAPH hMediaGraph,E_MSDK_AVIN_AINDEX eAIndex, 
									MSDK_AVIN_SINK_INFO_T *prSinkInfo);
MRESULT MediaGraph_SetAVInSource(HMEDIAGRAPH hMediaGraph,E_MSDK_AVIN_VINDEX eVIndex,
								 E_MSDK_AVIN_AINDEX eAIndex, MSDK_AVIN_SINK_INFO_T *prSinkInfo);

MRESULT MediaGraph_SetVideoInInfo(HMEDIAGRAPH hMediaGraph, MSDK_AVIN_SINK_INFO_T *prSinkInfo);
MRESULT MediaGraph_SwitchAVInSink(HMEDIAGRAPH hMediaGraph,E_MSDK_SINK_TYPE eSinkType,E_MSDK_MODE eMode);

MRESULT MediaGraph_GetAudioVOLInInfo(HMEDIAGRAPH hMediaGraph, MSDK_OUTPUT_VOL *prOutputVOLInfo, UINT16 uiSize);
MRESULT MediaGraph_VdoInGetIsSignalReady(HMEDIAGRAPH hMediaGraph, BOOL *fgIsSignalReady);
MRESULT MediaGraph_SetScale(HMEDIAGRAPH hMediaGraph,MSDK_SCALE_INFO_T *prScaleInfo);

MRESULT MediaGraph_GetAudInSpec(HMEDIAGRAPH hMediaGraph, UINT32 *pu1SValues, UINT32 *pu1LValues);

/**
*	@brief This method set media graph display target. 
*
*	@param[in] 	hMediaGraph media graph handle.
*	@param[in]    eSiType  video display target to front ,rear or both
*
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_Create()
*	@see	msdk_error.h
**/
MRESULT 	MediaGraph_SetDataSink(HMEDIAGRAPH hMediaGraph,E_MSDK_SINK_TYPE eSiType);

MRESULT 	MediaGraph_SetSinkInfo (HMEDIAGRAPH hMediaGraph,MSDK_SINK_INFO_T *prSinkInfo);


/**
*@}
*/

#ifdef __cplusplus
}
#endif

#endif //_MSDK_MEDIAGRAPH_H_


