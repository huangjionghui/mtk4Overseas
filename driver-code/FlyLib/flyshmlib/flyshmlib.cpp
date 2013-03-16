/*模块名称：flyshmlib.cpp
**模块功能：共享内存，事件响应相关操作
**
**修 改 人：黄炯辉
**修改日期：2012-09-01
**/
#include <windows.h>
#include <commctrl.h>
#include <assert.h>
#include <WinError.h>
#include "flyshmlib.h"
#include <basetsd.h>

#define SIZE_PAGE     4096

struct ShmCurLibPrivateData{
	shm_t* 	pCurMemoryMapAddr;
	HANDLE	hCurMemoryMap;

	fIpcFunction fCurIpcFun;
	HANDLE  hCurIpcThreadHandle;
	HANDLE  hCurIpcEventHandle;
	MODULE_ID_T eCurModuleID;
	BOOL  bKillIpcThread;
	UINT32 hDeviceContext;

	HINSTANCE mhinst;
};	

struct ShmCurLibPrivateData gCurLibPrivateData;

/*函数名称：GetShareMmapAddr
 *函数参数：iSize：内存大小
 *函数功能：取得共享内存，方法二（从驱动中取得内存）
 *函数返回：成功时返回TRUE
 *修 改 人：
 *修改日期：
 */
BOOL GetShareMmapAddr(UINT32 iSize)
{
	
	GetShareMmapAddrPorc GetProcAddr=NULL;

	//加载硬件DLL
	gCurLibPrivateData.mhinst = LoadLibrary(FLY_HW_DLL_PATH);
	if (!gCurLibPrivateData.mhinst){
		RETAILMSG(1, (TEXT("[shm lib] LoadLibrary FlyHwdrv.dll err!\r\n")));
		return FALSE;
	}

	//从audio.dll的导出函数GetProgressAddr()取得硬件表的地址函数
	GetProcAddr = (GetShareMmapAddrPorc)GetProcAddress(gCurLibPrivateData.mhinst,L"HWD_CreateShareMmap");
	if (NULL == GetProcAddr)
	{
		RETAILMSG(1,(TEXT("[shm lib] Error! Failed to get FlyHwdrv.dll Proc addr list.\r\n")));
		return	FALSE;
	}

	//调用导出函数，取得地址
	gCurLibPrivateData.pCurMemoryMapAddr = (shm_t *)GetProcAddr(iSize);
	if (NULL == gCurLibPrivateData.pCurMemoryMapAddr)
	{
		RETAILMSG(1,(TEXT("[shm lib] Error! Failed to get FlyHwdrv.ll addr table.\r\n")));
		return	FALSE;
	}

	//RETAILMSG(1,(TEXT("[fly hw lib] pHwDrvAddrTable:%p.\r\n"),gCurLibPrivateData.pHwDrvAddrTable));

	return TRUE;
}


/*函数名称：GetShmSize
 *函数参数：iSize：内存大小
 *函数功能：获得内存的大小
 *函数返回：实际要分配的内存大小
 *修 改 人：
 *修改日期：
 */
static UINT32 GetShmSize(UINT32 iSize)
{
	UINT16 ret = 0;

	if (iSize < SIZE_PAGE)
		ret = SIZE_PAGE;
	else if (iSize >= SIZE_PAGE  && iSize < 2*SIZE_PAGE)
		ret = 2*SIZE_PAGE;
	else if (iSize >= 2*SIZE_PAGE && iSize < 3*SIZE_PAGE)
		ret = 3*SIZE_PAGE;
	else if (iSize >= 3*SIZE_PAGE && iSize < 4*SIZE_PAGE)
		ret = 4*SIZE_PAGE;
	else
		ret = 0;

	return ret;
}

/*函数名称：SendEventToMoudle
 *函数参数：iEvent：某个模块的事件
 *函数功能：向某个模块发送事件
 *函数返回：
 *修 改 人：
 *修改日期：
 */
static void SendEventToMoudle(UINT32 iEvent)
{
	if (gCurLibPrivateData.pCurMemoryMapAddr->bIpcEventTable[iEvent])
	{
		if (gCurLibPrivateData.fCurIpcFun)
			gCurLibPrivateData.fCurIpcFun(iEvent);
	}
}

