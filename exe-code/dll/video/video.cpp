// video.cpp : 定义 DLL 应用程序的入口点。
//

#include "video.h"
#include <windows.h>
#include <commctrl.h>

#include "videoApi.h"
#include <assert.h>
#include "flyshm.h"
#include "common.h"


struct video_t gVideoInfo;

static void returnCurVideoSetting(BYTE *buf,UINT16 len)
{
	SocketWrite(buf, len);
}
static void returnCurVideoSigned(BYTE iChannel, BOOL bSigned)
{
	BYTE buf[3] = {0x20,0x00,0x00};
	buf[1] = iChannel;
	buf[2] = bSigned;
	SocketWrite(buf, 3);
}
static void returnVideoInitStatus(BOOL bInit)
{
	BYTE buf[2] = {0x01,0x00};
	buf[1] = bInit;
	SocketWrite(buf,2);
}

static void ControlVideoBrightness(UINT16 iValue)
{
	struct video_t *pVideoInfo = &gVideoInfo;

	switch (pVideoInfo->iCurVideoChannel)
	{
	case CAR_RECORDER:
	case AV_CDC:
	case BACK:
		setBrightness(sBrightness_AVIN[iValue]);
		DBGD((TEXT("\r\n[EXE-Video]setBrightness AVIN:%d"),sBrightness_AVIN[iValue]));
		break;

	case AUX:
		if (pVideoInfo->bAuxVideoSignal)
		{
			setBrightness(sBrightness_AVIN[iValue]);
			DBGD((TEXT("\r\n[EXE-Video]setBrightness AUX AVIN:%d"),sBrightness_AVIN[iValue]));
		}
		else
		{
			setBrightness(SET_BRIGHTNESS_DEFAUT);
			DBGD((TEXT("\r\n[EXE-Video]setBrightness AUX AVIN def")));
		}
		break;

	case MediaCD:
	case VAP:
		setBrightness(sBrightness_Def[iValue]);
		DBGD((TEXT("\r\n[EXE-Video]setBrightness def:%d"),sBrightness_Def[iValue]));
		break;

	default:
		setBrightness(SET_BRIGHTNESS_DEFAUT);
		DBGD((TEXT("\r\n[EXE-Video]setBrightness def")));
		break;
	}
}


static void ControlVideoContrast(UINT16 iValue)
{
	struct video_t *pVideoInfo = &gVideoInfo;
	DBGD((TEXT("\r\n[EXE-Video]curnet  Video:%d"),pVideoInfo->iCurVideoChannel));


	switch (pVideoInfo->iCurVideoChannel)
	{
	case CAR_RECORDER:
	case AV_CDC:
	case BACK:
	case VAP:
	case MediaCD:
		setContrast(sContrast_Video[iValue]);
		DBGD((TEXT("\r\n[EXE-Video]setContrast Video:%d"),sContrast_Video[iValue]));
		break;

	case AUX:
		if (pVideoInfo->bAuxVideoSignal)
		{
			setContrast(sContrast_Video[iValue]);
			DBGD((TEXT("\r\n[EXE-Video]setContrast AUX Video:%d"),sContrast_Video[iValue]));
		}
		else
		{
			setContrast(SET_CONTRAST_DEFAUT);
			DBGD((TEXT("\r\n[EXE-Video]setContrast AUX Video def")));
		}
		break;

	default:
		setContrast(SET_CONTRAST_DEFAUT);
		DBGD((TEXT("\r\n[EXE-Video]setContrast def:")));
		break;
	}
}

static void VideoEnterAndExitSetDefColor(BOOL bVideoEnter)
{
	if (bVideoEnter)
	{
		setBrightness(sBrightness_AVIN[5]);
		setContrast(sContrast_Video[5]);
	}
	else 
	{
		setBrightness(SET_BRIGHTNESS_DEFAUT);
		setContrast(SET_CONTRAST_DEFAUT);
	}
}

static void ControlVideoColor(UINT16 iType, UINT16 iValue)
{
	if (iValue > COLOR_STEP_COUNT-1)
		return;

	UINT32 temp = 0;
	struct video_t *pVideoInfo = &gVideoInfo;

	switch (iType)
	{
	case 0x00://饱和度
		setSaturation(sColor_N[iValue]);
		break;

	case 0x01://色调
		setHue(sHue_N[iValue]);
		break;

	case 0x02://对比度
		ControlVideoContrast(iValue);
		break;

	case 0x03://亮度
		ControlVideoBrightness(iValue);
		break;

	case 0x04://alpha
		break;

	default:
		break;

	}
}

static void ControlVideoBackSigned(BOOL bSigned)
{
	if (bSigned)
	{
		setYUVGain(FALSE);
	}
	else
	{
		setYUVGain(TRUE);
	}
	setBackVideoSigned(bSigned);
}

