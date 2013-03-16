// FLY7419.cpp : Defines the entry point for the DLL application.
//
//>>>2011-05-20: V0.02 整理了WinCE待机相关的代码	JQilin

#include "flyhwlib.h"
#include "flyuserlib.h"
#include "flysocketlib.h"
#include "FLY7419_Data.h"
#include "FLY7419.h"

P_FLY_TDA7419_INFO gpTda7419Info = NULL;

void FAU_Init(void);
void FAU_PowerUp(DWORD hDeviceContext);
void FAU_PowerDown(DWORD hDeviceContext);
void TDA7419_Volume(P_FLY_TDA7419_INFO pTda7419Info, BYTE iVolume);
void TDA7419_Navi_Mix(P_FLY_TDA7419_INFO pTda7419Info,BYTE para,BYTE iLevel);
void Fly7419Enable(P_FLY_TDA7419_INFO pTda7419Info,BOOL bOn);
void Fly7419AudioParaInitDefault(P_FLY_TDA7419_INFO pTda7419Info);
DWORD WINAPI ExtAmpProcessThread(LPVOID pContext);
void writeCarAmpDataToMcu(P_FLY_TDA7419_INFO pTda7419Info,BYTE *p,UINT length);

void volumeFaderInOut(P_FLY_TDA7419_INFO pTda7419Info,BOOL bEnable);


#define LARGER(A, B)    ((A) >= (B)? (A):(B))


#if SHICHAN_SHICHAN
	#define IO_7386_MUTE_ON		0
	#define IO_7386_MUTE_OFF	1
#else
	#define IO_7386_MUTE_ON		1
	#define IO_7386_MUTE_OFF	0
#endif

VOID IO_Control_Init(P_FLY_TDA7419_INFO pTda7419Info)
{

}

static void control4052Input(P_FLY_TDA7419_INFO pTda7419Info,BYTE eInput)
{

}

static void control7386Mute(P_FLY_TDA7419_INFO pTda7419Info,BOOL bMute)
{

}