/*函数名称：LoopSendEventToMoudle
 *函数参数：iStart：某个模块的起始事件，iEnd：某个模块的结束事件
 *函数功能：循环向某个模块发送事件
 *函数返回：
 *修 改 人：
 *修改日期：
 */
static void LoopSendEventToMoudle(UINT32 iStart, UINT32 iEnd)
{
	UINT32 i;
	for (i=iStart; i<iEnd; i++)
		SendEventToMoudle(i);
}

/*函数名称：IpcExchangeEvent
 *函数参数：无
 *函数功能：判断事件是属于哪一个模块的，用来分发事件
 *函数返回：
 *修 改 人：
 *修改日期：
 */
static void IpcExchangeEvent(void)
{
	assert(NULL != gCurLibPrivateData.pCurMemoryMapAddr);

	if (gCurLibPrivateData.eCurModuleID >= MODULE_MAX)
		return;

	UINT32 iStart,iEnd;

	if (gCurLibPrivateData.eCurModuleID == 0)
		iStart = 0;
	else 
		iStart = ModuleEventTable[gCurLibPrivateData.eCurModuleID-1];

	iEnd = ModuleEventTable[gCurLibPrivateData.eCurModuleID];
	LoopSendEventToMoudle(iStart,iEnd);
}

/*函数名称：WaitForIpcEventThread
 *函数参数：
 *函数功能：分发事件的线程
 *函数返回：
 *修 改 人：
 *修改日期：
 */
void WaitForIpcEventThread(LPVOID pContext)
{
	CeSetThreadPriority(gCurLibPrivateData.hCurIpcThreadHandle, 150);
	while (!gCurLibPrivateData.bKillIpcThread)
	{
		WaitForSingleObject(gCurLibPrivateData.hCurIpcEventHandle, INFINITE);
		{
			IpcExchangeEvent();
		}
	}
}

/*函数名称：DealDriverEventByMoudleID
**函数参数：bCreate:是否初始化事件，iID：模块ID
**函数功能：创建事件并处理事件
**函数返回：
**修 改 人：
**修改日期：
*/
BOOL DealDriverEventByMoudleID(BOOL bCreate, BYTE iID)
{
	//方法一
	//char module_id[sizeof("DRIVERS_EVENT_MODULE_ID_") + sizeof(BYTE)*2];
	//LPCWSTR sWideString[sizeof(module_id)*2];
	//sprintf(module_id,"DRIVERS_EVENT_MODULE_ID_%02X", iID);
	//MultiByteToWideChar(CP_ACP, 0, module_id, sizeof(module_id), (WCHAR*)sWideString, sizeof(sWideString));

	//if (bCreate)
	//{
	//	gCurLibPrivateData.hCurIpcEventHandle = CreateEvent(NULL, FALSE, TRUE, (LPCWSTR)(LRESULT)sWideString);	
	//	return TRUE;
	//}
	//else
	//{
	//	HANDLE handle = NULL;

	//	//发送事件
	//	handle =  CreateEvent(NULL, FALSE, TRUE, (LPCWSTR)(LRESULT)sWideString);
	//	if (NULL != handle)
	//	{
	//		SetEvent(handle);
	//		CloseHandle(handle);
	//		handle = NULL;
	//		return TRUE;
	//	}
	//}

	
	//方法二，验证是否死LOGO
	if (bCreate)
	{
		gCurLibPrivateData.hCurIpcEventHandle = CreateEvent(NULL, FALSE, TRUE, StrIpcEventNameTable[iID]);	
		return TRUE;
	}
	else
	{
		HANDLE handle = NULL;

		//发送事件
		handle =  CreateEvent(NULL, FALSE, TRUE, StrIpcEventNameTable[iID]);
		if (NULL != handle)
		{
			SetEvent(handle);
			CloseHandle(handle);
			handle = NULL;
			return TRUE;
		}
	}

	return FALSE;
}

/*函数名称：CreateShmThread
 *函数参数：无
 *函数功能：创建分发事件的线程
 *函数返回：
 *修 改 人：
 *修改日期：
 */
