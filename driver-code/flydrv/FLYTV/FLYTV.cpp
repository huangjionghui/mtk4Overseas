// -----------------------------------------------------------------------------
// File Name    : FLYV.cpp
// Title        : FLYTV Driver
// Author       : Mootall - Copyright (C) 2011
// Created      : 2011-06-16  
// Version      : 0.01
// Target MCU   : WinCE Driver
// -----------------------------------------------------------------------------
// Version History:
/*
>>>
>>>
*/
// FLYTV.cpp : Defines the entry point for the DLL application.

#include "FLYTV.h"
#include "flyuserlib.h"

P_FLY_TV_INFO gpFlyTVInfo = NULL;

VOID FTV_PowerUp(DWORD hDeviceContext);
VOID FTV_PowerDown(DWORD hDeviceContext);
//static VOID FlyPutToBuff(P_FLY_TV_INFO pFlyTVInfo,BYTE data)
//{
//	pFlyTVInfo->BuffToUser[pFlyTVInfo->iBuffToUserHx++] = data;
//	if (pFlyTVInfo->iBuffToUserHx >= DATA_BUFF_LENGTH)
//	{
//		pFlyTVInfo->iBuffToUserHx = 0;
//	}
//	if (pFlyTVInfo->iBuffToUserHx == pFlyTVInfo->iBuffToUserLx)
//	{
//		DBGE((TEXT("\r\n\r\n\r\nFlyAudio TV Buff to user overflow!!!\r\n\r\n\r\n")));
//	}
//}
//
//static VOID FlyReturnToUser(P_FLY_TV_INFO pFlyTVInfo,BYTE *buf,UINT len)
//{
//	BYTE crc;
//	UINT i;
//	EnterCriticalSection(&pFlyTVInfo->hCSSendToUser);
//	FlyPutToBuff(pFlyTVInfo,0xFF);
//	FlyPutToBuff(pFlyTVInfo,0x55);
//	FlyPutToBuff(pFlyTVInfo,len+1);
//	crc = len + 1;
//	for(i = 0;i < len;i++)
//	{
//		FlyPutToBuff(pFlyTVInfo,buf[i]);
//		crc += buf[i];
//	}
//	FlyPutToBuff(pFlyTVInfo,crc);
//	LeaveCriticalSection(&pFlyTVInfo->hCSSendToUser);
//	SetEvent(pFlyTVInfo->hBuffToUserDataEvent);
//
//	DBGD((TEXT("\r\nFlyTVReturnToUser:")));
//		for (i=0;i<len;i++)
//		{
//			DBGD((TEXT(" 0x%X"),buf[i]));
//		}
//}
static void returnPowerStatus(P_FLY_TV_INFO pFlyTVInfo,BOOL bPower)
{
	BYTE buff[] = {0x01,0x00};//0x00:关 0x01:开
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
static void returnWorkMode(P_FLY_TV_INFO pFlyTVInfo,BOOL bWork)
{
	BYTE buff[2];
	buff[0] = 0x02;
	if (bWork)
	{
		buff[1] = 0x01;
	} 
	else
	{
		buff[1] = 0x00;
	}
	ReturnToUser(buff,2);
}
static void returnTVConnectStatus(P_FLY_TV_INFO pFlyTVInfo,BOOL bHave)
{
	return;//由System返回
	BYTE buff[] = {0x03,0x00};
	if (bHave)
	{
		buff[1] = 0x01;
	} 
	else
	{
		buff[1] = 0x00;
	}
	ReturnToUser(buff,2);
}
static void returnFcwLdwInfo(P_FLY_TV_INFO pFlyTVInfo,BYTE *buf,UINT16 len)
{
	BYTE buff[255]={0x04};

	memcpy(&buff[1],buf,len);

	ReturnToUser(buff,len+1);

}
static void returnFactoryInfo(P_FLY_TV_INFO pFlyTVInfo,BYTE *buf,UINT16 len)
{
	BYTE buff[10]={0x05};

	memcpy(&buff[1],buf,9);

	ReturnToUser(buff,10);

}

static VOID powerParaDeInit(P_FLY_TV_INFO pFlyTVInfo)
{
	pFlyTVInfo->bPower = FALSE;

	pFlyTVInfo->iBuffToUserLx = 0;
	pFlyTVInfo->iBuffToUserHx = 0;
}

static VOID powerNormalInit(P_FLY_TV_INFO pFlyTVInfo)
{
	pFlyTVInfo->iBuffToUserLx = 0;
	pFlyTVInfo->iBuffToUserHx = 0;
	pFlyTVInfo->bHavaTV = FALSE;
	
	pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTVModule = FALSE;

}
static VOID powerOnFirstInit(P_FLY_TV_INFO pFlyTVInfo)
{
	pFlyTVInfo->bOpen = FALSE;
	pFlyTVInfo->bPower = FALSE;
	pFlyTVInfo->bSpecialPower = FALSE;

	pFlyTVInfo->hMsgQueueTVToExBoxCreate = NULL;
	pFlyTVInfo->hMsgQueueExBoxToTVCreate = NULL;
	pFlyTVInfo->bKillDispatchFlyMsgQueueEXBOXReadThread = TRUE;
	pFlyTVInfo->FLyMsgQueueEXBOXReadThreadHandle = NULL;

	pFlyTVInfo->bKillDispatchFlyTVThread = TRUE;
	pFlyTVInfo->hThreadHandleFlyTV = NULL;
	pFlyTVInfo->hDispatchThreadTVEvent = NULL;		
}
/*
FlyAudio TV Read MsgQueue From ExBox2 f0 d2
FlyAudio TV Read MsgQueue From ExBox2 f3 8a
FlyAudio TV Read MsgQueue From ExBox2 f4 d
*/
static VOID FlyTV_infoProcessor(P_FLY_TV_INFO pFlyTVInfo, BYTE data1, BYTE data2)
{
	if (data1 == 0xf3)
	{

	} 
	else if (data1 == 0xf4)
	{

	}	
}
static DWORD WINAPI ThreadExBoxRead(LPVOID pContext)
{
	P_FLY_TV_INFO pFlyTVInfo = (P_FLY_TV_INFO)pContext;
	ULONG WaitReturn;
	BYTE buff[FLY_MSG_QUEUE_EXBOX_TO_TV_LENGTH];
	DWORD dwRes;
	DWORD dwMsgFlag;

	while (!pFlyTVInfo->bKillDispatchFlyMsgQueueEXBOXReadThread)
	{
		WaitReturn = WaitForSingleObject(pFlyTVInfo->hMsgQueueExBoxToTVCreate, INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nFlyAudio TV ThreadRead Event hMsgQueueFromExBoxCreate Error")));
		}
		ReadMsgQueue(pFlyTVInfo->hMsgQueueExBoxToTVCreate, buff, FLY_MSG_QUEUE_EXBOX_TO_TV_LENGTH, &dwRes, 0, &dwMsgFlag);
		if(dwRes)
		{
			DBGD((TEXT("\r\nFlyAudio TV Read MsgQueue From ExBox%d %x %x"),dwRes,buff[0],buff[1]));
			if((buff[0] == 0xf3)||(buff[0] == 0xf4))
			{
				FlyTV_infoProcessor(pFlyTVInfo,buff[0],buff[1]);
			}
			else if (buff[0] == 0x1E)
			{
				returnFactoryInfo(pFlyTVInfo,buff,(UINT16)dwRes);
			}
			else
			{
				returnFcwLdwInfo(pFlyTVInfo,buff,(UINT16)dwRes);
			}
		}
	}
	pFlyTVInfo->FLyMsgQueueEXBOXReadThreadHandle = NULL;
	DBGE((TEXT("\r\nFlyAudio TV ThreadCOMxRead exit")));
		return 0;
}
static void powerOnSpecialEnable(P_FLY_TV_INFO pFlyTVInfo,BOOL bOn)
{
	DWORD dwThreadID;
	if (bOn)
	{
		MSGQUEUEOPTIONS  msgOpts;

		if (pFlyTVInfo->bSpecialPower)
		{
			return;
		}
		pFlyTVInfo->bSpecialPower = TRUE;

		pFlyTVInfo->hBuffToUserDataEvent = CreateEvent(NULL,FALSE,FALSE,NULL);	
		InitializeCriticalSection(&pFlyTVInfo->hCSSendToUser);

		//ExBox 
		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TV_TO_EXBOX_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TV_TO_EXBOX_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pFlyTVInfo->hMsgQueueTVToExBoxCreate = CreateMsgQueue(FLY_MSG_QUEUE_TV_TO_EXBOX_NAME, &msgOpts);
		if (NULL == pFlyTVInfo->hMsgQueueTVToExBoxCreate)
		{
			DBGE((TEXT("\r\nFlyAudio TV Create MsgQueue To COMx Fail!")));
		}

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_EXBOX_TO_TV_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_EXBOX_TO_TV_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pFlyTVInfo->hMsgQueueExBoxToTVCreate = CreateMsgQueue(FLY_MSG_QUEUE_EXBOX_TO_TV_NAME, &msgOpts);
		if (NULL == pFlyTVInfo->hMsgQueueExBoxToTVCreate)
		{
			DBGE((TEXT("\r\nFlyAudio TV Create MsgQueue FROM EXBOX Fail!")));
		}
		pFlyTVInfo->bKillDispatchFlyMsgQueueEXBOXReadThread = FALSE;
		pFlyTVInfo->FLyMsgQueueEXBOXReadThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadExBoxRead, //线程的全局函数
			pFlyTVInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\nFlyAudio ThreadExBoxRead ID:%x"),dwThreadID));
		if (NULL == pFlyTVInfo->FLyMsgQueueEXBOXReadThreadHandle)
		{
			pFlyTVInfo->bKillDispatchFlyMsgQueueEXBOXReadThread = TRUE;
			return;
		}
	}
	else
	{
		if (!pFlyTVInfo->bSpecialPower)
		{
			return;
		}
		pFlyTVInfo->bKillDispatchFlyMsgQueueEXBOXReadThread = TRUE;
		while (pFlyTVInfo->FLyMsgQueueEXBOXReadThreadHandle)
		{
			SetEvent(pFlyTVInfo->hMsgQueueExBoxToTVCreate);
			Sleep(10);
		}
		CloseMsgQueue(pFlyTVInfo->hMsgQueueExBoxToTVCreate);
		CloseMsgQueue(pFlyTVInfo->hMsgQueueExBoxToTVCreate);

		pFlyTVInfo->bSpecialPower = FALSE;

		CloseHandle(pFlyTVInfo->hBuffToUserDataEvent);
	}
}
static VOID TVWriteFile(P_FLY_TV_INFO pFlyTVInfo,BYTE *p,UINT length)
{
	if (pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalExBoxInfo.bInit)
	{
		if (FALSE == WriteMsgQueue(pFlyTVInfo->hMsgQueueTVToExBoxCreate, p, length, 0, 0))
		{
			DWORD dwError = GetLastError();

			DBGE((TEXT("\r\nFlyAudio TV Write MsgQueue To ExBox Error!0x%x"),dwError));
		}
	}
}
static VOID TVCmdPrintf(P_FLY_TV_INFO pFlyTVInfo,BYTE *buf,UINT len)
{
	BYTE sendBuf[2];
	sendBuf[0] = buf[0];
	sendBuf[1] = buf[1];
	TVWriteFile(pFlyTVInfo,sendBuf,2);
}
static VOID control_TVCmd(P_FLY_TV_INFO pFlyTVInfo,BYTE data1,BYTE data2)
{
	BYTE buff[2];
	buff[0] = data1;buff[1] = data2;
	TVCmdPrintf(pFlyTVInfo,buff,2);
}
static void control_FcwLdwCmd(P_FLY_TV_INFO pFlyTVInfo,BYTE *buf,BYTE len)
{
	TVWriteFile(pFlyTVInfo,buf,len);
}
static DWORD WINAPI ThreadFlyTVProc(LPVOID pContext)
{
	P_FLY_TV_INFO pFlyTVInfo = (P_FLY_TV_INFO)pContext;
	ULONG WaitReturn;
	while (!pFlyTVInfo->bKillDispatchFlyTVThread)
	{
		if (!pFlyTVInfo->bPower)
		{
			WaitReturn = WaitForSingleObject(pFlyTVInfo->hDispatchThreadTVEvent, 314);
		}
		else
		{
			WaitReturn = WaitForSingleObject(pFlyTVInfo->hDispatchThreadTVEvent, INFINITE);
		}

		if (pFlyTVInfo->bPower)
		{
			if (pFlyTVInfo->bHavaTV != pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTVModule)
			{
				pFlyTVInfo->bHavaTV = pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTVModule;
				returnTVConnectStatus(pFlyTVInfo,pFlyTVInfo->bHavaTV);
			}
		}
	}

	CloseHandle(pFlyTVInfo->hDispatchThreadTVEvent);
	DBGD((TEXT("\r\nFlyAudio ThreadFlyTVProc exit")));
	return 0;
}
void FlyTV_Enable(P_FLY_TV_INFO pFlyTVInfo,BOOL bEnable)
{
	DWORD dwThreadID;
	if (bEnable)
	{
		pFlyTVInfo->bKillDispatchFlyTVThread = FALSE;
		pFlyTVInfo->hDispatchThreadTVEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
		pFlyTVInfo->hThreadHandleFlyTV = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadFlyTVProc, //线程的全局函数
			pFlyTVInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\nFlyAudio ThreadFlyTVProc ID:%x"),dwThreadID));

			if (NULL == pFlyTVInfo->hThreadHandleFlyTV)
			{
				pFlyTVInfo->bKillDispatchFlyTVThread = TRUE;
				return;
			}	
	}
	else
	{
		pFlyTVInfo->bKillDispatchFlyTVThread = TRUE;
		SetEvent(pFlyTVInfo->hDispatchThreadTVEvent);		
	}
}
static VOID FlyTV_DealWinceCmd(P_FLY_TV_INFO pFlyTVInfo,BYTE *buf, UINT len)
{
	RETAILMSG(1, (TEXT(" OK!\r\n")));
	BYTE tvcmd;
	switch(buf[0])
	{
	case 0x01://初始化命令
		if (0x01 == buf[1])
		{
			returnPowerStatus(pFlyTVInfo,TRUE);
			returnWorkMode(pFlyTVInfo,TRUE);
			pFlyTVInfo->bPower = TRUE;
			SetEvent(pFlyTVInfo->hDispatchThreadTVEvent);
		}
		else if (0x00 == buf[1])
		{
			pFlyTVInfo->bPower = FALSE;
			returnPowerStatus(pFlyTVInfo,FALSE);
			returnWorkMode(pFlyTVInfo,FALSE);
		}	
		break;
	case 0x03://TV 指令
		if (buf[1] == 0x17) {
			tvcmd = 0x50;
		}
		else {tvcmd = buf[1];}
		control_TVCmd(pFlyTVInfo,0xb4,tvcmd);
		break;
	case 0x04://轨道偏离系统
		control_FcwLdwCmd(pFlyTVInfo,buf,len);
		break;

	case 0x05:
		TVWriteFile(pFlyTVInfo,&buf[1],len-1);
		break;

	case 0xff://调试用，正常使用不理会
		if (0x01 == buf[1])
		{
			FTV_PowerUp((DWORD)pFlyTVInfo);
		} 
		else if (0x00 == buf[1])
		{
			FTV_PowerDown((DWORD)pFlyTVInfo);
		}
		break;
	default:

		break;
	}
}

/********************************************************************************************************
*函数名称：WinCECommandProcessor
*函数功能：检验WINCE发下来的命令
*输入参数：
*输出参数：
*返 回 值：
**********************************************************************************************************/
//static VOID FlyTV_WinCECommandProcessor(P_FLY_TV_INFO pFlyTVInfo, BYTE *buf, UINT len)
//{
//	static BYTE checksum;
//	static UINT FrameLen;
//	static UINT FrameLenMax;
//	static BYTE ReceiveReg[256];
//
//	for(UINT i = 0;i < len;i++)
//	{
//		switch (pFlyTVInfo->buffToDriverProcessorStatus)
//		{
//		case 0:
//			if(0xFF == buf[i])
//			{
//				pFlyTVInfo->buffToDriverProcessorStatus = 1;
//			}
//			break;
//		case 1:
//			if(0x55 == buf[i])
//			{
//				pFlyTVInfo->buffToDriverProcessorStatus = 2;
//			}
//			else
//			{
//				pFlyTVInfo->buffToDriverProcessorStatus = 0;
//			}
//			break;
//		case 2:
//			pFlyTVInfo->buffToDriverProcessorStatus = 3;
//			FrameLenMax = buf[i];
//			FrameLen = 0;
//			checksum = buf[i];
//			break;
//		case 3:
//			if(FrameLen < FrameLenMax - 1)
//			{
//				ReceiveReg[FrameLen] = buf[i];
//				checksum += buf[i];
//				FrameLen ++;
//			}
//			else
//			{
//				if(buf[i] == checksum)
//				{
//					ReceiveReg[FrameLen] = 0;
//					FlyTV_DealWinceCmd(pFlyTVInfo,ReceiveReg,FrameLen);
//				}
//				pFlyTVInfo->buffToDriverProcessorStatus = 0;
//			}
//			break;
//		default:
//			pFlyTVInfo->buffToDriverProcessorStatus = 0;
//			break;
//		}
//	}
//}
void IpcRecv(UINT32 iEvent)
{
	P_FLY_TV_INFO	pFlyTVInfo = (P_FLY_TV_INFO)gpFlyTVInfo;

	DBGD((TEXT("\r\n FLY TV Recv IPC Event:%d"),iEvent));

	//SetEvent(pFlyTVInfo->hCollexBTMainThreadEvent);
}

/********************************************************************************************************
*函数名称：HANDLE FTV_Init()
*函数功能：加载设备，在设备被加载的时候调用
*输入参数：
*输出参数：
*返 回 值：
**********************************************************************************************************/
HANDLE FTV_Init(
				DWORD dwContext   //字符串，指向注册表中记录活动驱动程序的键
				)
{
	RETAILMSG(1, (TEXT("\r\nTV Init Start")));

	

	//分配内存
	P_FLY_TV_INFO pFlyTVInfo = (P_FLY_TV_INFO)LocalAlloc(LPTR, sizeof(FLY_TV_INFO));
	if (!pFlyTVInfo)
	{
		RETAILMSG(1, (TEXT("FlyAudio TV LocalAlloc Failed!\r\n")));
		return NULL;
	}

	gpFlyTVInfo = pFlyTVInfo;

	pFlyTVInfo->pFlyShmGlobalInfo = CreateShm(BT_MODULE,IpcRecv);
	if (NULL == pFlyTVInfo->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio TV Init create shm err\r\n")));
		return NULL;
	}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio TV Open create user buf err\r\n")));
		return NULL;
	}


	if (pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalStructSize
		!= sizeof(struct shm))
	{
		pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[0] = 'F';
		pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[1] = 'T';
		pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[2] = 'V';
	}

//	pFlyTVInfo->hHandleGlobalGlobalEvent = CreateEvent(NULL,FALSE,FALSE,DATA_GLOBAL_HANDLE_GLOBAL);
	pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTVInfo.bInit = TRUE;
	pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTVInfo.iDriverCompYear = year;
	pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTVInfo.iDriverCompMon = months;
	pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTVInfo.iDriverCompDay = day;
	pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTVInfo.iDriverCompHour = hours;
	pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTVInfo.iDriverCompMin = minutes;
	pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTVInfo.iDriverCompSec = seconds;

	//参数初始化
	powerNormalInit(pFlyTVInfo);
	powerOnFirstInit(pFlyTVInfo);
	powerOnSpecialEnable(pFlyTVInfo,TRUE);

	RETAILMSG(1, (TEXT("FlyAudio TV Init Build:Date%d.%d.%dTime%d:%d:%d\r\n"),
		year,months,day,hours,minutes,seconds));


	return (HANDLE)pFlyTVInfo;
}
/********************************************************************************************************
*函数名称：DWORD FTV_Open()
*函数功能：释放设备，在设备被卸载的时候调用
*输入参数：
*输出参数：
*返 回 值：返回设备卸载是否成功
**********************************************************************************************************/
BOOL FTV_Deinit(
				DWORD hDeviceContext		//XXX_Init()函数返回的设备上下文
				)
{
	P_FLY_TV_INFO pFlyTVInfo = (P_FLY_TV_INFO)hDeviceContext;
#if GLOBAL_COMM
	//全局
	pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTVInfo.bOpen = FALSE;
	pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTVInfo.bInit = FALSE;
	//SetEvent(pFlyTVInfo->hHandleGlobalGlobalEvent);
	//CloseHandle(pFlyTVInfo->hHandleGlobalGlobalEvent);
#endif
	powerOnSpecialEnable(pFlyTVInfo,FALSE);

	FreeUserBuff();
	FreeShm();

	LocalFree(pFlyTVInfo);
	RETAILMSG(1, (TEXT("FlyAudio TV LocalFree!\r\n")));
	return TRUE;
}

