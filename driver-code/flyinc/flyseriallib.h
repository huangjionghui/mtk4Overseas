#ifndef __FLY_SERIAL_LIB_H__
#define __FLY_SERIAL_LIB_H__

#include "flyglobaldef.h"
HANDLE OpenSerial(LPCWSTR strSerial, DWORD iBaudrate);
BOOL CloseSerial(HANDLE hHandle);
UINT32 ReadSerial(HANDLE hHandle,BYTE *buf, UINT32 len);
BOOL WriteSerial(HANDLE hHandle,BYTE *buf, UINT32 len);

#endif