BOOL CreateShmThread(void)
{
	//创建线程，等待事件
	DWORD dwThreadID;
	gCurLibPrivateData.bKillIpcThread = FALSE;
	gCurLibPrivateData.hCurIpcThreadHandle = CreateThread(
		(LPSECURITY_ATTRIBUTES) NULL,					//安全属性
		0,												//初始化线程栈的大小，缺省为与主线程大小相同
		(LPTHREAD_START_ROUTINE)WaitForIpcEventThread,	//线程的全局函数
		&gCurLibPrivateData,							//此处传入了主框架的句柄
		0, &dwThreadID );
	if (NULL == gCurLibPrivateData.hCurIpcThreadHandle)
	{
		return FALSE;
	}
	
	//CeSetThreadPriority(gCurLibPrivateData.hCurIpcThreadHandle, 150);
	 DealDriverEventByMoudleID(TRUE, gCurLibPrivateData.eCurModuleID);
	//RETAILMSG(1,(TEXT("[lib shm] hIpcEventhHandle:%d\r\n"),gCurLibPrivateData.hCurIpcEventHandle));
	
	return TRUE;
}

/*函数名称：CreateShm
 *函数参数：eModuleID：模块ID， fIpcFun：事件的回调函数
 *函数功能：创建共享内存
 *函数返回：创建好的共享内存
 *修 改 人：
 *修改日期：
 */
shm_t *CreateShm(MODULE_ID_T eModuleID,fIpcFunction fIpcFun)
{
	//获得要创建内存的大小
	UINT32 iSize = GetShmSize(sizeof(struct shm));
	if (0 == iSize)
		return NULL;

	//结构体清0
	memset(&gCurLibPrivateData,0,sizeof(struct ShmCurLibPrivateData));


	//创建命名共享内存。
	gCurLibPrivateData.hCurMemoryMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,iSize,_T("flyShm"));
	if (NULL == gCurLibPrivateData.hCurMemoryMap)
	{
		RETAILMSG(1,(TEXT("[lib shm] module ID:%d,mmap addr err!\r\n"),eModuleID));
		return NULL;
	}
	else
	{
		//映射共享内存
		gCurLibPrivateData.pCurMemoryMapAddr = (shm_t*)MapViewOfFile(
			gCurLibPrivateData.hCurMemoryMap,SECTION_ALL_ACCESS/*FILE_MAP_WRITE|FILE_MAP_READ*/ ,0,0,iSize);
		if (NULL == gCurLibPrivateData.pCurMemoryMapAddr)
		{
			CloseHandle(gCurLibPrivateData.hCurMemoryMap);
			gCurLibPrivateData.hCurMemoryMap = NULL;
			RETAILMSG(1,(TEXT("[lib shm] module ID:%d,can't mmap addr\r\n"),eModuleID));
		}
	}
	
	//方法二
	//if(!GetShareMmapAddr(iSize))
	//{
	//	RETAILMSG(1,(TEXT("[lib shm] module ID:%d，mmap create err!\r\n")，eModuleID));
	//	return NULL;
	//}

	//映射成功时，创建事件线程
	if (NULL != gCurLibPrivateData.pCurMemoryMapAddr)
	{
		gCurLibPrivateData.fCurIpcFun   = fIpcFun;
		gCurLibPrivateData.eCurModuleID = eModuleID;

		if (!CreateShmThread())
		{
			FreeShm();
			RETAILMSG(1,(TEXT("[lib shm] module ID:%d，mmap create thread err!\r\n"),eModuleID));
			return NULL;
		}
	}

	return gCurLibPrivateData.pCurMemoryMapAddr;
}


/*函数名称：FindModuleEvent
*函数参数：iStart:module开始，iEnd：结束，iEvent：事件
*函数功能：找到某个事件要发给某个模块
*函数返回：某个模块
*修 改 人：
*修改日期：
*/
static UINT32 FindModulesEvent(UINT32 iStart, UINT32 iEnd, UINT32 iEvent)
{
	BOOL bFind = FALSE;
	UINT32 i = 0;

	for (i=iStart; i<= iEnd; i++)
	{
		if (iEvent <= ModuleEventTable[i])
		{
			bFind = TRUE;
			break;
		}
	}

	if (bFind)
		return i;
	else 
		return (~0);
}

/*函数名称：FindWhosEvent
 *函数参数：iEvent：事件
 *函数功能：找到某个事件要发给某个模块
 *函数返回：某个模块
 *修 改 人：
 *修改日期：
 */
