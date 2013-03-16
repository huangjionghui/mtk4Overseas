// -----------------------------------------------------------------------------
// File Name    : FLYTPMS.cpp
// Title        : FLYTPMS Driver
// Author       : Mootall - Copyright (C) 2011
// Created      : 2011-06-14  
// Version      : 0.01
// Target MCU   : WinCE Driver
// -----------------------------------------------------------------------------
// Version History:
/*
>>>
>>>
*/
// FLYTPMS.cpp : Defines the entry point for the DLL application.

#include "FLYTPMS.h"
#include "flyuserlib.h"

#define GLOBAL_COMM	1

P_FLY_TPMS_INFO gpFlyTPMSInfo=NULL;


VOID FTP_PowerUp(DWORD hDeviceContext);
VOID FTP_PowerDown(DWORD hDeviceContext);


static void returnPowerStatus(P_FLY_TPMS_INFO pFlyTPMSInfo,BOOL bPower)
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
static void returnWorkMode(P_FLY_TPMS_INFO pFlyTPMSInfo,BOOL bWork)
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
static void returnTPMSConnectStatus(P_FLY_TPMS_INFO pFlyTPMSInfo,BOOL bHave)
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
static void returnTPMSPairStatus(P_FLY_TPMS_INFO pFlyTPMSInfo,BYTE pair)
{
	BYTE buff[2];
	buff[0] = 0x10;
	buff[1] = pair;
	ReturnToUser(buff,2);
}
//p 0:FL 1:FR 2:RL 3:RR
//t 0:POWER  1: UPDATA 2:PRESSURE  3:TEMPERATURE
//v value
static VOID returnAllInfo(P_FLY_TPMS_INFO pFlyTPMSInfo,BYTE p,BYTE t,BYTE v)
{
	BYTE buff[3];
	buff[0] = 0x20 | p;
	buff[1] = t;
	buff[2] = v;
	ReturnToUser(buff,3);
}
static VOID powerParaDeInit(P_FLY_TPMS_INFO pFlyTPMSInfo)
{
	pFlyTPMSInfo->bPower = FALSE;

	pFlyTPMSInfo->iBuffToUserLx = 0;
	pFlyTPMSInfo->iBuffToUserHx = 0;

	pFlyTPMSInfo->bPrePairStatus = FALSE;
	pFlyTPMSInfo->bCurPairStatus = FALSE;
}

