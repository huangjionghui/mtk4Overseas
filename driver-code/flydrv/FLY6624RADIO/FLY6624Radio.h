/*******************************************
NAME:        FLY6624Radio.h 
DRIVER NAME: FLYRADIO
PLATFORM:    WINCE
AUTHOR:      LJChan
VERSION:     V1
DATA:        2012-05-02
*******************************************/
#pragma once

#ifndef _FLYRADIO_TEF6624_RADIO_H_
#define _FLYRADIO_TEF6624_RADIO_H_

#include "flyshmlib.h"
#include "flygpio.h"
#include "flyhwlib.h"
#include "flyuserlib.h"
#include "flysocketlib.h"

#if SOFT_VERSION_FOR_OVERSEAS
#error "This software version is for overseras!!!"
#endif


#ifdef __cplusplus
extern "C"
{
#endif

//I2C
#define NORMAL_I2C_FREQ	   100000 
#define TEF6624_I2C_FREQ   100000

#define TEF6624_ADDR_R				0xC1
#define TEF6624_ADDR_W				0xC0

//RDS
#define RADIO_RDS 1
#define RADIO_RDS_DEBUG_MSG 0
#define RADIO_RDS_DISPLAY_DEBUG	0


#define OFFSET_A			0x00fc			// 0x00fc
#define OFFSET_B			0x0198			// 0x0198
#define OFFSET_C			0x0168			// 0x0168
#define OFFSET_C2			0x0350			// 0x0350
#define OFFSET_D			0x01b4			// 0x01b4



#define GROUP_A     0
#define GROUP_B     1

enum enumRadioMode{FM1=0,FM2,AM};
enum enumRadioArea{AREA_EU=0,AREA_USA_WB,AREA_OIRT,AREA_JAPAN,AREA_SAM};
enum enumRadioStepDirection{STEP_BACKWARD=0,STEP_FORWARD,STEP_NONE};
enum enumRadioStepMode{STEP_MANUAL=0,STEP_SCAN};

#define DATA_BUFF_LENGTH	1024

#define TEF6624_REG_R_LEVEL  1
#define TEF6624_REG_R_NOISE  2

#ifdef MTK_STOP_HIGHT_SENSITIVITYL
#define TEF6624_STOP_FM_LEVEL   136
#define TEF6624_STOP_AM_LEVEL   216

#define TEF6624_STOP_FM_NOISE   2
#define TEF6624_STOP_AM_NOISE   5
#else
#define TEF6624_STOP_FM_LEVEL   136
#define TEF6624_STOP_AM_LEVEL   210

#define TEF6624_STOP_FM_NOISE   2
#define TEF6624_STOP_AM_NOISE   5
#endif


#define TEF6624_WRITE_MODE_PRESET 0x20
#define TEF6624_WRITE_MODE_SEARCH 0x40

typedef struct _FLY_RADIO_INFO
{
	UINT iFreqFMMin;
	UINT iFreqFMMax;
	UINT iFreqFMManualStep;
	UINT iFreqFMScanStep;

	UINT iFreqAMMin;
	UINT iFreqAMMax;
	UINT iFreqAMManualStep;
	UINT iFreqAMScanStep;

	enumRadioMode ePreRadioMode;
	enumRadioMode eCurRadioMode;
	enumRadioArea ePreRadioArea;
	enumRadioArea eCurRadioArea;
	UINT iPreRadioFreqFM1;
	UINT iCurRadioFreqFM1;
	UINT iPreRadioFreqFM2;
	UINT iCurRadioFreqFM2;
	UINT iPreRadioFreqAM;
	UINT iCurRadioFreqAM;

	UINT *pPreRadioFreq;
	UINT *pCurRadioFreq;

	BOOL bPreScaning;
	BOOL bCurScaning;
	enumRadioStepDirection eScanDirection;
	BOOL bScanRepeatFlag;

	UINT iSignalLevel;
	UINT iSignalNoise;
	BYTE itef6624_writemode;

	BOOL bPreStepButtomDown;
	//BOOL bCurStepButtomDown;
	enumRadioStepDirection eButtomStepDirection;
	UINT iButtomStepCount;
}FLY_RADIO_INFO, *P_FLY_RADIO_INFO;

typedef struct _FLY_RDS_INFO
{
	BOOL RadioRDSAFControlOn;
	BOOL RadioRDSTAControlOn;

	UINT16 BaseReferencePI;
	UINT16 rdsdec_flag_recv[2];// 表示 正在播出的节目的GROUP 标志位 比如节目发了 0A 和 3B 组 此时rdsdec_flag_recv[0]的第一位置1 rdsdec_flag_recv[1]的第四位置1 		
	UINT16 rdsdec_flag_pi;	 //Programme Identification 每个节目只有唯一的PI 前四位是 地区编码
	//B15-B12	Country code
	//B11-B8	Programme type in terms of area coverage
	//B7-B0		Programme reference number
	BYTE  rdsdec_flag_pty; //0表示none，节目类型
	BYTE  rdsdec_flag_tp;	  //0表示没有携带，Traffic Programme (TP) Flag	 表示 节目带有 交通信息
	BYTE  rdsdec_flag_ms;	  //0语言类，1音乐类， music/speech 表示正在播出的节目是 音乐还是语音节目
	BYTE  rdsdec_flag_di[4];	 //	  Decoder Identification 解码标志位 暂时没有用到
	BYTE  rdsdec_flag_ta;	  //0表示无， Traffic Announcement (TP) Flag //TP 和 TA 同时为 1，表示Traffic report in progress on this service，此时关掉其他动作 听此报告

	BYTE  rdsdec_flag_eon;  //0表示无，表示有无Enhanced Other Networks (EON) 节目 暂时没有 做深入解码
	/* DEF_SUPPORT_0 */
	UINT16   rds_ecc;	    // Extended Country Code  根据它确定 某个国家 因为PI的前四位 可能是一些国家共用的
	UINT16   rds_language_code;	  //语言编码

	/* DEF_SUPPORT_0 */
	BYTE rdsdec_ps[2][8];   //Programme Service (PS) Name  节目名字
	BYTE rdsdec_ps_dis[8];
	//其实只有一组有效，当改变时往另一数组写数
	BYTE rdsdec_af[8];      //Alternative Frequency List—AF
	/* DEF_SUPPORT_2 */
	BYTE rdsdec_rt_a[2][64]; //RadioText (RT) 这是 GROUP_A的 TEXT_A 和 TEXT_B
	BYTE rdsdec_rt_a_dis[64];
	//其实只有一组有效，当改变时往另一数组写数
	BYTE rdsdec_rt_b[32]; // for Decode RadioText (RT) 这是 GROUP_B的 TEXT_A 和 TEXT_B
	//其实只有一组有效，当改变时往另一数组写数

	/* DEF_SUPPORT_4 */	   //以下是 RDS 的时间 校正
	UINT16 rds_clock_year;
	BYTE  rds_clock_month;
	BYTE  rds_clock_day;
	BYTE  rds_clock_wd; // weekday 1~7
	BYTE  rds_clock_hour;
	BYTE  rds_clock_min;
	BYTE  rds_clock_ofs;

	BYTE rdsdec_ptyn[2][8];   // for Decode 可编程的节目类型名
	//其实只有一组有效，当改变时往另一数组写数
}FLY_RDS_INFO, *P_FLY_RDS_INFO;

typedef struct _FLY_RDS_DATA_INFO
{
	UINT16 rds_year;
	BYTE  rds_month;
	BYTE  rds_day;
	BYTE  rds_weekday; 
	BYTE  rds_hour;
	BYTE  rds_min;
	BYTE  rds_ofs;
}FLY_RDS_DATA_INFO;

typedef struct _FLY_RDS_AF_INFO
{
	UINT16 aflist[25];
	BYTE aflist_count;
	BYTE af_checklist_point;
	BYTE eafswitch_mode;
}FLY_RDS_AF_INFO;

typedef struct _FLY_TEF6624_RDS_INFO
{
	//RDS线程相关
	BOOL    bKillDispatchRDSThread;
	HANDLE	hFlyRDSThreadHandle;
	HANDLE  hDispatchRDSThreadEvent;

	BYTE   iCurRadioMode;
	UINT32 iCurRadioFreq;
	UINT32 iTickTimes;

	UINT16 nRdsDataInfo;
	UINT16 nRdsDataCRC;
	BYTE   nRdsDataBitCnt;
	BOOL   bRdsDataSyn;
	BYTE   nRdsDataStep;

#define RDS_DATA_LEN  100
	UINT16 iRdsDataHx;
	UINT16 iRdsDataLx;
	BYTE   cRdsDataBuff[RDS_DATA_LEN][8];

	UINT16 iNewPICode;
	UINT16 iBasePICode;
	BYTE   iBaseFreq;

	BYTE iProgranType;
	BOOL bTrafficProgramIDTuned;
	BOOL bTrafficProgramIDOther;
	BOOL bTrafficAnnouncementIDTuned;

	BYTE iRdsSegmentPSDiAf;
	BYTE cRdsPSSegment[2];
	BYTE iPreProgramServiceName[2][8]; //PS
	BYTE iCurProgramServiceName[2][8];


	BOOL bMusicSpeechSwitch;

	BYTE iRdsGroup;
	BYTE iRdsGroupType;

	BYTE iRdsDISegment;                //DI
	BYTE iTempDI[4];
	BYTE iDecoderID; 
	
	BYTE iRdsRTSegmentA;
	BYTE cPreRadioTextA[64];        //RT
	BYTE cCurRadioTextA[64];

	BYTE iRdsRTSegmentB;
	BYTE cPreRadioTextB[32];        //RT
	BYTE cCurRadioTextB[32];

	BYTE iRdsPT_A_G;

	FLY_RDS_AF_INFO sAfInfo;
	FLY_RDS_DATA_INFO sDateInfo;


}FLY_TEF6624_RDS_INFO,*P_FLY_TEF6624_RDS_INFO;

typedef struct _FLYRADIO_TEF6624_INFO
{
	BOOL bOpen;
	BOOL bPowerUp;
	BOOL bNeedInit;

	BOOL bPreMute;
	BOOL bCurMute;

	BYTE buffToUser[DATA_BUFF_LENGTH];
	UINT buffToUserHx;
	UINT buffToUserLx;
	HANDLE hBuffToUserDataEvent;
	CRITICAL_SECTION hCSSendToUser;
	
	HANDLE	FlyRadioMainThreadHandle;
	BOOL bKillDispatchMainThread;
	HANDLE hDispatchMainThreadEvent;

	HANDLE	FlyRadioScanThreadHandle;
	BOOL bKillDispatchScanThread;
	HANDLE hDispatchScanThreadEvent;

	HANDLE hMsgQueueToRadioCreate;
	HANDLE FlyRadioRDSRecThreadHandle;
	BOOL bKillDispatchRDSRecThread;

	HANDLE	FlyRadioI2CHandle;

	FLY_RADIO_INFO radioInfo;
	FLY_RDS_INFO RDSInfo;
	FLY_TEF6624_RDS_INFO sTEF6624RDSInfo;

	//PFLY_DRIVER_GLOBAL pFlyDriverGlobalInfo;
	volatile shm_t *pFlyShmGlobalInfo;
	HANDLE hHandleGlobalGlobalEvent;
}FLYRADIO_TEF6624_INFO, *P_FLYRADIO_TEF6624_INFO;

//#if RADIO_RDS
//DWORD WINAPI FlyRadioRDSRecThread(LPVOID pContext);
//VOID FlyRadioReturnToUser(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BYTE *buf,UINT len);
//void RDSParaInit(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo);
//#endif

BOOL I2C_Write_TEF6624(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo, UCHAR ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen);
BOOL I2C_Read_TEF6624(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo, UCHAR ulRegAddr, PBYTE pRegValBuf, UINT uiValBufLen);



BOOL CreateTEF6624RDSThread(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo);


#define MSG_ENABLE_FLAG (pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if (FLY_DEBUG_6624 && GLOBAL_DEBUG)
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

#endif