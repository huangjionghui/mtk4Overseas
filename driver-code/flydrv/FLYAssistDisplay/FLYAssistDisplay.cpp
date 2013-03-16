// FLYAssistDisplay.cpp : Defines the entry point for the DLL application.
//

#define GLOBAL_COMM	1

#include "flyhwlib.h"
#include "flyuserlib.h"
#include "flyseriallib.h"
#include "FLYAssistDisplay.h"



void FlyAssistDisplay_Enable(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BOOL bEnable);
VOID FAD_PowerUp(DWORD hDeviceContext);
VOID FAD_PowerDown(DWORD hDeviceContext);
P_FLY_ASSISTDISPLAY_INFO gpAssistDisplayInfo = NULL;

VOID WriteToAssistCom(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BYTE *p,UINT len)
{
	//DWORD bufSendLength;
	////DBGD((TEXT("\r\nWrite to AssistCom:")));
	//UINT i;
	//for (i = 0;i < len;i++)//跳过换行符
	//{
	//	//DBGD((TEXT(" %X"),p[i]));
	//}
	//WriteFile(pAssistDisplayInfo->hAssistComm,p,len,&bufSendLength,NULL);
}

//static void DealBarCodeID(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BYTE data)
//{
//	static unsigned char checksum;
//	static unsigned char FrameLen;
//	static unsigned char FrameStatus;
//	static unsigned char FrameLenMax;
//	static unsigned char ReceiveReg[256];
//
//	switch (FrameStatus)
//	{
//	case 0:
//		if (data == 0xff) 
//		{
//			FrameStatus = 1;
//		}
//		break;
//	case 1:
//		if (data == 0x55) 
//		{
//			FrameStatus = 2;
//		}
//		else 
//		{
//			FrameStatus = 0;
//			pAssistDisplayInfo->ExBoxInfoFrameStatus =0;
//
//		}
//		break;
//	case 2:
//		if(data == 0)
//		{
//			FrameStatus =0;
//			pAssistDisplayInfo->ExBoxInfoFrameStatus =0;
//		}
//		else
//		{
//			FrameStatus = 3;
//			checksum = FrameLenMax = data;
//			FrameLen = 0;
//		}
//		break;
//	case 3:
//		if(FrameLen < (FrameLenMax))
//		{
//			ReceiveReg[FrameLen] = data;
//			checksum += data;
//			FrameLen ++;
//		}
//		else
//		{
//			if((BYTE)(0x100-data) == checksum)
//			{
//				ReceiveReg[FrameLen] = 0;
//				pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.BarCodeID[0]=FrameLen-2;
//				memcpy((void*)&pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.BarCodeID[1],&ReceiveReg[2],FrameLen-2);
//
//				IpcStartEvent(EVENT_GLOBAL_RETURN_BAR_CODE_ID);
//
//				DBGD((TEXT("\r\nFlyAudio BarCode ID :")));
//
//					for(BYTE i=0;i<pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.BarCodeID[0];i++)
//					{
//						DBGD((TEXT("%d  "),pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.BarCodeID[i]));		
//					}
//
//			}
//			else
//			{
//				DBGD((TEXT("\r\nFlyAudio BarCode ID CRC ERROR")));
//			}
//			FrameStatus = 0;
//			pAssistDisplayInfo->ExBoxInfoFrameStatus =0;
//		}
//		break;
//	default:			
//		FrameStatus = 0;
//		pAssistDisplayInfo->ExBoxInfoFrameStatus =0;
//		break;
//	}
//}
//static VOID OnCommRecv(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo, BYTE *buf, UINT buflen)
//{
//	for (UINT i = 0; i < buflen; i++)
//	{
//		DBGD((TEXT("\r\nFlyAudio Exbox read_data %X"),buf[i]));
//			//RETAILMSG(1, (TEXT("\r\nFlyAudio Exbox read_data %X"),buf[i])
//		switch (pAssistDisplayInfo->ExBoxInfoFrameStatus)
//		{
//			case 0://分流
//				if ((buf[i] & 0xf0) == 0xe0)
//				{
//					pAssistDisplayInfo->ExBoxInfoFrameStatus = 1;
//				}
//				else if ((buf[i] & 0xf0) == 0xf0)
//				{
//					if (buf[i] == 0xf0)
//					{
//						pAssistDisplayInfo->ExBoxInfoFrameStatus = 5;
//					}
//					else if(buf[i] == 0xff)//####################
//					{
//						pAssistDisplayInfo->ExBoxInfoFrameStatus = 6;
//						DealBarCodeID(pAssistDisplayInfo,buf[i]);
//					}
//					else
//					{
//						pAssistDisplayInfo->ExBoxInfoFrameStatus = 2;
//					}
//
//				}
//				else if ((buf[i] & 0xf0) == 0x80)
//				{
//					pAssistDisplayInfo->ExBoxInfoFrameStatus = 3;
//				}
//				else if (buf[i] == 0xaa)
//				{
//					pAssistDisplayInfo->ExBoxInfoFrameStatus = 4;
//				}
//				else
//				{
//					pAssistDisplayInfo->ExBoxInfoFrameStatus = 0;
//				}
//				//record data1
//				if (pAssistDisplayInfo->ExBoxInfoFrameStatus != 0)
//				{
//					pAssistDisplayInfo->bRevData[0] = buf[i];
//				}
//				break;
//				pAssistDisplayInfo->ExBoxInfoFrameStatus = 0;
//				break;
//			case 6:
//				DealBarCodeID(pAssistDisplayInfo,buf[i]);
//				break;
//			default:
//				pAssistDisplayInfo->ExBoxInfoFrameStatus = 0;
//				break;
//		}
//	}
//}
//static DWORD WINAPI ThreadFlyAssistCommProc(LPVOID pContext)
//{
//	P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo = (P_FLY_ASSISTDISPLAY_INFO)pContext;
//
//	UINT32 ret = 0;
//	BYTE *recvBuf = new BYTE[DATA_BUFF_COMM_LENGTH];
//	BYTE *zeroBuf = new BYTE[DATA_BUFF_COMM_LENGTH];
//	memset(zeroBuf,0,sizeof(BYTE)*DATA_BUFF_COMM_LENGTH);
//
//
//	while (!pAssistDisplayInfo->bKillFlyhAssistCommThread)
//	{
//		ret = ReadSerial(pAssistDisplayInfo->hAssistComm,recvBuf,DATA_BUFF_COMM_LENGTH);
//		if(ret <= 0)
//		{
//			//不能从串口读取数据
//			DBGI((TEXT("\r\nFlyAudio AssistDisplay ComProcThread cannot read data!")));
//		}
//		else
//		{
//			DBGD((TEXT("\r\nAssistDisplay Serial Read %d bytes:"),ret));
//			for (UINT i = 0;i < ret;i++)
//			{
//				DBGD((TEXT(" %x"),recvBuf[i]));
//			}
//
//			if (ret > 50)
//			{
//				//多判断几下
//				if (!memcmp(zeroBuf,recvBuf,10) 
//					|| (!memcmp(&zeroBuf[15],&recvBuf[15],10))
//					|| (!memcmp(&zeroBuf[30],&recvBuf[30],10)))
//				{
//					DBGD((TEXT("\r\nAssistDisplay Zero buf,coutinue read\r\n")));
//					continue;
//				}
//			}
//
//			//把数据赋值给全局变量
//			OnCommRecv(pAssistDisplayInfo,recvBuf,ret);
//		}  
//	}
//	delete []recvBuf;
//	delete []zeroBuf;
//
//	pAssistDisplayInfo->hThreadHandleFlyhhAssistComm= NULL;
//	RETAILMSG(1, (TEXT("\r\nThreadFlyAssistCommProc exit")));
//	return 0;
//}
//
//static VOID closeComm(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo)
//{
//	if (pAssistDisplayInfo->hThreadHandleFlyhhAssistComm)
//	{
//		CloseHandle(pAssistDisplayInfo->hThreadHandleFlyhhAssistComm);
//		pAssistDisplayInfo->hThreadHandleFlyhhAssistComm = NULL;
//	}
//
//	if (pAssistDisplayInfo->hAssistComm )
//	{
//		CloseSerial(pAssistDisplayInfo->hAssistComm);
//		pAssistDisplayInfo->hAssistComm  = NULL;
//	}
//}
//
//static VOID openComm(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo)
//{
//	if (pAssistDisplayInfo->hAssistComm)
//	{
//		return;
//	}
//
//	//打开串口
//	pAssistDisplayInfo->hAssistComm =OpenSerial(_T("COM5:"), 19200);
//	DBGI((TEXT("FlyAudio AssistDisplay open com5!")));
//	if(pAssistDisplayInfo->hAssistComm== INVALID_HANDLE_VALUE)
//	{
//		pAssistDisplayInfo->hAssistComm = NULL;
//		DBGE((TEXT("FlyAudio AssistDisplay cann't open com!")));
//		return;
//	}
//
//	pAssistDisplayInfo->bKillFlyhAssistCommThread= FALSE;
//
//	DWORD dwThreadID;
//	pAssistDisplayInfo->hThreadHandleFlyhhAssistComm= 
//		CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
//		0,//初始化线程栈的大小，缺省为与主线程大小相同
//		(LPTHREAD_START_ROUTINE)ThreadFlyAssistCommProc, //线程的全局函数
//		pAssistDisplayInfo, //此处传入了主框架的句柄
//		0, &dwThreadID );
//	DBGE((TEXT("\r\nThreadFlyAssistCommProc ID:%x"),dwThreadID));
//
//	if (NULL == pAssistDisplayInfo->hThreadHandleFlyhhAssistComm)
//	{
//		DBGE((TEXT("FlyAudio Assist CreateThread ThreadFlyAssistCommProc!")));
//		closeComm(pAssistDisplayInfo);
//		return;
//	}
//	DBGE((TEXT("FlyAudio Assist Comm init OK!")));
//}

