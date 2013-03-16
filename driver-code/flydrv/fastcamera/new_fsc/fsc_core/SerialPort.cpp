#include "stdafx.h"
#include "SerialPort.h"
#include<stdio.h>
#include <assert.h>

//
// Constructor
//
CSerialPort::CSerialPort()
{

	m_hComDev = NULL;
	memset(&m_osWrite,0,sizeof(OVERLAPPED));
	memset(&m_osRead,0,sizeof(OVERLAPPED));
	memset(&m_osCommEvent,0,sizeof(OVERLAPPED));
	m_fOpen = FALSE;
}

//
// Delete dynamic memory
//
CSerialPort::~CSerialPort()
{

  CloseHandle(m_hComDev);
}

BOOL  CSerialPort::InitPort(UINT portnr, UINT i4userBaudRate)
{

	DCB dcb;
	COMMTIMEOUTS commTimeOuts;
	TCHAR szPort[10];
	//====================================================
	m_osRead.Offset=0;
	m_osRead.OffsetHigh=0;
	m_osWrite.Offset=0;
	m_osWrite.OffsetHigh=0;
	m_osRead.hEvent = CreateEvent(NULL,TRUE/*bManualReset*/,FALSE,NULL);
	//manual reset event object should be used. 
	//So, system can make the event objecte nonsignalled.
	//osRead.hEvent & osWrite.hEvent may be used to check the completion of 
	// WriteFile() & ReadFile().



	if(m_osRead.hEvent==NULL)
	{
		NKDbgPrintfW(TEXT("[ERROR:CreateEvent for osRead.]\n"));
		return FALSE;
	}

	m_osWrite.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	if(m_osWrite.hEvent==NULL)
	{
		NKDbgPrintfW(TEXT("[ERROR:CreateEvent for osWrite.]\n"));
		return FALSE;
	}

	// prepare port strings
	swprintf(szPort, L"COM%d:", portnr);

	//====================================================
	m_hComDev=CreateFile(szPort,
		GENERIC_READ|GENERIC_WRITE,
		0, //exclusive access
		NULL,	
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if(m_hComDev==INVALID_HANDLE_VALUE)
	{
		NKDbgPrintfW(TEXT("[ERROR:CreateFile for opening COM port.]\n") );
		return FALSE;
	}

	SetCommMask(m_hComDev,EV_RXCHAR);
	SetupComm(m_hComDev,1024*1024,1024*1024);
	PurgeComm(m_hComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

	commTimeOuts.ReadIntervalTimeout=0xffffffff;
	commTimeOuts.ReadTotalTimeoutMultiplier=0;
	commTimeOuts.ReadTotalTimeoutConstant=1000*1000;
	commTimeOuts.WriteTotalTimeoutMultiplier=0;
	commTimeOuts.WriteTotalTimeoutConstant=1000;
	SetCommTimeouts(m_hComDev,&commTimeOuts);

	//====================================================
	dcb.DCBlength=sizeof(DCB);
	GetCommState(m_hComDev,&dcb);

	dcb.fBinary=TRUE;
	dcb.fParity=FALSE;
	dcb.BaudRate=i4userBaudRate;
	dcb.ByteSize=8;
	dcb.Parity=0;
	dcb.StopBits=0;
	dcb.fDtrControl=DTR_CONTROL_DISABLE;
	dcb.fRtsControl=RTS_CONTROL_DISABLE;
	dcb.fOutxCtsFlow=0;
	dcb.fOutxDsrFlow=0;

	if(SetCommState(m_hComDev,&dcb)==TRUE)
	{	

		m_osCommEvent.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
		SetCommMask(m_hComDev,EV_RXCHAR|EV_TXEMPTY|EV_ERR);

		return TRUE;
	}
	else
	{

		CloseHandle(m_hComDev);
		return FALSE;
	}


}
BOOL   CSerialPort::ClearPort()
{

   PurgeComm(m_hComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
   return TRUE;
}



DWORD CSerialPort::ReadCommBlockData(VOID *buf,DWORD  dwBytetoRead)
{	

	BOOL fReadStat;
	DWORD dwLengthofRead;
	DWORD dwEvtMask = 0;
	DWORD dwTotalRead;
	
	

    dwTotalRead = 0;
	while(dwBytetoRead)
	{
		fReadStat=ReadFile(m_hComDev,buf,dwBytetoRead,&dwLengthofRead,NULL);
		if(!fReadStat)   
		{
			RETAILMSG(TRUE, (TEXT("[BackCarAPP] Uart Read Failed\r\n")));
			return 0;
		}
		dwBytetoRead -= dwLengthofRead;
        dwTotalRead +=dwLengthofRead;
		buf = (BYTE *)buf + dwLengthofRead;

	}
	return dwTotalRead;
}
DWORD  CSerialPort::WriteCommBlockData(VOID *pData,DWORD dwByteToWrite)
{
	DWORD temp = 0;
	BOOL fWriteStat;
	while(dwByteToWrite)
	{
		fWriteStat = WriteFile(m_hComDev,pData,dwByteToWrite,&temp,NULL);

		if(!fWriteStat)
			return 0;

		//printf("Write %d Byte To COM port\r\n",temp);
		dwByteToWrite -=temp;
		pData = (BYTE *)pData + temp;
	}
	return temp;
}

