// FLY7741.cpp : Defines the entry point for the DLL application.

#include "flyhwlib.h"
#include "flyuserlib.h"
#include "flysocketlib.h"
#include "FLY7741_Data.h"
#include "FLY7741.h"

P_FLY_SAF7741_INFO gpSAF7741Info = NULL;

void SAF7741_Mute_P(P_FLY_SAF7741_INFO pSAF7741Info,BOOL para);

VOID SAF7741Enable(P_FLY_SAF7741_INFO pSAF7741Info,BOOL bOn);
VOID
FAU_PowerUp(DWORD hDeviceContext);
VOID
FAU_PowerDown(DWORD hDeviceContext);

#define LARGER(A, B)    ((A) >= (B)? (A):(B))

#define SAF7741_I2C_FREQ    50000      //400Khz
//#define R2S11002_I2C_FREQ   50000      //400Khz
//#define STM32_I2C_FREQ      10000      //400Khz

//#define IO_4052_A_G	3
//#define IO_4052_A_I	10
//#define IO_4052_B_G	3
//#define IO_4052_B_I	9
//
//#define IO_7386_MUTE_G	2
//#define IO_7386_MUTE_I	24

#if SHICHAN_SHICHAN
#define IO_7386_MUTE_ON		0
#define IO_7386_MUTE_OFF	1
#else
#define IO_7386_MUTE_ON		1
#define IO_7386_MUTE_OFF	0
#endif

static void SAF7741_SendVolumeToUser(P_FLY_SAF7741_INFO	pSAF7741Info)
{
	//如果是倒车状态时，不返回音量给界面显示
	if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable)
	{
		if (!pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
		{
			IpcStartEvent(EVENT_GLOBAL_VLAULE_TO_USER);
		}
	}
	else
	{
		IpcStartEvent(EVENT_GLOBAL_VLAULE_TO_USER);
	}
}

static void control_FlyAudio(P_FLY_SAF7741_INFO pSAF7741Info,BYTE *buf, UINT16 len)
{
	if (len <= 0)
		return;

	SocketWrite(buf,len);
}


static VOID Mtk_Loud(P_FLY_SAF7741_INFO pSAF7741Info, BYTE LoudFreq, BYTE LoudLevel)
{
	BYTE buf[2]={0x1C,0x00};

	buf[1]=LoudLevel;
	control_FlyAudio(pSAF7741Info,buf,2);
}

static VOID Mtk_Sub(P_FLY_SAF7741_INFO pSAF7741Info, BYTE SubFreq, BYTE SubLevel)
{
	BYTE buf[2]={0x1D,0x00};

	buf[1]=SubLevel;
	control_FlyAudio(pSAF7741Info,buf,2);
}

static VOID Mtk_Volume(P_FLY_SAF7741_INFO pSAF7741Info, BYTE Volume)
{
	//BYTE buf[2]={0x11,0x00};

	//buf[1]=Volume;
	//control_FlyAudio(pSAF7741Info,buf,2);
}


static void SAF7741SetAudioDefaultValue(P_FLY_SAF7741_INFO pSAF7741Info)
{ 
	//设置开机默认的值
	//Mtk_Loud(pSAF7741Info, 0, FALSE);
	//Mtk_Sub(pSAF7741Info, 0, FALSE);
	//Mtk_Volume(pSAF7741Info, MTK_VOLUME_DEF);
}
/****************************************************************/
/**	                 Write data to MCU			               **/
/****************************************************************/
void writeDataToMcu(P_FLY_SAF7741_INFO pSAF7741Info,BYTE *p,UINT length)
{
	if(FALSE == WriteMsgQueue(pSAF7741Info->hMsgQueueFromExtAmpCreate, p, length, 0, 0))
	{
		DWORD dwError = GetLastError();
		DBGI((TEXT("\r\nExtAmp Write MsgQueue To CarBody Error!0x%x"),dwError));
			switch(dwError)
		{
			case ERROR_INSUFFICIENT_BUFFER:
				DBGI((TEXT("\r\nExtAmp Write MsgQueue To CarBody Error-ERROR_INSUFFICIENT_BUFFER")));
					break;
			case ERROR_OUTOFMEMORY:
				DBGI((TEXT("\r\nExtAmp Write MsgQueue To CarBody Error-ERROR_OUTOFMEMORY")));
					break;				
			case ERROR_PIPE_NOT_CONNECTED:
				DBGI((TEXT("\r\nExtAmp Write MsgQueue To CarBody Error-ERROR_PIPE_NOT_CONNECTED")));
					break;
			case ERROR_TIMEOUT:
				DBGI((TEXT("\r\nExtAmp Write MsgQueue To CarBody Error!ERROR_TIMEOUT")));
					break;
			default:
				DBGI((TEXT("\r\nExtAmp Write MsgQueue To CarBody Error!-unknow error")));
					break;
		}				 

	}
}
/****************************************************************/
/**	                 Read data From MCU			               **/
/****************************************************************/
static DWORD WINAPI ThreadExtAmpRead(LPVOID pContext)
{
	P_FLY_SAF7741_INFO pSAF7741Info = (P_FLY_SAF7741_INFO)pContext;
	ULONG WaitReturn;
	BYTE buff[FLY_MSG_QUEUE_TO_EXTAMP_LENGTH];
	DWORD dwRes;
	DWORD dwMsgFlag;

	while (!pSAF7741Info->bKillDispatchFlyMsgQueueExtAmpReadThread)
	{
		WaitReturn = WaitForSingleObject(pSAF7741Info->hMsgQueueToExtAmpCreate, INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			RETAILMSG(1, (TEXT("\r\nFlyAudio ExtAmp ThreadRead Event hMsgQueueToExtAmp Open Error")));
		}
		ReadMsgQueue(pSAF7741Info->hMsgQueueToExtAmpCreate, buff, FLY_MSG_QUEUE_TO_EXTAMP_LENGTH, &dwRes, 0, &dwMsgFlag);

		if(dwRes)
		{
			DBGD((TEXT("ExtAmp Read MsgQueue%d %x %x"),dwRes,buff[0],buff[1]));
				switch (buff[0])
			{		
				case 0x01:
					if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCheckShellBabyError)
					{
						break;
					}
					if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable
						&& pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
					{
						break;
					}
					if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute)
					{
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume = 0;
					}
					pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume =buff[1];						
					if (0 == pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume)
					{
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
					}
					IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);	
					SAF7741_SendVolumeToUser(pSAF7741Info);
					break;
				case 0x02:
					if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable
						&& pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
					{
						break;
					}
					if (0x00 == buff[1])
					{
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
					}
					else if (0x01 == buff[1])
					{
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
					}
					if (0 == pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume)
					{
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
					}
					IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);
					SAF7741_SendVolumeToUser(pSAF7741Info);
					break;
				case 0x89:
					if(0xFA == buff[1])
					{
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveFlyAudioExtAMP=TRUE;
						if (pSAF7741Info->bPowerUp)
						{
							SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
						}
						FA8200LowVoltageRecovery(pSAF7741Info);
						DBGD((TEXT("\r\nFlyaudio amplifier exists!!!")));
					}
					break;
				default:
					DBGD((TEXT("\r\nFlyAudio ExtAmp ThreadRead Unhandle%d %x %x"),dwRes,buff[0],buff[1]));
						break;
			}
		}
	}
	DBGD((TEXT("\r\nFlyAudio ExtAmp ThreadRead exit")));
		pSAF7741Info->FLyMsgQueueExtAmpReadThreadHandle = NULL;
	return 0;
}

VOID IO_Control_Init(P_FLY_SAF7741_INFO pSAF7741Info)
{
	//SOC_IO_Output(IO_7386_MUTE_G,IO_7386_MUTE_I,IO_7386_MUTE_ON);

	//SOC_IO_Output(IO_4052_A_G,IO_4052_A_I,1);
	//SOC_IO_Output(IO_4052_B_G,IO_4052_B_I,0);
}

static void control4052Input(P_FLY_SAF7741_INFO pSAF7741Info,audio_source eInput)
{
	//if (BT_RING == eInput || MediaMP3 == eInput || IPOD == eInput)
	//{
	//	SOC_IO_Output(IO_4052_A_G,IO_4052_A_I,1);
	//}
	//else if (BT == eInput)
	//{
	//	SOC_IO_Output(IO_4052_A_G,IO_4052_A_I,0);
	//}
}

static void control7386Mute(P_FLY_SAF7741_INFO pSAF7741Info,BOOL bMute)
{
	//DBGI((TEXT("\r\nFlyAudio Mute%d"),bMute));
	//	if (bMute)
	//	{
	//		SOC_IO_Output(IO_7386_MUTE_G,IO_7386_MUTE_I,IO_7386_MUTE_ON);
	//	}
	//	else
	//	{
	//		SOC_IO_Output(IO_7386_MUTE_G,IO_7386_MUTE_I,IO_7386_MUTE_OFF);
	//	}
}

const GUID DEVICE_IFC_I2C_GUID;

UINT SAF7741CreatePrimaI2CRegAddr(UINT addr)
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

static BOOL I2C_Read_SAF7741(P_FLY_SAF7741_INFO pSAF7741Info, ULONG ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
{
	if (!pSAF7741Info->bOpen)
	{
		RETAILMSG(1, (TEXT("FlyAudio SAF7741 IIC Read not Open")));
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

	//DBGD((TEXT("FlyAudio SAF7741 I2C read %X :"),ulRegAddr));
	//for(UINT i = 0;i < uiValBufLen;i++)
	//{
	//	DBGD((TEXT(" %2X "),pRegValBuf[i]));
	//}
	//DBGD((TEXT("\r\n")));
	
	return TRUE;
}
static BOOL I2C_Write_SAF7741(P_FLY_SAF7741_INFO pSAF7741Info, ULONG ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
{
	if (!pSAF7741Info->bOpen)
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

	//DBGD((TEXT("FlyAudio SAF7741 I2C write %X :"),ulRegAddr));
	//for(UINT i = 0;i < uiValBufLen;i++)
	//{
	//	DBGD((TEXT(" %2X "),pRegValBuf[i]));
	//}
	//DBGD((TEXT("\r\n")));

	return TRUE;
}

static VOID SendToSAF7741UsingPortByLength(P_FLY_SAF7741_INFO pSAF7741Info,BYTE *p,BYTE level)
{
	UINT len;
	BYTE *p1,*p2,*p3;
	UINT regAddr;

	p2 = p;
	p3 = p2 + (1+1+3+3);
	len = p3[7];
	p1 = p3 + (1+1+3+3) + level*(5+len*2);
	regAddr = (p1[2] << 16) + (p1[3] << 8) + p1[4];
	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&p1[5],p1[1]-3);
	
	regAddr = (p2[2] << 16) + (p2[3] << 8) + p2[4];
	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&p2[5],p2[1]-3);

	regAddr = (p3[2] << 16) + (p3[3] << 8) + p3[4];
	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&p3[5],p3[1]-3);

}

static void SendToSAF7741NormalWriteData(P_FLY_SAF7741_INFO pSAF7741Info,BYTE *pData)
{
	BYTE MChipAdd;
	UINT iLength;
	UINT dataCnt = 0;
	UINT regAddr;

	while (*pData)
	{
		dataCnt++;
		MChipAdd = *pData++;
		if(MChipAdd != SAF7741_ADDR_W)
		{
			break;
		}
		iLength = *pData++;
		if(pData[0] == 0x00 && pData[1] == 0xFF && pData[2] == 0xFF)
		{
			pData += iLength;
			continue;
		}
		else
		{
			regAddr = (pData[0] << 16) + (pData[1] << 8) + pData[2];
			I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&pData[3],iLength - 3);
			//RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741:I2C write regAdd:%X,%d"),regAddr,iLength - 3));
			pData += iLength;
			//Sleep(10);
		}
	}
}

void SAF7741_GenCRC(BYTE *buf,UINT length)
{
	UINT i;
	BYTE crc = 0;
	for(i = 0;i < length;i++)
	{
		crc += buf[i];
	}
	buf[i] = crc;
}

static void SAF7741_Input_P(P_FLY_SAF7741_INFO pSAF7741Info,audio_source channel,BYTE InputGain)
{
	BYTE reg[6] = {0x0D, 0x00, 0x6F, 0x00, 0x00, 0x00};//XA_EasyP_Index
	BYTE regGain[5] = {0x0D, 0x10, 0x2D, 0x00, 0x00};//ADSP_Y_Vol_SrcScalP
	UINT regAddr;
	
	RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741:I2C input---->")));

	switch(channel)
	{
	case Init:
		reg[3] = 0x00;reg[4] = 0x00;reg[5] = 0x29;//I2S 2 空没来没用的
		regGain[3]=SAF7741_InputGain_Data_PS[InputGain*2+0];
		regGain[4]=SAF7741_InputGain_Data_PS[InputGain*2+1];
		RETAILMSG(1, (TEXT("Init")));
		break;
	case MediaCD://DVD	SPDIF1
		//reg[3] = 0x00;reg[4] = 0x00;reg[5] = 0x2B;//SPDIF 
		reg[3] = 0x00;reg[4] = 0x00;reg[5] = 0x21;//AIN2
		regGain[3]=SAF7741_InputGain_Data_PS[InputGain*2+0];
		regGain[4]=SAF7741_InputGain_Data_PS[InputGain*2+1];
		RETAILMSG(1, (TEXT("MediaCD")));
		break;
	case MediaMP3:
		reg[3] = 0x00;reg[4] = 0x00;reg[5] = 0x1D;//AIN1 
		regGain[3]=SAF7741_InputGain_Data_PS[InputGain*2+0];
		regGain[4]=SAF7741_InputGain_Data_PS[InputGain*2+1];
		control4052Input(pSAF7741Info,MediaMP3);
		RETAILMSG(1, (TEXT("MediaMP3")));
		break;
	case BT_RING:
		reg[3] = 0x00;reg[4] = 0x00;reg[5] = 0x1D;
		regGain[3]=SAF7741_InputGain_Data_PS[InputGain*2+0];
		regGain[4]=SAF7741_InputGain_Data_PS[InputGain*2+1];
		control4052Input(pSAF7741Info,BT_RING);
		RETAILMSG(1, (TEXT("BT_RING")));
		break;
	case IPOD ://适应USB控制的IPOD
		reg[3] = 0x00;reg[4] = 0x00;reg[5] = 0x1D;
		regGain[3]=SAF7741_InputGain_Data_PS[InputGain*2+0];
		regGain[4]=SAF7741_InputGain_Data_PS[InputGain*2+1];
		control4052Input(pSAF7741Info,IPOD);
		RETAILMSG(1, (TEXT("IPOD")));
	case A2DP:                                   
	case BT:     
		reg[3] = 0x00;reg[4] = 0x00;reg[5] = 0x21;//AIN2
		regGain[3]=SAF7741_InputGain_Data_PS[InputGain*2+0];
		regGain[4]=SAF7741_InputGain_Data_PS[InputGain*2+1];
		RETAILMSG(1, (TEXT("A2DP / BT")));
		break;
	case RADIO:  //Radio1
		reg[3] = 0x00;reg[4] = 0x00;reg[5] = 0x13;
		regGain[3]=SAF7741_InputGain_Data_PS[InputGain*2+0];
		regGain[4]=SAF7741_InputGain_Data_PS[InputGain*2+1];
		channel = Init;
		RETAILMSG(1, (TEXT("RADIO")));
		break;
	case EXT_TEL://AIN0
	case GPS:
		reg[3] = 0x00;reg[4] = 0x00;reg[5] = 0x19;
		regGain[3]=SAF7741_InputGain_Data_PS[InputGain*2+0];
		regGain[4]=SAF7741_InputGain_Data_PS[InputGain*2+1];
		RETAILMSG(1, (TEXT("EXT_TEL / GPS")));
		break;
	case AUX ://AIN4
	case TV:
	case CDC:
		reg[3] = 0x00;reg[4] = 0x00;reg[5] = 0x23;
		regGain[3]=SAF7741_InputGain_Data_PS[InputGain*2+0];
		regGain[4]=SAF7741_InputGain_Data_PS[InputGain*2+1];
		RETAILMSG(1, (TEXT("AUX / TV / CDC")));
		break;
	//default:
	//	reg[3] = 0x00;reg[4] = 0x00;reg[5] = 0x23;
	//	regGain[3]=SAF7741_InputGain_Data_PS[InputGain*2+0];
	//	regGain[4]=SAF7741_InputGain_Data_PS[InputGain*2+1];
	//	break;
	}

	//切通道前先静音
	SAF7741_Mute_P(pSAF7741Info,TRUE);
	Mtk_Volume(pSAF7741Info,MTK_VOLUME_MUTE);

	regAddr = (reg[0] << 16) + (reg[1] << 8) + reg[2];
	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&reg[3],3);
	//RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741:Input I2C write regAdd---->%X,Value---->%d"),regAddr,3));
	regAddr = (regGain[0] << 16) + (regGain[1] << 8) + regGain[2];
	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&regGain[3],2);
	//RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741:Gain I2C write regAdd---->%X,Value---->%d"),regAddr,2));


	BYTE buf[2]={0x18,0x00};
	buf[1]=channel;
	control_FlyAudio(pSAF7741Info,buf,2);

	//取消静音
	SAF7741_Mute_P(pSAF7741Info,FALSE);
	Mtk_Volume(pSAF7741Info,MTK_VOLUME_DEF);

	/*

	reg[3] = 0x00;reg[4] = 0x00;reg[5] = 0x1D;//AIN1  
	regGain[3]=SAF7741_InputGain_Data_PS[InputGain*2+0];
	regGain[4]=SAF7741_InputGain_Data_PS[InputGain*2+1];

	BYTE buf[2]={0x18,0x00};
	buf[1]=channel;
	control_FlyAudio(pSAF7741Info,buf,2);

	Sleep(300);
	regAddr = (reg[0] << 16) + (reg[1] << 8) + reg[2];
	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&reg[3],3);

	regAddr = (regGain[0] << 16) + (regGain[1] << 8) + regGain[2];
	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&regGain[3],2);
	*/
}


