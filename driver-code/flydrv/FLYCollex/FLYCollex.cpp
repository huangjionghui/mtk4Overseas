// -----------------------------------------------------------------------------
// File Name    : FLYCollex.cpp
// Title        : CollexBT Driver
// Author       : JQilin - Copyright (C) 2011
// Created      : 2011-02-22  
// Version      : 0.02
// Target MCU   : WinCE Driver
// -----------------------------------------------------------------------------
// Version History:
/*
*******************************************************************************************************************
>>>修改人：JQilin
>>修改时间：2011-07-08
>>修改内容：修改蓝牙电话本返回逻辑
>>修改时间：2011-06-10
>>修改内容：待机回来死机问题，串口数据处理溢出问题，蓝牙工作状态检查问题
>>修改时间：2011-05-28
>>修改内容：蓝牙基本功能已完成，电话本待完成
>>---------------华丽分隔线--------------------------<<
>>修改人：Huangjionghui
>>修改时间：2011-05-23
>>增加函数：
static void returnBTPowerStatus(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BOOL bWork)
static void returnBTWorkStatus(P_FLY_COLLEX_BT_INFO pCollexBTInfo, BOOL bWork)
static void returnCurrentPhoneBookType(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE type)
static void returnWaitingCallNumber(P_FLY_COLLEX_BT_INFO pCollexBTInfo, BYTE *p, UINT len)

static void controlCurrentPhoneBookType(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
static void controlWaitingCallType(P_FLY_COLLEX_BT_INFO pCollexBTInfo, BYTE type)
static void controlConnectActiveMobilePhoneStatus(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BOOL bConnect)

static BOOL FlyBT_JudgeTheBtPowerIsWorking(P_FLY_COLLEX_BT_INFO pCollexBTInfo, BYTE *p, UINT len)
>>---------------------------------------------<<
>>>2011-04-25: V0.03 适应新的硬件
>>>2011-02-25: V0.02 添加读取电话本功能
>>>2011-02-22: V0.01 first draft
********************************************************************************************************************
*/
// FLYCollex.cpp : Defines the entry point for the DLL application.
//

#include "flyhwlib.h"
#include "flyuserlib.h"
#include "flysocketlib.h"
#include "FLYCollex.h"

P_FLY_COLLEX_BT_INFO gpCollexBTInfo=NULL;

static void powerNormalDeInit(P_FLY_COLLEX_BT_INFO pCollexBTInfo);
static BOOL DealBTInfo(P_FLY_COLLEX_BT_INFO pCollexBTInfo);
//static void collexBT_Enable(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BOOL bEnable);
static void returnMobileStatusInit(P_FLY_COLLEX_BT_INFO pCollexBTInfo);
static void control_WriteToCollex(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE *p,UINT len);

void FBT_PowerUp(DWORD hDeviceContext);
void FBT_PowerDown(DWORD hDeviceContext);

static void socketWriteTest(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	BYTE buf[2]={0x02,0x01};
	control_WriteToCollex(pCollexBTInfo,buf,2);

	DBGI((TEXT("\r\nCollex Socket write %d bytes to EXE:%02X,%02X,%02X\r\n"),sizeof(buf),buf[0],buf[1]));
}

static void collex_PowerControl_On(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	//SOC_IO_Output(CONTROL_IO_BTPOWER_G,CONTROL_IO_BTPOWER_I,1);
	
	BYTE buf[4]={0x10,0x01,'\r','\n'};
	control_WriteToCollex(pCollexBTInfo,buf, 4);
	DBGI((TEXT("\r\nCollex PowerControl On")));
}

static void collex_PowerControl_Off(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	//SOC_IO_Output(CONTROL_IO_BTPOWER_G,CONTROL_IO_BTPOWER_I,0);

	BYTE buf[4]={0x10,0x00,'\r','\n'};
	control_WriteToCollex(pCollexBTInfo,buf, 4);
	DBGI((TEXT("\r\nCollex PowerControl Off")));
}

static void collex_ResetControl_On(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	//SOC_IO_Output(CONTROL_IO_BTRESET_G,CONTROL_IO_BTRESET_I,0);
	//pCollexBTInfo->iAutoResetControlTime = GetTickCount();
	//DBGE((TEXT("\r\nCollex ResetControl On")));
}

static void collex_ResetControl_Off(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	//SOC_IO_Output(CONTROL_IO_BTRESET_G,CONTROL_IO_BTRESET_I,1);
	//pCollexBTInfo->iAutoResetControlTime = GetTickCount();
	//DBGE((TEXT("\r\nCollex ResetControl Off")));
}

static  void collex_ChipEnableControl_On(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	//#if A4_PLAT
	//#else
	//SOC_IO_Output(CONTROL_IO_BTCE_G,CONTROL_IO_BTCE_I,0);
	//#endif
	//DBGE((TEXT("\r\nCollex ChipEnableControl On")));
}

static void collex_ChipEnableControl_Off(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	//#if A4_PLAT
	//#else
	//SOC_IO_Output(CONTROL_IO_BTCE_G,CONTROL_IO_BTCE_I,1);
	//#endif
	//DBGE((TEXT("\r\nCollex ChipEnableControl Off")));
}




//-------------------------------------------------------------------------------------
//Description:
// This function maps a character string to a wide-character (Unicode) string
//
//Parameters:
// lpcszStr: [in] Pointer to the character string to be converted 
// lpwszStr: [out] Pointer to a buffer that receives the translated string. 
// dwSize: [in] Size of the buffer
//
//Return Values:
// TRUE: Succeed
// FALSE: Failed
// 
//Example:
// MByteToWChar(szA,szW,sizeof(szW)/sizeof(szW[0]));
//---------------------------------------------------------------------------------------
static DWORD MByteToWChar(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize)
{
	// Get the required size of the buffer that receives the Unicode 
	// string. 
	DWORD dwMinSize=dwSize*2;
	//dwMinSize = MultiByteToWideChar (CP_ACP, 0, lpcszStr, -1, NULL, 0);

	//if(dwSize < dwMinSize)
	//	return FALSE;


	// Convert headers from ASCII to Unicode.
	MultiByteToWideChar (CP_ACP, 0, lpcszStr, -1, lpwszStr, dwMinSize);  
	return dwMinSize;
}

//-------------------------------------------------------------------------------------
//Description:
// This function maps a wide-character string to a new character string
//
//Parameters:
// lpcwszStr: [in] Pointer to the character string to be converted 
// lpszStr: [out] Pointer to a buffer that receives the translated string. 
// dwSize: [in] Size of the buffer
//
//Return Values:
// TRUE: Succeed
// FALSE: Failed
// 
//Example:
// MByteToWChar(szW,szA,sizeof(szA)/sizeof(szA[0]));
//---------------------------------------------------------------------------------------
static BOOL WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,NULL,0,NULL,FALSE);
	if(dwSize < dwMinSize)
		return FALSE;

	WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,lpszStr,dwSize,NULL,FALSE);
	return TRUE;
}


static void phoneListClearAll(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	P_COLLEX_BT_PHONE_LIST p,nextP;
	p = pCollexBTInfo->sCollexBTInfo.pBTPhoneList;
	while(p)
	{
		nextP = p->Next;
		delete p;
		p = nextP;
	}
	pCollexBTInfo->sCollexBTInfo.pBTPhoneList = NULL;
}

static void phoneListClearSelectList(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE iWhichList)
{
	P_COLLEX_BT_PHONE_LIST p,nextP,lastP;
	p = pCollexBTInfo->sCollexBTInfo.pBTPhoneList;
	while(p)
	{
		nextP = p->Next;
		if (iWhichList == p->iWhichPhoneList)
		{
			if (p == pCollexBTInfo->sCollexBTInfo.pBTPhoneList)
			{
				pCollexBTInfo->sCollexBTInfo.pBTPhoneList = p->Next;
			}
			else
			{
				lastP->Next = p->Next;
			}
			delete p;
		}
		lastP = p;
		p = nextP;
	}
}

static void debugMsg(P_FLY_COLLEX_BT_INFO pCollexBTInfo,LPCWSTR sStr, BYTE *buf,UINT16 len)
{
	UINT16 i=0;

	DBGD((sStr));
	for (i=0; i<len; i++)
	{
		DBGD((TEXT("%02X "),buf[i]));
	}
}

P_COLLEX_BT_PHONE_LIST phoneListNewOne(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE *buf,UINT len)
{
	//添加一个电话
	P_COLLEX_BT_PHONE_LIST newP = new COLLEX_BT_PHONE_LIST;
	memset(newP,0,sizeof(struct _COLLEX_BT_PHONE_LIST));

	if ((len-1+2) <= (COLLEX_PHONE_NUMBER+COLLEX_PHONE_NAME))
	{
		memcpy(newP->cPhoneNumberName,&buf[1],len-1);
		newP->cPhoneNumberName[len-1] = '\r';newP->cPhoneNumberName[len] = '\n';
		newP->iPhoneNumberNameLength = len+1;
	} 
	else
	{
		newP->cPhoneNumberName[0] = '\r';newP->cPhoneNumberName[1] = '\n';
		newP->iPhoneNumberNameLength = 2;
	}

	//debugMsg(pCollexBTInfo,TEXT("\r\ncPhoneNumberName:"), newP->cPhoneNumberName,newP->iPhoneNumberNameLength);

	UINT readOffset = 1;
	UINT writeOffset;
	newP->iWhichPhoneList = buf[0];

	writeOffset = 0;
	newP->iPhoneNumberLength = 0;
	//while (writeOffset < COLLEX_PHONE_NUMBER && readOffset < len && (((!(buf[readOffset] == '+')|| readOffset == 1)&& writeOffset != 0)))
	while (writeOffset < COLLEX_PHONE_NUMBER && readOffset < len && (!(buf[readOffset] == '+' && writeOffset != 0)))
	{
		newP->cPhoneNumber[writeOffset++] = buf[readOffset++];
		newP->iPhoneNumberLength++;
	}
	
	//debugMsg(pCollexBTInfo,TEXT("\r\ncPhoneNumber:"), newP->cPhoneNumber,writeOffset);

	readOffset++;							//buf[readOffset]='+'
	newP->iPhoneType = buf[readOffset++];	//buf[readOffset]=0x00

	//readOffset++;
	writeOffset = 0;
	while (writeOffset < COLLEX_PHONE_NAME && readOffset < len && (!(buf[readOffset] == '\r' && buf[readOffset+1] == '\n')))
	{
		newP->cPhoneName[writeOffset++] = buf[readOffset++];
		newP->iPhoneNameLength = writeOffset;
	}

	//debugMsg(pCollexBTInfo,TEXT("\r\ncPhoneName:"), newP->cPhoneName,newP->iPhoneNameLength);

	newP->Next = NULL;

	return newP;
}

static void phoneListAdd(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE *buf,UINT len)
{
	P_COLLEX_BT_PHONE_LIST p;

	P_COLLEX_BT_PHONE_LIST newP = phoneListNewOne(pCollexBTInfo,buf,len);//添加一个电话

	if (pCollexBTInfo->sCollexBTInfo.pBTPhoneList)
	{
		p = pCollexBTInfo->sCollexBTInfo.pBTPhoneList;//添加到链表中
		while (p->Next)
		{
			p = p->Next;
		}
		p->Next = newP;
	}
	else
	{
		pCollexBTInfo->sCollexBTInfo.pBTPhoneList = newP;
	}
}

