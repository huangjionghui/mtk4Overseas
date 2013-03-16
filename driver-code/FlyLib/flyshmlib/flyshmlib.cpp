/*ģ�����ƣ�flyshmlib.cpp
**ģ�鹦�ܣ������ڴ棬�¼���Ӧ��ز���
**
**�� �� �ˣ��ƾ���
**�޸����ڣ�2012-09-01
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

/*�������ƣ�GetShareMmapAddr
 *����������iSize���ڴ��С
 *�������ܣ�ȡ�ù����ڴ棬����������������ȡ���ڴ棩
 *�������أ��ɹ�ʱ����TRUE
 *�� �� �ˣ�
 *�޸����ڣ�
 */
BOOL GetShareMmapAddr(UINT32 iSize)
{
	
	GetShareMmapAddrPorc GetProcAddr=NULL;

	//����Ӳ��DLL
	gCurLibPrivateData.mhinst = LoadLibrary(FLY_HW_DLL_PATH);
	if (!gCurLibPrivateData.mhinst){
		RETAILMSG(1, (TEXT("[shm lib] LoadLibrary FlyHwdrv.dll err!\r\n")));
		return FALSE;
	}

	//��audio.dll�ĵ�������GetProgressAddr()ȡ��Ӳ����ĵ�ַ����
	GetProcAddr = (GetShareMmapAddrPorc)GetProcAddress(gCurLibPrivateData.mhinst,L"HWD_CreateShareMmap");
	if (NULL == GetProcAddr)
	{
		RETAILMSG(1,(TEXT("[shm lib] Error! Failed to get FlyHwdrv.dll Proc addr list.\r\n")));
		return	FALSE;
	}

	//���õ���������ȡ�õ�ַ
	gCurLibPrivateData.pCurMemoryMapAddr = (shm_t *)GetProcAddr(iSize);
	if (NULL == gCurLibPrivateData.pCurMemoryMapAddr)
	{
		RETAILMSG(1,(TEXT("[shm lib] Error! Failed to get FlyHwdrv.ll addr table.\r\n")));
		return	FALSE;
	}

	//RETAILMSG(1,(TEXT("[fly hw lib] pHwDrvAddrTable:%p.\r\n"),gCurLibPrivateData.pHwDrvAddrTable));

	return TRUE;
}


/*�������ƣ�GetShmSize
 *����������iSize���ڴ��С
 *�������ܣ�����ڴ�Ĵ�С
 *�������أ�ʵ��Ҫ������ڴ��С
 *�� �� �ˣ�
 *�޸����ڣ�
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

/*�������ƣ�SendEventToMoudle
 *����������iEvent��ĳ��ģ����¼�
 *�������ܣ���ĳ��ģ�鷢���¼�
 *�������أ�
 *�� �� �ˣ�
 *�޸����ڣ�
 */
static void SendEventToMoudle(UINT32 iEvent)
{
	if (gCurLibPrivateData.pCurMemoryMapAddr->bIpcEventTable[iEvent])
	{
		if (gCurLibPrivateData.fCurIpcFun)
			gCurLibPrivateData.fCurIpcFun(iEvent);
	}
}

/*�������ƣ�LoopSendEventToMoudle
 *����������iStart��ĳ��ģ�����ʼ�¼���iEnd��ĳ��ģ��Ľ����¼�
 *�������ܣ�ѭ����ĳ��ģ�鷢���¼�
 *�������أ�
 *�� �� �ˣ�
 *�޸����ڣ�
 */
static void LoopSendEventToMoudle(UINT32 iStart, UINT32 iEnd)
{
	UINT32 i;
	for (i=iStart; i<iEnd; i++)
		SendEventToMoudle(i);
}

