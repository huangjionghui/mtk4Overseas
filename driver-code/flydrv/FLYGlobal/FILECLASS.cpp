#include <windows.h>
#include <stdio.h>
#include "FLYGlobal.h"
#include "FILECLASS.h"

NDFlash::NDFlash()
{
	HANDLE H_FLASH = INVALID_HANDLE_VALUE;
}

//��nandflash������
BOOL NDFlash::OpenFlashFile(void)
{
	H_FLASH = CreateFile(_T("NDF1:"),GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0); 

	if(H_FLASH == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	return TRUE;
}
//��ȡ���ݳ���
unsigned int NDFlash::GetDatalength(unsigned int type) 
{

	// TODO: Add your control notification handler code here
	BOOL iRet = FALSE;
	DWORD length=0;
	if(H_FLASH == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	Sleep(50);
	iRet = DeviceIoControl(H_FLASH,type,NULL,0,NULL,0,&length,NULL);
	if(iRet == FALSE)
	{
		return 0;
	}
	return length;

}

//��ѯ�����豸
BOOL NDFlash::InqureDevice(unsigned int type) 
{

	// TODO: Add your control notification handler code here
	BOOL iRet = FALSE;
	DWORD length=0;
	if(H_FLASH == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	iRet = DeviceIoControl(H_FLASH,type,NULL,0,NULL,0,&length,NULL);
	return iRet;

}

//��ȡ����
BOOL NDFlash::ReadData(unsigned int type,unsigned char *pbuf,unsigned int size) 
{

	// TODO: Add your control notification handler code here
	BOOL iRet = FALSE;
	DWORD length=0;
	if(H_FLASH == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	Sleep(100);
	iRet = DeviceIoControl(H_FLASH,type,NULL,0,(LPVOID)pbuf,size,&length,NULL);
	if(iRet == FALSE)
	{
		Sleep(300);
		iRet = DeviceIoControl(H_FLASH,type,NULL,0,(LPVOID)pbuf,size,&length,NULL);
		if(iRet == FALSE)
			return 0;
	}

	return iRet;
}

//д����
BOOL NDFlash::WriteData(unsigned int type,unsigned char *pbuf,unsigned int size) 
{

	// TODO: Add your control notification handler code here
	BOOL iRet = FALSE;
	DWORD length=0;
	if(H_FLASH == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	iRet = DeviceIoControl(H_FLASH,type,(LPVOID)pbuf,size,NULL,0,&length,NULL);
	if(iRet == FALSE)
	{
		return 0;
	}

	return iRet;
}

//д����
void NDFlash::CloseFlashFile(void)
{
	CloseHandle(H_FLASH);
}