// FLY5150.cpp : Defines the entry point for the DLL application.
//

#include "flyhwlib.h"
#include "flyuserlib.h"
#include "flysocketlib.h"
#include "FLY5150.h"


P_FLY_5150_INFO gp5150Info = NULL;


BYTE sColor_N[COLOR_STEP_COUNT]		 = {0,1,2,3,4,5,6,7,8,9,10};
BYTE sContrast_N[COLOR_STEP_COUNT]	 = {0,1,2,3,4,5,6,7,8,9,10};
BYTE sBrightness_N[COLOR_STEP_COUNT] = {0,1,2,3,4,5,6,7,8,9,10};
BYTE sHue_N[COLOR_STEP_COUNT]		 = {0,1,2,3,4,5,6,7,8,9,10};


VOID Fly5150Enable(P_FLY_5150_INFO p5150Info,BOOL bOn);
VOID FVO_PowerUp(DWORD hDeviceContext);
VOID FVO_PowerDown(DWORD hDeviceContext);
static void powerNormalInit(P_FLY_5150_INFO p5150Info);



void returnVideoPowerMode(P_FLY_5150_INFO p5150Info,BYTE bPower)
{
	BYTE buff[] = {0x01,0x00};
	buff[1] = bPower;

	ReturnToUser(buff,2);
}

void returnVideoWorkMode(P_FLY_5150_INFO p5150Info,BYTE bWork)
{
	BYTE buff[] = {0x02,0x00};
	buff[1] = bWork;

	ReturnToUser(buff,2);
}

void returnDisplayValue(P_FLY_5150_INFO p5150Info,setDisplay enumWhat,BYTE iValue)
{
	BYTE buff[3] = {0x11,0x00,0x00};
	
	buff[1] = enumWhat;
	buff[2] = iValue;

	ReturnToUser(buff,3);
}
static void returnCurMainVideoChannel(P_FLY_5150_INFO p5150Info,BYTE iChannel)
{
	BYTE buff[2] = {0x10,0x00};
	buff[1] = iChannel;

	ReturnToUser(buff,2);
}

static void controlDisplayValue(P_FLY_5150_INFO p5150Info,setDisplay enumWhat,BYTE iValue)
{
	BYTE buff[3] = {0x11,0x00,0x00};

	buff[1] = enumWhat;
	buff[2] = iValue;

	SocketWrite(buff,3);
}
static void controlVideoChannel(P_FLY_5150_INFO p5150Info,BYTE iChannel)
{
	BYTE buff[2] = {0x10,0x00};

	buff[1] = iChannel;

	SocketWrite(buff,2);
}
static void controlVideoBackSigned(P_FLY_5150_INFO p5150Info,BOOL bSigned)
{
	BYTE buff[2] = {0x12,0x00};

	buff[1] = bSigned;

	SocketWrite(buff,2);
}

static void actualChangeVideoInput(P_FLY_5150_INFO p5150Info,PVIN_CONFIG p,VIN_CONFIG_CODE configWhat)
{
	DBGI((TEXT("\r\nFlyAudio 5150 Video Config Bri:%x,Cor:%x,Hue:%x,Cont:%x,Alpha:%x\r\nWhat:%d")
	,p->brightness,p->color,p->hue,p->contrast,p->alpha,configWhat));

	switch (configWhat)
	{
	case SET_COLORE:
		controlDisplayValue(p5150Info,E_C_COLOR,p->color);
		break;

	case SET_HUE:
		controlDisplayValue(p5150Info,E_C_HUE,p->hue);
		break;

	case SET_CONTRAST:
		controlDisplayValue(p5150Info,E_C_CONTRAST,p->contrast);
		break;

	case SET_ALPHA:
		break;

	case SET_BRIGHTNESS:
		controlDisplayValue(p5150Info,E_C_BRIGHTNESS,p->brightness);
		break;

	case SET_ALL:
		controlDisplayValue(p5150Info,E_C_BRIGHTNESS,p->brightness);
		controlDisplayValue(p5150Info,E_C_CONTRAST,p->contrast);
		controlDisplayValue(p5150Info,E_C_COLOR,p->color);
		controlDisplayValue(p5150Info,E_C_HUE,p->hue);
		break;

	default://其它不理会
		break;
	}
}

