#pragma  once

#include <windef.h>
#include <types.h>
#include <afxsock.h>// MFC Ì×½Ó×ÖÀ©Õ¹


BOOL WriteCurRegValues(LPCWSTR lpSubKey,
					   LPTSTR lpValueName,void *pData,UINT32 size);
BOOL ReadCurRegValues(LPCWSTR lpSubKey,
					  LPTSTR lpValueName,void *pData,UINT32 size);

UINT32 ForU8ToU32(BYTE *p);

void SendMsgToModules(BYTE iModules, UINT16 iCommd, UINT32 iPrama);


BOOL SocketWrite(BYTE *buf, UINT16 len);
BOOL CreateClientSocket(UINT32 SockPort);
BOOL CloseSocket(void);