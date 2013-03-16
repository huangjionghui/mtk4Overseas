
#include "FLYCarRecord.h"
#include "flyseriallib.h"
#include "flyuserlib.h"


P_FLY_CARECORD_INFO gpFlyCarRecordInfo = NULL;

VOID Record_info_init(P_FLY_CARECORD_INFO pFlyCarRecordInfo);


void returnCarRecordPowerMode(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BOOL bOn)
{
	BYTE buf[2] = {0x01,0x00};

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

void returnCarRecordInitStatus(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BOOL bOn)
{
	BYTE buf[2] = {0x02,0x00};

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

void returnCarRecordExist(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BOOL bexist)
{
	BYTE buf[2] = {0x03,0x00};

	if (bexist)
	{
		buf[1] = 1;
	}
	else
	{
		buf[1] = 0;
	}

	 ReturnToUser(buf,2);
}

void returnCarRecordState(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BYTE eCarRecordState)
{
	BYTE buf[2] = {0x10,0x00};

	buf[1] = eCarRecordState;

	RETAILMSG(1, (TEXT("\r\n333333333333333333send key: %d"),eCarRecordState));
	 ReturnToUser(buf,2);
}

void returnCarRecordResolution(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BYTE eRecordResolution)
{
	BYTE buf[2] = {0x11,0x00};

	buf[1] = eRecordResolution;

	 ReturnToUser(buf,2);
}

void returnCarRecordLanguage(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BYTE eRecordLanguage)
{
	BYTE buf[2] = {0x12,0x00};

	buf[1] = eRecordLanguage;

	 ReturnToUser(buf,2);
}

void returnCarRecordVideo_Record(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BOOL bVideo_Record)
{
	BYTE buf[2] = {0x14,0x00};

	buf[1] = bVideo_Record;

	 ReturnToUser(buf,2);
}

void returnCarRecordSound_Record(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BOOL bSound_Record)
{
	BYTE buf[2] = {0x15,0x00};

	buf[1] = bSound_Record;

	 ReturnToUser(buf,2);
}

void returnCarRecord_UpdataStatus(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BYTE UpdataStatus)
{
	BYTE buf[2] = {0x16,0x00};

	buf[1] = UpdataStatus;

	 ReturnToUser(buf,2);
}

void returnCarRecord_EnterState(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BYTE state)
{
	BYTE buf[2] = {0x17,0x00};

	buf[1] = state;

	 ReturnToUser(buf,2);
}

static VOID write_data_to_com(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BYTE* buffer,BYTE length)
{
	WriteSerial(pFlyCarRecordInfo->hCarRecordCom,buffer, length);
}

static VOID CarRecord_Set_Video_Record(P_FLY_CARECORD_INFO pFlyCarRecordInfo)
{
	BYTE buf[10] = {'$','S','E','T',',','O','0','0','\r','\n'};

	write_data_to_com(pFlyCarRecordInfo,buf,10);
}

static VOID CarRecord_Set_Sound_Record(P_FLY_CARECORD_INFO pFlyCarRecordInfo)
{
	BYTE buf[10] = {'$','S','E','T',',','V','0','0','\r','\n'};

	write_data_to_com(pFlyCarRecordInfo,buf,10);
}

static VOID CarRecord_Send_Key(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BYTE Key)
{
	BYTE buf[10] = {'$','S','E','T',',',0,'0','0','\r','\n'};

	buf[5] = Key;

	write_data_to_com(pFlyCarRecordInfo,buf,10);
}

static VOID CarRecord_Set_Record_Resolution(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BYTE resolution)
{
	BYTE buf[10] = {'$','S','E','T',',',0,0,0,'\r','\n'};

	buf[5] = 'A';
	if (resolution == HD)
	{
		buf[6] = '0';
		buf[7] = '0';
	}
	else if (resolution == VGA)
	{
		buf[6] = '0';
		buf[7] = '1';
	}
	else if (resolution == QVGA)
	{
		buf[6] = '0';
		buf[7] = '2';
	}

	write_data_to_com(pFlyCarRecordInfo,buf,10);
}

static VOID CarRecord_Set_Record_Language(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BYTE eRecordLanguage)
{
	BYTE buf[10] = {'$','S','E','T',',',0,0,0,'\r','\n'};

	buf[5] = 'T';
	if (eRecordLanguage == ENGLISH)
	{
		buf[6] = '0';
		buf[7] = '0';
	}
	else if (eRecordLanguage == SIMPLIFIED_CHINESE)
	{
		buf[6] = '0';
		buf[7] = '1';
	}
	else if (eRecordLanguage == RUSSIAN)
	{
		buf[6] = '0';
		buf[7] = '9';
	}

	write_data_to_com(pFlyCarRecordInfo,buf,10);
}
static void CarRecord_Set_Record_Timer(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BYTE *buff,UINT16 len)
{
	if (len != 6)
	{
		DBGE((TEXT("\r\n CarRecord_Set_Record_Timer not len\r\n")));
		return;
	}

	BYTE buf[] = {'$','S','E','T',',','Y','0','0','0','0','0','0','0','0','0','0','0','0','\r','\n'};
	buf[6] += (buff[0]/10);
	buf[7] += (buff[0]%10);//Y
	buf[8] += (buff[1]/10);
	buf[9] += (buff[1]%10);//M
	buf[10] += (buff[2]/10);
	buf[11] += (buff[2]%10);//D

	buf[12] += (buff[3]/10);
	buf[13] += (buff[3]%10);//H
	buf[14] += (buff[4]/10);
	buf[15] += (buff[4]%10);//M
	buf[16] += (buff[5]/10);
	buf[17] += (buff[5]%10);//S

	write_data_to_com(pFlyCarRecordInfo,buf,20);
}

void OnDataRecv(P_FLY_CARECORD_INFO pFlyCarRecordInfo, BYTE *buf)
{
	pFlyCarRecordInfo->iReceiveCarRecordMessageTime = GetTickCount();
	if(FALSE == pFlyCarRecordInfo->bHaveCarRecord)
	{
		RETAILMSG(1, (TEXT("\r\n222222222222222222222222222222222222222222222222")));
		pFlyCarRecordInfo->bHaveCarRecord = TRUE;
		returnCarRecordExist(pFlyCarRecordInfo,pFlyCarRecordInfo->bHaveCarRecord);
		CarRecord_Send_Key(pFlyCarRecordInfo,'M');
	}

	switch (buf[0])
	{
	//case 'S':
		//if ('H' == buf[1])
		//{
		//	pFlyCarRecordInfo->bHaveCarRecord = TRUE;
		//	returnCarRecordExist(pFlyCarRecordInfo,pFlyCarRecordInfo->bHaveCarRecord);
		//	CarRecord_Send_Key(pFlyCarRecordInfo,'M');
		//}
		//break;
	case 'I':
	case 'Z':
	case 'G':
	case 'C':
		if (buf[1]=='1' && buf[2]=='0')
		{
			pFlyCarRecordInfo->CarRecordInfo.epreRecordResolution = HD;
		}
		else if(buf[1]=='1' && buf[2]=='1')
		{
			pFlyCarRecordInfo->CarRecordInfo.epreRecordResolution = VGA;
		}
		else
		{
			pFlyCarRecordInfo->CarRecordInfo.epreRecordResolution = QVGA;
		}

		if ('I' == buf[0])
		{
			pFlyCarRecordInfo->CarRecordInfo.epreCarRecordState = RECORD;

			pFlyCarRecordInfo->CarRecordInfo.ecurCarRecordState = pFlyCarRecordInfo->CarRecordInfo.epreCarRecordState;
			returnCarRecordState(pFlyCarRecordInfo,pFlyCarRecordInfo->CarRecordInfo.ecurCarRecordState);
		}
		else if ('Z' == buf[0])
		{
			pFlyCarRecordInfo->CarRecordInfo.epreCarRecordState = PLAYBACK;

			pFlyCarRecordInfo->CarRecordInfo.ecurCarRecordState = pFlyCarRecordInfo->CarRecordInfo.epreCarRecordState;
			returnCarRecordState(pFlyCarRecordInfo,pFlyCarRecordInfo->CarRecordInfo.ecurCarRecordState);
		}
		else if('C' == buf[0])
		{
			pFlyCarRecordInfo->CarRecordInfo.epreCarRecordState = VIDEO_SETTING;

			pFlyCarRecordInfo->CarRecordInfo.ecurCarRecordState = pFlyCarRecordInfo->CarRecordInfo.epreCarRecordState;
			returnCarRecordState(pFlyCarRecordInfo,pFlyCarRecordInfo->CarRecordInfo.ecurCarRecordState);
		}	
		else if('G' == buf[0])
		{
			pFlyCarRecordInfo->CarRecordInfo.epreCarRecordState = TIME_SETTING;

			pFlyCarRecordInfo->CarRecordInfo.ecurCarRecordState = pFlyCarRecordInfo->CarRecordInfo.epreCarRecordState;
			returnCarRecordState(pFlyCarRecordInfo,pFlyCarRecordInfo->CarRecordInfo.ecurCarRecordState);
		}	
		break;
	case 'O':
		if (buf[1]=='1' && buf[2]=='0')
		{
			pFlyCarRecordInfo->CarRecordInfo.bpreVideo_Recording = FALSE;
		}
		else if (buf[1]=='1' && buf[2]=='1')
		{
			pFlyCarRecordInfo->CarRecordInfo.bpreVideo_Recording = TRUE;
		}	

		pFlyCarRecordInfo->CarRecordInfo.bcurVideo_Recording = pFlyCarRecordInfo->CarRecordInfo.bpreVideo_Recording;
		returnCarRecordVideo_Record(pFlyCarRecordInfo,pFlyCarRecordInfo->CarRecordInfo.bcurVideo_Recording);

		break;
	case 'V':
		if (buf[1]=='1' && buf[2]=='0')
		{
			pFlyCarRecordInfo->CarRecordInfo.bpreSound_Recording = FALSE;
		}
		else if (buf[1]=='1' && buf[2]=='1')
		{
			pFlyCarRecordInfo->CarRecordInfo.bpreSound_Recording = TRUE;
		}	

		pFlyCarRecordInfo->CarRecordInfo.bcurSound_Recording = pFlyCarRecordInfo->CarRecordInfo.bpreSound_Recording;
		returnCarRecordSound_Record(pFlyCarRecordInfo,pFlyCarRecordInfo->CarRecordInfo.bcurSound_Recording);

		break;
	case 'N':
		if (buf[1]=='1' && buf[2]=='0')
		{
			pFlyCarRecordInfo->CarRecordInfo.epreRecordLanguage = ENGLISH;
		}
		else if (buf[1]=='1' && buf[2]=='1')
		{
			pFlyCarRecordInfo->CarRecordInfo.epreRecordLanguage = SIMPLIFIED_CHINESE;
		}
		else if (buf[1]=='1' && buf[2]=='3')
		{
			pFlyCarRecordInfo->CarRecordInfo.epreRecordLanguage = TRADITIONAL_CHINESE;
		}
		else if (buf[1]=='1' && buf[2]=='9')
		{
			pFlyCarRecordInfo->CarRecordInfo.epreRecordLanguage = RUSSIAN;
		}
		break;
	case 'A':
		if (buf[1]=='1' && buf[2]=='0')
		{
			pFlyCarRecordInfo->CarRecordInfo.epreRecordResolution = HD;
		}
		else if (buf[1]=='1' && buf[2]=='1')
		{
			pFlyCarRecordInfo->CarRecordInfo.epreRecordResolution = VGA;
		}	
		else if (buf[1]=='1' && buf[2]=='2')
		{
			pFlyCarRecordInfo->CarRecordInfo.epreRecordResolution = QVGA;
		}
		break;
	case 'U':
		if (buf[1]=='1'&& buf[2]=='0')
		{
			returnCarRecord_UpdataStatus(pFlyCarRecordInfo,NO_UPDATA_FILE);
		}
		else if(buf[1]=='1'&& buf[2]=='1')
		{
			returnCarRecord_UpdataStatus(pFlyCarRecordInfo,UPDATAING);
		}
		else if(buf[1]=='1'&& buf[2]=='2')
		{
			returnCarRecord_UpdataStatus(pFlyCarRecordInfo,UPDATA_SUCCESS);
		}
		break;
	}

	SetEvent(pFlyCarRecordInfo->hFlyCarRecordMainThreadEvent);
}

void OnCommRecv(P_FLY_CARECORD_INFO pFlyCarRecordInfo, BYTE *buf, UINT buflen)
{
	BYTE i;

	for (i=0;i<buflen;i++)
	{
		switch (pFlyCarRecordInfo->iComDataState)
		{
		case 0:
			if (buf[i] == '$')
			{
				pFlyCarRecordInfo->iComDataState = 1;
			}
			else
			{
				pFlyCarRecordInfo->iComDataState = 0;
			}
			break;
		case 1:
			if (buf[i] == 'S')
			{
				pFlyCarRecordInfo->iComDataState = 2;
			}
			else
			{
				pFlyCarRecordInfo->iComDataState = 0;
			}
			break;
		case 2:
			if (buf[i] == 'E')
			{
				pFlyCarRecordInfo->iComDataState = 3;
			}
			else
			{
				pFlyCarRecordInfo->iComDataState = 0;
			}
			break;
		case 3:
			if (buf[i] == 'T')
			{
				pFlyCarRecordInfo->iComDataState = 4;
			}
			else
			{
				pFlyCarRecordInfo->iComDataState = 0;
			}
			break;
		case 4:
			if (buf[i] == ',')
			{
				pFlyCarRecordInfo->iComDataState = 5;
			}
			else
			{
				pFlyCarRecordInfo->iComDataState = 0;
			}
			break;
		case 5:
			if (buf[i] != '\r')
			{
				pFlyCarRecordInfo->Com_Data[pFlyCarRecordInfo->Com_Data_Count++] = buf[i];
				if (pFlyCarRecordInfo->Com_Data_Count == 10) //数据有错
				{
					pFlyCarRecordInfo->iComDataState = 0;
					pFlyCarRecordInfo->Com_Data_Count = 0;
					memset(pFlyCarRecordInfo->Com_Data,0,sizeof(pFlyCarRecordInfo->Com_Data));
				}
			}
			else
			{
				pFlyCarRecordInfo->iComDataState = 6;
			}
			break;
		case 6:
			if (buf[i] == '\n')
			{
				UINT j;
				for (j = 0;j < pFlyCarRecordInfo->Com_Data_Count;j++)//跳过换行符
				{
					RETAILMSG(1, (TEXT(" %c"),pFlyCarRecordInfo->Com_Data[j]));
				}
				OnDataRecv(pFlyCarRecordInfo,pFlyCarRecordInfo->Com_Data);
			}
				
			pFlyCarRecordInfo->iComDataState = 0;
			pFlyCarRecordInfo->Com_Data_Count = 0;
			memset(pFlyCarRecordInfo->Com_Data,0,sizeof(pFlyCarRecordInfo->Com_Data));
			break;
		}
	}
}


static void DealBarCodeID(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BYTE data)//####################
{
	static unsigned char checksum;
	static unsigned char FrameLen;
	static unsigned char FrameStatus;
	static unsigned char FrameLenMax;
	static unsigned char ReceiveReg[256];

	switch (FrameStatus)
	{
	case 0:
		if (data == 0xff) 
		{
			FrameStatus = 1;
		}
		break;
	case 1:
		if (data == 0x55) 
		{
			FrameStatus = 2;
		}
		else 
		{
			FrameStatus = 0;
			pFlyCarRecordInfo->ExBoxInfoFrameStatus =0;

		}
		break;
	case 2:
		if(data == 0)
		{
			FrameStatus =0;
			pFlyCarRecordInfo->ExBoxInfoFrameStatus =0;
		}
		else
		{
			FrameStatus = 3;
			checksum = FrameLenMax = data;
			FrameLen = 0;
		}
		break;
	case 3:
		{
			ReceiveReg[FrameLen] = data;
			checksum += data;
			FrameLen ++;

			if(data == 0x15)
			{
				FrameStatus = 5;
				FrameLen =0;
				ReceiveReg[FrameLen++] =0xff;
				ReceiveReg[FrameLen++] =0x55;
				ReceiveReg[FrameLen++] =FrameLenMax;
				ReceiveReg[FrameLen++] =data;
				FrameLenMax += 3;

			}
			else
			{
				FrameStatus =4;
			}
		}
		break;
	case 4:
		if(FrameLen < (FrameLenMax))
		{
			ReceiveReg[FrameLen] = data;
			checksum += data;
			FrameLen ++;
		}
		else
		{
			if((BYTE)(0x100-data) == checksum)
			{
				ReceiveReg[FrameLen] = 0;
				pFlyCarRecordInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.BarCodeID[0]=FrameLen-2;
				memcpy((void*)&pFlyCarRecordInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.BarCodeID[1],&ReceiveReg[2],FrameLen-2);
				IpcStartEvent(EVENT_GLOBAL_RETURN_BAR_CODE_ID);

				DBGD((TEXT("\r\nFlyAudio BarCode ID :")));

				for(BYTE i=0;i<pFlyCarRecordInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.BarCodeID[0];i++)
				{
					DBGD((TEXT("%d  "),pFlyCarRecordInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.BarCodeID[i]));		
				}

			}
			else
			{
				DBGD((TEXT("\r\nFlyAudio BarCode ID CRC ERROR")));
			}
			FrameStatus = 0;
			pFlyCarRecordInfo->ExBoxInfoFrameStatus =0;
		}
		break;
	default:	
		FrameStatus = 0;
		pFlyCarRecordInfo->ExBoxInfoFrameStatus =0;
		break;
	}
}

void OnCommRecv_For_BarCodeID(P_FLY_CARECORD_INFO pFlyCarRecordInfo, BYTE *buf, UINT buflen)
{
	for (UINT i = 0; i < buflen; i++)
	{
		switch (pFlyCarRecordInfo->ExBoxInfoFrameStatus)
		{
		case 0://分流
			if(buf[i] == 0xff)//条形码
			{
				pFlyCarRecordInfo->ExBoxInfoFrameStatus = 6;
				DealBarCodeID(pFlyCarRecordInfo,buf[i]);
			}
			else
			{
				pFlyCarRecordInfo->ExBoxInfoFrameStatus = 0;
			}
			break;
		case 6://条形码
			DealBarCodeID(pFlyCarRecordInfo,buf[i]);
			break;
		default:
			pFlyCarRecordInfo->ExBoxInfoFrameStatus = 0;
			break;
		}
	}
}


static DWORD WINAPI FlyCarRecord_ComProcThread(LPVOID pContext)
{
	P_FLY_CARECORD_INFO pFlyCarRecordInfo = (P_FLY_CARECORD_INFO)pContext;
	
	UINT32 ret = 0;
	BYTE *recvBuf = new BYTE[DATA_BUFF_COMM_LENGTH];
	BYTE *zeroBuf = new BYTE[DATA_BUFF_COMM_LENGTH];
	memset(zeroBuf,0,sizeof(BYTE)*DATA_BUFF_COMM_LENGTH);

	while (!pFlyCarRecordInfo->bKillFlyCarRecordComThread)
	{
		
		ret = ReadSerial(pFlyCarRecordInfo->hCarRecordCom,recvBuf,DATA_BUFF_COMM_LENGTH);
		if(ret <= 0)
		{
			//不能从串口读取数据
			DBGI((TEXT("\r\nFlyAudio CarRecord ComProcThread cannot read data!")));
		}
		else
		{
			DBGD((TEXT("\r\nCarRecor Serial Read %d bytes:"),ret));
			for (UINT i = 0;i < ret;i++)
			{
				DBGD((TEXT(" %x"),recvBuf[i]));
			}
			
			if (ret > 50)
			{
				//多判断几下
				if (!memcmp(zeroBuf,recvBuf,10) 
					|| (!memcmp(&zeroBuf[15],&recvBuf[15],10))
					|| (!memcmp(&zeroBuf[30],&recvBuf[30],10)))
				{
					DBGD((TEXT("\r\nCarRecor Zero buf,coutinue read\r\n")));
					continue;
				}
			}

			//把数据赋值给全局变量
			OnCommRecv(pFlyCarRecordInfo,recvBuf,ret);
			OnCommRecv_For_BarCodeID(pFlyCarRecordInfo,recvBuf,ret);
		}  
	}
	delete []recvBuf;
	delete []zeroBuf;
	DBGD((TEXT("\r\nFlyAudio CarRecord ComProcThread exit")));
	return 0;
}

static DWORD WINAPI FlyCarRecordMainThread(LPVOID pContext)
{
	P_FLY_CARECORD_INFO pFlyCarRecordInfo = (P_FLY_CARECORD_INFO)pContext;
	ULONG WaitReturn;


	ULONG iLastTime = GetTickCount();

	//ULONG run_count = 0;

	while (!pFlyCarRecordInfo->bKillFlyCarRecordMainThread)
	{
		WaitReturn = WaitForSingleObject(pFlyCarRecordInfo->hFlyCarRecordMainThreadEvent, 2000);
		if (WAIT_FAILED == WaitReturn)
		{
			RETAILMSG(1, (TEXT("\r\nFlyAudio Test hDispatchMainThreadEvent Error")));
		}

		if ((GetTickCount() - iLastTime) >= 2000)
		{
			//RETAILMSG(1, (TEXT("\r\n111111111111111111111111111111111")));
			iLastTime = GetTickCount();
			CarRecord_Send_Key(pFlyCarRecordInfo,'N');    //作为ping
		}

		RETAILMSG(1, (TEXT("\r\n------------------------------:%ld  %ld"),GetTickCount(),pFlyCarRecordInfo->iReceiveCarRecordMessageTime));

		if (pFlyCarRecordInfo->iReceiveCarRecordMessageTime != 0)
		{
			//RETAILMSG(1, (TEXT("\r\n2222222222222222222222222222222")));
			if ((GetTickCount() - pFlyCarRecordInfo->iReceiveCarRecordMessageTime) >= 6000)
			{
				//RETAILMSG(1, (TEXT("\r\n33333333333333333333333333333333333")));
				pFlyCarRecordInfo->iReceiveCarRecordMessageTime = 0;
				pFlyCarRecordInfo->bHaveCarRecord = FALSE;
				returnCarRecordExist(pFlyCarRecordInfo,pFlyCarRecordInfo->bHaveCarRecord);
				RETAILMSG(1, (TEXT("\r\n111111111111111111111111111111111111111111111111111")));

				Record_info_init(pFlyCarRecordInfo);
			}
		}

		if (pFlyCarRecordInfo->bHaveCarRecord)
		{
			//if (pFlyCarRecordInfo->CarRecordInfo.ecurCarRecordState != pFlyCarRecordInfo->CarRecordInfo.epreCarRecordState)
			//{
			//	pFlyCarRecordInfo->CarRecordInfo.ecurCarRecordState = pFlyCarRecordInfo->CarRecordInfo.epreCarRecordState;
			//	returnCarRecordState(pFlyCarRecordInfo,pFlyCarRecordInfo->CarRecordInfo.ecurCarRecordState);
			//}

			if (pFlyCarRecordInfo->CarRecordInfo.ecurRecordResolution != pFlyCarRecordInfo->CarRecordInfo.epreRecordResolution)
			{
				pFlyCarRecordInfo->CarRecordInfo.ecurRecordResolution = pFlyCarRecordInfo->CarRecordInfo.epreRecordResolution;
				returnCarRecordResolution(pFlyCarRecordInfo,pFlyCarRecordInfo->CarRecordInfo.ecurRecordResolution);
			}

			if (pFlyCarRecordInfo->CarRecordInfo.ecurRecordLanguage != pFlyCarRecordInfo->CarRecordInfo.epreRecordLanguage)
			{
				pFlyCarRecordInfo->CarRecordInfo.ecurRecordLanguage = pFlyCarRecordInfo->CarRecordInfo.epreRecordLanguage;
				returnCarRecordLanguage(pFlyCarRecordInfo,pFlyCarRecordInfo->CarRecordInfo.ecurRecordLanguage);
			}

			//if (pFlyCarRecordInfo->CarRecordInfo.bcurVideo_Recording != pFlyCarRecordInfo->CarRecordInfo.bpreVideo_Recording)
			//{
			//	pFlyCarRecordInfo->CarRecordInfo.bcurVideo_Recording = pFlyCarRecordInfo->CarRecordInfo.bpreVideo_Recording;
			//	returnCarRecordVideo_Record(pFlyCarRecordInfo,pFlyCarRecordInfo->CarRecordInfo.bcurVideo_Recording);
			//}

			//if (pFlyCarRecordInfo->CarRecordInfo.bcurSound_Recording != pFlyCarRecordInfo->CarRecordInfo.bpreSound_Recording)
			//{
			//	pFlyCarRecordInfo->CarRecordInfo.bcurSound_Recording = pFlyCarRecordInfo->CarRecordInfo.bpreSound_Recording;
			//	returnCarRecordSound_Record(pFlyCarRecordInfo,pFlyCarRecordInfo->CarRecordInfo.bcurSound_Recording);
			//}
		}
	}

	CloseHandle(pFlyCarRecordInfo->hFlyCarRecordMainThreadEvent);
	RETAILMSG(1, (TEXT("\r\nFlyAudio CarRecord ThreadMain exit")));
	return 0;
}

void FlyCarRecord_COM_Disable(P_FLY_CARECORD_INFO pFlyCarRecordInfo)
{
	pFlyCarRecordInfo->bKillFlyCarRecordComThread = TRUE;

	if (pFlyCarRecordInfo->hFlyCarRecordComThreadHandle)
	{
		CloseHandle(pFlyCarRecordInfo->hFlyCarRecordComThreadHandle);
		pFlyCarRecordInfo->hFlyCarRecordComThreadHandle= NULL;
	}

	if(pFlyCarRecordInfo->hCarRecordCom)
	{
		CloseSerial(pFlyCarRecordInfo->hCarRecordCom);
		pFlyCarRecordInfo->hCarRecordCom = NULL;
	}
}

void FlyCarRecord_COM_Enable(P_FLY_CARECORD_INFO pFlyCarRecordInfo)
{
	DWORD dwThreadID;

	if (pFlyCarRecordInfo->hCarRecordCom)
	{
		return;
	}
	//打开串口
	pFlyCarRecordInfo->hCarRecordCom =  OpenSerial(_T("COM5:"), 9600);
	if(pFlyCarRecordInfo->hCarRecordCom == INVALID_HANDLE_VALUE)
	{
		pFlyCarRecordInfo->hCarRecordCom = NULL;
		DBGI((TEXT("\nFlyAudio CarRecord cann't open com!")));
		return;
	}

	pFlyCarRecordInfo->bKillFlyCarRecordComThread = FALSE;
	pFlyCarRecordInfo->hFlyCarRecordComThreadHandle = 
		CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
		0,//初始化线程栈的大小，缺省为与主线程大小相同
		(LPTHREAD_START_ROUTINE)FlyCarRecord_ComProcThread, //线程的全局函数
		pFlyCarRecordInfo, //此处传入了主框架的句柄
		0, &dwThreadID );
	RETAILMSG(1, (TEXT("\r\nFlyAudio ComProcThread ID:%x"),dwThreadID));

	if (NULL == pFlyCarRecordInfo->hFlyCarRecordComThreadHandle)
	{
		DBGD((TEXT("FlyAudio CarRecord CreateThread ComProcThread!")));
		pFlyCarRecordInfo->bKillFlyCarRecordComThread = TRUE;
		FlyCarRecord_COM_Disable(pFlyCarRecordInfo);
		return;
	}
	RETAILMSG(1, (TEXT("FlyAudio CarRecord Comm init OK!")));
}

void FlyCarRecord_Enable(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BOOL bEnable)
{
	if (bEnable)
	{
		pFlyCarRecordInfo->bKillFlyCarRecordMainThread = FALSE;
		DWORD dwThreadID;
		pFlyCarRecordInfo->FlyCarRecordMainThreadHandle = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)FlyCarRecordMainThread, //线程的全局函数
			pFlyCarRecordInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		RETAILMSG(1, (TEXT("\r\nFlyCarRecordMainThread ID:%x"),dwThreadID));
		if (NULL == pFlyCarRecordInfo->FlyCarRecordMainThreadHandle)
		{
			pFlyCarRecordInfo->bKillFlyCarRecordMainThread = TRUE;
			return;
		}
		pFlyCarRecordInfo->hFlyCarRecordMainThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
	}
	else
	{
		pFlyCarRecordInfo->bKillFlyCarRecordMainThread = TRUE;
		SetEvent(pFlyCarRecordInfo->hFlyCarRecordMainThreadEvent);
	}
}