//倒车是否有视频事件，如江德鹏发出
static DWORD WINAPI BackCarThread(LPVOID pContext)
{
	DWORD ret = 0,pre_ret = 0;
	P_FLY_5150_INFO p5150Info = (P_FLY_5150_INFO)pContext;
	DBGI((TEXT("\r\nBack Car thread running........\r\n")));
	while (!p5150Info->bKillDispatchFlyBackCarThread)
	{
		
		WaitForSingleObject(p5150Info->hDispatchBackCarThreadEvent, INFINITE);
		ret  = GetEventData(p5150Info->hDispatchBackCarThreadEvent);
		DBGI((TEXT("\r\nBack Car bHave old:%d, new:%d\r\n"),pre_ret,ret));
		//if (pre_ret == ret)
		//	continue;

		EnterCriticalSection(&p5150Info->cDealBackcarThreadSection);
		//pre_ret = ret;
		if (ret & 0x10)
		{
			p5150Info->bBackVideoStartAction = TRUE;
			p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow = TRUE;
			DBGI((TEXT("\r\nBack Car Action On\r\n")));
		}
		else
		{
			if ((ret&0x2))//倒车
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow = TRUE;
			}
			else
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow = FALSE;
			}

			if ((ret&0x1))//视频
			{
				p5150Info->bBackVideoOn = TRUE;
			}
			else
			{
				p5150Info->bBackVideoOn = FALSE;
			}

			p5150Info->bBackVideoDealWINAction  = TRUE;
			//p5150Info->bBackVideoStartAction = TRUE;

			IpcStartEvent(EVENT_GLOBAL_STANDBY_DVD_ID);
			DBGI((TEXT("\r\nBack Car bHave old:%d, Action:%d, video:%d\r\n"),ret,
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow,p5150Info->bBackVideoOn));
		}
		IpcStartEvent(EVENT_GLOBAL_BACK_VIDEO_SIGNED_ID);
		SetEvent(p5150Info->hDispatchMainThreadEvent);

		LeaveCriticalSection(&p5150Info->cDealBackcarThreadSection);
	}

	DBGI((TEXT("\r\nBack Car thread exit\r\n")));
	p5150Info->bKillDispatchFlyBackCarThread = TRUE;
	return 0;
}

//AUX是否有视频事件，如江德鹏发出
static DWORD WINAPI AuxListerThread(LPVOID pContext)
{
	P_FLY_5150_INFO p5150Info = (P_FLY_5150_INFO)pContext;

	DBGI((TEXT("\r\nAUX Lister thread running........\r\n")));
	while (!p5150Info->bKillDispatchFlyAuxListerThread)
	{

		WaitForSingleObject(p5150Info->hDispatchAuxListerThreadEvent, INFINITE);

		if(GetEventData(p5150Info->hDispatchAuxListerThreadEvent))
		{
			p5150Info->bAuxValue = TRUE;
		}
		else
		{
			p5150Info->bAuxValue = FALSE;
		}

		DBGI((TEXT("\r\nAUX Have video:%d\r\n"),p5150Info->bAuxValue));
	}

	DBGI((TEXT("\r\nAUX Lister thread exit\r\n")));
	p5150Info->bKillDispatchFlyAuxListerThread = TRUE;
	return 0;
}


static void getVideoColor(P_FLY_5150_INFO p5150Info,BYTE iSetVideo)
{
	switch (iSetVideo)
	{
	case SET_VIDEO_COLOR:
		p5150Info->iTempVideoParaColor = p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaColor;
		p5150Info->iTempVideoParaHue   = p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaHue;
		p5150Info->iTempVideoParaContrast = p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaContrast;
		p5150Info->iTempVideoParaBrightness = p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaBrightness;
		break;

	case SET_DEF_COLOR:
		p5150Info->iTempVideoParaColor      = COLOR_STEP_COUNT/2;
		p5150Info->iTempVideoParaHue        = COLOR_STEP_COUNT/2;
		p5150Info->iTempVideoParaContrast   = COLOR_STEP_COUNT/2;
		p5150Info->iTempVideoParaBrightness = COLOR_STEP_COUNT/2;
		break;

	case SET_RESET_COLOR:
		p5150Info->iVideoParaContrast   = !p5150Info->iTempVideoParaContrast;
		p5150Info->iVideoParaBrightness = !p5150Info->iTempVideoParaBrightness;
		p5150Info->iVideoParaColor = !p5150Info->iTempVideoParaColor;
		p5150Info->iVideoParaHue = !p5150Info->iTempVideoParaHue;
		break;

	default:
		break;
	}
}

static void setVideoColor(P_FLY_5150_INFO p5150Info)
{
	DBGI((TEXT("\r\nFlyAudio 5150 setVideoColor Current Channel: %d,aux signal:%d, color reset:%d"),
		p5150Info->iCurVideoChannel, p5150Info->bIsHaveAUXVideoSignal,p5150Info->bAUXVideoColorReset));
	switch (p5150Info->iCurVideoChannel)
	{
	case AUX://AUX有信号时才进行色彩设置
		if (p5150Info->bIsHaveAUXVideoSignal)// && p5150Info->bAUXVideoColorReset)
		{
			getVideoColor(p5150Info,SET_VIDEO_COLOR);
			p5150Info->bSetColorEnable = TRUE;
		}
		return;
		break;

	case CDC:
	case TV:
	case VAP:
	case MediaCD:
	case CAR_RECORDER:
		getVideoColor(p5150Info,SET_VIDEO_COLOR);
		p5150Info->bSetColorEnable = TRUE;
		break;

	case BACK:
		if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow && p5150Info->bBackVideoOn)//有倒车，有视频
		{
			getVideoColor(p5150Info,SET_DEF_COLOR);
			p5150Info->bSetColorEnable = TRUE;
		}
		else
		{
			p5150Info->bSetColorEnable = FALSE;
		}
		break;

	default:
		if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
		{//ARM2开机有倒车？？
			p5150Info->iTempVideoChannel = BACK;
			p5150Info->bSetColorEnable = FALSE;
		}
		else if (p5150Info->bVideoFirstSelectChannel){
			getVideoColor(p5150Info,SET_DEF_COLOR);
			p5150Info->bSetColorEnable = TRUE;
			p5150Info->iOldVideoChannel = !p5150Info->iCurVideoChannel;
		}
		break;
	}


	if (p5150Info->iOldVideoChannel != p5150Info->iCurVideoChannel)
	{
		//重新设置一次
		getVideoColor(p5150Info,SET_RESET_COLOR);
		p5150Info->iOldVideoChannel = p5150Info->iCurVideoChannel;
	}
}

