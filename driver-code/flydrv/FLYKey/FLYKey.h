/************************************************************************/
/* FLYAUDIO KEY                                                     */
/************************************************************************/
#pragma once

#ifndef _FLYAUDIO_KEY_H_
#define _FLYAUDIO_KEY_H_

#include <windows.h>
#include <basetsd.h>
#include "flyshmlib.h"
#include "flygpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define GLOBAL_COMM	1

#define MSG_USE_CRC	0

#define SHORT_KEY_MIN	100//50
#define KEY_SCAN_SPACE	30

#define  REMOTE_R_UP     2200
#define  REMOTE_R_BASE   122

#define STEEL_ADC_STUDY_DANCE		10	//ѧϰʱ���ƶ�����Χ
#define STEEL_ADC_STUDY_DISTANCE	50	//ѧϰʱ���ʼֵ����С���
#define STEEL_ADC_CHANGE_DANCE		40	//������ת������������ڶ���Χ
#define STEEL_ADC_CHANGE_MAX		1000//���������ٽ�ֵ
	typedef struct _IRKEY_TAB{
		BYTE IRKEY_Value[IRKEYTABSIZE]; //
		BYTE IRKEY_Port[IRKEYTABSIZE];	 //1 Remote1,  2 Remote2
		UINT IRKEY_AD_Min[IRKEYTABSIZE];
		UINT IRKEY_AD_Max[IRKEYTABSIZE];
		BYTE CarTypeID[2];
		BYTE size;		//��Ч�����̰�������
	}IRKEY_TAB, *P_IRKEY_TAB;

	typedef struct _IRKEY_STUDY_TAB{
		BYTE Value[IRKEYTABSIZE];
		BYTE Port[IRKEYTABSIZE];
		UINT AD[IRKEYTABSIZE];
	}IRKEY_STUDY_TAB, *P_IRKEY_STUDY_TAB;

	typedef struct _FLY_KEY_ENCODER_INFO
	{
		BYTE curEncodeValueLeft;
		BYTE curEncodeValueRight;

		UINT iEncoderLeftIncCount;
		UINT iEncoderLeftDecCount;
		UINT iEncoderRightIncCount;
		UINT iEncoderRightDecCount;

		HANDLE hInterruptEventL1;
		BOOL bInterruptEventL1Running;
		HANDLE hInterruptEventL2;
		BOOL bInterruptEventL2Running;
		HANDLE hInterruptEventR1;
		BOOL bInterruptEventR1Running;
		HANDLE hInterruptEventR2;
		BOOL bInterruptEventR2Running;
		HANDLE hDispatchThreadEncoderEvent;
	}FLY_KEY_ENCODER_INFO, *P_FLY_KEY_ENCODER_INFO;

#define DATA_BUFF_LENGTH	1024

	typedef struct _FLY_KEY_INFO
	{
		BOOL bOpen;
		BOOL bSpecialPower;

		FLY_KEY_ENCODER_INFO FlyKeyEncoderInfo;

		BOOL bKillDispatchKeyEncoderThread;
		HANDLE FlyKeyEncoderThreadHandle;

		BOOL bKillDispatchKeyADCThread;
		HANDLE FlyKeyADCThreadHandle;
		HANDLE hDispatchThreadADCEvent;

		HANDLE hMsgQueueFromKeyCreate;
		HANDLE hMsgQueueToKeyCreate;
		BOOL bKillDispatchKeyReadThread;
		HANDLE FlyKeyReadThreadHandle;

		BYTE buffToUser[DATA_BUFF_LENGTH];
		UINT buffToUserHx;
		UINT buffToUserLx;
		HANDLE hBuffToUserDataEvent;
		CRITICAL_SECTION hCSSendToUser;

		ULONG nowTimer;

		tPANEL_TAB tPanelTab;
		UINT CurrentAD[5];

		IRKEY_TAB remoteTab;
		UINT remote1ADCS;
		UINT remote2ADCS;

		IRKEY_STUDY_TAB remoteStudyTab;
		BOOL remoteStudyStart;
		BYTE remoteStudyStep;
		BYTE remoteStudyCount;
		UINT remoteStudyNormalAD1;
		UINT remoteStudyNormalAD2;

		UINT remoteStudyCurrent1;//������ѧϰ��
		UINT remoteStudyCurrent2;

		UINT iKeyDemoState;
		ULONG iKeyDemoInnerTime;

		HANDLE hHandleGlobalGlobalEvent;
		//PFLY_DRIVER_GLOBAL pFlyDriverGlobalInfo;
		volatile shm_t *pFlyShmGlobalInfo;
		
		//����ΰ���ź���
		HANDLE hToSuWeiKeyTestEvent;

		//AV������ʱ������
		ULONG iKeyAVFilter;

		//����6SMute��λ
		ULONG iMutePressTime;
	}FLY_KEY_INFO, *P_FLY_KEY_INFO;

	extern BYTE key_Tab_short[3][7];
	DWORD WINAPI ThreadFlyKeyADC(LPVOID pContext);
	void FlyKeyReturnToUser(P_FLY_KEY_INFO pKeyInfo,BYTE *buf,UINT len,BOOL bRomoteOrPanel);

	void panelKeyTabInit(P_FLY_KEY_INFO pKeyInfo);

	void remoteKeyInit(P_FLY_KEY_INFO pKeyInfo);

	void TestLEDBlink(P_FLY_KEY_INFO pKeyInfo);

	void SteelwheelStudyRegDataRead(P_FLY_KEY_INFO pKeyInfo);
	void SteelwheelStudyRegDataWrite(P_FLY_KEY_INFO pKeyInfo);

#ifdef __cplusplus
}
#endif

#define MSG_ENABLE_FLAG (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_KEY && GLOBAL_DEBUG)
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


#endif// _FLYAUDIO_KEY_H_