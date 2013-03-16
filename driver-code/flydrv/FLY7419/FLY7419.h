/************************************************************************/
/* FLYAUDIO TDA7419                                                     */
/************************************************************************/
#pragma once

#ifndef _FLYAUDIO_TDA7419_H_
#define _FLYAUDIO_TDA7419_H_

#include <windows.h>
#include <basetsd.h>
#include "flyshmlib.h"
#include "flygpio.h"

#if SOFT_VERSION_FOR_OVERSEAS
#error "This software version is for overseras!!!"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define GLOBAL_COMM	1

#define FLY7419_ADDR_R				0x89
#define FLY7419_ADDR_W				0x88

#define FLY7419_I2C_FREQ   100000      //5Khz

//#define R2S11002_ADDR_R				0x91
//#define R2S11002_ADDR_W				0x90
//
//#define STM32_ADDR_R				0xFB
//#define STM32_ADDR_W				0xFA

	//BYTE audioChannelGainTab[14]={//增益//0dB59,6dB65
	//	59,59,59,59,59,59,59,59,59,59,59,59,59,59};
#define BALANCE_LEVEL_COUNT		21
#define FADER_LEVEL_COUNT		21

#define TREB_FREQ_COUNT		2
#define TREB_LEVEL_COUNT	11

#define MID_FREQ_COUNT		3
#define MID_LEVEL_COUNT		11

#define BASS_FREQ_COUNT		3
#define BASS_LEVEL_COUNT	11

#define LOUDNESS_FREQ_COUNT		3
#define LOUDNESS_LEVEL_COUNT	11

#define SUB_FILTER_COUNT	3
#define SUB_LEVEL_COUNT		11


#define DATA_BUFF_LENGTH_TO_DRIVER	1024
#define DATA_BUFF_LENGTH_TO_USER	1024

