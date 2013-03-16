// -----------------------------------------------------------------------------
// File Name    : FLYGlobal.cpp
// Title        : FlyGlobal Driver
// Author       : JQilin - Copyright (C) 2011
// Created      : 2011-02-25  
// Version      : 0.01
// Target MCU   : WinCE Driver
// -----------------------------------------------------------------------------
// Version History:
/*
>>>2011-03-03: V0.02 添加事件传递功能
>>>2011-02-25: V0.01 first draft
*/
// FLYGlobal.cpp : Defines the entry point for the DLL application.
//

#include "flyhwlib.h"
#include "flyuserlib.h"
#include "flysocketlib.h"

#include "FLYGlobal.h"
#include "FILECLASS.h"

P_FLY_GLOBAL_INFO gpGlobalInfo = NULL;

void GlobalEnable(P_FLY_GLOBAL_INFO pGlobalInfo,BOOL bEnable);
VOID FGB_PowerUp(DWORD hDeviceContext);
VOID FGB_PowerDown(DWORD hDeviceContext);


static UINT16 GetVolume4BTRingVolume(UINT16 iMaxVolume)
{
	UINT16 iVolume = 0;
	switch(iMaxVolume)
	{
	case 30:
	case 35:
		iVolume = 8;
		break;

	case 40:
	case 45:
		iVolume=10;
		break;

	default:
		iVolume = 15;
		break;
	}

	return iVolume;
}

/********************************************************************************************************
*函数名称：ConfigTheVolumePara
*函数功能：根据车型配置相关的参数
*输入参数：
*输出参数：
*返 回 值：
*作者    ：海盗
**********************************************************************************************************/

static void ConfigTheVolumePara(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	BOOL bHaveRadar=0;

	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax = NORMAL_VOLUME_MAX;
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp = FALSE;

	if(0x00== (memcmp((void*)&pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion[2],"007B-09-",sizeof("007B-09-")-1)))
	{//途观的音量最大值是30
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax = 30;
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName = CAR_TUGUAN_07B_9;
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp =FALSE;
	}
	else if(0x00== (memcmp((void*)&pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion[2],"007B-17-",sizeof("007B-17-")-1)))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax = 30;
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName = CAR_TUGUAN_07B_17;
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp =FALSE;
	}
	else if(0x00== (memcmp((void*)&pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion[2],"007B-16-",sizeof("007B-16-")-1)))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax = 30;
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName = CAR_TUGUAN_07B_16;
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp =FALSE;
	}
	else if(0x00== (memcmp((void*)&pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion[2],"022",sizeof("022")-1)))
	{//IX35的音量最大值是35
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax = 30;
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName = CAR_ZHISHENG;
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp = FALSE;
	}
	else if(0x00== (memcmp((void*)&pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion[2],"066",sizeof("066")-1)))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName = CAR_CAMRY_12;
	}
	else if(0x00== (memcmp((void*)&pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion[2],"071A-01-",sizeof("071A-01-")-1)))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax = 45;
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName = CAR_OUTLANDER;
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp = TRUE;		
	}
	else if(0x00== (memcmp((void*)&pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion[2],"088A-01-",sizeof("088A-01-")-1))
		||0x00== (memcmp((void*)&pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion[2],"062B-01-",sizeof("062B-01-")-1)))
	{//IX35的音量最大值是35
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax = 35;
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName = CAR_IX35_88_1;
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp = TRUE;
	}
	else if(0x00== (memcmp((void*)&pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion[2],"060",sizeof("060")-1))
		||0x00== (memcmp((void*)&pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion[2],"010",sizeof("010")-1)))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName = CAR_CRV_12;	
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax = 40;
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp = FALSE;
	}
	else if(0x00== (memcmp((void*)&pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion[2],"045",sizeof("045")-1)))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName = CAR_MAZDA_6;	
	}
	else if(0x00== (memcmp((void*)&pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion[2],"035",sizeof("035")-1))
		||0x00== (memcmp((void*)&pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion[2],"035",sizeof("035")-1)))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax = 30;
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName = CAR_FOCUS;	
	}
	else
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName =CAR_NORMAL;
	}

	//IpcStartEvent(EVENT_GLOBAL_RADIO_ANTENNA_ID);//有些车型需要关闭收音天线电源

	if(0x00== (memcmp((void*)&pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion[2],"069",sizeof("069")-1)))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBackLowestVolume =0;
	}
	else
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBackLowestVolume =NORMAL_VOLUME_BACK_LOWEST*pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax/NORMAL_VOLUME_MAX;
	}

	//pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume = NORMAL_VOLUME_DEFAULT*pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax/NORMAL_VOLUME_MAX;
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iFlyAudioAmpVolume =NORMAL_VOLUME_AMP*pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax/NORMAL_VOLUME_MAX;		
	
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume = 
		GetVolume4BTRingVolume(pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax);
	DBGE((_T("\r\niVolumeMax:%d"),pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax));
}

static void ReadPanelSteeringData(P_FLY_GLOBAL_INFO pGlobalInfo,UINT iSteelwheelIndex);

