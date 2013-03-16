
#pragma once

#include "FlyGlobal.h"

//��ѯAUX�Ƿ����ź������¼�
#define TO_VIDEO_SIGNAL_TEST_EVENT_NAME L"TO_VIDEO_SIGNAL_TEST_EVENT_NAME"

//0`100, ȡ11��
#define COLOR_STEP_COUNT 11
const BYTE sColor_N[COLOR_STEP_COUNT]={0,10,20,30,40,50,60,70,80,90,100};
const BYTE sContrast_N[COLOR_STEP_COUNT]={0,10,20,30,40,50,60,70,80,90,100};	
const BYTE sBrightness_N[COLOR_STEP_COUNT]={20,25,30,40,45,46,48,50,52,54,56};
const BYTE sHue_N[COLOR_STEP_COUNT]={0,10,20,30,40,50,60,70,80,90,100};

struct video_t{

	//��ʼ�����ϵ����
	BOOL  bInit;		//��ʼ��
	BOOL  bPowerOn;		//�ϵ�
	BOOL  bPowerUp;		//����
	BOOL  bPowerResume;	//����

	BOOL bVideoKillMainThreadFlag;
	HANDLE hVideoMainThead;
	HANDLE hVideoMainEvent;

	BYTE iIsHaveAUXVideoSignal;
	UINT16 iCurVideoChannel;

	//����API�ӿڣ�һ��Ҫ�����
	FlyGlobal_T sApiTable;
};

// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� VIDEO_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// VIDEO_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef VIDEO_EXPORTS
#define VIDEO_API __declspec(dllexport)
#else
#define VIDEO_API __declspec(dllimport)
#endif

//����������
extern "C"
{
	VIDEO_API FlyGlobal_T*GetDllProcAddr(void);
};


#define FLY_DEBUG 1
#define DEBUG_VIDIEO 1
#if FLY_DEBUG
#define DBGE(string) RETAILMSG(1,string)//��Ҫ����
#define DBGW(string) if(DEBUG_VIDIEO) RETAILMSG(1,string)//����
#define DBGI(string) RETAILMSG(1,string)//��Ϣ
#define DBGD(string) if(DEBUG_VIDIEO) RETAILMSG(1,string) //������Ϣ
#else
#define DBGE(string) RETAILMSG(1,string)//��Ҫ����
#define DBGW(string) DEBUGMSG(1,string) //����
#define DBGI(string) DEBUGMSG(1,string) //��Ϣ
#define DBGD(string) DEBUGMSG(1,string) //������Ϣ
#endif