void SAF7741_Mute_P(P_FLY_SAF7741_INFO pSAF7741Info,BOOL para)
{
	BYTE reg[5] = {0x0D, 0x10, 0x6D, 0x00, 0x00};//ADSP_Y_Mute_P
	UINT regAddr;

	RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741:Mute P---->")));
	regAddr = (reg[0] << 16) + (reg[1] << 8) + reg[2];

	if(para)
	{
		reg[3] = 0x00;reg[4] = 0x00;
		RETAILMSG(1, (TEXT("ON")));
		Mtk_Volume(pSAF7741Info, MTK_VOLUME_MUTE);
		I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&reg[3],2);
	}
	else
	{
		reg[3] = 0x08;reg[4] = 0x00;
		RETAILMSG(1, (TEXT("OFF")));
		I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&reg[3],2);
		Mtk_Volume(pSAF7741Info, MTK_VOLUME_DEF);
	}
}

//主通道AD使用AD12，从通道AD使用AD34，混音通道AD使用空闲的
void SAF7741_Navi_Mix(P_FLY_SAF7741_INFO pSAF7741Info,BYTE para,BYTE iLevel)
{
	BYTE temp = 0;

	//混音大小
	para = para*60/pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax;
	if (para >= 60)
		para = 60;

	BYTE buf[3]={0x49,0x00,0x00};
	buf[1] = para;
	buf[2] = iLevel;

	control_FlyAudio(pSAF7741Info,buf,3);

	/*
	//Navi input AD34
	BYTE reg[6] = {0x0D,0x00,0x6F,0x00,0x00,0x34};//XA_EasyP_Index
	//静音导航	08 00 取消静音
	BYTE regMUTE_Navi[5] = {0x0D, 0x10, 0x6F, 0x00, 0x00};//YA_Mute_N

	BYTE regOpenMixL[5] = {0x0D, 0x10, 0xC2, 0x0b, 0x00};//ADSP_Y_Sup_NonFL
	BYTE regOpenMixR[5] = {0x0D, 0x10, 0xC3, 0x0b, 0x00};//ADSP_Y_Sup_NonFR	

	BYTE regCloseMixL[5] = {0x0D, 0x10, 0xC2, 0x0F, 0xFF};
	BYTE regCloseMixR[5] = {0x0D, 0x10, 0xC3, 0x0F, 0xFF}; 

	//用GUI主面板的控
	BYTE regGain[5] = {0x0D, 0x10,0x4E, 0x00, 0x00};//ADSP_Y_Vol_Nav

	UINT regAddr;

	regGain[3] = SAF7741_InputGain_Data_Mix[para*2+0];
	regGain[4] = SAF7741_InputGain_Data_Mix[para*2+1];

	RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741:Navi Mix---->")));

	if(para)
	{
		regAddr = (reg[0] << 16) + (reg[1] << 8) + reg[2];
		I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&reg[3],3);
		regAddr = (regOpenMixL[0] << 16) + (regOpenMixL[1] << 8) + regOpenMixL[2];
		I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&regOpenMixL[3],2);
		regAddr = (regOpenMixR[0] << 16) + (regOpenMixR[1] << 8) + regOpenMixR[2];
		I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&regOpenMixR[3],2);
		regAddr = (regGain[0] << 16) + (regGain[1] << 8) + regGain[2];
		I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&regGain[3],2);
	
		RETAILMSG(1, (TEXT("ON")));
	}
	else
	{
		regAddr = (regCloseMixL[0] << 16) + (regCloseMixL[1] << 8) + regCloseMixL[2];
		I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&regCloseMixL[3],2);
		regAddr = (regCloseMixR[0] << 16) + (regCloseMixR[1] << 8) + regCloseMixR[2];
		I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&regCloseMixR[3],2);
	
		RETAILMSG(1, (TEXT("OFF")));
	}
	*/
}

static void SAF7741_Volume_P(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iVolume)
{
	BYTE reg1[5] = {0x0D, 0x10, 0x50, 0x00, 0x00};//ADSP_Y_Vol_Main1P
	BYTE reg2[5] = {0x0D, 0x10, 0x52, 0x00, 0x00};//ADSP_Y_Vol_Main1S
	UINT regAddr;

	reg1[3] = SAF7741_Volume_Data[4*iVolume+0];
	reg1[4] = SAF7741_Volume_Data[4*iVolume+1];
	reg2[3] = SAF7741_Volume_Data[4*iVolume+2];
	reg2[4] = SAF7741_Volume_Data[4*iVolume+3];

	if (!iVolume)
		Mtk_Volume(pSAF7741Info, MTK_VOLUME_MUTE);
	else
		Mtk_Volume(pSAF7741Info, MTK_VOLUME_DEF);

	DBGD((TEXT("\r\nFlyAudio SAF7741:SET Volume---->%d"),iVolume));
	regAddr = (reg1[0] << 16) + (reg1[1] << 8) + reg1[2];
	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&reg1[3],2);
	regAddr = (reg2[0] << 16) + (reg2[1] << 8) + reg2[2];
	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&reg2[3],2);
}

static VOID SAF7741_DesScalBMT_P(P_FLY_SAF7741_INFO pSAF7741Info)
{
	BYTE reg[5] = {0x0D, 0x10, 0x77, 0x00, 0x00};//ADSP_Y_BMT_GbasP
	UINT regAddr;

	UINT Temp1,Temp2;
	Temp1 = LARGER(pSAF7741Info->FlyAudioInfo.curBassLevel,pSAF7741Info->FlyAudioInfo.curMidLevel);
	Temp2 = LARGER(Temp1,pSAF7741Info->FlyAudioInfo.curTrebleLevel);

	reg[3] = SAF7741_DesScalBMT_Data[Temp2*2+0];
	reg[4] = SAF7741_DesScalBMT_Data[Temp2*2+1];

	DBGD((TEXT("\r\nFlyAudio SAF7741:Updata DesScalBMT---->%d"),Temp2));

	regAddr = (reg[0] << 16) + (reg[1] << 8) + reg[2];

	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&reg[3],2);
}

static VOID SAF7741_Bass_P(P_FLY_SAF7741_INFO pSAF7741Info,BYTE BassFreq,BYTE BassLevel)
{
	BYTE reg[5] = {0x0D, 0x14, 0x25, 0x00, 0x00};//ADSP_Y_BMT_GbasP
	UINT regAddr;

	DBGD((TEXT("\r\nFlyAudio SAF7741:Set Bass Freq---->%d,BassLevel---->%d"),BassFreq,BassLevel));

	if (BassFreq > (BASS_FREQ_COUNT - 1)) BassFreq = (BASS_FREQ_COUNT - 1);
	if (BassLevel > (BASS_LEVEL_COUNT - 1)) BassLevel = BASS_LEVEL_COUNT - 1;

	reg[3] = SAF7741_Bass_Data_P[BassLevel*2+0];  //60Hz,80Hz,100Hz数据一样
	reg[4] = SAF7741_Bass_Data_P[BassLevel*2+1];

	SendToSAF7741UsingPortByLength(pSAF7741Info,(BYTE *)BassFreqsel,BassFreq);

	regAddr = (reg[0] << 16) + (reg[1] << 8) + reg[2];

	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&reg[3],2);

	SAF7741_DesScalBMT_P(pSAF7741Info);
}

static VOID SAF7741_Mid_P(P_FLY_SAF7741_INFO pSAF7741Info,BYTE MidFreq,BYTE MidLevel)
{
	DBGD((TEXT("\r\nFlyAudio SAF7741:Set Mid Freq---->%d,BassLevel---->%d"),MidFreq,MidLevel));

	if (MidFreq > (MID_FREQ_COUNT - 1)) MidFreq = (MID_FREQ_COUNT - 1);
	if (MidLevel > (MID_LEVEL_COUNT - 1)) MidLevel = MID_LEVEL_COUNT - 1;

	if (MidFreq==M_FREQ_500)
	{		
		SendToSAF7741UsingPortByLength(pSAF7741Info,(BYTE *)SAF7741_Mid_500_Data_P,MidLevel);
	}
	else if (MidFreq==M_FREQ_1000)
	{
		SendToSAF7741UsingPortByLength(pSAF7741Info,(BYTE *)SAF7741_Mid_1000_Data_P,MidLevel);	
	}
	else if (MidFreq==M_FREQ_1500)
	{
		SendToSAF7741UsingPortByLength(pSAF7741Info,(BYTE *)SAF7741_Mid_1500_Data_P,MidLevel);
	}

	SAF7741_DesScalBMT_P(pSAF7741Info);
}

static VOID SAF7741_Treble_P(P_FLY_SAF7741_INFO pSAF7741Info,BYTE TrebFreq,BYTE TrebLevel)
{
	DBGD((TEXT("\r\nFlyAudio SAF7741:Set Treble Freq---->%d,Level---->%d"),TrebFreq,TrebLevel));

	if (TrebFreq > (TREB_FREQ_COUNT - 1)) TrebFreq = TREB_FREQ_COUNT - 1;
	if (TrebLevel > (TREB_LEVEL_COUNT - 1)) TrebLevel = TREB_LEVEL_COUNT - 1;

	if (TrebFreq==T_FREQ_10K)
	{		
		SendToSAF7741UsingPortByLength(pSAF7741Info,(BYTE *)SAF7741_Treble_10k_Data_P,TrebLevel); 
	}
	else if (TrebFreq==T_FREQ_12K)
	{
		SendToSAF7741UsingPortByLength(pSAF7741Info,(BYTE *)SAF7741_Treble_12k_Data_P,TrebLevel); 	
	}

	SAF7741_DesScalBMT_P(pSAF7741Info);
}

static VOID SAF7741_Loud_P(P_FLY_SAF7741_INFO pSAF7741Info,BYTE LoudFreq,BYTE LoudbLevel)
{
	BYTE reg[5] = {0x0D,0x13,0x02,0x00,0x00};  //ADSP_Y_Loudf_MaxBstB
	UINT regAddr;

	DBGD((TEXT("\r\nFlyAudio SAF7741:Set Loud Freq---->%d,Level---->%d"),LoudFreq,LoudbLevel));

	if (LoudFreq > LOUDNESS_FREQ_COUNT) LoudFreq = (LOUDNESS_FREQ_COUNT - 1);
	if (LoudbLevel > LOUDNESS_LEVEL_COUNT) LoudbLevel = (LOUDNESS_LEVEL_COUNT - 1);

	reg[3] = SAF7741_Loud_Data[LoudbLevel*2+0]; 
	reg[4] = SAF7741_Loud_Data[LoudbLevel*2+1];

	SendToSAF7741UsingPortByLength(pSAF7741Info,(BYTE *)SAF7741_Loud_Freq,LoudFreq);

	regAddr = (reg[0] << 16) + (reg[1] << 8) + reg[2];
	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&reg[3],2);
}

static VOID SAF7741_Balance_P(P_FLY_SAF7741_INFO pSAF7741Info,BYTE ibalance)
{
	DBGD((TEXT("\r\nFlyAudio SAF7741:Set Balance---->%d"),ibalance));

	BYTE reg1[5] = {0x0D, 0x10, 0x25, 0x00, 0x00};//ADSP_Y_Vol_BalPL
	BYTE reg2[5] = {0x0D, 0x10, 0x26, 0x00, 0x00};//ADSP_Y_Vol_BalPR
	UINT regAddr;

	if(ibalance > (BALANCE_LEVEL_COUNT - 1)) ibalance = BALANCE_LEVEL_COUNT - 1;

	reg1[3] = SAF7741_Balance_Fader_Data_P[4*ibalance+0];
	reg1[4] = SAF7741_Balance_Fader_Data_P[4*ibalance+1];
	reg2[3] = SAF7741_Balance_Fader_Data_P[4*ibalance+2];
	reg2[4] = SAF7741_Balance_Fader_Data_P[4*ibalance+3];

	regAddr = (reg1[0] << 16) + (reg1[1] << 8) + reg1[2];
	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&reg1[3],2);
	regAddr = (reg2[0] << 16) + (reg2[1] << 8) + reg2[2];
	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&reg2[3],2);
}

void SAF7741_Fader_P(P_FLY_SAF7741_INFO pSAF7741Info,BYTE ifader)
{ 
	DBGD((TEXT("\r\nFlyAudio SAF7741:Set Fader---->%d"),ifader));

	BYTE reg1[5] = {0x0D, 0x10, 0x23, 0x00, 0x00};//YA_Vol_FadF
	BYTE reg2[5] = {0x0D, 0x10, 0x24, 0x00, 0x00};//YA_Vol_FadR
	UINT regAddr;

	if (ifader > (FADER_LEVEL_COUNT - 1)) ifader = FADER_LEVEL_COUNT - 1; 

	reg1[3] = SAF7741_Balance_Fader_Data_P[4*ifader+0];
	reg1[4] = SAF7741_Balance_Fader_Data_P[4*ifader+1];
	reg2[3] = SAF7741_Balance_Fader_Data_P[4*ifader+2];
	reg2[4] = SAF7741_Balance_Fader_Data_P[4*ifader+3];

	regAddr = (reg1[0] << 16) + (reg1[1] << 8) + reg1[2];
	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&reg1[3],2);
	regAddr = (reg2[0] << 16) + (reg2[1] << 8) + reg2[2];
	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&reg2[3],2);
}

static VOID SAF7741_DesScalGEq(P_FLY_SAF7741_INFO pSAF7741Info)
{
	BYTE reg[5] = {0x0D, 0x10, 0x7A, 0x00, 0x00};//YA_Vol_DesScalGEq
	UINT regAddr;

	UINT Temp1,Temp2;
	Temp1 = LARGER(pSAF7741Info->FlyAudioInfo.curEQ[0],pSAF7741Info->FlyAudioInfo.curEQ[1]);
	Temp2 = LARGER(Temp1,pSAF7741Info->FlyAudioInfo.curEQ[2]);
	Temp1 = LARGER(Temp2,pSAF7741Info->FlyAudioInfo.curEQ[3]);
	Temp2 = LARGER(Temp1,pSAF7741Info->FlyAudioInfo.curEQ[4]);
	Temp1 = LARGER(Temp2,pSAF7741Info->FlyAudioInfo.curEQ[5]);
	Temp2 = LARGER(Temp1,pSAF7741Info->FlyAudioInfo.curEQ[6]);
	Temp1 = LARGER(Temp2,pSAF7741Info->FlyAudioInfo.curEQ[7]);
	Temp2 = LARGER(Temp1,pSAF7741Info->FlyAudioInfo.curEQ[8]);
	
	DBGD((TEXT("\r\nFlyAudio SAF7741:Updata DesScalGEq---->%d"),Temp2));

	reg[3] = SAF7741_DesScalGEq_Data[Temp2*2+0];
	reg[4] = SAF7741_DesScalGEq_Data[Temp2*2+1];

	regAddr = (reg[0] << 16) + (reg[1] << 8) + reg[2];

	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&reg[3],2);
}

