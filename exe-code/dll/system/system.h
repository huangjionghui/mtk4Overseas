#pragma once

#include "FlyGlobal.h"

struct system_t{

	//��ʼ�����ϵ����
	BOOL  bInit;		//��ʼ��
	BOOL  bPowerOn;		//�ϵ�
	BOOL  bPowerUp;		//����
	BOOL  bPowerResume;	//����

	//����API�ӿڣ�һ��Ҫ�����
	FlyGlobal_T sApiTable;
};

// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� SYSTEM_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// SYSTEM_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef SYSTEM_EXPORTS
#define SYSTEM_API __declspec(dllexport)
#else
#define SYSTEM_API __declspec(dllimport)
#endif
//����������
extern "C"
{
	SYSTEM_API FlyGlobal_T*GetDllProcAddr(void);
};