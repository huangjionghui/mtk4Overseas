// FLY7419.cpp : Defines the entry point for the DLL application.
//
//>>>2011-05-20: V0.02 整理了WinCE待机相关的代码	JQilin

#include "FLY7741.h"

#define LINFA_HOST_CON_ID		0x01
#define LINFA_FA8200_CON_ID		0x02
#define LINFA_REMOTE_CON_ID		0x03

#define FA8200_DATA_ID 0x01

void writeDataToMcu(P_FLY_SAF7741_INFO pSAF7741Info,BYTE *p,UINT length);
#define LARGER(A, B)    ((A) >= (B)? (A):(B))

static void FA8200checkbHaveFA8200(P_FLY_SAF7741_INFO pSAF7741Info)
{

	BYTE buff[] = {0x01,FA8200_DATA_ID,0x09};
	
	if(0 == pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveFlyAudioExtAMP)
	{
		writeDataToMcu(pSAF7741Info,buff,3);
	}
}

static void FA8200VolumeControl(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iVolume)
{
	BYTE  buff[] = {0x01,FA8200_DATA_ID,0x01,0x00};

	buff[3] = iVolume;
	writeDataToMcu(pSAF7741Info,buff,4);
}

static void FA8200MuteControl(P_FLY_SAF7741_INFO pSAF7741Info,BOOL bMute)
{
	BYTE buff[] = {0x01,FA8200_DATA_ID,0x02,0x00};

	buff[3] = bMute;
	writeDataToMcu(pSAF7741Info,buff,4);
}

static void FA8200InputControl(P_FLY_SAF7741_INFO pSAF7741Info,BYTE input)
{
	BYTE buff[] = {0x01,FA8200_DATA_ID,0x03,0x00};

	buff[3] = input;
	writeDataToMcu(pSAF7741Info,buff,4);
}

static void FA8200EQControl(P_FLY_SAF7741_INFO pSAF7741Info,BYTE *pdata)
{
	BYTE buff[12]={0x01,FA8200_DATA_ID,0x04};

	memcpy(&buff[3],pdata,9);
	writeDataToMcu(pSAF7741Info,buff,12);
}

static void FA8200NaviMixControl(P_FLY_SAF7741_INFO pSAF7741Info,BOOL bMix)
{
	BYTE buff[] = {0x01,FA8200_DATA_ID,0x06,0x00};

	buff[3] = bMix;
	writeDataToMcu(pSAF7741Info,buff,4);
}

static void FA8200SoundAudioControl(P_FLY_SAF7741_INFO pSAF7741Info,
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
	writeDataToMcu(pSAF7741Info,buff,11);

}

static void FA8200FaderBalanceControl(P_FLY_SAF7741_INFO pSAF7741Info,BYTE fader,BYTE balance)
{
	BYTE buff[] = {0x01,FA8200_DATA_ID,0x08,0x00,0x00};
	
	buff[3] = fader;
	buff[4] = balance;
	writeDataToMcu(pSAF7741Info,buff,5);
}

static void FA8200MPEGAudioControl(P_FLY_SAF7741_INFO pSAF7741Info,BYTE para)
{
	BYTE buff[] = {0x01,FA8200_DATA_ID,0x0B,0x00};

	buff[3] = para;
	writeDataToMcu(pSAF7741Info,buff,4);
}

BYTE currentVolume,currentMute,currentInput,currentEQ[9],currentNavi;
BYTE currentLoudFreq,currentLoudLevel,currentTerbFreq,currentTrebLevel,currentMidFreq,currentMidLevel,currentBassFreq,currentBassLevel;
BYTE currentFader,currentBanlace;
BYTE currentMPEGSound;

