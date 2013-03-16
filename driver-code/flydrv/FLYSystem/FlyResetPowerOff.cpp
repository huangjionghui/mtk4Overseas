// -----------------------------------------------------------------------------
// File Name    : FlyResetPowerOff.cpp
// Title        : FLYSystem Driver
// Author       : JQilin - Copyright (C) 2011
// Created      : 2011-08-02  
// Version      : 0.01
// Target MCU   : WinCE Driver
// -----------------------------------------------------------------------------
// Version History:
/*
>>>2011-08-02: V0.01 first draft
*/

#include "FLYSystem.h"

//���������Է���Ϊ��λ

#define SYSTEM_RESET_USE_EXT_CONFIG	(pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSystemResetUseExtConfig)

#define SYSTEM_RESET_AT_LEAST_DAYS	(pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSystemResetAtLeastDays)	
#define SYSTEM_RESET_ON_HOUR		(pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSystemResetOnHour)	
#define SYSTEM_RESET_ON_MINUTE		(pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSystemResetOnMinute)	

#define SYSTEM_RESET_INNER_MIN		(pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSystemResetInnerMin)	

#define SYSTEM_RESET_POWEROFF_MIN	(pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSystemResetPowerOffMin)

#define SYSTEM_CAN_RUN_AT_LEAST			(pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSystemResetCanRunLess)

DWORD returnTransToKylinTime(SYSTEMTIME *psTime)//��λ������
{
	FILETIME fTime;
	SystemTimeToFileTime(psTime,&fTime);
	return (DWORD)((*(ULONGLONG *)&fTime)/10000/1000/60);	
}

DWORD returnMinutesToNextByDay(DWORD now,DWORD next)
{
	if (next > now)
	{
		return next - now;
	}
	else
	{
		return 24*60 - now + next;
	}
}

void printfResetTimeToFlash(SYSTEMTIME *timeNow,SYSTEMTIME *timeFirstPowerOn,SYSTEMTIME *timeLastUserAccOff)
{
	char fileName[64];
	memset(fileName,0,64);
	strcpy(fileName,"\\SDMEM\\DriverSleepDebug.txt");

	FILE *pFile = NULL;
	pFile = fopen(fileName,"ab");
	if (NULL == pFile)
	{
		pFile = fopen(fileName,"wb");
	}
	if (pFile)
	{
		fseek(pFile,0,SEEK_END);
		fprintf(pFile,"\n\ntimeNow is: %d %d %d %d %d %d %d"
			,timeNow->wYear
			,timeNow->wMonth
			,timeNow->wDay
			,timeNow->wHour
			,timeNow->wMinute
			,timeNow->wSecond
			,timeNow->wMilliseconds);
		fprintf(pFile,"\ntimeFirstPowerOn is: %d %d %d %d %d %d %d"
			,timeFirstPowerOn->wYear
			,timeFirstPowerOn->wMonth
			,timeFirstPowerOn->wDay
			,timeFirstPowerOn->wHour
			,timeFirstPowerOn->wMinute
			,timeFirstPowerOn->wSecond
			,timeFirstPowerOn->wMilliseconds);
		fprintf(pFile,"\ntimeLastUserAccOff is: %d %d %d %d %d %d %d"
			,timeLastUserAccOff->wYear
			,timeLastUserAccOff->wMonth
			,timeLastUserAccOff->wDay
			,timeLastUserAccOff->wHour
			,timeLastUserAccOff->wMinute
			,timeLastUserAccOff->wSecond
			,timeLastUserAccOff->wMilliseconds);
		fclose(pFile);
	}
}

void resetPrintDebugMsg(P_FLY_SYSTEM_INFO pSystemInfo)
{
	SYSTEMTIME timeNow;
	GetLocalTime(&timeNow);//����ʱ��

	//printfResetTimeToFlash(&timeNow,&pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn,&pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff);

	RETAILMSG(1, (TEXT("\r\nSystem RP Now Time:Y%d M%d D%d W%d H%d M%d S%d MS%d Kylin%d")
		,timeNow.wYear
		,timeNow.wMonth
		,timeNow.wDay
		,timeNow.wDayOfWeek
		,timeNow.wHour
		,timeNow.wMinute
		,timeNow.wSecond
		,timeNow.wMilliseconds
		,returnTransToKylinTime(&timeNow)));

	RETAILMSG(1, (TEXT("\r\nSystem RP First PowerOn Time:Y%d M%d D%d W%d H%d M%d S%d MS%d Kylin%d")
		,pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn.wYear
		,pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn.wMonth
		,pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn.wDay
		,pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn.wDayOfWeek
		,pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn.wHour
		,pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn.wMinute
		,pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn.wSecond
		,pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn.wMilliseconds
		,returnTransToKylinTime(&pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn)));

	RETAILMSG(1, (TEXT("\r\nSystem RP Last UserACCOff Time:Y%d M%d D%d W%d H%d M%d S%d MS%d Kylin%d")
		,pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff.wYear
		,pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff.wMonth
		,pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff.wDay
		,pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff.wDayOfWeek
		,pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff.wHour
		,pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff.wMinute
		,pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff.wSecond
		,pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff.wMilliseconds
		,returnTransToKylinTime(&pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff)));

	RETAILMSG(1, (TEXT("\r\nSystem RP UserACCOff - PowerOn = %d"),returnTransToKylinTime(&pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff)-returnTransToKylinTime(&pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn)));
}