/********************************************************************************************************
*函数名称：ShareMemoryInit
*函数功能：
*输入参数：
*输出参数：
*返 回 值：
**********************************************************************************************************/
static VOID ShareMemoryWriteInit(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	pGlobalInfo->WriteDataBufLen = 0;
	memset(pGlobalInfo->WriteDataBuf, 0, sizeof(pGlobalInfo->WriteDataBuf));

	pGlobalInfo->ReadDataBufLen  = 0;
	memset(pGlobalInfo->ReadDataBuf,0,sizeof(pGlobalInfo->ReadDataBuf));

	pGlobalInfo->hMemoryMap = NULL;
	pGlobalInfo->lpData = NULL;

	pGlobalInfo->LEDTestShareMemoryReadThreadHandle = NULL;
	pGlobalInfo->bKillDispatchpLEDTestShareMemoryReadThread = TRUE;

}
/********************************************************************************************************
*函数名称：OnCreateMemory
*函数功能：
*输入参数：
*输出参数：
*返 回 值：
**********************************************************************************************************/
static VOID OnCreateWriteMemory(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	//共享RAM
	pGlobalInfo->hMemoryMap = CreateFileMapping(INVALID_HANDLE_VALUE,NULL, PAGE_READWRITE|SEC_COMMIT,
		0, CAN_WRITE_DATA, L"DataMap");
	if (pGlobalInfo->hMemoryMap != NULL)
	{
		//pGlobalInfo->lpData = (LPBYTE)MapViewOfFile(pGlobalInfo->hMemoryMap, FILE_MAP_WRITE,0,0,0);
		pGlobalInfo->lpData = (P_SHARE_MEMORY_WRITE_BUF)MapViewOfFile(pGlobalInfo->hMemoryMap, FILE_MAP_WRITE,0,0,0);
		if (pGlobalInfo->lpData == NULL)
		{
			CloseHandle(pGlobalInfo->hMemoryMap);
			pGlobalInfo->hMemoryMap = NULL;
			RETAILMSG(1, (TEXT("FlyAudio ShareMemory MapViewOfFile failed!\r\n")));
			return;
		}

		RETAILMSG(1, (TEXT("FlyAudio ShareMemory  Success!\r\n")));
	}
	else
	{
		RETAILMSG(1, (TEXT("FlyAudio ShareMemory CreateFileMapping failed!\r\n")));
		return;
	}

	DWORD dwThreadID;
	pGlobalInfo->bKillDispatchpLEDTestShareMemoryReadThread = FALSE;
	pGlobalInfo->LEDTestShareMemoryReadThreadHandle = 
		CreateThread( (LPSECURITY_ATTRIBUTES) NULL,0,(LPTHREAD_START_ROUTINE)LEDTestShareMemoryReadThread,pGlobalInfo,0, &dwThreadID );
	if (NULL == pGlobalInfo->LEDTestShareMemoryReadThreadHandle)
	{
		pGlobalInfo->bKillDispatchpLEDTestShareMemoryReadThread = TRUE;
		return;
	}

	pGlobalInfo->bMemoryMapHave = TRUE;
}
/********************************************************************************************************
*函数名称：DestoryMemory
*函数功能：
*输入参数：
*输出参数：
*返 回 值：
**********************************************************************************************************/
static VOID DestoryWriteMemory(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	pGlobalInfo->bKillDispatchpLEDTestShareMemoryReadThread = TRUE;
	while (pGlobalInfo->LEDTestShareMemoryReadThreadHandle)
	{
		SetEvent(pGlobalInfo->hOnReadMemoryEvent);
		Sleep(10);
	}

	if (pGlobalInfo->lpData != NULL)
	{
		UnmapViewOfFile(pGlobalInfo->lpData);
		pGlobalInfo->lpData = NULL;
		RETAILMSG(1, (TEXT("\r\nShareMemory Free MapViewOfFile()!\r\n")));
	}

	if (pGlobalInfo->hMemoryMap != NULL)
	{
		CloseHandle(pGlobalInfo->hMemoryMap);
		pGlobalInfo->hMemoryMap = NULL;
		RETAILMSG(1, (TEXT("FlyAudio ShareMemory Close CreateFileMapping()!\r\n")));
	}
}
/********************************************************************************************************
*函数名称：OnWriteMemory
*函数功能：
*输入参数：
*输出参数：
*返 回 值：
**********************************************************************************************************/
//static VOID OnWriteMemory(P_FLY_GLOBAL_INFO pGlobalInfo)
//{
//	if (pGlobalInfo->lpData != 0)
//	{
//		memcpy(pGlobalInfo->lpData, pGlobalInfo->WriteDataBuf, pGlobalInfo->WriteDataBufLen);
//
//		RETAILMSG(1, (TEXT("\r\nShareMemory Dll Write:")));
//		for(UINT i= 0; i<pGlobalInfo->WriteDataBufLen; i++)
//		{
//			RETAILMSG(1, (TEXT("%d"), pGlobalInfo->WriteDataBuf[i]));
//		}
//
//	}
//}

static DWORD WINAPI LEDTestShareMemoryReadThread(LPVOID pContex)
{
	P_FLY_GLOBAL_INFO pGlobalInfo = (P_FLY_GLOBAL_INFO)pContex;
	while (!pGlobalInfo->bKillDispatchpLEDTestShareMemoryReadThread)
	{
		WaitForSingleObject(pGlobalInfo->hOnReadMemoryEvent, INFINITE);
		RETAILMSG(1, (TEXT("\r\nShareMemoryLEDTestShareMemoryReadThread Read")));
		OnReadMemory(pGlobalInfo);
	}
	pGlobalInfo->LEDTestShareMemoryReadThreadHandle = NULL;
	RETAILMSG(1, (TEXT("\r\nShareMemoryLEDTestShareMemoryReadThread exit!")));
	return 0;
}

static VOID OnReadMemory(P_FLY_GLOBAL_INFO pGlobalInfo)
{

	if ((pGlobalInfo->lpData + READ_DATA) != 0)
	{
		memcpy(pGlobalInfo->ReadDataBuf,pGlobalInfo->lpData+READ_DATA, READ_DATA);

		pGlobalInfo->ReadDataBufLen = pGlobalInfo->ReadDataBuf[0];
		RETAILMSG(1, (TEXT("\r\nShareMemory Dll Read:%X "),pGlobalInfo->ReadDataBufLen));
		
		for (UINT i=0; i<pGlobalInfo->ReadDataBufLen; i++)
		{
			RETAILMSG(1, (TEXT("%X "),pGlobalInfo->ReadDataBuf[i+1]));
		}
	}
}


static void RegDataWriteSoftWareVersion(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	HKEY hKey; 
	DWORD dwDisposition;
	DWORD dwLen;
	BYTE pVersion[20];
	int i;

#if REGEDIT_NOT_SAVE
	return;
#endif

	DBGI((TEXT("\r\nRegidit software version !!!!!")));


	RegCreateKeyEx(HKEY_CURRENT_USER,L"Software\\FLYAUDIO\\NAVI",0,NULL,0,
		0,NULL,&hKey,&dwDisposition); /////////////////////////////////创建或打开注册表


	dwLen = 20;
	memcpy(pVersion,(void*)pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion,20);
	
	RegSetValueEx(hKey,L"dwVersion",0,REG_BINARY,(unsigned char *)pVersion,dwLen);

	RegCloseKey(hKey);

	DBGI((TEXT("\r\nVersion write finish ")));

	for(i=0;i<20;i++)
	{
		DBGI((TEXT("%c"),pVersion[i]));
	}
}

static void RegDataReadSoftWareVersion(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	HKEY hKey; 
	DWORD dwDisposition;
	DWORD dwTemp;
	DWORD dwLen;
	DWORD dwType;
	int i;

	unsigned char pVersion[20];

#if REGEDIT_NOT_SAVE
	return;
#endif

	DBGI((TEXT("\r\nRead software version !!!!!")));;


	RegCreateKeyEx(HKEY_CURRENT_USER,L"Software\\FLYAUDIO\\NAVI",0,NULL,0,
		0,NULL,&hKey,&dwDisposition); /////////////////////////////////创建或打开注册表

	dwTemp = 0;
	dwLen = 20;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwVersion",0,&dwType,(unsigned char *)pVersion,&dwLen))
	{
	
	}
	RegCloseKey(hKey);	
	DBGI((TEXT("\r\nVersion read:")));
	for(i=0;i<20;i++)
	{
		DBGI((TEXT("%c"),pVersion[i]));
	}
}

void returnGlobalPowerMode(P_FLY_GLOBAL_INFO pGlobalInfo,BYTE bPower)
{
	BYTE buff[2];
	buff[0] = 0x01;
	buff[1] = bPower;
	ReturnToUser(buff,2);
}

void returnGlobalWorkMode(P_FLY_GLOBAL_INFO pGlobalInfo,BYTE bWork)
{
	BYTE buff[2];
	buff[0] = 0x02;
	buff[1] = bWork;
	ReturnToUser(buff,2);
}

void returnGlobalOSDDebug(P_FLY_GLOBAL_INFO pGlobalInfo,UINT iLine,BYTE *p,UINT length)
{
	BYTE buff[1+1+OSD_DEBUG_WIDTH];
	buff[0] = 0x21;
	buff[1] = iLine;
	memcpy(&buff[2],p,length);
	ReturnToUser(buff,1+1+length);
}

