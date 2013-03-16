// video.cpp : 定义 DLL 应用程序的入口点。
//

#include "video.h"
#include <windows.h>

#include "videoApi.h"
#include "common.h"
#include <assert.h>
#include "FlyShm.h"

struct video_t gVideoInfo;

static void returnVideoSigned(UINT16 iChannel, BOOL bIsHave)
{
	BYTE buf[3]={0x20};
	buf[1] = iChannel;
	buf[2] = bIsHave;
	SocketWrite(buf, 3);
}

static void ControlVideoChannel(UINT16 iChannel)
{
	switch (iChannel)
	{
	case InitAV:
		setVideoSource(S_NEXTER_SOURCE,InitAV);
		SetEvent(gVideoInfo.hVideoMainEvent);
		return;
		break;

	case MediaCD:
		setVideoSource(S_NEXTER_SOURCE,MediaCD);
		break;

	case AUX://AUX 视频 2
		setVideoSource(S_VIDEO_AUX_SOURCE,AUX);
		returnVideoSigned(AUX, IsHaveVideoSignal(AUX));
		break;

	case 0x11://行车记录仪
		setVideoSource(S_VIDEO_CDC_SOURCE,CDC);
		break;

	//case BACK://倒车 3

	////	setVideoSource(3,BACK);
	//	break;

	default:
		break;
	}

	gVideoInfo.iCurVideoChannel = iChannel;
}

void ControlVideoColor(UINT16 iType, UINT16 iValue)
{
	if (iValue > COLOR_STEP_COUNT-1)
		return;

	UINT32 temp = 0;

	switch (iType)
	{
	case 0x00://饱和度
		getSaturation(&temp);
		if (temp != sColor_N[iValue])
			setSaturation(sColor_N[iValue]);
		break;

	case 0x01://色调
		getHue(&temp);
		if (temp != sHue_N[iValue])
			setHue(sHue_N[iValue]);
		break;

	case 0x02://对比度
		getContrast(&temp);
		if (temp != sContrast_N[iValue])
			setContrast(sContrast_N[iValue]);
		break;

	case 0x03://亮度
		getBrightness(&temp);
		if (temp != sBrightness_N[iValue])
			setBrightness(sBrightness_N[iValue]);
		break;

	default:
		break;

	}

	getSaturation(&temp);
	DBGI((TEXT("\r\n[Video DLL Set] getSaturation:%d\r\n"),temp));
	getHue(&temp);
	DBGI((TEXT("          getHue:%d\r\n",),temp));
	getContrast(&temp);
	DBGI((TEXT("          getContrast:%d\r\n",),temp));
	getBrightness(&temp);
	DBGI((TEXT("          getBrightness:%d\r\n",),temp));
}
void DealDataStreams(BYTE *buf, UINT16 len)
{
	UINT16 i=0;
	DBGD((TEXT("\r\nEXE-Video Socket Read %d bytes:"),len));
	for (i=0; i<len; i++)
		DBGD((TEXT("%02X "),buf[i]));

	struct video_t *pVideoInfo = &gVideoInfo;

	switch (buf[0])
	{
	case 0x10:
		ControlVideoChannel(buf[1]);
		break;

	case 0x11:
		ControlVideoColor(buf[1],buf[2]);
		break;

	default:
		break;
	}
}


static BOOL InitAudioStruct(struct video_t *pVideoInfo, BOOL bInit)
{
	assert(NULL != pVideoInfo);

	if (bInit)
	{
		memset(pVideoInfo,0,
			sizeof(struct video_t)-sizeof(FlyGlobal_T));
	}

	pVideoInfo->iIsHaveAUXVideoSignal = 0xFF;
	pVideoInfo->iCurVideoChannel = InitAV;

	return TRUE;
}

DWORD VideoMainThread(LPVOID pContext)
{
	UINT16 ret=0;
	struct video_t *pVideoInfo = (struct video_t *)pContext;

	while (!pVideoInfo->bVideoKillMainThreadFlag)
	{
		WaitForSingleObject(pVideoInfo->hVideoMainEvent,INFINITE);
		
		returnVideoSigned(AUX, IsHaveVideoSignal(AUX));
		ResetVideoSignal(AUX);

		if (pVideoInfo->iCurVideoChannel == AUX)
		{
			//TestVideoSignal(2);
		}
		
	}

	return 0;
}

BOOL CreateVideoThread(struct video_t *pVideoInfo)
{
	DWORD dwThreadID;

	pVideoInfo->bVideoKillMainThreadFlag = FALSE;
	pVideoInfo->hVideoMainEvent = CreateEvent(NULL, FALSE,FALSE,TO_VIDEO_SIGNAL_TEST_EVENT_NAME);	
	pVideoInfo->hVideoMainThead = CreateThread( 
		(LPSECURITY_ATTRIBUTES) NULL,0,	(LPTHREAD_START_ROUTINE)VideoMainThread, pVideoInfo,0, &dwThreadID );
	if (NULL == pVideoInfo->hVideoMainThead)
	{
		return FALSE;
	}
	CeSetThreadPriority(pVideoInfo->hVideoMainThead, 50);
	SetEvent(pVideoInfo->hVideoMainEvent);

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

	if (!VideoSourceInit())
	{
		DBGE((TEXT("[Video DLL] init source err.\r\n")));
		return FALSE;
	}


	if (!CreateClientSocket(9985))
	{
		DBGE((TEXT("[Video DLL] audio create client socket err.\r\n")));
		return FALSE;
	}

	CreateVideoThread(pVideoInfo);
	pVideoInfo->bInit = TRUE;

	BYTE buf[2] = {0x01,0x01};
	SocketWrite(buf,2);

	UINT32 temp=0;
	getSaturation(&temp);
	DBGI((TEXT("\r\n[Video DLL Init] getSaturation:%d\r\n",),temp));
	getHue(&temp);
	DBGI((TEXT("          getHue:%d\r\n",),temp));
	getContrast(&temp);
	DBGI((TEXT("          getContrast:%d\r\n",),temp));
	getBrightness(&temp);
	DBGI((TEXT("          getBrightness:%d\r\n",),temp));

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

	vedeoSourceReleass();
	pVideoInfo->bInit = FALSE;
	return TRUE;	
}

static FlyGlobal_T* RegisterApiTable(struct video_t *pVideoInfo)
{
	assert(NULL != pVideoInfo);

	pVideoInfo->sApiTable.Init   = Init;
	pVideoInfo->sApiTable.DeInit = Deinit;
	pVideoInfo->sApiTable.Write  = NULL;
	//RETAILMSG(1,(TEXT("DLL pGlobalTale:%p.\r\n"),pGlobalTale));

	return &pVideoInfo->sApiTable;
}

//导出涵数
VIDEO_API FlyGlobal_T *GetDllProcAddr(void)
{
	return RegisterApiTable(&gVideoInfo);
}