static DWORD WINAPI ThreadMain(LPVOID pContext)
{
	P_FLY_5150_INFO p5150Info = (P_FLY_5150_INFO)pContext;
	ULONG waitTime = 0;

	p5150Info->bPowerUp = TRUE;
	DBGI((TEXT("\r\nFlyAudio 5150 ThreadMain Start")));

	while (!p5150Info->bKillDispatchFlyMainThread)
	{
		if (0 == waitTime)
		{
			WaitForSingleObject(p5150Info->hDispatchMainThreadEvent, INFINITE);
		} 
		else
		{
			WaitForSingleObject(p5150Info->hDispatchMainThreadEvent, waitTime);
		}
		DBGI((TEXT("\r\nFlyAudio 5150 ThreadMain Running bNeed %d bHave %d WaitTime %d")
			,p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeVideoSleep
			,p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeVideoSleep
			,waitTime));

			waitTime = 0;


		if (IpcWhatEvent(EVENT_GLOBAL_SLEEP_PROC_VIDEO_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_SLEEP_PROC_VIDEO_ID);
			if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeVideoSleep
				!= p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeVideoSleep)
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeVideoSleep
					= p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeVideoSleep;

				powerNormalInit(p5150Info);

				DBGI((TEXT("\r\nFlyAudio 5150 Proc Before Sleep Finish")));
			}
		}
		if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeVideoSleep)
		{
			continue;
		}

		if (FALSE == p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeVideoSleep
			&& FALSE == p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeVideoSleep)
		{
			p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeVideoSleep = TRUE;
			p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeVideoSleep = FALSE;
			//powerNormalInit(p5150Info);
		}

		if (IpcWhatEvent(EVENT_GLOBAL_STANDBY_VIDEO_ID))//因为碟机复位，延时一下
		{
			IpcClearEvent(EVENT_GLOBAL_STANDBY_VIDEO_ID);

			if (!p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)
			{
				if (p5150Info->bPreVideoEnable && MediaCD == p5150Info->iPreVideoChannel)
				{
					Sleep(618);
				}
			}
		}

		if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable
			&& p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
		{
			if (p5150Info->bBackVideoStartAction)
			{
				if (p5150Info->bBackVideoOn)
				{
					p5150Info->bTempVideoEnable = TRUE;
					p5150Info->iTempVideoChannel = BACK;
				}
				else
				{
					p5150Info->bTempVideoEnable = FALSE;
					p5150Info->iTempVideoChannel = BACK;
				}
			}

		}
		else
		{
			p5150Info->bTempVideoEnable = p5150Info->bPreVideoEnable;
			p5150Info->iTempVideoChannel = p5150Info->iPreVideoChannel;
		}


		DBGI((TEXT("\r\n 5150 BACK status bBackDetectEnable:%d, bBackActiveNow:%d,video:%d,iTempVideoChannel:%d,iCurVideoChannel:%d\r\n"),
			p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable,
			p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow,
			p5150Info->bBackVideoOn,p5150Info->iTempVideoChannel,p5150Info->iCurVideoChannel));


		if (p5150Info->iCurVideoChannel != p5150Info->iTempVideoChannel)//通道
		{
			p5150Info->iCurVideoChannel = p5150Info->iTempVideoChannel;
			p5150Info->bVideoFirstSelectChannel = TRUE;
			DBGI((TEXT("\r\n 5150 BACK status channel:%d\r\n"),p5150Info->iCurVideoChannel));
			controlVideoChannel(p5150Info,p5150Info->iCurVideoChannel);
		}

		//if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable
		//	&& p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
		//{
		//}
		//else if (p5150Info->iCurVideoChannel == AUX || p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eAudioInput)
		//{
		//	if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAUXHaveVideo != p5150Info->bIsHaveAUXVideoSignal)
		//	{
		//		p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAUXHaveVideo = p5150Info->bIsHaveAUXVideoSignal;
		//		IpcStartEvent(EVENT_GLOBAL_AUX_CHECK_RETURN_ID);
		//	}
		//}

		if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)
		{
			if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable
				&& p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn)//倒车且有视频
			{
				p5150Info->bPreVideoBlack = FALSE;
			}
			else
			{
				p5150Info->bPreVideoBlack = TRUE;
			}
		}
		else
		{
			p5150Info->bPreVideoBlack = FALSE;
		}

		if (p5150Info->bBackVideoDealWINAction)
		{
			if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable)
			{
				if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
				{
					if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn != p5150Info->bBackVideoOn)
					{
						p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn = p5150Info->bBackVideoOn;
						controlVideoBackSigned(p5150Info,p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn);
						DBGI((TEXT("\r\nBACK Video Return:%d"),p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn));
					}
				}
				else
				{
					p5150Info->bBackVideoOn = FALSE;
					p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn = FALSE;
				}

				if (p5150Info->bBackVideoDealWINAction)
				{
					//IpcStartEvent(EVENT_GLOBAL_BACK_LOW_VOLUME_ID);
					IpcStartEvent(EVENT_GLOBAL_BACKDETECT_RETURN_ID);
				}
			}
			else
			{
				p5150Info->bBackVideoOn = FALSE;
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn = FALSE;
			}

			p5150Info->bBackVideoDealWINAction = FALSE;
			IpcStartEvent(EVENT_GOBAL_BACKACTION_PWM);
		}

		setVideoColor(p5150Info);
		if (p5150Info->bSocketConnecting && p5150Info->bSetColorEnable)
		{
			if (p5150Info->iVideoParaContrast != p5150Info->iTempVideoParaContrast)//对比度
			{
				p5150Info->iVideoParaContrast = p5150Info->iTempVideoParaContrast;
				p5150Info->cVinConfig.contrast = sContrast_N[p5150Info->iVideoParaContrast];
				actualChangeVideoInput(p5150Info,&p5150Info->cVinConfig,SET_CONTRAST);
			}
			if (p5150Info->iVideoParaBrightness != p5150Info->iTempVideoParaBrightness)//亮度
			{
				p5150Info->iVideoParaBrightness = p5150Info->iTempVideoParaBrightness;
				p5150Info->cVinConfig.brightness = sBrightness_N[p5150Info->iVideoParaBrightness];
				actualChangeVideoInput(p5150Info,&p5150Info->cVinConfig,SET_BRIGHTNESS);
			}

			if (p5150Info->iVideoParaColor != p5150Info->iTempVideoParaColor)//色彩
			{
				p5150Info->iVideoParaColor = p5150Info->iTempVideoParaColor;
				p5150Info->cVinConfig.color = sColor_N[p5150Info->iVideoParaColor];
				actualChangeVideoInput(p5150Info,&p5150Info->cVinConfig,SET_COLORE);
			}
			if (p5150Info->iVideoParaHue != p5150Info->iTempVideoParaHue)//色调
			{
				p5150Info->iVideoParaHue = p5150Info->iTempVideoParaHue;
				p5150Info->cVinConfig.hue = sHue_N[p5150Info->iVideoParaHue];
				actualChangeVideoInput(p5150Info,&p5150Info->cVinConfig,SET_HUE);
			}
		}

		if (!p5150Info->bSocketConnecting)
		{
			DBGE((TEXT("\r\n5150 socket can't recv msg(ensure the socket is connect and send init commd)\r\n")));
		}
	}

	p5150Info->FlyMainThreadHandle = NULL;
	CloseHandle(p5150Info->hDispatchMainThreadEvent);
	DBGI((TEXT("\r\nFlyAudio 5150 ThreadMain exit")));
	return 0;
}

