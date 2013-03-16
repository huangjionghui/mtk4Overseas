// FLY6730Radio.cpp : Defines the entry point for the DLL application.
//
#include "flyhwlib.h"
#include "flyuserlib.h"
#include "flysocketlib.h"
#include "FLY7541Radio.h"

P_FLY_TDA7541_RADIO_INFO gpTDA7541RadioInfo = NULL;

VOID TDA7541RadioEnable(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL bOn);
static BOOL bRadioSignalGood(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,enumRadioMode radioMode,UINT *pLevel);
static void buttomJumpFreqAndReturn(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo);
static void radioIDChangePara(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE iID);
void TDA7541RadioJumpNewFreqParaInit(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo);

VOID FRA_PowerUp(DWORD hDeviceContext);
VOID FRA_PowerDown(DWORD hDeviceContext);

static void RegDataReadRadio(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	HKEY hKey; 
	DWORD dwDisposition;
	DWORD dwTemp;
	DWORD dwLen;
	DWORD dwType;

#if REGEDIT_NOT_READ
	return;
#endif

	DBGI((TEXT("\r\nTDA7541 Reg Read Start")));

	//读取收音机频点参数
	RegCreateKeyEx(HKEY_CURRENT_USER,L"FlyDrivers\\Radio",0,NULL,0,
		0,NULL,&hKey,&dwDisposition); /////////////////////////////////创建或打开注册表

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwFreqFM1",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//FM1
	{
		pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM1 = dwTemp;
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwFreqFM2",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//FM2
	{
		pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM2 = dwTemp;
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwFreqAM",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//AM
	{
		pTDA7541RadioInfo->radioInfo.iPreRadioFreqAM = dwTemp;
	}

	RegCloseKey(hKey);	
	DBGI((TEXT("\r\nTDA7541 Reg Read Finish %d %d %d"),pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM1,pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM2,pTDA7541RadioInfo->radioInfo.iPreRadioFreqAM));
}

static void RegDataWriteRadio(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	HKEY hKey; 
	DWORD dwDisposition;
	DWORD dwTemp;
	DWORD dwLen;

#if REGEDIT_NOT_SAVE
	return;
#endif

	DBGI((TEXT("\r\nTDA7541 Reg Write Start")));

	//读取方向盘学习参数
	RegCreateKeyEx(HKEY_CURRENT_USER,L"FlyDrivers\\Radio",0,NULL,0,
		0,NULL,&hKey,&dwDisposition); /////////////////////////////////创建或打开注册表

	dwLen = 4;
	dwTemp = pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM1;
	RegSetValueEx(hKey,L"dwFreqFM1",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);

	dwLen = 4;
	dwTemp = pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM2;
	RegSetValueEx(hKey,L"dwFreqFM2",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);

	dwLen = 4;
	dwTemp = pTDA7541RadioInfo->radioInfo.iPreRadioFreqAM;
	RegSetValueEx(hKey,L"dwFreqAM",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);

	RegCloseKey(hKey);
	DBGI((TEXT("\r\nTDA7541 Reg Write Finish %d %d %d"),pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM1,pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM2,pTDA7541RadioInfo->radioInfo.iPreRadioFreqAM));
}

//BYTE I2CWriteSeq_NORMAL[] = {SEG_MASK_SEND_START | I2C_SEQ_SEG_DEVADDR_W | SEG_MASK_NEED_ACK,//写从设备地址
//I2C_SEQ_SEG_REGADDR | SEG_MASK_NEED_ACK,//写寄存器地址,先发低字节
//I2C_SEQ_SEG_DATA | SEG_MASK_NEED_ACK | SEG_MASK_SEND_STOP
//};
//
//BYTE I2CReadSeq_NORMAL[] = {SEG_MASK_SEND_START | I2C_SEQ_SEG_DEVADDR_W | SEG_MASK_NEED_ACK,
//I2C_SEQ_SEG_REGADDR | SEG_MASK_NEED_ACK,//写寄存器地址,先发低字节
//SEG_MASK_SEND_START | I2C_SEQ_SEG_DEVADDR_R | SEG_MASK_NEED_ACK,//发读命令
//I2C_SEQ_SEG_DATA    | SEG_MASK_NEED_NACK | SEG_MASK_SEND_STOP //读数据,要逆序
//};
//
//BYTE I2CReadSeq_TDA7541[] = {
//SEG_MASK_SEND_START | I2C_SEQ_SEG_DEVADDR_R | SEG_MASK_NEED_ACK,//发读命令
//I2C_SEQ_SEG_DATA    | SEG_MASK_NEED_NACK | SEG_MASK_SEND_STOP, //读数据,要使用NACK
//};

const GUID DEVICE_IFC_I2C_GUID;


static void radioANTControl(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL bOn)
{
	if (bOn)
	{
#if MTK_BOARD
	HwGpioOutput(IO_ANT1_I,1);
#else
		if((pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedSupplyAntPower == TRUE))
		{
			HwGpioOutput(IO_ANT1_I,1);
		}
		HwGpioOutput(IO_ANT2_I,1);
#endif
	}
	else
	{
		HwGpioOutput(IO_ANT1_I,0);
		HwGpioOutput(IO_ANT2_I,0);
	}
}

static void radioAFMuteControl(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL bOn)
{
	if (bOn)
	{
		HwGpioOutput(IO_AFMUTE_I,0);
	}
	else
	{
		HwGpioOutput(IO_AFMUTE_I,1);
	}
}

static const BYTE DefaultData[INIT_DATA_SIZE] = {
	0xCA, 0x12, 0x60, 0xD1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x04, 0x68, 0x08, 0xC5, 0xE4, 0x3D, 0x46, 0x53, 0x6F, 0x40, 
	0x5F, 0xEC, 0xA9, 0x91, 0xE8, 0x17, 0x6B, 0x89, 0x0D, 0x79, 0x8B, 0x04, 0x05, 0xE2, 0x0E, 0x93, 0x2D, 0xFF, 0x14, 0x29, 
	0xB1, 0x50, 0x6A, 0x49, 0x2E, 0x49, 0xA9, 0x31, 0xC8, 0xD7, 0x31, 0x74, 0x0D, 0x7D, 0x57, 0xFA, 0x05, 0xC0, 0x23, 0xFF, 
	0x00, 0x67, 0xCD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x64, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xD1, 0xC5, 0xC0, 0xCC, 0xC8, 0x9C, 0x99, 0xFF, 0x3A, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x04, 0x04, 0x04, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x05, 0x05, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1B, 0x4F, 0xE6, 0x89, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0x1C, 0xF2, 0x47}; 
BYTE FM_Test[8]={0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe};//FM test registers
BYTE AM_Test[8]={0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe};//AM test registers

VOID IO_Control_Init(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	HwGpioInput(IO_TDA7541_SSTOP_I);
	DBGD((TEXT("\r\nFlyAudio IO_7541_SSTOP Init")));
}

static BOOL I2C_Write_EEPROM(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo, UCHAR ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
{
	i2ccfg_t i2c_cfg;
	i2c_cfg.bus_id = 1;
	i2c_cfg.chip_addr = (BYTE)EEPROM_ADDR_W;
	i2c_cfg.sub_addr  = ulRegAddr;
	i2c_cfg.flag      = I2C_WRITE;
	i2c_cfg.msg_buf   = (char*)pRegValBuf;
	i2c_cfg.msg_size  = uiValBufLen;

	HwI2CWrite(&i2c_cfg);

	return TRUE;
}

static BOOL I2C_Read_EEPROM(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo, UCHAR ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
{
	i2ccfg_t i2c_cfg;
	i2c_cfg.bus_id = 1;
	i2c_cfg.chip_addr = (BYTE)EEPROM_ADDR_R;
	i2c_cfg.sub_addr  = ulRegAddr;
	i2c_cfg.flag      = I2C_READ;
	i2c_cfg.msg_buf   = (char*)pRegValBuf;
	i2c_cfg.msg_size  = uiValBufLen;

	HwI2CRead(&i2c_cfg);


	UINT i;
	DBGD((TEXT("\r\nFlyAudio EEPROM:I2C read sub:%X"),ulRegAddr));
	for (i=0; i<uiValBufLen; i++)
		DBGD((TEXT("%X "),i2c_cfg.msg_buf[i]));
		

	return TRUE;
}

static BOOL I2C_Write_TDA7541(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo, UCHAR ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
{
	//I2CTRANS  Trans;
	//Trans.bRead = FALSE;
	//Trans.dwRegAddr = ulRegAddr;
	//Trans.pValueBuf = pRegValBuf;
	//Trans.uiValueLen = uiValBufLen;
	//Trans.dwClock = NORMAL_I2C_FREQ;
	//Trans.i2cdevinfo.pReadSeq = I2CReadSeq_TDA7541;
	//Trans.i2cdevinfo.uiReadSeqLen = sizeof(I2CReadSeq_TDA7541);
	//Trans.i2cdevinfo.pWriteSeq = I2CWriteSeq_NORMAL;
	//Trans.i2cdevinfo.uiWriteSeqLen = sizeof(I2CWriteSeq_NORMAL);
	//Trans.i2cdevinfo.ulDevAddrW = TDA7541_ADDR_W;
	//Trans.i2cdevinfo.ulDevAddrR = TDA7541_ADDR_R;

	//if(!pTDA7541RadioInfo->bOpen)
	//{
	//	return TRUE;
	//}

	//if (!I2CTransact(pTDA7541RadioInfo->TDA7541RadioI2CHandle, &Trans))
	//{
	//	DBGI((TEXT("\r\nFlyAudio 7541:I2C write failed 0x%X"),ulRegAddr));
	//	return FALSE;
	//}

	i2ccfg_t i2c_cfg;
	i2c_cfg.bus_id = 1;
	i2c_cfg.chip_addr = (BYTE)TDA7541_ADDR_W;
	i2c_cfg.sub_addr  = ulRegAddr;
	i2c_cfg.flag      = I2C_WRITE;
	i2c_cfg.msg_buf   = (char*)pRegValBuf;
	i2c_cfg.msg_size  = uiValBufLen;

	HwI2CWrite(&i2c_cfg);

	return TRUE;
}

static BOOL I2C_Read_TDA7541(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo, UCHAR ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
{
	//I2CTRANS  Trans;
	//Trans.bRead = TRUE;
	//Trans.dwRegAddr = ulRegAddr;
	//Trans.pValueBuf = pRegValBuf;
	//Trans.uiValueLen = uiValBufLen;
	//Trans.dwClock = NORMAL_I2C_FREQ;
	//Trans.i2cdevinfo.pReadSeq = I2CReadSeq_TDA7541;
	//Trans.i2cdevinfo.uiReadSeqLen = sizeof(I2CReadSeq_TDA7541);
	//Trans.i2cdevinfo.pWriteSeq = I2CWriteSeq_NORMAL;
	//Trans.i2cdevinfo.uiWriteSeqLen = sizeof(I2CWriteSeq_NORMAL);
	//Trans.i2cdevinfo.ulDevAddrW = TDA7541_ADDR_W;
	//Trans.i2cdevinfo.ulDevAddrR = TDA7541_ADDR_R;

	//if(!pTDA7541RadioInfo->bOpen)
	//{
	//	return TRUE;
	//}

	//if (!I2CTransact(pTDA7541RadioInfo->TDA7541RadioI2CHandle, &Trans))
	//{
	//	DBGI((TEXT("\r\nFlyAudio 7541:I2C read failed %X"),ulRegAddr));
	//	return FALSE;
	//}

	i2ccfg_t i2c_cfg;
	i2c_cfg.bus_id = 1;
	i2c_cfg.chip_addr = (BYTE)TDA7541_ADDR_R;
	i2c_cfg.sub_addr  = ulRegAddr;
	i2c_cfg.flag      = I2C_READ;
	i2c_cfg.msg_buf   = (char*)pRegValBuf;
	i2c_cfg.msg_size  = uiValBufLen;

	HwI2CRead(&i2c_cfg);

	return TRUE;
}
static void TDA7541_WriteTunerRegister(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE start, BYTE end, BYTE *pBuffer)
{
	//DBGD((TEXT("\r\nFlyAudio prepare to write 7541's reg:")));
	//DBGD((TEXT("\r\n--addr:0x%x,--len:%d"),start,(end-start+1)));
	I2C_Write_TDA7541(pTDA7541RadioInfo,start,pBuffer,(end-start+1));
	if (start < end)
	{
		I2C_Write_TDA7541(pTDA7541RadioInfo,start|AUTO_INCREMENT_MODE,pBuffer,(end-start+1)); 
	}
	else if (start == end)
	{
		I2C_Write_TDA7541(pTDA7541RadioInfo,start,pBuffer,(end-start+1));
	}
}
static void TDA7541_UpdateInitTblBit(BYTE *InitTbl, BYTE start, BYTE end, BYTE data)
{
	UINT i;
	BYTE temp = 0;

	for(i=start; i<=end; i++)
	{
		temp |= (1 << i);
	}
	*InitTbl = (BYTE)(*InitTbl & (BYTE)~temp) | data;
}
static BYTE  _2NPower(BYTE sender)
{
	BYTE pow;
	switch(sender)
	{
	case 0:
		pow=0x01;
		break;
	case 1:
		pow=0x02;
		break;
	case 2:
		pow=0x04;
		break;
	case 3:
		pow=0x08;
		break;
	case 4:
		pow=0x10;
		break;
	case 5:
		pow=0x20;
		break;
	case 6:
		pow=0x40;
		break;
	case 7:
		pow=0x80;
		break;
	}
	return pow;
}
static BYTE TDA7541_GetTunerParameter(BYTE data,BYTE B_sta,BYTE B_sto)
{
	BYTE i,value0=0,temp,array0[8]={0};

	for(i=B_sta;i<=B_sto;i++)
	{
		temp = data & _2NPower(i);
		array0[i]=temp;
		value0|=array0[i];
	}	
	value0>>=B_sta;

	return value0;
}
static BYTE TDA7541_StereoInquiry(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	//BYTE mux;
	//mux=TDA7541_GetTunerParameter(pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+10], TDA7541_ADDR10_REG_MUX);
	//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+10],TDA7541_ADDR10_REG_MUX,3);
	//if(ST_QUALITY) 
	//{
	//	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+10],TDA7541_ADDR10_REG_MUX,mux);
	//	return 1;
	//}
	//else
	//{
	//	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+10],TDA7541_ADDR10_REG_MUX,mux);
	//	return 0;
	//}
}
static BYTE TDA7541_Calculate_TV_SM_Value(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,INT range,UINT RF_Freq, BYTE mode)
{
	UINT val;
	UINT sta_val,sto_val;
	UINT sta_freq,sto_freq;
	enumRadioArea Area;

	Area = pTDA7541RadioInfo->radioInfo.eCurRadioArea;
	if(Area==AREA_OIRT && RF_Freq<=74000)
	{
		if(mode==TV)
		{
			sta_val = pTDA7541RadioInfo->mParameterTable[OIRT_TV_OFFSET+range-1];
			sto_val = pTDA7541RadioInfo->mParameterTable[OIRT_TV_OFFSET+range];
		}
		else
		{
			sta_val = pTDA7541RadioInfo->mParameterTable[OIRT_SM_OFFSET+range-1];
			sto_val = pTDA7541RadioInfo->mParameterTable[OIRT_SM_OFFSET+range];
		}
		sta_freq = (UINT)((UINT)(pTDA7541RadioInfo->mParameterTable[OIRT_FREQ_OFFSET+range-1] + 640) * 100);
		sto_freq = (UINT)((UINT)(pTDA7541RadioInfo->mParameterTable[OIRT_FREQ_OFFSET+range] + 640) * 100);
	}
	else if(Area==AREA_JAPAN&& RF_Freq<=90000)
	{
		if(mode == TV)
		{
			sta_val = pTDA7541RadioInfo->mParameterTable[JAPAN_TV_OFFSET+range-1];
			sto_val = pTDA7541RadioInfo->mParameterTable[JAPAN_TV_OFFSET+range];
		}
		else
		{
			sta_val = pTDA7541RadioInfo->mParameterTable[JAPAN_SM_OFFSET+range-1];
			sto_val = pTDA7541RadioInfo->mParameterTable[JAPAN_SM_OFFSET+range];
		}
		sta_freq = (UINT)((UINT)(pTDA7541RadioInfo->mParameterTable[JAPAN_FREQ_OFFSET+range-1] + 760) * 100);
		sto_freq = (UINT)((UINT)(pTDA7541RadioInfo->mParameterTable[JAPAN_FREQ_OFFSET+range] + 760) * 100);
	}
	else
	{
		if(mode == TV)
		{
			sta_val = pTDA7541RadioInfo->mParameterTable[EU_TV_OFFSET+range-2];
			sto_val = pTDA7541RadioInfo->mParameterTable[EU_TV_OFFSET+range];
		}
		else
		{
			sta_val = pTDA7541RadioInfo->mParameterTable[EU_SM_OFFSET+range-1];
			sto_val = pTDA7541RadioInfo->mParameterTable[EU_SM_OFFSET+range];
		}
		sta_freq = (UINT)((UINT)(pTDA7541RadioInfo->mParameterTable[EU_FREQ_OFFSET+range-2] + 875) * 100);
		sto_freq = (UINT)((UINT)(pTDA7541RadioInfo->mParameterTable[EU_FREQ_OFFSET+range] + 875) * 100);
	}

	if(mode==TV)
	{
		sta_val=(sta_val<0x80)?(~sta_val)&0x7f:sta_val;
		sto_val=(sto_val<0x80)?(~sto_val)&0x7f:sto_val;
	}
	if(sta_val>=sto_val)
	{
		val=(UINT)((float) ((sto_freq-RF_Freq)*(sta_val-sto_val)/(sto_freq-sta_freq))+0.5);
		val=(sto_val+val);
	}
	else
	{
		val=(UINT)((float) ((RF_Freq-sta_freq)*(sto_val-sta_val)/(sto_freq-sta_freq))+0.5);
		val=(sta_val+val);
	}

	if(mode==TV)
	{
		return (val<0x80)?(~val)&0x7f:val;
	}
	else 
		return val;
}
static BYTE TDA7541_Get_TV_SM_Value(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE mode)
{
	INT i, align_num;
	UINT fr;
	enumRadioMode Band;
	enumRadioArea Area;
	UINT RF_Freq;

	Band = pTDA7541RadioInfo->radioInfo.eCurRadioMode;
	Area = pTDA7541RadioInfo->radioInfo.eCurRadioArea;
	RF_Freq = *pTDA7541RadioInfo->radioInfo.pCurRadioFreq;

    if(Band==FM1||Band==FM2)
    {
        RF_Freq = RF_Freq *10;
		if(Area==AREA_OIRT && RF_Freq<=74000)
        {
			align_num=(pTDA7541RadioInfo->mParameterTable[OIRT_EU_ANUM]>>4)&0x0f;//Get the 4 High Bits.It determined by the points of OIRT or EU.
			fr=(RF_Freq-64000)/10;//Range of value is from 0 to 1000.
			i=1;
			
            return TDA7541_Calculate_TV_SM_Value(pTDA7541RadioInfo,i, RF_Freq, mode);
        }
        else if(Area==AREA_JAPAN && RF_Freq<=90000)
        {
			align_num=(pTDA7541RadioInfo->mParameterTable[JPN_ANUM])&0x0f;
			fr=(RF_Freq-76000)/10;
			i=1;
			
            return TDA7541_Calculate_TV_SM_Value(pTDA7541RadioInfo,i, RF_Freq, mode);
        }
        else
        {
			align_num=(pTDA7541RadioInfo->mParameterTable[OIRT_EU_ANUM])&0x0f;
			fr=(RF_Freq-87500)/10;

			for(i=0;i<align_num;i++)
			{
				if(fr<=(UINT) pTDA7541RadioInfo->mParameterTable[EU_FREQ_OFFSET+i]*10) break;
			}
			if(i==0) i=1;
			
		return TDA7541_Calculate_TV_SM_Value(pTDA7541RadioInfo,i, RF_Freq, mode);
        }
    }
    else
    {
        if(mode==TV) return 0x80;
        else return 2;
    }
}
static BYTE TDA7541_GetCorrespondingSmeter(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	return TDA7541_Get_TV_SM_Value(pTDA7541RadioInfo,SMETER);
}

static BYTE TDA7541_GetCorrespondingTV(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	return TDA7541_Get_TV_SM_Value(pTDA7541RadioInfo,TV);
}
//y=ax+b(y单位是 mV)当FM时a=200;b=300 当AM时a=300;b=800
static UINT TDA7541_GetSmeterVoltage(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,enumRadioMode band)
{
	BYTE temp;
	UINT voltage;
	UINT adval;

	if(band != AM)
	{
		temp = (pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+10] >> 4) & 0x0f;
		DBGD((TEXT("\r\nSSTH----->%x"),temp));
		if(temp == 0)
		{
			return 0;
		}
		voltage = 200*temp + 300;	
	}
	else
	{
		temp = (pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+10] >> 4) & 0x0f;
		if(temp == 0)
		{
			return 0;
		}
		voltage = 300*temp + 800;		
	}
	adval = (1024 * voltage)/3300;
	return adval;
}
static UINT TDA7541_ADC_GetResult(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	BYTE timer;
	timer = 0;
	UINT adval;
	//pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRadioADReturn = FALSE;
	//IpcStartEvent(EVENT_GLOBAL_RADIO_AD_REQUEST_ID);
	//while ((!pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRadioADReturn) && (timer < 100))
	//{
	//	Sleep(10);
	//	timer++;
	//}
	//adval = pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioAD;

	HwADCGetValue(RADIO_ADC_CHANNEL5,&adval);
	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioAD = adval;
	
	DBGD((TEXT("\r\nTDA7541_ADC_GetResult----->%d,timer------>%d"),adval,timer));
	return pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioAD;
}
static UINT TDA7541_GetSmeter(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	return TDA7541_ADC_GetResult(pTDA7541RadioInfo);
}


void returnRadioPowerMode(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL bOn)
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
void returnRadioInitStatus(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL bOn)
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
/*
void returnRadioFreqStep(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL bOn)
{
	BYTE buf[] = {0x05,0x00};

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
void returnRadioScanDirection(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL bOn)
{
	BYTE buf[] = {0x06,0x00};

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
void returnRadioScanStatus(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL bOn)
{
	BYTE buf[] = {0x07,0x00};

	if (bOn)
	{
		buf[1] = 1;
	}
	else
	{
		buf[1] = 0;
	}

	ReturnToUser(buf,2);
}*/
void returnRadioFreq(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,UINT iFreq)
{
	BYTE buf[] = {0x10,0x00,0x00};

	buf[1] = iFreq >> 8;
	buf[2] = iFreq;

	ReturnToUser(buf,3);
}
void returnRadioMaxAndMinPara(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	BYTE buf[] = {0x00,0x00,0x00};

	buf[0] =0x40;
	buf[1] =pTDA7541RadioInfo->radioInfo.iFreqFMMax>>8;
	buf[2] =pTDA7541RadioInfo->radioInfo.iFreqFMMax;
	ReturnToUser(buf,3);

	buf[0] =0x41;
	buf[1] =pTDA7541RadioInfo->radioInfo.iFreqFMMin>>8;
	buf[2] =pTDA7541RadioInfo->radioInfo.iFreqFMMin;
	ReturnToUser(buf,3);

	buf[0] =0x42;
	buf[1] =pTDA7541RadioInfo->radioInfo.iFreqAMMax>>8;
	buf[2] =pTDA7541RadioInfo->radioInfo.iFreqAMMax;
	ReturnToUser(buf,3);

	buf[0] =0x43;
	buf[1] =pTDA7541RadioInfo->radioInfo.iFreqAMMin>>8;
	buf[2] =pTDA7541RadioInfo->radioInfo.iFreqAMMin;
	ReturnToUser(buf,3);

}
void returnRadioScanCtrl(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE cmd)
{
	BYTE buf[] = {0x13,0x00};

	buf[1] = cmd;

	ReturnToUser(buf,2);
}
void returnRadioHaveSearched(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL bHave)
{
	BYTE buf[] = {0x14,0x00};

	if (bHave)
	{
		buf[1] = 0;
	}

	ReturnToUser(buf,2);
}

void returnRadioMuteStatus(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL bOn)
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
void returnRadioAFStatus(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL bOn)
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


void returnRadioTAStatus(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL bOn)
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
void returnRadioBlinkingStatus(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL bOn)
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
void returnRadioMode(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,enumRadioMode eMode)
{
	BYTE buf[] = {0x20,0x00};

	buf[1] = eMode;

	ReturnToUser(buf,2);
}

void returnRadioRDSWorkStatus(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL bOn)
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
static UINT RadioStepFreqGenerate(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,enumRadioMode eMode,UINT iFreq,enumRadioStepDirection eForward,enumRadioStepMode eStepMode)
{
	if (STEP_FORWARD == eForward)//Forward
	{	
		if(AM != eMode)
		{
			if(STEP_MANUAL == eStepMode)//手动
			{
				iFreq += pTDA7541RadioInfo->radioInfo.iFreqFMManualStep;
			}
			else//自动
			{
				iFreq += pTDA7541RadioInfo->radioInfo.iFreqFMScanStep;
			}
			if(iFreq > pTDA7541RadioInfo->radioInfo.iFreqFMMax)	iFreq = pTDA7541RadioInfo->radioInfo.iFreqFMMin;
			if(iFreq < pTDA7541RadioInfo->radioInfo.iFreqFMMin)	iFreq = pTDA7541RadioInfo->radioInfo.iFreqFMMin;

		}
		else
		{
			if(STEP_MANUAL == eStepMode)//手动
			{
				iFreq += pTDA7541RadioInfo->radioInfo.iFreqAMManualStep;
			}
			else//自动
			{
				iFreq += pTDA7541RadioInfo->radioInfo.iFreqAMScanStep;
			}
			if(iFreq > pTDA7541RadioInfo->radioInfo.iFreqAMMax)	iFreq = pTDA7541RadioInfo->radioInfo.iFreqAMMin;
			if(iFreq < pTDA7541RadioInfo->radioInfo.iFreqAMMin)	iFreq = pTDA7541RadioInfo->radioInfo.iFreqAMMin;
		}
	}
	else if (STEP_BACKWARD == eForward)//Backward
	{
		if(AM != eMode)
		{
			if(STEP_MANUAL == eStepMode)//手动
			{
				iFreq -= pTDA7541RadioInfo->radioInfo.iFreqFMManualStep;
			}
			else//自动
			{
				iFreq -= pTDA7541RadioInfo->radioInfo.iFreqFMScanStep;
			}
			if(iFreq < pTDA7541RadioInfo->radioInfo.iFreqFMMin)	iFreq = pTDA7541RadioInfo->radioInfo.iFreqFMMax;
			if(iFreq > pTDA7541RadioInfo->radioInfo.iFreqFMMax)	iFreq = pTDA7541RadioInfo->radioInfo.iFreqFMMax;
		}
		else
		{
			if(STEP_MANUAL == eStepMode)//手动
			{
				iFreq -= pTDA7541RadioInfo->radioInfo.iFreqAMManualStep;
			}
			else// if(RadioScanStatus == 1)//自动
			{
				iFreq -= pTDA7541RadioInfo->radioInfo.iFreqAMScanStep;
			}
			if(iFreq < pTDA7541RadioInfo->radioInfo.iFreqAMMin)	iFreq = pTDA7541RadioInfo->radioInfo.iFreqAMMax;
			if(iFreq > pTDA7541RadioInfo->radioInfo.iFreqAMMax)	iFreq = pTDA7541RadioInfo->radioInfo.iFreqAMMax;
		}
	}
	else {
		if(AM != eMode)
		{
			if(iFreq > pTDA7541RadioInfo->radioInfo.iFreqFMMax)	iFreq = pTDA7541RadioInfo->radioInfo.iFreqFMMin;
			if(iFreq < pTDA7541RadioInfo->radioInfo.iFreqFMMin)	iFreq = pTDA7541RadioInfo->radioInfo.iFreqFMMin;
		}
		else
		{
			if(iFreq > pTDA7541RadioInfo->radioInfo.iFreqAMMax)	iFreq = pTDA7541RadioInfo->radioInfo.iFreqAMMin;
			if(iFreq < pTDA7541RadioInfo->radioInfo.iFreqAMMin)	iFreq = pTDA7541RadioInfo->radioInfo.iFreqAMMin;
		}
	}
	return iFreq;
}
static VOID TDA7541_Radio_Mute(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL ctrl)
{
	if (ctrl == TRUE)
	{
		TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0], TDA7541_ADDR0_REG_SDM,  TDA7541_STEREO_MUTE_ENABLE);  // mute
		TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+0], TDA7541_ADDR0_REG_SDM,  TDA7541_STEREO_MUTE_ENABLE);
	}
	else
	{
		TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0], TDA7541_ADDR0_REG_SDM,  TDA7541_STEREO_MUTE_DISABLE);  // demute
		TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+0], TDA7541_ADDR0_REG_SDM,  TDA7541_STEREO_MUTE_DISABLE);
	}
	TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR0_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0]);
}

