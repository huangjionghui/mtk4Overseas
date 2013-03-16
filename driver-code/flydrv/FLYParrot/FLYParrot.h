/***************************************************************************************************
**开发环境：飞歌7500新板
**功能描述： 
**文件名称： FLYParrot.h
**版本信息： 
**创建时间： 2011-05-13
**创建人  ： 
**--------------------------------------------------------------------------------------------------
**--------------------------------------------------------------------------------------------------
**		修改人：								修改时间：				修改内容：
** 1、 
** 2、
** 3、
****************************************************************************************************/
#pragma once

#ifndef _FLY_PARROT_H_
#define _FLY_PARROT_H_


#include "flyshmlib.h"

#if !SOFT_VERSION_FOR_OVERSEAS
#error "This software version is for china!!!"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define DATA_BUFF_TOUSER_LENGTH 1024
#define DATA_BUFF_LENGTH 1024
	typedef struct _FLY_PARROT_INFO
	{
		BOOL bOpen;
		BOOL bPower;
		BOOL bSpecialPower;
		BOOL bPowerUp;

		//Init初始化
		BOOL bKillParrotBTMainThread;
		HANDLE hParrotBTMainThread;
		HANDLE hParrotBTMainThreadEvent;

		UINT iBTInfoFrameStatus;
		UINT iFrameLenMax;
		UINT iFrameLen;
		BYTE iChecksum;
		BYTE ReceiveBuf[DATA_BUFF_LENGTH];


		//蓝牙restart时初始化
		HANDLE hBTComm;
		BOOL bKillFlyBTCommThread;
		HANDLE hThreadHandleFlyBTComm;

		//蓝牙电源控制
		BOOL controlPower;
		BOOL currentPower;


		HANDLE hHandleGlobalGlobalEvent;
		volatile shm_t *pFlyShmGlobalInfo;

	}FLY_PARROT_INFO, *P_FLY_PARROT_INFO;


#define MSG_ENABLE_FLAG (pFlyParrotInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_BT && GLOBAL_DEBUG)
#define DBGE(string) RETAILMSG(1,string)//重要错误
#define DBGW(string) if (1) RETAILMSG(1,string)//警告
#define DBGI(string) if (1) RETAILMSG(1,string)//信息
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