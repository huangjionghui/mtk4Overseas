/***************************************************************************************************
**�����������ɸ�7500�°�
**���������� Parrot����
**�ļ����ƣ� FLYParrot.cpp
**�汾��Ϣ�� 
**����ʱ�䣺 2011-05-13
**������  �� 
**--------------------------------------------------------------------------------------------------
**--------------------------------------------------------------------------------------------------
**�� �� �ˣ�Huangjionghui
**�޸�ʱ�䣺2011-05-17	            
**�޸����ݣ� ʵ�� 
**--------------------------------------------------------------------------------------------------
**�� �� �ˣ�Huangjionghui
**�޸�ʱ�䣺2011-05-23	            
**�޸����ݣ�1������ȫ�ֵ�״̬
			2���޸������ĳ�ʼ����ֵ
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

	//�رմ���
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
			//���ܴӴ��ڶ�ȡ����
			DBGI((TEXT("\r\nFlyAudio parrot ComProcThread cannot read data!")));
		}
		else
		{
			DBGD((TEXT("\r\nParrot Serial Read %d bytes:"),ret));
			for (UINT i = 0;i < ret;i++)
			{
				DBGD((TEXT(" %x"),recvBuf[i]));
			}

			//�����ݸ�ֵ��ȫ�ֱ���
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
				if((lastData == 0x0D && buf[i] == 0x0A))//�����β
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

			if (pFlyParrotInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)//��Դ����
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

			if (pFlyParrotInfo->bPower)//��λ
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
		//ȫ��
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
*�������ƣ�HANDLE FBT_Init()
*�������ܣ������豸�����豸�����ص�ʱ�����
*���������
*���������
*�� �� ֵ��
**********************************************************************************************************/
HANDLE FBT_Init(
				DWORD dwContext   //�ַ�����ָ��ע����м�¼���������ļ�
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

	//������ʼ��
	powerNormalInit(pFlyParrotInfo);
	powerOnFirstInit(pFlyParrotInfo);
	powerOnSpecialEnable(pFlyParrotInfo,TRUE);

	DBGI((TEXT("\r\nCollexBT Init Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return (HANDLE)pFlyParrotInfo;
}
/********************************************************************************************************
*�������ƣ�DWORD FBT_Open()
*�������ܣ��ͷ��豸�����豸��ж�ص�ʱ�����
*���������
*���������
*�� �� ֵ�������豸ж���Ƿ�ɹ�
**********************************************************************************************************/
BOOL FBT_Deinit(
				DWORD hDeviceContext		//XXX_Init()�������ص��豸������
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
*�������ƣ�DWORD FBT_Open()
*�������ܣ����豸���ж�д ��Ӧ�ó����CreateFile()������Ӧ
*���������
*���������
*�� �� ֵ�������豸�Ĵ�������
**********************************************************************************************************/
DWORD FBT_Open(
			   DWORD hDeviceContext,	//�豸�����ģ���XXX_Init()��������
			   DWORD AccessCode,		//�豸�ķ���ģʽ����CreateFile()��������
			   DWORD ShareMode			//�豸�Ĺ���ģʽ����CreateFile()��������
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
*�������ƣ�VOID FBT_PowerUp()
*�������ܣ��ر��豸 ��Ӧ�ó����CloseHandle()������Ӧ
*���������
*���������
*�� �� ֵ�������豸�ر��Ƿ�ɹ�
**********************************************************************************************************/
BOOL FBT_Close(
			   DWORD hDeviceContext		//�豸�Ĵ������ģ���XXX_Open������������
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
*�������ƣ�VOID FBT_PowerUp()
*�������ܣ���Դ����
*���������
*���������
*�� �� ֵ����
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
*�������ƣ�VOID FBT_PowerDown()
*�������ܣ���Դ����
*���������
*���������
*�� �� ֵ����
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
*�������ƣ�DWORD FBT_Write()
*�������ܣ����豸�ж�ȡ����  ��Ӧ�ó���ReadFile()�����ݶ�Ӧ
*���������
*���������
*�� �� ֵ������0��ʾ�ļ�����������-1��ʾʧ��,���ض�ȡ���ֽ�����ʾ�ɹ�
**********************************************************************************************************/
DWORD FBT_Read(
			   DWORD  hOpenContext,		//XXX_Open�������ص��豸��������
			   LPVOID pBuffer,			//�������������ָ�룬��ȡ���ݻᱻ���ڸû�������
			   DWORD  Count				//Ҫ��ȡ���ֽ���
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
*�������ƣ�DWORD FBT_Write()
*�������ܣ����豸��д������  ��Ӧ�ó���WriteFile()�����ݶ�Ӧ
*���������
*���������
*�� �� ֵ������-1��ʾʧ�ܣ�����д����ֽ�����ʾ�ɹ�
**********************************************************************************************************/
DWORD FBT_Write(
				DWORD    hOpenContext,		// XXX_Open���ص��豸��������
				LPCVOID  pSourceBytes,		//���룬ָ��Ҫд���豸�����ݵĻ���
				DWORD    NumberOfBytes		//�����е����ݵ��ֽ���
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
*�������ƣ�DWORD FBT_Seek()
*�������ܣ��ƶ��豸�е�����ָ��  ��Ӧ�ó���SetFilePointer()�����ݶ�Ӧ
*���������
*���������
*�� �� ֵ���������ݵ���ָ��λ�ã�-1��ʾʧ��
**********************************************************************************************************/
DWORD FBT_Seek(
			   DWORD  hOpenContext,		//XXX_Open()���ص��豸��������
			   LONG   Amount,			//Ҫ�ƶ��ľ��룬������ʾǰ�ƣ�������ʾ����
			   DWORD  Type				//�����е����ݵ��ֽ���
			   )
{
	RETAILMSG(1, (TEXT("FlyAudio Parrot Seek!\r\n")));
	return NULL;
}
/********************************************************************************************************
*�������ƣ�
*�������ܣ������������Ϳ�������  ��Ӧ�ó���DeviceIoControl()�����ݶ�Ӧ
*���������
*���������
*�� �� ֵ������ֵ��TRUE��ʾ�����ɹ���FALSE��ʾ����ʧ��
**********************************************************************************************************/
BOOL FBT_IOControl(
				   DWORD  hOpenContext,		//��XXX_Open()���ص��豸��������
				   DWORD  dwCode,			//Ҫ���͵Ŀ����룬һ��32λ�޷�����
				   PBYTE  pBufIn,			//���룬ָ�����뻺������ָ��
				   DWORD  dwLenIn,			//���뻺�����ĳ���
				   PBYTE  pBufOut,			//�����ָ�������������ָ��
				   DWORD  dwLenOut,			//�������������󳤶�
				   PDWORD pdwActualOut		//������豸ʵ��������ֽ���
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
*�������ƣ�	DllEntry()
*�������ܣ� ��̬���ӿ����ڣ�ÿ����̬���ӿⶼ��Ҫ��������������ֻ�ڶ�̬�ⱻ���غ�ж
��ʱ�����ã�Ҳ�����豸����������LoadLibrary����������װ���ڴ�͵���UnloadLibrary��
����ڴ��ͷ�ʱ�����ã��������ÿ����̬���ӿ����类���õĺ�����һ��������һЩȫ�ֱ���
�ĳ�ʼ��
*���������
*���������
*�� �� ֵ������ֵ��TRUE��ʾ�����ɹ���FALSE��ʾ����ʧ��
*************************************************************************************************************/
BOOL DllEntry(
			  HINSTANCE hinstDll,	//DLL�ľ������һ��EXE�ļ��ľ���������ƣ�һ�����ͨ�����ڵõ�DLL�е�һ
									//Щ��Դ������Ի��򣬳���֮��һ��ûʲô�ô���
			  DWORD dwReason,		//DLL_PROCESS_ATTACH :��̬�ⱻ���أ�
									//DLL_PROCESS_DETACH :��̬�ⱻ�ͷš�
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
