// FlyServiceExeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FlyServiceExe.h"
#include "FlyServiceExeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const LPCWSTR FLY_DLL_PATH_TABLE[FLY_DLL_MODUES_MAX]={
	L"system.dll",
	L"audio.dll",
	L"video.dll"
};


// CFlyServiceExeDlg 对话框

CFlyServiceExeDlg::CFlyServiceExeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFlyServiceExeDlg::IDD, pParent)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFlyServiceExeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFlyServiceExeDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	ON_MESSAGE(WM_SERICE_EXE_STA_MSG,OnMusicProcessIDStateMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




BOOL CFlyServiceExeDlg::GetGlobalApiTableAddr(UINT16 iIndex)
{
	GetDllProcAddr_t GetProcAddr=NULL;

	//加载硬件DLL
	sFlyExeInfo[iIndex].mhinst = LoadLibrary(FLY_DLL_PATH_TABLE[iIndex]);
	if (!sFlyExeInfo[iIndex].mhinst){
		RETAILMSG(1, (TEXT("[EXE] LoadLibrary %s err!\r\n"),FLY_DLL_PATH_TABLE[iIndex]));
		return FALSE;
	}

	//从audio.dll的导出函数GetProgressAddr()取得硬件表的地址函数
	GetProcAddr = (GetDllProcAddr_t)GetProcAddress(sFlyExeInfo[iIndex].mhinst,L"GetDllProcAddr");
	if (NULL == GetProcAddr)
	{
		RETAILMSG(1,(TEXT("[EXE] Error! Failed to get %s Proc addr list.\r\n"),FLY_DLL_PATH_TABLE[iIndex]));
		return	FALSE;
	}

	//调用导出函数，取得地址
	sFlyExeInfo[iIndex].pGlobalApiTable = (FlyGlobal_T *)GetProcAddr();
	if (NULL == sFlyExeInfo[iIndex].pGlobalApiTable)
	{
		RETAILMSG(1,(TEXT("[EXE] Error! Failed to get %s addr table.\r\n"),FLY_DLL_PATH_TABLE[iIndex]));
		return	FALSE;
	}

	sFlyExeInfo[iIndex].pGlobalApiTable->Init();

	RETAILMSG(1,(TEXT("[EXE] %s Init OK.\r\n"),FLY_DLL_PATH_TABLE[iIndex]));

	return TRUE;
}

// CFlyServiceExeDlg 消息处理程序

BOOL CFlyServiceExeDlg::OnInitDialog()
{
	UINT16 i=0;

	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	//SetIcon(m_hIcon, TRUE);			// 设置大图标
	//SetIcon(m_hIcon, FALSE);		// 设置小图标

	//发给FlyAudioNavi.exe
	HANDLE hHandle = CreateEvent(NULL,FALSE,FALSE,FLY_SERVICE_EXE_DLL_LOAD_FINISH);

	// TODO: 在此添加额外的初始化代码
	SetWindowPos(NULL,-100,-100,0,0,SW_HIDE);

	for (i=0; i<FLY_DLL_MODUES_MAX; i++)
		GetGlobalApiTableAddr(i);
	
	//告诉FlyAudioNavi.exe，FlyServiceExe.EXE程序已经加载完成
	SetEvent(hHandle);
	CloseHandle(hHandle);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFlyServiceExeDlg::SendMsgToDllMouldes(BYTE iModuleID, BYTE *buf, UINT16 len)
{
	if (iModuleID > FLY_DLL_MODUES_MAX-1)
		return;

	sFlyExeInfo[iModuleID].pGlobalApiTable->Write(buf,len);
}

void CFlyServiceExeDlg::ControlGPSProcessID(UINT32 iProcessID)
{
	BYTE buf[5] = {CMD_GPS_PROCESS_ID,0x00,0x00,0x00,0x00};
	memcpy(&buf[1],(UINT32*)&iProcessID, sizeof(UINT32));
	SendMsgToDllMouldes(AUDIO_DLL_MODULES,buf,5);
	DBGD((TEXT("[FlyServiceExe] GPS ProcessID is :0x%X\r\n"),(UINT32)iProcessID));
}

void CFlyServiceExeDlg::ControlMP3ProcessID(UINT32 iProcessID)
{
	BYTE buf[5] = {CMD_MP3_PROCESS_ID,0x00,0x00,0x00,0x00};
	memcpy(&buf[1],(UINT32*)&iProcessID, sizeof(UINT32));
	SendMsgToDllMouldes(AUDIO_DLL_MODULES,buf,5);
	DBGD((TEXT("[FlyServiceExe] MP3 ProcessID is :0x%X\r\n"),(UINT32)iProcessID));
}

void CFlyServiceExeDlg::ControlVideoSigned(BYTE iChannel, BOOL bSigned)
{
	BYTE buf[3] = {CMD_VIDEO_SIGNED,0x00,0x00};
	buf[1] = iChannel;
	buf[2] = bSigned;

	DBGD((TEXT("[FlyServiceExe] Video:%d,signed:%d\r\n"),iChannel, bSigned));
	SendMsgToDllMouldes(VIDEO_DLL_MODULES,buf,3);
}

void CFlyServiceExeDlg::ControlVideoCurAudioChannel(BYTE iChannel)
{
	BYTE buf[2] = {CMD_CUR_AUDIO_SOURCE,0x00};
	buf[1] = iChannel;

	DBGD((TEXT("[FlyServiceExe] cur Audio channel:%d\r\n"),iChannel));
	SendMsgToDllMouldes(VIDEO_DLL_MODULES,buf,2);
}
void CFlyServiceExeDlg::ControlDVDInitStatus(BOOL bInit)
{
	BYTE buf[2] = {CMD_DVD_INIT_STATUS,0x00};
	if (bInit)
		buf[1] = 0x01;

	DBGD((TEXT("[FlyServiceExe] DVD init Status:%d\r\n"), bInit));
	SendMsgToDllMouldes(AUDIO_DLL_MODULES,buf,2);
}
void CFlyServiceExeDlg::ControlDVDAudioSource(BOOL bAudioSource)
{
	BYTE buf[2] = {CMD_DVD_AUDIO_SOURCE,0x00};
	buf[1] = bAudioSource;

	DBGD((TEXT("[FlyServiceExe] DVD Audio source :%d\r\n"), bAudioSource));
	SendMsgToDllMouldes(AUDIO_DLL_MODULES,buf,2);
}
LRESULT CFlyServiceExeDlg::OnMusicProcessIDStateMessage(WPARAM wParam, LPARAM lParam)
{
	switch ((UINT32)lParam)
	{
	case CMD_GPS_PROCESS_ID:
		ControlGPSProcessID((UINT32)wParam);
		break;

	case CMD_MP3_PROCESS_ID:
		ControlMP3ProcessID((UINT32)wParam);
		break;

	case CMD_VIDEO_SIGNED:
		ControlVideoSigned(wParam&0xFF, (wParam>>8)&0xFF);
		break;

	case CMD_DVD_INIT_STATUS:
		ControlDVDInitStatus((BOOL)wParam);
		break;

	case CMD_DVD_AUDIO_SOURCE:
		ControlDVDAudioSource((BOOL)wParam);
		break;

	case CMD_CUR_AUDIO_SOURCE:
		ControlVideoCurAudioChannel((BYTE)wParam);
		break;

	default:
		break;
	}

	return 0;
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CFlyServiceExeDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_FLYSERVICEEXE_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_FLYSERVICEEXE_DIALOG));
	}
}
#endif

