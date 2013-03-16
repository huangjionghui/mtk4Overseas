/************************************************************************/
/* FLYAUDIO SAF7741                                                     */
/************************************************************************/
#pragma once

#ifndef _FLYAUDIO_SAF7741_H_
#define _FLYAUDIO_SAF7741_H_

#include <windows.h>
#include <basetsd.h>
#include "flyshmlib.h"
#include "flygpio.h"

#if !SOFT_VERSION_FOR_OVERSEAS
#error "This software version is for china!!!"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define CURRENT_SHARE_MEMORY_ID	SHARE_MEMORY_AUDIO

#define SAF7741_ADDR_R				0x39
#define SAF7741_ADDR_W				0x38

#define R2S11002_ADDR_R				0x91
#define R2S11002_ADDR_W				0x90

#define STM32_ADDR_R				0xFB
#define STM32_ADDR_W				0xFA

#define CAR_AMP_IN_VOLUME_CONSTANT  50

	//enum audio_source{Init=0,MediaCD,CDC,RADIO,AUX,IPOD,TV,MediaMP3,SRADIO,BT,EXT_TEL,GR_AUDIO,GPS,BACK};
	//BYTE audioChannelGainTab[14]={//增益//0dB59,6dB65
	//	59,59,59,59,59,59,59,59,59,59,59,59,59,59};

#define MTK_VOLUME_MUTE 50
#define MTK_VOLUME_DEF  50

#define DATA_BUFF_LENGTH_TO_DRIVER	1024
#define DATA_BUFF_LENGTH_TO_USER	1024

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

#define B_FREQ_60		0
#define B_FREQ_80		1
#define B_FREQ_100		2

#define M_FREQ_500     0
#define M_FREQ_1000    1
#define M_FREQ_1500    2

#define T_FREQ_10K     0
#define T_FREQ_12K     1

#define Sub_CutFREQ_80          0
#define Sub_CutFREQ_120         1
#define Sub_CutFREQ_160         2

#define DATA_READ_HANDLE_NAME	TEXT("FLYAUDIO_DATA_READ_EVENT")

const BYTE EQ_Default[9] = {6,6,6,6,6,6,6,6,6};
const BYTE EQ_Classic[9] = {10,5,6,6,7,6,8,10,9};
const BYTE EQ_Pop[9] = {8,4,7,5,7,5,7,9,8};
//const BYTE EQ_Jazz[9] = {8,4,7,5,7,5,7,9,8};


typedef struct _FLY_AUDIO_INFO//添加变量后请在Fly7419AudioParaInitDefault函数初始化
{
	BOOL bInit;
	//	BOOL GraphicalSpectrumAnalyzer;
		BYTE GraphicalSpectrumAnalyzerValue[9];

	BOOL bMuteRadio;//收音机
	BOOL bMuteBT;//BT

	audio_source preMainAudioInput;
	audio_source tmpMainAudioInput;
	audio_source curMainAudioInput;
	audio_source dspMainAudioInput;

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

	BYTE preEQ[10];
	BYTE curEQ[10];

	UINT prePos;
	UINT curPos;

	BYTE preGPSSpeaker;
	BYTE tmpGPSSpeaker;
	BYTE curGPSSpeaker;
	BYTE dspGPSSpeaker;

	BYTE preReadReg;
	BYTE curReadReg;
	BOOL ReadRegFlag;

	BOOL bEnableVolumeFader;
}FLY_AUDIO_INFO, *P_FLY_AUDIO_INFO;


typedef struct _FLY_SAF7741_INFO
{
	BOOL bOpen;
	BOOL bPower;
	BOOL bPowerUp;

	BOOL bPowerSpecialUp;
	BOOL bNeedInit;

	UINT buffToDriversProcessorStatus;

	BYTE GraphicalSpectrumAnalyzerValue[9];

	BYTE buffToUser[DATA_BUFF_LENGTH_TO_USER];
	UINT buffToUserHx;
	UINT buffToUserLx;
	HANDLE buffToUserDataEvent;
	CRITICAL_SECTION hCSSendToUser;

	UINT SAF7741PowerSteps;

	HANDLE	SAF7741MainThreadHandle;
	BOOL bKillDispatchSAF7741MainThread;
	HANDLE hDispatchSAF7741MainThreadEvent;

	HANDLE	SAF7741DelayThreadHandle;
	BOOL bKillDispatchSAF7741DelayThread;
	HANDLE hDispatchSAF7741DelayThreadEvent;

	HANDLE	SAF7741ExtAmpThreadHandle;
	BOOL bKillDispatchExtAmpThread;
	HANDLE hDispatchExtAmpThreadEvent;

	HANDLE hMsgQueueToExtAmpCreate;
	HANDLE hMsgQueueFromExtAmpCreate;
	BOOL bKillDispatchFlyMsgQueueExtAmpReadThread;
	HANDLE FLyMsgQueueExtAmpReadThreadHandle;

	HANDLE	SAF7741I2CHandle;
	
	FLY_AUDIO_INFO FlyAudioInfo;	
	
	BOOL bSocketConnecting;
	volatile shm_t *pFlyShmGlobalInfo;
}FLY_SAF7741_INFO, *P_FLY_SAF7741_INFO;

#define VALUE_UNKNOWN	0xFFFFFFFF

DWORD WINAPI FlyAudioExtAmpReadThread(LPVOID pContext);
void FA8200LowVoltageRecovery(P_FLY_SAF7741_INFO pSAF7741Info);

#define MSG_ENABLE_FLAG (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_7419)
#define DBGE(string) RETAILMSG(1,string)//重要错误
#define DBGW(string) if (1) RETAILMSG(1,string)//警告
#define DBGI(string) if (1) RETAILMSG(1,string)//信息
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



#endif// _FLYAUDIO_SAF7741_H_
