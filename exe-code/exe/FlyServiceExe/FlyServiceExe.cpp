// FlyServiceExe.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "FlyServiceExe.h"
#include "FlyServiceExeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFlyServiceExeApp

BEGIN_MESSAGE_MAP(CFlyServiceExeApp, CWinApp)
END_MESSAGE_MAP()


// CFlyServiceExeApp ����
CFlyServiceExeApp::CFlyServiceExeApp()
	: CWinApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CFlyServiceExeApp ����
CFlyServiceExeApp theApp;

// CFlyServiceExeApp ��ʼ��

BOOL CFlyServiceExeApp::InitInstance()
{

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CFlyServiceExeDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˴����ô����ʱ�á�ȷ�������ر�
		//  �Ի���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