void Fly5150Enable(P_FLY_5150_INFO p5150Info,BOOL bEnable)
{
	DWORD dwThreadID;
	if (bEnable)
	{
		if (p5150Info->bPower)
		{
			return;
		}
		p5150Info->bPower = TRUE;

		p5150Info->bKillDispatchFlyMainThread = FALSE;
		p5150Info->hDispatchMainThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
		p5150Info->FlyMainThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadMain, //线程的全局函数
			p5150Info, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGI((TEXT("\r\n5150 ThreadMain thread!%x"),dwThreadID));
		if (NULL == p5150Info->FlyMainThreadHandle)
		{
			p5150Info->bKillDispatchFlyMainThread = TRUE;
			return;
		}
		SetEvent(p5150Info->hDispatchMainThreadEvent);


		//倒车是否有视频事件，由江德鹏发出
		p5150Info->bKillDispatchFlyBackCarThread = FALSE;
		p5150Info->hDispatchBackCarThreadEvent = CreateEvent(NULL, FALSE, FALSE, FLY_EVENT_NAME_CAMERA_SIG);	
		p5150Info->FlyBackCarThreadHandle = CreateThread( (LPSECURITY_ATTRIBUTES) NULL, 0,
			(LPTHREAD_START_ROUTINE)BackCarThread, p5150Info, 0, &dwThreadID );
		SetThreadPriority(p5150Info->FlyBackCarThreadHandle,THREAD_PRIORITY_TIME_CRITICAL);
		DBGI((TEXT("\r\n5150 back car thread!%x"),dwThreadID));
		if (NULL == p5150Info->FlyBackCarThreadHandle)
		{
			p5150Info->bKillDispatchFlyBackCarThread = TRUE;
			return;
		}
		//SetEvent(p5150Info->hDispatchBackCarThreadEvent);


		//AUX是否有视频事件，由江德鹏发出
		p5150Info->bKillDispatchFlyAuxListerThread = FALSE;
		p5150Info->hDispatchAuxListerThreadEvent = CreateEvent(NULL, FALSE, TRUE, EVENT_FLY_AUX_ON_OFF);	
		p5150Info->FlyAuxThreadHandle = CreateThread( (LPSECURITY_ATTRIBUTES) NULL, 0,
			(LPTHREAD_START_ROUTINE)AuxListerThread, p5150Info, 0, &dwThreadID );
		if (NULL == p5150Info->FlyAuxThreadHandle)
		{
			p5150Info->bKillDispatchFlyAuxListerThread = TRUE;
			return;
		}
		SetEvent(p5150Info->hDispatchAuxListerThreadEvent);

	}
	else
	{
		if (!p5150Info->bPower)
		{
			return;
		}
		p5150Info->bPower = FALSE;
		p5150Info->bPowerUp = FALSE;

		p5150Info->bKillDispatchFlyMainThread = TRUE;
		SetEvent(p5150Info->hDispatchMainThreadEvent);

		while (p5150Info->FlyMainThreadHandle)
		{
			SetEvent(p5150Info->hDispatchMainThreadEvent);
			Sleep(10);
		}
	}
}