static void ChargePumpSetting(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	enumRadioArea Area;
	enumRadioMode Band;

	Band = pTDA7541RadioInfo->radioInfo.eCurRadioMode;
	Area = pTDA7541RadioInfo->radioInfo.eCurRadioArea;
	switch(Band)
	{
	case FM1:
	case FM2:
		switch(Area)
		{
		case AREA_EU:
		case AREA_OIRT:
		case AREA_JAPAN:
		case AREA_USA_WB:
		case AREA_SAM:
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0], TDA7541_ADDR0_REG_ICP, TDA7541_PLL_HICURRENT_2MA);
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR0_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0]);
			break;
		}
		break;
	case AM:
		TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0], TDA7541_ADDR0_REG_ICP, TDA7541_PLL_HICURRENT_3MA);
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR0_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0]);
		break;			
	}
}
static void Set_Freq_7541(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,enumRadioMode mode,UINT freq)
{
	UINT Pc;
	BYTE Pcl,Pch;
	BYTE tv, sm;
	BYTE OIRT_Image;
	BYTE i;
	BYTE Temp[10] = {1};
	enumRadioArea Area;

	Area = pTDA7541RadioInfo->radioInfo.eCurRadioArea;
	DBGD((TEXT("\r\nFlyAudio set radiofreq1111111111111____%d____mode______%d___AREA____%d"),freq,mode,Area));
	if(mode != AM)
	{
		freq = freq*10;
		TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4], TDA7541_ADDR4_REG_FMON, TDA7541_FMMODE);//FM mode
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR4_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4]);
		//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+14], TDA7541_ADDR14_IF2Q, TDA7541_IF2Q_R5k_FMQ3);
		if (mode == FM1)
		{
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0], TDA7541_ADDR0_REG_LM, TDA7541_LOCAL_DISABLE);
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+14], TDA7541_ADDR14_IF1G, TDA7541_IF2_GAIN_21);
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+18], TDA7541_ADDR18_SBC, TDA7541_SBC_58_PERSENT);//控制开始分离
		} 
		else
		{		
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0], TDA7541_ADDR0_REG_LM, TDA7541_LOCAL_DISABLE);
			//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0], TDA7541_ADDR0_REG_LM, TDA7541_LOCAL_ENABLE);
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+14], TDA7541_ADDR14_IF1G, TDA7541_IF2_GAIN_21);
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+18], TDA7541_ADDR18_SBC, TDA7541_SBC_58_PERSENT);
		}
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR0_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0]);
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR14_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+14]);
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR18_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+18]);
		
		if (freq > 100000)
		{
			//SSTOP Config 
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+10], TDA7541_ADDR10_REG_SSTH, TDA7541_SSTOP_IFC_FM1_3_AM2_3);
		}
		else if (freq < 98100)
		{
			//SSTOP Config 
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+10], TDA7541_ADDR10_REG_SSTH, TDA7541_SSTOP_IFC_FM0_9_AM1_7);
		}
		else
		{
			//SSTOP Config 
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+10], TDA7541_ADDR10_REG_SSTH, TDA7541_SSTOP_IFC_FM1_1_AM2_0);
		}
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR10_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+10]);
		
		switch(Area)
		{
		case AREA_OIRT:
			if((freq>=64000)&&(freq<=74000))
			{
				Pc=((freq+IF)/10)-32;
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_RC, TDA7541_PLL_REFERECE_10KHz);//10k
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOD, TDA7541_VCO_DIVIDER_3);// vco 1/3 div
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOI, TDA7541_VCO_PHASE_180);// 180
				TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR5_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5]);

				tv=TDA7541_GetCorrespondingTV(pTDA7541RadioInfo);
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+3], TDA7541_ADDR3_REG_TV, tv);// tv value
				TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR3_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+3]);

				sm=TDA7541_GetCorrespondingSmeter(pTDA7541RadioInfo);
				//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+13], TDA7541_ADDR13_REG_SMETER, sm);//load smeter
				TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR13_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+13]);

				OIRT_Image = TDA7541_GetTunerParameter(pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+15],TDA7541_ADDR15_ORT_IQ);
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[OIRT_IQ_OFFESET],TDA7541_ADDR15_ORT_IQ,OIRT_Image);//OIRT Image
				TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR15_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+15]);

				//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4],TDA7541_ADDR4_REG_TVO,TDA7541_PLL_TVOFFSET_ENABLE);
				//TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR4_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4]);
				TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR4_REG, &pTDA7541RadioInfo->mParameterTable[OIRT_TVOP]);// load ORT +3.175 setting from eeprom

				TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR17_REG,&pTDA7541RadioInfo->mParameterTable[OIRT_SEP]);// load ORT sep.
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+19],TDA7541_ADDR19_DEEMP,TDA7541_DEEMP_50US);				
				//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+19],TDA7541_ADDR19_HIGH_CUT_MAX,TDA7541_HIGH_CUT_MAX_10DB);//Maximum high cut 10dB.
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+19],TDA7541_ADDR19_HIGH_CUT_MAX,TDA7541_HIGH_CUT_MAX_7_5DB);//Maximum high cut 7.5dB.
				TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR19_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+19]);
				break;
			}
		case AREA_EU:					
			Pc=((freq+IF)/50)-32;
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_RC, TDA7541_PLL_REFERECE_50KHz);//50k	
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOD, TDA7541_VCO_DIVIDER_2);// vco 1/2 div
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOI, TDA7541_VCO_PHASE_180);// 180
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR5_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5]);

			tv=TDA7541_GetCorrespondingTV(pTDA7541RadioInfo);
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+3], TDA7541_ADDR3_REG_TV, tv);// tv value
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR3_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+3]);

			sm=TDA7541_GetCorrespondingSmeter(pTDA7541RadioInfo);
			//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+13], TDA7541_ADDR13_REG_SMETER, sm);//load smeter
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR13_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+13]);

			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR15_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+15]);//FM Image

			//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4],TDA7541_ADDR4_REG_TVO,TDA7541_PLL_TVOFFSET_DISABLE);// disable +3.175
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR4_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4]);// load EU disable +3.175 setting	

			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR17_REG,&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+17]);// load Eu sep.
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+19],TDA7541_ADDR19_DEEMP,TDA7541_DEEMP_50US);				
			//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+19],TDA7541_ADDR19_HIGH_CUT_MAX,TDA7541_HIGH_CUT_MAX_10DB);//Maximum high cut 10dB.
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+19],TDA7541_ADDR19_HIGH_CUT_MAX,TDA7541_HIGH_CUT_MAX_7_5DB);//Maximum high cut 7.5dB.
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR19_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+19]);
			break;
		case AREA_JAPAN:    		
			if(freq>=74000 && freq<87500)
			{
				Pc=((freq-IF)/50)-32;
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_RC, TDA7541_PLL_REFERECE_50KHz);//50k		
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOD, TDA7541_VCO_DIVIDER_3);// vco 1/3 div
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOI, TDA7541_VCO_PHASE_0);// 0	
				TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR5_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5]);					
			}                			
			else if(freq>=64000 && freq<=74000)
			{
				Pc=((freq+IF)/50)-32;
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_RC, TDA7541_PLL_REFERECE_10KHz);//10k
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOD, TDA7541_VCO_DIVIDER_3);// vco 1/3 div
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOI, TDA7541_VCO_PHASE_180);// 180
				TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR5_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5]);
			}					
			else 
			{
				Pc=((freq+IF)/50)-32;
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_RC, TDA7541_PLL_REFERECE_50KHz);//50k
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOD, TDA7541_VCO_DIVIDER_3);// vco 1/3 div
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOI, TDA7541_VCO_PHASE_180);// 180
				TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR5_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5]);
			}					
			tv=TDA7541_GetCorrespondingTV(pTDA7541RadioInfo);
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+3], TDA7541_ADDR3_REG_TV, tv);// tv value
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR3_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+3]);

			sm=TDA7541_GetCorrespondingSmeter(pTDA7541RadioInfo);
			//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+13], TDA7541_ADDR13_REG_SMETER, sm);//load smeter
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR13_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+13]);

			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+11],TDA7541_ADDR11_WNON,TDA7541_GetTunerParameter(pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+11],TDA7541_ADDR11_WNON));// wb off
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR11_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+11]);

			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4],TDA7541_ADDR4_REG_TVO,TDA7541_PLL_TVOFFSET_DISABLE);// disable +3.175
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR4_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4]);// load EU disable +3.175 setting	
			break;
		case AREA_USA_WB:
			Pc=((freq+IF)/25)-32;
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_RC, TDA7541_PLL_REFERECE_25KHz);//25k
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOD, TDA7541_VCO_DIVIDER_ORIGINAL);// vco 1/1 div
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOI, TDA7541_VCO_PHASE_180);// 180
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR5_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5]);

			tv=TDA7541_GetCorrespondingTV(pTDA7541RadioInfo);
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+3], TDA7541_ADDR3_REG_TV, tv);// tv value
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR3_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+3]);

			sm=TDA7541_GetCorrespondingSmeter(pTDA7541RadioInfo);
			//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+13], TDA7541_ADDR13_REG_SMETER, sm);//load smeter
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR13_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+13]);

			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+11],TDA7541_ADDR11_WNON,~(TDA7541_GetTunerParameter(pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+11],TDA7541_ADDR11_WNON)));// wb on
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR11_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+11]);

			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4],TDA7541_ADDR4_REG_TVO,TDA7541_PLL_TVOFFSET_DISABLE);// disable +3.175
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR4_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4]);// load EU disable +3.175 setting	
			break;
		case AREA_SAM:
			Pc=((freq+IF)/50)-32;
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_RC, TDA7541_PLL_REFERECE_50KHz);//50k
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOD, TDA7541_VCO_DIVIDER_2);// vco 1/2 div
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOI, TDA7541_VCO_PHASE_180);// 180
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR5_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5]);

			tv=TDA7541_GetCorrespondingTV(pTDA7541RadioInfo);
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+3], TDA7541_ADDR3_REG_TV, tv);// tv value
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR3_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+3]);

			sm=TDA7541_GetCorrespondingSmeter(pTDA7541RadioInfo);
			//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+13], TDA7541_ADDR13_REG_SMETER, sm);//load smeter
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR13_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+13]);

			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4],TDA7541_ADDR4_REG_TVO,TDA7541_PLL_TVOFFSET_DISABLE);// disable +3.175
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR4_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4]);// load EU disable +3.175 setting	

			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+19],TDA7541_ADDR19_DEEMP,TDA7541_DEEMP_75US);				
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR19_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+19]);
			break;
		}
		Pcl=Pc&0xff;
		Pch=((Pc>>8)&0xff);
		TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+1], TDA7541_ADDR1_REG_PC_LSB, Pcl);
		TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+2], TDA7541_ADDR2_REG_PC_MSB, Pch);
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR1_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+1]);
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR2_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+2]);

		//TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR10_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+10]);
	}
	else//AM
	{
		TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+4], TDA7541_ADDR4_REG_FMON, TDA7541_AMMODE);//AM mode
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR4_REG, &pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+4]);
		//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+14], TDA7541_ADDR14_IF2Q, TDA7541_IF2Q_NOR_AMQ28_FMQ28);
		//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+14], TDA7541_ADDR14_IF1G, TDA7541_IF2_GAIN_9);
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR14_REG, &pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+14]);

		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR10_REG, &pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+10]);
		Pc=(freq+IF)-32;			

		TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+5], TDA7541_ADDR5_REG_RC, TDA7541_PLL_REFERECE_10KHz);
		TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOD, TDA7541_VCO_DIVIDER_2);// vco 1/2 div
		TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+5], TDA7541_ADDR5_REG_AMD, TDA7541_AM_PREDIVIDER_10);
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR5_REG, &pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+5]);
		Pcl=Pc&0xff;
		Pch=((Pc>>8)&0xff);
		TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+1], TDA7541_ADDR1_REG_PC_LSB, Pcl);
		TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+2], TDA7541_ADDR2_REG_PC_MSB, Pch);
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR1_REG, &pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+1]);
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR2_REG, &pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+2]);
		TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+11],TDA7541_ADDR11_WNON,TDA7541_GetTunerParameter(pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+11],TDA7541_ADDR11_WNON));// wb off
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR11_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+11]);
		switch(Area)
		{
		case AREA_OIRT:					
		case AREA_EU:
		case AREA_JAPAN:
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+19],TDA7541_ADDR19_DEEMP,TDA7541_DEEMP_50US);				
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR19_REG, &pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+19]);
			break;
		case AREA_SAM:
		case AREA_USA_WB:
			TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+19],TDA7541_ADDR19_DEEMP,TDA7541_DEEMP_75US);				
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR19_REG, &pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+19]);
			break;
		}		
	}
	ChargePumpSetting(pTDA7541RadioInfo);
	DBGD((TEXT("\r\nFlyAudio FM/AM TUNER_PARA_AREA_ADDR0_23\r\n")));
	for (i = 0;i < 24; i++)
	{
		if (mode != AM)
		{		
			DBGD((TEXT("\t [%d] 0x%X "), i, pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+i]));
		}
		else
		{
			DBGD((TEXT("\t [%d] 0x%X "), i, pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+i]));
		}
	}
	DBGD((TEXT("\r\nFlyAudio set radiofreq222222222222____%d____mode______%d___AREA____%d"),freq,mode,Area));
}