void returnAssistDisplayPowerMode(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BOOL bPower)
{
	BYTE buf[] = {0x01,0x00};

	if (bPower)
	{
		buf[1] = 0x01;
	}

	ReturnToUser(buf,2);
}

void returnAssistDisplayWorkMode(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BOOL bWork)
{
	BYTE buf[] = {0x02,0x00};

	if (bWork)
	{
		buf[1] = 0x01;
	}

	ReturnToUser(buf,2);
}

void returnAssistDisplaybHave(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BOOL bHave)
{
	BYTE buf[] = {0x10,0x00};

	if (bHave)
	{
		buf[1] = 0x01;
	}

	ReturnToUser(buf,2);
}

static void assistDisplayWriteFile(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BYTE *p,UINT length)
{
	if (pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.bInit)
	{
		if (FALSE == WriteMsgQueue(pAssistDisplayInfo->hMsgQueueFromAssistDisplayCreate, p, length, 0, 0))
		{
			DWORD dwError = GetLastError();
			DBGE((TEXT("\r\nFlyAudio AssistDisplay Write MsgQueue Error!0x%x"),dwError));
		}
	}
	/*if(pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName == CAR_CRV_12)
	{
		DealCarLcdInfo(pAssistDisplayInfo,p,length);			
	}*/
}

