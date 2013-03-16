// -----------------------------------------------------------------------------
// File Name    : FLYForyou.cpp
// Title        : ForyouDVD Driver
// Author       : JQilin - Copyright (C) 2011
// Created      : 2011-02-25  
// Version      : 0.01
// Target MCU   : WinCE Driver
// -----------------------------------------------------------------------------
// Version History:
/*
>>>2011-03-03: V0.02 添加文件列表功能
>>>2011-02-25: V0.01 first draft
*/
// FLYForyou.cpp : Defines the entry point for the DLL application.
//

#include "flyhwlib.h"
#include "flyuserlib.h"
#include "flysocketlib.h"

#include "FLYForyou.h"

P_FLY_FORYOU_DVD_INFO gpForyouDVDInfo = NULL;

void ForyouDVD_Enable(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bEnable);
VOID FCD_PowerUp(DWORD hDeviceContext);
VOID FCD_PowerDown(DWORD hDeviceContext);
void DVDCmdPrintf(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *buf,UINT len);
static void procDVDStop(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bForceSend);

void DVD_Reset_On(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
{
	if (pForyouDVDInfo->bAutoResetControlOn)
		return;

	BYTE buf[]={0xFF,0x01};
	DVDCmdPrintf(pForyouDVDInfo,buf,2);

	pForyouDVDInfo->bAutoResetControlOn = TRUE;
	pForyouDVDInfo->iAutoResetControlTime = GetTickCount();

	structDVDInfoInit(pForyouDVDInfo,TRUE);

	DBGE((TEXT("\r\nForyouDVD Reset On")));
}

void DVD_Reset_Off(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
{
	if (!pForyouDVDInfo->bAutoResetControlOn)
		return;

	BYTE buf[]={0xFF,0x00};
	DVDCmdPrintf(pForyouDVDInfo,buf,2);

	pForyouDVDInfo->bAutoResetControlOn = FALSE;
	pForyouDVDInfo->iAutoResetControlTime = GetTickCount();
	DBGE((TEXT("\r\nForyouDVD Reset Off")));
}

void DVD_LEDControl_On(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
{
	//SOC_IO_Output(CONTROL_IO_LED_G,CONTROL_IO_LED_I,EJECT_LED_ON);
	DBGE((TEXT("\r\nForyouDVD LEDControl On")));
}

void DVD_LEDControl_Off(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
{
	//SOC_IO_Output(CONTROL_IO_LED_G,CONTROL_IO_LED_I,EJECT_LED_OFF);
	DBGE((TEXT("\r\nForyouDVD LEDControl Off")));
}

void listFileFolderClearAll(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bFile)
{
	if (bFile)
	{
		if (pForyouDVDInfo->sForyouDVDInfo.pFileTreeList)
		{
			delete []pForyouDVDInfo->sForyouDVDInfo.pFileTreeList;
			pForyouDVDInfo->sForyouDVDInfo.pFileTreeList = NULL;
		}
	} 
	else
	{
		if (pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList)
		{
			delete []pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList;
			pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList = NULL;
		}
	}
}

void listFileFolderNewAll(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bFile,UINT iCount)
{
	UINT i;
	if (bFile)
	{
		listFileFolderClearAll(pForyouDVDInfo,TRUE);
		pForyouDVDInfo->sForyouDVDInfo.pFileTreeList = new FOLDER_FILE_TREE_LIST[iCount];
		for (i = 0;i < iCount;i++)
		{
			pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[i].bStorage = FALSE;
		}
	}
	else
	{
		listFileFolderClearAll(pForyouDVDInfo,FALSE);
		pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList = new FOLDER_FILE_TREE_LIST[iCount];
		for (i = 0;i < iCount;i++)
		{
			pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[i].bStorage = FALSE;
		}
	}
}

void listFileFolderStorageOne(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bFile,UINT iCount,UINT parentIndex,UINT extension,BYTE *name,UINT nameLength)
{
	UINT i;
	BYTE temp8;
	BYTE sWideChar[512];
	UINT iWideCharLength;
	if (bFile)
	{
		if (iCount < pForyouDVDInfo->sForyouDVDInfo.pFileCount)
		{
			pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].bStorage = TRUE;
			pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].bFolder = FALSE;
			pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].parentFolderIndex = parentIndex;
			pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].extension = extension;

			if (nameLength < 256-1)
			{
				if (!pForyouDVDInfo->bFlyaudioDVD)//华阳碟机要交换一下
				{
					memcpy(pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name,name,nameLength);
					for (i = 0; i < nameLength; i += 2)
					{
						temp8 = pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name[i];
						pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name[i] = pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name[i+1];
						pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name[i+1] = temp8;
					}
					pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name[nameLength] = 0;
					pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name[nameLength+1] = 0;
					pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].nameLength = nameLength + 2;
				}
				else
				{
					memcpy(pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name,name,nameLength);
					pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name[nameLength] = 0;
					pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name[nameLength+1] = 0;
					//RETAILMSG(1,(TEXT("\r\nFileGB2312:")));
					//for (i = 0;i < nameLength+2;i++)
					//{
					//	RETAILMSG(1,(TEXT(" %x"),pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name[i]));
					//}

					//ASCII或者GB2312 To Unicode
					//iWideCharLength = MultiByteToWideChar (CHINA_CODE_PAGE, 0, (char*)(pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name), -1, NULL, 0);
					//RETAILMSG(1,(TEXT("\r\niWideCharLength:%d!"),iWideCharLength));
					//memset(sWideChar,0,512);
					//MultiByteToWideChar (CHINA_CODE_PAGE, 0, (char*)(pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name), -1, (WCHAR*)sWideChar, iWideCharLength);
					
					iWideCharLength
						= MultiByteToWideChar (CHINA_CODE_PAGE, 0, 
						(char*)(pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name), nameLength, (WCHAR*)sWideChar, 512);
					
					//RETAILMSG(1,(TEXT("\r\nUnicode:")));
					//for (i = 0;i < iWideCharLength;i++)
					//{
					//	RETAILMSG(1,(TEXT(" %x"),sWideChar[i]));
					//}
					memcpy(pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name,sWideChar,iWideCharLength*2);
					pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name[iWideCharLength*2] = 0;
					pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name[iWideCharLength*2+1] = 0;
					pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].nameLength = iWideCharLength*2 + 2;
				}
			} 
			else
			{
				pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].nameLength = 0;
			}
		}
	} 
	else
	{
		if (iCount < pForyouDVDInfo->sForyouDVDInfo.pFolderCount)
		{
			pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].bStorage = TRUE;
			pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].bFolder = TRUE;
			pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].parentFolderIndex = parentIndex;
			pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].extension = 0xFF;

			if (nameLength < 256-1)
			{
				if (!pForyouDVDInfo->bFlyaudioDVD)//华阳碟机要交换一下
				{
					memcpy(pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name,name,nameLength);
					for (i = 0; i < nameLength; i += 2)
					{
						temp8 = pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name[i];
						pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name[i] = pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name[i+1];
						pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name[i+1] = temp8;
					}
					pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name[nameLength] = 0;
					pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name[nameLength+1] = 0;
					pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].nameLength = nameLength + 2;
				}
				else
				{
					memcpy(pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name,name,nameLength);
					pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name[nameLength] = 0;
					pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name[nameLength+1] = 0;
					//RETAILMSG(1,(TEXT("\r\nFileGB2312:")));
					//for (i = 0;i < nameLength+2;i++)
					//{
					//	RETAILMSG(1,(TEXT(" %x"),pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name[i]));
					//}
					
					//ASCII或者GB2312 To Unicode
					//iWideCharLength = MultiByteToWideChar (CHINA_CODE_PAGE, 0, (char*)(pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name), -1, NULL, 0);
					//RETAILMSG(1,(TEXT("\r\nRadioCurText nTextLen:%d!\r\n"),iWideCharLength));
					//memset(sWideChar,0,512);
					//MultiByteToWideChar (CHINA_CODE_PAGE, 0, (char*)(pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name), -1, (WCHAR*)sWideChar, iWideCharLength);

					iWideCharLength
						= MultiByteToWideChar (CHINA_CODE_PAGE, 0, 
						(char*)(pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name), nameLength, (WCHAR*)sWideChar, 512);

					//RETAILMSG(1,(TEXT("\r\nUnicode:")));
					//for (i = 0;i < iWideCharLength*2;i++)
					//{
					//	RETAILMSG(1,(TEXT(" %x"),sWideChar[i]));
					//}
					memcpy(pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name,sWideChar,iWideCharLength*2);
					pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name[iWideCharLength*2] = 0;
					pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].name[iWideCharLength*2+1] = 0;
					pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].nameLength = iWideCharLength*2 + 2;
				}
			}
			else
			{
				pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[iCount].nameLength = 0;
			}
		}
	}
}

UINT getListFolderFileSelectParentIndex(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT index)
{
	UINT returnIndex = 0;

	if (0 == index)
	{
	}
	else
	{
		if (index < pForyouDVDInfo->sForyouDVDInfo.pFolderCount)
		{
			if (pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[index].bStorage)
			{
				returnIndex = pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[index].parentFolderIndex;
			}
		}
	}
	return returnIndex;
}

UINT getSelectParentFolderFileCount(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT parentIndex,BOOL bFolder)//返回同目录下文件或文件夹数量
{
	UINT i;
	UINT iCount = 0;
	if (bFolder)
	{
		for (i = 0;i < pForyouDVDInfo->sForyouDVDInfo.pFolderCount;i++)
		{
			if (pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[i].parentFolderIndex == parentIndex)
			{
				iCount++;
			}
		} 
	}
	else
	{
		for (i = 0;i < pForyouDVDInfo->sForyouDVDInfo.pFileCount;i++)
		{
			if (pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[i].parentFolderIndex == parentIndex)
			{
				iCount++;
			}
		} 
	}
	return iCount;
}

UINT getSelectParentFolderFileInfoBySubIndex(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT parentIndex,BOOL bFolder,UINT subIndex)//根据目录和子序号，返回全局序号
{
	UINT i;
	UINT iCount = 0;
	if (bFolder)
	{
		for (i = 0;i < pForyouDVDInfo->sForyouDVDInfo.pFolderCount;i++)
		{
			if (pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[i].parentFolderIndex == parentIndex)
			{
				if (iCount == subIndex)
				{
					return i;
				}
				iCount++;
			}
		} 
	}
	else
	{
		for (i = 0;i < pForyouDVDInfo->sForyouDVDInfo.pFileCount;i++)
		{
			if (pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[i].parentFolderIndex == parentIndex)
			{
				if (iCount == subIndex)
				{
					return i;
				}
				iCount++;
			}
		} 
	}
	return -1;
}

UINT getSelectFolderFileIndexByGlobalIndexInParent(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bFolder,UINT globalIndex)//根据全局序号，返回同目录下的子序号,此处特殊和协议的上一级目录序号有关
{
	UINT i;
	UINT iParent;
	UINT iFolderCount = 0;
	UINT iFileCount = 0;
	if (bFolder)
	{
		iParent = pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[globalIndex].parentFolderIndex;
		for (i = 0;i < pForyouDVDInfo->sForyouDVDInfo.pFolderCount;i++)
		{
			if (i == globalIndex)
			{
				if (iParent)
				{
					return iFolderCount + 1;
				}
				else
				{
					return iFolderCount;
				}
			}
			if (iParent == pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[i].parentFolderIndex)
			{
				iFolderCount++;
			}
		}
	}
	else
	{
		iParent = pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[globalIndex].parentFolderIndex;
		iFolderCount = getSelectParentFolderFileCount(pForyouDVDInfo,iParent,TRUE);
		for (i = 0;i < pForyouDVDInfo->sForyouDVDInfo.pFileCount;i++)
		{
			if (i == globalIndex)
			{
				if (iParent)
				{
					return iFolderCount + iFileCount + 1;
				}
				else
				{
					return iFolderCount + iFileCount;
				}
			}
			if (iParent == pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[i].parentFolderIndex)
			{
				iFileCount++;
			}
		}
	}
	return -1;
}

void OnDataRecv(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo, BYTE data)
{
	//UINT i,j;
	//switch (pForyouDVDInfo->DVDInfoFrameStatus)
	//{
	//case 0:
	//	if (data == 0xff)
	//	{
	//		pForyouDVDInfo->DVDInfoFrameStatus = 1;
	//	}
	//	break;
	//case 1:
	//	if (data == 0x55) 
	//	{
	//		pForyouDVDInfo->DVDInfoFrameStatus = 2;
	//	}
	//	else 
	//	{
	//		pForyouDVDInfo->DVDInfoFrameStatus = 0;
	//	}
	//	break;
	//case 2:
	//	if(255 == data)//这个判断为了解决华阳连发两次FF 55
	//	{
	//		pForyouDVDInfo->DVDInfoFrameStatus = 1;
	//		break;
	//	}
	//	pForyouDVDInfo->DVDInfoFrameStatus = 3;
	//	pForyouDVDInfo->DVDInfoFrameChecksum = pForyouDVDInfo->DVDInfoFrameLengthMax = data;
	//	pForyouDVDInfo->DVDInfoFrameLength = 0;
	//	break;
	//case 3:
	//	if (pForyouDVDInfo->DVDInfoFrameLength < pForyouDVDInfo->DVDInfoFrameLengthMax) 
	//	{
	//		pForyouDVDInfo->DVDInfoFrameBuff[pForyouDVDInfo->DVDInfoFrameLength] = data;
	//		pForyouDVDInfo->DVDInfoFrameChecksum += data;
	//		pForyouDVDInfo->DVDInfoFrameLength ++;
	//	}
	//	else 
	//	{
	//		if (data == (BYTE)(0xff - pForyouDVDInfo->DVDInfoFrameChecksum)) 
	//		{
	//			if (pForyouDVDInfo->bFlyaudioDVD)
	//			{
	//				DealFlyaudioInfo(pForyouDVDInfo,pForyouDVDInfo->DVDInfoFrameBuff,pForyouDVDInfo->DVDInfoFrameLength);
	//			}
	//			else
	//			{
	//				DealDVDInfo(pForyouDVDInfo,pForyouDVDInfo->DVDInfoFrameBuff,pForyouDVDInfo->DVDInfoFrameLength);
	//			}
	//		}
	//		else
	//		{
	//			DBGE((TEXT("\r\nForyou COM CRC ERROR!!!!!!!!")));
	//			for (i = 0;i < pForyouDVDInfo->DVDInfoFrameLength;i++)
	//			{
	//				DBGE((TEXT(" %x"),pForyouDVDInfo->DVDInfoFrameBuff[i]));
	//			}

	//			for (j = 0;j < pForyouDVDInfo->DVDInfoFrameLength;j++)//CRC错误时，尽量减少损失
	//			{
	//				if (0xFF == pForyouDVDInfo->DVDInfoFrameBuff[j])
	//				{
	//					memcpy(pForyouDVDInfo->DVDInfoFrameCRCErrorTailBuff
	//						,&pForyouDVDInfo->DVDInfoFrameBuff[j]
	//					,pForyouDVDInfo->DVDInfoFrameLength - j);
	//					pForyouDVDInfo->DVDInfoFrameCRCErrorTailLength = pForyouDVDInfo->DVDInfoFrameLength - j;

	//					DBGE((TEXT("\r\nForyou Tail")));
	//					for (i = 0;i < pForyouDVDInfo->DVDInfoFrameCRCErrorTailLength;i++)
	//					{
	//						DBGE((TEXT(" %x"),pForyouDVDInfo->DVDInfoFrameCRCErrorTailBuff[i]));
	//					}
	//					break;
	//				}
	//			}
	//		}
	//		pForyouDVDInfo->DVDInfoFrameStatus = 0;
	//	}
	//	break;
	//default:
	//	pForyouDVDInfo->DVDInfoFrameStatus = 0;
	//	break;
	//}
}

