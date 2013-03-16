/*ģ�����ƣ�flysocketlib.cpp
**ģ�鹦�ܣ��׽�����ز���
**
**�� �� �ˣ��ƾ���
**�޸����ڣ�2012-09-01
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
	CRITICAL_SECTION cCriticalSectionWrite;//дʱ����
};

struct ServerSocketInfo gSrvSocketInfo;

//�Ƿ�ѭ������
static void ControlLoopListerEnable(BOOL bEnale)
{
	gSrvSocketInfo.bLoopLister = bEnale;
}


/*�������ƣ�extractPacket
**����������search_sync_ptr���������Ŀ�ʼ��ַ
**�������ܣ���ȡһ��������������
**�������أ�����û�б���ȡ�����������ʼ��ַ
**�� �� �ˣ�
**�޸����ڣ�
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

/*�������ƣ�searchSync
**����������search_sync_ptr���������Ŀ�ʼ��ַ��search_end_ptr���������Ľ�����ַ��sync_status����־
**�������ܣ��ҵ�һ���������İ�ͷ
**�������أ������������Ŀ�ʼ��ַ
**�� �� �ˣ�
**�޸����ڣ�
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

/*�������ƣ�isCheckValueVaild
**����������search_sync_ptr���������Ŀ�ʼ��ַ
**�������ܣ�ѭ������������
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
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

/*�������ƣ�searchDataStream
**����������search_sync_ptr���������Ŀ�ʼ��ַ
**�������ܣ�����������
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
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
					//�Ƿ�Ϊ������������
					if ((search_end_ptr - search_sync_ptr) >= *(search_sync_ptr+2) + 3)
					{
						if (isCheckValueVaild(search_sync_ptr))
						{							
							//��ȡ����
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

/*�������ƣ�moveDataForNext
**����������start_buffer_ptr��������׵�ַ��start_data_ptr��Ҫ�����������Ŀ�ʼ��ַ��buffer_num����С
**�������ܣ���û����������ݴ�������Ϊ��һ��ʹ��
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
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

/*�������ƣ�dealDataStream
**����������search_sync_ptr���������Ŀ�ʼ��ַ,search_end_ptr���������Ľ�����ַ
**�������ܣ�����������
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
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

		//�Ѳ���������������������
		moveDataForNext( &gSrvSocketInfo.cSocketReadBuffer[0],
			search_sync_ptr, gSrvSocketInfo.iSocketReadBufferLen);
	}
	else
	{
		gSrvSocketInfo.iSocketReadBufferLen = 0;
	}
}

/*�������ƣ�WaitClientSocket
**����������
**�������ܣ��ȴ��ͻ�������
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
**/
static BOOL WaitClientSocket(void)
{
	//�ȴ��ͻ�������
	gSrvSocketInfo.len = sizeof(SOCKADDR);
	gSrvSocketInfo.sockComm = accept(gSrvSocketInfo.sockSrv,
		(SOCKADDR*)&gSrvSocketInfo.addrClient,&gSrvSocketInfo.len);
	gSrvSocketInfo.iSocketReadBufferLen = 0;

	return TRUE;
}

/*�������ƣ�SocketRead
**����������
**�������ܣ���ȡ���ݵ��߳�
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
**/
DWORD SocketRead(LPVOID pContext)
{
	INT ret=0;
	BYTE *search_sync_ptr = NULL;
	BYTE *search_end_ptr  = NULL;

	gSrvSocketInfo.bLoopLister = TRUE;
	while (!gSrvSocketInfo.bKillThread)
	{
		//�Ƿ�Ҫѭ������
		if (gSrvSocketInfo.bLoopLister)
		{
			WaitClientSocket();
			gSrvSocketInfo.bLoopLister = FALSE;
			RETAILMSG(1,(TEXT("server socket success!\r\n")));
		}

		//������
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
			//�ͻ����˳������¼���
			closesocket(gSrvSocketInfo.sockComm);
			gSrvSocketInfo.bLoopLister = TRUE;
		}
	}

	return 0;
}


/*�������ƣ�CreateSocketReadThread
**����������
**�������ܣ�������ȡ���ݵ��߳�
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
**/
static BOOL CreateSocketReadThread(void)
{
	//�����̣߳��ȴ��¼�
	DWORD dwThreadID;
	gSrvSocketInfo.bKillThread = FALSE;
	gSrvSocketInfo.hThreadHandle = CreateThread( 
		(LPSECURITY_ATTRIBUTES) NULL,			//��ȫ����
		0,										//��ʼ���߳�ջ�Ĵ�С��ȱʡΪ�����̴߳�С��ͬ
		(LPTHREAD_START_ROUTINE)SocketRead,		//�̵߳�ȫ�ֺ���
		&gSrvSocketInfo,						//�˴�����������ܵľ��
		0, &dwThreadID );
	if (NULL == gSrvSocketInfo.hThreadHandle)
	{
		return FALSE;
	}

	return TRUE;
}

/*�������ƣ�CreateServerSocket
**����������fSockRecvFun���ص�������SockPort���˿ں�
**�������ܣ������׽���
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
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

	//���׽���
	if(bind(gSrvSocketInfo.sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR_IN))== SOCKET_ERROR)
	{
		RETAILMSG(1,(TEXT("bind socket err \r\n")));
		return FALSE;
	}

	//���׽�����Ϊ����ģʽ��׼�����տͻ�����
	listen(gSrvSocketInfo.sockSrv,5);

	if(!CreateSocketReadThread())
	{
		RETAILMSG(1,(TEXT("Create socket read thread err!\r\n")));
		return FALSE;
	}

	return TRUE;
}

/*�������ƣ�FreeSocketServer
**����������
**�������ܣ��ͷ��׽���
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
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

/*�������ƣ�SocketWrite
**����������
**�������ܣ�д����
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
**/
BOOL SocketWrite(BYTE *buf, UINT16 len)
{
	assert(NULL != buf);
	if (len == 0 || len >= SOCKET_BUFF_MAX)
		return FALSE;

	//���ڼ���
	if (gSrvSocketInfo.bLoopLister)
		return FALSE;

	//�� FF 55
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
