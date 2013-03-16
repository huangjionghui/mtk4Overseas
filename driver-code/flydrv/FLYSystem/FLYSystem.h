/************************************************************************/
/* FLYAUDIO SYSTEM                                                     */
/************************************************************************/
#pragma once

#ifndef _FLYAUDIO_SYSTEM_H_
#define _FLYAUDIO_SYSTEM_H_

#include <windows.h>
#include <basetsd.h>

#include "flyshmlib.h"
#include "flygpio.h"



#ifdef __cplusplus
extern "C"
{
#endif

//查询AUX是否有信号命名事件
#define TO_VIDEO_SIGNAL_TEST_EVENT_NAME L"TO_VIDEO_SIGNAL_TEST_EVENT_NAME"
#define EVENT_BACKCAR_LISTEN_THREAD	TEXT("FLY_EVENT_NAME_BACKCAR_LISTEN_THREAD")

#define FROM_3360MUTE_EVENT L"AUD_MUTE_EVENT"
enum 
{ 
	FRONT_SEAT = 0x0, 
	REAR_SEAT  = 4, 
	GPS_SEAT   = 8 
}; 
enum 
{ 
	MUTE_STATE   = 0x0, 
	UNMUTE_STATE,  
}; 

#define CHECK_SHELL_BABY_INNER_TIME	(61800*10)
#define DATA_BUFF_LENGTH	1024
#define LOW_VOLTAGE_DELAY	1500

	typedef struct _FLY_SILENCE_POWEROFF_INFO
	{
		BOOL bReadRegData;
		SYSTEMTIME timeFirstPowerOn;
		SYSTEMTIME timeLastUserAccOff;//最后一次正常关机
	}FLY_SILENCE_POWEROFF_INFO, *P_FLY_SILENCE_POWEROFF_INFO;

	typedef struct _FLY_SYSTEM_CARBODY_INFO
	{
		BYTE preLightBrightDuty;//LCD背光亮度
		BYTE tmpLightBrightDuty;
		BYTE curLightBrightDuty;

		DWORD iProcVoltageShakeDelayTime;

		ULONG iBreakDetectDelayRead;
		ULONG iPhoneDetectDelayRead;

		ULONG iSendShellBabyTime;
		UINT32 iShellBabySend;
		UINT32 iShellBabyRec;

		BYTE iFanPWMDuty;

		BYTE iLEDBlinkOnWhat;
		BYTE iLEDBlinkOnWhatSub;
		ULONG iLEDBlinkOnTime;
	}FLY_SYSTEM_CARBODY_INFO, *P_FLY_SYSTEM_CARBODY_INFO;

	typedef struct _FLY_SYSTEM_INFO
	{
		BOOL bOpen;
		BOOL bPower;
		BOOL bSpecialPower;
		BOOL bPowerUp;
		BOOL bUserPowerUp;
		BOOL bSocketConnecting;

		BOOL bKillDispatchFlyMainThread;
		HANDLE FlyMainThreadHandle;
		HANDLE hDispatchMainThreadEvent;

		BOOL bNeedReserveSteelSelectData;

		BYTE buffToUser[DATA_BUFF_LENGTH];
		UINT buffToUserHx;
		UINT buffToUserLx;
		HANDLE hBuffToUserDataEvent;
		CRITICAL_SECTION hCSSendToUser;

		HANDLE hMsgQueueToSystemCreate;
		HANDLE hMsgQueueFromSystemCreate;
		BOOL bKillDispatchFlyMsgQueueReadThread;
		HANDLE FLyMsgQueueReadThreadHandle;

		HANDLE hFrom3360MuteEventHandle;
		HANDLE hFrom3360MuteThreadHandle;
		BOOL   bFrom3360Mute;

		BOOL bKillDispatchFlyInterThread;
		HANDLE FLyInterThreadHandle;
		HANDLE hDispatchInterThreadEvent;

		BOOL bBreakDetectIsrRunning;
		HANDLE hBreakDetectIsrEvent;

		BOOL bPhoneDetectIsrRunning;
		HANDLE hPhoneDetectIsrEvent;

		FLY_SYSTEM_CARBODY_INFO SystemCarbodyInfo;

		FLY_SILENCE_POWEROFF_INFO SilencePowerOffInfo;

		//PFLY_DRIVER_GLOBAL pFlyDriverGlobalInfo;
		volatile shm_t *pFlyShmGlobalInfo;
		HANDLE hHandleGlobalGlobalEvent;

		BOOL bRecRegeditRestoreMsg;
		BYTE iRecRegeditRestoreWhat;

		BOOL bStandbyStatusWithACCOff;
		BOOL bStandbyStatus;

		ULONG iProcACCOffTime;

		BOOL  bRegKeepFinish;
		
		BOOL  bAccOffBreakRecvForUser;
		BOOL  bBackDetectEnable;
		ULONG dwBackDetectTimer;
		HANDLE hBackDetectEnableEvent;

		BOOL bUserInitFinishStatus;

		//给VIDEO.DLL信号
		HANDLE hToVideoSignalTestEvent;
		BOOL  bNeedSendBackStatusToUser;

		//给素伟的信号量
		HANDLE hToSuWeiPowerDownReqEvent;
		HANDLE hToSuWeiPowerDownAckEvent;
		HANDLE hToSuWeiPowerDownRecEvent;
	}FLY_SYSTEM_INFO, *P_FLY_SYSTEM_INFO;

	void controlToMCUPowerToVeryLowOff(P_FLY_SYSTEM_INFO pSystemInfo);

	void resetPowerOn(P_FLY_SYSTEM_INFO pSystemInfo);
	void resetPowerOff(P_FLY_SYSTEM_INFO pSystemInfo);
	void resetPowerUp(P_FLY_SYSTEM_INFO pSystemInfo);
	void resetPowerDown(P_FLY_SYSTEM_INFO pSystemInfo);
	DWORD resetReturnNextResetTime(P_FLY_SYSTEM_INFO pSystemInfo);
	void resetPowerProcOnRecMCUWakeup(P_FLY_SYSTEM_INFO pSystemInfo);


#ifdef __cplusplus
}
#endif


#define MSG_ENABLE_FLAG (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_SYSTEM && GLOBAL_DEBUG)
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



#endif// _FLYAUDIO_SYSTEM_H_