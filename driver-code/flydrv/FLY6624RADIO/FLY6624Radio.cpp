/*******************************************
NAME:        FLY6624Radio.cpp 
DRIVER NAME: FLYRADIO
PLATFORM:    WINCE
AUTHOR:      LJChan
VERSION:     V1
DATA:        2012-05-02
*******************************************/
#include "FLY6624Radio.h"

void FlyRadioRDSInit(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo);
VOID FRA_PowerUp(DWORD hDeviceContext);
VOID FRA_PowerDown(DWORD hDeviceContext);

P_FLYRADIO_TEF6624_INFO gpFlyRadioInfo = NULL;

//BYTE FM_INIT_DATA[14] = {0x01,0x00,0x09,0x4D,0x99,0x0E,0xCD,0x66,0x15,0xCD,0xEE,0x14,0x40,0x14};
//BYTE AM_INIT_DATA[14] = {0x01,0x24,0x0E,0x56,0x08,0x12,0x55,0x00,0x00,0x00,0x00,0x14,0x40,0x14};

						/*02H  03H  04H  05H  06H  07H 08H  09H   0AH  0BH  0CH  0DH  0EH  0FH */
BYTE FM_INIT_DATA[16] = {0x00,0x00,0x09,0x4D,0x99,0x0E,0xCD,0x66,0x15,0x4D,0x66,0x14,0x40,0x14,0x40,0x00};
BYTE AM_INIT_DATA[16] = {0x01,0x24,0x0E,0x56,0x08,0x12,0x55,0x00,0x00,0x00,0x00,0x14,0x40,0x14,0x40,0x00};


/****************I2C通信**********************************/
//BYTE I2WriteSeq_TEF6624[] = {SEG_MASK_SEND_START | I2C_SEQ_SEG_DEVADDR_W | SEG_MASK_NEED_ACK,//写从设备地址
//I2C_SEQ_SEG_REGADDR | SEG_MASK_NEED_ACK,//写寄存器地址,先发低字节
//I2C_SEQ_SEG_DATA | SEG_MASK_NEED_ACK | SEG_MASK_SEND_STOP
//};
//
//BYTE I2CReadSeq_TEF6624[] = {SEG_MASK_SEND_START | I2C_SEQ_SEG_DEVADDR_R | SEG_MASK_NEED_ACK,//发读命令
//I2C_SEQ_SEG_DATA | SEG_MASK_NEED_NACK | SEG_MASK_SEND_STOP, //读数据,要使用NACK
//};

const GUID DEVICE_IFC_I2C_GUID;