static VOID ReadParameterFromEEPROM(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE* pReadParameter)
{
	BYTE ReadData[INIT_DATA_SIZE];
	I2C_Read_EEPROM(pTDA7541RadioInfo, 0, ReadData, INIT_DATA_SIZE);
	for(UINT i=0; i<INIT_DATA_SIZE; i++)
	{
		pReadParameter[i]=ReadData[i];
	}
}
static VOID  LoadAlignedData2Tuner(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,enumRadioMode Band)
{
	switch(Band)
	{
	case FM1:
	case FM2:
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TUNER_PARA_AREA_ADDR0_23, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET]);
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TUNER_TESTING_CTROL_ADDR24_31, &FM_Test[0]);
		break;
	case AM:
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TUNER_PARA_AREA_ADDR0_23, &pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET]);
		TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TUNER_TESTING_CTROL_ADDR24_31, &AM_Test[0]);	
		break;
	default:break;
	}
}

static void recoveryRadioRegData(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	LoadAlignedData2Tuner(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.eCurRadioMode);
}

static void ParameterTable_Init(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	UINT i;
	//BYTE temp;
	//BYTE checksum1,checksum2,checksum3;

	//ReadParameterFromEEPROM(pTDA7541RadioInfo,pTDA7541RadioInfo->mParameterTable);
	//checksum1 = 0;
	//for(i=0; i<34; i++)			// 计算校验
	//{
	//	checksum1 += pTDA7541RadioInfo->mParameterTable[i];
	//}
	//checksum1 -= 1;
	//checksum1 &= 0xff;

	//checksum2 = 0;
	//for(i=34; i<58; i++)
	//{
	//	checksum2 += pTDA7541RadioInfo->mParameterTable[i];
	//}
	//checksum2 -= 1;
	//checksum2 &= 0xff;

	//checksum3 = 0;
	//for(i=58; i<141; i++)
	//{
	//	checksum3 += pTDA7541RadioInfo->mParameterTable[i];
	//}
	//checksum3 -= 1;	
	//checksum3 &= 0xff;

	//if((pTDA7541RadioInfo->mParameterTable[0] != 0xca) || (pTDA7541RadioInfo->mParameterTable[FM_SUM] != checksum1) || 	   // 判断数据是否有错
	//	(pTDA7541RadioInfo->mParameterTable[AM_SUM] != checksum2) || (pTDA7541RadioInfo->mParameterTable[AN_SUM] != checksum3))	 
	//{	// 有错使用默认数据
	//	for(i=0; i<INIT_DATA_SIZE; i++)
	//	{
	//		pTDA7541RadioInfo->mParameterTable[i] = DefaultData[i];
	//	}
	//}	
	for (i=0; i<INIT_DATA_SIZE; i++)
	{
		pTDA7541RadioInfo->mParameterTable[i] = DefaultData[i];
	}
	//DBGD((TEXT("\r\nFlyAudio mParameterTable data[0] == %x"),pTDA7541RadioInfo->mParameterTable[0]));
	//DBGD((TEXT("\r\nFlyAudio mParameterTable checksum1 == %x[%x]"),checksum1,pTDA7541RadioInfo->mParameterTable[FM_SUM]));
	//DBGD((TEXT("\r\nFlyAudio mParameterTable checksum2 == %x[%x]"),checksum2,pTDA7541RadioInfo->mParameterTable[AM_SUM]));
	//DBGD((TEXT("\r\nFlyAudio mParameterTable checksum3 == %x[%x]\r\n"),checksum3,pTDA7541RadioInfo->mParameterTable[AN_SUM]));

	//FM
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0], TDA7541_ADDR0_REG_LDENA, TDA7541_PLL_LOCK_ENABLE);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0], TDA7541_ADDR0_REG_SDM, TDA7541_STEREO_MUTE_ENABLE);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0], TDA7541_ADDR0_REG_LM, TDA7541_LOCAL_DISABLE);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0], TDA7541_ADDR0_REG_ASFC, TDA7541_ASFC_NORMAL);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0], TDA7541_ADDR0_REG_SEEK, TDA7541_SEEK_OFF);

	pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+3] = 0x98;

	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+20], TDA7541_ADDR20_MPFAST, TDA7541_MPTC_ENABLE);

	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4], TDA7541_ADDR4_REG_TVM, TDA7541_PLL_TV_TRACK);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4], TDA7541_ADDR4_REG_TVO, TDA7541_PLL_TVOFFSET_DISABLE);

	//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4], TDA7541_ADDR4_REG_ISSENA, TDA7541_ISS_ENABLE);

	//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4], TDA7541_ADDR4_REG_IFS, TDA7541_PLL_TSAMPLE_FM10_24_AM64);
	//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4], TDA7541_ADDR4_REG_EW, TDA7541_PLL_IFC_EW_FM100_AM16K);

	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4], TDA7541_ADDR4_REG_IFS, TDA7541_PLL_TSAMPLE_FM20_48_AM128);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4], TDA7541_ADDR4_REG_EW, TDA7541_PLL_IFC_EW_FM12_5_AM2K);

	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+4], TDA7541_ADDR4_REG_FMON, TDA7541_FMMODE);

	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOD, TDA7541_VCO_DIVIDER_2);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOI, TDA7541_VCO_PHASE_180);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+5], TDA7541_ADDR5_REG_RC, TDA7541_PLL_REFERECE_50KHz);

	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+11], TDA7541_ADDR11_WNON, TDA7541_WB_DISABLE);

	//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+11], TDA7541_ADDR11_WMTH, TDA7541_WMTH_STARTMUTE_1);

	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+13], TDA7541_ADDR13_REG_SMETER, TDA7541_REG_SMETER_SLOPE_125V_20DB);
	//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+13], TDA7541_ADDR13_REG_XTAL, TDA7541_XTAL_24_75PF);

	//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+14], TDA7541_ADDR14_IF2A, TDA7541_IF2A_30_8PF);
	pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+14] = 0xEF;

	//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+15], TDA7541_ADDR15_PH, TDA7541_PH_3_DEGREE);

	//SSTOP Config 
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+10], TDA7541_ADDR10_REG_SSTH, TDA7541_SSTOP_IFC_FM1_1_AM2_0);

	//AM
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+0], TDA7541_ADDR0_REG_LDENA, TDA7541_PLL_LOCK_ENABLE);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+0], TDA7541_ADDR0_REG_SDM, TDA7541_STEREO_MUTE_ENABLE);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+0], TDA7541_ADDR0_REG_LM, TDA7541_LOCAL_DISABLE);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+0], TDA7541_ADDR0_REG_ASFC, TDA7541_ASFC_NORMAL);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+0], TDA7541_ADDR0_REG_SEEK, TDA7541_SEEK_OFF);

	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+20], TDA7541_ADDR20_MPFAST, TDA7541_MPTC_ENABLE);

	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+4], TDA7541_ADDR4_REG_TVM, TDA7541_PLL_TV_TRACK);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+4], TDA7541_ADDR4_REG_TVO, TDA7541_PLL_TVOFFSET_DISABLE);

	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+4], TDA7541_ADDR4_REG_IFS, TDA7541_PLL_TSAMPLE_FM10_24_AM64);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+4], TDA7541_ADDR4_REG_EW, TDA7541_PLL_IFC_EW_FM100_AM16K);

	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+4], TDA7541_ADDR4_REG_FMON, TDA7541_AMMODE);									

	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOD, TDA7541_VCO_DIVIDER_2);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+5], TDA7541_ADDR5_REG_VCOI, TDA7541_VCO_PHASE_180);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+5], TDA7541_ADDR5_REG_RC, TDA7541_PLL_REFERECE_10KHz);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+5], TDA7541_ADDR5_REG_AMD, TDA7541_AM_PREDIVIDER_10);


	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+11], TDA7541_ADDR11_WNON, TDA7541_WB_DISABLE); 

	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+11], TDA7541_ADDR11_WMTH, TDA7541_WMTH_STARTMUTE_3);

	pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+14] = 0x45;

	//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+10], TDA7541_ADDR10_REG_SSTH, TDA7541_SSTOP_IFC_FM0_7_AM1_4);
	TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+10], TDA7541_ADDR10_REG_SSTH, TDA7541_SSTOP_IFC_FM0_5_AM1_1);

	/* 以下3条程序是改正分离度问题 */
	//pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+15] = 0x00;
	pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+17] = 0x87;
	//pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+20] = 0x89;
	//pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+21] = 0x0e;
	//pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+22] = 0xc4;

	LoadAlignedData2Tuner(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.eCurRadioMode);
}


