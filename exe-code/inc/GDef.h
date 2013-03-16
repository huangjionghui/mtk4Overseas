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


/*****************************************************************************
 *
 * Filename:
 * ---------
 *   $Workfile:  $ 
 *
 * Project:
 * --------
 *   MiGO
 *
 * Description:
 * ------------
 *   
 *   
 * Author:
 * -------
 *   mcn03001
 *
 * Last changed:
 * ------------- 
 * $Author: yu.mo $ 
 *
 * $Modtime: $  
 *
 * $Revision: #3 $
****************************************************************************/
#ifndef _GDEF_H_
#define _GDEF_H_



#ifdef __cplusplus
extern "C" {
#endif

#ifndef GBASEVER
#define GBASEVER    0x0003
#endif /* GBASEVER */

/***************************************************************************/
/*                      INCLUDE HEADER FILES                               */
/***************************************************************************/
#if defined(MTK_FP)
    #include "MMIDataType.h"

#elif defined(WIN32) || defined(_WIN32_WCE)
    #include <windows.h>
    #include <tchar.h>
    #include <malloc.h>

    #ifdef WIN32
        #include <assert.h>
        #include <string.h>
        #include <stdio.h>
    #else
        #include <stdlib.h>
        #include <memory.h>
    #endif

#endif



/***************************************************************************/
/*                    PROPRIETARY TYPE DEFINITIONS                         */
/***************************************************************************/
#define MIGO_MAX_PATH       260
typedef unsigned long       GRESULT;

/***************************************************************************/
/*                PLATFORM INDEPENDENT DEFINITIONS                         */
/***************************************************************************/
#ifdef NULL
  #undef NULL
#endif

//#ifdef VOID
//  #undef VOID
//#endif
//
//#ifdef BOOL
//  #undef BOOL
//#endif

#ifdef IN
  #undef IN
#endif

#ifdef OUT
  #undef OUT
#endif

#ifdef INOUT
  #undef INOUT
#endif

#ifdef TRUE
  #undef TRUE
#endif

#ifdef FALSE
  #undef FALSE
#endif

#ifdef ON
  #undef ON
#endif

#ifdef OFF
  #define OFF
#endif

#ifdef GAPI
  #undef GAPI
#endif

#ifdef EXTERN
  #undef  EXTERN
#endif

#ifdef LOCAL
  #undef LOCAL
#endif

#ifdef ASSERT
  #undef ASSERT
#endif

#ifdef CONST
  #undef CONST
#endif

#ifdef GLOBAL
  #undef GLOBAL
#endif

#define DDRAW_PANEL_MOVE     1
#define USE_DDRAW_PANEL_MOVE_SURFACE   0
#define DDRAW_PANEL_EFFECT_ARRAY_TOTAL_NUM 2

#define MAX_SUPPORT_LANGUAGE    30
#define MAX_SUPPORT_STRING_LEN  300

#define _USE_DDRAW           1

#define LOAD_MRF_RESOURCE    0
#define GPOINT_FROM_PARAM(point, param)  \
    point.i4X = (GINT16)(param & 0xFFFF); \
    point.i4Y = (GINT16)((param >> 16) & 0xFFFF);
#define GPARAM_FROM_POINT(param, point)  \
    param = (((GUINT16)point.i4X) | (((GUINT16)point.i4Y) << 16))


typedef enum
{
    GLIDELEFT2RIGHT,          //  从左到右
    GLIDERIGHT2LEFT,          //  从右到左
    GLIDETOP2BOTTOM,          //  上到下
    GLIDEBOTTOM2TOP,          //  下到上
    ZOOMIN,                   //  缩小
    ZOOMOUT,                  //  放大
    //FADEIN,                   //  淡入 //mtk71372 2012.01.04 comment
    //FADEOUT,                   //  淡出 //mtk71372 2012.01.04 comment	
	//mtk71372 2011.12.22 added --------------------------- str
	OPEN_HORIZONTAL,       //左右百叶窗开
	CLOSE_HORIZONTAL,	   //左右百叶窗关	
	OPEN_VERTICAL,         //上下百叶窗开
	CLOSE_VERTICAL,        //上下百叶窗关
	SPLIT,                 //撕裂
	UNITE,                 //组合
	LEFT_TOP_ROLL_LEAVE,   //左上角离开
	LEFT_TOP_ROLL_REACH,   //左上角进来
	LEFT_BOTTOM_ROLL_LEAVE,//左下角离开
	LEFT_BOTTOM_ROLL_REACH,//左下角进来
	RIGHT_TOP_ROLL_LEAVE,  //右上角离开
	RIGHT_TOP_ROLL_REACH,  //右上角进来
	RIGHT_BOTTOM_ROLL_LEAVE,//右下角离开
	RIGHT_BOTTOM_ROLL_REACH,//右下角进来
	//mtk71372 2011.12.22 added --------------------------- end
} GGUIKITEFFECTMODE_T;


typedef enum
{
	GSLIDENO,         //  不允许拖动
    GSLIDEX,          //  允许 x 方向拖动
    GSLIDEY,          //  允许 y 方向拖动
    GSLIDEXY          //  允许 x y方向拖动
} GGUIKITSLIDEMODE_T;

typedef struct _GEFFECTARRAY
{
   int   i_Mode;
   HDC   hdcEffect;
   int   i_Start;
   int   i_End;
   int   i_Span;
   int   i_TimerInterval;
   int   i_HdcWidth;
   int   i_HdcHeight;
   int   i_StartLeft;
   int   i_StartTop;

} GEFFECTARRAY;

typedef struct _GEFFECTMOVEARRAY
{
   int   i_Mode;     // 移动模式
   HDC   hdcEffect;  // 要移动的 surface
   int   i_StartX;   // 移动起点
   int   i_StartY;
   int   i_EndX;     // 移动终点
   int   i_EndY;
   int   i_Span;     // 移动步进
   int   i_TimerInterval;  // 移动间隔
   int   i_HdcWidth;
   int   i_HdcHeight;
   int   i_ShowLeft;  // 在屏 surface 上显示的 起点
   int   i_ShowTop;

} GEFFECTMOVEARRAY;

#define  DEF_SCREEN_WIDTH   800
#define  DEF_SCREEN_HIGHT   480



#define IN
#define OUT
#define INOUT
#define TRUE            1
#define FALSE           0
#define ON              1
#define OFF             0
#define NULL            0
#define GVOID           void
#define LOCAL           static
#define CONST           const
#define GLOBAL

#undef _T
#undef __T
#undef TEXT

#ifdef _UNICODE
#define __T(x)          L##x
#else
#define __T(x)          x
#endif

#define _T(x)           __T(x)
#define TEXT            _T

/***************************************************************************/
/*               HW PLATFORM DEPENDENT DEFINITIONS                         */
/***************************************************************************/
#if defined(__arm)
    #define INLINE      __inline
    #define PACKED      __packed
    #define ALIGN(a)    __align(a)

#elif defined(WIN32)
    #define INLINE      __inline
    #define PACKED
    #define ALIGN(a)

#else
    #define INLINE
    #define PACKED
    #define ALIGN(a)
    
#endif

/***************************************************************************/
/*               SW PLATFORM DEPENDENT DEFINITIONS                         */
/***************************************************************************/
#if defined(MTK_FP)
    #define DLLEXPORT
    #define CALLBACK
    #define WINAPI              __stdcall
    #define GHWND                void*
    #define GTCHAR               GWCHAR

#ifndef GMAX_PATH
    #define GMAX_PATH      MIGO_MAX_PATH
#endif

    typedef          char       GCHAR;
    typedef unsigned char       GUCHAR;
    typedef unsigned short        GWCHAR;

    typedef signed   int        GINT;    
    typedef unsigned int        GUINT;

    typedef signed   char       GINT8;
    typedef unsigned char       GUINT8;

    typedef signed short int    GINT16;
    typedef unsigned short int  GUINT16;

    typedef signed long         GINT32;
    typedef unsigned long       GUINT32;

    typedef signed __int64      GINT64;
    typedef unsigned __int64    GUINT64;

    typedef unsigned char       GBYTE;
    typedef unsigned short      GWORD;
    typedef unsigned int        GDWORD;

    typedef const GCHAR *      GLPCSTR;
    typedef const GWCHAR *     GLPCWSTR;
    typedef const GTCHAR *     GLPCTSTR;
    typedef GTCHAR *           GLPTSTR;
    typedef const void *        GLPCVOID;
    typedef GUINT32            GLPARAM;
    typedef GUINT32            GWPARAM;
    typedef GUINT32            GLRESULT;

    typedef float               GFLOAT;
    typedef void *              GHANDLE;
    typedef double              GDOUBLE;

    #if defined(KAL_ON_OSCAR)
        typedef unsigned char   GBOOL;
    #else // KAL_IN_NUCLEUS
        typedef unsigned char   GBOOL;
    #endif

#elif defined(WIN32)
    typedef          char       GCHAR;
    typedef unsigned char       GUCHAR;
    typedef signed   int        GINT;    
    typedef unsigned int        GUINT;

    typedef signed   char       GINT8;
    typedef signed short        GINT16;
    typedef signed int          GINT32;
    typedef signed __int64      GINT64;

    typedef unsigned char       GUINT8;
    typedef unsigned short      GUINT16;
    typedef unsigned int        GUINT32;
    typedef unsigned __int64    GUINT64;

    typedef float               GFLOAT;
    typedef void *              GHANDLE;
    typedef double              GDOUBLE;

    #define GHWND              HWND //void*
    #define DLLEXPORT           _declspec(dllexport)

    typedef wchar_t             GWCHAR;
    typedef GWCHAR              GTCHAR;

    typedef unsigned char       GBOOL; 
    typedef unsigned char       GBYTE;
    typedef unsigned short      GWORD;
    typedef unsigned long       GDWORD;

    typedef const GCHAR *      GLPCSTR;
    typedef const GWCHAR *     GLPCWSTR;
    typedef const GTCHAR *     GLPCTSTR;
    typedef GTCHAR *           GLPTSTR;
    typedef const void *        GLPCVOID;
    typedef GUINT32            GLPARAM;
    typedef GUINT32            GWPARAM;
    typedef GUINT32            GLRESULT;

#ifndef GMAX_PATH
#define GMAX_PATH      MIGO_MAX_PATH
#endif

#else
    // T.B.D. some other platform in the future
#endif

/***************************************************************************/
/*                PLATFORM INDEPENDENT DEFINITIONS                         */
/***************************************************************************/
#ifdef __cplusplus
  #define GAPI          DLLEXPORT
  #define EXTERN        extern "C"
#else
  #define GAPI          extern DLLEXPORT
  #define EXTERN        extern
#endif

/************************************************************************/
/*           EXCEPTION AND ASSERTION  DEFINITIONS                       */
/************************************************************************/
/********  GB_ASSERT_DEBUG == 0  ********/
#if !GB_ASSERT_DEBUG 
  #if !defined(KAL_ON_OSCAR) && defined(MTK_FP)//defined(KAL_ON_OSCAR) || defined(WIN32)
    #define ASSERT(...)
  #else
    #define ASSERT
  #endif

/********  GB_ASSERT_DEBUG == 1  ********/
#else                       
    #include "GLog.h"
      
    GAPI  void   GAssert(GCHAR* , GCHAR* , GUINT32);
    GAPI  GCHAR *GStrrChrA(const GCHAR *pString, GCHAR c);

    #define GET_FILE(file)  (const char *)(((char*)GStrrChrA((file),'\\')) ? ((char*)GStrrChrA((file),'\\')) + 1 : (file))
    #define ASSERT(exp)     do{ if((exp)==FALSE) {LOG_Cmsg("<ASSERT> FILE(%s) LINE(%d) %s", GET_FILE(__FILE__), __LINE__, (#exp)); GAssert(#exp, __FILE__, __LINE__);} }while(0)
#endif /* _DEBUG */


#ifdef __cplusplus
}
#endif

#endif  // _GDEF_H_

