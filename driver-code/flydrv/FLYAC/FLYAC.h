/************************************************************************/
/* FLYAUDIO AC                                              */
/************************************************************************/
#pragma once

#ifndef _FLYAUDIO_AC_H_
#define _FLYAUDIO_AC_H_

#include <windows.h>
#include <basetsd.h>
#include "flyshmlib.h"
#include "flygpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define DATA_BUFF_LENGTH	1024


	typedef struct _FLY_AC_INFO
	{
		BOOL bOpen;
		BOOL bPower;
		BOOL bPowerUp;
		BOOL bSpecialPower;
		BOOL bUserPowerUp;

		BOOL bKillDispatchFlyMainThread;
		HANDLE FlyMainThreadHandle;
		HANDLE hDispatchMainThreadEvent;

		BYTE buffToUser[DATA_BUFF_LENGTH];
		UINT buffToUserHx;
		UINT buffToUserLx;
		HANDLE hBuffToUserDataEvent;
		CRITICAL_SECTION hCSSendToUser;

		HANDLE hMsgQueueFromAcCreate;
		HANDLE hMsgQueueToAcCreate;
		BOOL bKillDispatchFlyMsgQueueReadThread;
		HANDLE FLyMsgQueueReadThreadHandle;

		BOOL bKillDispatchFlyInterThread;
		HANDLE FLyInterThreadHandle;
		HANDLE hDispatchInterThreadEvent;

		//PFLY_DRIVER_GLOBAL pFlyDriverGlobalInfo;
		volatile shm_t *pFlyShmGlobalInfo;
		HANDLE hHandleGlobalGlobalEvent;

		BYTE iSendToMCULength[256];
		BYTE iSendToMCUBuff[256][256];
	}FLY_AC_INFO, *P_FLY_AC_INFO;

#define MSG_ENABLE_FLAG (pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_AC && GLOBAL_DEBUG)
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



#endif// _FLYAUDIO_AC_H_