static VOID TDA7541Radio_ChangeToFMAM(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,enumRadioMode mode)
{
	DBGD((TEXT("\r\nFlyAudio TDA7541Radio_ChangeToFMAM")));

	Set_Freq_7541(pTDA7541RadioInfo,mode,*pTDA7541RadioInfo->radioInfo.pCurRadioFreq);
	pTDA7541RadioInfo->bCurMute = TRUE;//触发恢复静音状态
}

static void defaultConfigInit(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	IO_Control_Init(pTDA7541RadioInfo);
	TDA7541Radio_ChangeToFMAM(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.eCurRadioMode);
	//returnRadioMode(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.eCurRadioMode);
	//returnRadioFreq(pTDA7541RadioInfo,*(pTDA7541RadioInfo->radioInfo.pCurRadioFreq));
	//returnRadioTAStatus(pTDA7541RadioInfo,pTDA7541RadioInfo->RDSInfo.RadioRDSTAControlOn);
	//returnRadioAFStatus(pTDA7541RadioInfo,pTDA7541RadioInfo->RDSInfo.RadioRDSAFControlOn);

	//if (pTDA7541RadioInfo->bPreMute == TRUE)
	//{
	//	returnRadioMuteStatus(pTDA7541RadioInfo,FALSE);
	//}	
	//else
	//{
	//	returnRadioMuteStatus(pTDA7541RadioInfo,TRUE);
	//}
	if (pTDA7541RadioInfo->bPowerUp)
	{
		SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
	}
}
static VOID DealWinceCmd(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE *pdata,UINT len)
{
	UINT iTemp;

	switch(pdata[0])
		{
		case 0x01:
			if (0x01 == pdata[1])//初始化命令开始
			{
				pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNeedProcVoltageShakeRadio = 85;

				ParameterTable_Init(pTDA7541RadioInfo);

				defaultConfigInit(pTDA7541RadioInfo);

				RegDataReadRadio(pTDA7541RadioInfo);

				radioAFMuteControl(pTDA7541RadioInfo,FALSE);

				TDA7541RadioJumpNewFreqParaInit(pTDA7541RadioInfo);

				returnRadioPowerMode(pTDA7541RadioInfo,TRUE);
				returnRadioInitStatus(pTDA7541RadioInfo,TRUE);
				returnRadioMaxAndMinPara(pTDA7541RadioInfo);

				pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditRadio = TRUE;

				pTDA7541RadioInfo->bPowerUp = TRUE;
				SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);//激活一次
			}
			else if (0x00 == pdata[1])
			{
				returnRadioPowerMode(pTDA7541RadioInfo,FALSE);
			}
			break;
		case 0x03://软件模拟按键 1-FM1 2-FM2 3-AM 4-STOP RADIO 5-AF 6-TA
			switch(pdata[1])
			{
			case 0x01:
			case 0x02:
			case 0x03:
				if (pTDA7541RadioInfo->radioInfo.bPreScaning)
				{
					pTDA7541RadioInfo->radioInfo.bPreScaning = FALSE;
				}
				if (pTDA7541RadioInfo->bPowerUp)//直到之前的扫描停止
				{	
					SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
					while (pTDA7541RadioInfo->radioInfo.bCurScaning)
					{
						Sleep(100);
					}
				}
				if (0x01 == pdata[1])
				{
					pTDA7541RadioInfo->radioInfo.ePreRadioMode = FM1;
					DBGD((TEXT("\r\nFlyAudio Radio set mode --------FM1")));
				} 
				else if (0x02 == pdata[1])
				{
					pTDA7541RadioInfo->radioInfo.ePreRadioMode = FM2;
					DBGD((TEXT("\r\nFlyAudio Radio set mode --------FM2")));
				}
				else if (0x03 == pdata[1])
				{
					pTDA7541RadioInfo->radioInfo.ePreRadioMode = AM;
					DBGD((TEXT("\r\nFlyAudio Radio set mode --------AM")));
				}

				returnRadioMode(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.ePreRadioMode);
				if (AM == pTDA7541RadioInfo->radioInfo.ePreRadioMode)
				{
					returnRadioFreq(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.iPreRadioFreqAM);
				}
				else if (FM1 == pTDA7541RadioInfo->radioInfo.ePreRadioMode)
				{
					returnRadioFreq(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM1);
				}
				else if (FM2 == pTDA7541RadioInfo->radioInfo.ePreRadioMode)
				{
					returnRadioFreq(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM2);
				}

				pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPreRadioChannel = pTDA7541RadioInfo->radioInfo.ePreRadioMode;
				if (pTDA7541RadioInfo->bPowerUp)
				{	
					SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
				}
				break;
			case 0x04:
				pTDA7541RadioInfo->radioInfo.bPreScaning = !pTDA7541RadioInfo->radioInfo.bPreScaning;
				if (pTDA7541RadioInfo->bPowerUp)
				{
					SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
				}
				break;
			case 0x05:
				pTDA7541RadioInfo->RDSInfo.RadioRDSAFControlOn = !pTDA7541RadioInfo->RDSInfo.RadioRDSAFControlOn;
				returnRadioAFStatus(pTDA7541RadioInfo,pTDA7541RadioInfo->RDSInfo.RadioRDSAFControlOn);
				if (pTDA7541RadioInfo->bPowerUp)
				{
					SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
				}
				break;
			case 0x06:
				pTDA7541RadioInfo->RDSInfo.RadioRDSTAControlOn = !pTDA7541RadioInfo->RDSInfo.RadioRDSTAControlOn;
				returnRadioTAStatus(pTDA7541RadioInfo,pTDA7541RadioInfo->RDSInfo.RadioRDSTAControlOn);
				if (pTDA7541RadioInfo->bPowerUp)
				{
					SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
				}
				break;
			case 0x07:
				if (pTDA7541RadioInfo->radioInfo.bPreScaning)
				{
					pTDA7541RadioInfo->radioInfo.bPreScaning = FALSE;
				}
				if (pTDA7541RadioInfo->radioInfo.ePreRadioMode == pTDA7541RadioInfo->radioInfo.eCurRadioMode)
				{
					*pTDA7541RadioInfo->radioInfo.pPreRadioFreq = 
						RadioStepFreqGenerate(pTDA7541RadioInfo
						,pTDA7541RadioInfo->radioInfo.ePreRadioMode
						,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq
						,STEP_FORWARD
						,STEP_MANUAL);
					DBGD((TEXT("\r\nFlyAudio TDA7541Radio set freq value:%d"),*pTDA7541RadioInfo->radioInfo.pPreRadioFreq));
						returnRadioFreq(pTDA7541RadioInfo,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq);
					if (pTDA7541RadioInfo->bPowerUp)
					{
						SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
					}
				}
				break;
			case 0x08:
				if (pTDA7541RadioInfo->radioInfo.bPreScaning)
				{
					pTDA7541RadioInfo->radioInfo.bPreScaning = FALSE;
				}
				if (pTDA7541RadioInfo->radioInfo.ePreRadioMode == pTDA7541RadioInfo->radioInfo.eCurRadioMode)
				{
					*pTDA7541RadioInfo->radioInfo.pPreRadioFreq = 
						RadioStepFreqGenerate(pTDA7541RadioInfo
						,pTDA7541RadioInfo->radioInfo.ePreRadioMode
						,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq
						,STEP_BACKWARD
						,STEP_MANUAL);
					DBGD((TEXT("\r\nFlyAudio TDA7541Radio set freq value:%d"),*pTDA7541RadioInfo->radioInfo.pPreRadioFreq));
					returnRadioFreq(pTDA7541RadioInfo,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq);
					if (pTDA7541RadioInfo->bPowerUp)
					{
						SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
					}
				}
				break;
			default:
				DBGD((TEXT("\r\nFlyAudio Tda7541 user command 0x03 %X"),pdata[1]));
				break;
			}
			break;
		case 0x10://设置收音机频率 
			if (pTDA7541RadioInfo->radioInfo.bPreScaning)
			{
				pTDA7541RadioInfo->radioInfo.bPreScaning = FALSE;
			}
			iTemp = pdata[1]*256+pdata[2];
			iTemp = 
				RadioStepFreqGenerate(pTDA7541RadioInfo
				,pTDA7541RadioInfo->radioInfo.ePreRadioMode
				,iTemp
				,STEP_NONE,STEP_MANUAL);
			if (AM == pTDA7541RadioInfo->radioInfo.ePreRadioMode)
			{
				pTDA7541RadioInfo->radioInfo.iPreRadioFreqAM = iTemp;
			}
			else if (FM1 == pTDA7541RadioInfo->radioInfo.ePreRadioMode)
			{
				pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM1 = iTemp;
			}
			else if (FM2 == pTDA7541RadioInfo->radioInfo.ePreRadioMode)
			{
				pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM2 = iTemp;
			}
			DBGD((TEXT("\r\nFlyAudio TDA7541Radio set freq value:%d"),iTemp));
			returnRadioFreq(pTDA7541RadioInfo,iTemp);
			if (pTDA7541RadioInfo->bPowerUp)
			{
				SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
			}
			break;
		case 0x11://频点+
			if (pTDA7541RadioInfo->radioInfo.bPreScaning)
			{
				pTDA7541RadioInfo->radioInfo.bPreScaning = FALSE;
			}
			pTDA7541RadioInfo->radioInfo.eButtomStepDirection = STEP_FORWARD;
			if (0x00 == pdata[1])
			{
				//ReturnToUser(pdata,len);
				buttomJumpFreqAndReturn(pTDA7541RadioInfo);
				pTDA7541RadioInfo->radioInfo.bPreStepButtomDown = TRUE;
				pTDA7541RadioInfo->radioInfo.iButtomStepCount = 0;
			}
			else if (0x01 == pdata[1])
			{
				//ReturnToUser(pdata,len);
				pTDA7541RadioInfo->radioInfo.bPreStepButtomDown = FALSE;
			}
			if (pTDA7541RadioInfo->bPowerUp)
			{
				SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
			}
			break;
		case 0x12://频点-
			if (pTDA7541RadioInfo->radioInfo.bPreScaning)
			{
				pTDA7541RadioInfo->radioInfo.bPreScaning = FALSE;
			}
			pTDA7541RadioInfo->radioInfo.eButtomStepDirection = STEP_BACKWARD;
			if (0x00 == pdata[1])
			{
				//ReturnToUser(pdata,len);
				buttomJumpFreqAndReturn(pTDA7541RadioInfo);
				pTDA7541RadioInfo->radioInfo.bPreStepButtomDown = TRUE;
				pTDA7541RadioInfo->radioInfo.iButtomStepCount = 0;
			}
			else if (0x01 == pdata[1])
			{
				//ReturnToUser(pdata,len);
				pTDA7541RadioInfo->radioInfo.bPreStepButtomDown = FALSE;
			}
			if (pTDA7541RadioInfo->bPowerUp)
			{
				SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
			}
			break;
		case 0x13://扫描控制
			switch(pdata[1])
			{
			case 0x00:
			case 0x01:
				pTDA7541RadioInfo->radioInfo.bPreScaning = TRUE;
				pTDA7541RadioInfo->radioInfo.eScanDirection = STEP_FORWARD;
				pTDA7541RadioInfo->radioInfo.bScanRepeatFlag = FALSE;
				break;
			case 0x02:
				pTDA7541RadioInfo->radioInfo.bPreScaning = TRUE;
				pTDA7541RadioInfo->radioInfo.eScanDirection = STEP_BACKWARD;
				pTDA7541RadioInfo->radioInfo.bScanRepeatFlag = FALSE;
				break;
			case 0x03:
				pTDA7541RadioInfo->radioInfo.bPreScaning = FALSE;
				break;
			case 0x04:
			case 0x05:
				pTDA7541RadioInfo->radioInfo.bPreScaning = TRUE;
				pTDA7541RadioInfo->radioInfo.eScanDirection = STEP_FORWARD;
				pTDA7541RadioInfo->radioInfo.bScanRepeatFlag = TRUE;
				break;
			case 0x06:
				pTDA7541RadioInfo->radioInfo.bPreScaning = TRUE;
				pTDA7541RadioInfo->radioInfo.eScanDirection = STEP_BACKWARD;
				pTDA7541RadioInfo->radioInfo.bScanRepeatFlag = TRUE;
				break;
			default:break;
			}
			returnRadioScanCtrl(pTDA7541RadioInfo,pdata[1]);
			if (pTDA7541RadioInfo->bPowerUp)
			{
				SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
			}
			DBGD((TEXT("\r\nFlyAudio TDA7541Radio set scan ctrl:%d"),pdata[1]));
			break;
		case 0x15://开始收音
			if (0x00 == pdata[1])
			{
				pTDA7541RadioInfo->bPreMute = TRUE;
				if (pTDA7541RadioInfo->radioInfo.bPreScaning)
				{
					pTDA7541RadioInfo->radioInfo.bPreScaning = FALSE;
				}
				returnRadioMuteStatus(pTDA7541RadioInfo,FALSE);
			}
			else if (0x01 == pdata[1])
			{
				pTDA7541RadioInfo->bPreMute = FALSE;
				returnRadioMuteStatus(pTDA7541RadioInfo,TRUE);
			}				
			if (pTDA7541RadioInfo->bPowerUp)
			{
				SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
			}			
			break;
		case 0x16://AF开关
			if (0x01 == pdata[1])
			{
				pTDA7541RadioInfo->RDSInfo.RadioRDSAFControlOn = TRUE;
				returnRadioAFStatus(pTDA7541RadioInfo,TRUE);
			}
			else if (0x00 == pdata[1])
			{
				pTDA7541RadioInfo->RDSInfo.RadioRDSAFControlOn = FALSE;
				returnRadioAFStatus(pTDA7541RadioInfo,FALSE);
			}
			if (pTDA7541RadioInfo->bPowerUp)
			{
				SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
			}
			break;
		case 0x17://TA开关
			if (0x01 == pdata[1])
			{
				pTDA7541RadioInfo->RDSInfo.RadioRDSTAControlOn = TRUE;
				returnRadioTAStatus(pTDA7541RadioInfo,TRUE);
			}
			else if (0x00 == pdata[1])
			{
				pTDA7541RadioInfo->RDSInfo.RadioRDSTAControlOn = FALSE;
				returnRadioTAStatus(pTDA7541RadioInfo,FALSE);
			}
			if (pTDA7541RadioInfo->bPowerUp)
			{
				SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
			}
			break;
		case 0x30://交通广播开关
			if (0x01 == pdata[1])
			{
				pTDA7541RadioInfo->RDSInfo.RadioRDSAFControlOn = TRUE;
				returnRadioRDSWorkStatus(pTDA7541RadioInfo,TRUE);				
			}
			else if (0x00 == pdata[1])
			{
				pTDA7541RadioInfo->RDSInfo.RadioRDSAFControlOn = FALSE;
				returnRadioRDSWorkStatus(pTDA7541RadioInfo,FALSE);
			}
			if (pTDA7541RadioInfo->bPowerUp)
			{
				SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
			}
			break;
		case 0xFF:
			if (0x01 == pdata[1])
			{
				FRA_PowerUp((DWORD)pTDA7541RadioInfo);
			} 
			else if (0x00 == pdata[1])
			{
				FRA_PowerDown((DWORD)pTDA7541RadioInfo);
			}
			break;
		default:
			DBGD((TEXT("\r\nFlyAudio Tda7541 user command unhandle %X"),pdata[1]));
			break;
		}
}
//
//static VOID TDA7541RadioCommandProcessor(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE *buf,UINT len)
//{
//	static BYTE checksum;
//	static UINT FrameLen;
//	static UINT FrameLenMax;
//	static BYTE ReceiveReg[256];
//
//	for(UINT i = 0;i < len;i++)
//	{
//		switch (pTDA7541RadioInfo->buffToDriversProcessorStatus)
//		{
//		case 0:
//			if(0xFF == buf[i])
//			{
//				pTDA7541RadioInfo->buffToDriversProcessorStatus = 1;
//			}
//			break;
//		case 1:
//			if(0x55 == buf[i])
//			{
//				pTDA7541RadioInfo->buffToDriversProcessorStatus = 2;
//			}
//			else
//			{
//				pTDA7541RadioInfo->buffToDriversProcessorStatus = 0;
//			}
//			break;
//		case 2:
//			pTDA7541RadioInfo->buffToDriversProcessorStatus = 3;
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
//					DealWinceCmd(pTDA7541RadioInfo,ReceiveReg,FrameLen);
//				}
//				pTDA7541RadioInfo->buffToDriversProcessorStatus = 0;
//			}
//			break;
//		default:
//			pTDA7541RadioInfo->buffToDriversProcessorStatus = 0;
//			break;
//		}
//	}
//}
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
static BOOL READ_IO_SSTOP_IN(void)
{
	return HwGpioInput(IO_TDA7541_SSTOP_I);
}
DWORD WINAPI TDA7541_SSTOP_io_isr(LPVOID lpvParam)
{
	P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo = (P_FLY_TDA7541_RADIO_INFO)lpvParam;

	pTDA7541RadioInfo->bInterruptEventSSTOPRunning = TRUE;

	DBGD((TEXT("\r\nTDA7541_SSTOP_io_isr")));

	while(pTDA7541RadioInfo->hInterruptEventSSTOP)
	{
		WaitForSingleObject(pTDA7541RadioInfo->hInterruptEventSSTOP, INFINITE);
		DBGD((TEXT("\r\nSSTOP isr running")));

		pTDA7541RadioInfo->TDA7541_IO_info.iSSTOPDecCount++;
		if (pTDA7541RadioInfo->hInterruptEventSSTOP && pTDA7541RadioInfo->TDA7541_IO_info.iSSTOPDecCount < 20)
		{
			//SOC_IO_ISR_ReEn(pTDA7541RadioInfo->hInterruptEventSSTOP);
		}
	}

	pTDA7541RadioInfo->bInterruptEventSSTOPRunning = FALSE;

	return NULL;
}
static BOOL bRadioSignalGood(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,enumRadioMode radioMode,UINT *pLevel)
{
	UINT sm, smth;
	UINT cnt,high_flag;

	cnt = 0;
	sm = 0;
	high_flag = 0;
	*pLevel = high_flag;

	if(radioMode != AM)
	{
		smth = TDA7541_GetSmeterVoltage(pTDA7541RadioInfo,FM1);
	}
	else 
	{
		smth = TDA7541_GetSmeterVoltage(pTDA7541RadioInfo,AM);
	}

	sm = TDA7541_GetSmeter(pTDA7541RadioInfo);
	DBGD((TEXT("\r\nFlyAudio Radio Signal sm_ad=%d ;smth_ad=%d ;sm_mV=%d ;smth_mV=%d"),sm,smth,sm*3300/1024,smth*3300/1024));
		if (radioMode != AM)
		{
			if(sm < smth)
			{		
				return FALSE;
			}
		}
		else//AM
		{
			if (pTDA7541RadioInfo->radioInfo.iCurRadioFreqAM < 900)
			{
				if(sm < 330)
				{		
					return FALSE;
				}
			}
			else if((900 < pTDA7541RadioInfo->radioInfo.iCurRadioFreqAM) && (pTDA7541RadioInfo->radioInfo.iCurRadioFreqAM < 1300))
			{
				if(sm < 305)
				{		
					return FALSE;
				}
			}
			else
			{
				if(sm < 290)
				{		
					return FALSE;
				}
			}	
		}
	Sleep(24);
	IO_Control_Init(pTDA7541RadioInfo);
	while (cnt<100)
	{
		if (READ_IO_SSTOP_IN())
		{
			high_flag++;
		}
		cnt++;
		//Sleep(1);
	}	
	DBGD((TEXT("\r\nFlyAudio Radio Signal sstop persent %d of 100"),high_flag));
	if(radioMode != AM)
	{
		if (high_flag<88)
		{
			return FALSE;
		}
	}
	else  //AM
	{
		if (high_flag<50)        //调节AM停台灵敏度
		{
			return FALSE;
		}
	}
	*pLevel = (high_flag + sm);
	return TRUE;
}
void TDA7541RadioJumpNewFreqParaInit(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
#if RADIO_RDS
	RDSParaInit(pTDA7541RadioInfo);
#endif
}