static VOID WinCECommandProcessor(P_FLY_5150_INFO p5150Info,BYTE *buff,UINT len)
{
	switch (buff[0])
	{
	case 0x01:
		if (0x01 == buff[1])
		{
			returnDisplayValue(p5150Info,E_C_COLOR,p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaColor);
			returnDisplayValue(p5150Info,E_C_HUE,p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaHue);
			returnDisplayValue(p5150Info,E_C_CONTRAST,p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaContrast);
			returnDisplayValue(p5150Info,E_C_BRIGHTNESS,p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaBrightness);
			returnVideoPowerMode(p5150Info,1);
			returnVideoWorkMode(p5150Info,1);
		}
		else
		{
			returnVideoPowerMode(p5150Info,0);
		}
		break;
	case 0x10:
		if (0x00 == buff[1] || 0x80 == buff[1])
		{
			p5150Info->bPreVideoEnable = FALSE;
			p5150Info->iPreVideoChannel = buff[1];
		}
		else
		{
			p5150Info->bPreVideoEnable = TRUE;
			p5150Info->iPreVideoChannel = buff[1];
		}

		if (AUX == p5150Info->iPreVideoChannel && !p5150Info->bAuxFirstReturnSignal)
		{
			p5150Info->bAuxFirstReturnSignal = TRUE;
		}

		DBGD((TEXT("\r\nFLY5150 main input:%d"),p5150Info->iPreVideoChannel));
		p5150Info->bVideoFirstSelectChannel = TRUE;
		p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eVideoInput = buff[1];
		if (p5150Info->bPowerUp)
		{
			SetEvent(p5150Info->hDispatchMainThreadEvent);
			ReturnToUser(buff,len);
		}
		break;

	case 0x11:
		switch (buff[1])
		{
		case 0x01:
			if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaColor < COLOR_STEP_COUNT - 1)
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaColor++;
			} 
			else
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaColor = COLOR_STEP_COUNT - 1;
			}
			if (p5150Info->bPowerUp)
			{
				SetEvent(p5150Info->hDispatchMainThreadEvent);
			}
			returnDisplayValue(p5150Info,E_C_COLOR,p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaColor);
			break;
		case 0x02:
			if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaColor >= COLOR_STEP_COUNT)
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaColor = COLOR_STEP_COUNT - 1;
			} 
			else if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaColor)
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaColor--;
			}
			else
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaColor = 0;
			}
			if (p5150Info->bPowerUp)
			{
				SetEvent(p5150Info->hDispatchMainThreadEvent);
			}
			returnDisplayValue(p5150Info,E_C_COLOR,p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaColor);
			break;
		case 0x03:
			if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaHue < COLOR_STEP_COUNT - 1)
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaHue++;
			} 
			else
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaHue = COLOR_STEP_COUNT - 1;
			}
			if (p5150Info->bPowerUp)
			{
				SetEvent(p5150Info->hDispatchMainThreadEvent);
			}
			returnDisplayValue(p5150Info,E_C_HUE,p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaHue);
			break;
		case 0x04:
			if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaHue >= COLOR_STEP_COUNT)
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaHue = COLOR_STEP_COUNT - 1;
			} 
			else if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaHue)
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaHue--;
			}
			else
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaHue = 0;
			}
			if (p5150Info->bPowerUp)
			{
				SetEvent(p5150Info->hDispatchMainThreadEvent);
			}
			returnDisplayValue(p5150Info,E_C_HUE,p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaHue);
			break;
		case 0x05:
			if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaContrast < COLOR_STEP_COUNT - 1)
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaContrast++;
			} 
			else
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaContrast = COLOR_STEP_COUNT - 1;
			}
			if (p5150Info->bPowerUp)
			{
				SetEvent(p5150Info->hDispatchMainThreadEvent);
			}
			returnDisplayValue(p5150Info,E_C_CONTRAST,p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaContrast);
			break;
		case 0x06:
			if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaContrast >= COLOR_STEP_COUNT)
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaContrast = COLOR_STEP_COUNT - 1;
			} 
			else if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaContrast)
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaContrast--;
			}
			else
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaContrast = 0;
			}
			if (p5150Info->bPowerUp)
			{
				SetEvent(p5150Info->hDispatchMainThreadEvent);
			}
			returnDisplayValue(p5150Info,E_C_CONTRAST,p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaContrast);
			break;
		case 0x07:
			if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaBrightness < COLOR_STEP_COUNT - 1)
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaBrightness++;
			} 
			else
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaBrightness = COLOR_STEP_COUNT - 1;
			}
			if (p5150Info->bPowerUp)
			{
				SetEvent(p5150Info->hDispatchMainThreadEvent);
			}
			returnDisplayValue(p5150Info,E_C_BRIGHTNESS,p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaBrightness);
			break;
		case 0x08:
			if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaBrightness >= COLOR_STEP_COUNT)
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaBrightness = COLOR_STEP_COUNT - 1;
			} 
			else if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaBrightness)
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaBrightness--;
			}
			else
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaBrightness = 0;
			}
			if (p5150Info->bPowerUp)
			{
				SetEvent(p5150Info->hDispatchMainThreadEvent);
			}
			returnDisplayValue(p5150Info,E_C_BRIGHTNESS,p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaBrightness);
			break;
		default:
			break;
		}
		break;
	case 0xF0:
		p5150Info->cVinConfig.alpha = 128;
		actualChangeVideoInput(p5150Info,&p5150Info->cVinConfig,SET_ALPHA);
		break;
	case 0xFF:
		if (0x01 == buff[1])
		{
			FVO_PowerUp((DWORD)p5150Info);
		} 
		else if (0x00 == buff[1])
		{
			FVO_PowerDown((DWORD)p5150Info);
		}
		break;
	default:
		break;
	}
}

