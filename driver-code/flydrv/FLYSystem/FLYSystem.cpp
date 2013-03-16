// -----------------------------------------------------------------------------
// File Name    : FLYSystem.cpp
// Title        : FLYSystem Driver
// Author       : JQilin - Copyright (C) 2011
// Created      : 2011-02-25  
// Version      : 0.01
// Target MCU   : WinCE Driver
// -----------------------------------------------------------------------------
// Version History:
/*
>>>2011-05-19: V0.02 整理了WinCE低功耗相关的代码
>>>2011-02-25: V0.01 first draft
*/

#define GLOBAL_COMM	1

#include "flyhwlib.h"
#include "flyuserlib.h"
#include "flysocketlib.h"

#include "FLYSystem.h"

#include "dual_hal.h"
#include "dual_callback.h"
#include "dual_task.h"

#include <windows.h>
#include <nkintr.h>

#define MODULE_BCAR  (0x1)

P_FLY_SYSTEM_INFO gpSystemInfo = NULL;

void printfMCUStringToFlash(BYTE *pData,UINT Length)
{
	SYSTEMTIME timeNow;
	GetLocalTime(&timeNow);//现在时间
	char fileName[64];
	memset(fileName,0,64);
	strcpy(fileName,"\\SDMEM\\MCUDebug.txt");

	FILE *pFile = NULL;
	pFile = fopen(fileName,"ab");
	if (NULL == pFile)
	{
		pFile = fopen(fileName,"wb");
	}
	if (pFile)
	{
		fseek(pFile,0,SEEK_END);
		fprintf(pFile,"\nNow Time is: %d %d %d %d %d %d %d"
			,timeNow.wYear
			,timeNow.wMonth
			,timeNow.wDay
			,timeNow.wHour
			,timeNow.wMinute
			,timeNow.wSecond
			,timeNow.wMilliseconds);
		fprintf(pFile,"\nMCU Data:");
		for (UINT i = 0;i < Length;i++)
		{
			fprintf(pFile," %02X",pData[i]);
		}
		fclose(pFile);
	}
}

//温敏电阻转换
const UINT RTCalculationTable[]=
{
	1656084,1195595,871853,641890,476929,
	357480,270209,205899,158117,122334,95334,
	74812,59105,47000,37611,30283,24528,19982,
	16370,13485,11168,9297,7780,6542,5529,4694,
	4005,3432,2955,2555,2219,1935,1695,1490
};
const INT RTTemperature[]=
{
	-40,-35,-30,-25,-20,-15,-10,-5,0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125
};
INT RTExchangeToTemperature(UINT iAD)
{
	UINT i;
	UINT iRT;
	iRT = (10000*iAD)/(4096-iAD);//上拉10K，下拉温敏电阻，10位AD

	if (iRT > RTCalculationTable[0])
	{
		return RTTemperature[0];
	}

	for (i = 0;i < sizeof(RTCalculationTable) - 1;i++)
	{
		if (iRT <= RTCalculationTable[i] && iRT >= RTCalculationTable[i+1])
		{
			return RTTemperature[i] + 
				(INT)((DOUBLE)(RTCalculationTable[i] - iRT)/(RTCalculationTable[i]-RTCalculationTable[i+1])*5);
		}
	}
	return RTTemperature[sizeof(RTTemperature-1)];
}

//const BYTE LCD_LED_NIGHT_PARA[4]={0,30,45,55};	//20100411_
//const BYTE LCD_LED_DAY_PARA[4]={0,60,71,100};	//20100411_passed
const BYTE LCD_LED_NIGHT_PARA[4]={0,30,45,65};	//20100411_
const BYTE LCD_LED_DAY_PARA[4]={0,65,75,100};	//20100411_passed


VOID FSY_PowerUp(DWORD hDeviceContext);
VOID FSY_PowerDown(DWORD hDeviceContext);

static void controlToMCULCDLightEn(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bOn);
void prePowerDown(P_FLY_SYSTEM_INFO pSystemInfo);

static void speedCheckInit(P_FLY_SYSTEM_INFO pSystemInfo)
{
	#if A4_PLAT
	#else
	HwGpioInput(SPEED_CHECK_I);
	#endif
}

//低电平有效
static void LCD_PanelPen_PWM(P_FLY_SYSTEM_INFO pSystemInfo,BYTE iDuty)
{
	HwPwmSetFreq(KEY_PWM, iDuty);
	DBGI((TEXT("\r\nFlyAudio System PanelPen PWM：%d\r\n"),iDuty));
}


static void LCD_LCDIdle_On(P_FLY_SYSTEM_INFO pSystemInfo)
{
	//高电平
	HwPwmSetFreq(LCD_IDLE, LCD_IDLE_ON);

	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMCUControlLCDLightOn = TRUE;
	controlToMCULCDLightEn(pSystemInfo,TRUE);
	DBGI((TEXT("\r\nFlyAudio System LCDIdle On")));
}

static void LCD_LCDIdle_Off(P_FLY_SYSTEM_INFO pSystemInfo)
{
	if (!pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bACCOffLightOn)
	{
		//低电平
		HwPwmSetFreq(LCD_IDLE, LCD_IDLE_OFF);
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMCUControlLCDLightOn = FALSE;
		controlToMCULCDLightEn(pSystemInfo,FALSE);
		DBGI((TEXT("\r\nFlyAudio System LCDIdle Off")));
	}
}

static void LCD_LCDBright_PWM(P_FLY_SYSTEM_INFO pSystemInfo,BYTE Duty)
{
	HwPwmSetFreq(LCD_PWM,Duty);
	DBGI((TEXT("\r\nFlyAudio System LCD PWM freq:%d"),Duty));
	
	////API
	//BYTE buf[2] = {0x10,0x00};
	//buf[1] = Duty;
	//SocketWrite(buf, 2);
}

void printfHowLongTime(P_FLY_SYSTEM_INFO pSystemInfo)
{
	return;
	ULONG timeNow;
	ULONG timeLast;
	timeNow = GetTickCount();
	timeLast = timeNow - pSystemInfo->iProcACCOffTime;
	pSystemInfo->iProcACCOffTime = timeNow;
	DBGI((TEXT("\r\nSystem Time Last %d"),timeLast));
}


static void returnSystemPowerMode(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bPower)
{
	BYTE buff[] = {0x01,0x00};
	if (bPower)
	{
		buff[1] = 0x01;
	}

	ReturnToUser(buff,2);
}
static void returnSystembInit(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bInit)
{
	BYTE buff[] = {0x02,0x00};
	if (bInit)
	{
		buff[1] = 0x01;
	}

	ReturnToUser(buff,2);
}

static void returnSystemBackDetectEnable(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bEnable)
{
	BYTE buff[] = {0x10,0x00};

	if (bEnable)
	{
		buff[1] = 0x01;
	} 

	ReturnToUser(buff,2);
}

static void returnLightDetectEnable(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bEnable)
{
	BYTE buff[] = {0x11,0x00};

	if (bEnable)
	{
		buff[1] = 0x01;
	} 

	ReturnToUser(buff,2);
}

static void returnSystemLCDBright(P_FLY_SYSTEM_INFO pSystemInfo,BYTE iBright)
{
	BYTE buff[] = {0x14,0x00};
	buff[1] = iBright;

	ReturnToUser(buff,2);
}

static void returnSystembDayNight(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bNight)
{
	BYTE buff[] = {0x12,0x00};
	if (bNight)
	{
		buff[1] = 0x01;
	}

	ReturnToUser(buff,2);

	if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDayNight)
	{
		returnSystemLCDBright(pSystemInfo,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightNight);
	}
	else
	{
		returnSystemLCDBright(pSystemInfo,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightDay);
	}
}

static void returnSystemGpsVolumeLevel(P_FLY_SYSTEM_INFO pSystemInfo,BYTE level)
{
	BYTE buff[] = {0x45,0x00};
	buff[1] = level;
	ReturnToUser(buff,2);
}

static void returnSystembHaveExtAmp(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bHave)
{
	BYTE buff[] = {0x13,0x00};
	if (bHave)
	{
		buff[1] = 0x01;
	}

	ReturnToUser(buff,2);
}

static void returnSystemPanelBright(P_FLY_SYSTEM_INFO pSystemInfo,BYTE iBright)
{
	BYTE buff[] = {0x15,0x00};
	buff[1] = iBright;

	ReturnToUser(buff,2);
}

static void returnSystemSteelwheelbOn(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bOn)
{
	BYTE buff[] = {0x16,0x00};
	if (bOn)
	{
		buff[1] = 0x01;
	}

	ReturnToUser(buff,2);
}

static void returnSystemRadioID(P_FLY_SYSTEM_INFO pSystemInfo,BYTE iID)
{
	BYTE buff[] = {0x17,0x00};
	buff[1] = iID;

	ReturnToUser(buff,2);
}

static void returnSystemDemoMode(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bOn)
{
	BYTE buff[] = {0x19,0x00};
	if (bOn)
	{
		buff[1] = 0x01;
	}

	ReturnToUser(buff,2);
}

static void returnSystemSteelwheelUseStudy(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bOn)
{
	BYTE buff[] = {0x1A,0x00};
	if (bOn)
	{
		buff[1] = 0x01;
	}

	ReturnToUser(buff,2);
}

static void returnSystemSteelStudyStatus(P_FLY_SYSTEM_INFO pSystemInfo,BYTE iStatus)
{
	BYTE buff[] = {0x1B,0x00};
	buff[1] = iStatus;

	ReturnToUser(buff,2);
}

static void returnDVDRegionCode(P_FLY_SYSTEM_INFO pSystemInfo,BYTE iRegionCode)
{
	BYTE buff[2];
	buff[0] = 0x1E;
	buff[1] = iRegionCode;
	ReturnToUser(buff,2);
}

static void returnSystemBreakStatus(P_FLY_SYSTEM_INFO pSystemInfo)
{
	BYTE buff[] = {0x20,0x00};

	if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBreakStatusIO
		|| pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBreakStatusBUS)
	{
		buff[1] = 0x01;
	}

	ReturnToUser(buff,2);
}

static void returnSystemBackStatus(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bBack,BOOL bVideo)
{
	BYTE buff[] = {0x21,0x00,0x00};

	if (bBack)
	{
		buff[1] = 0x01;
	}
	if (bVideo)
	{
		buff[2] = 0x01;
	}

	ReturnToUser(buff,3);
	IpcStartEvent(EVENT_GLOBAL_BACK_LOW_VOLUME_ID);
	IpcStartEvent(EVENT_GLOBAL_BACK_VIDEO_SIGNED_ID);
}

static void returnSystemBatteryVotege(P_FLY_SYSTEM_INFO pSystemInfo,BYTE iVoltage)
{
	BYTE buff[2];
	buff[0] = 0x22;
	buff[1] = iVoltage;
	ReturnToUser(buff,2);
}

static void returnSystemSteelwheelName(P_FLY_SYSTEM_INFO pSystemInfo,BYTE *p,UINT length)
{
	BYTE buff[1+256];
	buff[0] = 0x24;

	memcpy(&buff[1],p,length);

	ReturnToUser(buff,1+length);
}

static void returnSystembHaveTV(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bOn)
{
	BYTE buff[] = {0x25,0x00};

	if (bOn)
	{
		buff[1] = 0x01;
	}

	ReturnToUser(buff,2);
}

static void returnSystembHaveTPMS(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bOn)
{
	BYTE buff[] = {0x26,0x00};

	if (bOn)
	{
		buff[1] = 0x01;
	}

	ReturnToUser(buff,2);
}

static void returnSystembHave3G(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bOn)
{
	BYTE buff[] = {0x27,0x00};

	if (bOn)
	{
		buff[1] = 0x01;
	}

	ReturnToUser(buff,2);
}

static void returnSystembHaveAuxVideo(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bHave)
{
	BYTE buff[] = {0x28,0x00};

	if (bHave)
	{
		buff[1] = 0x01;
	}

	ReturnToUser(buff,2);
}

static void returnSystemHaveRestoreRegedit(P_FLY_SYSTEM_INFO pSystemInfo,BYTE iWhat)
{
	BYTE buff[] = {0x29,0x00};

	buff[1] = iWhat;

	ReturnToUser(buff,2);
}

static void returnSystemACCOn(P_FLY_SYSTEM_INFO pSystemInfo)
{
	BYTE buff[] = {0x30,0x00};

	ReturnToUser(buff,2);
}

static void returnSystemACCOff(P_FLY_SYSTEM_INFO pSystemInfo)
{
	BYTE buff[] = {0x31,0x00};

	ReturnToUser(buff,2);
}

static void returnSystemStandbyStatus(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bOn)
{
	BYTE buff[] = {0x32,0x00};

	if (bOn)
	{
		buff[1] = 0x01;
	}

	ReturnToUser(buff,2);
}

static void returnSystemMCUSoftwareVersion(P_FLY_SYSTEM_INFO pSystemInfo,BYTE *p,UINT length)
{
	BYTE buff[1+MCU_SOFT_VERSION_MAX];
	buff[0] = 0x33;
	memcpy(&buff[1],p,length);
	ReturnToUser(buff,1+length);
}

static void returnSystemDVDSoftwareVersion(P_FLY_SYSTEM_INFO pSystemInfo,BYTE *p,UINT length)
{
	BYTE buff[1+32];
	buff[0] = 0x34;
	memcpy(&buff[1],p,length);
	ReturnToUser(buff,1+length);
}