static UINT32 FindWhosEvent(UINT32 iEvent)
{
	UINT32 iStart, iEnd;

	//这里加快查找速度
	if (iEvent < IPC_KEY_MAX_EVENT) //audio ~ key
	{
		iStart = 0;
		iEnd = 2;
	}
	else if (iEvent < IPC_RADIO_MAX_EVENT) //system ~ radio
	{
		iStart = 2;
		iEnd = 4;
	}
	else //global ~ crarecod
	{
		iStart = 4;
		iEnd = 14;
	}

	return FindModulesEvent(iStart,iEnd,iEvent);
}

/*函数名称：IpcStartEvent
 *函数参数：iEvent：事件
 *函数功能：发送事件
 *函数返回：某个模块
 *修 改 人：
 *修改日期：
 */
void IpcStartEvent(UINT32 iEvent)
{
	assert(NULL != gCurLibPrivateData.pCurMemoryMapAddr);

	if (iEvent >= IPC_EVENT_MAX)
		return;

	UINT32 iWho   = ~0;
	gCurLibPrivateData.pCurMemoryMapAddr->bIpcEventTable[iEvent] = TRUE;

	//找出这个事件是给谁的
	iWho = FindWhosEvent(iEvent);
	if (iWho == ~0)
		return;

	//发送事件
	DealDriverEventByMoudleID(FALSE, iWho);
}

/*函数名称：IpcClearEvent
 *函数参数：iEvent：事件
 *函数功能：清除事件
 *函数返回：
 *修 改 人：
 *修改日期：
 */
void IpcClearEvent(UINT32 iEvent)
{
	assert(NULL != gCurLibPrivateData.pCurMemoryMapAddr);

	if (iEvent >= IPC_EVENT_MAX)
		return;	

	gCurLibPrivateData.pCurMemoryMapAddr->bIpcEventTable[iEvent] = FALSE;
}


/*函数名称：IpcWhatEvent
 *函数参数：iEvent：事件
 *函数功能：判断哪个事件
 *函数返回：
 *修 改 人：
 *修改日期：
 */
BOOL IpcWhatEvent(UINT32 iEvent)
{
	assert(NULL != gCurLibPrivateData.pCurMemoryMapAddr);

	if (iEvent >= IPC_EVENT_MAX)
		return FALSE;

	return gCurLibPrivateData.pCurMemoryMapAddr->bIpcEventTable[iEvent];
}


/*函数名称：FreeShm
 *函数参数：
 *函数功能：释放共享内存
 *函数返回：
 *修 改 人：
 *修改日期：
 */
void FreeShm(void)
{
	gCurLibPrivateData.bKillIpcThread = TRUE;
	SetEvent(gCurLibPrivateData.hCurIpcEventHandle);

	if (gCurLibPrivateData.pCurMemoryMapAddr != NULL)
	{
		UnmapViewOfFile(gCurLibPrivateData.pCurMemoryMapAddr);
		gCurLibPrivateData.pCurMemoryMapAddr = NULL;
	}

	if (gCurLibPrivateData.hCurMemoryMap != NULL)
	{
		CloseHandle(gCurLibPrivateData.hCurMemoryMap);
		gCurLibPrivateData.hCurMemoryMap = NULL;
	}

	if (gCurLibPrivateData.hCurIpcThreadHandle)
		CloseHandle(gCurLibPrivateData.hCurIpcThreadHandle);

	if (gCurLibPrivateData.hCurIpcEventHandle)
		CloseHandle(gCurLibPrivateData.hCurIpcEventHandle);

	memset(&gCurLibPrivateData,0,sizeof(struct ShmCurLibPrivateData));
}

/*函数名称：WriteRegValues
 *函数参数：
 *函数功能：写注册表
 *函数返回：
 *修 改 人：
 *修改日期：
 */
BOOL WriteRegValues(HKEY hKeyType, LPCWSTR lpSubKey, LPTSTR lpValueName,void *pData,UINT32 size)
{
	HKEY hKey; 
	LONG regError;
	DWORD dwDisposition;

	//打开或创建注册表
	RegCreateKeyEx(hKeyType,
		lpSubKey,0,NULL,0,0,NULL,&hKey,&dwDisposition);

	//写入
	regError = RegSetValueEx(hKey,
		lpValueName,0,REG_DWORD,(LPBYTE)pData,size);
	if (regError != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);	
		return FALSE;
	}

	//关闭
	RegCloseKey(hKey);	

	return TRUE;
}

