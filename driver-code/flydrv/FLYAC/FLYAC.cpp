// -----------------------------------------------------------------------------
// File Name    : FLYAC.cpp
// Title        : FLYAC Driver
// Author       : fengdaohai - Copyright (C) 2011
// Created      : 2011-10-19  
// Version      : 0.01
// Target MCU   : WinCE Driver
// -----------------------------------------------------------------------------

#define GLOBAL_COMM	1

#include "flyhwlib.h"
#include "flyuserlib.h"
#include "flysocketlib.h"
#include "FLYAC.h"


VOID FAC_PowerUp(DWORD hDeviceContext);
VOID FAC_PowerDown(DWORD hDeviceContext);

BYTE Ltemp_H,Ltemp_L,Rtemp_H,Rtemp_L;
BYTE Speed,AC,Auto,ModeUp,ModeFront,ModeDown,Max,Rear,Vane,Off,Dual,LeftHeat,RightHeat;
BYTE AC_MAX,Rear_Lock;
BYTE AcMode;
P_FLY_AC_INFO gpFlYAcInfo = NULL;

static void returnFACPowerMode(P_FLY_AC_INFO pFlYAcInfo,BOOL bPower)
{
	BYTE buff[] = {0x01,0x00};
	if (bPower)
	{
		buff[1] = 0x01;
	}
	else
	{
		buff[1] = 0x00;
	}

	ReturnToUser(buff,2);
}

static void returnFACbInit(P_FLY_AC_INFO pFlYAcInfo,BOOL bInit)
{
	BYTE buff[] = {0x02,0x00};
	if (bInit)
	{
		buff[1] = 0x01;
	}
	else
	{
		buff[1] = 0x00;
	}

	ReturnToUser(buff,2);
}

static void FACWriteFile(P_FLY_AC_INFO pFlYAcInfo,BYTE *p,UINT length)
{
	UINT i;
	
	if (pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.bInit)
	{
		if (FALSE == WriteMsgQueue(pFlYAcInfo->hMsgQueueFromAcCreate, p, length, 0, 0))
		{
			DWORD dwError = GetLastError();
			DBGE((TEXT("\r\nFlyAudio FAC Write MsgQueue Error!0x%x"),dwError));
		}
	}
	else
	{
		DBGE((TEXT("\r\nFlyAudio FAC Write MsgQueue Not Init")));
		for (i = 0;i < length;i++)
		{
			DBGE((TEXT(" %x"),p[i]));
		}
	}
}

static DWORD WINAPI ThreadInter(LPVOID pContext)
{
	P_FLY_AC_INFO pFlYAcInfo = (P_FLY_AC_INFO)pContext;
	ULONG WaitReturn;

	while (!pFlYAcInfo->bKillDispatchFlyInterThread)
	{
		WaitReturn = WaitForSingleObject(pFlYAcInfo->hDispatchInterThreadEvent, INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nFlyAudio FAC hDispatchInterThreadEvent Error")));
		}

	}
	pFlYAcInfo->FLyInterThreadHandle = NULL;
	DBGD((TEXT("\r\nFlyAudio FAC ThreadInter exit")));
	return 0;
}

static DWORD WINAPI ThreadMain(LPVOID pContext)
{
	P_FLY_AC_INFO pFlYAcInfo = (P_FLY_AC_INFO)pContext;
	ULONG WaitReturn;

	pFlYAcInfo->bPowerUp = TRUE;

	while (!pFlYAcInfo->bKillDispatchFlyMainThread)
	{
			WaitReturn = WaitForSingleObject(pFlYAcInfo->hDispatchMainThreadEvent, INFINITE);
			if (WAIT_FAILED == WaitReturn)
			{
				DBGE((TEXT("\r\nFlyAudio FAC hDispatchMainThreadEvent Error")));
			}
	}

	while (pFlYAcInfo->FLyInterThreadHandle)
	{
		SetEvent(pFlYAcInfo->hDispatchInterThreadEvent);
		Sleep(10);
	}
	CloseHandle(pFlYAcInfo->hDispatchInterThreadEvent);

	pFlYAcInfo->FlyMainThreadHandle = NULL;
	CloseHandle(pFlYAcInfo->hDispatchMainThreadEvent);
	DBGD((TEXT("\r\nFlyAudio FAC ThreadMain exit")));
		return 0;
}