BOOL I2C_Write_TEF6624(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo, UCHAR ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
{
	if (!pFlyRadioInfo->bOpen)
	{
		DBGE((TEXT("\r\nFlyRadio TEF6624 I2C write failed-->not open")));
		return TRUE;
	}

	i2ccfg_t i2c_cfg;
	i2c_cfg.bus_id = 1;
	i2c_cfg.chip_addr = (char)TEF6624_ADDR_W;
	i2c_cfg.sub_addr  = ulRegAddr;
	i2c_cfg.addr_type = IIC_8_BIT;
	i2c_cfg.clk_div   = 0x0F;  //clk = 3M/F = 200kHZ
	i2c_cfg.flag      = I2C_WRITE;
	i2c_cfg.msg_buf   = (char*)pRegValBuf;
	i2c_cfg.msg_size  = uiValBufLen;

	HwI2CWrite(&i2c_cfg);


	//DBGD((TEXT("\r\nFlyRadio TEF6624:I2C write 0x%2X"),ulRegAddr));
	//for (UINT i = 0;i < uiValBufLen;i++)
	//{
	//	DBGD((TEXT(" 0x%X"),pRegValBuf[i]));
	//}
	return TRUE;
}

BOOL I2C_Read_TEF6624(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo, UCHAR ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
{
	if (!pFlyRadioInfo->bOpen)
	{
		DBGE((TEXT("\r\nFlyRadio TEF6624 I2C write failed-->not open")));
		return TRUE;
	}

	i2ccfg_t i2c_cfg;
	i2c_cfg.bus_id = 1;
	i2c_cfg.chip_addr = (char)TEF6624_ADDR_R;
	i2c_cfg.sub_addr  = ulRegAddr;
	i2c_cfg.addr_type = IIC_8_BIT;
	i2c_cfg.clk_div   = 0x0F;  //clk = 3M/F = 200kHZ
	i2c_cfg.flag      = I2C_READ;
	i2c_cfg.msg_buf   = (char*)pRegValBuf;
	i2c_cfg.msg_size  = uiValBufLen;

	HwI2CRead(&i2c_cfg);

	DBGD((TEXT("\r\nFlyRadio TEF6624:I2C Read 0x%2X reg data:"),ulRegAddr));
	for (UINT i = 0;i < uiValBufLen;i++)
	{
		DBGI((TEXT(" 0x%X"),pRegValBuf[i]));
	}
	return TRUE;
}
/****************************************************/


static void radioANTControl(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BOOL bOn)
{
	if (bOn)
	{
		HwGpioOutput(IO_ANT1_I,1);
		HwGpioOutput(IO_ANT2_I,1);
	}
	else
	{
		HwGpioOutput(IO_ANT1_I,0);
		HwGpioOutput(IO_ANT2_I,0);
	}
}

/*******************************注册表***************/
static void RegDataReadRadio(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
	HKEY hKey; 
	DWORD dwDisposition;
	DWORD dwTemp;
	DWORD dwLen;
	DWORD dwType;

#if REGEDIT_NOT_READ
	return;
#endif

	DBGE((TEXT("\r\nFlyRadio TEF6624 Reg Read Start")));

	//读取收音机频点参数
	RegCreateKeyEx(HKEY_CURRENT_USER,L"FlyDrivers\\Radio",0,NULL,0,
	0,NULL,&hKey,&dwDisposition); /////////////////////////////////创建或打开注册表

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwFreqFM1",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//FM1
	{
		pFlyRadioInfo->radioInfo.iPreRadioFreqFM1 = dwTemp;
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwFreqFM2",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//FM2
	{
		pFlyRadioInfo->radioInfo.iPreRadioFreqFM2 = dwTemp;
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwFreqAM",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//AM
	{
		pFlyRadioInfo->radioInfo.iPreRadioFreqAM = dwTemp;
	}

	RegCloseKey(hKey);	
	DBGE((TEXT("\r\nFlyRadio TEF6624 Reg Read Finish %d %d %d"),pFlyRadioInfo->radioInfo.iPreRadioFreqFM1,pFlyRadioInfo->radioInfo.iPreRadioFreqFM2,pFlyRadioInfo->radioInfo.iPreRadioFreqAM));
}

static void RegDataWriteRadio(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
	HKEY hKey; 
	DWORD dwDisposition;
	DWORD dwTemp;
	DWORD dwLen;

#if REGEDIT_NOT_SAVE
	return;
#endif

	DBGE((TEXT("\r\nFlyRadio TEF6624 Reg Write Start")));

	//读取方向盘学习参数
	RegCreateKeyEx(HKEY_CURRENT_USER,L"FlyDrivers\\Radio",0,NULL,0,
	0,NULL,&hKey,&dwDisposition); /////////////////////////////////创建或打开注册表

	dwLen = 4;
	dwTemp = pFlyRadioInfo->radioInfo.iPreRadioFreqFM1;
	RegSetValueEx(hKey,L"dwFreqFM1",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);

	dwLen = 4;
	dwTemp = pFlyRadioInfo->radioInfo.iPreRadioFreqFM2;
	RegSetValueEx(hKey,L"dwFreqFM2",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);

	dwLen = 4;
	dwTemp = pFlyRadioInfo->radioInfo.iPreRadioFreqAM;
	RegSetValueEx(hKey,L"dwFreqAM",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);

	RegCloseKey(hKey);
	DBGE((TEXT("\r\nFlyRadio TEF6624 Reg Write Finish %d %d %d"),pFlyRadioInfo->radioInfo.iPreRadioFreqFM1,pFlyRadioInfo->radioInfo.iPreRadioFreqFM2,pFlyRadioInfo->radioInfo.iPreRadioFreqAM));
}


void returnRadioPowerMode(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BOOL bOn)
{
	BYTE buf[] = {0x01,0x00};

	if (bOn)
	{
		buf[1] = 1;
	}
	else
	{
		buf[1] = 0;
	}

	ReturnToUser(buf,2);
}
void returnRadioInitStatus(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BOOL bOn)
{
	BYTE buf[] = {0x02,0x00};

	if (bOn)
	{
		buf[1] = 1;
	}
	else
	{
		buf[1] = 0;
	}

	ReturnToUser(buf,2);
}

void returnRadioFreq(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,UINT iFreq)
{
	BYTE buf[] = {0x10,0x00,0x00};

	buf[1] = iFreq >> 8;
	buf[2] = iFreq;

	ReturnToUser(buf,3);
}
void returnRadioMaxAndMinPara(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
	BYTE buf[] = {0x00,0x00,0x00};

	buf[0] =0x40;
	buf[1] =pFlyRadioInfo->radioInfo.iFreqFMMax>>8;
	buf[2] =pFlyRadioInfo->radioInfo.iFreqFMMax;
	ReturnToUser(buf,3);

	buf[0] =0x41;
	buf[1] =pFlyRadioInfo->radioInfo.iFreqFMMin>>8;
	buf[2] =pFlyRadioInfo->radioInfo.iFreqFMMin;
	ReturnToUser(buf,3);

	buf[0] =0x42;
	buf[1] =pFlyRadioInfo->radioInfo.iFreqAMMax>>8;
	buf[2] =pFlyRadioInfo->radioInfo.iFreqAMMax;
	ReturnToUser(buf,3);

	buf[0] =0x43;
	buf[1] =pFlyRadioInfo->radioInfo.iFreqAMMin>>8;
	buf[2] =pFlyRadioInfo->radioInfo.iFreqAMMin;
	ReturnToUser(buf,3);

}
void returnRadioScanCtrl(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BYTE cmd)
{
	BYTE buf[] = {0x13,0x00};

	buf[1] = cmd;

	ReturnToUser(buf,2);
}
void returnRadioHaveSearched(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BOOL bHave)
{
	BYTE buf[] = {0x14,0x00};

	if (bHave)
	{
		buf[1] = 0;
	}

	ReturnToUser(buf,2);
}

void returnRadioMuteStatus(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BOOL bOn)
{
	BYTE buf[] = {0x15,0x00};

	if (bOn)
	{
		buf[1] = 1;
	}
	else
	{
		buf[1] = 0;
	}

	ReturnToUser(buf,2);
}
void returnRadioAFStatus(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BOOL bOn)
{
	BYTE buf[] = {0x16,0x00};

	if (bOn)
	{
		buf[1] = 1;
	}
	else
	{
		buf[1] = 0;
	}

	ReturnToUser(buf,2);
}


void returnRadioTAStatus(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BOOL bOn)
{
	BYTE buf[] = {0x17,0x00};

	if (bOn)
	{
		buf[1] = 1;
	}
	else
	{
		buf[1] = 0;
	}

	ReturnToUser(buf,2);
}
void returnRadioBlinkingStatus(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BOOL bOn)
{
	BYTE buf[] = {0x18,0x00};

	if (bOn)
	{
		buf[1] = 1;
	}
	else
	{
		buf[1] = 0;
	}

	ReturnToUser(buf,2);
}
void returnRadioMode(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,enumRadioMode eMode)
{
	BYTE buf[] = {0x20,0x00};

	buf[1] = eMode;

	ReturnToUser(buf,2);
}

void returnRadioRDSWorkStatus(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BOOL bOn)
{
	BYTE buf[] = {0x30,0x00};

	if (bOn)
	{
		buf[1] = 1;
	}
	else
	{
		buf[1] = 0;
	}

	ReturnToUser(buf,2);
}
/****************************************************/

/****************************TEF6624 HANDLE**********/
static void TEF6624_Set_Freq(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,enumRadioMode mode,UINT freq)
{
	BYTE freq_array[2] = {0x00,0x00};
	BYTE band = 0;
	BYTE factor = 0;
	BYTE unit = 0;

	DBGI((TEXT("\r\nFlyRadio TEF6624 SET MODE-->%d  FREQ-->%d"),mode,freq));

	if (AM != mode)
	{
		band = 1;
		factor = 20;
		unit = 100;
	}
	else
	{
		band  = 0;
		factor = 1;
		unit = 1;
	}
	freq_array[1] = ((freq*factor)/unit) & 0xff;
	freq_array[0] = (((freq*factor)/unit)>>8) | (band << 5); 
	I2C_Write_TEF6624(pFlyRadioInfo,pFlyRadioInfo->radioInfo.itef6624_writemode,freq_array,2);
	SetEvent(pFlyRadioInfo->sTEF6624RDSInfo.hDispatchRDSThreadEvent);
}

static VOID TEF6624_ChangeToFMAM(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,enumRadioMode mode)
{
	DBGE((TEXT("\r\nFlyRadio TEF6624 ChangeToFMAM-->%d"),mode));

	if (AM != pFlyRadioInfo->radioInfo.eCurRadioMode)
	{
		I2C_Write_TEF6624(pFlyRadioInfo,2,FM_INIT_DATA,16);
	}
	else
	{
		I2C_Write_TEF6624(pFlyRadioInfo,2,AM_INIT_DATA,16);
	}

	TEF6624_Set_Freq(pFlyRadioInfo,mode,*pFlyRadioInfo->radioInfo.pCurRadioFreq);
	//pFlyRadioInfo->bCurMute = TRUE;//触发恢复静音状态
}

static VOID TEF6624_Mute(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo, BOOL mute)
{
	if (mute)
	{
		pFlyRadioInfo->radioInfo.itef6624_writemode = TEF6624_WRITE_MODE_SEARCH;
		TEF6624_Set_Freq(pFlyRadioInfo,pFlyRadioInfo->radioInfo.eCurRadioMode,*pFlyRadioInfo->radioInfo.pCurRadioFreq);

		//while (IpcWhatEvent(EVENT_GLOBAL_RADIO_MUTE_IN_REQ_ID)
		//	|| IpcWhatEvent(EVENT_GLOBAL_RADIO_MUTE_OUT_REQ_ID))
		//{
		//	Sleep(10);
		//}
		//IpcStartEvent(EVENT_GLOBAL_RADIO_MUTE_IN_REQ_ID);//发送进入静音
		//while (!IpcWhatEvent(EVENT_GLOBAL_RADIO_MUTE_IN_OK_ID))//等待OK
		//{
		//	Sleep(10);
		//}
		//IpcClearEvent(EVENT_GLOBAL_RADIO_MUTE_IN_OK_ID);//清除
	}
	else
	{
		pFlyRadioInfo->radioInfo.itef6624_writemode = TEF6624_WRITE_MODE_PRESET;
		TEF6624_Set_Freq(pFlyRadioInfo,pFlyRadioInfo->radioInfo.eCurRadioMode,*pFlyRadioInfo->radioInfo.pCurRadioFreq);

		//IpcStartEvent(EVENT_GLOBAL_RADIO_MUTE_OUT_REQ_ID);//发送退出静音
	}
}

static VOID TEF6624_Get_Signalquality(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
	BYTE tuner_read[5];

	I2C_Read_TEF6624(pFlyRadioInfo, 0x00, tuner_read, 5);

	pFlyRadioInfo->radioInfo.iSignalLevel = tuner_read[TEF6624_REG_R_LEVEL];
	pFlyRadioInfo->radioInfo.iSignalNoise = (tuner_read[TEF6624_REG_R_NOISE] & 0xF0) >> 4;

	DBGE((TEXT("\r\nFlyRadio TEF6624 Get_Signalquality LEVEL-->%d, NOISE-->%d"),pFlyRadioInfo->radioInfo.iSignalLevel,pFlyRadioInfo->radioInfo.iSignalNoise));
}

static BOOL bRadioSignalGood(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,enumRadioMode radioMode)
{
	TEF6624_Get_Signalquality(pFlyRadioInfo);
	
	if (AM != radioMode)
	{
		if ((pFlyRadioInfo->radioInfo.iSignalLevel < TEF6624_STOP_FM_LEVEL) || (pFlyRadioInfo->radioInfo.iSignalNoise > TEF6624_STOP_FM_NOISE))
		{
			return FALSE;
		}
	}
	else
	{
		if ((pFlyRadioInfo->radioInfo.iSignalLevel < TEF6624_STOP_AM_LEVEL) || (pFlyRadioInfo->radioInfo.iSignalNoise > TEF6624_STOP_AM_NOISE))
		{
			return FALSE;
		}
	}

	return TRUE;
}

/****************************************************/

static UINT RadioStepFreqGenerate(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,enumRadioMode eMode,UINT iFreq,enumRadioStepDirection eForward,enumRadioStepMode eStepMode)
{
	if (STEP_FORWARD == eForward)//Forward
	{	
		if(AM != eMode)
		{
			if(STEP_MANUAL == eStepMode)//手动
			{
				iFreq += pFlyRadioInfo->radioInfo.iFreqFMManualStep;
			}
			else//自动
			{
				iFreq += pFlyRadioInfo->radioInfo.iFreqFMScanStep;
			}
			if(iFreq > pFlyRadioInfo->radioInfo.iFreqFMMax)	iFreq = pFlyRadioInfo->radioInfo.iFreqFMMin;
			if(iFreq < pFlyRadioInfo->radioInfo.iFreqFMMin)	iFreq = pFlyRadioInfo->radioInfo.iFreqFMMin;

		}
		else
		{
			if(STEP_MANUAL == eStepMode)//手动
			{
				iFreq += pFlyRadioInfo->radioInfo.iFreqAMManualStep;
			}
			else//自动
			{
				iFreq += pFlyRadioInfo->radioInfo.iFreqAMScanStep;
			}
			if(iFreq > pFlyRadioInfo->radioInfo.iFreqAMMax)	iFreq = pFlyRadioInfo->radioInfo.iFreqAMMin;
			if(iFreq < pFlyRadioInfo->radioInfo.iFreqAMMin)	iFreq = pFlyRadioInfo->radioInfo.iFreqAMMin;
		}
	}
	else if (STEP_BACKWARD == eForward)//Backward
	{
		if(AM != eMode)
		{
			if(STEP_MANUAL == eStepMode)//手动
			{
				iFreq -= pFlyRadioInfo->radioInfo.iFreqFMManualStep;
			}
			else//自动
			{
				iFreq -= pFlyRadioInfo->radioInfo.iFreqFMScanStep;
			}
			if(iFreq < pFlyRadioInfo->radioInfo.iFreqFMMin)	iFreq = pFlyRadioInfo->radioInfo.iFreqFMMax;
			if(iFreq > pFlyRadioInfo->radioInfo.iFreqFMMax)	iFreq = pFlyRadioInfo->radioInfo.iFreqFMMax;
		}
		else
		{
			if(STEP_MANUAL == eStepMode)//手动
			{
				iFreq -= pFlyRadioInfo->radioInfo.iFreqAMManualStep;
			}
			else// if(RadioScanStatus == 1)//自动
			{
				iFreq -= pFlyRadioInfo->radioInfo.iFreqAMScanStep;
			}
			if(iFreq < pFlyRadioInfo->radioInfo.iFreqAMMin)	iFreq = pFlyRadioInfo->radioInfo.iFreqAMMax;
			if(iFreq > pFlyRadioInfo->radioInfo.iFreqAMMax)	iFreq = pFlyRadioInfo->radioInfo.iFreqAMMax;
		}
	}
	else {
		if(AM != eMode)
		{
			if(iFreq > pFlyRadioInfo->radioInfo.iFreqFMMax)	iFreq = pFlyRadioInfo->radioInfo.iFreqFMMin;
			if(iFreq < pFlyRadioInfo->radioInfo.iFreqFMMin)	iFreq = pFlyRadioInfo->radioInfo.iFreqFMMin;
		}
		else
		{
			if(iFreq > pFlyRadioInfo->radioInfo.iFreqAMMax)	iFreq = pFlyRadioInfo->radioInfo.iFreqAMMin;
			if(iFreq < pFlyRadioInfo->radioInfo.iFreqAMMin)	iFreq = pFlyRadioInfo->radioInfo.iFreqAMMin;
		}
	}
	return iFreq;
}

static void buttomJumpFreqAndReturn(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)//界面按键跳频用
{
	*pFlyRadioInfo->radioInfo.pPreRadioFreq = 
		RadioStepFreqGenerate(pFlyRadioInfo
		,pFlyRadioInfo->radioInfo.ePreRadioMode
		,*pFlyRadioInfo->radioInfo.pPreRadioFreq
		,pFlyRadioInfo->radioInfo.eButtomStepDirection
		,STEP_MANUAL);
	//Set_Freq_7541(pFlyRadioInfo
	//	,pFlyRadioInfo->radioInfo.ePreRadioMode
	//	,*pFlyRadioInfo->radioInfo.pPreRadioFreq);
	returnRadioFreq(pFlyRadioInfo,*pFlyRadioInfo->radioInfo.pPreRadioFreq);
}
static UINT GetCorrectScanStartFreq(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,UINT *pFreq)
{
	UINT base;
	if (AM != pFlyRadioInfo->radioInfo.eCurRadioMode)
	{
		if (((*pFreq - pFlyRadioInfo->radioInfo.iFreqFMMin) % pFlyRadioInfo->radioInfo.iFreqFMScanStep) != 0)
		{
			base = 1;
		}
		else
		{
			base = 0;
		}
		*pFreq = pFlyRadioInfo->radioInfo.iFreqFMMin + (base + (*pFreq - pFlyRadioInfo->radioInfo.iFreqFMMin)/pFlyRadioInfo->radioInfo.iFreqFMScanStep)*pFlyRadioInfo->radioInfo.iFreqFMScanStep;
	}
	else
	{
		if (((*pFreq - pFlyRadioInfo->radioInfo.iFreqAMMin) % pFlyRadioInfo->radioInfo.iFreqAMScanStep) != 0)
		{
			base = 1;
		}
		else
		{
			base = 0;
		}		
		*pFreq = pFlyRadioInfo->radioInfo.iFreqAMMin + (base + (*pFreq - pFlyRadioInfo->radioInfo.iFreqAMMin)/pFlyRadioInfo->radioInfo.iFreqAMScanStep)*pFlyRadioInfo->radioInfo.iFreqAMScanStep;
	}
	//if (((*pFreq - pFlyRadioInfo->radioInfo.iFreqFMMin) % pFlyRadioInfo->radioInfo.iFreqFMScanStep) != 0)
	//{
	//	if (AM != pFlyRadioInfo->radioInfo.eCurRadioMode)
	//	{
	//		*pFreq = *pFreq + pFlyRadioInfo->radioInfo.iFreqFMScanStep;
	//	}
	//	else
	//	{
	//		*pFreq = *pFreq + pFlyRadioInfo->radioInfo.iFreqAMScanStep;
	//	}

	//}
	return *pFreq;
}

//处理协议
static VOID FlyRadioDealWinceCmd(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BYTE *pdata,UINT len)
{
	UINT iTemp;

	switch(pdata[0])
	{
	case 0x01:
		if (0x01 == pdata[1])//初始化命令开始
		{
			pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNeedProcVoltageShakeRadio = 85;
			pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditRadio = TRUE;

			RegDataReadRadio(pFlyRadioInfo);

			FlyRadioRDSInit(pFlyRadioInfo);

			returnRadioPowerMode(pFlyRadioInfo,TRUE);
			returnRadioInitStatus(pFlyRadioInfo,TRUE);
			returnRadioMaxAndMinPara(pFlyRadioInfo);

			pFlyRadioInfo->bPowerUp = TRUE;
			pFlyRadioInfo->bNeedInit = TRUE;
			SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);//激活一次
		}
		else if (0x00 == pdata[1])
		{
			returnRadioPowerMode(pFlyRadioInfo,FALSE);
		}
		break;
	case 0x03://软件模拟按键 1-FM1 2-FM2 3-AM 4-STOP 6624RADIO 5-AF 6-TA
		switch(pdata[1])
		{
		case 0x01:
		case 0x02:
		case 0x03:
			if (pFlyRadioInfo->radioInfo.bPreScaning)
			{
				pFlyRadioInfo->radioInfo.bPreScaning = FALSE;
			}
			if (pFlyRadioInfo->bPowerUp)//直到之前的扫描停止
			{	
				SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
				while (pFlyRadioInfo->radioInfo.bCurScaning)
				{
					Sleep(100);
				}
			}
			if (0x01 == pdata[1])
			{
				pFlyRadioInfo->radioInfo.ePreRadioMode = FM1;
				DBGD((TEXT("\r\nFlyRadio TEF6624 set mode -->FM1")));
			} 
			else if (0x02 == pdata[1])
			{
				pFlyRadioInfo->radioInfo.ePreRadioMode = FM2;
				DBGD((TEXT("\r\nFlyRadio TEF6624 set mode -->FM2")));
			}
			else if (0x03 == pdata[1])
			{
				pFlyRadioInfo->radioInfo.ePreRadioMode = AM;
				DBGD((TEXT("\r\nFlyRadio TEF6624 set mode -->AM")));
			}

			returnRadioMode(pFlyRadioInfo,pFlyRadioInfo->radioInfo.ePreRadioMode);
			if (AM == pFlyRadioInfo->radioInfo.ePreRadioMode)
			{
				returnRadioFreq(pFlyRadioInfo,pFlyRadioInfo->radioInfo.iPreRadioFreqAM);
			}
			else if (FM1 == pFlyRadioInfo->radioInfo.ePreRadioMode)
			{
				returnRadioFreq(pFlyRadioInfo,pFlyRadioInfo->radioInfo.iPreRadioFreqFM1);
			}
			else if (FM2 == pFlyRadioInfo->radioInfo.ePreRadioMode)
			{
				returnRadioFreq(pFlyRadioInfo,pFlyRadioInfo->radioInfo.iPreRadioFreqFM2);
			}

			if (pFlyRadioInfo->bPowerUp)
			{	
				SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
			}
			break;
		case 0x04:
			pFlyRadioInfo->radioInfo.bPreScaning = !pFlyRadioInfo->radioInfo.bPreScaning;
			if (pFlyRadioInfo->bPowerUp)
			{
				SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
			}
			break;
		case 0x05:
			pFlyRadioInfo->RDSInfo.RadioRDSAFControlOn = !pFlyRadioInfo->RDSInfo.RadioRDSAFControlOn;
			returnRadioAFStatus(pFlyRadioInfo,pFlyRadioInfo->RDSInfo.RadioRDSAFControlOn);
			if (pFlyRadioInfo->bPowerUp)
			{
				SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
			}
			break;
		case 0x06:
			pFlyRadioInfo->RDSInfo.RadioRDSTAControlOn = !pFlyRadioInfo->RDSInfo.RadioRDSTAControlOn;
			returnRadioTAStatus(pFlyRadioInfo,pFlyRadioInfo->RDSInfo.RadioRDSTAControlOn);
			if (pFlyRadioInfo->bPowerUp)
			{
				SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
			}
			break;
		case 0x07:
			if (pFlyRadioInfo->radioInfo.bPreScaning)
			{
				pFlyRadioInfo->radioInfo.bPreScaning = FALSE;
			}
			if (pFlyRadioInfo->radioInfo.ePreRadioMode == pFlyRadioInfo->radioInfo.eCurRadioMode)
			{
				*pFlyRadioInfo->radioInfo.pPreRadioFreq = 
					RadioStepFreqGenerate(pFlyRadioInfo
					,pFlyRadioInfo->radioInfo.ePreRadioMode
					,*pFlyRadioInfo->radioInfo.pPreRadioFreq
					,STEP_FORWARD
					,STEP_MANUAL);
				DBGD((TEXT("\r\nFlyRadio TEF6624 set freq value-->%d"),*pFlyRadioInfo->radioInfo.pPreRadioFreq));
					returnRadioFreq(pFlyRadioInfo,*pFlyRadioInfo->radioInfo.pPreRadioFreq);
				if (pFlyRadioInfo->bPowerUp)
				{
					SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
				}
			}
			break;
		case 0x08:
			if (pFlyRadioInfo->radioInfo.bPreScaning)
			{
				pFlyRadioInfo->radioInfo.bPreScaning = FALSE;
			}
			if (pFlyRadioInfo->radioInfo.ePreRadioMode == pFlyRadioInfo->radioInfo.eCurRadioMode)
			{
				*pFlyRadioInfo->radioInfo.pPreRadioFreq = 
					RadioStepFreqGenerate(pFlyRadioInfo
					,pFlyRadioInfo->radioInfo.ePreRadioMode
					,*pFlyRadioInfo->radioInfo.pPreRadioFreq
					,STEP_BACKWARD
					,STEP_MANUAL);
				DBGD((TEXT("\r\nFlyRadio TEF6624 set freq value-->%d"),*pFlyRadioInfo->radioInfo.pPreRadioFreq));
					returnRadioFreq(pFlyRadioInfo,*pFlyRadioInfo->radioInfo.pPreRadioFreq);
				if (pFlyRadioInfo->bPowerUp)
				{
					SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
				}
			}
			break;
		default:
			DBGD((TEXT("\r\nFlyRadio TEF6624 user command 0x03-->%X"),pdata[1]));
				break;
		}
		break;
	case 0x10://设置收音机频率 
		if (pFlyRadioInfo->radioInfo.bPreScaning)
		{
			pFlyRadioInfo->radioInfo.bPreScaning = FALSE;
		}
		iTemp = pdata[1]*256+pdata[2];
		iTemp = 
			RadioStepFreqGenerate(pFlyRadioInfo
			,pFlyRadioInfo->radioInfo.ePreRadioMode
			,iTemp
			,STEP_NONE,STEP_MANUAL);
		if (AM == pFlyRadioInfo->radioInfo.ePreRadioMode)
		{
			pFlyRadioInfo->radioInfo.iPreRadioFreqAM = iTemp;
		}
		else if (FM1 == pFlyRadioInfo->radioInfo.ePreRadioMode)
		{
			pFlyRadioInfo->radioInfo.iPreRadioFreqFM1 = iTemp;
		}
		else if (FM2 == pFlyRadioInfo->radioInfo.ePreRadioMode)
		{
			pFlyRadioInfo->radioInfo.iPreRadioFreqFM2 = iTemp;
		}
		DBGD((TEXT("\r\nFlyRadio TEF6624 set freq value-->%d"),iTemp));
			returnRadioFreq(pFlyRadioInfo,iTemp);
		if (pFlyRadioInfo->bPowerUp)
		{
			SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
		}
		break;
	case 0x11://频点+
		if (pFlyRadioInfo->radioInfo.bPreScaning)
		{
			pFlyRadioInfo->radioInfo.bPreScaning = FALSE;
		}
		pFlyRadioInfo->radioInfo.eButtomStepDirection = STEP_FORWARD;
		if (0x00 == pdata[1])
		{
			//ReturnToUser(pdata,len);
			buttomJumpFreqAndReturn(pFlyRadioInfo);
			pFlyRadioInfo->radioInfo.bPreStepButtomDown = TRUE;
			pFlyRadioInfo->radioInfo.iButtomStepCount = 0;
		}
		else if (0x01 == pdata[1])
		{
			//ReturnToUser(pdata,len);
			pFlyRadioInfo->radioInfo.bPreStepButtomDown = FALSE;
		}
		if (pFlyRadioInfo->bPowerUp)
		{
			SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
		}
		break;
	case 0x12://频点-
		if (pFlyRadioInfo->radioInfo.bPreScaning)
		{
			pFlyRadioInfo->radioInfo.bPreScaning = FALSE;
		}
		pFlyRadioInfo->radioInfo.eButtomStepDirection = STEP_BACKWARD;
		if (0x00 == pdata[1])
		{
			buttomJumpFreqAndReturn(pFlyRadioInfo);
			pFlyRadioInfo->radioInfo.bPreStepButtomDown = TRUE;
			pFlyRadioInfo->radioInfo.iButtomStepCount = 0;
		}
		else if (0x01 == pdata[1])
		{
			pFlyRadioInfo->radioInfo.bPreStepButtomDown = FALSE;
		}
		if (pFlyRadioInfo->bPowerUp)
		{
			SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
		}
		break;
	case 0x13://扫描控制
		switch(pdata[1])
		{
		case 0x00:
		case 0x01:
			pFlyRadioInfo->radioInfo.bPreScaning = TRUE;
			pFlyRadioInfo->radioInfo.eScanDirection = STEP_FORWARD;
			pFlyRadioInfo->radioInfo.bScanRepeatFlag = FALSE;
			break;
		case 0x02:
			pFlyRadioInfo->radioInfo.bPreScaning = TRUE;
			pFlyRadioInfo->radioInfo.eScanDirection = STEP_BACKWARD;
			pFlyRadioInfo->radioInfo.bScanRepeatFlag = FALSE;
			break;
		case 0x03:
			pFlyRadioInfo->radioInfo.bPreScaning = FALSE;
			break;
		case 0x04:
		case 0x05:
			pFlyRadioInfo->radioInfo.bPreScaning = TRUE;
			pFlyRadioInfo->radioInfo.eScanDirection = STEP_FORWARD;
			pFlyRadioInfo->radioInfo.bScanRepeatFlag = TRUE;
			break;
		case 0x06:
			pFlyRadioInfo->radioInfo.bPreScaning = TRUE;
			pFlyRadioInfo->radioInfo.eScanDirection = STEP_BACKWARD;
			pFlyRadioInfo->radioInfo.bScanRepeatFlag = TRUE;
			break;
		default:break;
		}
		returnRadioScanCtrl(pFlyRadioInfo,pdata[1]);
		if (pFlyRadioInfo->bPowerUp)
		{
			SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
		}
		DBGD((TEXT("\r\nFlyRadio TEF6624 set scan ctrl:%d"),pdata[1]));
			break;
	case 0x15://开始收音
		if (0x00 == pdata[1])
		{
			pFlyRadioInfo->bPreMute = TRUE;
			if (pFlyRadioInfo->radioInfo.bPreScaning)
			{
				pFlyRadioInfo->radioInfo.bPreScaning = FALSE;
			}
			returnRadioMuteStatus(pFlyRadioInfo,FALSE);
		}
		else if (0x01 == pdata[1])
		{
			pFlyRadioInfo->bPreMute = FALSE;
			returnRadioMuteStatus(pFlyRadioInfo,TRUE);
		}				
		if (pFlyRadioInfo->bPowerUp)
		{
			SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
		}			
		break;
	case 0x16://AF开关
		if (0x01 == pdata[1])
		{
			pFlyRadioInfo->RDSInfo.RadioRDSAFControlOn = TRUE;
			returnRadioAFStatus(pFlyRadioInfo,TRUE);
		}
		else if (0x00 == pdata[1])
		{
			pFlyRadioInfo->RDSInfo.RadioRDSAFControlOn = FALSE;
			returnRadioAFStatus(pFlyRadioInfo,FALSE);
		}
		if (pFlyRadioInfo->bPowerUp)
		{
			SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
		}
		break;
	case 0x17://TA开关
		if (0x01 == pdata[1])
		{
			pFlyRadioInfo->RDSInfo.RadioRDSTAControlOn = TRUE;
			returnRadioTAStatus(pFlyRadioInfo,TRUE);
		}
		else if (0x00 == pdata[1])
		{
			pFlyRadioInfo->RDSInfo.RadioRDSTAControlOn = FALSE;
			returnRadioTAStatus(pFlyRadioInfo,FALSE);
		}
		if (pFlyRadioInfo->bPowerUp)
		{
			SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
		}
		break;
	case 0x30://交通广播开关
		if (0x01 == pdata[1])
		{
			pFlyRadioInfo->RDSInfo.RadioRDSAFControlOn = TRUE;
			returnRadioRDSWorkStatus(pFlyRadioInfo,TRUE);				
		}
		else if (0x00 == pdata[1])
		{
			pFlyRadioInfo->RDSInfo.RadioRDSAFControlOn = FALSE;
			returnRadioRDSWorkStatus(pFlyRadioInfo,FALSE);
		}
		if (pFlyRadioInfo->bPowerUp)
		{
			SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
		}
		break;
	case 0xFF:
		if (0x01 == pdata[1])
		{
			FRA_PowerUp((DWORD)pFlyRadioInfo);
		} 
		else if (0x00 == pdata[1])
		{
			FRA_PowerDown((DWORD)pFlyRadioInfo);
		}
		break;
	default:
		DBGD((TEXT("\r\nFlyRadio TEF6624 user command unhandle-->%X"),pdata[1]));
			break;
	}
}

//RDS数据清除
void FlyRadioRDSInit(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
#if RADIO_RDS
	//RDSParaInit(pFlyRadioInfo);
#endif
}

//设置收音机的所在地区
static void FlyRadioIDChange(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BYTE iID)
{
	DBGE((TEXT("\r\nFlyRadio TEF6624 ID Change-->%d"),iID));

	if (0x00 == iID)//China
	{
		pFlyRadioInfo->radioInfo.iFreqFMMin = 8750;		
		pFlyRadioInfo->radioInfo.iFreqFMMax = 10800;
		pFlyRadioInfo->radioInfo.iFreqFMManualStep = 5;
		pFlyRadioInfo->radioInfo.iFreqFMScanStep = 10;

		pFlyRadioInfo->radioInfo.iFreqAMMin = 522;
		pFlyRadioInfo->radioInfo.iFreqAMMax = 1620;
		pFlyRadioInfo->radioInfo.iFreqAMManualStep = 9;
		pFlyRadioInfo->radioInfo.iFreqAMScanStep = 9;

		pFlyRadioInfo->radioInfo.iPreRadioFreqFM1 = 8750;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqFM1 = 8750;
		pFlyRadioInfo->radioInfo.iPreRadioFreqFM2 = 8750;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqFM2 = 8750;
		pFlyRadioInfo->radioInfo.iPreRadioFreqAM = 522;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqAM = 522;
	}
	else if (0x01 == iID)//USA
	{
		pFlyRadioInfo->radioInfo.iFreqFMMin = 8750;
		pFlyRadioInfo->radioInfo.iFreqFMMax = 10790;
		pFlyRadioInfo->radioInfo.iFreqFMManualStep = 5;
		pFlyRadioInfo->radioInfo.iFreqFMScanStep = 20;

		pFlyRadioInfo->radioInfo.iFreqAMMin = 530;
		pFlyRadioInfo->radioInfo.iFreqAMMax = 1720;
		pFlyRadioInfo->radioInfo.iFreqAMManualStep = 10;
		pFlyRadioInfo->radioInfo.iFreqAMScanStep = 10;

		pFlyRadioInfo->radioInfo.iPreRadioFreqFM1 = 8750;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqFM1 = 8750;
		pFlyRadioInfo->radioInfo.iPreRadioFreqFM2 = 8750;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqFM2 = 8750;
		pFlyRadioInfo->radioInfo.iPreRadioFreqAM = 530;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqAM = 530;
	}
	else if (0x02 == iID)//S.Amer-1
	{
		pFlyRadioInfo->radioInfo.iFreqFMMin = 8750;
		pFlyRadioInfo->radioInfo.iFreqFMMax = 10800;
		pFlyRadioInfo->radioInfo.iFreqFMManualStep = 10;
		pFlyRadioInfo->radioInfo.iFreqFMScanStep = 10;

		pFlyRadioInfo->radioInfo.iFreqAMMin = 530;
		pFlyRadioInfo->radioInfo.iFreqAMMax = 1710;
		pFlyRadioInfo->radioInfo.iFreqAMManualStep = 10;
		pFlyRadioInfo->radioInfo.iFreqAMScanStep = 10;

		pFlyRadioInfo->radioInfo.iPreRadioFreqFM1 = 8750;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqFM1 = 8750;
		pFlyRadioInfo->radioInfo.iPreRadioFreqFM2 = 8750;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqFM2 = 8750;
		pFlyRadioInfo->radioInfo.iPreRadioFreqAM = 530;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqAM = 530;
	}
	else if (0x03 == iID)//S.Amer-2
	{
		pFlyRadioInfo->radioInfo.iFreqFMMin = 8750;
		pFlyRadioInfo->radioInfo.iFreqFMMax = 10800;
		pFlyRadioInfo->radioInfo.iFreqFMManualStep = 10;
		pFlyRadioInfo->radioInfo.iFreqFMScanStep = 10;

		pFlyRadioInfo->radioInfo.iFreqAMMin = 520;
		pFlyRadioInfo->radioInfo.iFreqAMMax = 1600;
		pFlyRadioInfo->radioInfo.iFreqAMManualStep = 5;
		pFlyRadioInfo->radioInfo.iFreqAMScanStep = 5;

		pFlyRadioInfo->radioInfo.iPreRadioFreqFM1 = 8750;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqFM1 = 8750;
		pFlyRadioInfo->radioInfo.iPreRadioFreqFM2 = 8750;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqFM2 = 8750;
		pFlyRadioInfo->radioInfo.iPreRadioFreqAM = 520;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqAM = 520;
	}
	else if (0x04 == iID)//KOREA
	{
		pFlyRadioInfo->radioInfo.iFreqFMMin = 8810;
		pFlyRadioInfo->radioInfo.iFreqFMMax = 10790;
		pFlyRadioInfo->radioInfo.iFreqFMManualStep = 5;
		pFlyRadioInfo->radioInfo.iFreqFMScanStep = 20;

		pFlyRadioInfo->radioInfo.iFreqAMMin = 531;
		pFlyRadioInfo->radioInfo.iFreqAMMax = 1620;
		pFlyRadioInfo->radioInfo.iFreqAMManualStep = 9;
		pFlyRadioInfo->radioInfo.iFreqAMScanStep = 9;

		pFlyRadioInfo->radioInfo.iPreRadioFreqFM1 = 8810;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqFM1 = 8810;
		pFlyRadioInfo->radioInfo.iPreRadioFreqFM2 = 8810;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqFM2 = 8810;
		pFlyRadioInfo->radioInfo.iPreRadioFreqAM = 531;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqAM = 531;
	}
	else if (0x05 == iID)//Thailand
	{
		pFlyRadioInfo->radioInfo.iFreqFMMin = 8750;
		pFlyRadioInfo->radioInfo.iFreqFMMax = 10800;
		pFlyRadioInfo->radioInfo.iFreqFMManualStep = 5;
		pFlyRadioInfo->radioInfo.iFreqFMScanStep = 25;

		pFlyRadioInfo->radioInfo.iFreqAMMin = 531;
		pFlyRadioInfo->radioInfo.iFreqAMMax = 1602;
		pFlyRadioInfo->radioInfo.iFreqAMManualStep = 9;
		pFlyRadioInfo->radioInfo.iFreqAMScanStep = 9;

		pFlyRadioInfo->radioInfo.iPreRadioFreqFM1 = 8750;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqFM1 = 8750;
		pFlyRadioInfo->radioInfo.iPreRadioFreqFM2 = 8750;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqFM2 = 8750;
		pFlyRadioInfo->radioInfo.iPreRadioFreqAM = 531;
		//pFlyRadioInfo->radioInfo.iCurRadioFreqAM = 531;
	}
	pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iFreqFMMin=pFlyRadioInfo->radioInfo.iFreqFMMin;
	pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iFreqFMMax=pFlyRadioInfo->radioInfo.iFreqFMMax;	
	pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iFreqAMMin=pFlyRadioInfo->radioInfo.iFreqAMMin;
	pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iFreqAMMax=pFlyRadioInfo->radioInfo.iFreqAMMax;

	if (AM == pFlyRadioInfo->radioInfo.ePreRadioMode)
	{
		returnRadioFreq(pFlyRadioInfo,pFlyRadioInfo->radioInfo.iPreRadioFreqAM);
	}
	else if (FM1 == pFlyRadioInfo->radioInfo.ePreRadioMode)
	{
		returnRadioFreq(pFlyRadioInfo,pFlyRadioInfo->radioInfo.iPreRadioFreqFM1);
	}
	else if (FM2 == pFlyRadioInfo->radioInfo.ePreRadioMode)
	{
		returnRadioFreq(pFlyRadioInfo,pFlyRadioInfo->radioInfo.iPreRadioFreqFM2);
	}

	returnRadioMaxAndMinPara(pFlyRadioInfo);
}

//主线程
static DWORD WINAPI FlyRadioMainThread(LPVOID pContext)
{
	ULONG WaitReturn;
	static UINT RadioScanStatus;

	P_FLYRADIO_TEF6624_INFO pFlyRadioInfo = (P_FLYRADIO_TEF6624_INFO)pContext;

	while (!pFlyRadioInfo->bKillDispatchMainThread)
	{
		WaitReturn = WaitForSingleObject(pFlyRadioInfo->hDispatchMainThreadEvent, INFINITE);
		DBGD((TEXT("\r\nFlyRadio TEF6624 MainThread Running!")));

		if (IpcWhatEvent(EVENT_GLOBAL_REG_RESTORE_RADIO_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_REG_RESTORE_RADIO_ID);
			if (pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditRadio)
			{
				pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditRadio = FALSE;
				if (pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRestoreRegeditToFactory)
				{
					FlyRadioIDChange(pFlyRadioInfo,0);//pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser);
					pFlyRadioInfo->radioInfo.iPreRadioFreqFM1 = pFlyRadioInfo->radioInfo.iFreqFMMin;
					pFlyRadioInfo->radioInfo.iPreRadioFreqFM2 = pFlyRadioInfo->radioInfo.iFreqFMMin;
					pFlyRadioInfo->radioInfo.iPreRadioFreqAM = pFlyRadioInfo->radioInfo.iFreqAMMin;
					pFlyRadioInfo->radioInfo.iCurRadioFreqFM1 = pFlyRadioInfo->radioInfo.iFreqFMMin;
					pFlyRadioInfo->radioInfo.iCurRadioFreqFM2 = pFlyRadioInfo->radioInfo.iFreqFMMin;
					pFlyRadioInfo->radioInfo.iCurRadioFreqAM = pFlyRadioInfo->radioInfo.iFreqAMMin;
				}
				RegDataWriteRadio(pFlyRadioInfo);
			}
		}

		if (FALSE == pFlyRadioInfo->bPowerUp)
		{
		}
		else
		{
			if (TRUE == pFlyRadioInfo->bNeedInit)
			{
				pFlyRadioInfo->bNeedInit = FALSE;

				TEF6624_ChangeToFMAM(pFlyRadioInfo,pFlyRadioInfo->radioInfo.eCurRadioMode);
			}

			if (IpcWhatEvent(EVENT_GLOBAL_RADIO_CHANGE_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_RADIO_CHANGE_ID);
				DBGD((TEXT("\r\nFlyRadio TEF6624 Radio ID Change!")));
				FlyRadioIDChange(pFlyRadioInfo,
					pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser);
			}

			if (IpcWhatEvent(EVENT_GLOBAL_BATTERY_RECOVERY_RADIO_ID))
			{
				DBGD((TEXT("\r\nFlyRadio TEF6624 Voltage After Low Proc")));
				IpcClearEvent(EVENT_GLOBAL_BATTERY_RECOVERY_RADIO_ID);

				TEF6624_ChangeToFMAM(pFlyRadioInfo,pFlyRadioInfo->radioInfo.eCurRadioMode);
			}

			if (IpcWhatEvent(EVENT_GLOBAL_RADIO_ANTENNA_ID))
			{
				DBGD((TEXT("\r\nFlyRadio TEF6624 ANTENNA ctrl")));
					IpcClearEvent(EVENT_GLOBAL_RADIO_ANTENNA_ID);
				if (pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eAudioInput == RADIO)
				{
					radioANTControl(pFlyRadioInfo,TRUE);
				}
				else
				{
					radioANTControl(pFlyRadioInfo,FALSE);
				}
			}
			SetEvent(pFlyRadioInfo->hDispatchScanThreadEvent);
		}
	}
	DBGD((TEXT("\r\nFlyRadio TEF6624 MainThread Prepare exit!")));

	pFlyRadioInfo->bPowerUp = FALSE;
	pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNeedProcVoltageShakeRadio = 0;

	while (pFlyRadioInfo->FlyRadioScanThreadHandle)
	{
		SetEvent(pFlyRadioInfo->hDispatchScanThreadEvent);
		Sleep(10);
	}
	CloseHandle(pFlyRadioInfo->hDispatchScanThreadEvent);

	//if(pFlyRadioInfo->FlyRadioI2CHandle)
	//{
	//	I2CClose(pFlyRadioInfo->FlyRadioI2CHandle);
	//	pFlyRadioInfo->FlyRadioI2CHandle = NULL;
	//}

	pFlyRadioInfo->FlyRadioMainThreadHandle = NULL;

	DBGD((TEXT("\r\nFlyRadio TEF6624 MainThread exit!")));
	return 0;
}

//SCAN线程
static DWORD WINAPI FlyRadioScanThread(LPVOID pContext)
{
	P_FLYRADIO_TEF6624_INFO pFlyRadioInfo = (P_FLYRADIO_TEF6624_INFO)pContext;
	ULONG WaitReturn;
	UINT iScanStartFreq;
	BOOL bHaveSearched;
	UINT iHaveSearchedLevel;
	ULONG nowTimer,lastTimer;
	ULONG iThreadFreqStepWaitTime = 0;

	BYTE iBlinkingTimes;
	BOOL bBlinkingStatus;

	DBGD((TEXT("\r\nFlyRadio TEF6624 ScanThread in")));
	while (!pFlyRadioInfo->bKillDispatchScanThread)
	{
		if (0 == iThreadFreqStepWaitTime)
		{
			WaitReturn = WaitForSingleObject(pFlyRadioInfo->hDispatchScanThreadEvent, INFINITE);
		}
		else
		{
			WaitReturn = WaitForSingleObject(pFlyRadioInfo->hDispatchScanThreadEvent, iThreadFreqStepWaitTime);
		}
		iThreadFreqStepWaitTime = 0;
		DBGD((TEXT("\r\nFlyRadio TEF6624 ScanThread Running Start-->%d"),iThreadFreqStepWaitTime));

		if (pFlyRadioInfo->radioInfo.bCurScaning == TRUE && pFlyRadioInfo->radioInfo.bPreScaning == FALSE)//跳出搜台
		{
			DBGD((TEXT("\r\nFlyRadio TEF6624 ScanThread --------------exit scaning")));
			pFlyRadioInfo->radioInfo.bCurScaning = pFlyRadioInfo->radioInfo.bPreScaning;

			returnRadioFreq(pFlyRadioInfo,*pFlyRadioInfo->radioInfo.pPreRadioFreq);
			returnRadioScanCtrl(pFlyRadioInfo,0x03);//stop

			pFlyRadioInfo->bCurMute = !pFlyRadioInfo->bPreMute;
		}
		//设置波段
		if (pFlyRadioInfo->radioInfo.eCurRadioMode != pFlyRadioInfo->radioInfo.ePreRadioMode)
		{
			DBGD((TEXT("\r\nFlyRadio TEF6624 ScanThread-->change mode")));
			if (pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable)
			{
				while (IpcWhatEvent(EVENT_GLOBAL_RADIO_MUTE_IN_REQ_ID)
					|| IpcWhatEvent(EVENT_GLOBAL_RADIO_MUTE_OUT_REQ_ID))
				{
					Sleep(10);
				}
				IpcStartEvent(EVENT_GLOBAL_RADIO_MUTE_IN_REQ_ID);//发送进入静音
				while (!IpcWhatEvent(EVENT_GLOBAL_RADIO_MUTE_IN_OK_ID))//等待OK
				{
					Sleep(10);
				}
				IpcClearEvent(EVENT_GLOBAL_RADIO_MUTE_IN_OK_ID);//清除
			}

			pFlyRadioInfo->radioInfo.eCurRadioMode = pFlyRadioInfo->radioInfo.ePreRadioMode;			
			if (FM1 == pFlyRadioInfo->radioInfo.eCurRadioMode)
			{
				pFlyRadioInfo->radioInfo.pPreRadioFreq = &pFlyRadioInfo->radioInfo.iPreRadioFreqFM1;
				pFlyRadioInfo->radioInfo.pCurRadioFreq = &pFlyRadioInfo->radioInfo.iCurRadioFreqFM1;
			}
			else if (FM2 == pFlyRadioInfo->radioInfo.eCurRadioMode)
			{
				pFlyRadioInfo->radioInfo.pPreRadioFreq = &pFlyRadioInfo->radioInfo.iPreRadioFreqFM2;
				pFlyRadioInfo->radioInfo.pCurRadioFreq = &pFlyRadioInfo->radioInfo.iCurRadioFreqFM2;
			}
			else if (AM == pFlyRadioInfo->radioInfo.eCurRadioMode)
			{
				pFlyRadioInfo->radioInfo.pPreRadioFreq = &pFlyRadioInfo->radioInfo.iPreRadioFreqAM;
				pFlyRadioInfo->radioInfo.pCurRadioFreq = &pFlyRadioInfo->radioInfo.iCurRadioFreqAM;
			}
			TEF6624_ChangeToFMAM(pFlyRadioInfo,pFlyRadioInfo->radioInfo.eCurRadioMode);

			FlyRadioRDSInit(pFlyRadioInfo);

			if (pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable)
			{
				if (AM == pFlyRadioInfo->radioInfo.eCurRadioMode)
				{
					Sleep(314);
				}
				IpcStartEvent(EVENT_GLOBAL_RADIO_MUTE_OUT_REQ_ID);//发送退出静音
			}
		}
		//设置频率
		if (*pFlyRadioInfo->radioInfo.pCurRadioFreq != *pFlyRadioInfo->radioInfo.pPreRadioFreq)
		{
			DBGD((TEXT("\r\nFlyRadio TEF6624 ScanThread-->change freq")));
			
			//TEF6624_Mute(pFlyRadioInfo,TRUE);

			*pFlyRadioInfo->radioInfo.pCurRadioFreq = *pFlyRadioInfo->radioInfo.pPreRadioFreq;
			TEF6624_Set_Freq(pFlyRadioInfo,pFlyRadioInfo->radioInfo.eCurRadioMode,*pFlyRadioInfo->radioInfo.pCurRadioFreq);

			FlyRadioRDSInit(pFlyRadioInfo);
			if (pFlyRadioInfo->bPreMute == FALSE)
			{
				TEF6624_Mute(pFlyRadioInfo,FALSE);//取消静音
			}
		}
		if(pFlyRadioInfo->bPreMute != pFlyRadioInfo->bCurMute)//收音机静音开关
		{
			if (pFlyRadioInfo->bPreMute == TRUE)
			{
				TEF6624_Mute(pFlyRadioInfo,TRUE); // mute
			}
			else
			{
				TEF6624_Mute(pFlyRadioInfo,FALSE);// demute
			}
			pFlyRadioInfo->bCurMute = pFlyRadioInfo->bPreMute;
		}
		if (pFlyRadioInfo->radioInfo.bPreStepButtomDown)//按下，则持续跳台
		{
			if (0 == pFlyRadioInfo->radioInfo.iButtomStepCount)
			{
				pFlyRadioInfo->radioInfo.iButtomStepCount++;
				iThreadFreqStepWaitTime = 314;
			}
			else
			{
				buttomJumpFreqAndReturn(pFlyRadioInfo);
				pFlyRadioInfo->radioInfo.iButtomStepCount++;
				iThreadFreqStepWaitTime = 100;
			}
			continue;//跳到开头
		}

		nowTimer = GetTickCount();
		lastTimer = nowTimer;

		while(pFlyRadioInfo->radioInfo.bPreScaning)//搜索
		{
			DBGD((TEXT("\r\nFlyRadio TEF6624 ScanThread-->scaning")));
			nowTimer = GetTickCount();	

			if(FALSE == pFlyRadioInfo->bCurMute)
			{
				pFlyRadioInfo->bCurMute = TRUE;
				TEF6624_Mute(pFlyRadioInfo,TRUE); // mute	
			}
					
			if (pFlyRadioInfo->radioInfo.bCurScaning != pFlyRadioInfo->radioInfo.bPreScaning)//起始搜索频率
			{
				pFlyRadioInfo->radioInfo.bCurScaning = pFlyRadioInfo->radioInfo.bPreScaning;
				iScanStartFreq = GetCorrectScanStartFreq(pFlyRadioInfo,pFlyRadioInfo->radioInfo.pPreRadioFreq);
				bHaveSearched = FALSE;
				iHaveSearchedLevel = 0;
			}
			DBGD((TEXT("\r\nFlyRadio TEF6624 ScanThread iScanStartFreq-->%d,PreRadioFreq-->%d"),iScanStartFreq,*pFlyRadioInfo->radioInfo.pPreRadioFreq));
				
			*pFlyRadioInfo->radioInfo.pPreRadioFreq = RadioStepFreqGenerate(pFlyRadioInfo
				,pFlyRadioInfo->radioInfo.eCurRadioMode
				,*pFlyRadioInfo->radioInfo.pPreRadioFreq
				,pFlyRadioInfo->radioInfo.eScanDirection
				,STEP_SCAN);

			*pFlyRadioInfo->radioInfo.pCurRadioFreq = *pFlyRadioInfo->radioInfo.pPreRadioFreq;
			TEF6624_Set_Freq(pFlyRadioInfo,pFlyRadioInfo->radioInfo.eCurRadioMode,*pFlyRadioInfo->radioInfo.pCurRadioFreq);
			if (iScanStartFreq == *pFlyRadioInfo->radioInfo.pPreRadioFreq )//一圈都没好台
			{
				pFlyRadioInfo->radioInfo.bPreScaning = FALSE;
				pFlyRadioInfo->radioInfo.bCurScaning = TRUE;
				returnRadioFreq(pFlyRadioInfo,*pFlyRadioInfo->radioInfo.pPreRadioFreq);
				returnRadioScanCtrl(pFlyRadioInfo,0x03);//stop
				if (pFlyRadioInfo->bPreMute == FALSE)//收到台要出声音
				{
					TEF6624_Mute(pFlyRadioInfo,FALSE);
				}
			}
			if ((nowTimer - lastTimer) >  157)//定时返回频点
			{
				lastTimer = nowTimer;
				if (pFlyRadioInfo->radioInfo.bPreScaning)
				{
					returnRadioFreq(pFlyRadioInfo,*pFlyRadioInfo->radioInfo.pPreRadioFreq);
				}
			}
			if (AM != pFlyRadioInfo->radioInfo.eCurRadioMode)
			{
				Sleep(10);
			} 
			else
			{
				Sleep(138);
			}

			if (bRadioSignalGood(pFlyRadioInfo,pFlyRadioInfo->radioInfo.eCurRadioMode))
			{
				bHaveSearched = TRUE;
				if (iHaveSearchedLevel > pFlyRadioInfo->radioInfo.iSignalLevel)//OK
				{
					if (STEP_BACKWARD == pFlyRadioInfo->radioInfo.eScanDirection)
					{
						*pFlyRadioInfo->radioInfo.pPreRadioFreq = RadioStepFreqGenerate(pFlyRadioInfo
							,pFlyRadioInfo->radioInfo.eCurRadioMode
							,*pFlyRadioInfo->radioInfo.pPreRadioFreq
							,STEP_FORWARD
							,STEP_SCAN);
					} 
					else
					{
						*pFlyRadioInfo->radioInfo.pPreRadioFreq = RadioStepFreqGenerate(pFlyRadioInfo
							,pFlyRadioInfo->radioInfo.eCurRadioMode
							,*pFlyRadioInfo->radioInfo.pPreRadioFreq
							,STEP_BACKWARD
							,STEP_SCAN);
					}
					*pFlyRadioInfo->radioInfo.pCurRadioFreq = *pFlyRadioInfo->radioInfo.pPreRadioFreq;
					TEF6624_Set_Freq(pFlyRadioInfo,pFlyRadioInfo->radioInfo.eCurRadioMode,*pFlyRadioInfo->radioInfo.pCurRadioFreq);


					if (pFlyRadioInfo->bPreMute == FALSE)//收到台要出声音
					{
						TEF6624_Mute(pFlyRadioInfo,FALSE);
					}

					returnRadioFreq(pFlyRadioInfo,*pFlyRadioInfo->radioInfo.pPreRadioFreq);
					returnRadioHaveSearched(pFlyRadioInfo,TRUE);

					if (pFlyRadioInfo->radioInfo.bScanRepeatFlag == FALSE)
					{
						pFlyRadioInfo->radioInfo.bPreScaning = FALSE;
						pFlyRadioInfo->radioInfo.bCurScaning = FALSE;
						returnRadioScanCtrl(pFlyRadioInfo,0x03);//stop							
						break;
					}
					else
					{
						//blinking 5 times
						iBlinkingTimes = 0;
						bBlinkingStatus = TRUE;
						while (pFlyRadioInfo->radioInfo.bPreScaning == TRUE && iBlinkingTimes < 10)
						{
							Sleep(500);
							iBlinkingTimes++;
							bBlinkingStatus = !bBlinkingStatus;
							returnRadioBlinkingStatus(pFlyRadioInfo,bBlinkingStatus);							
						}
						returnRadioBlinkingStatus(pFlyRadioInfo,TRUE);
						Sleep(500);
						if (pFlyRadioInfo->radioInfo.bPreScaning == TRUE)
						{
							returnRadioScanCtrl(pFlyRadioInfo,0x04);//repeat
						}

						iScanStartFreq = *pFlyRadioInfo->radioInfo.pPreRadioFreq;
						bHaveSearched = FALSE;
						iHaveSearchedLevel = 0;
					}
				}				
			}
			else
			{
				if (bHaveSearched)
				{
					if (STEP_BACKWARD == pFlyRadioInfo->radioInfo.eScanDirection)
					{
						*pFlyRadioInfo->radioInfo.pPreRadioFreq = RadioStepFreqGenerate(pFlyRadioInfo
							,pFlyRadioInfo->radioInfo.eCurRadioMode
							,*pFlyRadioInfo->radioInfo.pPreRadioFreq
							,STEP_FORWARD
							,STEP_SCAN);
					} 
					else
					{
						*pFlyRadioInfo->radioInfo.pPreRadioFreq = RadioStepFreqGenerate(pFlyRadioInfo
							,pFlyRadioInfo->radioInfo.eCurRadioMode
							,*pFlyRadioInfo->radioInfo.pPreRadioFreq
							,STEP_BACKWARD
							,STEP_SCAN);
					}
					*pFlyRadioInfo->radioInfo.pCurRadioFreq = *pFlyRadioInfo->radioInfo.pPreRadioFreq;
					TEF6624_Set_Freq(pFlyRadioInfo,pFlyRadioInfo->radioInfo.eCurRadioMode,*pFlyRadioInfo->radioInfo.pCurRadioFreq);

					if (pFlyRadioInfo->bPreMute == FALSE)//收到台要出声音
					{
						TEF6624_Mute(pFlyRadioInfo,FALSE);
					}

					returnRadioFreq(pFlyRadioInfo,*pFlyRadioInfo->radioInfo.pPreRadioFreq);
					returnRadioHaveSearched(pFlyRadioInfo,TRUE);

					if (pFlyRadioInfo->radioInfo.bScanRepeatFlag == FALSE)
					{
						pFlyRadioInfo->radioInfo.bPreScaning = FALSE;
						pFlyRadioInfo->radioInfo.bCurScaning = FALSE;
						returnRadioScanCtrl(pFlyRadioInfo,0x03);//stop							
						break;
					}
					else
					{
						//blinking 5 times
						iBlinkingTimes = 0;
						bBlinkingStatus = TRUE;
						while (pFlyRadioInfo->radioInfo.bPreScaning == TRUE && iBlinkingTimes < 10)
						{
							Sleep(500);
							iBlinkingTimes++;
							bBlinkingStatus = !bBlinkingStatus;
							returnRadioBlinkingStatus(pFlyRadioInfo,bBlinkingStatus);						
						}
						returnRadioBlinkingStatus(pFlyRadioInfo,TRUE);
						Sleep(500);
						if (pFlyRadioInfo->radioInfo.bPreScaning == TRUE)
						{
							returnRadioScanCtrl(pFlyRadioInfo,0x04);//repeat
						}

						iScanStartFreq = *pFlyRadioInfo->radioInfo.pPreRadioFreq;
						bHaveSearched = FALSE;
						iHaveSearchedLevel = 0;
					}
				}
			}
			iHaveSearchedLevel = pFlyRadioInfo->radioInfo.iSignalLevel;
		}	
		DBGD((TEXT("\r\nFlyRadio TEF6624 ScanThread Running End")));
	}
	DBGD((TEXT("\r\nFlyRadio TEF6624 ScanThread exit!")));
	pFlyRadioInfo->FlyRadioScanThreadHandle = NULL;
	return 0;
}

//数据初始化(创建线程 打开需要用到的程动 创建事件)
VOID FlyRadioDriverEnable(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BOOL bOn)
{
	DWORD dwThreadID;

	DBGE((TEXT("\r\nFlyRadio TEF6624 Enable-->%d"),bOn));

	if(bOn)
	{
		pFlyRadioInfo->bKillDispatchMainThread = FALSE;
		pFlyRadioInfo->hDispatchMainThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		pFlyRadioInfo->FlyRadioMainThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)FlyRadioMainThread, //线程的全局函数
			pFlyRadioInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		if (NULL == pFlyRadioInfo->FlyRadioMainThreadHandle)
		{
			pFlyRadioInfo->bKillDispatchMainThread = TRUE;
			return;
		}
		DBGE((TEXT("\r\nFlyRadio TEF6624 MainThread ID-->%x!"),dwThreadID));

		pFlyRadioInfo->bKillDispatchScanThread = FALSE;
		pFlyRadioInfo->hDispatchScanThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		pFlyRadioInfo->FlyRadioScanThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)FlyRadioScanThread, //线程的全局函数
			pFlyRadioInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		if (NULL == pFlyRadioInfo->FlyRadioScanThreadHandle)
		{
			pFlyRadioInfo->bKillDispatchScanThread = TRUE;
			return;
		}
		DBGE((TEXT("\r\nFlyRadio TEF6624 ScanThread ID-->%x!"),dwThreadID));

		if (!CreateTEF6624RDSThread(pFlyRadioInfo))
		{
			DBGE((TEXT("\r\nFlyRadio TEF6624 Create RDS Thread ERR!!")));
			return;
		}

//#if RADIO_RDS
//		MSGQUEUEOPTIONS  msgOpts;
//		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
//		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
//		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_RADIO_ENTRIES;
//		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_RADIO_LENGTH;
//		msgOpts.bReadAccess = TRUE;
//		pFlyRadioInfo->hMsgQueueToRadioCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_RADIO_NAME, &msgOpts);
//		if (NULL == pFlyRadioInfo->hMsgQueueToRadioCreate)
//		{
//			DBGE((TEXT("\r\nFlyRadio TEF6624 Create MsgQueue To Radio Fail!")));
//		}
//
//		pFlyRadioInfo->bKillDispatchRDSRecThread = FALSE;
//		pFlyRadioInfo->FlyRadioRDSRecThreadHandle = 
//			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
//			0,//初始化线程栈的大小，缺省为与主线程大小相同
//			(LPTHREAD_START_ROUTINE)FlyRadioRDSRecThread, //线程的全局函数
//			pFlyRadioInfo, //此处传入了主框架的句柄
//			0, &dwThreadID );
//		if (NULL == pFlyRadioInfo->FlyRadioRDSRecThreadHandle)
//		{
//			pFlyRadioInfo->bKillDispatchRDSRecThread = TRUE;
//			return;
//		}
//		DBGE((TEXT("\r\nFlyRadio TEF6624 RDSRecThread ID %x!"),dwThreadID));
//#endif

		pFlyRadioInfo->hBuffToUserDataEvent = CreateEvent(NULL,FALSE,FALSE,NULL);	
		InitializeCriticalSection(&pFlyRadioInfo->hCSSendToUser);

	}
	else
	{
		pFlyRadioInfo->bKillDispatchScanThread = TRUE;
		SetEvent(pFlyRadioInfo->hDispatchScanThreadEvent);		
		pFlyRadioInfo->bKillDispatchMainThread = TRUE;
		SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);

		while (pFlyRadioInfo->FlyRadioMainThreadHandle)
		{
			SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
			Sleep(10);
		}
		CloseHandle(pFlyRadioInfo->hDispatchMainThreadEvent);
		CloseHandle(pFlyRadioInfo->hDispatchScanThreadEvent);

//#if RADIO_RDS
//		pFlyRadioInfo->bKillDispatchRDSRecThread = TRUE;
//		while (pFlyRadioInfo->FlyRadioRDSRecThreadHandle)
//		{
//			SetEvent(pFlyRadioInfo->hMsgQueueToRadioCreate);
//			Sleep(10);
//		}
//		CloseMsgQueue(pFlyRadioInfo->hMsgQueueToRadioCreate);
//#endif

		CloseHandle(pFlyRadioInfo->hBuffToUserDataEvent);
	}
}

