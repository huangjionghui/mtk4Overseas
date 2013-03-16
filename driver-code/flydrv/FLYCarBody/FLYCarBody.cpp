// -----------------------------------------------------------------------------
// File Name    : FLYCarbody.cpp
// Title        : FLYCarbody Driver
// Author       : JQilin - Copyright (C) 2011
// Created      : 2011-02-25  
// Version      : 0.01
// Target MCU   : WinCE Driver
// -----------------------------------------------------------------------------
// Version History:
/*
>>>2011-05-18: V0.02 整理了WinCE待机相关的代码
>>>2011-02-25: V0.01 first draft
*/


#define GLOBAL_COMM	1

#include "FLYCarBody.h"
#include "flyseriallib.h"
#include "flysocketlib.h"

P_FLY_CARBODY_INFO gpCarBodyInfo=NULL;

typedef DWORD (*PFN_SetSystemPowerState)(LPCWSTR, DWORD, DWORD);

extern void PowerOffSystem(void);

static BOOL CarAmpWriteFile(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p,UINT length);


static void systemWriteFile(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p,UINT length);
static BOOL extAmpWriteFile(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p,UINT length);
static void FaiiiWriteFile(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p,UINT length);
static void AcWriteFile(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p,UINT length);
static void SendSocketData(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p, UINT16 length);
void MCU_IIC_Isr_Enable(P_FLY_CARBODY_INFO pCarBodyInfo,BOOL bEnable);



static BOOL IsrPinRead(P_FLY_CARBODY_INFO pCarBodyInfo)
{
	return 0;
	//return SOC_IO_Input(IIC_ISR_PIN_G,IIC_ISR_PIN_I);
}

//static BYTE I2WriteSeq_MCU[] = {
//SEG_MASK_SEND_START | I2C_SEQ_SEG_DEVADDR_W | SEG_MASK_NEED_ACK,//写从设备地址
//I2C_SEQ_SEG_DATA | SEG_MASK_NEED_ACK | SEG_MASK_SEND_STOP
//};
//
//static BYTE I2CReadSeq_MCU[] = {
//SEG_MASK_SEND_START | I2C_SEQ_SEG_DEVADDR_R | SEG_MASK_NEED_ACK,//发读命令
//I2C_SEQ_SEG_DATA    | SEG_MASK_NEED_NACK | SEG_MASK_SEND_STOP //读数据,要逆序
//};

//const GUID DEVICE_IFC_I2C_GUID;

//static BOOL I2C_Write_MCU(P_FLY_CARBODY_INFO pCarBodyInfo, HANDLE pI2CHandle, ULONG ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
//{
//	//I2CTRANS  Trans;
//	//Trans.bRead = FALSE;
//	//Trans.dwRegAddr = ulRegAddr;
//	//Trans.pValueBuf = pRegValBuf;
//	//Trans.uiValueLen = uiValBufLen;
//	//Trans.dwClock = MCU_I2C_FREQ;
//	//Trans.i2cdevinfo.pReadSeq = (PBYTE)I2CReadSeq_MCU;
//	//Trans.i2cdevinfo.uiReadSeqLen = sizeof(I2CReadSeq_MCU);
//	//Trans.i2cdevinfo.pWriteSeq = (PBYTE)I2WriteSeq_MCU;
//	//Trans.i2cdevinfo.uiWriteSeqLen = sizeof(I2WriteSeq_MCU);
//	//Trans.i2cdevinfo.ulDevAddrW = MCU_ADDR_W;
//	//Trans.i2cdevinfo.ulDevAddrR = MCU_ADDR_R;
//
//	//if (!I2CTransact(pI2CHandle, &Trans))
//	//{
//	//	pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnMCU;
//	//	DBGE((TEXT("\r\nFlyAudio MCU:I2C write failed %X"),ulRegAddr));
//	//	return FALSE;
//	//}
//
//	return TRUE;
//}
//
//static BOOL I2C_Read_MCU(P_FLY_CARBODY_INFO pCarBodyInfo, HANDLE pI2CHandle, ULONG ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen)
//{
//	//I2CTRANS  Trans;
//	//Trans.bRead = TRUE;
//	//Trans.dwRegAddr = ulRegAddr;
//	//Trans.pValueBuf = pRegValBuf;
//	//Trans.uiValueLen = uiValBufLen;
//	//Trans.dwClock = MCU_I2C_FREQ;
//	//Trans.i2cdevinfo.pReadSeq = (PBYTE)I2CReadSeq_MCU;
//	//Trans.i2cdevinfo.uiReadSeqLen = sizeof(I2CReadSeq_MCU);
//	//Trans.i2cdevinfo.pWriteSeq = (PBYTE)I2WriteSeq_MCU;
//	//Trans.i2cdevinfo.uiWriteSeqLen = sizeof(I2WriteSeq_MCU);
//	//Trans.i2cdevinfo.ulDevAddrW = MCU_ADDR_W;
//	//Trans.i2cdevinfo.ulDevAddrR = MCU_ADDR_R;
//
//	//if (!I2CTransact(pI2CHandle, &Trans))
//	//{
//	//	pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnMCU;
//	//	DBGE((TEXT("\r\nFlyAudio MCU:I2C read failed %X"),ulRegAddr));
//	//	return FALSE;
//	//}
//
//	return TRUE;
//}

void setWinCEPowerOff(P_FLY_CARBODY_INFO pCarBodyInfo)
{
	//SetSystemPowerState(NULL, POWER_STATE_SUSPEND, POWER_FORCE);
	//return;

	//HMODULE hmCore;
	//PFN_SetSystemPowerState gpfnSetSystemPowerState	= NULL;

	//hmCore = (HMODULE) LoadLibrary(_T("coredll.dll"));

	//if (hmCore == NULL)
	//	return;

	//gpfnSetSystemPowerState = (PFN_SetSystemPowerState) GetProcAddress(hmCore, _T("SetSystemPowerState"));

	//if (gpfnSetSystemPowerState != NULL)
	//{
	//	gpfnSetSystemPowerState(NULL, POWER_STATE_SUSPEND, POWER_FORCE);
	//}
	//else
	//{
	//	PowerOffSystem();
	//}
}

void actualwriteToMCU(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p,UINT length)
{

	//LPC升级时不处理
	if (pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLpcUpdate 
		&& length > 5 && p[3] != 0x37)
		return;

	if (length)
	{
		DBGI((TEXT("\r\nCarbody  Write %d bytes to serial:"),length));
		for (UINT i=0; i<length; i++)
		{
			DBGI((TEXT(" %02X"),p[i]));
		}

		WriteSerial(pCarBodyInfo->hFlyCarbodySerial,p,length);
	}

	if (pCarBodyInfo->bHaveRecPowerOffOrReset)
	{
		pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNoMoreToSendDataWhenToSleep = TRUE;
		Sleep(100);
		setWinCEPowerOff(pCarBodyInfo);
	}
}

static void controlLpcUpdate(P_FLY_CARBODY_INFO pCarBodyInfo,BOOL bUpdate)
{
	BYTE buff[3]={0x37,0x90,0x00};
	if (bUpdate)
		buff[2] = 0x01;

	SocketWrite(buff,3);

	UINT i=0;
	DBGD((TEXT("\r\n FLY CarBody write socket %d bytes:"),3));
	for (i=0; i<3; i++)
		DBGD((TEXT("%02X "), buff[i]));
}
void writeToMCU(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p,UINT length)
{
	BYTE buff[256];
	BYTE crc;
	UINT i;
	if (0x00 == p[0] && 0x02 == p[1])
	{
	}
	else
	{
		DBGD((TEXT("\r\nCarBody WriteTo MCU:")));
		for (i = 0;i < length;i++)
		{
			DBGD((TEXT(" %x"),p[i]));
		}
	}

	if (pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNoMoreToSendDataWhenToSleep)//关机后不发数据
	{
		DBGE((TEXT("\r\nCarBody bNoMoreToSendDataWhenToSleep")));
		return;
	}

	if (0x00 == p[0])
	{
		if (0x01 == p[1])//关机
		{
			pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNoMoreToSendDataWhenToSleep = TRUE;
			MCU_IIC_Isr_Enable(pCarBodyInfo,FALSE);
		}
		else if (0x03 == p[1] && 0x04 == p[2])//Reset
		{
			pCarBodyInfo->bHaveRecPowerOffOrReset = TRUE;
		}
		else if (0x18 == p[1])//Factory
		{
			pCarBodyInfo->bHaveRecPowerOffOrReset = TRUE;
		}
		else if (0x99 == p[1])//Ultra Low Power
		{
			pCarBodyInfo->bHaveRecPowerOffOrReset = TRUE;
		}
	}

	buff[0] = 0xFF;
	buff[1] = 0x55;
	buff[2] = length + 1;
	crc = length + 1;
	for (i = 0;i < length;i++)
	{
		buff[3+i] = p[i];
		crc += p[i];
	}
	buff[3+i] = crc;
	actualwriteToMCU(pCarBodyInfo,buff,length+4);
}

