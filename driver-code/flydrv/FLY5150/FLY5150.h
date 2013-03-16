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
#define  FLY_EVENT_NAME_CAMERA_SIG		TEXT("FLY_BACK_CAMERA_ON_OFF") //倒车视频


#define  EVENT_FLY_AUX_ON_OFF          TEXT("FLY_AUX_ON_OFF")//AUX视频

	enum setDisplay{E_C_COLOR = 0,E_C_HUE,E_C_CONTRAST,E_C_BRIGHTNESS,E_C_ALPHA};

	enum SetColorCommd{
		SET_VIDEO_COLOR,
		SET_DEF_COLOR,
		SET_RESET_COLOR
	};

	typedef struct _VIN_CONFIG
    {
        // SET_ENABLE
        BOOL enable;   // 视频输入是否和显示视频

        // SET_CHANNEL
        unsigned char channel;//检测视频的通道 0/1

        // SET_WINDOW
        unsigned int width; // 显示视频窗口的宽度
        unsigned int height;// 显示视频窗口的高度
        unsigned int pos_x; // 显示视频的开始坐标X
        unsigned int pos_y; // 显示视频的开始坐标Y

        // SET_DISPLAY
        unsigned char brightness;  // 画面亮度
        unsigned char color;	  // 画面色彩
        unsigned char hue;	   // 画面色调
        unsigned char contrast;   // 画面对比度
        unsigned char alpha;	   // 画面透明度

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

		//倒车是否有视频事件，如江德鹏发出
		BOOL bKillDispatchFlyBackCarThread;
		HANDLE FlyBackCarThreadHandle;
		HANDLE hDispatchBackCarThreadEvent;
		BOOL  bBackCarValue;

		//AUX是否有视频事件，如江德鹏发出
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
#define DBGE(string) RETAILMSG(1,string)//重要错误
#define DBGW(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string)//警告
#define DBGI(string) if (1) RETAILMSG(1,string)//信息
#define DBGD(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string) //调试信息
#else
#define DBGE(string) RETAILMSG(1,string)//重要错误
#define DBGW(string) DEBUGMSG(1,string) //警告
#define DBGI(string) DEBUGMSG(1,string) //信息
#define DBGD(string) DEBUGMSG(1,string) //调试信息
#endif


#ifdef __cplusplus
}
#endif



#endif// _FLYAUDIO_5150_H_