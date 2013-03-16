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
	DEBUGMSG(..)����ֻ���ڹ�����Debugģʽ�±����Ժ�
	�Ż��ӡ����Ϣ�����������Releaseģʽ�±���ģ��Ͳ����ӡ��Ϣ������

	RETAILMSG(..)������Debugģʽ�º�Releaseģʽ�±��붼���ӡ��������Ϣ��
	����������ڹ��̵������е�"Build option"��ѡ����"Enable ship build"��
	��ôRETAILMSG�����Ͳ����ӡ��Ϣ�ˡ�
	*/

#define MSG_ENABLE_FLAG (pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_TV && GLOBAL_DEBUG)
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