static void buttomJumpFreqAndReturn(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)//界面按键跳频用
{
	*pTDA7541RadioInfo->radioInfo.pPreRadioFreq = 
		RadioStepFreqGenerate(pTDA7541RadioInfo
		,pTDA7541RadioInfo->radioInfo.ePreRadioMode
		,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq
		,pTDA7541RadioInfo->radioInfo.eButtomStepDirection
		,STEP_MANUAL);
	//Set_Freq_7541(pTDA7541RadioInfo
	//	,pTDA7541RadioInfo->radioInfo.ePreRadioMode
	//	,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq);
	returnRadioFreq(pTDA7541RadioInfo,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq);
}
static UINT GetCorrectScanStartFreq(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,UINT *pFreq)
{
	UINT base;
	if (AM != pTDA7541RadioInfo->radioInfo.eCurRadioMode)
	{
		if (((*pFreq - pTDA7541RadioInfo->radioInfo.iFreqFMMin) % pTDA7541RadioInfo->radioInfo.iFreqFMScanStep) != 0)
		{
			base = 1;
		}
		else
		{
			base = 0;
		}
		*pFreq = pTDA7541RadioInfo->radioInfo.iFreqFMMin + (base + (*pFreq - pTDA7541RadioInfo->radioInfo.iFreqFMMin)/pTDA7541RadioInfo->radioInfo.iFreqFMScanStep)*pTDA7541RadioInfo->radioInfo.iFreqFMScanStep;
	}
	else
	{
		if (((*pFreq - pTDA7541RadioInfo->radioInfo.iFreqAMMin) % pTDA7541RadioInfo->radioInfo.iFreqAMScanStep) != 0)
		{
			base = 1;
		}
		else
		{
			base = 0;
		}		
		*pFreq = pTDA7541RadioInfo->radioInfo.iFreqAMMin + (base + (*pFreq - pTDA7541RadioInfo->radioInfo.iFreqAMMin)/pTDA7541RadioInfo->radioInfo.iFreqAMScanStep)*pTDA7541RadioInfo->radioInfo.iFreqAMScanStep;
	}
	//if (((*pFreq - pTDA7541RadioInfo->radioInfo.iFreqFMMin) % pTDA7541RadioInfo->radioInfo.iFreqFMScanStep) != 0)
	//{
	//	if (AM != pTDA7541RadioInfo->radioInfo.eCurRadioMode)
	//	{
	//		*pFreq = *pFreq + pTDA7541RadioInfo->radioInfo.iFreqFMScanStep;
	//	}
	//	else
	//	{
	//		*pFreq = *pFreq + pTDA7541RadioInfo->radioInfo.iFreqAMScanStep;
	//	}

	//}
	return *pFreq;
}
static DWORD WINAPI TDA7541RadioScanThread(LPVOID pContext)
{
	P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo = (P_FLY_TDA7541_RADIO_INFO)pContext;
	ULONG WaitReturn;
	UINT iScanStartFreq;
	UINT m_RF_Freq;
	BOOL bHaveSearched;
	UINT iHaveSearchedLevel,iTempHaveSearchedLevel;
	ULONG nowTimer,lastTimer;
	ULONG iThreadFreqStepWaitTime = 0;

	BYTE iBlinkingTimes;
	BOOL bBlinkingStatus;


	DBGD((TEXT("\r\nFlyAudio TDA7541RadioScanThread in")));
	while (!pTDA7541RadioInfo->bKillDispatchScanThread)
	{
		if (0 == iThreadFreqStepWaitTime)
		{
			WaitReturn = WaitForSingleObject(pTDA7541RadioInfo->hDispatchScanThreadEvent, INFINITE);
		}
		else
		{
			WaitReturn = WaitForSingleObject(pTDA7541RadioInfo->hDispatchScanThreadEvent, iThreadFreqStepWaitTime);
		}
		iThreadFreqStepWaitTime = 0;
		DBGD((TEXT("\r\nFlyAudio TDA7541RadioScanThread Running Start%d"),iThreadFreqStepWaitTime));


		DBGD((TEXT("\r\nbCurScaning---->%d,bPreScaning---->%d"),pTDA7541RadioInfo->radioInfo.bCurScaning,pTDA7541RadioInfo->radioInfo.bPreScaning));

		if (pTDA7541RadioInfo->radioInfo.bCurScaning == TRUE && pTDA7541RadioInfo->radioInfo.bPreScaning == FALSE)//跳出搜台
		{
			DBGD((TEXT("\r\nFlyAudio TDA7541RadioScanThread --------------exit scaning")));
			pTDA7541RadioInfo->radioInfo.bCurScaning = pTDA7541RadioInfo->radioInfo.bPreScaning;
			//TDA7541_UpdateInitTblBit(&FM_Test[1],TDA7541_ADDR25_44,backup);//resume the 4th Bit of addr.25.
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR25_REG, &FM_Test[1]);
			//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET +10],TDA7541_ADDR10_REG_MUX,mux);
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR10_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET +10]);//write
			if(pTDA7541RadioInfo->radioInfo.eCurRadioMode != AM)		// exit seek, restore previos value
			{
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0], TDA7541_ADDR0_REG_SEEK, TDA7541_SEEK_OFF); //resume the 7th Bit of addr.0 under FM mode.
				TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR0_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0]);
			}
			else
			{
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+0], TDA7541_ADDR0_REG_SEEK, TDA7541_SEEK_OFF); //resume the 7th Bit of addr.0 under AM mode.
				TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR0_REG, &pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+0]);
			}
			returnRadioFreq(pTDA7541RadioInfo,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq);
			returnRadioScanCtrl(pTDA7541RadioInfo,0x03);//stop
		
			pTDA7541RadioInfo->bCurMute = !pTDA7541RadioInfo->bPreMute;
		}
		//设置波段
		if (pTDA7541RadioInfo->radioInfo.eCurRadioMode != pTDA7541RadioInfo->radioInfo.ePreRadioMode)
		{
			DBGD((TEXT("\r\nFlyAudio TDA7541RadioScanThread --------------change mode")));
			if (pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable)
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

			pTDA7541RadioInfo->radioInfo.eCurRadioMode = pTDA7541RadioInfo->radioInfo.ePreRadioMode;			
			if (FM1 == pTDA7541RadioInfo->radioInfo.eCurRadioMode)
			{
				pTDA7541RadioInfo->radioInfo.pPreRadioFreq = &pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM1;
				pTDA7541RadioInfo->radioInfo.pCurRadioFreq = &pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM1;
			}
			else if (FM2 == pTDA7541RadioInfo->radioInfo.eCurRadioMode)
			{
				pTDA7541RadioInfo->radioInfo.pPreRadioFreq = &pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM2;
				pTDA7541RadioInfo->radioInfo.pCurRadioFreq = &pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM2;
			}
			else if (AM == pTDA7541RadioInfo->radioInfo.eCurRadioMode)
			{
				pTDA7541RadioInfo->radioInfo.pPreRadioFreq = &pTDA7541RadioInfo->radioInfo.iPreRadioFreqAM;
				pTDA7541RadioInfo->radioInfo.pCurRadioFreq = &pTDA7541RadioInfo->radioInfo.iCurRadioFreqAM;
			}
			TDA7541Radio_ChangeToFMAM(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.eCurRadioMode);
			//returnRadioFreq(pTDA7541RadioInfo,*(pTDA7541RadioInfo->radioInfo.pCurRadioFreq));

			TDA7541RadioJumpNewFreqParaInit(pTDA7541RadioInfo);

			if (pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable)
			{
				if (AM == pTDA7541RadioInfo->radioInfo.eCurRadioMode)
				{
					Sleep(314);
				}
				IpcStartEvent(EVENT_GLOBAL_RADIO_MUTE_OUT_REQ_ID);//发送退出静音
			}
		}
		//设置频率
		if (*pTDA7541RadioInfo->radioInfo.pCurRadioFreq != *pTDA7541RadioInfo->radioInfo.pPreRadioFreq)
		{
			DBGD((TEXT("\r\nFlyAudio TDA7541RadioScanThread --------------change freq")));
			TDA7541_Radio_Mute(pTDA7541RadioInfo,TRUE);
			//if (pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable)
			//{
			//	IpcStartEvent(EVENT_GLOBAL_RADIO_MUTE_IN_REQ_ID);//发送进入静音
			//	while (!IpcWhatEvent(EVENT_GLOBAL_RADIO_MUTE_IN_OK_ID))//等待OK
			//	{
			//		Sleep(100);
			//	}
			//	IpcClearEvent(EVENT_GLOBAL_RADIO_MUTE_IN_OK_ID);//清除
			//}
			*pTDA7541RadioInfo->radioInfo.pCurRadioFreq = *pTDA7541RadioInfo->radioInfo.pPreRadioFreq;
			Set_Freq_7541(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.eCurRadioMode,*pTDA7541RadioInfo->radioInfo.pCurRadioFreq);
			//returnRadioFreq(pTDA7541RadioInfo,*(pTDA7541RadioInfo->radioInfo.pCurRadioFreq));
			TDA7541RadioJumpNewFreqParaInit(pTDA7541RadioInfo);
			if (pTDA7541RadioInfo->bPreMute == FALSE)
			{
				TDA7541_Radio_Mute(pTDA7541RadioInfo,FALSE);//取消静音
			}
			//if (pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable)
			//{
			//	IpcStartEvent(EVENT_GLOBAL_RADIO_MUTE_OUT_REQ_ID);//发送退出静音
			//}
		}
		if(pTDA7541RadioInfo->bPreMute != pTDA7541RadioInfo->bCurMute)//收音机静音开关
		{
			if (pTDA7541RadioInfo->bPreMute == TRUE)
			{
				TDA7541_Radio_Mute(pTDA7541RadioInfo,TRUE); // mute
			}
			else
			{
				TDA7541_Radio_Mute(pTDA7541RadioInfo,FALSE);// demute
			}
			pTDA7541RadioInfo->bCurMute = pTDA7541RadioInfo->bPreMute;
		}
		if (pTDA7541RadioInfo->radioInfo.bPreStepButtomDown)//按下，则持续跳台
		{
			if (0 == pTDA7541RadioInfo->radioInfo.iButtomStepCount)
			{
				pTDA7541RadioInfo->radioInfo.iButtomStepCount++;
				iThreadFreqStepWaitTime = 314;
			}
			else
			{
				buttomJumpFreqAndReturn(pTDA7541RadioInfo);
				pTDA7541RadioInfo->radioInfo.iButtomStepCount++;
				iThreadFreqStepWaitTime = 100;
			}
			continue;//跳到开头
		}
		nowTimer = GetTickCount();
		lastTimer = nowTimer;
		while(pTDA7541RadioInfo->radioInfo.bPreScaning)//搜索
		{
			nowTimer = GetTickCount();
			DBGD((TEXT("\r\nFlyAudio TDA7541RadioScanThread --------------scaning ....")));
			//FMTHVal=GetSSTOPPinLevel(TDA7541_GetTunerParameter(pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET +10],TDA7541_ADDR10_REG_SSTH));//get seek aligned value.
			//mux=TDA7541_GetTunerParameter(pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET +10],TDA7541_ADDR10_REG_MUX);	//backup
			//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET +10],TDA7541_ADDR10_REG_MUX,2);	//set
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR10_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET +10]);//write
			//backup=TDA7541_GetTunerParameter(FM_Test[1],TDA7541_ADDR25_44);//Backup the 4th Bit of addr.25.
			//TDA7541_UpdateInitTblBit(&FM_Test[1],TDA7541_ADDR25_44,1);//Force set 1 into its 4th Bit when seeking.
			TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR25_REG, &FM_Test[1]);//write	

			TDA7541_Radio_Mute(pTDA7541RadioInfo,TRUE); // mute			
			if(AM != pTDA7541RadioInfo->radioInfo.eCurRadioMode)
			{				

				//seek =TDA7541_GetTunerParameter(pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0],TDA7541_ADDR0_REG_SEEK);//Backup the 7th Bit of addr.0.
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0], TDA7541_ADDR0_REG_SEEK, TDA7541_SEEK_ON); //Force set 1 into its 7th Bit when seeking.
				TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR0_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0]);
				//temp = pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+16];
				//temp &= 0x3f;
				//TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR16_REG, &temp);
				m_RF_Freq = (*pTDA7541RadioInfo->radioInfo.pPreRadioFreq)*10;
			}
			else
			{
				//seek =TDA7541_GetTunerParameter(pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+0],TDA7541_ADDR0_REG_SEEK);//Backup the 7th Bit of addr.0.
				TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+0], TDA7541_ADDR0_REG_SEEK, TDA7541_SEEK_ON); //Force set 1 into its 7th Bit when seeking.
				TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR0_REG, &pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+0]);
				//temp = pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+16];
				//temp &= 0x3f;
				//TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR16_REG, &temp);
				m_RF_Freq = *pTDA7541RadioInfo->radioInfo.pPreRadioFreq;
			}
			
			if(m_RF_Freq>=87500)//EU/WB
			{
				TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR10_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+10]);//load aligned data of SSTH from eeprom to make sure that seek stop correctly
			}
			else if(m_RF_Freq>=64000&&m_RF_Freq<=74000)//OIRT
			{
				//TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR10_REG, &pTDA7541RadioInfo->mParameterTable[OIRT_SD_Offset]);
			}
			else if(m_RF_Freq<=19244)//MW LW SW
			{
				TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR10_REG, &pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+10]);//load aligned data of SSTH from eeprom to make sure that seek stop correctly
			}
			//
			{
				if (pTDA7541RadioInfo->radioInfo.bCurScaning != pTDA7541RadioInfo->radioInfo.bPreScaning)//起始搜索频率
				{
					pTDA7541RadioInfo->radioInfo.bCurScaning = pTDA7541RadioInfo->radioInfo.bPreScaning;
					iScanStartFreq = GetCorrectScanStartFreq(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.pPreRadioFreq);
					bHaveSearched = FALSE;
					iHaveSearchedLevel = 0;
				}
				*pTDA7541RadioInfo->radioInfo.pPreRadioFreq = RadioStepFreqGenerate(pTDA7541RadioInfo
					,pTDA7541RadioInfo->radioInfo.eCurRadioMode
					,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq
					,pTDA7541RadioInfo->radioInfo.eScanDirection
					,STEP_SCAN);

				*pTDA7541RadioInfo->radioInfo.pCurRadioFreq = *pTDA7541RadioInfo->radioInfo.pPreRadioFreq;
				Set_Freq_7541(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.eCurRadioMode,*pTDA7541RadioInfo->radioInfo.pCurRadioFreq);
				
				
				DBGD((TEXT("\r\nFlyAudio TDA7541RadioScanThread iScanStartFreq = %d,PreRadioFreq = %d"),iScanStartFreq,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq));
				if (iScanStartFreq == *pTDA7541RadioInfo->radioInfo.pPreRadioFreq )//一圈都没好台
				{
					pTDA7541RadioInfo->radioInfo.bPreScaning = FALSE;
					pTDA7541RadioInfo->radioInfo.bCurScaning = TRUE;
					returnRadioFreq(pTDA7541RadioInfo,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq);
					returnRadioScanCtrl(pTDA7541RadioInfo,0x03);//stop
					if (pTDA7541RadioInfo->bPreMute == FALSE)//收到台要出声音
					{
						TDA7541_Radio_Mute(pTDA7541RadioInfo,FALSE);
					}
				}
				if ((nowTimer - lastTimer) >  157)//定时返回频点
				{
					lastTimer = nowTimer;
					if (pTDA7541RadioInfo->radioInfo.bPreScaning)
					{
						returnRadioFreq(pTDA7541RadioInfo,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq);
					}
				}
				if (AM != pTDA7541RadioInfo->radioInfo.eCurRadioMode)
				{
					Sleep(10);
				} 
				else
				{
					Sleep(138);
				}
				
				if (bRadioSignalGood(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.eCurRadioMode,&iTempHaveSearchedLevel))
				{
				
					DBGD((TEXT("\r\niHaveSearchedLevel:%d,iTempHaveSearchedLevel:%d\r\n"),iHaveSearchedLevel,iTempHaveSearchedLevel));
					bHaveSearched = TRUE;
					if (iHaveSearchedLevel > iTempHaveSearchedLevel)//OK
					{
						if (STEP_BACKWARD == pTDA7541RadioInfo->radioInfo.eScanDirection)
						{
							*pTDA7541RadioInfo->radioInfo.pPreRadioFreq = RadioStepFreqGenerate(pTDA7541RadioInfo
								,pTDA7541RadioInfo->radioInfo.eCurRadioMode
								,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq
								,STEP_FORWARD
								,STEP_SCAN);
						} 
						else
						{
							*pTDA7541RadioInfo->radioInfo.pPreRadioFreq = RadioStepFreqGenerate(pTDA7541RadioInfo
								,pTDA7541RadioInfo->radioInfo.eCurRadioMode
								,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq
								,STEP_BACKWARD
								,STEP_SCAN);
						}
						*pTDA7541RadioInfo->radioInfo.pCurRadioFreq = *pTDA7541RadioInfo->radioInfo.pPreRadioFreq;
						Set_Freq_7541(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.eCurRadioMode,*pTDA7541RadioInfo->radioInfo.pCurRadioFreq);
						
						
						if (pTDA7541RadioInfo->bPreMute == FALSE)//收到台要出声音
						{
							TDA7541_Radio_Mute(pTDA7541RadioInfo,FALSE);
						}
						//TDA7541_UpdateInitTblBit(&FM_Test[1],TDA7541_ADDR25_44,backup);//resume the 4th Bit of addr.25.
						TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR25_REG, &FM_Test[1]);
						//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET +10],TDA7541_ADDR10_REG_MUX,mux);
						TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR10_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET +10]);//write
						if(pTDA7541RadioInfo->radioInfo.eCurRadioMode != AM)		// exit seek, restore previos value
						{
							TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0], TDA7541_ADDR0_REG_SEEK, TDA7541_SEEK_OFF); //resume the 7th Bit of addr.0 under FM mode.
							TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR0_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0]);
						}
						else
						{
							TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+0], TDA7541_ADDR0_REG_SEEK, TDA7541_SEEK_OFF); //resume the 7th Bit of addr.0 under AM mode.
							TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR0_REG, &pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+0]);
						}

						returnRadioFreq(pTDA7541RadioInfo,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq);
						returnRadioHaveSearched(pTDA7541RadioInfo,TRUE);


						if (pTDA7541RadioInfo->radioInfo.bScanRepeatFlag == FALSE)
						{
							pTDA7541RadioInfo->radioInfo.bPreScaning = FALSE;
							pTDA7541RadioInfo->radioInfo.bCurScaning = FALSE;
							returnRadioScanCtrl(pTDA7541RadioInfo,0x03);//stop							
							break;
						}
						else
						{
							//blinking 5 times
							iBlinkingTimes = 0;
							bBlinkingStatus = TRUE;
							while (pTDA7541RadioInfo->radioInfo.bPreScaning == TRUE && iBlinkingTimes < 10)
							{
								Sleep(500);
								iBlinkingTimes++;
								bBlinkingStatus = !bBlinkingStatus;
								returnRadioBlinkingStatus(pTDA7541RadioInfo,bBlinkingStatus);							
							}
							returnRadioBlinkingStatus(pTDA7541RadioInfo,TRUE);
							Sleep(500);
							if (pTDA7541RadioInfo->radioInfo.bPreScaning == TRUE)
							{
								returnRadioScanCtrl(pTDA7541RadioInfo,0x04);//repeat
							}

							iScanStartFreq = *pTDA7541RadioInfo->radioInfo.pPreRadioFreq;
							bHaveSearched = FALSE;
							iHaveSearchedLevel = 0;
						}
					}				
				}
				else
				{
					if (bHaveSearched)
					{
						if (STEP_BACKWARD == pTDA7541RadioInfo->radioInfo.eScanDirection)
						{
							*pTDA7541RadioInfo->radioInfo.pPreRadioFreq = RadioStepFreqGenerate(pTDA7541RadioInfo
								,pTDA7541RadioInfo->radioInfo.eCurRadioMode
								,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq
								,STEP_FORWARD
								,STEP_SCAN);
						} 
						else
						{
							*pTDA7541RadioInfo->radioInfo.pPreRadioFreq = RadioStepFreqGenerate(pTDA7541RadioInfo
								,pTDA7541RadioInfo->radioInfo.eCurRadioMode
								,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq
								,STEP_BACKWARD
								,STEP_SCAN);
						}
						*pTDA7541RadioInfo->radioInfo.pCurRadioFreq = *pTDA7541RadioInfo->radioInfo.pPreRadioFreq;
						Set_Freq_7541(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.eCurRadioMode,*pTDA7541RadioInfo->radioInfo.pCurRadioFreq);
						
						if (pTDA7541RadioInfo->bPreMute == FALSE)//收到台要出声音
						{
							TDA7541_Radio_Mute(pTDA7541RadioInfo,FALSE);
						}
						//TDA7541_UpdateInitTblBit(&FM_Test[1],TDA7541_ADDR25_44,backup);//resume the 4th Bit of addr.25.
						TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR25_REG, &FM_Test[1]);
						//TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET +10],TDA7541_ADDR10_REG_MUX,mux);
						TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR10_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET +10]);//write
						if(pTDA7541RadioInfo->radioInfo.eCurRadioMode != AM)		// exit seek, restore previos value
						{
							TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0], TDA7541_ADDR0_REG_SEEK, TDA7541_SEEK_OFF); //resume the 7th Bit of addr.0 under FM mode.
							TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR0_REG, &pTDA7541RadioInfo->mParameterTable[FM_REG_OFFSET+0]);
						}
						else
						{
							TDA7541_UpdateInitTblBit(&pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+0], TDA7541_ADDR0_REG_SEEK, TDA7541_SEEK_OFF); //resume the 7th Bit of addr.0 under AM mode.
							TDA7541_WriteTunerRegister(pTDA7541RadioInfo,TDA7541_ADDR0_REG, &pTDA7541RadioInfo->mParameterTable[AM_REG_OFFSET+0]);
						}

						returnRadioFreq(pTDA7541RadioInfo,*pTDA7541RadioInfo->radioInfo.pPreRadioFreq);
						returnRadioHaveSearched(pTDA7541RadioInfo,TRUE);

						if (pTDA7541RadioInfo->radioInfo.bScanRepeatFlag == FALSE)
						{
							pTDA7541RadioInfo->radioInfo.bPreScaning = FALSE;
							pTDA7541RadioInfo->radioInfo.bCurScaning = FALSE;
							returnRadioScanCtrl(pTDA7541RadioInfo,0x03);//stop							
							break;
						}
						else
						{
							//blinking 5 times
							iBlinkingTimes = 0;
							bBlinkingStatus = TRUE;
							while (pTDA7541RadioInfo->radioInfo.bPreScaning == TRUE && iBlinkingTimes < 10)
							{
								Sleep(500);
								iBlinkingTimes++;
								bBlinkingStatus = !bBlinkingStatus;
								returnRadioBlinkingStatus(pTDA7541RadioInfo,bBlinkingStatus);						
							}
							returnRadioBlinkingStatus(pTDA7541RadioInfo,TRUE);
							Sleep(500);
							if (pTDA7541RadioInfo->radioInfo.bPreScaning == TRUE)
							{
								returnRadioScanCtrl(pTDA7541RadioInfo,0x04);//repeat
							}
							
							iScanStartFreq = *pTDA7541RadioInfo->radioInfo.pPreRadioFreq;
							bHaveSearched = FALSE;
							iHaveSearchedLevel = 0;
						}
					}
				}
				iHaveSearchedLevel = iTempHaveSearchedLevel;
			}
		}	
		DBGD((TEXT("\r\nFlyAudio TDA7541RadioScanThread Running End")));
	}
	DBGD((TEXT("\r\nFlyAudio TDA7541RadioScanThread exit!")));
	pTDA7541RadioInfo->TDA7541RadioScanThreadHandle = NULL;
	return 0;
}
static DWORD WINAPI TDA7541RadioMainThread(LPVOID pContext)
{
	ULONG WaitReturn;
	static UINT RadioScanStatus;

	P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo = (P_FLY_TDA7541_RADIO_INFO)pContext;
	//DBGD((TEXT("\r\nFlyAudio TDA7541RadioMainThread")));

	while (!pTDA7541RadioInfo->bKillDispatchMainThread)
	{
		WaitReturn = WaitForSingleObject(pTDA7541RadioInfo->hDispatchMainThreadEvent, INFINITE);
		DBGD((TEXT("\r\nFlyAudio TDA7541RadioMainThread Running!")));

		if (IpcWhatEvent(EVENT_GLOBAL_REG_RESTORE_RADIO_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_REG_RESTORE_RADIO_ID);
			if (pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditRadio)
			{
				pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedRestoreRegeditRadio = FALSE;
				if (pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRestoreRegeditToFactory)
				{
					radioIDChangePara(pTDA7541RadioInfo,0);//pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser);
					pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM1 = pTDA7541RadioInfo->radioInfo.iFreqFMMin;
					pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM2 = pTDA7541RadioInfo->radioInfo.iFreqFMMin;
					pTDA7541RadioInfo->radioInfo.iPreRadioFreqAM = pTDA7541RadioInfo->radioInfo.iFreqAMMin;
					pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM1 = pTDA7541RadioInfo->radioInfo.iFreqFMMin;
					pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM2 = pTDA7541RadioInfo->radioInfo.iFreqFMMin;
					pTDA7541RadioInfo->radioInfo.iCurRadioFreqAM = pTDA7541RadioInfo->radioInfo.iFreqAMMin;
				}
				RegDataWriteRadio(pTDA7541RadioInfo);
			}
		}

		if (FALSE == pTDA7541RadioInfo->bPowerUp)
		{
		}
		else
		{
			if (IpcWhatEvent(EVENT_GLOBAL_RADIO_CHANGE_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_RADIO_CHANGE_ID);
				DBGD((TEXT("\r\nFlyAudio TDA7541 Radio ID Change!")));
				radioIDChangePara(pTDA7541RadioInfo
					,pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser);
			}

			if (IpcWhatEvent(EVENT_GLOBAL_BATTERY_RECOVERY_RADIO_ID))
			{
				DBGD((TEXT("\r\nFlyAudio TDA7541 Voltage After Low Proc")));
				IpcClearEvent(EVENT_GLOBAL_BATTERY_RECOVERY_RADIO_ID);
				recoveryRadioRegData(pTDA7541RadioInfo);
			}

			if (IpcWhatEvent(EVENT_GLOBAL_RADIO_ANTENNA_ID))
			{
				DBGD((TEXT("\r\nFlyAudio TDA7541 ANTENNA ctrl")));
				IpcClearEvent(EVENT_GLOBAL_RADIO_ANTENNA_ID);
				if (pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eAudioInput == RADIO)
				{
					radioANTControl(pTDA7541RadioInfo,TRUE);
				}
				else
				{
					radioANTControl(pTDA7541RadioInfo,FALSE);
				}
			}
			SetEvent(pTDA7541RadioInfo->hDispatchScanThreadEvent);
		}
	}
	DBGD((TEXT("\r\nFlyAudio TDA7541RadioMainThread Prepare exit!")));

	pTDA7541RadioInfo->bPowerUp = FALSE;
	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNeedProcVoltageShakeRadio = 0;

	while (pTDA7541RadioInfo->TDA7541RadioScanThreadHandle)
	{
		SetEvent(pTDA7541RadioInfo->hDispatchScanThreadEvent);
		Sleep(10);
	}
	CloseHandle(pTDA7541RadioInfo->hDispatchScanThreadEvent);

	if(pTDA7541RadioInfo->TDA7541RadioI2CHandle)
	{
		//I2CClose(pTDA7541RadioInfo->TDA7541RadioI2CHandle);
		pTDA7541RadioInfo->TDA7541RadioI2CHandle = NULL;
	}

	pTDA7541RadioInfo->TDA7541RadioMainThreadHandle = NULL;

	DBGD((TEXT("\r\nFlyAudio TDA7541RadioMainThread exit!")));
	return 0;
}

