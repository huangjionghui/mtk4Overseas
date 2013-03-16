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
 *[File]             GKal.h
 *[Version]          v1.0
 *[Revision Date]    2007-09-11
 *[Author]           Jau, mtk01843
 *[Description]      export definitions and interfaces of kal module
 ******************************************************************************/
#ifndef GKAL_H
#define GKAL_H
/*************************************************************************
 *                GBASE KAL INCLUDE FILES
 *************************************************************************/
#include "GDef.h"
#include "GErr.h"

#if defined(MTK_FP)
    #include <stdio.h>
    #include "stacklib.h"
    #include "stack_timer.h"
    #include "event_shed.h"
#endif

#ifdef __cplusplus
  extern "C" {
#endif

/*************************************************************************
 *                GBASE KAL CONSTANT DEFINITION
 *************************************************************************/
#ifndef INFINITE
  #define INFINITE            0xFFFFFFFF  // Infinite timeout
#endif

/***************************************************************************/
/*                    DVP Switch TVE                         */
/***************************************************************************/
#define WM_DVP_SWITCH_TVE          (WM_USER + 0X7500)
#define TVE_AP_CTRL                (0X00)
#define TVE_DVP_CTRL               (0X01)

/**********************************************************************/
/*            GBASE KAL STRING OPERATION ROUTINE                      */
/**********************************************************************/
/***************************/
/* GAC - Ascii Char String */
/***************************/
/* ASCII version of string operation - normal 1B char */
GAPI GUINT32 GStrLenA(const GCHAR *szStr);
GAPI GINT32  GStrCmpA(const GCHAR *szStr1, const GCHAR *szStr2);
GAPI GINT32  GStrCmpiA(const GCHAR *szStr1, const GCHAR *szStr2);
GAPI GCHAR  *GStrCpyA(GCHAR *szDst, const GCHAR *szSrc);
GAPI GINT32  GStrNCmpA(const GCHAR *szStr1, const GCHAR *szStr2, GUINT32 uCount);
GAPI GCHAR  *GStrNCpyA(GCHAR *szDst, const GCHAR *szSrc, GUINT32 uCount);
GAPI GCHAR  *GStrCatA(GCHAR *szDst, const GCHAR *szSrc);
GAPI GCHAR  *GStrChrA(const GCHAR *szStr, GCHAR c);
GAPI GCHAR  *GStrStrA(const GCHAR *szStr, const GCHAR *szCharSet);
GAPI GCHAR  *GStrStriA(const GCHAR *wszStr, const GCHAR *wszCharSet);
GAPI GCHAR  *GStrrChrA(const GCHAR *pString, GCHAR c);
GAPI GCHAR  *GStrUprA(GCHAR *szStr);
GAPI GCHAR  *GStrLwrA(GCHAR *szStr);
GAPI GCHAR   GAc2UprA(GCHAR c);

GAPI GINT32  GStr2IntA(const GCHAR *szStr);
GAPI GINT32  GStr2HexA(const GCHAR *szStr);
GAPI GINT32  GStr2IntExA(const GCHAR *szStr);
GAPI GFLOAT  GStr2FloatA(const GCHAR *szStr);
GAPI GCHAR  *GInt2StrA(GINT32 iNumber, GCHAR *pbBuf, GINT32 iRadix);

GAPI GVOID   GAcStr2WcStr(const GCHAR *szStr, GWCHAR *wszStr);
GAPI GVOID   GWcStr2AcStr(const GWCHAR *wszStr, GCHAR *szStr);

/***************************/
/* GWC - Wide Char String  */
/***************************/
/* UNICODE version of string operation - wide 2B char */
GAPI GUINT32 GStrLenW(const GWCHAR *wszStr);
GAPI GINT32  GStrCmpW(const GWCHAR *wszStr1, const GWCHAR *wszStr2);
GAPI GWCHAR *GStrCpyW(GWCHAR *wszDst, const GWCHAR *wszSrc);
GAPI GINT32  GStrNCmpW(const GWCHAR *wszStr1, const GWCHAR *wszStr2, GUINT32 uCount );
GAPI GWCHAR *GStrNCpyW(GWCHAR *wszDst, const GWCHAR *wszSrc, GUINT32 uCount);
GAPI GWCHAR *GStrCatW(GWCHAR *wszDst, const GWCHAR *wszSrc);
GAPI GWCHAR *GStrChrW(const GWCHAR *wszStr, GWCHAR c);
GAPI GWCHAR *GStrStrW(const GWCHAR *wszStr, const GWCHAR *wszCharSet);
GAPI GWCHAR *GStrrChrW(const GWCHAR *wszString, GWCHAR wc);
GAPI GWCHAR *GStrUprW(GWCHAR *wszStr);
GAPI GWCHAR *GStrLwrW(GWCHAR *wszStr);
GAPI GWCHAR  GWc2Upr(GWCHAR wc);

GAPI GINT32  GStr2IntW(const GWCHAR *wszStr);
GAPI GINT32  GStr2HexW(const GWCHAR *wszStr);
GAPI GINT32  GStr2IntExW(const GWCHAR *wszStr);
GAPI GFLOAT  GStr2FloatW(const GWCHAR *wszStr);
GAPI GVOID   GInt2StrW(GINT32 iNumber, GWCHAR *pbBuf, GINT32 iRadix);
GAPI GVOID   GFloat2StrW(GWCHAR *pOutStr, GFLOAT fInDec, GINT32 i4DecNum);

#if defined(MTK_FP)
#define GSprintfW   kal_wsprintf
#define GSprintfA   kal_sprintf
#else
#define GSprintfW   wsprintfW
#define GSprintfA   sprintf //wsprintfA
#endif

#ifndef UNICODE
    #define GStrLen     GStrLenA
    #define GStrCmp     GStrCmpA
    #define GStrNCmp    GStrNCmpA
    #define GStrCpy     GStrCpyA
    #define GStrNCpy    GStrNCpyA
    #define GStrCat     GStrCatA
    #define GStrChr     GStrChrA
    #define GStrStr     GStrStrA
    #define GStrUpr     GStrUprA
    #define GStrLwr     GStrLwrA
    #define GStrrChr    GStrrChrA
    //For string conversion
    #define GInt2Str    GInt2StrA
    #define GStr2Int    GStr2IntA
    #define GStr2IntEx     GStr2IntExA

#else // defined(WIN32)
    #define GStrLen     GStrLenW
    #define GStrCmp     GStrCmpW
    #define GStrNCmp    GStrNCmpW
    #define GStrCpy     GStrCpyW
    #define GStrNCpy    GStrNCpyW
    #define GStrCat     GStrCatW
    #define GStrChr     GStrChrW
    #define GStrStr     GStrStrW
    #define GStrUpr     GStrUprW
    #define GStrLwr     GStrLwrW    
    #define GStrrChr    GStrrChrW
    #define GInt2Str    GInt2StrW
    #define GStr2Int    GStr2IntW
    #define GStr2IntEx  GStr2IntExW

#if defined(MTK_FP) && !defined(HIWORD)
    #define HIWORD(arg)         (*((GUINT16 *)&(arg) + 1))
#endif

#if defined(MTK_FP) && !defined(LOWORD)
    #define LOWORD(arg)         (* (GUINT16 *)&(arg))    
#endif
#endif // defined(WIN32)


/*****************************************/
/* Strin Converter between UTF8 and UCS2 */
/*****************************************/
typedef enum
{
    STR_CONV_UTF8_TO_UCS2 = 0,
    STR_CONV_UCS2_TO_UTF8 = 1     
} GSTR_CONV_TYPE_T;

GAPI GINT32 GStrConv(GSTR_CONV_TYPE_T eConvType, GUINT8 *pUtf8, GWCHAR *pUcs2, GUINT32 u4Utf8Sz, GUINT32 u4Ucs2Sz);

/***************************************************************************/
/*            GBASE KAL MEMORY MANAGEMENT ROUTINE                          */
/***************************************************************************/
/* PART_POOL_CB = Partition Pool Control Block */
/* HEAP_POOL_CB = Dynamic Heap Pool Control Block */

#if GB_MEMORY_DIAGNOSE
  #define GPART_PCB_ELEM        (7 + 5)  // sizeof(GPART_PCB_T) / sizeof(GUINT32)
  #define GHEAP_PCB_ELEM        (4 + 11) // sizeof(GHEAP_PCB_T) / sizeof(GUINT32)
#else
  #define GPART_PCB_ELEM        7
  #define GHEAP_PCB_ELEM        4
#endif // GB_MEMORY_DIAGNOSE

typedef struct
{
    GUINT32 u4Dummy[GPART_PCB_ELEM];
} GPARTITIONPOOL_T;

typedef struct
{
  GUINT32  u4Dummy[GHEAP_PCB_ELEM]; /* placeholder */
} GHEAPPOOL_T, GMEMORYPOOL_T;

#define GPART_PCB_SZ    sizeof(GPARTITIONPOOL_T)
#define GHEAP_PCB_SZ    sizeof(GHEAPPOOL_T)

/**************************/
/* GPM - Partition Memory */
/**************************/
/* Partition APIs */
#if GB_MEMORY_DIAGNOSE
    #define GCreatePartitionPool(ppol, psa, sz, psz)  GCreatePPool((ppol),(psa),(sz),(psz))
    #define GDeletePartitionPool(ppol)                GDeletePPool((ppol))
    #define GAllocPartition(ppol)                     GAllocP((ppol),(__FILE__),(__LINE__))
    #define GFreePartition(ppol, part)                GFreeP((ppol),(part),(__FILE__),(__LINE__))
    #define GFreeAllPartition(ppol)                   GFreeAllP((ppol),(__FILE__),(__LINE__))
    GAPI GBOOL  GCreatePPool(GPARTITIONPOOL_T *prPool, GVOID *pvSA, GUINT32 u4SZ, GUINT32 u4PartitionSZ);
    GAPI GVOID  GDeletePPool(GPARTITIONPOOL_T *prPool);
    GAPI GVOID *GAllocP(GPARTITIONPOOL_T *prPool, const GCHAR *pFileName, GUINT32 uLineNo);
    GAPI GVOID  GFreeP(GPARTITIONPOOL_T *prPool, GVOID *pvPartition, const GCHAR *pFileName, GUINT32 uLineNo);
    GAPI GVOID  GFreeAllP(GPARTITIONPOOL_T *prPool, const GCHAR *pFileName, GUINT32 uLineNo);
#else //#ifdef GB_MEMORY_DIAGNOSE
    #define GCreatePartitionPool(ppol, psa, sz, psz)  GCreatePPool((ppol),(psa),(sz),(psz))
    #define GDeletePartitionPool(ppol)                GDeletePPool((ppol))
    #define GAllocPartition(ppol)                     GAllocP((ppol))
    #define GFreePartition(ppol, part)                GFreeP((ppol),(part))
    #define GFreeAllPartition(ppol)                   GFreeAllP((ppol))
    GAPI GBOOL  GCreatePPool(GPARTITIONPOOL_T *prPool, GVOID *pvSA, GUINT32 u4SZ, GUINT32 u4PartitionSZ);
    GAPI GVOID  GDeletePPool(GPARTITIONPOOL_T *prPool);
    GAPI GVOID *GAllocP(GPARTITIONPOOL_T *prPool);
    GAPI GVOID  GFreeP(GPARTITIONPOOL_T *prPool, GVOID *pvPartition);
    GAPI GVOID  GFreeAllP(GPARTITIONPOOL_T *prPool);
#endif //#ifdef GB_MEMORY_DIAGNOSE

#define GIsPartitionPoolUsed(ppol)                GIsPPoolUsed((ppol))
#define GGetFreePartitionNum(ppol)                GGetFreePcnt((ppol))
#define GSetPartionName(ppol, name)               GSetPPoolName((ppol), (name))
#define GGetPartitionPoolStat()                   GDumpPInfo(NULL)
GAPI GBOOL  GIsPPoolUsed(GPARTITIONPOOL_T *prPool);
GAPI GINT32 GGetFreePcnt(GPARTITIONPOOL_T *prPool);
GAPI GVOID  GSetPPoolName(GPARTITIONPOOL_T *prPool, GCHAR *pszName);
GAPI GVOID  GDumpPInfo(GWCHAR *wszFileName);



/**************************/
/* GDM - Dynamic Memory   */
/**************************/
/* Dynamic Heap API */
#if GB_MEMORY_DIAGNOSE
    #define GCreateMemoryPool(ppol, psa, sz)  GCreateHPool((ppol), (psa), (sz), (sz))
    #define GDeleteMemoryPool(ppol)           GDeleteHPool((ppol))
    #define GAllocMemory(ppol, sz)            GAllocH((ppol), (sz),__FILE__, __LINE__)
    #define GReAllocMemory(ppol, pmem, sz)    GAssert("GReAllocMemory forbidden", __FILE__, __LINE__)
    #define GFreeMemory(ppol, pmem)           GFreeH((ppol), (pmem), __FILE__, __LINE__)
    #define GAllocByTsk(u4Size)               GAllocByTask(u4Size, __FILE__, __LINE__)
    #define GFreeByTsk(pvBuf)                 GFreeByTask(pvBuf, __FILE__, __LINE__)
    GAPI GBOOL  GCreateHPool(GMEMORYPOOL_T *prPool, GVOID *pvSA, GUINT32 u4SZ, GUINT32 u4ExpMaxSZ);
    GAPI GVOID  GDeleteHPool(GMEMORYPOOL_T *prPool);
    GAPI GVOID *GAllocH(GMEMORYPOOL_T *prPool, GUINT32 u4SZ,const char *szFileName, GUINT32 u4LineNo);
    GAPI GVOID  GFreeH(GMEMORYPOOL_T *prPool, GVOID *pvMem, const char *szFileName, GUINT32 u4LineNo);
    GAPI GVOID *GReallocH(GMEMORYPOOL_T *prPool, GVOID *pvMem, GUINT32 u4SZ, const char *szFileName, GUINT32 u4LineNo);
    GAPI GUINT32 GGetSizeH(GVOID* pvMem);
    GVOID *GAllocByTask(GUINT32 u4Size, const char *szFileName, GUINT32 u4LineNo);
    GVOID GFreeByTask(GVOID *pvBuf, const char *szFileName, GUINT32 u4LineNo);
#else 
    #define GCreateMemoryPool(ppol, psa, sz)  GCreateHPool((ppol), (psa), (sz), (sz))
    #define GDeleteMemoryPool(ppol)           GDeleteHPool((ppol))
    #define GAllocMemory(ppol, sz)            GAllocH((ppol), (sz))
    #define GReAllocMemory(ppol, pmem, sz)    GAssert("GReAllocMemory forbidden", __FILE__, __LINE__)
    #define GFreeMemory(ppol, pmem)           GFreeH((ppol), (pmem))
    #define GAllocByTsk(u4Size)               GAllocByTask(u4Size)
    #define GFreeByTsk(pvBuf)                 GFreeByTask(pvBuf)
    GAPI GBOOL  GCreateHPool(GMEMORYPOOL_T *prPool, GVOID *pvSA, GUINT32 u4SZ, GUINT32 u4ExpMaxSZ);
    GAPI GVOID  GDeleteHPool(GMEMORYPOOL_T *prPool);    
    GAPI GVOID *GAllocH(GMEMORYPOOL_T *prPool, GUINT32 u4SZ);
    GAPI GVOID  GFreeH(GMEMORYPOOL_T *prPool, GVOID *pvMem);
    GAPI GVOID *GReallocH(GMEMORYPOOL_T *prPool, GVOID *pvMem, GUINT32 u4SZ);
    GVOID *GAllocByTask(GUINT32 u4Size);
    GVOID GFreeByTask(GVOID *pvBuf);
#endif // GB_MEMORY_DIAGNOSE

GVOID *GVirtualAlloc(GUINT32 u4Size, GUINT32 u4AllocType, GUINT32 u4Protect);
GVOID GVirtualFree(GVOID *pBuf, GUINT32 u4Size, GUINT32 u4FreeType);


#define GSetMemoryName(ppol, name)      GSetHPoolName((ppol), (name))


GAPI GUINT32 GGetNodeOverhd(GVOID* pvMem);
GAPI GVOID   GSetHPoolName(GMEMORYPOOL_T *prPool, GCHAR *pszName);
GAPI GVOID   GDumpHInfo(GWCHAR *wszFileName);

/****************************/
/* GCRT - C Run Time Memory */
/****************************/
/* Generic Clib memory APIs */
#if GB_MEMORY_DIAGNOSE && GB_CRTMEM_ADAPTOR
    #define GAlloc(sz)          GAllocC((sz), __FILE__, __LINE__)
    #define GRealloc(pmem, sz)  GAssert("GRealloc forbidden", __FILE__, __LINE__)
    #define GFree(pmem)         GFreeC((pmem), __FILE__, __LINE__)
    GAPI GVOID *GAllocC(GUINT32 u4SZ, const char *szFileName, GUINT32 u4LineNo);
    GAPI GVOID *GReallocC(GVOID *pvMem, GUINT32 u4NewSZ, const char *szFileName, GUINT32 u4LineNo);
    GAPI GVOID  GFreeC(GVOID *pvMem, const char *szFileName, GUINT32 u4LineNo);
    GAPI GUINT32 GGetSizeC(GVOID *pvMem);
#elif GB_CRTMEM_ADAPTOR
    #define GAlloc(sz)          GAllocC((sz))
    #define GRealloc(pmem, sz)  GAssert("GRealloc forbidden", __FILE__, __LINE__)
    #define GFree(pmem)         GFreeC((pmem))
    GAPI GVOID *GAllocC(GUINT32 u4SZ);
    GAPI GVOID *GReallocC(GVOID *pvMem, GUINT32 u4NewSZ);
    GAPI GVOID  GFreeC(GVOID *pvMem);
#else
    #define GAlloc              malloc
    #define GRealloc            realloc
    #define GFree               free
#endif //GB_MEMORY_DIAGNOSE



GAPI GBOOL GIsEngineTsk();//\\ JK,


/****************************/
/*   Memory Profiling APIs  */
/****************************/
GBOOL   GB_MemProf_Start(GVOID);
GBOOL   GB_MemProf_Stop(GVOID);
GVOID   GB_MemProf_Reset(GVOID);
GUINT32 GB_MemProf_Get(IN GUINT32 u4Mod);
GBOOL   GB_MemProf_QueryMod(IN GVOID *pvBuf, INOUT GUINT32 *pu4Idx);
GBOOL   GB_MemProf_QueryPool(IN GVOID *pvBuf, INOUT GUINT32 *pu4Idx);


/**********************************************************************/
/*            GBASE KAL MEMORY OPERATIONS ROUTINE                     */
/**********************************************************************/
GAPI GVOID* GMemSet(GVOID *pvDst, GINT32 iC, GUINT32 u4Cnt);
GAPI GVOID* GMemCpy(GVOID *pvDst, const GVOID *pvSrc, GUINT32 u4Cnt);
GAPI GINT32 GMemCmp(const GVOID *pBlock1, const GVOID *pBlock2, GUINT32 u4Cnt);
GAPI GVOID* GMemMove(GVOID *pvDst, const GVOID *pvSrc, GUINT32 u4Cnt);




/**********************************************************************/
/*            GBASE KAL TIMER SERVICE ROUNTINE                        */
/**********************************************************************/
typedef GVOID (*PFN_GTIMER)(GVOID *);  /* Timer Expiry Callback Function */

typedef struct
{
    GUINT32              u4TimerId;
    PFN_GTIMER           pfnExpiryFunc;
    GVOID                *pvParam;
    GUINT32              u4Duration;
    GUINT32              u4TickStart; /* Start Tick count of last timer. */
    GUINT8               fgEngTimer;
    GUINT8               au8Res[3];
  #if defined(MTK_FP)
    eventid              tKalTmEvtId;
  #elif defined(WIN32) || defined(_WIN32_WCE)
    GUINT32              tKalTmEvtId;
  #endif

  #if GB_DEBUG
    GCHAR                szCreatFile[32];
    GUINT32              u4CreatLine;
    GCHAR                szStartFile[32];
    GUINT32              u4StartLine;
  #endif
} GTIMER_T;

typedef enum
{
    TM_STATE_RUNNING    = 1,    /* timer is running */
    TM_STATE_STOP       = 2,    /* timer is just created or stopped */
    TM_STATE_UNCREATED  = 3,    /* timer is not created or has been deleted */
    TM_STATE_INVALID    = 4     /* timer is invalid */
} GTIMER_STATE_T;

//typedef GVOID (*PFN_TIMER_DISP_FUNC)(GTIMER_T *);

#if GB_DEBUG
  #define GCreateTimer(p1, p2, p3, p4)    GCreateTimerI(p1, p2, p3, p4, __FILE__, __LINE__)
  #define GCreateTimerUI(p1, p2, p3, p4)  GCreateTimerUII(p1, p2, p3, p4, __FILE__, __LINE__)
  #define GStartTimer(p1)                 GStartTimerI(p1, __FILE__, __LINE__)
  #define GTimerProc(p1, p2, p3, p4)      GTimerProcI(p1, p2, p3, p4, __FILE__, __LINE__)

  GAPI GRESULT GCreateTimerI(GTIMER_T *ptTcb, GUINT32 u4Duration, PFN_GTIMER pfnTimerCbk, GVOID *pvParam, GCHAR *szFile, GUINT32 u4Line);
  GAPI GRESULT GCreateTimerUII(GTIMER_T *ptTcb, GUINT32 u4Duration, PFN_GTIMER pfnTimerCbk, GVOID *pvParam, GCHAR *szFile, GUINT32 u4Line);
  GAPI GRESULT GStartTimerI(GTIMER_T *ptTcb, GCHAR *szFile, GUINT32 u4Line);
  GAPI GRESULT GTimerProcI(HWND hwnd, GUINT uMsg, UINT_PTR idEvent, GDWORD dwTime, GCHAR *szFile, GUINT32 u4Line);
#else
  #define GCreateTimer(p1, p2, p3, p4)    GCreateTimerI(p1, p2, p3, p4)
  #define GCreateTimerUI(p1, p2, p3, p4)  GCreateTimerUII(p1, p2, p3, p4)
  #define GStartTimer(p1)                 GStartTimerI(p1)
  #define GTimerProc(p1, p2, p3, p4)      GTimerProcI(p1, p2, p3, p4)

  GAPI GRESULT GCreateTimerI(GTIMER_T *ptTcb, GUINT32 u4Duration,  PFN_GTIMER pfnTimerCbk,  GVOID *pvParam);
  GAPI GRESULT GCreateTimerUII(GTIMER_T *ptTcb, GUINT32 u4Duration, PFN_GTIMER pfnTimerCbk, GVOID *pvParam);
  GAPI GRESULT GStartTimerI(GTIMER_T *ptTcb);
  GAPI GRESULT GTimerProcI(HWND hwnd, GUINT uMsg, UINT_PTR idEvent, GDWORD dwTime);
#endif

GAPI GRESULT GDeleteTimer(GTIMER_T *ptTcb);
GAPI GRESULT GStopTimer(GTIMER_T *ptTcb);
GAPI GRESULT GResetTimer(GTIMER_T *ptTcb, GUINT32 u4Duration, PFN_GTIMER pfnTimerCbk, GVOID *pvParam);
GAPI GTIMER_STATE_T GQueryTimerState(GTIMER_T *ptTcb);

/**********************************************************************/
/*            GBASE KAL DATE and TIME ROUNTINE                        */
/**********************************************************************/
typedef struct
{
   GUINT16 u2Year;     /**< Specifies the current year*/
   GUINT16 u2Month;    /**< Specifies the current month*/
   GUINT16 u2DayOfWeek;/**< Specifies the current day of the week;
                           Sunday = 0, Monday = 1, and so on */
   GUINT16 u2Day;      /**< Specifies the current day*/
   GUINT16 u2Hour;     /**< Specifies the current hour*/
   GUINT16 u2Minute;   /**< Specifies the current minute*/
   GUINT16 u2Second;   /**< Specifies the current second*/
   GUINT16 u2Padding;
} GDATETIME_T;

#define GGetSystemTime  GGetSysTime

GAPI GVOID   GGetSysTime(GDATETIME_T *ptDateTime);
GAPI GUINT32 GGetSysClock(GVOID);
GAPI GVOID   GGetSysTick(GUINT32 *pu4Ticks);
GAPI GVOID   GSetSysTime(GDATETIME_T *ptDateTime);
GAPI GVOID   GGetLocalTime(GDATETIME_T *ptDateTime);



/**********************************************************************/
/*            GBASE KAL TASK SERVICE ROUNTINE                         */
/**********************************************************************/
#ifdef WIN32
  #define GTASK_SZ              5 // sizeof(TASK_T) / sizeof(GUINT32)
#else
  #define GTASK_SZ              5 // ??? Unknow for which ???
#endif


typedef GVOID (*PFN_GTASK)(GVOID *);

typedef struct
{
  GUINT32    u4Dummy[GTASK_SZ]; /* placeholder */
} GTASK_T;


typedef enum
{   GTSK_CREATE_RUN         = 0,
    GTSK_CREATE_SUSPEND     = 1,
    GTSK_CREATE_BACKGROUND  = 2
} GTSK_CREATION_MODE_T;

#define KAL_CREATE_SUSPEND          GTSK_CREATE_SUSPEND
#define KAL_CREATE_RUN              GTSK_CREATE_RUN

GAPI GRESULT  GCreateTask(GTASK_T   *ptTask,
                          PFN_GTASK  pfnTaskEntry,
                           GVOID    *pvParam,
                           GVOID    *pvStackAddr, // NULL = allocate internally 
                           GUINT32   u4StackSize,
                          GTSK_CREATION_MODE_T eCreateMod);
GAPI GRESULT  GDeleteTask(GTASK_T *ptTask);
GAPI GVOID    GSuspendTask(GTASK_T *ptTask);
GAPI GRESULT  GResumeTask(GTASK_T *ptTask);
GAPI GRESULT  GSetTaskPriority(GTASK_T *ptTask, GUINT32 u4NewPri);
GAPI GINT32   GGetTaskId(GTASK_T *ptTask);
GAPI GUINT32  GGetCurrentTaskId(GVOID);
GAPI GTASK_T* GGetCurrentTask(GVOID);
GAPI GBOOL    GIsGISTask(GVOID);
GAPI GVOID    GSleep(GUINT32 u4MS);

GAPI GVOID    GTerminateThread(GTASK_T *ptTask);
GAPI GBOOL    GGetExitCodeThread(GTASK_T *ptTask, GUINT32* lpExitCode);


/**********************************************************************/
/*            GBASE KAL SYNCHRONIZATION SERVICE                       */
/**********************************************************************/
/***************************
 * Synchronization Service
 * (1) Critical Section
 * (2) Semaphore Mechanism
 * (3) Mutex Routines
 ***************************/
/* space holder definition */ 
#define GSYNC_SEM_SZ               1

#if defined (MTK_FP)
    #define GSYNC_MUX_SZ           32
#else
    #define GSYNC_MUX_SZ           1
#endif

#if defined(MTK_FP)
    #define GSYNC_CS_SZ            GSYNC_MUX_SZ
#else 
    #define GSYNC_CS_SZ            24 // must >= sizeof(CRITICAL_SECTION)=24B/4B
#endif // defined(WIN32)

#define GSM_DEFAULT_ATTRIBUTE      0x00
#define GSM_DEFAULT_MAXCOUNT       0xFF
#define GMU_WAIT_INFINITE          INFINITE

typedef enum
{   GSM_NOT_WAT        = 0,
    GSM_INFINITE_WAIT  = 1,
    GSM_USER_SPECIFY   = 2
} GSM_WAIT_MODE_T; /* the order must be synch with kal_wait_mode */


typedef struct
{ GUINT32  u4Dummy[GSYNC_CS_SZ];
} GCS_T;

typedef struct
{ GUINT32  u4Dummy[GSYNC_SEM_SZ];
} GSEM_T;

typedef struct
{ GUINT32  u4Dummy[GSYNC_MUX_SZ];
} GMUTEX_T;

/********************************/
/* SYNC - COMMON LOCK MACRO     */
/********************************/
#if defined(MTK_FP)
    #define GB_LOCK()       GEnterCritSect(NULL)
    #define GB_UNLOCK()     GLeaveCritSect(NULL)
#endif

/********************************/
/* GCS - Gbase Critical Section */
/********************************/

#define GEnterCritSect(pcs)         (S_OK == GEnterCritSectI((pcs), __FILE__, __LINE__))

GAPI GRESULT GInitCritSect(GCS_T *ptCS);
GAPI GRESULT GEnterCritSectI(GCS_T *ptCS, const GCHAR *pFileName, GUINT32 uLineNo);
GAPI GRESULT GLeaveCritSect(GCS_T *ptCS);
GAPI GRESULT GDeleteCritSect(GCS_T *ptCS);


/********************************/
/* GSM - Gbase SeMaphore        */
/********************************/
#define GCreateSem(psm, icnt)           (S_OK == GOpenSem((psm), NULL, (icnt), GSM_DEFAULT_MAXCOUNT))
#define GCreateSemEx(psm, icnt, mxcnt)  (S_OK == GOpenSem((psm), NULL, (icnt), (mxcnt)))
#define GTakeSem(psm, tmout)            (S_OK == GAcquireSem((psm), (tmout), GSM_USER_SPECIFY))
#define GTakeSemEx(psm, tmout, pret)    (S_OK == GAcquireSemEx((psm), (tmout), GSM_USER_SPECIFY, (pret)))
#define GGiveSem(psm)                   (S_OK == GReleaseSem((psm)))
#define GDeleteSem(psm)                 (S_OK == GCloseSem((psm)))

GAPI GRESULT GOpenSem(GSEM_T *ptSem, GCHAR *szSemName, GUINT32 u4InitCount, GUINT32 u4MaxCount);
GAPI GRESULT GAcquireSem(GSEM_T *ptSem, GUINT32 u4TimeOut, GSM_WAIT_MODE_T tWaitMode);
GAPI GRESULT GAcquireSemEx(GSEM_T *ptSem, GUINT32 u4TimeOut, GSM_WAIT_MODE_T tWaitMode, GUINT32 *pu4Ret);
GAPI GRESULT GReleaseSem(GSEM_T *ptSem);
GAPI GRESULT GCloseSem(GSEM_T *ptSem);


#define GCreateMutex(pmux)              (S_OK == GInitMutex((pmux), NULL) )
#define GDeleteMutex(pmux)              (S_OK == GDeinitMutex((pmux)))
#define GTakeMutex(pmux, tmout)         (S_OK == GWaitMutex((pmux), (tmout), __FILE__, __LINE__))
#define GGiveMutex(pmux)                (S_OK == GPostMutex((pmux)))

GAPI GRESULT GInitMutex(GMUTEX_T *ptMutex, GCHAR *szMuxName);
GAPI GRESULT GWaitMutex(GMUTEX_T *ptMutex, GUINT32 u4TimeOut, const GCHAR *pFileName, GUINT32 uLineNo);
GAPI GRESULT GPostMutex(GMUTEX_T *ptMutex);
GAPI GRESULT GDeinitMutex(GMUTEX_T *ptMutex);

GRESULT GInitMutexNP(IN GMUTEX_T *ptMutex,IN GCHAR *szMuxName);


/**********************************************************************/
/*            GBASE KAL DEBUG ROUNTINE                                */
/**********************************************************************/
#if defined(WIN32)
    #define GDebug  LOG_DEBUG
#else
    #define GDebug  LOG_DEBUG
#endif


/**********************************************************************/
/*            GBASE KAL BASIC ROUTINE                                 */
/**********************************************************************/
typedef struct 
{
    GVOID    *pvMemBuf; /* start address of memory buffer         */
    GUINT32   u4Sz1;    /* dedicate heap pool size                */
    GUINT32   u4Sz2;    /* crt pool size, partition included here */
  #if MTK_FP
    stack_timer_struct *pKalTimer;  /* Basic Timer for whole GIS (manager) */
    event_scheduler    *ptKalEvtSchd;
    kal_eventgrpid      tGisEvtId;
  #endif
} GB_KERNEL_T; /* kernel parameters configuration */

GAPI GVOID   GInitKal(GB_KERNEL_T *ptArg);
GAPI GVOID   GDeinitKal(GVOID);
GAPI GVOID   GAssert(GCHAR *szExpr, GCHAR* szFile, GUINT32 u4Line);

#ifdef __cplusplus
  }
#endif

#endif /* _GKAL_H_ */

