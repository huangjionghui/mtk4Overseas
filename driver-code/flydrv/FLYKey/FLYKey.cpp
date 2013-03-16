// -----------------------------------------------------------------------------
// File Name    : FLYKey.cpp
// Title        : FlyKey Driver
// Author       : JQilin - Copyright (C) 2011
// Created      : 2011-02-25  
// Version      : 0.01
// Target MCU   : WinCE Driver
// -----------------------------------------------------------------------------
// Version History:
/*
>>>2011-05-19: V0.02 整理了WinCE待机相关的代码
>>>2011-04-25: V0.02 适应新的硬件
>>>2011-02-25: V0.01 first draft
*/

#include "flyhwlib.h"
#include "flyuserlib.h"
#include "flysocketlib.h"

#include "FLYKey.h"

P_FLY_KEY_INFO gpKeyInfo = NULL;

tPANEL_TAB DefaultPanelTab = {	  //6507B
	{0xaa,0x55},
	0x01,
	{'A','V','7','5','0','7','-','0'},
	0x00,
	{0x36,0x35,0x3b,0x3c},
	{
		KB_DIMM,0x03,0x38,0x39,0x02,0x37,0x34,
		0x10,0x11,0x12,0x13,0x3a,KB_DVD_OPEN,KB_NAVI,
		KB_DEST,KB_LOCAL,KB_SETUP,KB_TUNE_DOWN,0x00,0x00,0x00
	},
	800,	//LCD_RGB_Wide
	480,	//LCD_RGB_High
};

//tPANEL_TAB DefaultPanelTab = {	  //6525
//	{0xaa,0x55},
//	0x01,
//	{'A','V','7','5','2','5','-','0'},
//	0x00,
//	{0x35,0x36,0x3C,0x3B},
//	{
//		0x00,0x02,0x39,0x38,0x03,0x33,0x00,
//		0x00,0x00,0x00,0x00,0x00,0x37,0x13,
//		0x3A,0x11,0x10,0x34,0x00,0x00,0x00
//	},
//	800,	//LCD_RGB_Wide
//	480,	//LCD_RGB_High
//};

//tPANEL_TAB DefaultPanelTab = {	  //8016-A
//	{0xaa,0x55},
//	0x01,
//	{'A','V','8','0','1','6','-','A'},
//	0x00,
//	{0x35,0x36,0x3C,0x3B},
//	{
//		0x00,0x39,0x38,0x10,0x33,0x37,0x00,
//		0x00,0x00,0x00,0x00,0x00,0x00,0x13,
//		0x08,0x11,0x34,0x3A,0x00,0x00,0x00
//	},
//	800,	//LCD_RGB_Wide
//	480,	//LCD_RGB_High
//};

BYTE key_Tab_short[3][7] = { //0表示无键位置.
	{0x01,0x02,0x03,0x04,0x05,0x06,0x13},//ADC1
	{0x07,0x08,0x09,0x0a,0x0b,0x0c,0x14},//ADC2
	{0x0d,0x0e,0x0f,0x10,0x11,0x12,0x15}//ADC3
};

VOID FKY_PowerUp(DWORD hDeviceContext);
VOID FKY_PowerDown(DWORD hDeviceContext);
void prePowerDown(P_FLY_KEY_INFO pKeyInfo);

BOOL xxxxxxxxON;
void TestLEDBlink(P_FLY_KEY_INFO pKeyInfo)
{
	
	if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bOSDDemoMode
		|| pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.debugMsgLevel)
	{
		if (27 == pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume)
		{
			if (xxxxxxxxON)
			{
				//HwGpioOutput(CONTROL_IO_LED_I,EJECT_LED_ON);
				xxxxxxxxON = FALSE;
			}
			else
			{
				//HwGpioOutput(CONTROL_IO_LED_I,EJECT_LED_OFF);
				xxxxxxxxON = TRUE;
			}
		}
	}
}

UINT sTestKeyPassword;
static void testKeyPassword(P_FLY_KEY_INFO pKeyInfo)
{
	sTestKeyPassword *= 10;
	sTestKeyPassword += pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
	sTestKeyPassword %= 10000;
	RETAILMSG(1, (TEXT("\r\nFlyaudio Key Password%d"),sTestKeyPassword));
	if (9521 == sTestKeyPassword)
	{
		pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bOSDDemoMode = TRUE;
	}
	else if (9822 == sTestKeyPassword)
	{
		 IpcStartEvent(EVENT_GLOBAL_IIC_MCU_ID);
	}
	else if (9823 == sTestKeyPassword)
	{
		pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNoMoreToSendDataWhenToSleep = TRUE;
	}
	else if (9831 == sTestKeyPassword)
	{
		pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable = TRUE;

		//写到注册表里，方便下次使用,下次上电时FLYGlobal再从中读出来
		WriteRegValues(HKEY_LOCAL_MACHINE,L"Drivers\\BuiltIn\\FLYKEY",L"bDebugMsgEnable" ,
			(LPBYTE)&pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable,sizeof(int));
	}
	else if (9830 == sTestKeyPassword)
	{
		pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable = FALSE;

		//写到注册表里，方便下次使用，下次上电时FLYGlobal再从中读出来
		WriteRegValues(HKEY_LOCAL_MACHINE,L"Drivers\\BuiltIn\\FLYKEY",L"bDebugMsgEnable" ,
			(LPBYTE)&pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable,sizeof(int));
	}
}

