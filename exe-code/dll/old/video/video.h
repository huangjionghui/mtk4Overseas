
#pragma once

#include "FlyGlobal.h"

//查询AUX是否有信号命名事件
#define TO_VIDEO_SIGNAL_TEST_EVENT_NAME L"TO_VIDEO_SIGNAL_TEST_EVENT_NAME"

//0`100, 取11格
#define COLOR_STEP_COUNT 11
const BYTE sColor_N[COLOR_STEP_COUNT]={0,10,20,30,40,50,60,70,80,90,100};
const BYTE sContrast_N[COLOR_STEP_COUNT]={0,10,20,30,40,50,60,70,80,90,100};	
const BYTE sBrightness_N[COLOR_STEP_COUNT]={20,25,30,40,45,46,48,50,52,54,56};
const BYTE sHue_N[COLOR_STEP_COUNT]={0,10,20,30,40,50,60,70,80,90,100};

struct video_t{

	//初始化，上电相关
	BOOL  bInit;		//初始化
	BOOL  bPowerOn;		//上电
	BOOL  bPowerUp;		//挂起
	BOOL  bPowerResume;	//唤醒

	BOOL bVideoKillMainThreadFlag;
	HANDLE hVideoMainThead;
	HANDLE hVideoMainEvent;

	BYTE iIsHaveAUXVideoSignal;
	UINT16 iCurVideoChannel;

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


#define FLY_DEBUG 1
#define DEBUG_VIDIEO 1
#if FLY_DEBUG
#define DBGE(string) RETAILMSG(1,string)//重要错误
#define DBGW(string) if(DEBUG_VIDIEO) RETAILMSG(1,string)//警告
#define DBGI(string) RETAILMSG(1,string)//信息
#define DBGD(string) if(DEBUG_VIDIEO) RETAILMSG(1,string) //调试信息
#else
#define DBGE(string) RETAILMSG(1,string)//重要错误
#define DBGW(string) DEBUGMSG(1,string) //警告
#define DBGI(string) DEBUGMSG(1,string) //信息
#define DBGD(string) DEBUGMSG(1,string) //调试信息
#endif