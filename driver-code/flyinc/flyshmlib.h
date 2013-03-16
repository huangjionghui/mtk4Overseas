#ifndef __SHM_H__
#define __SHM_H__

#include <windows.h>
#include <basetsd.h>
#include "flyglobaldef.h"



#define SHICHAN_SHICHAN	1

//������Ϣ�ܿ���
#define GLOBAL_DEBUG 1

#if GLOBAL_DEBUG
#define FLY_DEBUG_KEY		1
#define FLY_DEBUG_5150		1
#define FLY_DEBUG_7419		1
#define FLY_DEBUG_7541		1
#define FLY_DEBUG_6624      1
#define FLY_DEBUG_CARBODY	1
#define FLY_DEBUG_BT		1
#define FLY_DEBUG_EXBOX		1
#define FLY_DEBUG_DVD		1
#define FLY_DEBUG_GLOBAL	1
#define FLY_DEBUG_SYSTEM	1
#define FLY_DEBUG_TPMS		1
#define FLY_DEBUG_TV		1
#define FLY_DEBUG_AC        1
#define FLY_DEBUG_DIS       1
#elif
#define FLY_DEBUG_KEY		0
#define FLY_DEBUG_5150		0
#define FLY_DEBUG_7419		0
#define FLY_DEBUG_7541		0
#define FLY_DEBUG_6624      0
#define FLY_DEBUG_CARBODY	0
#define FLY_DEBUG_BT		0
#define FLY_DEBUG_EXBOX		0
#define FLY_DEBUG_DVD		0
#define FLY_DEBUG_GLOBAL	0
#define FLY_DEBUG_SYSTEM	0
#define FLY_DEBUG_TPMS		0
#define FLY_DEBUG_TV		0
#define FLY_DEBUG_AC        0
#define FLY_DEBUG_DIS       0
#endif

#if SHICHAN_SHICHAN
#define EJECT_LED_ON	0
#define EJECT_LED_OFF	1
#else
#define EJECT_LED_ON	1
#define EJECT_LED_OFF	0
#endif

//#define REGEDIT_NOT_READ
//#define REGEDIT_NOT_SAVE

//��������Ӳ�����
#define PCB_BOARD_V2	(1<<2)//�Բ�100̨����
//#define PCB_BOARD_V1	(1<<1)//�з��׶εĻ���

#define PCB_NOW_USE	PCB_BOARD_V2	//��ʱû�����ô����ʹ�������������

//��������Ӳ�����

#define SUWEI_KEY_TEST				L"SOC_DBG_ON"
#define SUWEI_POWERDOWN_REQ			L"SOC_PWRDDN"
#define SUWEI_POWERDOWN_ACK			L"SOC_PWRDDN_ACK"
#define SUWEI_POWERDOWN_REC			L"SOC_PWRDDN_RDY"

#define FLY_MSG_QUEUE_FLAG						MSGQUEUE_NOPRECOMMIT

//�ߵ�
#define FLY_MSG_QUEUE_TO_DRVIR_NAME				TEXT("TO_DRVIR_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_FROM_DRVIR_NAME			TEXT("FROM_DRVIR_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_TO_DRVIR_LENGTH			(32)
#define FLY_MSG_QUEUE_TO_DRVIR_ENTRIES			(256)
#define FLY_MSG_QUEUE_FROM_DRVIR_LENGTH			(32)
#define FLY_MSG_QUEUE_FROM_DRVIR_ENTRIES		(256)

#define FLY_MSG_QUEUE_TO_CANVIR_NAME			TEXT("TO_CANVIR_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_FROM_CANVIR_NAME			TEXT("FROM_CANVIR_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_TO_CANVIR_LENGTH			(32)
#define FLY_MSG_QUEUE_TO_CANVIR_ENTRIES			(256)
#define FLY_MSG_QUEUE_FROM_CANVIR_LENGTH		(32)
#define FLY_MSG_QUEUE_FROM_CANVIR_ENTRIES		(256)

#define FLY_MSG_QUEUE_TO_GLOBAL_NAME	TEXT("TO_GLOBAL_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_TO_GLOBAL_LENGTH	(2+1+255)
#define FLY_MSG_QUEUE_TO_GLOBAL_ENTRIES	(32)

#define FLY_MSG_QUEUE_TO_KEY_NAME		TEXT("TO_KEY_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_FROM_KEY_NAME		TEXT("FROM_KEY_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_TO_KEY_LENGTH		(1)
#define FLY_MSG_QUEUE_TO_KEY_ENTRIES	32
#define FLY_MSG_QUEUE_FROM_KEY_LENGTH	(1)
#define FLY_MSG_QUEUE_FROM_KEY_ENTRIES	32

#define FLY_MSG_QUEUE_TO_RADIO_NAME		TEXT("TO_RADIO_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_TO_RADIO_LENGTH		(8)
#define FLY_MSG_QUEUE_TO_RADIO_ENTRIES		32

#define FLY_MSG_QUEUE_TO_SYSTEM_NAME			TEXT("TO_SYSTEM_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_FROM_SYSTEM_NAME			TEXT("FROM_SYSTEM_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_TO_SYSTEM_LENGTH			(2+1+255)
#define FLY_MSG_QUEUE_TO_SYSTEM_ENTRIES			32
#define FLY_MSG_QUEUE_FROM_SYSTEM_LENGTH		(2+1+255)
#define FLY_MSG_QUEUE_FROM_SYSTEM_ENTRIES		32

