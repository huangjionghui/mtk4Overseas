/***************************************************************************************************
**开发环境：飞歌7500新板
**功能描述： Parrot驱动
**文件名称： FLYParrot.cpp
**版本信息： 
**创建时间： 2011-05-13
**创建人  ： 
**--------------------------------------------------------------------------------------------------
**--------------------------------------------------------------------------------------------------
**修 改 人：Huangjionghui
**修改时间：2011-05-17	            
**修改内容： 实现 
**--------------------------------------------------------------------------------------------------
**修 改 人：Huangjionghui
**修改时间：2011-05-23	            
**修改内容：1、增加全局的状态
			2、修改蓝牙的初始返回值
****************************************************************************************************/

#include "FLYParrot.h"
#include "flygpio.h"
#include "flyhwlib.h"
#include "flyuserlib.h"
#include "flyseriallib.h"


P_FLY_PARROT_INFO gpFlyParrotInfo=NULL;


#if A4_PLAT
#define IO_POWER_G	0
#define IO_POWER_I	22
#define IO_RESET_G	0
#define IO_RESET_I	18

#else
#define IO_POWER_G	2
#define IO_POWER_I	26
#define IO_RESET_G	2
#define IO_RESET_I	27
#define IO_CE_G	3
#define IO_CE_I	0
#endif


VOID FBT_PowerUp(DWORD hDeviceContext);
VOID FBT_PowerDown(DWORD hDeviceContext);
static DWORD WINAPI FlyBT_ThreadFlyParrotBTCommProc(LPVOID pContext);
static VOID FlyBT_OpenParrotBTComm(P_FLY_PARROT_INFO pFlyParrotInfo);
static VOID FlyBT_CloseParrotBTComm(P_FLY_PARROT_INFO pFlyParrotInfo);
static VOID FlyBT_DealBTInfo(P_FLY_PARROT_INFO pFlyParrotInfo, BYTE *buf, UINT len);
static VOID FlyBT_CheckBTRecvData(P_FLY_PARROT_INFO pFlyParrotInfo, BYTE *buf, UINT len);


static VOID FlyBT_PowerControl_On(P_FLY_PARROT_INFO pFlyParrotInfo)
{
	//SOC_IO_Output(IO_POWER_G,IO_POWER_I,1);
	RETAILMSG(1, (TEXT("\r\nFlyAudio ParrotBT PowerControl On")));
}

static VOID FlyBT_PowerControl_Off(P_FLY_PARROT_INFO pFlyParrotInfo)
{
	//SOC_IO_Output(IO_POWER_G,IO_POWER_I,0);
	RETAILMSG(1, (TEXT("\r\nFlyAudio ParrotBT PowerControl Off")));
}

static VOID FlyBT_ResetControl_On(P_FLY_PARROT_INFO pFlyParrotInfo)
{
	//SOC_IO_Output(IO_RESET_G,IO_RESET_I,0);
	RETAILMSG(1, (TEXT("\r\nFlyAudio ParrotBT ResetControl On")));
}

static VOID FlyBT_ResetControl_Off(P_FLY_PARROT_INFO pFlyParrotInfo)
{
	//SOC_IO_Output(IO_RESET_G,IO_RESET_I,1);
	RETAILMSG(1, (TEXT("\r\nFlyAudio ParrotBT ResetControl Off")));
}

static VOID FlyBT_ChipEnableControl_On(P_FLY_PARROT_INFO pFlyParrotInfo)
{
	//#if A4_PLAT
	//#else
	//SOC_IO_Output(IO_CE_G,IO_CE_I,0);
	//#endif
	RETAILMSG(1, (TEXT("\r\nFlyAudio ParrotBT ChipEnableControl On")));
}

static VOID FlyBT_ChipEnableControl_Off(P_FLY_PARROT_INFO pFlyParrotInfo)
{
	//#if A4_PLAT
	//#else
	//SOC_IO_Output(IO_CE_G,IO_CE_I,1);
	//#endif
	RETAILMSG(1, (TEXT("\r\nFlyAudio ParrotBT ChipEnableControl Off")));
}

