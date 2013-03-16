/************************************************************************/
/* FLYAUDIO KEY                                                     */
/************************************************************************/
#pragma once

#ifndef _FLYAUDIO_FORYOU_H_
#define _FLYAUDIO_FORYOU_H_


#include "flyshmlib.h"
#include <windows.h>
#include <basetsd.h>

#define DVD_VERSION "121026"

#ifdef __cplusplus
extern "C"
{
#endif

#define FORYOU_DVD_RESET_FLYER	0

#define FORYOU_DVD_BUG_FIX	1

#define FORYOU_DVD_USB_SUPPORT	1

#define CHINA_CODE_PAGE	936

#define NAME_COUNT_MAX	256

	typedef struct _FOLDER_FILE_TREE_LIST
	{
		BOOL bStorage;
		BOOL bFolder;
		UINT parentFolderIndex;
		UINT extension;
		BYTE name[NAME_COUNT_MAX];
		UINT nameLength;
	}FOLDER_FILE_TREE_LIST, *P_FOLDER_FILE_TREE_LIST;

	typedef struct _FORYOU_DVD
	{
		BOOL MechanismInitialize;
		ULONG bDeviceRec89;

		UINT DVDReqStep;
		BOOL bForceExecReqStep;

		BOOL LEDFlash;
		BOOL bLEDFlashAble;

		BOOL bFilterDiscInFirstPowerUp;
		BOOL bFilterDiscIn;
		ULONG iFilterDiscInTime;

		BOOL bQuickJumpNext;
		UINT iQuickJumpNextCount;
		UINT iQuickJumpTitlePos;
		ULONG iQuickJumpNextTimer;

		BOOL bStartChangePlayDevice;
		ULONG iStartChangePlayDeviceTime;

		BYTE CurrentMechanismStatus[4];//�����֡�������״̬
		BYTE CurrentMechanismType[4];//��ʵ���ֵ������CurrentMechanismDevice

		BYTE CurrentReqMechanismCircle;

		BYTE MediaDiscType;//���������CD����Ϣ//ý����Ϣ״̬������MP3��Ƭ
		BYTE MediaFileType;
		BYTE MediaVideoInfo;
		BYTE MediaAudioSampleFrequency;
		BYTE MediaAudioCoding;//���������CD����Ϣ

		BYTE preVideoAspect;
		BYTE curVideoAspect;

		UINT CurrentTitle;//���������D2����Ϣ
		UINT CurrentChar;
		UINT TotalTitle;
		UINT TotalChar;
		UINT EscapeHour;
		UINT EscapeMinute;
		UINT EscapeSecond;
		UINT TotalHour;
		UINT TotalMinute;
		UINT TotalSecond;
		BYTE PlayMode;
		BYTE AudioType;
		BYTE PlaySpeed;
		BYTE PlayStatus;
		BYTE DVDRoot;

		BYTE DeviceStatus;//������ȡ���״̬
		BYTE DeviceType;
		BYTE CurrentMechanismDevice;//������ǰ�Ĳ����豸

		BYTE HaveDisc;
		BYTE HaveUSB;
		BYTE HaveSD;//���������D2����Ϣ

		BOOL pBStartGetFolderFile;
		BOOL pBHaveGetFolderFile;
		BYTE pStartGetFolderFileCount;
		BOOL pReturnE8;
		BOOL pReturnEAFolder;
		BOOL pReturnEAFile;
		UINT16 pFolderCount;
		UINT16 pFileCount;
		BOOL bFinishGetFileFolderEB;
		BOOL bRecFileFolderUseEB;
		UINT pRecFolderCount;
		UINT pRecFileCount;
		UINT pLastRecFolderIndex;
		UINT pLastRecFileIndex;
		BOOL pBReqFolderFileCommandActiveNow;
		BOOL pBFolderFileListFolderErrorCheck;
		BOOL pBFolderFileListFileErrorCheck;
		BOOL pBGetFolderFinish;
		BOOL pBGetFileFinish;
		BOOL pBGetFileFolderFinish;
		P_FOLDER_FILE_TREE_LIST pFileTreeList;
		P_FOLDER_FILE_TREE_LIST pFolderTreeList;
		UINT pNowInWhatFolder;//�����ã���ǰ���ĸ�Ŀ¼��
		UINT pNeedReturnStart;//������ʼ
		UINT pNeedReturnCount;//��������
		UINT pNowReturnCount;//�ѷ�������
		UINT pNowReturnPlayingFileCount;
		UINT pNowPlayingInWhatFolder;

		BOOL bRecE0AndNeedProc;
		ULONG bNeedSend8CTime;

		BYTE iDVDReturnRegionCode;

		BYTE iSoftwareVersion[6];

		BYTE bDVDRequestState;
		BYTE bDVDResponseState;
		ULONG iDVDStateCheckTime;

#if FORYOU_DVD_BUG_FIX
		BOOL bNeedReturnNoDiscAfterClose;
		ULONG iNeedReturnNoDiscAfterCloseTime;
#endif
	}FORYOU_DVD, *P_FORYOU_DVD;

#define DATA_BUFF_COMM_LENGTH	10240
#define DATA_BUFF_TOUSER_LENGTH	10240

	typedef struct _FLY_FORYOU_DVD_INFO
	{
		//
		BOOL bOpen;
		BOOL bPower;
		BOOL bPowerUp;
		BOOL bSpecialPower;
		BOOL bSocketConnecting;
		
		BOOL bFlyaudioDVD;
		BOOL bEnterUpdateMode;
		BOOL bDVDDriverInitFirst;
		BOOL bDVDSendInitStatusStart;
		BOOL bDVDAudioChannelFirstInit;

		//Init��ʼ��
		BOOL bKillDispatchForyouDVDThread;
		HANDLE hThreadHandleFlyForyouDVD;
		HANDLE hDispatchThreadForyouDVDEvent;

		BOOL bKillDispatchForyouDVDLEDControlThread;
		HANDLE hThreadHandleFlyForyouDVDLEDControl;
		HANDLE hDispatchThreadForyouDVDLEDControlEvent;

		BYTE buffToUser[DATA_BUFF_TOUSER_LENGTH];
		UINT buffToUserHx;
		UINT buffToUserLx;
		HANDLE hBuffToUserDataEvent;
		CRITICAL_SECTION hCSSendToUser;

		//����restartʱ��ʼ��
		HANDLE hDVDComm;
		BOOL bKillFlyDVDCommThread;
		HANDLE hThreadHandleFlyDVDComm;
		UINT DVDInfoFrameStatus;
		UINT DVDInfoFrameLengthMax;
		UINT DVDInfoFrameLength;
		BYTE DVDInfoFrameChecksum;
		BYTE DVDInfoFrameBuff[DATA_BUFF_COMM_LENGTH];
		
		BYTE DVDInfoFrameCRCErrorTailBuff[256];
		UINT DVDInfoFrameCRCErrorTailLength;

		FORYOU_DVD sForyouDVDInfo;
		BOOL bAutoResetControlOn;
		ULONG iAutoResetControlTime;

		HANDLE hHandleGlobalGlobalEvent;
		//PFLY_DRIVER_GLOBAL pFlyDriverGlobalInfo;
		volatile shm_t *pFlyShmGlobalInfo;
	}FLY_FORYOU_DVD_INFO, *P_FLY_FORYOU_DVD_INFO;

	void DVD_LEDControl_On(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo);
	void DVD_LEDControl_Off(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo);
	void DVD_Reset_On(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo);
	void DVD_Reset_Off(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo);

	void structDVDInfoInit(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bInitAll);

	void control_DVD_ReqFileFolderDetailedInfo(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE bFile,UINT iAbs,UINT iOffset);
	void control_DVD_IR_CMD(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE IRCMD);
	void control_DVD_JumpNextN(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT iJumpN);
	void control_DVD_JumpPrevN(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT iJumpN);
	void control_DVD_ReqDVDSoftwareVersion(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo);

	void control_DVD_Set_View_Mode(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE para);
	void control_DVD_Video_Aspect_Radio(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE para);

	void returnDVDDeviceWorkMode(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE bWork);
	void returnDVDDevicePlayDevice(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iDevice);
	void returnDVDDeviceActionState(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iDevice,BYTE iState);
	void returnDVDDeviceContent(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iContent);
	void returnDVDPlayStatusSpeed(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iPlayStatus,BYTE iPlaySpeed);
	void returnDVDDeviceMedia(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iDisc,BYTE iFile);
	void returnDVDCurrentTime(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iHour,BYTE iMin,BYTE iSec);
	void returnDVDTotalTitleTrack(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT iTitle,UINT iTrack);
	void returnDVDCurrentTitleTrack(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT iTitle,UINT iTrack);
	void returnDVDFileFolderInfo(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bFolder,BYTE *pName,UINT nameLength,UINT index,UINT globalIndex);

	void returnDVDCurrentFolderInfo(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT totalCount,UINT folderCount,BOOL bRoot);
	void returnDVDPlayMode(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE iPlayMode);
	void returnDVDNowPlayingFileInfo(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,P_FOLDER_FILE_TREE_LIST p,UINT index,UINT globalIndex);
	void returnDVDNowPlayingInFolderName(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,P_FOLDER_FILE_TREE_LIST p);

	void DealFlyaudioInfo(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT length);
	void DealDVDInfo(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT length);
	void processFlyaudioDVD(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,ULONG WaitReturn);

	void listFileFolderNewAll(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bFile,UINT iCount);
	void listFileFolderStorageOne(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bFile,UINT iCount,UINT parentIndex,UINT extension,BYTE *name,UINT nameLength);

	UINT selectFolderOrFileByLocalIndex(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT inWhatFolder,UINT localIndex,BOOL bHaveFolderIndex);

	UINT getSelectParentFolderFileInfoBySubIndex(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT parentIndex,BOOL bFolder,UINT subIndex);//����Ŀ¼������ţ�����ȫ�����
	UINT getSelectParentFolderFileCount(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,UINT parentIndex,BOOL bFolder);//����ͬĿ¼���ļ����ļ�������
	UINT getSelectFolderFileIndexByGlobalIndexInParent(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BOOL bFolder,UINT globalIndex);//����ȫ����ţ�����ͬĿ¼�µ������,�˴������Э�����һ��Ŀ¼����й�

	void procDVDInfoE0(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo);


	void returnFlyAudioDVDInfo(P_FLY_FORYOU_DVD_INFO pForyouDVDInfo,BYTE *p,UINT len);

#ifdef __cplusplus
}
#endif


#define MSG_ENABLE_FLAG (pForyouDVDInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_DVD && GLOBAL_DEBUG)
#define DBGE(string) RETAILMSG(1,string)//��Ҫ����
#define DBGW(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string)//����
#define DBGI(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string)//��Ϣ
#define DBGD(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string) //������Ϣ
#else
#define DBGE(string) RETAILMSG(1,string)//��Ҫ����
#define DBGW(string) DEBUGMSG(1,string) //����
#define DBGI(string) DEBUGMSG(1,string) //��Ϣ
#define DBGD(string) DEBUGMSG(1,string) //������Ϣ
#endif


#endif// _FLYAUDIO_FORYOU_H_