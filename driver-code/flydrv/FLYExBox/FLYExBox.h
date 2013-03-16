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
DEBUGMSG(..)����ֻ���ڹ�����Debugģʽ�±����Ժ�
�Ż��ӡ����Ϣ�����������Releaseģʽ�±���ģ��Ͳ����ӡ��Ϣ������

RETAILMSG(..)������Debugģʽ�º�Releaseģʽ�±��붼���ӡ��������Ϣ��
����������ڹ��̵������е�"Build option"��ѡ����"Enable ship build"��
��ôRETAILMSG�����Ͳ����ӡ��Ϣ�ˡ�
*/

#define MSG_ENABLE_FLAG (pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_EXBOX && GLOBAL_DEBUG)
#define DBGE(string) RETAILMSG(1,string)//��Ҫ����
#define DBGW(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string)//����
#define DBGI(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string)//��Ϣ
#define DBGD(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string) //������Ϣ
#else
#define DBGE(string) RETAILMSG(1,string)//��Ҫ����
#define DBGW(string) DEBUGMSG(1,string) //����
#define DBGI(string) DEBUGMSG(1,string) //��Ϣ
#define DBGD(string) DEBUGMSG(1,string) //������Ϣ
#endif


#ifdef __cplusplus
}
#endif

#endif