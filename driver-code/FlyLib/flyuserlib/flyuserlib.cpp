/*ģ�����ƣ�flyuserlib.cpp
**ģ�鹦�ܣ��û���������ز���
**
**�� �� �ˣ��ƾ���
**�޸����ڣ�2012-09-01
**/
#include "flyuserlib.h"
#include <assert.h>
#include <windows.h>
#include <commctrl.h>


//���ظ��û���ѭ��BUF�Ĵ�С
#define BUF_TO_USER_MAX 100
#define BUF_TO_USER_LEN 400

//�����û����
struct UserInfo{
	UINT16 iBufToUserHx;
	UINT16 iBufToUserLx;
	BYTE BufToUser[BUF_TO_USER_MAX][BUF_TO_USER_LEN];
	HANDLE hToUserReadEvent;
	CRITICAL_SECTION hCSSendToUser; 
};

struct UserInfo sUserInfo;


/*�������ƣ�CreateUserBuff
**����������
**�������ܣ������û�������
**�������أ��ɹ�ʱ����TRUE
**�� �� �ˣ�
**�޸����ڣ�
**/
BOOL CreateUserBuff(void)
{
	sUserInfo.iBufToUserLx = 0;
	sUserInfo.iBufToUserHx = 0;
	InitializeCriticalSection(&sUserInfo.hCSSendToUser);
	sUserInfo.hToUserReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	

	return TRUE;
}

/*�������ƣ�FreeUserBuff
**����������
**�������ܣ��ͷ��û�������
**�������أ��ɹ�ʱ����TRUE
**�� �� �ˣ�
**�޸����ڣ�
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

/*�������ƣ�ReturnToUser
**����������
**�������ܣ��������ݸ��û�
**�������أ��ɹ�ʱ����TRUE
**�� �� �ˣ�
**�޸����ڣ�
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

/*�������ƣ�ReturnOriginalDataToUser
**����������
**�������ܣ�����ԭʼ���ݸ��û���û��FF 55 ��װ
**�������أ��ɹ�ʱ����TRUE
**�� �� �ˣ�
**�޸����ڣ�
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

/*�������ƣ�ReadData
**����������
**�������ܣ��û�����
**�������أ��ɹ�ʱ�����ֽڴ�С
**�� �� �ˣ�
**�޸����ڣ�
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