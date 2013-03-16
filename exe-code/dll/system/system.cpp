// system.cpp : 定义 DLL 应用程序的入口点。
//

#include "system.h"
#include <windows.h>
#include <commctrl.h>


#include "SystemApi.h"
#include "common.h"
#include "flyshm.h"

#include <assert.h>


struct system_t gSystemInfo;


void DealDataStreams(BYTE *buf, UINT16 len)
{
	UINT16 i=0;
	DBGD((TEXT("\r\nEXE-System Socket Read %d bytes:"),len));
	for (i=0; i<len; i++)
		DBGD((TEXT("%02X "),buf[i]));

	UINT32 temp=0;
	struct system_t *pSystemInfo = &gSystemInfo;

	switch (buf[0])
	{
	case 0x10:
		if (buf[1] < 11)
			setBacklight(buf[1]);
		break;

	default:
		break;
	}
}


static BOOL InitAudioStruct(struct system_t *pSystemInfo, BOOL bInit)
{
	assert(NULL != pSystemInfo);

	if (bInit)
	{
		memset(pSystemInfo,0,
			sizeof(struct system_t)-sizeof(FlyGlobal_T));
	}

	return TRUE;
}


static BOOL Init(void)
{
	struct system_t *pSystemInfo = &gSystemInfo;

	//初始化audio info结构体
	if(!InitAudioStruct(pSystemInfo,TRUE))
	{
		DBGE((TEXT("[System DLL] init struct err.\r\n")));
		return FALSE;
	}

	if (!CreateClientSocket(9980))
	{
		DBGE((TEXT("[System DLL] audio create client socket err.\r\n")));
		return FALSE;
	}

	pSystemInfo->bInit = TRUE;

	BYTE buf[2] = {0x01,0x01};
	SocketWrite(buf,2);

	DBGI((TEXT("\r\n[System DLL] Init OK time:")));
	DBGI((TEXT(__TIME__)));
	DBGI((TEXT(" data:")));
	DBGI((TEXT(__DATE__)));
	DBGI((TEXT("\r\n")));

	return TRUE;
}

static BOOL  Deinit(void)
{
	struct system_t *pSystemInfo = &gSystemInfo;


	pSystemInfo->bInit = FALSE;
	return TRUE;	
}

static BOOL Write(BYTE *buf, UINT32 len)
{
	switch (buf[0])
	{

	default:
		break;
	}


	return TRUE;
}

static FlyGlobal_T* RegisterApiTable(struct system_t *pSystemInfo)
{
	assert(NULL != pSystemInfo);

	pSystemInfo->sApiTable.Init   = Init;
	pSystemInfo->sApiTable.DeInit = Deinit;
	pSystemInfo->sApiTable.Write  = Write;
	//RETAILMSG(1,(TEXT("DLL pGlobalTale:%p.\r\n"),pGlobalTale));

	return &pSystemInfo->sApiTable;
}

//导出涵数
SYSTEM_API FlyGlobal_T *GetDllProcAddr(void)
{
	return RegisterApiTable(&gSystemInfo);
}