ULONG xxxxxxxxLightOnTime = 0;
UINT xxxxxxxxLightOnCount = 0;
static void xxxxxxxxTestKey(P_FLY_KEY_INFO pKeyInfo,BYTE iID)
{
	if (KB_DVD_OPEN == iID)
	{
		if (GetTickCount() - xxxxxxxxLightOnTime < 2000)
		{
			xxxxxxxxLightOnTime = GetTickCount();
			xxxxxxxxLightOnCount++;
		}
		else
		{
			xxxxxxxxLightOnTime = GetTickCount();
			xxxxxxxxLightOnCount = 0;
		}
		if (xxxxxxxxLightOnCount >= 3)
		{
			 IpcStartEvent(EVENT_GLOBAL_FORCE_LCD_LIGHT_ON_ID);
			xxxxxxxxLightOnCount = 0;
		}

		//HwGpioOutput(CONTROL_IO_LCDIDLE_I,1);
		RETAILMSG(1, (TEXT("\r\nFlyaudio Key Test For Open LCDLight")));
	}
	else if (KB_TUNE_DOWN_LONG == iID)
	{
		//给素伟的信号量
		//SetEvent(pKeyInfo->hToSuWeiKeyTestEvent);
		RETAILMSG(1, (TEXT("\r\nFlyaudio Key Test To SuWei")));
	}
}


void EncoderIDExchange(P_FLY_KEY_INFO pKeyInfo,BYTE index)
{

	//DBGD((TEXT("\r\nkey irq:%02X"),index));

	if (index >= 0x80 && index <= 0x83)
	{
		pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iKeyIndex = index;
		pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iKeyValue = pKeyInfo->tPanelTab.EK_Value[index];
		if (KB_VOL_INC == pKeyInfo->tPanelTab.EK_Value[index - 0x80])
		{
			pKeyInfo->FlyKeyEncoderInfo.iEncoderLeftIncCount++;
			pKeyInfo->FlyKeyEncoderInfo.iEncoderLeftDecCount = 0;
		}
		else if (KB_VOL_DEC == pKeyInfo->tPanelTab.EK_Value[index - 0x80])
		{
			pKeyInfo->FlyKeyEncoderInfo.iEncoderLeftDecCount++;
			pKeyInfo->FlyKeyEncoderInfo.iEncoderLeftIncCount = 0;
		}
		else if (KB_TUNE_INC == pKeyInfo->tPanelTab.EK_Value[index - 0x80])
		{
			pKeyInfo->FlyKeyEncoderInfo.iEncoderRightIncCount++;
			pKeyInfo->FlyKeyEncoderInfo.iEncoderRightDecCount = 0;
		}
		else if (KB_TUNE_DEC == pKeyInfo->tPanelTab.EK_Value[index - 0x80])
		{
			pKeyInfo->FlyKeyEncoderInfo.iEncoderRightDecCount++;
			pKeyInfo->FlyKeyEncoderInfo.iEncoderRightIncCount = 0;
		}

		SetEvent(pKeyInfo->FlyKeyEncoderInfo.hDispatchThreadEncoderEvent);
	}
}

void EncoderL1_io_isr(BOOL bRValue, BOOL bLValue)	//左左
{
	P_FLY_KEY_INFO pKeyInfo = gpKeyInfo;

	pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft = pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft << 4;

	if (bRValue)
		pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft |= (1 << 2);

	if (bLValue)
		pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft |= (1 << 0);

	DBGD((TEXT("\r\n@L1:%X"),pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft));

	if (pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft    == 0x14 
		|| pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft == 0x44
		|| pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft == 0x04) 
	{
		EncoderIDExchange(pKeyInfo,0x81);//音量加
	}
	else if (pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft == 0x11
		|| pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft    == 0x41
		|| pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft    == 0x01)
	{
		EncoderIDExchange(pKeyInfo,0x80);
	}
}

void EncoderL2_io_isr(BOOL bRValue, BOOL bLValue)//左右
{
	P_FLY_KEY_INFO pKeyInfo = gpKeyInfo;

	//pKeyInfo->FlyKeyEncoderInfo.bInterruptEventL2Running = TRUE;

	pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft = pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft << 4;

	if (bRValue)
		pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft |= (1 << 2);

	if (bLValue)
		pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft |= (1 << 0);


	DBGD((TEXT("\r\n@L2:%X"),pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft));

	if (pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft    == 0x14 
		|| pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft == 0x44
		|| pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft == 0x04) 
	{
		EncoderIDExchange(pKeyInfo,0x81);//音量加
	}
	else if (pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft == 0x11
		|| pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft    == 0x41
		|| pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft    == 0x01)
	{
		EncoderIDExchange(pKeyInfo,0x80);
	}
}
void EncoderR1_io_isr(BOOL bRValue, BOOL bLValue)//右左
{
	P_FLY_KEY_INFO pKeyInfo = gpKeyInfo;

	//pKeyInfo->FlyKeyEncoderInfo.bInterruptEventR1Running = TRUE;
	
	pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight = pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight << 4;

	if (bRValue)
		pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight |= (1 << 2);

	if (bLValue)
		pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight |= (1 << 0);

	DBGD((TEXT("\r\n@R1:%X"),pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight));


	if (pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight    == 0x14 
		|| pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight == 0x44
		|| pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight == 0x04) 
	{
		EncoderIDExchange(pKeyInfo,0x82);//加
	}
	else if (pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight == 0x11
		|| pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight    == 0x41
		|| pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight    == 0x01)
	{
		EncoderIDExchange(pKeyInfo,0x83);
	}

	TestLEDBlink(pKeyInfo);
}
void EncoderR2_io_isr(BOOL bRValue, BOOL bLValue)//右右
{
	P_FLY_KEY_INFO pKeyInfo = gpKeyInfo;

	//pKeyInfo->FlyKeyEncoderInfo.bInterruptEventR2Running = TRUE;

	pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight = pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight << 4;

	if (bRValue)
		pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight |= (1 << 2);

	if (bLValue)
		pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight |= (1 << 0);

	DBGD((TEXT("\r\n@R2:%X"),pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight));

	if (pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight    == 0x14 
		|| pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight == 0x44
		|| pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight == 0x04) 
	{
		EncoderIDExchange(pKeyInfo,0x82);//加
	}
	else if (pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight == 0x11
		|| pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight    == 0x41
		|| pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight    == 0x01)
	{
		EncoderIDExchange(pKeyInfo,0x83);
	}

	TestLEDBlink(pKeyInfo);
}