void SAF7741_EQ_P(P_FLY_SAF7741_INFO pSAF7741Info,UINT sel,UINT level)
{	
	DBGD((TEXT("\r\nFlyAudio SAF7741:Set EQ Band---->%d,level---->%d"),sel,level));

	SendToSAF7741UsingPortByLength(pSAF7741Info,(BYTE *)SAF7741_geq_freq_sel[sel],level);

	SAF7741_DesScalGEq(pSAF7741Info);
}

void SAF7741_Pos_P(P_FLY_SAF7741_INFO pSAF7741Info,UINT pos)
{	
	DBGD((TEXT("\r\nFlyAudio SAF7741:Set Pos---->%d"),pos));

	if(0 == pos)
	{
		SendToSAF7741NormalWriteData(pSAF7741Info,delay_not);	
	}
	else if(1 == pos)
	{
		SendToSAF7741NormalWriteData(pSAF7741Info,delay_pos1);	
	}
	else if(2 == pos)
	{
		SendToSAF7741NormalWriteData(pSAF7741Info,delay_pos2);	
	}
}

static VOID SAF7741_Sub_P(P_FLY_SAF7741_INFO pSAF7741Info,BYTE curSubFilter,BYTE Sublevel)
{
	BYTE reg1[5] = {0x0D,0x00,0x6F,0x00,0xE4};  //Open the SubWoofer
	UINT regAddr;

	DBGD((TEXT("\r\nFlyAudio SAF7741:Set Sub Filter-->%d,level-->%d"),curSubFilter,Sublevel));

	if (curSubFilter > SUB_FILTER_COUNT) curSubFilter = (SUB_FILTER_COUNT - 1);
	if (Sublevel > SUB_LEVEL_COUNT) Sublevel = (SUB_LEVEL_COUNT - 1);

	regAddr = (reg1[0] << 16) + (reg1[1] << 8) + reg1[2];
	I2C_Write_SAF7741(pSAF7741Info,SAF7741CreatePrimaI2CRegAddr(regAddr),&reg1[3],2);

	if (Sub_CutFREQ_80 == curSubFilter)
	{
		SendToSAF7741UsingPortByLength(pSAF7741Info,(BYTE *)SAF7741_subwoofer_80,Sublevel);
	}
	else if (Sub_CutFREQ_120 == curSubFilter)
	{
		SendToSAF7741UsingPortByLength(pSAF7741Info,(BYTE *)SAF7741_subwoofer_120,Sublevel);
	}
	else if (Sub_CutFREQ_160 == curSubFilter)
	{
		SendToSAF7741UsingPortByLength(pSAF7741Info,(BYTE *)SAF7741_subwoofer_160,Sublevel);
	}
}

void volumeFaderInOut(P_FLY_SAF7741_INFO pSAF7741Info,BOOL bEnable)
{
	if (bEnable)
	{
		pSAF7741Info->FlyAudioInfo.bEnableVolumeFader = TRUE;
	}
	else
	{
		pSAF7741Info->FlyAudioInfo.bEnableVolumeFader = FALSE;
	}
}

BOOL bVolumeFaderInOut(P_FLY_SAF7741_INFO pSAF7741Info)
{
	return pSAF7741Info->FlyAudioInfo.bEnableVolumeFader;
}

void returnAudioPowerMode(P_FLY_SAF7741_INFO pSAF7741Info,BOOL bPower)
{
	BYTE buf[] = {0x01,0x00};
	if (bPower)
	{
		buf[1] = 0x01;
	}
	else
	{
		buf[1] = 0x00;
	}

	ReturnToUser(buf,2);
}

void returnAudiobInit(P_FLY_SAF7741_INFO pSAF7741Info,BOOL bInit)
{
	BYTE buf[] = {0x02,0x00};
	if (bInit)
	{
		buf[1] = 0x01;
	}
	else
	{
		buf[1] = 0x00;
	}

	ReturnToUser(buf,2);
}

void returnAudioMainInput(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iInput)
{
	BYTE buf[] = {0x10,0x00};
	buf[1] = iInput;

	ReturnToUser(buf,2);
}

void returnAudioMainMute(P_FLY_SAF7741_INFO pSAF7741Info,BOOL bMute)
{
	BYTE buf[] = {0x11,0x00};
	if (bMute)
	{
		buf[1] = 0x01;
	}
	else
	{
		buf[1] = 0x00;
	}

	ReturnToUser(buf,2);
}

void returnAudioMainVolume(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iVolume)
{
	BYTE buf[] = {0x12,0x00};
	buf[1] = iVolume;

	ReturnToUser(buf,2);
}

void returnAudioMainBalance(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iBalance)
{
	BYTE buf[] = {0x13,0x00};
	buf[1] = iBalance;

	ReturnToUser(buf,2);
}

void returnAudioMainFader(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iFader)
{
	BYTE buf[] = {0x14,0x00};
	buf[1] = iFader;

	ReturnToUser(buf,2);
}

void returnAudioMainTrebFreq(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iTrebFreq)
{
	BYTE buf[] = {0x15,0x00};
	buf[1] = iTrebFreq;

	ReturnToUser(buf,2);
}

void returnAudioMainTrebLevel(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iTrebLevel)
{
	BYTE buf[] = {0x16,0x00};
	buf[1] = iTrebLevel;

	ReturnToUser(buf,2);
}

void returnAudioMainMidFreq(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iMidFreq)
{
	BYTE buf[] = {0x17,0x00};
	buf[1] = iMidFreq;

	ReturnToUser(buf,2);
}

void returnAudioMainMidLevel(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iMidLevel)
{
	BYTE buf[] = {0x18,0x00};
	buf[1] = iMidLevel;

	ReturnToUser(buf,2);
}

void returnAudioMainBassFreq(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iBassFreq)
{
	BYTE buf[] = {0x19,0x00};
	buf[1] = iBassFreq;

	ReturnToUser(buf,2);
}

void returnAudioMainBassLevel(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iBassLevel)
{
	BYTE buf[] = {0x1A,0x00};
	buf[1] = iBassLevel;

	ReturnToUser(buf,2);
}

void returnAudioMainLoudnessFreq(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iLoudnessFreq)
{
	BYTE buf[] = {0x1B,0x00};
	buf[1] = iLoudnessFreq;

	ReturnToUser(buf,2);
}

void returnAudioMainLoudnessLevel(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iLoudnessLevel)
{
	BYTE buf[] = {0x1C,0x00};
	buf[1] = iLoudnessLevel;

	ReturnToUser(buf,2);
}

void returnAudioMainSimEQ(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iSimEQ)
{
	BYTE buf[] = {0x21,0x00};
	buf[1] = iSimEQ;

	ReturnToUser(buf,2);
}

void returnAudioMainLoudnessOn(P_FLY_SAF7741_INFO pSAF7741Info,BOOL bLoudnessOn)
{
	BYTE buf[] = {0x30,0x00};
	if (bLoudnessOn)
	{
		buf[1] = 0x01;
	} 
	else
	{
		buf[1] = 0x00;
	}

	ReturnToUser(buf,2);
}

void returnAudioMainSubOn(P_FLY_SAF7741_INFO pSAF7741Info,BOOL bSubOn)
{
	BYTE buf[] = {0x31,0x00};
	if (bSubOn)
	{
		buf[1] = 0x01;
	} 
	else
	{
		buf[1] = 0x00;
	}

	ReturnToUser(buf,2);
}

void returnAudioMainSubFilter(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iSubFilter)
{
	BYTE buf[] = {0x32,0x00};
	buf[1] = iSubFilter;

	ReturnToUser(buf,2);
}

void returnAudioMainSubLevel(P_FLY_SAF7741_INFO pSAF7741Info,BYTE iSubLevel)
{
	BYTE buf[] = {0x33,0x00};
	buf[1] = iSubLevel;

	ReturnToUser(buf,2);
}

void returnAudioEQAll(P_FLY_SAF7741_INFO pSAF7741Info,BYTE *p)
{
	BYTE buf[10];
	buf[0] = 0x0B;
	memcpy(&buf[1],p,9);

	ReturnToUser(buf,10);
}

void SAF7741_ReadReg(P_FLY_SAF7741_INFO pSAF7741Info,UINT regAddr)
{		
	BYTE SAF7741Reg[27];
	I2C_Read_SAF7741(pSAF7741Info, SAF7741CreatePrimaI2CRegAddr(regAddr), &SAF7741Reg[2], 24);

	RETAILMSG(1, (TEXT("FlyAudio SAF7741 REG[%X] is-->"),regAddr));
	for(int i=0;i<24;i++)
	{
		RETAILMSG(1, (TEXT(" %X"),SAF7741Reg[2+i]));
	}
	SAF7741Reg[0]=0xfe;
	SAF7741Reg[1]=0xaa;
	ReturnToUser(SAF7741Reg,26);
}

