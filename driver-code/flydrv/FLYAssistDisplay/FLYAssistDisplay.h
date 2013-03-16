/************************************************************************/
/* FLYAUDIO ASSISTDISPLAY                                               */
/************************************************************************/
#pragma once

#ifndef _FLYAUDIO_ASSISTDISPLAY_H_
#define _FLYAUDIO_ASSISTDISPLAY_H_

#include "flyshmlib.h"

#define DATA_BUFF_COMM_LENGTH 1024
#ifdef __cplusplus
extern "C"
{
#endif

#define DATA_BUFF_LENGTH	1024
#define DATA_COMM_BUFF_LENGTH 1024

	enum eAssistDisplayChannel{eAD_Init=0,eAD_DVD,eAD_Radio,eAD_iPod,eAD_AUX,eAD_TV,eAD_MP3,eAD_XM,eAD_A2DP,eAD_BT};

	typedef struct _FLY_ASSISTDISPLAY_INFO
	{
		BOOL bOpen;
		BOOL bPower;
		BOOL bPowerUp;

		BOOL bKillDispatchFlyMainThread;
		HANDLE FlyMainThreadHandle;
		HANDLE hDispatchMainThreadEvent;
		BYTE ExBoxInfoFrameStatus;

		BYTE buffToUser[DATA_BUFF_LENGTH];
		UINT buffToUserHx;
		UINT buffToUserLx;
		HANDLE hBuffToUserDataEvent;
		CRITICAL_SECTION hCSSendToUser;

		HANDLE hMsgQueueFromAssistDisplayCreate;

		BOOL bKillDispatchFlyInterThread;
		HANDLE FLyInterThreadHandle;
		HANDLE hDispatchInterThreadEvent;

		//PFLY_DRIVER_GLOBAL pFlyDriverGlobalInfo;
		volatile shm_t *pFlyShmGlobalInfo;
		HANDLE hHandleGlobalGlobalEvent;

		ULONG iNormalTimer;
		HANDLE hAssistComm;
		BOOL bKillFlyhAssistCommThread;
		HANDLE hThreadHandleFlyhhAssistComm;
		UINT AssistInfoFrameStatus;
		BYTE AssistInfoFrameBuff[DATA_COMM_BUFF_LENGTH];
		
		BYTE bRevData[2];
		BYTE iSendToMCULength[256];
		BYTE iSendToMCUBuff[256][256];
	}FLY_ASSISTDISPLAY_INFO, *P_FLY_ASSISTDISPLAY_INFO;

	#define HAND_PHONE_INF  0XC0
	#define PLAYING_TYPE  	0xC1 //NO DISK,CD,ROM,HDCD,DVD,VCD,SVCD,MP3,WMA
	#define TRACK_H			0XC2
	#define TRACK_L			0XCB
	#define RPT_RDM     	0XC3
	#define MINUTE  		0XC4
	#define SECOND			0XC5
	#define CH				0XC6
	#define FM1_FRQ			0XC7
	#define FM2_FRQ			0XC8
	#define AM_FRQ			0XC9 
	#define TIME_SETUP  	0XCA
	//#define SELECT		0XCA
	#define VOLUME			0XCC
	#define PLAYING_STATUS 	0XCD //PAUSE,STOP,LOADING,MENU,EJECTING,CHECKING
	#define MENU			0XCE
	#define B6B7			0XCF
	#define MODE_CHANGE		0XCF
	#define PHONE			0XA1
	#define INQUIRE         0XAA

	
	#define TOTALTRACK_H	0XAB
	#define TOTALTRACK_L	0XAC
	#define TOTALMINUTE 	0XAD
	#define TOTALSECOND 	0XAE

	#define TIMEINFO		0XAF

	typedef enum 
	{	
		P_INFO_NO_DISC=0,
		P_INFO_ROM=1,
		P_INFO_HDCD=2,
		P_INFO_DVD=3,
		P_INFO_VCD=4,
		P_INFO_SVCD=5,
		P_INFO_CD=6,
		P_INFO_MP3=7,
		P_INFO_WMA=8,
		P_INFO_FM2=9,
		P_INFO_FM1=10,
		P_INFO_AM=11,
		P_INFO_SRADIO=0x15
			
	}PLAY_imfo;

	
	//void dealCANAssignmentUpdata(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BYTE cmd,BYTE data);

	VOID WriteToAssistCom(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BYTE *p,UINT len);
	void DealCarLcdInfo(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BYTE *pdata,UINT len);
	void SendDataToCivic(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo);


#define MSG_ENABLE_FLAG (pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_DIS && GLOBAL_DEBUG)
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



#endif// _FLYAUDIO_ASSISTDISPLAY_H_
