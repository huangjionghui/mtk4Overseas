/************************************************************************/
/* FLYAUDIO CARBODY                                                     */
/************************************************************************/
#pragma once

#ifndef _FLYAUDIO_CARBODY_H_
#define _FLYAUDIO_CARBODY_H_

#include <windows.h>
#include <basetsd.h>
#include "flyshmlib.h"
#include "flygpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MCU_ADDR_R				0xA1
#define MCU_ADDR_W				0xA0

#define MCU_I2C_FREQ   100000      //50Khz

#define DATA_BUFF_LENGTH_FROM_MCU		256	//立即处理

	typedef struct _FLY_CARBODY_INFO
	{
		BOOL bCarbodyEnable;

		BOOL bKillDispatchFlyMainThread;
		HANDLE FlyMainThreadHandle;
		HANDLE hDispatchMainThreadEvent;

		HANDLE hMsgQueueToGlobalCreate;

		HANDLE hMsgQueueToKeyCreate;
		HANDLE hMsgQueueFromKeyCreate;
		BOOL bKillDispatchFlyMsgQueueKeyReadThread;
		HANDLE FLyMsgQueueKeyReadThreadHandle;

		HANDLE hMsgQueueToRadioCreate;

		HANDLE hMsgQueueToSystemCreate;
		HANDLE hMsgQueueFromSystemCreate;
		BOOL bKillDispatchFlyMsgQueueSystemReadThread;
		HANDLE FLyMsgQueueSystemReadThreadHandle;
//AC
		HANDLE hMsgQueueToAcCreate;
		HANDLE hMsgQueueFromAcCreate;
		BOOL bKillDispatchFlyMsgQueueAcReadThread;
		HANDLE FLyMsgQueueAcReadThreadHandle;
//FAIII
		HANDLE hMsgQueueToFaiiiCreate;
		HANDLE hMsgQueueFromFaiiiCreate;
		BOOL bKillDispatchFlyMsgQueueFaiiiReadThread;
		HANDLE FLyMsgQueueFaiiiReadThreadHandle;
//FLYAUDIO AMP
		HANDLE hMsgQueueToExtAmpCreate;
		HANDLE hMsgQueueFromExtAmpCreate;
		BOOL bKillDispatchFlyMsgQueueExtAmpReadThread;
		HANDLE FLyMsgQueueExtAmpReadThreadHandle;
//CAR AMP
		HANDLE hMsgQueueToCarAmpCreate;
		HANDLE hMsgQueueFromCarAmpCreate;
		BOOL bKillDispatchFlyMsgQueueCarAmpReadThread;
		HANDLE FLyMsgQueueCarAmpReadThreadHandle;
//小屏显示
		HANDLE hMsgQueueFromAssistDisplayCreate;
		BOOL bKillDispatchFlyMsgQueueAssistDisplayReadThread;
		HANDLE FLyMsgQueueAssistDisplayReadThreadHandle;

		//HANDLE hFlyCarbodyIIC;
		HANDLE hFlyCarbodySerial;

		BOOL bKillDispatchFlyCarbodyCommThread;
		HANDLE hThreadHandleFlyCarbodyComm;
		HANDLE hThreadHandleFlyCarbodyCommEvent;

		BYTE buffFromMCU[DATA_BUFF_LENGTH_FROM_MCU];
		BYTE buffFromMCUProcessorStatus;
		UINT buffFromMCUFrameLength;
		UINT buffFromMCUFrameLengthMax;
		BYTE buffFromMCUCRC;
		BYTE buffFromMCUBak[DATA_BUFF_LENGTH_FROM_MCU];

		HANDLE hInterruptEventIsrPin;
		BOOL bInterruptIsrPinProcRunning;

		BYTE buffLpcFrame[300];

		//PFLY_DRIVER_GLOBAL pFlyDriverGlobalInfo;
		volatile shm_t *pFlyShmGlobalInfo;
		HANDLE hHandleGlobalGlobalEvent;

		BOOL xxxxxxxxStarting;

		BOOL bHaveRecPowerOffOrReset;
	}FLY_CARBODY_INFO, *P_FLY_CARBODY_INFO;

/*
DEBUGMSG(..)函数只有在工程在Debug模式下编译以后，
才会打印出信息来。如果是在Release模式下编译的，就不会打印信息出来。

RETAILMSG(..)函数在Debug模式下和Release模式下编译都会打印出调试信息，
但是如果你在工程的属性中的"Build option"中选择了"Enable ship build"，
那么RETAILMSG函数就不会打印信息了。
*/

#define MSG_ENABLE_FLAG (pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_CARBODY && GLOBAL_DEBUG)
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



#endif// _FLYAUDIO_CARBODY_H_