static void phoneListReplace(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE *buf,UINT len,UINT Count)//Count为某类别的Count
{
	P_COLLEX_BT_PHONE_LIST pLast,pCurrent;
	UINT32 iCount = 0;
	pLast = pCollexBTInfo->sCollexBTInfo.pBTPhoneList;
	pCurrent = pCollexBTInfo->sCollexBTInfo.pBTPhoneList;
	
	P_COLLEX_BT_PHONE_LIST newP = phoneListNewOne(pCollexBTInfo,buf,len);//添加一个电话

	if (pCurrent)//有
	{
		while (pCurrent)
		{
			if (pCurrent->iWhichPhoneList == newP->iWhichPhoneList)//此类别
			{
				if (iCount == Count)//找到序号
				{
					if (pLast == pCurrent)//只会在链表头部出现
					{
						newP->Next = pCurrent->Next;
						pCollexBTInfo->sCollexBTInfo.pBTPhoneList = newP;
						delete pCurrent;
					}
					else
					{
						newP->Next = pCurrent->Next;
						pLast->Next = newP;
						delete pCurrent;
					}
					return;
				}
				iCount++;
			}
			pLast = pCurrent;
			pCurrent = pCurrent->Next;
		}
		pLast->Next = newP;
	}
	else//无，则直接添加
	{
		pCollexBTInfo->sCollexBTInfo.pBTPhoneList = newP;
	}
}

P_COLLEX_BT_PHONE_LIST phoneListGetSelectOne(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE iWhichSelect,BYTE iCount)
{
	P_COLLEX_BT_PHONE_LIST p;
	UINT iCountCurrent = 0;

	p = pCollexBTInfo->sCollexBTInfo.pBTPhoneList;
	while (p)
	{
		if (iWhichSelect == p->iWhichPhoneList)
		{
			if (iCountCurrent == iCount)
			{
				return p;
			}
			iCountCurrent++;
		}
		p = p->Next;
	}

	return NULL;
}

UINT phoneListGetSelectCount(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE iWhichSelect)
{
	P_COLLEX_BT_PHONE_LIST p;
	UINT32 iCount = 0;

	p = pCollexBTInfo->sCollexBTInfo.pBTPhoneList;
	while (p)
	{
		if (iWhichSelect == p->iWhichPhoneList)
		{
			iCount++;
		}
		p = p->Next;
	}

	return iCount;
}

static P_COLLEX_BT_PHONE_LIST phoneEachListByNumber(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE *buf,UINT len)
{
	P_COLLEX_BT_PHONE_LIST p;
	p = pCollexBTInfo->sCollexBTInfo.pBTPhoneList;

	while (p)
	{
		if (!memcmp(p->cPhoneNumber,buf,len))
		{
			if (p->iPhoneNumberLength && len == p->iPhoneNumberLength)
				break;
		}
		
		p = p->Next;
	}

	return p;
}


static void collexPhoneListInit(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	pCollexBTInfo->sCollexBTInfo.iPhoneListType = 0;
	pCollexBTInfo->sCollexBTInfo._W_iPhoneListType = 0;
	pCollexBTInfo->sCollexBTInfo.bPhoneListMobileReturn = FALSE;
	pCollexBTInfo->sCollexBTInfo.iPhoneListMobileReturnCount = 0;
	pCollexBTInfo->sCollexBTInfo.bPhoneListStartReturn = FALSE;
	pCollexBTInfo->sCollexBTInfo.iPhoneListStart = 0;
	pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCount = 5;
	pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCurrent = 0;
	phoneListClearAll(pCollexBTInfo);
}

static void collexBTInfoInit(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BOOL bInitAll)
{
	UINT i;

	pCollexBTInfo->sCollexBTInfo.bWork = FALSE;//@@@@以后会改

	pCollexBTInfo->sCollexBTInfo.bPaired = FALSE;
	pCollexBTInfo->sCollexBTInfo.bConnected = FALSE;

	pCollexBTInfo->sCollexBTInfo.iPairedStatus = 0;

	pCollexBTInfo->sCollexBTInfo._W_bPairing = FALSE;


	pCollexBTInfo->sCollexBTInfo.iPairedDeviceType = 0;
	memset(pCollexBTInfo->sCollexBTInfo.BDAddress,0,6);

	memset(pCollexBTInfo->sCollexBTInfo.sDeviceName,0,COLLEX_PHONE_NAME);
	pCollexBTInfo->sCollexBTInfo.iDeviceNameLength = 0;

	memset(pCollexBTInfo->sCollexBTInfo.sWaitingCallNumber,0,COLLEX_PHONE_NAME);
	pCollexBTInfo->sCollexBTInfo.iWaitingCallLen = 0;
	pCollexBTInfo->sCollexBTInfo.iWaitingCallType = 0;
	memset(pCollexBTInfo->sCollexBTInfo.sCallInPhoneNumber,0,COLLEX_PHONE_NUMBER);


	//清除要返回的电话号码
	pCollexBTInfo->sCollexBTInfo.iCallNumberBufflen = 0;
	memset(pCollexBTInfo->sCollexBTInfo.cCallNumberBuff,'\0',COLLEX_PHONE_NUMBER);

	pCollexBTInfo->sCollexBTInfo.cMobileCallStatus = 'H';

	pCollexBTInfo->sCollexBTInfo.bAudioConnectionStatus = TRUE;
	pCollexBTInfo->sCollexBTInfo.iAudioConnectionStatusTime = 0;

	pCollexBTInfo->sCollexBTInfo.bStereoDeviceConnection = FALSE;

	pCollexBTInfo->sCollexBTInfo.mobileBattery = 0;
	pCollexBTInfo->sCollexBTInfo.mobileSignal = 0;
	pCollexBTInfo->sCollexBTInfo.mobileVolume = 0;

	collexPhoneListInit(pCollexBTInfo);

	pCollexBTInfo->sCollexBTInfo.bPhoneListNeedReturnFlush = FALSE;
	for (i = 0;i < 7;i++)
	{
		pCollexBTInfo->sCollexBTInfo.bPhoneListPhoneReadFinish[i] = FALSE;
	}

	if (pCollexBTInfo->bOpen)
	{
		returnMobileStatusInit(pCollexBTInfo);
	}
}

void throwFirstByte(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE *pData,UINT Count)
{
	UINT i;
	if (pData[0])
	{
		//DBGD((TEXT("\r\nThrow One Data")));
	}
	for (i = 0;i < Count - 1;i++)
	{
		pData[i] = pData[i+1];
	}
}

//static void OnCommRecv(P_FLY_COLLEX_BT_INFO pCollexBTInfo, BYTE *buf, UINT buflen)
//{
//	UINT i;
//	if (!pCollexBTInfo->bPower)//不再处理
//	{
//		pCollexBTInfo->BTInfoFrameStatus = 0;
//		return;
//	}
//
//	for (i = 0; i < buflen; i++)
//	{
//		if (0 == pCollexBTInfo->BTInfoFrameStatus
//			&& 0 == buf[i])//跳过无效数据
//		{
//			continue;
//		}
//		if (pCollexBTInfo->BTInfoFrameStatus > DATA_COMM_BUFF_LENGTH-10)//一条消息太长，多减点吧
//		{
//			throwFirstByte(pCollexBTInfo,pCollexBTInfo->BTInfoFrameBuff,pCollexBTInfo->BTInfoFrameStatus);
//			pCollexBTInfo->BTInfoFrameStatus--;
//		}
//		pCollexBTInfo->BTInfoFrameBuff[pCollexBTInfo->BTInfoFrameStatus++] = buf[i];
//		if (pCollexBTInfo->BTInfoFrameStatus >= 2
//			&& pCollexBTInfo->BTInfoFrameBuff[pCollexBTInfo->BTInfoFrameStatus - 2] == 0x0D
//			&& pCollexBTInfo->BTInfoFrameBuff[pCollexBTInfo->BTInfoFrameStatus - 1] == 0x0A)
//		{
//			pCollexBTInfo->BTInfoFrameBuff[pCollexBTInfo->BTInfoFrameStatus] = 0;
//			pCollexBTInfo->BTInfoFrameBuff[pCollexBTInfo->BTInfoFrameStatus+1] = 0;
//			while (pCollexBTInfo->BTInfoFrameStatus >= 2 && DealBTInfo(pCollexBTInfo))//消息异常、需要继续处理
//			{
//				if (pCollexBTInfo->BTInfoFrameBuff[0] == 0x0D
//					&& pCollexBTInfo->BTInfoFrameBuff[1] == 0x0A)//遇到结束符、退出
//				{
//					break;
//				}
//				throwFirstByte(pCollexBTInfo,pCollexBTInfo->BTInfoFrameBuff,pCollexBTInfo->BTInfoFrameStatus);
//				pCollexBTInfo->BTInfoFrameStatus--;
//			}
//			pCollexBTInfo->BTInfoFrameStatus = 0;
//		}
//	}
//	/* 取得控件指针 */
//}


