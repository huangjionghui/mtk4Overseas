// -----------------------------------------------------------------------------
// File Name    : FLYExBox.cpp
// Title        : FLYExBox Driver
// Author       : Mootall - Copyright (C) 2011
// Created      : 2011-06-15  
// Version      : 0.01
// Target MCU   : WinCE Driver
// -----------------------------------------------------------------------------
// Version History:
/*
>>>
>>>
*/
// FLYExBox.cpp : Defines the entry point for the DLL application.
//

#include "FLYExBox.h"
#include "flyseriallib.h"

#define GLOBAL_COMM	1

P_FLY_EXBOX_INFO gpFlyExBoxInfo = NULL;

static void powerOnFirstInit(P_FLY_EXBOX_INFO pFlyExBoxInfo)
{
	pFlyExBoxInfo->bExBoxEnable = FALSE;
	
	pFlyExBoxInfo->bHaveExBox =FALSE;

	pFlyExBoxInfo->bKillDispatchFlyMainThread = TRUE;
	pFlyExBoxInfo->FlyMainThreadHandle = NULL;
	pFlyExBoxInfo->hDispatchMainThreadEvent = NULL;

	pFlyExBoxInfo->hExBoxComm = NULL;
	pFlyExBoxInfo->hThreadHandleFlyExBoxComm = NULL;
	pFlyExBoxInfo->ExBoxInfoFrameStatus = 0;

	pFlyExBoxInfo->hMsgQueueTPMSToEXBOXCreate = NULL;
	pFlyExBoxInfo->hMsgQueueExBoxToTMPSCreate = NULL;
	pFlyExBoxInfo->bKillDispatchFlyMsgQueueReadTPMSThread = TRUE;
	pFlyExBoxInfo->FLyMsgQueueReadTPMSThreadHandle = NULL;

	pFlyExBoxInfo->hMsgQueueTVToEXBOXCreate = NULL;
	pFlyExBoxInfo->hMsgQueueExBoxToTVCreate = NULL;
	pFlyExBoxInfo->bKillDispatchFlyMsgQueueReadTVThread = TRUE;
	pFlyExBoxInfo->FLyMsgQueueReadTVThreadHandle = NULL;

	pFlyExBoxInfo->bRevData[0] = 0;
	pFlyExBoxInfo->bRevData[1] = 0;

	pFlyExBoxInfo->ulExBoxSyncTimer =GetTickCount();
}
static VOID ExBoxWriteFileToTPMS(P_FLY_EXBOX_INFO pFlyExBoxInfo,BYTE *p,UINT length)
{
	if (pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTPMSInfo.bInit)
	{
		if (FALSE == WriteMsgQueue(pFlyExBoxInfo->hMsgQueueExBoxToTMPSCreate, p, length, 0, 0))
		{
			DWORD dwError = GetLastError();

			DBGE((TEXT("\r\nFlyAudio ExBox Write MsgQueue To TPMS Error!0x%x"),dwError));
		}
	}
}
static VOID ExBoxWriteFileToTV(P_FLY_EXBOX_INFO pFlyExBoxInfo,BYTE *p,UINT length)
{
	if (pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTVInfo.bInit)
	{
		if (FALSE == WriteMsgQueue(pFlyExBoxInfo->hMsgQueueExBoxToTVCreate, p, length, 0, 0))
		{
			DWORD dwError = GetLastError();

			DBGE((TEXT("\r\nFlyAudio ExBox Write MsgQueue To TV Error!0x%x"),dwError));
		}
		DBGD((TEXT("\r\nFlyAudio ExBox Write MsgQueue To TV 0x%x,0x%x"),p[0],p[1]));
	}
	else
	{
		DBGE((TEXT("\r\nFlyAudio TV not init")));
	}
}
////((DEVICE_BT<<7) | (DEVICE_TV<<6) | (DEVICE_IR_STEERWHEEL<<5) | (DEVICE_TIRE<<4) | CDC_TYPE)
static VOID DeviceStatusInfo(P_FLY_EXBOX_INFO pFlyExBoxInfo,BYTE para)
{
	BYTE DEVICE_TV_HAVE;
	BYTE DEVICE_TIRE_HAVE;

	DEVICE_TV_HAVE = para & (1<<6);
	DEVICE_TIRE_HAVE = para & (1<<4);
	if (DEVICE_TV_HAVE)
	{
		if (!pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTVModule)
		{
			pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTVModule = TRUE;
			IpcStartEvent(EVENT_GLOBAL_TVBOX_EXIST_ID);
			
		}
		DBGD((TEXT("\r\nFlyAudio TV ON")));
	}
	else
	{
		if (pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTVModule)
		{
			pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTVModule = FALSE;
			IpcStartEvent(EVENT_GLOBAL_TVBOX_EXIST_ID);
		}
		DBGD((TEXT("\r\nFlyAudio TV OFF")));
	}
	if (DEVICE_TIRE_HAVE)
	{
		if (!pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTPMSModule)
		{
			pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTPMSModule = TRUE;
			IpcStartEvent(EVENT_GLOBAL_TPMSBOX_EXIST_ID);
		}
		DBGD((TEXT("\r\nFlyAudio TPMS ON")));
	}
	else
	{
		if (pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTPMSModule)
		{
			pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTPMSModule = FALSE;
			IpcStartEvent(EVENT_GLOBAL_TPMSBOX_EXIST_ID);
		}
		DBGD((TEXT("\r\nFlyAudio TPMS OFF")));
	}
}
static VOID MessageToTV(P_FLY_EXBOX_INFO pFlyExBoxInfo,BYTE data1,BYTE data2)
{
	BYTE sendBuf[2];
	sendBuf[0] = data1;
	sendBuf[1] = data2;
	ExBoxWriteFileToTV(pFlyExBoxInfo,sendBuf,2);
}
static VOID MessageToTPMS(P_FLY_EXBOX_INFO pFlyExBoxInfo,BYTE data1,BYTE data2)
{
	BYTE sendBuf[2];	
	sendBuf[0] = data1;
	sendBuf[1] = data2;
	ExBoxWriteFileToTPMS(pFlyExBoxInfo,sendBuf,2);
}
static VOID FlyExBoxSendCmdToUART(P_FLY_EXBOX_INFO pFlyExBoxInfo,BYTE data1,BYTE data2)
{
	BYTE sendBuf[2];
	DWORD bufSendLength;

	sendBuf[0] = data1;
	sendBuf[1] = data2;
	WriteFile(pFlyExBoxInfo->hExBoxComm,sendBuf,2,&bufSendLength,NULL);
	DBGD((TEXT("\r\nFlyAudio FlyExBoxSendCmdToUART 0x%x,0x%x"),sendBuf[0],sendBuf[1]));
}