static VOID FlyKeyPutToBuff(P_FLY_KEY_INFO pKeyInfo,BYTE data)
{
	pKeyInfo->buffToUser[pKeyInfo->buffToUserHx++] = data;
	if (pKeyInfo->buffToUserHx >= DATA_BUFF_LENGTH)
	{
		pKeyInfo->buffToUserHx = 0;
	}
}
void writeKeyDataToMcu(P_FLY_KEY_INFO pKeyInfo,BYTE *p,UINT length)
{
	UINT i;
	if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.bInit)
	{
		if(FALSE == WriteMsgQueue(pKeyInfo->hMsgQueueFromKeyCreate, p, length, 0, 0))
		{
			DWORD dwError = GetLastError();
			DBGE((TEXT("\r\nKey Write MsgQueue To CarBody Error!0x%x"),dwError));			 
		}
	}
	else
	{
		DBGE((TEXT("\r\nKey Write MsgQueue To CarBody Not Init")));
			for (i = 0;i < length;i++)
			{
				DBGE((TEXT(" %x"),p[i]));
			}
	}
}
//void FlyKeyReturnToUser(P_FLY_KEY_INFO pKeyInfo,BYTE *buf,UINT len)
void FlyKeyReturnToUser(P_FLY_KEY_INFO pKeyInfo,BYTE *buf,UINT len,BOOL bRomoteOrPanel)
{
	ULONG iTimeNow;
	
	//DBGD((TEXT("\r\nFlyaudio Key Value--------------%x"),buf[0]));
		
	if(pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveReceiveResetMsg)
	{
		return;
	}
	if(pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName == CAR_MAZDA_6)//马六特殊
	{
	//	RETAILMSG(1, (TEXT("\r\nMAZDA")));
		if(bRomoteOrPanel)
		{
	//		RETAILMSG(1, (TEXT("\r\nMAZDA%x"),buf[0]));
			writeKeyDataToMcu(pKeyInfo,buf,1);
			return;
		}
	}
	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iKeyCount++;
	xxxxxxxxTestKey(pKeyInfo,buf[0]);
	if (KB_MENU == buf[0])
	{
		testKeyPassword(pKeyInfo);
	}
	iTimeNow = GetTickCount();

	if (KB_SLEEP == buf[0])
	{
		if (!pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedWinCEPowerOff
			&& !pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
		{
			 DBGD((TEXT("\r\nFlyaudio Key Standby Key turn Down")));
			 IpcStartEvent(EVENT_GLOBAL_KEY_STANDBY_ID);
		}
		return;
	}

	if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)
	{
		return;
	}

	if (KB_AV == buf[0])
	{
		if (iTimeNow - pKeyInfo->iKeyAVFilter < 1000)
		{
			return;
		}
		pKeyInfo->iKeyAVFilter = iTimeNow;
	}
	else if (KB_VOL_INC == buf[0])
	{
		if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCheckShellBabyError)
		{
			return;
		}
		if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable
			&& pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
		{
			return;
		}
		if(pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp)
		{
			 IpcStartEvent(EVENT_GLOBAL_CAR_VOL_UP);
			return;
		}
		if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute)
		{
			pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
			if (0 == pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume)
			{
				pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume++;
			}
		}
		else if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume < pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax)
		{
			pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume++;
		}
		else
		{
			pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume = pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax;
		}
		if (0 == pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume)
		{
			pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
		}
		
		IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);
		//DBGD((TEXT("\r\nFlyaudio Key ===================  Vol+ %d"),pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume));
		return;
	} 
	else if (KB_VOL_DEC == buf[0])
	{
		if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable
			&& pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
		{
			return;
		}
		if(pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp)
		{
			 IpcStartEvent(EVENT_GLOBAL_CAR_VOL_DOWN);
			return;
		}
		if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute)
		{
			pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
		}
		else if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume > pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMin)
		{
			pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume--;
		}
		else
		{
			pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume = pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMin;	
		}
		if (0 == pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume)
		{
			pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
		}
		 IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);
		//DBGD((TEXT("\r\nFlyaudio Key =================== Vol- %d"),pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume));
		return;
	}
	else if (KB_MUTE == buf[0])
	{
		if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable
			&& pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
		{
			return;
		}
		if(pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp)
		{
			 IpcStartEvent(EVENT_GLOBAL_CAR_VOL_MUTE);
			return;
		}
		pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = !pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute;
		if (0 == pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume)
		{
			pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
		}
		 IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);
		DBGD((TEXT("\r\nFlyaudio Key Mute %d"),pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute));
		return;
	}


	if(pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName == CAR_FOCUS)
	{
		if(buf[0] == KB_SEEK_DEC)
		{
			if(pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus != 0)
			{
				if(pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus == 1)//来电中
				{
					buf[0] = KB_CALL_INOUT;
				}
				else
				{
					return;
				}
			}
		}
		else if(buf[0] == KB_SEEK_INC)
		{
			if(pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus != 0)
			{
	
				buf[0] = KB_CALL_REJECT;
			}
		}
	}


	if (KB_CALL_INOUT == buf[0])
	{
		if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus != 0x01)
		{//去电和挂断和接听中
			if (0x00 == pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus)
				buf[0] = KB_CALL_OUT;
			else
				buf[0] = KB_CALL_REJECT;
		}
		else
		{//来电
			buf[0] = KB_CALL_INOUT;
		}
	}

	BYTE buff[2];
	buff[0] = 0;
	buff[1] = buf[0];

	ReturnToUser(buff,2);
}