/*函数名称：ReadRegValues
 *函数参数：
 *函数功能：读注册表
 *函数返回：
 *修 改 人：
 *修改日期：
 */
BOOL ReadRegValues(HKEY hKeyType,LPCWSTR lpSubKey,
					  LPTSTR lpValueName,void *pData,UINT32 size)
{
	HKEY hKey; 
	LONG regError;
	DWORD dwDisposition;
	DWORD   dwDataSize;

	//打开或创建注册表
	RegCreateKeyEx(hKeyType,
		lpSubKey,0,NULL,0,0,NULL,&hKey,&dwDisposition);

	//读出
	dwDataSize = size;
	regError = RegQueryValueEx(hKey,
		lpValueName, NULL, NULL,(LPBYTE)pData,&dwDataSize);
	if (regError != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);	
		return FALSE;
	}

	//关闭
	RegCloseKey(hKey);	

	return TRUE;
}

/*函数名称：WriteCurRegValues
 *函数参数：
 *函数功能：写当前用户注册表
 *函数返回：
 *修 改 人：
 *修改日期：
 */
BOOL WriteCurRegValues(LPCWSTR lpSubKey,
		LPTSTR lpValueName,void *pData,UINT32 size)
{
	HKEY hKey; 
	LONG regError;
	DWORD dwDisposition;

	//打开或创建注册表
	RegCreateKeyEx(HKEY_CURRENT_USER,
		lpSubKey,0,NULL,0,0,NULL,&hKey,&dwDisposition);

	//写入
	regError = RegSetValueEx(hKey,
		lpValueName,0,REG_DWORD,(LPBYTE)pData,size);
	if (regError != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);	
		return FALSE;
	}

	//关闭
	RegCloseKey(hKey);	

	return TRUE;
}

/*函数名称：ReadCurRegValues
 *函数参数：
 *函数功能：读当前用户注册表
 *函数返回：
 *修 改 人：
 *修改日期：
*/
BOOL ReadCurRegValues(LPCWSTR lpSubKey,
		LPTSTR lpValueName,void *pData,UINT32 size)
{
	HKEY hKey; 
	LONG regError;
	DWORD dwDisposition;
	DWORD   dwDataSize;

	//打开或创建注册表
	RegCreateKeyEx(HKEY_CURRENT_USER,
		lpSubKey,0,NULL,0,0,NULL,&hKey,&dwDisposition);

	//读出
	dwDataSize = size;
	regError = RegQueryValueEx(hKey,
		lpValueName, NULL, NULL,(LPBYTE)pData,&dwDataSize);
	if (regError != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);	
		return FALSE;
	}

	//关闭
	RegCloseKey(hKey);	

	return TRUE;
}

/*函数名称：ReadCurRegValues
*函数参数：
*函数功能：打印错误信息
*函数返回：
*修 改 人：
*修改日期：
*/

VOID PrintErrMsg(LPCWSTR str)
{
	/*
	 *FORMAT_MESSAGE_FROM_SYSTEM:该标志告诉FormatMessage函数，我们想要系统定义的错误代码的字符串
	 *FORMAT_MESSAGE_ALLOCATE_BUFFER:告诉该函数为错误代码的文本描述分配足够大的内存块。该内存块的句柄将在lpMsgBuf变量中返回
	 */

	LPVOID lpMsgBuf = NULL;
	DWORD  dwErrNum = GetLastError();
	BOOL bFlag = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,dwErrNum,0,(LPTSTR)&lpMsgBuf, 0, NULL);
	if (lpMsgBuf != NULL){
		RETAILMSG(1,(TEXT("\r\n%s -->Get the error code(%d):"),str,dwErrNum));
		RETAILMSG(1,(TEXT("%s\r\n"),lpMsgBuf));
		LocalFree(lpMsgBuf);
		lpMsgBuf = NULL;
	}
	else{
		RETAILMSG(1,(TEXT("\r\n%s -->Not get the error code(%d)\r\n"),str,dwErrNum));
	}
}