static void SendFcwLdwInfo(P_FLY_EXBOX_INFO pFlyExBoxInfo,BYTE *buf,UINT16 len)
{
	BYTE sendBuf[255] ={0xf5,0x55};
	BYTE crc;
	BYTE i=0;
	DWORD bufSendLength;

	crc =len+1;
	sendBuf[2]=len+1;

	for(i=0;i<len;i++)
	{
		sendBuf[i+3] = buf[i];
		crc+=buf[i];
	}
	sendBuf[3+i] = crc;
	WriteFile(pFlyExBoxInfo->hExBoxComm,sendBuf,len+4,&bufSendLength,NULL);
	DBGD((TEXT("\r\nFlyAudio FlyExBoxSendCmdToUART 0x%x,0x%x"),sendBuf[0],sendBuf[1]));
}
static void DealBarCodeID(P_FLY_EXBOX_INFO pFlyExBoxInfo,BYTE data)//####################
{
	static unsigned char checksum;
	static unsigned char FrameLen;
	static unsigned char FrameStatus;
	static unsigned char FrameLenMax;
	static unsigned char ReceiveReg[256];
	
	switch (FrameStatus)
	{
		case 0:
			if (data == 0xff) 
			{
				FrameStatus = 1;
			}
			break;
		case 1:
			if (data == 0x55) 
			{
				FrameStatus = 2;
			}
			else 
			{
				FrameStatus = 0;
				pFlyExBoxInfo->ExBoxInfoFrameStatus =0;
				
			}
			break;
		case 2:
			if(data == 0)
			{
			   FrameStatus =0;
			   pFlyExBoxInfo->ExBoxInfoFrameStatus =0;
			}
			else
			{
				FrameStatus = 3;
				checksum = FrameLenMax = data;
				FrameLen = 0;
			}
			break;
		case 3:
			if(FrameLen < (FrameLenMax))
			{
				ReceiveReg[FrameLen] = data;
				checksum += data;
				FrameLen ++;
			}
			else
			{
				if((BYTE)(0x100-data) == checksum)
				{
					ReceiveReg[FrameLen] = 0;
					pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.BarCodeID[0]=FrameLen-2;
					memcpy((void*)&pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.BarCodeID[1],&ReceiveReg[2],FrameLen-2);

					IpcStartEvent(EVENT_GLOBAL_RETURN_BAR_CODE_ID);
					
					DBGD((TEXT("\r\nFlyAudio BarCode ID :")));

					for(BYTE i=0;i<pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.BarCodeID[0];i++)
					{
						DBGD((TEXT("%d  "),pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.BarCodeID[i]));		
					}
					
				}
				else
				{
					DBGD((TEXT("\r\nFlyAudio BarCode ID CRC ERROR")));
				}
				FrameStatus = 0;
				pFlyExBoxInfo->ExBoxInfoFrameStatus =0;
			}
			break;

		default:			
			FrameStatus = 0;
			pFlyExBoxInfo->ExBoxInfoFrameStatus =0;
		break;
	}
}
static void DealFcwLdwInfo(P_FLY_EXBOX_INFO pFlyExBoxInfo,BYTE data)//���ƫ��ϵͳ��Ϣ����
{
	static unsigned char checksum;
	static unsigned char FrameLen;
	static unsigned char FrameStatus;
	static unsigned char FrameLenMax;
	static unsigned char ReceiveReg[256];

	switch (FrameStatus)
	{
	case 0:
		if (data == 0xf5) 
		{
			FrameStatus = 1;
		}
		break;
	case 1:
		if (data == 0x55) 
		{
			FrameStatus = 2;
		}
		else 
		{
			FrameStatus = 0;
			pFlyExBoxInfo->ExBoxInfoFrameStatus =0;

		}
		break;
	case 2:
		if(data == 0)
		{
			FrameStatus =0;
			pFlyExBoxInfo->ExBoxInfoFrameStatus =0;
		}
		else
		{
			FrameStatus = 3;
			checksum = FrameLenMax = data;
			FrameLen = 0;
		}
		break;
	case 3:
		if(FrameLen < (FrameLenMax-1))
		{
			ReceiveReg[FrameLen] = data;
			checksum += data;
			FrameLen ++;
		}
		else
		{
			if(data== checksum)
			{
				ReceiveReg[FrameLen] = 0;
				ExBoxWriteFileToTV(pFlyExBoxInfo,ReceiveReg,FrameLen);
			}
			else
			{
				DBGD((TEXT("\r\nFlyAudio  ID CRC ERROR")));
			}
			FrameStatus = 0;
			pFlyExBoxInfo->ExBoxInfoFrameStatus =0;
		}
		break;
	default:			
		FrameStatus = 0;
		pFlyExBoxInfo->ExBoxInfoFrameStatus =0;
		break;
	}
}
static VOID OnCommRecv(P_FLY_EXBOX_INFO pFlyExBoxInfo, BYTE *buf, UINT buflen)
{
	for (UINT i = 0; i < buflen; i++)
	{
		//DBGD((TEXT("\r\nFlyAudio Exbox read_data %X"),buf[i]));
		pFlyExBoxInfo->ulExBoxSyncTimer=GetTickCount();
		pFlyExBoxInfo->bHaveExBox = TRUE;
		switch (pFlyExBoxInfo->ExBoxInfoFrameStatus)
		{
		case 0://����
			if ((buf[i] & 0xf0) == 0xe0)
			{
				pFlyExBoxInfo->ExBoxInfoFrameStatus = 1;
			}
			else if ((buf[i] & 0xf0) == 0xf0) 
			{
				if (buf[i] == 0xf0)
				{
					pFlyExBoxInfo->ExBoxInfoFrameStatus = 5;//����Щ�豸
				}
				else if(buf[i] == 0xff)//������
				{
					pFlyExBoxInfo->ExBoxInfoFrameStatus = 6;
					DealBarCodeID(pFlyExBoxInfo,buf[i]);
				}
				else if(buf[i] == 0xf5)//���ƫ��
				{
					pFlyExBoxInfo->ExBoxInfoFrameStatus = 7;
					DealFcwLdwInfo(pFlyExBoxInfo,buf[i]);
				}
				else
				{
					pFlyExBoxInfo->ExBoxInfoFrameStatus = 2;
				}
				
			}
			else if ((buf[i] & 0xf0) == 0x80)
			{
				pFlyExBoxInfo->ExBoxInfoFrameStatus = 3;
			}
			else if (buf[i] == 0xaa)
			{
				pFlyExBoxInfo->ExBoxInfoFrameStatus = 4;
			}
			else
			{
				pFlyExBoxInfo->ExBoxInfoFrameStatus = 0;
			}
			//record data1
			if (pFlyExBoxInfo->ExBoxInfoFrameStatus != 0)
			{
				pFlyExBoxInfo->bRevData[0] = buf[i];
			}
			break;
		case 1://TPMS
			pFlyExBoxInfo->bRevData[1] = buf[i];
			MessageToTPMS(pFlyExBoxInfo,pFlyExBoxInfo->bRevData[0],pFlyExBoxInfo->bRevData[1]);
			pFlyExBoxInfo->ExBoxInfoFrameStatus = 0;
			break;
		case 2://TV
			pFlyExBoxInfo->bRevData[1] = buf[i];
			MessageToTV(pFlyExBoxInfo,pFlyExBoxInfo->bRevData[0],pFlyExBoxInfo->bRevData[1]);
			pFlyExBoxInfo->ExBoxInfoFrameStatus = 0;
			break;
		case 3://CDC
			pFlyExBoxInfo->bRevData[1] = buf[i];
			//MessageToCDC(pFlyExBoxInfo,pFlyExBoxInfo->bRevData[0],pFlyExBoxInfo->bRevData[1]);				
			pFlyExBoxInfo->ExBoxInfoFrameStatus = 0;
			break;
		case 4:
			pFlyExBoxInfo->bRevData[1] = buf[i];
			if (buf[i] == 0xaa)
			{
				FlyExBoxSendCmdToUART(pFlyExBoxInfo,0x55,0x55);
			}
			pFlyExBoxInfo->ExBoxInfoFrameStatus = 0;
			break;
		case 5:
			pFlyExBoxInfo->bRevData[1] = buf[i];
			DeviceStatusInfo(pFlyExBoxInfo,pFlyExBoxInfo->bRevData[1]);	
			pFlyExBoxInfo->ExBoxInfoFrameStatus = 0;
			break;
		case 6://������
			DealBarCodeID(pFlyExBoxInfo,buf[i]);
			break;
		case 7://���ƫ��
			DealFcwLdwInfo(pFlyExBoxInfo,buf[i]);
			break;
		default:
			pFlyExBoxInfo->ExBoxInfoFrameStatus = 0;
			break;
		}
	}
}