static DWORD WINAPI ThreadFlyKeyRead(LPVOID pContext)
{
	P_FLY_KEY_INFO pKeyInfo = (P_FLY_KEY_INFO)pContext;
	DWORD dwRes;
	DWORD dwMsgFlag;
	BYTE read_buff[FLY_MSG_QUEUE_TO_KEY_LENGTH];

	while (!pKeyInfo->bKillDispatchKeyReadThread)
	{
		WaitForSingleObject(pKeyInfo->hMsgQueueToKeyCreate,INFINITE);

		ReadMsgQueue(pKeyInfo->hMsgQueueToKeyCreate, read_buff, FLY_MSG_QUEUE_TO_KEY_LENGTH, &dwRes, 0, &dwMsgFlag);
		DBGD((TEXT("\r\nFlyKey Read From LPC%x"),read_buff[0]));
		if (pKeyInfo->bOpen)
		{
			FlyKeyReturnToUser(pKeyInfo,read_buff,1,FALSE);
		}
	}

	pKeyInfo->FlyKeyReadThreadHandle = NULL;
	DBGD((TEXT("\r\nFlyKey ThreadFlyKeyRead exit")));
	return 0;
}

static DWORD WINAPI ThreadFlyKeyEncoder(LPVOID pContext)
{
	P_FLY_KEY_INFO pKeyInfo = (P_FLY_KEY_INFO)pContext;
	ULONG WaitReturn;
	BYTE i;
	BYTE buff[2];

	while (!pKeyInfo->bKillDispatchKeyEncoderThread)
	{
		if (pKeyInfo->FlyKeyEncoderInfo.iEncoderLeftIncCount
			|| pKeyInfo->FlyKeyEncoderInfo.iEncoderLeftDecCount
			|| pKeyInfo->FlyKeyEncoderInfo.iEncoderRightIncCount
			|| pKeyInfo->FlyKeyEncoderInfo.iEncoderRightDecCount)
		{
			WaitReturn = WaitForSingleObject(pKeyInfo->FlyKeyEncoderInfo.hDispatchThreadEncoderEvent, 3000);
			if (WAIT_TIMEOUT == WaitReturn)
			{
				DBGD((TEXT("\r\nFlyKey TimeOut Clear")));
				pKeyInfo->FlyKeyEncoderInfo.iEncoderLeftIncCount = 0;
				pKeyInfo->FlyKeyEncoderInfo.iEncoderLeftDecCount = 0;
				pKeyInfo->FlyKeyEncoderInfo.iEncoderRightIncCount = 0;
				pKeyInfo->FlyKeyEncoderInfo.iEncoderRightDecCount = 0;
			}
		}
		else
		{
			WaitReturn = WaitForSingleObject(pKeyInfo->FlyKeyEncoderInfo.hDispatchThreadEncoderEvent, INFINITE);
		}

		if (IpcWhatEvent(EVENT_GLOBAL_REG_RESTORE_KEY_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_REG_RESTORE_KEY_ID);
			if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditKey)
			{
				pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditKey = FALSE;
				if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRestoreRegeditToFactory)
				{
					pKeyInfo->remoteStudyStart = FALSE;
					for (i = 0;i < IRKEYTABSIZE;i++)
					{
						pKeyInfo->remoteStudyTab.Value[i] = 0;
					}
					SteelwheelStudyRegDataWrite(pKeyInfo);
					DBGD((TEXT("\r\nFlyAudio Key Steel Study Clear All")));
				}
			}
		}

		if (IpcWhatEvent(EVENT_GLOBAL_SLEEP_PROC_KEY_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_SLEEP_PROC_KEY_ID);
			if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeKeySleep
				!= pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeKeySleep)
			{
				prePowerDown(pKeyInfo);
				pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeKeySleep
					= pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeKeySleep;
			}
		}

		if (IpcWhatEvent(EVENT_GLOBAL_DEMO_KEY_START_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_DEMO_KEY_START_ID);
			if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bKeyDemoMode)
			{
				pKeyInfo->iKeyDemoState = 0;
				pKeyInfo->iKeyDemoInnerTime = GetTickCount();
			}
			else
			{
				pKeyInfo->iKeyDemoState = 0;
				pKeyInfo->iKeyDemoInnerTime = 0;
			}
		}

		//////////////////////////////面板按键部分
		if (IpcWhatEvent(EVENT_GLOBAL_PANELKEY_USE_IT_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_PANELKEY_USE_IT_ID);
			panelKeyTabInit(pKeyInfo);
		}

		//////////////////////////////方向盘部分
		if (IpcWhatEvent(EVENT_GLOBAL_REMOTE_STUDY_PRE_RETURN_FINISH_ID))
		{
			DBGD((TEXT("\r\nFlyAudio Key Steel Study Pre Finish")));
			IpcClearEvent(EVENT_GLOBAL_REMOTE_STUDY_PRE_RETURN_FINISH_ID);
			SteelwheelStudyRegDataWrite(pKeyInfo);
			 IpcStartEvent(EVENT_GLOBAL_REMOTE_STUDY_RETURN_FINISH_ID);
		}
		if (IpcWhatEvent(EVENT_GLOBAL_REMOTE_STUDY_PRE_RETURN_WAIT_ID))
		{
			DBGD((TEXT("\r\nFlyAudio Key Steel Study Pre Wait")));
			IpcClearEvent(EVENT_GLOBAL_REMOTE_STUDY_PRE_RETURN_WAIT_ID);
			 IpcStartEvent(EVENT_GLOBAL_REMOTE_STUDY_RETURN_WAIT_ID);
		}
		if (IpcWhatEvent(EVENT_GLOBAL_REMOTE_STUDY_START_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_REMOTE_STUDY_START_ID);
			pKeyInfo->remoteStudyStart = TRUE;
			pKeyInfo->remoteStudyStep = 0;
			pKeyInfo->remoteStudyCount = 0;
			DBGD((TEXT("\r\nFlyAudio Key Steel Study Start")));
		}
		if (IpcWhatEvent(EVENT_GLOBAL_REMOTE_STUDY_STOP_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_REMOTE_STUDY_STOP_ID);
			pKeyInfo->remoteStudyStart = FALSE;
			 IpcStartEvent(EVENT_GLOBAL_REMOTE_STUDY_RETURN_WAIT_ID);
			remoteKeyInit(pKeyInfo);
			DBGD((TEXT("\r\nFlyAudio Key Steel Study Stop")));
		}
		if (IpcWhatEvent(EVENT_GLOBAL_REMOTE_STUDY_CLEAR_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_REMOTE_STUDY_CLEAR_ID);
			pKeyInfo->remoteStudyStart = FALSE;
			for (i = 0;i < IRKEY_STUDY_COUNT;i++)
			{
				if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyClearID[i])
				{
					pKeyInfo->remoteStudyTab.Value[pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyClearID[i]] = 0;
					pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyClearID[i] = 0;
				}
			}
			SteelwheelStudyRegDataWrite(pKeyInfo);
			 IpcStartEvent(EVENT_GLOBAL_REMOTE_STUDY_RETURN_WAIT_ID);
			remoteKeyInit(pKeyInfo);
			DBGD((TEXT("\r\nFlyAudio Key Steel Study Clear")));
		}
		if (IpcWhatEvent(EVENT_GLOBAL_REMOTE_USE_IT_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_REMOTE_USE_IT_ID);
			remoteKeyInit(pKeyInfo);
		}

		while (pKeyInfo->FlyKeyEncoderInfo.iEncoderLeftIncCount >= 2)
		{
			pKeyInfo->FlyKeyEncoderInfo.iEncoderLeftIncCount--;
			buff[0] = KB_VOL_INC;
			FlyKeyReturnToUser(pKeyInfo,buff,1,FALSE);
			DBGD((TEXT("LW")));
		}
		while (pKeyInfo->FlyKeyEncoderInfo.iEncoderLeftDecCount >= 2)
		{
			pKeyInfo->FlyKeyEncoderInfo.iEncoderLeftDecCount--;
			buff[0] = KB_VOL_DEC;
			FlyKeyReturnToUser(pKeyInfo,buff,1,FALSE);	
			DBGD((TEXT("LM")));
		}

		//这个旋扭有手感，旋动一格要有变化
		while (pKeyInfo->FlyKeyEncoderInfo.iEncoderRightIncCount >= 1)
		{
			pKeyInfo->FlyKeyEncoderInfo.iEncoderRightIncCount--;
			buff[0] = KB_TUNE_INC;
			FlyKeyReturnToUser(pKeyInfo,buff,1,FALSE);	
			DBGD((TEXT("RW")));
		}
		while (pKeyInfo->FlyKeyEncoderInfo.iEncoderRightDecCount >= 1)
		{
			pKeyInfo->FlyKeyEncoderInfo.iEncoderRightDecCount--;
			buff[0] = KB_TUNE_DEC;
			FlyKeyReturnToUser(pKeyInfo,buff,1,FALSE);	
			DBGD((TEXT("RM")));
		}
	}
	pKeyInfo->FlyKeyEncoderThreadHandle = NULL;
	DBGD((TEXT("\r\nFlyKey ThreadFlyKeyEncoder exit")));
	return 0;
}

