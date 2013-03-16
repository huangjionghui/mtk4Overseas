#pragma  once

#include "GSysServices.h"
#include "GDef.h"

#define  DEBUG_AUDIO_OLD	0   //1Ϊ������			�ڲ�����
								//0Ϊ����Ƶ�����İ�,�ⲿ����

#define  COLOR_SPACE_LOW_VALUE 0xF81F
#define  COLOR_SPACE_HIGH_VALUE 0xF81F


#define SPEAKER_LAYOUT_MASK 0xFF

enum
{ 
	SETTING_AUDIO_DOWNMIX_LTRT,
	SETTING_AUDIO_DOWNMIX_STEREO,     
	SETTING_AUDIO_DOWNMIX_NONE,
};

enum
{
	SETTING_AUDIO_FRONT_LARGE,
	SETTING_AUDIO_FRONT_SMALL,
};

enum
{
	SETTING_AUDIO_CENTER_LARGE,
	SETTING_AUDIO_CENTER_SMALL,
	SETTING_AUDIO_CENTER_NONE,
};

enum
{
	SETTING_AUDIO_REAR_LARGE,
	SETTING_AUDIO_REAR_SMALL,
	SETTING_AUDIO_REAR_NONE,
};

enum
{
	SETTING_AUDIO_SUBWOOFER_ON,
	SETTING_AUDIO_SUBWOOFER_OFF,
};

enum{
	SPEAK_OUT_5_1_TYPE,//5.1ͨ��
	SPEAK_OUT_5_0_TYPE,
	SPEAK_OUT_4_1_TYPE,
	SPEAK_OUT_4_0_TYPE,
	SPEAK_OUT_STEREO_TYPE//��������
};

#define FULL_VOLUME_GAIN  0x20000
#define MAX_VOLUME_INDEX  100


enum
{
	SETTING_AUDIO_UPMIX_OFF,
	SETTING_AUDIO_UPMIX_ON,
};

#define MM_SET_DSP_MIX_OUTPUT_CH    (WM_USER + 0x17)

typedef enum
{
	AUD_NORMAL_MODE = 0,
	AUD_BT_CALL_MODE,
	AUD_OUTPUT_MODE_NUM,
}AUD_OUTPUT_MODE;
typedef enum
{
	DSP_MIX_FRONT_LR_CH = 1<<0,
	DSP_MIX_SURROUND_CH = 1<<1,
	DSP_MIX_CENTER_CH = 1<<2,
	DSP_MIX_SUBWOOFER_CH= 1<<3,
}DSP_MIX_OUTPUT_CH;

//GPS
#define MM_SET_GPS_PROCESSID	0x413//����GPS����ID
#define MM_SET_GPS_OUTPUT_CH	0x414//����GPS���ģʽ
#define MM_SET_GPS_VOLUME		0x415//����GPS����
#define MM_SET_GPS_CH_CHANGE    0x417//�������绰ʱ���绰�������LS,RS
#define WM_NAVI_SET_VOLUME			(WM_USER + 0X15)//������Ϣʱ�������Ѿ���BSP�ﶨ�壬����ʶ�𵽡�
#define WM_NAVI_SET_VOLUME_RANGE	(WM_USER + 0X16)

enum{
	GPS_BT_RING_SPKOUT_FROM_FRONT_LR  = 0,
	GPS_BT_RING_SPKOUT_FROM_FRONT_L   = 1,
	GPS_BT_RING_SPKOUT_FROM_FRONT_R   = 2,
	GPS_BT_RING_SPKOUT_FROM_FRONT_NOT = 3
};

#define GPS_BT_VOLUME_SET_SPK_FORNT 1


//�ⲿ����
typedef enum{
	X_AUD_FRONT,

	X_AUD_REAR,
	X_AUD_GPS,
	X_AUD_FRONT_REAR,
	X_AUD_SPDIF,
	X_AUD_OUT_MAX
} X_AUD_CFG_ID;

typedef enum {
	X_AUD_DAC_PWM,
	X_AUD_DAC_EXT,
} X_AUD_DAC_TYPE_T;

typedef struct _X_AUD_DAC_TYPE_SEL_T
{
	X_AUD_CFG_ID eOut;
	X_AUD_DAC_TYPE_T eDacType;
}X_AUD_DAC_TYPE_SEL_T;