void returnGlobalMemoryEventWhat(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	UINT i;
	BYTE buff[1+2];
	buff[0] = 0x11;

	for (i = 0;i < GLOBAL_MEMORY_EVENT_SAVE;i++)
	{
		if (pGlobalInfo->iGlobalMemoryEventWhat[i])
		{
			buff[1] = i;
			buff[2] = pGlobalInfo->iGlobalMemoryEventWhat[i];
			pGlobalInfo->iGlobalMemoryEventWhat[i] = 0;
			ReturnToUser(buff,3);
		}
	}
}

void setGlobalMemoryEventWhat(P_FLY_GLOBAL_INFO pGlobalInfo,enumGlobalMemoryChange iWhat)
{
	UINT iByteOffset,iBitOffset;
	iByteOffset = iWhat / 8;
	iBitOffset = iWhat % 8;
	pGlobalInfo->iGlobalMemoryEventWhat[iByteOffset] |= (1 << iBitOffset);
	SetEvent(pGlobalInfo->hOnWriteMemoryEvent);
}

static DWORD WINAPI ThreadLEDTest(LPVOID pContext)//LED灯测试错误
{
	P_FLY_GLOBAL_INFO pGlobalInfo = (P_FLY_GLOBAL_INFO)pContext;

	while (!pGlobalInfo->bKillDispatchFlyLEDTestThread)
	{
		if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLEDTestFlashCount)
		{
			WaitForSingleObject(pGlobalInfo->hDispatchLEDTestThreadEvent, 2000);
		}
		else
		{
			WaitForSingleObject(pGlobalInfo->hDispatchLEDTestThreadEvent, INFINITE);
		}
		IpcClearEvent(EVENT_GLOBAL_ERROR_LEDFLASH_ID);

		for (UINT i = 0;i < pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLEDTestFlashCount;i++)
		{
			HwGpioOutput(CONTROL_IO_LED_I,EJECT_LED_ON);
			Sleep(157);
			HwGpioOutput(CONTROL_IO_LED_I,EJECT_LED_OFF);
			Sleep(157);
		}
		Sleep(1000);
		for (UINT i = 0;i < pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLEDTestFlashCountSub;i++)
		{
			HwGpioOutput(CONTROL_IO_LED_I,EJECT_LED_ON);
			Sleep(157);
			HwGpioOutput(CONTROL_IO_LED_I,EJECT_LED_OFF);
			Sleep(157);
		}
	}
				
	pGlobalInfo->FlyLEDTestThreadHandle = NULL;
	RETAILMSG(1, (TEXT("\r\nGlobal ThreadLEDTest exit")));
	return 0;
}

static DWORD WINAPI ThreadRead(LPVOID pContext)
{
	P_FLY_GLOBAL_INFO pGlobalInfo = (P_FLY_GLOBAL_INFO)pContext;
	ULONG WaitReturn;
	BYTE buff[FLY_MSG_QUEUE_TO_GLOBAL_LENGTH];
	DWORD dwRes;
	DWORD dwMsgFlag;
	UINT i;

	while (!pGlobalInfo->bKillDispatchFlyMsgQueueReadThread)
	{
		WaitReturn = WaitForSingleObject(pGlobalInfo->hMsgQueueToGlobalCreate, INFINITE);

		ReadMsgQueue(pGlobalInfo->hMsgQueueToGlobalCreate, buff, FLY_MSG_QUEUE_TO_GLOBAL_LENGTH, &dwRes, 0, &dwMsgFlag);

		if(dwRes)
		{
			DBGD( (TEXT("\r\nGlobal Read MsgQueue%d %x %x"),dwRes,buff[0],buff[1]));

			if (buff[0] < OSD_DEBUG_LINES
				&& dwRes < (1+OSD_DEBUG_WIDTH))
			{
				for (i = 0;i < dwRes-1;i++)
				{
					pGlobalInfo->sDemoStr[buff[0]][i] = buff[i+1];
				}
				pGlobalInfo->iDemoStrLength[buff[0]] = i;
			}
		}
	}
	pGlobalInfo->LEDTestMsgQueueReadThreadHandle = NULL;
	DBGD((TEXT("\r\nGlobal ThreadRead exit")));
	return 0;
}

static DWORD WINAPI ThreadMain(LPVOID pContext)
{
	P_FLY_GLOBAL_INFO pGlobalInfo = (P_FLY_GLOBAL_INFO)pContext;
	UINT i;

	while (!pGlobalInfo->bKillDispatchFlyMainThread)
	{
		if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bOSDDemoMode)
		{
			WaitForSingleObject(pGlobalInfo->hDispatchMainThreadEvent, 618);
		} 
		else
		{
			WaitForSingleObject(pGlobalInfo->hDispatchMainThreadEvent, 5000);
		}

		if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSuccessReadDebugFileFromSDMEM)
		{
			//debugParaReadFromFile(pGlobalInfo,1);//启动过程中读SD卡可能出现问题
		}
	
		/////////////////////////////////////////////////////
		//新加，返回音量
		if (IpcWhatEvent(EVENT_GLOBAL_VLAULE_TO_USER))
		{
			IpcClearEvent(EVENT_GLOBAL_VLAULE_TO_USER);
			if (pGlobalInfo->bMemoryMapHave)
			{
				if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute)
				{
					pGlobalInfo->lpData->iMainVolume = 0x80 | pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
				} 
				else
				{
					pGlobalInfo->lpData->iMainVolume = pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
				}
				setGlobalMemoryEventWhat(pGlobalInfo,GLOBAL_MEMORY_EVENT_VOLUME);
			}
		}


		if (IpcWhatEvent(EVENT_GLOBAL_REMOTE_CHANGE_IT_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_REMOTE_CHANGE_IT_ID);
			ReadPanelSteeringData(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataUseWhat);
			pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bReadUpdateData = TRUE;
			IpcStartEvent(EVENT_GLOBAL_PANELKEY_USE_IT_ID);
			IpcStartEvent(EVENT_GLOBAL_REMOTE_USE_IT_ID);
			IpcStartEvent(EVENT_GLOBAL_REMOTE_RETURN_IT_ID);
			IpcStartEvent(EVENT_GLOBAL_RETURN_FLASH_DATA_ID);
		}

		if (IpcWhatEvent(EVENT_GLOBAL_DEMO_OSD_START_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_DEMO_OSD_START_ID);
			RETAILMSG(1, (TEXT("\r\nGlobal Demo Start")));
		}
		if (IpcWhatEvent(EVENT_GLOBAL_DEMO_OSD_DISPLAY_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_DEMO_OSD_DISPLAY_ID);
		}

		////////////////////////////////////OSD调试信息
		if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bOSDDemoMode)
		{
			if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[0]
			|| pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[1]
			|| pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[2])
			{
				RETAILMSG(1, (TEXT("\r\nGlobal Struct Access Error:%c%c%c")
					,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[0]
				,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[1]
				,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[2]));
				demoOSDDriversCompileTime(pGlobalInfo);//编译时间
			}


			if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecUserPingStart)
			{
				demoOSDStateError(pGlobalInfo);//主机状态错误
				demoOSDOpenStatus(pGlobalInfo);//显示驱动打开状态
			}

			if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bOSDDemoMode)
			{
				demoOSDDriversCompileTime(pGlobalInfo);//编译时间
				demoOSDKeyADDisplay(pGlobalInfo);//显示AD值、按键序列
				demoOSDDVDSoftVersion(pGlobalInfo);//DVD版本号
				demoOSDHostTemperature(pGlobalInfo);//显示主机温度
				demoOSDBreakAndPhoneStatus(pGlobalInfo);//显示刹车状态
				demoOSDOpenStatus(pGlobalInfo);//显示驱动打开状态
				demoOSDOtherInfo(pGlobalInfo);//其它调试信息
			}

			for (i = 0;i < OSD_DEBUG_LINES;i++)//最终显示
			{
				if (pGlobalInfo->iDemoStrLength[i])
				{
					pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bOSDDemoMode = TRUE;//把其他的信息也打印出来

					//RETAILMSG(1, (TEXT("Global Demo Line:%d:"),i));
					//for (j = 0;j < pGlobalInfo->iDemoStrLength[i];j++)
					//{
					//	RETAILMSG(1, (TEXT("%c"),pGlobalInfo->sDemoStr[i][j]));
					//}
				
					returnGlobalOSDDebug(pGlobalInfo,i
						,pGlobalInfo->sDemoStr[i]
					,pGlobalInfo->iDemoStrLength[i]);

					pGlobalInfo->iDemoStrLength[i] = 0;
				}
			}
		}
	}
	//while (pGlobalInfo->FlyInterThreadHandle)
	//{
	//	SetEvent(pGlobalInfo->hHandleGlobalGlobalEvent);
	//	Sleep(10);
	//}
	pGlobalInfo->FlyMainThreadHandle = NULL;
	RETAILMSG(1, (TEXT("\r\nGlobal ThreadMain exit")));
	return 0;
}