static BOOL I2C_Read_Tda7419(P_FLY_TDA7419_INFO pTda7419Info, UCHAR ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
{
	return TRUE;
}
static BOOL I2C_Write_Tda7419(P_FLY_TDA7419_INFO pTda7419Info, UCHAR ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
{
	return TRUE;
} 

static void control_FlyAudio(P_FLY_TDA7419_INFO pTda7419Info,BYTE *buf, UINT16 len)
{
	if (len <= 0)
		return;

	SocketWrite(buf,len);
}

static void Tda7419_Control_Radio_Channel(P_FLY_TDA7419_INFO pTda7419Info,BYTE iChannel)
{
	BYTE buf[2]={0x1B,0x00};
	buf[1]=iChannel;
	control_FlyAudio(pTda7419Info,buf,2);
}

static void Tda7419_Input(P_FLY_TDA7419_INFO pTda7419Info,BYTE channel,BYTE InputGain)
{

	DBGI((TEXT("\r\nFlyAudio 7419 Input ----->%d"),channel));

	BYTE buf[2]={0x18,0x00};
	buf[1]=channel;
	control_FlyAudio(pTda7419Info,buf,2);

	if (channel == RADIO)
	{
		Tda7419_Control_Radio_Channel(pTda7419Info,
			pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPreRadioChannel);
	}
}

// 7419 静音的特殊处理
void TDA7419_Mute_Control(P_FLY_TDA7419_INFO pTda7419Info,BOOL bMute)
{			
	BYTE buf[2]={0x10,0x00};

	if (bMute)
	{
		buf[1]=0x01;
	}
	control_FlyAudio(pTda7419Info,buf,2);
}

void controlEQTypes(P_FLY_TDA7419_INFO pTda7419Info, BYTE preSimEQ)
{
	BYTE buf[2]={0x19,0x00};

	buf[1]=preSimEQ;

	control_FlyAudio(pTda7419Info,buf,2);
}

void controlEQValues(P_FLY_TDA7419_INFO pTda7419Info, BYTE *buf, UINT16 len)
{
	if (len != EQ_VOLUE_TOTAL_NUM)
		return;

	BYTE buff[1+EQ_VOLUE_TOTAL_NUM]={0x1A};
	memcpy(&buff[1],buf,EQ_VOLUE_TOTAL_NUM);
	control_FlyAudio(pTda7419Info,buff,1+EQ_VOLUE_TOTAL_NUM);
}


void Tda7386_Mute(P_FLY_TDA7419_INFO pTda7419Info,BOOL para)
{
	if (para)
	{
		control7386Mute(pTda7419Info,TRUE);
	}
	else
	{
		control7386Mute(pTda7419Info,FALSE);
	}
}

void Tda7419_Mute(P_FLY_TDA7419_INFO pTda7419Info,BOOL para)
{
	if(para)
	{
		TDA7419_Mute_Control(pTda7419Info, TRUE);
	}
	else
	{
		TDA7419_Mute_Control(pTda7419Info, FALSE);
		control7386Mute(pTda7419Info,FALSE);
	}
	
}

static void TDA7419_Navi_Mix(P_FLY_TDA7419_INFO pTda7419Info,BYTE para,BYTE iLevel)
{
	BYTE temp;
	temp = 0;
	para = para*60/pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax;
	
	//混音大小
	if (para >= 60)
	{
		para = 60;
	}

	BYTE buf[3]={0x49,0x00,0x00};
	buf[1] = para;
	buf[2] = iLevel;

	control_FlyAudio(pTda7419Info,buf,3);
}

VOID TDA7419_Volume(P_FLY_TDA7419_INFO pTda7419Info, BYTE iVolume)
{
	BYTE buf[2]={0x11,0x00};
	
	iVolume = iVolume*60/pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax;
	if (iVolume > 60) 
		iVolume = 60;

	buf[1]=iVolume;
	control_FlyAudio(pTda7419Info,buf,2);
	
	//if(EXT_TEL == pTda7419Info->FlyAudioInfo.curMainAudioInput)
	//{
	//	iVolume = iVolume + 6;
	//	if (iVolume > sizeof(VolumeMask))
	//	{
	//		iVolume = sizeof(VolumeMask) - 1;
	//	}
	//}
	//temp = VolumeMask[iVolume];
	//temp = temp & 0x7F;
	//TDA7419_Para[3] = temp;
}

VOID TDA7419_Loud(P_FLY_TDA7419_INFO pTda7419Info, BYTE LoudFreq, BYTE LoudLevel)
{
	BYTE buf[2]={0x1C,0x00};

	buf[1]=LoudLevel;
	control_FlyAudio(pTda7419Info,buf,2);
}

static VOID Tda7419_Sub(P_FLY_TDA7419_INFO pTda7419Info, BYTE SubFreq, BYTE SubLevel)
{
	BYTE buf[2]={0x1D,0x00};

	buf[1]=SubLevel;
	control_FlyAudio(pTda7419Info,buf,2);
}

static VOID Tda7419_Bass(P_FLY_TDA7419_INFO pTda7419Info, BYTE BassFreq, BYTE BassLevel, BYTE *pLevelData)
{
	BYTE buf[2]={0x13,0x00};

	if(BassFreq > 2)
		BassFreq = 2;
	if(BassLevel > 10)
		BassLevel = 10;

	buf[1]=BassLevel;
	control_FlyAudio(pTda7419Info,buf,2);
}

static VOID Tda7419_Mid(P_FLY_TDA7419_INFO pTda7419Info, BYTE MidFreq, BYTE MidLevel, BYTE *pLevelData)
{
	BYTE buf[2]={0x14,0x00};

	if(MidFreq > 2)
		MidFreq = 2;
	if(MidLevel > 10)
		MidLevel = 10;

	buf[1]=MidLevel;
	control_FlyAudio(pTda7419Info,buf,2);
}

static VOID Tda7419_Treble(P_FLY_TDA7419_INFO pTda7419Info, BYTE TrebFreq, BYTE TrebLevel, BYTE *pLevelData)
{
	BYTE buf[2]={0x15,0x00};

	if(TrebFreq > 1)
		TrebFreq = 1;
	if(TrebLevel > 10) 
		TrebLevel = 10;

	buf[1]=TrebLevel;
	control_FlyAudio(pTda7419Info,buf,2);
}

static void Tda7419_BackVideoSigned(P_FLY_TDA7419_INFO pTda7419Info, BOOL bSined)
{
	BYTE buf[2]={0x20,0x00};
	buf[1] = bSined;
	control_FlyAudio(pTda7419Info,buf,2);
}
static VOID Tda7419_TreMidBass_ForRadio(P_FLY_TDA7419_INFO pTda7419Info)
{
	if (RADIO == pTda7419Info->FlyAudioInfo.curMainAudioInput)
	{
		Tda7419_Treble(pTda7419Info,pTda7419Info->FlyAudioInfo.curTrebleFreq,pTda7419Info->FlyAudioInfo.curTrebleLevel,(BYTE *)Treble_Middle_Bass_Mask_Radio_Tre);
		Tda7419_Mid(pTda7419Info,pTda7419Info->FlyAudioInfo.curMidFreq,pTda7419Info->FlyAudioInfo.curMidLevel,(BYTE *)Treble_Middle_Bass_Mask_Radio_Mid);
		Tda7419_Bass(pTda7419Info,pTda7419Info->FlyAudioInfo.curBassFreq,pTda7419Info->FlyAudioInfo.curBassLevel,(BYTE *)Treble_Middle_Bass_Mask_Radio_Bass);
	}
	else
	{
		Tda7419_Treble(pTda7419Info,pTda7419Info->FlyAudioInfo.curTrebleFreq,pTda7419Info->FlyAudioInfo.curTrebleLevel,(BYTE *)Treble_Middle_Bass_Mask_Normal);
		Tda7419_Mid(pTda7419Info,pTda7419Info->FlyAudioInfo.curMidFreq,pTda7419Info->FlyAudioInfo.curMidLevel,(BYTE *)Treble_Middle_Bass_Mask_Normal);
		Tda7419_Bass(pTda7419Info,pTda7419Info->FlyAudioInfo.curBassFreq,pTda7419Info->FlyAudioInfo.curBassLevel,(BYTE *)Treble_Middle_Bass_Mask_Normal);
	}
}

//前后左右平衡
static VOID TDA7419_Balance_Fader(P_FLY_TDA7419_INFO pTda7419Info, BYTE ibalance,BYTE iFader)
{
	if(ibalance > 20)
		ibalance = 20;
	if(iFader > 20)
		iFader = 20;


	//左右
	BYTE buf[3]={0x16,0x00,0x00};

	buf[1]=ibalance;
	buf[2]=iFader;
	control_FlyAudio(pTda7419Info,buf,3);
}

static void TDA7419_SendVolumeToUser(P_FLY_TDA7419_INFO pTda7419Info)
{
	//如果是倒车状态时，不返回音量给界面显示
	if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable)
	{
		if (!pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
		{
			IpcStartEvent(EVENT_GLOBAL_VLAULE_TO_USER);
		}
	}
	else
	{
		IpcStartEvent(EVENT_GLOBAL_VLAULE_TO_USER);
	}
}

void returnAudioPowerMode(P_FLY_TDA7419_INFO pTda7419Info,BOOL bPower)
{
	BYTE buf[] = {0x01,0x00};
	if (bPower)
	{
		buf[1] = 0x01;
	}
	else
	{
		buf[1] = 0x00;
	}

	ReturnToUser(buf,2);
}

void returnAudiobInit(P_FLY_TDA7419_INFO pTda7419Info,BOOL bInit)
{
	BYTE buf[] = {0x02,0x00};
	if (bInit)
	{
		buf[1] = 0x01;
	}
	else
	{
		buf[1] = 0x00;
	}

	ReturnToUser(buf,2);
}

void returnAudioMainInput(P_FLY_TDA7419_INFO pTda7419Info,BYTE iInput)
{
	BYTE buf[] = {0x10,0x00};
	buf[1] = iInput;

	ReturnToUser(buf,2);
}

void returnAudioMainMute(P_FLY_TDA7419_INFO pTda7419Info,BOOL bMute)
{
	BYTE buf[] = {0x11,0x00};
	if (bMute)
	{
		buf[1] = 0x01;
	}
	else
	{
		buf[1] = 0x00;
	}

	ReturnToUser(buf,2);
}

void returnAudioMainVolume(P_FLY_TDA7419_INFO pTda7419Info,BYTE iVolume)
{
	BYTE buf[] = {0x12,0x00};
	buf[1] = iVolume;

	ReturnToUser(buf,2);
}

void returnAudioMainBalance(P_FLY_TDA7419_INFO pTda7419Info,BYTE iBalance)
{
	BYTE buf[] = {0x13,0x00};
	buf[1] = iBalance;

	ReturnToUser(buf,2);
}

void returnAudioMainFader(P_FLY_TDA7419_INFO pTda7419Info,BYTE iFader)
{
	BYTE buf[] = {0x14,0x00};
	buf[1] = iFader;

	ReturnToUser(buf,2);
}

void returnAudioMainTrebFreq(P_FLY_TDA7419_INFO pTda7419Info,BYTE iTrebFreq)
{
	BYTE buf[] = {0x15,0x00};
	buf[1] = iTrebFreq;

	ReturnToUser(buf,2);
}

void returnAudioMainTrebLevel(P_FLY_TDA7419_INFO pTda7419Info,BYTE iTrebLevel)
{
	BYTE buf[] = {0x16,0x00};
	buf[1] = iTrebLevel;

	ReturnToUser(buf,2);
}

void returnAudioMainMidFreq(P_FLY_TDA7419_INFO pTda7419Info,BYTE iMidFreq)
{
	BYTE buf[] = {0x17,0x00};
	buf[1] = iMidFreq;

	ReturnToUser(buf,2);
}

void returnAudioMainMidLevel(P_FLY_TDA7419_INFO pTda7419Info,BYTE iMidLevel)
{
	BYTE buf[] = {0x18,0x00};
	buf[1] = iMidLevel;

	ReturnToUser(buf,2);
}

void returnAudioMainBassFreq(P_FLY_TDA7419_INFO pTda7419Info,BYTE iBassFreq)
{
	BYTE buf[] = {0x19,0x00};
	buf[1] = iBassFreq;

	ReturnToUser(buf,2);
}

void returnAudioMainBassLevel(P_FLY_TDA7419_INFO pTda7419Info,BYTE iBassLevel)
{
	BYTE buf[] = {0x1A,0x00};
	buf[1] = iBassLevel;

	ReturnToUser(buf,2);
}

void returnAudioMainLoudnessFreq(P_FLY_TDA7419_INFO pTda7419Info,BYTE iLoudnessFreq)
{
	BYTE buf[] = {0x1B,0x00};
	buf[1] = iLoudnessFreq;

	ReturnToUser(buf,2);
}

void returnAudioMainLoudnessLevel(P_FLY_TDA7419_INFO pTda7419Info,BYTE iLoudnessLevel)
{
	BYTE buf[] = {0x1C,0x00};
	buf[1] = iLoudnessLevel;

	ReturnToUser(buf,2);
}

void returnAudioEQValues(P_FLY_TDA7419_INFO pTda7419Info,BYTE *buf, UINT16 len)
{
	if (len != EQ_VOLUE_TOTAL_NUM)
		return;

	BYTE buff[EQ_VOLUE_TOTAL_NUM+1] = {0x1D};
	memcpy(&buff[1],buf,EQ_VOLUE_TOTAL_NUM);
	ReturnToUser(buff,EQ_VOLUE_TOTAL_NUM+1);
}

void returnAudioMainSimEQ(P_FLY_TDA7419_INFO pTda7419Info,BYTE iSimEQ)
{
	BYTE buf[] = {0x21,0x00};
	buf[1] = iSimEQ;

	ReturnToUser(buf,2);
}

void returnAudioMainLoudnessOn(P_FLY_TDA7419_INFO pTda7419Info,BOOL bLoudnessOn)
{
	BYTE buf[] = {0x30,0x00};
	if (bLoudnessOn)
	{
		buf[1] = 0x01;
	} 
	else
	{
		buf[1] = 0x00;
	}

	ReturnToUser(buf,2);
}

void returnAudioMainSubOn(P_FLY_TDA7419_INFO pTda7419Info,BOOL bSubOn)
{
	BYTE buf[] = {0x31,0x00};
	if (bSubOn)
	{
		buf[1] = 0x01;
	} 
	else
	{
		buf[1] = 0x00;
	}

	ReturnToUser(buf,2);
}

void returnAudioMainSubFilter(P_FLY_TDA7419_INFO pTda7419Info,BYTE iSubFilter)
{
	BYTE buf[] = {0x32,0x00};
	buf[1] = iSubFilter;

	ReturnToUser(buf,2);
}

void returnAudioMainSubLevel(P_FLY_TDA7419_INFO pTda7419Info,BYTE iSubLevel)
{
	BYTE buf[] = {0x33,0x00};
	buf[1] = iSubLevel;

	ReturnToUser(buf,2);
}

//void returnAudioEQAll(P_FLY_TDA7419_INFO pTda7419Info,BYTE *p)
//{
//	BYTE buf[10];
//	buf[0] = 0x0B;
//	memcpy(&buf[1],p,9);
//
//	ReturnToUser(buf,10);
//}

void Tda7419_ReadReg(P_FLY_TDA7419_INFO pTda7419Info,UCHAR regAddr)
{		
	BYTE Tda7419Reg[27];
	I2C_Read_Tda7419(pTda7419Info, regAddr, &Tda7419Reg[2], 24);

	DBGD((TEXT("FlyAudio Tda7419 REG[%X] is :"),regAddr));
	for(int i=0;i<24;i++)
	{
		DBGD((TEXT(" %X"),Tda7419Reg[2+i]));
	}
	Tda7419Reg[0]=0xfe;
	Tda7419Reg[1]=0xaa;
	ReturnToUser(Tda7419Reg,26);
}

static VOID DealWinceCmd(P_FLY_TDA7419_INFO pTda7419Info,BYTE *buf,UINT len)
{
	UINT i;
	DBGD((TEXT("\r\nTda7419 WinCE CMD:")));
	for (i = 0;i < len;i++)
	{
		DBGD((TEXT(" %X"),buf[i]));
	}
	
	if((pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp)||(pTda7419Info->bHaveGetCarInfo!=1))
	{
		switch(buf[0])
		{
			case 0x03://软件模拟按键
			case 0x11://mute
			case 0x12://volume
			case 0x13://balance
			case 0x14://fader
			case 0x15://treb freq
			case 0x16://treb
			case 0x17://mid freq
			case 0x18://mid 
			case 0x19://bass freq
			case 0x1a://bass
				DealEqControlData(pTda7419Info,buf,len);
				if(pTda7419Info->bHaveGetCarInfo!=1)
					break;
				return;
			default:
				break;
		}
	}
	switch(buf[0])
	{
		case 0x01:
		if (0x01 == buf[1])
		{
			returnAudioMainInput(pTda7419Info,pTda7419Info->FlyAudioInfo.preMainAudioInput);
			returnAudioMainBalance(pTda7419Info,pTda7419Info->FlyAudioInfo.preBalance);
			returnAudioMainFader(pTda7419Info,pTda7419Info->FlyAudioInfo.preFader);
			returnAudioMainTrebFreq(pTda7419Info,pTda7419Info->FlyAudioInfo.preTrebleFreq);
			returnAudioMainTrebLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preTrebleLevel);
			returnAudioMainMidFreq(pTda7419Info,pTda7419Info->FlyAudioInfo.preMidFreq);
			returnAudioMainMidLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preMidLevel);
			returnAudioMainBassFreq(pTda7419Info,pTda7419Info->FlyAudioInfo.preBassFreq);
			returnAudioMainBassLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preBassLevel);
			returnAudioMainLoudnessFreq(pTda7419Info,pTda7419Info->FlyAudioInfo.preLoudFreq);
			returnAudioMainLoudnessLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preLoudLevel);
			returnAudioMainLoudnessOn(pTda7419Info,pTda7419Info->FlyAudioInfo.preLoudnessOn);
			returnAudioMainSubOn(pTda7419Info,pTda7419Info->FlyAudioInfo.preSubOn);
			returnAudioMainSubFilter(pTda7419Info,pTda7419Info->FlyAudioInfo.preSubFilter);
			returnAudioMainSubLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preSubLevel);

			returnAudioMainSimEQ(pTda7419Info,pTda7419Info->FlyAudioInfo.preSimEQ);

			pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeAudioSleep = TRUE;
			pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeAudioSleep = FALSE;

			returnAudioPowerMode(pTda7419Info,TRUE);
			pTda7419Info->bPowerUp = TRUE;
			pTda7419Info->bNeedInit = TRUE;
			SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
		}
		else if (0x00 == buf[1])
		{
			returnAudioPowerMode(pTda7419Info,FALSE);
		}
		break;
	case 0x03:
		if (0x01 == buf[1])
		{
			if (pTda7419Info->FlyAudioInfo.preMainMute)
			{
				pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
				pTda7419Info->FlyAudioInfo.preMainMute = FALSE;
			} 
			else
			{
				pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
				pTda7419Info->FlyAudioInfo.preMainMute = TRUE;
			}
			IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);
			TDA7419_SendVolumeToUser(pTda7419Info);
			DBGD((TEXT("\r\nTda7419 MainMute %d"),pTda7419Info->FlyAudioInfo.preMainMute));
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainMute(pTda7419Info,pTda7419Info->FlyAudioInfo.preMainMute);
			}
		}
		else if (0x02 == buf[1])
		{
			pTda7419Info->FlyAudioInfo.preBalance++;
			if (pTda7419Info->FlyAudioInfo.preBalance >= BALANCE_LEVEL_COUNT)
			{
				pTda7419Info->FlyAudioInfo.preBalance = BALANCE_LEVEL_COUNT-1;
			}
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainBalance(pTda7419Info,pTda7419Info->FlyAudioInfo.preBalance);
			}
		}
		else if (0x03 == buf[1])
		{
			if (pTda7419Info->FlyAudioInfo.preBalance)
			{
				pTda7419Info->FlyAudioInfo.preBalance--;
			}
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainBalance(pTda7419Info,pTda7419Info->FlyAudioInfo.preBalance);
			}
		}
		else if (0x04 == buf[1])
		{
			pTda7419Info->FlyAudioInfo.preFader++;
			if (pTda7419Info->FlyAudioInfo.preFader >= FADER_LEVEL_COUNT)
			{
				pTda7419Info->FlyAudioInfo.preFader = FADER_LEVEL_COUNT-1;
			}
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainFader(pTda7419Info,pTda7419Info->FlyAudioInfo.preFader);
			}
		}
		else if (0x05 == buf[1])
		{
			if (pTda7419Info->FlyAudioInfo.preFader)
			{
				pTda7419Info->FlyAudioInfo.preFader--;
			}
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainFader(pTda7419Info,pTda7419Info->FlyAudioInfo.preFader);
			}
		}
		else if (0x06 == buf[1])
		{
			pTda7419Info->FlyAudioInfo.preTrebleFreq++;
			if (pTda7419Info->FlyAudioInfo.preTrebleFreq >= TREB_FREQ_COUNT)
			{
				pTda7419Info->FlyAudioInfo.preTrebleFreq = 0;
			}
			pTda7419Info->FlyAudioInfo.preSimEQ = 0;
			pTda7419Info->FlyAudioInfo.usrTrebleFreq = pTda7419Info->FlyAudioInfo.preTrebleFreq;
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainTrebFreq(pTda7419Info,pTda7419Info->FlyAudioInfo.preTrebleFreq);
			}
		}
		else if (0x07 == buf[1])
		{
			pTda7419Info->FlyAudioInfo.preTrebleLevel++;
			if (pTda7419Info->FlyAudioInfo.preTrebleLevel >= TREB_LEVEL_COUNT)
			{
				pTda7419Info->FlyAudioInfo.preTrebleLevel = TREB_LEVEL_COUNT-1;
			}
			pTda7419Info->FlyAudioInfo.preSimEQ = 0;
			pTda7419Info->FlyAudioInfo.usrTrebleLevel = pTda7419Info->FlyAudioInfo.preTrebleLevel;
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainTrebLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preTrebleLevel);
			}
		}
		else if (0x08 == buf[1])
		{
			if (pTda7419Info->FlyAudioInfo.preTrebleLevel)
			{
				pTda7419Info->FlyAudioInfo.preTrebleLevel--;
			}
			pTda7419Info->FlyAudioInfo.preSimEQ = 0;
			pTda7419Info->FlyAudioInfo.usrTrebleLevel = pTda7419Info->FlyAudioInfo.preTrebleLevel;
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainTrebLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preTrebleLevel);
			}
		}
		else if (0x09 == buf[1])
		{
			pTda7419Info->FlyAudioInfo.preMidFreq++;
			if (pTda7419Info->FlyAudioInfo.preMidFreq >= MID_FREQ_COUNT)
			{
				pTda7419Info->FlyAudioInfo.preMidFreq = 0;
			}
			pTda7419Info->FlyAudioInfo.preSimEQ = 0;
			pTda7419Info->FlyAudioInfo.usrMidFreq = pTda7419Info->FlyAudioInfo.preMidFreq;
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainMidFreq(pTda7419Info,pTda7419Info->FlyAudioInfo.preMidFreq);
			}
		}
		else if (0x0A == buf[1])
		{
			pTda7419Info->FlyAudioInfo.preMidLevel++;
			if (pTda7419Info->FlyAudioInfo.preMidLevel >= MID_LEVEL_COUNT)
			{
				pTda7419Info->FlyAudioInfo.preMidLevel = MID_LEVEL_COUNT-1;
			}
			pTda7419Info->FlyAudioInfo.preSimEQ = 0;
			pTda7419Info->FlyAudioInfo.usrMidLevel = pTda7419Info->FlyAudioInfo.preMidLevel;
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainMidLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preMidLevel);
			}
		}
		else if (0x0B == buf[1])
		{
			if (pTda7419Info->FlyAudioInfo.preMidLevel)
			{
				pTda7419Info->FlyAudioInfo.preMidLevel--;
			}
			pTda7419Info->FlyAudioInfo.preSimEQ = 0;
			pTda7419Info->FlyAudioInfo.usrMidLevel = pTda7419Info->FlyAudioInfo.preMidLevel;
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainMidLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preMidLevel);
			}
		}
		else if (0x0C == buf[1])
		{
			pTda7419Info->FlyAudioInfo.preBassFreq++;
			if (pTda7419Info->FlyAudioInfo.preBassFreq >= BASS_FREQ_COUNT)
			{
				pTda7419Info->FlyAudioInfo.preBassFreq = 0;
			}
			pTda7419Info->FlyAudioInfo.preSimEQ = 0;
			pTda7419Info->FlyAudioInfo.usrBassFreq = pTda7419Info->FlyAudioInfo.preBassFreq;
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainBassFreq(pTda7419Info,pTda7419Info->FlyAudioInfo.preBassFreq);
			}
		}
		else if (0x0D == buf[1])
		{
			pTda7419Info->FlyAudioInfo.preBassLevel++;
			if (pTda7419Info->FlyAudioInfo.preBassLevel >= BASS_LEVEL_COUNT)
			{
				pTda7419Info->FlyAudioInfo.preBassLevel = BASS_LEVEL_COUNT-1;
			}
			pTda7419Info->FlyAudioInfo.preSimEQ = 0;
			pTda7419Info->FlyAudioInfo.usrBassLevel = pTda7419Info->FlyAudioInfo.preBassLevel;
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainBassLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preBassLevel);
			}
		}
		else if (0x0E == buf[1])
		{
			if (pTda7419Info->FlyAudioInfo.preBassLevel)
			{
				pTda7419Info->FlyAudioInfo.preBassLevel--;
			}
			pTda7419Info->FlyAudioInfo.preSimEQ = 0;
			pTda7419Info->FlyAudioInfo.usrBassLevel = pTda7419Info->FlyAudioInfo.preBassLevel;
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainBassLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preBassLevel);
			}
		}
		else if (0x0F == buf[1])
		{
			pTda7419Info->FlyAudioInfo.preLoudFreq++;
			if (pTda7419Info->FlyAudioInfo.preLoudFreq >= LOUDNESS_FREQ_COUNT)
			{
				pTda7419Info->FlyAudioInfo.preLoudFreq = 0;
			}
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainLoudnessFreq(pTda7419Info,pTda7419Info->FlyAudioInfo.preLoudFreq);
			}
		}
		else if (0x10 == buf[1])
		{
			pTda7419Info->FlyAudioInfo.preLoudLevel++;
			if (pTda7419Info->FlyAudioInfo.preLoudLevel >= LOUDNESS_LEVEL_COUNT)
			{
				pTda7419Info->FlyAudioInfo.preLoudLevel = LOUDNESS_LEVEL_COUNT-1;
			}
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainLoudnessLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preLoudLevel);
			}
		}
		else if (0x11 == buf[1])
		{
			if (pTda7419Info->FlyAudioInfo.preLoudLevel)
			{
				pTda7419Info->FlyAudioInfo.preLoudLevel--;
			}
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainLoudnessLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preLoudLevel);
			}
		}
		else if (0x12 == buf[1])
		{
			pTda7419Info->FlyAudioInfo.preSubLevel++;
			if (pTda7419Info->FlyAudioInfo.preSubLevel >= SUB_LEVEL_COUNT)
			{
				pTda7419Info->FlyAudioInfo.preSubLevel = SUB_LEVEL_COUNT-1;
			}
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainSubLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preSubLevel);
			}
		}
		else if (0x13 == buf[1])
		{
			if (pTda7419Info->FlyAudioInfo.preSubLevel)
			{
				pTda7419Info->FlyAudioInfo.preSubLevel--;
			}
			if (pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				returnAudioMainSubLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preSubLevel);
			}
		}
		break;
	case 0x10:
		if (buf[1] == CAR_RECORDER) //行车记录仪不切音频通道
			return;

		pTda7419Info->FlyAudioInfo.preMainAudioInput = (BYTE)buf[1];
		pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eAudioInput = pTda7419Info->FlyAudioInfo.preMainAudioInput;
		IpcStartEvent(EVENT_GLOBAL_DVD_CHANNEL_ID);
		IpcStartEvent(EVENT_GLOBAL_A2DPAUDIO_CHANNEL_ID);
		IpcStartEvent(EVENT_GLOBAL_RADIO_ANTENNA_ID);
		IpcStartEvent(EVENT_GLOBAL_EXBOX_INPUT_CHANGE_ID);
		//IpcStartEvent(EVENT_GLOBAL_DVD_STOP_ID);
	
	
		DBGD((TEXT("\r\nTda7419 MainInput %d"),pTda7419Info->FlyAudioInfo.preMainAudioInput));
		if(pTda7419Info->bPowerUp)
		{
			SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
			ReturnToUser(buf,len);
		}
		break;
	case 0x11:
		if(1 == buf[1])
		{
			pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
			pTda7419Info->FlyAudioInfo.preMainMute = TRUE;
		}
		else
		{
			pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
			pTda7419Info->FlyAudioInfo.preMainMute = FALSE;
		}
		IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);
		TDA7419_SendVolumeToUser(pTda7419Info);
		DBGD((TEXT("\r\nTda7419 MainMute %d"),pTda7419Info->FlyAudioInfo.preMainMute));
		if(pTda7419Info->bPowerUp)
		{
			SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
			ReturnToUser(buf,len);
		}
		break;
	case 0x12:
		if (buf[1] <= 60)
		{
			pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume = buf[1];
			pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
			pTda7419Info->FlyAudioInfo.preMainVolume = buf[1];
			pTda7419Info->FlyAudioInfo.preMainMute = FALSE;

			IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);
			TDA7419_SendVolumeToUser(pTda7419Info);
			DBGD((TEXT("\r\nTda7419 MainVolume %d"),pTda7419Info->FlyAudioInfo.preMainVolume));

			DBGI((TEXT("\r\n Tda7419 BACK status bBackActiveNow:%d\r\n"),
				pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow));


			if(pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				ReturnToUser(buf,len);
			}
		}
		break;
	case 0x13:
		if (buf[1] < BALANCE_LEVEL_COUNT)
		{
			pTda7419Info->FlyAudioInfo.preBalance = buf[1];
			DBGD((TEXT("\r\nTda7419 Balance %d"),pTda7419Info->FlyAudioInfo.preBalance));
			if(pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				ReturnToUser(buf,len);
			}
		}
		break;
	case 0x14:
		if (buf[1] < FADER_LEVEL_COUNT)
		{
			pTda7419Info->FlyAudioInfo.preFader = buf[1];
			DBGD((TEXT("\r\nTda7419 Fader %d"),pTda7419Info->FlyAudioInfo.preFader));
			if(pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				ReturnToUser(buf,len);
			}
		}
		break;
	case 0x15:
		if (buf[1] < TREB_FREQ_COUNT)
		{
			pTda7419Info->FlyAudioInfo.preTrebleFreq = buf[1];
			pTda7419Info->FlyAudioInfo.preSimEQ = 0;
			pTda7419Info->FlyAudioInfo.usrTrebleFreq = pTda7419Info->FlyAudioInfo.preTrebleFreq;
			DBGD((TEXT("\r\nTda7419 Treble Freq %d"),pTda7419Info->FlyAudioInfo.preTrebleFreq));
			if(pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				ReturnToUser(buf,len);
			}
		}
		break;
	case 0x16:
		if (buf[1] < TREB_LEVEL_COUNT)
		{
			pTda7419Info->FlyAudioInfo.preTrebleLevel = buf[1];
			pTda7419Info->FlyAudioInfo.preSimEQ = 0;
			pTda7419Info->FlyAudioInfo.usrTrebleLevel = pTda7419Info->FlyAudioInfo.preTrebleLevel;
			DBGD((TEXT("\r\nTda7419 Treble Level %d"),pTda7419Info->FlyAudioInfo.preTrebleLevel));
			if(pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				ReturnToUser(buf,len);
			}
		}
		break;

	case 0x17:
		if (buf[1] < MID_FREQ_COUNT)
		{
			pTda7419Info->FlyAudioInfo.preMidFreq = buf[1];
			pTda7419Info->FlyAudioInfo.preSimEQ = 0;
			pTda7419Info->FlyAudioInfo.usrMidFreq = pTda7419Info->FlyAudioInfo.preMidFreq;
			DBGD((TEXT("\r\nTda7419 Mid Freq %d"),pTda7419Info->FlyAudioInfo.preMidFreq));
			if(pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				ReturnToUser(buf,len);
			}
		}
		break;
    case 0x18:
	    if (buf[1] < MID_LEVEL_COUNT)
	    {
			pTda7419Info->FlyAudioInfo.preMidLevel = buf[1];
			pTda7419Info->FlyAudioInfo.preSimEQ = 0;
			pTda7419Info->FlyAudioInfo.usrMidLevel = pTda7419Info->FlyAudioInfo.preMidLevel;
			DBGD((TEXT("\r\nTda7419 Mid Level %d"),pTda7419Info->FlyAudioInfo.preMidLevel));
			if(pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				ReturnToUser(buf,len);
			}
		}
		break;
	case 0x19 :
		if (buf[1] < BASS_FREQ_COUNT)
		{
			pTda7419Info->FlyAudioInfo.preBassFreq = buf[1];
			pTda7419Info->FlyAudioInfo.preSimEQ = 0;
			pTda7419Info->FlyAudioInfo.usrBassFreq = pTda7419Info->FlyAudioInfo.preBassFreq;
			DBGD((TEXT("\r\nTda7419 Bass Freq %d"),pTda7419Info->FlyAudioInfo.preBassFreq));
			if(pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				ReturnToUser(buf,len);
			}
		}
		break;
	case 0x1A:
		if (buf[1] < BASS_LEVEL_COUNT)
		{
			pTda7419Info->FlyAudioInfo.preBassLevel = buf[1];
			pTda7419Info->FlyAudioInfo.preSimEQ = 0;
			pTda7419Info->FlyAudioInfo.usrBassLevel = pTda7419Info->FlyAudioInfo.preBassLevel;
			DBGD((TEXT("\r\nTda7419 Bass Level %d"),pTda7419Info->FlyAudioInfo.preBassLevel));
			if(pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				ReturnToUser(buf,len);
			}
		}
		break;
	case 0x1B:
		if (buf[1] < LOUDNESS_FREQ_COUNT)
		{
			pTda7419Info->FlyAudioInfo.preLoudFreq = buf[1];
			DBGD((TEXT("\r\nTda7419 Loud Freq %d"),pTda7419Info->FlyAudioInfo.preLoudFreq));
			if(pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				ReturnToUser(buf,len);
			}
		}
		break;
	case 0x1C:
		if (buf[1] < LOUDNESS_LEVEL_COUNT)
		{
			pTda7419Info->FlyAudioInfo.preLoudLevel = buf[1];
			DBGD((TEXT("\r\nTda7419 Loud Level %d"),pTda7419Info->FlyAudioInfo.preLoudLevel));
			if(pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				ReturnToUser(buf,len);
			}
		}
		break;

	case 0x1D:
		if (len-1 >= EQ_VOLUE_TOTAL_NUM)
		{
			memcpy(&pTda7419Info->FlyAudioInfo.curEQValues[0],&buf[1],EQ_VOLUE_TOTAL_NUM);
			controlEQValues(pTda7419Info,&pTda7419Info->FlyAudioInfo.curEQValues[0],EQ_VOLUE_TOTAL_NUM);
			ReturnToUser(buf,len);
		}
		break;

	case 0x1E:
		if (buf[1] < EQ_VOLUE_TOTAL_NUM)
		{
			pTda7419Info->FlyAudioInfo.curEQValues[buf[1]] = buf[2];
			controlEQValues(pTda7419Info,&pTda7419Info->FlyAudioInfo.curEQValues[0],EQ_VOLUE_TOTAL_NUM);
			ReturnToUser(buf,len);
		}
		break;

	case 0x20:

		break;


	case 0x21:
		pTda7419Info->FlyAudioInfo.preSimEQ = buf[1];
		if (buf[1] <= 0x05)
		{
			if (0x01 == buf[1] || 0x00 == buf[1])//缺省
			{
				for (i=0; i<EQ_VOLUE_TOTAL_NUM; i++)
					pTda7419Info->FlyAudioInfo.curEQValues[i] = 6;
			}
			else if (0x02 == buf[1])//古典
			{
				if (EQ_VOLUE_TOTAL_NUM == 10)
				{
					pTda7419Info->FlyAudioInfo.curEQValues[0] = 6;
					pTda7419Info->FlyAudioInfo.curEQValues[1] = 10;
					pTda7419Info->FlyAudioInfo.curEQValues[2] = 5;
					pTda7419Info->FlyAudioInfo.curEQValues[3] = 6;
					pTda7419Info->FlyAudioInfo.curEQValues[4] = 6;
					pTda7419Info->FlyAudioInfo.curEQValues[5] = 7;
					pTda7419Info->FlyAudioInfo.curEQValues[6] = 6;
					pTda7419Info->FlyAudioInfo.curEQValues[7] = 8;
					pTda7419Info->FlyAudioInfo.curEQValues[8] = 10;
					pTda7419Info->FlyAudioInfo.curEQValues[9] = 9;
				}
				else if (EQ_VOLUE_TOTAL_NUM == 9)
				{
					pTda7419Info->FlyAudioInfo.curEQValues[0] = 10;
					pTda7419Info->FlyAudioInfo.curEQValues[1] = 5;
					pTda7419Info->FlyAudioInfo.curEQValues[2] = 6;
					pTda7419Info->FlyAudioInfo.curEQValues[3] = 6;
					pTda7419Info->FlyAudioInfo.curEQValues[4] = 7;
					pTda7419Info->FlyAudioInfo.curEQValues[5] = 6;
					pTda7419Info->FlyAudioInfo.curEQValues[6] = 8;
					pTda7419Info->FlyAudioInfo.curEQValues[7] = 10;
					pTda7419Info->FlyAudioInfo.curEQValues[8] = 9;
				}

			}
			else if (0x03 == buf[1])//流行
			{
				if (EQ_VOLUE_TOTAL_NUM == 10)
				{
					pTda7419Info->FlyAudioInfo.curEQValues[0] = 6;
					pTda7419Info->FlyAudioInfo.curEQValues[1] = 8;
					pTda7419Info->FlyAudioInfo.curEQValues[2] = 4;
					pTda7419Info->FlyAudioInfo.curEQValues[3] = 7;
					pTda7419Info->FlyAudioInfo.curEQValues[4] = 5;
					pTda7419Info->FlyAudioInfo.curEQValues[5] = 7;
					pTda7419Info->FlyAudioInfo.curEQValues[6] = 5;
					pTda7419Info->FlyAudioInfo.curEQValues[7] = 7;
					pTda7419Info->FlyAudioInfo.curEQValues[8] = 9;
					pTda7419Info->FlyAudioInfo.curEQValues[9] = 8;
				}
				else if (EQ_VOLUE_TOTAL_NUM == 9)
				{	
					pTda7419Info->FlyAudioInfo.curEQValues[0] = 8;
					pTda7419Info->FlyAudioInfo.curEQValues[1] = 4;
					pTda7419Info->FlyAudioInfo.curEQValues[2] = 7;
					pTda7419Info->FlyAudioInfo.curEQValues[3] = 5;
					pTda7419Info->FlyAudioInfo.curEQValues[4] = 7;
					pTda7419Info->FlyAudioInfo.curEQValues[5] = 5;
					pTda7419Info->FlyAudioInfo.curEQValues[6] = 7;
					pTda7419Info->FlyAudioInfo.curEQValues[7] = 9;
					pTda7419Info->FlyAudioInfo.curEQValues[8] = 8;
				}

			}
			else if (0x04 == buf[1])//摇滚
			{
				if (EQ_VOLUE_TOTAL_NUM == 10)
				{
					pTda7419Info->FlyAudioInfo.curEQValues[0] = 6;
					pTda7419Info->FlyAudioInfo.curEQValues[1] = 10;
					pTda7419Info->FlyAudioInfo.curEQValues[2] = 11;
					pTda7419Info->FlyAudioInfo.curEQValues[3] = 9;
					pTda7419Info->FlyAudioInfo.curEQValues[4] = 6;
					pTda7419Info->FlyAudioInfo.curEQValues[5] = 7;
					pTda7419Info->FlyAudioInfo.curEQValues[6] = 7;
					pTda7419Info->FlyAudioInfo.curEQValues[7] = 9;
					pTda7419Info->FlyAudioInfo.curEQValues[8] = 10;
					pTda7419Info->FlyAudioInfo.curEQValues[9] = 11;
				}
				else if (EQ_VOLUE_TOTAL_NUM == 9)
				{
					pTda7419Info->FlyAudioInfo.curEQValues[0] = 10;
					pTda7419Info->FlyAudioInfo.curEQValues[1] = 11;
					pTda7419Info->FlyAudioInfo.curEQValues[2] = 9;
					pTda7419Info->FlyAudioInfo.curEQValues[3] = 6;
					pTda7419Info->FlyAudioInfo.curEQValues[4] = 7;
					pTda7419Info->FlyAudioInfo.curEQValues[5] = 7;
					pTda7419Info->FlyAudioInfo.curEQValues[6] = 9;
					pTda7419Info->FlyAudioInfo.curEQValues[7] = 10;
					pTda7419Info->FlyAudioInfo.curEQValues[8] = 11;
				}

			}
			else if (0x05 == buf[1])//爵士
			{
				if (EQ_VOLUE_TOTAL_NUM == 10)
				{
					pTda7419Info->FlyAudioInfo.curEQValues[0] = 6;
					pTda7419Info->FlyAudioInfo.curEQValues[1] = 8;
					pTda7419Info->FlyAudioInfo.curEQValues[2] = 10;
					pTda7419Info->FlyAudioInfo.curEQValues[3] = 6;
					pTda7419Info->FlyAudioInfo.curEQValues[4] = 6;
					pTda7419Info->FlyAudioInfo.curEQValues[5] = 8;
					pTda7419Info->FlyAudioInfo.curEQValues[6] = 10;
					pTda7419Info->FlyAudioInfo.curEQValues[7] = 10;
					pTda7419Info->FlyAudioInfo.curEQValues[8] = 8;
					pTda7419Info->FlyAudioInfo.curEQValues[9] = 9;
				}
				else if (EQ_VOLUE_TOTAL_NUM == 9)
				{
					pTda7419Info->FlyAudioInfo.curEQValues[0] = 8;
					pTda7419Info->FlyAudioInfo.curEQValues[1] = 10;
					pTda7419Info->FlyAudioInfo.curEQValues[2] = 6;
					pTda7419Info->FlyAudioInfo.curEQValues[3] = 6;
					pTda7419Info->FlyAudioInfo.curEQValues[4] = 8;
					pTda7419Info->FlyAudioInfo.curEQValues[5] = 10;
					pTda7419Info->FlyAudioInfo.curEQValues[6] = 10;
					pTda7419Info->FlyAudioInfo.curEQValues[7] = 8;
					pTda7419Info->FlyAudioInfo.curEQValues[8] = 9;
				}

			}

			controlEQValues(pTda7419Info,&pTda7419Info->FlyAudioInfo.curEQValues[0],EQ_VOLUE_TOTAL_NUM);
			returnAudioEQValues(pTda7419Info,&pTda7419Info->FlyAudioInfo.curEQValues[0],EQ_VOLUE_TOTAL_NUM);
		}
		ReturnToUser(buf,len);
		break;

	case 0x22:

		break;

	case 0x24:
		volumeFaderInOut(pTda7419Info,TRUE);
		if (0x01 == buf[1])
		{
			pTda7419Info->FlyAudioInfo.preGPSSpeaker = buf[1];
			DBGI((TEXT("\r\nTda7419 GPSSpeaker %d"),pTda7419Info->FlyAudioInfo.preGPSSpeaker));
			if(pTda7419Info->bPowerUp)
			{
				ReturnToUser(buf,len);
				//if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.GPSSpeaker != buf[1])
				{
					pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.GPSSpeaker = buf[1];
					SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				}
			}
		}
		else if (0x00 == buf[1])
		{
			pTda7419Info->FlyAudioInfo.preGPSSpeaker = buf[1];
			DBGI((TEXT("\r\nTda7419 GPSSpeaker per:%d,glo:%d"),pTda7419Info->FlyAudioInfo.preGPSSpeaker,
				pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.GPSSpeaker));
			if(pTda7419Info->bPowerUp)
			{
				ReturnToUser(buf,len);
				//if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.GPSSpeaker != buf[1])
				{
					pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.GPSSpeaker = buf[1];
					SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				}
			}
		}
	break;

	case 0x30:
		if (0x01 == buf[1])
		{
			pTda7419Info->FlyAudioInfo.preLoudnessOn = TRUE;
		} 
		else
		{
			pTda7419Info->FlyAudioInfo.preLoudnessOn = FALSE;
		}
		if(pTda7419Info->bPowerUp)
		{
			SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
			ReturnToUser(buf,len);
		}
		break;
	case 0x31:
		if (0x01 == buf[1])
		{
			pTda7419Info->FlyAudioInfo.preSubOn = TRUE;
		} 
		else
		{
			pTda7419Info->FlyAudioInfo.preSubOn = FALSE;
		}
		if(pTda7419Info->bPowerUp)
		{
			SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
			ReturnToUser(buf,len);
		}
		break;
	case 0x32:
		if (buf[1] < SUB_FILTER_COUNT)
		{
			pTda7419Info->FlyAudioInfo.preSubLevel = buf[1];
			DBGD((TEXT("\r\nTda7419 Sub Filter %d"),pTda7419Info->FlyAudioInfo.preSubLevel));
			if(pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				ReturnToUser(buf,len);
			}
		}
		break;
	case 0x33:
		if (buf[1] < SUB_LEVEL_COUNT)
		{
			pTda7419Info->FlyAudioInfo.preSubLevel = buf[1];
			DBGD((TEXT("\r\nTda7419 Sub Level %d"),pTda7419Info->FlyAudioInfo.preSubLevel));
			if(pTda7419Info->bPowerUp)
			{
				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				ReturnToUser(buf,len);
			}
		}
		break;

	case 0x80://特殊车型功放信息
		{
			BYTE buff[50]={0x05};

			if(len>49)
				len=49;
			memcpy(&buff[1],buf,len);
			writeCarAmpDataToMcu(pTda7419Info,buff,len+1);
		}
		break;
	case 0xFF:
		if (0x01 == buf[1])
		{
			FAU_PowerUp((DWORD)pTda7419Info);
		} 
		else if (0x00 == buf[1])
		{
			FAU_PowerDown((DWORD)pTda7419Info);
		}
		break;

		//case 0xE0:
		//	DBGD((TEXT("\r\nFlyAudio Write To STM32 Test")));
		//	I2C_Write_STM32(pTda7419Info->Tda7419I2CHandle,buf[2],&buf[3],len-3);
		//	break;
		//case 0xE1:
		//	DBGD((TEXT("\r\nFlyAudio Read From STM32 Test")));
		//	I2C_Read_STM32(pTda7419Info->Tda7419I2CHandle,buf[2],&buf[3],len-3);
		//	ReturnToUser(buf,len);
		//	break;
	default:
		DBGD((TEXT("\r\nTda7419 user command unhandle %X"),buf[0]));
		break;
	}
	//case 0xfe:
	//	if(buf[1] == 0x55)
	//	{
	//		pTda7419Info->FlyAudioInfo.ReadRegFlag = TRUE;
	//		pTda7419Info->FlyAudioInfo.preReadReg = (buf[2] << 16) + (buf[3] << 8) + buf[4];
	//		DBGD((TEXT("\r\nTda7419 ReadReg Addr %X"),pTda7419Info->FlyAudioInfo.preReadReg));
	//		if(pTda7419Info->bPowerUp)
	//		{
	//			SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
	//		}
	//	}
	//	break;
}

