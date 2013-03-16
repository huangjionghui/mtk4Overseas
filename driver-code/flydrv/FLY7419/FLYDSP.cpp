// FLY7419.cpp : Defines the entry point for the DLL application.
//
//>>>2011-05-20: V0.02 整理了WinCE待机相关的代码	JQilin

#include "FLY7419.h"



#define LINFA_HOST_CON_ID		0x01
#define LINFA_FA8200_CON_ID		0x02
#define LINFA_REMOTE_CON_ID		0x03

#define FA8200_DATA_ID 0x01


void writeDataToMcu(P_FLY_TDA7419_INFO pTda7419Info,BYTE *p,UINT length);
#define LARGER(A, B)    ((A) >= (B)? (A):(B))

static void FA8200checkbHaveFA8200(P_FLY_TDA7419_INFO pTda7419Info)
{

	BYTE buff[] = {0x01,FA8200_DATA_ID,0x09};
	
	if(0 == pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveFlyAudioExtAMP)
	{
		writeDataToMcu(pTda7419Info,buff,3);
	}
}

static void FA8200VolumeControl(P_FLY_TDA7419_INFO pTda7419Info,BYTE iVolume)
{
	BYTE  buff[] = {0x01,FA8200_DATA_ID,0x01,0x00};

	buff[3] = iVolume;
	writeDataToMcu(pTda7419Info,buff,4);
}

static void FA8200MuteControl(P_FLY_TDA7419_INFO pTda7419Info,BOOL bMute)
{
	BYTE buff[] = {0x01,FA8200_DATA_ID,0x02,0x00};

	buff[3] = bMute;
	writeDataToMcu(pTda7419Info,buff,4);
}

static void FA8200InputControl(P_FLY_TDA7419_INFO pTda7419Info,BYTE input)
{
	BYTE buff[] = {0x01,FA8200_DATA_ID,0x03,0x00};

	buff[3] = input;
	writeDataToMcu(pTda7419Info,buff,4);
}

static void FA8200EQControl(P_FLY_TDA7419_INFO pTda7419Info,BYTE *pdata)
{
	BYTE buff[12]={0x01,FA8200_DATA_ID,0x04};

	memcpy(&buff[3],pdata,9);
	writeDataToMcu(pTda7419Info,buff,12);
}

static void FA8200NaviMixControl(P_FLY_TDA7419_INFO pTda7419Info,BOOL bMix)
{
	BYTE buff[] = {0x01,FA8200_DATA_ID,0x06,0x00};

	buff[3] = bMix;
	writeDataToMcu(pTda7419Info,buff,4);
}

static void FA8200SoundAudioControl(P_FLY_TDA7419_INFO pTda7419Info,
	BYTE loudFreq,BYTE loudQ,
	BYTE trebFreq,BYTE trebQ,
	BYTE middFreq,BYTE middQ,
	BYTE bassFreq,BYTE bassQ)
{
	BYTE buff[11]={0x01,FA8200_DATA_ID,0x07};
	
	buff[3] = loudFreq;buff[4] = loudQ;
	buff[5] = trebFreq;buff[6] = trebQ;
	buff[7] = middFreq;buff[8] = middQ;
	buff[9] = bassFreq;buff[10] = bassQ;
	writeDataToMcu(pTda7419Info,buff,11);

}

static void FA8200FaderBalanceControl(P_FLY_TDA7419_INFO pTda7419Info,BYTE fader,BYTE balance)
{
	BYTE buff[] = {0x01,FA8200_DATA_ID,0x08,0x00,0x00};
	
	buff[3] = fader;
	buff[4] = balance;
	writeDataToMcu(pTda7419Info,buff,5);
}

static void FA8200MPEGAudioControl(P_FLY_TDA7419_INFO pTda7419Info,BYTE para)
{
	BYTE buff[] = {0x01,FA8200_DATA_ID,0x0B,0x00};

	buff[3] = para;
	writeDataToMcu(pTda7419Info,buff,4);
}

BYTE currentVolume,currentMute,currentInput,currentEQ[9],currentNavi;
BYTE currentLoudFreq,currentLoudLevel,currentTerbFreq,currentTrebLevel,currentMidFreq,currentMidLevel,currentBassFreq,currentBassLevel;
BYTE currentFader,currentBanlace;
BYTE currentMPEGSound;

void FA8200LowVoltageRecovery(P_FLY_TDA7419_INFO pTda7419Info)
{
	currentVolume = 0xFF;currentMute = 0xFF;currentInput = 0xFF;
	memset(&currentEQ[0],0,9);currentNavi = 0xFF;
	currentLoudFreq = 0xFF;currentLoudLevel = 0xFF;
	currentTerbFreq = 0xFF;currentTrebLevel = 0xFF;
	currentMidFreq = 0xFF;currentMidLevel = 0xFF;
	currentBassFreq = 0xFF;currentBassLevel = 0xFF;
	currentFader = 0xFF;currentBanlace = 0xFF;
	currentMPEGSound = 0xFF;
	SetEvent(pTda7419Info->hDispatchExtAmpThreadEvent);
}