//AC
#define FLY_MSG_QUEUE_TO_AC_NAME				TEXT("TO_AC_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_FROM_AC_NAME				TEXT("FROM_AC_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_TO_AC_LENGTH				(2+1+255)
#define FLY_MSG_QUEUE_TO_AC_ENTRIES				32
#define FLY_MSG_QUEUE_FROM_AC_LENGTH			(2+1+255)
#define FLY_MSG_QUEUE_FROM_AC_ENTRIES			32

//FAIII
#define FLY_MSG_QUEUE_TO_FAIII_NAME				TEXT("TO_FAIII_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_FROM_FAIII_NAME			TEXT("FROM_FAIII_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_TO_FAIII_LENGTH			(2+1+255)
#define FLY_MSG_QUEUE_TO_FAIII_ENTRIES			32
#define FLY_MSG_QUEUE_FROM_FAIII_LENGTH			(2+1+255)
#define FLY_MSG_QUEUE_FROM_FAIII_ENTRIES		32


#define FLY_MSG_QUEUE_FROM_ASSISTDISPLAY_NAME		TEXT("FROM_ASSISTDISPLAY_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_FROM_ASSISTDISPLAY_LENGTH		(2+1+255)
#define FLY_MSG_QUEUE_FROM_ASSISTDISPLAY_ENTRIES	32

#define FLY_MSG_QUEUE_TPMS_TO_EXBOX_NAME		TEXT("TPMS_TO_EXBOX_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_EXBOX_TO_TPMS_NAME		TEXT("EXBOX_TO_TPMS_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_TPMS_TO_EXBOX_LENGTH		(6)
#define FLY_MSG_QUEUE_TPMS_TO_EXBOX_ENTRIES		32
#define FLY_MSG_QUEUE_EXBOX_TO_TPMS_LENGTH		(6)
#define FLY_MSG_QUEUE_EXBOX_TO_TPMS_ENTRIES		32

#define FLY_MSG_QUEUE_TV_TO_EXBOX_NAME			TEXT("TV_TO_EXBOX_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_EXBOX_TO_TV_NAME			TEXT("EXBOX_TO_TV_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_TV_TO_EXBOX_LENGTH		(6)
#define FLY_MSG_QUEUE_TV_TO_EXBOX_ENTRIES		32
#define FLY_MSG_QUEUE_EXBOX_TO_TV_LENGTH		(6)
#define FLY_MSG_QUEUE_EXBOX_TO_TV_ENTRIES		32

//FLYAUDIO AMP
#define FLY_MSG_QUEUE_TO_EXTAMP_NAME			TEXT("TO_EXTAMP_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_FROM_EXTAMP_NAME			TEXT("FROM_EXTAMP_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_TO_EXTAMP_LENGTH			(2+1+255)
#define FLY_MSG_QUEUE_TO_EXTAMP_ENTRIES			32
#define FLY_MSG_QUEUE_FROM_EXTAMP_LENGTH		(2+1+255)
#define FLY_MSG_QUEUE_FROM_EXTAMP_ENTRIES		32

//CAR AMP
#define FLY_MSG_QUEUE_TO_CARAMP_NAME			TEXT("TO_CARAMP_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_FROM_CARAMP_NAME			TEXT("FROM_CARAMP_NAME_FLY_MSG_QUEUE")
#define FLY_MSG_QUEUE_TO_CARAMP_LENGTH			(2+1+255)
#define FLY_MSG_QUEUE_TO_CARAMP_ENTRIES			32
#define FLY_MSG_QUEUE_FROM_CARAMP_LENGTH		(2+1+255)
#define FLY_MSG_QUEUE_FROM_CARAMP_ENTRIES		32