static void ReturnLTempToWince(P_FLY_AC_INFO pFlYAcInfo,BYTE ltemp_H,BYTE ltemp_L)
{
	BYTE buff[3] = {0x10};
	
	buff[1] = ltemp_H;
	buff[2] = ltemp_L;
	
	ReturnToUser(buff,3);	
}

static void ReturnRTempToWince(P_FLY_AC_INFO pFlYAcInfo,BYTE rtemp_H,BYTE rtemp_L)
{
	BYTE buff[3] = {0x11};
	
	buff[1] = rtemp_H;
	buff[2] = rtemp_L;
	
	ReturnToUser(buff,3);		
}

static void ReturnSpeedToWince(P_FLY_AC_INFO pFlYAcInfo,BYTE speed)
{
	BYTE buff[2] = {0x12};
	
	buff[1] = speed;
	
	ReturnToUser(buff,2);		
}
static void ReturnAcToWince(P_FLY_AC_INFO pFlYAcInfo,BYTE ac)
{
	BYTE buff[2] = {0x13};
	
	buff[1] = ac;
	
	ReturnToUser(buff,2);		
}
static void ReturnAutoToWince(P_FLY_AC_INFO pFlYAcInfo,BYTE pauto)
{
	BYTE buff[2] = {0x14};
	
	buff[1] = pauto;
	
	ReturnToUser(buff,2);		
}
static void ReturnModeUpToWince(P_FLY_AC_INFO pFlYAcInfo,BYTE modeup)
{
	BYTE buff[2] = {0x15};
	
	buff[1] = modeup;
	
	ReturnToUser(buff,2);		
}
static void ReturnModeFrontToWince(P_FLY_AC_INFO pFlYAcInfo,BYTE modefront)
{
	BYTE buff[2] = {0x16};
	
	buff[1] = modefront;
	
	ReturnToUser(buff,2);		
}

static void ReturnModeDownToWince(P_FLY_AC_INFO pFlYAcInfo,BYTE modedown)
{
	BYTE buff[2] = {0x17};
	
	buff[1] = modedown;
	
	ReturnToUser(buff,2);		
}

static void ReturnMaxToWince(P_FLY_AC_INFO pFlYAcInfo,BYTE max)
{
	BYTE buff[2] = {0x18};
	
	buff[1] = max;
	
	ReturnToUser(buff,2);		
}

static void ReturnRearToWince(P_FLY_AC_INFO pFlYAcInfo,BYTE rear)
{
	BYTE buff[2] = {0x19};
	
	buff[1] = rear;
	
	ReturnToUser(buff,2);		
}
static void ReturnVaneToWince(P_FLY_AC_INFO pFlYAcInfo,BYTE vane)
{
	BYTE buff[2] = {0x1a};
	
	buff[1] = vane;
	
	ReturnToUser(buff,2);		
}

