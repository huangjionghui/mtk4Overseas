#pragma once

#include "FlyGlobal.h"

//0`100, 取11格
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

	//初始化，上电相关
	BOOL  bInit;		//初始化
	BOOL  bPowerOn;		//上电
	BOOL  bPowerUp;		//挂起
	BOOL  bPowerResume;	//唤醒

	UINT16 iCurVideoChannel;
	UINT16 iCurAudioChannel;
	BOOL   bAuxVideoSignal;

	//函数API接口，一定要放最后
	FlyGlobal_T sApiTable;
};

// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 VIDEO_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// VIDEO_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef VIDEO_EXPORTS
#define VIDEO_API __declspec(dllexport)
#else
#define VIDEO_API __declspec(dllimport)
#endif

//导出函数表
extern "C"
{
	VIDEO_API FlyGlobal_T*GetDllProcAddr(void);
};