static VOID SAF7741DealWinceCmd(P_FLY_SAF7741_INFO pSAF7741Info,BYTE *buf,UINT len)
{
	UINT i;
	RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741 WinCE CMD-->")));
	for (i = 0;i < len;i++)
	{
		RETAILMSG(1, (TEXT(" %X"),buf[i]));
	}

	switch(buf[0])
	{
	case 0x01:
		if (0x01 == buf[1])
		{
			RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741 WinCE CMD-->Init")));
			returnAudioMainInput(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preMainAudioInput);
			returnAudioMainBalance(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preBalance);
			returnAudioMainFader(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preFader);
			returnAudioMainTrebFreq(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preTrebleFreq);
			returnAudioMainTrebLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preTrebleLevel);
			returnAudioMainMidFreq(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preMidFreq);
			returnAudioMainMidLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preMidLevel);
			returnAudioMainBassFreq(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preBassFreq);
			returnAudioMainBassLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preBassLevel);
			returnAudioMainLoudnessFreq(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preLoudFreq);
			returnAudioMainLoudnessLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preLoudLevel);
			returnAudioMainLoudnessOn(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preLoudnessOn);
			returnAudioMainSubOn(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preSubOn);
			returnAudioMainSubFilter(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preSubFilter);
			returnAudioMainSubLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preSubLevel);

			returnAudioMainSimEQ(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preSimEQ);

			pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeAudioSleep = TRUE;
			pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeAudioSleep = FALSE;

			returnAudioPowerMode(pSAF7741Info,TRUE);

			pSAF7741Info->bPowerUp = TRUE;
			pSAF7741Info->bNeedInit = TRUE;
			SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
		}
		else if (0x00 == buf[1])
		{
			returnAudioPowerMode(pSAF7741Info,FALSE);
		}
		break;
	case 0x03:
		if (0x01 == buf[1])
		{
			if (pSAF7741Info->FlyAudioInfo.preMainMute)
			{
				pSAF7741Info->FlyAudioInfo.preMainMute = FALSE;
				pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
			} 
			else
			{
				pSAF7741Info->FlyAudioInfo.preMainMute = TRUE;
				pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
			}
			IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);
			SAF7741_SendVolumeToUser(pSAF7741Info);
			DBGD((TEXT("\r\nSAF7741 MainMute %d"),pSAF7741Info->FlyAudioInfo.preMainMute));
				if (pSAF7741Info->bPowerUp)
				{
					SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
					returnAudioMainMute(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preMainMute);
				}
		}
		else if (0x02 == buf[1])
		{
			pSAF7741Info->FlyAudioInfo.preBalance++;
			if (pSAF7741Info->FlyAudioInfo.preBalance >= BALANCE_LEVEL_COUNT)
			{
				pSAF7741Info->FlyAudioInfo.preBalance = BALANCE_LEVEL_COUNT-1;
			}
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainBalance(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preBalance);
			}
		}
		else if (0x03 == buf[1])
		{
			if (pSAF7741Info->FlyAudioInfo.preBalance)
			{
				pSAF7741Info->FlyAudioInfo.preBalance--;
			}
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainBalance(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preBalance);
			}
		}
		else if (0x04 == buf[1])
		{
			pSAF7741Info->FlyAudioInfo.preFader++;
			if (pSAF7741Info->FlyAudioInfo.preFader >= FADER_LEVEL_COUNT)
			{
				pSAF7741Info->FlyAudioInfo.preFader = FADER_LEVEL_COUNT-1;
			}
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainFader(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preFader);
			}
		}
		else if (0x05 == buf[1])
		{
			if (pSAF7741Info->FlyAudioInfo.preFader)
			{
				pSAF7741Info->FlyAudioInfo.preFader--;
			}
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainFader(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preFader);
			}
		}
		else if (0x06 == buf[1])
		{
			pSAF7741Info->FlyAudioInfo.preTrebleFreq++;
			if (pSAF7741Info->FlyAudioInfo.preTrebleFreq >= TREB_FREQ_COUNT)
			{
				pSAF7741Info->FlyAudioInfo.preTrebleFreq = 0;
			}
			pSAF7741Info->FlyAudioInfo.preSimEQ = 0;
			pSAF7741Info->FlyAudioInfo.usrTrebleFreq = pSAF7741Info->FlyAudioInfo.preTrebleFreq;
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainTrebFreq(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preTrebleFreq);
			}
		}
		else if (0x07 == buf[1])
		{
			pSAF7741Info->FlyAudioInfo.preTrebleLevel++;
			if (pSAF7741Info->FlyAudioInfo.preTrebleLevel >= TREB_LEVEL_COUNT)
			{
				pSAF7741Info->FlyAudioInfo.preTrebleLevel = TREB_LEVEL_COUNT-1;
			}
			pSAF7741Info->FlyAudioInfo.preSimEQ = 0;
			pSAF7741Info->FlyAudioInfo.usrTrebleLevel = pSAF7741Info->FlyAudioInfo.preTrebleLevel;
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainTrebLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preTrebleLevel);
			}
		}
		else if (0x08 == buf[1])
		{
			if (pSAF7741Info->FlyAudioInfo.preTrebleLevel)
			{
				pSAF7741Info->FlyAudioInfo.preTrebleLevel--;
			}
			pSAF7741Info->FlyAudioInfo.preSimEQ = 0;
			pSAF7741Info->FlyAudioInfo.usrTrebleLevel = pSAF7741Info->FlyAudioInfo.preTrebleLevel;
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainTrebLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preTrebleLevel);
			}
		}
		else if (0x09 == buf[1])
		{
			pSAF7741Info->FlyAudioInfo.preMidFreq++;
			if (pSAF7741Info->FlyAudioInfo.preMidFreq >= MID_FREQ_COUNT)
			{
				pSAF7741Info->FlyAudioInfo.preMidFreq = 0;
			}
			pSAF7741Info->FlyAudioInfo.preSimEQ = 0;
			pSAF7741Info->FlyAudioInfo.usrMidFreq = pSAF7741Info->FlyAudioInfo.preMidFreq;
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainMidFreq(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preMidFreq);
			}
		}
		else if (0x0A == buf[1])
		{
			pSAF7741Info->FlyAudioInfo.preMidLevel++;
			if (pSAF7741Info->FlyAudioInfo.preMidLevel >= MID_LEVEL_COUNT)
			{
				pSAF7741Info->FlyAudioInfo.preMidLevel = MID_LEVEL_COUNT-1;
			}
			pSAF7741Info->FlyAudioInfo.preSimEQ = 0;
			pSAF7741Info->FlyAudioInfo.usrMidLevel = pSAF7741Info->FlyAudioInfo.preMidLevel;
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainMidLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preMidLevel);
			}
		}
		else if (0x0B == buf[1])
		{
			if (pSAF7741Info->FlyAudioInfo.preMidLevel)
			{
				pSAF7741Info->FlyAudioInfo.preMidLevel--;
			}
			pSAF7741Info->FlyAudioInfo.preSimEQ = 0;
			pSAF7741Info->FlyAudioInfo.usrMidLevel = pSAF7741Info->FlyAudioInfo.preMidLevel;
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainMidLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preMidLevel);
			}
		}
		else if (0x0C == buf[1])
		{
			pSAF7741Info->FlyAudioInfo.preBassFreq++;
			if (pSAF7741Info->FlyAudioInfo.preBassFreq >= BASS_FREQ_COUNT)
			{
				pSAF7741Info->FlyAudioInfo.preBassFreq = 0;
			}
			pSAF7741Info->FlyAudioInfo.preSimEQ = 0;
			pSAF7741Info->FlyAudioInfo.usrBassFreq = pSAF7741Info->FlyAudioInfo.preBassFreq;
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainBassFreq(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preBassFreq);
			}
		}
		else if (0x0D == buf[1])
		{
			pSAF7741Info->FlyAudioInfo.preBassLevel++;
			if (pSAF7741Info->FlyAudioInfo.preBassLevel >= BASS_LEVEL_COUNT)
			{
				pSAF7741Info->FlyAudioInfo.preBassLevel = BASS_LEVEL_COUNT-1;
			}
			pSAF7741Info->FlyAudioInfo.preSimEQ = 0;
			pSAF7741Info->FlyAudioInfo.usrBassLevel = pSAF7741Info->FlyAudioInfo.preBassLevel;
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainBassLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preBassLevel);
			}
		}
		else if (0x0E == buf[1])
		{
			if (pSAF7741Info->FlyAudioInfo.preBassLevel)
			{
				pSAF7741Info->FlyAudioInfo.preBassLevel--;
			}
			pSAF7741Info->FlyAudioInfo.preSimEQ = 0;
			pSAF7741Info->FlyAudioInfo.usrBassLevel = pSAF7741Info->FlyAudioInfo.preBassLevel;
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainBassLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preBassLevel);
			}
		}
		else if (0x0F == buf[1])
		{
			pSAF7741Info->FlyAudioInfo.preLoudFreq++;
			if (pSAF7741Info->FlyAudioInfo.preLoudFreq >= LOUDNESS_FREQ_COUNT)
			{
				pSAF7741Info->FlyAudioInfo.preLoudFreq = 0;
			}
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainLoudnessFreq(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preLoudFreq);
			}
		}
		else if (0x10 == buf[1])
		{
			pSAF7741Info->FlyAudioInfo.preLoudLevel++;
			if (pSAF7741Info->FlyAudioInfo.preLoudLevel >= LOUDNESS_LEVEL_COUNT)
			{
				pSAF7741Info->FlyAudioInfo.preLoudLevel = LOUDNESS_LEVEL_COUNT-1;
			}
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainLoudnessLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preLoudLevel);
			}
		}
		else if (0x11 == buf[1])
		{
			if (pSAF7741Info->FlyAudioInfo.preLoudLevel)
			{
				pSAF7741Info->FlyAudioInfo.preLoudLevel--;
			}
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainLoudnessLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preLoudLevel);
			}
		}
		else if (0x12 == buf[1])
		{
			pSAF7741Info->FlyAudioInfo.preSubLevel++;
			if (pSAF7741Info->FlyAudioInfo.preSubLevel >= SUB_LEVEL_COUNT)
			{
				pSAF7741Info->FlyAudioInfo.preSubLevel = SUB_LEVEL_COUNT-1;
			}
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainSubLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preSubLevel);
			}
		}
		else if (0x13 == buf[1])
		{
			if (pSAF7741Info->FlyAudioInfo.preSubLevel)
			{
				pSAF7741Info->FlyAudioInfo.preSubLevel--;
			}
			if (pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				returnAudioMainSubLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preSubLevel);
			}
		}
		break;
	case 0x10:
		pSAF7741Info->FlyAudioInfo.preMainAudioInput = (audio_source)buf[1];
		pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eAudioInput = pSAF7741Info->FlyAudioInfo.preMainAudioInput;
		IpcStartEvent(EVENT_GLOBAL_RADIO_ANTENNA_ID);
		IpcStartEvent(EVENT_GLOBAL_EXBOX_INPUT_CHANGE_ID);

	DBGD((TEXT("\r\nSAF7741 MainInput %d"),pSAF7741Info->FlyAudioInfo.preMainAudioInput));
			if(pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				ReturnToUser(buf,len);
			}
			break;
	case 0x11:
		if(1 == buf[1])
		{
			RETAILMSG(1, (TEXT("\r\nSAF7741 MUTE")));
			pSAF7741Info->FlyAudioInfo.preMainMute = TRUE;
			pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
		}
		else
		{
			pSAF7741Info->FlyAudioInfo.preMainMute = FALSE;
			pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
		}
		IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);
		SAF7741_SendVolumeToUser(pSAF7741Info);
		DBGD((TEXT("\r\nSAF7741 MainMute %d"),pSAF7741Info->FlyAudioInfo.preMainMute));
			if(pSAF7741Info->bPowerUp)
			{
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
				ReturnToUser(buf,len);
			}
			break;
	case 0x12:
		if (buf[1] <= 60)
		{
			pSAF7741Info->FlyAudioInfo.preMainVolume = buf[1];
			pSAF7741Info->FlyAudioInfo.preMainMute = FALSE;

			pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume = buf[1];
			pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;

			IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);
			SAF7741_SendVolumeToUser(pSAF7741Info);
			DBGD((TEXT("\r\nSAF7741 MainVolume %d"),pSAF7741Info->FlyAudioInfo.preMainVolume));
				if(pSAF7741Info->bPowerUp)
				{
					//pSAF7741Info->FlyAudioInfo.preMainMute = FALSE;
					//pSAF7741Info->FlyAudioInfo.curMainMute = FALSE;
					//SAF7741_Mute(pSAF7741Info, FALSE);
					SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
					ReturnToUser(buf,len);
				}
		}
		break;
	case 0x13:
		if (buf[1] < BALANCE_LEVEL_COUNT)
		{
			pSAF7741Info->FlyAudioInfo.preBalance = buf[1];
			DBGD((TEXT("\r\nSAF7741 Balance %d"),pSAF7741Info->FlyAudioInfo.preBalance));
				if(pSAF7741Info->bPowerUp)
				{
					SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
					ReturnToUser(buf,len);
				}
		}
		break;
	case 0x14:
		if (buf[1] < FADER_LEVEL_COUNT)
		{
			pSAF7741Info->FlyAudioInfo.preFader = buf[1];
			DBGD((TEXT("\r\nSAF7741 Fader %d"),pSAF7741Info->FlyAudioInfo.preFader));
				if(pSAF7741Info->bPowerUp)
				{
					SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
					ReturnToUser(buf,len);
				}
		}
		break;
	case 0x15:
		if (buf[1] < TREB_FREQ_COUNT)
		{
			pSAF7741Info->FlyAudioInfo.preTrebleFreq = buf[1];
			pSAF7741Info->FlyAudioInfo.preSimEQ = 0;
			pSAF7741Info->FlyAudioInfo.usrTrebleFreq = pSAF7741Info->FlyAudioInfo.preTrebleFreq;
			DBGD((TEXT("\r\nSAF7741 Treble Freq %d"),pSAF7741Info->FlyAudioInfo.preTrebleFreq));
				if(pSAF7741Info->bPowerUp)
				{
					SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
					ReturnToUser(buf,len);
				}
		}
		break;
	case 0x16:
		if (buf[1] < TREB_LEVEL_COUNT)
		{
			pSAF7741Info->FlyAudioInfo.preTrebleLevel = buf[1];
			pSAF7741Info->FlyAudioInfo.preSimEQ = 0;
			pSAF7741Info->FlyAudioInfo.usrTrebleLevel = pSAF7741Info->FlyAudioInfo.preTrebleLevel;
			DBGD((TEXT("\r\nSAF7741 Treble Level %d"),pSAF7741Info->FlyAudioInfo.preTrebleLevel));
				if(pSAF7741Info->bPowerUp)
				{
					SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
					ReturnToUser(buf,len);
				}
		}
		break;

	case 0x17:
		if (buf[1] < MID_FREQ_COUNT)
		{
			pSAF7741Info->FlyAudioInfo.preMidFreq = buf[1];
			pSAF7741Info->FlyAudioInfo.preSimEQ = 0;
			pSAF7741Info->FlyAudioInfo.usrMidFreq = pSAF7741Info->FlyAudioInfo.preMidFreq;
			DBGD((TEXT("\r\nSAF7741 Mid Freq %d"),pSAF7741Info->FlyAudioInfo.preMidFreq));
				if(pSAF7741Info->bPowerUp)
				{
					SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
					ReturnToUser(buf,len);
				}
		}
		break;
	case 0x18:
		if (buf[1] < MID_LEVEL_COUNT)
		{
			pSAF7741Info->FlyAudioInfo.preMidLevel = buf[1];
			pSAF7741Info->FlyAudioInfo.preSimEQ = 0;
			pSAF7741Info->FlyAudioInfo.usrMidLevel = pSAF7741Info->FlyAudioInfo.preMidLevel;
			DBGD((TEXT("\r\nSAF7741 Mid Level %d"),pSAF7741Info->FlyAudioInfo.preMidLevel));
				if(pSAF7741Info->bPowerUp)
				{
					SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
					ReturnToUser(buf,len);
				}
		}
		break;
	case 0x19 :
		if (buf[1] < BASS_FREQ_COUNT)
		{
			pSAF7741Info->FlyAudioInfo.preBassFreq = buf[1];
			pSAF7741Info->FlyAudioInfo.preSimEQ = 0;
			pSAF7741Info->FlyAudioInfo.usrBassFreq = pSAF7741Info->FlyAudioInfo.preBassFreq;
			DBGD((TEXT("\r\nSAF7741 Bass Freq %d"),pSAF7741Info->FlyAudioInfo.preBassFreq));
				if(pSAF7741Info->bPowerUp)
				{
					SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
					ReturnToUser(buf,len);
				}
		}
		break;
	case 0x1A:
		if (buf[1] < BASS_LEVEL_COUNT)
		{
			pSAF7741Info->FlyAudioInfo.preBassLevel = buf[1];
			pSAF7741Info->FlyAudioInfo.preSimEQ = 0;
			pSAF7741Info->FlyAudioInfo.usrBassLevel = pSAF7741Info->FlyAudioInfo.preBassLevel;
			DBGD((TEXT("\r\nSAF7741 Bass Level %d"),pSAF7741Info->FlyAudioInfo.preBassLevel));
				if(pSAF7741Info->bPowerUp)
				{
					SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
					ReturnToUser(buf,len);
				}
		}
		break;
	case 0x1B:
		if (buf[1] < LOUDNESS_FREQ_COUNT)
		{
			pSAF7741Info->FlyAudioInfo.preLoudFreq = buf[1];
			DBGD((TEXT("\r\nSAF7741 Loud Freq %d"),pSAF7741Info->FlyAudioInfo.preLoudFreq));
				if(pSAF7741Info->bPowerUp)
				{
					SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
					ReturnToUser(buf,len);
				}
		}
		break;
	case 0x1C:
		if (buf[1] < LOUDNESS_LEVEL_COUNT)
		{
			pSAF7741Info->FlyAudioInfo.preLoudLevel = buf[1];
			DBGD((TEXT("\r\nSAF7741 Loud Level %d"),pSAF7741Info->FlyAudioInfo.preLoudLevel));
				if(pSAF7741Info->bPowerUp)
				{
					SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
					ReturnToUser(buf,len);
				}
		}
		break;
	case 0x21:
		pSAF7741Info->FlyAudioInfo.preSimEQ = buf[1];
		if (0x01 == buf[1])//缺省
		{
			pSAF7741Info->FlyAudioInfo.preTrebleFreq = pSAF7741Info->FlyAudioInfo.usrTrebleFreq;
			pSAF7741Info->FlyAudioInfo.preTrebleLevel = pSAF7741Info->FlyAudioInfo.usrTrebleLevel;
			pSAF7741Info->FlyAudioInfo.preMidFreq = pSAF7741Info->FlyAudioInfo.usrMidFreq;
			pSAF7741Info->FlyAudioInfo.preMidLevel = pSAF7741Info->FlyAudioInfo.usrMidLevel;
			pSAF7741Info->FlyAudioInfo.preBassFreq = pSAF7741Info->FlyAudioInfo.usrBassFreq;
			pSAF7741Info->FlyAudioInfo.preBassLevel = pSAF7741Info->FlyAudioInfo.usrBassLevel;
			memcpy(&pSAF7741Info->FlyAudioInfo.preEQ[1], EQ_Default, 9);
		}
		else if (0x02 == buf[1])//古典
		{
			pSAF7741Info->FlyAudioInfo.preTrebleFreq = 1;
			pSAF7741Info->FlyAudioInfo.preTrebleLevel = 9;
			pSAF7741Info->FlyAudioInfo.preMidFreq = 0;
			pSAF7741Info->FlyAudioInfo.preMidLevel = 3;
			pSAF7741Info->FlyAudioInfo.preBassFreq = 0;
			pSAF7741Info->FlyAudioInfo.preBassLevel = 10;
			memcpy(&pSAF7741Info->FlyAudioInfo.preEQ[1], EQ_Classic, 9);
		}
		else if (0x03 == buf[1])//流行
		{
			pSAF7741Info->FlyAudioInfo.preTrebleFreq = 0;
			pSAF7741Info->FlyAudioInfo.preTrebleLevel = 7;
			pSAF7741Info->FlyAudioInfo.preMidFreq = 1;
			pSAF7741Info->FlyAudioInfo.preMidLevel = 6;
			pSAF7741Info->FlyAudioInfo.preBassFreq = 1;
			pSAF7741Info->FlyAudioInfo.preBassLevel = 7;
			memcpy(&pSAF7741Info->FlyAudioInfo.preEQ[1], EQ_Pop, 9);
		}
		else if (0x04 == buf[1])//摇滚
		{
			pSAF7741Info->FlyAudioInfo.preTrebleFreq = 0;
			pSAF7741Info->FlyAudioInfo.preTrebleLevel = 8;
			pSAF7741Info->FlyAudioInfo.preMidFreq = 0;
			pSAF7741Info->FlyAudioInfo.preMidLevel = 4;
			pSAF7741Info->FlyAudioInfo.preBassFreq = 0;
			pSAF7741Info->FlyAudioInfo.preBassLevel = 9;
		}
		else if (0x05 == buf[1])//爵士
		{
			pSAF7741Info->FlyAudioInfo.preTrebleFreq = 1;
			pSAF7741Info->FlyAudioInfo.preTrebleLevel = 8;
			pSAF7741Info->FlyAudioInfo.preMidFreq = 1;
			pSAF7741Info->FlyAudioInfo.preMidLevel = 5;
			pSAF7741Info->FlyAudioInfo.preBassFreq = 1;
			pSAF7741Info->FlyAudioInfo.preBassLevel = 8;
		}
		if(pSAF7741Info->bPowerUp)
		{
			SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
			ReturnToUser(buf,len);

			returnAudioMainTrebFreq(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preTrebleFreq);
			returnAudioMainTrebLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preTrebleLevel);
			returnAudioMainMidFreq(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preMidFreq);
			returnAudioMainMidLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preMidLevel);
			returnAudioMainBassFreq(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preBassFreq);
			returnAudioMainBassLevel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.preBassLevel);
		}
		break;
	case 0x30:
		if (0x01 == buf[1])
		{
			pSAF7741Info->FlyAudioInfo.preLoudnessOn = TRUE;
		} 
		else
		{
			pSAF7741Info->FlyAudioInfo.preLoudnessOn = FALSE;
		}
		if(pSAF7741Info->bPowerUp)
		{
			SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
			ReturnToUser(buf,len);
		}
		break;
	case 0x31:
		if (0x01 == buf[1])
		{
			pSAF7741Info->FlyAudioInfo.preSubOn = TRUE;
		} 
		else
		{
			pSAF7741Info->FlyAudioInfo.preSubOn = FALSE;
		}
		if(pSAF7741Info->bPowerUp)
		{
			SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
			ReturnToUser(buf,len);
		}
		break;
	case 0x32:
		if (buf[1] < SUB_FILTER_COUNT)
		{
			pSAF7741Info->FlyAudioInfo.preSubLevel = buf[1];
			DBGD((TEXT("\r\nSAF7741 Sub Filter %d"),pSAF7741Info->FlyAudioInfo.preSubLevel));
				if(pSAF7741Info->bPowerUp)
				{
					SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
					ReturnToUser(buf,len);
				}
		}
		break;
	case 0x33:
		if (buf[1] < SUB_LEVEL_COUNT)
		{
			pSAF7741Info->FlyAudioInfo.preSubLevel = buf[1];
			DBGD((TEXT("\r\nSAF7741 Sub Level %d"),pSAF7741Info->FlyAudioInfo.preSubLevel));
				if(pSAF7741Info->bPowerUp)
				{
					SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
					ReturnToUser(buf,len);
				}
		}
		break;
	case 0x24:
		volumeFaderInOut(pSAF7741Info,TRUE);
		pSAF7741Info->FlyAudioInfo.preGPSSpeaker = buf[1];
		if(pSAF7741Info->bPowerUp)
		{
			ReturnToUser(buf,len);
			if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.GPSSpeaker != buf[1])
			{
				pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.GPSSpeaker = buf[1];
				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
			}
		}
		break;
	case 0xFF:
		if (0x01 == buf[1])
		{
			FAU_PowerUp((DWORD)pSAF7741Info);
		} 
		else if (0x00 == buf[1])
		{
			FAU_PowerDown((DWORD)pSAF7741Info);
		}
		break;
	default:
		RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741 user command unhandle-->%X"),buf[1]));
		break;
	}
	//case 0xfe:
	//	if(buf[1] == 0x55)
	//	{
	//		pSAF7741Info->FlyAudioInfo.ReadRegFlag = TRUE;
	//		pSAF7741Info->FlyAudioInfo.preReadReg = (buf[2] << 16) + (buf[3] << 8) + buf[4];
	//		RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741 ReadReg Addr %X"),pSAF7741Info->FlyAudioInfo.preReadReg));
	//		if(pSAF7741Info->bPowerUp)
	//		{
	//			SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
	//		}
	//	}
	//	break;
}

