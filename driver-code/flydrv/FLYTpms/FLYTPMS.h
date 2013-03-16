#pragma once

#ifndef _FLY_TPMS_H_
#define _FLY_TPMS_H_


#include <windows.h>
#include <basetsd.h>

#include "flyshmlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define DATA_BUFF_LENGTH	1024


	typedef struct _FLY_TPMS_INFO
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

		HANDLE hMsgQueueTPMSToExBoxCreate;
		HANDLE hMsgQueueExBoxToTPMSCreate;
		BOOL bKillDispatchFlyMsgQueueEXBOXReadThread;
		HANDLE FLyMsgQueueEXBOXReadThreadHandle;

		BOOL bKillDispatchFlyTPMSThread;
		HANDLE hThreadHandleFlyTPMS;
		HANDLE hDispatchThreadTPMSEvent;

		BOOL bHavaTMPS;
		BOOL bPrePairStatus;
		BOOL bCurPairStatus;
		
		//PFLY_DRIVER_GLOBAL pFlyDriverGlobalInfo;
		volatile shm_t *pFlyShmGlobalInfo;
		HANDLE hHandleGlobalGlobalEvent;

	}FLY_TPMS_INFO, *P_FLY_TPMS_INFO;

/*
DEBUGMSG(..)函数只有在工程在Debug模式下编译以后，
才会打印出信息来。如果是在Release模式下编译的，就不会打印信息出来。

RETAILMSG(..)函数在Debug模式下和Release模式下编译都会打印出调试信息，
但是如果你在工程的属性中的"Build option"中选择了"Enable ship build"，
那么RETAILMSG函数就不会打印信息了。
*/

#define MSG_ENABLE_FLAG (pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_TPMS && GLOBAL_DEBUG)
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

#endif// _FLYAUDIO_KEY_H_