static VOID WinCECommandProcessor(P_FLY_GLOBAL_INFO pGlobalInfo,BYTE *buf,UINT len)
{
	RETAILMSG(1, (TEXT(" OK!")));

	switch(buf[0])
	{
	case 0x01:
		if (0x01 == buf[1])
		{
			//eventInterResetEventResponseTimer(pGlobalInfo);
			returnGlobalPowerMode(pGlobalInfo,TRUE);
			returnGlobalWorkMode(pGlobalInfo,TRUE);
		}
		else if (0x00 == buf[1])
		{
			returnGlobalPowerMode(pGlobalInfo,FALSE);
		}
		break;
	case 0x10:
		if (0x00 == buf[1])
		{
			//eventInterResetEventResponseTimer(pGlobalInfo);
			pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecUserPingStart = TRUE;
			IpcStartEvent(EVENT_GLOBAL_USER_PING_START_ID);
		}
		else if (0x01 == buf[1])
		{
			IpcStartEvent(EVENT_GLOBAL_USER_PING_WORK_ID);
		}
		break;
	case 0x11:
		returnGlobalMemoryEventWhat(pGlobalInfo);
		break;
	case 0xA0://调试信息等级控制
		//if (buf[1] == 'F' && buf[2] == 'G' && buf[3] == 'B')
		//{
		//	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.debugMsgLevel = buf[4];
		//}
		//else if (buf[1] == 'F' && buf[2] == 'A' && buf[3] == 'U')
		//{
		//	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAudioInfo.debugMsgLevel = buf[4];
		//}
		//else if (buf[1] == 'F' && buf[2] == 'K' && buf[3] == 'Y')
		//{
		//	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.debugMsgLevel = buf[4];
		//}
		//else if (buf[1] == 'F' && buf[2] == 'C' && buf[3] == 'D')
		//{
		//	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDVDInfo.debugMsgLevel = buf[4];
		//}
		break;
	case 0xFF:
		if (0x01 == buf[1])
		{
			FGB_PowerUp((DWORD)pGlobalInfo);
		} 
		else if (0x00 == buf[1])
		{
			FGB_PowerDown((DWORD)pGlobalInfo);
		}
		break;
	default:
		break;
	}
}
void GlobalEnable(P_FLY_GLOBAL_INFO pGlobalInfo,BOOL bEnable)
{
	DWORD dwThreadID;

	if (bEnable)
	{
		if (pGlobalInfo->bPower)
		{
			//RETAILMSG(1,(TEXT("pGlobalInfo->bPower=%d"),pGlobalInfo->bPower));
			return;
		}

		pGlobalInfo->bKillDispatchFlyMainThread = FALSE;
		pGlobalInfo->hDispatchMainThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
		pGlobalInfo->FlyMainThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadMain, //线程的全局函数
			pGlobalInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGI((TEXT("\r\nGlobal ThreadMain ID:%x"),dwThreadID));
		if (NULL == pGlobalInfo->FlyMainThreadHandle)
		{
			DBGI((TEXT("\r\nGlobal ThreadMain Create Fail")));
			pGlobalInfo->bKillDispatchFlyMainThread = TRUE;
			return;
		}

		pGlobalInfo->bKillDispatchFlyLEDTestThread = FALSE;
		pGlobalInfo->hDispatchLEDTestThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
		pGlobalInfo->FlyLEDTestThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadLEDTest, //线程的全局函数
			pGlobalInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGI((TEXT("\r\nGlobal ThreadLEDTest ID:%x"),dwThreadID));
		if (NULL == pGlobalInfo->FlyLEDTestThreadHandle)
		{
			DBGI((TEXT("\r\nGlobal ThreadLEDTest Create Fail")));
			pGlobalInfo->bKillDispatchFlyLEDTestThread = TRUE;
			return;
		}

		pGlobalInfo->bPower = TRUE;

	}
	else
	{
		if (!pGlobalInfo->bPower)
		{
			return;
		}
		pGlobalInfo->bKillDispatchFlyLEDTestThread = TRUE;
		SetEvent(pGlobalInfo->hDispatchLEDTestThreadEvent);
	
		pGlobalInfo->bKillDispatchFlyMainThread = TRUE;
		SetEvent(pGlobalInfo->hDispatchMainThreadEvent);

		pGlobalInfo->bKillDispatchFlyInterThread = TRUE;
		//SetEvent(pGlobalInfo->hHandleGlobalGlobalEvent);

		while (pGlobalInfo->FlyMainThreadHandle)
		{
			SetEvent(pGlobalInfo->hDispatchMainThreadEvent);
			Sleep(10);
		}
		CloseHandle(pGlobalInfo->hDispatchMainThreadEvent);

		pGlobalInfo->bPower = FALSE;

	}
}
void readRadioAntData(P_FLY_GLOBAL_INFO pGlobalInfo,BYTE *p,UINT length)
{
	if(p[0] == 1)
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedSupplyAntPower = TRUE;
	}
	else
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedSupplyAntPower = FALSE;
	}
	DBGD((TEXT("\r\nAnt Power:x% "),p[0]));
	IpcStartEvent(EVENT_GLOBAL_RADIO_ANTENNA_ID);//有些车型需要关闭收音天线电源
}