void KeyThreadEnable(P_FLY_KEY_INFO pKeyInfo,BOOL bEnable)
{
	DWORD dwThreadID;

	if (bEnable)
	{
		pKeyInfo->bKillDispatchKeyEncoderThread = FALSE;
		pKeyInfo->FlyKeyEncoderInfo.hDispatchThreadEncoderEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
		pKeyInfo->FlyKeyEncoderThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadFlyKeyEncoder, //线程的全局函数
			pKeyInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\nFlyAudio ThreadFlyKeyEncoder ID:%x"),dwThreadID));

		if (NULL == pKeyInfo->FlyKeyEncoderThreadHandle)
		{
			pKeyInfo->bKillDispatchKeyEncoderThread = TRUE;
			return;
		}
		CeSetThreadPriority(pKeyInfo->FlyKeyEncoderThreadHandle, 50);


		pKeyInfo->bKillDispatchKeyADCThread = FALSE;
		pKeyInfo->hDispatchThreadADCEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
		pKeyInfo->FlyKeyADCThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadFlyKeyADC, //线程的全局函数
			pKeyInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\nFlyAudio ThreadFlyKeyADC ID:%x"),dwThreadID));

		if (NULL == pKeyInfo->FlyKeyADCThreadHandle)
		{
			pKeyInfo->bKillDispatchKeyADCThread = TRUE;
			return;
		}
	}
	else
	{
		pKeyInfo->bKillDispatchKeyEncoderThread = TRUE;
		while (pKeyInfo->FlyKeyEncoderThreadHandle)
		{
			SetEvent(pKeyInfo->FlyKeyEncoderInfo.hDispatchThreadEncoderEvent);
			Sleep(10);
		}
		CloseHandle(pKeyInfo->FlyKeyEncoderInfo.hDispatchThreadEncoderEvent);
		pKeyInfo->bKillDispatchKeyADCThread = TRUE;
		while (pKeyInfo->FlyKeyADCThreadHandle)
		{
			SetEvent(pKeyInfo->hDispatchThreadADCEvent);
			Sleep(10);
		}
		CloseHandle(pKeyInfo->hDispatchThreadADCEvent);
	}
}