static void returnSystemBTSoftwareVersion(P_FLY_SYSTEM_INFO pSystemInfo,BYTE *p,UINT length)
{
	BYTE buff[1+32];
	buff[0] = 0x35;
	memcpy(&buff[1],p,length);
	ReturnToUser(buff,1+length);
}
static void returnSystemBarCodeID(P_FLY_SYSTEM_INFO pSystemInfo)//###################
{
	BYTE buff[256];
	buff[0]=0x36;
	memcpy(&buff[1],(void*)&pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.BarCodeID[1],pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.BarCodeID[0]);
	ReturnToUser(buff,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.BarCodeID[0]+1);
}
static void returnSystemTimeSetupPara(P_FLY_SYSTEM_INFO pSystemInfo,BYTE para)
{
	BYTE buff[] = {0x37,0x00};
	
	buff[1]= para;
	ReturnToUser(buff,2);
}
static void returnSystemVolumeMax(P_FLY_SYSTEM_INFO pSystemInfo, BYTE iVolume)
{
	BYTE buff[2] = {0x44,0x00};
	buff[1] = iVolume;
	ReturnToUser(buff,2);
}
static void systemWriteFile(P_FLY_SYSTEM_INFO pSystemInfo,BYTE *p,UINT length)
{
	UINT i;
	if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.bInit)
	{
		if (FALSE == WriteMsgQueue(pSystemInfo->hMsgQueueFromSystemCreate, p, length, 0, 0))
		{
			DWORD dwError = GetLastError();
			DBGI((TEXT("\r\nFlyAudio System Write MsgQueue Error!0x%x"),dwError));
		}
	}
	else
	{
		DBGI((TEXT("\r\nFlyAudio System Write MsgQueue Not Init")));
			for (i = 0;i < length;i++)
			{
				DBGI((TEXT(" %x"),p[i]));
			}
	}
}

static void controlToMCUTestIIC(P_FLY_SYSTEM_INFO pSystemInfo)
{
	BYTE buff[] = {0x00,0xFE,0,1,2,3,4,5,6,7};

	systemWriteFile(pSystemInfo,buff,10);
}

static void controlToMCUPowerOn(P_FLY_SYSTEM_INFO pSystemInfo)
{
	BYTE buff[] = {0x00,0x00,00};

	systemWriteFile(pSystemInfo,buff,3);
}

static void controlToMCUPowerOff(P_FLY_SYSTEM_INFO pSystemInfo)
{
	BYTE buff[] = {0x00,0x01,0x00};

	systemWriteFile(pSystemInfo,buff,3);
}

static void controlToMCUUserPing(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bWork)
{
	BYTE buff[] = {0x00,0x02,0x00};

	if (bWork)
	{
		buff[2] = 0x11;
	}
	else
	{
		buff[2] = 0x10;
	}

	systemWriteFile(pSystemInfo,buff,3);

	if (buff[2] == 0x11)
	{
		//只发一次，起来后第一次发倒车状态
		if (pSystemInfo->bNeedSendBackStatusToUser)
		{
			pSystemInfo->bNeedSendBackStatusToUser = FALSE;
			IpcStartEvent(EVENT_GLOBAL_BACK_VIDEO_SIGNED_ID);
			returnSystemBackStatus(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn);

			DBGI((TEXT("\r\nSystem recv user ping commd and return backaction:backactiveNow:%d,BackVideoOn:%d\r\n"),
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn));
		}
	}

	if(pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName == CAR_CRV_12)
	{
		BYTE buff[] = {0x00,0xFF,0x00};
		if((pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSteelAD[0]>860)//低配版的CRV使用的是AD方向盘
		&&(pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSteelAD[1]>860))		
		{
			buff[2] = 0x0;					
		}
		else
		{
			buff[2] = 0x1;	
		}
		systemWriteFile(pSystemInfo,buff,3);
	}
}

static void controlToMCUReset(P_FLY_SYSTEM_INFO pSystemInfo)
{
	BYTE buff[] = {0x00,0x03,0x04};

	systemWriteFile(pSystemInfo,buff,3);
}

static void controlToMCUDebug(P_FLY_SYSTEM_INFO pSystemInfo)
{
	BYTE buff[] = {0x00,0x03,0x05};

	systemWriteFile(pSystemInfo,buff,3);
}

static void controlToMCULightDetectEnable(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bEnable)
{
	BYTE buff[] = {0x00,0x11,0x00};
	if (bEnable)
	{
		buff[2] = 0x01;
	} 
	else
	{
		buff[2] = 0x00;
	}

	systemWriteFile(pSystemInfo,buff,3);
}

static void controlToMCUbDayNight(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bNight)
{
	BYTE buff[] = {0x00,0x12,0x00};
	if (bNight)
	{
		buff[2] = 0x01;
	} 

	systemWriteFile(pSystemInfo,buff,3);
}

static void controlToMCUbHaveExtAMP(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bHave)
{
	BYTE buff[] = {0x00,0x13,0x00};
	if (bHave)
	{
		buff[2] = 0x01;
	} 
	else
	{
		buff[2] = 0x00;
	}

	systemWriteFile(pSystemInfo,buff,3);
}
static void controlToMCUBackDetectEnable(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bEnable)
{
	BYTE buff[] = {0x00,0x14,0x00};
	buff[2] = bEnable;
	systemWriteFile(pSystemInfo,buff,3);
}
static void controlToMCUPanelLight(P_FLY_SYSTEM_INFO pSystemInfo,BYTE iLight)
{
	BYTE buff[] = {0x00,0x15,0x00};

	buff[2] = iLight;

	systemWriteFile(pSystemInfo,buff,3);
}

static void controlToMCUResetToFactory(P_FLY_SYSTEM_INFO pSystemInfo)
{
	BYTE buff[] = {0x00,0x18,0x00};

	systemWriteFile(pSystemInfo,buff,3);
}

static void controlToMCUKeyDemoMode(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bEnable)
{
	BYTE buff[] = {0x00,0x19,0x00};

	if (bEnable)
	{
		buff[2] = 0x01;
	}
	else
	{
		buff[2] = 0x00;
	}
	systemWriteFile(pSystemInfo,buff,3);
}
static void controlToMCUKeyAccOffMode(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bEnable)
{
	BYTE buff[] = {0x00,0x20,0x00};

	if (bEnable)
	{
		buff[2] = 0x01;
	}
	else
	{
		buff[2] = 0x00;
	}
	systemWriteFile(pSystemInfo,buff,3);
}

static void controlToMCUStandbyStatus(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bStandby)
{
	BYTE buff[] = {0x00,0x32,0x00};

	if (bStandby)
	{
		buff[2] = 0x01;
	}
	else
	{
		buff[2] = 0x00;
	}

	systemWriteFile(pSystemInfo,buff,3);
}

static void controlToMCUAMPOn(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bOn)
{
	BYTE buff[] = {0x00,0x91,0x00};

	if (bOn)
	{
		buff[2] = 0x01;
	}

	systemWriteFile(pSystemInfo,buff,3);
}
static void controlToMCUAMPMute(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bMute)
{
	BYTE buff[] = {0x00,0x92,0x00};

	if (!bMute)
	{
		buff[2] = 0x01;
	}

	systemWriteFile(pSystemInfo,buff,3);
}

static void controlToMCUReqRadioAD(P_FLY_SYSTEM_INFO pSystemInfo)
{
	BYTE buff[] = {0x00,0x93,0x00};

	systemWriteFile(pSystemInfo,buff,3);
}

static void controlToMCULCDLightEn(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bOn)
{
	BYTE buff[] = {0x00,0x94,0x00};

	if (bOn)
	{
		buff[2] = 1;
	}

	systemWriteFile(pSystemInfo,buff,3);
}

static void controlToMCUiPodPower(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bOn)
{
	BYTE buff[] = {0x00,0x95,0x00};

	if (bOn)
	{
		buff[2] = 1;
	}

	systemWriteFile(pSystemInfo,buff,3);
}

static void controlToMCUHowLongToPowerOn(P_FLY_SYSTEM_INFO pSystemInfo,DWORD iTime)
{
	BYTE buff[] = {0x00,0x98,0x00,0x00,0x00,0x00};

	buff[5] = (BYTE)(iTime);
	iTime = iTime >> 8;
	buff[4] = (BYTE)(iTime);
	iTime = iTime >> 8;
	buff[3] = (BYTE)(iTime);
	iTime = iTime >> 8;
	buff[2] = (BYTE)(iTime);

	systemWriteFile(pSystemInfo,buff,6);
}

void controlToMCUPowerToVeryLowOff(P_FLY_SYSTEM_INFO pSystemInfo)
{
	BYTE buff[] = {0x00,0x99};

	systemWriteFile(pSystemInfo,buff,2);
}

static void controlToMCURandom(P_FLY_SYSTEM_INFO pSystemInfo,UINT32 iRandom)
{
	BYTE buff[] = {0x00,0x96,0x00,0x00,0x00,0x00};

	buff[5] = iRandom;
	iRandom = iRandom >> 8;
	buff[4] = iRandom;
	iRandom = iRandom >> 8;
	buff[3] = iRandom;
	iRandom = iRandom >> 8;
	buff[2] = iRandom;

	systemWriteFile(pSystemInfo,buff,6);
}

UINT32 checkShellBaby(UINT32 iRandom)
{
	iRandom = iRandom * 37211237;
	iRandom = iRandom + 95277259;
	iRandom = iRandom * 16300361;
	return iRandom;
}

static DWORD BreakDetect_io_isr(LPVOID lpvParam)
{
	P_FLY_SYSTEM_INFO pSystemInfo = (P_FLY_SYSTEM_INFO)lpvParam;

	pSystemInfo->bBreakDetectIsrRunning = TRUE;

	while (pSystemInfo->hBreakDetectIsrEvent)
	{
		WaitForSingleObject(pSystemInfo->hBreakDetectIsrEvent,INFINITE);
		RETAILMSG(1, (TEXT("\r\nFlyAudio System Break IO Change")));

		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBreakStatusIO
			= HwGpioInput(BREAK_DETECT_I);
		IpcStartEvent(EVENT_GLOBAL_BREAKDETECT_CHANGE_ID);

		pSystemInfo->SystemCarbodyInfo.iBreakDetectDelayRead = GetTickCount();

		//if (pSystemInfo->hBreakDetectIsrEvent)
		//{
		//	SOC_IO_ISR_ReEn(pSystemInfo->hBreakDetectIsrEvent);
		//}
	}

	pSystemInfo->bBreakDetectIsrRunning = FALSE;
	return NULL;
}

static DWORD PhoneDetect_io_isr(LPVOID lpvParam)
{
	P_FLY_SYSTEM_INFO pSystemInfo = (P_FLY_SYSTEM_INFO)lpvParam;

	pSystemInfo->bPhoneDetectIsrRunning = TRUE;

	while (pSystemInfo->hPhoneDetectIsrEvent)
	{
		WaitForSingleObject(pSystemInfo->hPhoneDetectIsrEvent,INFINITE);
		DBGD((TEXT("\r\nFlyAudio System Phone IO Change")));

		IpcStartEvent(EVENT_GLOBAL_PHONEDETECT_CHANGE_ID);

		pSystemInfo->SystemCarbodyInfo.iPhoneDetectDelayRead = GetTickCount();

		//if (pSystemInfo->hPhoneDetectIsrEvent)
		//{
		//	SOC_IO_ISR_ReEn(pSystemInfo->hPhoneDetectIsrEvent);
		//}
	}

	pSystemInfo->bPhoneDetectIsrRunning = FALSE;
	return NULL;
}

void processFanControlByTemperature(P_FLY_SYSTEM_INFO pSystemInfo,INT iTemperature)
{
	if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus
		|| pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute
		|| pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBatteryVoltage < 110)
	{
		if (pSystemInfo->SystemCarbodyInfo.iFanPWMDuty != FAN_OFF)
		{
			pSystemInfo->SystemCarbodyInfo.iFanPWMDuty = FAN_OFF;
			HwPwmSetFreq(FAN_PWM, FAN_OFF);
		}
	}
	else if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bKeyDemoMode)
	{
		if (pSystemInfo->SystemCarbodyInfo.iFanPWMDuty != FAN_ON)
		{
			pSystemInfo->SystemCarbodyInfo.iFanPWMDuty = FAN_ON;
			HwPwmSetFreq(FAN_PWM, FAN_ON);
		}
	}
	else if (iTemperature >= 55 && iTemperature < 60)
	{
		if (pSystemInfo->SystemCarbodyInfo.iFanPWMDuty != FAN_PWN_DUTY)
		{
			pSystemInfo->SystemCarbodyInfo.iFanPWMDuty = FAN_PWN_DUTY;
			HwPwmSetFreq(FAN_PWM, FAN_PWN_DUTY);
		}
	}
	else if (iTemperature >= 60)
	{
		if (pSystemInfo->SystemCarbodyInfo.iFanPWMDuty != FAN_ON)
		{
			pSystemInfo->SystemCarbodyInfo.iFanPWMDuty = FAN_ON;
			HwPwmSetFreq(FAN_PWM, FAN_ON);
		}
	}
	else if (iTemperature < 50)
	{
		if (pSystemInfo->SystemCarbodyInfo.iFanPWMDuty != FAN_OFF)
		{
			pSystemInfo->SystemCarbodyInfo.iFanPWMDuty = FAN_OFF;
			HwPwmSetFreq(FAN_PWM, FAN_OFF);
		}
	}

	DBGD((TEXT("\r\n system temperature is :%d,bStandbyStatus:%d,bMute:%d,iBatteryVoltage:%d,bKeyDemoMode:%d iFanPWMDuty:%d\r\n"), 
		iTemperature,
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus,
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute,
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBatteryVoltage,
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bKeyDemoMode,
		pSystemInfo->SystemCarbodyInfo.iFanPWMDuty));
}

static void RegDataInit(P_FLY_SYSTEM_INFO pSystemInfo)
{
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGPSVolumeLevel = 1;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable = TRUE;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaColor = COLOR_STEP_COUNT/2;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaHue = COLOR_STEP_COUNT/2;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaContrast = COLOR_STEP_COUNT/2;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaBrightness = COLOR_STEP_COUNT/2;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDayNight = FALSE;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightDay = 3;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightNight = 3;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDDriver = 0;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode = 9;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSteerWheelOn = FALSE;
	if(pSystemInfo->bNeedReserveSteelSelectData ==FALSE)
	{
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataUseWhat = 0;
	}
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRemoteUseStudyOn = FALSE;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLightDetectEnable = TRUE;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelLightBright = 0xff;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveExtAmplifier = FALSE;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume = 15;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iExtTelVolume = 15;

}