//�¼���˭��Ӧ������¼��͹�����˭
typedef enum{
	EVENT_GLOBAL_DATA_CHANGE_VOLUME,
	EVENT_GLOBAL_BACK_LOW_VOLUME_ID,//������������
	EVENT_GLOBAL_BT_MUTE_IN_REQ_ID,//���뾲��   //�����¼�����
	EVENT_GLOBAL_BT_MUTE_OUT_REQ_ID,//�˳�����
	EVENT_GLOBAL_BT_MUTE_IN_OK_ID,
	EVENT_GLOBAL_BTCALLSTATUS_CHANGE_ID,//BT�绰״̬
	EVENT_GLOBAL_PHONECALLSTATUS_CHANGE_ID,
	EVENT_GLOBAL_CAR_VOL_UP,
	EVENT_GLOBAL_CAR_VOL_DOWN,
	EVENT_GLOBAL_CAR_VOL_MUTE,
	EVENT_GLOBAL_STANDBY_AUDIO_ID,
	EVENT_GLOBAL_SLEEP_PROC_AUDIO_ID,//˯���¼�
	EVENT_GLOBAL_BATTERY_RECOVERY_AUDIO_ID,
	EVENT_GLOBAL_RADIO_MUTE_IN_REQ_ID,
	EVENT_GLOBAL_RADIO_MUTE_OUT_REQ_ID,
	EVENT_GLOBAL__CARAMP_CHANGE_ID,
	EVENT_GLOBAL_BACK_VIDEO_SIGNED_ID,
	EVENT_GLOBAL_GPS_VOLUME_LEVEL_ID,//�����ȼ�����
	IPC_AUDIO_MAX_EVENT,//����Ϊaudio�¼� 


	EVENT_GLOBAL_AUX_CHECK_START_ID,//AUX�����¼�
	EVENT_GLOBAL_SLEEP_PROC_VIDEO_ID,
	EVENT_GLOBAL_STANDBY_VIDEO_ID,
	EVENT_GLOBAL_BACKDETECT_CHANGE_ID,
	EVENT_GLOBAL_BACK_EXIT_DEPENDON_LPC_ID,//LPC֪ͨ�˳�����
	IPC_VIDEO_MAX_EVENT,//����Ϊvideo�¼�  


	EVENT_GLOBAL_PANELKEY_USE_IT_ID,
	EVENT_GLOBAL_REMOTE_STUDY_START_ID,
	EVENT_GLOBAL_REMOTE_STUDY_STOP_ID,
	EVENT_GLOBAL_REMOTE_STUDY_CLEAR_ID,
	EVENT_GLOBAL_REMOTE_USE_IT_ID,
	EVENT_GLOBAL_REMOTE_STUDY_PRE_RETURN_WAIT_ID,
	EVENT_GLOBAL_REMOTE_STUDY_PRE_RETURN_FINISH_ID,
	EVENT_GLOBAL_DEMO_KEY_START_ID,
	EVENT_GLOBAL_SLEEP_PROC_KEY_ID,
	EVENT_GLOBAL_REG_RESTORE_KEY_ID,				//ע������¼�
	IPC_KEY_MAX_EVENT,//����Ϊkey�¼�  


	//Ӧ�ó����Ping
	EVENT_GLOBAL_USER_PING_START_ID,
	EVENT_GLOBAL_USER_PING_WORK_ID,
	EVENT_GLOBAL_RETURN_FLASH_DATA_ID,	//System����Flash���ݵ��¼�
	EVENT_GLOBAL_RETURN_DVD_VERSION_ID,
	EVENT_GLOBAL_RETURN_BT_VERSION_ID,
	EVENT_GLOBAL_RETURN_BAR_CODE_ID,
	EVENT_GLOBAL_RADIO_AD_REQUEST_ID,//AUX�����¼�
	EVENT_GLOBAL_AUX_CHECK_RETURN_ID,//AUX�����¼�
	EVENT_GLOBAL_POWERON_MCU_INIT_ID,
	EVENT_GLOBAL_TVBOX_EXIST_ID,//�ⲿTVС�д����¼�
	EVENT_GLOBAL_TPMSBOX_EXIST_ID,//�ⲿTPMSС�д����¼�
	EVENT_GLOBAL_REMOTE_RETURN_IT_ID,
	EVENT_GLOBAL_BACKDETECT_RETURN_ID,
	EVENT_GLOBAL_INNER_AMP_ON_ID,
	EVENT_GLOBAL_PHONEDETECT_CHANGE_ID,
	EVENT_GLOBAL_KEY_STANDBY_ID,                  //-----------49����
	EVENT_GLOBAL_REMOTE_STUDY_RETURN_WAIT_ID,
	EVENT_GLOBAL_REMOTE_STUDY_RETURN_START_ID,
	EVENT_GLOBAL_REMOTE_STUDY_RETURN_FINISH_ID,
	EVENT_GLOBAL_BREAKDETECT_CHANGE_ID,
	EVENT_GLOBAL_REG_TO_SYSTEM_ID,		//���Ѻ��ͬ��
	EVENT_GLOBAL_SILENCE_POWER_UP_ID,	//͵͵������
	EVENT_GLOBAL_FORCE_RESET_ID,		//ǿ�Ƹ�λ
	EVENT_GLOBAL_IIC_MCU_ID,			//����
	EVENT_GLOBAL_LPC_UPDATE,			//LPC�����¼�
	EVENT_GOBAL_BACKACTION_READY,		//֪ͨLPC�Ѿ�׼���õ���
	EVENT_GOBAL_BACKACTION_PWM,
	IPC_SYSTEM_MAX_EVENT,				//����Ϊsystem�¼�   


	EVENT_GLOBAL_RADIO_ANTENNA_ID, 
	EVENT_GLOBAL_RADIO_CHANGE_ID,
	EVENT_GLOBAL_REG_RESTORE_RADIO_ID,				//ע������¼�
	EVENT_GLOBAL_BATTERY_RECOVERY_RADIO_ID,
	EVENT_GLOBAL_RADIO_MUTE_IN_OK_ID,
	IPC_RADIO_MAX_EVENT,//����Ϊradio�¼�  



	EVENT_GLOBAL_REMOTE_CHANGE_IT_ID,
	EVENT_GLOBAL_DEMO_OSD_START_ID,//OSD������Ϣ��ʾ�¼�
	EVENT_GLOBAL_DEMO_OSD_DISPLAY_ID,
	EVENT_GLOBAL_VLAULE_TO_USER,//������OSD��ʾ
	IPC_GLOBAL_MAX_EVENT,//����Ϊglobal�¼�


	EVENT_GLOBAL_STANDBY_BT_ID,
	EVENT_GLOBAL_SLEEP_PROC_BT_ID,
	EVENT_GLOBAL_A2DPAUDIO_CHANNEL_ID,//���������Ƿ���A2DPͨ��
	IPC_BT_MAX_EVENT,//����ΪBT�¼�

	EVENT_GLOBAL_STANDBY_DVD_ID,
	EVENT_GLOBAL_DVD_REGION_SET_ID,	//�����������¼�
	EVENT_GLOBAL_ERROR_LEDFLASH_ID,	//LED��˸ָʾ����
	EVENT_GLOBAL_DVD_STOP_ID,		//DVDͣ������
	EVENT_BT_OR_RING_STATUS_ID,		//���������ֹDVD��Ƶ
	EVENT_GLOBAL_DVD_CHANNEL_ID,
	IPC_DVD_MAX_EVENT,//����ΪDVD�¼�


	EVENT_GLOBAL_LPC_UPDATA_FLAG,//��Ӧ�ò���¼�
	IPC_CARBODY_MAX_EVENT,

	//exbox
	//�ⲿCDCС���л��¼�
	EVENT_GLOBAL_EXBOX_INPUT_CHANGE_ID,
	EVENT_GLOBAL_GOT_CARNAME_ID,
	IPC_EXBOX_MAX_EVENT,


	IPC_TPMS_MAX_EVENT,


	IPC_TV_MAX_EVENT,





	IPC_AC_MAX_EVENT,

	//����С��
	EVENT_GLOBAL_DEALED_CARNAME_ID,
	IPC_DIS_MAX_EVENT,


	IPC_CRARECORD_MAX_EVENT,
	//δ���� 

	EVENT_GLOBAL_FORCE_LCD_LIGHT_ON_ID,

	IPC_EVENT_MAX
}IPC_EVENT_T;