static void ReturnOffToWince(P_FLY_AC_INFO pFlYAcInfo,BYTE off)
{
	BYTE buff[2] = {0x1b};
	
	buff[1] = off;
	
	ReturnToUser(buff,2);		
}
static void ReturnDualToWince(P_FLY_AC_INFO pFlYAcInfo,BYTE dual)
{
	BYTE buff[2] = {0x1c};
	
	buff[1] = dual;
		
	ReturnToUser(buff,2);		
		
}
static void ReturnleftHeatToWince(P_FLY_AC_INFO pFlYAcInfo,BYTE leftHeat)
{
	BYTE buff[2] = {0x1d};

	buff[1] = leftHeat;

	ReturnToUser(buff,2);		
}
static void ReturnRightHeatToWince(P_FLY_AC_INFO pFlYAcInfo,BYTE rightHeat)
{
	BYTE buff[2] = {0x1e};

	buff[1] = rightHeat;

	ReturnToUser(buff,2);		
}
static void ReturnAcMax(P_FLY_AC_INFO pFlYAcInfo,BYTE acMax)
{
	BYTE buff[2] = {0x1f};

	buff[1] = acMax;

	ReturnToUser(buff,2);	
}
static void ReturnRearLock(P_FLY_AC_INFO pFlYAcInfo,BYTE rearLock)
{
	BYTE buff[2] = {0x20};

	buff[1] = rearLock;

	ReturnToUser(buff,2);	
}
static void ReturnAcMode(P_FLY_AC_INFO pFlYAcInfo,BYTE acMode)
{
	BYTE buff[2] = {0x21};

	buff[1] = acMode;

	ReturnToUser(buff,2);	
}
static void DealB50AcData(P_FLY_AC_INFO pFlYAcInfo,BYTE *buf)
{
	if(Speed != (buf[0]&0x0f))
	{
		Speed = buf[0]&0x0f;
		ReturnSpeedToWince(pFlYAcInfo,Speed);
	}

	if(AcMode != ((buf[0]>>4)&0xf))
	{
		AcMode = (buf[0]>>4)&0xf;
		ReturnAcMode(pFlYAcInfo,AcMode);
	}

	if(Auto != ((buf[1]>>6)&0x03))
	{
		Auto = (buf[1]>>6)&0x03;
		ReturnAutoToWince(pFlYAcInfo,Auto);
	}

	if(Off != ((buf[1]>>4)&0x03))
	{
		Off = (buf[1]>>4)&0x03;
		ReturnOffToWince(pFlYAcInfo,Off);
	}

	if(AC != ((buf[1]>>2)&0x03))
	{
		AC = (buf[1]>>2)&0x03;
		ReturnAcToWince(pFlYAcInfo,AC);
	}

	if(Vane != ((buf[1] &0x03)))
	{
		Vane = buf[1] &0x03;
		ReturnVaneToWince(pFlYAcInfo,Vane);
	}

	if(Ltemp_L != (BYTE)(buf[2]*10))
	{
		Rtemp_L=Ltemp_L = buf[2]*10;
		Rtemp_H=Ltemp_H =(buf[2]*10)>>8;
		ReturnLTempToWince(pFlYAcInfo,Ltemp_H,Ltemp_L);		
		ReturnRTempToWince(pFlYAcInfo,Rtemp_H,Rtemp_L);
	}

}
static void DealTuguanAcData(P_FLY_AC_INFO pFlYAcInfo,BYTE *buf,UINT length)
{

	switch(buf[3])
	{
		case 0x5b://left temperature
			if(Ltemp_L != (BYTE)(buf[4]*10))
			{
				Ltemp_L = buf[4]*10;
				Ltemp_H =(buf[4]*10)>>8;
				ReturnLTempToWince(pFlYAcInfo,Ltemp_H,Ltemp_L);				
			}
			break;
		case 0x61://right temperature
			if(Rtemp_L != (BYTE)(buf[4]*10))
			{
				Rtemp_L = buf[4]*10;
				Rtemp_H =(buf[4]*10)>>8;
				ReturnRTempToWince(pFlYAcInfo,Rtemp_H,Rtemp_L);
			}
			break;
		case 0x6e://max
			if(Max != (buf[4] & 0x01))
			{
				Max = buf[4] & 0x01;
				ReturnMaxToWince(pFlYAcInfo,Max);
			}
			break;
		case 0x71://ac
			if(AC != (buf[4]&0x01))
			{
				AC = buf[4] & 0x01;
				ReturnAcToWince(pFlYAcInfo,AC);
			}
			break;	
		case 0x5d://auto
		case 0x63:
			if(pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName == CAR_TUGUAN_07B_9)
			{
				if((buf[4]&0xf0) == 0)
				{
					if(Auto != 0)
					{
						Auto =0;
						ReturnAutoToWince(pFlYAcInfo,0);
					}
				}
				else
				{
					if(buf[4] & 0x10)
					{
						if(Auto != 1)
						{
							Auto = 1;
							ReturnAutoToWince(pFlYAcInfo,1);
						}
					}
					else
					{
						if(Auto != 2)
						{
							Auto =2;
							ReturnAutoToWince(pFlYAcInfo,2);					
						}
					}
				}
			}
			break;
		case 0x55://dual
			if(( buf[4] ==0 )&&(buf[5] == 0))
			{
				if(Dual != 1)
				{
					Dual =1;
					ReturnDualToWince(pFlYAcInfo,1);
				}
			}
			else
			{
				if(Dual != 0)
				{
					Dual =0;
					ReturnDualToWince(pFlYAcInfo,0);
				}
			}
			break;
		case 0x5e://mode
			if(buf[4] == 0xC)
			{
				if(ModeUp != 1)
				{
					ModeUp = 1;
					ReturnModeUpToWince(pFlYAcInfo,1);
				}
			}
			else
			{
				if(ModeUp != 0)
				{
					ModeUp = 0;
					ReturnModeUpToWince(pFlYAcInfo,0);
				}
			}
			if(buf[5] == 0xC)
			{
				if(ModeFront != 1)
				{
					ModeFront = 1;
					ReturnModeFrontToWince(pFlYAcInfo,1);
				}
			}
			else
			{
				if(ModeFront != 0)
				{
					ModeFront = 0;
					ReturnModeFrontToWince(pFlYAcInfo,0);
				}
			}
			if(buf[6] == 0xC)
			{
				if(ModeDown != 1)
				{
					ModeDown = 1;
					ReturnModeDownToWince(pFlYAcInfo,1);
				}
			}
			else
			{
				if(ModeDown != 0)
				{
					ModeDown = 0;
					ReturnModeDownToWince(pFlYAcInfo,0);
				}
			}
			if(pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName == CAR_TUGUAN_07B_17)
			{
				if(buf[7] == 0x03)
				{
					if(Auto !=1)
					{
						Auto =1;
						ReturnAutoToWince(pFlYAcInfo,Auto);	
					}
				}
				else
				{
					if(Auto !=0)
					{
						Auto =0;
						ReturnAutoToWince(pFlYAcInfo,Auto);	
					}
				}
			}
			break;
		case 0x5c://speed
		case 0x62:
			if(Speed != buf[4])
			{
				Speed = buf[4];
				ReturnSpeedToWince(pFlYAcInfo,buf[4]);
			}
			break;
		case 0x50://vane
			if(Vane != (buf[4]&1))
			{
				Vane = (buf[4]&0x01);
				ReturnVaneToWince(pFlYAcInfo,Vane);
			}
			break;
		case 0x51://vane
			if(buf[4]  ==1)
			{
				if(Vane != 1)
				{
					Vane =1;
					ReturnVaneToWince(pFlYAcInfo,1);
				}
			}
			else if(buf[4] ==2)
			{
				if(Vane != 2)
				{
					Vane =2;
					ReturnVaneToWince(pFlYAcInfo,2);
				}
			}
			else
			{
				if(Vane != 0)
				{
					Vane =0;
					ReturnVaneToWince(pFlYAcInfo,0);
				}

			}
			break;
		case 0x58://rear
			if(Rear != (buf[4]&0x1))
			{
				Rear = buf[4]&0x1;
				ReturnRearToWince(pFlYAcInfo,Rear);
			}
			break;
		case 0x4d://off
			if(buf[4] & 0x10)
			{
				if(Off != 0)
				{
					Off =0;
					ReturnOffToWince(pFlYAcInfo,0);

				}
			}
			else
			{
				if(Off != 1)
				{
					Off =1;
					ReturnOffToWince(pFlYAcInfo,1);
				}
			}
			break;
		case 0x56:
			break;
		case 0x5f://左座椅加热
			if(LeftHeat!= buf[4])
			{
				LeftHeat =buf[4];
				ReturnleftHeatToWince(pFlYAcInfo,buf[4]);
			}
			break;
		case 0x65://右座椅加热
			if(RightHeat!= buf[4])
			{
				RightHeat =buf[4];
				ReturnRightHeatToWince(pFlYAcInfo,buf[4]);
			}
			break;
		case 0x67:
			if(pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName == CAR_TUGUAN_07B_17)
			{
				if(buf[4] ==0)
				{
					if(Rear_Lock != 1)
					{
						Rear_Lock =1;
						ReturnRearLock(pFlYAcInfo,Rear_Lock);
					}
				}
				else
				{
					if(Rear_Lock != 0)
					{
						Rear_Lock =0;
						ReturnRearLock(pFlYAcInfo,Rear_Lock);
					}
				}
			}
			break;	
		case 0x75:
			if(pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName == CAR_TUGUAN_07B_17)
			{
				if(AC_MAX != buf[4])
				{
					AC_MAX = buf[4];
					ReturnAcMax(pFlYAcInfo,AC_MAX);
				}
			}
			break;
		default:
			break;
	}

}


