/************************************************************************/
/* FLYAUDIO GLOBAL                                                     */
/************************************************************************/
#pragma once

#ifndef _FLYAUDIO_GLOBAL_H_
#define _FLYAUDIO_GLOBAL_H_


#include <windows.h>
#include <basetsd.h>
#include "flyshmlib.h"
#include "flygpio.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define MEM_SIZE_32_KB         	     (0x00008000)



#define DATA_BUFF_LENGTH	1024
#define CAN_WRITE_DATA      4096
#define WRITE_DATA          2048
#define READ_DATA           2048

#define GLOBAL_MEMORY_EVENT_COUNT	1	//����Ϊ0
#define GLOBAL_MEMORY_EVENT_SAVE	(((GLOBAL_MEMORY_EVENT_COUNT-1)/8)+1)	//�ֽ���

#define DEBUG_BUFF_LINE_LENGTH	256

enum enumGlobalMemoryChange{
GLOBAL_MEMORY_EVENT_VOLUME = 0	//��0��ʼ
};

#define OSD_DEBUG_WIDTH	80
#define OSD_DEBUG_LINES	16

#define OSD_DEBUG_COMPILE_TIME_ERROR		0
#define OSD_DEBUG_STATE_ERROR				2
#define OSD_DEBUG_KEYAD_LIST				4
#define OSD_DEBUG_SOFTVERSION				6
#define OSD_DEBUG_HOST_TEMPERATURE			8
#define OSD_DEBUG_BREAK_AND_PHONE_STATUS	10
#define OSD_DEBUG_INIT_OPEN_STATUS			12
#define OSD_DEBUG_OTHER_INFO				14

	typedef struct _SHARE_MEMORY_WRITE_BUF
	{
		BYTE iMainVolume;
	}SHARE_MEMORY_WRITE_BUF, *P_SHARE_MEMORY_WRITE_BUF;


	typedef struct _FLY_GLOBAL_INFO
	{
		BOOL bOpen;
		BOOL bPower;
		BOOL bPowerUp;
		BOOL bSpecialPower;

		HANDLE hMsgQueueToGlobalCreate;
		BOOL bKillDispatchFlyMsgQueueReadThread;
		HANDLE LEDTestMsgQueueReadThreadHandle;

		BOOL bKillDispatchFlyMainThread;
		HANDLE FlyMainThreadHandle;
		HANDLE hDispatchMainThreadEvent;

		BOOL bKillDispatchFlyLEDTestThread;
		HANDLE FlyLEDTestThreadHandle;
		HANDLE hDispatchLEDTestThreadEvent;

		BYTE buffToUser[DATA_BUFF_LENGTH];
		UINT buffToUserHx;
		UINT buffToUserLx;
		HANDLE hBuffToUserDataEvent;
		CRITICAL_SECTION hCSSendToUser;

		BOOL bKillDispatchFlyInterThread;
		HANDLE FlyInterThreadHandle;

		HANDLE hHandleGlobalGlobalEvent;

		HANDLE hHandleGlobalDVDEvent;
		HANDLE hHandleGlobalAudioEvent;
		HANDLE hHandleGlobalRadioEvent;
		HANDLE hHandleGlobalVideoEvent;
		HANDLE hHandleGlobalSystemEvent;
		HANDLE hHandleGlobalKeyEvent;
		HANDLE hHandleGlobalBTEvent;
		HANDLE hHandleGlobalExBoxEvent;
		HANDLE hHandleGlobalTVEvent;
		HANDLE hHandleGlobalTPMSEvent;
		HANDLE hHandleGlobalADEvent;
		HANDLE hHandleGlobalFAIIIEvent;

		//LPBYTE lpData;
		P_SHARE_MEMORY_WRITE_BUF lpData;
		BYTE  WriteDataBuf[WRITE_DATA];
		UINT  WriteDataBufLen;

		HANDLE hMemoryMap;
		HANDLE hOnWriteMemoryEvent;
		BOOL bMemoryMapHave;

		BYTE ReadDataBuf[READ_DATA];
		UINT ReadDataBufLen;

		HANDLE hOnReadMemoryEvent;
		HANDLE LEDTestShareMemoryReadThreadHandle;
		BOOL   bKillDispatchpLEDTestShareMemoryReadThread;

		BYTE debugParaName[DEBUG_BUFF_LINE_LENGTH];
		UINT32 debugParaValue;
		BOOL debugbReadParaName;
		UINT debugiParaLength;
		BOOL debugReadParaEnd;

		BYTE iGlobalMemoryEventWhat[GLOBAL_MEMORY_EVENT_SAVE];
		//PFLY_DRIVER_GLOBAL pFlyDriverGlobalInfo;
		volatile shm_t *pFlyShmGlobalInfo;

		BYTE sDemoStr[OSD_DEBUG_LINES][OSD_DEBUG_WIDTH];
		BYTE iDemoStrLength[OSD_DEBUG_LINES];

		UINT iOSDDemoStrRow;
		UINT iOSDDemoStrLength;

		BYTE *pReadPackageData;
	}FLY_GLOBAL_INFO, *P_FLY_GLOBAL_INFO;

	void debugParaReadFromFile(P_FLY_GLOBAL_INFO pGlobalInfo,BYTE iPath);

	void demoOSDHostTemperature(P_FLY_GLOBAL_INFO pGlobalInfo);
	void demoOSDDVDSoftVersion(P_FLY_GLOBAL_INFO pGlobalInfo);
	void demoOSDDriversCompileTime(P_FLY_GLOBAL_INFO pGlobalInfo);
	void demoOSDKeyADDisplay(P_FLY_GLOBAL_INFO pGlobalInfo);
	void demoOSDBreakAndPhoneStatus(P_FLY_GLOBAL_INFO pGlobalInfo);
	void demoOSDStateError(P_FLY_GLOBAL_INFO pGlobalInfo);
	void demoOSDOpenStatus(P_FLY_GLOBAL_INFO pGlobalInfo);
	void demoOSDOtherInfo(P_FLY_GLOBAL_INFO pGlobalInfo);

#ifdef __cplusplus
}
#endif

/*
DEBUGMSG(..)����ֻ���ڹ�����Debugģʽ�±����Ժ�
�Ż��ӡ����Ϣ�����������Releaseģʽ�±���ģ��Ͳ����ӡ��Ϣ������

RETAILMSG(..)������Debugģʽ�º�Releaseģʽ�±��붼���ӡ��������Ϣ��
����������ڹ��̵������е�"Build option"��ѡ����"Enable ship build"��
��ôRETAILMSG�����Ͳ����ӡ��Ϣ�ˡ�
*/

#define MSG_ENABLE_FLAG (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_GLOBAL && GLOBAL_DEBUG)
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


static VOID OnReadMemory(P_FLY_GLOBAL_INFO pGlobalInfo);
static DWORD WINAPI LEDTestShareMemoryReadThread(LPVOID pContex);

#endif// _FLYAUDIO_GLOBAL_H_