static VOID Return_BTDevicePowerMode(P_FLY_PARROT_INFO pFlyParrotInfo, BOOL bEnable)
{
	BYTE buff[2]={0x01, 0x00};
	if (bEnable)
	{
		buff[1] = 0x01;
	}
	ReturnToUser(buff,2);
}
static VOID Return_BTDeviceInitStatus(P_FLY_PARROT_INFO pFlyParrotInfo, BOOL bInit)
{
	BYTE buff[2] = {0x02, 0x00};
	if (bInit)
	{
		buff[1] = 0x01;
	}
	ReturnToUser(buff,2);
}

static VOID Return_BTDevice(P_FLY_PARROT_INFO pFlyParrotInfo)
{
	BYTE buff[2] = {0x03, 0x01};

	ReturnToUser(buff,2);

}
static VOID Return_BTPowerStatus(P_FLY_PARROT_INFO pFlyParrotInfo, BOOL bEnable)
{
	BYTE buff[2] = {0x10, 0x00};
	if (bEnable)
	{
		buff[1] = 0x01;
	}
	ReturnToUser(buff,2);
}
static VOID Return_BTData(P_FLY_PARROT_INFO pFlyParrotInfo, BYTE *p, BYTE len)
{
	BYTE buff[255];

	if(len>=254)len =254;

	buff[0] = 0x40;
	memcpy(&buff[1],p,len);
	ReturnToUser(buff,len+1);
}

static VOID Control_WriteToBT(P_FLY_PARROT_INFO pFlyParrotInfo, BYTE *p, UINT len)
{
	DWORD bufSendLength;
	UINT i;

	RETAILMSG(1, (TEXT("\r\nFlyAudio Write to ParrotBT:")));
	for (i = 0;i < len; i++)
	{
		RETAILMSG(1, (TEXT("%c"),p[i]));
	}

	WriteFile(pFlyParrotInfo->hBTComm, p, len, &bufSendLength, NULL);
}

static VOID Send_AT_Cmd(P_FLY_PARROT_INFO pFlyParrotInfo, BYTE *p, BYTE len)
{
	BYTE buff[255]={'A','T','+'};
	BYTE i;

	if(len>=254) len = 254;

	for(i=0;i<len;i++)
	{
		buff[i+3] = p[i];
	}

	buff[len+3]= 0x0d;

	Control_WriteToBT(pFlyParrotInfo,buff,len+4);
}


static VOID FlyBT_InitParrotBT(P_FLY_PARROT_INFO pFlyParrotInfo)
{
	pFlyParrotInfo->iBTInfoFrameStatus = 0;
	pFlyParrotInfo->iFrameLenMax = 0;
	pFlyParrotInfo->iFrameLen = 0;
	pFlyParrotInfo->iChecksum = 0;
	memset(pFlyParrotInfo->ReceiveBuf,0,sizeof(pFlyParrotInfo->ReceiveBuf));
}

static VOID FlyBT_OpenParrotBTComm(P_FLY_PARROT_INFO pFlyParrotInfo)
{
	if (pFlyParrotInfo->hBTComm)
	{
		return;
	}

	pFlyParrotInfo->hBTComm = OpenSerial(_T("COM5:"), 115200);
	RETAILMSG(1, (TEXT("FlyAudio ParrotBT open com5!")));
	if(pFlyParrotInfo->hBTComm == INVALID_HANDLE_VALUE)
	{
		pFlyParrotInfo->hBTComm = NULL;
		RETAILMSG(1, (TEXT("FlyAudio ParrotBT cann't open com!")));
		return;
	}

	DWORD dwThreadID;
	pFlyParrotInfo->hThreadHandleFlyBTComm = 
		CreateThread((LPSECURITY_ATTRIBUTES) NULL, 0,(LPTHREAD_START_ROUTINE)FlyBT_ThreadFlyParrotBTCommProc,
		pFlyParrotInfo, 0, &dwThreadID );
	DBGI((TEXT("\r\nFlyAudio FlyBT_ThreadFlyParrotBTCommProc ID %x!"),dwThreadID));
	if (NULL == pFlyParrotInfo->hThreadHandleFlyBTComm)
	{
		RETAILMSG(1, (TEXT("FlyAudio ParrotBT CreateThread ThreadFlyBTCommProc!")));
		FlyBT_CloseParrotBTComm(pFlyParrotInfo);
		return;
	}
	RETAILMSG(1, (TEXT("FlyAudio ParrotBT Comm init OK!")));
}
static VOID FlyBT_CloseParrotBTComm(P_FLY_PARROT_INFO pFlyParrotInfo)
{
	SetCommMask(pFlyParrotInfo->hThreadHandleFlyBTComm,0);

	if (pFlyParrotInfo->hThreadHandleFlyBTComm)
	{
		CloseHandle(pFlyParrotInfo->hThreadHandleFlyBTComm);
	}

	//关闭串口
	CloseSerial(pFlyParrotInfo->hBTComm);
	pFlyParrotInfo->hBTComm = NULL;
}