static VOID SAF7741CommandProcessor(P_FLY_SAF7741_INFO pSAF7741Info,BYTE *buf,UINT len)
{
	static BYTE checksum;
	static UINT FrameLen;
	static UINT FrameLenMax;
	static BYTE ReceiveReg[256];

	for(UINT i = 0;i < len;i++)
	{
		switch (pSAF7741Info->buffToDriversProcessorStatus)
		{
		case 0:
			if(0xFF == buf[i])
			{
				pSAF7741Info->buffToDriversProcessorStatus = 1;
			}
			break;
		case 1:
			if(0x55 == buf[i])
			{
				pSAF7741Info->buffToDriversProcessorStatus = 2;
			}
			else
			{
				pSAF7741Info->buffToDriversProcessorStatus = 0;
			}
			break;
		case 2:
			pSAF7741Info->buffToDriversProcessorStatus = 3;
			FrameLenMax = buf[i];
			FrameLen = 0;
			checksum = buf[i];
			break;
		case 3:
			if(FrameLen < FrameLenMax - 1)
			{
				ReceiveReg[FrameLen] = buf[i];
				checksum += buf[i];
				FrameLen ++;
			}
			else
			{
				if(buf[i] == checksum)
				{
					ReceiveReg[FrameLen] = 0;
					SAF7741DealWinceCmd(pSAF7741Info,ReceiveReg,FrameLen);
				}
				//RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741 crc %X %X"),buf[i],checksum));
				pSAF7741Info->buffToDriversProcessorStatus = 0;
			}
			break;
		default:
			pSAF7741Info->buffToDriversProcessorStatus = 0;
			break;
		}
		//RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741 Status %d"),pSAF7741Info->buffToDriversProcessorStatus));
	}
}

#define ADSP_X_Gsa_Bar321                          0x0D0AFB
#define ADSP_X_Gsa_Bar654                          0x0D0AFA
#define ADSP_X_Gsa_Bar987                          0x0D0AF9

void SAF7741_ReadGraphicalSpectrumAnalyzer(P_FLY_SAF7741_INFO pSAF7741Info)//寄存器地址，字节MSB，Prima先传低字节,7730先接受高字节
{
	BYTE i;
	BYTE Value[3];//7730先发高字节，Prima先存低字节
	BYTE commandValue[] = {0x01,0xF0,1,2,3,4,5,6,7,8,9};
	//return;
	I2C_Read_SAF7741(pSAF7741Info, ADSP_X_Gsa_Bar321, Value, 3);
	pSAF7741Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[0] = Value[2];
	pSAF7741Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[1] = Value[1];
	pSAF7741Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[2] = Value[0];
	//Sleep(10);
	I2C_Read_SAF7741(pSAF7741Info, ADSP_X_Gsa_Bar654, Value, 3);
	pSAF7741Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[3] = Value[2];
	pSAF7741Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[4] = Value[1];
	pSAF7741Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[5] = Value[0];
	//Sleep(10);
	I2C_Read_SAF7741(pSAF7741Info, ADSP_X_Gsa_Bar987, Value, 3);
	pSAF7741Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[6] = Value[2];
	pSAF7741Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[7] = Value[1];
	pSAF7741Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[8] = Value[0];
	//Sleep(10);

	DBGI((TEXT("\r\nFlyAudio SAF7741 Read GraphicalSpectrumAnalyzer-->")));
	for(i = 0;i <9;i++)
	{
		DBGI((TEXT("  %d"),pSAF7741Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[i]));
	}

	memcpy((VOID *)&commandValue[2],(VOID *)&pSAF7741Info->FlyAudioInfo.GraphicalSpectrumAnalyzerValue[0],9);
	ReturnToUser(commandValue,11);
}

//bDelayOn为TRUE时，iDelayStage为0或1两种状态
static DWORD WINAPI SAF7741DelayThread(LPVOID pContext)
{
	P_FLY_SAF7741_INFO pSAF7741Info = (P_FLY_SAF7741_INFO)pContext;
	ULONG WaitReturn;
	BOOL bDelayOn = FALSE;
	BOOL bDelayEnough = FALSE;
	BYTE iDelayStage = 0;
	UINT iDelayTime = 100;
	BOOL bMute = FALSE;
	DWORD iLastTime,iNowTime;

	while (!pSAF7741Info->bKillDispatchSAF7741DelayThread)
	{
		if (bDelayOn)//处理任务中
		{
			iNowTime = GetTickCount();
			if (iNowTime - iLastTime < iDelayTime)
			{
				WaitReturn = WaitForSingleObject(pSAF7741Info->hDispatchSAF7741DelayThreadEvent, iDelayTime - (iNowTime - iLastTime));
			}
			else
			{
				bDelayEnough = TRUE;
			}
			if (!bDelayEnough)//补充
			{
				iNowTime = GetTickCount();
				if (iNowTime - iLastTime >= iDelayTime)
				{
					bDelayEnough = TRUE;
				}
			}
		}
		else
		{
			WaitReturn = WaitForSingleObject(pSAF7741Info->hDispatchSAF7741DelayThreadEvent, INFINITE);
		}

		if (FALSE == pSAF7741Info->bPowerUp)//等待初始化
		{
			bDelayOn = FALSE;iDelayStage = 0;bDelayEnough = FALSE;iDelayTime = 100;iLastTime = GetTickCount();//跳过
			DBGD((TEXT("Wait PowerUp!")));
				continue;
		}

		if(pSAF7741Info->FlyAudioInfo.curMainAudioInput != pSAF7741Info->FlyAudioInfo.tmpMainAudioInput)//切换通道
		{
			if (FALSE == bDelayOn || (bDelayOn && iDelayStage > 2))//启动
			{
				bDelayOn = TRUE;iDelayStage = 1;bDelayEnough = FALSE;iDelayTime = 100;iLastTime = GetTickCount();
				volumeFaderInOut(pSAF7741Info,TRUE);
			}
		}

		if (1 == iDelayStage)
		{
			DBGD((TEXT("\r\nSAF7741 DelayThread MuteOn")));
				bDelayOn = TRUE;iDelayStage = 2;bDelayEnough = FALSE;iDelayTime = 314;iLastTime = GetTickCount();
			if (
				(pSAF7741Info->FlyAudioInfo.curMainAudioInput == MediaMP3 && pSAF7741Info->FlyAudioInfo.tmpMainAudioInput == IPOD)
				|| 
				(pSAF7741Info->FlyAudioInfo.curMainAudioInput == IPOD && pSAF7741Info->FlyAudioInfo.tmpMainAudioInput == MediaMP3)
				)
			{
			}
			else if (
				(pSAF7741Info->FlyAudioInfo.curMainAudioInput == BT && pSAF7741Info->FlyAudioInfo.tmpMainAudioInput == A2DP)
				|| 
				(pSAF7741Info->FlyAudioInfo.curMainAudioInput == A2DP && pSAF7741Info->FlyAudioInfo.tmpMainAudioInput == BT)
				)
			{
			}
			else if (FALSE == bMute)//静音
			{
				DBGD((TEXT(" Run")));
					bMute = TRUE;
				pSAF7741Info->FlyAudioInfo.curMainMute = TRUE;
				SAF7741_Mute_P(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curMainMute);
				pSAF7741Info->FlyAudioInfo.curMainVolume = 0;
				SAF7741_Volume_P(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curMainVolume);
			}
		}
		else if (2 == iDelayStage && bDelayEnough)//切换
		{
			bDelayOn = TRUE;iDelayStage = 3;bDelayEnough = FALSE;iDelayTime = 314;iLastTime = GetTickCount();

			if (pSAF7741Info->FlyAudioInfo.curMainAudioInput != pSAF7741Info->FlyAudioInfo.tmpMainAudioInput)
			{
				DBGD((TEXT("\r\nSAF7741 DelayThread ChangeInput%d"),pSAF7741Info->FlyAudioInfo.tmpMainAudioInput));
					//独立音量控制
					if (EXT_TEL == pSAF7741Info->FlyAudioInfo.curMainAudioInput)//保存当前
					{
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iExtTelVolume
							= pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
					}
					else if (BT == pSAF7741Info->FlyAudioInfo.curMainAudioInput
						|| BT_RING == pSAF7741Info->FlyAudioInfo.curMainAudioInput)
					{
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume
							= pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
					}
					else
					{
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNormalVolume
							= pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNormalMute
							= pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute;
					}

					if (EXT_TEL == pSAF7741Info->FlyAudioInfo.tmpMainAudioInput)//恢复以前
					{
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume
							= pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iExtTelVolume;
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
					}
					else if (BT == pSAF7741Info->FlyAudioInfo.tmpMainAudioInput
						|| BT_RING == pSAF7741Info->FlyAudioInfo.tmpMainAudioInput)
					{
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume
							= pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume;
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = FALSE;
					}
					else
					{
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume
							= pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNormalVolume;
						pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute
							= pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNormalMute;
					}

					if (EXT_TEL == pSAF7741Info->FlyAudioInfo.curMainAudioInput
						|| BT == pSAF7741Info->FlyAudioInfo.curMainAudioInput
						|| BT_RING == pSAF7741Info->FlyAudioInfo.curMainAudioInput

						|| EXT_TEL == pSAF7741Info->FlyAudioInfo.tmpMainAudioInput
						|| BT == pSAF7741Info->FlyAudioInfo.tmpMainAudioInput
						|| BT_RING == pSAF7741Info->FlyAudioInfo.tmpMainAudioInput)
					{
						IpcStartEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);
						SAF7741_SendVolumeToUser(pSAF7741Info);
					}
					//独立音量控制

					pSAF7741Info->FlyAudioInfo.curMainAudioInput = pSAF7741Info->FlyAudioInfo.tmpMainAudioInput;
					pSAF7741Info->FlyAudioInfo.dspMainAudioInput = pSAF7741Info->FlyAudioInfo.tmpMainAudioInput;

					pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eCurAudioInput = pSAF7741Info->FlyAudioInfo.curMainAudioInput;

					SAF7741_Input_P(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curMainAudioInput,audioChannelGainTab[pSAF7741Info->FlyAudioInfo.curMainAudioInput]);

					//MasterSlaveAudioMainChannel(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curMainAudioInput);
					//设定恢复时间
					if (iDelayTime < 618)
					{
						iDelayTime = 618;
					}
				}
				if(pSAF7741Info->FlyAudioInfo.curPos != pSAF7741Info->FlyAudioInfo.prePos)
				{
					DBGD((TEXT("\r\nSAF7741 DelayThread ChangePos")));

					pSAF7741Info->FlyAudioInfo.curPos = pSAF7741Info->FlyAudioInfo.prePos;
					SAF7741_Pos_P(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curPos);
					//设定恢复时间
					if (iDelayTime < 314)
					{
						iDelayTime = 314;
					}
				}
		}

		for(int j=0;j<9;j++)
		{
			if(pSAF7741Info->FlyAudioInfo.curEQ[j] != pSAF7741Info->FlyAudioInfo.preEQ[j])
			{
				pSAF7741Info->FlyAudioInfo.curEQ[j] = pSAF7741Info->FlyAudioInfo.preEQ[j];
				SAF7741_EQ_P(pSAF7741Info,j,pSAF7741Info->FlyAudioInfo.curEQ[j]);
			}
		} 
		/*
		if (pSAF7741Info->FlyAudioInfo.curGPSSpeaker != pSAF7741Info->FlyAudioInfo.preGPSSpeaker)
		{
			pSAF7741Info->FlyAudioInfo.curGPSSpeaker = pSAF7741Info->FlyAudioInfo.preGPSSpeaker;
			if (pSAF7741Info->FlyAudioInfo.curGPSSpeaker)
			{
				SAF7741_Navi_Mix(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curMainVolume);
			}
			else
			{
				SAF7741_Navi_Mix(pSAF7741Info,0);
			}
		}
		*/
		if (IpcWhatEvent(EVENT_GLOBAL_RADIO_MUTE_IN_REQ_ID))//赋值
		{
			pSAF7741Info->FlyAudioInfo.bMuteRadio = TRUE;
		}
		if (IpcWhatEvent(EVENT_GLOBAL_RADIO_MUTE_OUT_REQ_ID))//赋值
		{
			pSAF7741Info->FlyAudioInfo.bMuteRadio = FALSE;
		}
		if (IpcWhatEvent(EVENT_GLOBAL_BT_MUTE_IN_REQ_ID))//赋值
		{
			pSAF7741Info->FlyAudioInfo.bMuteBT = TRUE;
		}
		if (IpcWhatEvent(EVENT_GLOBAL_BT_MUTE_OUT_REQ_ID))//赋值
		{
			pSAF7741Info->FlyAudioInfo.bMuteBT = FALSE;
		}

		if (pSAF7741Info->FlyAudioInfo.bMuteRadio && pSAF7741Info->FlyAudioInfo.curMainAudioInput == RADIO)//收音机静音
		{
			pSAF7741Info->FlyAudioInfo.tmpMainMute = TRUE;
		}
		else if (pSAF7741Info->FlyAudioInfo.bMuteBT && 
			(pSAF7741Info->FlyAudioInfo.curMainAudioInput == BT || pSAF7741Info->FlyAudioInfo.curMainAudioInput == A2DP))//蓝牙静音
		{
			pSAF7741Info->FlyAudioInfo.tmpMainMute = TRUE;
		}
		else
		{
			pSAF7741Info->FlyAudioInfo.tmpMainMute = pSAF7741Info->FlyAudioInfo.preMainMute;
		}

		pSAF7741Info->FlyAudioInfo.dspMainMute = pSAF7741Info->FlyAudioInfo.tmpMainMute;

		if(pSAF7741Info->FlyAudioInfo.curMainMute != pSAF7741Info->FlyAudioInfo.tmpMainMute)//直接静音控制
		{
			if (FALSE == bDelayOn || (bDelayEnough && 3 == iDelayStage))
			{
				DBGD((TEXT("\r\nSAF7741 DelayThread MuteOff?%d"),pSAF7741Info->FlyAudioInfo.tmpMainMute));
					bMute = FALSE;
				pSAF7741Info->FlyAudioInfo.curMainMute = pSAF7741Info->FlyAudioInfo.tmpMainMute;
				SAF7741_Mute_P(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curMainMute);
				if (pSAF7741Info->FlyAudioInfo.curMainMute)
				{
					pSAF7741Info->FlyAudioInfo.curMainVolume = 0;
					SAF7741_Volume_P(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curMainVolume);
				}
			}
		}

		if (pSAF7741Info->FlyAudioInfo.cur7386Mute != pSAF7741Info->FlyAudioInfo.preMainMute)
		{
			pSAF7741Info->FlyAudioInfo.cur7386Mute = pSAF7741Info->FlyAudioInfo.preMainMute;
			control7386Mute(pSAF7741Info,pSAF7741Info->FlyAudioInfo.cur7386Mute);
		}

		if (IpcWhatEvent(EVENT_GLOBAL_RADIO_MUTE_IN_REQ_ID))//返回
		{
			IpcClearEvent(EVENT_GLOBAL_RADIO_MUTE_IN_REQ_ID);
			Sleep(314);
			IpcStartEvent(EVENT_GLOBAL_RADIO_MUTE_IN_OK_ID);
		}
		if (IpcWhatEvent(EVENT_GLOBAL_RADIO_MUTE_OUT_REQ_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_RADIO_MUTE_OUT_REQ_ID);
		}
		if (IpcWhatEvent(EVENT_GLOBAL_BT_MUTE_IN_REQ_ID))//返回
		{
			IpcClearEvent(EVENT_GLOBAL_BT_MUTE_IN_REQ_ID);
			Sleep(314);
			IpcStartEvent(EVENT_GLOBAL_BT_MUTE_IN_OK_ID);
		}
		if (IpcWhatEvent(EVENT_GLOBAL_BT_MUTE_OUT_REQ_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_BT_MUTE_OUT_REQ_ID);
		}

		if(FALSE == pSAF7741Info->FlyAudioInfo.curMainMute && pSAF7741Info->FlyAudioInfo.curMainVolume != pSAF7741Info->FlyAudioInfo.preMainVolume)
		{
			if (FALSE == bDelayOn || (bDelayEnough && 3 == iDelayStage))//静音后的恢复音量
			{
				if (bVolumeFaderInOut(pSAF7741Info))
				{
					if (pSAF7741Info->FlyAudioInfo.curMainVolume < pSAF7741Info->FlyAudioInfo.preMainVolume)
					{
						pSAF7741Info->FlyAudioInfo.curMainVolume++;
						bDelayOn = TRUE;iDelayStage = 3;bDelayEnough = FALSE;iDelayTime = 30;iLastTime = GetTickCount();
					}
					else if (pSAF7741Info->FlyAudioInfo.curMainVolume > pSAF7741Info->FlyAudioInfo.preMainVolume)
					{
						pSAF7741Info->FlyAudioInfo.curMainVolume--;
						bDelayOn = TRUE;iDelayStage = 3;bDelayEnough = FALSE;iDelayTime = 30;iLastTime = GetTickCount();
					}
				}
				else
				{
					pSAF7741Info->FlyAudioInfo.curMainVolume = pSAF7741Info->FlyAudioInfo.preMainVolume;
				}

				if (3 == iDelayStage
					&& pSAF7741Info->FlyAudioInfo.curMainVolume == pSAF7741Info->FlyAudioInfo.preMainVolume)
				{
					iDelayStage = 4;//最后了
					volumeFaderInOut(pSAF7741Info,FALSE);
				}
				else if (FALSE == bDelayOn)
				{
					bDelayOn = TRUE;iDelayStage = 3;bDelayEnough = FALSE;iDelayTime = 30;iLastTime = GetTickCount();
				}

				RETAILMSG(1, (TEXT("\r\nSAF7741 DelayThread Change Volume:%d"),pSAF7741Info->FlyAudioInfo.curMainVolume));
					SAF7741_Volume_P(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curMainVolume);
				if (pSAF7741Info->FlyAudioInfo.curMainVolume == pSAF7741Info->FlyAudioInfo.preMainVolume)
				{
				}
			}
		}
		else
		{
			if (bDelayEnough && 3 == iDelayStage)
			{
				iDelayStage = 4;//最后了
			}
		}

		if (4 == iDelayStage)
		{
			DBGD((TEXT("\r\nSAF7741 DelayThread Finish")));
				bDelayOn = FALSE;iDelayStage = 0;bDelayEnough = FALSE;iDelayTime = 100;iLastTime = GetTickCount();
		}

		if (FALSE == bDelayOn)
		{


			/*
			if (
				(MediaMP3 == pSAF7741Info->FlyAudioInfo.curMainAudioInput
				|| IPOD == pSAF7741Info->FlyAudioInfo.curMainAudioInput)
				)
			{
				pSAF7741Info->FlyAudioInfo.tmpGPSSpeaker = 0;
			}
			else if (
				BT_RING == pSAF7741Info->FlyAudioInfo.curMainAudioInput
				|| BT == pSAF7741Info->FlyAudioInfo.curMainAudioInput
				|| EXT_TEL == pSAF7741Info->FlyAudioInfo.curMainAudioInput)//NaviMix处理逻辑
			{
				pSAF7741Info->FlyAudioInfo.tmpGPSSpeaker = 0;
			}
			else if (pSAF7741Info->FlyAudioInfo.preGPSSpeaker)
			{
				pSAF7741Info->FlyAudioInfo.tmpGPSSpeaker = pSAF7741Info->FlyAudioInfo.preMainVolume;
			}
			else
			{
				pSAF7741Info->FlyAudioInfo.tmpGPSSpeaker = 0;
			}
			if (pSAF7741Info->FlyAudioInfo.curGPSSpeaker != pSAF7741Info->FlyAudioInfo.tmpGPSSpeaker)
			{
				pSAF7741Info->FlyAudioInfo.curGPSSpeaker = pSAF7741Info->FlyAudioInfo.tmpGPSSpeaker;
				SAF7741_Navi_Mix(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curGPSSpeaker);
			}

			pSAF7741Info->FlyAudioInfo.dspGPSSpeaker = pSAF7741Info->FlyAudioInfo.preGPSSpeaker;
			*/
		}

		//导航声音控制
		if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)
		{
			pSAF7741Info->FlyAudioInfo.tmpGPSSpeaker = 0;
		}
		else
		{
			if (BT_RING == pSAF7741Info->FlyAudioInfo.curMainAudioInput
				|| BT == pSAF7741Info->FlyAudioInfo.curMainAudioInput
				|| EXT_TEL == pSAF7741Info->FlyAudioInfo.curMainAudioInput)//NaviMix处理逻辑
			{
				pSAF7741Info->FlyAudioInfo.tmpGPSSpeaker = 0;
			}
			else if (pSAF7741Info->FlyAudioInfo.preGPSSpeaker && !pSAF7741Info->FlyAudioInfo.dspMainMute)
			{
				if(pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp)
				{
					pSAF7741Info->FlyAudioInfo.tmpGPSSpeaker =CAR_AMP_IN_VOLUME_CONSTANT*pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax/60;
				}
				else if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable 
					&& pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
				{
					if (pSAF7741Info->FlyAudioInfo.curGPSSpeaker > pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBackLowestVolume)
					{
						pSAF7741Info->FlyAudioInfo.tmpGPSSpeaker = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBackLowestVolume;
					}
				}
				else
				{
					pSAF7741Info->FlyAudioInfo.tmpGPSSpeaker = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
				}
			}
			else
			{
				pSAF7741Info->FlyAudioInfo.tmpGPSSpeaker = 0;
			}
		}

		if (pSAF7741Info->FlyAudioInfo.curGPSSpeaker != pSAF7741Info->FlyAudioInfo.tmpGPSSpeaker
			|| IpcWhatEvent(EVENT_GLOBAL_GPS_VOLUME_LEVEL_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_GPS_VOLUME_LEVEL_ID);
			if (pSAF7741Info->FlyAudioInfo.tmpGPSSpeaker > pSAF7741Info->FlyAudioInfo.curGPSSpeaker)
			{
				pSAF7741Info->FlyAudioInfo.curGPSSpeaker++;
			}
			else if (pSAF7741Info->FlyAudioInfo.tmpGPSSpeaker < pSAF7741Info->FlyAudioInfo.curGPSSpeaker)
			{
				pSAF7741Info->FlyAudioInfo.curGPSSpeaker--;
			}

			SAF7741_Navi_Mix(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curGPSSpeaker,
				pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGPSVolumeLevel);
			Sleep(5);
			SetEvent(pSAF7741Info->hDispatchSAF7741DelayThreadEvent);
		}
		pSAF7741Info->FlyAudioInfo.dspGPSSpeaker = pSAF7741Info->FlyAudioInfo.preGPSSpeaker;

		/*******************************************************/
		//SetEvent(pSAF7741Info->hDispatchExtAmpThreadEvent);
		/*******************************************************/
	}
	pSAF7741Info->SAF7741DelayThreadHandle = NULL;
	return 0;
}