static VOID DealWinCECommandProcessor(P_FLY_CARECORD_INFO pFlyCarRecordInfo,BYTE *buff,UINT len)
{
	BYTE temp;
	switch (buff[0])
	{
	case 0x01:
		if (0x01 == buff[1])
		{
			returnCarRecordPowerMode(pFlyCarRecordInfo,TRUE);
			returnCarRecordInitStatus(pFlyCarRecordInfo,TRUE);
			returnCarRecordExist(pFlyCarRecordInfo,pFlyCarRecordInfo->bHaveCarRecord);

			returnCarRecordState(pFlyCarRecordInfo,pFlyCarRecordInfo->CarRecordInfo.ecurCarRecordState);
			returnCarRecordVideo_Record(pFlyCarRecordInfo,pFlyCarRecordInfo->CarRecordInfo.bcurVideo_Recording);
			returnCarRecordSound_Record(pFlyCarRecordInfo,pFlyCarRecordInfo->CarRecordInfo.bcurSound_Recording);
			returnCarRecordResolution(pFlyCarRecordInfo,pFlyCarRecordInfo->CarRecordInfo.ecurRecordResolution);
			returnCarRecordLanguage(pFlyCarRecordInfo,pFlyCarRecordInfo->CarRecordInfo.ecurRecordLanguage);
		}
		else if (0x00 == buff[1])
		{
			returnCarRecordPowerMode(pFlyCarRecordInfo,FALSE);
		}
		break;

	case 0x03:
		CarRecord_Send_Key(pFlyCarRecordInfo,buff[1]);
		break;

	case 0x10:
		if (0x00 == buff[1])
		{
			temp = 'I';
			CarRecord_Send_Key(pFlyCarRecordInfo,temp);
		}
		else if (0x01 == buff[1])
		{
			temp = 'Z';
			CarRecord_Send_Key(pFlyCarRecordInfo,temp);
		}
		else if (0x02 == buff[1])
		{
			temp = 'G';
			CarRecord_Send_Key(pFlyCarRecordInfo,temp);
		}
		else if (0x03 == buff[1])
		{
			temp = 'C';
			CarRecord_Send_Key(pFlyCarRecordInfo,temp);
		}
		break;
	case 0x11:
		if (pFlyCarRecordInfo->CarRecordInfo.ecurRecordResolution != buff[1])
		{
			CarRecord_Set_Record_Resolution(pFlyCarRecordInfo,buff[1]);
		}
		break;
	case 0x12:
		if (pFlyCarRecordInfo->CarRecordInfo.ecurRecordLanguage != buff[1])
		{
			CarRecord_Set_Record_Language(pFlyCarRecordInfo,buff[1]);
		}
		break;

	case 0x13:
		CarRecord_Set_Record_Timer(pFlyCarRecordInfo,&buff[1],len-1);
		break;
	}
}

