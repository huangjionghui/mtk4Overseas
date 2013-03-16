

#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

class CSerialPort
{														 
public:
	// contruction and destruction
	CSerialPort();
	virtual		~CSerialPort();

	// port initialisation											
	BOOL   InitPort(UINT portnr = 1, UINT baud = 115200);
	DWORD  ReadCommBlockData(VOID *buf,DWORD  dwBytetoRead);
	DWORD  WriteCommBlockData(VOID *pData,DWORD dwByteToWrite);
	DWORD  ReadCommBlockData(VOID *buf,DWORD  dwBytetoRead,DWORD dwTimeout);
	BOOL   ClearPort();

protected:
	DWORD  m_dwBaudRate ;
	HANDLE m_hComDev;
	OVERLAPPED m_osWrite;
	OVERLAPPED  m_osRead;
	OVERLAPPED m_osCommEvent;
	UINT				m_nPortNr;
	BOOL  m_fOpen;
};

#endif __SERIALPORT_H__