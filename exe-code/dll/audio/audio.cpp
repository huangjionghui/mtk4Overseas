// audio.cpp : 定义 DLL 应用程序的入口点。
//

#include "audio.h"
#include "audioapi.h"
#include "common.h"
#include <assert.h>
#include <windows.h>
#include <commctrl.h>
#include "flyshm.h"

struct audio_t gAudioInfo;

static void ControlAudioVolume(UINT16 iVolume)
{
	UINT32 temp = 0;
	setIpodVolume(iVolume);

	if (gAudioInfo.iCurAudioChannel == RADIO)
	{
		if (0x00 == iVolume)
			iVolume = 0;
		else if (gAudioInfo.iCurAudioParma == AM)
			iVolume = iVolume + 6;
		else 
		{
			if (iVolume <= 55)
				iVolume = iVolume + 4;
			else
				iVolume = iVolume + 8;
		}
	}
	else if (gAudioInfo.iCurAudioChannel == AUX)
	{
		if (0x00 == iVolume)
			iVolume = 0;
		else 
			iVolume = iVolume + 8;
	}
	else if (gAudioInfo.iCurAudioChannel == BT)
	{
		temp = BT_VOLUME_TABLE[iVolume];
		setBTVolume(temp);

		temp = BTRING_VOLUME_TABLE[iVolume];
		setBTRingVolume(temp);
	}
	else if (gAudioInfo.iCurAudioChannel == MediaMP3)
	{
		temp = MP3_VOLUME_TABLE[iVolume];
		setMP3Volume(temp);
	}
	else if (gAudioInfo.iCurAudioChannel == MediaCD)
	{
		temp = DVP_VOLUME_TABLE[iVolume];
		setDVPValume(temp);
	}

	temp = VOLUME_TABLE[iVolume];
	setFrontValume(temp);
	setBackValume(temp);
}

static void returnAudioInitStatus(BOOL bInit)
{
	BYTE buf[2] = {0x01,0x00};
	buf[1] = bInit;
	SocketWrite(buf,2);
}

static void returnCurAudioSetting(BYTE *buf, UINT16 len)
{
	SocketWrite(buf,len);
}
static void ControlAudioSourceChannel(BYTE iChannel)
{
	switch(iChannel)
	{
	case InitAV:
		setAudioSourceSelect(S_AUDIO_NO_SOURCE,InitAV);
		break;

	case MediaCD:  
		 setAudioSourceSelect(S_NEXTER_SOURCE,MediaCD);
		break;

	case AUX:// 3=AUX
		setAudioSourceSelect(S_AUDIO_AUX_SOURCE,AUX);
		break;

	case AV_CDC:
		setAudioSourceSelect(S_AUDIO_CDC_SOURCE,AV_CDC);
		break;

	case RADIO://1=RADIO
		setAudioSourceSelect(S_AUDIO_RADIO_SOURCE,RADIO);
		break;

	case BT:
	case BT_RING:
		iChannel = BT;
		setAudioSourceSelect(S_DEFINE_SOURCE,iChannel);
		break;

	case A2DP:
		setAudioSourceSelect(S_NEXTER_SOURCE,A2DP);
		break;

	case MediaMP3:
		setAudioSourceSelect(S_AUDIO_MP3_SOURCE,MediaMP3);
		break;

	case VAP:
		setAudioSourceSelect(S_AUDIO_VAP_SOURCE,VAP);
		break;

	default:
		setAudioSourceSelect(S_DEFINE_SOURCE,iChannel);
		break;
	}

	gAudioInfo.iCurAudioChannel = iChannel;
	SendMsgToModules(VIDEO_MODULE, SET_VIDEO_AUDIO, gAudioInfo.iCurAudioChannel);
}