static DWORD WINAPI ThreadInter(LPVOID pContext)
{
	P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo = (P_FLY_ASSISTDISPLAY_INFO)pContext;
	ULONG WaitReturn;
	UINT waitTime=200;

	while (!pAssistDisplayInfo->bKillDispatchFlyInterThread)
	{
		WaitReturn = WaitForSingleObject(pAssistDisplayInfo->hDispatchInterThreadEvent, waitTime);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nFlyAudio AssistDisplay hDispatchInterThreadEvent Error")));
		}
		/*if(pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName == CAR_CRV_12)
		{
			SendDataToCivic(pAssistDisplayInfo);
		}*/
	}
	pAssistDisplayInfo->FLyInterThreadHandle = NULL;
	DBGE((TEXT("\r\nFlyAudio AssistDisplay ThreadInter exit")));
	return 0;
}

static DWORD WINAPI ThreadMain(LPVOID pContext)
{
	P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo = (P_FLY_ASSISTDISPLAY_INFO)pContext;
	ULONG WaitReturn;

	pAssistDisplayInfo->bPowerUp = TRUE;

	while (!pAssistDisplayInfo->bKillDispatchFlyMainThread)
	{
		WaitReturn = WaitForSingleObject(pAssistDisplayInfo->hDispatchMainThreadEvent, INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nFlyAudio AssistDisplay hDispatchMainThreadEvent Error")));
		}
		if(IpcWhatEvent(EVENT_GLOBAL_DEALED_CARNAME_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_DEALED_CARNAME_ID);
			/*if(pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName == CAR_CRV_12)
			{
				openComm(pAssistDisplayInfo);
			}*/
		}
		
	}

	while (pAssistDisplayInfo->FLyInterThreadHandle)
	{
		SetEvent(pAssistDisplayInfo->hDispatchInterThreadEvent);
		Sleep(10);
	}
	CloseHandle(pAssistDisplayInfo->hDispatchInterThreadEvent);

	CloseHandle(pAssistDisplayInfo->hDispatchMainThreadEvent);
	DBGE((TEXT("\r\nFlyAudio AssistDisplay ThreadMain exit")));
	return 0;
}