/*
 *
 *
 *˳������
 *
 */
typedef enum{
	AUDIO_MODULE,
	VIDEO_MODULE,
	KEY_MODULE,
	SYSTEM_MODULE,
	RADIO_MODULE,
	GLOBAL_MODULE,
	BT_MODULE,
	DVD_MODULE,
	CARBODY_MODULE,
	EXBOX_MODULE,
	TPMS_MODULE,
	TV_MODULE,
	AC_MODULE,
	DIS_MODULE,
	CRARECORD_MODULE,
	MODULE_MAX
}MODULE_ID_T;

//һ��Ҫ�� MODULE_ID_T �໥��Ӧ,����������������LOGO
const LPCWSTR StrIpcEventNameTable[MODULE_MAX]={
	TEXT("AUDIO_MODULE_EVENT_NAME"),
	TEXT("VIDEO_MODULE_EVENT_NAME"),
	TEXT("KEY_MODULE_EVENT_NAME"),
	TEXT("SYSTEM_MODULE_EVENT_NAME"),
	TEXT("RADIO_MODULE_EVENT_NAME"),
	TEXT("GLOBAL_MODULE_EVENT_NAME"),
	TEXT("BT_MODULE_EVENT_NAME"),
	TEXT("DVD_MODULE_EVENT_NAME"),
	TEXT("CARBODY_MODULE_EVENT_NAME"),
	TEXT("EXBOX_MODULE_EVENT_NAME"),
	TEXT("TPMS_MODULE_EVENT_NAME"),
	TEXT("TV_MODULE_EVENT_NAME"),
	TEXT("AC_MODULE_EVENT_NAME"),
	TEXT("DIS_MODULE_EVENT_NAME"),
	TEXT("CRARECORD_MODULE_EVENT_NAME"),
};
const UINT32 ModuleEventTable[MODULE_MAX]={
	IPC_AUDIO_MAX_EVENT,
	IPC_VIDEO_MAX_EVENT,
	IPC_KEY_MAX_EVENT,
	IPC_SYSTEM_MAX_EVENT,
	IPC_RADIO_MAX_EVENT,
	IPC_GLOBAL_MAX_EVENT,
	IPC_BT_MAX_EVENT,
	IPC_DVD_MAX_EVENT,
	IPC_CARBODY_MAX_EVENT,
	IPC_EXBOX_MAX_EVENT,
	IPC_TPMS_MAX_EVENT,
	IPC_TV_MAX_EVENT,
	IPC_AC_MAX_EVENT,
	IPC_DIS_MAX_EVENT,
	IPC_CRARECORD_MAX_EVENT,
};
/*********************************************************************************/

//����DLL�¼���ӳ����
typedef void (*fIpcFunction)(UINT32 ipcEvent);
typedef void (*fIrqFunction)(BYTE *buf, UINT16 len);
typedef void (*fSockRecvFunction)(BYTE *buf, UINT16 len);



#define COLOR_STEP_COUNT	11	//��Ƶ���ڵȼ�
#define RADIO_COUNTRY_ID	6	//��������������
#define IRKEYTABSIZE	0xFF	//�����̴�С
#define IRKEY_STUDY_COUNT	9
#define BACK_VIDEO_ACTIVE_DELAY_TIME	618	//������Ƶ��ʱ����
#define BACK_VIDEO_DISAPPEAR_DELAY_TIME	618	//������Ƶ��ʧ��ʱ����

#define MCU_SOFT_VERSION_MAX	64

#define MODULE_SOFT_VERSION_MAX	64

enum eLedFlashError{eLEDNormal=0,eLEDPreSleepTimeOut,eLEDEnterSleepTimeOut,eLEDControlMCUSleepTimeOut};

enum audio_source{Init=0,	MediaCD,	CDC,	RADIO,
				   AUX,		IPOD,		TV,		MediaMP3,
				   SRADIO,	A2DP,		EXT_TEL,GR_AUDIO,
				   BT,		BACK,		GPS,	BT_RING,
				   CAR_RECORDER= 0x11,  VAP};

enum eBTCALL_S {BtCALL_IDEL=0,BtCALL_IN,BtCALL_OUT,BtCALL_ON};



#define NORMAL_VOLUME_MAX    60
#define NORMAL_VOLUME_DEFAULT 30
#define NORMAL_VOLUME_AMP    34
#define NORMAL_VOLUME_BACK_LOWEST 10

