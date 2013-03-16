#include "common.h"


//socket 读的缓冲区大小
#define SOCKET_READ_MAX_BUFF_LEN 400
#define SYNC_MISSING 0
#define SYNC_FOUND   1
#define SYNC_PARTIAL 2


SOCKET sockClient=0;

//socket thread 
BOOL bKillSocketThread   =TRUE;
HANDLE hSocketReadThread = NULL;

UINT16 iReadBuffLen = 0;
BYTE ReadBuff[SOCKET_READ_MAX_BUFF_LEN]={0};


extern void DealDataStreams(BYTE *buf, UINT16 len);

UINT16 SocketRead(BYTE *buf, UINT16 len)
{
	if (sockClient <= 0)
		return 0;

	return recv(sockClient, (char*)buf,len,0);
}

BOOL SocketWrite(BYTE *buf, UINT16 len)
{
	if (sockClient <= 0)
		return FALSE;

	BYTE crc=0,i;
	BYTE buff[SOCKET_READ_MAX_BUFF_LEN];
	buff[0] = 0xFF;
	buff[1] = 0x55;
	buff[2] = len+1;
	memcpy(&buff[3],buf,len);

	crc = len+1;
	for (i=0; i<len;i++)
		crc += buf[i];

	buff[3+len] = crc;

	return 	send(sockClient,(char*)buff,len+1+3,0);
}



BYTE* extractPacket(BYTE *search_sync_ptr)
{
	UINT16 length = 0;
	BYTE buff[SOCKET_READ_MAX_BUFF_LEN];

	length = *(search_sync_ptr + 2);
	memcpy(buff, search_sync_ptr + 3, length);

	DealDataStreams(buff,length-1);//去掉检验位

	search_sync_ptr = search_sync_ptr + length + 2 + 1;

	return search_sync_ptr;
}

BYTE* searchSync(BYTE *search_sync_ptr, BYTE *search_end_ptr, BYTE *sync_status)
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
BOOL isCheckValueVaild(BYTE *search_sync_ptr)
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
	return TRUE;

	return FALSE;
}
BYTE* searchDataStream(BYTE *search_sync_ptr, BYTE *search_end_ptr)
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
void moveDataForNext(BYTE *start_buffer_ptr, BYTE *start_data_ptr, UINT16 buffer_num)
{
	UINT16 i;

	for (i=0; i<buffer_num; i++)
	{
		*start_buffer_ptr = *start_data_ptr;
		start_buffer_ptr++;
		start_data_ptr++;
	}
}
void dealDataStream(BYTE *search_sync_ptr, BYTE *search_end_ptr)
{
	search_sync_ptr = searchDataStream(search_sync_ptr, search_end_ptr);

	if (search_sync_ptr < search_end_ptr)
	{
		iReadBuffLen = (UINT16)(search_end_ptr - search_sync_ptr);
		if (iReadBuffLen >= SOCKET_READ_MAX_BUFF_LEN)
		{
			iReadBuffLen = 0;
		}

		moveDataForNext(&ReadBuff[0],search_sync_ptr, iReadBuffLen);
	}
	else
	{
		iReadBuffLen = 0;
	}
}

DWORD SocketReadThread(LPVOID pContext)
{
	UINT16 ret=0;
	BYTE *search_sync_ptr = NULL;
	BYTE *search_end_ptr  = NULL;

	while (!bKillSocketThread)
	{
		ret = SocketRead(&ReadBuff[iReadBuffLen],SOCKET_READ_MAX_BUFF_LEN - iReadBuffLen);
		if (ret > 0)
		{
			iReadBuffLen += (UINT16)ret;
			search_sync_ptr = &ReadBuff[0];
			search_end_ptr  = &ReadBuff[iReadBuffLen];

			dealDataStream(search_sync_ptr,search_end_ptr);
		}
	}

	return 0;
}

static BOOL CreateThread(void)
{
	DWORD dwThreadID;

	bKillSocketThread = FALSE;
	hSocketReadThread = CreateThread( 
		(LPSECURITY_ATTRIBUTES) NULL,0,	(LPTHREAD_START_ROUTINE)SocketReadThread, NULL,0, &dwThreadID );
	if (NULL == hSocketReadThread)
	{
		return FALSE;
	}

	return TRUE;
}
BOOL CreateClientSocket(UINT32 SockPort)
{
	int err;
	WSADATA wsaData;
	WORD wVersionRequested;

	SOCKADDR_IN addrSrv;

	wVersionRequested = MAKEWORD(2,2); 
	err = WSAStartup(wVersionRequested,&wsaData);
	if (0 != err)
	{
		RETAILMSG(1,(TEXT("Create server socket err!\r\n")));
		return FALSE;
	}

	//if (LOBYTE(wsaData.wVersion) != 2
	//	|| HIBYTE(wsaData.wVersion != 2))
	//{
	//	RETAILMSG(1,(TEXT("socket version err!\r\n")));
	//	WSACleanup();
	//	return FALSE;
	//}

	sockClient = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	addrSrv.sin_addr.S_un.S_addr =inet_addr("127.0.0.1");
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(SockPort);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	//创建线程
	if(!CreateThread())
	{
		RETAILMSG(1,(TEXT("[socket ] create socket read thread err.\r\n")));
		return FALSE;
	}

	return TRUE;
}
BOOL CloseSocket(void)
{
	return TRUE;
}
