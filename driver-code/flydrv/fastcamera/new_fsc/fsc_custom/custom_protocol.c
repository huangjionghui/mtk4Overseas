
#include<windows.h>

#include"BCLib.h"
#include "custom_protocol.h"
#include "backcar_msg.h"

static UINT16  *g_DateBuffer = NULL;

/**************************/
#define DGB_BACKCAR 0

#define GPIO_SET_INPUT    1
#define GPIO_SET_OUTPUT   2
#define GPIO_READ         3

#define BACKCAR_CHECK_ON   0x01
#define BACKCAR_CHECK_OFF 0x00

DWORD GpioRead();
void GpioInit();
HANDLE hFlySocMtk;

/***************************///modify by jdp for backup 20120802
BOOL  SendAck(UINT32 u4Satus)
{
	TransferStatus  TS;
	TS.u4Status = u4Satus;
	BCUartWriteBlockData((BYTE *)&TS,sizeof(TransferStatus));

	return TRUE;

}




BOOL OnDistance(UINT32 u4Dis)
{
    

	ShowDistance(u4Dis);

    return TRUE;

}

#define BACK_CAR_EVENT_CONTROL 0
#define BACK_CAR_EVENT_CONTROL_NAME L"BACK_CAR_EVENT_CONTROL_NAME"
enum
{
    BACK_CAR_CTRL_START = 0,
    BACK_CAR_CTRL_STOP,
    BACK_CAR_CTRL_DATA,
};


