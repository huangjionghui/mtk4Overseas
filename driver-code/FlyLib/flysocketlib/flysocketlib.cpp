/*模块名称：flysocketlib.cpp
**模块功能：套接字相关操作
**
**修 改 人：黄炯辉
**修改日期：2012-09-01
**/
#include "flysocketlib.h"
#include <assert.h>

#define SOCKET_BUFF_MAX 400
#define SYNC_MISSING 0
#define SYNC_FOUND   1
#define SYNC_PARTIAL 2

struct ServerSocketInfo{
	
	SOCKET sockComm;
	SOCKET sockSrv;
	SOCKADDR_IN addrClient;
	int len;

	BOOL bKillThread;
	HANDLE hThreadHandle;
	HANDLE hListerThreadHandle;

	BOOL bLoopLister;
	UINT32 SockPort;

	fSockRecvFunction fSockRecvFun;
	

	//socket buff
	BYTE cSocketReadBuffer[SOCKET_BUFF_MAX];
	UINT16 iSocketReadBufferLen;
	BYTE cSocketWriteBuffer[SOCKET_BUFF_MAX];
	UINT16 iSocketWriteBufferLen;
	CRITICAL_SECTION cCriticalSectionWrite;//写时加锁
};

struct ServerSocketInfo gSrvSocketInfo;

//是否循环监听
static void ControlLoopListerEnable(BOOL bEnale)
{
	gSrvSocketInfo.bLoopLister = bEnale;
}


/*函数名称：extractPacket
**函数参数：search_sync_ptr：数据流的开始地址
**函数功能：提取一个完整的数据流
**函数返回：返回没有被提取完的数据流开始地址
**修 改 人：
**修改日期：
**/
static BYTE *extractPacket(BYTE *search_sync_ptr)
{
	UINT16 length = 0;
	BYTE buff[SOCKET_BUFF_MAX];

	length = *(search_sync_ptr + 2);
	memcpy(buff, search_sync_ptr + 3, length);

	gSrvSocketInfo.fSockRecvFun(buff,length);
	search_sync_ptr = search_sync_ptr + length + 2 + 1;
	return search_sync_ptr;
}

/*函数名称：searchSync
**函数参数：search_sync_ptr：数据流的开始地址，search_end_ptr：数据流的结束地址，sync_status：标志
**函数功能：找到一个数据流的包头
**函数返回：返回数据流的开始地址
**修 改 人：
**修改日期：
**/
static BYTE *searchSync(BYTE *search_sync_ptr, BYTE *search_end_ptr, BYTE *sync_status)
{
	*sync_status = SYNC_MISSING;

	for (; search_sync_ptr < search_end_ptr; search_sync_ptr++)
	{
		if (*search_sync_ptr == 0xFF)
		{
			if ((search_end_ptr - search_sync_ptr) > 1)
			{
				if (*(search_sync_ptr+1) == 0x55)
				{
					*sync_status = SYNC_FOUND;
					break;
				}
				else
				{
					*sync_status = SYNC_MISSING;
					break;
				}
			}
			else
			{
				*sync_status = SYNC_PARTIAL;
				break;
			}
		}
	}

	return search_sync_ptr;
}

/*函数名称：isCheckValueVaild
**函数参数：search_sync_ptr：数据流的开始地址
**函数功能：循环查找数据流
**函数返回：
**修 改 人：
**修改日期：
**/
static BOOL isCheckValueVaild(BYTE *search_sync_ptr)
{
	//UINT16 i=0;
	//BYTE checksum = 0;
	//BYTE oldChecksum = 0;
	//UINT16 streamLength = 0;

	//streamLength = *(search_sync_ptr + 2) + 1;
	//for(i=0; i<streamLength; i++)
	//{
	//	checksum += *(search_sync_ptr + i + 2);
	//}

	//oldChecksum = (BYTE)(0xFF - *(search_sync_ptr + *(search_sync_ptr + 2)+3));
	//if (checksum == oldChecksum)
	//{
	//	return TRUE;
	//}
	//return FALSE;

	return TRUE;	
}

