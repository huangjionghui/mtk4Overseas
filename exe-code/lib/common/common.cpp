#include <windows.h>
#include "flyshm.h"

//注册表操作
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

UINT32 ForU8ToU32(BYTE *p)
{
	return ((p[3] << 24) + (p[2] << 16) + (p[1] << 8) + p[0]);
}


static void SendMsgToBTMoudle(UINT16 commd, UINT32 prama)
{
	HWND FlyBTWnd = FindWindow(NULL,L"BTDevice");
	if(FlyBTWnd)
	{
		RETAILMSG(1,(TEXT("\r\nSend BTDevice.exe commd:%02X,prama:%02X\r\n"),commd,prama));
		SendMessage(FlyBTWnd,WM_BT_MODULE_STA_MSG,prama,commd);
	}
	else
	{
		RETAILMSG(1,(TEXT("\r\nSend Volume not find BTDevice.exe\r\n")));
	}
}
static void SendMsgToDVDMoudle(UINT16 commd, UINT32 prama)
{
	HWND FlyDVDWnd = FindWindow(NULL,L"DVPDevice");
	if(FlyDVDWnd)
	{
		if (commd != 0x11)
			RETAILMSG(1,(TEXT("\r\nSend DVPDevice.exe commd:%02X,prama:%02X\r\n"),commd,prama));
		else
			RETAILMSG(1,(TEXT("\r\nSend DVPDevice.exe commd:%02X,prama1:%02X,prama2:%02X\r\n"),commd,(prama>>8)&0xFF,prama&0xFF));
		SendMessage(FlyDVDWnd,WM_DVD_MODULE_STA_MSG,prama,commd);
	}
	else
	{
		RETAILMSG(1,(TEXT("\r\nSend Volume not find DVPDevice.exe\r\n")));
	}
}

static void SendMsgToMP3Moudle(UINT16 commd, UINT32 prama)
{
	HWND FlyServiceWnd = FindWindow(NULL,L"MediaService");
	if(FlyServiceWnd)
	{
		RETAILMSG(1,(TEXT("\r\nSend  MediaService.exe commd:%X, prama:%X\r\n"),commd,prama));
		SendMessage(FlyServiceWnd,WM_MP3_MODULE_STA_MSG,prama,commd);
	}
	else
	{
		RETAILMSG(1,(TEXT("\r\nNot find MediaService\r\n")));
	}
}
static void SendMsgToIpodMoudle(UINT16 commd, UINT32 prama)
{
	HWND FlyServiceWnd = FindWindow(NULL,L"IpodService");
	if(FlyServiceWnd)
	{
		RETAILMSG(1,(TEXT("\r\nSend  IpodService.exe commd:%X, prama:%X\r\n"),commd,prama));
		SendMessage(FlyServiceWnd,WM_IPOD_MODULE_STA_MSG,prama,commd);
	}
	else
	{
		RETAILMSG(1,(TEXT("\r\nNot find IpodService.exe\r\n")));
	}
}
static void SendMsgVideoMoudle(UINT16 commd, UINT32 prama)
{
	HWND FlyServiceWnd = FindWindow(NULL,L"FlyServiceExe");
	if(FlyServiceWnd)
	{
		RETAILMSG(1,(TEXT("\r\nSend  FlyServiceExe.exe Cur Audio commd:%X, prama:%X\r\n"),commd,prama));
		SendMessage(FlyServiceWnd,WM_SERICE_EXE_STA_MSG,prama,commd);
	}
	else
	{
		RETAILMSG(1,(TEXT("\r\nNot find FlyServiceExe.exe\r\n")));
	}
}

void SendMsgToModules(BYTE iModules, UINT16 iCommd, UINT32 iPrama)
{
	switch (iModules)
	{
	case MP3_MODULE:
		SendMsgToMP3Moudle(iCommd, iPrama);
		break;

	case DVD_MODULE:
		SendMsgToDVDMoudle(iCommd, iPrama);
		break;

	case BT_MODULE:
		SendMsgToBTMoudle(iCommd, iPrama);
		break;

	case IPOD_MODULE:
		SendMsgToIpodMoudle(iCommd, iPrama);
		break;

	case VIDEO_MODULE:
		SendMsgVideoMoudle(iCommd, iPrama);
		break;

	default:
		break;

	}
}



