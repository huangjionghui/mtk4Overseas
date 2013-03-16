/*ģ�����ƣ�flyseriallib.cpp
**ģ�鹦�ܣ�������ز���
**
**�� �� �ˣ��ƾ���
**�޸����ڣ�2012-09-01
**/
#include <windows.h>
#include <commctrl.h>
#include <assert.h>
#include "flyseriallib.h"

struct serial_struct{
	HANDLE  hSerialHandle;
	DWORD   dwCommModemStatus;
	DWORD   dwLength;
	COMSTAT ComStat;
	DWORD   dwErrorFlags;
	DWORD	bufSendLength;
};

struct serial_struct serial_info;

/*�������ƣ�InitDCB
**����������
**�������ܣ�
**�������أ��ɹ�ʱ����TRUE
**�� �� �ˣ�
**�޸����ڣ�
**/
static BOOL InitDCB(HANDLE hHandle,DWORD iBaudrate)
{
	DCB PortDCB;

	//�ı�DCB�ṹ���ã��õ��˿ڵ�Ĭ��������Ϣ
	GetCommState(hHandle, &PortDCB);
	PortDCB.DCBlength = sizeof (DCB);    
	PortDCB.BaudRate = iBaudrate;				//������ 
	PortDCB.fBinary = TRUE;						//Win32��֧�ַǶ����ƴ��д���ģʽ������ΪTRUE 
	PortDCB.fParity = FALSE;					//������żУ�� 
	PortDCB.fOutxCtsFlow = FALSE;				//���ж˿ڵ������CTS�߿���
	PortDCB.fOutxDsrFlow = FALSE;				//�رմ��ж˿ڵ�DSR������ 
	PortDCB.fDtrControl = DTR_CONTROL_DISABLE;  //����DTR��
	PortDCB.fDsrSensitivity = FALSE;			//�����ΪTRUE�������κ�������ֽڣ�����DSR�߱����� 
	//PortDCB.fTXContinueOnXoff = TRUE;			//��ΪTRUEʱ��������ջ��������������������Ѵ���XOFF�ַ�����ʹ��������ֹͣ�����ַ�
	PortDCB.fTXContinueOnXoff = FALSE;
	PortDCB.fOutX = FALSE;						//��ΪTRUEָ��XON/XOFF���Ʊ����ڿ��ƴ������ 
	PortDCB.fInX = FALSE;						//��ΪTRUEָ��XON/XOFF���Ʊ����ڿ��ƴ������� 
	PortDCB.fErrorChar = FALSE;					//WINCE�������������Ĭ��ִ�н���������ֶ� 
	PortDCB.fNull = FALSE;						//��ΪTRUE��ʹ����������������յ��Ŀ��ֽ� 
	PortDCB.fRtsControl = RTS_CONTROL_DISABLE;  //����RTS�� 
	PortDCB.fAbortOnError = FALSE;				//WINCE�������������Ĭ��ִ�н���������ֶ�
	PortDCB.ByteSize = 8;						//ÿ�ֽڵ�λ�� 
	PortDCB.Parity = NOPARITY;					//����żУ�� 
	PortDCB.StopBits = ONESTOPBIT;				//ÿ�ֽ�һλֹͣλ 

	//����DCB�ṹ���ö˿� 
	if (!SetCommState(hHandle, &PortDCB))
	{
		return FALSE;
	}

	return TRUE;
}

/*�������ƣ�InitCommTimeouts
**����������
**�������ܣ�
**�������أ��ɹ�ʱ����TRUE
**�� �� �ˣ�
**�޸����ڣ�
**/
static BOOL InitCommTimeouts(HANDLE hHandle)
{
	COMMTIMEOUTS CommTimeouts;

	//�õ���ʱ�������ı�COMMTIMEOUTS�ṹ����
	GetCommTimeouts(hHandle, &CommTimeouts);
	CommTimeouts.ReadIntervalTimeout  = MAXDWORD;  
	CommTimeouts.ReadTotalTimeoutMultiplier  = 0;  
	CommTimeouts.ReadTotalTimeoutConstant    = 0;    
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;  
	CommTimeouts.WriteTotalTimeoutConstant   = 0;

	//���ö˿ڳ�ʱֵ 
	if (!SetCommTimeouts(hHandle, &CommTimeouts))
	{
		return FALSE;
	}

	return TRUE;
}

