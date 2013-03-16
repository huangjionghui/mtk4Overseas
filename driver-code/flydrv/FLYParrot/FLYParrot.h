/***************************************************************************************************
**�����������ɸ�7500�°�
**���������� 
**�ļ����ƣ� FLYParrot.h
**�汾��Ϣ�� 
**����ʱ�䣺 2011-05-13
**������  �� 
**--------------------------------------------------------------------------------------------------
**--------------------------------------------------------------------------------------------------
**		�޸��ˣ�								�޸�ʱ�䣺				�޸����ݣ�
** 1�� 
** 2��
** 3��
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

		//Init��ʼ��
		BOOL bKillParrotBTMainThread;
		HANDLE hParrotBTMainThread;
		HANDLE hParrotBTMainThreadEvent;

		UINT iBTInfoFrameStatus;
		UINT iFrameLenMax;
		UINT iFrameLen;
		BYTE iChecksum;
		BYTE ReceiveBuf[DATA_BUFF_LENGTH];


		//����restartʱ��ʼ��
		HANDLE hBTComm;
		BOOL bKillFlyBTCommThread;
		HANDLE hThreadHandleFlyBTComm;

		//������Դ����
		BOOL controlPower;
		BOOL currentPower;


		HANDLE hHandleGlobalGlobalEvent;
		volatile shm_t *pFlyShmGlobalInfo;

	}FLY_PARROT_INFO, *P_FLY_PARROT_INFO;


#define MSG_ENABLE_FLAG (pFlyParrotInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_BT && GLOBAL_DEBUG)
#define DBGE(string) RETAILMSG(1,string)//��Ҫ����
#define DBGW(string) if (1) RETAILMSG(1,string)//����
#define DBGI(string) if (1) RETAILMSG(1,string)//��Ϣ
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