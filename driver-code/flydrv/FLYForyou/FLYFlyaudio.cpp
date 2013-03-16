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

#include "FLYForyou.h"

#define FLYAUDIO_DVD_FILE_MAX	2500
#define FLYAUDIO_DVD_FOLDER_MAX	500

void transFlyaudioInfoCB(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{
	if (pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice != p[0])
	{
		pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice = p[0];
		returnDVDDevicePlayDevice(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice);
	}
	if (pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[p[0]] != p[1])
	{
		if (0x02 == p[1])
		{
			DBGD((TEXT("\r\nForyouDVD LED Flash")));
			pForyouDVDInfo->sForyouDVDInfo.LEDFlash = TRUE;
			SetEvent(pForyouDVDInfo->hDispatchThreadForyouDVDLEDControlEvent);
		}
		pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[p[0]] = p[1];
		returnDVDDeviceActionState(pForyouDVDInfo,p[0],p[1]);
		if (0x09 != pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismStatus[p[0]])
		{
			structDVDInfoInit(pForyouDVDInfo,FALSE);
		}
	}
}

void transFlyaudioInfoCC(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{
	DBGD((TEXT("\r\nForyouDVD LDH CC:  %x %x "),p[0],p[1]));//Add by LDH
	if (pForyouDVDInfo->sForyouDVDInfo.PlayStatus != p[0]
		|| pForyouDVDInfo->sForyouDVDInfo.PlaySpeed != p[1])
	{
		pForyouDVDInfo->sForyouDVDInfo.PlayStatus = p[0];
		pForyouDVDInfo->sForyouDVDInfo.PlaySpeed = p[1];
		returnDVDPlayStatusSpeed(pForyouDVDInfo,
			pForyouDVDInfo->sForyouDVDInfo.PlayStatus,
			pForyouDVDInfo->sForyouDVDInfo.PlaySpeed);
	}
}

void transFlyaudioInfoCD(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)//媒体状态
{
	if (pForyouDVDInfo->sForyouDVDInfo.MediaDiscType != p[0]
	|| pForyouDVDInfo->sForyouDVDInfo.MediaFileType != p[1])
	{
		pForyouDVDInfo->sForyouDVDInfo.MediaDiscType = p[0];
		pForyouDVDInfo->sForyouDVDInfo.MediaFileType = p[1];
		DBGD((TEXT("\r\nForyouDVD LDH CD:  %x %x "),p[0],p[1]));//Add by LDH
		returnDVDDeviceMedia(pForyouDVDInfo,
			pForyouDVDInfo->sForyouDVDInfo.MediaDiscType,
			pForyouDVDInfo->sForyouDVDInfo.MediaFileType);

	}
}

void transFlyaudioInfoCE(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{
	//if (pForyouDVDInfo->sForyouDVDInfo.EscapeSecond != p[2]
	//|| pForyouDVDInfo->sForyouDVDInfo.EscapeMinute != p[1]
	//|| pForyouDVDInfo->sForyouDVDInfo.EscapeHour != p[0])
	{
		pForyouDVDInfo->sForyouDVDInfo.EscapeHour = p[0];
		pForyouDVDInfo->sForyouDVDInfo.EscapeMinute = p[1];
		pForyouDVDInfo->sForyouDVDInfo.EscapeSecond = p[2];
		returnDVDCurrentTime(pForyouDVDInfo,
			pForyouDVDInfo->sForyouDVDInfo.EscapeHour,
			pForyouDVDInfo->sForyouDVDInfo.EscapeMinute,
			pForyouDVDInfo->sForyouDVDInfo.EscapeSecond);
	}
}

void transFlyaudioInfoCF(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{
	if (pForyouDVDInfo->sForyouDVDInfo.CurrentChar != (p[1] * 256 + p[2])
		|| pForyouDVDInfo->sForyouDVDInfo.CurrentTitle != p[0])
	{
		pForyouDVDInfo->sForyouDVDInfo.CurrentTitle = p[0];
		pForyouDVDInfo->sForyouDVDInfo.CurrentChar = p[1] * 256 + p[2];
		DBGD( (TEXT("\r\nForyouDVD LDH CF : p[0] = %x  p[1] = %x  p[2] = %x"),p[0],p[1],p[2]));//Add by LDH
		DBGD( (TEXT("\r\nForyouDVD LDH CF CurrentTitle = %x CurrentChar = %x"),pForyouDVDInfo->sForyouDVDInfo.CurrentTitle,pForyouDVDInfo->sForyouDVDInfo.CurrentChar));//Add by LDH
		returnDVDCurrentTitleTrack(pForyouDVDInfo,
			pForyouDVDInfo->sForyouDVDInfo.CurrentTitle,
			pForyouDVDInfo->sForyouDVDInfo.CurrentChar);
		//MEATILMSG(1,)
	}

	if (pForyouDVDInfo->sForyouDVDInfo.TotalChar != (p[4] * 256 + p[5])
		|| pForyouDVDInfo->sForyouDVDInfo.TotalTitle != p[3])
	{
		pForyouDVDInfo->sForyouDVDInfo.TotalTitle = p[3];
		pForyouDVDInfo->sForyouDVDInfo.TotalChar = p[4] * 256 + p[5];
		DBGD( (TEXT("\r\nForyouDVD LDH CF :  p[3] = %x  p[4] = %x  p[5] = %x "),p[3],p[4],p[5]));//Add by LDH
		DBGD( (TEXT("\r\nForyouDVD LDH CF TotalTitle = %x TotalChar = %x"),pForyouDVDInfo->sForyouDVDInfo.TotalTitle,pForyouDVDInfo->sForyouDVDInfo.TotalChar));//Add by LDH
		returnDVDTotalTitleTrack(pForyouDVDInfo,
		pForyouDVDInfo->sForyouDVDInfo.TotalTitle,
		pForyouDVDInfo->sForyouDVDInfo.TotalChar);
	}

	//DBGD( (TEXT("\r\nForyouDVD LDH CF:  %x %x %x %x %x %x "),p[0],p[1],p[2],p[3],p[4],p[5]));//Add by LDH
	
}

void transFlyaudioInfoDA(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{	
	DBGD( (TEXT("\r\nForyouDVD LDH DA :  p[0] = %x"),p[0]));//Add by LDH
	if(pForyouDVDInfo->sForyouDVDInfo.PlayMode != p[0])
	{
		pForyouDVDInfo->sForyouDVDInfo.PlayMode = p[0];//播放模式
		returnDVDPlayMode(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.PlayMode);
	}
}

void transFlyaudioInfoEB(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{
	pForyouDVDInfo->sForyouDVDInfo.bRecFileFolderUseEB = TRUE;
	if (0x00 == p[0])//开始
	{
		listFileFolderNewAll(pForyouDVDInfo,TRUE,FLYAUDIO_DVD_FILE_MAX);//File
		listFileFolderNewAll(pForyouDVDInfo,FALSE,FLYAUDIO_DVD_FOLDER_MAX);//Folder
		pForyouDVDInfo->sForyouDVDInfo.pFileCount = 0;
		pForyouDVDInfo->sForyouDVDInfo.pFolderCount = 0;

		pForyouDVDInfo->sForyouDVDInfo.pBReqFolderFileCommandActiveNow = TRUE;

		pForyouDVDInfo->sForyouDVDInfo.pBStartGetFolderFile = TRUE;

		pForyouDVDInfo->sForyouDVDInfo.bFinishGetFileFolderEB = FALSE;
	}
	else if (0x01 == p[0])//结束
	{
		pForyouDVDInfo->sForyouDVDInfo.bFinishGetFileFolderEB = TRUE;
	}
}

void transFlyaudioInfoE8(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{
	pForyouDVDInfo->sForyouDVDInfo.pReturnE8 = TRUE;
	pForyouDVDInfo->sForyouDVDInfo.pFileCount = p[2]*256 + p[3];
	pForyouDVDInfo->sForyouDVDInfo.pFolderCount = p[0]*256 + p[1];

	DBGD( (TEXT("\r\nFileCount:%d,FolderCount:%d")
		,pForyouDVDInfo->sForyouDVDInfo.pFileCount
		,pForyouDVDInfo->sForyouDVDInfo.pFolderCount));

	listFileFolderNewAll(pForyouDVDInfo,TRUE,pForyouDVDInfo->sForyouDVDInfo.pFileCount);
	listFileFolderNewAll(pForyouDVDInfo,FALSE,pForyouDVDInfo->sForyouDVDInfo.pFolderCount);

	pForyouDVDInfo->sForyouDVDInfo.pBReqFolderFileCommandActiveNow = TRUE;

	pForyouDVDInfo->sForyouDVDInfo.pBStartGetFolderFile = TRUE;

	pForyouDVDInfo->sForyouDVDInfo.bFinishGetFileFolderEB = TRUE;

	SetEvent(pForyouDVDInfo->hDispatchThreadForyouDVDEvent);
}

void transFlyaudioInfoEA(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{
	UINT i;
	if (p[4])
	{
		DBGD( (TEXT("\r\nFile:%d,Gi:%d,Pi:%d,Nl:%d"),p[4],p[0]*256 + p[1],p[2]*256 + p[3],len-6));
	}
	else
	{
		DBGD( (TEXT("\r\nFolder:%d,Gi:%d,Pi:%d,Nl:%d"),p[4],p[0]*256 + p[1],p[2]*256 + p[3],len-6));
	}
	DBGD( (TEXT("\r\nName")));
	for (i = 6;i < len;i++)
	{
		DBGD( (TEXT(" %x"),p[i]));
	}
	if (pForyouDVDInfo->sForyouDVDInfo.pBStartGetFolderFile || pForyouDVDInfo->sForyouDVDInfo.pBHaveGetFolderFile)
	{
		pForyouDVDInfo->sForyouDVDInfo.pBStartGetFolderFile = FALSE;
		pForyouDVDInfo->sForyouDVDInfo.pBHaveGetFolderFile = TRUE;

		if (p[4])
		{
			if (pForyouDVDInfo->sForyouDVDInfo.bRecFileFolderUseEB)
			{
				if (pForyouDVDInfo->sForyouDVDInfo.pFileCount < p[0]*256 + p[1] + 1)
				{
					pForyouDVDInfo->sForyouDVDInfo.pFileCount = p[0]*256 + p[1] + 1;
				}
				if (pForyouDVDInfo->sForyouDVDInfo.pFileCount > FLYAUDIO_DVD_FILE_MAX)
				{
					pForyouDVDInfo->sForyouDVDInfo.pFileCount = FLYAUDIO_DVD_FILE_MAX;
				}
			}
			listFileFolderStorageOne(pForyouDVDInfo,TRUE,p[0]*256 + p[1],p[2]*256 + p[3],p[5],&p[6],len-6);
		}
		else
		{
			if (pForyouDVDInfo->sForyouDVDInfo.bRecFileFolderUseEB)
			{
				if (pForyouDVDInfo->sForyouDVDInfo.pFolderCount < p[0]*256 + p[1] + 1)
				{
					pForyouDVDInfo->sForyouDVDInfo.pFolderCount = p[0]*256 + p[1] + 1;
				}
				if (pForyouDVDInfo->sForyouDVDInfo.pFolderCount > FLYAUDIO_DVD_FOLDER_MAX)
				{
					pForyouDVDInfo->sForyouDVDInfo.pFolderCount = FLYAUDIO_DVD_FOLDER_MAX;
				}
			}
			listFileFolderStorageOne(pForyouDVDInfo,FALSE,p[0]*256 + p[1],p[2]*256 + p[3],p[5],&p[6],len-6);
		}

		pForyouDVDInfo->sForyouDVDInfo.pBFolderFileListFolderErrorCheck = TRUE;
		pForyouDVDInfo->sForyouDVDInfo.pBFolderFileListFileErrorCheck = TRUE;
	}
	else
	{
		DBGD( (TEXT("\r\nFile And Folder List Not Init")));
	}

	SetEvent(pForyouDVDInfo->hDispatchThreadForyouDVDEvent);
}

void transFlyaudioInfoE0(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len)
{
	UINT highLightIndex = p[0]*256+p[1];
	DBGD( (TEXT("\r\nHigh Light:%d"),highLightIndex));
	if (pForyouDVDInfo->sForyouDVDInfo.pNowReturnPlayingFileCount != highLightIndex)
	{
		pForyouDVDInfo->sForyouDVDInfo.pNowReturnPlayingFileCount = highLightIndex;
		if (pForyouDVDInfo->sForyouDVDInfo.pBGetFileFolderFinish)
		{
			procDVDInfoE0(pForyouDVDInfo);
		}
		else
		{
			pForyouDVDInfo->sForyouDVDInfo.bRecE0AndNeedProc = TRUE;
			DBGD( (TEXT("\r\nForyouDVD Rec E0 But Haven't Finish Rec File Or Folder Info")));
		}
	}
}




void DealFlyaudioInfo(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT length)
{
	pForyouDVDInfo->iAutoResetControlTime = GetTickCount();//从操作系统启动到现在经过的时间...

	switch (p[0])
	{
	case 0x2B:
		returnDVDDeviceActionState(pForyouDVDInfo,0,0x03);
		break;

	case 0xDB:
		if (2 == length && 0x00 == p[1])
		{
			pForyouDVDInfo->bFlyaudioDVD = TRUE;//凌阳DVD
		}
		else
		{
			pForyouDVDInfo->bFlyaudioDVD = FALSE;//华阳DVD
		}
		pForyouDVDInfo->sForyouDVDInfo.MechanismInitialize = TRUE;//DVD初始化完成
		structDVDInfoInit(pForyouDVDInfo,FALSE);

		returnDVDDeviceWorkMode(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.MechanismInitialize);
		SetEvent(pForyouDVDInfo->hDispatchThreadForyouDVDEvent);
		break;

	case 0xCB:
		transFlyaudioInfoCB(pForyouDVDInfo,&p[1],length-1);
		break;
	case 0xCC:
		transFlyaudioInfoCC(pForyouDVDInfo,&p[1],length-1);
		break;
	case 0xCD:
		transFlyaudioInfoCD(pForyouDVDInfo,&p[1],length-1);
		break;
	case 0xCE:
		transFlyaudioInfoCE(pForyouDVDInfo,&p[1],length-1);
		break;
	case 0xCF:
		transFlyaudioInfoCF(pForyouDVDInfo,&p[1],length-1);
		break;
	case 0XDA:
		transFlyaudioInfoDA(pForyouDVDInfo,&p[1],length-1);
		break;
	case 0xE8:
		transFlyaudioInfoE8(pForyouDVDInfo,&p[1],length-1);
		break;
	case 0xEA:
		transFlyaudioInfoEA(pForyouDVDInfo,&p[1],length-1);
		break;
	case 0xEB:
		transFlyaudioInfoEB(pForyouDVDInfo,&p[1],length-1);
		break;
	case 0xE0:
		transFlyaudioInfoE0(pForyouDVDInfo,&p[1],length-1);
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
		break;

	case 0x40:
		returnFlyAudioDVDInfo(pForyouDVDInfo,&p[1],length-1);
		break;

	default:
		DBGD( (TEXT("unHandle!")));
		break;
	}
}

BYTE FlyaudioDVDReqStepChangeAndReq(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE DVDReqStep)
{
	UINT i,j;
	BOOL bCheck = FALSE;
	do 
	{
		DBGD( (TEXT("\r\nStep %d"),DVDReqStep));

		if (pForyouDVDInfo->sForyouDVDInfo.pBGetFolderFinish && pForyouDVDInfo->sForyouDVDInfo.pBGetFileFinish)
		{
			if (FALSE == pForyouDVDInfo->sForyouDVDInfo.pBGetFileFolderFinish)
			{
				DBGD( (TEXT("\r\nFlyAudio FolderFileList Start Return!")));
				pForyouDVDInfo->sForyouDVDInfo.pBStartGetFolderFile = FALSE;
				pForyouDVDInfo->sForyouDVDInfo.pBHaveGetFolderFile = FALSE;
				pForyouDVDInfo->sForyouDVDInfo.pBGetFileFolderFinish = TRUE;

				pForyouDVDInfo->sForyouDVDInfo.bFinishGetFileFolderEB = FALSE;

				UINT isFolderCount = getSelectParentFolderFileCount(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder,TRUE);
				UINT isFileCount = getSelectParentFolderFileCount(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder,FALSE);
				returnDVDCurrentFolderInfo(pForyouDVDInfo,isFolderCount+isFileCount,isFolderCount,TRUE);

				if (pForyouDVDInfo->sForyouDVDInfo.bRecE0AndNeedProc)
				{
					procDVDInfoE0(pForyouDVDInfo);
				}
			}
		}

		if (0 == DVDReqStep)
		{
			DVDReqStep = 1;
			if (pForyouDVDInfo->sForyouDVDInfo.pBFolderFileListFolderErrorCheck && pForyouDVDInfo->sForyouDVDInfo.bFinishGetFileFolderEB)
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
						DBGD( (TEXT("<%d>"),DVDReqStep));
						DBGD( (TEXT("\r\nFlyAudio FolderList has error!Start:%d,Count:%d"),i,j-i));
						return DVDReqStep;
					}
				}
				if (i == pForyouDVDInfo->sForyouDVDInfo.pFolderCount)
				{
					DBGD( (TEXT("\r\nFlyAudio FolderList check OK!")));
						pForyouDVDInfo->sForyouDVDInfo.pBFolderFileListFolderErrorCheck = FALSE;
					pForyouDVDInfo->sForyouDVDInfo.pBGetFolderFinish = TRUE;
				}
			}
		}
		if (1 == DVDReqStep)
		{
			DVDReqStep = 2;
			if (pForyouDVDInfo->sForyouDVDInfo.pBFolderFileListFileErrorCheck && pForyouDVDInfo->sForyouDVDInfo.bFinishGetFileFolderEB)
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
						DBGD( (TEXT("<%d>"),DVDReqStep));
						DBGD( (TEXT("\r\nFlyAudio FileList has error!Start:%d,Count:%d"),i,j-i));
						return DVDReqStep;
					}
				}
				if (i == pForyouDVDInfo->sForyouDVDInfo.pFileCount)
				{
					DBGD( (TEXT("\r\nFlyAudio FileList check OK!")));
						pForyouDVDInfo->sForyouDVDInfo.pBFolderFileListFileErrorCheck = FALSE;
					pForyouDVDInfo->sForyouDVDInfo.pBGetFileFinish = TRUE;
				}
			}
		}
		if (2 == DVDReqStep)//查询碟机程序版本号，确保有一个放在最后一直发送
		{
			DVDReqStep = 0;
			//if (0x00 == pForyouDVDInfo->sForyouDVDInfo.iSoftwareVersion[0])
			//{
			control_DVD_ReqDVDSoftwareVersion(pForyouDVDInfo);
			DBGD( (TEXT("<%d>"),DVDReqStep));
				return DVDReqStep;
			//}
		}
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

void processFlyaudioDVD(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,ULONG WaitReturn)
{

	if (IpcWhatEvent(EVENT_GLOBAL_DVD_STOP_ID))
	{
		IpcClearEvent(EVENT_GLOBAL_DVD_STOP_ID);

		////不在DVD通道时，返回loading状态，
		//if (pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eAudioInput != MediaCD)
		//{
		//	returnDVDDeviceActionState(pForyouDVDInfo,
		//		pForyouDVDInfo->sForyouDVDInfo.CurrentMechanismDevice,0x05);
		//}
	}


	if (pForyouDVDInfo->hDVDComm && pForyouDVDInfo->hDVDComm != INVALID_HANDLE_VALUE)//串口OK
	{
		if (pForyouDVDInfo->bPower)
		{
			if (!pForyouDVDInfo->bPowerUp)
			{
				pForyouDVDInfo->bPowerUp = TRUE;
				DVD_LEDControl_Off(pForyouDVDInfo);

				if (!pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)
				{
					//DVD_Reset_On(pForyouDVDInfo);
					//Sleep(100);
					//DVD_Reset_Off(pForyouDVDInfo);
				}
			}
		}

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
			Sleep(100);
			if (0x00 == pForyouDVDInfo->sForyouDVDInfo.curVideoAspect)
			{
				control_DVD_Set_View_Mode(pForyouDVDInfo,0x04);
				Sleep(1000);
				control_DVD_Video_Aspect_Radio(pForyouDVDInfo,0x01);
				Sleep(1000);
			}
			else if (0x01 == pForyouDVDInfo->sForyouDVDInfo.curVideoAspect)
			{
				control_DVD_Set_View_Mode(pForyouDVDInfo,0x04);
				Sleep(1000);
				control_DVD_Video_Aspect_Radio(pForyouDVDInfo,0x00);
				Sleep(1000);
			}
			else if (0x02 == pForyouDVDInfo->sForyouDVDInfo.curVideoAspect)
			{
				control_DVD_Set_View_Mode(pForyouDVDInfo,0x00);
				Sleep(1000);
				//control_DVD_Video_Aspect_Radio(pForyouDVDInfo,0x01);
				//Sleep(1000);
			}

			//control_DVD_IR_CMD(pForyouDVDInfo,0x14);
		}

		if (pForyouDVDInfo->sForyouDVDInfo.pBGetFileFolderFinish)//已完成接收
		{
			if (pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount != pForyouDVDInfo->sForyouDVDInfo.pNeedReturnCount)//需要返回
			{
				UINT isFolderCount = getSelectParentFolderFileCount(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder,TRUE);
				UINT isFileCount = getSelectParentFolderFileCount(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder,FALSE);
				UINT isWhere;
				DBGD( (TEXT("\r\nFlyAudio ThreadFlyForyouDVDProc Need Return In:%d TotalFolder:%d TotalFile:%d"),pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder,isFolderCount,isFileCount));
					while (pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount != pForyouDVDInfo->sForyouDVDInfo.pNeedReturnCount)//需要返回
					{
						DBGD( (TEXT(" More")));
							if ((pForyouDVDInfo->sForyouDVDInfo.pNeedReturnStart + pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount) < isFolderCount)//返回文件夹
							{
								isWhere = getSelectParentFolderFileInfoBySubIndex(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.pNowInWhatFolder,TRUE
									,pForyouDVDInfo->sForyouDVDInfo.pNeedReturnStart + pForyouDVDInfo->sForyouDVDInfo.pNowReturnCount);
								DBGD( (TEXT(" Folder:%d"),isWhere));
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
								DBGD( (TEXT(" File:%d"),isWhere));
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

		if (WAIT_TIMEOUT == WaitReturn)//如果超时退出
		{
			pForyouDVDInfo->sForyouDVDInfo.DVDReqStep = FlyaudioDVDReqStepChangeAndReq(pForyouDVDInfo,pForyouDVDInfo->sForyouDVDInfo.DVDReqStep);
		}
	}
}