// FlyServiceExeDlg.h : 头文件
//

#pragma once
#include "FlyGlobal.h"
#include "flyshm.h"

struct FlyExeInfo{
	HINSTANCE mhinst;
	FlyGlobal_T *pGlobalApiTable;
};
// CFlyServiceExeDlg 对话框
class CFlyServiceExeDlg : public CDialog
{
// 构造
public:
	CFlyServiceExeDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FLYSERVICEEXE_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	afx_msg LRESULT OnMusicProcessIDStateMessage(WPARAM wParam, LPARAM lParam);

public:
	struct FlyExeInfo sFlyExeInfo[FLY_DLL_MODUES_MAX];
	BOOL GetGlobalApiTableAddr(UINT16 iIndex);

	//发送消息给DLL
	void SendMsgToDllMouldes(BYTE iModuleID, BYTE *buf, UINT16 len);
	void ControlGPSProcessID(UINT32 iProcessID);
	void ControlMP3ProcessID(UINT32 iProcessID);
	void ControlVideoSigned(BYTE iChannel, BOOL bSigned);
	void ControlDVDInitStatus(BOOL bInit);
	void ControlVideoCurAudioChannel(BYTE iChannel);
	void ControlDVDAudioSource(BOOL bAudioSource);


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
};