void Encoder_isr_Enable(P_FLY_KEY_INFO pKeyInfo,BOOL bEnable)
{
	if (bEnable)
	{
		pKeyInfo->FlyKeyEncoderInfo.curEncodeValueLeft = 0;
		pKeyInfo->FlyKeyEncoderInfo.curEncodeValueRight = 0;

		pKeyInfo->FlyKeyEncoderInfo.iEncoderLeftDecCount = 0;
		pKeyInfo->FlyKeyEncoderInfo.iEncoderLeftIncCount = 0;

		//这个旋扭有手感，旋动一格要有变化
		pKeyInfo->FlyKeyEncoderInfo.iEncoderRightDecCount = 1;
		pKeyInfo->FlyKeyEncoderInfo.iEncoderRightIncCount = 1;

		HwKeyIrqEnable(EncoderR2_io_isr,EncoderR1_io_isr,
			EncoderL2_io_isr,EncoderL1_io_isr,TRUE);

	}
	else
	{
		HwKeyIrqEnable(EncoderR2_io_isr,EncoderR1_io_isr,
			EncoderL2_io_isr,EncoderL1_io_isr,FALSE);
	}
}

static void powerNormalInit(P_FLY_KEY_INFO pKeyInfo)
{
	pKeyInfo->buffToUserHx = 0;
	pKeyInfo->buffToUserLx = 0;

	pKeyInfo->iKeyAVFilter = GetTickCount();

	pKeyInfo->iMutePressTime = 0;
}

static void powerOnFirstInit(P_FLY_KEY_INFO pKeyInfo)
{
	pKeyInfo->bOpen = FALSE;
	pKeyInfo->bSpecialPower = FALSE;

	memset(&pKeyInfo->FlyKeyEncoderInfo,0,sizeof(FLY_KEY_ENCODER_INFO));

	pKeyInfo->bKillDispatchKeyEncoderThread = TRUE;
	pKeyInfo->FlyKeyEncoderThreadHandle = NULL;

	pKeyInfo->bKillDispatchKeyADCThread = TRUE;
	pKeyInfo->FlyKeyADCThreadHandle = NULL;
	pKeyInfo->hDispatchThreadADCEvent = NULL;

	memset(&pKeyInfo->tPanelTab,0,sizeof(tPANEL_TAB));

	remoteKeyInit(pKeyInfo);
}

static void powerOnSpecialEnable(P_FLY_KEY_INFO pKeyInfo,BOOL bOn)
{
	DWORD dwThreadID;

	if (bOn)
	{
		if (pKeyInfo->bSpecialPower)
		{
			return;
		}
		pKeyInfo->bSpecialPower = TRUE;

		//给素伟的信号量
		//pKeyInfo->hToSuWeiKeyTestEvent = CreateEvent(NULL,FALSE,FALSE,SUWEI_KEY_TEST);	

		pKeyInfo->hBuffToUserDataEvent = CreateEvent(NULL,FALSE,FALSE,NULL);	
		InitializeCriticalSection(&pKeyInfo->hCSSendToUser);

		MSGQUEUEOPTIONS  msgOpts;

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_FROM_KEY_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_FROM_KEY_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pKeyInfo->hMsgQueueFromKeyCreate = CreateMsgQueue(FLY_MSG_QUEUE_FROM_KEY_NAME, &msgOpts);
		if (NULL == pKeyInfo->hMsgQueueFromKeyCreate)
		{
			RETAILMSG(1, (TEXT("\r\nCarbody Create MsgQueue From Key Fail!")));
		}

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_KEY_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_KEY_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pKeyInfo->hMsgQueueToKeyCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_KEY_NAME, &msgOpts);
		if (NULL == pKeyInfo->hMsgQueueToKeyCreate)
		{
			DBGE((TEXT("\r\nFlyAudio Key Create MsgQueue To Key Fail!")));
		}

		pKeyInfo->bKillDispatchKeyReadThread = FALSE;
		pKeyInfo->FlyKeyReadThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadFlyKeyRead, //线程的全局函数
			pKeyInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		if (NULL == pKeyInfo->FlyKeyReadThreadHandle)
		{
			pKeyInfo->bKillDispatchKeyReadThread = TRUE;
			return;
		}
		DBGE((TEXT("\r\nFlyAudio ThreadFlyKeyRead ID %x!"),dwThreadID));
	}
	else
	{
		if (!pKeyInfo->bSpecialPower)
		{
			return;
		}
		pKeyInfo->bSpecialPower = FALSE;

		pKeyInfo->bKillDispatchKeyReadThread = TRUE;


		while (pKeyInfo->FlyKeyReadThreadHandle)
		{
			SetEvent(pKeyInfo->hMsgQueueToKeyCreate);
			Sleep(10);
		}
		CloseMsgQueue(pKeyInfo->hMsgQueueToKeyCreate);

		CloseMsgQueue(pKeyInfo->hMsgQueueFromKeyCreate);

		CloseHandle(pKeyInfo->hBuffToUserDataEvent);

		//给素伟的信号量
		//CloseHandle(pKeyInfo->hToSuWeiKeyTestEvent);	
	}
}