static DWORD WINAPI FlyBT_ThreadFlyParrotBTCommProc(LPVOID pContext)
{
	P_FLY_PARROT_INFO pFlyParrotInfo = (P_FLY_PARROT_INFO)pContext;
	DWORD ret;
	BYTE recvBuf[1024];

	if (!pFlyParrotInfo->hBTComm || pFlyParrotInfo->hBTComm == INVALID_HANDLE_VALUE)
	{
		RETAILMSG(1, (TEXT("\r\nFlyAudio FlyBT_ThreadFlyParrotBTCommProc exit with com error")));
		return 0;
	}
	else
	{
		RETAILMSG(1, (TEXT("\r\nFlyAudio FlyBT_ThreadFlyParrotBTCommProc start")));

	}

	while (pFlyParrotInfo->hBTComm)
	{
		ret = ReadSerial(pFlyParrotInfo->hBTComm,recvBuf,1024);
		if(ret <= 0)
		{
			//不能从串口读取数据
			DBGI((TEXT("\r\nFlyAudio parrot ComProcThread cannot read data!")));
		}
		else
		{
			DBGD((TEXT("\r\nParrot Serial Read %d bytes:"),ret));
			for (UINT i = 0;i < ret;i++)
			{
				DBGD((TEXT(" %x"),recvBuf[i]));
			}

			//把数据赋值给全局变量
			FlyBT_CheckBTRecvData(pFlyParrotInfo,recvBuf,ret);
		}
	}
	RETAILMSG(1, (TEXT("\r\nFlyAudio ThreadFlyBTCommProc exit")));
	return 0;
}

static VOID FlyBT_CheckBTRecvData(P_FLY_PARROT_INFO pFlyParrotInfo, BYTE *buf, UINT len)
{

	static BYTE lastData;
	UINT i;
	if(!pFlyParrotInfo->bPower)
	{
		return;
	}

	for (i = 0; i < len; i++)
	{
		pFlyParrotInfo->ReceiveBuf[pFlyParrotInfo->iFrameLen++] = buf[i];

		if(pFlyParrotInfo->iFrameLen > 2)
		{
			if((pFlyParrotInfo->ReceiveBuf[0] == 0x0d)
				&&(pFlyParrotInfo->ReceiveBuf[1] == 0x0A)
				&&(pFlyParrotInfo->ReceiveBuf[2] != 0x0d))
			{
				if((lastData == 0x0D && buf[i] == 0x0A))//命令结尾
				{
					FlyBT_DealBTInfo(pFlyParrotInfo, pFlyParrotInfo->ReceiveBuf, pFlyParrotInfo->iFrameLen);	
					pFlyParrotInfo->iFrameLen = 0;
				}
				if(pFlyParrotInfo->iFrameLen > 234)
						pFlyParrotInfo->iFrameLen = 0;
				lastData = buf[i];	
			}
			else
			{
				pFlyParrotInfo->ReceiveBuf[0] = pFlyParrotInfo->ReceiveBuf[1];
				pFlyParrotInfo->ReceiveBuf[1]= pFlyParrotInfo->ReceiveBuf[2];
				pFlyParrotInfo->iFrameLen = 2;
			}
		}
	}
}
static VOID FlyBT_DealBTInfo(P_FLY_PARROT_INFO pFlyParrotInfo, BYTE *buf, UINT len)
{
	RETAILMSG(1, (TEXT("FlyParrotBT Recv data:")));
	for (UINT i=0; i<len; i++)
	{
		RETAILMSG(1,(TEXT("%c"), buf[i]));
	}		
	for (UINT i=0; i<len; i++)
	{
		RETAILMSG(1,(TEXT("%X"), buf[i]));
	}	
	Return_BTData(pFlyParrotInfo, &buf[0],len);
}