static void RegDataWrite(P_FLY_SYSTEM_INFO pSystemInfo)
{
	HKEY hKey; 
	DWORD dwDisposition;
	DWORD dwTemp;
	DWORD dwLen;

#if REGEDIT_NOT_SAVE
	return;
#endif

	RETAILMSG(1, (TEXT("\r\nSystem Reg Data Write Start")));

	//写视频参数
	RegCreateKeyEx(HKEY_CURRENT_USER,L"FlyDrivers\\Video",0,NULL,0,
		0,NULL,&hKey,&dwDisposition); /////////////////////////////////创建或打开注册表

	dwLen = 4;
	dwTemp = pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaColor;
	RegSetValueEx(hKey,L"dwVideoParaColor",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	dwLen = 4;
	dwTemp = pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaHue;
	RegSetValueEx(hKey,L"dwVideoParaHue",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	dwLen = 4;
	dwTemp = pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaContrast;
	RegSetValueEx(hKey,L"dwVideoParaContrast",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	dwLen = 4;
	dwTemp = pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaBrightness;
	RegSetValueEx(hKey,L"dwVideoParaBrightness",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);

	RegCloseKey(hKey);

	//写系统参数
	RegCreateKeyEx(HKEY_CURRENT_USER,L"FlyDrivers\\System",0,NULL,0,
		0,NULL,&hKey,&dwDisposition); /////////////////////////////////创建或打开注册表

	//dwLen = 4;
	//if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable)//倒车检测
	//{
	//	dwTemp = 1;
	//}
	//else
	//{
	//	dwTemp = 0;
	//}
	//RegSetValueEx(hKey,L"dwBackDetectEnable",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	
	dwLen = 4;
	dwTemp = pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGPSVolumeLevel; //GPS level
	RegSetValueEx(hKey,L"dwGPSVolumeLevel",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);

	dwLen = 4;
	if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDayNight)
	{
		dwTemp = 1;
	}
	else
	{
		dwTemp = 0;
	}
	RegSetValueEx(hKey,L"dwDayNight",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	dwLen = 4;
	dwTemp = pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightDay;
	RegSetValueEx(hKey,L"dwLCDLightBrightDay",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	dwLen = 4;
	dwTemp = pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightNight;
	RegSetValueEx(hKey,L"dwLCDLightBrightNight",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	dwLen = 4;
	dwTemp = pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDDriver;
	RegSetValueEx(hKey,L"dwRadioID",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	dwLen = 4;
	dwTemp = pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode;
	RegSetValueEx(hKey,L"dwDVDRegion",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	dwLen = 4;
	dwTemp = pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDWantPlayDevice;
	RegSetValueEx(hKey,L"dwDVDPlayDevice",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	dwLen = 4;
	if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSteerWheelOn)
	{
		dwTemp = 1;
	}
	else
	{
		dwTemp = 0;
	}
	RegSetValueEx(hKey,L"dwSteerWheelOn",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	dwLen = 4;
	dwTemp = pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataUseWhat;
	RegSetValueEx(hKey,L"dwRemoteDataUseWhat",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	dwLen = 4;
	if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRemoteUseStudyOn)
	{
		dwTemp = 1;
	}
	else
	{
		dwTemp = 0;
	}
	RegSetValueEx(hKey,L"dwRemoteUseStudyOn",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	dwLen = 4;
	if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLightDetectEnable)
	{
		dwTemp = 1;
	}
	else
	{
		dwTemp = 0;
	}
	RegSetValueEx(hKey,L"dwLightDetectEnable",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	dwLen = 4;
	dwTemp = pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelLightBright;
	RegSetValueEx(hKey,L"dwPanelLightBright",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	dwLen = 4;
	if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveExtAmplifier)
	{
		dwTemp = 1;
	}
	else
	{
		dwTemp = 0;
	}
	RegSetValueEx(hKey,L"dwHaveExtAmplifier",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	dwLen = 4;
	dwTemp = pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume;
	RegSetValueEx(hKey,L"dwBTCallVolume",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	dwLen = 4;
	dwTemp = pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iExtTelVolume;
	RegSetValueEx(hKey,L"dwExtTelVolume",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);

	RegCloseKey(hKey);

	RETAILMSG(1, (TEXT("\r\nSystem Reg Data Write Finish")));
}

static void RegDataRead(P_FLY_SYSTEM_INFO pSystemInfo)
{
	HKEY hKey; 
	DWORD dwDisposition;
	DWORD dwTemp;
	DWORD dwLen;
	DWORD dwType;

#if REGEDIT_NOT_READ
	return;
#endif

	RETAILMSG(1, (TEXT("\r\nSystem Reg Data Read Start")));

	//读取视频参数
	RegCreateKeyEx(HKEY_CURRENT_USER,L"FlyDrivers\\Video",0,NULL,0,
		0,NULL,&hKey,&dwDisposition); /////////////////////////////////创建或打开注册表

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwVideoParaColor",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//色彩
	{
		if (dwTemp > COLOR_STEP_COUNT - 1)
		{
			dwTemp = COLOR_STEP_COUNT/2;
		}
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaColor = (BYTE)dwTemp;
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwVideoParaHue",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//色调
	{
		if (dwTemp > COLOR_STEP_COUNT - 1)
		{
			dwTemp = COLOR_STEP_COUNT/2;
		}
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaHue = (BYTE)dwTemp;
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwVideoParaContrast",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//对比度
	{
		if (dwTemp > COLOR_STEP_COUNT - 1)
		{
			dwTemp = COLOR_STEP_COUNT/2;
		}
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaContrast = (BYTE)dwTemp;
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwVideoParaBrightness",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//亮度
	{
		if (dwTemp > COLOR_STEP_COUNT - 1)
		{
			dwTemp = COLOR_STEP_COUNT/2;
		}
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVideoParaBrightness = (BYTE)dwTemp;
	}

	RegCloseKey(hKey);	

	//读取系统参数
	RegCreateKeyEx(HKEY_CURRENT_USER,L"FlyDrivers\\System",0,NULL,0,
		0,NULL,&hKey,&dwDisposition); /////////////////////////////////创建或打开注册表

	//dwTemp = 0;
	//dwLen = 4;
	//if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwBackDetectEnable",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//倒车检测
	//{
	//	if (dwTemp)
	//	{
	//		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable = TRUE;
	//	} 
	//	else
	//	{
	//		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable = FALSE;
	//	}
	//}
	
	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwGPSVolumeLevel",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//GPS level
	{
		if (dwTemp > 3)
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGPSVolumeLevel = 3;
		else
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGPSVolumeLevel = (BYTE)dwTemp;
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwLightDetectEnable",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//灯光禁止使能
	{
		if (dwTemp)
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLightDetectEnable = TRUE;
		}
		else
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLightDetectEnable = FALSE;
		}
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwPanelLightBright",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//按键灯亮度
	{
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelLightBright = (BYTE)dwTemp;
	}

	if (!pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLightDetectEnable)//灯光检测使能时，此值无效
	{
		dwTemp = 0;
		dwLen = 4;
		if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwDayNight",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//白天或黑夜
		{
			if (dwTemp)
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDayNight = TRUE;
			}
			else
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDayNight = FALSE;
			}
		}
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwLCDLightBrightDay",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//亮度
	{
		if (dwTemp > COLOR_STEP_COUNT - 1)
		{
			dwTemp = COLOR_STEP_COUNT/2;
		}
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightDay = (BYTE)dwTemp;
	}
	if (0 == pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightDay
		|| pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightDay > 3)
	{
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightDay = 3;
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwLCDLightBrightNight",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//亮度
	{
		if (dwTemp > COLOR_STEP_COUNT - 1)
		{
			dwTemp = COLOR_STEP_COUNT/2;
		}
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightNight = (BYTE)dwTemp;
	}
	if (0 == pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightNight
		|| pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightNight > 3)
	{
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightNight = 3;
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwRadioID",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//收音机区域
	{
		if (dwTemp > RADIO_COUNTRY_ID - 1)
		{
			dwTemp = 0;
		}
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDDriver = (BYTE)dwTemp;
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwDVDRegion",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//DVD区域码
	{
		if (dwTemp > 0 && dwTemp <= 6)
		{

		}
		else if (dwTemp == 9)
		{
		}
		else
		{
			dwTemp = 9;
		}
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode = (BYTE)dwTemp;
	}
	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwDVDPlayDevice",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//DVD区域码
	{
		if (dwTemp > 1)
		{
			dwTemp = 0;
		}
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDWantPlayDevice = (BYTE)dwTemp;
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwSteerWheelOn",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//方向盘开关
	{
		if (dwTemp)
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSteerWheelOn = TRUE;
		}
		else
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSteerWheelOn = FALSE;
		}
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwRemoteDataUseWhat",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//方向盘数据选择
	{
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataUseWhat = dwTemp;
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwRemoteUseStudyOn",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//方向盘学习使用
	{
		if (dwTemp)
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRemoteUseStudyOn = TRUE;
		}
		else
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRemoteUseStudyOn = FALSE;
		}
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwHaveExtAmplifier",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//外置功放
	{
		if (dwTemp)
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveExtAmplifier = TRUE;
		}
		else
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveExtAmplifier = FALSE;
		}
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwBTCallVolume",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
	{
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume = (BYTE)dwTemp;
	}
	if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume < 5)
	{
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume = 5;
	}
	if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume > 40)
	{
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume = 40;
	}
	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwExtTelVolume",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
	{
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iExtTelVolume = (BYTE)dwTemp;
	}
	if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iExtTelVolume < 5)
	{
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iExtTelVolume = 5;
	}
	if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iExtTelVolume > 40)
	{
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iExtTelVolume = 40;
	}

	RegCloseKey(hKey);

	IpcStartEvent(EVENT_GLOBAL_REG_TO_SYSTEM_ID);

	RETAILMSG(1, (TEXT("\r\nSystem Reg Data Read Finish")));
}



static DWORD WINAPI ThreadInter(LPVOID pContext)
{
	P_FLY_SYSTEM_INFO pSystemInfo = (P_FLY_SYSTEM_INFO)pContext;
	ULONG WaitReturn;

	while (!pSystemInfo->bKillDispatchFlyInterThread)
	{
		WaitReturn = WaitForSingleObject(pSystemInfo->hDispatchInterThreadEvent, INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGI((TEXT("\r\nFlyAudio System hDispatchInterThreadEvent Error")));
		}

	}
	pSystemInfo->FLyInterThreadHandle = NULL;
	DBGD((TEXT("\r\nFlyAudio System ThreadInter exit")));
		return 0;
}

static DWORD WINAPI ThreadMain(LPVOID pContext)
{
	P_FLY_SYSTEM_INFO pSystemInfo = (P_FLY_SYSTEM_INFO)pContext;
	ULONG WaitReturn;

	pSystemInfo->bPowerUp = TRUE;

	while (!pSystemInfo->bKillDispatchFlyMainThread)
	{
		if (pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhat || pSystemInfo->SystemCarbodyInfo.iProcVoltageShakeDelayTime)
		{
			WaitReturn = WaitForSingleObject(pSystemInfo->hDispatchMainThreadEvent, 618);
			DBGD((TEXT("\r\nFlyAudio System Run Due Sleep Proc")));
		}
		else if (pSystemInfo->SystemCarbodyInfo.iBreakDetectDelayRead
			|| pSystemInfo->SystemCarbodyInfo.iPhoneDetectDelayRead
			|| pSystemInfo->bRecRegeditRestoreMsg
			|| pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecWinCESleepMsg
			|| pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecWinCERestartMsg
			|| pSystemInfo->dwBackDetectTimer)
		{
			WaitReturn = WaitForSingleObject(pSystemInfo->hDispatchMainThreadEvent, 100);
			DBGD((TEXT("\r\nFlyAudio System Run Due Break Or Phone Proc")));
		}
		else
		{
			WaitReturn = WaitForSingleObject(pSystemInfo->hDispatchMainThreadEvent, CHECK_SHELL_BABY_INNER_TIME);
		}


	
		if (pSystemInfo->dwBackDetectTimer)
		{
			if (GetTickCount() - pSystemInfo->dwBackDetectTimer >= 800)
			{
				pSystemInfo->dwBackDetectTimer = 0;
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable = pSystemInfo->bBackDetectEnable;
				controlToMCUBackDetectEnable(pSystemInfo,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable);
				IpcStartEvent(EVENT_GLOBAL_BACK_LOW_VOLUME_ID);

				//倒车是否使能，发给倒车模块
				SetEventData(pSystemInfo->hBackDetectEnableEvent,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable);
				SetEvent(pSystemInfo->hBackDetectEnableEvent);
			}
		}

		if (pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhat)//应用程序处理超时
		{
			if (GetTickCount() - pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnTime > 20000)
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLEDTestFlashCount = pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhat;
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLEDTestFlashCountSub = pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhatSub;
				pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhat = 0;
				IpcStartEvent(EVENT_GLOBAL_ERROR_LEDFLASH_ID);
			}
		}

		if (GetTickCount() - pSystemInfo->SystemCarbodyInfo.iSendShellBabyTime >= CHECK_SHELL_BABY_INNER_TIME)
		{
			if (pSystemInfo->SystemCarbodyInfo.iShellBabySend)
			{
				if (pSystemInfo->SystemCarbodyInfo.iShellBabyRec == checkShellBaby(pSystemInfo->SystemCarbodyInfo.iShellBabySend))
				{
					pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCheckShellBabyError = FALSE;
				}
				else
				{
					pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCheckShellBabyError = TRUE;
				}
			}
			pSystemInfo->SystemCarbodyInfo.iSendShellBabyTime = GetTickCount();
			pSystemInfo->SystemCarbodyInfo.iShellBabySend = rand();
			pSystemInfo->SystemCarbodyInfo.iShellBabySend = pSystemInfo->SystemCarbodyInfo.iShellBabySend << 16;
			pSystemInfo->SystemCarbodyInfo.iShellBabySend += rand();
			controlToMCURandom(pSystemInfo,pSystemInfo->SystemCarbodyInfo.iShellBabySend);
		}

		if (WAIT_TIMEOUT == WaitReturn)
		{
			if (pSystemInfo->SystemCarbodyInfo.iBreakDetectDelayRead)//延时检测刹车
			{
				if (GetTickCount() - pSystemInfo->SystemCarbodyInfo.iBreakDetectDelayRead >= 100)
				{
					pSystemInfo->SystemCarbodyInfo.iBreakDetectDelayRead = 0;
					pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBreakStatusIO
						= HwGpioInput(BREAK_DETECT_I);
					IpcStartEvent(EVENT_GLOBAL_BREAKDETECT_CHANGE_ID);
				}
			}
			if (pSystemInfo->SystemCarbodyInfo.iPhoneDetectDelayRead)
			{
				if (GetTickCount() - pSystemInfo->SystemCarbodyInfo.iPhoneDetectDelayRead >= 100)
				{
					pSystemInfo->SystemCarbodyInfo.iPhoneDetectDelayRead = 0;
					pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bExtPhoneStatusIO
						= HwGpioInput(PHONE_DETECT_I);
					IpcStartEvent(EVENT_GLOBAL_PHONEDETECT_CHANGE_ID);
				}
			}
		}

		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bExtPhoneStatusIO
			= HwGpioInput(PHONE_DETECT_I);

		if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bVolumeMaxReady)
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bVolumeMaxReady = FALSE;
			returnSystemVolumeMax(pSystemInfo, 
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax);
		}

		if (IpcWhatEvent(EVENT_GLOBAL_IIC_MCU_ID))
		{
			while (1)
			{
				controlToMCUTestIIC(pSystemInfo);
				Sleep(1);
			}
		}

		if (pSystemInfo->bUserPowerUp)
		{
			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDDriver
				!= pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser)
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDDriver
					= pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser;
				IpcStartEvent(EVENT_GLOBAL_RADIO_CHANGE_ID);
			}
		}

		if (IpcWhatEvent(EVENT_GLOBAL_USER_PING_START_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_USER_PING_START_ID);
			controlToMCUUserPing(pSystemInfo,FALSE);
		}
		if (IpcWhatEvent(EVENT_GLOBAL_USER_PING_WORK_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_USER_PING_WORK_ID);
			controlToMCUUserPing(pSystemInfo,TRUE);
		}

		if (IpcWhatEvent(EVENT_GLOBAL_FORCE_RESET_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_FORCE_RESET_ID);
			controlToMCUReset(pSystemInfo);
		}

		if (IpcWhatEvent(EVENT_GLOBAL_RETURN_FLASH_DATA_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_RETURN_FLASH_DATA_ID);
			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bReadUpdateData)
			{
				returnSystemSteelwheelName(pSystemInfo
					,(BYTE*)pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sRemoteDataName
					,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataNameLength);

				returnSystemMCUSoftwareVersion(pSystemInfo
					,(BYTE*)pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion
					,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersionLength);
			}
		}

		if (IpcWhatEvent(EVENT_GLOBAL_RETURN_DVD_VERSION_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_RETURN_DVD_VERSION_ID);
			returnSystemDVDSoftwareVersion(pSystemInfo
				,(BYTE*)pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersionLength);
		}

		if (IpcWhatEvent(EVENT_GLOBAL_RETURN_BAR_CODE_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_RETURN_BAR_CODE_ID);
			returnSystemBarCodeID(pSystemInfo);
			
		}
		if (IpcWhatEvent(EVENT_GLOBAL_RETURN_BT_VERSION_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_RETURN_BT_VERSION_ID);
			returnSystemBTSoftwareVersion(pSystemInfo
				,(BYTE*)pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTSoftwareVersion
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTSoftwareVersionLength);
		}

		if (IpcWhatEvent(EVENT_GLOBAL_RADIO_AD_REQUEST_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_RADIO_AD_REQUEST_ID);
			controlToMCUReqRadioAD(pSystemInfo);
		}

		//////////////////////////////////////////注册表保存//////////////////////////////////////////////////////////
		//等特UI发 “可以待机命令” 下来
		if (pSystemInfo->bRecRegeditRestoreMsg)
		{
			if (0x01 == pSystemInfo->iRecRegeditRestoreWhat)
			{
				//恢复出厂设置
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRestoreRegeditToFactory = TRUE;
			}
			else
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRestoreRegeditToFactory = FALSE;
			}

			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeAudioSleep
				!= pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeAudioSleep)//静音
			{
				IpcStartEvent(EVENT_GLOBAL_SLEEP_PROC_AUDIO_ID);
			}
			LCD_LCDIdle_Off(pSystemInfo);//关背光
			LCD_LCDBright_PWM(pSystemInfo,LCD_OFF);

			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditRadio)
			{
				IpcStartEvent(EVENT_GLOBAL_REG_RESTORE_RADIO_ID);//收音机注册表保存事件
			}
			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditKey)
			{
				IpcStartEvent(EVENT_GLOBAL_REG_RESTORE_KEY_ID);
			}

			if (!pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditRadio
				&& !pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditKey)
			{
				pSystemInfo->bRecRegeditRestoreMsg = FALSE;
				if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecWinCEFactoryMsg
					|| pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRestoreRegeditToFactory)
				{
					//出厂设置，全部参数清0
					RegDataInit(pSystemInfo);
				}

				//写入注册表
				RegDataWrite(pSystemInfo);
				//RegFlushKey(HKEY_CURRENT_USER);
				returnSystemHaveRestoreRegedit(pSystemInfo,pSystemInfo->iRecRegeditRestoreWhat);
				RETAILMSG(1, (TEXT("\r\nSystem WinCE Return Ready Write Regedit!!!!!!!!")));
				printfHowLongTime(pSystemInfo);
					
			}

			
		}

		//if (pSystemInfo->bRegKeepFinish)
		//{
		//	pSystemInfo->bRegKeepFinish = FALSE;
		//	controlToMCUPowerOff(pSystemInfo);//通知可以关机
		//}
		//////////////////////////////////////////////////////////////////////////////////////////////////////


		if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecWinCESleepMsg
			|| pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecWinCERestartMsg)
		{
			if ((pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeAudioSleep
					== pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeAudioSleep)
				&&(pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeVideoSleep
					== pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeVideoSleep)
				&&(pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeBTSleep
					== pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeBTSleep)
				&&(pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeKeySleep
					== pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeKeySleep))
			{
				prePowerDown(pSystemInfo);

				if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecWinCERestartMsg)
				{
					if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecWinCEFactoryMsg)
					{
						RETAILMSG(1, (TEXT("\r\nSystem Control To MCU ResetToFactory!!!!!!!!")));
						printfHowLongTime(pSystemInfo);
						LCD_LCDIdle_Off(pSystemInfo);//关背光
						//Sleep(618);
						pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhat = eLEDControlMCUSleepTimeOut;
						pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhatSub = 0;
						pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnTime = GetTickCount();

						controlToMCUResetToFactory(pSystemInfo);
					}
					else//CPU复位
					{
						RETAILMSG(1, (TEXT("\r\nSystem Control To MCU Reset!!!!!!!!")));
						printfHowLongTime(pSystemInfo);
						LCD_LCDIdle_Off(pSystemInfo);//关背光
					//	Sleep(618);
						pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhat = eLEDControlMCUSleepTimeOut;
						pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhatSub = 0;
						pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnTime = GetTickCount();

						controlToMCUReset(pSystemInfo);
					}
				}
				else if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecWinCESleepMsg)
				{
					controlToMCUHowLongToPowerOn(pSystemInfo,resetReturnNextResetTime(pSystemInfo));
					resetPowerDown(pSystemInfo);

					RETAILMSG(1, (TEXT("\r\nSystem Wait SuWei Say ACC Off 10S!!!!!!!!")));
					printfHowLongTime(pSystemInfo);
					//#if !A4_PLAT
					//WaitForSingleObject(pSystemInfo->hToSuWeiPowerDownRecEvent, 10000);
					//#endif

					RETAILMSG(1, (TEXT("\r\nSystem Control To MCU ACC Off!!!!!!!!")));
					printfHowLongTime(pSystemInfo);
					LCD_LCDIdle_Off(pSystemInfo);//关背光
					//Sleep(618);
					pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhat = eLEDControlMCUSleepTimeOut;
					pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhatSub = 0;
					pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnTime = GetTickCount();

					controlToMCUPowerOff(pSystemInfo);
				}
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecWinCESleepMsg = FALSE;//处理完毕
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecWinCEFactoryMsg = FALSE;//处理完毕
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecWinCERestartMsg = FALSE;//处理完毕
				pSystemInfo->bUserPowerUp = FALSE;
			}
			else
			{
				if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeAudioSleep
					!= pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeAudioSleep)
				{
					pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhatSub = 1;
					IpcStartEvent(EVENT_GLOBAL_SLEEP_PROC_AUDIO_ID);
				}
				if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeVideoSleep
					!= pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeVideoSleep)
				{
					pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhatSub = 2;
					IpcStartEvent(EVENT_GLOBAL_SLEEP_PROC_VIDEO_ID);
				}
				if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeBTSleep
					!= pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeBTSleep)
				{
					pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhatSub = 3;
					IpcStartEvent(EVENT_GLOBAL_SLEEP_PROC_BT_ID);
				}
				if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeKeySleep
					!= pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeKeySleep)
				{
					pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhatSub = 4;
					IpcStartEvent(EVENT_GLOBAL_SLEEP_PROC_KEY_ID);
				}
			}
		}
		else//在非进入低功耗时
		{
			if (pSystemInfo->SystemCarbodyInfo.iProcVoltageShakeDelayTime)//之前电压不正常
			{
				if (GetTickCount() - pSystemInfo->SystemCarbodyInfo.iProcVoltageShakeDelayTime >= LOW_VOLTAGE_DELAY)//电压正常一段时间后
				{
					if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBatteryVoltageLowRadio)//收音机
					{
						pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBatteryVoltageLowRadio = FALSE;
						IpcStartEvent(EVENT_GLOBAL_BATTERY_RECOVERY_RADIO_ID);
						DBGD((TEXT("\r\nFlyAudio System Radio Voltage After Low Proc")));
					}
					if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBatteryVoltageLowAudio)//音频
					{
						pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBatteryVoltageLowAudio = FALSE;
						IpcStartEvent(EVENT_GLOBAL_BATTERY_RECOVERY_AUDIO_ID);
						DBGD((TEXT("\r\nFlyAudio System Audio Voltage After Low Proc")));
					}

					pSystemInfo->SystemCarbodyInfo.iProcVoltageShakeDelayTime = 0;
				}
			}
		}

		if (IpcWhatEvent(EVENT_GLOBAL_POWERON_MCU_INIT_ID))//初始化消息
		{
			IpcClearEvent(EVENT_GLOBAL_POWERON_MCU_INIT_ID);
			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMCUControlLCDLightOn)
			{
				LCD_LCDIdle_On(pSystemInfo);
			}


			DBGD((TEXT("\r\nFlyAudio System bLightDetectEnable:%d, bDayNight:%d"),
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLightDetectEnable,
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDayNight));


			controlToMCULightDetectEnable(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLightDetectEnable);

			if (!pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLightDetectEnable)
			{
				controlToMCUbDayNight(pSystemInfo
					,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDayNight);
			}


			controlToMCUbHaveExtAMP(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveExtAmplifier);
			controlToMCUPanelLight(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelLightBright);
			controlToMCUStandbyStatus(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus);
		}


		if (IpcWhatEvent(EVENT_GLOBAL_KEY_STANDBY_ID))//待机事件
		{
			IpcClearEvent(EVENT_GLOBAL_KEY_STANDBY_ID);

			if (pSystemInfo->bStandbyStatusWithACCOff
				!= pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatusWithACCOff)
			{
				pSystemInfo->bStandbyStatusWithACCOff
					= pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatusWithACCOff;
				if (pSystemInfo->bStandbyStatusWithACCOff)
				{
					pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatusLast
						= pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus;
					pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus = TRUE;
				}
				else
				{
					pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus
						= pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatusLast;
				}
			}
			else if (!pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatusWithACCOff)
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus = 
					!pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus;
			}

			DBGD((TEXT("\r\n______________FlyAudio System Standby %d:%d\r\n"),pSystemInfo->bStandbyStatus,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus));

			if (pSystemInfo->bStandbyStatus
				!= pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)
			{
				pSystemInfo->bStandbyStatus
					= pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus;

				returnSystemStandbyStatus(pSystemInfo
					,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus);
				controlToMCUStandbyStatus(pSystemInfo
					,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus);
				DBGD((TEXT("\r\nFlyAudio System Standby %d\r\n"),pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus));
				IpcStartEvent(EVENT_GLOBAL_STANDBY_AUDIO_ID);
				IpcStartEvent(EVENT_GLOBAL_STANDBY_BT_ID);
				IpcStartEvent(EVENT_GLOBAL_STANDBY_DVD_ID);
				IpcStartEvent(EVENT_GLOBAL_STANDBY_VIDEO_ID);
				if (!pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)//待机背光控制
				{
					LCD_LCDIdle_On(pSystemInfo);//Mootall
				}
				else
				{
					LCD_LCDIdle_Off(pSystemInfo);//Mootall
				}

				
				if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)
				{
					//pSystemInfo->bRecRegeditRestoreMsg = TRUE;
					//pSystemInfo->bRegKeepFinish = TRUE;
					DBGD((TEXT("\r\nFlyAudio System Standby and save file start........\r\n")));
					SetEvent(pSystemInfo->hDispatchMainThreadEvent);
				}

			}
		}

		if (IpcWhatEvent(EVENT_GLOBAL_REG_TO_SYSTEM_ID))//读注册表事件
		{
			IpcClearEvent(EVENT_GLOBAL_REG_TO_SYSTEM_ID);
		}

		if (pSystemInfo->bUserPowerUp)//等到初始化命令之后才返回
		{
			if (IpcWhatEvent(EVENT_GLOBAL_TVBOX_EXIST_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_TVBOX_EXIST_ID);
				returnSystembHaveTV(pSystemInfo
					,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTVModule);
			}
			if (IpcWhatEvent(EVENT_GLOBAL_TPMSBOX_EXIST_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_TPMSBOX_EXIST_ID);
				returnSystembHaveTPMS(pSystemInfo
					,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTPMSModule);
			}
		}

		if (IpcWhatEvent(EVENT_GLOBAL_REMOTE_STUDY_RETURN_START_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_REMOTE_STUDY_RETURN_START_ID);
			returnSystemSteelStudyStatus(pSystemInfo,0x01);
		}
		if (IpcWhatEvent(EVENT_GLOBAL_REMOTE_STUDY_RETURN_WAIT_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_REMOTE_STUDY_RETURN_WAIT_ID);
			returnSystemSteelStudyStatus(pSystemInfo,0x00);
		}
		if (IpcWhatEvent(EVENT_GLOBAL_REMOTE_STUDY_RETURN_FINISH_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_REMOTE_STUDY_RETURN_FINISH_ID);
			returnSystemSteelStudyStatus(pSystemInfo,0x02);
		}

		if (IpcWhatEvent(EVENT_GLOBAL_REMOTE_RETURN_IT_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_REMOTE_RETURN_IT_ID);
			returnSystemSteelwheelName(pSystemInfo
				,(BYTE*)pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sRemoteDataName
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataNameLength);
		}

		if (IpcWhatEvent(EVENT_GLOBAL_BACKDETECT_RETURN_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_BACKDETECT_RETURN_ID);
			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
			{
				returnSystemBackStatus(pSystemInfo
					,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow
					,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn);
			}
			else
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn =FALSE;
				returnSystemBackStatus(pSystemInfo
					,FALSE
					,FALSE);
			}

			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow
				&& pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn)//倒车背光控制
			{
				LCD_LCDIdle_On(pSystemInfo);//Mootall
			}
			else if (!pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus
				&& (!pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedWinCEPowerOff))
			{
				LCD_LCDIdle_On(pSystemInfo);//Mootall
			}
			else
			{
				LCD_LCDIdle_Off(pSystemInfo);//Mootall
			}
		}

		if (IpcWhatEvent(EVENT_GLOBAL_INNER_AMP_ON_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_INNER_AMP_ON_ID);
			controlToMCUAMPOn(pSystemInfo,TRUE);
			DBGD((TEXT("\r\nFlyAudio System AMP XXXXXXXXXXXXXXXXXXXXXXXXX")));
		}

		if (IpcWhatEvent(EVENT_GLOBAL_BREAKDETECT_CHANGE_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_BREAKDETECT_CHANGE_ID);
			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bOSDDemoMode)
			{
				IpcStartEvent(EVENT_GLOBAL_DEMO_OSD_DISPLAY_ID);
			}
			returnSystemBreakStatus(pSystemInfo);
		}

		if (IpcWhatEvent(EVENT_GLOBAL_PHONEDETECT_CHANGE_ID))
		{

			IpcClearEvent(EVENT_GLOBAL_PHONEDETECT_CHANGE_ID);
			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bExtPhoneStatusBUS
				|| pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bExtPhoneStatusIO
				)
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iExtTelCallStatus = 0x03;
			}
			else
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iExtTelCallStatus = 0x00;
			}
			IpcStartEvent(EVENT_GLOBAL_PHONECALLSTATUS_CHANGE_ID);
			DBGD((TEXT("\r\nSystem ExtPhone Status Change %d"),pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iExtTelCallStatus));
		}
		
		if (IpcWhatEvent(EVENT_GLOBAL_LPC_UPDATE))
		{
			IpcClearEvent(EVENT_GLOBAL_LPC_UPDATE);

			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLpcUpdate)
			{
				//做一些保存
				pSystemInfo->bRecRegeditRestoreMsg = TRUE;

				//发送可以升级的命令
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLpcUpdateFlag = TRUE;
				IpcStartEvent(EVENT_GLOBAL_LPC_UPDATA_FLAG);
			}
			else
			{
				//发送可以升级的命令
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLpcUpdateFlag = FALSE;
				IpcStartEvent(EVENT_GLOBAL_LPC_UPDATA_FLAG);
			}
		}

		if (IpcWhatEvent(EVENT_GOBAL_BACKACTION_PWM))
		{
			IpcClearEvent(EVENT_GOBAL_BACKACTION_PWM);
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActioning = FALSE;
			
			DBGD((TEXT("\r\n################################\r\n")));
		}

		if (!pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActioning)//如果有倒车时，等待倒车状态再去控PWM的亮度
		{	
			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDayNight)//设置亮度
			{
				pSystemInfo->SystemCarbodyInfo.preLightBrightDuty = LCD_LED_NIGHT_PARA[pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightNight];
			}
			else
			{
				pSystemInfo->SystemCarbodyInfo.preLightBrightDuty = LCD_LED_DAY_PARA[pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightDay];
			}

			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSilencePowerUp
				|| pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBatteryVoltageLowRadio
				|| pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBatteryVoltageLowAudio)//低电压
			{
				pSystemInfo->SystemCarbodyInfo.tmpLightBrightDuty = 0;
			}
			else if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable
				&& pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn)//倒车且有视频
			{
				if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDayNight)
				{
					pSystemInfo->SystemCarbodyInfo.tmpLightBrightDuty = LCD_LED_NIGHT_PARA[3];
				}
				else
				{
					pSystemInfo->SystemCarbodyInfo.tmpLightBrightDuty = LCD_LED_DAY_PARA[3];
				}
			}
			else if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)//待机
			{
				pSystemInfo->SystemCarbodyInfo.tmpLightBrightDuty = 0;
			}
			else//正常时
			{
				pSystemInfo->SystemCarbodyInfo.tmpLightBrightDuty = pSystemInfo->SystemCarbodyInfo.preLightBrightDuty;
			}

			if (IpcWhatEvent(EVENT_GLOBAL_FORCE_LCD_LIGHT_ON_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_FORCE_LCD_LIGHT_ON_ID);
				//pSystemInfo->SystemCarbodyInfo.tmpLightBrightDuty = LCD_LED_DAY_PARA[3];
			}

			if (pSystemInfo->SystemCarbodyInfo.curLightBrightDuty != pSystemInfo->SystemCarbodyInfo.tmpLightBrightDuty)//设置亮度
			{
				pSystemInfo->SystemCarbodyInfo.curLightBrightDuty = pSystemInfo->SystemCarbodyInfo.tmpLightBrightDuty;

				DBGD((TEXT("\r\nFlyAudio System Set LCD PWM %d"),pSystemInfo->SystemCarbodyInfo.curLightBrightDuty));
				LCD_LCDBright_PWM(pSystemInfo,pSystemInfo->SystemCarbodyInfo.curLightBrightDuty);
				//if (pSystemInfo->SystemCarbodyInfo.curLightBrightDuty)
				//{
				//	LCD_LCDIdle_On(pSystemInfo);
				//}
				//else
				//{
				//	LCD_LCDIdle_Off(pSystemInfo);
				//}
				
			}
		}
		if (!pSystemInfo->bSocketConnecting)
		{
			DBGE((TEXT("\r\nSystem socket can't recv msg(ensure the socket is connect and send init commd)\r\n")));
		}

		DBGD((TEXT("\r\nDrivers build : Date%d-%d-%d  Time%d:%d:%d GlobalMemorySize:%d\r\n")
			,year,months,day,hours,minutes,seconds,sizeof(struct shm)));
	}

	while (pSystemInfo->FLyInterThreadHandle)
	{
		SetEvent(pSystemInfo->hDispatchInterThreadEvent);
		Sleep(10);
	}
	CloseHandle(pSystemInfo->hDispatchInterThreadEvent);

	pSystemInfo->FlyMainThreadHandle = NULL;
	CloseHandle(pSystemInfo->hDispatchMainThreadEvent);
	DBGD((TEXT("\r\nFlyAudio System ThreadMain exit")));
		return 0;
}