static DWORD WINAPI ThreadRead(LPVOID pContext)
{
	P_FLY_AC_INFO pFlYAcInfo = (P_FLY_AC_INFO)pContext;
	ULONG WaitReturn;
	BYTE buff[FLY_MSG_QUEUE_TO_AC_LENGTH];
	DWORD dwRes;
	DWORD dwMsgFlag;

	while (!pFlYAcInfo->bKillDispatchFlyMsgQueueReadThread)
	{
		WaitReturn = WaitForSingleObject(pFlYAcInfo->hMsgQueueToAcCreate, INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nFlyAudio FAC ThreadRead Event hMsgQueueToFACmOpen Error")));
		}
		ReadMsgQueue(pFlYAcInfo->hMsgQueueToAcCreate, buff, FLY_MSG_QUEUE_TO_AC_LENGTH, &dwRes, 0, &dwMsgFlag);

		if (pFlYAcInfo->bPowerUp)
		{
			if(dwRes)
			{
				DBGD((TEXT("\r\nFAC Read MsgQueue%d %x %x Open%d"),dwRes,buff[0],buff[1],pFlYAcInfo->bOpen));
				switch (buff[1])
				{					
					case 0x00://途观
						DealTuguanAcData(pFlYAcInfo,&buff[0],buff[0]-1);
						break;
					case 0x01:
						DealB50AcData(pFlYAcInfo,&buff[2]);//,buff[0]-1);
						break;
					default:
						DBGD((TEXT("\r\nFlyAudio FAC ThreadRead Unhandle%d %x %x"),dwRes,buff[0],buff[1]));
							break;
				}
			}
		}
	}
	DBGD((TEXT("\r\nFlyAudio FAC ThreadRead exit")));
		pFlYAcInfo->FLyMsgQueueReadThreadHandle = NULL;
	return 0;
}

