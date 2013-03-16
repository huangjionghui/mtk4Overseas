// FlyServiceExeDlg.h : ͷ�ļ�
//

#pragma once
#include "FlyGlobal.h"
#include "flyshm.h"

struct FlyExeInfo{
	HINSTANCE mhinst;
	FlyGlobal_T *pGlobalApiTable;
};
// CFlyServiceExeDlg �Ի���
class CFlyServiceExeDlg : public CDialog
{
// ����
public:
	CFlyServiceExeDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FLYSERVICEEXE_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	afx_msg LRESULT OnMusicProcessIDStateMessage(WPARAM wParam, LPARAM lParam);

public:
	struct FlyExeInfo sFlyExeInfo[FLY_DLL_MODUES_MAX];
	BOOL GetGlobalApiTableAddr(UINT16 iIndex);

	//������Ϣ��DLL
	void SendMsgToDllMouldes(BYTE iModuleID, BYTE *buf, UINT16 len);
	void ControlGPSProcessID(UINT32 iProcessID);
	void ControlMP3ProcessID(UINT32 iProcessID);
	void ControlVideoSigned(BYTE iChannel, BOOL bSigned);
	void ControlDVDInitStatus(BOOL bInit);
	void ControlVideoCurAudioChannel(BYTE iChannel);
	void ControlDVDAudioSource(BOOL bAudioSource);


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
};