void DealDataStreams(BYTE *buf, UINT16 len)
{
	UINT16 i=0;
	DBGD((TEXT("\r\nEXE-Audio Socket Read %d bytes:"),len));
	for (i=0; i<len; i++)
		DBGD((TEXT("%02X "),buf[i]));

	UINT32 temp=0;
	struct audio_t *pAudioInfo = &gAudioInfo;


	switch (buf[0])
	{
	case 0x10://静音
		setMute(buf[1]);
		break;

	case 0x11://音量
		if (buf[1] >= VOLUME_LEVEL_MAX)
			buf[1] = 60;

		ControlAudioVolume(buf[1]);
		gAudioInfo.iCurAudioVolume = buf[1];
		break;

	//case 0x12://响度
	//	if (buf[1] >= LOUDNESS_LEVEL_MAX)
	//		buf[1] = LOUDNESS_LEVEL_MAX-1;

	//	temp = LOUDNESS_TABLE[buf[1]];
	//	setLoudness(temp);
	//	break;

	//case 0x13://低音
	//	if (buf[1] >= TREBLE_MID_BASS_LEVEL_MAX)
	//		buf[1] = TREBLE_MID_BASS_LEVEL_MAX-1;

	//	temp = TREBLE_MID_BASS_TABLE[buf[1]];
	//	setBass(temp);
	//	break;

	//case 0x14://中音
	//	if (buf[1] >= TREBLE_MID_BASS_LEVEL_MAX)
	//		buf[1] = TREBLE_MID_BASS_LEVEL_MAX-1;

	//	temp = TREBLE_MID_BASS_TABLE[buf[1]];
	//	setMid(temp);
	//	break;

	//case 0x15://高音
	//	if (buf[1] >= TREBLE_MID_BASS_LEVEL_MAX)
	//		buf[1] = TREBLE_MID_BASS_LEVEL_MAX-1;

	//	temp = TREBLE_MID_BASS_TABLE[buf[1]];
	//	setTreble(temp);
	//	break;

	case 0x16://前后左右平衡
		if (buf[1] >= FR_LR_BALANCE_MAX)
			buf[1] = FR_LR_BALANCE_MAX-1;
		if (buf[2] >= FR_LR_BALANCE_MAX)
			buf[2] = FR_LR_BALANCE_MAX-1;

		BYTE iFL,iFR,iRL,iRR,ibalance,iFader;
		ibalance = buf[1];
		iFader   = buf[2];
		
		iFL = 40-FR_LR_BALANCE_TABLE[LARGER(ibalance,iFader)];				//前左
		iFR = 40-FR_LR_BALANCE_TABLE[LARGER((20-ibalance),iFader)];			//前右
		iRL = 40-FR_LR_BALANCE_TABLE[LARGER(ibalance,(20-iFader))];			//后左
		iRR = 40-FR_LR_BALANCE_TABLE[LARGER((20-ibalance),(20-iFader))];	//后右

		DBGD((TEXT("[Audio DLL] iFL:%d,iFR:%d,iRL:%d,iRR:%d\r\n"),iFL,iFR,iRL,iRR));
		
		setBalance(iFL,iFR,iRL,iRR);
		break;

	case 0x18://声道切换
		ControlAudioSourceChannel(buf[1]);
		break;

	case 0x19://EQ 类型
		//if (buf[1] < 0x06)
		//{
		//	setEQType(getEQCurType(buf[1],FALSE));
		//}
		break;

	case 0x1A://EQ VALUES
		MISC_EQ_GAIN_T eqGain;
		BYTE buff[EQ_VOLUE_TOTAL_NUM];

		for (UINT16 i=0; i<EQ_VOLUE_TOTAL_NUM; i++)
		{
			buff[i] = EQ_VALUES_TABLE[buf[1+i]];
		}

		dealEQGains(&eqGain,&buff[0],EQ_VOLUE_TOTAL_NUM,TRUE);
		setEQValues(eqGain);
		break;
	case 0x1B:
		gAudioInfo.iCurAudioParma = buf[1];
		ControlAudioVolume(gAudioInfo.iCurAudioVolume);
		break;

	case 0x1C:
		if (0x01 == buf[1]){
			setLoudness(LOUDNESS_TABLE[LOUDNESS_LEVEL_MAX/2]);
		}else{
			setLoudness(LOUDNESS_TABLE[0]);
		}
		break;

	case 0x1D:
		if (0x01 == buf[1]){
			gAudioInfo.bSubAudioStatus = TRUE;//5.1
		}else{
			gAudioInfo.bSubAudioStatus = FALSE;
		}
		setSpkAudioType(gAudioInfo.bSubAudioStatus);
		break;


	case 0x20:
		setBackVideoSigned(buf[1]);
		break;

	case 0x49:
		if (!buf[1])
			gAudioInfo.iCurGPSVolume = 0;
		else
		{
			if (buf[2] == 1)
			{
				temp = GPS_VOLUME_LEVEL_1;
			}
			else if (buf[2] == 2)
			{
				temp = GPS_VOLUME_LEVEL_2;
			}
			else
			{
				temp = GPS_VOLUME_LEVEL_3;
			}
			gAudioInfo.iCurGPSVolume = buf[1]+temp;
		}
		setGPSVolume(GPS_VOLUME_TABLE[gAudioInfo.iCurGPSVolume]);
		break;

	default:
		break;
	}

	//returnCurAudioSetting(buf,len);
}