static void closeBTComm(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{

}

static void openBTComm(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{

}

static void returnMobilePowerMode(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BOOL bPower)
{
	BYTE buff[2];
	buff[0] = 0x01;
	if (bPower)
	{
		buff[1] = 0x01;
	} 
	else
	{
		buff[1] = 0x00;
	}
	ReturnToUser(buff,2);
}

static void returnMobileWorkMode(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BOOL bWork)
{
	BYTE buff[2];
	buff[0] = 0x02;
	if (bWork)
	{
		buff[1] = 0x01;
	} 
	else
	{
		buff[1] = 0x00;
	}
	ReturnToUser(buff,2);
}

static void returnMobilePairStatus(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BOOL bPair)
{
	BYTE buff[2];
	buff[0] = 0x20;
	if (bPair)
	{
		buff[1] = 0x01;
	} 
	else
	{
		buff[1] = 0x00;
	}
	ReturnToUser(buff,2);
}

static void returnMobileAudioTransfer(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BOOL bModule)
{
	BYTE buff[2];
	buff[0] = 0x21;
	if (bModule)
	{
		buff[1] = 0x00;
	} 
	else
	{
		buff[1] = 0x01;
	}
	ReturnToUser(buff,2);
}

static void returnBTA2DPbConnect(P_FLY_COLLEX_BT_INFO pCollexBTInfo, BOOL bConnect)
{
	BYTE buff[2]={0x50,0x00};
	if (bConnect)
	{
		buff[1] = 0x01;
	}
	ReturnToUser(buff,2);
}

static void returnMobileDailOutNumber(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	UINT length; 
	P_COLLEX_BT_PHONE_LIST plist;
	BYTE buff[2+COLLEX_PHONE_NUMBER+COLLEX_PHONE_NAME+1];
	buff[0] = 0x23;
	buff[1] = 0x02;

	length = pCollexBTInfo->sCollexBTInfo.iCallNumberBufflen;
	if (0 == length)
	{
		memset(pCollexBTInfo->sCollexBTInfo.cCallNumberBuff,'\0',COLLEX_PHONE_NUMBER);
		ReturnToUser(buff,2);
	}
	else if (length < COLLEX_PHONE_NUMBER)
	{
		//把电话号码先存起来 
		memcpy(&buff[2],pCollexBTInfo->sCollexBTInfo.cCallNumberBuff,length);

		//以号码找名字 
		plist = phoneEachListByNumber(pCollexBTInfo,
			pCollexBTInfo->sCollexBTInfo.cCallNumberBuff,length);
		if (plist != NULL && plist->iPhoneNameLength != 0){
			buff[length+2] = '+';
			buff[length+3] = 0x00;
			length +=4;
			memcpy(&buff[length],plist->cPhoneName,plist->iPhoneNameLength);
			length += (plist->iPhoneNameLength);
			ReturnToUser(buff,length);	
		}
		else
			ReturnToUser(buff,length+2);
	}
}

//蓝牙来电时，禁止DVD视频
static void sendBTStatusToDVD(P_FLY_COLLEX_BT_INFO pCollexBTInfo, BYTE iStatus)
{
	BOOL bBtStatus = FALSE;
	if (iStatus == 'A' || iStatus == 'R' || iStatus == 'D')
	{
		bBtStatus = TRUE;
	}
	else
	{
		bBtStatus = FALSE;
	}

	if (pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDVDBTStatus != bBtStatus)
	{
		pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDVDBTStatus = bBtStatus;
		IpcStartEvent(EVENT_BT_OR_RING_STATUS_ID);
	}
}

static void returnMobileDialStatus(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE *buf, UINT16 len)
{
	BYTE buff[2];
	BYTE para = buf[0];
	
	buff[0] = 0x23;
	if ('A' == para)
	{
		buff[1] = 0x03;
	} 
	else if ('R' == para)
	{
		buff[1] = 0x01;
	}
	else if ('D' == para)
	{
		buff[1] = 0x02;

		//记录拔打的电话号码
		//pCollexBTInfo->sCollexBTInfo.iCallNumberBufflen = 
		//	MByteToWChar((LPCSTR)&buf[2], (LPWSTR)pCollexBTInfo->sCollexBTInfo.cCallNumberBuff, len-2);

		if (len == 1)
		{
			pCollexBTInfo->sCollexBTInfo.iCallNumberBufflen = 0;
			memset(pCollexBTInfo->sCollexBTInfo.cCallNumberBuff,
				0x00,sizeof(pCollexBTInfo->sCollexBTInfo.cCallNumberBuff));
		}
		else
		{
			pCollexBTInfo->sCollexBTInfo.iCallNumberBufflen = len-1;
			memcpy(pCollexBTInfo->sCollexBTInfo.cCallNumberBuff,&buf[1],len-1);
		}
		//UINT i;
		//DBGI((TEXT("\r\nBT call number:")));
		//for (i=0; i<pCollexBTInfo->sCollexBTInfo.iCallNumberBufflen; i++)
		//{
		//	DBGI((TEXT("%02X "), pCollexBTInfo->sCollexBTInfo.cCallNumberBuff[i]));
		//}

		returnMobileDailOutNumber(pCollexBTInfo);
	}
	else
	{
		buff[1] = 0x00;
	}

	if (pCollexBTInfo->sCollexBTInfo.cMobileCallStatus == 'H')
	{
		pCollexBTInfo->sCollexBTInfo.iWaitingCallLen = 0;
		memset(pCollexBTInfo->sCollexBTInfo.sCallInPhoneNumber,0,COLLEX_PHONE_NUMBER);

		//清除要返回的电话号码
		pCollexBTInfo->sCollexBTInfo.iCallNumberBufflen = 0;
		memset(pCollexBTInfo->sCollexBTInfo.cCallNumberBuff,'\0',COLLEX_PHONE_NUMBER);
	}
	
	//全局
	pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus = buff[1];
	IpcStartEvent(EVENT_GLOBAL_BTCALLSTATUS_CHANGE_ID);
 
#if COLLEX_AUDIO_TRANS_USE_COLLEX
#else
	if (0x00 == buff[1])
	{
		returnMobileAudioTransfer(pCollexBTInfo,TRUE);
	}
#endif

	if ('D' != para)
		ReturnToUser(buff,2);
}

static void returnMobileLinkStatus(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BOOL bLink)
{
	BYTE buff[2];
	buff[0] = 0x22;
	if (bLink)
	{
		buff[1] = 0x01;
	} 
	else
	{
		buff[1] = 0x00;
	}

	if (FALSE == bLink)//断开连接肯定不能通话或者用A2DP了
	{
		pCollexBTInfo->sCollexBTInfo.cMobileCallStatus = 'H';
		returnMobileDialStatus(pCollexBTInfo,&pCollexBTInfo->sCollexBTInfo.cMobileCallStatus,1);
		pCollexBTInfo->sCollexBTInfo.bStereoDeviceConnection = FALSE;
		returnBTA2DPbConnect(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.bStereoDeviceConnection);
	}
	ReturnToUser(buff,2);
}

static void returnMobileDailInNumber(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE *p,UINT length)
{
	BYTE buff[2+COLLEX_PHONE_NUMBER+COLLEX_PHONE_NAME+1];
	buff[0] = 0x23;buff[1] = 0x01;
	if (length <= COLLEX_PHONE_NUMBER)
	{
		if (memcmp(pCollexBTInfo->sCollexBTInfo.sCallInPhoneNumber,p,length))
		{
			//全局
			pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus = 0x01;
			IpcStartEvent(EVENT_GLOBAL_BTCALLSTATUS_CHANGE_ID);

			memcpy(pCollexBTInfo->sCollexBTInfo.sCallInPhoneNumber,p,length);
			memcpy(&buff[2],p,length);
			ReturnToUser(buff,length+2);
		}
	}


}
static void returnMobilePhoneList(P_FLY_COLLEX_BT_INFO pCollexBTInfo,P_COLLEX_BT_PHONE_LIST p)
{
	BYTE buff[128];
	buff[0] = 0x27;

	if (p)
	{
		memcpy(&buff[1],p->cPhoneNumberName,p->iPhoneNumberNameLength);
		ReturnToUser(buff,p->iPhoneNumberNameLength+1);
	}
}
static void returnMobileName(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE *p,UINT len)
{
	BYTE buff[1+COLLEX_PHONE_NAME];
	buff[0] = 0x31;
	if (len <= COLLEX_PHONE_NAME)
	{
		memcpy(&buff[1],p,len);
		ReturnToUser(buff,len+1);
	}
}

////---------------
//static void returnMobileCallInNumber(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE *p,UINT len)
//{
//	BYTE buff[64];
//	buff[0] = 0x06;buff[1] = 0x01;
//	memcpy(&buff[2],p,len);
//
//	memcpy(pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalBTInfo.cMobileCallInNumber,p,len);
//	pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalBTInfo.iMobileCallInNumberLength = len;
//	SetEvent(pCollexBTInfo->hHandleGlobalGlobalEvent);
//
//	ReturnToUser(buff,len+2);
//}

static void returnBTVersion(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE *p,UINT len)
{
	BYTE buff[64];
	buff[0] = 0x2F;
	memcpy(&buff[1],p,len);
	ReturnToUser(buff,len+1);
}

static void returnMobileSignalStrength(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE para)
{
	BYTE buff[2];
	buff[0] = 0x40;buff[1] = para;
	ReturnToUser(buff,2);
}

static void returnMobileBattery(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE para)
{
	BYTE buff[2];
	buff[0] = 0x41;buff[1] = para;
	ReturnToUser(buff,2);
}

static void returnBTPowerStatus(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BOOL bWork)
{
	BYTE buff[2] = {0x10,0x00};
	if (bWork)
	{
		buff[1] = 0x01;
	} 
	ReturnToUser(buff,2);
}
static void returnCurrentPhoneBookType(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE type)
{
	BYTE buff[2]={0x25,0x00};
	buff[1] = type;
	ReturnToUser(buff,2);
}
static void returnBTWorkStatus(P_FLY_COLLEX_BT_INFO pCollexBTInfo, BOOL bWork)
{
	BYTE buff[2]={0x30,0x00};
	if (bWork)
	{
		buff[1] = 0x01;
	}
	ReturnToUser(buff,2);
}
static void returnBTPhoneBookReadStatus(P_FLY_COLLEX_BT_INFO pCollexBTInfo, BYTE iStatus)
{
	BYTE buff[2]={0x32,0x00};
	buff[1] = iStatus;
	ReturnToUser(buff,2);
}
static void returnBTPhoneBookPageFlush(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	BYTE buff[2]={0x33,0x00};
	ReturnToUser(buff,2);
}
static void returnWaitingCallNumber(P_FLY_COLLEX_BT_INFO pCollexBTInfo, BYTE *p, UINT len)
{
	BYTE buff[64];
	buff[0] = 0x40;
	memcpy(&buff[1],p,len);
	ReturnToUser(buff,len+1);
}

static void returnMobileStatusInit(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	returnMobilePairStatus(pCollexBTInfo,FALSE);
	returnMobileLinkStatus(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.bConnected);
	returnMobileDialStatus(pCollexBTInfo,&pCollexBTInfo->sCollexBTInfo.cMobileCallStatus,1);
	returnMobileAudioTransfer(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.bAudioConnectionStatus);
	returnMobileName(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.sDeviceName,pCollexBTInfo->sCollexBTInfo.iDeviceNameLength);
	returnMobileSignalStrength(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.mobileSignal);
	returnMobileBattery(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.mobileBattery);

	returnBTA2DPbConnect(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.bStereoDeviceConnection);
	//返回工作状态
	returnBTWorkStatus(pCollexBTInfo, pCollexBTInfo->sCollexBTInfo.bWork);
	returnBTVersion(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.sVersion,8);

}

static void control_WriteToCollex(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE *p,UINT len)
{
	if (pCollexBTInfo->bUpdater && !pCollexBTInfo->bSocketConnecting)
		return;

	UINT16 i;
	DBGD((TEXT("\r\nWrite to CollexBT:")));
	for (i=0; i<len-2; i++)
		DBGD((TEXT(" %c"),p[i]));

	SocketWrite(p, len);
}

static void controlBTDeletePairedDevice(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	BYTE buff[9];
	buff[0] = 'e';
	memset(&buff[1],0xFF,sizeof(BYTE)*6);
	buff[7] = '\r';buff[8] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,9);
}

static void controlBTAudioTrans(P_FLY_COLLEX_BT_INFO pCollexBTInfo, BOOL bCarAudioTrans)
{
	BYTE buff[4]={'t','0','\r','\n'};
	
	if (bCarAudioTrans)//车载接听
		buff[1] = '1';

	control_WriteToCollex(pCollexBTInfo,buff,4);
}

static void controlBTPair(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	BYTE buff[3];
	buff[0] = 'p';buff[1] = '\r';buff[2] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,3);
}

static void controlBTReqVersion(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	BYTE buff[3];
	buff[0] = 'i';buff[1] = '\r';buff[2] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,3);
}

static void controlBTAnswerCall(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	BYTE buff[3];
	buff[0] = 'a';buff[1] = '\r';buff[2] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,3);
}

static void controlBTHangCall(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	BYTE buff[3];
	buff[0] = 'h';buff[1] = '\r';buff[2] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,3);
}

static void controlBTDailNumber(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE *p,UINT len)
{
	BYTE buff[64];
	buff[0] = 'd';
	memcpy(&buff[1],p,len);
	buff[len + 1] = '\r';buff[len + 2] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,len + 3);
}

static void controlBTReqMobileName(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	BYTE buff[3];
	buff[0] = 'q';buff[1] = '\r';buff[2] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,3);
}

static void controlBTDTMFKey(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE key)
{
	BYTE buff[4];
	buff[0] = 'k';
	buff[1] = key;
	buff[2] = '\r';buff[3] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,4);
}

static void controlBTSelectPhoneBook(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE phoneBook)
{
	BYTE buff[5];
	buff[0] = 'b';
	buff[1] = '0';
	buff[2] = phoneBook;
	buff[3] = '\r';buff[4] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,5);
}

static void controlBTReqPhoneBookListOne(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE phoneBook)
{
	BYTE buff[5];
	buff[0] = 'b';
	buff[1] = '1';
	buff[2] = '1';
	buff[3] = '\r';buff[4] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,5);
}

static void controlBTReqPhoneBookListFive(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE phoneBook)
{
	BYTE buff[5];
	buff[0] = 'b';
	buff[1] = '2';
	buff[2] = '1';
	buff[3] = '\r';buff[4] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,5);
}

static void controlBTReqPhoneBookListAll(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	BYTE buff[5];
	buff[0] = 'b';
	buff[1] = '3';
	buff[2] = '0';
	buff[3] = '\r';buff[4] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,5);
}

static void controlBTVolumeControl(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE key)
{
	BYTE buff[4];
	buff[0] = 's';
	buff[1] = key;
	buff[2] = '\r';buff[3] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,4);
}

static void controlCurrentPhoneBookType(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	BYTE buff[5];
	buff[0] = 'b';
	buff[1] = '0'; buff[2] = '0';
	buff[3] = '\r';buff[4] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,5);
}
static void controlConnectActiveMobilePhoneStatus(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BOOL bConnect)
{
	BYTE buff[4];
	buff[0] = 'c';
	if (bConnect)
	{
		buff[1] = '1';
	}
	else
	{
		buff[1] = '0'; 
	}
	buff[2] = '\r';buff[3] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,4);
}

static void controlConnectA2DP(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BOOL bConnect)
{
	BYTE buff[4];
	buff[0] = 'x';
	if (bConnect)
	{
		buff[1] = '1';
	}
	else
	{
		buff[1] = '0'; 
	}
	buff[2] = '\r';buff[3] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,4);
}