/*函数名称：searchDataStream
**函数参数：search_sync_ptr：数据流的开始地址
**函数功能：数据流检验
**函数返回：
**修 改 人：
**修改日期：
**/
static BYTE *searchDataStream(BYTE *search_sync_ptr, BYTE *search_end_ptr)
{	
	BYTE sync_status;

	while (search_sync_ptr < search_end_ptr)
	{
		search_sync_ptr = searchSync(search_sync_ptr, search_end_ptr, &sync_status);

		if (sync_status == SYNC_FOUND)
		{
			if (search_end_ptr - search_sync_ptr > 2)
			{
				{
					//是否为完整的数据流
					if ((search_end_ptr - search_sync_ptr) >= *(search_sync_ptr+2) + 3)
					{
						if (isCheckValueVaild(search_sync_ptr))
						{							
							//提取数据
							//RETAILMSG(1,(TEXT("socket Read right!\r\n")));
							search_sync_ptr = extractPacket(search_sync_ptr);
						}
						else
						{
							//RETAILMSG(1,(TEXT("socket Read err!\r\n")));
							search_sync_ptr += 2;
						}
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				break;
			}
		}
		else if (sync_status == SYNC_MISSING)
		{
			search_sync_ptr += 1;
		}
		else
		{
			break;
		}
	}

	return search_sync_ptr;
}

/*函数名称：moveDataForNext
**函数参数：start_buffer_ptr：保存的首地址，start_data_ptr：要保存数据流的开始地址，buffer_num：大小
**函数功能：把没处理完的数据存起来，为下一次使用
**函数返回：
**修 改 人：
**修改日期：
**/
static void moveDataForNext(BYTE *start_buffer_ptr, BYTE *start_data_ptr, UINT16 buffer_num)
{
	UINT16 i;

	for (i=0; i<buffer_num; i++)
	{
		*start_buffer_ptr = *start_data_ptr;
		start_buffer_ptr++;
		start_data_ptr++;
	}
}

/*函数名称：dealDataStream
**函数参数：search_sync_ptr：数据流的开始地址,search_end_ptr：数据流的结束地址
**函数功能：处理数据流
**函数返回：
**修 改 人：
**修改日期：
**/
static void dealDataStream(BYTE *search_sync_ptr, BYTE *search_end_ptr)
{
	search_sync_ptr = searchDataStream(search_sync_ptr, search_end_ptr);

	if (search_sync_ptr < search_end_ptr)
	{
		gSrvSocketInfo.iSocketReadBufferLen = (UINT16)(search_end_ptr - search_sync_ptr);
		if (gSrvSocketInfo.iSocketReadBufferLen >= SOCKET_BUFF_MAX)
		{
			gSrvSocketInfo.iSocketReadBufferLen = 0;
		}

		//把不完整的数据流保存起来
		moveDataForNext( &gSrvSocketInfo.cSocketReadBuffer[0],
			search_sync_ptr, gSrvSocketInfo.iSocketReadBufferLen);
	}
	else
	{
		gSrvSocketInfo.iSocketReadBufferLen = 0;
	}
}

/*函数名称：WaitClientSocket
**函数参数：
**函数功能：等待客户请求到来
**函数返回：
**修 改 人：
**修改日期：
**/
static BOOL WaitClientSocket(void)
{
	//等待客户请求到来
	gSrvSocketInfo.len = sizeof(SOCKADDR);
	gSrvSocketInfo.sockComm = accept(gSrvSocketInfo.sockSrv,
		(SOCKADDR*)&gSrvSocketInfo.addrClient,&gSrvSocketInfo.len);
	gSrvSocketInfo.iSocketReadBufferLen = 0;

	return TRUE;
}

/*函数名称：SocketRead
**函数参数：
**函数功能：读取数据的线程
**函数返回：
**修 改 人：
**修改日期：
**/
DWORD SocketRead(LPVOID pContext)
{
	INT ret=0;
	BYTE *search_sync_ptr = NULL;
	BYTE *search_end_ptr  = NULL;

	gSrvSocketInfo.bLoopLister = TRUE;
	while (!gSrvSocketInfo.bKillThread)
	{
		//是否要循环监听
		if (gSrvSocketInfo.bLoopLister)
		{
			WaitClientSocket();
			gSrvSocketInfo.bLoopLister = FALSE;
			RETAILMSG(1,(TEXT("server socket success!\r\n")));
		}

		//读数据
		ret = recv(gSrvSocketInfo.sockComm, 
			(char*)&gSrvSocketInfo.cSocketReadBuffer[gSrvSocketInfo.iSocketReadBufferLen],
			SOCKET_BUFF_MAX-gSrvSocketInfo.iSocketReadBufferLen,0);
		if (ret > 0)
		{

			gSrvSocketInfo.iSocketReadBufferLen += (UINT16)ret;
			search_sync_ptr = &gSrvSocketInfo.cSocketReadBuffer[0];
			search_end_ptr  = &gSrvSocketInfo.cSocketReadBuffer[gSrvSocketInfo.iSocketReadBufferLen];
			dealDataStream(search_sync_ptr,search_end_ptr);
		}
		else 
		{
			//客户端退出，重新监听
			closesocket(gSrvSocketInfo.sockComm);
			gSrvSocketInfo.bLoopLister = TRUE;
		}
	}

	return 0;
}


/*函数名称：CreateSocketReadThread
**函数参数：
**函数功能：创建读取数据的线程
**函数返回：
**修 改 人：
**修改日期：
**/
static BOOL CreateSocketReadThread(void)
{
	//创建线程，等待事件
	DWORD dwThreadID;
	gSrvSocketInfo.bKillThread = FALSE;
	gSrvSocketInfo.hThreadHandle = CreateThread( 
		(LPSECURITY_ATTRIBUTES) NULL,			//安全属性
		0,										//初始化线程栈的大小，缺省为与主线程大小相同
		(LPTHREAD_START_ROUTINE)SocketRead,		//线程的全局函数
		&gSrvSocketInfo,						//此处传入了主框架的句柄
		0, &dwThreadID );
	if (NULL == gSrvSocketInfo.hThreadHandle)
	{
		return FALSE;
	}

	return TRUE;
}

/*函数名称：CreateServerSocket
**函数参数：fSockRecvFun：回调函数，SockPort：端口号
**函数功能：创建套接字
**函数返回：
**修 改 人：
**修改日期：
**/
BOOL CreateServerSocket(fSockRecvFunction fSockRecvFun, UINT32 SockPort)
{

	int err;
	WSADATA wsaData;
	WORD wVersionRequested;

	LINGER lingerOpt;
	BOOL dontLinger = TRUE;

	wVersionRequested = MAKEWORD(2,2); 
	err = WSAStartup(wVersionRequested,&wsaData);
	if (0 != err)
	{
		RETAILMSG(1,(TEXT("Create server socket err!\r\n")));
		return FALSE;
	}

	gSrvSocketInfo.sockSrv = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	lingerOpt.l_linger = 0;
	lingerOpt.l_onoff = 0;

	if (setsockopt(gSrvSocketInfo.sockSrv, 
		SOL_SOCKET,SO_DONTLINGER,(char *)&dontLinger,sizeof(BOOL)) != 0)
	{
		closesocket(gSrvSocketInfo.sockSrv);
		RETAILMSG(1,(TEXT("set socket err 1\r\n")));
		return FALSE;
	}

	if (setsockopt(gSrvSocketInfo.sockSrv, 
		SOL_SOCKET,SO_LINGER,(char *)&lingerOpt,sizeof(LINGER)) != 0)
	{
		closesocket(gSrvSocketInfo.sockSrv);
		RETAILMSG(1,(TEXT("set socket err 2\r\n")));
		return FALSE;
	}

	gSrvSocketInfo.fSockRecvFun = fSockRecvFun;
	gSrvSocketInfo.sockComm = 0;
	gSrvSocketInfo.SockPort = SockPort;
	InitializeCriticalSection(&gSrvSocketInfo.cCriticalSectionWrite);


	SOCKADDR_IN addrSrv;
	memset(&addrSrv,0,sizeof(SOCKADDR_IN));
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(gSrvSocketInfo.SockPort);

	//绑定套接字
	if(bind(gSrvSocketInfo.sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR_IN))== SOCKET_ERROR)
	{
		RETAILMSG(1,(TEXT("bind socket err \r\n")));
		return FALSE;
	}

	//将套接字设为监听模式，准备接收客户请求
	listen(gSrvSocketInfo.sockSrv,5);

	if(!CreateSocketReadThread())
	{
		RETAILMSG(1,(TEXT("Create socket read thread err!\r\n")));
		return FALSE;
	}

	return TRUE;
}

/*函数名称：FreeSocketServer
**函数参数：
**函数功能：释放套接字
**函数返回：
**修 改 人：
**修改日期：
**/
BOOL FreeSocketServer(void)
{
	gSrvSocketInfo.bKillThread = TRUE;
	CloseHandle(gSrvSocketInfo.hThreadHandle);
	DeleteCriticalSection(&gSrvSocketInfo.cCriticalSectionWrite);

	closesocket(gSrvSocketInfo.sockSrv);
	WSACleanup();

	return TRUE;
}

/*函数名称：SocketWrite
**函数参数：
**函数功能：写数据
**函数返回：
**修 改 人：
**修改日期：
**/
BOOL SocketWrite(BYTE *buf, UINT16 len)
{
	assert(NULL != buf);
	if (len == 0 || len >= SOCKET_BUFF_MAX)
		return FALSE;

	//正在监听
	if (gSrvSocketInfo.bLoopLister)
		return FALSE;

	//加 FF 55
	EnterCriticalSection(&gSrvSocketInfo.cCriticalSectionWrite);

	BYTE crc = 0,i;
	gSrvSocketInfo.cSocketWriteBuffer[0]=0xFF;
	gSrvSocketInfo.cSocketWriteBuffer[1]=0x55;
	gSrvSocketInfo.cSocketWriteBuffer[2]=len+1;
	memcpy(&gSrvSocketInfo.cSocketWriteBuffer[3],buf,len);

	crc = len+1;
	for (i=0;i<len; i++)
		crc += buf[i];

	gSrvSocketInfo.cSocketWriteBuffer[3+len] = crc;
	gSrvSocketInfo.iSocketWriteBufferLen = 3+len+1;

	LeaveCriticalSection(&gSrvSocketInfo.cCriticalSectionWrite);

	if (gSrvSocketInfo.sockComm > 0)
	{
		send(gSrvSocketInfo.sockComm,
			(char*)gSrvSocketInfo.cSocketWriteBuffer,gSrvSocketInfo.iSocketWriteBufferLen,0);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}
