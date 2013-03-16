#pragma once

#include "FlyGlobal.h"

//0`100, ȡ11��
#define COLOR_STEP_COUNT 11
const BYTE sColor_N[COLOR_STEP_COUNT]={0,15,35,55,75,85,87,89,92,97,100};
const BYTE sHue_N[COLOR_STEP_COUNT]={20,30,40,45,50,55,60,65,70,75,80};

const BYTE sContrast_Video[COLOR_STEP_COUNT]={0,5,7,9,13,20,22,25,30,35,40};
const BYTE sContrast_Def[COLOR_STEP_COUNT]={0,10,20,25,30,35,45,50,55,60,65};

const BYTE sBrightness_AVIN[COLOR_STEP_COUNT]={28,29,30,31,32,33,36,38,39,40,42};
const BYTE sBrightness_Def[COLOR_STEP_COUNT]={35,36,37,38,39,40,43,44,48,50,55};

#define SET_CONTRAST_DEFAUT   25
#define SET_BRIGHTNESS_DEFAUT 48


struct video_t{

	//��ʼ�����ϵ����
	BOOL  bInit;		//��ʼ��
	BOOL  bPowerOn;		//�ϵ�
	BOOL  bPowerUp;		//����
	BOOL  bPowerResume;	//����

	UINT16 iCurVideoChannel;
	UINT16 iCurAudioChannel;
	BOOL   bAuxVideoSignal;

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