static DWORD WINAPI ThreadFlyExBoxCommProc(LPVOID pContext)
{
	P_FLY_EXBOX_INFO pFlyExBoxInfo = (P_FLY_EXBOX_INFO)pContext;
	DWORD ret;
	BYTE recvBuf[1024];
	BYTE *zeroBuf = new BYTE[1024];
	memset(zeroBuf,0,sizeof(BYTE)*1024);


	if (!pFlyExBoxInfo->hExBoxComm || pFlyExBoxInfo->hExBoxComm == INVALID_HANDLE_VALUE)
	{
		DBGE((TEXT("\r\nFlyAudio ThreadFlyExBoxCommProc exit with com error")));
		return 0;
	}
	else
	{
		DBGD((TEXT("\r\nFlyAudio ThreadFlyExBoxCommProc start")));
	}

	while (pFlyExBoxInfo->hExBoxComm)
	{
		ret = ReadSerial(pFlyExBoxInfo->hExBoxComm,recvBuf,1024);
		if(ret <= 0)
		{
			//���ܴӴ��ڶ�ȡ����
			DBGI((TEXT("\r\nFlyAudio EXBOX ComProcThread cannot read data!")));
		}
		else
		{
			DBGD((TEXT("\r\nEXBOX Serial Read %d bytes:"),ret));
			for (UINT i = 0;i < ret;i++)
			{
				DBGD((TEXT(" %x"),recvBuf[i]));
			}

			if (ret > 50)
			{
				//���жϼ���
				if (!memcmp(zeroBuf,recvBuf,10) 
					|| (!memcmp(&zeroBuf[15],&recvBuf[15],10))
					|| (!memcmp(&zeroBuf[30],&recvBuf[30],10)))
				{
					DBGD((TEXT("\r\nEXBOX Zero buf,coutinue read\r\n")));
					continue;
				}
			}

			//�����ݸ�ֵ��ȫ�ֱ���
			OnCommRecv(pFlyExBoxInfo,recvBuf,ret);
		}  

	}
	delete []zeroBuf;
	DBGD((TEXT("\r\nFlyAudio ThreadFlyExBoxCommProc exit")));
	return 0;
}