#define CAR_NORMAL		  0
#define CAR_TUGUAN_07B_9  1
#define CAR_IX35_88_1	  2
#define CAR_CRV_12		  3
#define CAR_ZHISHENG	  4
#define CAR_CAMRY_12	  5
#define CAR_OUTLANDER	  6
#define CAR_TUGUAN_07B_17 7
#define CAR_BENTENG_B50   8
#define CAR_TUGUAN_07B_16 9
#define CAR_FOCUS		  10
#define CAR_MAZDA_6		  11



typedef struct _FLY_GLOBAL_DRIVER_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;
}FLY_GLOBAL_DRIVER_INFO, *P_FLY_GLOBAL_DRIVER_INFO;

typedef struct _FLY_GLOBAL_SYSTEM_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;

}FLY_GLOBAL_SYSTEM_INFO, *P_FLY_GLOBAL_SYSTEM_INFO;

typedef struct _FLY_GLOBAL_IPOD_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;
}FLY_GLOBAL_IPOD_INFO, *P_FLY_GLOBAL_IPOD_INFO;

typedef struct _FLY_GLOBAL_AUDIO_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;

}FLY_GLOBAL_AUDIO_INFO, *P_FLY_GLOBAL_AUDIO_INFO;

typedef struct _FLY_GLOBAL_RADIO_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;

}FLY_GLOBAL_RADIO_INFO, *P_FLY_GLOBAL_RADIO_INFO;

typedef struct _FLY_GLOBAL_VIDEO_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;
}FLY_GLOBAL_VIDEO_INFO, *P_FLY_GLOBAL_VIDEO_INFO;

typedef struct _FLY_GLOBAL_TEST_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;
}FLY_GLOBAL_TEST_INFO, *P_FLY_GLOBAL_TEST_INFO;

typedef struct {
	BYTE valid[2];  //�Ƿ��Ѿ���ʼ��  55h AAh
	BYTE type;	 //���汾��0x01 �������� 3��7����
	BYTE PanelName[8];//�������  �硰AV6000XX��
	BYTE slid;	//01  ����	    00 �̶�
	BYTE EK_Value[4];   //���α�ʾ 0x80~0x83��ֵ��Ӧ��ϵ
	BYTE KeyValue[30]; //��Ӧ����е�1��21��ֵ��Ӧ����ֵ ,Ԥ�����ֿռ�9BYTESΪ�Ժ�����
	UINT16 LCD_RGB_Wide;	   
	UINT16 LCD_RGB_High;	   
}tPANEL_TAB;

typedef struct _FLY_GLOBAL_KEY_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;

	tPANEL_TAB tPanelTab;
}FLY_GLOBAL_KEY_INFO, *P_FLY_GLOBAL_KEY_INFO;

typedef struct _FLY_GLOBAL_BT_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;
}FLY_GLOBAL_BT_INFO, *P_FLY_GLOBAL_BT_INFO;

typedef struct _FLY_GLOBAL_DVD_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;
}FLY_GLOBAL_DVD_INFO, *P_FLY_GLOBAL_DVD_INFO;

typedef struct _FLY_GLOBAL_ASSISTDISPLAY_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;
}FLY_GLOBAL_ASSISTDISPLAY_INFO, *P_FLY_GLOBAL_ASSISTDISPLAY_INFO;

typedef struct _FLY_GLOBAL_CARBODY_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;
}FLY_GLOBAL_CARBODY_INFO, *P_FLY_GLOBAL_CARBODY_INFO;

typedef struct _FLY_GLOBAL_XMRADIO_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;
}FLY_GLOBAL_XMRADIO_INFO, *P_FLY_GLOBAL_XMRADIO_INFO;

typedef struct _FLY_GLOBAL_EXBOX_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;
}FLY_GLOBAL_EXBOX_INFO, *P_FLY_GLOBAL_EXBOX_INFO;
typedef struct _FLY_GLOBAL_TPMS_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;
}FLY_GLOBAL_TPMS_INFO, *P_FLY_GLOBAL_TPMS_INFO;
typedef struct _FLY_GLOBAL_TV_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;
}FLY_GLOBAL_TV_INFO, *P_FLY_GLOBAL_TV_INFO;

typedef struct _FLY_GLOBAL_FAIII_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;
}FLY_GLOBAL_FAIII_INFO, *P_FLY_GLOBAL_FAIII_INFO;

typedef struct _FLY_GLOBAL_AC_INFO
{
	BOOL bInit;
	BOOL bOpen;
	BYTE debugMsgLevel;

	UINT iDriverCompYear;
	BYTE iDriverCompMon;
	BYTE iDriverCompDay;
	BYTE iDriverCompHour;
	BYTE iDriverCompMin;
	BYTE iDriverCompSec;
}FLY_GLOBAL_AC_INFO, *P_FLY_GLOBAL_AC_INFO;
//ע�������Ϣ
#define REG_SAVE_FLAG 0x589246
struct RegSaveInfo{
	UINT32 RegSaveFlag;//�Ƿ񱣴�

	BYTE sErrorDriverName[3];

	BOOL bSystemResetUseExtConfig;//ʹ���ⲿ����

	UINT iSystemResetAtLeastDays;//���ٶ�����
	UINT iSystemResetOnHour;//��ʲôʱ
	UINT iSystemResetOnMinute;//��ʲô��
	UINT iSystemResetInnerMin;//�ػ������ٶ��ٷ���

	UINT iSystemResetPowerOffMin;//���������೤ʱ��������͹���

	UINT iSystemResetCanRunLess;//���������ж೤ʱ��

	//CRITICAL_SECTION hCSGlobalDataChange;
	//BOOL iGlobalDataChangeWhatStart[EVENT_GLOBAL_DATA_CHANGE_MAX + 1];
	//BOOL iGlobalDataChangeWhatExchange[EVENT_GLOBAL_DATA_CHANGE_MAX + 1];
	//ULONG iGlobalDataChangeWhatStartTime[EVENT_GLOBAL_DATA_CHANGE_MAX + 1];
	//HANDLE iGlobalDataChangeWhatHandle[EVENT_GLOBAL_DATA_CHANGE_MAX + 1];
	//BOOL iGlobalDataChangeWhatOn;

	BOOL bMute;
	BYTE iVolume;
	BYTE iVolumeMin;
	BYTE iVolumeMax;
	BOOL bVolumeMaxReady;
	BYTE iFlyAudioAmpVolume;
	BYTE iBackLowestVolume;//����ʱ�������ֵ�����ڴ�ֵʱ�������������͵���ֵ��С�����ֵʱ����ά�ֲ���
	BYTE iGPSVolumeLevel;

	//������Ϣ����Ӧ
	BOOL bIngoreIPCEventMsgResponse;

	//������͵͵������
	BOOL bSilencePowerUp;

	//�Ƿ���TVģ��
	BOOL bHaveTVModule;
	//�Ƿ���TPMSģ��
	BOOL bHaveTPMSModule;

	//�Ƿ��зɸ��ⲿ����
	BOOL bHaveFlyAudioExtAMP;
	//�յ���λ�ź�
	BOOL bHaveReceiveResetMsg;

	//ACC OFF
	BOOL bNeedWinCEPowerOff;//�������յ�ACC OFF
	BOOL bRecWinCESleepMsg;//���յ��ػ�
	BOOL bRecWinCEFactoryMsg;//���յ��ָ�����ģʽ
	BOOL bRecWinCERestartMsg;//���յ�����
	//ACC OFF���ٷ�������
	BOOL bNoMoreToSendDataWhenToSleep;
	//����
	BOOL bStandbyStatus;
	BOOL bStandbyStatusLast;
	BOOL bStandbyStatusWithACCOff;
	//OSD DEMOģʽ
	BOOL bOSDDemoMode;
	//����DEMOģʽ
	BOOL bKeyDemoMode;
	//��Դ��ѹ
	BYTE iBatteryVoltage;
	//�Ƿ����
	BOOL bBatteryVoltageLowAudio;
	BOOL bBatteryVoltageLowRadio;

	//�����¶�
	INT iHostTemperature;

	//�������״̬
	BOOL bMCUControlLCDLightOn;

	//��ǰͨ��
	BYTE eAudioInput;
	BYTE eVideoInput;

	BYTE eCurAudioInput;//��ǰʵ��ͨ��

	BYTE GPSSpeaker;

	//�绰���
	BOOL bExtPhoneStatusIO;
	BOOL bExtPhoneStatusBUS;
	//ɲ��
	BOOL bBreakStatusIO;
	BOOL bBreakStatusBUS;
	//����
	BOOL bBackDetectEnable;
	BOOL bBackActiveNow;
	BOOL bBackVideoOn;
	BOOL bBackActioning;

	//�ƹ�
	BOOL bLightDetectEnable;
	BOOL bDayNight;
	BYTE iLCDLightBrightDay;
	BYTE iLCDLightBrightNight;
	BYTE iPanelLightBright;
	//����
	BYTE iRadioIDUser;
	BYTE iRadioIDDriver;
	UINT iFreqFMMin;
	UINT iFreqFMMax;
	UINT iFreqAMMin;
	UINT iFreqAMMax;
	BOOL bNeedSupplyAntPower;
	BYTE iPreRadioChannel;

	//�Ƿ����ⲿ���ţ������ף�
	BOOL bHaveExtAmplifier;
	//�Ƿ��ȡ����ʱ��д������
	BOOL bReadUpdateData;
	//������
	BOOL bSteerWheelOn;
	UINT iRemoteDataCount;//����N
	UINT iRemoteDataUseWhat;//���0~N-1
	BYTE sRemoteDataName[256];
	UINT iRemoteDataNameLength;
	BYTE sRemoteData[1+2+5*IRKEYTABSIZE];
	BOOL bRemoteDataHave;
	//������ѧϰ
	BYTE iRemoteStudyClearID[IRKEY_STUDY_COUNT];
	BYTE iRemoteStudyID;
	BOOL bRemoteUseStudyOn;

	BYTE iVideoParaColor;
	BYTE iVideoParaHue;
	BYTE iVideoParaContrast;
	BYTE iVideoParaBrightness;

	//����
	BYTE iDVDRegionCode;
	BYTE iDVDWantPlayDevice;
	BOOL bDVDBTStatus;

	//����
	BYTE iBTCallStatus;
	BYTE iBTCallVolume;
	//�ⲿ�绰
	BYTE iExtTelCallStatus;
	BYTE iExtTelVolume;
	//����ʱ������
	BYTE iNormalVolume;
	BOOL bNormalMute;
	//�Ƿ�������ʱ�䣬һЩ���⳵���õ�
	BYTE bTimeSetupMode;

	//AUX
	BOOL bAUXHaveVideo;

	//��MCU��ȡRadioADֵ
	BOOL bRadioADReturn;
	UINT iRadioAD;