#define CAR_AMP_IN_VOLUME_CONSTANT  50
#define EQ_VOLUE_TOTAL_NUM 10

	//BYTE EQ_Default[9] = {6,6,6,6,6,6,6,6,6};
	//BYTE EQ_Classic[9] = {10,5,6,6,7,6,8,10,9};
	//BYTE EQ_Pop[9] = {8,4,7,5,7,5,7,9,8};

	typedef struct _FLY_AUDIO_INFO//添加变量后请在Fly7419AudioParaInitDefault函数初始化
	{
		BOOL bInit;
		//	BOOL GraphicalSpectrumAnalyzer;
		//	BYTE GraphicalSpectrumAnalyzerValue[9];

		BOOL bMuteRadio;//收音机
		BOOL bMuteBT;//BT

		BYTE preMainAudioInput;
		BYTE tmpMainAudioInput;
		BYTE curMainAudioInput;
		BYTE dspMainAudioInput;

		BYTE preMainVolume;
		BYTE curMainVolume;
		BYTE dspMainVolume;

		BOOL preMainMute;
		BOOL tmpMainMute;//中转值
		BOOL curMainMute;
		BOOL dspMainMute;
		BOOL cur7386Mute;

		BYTE preBassFreq;
		BYTE tmpBassFreq;//中转值
		BYTE curBassFreq;
		BYTE dspBassFreq;
		BYTE usrBassFreq;

		BYTE preBassLevel;
		BYTE tmpBassLevel;
		BYTE curBassLevel;
		BYTE dspBassLevel;
		BYTE usrBassLevel;

		BYTE preMidFreq;
		BYTE tmpMidFreq;
		BYTE curMidFreq;
		BYTE dspMidFreq;
		BYTE usrMidFreq;

		BYTE preMidLevel;
		BYTE tmpMidLevel;
		BYTE curMidLevel;
		BYTE dspMidLevel;
		BYTE usrMidLevel;

		BYTE preTrebleFreq;
		BYTE tmpTrebleFreq;
		BYTE curTrebleFreq;
		BYTE dspTrebleFreq;
		BYTE usrTrebleFreq;

		BYTE preTrebleLevel;
		BYTE tmpTrebleLevel;
		BYTE curTrebleLevel;
		BYTE dspTrebleLevel;
		BYTE usrTrebleLevel;

		BYTE preLoudFreq;
		BYTE tmpLoudFreq;
		BYTE curLoudFreq;
		BYTE dspLoudFreq;

		BYTE preLoudLevel;
		BYTE tmpLoudLevel;
		BYTE curLoudLevel;
		BYTE dspLoudLevel;

		BYTE preBalance;
		BYTE tmpBalance;
		BYTE curBalance;
		BYTE dspBalance;

		BYTE preFader;
		BYTE tmpFader;
		BYTE curFader;
		BYTE dspFader;

		BOOL preLoudnessOn;
		BOOL tmpLoudnessOn;
		BOOL curLoudnessOn;
		BOOL dspLoudnessOn;

		BOOL preSubOn;
		BOOL tmpSubOn;
		BOOL curSubOn;
		BOOL dspSubOn;

		BYTE preSubFilter;
		BYTE tmpSubFilter;
		BYTE curSubFilter;
		BYTE dspSubFilter;

		BYTE preSubLevel;
		BYTE tmpSubLevel;
		BYTE curSubLevel;
		BYTE dspSubLevel;

		BYTE preSimEQ;
		BYTE curSimEQ;
		BYTE dspSimEQ;

		BYTE preGPSSpeaker;
		BYTE tmpGPSSpeaker;
		BYTE curGPSSpeaker;
		BYTE dspGPSSpeaker;

		BYTE preReadReg;
		BYTE curReadReg;
		BOOL ReadRegFlag;

		BYTE iCurRadioChannel;

		BYTE curEQValues[EQ_VOLUE_TOTAL_NUM];

		BOOL bEnableVolumeFader;
	}FLY_AUDIO_INFO, *P_FLY_AUDIO_INFO;

	typedef struct _FLY_TDA7419_INFO
	{
		BOOL bOpen;
		BOOL bPowerUp;
		BOOL bPowerSpecialUp;
		BOOL bSocketConnecting;
		BOOL bNeedInit;

	
		BYTE bHaveGetCarInfo;
		BYTE buffToUser[DATA_BUFF_LENGTH_TO_USER];
		UINT buffToUserHx;
		UINT buffToUserLx;
		HANDLE hBuffToUserDataEvent;
		CRITICAL_SECTION hCSSendToUser;

		BOOL bKillDispatchTda7419MainThread;
		HANDLE	Tda7419MainThreadHandle;
		HANDLE hDispatchTda7419MainThreadEvent;

		BOOL bKillDispatchTda7419DelayThread;
		HANDLE	Tda7419DelayThreadHandle;
		HANDLE hDispatchTda7419DelayThreadEvent;
		// flyaudio amp
		BOOL bKillDispatchExtAmpThread;
		HANDLE	ExtAmpThreadHandle;
		HANDLE hDispatchExtAmpThreadEvent;

		HANDLE hMsgQueueToExtAmpCreate;
		HANDLE hMsgQueueFromExtAmpCreate;
		BOOL bKillDispatchFlyMsgQueueExtAmpReadThread;
		HANDLE FLyMsgQueueExtAmpReadThreadHandle;
		
		// CAR amp
		HANDLE hMsgQueueToCarAmpCreate;
		HANDLE hMsgQueueFromCarAmpCreate;
		BOOL bKillDispatchFlyMsgQueueCarAmpReadThread;
		HANDLE FLyMsgQueueCarAmpReadThreadHandle;

		HANDLE	Tda7419I2CHandle;

		FLY_AUDIO_INFO FlyAudioInfo;		 

		HANDLE hHandleGlobalGlobalEvent;
		volatile shm_t *pFlyShmGlobalInfo;
	}FLY_TDA7419_INFO, *P_FLY_TDA7419_INFO;

	void FA8200LowVoltageRecovery(P_FLY_TDA7419_INFO pTda7419Info);
	void DealEqControlData(P_FLY_TDA7419_INFO pTda7419Info,BYTE *buf,UINT len);
	void DealVolumeData(P_FLY_TDA7419_INFO pTda7419Info,BYTE bAction);
	void CarVolumeControl(P_FLY_TDA7419_INFO pTda7419Info,BYTE iVolume);

#define VALUE_UNKNOWN	0xFFFFFFFF

#define B_FREQ_60		0
#define B_FREQ_80		1
#define B_FREQ_100		2
#define B_FREQ_200		3

#define M_FREQ_500     0
#define M_FREQ_1000    1
#define M_FREQ_1500    2
#define M_FREQ_2000    3

#define T_FREQ_10K     0
#define T_FREQ_12K5    1
#define T_FREQ_15K     2
#define T_FREQ_17K5    3

#define L_FREQ_Flat    0
#define L_FREQ_400Hz   1
#define L_FREQ_800Hz   2
#define L_FREQ_2K4     3


#define MSG_ENABLE_FLAG (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_7419 && GLOBAL_DEBUG)
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



#endif// _FLYAUDIO_TDA7419_H_