static void dealVideoData(P_FLY_5150_INFO p5150Info, BYTE *buf,UINT16 len)
{
	switch (buf[0])
	{
	case 0x01:
		SetEvent(p5150Info->hDispatchMainThreadEvent);
		break;


	case 0x10:
		break;

	case 0x20:
		if (buf[1] == AUX)
		{
			if (p5150Info->bAuxFirstReturnSignal)
			{
				//每切一次一定要返回
				p5150Info->bAuxFirstReturnSignal = FALSE;
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAUXHaveVideo = !buf[2];
			}
			
			if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAUXHaveVideo != buf[2])
			{
				p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAUXHaveVideo = buf[2];
				p5150Info->bIsHaveAUXVideoSignal = buf[2];
				IpcStartEvent(EVENT_GLOBAL_AUX_CHECK_RETURN_ID);
			}
		}
		break;

	default:
		break;
	}
}

static void powerNormalInit(P_FLY_5150_INFO p5150Info)
{
	p5150Info->buffToUserHx = 0;
	p5150Info->buffToUserLx = 0;

	p5150Info->bBackChecking = FALSE;
	p5150Info->bBackVideoOn = FALSE;

	p5150Info->bBackVideoDealWINAction = FALSE;
	p5150Info->bBackCheckNeedFirstReturn = TRUE;
	p5150Info->bBackCheckReturn = FALSE;
	p5150Info->iBackStartCheckingTime = 0;
	p5150Info->iBackStartReturnTime = 0;
	p5150Info->iBackLPCStartCheckingTime = GetTickCount();

	p5150Info->iPreVideoChannel = 0;
	p5150Info->iCurVideoChannel = 0;
	p5150Info->iChannelChangeTime = GetTickCount();
	p5150Info->bPreVideoEnable = FALSE;
	p5150Info->bCurVideoEnable = FALSE;
	p5150Info->bPreVideoBlack = FALSE;
	p5150Info->bCurVideoBlack = FALSE;
	
	p5150Info->bAuxVideoOn = FALSE;

	p5150Info->iVideoParaColor = p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaColor;
	p5150Info->iVideoParaHue = p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaHue;
	p5150Info->iVideoParaContrast = p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaContrast;
	p5150Info->iVideoParaBrightness = p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaBrightness;

	p5150Info->cVinConfig.enable = FALSE;
	p5150Info->cVinConfig.black = FALSE;
	p5150Info->cVinConfig.channel = 0;
	p5150Info->cVinConfig.width = 800;
	p5150Info->cVinConfig.height = 480;
	p5150Info->cVinConfig.pos_x = 0;
	p5150Info->cVinConfig.pos_y = 0;
	p5150Info->cVinConfig.brightness = sBrightness_N[p5150Info->iVideoParaBrightness];
	p5150Info->cVinConfig.color      = sColor_N[p5150Info->iVideoParaColor];
	p5150Info->cVinConfig.hue        = sHue_N[p5150Info->iVideoParaHue];
	p5150Info->cVinConfig.contrast   = sContrast_N[p5150Info->iVideoParaContrast];
	p5150Info->cVinConfig.alpha = 255;
	//actualChangeVideoInput(p5150Info,&p5150Info->cVinConfig,SET_ALL);

	p5150Info->bIsHaveAUXVideoSignal = FALSE;
	p5150Info->bAuxFirstReturnSignal = FALSE;

	DBGI((TEXT("\r\nFlyAudio 5150 Normal Init!")));
}

