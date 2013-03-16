/************************************************************************/
/* FLYAUDIO SAF7741                                                     */
/************************************************************************/
#pragma once

#ifndef _FLYAUDIO_SAF7741_RADIO_H_
#define _FLYAUDIO_SAF7741_RADIO_H_

#include "flyshmlib.h"
#include "flygpio.h"
#include "flyhwlib.h"
#include "flyuserlib.h"
#include "flysocketlib.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define CURRENT_SHARE_MEMORY_ID	SHARE_MEMORY_RADIO

//RDS
#define RADIORDS_ID	0x03

#define RADIORDS_ID_TA	0x12
#define RADIORDS_ID_PS	0x13
#define RADIORDS_ID_PTY	0x14
#define RADIORDS_ID_RT	0x15
#define RADIORDS_ID_AF	0x16
//I2C ADDR
#define SAF7741_ADDR_R				0x39
#define SAF7741_ADDR_W				0x38

#define TEF7000_1_ADDR_R			0xC138
#define TEF7000_1_ADDR_W			0xC038

#define TEF7000_2_ADDR_R			0xC538
#define TEF7000_2_ADDR_W			0xC438

#define SAFTEFNVM_ADDR_R			0xA1
#define SAFTEFNVM_ADDR_W			0xA038

#define NORMAL_ADDR_R				0xA1
#define NORMAL_ADDR_W				0xA0
//SCAN STOP
#define FM_SCAN_STOP_LEVEL          30
#define FM_SCAN_STOP_NOISE          20
#define FM_SCAN_STOP_OFS            40
#define AM_SCAN_STOP_LEVEL          28

enum enumRadioMode{FM1=0,FM2,AM};
enum enumRadioArea{AREA_EU=0,AREA_USA_WB,AREA_OIRT,AREA_JAPAN,AREA_SAM};
enum enumRadioStepDirection{STEP_BACKWARD=0,STEP_FORWARD,STEP_NONE};
enum enumRadioStepMode{STEP_MANUAL=0,STEP_SCAN};

#define DATA_BUFF_LENGTH	1024
#define DATA_READ_HANDLE_NAME	TEXT("FLYRADIO_DATA_READ_EVENT")

#define RADIO_RDS		1
#define RADIO_RDS_DEBUG_MSG	1

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
	//其实只有一组有效，当改变时往另一数组写数
	BYTE rdsdec_af[8];      //Alternative Frequency List—AF
	/* DEF_SUPPORT_2 */
	BYTE rdsdec_rt_a[64+1]; //RadioText (RT) 这是 GROUP_A的 TEXT_A 和 TEXT_B
	BYTE brdsdec_rt_a;
	//其实只有一组有效，当改变时往另一数组写数
	BYTE rdsdec_rt_b[32+1]; // for Decode RadioText (RT) 这是 GROUP_B的 TEXT_A 和 TEXT_B
	BYTE brdsdec_rt_b;
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

#define GROUP_A     0
#define GROUP_B     1

#define DATA_BUF_LEN 100

typedef struct _FLY_SAF7741_RADIO_INFO
{
	BOOL bOpen;
	BOOL bPower;
	BOOL bPowerUp;

	BOOL bNeedInit;

	BYTE buffToUser[DATA_BUFF_LENGTH];
	UINT buffToUserHx;
	UINT buffToUserLx;
	HANDLE hBuffToUserDataEvent;
	CRITICAL_SECTION hCSSendToUser;

	BOOL bPreMute;
	BOOL bCurMute;

	HANDLE hMsgQueueToRadioCreate;

	HANDLE  Saf7741RadioMainThreadHandle;
	BOOL bKillDispatchMainThread;
	HANDLE hDispatchMainThreadEvent;

	HANDLE	Saf7741RadioScanThreadHandle;
	BOOL bKillDispatchScanThread;
	HANDLE hDispatchScanThreadEvent;

	HANDLE	Saf7741RadioRDSRecThreadHandle;
	BOOL bKillDispatchRDSRecThread;

	HANDLE	Saf7741RadioI2CHandle;
	FLY_RADIO_INFO radioInfo;
	FLY_RDS_INFO RDSInfo;


	BOOL bSocketConnecting;
	IIC_SEG_T I2c_TEF7000_1_DataBuf[7+DATA_BUF_LEN];
	IIC_SEG_T I2c_TEF7000_2_DataBuf[7+DATA_BUF_LEN];
	volatile shm_t *pFlyShmGlobalInfo;

}FLY_SAF7741_RADIO_INFO, *P_FLY_SAF7741_RADIO_INFO;

VOID ReadFromSAF7730(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,UINT regAddr,BYTE *pdata,BYTE len);

#if RADIO_RDS
	VOID ReadFromSAF7741(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,UINT regAddr,BYTE *pdata,BYTE len);
	DWORD WINAPI SAF7741RadioRDSRecThread(LPVOID pContext);
	VOID SAF7741RadioReturnToUser(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo,BYTE *buf,UINT len);
	void RDSParaInit(P_FLY_SAF7741_RADIO_INFO pSAF7741RadioInfo);
#endif


#define MSG_ENABLE_FLAG (pSAF7741RadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bDebugMsgEnable)

#if GLOBAL_DEBUG
#define DBGE(string) RETAILMSG(1,string)//重要错误
#define DBGW(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string)//警告
#define DBGI(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string)//信息
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

#endif// _FLYAUDIO_SAF7741_H_
