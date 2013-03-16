#include "flyhwlib.h"
#include "flyuserlib.h"
#include "flysocketlib.h"
#include "FLY7741Radio.h"
#include "FLY7741Radio_Data.h"

P_FLY_SAF7741_RADIO_INFO	gpSAF7741RadioInfo = NULL;

static void radioIDChangePara(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BYTE iID);
VOID Fly7741RadioEnable(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BOOL bOn);
void FlyRadio_SAF7741_TEF7000_Init(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo);
void SAF7741RadioJumpNewFreqParaInit(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo);
static BOOL bRadioSignalGood(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,enumRadioMode radioMode,UINT *pLevel);
VOID
FRA_PowerUp(DWORD hDeviceContext);
VOID
FRA_PowerDown(DWORD hDeviceContext);
      
#define SAF7741_I2C_FREQ	50000 
#define TEF7000_I2C_FREQ	50000
#define NVM_I2C_FREQ	    50000
#define NORMAL_I2C_FREQ		50000  


/*
IIC_SEG_MASK_SEND_START		= 1 << 0,
IIC_SEG_MASK_NEED_ACK		= 1 << 1,
IIC_SEG_SEQ_DEVADDR_W		= 1 << 2,
IIC_SEG_SEQ_DEVADDR_R		= 1 << 3,
IIC_SEG_SEQ_REGADDR			= 1 << 4,
IIC_SEG_SEQ_DATA			= 1 << 5,
IIC_SEG_SEQ_MASK_SEND_STOP  = 1 << 6,
*/
//IIC读写时序
const IIC_SEG_T I2CWriteSeq_TEF7000_1[6]={
	{IIC_SEG_MASK_SEND_START|IIC_SEG_SEQ_DEVADDR_W|IIC_SEG_MASK_NEED_ACK,(TEF7000_1_ADDR_W&0xFF)>>1},//写7741-0X38地址
	{IIC_SEG_SEQ_REGADDR|IIC_SEG_MASK_NEED_ACK,0x00},
	{IIC_SEG_SEQ_REGADDR|IIC_SEG_MASK_NEED_ACK,0xFF},
	{IIC_SEG_SEQ_REGADDR|IIC_SEG_MASK_NEED_ACK,0xFF},//写7741里0X00FFFF寄存器地址
	{IIC_SEG_MASK_SEND_START|IIC_SEG_SEQ_DEVADDR_W|IIC_SEG_MASK_NEED_ACK,((TEF7000_1_ADDR_W>>8)&0xFF)>>1},//写7000_1 C0地址
	{IIC_SEG_SEQ_REGADDR|IIC_SEG_MASK_NEED_ACK,0x00}//写7000_1里的寄存器地址
};
const IIC_SEG_T I2CWriteSeq_TEF7000_2[6]={
	{IIC_SEG_MASK_SEND_START|IIC_SEG_SEQ_DEVADDR_W|IIC_SEG_MASK_NEED_ACK,(TEF7000_2_ADDR_W&0xFF)>>1},//写7741-0X38地址
	{IIC_SEG_SEQ_REGADDR|IIC_SEG_MASK_NEED_ACK,0x00},
	{IIC_SEG_SEQ_REGADDR|IIC_SEG_MASK_NEED_ACK,0xFF},
	{IIC_SEG_SEQ_REGADDR|IIC_SEG_MASK_NEED_ACK,0xFF},//写7741里0X00FFFF寄存器地址
	{IIC_SEG_MASK_SEND_START|IIC_SEG_SEQ_DEVADDR_W|IIC_SEG_MASK_NEED_ACK,((TEF7000_2_ADDR_W>>8)&0xFF)>>1},//写7000_1 C0地址
	{IIC_SEG_SEQ_REGADDR|IIC_SEG_MASK_NEED_ACK,0x00}//写7000_2里的寄存器地址
};

#define IIC_DATA_MASK_SEQ (IIC_SEG_SEQ_DATA|IIC_SEG_MASK_NEED_ACK) //写7000_1里的寄存器地址
#define IIC_STOP_MASK_SEQ (IIC_SEG_SEQ_DATA|IIC_SEG_MASK_NEED_NACK|IIC_SEG_SEQ_MASK_SEND_STOP) //写数据并停止