void FlyFACEnable(P_FLY_AC_INFO pFlYAcInfo,BOOL bEnable)
{
	DWORD dwThreadID;

	if (bEnable)
	{
		if (pFlYAcInfo->bPower)
		{
			return;
		}
		pFlYAcInfo->bPower = TRUE;

		pFlYAcInfo->bKillDispatchFlyMainThread = FALSE;
		pFlYAcInfo->hDispatchMainThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
		pFlYAcInfo->FlyMainThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadMain, //线程的全局函数
			pFlYAcInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		
		DBGE((TEXT("\r\nFlyAudio FAC ThreadMain ID:%x"),dwThreadID));
		if (NULL == pFlYAcInfo->FlyMainThreadHandle)
		{
			pFlYAcInfo->bKillDispatchFlyMainThread = TRUE;
			return;
		}
		SetEvent(pFlYAcInfo->hDispatchMainThreadEvent);//初始运行一次

		pFlYAcInfo->bKillDispatchFlyInterThread = FALSE;
		pFlYAcInfo->hDispatchInterThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
		pFlYAcInfo->FLyInterThreadHandle = 
				CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
				0,//初始化线程栈的大小，缺省为与主线程大小相同
				(LPTHREAD_START_ROUTINE)ThreadInter, //线程的全局函数
				pFlYAcInfo, //此处传入了主框架的句柄
				0, &dwThreadID );
		DBGE((TEXT("\r\nFlyAudio FAC ThreadInter ID:%x"),dwThreadID));
		if (NULL == pFlYAcInfo->FLyInterThreadHandle)
		{
			pFlYAcInfo->bKillDispatchFlyInterThread = TRUE;
			return;
		}
	}
	else
	{
		if (!pFlYAcInfo->bPower)
		{
			return;
		}
		pFlYAcInfo->bPower = FALSE;
		pFlYAcInfo->bPowerUp = FALSE;

		pFlYAcInfo->bKillDispatchFlyMainThread = TRUE;
		SetEvent(pFlYAcInfo->hDispatchMainThreadEvent);

		pFlYAcInfo->bKillDispatchFlyInterThread = TRUE;
		SetEvent(pFlYAcInfo->hDispatchInterThreadEvent);

		while (pFlYAcInfo->FlyMainThreadHandle)
		{
			SetEvent(pFlYAcInfo->hDispatchMainThreadEvent);
			Sleep(10);
		}
	}
}