static DWORD WINAPI SAF7741MainThread(LPVOID pContext)
{
	P_FLY_SAF7741_INFO pSAF7741Info = (P_FLY_SAF7741_INFO)pContext;
	ULONG WaitReturn;

	//BYTE SAF7741ID[4];

	while (!pSAF7741Info->bKillDispatchSAF7741MainThread)
	{

		WaitReturn = WaitForSingleObject(pSAF7741Info->hDispatchSAF7741MainThreadEvent, INFINITE);	

		if (IpcWhatEvent(EVENT_GLOBAL_SLEEP_PROC_AUDIO_ID))
		{
			pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeAudioSleep
				= pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeAudioSleep;			
		}

		if (FALSE == pSAF7741Info->bPowerUp)
		{
		}
		else
		{
			if (pSAF7741Info->bNeedInit)
			{
				pSAF7741Info->bNeedInit = FALSE;

				DBGD((TEXT("\r\nSAF7741 Init start....")));

				SendToSAF7741NormalWriteData(pSAF7741Info,SAF7741_Audio_Init_Data);//初始化参数1


				SAF7741_Mute_P(pSAF7741Info,FALSE);

				pSAF7741Info->bPowerUp = TRUE;

				pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable = TRUE;//音频芯片替其它驱动控制静音开关

				returnAudiobInit(pSAF7741Info,TRUE);

				SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);

				if (!pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSilencePowerUp)
				{
					IpcStartEvent(EVENT_GLOBAL_INNER_AMP_ON_ID);
				}
				Sleep(618);

				pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iNeedProcVoltageShakeAudio = 80;//低电压

				DBGD((TEXT("\r\nSAF7741 Init OK")));
			}

			if (IpcWhatEvent(EVENT_GLOBAL_BATTERY_RECOVERY_AUDIO_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_BATTERY_RECOVERY_AUDIO_ID);
				if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBatteryVoltageLowAudio)
				{
				}
				else
				{
					//pSAF7741Info->bNeedInit = TRUE;
				}
			}

			if (IpcWhatEvent(EVENT_GLOBAL_SLEEP_PROC_AUDIO_ID))//待机要静音，静音完再清除这个标志
			{
				IpcClearEvent(EVENT_GLOBAL_SLEEP_PROC_AUDIO_ID);
				pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute = TRUE;
			}

			if (IpcWhatEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME))
			{
				IpcClearEvent(EVENT_GLOBAL_DATA_CHANGE_VOLUME);
				if (pSAF7741Info->FlyAudioInfo.preMainMute != pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute)//全局
				{
					pSAF7741Info->FlyAudioInfo.preMainMute = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute;
					DBGD((TEXT("\r\nFlyAudio SAF7741 Global Mute-->%d"),pSAF7741Info->FlyAudioInfo.preMainMute));
				}
				if (pSAF7741Info->FlyAudioInfo.preMainVolume != pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume)//全局
				{
					pSAF7741Info->FlyAudioInfo.preMainVolume = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
					pSAF7741Info->FlyAudioInfo.dspMainVolume = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
					DBGD((TEXT("\r\nFlyAudio SAF7741 Global Volume-->%d"),pSAF7741Info->FlyAudioInfo.preMainVolume));
				}
			}

			if (IpcWhatEvent(EVENT_GLOBAL_STANDBY_AUDIO_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_STANDBY_AUDIO_ID);
				if (!pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)
				{
					//Sleep(618);
					//SendToSAF7741NormalWriteData(pSAF7741Info,SAF7741_Audio_Init_Data);//初始化参数1
				}
			}

			if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)
			{
				pSAF7741Info->FlyAudioInfo.preMainMute = TRUE;
			}
			else if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBatteryVoltageLowAudio)
			{
				pSAF7741Info->FlyAudioInfo.preMainMute = TRUE;
			}
			else
			{
				pSAF7741Info->FlyAudioInfo.preMainMute = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute;
			}

			//倒车降低音量
			if (IpcWhatEvent(EVENT_GLOBAL_BACK_LOW_VOLUME_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_BACK_LOW_VOLUME_ID);
				volumeFaderInOut(pSAF7741Info,TRUE);
			}


			/*
			if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveFlyAudioExtAMP)
			{
				pSAF7741Info->FlyAudioInfo.preMainVolume = 34;
			}
			else if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable
				&& pSAF7741Info->FlyAudioInfo.curMainAudioInput != EXT_TEL
				&& pSAF7741Info->FlyAudioInfo.curMainAudioInput != BT 
				&& pSAF7741Info->FlyAudioInfo.curMainAudioInput != BT_RING)
			{
				if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow
					&& pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn)
				{
					if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume > 20)
					{
						pSAF7741Info->FlyAudioInfo.preMainVolume = 20;
					}
					else
					{
						pSAF7741Info->FlyAudioInfo.preMainVolume = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
					}				
				}
				else
				{
					pSAF7741Info->FlyAudioInfo.preMainVolume = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
				}
			}
			else if (pSAF7741Info->FlyAudioInfo.preGPSSpeaker
				&& MediaMP3 != pSAF7741Info->FlyAudioInfo.curMainAudioInput
				&& IPOD != pSAF7741Info->FlyAudioInfo.curMainAudioInput
				&& BT_RING != pSAF7741Info->FlyAudioInfo.curMainAudioInput
				&& BT != pSAF7741Info->FlyAudioInfo.curMainAudioInput
				&& EXT_TEL != pSAF7741Info->FlyAudioInfo.curMainAudioInput)
			{
				if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume > 3)
				{
					pSAF7741Info->FlyAudioInfo.preMainVolume = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume - 3;
				}
				else
				{
					pSAF7741Info->FlyAudioInfo.preMainVolume = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
				}				
			}
			else
			{
				pSAF7741Info->FlyAudioInfo.preMainVolume = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
			}
			*/

			else if (BT_RING == pSAF7741Info->FlyAudioInfo.curMainAudioInput
				||  BT == pSAF7741Info->FlyAudioInfo.curMainAudioInput)
			{
				pSAF7741Info->FlyAudioInfo.preMainVolume = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallVolume;
			}
			if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveFlyAudioExtAMP)
			{
				pSAF7741Info->FlyAudioInfo.preMainVolume = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iFlyAudioAmpVolume;
			}
			else if(pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCarHaveAmp)
			{
				pSAF7741Info->FlyAudioInfo.preMainVolume =CAR_AMP_IN_VOLUME_CONSTANT*pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolumeMax/60;
			}
			else if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackDetectEnable
				&& pSAF7741Info->FlyAudioInfo.curMainAudioInput != EXT_TEL
				&& pSAF7741Info->FlyAudioInfo.curMainAudioInput != BT 
				&& pSAF7741Info->FlyAudioInfo.curMainAudioInput != BT_RING)
			{
				if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
					//&& pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackVideoOn)
				{
					if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume > pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBackLowestVolume)
					{
						pSAF7741Info->FlyAudioInfo.preMainVolume = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBackLowestVolume;
					}
					else
					{
						pSAF7741Info->FlyAudioInfo.preMainVolume = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
					}				
				}
				else
				{
					pSAF7741Info->FlyAudioInfo.preMainVolume = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
				}
			}
			else
			{
				pSAF7741Info->FlyAudioInfo.preMainVolume = pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iVolume;
			}

			if (IpcWhatEvent(EVENT_GLOBAL_BTCALLSTATUS_CHANGE_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_BTCALLSTATUS_CHANGE_ID);
			}
			if (IpcWhatEvent(EVENT_GLOBAL_PHONECALLSTATUS_CHANGE_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_PHONECALLSTATUS_CHANGE_ID);
			}
			if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iExtTelCallStatus)//外部电话
			{
				pSAF7741Info->FlyAudioInfo.tmpMainAudioInput = EXT_TEL;
			}
			else if (0x03 == pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus)//通话中
			{
				pSAF7741Info->FlyAudioInfo.tmpMainAudioInput = BT;
			}
			else if (0x02 == pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus)//去电中
			{
				pSAF7741Info->FlyAudioInfo.tmpMainAudioInput = BT;
			}
			else if (0x01 == pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus)//来电中
			{
				pSAF7741Info->FlyAudioInfo.tmpMainAudioInput = BT_RING;
			}
			else//平时正常
			{
				pSAF7741Info->FlyAudioInfo.tmpMainAudioInput = pSAF7741Info->FlyAudioInfo.preMainAudioInput;
			}

			if (pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveFlyAudioExtAMP)
			{
				pSAF7741Info->FlyAudioInfo.tmpBassFreq = 0;
				pSAF7741Info->FlyAudioInfo.tmpBassLevel = BASS_LEVEL_COUNT/2;
				pSAF7741Info->FlyAudioInfo.tmpMidFreq = 0;
				pSAF7741Info->FlyAudioInfo.tmpMidLevel = MID_LEVEL_COUNT/2;
				pSAF7741Info->FlyAudioInfo.tmpTrebleFreq = 0;
				pSAF7741Info->FlyAudioInfo.tmpTrebleLevel = TREB_LEVEL_COUNT/2;
				pSAF7741Info->FlyAudioInfo.tmpLoudnessOn = FALSE;
				pSAF7741Info->FlyAudioInfo.tmpLoudFreq = 0;
				pSAF7741Info->FlyAudioInfo.tmpLoudLevel = LOUDNESS_LEVEL_COUNT/2;
				pSAF7741Info->FlyAudioInfo.tmpBalance = BALANCE_LEVEL_COUNT/2;
				pSAF7741Info->FlyAudioInfo.tmpFader = FADER_LEVEL_COUNT/2;
				pSAF7741Info->FlyAudioInfo.tmpSubOn = FALSE;
				pSAF7741Info->FlyAudioInfo.tmpSubFilter = 0;
				pSAF7741Info->FlyAudioInfo.tmpSubLevel = 0;
			}
			else if (pSAF7741Info->FlyAudioInfo.tmpMainAudioInput == EXT_TEL
				|| pSAF7741Info->FlyAudioInfo.tmpMainAudioInput == BT 
				|| pSAF7741Info->FlyAudioInfo.tmpMainAudioInput == BT_RING)
			{
				pSAF7741Info->FlyAudioInfo.tmpBassFreq = 0;
				pSAF7741Info->FlyAudioInfo.tmpBassLevel = BASS_LEVEL_COUNT/2;
				pSAF7741Info->FlyAudioInfo.tmpMidFreq = 0;
				pSAF7741Info->FlyAudioInfo.tmpMidLevel = MID_LEVEL_COUNT/2;
				pSAF7741Info->FlyAudioInfo.tmpTrebleFreq = 0;
				pSAF7741Info->FlyAudioInfo.tmpTrebleLevel = TREB_LEVEL_COUNT/2;
				pSAF7741Info->FlyAudioInfo.tmpLoudnessOn = FALSE;
				pSAF7741Info->FlyAudioInfo.tmpLoudFreq = 0;
				pSAF7741Info->FlyAudioInfo.tmpLoudLevel = LOUDNESS_LEVEL_COUNT/2;
				pSAF7741Info->FlyAudioInfo.tmpBalance = BALANCE_LEVEL_COUNT/2;
				pSAF7741Info->FlyAudioInfo.tmpFader = FADER_LEVEL_COUNT/2;
				pSAF7741Info->FlyAudioInfo.tmpSubOn = FALSE;
				pSAF7741Info->FlyAudioInfo.tmpSubFilter = 0;
				pSAF7741Info->FlyAudioInfo.tmpSubLevel = 0;		
			}
			else
			{
				pSAF7741Info->FlyAudioInfo.tmpBassFreq = pSAF7741Info->FlyAudioInfo.preBassFreq;
				pSAF7741Info->FlyAudioInfo.tmpBassLevel = pSAF7741Info->FlyAudioInfo.preBassLevel;
				pSAF7741Info->FlyAudioInfo.tmpMidFreq = pSAF7741Info->FlyAudioInfo.preMidFreq;
				pSAF7741Info->FlyAudioInfo.tmpMidLevel = pSAF7741Info->FlyAudioInfo.preMidLevel;
				pSAF7741Info->FlyAudioInfo.tmpTrebleFreq = pSAF7741Info->FlyAudioInfo.preTrebleFreq;
				pSAF7741Info->FlyAudioInfo.tmpTrebleLevel = pSAF7741Info->FlyAudioInfo.preTrebleLevel;
				pSAF7741Info->FlyAudioInfo.tmpLoudnessOn = pSAF7741Info->FlyAudioInfo.preLoudnessOn;
				pSAF7741Info->FlyAudioInfo.tmpLoudFreq = pSAF7741Info->FlyAudioInfo.preLoudFreq;
				pSAF7741Info->FlyAudioInfo.tmpLoudLevel = pSAF7741Info->FlyAudioInfo.preLoudLevel;
				pSAF7741Info->FlyAudioInfo.tmpBalance = pSAF7741Info->FlyAudioInfo.preBalance;
				pSAF7741Info->FlyAudioInfo.tmpFader = pSAF7741Info->FlyAudioInfo.preFader;
				pSAF7741Info->FlyAudioInfo.tmpSubOn = pSAF7741Info->FlyAudioInfo.preSubOn;
				pSAF7741Info->FlyAudioInfo.tmpSubFilter = pSAF7741Info->FlyAudioInfo.preSubFilter;
				pSAF7741Info->FlyAudioInfo.tmpSubLevel = pSAF7741Info->FlyAudioInfo.preSubLevel;
			}

			pSAF7741Info->FlyAudioInfo.dspBassFreq = pSAF7741Info->FlyAudioInfo.preBassFreq;
			pSAF7741Info->FlyAudioInfo.dspBassLevel = pSAF7741Info->FlyAudioInfo.preBassLevel;
			pSAF7741Info->FlyAudioInfo.dspMidFreq = pSAF7741Info->FlyAudioInfo.preMidFreq;
			pSAF7741Info->FlyAudioInfo.dspMidLevel = pSAF7741Info->FlyAudioInfo.preMidLevel;
			pSAF7741Info->FlyAudioInfo.dspTrebleFreq = pSAF7741Info->FlyAudioInfo.preTrebleFreq;
			pSAF7741Info->FlyAudioInfo.dspTrebleLevel = pSAF7741Info->FlyAudioInfo.preTrebleLevel;
			pSAF7741Info->FlyAudioInfo.dspLoudnessOn = pSAF7741Info->FlyAudioInfo.preLoudnessOn;
			pSAF7741Info->FlyAudioInfo.dspLoudFreq = pSAF7741Info->FlyAudioInfo.preLoudFreq;
			pSAF7741Info->FlyAudioInfo.dspLoudLevel = pSAF7741Info->FlyAudioInfo.preLoudLevel;
			pSAF7741Info->FlyAudioInfo.dspBalance = pSAF7741Info->FlyAudioInfo.preBalance;
			pSAF7741Info->FlyAudioInfo.dspFader = pSAF7741Info->FlyAudioInfo.preFader;
			pSAF7741Info->FlyAudioInfo.dspSubOn = pSAF7741Info->FlyAudioInfo.preSubOn;
			pSAF7741Info->FlyAudioInfo.dspSubFilter = pSAF7741Info->FlyAudioInfo.preSubFilter;
			pSAF7741Info->FlyAudioInfo.dspSubLevel = pSAF7741Info->FlyAudioInfo.preSubLevel;

			/*******************************************************/
			//SetEvent(pSAF7741Info->hDispatchExtAmpThreadEvent);//这个任务在延时任务
			/*******************************************************/

			SetEvent(pSAF7741Info->hDispatchSAF7741DelayThreadEvent);//启动延时任务，控制切换等时序

			if (pSAF7741Info->FlyAudioInfo.curSimEQ != pSAF7741Info->FlyAudioInfo.preSimEQ)
			{
				pSAF7741Info->FlyAudioInfo.curSimEQ = pSAF7741Info->FlyAudioInfo.preSimEQ;
				returnAudioMainSimEQ(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curSimEQ);
			}

			if(pSAF7741Info->FlyAudioInfo.curBassFreq != pSAF7741Info->FlyAudioInfo.tmpBassFreq || pSAF7741Info->FlyAudioInfo.curBassLevel != pSAF7741Info->FlyAudioInfo.tmpBassLevel)
			{				
				pSAF7741Info->FlyAudioInfo.curBassFreq = pSAF7741Info->FlyAudioInfo.tmpBassFreq;
				pSAF7741Info->FlyAudioInfo.curBassLevel = pSAF7741Info->FlyAudioInfo.tmpBassLevel;
				SAF7741_Bass_P(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curBassFreq,pSAF7741Info->FlyAudioInfo.curBassLevel);
			}
			if(pSAF7741Info->FlyAudioInfo.curMidFreq != pSAF7741Info->FlyAudioInfo.tmpMidFreq || pSAF7741Info->FlyAudioInfo.curMidLevel != pSAF7741Info->FlyAudioInfo.tmpMidLevel)
			{
				pSAF7741Info->FlyAudioInfo.curMidFreq = pSAF7741Info->FlyAudioInfo.tmpMidFreq;
				pSAF7741Info->FlyAudioInfo.curMidLevel = pSAF7741Info->FlyAudioInfo.tmpMidLevel;
				SAF7741_Mid_P(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curMidFreq,pSAF7741Info->FlyAudioInfo.curMidLevel);
			}
			if(pSAF7741Info->FlyAudioInfo.curTrebleFreq != pSAF7741Info->FlyAudioInfo.tmpTrebleFreq || pSAF7741Info->FlyAudioInfo.curTrebleLevel != pSAF7741Info->FlyAudioInfo.tmpTrebleLevel)
			{
				pSAF7741Info->FlyAudioInfo.curTrebleFreq = pSAF7741Info->FlyAudioInfo.tmpTrebleFreq;
				pSAF7741Info->FlyAudioInfo.curTrebleLevel = pSAF7741Info->FlyAudioInfo.tmpTrebleLevel;
				SAF7741_Treble_P(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curTrebleFreq,pSAF7741Info->FlyAudioInfo.curTrebleLevel);
			}
			if (pSAF7741Info->FlyAudioInfo.curLoudnessOn != pSAF7741Info->FlyAudioInfo.tmpLoudnessOn
				|| pSAF7741Info->FlyAudioInfo.curLoudFreq != pSAF7741Info->FlyAudioInfo.tmpLoudFreq 
				|| pSAF7741Info->FlyAudioInfo.curLoudLevel != pSAF7741Info->FlyAudioInfo.tmpLoudLevel)
			{
				pSAF7741Info->FlyAudioInfo.curLoudnessOn = pSAF7741Info->FlyAudioInfo.tmpLoudnessOn;
				pSAF7741Info->FlyAudioInfo.curLoudFreq = pSAF7741Info->FlyAudioInfo.tmpLoudFreq;
				pSAF7741Info->FlyAudioInfo.curLoudLevel = pSAF7741Info->FlyAudioInfo.tmpLoudLevel;
				if (pSAF7741Info->FlyAudioInfo.curLoudnessOn)
				{
					SAF7741_Loud_P(pSAF7741Info, pSAF7741Info->FlyAudioInfo.curLoudFreq, pSAF7741Info->FlyAudioInfo.curLoudLevel);
				} 
				else
				{
					SAF7741_Loud_P(pSAF7741Info,  0, LOUDNESS_LEVEL_COUNT/2);
				}
			}
			if((pSAF7741Info->FlyAudioInfo.curBalance != pSAF7741Info->FlyAudioInfo.tmpBalance) || (pSAF7741Info->FlyAudioInfo.curFader != pSAF7741Info->FlyAudioInfo.tmpFader))
			{
				pSAF7741Info->FlyAudioInfo.curBalance = pSAF7741Info->FlyAudioInfo.tmpBalance;
				pSAF7741Info->FlyAudioInfo.curFader = pSAF7741Info->FlyAudioInfo.tmpFader;
				SAF7741_Balance_P(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curBalance);
				SAF7741_Fader_P(pSAF7741Info, pSAF7741Info->FlyAudioInfo.curFader);
			}
			if (pSAF7741Info->FlyAudioInfo.curSubOn != pSAF7741Info->FlyAudioInfo.tmpSubOn
				|| pSAF7741Info->FlyAudioInfo.curSubFilter != pSAF7741Info->FlyAudioInfo.tmpSubFilter
				|| pSAF7741Info->FlyAudioInfo.curSubLevel != pSAF7741Info->FlyAudioInfo.tmpSubLevel)
			{
				pSAF7741Info->FlyAudioInfo.curSubOn = pSAF7741Info->FlyAudioInfo.tmpSubOn;
				pSAF7741Info->FlyAudioInfo.curSubFilter = pSAF7741Info->FlyAudioInfo.tmpSubFilter;
				pSAF7741Info->FlyAudioInfo.curSubLevel = pSAF7741Info->FlyAudioInfo.tmpSubLevel;
				if (pSAF7741Info->FlyAudioInfo.curSubOn)
				{
					SAF7741_Sub_P(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curSubFilter,pSAF7741Info->FlyAudioInfo.curSubLevel);
				} 
				else
				{
					SAF7741_Sub_P(pSAF7741Info,0,0);
				}
			}

			if(pSAF7741Info->FlyAudioInfo.curReadReg != pSAF7741Info->FlyAudioInfo.preReadReg || pSAF7741Info->FlyAudioInfo.ReadRegFlag == TRUE)
			{
				pSAF7741Info->FlyAudioInfo.ReadRegFlag=FALSE;
				pSAF7741Info->FlyAudioInfo.curReadReg = pSAF7741Info->FlyAudioInfo.preReadReg;
				SAF7741_ReadReg(pSAF7741Info,pSAF7741Info->FlyAudioInfo.curReadReg);
			}

			SAF7741_ReadGraphicalSpectrumAnalyzer(pSAF7741Info);
		}
	}

	RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741 Main Thread---->exit!")));
	pSAF7741Info->bPowerUp = FALSE;
	while (pSAF7741Info->SAF7741DelayThreadHandle)
	{
		SetEvent(pSAF7741Info->hDispatchSAF7741DelayThreadEvent);
		Sleep(10);
	}
	CloseHandle(pSAF7741Info->hDispatchSAF7741DelayThreadEvent);


	CloseHandle(pSAF7741Info->hDispatchSAF7741MainThreadEvent);

	CloseHandle(pSAF7741Info->buffToUserDataEvent);
	return 0;
}