void FlyAssistDisplay_Enable(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BOOL bEnable)
{
	DWORD dwThreadID;

	if (bEnable)
	{
		if (pAssistDisplayInfo->bPower)
		{
			return;
		}
		pAssistDisplayInfo->bPower = TRUE;

		pAssistDisplayInfo->bKillDispatchFlyMainThread = FALSE;
		pAssistDisplayInfo->FlyMainThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadMain, //线程的全局函数
			pAssistDisplayInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGI((TEXT("\r\nAssistDisplay ThreadMain ID %x!"),dwThreadID));
		if (NULL == pAssistDisplayInfo->FlyMainThreadHandle)
		{
			pAssistDisplayInfo->bKillDispatchFlyMainThread = TRUE;
			return;
		}
		pAssistDisplayInfo->hDispatchMainThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	

		pAssistDisplayInfo->bKillDispatchFlyInterThread = FALSE;
		pAssistDisplayInfo->FLyInterThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadInter, //线程的全局函数
			pAssistDisplayInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGI((TEXT("\r\nAssistDisplay ThreadInter ID %x!"),dwThreadID));
		if (NULL == pAssistDisplayInfo->FLyInterThreadHandle)
		{
			pAssistDisplayInfo->bKillDispatchFlyInterThread = TRUE;
			return;
		}
		pAssistDisplayInfo->hDispatchInterThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
	}
	else
	{
		if (!pAssistDisplayInfo->bPower)
		{
			return;
		}
		pAssistDisplayInfo->bPower = FALSE;
		pAssistDisplayInfo->bPowerUp = FALSE;

		pAssistDisplayInfo->bKillDispatchFlyMainThread = TRUE;
		SetEvent(pAssistDisplayInfo->hDispatchMainThreadEvent);

		pAssistDisplayInfo->bKillDispatchFlyInterThread = TRUE;
		SetEvent(pAssistDisplayInfo->hDispatchInterThreadEvent);
	}
}

static void buffSendToBuff(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BYTE ID,BYTE *pData,UINT len)
{
	memcpy(pAssistDisplayInfo->iSendToMCUBuff[ID],pData,len);
	pAssistDisplayInfo->iSendToMCULength[ID] = len;
}

static void buffSendToMCU(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BYTE ID)
{
	if (pAssistDisplayInfo->iSendToMCULength[ID])
	{
		assistDisplayWriteFile(pAssistDisplayInfo,pAssistDisplayInfo->iSendToMCUBuff[ID],pAssistDisplayInfo->iSendToMCULength[ID]);
	}
}