static void controlToMCUPing(P_FLY_CARBODY_INFO pCarBodyInfo,BOOL bWork)
{
	BYTE buff[] = {0x00,0x02,0x00};

	if (bWork)
	{
		buff[2] = 0x00;
	}
	else
	{
		buff[2] = 0x01;
	}

	writeToMCU(pCarBodyInfo,buff,3);
}

static void dealReadFromMCUAll(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p,UINT length)
{
	UINT i;

	DBGD((TEXT("\r\nCarbody Read From MCU commd:")));
	for (i = 0;i < length;i++)
	{
		DBGD((TEXT("%02X "),p[i]));
	}

	switch (p[0])
	{
	case 0x00://System
		switch (p[1])
		{
		case 0x90:
			if (pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.bInit)
			{
				if (FALSE == WriteMsgQueue(pCarBodyInfo->hMsgQueueToRadioCreate, &p[2], length - 2, 0, 0))
				{
					DWORD dwError = GetLastError();

					DBGE((TEXT("\r\nCarbody Write MsgQueue To Radio Error!0x%x"),dwError));
				}
			}
			else
			{
				DBGE((TEXT("\r\nCarbody Write MsgQueue To Radio Not Init")));
			}
			break;
		case 0x92:
			if (FALSE == WriteMsgQueue(pCarBodyInfo->hMsgQueueToGlobalCreate,&p[2], length-2, 0, 0))
			{
				DWORD dwError = GetLastError();

				DBGE((TEXT("\r\nCarbody Write MsgQueue To Global Error!0x%x"),dwError));
			}
			break;
		case 0x93:
			pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRadioAD = p[2]*256 + p[3];
			pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRadioADReturn = TRUE;
			break;
		default:
			systemWriteFile(pCarBodyInfo,&p[1],length-1);
			if (0x00 == p[1] || 0x01 == p[1])
			{
				RETAILMSG(1, (TEXT("\r\nCarbody Read MCU MsgQueue ACC ID %d!!!!!!!!"),p[1]));
			}
			break;
		}
		break;


	case 0x01:// amplifier
		if (!extAmpWriteFile(pCarBodyInfo,&p[1],length-1))
		{
			if (0x89 == p[1] && 0xFA == p[2])
			{
				pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveFlyAudioExtAMP = TRUE;
			}
		}
		break;	


	case 0x02:
		if (pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.bInit)
		{
			if (FALSE == WriteMsgQueue(pCarBodyInfo->hMsgQueueToKeyCreate,&p[2], 1, 0, 0))
			{
				DWORD dwError = GetLastError();

				DBGE((TEXT("\r\nCarbody Write MsgQueue To Key Error!0x%x"),dwError));
			}
		}
		else
		{
			DBGE((TEXT("\r\nCarbody Write MsgQueue To Key Not Init")));
		}
		break;

	case 0x03://小屏
		break;

	case 0x04://AC
		p[0] = length;
		AcWriteFile(pCarBodyInfo,&p[0],length);
		break;

	case 0x05:
		CarAmpWriteFile(pCarBodyInfo,&p[1],length-1);
		break;

	case 0x80://faiii
		p[0] = length;
		FaiiiWriteFile(pCarBodyInfo,&p[0],length);
		break;

	case 0x37://LPC 升级
		SendSocketData(pCarBodyInfo,p,length);
		break;

	default:
		break;
	}
}