void OnCommRecv(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo, BYTE *buf, UINT buflen)
{
	//for (UINT i = 0; i < buflen; i++)
	//{
	//	if (pForyouDVDInfo->DVDInfoFrameCRCErrorTailLength)
	//	{
	//		for (UINT j = 0;j < pForyouDVDInfo->DVDInfoFrameCRCErrorTailLength;j++)
	//		{
	//			OnDataRecv(pForyouDVDInfo,pForyouDVDInfo->DVDInfoFrameCRCErrorTailBuff[j]);
	//		}
	//		pForyouDVDInfo->DVDInfoFrameCRCErrorTailLength = 0;
	//	}
	//	OnDataRecv(pForyouDVDInfo,buf[i]);
	//}
	///* 取得控件指针 */
}

static DWORD WINAPI ThreadFlyDVDCommProc(LPVOID pContext)
{
	P_FLY_FORYOU_DVD_INFO pForyouDVDInfo = (P_FLY_FORYOU_DVD_INFO)pContext;
	//BOOL fReadState;
	//DWORD dwCommModemStatus;
	//DWORD dwLength;
	//COMSTAT ComStat;
	//DWORD dwErrorFlags;
	//BYTE *recvBuf = new BYTE[DATA_BUFF_COMM_LENGTH];

	//if (!pForyouDVDInfo->hDVDComm || pForyouDVDInfo->hDVDComm == INVALID_HANDLE_VALUE
	//	|| pForyouDVDInfo->bKillFlyDVDCommThread)
	//{
	//	DBGE((TEXT("\r\nFlyAudio ThreadFlyDVDCommProc exit with com error")));
	//	return 0;
	//}
	//else
	//{
	//	DBGD((TEXT("\r\nFlyAudio ThreadFlyDVDCommProc start")));

	//}

	//while (!pForyouDVDInfo->bKillFlyDVDCommThread)
	//{
		////等待串口的事件发生
		//WaitCommEvent (pForyouDVDInfo->hDVDComm, &dwCommModemStatus, 0);
		//if (dwCommModemStatus & EV_RXCHAR) 
		//{
		//	ClearCommError(pForyouDVDInfo->hDVDComm,&dwErrorFlags,&ComStat);
		//	//cbInQue返回在串行驱动程序输入队列中的字符数
		//	dwLength=ComStat.cbInQue;
		//	if(dwLength>0)
		//	{
		//		//从串口读取数据

		//		fReadState=ReadFile(pForyouDVDInfo->hDVDComm,recvBuf,DATA_BUFF_COMM_LENGTH,&dwLength,NULL);
		//		if(!fReadState)
		//		{
		//			//不能从串口读取数据
		//			DBGE((TEXT("\r\nFlyAudio ThreadFlyDVDCommProc cannot read data!")));
		//		}
		//		else
		//		{
		//			//把数据赋值给全局变量
		//			OnCommRecv(pForyouDVDInfo,recvBuf,dwLength);
		//		}
		//	}     

		//}

		//GetCommModemStatus (pForyouDVDInfo->hDVDComm, &dwCommModemStatus);
	//}
	//delete []recvBuf;
	//DBGD((TEXT("\r\nFlyAudio ThreadFlyDVDCommProc exit")));
	return 0;
}