static void buffChangeInput(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,eAssistDisplayChannel eInput)
{
	UINT i=0;
	if (eAD_DVD == eInput)
	{
		buffSendToMCU(pAssistDisplayInfo,0x20);
		buffSendToMCU(pAssistDisplayInfo,0x21);
		buffSendToMCU(pAssistDisplayInfo,0x22);
		buffSendToMCU(pAssistDisplayInfo,0x23);

		DBGI((TEXT("\r\nAssistDisplay xxxxxx ID %d"),pAssistDisplayInfo->iSendToMCUBuff[0x23][1]));
		

		//DVD 没碟
		if (0x00 == pAssistDisplayInfo->iSendToMCUBuff[0x24][1])
		{
			buffSendToMCU(pAssistDisplayInfo,0x24);
		}

		/*
		//DVD 没碟 stop
		if (0x00 == pAssistDisplayInfo->iSendToMCUBuff[0x23][1] &&
			0x00 == pAssistDisplayInfo->iSendToMCUBuff[0x24][1])
		{
			buffSendToMCU(pAssistDisplayInfo,0x24);
		}
		*/

		/*
		//0x23:0x2--pause
		if (0x02 != pAssistDisplayInfo->iSendToMCUBuff[0x23][1] &&
			0x01 != pAssistDisplayInfo->iSendToMCUBuff[0x23][1])
		{
			buffSendToMCU(pAssistDisplayInfo,0x24);
		}
		*/

		//buffSendToMCU(pAssistDisplayInfo,0x25);
	}
	else if (eAD_Radio == eInput)
	{
		buffSendToMCU(pAssistDisplayInfo,0x30);
		buffSendToMCU(pAssistDisplayInfo,0x31);
		buffSendToMCU(pAssistDisplayInfo,0x32);
		buffSendToMCU(pAssistDisplayInfo,0x33);
	}
	else if(eAD_iPod == eInput)
	{
		buffSendToMCU(pAssistDisplayInfo,0x40);
		buffSendToMCU(pAssistDisplayInfo,0x41);
		buffSendToMCU(pAssistDisplayInfo,0x42);
	}
	else if(eAD_AUX == eInput)
	{

	}
	else if(eAD_MP3 == eInput)
	{
		buffSendToMCU(pAssistDisplayInfo,0x50);
		buffSendToMCU(pAssistDisplayInfo,0x51);
		buffSendToMCU(pAssistDisplayInfo,0x52);
	}
	else if(eAD_XM == eInput)
	{
		buffSendToMCU(pAssistDisplayInfo,0x60);
		buffSendToMCU(pAssistDisplayInfo,0x61);
		buffSendToMCU(pAssistDisplayInfo,0x62);
	}
	else if(eAD_BT == eInput)
	{
		buffSendToMCU(pAssistDisplayInfo,0x70);
		buffSendToMCU(pAssistDisplayInfo,0x71);
	}

}

static VOID WinCECommandProcessor(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BYTE *buff,UINT len)
{
	switch (buff[0])
	{
	case 0x01:
		if (0x01 == buff[1])
		{
			returnAssistDisplayPowerMode(pAssistDisplayInfo,TRUE);
			returnAssistDisplayWorkMode(pAssistDisplayInfo,TRUE);
			returnAssistDisplaybHave(pAssistDisplayInfo,TRUE);
		}
		else
		{
			returnAssistDisplayPowerMode(pAssistDisplayInfo,FALSE);
		}
		break;
	//case 0xFF:
	//	if (0x01 == buff[1])
	//	{
	//		FAD_PowerUp((DWORD)pAssistDisplayInfo);
	//	} 
	//	else if (0x00 == buff[1])
	//	{
	//		FAD_PowerDown((DWORD)pAssistDisplayInfo);
	//	}
	//	break;
	default:
		buffSendToBuff(pAssistDisplayInfo,buff[0],&buff[0],len);
		assistDisplayWriteFile(pAssistDisplayInfo,&buff[0],len);
		if (0x11 == buff[0])
		{
			buffChangeInput(pAssistDisplayInfo,(eAssistDisplayChannel)buff[1]);
		}
		break;
	}
}

static void powerNormalInit(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo)
{
	memset(pAssistDisplayInfo->iSendToMCULength,0,256);
	memset(pAssistDisplayInfo->iSendToMCUBuff,0,256*256);
}

void IpcRecv(UINT32 iEvent)
{
	P_FLY_ASSISTDISPLAY_INFO	pAssistDisplayInfo = (P_FLY_ASSISTDISPLAY_INFO)gpAssistDisplayInfo;

	DBGD((TEXT("\r\nFLY Display Recv IPC iEvent:%d\r\n"),iEvent));

	SetEvent(pAssistDisplayInfo->hDispatchMainThreadEvent);
}

