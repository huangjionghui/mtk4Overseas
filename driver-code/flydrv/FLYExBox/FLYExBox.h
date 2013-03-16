// -----------------------------------------------------------------------------
// File Name    : FLYExBox.h
// Title        : FLYExBox Driver
// Author       : Mootall - Copyright (C) 2011
// Created      : 2011-06-15  
// Version      : 0.01
// Target MCU   : WinCE Driver
// -----------------------------------------------------------------------------
// Version History:
/*
>>>
>>>
*/
// FLYExBox.h : Defines the entry point for the DLL application.
#pragma once

#ifndef _FLY_ExBox_H_
#define _FLY_ExBox_H_

#include <windows.h>
#include <basetsd.h>
#include "flyshmlib.h"
#include "flygpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct _FLY_ExBox_INFO
	{
		BOOL bExBoxEnable;

		BOOL bHaveExBox;
		ULONG ulExBoxSyncTimer;

		BOOL bKillDispatchFlyMainThread;
		HANDLE FlyMainThreadHandle;
		HANDLE hDispatchMainThreadEvent;

		HANDLE hExBoxComm;
		HANDLE hThreadHandleFlyExBoxComm;
		BYTE ExBoxInfoFrameStatus;

		HANDLE hMsgQueueTPMSToEXBOXCreate;
		HANDLE hMsgQueueExBoxToTMPSCreate;
		BOOL bKillDispatchFlyMsgQueueReadTPMSThread;
		HANDLE FLyMsgQueueReadTPMSThreadHandle;

		HANDLE hMsgQueueTVToEXBOXCreate;
		HANDLE hMsgQueueExBoxToTVCreate;
		BOOL bKillDispatchFlyMsgQueueReadTVThread;
		HANDLE FLyMsgQueueReadTVThreadHandle;

		//PFLY_DRIVER_GLOBAL pFlyDriverGlobalInfo;
		volatile shm_t *pFlyShmGlobalInfo;
		HANDLE hHandleGlobalGlobalEvent;

		BYTE bRevData[2];

	}FLY_EXBOX_INFO, *P_FLY_EXBOX_INFO;

/*
DEBUGMSG(..)函数只有在工程在Debug模式下编译以后，
才会打印出信息来。如果是在Release模式下编译的，就不会打印信息出来。

RETAILMSG(..)函数在Debug模式下和Release模式下编译都会打印出调试信息，
但是如果你在工程的属性中的"Build option"中选择了"Enable ship build"，
那么RETAILMSG函数就不会打印信息了。
*/

#define MSG_ENABLE_FLAG (pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_EXBOX && GLOBAL_DEBUG)
#define DBGE(string) RETAILMSG(1,string)//重要错误
#define DBGW(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string)//警告
#define DBGI(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string)//信息
#define DBGD(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string) //调试信息
#else
#define DBGE(string) RETAILMSG(1,string)//重要错误
#define DBGW(string) DEBUGMSG(1,string) //警告
#define DBGI(string) DEBUGMSG(1,string) //信息
#define DBGD(string) DEBUGMSG(1,string) //调试信息
#endif


#ifdef __cplusplus
}
#endif

#endif