//把取得的方向盘数据写入内存
void readPanelData(P_FLY_GLOBAL_INFO pGlobalInfo,BYTE *p,UINT length)
{
	UINT i;
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.valid[0] = 0xAA;
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.valid[1] = 0x55;
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.type = p[0];
	DBGD((TEXT("\r\nGolbal Panel Data Name ")));
	for (i = 0;i < 8;i++)
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.PanelName[i] = p[1+i];
		DBGD((TEXT(" %c"),pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.PanelName[i]));
	}
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.slid = p[9];
	for (i = 0;i < 4;i++)
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.EK_Value[i] = p[10+i];
	}
	DBGD((TEXT("\r\nGolbal Panel Data Encode:")));
	DBGD((TEXT("\r\n")));
	for (i = 0;i < 4;i++)
	{
		DBGD((TEXT(" %x"),pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.EK_Value[i]));
	}
	for (i = 0;i < 30;i++)
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.KeyValue[i] = p[14+i];
	}
	DBGD((TEXT("\r\nGolbal Panel Data AD:")));
	DBGD((TEXT("\r\n")));
	for (i = 0;i < 8;i++)
	{
		DBGD((TEXT(" %x"),pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.KeyValue[i]));
	}
	DBGD((TEXT("\r\n")));
	for (i = 0;i < 8;i++)
	{
		//DBGD((TEXT(" %x"),pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.KeyValue[i+8]));
	}
	DBGD((TEXT("\r\n")));
	for (i = 0;i < 8;i++)
	{
		DBGD((TEXT(" %x"),pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.KeyValue[i+16]));
	}
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.LCD_RGB_Wide = p[44] + (p[45] << 8);
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.LCD_RGB_High = p[46] + (p[47] << 8);
}

UINT32 forU8ToU32(P_FLY_GLOBAL_INFO pGlobalInfo,BYTE *p)
{
	UINT32 iTemp = 0;
	iTemp = (p[3] << 24) + (p[2] << 16) + (p[1] << 8) + p[0];
	return iTemp;
}

/*
8	方向盘头（WHEEL   ）

4	结构体总大小（包括头、尾、数据）
4	方向盘数量
4	方向盘名称长度

N	方向盘名称（Unicode编码、逗号隔开 2C 00）

4	方向盘ID
4	方向盘数据偏移量（从头开始）
4	方向盘数据大小
。。。重复上面三行

N	方向盘数据

8	方向盘尾（WHEEL   ）
*/
//bDefault读取缺省方向盘数据？
//iSteelwheelIndex	0为缺省	1~Count为选择
void readSteelwheelData(P_FLY_GLOBAL_INFO pGlobalInfo,BYTE *p,UINT length,UINT iSteelwheelIndex,BOOL bDefault)
{
	UINT iStructSize;
	UINT iRemoteDataCount;
	UINT iRemoteNameLength;
	UINT iNowRemoteNameIndex;
	UINT iRemoteDataOneOffset;
	UINT iRemoteDataOneSize;
	UINT i;
	if ('W' == p[0] && 'H' == p[1] && 'E' == p[2] && 'E' == p[3]
	 && 'L' == p[4] && ' ' == p[5] && ' ' == p[6] && ' ' == p[7]
			&& 'W' == p[length-8] && 'H' == p[length-7] && 'E' == p[length-6] && 'E' == p[length-5]
			&& 'L' == p[length-4] && ' ' == p[length-3] && ' ' == p[length-2] && ' ' == p[length-1])
	{
		iStructSize = forU8ToU32(pGlobalInfo,&p[8]);
		if (iStructSize == length)
		{
			iRemoteDataCount = forU8ToU32(pGlobalInfo,&p[8+4]);
			if (!bDefault)//非缺省
			{
				pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataCount = iRemoteDataCount;
				DBGD((TEXT("\r\nGolbal Remote Count: %d Index: %d"),iRemoteDataCount,iSteelwheelIndex));
				//if (0 == iSteelwheelIndex)
				//{
				//	RETAILMSG(1, (TEXT("\r\nGolbal Remote Read Return For Read Not Default But Index 0")));
				//	return;
				//}
				//else
				//{
				//	iSteelwheelIndex--;
				//}
			}
			else
			{
				DBGD((TEXT("\r\nGolbal Remote Count: %d Index: %d"),iRemoteDataCount,iSteelwheelIndex));
				//if (iSteelwheelIndex)
				//{
				//	RETAILMSG(1, (TEXT("\r\nGolbal Remote Read Return For Read Default But Index Not 0")));
				//	return;
				//}
				return;
			}
			if (iSteelwheelIndex < iRemoteDataCount)
			{
				iRemoteNameLength = forU8ToU32(pGlobalInfo,&p[8+4+4]);
				iNowRemoteNameIndex = 0;
				pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataNameLength = 0;//清0
				for (i = 0;i < iRemoteNameLength;i+=2)
				{
					if (0x2C == p[8+4+4+4+i] && 0x00 == p[8+4+4+4+i+1])
					{
						iNowRemoteNameIndex++;
					}
					if (iNowRemoteNameIndex < iSteelwheelIndex)//小于则等待
					{
					}
					else if (iNowRemoteNameIndex == iSteelwheelIndex)//等于则读取
					{
						pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sRemoteDataName[pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataNameLength++] = p[8+4+4+4+i];
						pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sRemoteDataName[pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataNameLength++] = p[8+4+4+4+i+1];
					}
					else//大于则跳出
					{
						break;
					}
				}
				////补0
				//pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sRemoteDataName[pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataNameLength++] = 0x00;
				//pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sRemoteDataName[pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataNameLength++] = 0x00;
				DBGD((TEXT("\r\nGolbal Remote Name%d:"),pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataNameLength));
				for (i = 0;i < pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataNameLength;i++)
				{
					DBGD((TEXT(" %c"),pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sRemoteDataName[i]));
				}

				iRemoteDataOneOffset = forU8ToU32(pGlobalInfo,&p[8+4+4+4+iRemoteNameLength+(12*iSteelwheelIndex)+4]);				
				iRemoteDataOneSize = forU8ToU32(pGlobalInfo,&p[8+4+4+4+iRemoteNameLength+(12*iSteelwheelIndex)+4+4]);
				memcpy((void*)&pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sRemoteData
					,&p[iRemoteDataOneOffset]
					,iRemoteDataOneSize);
				pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRemoteDataHave = TRUE;
				DBGD((TEXT("\r\nGolbal Remote Data:")));
				for (i = 0;i < iRemoteDataOneSize;i++)
				{
					DBGD((TEXT(" %x"),p[iRemoteDataOneOffset+i]));
				}
			}
			else
			{
				pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataNameLength = 0;
				pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRemoteDataHave = FALSE;
				RETAILMSG(1, (TEXT("\r\nGolbal Remote Data Count Less For Req Error")));
			}
		}
		else
		{
			pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataNameLength = 0;
			pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRemoteDataHave = FALSE;
			RETAILMSG(1, (TEXT("\r\nGolbal Remote Data Struct Size Error")));
		}
	}
	else
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataNameLength = 0;
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRemoteDataHave = FALSE;
		RETAILMSG(1, (TEXT("\r\nGolbal Remote Data Header Or Tail Error")));
	}
}

static void readMCUSoftVersion(P_FLY_GLOBAL_INFO pGlobalInfo,BYTE *p,UINT length)
{
	UINT iMCUSoftwareVersionLength;
	if ('V' == p[0] && 'E' == p[1] && 'R' == p[2] && 'S' == p[3]
	&& 'I' == p[4] && 'O' == p[5] && 'N' == p[6] && ' ' == p[7]
	&& 'V' == p[length-8] && 'E' == p[length-7] && 'R' == p[length-6] && 'S' == p[length-5]
	&& 'I' == p[length-4] && 'O' == p[length-3] && 'N' == p[length-2] && ' ' == p[length-1])
	{
		RETAILMSG(1, (TEXT("\r\nGlobal Read MCU Software Version Start")));
		iMCUSoftwareVersionLength = forU8ToU32(pGlobalInfo,&p[8]) - 8 - 4 - 8;
		if (iMCUSoftwareVersionLength < MCU_SOFT_VERSION_MAX)
		{
			RETAILMSG(1, (TEXT("\r\nGlobal Read MCU Software Version Length OK")));
			memcpy((void*)pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion,&p[8+4],iMCUSoftwareVersionLength);
			pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersionLength = iMCUSoftwareVersionLength;

			RegDataWriteSoftWareVersion(pGlobalInfo);

			RegDataReadSoftWareVersion(pGlobalInfo);

			ConfigTheVolumePara(pGlobalInfo);

			IpcStartEvent(EVENT_GLOBAL_GOT_CARNAME_ID);//

			IpcStartEvent(EVENT_GLOBAL__CARAMP_CHANGE_ID);//
			
		}
	}
	else
	{
		RETAILMSG(1, (TEXT("\r\nGlobal Read MCU Software Version Error")));
	}
}