/********************************************************************************************************
*函数名称：DWORD FTV_Open()
*函数功能：打开设备进行读写 与应用程序的CreateFile()函数对应
*输入参数：
*输出参数：
*返 回 值：返回设备的打开上下文
**********************************************************************************************************/
DWORD FTV_Open(
			   DWORD hDeviceContext,	//设备上下文，由XXX_Init()函数创建
			   DWORD AccessCode,		//设备的访问模式，从CreateFile()函数传入
			   DWORD ShareMode			//设备的共享模式，从CreateFile()函数传入
			   )
{
	P_FLY_TV_INFO pFlyTVInfo = (P_FLY_TV_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;
	
	if (pFlyTVInfo->bOpen)
	{
		return NULL;
	}
	pFlyTVInfo->bOpen = TRUE;

	FlyTV_Enable(pFlyTVInfo,TRUE);

	pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTVInfo.bOpen = TRUE;

	RETAILMSG(1, (TEXT("FlyAudio TV Open Build:Date%d.%d.%dTime%d:%d:%d\r\n"),
		year,months,day,hours,minutes,seconds));

	return returnWhat;
}

/********************************************************************************************************
*函数名称：VOID FTV_PowerUp()
*函数功能：关闭设备 与应用程序的CloseHandle()函数对应
*输入参数：
*输出参数：
*返 回 值：返回设备关闭是否成功
**********************************************************************************************************/
BOOL FTV_Close(
			   DWORD hDeviceContext		//设备的打开上下文，由XXX_Open（）函数返回
			   )
{
	P_FLY_TV_INFO pFlyTVInfo = (P_FLY_TV_INFO)hDeviceContext;

	if (!pFlyTVInfo->bOpen)
	{
		return FALSE;
	}
	pFlyTVInfo->bOpen = FALSE;

	FlyTV_Enable(pFlyTVInfo,FALSE);
	pFlyTVInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTVInfo.bOpen = FALSE;
	RETAILMSG(1, (TEXT("FlyAudio TV Close!\r\n")));
	return TRUE;
}
/********************************************************************************************************
*函数名称：VOID FTV_PowerUp()
*函数功能：电源挂起
*输入参数：
*输出参数：
*返 回 值：无
**********************************************************************************************************/
VOID FTV_PowerUp(DWORD hDeviceContext)
{
	P_FLY_TV_INFO pFlyTVInfo = (P_FLY_TV_INFO)hDeviceContext;
	
	if (pFlyTVInfo->bOpen)
	{
		powerNormalInit(pFlyTVInfo);
	}
	
	RETAILMSG(1, (TEXT("FlyAudio TV PowerUp!\r\n")));
}
/********************************************************************************************************
*函数名称：VOID FTV_PowerDown()
*函数功能：电源挂起
*输入参数：
*输出参数：
*返 回 值：无
**********************************************************************************************************/
VOID FTV_PowerDown(DWORD hDeviceContext)
{
	P_FLY_TV_INFO pFlyTVInfo = (P_FLY_TV_INFO)hDeviceContext;

	if (pFlyTVInfo->bOpen)
	{
		powerParaDeInit(pFlyTVInfo);
	}
	RETAILMSG(1, (TEXT("FlyAudio TV PowerDown!\r\n")));
}
/********************************************************************************************************
*函数名称：DWORD FTV_Write()
*函数功能：从设备中读取数据  与应用程序ReadFile()函数据对应
*输入参数：
*输出参数：
*返 回 值：返回0表示文件结束，返回-1表示失败,返回读取的字节数表示成功
**********************************************************************************************************/
DWORD FTV_Read(
			   DWORD  hOpenContext,		//XXX_Open（）返回的设备打开上下文
			   LPVOID pBuffer,			//输出，缓冲区的指针，读取数据会被放在该缓冲区内
			   DWORD  Count				//要读取的字节数
			   )
{
	P_FLY_TV_INFO pFlyTVInfo = (P_FLY_TV_INFO)hOpenContext;

	UINT16 dwRead = 0,i;
	BYTE *buf = (BYTE *)pBuffer;

	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio TV return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}

	return dwRead;//返回数组中字节大小，不等于0时，用户程开始读数据
}
/********************************************************************************************************
*函数名称：DWORD FTV_Write()
*函数功能：向设备中写入数据  与应用程序WriteFile()函数据对应
*输入参数：
*输出参数：
*返 回 值：返回-1表示失败，返回写入的字节数表示成功
**********************************************************************************************************/
DWORD FTV_Write(
				DWORD    hOpenContext,		// XXX_Open返回的设备打开上下文
				LPCVOID  pSourceBytes,		//输入，指向要写入设备的数据的缓冲
				DWORD    NumberOfBytes		//缓冲中的数据的字节数
				)
{
	P_FLY_TV_INFO pFlyTVInfo = (P_FLY_TV_INFO)hOpenContext;
	BYTE *p;
	p = (BYTE *)pSourceBytes;
	RETAILMSG(1, (TEXT("\r\nFlyAudio TV Write %d "), NumberOfBytes));
	for(UINT i = 0;i < NumberOfBytes;i++)
	{
		RETAILMSG(1, (TEXT(" %X"), *((BYTE *)pSourceBytes + i)));
	}
	if(NumberOfBytes >= 5)
	{
		FlyTV_DealWinceCmd(pFlyTVInfo,&p[3],NumberOfBytes-4);
	}
	return NULL;
}
/********************************************************************************************************
*函数名称：DWORD FTV_Seek()
*函数功能：移动设备中的数据指针  与应用程序SetFilePointer()函数据对应
*输入参数：
*输出参数：
*返 回 值：返回数据的新指针位置，-1表示失败
**********************************************************************************************************/
DWORD FTV_Seek(
			   DWORD  hOpenContext,		//XXX_Open()返回的设备打开上下文
			   LONG   Amount,			//要移动的距离，负数表示前移，正数表示后移
			   DWORD  Type				//缓冲中的数据的字节数
			   )
{
	RETAILMSG(1, (TEXT("FlyAudio TV Seek!\r\n")));
	return NULL;
}
/********************************************************************************************************
*函数名称：
*函数功能：向驱动程序发送控制命令  与应用程序DeviceIoControl()函数据对应
*输入参数：
*输出参数：
*返 回 值：布尔值：TRUE表示操作成功，FALSE表示操作失败
**********************************************************************************************************/
BOOL FTV_IOControl(
				   DWORD  hOpenContext,		//由XXX_Open()返回的设备打开上下文
				   DWORD  dwCode,			//要发送的控制码，一个32位无符号数
				   PBYTE  pBufIn,			//输入，指向输入缓冲区的指针
				   DWORD  dwLenIn,			//输入缓冲区的长度
				   PBYTE  pBufOut,			//输出，指向输出缓冲区的指针
				   DWORD  dwLenOut,			//输出缓冲区的最大长度
				   PDWORD pdwActualOut		//输出，设备实际输出的字节数
				   )
{
	P_FLY_TV_INFO pFlyTVInfo = (P_FLY_TV_INFO)hOpenContext;
	//RETAILMSG(1, (TEXT("FlyAudio TV IOControl!\r\n")));
	BOOL bRes = TRUE;
	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	DBGD((TEXT("\r\nFlyAudio Parrot IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)"),
	//		hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	DBGD((TEXT("\r\nFlyAudio Parrot IOControl Set IOCTL_SERIAL_SET_WAIT_MASK")));
	//		break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	//DBGD((TEXT("\r\nFlyAudio Parrot IOControl Set IOCTL_SERIAL_WAIT_ON_MASK")));
	//	if (pFlyTVInfo->bOpen)
	//	{
	//		WaitForSingleObject(pFlyTVInfo->hBuffToUserDataEvent,INFINITE);
	//	} 
	//	else
	//	{
	//		WaitForSingleObject(pFlyTVInfo->hBuffToUserDataEvent,0);
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

/**********************************************************************************************************
*函数名称：	DllEntry()
*函数功能： 动态链接库的入口，每个动态链接库都需要输出这个函数，它只在动态库被加载和卸
载时被调用，也就是设备管理器调用LoadLibrary而引起它被装入内存和调用UnloadLibrary将
其从内存释放时被调用，因而它是每个动态链接库最早被调用的函数，一般用它做一些全局变量
的初始化
*输入参数：
*输出参数：
*返 回 值：布尔值：TRUE表示操作成功，FALSE表示操作失败
*************************************************************************************************************/
BOOL DllEntry(
			  HINSTANCE hinstDll,	//DLL的句柄，与一个EXE文件的句柄功能类似，一般可以通过它在得到DLL中的一
									//些资源，例如对话框，除此之外一般没什么用处。
			  DWORD dwReason,		//DLL_PROCESS_ATTACH :动态库被加载，
									//DLL_PROCESS_DETACH :动态库被释放。
			  LPVOID lpReserved		//
			  )
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		RETAILMSG(1,(TEXT("Attach in FlyAudio TV DllEntry")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		RETAILMSG(1,(TEXT("Dettach in FlyAudio TV DllEntry")));
	}

	RETAILMSG(1,(TEXT("Leaving in FlyAudio TV DllEntry")));

	return (TRUE);
}