static DWORD WINAPI MT3360MuteThreadProc(LPVOID pContext)
{
	P_FLY_SYSTEM_INFO pSystemInfo = (P_FLY_SYSTEM_INFO)pContext;
	BOOL bMute = pSystemInfo->bFrom3360Mute;

	while (TRUE)
	{
		WaitForSingleObject(pSystemInfo->hFrom3360MuteEventHandle, INFINITE); 
		ResetEvent(pSystemInfo->hFrom3360MuteEventHandle); 
		DWORD dwMuteData = GetEventData(pSystemInfo->hFrom3360MuteEventHandle);

		if ((dwMuteData & (UNMUTE_STATE<<FRONT_SEAT)) == 0) 
			pSystemInfo->bFrom3360Mute = TRUE;
		else 
			pSystemInfo->bFrom3360Mute = FALSE;
	
		//mute   
		DBGD((TEXT("\r\nMT3360 Front Mute status:%d \r\n"),pSystemInfo->bFrom3360Mute)); 

		//if (bMute != pSystemInfo->bFrom3360Mute){
		//	bMute = pSystemInfo->bFrom3360Mute;
		//	controlToMCUAMPMute(pSystemInfo,pSystemInfo->bFrom3360Mute);
		//}
	}
}
static DWORD WINAPI ThreadRead(LPVOID pContext)
{
	P_FLY_SYSTEM_INFO pSystemInfo = (P_FLY_SYSTEM_INFO)pContext;
	ULONG WaitReturn;
	BYTE buff[FLY_MSG_QUEUE_TO_SYSTEM_LENGTH];
	DWORD dwRes;
	DWORD dwMsgFlag;

	while (!pSystemInfo->bKillDispatchFlyMsgQueueReadThread)
	{
		WaitReturn = WaitForSingleObject(pSystemInfo->hMsgQueueToSystemCreate, INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGI((TEXT("\r\nFlyAudio System ThreadRead Event hMsgQueueToSystemOpen Error")));
		}
		ReadMsgQueue(pSystemInfo->hMsgQueueToSystemCreate, buff, FLY_MSG_QUEUE_TO_SYSTEM_LENGTH, &dwRes, 0, &dwMsgFlag);

		if (pSystemInfo->bPowerUp)
		{
			if(dwRes)
			{
				DBGD((TEXT("\r\nSystem Read MsgQueue len:%d bytes:%x %x Open%d\r\n"),dwRes,buff[0],buff[1],pSystemInfo->bOpen));
				switch (buff[0])
				{
					case 0x00:
						break;
					case 0x01:
						if (!pSystemInfo->bAccOffBreakRecvForUser)
						{

							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedWinCEPowerOff = TRUE;
							pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhat = eLEDPreSleepTimeOut;
							pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhatSub = 0;
							pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnTime = GetTickCount();

							LCD_LCDIdle_Off(pSystemInfo);//关背光

							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow = FALSE;//退出倒车
							IpcStartEvent(EVENT_GLOBAL_BACK_LOW_VOLUME_ID);
							IpcStartEvent(EVENT_GLOBAL_BACKDETECT_RETURN_ID);
							IpcStartEvent(EVENT_GLOBAL_BACKDETECT_CHANGE_ID);

							if (pSystemInfo->bUserPowerUp)
							{
								returnSystemACCOff(pSystemInfo);
								RETAILMSG(1, (TEXT("\r\nSystem Return To WinCE Power Off!!!!!!!!")));
								printfHowLongTime(pSystemInfo);
							}
							else
							{
								RETAILMSG(1, (TEXT("\r\nSystem Storage To WinCE Power Off!!!!!!!!")));
								printfHowLongTime(pSystemInfo);
							}
						}
						else
						{
							DBGD((TEXT("\r\nSystem User had recv acc off commd")));
						}
						break;
					case 0x08:
						if (0x01 == buff[1])
						{
							returnSystemACCOn(pSystemInfo);
							DBGD((TEXT("\r\nSystem Return To WinCE ACC On!!!!!!!!")));
							printfHowLongTime(pSystemInfo);
						}
						break;
					case 0x09:
						if (0x00 == buff[1])
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatusWithACCOff = TRUE;
						}
						else if (0x01 == buff[1])
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatusWithACCOff = FALSE;
						}
						pSystemInfo->bAccOffBreakRecvForUser = FALSE;
						RETAILMSG(1, (TEXT("\r\nSystem Rec MCU PreACC %d!!!!!!!!"),buff[1]));
						printfHowLongTime(pSystemInfo);
						IpcStartEvent(EVENT_GLOBAL_KEY_STANDBY_ID);
						break;
					case 0x11:
						if (0x01 == buff[1])
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLightDetectEnable = TRUE;
						}
						else if (0x00 == buff[1])
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLightDetectEnable = FALSE;
						}
						returnLightDetectEnable(pSystemInfo,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLightDetectEnable);
						if (pSystemInfo->bPowerUp)
						{
							SetEvent(pSystemInfo->hDispatchMainThreadEvent);
						}
						break;
					case 0x12:
						if (0x01 == buff[1])
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDayNight = TRUE;
						}
						else if (0x00 == buff[1])
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDayNight = FALSE;
						}
						if(pSystemInfo->bUserPowerUp == TRUE)//20120518
							returnSystembDayNight(pSystemInfo,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDayNight);
						
						if (pSystemInfo->bPowerUp)
						{
							SetEvent(pSystemInfo->hDispatchMainThreadEvent);
						}
						break;
					case 0x13:
						if (0x01 == buff[1])
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveExtAmplifier = TRUE;
						} 
						else
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveExtAmplifier = FALSE;
						}
						returnSystembHaveExtAmp(pSystemInfo,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveExtAmplifier);
						break;
					case 0x15:
						pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelLightBright = buff[1];
						returnSystemPanelBright(pSystemInfo,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelLightBright);
						if (pSystemInfo->bPowerUp)
						{
							SetEvent(pSystemInfo->hDispatchMainThreadEvent);
						}
						break;
					case 0x20:
						if (0x01 == buff[1])
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBreakStatusBUS = TRUE;
						} 
						else if (0x00 == buff[1])
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBreakStatusBUS = FALSE;
						}
						IpcStartEvent(EVENT_GLOBAL_BREAKDETECT_CHANGE_ID);
						break;
					case 0x21:
						if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedWinCEPowerOff)
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow = FALSE;
						}
						else if (0x01 == buff[1])
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow = TRUE;
						} 
						else if (0x00 == buff[1])
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow = FALSE;
						}

						pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActioning 
							= pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow;
						if (!pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActioning)
						{
							IpcStartEvent(EVENT_GLOBAL_BACK_EXIT_DEPENDON_LPC_ID);//退出倒车时优先处理
						}
						IpcStartEvent(EVENT_GLOBAL_BACK_LOW_VOLUME_ID);
						//IpcStartEvent(EVENT_GLOBAL_BACKDETECT_CHANGE_ID);
						break;
					case 0x22:
						pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBatteryVoltage = buff[1];
						returnSystemBatteryVotege(pSystemInfo
							,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBatteryVoltage);

						//电压抖动处理
						if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBatteryVoltage
							< pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNeedProcVoltageShakeRadio)//低于收音机
						{
							pSystemInfo->SystemCarbodyInfo.iProcVoltageShakeDelayTime = GetTickCount();
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBatteryVoltageLowRadio = TRUE;
							SetEvent(pSystemInfo->hDispatchMainThreadEvent);
							DBGD((TEXT("\r\nFlyAudio System Radio Voltage Low")));
						}
						if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBatteryVoltage
							< pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNeedProcVoltageShakeAudio)//低于音频
						{
							pSystemInfo->SystemCarbodyInfo.iProcVoltageShakeDelayTime = GetTickCount();
							if (!pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBatteryVoltageLowAudio)
							{
								pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBatteryVoltageLowAudio = TRUE;
								IpcStartEvent(EVENT_GLOBAL_BATTERY_RECOVERY_AUDIO_ID);
							}
							SetEvent(pSystemInfo->hDispatchMainThreadEvent);
							DBGD((TEXT("\r\nFlyAudio System Audio Voltage Low")));
						}
						break;
					case 0x23:
						pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iHostTemperature
							= RTExchangeToTemperature(buff[1]*256+buff[2]);
						//DBGD((TEXT("\r\nFlyAudio System AD:%d\r\n"),buff[1]*256+buff[2]));
						processFanControlByTemperature(pSystemInfo
							,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iHostTemperature);
						break;
					case 0x24:
						if(buff[1] == 0x1)
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bTimeSetupMode = 1;
						}
						else if(buff[1] == 0x02)
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bTimeSetupMode = 0;
						}
						returnSystemTimeSetupPara(pSystemInfo,buff[1]);
						break;
					case 0x41:case 0x42:case 0x43:
						ReturnToUser(buff,(UINT16)dwRes);
						break;
					case 0x95:
						if (0x01 == buff[1])
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bExtPhoneStatusBUS = TRUE;
						}
						else if (0x00 == buff[1])
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bExtPhoneStatusBUS = FALSE;
						}
						IpcStartEvent(EVENT_GLOBAL_PHONEDETECT_CHANGE_ID);
						break;
					case 0x96:
						pSystemInfo->SystemCarbodyInfo.iShellBabyRec = buff[1];
						pSystemInfo->SystemCarbodyInfo.iShellBabyRec = pSystemInfo->SystemCarbodyInfo.iShellBabyRec << 8;
						pSystemInfo->SystemCarbodyInfo.iShellBabyRec += buff[2];
						pSystemInfo->SystemCarbodyInfo.iShellBabyRec = pSystemInfo->SystemCarbodyInfo.iShellBabyRec << 8;
						pSystemInfo->SystemCarbodyInfo.iShellBabyRec += buff[3];
						pSystemInfo->SystemCarbodyInfo.iShellBabyRec = pSystemInfo->SystemCarbodyInfo.iShellBabyRec << 8;
						pSystemInfo->SystemCarbodyInfo.iShellBabyRec += buff[4];
						if (pSystemInfo->SystemCarbodyInfo.iShellBabyRec == checkShellBaby(pSystemInfo->SystemCarbodyInfo.iShellBabySend))
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCheckShellBabyError = FALSE;
							DBGD((TEXT("\r\nFlyAudio System Shell Baby OK")));
						}
						else
						{
							pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCheckShellBabyError = TRUE;
							DBGD((TEXT("\r\nFlyAudio System Shell Baby Error")));
						}
						break;
					case 0x97:
						pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSilencePowerUp = TRUE;
						IpcStartEvent(EVENT_GLOBAL_SILENCE_POWER_UP_ID);
						if (pSystemInfo->SilencePowerOffInfo.bReadRegData)
						{
							resetPowerProcOnRecMCUWakeup(pSystemInfo);
						}
						break;

					case 0xA0:
						if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDayNight
							|| pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLightDetectEnable)
						{
							if (buff[1] > 100)
								buff[1] = 100;

							LCD_PanelPen_PWM(pSystemInfo, 100 - buff[1]);
						}
						else
						{
							LCD_PanelPen_PWM(pSystemInfo, KEY_OFF);
						}
						break;

					case 0xA2:
						printfMCUStringToFlash(&buff[1],dwRes-1);
						break;
					default:
						DBGD((TEXT("\r\nFlyAudio System ThreadRead Unhandle%d %x %x"),dwRes,buff[0],buff[1]));
							break;
				}
			}
		}
	}
	DBGD((TEXT("\r\nFlyAudio System ThreadRead exit")));
		pSystemInfo->FLyMsgQueueReadThreadHandle = NULL;
	return 0;
}