static VOID First_init(P_FLY_CARECORD_INFO pFlyCarRecordInfo)
{
	pFlyCarRecordInfo->bopen = FALSE;
	pFlyCarRecordInfo->bHaveCarRecord = FALSE;
	pFlyCarRecordInfo->bKillFlyCarRecordMainThread = TRUE;
	pFlyCarRecordInfo->FlyCarRecordMainThreadHandle = NULL;
	pFlyCarRecordInfo->hFlyCarRecordMainThreadEvent = NULL;

	pFlyCarRecordInfo->hCarRecordCom = NULL;
	pFlyCarRecordInfo->bKillFlyCarRecordComThread = TRUE;
	pFlyCarRecordInfo->hFlyCarRecordComThreadHandle = NULL;

	pFlyCarRecordInfo->buffToUserHx = 0;
	pFlyCarRecordInfo->buffToUserLx = 0;
	pFlyCarRecordInfo->ExBoxInfoFrameStatus = 0;
	pFlyCarRecordInfo->hBuffToUserDataEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	InitializeCriticalSection(&pFlyCarRecordInfo->hCSSendToUser);
}

VOID Record_info_init(P_FLY_CARECORD_INFO pFlyCarRecordInfo)
{
	pFlyCarRecordInfo->CarRecordInfo.bpreSound_Recording = FALSE;
	pFlyCarRecordInfo->CarRecordInfo.bcurSound_Recording = FALSE;
	pFlyCarRecordInfo->CarRecordInfo.bpreVideo_Recording = TRUE;
	pFlyCarRecordInfo->CarRecordInfo.bcurVideo_Recording = TRUE;
	pFlyCarRecordInfo->CarRecordInfo.epreCarRecordState = RECORD;
	pFlyCarRecordInfo->CarRecordInfo.ecurCarRecordState = RECORD;
	pFlyCarRecordInfo->CarRecordInfo.epreRecordResolution = HD;
	pFlyCarRecordInfo->CarRecordInfo.ecurRecordResolution = HD;
	pFlyCarRecordInfo->CarRecordInfo.epreRecordLanguage = SIMPLIFIED_CHINESE;
	pFlyCarRecordInfo->CarRecordInfo.ecurRecordLanguage = SIMPLIFIED_CHINESE;

	pFlyCarRecordInfo->iComDataState = 0;
	pFlyCarRecordInfo->Com_Data_Count = 0;

	memset(pFlyCarRecordInfo->Com_Data,0,sizeof(pFlyCarRecordInfo->Com_Data));
}

