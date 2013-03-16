#pragma once

#include "FlyGlobal.h"

struct system_t{

	//初始化，上电相关
	BOOL  bInit;		//初始化
	BOOL  bPowerOn;		//上电
	BOOL  bPowerUp;		//挂起
	BOOL  bPowerResume;	//唤醒

	//函数API接口，一定要放最后
	FlyGlobal_T sApiTable;
};

// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 SYSTEM_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// SYSTEM_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef SYSTEM_EXPORTS
#define SYSTEM_API __declspec(dllexport)
#else
#define SYSTEM_API __declspec(dllimport)
#endif
//导出函数表
extern "C"
{
	SYSTEM_API FlyGlobal_T*GetDllProcAddr(void);
};