	//�͹��ġ��������ָ����������¼�
	BOOL bNeedProcBeforeAudioSleep;//��ƵҪ����
	BOOL bHaveProcBeforeAudioSleep;
	BOOL bNeedProcBeforeVideoSleep;//��ƵҪ�л���
	BOOL bHaveProcBeforeVideoSleep;
	BOOL bNeedProcBeforeBTSleep;//�����ָ���������ʱҪ��������Ϣ
	BOOL bHaveProcBeforeBTSleep;
	BOOL bNeedProcBeforeKeySleep;//����Ҫ���ж�
	BOOL bHaveProcBeforeKeySleep;

	//ע������¼�
	BOOL bRestoreRegeditToFactory;//�ָ���������
	BOOL bNeedRestoreRegeditRadio;//����Ҫ�������
	BOOL bNeedRestoreRegeditKey;//����Ҫ�������
	//�͵�ѹ�¼�
	UINT iNeedProcVoltageShakeRadio;//0Ϊ�رգ����࿴Ӳ��
	UINT iNeedProcVoltageShakeAudio;//0Ϊ�رգ����࿴Ӳ��

	//�����¼�
	BOOL bAudioMuteControlable;//��Ƶ�������Ծ���

	//MCU�汾��
	BYTE iMCUSoftVersion[MCU_SOFT_VERSION_MAX];
	UINT iMCUSoftVersionLength;
	//���ִ���
	BOOL bCheckShellBabyError;
	//DEMOģʽ��
	UINT iLEDTestFlashCount;
	UINT iLEDTestFlashCountSub;
	//ACCOffLightOn
	BOOL bACCOffLightOn;
	//�����ļ��Ƿ��гɹ���SD��
	BOOL bSuccessReadDebugFileFromSDMEM;

	BYTE iDVDSoftwareVersion[MODULE_SOFT_VERSION_MAX];
	UINT iDVDSoftwareVersionLength;
	BYTE iBTSoftwareVersion[MODULE_SOFT_VERSION_MAX];
	UINT iBTSoftwareVersionLength;
	UINT iPanelKeyAD[3];
	UINT iSteelAD[2];
	BYTE iKeyIndex;
	BYTE iKeyValue;
	UINT iKeyCount;

	//���ͺ�
	UINT iCarName;
	//ԭ���Ƿ������
	BOOL bCarHaveAmp;

	//LPC������־
	BOOL bLpcUpdate;
	BOOL bLpcUpdateFlag;

	//
	UINT32 IICSpeadOnAudio;

	UINT iIICErrorOnAudio;
	UINT iIICErrorOnRadio;
	UINT iIICErrorOnMCU;

	BOOL bRecUserPingStart;

	UINT iGlobalStructSize;

	//������Ϣ����
	BOOL bDebugMsgEnable;
};


typedef struct _FLY_DRIVER_GLOBAL
{
	//System Running Info
	//FLY_SYSTEM_RUNNING_INFO FlySystemRunningInfo;
	struct RegSaveInfo FlySystemRunningInfo;

	//PCB_VERSION
	DWORD PCB_Version;

	BYTE BarCodeID[256];
	//Global Driver
	FLY_GLOBAL_DRIVER_INFO FlyGlobalDriver;
	//Test
	FLY_GLOBAL_TEST_INFO FlyGlobalTestInfo;
	//System
	FLY_GLOBAL_SYSTEM_INFO FlyGlobalSystemInfo;
	//Key
	FLY_GLOBAL_KEY_INFO FlyGlobalKeyInfo;
	//BT
	FLY_GLOBAL_BT_INFO FlyGlobalBTInfo;
	//DVD
	FLY_GLOBAL_DVD_INFO FlyGlobalDVDInfo;
	//iPod
	FLY_GLOBAL_IPOD_INFO FlyGlobaliPodInfo;

	//Audio
	FLY_GLOBAL_AUDIO_INFO FlyGlobalAudioInfo;
	//Video
	FLY_GLOBAL_VIDEO_INFO FlyGlobalVideoInfo;
	//Radio
	FLY_GLOBAL_RADIO_INFO FlyGlobalRadioInfo;

	//Carbody
	FLY_GLOBAL_CARBODY_INFO FlyGlobalCarbodyInfo;
	//AssistDisplay
	FLY_GLOBAL_ASSISTDISPLAY_INFO FlyGlobalAssistDisplayInfo;
	//XMRadio
	FLY_GLOBAL_XMRADIO_INFO FlyGlobalXMRadioInfo;

	//EXBOX
	FLY_GLOBAL_EXBOX_INFO FlyGlobalExBoxInfo;
	//TPMS
	FLY_GLOBAL_TPMS_INFO FlyGlobalTPMSInfo;
	//TV
	FLY_GLOBAL_TV_INFO FlyGlobalTVInfo;
	// FA_III
	FLY_GLOBAL_FAIII_INFO FlyGlobalFaiiiInfo;

	//AC
	FLY_GLOBAL_AC_INFO FlyGlobalAcInfo;
	//....


} FLY_DRIVER_GLOBAL, *PFLY_DRIVER_GLOBAL;


typedef struct shm{

	BOOL bIpcEventTable[IPC_EVENT_MAX];
	FLY_DRIVER_GLOBAL sFlyDriverGlobalInfo;
	
}shm_t;


shm_t *CreateShm(MODULE_ID_T eModuleID,fIpcFunction fIpcFun);
void IpcStartEvent(UINT32 iEvent);
void IpcClearEvent(UINT32 iEvent);
BOOL IpcWhatEvent(UINT32 iEvent);
void FreeShm(void);