VOID TDA7541RadioEnable(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL bOn)
{
	DWORD dwThreadID;

	if(bOn)
	{
		//pTDA7541RadioInfo->TDA7541RadioI2CHandle = I2COpen(TEXT("I2C1:"));
		//if(pTDA7541RadioInfo->TDA7541RadioI2CHandle == NULL)
		//{
		//	DBGI((TEXT("\r\nTDA7541Radio: Error open I2C1!")));
		//	return;
		//}

		pTDA7541RadioInfo->bKillDispatchMainThread = FALSE;
		pTDA7541RadioInfo->hDispatchMainThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		pTDA7541RadioInfo->TDA7541RadioMainThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)TDA7541RadioMainThread, //线程的全局函数
			pTDA7541RadioInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		if (NULL == pTDA7541RadioInfo->TDA7541RadioMainThreadHandle)
		{
			pTDA7541RadioInfo->bKillDispatchMainThread = TRUE;
			return;
		}
		DBGI((TEXT("\r\nFlyAudio TDA7541RadioMainThread ID %x!"),dwThreadID));

		pTDA7541RadioInfo->bKillDispatchScanThread = FALSE;
		pTDA7541RadioInfo->hDispatchScanThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		pTDA7541RadioInfo->TDA7541RadioScanThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)TDA7541RadioScanThread, //线程的全局函数
			pTDA7541RadioInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		if (NULL == pTDA7541RadioInfo->TDA7541RadioScanThreadHandle)
		{
			pTDA7541RadioInfo->bKillDispatchScanThread = TRUE;
			return;
		}
		DBGI((TEXT("\r\nFlyAudio TDA7541RadioScanThread ID %x!"),dwThreadID));

	}
	else
	{
		pTDA7541RadioInfo->bKillDispatchScanThread = TRUE;
		SetEvent(pTDA7541RadioInfo->hDispatchScanThreadEvent);		
		pTDA7541RadioInfo->bKillDispatchMainThread = TRUE;
		SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);

		while (pTDA7541RadioInfo->TDA7541RadioMainThreadHandle)
		{
			SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
			Sleep(10);
		}
		CloseHandle(pTDA7541RadioInfo->hDispatchMainThreadEvent);
	}
}