static void controlWaitingCallType(P_FLY_COLLEX_BT_INFO pCollexBTInfo, BYTE type)
{
	BYTE buff[4];
	buff[0] = 'w';
	buff[1] = type+'0';
	buff[2] = '\r';buff[3] = '\n';
	control_WriteToCollex(pCollexBTInfo,buff,4);
}

static void controlBTUseInnerMic(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BOOL bInner)
{
	BYTE buff[5] = {'j','a','1','\r','\n'};

	if (bInner)
	{
		buff[2] = '2';
	}

	control_WriteToCollex(pCollexBTInfo,buff,5);
}

static void controlBTWarmReset(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	BYTE buff[3] = {'r','\r','\n'};

	control_WriteToCollex(pCollexBTInfo,buff,3);
}

static void controlA2DPButtom(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE iButtom,BYTE bPressed)
{
	BYTE buff[5] = {'n','1','1','\r','\n'};

	buff[1] = iButtom;
	buff[2] = bPressed;
	control_WriteToCollex(pCollexBTInfo,buff,5);
}
//告诉蓝牙，当前是否为A2DP通道
static void controlCurentAudioChannel(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	//BYTE buff[4] = {'z','1','\r','\n'};

	//if (pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eAudioInput != A2DP)
	//{
	//	buff[1] = '0';
	//}

	//control_WriteToCollex(pCollexBTInfo,buff,4);
}

static void selectPhoneBookType(P_FLY_COLLEX_BT_INFO pCollexBTInfo, BYTE type)
{
	if (type <= 0x06)
	{
		//if (pCollexBTInfo->sCollexBTInfo._W_iPhoneListType != (type + '0'))
		{
			pCollexBTInfo->sCollexBTInfo._W_iPhoneListType = type + '0';
			pCollexBTInfo->sCollexBTInfo.iPhoneListStart = 0;
			pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCount = 5;
			pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCurrent = 0;
			//需要返回电话本
			//pCollexBTInfo->sCollexBTInfo.bPhoneListStartReturn = TRUE;


			//controlBTSelectPhoneBook(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo._W_iPhoneListType);

			//pCollexBTInfo->sCollexBTInfo.bPhoneListNeedReturnFlush = TRUE;
			//pCollexBTInfo->sCollexBTInfo.bPhoneListStartReturn = TRUE;
			//	PostSignal(&pCollexBTInfo->CollexBTMutex,&pCollexBTInfo->CollexBTCond,&pCollexBTInfo->bCollexBTThreadRunAgain);
		}
	}
}

static void dealPhoneBook(P_FLY_COLLEX_BT_INFO pCollexBTInfo, BYTE *p, UINT len)
{
	if (!memcmp(pCollexBTInfo->sCollexBTInfo.sDeviceOldName,p,len))
	{
		DBGD((TEXT("\r\nBT name old .......\r\n")));
		selectPhoneBookType(pCollexBTInfo, 2);
		returnCurrentPhoneBookType(pCollexBTInfo,0x02);
	}
	else
	{
		DBGD((TEXT("\r\nBT name new .......\r\n")));
		//新的手机连接 
		memset(pCollexBTInfo->sCollexBTInfo.sDeviceOldName,0,COLLEX_PHONE_NAME);
		memcpy(pCollexBTInfo->sCollexBTInfo.sDeviceOldName,p,len);
		pCollexBTInfo->sCollexBTInfo.iDeviceOldNameLength = len;
		phoneListClearAll(pCollexBTInfo);

		pCollexBTInfo->sCollexBTInfo._W_iPhoneListType = '2';
		controlBTSelectPhoneBook(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo._W_iPhoneListType);
		returnCurrentPhoneBookType(pCollexBTInfo,0x02);
	}
	
	SetEvent(pCollexBTInfo->hCollexBTMainThreadEvent);
}

static BOOL DealBTInfo(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE *buf, UINT16 len)
{
	UINT16 i=0;
	BOOL bMsgHandle = TRUE;
	BOOL bRet = FALSE;

	//if (pCollexBTInfo->BTInfoFrameBuff[0] == 0x00)
	//{
	//	bRet = TRUE;
	//	return bRet;
	//}

	//	if ('I' != pCollexBTInfo->BTInfoFrameBuff[0])
	//{
	//	DBGD((TEXT("\r\nCollexBT decode:")));
	//	for (i = 0;i < pCollexBTInfo->BTInfoFrameStatus - 2;i++)//跳过换行符
	//	{
	//		if (pCollexBTInfo->BTInfoFrameBuff[i] < 0x20 || pCollexBTInfo->BTInfoFrameBuff[i] > 0x7F)
	//		{
	//			DBGD((TEXT("0x%x"),pCollexBTInfo->BTInfoFrameBuff[i]));
	//		}
	//		else
	//		{
	//			DBGD((TEXT("%c"),pCollexBTInfo->BTInfoFrameBuff[i]));
	//		}
	//	}
	//	if ('I' == pCollexBTInfo->BTInfoFrameBuff[0])
	//	{
	//		DBGD((TEXT("********")));
	//	}
	//}

	switch (buf[0])
	{
	case 'E':
		break;
	case 'C':
		{
			UINT16 iDailInNumberLength = 0;
			while (iDailInNumberLength < len - 1
				&& buf[iDailInNumberLength + 1] != '\r')
			{
				//if (buf[iDailInNumberLength + 1] == '+' && iDailInNumberLength)
				//{
				//	break;
				//}
				iDailInNumberLength++;
			}
			returnMobileDailInNumber(pCollexBTInfo,&buf[1],iDailInNumberLength);
		}
		break;
	case 'B':
		if ('0' == buf[1])
		{
			pCollexBTInfo->sCollexBTInfo.iPhoneListType = buf[2];
			pCollexBTInfo->sCollexBTInfo.iPhoneListMobileReturnCount = 0;

			if (pCollexBTInfo->sCollexBTInfo._W_iPhoneListType == pCollexBTInfo->sCollexBTInfo.iPhoneListType)
			{
				//controlBTReqPhoneBookListAll(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.iPhoneListType);
				controlBTReqPhoneBookListFive(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.iPhoneListType);
				//returnCurrentPhoneBookType(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.iPhoneListType-'0');
			}
		}
		else if ('1' == buf[1])
		{
			//phoneListAdd(pCollexBTInfo,&buf[2],len-2);
			phoneListReplace(pCollexBTInfo,&buf[2],len-2,pCollexBTInfo->sCollexBTInfo.iPhoneListMobileReturnCount);
			pCollexBTInfo->sCollexBTInfo.iPhoneListMobileReturnCount++;
			if (pCollexBTInfo->sCollexBTInfo.iPhoneListType >= '0'
				&& pCollexBTInfo->sCollexBTInfo.iPhoneListType <= '6')
			{
				if (pCollexBTInfo->sCollexBTInfo.iPhoneListMobileReturnCount > phoneListGetSelectCount(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.iPhoneListType))
				{
					pCollexBTInfo->sCollexBTInfo.iPhoneListMobileReturnCount = phoneListGetSelectCount(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.iPhoneListType);
				}
			}
			pCollexBTInfo->sCollexBTInfo.bPhoneListMobileReturn = TRUE;
		}
		else if ('2' == buf[1])
		{
			if ('1' == buf[2])
			{
				if (pCollexBTInfo->sCollexBTInfo._W_iPhoneListType == pCollexBTInfo->sCollexBTInfo.iPhoneListType)
				{
					if (pCollexBTInfo->sCollexBTInfo.bPhoneListMobileReturn)
					{
						pCollexBTInfo->sCollexBTInfo.bPhoneListMobileReturn = FALSE;
						controlBTReqPhoneBookListFive(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.iPhoneListType);
					}
					else
					{
						if (pCollexBTInfo->sCollexBTInfo.iPhoneListType >= '0'
							&& pCollexBTInfo->sCollexBTInfo.iPhoneListType <= '6')
						{
							pCollexBTInfo->sCollexBTInfo.bPhoneListPhoneReadFinish[pCollexBTInfo->sCollexBTInfo.iPhoneListType - '0'] = TRUE;
						}
					}
				}
			}
			else if ('3' == buf[2] || '6' == buf[2])
			{
				if (pCollexBTInfo->sCollexBTInfo._W_iPhoneListType == pCollexBTInfo->sCollexBTInfo.iPhoneListType)
				{
					pCollexBTInfo->sCollexBTInfo.iPhoneListType = 0xFF;//蓝牙返回无法获取电话本
					returnBTPhoneBookReadStatus(pCollexBTInfo,buf[2] - '0');
				}
			}
		}

		if (pCollexBTInfo->sCollexBTInfo._W_iPhoneListType != pCollexBTInfo->sCollexBTInfo.iPhoneListType)
		{
			//pCollexBTInfo->sCollexBTInfo.bPhoneListStartReturn = FALSE;
			if ('2' == buf[1])
			{
				if ('1' == buf[2]
				|| '2' == buf[2]
				|| '4' == buf[2])
				{
					controlBTSelectPhoneBook(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo._W_iPhoneListType);
				}
			}
		}
		else
		{
			pCollexBTInfo->sCollexBTInfo.bPhoneListStartReturn = TRUE;
			SetEvent(pCollexBTInfo->hCollexBTMainThreadEvent);
		}
		break;
	case 'I':
		if (memcmp(&pCollexBTInfo->sCollexBTInfo.sVersion[0],&buf[1],8))
		{
			memcpy(&pCollexBTInfo->sCollexBTInfo.sVersion[0],&buf[1],8);
			memcpy((void*)&pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTSoftwareVersion[0],BT_VERSION,8);
			pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTSoftwareVersionLength = 8;
			IpcStartEvent(EVENT_GLOBAL_RETURN_BT_VERSION_ID);

			//returnBTVersion(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.sVersion,8);
		}
		break;
	case 'D':case 'A':case 'R':case 'H':
		pCollexBTInfo->sCollexBTInfo.cMobileCallStatus = buf[0];
		returnMobileDialStatus(pCollexBTInfo,&buf[0],len-2);
		sendBTStatusToDVD(pCollexBTInfo, pCollexBTInfo->sCollexBTInfo.cMobileCallStatus);
		break;
	case 'U':
		if (buf[1] == '0')
		{
			pCollexBTInfo->sCollexBTInfo.bAudioConnectionStatus = FALSE;
		} 
		else
		{
			pCollexBTInfo->sCollexBTInfo.bAudioConnectionStatus = TRUE;
		}
		pCollexBTInfo->sCollexBTInfo.iAudioConnectionStatusTime = GetTickCount();

#if COLLEX_AUDIO_TRANS_USE_COLLEX
		returnMobileAudioTransfer(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.bAudioConnectionStatus);//根据蓝牙返回同步界面
#endif
		break;
	case 'Q'://~~~~~~~~~~~
		if((pCollexBTInfo->sCollexBTInfo.iDeviceNameLength != len - 3)
			||(memcmp(&pCollexBTInfo->sCollexBTInfo.sDeviceName[0],&buf[1],len-3)))
		{
			memset(&pCollexBTInfo->sCollexBTInfo.sDeviceName[0],0,COLLEX_PHONE_NAME);
			memcpy(&pCollexBTInfo->sCollexBTInfo.sDeviceName[0],&buf[1],len-3);
			pCollexBTInfo->sCollexBTInfo.iDeviceNameLength = len - 3;

			pCollexBTInfo->sCollexBTInfo._W_bPairing = FALSE;
			returnMobilePairStatus(pCollexBTInfo,FALSE);//联通
			returnMobileName(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.sDeviceName,pCollexBTInfo->sCollexBTInfo.iDeviceNameLength);
			dealPhoneBook(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.sDeviceName,pCollexBTInfo->sCollexBTInfo.iDeviceNameLength);
		}
		break;
	case 'P':
		pCollexBTInfo->sCollexBTInfo.iPairedStatus = buf[1];
		if ('R' == buf[1])
		{
			returnMobilePairStatus(pCollexBTInfo,TRUE);
		}
		else if ('0' == buf[1])//配对失败
		{
			returnMobilePairStatus(pCollexBTInfo,FALSE);
			pCollexBTInfo->sCollexBTInfo._W_bPairing = FALSE;
		}
		SetEvent(pCollexBTInfo->hCollexBTMainThreadEvent);
		//其它暂不处理
		break;
	case 'X':
		if (buf[1] == '3')
		{
			if (buf[2] == '0')
			{
				DBGI((TEXT("\r\nCollexBT Control Enter Mute :%d"),pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute));
				//if (pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable)
				//{
				//	IpcStartEvent(EVENT_GLOBAL_BT_MUTE_IN_REQ_ID);//发送进入静音
				//	pCollexBTInfo->iControlAudioMuteTimeoutCount = 0;
				//	//while (!IpcWhatEvent(EVENT_GLOBAL_BT_MUTE_IN_OK_ID))//等待OK
				//	//{
				//	//	//DBGE((TEXT("#")));
				//	//	pCollexBTInfo->iControlAudioMuteTimeoutCount++;
				//	//	if (pCollexBTInfo->iControlAudioMuteTimeoutCount > 10)
				//	//	{
				//	//		break;
				//	//	}
				//		Sleep(100);
				//	//}
				//	pCollexBTInfo->bControlAudioMuteLongTime = TRUE;
				//	pCollexBTInfo->iControlAudioMuteTime = GetTickCount();
				//	//IpcClearEvent(EVENT_GLOBAL_BT_MUTE_IN_OK_ID);//清除
				//}
			}
			else if (buf[2] == '1')
			{
				if (pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable)
				{
					pCollexBTInfo->bControlAudioMuteLongTime = FALSE;
					pCollexBTInfo->iControlAudioMuteTime = 0;
					IpcStartEvent(EVENT_GLOBAL_BT_MUTE_OUT_REQ_ID);//发送退出静音
				}
			}	
		}
		else if (buf[1] == '0')
		{
			pCollexBTInfo->sCollexBTInfo.bStereoDeviceConnection = FALSE;
		}
		else
		{
			pCollexBTInfo->sCollexBTInfo.bStereoDeviceConnection = TRUE;
		}
		returnBTA2DPbConnect(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.bStereoDeviceConnection);
		break;
	case 'Y':
		pCollexBTInfo->sCollexBTInfo.iPairedDeviceType = buf[1];
		memcpy(&pCollexBTInfo->sCollexBTInfo.BDAddress[0],&buf[2],6);
		pCollexBTInfo->sCollexBTInfo._W_bPairing = FALSE;
		if ('1' == buf[1] || '2' == buf[1])
		{
			if (len > (2+6+2))
			{
				memset(&pCollexBTInfo->sCollexBTInfo.sDeviceName[0],0,COLLEX_PHONE_NAME);
				memcpy(&pCollexBTInfo->sCollexBTInfo.sDeviceName[0],&buf[8],len-(2+6+2));
				pCollexBTInfo->sCollexBTInfo.iDeviceNameLength = len - (2+6+2);

				returnMobilePairStatus(pCollexBTInfo,FALSE);
				returnMobileName(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.sDeviceName,pCollexBTInfo->sCollexBTInfo.iDeviceNameLength);
				dealPhoneBook(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.sDeviceName,pCollexBTInfo->sCollexBTInfo.iDeviceNameLength);
			}
		}
		break;
	case 'S':
		if (buf[2] == '0')
		{
			pCollexBTInfo->sCollexBTInfo.bPaired = FALSE;
		} 
		else
		{
			pCollexBTInfo->sCollexBTInfo.bPaired = TRUE;

			if (buf[3] == '1')//配对后才有有效连接
			{
				pCollexBTInfo->sCollexBTInfo.bConnected = TRUE;
				pCollexBTInfo->sCollexBTInfo._W_bPairing = FALSE;

				//连接以后，发个获取全部电话本的命令，给API，
				//提高电话本的效率
				controlBTReqPhoneBookListAll(pCollexBTInfo);
			}
		}

		if (buf[3] == '0')
		{
			pCollexBTInfo->sCollexBTInfo.bConnected = FALSE;

			memset(&pCollexBTInfo->sCollexBTInfo.sDeviceName[0],0,COLLEX_PHONE_NAME);
			pCollexBTInfo->sCollexBTInfo.iDeviceNameLength = 0;
			collexPhoneListInit(pCollexBTInfo);
		} 
		
		DBGD((TEXT("********=================, bconnected:%d, bPaired:%d\r\n"),
			pCollexBTInfo->sCollexBTInfo.bConnected,pCollexBTInfo->sCollexBTInfo.bPaired));

		returnMobileLinkStatus(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.bConnected);
		break;
	case 'J':
		if (buf[1] == '0')
		{
			pCollexBTInfo->sCollexBTInfo.mobileBattery = buf[2] - '0';
			returnMobileBattery(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.mobileBattery);
		}
		else if (buf[1] == '1')
		{
			pCollexBTInfo->sCollexBTInfo.mobileSignal = buf[2] - '0';
			returnMobileSignalStrength(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.mobileSignal);
		}
		else if (buf[1] == '2')
		{
			pCollexBTInfo->sCollexBTInfo.mobileVolume = (buf[2] - '0') * 10 + (buf[3] - '0');
		}
		break;

	case 'W':
		if ('A' == pCollexBTInfo->sCollexBTInfo.cMobileCallStatus 
			&& 0 == pCollexBTInfo->sCollexBTInfo.iWaitingCallLen)
		{
			memset(&pCollexBTInfo->sCollexBTInfo.sWaitingCallNumber[0],0,COLLEX_PHONE_NAME);
			memcpy(&pCollexBTInfo->sCollexBTInfo.sWaitingCallNumber[0],
				&buf[1],len-3);
			pCollexBTInfo->sCollexBTInfo.iWaitingCallLen = len - 3;
			returnWaitingCallNumber(pCollexBTInfo,
				pCollexBTInfo->sCollexBTInfo.sWaitingCallNumber,pCollexBTInfo->sCollexBTInfo.iWaitingCallLen);
		}
		break;
	default:
		bMsgHandle = FALSE;
		bRet = TRUE;
		DBGE((TEXT("unHandle!")));
		break;
	}

	if (bMsgHandle && !pCollexBTInfo->sCollexBTInfo.bWork)//蓝牙工作
	{
		pCollexBTInfo->sCollexBTInfo.bWork = TRUE;
		returnBTWorkStatus(pCollexBTInfo, pCollexBTInfo->sCollexBTInfo.bWork);
	}

	if (FALSE == bRet)
	{
		pCollexBTInfo->iAutoResetControlTime = GetTickCount();
	}
	return bRet;
}