static BOOL InitAudioStruct(struct audio_t *pAudioInfo, BOOL bInit)
{
	assert(NULL != pAudioInfo);

	if (bInit)
	{
		memset(pAudioInfo,0,
			sizeof(struct audio_t)-sizeof(FlyGlobal_T));

		pAudioInfo->iCurAudioChannel = InitAV;
		gAudioInfo.iCurAudioParma = 0;
		gAudioInfo.bSubAudioStatus = FALSE;
	}

	return TRUE;
}

static BOOL Init(void)
{
	struct audio_t *pAudioInfo = &gAudioInfo;

	//初始化audio info结构体
	if(!InitAudioStruct(pAudioInfo,TRUE))
	{
		DBGE((TEXT("[Audio DLL] init struct err.\r\n")));
		return FALSE;
	}

	if (!AudioSourceInit())
	{	
		DBGE((TEXT("[Audio DLL] audio source init err.\r\n")));
		return FALSE;
	}

	if (!CreateClientSocket(9983))
	{
		DBGE((TEXT("[Audio DLL] audio create client socket err.\r\n")));
		return FALSE;
	}
	
	pAudioInfo->bInit = TRUE;
	returnAudioInitStatus(pAudioInfo->bInit);

	DBGI((TEXT("\r\n[Audio DLL] Init OK time:")));
	DBGI((TEXT(__TIME__)));
	DBGI((TEXT(" data:")));
	DBGI((TEXT(__DATE__)));
	DBGI((TEXT("\r\n")));
	return TRUE;
}

static BOOL  Deinit(void)
{
	struct audio_t *pAudioInfo = &gAudioInfo;


	//AudioSourceRelease();

	pAudioInfo->bInit = FALSE;
	return TRUE;	
}

static BOOL Write(BYTE *buf, UINT32 len)
{
	switch (buf[0])
	{
	case CMD_GPS_PROCESS_ID:
		 setProcessID(CMD_GPS_PROCESS_ID,ForU8ToU32(&buf[1]));
		break;

	case CMD_MP3_PROCESS_ID:
		setProcessID(CMD_MP3_PROCESS_ID,ForU8ToU32(&buf[1]));
		break;

	case CMD_DVD_INIT_STATUS:
		setDVDInitStatus(buf[1]);
		//if (buf[1]){
		//	setSpkAudioType(gAudioInfo.bSubAudioStatus);
		//}
		break;

	case CMD_DVD_AUDIO_SOURCE:
		if (buf[1]){
			setSpkAudioType(gAudioInfo.bSubAudioStatus);
		}
		break;

	default:
		break;
	}


	return TRUE;
}

static FlyGlobal_T* RegisterApiTable(struct audio_t *pAudioInfo)
{
	assert(NULL != pAudioInfo);

	pAudioInfo->sApiTable.Init   = Init;
	pAudioInfo->sApiTable.DeInit = Deinit;
	pAudioInfo->sApiTable.Write  = Write;
	//RETAILMSG(1,(TEXT("DLL pGlobalTale:%p.\r\n"),pGlobalTale));

	return &pAudioInfo->sApiTable;
}

//导出涵数
AUDIO_API FlyGlobal_T *GetDllProcAddr(void)
{
	return RegisterApiTable(&gAudioInfo);
}