static DWORD WINAPI ThreadCollexBTMainProc(LPVOID pContext)
{
	P_FLY_PARROT_INFO pFlyParrotInfo = (P_FLY_PARROT_INFO)pContext;
	ULONG WaitReturn;

	while (!pFlyParrotInfo->bKillParrotBTMainThread)
	{

		WaitReturn = WaitForSingleObject(pFlyParrotInfo->hParrotBTMainThreadEvent, 2000);
		DBGD((TEXT("\r\nParrot BT Main process!")));

			if (IpcWhatEvent(EVENT_GLOBAL_SLEEP_PROC_BT_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_SLEEP_PROC_BT_ID);

				
					pFlyParrotInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeBTSleep
						= pFlyParrotInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeBTSleep;
			}

			if (IpcWhatEvent(EVENT_GLOBAL_STANDBY_BT_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_STANDBY_BT_ID);
			}

			if (pFlyParrotInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)//电源控制
			{
				if (pFlyParrotInfo->currentPower)
				{
					pFlyParrotInfo->currentPower = FALSE;
					FlyBT_PowerControl_Off(pFlyParrotInfo);
				}
			}
			else if (pFlyParrotInfo->controlPower)
			{
				if (!pFlyParrotInfo->currentPower)
				{
					pFlyParrotInfo->currentPower = TRUE;
					pFlyParrotInfo->bPower = TRUE;
					FlyBT_PowerControl_On(pFlyParrotInfo);
				}
			}
			else if (!pFlyParrotInfo->controlPower)
			{
				if (pFlyParrotInfo->currentPower)
				{	
					pFlyParrotInfo->bPower = FALSE;
					pFlyParrotInfo->currentPower = FALSE;
					FlyBT_PowerControl_Off(pFlyParrotInfo);
				}
			}
			//////
			{
				BYTE buff[4]={'C','G','M','I'};
				Send_AT_Cmd(pFlyParrotInfo,buff,4);
			}

			if (pFlyParrotInfo->bPower)//复位
			{
				if (!pFlyParrotInfo->bPowerUp)
				{
					DBGD((TEXT("\r\nParrot Reset")));
					FlyBT_ChipEnableControl_On(pFlyParrotInfo);

					FlyBT_ResetControl_On(pFlyParrotInfo);
					FlyBT_PowerControl_Off(pFlyParrotInfo);
					Sleep(100);
					FlyBT_PowerControl_On(pFlyParrotInfo);
					Sleep(100);
					FlyBT_ResetControl_Off(pFlyParrotInfo);
					Sleep(314);

					pFlyParrotInfo->bPowerUp = TRUE;
				}
			}
	}
	pFlyParrotInfo->hParrotBTMainThread = NULL;
	DBGD((TEXT("\r\nThreadParrot BTMainProc exit")));
		return 0;
}
static VOID collexBT_Enable(P_FLY_PARROT_INFO pFlyParrotInfo,BOOL bEnable)
{
	if (bEnable)
	{
		DBGI((TEXT("\r\nCollexBT_Enable Enable!")));

		FlyBT_PowerControl_Off(pFlyParrotInfo);
		FlyBT_ChipEnableControl_On(pFlyParrotInfo);
		FlyBT_ResetControl_Off(pFlyParrotInfo);

		pFlyParrotInfo->bKillParrotBTMainThread = FALSE;

		DWORD dwThreadID;
		pFlyParrotInfo->hParrotBTMainThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
		pFlyParrotInfo->hParrotBTMainThread = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL,0,
			(LPTHREAD_START_ROUTINE)ThreadCollexBTMainProc, pFlyParrotInfo,0, &dwThreadID );

		DBGI((TEXT("\r\nThreadParrotBTMainProc ID:%x"),dwThreadID));

		if (NULL == pFlyParrotInfo->hParrotBTMainThread)
		{
			pFlyParrotInfo->bKillParrotBTMainThread = TRUE;
			return;
		}
		FlyBT_OpenParrotBTComm(pFlyParrotInfo);
	}
	else
	{

		pFlyParrotInfo->bKillParrotBTMainThread = TRUE;
		SetEvent(pFlyParrotInfo->hParrotBTMainThreadEvent);
		while (pFlyParrotInfo->hParrotBTMainThread)
		{
			SetEvent(pFlyParrotInfo->hParrotBTMainThreadEvent);
			Sleep(10);
		}

		CloseHandle(pFlyParrotInfo->hParrotBTMainThread);
		pFlyParrotInfo->hParrotBTMainThread = NULL;
		pFlyParrotInfo->hParrotBTMainThreadEvent = NULL;
		FlyBT_CloseParrotBTComm(pFlyParrotInfo); 
		FlyBT_PowerControl_Off(pFlyParrotInfo);
	}
}