//#define ADSP_X_Gsa_Bar321                          0x3C070D//0x0D073C//
//#define ADSP_X_Gsa_Bar654                          0x3B070D//0x0D073B//
//#define ADSP_X_Gsa_Bar987                          0x3A070D//0x0D073A//

//void Tda7419_ReadGraphicalSpectrumAnalyzer(P_FLY_Tda7419_INFO pTda7419Info)//寄存器地址，字节MSB，Prima先传低字节,7419先接受高字节
//{
//	BYTE Value[3];//7419先发高字节，Prima先存低字节
//	BYTE commandValue[] = {0x01,0xF0,1,2,3,4,5,6,7,8,9};
//	return;
//	I2C_Read_Tda7419(pTda7419Info, ADSP_X_Gsa_Bar321, Value, 3);
//	pTda7419Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[0] = Value[2];
//	pTda7419Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[1] = Value[1];
//	pTda7419Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[2] = Value[0];
//	Sleep(10);
//	I2C_Read_Tda7419(pTda7419Info, ADSP_X_Gsa_Bar654, Value, 3);
//	pTda7419Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[3] = Value[2];
//	pTda7419Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[4] = Value[1];
//	pTda7419Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[5] = Value[0];
//	Sleep(10);
//	I2C_Read_Tda7419(pTda7419Info, ADSP_X_Gsa_Bar987, Value, 3);
//	pTda7419Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[6] = Value[2];
//	pTda7419Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[7] = Value[1];
//	pTda7419Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[8] = Value[0];
//	Sleep(10);
//	memcpy((VOID *)&commandValue[2],(VOID *)&pTda7419Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[0],9);
//	ReturnToUser(commandValue,11);
//}