/*�������ƣ�IpcExchangeEvent
 *������������
 *�������ܣ��ж��¼���������һ��ģ��ģ������ַ��¼�
 *�������أ�
 *�� �� �ˣ�
 *�޸����ڣ�
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

/*�������ƣ�WaitForIpcEventThread
 *����������
 *�������ܣ��ַ��¼����߳�
 *�������أ�
 *�� �� �ˣ�
 *�޸����ڣ�
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

/*�������ƣ�DealDriverEventByMoudleID
**����������bCreate:�Ƿ��ʼ���¼���iID��ģ��ID
**�������ܣ������¼��������¼�
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
*/
BOOL DealDriverEventByMoudleID(BOOL bCreate, BYTE iID)
{
	//����һ
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

	//	//�����¼�
	//	handle =  CreateEvent(NULL, FALSE, TRUE, (LPCWSTR)(LRESULT)sWideString);
	//	if (NULL != handle)
	//	{
	//		SetEvent(handle);
	//		CloseHandle(handle);
	//		handle = NULL;
	//		return TRUE;
	//	}
	//}

	
	//����������֤�Ƿ���LOGO
	if (bCreate)
	{
		gCurLibPrivateData.hCurIpcEventHandle = CreateEvent(NULL, FALSE, TRUE, StrIpcEventNameTable[iID]);	
		return TRUE;
	}
	else
	{
		HANDLE handle = NULL;

		//�����¼�
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

/*�������ƣ�CreateShmThread
 *������������
 *�������ܣ������ַ��¼����߳�
 *�������أ�
 *�� �� �ˣ�
 *�޸����ڣ�
 */
BOOL CreateShmThread(void)
{
	//�����̣߳��ȴ��¼�
	DWORD dwThreadID;
	gCurLibPrivateData.bKillIpcThread = FALSE;
	gCurLibPrivateData.hCurIpcThreadHandle = CreateThread(
		(LPSECURITY_ATTRIBUTES) NULL,					//��ȫ����
		0,												//��ʼ���߳�ջ�Ĵ�С��ȱʡΪ�����̴߳�С��ͬ
		(LPTHREAD_START_ROUTINE)WaitForIpcEventThread,	//�̵߳�ȫ�ֺ���
		&gCurLibPrivateData,							//�˴�����������ܵľ��
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

/*�������ƣ�CreateShm
 *����������eModuleID��ģ��ID�� fIpcFun���¼��Ļص�����
 *�������ܣ����������ڴ�
 *�������أ������õĹ����ڴ�
 *�� �� �ˣ�
 *�޸����ڣ�
 */
shm_t *CreateShm(MODULE_ID_T eModuleID,fIpcFunction fIpcFun)
{
	//���Ҫ�����ڴ�Ĵ�С
	UINT32 iSize = GetShmSize(sizeof(struct shm));
	if (0 == iSize)
		return NULL;

	//�ṹ����0
	memset(&gCurLibPrivateData,0,sizeof(struct ShmCurLibPrivateData));


	//�������������ڴ档
	gCurLibPrivateData.hCurMemoryMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,iSize,_T("flyShm"));
	if (NULL == gCurLibPrivateData.hCurMemoryMap)
	{
		RETAILMSG(1,(TEXT("[lib shm] module ID:%d,mmap addr err!\r\n"),eModuleID));
		return NULL;
	}
	else
	{
		//ӳ�乲���ڴ�
		gCurLibPrivateData.pCurMemoryMapAddr = (shm_t*)MapViewOfFile(
			gCurLibPrivateData.hCurMemoryMap,SECTION_ALL_ACCESS/*FILE_MAP_WRITE|FILE_MAP_READ*/ ,0,0,iSize);
		if (NULL == gCurLibPrivateData.pCurMemoryMapAddr)
		{
			CloseHandle(gCurLibPrivateData.hCurMemoryMap);
			gCurLibPrivateData.hCurMemoryMap = NULL;
			RETAILMSG(1,(TEXT("[lib shm] module ID:%d,can't mmap addr\r\n"),eModuleID));
		}
	}
	
	//������
	//if(!GetShareMmapAddr(iSize))
	//{
	//	RETAILMSG(1,(TEXT("[lib shm] module ID:%d��mmap create err!\r\n")��eModuleID));
	//	return NULL;
	//}

	//ӳ��ɹ�ʱ�������¼��߳�
	if (NULL != gCurLibPrivateData.pCurMemoryMapAddr)
	{
		gCurLibPrivateData.fCurIpcFun   = fIpcFun;
		gCurLibPrivateData.eCurModuleID = eModuleID;

		if (!CreateShmThread())
		{
			FreeShm();
			RETAILMSG(1,(TEXT("[lib shm] module ID:%d��mmap create thread err!\r\n"),eModuleID));
			return NULL;
		}
	}

	return gCurLibPrivateData.pCurMemoryMapAddr;
}


/*�������ƣ�FindModuleEvent
*����������iStart:module��ʼ��iEnd��������iEvent���¼�
*�������ܣ��ҵ�ĳ���¼�Ҫ����ĳ��ģ��
*�������أ�ĳ��ģ��
*�� �� �ˣ�
*�޸����ڣ�
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

/*�������ƣ�FindWhosEvent
 *����������iEvent���¼�
 *�������ܣ��ҵ�ĳ���¼�Ҫ����ĳ��ģ��
 *�������أ�ĳ��ģ��
 *�� �� �ˣ�
 *�޸����ڣ�
 */
static UINT32 FindWhosEvent(UINT32 iEvent)
{
	UINT32 iStart, iEnd;

	//����ӿ�����ٶ�
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

/*�������ƣ�IpcStartEvent
 *����������iEvent���¼�
 *�������ܣ������¼�
 *�������أ�ĳ��ģ��
 *�� �� �ˣ�
 *�޸����ڣ�
 */
void IpcStartEvent(UINT32 iEvent)
{
	assert(NULL != gCurLibPrivateData.pCurMemoryMapAddr);

	if (iEvent >= IPC_EVENT_MAX)
		return;

	UINT32 iWho   = ~0;
	gCurLibPrivateData.pCurMemoryMapAddr->bIpcEventTable[iEvent] = TRUE;

	//�ҳ�����¼��Ǹ�˭��
	iWho = FindWhosEvent(iEvent);
	if (iWho == ~0)
		return;

	//�����¼�
	DealDriverEventByMoudleID(FALSE, iWho);
}

/*�������ƣ�IpcClearEvent
 *����������iEvent���¼�
 *�������ܣ�����¼�
 *�������أ�
 *�� �� �ˣ�
 *�޸����ڣ�
 */
void IpcClearEvent(UINT32 iEvent)
{
	assert(NULL != gCurLibPrivateData.pCurMemoryMapAddr);

	if (iEvent >= IPC_EVENT_MAX)
		return;	

	gCurLibPrivateData.pCurMemoryMapAddr->bIpcEventTable[iEvent] = FALSE;
}


/*�������ƣ�IpcWhatEvent
 *����������iEvent���¼�
 *�������ܣ��ж��ĸ��¼�
 *�������أ�
 *�� �� �ˣ�
 *�޸����ڣ�
 */
BOOL IpcWhatEvent(UINT32 iEvent)
{
	assert(NULL != gCurLibPrivateData.pCurMemoryMapAddr);

	if (iEvent >= IPC_EVENT_MAX)
		return FALSE;

	return gCurLibPrivateData.pCurMemoryMapAddr->bIpcEventTable[iEvent];
}


/*�������ƣ�FreeShm
 *����������
 *�������ܣ��ͷŹ����ڴ�
 *�������أ�
 *�� �� �ˣ�
 *�޸����ڣ�
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

/*�������ƣ�WriteRegValues
 *����������
 *�������ܣ�дע���
 *�������أ�
 *�� �� �ˣ�
 *�޸����ڣ�
 */
BOOL WriteRegValues(HKEY hKeyType, LPCWSTR lpSubKey, LPTSTR lpValueName,void *pData,UINT32 size)
{
	HKEY hKey; 
	LONG regError;
	DWORD dwDisposition;

	//�򿪻򴴽�ע���
	RegCreateKeyEx(hKeyType,
		lpSubKey,0,NULL,0,0,NULL,&hKey,&dwDisposition);

	//д��
	regError = RegSetValueEx(hKey,
		lpValueName,0,REG_DWORD,(LPBYTE)pData,size);
	if (regError != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);	
		return FALSE;
	}

	//�ر�
	RegCloseKey(hKey);	

	return TRUE;
}

/*�������ƣ�ReadRegValues
 *����������
 *�������ܣ���ע���
 *�������أ�
 *�� �� �ˣ�
 *�޸����ڣ�
 */
BOOL ReadRegValues(HKEY hKeyType,LPCWSTR lpSubKey,
					  LPTSTR lpValueName,void *pData,UINT32 size)
{
	HKEY hKey; 
	LONG regError;
	DWORD dwDisposition;
	DWORD   dwDataSize;

	//�򿪻򴴽�ע���
	RegCreateKeyEx(hKeyType,
		lpSubKey,0,NULL,0,0,NULL,&hKey,&dwDisposition);

	//����
	dwDataSize = size;
	regError = RegQueryValueEx(hKey,
		lpValueName, NULL, NULL,(LPBYTE)pData,&dwDataSize);
	if (regError != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);	
		return FALSE;
	}

	//�ر�
	RegCloseKey(hKey);	

	return TRUE;
}

/*�������ƣ�WriteCurRegValues
 *����������
 *�������ܣ�д��ǰ�û�ע���
 *�������أ�
 *�� �� �ˣ�
 *�޸����ڣ�
 */
BOOL WriteCurRegValues(LPCWSTR lpSubKey,
		LPTSTR lpValueName,void *pData,UINT32 size)
{
	HKEY hKey; 
	LONG regError;
	DWORD dwDisposition;

	//�򿪻򴴽�ע���
	RegCreateKeyEx(HKEY_CURRENT_USER,
		lpSubKey,0,NULL,0,0,NULL,&hKey,&dwDisposition);

	//д��
	regError = RegSetValueEx(hKey,
		lpValueName,0,REG_DWORD,(LPBYTE)pData,size);
	if (regError != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);	
		return FALSE;
	}

	//�ر�
	RegCloseKey(hKey);	

	return TRUE;
}

/*�������ƣ�ReadCurRegValues
 *����������
 *�������ܣ�����ǰ�û�ע���
 *�������أ�
 *�� �� �ˣ�
 *�޸����ڣ�
*/
BOOL ReadCurRegValues(LPCWSTR lpSubKey,
		LPTSTR lpValueName,void *pData,UINT32 size)
{
	HKEY hKey; 
	LONG regError;
	DWORD dwDisposition;
	DWORD   dwDataSize;

	//�򿪻򴴽�ע���
	RegCreateKeyEx(HKEY_CURRENT_USER,
		lpSubKey,0,NULL,0,0,NULL,&hKey,&dwDisposition);

	//����
	dwDataSize = size;
	regError = RegQueryValueEx(hKey,
		lpValueName, NULL, NULL,(LPBYTE)pData,&dwDataSize);
	if (regError != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);	
		return FALSE;
	}

	//�ر�
	RegCloseKey(hKey);	

	return TRUE;
}

/*�������ƣ�ReadCurRegValues
*����������
*�������ܣ���ӡ������Ϣ
*�������أ�
*�� �� �ˣ�
*�޸����ڣ�
*/

VOID PrintErrMsg(LPCWSTR str)
{
	/*
	 *FORMAT_MESSAGE_FROM_SYSTEM:�ñ�־����FormatMessage������������Ҫϵͳ����Ĵ��������ַ���
	 *FORMAT_MESSAGE_ALLOCATE_BUFFER:���߸ú���Ϊ���������ı����������㹻����ڴ�顣���ڴ��ľ������lpMsgBuf�����з���
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