static void controlBTPowerControl(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BOOL bPowerOn)
{
	if (bPowerOn)
	{
		collexBTInfoInit(pCollexBTInfo,FALSE);
		collex_PowerControl_On(pCollexBTInfo);//打开蓝牙电源
		pCollexBTInfo->bPower = TRUE;

		pCollexBTInfo->sCollexBTInfo.cMobileCallStatus = 'H';
		returnMobileDialStatus(pCollexBTInfo,&pCollexBTInfo->sCollexBTInfo.cMobileCallStatus,1);
	}
	else
	{
		collex_PowerControl_Off(pCollexBTInfo);//关闭蓝牙电源
		powerNormalDeInit(pCollexBTInfo);
		collexBTInfoInit(pCollexBTInfo,TRUE);
	}
}

static void WinCECommandProcessor(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BYTE *buf,UINT len)
{
	DBGE((TEXT(" OK!")));

	switch(buf[0])
	{
		case 0x01:
			if (0x01 == buf[1])
			{
				pCollexBTInfo->controlPower = FALSE;
				pCollexBTInfo->currentPower = FALSE;
				returnMobileStatusInit(pCollexBTInfo);
				//collexBTInfoInit(pCollexBTInfo,FALSE);
				returnMobilePowerMode(pCollexBTInfo,TRUE);//电源开
				returnMobileWorkMode(pCollexBTInfo,TRUE);//初始化正常
				pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeBTSleep = TRUE;
				pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeBTSleep = FALSE;
				memset(&pCollexBTInfo->sCollexBTInfo.sVersion[0],'\0',8);
			}
			else if (0x00 == buf[1])
			{
				pCollexBTInfo->controlPower = FALSE;
				pCollexBTInfo->currentPower = FALSE;
				collexBTInfoInit(pCollexBTInfo,TRUE);
				returnMobileWorkMode(pCollexBTInfo,FALSE);//初始化中
				returnMobilePowerMode(pCollexBTInfo,FALSE); //电源关
			}
			break;
		case 0x03:
			if (buf[1] == 0x00)
			{

			}
			else 
			{
				if ('H' == pCollexBTInfo->sCollexBTInfo.cMobileCallStatus)
				{
					if (0x01 == buf[1])
					{
						controlBTVolumeControl(pCollexBTInfo,'2');
					} 
					else if (0x02 == buf[1])
					{
						controlBTVolumeControl(pCollexBTInfo,'3');
					}
				}
				else
				{
					if (0x01 == buf[1])
					{
						controlBTVolumeControl(pCollexBTInfo,'0');
					} 
					else if (0x02 == buf[1])
					{
						controlBTVolumeControl(pCollexBTInfo,'1');
					}
				}
			}	 
			break;
		case 0x04:
			controlCurentAudioChannel(pCollexBTInfo);
			controlA2DPButtom(pCollexBTInfo,buf[1]+'0',buf[2]+'0');
			break;
		case 0x10:
			if (0x01 == buf[1])
			{
				pCollexBTInfo->controlPower = TRUE;
				SetEvent(pCollexBTInfo->hCollexBTMainThreadEvent);
				returnBTPowerStatus(pCollexBTInfo,TRUE);
			}
			else
			{
				pCollexBTInfo->controlPower = FALSE;
				SetEvent(pCollexBTInfo->hCollexBTMainThreadEvent);
				returnBTPowerStatus(pCollexBTInfo,FALSE);
			}
			break;

		case 0x20:
			if (pCollexBTInfo->sCollexBTInfo.bWork)
			{
				if (1 == buf[1])//启动配对
				{
					pCollexBTInfo->sCollexBTInfo.iPairedStatus = 0;
					pCollexBTInfo->sCollexBTInfo._W_bPairing = TRUE;
					returnMobilePairStatus(pCollexBTInfo,TRUE);
				}
				else
				{
					pCollexBTInfo->sCollexBTInfo._W_bPairing = FALSE;
					if ('R' == pCollexBTInfo->sCollexBTInfo.iPairedStatus)
					{
						controlBTPair(pCollexBTInfo);
					}
					returnMobilePairStatus(pCollexBTInfo,FALSE);
				}
				SetEvent(pCollexBTInfo->hCollexBTMainThreadEvent);
			}
			DBGD((TEXT("FlyAudio WinCE Pair %d\r\n"),buf[1]));
				break;
		case 0x21:
			if ('A' == pCollexBTInfo->sCollexBTInfo.cMobileCallStatus)
			{
				if (0x01 == buf[1])//手机
				{
					if (pCollexBTInfo->sCollexBTInfo.bAudioConnectionStatus)
					{
						ReturnToUser(buf,len);
						pCollexBTInfo->sCollexBTInfo.iAudioConnectionStatusTime =0;
						if (pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable)
						{
							IpcStartEvent(EVENT_GLOBAL_BT_MUTE_IN_REQ_ID);//发送进入静音
							pCollexBTInfo->iControlAudioMuteTimeoutCount = 0;
							while (!IpcWhatEvent(EVENT_GLOBAL_BT_MUTE_IN_OK_ID))//等待OK
							{
								//DBGE((TEXT("#")));
								pCollexBTInfo->iControlAudioMuteTimeoutCount++;
								if (pCollexBTInfo->iControlAudioMuteTimeoutCount > 10)
								{
									break;
								}
								Sleep(120);
							}
							pCollexBTInfo->iControlAudioMuteTime = GetTickCount();
							IpcClearEvent(EVENT_GLOBAL_BT_MUTE_IN_OK_ID);//清除
						}
						DBGI((TEXT("\r\n++CollexBT Control Enter Mute :%d"),pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute));
						controlBTAudioTrans(pCollexBTInfo,FALSE);	
						pCollexBTInfo->sCollexBTInfo.iAudioConnectionStatusTime =0;
					}
				} 
				else if (0x00 == buf[1])//模块
				{
					if (!pCollexBTInfo->sCollexBTInfo.bAudioConnectionStatus)
					{
						ReturnToUser(buf,len);
						pCollexBTInfo->sCollexBTInfo.iAudioConnectionStatusTime =0;
						if (pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable)
						{
							IpcStartEvent(EVENT_GLOBAL_BT_MUTE_IN_REQ_ID);//发送进入静音
							pCollexBTInfo->iControlAudioMuteTimeoutCount = 0;
							while (!IpcWhatEvent(EVENT_GLOBAL_BT_MUTE_IN_OK_ID))//等待OK
							{
								//DBGE((TEXT("#")));
								pCollexBTInfo->iControlAudioMuteTimeoutCount++;
								if (pCollexBTInfo->iControlAudioMuteTimeoutCount > 10)
								{
									break;
								}
								Sleep(100);
							}
							pCollexBTInfo->iControlAudioMuteTime = GetTickCount();
							IpcClearEvent(EVENT_GLOBAL_BT_MUTE_IN_OK_ID);//清除
						}
						DBGI((TEXT("\r\n--CollexBT Control Enter Mute :%d"),pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bMute));
						controlBTAudioTrans(pCollexBTInfo,TRUE);	
						pCollexBTInfo->sCollexBTInfo.iAudioConnectionStatusTime =0;
					}
				}
			}			
			break;

		case 0x22:
			if (0x01 == buf[1])
			{
				if (!pCollexBTInfo->sCollexBTInfo.bConnected)
				{	
					controlConnectActiveMobilePhoneStatus(pCollexBTInfo, TRUE);
				}
			}
			else
			{
				if (pCollexBTInfo->sCollexBTInfo.bConnected)
				{	
					controlConnectActiveMobilePhoneStatus(pCollexBTInfo, FALSE);
				}
			}
			break;

		case 0x23:
			if (0x02 == buf[1])//呼出电话
			{
				if (pCollexBTInfo->sCollexBTInfo.bConnected)
				{
					if ('H' == pCollexBTInfo->sCollexBTInfo.cMobileCallStatus)
					{
						controlBTDailNumber(pCollexBTInfo,&buf[2],len - 2);
					}
					else
					{
						DBGD((TEXT("\r\ndail but not in H mode")));
					}
				}
				else
				{
					DBGD((TEXT("\r\ndail but not connect")));
				}
			}
			else if (0x00 == buf[1])
			{
				if ('H' == pCollexBTInfo->sCollexBTInfo.cMobileCallStatus)
				{
					returnMobileDialStatus(pCollexBTInfo,&pCollexBTInfo->sCollexBTInfo.cMobileCallStatus,1);
				}
				else
				{
					pCollexBTInfo->iBTSelfReturnHandDownTime = GetTickCount();
				}
				controlBTHangCall(pCollexBTInfo);
			}
			else if (0x01 == buf[1])
			{
				controlBTAnswerCall(pCollexBTInfo);
			}
			break;
		case 0x24:
			if (pCollexBTInfo->sCollexBTInfo.bConnected)
			{
				if ('A' == pCollexBTInfo->sCollexBTInfo.cMobileCallStatus)
				{
					controlBTDTMFKey(pCollexBTInfo,buf[1]);
				}
			}
			break;
		case 0x25:
			if (pCollexBTInfo->sCollexBTInfo.bConnected)
			{
				controlCurrentPhoneBookType(pCollexBTInfo);
			}
			break;

		case 0x26:
			if (buf[1] <= 0x06)
			{
				if (pCollexBTInfo->sCollexBTInfo._W_iPhoneListType != (buf[1] + '0'))
				{
					pCollexBTInfo->sCollexBTInfo._W_iPhoneListType = buf[1] + '0';
					pCollexBTInfo->sCollexBTInfo.iPhoneListStart = 0;
					pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCount = buf[2];
					pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCurrent = 0;

					controlBTSelectPhoneBook(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo._W_iPhoneListType);

					pCollexBTInfo->sCollexBTInfo.bPhoneListNeedReturnFlush = TRUE;
					pCollexBTInfo->sCollexBTInfo.bPhoneListStartReturn = TRUE;
					SetEvent(pCollexBTInfo->hCollexBTMainThreadEvent);
				}
			}
			break;
		case 0x27:
			if (0x01 == buf[1])
			{
				if (pCollexBTInfo->sCollexBTInfo.iPhoneListStart + pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCount
					< phoneListGetSelectCount(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo._W_iPhoneListType))
				{
					pCollexBTInfo->sCollexBTInfo.iPhoneListStart += pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCount;
					pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCurrent = 0;
					pCollexBTInfo->sCollexBTInfo.bPhoneListNeedReturnFlush = TRUE;
					pCollexBTInfo->sCollexBTInfo.bPhoneListStartReturn = TRUE;
				}
			} 
			else if (0x00 == buf[1])
			{
				if (pCollexBTInfo->sCollexBTInfo.iPhoneListStart > pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCount)
				{
					pCollexBTInfo->sCollexBTInfo.iPhoneListStart -= pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCount;
					pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCurrent = 0;
					pCollexBTInfo->sCollexBTInfo.bPhoneListNeedReturnFlush = TRUE;
					pCollexBTInfo->sCollexBTInfo.bPhoneListStartReturn = TRUE;
				}
				else if (pCollexBTInfo->sCollexBTInfo.iPhoneListStart)
				{
					pCollexBTInfo->sCollexBTInfo.iPhoneListStart = 0;
					pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCurrent = 0;
					pCollexBTInfo->sCollexBTInfo.bPhoneListNeedReturnFlush = TRUE;
					pCollexBTInfo->sCollexBTInfo.bPhoneListStartReturn = TRUE;
				}
				else
				{
					pCollexBTInfo->sCollexBTInfo.iPhoneListStart = 0;
					pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCurrent = 0;
				}
			}
			SetEvent(pCollexBTInfo->hCollexBTMainThreadEvent);
			break;

			//case 0x40:
			//	if (0<=buf[1] && buf[1]<4)
			//	{
			//		pCollexBTInfo->sCollexBTInfo.iWaitingCallType = buf[1];
			//		controlWaitingCallType(pCollexBTInfo, pCollexBTInfo->sCollexBTInfo.iWaitingCallType);
			//	}
			//	break;
		case 0x80:
			if (0x01 == buf[1])
			{
				pCollexBTInfo->bUpdater = TRUE;
				controlBTUseInnerMic(pCollexBTInfo,TRUE);
				Sleep(1000);
				closeBTComm(pCollexBTInfo);
				ReturnToUser(buf,len);
			}
			else if (0x00 == buf[1])
			{
				openBTComm(pCollexBTInfo);
				pCollexBTInfo->bUpdater = FALSE;
				ReturnToUser(buf,len);
			}
			break;
		case 0xFF:
			if (0x01 == buf[1])
			{
				FBT_PowerUp((DWORD)pCollexBTInfo);
			} 
			else if (0x00 == buf[1])
			{
				FBT_PowerDown((DWORD)pCollexBTInfo);
			}
		default:
			break;
	}
}

