#ifndef __FLY_USER_LIB_H__
#define __FLY_USER_LIB_H__

#include <windows.h>
#include <basetsd.h>
#include "flyglobaldef.h"

BOOL CreateUserBuff(void);
BOOL FreeUserBuff(void);
BOOL ReturnToUser(BYTE *buf,UINT16 len);
BOOL ReturnOriginalDataToUser(BYTE *buf,UINT16 len);
UINT16 ReadData(BYTE *buf,UINT16 len);

#endif