void IpcRecv(UINT32 iEvent)
{
	P_FLY_CARECORD_INFO	pFlyCarRecordInfo = (P_FLY_CARECORD_INFO)gpFlyCarRecordInfo;

	DBGD((TEXT("\r\nFlyCarRecord Recv IPC iEvent:%d\r\n"),iEvent));

	SetEvent(pFlyCarRecordInfo->hFlyCarRecordMainThreadEvent);
}

HANDLE
FCR_Init(DWORD dwContext)
{
	P_FLY_CARECORD_INFO pFlyCarRecordInfo;

	RETAILMSG(1, (TEXT("\r\nFlyAudio CarRecord Init Start")));

	pFlyCarRecordInfo = (P_FLY_CARECORD_INFO)LocalAlloc(LPTR, sizeof(FLY_CARECORD_INFO));
	if (!pFlyCarRecordInfo)
	{
		return NULL;
	}

	gpFlyCarRecordInfo = pFlyCarRecordInfo;

	pFlyCarRecordInfo->pFlyShmGlobalInfo = CreateShm(CRARECORD_MODULE,IpcRecv);
	if (NULL == pFlyCarRecordInfo->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio CarRecord Init create shm err\r\n")));
		return NULL;
	}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio CarRecord Open create user buf err\r\n")));
		return NULL;
	}

	//参数初始化
	First_init(pFlyCarRecordInfo);
	Record_info_init(pFlyCarRecordInfo);