static DWORD WINAPI ThreadCollexBTMainProc(LPVOID pContext)
{
	P_FLY_COLLEX_BT_INFO pCollexBTInfo = (P_FLY_COLLEX_BT_INFO)pContext;
	ULONG WaitReturn;

	while (!pCollexBTInfo->bKillCollexBTMainThread)
	{
		if (pCollexBTInfo->sCollexBTInfo._W_bPairing
			|| pCollexBTInfo->iControlAudioMuteTime)
		{
			WaitReturn = WaitForSingleObject(pCollexBTInfo->hCollexBTMainThreadEvent, 314);
		}
		else
		{
			WaitReturn = WaitForSingleObject(pCollexBTInfo->hCollexBTMainThreadEvent, 2000);
		}

		if (pCollexBTInfo->iBTSelfReturnHandDownTime
			&& GetTickCount() - pCollexBTInfo->iBTSelfReturnHandDownTime >= 1500)
		{
			pCollexBTInfo->iBTSelfReturnHandDownTime = 0;
			if ('H' != pCollexBTInfo->sCollexBTInfo.cMobileCallStatus)
			{
				pCollexBTInfo->sCollexBTInfo.cMobileCallStatus = 'H';
				returnMobileDialStatus(pCollexBTInfo,&pCollexBTInfo->sCollexBTInfo.cMobileCallStatus,1);
			}
		}

		/*if (pCollexBTInfo->iControlAudioMuteTime
			&& 
			((pCollexBTInfo->bControlAudioMuteLongTime && GetTickCount() - pCollexBTInfo->iControlAudioMuteTime >= 5000)
			|| (!pCollexBTInfo->bControlAudioMuteLongTime && GetTickCount() - pCollexBTInfo->iControlAudioMuteTime >= 1200))
			)
		{
			DBGD((TEXT("\r\nCollexBT Auto Exit Mute")));
			pCollexBTInfo->bControlAudioMuteLongTime = FALSE;
			pCollexBTInfo->iControlAudioMuteTime = 0;
			if (pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable)
			{
				IpcStartEvent(EVENT_GLOBAL_BT_MUTE_OUT_REQ_ID);//发送退出静音
			}
		}*/

		if (IpcWhatEvent(EVENT_GLOBAL_SLEEP_PROC_BT_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_SLEEP_PROC_BT_ID);
			if (pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeBTSleep
				!= pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeBTSleep)
			{
				if (pCollexBTInfo->sCollexBTInfo.bAudioConnectionStatus)//切回手机
				{
					controlBTAudioTrans(pCollexBTInfo,FALSE);
				}
				if (pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRecWinCEFactoryMsg)
				{
					controlBTDeletePairedDevice(pCollexBTInfo);
				}
				//Sleep(1000);
				pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeBTSleep
					= pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeBTSleep;
			}
		}

		if (IpcWhatEvent(EVENT_GLOBAL_STANDBY_BT_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_STANDBY_BT_ID);
		}

		if (pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)//电源控制
		{
			if (pCollexBTInfo->currentPower)
			{
				if (pCollexBTInfo->sCollexBTInfo.bAudioConnectionStatus)//切回手机
				{
					controlBTAudioTrans(pCollexBTInfo,FALSE);
				}
				Sleep(100);
				pCollexBTInfo->currentPower = FALSE;
				controlBTPowerControl(pCollexBTInfo,FALSE);
			}
		}
		else if (pCollexBTInfo->controlPower)
		{
			if (!pCollexBTInfo->currentPower)
			{
				pCollexBTInfo->currentPower = TRUE;
				controlBTPowerControl(pCollexBTInfo,TRUE);
			}
		}
		else if (!pCollexBTInfo->controlPower)
		{
			if (pCollexBTInfo->currentPower)
			{
				if (pCollexBTInfo->sCollexBTInfo.bAudioConnectionStatus)//切回手机
				{
					controlBTAudioTrans(pCollexBTInfo,FALSE);
				}
				if (pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable)
				{
					if ('H' != pCollexBTInfo->sCollexBTInfo.cMobileCallStatus)
					{
						IpcStartEvent(EVENT_GLOBAL_BT_MUTE_IN_REQ_ID);//发送进入静音
						pCollexBTInfo->iControlAudioMuteTimeoutCount = 0;
						while (!IpcWhatEvent(EVENT_GLOBAL_BT_MUTE_IN_OK_ID))//等待OK
						{
							//DBGE((TEXT("#")));
							pCollexBTInfo->iControlAudioMuteTimeoutCount++;
							if (pCollexBTInfo->iControlAudioMuteTimeoutCount > 10)
							{
								break;
							}
							Sleep(100);
						}
						IpcClearEvent(EVENT_GLOBAL_BT_MUTE_IN_OK_ID);//清除
					}
				}

				pCollexBTInfo->currentPower = FALSE;
				controlBTPowerControl(pCollexBTInfo,FALSE);
				Sleep(618);
				if (pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bAudioMuteControlable)
				{
					IpcStartEvent(EVENT_GLOBAL_BT_MUTE_OUT_REQ_ID);//发送退出静音
				}
			}
		}

		if (pCollexBTInfo->bPower)//复位
		{
			if (!pCollexBTInfo->bPowerUp)
			{
				DBGE((TEXT("\r\nCollex Reset")));
				//collex_ChipEnableControl_On(pCollexBTInfo);
	
				//collex_ResetControl_On(pCollexBTInfo);

				//collex_PowerControl_Off(pCollexBTInfo);
				//Sleep(100);
				//collex_PowerControl_On(pCollexBTInfo);
				//Sleep(100);
				//collex_ResetControl_Off(pCollexBTInfo);
				//Sleep(314);

				pCollexBTInfo->bPowerUp = TRUE;
			}
		}

		controlBTReqVersion(pCollexBTInfo);

		if (pCollexBTInfo->bPowerUp)//串口OK且蓝牙打开
		{
			if (!pCollexBTInfo->sCollexBTInfo.bWork)//查询蓝牙工作状态
			{
				//controlBTReqVersion(pCollexBTInfo);
				//controlBTUseInnerMic(pCollexBTInfo,TRUE);
			}
			else//控制蓝牙复位
			{
				if (!pCollexBTInfo->bUpdater)//蓝牙死机的复位
				{
					if (GetTickCount() - pCollexBTInfo->iAutoResetControlTime >= 1000
						&& GetTickCount() - pCollexBTInfo->iAutoResetControlTime < 10*1000)
					{
						if (pCollexBTInfo->sCollexBTInfo.bConnected)
						{
							if (0 == pCollexBTInfo->sCollexBTInfo.iControlReqStep)
							{
								pCollexBTInfo->sCollexBTInfo.iControlReqStep = 1;
								//controlBTReqMobileName(pCollexBTInfo);//~~~~~~~~~
							}
							else if (1 == pCollexBTInfo->sCollexBTInfo.iControlReqStep)
							{
								pCollexBTInfo->sCollexBTInfo.iControlReqStep = 0;
								if (!pCollexBTInfo->sCollexBTInfo.bStereoDeviceConnection)
								{
									controlConnectA2DP(pCollexBTInfo,TRUE);
								}
							}
						}
						else
						{								

							if (0 == pCollexBTInfo->sCollexBTInfo.iControlReqStep)
							{
								pCollexBTInfo->sCollexBTInfo.iControlReqStep = 1;
								controlBTReqVersion(pCollexBTInfo);
							}
							else if (1 == pCollexBTInfo->sCollexBTInfo.iControlReqStep)
							{
								pCollexBTInfo->sCollexBTInfo.iControlReqStep = 0;
								controlBTUseInnerMic(pCollexBTInfo,TRUE);
							}
						}
					}
					else if (GetTickCount() - pCollexBTInfo->iAutoResetControlTime >= 10*1000)
					{
						//collex_ResetControl_On(pCollexBTInfo);

						//collex_PowerControl_Off(pCollexBTInfo);
						//Sleep(100);
						//collex_PowerControl_On(pCollexBTInfo);
						//Sleep(100);
						//collex_ResetControl_Off(pCollexBTInfo);
						//Sleep(314);
						//controlBTPowerControl(pCollexBTInfo,FALSE);
						//controlBTPowerControl(pCollexBTInfo,TRUE);
					}
				}
			}
			
			if (pCollexBTInfo->sCollexBTInfo.iAudioConnectionStatusTime)
			{
				if (GetTickCount() - pCollexBTInfo->sCollexBTInfo.iAudioConnectionStatusTime > 618*4)
				{
					pCollexBTInfo->sCollexBTInfo.iAudioConnectionStatusTime = 0;

					if ('A' == pCollexBTInfo->sCollexBTInfo.cMobileCallStatus)
					{
						returnMobileAudioTransfer(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo.bAudioConnectionStatus);//根据蓝牙返回同步界面
					}
					else if ('H' == pCollexBTInfo->sCollexBTInfo.cMobileCallStatus)//这代码有问题
					{
						if (FALSE == pCollexBTInfo->sCollexBTInfo.bAudioConnectionStatus)
						{
							controlBTAudioTrans(pCollexBTInfo,TRUE);					
						}
					}				
				}
			}

			if (pCollexBTInfo->sCollexBTInfo._W_bPairing)//iPairedStatus 0初始化 1发送了清空 2发送了配对
			{
				if (pCollexBTInfo->sCollexBTInfo.bConnected)
				{
					controlConnectActiveMobilePhoneStatus(pCollexBTInfo,FALSE);
				}
				else if (pCollexBTInfo->sCollexBTInfo.bPaired)
				{
					controlBTDeletePairedDevice(pCollexBTInfo);//删除之前的配对信息
				}
				else if (pCollexBTInfo->sCollexBTInfo.iPairedStatus == 0)
				{
					controlBTDeletePairedDevice(pCollexBTInfo);//删除之前的配对信息
					pCollexBTInfo->sCollexBTInfo.iPairedStatus = 1;
				}
				else if (pCollexBTInfo->sCollexBTInfo.iPairedStatus == '0')
				{
					pCollexBTInfo->sCollexBTInfo._W_bPairing = FALSE;
					//返回需要配对状态 
					returnMobilePairStatus(pCollexBTInfo,TRUE);
				}
				else
				{
					if (pCollexBTInfo->sCollexBTInfo.iPairedStatus != 'R'
						&& pCollexBTInfo->sCollexBTInfo.iPairedStatus != '1')//新蓝牙模块，某个手机的问题
					{
						if (pCollexBTInfo->sCollexBTInfo.iPairedStatus != 2)
						{
							controlBTPair(pCollexBTInfo);
							pCollexBTInfo->sCollexBTInfo.iPairedStatus = 2;//表示已发送
						}
					}
				}
			}
			if (pCollexBTInfo->sCollexBTInfo.bConnected)
			{
				if (0 == pCollexBTInfo->sCollexBTInfo.iDeviceNameLength)
				{
					controlBTReqMobileName(pCollexBTInfo);
				}
				else if (pCollexBTInfo->sCollexBTInfo.bPhoneListStartReturn)//需要返回电话本
				{
					DBGD((TEXT("\r\nThreadCollexBTMainProc need return phonebook")));
						//if (pCollexBTInfo->sCollexBTInfo._W_iPhoneListType == pCollexBTInfo->sCollexBTInfo.iPhoneListType)
						if (pCollexBTInfo->sCollexBTInfo._W_iPhoneListType >= '1'
							&& pCollexBTInfo->sCollexBTInfo._W_iPhoneListType <= '6')//返回某个电话本
						{
							DBGD((TEXT(" curCount %d,needCount %d, startCount %d, totalCount %d")
								,pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCurrent
								,pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCount
								,pCollexBTInfo->sCollexBTInfo.iPhoneListStart
								,phoneListGetSelectCount(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo._W_iPhoneListType)));
								while (pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCurrent != pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCount)//没有返回足够数量
								{
									DBGE((TEXT(" needreturnmore")));
									if (0 == pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCurrent)//每页开始返回时
									{
										UINT iSelectCount = phoneListGetSelectCount(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo._W_iPhoneListType);
										if (pCollexBTInfo->sCollexBTInfo.iPhoneListStart >= iSelectCount)//判断是否超过
										{
											if (pCollexBTInfo->sCollexBTInfo.iPhoneListStart)//跳过全都没有的情况
											{
												pCollexBTInfo->sCollexBTInfo.iPhoneListStart = iSelectCount - 1;
											}
										}
									}
									P_COLLEX_BT_PHONE_LIST p;
									p = phoneListGetSelectOne(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo._W_iPhoneListType,pCollexBTInfo->sCollexBTInfo.iPhoneListStart+pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCurrent);
									if (p)
									{
										DBGD((TEXT(" index:%d"),pCollexBTInfo->sCollexBTInfo.iPhoneListStart+pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCurrent));
											if (0 == pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCurrent)//返回开始前刷新页面
											{
												returnBTPhoneBookPageFlush(pCollexBTInfo);
											}
											returnMobilePhoneList(pCollexBTInfo,p);
											pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCurrent++;
									}
									else
									{
										break;
									}
								}
						}
						pCollexBTInfo->sCollexBTInfo.bPhoneListStartReturn = FALSE;
						if (pCollexBTInfo->sCollexBTInfo.bPhoneListNeedReturnFlush)
						{
							DBGD((TEXT("NeedFlush RecFinish%d Max%d")
								,pCollexBTInfo->sCollexBTInfo.bPhoneListPhoneReadFinish[pCollexBTInfo->sCollexBTInfo._W_iPhoneListType - '0']
							,phoneListGetSelectCount(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo._W_iPhoneListType)));

								if (pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCurrent == pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCount)
								{
									returnBTPhoneBookReadStatus(pCollexBTInfo,1);
									pCollexBTInfo->sCollexBTInfo.bPhoneListNeedReturnFlush = FALSE;
								}
								else if (pCollexBTInfo->sCollexBTInfo.bPhoneListPhoneReadFinish[pCollexBTInfo->sCollexBTInfo._W_iPhoneListType - '0'])
								{
									if (pCollexBTInfo->sCollexBTInfo.iPhoneListStart+pCollexBTInfo->sCollexBTInfo.iPhoneListReturnCurrent
										>= phoneListGetSelectCount(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo._W_iPhoneListType))
									{
										returnBTPhoneBookReadStatus(pCollexBTInfo,1);
										pCollexBTInfo->sCollexBTInfo.bPhoneListNeedReturnFlush = FALSE;
									}
								}
						}
				}
				else if (pCollexBTInfo->sCollexBTInfo._W_iPhoneListType != pCollexBTInfo->sCollexBTInfo.iPhoneListType)//电话本不匹配
				{
					if (0xFF != pCollexBTInfo->sCollexBTInfo.iPhoneListType)
					{
						controlBTSelectPhoneBook(pCollexBTInfo,pCollexBTInfo->sCollexBTInfo._W_iPhoneListType);
					}
				}
			}
			else
			{
				if (pCollexBTInfo->sCollexBTInfo.bWork)//尝试连接
				{
					if (pCollexBTInfo->sCollexBTInfo.bPaired && 'R' != pCollexBTInfo->sCollexBTInfo.iPairedStatus)//不是正在配对状态
					{
						controlConnectActiveMobilePhoneStatus(pCollexBTInfo,TRUE);
					}
				}
			}
		}

		if (!pCollexBTInfo->bSocketConnecting)
		{
			DBGE((TEXT("\r\nBT socket can't recv msg(ensure the socket is connect and send init commd)\r\n")));
		}
	}
	pCollexBTInfo->hCollexBTMainThread = NULL;
	DBGE((TEXT("\r\nThreadCollexBTMainProc exit")));
	return 0;
}