void FlyAudio_ParaInitDefault(P_FLY_SAF7741_INFO pSAF7741Info)
{
	pSAF7741Info->FlyAudioInfo.bMuteRadio = FALSE;
	pSAF7741Info->FlyAudioInfo.bMuteBT = FALSE;

	pSAF7741Info->FlyAudioInfo.preMainAudioInput = MediaMP3;
	pSAF7741Info->FlyAudioInfo.curMainAudioInput = Init;

	pSAF7741Info->FlyAudioInfo.preMainVolume = 0;//涉及到淡入淡出，全部归零
	pSAF7741Info->FlyAudioInfo.curMainVolume = 0;

	pSAF7741Info->FlyAudioInfo.preMainMute = TRUE;
	pSAF7741Info->FlyAudioInfo.usrBassFreq = pSAF7741Info->FlyAudioInfo.preBassFreq = 0;
	pSAF7741Info->FlyAudioInfo.usrBassLevel = pSAF7741Info->FlyAudioInfo.preBassLevel = BASS_LEVEL_COUNT/2;
	pSAF7741Info->FlyAudioInfo.usrMidFreq = pSAF7741Info->FlyAudioInfo.preMidFreq = 0;
	pSAF7741Info->FlyAudioInfo.usrMidLevel = pSAF7741Info->FlyAudioInfo.preMidLevel = MID_LEVEL_COUNT/2;
	pSAF7741Info->FlyAudioInfo.usrTrebleFreq = pSAF7741Info->FlyAudioInfo.preTrebleFreq = 0;
	pSAF7741Info->FlyAudioInfo.usrTrebleLevel = pSAF7741Info->FlyAudioInfo.preTrebleLevel = TREB_LEVEL_COUNT/2;
	pSAF7741Info->FlyAudioInfo.preLoudFreq = 0;
	pSAF7741Info->FlyAudioInfo.preLoudLevel = LOUDNESS_LEVEL_COUNT/2;
	pSAF7741Info->FlyAudioInfo.preBalance = BALANCE_LEVEL_COUNT/2;
	pSAF7741Info->FlyAudioInfo.preFader = FADER_LEVEL_COUNT/2;
	pSAF7741Info->FlyAudioInfo.preLoudnessOn = TRUE;
	pSAF7741Info->FlyAudioInfo.preSubOn = TRUE;
	pSAF7741Info->FlyAudioInfo.preSubFilter = 0;
	pSAF7741Info->FlyAudioInfo.preSubLevel = 5;

	memset(pSAF7741Info->FlyAudioInfo.preEQ,0,sizeof(pSAF7741Info->FlyAudioInfo.preEQ));
	memset(pSAF7741Info->FlyAudioInfo.curEQ,0,sizeof(pSAF7741Info->FlyAudioInfo.curEQ));

	memset(pSAF7741Info->GraphicalSpectrumAnalyzerValue,0,sizeof(pSAF7741Info->GraphicalSpectrumAnalyzerValue));

	pSAF7741Info->FlyAudioInfo.preGPSSpeaker = 0;

	pSAF7741Info->FlyAudioInfo.bEnableVolumeFader = FALSE;
}