void resetRegDataRead(P_FLY_SYSTEM_INFO pSystemInfo)
{
	HKEY hKey; 
	DWORD dwDisposition;
//	DWORD dwTemp;
	DWORD dwLen;
	DWORD dwType;

#if REGEDIT_NOT_READ
	return;
#endif

	RETAILMSG(1, (TEXT("\r\nSystem RP reg data Read")));

	//��ȡResetPowerOff����
	RegCreateKeyEx(HKEY_CURRENT_USER,L"FlyDrivers\\ResetPowerOff",0,NULL,0,
		0,NULL,&hKey,&dwDisposition); /////////////////////////////////�������ע���

	if (NULL != RegQueryValueEx(hKey,L"timeLastUserAccOff",0,&dwType,(unsigned char *)&pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff,&dwLen))//�����߶�����
	{
		pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff
			= pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn;
	}
	if (pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff.wYear < 2011)//��ע����쳣
	{
		GetLocalTime(&pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn);//��һ���ϵ�
		pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff
			= pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn;
	}
	RegCloseKey(hKey);	
}

void resetRegDataWrite(P_FLY_SYSTEM_INFO pSystemInfo)
{
	HKEY hKey; 
	DWORD dwDisposition;
	//DWORD dwTemp;
	//DWORD dwLen;

#if REGEDIT_NOT_SAVE
	return;
#endif

	RETAILMSG(1, (TEXT("\r\nSystem RP reg data Write")));

	//дResetPowerOff����
	RegCreateKeyEx(HKEY_CURRENT_USER,L"FlyDrivers\\ResetPowerOff",0,NULL,0,
		0,NULL,&hKey,&dwDisposition); /////////////////////////////////�������ע���

	RegSetValueEx(hKey,L"timeLastUserAccOff",0,REG_BINARY,(unsigned char *)&pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff,sizeof(SYSTEMTIME));

	RegFlushKey(hKey);
	RegCloseKey(hKey);
}

void resetPowerOn(P_FLY_SYSTEM_INFO pSystemInfo)//����
{
	if (!SYSTEM_RESET_USE_EXT_CONFIG)
	{
		SYSTEM_RESET_AT_LEAST_DAYS = 7;	//���ٶ�����
		SYSTEM_RESET_ON_HOUR = 3;	//��ʲôСʱ
		SYSTEM_RESET_ON_MINUTE = 45;	//��ʲô��

		SYSTEM_RESET_INNER_MIN = 60;	//�ػ������ٶ��ٷ���

		SYSTEM_RESET_POWEROFF_MIN = (5*24*60);	//�೤ʱ����͹��ģ�����

		SYSTEM_CAN_RUN_AT_LEAST = (30*24*60);	//���������ж೤ʱ��
	}

	GetLocalTime(&pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn);//��һ���ϵ�

	resetRegDataRead(pSystemInfo);

	resetPrintDebugMsg(pSystemInfo);

	pSystemInfo->SilencePowerOffInfo.bReadRegData = TRUE;
}

void resetPowerOff(P_FLY_SYSTEM_INFO pSystemInfo)//�ػ�
{
	//��ʱ�ò���
}

void resetPowerUp(P_FLY_SYSTEM_INFO pSystemInfo)//ACC On
{
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSilencePowerUp = FALSE;

	resetPrintDebugMsg(pSystemInfo);
}

void resetPowerDown(P_FLY_SYSTEM_INFO pSystemInfo)//ACC Off
{
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSilencePowerUp = FALSE;

	if (!pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSilencePowerUp)
	{
		GetLocalTime(&pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff);//�û���ACC
	}

	resetRegDataWrite(pSystemInfo);

	resetPrintDebugMsg(pSystemInfo);
}