static VOID powerNormalInit(P_FLY_TPMS_INFO pFlyTPMSInfo)
{
	pFlyTPMSInfo->iBuffToUserLx = 0;
	pFlyTPMSInfo->iBuffToUserHx = 0;	

	pFlyTPMSInfo->bHavaTMPS = FALSE;
	
	pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTPMSModule = FALSE;
	pFlyTPMSInfo->bPrePairStatus = FALSE;
	pFlyTPMSInfo->bCurPairStatus = FALSE;
}
static VOID powerOnFirstInit(P_FLY_TPMS_INFO pFlyTPMSInfo)
{
	pFlyTPMSInfo->bOpen = FALSE;
	pFlyTPMSInfo->bPower = FALSE;
	pFlyTPMSInfo->bSpecialPower = FALSE;

	pFlyTPMSInfo->hMsgQueueTPMSToExBoxCreate = NULL;
	pFlyTPMSInfo->hMsgQueueExBoxToTPMSCreate = NULL;
	pFlyTPMSInfo->bKillDispatchFlyMsgQueueEXBOXReadThread = TRUE;
	pFlyTPMSInfo->FLyMsgQueueEXBOXReadThreadHandle = NULL;

	pFlyTPMSInfo->bKillDispatchFlyTPMSThread = TRUE;
	pFlyTPMSInfo->hThreadHandleFlyTPMS = NULL;
	pFlyTPMSInfo->hDispatchThreadTPMSEvent = NULL;		
}
static VOID FlyTPMS_infoProcessor(P_FLY_TPMS_INFO pFlyTPMSInfo, BYTE data1, BYTE data2)
{
	BYTE position0;
	BYTE type0;
	BYTE value0;
	if (data1 == 0xe1) {
		if (data2 == 0x10) { //Learn
			returnTPMSPairStatus(pFlyTPMSInfo,1);
			DBGE((TEXT("\r\nFlyAudio TPMS Learning\r\n")));
		}
		else if (data2 == 0x20){ //learn end
			returnTPMSPairStatus(pFlyTPMSInfo,0);
			pFlyTPMSInfo->bPrePairStatus = FALSE;
			pFlyTPMSInfo->bCurPairStatus = FALSE;
			DBGE((TEXT("\r\nFlyAudio TPMS Learn end\r\n")));
		}
		else {
			position0 = (data1>>2)&0x03; //0:FL 1:FR 2:RL 3:RR
			type0 = data1&0x03;	   //0:POWER  1: UPDATA 2:PRESSURE  3:TEMPERATURE
			value0 = data2;		   //value
			returnAllInfo(pFlyTPMSInfo,position0,type0,value0);
		}
	}
	else
	{
		position0 = (data1>>2)&0x03; //0:FL 1:FR 2:RL 3:RR
		type0 = data1&0x03;	   //0:POWER  1: UPDATA 2:PRESSURE  3:TEMPERATURE
		value0 = data2;		   //value
		returnAllInfo(pFlyTPMSInfo,position0,type0,value0);
	}
}
static DWORD WINAPI ThreadExBoxRead(LPVOID pContext)
{
	P_FLY_TPMS_INFO pFlyTPMSInfo = (P_FLY_TPMS_INFO)pContext;
	ULONG WaitReturn;
	BYTE buff[FLY_MSG_QUEUE_EXBOX_TO_TPMS_LENGTH];
	DWORD dwRes;
	DWORD dwMsgFlag;

	while (!pFlyTPMSInfo->bKillDispatchFlyMsgQueueEXBOXReadThread)
	{
		WaitReturn = WaitForSingleObject(pFlyTPMSInfo->hMsgQueueExBoxToTPMSCreate, INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nFlyAudio TPMS ThreadRead Event hMsgQueueFromCOMxCreate Error")));
		}
		ReadMsgQueue(pFlyTPMSInfo->hMsgQueueExBoxToTPMSCreate, buff, FLY_MSG_QUEUE_EXBOX_TO_TPMS_LENGTH, &dwRes, 0, &dwMsgFlag);
		if(dwRes)
		{
			DBGD((TEXT("\r\nFlyAudio TPMS Read MsgQueue From ExBox --->%d %x %x"),dwRes,buff[0],buff[1]));
			FlyTPMS_infoProcessor(pFlyTPMSInfo,buff[0],buff[1]);
		}
	}
	pFlyTPMSInfo->FLyMsgQueueEXBOXReadThreadHandle = NULL;
	DBGE((TEXT("\r\nFlyAudio TPMS ThreadCOMxRead exit")));
		return 0;
}
static void powerOnSpecialEnable(P_FLY_TPMS_INFO pFlyTPMSInfo,BOOL bOn)
{
	DWORD dwThreadID;
	if (bOn)
	{
		MSGQUEUEOPTIONS  msgOpts;

		if (pFlyTPMSInfo->bSpecialPower)
		{
			return;
		}
		pFlyTPMSInfo->bSpecialPower = TRUE;

		pFlyTPMSInfo->hBuffToUserDataEvent = CreateEvent(NULL,FALSE,FALSE,NULL);	
		InitializeCriticalSection(&pFlyTPMSInfo->hCSSendToUser);

		//ExBox 
		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TPMS_TO_EXBOX_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TPMS_TO_EXBOX_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pFlyTPMSInfo->hMsgQueueTPMSToExBoxCreate = CreateMsgQueue(FLY_MSG_QUEUE_TPMS_TO_EXBOX_NAME, &msgOpts);
		if (NULL == pFlyTPMSInfo->hMsgQueueTPMSToExBoxCreate)
		{
			DBGE((TEXT("\r\nFlyAudio TPMS Create MsgQueue To COMx Fail!")));
		}

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_EXBOX_TO_TPMS_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_EXBOX_TO_TPMS_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pFlyTPMSInfo->hMsgQueueExBoxToTPMSCreate = CreateMsgQueue(FLY_MSG_QUEUE_EXBOX_TO_TPMS_NAME, &msgOpts);
		if (NULL == pFlyTPMSInfo->hMsgQueueExBoxToTPMSCreate)
		{
			DBGE((TEXT("\r\nFlyAudio TPMS Create MsgQueue FROM EXBOX Fail!")));
		}
		pFlyTPMSInfo->bKillDispatchFlyMsgQueueEXBOXReadThread = FALSE;
		pFlyTPMSInfo->FLyMsgQueueEXBOXReadThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //��ȫ����
			0,//��ʼ���߳�ջ�Ĵ�С��ȱʡΪ�����̴߳�С��ͬ
			(LPTHREAD_START_ROUTINE)ThreadExBoxRead, //�̵߳�ȫ�ֺ���
			pFlyTPMSInfo, //�˴�����������ܵľ��
			0, &dwThreadID );
		DBGE((TEXT("\r\nFlyAudio ThreadExBoxRead ID:%x"),dwThreadID));
		if (NULL == pFlyTPMSInfo->FLyMsgQueueEXBOXReadThreadHandle)
		{
			pFlyTPMSInfo->bKillDispatchFlyMsgQueueEXBOXReadThread = TRUE;
			return;
		}
	}
	else
	{
		if (!pFlyTPMSInfo->bSpecialPower)
		{
			return;
		}
		pFlyTPMSInfo->bKillDispatchFlyMsgQueueEXBOXReadThread = TRUE;
		while (pFlyTPMSInfo->FLyMsgQueueEXBOXReadThreadHandle)
		{
			SetEvent(pFlyTPMSInfo->hMsgQueueExBoxToTPMSCreate);
			Sleep(10);
		}
		CloseMsgQueue(pFlyTPMSInfo->hMsgQueueExBoxToTPMSCreate);
		CloseMsgQueue(pFlyTPMSInfo->hMsgQueueExBoxToTPMSCreate);

		pFlyTPMSInfo->bSpecialPower = FALSE;

		CloseHandle(pFlyTPMSInfo->hBuffToUserDataEvent);
	}
}
static VOID TPMSWriteFile(P_FLY_TPMS_INFO pFlyTPMSInfo,BYTE *p,UINT length)
{
	//if (pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalExBoxInfo.bInit)
	{
		if (FALSE == WriteMsgQueue(pFlyTPMSInfo->hMsgQueueTPMSToExBoxCreate, p, length, 0, 0))
		{
			DWORD dwError = GetLastError();

			DBGE((TEXT("\r\nFlyAudio TPMS Write MsgQueue To COMx Error!0x%x"),dwError));
		}
	}
}
static VOID TMPSCmdPrintf(P_FLY_TPMS_INFO pFlyTPMSInfo,BYTE *buf,UINT len)
{
	BYTE sendBuf[2];
	sendBuf[0] = buf[0];
	sendBuf[1] = buf[1];
	TPMSWriteFile(pFlyTPMSInfo,sendBuf,2);
}
static VOID control_TPMSLearnStart(P_FLY_TPMS_INFO pFlyTPMSInfo)
{
	BYTE buff[2];
	buff[0] = 0xe0;buff[1] = 0x01;
	TMPSCmdPrintf(pFlyTPMSInfo,buff,2);
}
static VOID control_TPMSLearnEnd(P_FLY_TPMS_INFO pFlyTPMSInfo)
{
	BYTE buff[2];
	buff[0] = 0xe0;buff[1] = 0x02;
	TMPSCmdPrintf(pFlyTPMSInfo,buff,2);
}
static DWORD WINAPI ThreadFlyTPMSProc(LPVOID pContext)
{
	P_FLY_TPMS_INFO pFlyTPMSInfo = (P_FLY_TPMS_INFO)pContext;
	ULONG WaitReturn;
	while (!pFlyTPMSInfo->bKillDispatchFlyTPMSThread)
	{
		if (!pFlyTPMSInfo->bPower)
		{
			WaitReturn = WaitForSingleObject(pFlyTPMSInfo->hDispatchThreadTPMSEvent, 314);
		}
		else
		{
			WaitReturn = WaitForSingleObject(pFlyTPMSInfo->hDispatchThreadTPMSEvent, INFINITE);
		}

		//DBGD((TEXT("\r\nFlyAudio ThreadFlyTPMSProc ing (%d  %d)"),pFlyTPMSInfo->bPrePairStatus,pFlyTPMSInfo->bCurPairStatus));

		if (pFlyTPMSInfo->bPower)
		{
			if(pFlyTPMSInfo->bPrePairStatus != pFlyTPMSInfo->bCurPairStatus)
			{
				pFlyTPMSInfo->bCurPairStatus = pFlyTPMSInfo->bPrePairStatus;
				if (pFlyTPMSInfo->bCurPairStatus)
				{
					control_TPMSLearnStart(pFlyTPMSInfo);
				}
				else
				{
					control_TPMSLearnEnd(pFlyTPMSInfo);
				}
			}

			if (pFlyTPMSInfo->bHavaTMPS != pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTPMSModule)
			{
				pFlyTPMSInfo->bHavaTMPS = pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTPMSModule;
				returnTPMSConnectStatus(pFlyTPMSInfo,pFlyTPMSInfo->bHavaTMPS);
			}
		}
	}

	CloseHandle(pFlyTPMSInfo->hDispatchThreadTPMSEvent);
	DBGD((TEXT("\r\nFlyAudio ThreadFlyTPMSProc exit")));
	return 0;
}
void FlyTPMS_Enable(P_FLY_TPMS_INFO pFlyTPMSInfo,BOOL bEnable)
{
	DWORD dwThreadID;
	if (bEnable)
	{
		pFlyTPMSInfo->bKillDispatchFlyTPMSThread = FALSE;
		pFlyTPMSInfo->hDispatchThreadTPMSEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
		pFlyTPMSInfo->hThreadHandleFlyTPMS = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //��ȫ����
			0,//��ʼ���߳�ջ�Ĵ�С��ȱʡΪ�����̴߳�С��ͬ
			(LPTHREAD_START_ROUTINE)ThreadFlyTPMSProc, //�̵߳�ȫ�ֺ���
			pFlyTPMSInfo, //�˴�����������ܵľ��
			0, &dwThreadID );
		DBGE((TEXT("\r\nFlyAudio ThreadFlyTPMSProc ID:%x"),dwThreadID));



// -------------------------------------------------------------------------
		if (NULL == pFlyTPMSInfo->hThreadHandleFlyTPMS)
		{
			pFlyTPMSInfo->bKillDispatchFlyTPMSThread = TRUE;
			return;
		}
	}
	else
	{
		pFlyTPMSInfo->bKillDispatchFlyTPMSThread = TRUE;
		SetEvent(pFlyTPMSInfo->hDispatchThreadTPMSEvent);		
	}
}
/********************************************************************************************************
*�������ƣ�WinCECommandProcessor
*�������ܣ�����WINCE������������
*���������
*���������
*�� �� ֵ��
**********************************************************************************************************/
static VOID FlyTPMS_DealWinceCmd(P_FLY_TPMS_INFO pFlyTPMSInfo,BYTE *buf, UINT len)
{
	RETAILMSG(1, (TEXT(" OK!\r\n")));

	switch(buf[0])
	{
	case 0x01://��ʼ������
		if (0x01 == buf[1])
		{
			returnPowerStatus(pFlyTPMSInfo,TRUE);
			returnWorkMode(pFlyTPMSInfo,TRUE);
			pFlyTPMSInfo->bPower = TRUE;
			SetEvent(pFlyTPMSInfo->hDispatchThreadTPMSEvent);
		}
		else if (0x00 == buf[1])
		{
			pFlyTPMSInfo->bPower = FALSE;
			returnPowerStatus(pFlyTPMSInfo,FALSE);
			returnWorkMode(pFlyTPMSInfo,TRUE);
		}		
		break;
	case 0x03://���ģ�ⰴ��
		if (pFlyTPMSInfo->bCurPairStatus)
		{
			pFlyTPMSInfo->bPrePairStatus = FALSE;
		} 
		else
		{
			pFlyTPMSInfo->bPrePairStatus = TRUE;
		}
		SetEvent(pFlyTPMSInfo->hDispatchThreadTPMSEvent);
		break;
	case 0x10://TPMS ���
		pFlyTPMSInfo->bPrePairStatus = (BOOL)buf[1];
		SetEvent(pFlyTPMSInfo->hDispatchThreadTPMSEvent);
		break;
	case 0xff://�����ã�����ʹ�ò����
		if (0x01 == buf[1])
		{
			FTP_PowerUp((DWORD)pFlyTPMSInfo);
		} 
		else if (0x00 == buf[1])
		{
			FTP_PowerDown((DWORD)pFlyTPMSInfo);
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
//static VOID FlyTPMS_WinCECommandProcessor(P_FLY_TPMS_INFO pFlyTPMSInfo, BYTE *buf, UINT len)
//{
//	static BYTE checksum;
//	static UINT FrameLen;
//	static UINT FrameLenMax;
//	static BYTE ReceiveReg[256];
//
//	for(UINT i = 0;i < len;i++)
//	{
//		switch (pFlyTPMSInfo->buffToDriverProcessorStatus)
//		{
//		case 0:
//			if(0xFF == buf[i])
//			{
//				pFlyTPMSInfo->buffToDriverProcessorStatus = 1;
//			}
//			break;
//		case 1:
//			if(0x55 == buf[i])
//			{
//				pFlyTPMSInfo->buffToDriverProcessorStatus = 2;
//			}
//			else
//			{
//				pFlyTPMSInfo->buffToDriverProcessorStatus = 0;
//			}
//			break;
//		case 2:
//			pFlyTPMSInfo->buffToDriverProcessorStatus = 3;
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
//					FlyTPMS_DealWinceCmd(pFlyTPMSInfo,ReceiveReg,FrameLen);
//				}
//				pFlyTPMSInfo->buffToDriverProcessorStatus = 0;
//			}
//			break;
//		default:
//			pFlyTPMSInfo->buffToDriverProcessorStatus = 0;
//			break;
//		}
//	}
//}

void IpcRecv(UINT32 iEvent)
{
	P_FLY_TPMS_INFO	pFlyTPMSInfo = (P_FLY_TPMS_INFO)gpFlyTPMSInfo;

	DBGD((TEXT("\r\n FLY TPMS Recv IPC Event:%d"),iEvent));

	//SetEvent(pFlyTPMSInfo->hCollexBTMainThreadEvent);
}

/********************************************************************************************************
*�������ƣ�HANDLE FTP_Init()
*�������ܣ������豸�����豸�����ص�ʱ�����
*���������
*���������
*�� �� ֵ��
**********************************************************************************************************/
HANDLE FTP_Init(
				DWORD dwContext   //�ַ�����ָ��ע����м�¼���������ļ�
				)
{
	P_FLY_TPMS_INFO pFlyTPMSInfo;

	RETAILMSG(1, (TEXT("\r\nTPMS Init Start")));


	//�����ڴ�
	pFlyTPMSInfo = (P_FLY_TPMS_INFO)LocalAlloc(LPTR, sizeof(FLY_TPMS_INFO));
	if (!pFlyTPMSInfo)
	{
		RETAILMSG(1, (TEXT("FlyAudio TPMS LocalAlloc Failed!\r\n")));
		return NULL;
	}

	gpFlyTPMSInfo = pFlyTPMSInfo;

	pFlyTPMSInfo->pFlyShmGlobalInfo = CreateShm(TPMS_MODULE,IpcRecv);
	if (NULL == pFlyTPMSInfo->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio TPMS Init create shm err\r\n")));
		return NULL;
	}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio TPMS Open create user buf err\r\n")));
		return NULL;
	}


	if (pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalStructSize
		!= sizeof(struct shm))
	{
		pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[0] = 'F';
		pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[1] = 'T';
		pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[2] = 'P';
	}

	//pFlyTPMSInfo->hHandleGlobalGlobalEvent = CreateEvent(NULL,FALSE,FALSE,DATA_GLOBAL_HANDLE_GLOBAL);
	pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTPMSInfo.bInit = TRUE;
	pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTPMSInfo.iDriverCompYear = year;
	pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTPMSInfo.iDriverCompMon = months;
	pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTPMSInfo.iDriverCompDay = day;
	pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTPMSInfo.iDriverCompHour = hours;
	pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTPMSInfo.iDriverCompMin = minutes;
	pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTPMSInfo.iDriverCompSec = seconds;

	DBGE((TEXT("\r\nFlyAudio TPMS Init Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	//������ʼ��
	powerNormalInit(pFlyTPMSInfo);
	powerOnFirstInit(pFlyTPMSInfo);
	powerOnSpecialEnable(pFlyTPMSInfo,TRUE);

	return (HANDLE)pFlyTPMSInfo;
}
/********************************************************************************************************
*�������ƣ�DWORD FTP_Deinit()
*�������ܣ��ͷ��豸�����豸��ж�ص�ʱ�����
*���������
*���������
*�� �� ֵ�������豸ж���Ƿ�ɹ�
**********************************************************************************************************/
BOOL FTP_Deinit(
				DWORD hDeviceContext		//XXX_Init()�������ص��豸������
				)
{
	P_FLY_TPMS_INFO pFlyTPMSInfo = (P_FLY_TPMS_INFO)hDeviceContext;

#if GLOBAL_COMM
	//ȫ��
	pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTPMSInfo.bOpen = FALSE;
	pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTPMSInfo.bInit = FALSE;
	//SetEvent(pFlyTPMSInfo->hHandleGlobalGlobalEvent);
	//CloseHandle(pFlyTPMSInfo->hHandleGlobalGlobalEvent);
#endif
	powerOnSpecialEnable(pFlyTPMSInfo,FALSE);

	FreeUserBuff();
	FreeShm();
	LocalFree(pFlyTPMSInfo);
	DBGE((TEXT("FlyAudio TPMS LocalFree!\r\n")));
	return TRUE;
}

/********************************************************************************************************
*�������ƣ�DWORD FTP_Open()
*�������ܣ����豸���ж�д ��Ӧ�ó����CreateFile()������Ӧ
*���������
*���������
*�� �� ֵ�������豸�Ĵ�������
**********************************************************************************************************/
DWORD FTP_Open(
			   DWORD hDeviceContext,	//�豸�����ģ���XXX_Init()��������
			   DWORD AccessCode,		//�豸�ķ���ģʽ����CreateFile()��������
			   DWORD ShareMode			//�豸�Ĺ���ģʽ����CreateFile()��������
			   )
{
	P_FLY_TPMS_INFO pFlyTPMSInfo = (P_FLY_TPMS_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;
	
	if (pFlyTPMSInfo->bOpen)
	{
		return NULL;
	}
	pFlyTPMSInfo->bOpen = TRUE;
	
	FlyTPMS_Enable(pFlyTPMSInfo,TRUE);

	pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTPMSInfo.bOpen = TRUE;

	DBGE((TEXT("FlyAudio TPMS Open Build:Date%d.%d.%dTime%d:%d:%d\r\n"),
		year,months,day,hours,minutes,seconds));

	return returnWhat;
}

/********************************************************************************************************
*�������ƣ�VOID FTP_Close()
*�������ܣ��ر��豸 ��Ӧ�ó����CloseHandle()������Ӧ
*���������
*���������
*�� �� ֵ�������豸�ر��Ƿ�ɹ�
**********************************************************************************************************/
BOOL FTP_Close(
			   DWORD hDeviceContext		//�豸�Ĵ������ģ���XXX_Open������������
			   )
{
	P_FLY_TPMS_INFO pFlyTPMSInfo = (P_FLY_TPMS_INFO)hDeviceContext;

	if (!pFlyTPMSInfo->bOpen)
	{
		return FALSE;
	}
	pFlyTPMSInfo->bOpen = FALSE;

	pFlyTPMSInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTPMSInfo.bOpen = FALSE;

	FlyTPMS_Enable(pFlyTPMSInfo,FALSE);

	DBGE((TEXT("FlyAudio TPMS Close!\r\n")));
	return TRUE;
}
/********************************************************************************************************
*�������ƣ�VOID FTP_PowerUp()
*�������ܣ���Դ����
*���������
*���������
*�� �� ֵ����
**********************************************************************************************************/
VOID FTP_PowerUp(DWORD hDeviceContext)
{
	P_FLY_TPMS_INFO pFlyTPMSInfo = (P_FLY_TPMS_INFO)hDeviceContext;
	
	if (pFlyTPMSInfo->bOpen)
	{
		powerNormalInit(pFlyTPMSInfo);
	}
	
	DBGE((TEXT("FlyAudio TPMS PowerUp!\r\n")));
}
/********************************************************************************************************
*�������ƣ�VOID FTP_PowerDown()
*�������ܣ���Դ����
*���������
*���������
*�� �� ֵ����
**********************************************************************************************************/
VOID FTP_PowerDown(DWORD hDeviceContext)
{
	P_FLY_TPMS_INFO pFlyTPMSInfo = (P_FLY_TPMS_INFO)hDeviceContext;

	if (pFlyTPMSInfo->bOpen)
	{
		powerParaDeInit(pFlyTPMSInfo);
	}
	DBGE((TEXT("FlyAudio TPMS PowerDown!\r\n")));
}
/********************************************************************************************************
*�������ƣ�DWORD FTP_Read()
*�������ܣ����豸�ж�ȡ����  ��Ӧ�ó���ReadFile()�����ݶ�Ӧ
*���������
*���������
*�� �� ֵ������0��ʾ�ļ�����������-1��ʾʧ��,���ض�ȡ���ֽ�����ʾ�ɹ�
**********************************************************************************************************/
DWORD FTP_Read(
			   DWORD  hOpenContext,		//XXX_Open�������ص��豸��������
			   LPVOID pBuffer,			//�������������ָ�룬��ȡ���ݻᱻ���ڸû�������
			   DWORD  Count				//Ҫ��ȡ���ֽ���
			   )
{
	P_FLY_TPMS_INFO pFlyTPMSInfo = (P_FLY_TPMS_INFO)hOpenContext;

	UINT dwRead = 0,i;
	BYTE *buf = (BYTE *)pBuffer;

	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio TPMS return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}
	return dwRead;


	//while(dwRead < Count && pFlyTPMSInfo->iBuffToUserLx != pFlyTPMSInfo->iBuffToUserHx)
	//{
	//	SendBuf[dwRead++] = pFlyTPMSInfo->BuffToUser[pFlyTPMSInfo->iBuffToUserLx++];
	//	if (pFlyTPMSInfo->iBuffToUserLx >= DATA_BUFF_LENGTH)
	//	{
	//		pFlyTPMSInfo->iBuffToUserLx = 0;
	//	}
	//}
	//if(pFlyTPMSInfo->iBuffToUserLx != pFlyTPMSInfo->iBuffToUserHx)
	//{
	//	SetEvent(pFlyTPMSInfo->hBuffToUserDataEvent);
	//}

	return dwRead;//�����������ֽڴ�С��������0ʱ���û��̿�ʼ������
}
/********************************************************************************************************
*�������ƣ�DWORD FTP_Write()
*�������ܣ����豸��д������  ��Ӧ�ó���WriteFile()�����ݶ�Ӧ
*���������
*���������
*�� �� ֵ������-1��ʾʧ�ܣ�����д����ֽ�����ʾ�ɹ�
**********************************************************************************************************/
DWORD FTP_Write(
				DWORD    hOpenContext,		// XXX_Open���ص��豸��������
				LPCVOID  pSourceBytes,		//���룬ָ��Ҫд���豸�����ݵĻ���
				DWORD    NumberOfBytes		//�����е����ݵ��ֽ���
				)
{
	P_FLY_TPMS_INFO pFlyTPMSInfo = (P_FLY_TPMS_INFO)hOpenContext;
	BYTE *p;
	p = (BYTE *)pSourceBytes;
	RETAILMSG(1, (TEXT("\r\nFlyAudio TPMS Write %d "), NumberOfBytes));
	for(UINT i = 0;i < NumberOfBytes;i++)
	{
		DBGD((TEXT(" %X"), *((BYTE *)pSourceBytes + i)));
	}

	if(NumberOfBytes >= 5)
	{
		FlyTPMS_DealWinceCmd(pFlyTPMSInfo,&p[3],NumberOfBytes-4);
	}
	return NULL;
}
/********************************************************************************************************
*�������ƣ�DWORD FTP_Seek()
*�������ܣ��ƶ��豸�е�����ָ��  ��Ӧ�ó���SetFilePointer()�����ݶ�Ӧ
*���������
*���������
*�� �� ֵ���������ݵ���ָ��λ�ã�-1��ʾʧ��
**********************************************************************************************************/
DWORD FTP_Seek(
			   DWORD  hOpenContext,		//XXX_Open()���ص��豸��������
			   LONG   Amount,			//Ҫ�ƶ��ľ��룬������ʾǰ�ƣ�������ʾ����
			   DWORD  Type				//�����е����ݵ��ֽ���
			   )
{
	return NULL;
}
/********************************************************************************************************
*�������ƣ�
*�������ܣ������������Ϳ�������  ��Ӧ�ó���DeviceIoControl()�����ݶ�Ӧ
*���������
*���������
*�� �� ֵ������ֵ��TRUE��ʾ�����ɹ���FALSE��ʾ����ʧ��
**********************************************************************************************************/
BOOL FTP_IOControl(
				   DWORD  hOpenContext,		//��XXX_Open()���ص��豸��������
				   DWORD  dwCode,			//Ҫ���͵Ŀ����룬һ��32λ�޷�����
				   PBYTE  pBufIn,			//���룬ָ�����뻺������ָ��
				   DWORD  dwLenIn,			//���뻺�����ĳ���
				   PBYTE  pBufOut,			//�����ָ�������������ָ��
				   DWORD  dwLenOut,			//�������������󳤶�
				   PDWORD pdwActualOut		//������豸ʵ��������ֽ���
				   )
{
	P_FLY_TPMS_INFO pFlyTPMSInfo = (P_FLY_TPMS_INFO)hOpenContext;


	BOOL bRes = TRUE;

	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	DBGD((TEXT("\r\nFlyAudio Parrot IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)"),
	//		hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	DBGD((TEXT("\r\nFlyAudio TPMS IOControl Set IOCTL_SERIAL_SET_WAIT_MASK")));
	//		break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	//DBGD((TEXT("\r\nFlyAudio TPMS IOControl Set IOCTL_SERIAL_WAIT_ON_MASK")));
	//	if (pFlyTPMSInfo->bOpen)
	//	{
	//		WaitForSingleObject(pFlyTPMSInfo->hBuffToUserDataEvent,INFINITE);
	//	} 
	//	else
	//	{
	//		WaitForSingleObject(pFlyTPMSInfo->hBuffToUserDataEvent,0);
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
		RETAILMSG(1,(TEXT("Attach in FlyAudio TPMS DllEntry")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		RETAILMSG(1,(TEXT("Dettach in FlyAudio TPMS DllEntry")));
	}

	RETAILMSG(1,(TEXT("Leaving in FlyAudio TPMS DllEntry")));

	return (TRUE);
}