HANDLE FAD_Init(DWORD dwContext)
{
	P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo;

	RETAILMSG(1, (TEXT("\r\nAssistDisplay Init Start")));

	// 加载驱动并取得函数指针数组的地址(存放了要引出的底层函数的指针)
	pAssistDisplayInfo = (P_FLY_ASSISTDISPLAY_INFO)LocalAlloc(LPTR, sizeof(FLY_ASSISTDISPLAY_INFO));
	if (!pAssistDisplayInfo)
	{
		return NULL;
	}

	gpAssistDisplayInfo = pAssistDisplayInfo;

	pAssistDisplayInfo->pFlyShmGlobalInfo = CreateShm(DIS_MODULE,IpcRecv);
	if (NULL == pAssistDisplayInfo->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio Display Init create shm err\r\n")));
		return NULL;
	}

	//if (!GetDllAddrTable())
	//{
	//	DBGE((TEXT("FlyAudio Display  GetDllAddrTable err\r\n")));
	//	return NULL;
	//}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio Display Open create user buf err\r\n")));
		return NULL;
	}


	//参数初始化
	pAssistDisplayInfo->bOpen = FALSE;
	pAssistDisplayInfo->bPower = FALSE;
	pAssistDisplayInfo->ExBoxInfoFrameStatus =0;

	pAssistDisplayInfo->bKillDispatchFlyMainThread = TRUE;
	pAssistDisplayInfo->FlyMainThreadHandle = NULL;
	pAssistDisplayInfo->hDispatchMainThreadEvent = NULL;
	pAssistDisplayInfo->buffToUserHx = 0;
	pAssistDisplayInfo->buffToUserLx = 0;
	pAssistDisplayInfo->hBuffToUserDataEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	InitializeCriticalSection(&pAssistDisplayInfo->hCSSendToUser);

	pAssistDisplayInfo->bKillDispatchFlyInterThread = TRUE;
	pAssistDisplayInfo->FLyInterThreadHandle = NULL;
	pAssistDisplayInfo->hDispatchInterThreadEvent = NULL;

	powerNormalInit(pAssistDisplayInfo);

	
	if (pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalStructSize
		!= sizeof(struct shm))
	{
		pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[0] = 'F';
		pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[1] = 'A';
		pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[2] = 'D';
		DBGE((TEXT("\r\nGlobal Struct Size Error:%d %d"),pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalStructSize,sizeof(FLY_DRIVER_GLOBAL)));
	}

	pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAssistDisplayInfo.bInit = TRUE;
	pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAssistDisplayInfo.iDriverCompYear = year;
	pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAssistDisplayInfo.iDriverCompMon = months;
	pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAssistDisplayInfo.iDriverCompDay = day;
	pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAssistDisplayInfo.iDriverCompHour = hours;
	pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAssistDisplayInfo.iDriverCompMin = minutes;
	pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAssistDisplayInfo.iDriverCompSec = seconds;

	MSGQUEUEOPTIONS  msgOpts;

	msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
	msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
	msgOpts.dwMaxMessages = FLY_MSG_QUEUE_FROM_ASSISTDISPLAY_ENTRIES;
	msgOpts.cbMaxMessage = FLY_MSG_QUEUE_FROM_ASSISTDISPLAY_LENGTH;
	msgOpts.bReadAccess = FALSE;
	pAssistDisplayInfo->hMsgQueueFromAssistDisplayCreate = CreateMsgQueue(FLY_MSG_QUEUE_FROM_ASSISTDISPLAY_NAME, &msgOpts);
	if (NULL == pAssistDisplayInfo->hMsgQueueFromAssistDisplayCreate)
	{
		DBGE((TEXT("\r\nFlyAudio AssistDisplay Create MsgQueue From AssistDisplay Fail!")));
	}

	DBGE((TEXT("\r\nFlyAudio AssistDisplay Init Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return (HANDLE)pAssistDisplayInfo;
}

BOOL
FAD_Deinit(DWORD hDeviceContext)
{
	P_FLY_ASSISTDISPLAY_INFO	pAssistDisplayInfo = (P_FLY_ASSISTDISPLAY_INFO)hDeviceContext;
	CloseHandle(pAssistDisplayInfo->hBuffToUserDataEvent);

#if GLOBAL_COMM
	//全局
	pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAssistDisplayInfo.bInit = FALSE;
#endif

	FreeShm();
	//FreeSocketServer();
	FreeUserBuff();
	CloseMsgQueue(pAssistDisplayInfo->hMsgQueueFromAssistDisplayCreate);

	DBGE((TEXT("FlyAudio AssistDisplay DeInit\r\n")));
	LocalFree(pAssistDisplayInfo);
	return TRUE;
}

DWORD
FAD_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	P_FLY_ASSISTDISPLAY_INFO	pAssistDisplayInfo = (P_FLY_ASSISTDISPLAY_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;

	if (pAssistDisplayInfo->bOpen)
	{
		return NULL;
	}
	pAssistDisplayInfo->bOpen = TRUE;

	FlyAssistDisplay_Enable(pAssistDisplayInfo,TRUE);

#if GLOBAL_COMM
	//全局
	pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAssistDisplayInfo.bOpen = TRUE;
#endif

	DBGI((TEXT("\r\nFlyAudio AssistDisplay Open Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return returnWhat;
}

BOOL
FAD_Close(DWORD hDeviceContext)
{
	P_FLY_ASSISTDISPLAY_INFO	pAssistDisplayInfo = (P_FLY_ASSISTDISPLAY_INFO)hDeviceContext;

	FlyAssistDisplay_Enable(pAssistDisplayInfo,FALSE);

#if GLOBAL_COMM
	//全局
	pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAssistDisplayInfo.bOpen = FALSE;
#endif

	pAssistDisplayInfo->bOpen = FALSE;
	SetEvent(pAssistDisplayInfo->hBuffToUserDataEvent);

	DBGE((TEXT("FlyAudio AssistDisplay Close\r\n")));
	return TRUE;
}

VOID
FAD_PowerUp(DWORD hDeviceContext)
{
	P_FLY_ASSISTDISPLAY_INFO	pAssistDisplayInfo = (P_FLY_ASSISTDISPLAY_INFO)hDeviceContext;

	powerNormalInit(pAssistDisplayInfo);

	DBGE((TEXT("FlyAudio AssistDisplay PowerUp\r\n")));
}

VOID
FAD_PowerDown(DWORD hDeviceContext)
{
	P_FLY_ASSISTDISPLAY_INFO	pAssistDisplayInfo = (P_FLY_ASSISTDISPLAY_INFO)hDeviceContext;

	DBGE((TEXT("FlyAudio AssistDisplay PowerDown\r\n")));
}

DWORD
FAD_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	P_FLY_ASSISTDISPLAY_INFO	pAssistDisplayInfo = (P_FLY_ASSISTDISPLAY_INFO)hOpenContext;

	UINT dwRead = 0,i;
	BYTE *buf = (BYTE *)pBuffer;
	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio AssistDisplay return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}

	return dwRead;
}

DWORD
FAD_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	P_FLY_ASSISTDISPLAY_INFO	pAssistDisplayInfo = (P_FLY_ASSISTDISPLAY_INFO)hOpenContext;
	DBGD((TEXT("\r\nFlyAudio AssistDisplay Write %d "),NumberOfBytes));
	for(UINT i = 0;i < NumberOfBytes;i++)
	{
		DBGD((TEXT(" %X"),*((BYTE *)pSourceBytes + i)));
	}
	if(NumberOfBytes)
	{
		WinCECommandProcessor(pAssistDisplayInfo,(((BYTE *)pSourceBytes)+3),((BYTE *)pSourceBytes)[2]);
	}

	return NULL;
}

DWORD
FAD_Seek(DWORD hOpenContext, LONG Amount, DWORD  Type)
{
	return NULL;
}

BOOL
FAD_IOControl(DWORD hOpenContext,
			  DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn,
			  PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
	P_FLY_ASSISTDISPLAY_INFO	pAssistDisplayInfo = (P_FLY_ASSISTDISPLAY_INFO)hOpenContext;

	BOOL bRes = TRUE;

	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	DBG1(RETAILMSG(1, (TEXT("\r\nAssistDisplay IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)"),
	//		hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	DBG1(RETAILMSG(1, (TEXT("\r\nXMRadio IOControl Set IOCTL_SERIAL_SET_WAIT_MASK")));
	//		break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	//DBG1(RETAILMSG(1, (TEXT("\r\nXMRadio IOControl Set IOCTL_SERIAL_WAIT_ON_MASK")));
	//	if (pAssistDisplayInfo->bOpen)
	//	{
	//		WaitForSingleObject(pAssistDisplayInfo->hBuffToUserDataEvent,INFINITE);
	//	} 
	//	else
	//	{
	//		WaitForSingleObject(pAssistDisplayInfo->hBuffToUserDataEvent,0);
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
		DEBUGMSG(1, (TEXT("Attach in FlyAssistDisplay DllEntry\n")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}

	if (dwReason == DLL_PROCESS_DETACH)
	{
		DEBUGMSG(1, (TEXT("Dettach in FlyAssistDisplay DllEntry\n")));
	}

	DEBUGMSG(1, (TEXT("Leaving FlyAssistDisplay DllEntry\n")));

	return (TRUE);
}