static void ControlVideoDisplay(UINT16 iChannel)
{
	struct video_t *pVideoInfo = &gVideoInfo;

	switch (iChannel)
	{
	case CAR_RECORDER:
	case AV_CDC:
		setYUVGain(FALSE);
		break;

	case BACK://倒车时当有倒车没视频时不处理
		break;

	case AUX:
		if (pVideoInfo->bAuxVideoSignal)
		{
			setYUVGain(FALSE);
		}
		else
		{
			setYUVGain(TRUE);
		}
		break;

	default:
		setYUVGain(TRUE);
		break;
	}
}
static void ControlVideoChannel(UINT16 iChannel)
{
	struct video_t *pVideoInfo = &gVideoInfo;

	switch (iChannel)
	{
	case InitAV:
		setVideoSource(S_NEXTER_SOURCE,InitAV);
		pVideoInfo->bAuxVideoSignal = FALSE;
		break;

	case MediaCD:
		setVideoSource(S_NEXTER_SOURCE,MediaCD);
		break;

	case MediaMP3:
		setVideoSource(S_NEXTER_SOURCE,MediaMP3);
		break;

	case AUX://AUX 视频 2
		VideoEnterAndExitSetDefColor(TRUE);
		setVideoSource(S_VIDEO_AUX_SOURCE,AUX);
		break;

	case CAR_RECORDER://行车记录仪与CDC
		setVideoSource(S_VIDEO_CDC_SOURCE,AV_CDC);
		break;

	case AV_CDC:
		setVideoSource(S_VIDEO_CDC_SOURCE,AV_CDC);
		break;

	case VAP:
		setVideoSource(S_NEXTER_SOURCE,VAP);
		break;

	case BACK://倒车 3
		setVideoSource(S_NEXTER_SOURCE,BACK);
		break;

	default:
		break;
	}
	
	gVideoInfo.iCurVideoChannel = iChannel;
	ControlVideoDisplay(gVideoInfo.iCurVideoChannel);
}

void DealDataStreams(BYTE *buf, UINT16 len)
{
	UINT16 i=0;
	DBGI((TEXT("\r\nEXE-Video Socket Read %d bytes:"),len));
	for (i=0; i<len; i++)
		DBGI((TEXT("%02X "),buf[i]));

	struct video_t *pVideoInfo = &gVideoInfo;

	switch (buf[0])
	{
	case 0x10:
		ControlVideoChannel(buf[1]);
		break;

	case 0x11:
		ControlVideoColor(buf[1],buf[2]);
		break;

	case 0x12:
		ControlVideoBackSigned(buf[1]);
		break;

	default:
		break;
	}

	//returnCurVideoSetting(buf,len);
}


static BOOL InitAudioStruct(struct video_t *pVideoInfo, BOOL bInit)
{
	assert(NULL != pVideoInfo);

	if (bInit)
	{
		memset(pVideoInfo,0,
			sizeof(struct video_t)-sizeof(FlyGlobal_T));
	}

	pVideoInfo->bAuxVideoSignal  = FALSE;
	pVideoInfo->iCurVideoChannel = InitAV;
	ControlVideoChannel(InitAV);

	return TRUE;
}

static BOOL Init(void)
{
	struct video_t *pVideoInfo = &gVideoInfo;

	//初始化audio info结构体
	if(!InitAudioStruct(pVideoInfo,TRUE))
	{
		DBGE((TEXT("[Video DLL] init struct err.\r\n")));
		return FALSE;
	}

	if (!videoSourceInit())
	{
		DBGE((TEXT("[Video DLL] init source err.\r\n")));
		return FALSE;
	}


	if (!CreateClientSocket(9985))
	{
		DBGE((TEXT("[Video DLL] audio create client socket err.\r\n")));
		return FALSE;
	}

	pVideoInfo->bInit = TRUE;
	returnVideoInitStatus(pVideoInfo->bInit);

	DBGI((TEXT("\r\n[Video DLL] Init OK time:")));
	DBGI((TEXT(__TIME__)));
	DBGI((TEXT(" data:")));
	DBGI((TEXT(__DATE__)));
	DBGI((TEXT("\r\n")));

	return TRUE;
}

static BOOL  Deinit(void)
{
	struct video_t *pVideoInfo = &gVideoInfo;

	videoSourceReleass();
	pVideoInfo->bInit = FALSE;
	return TRUE;	
}
static BOOL Write(BYTE *buf, UINT32 len)
{
	struct video_t *pVideoInfo = &gVideoInfo;

	switch (buf[0])
	{
	case CMD_VIDEO_SIGNED:
		if (buf[1] == 0x02)//AUX通道
		{
			pVideoInfo->bAuxVideoSignal = buf[2];//AUX信号
			if (AUX == gVideoInfo.iCurVideoChannel)
				ControlVideoDisplay(AUX);
			ControlAuxSignal(pVideoInfo->bAuxVideoSignal);
			returnCurVideoSigned(AUX,pVideoInfo->bAuxVideoSignal);
		}
		break;
	case CMD_CUR_AUDIO_SOURCE://当前音频通道
		ControlCurAudioChannel(buf[1]);
		break;

	default:
		break;
	}


	return TRUE;
}

static FlyGlobal_T* RegisterApiTable(struct video_t *pVideoInfo)
{
	assert(NULL != pVideoInfo);

	pVideoInfo->sApiTable.Init   = Init;
	pVideoInfo->sApiTable.DeInit = Deinit;
	pVideoInfo->sApiTable.Write  = Write;
	//RETAILMSG(1,(TEXT("DLL pGlobalTale:%p.\r\n"),pGlobalTale));

	return &pVideoInfo->sApiTable;
}

//导出涵数
VIDEO_API FlyGlobal_T *GetDllProcAddr(void)
{
	return RegisterApiTable(&gVideoInfo);
}
