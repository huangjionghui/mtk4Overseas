/************************************************************************/
/* FLYAUDIO 5150                                                     */
/************************************************************************/
#pragma once

#ifndef _FLYAUDIO_5150_H_
#define _FLYAUDIO_5150_H_

#include <windows.h>
#include <basetsd.h>
#include "flyshmlib.h"
#include "flygpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define DATA_BUFF_LENGTH	1024
#define  FLY_EVENT_NAME_CAMERA_SIG		TEXT("FLY_BACK_CAMERA_ON_OFF") //������Ƶ


#define  EVENT_FLY_AUX_ON_OFF          TEXT("FLY_AUX_ON_OFF")//AUX��Ƶ

	enum setDisplay{E_C_COLOR = 0,E_C_HUE,E_C_CONTRAST,E_C_BRIGHTNESS,E_C_ALPHA};

	enum SetColorCommd{
		SET_VIDEO_COLOR,
		SET_DEF_COLOR,
		SET_RESET_COLOR
	};

	typedef struct _VIN_CONFIG
    {
        // SET_ENABLE
        BOOL enable;   // ��Ƶ�����Ƿ����ʾ��Ƶ

        // SET_CHANNEL
        unsigned char channel;//�����Ƶ��ͨ�� 0/1

        // SET_WINDOW
        unsigned int width; // ��ʾ��Ƶ���ڵĿ��
        unsigned int height;// ��ʾ��Ƶ���ڵĸ߶�
        unsigned int pos_x; // ��ʾ��Ƶ�Ŀ�ʼ����X
        unsigned int pos_y; // ��ʾ��Ƶ�Ŀ�ʼ����Y

        // SET_DISPLAY
        unsigned char brightness;  // ��������
        unsigned char color;	  // ����ɫ��
        unsigned char hue;	   // ����ɫ��
        unsigned char contrast;   // ����Աȶ�
        unsigned char alpha;	   // ����͸����

		BOOL black;
    } VIN_CONFIG, *PVIN_CONFIG;

	 typedef enum
    {
        SET_ENABLE, //enable
        SET_CHANNEL, //channel
        SET_WINDOW, //width,height,pos_x,pos_y
		SET_COLORE,
		SET_HUE,
		SET_CONTRAST,
		SET_BRIGHTNESS,
		SET_ALPHA,
        SET_ALL
    } VIN_CONFIG_CODE;


	typedef struct _FLY_5150_INFO
	{
		BOOL bOpen;
		BOOL bPower;
		BOOL bPowerUp;
		BOOL bSpecialPower;
		BOOL bSocketConnecting;
		BOOL bSetColorEnable;

		BOOL bKillDispatchFlyMainThread;
		HANDLE FlyMainThreadHandle;
		HANDLE hDispatchMainThreadEvent;

		//�����Ƿ�����Ƶ�¼����罭��������
		BOOL bKillDispatchFlyBackCarThread;
		HANDLE FlyBackCarThreadHandle;
		HANDLE hDispatchBackCarThreadEvent;
		BOOL  bBackCarValue;

		//AUX�Ƿ�����Ƶ�¼����罭��������
		BOOL bKillDispatchFlyAuxListerThread;
		HANDLE FlyAuxThreadHandle;
		HANDLE hDispatchAuxListerThreadEvent;
		BOOL  bAuxValue;


		BYTE buffToUser[DATA_BUFF_LENGTH];
		UINT buffToUserHx;
		UINT buffToUserLx;
		HANDLE hBuffToUserDataEvent;
		CRITICAL_SECTION hCSSendToUser;

		BOOL bBackChecking;
		BOOL bBackVideoOn;
		BOOL bBackVideoDealWINAction;
		BOOL bBackVideoStartAction;
		BOOL bBackCheckNeedFirstReturn;
		BOOL bBackCheckReturn;
		BYTE iBackActionCounts;

		ULONG iBackStartCheckingTime;
		ULONG iBackStartReturnTime;
		ULONG iBackLPCStartCheckingTime;

		BOOL bAuxVideoOn;
		BOOL bAuxFirstReturnSignal;

		BYTE iPreVideoChannel;
		BYTE iTempVideoChannel;
		BYTE iCurVideoChannel;
		BYTE iOldVideoChannel;
		BOOL bAUXVideoColorReset;
		BOOL bVideoFirstSelectChannel;

		BOOL bChannelChangeNeedEnable;
		ULONG iChannelChangeTime;
		BOOL bPreVideoEnable;
		BOOL bTempVideoEnable;
		BOOL bCurVideoEnable;
		BOOL bPreVideoBlack;
		BOOL bCurVideoBlack;

		BYTE iTempVideoParaColor;
		BYTE iTempVideoParaHue;
		BYTE iTempVideoParaContrast;
		BYTE iTempVideoParaBrightness;

		BYTE iVideoParaColor;
		BYTE iVideoParaHue;
		BYTE iVideoParaContrast;
		BYTE iVideoParaBrightness;

		VIN_CONFIG cVinConfig;

		BOOL bIsHaveAUXVideoSignal;
		BYTE iVideoReturnChannel;
		CRITICAL_SECTION cDealBackcarThreadSection;

		//PFLY_DRIVER_GLOBAL pFlyDriverGlobalInfo;
		volatile shm_t *pFlyShmGlobalInfo;
		HANDLE hHandleGlobalGlobalEvent;
	}FLY_5150_INFO, *P_FLY_5150_INFO;


#define MSG_ENABLE_FLAG (p5150Info->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_5150 && GLOBAL_DEBUG)
#define DBGE(string) RETAILMSG(1,string)//��Ҫ����
#define DBGW(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string)//����
#define DBGI(string) if (1) RETAILMSG(1,string)//��Ϣ
#define DBGD(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string) //������Ϣ
#else
#define DBGE(string) RETAILMSG(1,string)//��Ҫ����
#define DBGW(string) DEBUGMSG(1,string) //����
#define DBGI(string) DEBUGMSG(1,string) //��Ϣ
#define DBGD(string) DEBUGMSG(1,string) //������Ϣ
#endif


#ifdef __cplusplus
}
#endif



#endif// _FLYAUDIO_5150_H_