VOID SAF7741ParaInit(P_FLY_SAF7741_INFO pSAF7741Info)
{
	pSAF7741Info->bPowerUp = FALSE;
	pSAF7741Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable = FALSE;//音频芯片替其它驱动控制静音开关

	IO_Control_Init(pSAF7741Info);
	FlyAudio_ParaInitDefault(pSAF7741Info);

	pSAF7741Info->buffToDriversProcessorStatus = 0;

	pSAF7741Info->buffToUserHx = 0;
	pSAF7741Info->buffToUserLx = 0;
	pSAF7741Info->buffToUserDataEvent = NULL;

	pSAF7741Info->SAF7741PowerSteps = 0;

	pSAF7741Info->SAF7741MainThreadHandle = NULL;
	pSAF7741Info->bKillDispatchSAF7741MainThread = TRUE;
	pSAF7741Info->hDispatchSAF7741MainThreadEvent = NULL;

	pSAF7741Info->SAF7741DelayThreadHandle = NULL;
	pSAF7741Info->bKillDispatchSAF7741DelayThread = TRUE;
	pSAF7741Info->hDispatchSAF7741DelayThreadEvent = NULL;

	/****************INIT FLYAUDIO AMPLIFIER******************/
	pSAF7741Info->bKillDispatchFlyMsgQueueExtAmpReadThread = TRUE;
	pSAF7741Info->hMsgQueueToExtAmpCreate = NULL;
	pSAF7741Info->hMsgQueueFromExtAmpCreate = NULL;
	pSAF7741Info->FLyMsgQueueExtAmpReadThreadHandle = NULL;

	pSAF7741Info->bKillDispatchExtAmpThread = TRUE;
	pSAF7741Info->bKillDispatchExtAmpThread = NULL;
	pSAF7741Info->hDispatchExtAmpThreadEvent = NULL;
	/********************************************************/


	memset(&pSAF7741Info->FlyAudioInfo,0x00,sizeof(_FLY_AUDIO_INFO));
}

VOID SAF7741Enable(P_FLY_SAF7741_INFO pSAF7741Info,BOOL bOn)
{
	DWORD dwThreadID;

	if(bOn)
	{
		if (pSAF7741Info->bPower)
		{
			return;
		}
		SAF7741ParaInit(pSAF7741Info);


		pSAF7741Info->bKillDispatchSAF7741MainThread = FALSE;
		pSAF7741Info->SAF7741MainThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)SAF7741MainThread, //线程的全局函数
			pSAF7741Info, //此处传入了主框架的句柄
			0, &dwThreadID );
		if (NULL == pSAF7741Info->SAF7741MainThreadHandle)
		{
			pSAF7741Info->bKillDispatchSAF7741MainThread = TRUE;
			return;
		}
		pSAF7741Info->hDispatchSAF7741MainThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741 start main thread!ID---->%x"),dwThreadID));

		pSAF7741Info->bKillDispatchSAF7741DelayThread = FALSE;
		pSAF7741Info->SAF7741DelayThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)SAF7741DelayThread, //线程的全局函数
			pSAF7741Info, //此处传入了主框架的句柄
			0, &dwThreadID );
		if (NULL == pSAF7741Info->SAF7741DelayThreadHandle)
		{
			pSAF7741Info->bKillDispatchSAF7741DelayThread = TRUE;
			return;
		}
		pSAF7741Info->hDispatchSAF7741DelayThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741 start delay thread!ID---->%x"),dwThreadID));

		RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741 create DATA_READ_HANDLE_NAME!")));
		pSAF7741Info->buffToUserDataEvent = CreateEvent(NULL,FALSE,FALSE,DATA_READ_HANDLE_NAME);

		pSAF7741Info->bPower = TRUE;
	}
	else
	{
		if (!pSAF7741Info->bPower)
		{
			return;
		}
		RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741 disable!")));

		pSAF7741Info->bKillDispatchSAF7741MainThread = TRUE;
		SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);

		pSAF7741Info->bKillDispatchSAF7741DelayThread = TRUE;
		SetEvent(pSAF7741Info->hDispatchSAF7741DelayThreadEvent);

		pSAF7741Info->bPower = FALSE;
	}
}


static void powerOnSpecialEnable(P_FLY_SAF7741_INFO pSAF7741Info,BOOL bOn)
{
	DWORD dwThreadID;

	if (bOn)
	{
		if (pSAF7741Info->bPowerSpecialUp)
		{
			return;
		}
		pSAF7741Info->bPowerSpecialUp = TRUE;

		pSAF7741Info->buffToUserDataEvent = CreateEvent(NULL,FALSE,FALSE,NULL);	
		InitializeCriticalSection(&pSAF7741Info->hCSSendToUser);

		/***********************Creat MSGS of  FLYAUDIO EXT AMPLIFIER***********************************/
		MSGQUEUEOPTIONS  msgOpts;

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_EXTAMP_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_EXTAMP_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pSAF7741Info->hMsgQueueToExtAmpCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_EXTAMP_NAME, &msgOpts);
		if (NULL == pSAF7741Info->hMsgQueueToExtAmpCreate)
		{
			DBGI((TEXT("\r\nFlyAudio SAF7741 Create MsgQueue To ExtAmp Fail!")));
		}

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_FROM_EXTAMP_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_FROM_EXTAMP_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pSAF7741Info->hMsgQueueFromExtAmpCreate = CreateMsgQueue(FLY_MSG_QUEUE_FROM_EXTAMP_NAME, &msgOpts);
		if (NULL == pSAF7741Info->hMsgQueueFromExtAmpCreate)
		{
			DBGI((TEXT("\r\nFlyAudio ExtAmp Create MsgQueue From ExtAmp Fail!")));
		}

		pSAF7741Info->bKillDispatchFlyMsgQueueExtAmpReadThread = FALSE;
		pSAF7741Info->FLyMsgQueueExtAmpReadThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)FlyAudioExtAmpReadThread, //线程的全局函数
			pSAF7741Info, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGI((TEXT("\r\nFlyAudio FlyAudioExtAmpReadThread ID---->%x"),dwThreadID));
			if (NULL == pSAF7741Info->FLyMsgQueueExtAmpReadThreadHandle)
			{
				pSAF7741Info->bKillDispatchFlyMsgQueueExtAmpReadThread = TRUE;
				return;
			}
			/********************************************************************************************/
	} 
	else
	{
		if (!pSAF7741Info->bPowerSpecialUp)
		{
			return;
		}
		pSAF7741Info->bPowerSpecialUp = FALSE;

		/*******Close MSGS of FlyAudio amplifier***********************/
		pSAF7741Info->bKillDispatchFlyMsgQueueExtAmpReadThread = TRUE;
		while (pSAF7741Info->FLyMsgQueueExtAmpReadThreadHandle)
		{
			SetEvent(pSAF7741Info->hMsgQueueToExtAmpCreate);
			Sleep(10);
		}
		CloseMsgQueue(pSAF7741Info->hMsgQueueToExtAmpCreate);
		CloseMsgQueue(pSAF7741Info->hMsgQueueFromExtAmpCreate);
		/****************************************************************/

		CloseHandle(pSAF7741Info->buffToUserDataEvent);
	}
}



void IpcRecv(UINT32 iEvent)
{
	P_FLY_SAF7741_INFO	pSAF7741Info = (P_FLY_SAF7741_INFO)gpSAF7741Info;

	DBGD((TEXT("\r\nFLY7741 Recv IPC iEvent:%d\r\n"),iEvent));

	if (iEvent == EVENT_GLOBAL_DATA_CHANGE_VOLUME)
	{
		SAF7741_SendVolumeToUser(pSAF7741Info);
	}

	SetEvent(pSAF7741Info->hDispatchSAF7741MainThreadEvent);
}
void SockRecv(BYTE *buf, UINT16 len)
{
	UINT16 i=0;
	P_FLY_SAF7741_INFO	pSAF7741Info = (P_FLY_SAF7741_INFO)gpSAF7741Info;

	DBGD((TEXT("\r\n FLY SAF7741 socket recv %d bytes: "),len-1));
	for (i=0; i<len-1; i++)
		DBGD((TEXT("%02X "), buf[i]));

	switch (buf[0])
	{
	case 0x01://与audio.dll通信成功，设置默认的值
		SAF7741SetAudioDefaultValue(pSAF7741Info);
		break;

	default:
		break;
	}


	if (!pSAF7741Info->bSocketConnecting)
		pSAF7741Info->bSocketConnecting  = TRUE;
}


HANDLE
FAU_Init(DWORD dwContext)

{
	P_FLY_SAF7741_INFO pSAF7741Info;

	pSAF7741Info = (P_FLY_SAF7741_INFO)LocalAlloc(LPTR, sizeof(FLY_SAF7741_INFO));
	if (!pSAF7741Info)
	{
		return NULL;
	}


	gpSAF7741Info = pSAF7741Info;

	pSAF7741Info->pFlyShmGlobalInfo = CreateShm(AUDIO_MODULE,IpcRecv);
	if (NULL == pSAF7741Info->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio SAF7741 Init create shm err\r\n")));
		return NULL;
	}

	if (!GetDllAddrTable())
	{
		DBGE((TEXT("FlyAudio SAF7741  GetDllAddrTable err\r\n")));
		return NULL;
	}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio SAF7741 Open create user buf err\r\n")));
		return NULL;
	}
	if(!CreateServerSocket(SockRecv, TCP_PORT_AUDIO))
	{
		DBGE((TEXT("FlyAudio SAF7741 Open create server socket err\r\n")));
		return NULL;
	}

	//参数初始化
	SAF7741ParaInit(pSAF7741Info);
	pSAF7741Info->bOpen = FALSE;
	pSAF7741Info->bPower = FALSE;
	pSAF7741Info->bNeedInit = FALSE;
	//powerOnSpecialEnable(pSAF7741Info,TRUE);
	SAF7741Enable(pSAF7741Info,TRUE);

	control7386Mute(pSAF7741Info,FALSE);

	return (HANDLE)pSAF7741Info;
}

BOOL
FAU_Deinit(DWORD hDeviceContext)
{
	P_FLY_SAF7741_INFO	pSAF7741Info = (P_FLY_SAF7741_INFO)hDeviceContext;
	RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741 DeInit")));
	CloseHandle(pSAF7741Info->buffToUserDataEvent);
	LocalFree(pSAF7741Info);
	return TRUE;
}

DWORD
FAU_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	P_FLY_SAF7741_INFO	pSAF7741Info = (P_FLY_SAF7741_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;

	RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741 Open")));

	if(pSAF7741Info->bOpen)
	{
		returnWhat = NULL;
	}
	else
	{
		pSAF7741Info->bOpen = TRUE;
	}
	SAF7741Enable(pSAF7741Info,TRUE);


	return returnWhat;
}

BOOL
FAU_Close(DWORD hDeviceContext)
{
	P_FLY_SAF7741_INFO	pSAF7741Info = (P_FLY_SAF7741_INFO)hDeviceContext;

	RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741 Close")));


	pSAF7741Info->bOpen = FALSE;
	SAF7741Enable(pSAF7741Info,FALSE);

	return TRUE;
}

VOID
FAU_PowerUp(DWORD hDeviceContext)
{
	P_FLY_SAF7741_INFO pSAF7741Info = (P_FLY_SAF7741_INFO)hDeviceContext;

}

VOID
FAU_PowerDown(DWORD hDeviceContext)
{
	P_FLY_SAF7741_INFO pSAF7741Info = (P_FLY_SAF7741_INFO)hDeviceContext;

	SAF7741Enable(pSAF7741Info,FALSE);
}

DWORD
FAU_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	P_FLY_SAF7741_INFO	pSAF7741Info = (P_FLY_SAF7741_INFO)hOpenContext;
	UINT dwRead = 0,i;

	BYTE *buf = (BYTE *)pBuffer;
	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio SAF7741 return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}

	return dwRead;
}

DWORD
FAU_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	P_FLY_SAF7741_INFO	pSAF7741Info = (P_FLY_SAF7741_INFO)hOpenContext;
	RETAILMSG(1, (TEXT("\r\nFlyAudio SAF7741 Write-->%d "),
					NumberOfBytes));
	for(UINT i = 0;i < NumberOfBytes;i++)
	{
		RETAILMSG(1, (TEXT(" %X "),
			*((BYTE *)pSourceBytes + i)));
	}
	if(NumberOfBytes)
	{
		//SAF7741CommandProcessor(pSAF7741Info,(BYTE *)pSourceBytes,NumberOfBytes);
		SAF7741DealWinceCmd(pSAF7741Info,(((BYTE *)pSourceBytes)+3),((BYTE *)pSourceBytes)[2] - 1);
	}
	return NULL;
}

DWORD
FAU_Seek(DWORD hOpenContext, LONG Amount, DWORD  Type)
{
	return NULL;
}

BOOL
FAU_IOControl(DWORD hOpenContext,
			  DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn,
			  PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
	P_FLY_SAF7741_INFO	pSAF7741Info = (P_FLY_SAF7741_INFO)hOpenContext;
	BOOL bRes = TRUE;

	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	DBGD((TEXT("\r\nFlyAudio SAF7741 IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)"),
	//		hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	DBGD((TEXT("\r\nFlyAudio SAF7741 Set IOCTL_SERIAL_SET_WAIT_MASK")));
	//		break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	DBGD((TEXT("\r\nFlyAudio SAF7741 Set IOCTL_SERIAL_WAIT_ON_MASK Enter")));
	//		if (pSAF7741Info->bOpen)
	//		{
	//			WaitForSingleObject(pSAF7741Info->buffToUserDataEvent,INFINITE);
	//		} 
	//		else
	//		{
	//			WaitForSingleObject(pSAF7741Info->buffToUserDataEvent,0);
	//		}
	//		if ((dwLenOut < sizeof(DWORD)) || (NULL == pBufOut) ||
	//			(NULL == pdwActualOut))
	//		{
	//			bRes = FALSE;
	//			break;
	//		}
	//		*(DWORD *)pBufOut = EV_RXCHAR;
	//		*pdwActualOut = sizeof(DWORD);
	//		DBGD((TEXT("\r\nFlyAudio SAF7741 Set IOCTL_SERIAL_WAIT_ON_MASK Exit")));
	//			break;
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
		DEBUGMSG(1, (TEXT("Attach in SAF7741 DllEntry\n")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}

	if (dwReason == DLL_PROCESS_DETACH)
	{
		DEBUGMSG(1, (TEXT("Dettach in SAF7741 DllEntry\n")));
	}

	DEBUGMSG(1, (TEXT("Leaving SAF7741 DllEntry\n")));

	return (TRUE);
}