void panelKeyTabInit(P_FLY_KEY_INFO pKeyInfo)
{
	if ((pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.valid[0] == 0x55 
		&& pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.valid[1] == 0xAA)
		||(pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.valid[0] == 0xAA 
		&& pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab.valid[1] == 0x55))
	{
		memcpy((void*)&pKeyInfo->tPanelTab,(void*)&pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.tPanelTab,sizeof(tPANEL_TAB));
		RETAILMSG(1, (TEXT("\r\nFlyAudio Key Using Update Panel Data")));
	}
	else
	{
		memcpy(&pKeyInfo->tPanelTab,&DefaultPanelTab,sizeof(tPANEL_TAB));
		RETAILMSG(1, (TEXT("\r\nFlyAudio Key Using Default Panel Data")));
	}
}

void IpcRecv(UINT32 iEvent)
{
	P_FLY_KEY_INFO	pKeyInfo = (P_FLY_KEY_INFO)gpKeyInfo;

	DBGD((TEXT("\r\nFLY key Recv IPC iEvent:%d\r\n"),iEvent));

	SetEvent(pKeyInfo->FlyKeyEncoderInfo.hDispatchThreadEncoderEvent);
}
void SockRecv(BYTE *buf, UINT16 len)
{
	P_FLY_KEY_INFO	pKeyInfo = (P_FLY_KEY_INFO)gpKeyInfo;
}

HANDLE
FKY_Init(DWORD dwContext)
{
	P_FLY_KEY_INFO pKeyInfo;

	RETAILMSG(1, (TEXT("\r\nKey Init Start")));

	pKeyInfo = (P_FLY_KEY_INFO)LocalAlloc(LPTR, sizeof(FLY_KEY_INFO));
	if (!pKeyInfo)
	{
		DBGE((TEXT("FlyAudio key Init LocalAloc err\r\n")));
		return NULL;
	}
	gpKeyInfo = pKeyInfo;

	pKeyInfo->pFlyShmGlobalInfo = CreateShm(KEY_MODULE,IpcRecv);
	if (NULL == pKeyInfo->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio key Init create shm err\r\n")));
		return NULL;
	}

	if (!GetDllAddrTable())
	{
		DBGE((TEXT("FlyAudio key  GetDllAddrTable err\r\n")));
		return NULL;
	}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio key Open create user buf err\r\n")));
		return NULL;
	}

	if(!CreateServerSocket(SockRecv, TCP_PORT_KEY))
	{
		DBGE((TEXT("FlyAudio key Open create server socket err\r\n")));
		return NULL;
	}

	powerNormalInit(pKeyInfo);
	powerOnFirstInit(pKeyInfo);
	powerOnSpecialEnable(pKeyInfo,TRUE);

#if GLOBAL_COMM
	//pKeyInfo->hHandleGlobalGlobalEvent = CreateEvent(NULL,FALSE,FALSE,DATA_GLOBAL_HANDLE_GLOBAL);
	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.bInit = TRUE;
	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.iDriverCompYear = year;
	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.iDriverCompMon = months;
	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.iDriverCompDay = day;
	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.iDriverCompHour = hours;
	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.iDriverCompMin = minutes;
	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.iDriverCompSec = seconds;
	

	panelKeyTabInit(pKeyInfo);

	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iKeyCount = 0;
