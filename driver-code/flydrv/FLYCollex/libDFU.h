// libDFU.h : main header file for the libDFU DLL
//

#pragma once

//#ifndef __AFXWIN_H__
//	#error "include 'stdafx.h' before including this file for PCH"
//#endif

//#include "resource.h"

// ClibDFUApp
// See libDFU.cpp for the implementation of this class
//

//class ClibDFUApp : public CWinApp
//{
//public:
//	ClibDFUApp();
//
//// Overrides
//public:
//	virtual BOOL InitInstance();
//
//	DECLARE_MESSAGE_MAP()
//};

enum {
	DFU_SUCCESS = 0,
	DFU_ERROR_SETUP_UART,
	DFU_ERROR_DFU_FILE,
	DFU_ERROR_EXECUTION
};

extern "C" __declspec(dllexport) UINT BtDFU(CString port, CString FilePath, unsigned int initBaudRate, unsigned int speedBaudRate, unsigned short BufferSize);
extern "C" __declspec(dllexport) UINT GetProgress(void);
extern "C" __declspec(dllexport) void GetDFUError(UINT* CurStage, UINT* CurEvent, UINT* ErrCode, UINT* AdvErrCode);
