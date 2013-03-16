#ifndef __FLY_SOCKET_LIB_H__
#define __FLY_SOCKET_LIB_H__

#include "flyshmlib.h"
#include "flyglobaldef.h"
#include <windows.h>
#include <Winsock2.h>

#define TCP_PORT_BT  9168
#define TCP_PORT_DVD 9169

#define TCP_PORT_SYSTEM  9980
#define TCP_PORT_RADIO   9981
#define TCP_PORT_GLOBAL  9982
#define TCP_PORT_AUDIO   9983
#define TCP_PORT_KEY     9984
#define TCP_PORT_VIDEO   9985
#define TCP_PORT_CARBODY 9986


BOOL CreateServerSocket(fSockRecvFunction fSockRecvFun, UINT32 SockPort);
BOOL FreeSocketServer(void);
BOOL SocketWrite(BYTE *buf, UINT16 len);

#endif 