static void powerOnFirstInit(P_FLY_5150_INFO p5150Info)
{
	p5150Info->bOpen = FALSE;
	p5150Info->bPower = FALSE;
	p5150Info->bPowerUp = FALSE;
	p5150Info->bSpecialPower = FALSE;

	p5150Info->bKillDispatchFlyMainThread = TRUE;
	p5150Info->FlyMainThreadHandle = NULL;
	p5150Info->hDispatchMainThreadEvent = NULL;

	p5150Info->bKillDispatchFlyBackCarThread = TRUE;
	p5150Info->FlyBackCarThreadHandle = NULL;
	p5150Info->hDispatchBackCarThreadEvent = NULL;
	p5150Info->bBackCarValue = FALSE;
	InitializeCriticalSection(&p5150Info->cDealBackcarThreadSection);
}

static void powerOnSpecialEnable(P_FLY_5150_INFO p5150Info,BOOL bOn)
{
	if (bOn)
	{
		if (p5150Info->bSpecialPower)
		{
			return;
		}
		p5150Info->bSpecialPower = TRUE;
	}
	else
	{
		if (!p5150Info->bSpecialPower)
		{
			return;
		}
		p5150Info->bSpecialPower = FALSE;
	}
}
void IpcRecv(UINT32 iEvent)
{
	P_FLY_5150_INFO	p5150Info = (P_FLY_5150_INFO)gp5150Info;
	
	DBGD((TEXT("\r\nFLY5150 Recv IPC iEvent:%d\r\n"),iEvent));

	switch (iEvent)
	{
	case EVENT_GLOBAL_AUX_CHECK_START_ID:
		IpcClearEvent(EVENT_GLOBAL_AUX_CHECK_START_ID);
		p5150Info->iCurVideoChannel = AUX;
		p5150Info->bAuxFirstReturnSignal = TRUE;
		controlVideoChannel(p5150Info,p5150Info->iCurVideoChannel);
		break;

	case EVENT_GLOBAL_BACK_EXIT_DEPENDON_LPC_ID:
		IpcClearEvent(EVENT_GLOBAL_BACK_EXIT_DEPENDON_LPC_ID);
		if (p5150Info->hDispatchBackCarThreadEvent)
		{
			//退出倒车时优先处理
			SetEventData(gp5150Info->hDispatchBackCarThreadEvent,0);
			SetEvent(p5150Info->hDispatchBackCarThreadEvent);
			DBGD((TEXT("\r\nLpc exit back")));
		}
		break;

	case EVENT_GLOBAL_BACKDETECT_CHANGE_ID:
		IpcClearEvent(EVENT_GLOBAL_BACKDETECT_CHANGE_ID);
		break;

	default:
		SetEvent(p5150Info->hDispatchMainThreadEvent);
		break;
	}
}
void SockRecv(BYTE *buf, UINT16 len)
{
	UINT16 i=0;
	P_FLY_5150_INFO	p5150Info = (P_FLY_5150_INFO)gp5150Info;

	if (len <= 0)
		return;

	DBGD((TEXT("\r\n FLY 5150 socket recv %d bytes:"),len-1));
	for (i=0; i<len-1; i++)
		DBGD((TEXT("%02X "), buf[i]));

	if (!p5150Info->bSocketConnecting)
		p5150Info->bSocketConnecting  = TRUE;

	dealVideoData(p5150Info,buf,len-1);
}