void FA8200LowVoltageRecovery(P_FLY_SAF7741_INFO pSAF7741Info)
{
	currentVolume = 0xFF;currentMute = 0xFF;currentInput = 0xFF;
	memset(&currentEQ[0],0,9);currentNavi = 0xFF;
	currentLoudFreq = 0xFF;currentLoudLevel = 0xFF;
	currentTerbFreq = 0xFF;currentTrebLevel = 0xFF;
	currentMidFreq = 0xFF;currentMidLevel = 0xFF;
	currentBassFreq = 0xFF;currentBassLevel = 0xFF;
	currentFader = 0xFF;currentBanlace = 0xFF;
	currentMPEGSound = 0xFF;
	SetEvent(pSAF7741Info->hDispatchExtAmpThreadEvent);
}

DWORD WINAPI FlyAudioExtAmpReadThread(LPVOID pContext)
{
	P_FLY_SAF7741_INFO pSAF7741Info = (P_FLY_SAF7741_INFO)pContext;
	ULONG WaitReturn;

	FA8200LowVoltageRecovery(pSAF7741Info);

	while (!pSAF7741Info->bKillDispatchExtAmpThread)
	{
		WaitReturn = WaitForSingleObject(pSAF7741Info->hDispatchExtAmpThreadEvent, 5000);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGI((TEXT("\r\nFlyAudio ExtAmp Process Event Open Error")));
		}
		if (pSAF7741Info->bPowerUp
			&& pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveFlyAudioExtAMP)
		{
			if(currentVolume != pSAF7741Info->FlyAudioInfo.dspMainVolume)
			{
				currentVolume = pSAF7741Info->FlyAudioInfo.dspMainVolume;
				FA8200VolumeControl(pSAF7741Info,currentVolume);
			}
			if(currentMute != pSAF7741Info->FlyAudioInfo.dspMainMute)
			{
				currentMute = pSAF7741Info->FlyAudioInfo.dspMainMute;
				FA8200MuteControl(pSAF7741Info,currentMute);
			}
			if(currentInput != (BYTE)(pSAF7741Info->FlyAudioInfo.dspMainAudioInput))
			{
				currentInput = (BYTE)(pSAF7741Info->FlyAudioInfo.dspMainAudioInput);
				if (MediaCD == currentInput)
				{
					FA8200InputControl(pSAF7741Info,0x01);
				}
				else
				{
					FA8200InputControl(pSAF7741Info,0x00);
				}
			}
			if(currentNavi != pSAF7741Info->FlyAudioInfo.dspGPSSpeaker)
			{
				currentNavi = pSAF7741Info->FlyAudioInfo.dspGPSSpeaker;
				if(currentNavi)
					FA8200NaviMixControl(pSAF7741Info,0);
				else
					FA8200NaviMixControl(pSAF7741Info,1);

			}
			if((currentLoudFreq != pSAF7741Info->FlyAudioInfo.dspLoudFreq)
				||(currentLoudLevel != pSAF7741Info->FlyAudioInfo.dspLoudLevel)
				||(currentTerbFreq != pSAF7741Info->FlyAudioInfo.dspTrebleFreq)
				||(currentTrebLevel != pSAF7741Info->FlyAudioInfo.dspTrebleLevel)
				||(currentMidFreq != pSAF7741Info->FlyAudioInfo.dspMidFreq)
				||(currentMidLevel != pSAF7741Info->FlyAudioInfo.dspMidLevel)
				||(currentBassFreq != pSAF7741Info->FlyAudioInfo.dspBassFreq)
				||(currentBassLevel != pSAF7741Info->FlyAudioInfo.dspBassLevel))
			{
				currentLoudFreq = pSAF7741Info->FlyAudioInfo.dspLoudFreq;
				currentLoudLevel = pSAF7741Info->FlyAudioInfo.dspLoudLevel;
				currentTerbFreq = pSAF7741Info->FlyAudioInfo.dspTrebleFreq;
				currentTrebLevel = pSAF7741Info->FlyAudioInfo.dspTrebleLevel;
				currentMidFreq = pSAF7741Info->FlyAudioInfo.dspMidFreq;
				currentMidLevel = pSAF7741Info->FlyAudioInfo.dspMidLevel;
				currentBassFreq = pSAF7741Info->FlyAudioInfo.dspBassFreq;
				currentBassLevel = pSAF7741Info->FlyAudioInfo.dspBassLevel;
				FA8200SoundAudioControl(pSAF7741Info,
					currentLoudFreq,currentLoudLevel,currentTerbFreq,currentTrebLevel,
					currentMidFreq,currentMidLevel,currentBassFreq,currentBassLevel);
			}
			if((currentFader !=  pSAF7741Info->FlyAudioInfo.dspFader)
				||(currentBanlace != pSAF7741Info->FlyAudioInfo.dspBalance))
			{
					currentFader =pSAF7741Info->FlyAudioInfo.dspFader;
					currentBanlace =pSAF7741Info->FlyAudioInfo.dspBalance;
					FA8200FaderBalanceControl(pSAF7741Info,currentFader,currentBanlace);
			}
			//if(memcmp(currentEQ,pSAF7741Info->FlyAudioInfo.dspEQ,9))
			//{
			//	memcpy(currentEQ,pSAF7741Info->FlyAudioInfo.dspEQ,9);
			//	FA8200InputControl(pSAF7741Info,currentEQ);				
			//}
		}
	}
	DBGI((TEXT("\r\nFlyAudio ExtAmp Process exit")));
	pSAF7741Info->SAF7741ExtAmpThreadHandle = NULL;
	return 0;
}

