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
	//��ʵֻ��һ����Ч�����ı�ʱ����һ����д��
	BYTE rdsdec_af[8];      //Alternative Frequency List��AF
	/* DEF_SUPPORT_2 */
	BYTE rdsdec_rt_a[64+1]; //RadioText (RT) ���� GROUP_A�� TEXT_A �� TEXT_B
	BYTE brdsdec_rt_a;
	//��ʵֻ��һ����Ч�����ı�ʱ����һ����д��
	BYTE rdsdec_rt_b[32+1]; // for Decode RadioText (RT) ���� GROUP_B�� TEXT_A �� TEXT_B
	BYTE brdsdec_rt_b;
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
#define DBGE(string) RETAILMSG(1,string)//��Ҫ����
#define DBGW(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string)//����
#define DBGI(string) if (MSG_ENABLE_FLAG) RETAILMSG(1,string)//��Ϣ
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

#endif// _FLYAUDIO_SAF7741_H_