DWORD WINAPI ExtAmpProcessThread(LPVOID pContext)
{
	P_FLY_TDA7419_INFO pTda7419Info = (P_FLY_TDA7419_INFO)pContext;
	ULONG WaitReturn;

	FA8200LowVoltageRecovery(pTda7419Info);

	while (!pTda7419Info->bKillDispatchExtAmpThread)
	{
		WaitReturn = WaitForSingleObject(pTda7419Info->hDispatchExtAmpThreadEvent, 5000);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nFlyAudio ExtAmp Process Event Open Error")));
		}
		if (pTda7419Info->bPowerUp
			&& pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveFlyAudioExtAMP)
		{
			if(currentVolume != pTda7419Info->FlyAudioInfo.dspMainVolume)
			{
				currentVolume = pTda7419Info->FlyAudioInfo.dspMainVolume;
				FA8200VolumeControl(pTda7419Info,currentVolume);
			}
			if(currentMute != pTda7419Info->FlyAudioInfo.dspMainMute)
			{
				currentMute = pTda7419Info->FlyAudioInfo.dspMainMute;
				FA8200MuteControl(pTda7419Info,currentMute);
			}
			if(currentInput != (BYTE)(pTda7419Info->FlyAudioInfo.dspMainAudioInput))
			{
				currentInput = (BYTE)(pTda7419Info->FlyAudioInfo.dspMainAudioInput);
				if (MediaCD == currentInput)
				{
					FA8200InputControl(pTda7419Info,0x01);
				}
				else
				{
					FA8200InputControl(pTda7419Info,0x00);
				}
			}
			if(currentNavi != pTda7419Info->FlyAudioInfo.dspGPSSpeaker)
			{
				currentNavi = pTda7419Info->FlyAudioInfo.dspGPSSpeaker;
				if(currentNavi)
					FA8200NaviMixControl(pTda7419Info,0);
				else
					FA8200NaviMixControl(pTda7419Info,1);

			}
			if((currentLoudFreq != pTda7419Info->FlyAudioInfo.dspLoudFreq)
				||(currentLoudLevel != pTda7419Info->FlyAudioInfo.dspLoudLevel)
				||(currentTerbFreq != pTda7419Info->FlyAudioInfo.dspTrebleFreq)
				||(currentTrebLevel != pTda7419Info->FlyAudioInfo.dspTrebleLevel)
				||(currentMidFreq != pTda7419Info->FlyAudioInfo.dspMidFreq)
				||(currentMidLevel != pTda7419Info->FlyAudioInfo.dspMidLevel)
				||(currentBassFreq != pTda7419Info->FlyAudioInfo.dspBassFreq)
				||(currentBassLevel != pTda7419Info->FlyAudioInfo.dspBassLevel))
			{
				currentLoudFreq = pTda7419Info->FlyAudioInfo.dspLoudFreq;
				currentLoudLevel = pTda7419Info->FlyAudioInfo.dspLoudLevel;
				currentTerbFreq = pTda7419Info->FlyAudioInfo.dspTrebleFreq;
				currentTrebLevel = pTda7419Info->FlyAudioInfo.dspTrebleLevel;
				currentMidFreq = pTda7419Info->FlyAudioInfo.dspMidFreq;
				currentMidLevel = pTda7419Info->FlyAudioInfo.dspMidLevel;
				currentBassFreq = pTda7419Info->FlyAudioInfo.dspBassFreq;
				currentBassLevel = pTda7419Info->FlyAudioInfo.dspBassLevel;
				FA8200SoundAudioControl(pTda7419Info,
					currentLoudFreq,currentLoudLevel,currentTerbFreq,currentTrebLevel,
					currentMidFreq,currentMidLevel,currentBassFreq,currentBassLevel);
			}
			if((currentFader !=  pTda7419Info->FlyAudioInfo.dspFader)
				||(currentBanlace != pTda7419Info->FlyAudioInfo.dspBalance))
			{
					currentFader =pTda7419Info->FlyAudioInfo.dspFader;
					currentBanlace =pTda7419Info->FlyAudioInfo.dspBalance;
					FA8200FaderBalanceControl(pTda7419Info,currentFader,currentBanlace);
			}
			//if(memcmp(currentEQ,pTda7419Info->FlyAudioInfo.dspEQ,9))
			//{
			//	memcpy(currentEQ,pTda7419Info->FlyAudioInfo.dspEQ,9);
			//	FA8200InputControl(pTda7419Info,currentEQ);				
			//}
		}
	}
	DBGD((TEXT("\r\nFlyAudio ExtAmp Process exit")));
	pTda7419Info->ExtAmpThreadHandle = NULL;
	return 0;
}