static VOID WinCECommandProcessor(P_FLY_AC_INFO pFlYAcInfo,BYTE *buff,UINT len)
{
	switch (buff[0])
	{
	case 0x01:
		if(0x01 == buff[1])
		{
			returnFACPowerMode(pFlYAcInfo,TRUE);
			returnFACbInit(pFlYAcInfo,TRUE);
			ReturnRTempToWince(pFlYAcInfo,Rtemp_H,Rtemp_L);
			ReturnLTempToWince(pFlYAcInfo,Ltemp_H,Ltemp_L);	
			ReturnMaxToWince(pFlYAcInfo,Max);
			ReturnAcToWince(pFlYAcInfo,AC);
			ReturnAutoToWince(pFlYAcInfo,Auto);
			ReturnDualToWince(pFlYAcInfo,Dual);
			ReturnModeUpToWince(pFlYAcInfo,ModeUp);
			ReturnModeFrontToWince(pFlYAcInfo,ModeFront);
			ReturnModeDownToWince(pFlYAcInfo,ModeDown);
			ReturnSpeedToWince(pFlYAcInfo,Speed);
			ReturnVaneToWince(pFlYAcInfo,Vane);
			ReturnRearToWince(pFlYAcInfo,Rear);
			ReturnOffToWince(pFlYAcInfo,Off);
			ReturnleftHeatToWince(pFlYAcInfo,LeftHeat);
			ReturnRightHeatToWince(pFlYAcInfo,RightHeat);
			ReturnAcMax(pFlYAcInfo,AC_MAX);
			ReturnRearLock(pFlYAcInfo,Rear_Lock);			
			ReturnAcMode(pFlYAcInfo,AcMode);
		}
		else
		{
			returnFACPowerMode(pFlYAcInfo,false);
		}
		break;
	case 47://途观的数据
		break;
	case 0xFF:
		if (0x01 == buff[1])
		{
			FAC_PowerUp((DWORD)pFlYAcInfo);
		} 
		else if (0x00 == buff[1])
		{
			FAC_PowerDown((DWORD)pFlYAcInfo);
		}
		break;
	default:
		break;
	}
}

static void powerNormalInit(P_FLY_AC_INFO pFlYAcInfo)
{

	pFlYAcInfo->buffToUserHx = 0;
	pFlYAcInfo->buffToUserLx = 0;
}

static void powerOnFirstInit(P_FLY_AC_INFO pFlYAcInfo)
{
	pFlYAcInfo->bOpen = FALSE;
	pFlYAcInfo->bPower = FALSE;
	pFlYAcInfo->bSpecialPower = FALSE;
	pFlYAcInfo->bPowerUp = FALSE;
	pFlYAcInfo->bUserPowerUp = FALSE;

	pFlYAcInfo->bKillDispatchFlyMainThread = TRUE;
	pFlYAcInfo->FlyMainThreadHandle = NULL;
	pFlYAcInfo->hDispatchMainThreadEvent = NULL;

	pFlYAcInfo->hMsgQueueToAcCreate = NULL;
	pFlYAcInfo->hMsgQueueFromAcCreate = NULL;
	pFlYAcInfo->bKillDispatchFlyMsgQueueReadThread = TRUE;
	pFlYAcInfo->FLyMsgQueueReadThreadHandle = NULL;

	pFlYAcInfo->bKillDispatchFlyInterThread = TRUE;
	pFlYAcInfo->FLyInterThreadHandle = NULL;
	pFlYAcInfo->hDispatchInterThreadEvent = NULL;

}

static void powerOnSpecialEnable(P_FLY_AC_INFO pFlYAcInfo,BOOL bOn)
{
	DWORD dwThreadID;

	if (bOn)
	{
		if (pFlYAcInfo->bSpecialPower)
		{
			return;
		}
		pFlYAcInfo->bSpecialPower = TRUE;	

		pFlYAcInfo->hBuffToUserDataEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		InitializeCriticalSection(&pFlYAcInfo->hCSSendToUser);

		MSGQUEUEOPTIONS  msgOpts;

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_AC_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_AC_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pFlYAcInfo->hMsgQueueToAcCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_AC_NAME, &msgOpts);
		if (NULL == pFlYAcInfo->hMsgQueueToAcCreate)
		{
			DBGE((TEXT("\r\nFlyAudio FAC Create MsgQueue To FAC Fail!")));
		}

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_FROM_AC_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_FROM_AC_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pFlYAcInfo->hMsgQueueFromAcCreate = CreateMsgQueue(FLY_MSG_QUEUE_FROM_AC_NAME, &msgOpts);
		if (NULL == pFlYAcInfo->hMsgQueueFromAcCreate)
		{
			DBGE((TEXT("\r\nFlyAudio FAC Create MsgQueue From FAC Fail!")));
		}

		pFlYAcInfo->bKillDispatchFlyMsgQueueReadThread = FALSE;
		pFlYAcInfo->FLyMsgQueueReadThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadRead, //线程的全局函数
			pFlYAcInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\nFAC ThreadRead ID:%x"),dwThreadID));
		if (NULL == pFlYAcInfo->FLyMsgQueueReadThreadHandle)
		{
			pFlYAcInfo->bKillDispatchFlyMsgQueueReadThread = TRUE;
			return;
		}
	} 
	else
	{
		if (!pFlYAcInfo->bSpecialPower)
		{
			return;
		}
		pFlYAcInfo->bSpecialPower = FALSE;

		CloseHandle(pFlYAcInfo->hBuffToUserDataEvent);

		pFlYAcInfo->bKillDispatchFlyMsgQueueReadThread = TRUE;
		while (pFlYAcInfo->FLyMsgQueueReadThreadHandle)
		{
			SetEvent(pFlYAcInfo->hMsgQueueToAcCreate);
			Sleep(10);
		}
		CloseMsgQueue(pFlYAcInfo->hMsgQueueToAcCreate);
		CloseMsgQueue(pFlYAcInfo->hMsgQueueFromAcCreate);
	}
}

