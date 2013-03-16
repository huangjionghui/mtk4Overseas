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
* file MSDK_Defs.h
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
* $Revision: #36 $
* $Modtime:$
* $Log:$
*
*******************************************************************************/

#ifndef _MSDK_DEFS_H_
#define _MSDK_DEFS_H_

#include <GDef.h>
#include "ddraw.h"


typedef     GUINT32         MRESULT;

typedef     GVOID *         HMEDIAGRAPH;    /**< mediagraph handle 		*/
typedef     GVOID *         HVIDEOINFO;	  /**< videoinfo handle    		*/
typedef     GVOID *         HAUDIOINFO;	  /**< audioinfo handle    		*/
typedef     GVOID *         HSUBTITLEINFO;  /**< subtitleinfo handle  		*/
typedef     GVOID *         HDVDINFO;		  /**< dvdinfo handle         	*/
typedef     GVOID *         HMP3INFO;		  /**< mp3info handle       		*/
typedef     GVOID *         HDVXMENUINFO;	  /**< divxmenuinfo handle       */
typedef     GVOID *         HVIDEOCTRL;	  /**< divxmenuinfo handle       */
typedef     GVOID *         HAUDIOCTRL;	  /**< audiocontrol handle         */
typedef     GVOID *         HSUBTITLECTRL;	  /**< subtitle control handle      */
typedef     GVOID *         HDVDCTRL;		  /**< dvd control handle      */
typedef     GVOID *         HLYRICINFO;      /**< lyricinfo handle    */

typedef     GUINT32         MSDK_IID;

#define     INVALID_MSDK_HANDLE     (NULL)

#define     MAX_AUDIO_ID3INFO_LEN    (27)


#define MSDK_SWI_INFO_SHOW        		0x00000001
#define MSDK_SWI_INFO_HIDE        		0x00000002
#define MSDK_SWI_INFO_DESTCOLKEY        0x00000004
#define MSDK_SWI_INFO_SRCCOLKEY         0x00000008
#define MSDK_SWI_INFO_ALPHA				0x00000010
#define MSDK_SWI_INFO_RESTORE			0x00000020
typedef enum
{
    MSDK_STATE_INITIALIZED,
    MSDK_STATE_UNINITIALIZED,
    MSDK_STATE_RUNNING
} E_MSDK_STATE;

/**mediagraph state
*
*/
typedef enum
{
    MSDK_MGSTATE_PLAYING,/**<mediagraph playing sate*/
    MSDK_MGSTATE_PAUSE,  /**<mediagraph pause sate*/
    MSDK_MGSTATE_STOP    /**<mediagraph stop sate*/
} E_MSDK_MGSTATE;

/**
* MSDK source type
*/
typedef enum
{
    MSDK_SOURCE_NONE,	 /**<none source*/
	MSDK_SOURCE_DVD,	 /**<dvd source */
	MSDK_SOURCE_FILE,	 /**<file source*/
    MSDK_SOURCE_STRM_AV, /**<line in*/
    MSDK_SOURCE_STRM_A,  /**<audio in*/
    MSDK_SOURCE_STRM_V   /**<video in*/
} E_MSDK_SOURCE_TYPE;

// Snail(MTK71174) add by 2012-04-28
typedef enum
{
    MSDK_AVIN_ANONE,
	MSDK_AVIN_A0,
	MSDK_AVIN_A1,
	MSDK_AVIN_A2,
	MSDK_AVIN_A3,
	MSDK_AVIN_A4
}E_MSDK_AVIN_AINDEX;

typedef enum
{
    MSDK_AVIN_VNONE,
	MSDK_AVIN_V1,
	MSDK_AVIN_V2,
	MSDK_AVIN_V3,
	MSDK_AVIN_V4,
	MSDK_AVIN_V0
}E_MSDK_AVIN_VINDEX;



typedef enum
{
    E_MSDK_AUDIO_NO_LIMITATION,
    E_MSDK_AUDIO_NO_AUDIO,
    E_MSDK_AUDIO_FEACHER_NOT_SUPPORT,
    E_MSDK_AUDIO_SAMPLERATE_NOT_SUPPORT,
    E_MSDK_AUDIO_BITRATE_NOT_SUPPORT,
    E_MSDK_AUDIO_CODEC_NOT_SUPPORT,
    E_MSDK_AUDIO_UNKNOWN_NOT_SUPPORT
}E_MSDK_AUDIO_INFO_LIMITATION_T;

typedef enum
{
    E_MSDK_VIDEO_NO_LIMITATION,
	E_MSDK_VIDEO_NO_VIDEO,
    E_MSDK_VIDEO_FEACHER_NOT_SUPPORT,
    E_MSDK_VIDEO_RESOLUTION_NOT_SUPPPORT,
    E_MSDK_VIDEO_BITRATE_NOT_SUPPORT,
    E_MSDK_VIDEO_FRAMERATE_NOT_SUPPORT,
    E_MSDK_VIDEO_CODEC_NOT_SUPPORT,
    E_MSDK_VIDEO_UNKNOWN_NOT_SUPPORT
}E_MSDK_VIDEO_INFO_LIMITATION_T;