/******************************************************************/
void GpioInit()
{
	hFlySocMtk = CreateFile(_T("FLY1:"),GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(hFlySocMtk==INVALID_HANDLE_VALUE)
	{
		RETAILMSG(TRUE, (TEXT("[      GpioInit ] CreateFile FLY error\r\n")));
	}

}

DWORD GpioRead()
{
	char Outbuf[4];
	if(DeviceIoControl(hFlySocMtk,GPIO_READ,NULL,NULL,Outbuf,NULL,NULL,NULL) == 0)
	{	
		RETAILMSG(TRUE, (TEXT("[      GpioRead ] GpioRead fail\r\n")));			
	}	
	return Outbuf[0];
}

HANDLE    g_hEventBackSig;
BOOL g_bBackCarFlagEnable = FALSE;

#define  FLY_CAMERA_SIG_ON 1
#define  FLY_CAMERA_SIG_OFF 0
#define  FLY_BACKCAR_STATE_ON	1<<1	
#define  FLY_BACKCAR_STATE_OFF  0<<1

#define  FLY_BACKCAR_ACTION_ON   1<<4
#define  FLY_BACKCAR_ACTION_OFF  0<<4

#define TIMES_DELAY    100

#define FLY_SYN_BACK_INIT_FINISH_NAME L"FLY_SYN_BACK_INIT_FINISH_NAME"

void SendBackStatusToDriver(UINT32 iData)
{
	SetEventData(g_hEventBackSig,iData);
	SetEvent(g_hEventBackSig);
}

#if 1
static DWORD CameraStateCfg = 1,dwCurCameraStateCfg = 1;
#else
DWORD CameraStateCfg,dwCurCameraStateCfg;
#endif

HANDLE g_hEventBackcarEnabel;


DWORD WINAPI BCCustomEntry()
{
	HANDLE g_hHandle;
	DWORD dwDis,dwARM2BackCarStatus;
	int iBackGpioStatus;
	BOOL bFrist =FALSE;
	BOOL bBackFirstStatus = TRUE;

	GpioInit();
	CeSetThreadPriority( GetCurrentThread(), 245/*120*/ );
	g_DateBuffer = (UINT16 *)malloc(50*1024);
	if(g_DateBuffer == NULL)
	{
		RETAILMSG(TRUE, (TEXT("[BackCarAPP] ALLOC MEM FAILED\r\n")));
		return -1;
	}
	dwARM2BackCarStatus = BCWaitInitFin(&dwDis);
  
	g_hEventBackSig = OpenEvent(EVENT_ALL_ACCESS , FALSE , TEXT("FLY_BACK_CAMERA_ON_OFF"));//OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT("SYSTEM/BackCarAppReady"));
	if(g_hEventBackSig==NULL)
	{
		 g_hEventBackSig = CreateEvent(NULL, FALSE, FALSE, TEXT("FLY_BACK_CAMERA_ON_OFF"));
	}
	
	g_hEventBackcarEnabel= CreateEvent(NULL, FALSE, TRUE, TEXT("FLY_BACKCAR_ENABLE_HANDLE"));


	g_hHandle = CreateEvent(NULL,FALSE,FALSE,FLY_SYN_BACK_INIT_FINISH_NAME);
	SetEvent(g_hHandle);
	CloseHandle(g_hHandle);

	RETAILMSG(1 , (TEXT("\r\n[fastcamera ]:BCCustomEntry Check GPIO Start.....:\r\n")));


	if(ARM2_STATUS_BACKING_CAR == dwARM2BackCarStatus)//ARM2倒车
	{
		//先通知DRIVER有倒车，释放AUX/CDC视频源
		SendBackStatusToDriver(FLY_BACKCAR_ACTION_ON);
		g_bBackCarFlagEnable = TRUE;
		Sleep(TIMES_DELAY);
		BCAllocateResource();
		SendBackStatusToDriver(2|dwDis);
		RETAILMSG(1 , (TEXT("\r\n[fastcamera ]:Back ARM2 Action:1,Video:%d\r\n"),dwDis));
	}

	while(TRUE)
	{
		iBackGpioStatus = GpioRead();
		while (TRUE)
		{
			WaitForSingleObject(g_hEventBackcarEnabel, 50);

			if(GpioRead()  != iBackGpioStatus)
			{
				iBackGpioStatus = GpioRead();
				RETAILMSG(TRUE, (TEXT("\r\n Back car gpio read status:%d\r\n"),iBackGpioStatus));
				break;
			}

			if (dwCurCameraStateCfg != CameraStateCfg)
			{
				dwCurCameraStateCfg = CameraStateCfg;
				break;
			}
		}

		if (ARM2_STATUS_BACKING_CAR == dwARM2BackCarStatus)//之前是ARM2倒车
		{
			if (iBackGpioStatus == 0)//退出倒车
			{
				dwARM2BackCarStatus = ARM2_STATUS_NO_BACK_CAR;
				g_bBackCarFlagEnable = FALSE;
				BCReleaseResource();
				Sleep(TIMES_DELAY);
				SendBackStatusToDriver(0);//退出倒车
				RETAILMSG(1 , (TEXT("\r\n[fastcamera ]:Back ARM2 Action:0,Video:0\r\n")));
			}
		}
		else //正常倒车
		{
			if (!dwCurCameraStateCfg)
			{
				if (g_bBackCarFlagEnable)
				{
					g_bBackCarFlagEnable = FALSE;
					BCReleaseResource();
				}	
				SendBackStatusToDriver(0);
				RETAILMSG(1 , (TEXT("\r\n[Back-custom_protocol.c]:Back enable status:%d\r\n"),dwCurCameraStateCfg));
				continue;
			}

			if (iBackGpioStatus == 1 && !g_bBackCarFlagEnable){	//进入倒车
				//先通知DRIVER有倒车，释放(AUX/CDC)视频源
				SendBackStatusToDriver(FLY_BACKCAR_ACTION_ON);
				RETAILMSG(1 , (TEXT("\r\n[fastcamera ]:Back Wince Starting.....\r\n")));
				g_bBackCarFlagEnable = TRUE;	
				Sleep(TIMES_DELAY);
				BCAllocateResource();			

			}
			else if (iBackGpioStatus == 0 && g_bBackCarFlagEnable)
			{
				RETAILMSG(1 , (TEXT("\r\n[Back-custom_protocol.c]:Back Wince Action:0,Video:0\r\n")));
				g_bBackCarFlagEnable = FALSE;
				BCReleaseResource();
				Sleep(TIMES_DELAY);
				SendBackStatusToDriver(0);				
			}
		}
	}

	return 0;
}