void FlySystemEnable(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bEnable)
{
	DWORD dwThreadID;

	if (bEnable)
	{
		if (pSystemInfo->bPower)
		{
			return;
		}
		pSystemInfo->bPower = TRUE;

		pSystemInfo->bKillDispatchFlyMainThread = FALSE;
		pSystemInfo->hDispatchMainThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
		pSystemInfo->FlyMainThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadMain, //线程的全局函数
			pSystemInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGI((TEXT("\r\nFlyAudio System ThreadMain ID:%x"),dwThreadID));
			if (NULL == pSystemInfo->FlyMainThreadHandle)
			{
				pSystemInfo->bKillDispatchFlyMainThread = TRUE;
				return;
			}
			SetEvent(pSystemInfo->hDispatchMainThreadEvent);//初始运行一次

			pSystemInfo->bKillDispatchFlyInterThread = FALSE;
			pSystemInfo->hDispatchInterThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
			pSystemInfo->FLyInterThreadHandle = 
				CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
				0,//初始化线程栈的大小，缺省为与主线程大小相同
				(LPTHREAD_START_ROUTINE)ThreadInter, //线程的全局函数
				pSystemInfo, //此处传入了主框架的句柄
				0, &dwThreadID );
			DBGI((TEXT("\r\nFlyAudio System ThreadInter ID:%x"),dwThreadID));
				if (NULL == pSystemInfo->FLyInterThreadHandle)
				{
					pSystemInfo->bKillDispatchFlyInterThread = TRUE;
					return;
				}

		pSystemInfo->hBackDetectEnableEvent = CreateEvent(NULL, FALSE, TRUE, EVENT_BACKCAR_LISTEN_THREAD);	
	}
	else
	{
		if (!pSystemInfo->bPower)
		{
			return;
		}
		pSystemInfo->bPower = FALSE;
		pSystemInfo->bPowerUp = FALSE;

		pSystemInfo->bKillDispatchFlyMainThread = TRUE;
		SetEvent(pSystemInfo->hDispatchMainThreadEvent);

		pSystemInfo->bKillDispatchFlyInterThread = TRUE;
		SetEvent(pSystemInfo->hDispatchInterThreadEvent);

		while (pSystemInfo->FlyMainThreadHandle)
		{
			SetEvent(pSystemInfo->hDispatchMainThreadEvent);
			Sleep(10);
		}
	}
}