void collexBT_Enable(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BOOL bEnable)
{
	if (bEnable)
	{
		DBGD((TEXT("\r\nCollexBT_Enable Enable!")));

		//collex_PowerControl_Off(pCollexBTInfo);
		collex_ChipEnableControl_On(pCollexBTInfo);
		collex_ResetControl_On(pCollexBTInfo);

		pCollexBTInfo->bKillCollexBTMainThread = FALSE;
		DWORD dwThreadID;
		pCollexBTInfo->hCollexBTMainThreadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
		pCollexBTInfo->hCollexBTMainThread = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL,0,
			(LPTHREAD_START_ROUTINE)ThreadCollexBTMainProc, pCollexBTInfo,0, &dwThreadID );
		DBGD((TEXT("\r\nThreadCollexBTMainProc ID:%x"),dwThreadID));

		if (NULL == pCollexBTInfo->hCollexBTMainThread)
		{
			pCollexBTInfo->bKillCollexBTMainThread = TRUE;
			return;
		}
		openBTComm(pCollexBTInfo);
	}
	else
	{
		DBGD((TEXT("\r\nCollexBT_Enable Disable!")));

		if (pCollexBTInfo->sCollexBTInfo.bConnected)
		{
			//断开手机连接
			pCollexBTInfo->sCollexBTInfo.bConnected = FALSE;
			controlConnectActiveMobilePhoneStatus(pCollexBTInfo,FALSE);
		}

		pCollexBTInfo->bKillCollexBTMainThread = TRUE;
		SetEvent(pCollexBTInfo->hCollexBTMainThreadEvent);
		while (pCollexBTInfo->hCollexBTMainThread)
		{
			SetEvent(pCollexBTInfo->hCollexBTMainThreadEvent);
			Sleep(10);
		}

		CloseHandle(pCollexBTInfo->hCollexBTMainThread);
		pCollexBTInfo->hCollexBTMainThread = NULL;
		pCollexBTInfo->hCollexBTMainThreadEvent = NULL;
		closeBTComm(pCollexBTInfo); 
		collex_PowerControl_Off(pCollexBTInfo);
	}
}