//bDelayOn为TRUE时，iDelayStage慢慢递增
void volumeFaderInOut(P_FLY_TDA7419_INFO pTda7419Info,BOOL bEnable)
{
	if (bEnable)
	{
		pTda7419Info->FlyAudioInfo.bEnableVolumeFader = TRUE;
	}
	else
	{
		pTda7419Info->FlyAudioInfo.bEnableVolumeFader = FALSE;
	}
}

/****************************************************************/
/**	                 Write data to MCU			               **/
/****************************************************************/
void writeCarAmpDataToMcu(P_FLY_TDA7419_INFO pTda7419Info,BYTE *p,UINT length)
{
	UINT16 i=0;

	if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.bInit)
	{
		if(FALSE == WriteMsgQueue(pTda7419Info->hMsgQueueFromCarAmpCreate, p, length, 0, 0))
		{
			DWORD dwError = GetLastError();
			DBGE((TEXT("\r\nCarAmp Write MsgQueue To CarBody Error!0x%x"),dwError));
				switch(dwError)
			{
				case ERROR_INSUFFICIENT_BUFFER:
					DBGE((TEXT("\r\nCarAmp Write MsgQueue To CarBody Error-ERROR_INSUFFICIENT_BUFFER")));
						break;
				case ERROR_OUTOFMEMORY:
					DBGE((TEXT("\r\nCarAmp Write MsgQueue To CarBody Error-ERROR_OUTOFMEMORY")));
						break;				
				case ERROR_PIPE_NOT_CONNECTED:
					DBGE((TEXT("\r\nCarAmp Write MsgQueue To CarBody Error-ERROR_PIPE_NOT_CONNECTED")));
						break;
				case ERROR_TIMEOUT:
					DBGE((TEXT("\r\nCarAmp Write MsgQueue To CarBody Error!ERROR_TIMEOUT")));
						break;
				default:
					DBGE((TEXT("\r\nCarAmp Write MsgQueue To CarBody Error!-unknow error")));
						break;
			}				 

		}
	}
	else
	{
		DBGD((TEXT("\r\nCarAmp Write MsgQueue To CarBody Not Init")));
			for (i = 0;i < length;i++)
			{
				DBGD((TEXT(" %x"),p[i]));
			}
	}
}
/****************************************************************/
/**	                 Write data to MCU			               **/
/****************************************************************/
void writeDataToMcu(P_FLY_TDA7419_INFO pTda7419Info,BYTE *p,UINT length)
{
	UINT16 i=0;

	if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.bInit)
	{
		if(FALSE == WriteMsgQueue(pTda7419Info->hMsgQueueFromExtAmpCreate, p, length, 0, 0))
		{
			DWORD dwError = GetLastError();
			DBGE((TEXT("\r\nExtAmp Write MsgQueue To CarBody Error!0x%x"),dwError));
				switch(dwError)
			{
				case ERROR_INSUFFICIENT_BUFFER:
					DBGE((TEXT("\r\nExtAmp Write MsgQueue To CarBody Error-ERROR_INSUFFICIENT_BUFFER")));
						break;
				case ERROR_OUTOFMEMORY:
					DBGE((TEXT("\r\nExtAmp Write MsgQueue To CarBody Error-ERROR_OUTOFMEMORY")));
						break;				
				case ERROR_PIPE_NOT_CONNECTED:
					DBGE((TEXT("\r\nExtAmp Write MsgQueue To CarBody Error-ERROR_PIPE_NOT_CONNECTED")));
						break;
				case ERROR_TIMEOUT:
					DBGE((TEXT("\r\nExtAmp Write MsgQueue To CarBody Error!ERROR_TIMEOUT")));
						break;
				default:
					DBGE((TEXT("\r\nExtAmp Write MsgQueue To CarBody Error!-unknow error")));
						break;
			}				 

		}
	}
	else
	{
		DBGD((TEXT("\r\nExtAmp Write MsgQueue To CarBody Not Init")));
			for (i = 0;i < length;i++)
			{
				DBGD((TEXT(" %x"),p[i]));
			}
	}
}
/****************************************************************/
/**	                 Read data From MCU			               **/
/****************************************************************/
static DWORD WINAPI ThreadExtAmpRead(LPVOID pContext)
{
	P_FLY_TDA7419_INFO pTda7419Info = (P_FLY_TDA7419_INFO)pContext;
	ULONG WaitReturn;
	BYTE buff[FLY_MSG_QUEUE_TO_EXTAMP_LENGTH];
	DWORD dwRes;
	DWORD dwMsgFlag;

	while (!pTda7419Info->bKillDispatchFlyMsgQueueExtAmpReadThread)
	{
		WaitReturn = WaitForSingleObject(pTda7419Info->hMsgQueueToExtAmpCreate, INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nFlyAudio ExtAmp ThreadRead Event hMsgQueueToExtAmp Open Error")));
		}
		ReadMsgQueue(pTda7419Info->hMsgQueueToExtAmpCreate, buff, FLY_MSG_QUEUE_TO_EXTAMP_LENGTH, &dwRes, 0, &dwMsgFlag);

		if(dwRes)
		{
			DBGD((TEXT("ExtAmp Read MsgQueue%d %x %x"),dwRes,buff[0],buff[1]));
				switch (buff[0])
			{		
				case 0x01:
					if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCheckShellBabyError)
					{
						break;
					}
					if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable
						&& pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
					{
						break;
					}
					if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute)
					{
						pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
						pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume = 0;
					}
					pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume =buff[1];						
					if (0 == pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume)
					{
						pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
					}
					IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);	
					TDA7419_SendVolumeToUser(pTda7419Info);
					break;
				case 0x02:
					if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable
						&& pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
					{
						break;
					}
					if (0x00 == buff[1])
					{
						pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
					}
					else if (0x01 == buff[1])
					{
						pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
					}
					if (0 == pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume)
					{
						pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
					}
					IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);
					TDA7419_SendVolumeToUser(pTda7419Info);
					break;
				case 0x89:
					if(0xFA == buff[1])
					{
						pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveFlyAudioExtAMP=TRUE;
						if (pTda7419Info->bPowerUp)
						{
							SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
						}
						FA8200LowVoltageRecovery(pTda7419Info);
						DBGD((TEXT("\r\nFlyaudio amplifier exists!!!")));
					}
					break;
				default:
					DBGD((TEXT("\r\nFlyAudio ExtAmp ThreadRead Unhandle%d %x %x"),dwRes,buff[0],buff[1]));
						break;
			}
		}
	}
	DBGD((TEXT("\r\nFlyAudio ExtAmp ThreadRead exit")));
		pTda7419Info->FLyMsgQueueExtAmpReadThreadHandle = NULL;
	return 0;
}

/****************************************************************/
/**	                 Read car amp data From MCU			       **/
/****************************************************************/
static DWORD WINAPI ThreadCarAmpRead(LPVOID pContext)
{
	P_FLY_TDA7419_INFO pTda7419Info = (P_FLY_TDA7419_INFO)pContext;
	ULONG WaitReturn;
	BYTE buff[FLY_MSG_QUEUE_TO_CARAMP_LENGTH];
	DWORD dwRes;
	DWORD dwMsgFlag;

	while (!pTda7419Info->bKillDispatchFlyMsgQueueCarAmpReadThread)
	{
		WaitReturn = WaitForSingleObject(pTda7419Info->hMsgQueueToCarAmpCreate, INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nFlyAudio CarAmp ThreadRead Event hMsgQueueToExtAmp Open Error")));
		}
		ReadMsgQueue(pTda7419Info->hMsgQueueToCarAmpCreate, buff, FLY_MSG_QUEUE_TO_CARAMP_LENGTH, &dwRes, 0, &dwMsgFlag);

		if(dwRes)
		{
			DBGD((TEXT("CarAmp Read MsgQueue%d %x %x"),dwRes,buff[0],buff[1]));
			switch (buff[0])
			{		
				case 0x10://volume
					if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCheckShellBabyError)
					{
						break;
					}
					if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable
						&& pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
					{
						break;
					}
					if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute)
					{
						pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
						pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume = 0;
					}
					pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume =buff[1];						
					if (0 == pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume)
					{
						pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
					}
					IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);	
					TDA7419_SendVolumeToUser(pTda7419Info);
					break;
				case 0x11://treb
					pTda7419Info->FlyAudioInfo.preTrebleLevel =buff[1];
					pTda7419Info->FlyAudioInfo.preSimEQ = 0;
					pTda7419Info->FlyAudioInfo.usrTrebleLevel = pTda7419Info->FlyAudioInfo.preTrebleLevel;
					if (pTda7419Info->bPowerUp)
					{
						SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
						returnAudioMainTrebLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preTrebleLevel);
					}
					break;
				case 0x12://mid
					pTda7419Info->FlyAudioInfo.preMidLevel=buff[1];
					pTda7419Info->FlyAudioInfo.preSimEQ = 0;
					pTda7419Info->FlyAudioInfo.usrMidLevel= pTda7419Info->FlyAudioInfo.preMidLevel;
					if (pTda7419Info->bPowerUp)
					{
						SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
						returnAudioMainMidLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preMidLevel);
					}
					break;
				case 0x13://bass
					pTda7419Info->FlyAudioInfo.preBassLevel=buff[1];
					pTda7419Info->FlyAudioInfo.preSimEQ = 0;
					pTda7419Info->FlyAudioInfo.usrBassLevel= pTda7419Info->FlyAudioInfo.preBassLevel;
					if (pTda7419Info->bPowerUp)
					{
						SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
						returnAudioMainBassLevel(pTda7419Info,pTda7419Info->FlyAudioInfo.preBassLevel);
					}
					break;
				case 0x14://bal
					pTda7419Info->FlyAudioInfo.preBalance=buff[1];
					if (pTda7419Info->bPowerUp)
					{
						SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
						returnAudioMainBalance(pTda7419Info,pTda7419Info->FlyAudioInfo.preBalance);
					}
					break;
				case 0x15://fader
					pTda7419Info->FlyAudioInfo.preFader=buff[1];
					if (pTda7419Info->bPowerUp)
					{
						SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
						returnAudioMainFader(pTda7419Info,pTda7419Info->FlyAudioInfo.preFader);
					}
					break;
				case 0x16://mute
					if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable
						&& pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
					{
						break;
					}
					if (0x00 == buff[1])
					{
						pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
					}
					else if (0x01 == buff[1])
					{
						pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
					}
					if (0 == pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume)
					{
						pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
					}
					IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);
					TDA7419_SendVolumeToUser(pTda7419Info);
					break;
				case 0x80://特殊车型功放信息
					ReturnToUser(buff,(UINT16)dwRes);
					break;
				default:
					DBGD((TEXT("\r\nFlyAudio Car Amp ThreadRead Unhandle%d %x %x"),dwRes,buff[0],buff[1]));
						break;
			}
		}
	}
	DBGD((TEXT("\r\nFlyAudio CarAmp ThreadRead exit")));
		pTda7419Info->FLyMsgQueueCarAmpReadThreadHandle = NULL;
	return 0;
}


BOOL bVolumeFaderInOut(P_FLY_TDA7419_INFO pTda7419Info)
{
	return pTda7419Info->FlyAudioInfo.bEnableVolumeFader;
}