//初始化结构体里的主要数据
static void FlyRadioInitMainStruct(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
	DBGE((TEXT("\r\nFlyRadio TEF6624 Init Main Struct")));
	pFlyRadioInfo->bOpen = FALSE;
	pFlyRadioInfo->bPowerUp = FALSE;
	pFlyRadioInfo->bNeedInit = TRUE;

	pFlyRadioInfo->bPreMute = FALSE;
	pFlyRadioInfo->bCurMute = FALSE;

	pFlyRadioInfo->buffToUserHx = 0;
	pFlyRadioInfo->buffToUserLx = 0;
	memset(pFlyRadioInfo->buffToUser,0,DATA_BUFF_LENGTH);

	pFlyRadioInfo->bKillDispatchMainThread = TRUE;
	pFlyRadioInfo->hDispatchMainThreadEvent = NULL;
	pFlyRadioInfo->FlyRadioMainThreadHandle = NULL;

	pFlyRadioInfo->bKillDispatchScanThread = TRUE;
	pFlyRadioInfo->FlyRadioScanThreadHandle = NULL;
	pFlyRadioInfo->hDispatchScanThreadEvent = NULL;

	pFlyRadioInfo->bKillDispatchRDSRecThread = TRUE;
	pFlyRadioInfo->FlyRadioRDSRecThreadHandle = NULL;

	pFlyRadioInfo->FlyRadioI2CHandle = NULL;
}