#if GLOBAL_COMM

	if (pFlyCarRecordInfo->pFlyDriverGlobalInfo->FlySystemRunningInfo.iGlobalStructSize
		!= sizeof(struct shm))
	{
		pFlyCarRecordInfo->pFlyDriverGlobalInfo->FlySystemRunningInfo.sErrorDriverName[0] = 'F';
		pFlyCarRecordInfo->pFlyDriverGlobalInfo->FlySystemRunningInfo.sErrorDriverName[1] = 'C';
		pFlyCarRecordInfo->pFlyDriverGlobalInfo->FlySystemRunningInfo.sErrorDriverName[2] = 'R';
	}

	pFlyCarRecordInfo->hHandleGlobalGlobalEvent = CreateEvent(NULL,FALSE,FALSE,DATA_GLOBAL_HANDLE_GLOBAL);
	pFlyCarRecordInfo->pFlyDriverGlobalInfo->FlySystemRunningInfo.bInit[CURRENT_SHARE_MEMORY_ID] = TRUE;
#endif

	DBGI((TEXT("\r\nCarRecord Init Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));

	return (HANDLE)pFlyCarRecordInfo;
}

BOOL
FCR_Deinit(DWORD hDeviceContext)
{
	P_FLY_CARECORD_INFO	pFlyCarRecordInfo = (P_FLY_CARECORD_INFO)hDeviceContext;
	CloseHandle(pFlyCarRecordInfo->hBuffToUserDataEvent);
	DeleteCriticalSection(&pFlyCarRecordInfo->hCSSendToUser);

	FlyCarRecord_Enable(pFlyCarRecordInfo,FALSE);

	RETAILMSG(1, (TEXT("FlyAudio CarRecord DeInit\r\n")));
	
	FreeShm();
	FreeUserBuff();

	LocalFree(pFlyCarRecordInfo);

	return TRUE;
}

DWORD
FCR_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	P_FLY_CARECORD_INFO	pFlyCarRecordInfo = (P_FLY_CARECORD_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;

	if (FALSE == pFlyCarRecordInfo->bopen)
	{
		FlyCarRecord_Enable(pFlyCarRecordInfo,TRUE);
		FlyCarRecord_COM_Enable(pFlyCarRecordInfo);
		pFlyCarRecordInfo->bopen = TRUE;
	}

	pFlyCarRecordInfo->iReceiveCarRecordMessageTime = 0;

	CarRecord_Send_Key(pFlyCarRecordInfo,'M');

	RETAILMSG(1, (TEXT("FlyAudio CarRecord Open\r\n")));
	return returnWhat;
}

BOOL
FCR_Close(DWORD hDeviceContext)
{
	P_FLY_CARECORD_INFO	pFlyCarRecordInfo = (P_FLY_CARECORD_INFO)hDeviceContext;

	if (TRUE == pFlyCarRecordInfo->bopen)
	{
		FlyCarRecord_COM_Disable(pFlyCarRecordInfo);
		FlyCarRecord_Enable(pFlyCarRecordInfo,FALSE);
		pFlyCarRecordInfo->bopen = FALSE;
	}
	
	RETAILMSG(1, (TEXT("FlyAudio CarRecord Close\r\n")));
	return TRUE;
}

VOID
FCR_PowerUp(DWORD hDeviceContext)
{
	P_FLY_CARECORD_INFO	pFlyCarRecordInfo = (P_FLY_CARECORD_INFO)hDeviceContext;

	Record_info_init(pFlyCarRecordInfo);

	RETAILMSG(1, (TEXT("FlyAudio CarRecord PowerUp\r\n")));
}

VOID
FCR_PowerDown(DWORD hDeviceContext)
{
	P_FLY_CARECORD_INFO	pFlyCarRecordInfo = (P_FLY_CARECORD_INFO)hDeviceContext;

	RETAILMSG(1, (TEXT("FlyAudio CarRecord PowerDown\r\n")));
}

DWORD
FCR_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	P_FLY_CARECORD_INFO	pFlyCarRecordInfo = (P_FLY_CARECORD_INFO)hOpenContext;
	UINT dwRead = 0,i;

	BYTE *buf = (BYTE *)pBuffer;
	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio CarRecord return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}

	return dwRead;
}