static DWORD WINAPI Tda7419DelayThread(LPVOID pContext)
{
	P_FLY_TDA7419_INFO pTda7419Info = (P_FLY_TDA7419_INFO)pContext;
	ULONG WaitReturn;
	BOOL bDelayOn = FALSE;BYTE iDelayStage = 0;BOOL bDelayEnough = FALSE;UINT iDelayTime = 100;DWORD iLastTime,iNowTime;
	BOOL bMute = FALSE;

	while (!pTda7419Info->bKillDispatchTda7419DelayThread)
	{
		if (bDelayOn)//处理任务中
		{
			iNowTime = GetTickCount();
			if (iNowTime - iLastTime < iDelayTime)
			{
				WaitReturn = WaitForSingleObject(pTda7419Info->hDispatchTda7419DelayThreadEvent, iDelayTime - (iNowTime - iLastTime));
			}
			else
			{
				bDelayEnough = TRUE;
			}
			if (!bDelayEnough)//补充
			{
				iNowTime = GetTickCount();
				if (iNowTime - iLastTime >= iDelayTime)
				{
					bDelayEnough = TRUE;
				}
			}
		}
		else
		{
			WaitReturn = WaitForSingleObject(pTda7419Info->hDispatchTda7419DelayThreadEvent, INFINITE);
		}
		
		//RETAILMSG(1, (TEXT("\r\nTda7419 X ON(%d) Step(%d) F(%d) T(%d) CV(%d) PV(%d) Input(%d)")
		//	,bDelayOn,iDelayStage,bDelayEnough,iDelayTime
		//	,pTda7419Info->FlyAudioInfo.curMainVolume,pTda7419Info->FlyAudioInfo.preMainVolume
		//	,pTda7419Info->FlyAudioInfo.preMainAudioInput));
		//RETAILMSG(1, (TEXT("\r\n")));
		//for (i = 0;i < 18; i++)
		//{
		//	RETAILMSG(1, (TEXT(" %x"),TDA7419_Para[i]));
		//}

		
		if (FALSE == pTda7419Info->bPowerUp)//等待初始化
		{
			bDelayOn = FALSE;iDelayStage = 0;bDelayEnough = FALSE;iDelayTime = 100;iLastTime = GetTickCount();//跳过
			DBGD((TEXT("Wait PowerUp!")));
			continue;
		}


		if(pTda7419Info->FlyAudioInfo.curMainAudioInput != pTda7419Info->FlyAudioInfo.tmpMainAudioInput)//切换通道
		{
			if (FALSE == bDelayOn || (bDelayOn && iDelayStage > 2))//启动
			{
				bDelayOn = TRUE;iDelayStage = 1;bDelayEnough = FALSE;iDelayTime = 100;iLastTime = GetTickCount();
				volumeFaderInOut(pTda7419Info,TRUE);
			}
		}

		if (1 == iDelayStage)
		{
			DBGD((TEXT("\r\nTda7419 DelayThread MuteOn")));
			bDelayOn = TRUE;iDelayStage = 2;bDelayEnough = FALSE;iDelayTime = 314;iLastTime = GetTickCount();
			if (
				(pTda7419Info->FlyAudioInfo.curMainAudioInput == MediaMP3 && pTda7419Info->FlyAudioInfo.tmpMainAudioInput == IPOD)
				|| 
				(pTda7419Info->FlyAudioInfo.curMainAudioInput == IPOD && pTda7419Info->FlyAudioInfo.tmpMainAudioInput == MediaMP3)
				)
			{
			}
			else if (
				(pTda7419Info->FlyAudioInfo.curMainAudioInput == BT && pTda7419Info->FlyAudioInfo.tmpMainAudioInput == A2DP)
				|| 
				(pTda7419Info->FlyAudioInfo.curMainAudioInput == A2DP && pTda7419Info->FlyAudioInfo.tmpMainAudioInput == BT)
				)
			{
			}
			else if (FALSE == bMute)//声道切换时静音
			{
				DBGD((TEXT(" Run")));
				bMute = TRUE;
				pTda7419Info->FlyAudioInfo.curMainVolume = 0;
				TDA7419_Volume(pTda7419Info,pTda7419Info->FlyAudioInfo.curMainVolume);
				pTda7419Info->FlyAudioInfo.curMainMute = TRUE;
				Tda7419_Mute(pTda7419Info,pTda7419Info->FlyAudioInfo.curMainMute);
			}
		}
		else if (2 == iDelayStage && bDelayEnough)//切换
		{
			bDelayOn = TRUE;iDelayStage = 3;bDelayEnough = FALSE;iDelayTime = 314;iLastTime = GetTickCount();

			if (pTda7419Info->FlyAudioInfo.curMainAudioInput != pTda7419Info->FlyAudioInfo.tmpMainAudioInput)
			{
				DBGD((TEXT("\r\nTda7419 DelayThread ChangeInput:%d,preMainVolume volume:%d,curMainVolume volume:%d"),
					pTda7419Info->FlyAudioInfo.tmpMainAudioInput,
					pTda7419Info->FlyAudioInfo.preMainVolume,
					pTda7419Info->FlyAudioInfo.curMainVolume));

				//独立音量控制
				if (EXT_TEL == pTda7419Info->FlyAudioInfo.curMainAudioInput)//保存当前
				{
					pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iExtTelVolume
						= pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
				}
				else if (BT == pTda7419Info->FlyAudioInfo.curMainAudioInput
					|| BT_RING == pTda7419Info->FlyAudioInfo.curMainAudioInput)
				{
					//pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume
					//	= pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
					
					//pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume = 
					//	GetVolume4BTRingVolume(pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax);
				}
				else
				{
						pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNormalVolume
							= pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
						pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNormalMute
							= pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute;
				}

				if (EXT_TEL == pTda7419Info->FlyAudioInfo.tmpMainAudioInput)//恢复以前
				{
					pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume
						= pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iExtTelVolume;
					pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
					
					if(pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp)//如果原车功放可以直接控制音量
					{
						if(EXT_TEL != pTda7419Info->FlyAudioInfo.curMainAudioInput)
						{
							CarVolumeControl(pTda7419Info,pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume);	
						}
					}
				}
				else if (BT == pTda7419Info->FlyAudioInfo.tmpMainAudioInput
					|| BT_RING == pTda7419Info->FlyAudioInfo.tmpMainAudioInput)
				{
					pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume
						= pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume;
					pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
					
					if(pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp)
					{
						if(BT != pTda7419Info->FlyAudioInfo.curMainAudioInput
						&& BT_RING != pTda7419Info->FlyAudioInfo.curMainAudioInput)
						{
							CarVolumeControl(pTda7419Info,pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume);	
						}
					}
				}
				else
				{		
					pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume
						= pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNormalVolume;
					pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute
						= pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNormalMute;
				
					if(pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp)
					{
						if(EXT_TEL == pTda7419Info->FlyAudioInfo.curMainAudioInput
						||BT == pTda7419Info->FlyAudioInfo.curMainAudioInput
						||BT_RING == pTda7419Info->FlyAudioInfo.curMainAudioInput)
						{
							CarVolumeControl(pTda7419Info,pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume);
						}
					}	
				}

				if (EXT_TEL == pTda7419Info->FlyAudioInfo.curMainAudioInput
					|| BT == pTda7419Info->FlyAudioInfo.curMainAudioInput
					|| BT_RING == pTda7419Info->FlyAudioInfo.curMainAudioInput

					|| EXT_TEL == pTda7419Info->FlyAudioInfo.tmpMainAudioInput
					|| BT == pTda7419Info->FlyAudioInfo.tmpMainAudioInput
					|| BT_RING == pTda7419Info->FlyAudioInfo.tmpMainAudioInput)
				{
					IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);
					TDA7419_SendVolumeToUser(pTda7419Info);
				}

				//独立音量控制
				pTda7419Info->FlyAudioInfo.curMainAudioInput = pTda7419Info->FlyAudioInfo.tmpMainAudioInput;
				pTda7419Info->FlyAudioInfo.dspMainAudioInput = pTda7419Info->FlyAudioInfo.tmpMainAudioInput;

				pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eCurAudioInput = pTda7419Info->FlyAudioInfo.curMainAudioInput;

				Tda7419_Input(pTda7419Info,pTda7419Info->FlyAudioInfo.curMainAudioInput,audioChannelGainTab[pTda7419Info->FlyAudioInfo.curMainAudioInput]);
				Tda7419_TreMidBass_ForRadio(pTda7419Info);

				//MasterSlaveAudioMainChannel(pTda7419Info,pTda7419Info->FlyAudioInfo.curMainAudioInput);
				//设定恢复时间
				if (iDelayTime < 618)
				{
					iDelayTime = 618;
				}
			}

		}

		if (IpcWhatEvent(EVENT_GLOBAL_RADIO_MUTE_IN_REQ_ID))//赋值
		{
			pTda7419Info->FlyAudioInfo.bMuteRadio = TRUE;
		}
		if (IpcWhatEvent(EVENT_GLOBAL_RADIO_MUTE_OUT_REQ_ID))//赋值
		{
			pTda7419Info->FlyAudioInfo.bMuteRadio = FALSE;
		}
		if (IpcWhatEvent(EVENT_GLOBAL_BT_MUTE_IN_REQ_ID))//赋值
		{
			pTda7419Info->FlyAudioInfo.bMuteBT = TRUE;
		}
		if (IpcWhatEvent(EVENT_GLOBAL_BT_MUTE_OUT_REQ_ID))//赋值
		{
			pTda7419Info->FlyAudioInfo.bMuteBT = FALSE;
		}

		if (pTda7419Info->FlyAudioInfo.bMuteRadio && pTda7419Info->FlyAudioInfo.curMainAudioInput == RADIO)//收音机静音
		{
			pTda7419Info->FlyAudioInfo.tmpMainMute = TRUE;
		}
		else if (pTda7419Info->FlyAudioInfo.bMuteBT && 
			(pTda7419Info->FlyAudioInfo.curMainAudioInput == BT || pTda7419Info->FlyAudioInfo.curMainAudioInput == A2DP))//蓝牙静音
		{
			pTda7419Info->FlyAudioInfo.tmpMainMute = TRUE;
		}
		else
		{
			pTda7419Info->FlyAudioInfo.tmpMainMute = pTda7419Info->FlyAudioInfo.preMainMute;
		}

		pTda7419Info->FlyAudioInfo.dspMainMute = pTda7419Info->FlyAudioInfo.tmpMainMute;

		if(pTda7419Info->FlyAudioInfo.curMainMute != pTda7419Info->FlyAudioInfo.tmpMainMute)//直接静音控制
		{
			if (FALSE == bDelayOn || (bDelayEnough && 3 == iDelayStage))
			{
				DBGD((TEXT("\r\nTda7419 DelayThread MuteOff?%d"),pTda7419Info->FlyAudioInfo.tmpMainMute));
				bMute = FALSE;
				pTda7419Info->FlyAudioInfo.curMainMute = pTda7419Info->FlyAudioInfo.tmpMainMute;
				if (pTda7419Info->FlyAudioInfo.curMainMute)
				{
					pTda7419Info->FlyAudioInfo.curMainVolume = 0;
					TDA7419_Volume(pTda7419Info,pTda7419Info->FlyAudioInfo.curMainVolume);
				}
				Tda7419_Mute(pTda7419Info,pTda7419Info->FlyAudioInfo.curMainMute);
			}
		}

		//if (pTda7419Info->FlyAudioInfo.cur7386Mute != pTda7419Info->FlyAudioInfo.preMainMute)
		//{
		//	pTda7419Info->FlyAudioInfo.cur7386Mute = pTda7419Info->FlyAudioInfo.preMainMute;
		//	Tda7386_Mute(pTda7419Info,pTda7419Info->FlyAudioInfo.cur7386Mute);
		//}

		if (IpcWhatEvent(EVENT_GLOBAL_RADIO_MUTE_OUT_REQ_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_RADIO_MUTE_OUT_REQ_ID);
		}
		if (IpcWhatEvent(EVENT_GLOBAL_RADIO_MUTE_IN_REQ_ID))//返回
		{
			IpcClearEvent(EVENT_GLOBAL_RADIO_MUTE_IN_REQ_ID);
			Sleep(314);
			IpcStartEvent(EVENT_GLOBAL_RADIO_MUTE_IN_OK_ID);
		}
		if (IpcWhatEvent(EVENT_GLOBAL_BT_MUTE_IN_REQ_ID))//返回
		{
			IpcClearEvent(EVENT_GLOBAL_BT_MUTE_IN_REQ_ID);
			Sleep(314);
			IpcStartEvent(EVENT_GLOBAL_BT_MUTE_IN_OK_ID);
		}
		if (IpcWhatEvent(EVENT_GLOBAL_BT_MUTE_OUT_REQ_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_BT_MUTE_OUT_REQ_ID);
		}

		if(FALSE == pTda7419Info->FlyAudioInfo.curMainMute && 
			pTda7419Info->FlyAudioInfo.curMainVolume != pTda7419Info->FlyAudioInfo.preMainVolume)
		{
			if (FALSE == bDelayOn || (bDelayEnough && 3 == iDelayStage))//静音后的恢复音量
			{
				if (1)//bVolumeFaderInOut(pTda7419Info))
				{
					if (pTda7419Info->FlyAudioInfo.curMainVolume < pTda7419Info->FlyAudioInfo.preMainVolume)
					{
						pTda7419Info->FlyAudioInfo.curMainVolume++;
						bDelayOn = TRUE;iDelayStage = 3;bDelayEnough = FALSE;iDelayTime = 30;iLastTime = GetTickCount();
					}
					else if (pTda7419Info->FlyAudioInfo.curMainVolume > pTda7419Info->FlyAudioInfo.preMainVolume)
					{
						pTda7419Info->FlyAudioInfo.curMainVolume--;
						bDelayOn = TRUE;iDelayStage = 3;bDelayEnough = FALSE;iDelayTime = 30;iLastTime = GetTickCount();
					}
				}
				else
				{
					pTda7419Info->FlyAudioInfo.curMainVolume = pTda7419Info->FlyAudioInfo.preMainVolume;
				}

				if (3 == iDelayStage
					&& pTda7419Info->FlyAudioInfo.curMainVolume == pTda7419Info->FlyAudioInfo.preMainVolume)
				{
					iDelayStage = 4;//最后了
					volumeFaderInOut(pTda7419Info,FALSE);
				}
				else if (FALSE == bDelayOn)
				{
					bDelayOn = TRUE;iDelayStage = 3;bDelayEnough = FALSE;iDelayTime = 30;iLastTime = GetTickCount();
				}

				DBGD((TEXT("\r\nTda7419 DelayThread Change Volume:%d"),pTda7419Info->FlyAudioInfo.curMainVolume));
				TDA7419_Volume(pTda7419Info,pTda7419Info->FlyAudioInfo.curMainVolume);
				if (EXT_TEL == pTda7419Info->FlyAudioInfo.curMainAudioInput
					|| BT == pTda7419Info->FlyAudioInfo.curMainAudioInput
					|| BT_RING == pTda7419Info->FlyAudioInfo.curMainAudioInput
					|| EXT_TEL == pTda7419Info->FlyAudioInfo.tmpMainAudioInput
					|| BT == pTda7419Info->FlyAudioInfo.tmpMainAudioInput
					|| BT_RING == pTda7419Info->FlyAudioInfo.tmpMainAudioInput)
				{
					pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume 
						= pTda7419Info->FlyAudioInfo.curMainVolume; 
				}

				if (pTda7419Info->FlyAudioInfo.curMainVolume == pTda7419Info->FlyAudioInfo.preMainVolume)
				{
					//I2C_Write_Tda7419(pTda7419Info, 0, &TDA7419_Para[0], 8);
					//I2C_Write_Tda7419(pTda7419Info, 8, &TDA7419_Para[8], 9);
				}
			}
		}
		else
		{
			if (bDelayEnough && 3 == iDelayStage)
			{
				iDelayStage = 4;//最后了
			}
		}
		
		if (4 == iDelayStage)
		{
			DBGD((TEXT("\r\nTda7419 DelayThread Finish")));
			bDelayOn = FALSE;iDelayStage = 0;bDelayEnough = FALSE;iDelayTime = 100;iLastTime = GetTickCount();
		}

		if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)
		{
			pTda7419Info->FlyAudioInfo.tmpGPSSpeaker = 0;
		}
		else
		{
			if (BT_RING == pTda7419Info->FlyAudioInfo.curMainAudioInput
				|| BT == pTda7419Info->FlyAudioInfo.curMainAudioInput
				|| EXT_TEL == pTda7419Info->FlyAudioInfo.curMainAudioInput)//NaviMix处理逻辑
			{
				pTda7419Info->FlyAudioInfo.tmpGPSSpeaker = 0;
			}
			else if (pTda7419Info->FlyAudioInfo.preGPSSpeaker && !pTda7419Info->FlyAudioInfo.dspMainMute)
			{
				if(pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp)
				{
					pTda7419Info->FlyAudioInfo.tmpGPSSpeaker =CAR_AMP_IN_VOLUME_CONSTANT*pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax/60;
				}
				else if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable 
					&& pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
				{
					if (pTda7419Info->FlyAudioInfo.curGPSSpeaker > pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBackLowestVolume)
					{
						 pTda7419Info->FlyAudioInfo.tmpGPSSpeaker = pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBackLowestVolume;
					}
				}
				else
				{
					pTda7419Info->FlyAudioInfo.tmpGPSSpeaker = pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
				}
			}
			else
			{
				pTda7419Info->FlyAudioInfo.tmpGPSSpeaker = 0;
			}
		}

		if (pTda7419Info->FlyAudioInfo.curGPSSpeaker != pTda7419Info->FlyAudioInfo.tmpGPSSpeaker
			|| IpcWhatEvent(EVENT_GLOBAL_GPS_VOLUME_LEVEL_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_GPS_VOLUME_LEVEL_ID);
			if (pTda7419Info->FlyAudioInfo.tmpGPSSpeaker > pTda7419Info->FlyAudioInfo.curGPSSpeaker)
			{
				pTda7419Info->FlyAudioInfo.curGPSSpeaker++;
			}
			else if (pTda7419Info->FlyAudioInfo.tmpGPSSpeaker < pTda7419Info->FlyAudioInfo.curGPSSpeaker)
			{
				pTda7419Info->FlyAudioInfo.curGPSSpeaker--;
			}
			
			TDA7419_Navi_Mix(pTda7419Info,pTda7419Info->FlyAudioInfo.curGPSSpeaker,
				pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGPSVolumeLevel);
			Sleep(5);
			SetEvent(pTda7419Info->hDispatchTda7419DelayThreadEvent);
		}

		pTda7419Info->FlyAudioInfo.dspGPSSpeaker = pTda7419Info->FlyAudioInfo.preGPSSpeaker;
		SetEvent(pTda7419Info->hDispatchExtAmpThreadEvent);
	}
	pTda7419Info->Tda7419DelayThreadHandle = NULL;
	DBGD((TEXT("\r\nTda7419 DelayThread Exit")));
	return 0;
}

