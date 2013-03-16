/*模块名称：flyuserlib.cpp
**模块功能：用户缓冲区相关操作
**
**修 改 人：黄炯辉
**修改日期：2012-09-01
**/
#include "flyuserlib.h"
#include <assert.h>
#include <windows.h>
#include <commctrl.h>


//返回给用户的循环BUF的大小
#define BUF_TO_USER_MAX 100
#define BUF_TO_USER_LEN 400

//返回用户相关
struct UserInfo{
	UINT16 iBufToUserHx;
	UINT16 iBufToUserLx;
	BYTE BufToUser[BUF_TO_USER_MAX][BUF_TO_USER_LEN];
	HANDLE hToUserReadEvent;
	CRITICAL_SECTION hCSSendToUser; 
};

struct UserInfo sUserInfo;


/*函数名称：CreateUserBuff
**函数参数：
**函数功能：创建用户缓冲区
**函数返回：成功时返回TRUE
**修 改 人：
**修改日期：
**/
BOOL CreateUserBuff(void)
{
	sUserInfo.iBufToUserLx = 0;
	sUserInfo.iBufToUserHx = 0;
	InitializeCriticalSection(&sUserInfo.hCSSendToUser);
	sUserInfo.hToUserReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	

	return TRUE;
}

/*函数名称：FreeUserBuff
**函数参数：
**函数功能：释放用户缓冲区
**函数返回：成功时返回TRUE
**修 改 人：
**修改日期：
**/
BOOL FreeUserBuff(void)
{
	sUserInfo.iBufToUserLx = 0;
	sUserInfo.iBufToUserHx = 0;
	CloseHandle(sUserInfo.hToUserReadEvent);
	DeleteCriticalSection(&sUserInfo.hCSSendToUser);
	sUserInfo.hToUserReadEvent = NULL;

	return TRUE;
}

/*函数名称：ReturnToUser
**函数参数：
**函数功能：返回数据给用户
**函数返回：成功时返回TRUE
**修 改 人：
**修改日期：
**/
BOOL ReturnToUser(BYTE *buf,UINT16 len)
{
	BYTE crc=0,i=0;
	if (len == 0)
		return FALSE;
	
	if (len > BUF_TO_USER_LEN - 4)
		len = BUF_TO_USER_LEN - 4;


	EnterCriticalSection(&sUserInfo.hCSSendToUser);
	sUserInfo.BufToUser[sUserInfo.iBufToUserHx][0] = 0xFF;
	sUserInfo.BufToUser[sUserInfo.iBufToUserHx][1] = 0x55;
	sUserInfo.BufToUser[sUserInfo.iBufToUserHx][2] = len+1;
	memcpy(&sUserInfo.BufToUser[sUserInfo.iBufToUserHx][3],buf,len);

	crc = len + 1;
	for(i = 0;i <len;i++)
		crc += buf[i];

	sUserInfo.BufToUser[sUserInfo.iBufToUserHx][3+len] = crc;//CRC

	sUserInfo.iBufToUserHx++;
	if (sUserInfo.iBufToUserHx > BUF_TO_USER_MAX - 1)
		sUserInfo.iBufToUserHx = 0;
	LeaveCriticalSection(&sUserInfo.hCSSendToUser);

	SetEvent(sUserInfo.hToUserReadEvent);

	return TRUE;
}

/*函数名称：ReturnOriginalDataToUser
**函数参数：
**函数功能：返回原始数据给用户，没用FF 55 封装
**函数返回：成功时返回TRUE
**修 改 人：
**修改日期：
**/
BOOL ReturnOriginalDataToUser(BYTE *buf,UINT16 len)
{
	BYTE i=0;
	if (len == 0 || len > BUF_TO_USER_LEN - 4)
		return FALSE;

	EnterCriticalSection(&sUserInfo.hCSSendToUser);

	memcpy(&sUserInfo.BufToUser[sUserInfo.iBufToUserHx][0],buf,len);
	sUserInfo.iBufToUserHx++;
	if (sUserInfo.iBufToUserHx > BUF_TO_USER_MAX - 1)
		sUserInfo.iBufToUserHx = 0;

	LeaveCriticalSection(&sUserInfo.hCSSendToUser);

	SetEvent(sUserInfo.hToUserReadEvent);

	return TRUE;
}

/*函数名称：ReadData
**函数参数：
**函数功能：用户读据
**函数返回：成功时返回字节大小
**修 改 人：
**修改日期：
**/
UINT16 ReadData(BYTE *buf,UINT16 len)
{
	assert(NULL != buf);

	if (len <= 0)
		return 0;

	UINT16 retCounts=0;

	WaitForSingleObject(sUserInfo.hToUserReadEvent, INFINITE);

	EnterCriticalSection(&sUserInfo.hCSSendToUser);
	if (sUserInfo.iBufToUserHx != sUserInfo.iBufToUserLx)
	{
		retCounts = sUserInfo.BufToUser[sUserInfo.iBufToUserLx][2]+3;
		if (retCounts > BUF_TO_USER_LEN-1)
		{
			LeaveCriticalSection(&sUserInfo.hCSSendToUser);
			return 0;
		}

		memcpy(buf,&sUserInfo.BufToUser[sUserInfo.iBufToUserLx][0],retCounts);

		sUserInfo.iBufToUserLx++;
		if (sUserInfo.iBufToUserLx > BUF_TO_USER_MAX - 1)
			sUserInfo.iBufToUserLx = 0;


	}
	LeaveCriticalSection(&sUserInfo.hCSSendToUser);

	if (sUserInfo.iBufToUserHx != sUserInfo.iBufToUserLx)
		SetEvent(sUserInfo.hToUserReadEvent);

	return retCounts;
}