#define READ_PANELSTEER_FROM_FILE	1

#define PANEL	1
#define SKIN	2

#define TYPE	1

//获取NANDFLASH数据
static void ReadPanelSteeringData(P_FLY_GLOBAL_INFO pGlobalInfo,UINT iSteelwheelIndex) 
{
	UINT iTotalTypeCount;
	UINT iTypeID,iTypeStart,iTypeCount;
	UINT i;

	UINT length = 0;

	if (NULL == pGlobalInfo->pReadPackageData)
	{
		//Sleep(3*1000);

#if READ_PANELSTEER_FROM_FILE
		DBGI((TEXT("\r\nGlobal open PanelSteering.dat Start")));
		
		FILE *pFile = NULL;

		
		if (GetFileAttributes(L"\\Windows\\PanelSteering.dat") != -1)
		{
			pFile = fopen("\\Windows\\PanelSteering.dat","rb");
			DBGI((TEXT("\r\nGlobal open \\Windows\\PanelSteering.dat\r\n")));
		}
		else if (GetFileAttributes(L"\\APP Disk\\PanelSteering.dat") != -1)
		{
			pFile = fopen("\\APP Disk\\PanelSteering.dat","rb");
			DBGI((TEXT("\r\nGlobal open \\APP Disk\\PanelSteering.dat\r\n")));
		}
		else
		{
			DBGI((TEXT("\r\nGlobal PanelSteering.dat not find\r\n")));
			goto err_return;
		}
		
		
		if (pFile)
		{
			fseek(pFile,0,SEEK_END);
			length = ftell(pFile);
			fseek(pFile,0,SEEK_SET);
			pGlobalInfo->pReadPackageData = new BYTE[length+8];
			fread(pGlobalInfo->pReadPackageData,1,length,pFile);
			fclose(pFile);
		}
		else
		{
			DBGI((TEXT("\r\nGlobal Can not open PanelSteer.dat")));
			goto err_return;
		}
		//}
		//_except(GetExceptionCode())// == STATUS_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		//{
		//	DBGI((TEXT("\r\nGlobal Read PanelSteering catch fail")));
		//	return;
		//}
		if (pGlobalInfo->pReadPackageData == NULL)
		{
			goto err_return;
		}

#else

#if TYPE==PANEL

		unsigned int TYPELEN = GET_PANELSTEER_CODE_LEN;		//	GET_S_W_LEN
		unsigned int TYPEREAD = READ_PANELSTEER_CODE_BUF;	//	READ_S_W_BUF
#else if TYPE==SKIN
		//由于皮肤是多个的，所以在这边要进行皮肤选择
		unsigned int TYPELEN = NDF_CODE(SKIN_CODE,0x01,GETLEN);	
		unsigned int TYPEREAD = NDF_CODE(SKIN_CODE,0x01,READBUF);
#endif

		NDFlash pNDF;
		if(!pNDF.OpenFlashFile())	//打开FLASH
			goto err_return;
		length = pNDF.GetDatalength(TYPELEN);	//获取该类型数据的长度
		if(length == 0)
			goto err_return;

		pGlobalInfo->pReadPackageData = new BYTE[length+8];
		if (!pNDF.ReadData(TYPEREAD,pGlobalInfo->pReadPackageData,length))	//读取该类型数据
		{
			RETAILMSG(1, (TEXT("Read Fail")));
		}
		pNDF.CloseFlashFile();		//关闭FLASH
		if (0 == length)
		{
			DBGI((TEXT("\r\nGlobal Can not Read XingXing")));
		}
#endif
	}

	//处理开始
	DBGD((TEXT("\r\nGolbal Panel And Default Steelwheel Data:")));
	for (i = 0;i < length;i++)
	{
		DBGD((TEXT(" %02X"),pGlobalInfo->pReadPackageData[i]));
	}
	//iTotalTypeCount = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[0]);
	//if (3 == iTotalTypeCount)
	//{
	//	for (i = 0;i < 3;i++)
	//	{
	//		iTypeID = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[4+4*3*i]);
	//		iTypeStart = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[4+4*3*i+4]);
	//		iTypeCount = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[4+4*3*i+8]);
	//		if (1 == iTypeID)
	//		{
	//			readMCUSoftVersion(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount);
	//		}
	//		else if (2 == iTypeID)
	//		{
	//			readPanelData(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount);
	//		}
	//		else if (3 == iTypeID)
	//		{
	//			readSteelwheelData(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount,iSteelwheelIndex,FALSE);
	//		}
	//	}
	//}
	//else if(4 == iTotalTypeCount)
	//{
	//	for (i = 0;i < 4;i++)
	//	{
	//		iTypeID = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[4+4*3*i]);
	//		iTypeStart = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[4+4*3*i+4]);
	//		iTypeCount = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[4+4*3*i+8]);
	//		if (1 == iTypeID)
	//		{
	//			readMCUSoftVersion(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount);
	//		}
	//		else if (2 == iTypeID)
	//		{
	//			readPanelData(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount);
	//		}
	//		else if (3 == iTypeID)
	//		{
	//			readSteelwheelData(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount,iSteelwheelIndex,FALSE);
	//		}
	//		else if(4 == iTypeID)
	//		{
	//			readRadioAntData(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount);
	//		}
	//	}
	//}
	//else
	//{
	//	RETAILMSG(1, (TEXT("\r\nGolbal Flash Read Type Are Not 3 %s"),__TIME__));
	//}
	//处理结束


	iTotalTypeCount = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[0]);
	if (3 == iTotalTypeCount)
	{
		for (i = 0;i < 3;i++)
		{
			iTypeID = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[4+4*3*i]);
			iTypeStart = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[4+4*3*i+4]);
			iTypeCount = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[4+4*3*i+8]);
			if (1 == iTypeID)
			{
				readMCUSoftVersion(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount);
			}
			else if (2 == iTypeID)
			{
				readPanelData(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount);
			}
			else if (3 == iTypeID)
			{
				readSteelwheelData(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount,iSteelwheelIndex,FALSE);
			}
		}
	}
	else if(4 == iTotalTypeCount)
	{
		for (i = 0;i < 4;i++)
		{
			iTypeID = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[4+4*3*i]);
			iTypeStart = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[4+4*3*i+4]);
			iTypeCount = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[4+4*3*i+8]);
			if (1 == iTypeID)
			{
				readMCUSoftVersion(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount);
			}
			else if (2 == iTypeID)
			{
				readPanelData(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount);
			}
			else if (3 == iTypeID)
			{
				readSteelwheelData(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount,iSteelwheelIndex,FALSE);
			}
			else if(4 == iTypeID)
			{
				readRadioAntData(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount);
			}
		}
	}
	else if(5 == iTotalTypeCount)
	{
		for (i = 0;i < 5;i++)
		{
			iTypeID = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[4+4*3*i]);
			iTypeStart = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[4+4*3*i+4]);
			iTypeCount = forU8ToU32(pGlobalInfo,&pGlobalInfo->pReadPackageData[4+4*3*i+8]);
			if (1 == iTypeID)
			{
				readMCUSoftVersion(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount);
			}
			else if (2 == iTypeID)
			{
				readPanelData(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount);
			}
			else if (3 == iTypeID)
			{
				readSteelwheelData(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount,iSteelwheelIndex,FALSE);
			}
			else if(4 == iTypeID)
			{
				readRadioAntData(pGlobalInfo,&pGlobalInfo->pReadPackageData[iTypeStart],iTypeCount);
			}
			else if(5 == iTypeID)
			{

			}
		}
	}
	else
	{
		RETAILMSG(1, (TEXT("\r\nGolbal Flash Read Type Are Not 3 %s"),__TIME__));
	}