static void RegDataReadRadio(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo)
{
	HKEY hKey; 
	DWORD dwDisposition;
	DWORD dwTemp;
	DWORD dwLen;
	DWORD dwType;


#if REGEDIT_NOT_READ
	return;
#endif

	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Reg Read Start")));

		//读取收音机频点参数
		RegCreateKeyEx(HKEY_CURRENT_USER,L"FlyDrivers\\Radio",0,NULL,0,
		0,NULL,&hKey,&dwDisposition); /////////////////////////////////创建或打开注册表

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwFreqFM1",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//FM1
	{
		pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1 = dwTemp;
		if (pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1 > 10800 || pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1 < 8750)
		{
			pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1 = pSAF7741RadioInfo->radioInfo.iFreqFMMin;
		}
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwFreqFM2",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//FM2
	{
		pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2 = dwTemp;
		if (pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2 > 10800 || pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2 < 8750)
		{
			pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2 = pSAF7741RadioInfo->radioInfo.iFreqFMMin;
		}
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwFreqAM",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//AM
	{
		pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM = dwTemp;
		if (pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM > 10800 || pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM < 8750)
		{
			pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM = pSAF7741RadioInfo->radioInfo.iFreqAMMin;
		}
	}

	RegCloseKey(hKey);	
	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Reg Read Finish %d %d %d"),pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1,pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2,pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM));
}

static void RegDataWriteRadio(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo)
{
	HKEY hKey; 
	DWORD dwDisposition;
	DWORD dwTemp;
	DWORD dwLen;

#if REGEDIT_NOT_SAVE
	return;
#endif

	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Reg Write Start")));

		//读取方向盘学习参数
		RegCreateKeyEx(HKEY_CURRENT_USER,L"FlyDrivers\\Radio",0,NULL,0,
		0,NULL,&hKey,&dwDisposition); /////////////////////////////////创建或打开注册表

	dwLen = 4;
	dwTemp = pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1;
	RegSetValueEx(hKey,L"dwFreqFM1",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);

	dwLen = 4;
	dwTemp = pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2;
	RegSetValueEx(hKey,L"dwFreqFM2",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);

	dwLen = 4;
	dwTemp = pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM;
	RegSetValueEx(hKey,L"dwFreqAM",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);

	RegCloseKey(hKey);
	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Reg Write Finish %d %d %d"),pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1,pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2,pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM));
}

/*
static BOOL I2C_Write_Normal(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo, ULONG ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
{
	if(!pSAF7741RadioInfo->bOpen)
	{
		return TRUE;
	}

	i2ccfg_t i2c_cfg;
	i2c_cfg.bus_id = 1;
	i2c_cfg.chip_addr = (char)NORMAL_ADDR_W;
	i2c_cfg.sub_addr  = ulRegAddr;
	i2c_cfg.flag      = I2C_WRITE;
	i2c_cfg.msg_buf   = (char*)pRegValBuf;
	i2c_cfg.msg_size  = uiValBufLen;

	HwI2CWrite(&i2c_cfg);


	DBGD((TEXT("\r\nFlyRadio SAF7741Radio:I2C write 0x%2X"),ulRegAddr));
	for (UINT i = 0;i < uiValBufLen;i++)
	{
		DBGD((TEXT(" 0x%X"),pRegValBuf[i]));
	}

	return TRUE;
}

static BOOL I2C_Read_Normal(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo, ULONG ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
{
	if (!pSAF7741RadioInfo->bOpen)
	{
		return TRUE;
	}

	i2ccfg_t i2c_cfg;
	i2c_cfg.bus_id = 1;
	i2c_cfg.chip_addr = (char)NORMAL_ADDR_R;
	i2c_cfg.sub_addr  = ulRegAddr;
	i2c_cfg.flag      = I2C_READ;
	i2c_cfg.msg_buf   = (char*)pRegValBuf;
	i2c_cfg.msg_size  = uiValBufLen;

	HwI2CWrite(&i2c_cfg);

	DBGI((TEXT("\r\nFlyRadio SAF7741(Normal):I2C read Addr-->%X,Length-->%X,Value-->%X"),ulRegAddr,uiValBufLen));
	for(UINT i = 0;i < uiValBufLen;i++)
	{
		DBGI((TEXT(" %X "),pRegValBuf[i]));
	}
	DBGI((TEXT("")));
	return TRUE;
}
*/
UINT SAF7741CreateI2CRegAddr(UINT addr)
{
	//UINT regAddr;
	//UINT regAddr1,regAddr2,regAddr3;

	//regAddr1 = addr & 0xff;
	//regAddr2 = (addr & 0xff00) >> 8;
	//regAddr3 = (addr & 0xff0000) >> 16;

	//regAddr = (regAddr1 << 16) + (regAddr2 << 8) + regAddr3;
	//return regAddr;

	return addr;
}

static BOOL I2C_Write_TEF7000_1(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo, ULONG ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
{
	if (!pSAF7741RadioInfo->bOpen)
	{
		return TRUE;
	}
	if (uiValBufLen >= DATA_BUF_LEN)
	{
		DBGE((TEXT("I2C_Write_TEF7000_1 buf len is large\r\n")));
		return FALSE;
	}

	UINT i=0;
	memcpy(&pSAF7741RadioInfo->I2c_TEF7000_1_DataBuf[0],I2CWriteSeq_TEF7000_1,sizeof(I2CWriteSeq_TEF7000_1));
	pSAF7741RadioInfo->I2c_TEF7000_1_DataBuf[5].uData = ((BYTE)ulRegAddr)&0xFF;
	for (i=0; i<uiValBufLen-1; i++)
	{
		pSAF7741RadioInfo->I2c_TEF7000_1_DataBuf[6+i].uSeg = IIC_DATA_MASK_SEQ;
		pSAF7741RadioInfo->I2c_TEF7000_1_DataBuf[6+i].uData = pRegValBuf[i];
	}
	pSAF7741RadioInfo->I2c_TEF7000_1_DataBuf[6+i].uSeg = IIC_DATA_MASK_SEQ;
	pSAF7741RadioInfo->I2c_TEF7000_1_DataBuf[6+i].uData = pRegValBuf[i];
	
	HwI2CSpecialClkWrite(0x1E/*100K*/,(BYTE *)pSAF7741RadioInfo->I2c_TEF7000_1_DataBuf, sizeof(IIC_SEG_T)*(6+i+1));

	//DBGI((TEXT("\r\nFlyAudio TEF7000_1-I2C write ulRegAddr:")));
	//for(UINT i = 0;i < sizeof(IIC_SEG_T)*(6+i+1);i++)
	//{
	//	DBGI((TEXT(" [%X %X] "),pSAF7741RadioInfo->I2c_TEF7000_1_DataBuf[i].uSeg,
	//		pSAF7741RadioInfo->I2c_TEF7000_1_DataBuf->uData));
	//}
	//DBGI((TEXT("")));

	return TRUE;
}

static BOOL I2C_Write_TEF7000_2(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo, ULONG ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
{
	if (!pSAF7741RadioInfo->bOpen)
	{
		return TRUE;
	}

	if (uiValBufLen >= DATA_BUF_LEN)
	{
		DBGE((TEXT("I2C_Write_TEF7000_2 buf len is large\r\n")));
		return FALSE;
	}

	UINT i=0;
	memcpy(&pSAF7741RadioInfo->I2c_TEF7000_2_DataBuf[0],I2CWriteSeq_TEF7000_2,sizeof(I2CWriteSeq_TEF7000_2));
	pSAF7741RadioInfo->I2c_TEF7000_2_DataBuf[5].uData = ((BYTE)ulRegAddr)&0xFF;
	for (i=0; i<uiValBufLen-1; i++)
	{
		pSAF7741RadioInfo->I2c_TEF7000_2_DataBuf[6+i].uSeg = IIC_DATA_MASK_SEQ;
		pSAF7741RadioInfo->I2c_TEF7000_2_DataBuf[6+i].uData = pRegValBuf[i];
	}
	pSAF7741RadioInfo->I2c_TEF7000_2_DataBuf[6+i].uSeg = IIC_DATA_MASK_SEQ;
	pSAF7741RadioInfo->I2c_TEF7000_2_DataBuf[6+i].uData = pRegValBuf[i];

	HwI2CSpecialClkWrite(0x1E/*100K*/,(BYTE *)pSAF7741RadioInfo->I2c_TEF7000_2_DataBuf, sizeof(IIC_SEG_T)*(6+i+1));



	//DBGI((TEXT("\r\nFlyAudio TEF7000_2:I2C write ulRegAddr-->%X "),iTemp));
	//for(UINT i = 0;i < uiValBufLen;i++)
	//{
	//	DBGI((TEXT(" %X "),pRegValBuf[i]));
	//}
	//DBGI((TEXT("")));

	return TRUE;
}

static BOOL I2C_Write_SAF7741(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo, ULONG ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
{
	if (!pSAF7741RadioInfo->bOpen)
	{
		return TRUE;
	}

	i2ccfg_t i2c_cfg;
	i2c_cfg.bus_id = 1;
	i2c_cfg.chip_addr = SAF7741_ADDR_W;
	i2c_cfg.sub_addr  = ulRegAddr;
	i2c_cfg.addr_type = IIC_24_BIT;
	i2c_cfg.clk_div   = 0x1E;  //clk = 3M/1E = 100kHZ
	i2c_cfg.flag      = I2C_WRITE;
	i2c_cfg.msg_buf   = (char*)pRegValBuf;
	i2c_cfg.msg_size  = uiValBufLen;

	HwI2CWrite(&i2c_cfg);

	//DBGI((TEXT("\r\nFlyRadio SAF7741:I2C write-->%X "),SAF7741CreateI2CRegAddr(ulRegAddr)));
	//for(UINT i = 0;i < uiValBufLen;i++)
	//{
	//	DBGI((TEXT(" %X "),pRegValBuf[i]));
	//}
	//DBGI((TEXT("")));

	return TRUE;
}
static BOOL I2C_Read_SAF7741(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo, ULONG ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
{
	if (!pSAF7741RadioInfo->bOpen)
	{
		return TRUE;
	}

	i2ccfg_t i2c_cfg;
	i2c_cfg.bus_id = 1;
	i2c_cfg.chip_addr = SAF7741_ADDR_R;
	i2c_cfg.sub_addr  = ulRegAddr;
	i2c_cfg.addr_type = IIC_24_BIT;
	i2c_cfg.clk_div   = 0x1E;  //clk = 3M/1E = 100kHZ
	i2c_cfg.flag      = I2C_READ;
	i2c_cfg.msg_buf   = (char*)pRegValBuf;
	i2c_cfg.msg_size  = uiValBufLen;

	HwI2CRead(&i2c_cfg);

	return TRUE;
}
static void SendToSAF7741NormalWriteData(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BYTE *pData)
{
	BYTE MChipAdd;
	UINT iLength;
	UINT dataCnt = 0;
	UINT regAddr;

	while (*pData)
	{
		dataCnt++;
		MChipAdd = *pData++;
		if(MChipAdd != SAF7741_ADDR_W)break;
		iLength = *pData++;
		if(pData[0] == 0x00 && pData[1] == 0xFF && pData[2] == 0xFF)
		{
			pData += iLength;
			continue;
		}
		else
		{
			regAddr = (pData[0] << 16) + (pData[1] << 8) + pData[2];
			I2C_Write_SAF7741(pSAF7741RadioInfo,SAF7741CreateI2CRegAddr(regAddr),&pData[3],iLength - 3);
			//DBGI((TEXT("\r\nFlyAudio SAF7730(7741):I2C write regAdd:%X,%d"),regAddr,iLength - 3));
			Sleep(2);
			pData += iLength;
		}
	}
}

static VOID SAF7741RadioPutToBuff(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BYTE data)
{
	pSAF7741RadioInfo->buffToUser[pSAF7741RadioInfo->buffToUserHx++] = data;
	pSAF7741RadioInfo->buffToUserHx %= DATA_BUFF_LENGTH;
}
VOID SAF7741RadioReturnToUser(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BYTE *buf,UINT len)
{
	BYTE crc;
	UINT i;
	EnterCriticalSection(&pSAF7741RadioInfo->hCSSendToUser);
	SAF7741RadioPutToBuff(pSAF7741RadioInfo,0xFF);
	SAF7741RadioPutToBuff(pSAF7741RadioInfo,0x55);
	SAF7741RadioPutToBuff(pSAF7741RadioInfo,len+1);
	crc = len + 1;
	for(i = 0;i < len;i++)
	{
		SAF7741RadioPutToBuff(pSAF7741RadioInfo,buf[i]);
		crc += buf[i];
	}
	SAF7741RadioPutToBuff(pSAF7741RadioInfo,crc);
	SetEvent(pSAF7741RadioInfo->hBuffToUserDataEvent);
	LeaveCriticalSection(&pSAF7741RadioInfo->hCSSendToUser);
}

void returnRadioPowerMode(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BOOL bOn)
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
void returnRadioInitStatus(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BOOL bOn)
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

void returnRadioFreq(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,UINT iFreq)
{
	BYTE buf[] = {0x10,0x00,0x00};

	buf[1] = iFreq >> 8;
	buf[2] = iFreq;

	ReturnToUser(buf,3);
}
void returnRadioScanCtrl(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BYTE cmd)
{
	BYTE buf[] = {0x13,0x00};

	buf[1] = cmd;

	ReturnToUser(buf,2);
}
void returnRadioHaveSearched(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BOOL bHave)
{
	BYTE buf[] = {0x14,0x00};

	if (bHave)
	{
		buf[1] = 0;
	}

	ReturnToUser(buf,2);
}

void returnRadioMuteStatus(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BOOL bOn)
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
void returnRadioAFStatus(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BOOL bOn)
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


void returnRadioTAStatus(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BOOL bOn)
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
void returnRadioBlinkingStatus(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BOOL bOn)
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
void returnRadioMode(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,enumRadioMode eMode)
{
	BYTE buf[] = {0x20,0x00};

	buf[1] = eMode;

	ReturnToUser(buf,2);
}

void returnRadioRDSWorkStatus(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BOOL bOn)
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

static UINT RadioStepFreqGenerate(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,enumRadioMode eMode,UINT iFreq,enumRadioStepDirection eForward,enumRadioStepMode eStepMode)
{
	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Freq Generate Mode-->%d,Freq-->%d,Forward-->%d,StepMode-->%d"),eMode,iFreq,eForward,eStepMode));
	if (STEP_FORWARD == eForward)//Forward
	{	
		if(AM != eMode)
		{
			if(STEP_MANUAL == eStepMode)//手动
			{
				iFreq += pSAF7741RadioInfo->radioInfo.iFreqFMManualStep;
			}
			else//自动
			{
				iFreq += pSAF7741RadioInfo->radioInfo.iFreqFMScanStep;
			}
			if(iFreq > pSAF7741RadioInfo->radioInfo.iFreqFMMax)	iFreq = pSAF7741RadioInfo->radioInfo.iFreqFMMin;
			if(iFreq < pSAF7741RadioInfo->radioInfo.iFreqFMMin)	iFreq = pSAF7741RadioInfo->radioInfo.iFreqFMMin;

		}
		else
		{
			if(STEP_MANUAL == eStepMode)//手动
			{
				iFreq += pSAF7741RadioInfo->radioInfo.iFreqAMManualStep;
			}
			else//自动
			{
				iFreq += pSAF7741RadioInfo->radioInfo.iFreqAMScanStep;
			}
			if(iFreq > pSAF7741RadioInfo->radioInfo.iFreqAMMax)	iFreq = pSAF7741RadioInfo->radioInfo.iFreqAMMin;
			if(iFreq < pSAF7741RadioInfo->radioInfo.iFreqAMMin)	iFreq = pSAF7741RadioInfo->radioInfo.iFreqAMMin;
		}
	}
	else if (STEP_BACKWARD == eForward)//Backward
	{
		if(AM != eMode)
		{
			if(STEP_MANUAL == eStepMode)//手动
			{
				iFreq -= pSAF7741RadioInfo->radioInfo.iFreqFMManualStep;
			}
			else//自动
			{
				iFreq -= pSAF7741RadioInfo->radioInfo.iFreqFMScanStep;
			}
			if(iFreq < pSAF7741RadioInfo->radioInfo.iFreqFMMin)	iFreq = pSAF7741RadioInfo->radioInfo.iFreqFMMax;
			if(iFreq > pSAF7741RadioInfo->radioInfo.iFreqFMMax)	iFreq = pSAF7741RadioInfo->radioInfo.iFreqFMMax;
		}
		else
		{
			if(STEP_MANUAL == eStepMode)//手动
			{
				iFreq -= pSAF7741RadioInfo->radioInfo.iFreqAMManualStep;
			}
			else// if(RadioScanStatus == 1)//自动
			{
				iFreq -= pSAF7741RadioInfo->radioInfo.iFreqAMScanStep;
			}
			if(iFreq < pSAF7741RadioInfo->radioInfo.iFreqAMMin)	iFreq = pSAF7741RadioInfo->radioInfo.iFreqAMMax;
			if(iFreq > pSAF7741RadioInfo->radioInfo.iFreqAMMax)	iFreq = pSAF7741RadioInfo->radioInfo.iFreqAMMax;
		}
	}
	else {
		if(AM != eMode)
		{
			if(iFreq > pSAF7741RadioInfo->radioInfo.iFreqFMMax)	iFreq = pSAF7741RadioInfo->radioInfo.iFreqFMMin;
			if(iFreq < pSAF7741RadioInfo->radioInfo.iFreqFMMin)	iFreq = pSAF7741RadioInfo->radioInfo.iFreqFMMin;
		}
		else
		{
			if(iFreq > pSAF7741RadioInfo->radioInfo.iFreqAMMax)	iFreq = pSAF7741RadioInfo->radioInfo.iFreqAMMin;
			if(iFreq < pSAF7741RadioInfo->radioInfo.iFreqAMMin)	iFreq = pSAF7741RadioInfo->radioInfo.iFreqAMMin;
		}
	}
	return iFreq;
}

//static BYTE DAA_Process(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BYTE freq)
//{
//
//	BYTE bassValue,offsetValue,bUp;
//	BYTE i;
//	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 DAA Process")));
//	for(i = 0; i < 6; i++)
//	{
//		if(i == 6)break;
//		if(freq >= pSAF7741RadioInfo->DAA_Freq7[i] && freq <= pSAF7741RadioInfo->DAA_Freq7[i+1])break;
//	}	
//	if(i == 6)return(0x40);//返回缺省值
//	bassValue = pSAF7741RadioInfo->DAA_Value[i];
//	if(pSAF7741RadioInfo->DAA_Value[i] > pSAF7741RadioInfo->DAA_Value[i+1])
//	{
//		offsetValue = pSAF7741RadioInfo->DAA_Value[i] - pSAF7741RadioInfo->DAA_Value[i+1];
//		bUp = 0;
//	}
//	else
//	{
//		offsetValue = pSAF7741RadioInfo->DAA_Value[i+1] - pSAF7741RadioInfo->DAA_Value[i];
//		bUp = 1;
//	}
//	if(bUp)
//	{
//		bassValue = bassValue+(BYTE)(1.0*offsetValue*(freq-pSAF7741RadioInfo->DAA_Freq7[i])/(pSAF7741RadioInfo->DAA_Freq7[i+1]-pSAF7741RadioInfo->DAA_Freq7[i]));
//	}
//	else
//	{
//		bassValue = bassValue-(BYTE)(1.0*offsetValue*(freq-pSAF7741RadioInfo->DAA_Freq7[i])/(pSAF7741RadioInfo->DAA_Freq7[i+1]-pSAF7741RadioInfo->DAA_Freq7[i]));
//	}
//	return bassValue;
//}

static void FlyRadio_Set_Freq(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,enumRadioMode mode,UINT freq)
{
	UINT temp;
	BYTE Freq[] = {0x00,0x00};
	BYTE IIC_RDS1_CTR[5] = {0x00, 0x00, 0x35, 0x00, 0x20};
	BYTE IIC_RDS2_CTR[5] = {0x00, 0x00, 0x3D, 0x00, 0x20};
	UINT regAddr;
	if(mode != AM)
	{
		DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Set Freq FM-->%d"),freq));
		//Set Freq
		temp = (freq / 5);
		Freq[0] = (BYTE)(temp >> 8);
		Freq[1] = (BYTE)temp;
		//Set Band-->FM Standrad World
		Freq[0] = (Freq[0] & 0x1F) | 0x80; 

		I2C_Write_TEF7000_1(pSAF7741RadioInfo,0x30,Freq,2);
		I2C_Write_TEF7000_2(pSAF7741RadioInfo,0x30,Freq,2);

		regAddr = (IIC_RDS1_CTR[0] << 16) + (IIC_RDS1_CTR[1] << 8) + IIC_RDS1_CTR[2];
		I2C_Write_SAF7741(pSAF7741RadioInfo,SAF7741CreateI2CRegAddr(regAddr),&IIC_RDS1_CTR[3],2);
		regAddr = (IIC_RDS2_CTR[0] << 16) + (IIC_RDS2_CTR[1] << 8) + IIC_RDS2_CTR[2];
		I2C_Write_SAF7741(pSAF7741RadioInfo,SAF7741CreateI2CRegAddr(regAddr),&IIC_RDS2_CTR[3],2);
	}
	else
	{
		//Set Freq
		Freq[0] = (BYTE)(freq >> 8);
		Freq[1] = (BYTE)freq;
		//Set Band-->FM Standrad World
		Freq[0] = (Freq[0] & 0x1F) | 0x20; 

		I2C_Write_TEF7000_1(pSAF7741RadioInfo,0x30,Freq,2);

		regAddr = (IIC_RDS2_CTR[0] << 16) + (IIC_RDS2_CTR[1] << 8) + IIC_RDS2_CTR[2];
		I2C_Write_SAF7741(pSAF7741RadioInfo,SAF7741CreateI2CRegAddr(regAddr),&IIC_RDS2_CTR[3],2);
	}
}

void FlyRadio_Mute_P(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BOOL para)
{
	BYTE reg[5] = {0x0D, 0x10, 0x6D, 0x00, 0x00};//ADSP_Y_Mute_P
	UINT regAddr;

	if(para)
	{
		reg[3] = 0x00;reg[4] = 0x00;
		DBGI((TEXT("\r\nFlyRadio SAF7741:Mute P---->ON")));
	}
	else
	{
		reg[3] = 0x08;reg[4] = 0x00;
		DBGI((TEXT("\r\nFlyRadio SAF7741:Mute P---->OFF")));
	}
	regAddr = (reg[0] << 16) + (reg[1] << 8) + reg[2];
	I2C_Write_SAF7741(pSAF7741RadioInfo,SAF7741CreateI2CRegAddr(regAddr),&reg[3],2);
}

static void buttomJumpFreqAndReturn(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo)//界面按键跳频用
{
	*pSAF7741RadioInfo->radioInfo.pPreRadioFreq = 
		RadioStepFreqGenerate(pSAF7741RadioInfo
		,pSAF7741RadioInfo->radioInfo.ePreRadioMode
		,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq
		,pSAF7741RadioInfo->radioInfo.eButtomStepDirection
		,STEP_MANUAL);

	returnRadioFreq(pSAF7741RadioInfo,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq);
}

//static BYTE ReadFromSAFTEFNVM(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,UINT add)
//{
//	BYTE data;
//	BYTE data2;
//
//	I2C_Read_SAFTEFNVM_Step1(pSAF7741RadioInfo,add,&data, 1);
//	I2C_Read_SAFTEFNVM_Step2(pSAF7741RadioInfo,add,&data2,1);
//	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 ReadFromSAFTEFNVM add-->%x,data-->%x"),add,data2));
//	return data2;
//}
//static void DAA_Init(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo)
//{
//	pSAF7741RadioInfo->DAA_Value_Max = 0;
//	if(ReadFromSAFTEFNVM(pSAF7741RadioInfo,0x09) == ((0x00<<6)|14))
//	{
//		pSAF7741RadioInfo->NomberOfDAA = ReadFromSAFTEFNVM(pSAF7741RadioInfo,0x0A);
//		if(pSAF7741RadioInfo->NomberOfDAA > 7)pSAF7741RadioInfo->NomberOfDAA = 7;
//		for(UINT i = 0; i < pSAF7741RadioInfo->NomberOfDAA; i++)
//		{
//			pSAF7741RadioInfo->DAA_Freq7[i] = 8750 + ReadFromSAFTEFNVM(pSAF7741RadioInfo,(0x0B + i*2))*10;
//			pSAF7741RadioInfo->DAA_Value[i] = ReadFromSAFTEFNVM(pSAF7741RadioInfo,(0x0C + i*2));
//			if(pSAF7741RadioInfo->DAA_Value_Max < pSAF7741RadioInfo->DAA_Value[i])pSAF7741RadioInfo->DAA_Value_Max = pSAF7741RadioInfo->DAA_Value[i];
//		}
//	}
//	else
//	{
//		DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 DAA_Value(default)")));
//		pSAF7741RadioInfo->DAA_Value[0] = 0x38;
//		pSAF7741RadioInfo->DAA_Value[1] = 0x38;
//		pSAF7741RadioInfo->DAA_Value[2] = 0x38;
//		pSAF7741RadioInfo->DAA_Value[3] = 0x38;
//		pSAF7741RadioInfo->DAA_Value[4] = 0x38;
//		pSAF7741RadioInfo->DAA_Value[5] = 0x38;
//		pSAF7741RadioInfo->DAA_Value[6] = 0x38;
//		pSAF7741RadioInfo->DAA_Value_Max = 0x38;	
//	}
//	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 DAA_Value-->")));
//	for (UINT i=0;i<7;i++)
//	{
//		DBGI((TEXT(" [%d]%X"),i,pSAF7741RadioInfo->DAA_Value[i]));
//	}
//	DBGI((TEXT("")));
//}
//static void TunerDSPAlignmentsInit(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo)
//{
//	BYTE data;
//	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 TunerDSP Alignments Init")));
//	data = ReadFromSAFTEFNVM(pSAF7741RadioInfo,0x09);
//	if(ReadFromSAFTEFNVM(pSAF7741RadioInfo,0x09) != ((0x00<<6)|14))//0x0e
//	{
//		pSAF7741RadioInfo->FMLv1Europe = 0x0FC3;
//		pSAF7741RadioInfo->MWLv1Europe = 0x0FD9;
//		pSAF7741RadioInfo->FMChanSep = 0x0520;
//		pSAF7741RadioInfo->AMChanSep = 0x0544;
//		pSAF7741RadioInfo->FreqOffset = 0x00E358B8;
//		return;
//	}
//
//	data = ReadFromSAFTEFNVM(pSAF7741RadioInfo,0xD0);
//	if(data >= 0x80)
//	{
//		pSAF7741RadioInfo->FMLv1Europe = (UINT)(0xFC2 + (0x100 - data)/7.5);
//	}
//	else
//	{
//		pSAF7741RadioInfo->FMLv1Europe = (UINT)(0xFC2 - data/7.5);
//	}
//
//
//	data = ReadFromSAFTEFNVM(pSAF7741RadioInfo,0xD4);
//	if(data >= 0x80)
//	{
//		pSAF7741RadioInfo->MWLv1Europe = (UINT)(0xFE0 + (0x100 - data)/7.5);
//	}
//	else
//	{
//		pSAF7741RadioInfo->MWLv1Europe = (UINT)(0xFE0 - data/7.5);
//	}
//
//	data = ReadFromSAFTEFNVM(pSAF7741RadioInfo,0xD7);
//	pSAF7741RadioInfo->FMChanSep = (UINT)(data*10.68);
//
//	data = ReadFromSAFTEFNVM(pSAF7741RadioInfo,0xD8);
//	if(data == 0xff)
//		pSAF7741RadioInfo->AMChanSep = 0x0544;
//	else
//		pSAF7741RadioInfo->AMChanSep = (UINT)(data*12.0);
//	I2C_Read_Normal(pSAF7741RadioInfo,0x0c,&data,1);
//	if(data >= 0x80)
//	{
//		pSAF7741RadioInfo->FreqOffset = (UINT)(0x00E27627 + (0x100 - data)*276.5);
//	}
//	else
//	{
//		pSAF7741RadioInfo->FreqOffset = (UINT)(0x00E27627 - data*276.5);
//	}
//}
//static void TunerDSPAlignmentsWrite(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,enumRadioMode mode)
//{
//	BYTE TDSP1E_Y_FW_FDLE_1_RanOfs[5]={0x03 ,0x10, 0x1A, 0x0F, 0xBB}; 
//	BYTE TDSP2_Y_FDLE_2_RanOfs[5]={0x05, 0x10, 0xCF, 0x0F, 0xC2}; 
//	BYTE TDSP1E_Y_FMLR_1_GanCor[5]={0x03, 0x13, 0x12, 0x05, 0x24}; 
//	BYTE cFreqOffsetRadio1[6]={0x01,0x0F,0xF5,0xE3,0x58,0xB8};
//	BYTE cFreqOffsetRadio2[6]={0x05,0x0F,0xF5,0xE2,0x78,0xA9};
//
//	BYTE cMWLv1Europe[5]={0x03,0x11,0x3F,0x0F,0xD9};
//	BYTE cAMChanSep[5]={0x03,0x11,0x82,0x06,0x40};
//
//	UINT regAddr;
//
//	DAA_Init(pSAF7741RadioInfo);
//	TunerDSPAlignmentsInit(pSAF7741RadioInfo);
////#if RDS_MODEL == RDS_7730
////	RDSInit();
////#endif
//	pSAF7741RadioInfo->MWLv1Europe = 0x0FD9;
//	pSAF7741RadioInfo->FMChanSep = 0x0520;//0x0520;//use default value	
//	pSAF7741RadioInfo->AMChanSep = 0x0544;
//
//	cFMLv1Europe[3] = (BYTE)(pSAF7741RadioInfo->FMLv1Europe >> 8);
//	cFMLv1Europe[4] = (BYTE)(pSAF7741RadioInfo->FMLv1Europe);
//	cMWLv1Europe[3] = (BYTE)(pSAF7741RadioInfo->MWLv1Europe >> 8);
//	cMWLv1Europe[4] = (BYTE)(pSAF7741RadioInfo->MWLv1Europe);
//	cFMChanSep[3] = (BYTE)(pSAF7741RadioInfo->FMChanSep >> 8);
//	cFMChanSep[4] = (BYTE)(pSAF7741RadioInfo->FMChanSep);
//	cAMChanSep[3] = (BYTE)(pSAF7741RadioInfo->AMChanSep >> 8);
//	cAMChanSep[4] = (BYTE)(pSAF7741RadioInfo->AMChanSep);
//	cFreqOffsetRadio1[3] = (BYTE)(pSAF7741RadioInfo->FreqOffset >> 16);
//	cFreqOffsetRadio1[4] = (BYTE)(pSAF7741RadioInfo->FreqOffset >> 8);
//	cFreqOffsetRadio1[5] = (BYTE)(pSAF7741RadioInfo->FreqOffset);
//	if(pSAF7741RadioInfo->radioInfo.eCurRadioMode != AM)
//	{
//		regAddr = (cFMLv1Europe[0] << 16) + (cFMLv1Europe[1] << 8) + cFMLv1Europe[2];
//		I2C_Write_Saf7730(pSAF7741RadioInfo,Saf7741CreatePrimaI2CRegAddr(regAddr),&cFMLv1Europe[3],2);
//		regAddr = (cRanOfs[0] << 16) + (cRanOfs[1] << 8) + cRanOfs[2];
//		I2C_Write_Saf7730(pSAF7741RadioInfo,Saf7741CreatePrimaI2CRegAddr(regAddr),&cRanOfs[3],2);
//		regAddr = (cFMChanSep[0] << 16) + (cFMChanSep[1] << 8) + cFMChanSep[2];
//		I2C_Write_Saf7730(pSAF7741RadioInfo,Saf7741CreatePrimaI2CRegAddr(regAddr),&cFMChanSep[3],2);
//	}
//	else
//	{
//		regAddr = (cMWLv1Europe[0] << 16) + (cMWLv1Europe[1] << 8) + cMWLv1Europe[2];
//		I2C_Write_Saf7730(pSAF7741RadioInfo,Saf7741CreatePrimaI2CRegAddr(regAddr),&cMWLv1Europe[3],2);
//		regAddr = (cRanOfs[0] << 16) + (cRanOfs[1] << 8) + cRanOfs[2];
//		I2C_Write_Saf7730(pSAF7741RadioInfo,Saf7741CreatePrimaI2CRegAddr(regAddr),&cRanOfs[3],2);
//		regAddr = (cAMChanSep[0] << 16) + (cAMChanSep[1] << 8) + cAMChanSep[2];
//		I2C_Write_Saf7730(pSAF7741RadioInfo,Saf7741CreatePrimaI2CRegAddr(regAddr),&cAMChanSep[3],2);
//	}
//	regAddr = (cFreqOffsetRadio1[0] << 16) + (cFreqOffsetRadio1[1] << 8) + cFreqOffsetRadio1[2];
//	I2C_Write_Saf7730(pSAF7741RadioInfo,Saf7741CreatePrimaI2CRegAddr(regAddr),&cFreqOffsetRadio1[3],3);
//	regAddr = (cFreqOffsetRadio2[0] << 16) + (cFreqOffsetRadio2[1] << 8) + cFreqOffsetRadio2[2];
//	I2C_Write_Saf7730(pSAF7741RadioInfo,Saf7741CreatePrimaI2CRegAddr(regAddr),&cFreqOffsetRadio2[3],3);
//
//	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 TunerDSPAlignments FMLv1Europe-->%X,FMChanSep-->%X,FreqOffset-->%X,"),pSAF7741RadioInfo->FMLv1Europe,pSAF7741RadioInfo->FMChanSep,pSAF7741RadioInfo->FreqOffset));
//	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 TunerDSPAlignments MWLv1Europe-->%X,AMChanSep-->%X,FreqOffset-->%X,"),pSAF7741RadioInfo->MWLv1Europe,pSAF7741RadioInfo->AMChanSep,pSAF7741RadioInfo->FreqOffset));
//}

static void SAF7741Radio_ChangeToFMAM(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,enumRadioMode mode)
{
	BYTE TEF7000_1_FM[4] = {0x86, 0xD6, 0x80, 0xC0};
	BYTE TEF7000_2_FM[4] = {0x86, 0xD6, 0x80, 0xC0};

	BYTE TEF7000_1_AM_1[2] = {0x22, 0x0A};
	BYTE TEF7000_1_AM_2[4] = {0x22, 0x0A, 0x00, 0x00};
	BYTE TEF7000_2_AM[4] = {0x22, 0x0A, 0x00, 0x00};

	if(mode != AM)
	{
		DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 ChangeToFMAM-->FM")));

		SendToSAF7741NormalWriteData(pSAF7741RadioInfo,SAF7741_Radio_Init_FM_1);

		I2C_Write_TEF7000_1(pSAF7741RadioInfo,0xD0,TEF7000_1_FM,4);
		I2C_Write_TEF7000_2(pSAF7741RadioInfo,0xD0,TEF7000_2_FM,4);

		SendToSAF7741NormalWriteData(pSAF7741RadioInfo,SAF7741_Radio_Init_FM_2);
	}
	else
	{
		DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 ChangeToFMAM-->AM")));

		I2C_Write_TEF7000_1(pSAF7741RadioInfo,0x30,TEF7000_1_AM_1,2);

		SendToSAF7741NormalWriteData(pSAF7741RadioInfo,SAF7741_Radio_Init_AM_1);

		I2C_Write_TEF7000_1(pSAF7741RadioInfo,0xD0,TEF7000_1_AM_2,4);
		I2C_Write_TEF7000_2(pSAF7741RadioInfo,0xD0,TEF7000_2_AM,4);

		SendToSAF7741NormalWriteData(pSAF7741RadioInfo,SAF7741_Radio_Init_AM_2);
	}
}

static VOID SAF7741RadioDealWinceCmd(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BYTE *pdata,UINT len)
{
	UINT iTemp;
	switch(pdata[0])
	{
		case 0x01:
			if (0x01 == pdata[1])
			{
				DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Deal WINCE-->Init")));
				pSAF7741RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNeedProcVoltageShakeRadio = 85;

				RegDataReadRadio(pSAF7741RadioInfo);
				SAF7741RadioJumpNewFreqParaInit(pSAF7741RadioInfo);

				returnRadioPowerMode(pSAF7741RadioInfo,TRUE);
				returnRadioInitStatus(pSAF7741RadioInfo,TRUE);

				pSAF7741RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditRadio = TRUE;

				pSAF7741RadioInfo->bPowerUp = TRUE;
				pSAF7741RadioInfo->bNeedInit = TRUE;
				SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);//激活一次
			}
			else if (0x00 == pdata[1])
			{
				DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Deal WINCE-->DeInit")));
				Fly7741RadioEnable(pSAF7741RadioInfo,FALSE);
				returnRadioPowerMode(pSAF7741RadioInfo,FALSE);
			}
			break;
		case 0x03://软件模拟按键 1-FM1 2-FM2 3-AM 4-STOP RADIO 5-AF 6-TA
			switch(pdata[1])
			{
			case 0x01:
			case 0x02:
			case 0x03:
				if (pSAF7741RadioInfo->radioInfo.bPreScaning)
				{
					pSAF7741RadioInfo->radioInfo.bPreScaning = FALSE;
				}
				if (pSAF7741RadioInfo->bPowerUp)//直到之前的扫描停止
				{	
					SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
					while (pSAF7741RadioInfo->radioInfo.bCurScaning)
					{
						Sleep(100);
					}
				}
				if (0x01 == pdata[1])
				{
					pSAF7741RadioInfo->radioInfo.ePreRadioMode = FM1;
					DBGD((TEXT("\r\nFlyRadio SAF7741 TEF7000 set mode --------FM1")));
				} 
				else if (0x02 == pdata[1])
				{
					pSAF7741RadioInfo->radioInfo.ePreRadioMode = FM2;
					DBGD((TEXT("\r\nFlyRadio SAF7741 TEF7000 set mode --------FM2")));
				}
				else if (0x03 == pdata[1])
				{
					pSAF7741RadioInfo->radioInfo.ePreRadioMode = AM;
					DBGD((TEXT("\r\nFlyRadio SAF7741 TEF7000 set mode --------AM")));
				}

				returnRadioMode(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.ePreRadioMode);
				if (AM == pSAF7741RadioInfo->radioInfo.ePreRadioMode)
				{
					returnRadioFreq(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM);
				}
				else if (FM1 == pSAF7741RadioInfo->radioInfo.ePreRadioMode)
				{
					returnRadioFreq(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1);
				}
				else if (FM2 == pSAF7741RadioInfo->radioInfo.ePreRadioMode)
				{
					returnRadioFreq(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2);
				}

				if (pSAF7741RadioInfo->bPowerUp)
				{	
					SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
				}
				break;
			case 0x04:
				pSAF7741RadioInfo->radioInfo.bPreScaning = !pSAF7741RadioInfo->radioInfo.bPreScaning;
				if (pSAF7741RadioInfo->bPowerUp)
				{
					SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
				}
				break;
			case 0x05:
				pSAF7741RadioInfo->RDSInfo.RadioRDSAFControlOn = !pSAF7741RadioInfo->RDSInfo.RadioRDSAFControlOn;
				returnRadioAFStatus(pSAF7741RadioInfo,pSAF7741RadioInfo->RDSInfo.RadioRDSAFControlOn);
				if (pSAF7741RadioInfo->bPowerUp)
				{
					SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
				}
				break;
			case 0x06:
				pSAF7741RadioInfo->RDSInfo.RadioRDSTAControlOn = !pSAF7741RadioInfo->RDSInfo.RadioRDSTAControlOn;
				returnRadioTAStatus(pSAF7741RadioInfo,pSAF7741RadioInfo->RDSInfo.RadioRDSTAControlOn);
				if (pSAF7741RadioInfo->bPowerUp)
				{
					SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
				}
				break;
			case 0x07:
				if (pSAF7741RadioInfo->radioInfo.bPreScaning)
				{
					pSAF7741RadioInfo->radioInfo.bPreScaning = FALSE;
				}
				if (pSAF7741RadioInfo->radioInfo.ePreRadioMode == pSAF7741RadioInfo->radioInfo.eCurRadioMode)
				{
					*pSAF7741RadioInfo->radioInfo.pPreRadioFreq = 
						RadioStepFreqGenerate(pSAF7741RadioInfo
						,pSAF7741RadioInfo->radioInfo.ePreRadioMode
						,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq
						,STEP_FORWARD
						,STEP_MANUAL);
					DBGD((TEXT("\r\nFlyRadio SAF7741 TEF7000 set freq value:%d"),*pSAF7741RadioInfo->radioInfo.pPreRadioFreq));
						returnRadioFreq(pSAF7741RadioInfo,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq);
					if (pSAF7741RadioInfo->bPowerUp)
					{
						SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
					}
				}
				break;
			case 0x08:
				if (pSAF7741RadioInfo->radioInfo.bPreScaning)
				{
					pSAF7741RadioInfo->radioInfo.bPreScaning = FALSE;
				}
				if (pSAF7741RadioInfo->radioInfo.ePreRadioMode == pSAF7741RadioInfo->radioInfo.eCurRadioMode)
				{
					*pSAF7741RadioInfo->radioInfo.pPreRadioFreq = 
						RadioStepFreqGenerate(pSAF7741RadioInfo
						,pSAF7741RadioInfo->radioInfo.ePreRadioMode
						,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq
						,STEP_BACKWARD
						,STEP_MANUAL);
					DBGD((TEXT("\r\nFlyRadio SAF7741 TEF7000 set freq value:%d"),*pSAF7741RadioInfo->radioInfo.pPreRadioFreq));
						returnRadioFreq(pSAF7741RadioInfo,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq);
					if (pSAF7741RadioInfo->bPowerUp)
					{
						SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
					}
				}
				break;
			default:
				DBGD((TEXT("\r\nFlyRadio SAF7741 TEF7000 user command Undeal 0x03 %X"),pdata[1]));
				break;
		    }
			break;
		case 0x10://设置收音机频率 
			if (pSAF7741RadioInfo->radioInfo.bPreScaning)
			{
				pSAF7741RadioInfo->radioInfo.bPreScaning = FALSE;
			}
			iTemp = pdata[1]*256+pdata[2];
			iTemp = 
				RadioStepFreqGenerate(pSAF7741RadioInfo
				,pSAF7741RadioInfo->radioInfo.ePreRadioMode
				,iTemp
				,STEP_NONE,STEP_MANUAL);
			if (AM == pSAF7741RadioInfo->radioInfo.ePreRadioMode)
			{
				pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM = iTemp;
			}
			else if (FM1 == pSAF7741RadioInfo->radioInfo.ePreRadioMode)
			{
				pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1 = iTemp;
			}
			else if (FM2 == pSAF7741RadioInfo->radioInfo.ePreRadioMode)
			{
				pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2 = iTemp;
			}
			DBGD((TEXT("\r\nFlyRadio SAF7741 TEF7000 set freq value:%d"),iTemp));
				returnRadioFreq(pSAF7741RadioInfo,iTemp);
			if (pSAF7741RadioInfo->bPowerUp)
			{
				SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
			}
			break;
		case 0x11://频点+
			if (pSAF7741RadioInfo->radioInfo.bPreScaning)
			{
				pSAF7741RadioInfo->radioInfo.bPreScaning = FALSE;
			}
			pSAF7741RadioInfo->radioInfo.eButtomStepDirection = STEP_FORWARD;
			if (0x00 == pdata[1])
			{
				//TDA7541RadioReturnToUser(pSAF7741RadioInfo,pdata,len);
				buttomJumpFreqAndReturn(pSAF7741RadioInfo);
				pSAF7741RadioInfo->radioInfo.bPreStepButtomDown = TRUE;
				pSAF7741RadioInfo->radioInfo.iButtomStepCount = 0;
			}
			else if (0x01 == pdata[1])
			{
				//TDA7541RadioReturnToUser(pSAF7741RadioInfo,pdata,len);
				pSAF7741RadioInfo->radioInfo.bPreStepButtomDown = FALSE;
			}
			if (pSAF7741RadioInfo->bPowerUp)
			{
				SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
			}
			break;
		case 0x12://频点-
			if (pSAF7741RadioInfo->radioInfo.bPreScaning)
			{
				pSAF7741RadioInfo->radioInfo.bPreScaning = FALSE;
			}
			pSAF7741RadioInfo->radioInfo.eButtomStepDirection = STEP_BACKWARD;
			if (0x00 == pdata[1])
			{
				//TDA7541RadioReturnToUser(pSAF7741RadioInfo,pdata,len);
				buttomJumpFreqAndReturn(pSAF7741RadioInfo);
				pSAF7741RadioInfo->radioInfo.bPreStepButtomDown = TRUE;
				pSAF7741RadioInfo->radioInfo.iButtomStepCount = 0;
			}
			else if (0x01 == pdata[1])
			{
				//TDA7541RadioReturnToUser(pSAF7741RadioInfo,pdata,len);
				pSAF7741RadioInfo->radioInfo.bPreStepButtomDown = FALSE;
			}
			if (pSAF7741RadioInfo->bPowerUp)
			{
				SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
			}
			break;
		case 0x13://扫描控制
			switch(pdata[1])
			{
			case 0x00:
			case 0x01:
				pSAF7741RadioInfo->radioInfo.bPreScaning = TRUE;
				pSAF7741RadioInfo->radioInfo.eScanDirection = STEP_FORWARD;
				pSAF7741RadioInfo->radioInfo.bScanRepeatFlag = FALSE;
				break;
			case 0x02:
				pSAF7741RadioInfo->radioInfo.bPreScaning = TRUE;
				pSAF7741RadioInfo->radioInfo.eScanDirection = STEP_BACKWARD;
				pSAF7741RadioInfo->radioInfo.bScanRepeatFlag = FALSE;
				break;
			case 0x03:
				pSAF7741RadioInfo->radioInfo.bPreScaning = FALSE;
				break;
			case 0x04:
			case 0x05:
				pSAF7741RadioInfo->radioInfo.bPreScaning = TRUE;
				pSAF7741RadioInfo->radioInfo.eScanDirection = STEP_FORWARD;
				pSAF7741RadioInfo->radioInfo.bScanRepeatFlag = TRUE;
				break;
			case 0x06:
				pSAF7741RadioInfo->radioInfo.bPreScaning = TRUE;
				pSAF7741RadioInfo->radioInfo.eScanDirection = STEP_BACKWARD;
				pSAF7741RadioInfo->radioInfo.bScanRepeatFlag = TRUE;
				break;
			default:break;
			}
			returnRadioScanCtrl(pSAF7741RadioInfo,pdata[1]);
			if (pSAF7741RadioInfo->bPowerUp)
			{
				SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
			}
			DBGD((TEXT("\r\nFlyRadio SAF7741 TEF7000 set scan ctrl:%d"),pdata[1]));
				break;
		case 0x15://开始收音
			if (0x00 == pdata[1])
			{
				pSAF7741RadioInfo->bPreMute = TRUE;
				if (pSAF7741RadioInfo->radioInfo.bPreScaning)
				{
					pSAF7741RadioInfo->radioInfo.bPreScaning = FALSE;
				}
				returnRadioMuteStatus(pSAF7741RadioInfo,FALSE);
			}
			else if (0x01 == pdata[1])
			{
				pSAF7741RadioInfo->bPreMute = FALSE;
				returnRadioMuteStatus(pSAF7741RadioInfo,TRUE);
			}				
			if (pSAF7741RadioInfo->bPowerUp)
			{
				SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
			}			
			break;
		case 0x16://AF开关
			if (0x01 == pdata[1])
			{
				pSAF7741RadioInfo->RDSInfo.RadioRDSAFControlOn = TRUE;
				returnRadioAFStatus(pSAF7741RadioInfo,TRUE);
			}
			else if (0x00 == pdata[1])
			{
				pSAF7741RadioInfo->RDSInfo.RadioRDSAFControlOn = FALSE;
				returnRadioAFStatus(pSAF7741RadioInfo,FALSE);
			}
			if (pSAF7741RadioInfo->bPowerUp)
			{
				SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
			}
			break;
		case 0x17://TA开关
			if (0x01 == pdata[1])
			{
				pSAF7741RadioInfo->RDSInfo.RadioRDSTAControlOn = TRUE;
				returnRadioTAStatus(pSAF7741RadioInfo,TRUE);
			}
			else if (0x00 == pdata[1])
			{
				pSAF7741RadioInfo->RDSInfo.RadioRDSTAControlOn = FALSE;
				returnRadioTAStatus(pSAF7741RadioInfo,FALSE);
			}
			if (pSAF7741RadioInfo->bPowerUp)
			{
				SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
			}
			break;
		case 0x30://交通广播开关
			if (0x01 == pdata[1])
			{
				pSAF7741RadioInfo->RDSInfo.RadioRDSAFControlOn = TRUE;
				returnRadioRDSWorkStatus(pSAF7741RadioInfo,TRUE);				
			}
			else if (0x00 == pdata[1])
			{
				pSAF7741RadioInfo->RDSInfo.RadioRDSAFControlOn = FALSE;
				returnRadioRDSWorkStatus(pSAF7741RadioInfo,FALSE);
			}
			if (pSAF7741RadioInfo->bPowerUp)
			{
				SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
			}
			break;
		case 0xFF:
			if (0x01 == pdata[1])
			{
				FRA_PowerUp((DWORD)pSAF7741RadioInfo);
			} 
			else if (0x00 == pdata[1])
			{
				FRA_PowerDown((DWORD)pSAF7741RadioInfo);
			}
			break;
		default:
			DBGD((TEXT("\r\nFlyRadio SAF7741 TEF7000 user command unhandle %X"),pdata[1]));
				break;
		}
}

UINT rU8ComboToU32(BYTE *p,BYTE len)
{
	BYTE i;
	UINT data = 0;
	for(i = 0; i < len; i++)
	{
		data = (data << 8) + p[i];	
	}
	return data;
} 
VOID ReadFromSAF7741(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,UINT regAddr,BYTE *pdata,BYTE len)
{
	I2C_Read_SAF7741(pSAF7741RadioInfo, SAF7741CreateI2CRegAddr(regAddr), pdata, len);
}
static BOOL bRadioSignalGood(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,enumRadioMode radioMode,UINT *pLevel)
{
	BYTE bGood = 0;
	BYTE bGoodLevel;

	UINT regAddr;

	BYTE regFMLevel[6] = {0x03,0x00,0xD9,0x00,0x00,0x00};//TDSP1E_X_wFDLE_1_LvlQpd_out//[-0,2..1>, i.e. [-20dBμV .. 100dBμV>.
	UINT regdataFMLevel;
	BYTE regFMUSNoise[6] = {0x01,0x01,0x51,0x00,0x00,0x00};//TDSP1_X_wFDUN_1_SlwAvg_out//Full Scale按比例0-1

	BYTE regFMOFS[6] = {0x03,0x00,0xE9,0x00,0x00,0x00};//TDSP1E_X_wFIOF_1_Ofs_out

	BYTE regAMLevel[6] = {0x03,0x00,0xAF,0x00,0x00,0x00};//TDSP1E_X_wADLE_1_LvlFlt_out
	UINT regdataAMLevel;
	BYTE regAMOFS[6] = {0x03,0x00,0x73,0x00,0x00,0x00};//TDSP1E_X_wAIOF_1_Ofs_out

	if(radioMode != AM)
	{
		regAddr = (regFMLevel[0] << 16) + (regFMLevel[1] << 8) + regFMLevel[2];
		ReadFromSAF7741(pSAF7741RadioInfo,regAddr,&regFMLevel[3],3);
		regdataFMLevel = rU8ComboToU32(&regFMLevel[3],3);
		DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 CMD regdataFMLevel-->%X"),regdataFMLevel));
		if(regdataFMLevel >= 0x00800000)regdataFMLevel = 0;
		bGoodLevel = (BYTE)(regdataFMLevel*1.0*100/0x00800000);

		if(bGoodLevel > FM_SCAN_STOP_LEVEL)
		{
			return TRUE;
			//bGood |= 0x01;
		}
		//else
		//{
		//	//bGood &=~ 0x01;	
		//}

		//regAddr = (regFMUSNoise[0] << 16) + (regFMUSNoise[1] << 8) + regFMUSNoise[2];
		//ReadFromSAF7741(pSAF7741RadioInfo,regAddr,&regFMUSNoise[3],3);
		//regdataFMUSNoise = rU8ComboToU32(&regFMUSNoise[3],3);
		//DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 CMD regdataFMUSNoise-->%X"),regdataFMUSNoise));
		//iRange = (BYTE) (regdataFMUSNoise*1.0*100/0x01000000);
		//bNoiseLevel = iRange;
		//if(iRange < FM_SCAN_STOP_NOISE)
		//{
		//	bGood |= 0x02;
		//}
		//else
		//{
		//	bGood &= ~0x02;
		//}

		//regAddr = (regFMOFS[0] << 16) + (regFMOFS[1] << 8) + regFMOFS[2];
		//ReadFromSAF7730(pSAF7741RadioInfo,regAddr,&regFMOFS[3],3);
		//regdataFMOFS = rU8ComboToU32(&regFMOFS[3],3);
		//DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 CMD regdataFMOFS-->%X"),regdataFMOFS));
		//if(regdataFMOFS >= 0x00800000)regdataFMOFS = 0x01000000 - regdataFMOFS;
		//iRange = (BYTE)(regdataFMOFS*1.0*100/0x00800000);
		//if(iRange < FM_SCAN_STOP_OFS)
		//{
		//	bGood |= 0x04;
		//}
		//else
		//{
		//	bGood &= ~0x04;
		//}

		//if((bGood & 0x07) == 0x07)
		//{
		//	*pLevel = bGoodLevel;
		//	return TRUE;
		//}
	}	
	else
	{
		regAddr = (regAMLevel[0] << 16) + (regAMLevel[1] << 8) + regAMLevel[2];
		ReadFromSAF7741(pSAF7741RadioInfo,regAddr,&regAMLevel[3],3);
		regdataAMLevel = rU8ComboToU32(&regAMLevel[3],3);
		DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 CMD regdataAMLevel-->%X"),regdataAMLevel));
		if(regdataAMLevel >= 0x00800000)regdataAMLevel = 0;
		bGoodLevel = (BYTE)(regdataAMLevel*1.0*100/0x00800000);//regdataAMLevel*1.0/0x00800000*100;
		
		if(bGoodLevel > AM_SCAN_STOP_LEVEL)//48
		{
			return TRUE;
		}
	}
	return FALSE;
}

static void FlyRadio_Scaning(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BOOL Bon)
{
	BYTE Y1_FCIB_1_BWSetMan[5] = {0x01, 0x11, 0xA5, 0x00, 0x00};
	BYTE X1_FCIB_1_FixSet[6] = {0x01, 0x03, 0x1E, 0x00, 0x00, 0x00};
	UINT regAddr;

	if (Bon)
	{
		if (AM != pSAF7741RadioInfo->radioInfo.eCurRadioMode)
		{
			Y1_FCIB_1_BWSetMan[3] = 0x07;
			Y1_FCIB_1_BWSetMan[4] = 0xFF;
			regAddr = (Y1_FCIB_1_BWSetMan[0] << 16) + (Y1_FCIB_1_BWSetMan[1] << 8) + Y1_FCIB_1_BWSetMan[2];
			I2C_Write_SAF7741(pSAF7741RadioInfo,SAF7741CreateI2CRegAddr(regAddr),&Y1_FCIB_1_BWSetMan[3],2);

			X1_FCIB_1_FixSet[3] = 0x00;
			X1_FCIB_1_FixSet[4] = 0x00;
			X1_FCIB_1_FixSet[5] = 0x07;
			regAddr = (X1_FCIB_1_FixSet[0] << 16) + (X1_FCIB_1_FixSet[1] << 8) + X1_FCIB_1_FixSet[2];
			I2C_Write_SAF7741(pSAF7741RadioInfo,SAF7741CreateI2CRegAddr(regAddr),&X1_FCIB_1_FixSet[3],3);
			I2C_Write_SAF7741(pSAF7741RadioInfo,SAF7741CreateI2CRegAddr(regAddr),&X1_FCIB_1_FixSet[3],3);
		}
	}
	else
	{
		if (AM != pSAF7741RadioInfo->radioInfo.eCurRadioMode)
		{
			X1_FCIB_1_FixSet[3] = 0x00;
			X1_FCIB_1_FixSet[4] = 0x00;
			X1_FCIB_1_FixSet[5] = 0x08;
			regAddr = (X1_FCIB_1_FixSet[0] << 16) + (X1_FCIB_1_FixSet[1] << 8) + X1_FCIB_1_FixSet[2];
			I2C_Write_SAF7741(pSAF7741RadioInfo,SAF7741CreateI2CRegAddr(regAddr),&X1_FCIB_1_FixSet[3],3);

			Y1_FCIB_1_BWSetMan[3] = 0x00;
			Y1_FCIB_1_BWSetMan[4] = 0x00;
			regAddr = (Y1_FCIB_1_BWSetMan[0] << 16) + (Y1_FCIB_1_BWSetMan[1] << 8) + Y1_FCIB_1_BWSetMan[2];
			I2C_Write_SAF7741(pSAF7741RadioInfo,SAF7741CreateI2CRegAddr(regAddr),&Y1_FCIB_1_BWSetMan[3],2);
		}
	}
}

void SAF7741RadioJumpNewFreqParaInit(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo)
{
#if RADIO_RDS
	RDSParaInit(pSAF7741RadioInfo);
#endif
}

static UINT GetCorrectScanStartFreq(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,UINT *pFreq)
{
	UINT base;
	if (AM != pSAF7741RadioInfo->radioInfo.eCurRadioMode)
	{
		if (((*pFreq - pSAF7741RadioInfo->radioInfo.iFreqFMMin) % pSAF7741RadioInfo->radioInfo.iFreqFMScanStep) != 0)
		{
			base = 1;
		}
		else
		{
			base = 0;
		}
		*pFreq = pSAF7741RadioInfo->radioInfo.iFreqFMMin + (base + (*pFreq - pSAF7741RadioInfo->radioInfo.iFreqFMMin)/pSAF7741RadioInfo->radioInfo.iFreqFMScanStep)*pSAF7741RadioInfo->radioInfo.iFreqFMScanStep;
	}
	else
	{
		if (((*pFreq - pSAF7741RadioInfo->radioInfo.iFreqAMMin) % pSAF7741RadioInfo->radioInfo.iFreqAMScanStep) != 0)
		{
			base = 1;
		}
		else
		{
			base = 0;
		}		
		*pFreq = pSAF7741RadioInfo->radioInfo.iFreqAMMin + (base + (*pFreq - pSAF7741RadioInfo->radioInfo.iFreqAMMin)/pSAF7741RadioInfo->radioInfo.iFreqAMScanStep)*pSAF7741RadioInfo->radioInfo.iFreqAMScanStep;
	}
	//if (((*pFreq - pSAF7741RadioInfo->radioInfo.iFreqFMMin) % pSAF7741RadioInfo->radioInfo.iFreqFMScanStep) != 0)
	//{
	//	if (AM != pSAF7741RadioInfo->radioInfo.eCurRadioMode)
	//	{
	//		*pFreq = *pFreq + pSAF7741RadioInfo->radioInfo.iFreqFMScanStep;
	//	}
	//	else
	//	{
	//		*pFreq = *pFreq + pSAF7741RadioInfo->radioInfo.iFreqAMScanStep;
	//	}

	//}
	return *pFreq;
}

static DWORD WINAPI SAF7741RadioScanThread(LPVOID pContext)
{
	P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo = (P_FLY_SAF7741_RADIO_INFO)pContext;
	ULONG WaitReturn;
	UINT iScanStartFreq;
	BOOL bHaveSearched;
	UINT iHaveSearchedLevel,iTempHaveSearchedLevel;

	ULONG nowTimer,lastTimer;
	ULONG iThreadFreqStepWaitTime = 0;

	ULONG tBlinkingTimerStart,tBlinkingTimerEnd;
	BYTE iBlinkingTimes;
	BOOL bBlinkingStatus;

	while (!pSAF7741RadioInfo->bKillDispatchScanThread)
	{
		if (0 == iThreadFreqStepWaitTime)
		{
			WaitReturn = WaitForSingleObject(pSAF7741RadioInfo->hDispatchScanThreadEvent, INFINITE);
		}
		else
		{
			WaitReturn = WaitForSingleObject(pSAF7741RadioInfo->hDispatchScanThreadEvent, iThreadFreqStepWaitTime);
		}
		iThreadFreqStepWaitTime = 0;

		if (pSAF7741RadioInfo->radioInfo.bCurScaning == TRUE && pSAF7741RadioInfo->radioInfo.bPreScaning == FALSE)//跳出搜台
		{
			FlyRadio_Scaning(pSAF7741RadioInfo,FALSE);
			returnRadioFreq(pSAF7741RadioInfo,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq);
			returnRadioScanCtrl(pSAF7741RadioInfo,0x03);//stop
			pSAF7741RadioInfo->bCurMute = !pSAF7741RadioInfo->bPreMute;
		}

		DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Scan Thread-->in")));
		//设置波段
		if (pSAF7741RadioInfo->radioInfo.eCurRadioMode != pSAF7741RadioInfo->radioInfo.ePreRadioMode)
		{
			if (pSAF7741RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable)
			{
				IpcStartEvent(EVENT_GLOBAL_RADIO_MUTE_IN_REQ_ID);//发送进入静音
				while (!IpcWhatEvent(EVENT_GLOBAL_RADIO_MUTE_IN_OK_ID))//等待OK
				{
					Sleep(10);
				}
				IpcClearEvent(EVENT_GLOBAL_RADIO_MUTE_IN_OK_ID);//清除
			}

			pSAF7741RadioInfo->radioInfo.eCurRadioMode = pSAF7741RadioInfo->radioInfo.ePreRadioMode;
			SAF7741Radio_ChangeToFMAM(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.eCurRadioMode);
			if (FM1 == pSAF7741RadioInfo->radioInfo.eCurRadioMode)
			{
				pSAF7741RadioInfo->radioInfo.pPreRadioFreq = &pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1;
				pSAF7741RadioInfo->radioInfo.pCurRadioFreq = &pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM1;
			}
			else if (FM2 == pSAF7741RadioInfo->radioInfo.eCurRadioMode)
			{
				pSAF7741RadioInfo->radioInfo.pPreRadioFreq = &pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2;
				pSAF7741RadioInfo->radioInfo.pCurRadioFreq = &pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM2;
			}
			else if (AM == pSAF7741RadioInfo->radioInfo.eCurRadioMode)
			{
				pSAF7741RadioInfo->radioInfo.pPreRadioFreq = &pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM;
				pSAF7741RadioInfo->radioInfo.pCurRadioFreq = &pSAF7741RadioInfo->radioInfo.iCurRadioFreqAM;
			}
			SAF7741RadioJumpNewFreqParaInit(pSAF7741RadioInfo);

			if (pSAF7741RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable)
			{
				if (AM == pSAF7741RadioInfo->radioInfo.eCurRadioMode)
				{
					Sleep(314);
				}
				IpcStartEvent(EVENT_GLOBAL_RADIO_MUTE_OUT_REQ_ID);//发送退出静音
			}
		}
		//设置频率
		if (*pSAF7741RadioInfo->radioInfo.pCurRadioFreq != *pSAF7741RadioInfo->radioInfo.pPreRadioFreq)
		{
			FlyRadio_Mute_P(pSAF7741RadioInfo,TRUE);

			*pSAF7741RadioInfo->radioInfo.pCurRadioFreq = *pSAF7741RadioInfo->radioInfo.pPreRadioFreq;
			FlyRadio_Set_Freq(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.eCurRadioMode,*pSAF7741RadioInfo->radioInfo.pCurRadioFreq);
			SAF7741RadioJumpNewFreqParaInit(pSAF7741RadioInfo);

			if (pSAF7741RadioInfo->bPreMute == FALSE)
			{
				FlyRadio_Mute_P(pSAF7741RadioInfo,FALSE);//取消静音
			}
		}
		//收音机静音开关
		if(pSAF7741RadioInfo->bPreMute != pSAF7741RadioInfo->bCurMute)
		{
			if (pSAF7741RadioInfo->bPreMute == TRUE)
			{
				FlyRadio_Mute_P(pSAF7741RadioInfo,TRUE); // mute
			}
			else
			{
				FlyRadio_Mute_P(pSAF7741RadioInfo,FALSE);// demute
			}
			pSAF7741RadioInfo->bCurMute = pSAF7741RadioInfo->bPreMute;
		}
		if (pSAF7741RadioInfo->radioInfo.bPreStepButtomDown)//按下，则持续跳台
		{
			if (0 == pSAF7741RadioInfo->radioInfo.iButtomStepCount)
			{
				pSAF7741RadioInfo->radioInfo.iButtomStepCount++;
				iThreadFreqStepWaitTime = 314;
			}
			else
			{
				buttomJumpFreqAndReturn(pSAF7741RadioInfo);
				pSAF7741RadioInfo->radioInfo.iButtomStepCount++;
				iThreadFreqStepWaitTime = 100;
			}
			continue;//跳到开头
		}
		nowTimer = GetTickCount();
		lastTimer = nowTimer;
		//搜索
		while (pSAF7741RadioInfo->radioInfo.bPreScaning)
		{
			nowTimer = GetTickCount();
			FlyRadio_Mute_P(pSAF7741RadioInfo,TRUE); // mute
			DBGD((TEXT("\r\nFlyRadio TDA7541RadioScanThread --------------scaning ....")));
			if (pSAF7741RadioInfo->radioInfo.bCurScaning != pSAF7741RadioInfo->radioInfo.bPreScaning)//起始搜索频率
			{
				FlyRadio_Scaning(pSAF7741RadioInfo,TRUE);

				pSAF7741RadioInfo->radioInfo.bCurScaning = pSAF7741RadioInfo->radioInfo.bPreScaning;
				iScanStartFreq = GetCorrectScanStartFreq(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.pPreRadioFreq);
				bHaveSearched = FALSE;
				iHaveSearchedLevel = 0;
			}	

			DBGD((TEXT("\r\nFlyRadio SAF7741 ScanThread iScanStartFreq = %d,PreRadioFreq = %d"),iScanStartFreq,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq));

			*pSAF7741RadioInfo->radioInfo.pPreRadioFreq = RadioStepFreqGenerate(pSAF7741RadioInfo
															,pSAF7741RadioInfo->radioInfo.eCurRadioMode
															,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq
															,pSAF7741RadioInfo->radioInfo.eScanDirection
															,STEP_SCAN);

			*pSAF7741RadioInfo->radioInfo.pCurRadioFreq = *pSAF7741RadioInfo->radioInfo.pPreRadioFreq;
			FlyRadio_Set_Freq(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.eCurRadioMode,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq);
			if (iScanStartFreq == *pSAF7741RadioInfo->radioInfo.pPreRadioFreq )
			{
				pSAF7741RadioInfo->radioInfo.bPreScaning = FALSE;
				pSAF7741RadioInfo->radioInfo.bCurScaning = FALSE;

				FlyRadio_Scaning(pSAF7741RadioInfo,FALSE);

				returnRadioFreq(pSAF7741RadioInfo,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq);
				returnRadioScanCtrl(pSAF7741RadioInfo,0x03);//stop
				if (pSAF7741RadioInfo->bPreMute == FALSE)
				{
					FlyRadio_Mute_P(pSAF7741RadioInfo,FALSE);
				}
			}
			if ((nowTimer - lastTimer) >  157)//定时返回频点
			{
				lastTimer = nowTimer;
				if (pSAF7741RadioInfo->radioInfo.bPreScaning)
				{
					returnRadioFreq(pSAF7741RadioInfo,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq);
				}
			}
			if (AM != pSAF7741RadioInfo->radioInfo.eCurRadioMode)
			{
				Sleep(10);
			} 
			else
			{
				Sleep(138);
			}
			if (bRadioSignalGood(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.eCurRadioMode,&iTempHaveSearchedLevel))
			{
				bHaveSearched = TRUE;
				if (iHaveSearchedLevel > iTempHaveSearchedLevel)//OK
				{
					if (STEP_BACKWARD == pSAF7741RadioInfo->radioInfo.eScanDirection)
					{
						*pSAF7741RadioInfo->radioInfo.pPreRadioFreq = RadioStepFreqGenerate(pSAF7741RadioInfo
							,pSAF7741RadioInfo->radioInfo.eCurRadioMode
							,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq
							,STEP_FORWARD
							,STEP_SCAN);
					} 
					else
					{
						*pSAF7741RadioInfo->radioInfo.pPreRadioFreq = RadioStepFreqGenerate(pSAF7741RadioInfo
							,pSAF7741RadioInfo->radioInfo.eCurRadioMode
							,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq
							,STEP_BACKWARD
							,STEP_SCAN);
					}

					*pSAF7741RadioInfo->radioInfo.pCurRadioFreq = *pSAF7741RadioInfo->radioInfo.pPreRadioFreq;
					FlyRadio_Set_Freq(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.eCurRadioMode,*pSAF7741RadioInfo->radioInfo.pCurRadioFreq);

					if (pSAF7741RadioInfo->bPreMute == FALSE)//收到台要出声音
					{
						FlyRadio_Mute_P(pSAF7741RadioInfo,FALSE);
					}

					returnRadioFreq(pSAF7741RadioInfo,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq);
					returnRadioHaveSearched(pSAF7741RadioInfo,TRUE);

					if (pSAF7741RadioInfo->radioInfo.bScanRepeatFlag == FALSE)
					{
						FlyRadio_Scaning(pSAF7741RadioInfo,FALSE);

						pSAF7741RadioInfo->radioInfo.bPreScaning = FALSE;
						pSAF7741RadioInfo->radioInfo.bCurScaning = FALSE;
						returnRadioScanCtrl(pSAF7741RadioInfo,0x03);//stop							
						break;
					}
					else
					{
						//blinking 5 times
						tBlinkingTimerStart = GetTickCount();
						iBlinkingTimes = 0;
						bBlinkingStatus = TRUE;
						while (pSAF7741RadioInfo->radioInfo.bPreScaning == TRUE && iBlinkingTimes < 10)
						{
							tBlinkingTimerEnd = GetTickCount();
							if ((tBlinkingTimerEnd - tBlinkingTimerStart) > 500)//0.5s
							{
								tBlinkingTimerStart = tBlinkingTimerEnd;
								iBlinkingTimes++;
								bBlinkingStatus = !bBlinkingStatus;
								returnRadioBlinkingStatus(pSAF7741RadioInfo,bBlinkingStatus);
							}								
						}

						returnRadioBlinkingStatus(pSAF7741RadioInfo,TRUE);
						Sleep(500);
						if (pSAF7741RadioInfo->radioInfo.bPreScaning == TRUE)
						{
							returnRadioScanCtrl(pSAF7741RadioInfo,0x04);//repeat
						}

						iScanStartFreq = *pSAF7741RadioInfo->radioInfo.pPreRadioFreq;
						bHaveSearched = FALSE;
						iHaveSearchedLevel = 0;
					}
				}
			}
			else
			{
				if (bHaveSearched)
				{
					if (STEP_BACKWARD == pSAF7741RadioInfo->radioInfo.eScanDirection)
					{
						*pSAF7741RadioInfo->radioInfo.pPreRadioFreq = RadioStepFreqGenerate(pSAF7741RadioInfo
							,pSAF7741RadioInfo->radioInfo.eCurRadioMode
							,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq
							,STEP_FORWARD
							,STEP_SCAN);
					} 
					else
					{
						*pSAF7741RadioInfo->radioInfo.pPreRadioFreq = RadioStepFreqGenerate(pSAF7741RadioInfo
							,pSAF7741RadioInfo->radioInfo.eCurRadioMode
							,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq
							,STEP_BACKWARD
							,STEP_SCAN);
					}

					*pSAF7741RadioInfo->radioInfo.pCurRadioFreq = *pSAF7741RadioInfo->radioInfo.pPreRadioFreq;
					FlyRadio_Set_Freq(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.eCurRadioMode,*pSAF7741RadioInfo->radioInfo.pCurRadioFreq);

					if (pSAF7741RadioInfo->bPreMute == FALSE)//收到台要出声音
					{
						FlyRadio_Mute_P(pSAF7741RadioInfo,FALSE);
					}

					returnRadioFreq(pSAF7741RadioInfo,*pSAF7741RadioInfo->radioInfo.pPreRadioFreq);
					returnRadioHaveSearched(pSAF7741RadioInfo,TRUE);

					if (pSAF7741RadioInfo->radioInfo.bScanRepeatFlag == FALSE)
					{
						FlyRadio_Scaning(pSAF7741RadioInfo,FALSE);

						pSAF7741RadioInfo->radioInfo.bPreScaning = FALSE;
						pSAF7741RadioInfo->radioInfo.bCurScaning = FALSE;
						returnRadioScanCtrl(pSAF7741RadioInfo,0x03);//stop							
						break;
					}
					else
					{
						//blinking 5 times
						tBlinkingTimerStart = GetTickCount();
						iBlinkingTimes = 0;
						bBlinkingStatus = TRUE;
						while (pSAF7741RadioInfo->radioInfo.bPreScaning == TRUE && iBlinkingTimes < 10)
						{
							tBlinkingTimerEnd = GetTickCount();
							if ((tBlinkingTimerEnd - tBlinkingTimerStart) > 500)//0.5s
							{
								tBlinkingTimerStart = tBlinkingTimerEnd;
								iBlinkingTimes++;
								bBlinkingStatus = !bBlinkingStatus;
								returnRadioBlinkingStatus(pSAF7741RadioInfo,bBlinkingStatus);
							}								
						}

						FlyRadio_Scaning(pSAF7741RadioInfo,TRUE);

						returnRadioBlinkingStatus(pSAF7741RadioInfo,TRUE);
						Sleep(500);
						if (pSAF7741RadioInfo->radioInfo.bPreScaning == TRUE)
						{
							returnRadioScanCtrl(pSAF7741RadioInfo,0x04);//repeat
						}

						iScanStartFreq = *pSAF7741RadioInfo->radioInfo.pPreRadioFreq;
						bHaveSearched = FALSE;
						iHaveSearchedLevel = 0;
					}
				}
			}
			iHaveSearchedLevel = iTempHaveSearchedLevel;
		}
		DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Scan Thread-->out")));
	}
	pSAF7741RadioInfo->Saf7741RadioScanThreadHandle = NULL;
	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Scan Thread-->exit")));
	return 0;
}

static DWORD WINAPI Saf7741RadioMainThread(LPVOID pContext)
{
	ULONG WaitReturn;
	static SYSTEMTIME  sysTime1,sysTime2; 
	static UINT RadioScanStatus;

	P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo = (P_FLY_SAF7741_RADIO_INFO)pContext;

	while (!pSAF7741RadioInfo->bKillDispatchMainThread)
	{
		WaitReturn = WaitForSingleObject(pSAF7741RadioInfo->hDispatchMainThreadEvent, INFINITE);

		if (IpcWhatEvent(EVENT_GLOBAL_REG_RESTORE_RADIO_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_REG_RESTORE_RADIO_ID);
			if (pSAF7741RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditRadio)
			{
				pSAF7741RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditRadio = FALSE;
				if (pSAF7741RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRestoreRegeditToFactory)
				{
					radioIDChangePara(pSAF7741RadioInfo,pSAF7741RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser);
					pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1 = pSAF7741RadioInfo->radioInfo.iFreqFMMin;
					pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2 = pSAF7741RadioInfo->radioInfo.iFreqFMMin;
					pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM = pSAF7741RadioInfo->radioInfo.iFreqAMMin;
					pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM1 = pSAF7741RadioInfo->radioInfo.iFreqFMMin;
					pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM2 = pSAF7741RadioInfo->radioInfo.iFreqFMMin;
					pSAF7741RadioInfo->radioInfo.iCurRadioFreqAM = pSAF7741RadioInfo->radioInfo.iFreqAMMin;
				}
				RegDataWriteRadio(pSAF7741RadioInfo);
			}
		}
		
		DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Main Thread-->in")));
		if (FALSE == pSAF7741RadioInfo->bPowerUp)
		{
		}
		else
		{
			if (pSAF7741RadioInfo->bNeedInit)
			{
				pSAF7741RadioInfo->bNeedInit = FALSE;

				//while (pSAF7741RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bFlyAudioInitFinish == FALSE)
				//{
				//	Sleep(314);
				//}

				FlyRadio_SAF7741_TEF7000_Init(pSAF7741RadioInfo);

				pSAF7741RadioInfo->bPowerUp = TRUE;

				SAF7741Radio_ChangeToFMAM(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.eCurRadioMode);
				FlyRadio_Set_Freq(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.eCurRadioMode,*pSAF7741RadioInfo->radioInfo.pCurRadioFreq);

			}
			if (IpcWhatEvent(EVENT_GLOBAL_RADIO_CHANGE_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_RADIO_CHANGE_ID);
				DBGI((TEXT("\r\nFlyAudio TDA7541 Radio ID Change!")));
					radioIDChangePara(pSAF7741RadioInfo
					,pSAF7741RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser);
			}

			if (IpcWhatEvent(EVENT_GLOBAL_BATTERY_RECOVERY_RADIO_ID))
			{
				DBGI((TEXT("\r\nFlyAudio TDA7541 Voltage After Low Proc")));
					IpcClearEvent(EVENT_GLOBAL_BATTERY_RECOVERY_RADIO_ID);

				SAF7741Radio_ChangeToFMAM(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.eCurRadioMode);
			}

			if (IpcWhatEvent(EVENT_GLOBAL_RADIO_ANTENNA_ID))
			{
				DBGI((TEXT("\r\nFlyAudio TDA7541 ANTENNA ctrl")));
					IpcClearEvent(EVENT_GLOBAL_RADIO_ANTENNA_ID);
				//if (pSAF7741RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eAudioInput == RADIO)
				//{
				//	//radioANTControl(pSAF7741RadioInfo,TRUE);
				//}
				//else
				//{
				//	//radioANTControl(pSAF7741RadioInfo,FALSE);
				//}
			}

			SetEvent(pSAF7741RadioInfo->hDispatchScanThreadEvent);
		}
		DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Main Thread-->out")));
	}

	pSAF7741RadioInfo->bPowerUp = FALSE;
	pSAF7741RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNeedProcVoltageShakeRadio = 0;

	while (pSAF7741RadioInfo->Saf7741RadioScanThreadHandle)
	{
		SetEvent(pSAF7741RadioInfo->hDispatchScanThreadEvent);
		Sleep(10);
	}
	CloseHandle(pSAF7741RadioInfo->hDispatchScanThreadEvent);
#if RADIO_RDS
	while (pSAF7741RadioInfo->Saf7741RadioRDSRecThreadHandle)
	{
		Sleep(10);
	}
#endif
	pSAF7741RadioInfo->bPowerUp = FALSE;

	CloseHandle(pSAF7741RadioInfo->hDispatchMainThreadEvent);
	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Main Thread-->exit!")));
	return 0;
}


void FlyRadio_SAF7741_TEF7000_Init(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo)
{
	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 init")));
	BYTE TEF7000_1_Init_Data[7] = {0x86, 0xD6, 0x80, 0xC0, 0x00, 0x00, 0x00};
	BYTE TEF7000_2_Init_Data[7] = {0x86, 0xD6, 0x80, 0xC0, 0x00, 0x00, 0x00};
	SendToSAF7741NormalWriteData(pSAF7741RadioInfo,SAF7741_Radio_Init_Data);
	//INFO: Initialise Tuner1 module....
	I2C_Write_TEF7000_1(pSAF7741RadioInfo,0xD0,TEF7000_1_Init_Data,7);
	//INFO: Initialise Tuner2 module....
	I2C_Write_TEF7000_2(pSAF7741RadioInfo,0xD0,TEF7000_2_Init_Data,7);
}

static void radioIDChangePara(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BYTE iID)
{

	if (0x00 == iID)//China
	{
		pSAF7741RadioInfo->radioInfo.iFreqFMMin = 8750;
		pSAF7741RadioInfo->radioInfo.iFreqFMMax = 10800;
		pSAF7741RadioInfo->radioInfo.iFreqFMManualStep = 5;
		pSAF7741RadioInfo->radioInfo.iFreqFMScanStep = 10;

		pSAF7741RadioInfo->radioInfo.iFreqAMMin = 522;
		pSAF7741RadioInfo->radioInfo.iFreqAMMax = 1620;
		pSAF7741RadioInfo->radioInfo.iFreqAMManualStep = 9;
		pSAF7741RadioInfo->radioInfo.iFreqAMScanStep = 9;

		pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1 = 8750;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM1 = 8750;
		pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2 = 8750;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM2 = 8750;
		pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM = 522;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqAM = 522;
	}
	else if (0x01 == iID)//USA
	{
		pSAF7741RadioInfo->radioInfo.iFreqFMMin = 8750;
		pSAF7741RadioInfo->radioInfo.iFreqFMMax = 10790;
		pSAF7741RadioInfo->radioInfo.iFreqFMManualStep = 5;
		pSAF7741RadioInfo->radioInfo.iFreqFMScanStep = 20;

		pSAF7741RadioInfo->radioInfo.iFreqAMMin = 530;
		pSAF7741RadioInfo->radioInfo.iFreqAMMax = 1720;
		pSAF7741RadioInfo->radioInfo.iFreqAMManualStep = 10;
		pSAF7741RadioInfo->radioInfo.iFreqAMScanStep = 10;

		pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1 = 8750;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM1 = 8750;
		pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2 = 8750;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM2 = 8750;
		pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM = 530;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqAM = 530;
	}
	else if (0x02 == iID)//S.Amer-1
	{
		pSAF7741RadioInfo->radioInfo.iFreqFMMin = 8750;
		pSAF7741RadioInfo->radioInfo.iFreqFMMax = 10800;
		pSAF7741RadioInfo->radioInfo.iFreqFMManualStep = 10;
		pSAF7741RadioInfo->radioInfo.iFreqFMScanStep = 10;

		pSAF7741RadioInfo->radioInfo.iFreqAMMin = 530;
		pSAF7741RadioInfo->radioInfo.iFreqAMMax = 1710;
		pSAF7741RadioInfo->radioInfo.iFreqAMManualStep = 10;
		pSAF7741RadioInfo->radioInfo.iFreqAMScanStep = 10;

		pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1 = 8750;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM1 = 8750;
		pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2 = 8750;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM2 = 8750;
		pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM = 530;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqAM = 530;
	}
	else if (0x03 == iID)//S.Amer-2
	{
		pSAF7741RadioInfo->radioInfo.iFreqFMMin = 8750;
		pSAF7741RadioInfo->radioInfo.iFreqFMMax = 10800;
		pSAF7741RadioInfo->radioInfo.iFreqFMManualStep = 10;
		pSAF7741RadioInfo->radioInfo.iFreqFMScanStep = 10;

		pSAF7741RadioInfo->radioInfo.iFreqAMMin = 520;
		pSAF7741RadioInfo->radioInfo.iFreqAMMax = 1600;
		pSAF7741RadioInfo->radioInfo.iFreqAMManualStep = 5;
		pSAF7741RadioInfo->radioInfo.iFreqAMScanStep = 5;

		pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1 = 8750;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM1 = 8750;
		pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2 = 8750;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM2 = 8750;
		pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM = 520;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqAM = 520;
	}
	else if (0x04 == iID)//KOREA
	{
		pSAF7741RadioInfo->radioInfo.iFreqFMMin = 8810;
		pSAF7741RadioInfo->radioInfo.iFreqFMMax = 10790;
		pSAF7741RadioInfo->radioInfo.iFreqFMManualStep = 5;
		pSAF7741RadioInfo->radioInfo.iFreqFMScanStep = 20;

		pSAF7741RadioInfo->radioInfo.iFreqAMMin = 531;
		pSAF7741RadioInfo->radioInfo.iFreqAMMax = 1620;
		pSAF7741RadioInfo->radioInfo.iFreqAMManualStep = 9;
		pSAF7741RadioInfo->radioInfo.iFreqAMScanStep = 9;

		pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1 = 8810;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM1 = 8810;
		pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2 = 8810;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM2 = 8810;
		pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM = 531;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqAM = 531;
	}
	else if (0x05 == iID)//Thailand
	{
		pSAF7741RadioInfo->radioInfo.iFreqFMMin = 8750;
		pSAF7741RadioInfo->radioInfo.iFreqFMMax = 10800;
		pSAF7741RadioInfo->radioInfo.iFreqFMManualStep = 5;
		pSAF7741RadioInfo->radioInfo.iFreqFMScanStep = 25;

		pSAF7741RadioInfo->radioInfo.iFreqAMMin = 531;
		pSAF7741RadioInfo->radioInfo.iFreqAMMax = 1602;
		pSAF7741RadioInfo->radioInfo.iFreqAMManualStep = 9;
		pSAF7741RadioInfo->radioInfo.iFreqAMScanStep = 9;

		pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1 = 8750;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM1 = 8750;
		pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2 = 8750;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM2 = 8750;
		pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM = 531;
		//pSAF7741RadioInfo->radioInfo.iCurRadioFreqAM = 531;
	}
	if (AM == pSAF7741RadioInfo->radioInfo.ePreRadioMode)
	{
		returnRadioFreq(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.iPreRadioFreqAM);
	}
	else if (FM1 == pSAF7741RadioInfo->radioInfo.ePreRadioMode)
	{
		returnRadioFreq(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1);
	}
	else if (FM2 == pSAF7741RadioInfo->radioInfo.ePreRadioMode)
	{
		returnRadioFreq(pSAF7741RadioInfo,pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM2);
	}
}

static void powerOnNormalInit(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo)
{
	pSAF7741RadioInfo->bPowerUp = FALSE;

	pSAF7741RadioInfo->buffToUserHx = 0;
	pSAF7741RadioInfo->buffToUserLx = 0;

	pSAF7741RadioInfo->bPreMute = FALSE;
	pSAF7741RadioInfo->bCurMute = TRUE;

	radioIDChangePara(pSAF7741RadioInfo,pSAF7741RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser);

	pSAF7741RadioInfo->radioInfo.ePreRadioMode = FM1;
	pSAF7741RadioInfo->radioInfo.eCurRadioMode = FM1;

	pSAF7741RadioInfo->radioInfo.pPreRadioFreq = &pSAF7741RadioInfo->radioInfo.iPreRadioFreqFM1;
	pSAF7741RadioInfo->radioInfo.pCurRadioFreq = &pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM1;

	pSAF7741RadioInfo->radioInfo.bPreScaning = FALSE;
	pSAF7741RadioInfo->radioInfo.bCurScaning = FALSE;
	pSAF7741RadioInfo->radioInfo.eScanDirection = STEP_FORWARD;	
	pSAF7741RadioInfo->radioInfo.bScanRepeatFlag = FALSE;

	pSAF7741RadioInfo->radioInfo.bPreStepButtomDown = FALSE;
	//pSAF7741RadioInfo->radioInfo.bCurStepButtomDown = FALSE;
	pSAF7741RadioInfo->radioInfo.eButtomStepDirection = STEP_FORWARD;
	pSAF7741RadioInfo->radioInfo.iButtomStepCount = 0;

	pSAF7741RadioInfo->RDSInfo.RadioRDSAFControlOn = FALSE;
	pSAF7741RadioInfo->RDSInfo.RadioRDSTAControlOn = FALSE;

	//IO_Control_Init(pSAF7741RadioInfo);
	DBGD((TEXT("\r\nFlyRadio SAF7741 (RadioParaInit)%d____%d_____%d____________%d"),(pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM1),(pSAF7741RadioInfo->radioInfo.iCurRadioFreqFM2),(pSAF7741RadioInfo->radioInfo.iCurRadioFreqAM),*pSAF7741RadioInfo->radioInfo.pCurRadioFreq));
}

static void powerOnFirstInit(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo)
{
	pSAF7741RadioInfo->bOpen = FALSE;

	pSAF7741RadioInfo->bKillDispatchMainThread = TRUE;
	pSAF7741RadioInfo->Saf7741RadioMainThreadHandle = NULL;
	pSAF7741RadioInfo->hDispatchMainThreadEvent = NULL;

	pSAF7741RadioInfo->bKillDispatchScanThread = TRUE;
	pSAF7741RadioInfo->Saf7741RadioScanThreadHandle = NULL;
	pSAF7741RadioInfo->hDispatchScanThreadEvent = NULL;

	pSAF7741RadioInfo->bKillDispatchRDSRecThread = TRUE;
	pSAF7741RadioInfo->Saf7741RadioRDSRecThreadHandle = NULL;

}

VOID Fly7741RadioEnable(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BOOL bOn)
{
	DWORD dwThreadID;

	if(bOn)
	{
		DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000-->Enable!")));


		pSAF7741RadioInfo->bKillDispatchMainThread = FALSE;
		pSAF7741RadioInfo->Saf7741RadioMainThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)Saf7741RadioMainThread, //线程的全局函数
			pSAF7741RadioInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		if (NULL == pSAF7741RadioInfo->Saf7741RadioMainThreadHandle)
		{
			pSAF7741RadioInfo->bKillDispatchMainThread = TRUE;
			return;
		}
		pSAF7741RadioInfo->hDispatchMainThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 MainThread ID-->%x!"),dwThreadID));

		pSAF7741RadioInfo->bKillDispatchScanThread = FALSE;
		pSAF7741RadioInfo->Saf7741RadioScanThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)SAF7741RadioScanThread, //线程的全局函数
			pSAF7741RadioInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		if (NULL == pSAF7741RadioInfo->Saf7741RadioScanThreadHandle)
		{
			pSAF7741RadioInfo->bKillDispatchScanThread = TRUE;
			return;
		}
		pSAF7741RadioInfo->hDispatchScanThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 ScanThread ID-->%x!"),dwThreadID));

#if RADIO_RDS
		pSAF7741RadioInfo->bKillDispatchRDSRecThread = FALSE;
		pSAF7741RadioInfo->Saf7741RadioRDSRecThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)SAF7741RadioRDSRecThread, //线程的全局函数
			pSAF7741RadioInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		if (NULL == pSAF7741RadioInfo->Saf7741RadioRDSRecThreadHandle)
		{
			pSAF7741RadioInfo->bKillDispatchRDSRecThread = TRUE;
			return;
		}
		DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 RDSRecThread ID %x!"),dwThreadID));
#endif
	}
	else
	{
		DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000--> Unable!")));
		if (FALSE == pSAF7741RadioInfo->bPower)
		{
			return;
		}
		pSAF7741RadioInfo->bPower = FALSE;

#if RADIO_RDS
		pSAF7741RadioInfo->bKillDispatchRDSRecThread = TRUE;
#endif
		pSAF7741RadioInfo->bKillDispatchScanThread = TRUE;
		SetEvent(pSAF7741RadioInfo->hDispatchScanThreadEvent);
		pSAF7741RadioInfo->bKillDispatchMainThread = TRUE;
		SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
	}
}

void IpcRecv(UINT32 iEvent)
{
	P_FLY_SAF7741_RADIO_INFO	pSAF7741RadioInfo = (P_FLY_SAF7741_RADIO_INFO)gpSAF7741RadioInfo;

	DBGD((TEXT("\r\nFLY SAF7741Radio Recv IPC iEvent:%d\r\n"),iEvent));

	SetEvent(pSAF7741RadioInfo->hDispatchMainThreadEvent);
}
//void SockRecv(BYTE *buf, UINT16 len)
//{
//	UINT16 i=0;
//	P_FLY_SAF7741_RADIO_INFO	pSAF7741RadioInfo = (P_FLY_SAF7741_RADIO_INFO)gpSAF7741RadioInfo;
//
//	DBGD((TEXT("\r\n FLY SAF7741Radio socket recv %d bytes: "),len-1));
//	for (i=0; i<len-1; i++)
//		DBGD((TEXT("%02X "), buf[i]));
//
//	if (!pSAF7741RadioInfo->bSocketConnecting)
//		pSAF7741RadioInfo->bSocketConnecting  = TRUE;
//}

HANDLE
FRA_Init(DWORD dwContext)
{
	P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo;

	pSAF7741RadioInfo = (P_FLY_SAF7741_RADIO_INFO)LocalAlloc(LPTR, sizeof(FLY_SAF7741_RADIO_INFO));
	if (!pSAF7741RadioInfo)
	{
		return NULL;
	}

	gpSAF7741RadioInfo = pSAF7741RadioInfo;

	pSAF7741RadioInfo->pFlyShmGlobalInfo = CreateShm(RADIO_MODULE,IpcRecv);
	if (NULL == pSAF7741RadioInfo->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio SAF7741Radio Init create shm err\r\n")));
		return NULL;
	}

	if (!GetDllAddrTable())
	{
		DBGE((TEXT("FlyAudio SAF7741Radio  GetDllAddrTable err\r\n")));
		return NULL;
	}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio SAF7741Radio Open create user buf err\r\n")));
		return NULL;
	}
	//if(!CreateServerSocket(SockRecv, TCP_PORT_RADIO))
	//{
	//	DBGE((TEXT("FlyAudio SAF7741Radio Open create server socket err\r\n")));
	//	return NULL;
	//}


	//参数初始化
	powerOnNormalInit(pSAF7741RadioInfo);
	powerOnFirstInit(pSAF7741RadioInfo);

	pSAF7741RadioInfo->hBuffToUserDataEvent = CreateEvent(NULL,FALSE,FALSE,DATA_READ_HANDLE_NAME);

	pSAF7741RadioInfo->bPowerUp = FALSE;
	pSAF7741RadioInfo->bNeedInit = FALSE;

	DBGI((TEXT("\r\nSAF7741Radio Init Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return (HANDLE)pSAF7741RadioInfo;
}

BOOL
FRA_Deinit(DWORD hDeviceContext)
{
	P_FLY_SAF7741_RADIO_INFO	pSAF7741RadioInfo = (P_FLY_SAF7741_RADIO_INFO)hDeviceContext;
	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 DeInit")));

	FreeShm();
	FreeSocketServer();
	FreeUserBuff();

	CloseHandle(pSAF7741RadioInfo->hBuffToUserDataEvent);
	LocalFree(pSAF7741RadioInfo);
	return TRUE;
}

DWORD
FRA_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	P_FLY_SAF7741_RADIO_INFO	pSAF7741RadioInfo = (P_FLY_SAF7741_RADIO_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;

	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Open%08X"),hDeviceContext));

	if(pSAF7741RadioInfo->bOpen)
	{
		return NULL;
	}

	Fly7741RadioEnable(pSAF7741RadioInfo,TRUE);

	pSAF7741RadioInfo->bOpen = TRUE;

	return returnWhat;
}

BOOL
FRA_Close(DWORD hDeviceContext)
{
	P_FLY_SAF7741_RADIO_INFO	pSAF7741RadioInfo = (P_FLY_SAF7741_RADIO_INFO)hDeviceContext;

	Fly7741RadioEnable(pSAF7741RadioInfo,FALSE);
	pSAF7741RadioInfo->bOpen = FALSE;

	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Close")));
	return TRUE;
}

VOID
FRA_PowerUp(DWORD hDeviceContext)
{
	P_FLY_SAF7741_RADIO_INFO	pSAF7741RadioInfo = (P_FLY_SAF7741_RADIO_INFO)hDeviceContext;
}

VOID
FRA_PowerDown(DWORD hDeviceContext)
{
	P_FLY_SAF7741_RADIO_INFO	pSAF7741RadioInfo = (P_FLY_SAF7741_RADIO_INFO)hDeviceContext;

	Fly7741RadioEnable(pSAF7741RadioInfo,FALSE);
}

DWORD
FRA_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	P_FLY_SAF7741_RADIO_INFO	pSAF7741RadioInfo = (P_FLY_SAF7741_RADIO_INFO)hOpenContext;
	DBGD((TEXT("\r\nFlyAudio 7741RADIO read start....")));

	UINT dwRead = 0,i;
	BYTE *buf = (BYTE *)pBuffer;
	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio 7741RADIO return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}
	return dwRead;
}

DWORD
FRA_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	P_FLY_SAF7741_RADIO_INFO	pSAF7741RadioInfo = (P_FLY_SAF7741_RADIO_INFO)hOpenContext;
	BYTE *p;
	p = (BYTE *)pSourceBytes;
	DBGI((TEXT("\r\nFlyRadio SAF7741 TEF7000 Write %d "),
		NumberOfBytes));
	for(UINT i = 0;i < NumberOfBytes;i++)
	{
		DBGI((TEXT(" %X "),
			*(p + i)));
	}
	if(NumberOfBytes >= 5)
	{
		SAF7741RadioDealWinceCmd(pSAF7741RadioInfo,&p[3],NumberOfBytes-4);
	}
	return NULL;
}

DWORD
FRA_Seek(DWORD hOpenContext, LONG Amount, DWORD  Type)
{
	return NULL;
}

BOOL
FRA_IOControl(DWORD hOpenContext,
			  DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn,
			  PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
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
		DEBUGMSG(1, (TEXT("Attach in Fly7741Radio DllEntry\n")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}

	if (dwReason == DLL_PROCESS_DETACH)
	{
		DEBUGMSG(1, (TEXT("Dettach in Fly7741Radio DllEntry\n")));
	}

	DEBUGMSG(1, (TEXT("Leaving Fly7741Radio DllEntry\n")));

	return (TRUE);
}