//typedef enum
//{
//    MSDK_AVIN_ANONE,
//	MSDK_AVIN_A1,
//	MSDK_AVIN_A2,
//	MSDK_AVIN_A3,
//	MSDK_AVIN_A4,
//	MSDK_AVIN_A5
//}E_MSDK_AVIN_AINDEX;
//
//typedef enum
//{
//    MSDK_AVIN_VNONE,
//	MSDK_AVIN_V1,
//	MSDK_AVIN_V2,
//	MSDK_AVIN_V3,
//	MSDK_AVIN_V4,
//	MSDK_AVIN_V5
//}E_MSDK_AVIN_VINDEX;

typedef enum
{
    MSDK_SOURCEFMT_DVD,
    MSDK_SOURCEFMT_FILE,
    MSDK_SOURCEFMT_STREAM
} E_MSDK_SOURCEFMT;

//msdk sink type that is display target
typedef enum
{
	MSDK_SINK_NONE,		/**<msdk no target*/
    MSDK_SINK_FRONT,	/**<msdk target to front*/
    MSDK_SINK_REAR ,	/**<msdk target to rear*/
    MSDK_SINK_FRONT_REAR/**<msdk target to front and rear*/
}E_MSDK_SINK_TYPE;

typedef enum
{
	MSDK_DRAM,
	MSDK_BYPASS
}E_MSDK_MODE;

typedef struct
{
    E_MSDK_SINK_TYPE eSinkType;
    RECT             rRect;
    GBOOL            fgAbortScale;
}MSDK_SCALE_INFO_T;

typedef struct
{
    E_MSDK_SINK_TYPE	eSink;
    RECT 				rFrontDestRect;
    RECT                rRearDestRect;
    UINT32          	u4FrontFlag;
    UINT32          	u4RearFlag;
    DDOVERLAYFX    	 	rFrontOvfx;
    DDOVERLAYFX    	 	rRearOvfx;
    E_MSDK_MODE			eMode;//just for rear video choose bypass(not support now) or dram,
}MSDK_AVIN_SINK_INFO_T;

typedef struct
{
	E_MSDK_SINK_TYPE	eSink;
	RECT 				DestRect;
	UINT32          	u4Flag;
	DDOVERLAYFX    	 	ovfx;
	E_MSDK_MODE			eMode;//just for video choose bypass or dram
}MSDK_SINK_INFO_T;

// Snail (MTK71174) add by 12-02-20
// Start
// Vol Info
typedef struct _MSDK_OUTPUT_VOL
{
	UINT32 MSDK_u4ChVolFrontL;		//  front Left
	UINT32 MSDK_u4ChVolFrontR;        //  front Right
	UINT32 MSDK_u4ChVolFrontLs;      //   front Left surround
	UINT32 MSDK_u4ChVolFrontRs;     //    front Right surround
	UINT32 MSDK_u4ChVolFrontC;	     //     front  C				
	UINT32 MSDK_u4ChVolFrontSub;   //    front Sub

	UINT32 MSDK_u4ChVolGpsMix;	//   GpsMix		

	UINT32 MSDK_u4ChVolRearL;	       //  rear Left		
	UINT32 MSDK_u4ChVolRearR;	       //  rear Right

	UINT32 MSDK_u4ChVolBypassL;      //  bypass Left	  		
	UINT32 MSDK_u4ChVolBypassR;    //    bypass Right		
}MSDK_OUTPUT_VOL;

// end

typedef enum
{
	MSDK_PLAY_SPEED_NORAML, /**<normal play*/
	MSDK_PLAY_SPEED_2X,     /**<play at twice the speed*/
	MSDK_PLAY_SPEED_4X,     /**<play at four times the speed*/
	MSDK_PLAY_SPEED_8X,		/**<play at eight times the speed*/
	MSDK_PLAY_SPEED_16X,	/**<play at sixteen times the speed*/
	MSDK_PLAY_SPEED_32X		/**<play at thirty-two times the speed*/
}E_MSDK_PLAY_SPEED;

//media graph config
typedef struct _MG_CONFIG
{
    GBOOL       fgRepeat;             /**<if true the video repeat play*/
    GUINT32     u4MediaPosNotifyFreq; /**<set notify frequency             */
} MG_CONFIG_T;


typedef enum
{
    MSDK_AVCODEC_MP4
} E_MSDK_AVCODEC;

//subtitle type
typedef enum
{
    MSDK_SUBTITLE_INTERNAL_SUBPIC,/**<internal subtitle is picture type*/
    MSDK_SUBTITLE_INTERNAL_TEXT,  /**<internal subtitle is text type*/
    MSDK_SUBTITLE_EXTERNAL_SUBPIC,/**<external subtitle is picture type*/
    MSDK_SUBTITLE_EXTERNAL_TEXT	  /**<external subtitle is text type*/
} E_MSDK_SUBTITLE_TYPE;