void IpcRecv(UINT32 iEvent)
{
	P_FLY_AC_INFO	pFlYAcInfo = (P_FLY_AC_INFO)gpFlYAcInfo;

	DBGD((TEXT("\r\nFLYAC Recv IPC iEvent:%d\r\n"),iEvent));

	SetEvent(pFlYAcInfo->hDispatchMainThreadEvent);
}
//void SockRecv(BYTE *buf, UINT16 len)
//{
//	P_FLY_TDA7541_RADIO_INFO	pFlYAcInfo = (P_FLY_TDA7541_RADIO_INFO)gpTDA7541RadioInfo;
//}

HANDLE
FAC_Init(DWORD dwContext)
{
	P_FLY_AC_INFO pFlYAcInfo;

	RETAILMSG(1, (TEXT("\r\nFACInit Start")));

	pFlYAcInfo = (P_FLY_AC_INFO)LocalAlloc(LPTR, sizeof(FLY_AC_INFO));
	if (!pFlYAcInfo)
	{
		return NULL;
	}

	gpFlYAcInfo = pFlYAcInfo;

	pFlYAcInfo->pFlyShmGlobalInfo = CreateShm(AC_MODULE,IpcRecv);
	if (NULL == pFlYAcInfo->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio AC Init create shm err\r\n")));
		return NULL;
	}

	if (!GetDllAddrTable())
	{
		DBGE((TEXT("FlyAudio AC  GetDllAddrTable err\r\n")));
		return NULL;
	}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio AC Open create user buf err\r\n")));
		return NULL;
	}
	//if(!CreateServerSocket(SockRecv, TCP_PORT_RADIO))
	//{
	//	DBGE((TEXT("FlyAudio AC Open create server socket err\r\n")));
	//	return NULL;
	//}



	if (pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalStructSize != sizeof(struct shm))
	{
		pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[0] = 'F';
		pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[1] = 'A';
		pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[2] = 'C';
	}

	powerNormalInit(pFlYAcInfo);
	powerOnFirstInit(pFlYAcInfo);
	powerOnSpecialEnable(pFlYAcInfo,TRUE);

#if GLOBAL_COMM
	pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAcInfo.bInit = TRUE;
	pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAcInfo.iDriverCompYear = year;
	pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAcInfo.iDriverCompMon = months;
	pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAcInfo.iDriverCompDay = day;
	pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAcInfo.iDriverCompHour = hours;
	pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAcInfo.iDriverCompMin = minutes;
	pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAcInfo.iDriverCompSec = seconds;

#endif


	RETAILMSG(1, (TEXT("\r\nFlyAudio AC Init Build:Date%d.%d.%dTime%d:%d:%ddebugMsgLevel:%d")
		,year,months,day,hours,minutes,seconds
		,pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAcInfo.debugMsgLevel));
	return (HANDLE)pFlYAcInfo;
}

BOOL
FAC_Deinit(DWORD hDeviceContext)
{
	P_FLY_AC_INFO	pFlYAcInfo = (P_FLY_AC_INFO)hDeviceContext;
	CloseHandle(pFlYAcInfo->hBuffToUserDataEvent);

	powerOnSpecialEnable(pFlYAcInfo,FALSE);

#if GLOBAL_COMM
	//全局
	pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAcInfo.bInit = FALSE;
#endif

	RETAILMSG(1, (TEXT("\r\nFlyAudio FAC DeInit")));
	FreeShm();
	//FreeSocketServer();
	FreeUserBuff();

	LocalFree(pFlYAcInfo);
	return TRUE;
}

