/************************************************************************/
/* FLYAUDIO CARECORD                                                    */
/************************************************************************/
#pragma once

#ifndef _FLYAUDIO_CARECORD_H_
#define _FLYAUDIO_CARECORD_H_

#include <windows.h>
#include <basetsd.h>
#include "flyshmlib.h"
#include "flygpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CURRENT_SHARE_MEMORY_ID	SHARE_MEMORY_DVR

enum enumRecordState{RECORD=0,PLAYBACK,TIME_SETTING,VIDEO_SETTING};
enum enumRecordResolution{HD=0,VGA,QVGA};
enum enumRecordLanguage{ENGLISH=0,SIMPLIFIED_CHINESE,TRADITIONAL_CHINESE,RUSSIAN};
enum enumRecordUpdataStatus{NO_UPDATA_FILE=0,UPDATAING,UPDATA_SUCCESS};

#define DATA_BUFF_LENGTH	    4096
#define DATA_BUFF_COMM_LENGTH	4096

typedef struct _CARECORD_INFO
{
	enumRecordState epreCarRecordState;
	enumRecordState ecurCarRecordState;
	enumRecordResolution epreRecordResolution;
	enumRecordResolution ecurRecordResolution;
	enumRecordLanguage epreRecordLanguage;
	enumRecordLanguage ecurRecordLanguage;
	BOOL bpreVideo_Recording;
	BOOL bcurVideo_Recording;
	BOOL bpreSound_Recording;
	BOOL bcurSound_Recording;
}CARECORD_INFO, *P_CARECORD_INFO;

typedef struct _FLY_CARECORD_INFO
{
	BOOL bopen;

	BOOL bHaveCarRecord;
	BYTE ExBoxInfoFrameStatus;
	ULONG iReceiveCarRecordMessageTime;

	BOOL bKillFlyCarRecordMainThread;
	HANDLE FlyCarRecordMainThreadHandle;
	HANDLE hFlyCarRecordMainThreadEvent;

	HANDLE hCarRecordCom;
	BOOL bKillFlyCarRecordComThread;
	HANDLE hFlyCarRecordComThreadHandle;

	BYTE Com_Data[20];
	BYTE Com_Data_Count;
	BYTE iComDataState;

	BYTE buffToUser[DATA_BUFF_LENGTH];
	UINT buffToUserHx;
	UINT buffToUserLx;
	HANDLE hBuffToUserDataEvent;
	CRITICAL_SECTION hCSSendToUser;

	CARECORD_INFO CarRecordInfo;

	//PFLY_DRIVER_GLOBAL pFlyDriverGlobalInfo;
	volatile shm_t *pFlyShmGlobalInfo;
	HANDLE hHandleGlobalGlobalEvent;
}FLY_CARECORD_INFO, *P_FLY_CARECORD_INFO;


/*
DEBUGMSG(..)����ֻ���ڹ�����Debugģʽ�±����Ժ�
�Ż��ӡ����Ϣ�����������Releaseģʽ�±���ģ��Ͳ����ӡ��Ϣ������

RETAILMSG(..)������Debugģʽ�º�Releaseģʽ�±��붼���ӡ��������Ϣ��
����������ڹ��̵������е�"Build option"��ѡ����"Enable ship build"��
��ôRETAILMSG�����Ͳ����ӡ��Ϣ�ˡ�
*/

#define MSG_ENABLE_FLAG (pFlyCarRecordInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (GLOBAL_DEBUG)
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



#endif// _FLYAUDIO_CARECORD_H_