enum
{
    MSDK_LANG_CHINESE,
    MSDK_LANG_ENGLISH
};


typedef enum
{
    MSDK_AUDPROC_NONE
} E_MSDK_AUDPROC;


typedef enum
{
    MSDK_TIMEFMT_FRAME,
    MSDK_TIMEFMT_MEDIA_TIME,
    MSDK_TIMEFMT_SAMPLE,
    MSDK_TIMEFMT_FIELD,
    MSDK_TIMEFMT_DEFAULT = MSDK_TIMEFMT_MEDIA_TIME
} E_MSDK_TIME_FORMAT;

//code page type
typedef enum
{
    MSDK_CPAGE_INVAL = 0x00,     /**<invalid code page index */
    MSDK_CPAGE_1250  = 0x01,     /**<cp1250 Central Europe */
    MSDK_CPAGE_1251  = 0x02,     /**<cp1251 Cyrillic */
    MSDK_CPAGE_1252  = 0x03,     /**<cp1252 Latin I */
    MSDK_CPAGE_1253  = 0x04,     /**<cp1253 Greek */
    MSDK_CPAGE_1254  = 0x05,     /**<cp1254 Turkish */
    MSDK_CPAGE_1255  = 0x06,     /**<cp1255 Hebrew (from right to left)*/
    MSDK_CPAGE_1255_2= 0x07,     /**<cp1255_2 Hebrew (from left to right)*/
    MSDK_CPAGE_1256  = 0x08,     /**<cp1256 Arabic */
    MSDK_CPAGE_1257  = 0x09,     /**<cp1257 Baltic */
    MSDK_CPAGE_1258  = 0x0A,     /**<cp1258 Vietnam */
    MSDK_CPAGE_874   = 0x0B,     /**<cp874 Thai */
    MSDK_CPAGE_932   = 0x0C,     /**<cp932 Shift JIS */
    MSDK_CPAGE_936   = 0x0D,     /**<cp936 Simplified Chinese GBK */
    MSDK_CPAGE_949   = 0x0E,     /**<cp949 Korean */
    MSDK_CPAGE_950       = 0x0F,     /**<cp950 Traditional Chinese BIG5 */
    MSDK_CPAGE_8859_1    = 0x10,     /**<ISO 8859-1 Western European */
    MSDK_CPAGE_8859_2    = 0x11,     /**<ISO 8859-2 Eastern and Central European languages */
    MSDK_CPAGE_8859_3    = 0x12,     /**<ISO 8859-3 Southern European languages */
    MSDK_CPAGE_8859_4    = 0x13,     /**<ISO 8859-4 Northern European languages */
    MSDK_CPAGE_8859_5    = 0x14,     /**<ISO 8859-5 Latin/Cyrillic language */
    MSDK_CPAGE_8859_6    = 0x15,     /**<ISO 8859-6 Latin/Arabic language */
    MSDK_CPAGE_8859_7    = 0x16,     /**<ISO 8859-7 Latin/Greek language */
    MSDK_CPAGE_8859_8    = 0x17,     /**<ISO 8859-8 Latin/Hebrew language */
    MSDK_CPAGE_8859_9    = 0x18,     /**<ISO 8859-9 Turkish language */
    MSDK_CPAGE_KANJI     = 0x19,     /**<ARIB TR-B15 Japanese kanji character set */
    MSDK_CPAGE_FULL      = 0x7F      /**<ISO 10636-1 unicode (UCS) */
} E_MSDK_CODEPAGE_SET;

typedef struct
{
	GTCHAR  szTitle[MAX_AUDIO_ID3INFO_LEN + 1];
	GTCHAR  szYear[MAX_AUDIO_ID3INFO_LEN + 1];
	GTCHAR  szAlbum[MAX_AUDIO_ID3INFO_LEN + 1];
	GTCHAR  szAuthor[MAX_AUDIO_ID3INFO_LEN + 1];
	GTCHAR  szGenre[MAX_AUDIO_ID3INFO_LEN + 1];
}MSDK_AUDIO_ID3INFO_T;

typedef struct
{
	GBYTE   *pAPicBuf;
	GUINT32 u4APicDataLen;
}MSDK_ID3INFO_APIC_T;

typedef enum
{
    PIXFMT_32_ALPHARGB,
    PIXFMT_32_RGB,
    PIXFMT_16_RGB565          
}E_MSDK_PIXELFORMAT;

typedef MRESULT (*PFN_MSDK_MEDIAEVENT_LISTENER)(HMEDIAGRAPH hMediaGraph,
                                                GUINT32 u4Evt, 
                                                GUINT32 u4Param1,
                                                GUINT32 u4Param2,
                                                GUINT32 u4CustomData);


#endif //_MSDK_DEFS_H_