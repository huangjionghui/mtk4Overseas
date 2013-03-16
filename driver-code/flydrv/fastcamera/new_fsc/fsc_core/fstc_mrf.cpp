#include "stdafx.h"
#include <windows.h>
#include "mrf.h"


extern "C"
void*   GetRCObjectMemAddr(HANDLE hMrf,RCOBJECT *pObjInfo)
{


    return (void* )((BYTE *)hMrf + pObjInfo->u4OffsetData);
     
}

extern "C"
BOOL GetWavResnfo(HANDLE hMrf,DWORD dwID,WAVOBJINFO *pInfo)
{
	RCOBJECT *pObjInfo;
	LPVOID lpBuf = (LPVOID)hMrf;
    MRFHEADER  *pMrfHeader = (MRFHEADER *)lpBuf;
	
	pObjInfo = (RCOBJECT *)((BYTE *)lpBuf + pMrfHeader->u4OffsetData);

    do
	{
         if(pObjInfo->u4FileID == dwID)
         	{
         	    memcpy(pInfo,pObjInfo,sizeof(WAVOBJINFO));
			  	return TRUE;
         	}
		 	
		pObjInfo = (RCOBJECT *)((BYTE *)lpBuf + pObjInfo->u4OffsetNextImage);
	}while(pObjInfo  != lpBuf);

   return FALSE;
   
}

extern "C"
BOOL GetBitmapInfo(HANDLE hMrf,DWORD dwID,BITMAPOBJINFO *pInfo)
{
	BITMAPOBJINFO *pBitmapInfo;
	LPVOID lpBuf = (LPVOID)hMrf;
    MRFHEADER  *pMrfHeader = (MRFHEADER *)lpBuf;
	
	pBitmapInfo = (BITMAPOBJINFO *)((BYTE *)lpBuf + pMrfHeader->u4OffsetData);

    do
	{
         if(pBitmapInfo->u4FileID == dwID)
         	{
         	  memcpy(pInfo,pBitmapInfo,sizeof(BITMAPOBJINFO));
			  	return TRUE;
         	}
		 	
		pBitmapInfo = (BITMAPOBJINFO *)((BYTE *)lpBuf + pBitmapInfo->u4OffsetNextImage);
	}while(pBitmapInfo  != lpBuf);



   return FALSE;
      
}


extern "C"
HANDLE LoadMRF()
{
	DWORD dwFileSize = 0;
    LPVOID lpBuf = NULL;
	DWORD dwBytesRead = 0;
	
	
	HANDLE hdstFile = ::CreateFile(L"\\windows\\BackCar.mrf", GENERIC_READ, 
		FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hdstFile == INVALID_HANDLE_VALUE)
	{
	
		
		RETAILMSG(TRUE, (TEXT("open file failed\r\n")));
		return NULL;
	}

    dwFileSize = GetFileSize(hdstFile,&dwFileSize);
	
	RETAILMSG(TRUE, (TEXT("File Size: %d\r\n"),dwFileSize));


    if(dwFileSize > 0)
     lpBuf = malloc(dwFileSize);

	if(NULL == lpBuf)
	{
		
		RETAILMSG(TRUE, (TEXT("malloc memory failed\r\n")));

		CloseHandle(hdstFile);
		return NULL;
	}
	ReadFile(hdstFile,lpBuf,dwFileSize,&dwBytesRead,NULL);


    CloseHandle(hdstFile);
	return (HANDLE)lpBuf;
	
}