HANDLE FVO_Init(DWORD dwContext)
{
	P_FLY_5150_INFO p5150Info;

	p5150Info = (P_FLY_5150_INFO)LocalAlloc(LPTR, sizeof(FLY_5150_INFO));
	if (!p5150Info)
	{
		return NULL;
	}
	gp5150Info = p5150Info;

	p5150Info->pFlyShmGlobalInfo = CreateShm(VIDEO_MODULE,IpcRecv);
	if (NULL == p5150Info->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio 5150 Init create shm err\r\n")));
		return NULL;
	}

	if (!GetDllAddrTable())
	{
		DBGE((TEXT("FlyAudio 5150  GetDllAddrTable err\r\n")));
		return NULL;
	}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio 5150 Open create user buf err\r\n")));
		return NULL;
	}
	if(!CreateServerSocket(SockRecv, TCP_PORT_VIDEO))
	{
		DBGE((TEXT("FlyAudio 5150 Open create server socket err\r\n")));
		return NULL;
	}


	if (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalStructSize != sizeof(struct shm))
	{
		p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[0] = 'F';
		p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[1] = 'V';
		p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[2] = 'O';
	}

	//参数初始化
	powerOnFirstInit(p5150Info);
	powerNormalInit(p5150Info);
	powerOnSpecialEnable(p5150Info,TRUE);

	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalVideoInfo.bInit = TRUE;
	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompYear = year;
	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompMon = months;
	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompDay = day;
	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompHour = hours;
	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompMin = minutes;
	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompSec = seconds;

	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable = TRUE;
	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActioning = TRUE;
	Fly5150Enable(p5150Info,TRUE);

	DBGI((TEXT("\r\nFlyAudio 5150 Init Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return (HANDLE)p5150Info;
}

BOOL
FVO_Deinit(DWORD hDeviceContext)
{
	P_FLY_5150_INFO	p5150Info = (P_FLY_5150_INFO)hDeviceContext;

	Fly5150Enable(p5150Info,FALSE);

	//全局
	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalVideoInfo.bInit = FALSE;
	powerOnSpecialEnable(p5150Info,FALSE);
	DBGI((TEXT("FlyAudio 5150 DeInit\r\n")));

	FreeShm();
	FreeSocketServer();
	FreeUserBuff();
	LocalFree(p5150Info);
	return TRUE;
}

DWORD
FVO_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	P_FLY_5150_INFO	p5150Info = (P_FLY_5150_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;

	if (p5150Info->bOpen)
	{
		return NULL;
	}
	p5150Info->bOpen = TRUE;

	//全局
	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalVideoInfo.bOpen = TRUE;
	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeVideoSleep = FALSE;
	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeVideoSleep = FALSE;

	DBGI((TEXT("\r\nFlyAudio 5150 Open Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return returnWhat;
}

BOOL
FVO_Close(DWORD hDeviceContext)
{
	P_FLY_5150_INFO	p5150Info = (P_FLY_5150_INFO)hDeviceContext;

	//全局
	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalVideoInfo.bOpen = FALSE;
	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeVideoSleep = TRUE;
	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeVideoSleep = TRUE;

	p5150Info->bOpen = FALSE;

	DBGI((TEXT("FlyAudio 5150 Close\r\n")));
	return TRUE;
}

VOID
FVO_PowerUp(DWORD hDeviceContext)
{
	P_FLY_5150_INFO	p5150Info = (P_FLY_5150_INFO)hDeviceContext;

	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeVideoSleep = FALSE;
	p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeVideoSleep = FALSE;
	SetEvent(p5150Info->hDispatchMainThreadEvent);

	DBGI((TEXT("FlyAudio 5150 PowerUp\r\n")));
}

VOID
FVO_PowerDown(DWORD hDeviceContext)
{
	P_FLY_5150_INFO	p5150Info = (P_FLY_5150_INFO)hDeviceContext;

	DBGI((TEXT("FlyAudio 5150 PowerDown\r\n")));
}

DWORD
FVO_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	P_FLY_5150_INFO	p5150Info = (P_FLY_5150_INFO)hOpenContext;

	UINT dwRead = 0,i;

	BYTE *buf = (BYTE *)pBuffer;
	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio 5150 return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}

	return dwRead;
}

DWORD
FVO_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	P_FLY_5150_INFO	p5150Info = (P_FLY_5150_INFO)hOpenContext;
	DBGI((TEXT("\r\nFlyAudio 5150 Write %d bytes:"),NumberOfBytes));
	for(UINT i = 0;i < NumberOfBytes;i++)
	{
		DBGI((TEXT(" %X"),*((BYTE *)pSourceBytes + i)));
	}


	if(NumberOfBytes)
	{
		WinCECommandProcessor(p5150Info,(((BYTE *)pSourceBytes)+3),((BYTE *)pSourceBytes)[2]);
	}

	return NULL;
}

DWORD
FVO_Seek(DWORD hOpenContext, LONG Amount, DWORD  Type)
{
	return NULL;
}

BOOL
FVO_IOControl(DWORD hOpenContext,
			  DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn,
			  PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
	P_FLY_5150_INFO	p5150Info = (P_FLY_5150_INFO)hOpenContext;
	BOOL bRes = TRUE;

	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	DBG2(DBGI((TEXT("\r\nFlyAudio 5150 IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)"),
	//		hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	DBGD((TEXT("\r\nFlyAudio 5150 IOControl Set IOCTL_SERIAL_SET_WAIT_MASK")));
	//		break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	DBGD((TEXT("\r\nFlyAudio 5150 IOControl Set IOCTL_SERIAL_WAIT_ON_MASK Enter")));
	//		if (p5150Info->bOpen)
	//		{
	//			WaitForSingleObject(p5150Info->hBuffToUserDataEvent,INFINITE);
	//		} 
	//		else
	//		{
	//			WaitForSingleObject(p5150Info->hBuffToUserDataEvent,0);
	//		}
	//		if ((dwLenOut < sizeof(DWORD)) || (NULL == pBufOut) ||
	//			(NULL == pdwActualOut))
	//		{
	//			bRes = FALSE;
	//			break;
	//		}
	//		*(DWORD *)pBufOut = EV_RXCHAR;
	//		*pdwActualOut = sizeof(DWORD);
	//		DBGD((TEXT("\r\nFlyAudio 5150 IOControl Set IOCTL_SERIAL_WAIT_ON_MASK Exit")));
	//			break;
	//default :
	//	break;
	//}

	return TRUE;
}

BOOL
DllEntry(
		 HINSTANCE hinstDll,
		 DWORD dwReason,
		 LPVOID lpReserved
		 )
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DEBUGMSG(1, (TEXT("Attach in Fly5150 DllEntry\n")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}

	if (dwReason == DLL_PROCESS_DETACH)
	{
		DEBUGMSG(1, (TEXT("Dettach in Fly5150 DllEntry\n")));
	}

	DEBUGMSG(1, (TEXT("Leaving Fly5150 DllEntry\n")));

	return (TRUE);
}