static BOOL readFromMCUProcessor(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p,UINT length)
{
	UINT i;

	for (i = 0;i < length;i++)
	{
		switch (pCarBodyInfo->buffFromMCUProcessorStatus)
		{
		case 0:
			if (0xFF == *p)
			{
				pCarBodyInfo->buffFromMCUProcessorStatus = 1;
			}
			break;
		case 1:
			if (0xFF == *p)
			{
				pCarBodyInfo->buffFromMCUProcessorStatus = 1;
			}
			else if (0x55 == *p)
			{
				pCarBodyInfo->buffFromMCUProcessorStatus = 2;
			}
			else
			{
				pCarBodyInfo->buffFromMCUProcessorStatus = 0;
			}
			break;
		case 2:
			pCarBodyInfo->buffFromMCUProcessorStatus = 3;
			pCarBodyInfo->buffFromMCUFrameLength = 0;
			pCarBodyInfo->buffFromMCUFrameLengthMax = *p;
			pCarBodyInfo->buffFromMCUCRC = *p;
			break;
		case 3:
			if (pCarBodyInfo->buffFromMCUFrameLength < (pCarBodyInfo->buffFromMCUFrameLengthMax - 1))
			{
				pCarBodyInfo->buffFromMCU[pCarBodyInfo->buffFromMCUFrameLength] = *p;
				pCarBodyInfo->buffFromMCUCRC += *p;
				pCarBodyInfo->buffFromMCUFrameLength++;
			}
			else
			{
				pCarBodyInfo->buffFromMCUProcessorStatus = 0;
				if (pCarBodyInfo->buffFromMCUCRC == *p)
				{
					dealReadFromMCUAll(pCarBodyInfo,pCarBodyInfo->buffFromMCU,pCarBodyInfo->buffFromMCUFrameLengthMax-1);
				}
				else
				{
					DBGE((TEXT("\r\nCarbody Read From MCU CRC Error")));
				}
			}
			break;
		default:
			pCarBodyInfo->buffFromMCUProcessorStatus = 0;
			break;
		}
		p++;
	}

	if (pCarBodyInfo->buffFromMCUProcessorStatus > 1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL actualReadFromMCU(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p,UINT length)
{


	return TRUE;
}

static void systemWriteFile(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p,UINT length)
{
	UINT i;
	if (pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.bInit)
	{
		if (FALSE == WriteMsgQueue(pCarBodyInfo->hMsgQueueToSystemCreate, p, length, 0, 0))
		{
			DWORD dwError = GetLastError();
			DBGE((TEXT("\r\nCarbody Write MsgQueue To System Error!0x%x"),dwError));
		}
	}
	else
	{
		DBGE((TEXT("\r\nCarbody Write MsgQueue To System Not Init")));
		for (i = 0;i < length;i++)
		{
			DBGE((TEXT(" %x"),p[i]));
		}
	}
} 
/******************************    FAIII      ******************************/
static void FaiiiWriteFile(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p,UINT length)
{
	UINT i;
	if (pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalFaiiiInfo.bInit)
	{
		if (FALSE == WriteMsgQueue(pCarBodyInfo->hMsgQueueToFaiiiCreate, p, length, 0, 0))
		{
			DWORD dwError = GetLastError();
			DBGE((TEXT("\r\nCarbody Write MsgQueue To Faiii Error!0x%x"),dwError));
		}
	}
	else
	{
		DBGE((TEXT("\r\nCarbody Write MsgQueue To Faiii Not Init")));
		for (i = 0;i < length;i++)
		{
			DBGE((TEXT(" %x"),p[i]));
		}
	}
}
/******************************    AC     ******************************/
static void AcWriteFile(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p,UINT length)
{
	UINT i;
	if (pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAcInfo.bInit)
	{
		if (FALSE == WriteMsgQueue(pCarBodyInfo->hMsgQueueToAcCreate, p, length, 0, 0))
		{
			DWORD dwError = GetLastError();
			DBGE((TEXT("\r\nCarbody Write MsgQueue To AC Error!0x%x"),dwError));
		}
	}
	else
	{
		DBGE((TEXT("\r\nCarbody Write MsgQueue To AC Not Init")));
		for (i = 0;i < length;i++)
		{
			DBGE((TEXT(" %x"),p[i]));
		}
	}
}
/****************************************************************/
/**	Write data to Flyaudio amplifier moudle					   **/
/***************************************************************/
static BOOL extAmpWriteFile(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p,UINT length)
{
	UINT i;
	if (pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAudioInfo.bInit)
	{
		if(FALSE == WriteMsgQueue(pCarBodyInfo->hMsgQueueToExtAmpCreate, p, length, 0, 0))
		{
			DWORD dwError = GetLastError();
			DBGE((TEXT("\r\nCarbody Write MsgQueue To ExtAmp Error!0x%x"),dwError));
			return FALSE;
		}
		return TRUE;
	}
	else
	{
		DBGE((TEXT("\r\nCarbody Write MsgQueue To ExtAmp Not Init")));
		for (i = 0;i < length;i++)
		{
			DBGE((TEXT(" %x"),p[i]));
		}
		return FALSE;
	}
}

/****************************************************************/
/**	Write data to CAR AMP moudle					   **/
/***************************************************************/
static BOOL CarAmpWriteFile(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p,UINT length)
{
	UINT i;
	if (pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAudioInfo.bInit)
	{
		if(FALSE == WriteMsgQueue(pCarBodyInfo->hMsgQueueToCarAmpCreate, p, length, 0, 0))
		{
			DWORD dwError = GetLastError();
			DBGE((TEXT("\r\nCarbody Write MsgQueue To CarAmp Error!0x%x"),dwError));
			return FALSE;
		}
		return TRUE;
	}
	else
	{
		DBGE((TEXT("\r\nCarbody Write MsgQueue To CarAmp Not Init")));
		for (i = 0;i < length;i++)
		{
			DBGE((TEXT(" %x"),p[i]));
		}
		return FALSE;
	}
}

static void SendSocketData(P_FLY_CARBODY_INFO pCarBodyInfo,BYTE *p, UINT16 length)
{
	if (length > 0)
		SocketWrite(p, length);
}


/*******************************************************/
/**	FFAIII    **/
/******************************************************/
static DWORD WINAPI ThreadFaiiiRead(LPVOID pContext)
{
	P_FLY_CARBODY_INFO pCarBodyInfo = (P_FLY_CARBODY_INFO)pContext;	
	ULONG WaitReturn;
	BYTE buff[FLY_MSG_QUEUE_FROM_FAIII_LENGTH];
	DWORD dwRes;
	DWORD dwMsgFlag;
	UINT i;

	while (!pCarBodyInfo->bKillDispatchFlyMsgQueueFaiiiReadThread)
	{
		WaitReturn = WaitForSingleObject(pCarBodyInfo->hMsgQueueFromFaiiiCreate,INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nCarbody ThreadRead Event hMsgQueueFromFaiiiCreate Error")));
		}
		ReadMsgQueue(pCarBodyInfo->hMsgQueueFromFaiiiCreate, buff, FLY_MSG_QUEUE_FROM_FAIII_LENGTH, &dwRes, 0, &dwMsgFlag);
		if(dwRes)
		{
			DBGD((TEXT("\r\nCarbody Read Faiii MsgQueue length:%d"),dwRes));
			for (i = 0;i < dwRes;i++)
			{
				DBGD((TEXT(" %x"),buff[i]));
			}

			writeToMCU(pCarBodyInfo,buff,dwRes);
		}

	}
	pCarBodyInfo->FLyMsgQueueFaiiiReadThreadHandle = NULL;
	DBGE((TEXT("\r\nCarbody ThreadExtAmpRead exit")));
		return 0;
}

/*******************************************************/
/**	AC    **/
/******************************************************/
static DWORD WINAPI ThreadAcRead(LPVOID pContext)
{
	P_FLY_CARBODY_INFO pCarBodyInfo = (P_FLY_CARBODY_INFO)pContext;	
	ULONG WaitReturn;
	BYTE buff[FLY_MSG_QUEUE_FROM_AC_LENGTH];
	DWORD dwRes;
	DWORD dwMsgFlag;
	UINT i;

	while (!pCarBodyInfo->bKillDispatchFlyMsgQueueAcReadThread)
	{
		WaitReturn = WaitForSingleObject(pCarBodyInfo->hMsgQueueFromAcCreate,INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nCarbody ThreadRead Event hMsgQueueFromAcCreate Error")));
		}
		ReadMsgQueue(pCarBodyInfo->hMsgQueueFromAcCreate, buff, FLY_MSG_QUEUE_FROM_AC_LENGTH, &dwRes, 0, &dwMsgFlag);
		if(dwRes)
		{
			DBGD((TEXT("\r\nCarbody Read AC MsgQueue length:%d"),dwRes));
				for (i = 0;i < dwRes;i++)
				{
					DBGD((TEXT(" %x"),buff[i]));
				}

				writeToMCU(pCarBodyInfo,buff,dwRes);
		}

	}
	pCarBodyInfo->FLyMsgQueueAcReadThreadHandle = NULL;
	DBGE((TEXT("\r\nCarbody ThreadExtAmpRead exit")));
		return 0;
}

/*******************************************************/
/**	READ DATA FROM FLYAUDIO EXT AMPLIFIER MODULE     **/
/******************************************************/
static DWORD WINAPI ThreadExtAmpRead(LPVOID pContext)
{
	P_FLY_CARBODY_INFO pCarBodyInfo = (P_FLY_CARBODY_INFO)pContext;	
	ULONG WaitReturn;
	BYTE buff[FLY_MSG_QUEUE_FROM_EXTAMP_LENGTH];
	DWORD dwRes;
	DWORD dwMsgFlag;
	UINT i;

	while (!pCarBodyInfo->bKillDispatchFlyMsgQueueExtAmpReadThread)
	{
		WaitReturn = WaitForSingleObject(pCarBodyInfo->hMsgQueueFromExtAmpCreate,INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nCarbody ThreadRead Event hMsgQueueFromExtAmpCreate Error")));
		}
		ReadMsgQueue(pCarBodyInfo->hMsgQueueFromExtAmpCreate, buff, FLY_MSG_QUEUE_FROM_EXTAMP_LENGTH, &dwRes, 0, &dwMsgFlag);
		if(dwRes)
		{
			DBGD((TEXT("\r\nCarbody Read ExtAmp MsgQueue length:%d"),dwRes));
				for (i = 0;i < dwRes;i++)
				{
					DBGD((TEXT(" %x"),buff[i]));
				}

				writeToMCU(pCarBodyInfo,buff,dwRes);
		}

	}
	pCarBodyInfo->FLyMsgQueueExtAmpReadThreadHandle = NULL;
	DBGE((TEXT("\r\nCarbody ThreadExtAmpRead exit")));
		return 0;
}

/*******************************************************/
/**	READ DATA FROM CAR AMPLIFIER MODULE     **/
/******************************************************/
static DWORD WINAPI ThreadCarAmpRead(LPVOID pContext)
{
	P_FLY_CARBODY_INFO pCarBodyInfo = (P_FLY_CARBODY_INFO)pContext;	
	ULONG WaitReturn;
	BYTE buff[FLY_MSG_QUEUE_FROM_CARAMP_LENGTH];
	DWORD dwRes;
	DWORD dwMsgFlag;
	UINT i;

	while (!pCarBodyInfo->bKillDispatchFlyMsgQueueCarAmpReadThread)
	{
		WaitReturn = WaitForSingleObject(pCarBodyInfo->hMsgQueueFromCarAmpCreate,INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nCarbody ThreadRead Event hMsgQueueFromCarAmpCreate Error")));
		}
		ReadMsgQueue(pCarBodyInfo->hMsgQueueFromCarAmpCreate, buff, FLY_MSG_QUEUE_FROM_CARAMP_LENGTH, &dwRes, 0, &dwMsgFlag);
		if(dwRes)
		{
			DBGD((TEXT("\r\nCarbody Read Car Amp MsgQueue length:%d"),dwRes));
				for (i = 0;i < dwRes;i++)
				{
					DBGD((TEXT(" %x"),buff[i]));
				}

				writeToMCU(pCarBodyInfo,buff,dwRes);
		}

	}
	pCarBodyInfo->FLyMsgQueueCarAmpReadThreadHandle = NULL;
	DBGE((TEXT("\r\nCarbody Thread CarAmp Read exit")));
		return 0;
}
//key
static DWORD WINAPI ThreadKeyRead(LPVOID pContext)
{
	P_FLY_CARBODY_INFO pCarBodyInfo = (P_FLY_CARBODY_INFO)pContext;
	ULONG WaitReturn;
	BYTE buff[FLY_MSG_QUEUE_FROM_KEY_LENGTH];
	BYTE sendBuf[3]={0x02,0x00,0x00};
	DWORD dwRes;
	DWORD dwMsgFlag;

	while (!pCarBodyInfo->bKillDispatchFlyMsgQueueKeyReadThread)
	{
		WaitReturn = WaitForSingleObject(pCarBodyInfo->hMsgQueueFromKeyCreate, INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nCarbody ThreadRead Event hMsgQueueFromKeyCreate Error")));
		}
		ReadMsgQueue(pCarBodyInfo->hMsgQueueFromKeyCreate, buff, FLY_MSG_QUEUE_FROM_KEY_LENGTH, &dwRes, 0, &dwMsgFlag);
		if(dwRes)
		{
			DBGD((TEXT(" %x"),buff[0]));
			sendBuf[2]= buff[0];
			writeToMCU(pCarBodyInfo,sendBuf,3);
		}
	}
	pCarBodyInfo->FLyMsgQueueKeyReadThreadHandle = NULL;
	DBGE((TEXT("\r\nCarbody ThreadKeyRead exit")));
		return 0;
}


static DWORD WINAPI ThreadSystemRead(LPVOID pContext)
{
	P_FLY_CARBODY_INFO pCarBodyInfo = (P_FLY_CARBODY_INFO)pContext;
	ULONG WaitReturn;
	BYTE buff[FLY_MSG_QUEUE_FROM_SYSTEM_LENGTH];
	DWORD dwRes;
	DWORD dwMsgFlag;
	UINT i;

	while (!pCarBodyInfo->bKillDispatchFlyMsgQueueSystemReadThread)
	{
		WaitReturn = WaitForSingleObject(pCarBodyInfo->hMsgQueueFromSystemCreate, INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nCarbody ThreadRead Event hMsgQueueFromSystemCreate Error")));
		}
		ReadMsgQueue(pCarBodyInfo->hMsgQueueFromSystemCreate, buff, FLY_MSG_QUEUE_FROM_SYSTEM_LENGTH, &dwRes, 0, &dwMsgFlag);
		if(dwRes)
		{
			DBGD((TEXT("\r\nCarbody Read System MsgQueue length:%d"),dwRes));
			for (i = 0;i < dwRes;i++)
			{
				DBGD((TEXT(" %x"),buff[i]));
			}

			writeToMCU(pCarBodyInfo,buff,dwRes);
		}
	}
	pCarBodyInfo->FLyMsgQueueSystemReadThreadHandle = NULL;
	DBGE((TEXT("\r\nCarbody ThreadSystemRead exit")));
	return 0;
}

static DWORD WINAPI ThreadAssistDisplayRead(LPVOID pContext)
{
	P_FLY_CARBODY_INFO pCarBodyInfo = (P_FLY_CARBODY_INFO)pContext;
	ULONG WaitReturn;
	BYTE buff[1+FLY_MSG_QUEUE_FROM_ASSISTDISPLAY_LENGTH];
	DWORD dwRes;
	DWORD dwMsgFlag;

	buff[0] = 0x03;//按照协议更改

	while (!pCarBodyInfo->bKillDispatchFlyMsgQueueAssistDisplayReadThread)
	{
		WaitReturn = WaitForSingleObject(pCarBodyInfo->hMsgQueueFromAssistDisplayCreate, INFINITE);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nCarbody ThreadRead Event hMsgQueueFromAssistDisplayCreate Error")));
		}
		ReadMsgQueue(pCarBodyInfo->hMsgQueueFromAssistDisplayCreate, &buff[1], FLY_MSG_QUEUE_FROM_ASSISTDISPLAY_LENGTH, &dwRes, 0, &dwMsgFlag);
		if(dwRes)
		{
			DBGD((TEXT("\r\nFlyAudio Carbody Read AssistDisplay MsgQueue%d %d\r\n"),dwRes,buff[1]));
			writeToMCU(pCarBodyInfo,buff,dwRes+1);
		}
	}
	DBGE((TEXT("\r\nCarbody ThreadAssistDisplayRead exit")));
	pCarBodyInfo->FLyMsgQueueAssistDisplayReadThreadHandle = NULL;
	return 0;
}

static DWORD WINAPI ThreadCarbodyComm(LPVOID pContext)
{
	P_FLY_CARBODY_INFO pCarBodyInfo = (P_FLY_CARBODY_INFO)pContext;
	BYTE readBuff[DATA_BUFF_LENGTH_FROM_MCU];
	UINT16 ret=0;

	DBGE((TEXT("\r\nCarbody ThreadAssistDisplayRead Start")));

	while (!pCarBodyInfo->bKillDispatchFlyCarbodyCommThread)
	{
		if (!pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNoMoreToSendDataWhenToSleep)
		{

			//actualReadFromMCU(pCarBodyInfo,readBuff,DATA_BUFF_LENGTH_FROM_MCU);

			ret = ReadSerial(pCarBodyInfo->hFlyCarbodySerial,readBuff,DATA_BUFF_LENGTH_FROM_MCU);
			if (ret > 0)
			{	
				DBGI((TEXT("\r\nCarbody Serial Read %d bytes:"),ret));
				for (UINT i = 0;i < ret;i++)
				{
					DBGI((TEXT(" %x"),readBuff[i]));
				}

				readFromMCUProcessor(pCarBodyInfo,readBuff,ret);
			}
		}
	}
	pCarBodyInfo->hThreadHandleFlyCarbodyComm = NULL;
	DBGE((TEXT("\r\nCarBody ThreadCarbodyComm Exit")));
	return 0;
}

static DWORD WINAPI ThreadMain(LPVOID pContext)
{
	P_FLY_CARBODY_INFO pCarBodyInfo = (P_FLY_CARBODY_INFO)pContext;
	ULONG WaitReturn;

	ULONG timeInter = 0;


	while (!pCarBodyInfo->bKillDispatchFlyMainThread)
	{
		WaitReturn = WaitForSingleObject(pCarBodyInfo->hDispatchMainThreadEvent, 618);
		if (WAIT_FAILED == WaitReturn)
		{
			DBGE((TEXT("\r\nCarBody hDispatchMainThreadEvent Error")));
		}

		if (!pCarBodyInfo->xxxxxxxxStarting)
		{
			if (pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.bInit)
			{
				pCarBodyInfo->xxxxxxxxStarting = TRUE;
				controlToMCUPing(pCarBodyInfo,FALSE);
				IpcStartEvent(EVENT_GLOBAL_POWERON_MCU_INIT_ID);
			}
		}
		else if (GetTickCount() - timeInter > 3000)
		{
			timeInter = GetTickCount();
			controlToMCUPing(pCarBodyInfo,TRUE);
		}




		if (IpcWhatEvent(EVENT_GLOBAL_LPC_UPDATA_FLAG))
		{
			IpcClearEvent(EVENT_GLOBAL_LPC_UPDATA_FLAG);
			
			//准备好了，可以升级 
			if (pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLpcUpdateFlag)
			{
				controlLpcUpdate(pCarBodyInfo,TRUE);
			}
			else 
			{
				controlLpcUpdate(pCarBodyInfo,FALSE);
			}
		}
	}

	while (pCarBodyInfo->hThreadHandleFlyCarbodyComm)
	{
		DBGE((TEXT("\r\nWait hThreadHandleFlyCarbodyComm")));
		SetEvent(pCarBodyInfo->hThreadHandleFlyCarbodyCommEvent);
		Sleep(10);
	}
	CloseHandle(pCarBodyInfo->hThreadHandleFlyCarbodyCommEvent);

	CloseHandle(pCarBodyInfo->hDispatchMainThreadEvent);
	DBGE((TEXT("\r\nCarBody ThreadMain exit")));
	pCarBodyInfo->FlyMainThreadHandle = NULL;
	return 0;
}

DWORD WINAPI IsrPin_io_isr(LPVOID lpvParam)
{
	P_FLY_CARBODY_INFO pCarBodyInfo = (P_FLY_CARBODY_INFO)lpvParam;

	DBGE((TEXT("\r\nIsrPin_io_isr Start")));
	pCarBodyInfo->bInterruptIsrPinProcRunning = TRUE;

	while (pCarBodyInfo->hInterruptEventIsrPin)
	{
		WaitForSingleObject(pCarBodyInfo->hInterruptEventIsrPin, INFINITE);

		if (pCarBodyInfo->hInterruptEventIsrPin)
		{
			DBGD((TEXT("\r\nIsrPin_io_isr Running")));
			SetEvent(pCarBodyInfo->hThreadHandleFlyCarbodyCommEvent);
	
			//SOC_IO_ISR_ReEn(pCarBodyInfo->hInterruptEventIsrPin);
		}
	}

	pCarBodyInfo->bInterruptIsrPinProcRunning = FALSE;
	DBGE((TEXT("\r\nIsrPin_io_isr Finish")));
	return NULL;
}

void MCU_IIC_Isr_Enable(P_FLY_CARBODY_INFO pCarBodyInfo,BOOL bEnable)
{
	if (bEnable)
	{
		//中断开启
		//SOC_IO_ISR_Add(IIC_ISR_PIN_G,IIC_ISR_PIN_I,GPIO_INTTYPE_FALLING_EDGE,IsrPin_io_isr,pCarBodyInfo,&pCarBodyInfo->hInterruptEventIsrPin);
		//DBGE((TEXT("\r\nCarbody Create IsrPin_io_isr")));

		SetEvent(pCarBodyInfo->hThreadHandleFlyCarbodyCommEvent);
	}
	else
	{
		////中断关闭
		//if (pCarBodyInfo->hInterruptEventIsrPin)
		//{
		//	SOC_IO_ISR_Del(&(pCarBodyInfo->hInterruptEventIsrPin));
		//	//while (pCarBodyInfo->bInterruptIsrPinProcRunning)
		//	//{
		//	//	Sleep(10);
		//	//}
		//}
	}
}

void CarBodyEnable(P_FLY_CARBODY_INFO pCarBodyInfo,BOOL bEnable)
{
	DWORD dwThreadID;

	if (bEnable)
	{
		if (pCarBodyInfo->bCarbodyEnable)
		{
			return;
		}
		pCarBodyInfo->bCarbodyEnable = TRUE;

		pCarBodyInfo->hFlyCarbodySerial = OpenSerial(_T("COM4:"),115200);
		if(pCarBodyInfo->hFlyCarbodySerial == NULL)
		{
			DBGE((TEXT("FlyAudio Carbody Open Serial error!\r\n")));
			return;
		}



		pCarBodyInfo->bKillDispatchFlyMainThread = FALSE;
		pCarBodyInfo->hDispatchMainThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
		pCarBodyInfo->FlyMainThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadMain, //线程的全局函数
			pCarBodyInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\nCarbody Create ThreadMain ID:0x%x"),dwThreadID));
		if (NULL == pCarBodyInfo->FlyMainThreadHandle)
		{
			pCarBodyInfo->bKillDispatchFlyMainThread = TRUE;
			return;
		}

		pCarBodyInfo->bKillDispatchFlyCarbodyCommThread = FALSE;
		pCarBodyInfo->hThreadHandleFlyCarbodyCommEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		pCarBodyInfo->hThreadHandleFlyCarbodyComm = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadCarbodyComm, //线程的全局函数
			pCarBodyInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\nCarbody Create ThreadCarbodyComm ID:0x%x"),dwThreadID));
		if (NULL == pCarBodyInfo->hThreadHandleFlyCarbodyComm)
		{
			pCarBodyInfo->bKillDispatchFlyCarbodyCommThread = TRUE;
			return;
		}

		MCU_IIC_Isr_Enable(pCarBodyInfo,TRUE);
		SetEvent(pCarBodyInfo->hDispatchMainThreadEvent);
	}
	else
	{
		if (!pCarBodyInfo->bCarbodyEnable)
		{
			return;
		}
		pCarBodyInfo->bCarbodyEnable = FALSE;

		pCarBodyInfo->bKillDispatchFlyCarbodyCommThread = TRUE;
		SetEvent(pCarBodyInfo->hThreadHandleFlyCarbodyCommEvent);
		pCarBodyInfo->bKillDispatchFlyMainThread = TRUE;
		SetEvent(pCarBodyInfo->hDispatchMainThreadEvent);

		MCU_IIC_Isr_Enable(pCarBodyInfo,FALSE);

		CloseSerial(pCarBodyInfo->hFlyCarbodySerial);
		pCarBodyInfo->hFlyCarbodySerial = NULL;

		while (pCarBodyInfo->FlyMainThreadHandle)
		{
			Sleep(10);
		}
	}
}

static DWORD WINAPI ThreadPowerUp(LPVOID pContext)
{
	P_FLY_CARBODY_INFO	pCarBodyInfo = (P_FLY_CARBODY_INFO)pContext;

	//Sleep(314);
	MCU_IIC_Isr_Enable(pCarBodyInfo,TRUE);
	RETAILMSG(1, (TEXT("\r\nCarBody ThreadPowerUp")));

	return 0;
}

static void powerOnNormalInit(P_FLY_CARBODY_INFO pCarBodyInfo)
{
	pCarBodyInfo->buffFromMCUProcessorStatus = 0;
	pCarBodyInfo->buffFromMCUFrameLength = 0;
	pCarBodyInfo->buffFromMCUFrameLengthMax = 0;
	pCarBodyInfo->buffFromMCUCRC = 0;

	pCarBodyInfo->xxxxxxxxStarting = FALSE;

	pCarBodyInfo->bHaveRecPowerOffOrReset = FALSE;
	pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNoMoreToSendDataWhenToSleep = FALSE;
}

static void powerOnFirstInit(P_FLY_CARBODY_INFO pCarBodyInfo)
{
	pCarBodyInfo->bCarbodyEnable = FALSE;

	pCarBodyInfo->bKillDispatchFlyMainThread = TRUE;
	pCarBodyInfo->FlyMainThreadHandle = NULL;
	pCarBodyInfo->hDispatchMainThreadEvent = NULL;

	pCarBodyInfo->hFlyCarbodySerial = NULL;
	pCarBodyInfo->bKillDispatchFlyCarbodyCommThread = TRUE;
	pCarBodyInfo->hThreadHandleFlyCarbodyComm = NULL;

	pCarBodyInfo->hMsgQueueToRadioCreate = NULL;

	pCarBodyInfo->hMsgQueueToSystemCreate = NULL;
	pCarBodyInfo->hMsgQueueFromSystemCreate = NULL;
	pCarBodyInfo->bKillDispatchFlyMsgQueueSystemReadThread = TRUE;
	pCarBodyInfo->FLyMsgQueueSystemReadThreadHandle = NULL;

	//key
	pCarBodyInfo->hMsgQueueToKeyCreate = NULL;
	pCarBodyInfo->hMsgQueueFromKeyCreate = NULL;
	pCarBodyInfo->bKillDispatchFlyMsgQueueKeyReadThread = TRUE;
	pCarBodyInfo->FLyMsgQueueKeyReadThreadHandle = NULL;

	/****************INIT CAR AMPLIFIER******************/
	pCarBodyInfo->hMsgQueueToCarAmpCreate = NULL;
	pCarBodyInfo->hMsgQueueFromCarAmpCreate = NULL;
	pCarBodyInfo->bKillDispatchFlyMsgQueueCarAmpReadThread = TRUE;
	pCarBodyInfo->FLyMsgQueueCarAmpReadThreadHandle = NULL;
	/********************************************************/

	/****************INIT FLYAUDIO AMPLIFIER******************/
	pCarBodyInfo->hMsgQueueToExtAmpCreate = NULL;
	pCarBodyInfo->hMsgQueueFromExtAmpCreate = NULL;
	pCarBodyInfo->bKillDispatchFlyMsgQueueExtAmpReadThread = TRUE;
	pCarBodyInfo->FLyMsgQueueExtAmpReadThreadHandle = NULL;
	/********************************************************/

	/***************FFAIII***************************************/	
	pCarBodyInfo->hMsgQueueToFaiiiCreate = NULL;
	pCarBodyInfo->hMsgQueueFromFaiiiCreate = NULL;
	pCarBodyInfo->bKillDispatchFlyMsgQueueFaiiiReadThread = TRUE;
	pCarBodyInfo->FLyMsgQueueFaiiiReadThreadHandle = NULL;
	/*********************************************************/

	/***************    AC     *********************************/	
	pCarBodyInfo->hMsgQueueToAcCreate = NULL;
	pCarBodyInfo->hMsgQueueFromAcCreate = NULL;
	pCarBodyInfo->bKillDispatchFlyMsgQueueAcReadThread = TRUE;
	pCarBodyInfo->FLyMsgQueueAcReadThreadHandle = NULL;
	/***********************************************************/

	pCarBodyInfo->hMsgQueueFromAssistDisplayCreate = NULL;
	pCarBodyInfo->bKillDispatchFlyMsgQueueAssistDisplayReadThread = TRUE;
	pCarBodyInfo->FLyMsgQueueAssistDisplayReadThreadHandle = NULL;

	pCarBodyInfo->hFlyCarbodySerial = NULL;

	pCarBodyInfo->bKillDispatchFlyCarbodyCommThread = TRUE;
	pCarBodyInfo->hThreadHandleFlyCarbodyComm = NULL;
	pCarBodyInfo->hThreadHandleFlyCarbodyCommEvent = NULL;

	pCarBodyInfo->hInterruptEventIsrPin = NULL;
	pCarBodyInfo->bInterruptIsrPinProcRunning = FALSE;

	pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNoMoreToSendDataWhenToSleep = FALSE;
}

static void powerOnSpecialEnable(P_FLY_CARBODY_INFO pCarBodyInfo,BOOL bOn)
{
	DWORD dwThreadID;

	if (bOn)
	{
		MSGQUEUEOPTIONS  msgOpts;

		//Global
		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_GLOBAL_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_GLOBAL_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pCarBodyInfo->hMsgQueueToGlobalCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_GLOBAL_NAME, &msgOpts);
		if (NULL == pCarBodyInfo->hMsgQueueToGlobalCreate)
		{
			RETAILMSG(1, (TEXT("\r\nCarbody Create MsgQueue To Global Fail!")));
		}

		//Key
		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_KEY_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_KEY_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pCarBodyInfo->hMsgQueueToKeyCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_KEY_NAME, &msgOpts);
		if (NULL == pCarBodyInfo->hMsgQueueToKeyCreate)
		{
			RETAILMSG(1, (TEXT("\r\nCarbody Create MsgQueue To Key Fail!")));
		}

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_FROM_KEY_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_FROM_KEY_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pCarBodyInfo->hMsgQueueFromKeyCreate = CreateMsgQueue(FLY_MSG_QUEUE_FROM_KEY_NAME, &msgOpts);
		if (NULL == pCarBodyInfo->hMsgQueueFromKeyCreate)
		{
			RETAILMSG(1, (TEXT("\r\nCarbody Create MsgQueue From Key Fail!")));
		}

		pCarBodyInfo->bKillDispatchFlyMsgQueueKeyReadThread = FALSE;
		pCarBodyInfo->FLyMsgQueueKeyReadThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadKeyRead, //线程的全局函数
			pCarBodyInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
			DBGE((TEXT("\r\nCarbody ThreadKeyRead ID:0x%x"),dwThreadID));
			if (NULL == pCarBodyInfo->FLyMsgQueueKeyReadThreadHandle)
			{
				pCarBodyInfo->bKillDispatchFlyMsgQueueKeyReadThread = TRUE;
				return;
			}
		//Radio
		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_RADIO_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_RADIO_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pCarBodyInfo->hMsgQueueToRadioCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_RADIO_NAME, &msgOpts);
		if (NULL == pCarBodyInfo->hMsgQueueToRadioCreate)
		{
			RETAILMSG(1, (TEXT("\r\nCarbody Create MsgQueue To Radio Fail!")));
		}

		//System
		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_SYSTEM_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_SYSTEM_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pCarBodyInfo->hMsgQueueToSystemCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_SYSTEM_NAME, &msgOpts);
		if (NULL == pCarBodyInfo->hMsgQueueToSystemCreate)
		{
			RETAILMSG(1, (TEXT("\r\nCarbody Create MsgQueue To System Fail!")));
		}

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_FROM_SYSTEM_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_FROM_SYSTEM_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pCarBodyInfo->hMsgQueueFromSystemCreate = CreateMsgQueue(FLY_MSG_QUEUE_FROM_SYSTEM_NAME, &msgOpts);
		if (NULL == pCarBodyInfo->hMsgQueueFromSystemCreate)
		{
			RETAILMSG(1, (TEXT("\r\nCarbody Create MsgQueue From System Fail!")));
		}

		pCarBodyInfo->bKillDispatchFlyMsgQueueSystemReadThread = FALSE;
		pCarBodyInfo->FLyMsgQueueSystemReadThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadSystemRead, //线程的全局函数
			pCarBodyInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\nCarbody ThreadSystemRead ID:0x%x"),dwThreadID));
		if (NULL == pCarBodyInfo->FLyMsgQueueSystemReadThreadHandle)
		{
			pCarBodyInfo->bKillDispatchFlyMsgQueueSystemReadThread = TRUE;
			return;
		}
		/****************************FFAIII***************************/
		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_FAIII_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_FAIII_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pCarBodyInfo->hMsgQueueToFaiiiCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_FAIII_NAME, &msgOpts);
		if (NULL == pCarBodyInfo->hMsgQueueToFaiiiCreate)
		{
			RETAILMSG(1, (TEXT("\r\nCarbody Create MsgQueue To Faiii Fail!")));
		}

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_FROM_FAIII_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_FROM_FAIII_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pCarBodyInfo->hMsgQueueFromFaiiiCreate = CreateMsgQueue(FLY_MSG_QUEUE_FROM_FAIII_NAME, &msgOpts);
		if (NULL == pCarBodyInfo->hMsgQueueFromFaiiiCreate)
		{
			RETAILMSG(1, (TEXT("\r\nCarbody Create MsgQueue From Faiii Fail!")));
		}

		pCarBodyInfo->bKillDispatchFlyMsgQueueFaiiiReadThread = FALSE;
		pCarBodyInfo->FLyMsgQueueFaiiiReadThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadFaiiiRead, //线程的全局函数
			pCarBodyInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\nCarbody ThreadFaiiiRead ID:0x%x"),dwThreadID));
		if (NULL == pCarBodyInfo->FLyMsgQueueFaiiiReadThreadHandle)
		{
			pCarBodyInfo->bKillDispatchFlyMsgQueueFaiiiReadThread = TRUE;
			return;
		}
		/*************************************************************/

		/****************************  AC  ***************************/
		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_AC_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_AC_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pCarBodyInfo->hMsgQueueToAcCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_AC_NAME, &msgOpts);
		if (NULL == pCarBodyInfo->hMsgQueueToAcCreate)
		{
			RETAILMSG(1, (TEXT("\r\nCarbody Create MsgQueue To AC Fail!")));
		}

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_FROM_AC_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_FROM_AC_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pCarBodyInfo->hMsgQueueFromAcCreate = CreateMsgQueue(FLY_MSG_QUEUE_FROM_AC_NAME, &msgOpts);
		if (NULL == pCarBodyInfo->hMsgQueueFromAcCreate)
		{
			RETAILMSG(1, (TEXT("\r\nCarbody Create MsgQueue From AC Fail!")));
		}

		pCarBodyInfo->bKillDispatchFlyMsgQueueAcReadThread = FALSE;
		pCarBodyInfo->FLyMsgQueueAcReadThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadAcRead, //线程的全局函数
			pCarBodyInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\nCarbody ThreadACRead ID:0x%x"),dwThreadID));
		if (NULL == pCarBodyInfo->FLyMsgQueueAcReadThreadHandle)
		{
			pCarBodyInfo->bKillDispatchFlyMsgQueueAcReadThread = TRUE;
			return;
		}
		/*************************************************************/
		
		/************CREATE AND ENABLE FLYAUDIO EXT AMPLIFIER**********/
		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_EXTAMP_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_EXTAMP_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pCarBodyInfo->hMsgQueueToExtAmpCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_EXTAMP_NAME, &msgOpts);
		if (NULL == pCarBodyInfo->hMsgQueueToExtAmpCreate)
		{
			RETAILMSG(1, (TEXT("\r\nCarbody Create MsgQueue To EXTAMP Fail!")));
		}

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_FROM_EXTAMP_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_FROM_EXTAMP_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pCarBodyInfo->hMsgQueueFromExtAmpCreate = CreateMsgQueue(FLY_MSG_QUEUE_FROM_EXTAMP_NAME, &msgOpts);
		if (NULL == pCarBodyInfo->hMsgQueueFromExtAmpCreate)
		{
			RETAILMSG(1, (TEXT("\r\nCarbody Create MsgQueue From EXTAMP Fail!")));
		}

		pCarBodyInfo->bKillDispatchFlyMsgQueueExtAmpReadThread = FALSE;
		pCarBodyInfo->FLyMsgQueueExtAmpReadThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadExtAmpRead, //线程的全局函数
			pCarBodyInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\nCarbody ThreadExtAmpRead ID:0x%x"),dwThreadID));
		if (NULL == pCarBodyInfo->FLyMsgQueueExtAmpReadThreadHandle)
		{
			pCarBodyInfo->bKillDispatchFlyMsgQueueExtAmpReadThread = TRUE;
			return;
		}
		/***************************************************************/

		/*******************     CAR AMPLIFIER     ************************/
		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_TO_CARAMP_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_TO_CARAMP_LENGTH;
		msgOpts.bReadAccess = FALSE;
		pCarBodyInfo->hMsgQueueToCarAmpCreate = CreateMsgQueue(FLY_MSG_QUEUE_TO_CARAMP_NAME, &msgOpts);
		if (NULL == pCarBodyInfo->hMsgQueueToCarAmpCreate)
		{
			RETAILMSG(1, (TEXT("\r\nCarbody Create MsgQueue To CARAMP Fail!")));
		}

		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_FROM_CARAMP_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_FROM_CARAMP_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pCarBodyInfo->hMsgQueueFromCarAmpCreate = CreateMsgQueue(FLY_MSG_QUEUE_FROM_CARAMP_NAME, &msgOpts);
		if (NULL == pCarBodyInfo->hMsgQueueFromCarAmpCreate)
		{
			RETAILMSG(1, (TEXT("\r\nCarbody Create MsgQueue From CAR AMP Fail!")));
		}

		pCarBodyInfo->bKillDispatchFlyMsgQueueCarAmpReadThread = FALSE;
		pCarBodyInfo->FLyMsgQueueCarAmpReadThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadCarAmpRead, //线程的全局函数
			pCarBodyInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\nCarbody Thread Car Amp Read ID:0x%x"),dwThreadID));
		if (NULL == pCarBodyInfo->FLyMsgQueueCarAmpReadThreadHandle)
		{
			pCarBodyInfo->bKillDispatchFlyMsgQueueCarAmpReadThread = TRUE;
			return;
		}
		/***************************************************************/
		//AssistDisplay
		msgOpts.dwSize = sizeof(MSGQUEUEOPTIONS);
		msgOpts.dwFlags = FLY_MSG_QUEUE_FLAG;
		msgOpts.dwMaxMessages = FLY_MSG_QUEUE_FROM_ASSISTDISPLAY_ENTRIES;
		msgOpts.cbMaxMessage = FLY_MSG_QUEUE_FROM_ASSISTDISPLAY_LENGTH;
		msgOpts.bReadAccess = TRUE;
		pCarBodyInfo->hMsgQueueFromAssistDisplayCreate = CreateMsgQueue(FLY_MSG_QUEUE_FROM_ASSISTDISPLAY_NAME, &msgOpts);
		if (NULL == pCarBodyInfo->hMsgQueueFromAssistDisplayCreate)
		{
			RETAILMSG(1, (TEXT("\r\nCarbody Create MsgQueue From AssistDisplay Fail!")));
		}

		pCarBodyInfo->bKillDispatchFlyMsgQueueAssistDisplayReadThread = FALSE;
		pCarBodyInfo->FLyMsgQueueAssistDisplayReadThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadAssistDisplayRead, //线程的全局函数
			pCarBodyInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\nCarbody ThreadAssistDisplayRead ID:0x%x"),dwThreadID));
		if (NULL == pCarBodyInfo->FLyMsgQueueAssistDisplayReadThreadHandle)
		{
			pCarBodyInfo->bKillDispatchFlyMsgQueueAssistDisplayReadThread = TRUE;
			return;
		}
	} 
	else
	{
		pCarBodyInfo->bKillDispatchFlyMsgQueueSystemReadThread = TRUE;
		while (pCarBodyInfo->FLyMsgQueueSystemReadThreadHandle)
		{
			SetEvent(pCarBodyInfo->hMsgQueueFromSystemCreate);
			Sleep(10);
		}
		CloseMsgQueue(pCarBodyInfo->hMsgQueueToSystemCreate);
		CloseMsgQueue(pCarBodyInfo->hMsgQueueFromSystemCreate);

		/********CLOSE AND DISABLE MSG FLYAUDIO EXT AMPLIFIER***/
		pCarBodyInfo->bKillDispatchFlyMsgQueueExtAmpReadThread = TRUE;
		while (pCarBodyInfo->FLyMsgQueueExtAmpReadThreadHandle)
		{
			SetEvent(pCarBodyInfo->hMsgQueueFromExtAmpCreate);
			Sleep(10);
		}
		CloseMsgQueue(pCarBodyInfo->hMsgQueueToExtAmpCreate);
		CloseMsgQueue(pCarBodyInfo->hMsgQueueFromExtAmpCreate);
		/********************************************************/

		/********CLOSE AND DISABLE MSG CAR AMPLIFIER************/
		pCarBodyInfo->bKillDispatchFlyMsgQueueCarAmpReadThread = TRUE;
		while (pCarBodyInfo->FLyMsgQueueCarAmpReadThreadHandle)
		{
			SetEvent(pCarBodyInfo->hMsgQueueFromCarAmpCreate);
			Sleep(10);
		}
		CloseMsgQueue(pCarBodyInfo->hMsgQueueToCarAmpCreate);
		CloseMsgQueue(pCarBodyInfo->hMsgQueueFromCarAmpCreate);
		/********************************************************/

		/**************************FFAIII****************************/
		pCarBodyInfo->bKillDispatchFlyMsgQueueFaiiiReadThread = TRUE;
		while (pCarBodyInfo->FLyMsgQueueFaiiiReadThreadHandle)
		{
			SetEvent(pCarBodyInfo->hMsgQueueFromFaiiiCreate);
			Sleep(10);
		}
		CloseMsgQueue(pCarBodyInfo->hMsgQueueToFaiiiCreate);
		CloseMsgQueue(pCarBodyInfo->hMsgQueueFromFaiiiCreate);
		/***********************************************************/

		/*************************  AC ****************************/
		pCarBodyInfo->bKillDispatchFlyMsgQueueAcReadThread = TRUE;
		while (pCarBodyInfo->FLyMsgQueueAcReadThreadHandle)
		{
			SetEvent(pCarBodyInfo->hMsgQueueFromAcCreate);
			Sleep(10);
		}
		CloseMsgQueue(pCarBodyInfo->hMsgQueueToAcCreate);
		CloseMsgQueue(pCarBodyInfo->hMsgQueueFromAcCreate);
		/***********************************************************/

		pCarBodyInfo->bKillDispatchFlyMsgQueueAssistDisplayReadThread = TRUE;
		while (pCarBodyInfo->FLyMsgQueueAssistDisplayReadThreadHandle)
		{
			SetEvent(pCarBodyInfo->hMsgQueueFromAssistDisplayCreate);
			Sleep(10);
		}
		CloseMsgQueue(pCarBodyInfo->hMsgQueueFromAssistDisplayCreate);

		CloseMsgQueue(pCarBodyInfo->hMsgQueueToRadioCreate);

		//key
		pCarBodyInfo->bKillDispatchFlyMsgQueueKeyReadThread = TRUE;
		while (pCarBodyInfo->FLyMsgQueueKeyReadThreadHandle)
		{
			SetEvent(pCarBodyInfo->hMsgQueueFromKeyCreate);
			Sleep(10);
		}
		CloseMsgQueue(pCarBodyInfo->hMsgQueueToKeyCreate);
		CloseMsgQueue(pCarBodyInfo->hMsgQueueFromKeyCreate);
	}
		
}