static VOID FlyBT_WinCECommandProcessor(P_FLY_PARROT_INFO pFlyParrotInfo, BYTE *buf, UINT len)
{
	RETAILMSG(1, (TEXT(" OK!")));

	switch(buf[0])
	{
	case 0x01:
		if (0x01 == buf[1])
		{

			pFlyParrotInfo->controlPower = FALSE;
			pFlyParrotInfo->currentPower = FALSE;

			pFlyParrotInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeBTSleep = TRUE;
			pFlyParrotInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeBTSleep = FALSE;

			FlyBT_InitParrotBT(pFlyParrotInfo);
			Return_BTDevicePowerMode(pFlyParrotInfo,TRUE);
			Return_BTDeviceInitStatus(pFlyParrotInfo,TRUE);
			Return_BTDevice(pFlyParrotInfo);
		}
		else if (0x00 == buf[1])
		{
			pFlyParrotInfo->controlPower = FALSE;
			pFlyParrotInfo->currentPower = FALSE;
			FlyBT_InitParrotBT(pFlyParrotInfo);
			Return_BTDevicePowerMode(pFlyParrotInfo,FALSE);
			Return_BTDeviceInitStatus(pFlyParrotInfo,FALSE);
		}
		break;
	case 0x03:
		break;
	case 0x10:
		if (0x01 == buf[1])
		{
			pFlyParrotInfo->controlPower = TRUE;
			pFlyParrotInfo->bPower =TRUE;
			SetEvent(pFlyParrotInfo->hParrotBTMainThreadEvent);
			Return_BTPowerStatus(pFlyParrotInfo,TRUE);
		}
		else
		{
			pFlyParrotInfo->controlPower = FALSE;
			pFlyParrotInfo->bPower =TRUE;
			SetEvent(pFlyParrotInfo->hParrotBTMainThreadEvent);
			Return_BTPowerStatus(pFlyParrotInfo,FALSE);
		}
		break;
	case 0x20:
		//全局
		pFlyParrotInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus = buf[1];
		IpcStartEvent(EVENT_GLOBAL_BTCALLSTATUS_CHANGE_ID);
		break;
	case 0x40:
		Control_WriteToBT(pFlyParrotInfo,&buf[1],len-1);
		break;
	case 0xFF:
		if (0x01 == buf[1])
		{
			FBT_PowerUp((DWORD)pFlyParrotInfo);
		} 
		else if (0x00 == buf[1])
		{
			FBT_PowerDown((DWORD)pFlyParrotInfo);
		}
	default:
		break;
	}
}
static VOID powerOnSpecialEnable(P_FLY_PARROT_INFO pFlyParrotInfo,BOOL bOn)
{
	if (bOn)
	{
		if (pFlyParrotInfo->bSpecialPower)
		{
			return;
		}
		pFlyParrotInfo->bSpecialPower = TRUE;
	} 
	else
	{
		if (!pFlyParrotInfo->bSpecialPower)
		{
			return;
		}
		pFlyParrotInfo->bSpecialPower = FALSE;
	}
}
static VOID powerOnFirstInit(P_FLY_PARROT_INFO pFlyParrotInfo)
{
	pFlyParrotInfo->bOpen = FALSE;
	pFlyParrotInfo->bPower = FALSE;
	pFlyParrotInfo->bSpecialPower = FALSE;
	pFlyParrotInfo->bPowerUp = FALSE;

	pFlyParrotInfo->bKillParrotBTMainThread = TRUE;
	pFlyParrotInfo->hParrotBTMainThread = NULL;
	pFlyParrotInfo->hParrotBTMainThreadEvent = NULL;

	pFlyParrotInfo->hBTComm = NULL;
	pFlyParrotInfo->bKillFlyBTCommThread = TRUE;
	pFlyParrotInfo->hThreadHandleFlyBTComm = NULL;
}
static VOID powerNormalInit(P_FLY_PARROT_INFO pFlyParrotInfo)
{
	pFlyParrotInfo->iBTInfoFrameStatus = 0;

	FlyBT_PowerControl_Off(pFlyParrotInfo);
	FlyBT_ChipEnableControl_Off(pFlyParrotInfo);
	FlyBT_ResetControl_On(pFlyParrotInfo);
}