void FlySystemPowerEnable(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bEnable)
{
	//if (bEnable)
	//{
	//	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBreakStatusIO
	//		= HwGpioInput(BREAK_DETECT_I);//之前做一次检测
	//	SOC_IO_ISR_Add(BREAK_DETECT_G,BREAK_DETECT_I,GPIO_INTTYPE_RISE_FALL_EDGE,BreakDetect_io_isr,pSystemInfo,&pSystemInfo->hBreakDetectIsrEvent);
	//	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bExtPhoneStatusIO
	//		= HwGpioInput(PHONE_DETECT_I);//之前做一次检测
	//	SOC_IO_ISR_Add(PHONE_DETECT_G,PHONE_DETECT_I,GPIO_INTTYPE_RISE_FALL_EDGE,PhoneDetect_io_isr,pSystemInfo,&pSystemInfo->hPhoneDetectIsrEvent);
	//} 
	//else
	//{
	//	SOC_IO_ISR_Del(&pSystemInfo->hBreakDetectIsrEvent);
	//	//while (pSystemInfo->bBreakDetectIsrRunning)
	//	//{
	//	//	Sleep(10);
	//	//}

	//	SOC_IO_ISR_Del(&pSystemInfo->hPhoneDetectIsrEvent);
	//	//while (pSystemInfo->bPhoneDetectIsrRunning)
	//	//{
	//	//	Sleep(10);
	//	//}
	//}
}