DWORD resetReturnNextResetTime(P_FLY_SYSTEM_INFO pSystemInfo)
{
	SYSTEMTIME timeNow;
	DWORD kylinTimeNow;

	DWORD timeTemp;

	DWORD kylinTimeToRestWant;

	DWORD timeTail;

	DWORD timeToReset = 0;

	resetPrintDebugMsg(pSystemInfo);

	GetLocalTime(&timeNow);//����ʱ��
	kylinTimeNow = returnTransToKylinTime(&timeNow);

	timeTemp = returnTransToKylinTime(&pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn);
	if (timeTemp > kylinTimeNow)//�쳣����δ������ģ�
	{
		GetLocalTime(&pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn);//����ʱ��
	}

	timeTemp = returnTransToKylinTime(&pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff);
	if (timeTemp > kylinTimeNow)//�쳣����δ������ģ�
	{
		GetLocalTime(&pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff);//����ʱ��
	}

	GetLocalTime(&timeNow);//���»�ȡ����ʱ��
	kylinTimeNow = returnTransToKylinTime(&timeNow);
	RETAILMSG(1, (TEXT("\r\nSystem RP KylinTime Now%d"),kylinTimeNow));


	timeTail = timeNow.wHour*60 + timeNow.wMinute;
	kylinTimeToRestWant = returnMinutesToNextByDay(timeTail,(SYSTEM_RESET_ON_HOUR*60+SYSTEM_RESET_ON_MINUTE));//24Сʱ�ڵ���һ��ʱ���ķ�����
	kylinTimeToRestWant += kylinTimeNow;
	RETAILMSG(1, (TEXT("\r\nSystem RP Time To Next KeyPoint%d"),kylinTimeToRestWant));

	timeTemp = returnTransToKylinTime(&pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn);
	if ((kylinTimeNow - timeTemp)/24/60 < SYSTEM_RESET_AT_LEAST_DAYS)
	{
		kylinTimeToRestWant += (SYSTEM_RESET_AT_LEAST_DAYS - ((kylinTimeNow - timeTemp)/24/60))*24*60;
	}
	RETAILMSG(1, (TEXT("\r\nSystem RP Time To Next Reset%d"),kylinTimeToRestWant));

	if (kylinTimeToRestWant - kylinTimeNow >= SYSTEM_RESET_POWEROFF_MIN)//������͹���ʱ��
	{
		timeToReset = SYSTEM_RESET_POWEROFF_MIN;
		RETAILMSG(1, (TEXT("\r\nSystem RP Time To Next PowerOff%d"),timeToReset));
	}
	else
	{
		timeToReset = kylinTimeToRestWant - kylinTimeNow;
	}

	if (kylinTimeNow - returnTransToKylinTime(&pSystemInfo->SilencePowerOffInfo.timeFirstPowerOn) >= SYSTEM_CAN_RUN_AT_LEAST)
	{
		timeToReset = SYSTEM_RESET_INNER_MIN;
		RETAILMSG(1, (TEXT("\r\nSystem RP Have Run More Than SYSTEM_CAN_RUN_LESS%d"),timeToReset));
	}
	if (timeToReset < SYSTEM_RESET_INNER_MIN)
	{
		timeToReset += 24*60;
		RETAILMSG(1, (TEXT("\r\nSystem RP Time Roll To Next Day%d"),timeToReset));
	}

	RETAILMSG(1, (TEXT("\r\n\r\nSystem RP Will PowerOn %d Minutes Later!!!!!!!!\r\n\r\n"),timeToReset));
	timeToReset *= 60;//ת������
	return timeToReset;
}

void resetPowerProcOnRecMCUWakeup(P_FLY_SYSTEM_INFO pSystemInfo)//����
{
	SYSTEMTIME timeNow;
	DWORD kylinTimeNow;

	DWORD kylinUserAccOff;

	RETAILMSG(1, (TEXT("\r\nSystem RP Rec MCU WatchDog Wakeup")));
	resetPrintDebugMsg(pSystemInfo);

	GetLocalTime(&timeNow);//����ʱ��
	kylinTimeNow = returnTransToKylinTime(&timeNow);

	kylinUserAccOff = returnTransToKylinTime(&pSystemInfo->SilencePowerOffInfo.timeLastUserAccOff);

	if (kylinTimeNow - kylinUserAccOff >= SYSTEM_RESET_POWEROFF_MIN)
	{
		RETAILMSG(1, (TEXT("\r\nSystem RP Control To VeryLow Power")));
		controlToMCUPowerToVeryLowOff(pSystemInfo);
	}
}
