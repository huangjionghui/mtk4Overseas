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
	BYTE buff[] = {0x01,0x00};//0x00:�� 0x01:��
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
	return;//��System����
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
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //��ȫ����
			0,//��ʼ���߳�ջ�Ĵ�С��ȱʡΪ�����̴߳�С��ͬ
			(LPTHREAD_START_ROUTINE)ThreadExBoxRead, //�̵߳�ȫ�ֺ���
			pFlyTVInfo, //�˴�����������ܵľ��
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
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //��ȫ����
			0,//��ʼ���߳�ջ�Ĵ�С��ȱʡΪ�����̴߳�С��ͬ
			(LPTHREAD_START_ROUTINE)ThreadFlyTVProc, //�̵߳�ȫ�ֺ���
			pFlyTVInfo, //�˴�����������ܵľ��
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
	case 0x01://��ʼ������
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
	case 0x03://TV ָ��
		if (buf[1] == 0x17) {
			tvcmd = 0x50;
		}
		else {tvcmd = buf[1];}
		control_TVCmd(pFlyTVInfo,0xb4,tvcmd);
		break;
	case 0x04://���ƫ��ϵͳ
		control_FcwLdwCmd(pFlyTVInfo,buf,len);
		break;

	case 0x05:
		TVWriteFile(pFlyTVInfo,&buf[1],len-1);
		break;

	case 0xff://�����ã�����ʹ�ò����
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
*�������ƣ�WinCECommandProcessor
*�������ܣ�����WINCE������������
*���������
*���������
*�� �� ֵ��
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
*�������ƣ�HANDLE FTV_Init()
*�������ܣ������豸�����豸�����ص�ʱ�����
*���������
*���������
*�� �� ֵ��
**********************************************************************************************************/
HANDLE FTV_Init(
				DWORD dwContext   //�ַ�����ָ��ע����м�¼���������ļ�
				)
{
	RETAILMSG(1, (TEXT("\r\nTV Init Start")));

	

	//�����ڴ�
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

	//������ʼ��
	powerNormalInit(pFlyTVInfo);
	powerOnFirstInit(pFlyTVInfo);
	powerOnSpecialEnable(pFlyTVInfo,TRUE);

	RETAILMSG(1, (TEXT("FlyAudio TV Init Build:Date%d.%d.%dTime%d:%d:%d\r\n"),
		year,months,day,hours,minutes,seconds));


	return (HANDLE)pFlyTVInfo;
}
/********************************************************************************************************
*�������ƣ�DWORD FTV_Open()
*�������ܣ��ͷ��豸�����豸��ж�ص�ʱ�����
*���������
*���������
*�� �� ֵ�������豸ж���Ƿ�ɹ�
**********************************************************************************************************/
BOOL FTV_Deinit(
				DWORD hDeviceContext		//XXX_Init()�������ص��豸������
				)
{
	P_FLY_TV_INFO pFlyTVInfo = (P_FLY_TV_INFO)hDeviceContext;
#if GLOBAL_COMM
	//ȫ��
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
*�������ƣ�DWORD FTV_Open()
*�������ܣ����豸���ж�д ��Ӧ�ó����CreateFile()������Ӧ
*���������
*���������
*�� �� ֵ�������豸�Ĵ�������
**********************************************************************************************************/
DWORD FTV_Open(
			   DWORD hDeviceContext,	//�豸�����ģ���XXX_Init()��������
			   DWORD AccessCode,		//�豸�ķ���ģʽ����CreateFile()��������
			   DWORD ShareMode			//�豸�Ĺ���ģʽ����CreateFile()��������
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
*�������ƣ�VOID FTV_PowerUp()
*�������ܣ��ر��豸 ��Ӧ�ó����CloseHandle()������Ӧ
*���������
*���������
*�� �� ֵ�������豸�ر��Ƿ�ɹ�
**********************************************************************************************************/
BOOL FTV_Close(
			   DWORD hDeviceContext		//�豸�Ĵ������ģ���XXX_Open������������
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
*�������ƣ�VOID FTV_PowerUp()
*�������ܣ���Դ����
*���������
*���������
*�� �� ֵ����
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
*�������ƣ�VOID FTV_PowerDown()
*�������ܣ���Դ����
*���������
*���������
*�� �� ֵ����
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
*�������ƣ�DWORD FTV_Write()
*�������ܣ����豸�ж�ȡ����  ��Ӧ�ó���ReadFile()�����ݶ�Ӧ
*���������
*���������
*�� �� ֵ������0��ʾ�ļ�����������-1��ʾʧ��,���ض�ȡ���ֽ�����ʾ�ɹ�
**********************************************************************************************************/
DWORD FTV_Read(
			   DWORD  hOpenContext,		//XXX_Open�������ص��豸��������
			   LPVOID pBuffer,			//�������������ָ�룬��ȡ���ݻᱻ���ڸû�������
			   DWORD  Count				//Ҫ��ȡ���ֽ���
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

	return dwRead;//�����������ֽڴ�С��������0ʱ���û��̿�ʼ������
}
/********************************************************************************************************
*�������ƣ�DWORD FTV_Write()
*�������ܣ����豸��д������  ��Ӧ�ó���WriteFile()�����ݶ�Ӧ
*���������
*���������
*�� �� ֵ������-1��ʾʧ�ܣ�����д����ֽ�����ʾ�ɹ�
**********************************************************************************************************/
DWORD FTV_Write(
				DWORD    hOpenContext,		// XXX_Open���ص��豸��������
				LPCVOID  pSourceBytes,		//���룬ָ��Ҫд���豸�����ݵĻ���
				DWORD    NumberOfBytes		//�����е����ݵ��ֽ���
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
*�������ƣ�DWORD FTV_Seek()
*�������ܣ��ƶ��豸�е�����ָ��  ��Ӧ�ó���SetFilePointer()�����ݶ�Ӧ
*���������
*���������
*�� �� ֵ���������ݵ���ָ��λ�ã�-1��ʾʧ��
**********************************************************************************************************/
DWORD FTV_Seek(
			   DWORD  hOpenContext,		//XXX_Open()���ص��豸��������
			   LONG   Amount,			//Ҫ�ƶ��ľ��룬������ʾǰ�ƣ�������ʾ����
			   DWORD  Type				//�����е����ݵ��ֽ���
			   )
{
	RETAILMSG(1, (TEXT("FlyAudio TV Seek!\r\n")));
	return NULL;
}
/********************************************************************************************************
*�������ƣ�
*�������ܣ������������Ϳ�������  ��Ӧ�ó���DeviceIoControl()�����ݶ�Ӧ
*���������
*���������
*�� �� ֵ������ֵ��TRUE��ʾ�����ɹ���FALSE��ʾ����ʧ��
**********************************************************************************************************/
BOOL FTV_IOControl(
				   DWORD  hOpenContext,		//��XXX_Open()���ص��豸��������
				   DWORD  dwCode,			//Ҫ���͵Ŀ����룬һ��32λ�޷�����
				   PBYTE  pBufIn,			//���룬ָ�����뻺������ָ��
				   DWORD  dwLenIn,			//���뻺�����ĳ���
				   PBYTE  pBufOut,			//�����ָ�������������ָ��
				   DWORD  dwLenOut,			//�������������󳤶�
				   PDWORD pdwActualOut		//������豸ʵ��������ֽ���
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
*�������ƣ�	DllEntry()
*�������ܣ� ��̬���ӿ����ڣ�ÿ����̬���ӿⶼ��Ҫ��������������ֻ�ڶ�̬�ⱻ���غ�ж
��ʱ�����ã�Ҳ�����豸����������LoadLibrary����������װ���ڴ�͵���UnloadLibrary��
����ڴ��ͷ�ʱ�����ã��������ÿ����̬���ӿ����类���õĺ�����һ��������һЩȫ�ֱ���
�ĳ�ʼ��
*���������
*���������
*�� �� ֵ������ֵ��TRUE��ʾ�����ɹ���FALSE��ʾ����ʧ��
*************************************************************************************************************/
BOOL DllEntry(
			  HINSTANCE hinstDll,	//DLL�ľ������һ��EXE�ļ��ľ���������ƣ�һ�����ͨ�����ڵõ�DLL�е�һ
									//Щ��Դ������Ի��򣬳���֮��һ��ûʲô�ô���
			  DWORD dwReason,		//DLL_PROCESS_ATTACH :��̬�ⱻ���أ�
									//DLL_PROCESS_DETACH :��̬�ⱻ�ͷš�
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