enum {
	DB_50L =  0xc000,
	DB_55L =  0x8000,
	DB_60L =  0x4000,
	DB_65L =  0x2000,
	DB_70L =  0x1000,
	DB_80L =  0x800,
	DB_85L =  0x400,
	DB_95L =  0X100
};

struct audioapi_t{

	UINT32 u4SpeakerLayout;
	UINT32 u4SpeakerSize;

	//������ǰ������
	BOOL bCurMute;

	UINT32 iPreForntVolume;	//ǰ��
	UINT32 iPreBackVolume;	//����
	UINT32 iPreBTVolume;	//����
	UINT32 iPreBTRingVolume; //������������
	UINT32 iPreGPSVolume;	//GPS
	UINT32 iPreMP3Volume;	//MP3
	UINT32 iPreDVPVolume;	//DVP
	UINT32 iPreIopdVolume;   

	//GPS,MP3 ����ID
	UINT32 iPreGPSProcessID;

	//��ǰ�������ĸ�ͨ��
	BYTE iPreAudioSource;
	BOOL bPreDvdAtBtChannel;
	BOOL bA2DPMustFalseFlag;

	BOOL bBackVideoSigned;
	BOOL bDvdInitStatus;
};

BYTE getEQCurType(BYTE iTemp,BOOL bGetType);
BOOL setUpmix(AUD_UPMIX_T eAudUpMix);
BOOL setSpeak(void);
BOOL setSpkAudioType(BOOL bSubOn);

BOOL setTreble(UINT32 setValue);
BOOL getTreble(UINT32 *getValue);
BOOL setMid(UINT32 setValue);
BOOL getMid(UINT32 *getValue);
BOOL setBass(UINT32 setValue);
BOOL getBass(UINT32 *getValue);
BOOL setBalance(UINT32 iFL,UINT32 iFR,UINT32 iRL,UINT32 iRR);
BOOL getBalance(BYTE iIndex,UINT32 *getValue);
BOOL setLoudness(UINT32 setValue);
BOOL getLoudness(UINT32 *getValue);
BOOL setFrontValume(UINT32 setValue);
BOOL getFrontValume(UINT32 *getValue);
BOOL setBackValume(UINT32 setValue);
BOOL getBackValume(UINT32 *getValue);
BOOL setBTVolume(UINT32 iVolume);
BOOL setBTRingVolume(UINT32 iVolume);
BOOL setGPSVolume(UINT16 iVolume);
BOOL setMP3Volume(UINT16 iVolume);
BOOL setDVPValume(UINT32 setValue);
BOOL setIpodVolume(UINT32 iVolume);


BOOL setMute(UINT32 iMute);
BOOL getMute(UINT32 *iMute);
BOOL setEQType(UINT32 setValue);
BOOL getEQType(UINT32 *getValue);

BOOL setBackValumeEx(UINT32 setValue);
BOOL setFrontValumeEx(UINT32 setValue);

BOOL setCenterDelay(UINT32 iSetValue);
BOOL getCenterDelay(UINT32* getValue);
BOOL setSubwooferDelay(UINT32 iSetValue);
BOOL getSubwooferDelay(UINT32* getValue);
BOOL setRearLeftDelay(UINT32 iSetValue);
BOOL getRearLeftDelay(UINT32* getValue);
BOOL setRearRightDelay(UINT32 iSetValue);
BOOL getRearRightDelay(UINT32* getValue);


BOOL dealEQGains(MISC_EQ_GAIN_T *pEqGain,BYTE *buf,UINT16 len,BOOL bSetValue);
//BOOL setEQValues(AUD_EQVALUES_T rEQValues);
BOOL setEQValues(MISC_EQ_GAIN_T pEqGain);

BOOL getEQValues(AUD_EQVALUES_T *prEQValues);
BOOL setReverbType(UINT32 setValue);
BOOL getReverbType(UINT32 *getValue);

BOOL  AudioSourceInit(void);
BOOL  dealEQvalues(AUD_EQVALUES_T *rEQValues,BYTE *buf,BOOL bSetValue);
BOOL  setAudioSourceSelect(UINT32 setValue,BYTE iChannel);
void  setProcessID(BYTE type, UINT32 iProcessID);

void setBackVideoSigned(BOOL bSined);
void setDVDInitStatus(BOOL bInitStauts);
 