static void radioIDChangePara(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE iID)
{

	if (0x00 == iID)//China
	{
		pTDA7541RadioInfo->radioInfo.iFreqFMMin = 8750;		
		pTDA7541RadioInfo->radioInfo.iFreqFMMax = 10800;
		pTDA7541RadioInfo->radioInfo.iFreqFMManualStep = 5;
		pTDA7541RadioInfo->radioInfo.iFreqFMScanStep = 10;

		pTDA7541RadioInfo->radioInfo.iFreqAMMin = 522;
		pTDA7541RadioInfo->radioInfo.iFreqAMMax = 1620;
		pTDA7541RadioInfo->radioInfo.iFreqAMManualStep = 9;
		pTDA7541RadioInfo->radioInfo.iFreqAMScanStep = 9;

		pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM1 = 8750;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM1 = 8750;
		pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM2 = 8750;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM2 = 8750;
		pTDA7541RadioInfo->radioInfo.iPreRadioFreqAM = 522;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqAM = 522;
	}
	else if (0x01 == iID)//USA
	{
		pTDA7541RadioInfo->radioInfo.iFreqFMMin = 8750;
		pTDA7541RadioInfo->radioInfo.iFreqFMMax = 10790;
		pTDA7541RadioInfo->radioInfo.iFreqFMManualStep = 5;
		pTDA7541RadioInfo->radioInfo.iFreqFMScanStep = 20;

		pTDA7541RadioInfo->radioInfo.iFreqAMMin = 530;
		pTDA7541RadioInfo->radioInfo.iFreqAMMax = 1720;
		pTDA7541RadioInfo->radioInfo.iFreqAMManualStep = 10;
		pTDA7541RadioInfo->radioInfo.iFreqAMScanStep = 10;

		pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM1 = 8750;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM1 = 8750;
		pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM2 = 8750;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM2 = 8750;
		pTDA7541RadioInfo->radioInfo.iPreRadioFreqAM = 530;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqAM = 530;
	}
	else if (0x02 == iID)//S.Amer-1
	{
		pTDA7541RadioInfo->radioInfo.iFreqFMMin = 8750;
		pTDA7541RadioInfo->radioInfo.iFreqFMMax = 10800;
		pTDA7541RadioInfo->radioInfo.iFreqFMManualStep = 10;
		pTDA7541RadioInfo->radioInfo.iFreqFMScanStep = 10;

		pTDA7541RadioInfo->radioInfo.iFreqAMMin = 530;
		pTDA7541RadioInfo->radioInfo.iFreqAMMax = 1710;
		pTDA7541RadioInfo->radioInfo.iFreqAMManualStep = 10;
		pTDA7541RadioInfo->radioInfo.iFreqAMScanStep = 10;

		pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM1 = 8750;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM1 = 8750;
		pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM2 = 8750;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM2 = 8750;
		pTDA7541RadioInfo->radioInfo.iPreRadioFreqAM = 530;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqAM = 530;
	}
	else if (0x03 == iID)//S.Amer-2
	{
		pTDA7541RadioInfo->radioInfo.iFreqFMMin = 8750;
		pTDA7541RadioInfo->radioInfo.iFreqFMMax = 10800;
		pTDA7541RadioInfo->radioInfo.iFreqFMManualStep = 10;
		pTDA7541RadioInfo->radioInfo.iFreqFMScanStep = 10;

		pTDA7541RadioInfo->radioInfo.iFreqAMMin = 520;
		pTDA7541RadioInfo->radioInfo.iFreqAMMax = 1600;
		pTDA7541RadioInfo->radioInfo.iFreqAMManualStep = 5;
		pTDA7541RadioInfo->radioInfo.iFreqAMScanStep = 5;

		pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM1 = 8750;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM1 = 8750;
		pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM2 = 8750;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM2 = 8750;
		pTDA7541RadioInfo->radioInfo.iPreRadioFreqAM = 520;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqAM = 520;
	}
	else if (0x04 == iID)//KOREA
	{
		pTDA7541RadioInfo->radioInfo.iFreqFMMin = 8810;
		pTDA7541RadioInfo->radioInfo.iFreqFMMax = 10790;
		pTDA7541RadioInfo->radioInfo.iFreqFMManualStep = 5;
		pTDA7541RadioInfo->radioInfo.iFreqFMScanStep = 20;

		pTDA7541RadioInfo->radioInfo.iFreqAMMin = 531;
		pTDA7541RadioInfo->radioInfo.iFreqAMMax = 1620;
		pTDA7541RadioInfo->radioInfo.iFreqAMManualStep = 9;
		pTDA7541RadioInfo->radioInfo.iFreqAMScanStep = 9;

		pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM1 = 8810;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM1 = 8810;
		pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM2 = 8810;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM2 = 8810;
		pTDA7541RadioInfo->radioInfo.iPreRadioFreqAM = 531;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqAM = 531;
	}
	else if (0x05 == iID)//Thailand
	{
		pTDA7541RadioInfo->radioInfo.iFreqFMMin = 8750;
		pTDA7541RadioInfo->radioInfo.iFreqFMMax = 10800;
		pTDA7541RadioInfo->radioInfo.iFreqFMManualStep = 5;
		pTDA7541RadioInfo->radioInfo.iFreqFMScanStep = 25;

		pTDA7541RadioInfo->radioInfo.iFreqAMMin = 531;
		pTDA7541RadioInfo->radioInfo.iFreqAMMax = 1602;
		pTDA7541RadioInfo->radioInfo.iFreqAMManualStep = 9;
		pTDA7541RadioInfo->radioInfo.iFreqAMScanStep = 9;

		pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM1 = 8750;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM1 = 8750;
		pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM2 = 8750;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM2 = 8750;
		pTDA7541RadioInfo->radioInfo.iPreRadioFreqAM = 531;
		//pTDA7541RadioInfo->radioInfo.iCurRadioFreqAM = 531;
	}
	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iFreqFMMin=pTDA7541RadioInfo->radioInfo.iFreqFMMin;
	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iFreqFMMax=pTDA7541RadioInfo->radioInfo.iFreqFMMax;	
	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iFreqAMMin=pTDA7541RadioInfo->radioInfo.iFreqAMMin;
	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iFreqAMMax=pTDA7541RadioInfo->radioInfo.iFreqAMMax;

	if (AM == pTDA7541RadioInfo->radioInfo.ePreRadioMode)
	{
		returnRadioFreq(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.iPreRadioFreqAM);
	}
	else if (FM1 == pTDA7541RadioInfo->radioInfo.ePreRadioMode)
	{
		returnRadioFreq(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM1);
	}
	else if (FM2 == pTDA7541RadioInfo->radioInfo.ePreRadioMode)
	{
		returnRadioFreq(pTDA7541RadioInfo,pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM2);
	}

	returnRadioMaxAndMinPara(pTDA7541RadioInfo);
}

