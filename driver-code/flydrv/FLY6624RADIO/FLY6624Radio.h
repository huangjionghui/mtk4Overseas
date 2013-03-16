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
	UINT16 rdsdec_flag_recv[2];// ��ʾ ���ڲ����Ľ�Ŀ��GROUP ��־λ �����Ŀ���� 0A �� 3B �� ��ʱrdsdec_flag_recv[0]�ĵ�һλ��1 rdsdec_flag_recv[1]�ĵ���λ��1 		
	UINT16 rdsdec_flag_pi;	 //Programme Identification ÿ����Ŀֻ��Ψһ��PI ǰ��λ�� ��������
	//B15-B12	Country code
	//B11-B8	Programme type in terms of area coverage
	//B7-B0		Programme reference number
	BYTE  rdsdec_flag_pty; //0��ʾnone����Ŀ����
	BYTE  rdsdec_flag_tp;	  //0��ʾû��Я����Traffic Programme (TP) Flag	 ��ʾ ��Ŀ���� ��ͨ��Ϣ
	BYTE  rdsdec_flag_ms;	  //0�����࣬1�����࣬ music/speech ��ʾ���ڲ����Ľ�Ŀ�� ���ֻ���������Ŀ
	BYTE  rdsdec_flag_di[4];	 //	  Decoder Identification �����־λ ��ʱû���õ�
	BYTE  rdsdec_flag_ta;	  //0��ʾ�ޣ� Traffic Announcement (TP) Flag //TP �� TA ͬʱΪ 1����ʾTraffic report in progress on this service����ʱ�ص��������� ���˱���

	BYTE  rdsdec_flag_eon;  //0��ʾ�ޣ���ʾ����Enhanced Other Networks (EON) ��Ŀ ��ʱû�� ���������
	/* DEF_SUPPORT_0 */
	UINT16   rds_ecc;	    // Extended Country Code  ������ȷ�� ĳ������ ��ΪPI��ǰ��λ ������һЩ���ҹ��õ�
	UINT16   rds_language_code;	  //���Ա���

	/* DEF_SUPPORT_0 */
	BYTE rdsdec_ps[2][8];   //Programme Service (PS) Name  ��Ŀ����
	BYTE rdsdec_ps_dis[8];
	//��ʵֻ��һ����Ч�����ı�ʱ����һ����д��
	BYTE rdsdec_af[8];      //Alternative Frequency List��AF
	/* DEF_SUPPORT_2 */
	BYTE rdsdec_rt_a[2][64]; //RadioText (RT) ���� GROUP_A�� TEXT_A �� TEXT_B
	BYTE rdsdec_rt_a_dis[64];
	//��ʵֻ��һ����Ч�����ı�ʱ����һ����д��
	BYTE rdsdec_rt_b[32]; // for Decode RadioText (RT) ���� GROUP_B�� TEXT_A �� TEXT_B
	//��ʵֻ��һ����Ч�����ı�ʱ����һ����д��

	/* DEF_SUPPORT_4 */	   //������ RDS ��ʱ�� У��
	UINT16 rds_clock_year;
	BYTE  rds_clock_month;
	BYTE  rds_clock_day;
	BYTE  rds_clock_wd; // weekday 1~7
	BYTE  rds_clock_hour;
	BYTE  rds_clock_min;
	BYTE  rds_clock_ofs;

	BYTE rdsdec_ptyn[2][8];   // for Decode �ɱ�̵Ľ�Ŀ������
	//��ʵֻ��һ����Ч�����ı�ʱ����һ����д��
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
	//RDS�߳����
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

#endif