err_return:
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bVolumeMaxReady = TRUE;
	return;
}

static DWORD WINAPI ThreadPowerUp(LPVOID pContext)
{
	P_FLY_GLOBAL_INFO	pGlobalInfo = (P_FLY_GLOBAL_INFO)pContext;

	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLEDTestFlashCount = 0;
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLEDTestFlashCountSub = 0;

	return 0;
}


static void powerOnNormalInit(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	pGlobalInfo->buffToUserHx = 0;
	pGlobalInfo->buffToUserLx = 0;

	memset(pGlobalInfo->iDemoStrLength,0,OSD_DEBUG_LINES);
}

static void powerOnFirstInit(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	pGlobalInfo->bOpen = FALSE;
	pGlobalInfo->bPower = FALSE;
	pGlobalInfo->bPowerUp = FALSE;
	pGlobalInfo->bSpecialPower = FALSE;

	pGlobalInfo->bMemoryMapHave = FALSE;

	pGlobalInfo->pReadPackageData = NULL;
}

static void powerOnSpecialEnable(P_FLY_GLOBAL_INFO pGlobalInfo,BOOL bOn)
{
	DWORD dwThreadID;

	if (bOn)
	{
		if (pGlobalInfo->bSpecialPower)
		{
			return;
		}
		pGlobalInfo->bSpecialPower = TRUE;

		pGlobalInfo->hBuffToUserDataEvent = CreateEvent(NULL,FALSE,FALSE,NULL);	
		InitializeCriticalSection(&pGlobalInfo->hCSSendToUser);

		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
		//pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume = 30;
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMin = 0;
		//pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax = 60;
		ConfigTheVolumePara(pGlobalInfo);
		
		//pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalDataChangeWhatOn = TRUE;

		pGlobalInfo->hOnReadMemoryEvent  = CreateEvent(NULL,FALSE,FALSE,TEXT("FLY_SHARE_MEMORY_USER_WRITE_DATA_HANDLE_NAME"));
		pGlobalInfo->hOnWriteMemoryEvent = CreateEvent(NULL,FALSE,FALSE,TEXT("FLY_SHARE_MEMORY_DRIVER_WRITE_DATA_HANDLE_NAME"));

		MSGQUEUEOPTIONS  msgOpts;

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_GLOBAL_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_GLOBAL_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pGlobalInfo->hMsgQueueToGlobalCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_GLOBAL_NAME, &msgOpts);
		if (NULL == pGlobalInfo->hMsgQueueToGlobalCreate)
		{
			DBGI((TEXT("\r\nGlobal Create MsgQueue To Global Fail!")));
		}

		pGlobalInfo->bKillDispatchFlyMsgQueueReadThread = FALSE;
		pGlobalInfo->LEDTestMsgQueueReadThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadRead, //线程的全局函数
			pGlobalInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGI((TEXT("\r\nGlobal ThreadRead ID:%x"),dwThreadID));
		if (NULL == pGlobalInfo->LEDTestMsgQueueReadThreadHandle)
		{
			DBGI((TEXT("\r\nFlyAudio Global Create MsgQueue To Global Fail!")));
			pGlobalInfo->bKillDispatchFlyMsgQueueReadThread = TRUE;
			return;
		}

		GlobalEnable(pGlobalInfo,TRUE);
	} 
	else
	{
		if (!pGlobalInfo->bSpecialPower)
		{
			return;
		}
		pGlobalInfo->bSpecialPower = FALSE;

		pGlobalInfo->bKillDispatchFlyMsgQueueReadThread = TRUE;
		while (pGlobalInfo->LEDTestMsgQueueReadThreadHandle)
		{
			SetEvent(pGlobalInfo->hMsgQueueToGlobalCreate);
			Sleep(10);
		}
		CloseMsgQueue(pGlobalInfo->hMsgQueueToGlobalCreate);

		DestoryWriteMemory(pGlobalInfo);
		GlobalEnable(pGlobalInfo,FALSE);

		CloseHandle(pGlobalInfo->hOnWriteMemoryEvent);
		CloseHandle(pGlobalInfo->hOnReadMemoryEvent);

		CloseHandle(pGlobalInfo->hBuffToUserDataEvent);
	}
}


//方向盘数据还没有读 --- 等修改
static DWORD WINAPI ThreadSpecialRead(LPVOID pContext)
{
	P_FLY_GLOBAL_INFO	pGlobalInfo = (P_FLY_GLOBAL_INFO)pContext;

	debugParaReadFromFile(pGlobalInfo,0);

	return 0;

	//ReadPanelSteeringData(pGlobalInfo,0);//读面板和方向盘数据//此处会导致开机的时候概率死机

	return 0;
}

#define	SUWEI_DEBUG_ON_REG	(*(volatile unsigned int*)(0x8a050018)) 
#define	SUWEI_DEBUG_ON		0xFF
#define	SUWEI_DEBUG_OFF		0x0

void IpcRecv(UINT32 iEvent)
{
	P_FLY_GLOBAL_INFO	pGlobalInfo = (P_FLY_GLOBAL_INFO)gpGlobalInfo;

	DBGD((TEXT("\r\nFLY GLOBAL Recv IPC iEvent:%d\r\n"),iEvent));

	SetEvent(pGlobalInfo->hDispatchMainThreadEvent);
}
//void SockRecv(BYTE *buf, UINT16 len)
//{
//	P_FLY_GLOBAL_INFO	pGlobalInfo = (P_FLY_GLOBAL_INFO)gpGlobalInfo;
//}