static void powerOnNormalInit(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	pTDA7541RadioInfo->bPowerUp = FALSE;

	pTDA7541RadioInfo->buffToUserHx = 0;
	pTDA7541RadioInfo->buffToUserLx = 0;

	pTDA7541RadioInfo->bPreMute = FALSE;
	pTDA7541RadioInfo->bCurMute = TRUE;

	radioIDChangePara(pTDA7541RadioInfo,pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioIDUser);

	pTDA7541RadioInfo->radioInfo.ePreRadioMode = FM1;
	pTDA7541RadioInfo->radioInfo.eCurRadioMode = FM1;

	pTDA7541RadioInfo->radioInfo.pPreRadioFreq = &pTDA7541RadioInfo->radioInfo.iPreRadioFreqFM1;
	pTDA7541RadioInfo->radioInfo.pCurRadioFreq = &pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM1;

	pTDA7541RadioInfo->radioInfo.bPreScaning = FALSE;
	pTDA7541RadioInfo->radioInfo.bCurScaning = FALSE;
	pTDA7541RadioInfo->radioInfo.eScanDirection = STEP_FORWARD;	
	pTDA7541RadioInfo->radioInfo.bScanRepeatFlag = FALSE;

	pTDA7541RadioInfo->radioInfo.bPreStepButtomDown = FALSE;
	//pTDA7541RadioInfo->radioInfo.bCurStepButtomDown = FALSE;
	pTDA7541RadioInfo->radioInfo.eButtomStepDirection = STEP_FORWARD;
	pTDA7541RadioInfo->radioInfo.iButtomStepCount = 0;

	pTDA7541RadioInfo->RDSInfo.RadioRDSAFControlOn = FALSE;
	pTDA7541RadioInfo->RDSInfo.RadioRDSTAControlOn = FALSE;

	memcpy(pTDA7541RadioInfo->mParameterTable,DefaultData,INIT_DATA_SIZE);

	//IO_Control_Init(pTDA7541RadioInfo);
	DBGD((TEXT("\r\nFlyAudio TDA7541 (TDA7541RadioParaInit)%d____%d_____%d____________%d"),(pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM1),(pTDA7541RadioInfo->radioInfo.iCurRadioFreqFM2),(pTDA7541RadioInfo->radioInfo.iCurRadioFreqAM),*pTDA7541RadioInfo->radioInfo.pCurRadioFreq));
}

static void powerOnFirstInit(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	pTDA7541RadioInfo->bOpen = FALSE;
	pTDA7541RadioInfo->bPowerSpecialUp = FALSE;

	pTDA7541RadioInfo->bKillDispatchMainThread = TRUE;
	pTDA7541RadioInfo->TDA7541RadioMainThreadHandle = NULL;
	pTDA7541RadioInfo->hDispatchMainThreadEvent = NULL;

	pTDA7541RadioInfo->bKillDispatchScanThread = TRUE;
	pTDA7541RadioInfo->TDA7541RadioScanThreadHandle = NULL;
	pTDA7541RadioInfo->hDispatchScanThreadEvent = NULL;

	pTDA7541RadioInfo->bKillDispatchRDSRecThread = TRUE;
	pTDA7541RadioInfo->TDA7541RadioRDSRecThreadHandle = NULL;

	pTDA7541RadioInfo->TDA7541RadioI2CHandle = NULL;

	pTDA7541RadioInfo->hInterruptEventSSTOP = NULL;
	pTDA7541RadioInfo->bInterruptEventSSTOPRunning = FALSE;

}

static void powerOnSpecialEnable(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BOOL bOn)
{
	DWORD dwThreadID;

	if (bOn)
	{
		if (pTDA7541RadioInfo->bPowerSpecialUp)
		{
			return;
		}
		pTDA7541RadioInfo->bPowerSpecialUp = TRUE;

#if RADIO_RDS
		MSGQUEUEOPTIONS  msgOpts;

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_RADIO_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_RADIO_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pTDA7541RadioInfo->hMsgQueueToRadioCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_RADIO_NAME, &msgOpts);
		if (NULL == pTDA7541RadioInfo->hMsgQueueToRadioCreate)
		{
			DBGI((TEXT("\r\nFlyAudio 7541 Create MsgQueue To Radio Fail!")));
		}

		pTDA7541RadioInfo->bKillDispatchRDSRecThread = FALSE;
		pTDA7541RadioInfo->TDA7541RadioRDSRecThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)TDA7541RadioRDSRecThread, //线程的全局函数
			pTDA7541RadioInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		if (NULL == pTDA7541RadioInfo->TDA7541RadioRDSRecThreadHandle)
		{
			pTDA7541RadioInfo->bKillDispatchRDSRecThread = TRUE;
			return;
		}
		DBGI((TEXT("\r\nFlyAudio TDA7541RadioRDSRecThread ID %x!"),dwThreadID));
#endif

		pTDA7541RadioInfo->hBuffToUserDataEvent = CreateEvent(NULL,FALSE,FALSE,NULL);	
		InitializeCriticalSection(&pTDA7541RadioInfo->hCSSendToUser);
	} 
	else
	{
		if (!pTDA7541RadioInfo->bPowerSpecialUp)
		{
			return;
		}
		pTDA7541RadioInfo->bPowerSpecialUp = FALSE;
#if RADIO_RDS
		pTDA7541RadioInfo->bKillDispatchRDSRecThread = TRUE;
		while (pTDA7541RadioInfo->TDA7541RadioRDSRecThreadHandle)
		{
			SetEvent(pTDA7541RadioInfo->hMsgQueueToRadioCreate);
			Sleep(10);
		}
		CloseMsgQueue(pTDA7541RadioInfo->hMsgQueueToRadioCreate);
#endif

		CloseHandle(pTDA7541RadioInfo->hBuffToUserDataEvent);
	}
}

void IpcRecv(UINT32 iEvent)
{
	P_FLY_TDA7541_RADIO_INFO	pTDA7541RadioInfo = (P_FLY_TDA7541_RADIO_INFO)gpTDA7541RadioInfo;

	DBGD((TEXT("\r\nFLYRadio Recv IPC iEvent:%d\r\n"),iEvent));

	SetEvent(pTDA7541RadioInfo->hDispatchMainThreadEvent);
}
//void SockRecv(BYTE *buf, UINT16 len)
//{
//	P_FLY_TDA7541_RADIO_INFO	pTDA7541RadioInfo = (P_FLY_TDA7541_RADIO_INFO)gpTDA7541RadioInfo;
//}

HANDLE FRA_Init(DWORD dwContext)
{
	P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo;

	RETAILMSG(1, (TEXT("\r\n7541 Init Start")));


	pTDA7541RadioInfo = (P_FLY_TDA7541_RADIO_INFO)LocalAlloc(LPTR, sizeof(FLY_TDA7541_RADIO_INFO));
	if (!pTDA7541RadioInfo)
	{
		return NULL;
	}

	gpTDA7541RadioInfo = pTDA7541RadioInfo;

	pTDA7541RadioInfo->pFlyShmGlobalInfo = CreateShm(RADIO_MODULE,IpcRecv);
	if (NULL == pTDA7541RadioInfo->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio RADIO Init create shm err\r\n")));
		return NULL;
	}

	if (!GetDllAddrTable())
	{
		DBGE((TEXT("FlyAudio RADIO  GetDllAddrTable err\r\n")));
		return NULL;
	}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio RADIO Open create user buf err\r\n")));
		return NULL;
	}
	//if(!CreateServerSocket(SockRecv, TCP_PORT_RADIO))
	//{
	//	DBGE((TEXT("FlyAudio RADIO Open create server socket err\r\n")));
	//	return NULL;
	//}


	////参数初始化
	powerOnNormalInit(pTDA7541RadioInfo);
	powerOnFirstInit(pTDA7541RadioInfo);
	powerOnSpecialEnable(pTDA7541RadioInfo,TRUE);

	if (pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalStructSize
		!= sizeof(struct shm))
	{
		pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[0] = 'F';
		pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[1] = 'R';
		pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[2] = 'A';
	}

	//pTDA7541RadioInfo->hHandleGlobalGlobalEvent = CreateEvent(NULL,FALSE,FALSE,DATA_GLOBAL_HANDLE_GLOBAL);
	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.bInit = TRUE;
	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.iDriverCompYear = year;
	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.iDriverCompMon = months;
	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.iDriverCompDay = day;
	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.iDriverCompHour = hours;
	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.iDriverCompMin = minutes;
	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.iDriverCompSec = seconds;

	DBGI((TEXT("\r\nFlyAudio TDA7541Radio Init Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return (HANDLE)pTDA7541RadioInfo;
}

BOOL FRA_Deinit(DWORD hDeviceContext)
{
	P_FLY_TDA7541_RADIO_INFO	pTDA7541RadioInfo = (P_FLY_TDA7541_RADIO_INFO)hDeviceContext;
	DBGI((TEXT("\r\nFlyAudio TDA7541Radio DeInit")));

	powerOnSpecialEnable(pTDA7541RadioInfo,FALSE);

	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.bInit = FALSE;
	SetEvent(pTDA7541RadioInfo->hHandleGlobalGlobalEvent);
	CloseHandle(pTDA7541RadioInfo->hHandleGlobalGlobalEvent);

	FreeShm();
	//FreeSocketServer();
	FreeUserBuff();
	LocalFree(pTDA7541RadioInfo);
	return TRUE;
}

DWORD FRA_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	P_FLY_TDA7541_RADIO_INFO	pTDA7541RadioInfo = (P_FLY_TDA7541_RADIO_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;

	DBGI((TEXT("\r\nFlyAudio TDA7541Radio Open")));

	if(pTDA7541RadioInfo->bOpen)
	{
		return NULL;
	}
	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.bOpen = TRUE;
	SetEvent(pTDA7541RadioInfo->hHandleGlobalGlobalEvent);

	powerOnNormalInit(pTDA7541RadioInfo);	
	pTDA7541RadioInfo->bOpen = TRUE;

	TDA7541RadioEnable(pTDA7541RadioInfo,TRUE);

	return returnWhat;
}

BOOL FRA_Close(DWORD hDeviceContext)
{
	P_FLY_TDA7541_RADIO_INFO	pTDA7541RadioInfo = (P_FLY_TDA7541_RADIO_INFO)hDeviceContext;

	TDA7541RadioEnable(pTDA7541RadioInfo,FALSE);

	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.bOpen = FALSE;
	SetEvent(pTDA7541RadioInfo->hHandleGlobalGlobalEvent);

	pTDA7541RadioInfo->bOpen = FALSE;
	SetEvent(pTDA7541RadioInfo->hBuffToUserDataEvent);

	DBGI((TEXT("\r\nFlyAudio TDA7541 Close")));
	return TRUE;
}

VOID FRA_PowerUp(DWORD hDeviceContext)
{
	P_FLY_TDA7541_RADIO_INFO	pTDA7541RadioInfo = (P_FLY_TDA7541_RADIO_INFO)hDeviceContext;

	DBGI((TEXT("\r\nFlyAudio TDA7541 PowerUp")));
}

VOID FRA_PowerDown(DWORD hDeviceContext)
{
	P_FLY_TDA7541_RADIO_INFO	pTDA7541RadioInfo = (P_FLY_TDA7541_RADIO_INFO)hDeviceContext;

	pTDA7541RadioInfo->bPowerUp = FALSE;

	TDA7541RadioJumpNewFreqParaInit(pTDA7541RadioInfo);

	pTDA7541RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNeedProcVoltageShakeRadio = 0;

	DBGI((TEXT("\r\nFlyAudio TDA7541 PowerDown")));
}

DWORD FRA_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	P_FLY_TDA7541_RADIO_INFO	pTDA7541RadioInfo = (P_FLY_TDA7541_RADIO_INFO)hOpenContext;
	UINT dwRead = 0,i;
	BYTE *buf = (BYTE *)pBuffer;
	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio RADIO return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}

	return dwRead;
}

DWORD FRA_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	P_FLY_TDA7541_RADIO_INFO	pTDA7541RadioInfo = (P_FLY_TDA7541_RADIO_INFO)hOpenContext;
	BYTE *p;
	p = (BYTE *)pSourceBytes;
	DBGD((TEXT("\r\nFlyAudio TDA7541Radio Write %d "),NumberOfBytes));
	//RETAILMSG(1, (TEXT("\r\nFlyAudio TDA7541Radio Write %d ---->"),NumberOfBytes));
	for(UINT i = 0;i < NumberOfBytes;i++)
	{
		DBGD((TEXT(" %X "),*(p + i)));
		//RETAILMSG(1, (TEXT(" %X "),*(p + i)));
	}
	if(NumberOfBytes >= 5)
	{
		DealWinceCmd(pTDA7541RadioInfo,&p[3],NumberOfBytes-4);
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
	P_FLY_TDA7541_RADIO_INFO	pTDA7541RadioInfo = (P_FLY_TDA7541_RADIO_INFO)hOpenContext;
	BOOL bRes = TRUE;

	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	DBGD((TEXT("\r\nFlyAudio TDA7541Radio IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)"),
	//		hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	DBGD((TEXT("\r\nFlyAudio TDA7541Radio IOControl Set IOCTL_SERIAL_SET_WAIT_MASK")));
	//		break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	DBGD((TEXT("\r\nFlyAudio TDA7541Radio IOControl Set IOCTL_SERIAL_WAIT_ON_MASK Enter")));
	//		if (pTDA7541RadioInfo->bOpen)
	//		{
	//			WaitForSingleObject(pTDA7541RadioInfo->hBuffToUserDataEvent,INFINITE);
	//		} 
	//		else
	//		{
	//			WaitForSingleObject(pTDA7541RadioInfo->hBuffToUserDataEvent,0);
	//		}
	//		if ((dwLenOut < sizeof(DWORD)) || (NULL == pBufOut) ||
	//			(NULL == pdwActualOut))
	//		{
	//			bRes = FALSE;
	//			break;
	//		}
	//		*(DWORD *)pBufOut = EV_RXCHAR;
	//		*pdwActualOut = sizeof(DWORD);
	//		DBGD((TEXT("\r\nFlyAudio TDA7541Radio IOControl Set IOCTL_SERIAL_WAIT_ON_MASK Exit")));
	//			break;
	//default :
	//	break;
	//}

	return TRUE;
}

BOOL DllEntry(
	HINSTANCE hinstDll,
	DWORD dwReason,
	LPVOID lpReserved
	)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DEBUGMSG(1, (TEXT("Attach in TDA7541Radio DllEntry\n")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}

	if (dwReason == DLL_PROCESS_DETACH)
	{
		DEBUGMSG(1, (TEXT("Dettach in TDA7541Radio DllEntry\n")));
	}

	DEBUGMSG(1, (TEXT("Leaving TDA7541Radio DllEntry\n")));

	return (TRUE);
}
