/*模块名称：flyseriallib.cpp
**模块功能：串口相关操作
**
**修 改 人：黄炯辉
**修改日期：2012-09-01
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

/*函数名称：InitDCB
**函数参数：
**函数功能：
**函数返回：成功时返回TRUE
**修 改 人：
**修改日期：
**/
static BOOL InitDCB(HANDLE hHandle,DWORD iBaudrate)
{
	DCB PortDCB;

	//改变DCB结构设置，得到端口的默认设置信息
	GetCommState(hHandle, &PortDCB);
	PortDCB.DCBlength = sizeof (DCB);    
	PortDCB.BaudRate = iBaudrate;				//波特率 
	PortDCB.fBinary = TRUE;						//Win32不支持非二进制串行传输模式，必须为TRUE 
	PortDCB.fParity = FALSE;					//启用奇偶校验 
	PortDCB.fOutxCtsFlow = FALSE;				//串行端口的输出由CTS线控制
	PortDCB.fOutxDsrFlow = FALSE;				//关闭串行端口的DSR流控制 
	PortDCB.fDtrControl = DTR_CONTROL_DISABLE;  //启用DTR线
	PortDCB.fDsrSensitivity = FALSE;			//如果设为TRUE将忽略任何输入的字节，除非DSR线被启用 
	//PortDCB.fTXContinueOnXoff = TRUE;			//当为TRUE时，如果接收缓冲区已满且驱动程序已传送XOFF字符，将使驱动程序停止传输字符
	PortDCB.fTXContinueOnXoff = FALSE;
	PortDCB.fOutX = FALSE;						//设为TRUE指定XON/XOFF控制被用于控制串行输出 
	PortDCB.fInX = FALSE;						//设为TRUE指定XON/XOFF控制被用于控制串行输入 
	PortDCB.fErrorChar = FALSE;					//WINCE串行驱动程序的默认执行将忽略这个字段 
	PortDCB.fNull = FALSE;						//设为TRUE将使串行驱动程序忽略收到的空字节 
	PortDCB.fRtsControl = RTS_CONTROL_DISABLE;  //启用RTS线 
	PortDCB.fAbortOnError = FALSE;				//WINCE串行驱动程序的默认执行将忽略这个字段
	PortDCB.ByteSize = 8;						//每字节的位数 
	PortDCB.Parity = NOPARITY;					//无奇偶校验 
	PortDCB.StopBits = ONESTOPBIT;				//每字节一位停止位 

	//根据DCB结构配置端口 
	if (!SetCommState(hHandle, &PortDCB))
	{
		return FALSE;
	}

	return TRUE;
}

/*函数名称：InitCommTimeouts
**函数参数：
**函数功能：
**函数返回：成功时返回TRUE
**修 改 人：
**修改日期：
**/
static BOOL InitCommTimeouts(HANDLE hHandle)
{
	COMMTIMEOUTS CommTimeouts;

	//得到超时参数，改变COMMTIMEOUTS结构设置
	GetCommTimeouts(hHandle, &CommTimeouts);
	CommTimeouts.ReadIntervalTimeout  = MAXDWORD;  
	CommTimeouts.ReadTotalTimeoutMultiplier  = 0;  
	CommTimeouts.ReadTotalTimeoutConstant    = 0;    
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;  
	CommTimeouts.WriteTotalTimeoutConstant   = 0;

	//设置端口超时值 
	if (!SetCommTimeouts(hHandle, &CommTimeouts))
	{
		return FALSE;
	}

	return TRUE;
}

/*函数名称：OpenSerial
**函数参数：strSerial：串口字符串，iBaudrate：波特率
**函数功能：打开串口
**函数返回：
**修 改 人：
**修改日期：
**/
HANDLE OpenSerial(LPCWSTR strSerial, DWORD iBaudrate)
{
	//结构体先清0
	memset(&serial_info,0,sizeof(struct serial_struct));

	//打开串口
	serial_info.hSerialHandle = CreateFile(strSerial, 
		GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(serial_info.hSerialHandle == INVALID_HANDLE_VALUE)
	{
		serial_info.hSerialHandle= NULL;
		return NULL;
	}

	
	SetCommMask(serial_info.hSerialHandle, EV_RXCHAR);					//指定端口监测的事件集
	SetupComm(serial_info.hSerialHandle, 51200, 51200);					//设置接收与发送的缓冲区大小 
	PurgeComm(serial_info.hSerialHandle,PURGE_TXCLEAR|PURGE_RXCLEAR);	//初始化缓冲区中的信息

	//配置串行端口
	if(!InitDCB(serial_info.hSerialHandle,iBaudrate))
		return NULL;

	//设置端口超时值
	if(!InitCommTimeouts(serial_info.hSerialHandle))
		return NULL;

	//设置端口上指定信号的状态
	// SETDTR: 发送DTR (data-terminal-ready)信号
	// SETRTS: 发送RTS (request-to-send)信号
	EscapeCommFunction(serial_info.hSerialHandle, SETDTR);
	EscapeCommFunction(serial_info.hSerialHandle, SETRTS);

	return serial_info.hSerialHandle;
}

/*函数名称：CloseSerial
**函数参数：
**函数功能：关闭串口
**函数返回：
**修 改 人：
**修改日期：
**/
BOOL CloseSerial(HANDLE hHandle)
{
	if (NULL == hHandle)
		return FALSE;

	//清除端口上指定信号的状态
	SetCommMask(hHandle,0);
	EscapeCommFunction(hHandle,CLRDTR);
	EscapeCommFunction(hHandle,CLRRTS);

	//清除驱动程序内部的发送和接收队列
	PurgeComm(hHandle,PURGE_TXCLEAR|PURGE_RXCLEAR);

	//关闭串口
	CloseHandle(hHandle);

	return TRUE;
}

/*函数名称：ReadSerial
**函数参数：
**函数功能：读串口数据
**函数返回：
**修 改 人：
**修改日期：
**/
UINT32 ReadSerial(HANDLE hHandle,BYTE *buf, UINT32 len)
{
	assert(NULL != hHandle);
	assert(NULL != buf);

	if (len == 0){
		RETAILMSG(1,(TEXT("[Serial lib] Read serial len range is zero.\r\n")));
		return 0;
	}

	//等待串口的事件发生
	WaitCommEvent(hHandle,&serial_info.dwCommModemStatus, 0);
	if (serial_info.dwCommModemStatus & EV_RXCHAR) 
	{
		ClearCommError(hHandle,&serial_info.dwErrorFlags,&serial_info.ComStat);
		serial_info.dwLength=serial_info.ComStat.cbInQue;	//cbInQue返回在串行驱动程序输入队列中的字符数
		if(serial_info.dwLength>0)				//串口有数据
		{
			//从串口读取数据
			if(!ReadFile(hHandle,buf,len,&serial_info.dwLength,NULL))
				serial_info.dwLength = 0;//不能从串口读取数据
		}     
	}

	return serial_info.dwLength;
}

/*函数名称：WriteSerial
**函数参数：
**函数功能：写串口数据
**函数返回：
**修 改 人：
**修改日期：
**/
BOOL WriteSerial(HANDLE hHandle,BYTE *buf, UINT32 len)
{
	assert(NULL != hHandle);
	assert(NULL != buf);

	if (len <= 0)
		return FALSE;

	return WriteFile(hHandle,buf,len,&serial_info.bufSendLength,NULL);
}