//ע������
BOOL WriteCurRegValues(LPCWSTR lpSubKey,LPTSTR lpValueName,void *pData,UINT32 size);
BOOL ReadCurRegValues(LPCWSTR lpSubKey, LPTSTR lpValueName,void *pData,UINT32 size);
BOOL WriteRegValues(HKEY hKeyType, LPCWSTR lpSubKey, LPTSTR lpValueName,void *pData,UINT32 size);
BOOL ReadRegValues(HKEY hKeyType,LPCWSTR lpSubKey, LPTSTR lpValueName,void *pData,UINT32 size);

VOID PrintErrMsg(LPCWSTR str);


#define  KB_NP						0
#define  KB_SLEEP					1

#define  KB_MENU					2
#define  KB_AV						3			 
#define  KB_DVD						4
#define  KB_CDC						5
#define  KB_RADIO					6
#define  KB_IPOD					7
#define  KB_BT 						8
#define  KB_AC						9
#define  KB_TV						0x0a
#define  KB_FM1						0x0b
#define  KB_FM2						0x0c
#define  KB_AM						0x0d
#define  KB_FM						0x0e
//	#define  KB_AV_DEC					0x0f			 //20081216

#define  KB_NAVI					0x10
#define  KB_DEST					0x11
#define  KB_LOCAL					0x12
#define  KB_SETUP					0x13
#define  KB_INFO					0x14

#define  KB_CALL					0x31
#define  KB_CALL_REJECT				0x32
#define  KB_DIMM					0x33
#define  KB_DVD_OPEN				0x34	  //DVD��������Ƽ�
#define  KB_VOL_INC					0x35	  //ÿ��������1
#define  KB_VOL_DEC					0x36	  //ÿ��������1
#define  KB_MUTE					0x37	  //3S˯��
#define  KB_SEEK_INC				0x38	  //0��3��
#define  KB_SEEK_DEC				0x39	  //0.3��
#define  KB_TUNE_DOWN				0x3a
#define  KB_TUNE_INC				0x3b	  //0.2S
#define  KB_TUNE_DEC				0x3c	  //0.2S
#define  KB_SEEK_INC2				0x3d	  //0.3S
#define  KB_SEEK_DEC2				0x3e	  //0.3S
#define  KB_PANEL_ANGLE				0x3f	  //���ǻ���һ��һ�λ����׵�OPEN��
#define  KB_PANEL_OPEN				0x40	  //���ǻ���һ��һ�λ����׵�OPEN��
#define  KB_UP						0x41	  //���ǻ���һ���ֱ������������ҿ���   0.3S
#define  KB_DOWN					0x42
#define  KB_LEFT					0x43
#define  KB_RIGHT					0x44	  //0.3S
#define  KB_ENTER					0x45
#define  KB_CALL_OUT                0x46
#define  KB_CALL_INOUT              0x47
#define  KB_SCAN               		0x48
#define  KB_PAUSE              		0x49
#define  KB_TUNE_DOWN_LONG			0x4a
#define  KB_SPEECH_IDENTIFY_START	0x4b 	
#define  KB_SPEECH_IDENTIFY_END		0x4c

#define  KB_DVD_SEEK_INC            0x60  
#define  KB_DVD_SEEK_DEC            0x61
#define  KB_RADIO_SEEK_INC          0x62
#define  KB_RADIO_SEEK_DEC		    0x63
#define  KB_RADIO_PROG				0x66
#define  KB_SRADIO_SEEK_INC         0x64
#define  KB_SRADIO_SEEK_DEC         0x65

//ȡ�ù����ڴ�ĵ�ַ
#define  FLY_HW_DLL_PATH _T("FlyHwdrv.dll")
typedef void* (*GetShareMmapAddrPorc)(UINT32 iSize);


//���ظ��û���ѭ��BUF�Ĵ�С
#define BUF_TO_USER_MAX 100
#define BUF_TO_USER_LEN 300


//�����������
#define DIGIT(s, no) ((s)[no] - '0')

const int hours = (10 * DIGIT(__TIME__, 0) + DIGIT(__TIME__, 1));
const int minutes = (10 * DIGIT(__TIME__, 3) + DIGIT(__TIME__, 4));
const int seconds = (10 * DIGIT(__TIME__, 6) + DIGIT(__TIME__, 7));
/* WARNING: This will fail in year 10000 and beyond, as it assumes
* that a year has four digits. */
const int year = ( 1000 * DIGIT(__DATE__, 7)
				  + 100 * DIGIT(__DATE__, 8)
				  + 10 * DIGIT(__DATE__, 9)
				  + DIGIT(__DATE__, 10));

/*
* Jan - 1
* Feb - 2
* Mar - 3
* Apr - 4
* May - 5
* Jun - 6
* Jul - 7
* Aug - 8
* Sep - 9
* Oct - 10
* Nov - 11
* Dec - 12
*
* Use the last letter as primary "key" and middle letter whenever
* two months end in the same letter. */
const int months = (__DATE__[2] == 'b' ? 2 :
					(__DATE__[2] == 'y' ? 5 :
					(__DATE__[2] == 'l' ? 7 :
					(__DATE__[2] == 'g' ? 8 :
					(__DATE__[2] == 'p' ? 9 :
					(__DATE__[2] == 't' ? 10 :
					(__DATE__[2] == 'v' ? 11 :
					(__DATE__[2] == 'c' ? 12 :
					(__DATE__[2] == 'n' ?
					(__DATE__[1] == 'a' ? 1 : 6) :
					/* Implicit "r" */
					(__DATE__[1] == 'a' ? 3 : 4))))))))));
const int day = ( 10 * (__DATE__[4] == ' ' ? 0 : DIGIT(__DATE__, 4))
				 + DIGIT(__DATE__, 5));

#endif 