static VOID WinCECommandProcessor(P_FLY_SYSTEM_INFO pSystemInfo,BYTE *buff,UINT len)
{
	switch (buff[0])
	{
	case 0x01:
		if (0x01 == buff[1])
		{
			RegDataInit(pSystemInfo);
			RegDataRead(pSystemInfo);
			resetPowerOn(pSystemInfo);

			IpcStartEvent(EVENT_GLOBAL_REMOTE_CHANGE_IT_ID);


			//LCD_LCDIdle_On(pSystemInfo);//开背光
			returnSystemBackDetectEnable(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable);
			
			//倒车是否使能，发给倒车模块
			SetEventData(pSystemInfo->hBackDetectEnableEvent,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable);
			SetEvent(pSystemInfo->hBackDetectEnableEvent);

			
			returnLightDetectEnable(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLightDetectEnable);

			controlToMCULightDetectEnable(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLightDetectEnable);//有时候出现不一致
			
			if (!pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLightDetectEnable)
			{
				controlToMCUbDayNight(pSystemInfo
					,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDayNight);
			}
			returnSystembDayNight(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDayNight);

			returnSystemGpsVolumeLevel(pSystemInfo,
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGPSVolumeLevel);
			

			returnSystembHaveExtAmp(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveExtAmplifier);
			returnSystemPanelBright(pSystemInfo,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelLightBright);
			
			controlToMCUPanelLight(pSystemInfo,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelLightBright);//有时出现不一致
			
			returnSystemSteelwheelbOn(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSteerWheelOn);
			returnSystemRadioID(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser);
			returnSystemDemoMode(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bOSDDemoMode);
			returnSystemSteelwheelUseStudy(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRemoteUseStudyOn);
			returnDVDRegionCode(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode);
			returnSystemBreakStatus(pSystemInfo);
			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable)
			{
				returnSystemBackStatus(pSystemInfo
					,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow
					,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn
					);
			}
			else
			{
				returnSystemBackStatus(pSystemInfo
					,FALSE
					,FALSE
					);
			}
			returnSystemBatteryVotege(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBatteryVoltage);
			//returnSystembHaveTV(pSystemInfo
			//	,FALSE);
			returnSystembHaveTV(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTVModule);
			//returnSystembHaveTPMS(pSystemInfo
			//	,FALSE);
			returnSystembHaveTPMS(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTPMSModule);
			returnSystembHave3G(pSystemInfo
				,FALSE);

			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bReadUpdateData)
			{
				returnSystemSteelwheelName(pSystemInfo
					,(BYTE*)pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sRemoteDataName
					,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataNameLength);

				returnSystemMCUSoftwareVersion(pSystemInfo
					,(BYTE*)pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersion
					,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iMCUSoftVersionLength);
			}


			returnSystemPowerMode(pSystemInfo,TRUE);
			returnSystembInit(pSystemInfo,TRUE);

			controlToMCUPowerOn(pSystemInfo);


			returnSystemStandbyStatus(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus);
			controlToMCUStandbyStatus(pSystemInfo
				,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus);

			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedWinCEPowerOff)//和WinCE同步关机
			{
				returnSystemACCOff(pSystemInfo);
			}

			pSystemInfo->bUserPowerUp = TRUE;

			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSilencePowerUp)
			{
				resetPowerProcOnRecMCUWakeup(pSystemInfo);
			}
		}
		else
		{
			returnSystemPowerMode(pSystemInfo,FALSE);
		}

		if (pSystemInfo->bPowerUp)
		{
			SetEvent(pSystemInfo->hDispatchMainThreadEvent);
		}
		break;
	case 0x03:
		if (0x00 == buff[1])
		{
			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataCount)
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataUseWhat++;
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataUseWhat %=
					(pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataCount);
				IpcStartEvent(EVENT_GLOBAL_REMOTE_CHANGE_IT_ID);
			}
		}
		else if (0x01 == buff[1])
		{
			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataCount)
			{
				if (0x00 == pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataUseWhat)
				{
					pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataUseWhat = 
						pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataCount - 1;
				}
				else
				{
					pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteDataUseWhat--;
				}
				IpcStartEvent(EVENT_GLOBAL_REMOTE_CHANGE_IT_ID);
			}
		}
		else if (0x02 == buff[1])
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser++;
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser %= RADIO_COUNTRY_ID;		
			returnSystemRadioID(pSystemInfo,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser);
			SetEvent(pSystemInfo->hDispatchMainThreadEvent);
		}
		else if (0x03 == buff[1])
		{
			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser)
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser--;
			}
			else
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser = RADIO_COUNTRY_ID - 1;		
			}
			returnSystemRadioID(pSystemInfo,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser);
			SetEvent(pSystemInfo->hDispatchMainThreadEvent);
		}
		else if (0x04 == buff[1])
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecWinCERestartMsg = TRUE;
			if (pSystemInfo->bPowerUp)
			{
				SetEvent(pSystemInfo->hDispatchMainThreadEvent);
				LCD_LCDIdle_Off(pSystemInfo);//关背光
				LCD_LCDBright_PWM(pSystemInfo,LCD_OFF);
			}
			RETAILMSG(1, (TEXT("\r\nSystem WinCE Write Reset!!!!!!!!")));
			printfHowLongTime(pSystemInfo);
		}
		else if (0x05 == buff[1])
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bOSDDemoMode = TRUE;
			//pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bOSDDemoMode = !pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bOSDDemoMode;
			IpcStartEvent(EVENT_GLOBAL_DEMO_OSD_START_ID);
			controlToMCUDebug(pSystemInfo);
		}
		else if (0x06 == buff[1])
		{
			if (!(pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode == 9 
				|| (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode > 0 && pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode <= 6)))
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode = 9;	
			}
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode++;
			if (10 == pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode)
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode = 1;
			}
			else if (7 == pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode)
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode = 9;
			}
			returnDVDRegionCode(pSystemInfo,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode);
			IpcStartEvent(EVENT_GLOBAL_DVD_REGION_SET_ID);
		}
		else if (0x07 == buff[1])
		{
			if (!(pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode == 9 
				|| (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode > 0 && pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode <= 6)))
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode = 9;	
			}
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode--;
			if (0 == pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode)
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode = 9;
			}
			else if (8 == pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode)
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode = 6;
			}
			else
			{
			}
			returnDVDRegionCode(pSystemInfo,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode);
			IpcStartEvent(EVENT_GLOBAL_DVD_REGION_SET_ID);
		}
		break;
	case 0x10:

		//现在默认为常开
		//if (0x01 == buff[1])
		//{
			pSystemInfo->bBackDetectEnable = TRUE;
			buff[1] = TRUE;
		//}
		//else if (0x00 == buff[1])
		//{
		//	pSystemInfo->bBackDetectEnable = FALSE;
		//}
		ReturnToUser(buff,2);
		pSystemInfo->dwBackDetectTimer = GetTickCount();
		if (pSystemInfo->bPowerUp)
		{
			SetEvent(pSystemInfo->hDispatchMainThreadEvent);
		}

		
		//if (0x01 == buff[1])
		//{
		//	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable = TRUE;
		//}
		//else if (0x00 == buff[1])
		//{
		//	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable = FALSE;
		//}
		//IpcStartEvent(EVENT_GLOBAL_BACKDETECT_CHANGE_ID);
		//IpcStartEvent(EVENT_GLOBAL_BACK_LOW_VOLUME_ID);
		//controlToMCUBackDetectEnable(pSystemInfo,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable);
		//ReturnToUser(buff,2);
		break;
	case 0x11:
		if (0x01 == buff[1])
		{
			controlToMCULightDetectEnable(pSystemInfo,TRUE);
		}
		else if (0x00 == buff[1])
		{
			controlToMCULightDetectEnable(pSystemInfo,FALSE);
		}
		break;
	case 0x12:
		if (0x01 == buff[1])
		{
			controlToMCUbDayNight(pSystemInfo,TRUE);
		}
		else if (0x00 == buff[1])
		{
			controlToMCUbDayNight(pSystemInfo,FALSE);
		}
		break;
	case 0x13:
		if (0x01 == buff[1])
		{
			controlToMCUbHaveExtAMP(pSystemInfo,TRUE);
		} 
		else
		{
			controlToMCUbHaveExtAMP(pSystemInfo,FALSE);
		}
		break;
	case 0x14:
		if (buff[1] <= 3)
		{
			if (0 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightNight = 0;
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightDay = 0;
			}
			else
			{
				if (0 == pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightNight
					|| 0 == pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightDay)
				{
					pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightNight = buff[1];
					pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightDay = buff[1];
				}
			}
			if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDayNight)
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightNight = buff[1];
			}
			else
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iLCDLightBrightDay = buff[1];
			}
		}
		DBGD((TEXT("\r\n len:%d,buff[0]:%x==============="),len,buff[0]));
		ReturnToUser(buff,len);
		if (pSystemInfo->bPowerUp)
		{
			SetEvent(pSystemInfo->hDispatchMainThreadEvent);
		}
		break;
	case 0x15:
		controlToMCUPanelLight(pSystemInfo,buff[1]);
		break;
	case 0x16:
		if (0x01 == buff[1])
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSteerWheelOn = TRUE;
		}
		else
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSteerWheelOn = FALSE;
		}
		ReturnToUser(buff,len);
		if (pSystemInfo->bPowerUp)
		{
			SetEvent(pSystemInfo->hDispatchMainThreadEvent);
		}
		break;
	case 0x17:
		if (buff[1] > RADIO_COUNTRY_ID - 1)
		{
			buff[1] = 0;
		}
		if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser != buff[1])
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser = buff[1];
		}
		ReturnToUser(buff,len);
		SetEvent(pSystemInfo->hDispatchMainThreadEvent);
		break;
	case 0x18:
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecWinCEFactoryMsg = TRUE;
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecWinCERestartMsg = TRUE;
		if (pSystemInfo->bPowerUp)
		{
			SetEvent(pSystemInfo->hDispatchMainThreadEvent);
			LCD_LCDIdle_Off(pSystemInfo);//关背光
			LCD_LCDBright_PWM(pSystemInfo,LCD_OFF);
		}
		RETAILMSG(1, (TEXT("\r\nSystem WinCE Write Reset To Factory!!!!!!!!")));
		printfHowLongTime(pSystemInfo);
		break;
	case 0x19:
		if (0x01 == buff[1])
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bKeyDemoMode = TRUE;
			controlToMCUKeyDemoMode(pSystemInfo,TRUE);
		}
		else if (0x00 == buff[1])
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bKeyDemoMode = FALSE;
			controlToMCUKeyDemoMode(pSystemInfo,FALSE);
		}
		IpcStartEvent(EVENT_GLOBAL_DEMO_KEY_START_ID);
		break;
	case 0x1A:
		if (0x01 == buff[1])
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRemoteUseStudyOn = TRUE;
		} 
		else
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRemoteUseStudyOn = FALSE;
		}
		IpcStartEvent(EVENT_GLOBAL_REMOTE_USE_IT_ID);
		ReturnToUser(buff,len);
		break;
	case 0x1B:
		if (buff[1] < IRKEY_STUDY_COUNT)
		{
			if (0x00 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyID = KB_AV;
			}
			else if (0x01 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyID = KB_SEEK_INC;
			}
			else if (0x02 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyID = KB_SEEK_DEC;
			}
			else if (0x03 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyID = KB_MUTE;
			}
			else if (0x04 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyID = KB_VOL_INC;
			}
			else if (0x05 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyID = KB_VOL_DEC;
			}
			else if (0x06 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyID = KB_CALL_INOUT;
			}
			else if (0x07 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyID = KB_CALL_REJECT;
			}
			else if (0x08 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyID = KB_NAVI;
			}
			IpcStartEvent(EVENT_GLOBAL_REMOTE_STUDY_START_ID);
		}
		break;
	case 0x1C:
		IpcStartEvent(EVENT_GLOBAL_REMOTE_STUDY_STOP_ID);
		break;
	case 0x1D:
		if (buff[1] < IRKEY_STUDY_COUNT)
		{
			if (0x00 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyClearID[0x00] = KB_AV;
			}
			else if (0x01 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyClearID[0x01] = KB_SEEK_INC;
			}
			else if (0x02 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyClearID[0x02] = KB_SEEK_DEC;
			}
			else if (0x03 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyClearID[0x03] = KB_MUTE;
			}
			else if (0x04 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyClearID[0x04] = KB_VOL_INC;
			}
			else if (0x05 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyClearID[0x05] = KB_VOL_DEC;
			}
			else if (0x06 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyClearID[0x06] = KB_CALL_INOUT;
			}
			else if (0x07 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyClearID[0x07] = KB_CALL_REJECT;
			}
			else if (0x08 == buff[1])
			{
				pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyClearID[0x08] = KB_NAVI;
			}
			IpcStartEvent(EVENT_GLOBAL_REMOTE_STUDY_CLEAR_ID);
		}
		break;
	case 0x1E:
		if (!(buff[1] == 9 || (buff[1] > 0 && buff[1] <= 6)))
		{
			buff[1] = 9;
		}
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode = buff[1];
		returnDVDRegionCode(pSystemInfo,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode);
		IpcStartEvent(EVENT_GLOBAL_DVD_REGION_SET_ID);
		break;
	case 0x1F:
		LCD_LCDIdle_On(pSystemInfo);//开背光
		break;
	case 0x20:
		controlToMCUKeyAccOffMode(pSystemInfo,buff[1]);
		break;
	case 0x28:
		IpcStartEvent(EVENT_GLOBAL_AUX_CHECK_START_ID);
//#else 
//		IpcStartEvent(EVENT_GLOBAL_AUX_CHECK_START_ID);
//		returnSystembHaveAuxVideo(pSystemInfo,1);
//#endif 
		break;
	case 0x29:
		pSystemInfo->bRecRegeditRestoreMsg = TRUE;
		pSystemInfo->iRecRegeditRestoreWhat = buff[1];
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveReceiveResetMsg = TRUE;
		RETAILMSG(1, (TEXT("\r\nSystem WinCE Write Write Regedit!!!!!!!!")));
		printfHowLongTime(pSystemInfo);
		if (pSystemInfo->iRecRegeditRestoreWhat == 0x02)
		{
			pSystemInfo->bAccOffBreakRecvForUser =TRUE; 
		}
		SetEvent(pSystemInfo->hDispatchMainThreadEvent);
		break;
	case 0x2a:
		pSystemInfo->bNeedReserveSteelSelectData =TRUE;
		break;
	case 0x30:
		break;
	case 0x31:
		if (0x01 == buff[1])
		{
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecWinCESleepMsg = TRUE;
			pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhat = eLEDEnterSleepTimeOut;
			pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhatSub = 0;
			pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnTime = GetTickCount();
			if (pSystemInfo->bPowerUp)
			{
				SetEvent(pSystemInfo->hDispatchMainThreadEvent);
				LCD_LCDIdle_Off(pSystemInfo);//关背光
			}

			RETAILMSG(1, (TEXT("\r\nSystem WinCE Write Ready Power Off!!!!!!!!")));
			printfHowLongTime(pSystemInfo);
		} 
		else
		{
			RETAILMSG(1, (TEXT("\r\nSystem WinCE Write Prepare Power Off!!!!!!!!")));
			printfHowLongTime(pSystemInfo);
		}
		break;

	case 0x45:
		if (buff[1] > 3)
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGPSVolumeLevel = 3;
		else
			pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGPSVolumeLevel = buff[1];
		IpcStartEvent(EVENT_GLOBAL_GPS_VOLUME_LEVEL_ID);
		ReturnToUser(buff,len);
		break;

	case 0xFF:
		if (0x01 == buff[1])
		{
			FSY_PowerUp((DWORD)pSystemInfo);
		} 
		else if (0x00 == buff[1])
		{
			FSY_PowerDown((DWORD)pSystemInfo);
		}
		break;
	default:
		break;
	}
}

static DWORD WINAPI ThreadPowerUp(LPVOID pContext)
{
	P_FLY_SYSTEM_INFO	pSystemInfo = (P_FLY_SYSTEM_INFO)pContext;

	speedCheckInit(pSystemInfo);

	FlySystemPowerEnable(pSystemInfo,TRUE);
	HwPwmSetFreq(FAN_PWM, FAN_OFF);
	//LCD_PanelPen_PWM(pSystemInfo,KEY_ON);

	resetPowerUp(pSystemInfo);

	if (pSystemInfo->bOpen)
	{
		Sleep(618);
		if (!pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)
		{
			LCD_LCDIdle_On(pSystemInfo);//Mootall
		}
	}
	else
	{
		LCD_LCDIdle_On(pSystemInfo);
	}

	return 0;
}

static void powerNormalInit(P_FLY_SYSTEM_INFO pSystemInfo)
{
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bKeyDemoMode = FALSE;

	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedWinCEPowerOff = FALSE;

	//低功耗和待机的关系
	pSystemInfo->bStandbyStatusWithACCOff = FALSE;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatusWithACCOff = FALSE;

	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveReceiveResetMsg = FALSE;

	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus
		= pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatusLast;
	pSystemInfo->bStandbyStatus
		= pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus;

	pSystemInfo->buffToUserHx = 0;
	pSystemInfo->buffToUserLx = 0;

	pSystemInfo->SystemCarbodyInfo.iProcVoltageShakeDelayTime = 0;

	pSystemInfo->SystemCarbodyInfo.iBreakDetectDelayRead = 0;
	pSystemInfo->SystemCarbodyInfo.iPhoneDetectDelayRead = 0;

	pSystemInfo->SystemCarbodyInfo.iSendShellBabyTime = GetTickCount();

	pSystemInfo->SystemCarbodyInfo.iFanPWMDuty = FAN_OFF;

	pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhat = 0;
	pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnWhatSub = 0;
	pSystemInfo->SystemCarbodyInfo.iLEDBlinkOnTime = 0;

	pSystemInfo->bRecRegeditRestoreMsg = FALSE;
}

static void powerOnFirstInit(P_FLY_SYSTEM_INFO pSystemInfo)
{
	pSystemInfo->bOpen = FALSE;
	pSystemInfo->bPower = FALSE;
	pSystemInfo->bSpecialPower = FALSE;
	pSystemInfo->bPowerUp = FALSE;
	pSystemInfo->bUserPowerUp = FALSE;
	pSystemInfo->bNeedReserveSteelSelectData =FALSE;

	pSystemInfo->bKillDispatchFlyMainThread = TRUE;
	pSystemInfo->FlyMainThreadHandle = NULL;
	pSystemInfo->hDispatchMainThreadEvent = NULL;

	pSystemInfo->hMsgQueueToSystemCreate = NULL;
	pSystemInfo->hMsgQueueFromSystemCreate = NULL;
	pSystemInfo->bKillDispatchFlyMsgQueueReadThread = TRUE;
	pSystemInfo->FLyMsgQueueReadThreadHandle = NULL;

	pSystemInfo->bKillDispatchFlyInterThread = TRUE;
	pSystemInfo->FLyInterThreadHandle = NULL;
	pSystemInfo->hDispatchInterThreadEvent = NULL;

	pSystemInfo->bBreakDetectIsrRunning = FALSE;
	pSystemInfo->hBreakDetectIsrEvent = NULL;

	pSystemInfo->bPhoneDetectIsrRunning = FALSE;
	pSystemInfo->hPhoneDetectIsrEvent = NULL;

	pSystemInfo->bNeedSendBackStatusToUser = TRUE;
	pSystemInfo->SilencePowerOffInfo.bReadRegData = FALSE;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSilencePowerUp = FALSE;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActioning = TRUE;
	
	pSystemInfo->bAccOffBreakRecvForUser = FALSE;
	//pSystemInfo->bFrom3360Mute = TRUE;
}