static DWORD WINAPI Tda7419MainThread(LPVOID pContext)
{
	P_FLY_TDA7419_INFO pTda7419Info = (P_FLY_TDA7419_INFO)pContext;
	ULONG WaitReturn;

	//BYTE Tda7419ID[4];

	DBGD((TEXT("\r\nTda7419 Tda7419MainThread Start")));
	while (!pTda7419Info->bKillDispatchTda7419MainThread)
	{
		WaitReturn = WaitForSingleObject(pTda7419Info->hDispatchTda7419MainThreadEvent, INFINITE);	
		DBGD((TEXT("\r\nTda7419 Tda7419MainThread Run")));

		if (IpcWhatEvent(EVENT_GLOBAL_SLEEP_PROC_AUDIO_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_SLEEP_PROC_AUDIO_ID);
			pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;//待机要静音，静音完再清除这个标志
			pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeAudioSleep
				= pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeAudioSleep;			
		}

		if (IpcWhatEvent(EVENT_GLOBAL_BACK_VIDEO_SIGNED_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_BACK_VIDEO_SIGNED_ID);
			Tda7419_BackVideoSigned(pTda7419Info, pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow);
		}


		if (FALSE == pTda7419Info->bPowerUp)
		{
		}
		else
		{
			if (pTda7419Info->bNeedInit)
			{
				pTda7419Info->bNeedInit = FALSE;

				I2C_Write_Tda7419(pTda7419Info, 0, &TDA7419_Para[0], 8);
				I2C_Write_Tda7419(pTda7419Info, 8, &TDA7419_Para[8], 9);


				pTda7419Info->bPowerUp = TRUE;
				pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable = TRUE;//音频芯片替其它驱动控制静音开关

				returnAudiobInit(pTda7419Info,TRUE);

				SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
				//if (!pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSilencePowerUp)
				//{
				//	IpcStartEvent(EVENT_GLOBAL_INNER_AMP_ON_ID);
				//}
				//Sleep(618);

				pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNeedProcVoltageShakeAudio = 80;//低电压

				DBGD((TEXT("\r\nTda7419 Init OK")));
			}

			if (IpcWhatEvent(EVENT_GLOBAL_BATTERY_RECOVERY_AUDIO_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_BATTERY_RECOVERY_AUDIO_ID);
				if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBatteryVoltageLowAudio)
				{
				}
				else
				{
					I2C_Write_Tda7419(pTda7419Info, 0, &TDA7419_Para[0], 8);
					I2C_Write_Tda7419(pTda7419Info, 8, &TDA7419_Para[8], 9);
				}
				//FA8200LowVoltageRecovery(pTda7419Info);//由LPC做此检测
			}

			if ( pTda7419Info->FlyAudioInfo.iCurRadioChannel != pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPreRadioChannel)
			{
				pTda7419Info->FlyAudioInfo.iCurRadioChannel = pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPreRadioChannel;
				
				if (pTda7419Info->FlyAudioInfo.curMainAudioInput == RADIO)
				{
					Tda7419_Control_Radio_Channel(pTda7419Info,pTda7419Info->FlyAudioInfo.iCurRadioChannel);
				}
			}


			if (IpcWhatEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME))
			{
				IpcClearEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);
				if (pTda7419Info->FlyAudioInfo.preMainMute != pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute)//全局
				{
					pTda7419Info->FlyAudioInfo.preMainMute = pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute;
					DBGD((TEXT("\r\nTda7419 Global Mute:%d"),pTda7419Info->FlyAudioInfo.preMainMute));
				}
				if (pTda7419Info->FlyAudioInfo.preMainVolume != pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume)//全局
				{
					if (BT == pTda7419Info->FlyAudioInfo.curMainAudioInput
						|| BT_RING == pTda7419Info->FlyAudioInfo.curMainAudioInput
						|| BT == pTda7419Info->FlyAudioInfo.tmpMainAudioInput
						|| BT_RING == pTda7419Info->FlyAudioInfo.tmpMainAudioInput)
					{
						pTda7419Info->FlyAudioInfo.preMainVolume = pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume;
					}
					else{
						pTda7419Info->FlyAudioInfo.preMainVolume = pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
					}

					pTda7419Info->FlyAudioInfo.dspMainVolume = pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
					DBGD((TEXT("\r\nTda7419 Global Volume:%d"),pTda7419Info->FlyAudioInfo.preMainVolume));
				}
			}
			
			if (IpcWhatEvent(EVENT_GLOBAL_STANDBY_AUDIO_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_STANDBY_AUDIO_ID);
				if (!pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)
				{
					Sleep(618);
					I2C_Write_Tda7419(pTda7419Info, 0, &TDA7419_Para[0], 8);
					I2C_Write_Tda7419(pTda7419Info, 8, &TDA7419_Para[8], 9);
				}
			}
			if(IpcWhatEvent(EVENT_GLOBAL_CAR_VOL_UP))
			{
				IpcClearEvent(EVENT_GLOBAL_CAR_VOL_UP);
				DealVolumeData(pTda7419Info,1);
			}
			if(IpcWhatEvent(EVENT_GLOBAL_CAR_VOL_DOWN))
			{
				IpcClearEvent(EVENT_GLOBAL_CAR_VOL_DOWN);	
				DealVolumeData(pTda7419Info,2);
			}
			if(IpcWhatEvent(EVENT_GLOBAL_CAR_VOL_MUTE))
			{
				IpcClearEvent(EVENT_GLOBAL_CAR_VOL_MUTE);
				DealVolumeData(pTda7419Info,0);
			}
			if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)
			{
				pTda7419Info->FlyAudioInfo.preMainMute = TRUE;
			}
			else if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBatteryVoltageLowAudio)
			{
				pTda7419Info->FlyAudioInfo.preMainMute = TRUE;
			}
			else
			{
				pTda7419Info->FlyAudioInfo.preMainMute = pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute;
			}

			//倒车降低音量
			if (IpcWhatEvent(EVENT_GLOBAL_BACK_LOW_VOLUME_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_BACK_LOW_VOLUME_ID);
				volumeFaderInOut(pTda7419Info,TRUE);
			}


			else if (BT_RING == pTda7419Info->FlyAudioInfo.curMainAudioInput
				||  BT == pTda7419Info->FlyAudioInfo.curMainAudioInput)
			{
				pTda7419Info->FlyAudioInfo.preMainVolume = pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume;
			}
			if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveFlyAudioExtAMP)
			{
				pTda7419Info->FlyAudioInfo.preMainVolume = pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iFlyAudioAmpVolume;
			}
			else if(pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp)
			{
				pTda7419Info->FlyAudioInfo.preMainVolume =CAR_AMP_IN_VOLUME_CONSTANT*pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax/60;
			}
			else if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable
				&& pTda7419Info->FlyAudioInfo.curMainAudioInput != EXT_TEL
				&& pTda7419Info->FlyAudioInfo.curMainAudioInput != BT 
				&& pTda7419Info->FlyAudioInfo.curMainAudioInput != BT_RING)
			{
				if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
					//&& pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn)
				{
					if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume > pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBackLowestVolume)
					{
						pTda7419Info->FlyAudioInfo.preMainVolume = pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBackLowestVolume;
					}
					else
					{
						pTda7419Info->FlyAudioInfo.preMainVolume = pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
					}				
				}
				else
				{
					pTda7419Info->FlyAudioInfo.preMainVolume = pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
				}
			}
			/*突出导航音量
			else if (pTda7419Info->FlyAudioInfo.preGPSSpeaker
				&& MediaMP3 != pTda7419Info->FlyAudioInfo.curMainAudioInput
				&& IPOD != pTda7419Info->FlyAudioInfo.curMainAudioInput
				&& BT_RING != pTda7419Info->FlyAudioInfo.curMainAudioInput
				&& BT != pTda7419Info->FlyAudioInfo.curMainAudioInput
				&& EXT_TEL != pTda7419Info->FlyAudioInfo.curMainAudioInput)
			{
				if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume > 3)
				{
					pTda7419Info->FlyAudioInfo.preMainVolume = pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume - 3;
				}
				else
				{
					pTda7419Info->FlyAudioInfo.preMainVolume = pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
				}				
			}
			*/
			else
			{
				pTda7419Info->FlyAudioInfo.preMainVolume = pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
			}
			
			if (IpcWhatEvent(EVENT_GLOBAL_BTCALLSTATUS_CHANGE_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_BTCALLSTATUS_CHANGE_ID);
			}
			if (IpcWhatEvent(EVENT_GLOBAL_PHONECALLSTATUS_CHANGE_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_PHONECALLSTATUS_CHANGE_ID);
			}
			if (IpcWhatEvent(EVENT_GLOBAL__CARAMP_CHANGE_ID))
			{
				IpcClearEvent(EVENT_GLOBAL__CARAMP_CHANGE_ID);
				pTda7419Info->bHaveGetCarInfo =1;
			}

			if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iExtTelCallStatus)//外部电话
			{
				pTda7419Info->FlyAudioInfo.tmpMainAudioInput = EXT_TEL;
			}
			else if (0x03 == pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus)//通话中
			{
				pTda7419Info->FlyAudioInfo.tmpMainAudioInput = BT;
			}
			else if (0x02 == pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus)//去电中
			{
				pTda7419Info->FlyAudioInfo.tmpMainAudioInput = BT;
			}
			else if (0x01 == pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus)//来电中
			{
				pTda7419Info->FlyAudioInfo.tmpMainAudioInput = BT_RING;
			}
			else//平时正常
			{
				pTda7419Info->FlyAudioInfo.tmpMainAudioInput = pTda7419Info->FlyAudioInfo.preMainAudioInput;
			}

			if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveFlyAudioExtAMP)
			{
				pTda7419Info->FlyAudioInfo.tmpBassFreq = 0;
				pTda7419Info->FlyAudioInfo.tmpBassLevel = BASS_LEVEL_COUNT/2;
				pTda7419Info->FlyAudioInfo.tmpMidFreq = 0;
				pTda7419Info->FlyAudioInfo.tmpMidLevel = MID_LEVEL_COUNT/2;
				pTda7419Info->FlyAudioInfo.tmpTrebleFreq = 0;
				pTda7419Info->FlyAudioInfo.tmpTrebleLevel = TREB_LEVEL_COUNT/2;
				//pTda7419Info->FlyAudioInfo.tmpLoudnessOn = FALSE;
				pTda7419Info->FlyAudioInfo.tmpLoudFreq = 0;
				pTda7419Info->FlyAudioInfo.tmpLoudLevel = LOUDNESS_LEVEL_COUNT/2;
				pTda7419Info->FlyAudioInfo.tmpBalance = BALANCE_LEVEL_COUNT/2;
				pTda7419Info->FlyAudioInfo.tmpFader = FADER_LEVEL_COUNT/2;
				//pTda7419Info->FlyAudioInfo.tmpSubOn = FALSE;
				pTda7419Info->FlyAudioInfo.tmpSubFilter = 0;
				pTda7419Info->FlyAudioInfo.tmpSubLevel = 0;
			}
			else if(pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp)
			{
				pTda7419Info->FlyAudioInfo.tmpBassFreq = 0;
				pTda7419Info->FlyAudioInfo.tmpBassLevel = BASS_LEVEL_COUNT/2;
				pTda7419Info->FlyAudioInfo.tmpMidFreq = 0;
				pTda7419Info->FlyAudioInfo.tmpMidLevel = MID_LEVEL_COUNT/2;
				pTda7419Info->FlyAudioInfo.tmpTrebleFreq = 0;
				pTda7419Info->FlyAudioInfo.tmpTrebleLevel = TREB_LEVEL_COUNT/2;
				//pTda7419Info->FlyAudioInfo.tmpLoudnessOn = FALSE;
				pTda7419Info->FlyAudioInfo.tmpLoudFreq = 0;
				pTda7419Info->FlyAudioInfo.tmpLoudLevel = LOUDNESS_LEVEL_COUNT/2;
				pTda7419Info->FlyAudioInfo.tmpBalance = BALANCE_LEVEL_COUNT/2;
				pTda7419Info->FlyAudioInfo.tmpFader = FADER_LEVEL_COUNT/2;
				//pTda7419Info->FlyAudioInfo.tmpSubOn = FALSE;
				pTda7419Info->FlyAudioInfo.tmpSubFilter = 0;
				pTda7419Info->FlyAudioInfo.tmpSubLevel = 0;

			}
			else if (pTda7419Info->FlyAudioInfo.tmpMainAudioInput == EXT_TEL
				|| pTda7419Info->FlyAudioInfo.tmpMainAudioInput == BT 
				|| pTda7419Info->FlyAudioInfo.tmpMainAudioInput == BT_RING)
			{
				pTda7419Info->FlyAudioInfo.tmpBassFreq = 0;
				pTda7419Info->FlyAudioInfo.tmpBassLevel = BASS_LEVEL_COUNT/2;
				pTda7419Info->FlyAudioInfo.tmpMidFreq = 0;
				pTda7419Info->FlyAudioInfo.tmpMidLevel = MID_LEVEL_COUNT/2;
				pTda7419Info->FlyAudioInfo.tmpTrebleFreq = 0;
				pTda7419Info->FlyAudioInfo.tmpTrebleLevel = TREB_LEVEL_COUNT/2;
				//pTda7419Info->FlyAudioInfo.tmpLoudnessOn = FALSE;
				pTda7419Info->FlyAudioInfo.tmpLoudFreq = 0;
				pTda7419Info->FlyAudioInfo.tmpLoudLevel = LOUDNESS_LEVEL_COUNT/2;
				pTda7419Info->FlyAudioInfo.tmpBalance = BALANCE_LEVEL_COUNT/2;
				pTda7419Info->FlyAudioInfo.tmpFader = FADER_LEVEL_COUNT/2;
				//pTda7419Info->FlyAudioInfo.tmpSubOn = FALSE;
				pTda7419Info->FlyAudioInfo.tmpSubFilter = 0;
				pTda7419Info->FlyAudioInfo.tmpSubLevel = 0;		
			}
			else
			{
				pTda7419Info->FlyAudioInfo.tmpBassFreq = pTda7419Info->FlyAudioInfo.preBassFreq;
				pTda7419Info->FlyAudioInfo.tmpBassLevel = pTda7419Info->FlyAudioInfo.preBassLevel;
				pTda7419Info->FlyAudioInfo.tmpMidFreq = pTda7419Info->FlyAudioInfo.preMidFreq;
				pTda7419Info->FlyAudioInfo.tmpMidLevel = pTda7419Info->FlyAudioInfo.preMidLevel;
				pTda7419Info->FlyAudioInfo.tmpTrebleFreq = pTda7419Info->FlyAudioInfo.preTrebleFreq;
				pTda7419Info->FlyAudioInfo.tmpTrebleLevel = pTda7419Info->FlyAudioInfo.preTrebleLevel;
				pTda7419Info->FlyAudioInfo.tmpLoudnessOn = pTda7419Info->FlyAudioInfo.preLoudnessOn;
				pTda7419Info->FlyAudioInfo.tmpLoudFreq = pTda7419Info->FlyAudioInfo.preLoudFreq;
				pTda7419Info->FlyAudioInfo.tmpLoudLevel = pTda7419Info->FlyAudioInfo.preLoudLevel;
				pTda7419Info->FlyAudioInfo.tmpBalance = pTda7419Info->FlyAudioInfo.preBalance;
				pTda7419Info->FlyAudioInfo.tmpFader = pTda7419Info->FlyAudioInfo.preFader;
				pTda7419Info->FlyAudioInfo.tmpSubOn = pTda7419Info->FlyAudioInfo.preSubOn;
				pTda7419Info->FlyAudioInfo.tmpSubFilter = pTda7419Info->FlyAudioInfo.preSubFilter;
				pTda7419Info->FlyAudioInfo.tmpSubLevel = pTda7419Info->FlyAudioInfo.preSubLevel;
			}

			pTda7419Info->FlyAudioInfo.dspBassFreq = pTda7419Info->FlyAudioInfo.preBassFreq;
			pTda7419Info->FlyAudioInfo.dspBassLevel = pTda7419Info->FlyAudioInfo.preBassLevel;
			pTda7419Info->FlyAudioInfo.dspMidFreq = pTda7419Info->FlyAudioInfo.preMidFreq;
			pTda7419Info->FlyAudioInfo.dspMidLevel = pTda7419Info->FlyAudioInfo.preMidLevel;
			pTda7419Info->FlyAudioInfo.dspTrebleFreq = pTda7419Info->FlyAudioInfo.preTrebleFreq;
			pTda7419Info->FlyAudioInfo.dspTrebleLevel = pTda7419Info->FlyAudioInfo.preTrebleLevel;
			pTda7419Info->FlyAudioInfo.dspLoudnessOn = pTda7419Info->FlyAudioInfo.preLoudnessOn;
			pTda7419Info->FlyAudioInfo.dspLoudFreq = pTda7419Info->FlyAudioInfo.preLoudFreq;
			pTda7419Info->FlyAudioInfo.dspLoudLevel = pTda7419Info->FlyAudioInfo.preLoudLevel;
			pTda7419Info->FlyAudioInfo.dspBalance = pTda7419Info->FlyAudioInfo.preBalance;
			pTda7419Info->FlyAudioInfo.dspFader = pTda7419Info->FlyAudioInfo.preFader;
			pTda7419Info->FlyAudioInfo.dspSubOn = pTda7419Info->FlyAudioInfo.preSubOn;
			pTda7419Info->FlyAudioInfo.dspSubFilter = pTda7419Info->FlyAudioInfo.preSubFilter;
			pTda7419Info->FlyAudioInfo.dspSubLevel = pTda7419Info->FlyAudioInfo.preSubLevel;

			/*******************************************************/
			//SetEvent(pTda7419Info->hDispatchExtAmpThreadEvent);//这个任务在延时任务
			/*******************************************************/

			SetEvent(pTda7419Info->hDispatchTda7419DelayThreadEvent);//启动延时任务，控制切换等时序

			DBGD((TEXT("\r\nTda7419 Main Thread Running")));

			if (pTda7419Info->FlyAudioInfo.curSimEQ != pTda7419Info->FlyAudioInfo.preSimEQ)
			{
				pTda7419Info->FlyAudioInfo.curSimEQ = pTda7419Info->FlyAudioInfo.preSimEQ;
				returnAudioMainSimEQ(pTda7419Info,pTda7419Info->FlyAudioInfo.curSimEQ);
			}

			if(pTda7419Info->FlyAudioInfo.curBassFreq != pTda7419Info->FlyAudioInfo.tmpBassFreq 
				|| pTda7419Info->FlyAudioInfo.curBassLevel != pTda7419Info->FlyAudioInfo.tmpBassLevel)
			{				
				pTda7419Info->FlyAudioInfo.curBassFreq = pTda7419Info->FlyAudioInfo.tmpBassFreq;
				pTda7419Info->FlyAudioInfo.curBassLevel = pTda7419Info->FlyAudioInfo.tmpBassLevel;
				Tda7419_TreMidBass_ForRadio(pTda7419Info);
			}
			if(pTda7419Info->FlyAudioInfo.curMidFreq != pTda7419Info->FlyAudioInfo.tmpMidFreq || pTda7419Info->FlyAudioInfo.curMidLevel != pTda7419Info->FlyAudioInfo.tmpMidLevel)
			{
				pTda7419Info->FlyAudioInfo.curMidFreq = pTda7419Info->FlyAudioInfo.tmpMidFreq;
				pTda7419Info->FlyAudioInfo.curMidLevel = pTda7419Info->FlyAudioInfo.tmpMidLevel;
				Tda7419_TreMidBass_ForRadio(pTda7419Info);
			}
			if(pTda7419Info->FlyAudioInfo.curTrebleFreq != pTda7419Info->FlyAudioInfo.tmpTrebleFreq || pTda7419Info->FlyAudioInfo.curTrebleLevel != pTda7419Info->FlyAudioInfo.tmpTrebleLevel)
			{
				pTda7419Info->FlyAudioInfo.curTrebleFreq = pTda7419Info->FlyAudioInfo.tmpTrebleFreq;
				pTda7419Info->FlyAudioInfo.curTrebleLevel = pTda7419Info->FlyAudioInfo.tmpTrebleLevel;
				Tda7419_TreMidBass_ForRadio(pTda7419Info);
			}
			if (pTda7419Info->FlyAudioInfo.curLoudnessOn != pTda7419Info->FlyAudioInfo.tmpLoudnessOn
				/*|| pTda7419Info->FlyAudioInfo.curLoudFreq != pTda7419Info->FlyAudioInfo.tmpLoudFreq 
				|| pTda7419Info->FlyAudioInfo.curLoudLevel != pTda7419Info->FlyAudioInfo.tmpLoudLevel*/)
			{
				pTda7419Info->FlyAudioInfo.curLoudnessOn = pTda7419Info->FlyAudioInfo.tmpLoudnessOn;
				//pTda7419Info->FlyAudioInfo.curLoudFreq = pTda7419Info->FlyAudioInfo.tmpLoudFreq;
				//pTda7419Info->FlyAudioInfo.curLoudLevel = pTda7419Info->FlyAudioInfo.tmpLoudLevel;

				TDA7419_Loud(pTda7419Info, pTda7419Info->FlyAudioInfo.curLoudFreq, pTda7419Info->FlyAudioInfo.curLoudnessOn);

				//if (pTda7419Info->FlyAudioInfo.curLoudnessOn)
				//{
				//	TDA7419_Loud(pTda7419Info, pTda7419Info->FlyAudioInfo.curLoudFreq, pTda7419Info->FlyAudioInfo.curLoudLevel);
				//} 
				//else
				//{
				//	TDA7419_Loud(pTda7419Info,  0, LOUDNESS_LEVEL_COUNT/2);
				//}
			}

			if((pTda7419Info->FlyAudioInfo.curBalance != pTda7419Info->FlyAudioInfo.tmpBalance) 
				|| (pTda7419Info->FlyAudioInfo.curFader != pTda7419Info->FlyAudioInfo.tmpFader))
			{
				pTda7419Info->FlyAudioInfo.curBalance = pTda7419Info->FlyAudioInfo.tmpBalance;
				pTda7419Info->FlyAudioInfo.curFader = pTda7419Info->FlyAudioInfo.tmpFader;
				TDA7419_Balance_Fader(pTda7419Info,pTda7419Info->FlyAudioInfo.curBalance,
					pTda7419Info->FlyAudioInfo.curFader);
				
			}

			if (pTda7419Info->FlyAudioInfo.curSubOn != pTda7419Info->FlyAudioInfo.tmpSubOn
				/*|| pTda7419Info->FlyAudioInfo.curSubFilter != pTda7419Info->FlyAudioInfo.tmpSubFilter
				|| pTda7419Info->FlyAudioInfo.curSubLevel != pTda7419Info->FlyAudioInfo.tmpSubLevel*/)
			{
				pTda7419Info->FlyAudioInfo.curSubOn = pTda7419Info->FlyAudioInfo.tmpSubOn;
	/*			pTda7419Info->FlyAudioInfo.curSubFilter = pTda7419Info->FlyAudioInfo.tmpSubFilter;
				pTda7419Info->FlyAudioInfo.curSubLevel = pTda7419Info->FlyAudioInfo.tmpSubLevel;*/
				Tda7419_Sub(pTda7419Info,pTda7419Info->FlyAudioInfo.curSubFilter,pTda7419Info->FlyAudioInfo.curSubOn);

				//if (pTda7419Info->FlyAudioInfo.curSubOn)
				//{
				//	Tda7419_Sub(pTda7419Info,pTda7419Info->FlyAudioInfo.curSubFilter,pTda7419Info->FlyAudioInfo.tmpSubOn);
				//} 
				//else
				//{
				//	Tda7419_Sub(pTda7419Info,0,0);
				//}
			}

			if(pTda7419Info->FlyAudioInfo.curReadReg != pTda7419Info->FlyAudioInfo.preReadReg || pTda7419Info->FlyAudioInfo.ReadRegFlag == TRUE)
			{
				pTda7419Info->FlyAudioInfo.ReadRegFlag=FALSE;
				pTda7419Info->FlyAudioInfo.curReadReg = pTda7419Info->FlyAudioInfo.preReadReg;
				Tda7419_ReadReg(pTda7419Info,pTda7419Info->FlyAudioInfo.curReadReg);
			}

			//Tda7419_ReadGraphicalSpectrumAnalyzer(pTda7419Info);
		}
		//DBGD((TEXT("#Tda7419#")));


		if (!pTda7419Info->bSocketConnecting)
		{
			DBGE((TEXT("\r\n7419 socket can't recv msg(ensure the socket is connect and send init commd)\r\n")));
		}
	}
	pTda7419Info->bPowerUp = FALSE;
	pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable = FALSE;//音频芯片替其它驱动控制静音开关

	while (pTda7419Info->Tda7419DelayThreadHandle)
	{
		SetEvent(pTda7419Info->hDispatchTda7419DelayThreadEvent);
		Sleep(10);
	}
	CloseHandle(pTda7419Info->hDispatchTda7419DelayThreadEvent);

	//if(pTda7419Info->Tda7419I2CHandle)
	//{
	//	I2CClose(pTda7419Info->Tda7419I2CHandle);
	//	pTda7419Info->Tda7419I2CHandle = NULL;
	//}

	pTda7419Info->Tda7419MainThreadHandle = NULL;

	DBGD((TEXT("\r\nTda7419 Main Thread Exit)")));
	return 0;
}