static VOID closeExBoxComm(P_FLY_EXBOX_INFO pFlyExBoxInfo)
{
	if (pFlyExBoxInfo->hThreadHandleFlyExBoxComm)
	{
		CloseHandle(pFlyExBoxInfo->hThreadHandleFlyExBoxComm);
		pFlyExBoxInfo->hThreadHandleFlyExBoxComm = NULL;
	}

	if (pFlyExBoxInfo->hExBoxComm)
	{
		CloseSerial(pFlyExBoxInfo->hExBoxComm);
		pFlyExBoxInfo->hExBoxComm = NULL;
	}
}
static VOID openExBoxComm(P_FLY_EXBOX_INFO pFlyExBoxInfo)
{
	if (pFlyExBoxInfo->hExBoxComm)
	{
		return;
	}

	//�򿪴���
	pFlyExBoxInfo->hExBoxComm =OpenSerial(_T("COM5:"), 19200);
	DBGI((TEXT("FlyAudio ExBox open com5!")));
	if(pFlyExBoxInfo->hExBoxComm == INVALID_HANDLE_VALUE ||
		!pFlyExBoxInfo->hExBoxComm)
	{
		pFlyExBoxInfo->hExBoxComm = NULL;
		DBGE((TEXT("FlyAudio ExBox cann't open com!")));
		PrintErrMsg(L"YYYYYYYYYYYYYY");
		return;
	}


	DWORD dwThreadID;
	pFlyExBoxInfo->hThreadHandleFlyExBoxComm = 
		CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //��ȫ����
		0,//��ʼ���߳�ջ�Ĵ�С��ȱʡΪ�����̴߳�С��ͬ
		(LPTHREAD_START_ROUTINE)ThreadFlyExBoxCommProc, //�̵߳�ȫ�ֺ���
		pFlyExBoxInfo, //�˴�����������ܵľ��
		0, &dwThreadID );
	DBGE((TEXT("\r\nFlyAudio ThreadFlyExBoxCommProc ID:%x"),dwThreadID));

		if (NULL == pFlyExBoxInfo->hThreadHandleFlyExBoxComm)
		{
			DBGD((TEXT("FlyAudio ForyouDVD CreateThread ThreadFlyExBoxCommProc!")));
				closeExBoxComm(pFlyExBoxInfo);
			return;
		}
		DBGD((TEXT("FlyAudio ExBox Comm init OK!")));
}
static DWORD WINAPI ThreadRead_TV(LPVOID pContext)
{
	P_FLY_EXBOX_INFO pFlyExBoxInfo = (P_FLY_EXBOX_INFO)pContext;
	ULONG WaitReturn;
	BYTE buff[FLY_MSG_QUEUE_TV_TO_EXBOX_LENGTH];
	DWORD dwRes;
	DWORD dwMsgFlag;

	while (!pFlyExBoxInfo->bKillDispatchFlyMsgQueueReadTVThread)
	{
		WaitReturn = WaitForSingleObject(pFlyExBoxInfo->hMsgQueueTVToEXBOXCreate, INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			RETAILMSG(1, (TEXT("\r\nFlyAudio ExBox ThreadRead Event hMsgQueueToSystemOpen Error")));
		}
		ReadMsgQueue(pFlyExBoxInfo->hMsgQueueTVToEXBOXCreate, buff, FLY_MSG_QUEUE_TV_TO_EXBOX_LENGTH, &dwRes, 0, &dwMsgFlag);

		if(dwRes)
		{
			RETAILMSG(1, (TEXT("\r\nFlyAudio ExBox Read MsgQueue From TV%d %x %x"),dwRes,buff[0],buff[1]));
			if(buff[0] == 0x04)
			{
				SendFcwLdwInfo(pFlyExBoxInfo,&buff[1],(UINT16)(dwRes-1));
			}
			else
			{
				FlyExBoxSendCmdToUART(pFlyExBoxInfo,buff[0],buff[1]);
			}
		}
	}
	RETAILMSG(1, (TEXT("\r\nFlyAudio ExBox ThreadRead exit")));
	pFlyExBoxInfo->FLyMsgQueueReadTVThreadHandle = NULL;
	return 0;
}
static DWORD WINAPI ThreadRead_TPMS(LPVOID pContext)
{
	P_FLY_EXBOX_INFO pFlyExBoxInfo = (P_FLY_EXBOX_INFO)pContext;
	ULONG WaitReturn;
	BYTE buff[FLY_MSG_QUEUE_TPMS_TO_EXBOX_LENGTH];
	DWORD dwRes;
	DWORD dwMsgFlag;

	while (!pFlyExBoxInfo->bKillDispatchFlyMsgQueueReadTPMSThread)
	{
		WaitReturn = WaitForSingleObject(pFlyExBoxInfo->hMsgQueueTPMSToEXBOXCreate, INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			RETAILMSG(1, (TEXT("\r\nFlyAudio ExBox ThreadRead Event hMsgQueueToSystemOpen Error")));
		}
		ReadMsgQueue(pFlyExBoxInfo->hMsgQueueTPMSToEXBOXCreate, buff, FLY_MSG_QUEUE_TPMS_TO_EXBOX_LENGTH, &dwRes, 0, &dwMsgFlag);

		if(dwRes)
		{
			RETAILMSG(1, (TEXT("\r\nFlyAudio ExBox Read MsgQueue From TPMS%d %x %x"),dwRes,buff[0],buff[1]));
			FlyExBoxSendCmdToUART(pFlyExBoxInfo,buff[0],buff[1]);
		}
	}
	RETAILMSG(1, (TEXT("\r\nFlyAudio ExBox ThreadRead exit")));
	pFlyExBoxInfo->FLyMsgQueueReadTPMSThreadHandle = NULL;
	return 0;
}
static DWORD WINAPI ThreadMain(LPVOID pContext)
{
	P_FLY_EXBOX_INFO pFlyExBoxInfo = (P_FLY_EXBOX_INFO)pContext;

	while (!pFlyExBoxInfo->bKillDispatchFlyMainThread)
	{
		WaitForSingleObject(pFlyExBoxInfo->hDispatchMainThreadEvent, 5000);
		//reserve
		if (IpcWhatEvent(EVENT_GLOBAL_EXBOX_INPUT_CHANGE_ID))
		{		
			IpcClearEvent(EVENT_GLOBAL_EXBOX_INPUT_CHANGE_ID);

		    if(pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eAudioInput ==IPOD )
			{
				FlyExBoxSendCmdToUART(pFlyExBoxInfo,0xf1,0x05);
			}
			else if (pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eAudioInput == TV) {
				FlyExBoxSendCmdToUART(pFlyExBoxInfo,0xf1,0x03);
				FlyExBoxSendCmdToUART(pFlyExBoxInfo,0xf2,0x00);
				DBGD((TEXT("\r\nFlyAudio ExBox Input Changed to TV")));
			}
			else if (pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eAudioInput == SRADIO) {
				FlyExBoxSendCmdToUART(pFlyExBoxInfo,0xf1,0x04);
				DBGD((TEXT("\r\nFlyAudio ExBox Input Changed to XM(SRADIO)")));
			}
			else //if (pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eAudioInput == AUX)  
			{
				FlyExBoxSendCmdToUART(pFlyExBoxInfo,0xf1,0x01);
				FlyExBoxSendCmdToUART(pFlyExBoxInfo,0xf2,0x01);
				DBGD((TEXT("\r\nFlyAudio ExBox Input Changed to CDC")));
			}
		}
		
		if((pFlyExBoxInfo->bHaveExBox)&&((GetTickCount()-pFlyExBoxInfo->ulExBoxSyncTimer)>4000))
		{
			pFlyExBoxInfo->bHaveExBox =FALSE;
			pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTVModule = FALSE;
			IpcStartEvent(EVENT_GLOBAL_TVBOX_EXIST_ID);
			pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTPMSModule = FALSE;
			IpcStartEvent(EVENT_GLOBAL_TPMSBOX_EXIST_ID);
		}
		
		if(IpcWhatEvent(EVENT_GLOBAL_GOT_CARNAME_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_GOT_CARNAME_ID);
			//if(pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName != CAR_CRV_12)
			{
				openExBoxComm(pFlyExBoxInfo);
			}
			IpcStartEvent(EVENT_GLOBAL_DEALED_CARNAME_ID);
			
		}
	}

	pFlyExBoxInfo->FlyMainThreadHandle = NULL;
	CloseHandle(pFlyExBoxInfo->hDispatchMainThreadEvent);
	RETAILMSG(1, (TEXT("\r\nFlyAudio ExBox ThreadMain exit")));
	return 0;
}
static VOID FlyExBox_Enable(P_FLY_EXBOX_INFO pFlyExBoxInfo,BOOL bEnable)
{
	DWORD dwThreadID;
	if (bEnable)
	{
		if (pFlyExBoxInfo->bExBoxEnable)
		{
			return;
		}
		pFlyExBoxInfo->bExBoxEnable = TRUE;

		MSGQUEUEOPTIONS  msgOpts;
		//TPMS 
		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TPMS_TO_EXBOX_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TPMS_TO_EXBOX_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pFlyExBoxInfo->hMsgQueueTPMSToEXBOXCreate = CreateMsgQueue(FLY_MSG_QUEUE_TPMS_TO_EXBOX_NAME, &msgOpts);
		if (NULL == pFlyExBoxInfo->hMsgQueueTPMSToEXBOXCreate)
		{
			DBGE((TEXT("\r\nFlyAudio TPMS Create MsgQueue To EXBOX Fail!")));
		}

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_EXBOX_TO_TPMS_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_EXBOX_TO_TPMS_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pFlyExBoxInfo->hMsgQueueExBoxToTMPSCreate = CreateMsgQueue(FLY_MSG_QUEUE_EXBOX_TO_TPMS_NAME, &msgOpts);
		if (NULL == pFlyExBoxInfo->hMsgQueueExBoxToTMPSCreate)
		{
			DBGE((TEXT("\r\nFlyAudio TPMS Create MsgQueue FROM EXBOX Fail!")));
		}
		pFlyExBoxInfo->bKillDispatchFlyMsgQueueReadTPMSThread = FALSE;
		pFlyExBoxInfo->FLyMsgQueueReadTPMSThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //��ȫ����
			0,//��ʼ���߳�ջ�Ĵ�С��ȱʡΪ�����̴߳�С��ͬ
			(LPTHREAD_START_ROUTINE)ThreadRead_TPMS, //�̵߳�ȫ�ֺ���
			pFlyExBoxInfo, //�˴�����������ܵľ��
			0, &dwThreadID );
		DBGE((TEXT("\r\nFlyAudio ExBox ThreadRead_TPMS ID:%x"),dwThreadID));
		if (NULL == pFlyExBoxInfo->FLyMsgQueueReadTPMSThreadHandle)
		{
			pFlyExBoxInfo->bKillDispatchFlyMsgQueueReadTPMSThread = TRUE;
			return;
		}
		//TV
		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TV_TO_EXBOX_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TV_TO_EXBOX_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pFlyExBoxInfo->hMsgQueueTVToEXBOXCreate = CreateMsgQueue(FLY_MSG_QUEUE_TV_TO_EXBOX_NAME, &msgOpts);
		if (NULL == pFlyExBoxInfo->hMsgQueueTVToEXBOXCreate)
		{
			DBGE((TEXT("\r\nFlyAudio TV Create MsgQueue To EXBOX Fail!")));
		}

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_EXBOX_TO_TV_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_EXBOX_TO_TV_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pFlyExBoxInfo->hMsgQueueExBoxToTVCreate = CreateMsgQueue(FLY_MSG_QUEUE_EXBOX_TO_TV_NAME, &msgOpts);
		if (NULL == pFlyExBoxInfo->hMsgQueueExBoxToTVCreate)
		{
			DBGE((TEXT("\r\nFlyAudio TV Create MsgQueue FROM EXBOX Fail!")));
		}
		pFlyExBoxInfo->bKillDispatchFlyMsgQueueReadTVThread = FALSE;
		pFlyExBoxInfo->FLyMsgQueueReadTVThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //��ȫ����
			0,//��ʼ���߳�ջ�Ĵ�С��ȱʡΪ�����̴߳�С��ͬ
			(LPTHREAD_START_ROUTINE)ThreadRead_TV, //�̵߳�ȫ�ֺ���
			pFlyExBoxInfo, //�˴�����������ܵľ��
			0, &dwThreadID );
		DBGE((TEXT("\r\nFlyAudio ExBox ThreadRead_TV ID:%x"),dwThreadID));
		if (NULL == pFlyExBoxInfo->FLyMsgQueueReadTVThreadHandle)
		{
			pFlyExBoxInfo->bKillDispatchFlyMsgQueueReadTVThread = TRUE;
			return;
		}

		//main
		pFlyExBoxInfo->bKillDispatchFlyMainThread = FALSE;
		pFlyExBoxInfo->hDispatchMainThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
		pFlyExBoxInfo->FlyMainThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //��ȫ����
			0,//��ʼ���߳�ջ�Ĵ�С��ȱʡΪ�����̴߳�С��ͬ
			(LPTHREAD_START_ROUTINE)ThreadMain, //�̵߳�ȫ�ֺ���
			pFlyExBoxInfo, //�˴�����������ܵľ��
			0, &dwThreadID );
		DBGE((TEXT("\r\nFlyAudio ExBox ThreadMain ID:%x"),dwThreadID));
		if (NULL == pFlyExBoxInfo->FlyMainThreadHandle)
		{
			pFlyExBoxInfo->bKillDispatchFlyMainThread = TRUE;
			return;
		}
		SetEvent(pFlyExBoxInfo->hDispatchMainThreadEvent);//��ʼ����һ��
	}
	else
	{
		if (!pFlyExBoxInfo->bExBoxEnable)
		{
			return;
		}
		pFlyExBoxInfo->bExBoxEnable = FALSE;

		pFlyExBoxInfo->bKillDispatchFlyMsgQueueReadTPMSThread = TRUE;
		while (pFlyExBoxInfo->FLyMsgQueueReadTPMSThreadHandle)
		{
			SetEvent(pFlyExBoxInfo->hMsgQueueTPMSToEXBOXCreate);
			Sleep(10);
		}
		CloseMsgQueue(pFlyExBoxInfo->hMsgQueueTPMSToEXBOXCreate);
		CloseMsgQueue(pFlyExBoxInfo->hMsgQueueExBoxToTMPSCreate);

		pFlyExBoxInfo->bKillDispatchFlyMsgQueueReadTVThread = TRUE;
		while (pFlyExBoxInfo->FLyMsgQueueReadTVThreadHandle)
		{
			SetEvent(pFlyExBoxInfo->hMsgQueueTVToEXBOXCreate);
			Sleep(10);
		}
		CloseMsgQueue(pFlyExBoxInfo->hMsgQueueTVToEXBOXCreate);
		CloseMsgQueue(pFlyExBoxInfo->hMsgQueueExBoxToTVCreate);

		pFlyExBoxInfo->bKillDispatchFlyMainThread = TRUE;
		while (pFlyExBoxInfo->FlyMainThreadHandle)
		{
			SetEvent(pFlyExBoxInfo->hDispatchMainThreadEvent);
			Sleep(10);
		}
		closeExBoxComm(pFlyExBoxInfo);    
	}
}

