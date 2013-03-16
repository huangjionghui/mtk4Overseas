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

#define DATA_BUFF_LENGTH_FROM_MCU		256	//��������

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
//С����ʾ
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
DEBUGMSG(..)����ֻ���ڹ�����Debugģʽ�±����Ժ�
�Ż��ӡ����Ϣ�����������Releaseģʽ�±���ģ��Ͳ����ӡ��Ϣ������

RETAILMSG(..)������Debugģʽ�º�Releaseģʽ�±��붼���ӡ��������Ϣ��
����������ڹ��̵������е�"Build option"��ѡ����"Enable ship build"��
��ôRETAILMSG�����Ͳ����ӡ��Ϣ�ˡ�
*/

#define MSG_ENABLE_FLAG (pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_CARBODY && GLOBAL_DEBUG)
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



#endif// _FLYAUDIO_CARBODY_H_