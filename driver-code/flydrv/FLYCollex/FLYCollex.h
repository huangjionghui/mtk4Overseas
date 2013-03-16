/************************************************************************/
/* FLYAUDIO KEY                                                     */
/************************************************************************/
#pragma once

#ifndef _FLYAUDIO_COLLEX_H_
#define _FLYAUDIO_COLLEX_H_

#include <windows.h>
#include <basetsd.h>

#include "flyshmlib.h"


#define BT_VERSION "BT-V1.1"

#ifdef __cplusplus
extern "C"
{
#endif

#define COLLEX_AUDIO_TRANS_USE_COLLEX	0

#define COLLEX_PHONE_NUMBER	64
#define COLLEX_PHONE_NAME	64
	typedef struct _COLLEX_BT_PHONE_LIST
	{
		BYTE iWhichPhoneList;
		BYTE cPhoneNumber[COLLEX_PHONE_NUMBER];
		UINT iPhoneNumberLength;
		BYTE iPhoneType;
		BYTE cPhoneName[COLLEX_PHONE_NAME];
		UINT iPhoneNameLength;
		BYTE cPhoneNumberName[COLLEX_PHONE_NUMBER+COLLEX_PHONE_NAME];
		UINT iPhoneNumberNameLength;
		_COLLEX_BT_PHONE_LIST *Next;
	}COLLEX_BT_PHONE_LIST, *P_COLLEX_BT_PHONE_LIST;

	typedef struct _COLLEX_BT
	{
		BOOL bWork;

		BYTE iControlReqStep;

		BOOL bPaired;
		BOOL bConnected;

		BYTE iPairedStatus;

		BOOL _W_bPairing;

		BYTE iPairedDeviceType;
		BYTE BDAddress[6];

		BYTE sVersion[8];

		BYTE sDeviceName[COLLEX_PHONE_NAME];
		UINT iDeviceNameLength;

		BYTE sDeviceOldName[COLLEX_PHONE_NAME];
		UINT iDeviceOldNameLength;

		BYTE sWaitingCallNumber[COLLEX_PHONE_NAME];
		UINT iWaitingCallLen;
		UINT iWaitingCallType;
		BYTE sCallInPhoneNumber[COLLEX_PHONE_NUMBER];

		UINT iCallNumberBufflen;
		BYTE cCallNumberBuff[COLLEX_PHONE_NUMBER];

		BYTE cMobileCallStatus;

		BOOL bAudioConnectionStatus;//�����л�ͨ��
		ULONG iAudioConnectionStatusTime;

		BOOL bStereoDeviceConnection;

		BYTE mobileBattery;
		BYTE mobileSignal;
		BYTE mobileVolume;

		BYTE iPhoneListType;
		BYTE _W_iPhoneListType;
		BOOL bPhoneListMobileReturn;
		UINT iPhoneListMobileReturnCount;
		BOOL bPhoneListStartReturn;
		UINT iPhoneListStart;
		UINT iPhoneListReturnCount;
		UINT iPhoneListReturnCurrent;
		P_COLLEX_BT_PHONE_LIST pBTPhoneList;

		BOOL bPhoneListNeedReturnFlush;
		BOOL bPhoneListPhoneReadFinish[7];
	}COLLEX_BT, *P_COLLEX_BT;

#define DATA_BUFF_LENGTH	2048
#define DATA_COMM_BUFF_LENGTH	4096

//#define DATA_READ_HANDLE_NAME	TEXT("FLYBT_DATA_READ_EVENT")

typedef struct _FLY_COLLEX_BT_INFO
{
	BOOL bOpen;
	BOOL bPower;
	BOOL bSpecialPower;
	BOOL bPowerUp;
	BOOL bSocketConnecting;

	BOOL bUpdater;
	ULONG iAutoResetControlTime;

	//Init��ʼ��
	BOOL bKillCollexBTMainThread;
	HANDLE hCollexBTMainThread;
	HANDLE hCollexBTMainThreadEvent;

	BYTE buffToUser[DATA_BUFF_LENGTH];
	UINT buffToUserHx;
	UINT buffToUserLx;
	HANDLE hBuffToUserDataEvent;
	CRITICAL_SECTION hCSSendToUser;

	//����restartʱ��ʼ��
	HANDLE hBTComm;
	BOOL bKillFlyBTCommThread;
	HANDLE hThreadHandleFlyBTComm;
	UINT BTInfoFrameStatus;
	//BYTE BTInfoFrameBuff[DATA_COMM_BUFF_LENGTH];

	COLLEX_BT sCollexBTInfo;

	//������Դ����
	BOOL controlPower;
	BOOL currentPower;

	UINT iControlAudioMuteTimeoutCount;
	ULONG iControlAudioMuteTime;
	BOOL bControlAudioMuteLongTime;
	ULONG iBTSelfReturnHandDownTime;

	HANDLE hHandleGlobalGlobalEvent;
	volatile shm_t *pFlyShmGlobalInfo;
}FLY_COLLEX_BT_INFO, *P_FLY_COLLEX_BT_INFO;


/*
DEBUGMSG(..)����ֻ���ڹ�����Debugģʽ�±����Ժ�
�Ż��ӡ����Ϣ�����������Releaseģʽ�±���ģ��Ͳ����ӡ��Ϣ������

RETAILMSG(..)������Debugģʽ�º�Releaseģʽ�±��붼���ӡ��������Ϣ��
����������ڹ��̵������е�"Build option"��ѡ����"Enable ship build"��
��ôRETAILMSG�����Ͳ����ӡ��Ϣ�ˡ�
*/

#define MSG_ENABLE_FLAG (pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_BT && GLOBAL_DEBUG)
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



#endif// _FLYAUDIO_COLLEX_H_