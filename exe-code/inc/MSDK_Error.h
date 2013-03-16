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
* file MSDK_Error.h
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


#ifndef _MSDK_ERROR_H_
#define _MSDK_ERROR_H_

/**
*@addtogroup PlaybackMechanism
*@{
*
*/

//msdk error code
enum
{
    S_MSDK_OK,						/**<msdk successful			*/
    E_MSDK_OUTOFMEMORY,				/**<error out of memory		*/
    E_MSDK_INVALID_FILE_FORMAT,		/**<error invalid file format		*/
    E_MSDK_INVALID_PARAM,			/**<error invalid parameter		*/
    E_MSDK_INVALID_SOURCE,			/**<error invalid source		*/
    E_MSDK_UNSUPPORTED_VIDEO,		/**<unsupported video			*/
    E_MSDK_UNSUPPORTED_AUDIO,		/**<unsupported audio			*/
    E_MSDK_INVALID_MEDIA_TYPE,		/**<invalid media type			*/
    E_MSDK_ERROR_MEDIA_DATA,		/**<error meida data			*/
    E_MSDK_SYSTEM_BUSY,				/**<system busy				*/
    E_MSDK_NO_SOURCE,				/**<error no source			*/
    E_MSDK_NO_SINK,					/**<error no sink				*/
    E_MSDK_CANNOT_CONNECT,			/**<connect fail				*/
    E_MSDK_CANNOT_RENDER,			/**<render fail				*/
    E_MSDK_NOT_PLAYING,
    E_MSDK_NOT_PAUSED,
    E_MSDK_NOT_STOPED,
    E_MSDK_TIMEOUT,
    E_MSDK_UNSUPPORTED_STREAM,
    E_MSDK_DRM_NOT_SUPPORT,
    E_MSDK_NOT_SUPPORT,
    E_MSDK_NO_MEDIA_GRAPH,
    E_MSDK_NOT_WINDOW_MODE,
    E_MSDK_ALREADY_WINDOW_MODE,
    E_MSDK_FAILED
};

/**
*@}
*/

#endif //_MSDK_ERROR_H_