void IpcRecv(UINT32 iEvent)
{
	P_FLY_CARBODY_INFO	pCarBodyInfo = (P_FLY_CARBODY_INFO)gpCarBodyInfo;

	DBGD((TEXT("\r\n FLY Carbody Recv IPC Event:%d"),iEvent));

	SetEvent(pCarBodyInfo->hDispatchMainThreadEvent);
}

void SockRecv(BYTE *buf, UINT16 len)
{
	UINT16 i=0;
	P_FLY_CARBODY_INFO	pCarBodyInfo = (P_FLY_CARBODY_INFO)gpCarBodyInfo;

	if (buf[1] == 0x90)//升级命令
	{
		DBGD((TEXT("\r\n FLY CarBody socket recv %d bytes:"),len));
		for (i=0; i<len; i++)
			DBGD((TEXT("%02X "), buf[i]));

		if (buf[2] == 0x01)//准备升级
		{
			//告诉SYSTEM，准备进行LPC升级
			pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLpcUpdate = TRUE;
			IpcStartEvent(EVENT_GLOBAL_LPC_UPDATE);//LPC升级状态
		}
		else	//退出升级
		{
			//告诉SYSTEM,退出LPC升级
			pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bLpcUpdate = FALSE;
			IpcStartEvent(EVENT_GLOBAL_LPC_UPDATE);//LPC升级状态
		}
	}
	else //不关心这些协议，转发给LPC
	{
		//加头：FF 55 
		pCarBodyInfo->buffLpcFrame[0] = 0xFF;
		pCarBodyInfo->buffLpcFrame[1] = 0x55;
		pCarBodyInfo->buffLpcFrame[2] = (BYTE)len;
		memcpy(&pCarBodyInfo->buffLpcFrame[3],buf,len);
		actualwriteToMCU(pCarBodyInfo,pCarBodyInfo->buffLpcFrame,len+3);

		DBGD((TEXT("\r\n FLY CarBody socket recv %d bytes:"),len+3));
		for (i=0; i<len+3; i++)
			DBGD((TEXT("%02X "), pCarBodyInfo->buffLpcFrame[i]));
	}
}


