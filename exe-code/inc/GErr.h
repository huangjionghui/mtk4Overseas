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
/******************************************************************************
 *[File]             GErr.h
 *[Version]          v1.0
 *[Revision Date]    2007-09-11
 *[Author]           Jau, mtk01843
 *[Description]      export definitions and helper macros to form error code
 ******************************************************************************/
#ifndef GERR_H
#define GERR_H

#include "GDef.h"

#ifdef __cplusplus
  extern "C" {
#endif

/*************************************************************************
 *            GBASE ERROR MODULE INVALID VALUE DEFINITION
 *************************************************************************/
#define INVALID_ID                          (0xFFFFFFFF)
#define INVALID_VALUE                       (0xFFFFFFFF)

#define INVALID_GUINT32                 (0xFFFFFFFF)
#define INVALID_GUINT16                 (0xFFFF)
#define INVALID_GUINT8                  (0xFF)
#define INVALID_GINT					(-1)

/*************************************************************************
 *            GBASE ERROR HELPER MACROS
 *************************************************************************/
/*************************************************************************
 * Error code structure :
 *
 * | 31(1 bit) |30-24(7 bits) | 23-16(8 bits) | 15-0(16 bits) |
 * | Indicator | Module ID    |   Extened     |     Code      |
 *
 * Indicator : 0 - success, 1 - error
 * Module ID : module ID, defined below
 *             only 7 bits, so the maximum ID value is 0x7FL !!!
 * Extended  : module dependent, but providee macro to add partial line info
 * Code      : is the module's status code
 *************************************************************************/
/* Composer for Module specific error code */
#define MAKE_GERR(mod, code)          \
    ((GUINT32)                         \
     ((GUINT32)(0x80000000) |           \
     (GUINT32)(((mod) & 0x7f) << 24) | \
     (GUINT32)((code) & 0xffff))       \
    )

#define MAKE_GERR_EX(mod, ext, code)  \
    ((GUINT32)                         \
     ((GUINT32)(0x80000000) |           \
     (GUINT32)(((mod) & 0x7f) << 24) | \
     (GUINT32)(((ext) & 0xff) << 16)   \
     (GUINT32)((code) & 0xffff))       \
    )

/* Get module Id */
#define GERR_MOD(err)   (((err) >> 24) & 0x7F)
/* Get extended content */
#define GERR_EXT(err)   (((err) >> 16) & 0xFF)
/* get code */
#define GERR_CODE(err)  ((err) & 0xFFFF)


/*************************************************************************
 *            GBASE ERROR MODULE ID LAYOUT DISTRIBUTION
 *************************************************************************/
/*        0x00 : KAL or module unaware
 * 0x01 ~ 0x10 : Reserved
 * 0x11 ~ 0x1F : GBase PAL
 * 0x20 ~ 0x3F : GIS Engine
 * 0x40 ~ 0x4F : Reserved
 * 0x50 ~ 0x5F : Toolkit group
 * 0x60 ~ 0x7F : APP specific
 */

// generic
#define MOD_NULL            0x00L    /* used for module unware code */
#define MOD_GKAL            MOD_NULL /* typically KAL use it */

// 0x01 ~ 0x10 : Reserved

// 0x11 ~ 0x1F : GBase PAL
#define MOD_PAL_GDI         0x11L   /* Graphic */
#define MOD_PAL_RES         0x12L   /* Resource */
#define MOD_PAL_SND         0x13L   /* Sound */
#define MOD_PAL_FS          0x14L   /* File System */
#define MOD_PAL_PREFS       0x15L   /* Preference */
#define MOD_PAL_LOG         0x16L   /* Logger */

// 0x20 ~ 0x3F : GIS Engine
#define MOD_GIS_ENGINE      0x20L   /* Engine */
#define MOD_GIS_DB          0x21L   /* MapDB */
#define MOD_GIS_MD          0x22L   /* Display */
#define MOD_GIS_MS          0x25L   /* Searcher */
#define MOD_GIS_RP          0x26L   /* Route Planner */
#define MOD_GIS_RG          0x27L   /* Route Guidance */
#define MOD_GIS_MM          0x24L   /* Map Matching */
#define MOD_GIS_UT          0x29L   /* Utilities */
#define MOD_GIS_TK          0x30L   /* Track */
#define MOD_GIS_WP          0x31L   /* Waypoint */

// 0x40 ~ 0x4F : Reserved

// 0x50 ~ 0x5F : Toolkit group
#define MOD_TK_GPS          0x50L
#define MOD_TK_GIS          0x51L
#define MOD_TK_GUI          0x52L

// 0x60 ~ 0x7F : APP specific
#define MOD_APP             0x60L


/*************************************************************************
 *            GBASE ERROR MODULE COMMON ERROR CODE PART
 *************************************************************************/
/* Generic codes (the "Code" part) */
#define ERR_FAIL                  0x01L
#define ERR_NOTIMPL               0x02L
#define ERR_OUTOFMEMORY           0x03L
#define ERR_INVALIDARG            0x04L
#define ERR_NOINTERFACE           0x05L
#define ERR_POINTER               0x06L
#define ERR_HANDLE                0x07L
#define ERR_ABORT                 0x08L
#define ERR_UNEXPECTED            0x09L
#define ERR_ACCESSDENIED          0x0AL
#define ERR_MEMDESTROYED          0x0BL  /* memory pattern destroyed */
#define ERR_SEMUNAVAILABLE        0x0CL  /* semaphore unavailable */
#define ERR_INVSTATE              0x0DL  /* invalid state or un-initialized */


#define ERR_MODFIRST              0x200L /**< module specific code define from here! */


/* KAL Composed Error Code */
#define RET_OK                    ((GUINT32)0x00000000)
#define RET_FAIL                  MAKE_GERR(MOD_GKAL,ERR_FAIL)
#define RET_NOTIMPL               MAKE_GERR(MOD_GKAL,ERR_NOTIMPL)
#define RET_INVALIDARG            MAKE_GERR(MOD_GKAL,ERR_INVALIDARG)

#define RET_OUTOFMEMORY           MAKE_GERR(MOD_GKAL,ERR_OUTOFMEMORY)
#define RET_NOINTERFACE           MAKE_GERR(MOD_GKAL,ERR_NOINTERFACE)
#define RET_POINTER               MAKE_GERR(MOD_GKAL,ERR_POINTER)
#define RET_HANDLE                MAKE_GERR(MOD_GKAL,ERR_HANDLE)
#define RET_ABORT                 MAKE_GERR(MOD_GKAL,ERR_ABORT)
#define RET_UNEXPECTED            MAKE_GERR(MOD_GKAL,ERR_UNEXPECTED)
#define RET_ACCESSDENIED          MAKE_GERR(MOD_GKAL,ERR_ACCESSDENIED)
#define RET_MEMDESTROYED          MAKE_GERR(MOD_GKAL,ERR_MEMDESTROYED)
#define RET_SEMUNAVAILABLE        MAKE_GERR(MOD_GKAL,ERR_SEMUNAVAILABLE)
#define RET_INVSTATE              MAKE_GERR(MOD_GKAL,ERR_INVSTATE)
#define GSUCCEEDED(Status)     ((GRESULT)(Status) == RET_OK)

/* File System Error Code */
#define RET_FS_FAIL                 MAKE_GERR(MOD_PAL_FS, ERR_FAIL)
#define RET_FS_CREATE_FAIL   MAKE_GERR(MOD_PAL_FS, 2)
#define RET_FS_OPEN_FAIL       MAKE_GERR(MOD_PAL_FS, 3)
#define RET_FS_WRITE_FAIL       MAKE_GERR(MOD_PAL_FS, 4)
#define RET_FS_READ_FAIL       MAKE_GERR(MOD_PAL_FS, 5)
#define RET_FS_SEEK_FAIL       MAKE_GERR(MOD_PAL_FS, 6)
#define RET_FS_CLOSE_FAIL        MAKE_GERR(MOD_PAL_FS, 7)
#define RET_FS_DELETE_FAIL        MAKE_GERR(MOD_PAL_FS, 8)

/* please dont use the following especially in feature phone version */
/* notice : this will be removed soon in the future !!!!! */
#ifndef _WINERROR_
    #define S_OK                       ((GINT32)0x00000000)
    #define E_FAIL                     MAKE_GERR(MOD_NULL,ERR_FAIL)
    #define E_NOTIMPL                  MAKE_GERR(MOD_NULL,ERR_NOTIMPL)
    #define E_INVALIDARG               MAKE_GERR(MOD_NULL,ERR_INVALIDARG)

    #define E_OUTOFMEMORY              MAKE_GERR(MOD_NULL,ERR_OUTOFMEMORY)
    #define E_NOINTERFACE              MAKE_GERR(MOD_NULL,ERR_NOINTERFACE)
    #define E_POINTER                  MAKE_GERR(MOD_NULL,ERR_POINTER)
    #define E_HANDLE                   MAKE_GERR(MOD_NULL,ERR_HANDLE)
    #define E_ABORT                    MAKE_GERR(MOD_NULL,ERR_ABORT)
    #define E_UNEXPECTED               MAKE_GERR(MOD_NULL,ERR_UNEXPECTED)
    #define E_ACCESSDENIED             MAKE_GERR(MOD_NULL,ERR_ACCESSDENIED)
    #define E_MEMDESTROYED             MAKE_GERR(MOD_NULL,ERR_MEMDESTROYED)
#endif //_WINERROR_



#ifdef __cplusplus
  }
#endif

#endif // GERR_H