void closeDVDComm(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
{
}

void openDVDComm(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
{
}



void returnDVDVersion(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{
	BYTE buff[18];
	buff[0] = 0x2F;
	memcpy(&buff[1],p,len);
	ReturnToUser(buff,len+1);
}

void returnDVDDevicePowerMode(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE bPower)
{
	BYTE buff[2];
	buff[0] = 0x01;
	buff[1] = bPower;
	ReturnToUser(buff,2);
}

void returnDVDDeviceWorkMode(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE bWork)
{
	BYTE buff[2];
	buff[0] = 0x02;
	buff[1] = bWork;
	ReturnToUser(buff,2);
}

void returnDVDDevicePlayDevice(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iDevice)
{
	BYTE buff[2];
	buff[0] = 0x20;
	buff[1] = iDevice;
	ReturnToUser(buff,2);
}

void returnDVDDeviceActionState(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iDevice,BYTE iState)
{
	BYTE buff[3];
	buff[0] = 0x21;
	buff[1] = iDevice;
	buff[2] = iState;
	ReturnToUser(buff,3);
}

void returnDVDDeviceContent(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iContent)
{
	BYTE buff[2];
	buff[0] = 0x22;
	buff[1] = iContent;
	ReturnToUser(buff,2);
}

void returnDVDDeviceMedia(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iDisc,BYTE iFile)
{
	BYTE buff[3];
	buff[0] = 0x23;
	buff[1] = iDisc;buff[2] = iFile;
	ReturnToUser(buff,3);
}

void returnDVDTotalTitleTrack(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT iTitle,UINT iTrack)
{
	BYTE buff[5];
	buff[0] = 0x24;
	if (iTitle == 0xFF || iTrack == 0xFFFF)
	{
		buff[1] = 0;buff[2] = 0;buff[3] = 0;buff[4] = 0;
	} 
	else
	{
		buff[1] = iTitle >> 8;buff[2] = iTitle;buff[3] = iTrack >> 8;buff[4] = iTrack;
	}
	ReturnToUser(buff,5);
}

void returnDVDCurrentTitleTrack(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT iTitle,UINT iTrack)
{
	BYTE buff[5];
	buff[0] = 0x25;
	if (iTitle == 0xFF)
	{
		iTitle = 0;
	}
	if (iTrack == 0xFFFF)
	{
		iTrack = 0;
	}

	buff[1] = iTitle >> 8;buff[2] = iTitle;buff[3] = iTrack >> 8;buff[4] = iTrack;

	if (!pForyouDVDInfo->bFlyaudioDVD
		&& 0 == iTitle && 0 == iTrack)
	{
		return;
	}
	ReturnToUser(buff,5);
}

void returnDVDTotalTime(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iHour,BYTE iMin,BYTE iSec)
{
	BYTE buff[4];
	buff[0] = 0x26;
	if (iHour == 86 && iMin == 45 && iSec == 19)
	{
		buff[1] = 0;buff[2] = 0;buff[3] = 0;
	} 
	else
	{
		buff[1] = iHour;buff[2] = iMin;buff[3] = iSec;
	}
	ReturnToUser(buff,4);
}

void returnDVDCurrentTime(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iHour,BYTE iMin,BYTE iSec)
{
	BYTE buff[4];
	buff[0] = 0x27;
	if (iHour == 86 && iMin == 45 && iSec == 19)
	{
		buff[1] = 0;buff[2] = 0;buff[3] = 0;
	} 
	else
	{
		buff[1] = iHour;buff[2] = iMin;buff[3] = iSec;
	}
	ReturnToUser(buff,4);
}

void returnDVDPlayStatusSpeed(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iPlayStatus,BYTE iPlaySpeed)
{
	BYTE buff[3];
	buff[0] = 0x28;
	DBGD((TEXT("\r\nFlyAudio returnDVDPlayStatusSpeed LDH:iPlayStatus = %d iPlaySpeed= %d")));
	buff[1] = iPlayStatus;buff[2] = iPlaySpeed;

	pForyouDVDInfo->sForyouDVDInfo.bDVDResponseState = iPlayStatus;
	procDVDStop(pForyouDVDInfo,TRUE);

	ReturnToUser(buff,3);
}

void returnDVDPlayMode(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iPlayMode)
{
	BYTE buff[2];
	buff[0] = 0x11;

	buff[1] = iPlayMode;
	
	ReturnToUser(buff,2);
}

void returnDVDUpdateStatus(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE update)
{
	BYTE buff[2];

	buff[0]=0xE7;
	buff[1]=update;
	ReturnToUser(buff,2);
}
void returnDVDCurrentFolderInfo(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT totalCount,UINT folderCount,BOOL bRoot)
{
	BYTE buff[1+2+2+1];
	buff[0] = 0x12;

	buff[1] = totalCount >> 8;
	buff[2] = totalCount;

	buff[3] = folderCount >> 8;
	buff[4] = folderCount;

	if (bRoot)
	{
		buff[5] = 1;
	} 
	else
	{
		buff[5] = 0;
	}

	ReturnToUser(buff,6);
}
void returnFlyAudioDVDInfo(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{
	if (len <= 0)
		return;

	ReturnToUser(p,len);
}
void returnDVDFileFolderInfo(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bFolder,BYTE *pName,UINT nameLength,UINT index,UINT globalIndex)
{
	BYTE buff[6+256];
	buff[0] = 0x13;

	buff[1] = index >> 8;buff[2] = index;
	buff[3] = globalIndex >> 8;buff[4] = globalIndex;
	if (bFolder)
	{
		buff[5] = 0;
	}
	else
	{
		buff[5] = 1;
	}
	memcpy(&buff[6],pName,nameLength);

	ReturnToUser(buff,nameLength+6);
}

void returnDVDNowPlayingFileInfo(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,P_FOLDER_FILE_TREE_LIST p,UINT index,UINT globalIndex)
{
	BYTE buff[6+256];
	buff[0] = 0x2A;

	buff[1] = index >> 8;buff[2] = index;
	buff[3] = globalIndex >> 8;buff[4] = globalIndex;
	buff[5] = p->extension;
	memcpy(&buff[6],p->name,p->nameLength);

	ReturnToUser(buff,p->nameLength+6);
}

void returnDVDNowPlayingInFolderName(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,P_FOLDER_FILE_TREE_LIST p)
{
	BYTE buff[1+256];
	buff[0] = 0x29;

	memcpy(&buff[1],p->name,p->nameLength);

	ReturnToUser(buff,p->nameLength+1);
}

void returnDVDErrorStatus(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iError)
{
	BYTE buff[2];
	buff[0] = 0x10;

	buff[1] = iError;

	ReturnToUser(buff,2);
}
static void returnDVDBtStatus(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bStatus)
{
	BYTE buff[2];
	buff[0] = 0x50;

	buff[1] = bStatus;

	ReturnToUser(buff,2);
}

void DVDCmdPrintf(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *buf,UINT len)
{
	if (len > 256)
	{
		DBGE((TEXT("DVDCmdPrintf len is to length!!!\r\n")));
		return;
	}

	//BYTE sendBuf[256];
	//BYTE checksum;
	UINT i;	
	//DWORD bufSendLength;

	//sendBuf[0] = 0xFF;
	//sendBuf[1] = 0x55;
	//sendBuf[2] = len;
	//checksum = len;
	//for(i = 0;i < len;i++)
	//{
	//	sendBuf[3+i] = buf[i];
	//	checksum += buf[i];
	//}
	//sendBuf[3+i] = 0xFF - checksum;

	DBGD((TEXT("\r\nForyouDVD Control Socket %d bytes To DVD:"), len));
	for (i = 0; i < len; i++)
	{
		DBGD((TEXT("%02X "),buf[i]));
	}

	SocketWrite(buf, len);
}

static void control_DVD_BackCar_Status(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bState)
{
	BYTE buf[]={0xFE,0x00};
	buf[1] = bState;
	DVDCmdPrintf(pForyouDVDInfo,buf,2);
}

void control_DVD_Video_Aspect_Radio(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE para)
{
	BYTE buff[] = {0x09,0x00};

	buff[1] = para;

	DVDCmdPrintf(pForyouDVDInfo,buff,2);
}

void control_DVD_Set_View_Mode(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE para)
{
	BYTE buff[] = {0x0A,0x00};

	buff[1] = para;

	DVDCmdPrintf(pForyouDVDInfo,buff,2);
}

void control_DVD_PlayBack_DisplayInfo_State_Request(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bOn)
{
	BYTE buff[] = {0x89,0x00};
	if (bOn)
	{
		buff[1] = 0x01;
	} 
	else
	{
		buff[1] = 0x00;
	}
	DVDCmdPrintf(pForyouDVDInfo,buff,2);
}

void control_DVD_IR_CMD(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE IRCMD)
{
	BYTE buff[] = {0x6B,0x00};
	buff[1] = IRCMD;
	DVDCmdPrintf(pForyouDVDInfo,buff,2);
}

void control_DVD_ReqMechanismState(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iDevice)
{
	BYTE buff[] = {0x83,0x00};
	buff[1] = iDevice;
	DVDCmdPrintf(pForyouDVDInfo,buff,2);
}

void control_DVD_ReqMediaState(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
{
	BYTE buff[] = {0x85};
	DVDCmdPrintf(pForyouDVDInfo,buff,1);
}

void control_DVD_ReqDVDSoftwareVersion(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
{
	BYTE buff[] = {0x9A};
	DVDCmdPrintf(pForyouDVDInfo,buff,1);
}

void control_DVD_ReqFileFolderCount(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
{
	BYTE buff[] = {0x9B};
	DVDCmdPrintf(pForyouDVDInfo,buff,1);
}

void control_DVD_ReqFileFolderDetailedInfo(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE bFile,UINT iAbs,UINT iOffset)
{
	BYTE buff[6];
	buff[0] = 0x9D;
	buff[1] = bFile;
	buff[2] = iAbs >> 8;buff[3] = iAbs;
	buff[4] = iOffset >> 8;buff[5] = iOffset;
	DVDCmdPrintf(pForyouDVDInfo,buff,6);
}

void control_DVD_PlayFileByAbsCount(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT parentIndex,UINT index)
{
	BYTE buff[6];
	buff[0] = 0x70;
	buff[1] = parentIndex >> 8;buff[2] = parentIndex;
	buff[3] = index >> 8;buff[4] = index;
	buff[5] = pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[index].extension;
	if (pForyouDVDInfo->bFlyaudioDVD)
	{
		DVDCmdPrintf(pForyouDVDInfo,buff,6);
	}
	else
	{
		DVDCmdPrintf(pForyouDVDInfo,buff,5);
	}
}

void control_DVD_ReqHighLightFileIndex(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
{
	BYTE buff[] = {0x96};
	DVDCmdPrintf(pForyouDVDInfo,buff,1);
}

void control_DVD_ReqRegionCode(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
{
	BYTE buff[] = {0x82,0x02};
	DVDCmdPrintf(pForyouDVDInfo,buff,2);
}

void control_DVD_ID3CDText(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iWhat,UINT iStart,UINT iCount)
{
	BYTE buff[] = {0x8C,0x51,0x00,0x01,0x00,0x01};

	buff[1] = iWhat;

	buff[2] = iStart >> 8;
	buff[3] = iStart;

	buff[4] = iCount >> 8;
	buff[5] = iCount;
	DVDCmdPrintf(pForyouDVDInfo,buff,6);
}

void control_DVD_ControlRegionCode(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iRegionCode)
{
	BYTE buff[] = {0x02,0x09};
	buff[1] = iRegionCode;
	DVDCmdPrintf(pForyouDVDInfo,buff,2);
}

void control_DVD_VideoSetup(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
{
	BYTE buff[] = {0x03,0x00,0x00,0x00,0x14,20,16,9,9};
	//cmd ,CVBS+PAL,16:9,full screen,standard,contrast,brightness,hue,saturation
	DVDCmdPrintf(pForyouDVDInfo,buff,9);
}

void control_DVD_JumpNextN(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT iJumpN)
{
	BYTE buff[] = {0x4F,0x00,0x00};
	buff[1] = iJumpN >> 8;
	buff[2] = iJumpN;
	DVDCmdPrintf(pForyouDVDInfo,buff,3);
}

void control_DVD_JumpPrevN(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT iJumpN)
{
	BYTE buff[] = {0x50,0x00,0x00};
	buff[1] = iJumpN >> 8;
	buff[2] = iJumpN;
	DVDCmdPrintf(pForyouDVDInfo,buff,3);
}

//+++++++++++++++++++++++++++++++++++++++++++start+++++++++++++++++++++++++++++++++++++++++++++++
//新加协议，透时通道
void control_FlyAudioInfo(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p, UINT16 len)
{
	if (len <= 0)
		return;

	DVDCmdPrintf(pForyouDVDInfo,p,len);
}

//+++++++++++++++++++++++++++++++++++++++++++++end+++++++++++++++++++++++++++++++++++++++++++++++++++

//void InitDVDVersion(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
//{
//
//	memcpy(pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion,DVD_VERSION,6);
//
//	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[0] = 'Y';
//	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[1]
//	= pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion[0];
//	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[2]
//	= pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion[1];
//
//	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[3] = 'M';
//	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[4]
//	= pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion[2];
//	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[5]
//	= pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion[3];
//
//	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[6] = 'D';
//	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[7]
//	= pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion[4];
//	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[8]
//	= pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion[5];
//
//	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersionLength = 9;
//	IpcStartEvent(EVENT_GLOBAL_RETURN_DVD_VERSION_ID);
//}

void structDVDInfoInit(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bInitAll)
{

	if (bInitAll)
	{
		pForyouDVDInfo->sForyouDVDInfo.MechanismInitialize = FALSE;
		pForyouDVDInfo->sForyouDVDInfo.bDeviceRec89 = 0;

		pForyouDVDInfo->sForyouDVDInfo.DeviceStatus = 0;
		pForyouDVDInfo->sForyouDVDInfo.DeviceType = 0;
		pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice = 0xFF;

		pForyouDVDInfo->sForyouDVDInfo.HaveDisc = 0;
		pForyouDVDInfo->sForyouDVDInfo.HaveUSB = 0;
		pForyouDVDInfo->sForyouDVDInfo.HaveSD = 0;

		pForyouDVDInfo->sForyouDVDInfo.bStartChangePlayDevice = FALSE;
	}
	DBGD((TEXT("\r\nForyouDVD para init %d"),bInitAll));

	pForyouDVDInfo->sForyouDVDInfo.DVDReqStep = 0;
	pForyouDVDInfo->sForyouDVDInfo.bForceExecReqStep = FALSE;

	pForyouDVDInfo->sForyouDVDInfo.bQuickJumpNext = FALSE;
	pForyouDVDInfo->sForyouDVDInfo.iQuickJumpNextCount = 0;
	pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos = 0;
	pForyouDVDInfo->sForyouDVDInfo.iQuickJumpNextTimer = 0;

	pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[0] = 0xFF;
	pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[1] = 0xFF;
	pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[2] = 0xFF;
	pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[3] = 0xFF;
	pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismType[0] = 0xFF;
	pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismType[1] = 0xFF;
	pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismType[2] = 0xFF;
	pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismType[3] = 0xFF;

	pForyouDVDInfo->sForyouDVDInfo.CurrentReqMechanismCircle = 0;

	pForyouDVDInfo->sForyouDVDInfo.MediaDiscType = 0;
	pForyouDVDInfo->sForyouDVDInfo.MediaFileType = 0;
	pForyouDVDInfo->sForyouDVDInfo.MediaVideoInfo = 0;
	pForyouDVDInfo->sForyouDVDInfo.MediaAudioSampleFrequency = 0;
	pForyouDVDInfo->sForyouDVDInfo.MediaAudioCoding = 0;

	pForyouDVDInfo->sForyouDVDInfo.preVideoAspect = 0xFF;
	pForyouDVDInfo->sForyouDVDInfo.curVideoAspect = 0xFF;

	pForyouDVDInfo->sForyouDVDInfo.CurrentTitle = -1;
	pForyouDVDInfo->sForyouDVDInfo.CurrentChar = -1;
	pForyouDVDInfo->sForyouDVDInfo.TotalTitle = 0;
	pForyouDVDInfo->sForyouDVDInfo.TotalChar = 0;
	pForyouDVDInfo->sForyouDVDInfo.EscapeHour = 0;
	pForyouDVDInfo->sForyouDVDInfo.EscapeMinute = 0;
	pForyouDVDInfo->sForyouDVDInfo.EscapeSecond = 0;
	pForyouDVDInfo->sForyouDVDInfo.TotalHour = 0;
	pForyouDVDInfo->sForyouDVDInfo.TotalMinute = 0;
	pForyouDVDInfo->sForyouDVDInfo.TotalSecond = 0;
	pForyouDVDInfo->sForyouDVDInfo.PlayMode = 0;
	pForyouDVDInfo->sForyouDVDInfo.AudioType = 0;
	pForyouDVDInfo->sForyouDVDInfo.PlaySpeed = 0;
	pForyouDVDInfo->sForyouDVDInfo.PlayStatus = 0;
	pForyouDVDInfo->sForyouDVDInfo.DVDRoot = 0;

	pForyouDVDInfo->sForyouDVDInfo.pBStartGetFolderFile = FALSE;
	pForyouDVDInfo->sForyouDVDInfo.pBHaveGetFolderFile = FALSE;
	pForyouDVDInfo->sForyouDVDInfo.pStartGetFolderFileCount = 0;
	pForyouDVDInfo->sForyouDVDInfo.pReturnE8 = FALSE;
	pForyouDVDInfo->sForyouDVDInfo.pReturnEAFolder = FALSE;
	pForyouDVDInfo->sForyouDVDInfo.pReturnEAFile = FALSE;
	pForyouDVDInfo->sForyouDVDInfo.pFolderCount = 0;
	pForyouDVDInfo->sForyouDVDInfo.pFileCount = 0;
	pForyouDVDInfo->sForyouDVDInfo.bFinishGetFileFolderEB = FALSE;
	pForyouDVDInfo->sForyouDVDInfo.bRecFileFolderUseEB = FALSE;
	pForyouDVDInfo->sForyouDVDInfo.pRecFolderCount = 0;
	pForyouDVDInfo->sForyouDVDInfo.pRecFileCount = 0;
	pForyouDVDInfo->sForyouDVDInfo.pLastRecFolderIndex = 0;
	pForyouDVDInfo->sForyouDVDInfo.pLastRecFileIndex = 0;
	pForyouDVDInfo->sForyouDVDInfo.pBReqFolderFileCommandActiveNow = FALSE;
	pForyouDVDInfo->sForyouDVDInfo.pBFolderFileListFolderErrorCheck = FALSE;
	pForyouDVDInfo->sForyouDVDInfo.pBFolderFileListFileErrorCheck = FALSE;
	pForyouDVDInfo->sForyouDVDInfo.pBGetFolderFinish = FALSE;
	pForyouDVDInfo->sForyouDVDInfo.pBGetFileFinish = FALSE;
	pForyouDVDInfo->sForyouDVDInfo.pBGetFileFolderFinish = FALSE;
	listFileFolderClearAll(pForyouDVDInfo,TRUE);
	listFileFolderClearAll(pForyouDVDInfo,FALSE);
	pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder = 0;
	pForyouDVDInfo->sForyouDVDInfo.pNeedReturnStart = 0;
	pForyouDVDInfo->sForyouDVDInfo.pNeedReturnCount = 0;
	pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount = 0;
	pForyouDVDInfo->sForyouDVDInfo.pNowReturnPlayingFileCount = -1;
	pForyouDVDInfo->sForyouDVDInfo.pNowPlayingInWhatFolder = -1;

	pForyouDVDInfo->sForyouDVDInfo.bRecE0AndNeedProc = FALSE;
	pForyouDVDInfo->sForyouDVDInfo.bNeedSend8CTime = 0;

	pForyouDVDInfo->sForyouDVDInfo.iDVDReturnRegionCode = 0xFF;//缺省

	memset(pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion,0,6);
	pForyouDVDInfo->sForyouDVDInfo.bDVDRequestState = 0;
	pForyouDVDInfo->sForyouDVDInfo.bDVDResponseState = 0;
	pForyouDVDInfo->sForyouDVDInfo.iDVDStateCheckTime = 0;

#if FORYOU_DVD_BUG_FIX
	pForyouDVDInfo->sForyouDVDInfo.bNeedReturnNoDiscAfterClose = FALSE;
	pForyouDVDInfo->sForyouDVDInfo.iNeedReturnNoDiscAfterCloseTime = 0;
#endif
	
}

void transDVDInfoCB(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{
	if (p[0] > 3)
	{
		return;
	}

	if (pForyouDVDInfo->sForyouDVDInfo.bFilterDiscInFirstPowerUp)//过滤DiscIn
	{
		if (0x00 == p[0])//过滤碟机
		{
			if (0x00 == p[1])//开机无碟
			{
				pForyouDVDInfo->sForyouDVDInfo.bFilterDiscInFirstPowerUp = FALSE;
				DBGE((TEXT("\r\nForyouDVD DVD DiscIn Filter End With Real NoDisc")));
			}
			else if (0x03 == p[1])//开始过滤
			{
				pForyouDVDInfo->sForyouDVDInfo.bFilterDiscIn = TRUE;
				DBGE((TEXT("\r\nForyouDVD DVD DiscIn Filter")));
				return;
			}
			else
			{
				if (pForyouDVDInfo->sForyouDVDInfo.bFilterDiscIn)//过滤完成
				{
					pForyouDVDInfo->sForyouDVDInfo.bFilterDiscInFirstPowerUp = FALSE;
					DBGE((TEXT("\r\nForyouDVD DVD DiscIn Filter End")));
				}
			}
		}
	}

	if (!pForyouDVDInfo->sForyouDVDInfo.MechanismInitialize
		|| (0x02 == p[1] || 0x03 == p[1])
		&& (GetTickCount() - pForyouDVDInfo->sForyouDVDInfo.iFilterDiscInTime < 2000))//上电一段时间内过滤入碟消息
	{
		return;
	}


#if FORYOU_DVD_BUG_FIX
	if (0x00 == p[0])
	{
		if (0x03 == p[1])
		{
			pForyouDVDInfo->sForyouDVDInfo.bNeedReturnNoDiscAfterClose = TRUE;
			pForyouDVDInfo->sForyouDVDInfo.iNeedReturnNoDiscAfterCloseTime = GetTickCount();
		}
	}
#endif

			//P[0]=0X00 P[1]=0X05, P[2]=0X09

	if (3 != p[0])
	{
		if (pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[p[0]] != p[1])
		{
			if (0x00 == p[0])//DISC LED Flash
			{
				if (0x02 == p[1] && pForyouDVDInfo->sForyouDVDInfo.bLEDFlashAble)
				{
					pForyouDVDInfo->sForyouDVDInfo.bLEDFlashAble = FALSE;
					DBGD((TEXT("\r\nForyouDVD LED Flash")));
					pForyouDVDInfo->sForyouDVDInfo.LEDFlash = TRUE;
					SetEvent(pForyouDVDInfo->hDispatchThreadForyouDVDLEDControlEvent);
				}
				else
				{
					pForyouDVDInfo->sForyouDVDInfo.bLEDFlashAble = TRUE;
				}
			}
			pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[p[0]] = p[1];
			returnDVDDeviceActionState(pForyouDVDInfo,p[0],pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[p[0]]);
		}
		if (pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismType[p[0]] != p[2])
		{
			pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismType[p[0]] = p[2];
			if (p[0] == pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice)
			{
				returnDVDDeviceContent(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismType[p[0]]);
			}
		}
	}

	pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[p[0]] = p[1];
	pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismType[p[0]] = p[2];

	if (0x01 == p[0] && 0x0A == p[1])//USB拔出。。。
	{
		control_DVD_IR_CMD(pForyouDVDInfo,0x95);
	}

	if(0x05 == pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[p[0]])//每次必有读碟动作
	{
		structDVDInfoInit(pForyouDVDInfo,FALSE);
	}
}

void transDVDInfoCD(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{
	if(pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[3] != 0x09)//启动获取流程
	{
		DBGD((TEXT("\r\nForyouDVD CD Filter For Mechanism Not Ready")));
		return;
	}

	if ((pForyouDVDInfo->sForyouDVDInfo.MediaDiscType != (p[0] >> 4))
		|| (pForyouDVDInfo->sForyouDVDInfo.MediaFileType != (p[0] & 0x0F)))
	{
		pForyouDVDInfo->sForyouDVDInfo.MediaDiscType = p[0] >> 4;
		pForyouDVDInfo->sForyouDVDInfo.MediaFileType = p[0] & 0x0F;
		returnDVDDeviceMedia(pForyouDVDInfo,
			pForyouDVDInfo->sForyouDVDInfo.MediaDiscType,
			pForyouDVDInfo->sForyouDVDInfo.MediaFileType);
	}
	pForyouDVDInfo->sForyouDVDInfo.MediaVideoInfo = p[1] >> 6;
	pForyouDVDInfo->sForyouDVDInfo.MediaAudioSampleFrequency = (p[1] >> 3) & 0x07;
	pForyouDVDInfo->sForyouDVDInfo.MediaAudioCoding = p[1] & 0x07;
}

void transDVDInfoD2(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{
	pForyouDVDInfo->sForyouDVDInfo.bDeviceRec89 = GetTickCount();

	pForyouDVDInfo->sForyouDVDInfo.HaveDisc = p[18];
	pForyouDVDInfo->sForyouDVDInfo.HaveUSB = p[19];
	pForyouDVDInfo->sForyouDVDInfo.HaveSD = p[20];

	if (pForyouDVDInfo->sForyouDVDInfo.PlayMode != p[12])
	{
		pForyouDVDInfo->sForyouDVDInfo.PlayMode = p[12];
		returnDVDPlayMode(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.PlayMode);
	}

	pForyouDVDInfo->sForyouDVDInfo.AudioType = p[13];

	if (((pForyouDVDInfo->sForyouDVDInfo.PlaySpeed << 4) + pForyouDVDInfo->sForyouDVDInfo.PlayStatus) != p[14])
	{
		pForyouDVDInfo->sForyouDVDInfo.PlaySpeed = p[14] >> 4;
		pForyouDVDInfo->sForyouDVDInfo.PlayStatus = p[14] & 0x0F;
		returnDVDPlayStatusSpeed(pForyouDVDInfo,
			pForyouDVDInfo->sForyouDVDInfo.PlayStatus,
			pForyouDVDInfo->sForyouDVDInfo.PlaySpeed);
	}

	pForyouDVDInfo->sForyouDVDInfo.DVDRoot = p[15];

	pForyouDVDInfo->sForyouDVDInfo.DeviceStatus = p[16];
	pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismType[3] = p[17];
	if (pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[3] != pForyouDVDInfo->sForyouDVDInfo.DeviceStatus
		|| pForyouDVDInfo->sForyouDVDInfo.DeviceType != p[17])
	{
		pForyouDVDInfo->sForyouDVDInfo.DeviceType = p[17];
		pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[3] = pForyouDVDInfo->sForyouDVDInfo.DeviceStatus;

		if (0x00 == pForyouDVDInfo->sForyouDVDInfo.HaveDisc)
		{
			if (0x08 == pForyouDVDInfo->sForyouDVDInfo.DeviceType)
			{
				if (0xFF == pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[1])
				{
					returnDVDDeviceActionState(pForyouDVDInfo,0x01,pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[3]);
				}
			}
			else
			{
				if (0xFF == pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[0])
				{
					returnDVDDeviceActionState(pForyouDVDInfo,0x00,pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[3]);
				}
			}
		}
	}

	if (0x09 == pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[3])
	{
		if (pForyouDVDInfo->sForyouDVDInfo.CurrentChar != (p[1] * 256 + p[2])
			|| pForyouDVDInfo->sForyouDVDInfo.CurrentTitle != p[0])
		{
			pForyouDVDInfo->sForyouDVDInfo.CurrentTitle = p[0];
			pForyouDVDInfo->sForyouDVDInfo.CurrentChar = p[1] * 256 + p[2];
			returnDVDCurrentTitleTrack(pForyouDVDInfo,
				pForyouDVDInfo->sForyouDVDInfo.CurrentTitle,
				pForyouDVDInfo->sForyouDVDInfo.CurrentChar);
		}

		if (pForyouDVDInfo->sForyouDVDInfo.TotalChar != (p[4] * 256 + p[5])
			|| pForyouDVDInfo->sForyouDVDInfo.TotalTitle != p[3])
		{
			pForyouDVDInfo->sForyouDVDInfo.TotalTitle = p[3];
			pForyouDVDInfo->sForyouDVDInfo.TotalChar = p[4] * 256 + p[5];
			returnDVDTotalTitleTrack(pForyouDVDInfo,
				pForyouDVDInfo->sForyouDVDInfo.TotalTitle,
				pForyouDVDInfo->sForyouDVDInfo.TotalChar);
		}

		if (pForyouDVDInfo->sForyouDVDInfo.EscapeSecond != p[8]
		|| pForyouDVDInfo->sForyouDVDInfo.EscapeMinute != p[7]
		|| pForyouDVDInfo->sForyouDVDInfo.EscapeHour != p[6])
		{
			pForyouDVDInfo->sForyouDVDInfo.EscapeHour = p[6];
			pForyouDVDInfo->sForyouDVDInfo.EscapeMinute = p[7];
			pForyouDVDInfo->sForyouDVDInfo.EscapeSecond = p[8];
			returnDVDCurrentTime(pForyouDVDInfo,
				pForyouDVDInfo->sForyouDVDInfo.EscapeHour,
				pForyouDVDInfo->sForyouDVDInfo.EscapeMinute,
				pForyouDVDInfo->sForyouDVDInfo.EscapeSecond);
		}

		if (pForyouDVDInfo->sForyouDVDInfo.TotalSecond != p[11]
		|| pForyouDVDInfo->sForyouDVDInfo.TotalMinute != p[10]
		|| pForyouDVDInfo->sForyouDVDInfo.TotalHour != p[9])
		{
			pForyouDVDInfo->sForyouDVDInfo.TotalHour = p[9];
			pForyouDVDInfo->sForyouDVDInfo.TotalMinute = p[10];
			pForyouDVDInfo->sForyouDVDInfo.TotalSecond = p[11];
			returnDVDTotalTime(pForyouDVDInfo,
				pForyouDVDInfo->sForyouDVDInfo.TotalHour,
				pForyouDVDInfo->sForyouDVDInfo.TotalMinute,
				pForyouDVDInfo->sForyouDVDInfo.TotalSecond);
		}
	}

	if (0x09 == pForyouDVDInfo->sForyouDVDInfo.DeviceType)//Disc
	{
		if (pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice != 0)
		{
			structDVDInfoInit(pForyouDVDInfo,FALSE);
			pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice = 0;//Disc
			pForyouDVDInfo->sForyouDVDInfo.HaveDisc = p[18];
			pForyouDVDInfo->sForyouDVDInfo.HaveUSB = p[19];
			pForyouDVDInfo->sForyouDVDInfo.HaveSD = p[20];
			//returnDVDDevicePlayDevice(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice);
		}
	}
	else if (0x08 == pForyouDVDInfo->sForyouDVDInfo.DeviceType)
	{
		if (pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice != 1)
		{
			structDVDInfoInit(pForyouDVDInfo,FALSE);
			pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice = 1;//USB
			pForyouDVDInfo->sForyouDVDInfo.HaveDisc = p[18];
			pForyouDVDInfo->sForyouDVDInfo.HaveUSB = p[19];
			pForyouDVDInfo->sForyouDVDInfo.HaveSD = p[20];
			//returnDVDDevicePlayDevice(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice);
		}
	}
	else
	{
		RETAILMSG(1, (TEXT("\r\nForyouDVD Play Unknower Device %d"),pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice));
		//if (1 == pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice)
		//{
		//	structDVDInfoInit(pForyouDVDInfo,FALSE);
		//	control_DVD_IR_CMD(pForyouDVDInfo,0x95);//切换到DVD
		//}
	}

	if(pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[3] == 0x09)//启动获取流程
	{
		if(pForyouDVDInfo->sForyouDVDInfo.MediaDiscType == 0x02)//Clips
		{
			if(pForyouDVDInfo->sForyouDVDInfo.pBStartGetFolderFile == FALSE)
			{
				if (!pForyouDVDInfo->sForyouDVDInfo.pBHaveGetFolderFile)
				{
					pForyouDVDInfo->sForyouDVDInfo.pBHaveGetFolderFile = TRUE;
					pForyouDVDInfo->sForyouDVDInfo.pBStartGetFolderFile = TRUE;
					pForyouDVDInfo->sForyouDVDInfo.pStartGetFolderFileCount = 0;
					pForyouDVDInfo->sForyouDVDInfo.pBReqFolderFileCommandActiveNow = TRUE;

					pForyouDVDInfo->sForyouDVDInfo.pBGetFolderFinish = FALSE;
					pForyouDVDInfo->sForyouDVDInfo.pBGetFileFinish = FALSE;
					DBGD((TEXT("\r\nForyouDVD Start get folder and file list")));
				}
				else
				{
					DBGD((TEXT(" already get")));
				}
			}
			else
			{
				DBGD((TEXT(" already start")));
			}
		}
		else
		{
			DBGD((TEXT(" not clips")));
		}
	}
	else
	{
		DBGD((TEXT(" not ready")));
	}
}

void transDVDInfoD5(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{
	UINT i;
	UINT iCount;
	if (0x51 == p[0])
	{
		iCount = p[1] * 256 + p[2] - 1;
		if (pForyouDVDInfo->sForyouDVDInfo.pFileCount)
		{
			if (iCount < pForyouDVDInfo->sForyouDVDInfo.pFileCount)
			{
				for (i = 3;i < len - 1;i+=2)
				{
					if (0 == p[i] && 0 == p[i+1])
					{
						break;
					}
					else
					{
						pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name[i-3] = p[i+1];
						pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name[i-2] = p[i];
						pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name[i-1] = 0;
						pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].name[i-0] = 0;
						pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[iCount].nameLength = i - 1 + 2;
					}
				}
				if (iCount == pForyouDVDInfo->sForyouDVDInfo.pNowReturnPlayingFileCount)
				{
					returnDVDNowPlayingFileInfo(pForyouDVDInfo,
						&pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[pForyouDVDInfo->sForyouDVDInfo.pNowReturnPlayingFileCount],
						getSelectFolderFileIndexByGlobalIndexInParent(pForyouDVDInfo,FALSE,pForyouDVDInfo->sForyouDVDInfo.pNowReturnPlayingFileCount)
						,pForyouDVDInfo->sForyouDVDInfo.pNowReturnPlayingFileCount);
				}
			}
		}
	}
}

void transDVDInfoE8(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{
	pForyouDVDInfo->sForyouDVDInfo.pReturnE8 = TRUE;
	pForyouDVDInfo->sForyouDVDInfo.pFileCount = p[2]*256 + p[3];
	pForyouDVDInfo->sForyouDVDInfo.pFolderCount = p[0]*256 + p[1];
	DBGD((TEXT("\r\nDisc List Count File:%d Folder:%d")
		,pForyouDVDInfo->sForyouDVDInfo.pFileCount
		,pForyouDVDInfo->sForyouDVDInfo.pFolderCount));

	listFileFolderNewAll(pForyouDVDInfo,TRUE,pForyouDVDInfo->sForyouDVDInfo.pFileCount);
	listFileFolderNewAll(pForyouDVDInfo,FALSE,pForyouDVDInfo->sForyouDVDInfo.pFolderCount);

	pForyouDVDInfo->sForyouDVDInfo.pBReqFolderFileCommandActiveNow = TRUE;

	SetEvent(pForyouDVDInfo->hDispatchThreadForyouDVDEvent);
}

void transDVDInfoEA(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{
	DBGD((TEXT("File:%d,C:%d,P:%d,L:%d"),p[4],p[0]*256 + p[1],p[2]*256 + p[3],len-6));
	if (p[4])
	{
		pForyouDVDInfo->sForyouDVDInfo.pReturnEAFile = TRUE;
		pForyouDVDInfo->sForyouDVDInfo.pLastRecFileIndex = p[0]*256 + p[1];

		listFileFolderStorageOne(pForyouDVDInfo,TRUE,p[0]*256 + p[1],p[2]*256 + p[3],p[5],&p[6],len-6);

		if (pForyouDVDInfo->sForyouDVDInfo.pLastRecFileIndex == pForyouDVDInfo->sForyouDVDInfo.pFileCount - 1)
		{
			pForyouDVDInfo->sForyouDVDInfo.pBReqFolderFileCommandActiveNow = TRUE;
		}
	}
	else
	{
		pForyouDVDInfo->sForyouDVDInfo.pReturnEAFolder = TRUE;
		pForyouDVDInfo->sForyouDVDInfo.pLastRecFolderIndex = p[0]*256 + p[1];

		listFileFolderStorageOne(pForyouDVDInfo,FALSE,p[0]*256 + p[1],p[2]*256 + p[3],p[5],&p[6],len-6);

		if (pForyouDVDInfo->sForyouDVDInfo.pLastRecFolderIndex == pForyouDVDInfo->sForyouDVDInfo.pFolderCount - 1)
		{
			pForyouDVDInfo->sForyouDVDInfo.pBReqFolderFileCommandActiveNow = TRUE;
		}
	}
	if (p[4])
	{
		if (p[0]*256 + p[1] == pForyouDVDInfo->sForyouDVDInfo.pFileCount - 1)
		{
			pForyouDVDInfo->sForyouDVDInfo.bForceExecReqStep = TRUE;
			SetEvent(pForyouDVDInfo->hDispatchThreadForyouDVDEvent);
		}
	}
	else
	{
		if (p[0]*256 + p[1] == pForyouDVDInfo->sForyouDVDInfo.pFolderCount - 1)
		{
			pForyouDVDInfo->sForyouDVDInfo.bForceExecReqStep = TRUE;
			SetEvent(pForyouDVDInfo->hDispatchThreadForyouDVDEvent);
		}
	}
}

void procDVDInfoE0(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
{
	pForyouDVDInfo->sForyouDVDInfo.bRecE0AndNeedProc = FALSE;
	pForyouDVDInfo->sForyouDVDInfo.pNowPlayingInWhatFolder = pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[pForyouDVDInfo->sForyouDVDInfo.pNowReturnPlayingFileCount].parentFolderIndex;
	returnDVDNowPlayingFileInfo(pForyouDVDInfo,
		&pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[pForyouDVDInfo->sForyouDVDInfo.pNowReturnPlayingFileCount],
		getSelectFolderFileIndexByGlobalIndexInParent(pForyouDVDInfo,FALSE,pForyouDVDInfo->sForyouDVDInfo.pNowReturnPlayingFileCount)
		,pForyouDVDInfo->sForyouDVDInfo.pNowReturnPlayingFileCount);
	returnDVDNowPlayingInFolderName(pForyouDVDInfo
		,&pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[pForyouDVDInfo->sForyouDVDInfo.pNowReturnPlayingFileCount].parentFolderIndex]);
}

void DealDVDInfo(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT length)
{
	pForyouDVDInfo->iAutoResetControlTime = GetTickCount();


	if((pForyouDVDInfo->bEnterUpdateMode == TRUE)&&((p[0] == 0xdb)||(p[0] ==0xcb)))
	{
		pForyouDVDInfo->bEnterUpdateMode = FALSE;
		returnDVDUpdateStatus(pForyouDVDInfo,0x0);
	}

	switch (p[0])
	{
	case 0x2B:
		returnDVDDeviceActionState(pForyouDVDInfo,0,0x03);
		break;

	case 0xDB:
		if (2 == length && 0x00 == p[1])
		{
			pForyouDVDInfo->bFlyaudioDVD = TRUE;
		}
		else
		{
			pForyouDVDInfo->bFlyaudioDVD = FALSE;
		}
		structDVDInfoInit(pForyouDVDInfo,TRUE);
		pForyouDVDInfo->sForyouDVDInfo.MechanismInitialize = TRUE;

		//control_DVD_IR_CMD(pForyouDVDInfo,0x17);
		//control_DVD_VideoSetup(pForyouDVDInfo);

		control_DVD_PlayBack_DisplayInfo_State_Request(pForyouDVDInfo,TRUE);
		returnDVDDeviceWorkMode(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.MechanismInitialize);
		
		pForyouDVDInfo->sForyouDVDInfo.bFilterDiscInFirstPowerUp = FALSE;//新的碟机程序解决了？
		pForyouDVDInfo->sForyouDVDInfo.bFilterDiscIn = FALSE;
		pForyouDVDInfo->sForyouDVDInfo.iFilterDiscInTime = GetTickCount();
#if FORYOU_DVD_USB_SUPPORT
		pForyouDVDInfo->sForyouDVDInfo.bStartChangePlayDevice = TRUE;
		pForyouDVDInfo->sForyouDVDInfo.iStartChangePlayDeviceTime = GetTickCount();
		returnDVDDevicePlayDevice(pForyouDVDInfo,pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDWantPlayDevice);
#endif
		break;
	case 0xC1:
		//DBGD((TEXT(" %x"),p[1]));
		if (0x06 == p[1] || 0x07 == p[1] || 0x08 == p[1] || 0x09 == p[1] || 0x0A == p[1])
		{
			returnDVDErrorStatus(pForyouDVDInfo,p[1]);
		}
		break;
	case 0xC2:
		pForyouDVDInfo->sForyouDVDInfo.iDVDReturnRegionCode = p[1];
		if (pForyouDVDInfo->sForyouDVDInfo.iDVDReturnRegionCode
			!= pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode)//设置并重新查询
		{
			control_DVD_ControlRegionCode(pForyouDVDInfo,pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode);
			pForyouDVDInfo->sForyouDVDInfo.iDVDReturnRegionCode = 0xFF;//重新查询
		}
		break;
	case 0xCC:
		if (((pForyouDVDInfo->sForyouDVDInfo.PlaySpeed << 4) + pForyouDVDInfo->sForyouDVDInfo.PlayStatus) != p[1])
		{
			pForyouDVDInfo->sForyouDVDInfo.PlaySpeed = p[1] >> 4;
			pForyouDVDInfo->sForyouDVDInfo.PlayStatus = p[1] & 0x0F;
			returnDVDPlayStatusSpeed(pForyouDVDInfo,
				pForyouDVDInfo->sForyouDVDInfo.PlayStatus,
				pForyouDVDInfo->sForyouDVDInfo.PlaySpeed);
		}
		break;
	case 0xCB:
		transDVDInfoCB(pForyouDVDInfo,&pForyouDVDInfo->DVDInfoFrameBuff[1],length-1);
		break;
	case 0xCD:
		transDVDInfoCD(pForyouDVDInfo,&pForyouDVDInfo->DVDInfoFrameBuff[1],length-1);
		break;
	case 0xD2:
		transDVDInfoD2(pForyouDVDInfo,&pForyouDVDInfo->DVDInfoFrameBuff[1],length-1);
		break;
	case 0xD5:
		transDVDInfoD5(pForyouDVDInfo,&pForyouDVDInfo->DVDInfoFrameBuff[1],length-1);
		break;
	case 0xE8:
		transDVDInfoE8(pForyouDVDInfo,&pForyouDVDInfo->DVDInfoFrameBuff[1],length-1);
		break;
	case 0xEA:
		transDVDInfoEA(pForyouDVDInfo,&pForyouDVDInfo->DVDInfoFrameBuff[1],length-1);
		break;
	case 0xE0:
		DBGD((TEXT("E0 %x %x %x"),p[1],p[2],p[3]));
		if (pForyouDVDInfo->sForyouDVDInfo.pNowReturnPlayingFileCount != (p[1]*256 + p[2]))
		{
			pForyouDVDInfo->sForyouDVDInfo.pNowReturnPlayingFileCount = p[1]*256 + p[2];
			if (pForyouDVDInfo->sForyouDVDInfo.pBGetFileFolderFinish)
			{
				procDVDInfoE0(pForyouDVDInfo);
			}
			else
			{
				pForyouDVDInfo->sForyouDVDInfo.bRecE0AndNeedProc = TRUE;
				DBGD((TEXT("\r\nForyouDVD Rec E0 But Haven't Rec File Or Folder Info")));
			}
		}
		break;
	case 0xE5:
		if (memcmp(pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion,&p[1],6))
		{
			memcpy(pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion,&p[1],6);

			pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[0] = 'Y';
			pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[1]
				= (pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion[0] >> 4)/10 + '0';
			pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[2]
				= (pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion[0] >> 4)%10 + '0';

			pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[3] = 'M';
			pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[4]
				= (pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion[0] & 0x0F)/10 + '0';
			pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[5]
				= (pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion[0] & 0x0F)%10 + '0';

			pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[6] = 'D';
			pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[7]
				= pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion[1]/10 + '0';
			pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[8]
				= pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion[1]%10 + '0';

			pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersionLength = 9;
			IpcStartEvent(EVENT_GLOBAL_RETURN_DVD_VERSION_ID);
		}
		break;
	case 0xE7:
		pForyouDVDInfo->bEnterUpdateMode = TRUE;
		returnDVDUpdateStatus(pForyouDVDInfo,0x01);
		break;

	case 0xFF:
		if (p[1] == 0x01)
		{
			pForyouDVDInfo->bDVDSendInitStatusStart = TRUE;
			//IpcStartEvent(EVENT_GLOBAL_STANDBY_DVD_ID);
		}
		break;

	default:
		DBGD((TEXT("unHandle!")));
		break;
	}
}

UINT selectFolderOrFileByLocalIndex(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT inWhatFolder,UINT localIndex,BOOL bHaveFolderIndex)
{
	UINT isFolderCount = getSelectParentFolderFileCount(pForyouDVDInfo,inWhatFolder,TRUE);
	UINT isFileCount = getSelectParentFolderFileCount(pForyouDVDInfo,inWhatFolder,FALSE);
	DBGD((TEXT("\r\nForyou List Sel In Folder%d FolderCount%d FileCount%d"),inWhatFolder,isFolderCount,isFileCount));
	if (0 == inWhatFolder)//当前在根目录
	{
		if (!bHaveFolderIndex)
		{
			localIndex = localIndex + isFolderCount;
		}
		DBGD((TEXT(" root folder%d file%d sel%d"),isFolderCount,isFileCount,localIndex));
		if (localIndex < isFolderCount)//选择的是文件夹
		{
			inWhatFolder = 
				getSelectParentFolderFileInfoBySubIndex(pForyouDVDInfo,inWhatFolder,TRUE,localIndex);
			isFolderCount = getSelectParentFolderFileCount(pForyouDVDInfo,inWhatFolder,TRUE);
			isFileCount = getSelectParentFolderFileCount(pForyouDVDInfo,inWhatFolder,FALSE);
			returnDVDCurrentFolderInfo(pForyouDVDInfo,isFolderCount+1+isFileCount,isFolderCount+1,FALSE);
		}
		else if (localIndex < (isFolderCount + isFileCount))//选择的是文件
		{
			control_DVD_PlayFileByAbsCount(pForyouDVDInfo,
				pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[getSelectParentFolderFileInfoBySubIndex(pForyouDVDInfo,inWhatFolder,FALSE,localIndex - isFolderCount)].parentFolderIndex,
				getSelectParentFolderFileInfoBySubIndex(pForyouDVDInfo,inWhatFolder,FALSE,localIndex - isFolderCount));
		}
	}
	else//当前不在根目录
	{
		if (!bHaveFolderIndex)
		{
			localIndex = localIndex + isFolderCount + 1;
		}
		DBGD((TEXT(" not root folder%d file%d sel%d"),isFolderCount,isFileCount,localIndex));
		if (0 == localIndex)//选择上一级目录
		{
			inWhatFolder = 
				pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[inWhatFolder].parentFolderIndex;
			isFolderCount = getSelectParentFolderFileCount(pForyouDVDInfo,inWhatFolder,TRUE);
			isFileCount = getSelectParentFolderFileCount(pForyouDVDInfo,inWhatFolder,FALSE);
			if (0 == inWhatFolder)//当前在根目录
			{
				returnDVDCurrentFolderInfo(pForyouDVDInfo,isFolderCount+isFileCount,isFolderCount,TRUE);
			}
			else
			{
				returnDVDCurrentFolderInfo(pForyouDVDInfo,isFolderCount+1+isFileCount,isFolderCount+1,FALSE);
			}
		}
		else
		{
			if ((localIndex - 1) < isFolderCount)//选择的是文件夹
			{
				inWhatFolder = 
					getSelectParentFolderFileInfoBySubIndex(pForyouDVDInfo,inWhatFolder,TRUE,localIndex - 1);
				isFolderCount = getSelectParentFolderFileCount(pForyouDVDInfo,inWhatFolder,TRUE);
				isFileCount = getSelectParentFolderFileCount(pForyouDVDInfo,inWhatFolder,FALSE);
				returnDVDCurrentFolderInfo(pForyouDVDInfo,isFolderCount+1+isFileCount,isFolderCount+1,FALSE);
			}
			else if ((localIndex - 1) < (isFolderCount + isFileCount))//选择的是文件
			{
				control_DVD_PlayFileByAbsCount(pForyouDVDInfo,
					pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[getSelectParentFolderFileInfoBySubIndex(pForyouDVDInfo,inWhatFolder,FALSE,localIndex - 1 - isFolderCount)].parentFolderIndex,
					getSelectParentFolderFileInfoBySubIndex(pForyouDVDInfo,inWhatFolder,FALSE,localIndex - 1 - isFolderCount));
			}
		}
	}
	return inWhatFolder;
}

static VOID WinCECommandProcessor(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *buf,UINT len)
{
	//DBGD((TEXT(" WinCECommandProcessor OK!")));

	switch(buf[0])
	{
	case 0x01:
		if (0x01 == buf[1])
		{
			pForyouDVDInfo->bPower = TRUE;
			returnDVDDevicePowerMode(pForyouDVDInfo,TRUE);
			IpcStartEvent(EVENT_GLOBAL_RETURN_DVD_VERSION_ID);

			//if (pForyouDVDInfo->bDVDSendInitStatusStart)
			//	IpcStartEvent(EVENT_GLOBAL_STANDBY_DVD_ID);
		}
		else if (0x00 == buf[1])
		{
			pForyouDVDInfo->bPower = FALSE;
			returnDVDDevicePowerMode(pForyouDVDInfo,FALSE);
		}
		break;
	case 0x10:
		pForyouDVDInfo->sForyouDVDInfo.bDVDRequestState = buf[1];
		pForyouDVDInfo->sForyouDVDInfo.iDVDStateCheckTime = GetTickCount();
		control_DVD_IR_CMD(pForyouDVDInfo,buf[1]);

		//if (0x18 == buf[1])//Next
		//{
		//	if (0x02 == pForyouDVDInfo->sForyouDVDInfo.MediaDiscType
		//		&& pForyouDVDInfo->sForyouDVDInfo.PlayMode != 0x01
		//		&& pForyouDVDInfo->sForyouDVDInfo.PlayMode != 0x30)//Clips碟片直接选取
		//	{
		//		if (pForyouDVDInfo->sForyouDVDInfo.pBGetFileFolderFinish)
		//		{
		//			pForyouDVDInfo->sForyouDVDInfo.bQuickJumpNext = TRUE;
		//			pForyouDVDInfo->sForyouDVDInfo.iQuickJumpNextCount++;
		//			if (0 == pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos)
		//			{
		//				pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos = pForyouDVDInfo->sForyouDVDInfo.CurrentChar + 1;
		//			}
		//			else
		//			{
		//				pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos++;
		//			}
		//			if (pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos
		//				>= pForyouDVDInfo->sForyouDVDInfo.TotalChar)
		//			{
		//				pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos = pForyouDVDInfo->sForyouDVDInfo.TotalChar;
		//			}
		//			returnDVDCurrentTitleTrack(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.CurrentTitle
		//				,pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos);
		//		}
		//	}
		//	else
		//	{
		//		control_DVD_IR_CMD(pForyouDVDInfo,0x18);
		//	}
		//	pForyouDVDInfo->sForyouDVDInfo.iQuickJumpNextTimer = GetTickCount();
		//	RETAILMSG(1, (TEXT("\r\n>TT:%dTI:%dCT:%dCI:%d")
		//		,pForyouDVDInfo->sForyouDVDInfo.TotalTitle
		//		,pForyouDVDInfo->sForyouDVDInfo.TotalChar
		//		,pForyouDVDInfo->sForyouDVDInfo.CurrentTitle
		//		,pForyouDVDInfo->sForyouDVDInfo.CurrentChar));
		//}
		//else if (0x19 == buf[1])//Prev
		//{
		//	if (0x02 == pForyouDVDInfo->sForyouDVDInfo.MediaDiscType
		//		&& pForyouDVDInfo->sForyouDVDInfo.PlayMode != 0x01
		//		&& pForyouDVDInfo->sForyouDVDInfo.PlayMode != 0x30)//Clips碟片直接选取
		//	{
		//		if (pForyouDVDInfo->sForyouDVDInfo.pBGetFileFolderFinish)
		//		{
		//			pForyouDVDInfo->sForyouDVDInfo.bQuickJumpNext = FALSE;
		//			pForyouDVDInfo->sForyouDVDInfo.iQuickJumpNextCount++;
		//			if (0 == pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos)
		//			{
		//				pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos = pForyouDVDInfo->sForyouDVDInfo.CurrentChar - 1;
		//			}
		//			else
		//			{
		//				pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos--;
		//			}				
		//			if (0 == pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos)
		//			{
		//				pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos = 1;
		//			}
		//			returnDVDCurrentTitleTrack(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.CurrentTitle
		//				,pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos);
		//		}
		//	}
		//	else
		//	{
		//		control_DVD_IR_CMD(pForyouDVDInfo,0x19);
		//	}
		//	pForyouDVDInfo->sForyouDVDInfo.iQuickJumpNextTimer = GetTickCount();
		//	RETAILMSG(1, (TEXT("\r\n<TT:%dTI:%dCT:%dCI:%d")
		//		,pForyouDVDInfo->sForyouDVDInfo.TotalTitle
		//		,pForyouDVDInfo->sForyouDVDInfo.TotalChar
		//		,pForyouDVDInfo->sForyouDVDInfo.CurrentTitle
		//		,pForyouDVDInfo->sForyouDVDInfo.CurrentChar));
		//}
		//else
		//{
		//	control_DVD_IR_CMD(pForyouDVDInfo,buf[1]);
		//}
		break;
	//case 0x11:
	//	if (0x00 == buf[1])
	//	{
	//		control_DVD_IR_CMD(pForyouDVDInfo,0x62);
	//	} 
	//	else if (0x01 == buf[1])
	//	{
	//		control_DVD_IR_CMD(pForyouDVDInfo,0x64);
	//	}
	//	else if (0x02 == buf[1])
	//	{
	//		control_DVD_IR_CMD(pForyouDVDInfo,0x65);
	//	}
	//	else if (0x03 == buf[1])
	//	{
	//		control_DVD_IR_CMD(pForyouDVDInfo,0x63);
	//	}
	//	break;
	//case 0x12://为了不引起混乱，只有接口处把根目录计算在数量内
	//	buf[0] = 0x2C;
	//	control_FlyAudioInfo(pForyouDVDInfo,buf,len);
	//	/*
	//	pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder
	//		= selectFolderOrFileByLocalIndex(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder,(UINT)(buf[1]*256 + buf[2]),TRUE);
	//	*/
	//	break;
	//case 0x13://为了不引起混乱，只有接口处把根目录计算在数量内
	//	buf[0] = 0x2D;
	//	control_FlyAudioInfo(pForyouDVDInfo,buf,len);
	//	//if (0 == pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder)//当前在根目录
	//	//{
	//	//	pForyouDVDInfo->sForyouDVDInfo.pNeedReturnStart = (UINT)(buf[2]*256 + buf[3]);
	//	//	pForyouDVDInfo->sForyouDVDInfo.pNeedReturnCount = buf[1];
	//	//}
	//	//else
	//	//{
	//	//	if (0 == (UINT)(buf[2]*256 + buf[3]))//从上一级目录开始
	//	//	{
	//	//		if (buf[1])
	//	//		{
	//	//			BYTE nameFolder[] = {0,0};
	//	//			returnDVDFileFolderInfo(pForyouDVDInfo,TRUE,nameFolder,2,0,0);//上一级目录
	//	//			pForyouDVDInfo->sForyouDVDInfo.pNeedReturnStart = (UINT)(buf[2]*256 + buf[3]);
	//	//			pForyouDVDInfo->sForyouDVDInfo.pNeedReturnCount = buf[1] - 1;
	//	//		}
	//	//		else
	//	//		{
	//	//			pForyouDVDInfo->sForyouDVDInfo.pNeedReturnStart = (UINT)(buf[2]*256 + buf[3]);
	//	//			pForyouDVDInfo->sForyouDVDInfo.pNeedReturnCount = 0;
	//	//		}
	//	//	}
	//	//	else
	//	//	{
	//	//		pForyouDVDInfo->sForyouDVDInfo.pNeedReturnStart = (UINT)(buf[2]*256 + buf[3] - 1);
	//	//		pForyouDVDInfo->sForyouDVDInfo.pNeedReturnCount = buf[1];
	//	//	}
	//	//}
	//	//pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount = 0;
	//	//SetEvent(pForyouDVDInfo->hDispatchThreadForyouDVDEvent);
	//	break;
	case 0x15:
		if (0x00 == buf[1] || 0x01 == buf[1] || 0x02 == buf[1])
		{
			pForyouDVDInfo->sForyouDVDInfo.preVideoAspect = buf[1];
			//SetEvent(pForyouDVDInfo->hDispatchThreadForyouDVDEvent);

			//视频比例切换
			pForyouDVDInfo->sForyouDVDInfo.curVideoAspect = pForyouDVDInfo->sForyouDVDInfo.preVideoAspect;

			//control_DVD_IR_CMD(pForyouDVDInfo,0x17);
			//Sleep(100);
			control_DVD_Video_Aspect_Radio(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.curVideoAspect);
			//control_DVD_IR_CMD(pForyouDVDInfo,0x14);
		}
		break;

	case 0x40:
		control_FlyAudioInfo(pForyouDVDInfo,&buf[1],len-1);
		break;

#if FORYOU_DVD_USB_SUPPORT
	case 0x20:
		if (0x00 == buf[1])//Disc
		{
			if (pForyouDVDInfo->sForyouDVDInfo.HaveDisc)//有Disc设备才跳转
			{
				pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDWantPlayDevice = buf[1];
				pForyouDVDInfo->sForyouDVDInfo.bStartChangePlayDevice = TRUE;
				pForyouDVDInfo->sForyouDVDInfo.iStartChangePlayDeviceTime = GetTickCount();
				returnDVDDevicePlayDevice(pForyouDVDInfo,buf[1]);
				control_DVD_IR_CMD(pForyouDVDInfo,0x95);
			}
		}
		else if (0x01 == buf[1])//USB
		{
			if (pForyouDVDInfo->sForyouDVDInfo.HaveUSB)//有USB设备才跳转
			{
				pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDWantPlayDevice = buf[1];
				pForyouDVDInfo->sForyouDVDInfo.bStartChangePlayDevice = TRUE;
				pForyouDVDInfo->sForyouDVDInfo.iStartChangePlayDeviceTime = GetTickCount();
				returnDVDDevicePlayDevice(pForyouDVDInfo,buf[1]);
				control_DVD_IR_CMD(pForyouDVDInfo,0x96);
			}
		}
		break;
#endif
	case 0xFF:
		if (0x01 == buf[1])
		{
			FCD_PowerUp((DWORD)pForyouDVDInfo);
		} 
		else if (0x00 == buf[1])
		{
			FCD_PowerDown((DWORD)pForyouDVDInfo);
		}
		break;
	default:
		DBGD((TEXT(" Unhandle WinCE Command!")));
		break;
	}
}

BYTE DVDReqStepChangeAndReq(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE DVDReqStep)
{
	UINT i,j;
	BOOL bCheck = FALSE;
	do 
	{
		if (pForyouDVDInfo->sForyouDVDInfo.pBFolderFileListFolderErrorCheck)//文件夹接收检测
		{
			for (i = 0;i < pForyouDVDInfo->sForyouDVDInfo.pFolderCount;i++)
			{
				if (!pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[i].bStorage)
				{
					j = i+1;
					while (j < pForyouDVDInfo->sForyouDVDInfo.pFolderCount &&
						(!pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[j].bStorage))
					{
						j++;
					}
					control_DVD_ReqFileFolderDetailedInfo(pForyouDVDInfo,FALSE,i,j-i);
					DBGD((TEXT("<%d>"),DVDReqStep));
					DBGD((TEXT("\r\nFlyAudio FolderList has error!Start:%d,Count:%d"),i,j-i));
					return DVDReqStep;
				}
			}
			if (i == pForyouDVDInfo->sForyouDVDInfo.pFolderCount)
			{
				DBGD((TEXT("\r\nFlyAudio FolderList check OK!")));
				pForyouDVDInfo->sForyouDVDInfo.pBFolderFileListFolderErrorCheck = FALSE;
				pForyouDVDInfo->sForyouDVDInfo.pBGetFolderFinish = TRUE;
			}
		}
		if (pForyouDVDInfo->sForyouDVDInfo.pBFolderFileListFileErrorCheck)//文件接收检测
		{
			for (i = 0;i < pForyouDVDInfo->sForyouDVDInfo.pFileCount;i++)
			{
				if (!pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[i].bStorage)
				{
					j = i+1;
					while (j < pForyouDVDInfo->sForyouDVDInfo.pFileCount &&
						(!pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[j].bStorage))
					{
						j++;
					}
					control_DVD_ReqFileFolderDetailedInfo(pForyouDVDInfo,TRUE,i,j-i);
					DBGD((TEXT("<%d>"),DVDReqStep));
						DBGD((TEXT("\r\nFlyAudio FileList has error!Start:%d,Count:%d"),i,j-i));
						return DVDReqStep;
				}
			}
			if (i == pForyouDVDInfo->sForyouDVDInfo.pFileCount)
			{
				DBGD((TEXT("\r\nFlyAudio FileList check OK!")));
				pForyouDVDInfo->sForyouDVDInfo.pBFolderFileListFileErrorCheck = FALSE;
				pForyouDVDInfo->sForyouDVDInfo.pBGetFileFinish = TRUE;
			}
		}
		if (pForyouDVDInfo->sForyouDVDInfo.pBGetFolderFinish && pForyouDVDInfo->sForyouDVDInfo.pBGetFileFinish)
		{
			if (FALSE == pForyouDVDInfo->sForyouDVDInfo.pBGetFileFolderFinish)
			{
				DBGD((TEXT("\r\nFlyAudio FolderFileList Start Return!")));
					pForyouDVDInfo->sForyouDVDInfo.pBGetFileFolderFinish = TRUE;
				UINT isFolderCount = getSelectParentFolderFileCount(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder,TRUE);
				UINT isFileCount = getSelectParentFolderFileCount(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder,FALSE);
				returnDVDCurrentFolderInfo(pForyouDVDInfo,isFolderCount+isFileCount,isFolderCount,TRUE);

				if (pForyouDVDInfo->sForyouDVDInfo.bRecE0AndNeedProc)
				{
					procDVDInfoE0(pForyouDVDInfo);
				}
	
				pForyouDVDInfo->sForyouDVDInfo.bNeedSend8CTime = GetTickCount();
			}
		}

		if (0 == DVDReqStep)//查播放设备
		{
			DVDReqStep = 2;
			//control_DVD_ReqMechanismState(pForyouDVDInfo,0x03);
			//DBGD((TEXT("<%d>"),DVDReqStep));
			//return DVDReqStep;
		}
		//if (1 == DVDReqStep)//查播放设备状态
		//{
		//	DVDReqStep = 2;
		//	if (pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice <= 2)
		//	{
		//		control_DVD_ReqMechanismState(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice);
		//		DBGD((TEXT("<%d>"),DVDReqStep));
		//		return DVDReqStep;
		//	}
		//	else if (0xFF == pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice)
		//	{
		//		control_DVD_ReqMechanismState(pForyouDVDInfo,0);
		//		DBGD((TEXT("<%d>"),DVDReqStep));
		//		return DVDReqStep;
		//	}
		//}
		if (2 == DVDReqStep)
		{
			DVDReqStep = 3;
			if (0x09 == pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[3])
			{
				control_DVD_ReqMediaState(pForyouDVDInfo);
				DBGD((TEXT("<%d>"),DVDReqStep));
				return DVDReqStep;
			}
		}
		if (3 == DVDReqStep)
		{
			DVDReqStep = 4;
			if (pForyouDVDInfo->sForyouDVDInfo.pBGetFileFolderFinish)
			{
				control_DVD_ReqHighLightFileIndex(pForyouDVDInfo);
				DBGD((TEXT("<%d>"),DVDReqStep));
				return DVDReqStep;
			}
		}
		if (4 == DVDReqStep)
		{
			DVDReqStep =5;
			if (0xFF == pForyouDVDInfo->sForyouDVDInfo.iDVDReturnRegionCode)
			{
				control_DVD_ReqRegionCode(pForyouDVDInfo);
				DBGD((TEXT("<%d>"),DVDReqStep));
				return DVDReqStep;
			}
		}
		if (5 == DVDReqStep)//确保碟机开启自动发送
		{
			DVDReqStep = 6;
			if (GetTickCount() - pForyouDVDInfo->sForyouDVDInfo.bDeviceRec89 > 3000)
			{
				pForyouDVDInfo->sForyouDVDInfo.bDeviceRec89 = GetTickCount();
				control_DVD_PlayBack_DisplayInfo_State_Request(pForyouDVDInfo,TRUE);
				DBGD((TEXT("<%d>"),DVDReqStep));
				return DVDReqStep;
			}
		}
		if (6 == DVDReqStep)//查询碟机程序版本号，确保有一个放在最后一直发送
		{
			DVDReqStep = 0;
			//if (0x00 == pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion[0])
			//{
				control_DVD_ReqDVDSoftwareVersion(pForyouDVDInfo);
				DBGD((TEXT("<%d>"),DVDReqStep));
				return DVDReqStep;
			//}
		}
		//if (7 == DVDReqStep)//循环查询各个设备状态，确保有一个放在最后一直发送
		//{
		//	DVDReqStep = 0;
		//	pForyouDVDInfo->sForyouDVDInfo.CurrentReqMechanismCircle++;
		//	pForyouDVDInfo->sForyouDVDInfo.CurrentReqMechanismCircle %= 2;
		//	control_DVD_ReqMechanismState(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.CurrentReqMechanismCircle);
		//	DBGD((TEXT("<%d>"),DVDReqStep));
		//	return DVDReqStep;
		//}		
		if (bCheck)//大意是重新检查一次
		{
			bCheck = FALSE;
		} 
		else
		{
			bCheck = TRUE;
		}
	} while (bCheck);
	DVDReqStep = 0;
	return DVDReqStep;
}
static DWORD WINAPI ThreadFlyForyouDVDLECControlProc(LPVOID pContext)
{
	P_FLY_FORYOU_DVD_INFO pForyouDVDInfo = (P_FLY_FORYOU_DVD_INFO)pContext;
	ULONG WaitReturn;

	while (!pForyouDVDInfo->bKillDispatchForyouDVDLEDControlThread)
	{
		WaitReturn = WaitForSingleObject(pForyouDVDInfo->hDispatchThreadForyouDVDLEDControlEvent, INFINITE);
		if (pForyouDVDInfo->sForyouDVDInfo.LEDFlash)
		{
			DVD_LEDControl_On(pForyouDVDInfo);Sleep(365);DVD_LEDControl_Off(pForyouDVDInfo);Sleep(365);
			DVD_LEDControl_On(pForyouDVDInfo);Sleep(365);DVD_LEDControl_Off(pForyouDVDInfo);Sleep(365);
			DVD_LEDControl_On(pForyouDVDInfo);Sleep(365);DVD_LEDControl_Off(pForyouDVDInfo);Sleep(365);
			DVD_LEDControl_On(pForyouDVDInfo);Sleep(365);DVD_LEDControl_Off(pForyouDVDInfo);Sleep(365);
		}
	}
	pForyouDVDInfo->hThreadHandleFlyForyouDVDLEDControl = NULL;
	DBGD((TEXT("\r\nFlyAudio ThreadFlyForyouDVDProcLEDControl exit")));
	return 0;
}

static void procDVDStop(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bForceSend)
{
	if (bForceSend || GetTickCount() - pForyouDVDInfo->sForyouDVDInfo.iDVDStateCheckTime >= 3000)//一定要控到STOP
	{
		pForyouDVDInfo->sForyouDVDInfo.iDVDStateCheckTime = GetTickCount();
		if (0x17 == pForyouDVDInfo->sForyouDVDInfo.bDVDRequestState
			&& 0x00 != pForyouDVDInfo->sForyouDVDInfo.bDVDResponseState)
		{
			RETAILMSG(1, (TEXT("\r\nForyou Force DVD State")));
			//control_DVD_IR_CMD(pForyouDVDInfo,0x17);
		}
	}
}
static void processForyouDVD(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,ULONG WaitReturn)
{
	if (pForyouDVDInfo->hDVDComm && pForyouDVDInfo->hDVDComm != INVALID_HANDLE_VALUE)//串口OK
	{
		if (pForyouDVDInfo->bPower)
		{
			if (!pForyouDVDInfo->bPowerUp)
			{
				pForyouDVDInfo->bPowerUp = TRUE;
				DVD_LEDControl_Off(pForyouDVDInfo);
#if FORYOU_DVD_RESET_FLYER
				pForyouDVDInfo->sForyouDVDInfo.MechanismInitialize = TRUE;
				returnDVDDeviceWorkMode(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.MechanismInitialize);
				control_DVD_PlayBack_DisplayInfo_State_Request(pForyouDVDInfo,TRUE);
				if (0xFF != pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[0])
				{
					returnDVDDeviceActionState(pForyouDVDInfo,0,pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[0]);
				}
				control_DVD_ReqMechanismState(pForyouDVDInfo,0);
#else
				if (!pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)
				{
					//DVD_Reset_On(pForyouDVDInfo);
					//Sleep(100);
					//DVD_Reset_Off(pForyouDVDInfo);
				}
#endif
			}
		}

		if (pForyouDVDInfo->sForyouDVDInfo.MechanismInitialize)
		{
#if FORYOU_DVD_BUG_FIX
			if (pForyouDVDInfo->sForyouDVDInfo.bNeedReturnNoDiscAfterClose)
			{
				if (GetTickCount() - pForyouDVDInfo->sForyouDVDInfo.iNeedReturnNoDiscAfterCloseTime >= 618*5)
				{
					pForyouDVDInfo->sForyouDVDInfo.bNeedReturnNoDiscAfterClose = FALSE;
					if (0x03 == pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[0])
					{
						pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[0] = 0x00;
						returnDVDDeviceActionState(pForyouDVDInfo,0,pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[0]);
					}
				}
			}
#endif
			DBGD((TEXT("\r\nFlyAudio processForyouDVD-> procDVDStop LDH:")));
			procDVDStop(pForyouDVDInfo,FALSE);

			if (pForyouDVDInfo->sForyouDVDInfo.bNeedSend8CTime)//CD-Text
			{
				if (GetTickCount() - pForyouDVDInfo->sForyouDVDInfo.bNeedSend8CTime >= 100)
				{
					pForyouDVDInfo->sForyouDVDInfo.bNeedSend8CTime = 0;
					if (
						((pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion[0] << 8) + pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion[1])
						>= ((((11 << 4) + 11) << 8) + 11)
						)//确定版本号，11.11.11之后的都要有
					{
						if (0x03 == pForyouDVDInfo->sForyouDVDInfo.MediaFileType)//CDDA
						{
							control_DVD_ID3CDText(pForyouDVDInfo,0x51,1,pForyouDVDInfo->sForyouDVDInfo.pFileCount);
						}
					}
				}
			}
			if (IpcWhatEvent(EVENT_GLOBAL_DVD_REGION_SET_ID))
			{
				IpcClearEvent(EVENT_GLOBAL_DVD_REGION_SET_ID);
				control_DVD_ControlRegionCode(pForyouDVDInfo,pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDRegionCode);
				pForyouDVDInfo->sForyouDVDInfo.iDVDReturnRegionCode = 0xFF;//重新查询
			}


#if FORYOU_DVD_USB_SUPPORT
			if (pForyouDVDInfo->sForyouDVDInfo.bStartChangePlayDevice)
			{
				if (0x09 == pForyouDVDInfo->sForyouDVDInfo.DeviceStatus
					&& 0x09 == pForyouDVDInfo->sForyouDVDInfo.DeviceType
					&& 0x00 == pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDWantPlayDevice)//读取完成且是想要的
				{
					RETAILMSG(1, (TEXT("\r\nFlyAudio USB Read Disc Ready")));
					pForyouDVDInfo->sForyouDVDInfo.bStartChangePlayDevice = FALSE;
				}
				else if (0x09 == pForyouDVDInfo->sForyouDVDInfo.DeviceStatus
					&& 0x08 == pForyouDVDInfo->sForyouDVDInfo.DeviceType
					&& 0x01 == pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDWantPlayDevice)//读取完成且是想要的
				{
					RETAILMSG(1, (TEXT("\r\nFlyAudio USB Read USB Ready")));
					pForyouDVDInfo->sForyouDVDInfo.bStartChangePlayDevice = FALSE;
				}
				else//否则，给我一直切
				{
					if (0x00 == pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDWantPlayDevice)
					{
						control_DVD_IR_CMD(pForyouDVDInfo,0x95);//切换到DVD
					}
					else if (0x01 == pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDWantPlayDevice)
					{
						control_DVD_IR_CMD(pForyouDVDInfo,0x96);//切换到USB
					}

					if (GetTickCount() - pForyouDVDInfo->sForyouDVDInfo.iStartChangePlayDeviceTime > 20000)//超过最长时间，放弃吧
					{
						RETAILMSG(1, (TEXT("\r\nFlyAudio USB Read TimeOut")));
						pForyouDVDInfo->sForyouDVDInfo.bStartChangePlayDevice = FALSE;
					}
				}
			}
			else
			{
				if (0x09 == pForyouDVDInfo->sForyouDVDInfo.DeviceStatus)//读取完成
				{
					if (0x09 == pForyouDVDInfo->sForyouDVDInfo.DeviceType
						&& 0x01 == pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDWantPlayDevice)//与现实不同
					{
						RETAILMSG(1, (TEXT("\r\nFlyAudio USB Read To Disc Crack")));
						pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDWantPlayDevice = 0x00;
						returnDVDDevicePlayDevice(pForyouDVDInfo,pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDWantPlayDevice);
					}
					else if (0x08 == pForyouDVDInfo->sForyouDVDInfo.DeviceType
						&& 0x00 == pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDWantPlayDevice)//与现实不同
					{
						RETAILMSG(1, (TEXT("\r\nFlyAudio USB Read To USB Crack")));
						pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDWantPlayDevice = 0x01;
						returnDVDDevicePlayDevice(pForyouDVDInfo,pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDWantPlayDevice);
					}
				}

				if (0x01 == pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDWantPlayDevice
					&& pForyouDVDInfo->sForyouDVDInfo.HaveUSB
					&& 0x00 == pForyouDVDInfo->sForyouDVDInfo.PlayStatus)
				{
					RETAILMSG(1, (TEXT("\r\nFlyAudio USB Change To USB Continue %d"), pForyouDVDInfo->sForyouDVDInfo.PlayStatus));
					control_DVD_IR_CMD(pForyouDVDInfo,0x96);//切换到USB
				}
			}
#endif

			if (pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos)
			{
				RETAILMSG(1, (TEXT("\r\nForyou Have Jump")));
				if (GetTickCount() - pForyouDVDInfo->sForyouDVDInfo.iQuickJumpNextTimer >= 618)
				{
					RETAILMSG(1, (TEXT("\r\nForyou Have Jump On Time")));
					if (0x02 == pForyouDVDInfo->sForyouDVDInfo.MediaDiscType)//Clips
					{
						if (1 == pForyouDVDInfo->sForyouDVDInfo.iQuickJumpNextCount)
						{
							if (pForyouDVDInfo->sForyouDVDInfo.bQuickJumpNext)
							{
								control_DVD_IR_CMD(pForyouDVDInfo,0x18);
							}
							else
							{
								control_DVD_IR_CMD(pForyouDVDInfo,0x19);
							}
						}
						else
						{
							selectFolderOrFileByLocalIndex(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.pNowPlayingInWhatFolder,pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos-1,FALSE);
						}
						pForyouDVDInfo->sForyouDVDInfo.iQuickJumpNextCount = 0;
						pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos = 0;
					}
					else if (pForyouDVDInfo->sForyouDVDInfo.bQuickJumpNext)
					{
						RETAILMSG(1, (TEXT("\r\nForyou Have Jump Next %d"),pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos));
						if (1 == pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos)
						{
							control_DVD_IR_CMD(pForyouDVDInfo,0x18);
						}
						else
						{
							control_DVD_JumpNextN(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos);
						}
						pForyouDVDInfo->sForyouDVDInfo.iQuickJumpNextCount = 0;
						pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos = 0;
					}
					else
					{
						RETAILMSG(1, (TEXT("\r\nForyou Have Jump Prev %d"),pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos));
						if (1 == pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos)
						{
							control_DVD_IR_CMD(pForyouDVDInfo,0x19);
						}
						else
						{
							control_DVD_JumpPrevN(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos);
						}
						pForyouDVDInfo->sForyouDVDInfo.iQuickJumpNextCount = 0;
						pForyouDVDInfo->sForyouDVDInfo.iQuickJumpTitlePos = 0;
					}
				}
			}

			if (pForyouDVDInfo->sForyouDVDInfo.curVideoAspect !=pForyouDVDInfo->sForyouDVDInfo.preVideoAspect)
			{
				//视频比例切换
				pForyouDVDInfo->sForyouDVDInfo.curVideoAspect = pForyouDVDInfo->sForyouDVDInfo.preVideoAspect;

				//control_DVD_IR_CMD(pForyouDVDInfo,0x17);
				//Sleep(100);
				control_DVD_Video_Aspect_Radio(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.curVideoAspect);

				//if (0x00 == pForyouDVDInfo->sForyouDVDInfo.curVideoAspect)
				//{
				//	//control_DVD_Set_View_Mode(pForyouDVDInfo,0x04);
				//	//Sleep(1000);
				//	control_DVD_Video_Aspect_Radio(pForyouDVDInfo,0x01);
				//	//Sleep(1000);
				//}
				//else if (0x01 == pForyouDVDInfo->sForyouDVDInfo.curVideoAspect)
				//{
				//	//control_DVD_Set_View_Mode(pForyouDVDInfo,0x04);
				//	//Sleep(1000);
				//	control_DVD_Video_Aspect_Radio(pForyouDVDInfo,0x00);
				////	Sleep(1000);
				//}
				//else if (0x02 == pForyouDVDInfo->sForyouDVDInfo.curVideoAspect)
				//{
				//	//control_DVD_Set_View_Mode(pForyouDVDInfo,0x00);
				////	Sleep(1000);
				//	control_DVD_Video_Aspect_Radio(pForyouDVDInfo,0x01);
				//	//Sleep(1000);
				//}

				control_DVD_IR_CMD(pForyouDVDInfo,0x14);
			}

			if (pForyouDVDInfo->sForyouDVDInfo.pBGetFileFolderFinish)//已完成接收
			{
				if (pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount != pForyouDVDInfo->sForyouDVDInfo.pNeedReturnCount)//需要返回
				{
					UINT isFolderCount = getSelectParentFolderFileCount(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder,TRUE);
					UINT isFileCount = getSelectParentFolderFileCount(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder,FALSE);
					UINT isWhere;
					DBGD((TEXT("\r\nFlyAudio ThreadFlyForyouDVDProc Need Return In:%d TotalFolder:%d TotalFile:%d"),pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder,isFolderCount,isFileCount));
						while (pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount != pForyouDVDInfo->sForyouDVDInfo.pNeedReturnCount)//需要返回
						{
							DBGD((TEXT(" More")));
								if ((pForyouDVDInfo->sForyouDVDInfo.pNeedReturnStart + pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount) < isFolderCount)//返回文件夹
								{
									isWhere = getSelectParentFolderFileInfoBySubIndex(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder,TRUE
										,pForyouDVDInfo->sForyouDVDInfo.pNeedReturnStart + pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount);
									DBGD((TEXT(" Folder:%d"),isWhere));
										if (-1 != isWhere)
										{
											if (0 == pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder)//当前是根目录
											{
												returnDVDFileFolderInfo(pForyouDVDInfo,TRUE
													,pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[isWhere].name
													,pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[isWhere].nameLength
													,pForyouDVDInfo->sForyouDVDInfo.pNeedReturnStart + pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount
													,isWhere);
											}
											else
											{
												returnDVDFileFolderInfo(pForyouDVDInfo,TRUE
													,pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[isWhere].name
													,pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList[isWhere].nameLength
													,pForyouDVDInfo->sForyouDVDInfo.pNeedReturnStart + pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount + 1
													,isWhere);
											}
											pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount++;
										}
										else
										{
											pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount = pForyouDVDInfo->sForyouDVDInfo.pNeedReturnCount;
										}
								}
								else if ((pForyouDVDInfo->sForyouDVDInfo.pNeedReturnStart + pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount) < (isFolderCount + isFileCount))//返回文件
								{
									isWhere = getSelectParentFolderFileInfoBySubIndex(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder,FALSE
										,pForyouDVDInfo->sForyouDVDInfo.pNeedReturnStart + pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount - isFolderCount);
									DBGD((TEXT(" File:%d"),isWhere));
										if (-1 != isWhere)
										{
											if (0 == pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder)//当前是根目录
											{
												returnDVDFileFolderInfo(pForyouDVDInfo,FALSE
													,pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[isWhere].name
													,pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[isWhere].nameLength
													,pForyouDVDInfo->sForyouDVDInfo.pNeedReturnStart + pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount
													,isWhere);
											}
											else
											{
												returnDVDFileFolderInfo(pForyouDVDInfo,FALSE
													,pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[isWhere].name
													,pForyouDVDInfo->sForyouDVDInfo.pFileTreeList[isWhere].nameLength
													,pForyouDVDInfo->sForyouDVDInfo.pNeedReturnStart + pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount + 1
													,isWhere);
											}
											pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount++;
										}
										else
										{
											pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount = pForyouDVDInfo->sForyouDVDInfo.pNeedReturnCount;
										}
								}
								else
								{
									pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount = pForyouDVDInfo->sForyouDVDInfo.pNeedReturnCount;
								}
						}
				}
			}

			if (pForyouDVDInfo->sForyouDVDInfo.pBStartGetFolderFile)
			{
				if (0 == pForyouDVDInfo->sForyouDVDInfo.pStartGetFolderFileCount)//控制状态跳转
				{
					if (pForyouDVDInfo->sForyouDVDInfo.pReturnE8)
					{
						if (pForyouDVDInfo->sForyouDVDInfo.pFolderCount)
						{
							pForyouDVDInfo->sForyouDVDInfo.pStartGetFolderFileCount = 1;
						} 
						else if (pForyouDVDInfo->sForyouDVDInfo.pFileCount)
						{
							pForyouDVDInfo->sForyouDVDInfo.pStartGetFolderFileCount = 2;
						}
						else
						{
							pForyouDVDInfo->sForyouDVDInfo.pStartGetFolderFileCount = 3;
						}
					}
				}
				else if (1 == pForyouDVDInfo->sForyouDVDInfo.pStartGetFolderFileCount)
				{
					if (pForyouDVDInfo->sForyouDVDInfo.pReturnEAFolder)
					{
						if (pForyouDVDInfo->sForyouDVDInfo.pFileCount)
						{
							pForyouDVDInfo->sForyouDVDInfo.pStartGetFolderFileCount = 2;
						}
						else
						{
							pForyouDVDInfo->sForyouDVDInfo.pStartGetFolderFileCount = 3;
						}
					}
				}
				else if (2 == pForyouDVDInfo->sForyouDVDInfo.pStartGetFolderFileCount)
				{
					if (pForyouDVDInfo->sForyouDVDInfo.pReturnEAFile)
					{
						pForyouDVDInfo->sForyouDVDInfo.pStartGetFolderFileCount = 3;
					}
				}

				if (0 == pForyouDVDInfo->sForyouDVDInfo.pStartGetFolderFileCount)//执行状态操作
				{
					if (WAIT_TIMEOUT == WaitReturn || pForyouDVDInfo->sForyouDVDInfo.pBReqFolderFileCommandActiveNow)
					{
						control_DVD_ReqFileFolderCount(pForyouDVDInfo);
					}
				}
				else if (1 == pForyouDVDInfo->sForyouDVDInfo.pStartGetFolderFileCount)
				{
					if (WAIT_TIMEOUT == WaitReturn || pForyouDVDInfo->sForyouDVDInfo.pBReqFolderFileCommandActiveNow)
					{
						control_DVD_ReqFileFolderDetailedInfo(pForyouDVDInfo,0,0,pForyouDVDInfo->sForyouDVDInfo.pFolderCount);
					}				
				}
				else if (2 == pForyouDVDInfo->sForyouDVDInfo.pStartGetFolderFileCount)
				{
					if (WAIT_TIMEOUT == WaitReturn || pForyouDVDInfo->sForyouDVDInfo.pBReqFolderFileCommandActiveNow)
					{
						control_DVD_ReqFileFolderDetailedInfo(pForyouDVDInfo,1,0,pForyouDVDInfo->sForyouDVDInfo.pFileCount);
					}
				}
				else
				{
					DBGD((TEXT("\r\nForyouDVD End get folder and file list")));
					pForyouDVDInfo->sForyouDVDInfo.pBStartGetFolderFile = FALSE;
					pForyouDVDInfo->sForyouDVDInfo.pBFolderFileListFolderErrorCheck = TRUE;
					pForyouDVDInfo->sForyouDVDInfo.pBFolderFileListFileErrorCheck = TRUE;
				}

				pForyouDVDInfo->sForyouDVDInfo.pBReqFolderFileCommandActiveNow = FALSE;
			}
			else if (WAIT_TIMEOUT == WaitReturn || pForyouDVDInfo->sForyouDVDInfo.bForceExecReqStep)//如果超时退出
			{
				pForyouDVDInfo->sForyouDVDInfo.bForceExecReqStep = FALSE;
				pForyouDVDInfo->sForyouDVDInfo.DVDReqStep = DVDReqStepChangeAndReq(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.DVDReqStep);
			}
			else
			{

			}
		}
	}
}

static DWORD WINAPI ThreadFlyForyouDVDProc(LPVOID pContext)
{
	P_FLY_FORYOU_DVD_INFO pForyouDVDInfo = (P_FLY_FORYOU_DVD_INFO)pContext;
	ULONG WaitReturn;

	while (!pForyouDVDInfo->bKillDispatchForyouDVDThread)
	{
		if (pForyouDVDInfo->sForyouDVDInfo.pBStartGetFolderFile)
		{
			WaitReturn = WaitForSingleObject(pForyouDVDInfo->hDispatchThreadForyouDVDEvent, 618*5);
		}
		else
		{
			WaitReturn = WaitForSingleObject(pForyouDVDInfo->hDispatchThreadForyouDVDEvent, 618);
		}
	//	DBGD((TEXT("\r\nFlyAudio ThreadFlyForyouDVDProc %d"),WaitReturn));

		if (pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSilencePowerUp)
		{
			//DVD_Reset_On(pForyouDVDInfo);
			continue;
		}

		if (!pForyouDVDInfo->bEnterUpdateMode
			&& !pForyouDVDInfo->bAutoResetControlOn
			&& !pForyouDVDInfo->bFlyaudioDVD)//碟机死机的复位
		{
			if (GetTickCount() - pForyouDVDInfo->iAutoResetControlTime >= 150*1000)
			{
				//DVD_Reset_On(pForyouDVDInfo);
				//Sleep(100);
				//DVD_Reset_Off(pForyouDVDInfo);
			}
		}
		
		if (IpcWhatEvent(EVENT_GLOBAL_DVD_CHANNEL_ID))
		{
			IpcClearEvent(EVENT_GLOBAL_DVD_CHANNEL_ID);
			if (MediaCD == pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eAudioInput)
			{
				//第一次切DVD
				if (pForyouDVDInfo->bDVDAudioChannelFirstInit)
				{
					pForyouDVDInfo->bDVDAudioChannelFirstInit = FALSE;
					if (pForyouDVDInfo->bDVDSendInitStatusStart)
						IpcStartEvent(EVENT_GLOBAL_STANDBY_DVD_ID);	
				}
			}
		}

		if (IpcWhatEvent(EVENT_GLOBAL_STANDBY_DVD_ID) 
			&& pForyouDVDInfo->bPower
			&& !pForyouDVDInfo->bDVDAudioChannelFirstInit)
		{
			DBGE((_T("\r\n EVENT_GLOBAL_STANDBY_DVD_ID bDVDSendInitStatusStart:%d"),pForyouDVDInfo->bDVDSendInitStatusStart));
			IpcClearEvent(EVENT_GLOBAL_STANDBY_DVD_ID);
			if (pForyouDVDInfo->bDVDSendInitStatusStart)//确定是DVD EXE发过来的
			{
				if (pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)
				{
					DVD_Reset_Off(pForyouDVDInfo);
				}
				else
				{
					//倒车有视频，DVD不先初始化
					if (pForyouDVDInfo->bDVDDriverInitFirst)//DVD第一次起来
					{
						if (pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBackActiveNow)
						{
							DBGI((TEXT("\r\nDVD have back status.....\r\n")));
						}
						else
						{
							pForyouDVDInfo->bDVDDriverInitFirst = FALSE;
							DVD_Reset_On(pForyouDVDInfo);
						}	
					}
					else
					{
						DVD_Reset_On(pForyouDVDInfo);
					}
				}
			}
		}

		if (IpcWhatEvent(EVENT_BT_OR_RING_STATUS_ID))
		{
			IpcClearEvent(EVENT_BT_OR_RING_STATUS_ID);
			if (pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDVDBTStatus)
			{
				returnDVDBtStatus(pForyouDVDInfo,TRUE);
			}
			else
			{
				returnDVDBtStatus(pForyouDVDInfo,FALSE);
			}
		}

		if (pForyouDVDInfo->bFlyaudioDVD)
		{
			processFlyaudioDVD(pForyouDVDInfo,WaitReturn);
		}
		else
		{
			processForyouDVD(pForyouDVDInfo,WaitReturn);
		}

		if (!pForyouDVDInfo->bSocketConnecting)
		{
			DBGE((TEXT("\r\nDVD socket can't recv msg(ensure the socket is connect and send init commd)\r\n")));
		}
	}
	while (pForyouDVDInfo->hThreadHandleFlyForyouDVDLEDControl)
	{
		SetEvent(pForyouDVDInfo->hDispatchThreadForyouDVDLEDControlEvent);
		Sleep(10);
	}
	CloseHandle(pForyouDVDInfo->hDispatchThreadForyouDVDLEDControlEvent);

	CloseHandle(pForyouDVDInfo->hDispatchThreadForyouDVDEvent);
	DBGD((TEXT("\r\nFlyAudio ThreadFlyForyouDVDProc exit")));
	return 0;
}

void ForyouDVD_Enable(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bEnable)
{
	DWORD dwThreadID;
	if (bEnable)
	{
//#if FORYOU_DVD_RESET_FLYER
//		DVD_Reset_Off(pForyouDVDInfo);
//#else
//		DVD_Reset_On(pForyouDVDInfo);
//#endif
//		DVD_LEDControl_Off(pForyouDVDInfo);

		pForyouDVDInfo->bKillDispatchForyouDVDLEDControlThread = FALSE;
		pForyouDVDInfo->hDispatchThreadForyouDVDLEDControlEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
		pForyouDVDInfo->hThreadHandleFlyForyouDVDLEDControl = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadFlyForyouDVDLECControlProc, //线程的全局函数
			pForyouDVDInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\nFlyAudio ThreadFlyForyouDVDLEDControlProc ID:%x"),dwThreadID));

		if (NULL == pForyouDVDInfo->hThreadHandleFlyForyouDVDLEDControl)
		{
			pForyouDVDInfo->bKillDispatchForyouDVDLEDControlThread = TRUE;
			return;
		}

		pForyouDVDInfo->bKillDispatchForyouDVDThread = FALSE;
		pForyouDVDInfo->hDispatchThreadForyouDVDEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	
		pForyouDVDInfo->hThreadHandleFlyForyouDVD = 
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL, //安全属性
			0,//初始化线程栈的大小，缺省为与主线程大小相同
			(LPTHREAD_START_ROUTINE)ThreadFlyForyouDVDProc, //线程的全局函数
			pForyouDVDInfo, //此处传入了主框架的句柄
			0, &dwThreadID );
		DBGE((TEXT("\r\nFlyAudio ThreadFlyForyouDVDProc ID:%x"),dwThreadID));

		if (NULL == pForyouDVDInfo->hThreadHandleFlyForyouDVD)
		{
			pForyouDVDInfo->bKillDispatchForyouDVDThread = TRUE;
			return;
		}

		openDVDComm(pForyouDVDInfo);
		structDVDInfoInit(pForyouDVDInfo,TRUE);
	}
	else
	{
		pForyouDVDInfo->bKillDispatchForyouDVDThread = TRUE;
		SetEvent(pForyouDVDInfo->hDispatchThreadForyouDVDEvent);

		pForyouDVDInfo->bKillDispatchForyouDVDLEDControlThread = TRUE;
		SetEvent(pForyouDVDInfo->hDispatchThreadForyouDVDLEDControlEvent);

		closeDVDComm(pForyouDVDInfo);
	}
}

static void powerNormalDeInit(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
{
	pForyouDVDInfo->buffToUserHx = 0;
	pForyouDVDInfo->buffToUserLx = 0;

	pForyouDVDInfo->DVDInfoFrameStatus = 0;
	pForyouDVDInfo->DVDInfoFrameLengthMax = 0;
	pForyouDVDInfo->DVDInfoFrameLength = 0;
	pForyouDVDInfo->DVDInfoFrameChecksum = 0;
	pForyouDVDInfo->DVDInfoFrameCRCErrorTailLength = 0;
	pForyouDVDInfo->bPower = FALSE;
	pForyouDVDInfo->bPowerUp = FALSE;
}

static void powerNormalInit(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
{
	pForyouDVDInfo->buffToUserHx = 0;
	pForyouDVDInfo->buffToUserLx = 0;

	pForyouDVDInfo->DVDInfoFrameStatus = 0;
	pForyouDVDInfo->DVDInfoFrameLengthMax = 0;
	pForyouDVDInfo->DVDInfoFrameLength = 0;
	pForyouDVDInfo->DVDInfoFrameChecksum = 0;
}

static void powerOnFirstInit(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo)
{
	pForyouDVDInfo->bOpen = FALSE;
	pForyouDVDInfo->bPower = FALSE;
	pForyouDVDInfo->bPowerUp = FALSE;
	pForyouDVDInfo->bSpecialPower = FALSE;

	pForyouDVDInfo->bEnterUpdateMode   = FALSE;
	pForyouDVDInfo->bDVDDriverInitFirst = TRUE;

	pForyouDVDInfo->bKillDispatchForyouDVDThread = TRUE;
	pForyouDVDInfo->hThreadHandleFlyForyouDVD = NULL;
	pForyouDVDInfo->hDispatchThreadForyouDVDEvent = NULL;

	pForyouDVDInfo->bKillDispatchForyouDVDLEDControlThread = TRUE;
	pForyouDVDInfo->hThreadHandleFlyForyouDVDLEDControl = NULL;
	pForyouDVDInfo->hThreadHandleFlyForyouDVDLEDControl = NULL;

	pForyouDVDInfo->hDVDComm = NULL;
	pForyouDVDInfo->bKillFlyDVDCommThread = TRUE;
	pForyouDVDInfo->hThreadHandleFlyDVDComm = NULL;

	pForyouDVDInfo->bDVDSendInitStatusStart = FALSE;
	pForyouDVDInfo->bDVDAudioChannelFirstInit = TRUE;
//#if FORYOU_DVD_RESET_FLYER
//	DVD_Reset_Off(pForyouDVDInfo);
//#else
//	DVD_Reset_On(pForyouDVDInfo);
//#endif
//	DVD_LEDControl_Off(pForyouDVDInfo);
}

static void powerOnSpecialEnable(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bOn)
{
	if (bOn)
	{
		if (pForyouDVDInfo->bSpecialPower)
		{
			return;
		}
		pForyouDVDInfo->bSpecialPower = TRUE;

		pForyouDVDInfo->hBuffToUserDataEvent = CreateEvent(NULL,FALSE,FALSE,NULL);	
		InitializeCriticalSection(&pForyouDVDInfo->hCSSendToUser);

		pForyouDVDInfo->sForyouDVDInfo.pFolderTreeList = NULL;//链表需要提前初始化
		pForyouDVDInfo->sForyouDVDInfo.pFileTreeList = NULL;//链表需要提前初始化
		structDVDInfoInit(pForyouDVDInfo,TRUE);

//#if FORYOU_DVD_RESET_FLYER
//		DVD_Reset_Off(pForyouDVDInfo);
//#else
//		DVD_Reset_On(pForyouDVDInfo);
//#endif
//		DVD_LEDControl_Off(pForyouDVDInfo);
	}
	else
	{
		if (!pForyouDVDInfo->bSpecialPower)
		{
			return;
		}
		pForyouDVDInfo->bSpecialPower = FALSE;

		structDVDInfoInit(pForyouDVDInfo,TRUE);

//#if FORYOU_DVD_RESET_FLYER
//		DVD_Reset_Off(pForyouDVDInfo);
//#else
//		DVD_Reset_On(pForyouDVDInfo);
//#endif
//		DVD_LEDControl_Off(pForyouDVDInfo);

		CloseHandle(pForyouDVDInfo->hBuffToUserDataEvent);
	}
}

void IpcRecv(UINT32 iEvent)
{
	P_FLY_FORYOU_DVD_INFO	pForyouDVDInfo = (P_FLY_FORYOU_DVD_INFO)gpForyouDVDInfo;

	if (iEvent == EVENT_GLOBAL_ERROR_LEDFLASH_ID)
	{
		SetEvent(pForyouDVDInfo->hDispatchThreadForyouDVDLEDControlEvent);
	}
	else
	{
		SetEvent(pForyouDVDInfo->hDispatchThreadForyouDVDEvent);
	}
}
void SockRecv(BYTE *buf, UINT16 len)
{
	UINT16 i=0;
	P_FLY_FORYOU_DVD_INFO	pForyouDVDInfo = (P_FLY_FORYOU_DVD_INFO)gpForyouDVDInfo;

	if (len <= 0)
		return;


	DBGD((TEXT("\r\n FLY DVD socket recv %d bytes:"),len-1));
	for (i=0; i<len-1; i++)
		DBGD((TEXT("%02X "), buf[i]));


	//减1，去年检验位
	if (pForyouDVDInfo->bFlyaudioDVD)
	{
		DealFlyaudioInfo(pForyouDVDInfo,buf,len-1);
	}
	else
	{
		DealDVDInfo(pForyouDVDInfo,buf,len-1);
	}

	DBGD((TEXT("\r\n FlyAudio dvd bFlyaudioDVD:%d\r\n"),pForyouDVDInfo->bFlyaudioDVD));

	if (!pForyouDVDInfo->bSocketConnecting)
		pForyouDVDInfo->bSocketConnecting  = TRUE;
}


HANDLE
FCD_Init(DWORD dwContext)
{
	P_FLY_FORYOU_DVD_INFO pForyouDVDInfo;

	RETAILMSG(1, (TEXT("\r\nForyou Init Start")));


	pForyouDVDInfo = (P_FLY_FORYOU_DVD_INFO)LocalAlloc(LPTR, sizeof(FLY_FORYOU_DVD_INFO));
	if (!pForyouDVDInfo)
	{
		return NULL;
	}
	gpForyouDVDInfo = pForyouDVDInfo;

	pForyouDVDInfo->pFlyShmGlobalInfo = CreateShm(DVD_MODULE,IpcRecv);
	if (NULL == pForyouDVDInfo->pFlyShmGlobalInfo)
	{
		DBGE((TEXT("FlyAudio DVD Init create shm err\r\n")));
		return NULL;
	}

	if (!GetDllAddrTable())
	{
		DBGE((TEXT("FlyAudio DVD  GetDllAddrTable err\r\n")));
		return NULL;
	}

	if (!CreateUserBuff())
	{
		DBGE((TEXT("FlyAudio DVD Open create user buf err\r\n")));
		return NULL;
	}

	if(!CreateServerSocket(SockRecv, TCP_PORT_DVD))
	{
		DBGE((TEXT("FlyAudio CollexBT Open create server socket err\r\n")));
		return NULL;
	}


	if (pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalStructSize
		!= sizeof(struct shm))
	{
		pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[0] = 'F';
		pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[1] = 'C';
		pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[2] = 'D';
	}

	//参数初始化
	powerNormalInit(pForyouDVDInfo);//电源正常初始化
	powerOnFirstInit(pForyouDVDInfo);
	powerOnSpecialEnable(pForyouDVDInfo,TRUE);


	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDVDInfo.bInit = TRUE;
	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompYear = year;
	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompMon = months;
	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompDay = day;
	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompHour = hours;
	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompMin = minutes;
	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.iDriverCompSec = seconds;

	DBGI((TEXT("\r\nForyouDVD Init Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return (HANDLE)pForyouDVDInfo;
}

BOOL FCD_Deinit(DWORD hDeviceContext)
{
	P_FLY_FORYOU_DVD_INFO pForyouDVDInfo = (P_FLY_FORYOU_DVD_INFO)hDeviceContext;

	//全局
	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDVDInfo.bInit = FALSE;

	powerOnSpecialEnable(pForyouDVDInfo,FALSE);

	FreeShm();
	FreeSocketServer();
	FreeUserBuff();

	LocalFree(pForyouDVDInfo);
	DBGE((TEXT("\r\nForyouDVD DeInit")));
	return TRUE;
}

DWORD
FCD_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	P_FLY_FORYOU_DVD_INFO pForyouDVDInfo = (P_FLY_FORYOU_DVD_INFO)hDeviceContext;
	DWORD returnWhat = hDeviceContext;

	if (pForyouDVDInfo->bOpen)
	{
		return NULL;
	}
	pForyouDVDInfo->bOpen = TRUE;

	ForyouDVD_Enable(pForyouDVDInfo,TRUE);

	//全局
	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDVDInfo.bOpen = TRUE;

	DBGI((TEXT("\r\nForyouDVD Open Build:Date%d.%d.%dTime%d:%d:%d"),year,months,day,hours,minutes,seconds));
	return returnWhat;
}

BOOL
FCD_Close(DWORD hDeviceContext)
{
	P_FLY_FORYOU_DVD_INFO pForyouDVDInfo = (P_FLY_FORYOU_DVD_INFO)hDeviceContext;

	pForyouDVDInfo->bOpen = FALSE;

	ForyouDVD_Enable(pForyouDVDInfo,FALSE);

	//全局
	pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDVDInfo.bOpen = FALSE;

	SetEvent(pForyouDVDInfo->hBuffToUserDataEvent);
	DBGE((TEXT("\r\nForyouDVD Close")));
	return TRUE;
}

VOID
FCD_PowerUp(DWORD hDeviceContext)
{
	P_FLY_FORYOU_DVD_INFO pForyouDVDInfo = (P_FLY_FORYOU_DVD_INFO)hDeviceContext;

	powerNormalInit(pForyouDVDInfo);

	DBGE((TEXT("\r\nForyouDVD PowerUp")));
}

VOID
FCD_PowerDown(DWORD hDeviceContext)
{
	P_FLY_FORYOU_DVD_INFO pForyouDVDInfo = (P_FLY_FORYOU_DVD_INFO)hDeviceContext;

	powerNormalDeInit(pForyouDVDInfo);

	DBGE((TEXT("\r\nForyouDVD PowerDown")));
}

DWORD
FCD_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	P_FLY_FORYOU_DVD_INFO pForyouDVDInfo = (P_FLY_FORYOU_DVD_INFO)hOpenContext;

	UINT16 dwRead = 0,i;
	BYTE *buf = (BYTE *)pBuffer;

	dwRead = ReadData(buf,(UINT16)Count);
	if(dwRead > 0)
	{
		DBGD((TEXT("\r\nFlyAudio DVD return %d bytes:"),dwRead));
		for (i=0; i<dwRead; i++)
			DBGD((TEXT("%02X "),buf[i]));
	}

	return dwRead;
}

DWORD
FCD_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	P_FLY_FORYOU_DVD_INFO pForyouDVDInfo = (P_FLY_FORYOU_DVD_INFO)hOpenContext;


	DBGD((TEXT("\r\nForyouDVD Write %d "),
					NumberOfBytes));
	for(UINT i = 0;i < NumberOfBytes;i++)
	{
		DBGD((TEXT(" %X"),
			*((BYTE *)pSourceBytes + i)));
	}
	if(NumberOfBytes)
	{
		WinCECommandProcessor(pForyouDVDInfo,(((BYTE *)pSourceBytes)+3),((BYTE *)pSourceBytes)[2] - 1);
	}
	return NULL;
}

DWORD
FCD_Seek(DWORD hOpenContext, LONG Amount, DWORD  Type)
{
	return NULL;
}

BOOL
FCD_IOControl(DWORD hOpenContext,
			  DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn,
			  PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
	P_FLY_FORYOU_DVD_INFO pForyouDVDInfo = (P_FLY_FORYOU_DVD_INFO)hOpenContext;
	BOOL bRes = TRUE;

	//if (dwCode != IOCTL_SERIAL_WAIT_ON_MASK)
	//{
	//	DBGE((TEXT("\r\nFlyAudio Foryou IOControl(h:0x%X, Code:%X, IN:0x%X, %d, OUT:0x%X, %d, ActOut:0x%X)\r\n"),
	//		hOpenContext, dwCode, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut));
	//}

	//switch (dwCode)
	//{
	//case IOCTL_SERIAL_SET_WAIT_MASK:
	//	DBGE((TEXT("\r\nFlyAudio Foryou IOControl Set IOCTL_SERIAL_SET_WAIT_MASK")));
	//	break;
	//case IOCTL_SERIAL_WAIT_ON_MASK:
	//	//DBGE((TEXT("\r\nFlyAudio Foryou IOControl Set IOCTL_SERIAL_WAIT_ON_MASK")));
	//	if (pForyouDVDInfo->bOpen)
	//	{
	//		WaitForSingleObject(pForyouDVDInfo->hBuffToUserDataEvent,INFINITE);
	//	}
	//	else
	//	{
	//		WaitForSingleObject(pForyouDVDInfo->hBuffToUserDataEvent,0);
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

	return bRes;
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
		DEBUGMSG(1, (TEXT("Attach in FlyAudio FouyouDVD DllEntry\n")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}

	if (dwReason == DLL_PROCESS_DETACH)
	{
		DEBUGMSG(1, (TEXT("Dettach in FlyAudio FouyouDVD DllEntry\n")));
	}

	DEBUGMSG(1, (TEXT("Leaving FlyAudio FouyouDVD DllEntry\n")));

	return (TRUE);
}