void powerNormalDeInit(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	pCollexBTInfo->BTInfoFrameStatus = 0;

	collexBTInfoInit(pCollexBTInfo,TRUE);
	pCollexBTInfo->bPower = FALSE;
	pCollexBTInfo->bPowerUp = FALSE;
	//collex_PowerControl_Off(pCollexBTInfo);
	//collex_ChipEnableControl_On(pCollexBTInfo);
	//collex_ResetControl_On(pCollexBTInfo);
}

static void powerNormalInit(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	pCollexBTInfo->buffToUserHx = 0;
	pCollexBTInfo->buffToUserLx = 0;

	pCollexBTInfo->BTInfoFrameStatus = 0;

	pCollexBTInfo->iBTSelfReturnHandDownTime = 0;

	//collex_PowerControl_Off(pCollexBTInfo);
	//collex_ChipEnableControl_On(pCollexBTInfo);
	//collex_ResetControl_On(pCollexBTInfo);
}

static void powerOnFirstInit(P_FLY_COLLEX_BT_INFO pCollexBTInfo)
{
	pCollexBTInfo->bOpen = FALSE;
	pCollexBTInfo->bPower = FALSE;
	pCollexBTInfo->bSpecialPower = FALSE;
	pCollexBTInfo->bPowerUp = FALSE;

	pCollexBTInfo->bUpdater = FALSE;

	pCollexBTInfo->bKillCollexBTMainThread = TRUE;
	pCollexBTInfo->hCollexBTMainThread = NULL;
	pCollexBTInfo->hCollexBTMainThreadEvent = NULL;

	pCollexBTInfo->hBTComm = NULL;
	pCollexBTInfo->bKillFlyBTCommThread = TRUE;
	pCollexBTInfo->hThreadHandleFlyBTComm = NULL;
	pCollexBTInfo->BTInfoFrameStatus = 0;

	pCollexBTInfo->sCollexBTInfo.pBTPhoneList = NULL;
	collexBTInfoInit(pCollexBTInfo,TRUE);

	pCollexBTInfo->iControlAudioMuteTime = 0;
	pCollexBTInfo->bControlAudioMuteLongTime = FALSE;
}

static void powerOnSpecialEnable(P_FLY_COLLEX_BT_INFO pCollexBTInfo,BOOL bOn)
{
	if (bOn)
	{
		if (pCollexBTInfo->bSpecialPower)
		{
			return;
		}
		pCollexBTInfo->bSpecialPower = TRUE;

		pCollexBTInfo->hBuffToUserDataEvent = CreateEvent(NULL,FALSE,FALSE,NULL);	
		InitializeCriticalSection(&pCollexBTInfo->hCSSendToUser);
	} 
	else
	{
		if (!pCollexBTInfo->bSpecialPower)
		{
			return;
		}
		pCollexBTInfo->bSpecialPower = FALSE;

		collexBTInfoInit(pCollexBTInfo,TRUE);
		CloseHandle(pCollexBTInfo->hBuffToUserDataEvent);
	}
}


void IpcRecv(UINT32 iEvent)
{
	P_FLY_COLLEX_BT_INFO	pCollexBTInfo = (P_FLY_COLLEX_BT_INFO)gpCollexBTInfo;

	DBGD((TEXT("\r\n FLY bt Recv IPC Event:%d"),iEvent));

	if (EVENT_GLOBAL_A2DPAUDIO_CHANNEL_ID == iEvent)
	{
		controlCurentAudioChannel(pCollexBTInfo);
	}

	SetEvent(pCollexBTInfo->hCollexBTMainThreadEvent);
}
void SockRecv(BYTE *buf, UINT16 len)
{
	UINT16 i=0;
	P_FLY_COLLEX_BT_INFO	pCollexBTInfo = (P_FLY_COLLEX_BT_INFO)gpCollexBTInfo;

	if (len <= 0)
		return;

	DBGD((TEXT("\r\n FLY bt socket recv %d bytes:%c "),len-1, buf[0]));
	for (i=1; i<len-1; i++)
		DBGD((TEXT("%02X "), buf[i]));

	DealBTInfo(pCollexBTInfo,buf,len-1);//去掉检验位

	if (!pCollexBTInfo->bSocketConnecting)
		pCollexBTInfo->bSocketConnecting  = TRUE;
}


HANDLE FBT_Init(DWORD dwContext)
{
	P_FLY_COLLEX_BT_INFO pCollexBTInfo;

	pCollexBTInfo = (P_FLY_COLLEX_BT_INFO)LocalAlloc(LPTR, sizeof(FLY_COLLEX_BT_INFO));
	if (!pCollexBTInfo)
	{
		return NULL;
	}
	gpCollexBTInfo = pCollexBTInfo;
	
	pCollexBTInfo->pFlyShmGlobalInfo = CreateShm(BT_MODULE,IpcRecv);
	if (NULL == pCollexBTInfo->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio CollexBT Init create shm err\r\n")));
		return NULL;
	}
	
	if (!GetDllAddrTable())
	{
		DBGE((TEXT("FlyAudio CollexBT  GetDllAddrTable err\r\n")));
		return NULL;
	}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio CollexBT Open create user buf err\r\n")));
		return NULL;
	}

	if(!CreateServerSocket(SockRecv, TCP_PORT_BT))
	{
		DBGE((TEXT("FlyAudio CollexBT Open create server socket err\r\n")));
		return NULL;
	}

	pCollexBTInfo->controlPower = FALSE;
	pCollexBTInfo->currentPower = FALSE;

	//参数初始化
	powerNormalInit(pCollexBTInfo);
	powerOnFirstInit(pCollexBTInfo);
	powerOnSpecialEnable(pCollexBTInfo,TRUE);
	
	//pCollexBTInfo->hHandleGlobalGlobalEvent = CreateEvent(NULL,FALSE,FALSE,DATA_GLOBAL_HANDLE_GLOBAL);
	
	pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalBTInfo.bInit = TRUE;
	pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalBTInfo.iDriverCompYear = year;
	pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalBTInfo.iDriverCompMon = months;
	pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalBTInfo.iDriverCompDay = day;
	pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalBTInfo.iDriverCompHour = hours;
	pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalBTInfo.iDriverCompMin = minutes;
	pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalBTInfo.iDriverCompSec = seconds;

	DBGI((TEXT("\r\nCollexBT Init Build:Date%d-%d-%d Time%d:%d:%d"),year,months,day,hours,minutes,seconds));

	return (HANDLE)pCollexBTInfo;
}

BOOL FBT_Deinit(DWORD hDeviceContext)
{
	P_FLY_COLLEX_BT_INFO pCollexBTInfo = (P_FLY_COLLEX_BT_INFO)hDeviceContext;
	DBGI((TEXT("FlyAudio CollexBT DeInit\r\n")));

	pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalBTInfo.bInit = FALSE;
	//SetEvent(pCollexBTInfo->hHandleGlobalGlobalEvent);
	//CloseHandle(pCollexBTInfo->hHandleGlobalGlobalEvent);

	powerOnSpecialEnable(pCollexBTInfo,FALSE);

	FreeShm();
	FreeSocketServer();
	FreeUserBuff();

	LocalFree(pCollexBTInfo);
	return TRUE;
}

DWORD
FBT_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	P_FLY_COLLEX_BT_INFO pCollexBTInfo = (P_FLY_COLLEX_BT_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;

	pCollexBTInfo->bOpen = TRUE;

	collexBT_Enable(pCollexBTInfo,TRUE);
	pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalBTInfo.bOpen = TRUE;

	DBGI((TEXT("\r\nCollexBT Open Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return returnWhat;
}

BOOL
FBT_Close(DWORD hDeviceContext)
{
	P_FLY_COLLEX_BT_INFO pCollexBTInfo = (P_FLY_COLLEX_BT_INFO)hDeviceContext;

	pCollexBTInfo->bOpen = FALSE;
	pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalBTInfo.bOpen = FALSE;

	collexBT_Enable(pCollexBTInfo,FALSE);

	DBGI((TEXT("FlyAudio CollexBT Close\r\n")));
		return TRUE;
}

void FBT_PowerUp(DWORD hDeviceContext)
{
	P_FLY_COLLEX_BT_INFO pCollexBTInfo = (P_FLY_COLLEX_BT_INFO)hDeviceContext;

	powerNormalInit(pCollexBTInfo);

	DBGI((TEXT("FlyAudio CollexBT PowerUp!\r\n")));
}

void FBT_PowerDown(DWORD hDeviceContext)
{
	P_FLY_COLLEX_BT_INFO pCollexBTInfo = (P_FLY_COLLEX_BT_INFO)hDeviceContext;

	powerNormalDeInit(pCollexBTInfo);

	pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNeedProcBeforeBTSleep = FALSE;
	pCollexBTInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveProcBeforeBTSleep = FALSE;

	DBGI((TEXT("FlyAudio CollexBT PowerDown!\r\n")));
}

DWORD FBT_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	DWORD dwRead = 0,i=0;
	P_FLY_COLLEX_BT_INFO pCollexBTInfo = (P_FLY_COLLEX_BT_INFO)hOpenContext;

	BYTE *buf = (BYTE*)pBuffer;
	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio CollexBT return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}

	return dwRead;
}

DWORD FBT_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	P_FLY_COLLEX_BT_INFO pCollexBTInfo = (P_FLY_COLLEX_BT_INFO)hOpenContext;

	DBGD((TEXT("\r\nCollexBT Write %d bytes:"),NumberOfBytes));
	for(UINT i = 0;i < NumberOfBytes;i++)
	{
		DBGD((TEXT(" %X"),*((BYTE *)pSourceBytes + i)));
	}

	if(NumberOfBytes)
	{
		WinCECommandProcessor(pCollexBTInfo,(((BYTE *)pSourceBytes)+3),((BYTE *)pSourceBytes)[2]-1);
	}
	return NULL;
}

DWORD FBT_Seek(DWORD hOpenContext, LONG Amount, DWORD  Type)
{
	return NULL;
}

BOOL FBT_IOControl(DWORD hOpenContext,
			  DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn,
			  PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
	P_FLY_COLLEX_BT_INFO pCollexBTInfo = (P_FLY_COLLEX_BT_INFO)hOpenContext;
	BOOL bRes = TRUE;

	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	DBGE((TEXT("\r\nCollex IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)"),
	//		hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	DBGE((TEXT("\r\nCollex IOControl Set IOCTL_SERIAL_SET_WAIT_MASK")));
	//		break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	//DBGE((TEXT("\r\nParrot IOControl Set IOCTL_SERIAL_WAIT_ON_MASK")));
	//	if (pCollexBTInfo->bOpen)
	//	{
	//		WaitForSingleObject(pCollexBTInfo->hBuffToUserDataEvent,INFINITE);
	//	} 
	//	else
	//	{
	//		WaitForSingleObject(pCollexBTInfo->hBuffToUserDataEvent,0);
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

BOOL DllEntry(
		 HINSTANCE hinstDll,
		 DWORD dwReason,
		 LPVOID lpReserved
		 )
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DEBUGMSG(1, (TEXT("Attach in Collex DllEntry\n")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}

	if (dwReason == DLL_PROCESS_DETACH)
	{
		DEBUGMSG(1, (TEXT("Dettach in Collex DllEntry\n")));
	}

	DEBUGMSG(1, (TEXT("Leaving Collex DllEntry\n")));

	return (TRUE);
}
