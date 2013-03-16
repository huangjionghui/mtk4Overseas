#pragma once
#ifndef __FLY_GLOBAL_H__
#define __FLY_GLOBAL_H__

#include <windef.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	BOOL   (*Init)(void);
	BOOL   (*DeInit)(void);	
	BOOL   (*Write)(BYTE *buf, UINT32 len);
}FlyGlobal_T;


typedef FlyGlobal_T* (*GetDllProcAddr_t)(void);


#ifdef __cplusplus
}
#endif

#endif