void Fly7419AudioParaInitDefault(P_FLY_TDA7419_INFO pTda7419Info)
{
	memset(&pTda7419Info->FlyAudioInfo,0xFF,sizeof(FLY_AUDIO_INFO));

	pTda7419Info->FlyAudioInfo.bMuteRadio = FALSE;
	pTda7419Info->FlyAudioInfo.bMuteBT = FALSE;

	pTda7419Info->FlyAudioInfo.preMainAudioInput = MediaCD;
	pTda7419Info->FlyAudioInfo.curMainAudioInput = Init;

	pTda7419Info->FlyAudioInfo.preMainVolume = 0;//涉及到淡入淡出，全部归零
	pTda7419Info->FlyAudioInfo.curMainVolume = 0;

	pTda7419Info->FlyAudioInfo.preMainMute = TRUE;
	pTda7419Info->FlyAudioInfo.cur7386Mute = FALSE;
	pTda7419Info->FlyAudioInfo.usrBassFreq = pTda7419Info->FlyAudioInfo.preBassFreq = 0;
	pTda7419Info->FlyAudioInfo.usrBassLevel = pTda7419Info->FlyAudioInfo.preBassLevel = BASS_LEVEL_COUNT/2;
	pTda7419Info->FlyAudioInfo.usrMidFreq = pTda7419Info->FlyAudioInfo.preMidFreq = 0;
	pTda7419Info->FlyAudioInfo.usrMidLevel = pTda7419Info->FlyAudioInfo.preMidLevel = MID_LEVEL_COUNT/2;
	pTda7419Info->FlyAudioInfo.usrTrebleFreq = pTda7419Info->FlyAudioInfo.preTrebleFreq = 0;
	pTda7419Info->FlyAudioInfo.usrTrebleLevel = pTda7419Info->FlyAudioInfo.preTrebleLevel = TREB_LEVEL_COUNT/2;
	pTda7419Info->FlyAudioInfo.preLoudFreq = 0;
	pTda7419Info->FlyAudioInfo.preLoudLevel = LOUDNESS_LEVEL_COUNT/2;
	pTda7419Info->FlyAudioInfo.preBalance = BALANCE_LEVEL_COUNT/2;
	pTda7419Info->FlyAudioInfo.preFader = FADER_LEVEL_COUNT/2;
	pTda7419Info->FlyAudioInfo.preLoudnessOn = FALSE;
	pTda7419Info->FlyAudioInfo.preSubOn = FALSE;
	pTda7419Info->FlyAudioInfo.preSubFilter = 0;
	pTda7419Info->FlyAudioInfo.preSubLevel = 5;

	pTda7419Info->FlyAudioInfo.preSimEQ = 0;

	pTda7419Info->FlyAudioInfo.preGPSSpeaker = 0;
	pTda7419Info->FlyAudioInfo.curGPSSpeaker = 0;

	pTda7419Info->FlyAudioInfo.bEnableVolumeFader = FALSE;
}

static void powerNormalInit(P_FLY_TDA7419_INFO pTda7419Info)
{
	pTda7419Info->bPowerUp = FALSE;
	pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable = FALSE;//音频芯片替其它驱动控制静音开关

	pTda7419Info->bNeedInit = FALSE;

	pTda7419Info->buffToUserHx = 0;
	pTda7419Info->buffToUserLx = 0;

	memcpy(TDA7419_Para, TDA7741_Init_Data, 18);
	
	(pTda7419Info);
	Fly7419AudioParaInitDefault(pTda7419Info);
}

static void powerOnFirstInit(P_FLY_TDA7419_INFO pTda7419Info)
{
	pTda7419Info->bOpen = FALSE;
	pTda7419Info->bPowerSpecialUp = FALSE;

	pTda7419Info->bKillDispatchTda7419MainThread = TRUE;
	pTda7419Info->Tda7419MainThreadHandle = NULL;
	pTda7419Info->hDispatchTda7419MainThreadEvent = NULL;

	/****************INIT FLYAUDIO AMPLIFIER******************/
	pTda7419Info->bKillDispatchFlyMsgQueueExtAmpReadThread = TRUE;
	pTda7419Info->hMsgQueueToExtAmpCreate = NULL;
	pTda7419Info->hMsgQueueFromExtAmpCreate= NULL;
	pTda7419Info->FLyMsgQueueExtAmpReadThreadHandle= NULL;

	pTda7419Info->bKillDispatchExtAmpThread = TRUE;
	pTda7419Info->ExtAmpThreadHandle = NULL;
	pTda7419Info->hDispatchExtAmpThreadEvent =NULL;
	/********************************************************/

	/****************INIT CAR AMPLIFIER******************/
	pTda7419Info->bKillDispatchFlyMsgQueueCarAmpReadThread = TRUE;
	pTda7419Info->hMsgQueueToCarAmpCreate = NULL;
	pTda7419Info->hMsgQueueFromCarAmpCreate= NULL;
	pTda7419Info->FLyMsgQueueCarAmpReadThreadHandle= NULL;
	/********************************************************/

	pTda7419Info->bKillDispatchTda7419DelayThread = TRUE;
	pTda7419Info->Tda7419DelayThreadHandle = NULL;
	pTda7419Info->hDispatchTda7419DelayThreadEvent = NULL;

	pTda7419Info->Tda7419I2CHandle = NULL;

}