//初始化radioInfo结构体
static void FlyRadioInitRadioInfoStruct(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
	DBGE((TEXT("\r\nFlyRadio TEF6624 Init radioInfo Struct")));
	FlyRadioIDChange(pFlyRadioInfo,pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser);

	pFlyRadioInfo->radioInfo.ePreRadioMode = FM1;
	pFlyRadioInfo->radioInfo.eCurRadioMode = FM1;

	pFlyRadioInfo->radioInfo.pPreRadioFreq = &pFlyRadioInfo->radioInfo.iPreRadioFreqFM1;
	pFlyRadioInfo->radioInfo.pCurRadioFreq = &pFlyRadioInfo->radioInfo.iCurRadioFreqFM1;

	pFlyRadioInfo->radioInfo.bPreScaning = FALSE;
	pFlyRadioInfo->radioInfo.bCurScaning = FALSE;
	pFlyRadioInfo->radioInfo.eScanDirection = STEP_FORWARD;
	pFlyRadioInfo->radioInfo.bScanRepeatFlag = FALSE;

	pFlyRadioInfo->radioInfo.bPreStepButtomDown = FALSE;
	pFlyRadioInfo->radioInfo.eButtomStepDirection = STEP_FORWARD;
	pFlyRadioInfo->radioInfo.iButtomStepCount = 0;

	pFlyRadioInfo->RDSInfo.RadioRDSAFControlOn = FALSE;
	pFlyRadioInfo->RDSInfo.RadioRDSTAControlOn = FALSE;

	pFlyRadioInfo->radioInfo.iSignalLevel = 0;
	pFlyRadioInfo->radioInfo.iSignalNoise = 0;
	pFlyRadioInfo->radioInfo.itef6624_writemode = TEF6624_WRITE_MODE_PRESET;
}