void IpcRecv(UINT32 iEvent)
{
	P_FLY_PARROT_INFO	pFlyParrotInfo = (P_FLY_PARROT_INFO)gpFlyParrotInfo;

	DBGD((TEXT("\r\n FLY parrot Recv IPC Event:%d"),iEvent));


	SetEvent(pFlyParrotInfo->hParrotBTMainThreadEvent);
}

/********************************************************************************************************
*函数名称：HANDLE FBT_Init()
*函数功能：加载设备，在设备被加载的时候调用
*输入参数：
*输出参数：
*返 回 值：
**********************************************************************************************************/
HANDLE FBT_Init(
				DWORD dwContext   //字符串，指向注册表中记录活动驱动程序的键
				)
{
	RETAILMSG(1, (TEXT("\r\nParrot Init Start")));


	P_FLY_PARROT_INFO pFlyParrotInfo = (P_FLY_PARROT_INFO)LocalAlloc(LPTR, sizeof(FLY_PARROT_INFO));
	if (!pFlyParrotInfo)
	{
		RETAILMSG(1, (TEXT("FlyAudio Parrot LocalAlloc Failed!\r\n")));
		return NULL;
	}
	gpFlyParrotInfo = pFlyParrotInfo;

	pFlyParrotInfo->pFlyShmGlobalInfo = CreateShm(BT_MODULE,IpcRecv);
	if (NULL == pFlyParrotInfo->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio parrot Init create shm err\r\n")));
		return NULL;
	}

	if (!GetDllAddrTable())
	{
		DBGE((TEXT("FlyAudio parrot  GetDllAddrTable err\r\n")));
		return NULL;
	}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio parrot Open create user buf err\r\n")));
		return NULL;
	}

	pFlyParrotInfo->controlPower = FALSE;
	pFlyParrotInfo->currentPower = FALSE;

	//参数初始化
	powerNormalInit(pFlyParrotInfo);
	powerOnFirstInit(pFlyParrotInfo);
	powerOnSpecialEnable(pFlyParrotInfo,TRUE);

	DBGI((TEXT("\r\nCollexBT Init Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return (HANDLE)pFlyParrotInfo;
}
/********************************************************************************************************
*函数名称：DWORD FBT_Open()
*函数功能：释放设备，在设备被卸载的时候调用
*输入参数：
*输出参数：
*返 回 值：返回设备卸载是否成功
**********************************************************************************************************/
BOOL FBT_Deinit(
				DWORD hDeviceContext		//XXX_Init()函数返回的设备上下文
				)
{
	P_FLY_PARROT_INFO pFlyParrotInfo = (P_FLY_PARROT_INFO)hDeviceContext;
	
	powerOnSpecialEnable(pFlyParrotInfo,FALSE);

	FreeShm();
	FreeUserBuff();

	LocalFree(pFlyParrotInfo);
	RETAILMSG(1, (TEXT("FlyAudio Parrot LocalFree!\r\n")));
	return TRUE;
}

/********************************************************************************************************
*函数名称：DWORD FBT_Open()
*函数功能：打开设备进行读写 与应用程序的CreateFile()函数对应
*输入参数：
*输出参数：
*返 回 值：返回设备的打开上下文
**********************************************************************************************************/
DWORD FBT_Open(
			   DWORD hDeviceContext,	//设备上下文，由XXX_Init()函数创建
			   DWORD AccessCode,		//设备的访问模式，从CreateFile()函数传入
			   DWORD ShareMode			//设备的共享模式，从CreateFile()函数传入
			   )
{
	P_FLY_PARROT_INFO pFlyParrotInfo = (P_FLY_PARROT_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;
	
	if (pFlyParrotInfo->bOpen)
	{
		return NULL;
	}
	pFlyParrotInfo->bOpen = TRUE;

	collexBT_Enable(pFlyParrotInfo,TRUE);


	RETAILMSG(1, (TEXT("FlyAudio Parrot Open Build:Date%d.%d.%dTime%d:%d:%d\r\n"),
		year,months,day,hours,minutes,seconds));

	return returnWhat;
}

/********************************************************************************************************
*函数名称：VOID FBT_PowerUp()
*函数功能：关闭设备 与应用程序的CloseHandle()函数对应
*输入参数：
*输出参数：
*返 回 值：返回设备关闭是否成功
**********************************************************************************************************/
BOOL FBT_Close(
			   DWORD hDeviceContext		//设备的打开上下文，由XXX_Open（）函数返回
			   )
{
	P_FLY_PARROT_INFO pFlyParrotInfo = (P_FLY_PARROT_INFO)hDeviceContext;

	if (!pFlyParrotInfo->bOpen)
	{
		return FALSE;
	}
	pFlyParrotInfo->bOpen = FALSE;

	RETAILMSG(1, (TEXT("FlyAudio Parrot Close!\r\n")));
	return TRUE;
}
/********************************************************************************************************
*函数名称：VOID FBT_PowerUp()
*函数功能：电源挂起
*输入参数：
*输出参数：
*返 回 值：无
**********************************************************************************************************/
VOID FBT_PowerUp(DWORD hDeviceContext)
{
	P_FLY_PARROT_INFO pFlyParrotInfo = (P_FLY_PARROT_INFO)hDeviceContext;
	
	if (pFlyParrotInfo->bOpen)
	{
		
	}
	
	RETAILMSG(1, (TEXT("FlyAudio Parrot PowerUp!\r\n")));
}
/********************************************************************************************************
*函数名称：VOID FBT_PowerDown()
*函数功能：电源挂起
*输入参数：
*输出参数：
*返 回 值：无
**********************************************************************************************************/
VOID FBT_PowerDown(DWORD hDeviceContext)
{
	P_FLY_PARROT_INFO pFlyParrotInfo = (P_FLY_PARROT_INFO)hDeviceContext;

	if (pFlyParrotInfo->bOpen)
	{
		
	}
	RETAILMSG(1, (TEXT("FlyAudio Parrot PowerDown!\r\n")));
}
/********************************************************************************************************
*函数名称：DWORD FBT_Write()
*函数功能：从设备中读取数据  与应用程序ReadFile()函数据对应
*输入参数：
*输出参数：
*返 回 值：返回0表示文件结束，返回-1表示失败,返回读取的字节数表示成功
**********************************************************************************************************/
DWORD FBT_Read(
			   DWORD  hOpenContext,		//XXX_Open（）返回的设备打开上下文
			   LPVOID pBuffer,			//输出，缓冲区的指针，读取数据会被放在该缓冲区内
			   DWORD  Count				//要读取的字节数
			   )
{
	P_FLY_PARROT_INFO pFlyParrotInfo = (P_FLY_PARROT_INFO)hOpenContext;
	DWORD dwRead = 0,i=0;

	BYTE *buf = (BYTE*)pBuffer;
	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio parrot return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}

	return dwRead;
}
/********************************************************************************************************
*函数名称：DWORD FBT_Write()
*函数功能：向设备中写入数据  与应用程序WriteFile()函数据对应
*输入参数：
*输出参数：
*返 回 值：返回-1表示失败，返回写入的字节数表示成功
**********************************************************************************************************/
DWORD FBT_Write(
				DWORD    hOpenContext,		// XXX_Open返回的设备打开上下文
				LPCVOID  pSourceBytes,		//输入，指向要写入设备的数据的缓冲
				DWORD    NumberOfBytes		//缓冲中的数据的字节数
				)
{
	P_FLY_PARROT_INFO pFlyParrotInfo = (P_FLY_PARROT_INFO)hOpenContext;

	RETAILMSG(1, (TEXT("\r\nFlyAudio Parrot Write %d "), NumberOfBytes));
	for(UINT i = 0;i < NumberOfBytes;i++)
	{
		RETAILMSG(1, (TEXT(" %X"), *((BYTE *)pSourceBytes + i)));
	}
	if(NumberOfBytes)
	{
		FlyBT_WinCECommandProcessor(pFlyParrotInfo,(((BYTE *)pSourceBytes)+3), ((BYTE *)pSourceBytes)[2]-1);
	}

	return NULL;
}
/********************************************************************************************************
*函数名称：DWORD FBT_Seek()
*函数功能：移动设备中的数据指针  与应用程序SetFilePointer()函数据对应
*输入参数：
*输出参数：
*返 回 值：返回数据的新指针位置，-1表示失败
**********************************************************************************************************/
DWORD FBT_Seek(
			   DWORD  hOpenContext,		//XXX_Open()返回的设备打开上下文
			   LONG   Amount,			//要移动的距离，负数表示前移，正数表示后移
			   DWORD  Type				//缓冲中的数据的字节数
			   )
{
	RETAILMSG(1, (TEXT("FlyAudio Parrot Seek!\r\n")));
	return NULL;
}
/********************************************************************************************************
*函数名称：
*函数功能：向驱动程序发送控制命令  与应用程序DeviceIoControl()函数据对应
*输入参数：
*输出参数：
*返 回 值：布尔值：TRUE表示操作成功，FALSE表示操作失败
**********************************************************************************************************/
BOOL FBT_IOControl(
				   DWORD  hOpenContext,		//由XXX_Open()返回的设备打开上下文
				   DWORD  dwCode,			//要发送的控制码，一个32位无符号数
				   PBYTE  pBufIn,			//输入，指向输入缓冲区的指针
				   DWORD  dwLenIn,			//输入缓冲区的长度
				   PBYTE  pBufOut,			//输出，指向输出缓冲区的指针
				   DWORD  dwLenOut,			//输出缓冲区的最大长度
				   PDWORD pdwActualOut		//输出，设备实际输出的字节数
				   )
{
	P_FLY_PARROT_INFO pFlyParrotInfo = (P_FLY_PARROT_INFO)hOpenContext;
	BOOL bRes = TRUE;

	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	DBGD((TEXT("\r\nFlyAudio Parrot IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)"),
	//		hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	DBGD((TEXT("\r\nFlyAudio Parrot IOControl Set IOCTL_SERIAL_SET_WAIT_MASK")));
	//		break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	//DBGD((TEXT("\r\nFlyAudio Parrot IOControl Set IOCTL_SERIAL_WAIT_ON_MASK")));
	//	if (pFlyParrotInfo->bOpen)
	//	{
	//		WaitForSingleObject(pFlyParrotInfo->hBuffToUserDataEvent,INFINITE);
	//	} 
	//	else
	//	{
	//		WaitForSingleObject(pFlyParrotInfo->hBuffToUserDataEvent,0);
	//	}
	//	if ((dwLenOut<sizeof(DWORD)) || (NULL==pBufOut) || (NULL==pdwActualOut))
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
*函数名称：	DllEntry()
*函数功能： 动态链接库的入口，每个动态链接库都需要输出这个函数，它只在动态库被加载和卸
载时被调用，也就是设备管理器调用LoadLibrary而引起它被装入内存和调用UnloadLibrary将
其从内存释放时被调用，因而它是每个动态链接库最早被调用的函数，一般用它做一些全局变量
的初始化
*输入参数：
*输出参数：
*返 回 值：布尔值：TRUE表示操作成功，FALSE表示操作失败
*************************************************************************************************************/
BOOL DllEntry(
			  HINSTANCE hinstDll,	//DLL的句柄，与一个EXE文件的句柄功能类似，一般可以通过它在得到DLL中的一
									//些资源，例如对话框，除此之外一般没什么用处。
			  DWORD dwReason,		//DLL_PROCESS_ATTACH :动态库被加载，
									//DLL_PROCESS_DETACH :动态库被释放。
			  LPVOID lpReserved		//
			  )
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		RETAILMSG(1,(TEXT("Attach in FlyAudio parrot DllEntry")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		RETAILMSG(1,(TEXT("Dettach in FlyAudio parrot DllEntry")));
	}

	RETAILMSG(1,(TEXT("Leaving in FlyAudio parrot DllEntry")));

	return (TRUE);
}