/*�������ƣ�OpenSerial
**����������strSerial�������ַ�����iBaudrate��������
**�������ܣ��򿪴���
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
**/
HANDLE OpenSerial(LPCWSTR strSerial, DWORD iBaudrate)
{
	//�ṹ������0
	memset(&serial_info,0,sizeof(struct serial_struct));

	//�򿪴���
	serial_info.hSerialHandle = CreateFile(strSerial, 
		GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(serial_info.hSerialHandle == INVALID_HANDLE_VALUE)
	{
		serial_info.hSerialHandle= NULL;
		return NULL;
	}

	
	SetCommMask(serial_info.hSerialHandle, EV_RXCHAR);					//ָ���˿ڼ����¼���
	SetupComm(serial_info.hSerialHandle, 51200, 51200);					//���ý����뷢�͵Ļ�������С 
	PurgeComm(serial_info.hSerialHandle,PURGE_TXCLEAR|PURGE_RXCLEAR);	//��ʼ���������е���Ϣ

	//���ô��ж˿�
	if(!InitDCB(serial_info.hSerialHandle,iBaudrate))
		return NULL;

	//���ö˿ڳ�ʱֵ
	if(!InitCommTimeouts(serial_info.hSerialHandle))
		return NULL;

	//���ö˿���ָ���źŵ�״̬
	// SETDTR: ����DTR (data-terminal-ready)�ź�
	// SETRTS: ����RTS (request-to-send)�ź�
	EscapeCommFunction(serial_info.hSerialHandle, SETDTR);
	EscapeCommFunction(serial_info.hSerialHandle, SETRTS);

	return serial_info.hSerialHandle;
}

/*�������ƣ�CloseSerial
**����������
**�������ܣ��رմ���
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
**/
BOOL CloseSerial(HANDLE hHandle)
{
	if (NULL == hHandle)
		return FALSE;

	//����˿���ָ���źŵ�״̬
	SetCommMask(hHandle,0);
	EscapeCommFunction(hHandle,CLRDTR);
	EscapeCommFunction(hHandle,CLRRTS);

	//������������ڲ��ķ��ͺͽ��ն���
	PurgeComm(hHandle,PURGE_TXCLEAR|PURGE_RXCLEAR);

	//�رմ���
	CloseHandle(hHandle);

	return TRUE;
}

/*�������ƣ�ReadSerial
**����������
**�������ܣ�����������
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
**/
UINT32 ReadSerial(HANDLE hHandle,BYTE *buf, UINT32 len)
{
	assert(NULL != hHandle);
	assert(NULL != buf);

	if (len == 0){
		RETAILMSG(1,(TEXT("[Serial lib] Read serial len range is zero.\r\n")));
		return 0;
	}

	//�ȴ����ڵ��¼�����
	WaitCommEvent(hHandle,&serial_info.dwCommModemStatus, 0);
	if (serial_info.dwCommModemStatus & EV_RXCHAR) 
	{
		ClearCommError(hHandle,&serial_info.dwErrorFlags,&serial_info.ComStat);
		serial_info.dwLength=serial_info.ComStat.cbInQue;	//cbInQue�����ڴ�������������������е��ַ���
		if(serial_info.dwLength>0)				//����������
		{
			//�Ӵ��ڶ�ȡ����
			if(!ReadFile(hHandle,buf,len,&serial_info.dwLength,NULL))
				serial_info.dwLength = 0;//���ܴӴ��ڶ�ȡ����
		}     
	}

	return serial_info.dwLength;
}

/*�������ƣ�WriteSerial
**����������
**�������ܣ�д��������
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
**/
BOOL WriteSerial(HANDLE hHandle,BYTE *buf, UINT32 len)
{
	assert(NULL != hHandle);
	assert(NULL != buf);

	if (len <= 0)
		return FALSE;

	return WriteFile(hHandle,buf,len,&serial_info.bufSendLength,NULL);
}