void IpcRecv(UINT32 iEvent)
{
	P_FLYRADIO_TEF6624_INFO	pFlyRadioInfo = (P_FLYRADIO_TEF6624_INFO)gpFlyRadioInfo;

	DBGD((TEXT("\r\nFLY6624Radio Recv IPC iEvent:%d\r\n"),iEvent));

	SetEvent(pFlyRadioInfo->hDispatchMainThreadEvent);
}
//void SockRecv(BYTE *buf, UINT16 len)
//{
//	P_FLY_TDA7541_RADIO_INFO	pFlyRadioInfo = (P_FLY_TDA7541_RADIO_INFO)gpTDA7541RadioInfo;
//}

//流驱动接口
HANDLE FRA_Init(DWORD dwContext)
{
	P_FLYRADIO_TEF6624_INFO pFlyRadioInfo;

	RETAILMSG(1, (TEXT("\r\nFlyRadio TEF6624 Driver Init Start")));

	pFlyRadioInfo = (P_FLYRADIO_TEF6624_INFO)LocalAlloc(LPTR, sizeof(FLYRADIO_TEF6624_INFO));
	if (!pFlyRadioInfo)
	{
		return NULL;
	}
	gpFlyRadioInfo = pFlyRadioInfo;

	pFlyRadioInfo->pFlyShmGlobalInfo = CreateShm(RADIO_MODULE,IpcRecv);
	if (NULL == pFlyRadioInfo->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio 6624RADIO Init create shm err\r\n")));
		return NULL;
	}

	if (!GetDllAddrTable())
	{
		DBGE((TEXT("FlyAudio 6624RADIO  GetDllAddrTable err\r\n")));
		return NULL;
	}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio 6624RADIO Open create user buf err\r\n")));
		return NULL;
	}
	//if(!CreateServerSocket(SockRecv, TCP_PORT_RADIO))
	//{
	//	DBGE((TEXT("FlyAudio 6624RADIO Open create server socket err\r\n")));
	//	return NULL;
	//}



	if (pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalStructSize != sizeof(struct shm))
	{
		pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[0] = 'F';
		pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[1] = 'R';
		pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[2] = 'A';
	}

	////参数初始化
	FlyRadioInitMainStruct(pFlyRadioInfo);
	FlyRadioInitRadioInfoStruct(pFlyRadioInfo);
	memset(&pFlyRadioInfo->sTEF6624RDSInfo,0,sizeof(FLY_TEF6624_RDS_INFO));

	//pFlyRadioInfo->hHandleGlobalGlobalEvent = CreateEvent(NULL,FALSE,FALSE,DATA_GLOBAL_HANDLE_GLOBAL);
	pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.bInit = TRUE;
	pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.iDriverCompYear = year;
	pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.iDriverCompMon = months;
	pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.iDriverCompDay = day;
	pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.iDriverCompHour = hours;
	pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.iDriverCompMin = minutes;
	pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.iDriverCompSec = seconds;

	DBGI((TEXT("\r\nFlyRadio TEF6624 Init Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));

	return (HANDLE)pFlyRadioInfo;
}

BOOL FRA_Deinit(DWORD hDeviceContext)
{
	P_FLYRADIO_TEF6624_INFO	pFlyRadioInfo = (P_FLYRADIO_TEF6624_INFO)hDeviceContext;
	DBGE((TEXT("\r\nFlyRadio TEF6624 DeInit")));

	pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.bInit = FALSE;
	//SetEvent(pFlyRadioInfo->hHandleGlobalGlobalEvent);
	//CloseHandle(pFlyRadioInfo->hHandleGlobalGlobalEvent);

	FreeShm();
	//FreeSocketServer();
	FreeUserBuff();

	LocalFree(pFlyRadioInfo);
	return TRUE;
}

DWORD FRA_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	P_FLYRADIO_TEF6624_INFO	pFlyRadioInfo = (P_FLYRADIO_TEF6624_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;

	DBGE((TEXT("\r\nFlyRadio TEF6624 Open")));

	if(pFlyRadioInfo->bOpen)
	{
		return NULL;
	}
	pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.bOpen = TRUE;
	//SetEvent(pFlyRadioInfo->hHandleGlobalGlobalEvent);

	FlyRadioDriverEnable(pFlyRadioInfo,TRUE);

	pFlyRadioInfo->bOpen = TRUE;

	return returnWhat;
}

BOOL FRA_Close(DWORD hDeviceContext)
{
	P_FLYRADIO_TEF6624_INFO	pFlyRadioInfo = (P_FLYRADIO_TEF6624_INFO)hDeviceContext;

	FlyRadioDriverEnable(pFlyRadioInfo,FALSE);

	pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.bOpen = FALSE;
	//SetEvent(pFlyRadioInfo->hHandleGlobalGlobalEvent);

	SetEvent(pFlyRadioInfo->hBuffToUserDataEvent);

	pFlyRadioInfo->bOpen = FALSE;

	DBGE((TEXT("\r\nFlyRadio TEF6624 Close")));
		
	return TRUE;
}

VOID FRA_PowerUp(DWORD hDeviceContext)
{
	P_FLYRADIO_TEF6624_INFO	pFlyRadioInfo = (P_FLYRADIO_TEF6624_INFO)hDeviceContext;

	DBGE((TEXT("\r\nFlyRadio TEF6624 PowerUp")));
}

VOID FRA_PowerDown(DWORD hDeviceContext)
{
	P_FLYRADIO_TEF6624_INFO	pFlyRadioInfo = (P_FLYRADIO_TEF6624_INFO)hDeviceContext;

	pFlyRadioInfo->bPowerUp = FALSE;

	FlyRadioRDSInit(pFlyRadioInfo);

	pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNeedProcVoltageShakeRadio = 0;

	DBGE((TEXT("\r\nFlyRadio TEF6624 PowerDown")));
}

DWORD FRA_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	P_FLYRADIO_TEF6624_INFO	pFlyRadioInfo = (P_FLYRADIO_TEF6624_INFO)hOpenContext;
	UINT dwRead = 0,i;
	BYTE *buf = (BYTE *)pBuffer;
	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio 6624RADIO return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}
	return dwRead;
}