static void powerOnSpecialEnable(P_FLY_TDA7419_INFO pTda7419Info,BOOL bOn)
{
	DWORD dwThreadID;

	if (bOn)
	{
		if (pTda7419Info->bPowerSpecialUp)
		{
			return;
		}
		pTda7419Info->bPowerSpecialUp = TRUE;
		
		pTda7419Info->hBuffToUserDataEvent = CreateEvent(NULL,FALSE,FALSE,NULL);	
		InitializeCriticalSection(&pTda7419Info->hCSSendToUser);

		/***********************Creat MSGS of  FLYAUDIO EXT AMPLIFIER***********************************/
		MSGQUEUEOPTIONS  msgOpts;

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_EXTAMP_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_EXTAMP_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pTda7419Info->hMsgQueueToExtAmpCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_EXTAMP_NAME, &msgOpts);
		if (NULL == pTda7419Info->hMsgQueueToExtAmpCreate)
		{
			DBGE((TEXT("\r\nFlyAudio TDA7419 Create MsgQueue To ExtAmp Fail!")));
		}

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_FROM_EXTAMP_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_FROM_EXTAMP_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pTda7419Info->hMsgQueueFromExtAmpCreate = CreateMsgQueue(FLY_MSG_QUEUE_FROM_EXTAMP_NAME, &msgOpts);
		if (NULL == pTda7419Info->hMsgQueueFromExtAmpCreate)
		{
			DBGE((TEXT("\r\nFlyAudio ExtAmp Create MsgQueue From ExtAmp Fail!")));
		}

		pTda7419Info->bKillDispatchFlyMsgQueueExtAmpReadThread = FALSE;
		pTda7419Info->FLyMsgQueueExtAmpReadThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadExtAmpRead, //线程的全局函数
			pTda7419Info, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\n7419 ThreadExtAmpRead thread!%x"),dwThreadID));
		if (NULL == pTda7419Info->FLyMsgQueueExtAmpReadThreadHandle)
		{
			pTda7419Info->bKillDispatchFlyMsgQueueExtAmpReadThread = TRUE;
			return;
		}
		/********************************************************************************************/

		/***********************Creat MSGS of  CAR AMPLIFIER***********************************/

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_CARAMP_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_CARAMP_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pTda7419Info->hMsgQueueToCarAmpCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_CARAMP_NAME, &msgOpts);
		if (NULL == pTda7419Info->hMsgQueueToCarAmpCreate)
		{
			DBGE((TEXT("\r\nFlyAudio TDA7419 Create MsgQueue To CarAmp Fail!")));
		}

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_FROM_CARAMP_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_FROM_CARAMP_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pTda7419Info->hMsgQueueFromCarAmpCreate = CreateMsgQueue(FLY_MSG_QUEUE_FROM_CARAMP_NAME, &msgOpts);
		if (NULL == pTda7419Info->hMsgQueueFromCarAmpCreate)
		{
			DBGE((TEXT("\r\nFlyAudio CarAmp Create MsgQueue From CarAmp Fail!")));
		}

		pTda7419Info->bKillDispatchFlyMsgQueueCarAmpReadThread = FALSE;
		pTda7419Info->FLyMsgQueueCarAmpReadThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadCarAmpRead, //线程的全局函数
			pTda7419Info, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\n7419 ThreadCarAmpRead thread!%x"),dwThreadID));
		if (NULL == pTda7419Info->FLyMsgQueueCarAmpReadThreadHandle)
		{
			pTda7419Info->bKillDispatchFlyMsgQueueCarAmpReadThread = TRUE;
			return;
		}
		/********************************************************************************************/
	} 
	else
	{
		if (!pTda7419Info->bPowerSpecialUp)
		{
			return;
		}
		pTda7419Info->bPowerSpecialUp = FALSE;

		/*******Close MSGS of Flyaudio amplifier***********************/
		pTda7419Info->bKillDispatchFlyMsgQueueExtAmpReadThread = TRUE;
		while (pTda7419Info->FLyMsgQueueExtAmpReadThreadHandle)
		{
			SetEvent(pTda7419Info->hMsgQueueToExtAmpCreate);
			Sleep(10);
		}
		CloseMsgQueue(pTda7419Info->hMsgQueueToExtAmpCreate);
		CloseMsgQueue(pTda7419Info->hMsgQueueFromExtAmpCreate);
		/****************************************************************/

		/*******Close MSGS of CAR amplifier***********************/
		pTda7419Info->bKillDispatchFlyMsgQueueCarAmpReadThread = TRUE;
		while (pTda7419Info->FLyMsgQueueCarAmpReadThreadHandle)
		{
			SetEvent(pTda7419Info->hMsgQueueToCarAmpCreate);
			Sleep(10);
		}
		CloseMsgQueue(pTda7419Info->hMsgQueueToCarAmpCreate);
		CloseMsgQueue(pTda7419Info->hMsgQueueFromCarAmpCreate);
		/****************************************************************/

		CloseHandle(pTda7419Info->hBuffToUserDataEvent);
	}
}

VOID Fly7419Enable(P_FLY_TDA7419_INFO pTda7419Info,BOOL bOn)
{
	DWORD dwThreadID;

	if(bOn)
	{
		pTda7419Info->bKillDispatchTda7419MainThread = FALSE;
		pTda7419Info->hDispatchTda7419MainThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		pTda7419Info->Tda7419MainThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)Tda7419MainThread, //线程的全局函数
			pTda7419Info, //此处传入了主框架的句柄
			0, &dwThreadID );
		if (NULL == pTda7419Info->Tda7419MainThreadHandle)
		{
			pTda7419Info->bKillDispatchTda7419MainThread = TRUE;
			return;
		}
		DBGE((TEXT("\r\n7419 Tda7419MainThread thread!%x"),dwThreadID));

		pTda7419Info->bKillDispatchTda7419DelayThread = FALSE;
		pTda7419Info->hDispatchTda7419DelayThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		pTda7419Info->Tda7419DelayThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)Tda7419DelayThread, //线程的全局函数
			pTda7419Info, //此处传入了主框架的句柄
			0, &dwThreadID );
		if (NULL == pTda7419Info->Tda7419DelayThreadHandle)
		{
			pTda7419Info->bKillDispatchTda7419DelayThread = TRUE;
			return;
		}
		DBGE((TEXT("\r\n7419 Tda7419DelayThread thread!%x"),dwThreadID));

		/**************Create and enable Flyaudio Amplifier process ********************************/
		pTda7419Info->bKillDispatchExtAmpThread = FALSE;
		pTda7419Info->hDispatchExtAmpThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		pTda7419Info->ExtAmpThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ExtAmpProcessThread, //线程的全局函数
			pTda7419Info, //此处传入了主框架的句柄
			0, &dwThreadID );
		if (NULL == pTda7419Info->ExtAmpThreadHandle)
		{
			pTda7419Info->bKillDispatchExtAmpThread = TRUE;
			return;
		}
		DBGE((TEXT("\r\n7419 ExtAmpProcessThread thread!%x"),dwThreadID));
			/*************************************************************************************/

	}
	else
	{
		pTda7419Info->bKillDispatchTda7419MainThread = TRUE;
		SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);

		pTda7419Info->bKillDispatchTda7419DelayThread = TRUE;
		SetEvent(pTda7419Info->hDispatchTda7419DelayThreadEvent);

		/*************Disable Flyaudio Amplifier process thread*********/
		pTda7419Info->bKillDispatchExtAmpThread = TRUE;
		SetEvent(pTda7419Info->hDispatchExtAmpThreadEvent);
		/***************************************************************/

		while (pTda7419Info->Tda7419MainThreadHandle)
		{
			SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
			Sleep(10);
		}
		CloseHandle(pTda7419Info->hDispatchTda7419MainThreadEvent);
	}
}

void IpcRecv(UINT32 iEvent)
{
	P_FLY_TDA7419_INFO	pTda7419Info = (P_FLY_TDA7419_INFO)gpTda7419Info;
	
	DBGD((TEXT("\r\nFLY7419 Recv IPC iEvent:%d\r\n"),iEvent));

	if (iEvent == EVENT_GLOBAL_DATA_CHANGE_VOLUME)
	{
		TDA7419_SendVolumeToUser(pTda7419Info);
	}

	SetEvent(pTda7419Info->hDispatchTda7419MainThreadEvent);
}
void SockRecv(BYTE *buf, UINT16 len)
{
	UINT16 i=0;
	P_FLY_TDA7419_INFO	pTda7419Info = (P_FLY_TDA7419_INFO)gpTda7419Info;

	DBGD((TEXT("\r\n FLY 7419 socket recv %d bytes: "),len-1));
	for (i=0; i<len-1; i++)
		DBGD((TEXT("%02X "), buf[i]));

	switch (buf[0])
	{
	case 0x18://当前通道
		//returnAudioMainInput(pTda7419Info,buf[1]);
		break;
	
	default:
		break;
	}

	if (!pTda7419Info->bSocketConnecting)
		pTda7419Info->bSocketConnecting  = TRUE;
}

HANDLE FAU_Init(DWORD dwContext)
{
	P_FLY_TDA7419_INFO pTda7419Info;

	RETAILMSG(1, (TEXT("\r\n7419 Init Start")));

	pTda7419Info = (P_FLY_TDA7419_INFO)LocalAlloc(LPTR, sizeof(FLY_TDA7419_INFO));
	if (!pTda7419Info)
	{
		return NULL;
	}
	gpTda7419Info = pTda7419Info;

	pTda7419Info->pFlyShmGlobalInfo = CreateShm(AUDIO_MODULE,IpcRecv);
	if (NULL == pTda7419Info->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio 7419 Init create shm err\r\n")));
		return NULL;
	}

	if (!GetDllAddrTable())
	{
		DBGE((TEXT("FlyAudio 7419  GetDllAddrTable err\r\n")));
		return NULL;
	}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio 7419 Open create user buf err\r\n")));
		return NULL;
	}
	if(!CreateServerSocket(SockRecv, TCP_PORT_AUDIO))
	{
		DBGE((TEXT("FlyAudio 7419 Open create server socket err\r\n")));
		return NULL;
	}

	if (pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalStructSize != sizeof(struct shm))
	{
		pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[0] = 'F';
		pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[1] = 'A';
		pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[2] = 'U';
	}

	//参数初始化
	powerNormalInit(pTda7419Info);
	powerOnFirstInit(pTda7419Info);
	powerOnSpecialEnable(pTda7419Info,TRUE);

	control7386Mute(pTda7419Info,TRUE);

#if GLOBAL_COMM
	//pTda7419Info->hHandleGlobalGlobalEvent = CreateEvent(NULL,FALSE,FALSE,DATA_GLOBAL_HANDLE_GLOBAL);
	pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAudioInfo.bInit = TRUE;
	pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAudioInfo.iDriverCompYear = year;
	pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAudioInfo.iDriverCompMon = months;
	pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAudioInfo.iDriverCompDay = day;
	pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAudioInfo.iDriverCompHour = hours;
	pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAudioInfo.iDriverCompMin = minutes;
	pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAudioInfo.iDriverCompSec = seconds;
#endif

	RETAILMSG(1, (TEXT("\r\nTda7419 Init Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return (HANDLE)pTda7419Info;
}

BOOL FAU_Deinit(DWORD hDeviceContext)
{
	P_FLY_TDA7419_INFO	pTda7419Info = (P_FLY_TDA7419_INFO)hDeviceContext;
	RETAILMSG(1, (TEXT("\r\nTda7419 DeInit")));
	CloseHandle(pTda7419Info->hBuffToUserDataEvent);

	FreeShm();
	FreeSocketServer();
	FreeUserBuff();

	LocalFree(pTda7419Info);
	return TRUE;
}

DWORD FAU_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	P_FLY_TDA7419_INFO	pTda7419Info = (P_FLY_TDA7419_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;

	RETAILMSG(1, (TEXT("\r\nTda7419 Open Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));

	if(pTda7419Info->bOpen)
	{
		returnWhat = NULL;
	}
	else
	{
		pTda7419Info->bOpen = TRUE;
	}

	Fly7419Enable(pTda7419Info,TRUE);

	pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAudioInfo.bOpen = TRUE;

	return returnWhat;
}

BOOL FAU_Close(DWORD hDeviceContext)
{
	P_FLY_TDA7419_INFO	pTda7419Info = (P_FLY_TDA7419_INFO)hDeviceContext;


	pTda7419Info->bOpen = FALSE;
	//pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAudioInfo.bOpen = FALSE;
	Fly7419Enable(pTda7419Info,FALSE);

	SetEvent(pTda7419Info->hBuffToUserDataEvent);
	RETAILMSG(1, (TEXT("\r\nTda7419 Close")));
	return TRUE;
}

VOID FAU_PowerUp(DWORD hDeviceContext)
{
	P_FLY_TDA7419_INFO pTda7419Info = (P_FLY_TDA7419_INFO)hDeviceContext;

	powerNormalInit(pTda7419Info);

	RETAILMSG(1, (TEXT("\r\nTda7419 PowerUp")));
}

VOID FAU_PowerDown(DWORD hDeviceContext)
{
	P_FLY_TDA7419_INFO pTda7419Info = (P_FLY_TDA7419_INFO)hDeviceContext;

	pTda7419Info->bPowerUp = FALSE;
	//pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable = FALSE;//音频芯片替其它驱动控制静音开关

	//pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNeedProcVoltageShakeAudio = 0;

	control7386Mute(pTda7419Info,TRUE);

	//pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeAudioSleep = FALSE;
	//pTda7419Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeAudioSleep = FALSE;

	RETAILMSG(1, (TEXT("\r\nTda7419 PowerDown")));
}

DWORD FAU_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	P_FLY_TDA7419_INFO	pTda7419Info = (P_FLY_TDA7419_INFO)hOpenContext;
	UINT dwRead = 0,i;

	BYTE *buf = (BYTE *)pBuffer;
	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio Audio return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}

	return dwRead;
}

DWORD FAU_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	P_FLY_TDA7419_INFO	pTda7419Info = (P_FLY_TDA7419_INFO)hOpenContext;
	DBGD((TEXT("\r\nTda7419 Write %d "),NumberOfBytes));
	for(UINT i = 0;i < NumberOfBytes;i++)
	{
		DBGD((TEXT(" %X"),*((BYTE *)pSourceBytes + i)));
	}

	if(NumberOfBytes)
	{
		DealWinceCmd(pTda7419Info,(((BYTE *)pSourceBytes)+3),((BYTE *)pSourceBytes)[2] - 1);
	}
	return NULL;
}

DWORD FAU_Seek(DWORD hOpenContext, LONG Amount, DWORD  Type)
{
	return NULL;
}

BOOL FAU_IOControl(DWORD hOpenContext,
				   DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn,
				   PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
	P_FLY_TDA7419_INFO	pTda7419Info = (P_FLY_TDA7419_INFO)hOpenContext;
	BOOL bRes = TRUE;

	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	DBGD((TEXT("\r\nTda7419 IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)"),
	//		hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	DBGD((TEXT("\r\nTda7419 IOControl Set IOCTL_SERIAL_SET_WAIT_MASK")));
	//	break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	DBGD((TEXT("\r\nTda7419 IOControl Set IOCTL_SERIAL_WAIT_ON_MASK Enter")));
	//	if (pTda7419Info->bOpen)
	//	{
	//		WaitForSingleObject(pTda7419Info->hBuffToUserDataEvent,INFINITE);
	//	} 
	//	else
	//	{
	//		WaitForSingleObject(pTda7419Info->hBuffToUserDataEvent,0);
	//	}
	//	if ((dwLenOut < sizeof(DWORD)) || (NULL == pBufOut) ||
	//		(NULL == pdwActualOut))
	//	{
	//		bRes = FALSE;
	//		break;
	//	}
	//	*(DWORD *)pBufOut = EV_RXCHAR;
	//	*pdwActualOut = sizeof(DWORD);
	//	DBGD((TEXT("\r\nTda7419 IOControl Set IOCTL_SERIAL_WAIT_ON_MASK Exit")));
	//	break;
	//default :
	//	break;
	//}

	return TRUE;
}

BOOL DllEntry(
			  HINSTANCE hinstDll,
			  DWORD dwReason,
			  LPVOID lpReserved
			  )
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DEBUGMSG(1, (TEXT("Attach in Fly7419 DllEntry\n")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}

	if (dwReason == DLL_PROCESS_DETACH)
	{
		DEBUGMSG(1, (TEXT("Dettach in Fly7419 DllEntry\n")));
	}

	DEBUGMSG(1, (TEXT("Leaving Fly7419 DllEntry\n")));

	return (TRUE);
}