HANDLE
FCB_Init(DWORD dwContext)
{
	P_FLY_CARBODY_INFO pCarBodyInfo;

	RETAILMSG(1, (TEXT("\r\nCarbody Init Start")));


	pCarBodyInfo = (P_FLY_CARBODY_INFO)LocalAlloc(LPTR, sizeof(FLY_CARBODY_INFO));
	if (!pCarBodyInfo)
	{
		return NULL;
	}
	gpCarBodyInfo = pCarBodyInfo;

	pCarBodyInfo->pFlyShmGlobalInfo = CreateShm(CARBODY_MODULE,IpcRecv);
	if (NULL == pCarBodyInfo->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio CarBody Init create shm err\r\n")));
		return NULL;
	}
	
	if(!CreateServerSocket(SockRecv, TCP_PORT_CARBODY))
	{
		DBGE((TEXT("FlyAudio CarBody Open create server socket err\r\n")));
		return NULL;
	}


	if (pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalStructSize
		!= sizeof(struct shm))
	{
		pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[0] = 'F';
		pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[1] = 'C';
		pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[2] = 'B';
	}

	//参数初始化
	powerOnNormalInit(pCarBodyInfo);
	powerOnFirstInit(pCarBodyInfo);
	powerOnSpecialEnable(pCarBodyInfo,TRUE);

	CarBodyEnable(pCarBodyInfo,TRUE);

	//pCarBodyInfo->hHandleGlobalGlobalEvent = CreateEvent(NULL,FALSE,FALSE,DATA_GLOBAL_HANDLE_GLOBAL);
	pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.bInit = TRUE;
	pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.bOpen = TRUE;
	pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.iDriverCompYear = year;
	pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.iDriverCompMon = months;
	pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.iDriverCompDay = day;
	pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.iDriverCompHour = hours;
	pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.iDriverCompMin = minutes;
	pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.iDriverCompSec = seconds;

	RETAILMSG(1, (TEXT("\r\nCarbody Init Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return (HANDLE)pCarBodyInfo;
}

BOOL
FCB_Deinit(DWORD hDeviceContext)
{
	P_FLY_CARBODY_INFO	pCarBodyInfo = (P_FLY_CARBODY_INFO)hDeviceContext;

#if GLOBAL_COMM
	//全局
	pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.bOpen = FALSE;
	pCarBodyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.bInit = FALSE;
	SetEvent(pCarBodyInfo->hHandleGlobalGlobalEvent);
	CloseHandle(pCarBodyInfo->hHandleGlobalGlobalEvent);
#endif

	CarBodyEnable(pCarBodyInfo,FALSE);
	powerOnSpecialEnable(pCarBodyInfo,FALSE);

	FreeShm();
	FreeSocketServer();
	LocalFree(pCarBodyInfo);

	RETAILMSG(1, (TEXT("FlyAudio CarBody DeInit\r\n")));
	return TRUE;
}

DWORD
FCB_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	P_FLY_CARBODY_INFO	pCarBodyInfo = (P_FLY_CARBODY_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;

	RETAILMSG(1, (TEXT("\r\nCarbody Open Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return returnWhat;
}

BOOL
FCB_Close(DWORD hDeviceContext)
{
	P_FLY_CARBODY_INFO	pCarBodyInfo = (P_FLY_CARBODY_INFO)hDeviceContext;

	RETAILMSG(1, (TEXT("FlyAudio CarBody Close\r\n")));
	return TRUE;
}

VOID
FCB_PowerUp(DWORD hDeviceContext)
{
	P_FLY_CARBODY_INFO	pCarBodyInfo = (P_FLY_CARBODY_INFO)hDeviceContext;
	DWORD dwThreadID;

	powerOnNormalInit(pCarBodyInfo);
	

	CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
		0,//初始化线程栈的大小，缺省为与主线程大小相同
		(LPTHREAD_START_ROUTINE)ThreadPowerUp, //线程的全局函数
		pCarBodyInfo, //此处传入了主框架的句柄
		0, &dwThreadID );
	DBGE((TEXT("\r\nCarbody ThreadPowerUp ID:0x%x"),dwThreadID));

	RETAILMSG(1, (TEXT("FlyAudio CarBody PowerUp\r\n")));
}

VOID
FCB_PowerDown(DWORD hDeviceContext)
{
	P_FLY_CARBODY_INFO	pCarBodyInfo = (P_FLY_CARBODY_INFO)hDeviceContext;

	RETAILMSG(1, (TEXT("FlyAudio CarBody PowerDown\r\n")));
}

DWORD
FCB_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	P_FLY_CARBODY_INFO	pCarBodyInfo = (P_FLY_CARBODY_INFO)hOpenContext;

	UINT dwRead = 0;

	return dwRead;
}

DWORD
FCB_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	P_FLY_CARBODY_INFO	pCarBodyInfo = (P_FLY_CARBODY_INFO)hOpenContext;
	DBGD((TEXT("FlyAudio CarBody Write %d \r\n"),
					NumberOfBytes));

	return NULL;
}

DWORD
FCB_Seek(DWORD hOpenContext, LONG Amount, DWORD  Type)
{
	return NULL;
}

BOOL
FCB_IOControl(DWORD hOpenContext,
			  DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn,
			  PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
	P_FLY_CARBODY_INFO	pCarBodyInfo = (P_FLY_CARBODY_INFO)hOpenContext;
	BOOL bRes = TRUE;

	RETAILMSG(1, (TEXT("FlyAudio CarBody IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)\r\n"),
					hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));

	//switch (dwCode)
	//{
	//default :
	//	bRes = FALSE;
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
		DEBUGMSG(1, (TEXT("Attach in FlyCarBody DllEntry\n")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}

	if (dwReason == DLL_PROCESS_DETACH)
	{
		DEBUGMSG(1, (TEXT("Dettach in FlyCarBody DllEntry\n")));
	}

	DEBUGMSG(1, (TEXT("Leaving FlyCarBody DllEntry\n")));

	return (TRUE);
}