#endif


	pKeyInfo->CurrentAD[0] = 255;
	pKeyInfo->CurrentAD[1] = 255;
	pKeyInfo->CurrentAD[2] = 255;
	pKeyInfo->CurrentAD[3] = 1023;
	pKeyInfo->CurrentAD[4] = 1023;

	RETAILMSG(1,(TEXT("\r\nFlyAudio Key Init Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return (HANDLE)pKeyInfo;
}

BOOL
FKY_Deinit(DWORD hDeviceContext)
{
	P_FLY_KEY_INFO	pKeyInfo = (P_FLY_KEY_INFO)hDeviceContext;
	DBGE((TEXT("\r\nFlyAudio Key DeInit")));

	powerOnSpecialEnable(pKeyInfo,FALSE);

	FreeShm();
	FreeSocketServer();
	FreeUserBuff();

	LocalFree(pKeyInfo);
	return TRUE;
}

DWORD
FKY_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	P_FLY_KEY_INFO	pKeyInfo = (P_FLY_KEY_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;

	if (pKeyInfo->bOpen)
	{
		return NULL;
	}
	pKeyInfo->bOpen = TRUE;
	
	KeyThreadEnable(pKeyInfo,TRUE);
	Encoder_isr_Enable(pKeyInfo,TRUE);

	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.bOpen = TRUE;
	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditKey = TRUE;
	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeKeySleep = TRUE;
	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeKeySleep = FALSE;
	DBGI((TEXT("\r\nFlyAudio Key Open Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return returnWhat;
}

BOOL
FKY_Close(DWORD hDeviceContext)
{
	P_FLY_KEY_INFO	pKeyInfo = (P_FLY_KEY_INFO)hDeviceContext;

	if (!pKeyInfo->bOpen)
	{
		return FALSE;
	}

	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeKeySleep = FALSE;
	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeKeySleep = FALSE;
	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditKey = FALSE;

	pKeyInfo->bOpen = FALSE;
	pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.bOpen = FALSE;
	Encoder_isr_Enable(pKeyInfo,FALSE);
	KeyThreadEnable(pKeyInfo,FALSE);

	SetEvent(pKeyInfo->hBuffToUserDataEvent);
	DBGE((TEXT("\r\nFlyAudio Key Close")));
	return TRUE;
}

static DWORD WINAPI ThreadPowerUp(LPVOID pContext)
{
	P_FLY_KEY_INFO	pKeyInfo = (P_FLY_KEY_INFO)pContext;

	if (pKeyInfo->bOpen)
	{
		Encoder_isr_Enable(pKeyInfo,TRUE);
		pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeKeySleep = TRUE;
		pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeKeySleep = FALSE;
	}

	return 0;
}

VOID
FKY_PowerUp(DWORD hDeviceContext)
{
	P_FLY_KEY_INFO pKeyInfo = (P_FLY_KEY_INFO)hDeviceContext;
	DWORD dwThreadID;

	if (pKeyInfo->bOpen)
	{
		pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditKey = TRUE;

		CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadPowerUp, //线程的全局函数
			pKeyInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\nKey ThreadPowerUp ID:%x"),dwThreadID));
	}

	DBGE((TEXT("\r\nFlyAudio Key PowerUp")));
}

void prePowerDown(P_FLY_KEY_INFO pKeyInfo)
{
	if (pKeyInfo->bOpen)
	{
		pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeKeySleep = FALSE;
		pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeKeySleep = FALSE;
		Encoder_isr_Enable(pKeyInfo,FALSE);
	}
}

VOID
FKY_PowerDown(DWORD hDeviceContext)
{
	P_FLY_KEY_INFO pKeyInfo = (P_FLY_KEY_INFO)hDeviceContext;

	if (pKeyInfo->bOpen)
	{
		pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditKey = FALSE;
	}

	DBGE((TEXT("\r\nFlyAudio Key PowerDown")));
}

DWORD
FKY_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	P_FLY_KEY_INFO	pKeyInfo = (P_FLY_KEY_INFO)hOpenContext;

	UINT16 dwRead = 0,i;
	BYTE *buf = (BYTE *)pBuffer;

	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio key return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}
	return dwRead;

	//P_FLY_KEY_INFO	pKeyInfo = (P_FLY_KEY_INFO)hOpenContext;

	//UINT dwRead = 0,i;
	//BYTE *SendBuf = (BYTE *)pBuffer;

	//while(dwRead < Count && pKeyInfo->buffToUserLx != pKeyInfo->buffToUserHx)
	//{
	//	//DBGD((TEXT("R")));
	//	SendBuf[dwRead++] = pKeyInfo->buffToUser[pKeyInfo->buffToUserLx++];
	//	if (pKeyInfo->buffToUserLx >= DATA_BUFF_LENGTH)
	//	{
	//		pKeyInfo->buffToUserLx = 0;
	//	}

	//	if(dwRead > 0)
	//	{
	//		DBGD((TEXT("\r\nFlyAudio Key return %d bytes:"),dwRead));
	//		for (i=0; i<dwRead; i++)
	//			DBGD((TEXT("%02X "),SendBuf[i]));
	//	}


	//}
	//if(pKeyInfo->buffToUserLx != pKeyInfo->buffToUserHx)
	//{
	//	SetEvent(pKeyInfo->hBuffToUserDataEvent);
	//}



	//return dwRead;
}

DWORD
FKY_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	P_FLY_KEY_INFO	pKeyInfo = (P_FLY_KEY_INFO)hOpenContext;
	//DBGD((TEXT("\r\nFlyAudio Key Write %d"),
	//				NumberOfBytes));
	//BYTE *p;
	//p = (BYTE *)pSourceBytes;
	//DBGD((TEXT(" 0x%x 0x%x"),p[0],p[1]));

	//switch (p[0])
	//{
	//case 0xFF:
	//	if (0x01 == p[1])
	//	{
	//		FKY_PowerUp((DWORD)pKeyInfo);
	//	} 
	//	else if (0x00 == p[1])
	//	{
	//		FKY_PowerDown((DWORD)pKeyInfo);
	//	}
	//	break;
	//default:
	//	break;
	//}
	return NULL;
}

DWORD
FKY_Seek(DWORD hOpenContext, LONG Amount, DWORD  Type)
{
	return NULL;
}

BOOL
FKY_IOControl(DWORD hOpenContext,
			  DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn,
			  PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
	P_FLY_KEY_INFO	pKeyInfo = (P_FLY_KEY_INFO)hOpenContext;
	BOOL bRes = TRUE;

	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	DBGD((TEXT("\r\nFlyAudio Key IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)"),
	//				hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	DBGD((TEXT("\r\nFlyAudio Key IOControl Set IOCTL_SERIAL_SET_WAIT_MASK")));
	//	break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	//DBGD((TEXT("\r\nFlyAudio Key IOControl Set IOCTL_SERIAL_WAIT_ON_MASK")));
	//	if (pKeyInfo->bOpen)
	//	{
	//		WaitForSingleObject(pKeyInfo->hBuffToUserDataEvent,INFINITE);
	//	} 
	//	else
	//	{
	//		WaitForSingleObject(pKeyInfo->hBuffToUserDataEvent,0);
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