DWORD
FCR_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	P_FLY_CARECORD_INFO	pFlyCarRecordInfo = (P_FLY_CARECORD_INFO)hOpenContext;
	RETAILMSG(1, (TEXT("\r\nFlyAudio CarRecord Write %d "),NumberOfBytes));

	BYTE *p = (BYTE *)pSourceBytes;

	for(UINT i = 0;i < NumberOfBytes;i++)
	{
		RETAILMSG(1, (TEXT(" %X"),*((BYTE *)pSourceBytes + i)));
	}

	if(NumberOfBytes)
	{
		DealWinCECommandProcessor(pFlyCarRecordInfo,&p[3],NumberOfBytes-4);
	}

	return NULL;
}

DWORD
FCR_Seek(DWORD hOpenContext, LONG Amount, DWORD  Type)
{
	return NULL;
}

BOOL 
FCR_IOControl(DWORD hOpenContext,
			  DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn,
			  PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
	P_FLY_CARECORD_INFO	pFlyCarRecordInfo = (P_FLY_CARECORD_INFO)hOpenContext;
	BOOL bRes = TRUE;

	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	DBG2(RETAILMSG(1, (TEXT("\r\nFlyAudio CarRecord IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)"),
	//		hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	DBG3(RETAILMSG(1, (TEXT("\r\nFlyAudio CarRecord IOControl Set IOCTL_SERIAL_SET_WAIT_MASK")));
	//		break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	//RETAILMSG(1, (TEXT("\r\n\r\n\r\nFlyAudio CarRecord IOControl Set IOCTL_SERIAL_WAIT_ON_MASK Enter\r\n\r\n\r\n")));
	//		if (pFlyCarRecordInfo->bopen)
	//		{
	//			WaitForSingleObject(pFlyCarRecordInfo->hBuffToUserDataEvent,INFINITE);
	//		} 
	//		else
	//		{
	//			WaitForSingleObject(pFlyCarRecordInfo->hBuffToUserDataEvent,0);
	//		}
	//		if ((dwLenOut < sizeof(DWORD)) || (NULL == pBufOut) ||
	//			(NULL == pdwActualOut))
	//		{
	//			bRes = FALSE;
	//			break;
	//		}
	//		*(DWORD *)pBufOut = EV_RXCHAR;
	//		*pdwActualOut = sizeof(DWORD);
	//		//RETAILMSG(1, (TEXT("\r\nFlyAudio CarRecord IOControl Set IOCTL_SERIAL_WAIT_ON_MASK Exit")));
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
		DEBUGMSG(1, (TEXT("Attach in FlyCarRecord DllEntry\n")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}

	if (dwReason == DLL_PROCESS_DETACH)
	{
		DEBUGMSG(1, (TEXT("Dettach in FlyCarRecord DllEntry\n")));
	}

	DEBUGMSG(1, (TEXT("Leaving FlyCarRecord DllEntry\n")));

	return (TRUE);
}