HANDLE
FGB_Init(DWORD dwContext)
{
	//SUWEI_DEBUG_ON_REG = SUWEI_DEBUG_ON;//打开串口调试

	DWORD dwThreadID;
	P_FLY_GLOBAL_INFO pGlobalInfo;

	RETAILMSG(1, (TEXT("\r\nGlobal Init Start")));

	pGlobalInfo = (P_FLY_GLOBAL_INFO)LocalAlloc(LPTR, sizeof(FLY_GLOBAL_INFO));
	if (!pGlobalInfo)
	{
		return NULL;
	}
	gpGlobalInfo = pGlobalInfo;
	
	pGlobalInfo->pFlyShmGlobalInfo = CreateShm(GLOBAL_MODULE,IpcRecv);
	if (NULL == pGlobalInfo->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio global Init create shm err\r\n")));
		return NULL;
	}

	if (!GetDllAddrTable())
	{
		DBGE((TEXT("FlyAudio global  GetDllAddrTable err\r\n")));
		return NULL;
	}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio global Open create user buf err\r\n")));
		return NULL;
	}

	//if(!CreateServerSocket(SockRecv, TCP_PORT_GLOBAL))
	//{
	//	DBGE((TEXT("FlyAudio global Open create server socket err\r\n")));
	//	return NULL;
	//}

	//内存清0
	memset((void*)pGlobalInfo->pFlyShmGlobalInfo,0, sizeof(struct shm));
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalStructSize = sizeof(struct shm);
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.PCB_Version = PCB_NOW_USE;


	//把是否要打印的标志位读出来
	UINT16 iTemp=0;
	ReadRegValues(HKEY_LOCAL_MACHINE,L"Drivers\\BuiltIn\\FLYKEY",L"bDebugMsgEnable",
		(LPBYTE)&iTemp,sizeof(int));
	if (iTemp == 1)
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable = TRUE;
	else
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable = FALSE;

	//pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable = TRUE;


	//pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLEDTestFlashCount = 3;

	//参数初始化
	powerOnNormalInit(pGlobalInfo);
	powerOnFirstInit(pGlobalInfo);
	powerOnSpecialEnable(pGlobalInfo,TRUE);


	memset(pGlobalInfo->iGlobalMemoryEventWhat,0,GLOBAL_MEMORY_EVENT_SAVE);
	ShareMemoryWriteInit(pGlobalInfo);
	OnCreateWriteMemory(pGlobalInfo);

	CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
		0,//初始化线程栈的大小，缺省为与主线程大小相同
		(LPTHREAD_START_ROUTINE)ThreadSpecialRead, //线程的全局函数
		pGlobalInfo, //此处传入了主框架的句柄
		0, &dwThreadID );
	DBGI((TEXT("\r\nGlobal ThreadSpecialRead ID:%x"),dwThreadID));



	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompYear = year;
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompMon = months;
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompDay = day;
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompHour = hours;
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompMin = minutes;
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompSec = seconds;

	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.bInit = TRUE;


	RETAILMSG(1, (TEXT("\r\nGlobal Init Build:Date%d.%d.%dTime%d:%d:%d debugMsgLevel:%d GlobalMemorySize:%d")
		,year,months,day,hours,minutes,seconds
		,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.debugMsgLevel
		,sizeof(struct shm)));
	return (HANDLE)pGlobalInfo;
}

BOOL
FGB_Deinit(DWORD hDeviceContext)
{
	P_FLY_GLOBAL_INFO	pGlobalInfo = (P_FLY_GLOBAL_INFO)hDeviceContext;

	powerOnSpecialEnable(pGlobalInfo,FALSE);

	RETAILMSG(1, (TEXT("FlyAudio Golbal DeInit\r\n")));
	FreeShm();
	//FreeSocketServer();
	FreeUserBuff();
	LocalFree(pGlobalInfo);
	return TRUE;
}

DWORD
FGB_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	P_FLY_GLOBAL_INFO	pGlobalInfo = (P_FLY_GLOBAL_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;

	if (pGlobalInfo->bOpen)
	{
		return NULL;
	}
	pGlobalInfo->bOpen = TRUE;

	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.bOpen = TRUE;

	RETAILMSG(1, (TEXT("FlyAudio Global Open\r\n")));
	return returnWhat;
}

BOOL
FGB_Close(DWORD hDeviceContext)
{
	P_FLY_GLOBAL_INFO	pGlobalInfo = (P_FLY_GLOBAL_INFO)hDeviceContext;

	pGlobalInfo->bOpen = FALSE;

	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.bOpen = FALSE;

	RETAILMSG(1, (TEXT("FlyAudio Global Close\r\n")));
	return TRUE;
}

VOID
FGB_PowerUp(DWORD hDeviceContext)
{
	P_FLY_GLOBAL_INFO	pGlobalInfo = (P_FLY_GLOBAL_INFO)hDeviceContext;
	DWORD dwThreadID;

	CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
		0,//初始化线程栈的大小，缺省为与主线程大小相同
		(LPTHREAD_START_ROUTINE)ThreadPowerUp, //线程的全局函数
		pGlobalInfo, //此处传入了主框架的句柄
		0, &dwThreadID );
	DBGI((TEXT("\r\nGlobal ThreadPowerUp ID:%x"),dwThreadID));

	RETAILMSG(1, (TEXT("FlyAudio Global PowerUp\r\n")));
}

VOID
FGB_PowerDown(DWORD hDeviceContext)
{
	P_FLY_GLOBAL_INFO	pGlobalInfo = (P_FLY_GLOBAL_INFO)hDeviceContext;

	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnAudio = 0;
	pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnRadio = 0;

	RETAILMSG(1, (TEXT("FlyAudio Global PowerDown\r\n")));
}

DWORD
FGB_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	P_FLY_GLOBAL_INFO	pGlobalInfo = (P_FLY_GLOBAL_INFO)hOpenContext;

	UINT32 dwRead = 0,i;
	BYTE *buf = (BYTE *)pBuffer;

	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio global return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}

	return dwRead;
}

DWORD
FGB_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	P_FLY_GLOBAL_INFO pGlobalInfo = (P_FLY_GLOBAL_INFO)hOpenContext;
	DBGD((TEXT("\r\nGlobal Write %d Bytes:"),NumberOfBytes));
	for(UINT i = 0;i < NumberOfBytes;i++)
	{
		DBGD((TEXT(" %X"),*((BYTE *)pSourceBytes + i)));
	}

	if(NumberOfBytes)
	{
		WinCECommandProcessor(pGlobalInfo,(((BYTE *)pSourceBytes)+3),((BYTE *)pSourceBytes)[2]);
	}
	return NULL;
}

DWORD
FGB_Seek(DWORD hOpenContext, LONG Amount, DWORD  Type)
{
	return NULL;
}

BOOL
FGB_IOControl(DWORD hOpenContext,
			  DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn,
			  PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
	P_FLY_GLOBAL_INFO	pGlobalInfo = (P_FLY_GLOBAL_INFO)hOpenContext;
	BOOL bRes = TRUE;

	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	RETAILMSG(1, (TEXT("FlyAudio Global IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)\r\n"),
	//				hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	RETAILMSG(1, (TEXT("\r\nGlobal IOControl Set IOCTL_SERIAL_SET_WAIT_MASK")));
	//	break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	//RETAILMSG(1, (TEXT("\r\nGlobal IOControl Set IOCTL_SERIAL_WAIT_ON_MASK")));
	//	if (pGlobalInfo->bOpen)
	//	{
	//		WaitForSingleObject(pGlobalInfo->hBuffToUserDataEvent,INFINITE);
	//	}
	//	else
	//	{
	//		WaitForSingleObject(pGlobalInfo->hBuffToUserDataEvent,0);
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
		DEBUGMSG(1, (TEXT("Attach in FlyKey DllEntry\n")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}

	if (dwReason == DLL_PROCESS_DETACH)
	{
		DEBUGMSG(1, (TEXT("Dettach in FlyKey DllEntry\n")));
	}

	DEBUGMSG(1, (TEXT("Leaving FlyKey DllEntry\n")));

	return (TRUE);
}