static void powerOnSpecialEnable(P_FLY_SYSTEM_INFO pSystemInfo,BOOL bOn)
{
	DWORD dwThreadID;

	if (bOn)
	{
		if (pSystemInfo->bSpecialPower)
		{
			return;
		}
		pSystemInfo->bSpecialPower = TRUE;

		//给video.dll的信号，
		pSystemInfo->hToVideoSignalTestEvent = CreateEvent(NULL,FALSE,FALSE,TO_VIDEO_SIGNAL_TEST_EVENT_NAME);	

		//给素伟的信号量
		pSystemInfo->hToSuWeiPowerDownReqEvent = CreateEvent(NULL,FALSE,FALSE,SUWEI_POWERDOWN_REQ);	
		pSystemInfo->hToSuWeiPowerDownAckEvent = CreateEvent(NULL,FALSE,FALSE,SUWEI_POWERDOWN_ACK);	
		pSystemInfo->hToSuWeiPowerDownRecEvent = CreateEvent(NULL,FALSE,FALSE,SUWEI_POWERDOWN_REC);	

		pSystemInfo->hBuffToUserDataEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		InitializeCriticalSection(&pSystemInfo->hCSSendToUser);

		MSGQUEUEOPTIONS  msgOpts;

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_SYSTEM_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_SYSTEM_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pSystemInfo->hMsgQueueToSystemCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_SYSTEM_NAME, &msgOpts);
		if (NULL == pSystemInfo->hMsgQueueToSystemCreate)
		{
			DBGI((TEXT("\r\nFlyAudio System Create MsgQueue To System Fail!")));
		}

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_FROM_SYSTEM_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_FROM_SYSTEM_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pSystemInfo->hMsgQueueFromSystemCreate = CreateMsgQueue(FLY_MSG_QUEUE_FROM_SYSTEM_NAME, &msgOpts);
		if (NULL == pSystemInfo->hMsgQueueFromSystemCreate)
		{
			DBGI((TEXT("\r\nFlyAudio System Create MsgQueue From System Fail!")));
		}

		pSystemInfo->bKillDispatchFlyMsgQueueReadThread = FALSE;
		pSystemInfo->FLyMsgQueueReadThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadRead, //线程的全局函数
			pSystemInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGI((TEXT("\r\nSystem ThreadRead ID:%x"),dwThreadID));
		if (NULL == pSystemInfo->FLyMsgQueueReadThreadHandle)
		{
			pSystemInfo->bKillDispatchFlyMsgQueueReadThread = TRUE;
			return;
		}

		//3360判断有没有音频输出信号
		pSystemInfo->hFrom3360MuteEventHandle = OpenEvent(EVENT_ALL_ACCESS, FALSE,FROM_3360MUTE_EVENT); 
		pSystemInfo->hFrom3360MuteThreadHandle  = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MT3360MuteThreadProc, pSystemInfo, 0, &dwThreadID);
		if (NULL == pSystemInfo->hFrom3360MuteThreadHandle)
		{
			return;
		}
	} 
	else
	{
		if (!pSystemInfo->bSpecialPower)
		{
			return;
		}
		pSystemInfo->bSpecialPower = FALSE;

		CloseHandle(pSystemInfo->hBuffToUserDataEvent);

		pSystemInfo->bKillDispatchFlyMsgQueueReadThread = TRUE;
		while (pSystemInfo->FLyMsgQueueReadThreadHandle)
		{
			SetEvent(pSystemInfo->hMsgQueueToSystemCreate);
			Sleep(10);
		}
		CloseMsgQueue(pSystemInfo->hMsgQueueToSystemCreate);
		CloseMsgQueue(pSystemInfo->hMsgQueueFromSystemCreate);

		//给素伟的信号量
		CloseHandle(pSystemInfo->hToSuWeiPowerDownReqEvent);	
		CloseHandle(pSystemInfo->hToSuWeiPowerDownAckEvent);	
		CloseHandle(pSystemInfo->hToSuWeiPowerDownRecEvent);	
	}
}

void IpcRecv(UINT32 iEvent)
{
	P_FLY_SYSTEM_INFO	pSystemInfo = (P_FLY_SYSTEM_INFO)gpSystemInfo;
	DBGD((TEXT("\r\nFLY system Recv IPC iEvent:%d\r\n"),iEvent));

	switch (iEvent)
	{
	case EVENT_GLOBAL_AUX_CHECK_RETURN_ID:
		IpcClearEvent(EVENT_GLOBAL_AUX_CHECK_RETURN_ID);
		returnSystembHaveAuxVideo(pSystemInfo
			,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAUXHaveVideo);
		break;

	default:
		SetEvent(pSystemInfo->hDispatchMainThreadEvent);
		break;
	}
}
void SockRecv(BYTE *buf, UINT16 len)
{
	UINT16 i=0;
	P_FLY_SYSTEM_INFO	pSystemInfo = (P_FLY_SYSTEM_INFO)gpSystemInfo;
	
	DBGD((TEXT("\r\nFlyAudio system Read Socket:")));
	for (i=0; i<len; i++)
		DBGD((TEXT("%02X "),buf[i]));

	if (!pSystemInfo->bSocketConnecting)
		pSystemInfo->bSocketConnecting  = TRUE;
}


HANDLE
FSY_Init(DWORD dwContext)
{
	P_FLY_SYSTEM_INFO pSystemInfo;

	RETAILMSG(1, (TEXT("\r\nSystem Init Start")));


	pSystemInfo = (P_FLY_SYSTEM_INFO)LocalAlloc(LPTR, sizeof(FLY_SYSTEM_INFO));
	if (!pSystemInfo)
	{
		return NULL;
	}
	gpSystemInfo = pSystemInfo;

	pSystemInfo->pFlyShmGlobalInfo = CreateShm(SYSTEM_MODULE,IpcRecv);
	if (NULL == pSystemInfo->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio system Init create shm err\r\n")));
		return NULL;
	}

	if (!GetDllAddrTable())
	{
		DBGE((TEXT("FlyAudio system  GetDllAddrTable err\r\n")));
		return NULL;
	}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio system Open create user buf err\r\n")));
		return NULL;
	}

	if(!CreateServerSocket(SockRecv, TCP_PORT_SYSTEM))
	{
		DBGE((TEXT("FlyAudio system Open create server socket err\r\n")));
		return NULL;
	}

	if (pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalStructSize
		!= sizeof(struct shm))
	{
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[0] = 'F';
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[1] = 'S';
		pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[2] = 'Y';
	}

	powerNormalInit(pSystemInfo);
	powerOnFirstInit(pSystemInfo);
	powerOnSpecialEnable(pSystemInfo,TRUE);
	FlySystemEnable(pSystemInfo,TRUE);
	FlySystemPowerEnable(pSystemInfo,TRUE);

	speedCheckInit(pSystemInfo);

	//HwPwmSetFreq(FAN_PWM, FAN_OFF);
	//LCD_PanelPen_PWM(pSystemInfo,KEY_ON);
	//LCD_LCDIdle_On(pSystemInfo);
	//LCD_LCDBright_PWM(pSystemInfo,100);在线程中处理

	//参数初始化

#if GLOBAL_COMM
	//pSystemInfo->hHandleGlobalGlobalEvent = CreateEvent(NULL,FALSE,FALSE,DATA_GLOBAL_HANDLE_GLOBAL);
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.bInit = TRUE;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompYear = year;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompMon = months;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompDay = day;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompHour = hours;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompMin = minutes;
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompSec = seconds;

#endif

	RETAILMSG(1, (TEXT("\r\nFlyAudio System Init Build:Date%d.%d.%dTime%d:%d:%ddebugMsgLevel:%d")
		,year,months,day,hours,minutes,seconds
		,pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.debugMsgLevel));
	return (HANDLE)pSystemInfo;
}

BOOL
FSY_Deinit(DWORD hDeviceContext)
{
	P_FLY_SYSTEM_INFO	pSystemInfo = (P_FLY_SYSTEM_INFO)hDeviceContext;
	CloseHandle(pSystemInfo->hBuffToUserDataEvent);

	FlySystemPowerEnable(pSystemInfo,FALSE);
	FlySystemEnable(pSystemInfo,FALSE);
	powerOnSpecialEnable(pSystemInfo,FALSE);

#if GLOBAL_COMM
	//全局
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.bInit = FALSE;
	SetEvent(pSystemInfo->hHandleGlobalGlobalEvent);
	CloseHandle(pSystemInfo->hHandleGlobalGlobalEvent);
#endif

	RETAILMSG(1, (TEXT("\r\nFlyAudio System DeInit")));

	FreeShm();
	FreeSocketServer();
	FreeUserBuff();
	LocalFree(pSystemInfo);
	return TRUE;
}

DWORD
FSY_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	P_FLY_SYSTEM_INFO	pSystemInfo = (P_FLY_SYSTEM_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;

	if (pSystemInfo->bOpen)
	{
		return NULL;
	}
	pSystemInfo->bOpen = TRUE;
#if GLOBAL_COMM
	//全局
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.bOpen = TRUE;
	SetEvent(pSystemInfo->hHandleGlobalGlobalEvent);
#endif

	RETAILMSG(1, (TEXT("\r\nFlyAudio System Open Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return returnWhat;
}

BOOL
FSY_Close(DWORD hDeviceContext)
{
	P_FLY_SYSTEM_INFO	pSystemInfo = (P_FLY_SYSTEM_INFO)hDeviceContext;

#if GLOBAL_COMM
	//全局
	pSystemInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.bOpen = FALSE;
	SetEvent(pSystemInfo->hHandleGlobalGlobalEvent);
#endif

	SetEvent(pSystemInfo->hBuffToUserDataEvent);

	pSystemInfo->bOpen = FALSE;

	RETAILMSG(1, (TEXT("\r\nFlyAudio System Close")));
	return TRUE;
}

VOID
FSY_PowerUp(DWORD hDeviceContext)
{
	P_FLY_SYSTEM_INFO	pSystemInfo = (P_FLY_SYSTEM_INFO)hDeviceContext;
	DWORD dwThreadID;

	powerNormalInit(pSystemInfo);

	CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
		0,//初始化线程栈的大小，缺省为与主线程大小相同
		(LPTHREAD_START_ROUTINE)ThreadPowerUp, //线程的全局函数
		pSystemInfo, //此处传入了主框架的句柄
		0, &dwThreadID );
	DBGI((TEXT("\r\nSystem ThreadPowerUp ID:%x"),dwThreadID));

	SetEvent(pSystemInfo->hDispatchMainThreadEvent);

	RETAILMSG(1, (TEXT("\r\nFlyAudio System PowerUp")));
}

void prePowerDown(P_FLY_SYSTEM_INFO pSystemInfo)
{
	FlySystemPowerEnable(pSystemInfo,FALSE);
	//LCD_PanelPen_PWM(pSystemInfo,KEY_OFF);
}

VOID
FSY_PowerDown(DWORD hDeviceContext)
{
	P_FLY_SYSTEM_INFO	pSystemInfo = (P_FLY_SYSTEM_INFO)hDeviceContext;

	RETAILMSG(1, (TEXT("\r\nFlyAudio System PowerDown")));
}

DWORD
FSY_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	P_FLY_SYSTEM_INFO	pSystemInfo = (P_FLY_SYSTEM_INFO)hOpenContext;

	UINT dwRead = 0,i;
	BYTE *buf = (BYTE *)pBuffer;

	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio system return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}

	
	return dwRead;
}

DWORD
FSY_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	P_FLY_SYSTEM_INFO	pSystemInfo = (P_FLY_SYSTEM_INFO)hOpenContext;
	DBGI((TEXT("\r\nFlyAudio System Write %d bytes:"),NumberOfBytes));
	for(UINT i = 0;i < NumberOfBytes;i++)
	{
		DBGI((TEXT(" %X"),*((BYTE *)pSourceBytes + i)));
	}
	if(NumberOfBytes)
	{
		WinCECommandProcessor(pSystemInfo,(((BYTE *)pSourceBytes)+3),((BYTE *)pSourceBytes)[2]);
	}

	return NULL;
}

DWORD
FSY_Seek(DWORD hOpenContext, LONG Amount, DWORD  Type)
{
	return NULL;
}

BOOL
FSY_IOControl(DWORD hOpenContext,
			  DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn,
			  PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
	P_FLY_SYSTEM_INFO	pSystemInfo = (P_FLY_SYSTEM_INFO)hOpenContext;
	BOOL bRes = TRUE;

	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	DBGD((TEXT("\r\nFlyAudio System IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)"),
	//		hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	DBGD((TEXT("\r\nFlyAudio System IOControl Set IOCTL_SERIAL_SET_WAIT_MASK")));
	//		break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	//DBGD((TEXT("\r\nFlyAudio System IOControl Set IOCTL_SERIAL_WAIT_ON_MASK")));
	//	if (pSystemInfo->bOpen)
	//	{
	//		WaitForSingleObject(pSystemInfo->hBuffToUserDataEvent,INFINITE);
	//	}
	//	else
	//	{
	//		WaitForSingleObject(pSystemInfo->hBuffToUserDataEvent,0);
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
		DEBUGMSG(1, (TEXT("Attach in FlySystem DllEntry\n")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}

	if (dwReason == DLL_PROCESS_DETACH)
	{
		DEBUGMSG(1, (TEXT("Dettach in FlySystem DllEntry\n")));
	}

	DEBUGMSG(1, (TEXT("Leaving FlySystem DllEntry\n")));

	return (TRUE);
}
