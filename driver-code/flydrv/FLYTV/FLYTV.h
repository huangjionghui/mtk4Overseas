#pragma once

#ifndef _FLY_TV_H_
#define _FLY_TV_H_


#include <windows.h>
#include <basetsd.h>

#include "flyshmlib.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define DATA_BUFF_LENGTH	1024


	typedef struct _FLY_TV_INFO
	{
		BOOL bOpen;
		BOOL bPower;
		BOOL bSpecialPower;

		UINT	iBuffToUserLx;
		UINT	iBuffToUserHx;
		BYTE	BuffToUser[DATA_BUFF_LENGTH];
		HANDLE	hBuffToUserDataEvent;
		UINT	buffToDriverProcessorStatus;
		CRITICAL_SECTION hCSSendToUser;

		HANDLE hMsgQueueTVToExBoxCreate;
		HANDLE hMsgQueueExBoxToTVCreate;
		BOOL bKillDispatchFlyMsgQueueEXBOXReadThread;
		HANDLE FLyMsgQueueEXBOXReadThreadHandle;

		BOOL bKillDispatchFlyTVThread;
		HANDLE hThreadHandleFlyTV;
		HANDLE hDispatchThreadTVEvent;

		BOOL bHavaTV;

		//PFLY_DRIVER_GLOBAL pFlyDriverGlobalInfo;
		volatile shm_t *pFlyShmGlobalInfo;
		HANDLE hHandleGlobalGlobalEvent;

	}FLY_TV_INFO, *P_FLY_TV_INFO;


	/*
	DEBUGMSG(..)函数只有在工程在Debug模式下编译以后，
	才会打印出信息来。如果是在Release模式下编译的，就不会打印信息出来。

	RETAILMSG(..)函数在Debug模式下和Release模式下编译都会打印出调试信息，
	但是如果你在工程的属性中的"Build option"中选择了"Enable ship build"，
	那么RETAILMSG函数就不会打印信息了。
	*/

#define MSG_ENABLE_FLAG (pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_TV && GLOBAL_DEBUG)
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