DWORD
FAC_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	P_FLY_AC_INFO	pFlYAcInfo = (P_FLY_AC_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;

	if (pFlYAcInfo->bOpen)
	{
		return NULL;
	}
	pFlYAcInfo->bOpen = TRUE;
	
	FlyFACEnable(pFlYAcInfo,TRUE);
#if GLOBAL_COMM
	//全局
	pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAcInfo.bOpen = TRUE;
#endif

	RETAILMSG(1, (TEXT("\r\nFlyAudio FAC Open Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return returnWhat;
}

BOOL
FAC_Close(DWORD hDeviceContext)
{
	P_FLY_AC_INFO	pFlYAcInfo = (P_FLY_AC_INFO)hDeviceContext;

	FlyFACEnable(pFlYAcInfo,FALSE);
	
#if GLOBAL_COMM
	//全局
	pFlYAcInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAcInfo.bOpen = FALSE;
#endif

	pFlYAcInfo->bOpen = FALSE;
	SetEvent(pFlYAcInfo->hBuffToUserDataEvent);



	RETAILMSG(1, (TEXT("\r\nFlyAudio FAC Close")));
	return TRUE;
}

VOID
FAC_PowerUp(DWORD hDeviceContext)
{
	P_FLY_AC_INFO	pFlYAcInfo = (P_FLY_AC_INFO)hDeviceContext;

	powerNormalInit(pFlYAcInfo);

	SetEvent(pFlYAcInfo->hDispatchMainThreadEvent);

	RETAILMSG(1, (TEXT("\r\nFlyAudio FAC PowerUp")));
}

VOID
FAC_PowerDown(DWORD hDeviceContext)
{
	P_FLY_AC_INFO	pFlYAcInfo = (P_FLY_AC_INFO)hDeviceContext;

	RETAILMSG(1, (TEXT("\r\nFlyAudio FAC PowerDown")));
}

DWORD
FAC_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	P_FLY_AC_INFO	pFlYAcInfo = (P_FLY_AC_INFO)hOpenContext;
	UINT dwRead = 0,i;
	BYTE *buf = (BYTE *)pBuffer;
	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio AC return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}

	return dwRead;
}

DWORD
FAC_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	P_FLY_AC_INFO	pFlYAcInfo = (P_FLY_AC_INFO)hOpenContext;
	DBGD((TEXT("\r\nFlyAudio FAC Write %d"),NumberOfBytes));
		for(UINT i = 0;i < NumberOfBytes;i++)
		{
			DBGD((TEXT(" %X"),
				*((BYTE *)pSourceBytes + i)));
		}
		if(NumberOfBytes)
		{
			WinCECommandProcessor(pFlYAcInfo,(((BYTE *)pSourceBytes)+3),((BYTE *)pSourceBytes)[2]);
		}

		return NULL;
}

DWORD
FAC_Seek(DWORD hOpenContext, LONG Amount, DWORD  Type)
{
	return NULL;
}

BOOL
FAC_IOControl(DWORD hOpenContext,
			  DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn,
			  PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
	P_FLY_AC_INFO	pFlYAcInfo = (P_FLY_AC_INFO)hOpenContext;
	BOOL bRes = TRUE;

	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	DBGD((TEXT("\r\nFlyAudio FAC IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)"),
	//		hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut);
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	DBGD((TEXT("\r\nFlyAudio FAC IOControl Set IOCTL_SERIAL_SET_WAIT_MASK"));
	//		break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	//DBGD((TEXT("\r\nFlyAudio FAC IOControl Set IOCTL_SERIAL_WAIT_ON_MASK"));
	//	if (pFlYAcInfo->bOpen)
	//	{
	//		WaitForSingleObject(pFlYAcInfo->hBuffToUserDataEvent,INFINITE);
	//	}
	//	else
	//	{
	//		WaitForSingleObject(pFlYAcInfo->hBuffToUserDataEvent,0);
	//	}
	//	if ((dwLenOut < sizeof(DWORD)) || (NULL == pBufOut) ||
	//		(NULL == pdwActualOut))
	//	{
	//		bRes = FALSE;
	//		break;
	//	}
	//	*(DWORD *)pBufOut = EV_RXCHAR;
	//	*pdwActualOut = sizeof(DWORD);
	//	break;
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
		DEBUGMSG(1, (TEXT("Attach in FAC DllEntry\n")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}

	if (dwReason == DLL_PROCESS_DETACH)
	{
		DEBUGMSG(1, (TEXT("Dettach in FAC DllEntry\n")));
	}

	DEBUGMSG(1, (TEXT("Leaving FAC DllEntry\n")));

	return (TRUE);
}