DWORD FRA_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	P_FLYRADIO_TEF6624_INFO	pFlyRadioInfo = (P_FLYRADIO_TEF6624_INFO)hOpenContext;
	BYTE *p= (BYTE *)pSourceBytes;
	DBGD((TEXT("\r\nFlyRadio TEF6624 Write"),NumberOfBytes));
	for(UINT i = 0;i < NumberOfBytes;i++)
	{
		DBGD((TEXT(" %X "),*(p + i)));
	}

	if(NumberOfBytes >= 5)
	{
		FlyRadioDealWinceCmd(pFlyRadioInfo,&p[3],NumberOfBytes-4);
	}
	return NULL;
}

DWORD FRA_Seek(DWORD hOpenContext, LONG Amount, DWORD  Type)
{
	return TRUE;
}

BOOL FRA_IOControl(DWORD hOpenContext,
				   DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn,
				   PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
	P_FLYRADIO_TEF6624_INFO	pFlyRadioInfo = (P_FLYRADIO_TEF6624_INFO)hOpenContext;
	BOOL bRes = TRUE;

	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	DBGD((TEXT("\r\nFlyRadio TEF6624 IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)"),
	//		hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut);
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	DBGD((TEXT("\r\nFlyRadio TEF6624 IOControl Set IOCTL_SERIAL_SET_WAIT_MASK"));
	//		break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	DBGD((TEXT("\r\nFlyRadio TEF6624 IOControl Set IOCTL_SERIAL_WAIT_ON_MASK Enter"));
	//		if (pFlyRadioInfo->bOpen)
	//		{
	//			WaitForSingleObject(pFlyRadioInfo->hBuffToUserDataEvent,INFINITE);
	//		} 
	//		else
	//		{
	//			WaitForSingleObject(pFlyRadioInfo->hBuffToUserDataEvent,0);
	//		}
	//		if ((dwLenOut < sizeof(DWORD)) || (NULL == pBufOut) ||
	//			(NULL == pdwActualOut))
	//		{
	//			bRes = FALSE;
	//			break;
	//		}
	//		*(DWORD *)pBufOut = EV_RXCHAR;
	//		*pdwActualOut = sizeof(DWORD);
	//		DBGD((TEXT("\r\nFlyRadio TEF6624 IOControl Set IOCTL_SERIAL_WAIT_ON_MASK Exit"));
	//			break;
	//default :
	//	break;
	//}

	return TRUE;
}