/*void dealLINFA_REMOTEProcessor(P_FLY_SAF7741_INFO pSAF7741Info,BYTE *pdata,WORD len)
{
	switch(pdata[0])
	{
		case 0x01:
			if( BaseDATA.audio_MainVolume != pdata[1])
			{
				#if CAR_TUGUAN || CAR_BENZ_VIANO_4S
					BaseDATA.audio_MainVolume = pdata[1]/2;
				#else
					BaseDATA.audio_MainVolume =	pdata[1];
				#endif
				volumeToWinCEUpdate(BaseDATA.audio_MainVolume);
				TASK_SET(audioVolumeBit);
			}
			break;
		case 0x41:
			add_keymessageToARM(KB_AV,0);
			break;
		case 0x42:
			if(pdata[1])
			{
				add_keymessageToARM(KB_SEEK_INC,0);
			}
			else
			{
				add_keymessageToARM(KB_SEEK_DEC,0);
			}
			break;
		default:
			break;
	}
}
*/
/*void EvtFA8200Processor(void)
{
	if(LINFAInfo.bHaveFA8200 == 2)
	{
		FA8200MuteControl(audio_MuteStatus);
		if(MediaCD == BaseDATA.audio_Source &&\
		 BT_speaker == 0 && GPRS_speaker == 0 && TEL_speaker == 0)//架构原因，修改一下，个别原因个别处理
		{
			FA8200InputControl(FA8200INPUT_DIGITAL);
		}
		else
		{
			FA8200InputControl(FA8200INPUT_ANALOG);
		}

		#if CAR_TUGUAN || CAR_BENZ_VIANO_4S	  
		FA8200VolumeControl(BaseDATA.audio_MainVolume*2);
		#else
		FA8200VolumeControl(BaseDATA.audio_MainVolume);
		#endif

		FA8200EQControl(&BaseDATA.EQ_Config[0]);
		if(GPS_speaker)
		{
			FA8200NaviMixControl(0);
		}
		else
		{
			FA8200NaviMixControl(1);
		}
		FA8200SoundAudioControl(BaseDATA.audio_Loud_Freq,BaseDATA.audio_Loud,
		BaseDATA.audio_Treb_Freq,BaseDATA.audio_Treb,
		BaseDATA.audio_Mid_Freq,BaseDATA.audio_Mid,
		BaseDATA.audio_Bass_Freq,BaseDATA.audio_Bass);
		FA8200FrontRearControl(BaseDATA.audio_Balance_FR,BaseDATA.audio_Balance_LR);
		FA8200MPEGAudioControl(0);
	}
}*/