void IpcRecv(UINT32 iEvent)
{
	P_FLY_EXBOX_INFO	pFlyExBoxInfo = (P_FLY_EXBOX_INFO)gpFlyExBoxInfo;

	DBGD((TEXT("\r\n FLY ExBox Recv IPC Event:%d"),iEvent));

	SetEvent(pFlyExBoxInfo->hDispatchMainThreadEvent);
}

HANDLE FEB_Init(DWORD dwContext)   //�ַ�����ָ��ע����м�¼���������ļ�
{

	//�����ڴ�
	P_FLY_EXBOX_INFO pFlyExBoxInfo = (P_FLY_EXBOX_INFO)LocalAlloc(LPTR, sizeof(FLY_EXBOX_INFO));
	if (!pFlyExBoxInfo)
	{
		RETAILMSG(1, (TEXT("FlyAudio ExBox LocalAlloc Failed!\r\n")));
		return NULL;
	}
	gpFlyExBoxInfo = pFlyExBoxInfo;

	pFlyExBoxInfo->pFlyShmGlobalInfo = CreateShm(EXBOX_MODULE,IpcRecv);
	if (NULL == pFlyExBoxInfo->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio ExBox Init create shm err\r\n")));
		return NULL;
	}
	

	if (pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalStructSize
		!= sizeof(struct shm))
	{
		pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[0] = 'F';
		pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[1] = 'E';
		pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[2] = 'B';
	}

	//��ʼ������
	powerOnFirstInit(pFlyExBoxInfo);
	FlyExBox_Enable(pFlyExBoxInfo,TRUE);

	//pFlyExBoxInfo->hHandleGlobalGlobalEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalExBoxInfo.bInit = TRUE;
	pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalExBoxInfo.bOpen = TRUE;
	pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalExBoxInfo.iDriverCompYear = year;
	pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalExBoxInfo.iDriverCompMon = months;
	pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalExBoxInfo.iDriverCompDay = day;
	pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalExBoxInfo.iDriverCompHour = hours;
	pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalExBoxInfo.iDriverCompMin = minutes;
	pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalExBoxInfo.iDriverCompSec = seconds;
	
	RETAILMSG(1, (TEXT("\r\nFlyAudio ExBox Init Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return (HANDLE)pFlyExBoxInfo;
}

BOOL FEB_Deinit(
				DWORD hDeviceContext		//XXX_Init()�������ص��豸������
				)
{
	P_FLY_EXBOX_INFO pFlyExBoxInfo = (P_FLY_EXBOX_INFO)hDeviceContext;

#if GLOBAL_COMM
	//ȫ��
	pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalExBoxInfo.bOpen = FALSE;
	pFlyExBoxInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalExBoxInfo.bInit = FALSE;
	//SetEvent(pFlyExBoxInfo->hHandleGlobalGlobalEvent);
	//CloseHandle(pFlyExBoxInfo->hHandleGlobalGlobalEvent);
#endif
	FlyExBox_Enable(pFlyExBoxInfo,FALSE);
	LocalFree(pFlyExBoxInfo);
	RETAILMSG(1, (TEXT("FlyAudio ExBox LocalFree!\r\n")));
	return TRUE;
}


DWORD FEB_Open(
			   DWORD hDeviceContext,	//�豸�����ģ���XXX_Init()��������
			   DWORD AccessCode,		//�豸�ķ���ģʽ����CreateFile()��������
			   DWORD ShareMode			//�豸�Ĺ���ģʽ����CreateFile()��������
			   )
{
	P_FLY_EXBOX_INFO pFlyExBoxInfo = (P_FLY_EXBOX_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;

	RETAILMSG(1, (TEXT("FlyAudio ExBox Open Build:Date%d.%d.%dTime%d:%d:%d\r\n"),
		year,months,day,hours,minutes,seconds));

	return returnWhat;
}

BOOL FEB_Close(
			   DWORD hDeviceContext		//�豸�Ĵ������ģ���XXX_Open������������
			   )
{
	P_FLY_EXBOX_INFO pFlyExBoxInfo = (P_FLY_EXBOX_INFO)hDeviceContext;

	RETAILMSG(1, (TEXT("FlyAudio ExBox Close!\r\n")));
	return TRUE;
}
VOID FEB_PowerUp(DWORD hDeviceContext)
{
	P_FLY_EXBOX_INFO pFlyExBoxInfo = (P_FLY_EXBOX_INFO)hDeviceContext;	
	
	RETAILMSG(1, (TEXT("FlyAudio ExBox PowerUp!\r\n")));
}
VOID FEB_PowerDown(DWORD hDeviceContext)
{
	P_FLY_EXBOX_INFO pFlyExBoxInfo = (P_FLY_EXBOX_INFO)hDeviceContext;

	RETAILMSG(1, (TEXT("FlyAudio ExBox PowerDown!\r\n")));
}
DWORD FEB_Read(
			   DWORD  hOpenContext,		//XXX_Open�������ص��豸��������
			   LPVOID pBuffer,			//�������������ָ�룬��ȡ���ݻᱻ���ڸû�������
			   DWORD  Count				//Ҫ��ȡ���ֽ���
			   )
{
	UINT dwRead = 0;
	BYTE *SendBuf = (BYTE *)pBuffer;
	P_FLY_EXBOX_INFO pFlyExBoxInfo = (P_FLY_EXBOX_INFO)hOpenContext;

	return dwRead;//�����������ֽڴ�С��������0ʱ���û��̿�ʼ������
}
DWORD FEB_Write(
				DWORD    hOpenContext,		// XXX_Open���ص��豸��������
				LPCVOID  pSourceBytes,		//���룬ָ��Ҫд���豸�����ݵĻ���
				DWORD    NumberOfBytes		//�����е����ݵ��ֽ���
				)
{
	P_FLY_EXBOX_INFO pFlyExBoxInfo = (P_FLY_EXBOX_INFO)hOpenContext;

	return NULL;
}

DWORD FEB_Seek(
			   DWORD  hOpenContext,		//XXX_Open()���ص��豸��������
			   LONG   Amount,			//Ҫ�ƶ��ľ��룬������ʾǰ�ƣ�������ʾ����
			   DWORD  Type				//�����е����ݵ��ֽ���
			   )
{
	RETAILMSG(1, (TEXT("FlyAudio ExBox Seek!\r\n")));
	return NULL;
}
/********************************************************************************************************
*�������ƣ�
*�������ܣ������������Ϳ�������  ��Ӧ�ó���DeviceIoControl()�����ݶ�Ӧ
*���������
*���������
*�� �� ֵ������ֵ��TRUE��ʾ�����ɹ���FALSE��ʾ����ʧ��
**********************************************************************************************************/
BOOL FEB_IOControl(
				   DWORD  hOpenContext,		//��XXX_Open()���ص��豸��������
				   DWORD  dwCode,			//Ҫ���͵Ŀ����룬һ��32λ�޷�����
				   PBYTE  pBufIn,			//���룬ָ�����뻺������ָ��
				   DWORD  dwLenIn,			//���뻺�����ĳ���
				   PBYTE  pBufOut,			//�����ָ�������������ָ��
				   DWORD  dwLenOut,			//�������������󳤶�
				   PDWORD pdwActualOut		//������豸ʵ��������ֽ���
				   )
{
	P_FLY_EXBOX_INFO pFlyExBoxInfo = (P_FLY_EXBOX_INFO)hOpenContext;
	RETAILMSG(1, (TEXT("FlyAudio ExBox IOControl!\r\n")));
	BOOL bRes = TRUE;
	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	DBGD((TEXT("\r\nFlyAudio ExBox IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)"),
	//		hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	DBGD((TEXT("\r\nFlyAudio ExBox IOControl Set IOCTL_SERIAL_SET_WAIT_MASK")));
	//		break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	//DBGD((TEXT("\r\nFlyAudio Parrot IOControl Set IOCTL_SERIAL_WAIT_ON_MASK")));
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
		RETAILMSG(1,(TEXT("Attach in FlyAudio IPOD DllEntry")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		RETAILMSG(1,(TEXT("Dettach in FlyAudio IPOD DllEntry")));
	}

	RETAILMSG(1,(TEXT("Leaving in FlyAudio IPOD DllEntry")));

	return (TRUE);
}
