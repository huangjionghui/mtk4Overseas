/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008 MediaTek Inc.
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*******************************************************************************
*
* Filename:
* ---------
* file GSysServices.h
*
* Project:
* --------
*   CNB
*
* Description:
* ------------
*
*
* Author:
* -------
*
*
*------------------------------------------------------------------------------
* $Revision: #50 $
* $Modtime:$
* $Log:$
*
*******************************************************************************/

#ifndef _GSYSSERVICES_H_
#define _GSYSSERVICES_H_

#include "windows.h"
#include "GDef.h"
#include "GKal.h"
#include "winioctl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*  @defgroup SystemServices  SystemServices
*
*  @brief The module for AV switch between front and rear,
*  @{
*/

#define MISC_SET_PL2			1
#define	MISC_SET_SRS			1
#define	MISC_CLIENT_SET_GAIN	1

typedef enum
{
    AUD_DEC_PROLOGICII,
    AUD_DEC_SACD,
    AUD_DEC_AAC,
    AUD_DEC_AACUSED_ONLY,
    AUD_DEC_NEO6
}AUD_DEC_FEATURE_INFO_T;


typedef enum{
	CFG_DISPLAY_PS,
	CFG_DISPLAY_LB,
	CFG_DISPLAY_WIDE,
	CFG_DISPLAY_WIDESQZ
}TVDISPLAY_T;

typedef enum{
	CFG_ON,
	CFG_OFF
}ONOFF_T;

typedef enum{
	CFG_TVTYPE_PAL,
	CFG_TVTYPE_AUTO,
	CFG_TVTYPE_NTSC
}TVTYPE_T;

typedef enum{
	CFG_FALSE,
	CFG_TRUE
}PBCTYPE_T;

typedef enum{
	CFG_LANGUAGE_ENGLISH,
	CFG_LANGUAGE_FRENCH,
    CFG_LANGUAGE_SPANISH,
    CFG_LANGUAGE_CHINESE,
    CFG_LANGUAGE_JAPANESE,
    CFG_LANGUAGE_OTHER,
    CFG_LANGUAGE_GERMAN,
    CFG_LANGUAGE_SWEDISH,
    CFG_LANGUAGE_DANISH,
    CFG_LANGUAGE_NORWEGIAN,
    CFG_LANGUAGE_FINNISH,
    CFG_LANGUAGE_DUTCH,
    CFG_LANGUAGE_ICELANDIC,
    CFG_LANGUAGE_PORTUGUES,
    CFG_LANGUAGE_HEBREW,
    CFG_LANGUAGE_GREEK,
    CFG_LANGUAGE_CROATIAN,
    CFG_LANGUAGE_TURKISH,
    CFG_LANGUAGE_ITALIAN,
    CFG_LANGUAGE_POLISH,
    CFG_LANGUAGE_HUNGARIAN,
    CFG_LANGUAGE_CZECH,
    CFG_LANGUAGE_KOREAN,
    CFG_LANGUAGE_RUSSIAN,
    CFG_LANGUAGE_THAI,
    CFG_LANGUAGE_INDONESIAN,
    CFG_LANGUAGE_MALAY,
    CFG_LANGUAGE_IRISH,
    CFG_LANGUAGE_ROMANIAN,
    CFG_LANGUAGE_LANG_OFF
}LANGUAGE_T;

typedef enum{
	CFG_PR_KID = 1,
	CFG_PR_G,
	CFG_PR_PG,
	CFG_PR_PG13,
	CFG_PR_PR5,
	CFG_PR_GR,
	CFG_PR_NC,
	CFG_PR_ADULT,
	CFG_PR_OFF = 0xF
}PARENTAL_T;

typedef enum{
	CFG_SPDIFOUTPUT_OFF,
	CFG_SPDIFOUTPUT_RAW,
	CFG_SPDIFOUTPUT_PCM,
	CFG_SPDIFOUTPUT_MANUAL
}SPDIFOUTPUT_T;

typedef enum{
	CFG_LPCMOUT_48K,
	CFG_LPCMOUT_96K
}LPCMOUT_T;

typedef enum{
	CFG_EQ_OFF,
	CFG_EQ_ROCK,
	CFG_EQ_POP,
	CFG_EQ_LIVING,
	CFG_EQ_DANCE,
	CFG_EQ_CLASSICAL,
	CFG_EQ_SOFT
}EQTYPE_T;

typedef enum
{
	AUD_SE_PL2_OFF,
	AUD_SE_PL2_MUSIC,
	AUD_SE_PL2_MOVIE,
	AUD_SE_PL2_AUTO
}PLII_TYPE;

typedef enum{
	DVP_CFG_PL2_OFF,
	DVP_CFG_PL2_MUSIC,
	DVP_CFG_PL2_MOVIE,
	DVP_CFG_PL2_EMU,
	DVP_CFG_PL2_MATRIX,
	DVP_CFG_PL2_MD_AUTO,
	DVP_CFG_PL2_MODE_MAX
}DVP_PL2_TYPE;

typedef enum{
	CFG_REVERB_OFF,
	CFG_REVERB_LIVINGROOM,
	CFG_REVERB_HALL,
	CFG_REVERB_CONCERT,
	CFG_REVERB_CAVE,
	CFG_REVERB_BATHROOM,
	CFG_REVERB_ARENA
}REVERBTYPE_T;

#define MISC_REVERB_FS_CNT            (3)         // support 3 sample rate type 32/44/48
#define MISC_REVERB_BANK_CNT          (4)         // reverb bank count 4

typedef struct {
    UINT32 u4Gain;
    UINT32 u4FeedBackGain;
    UINT32 u4BankSize[MISC_REVERB_FS_CNT][MISC_REVERB_BANK_CNT];
} MISC_REVERB_COEF_T;

typedef enum{
	CFG_DUALMONO_STEREO,
	CFG_DUALMONO_LEFT,
	CFG_DUALMONO_RIGHT,
	CFG_DUALMONO_MIX
}DUALMONO_T;

typedef enum{
	AUD_UPMIX_OFF,
	AUD_UPMIX_ON
}AUD_UPMIX_T;

enum
{
	SETTING_AUDIO_SPDIF_OFF,
	SETTING_AUDIO_SPDIF_RAW,
	SETTING_AUDIO_SPDIF_PCM,
};

typedef struct _MISC_UPMIX_GAIN_T
{
    GUINT32		u4Gain0;
	GUINT32		u4Gain1;
	GUINT32		u4Gain2;
	GUINT32		u4Gain3;
	GUINT32		u4Gain4;
	GUINT32		u4Gain5;
	GUINT32		u4Gain6;
	GUINT32		u4Gain7;
}MISC_UPMIX_GAIN_T;

typedef struct _MISC_LOUDNESS_GAIN_T
{
    GUINT32		u4Gain0;
	GUINT32		u4Gain1;
	GUINT32		u4Gain2;
	GUINT32		u4Gain3;
	GUINT32		u4Gain4;
	GUINT32		u4Gain5;
}MISC_LOUDNESS_GAIN_T;

typedef enum{
	CFG_MVS_OFF,
	CFG_MVS_MODE_MOVIE,
	CFG_MVS_MODE_MUSIC
}MISC_MVS_T;

typedef struct _MISC_MVS_GAIN_T
{
    GUINT32		u4GainScale;
	GUINT32		u4WidthGain;
	GUINT32		u4LRGain;
	GUINT32		u4CenterGain;
	GUINT32		u4CrosstalkGain;
	GUINT32		u4BassGain;
	GUINT32		u4OutputGain;
	GUINT32		u4InputGain;
	GUINT32		u4VRPhase;
}MISC_MVS_GAIN_T;


typedef struct _MISC_AUD_THRESHOLD
{
	UINT32 u4FrontThrshld;		
	UINT32 u4RearThrshld;
	UINT32 u4WaveFormThrshld;
}MISC_AUD_THRESHOLD_T;

/*---------------------------For Aud Output-------------------------*/
typedef enum{
    AUD_FRONT,
    AUD_REAR,
    AUD_GPS,
    AUD_FRONT_REAR,
    AUD_SPDIF,
    AUD_OUT_MAX
} AUD_CFG_ID;

typedef enum{
    AUD_DAC_PWM,
    AUD_DAC_EXT,
}AUD_DAC_TYPE_T;

typedef enum{
    AUD_AOUT1,
    AUD_AOUT2,
    AUD_DVD_OUT,
    AUD_GPS_OUT,
    AUD_UNDEF_OUT
} AUD_OUT_TYPE_T;


typedef struct _AUD_OUTPUT_PATH_T
{
    AUD_CFG_ID          eOut;           /* Front, GPS, Rear */
    AUD_OUT_TYPE_T      eSrc;           /* Aout1, Aout2, DVP Aout, GPS Aout */
}AUD_OUTPUT_PATH_T;

typedef struct _AUD_EQVALUES_T
{
    GINT32		i4Band0;
	GINT32		i4Band1;
	GINT32		i4Band2;
	GINT32		i4Band3;
	GINT32		i4Band4;
	GINT32		i4Band5;
	GINT32		i4Band6;
	GINT32		i4Band7;
	GINT32		i4Band8;
	GINT32		i4Band9;
	GINT32		i4Band10;
}AUD_EQVALUES_T;

typedef struct _MISC_EQ_GAIN_T
{
    GUINT32		u4Gain0;
	GUINT32		u4Gain1;
	GUINT32		u4Gain2;
	GUINT32		u4Gain3;
	GUINT32		u4Gain4;
	GUINT32		u4Gain5;
	GUINT32		u4Gain6;
	GUINT32		u4Gain7;
	GUINT32		u4Gain8;
	GUINT32		u4Gain9;
	GUINT32		u4Gain10;
}MISC_EQ_GAIN_T;

typedef struct _MISC_DVP_BALANCES_T
{
    GINT32		i4CenterValue;
	GINT32		i4FLeftValue;
	GINT32		i4FRightValue;
	GINT32		i4RLeftValue;
	GINT32		i4RRightValue;
	GINT32		i4SubWooferValue;
}MISC_DVP_BALANCES_T;

/*--------------------------------------------------------------------*/

/*------------------Speaker Layout----------------*/
typedef enum
{
	SPEAKER_LAYOUT_MIN       = 0,            // None
    SPEAKER_LAYOUT_LR        = 0x0001 << 0,  //LT/RT
    SPEAKER_LAYOUT_MONO      = 0x0001 << 1,  //Mono
    SPEAKER_LAYOUT_STEREO    = 0x0001 << 2,  //Stereo
    SPEAKER_LAYOUT_LRC       = 0x0001 << 3,  //L/R/C
    SPEAKER_LAYOUT_LRS       = 0x0001 << 4,  //L/R/S
    SPEAKER_LAYOUT_LRCS      = 0x0001 << 5,  //L/R/C/S
    SPEAKER_LAYOUT_LRLSRS    = 0x0001 << 6,  //L/R/LS/RS
    SPEAKER_LAYOUT_LRCLSRS   = 0x0001 << 7,  //L/R/C/LS/RS
    SPEAKER_LAYOUT_SUBWOOFER = 0x0001 << 8,  //LFE
    SPEAKER_LAYOUT_MAX       = 0x00FF
}SPEAKER_LAYOUT_TYPE;

typedef enum
{
    BALANCE_FRONT_LEFT = 0,
    BALANCE_FRONT_RIGHT,
    BALANCE_REAR_LEFT,
    BALANCE_REAR_RIGHT,
    BALANCE_CENTER,
    BALANCE_SUB_WOOFER
}BALANCE_CHANGE_TYPE;

typedef enum
{
    SPEAKER_LAYOUT_C_LARGE  = 0x0001 << 0,   //Center channel large
    SPEAKER_LAYOUT_L_LARGE  = 0x0001 << 1,   //Left channel large
    SPEAKER_LAYOUT_R_LARGE  = 0x0001 << 2,   //Right channel large
    SPEAKER_LAYOUT_LS_LARGE = 0x0001 << 3,   //Left surround channel large
    SPEAKER_LAYOUT_RS_LARGE = 0x0001 << 4,   //Right surround channel large
}SPEAKER_SIZE_TYPE;

typedef enum
{
    AUD_MUTE_OFF = 0,
    AUD_MUTE_ON
}AUD_MUTE_TYPE_T;

/* for test tone */
typedef enum
{
	TEST_TONE_FRONT,
	TEST_TONE_REAR,
	TEST_TONE_OFF
}AUD_TEST_TONE_T;

typedef enum
{
    MISC_TEST_TONE_PINK_NOISE,
    MISC_TEST_TONE_TRIANGLE_WAVE,
    MISC_TEST_TONE_SINE_WAVE,
    MISC_TEST_TONE_WHITE_NOISE,
    MISC_TEST_TONE_PINK_NOISE_DOLBY  
} MISC_TEST_TONE_TYPE_T;

#define DISPLAY_SET_CONTRAST                     (0x00020005)
#define DISPLAY_SET_BRIGNTNESS                   (0x00020006)
#define DISPLAY_SET_BKL_INTENSITY                (0x00020008)
#define DISPLAY_SET_SATUREATION					 (0x00020007)
#define DISPLAY_SET_HUE							 (0x00020009)

//choose spdif output
typedef enum {
	MISC_AUD_AOUT1, 
	MISC_AUD_DVD_OUT,
	MISC_AUD_OFF
}MISC_AUD_OUT_TYPE_T;

typedef struct _MISC_AUD_FUNC_OPTION
{
	UINT32 u4FuncOption0;		//default: 0
	UINT32 u4FuncOption1;		//default: 0
	UINT32 u4FuncOption2;		//default: 0
	UINT32 u4BassCutOffFreq;	//default: 100 (Hz)
	UINT32 u4GainAvIn;			//default: 0x20000
	UINT32 u4GainUSB;			//default: 0x20000
	UINT32 u4GainDVD;			//default: 0x20000	
	UINT32 u4Reserve1;			//default: 0
	UINT32 u4Reserve2;			//default: 0
	UINT32 u4Reserve3;			//default: 0
	UINT32 u4Reserve4;			//default: 0
	UINT32 u4Reserve5;			//default: 0
	UINT32 u4Reserve6;			//default: 0	
}MISC_AUD_FUNC_OPTION_T;


typedef enum{
    AUD_DEC_DIV_TYPE_LOGO_DOLBY,
    AUD_DEC_DIV_TYPE_LOGO_DTS,
    AUD_DEC_DIV_TYPE_ANALOG_OUTPUT_CHANNEL,
    AUD_DEC_DIV_TYPE_HDMI_OUTPUT_CHANNEL,
    AUD_DEC_DIV_TYPE_POST_PROCESS,
    AUD_DEC_DIV_TYPE_CODEC_SUPPORT
}AUD_DEC_DIV_TYPE_T;

typedef struct _AUD_DEC_DIV_INFO_T
{
    AUD_DEC_DIV_TYPE_T     e_type;
    UINT8                  u1_setting;
} AUD_DEC_DIV_INFO_T;

#define   DOLBY_DECODE_2CH_SETTING              0x80      //Dolby AC3 2channel
#define   DOLBY_DECODE_MULTI_CH_SETTING         0x00      //Dolby AC3 multi channel

/*-----------------------------------------------*/
/**
* @brief set Volume, set Balance, set EQ type, set SpeakerLayout and so on.  
*
* @return NONE
*			   
**/
void  GInit();

/**
* @brief direct return, do nothing.
*
* @return NONE
*
* @note not implemented
*
**/
void  GDeInit();

/**
* @brief set Brightness, set Contrast, set Hue, set Saturation and set BackLight.  
*
* @return 1 indicate success      
*			   
**/
GUINT32  GSysSetting();

/**
* @brief set brightness level and save in regedit
*
* @param[in] u4Level brightness level to be set.
*
* @return GUINT32 define in GDef.h
*
* @note u4Level to set between 0 and 100
*
**/
GUINT32  GSetBrightnessLevel(GUINT32 u4Level); 

/**
* @brief get brightness level from regedit
*
* @param[out] pu4Level pointer to brightness level got.
*
* @return TRUE indicate success, FALSE indicate fail
*
* @note pu4Level to get between 0 and 100
*
**/
GUINT32  GGetBrightnessLevel(GUINT32 *pu4Level); 

/**
* @brief set contrast level and save in regedit
*
* @param[in] u4Level contrast level to be set.
*
* @return GUINT32 define in GDef.h
*
* @note u4Level to set between 0 and 100
*
**/
GUINT32  GSetContrastLevel(GUINT32 u4Level); 

/**
* @brief get brightness level from regedit
*
* @param[out] pu4Level pointer to contrast level got.
*
* @return TRUE indicate success, FALSE indicate fail
*
* @note pu4Level to get between 0 and 100
*
**/
GUINT32  GGetContrastLevel(GUINT32 *pu4Level); 

/**
* @brief set backlight level  and save in regedit
*
* @param[in] u4Level backlight level to be set.
*
* @return GUINT32 define in GDef.h
*
* @note u4Level to set between 0 and 10
*
**/

GUINT32  GSetBackLightLevel(GUINT32 u4Level);

/**
* @brief get backlight level from regedit
*
* @param[out] pu4Level pointer to backlight level got.
*
* @return TRUE indicate success, FALSE indicate fail
*
* @note pu4Level to get between 0 and 10
*
**/
GUINT32  GGetBackLightLevel(GUINT32 *pu4Level);

/**
* @brief set hue level and save in regedit
*
* @param[in] u4Level hue level to be set.
*
* @return GUINT32 define in GDef.h
*
* @note u4Level to set between 0 and 100
*
**/
GUINT32  GSetHueLevel(GUINT32 u4Level);

/**
* @brief get hue level from regedit
*
* @param[out] pu4Level pointer to hue level got.
*
* @return TRUE indicate success, FALSE indicate fail
*
* @note pu4Level to get between 0 and 100
*
**/
GUINT32  GGetHueLevel(GUINT32 *pu4Level); 

/**
* @brief set saturation level and save in regedit
*
* @param[in] u4Level saturation level to be set.
*
* @return GUINT32 define in GDef.h
*
* @note u4Level to set between 0 and 100
*
**/
GUINT32  GSetSaturationLevel(GUINT32 u4Level);

/**
* @brief  get saturation level from regedit
*
* @param[out] pu4Level pointer to saturation level got.
*
* @return TRUE indicate success, FALSE indicate fail
*
* @note u4Level to get between 0 and 100
*
**/
GUINT32  GGetSaturationLevel(GUINT32 *pu4Level); 

/**
* @brief  set backlight on/off
*
* @param[in] bBacklightOn the status of backlight.
*
* @return TRUE indicate success, FALSE indicate fail
*
**/
GUINT32 GSetBackLight(GBOOL  bBacklightOn);

/**
* @brief  open audio device
*
* @return GHANDLE define in GDef.h
*
**/
GHANDLE GOpenAudioDev();

/**
* @brief  close audio device
*
* @param[in] hAudioDev audio device handle 
*
* @return GUINT32 define in GDef.h
*
**/
GUINT32 GCloseAudioDev(GHANDLE hAudioDev);

/**
* @brief  set audio device mute and save in regedit
*
* @param[in] hAudioDev audio device handle 
* @param[in] eMute on/off
*
* @return TRUE indicate success, FALSE indicate fail
*
**/
GUINT32 GSetMute(GHANDLE hAudioDev,AUD_MUTE_TYPE_T  eMute);

/**
* @brief  get audio device mute status from regedit
*
* @param[in] hAudioDev audio device handle 
* @param[out] peMute pointer to mute status
*
* @return 1 indicate success, 0 indicate fail
*
**/
GUINT32 GGetMute(GHANDLE hAudioDev,AUD_MUTE_TYPE_T *peMute);

/**
* @brief  set audio volume and don't save in regedit
*
* @param[in] hAudioDev audio device handle 
* @param[in] u4Vol the volume value to be set
*
* @return TRUE indicate success, FALSE indicate fail
*
**/
GUINT32  GSetVolumeEx(GHANDLE hAudioDev,GINT32  u4Vol);

/**
* @brief  set audio volume and save in regedit
*
* @param[in] hAudioDev audio device handle 
* @param[in] u4Vol the volume value to be set
*
* @return TRUE indicate success, FALSE indicate fail
*
**/
GUINT32 GSetVolume(GHANDLE hAudioDev,GUINT32  u4Vol);

/**
* @brief  get audio volume from regedit
*
* @param[in] hAudioDev audio device handle 
* @param[out] pu4Vol pointer to audio volume value
*
* @return TRUE indicate success, FALSE indicate fail
*
**/
GUINT32 GGetVolume(GHANDLE hAudioDev,GUINT32  *pu4Vol);

GUINT32 GSetRearVolumeEx(GHANDLE hAudioDev, GUINT32 u4Vol);

/**
* @brief  set audio rear volume and save in regedit
*
* @param[in] hAudioDev audio device handle 
* @param[in] u4Vol the rear volume value to be set
*
* @return TRUE indicate success, FALSE indicate fail
*
**/
GUINT32  GSetRearVolume(GHANDLE hAudioDev, GUINT32 u4Vol);

/**
* @brief  get audio rear volume from regedit
*
* @param[in] hAudioDev audio device handle 
* @param[out] pu4Vol pointer to audio rear volume value
*
* @return TRUE indicate success, FALSE indicate fail
*
**/
GUINT32  GGetRearVolume(GHANDLE hAudioDev, GUINT32 *pu4Vol);

/**
* @brief  set audio TestTone and save in regedit
*
* @param[in] hAudioDev audio device handle 
* @param[in] eTestTone the TestTone value to be set
*
* @return 1 indicate success, 0 indicate fail
*
**/
GUINT32 GSetTestTone(GHANDLE hAudioDev, AUD_TEST_TONE_T eTestTone);

/**
* @brief  get audio TestTone from regedit
*
* @param[in] hAudioDev audio device handle 
* @param[out] peTestTone pointer to audio TestTone value
*
* @return 1 indicate success, 0 indicate fail
*
**/
GUINT32 GGetTestTone(GHANDLE hAudioDev, AUD_TEST_TONE_T *peTestTone);

/**
* @brief  set audio EQ on the specified band and save in regedit
*
* @param[in] hAudioDev audio device handle 
* @param[in] u4Band the band value
* @param[in] i4Value the EQ value on u4Band
*
* @return 1 indicate success, 0 indicate fail
*
**/
GUINT32 GSetEQValue(GHANDLE hAudioDev,GUINT u4Band,GINT32 i4Value);
/**
* @brief  get audio EQ value on the specified band
*
* @param[in] hAudioDev audio device handle 
* @param[in] u4Band  the band value
* @param[out] pi4Value pointer to the EQ value on u4Band
*
* @return 1 indicate success, 0 indicate fail
*
**/
GUINT32 GGetEQValue(GHANDLE hAudioDev,GUINT u4Band,GINT32* pi4Value);

/**
* @brief  set audio EQ on all band and save in regedit
*
* @param[in] hAudioDev audio device handle 
* @param[in] rEQValues the all band value
*
* @return 1 indicate success, 0 indicate fail
*
**/
GUINT32 GSetEQValues(GHANDLE hAudioDev, AUD_EQVALUES_T rEQValues);

/**
* @brief  get audio EQ value on all band
*
* @param[in] hAudioDev audio device handle 
* @param[out] prEQValues pointer to the EQ value on all band
*
* @return 1 indicate success, 0 indicate fail
*
**/
GUINT32 GGetEQValues(GHANDLE hAudioDev, AUD_EQVALUES_T* prEQValues);

/**
* @brief  set audio EQ bound on the specified band
*
* @param[in] hAudioDev audio device handle 
* @param[in] u4Band the band value
* @param[in] u4mix_value the min value
* @param[in] u4max_value the max value
*
* @return GUINT32 define in GDef.h
*
**/
GUINT32 GSetEQBounds(GHANDLE hAudioDev,
					 GUINT u4Band,
					 GUINT32 u4mix_value,
					 GUINT32 u4max_value);

/**
* @brief  get audio EQ bounds on the specified band
*
* @param[in] hAudioDev audio device handle 
* @param[in] u4Band the band value
* @param[out] pu4mix_value GUINT32 * the min value
* @param[out] pu4max_value GUINT32 * the max value
*
* @return GUINT32 define in GDef.h
*
**/
GUINT32 GGetEQBounds(GHANDLE hAudioDev,
					 GUINT u4Band,
					 GUINT32 *pu4mix_value,
					 GUINT32 *pu4max_value);

/**
* @brief  get EQ nums
*
* @param[in] hAudioDev audio device handle 
* @param[out] pu4BandsNum GUINT * the band nums
*
* @return GUINT32 define in GDef.h
*
**/
GUINT32 GGetEQBands(GHANDLE hAudioDev,GUINT *pu4BandsNum);

/**
* @brief  query spectrum on the specified band
*
* @param[in] hAudioDev audio device handle 
* @param[in] u4Band the band value
* @param[out] u4Values GUINT32* the spectrum value
*
* @return GUINT32 define in GDef.h
*
**/
GUINT32 GQuerySpectrum(GHANDLE hAudioDev,GUINT u4Band,GUINT32* u4Values);

/**
* @brief  query spectrum bounds on the specified band
*
* @param[in] hAudioDev audio device handle 
* @param[in] u4Band the band value
* @param[out] pu4mix_value GUINT32* the min value
* @param[out] pu4max_value GUINT32* the max value
*
* @return GUINT32 define in GDef.h
*
**/
GUINT32 GGetSpectrumBounds(GHANDLE hAudioDev,
						   GUINT u4Band,
						   GUINT32 *pu4mix_value,
						   GUINT32 *pu4max_value);

/**
* @brief  query spectrum band nums
*
* @param[in] hAudioDev audio device handle 
* @param[out] pu4BandsNum GUINT32* the band num value
*
* @return GUINT32 define in GDef.h
*
**/
GUINT32 GGetSpectrumBands(GHANDLE hAudioDev,GUINT32 *pu4BandsNum);

/**
* @brief  set audio feature
*
* @param[in] hAudioDev audio device handle 
* @param[in] eAudFeatur audio feature value
*
* @return GBOOL define in GDef.h
*
*	@see AUD_DEC_FEATURE_INFO_T
*
**/
GBOOL   GSetAudFeature(GHANDLE hAudioDev, AUD_DEC_FEATURE_INFO_T eAudFeatur);

/**
* @brief  set audio balance on the specified type
*
* @param[in] hAudioDev audio device handle 
* @param[in] i4Values the balance value
* @param[in] eBalanceType the specified balace type
*
* @return GUINT32 define in GDef.h
*
*	@see BALANCE_CHANGE_TYPE
*
**/
GUINT32 GSetBalance(GHANDLE hAudioDev, GINT32 i4Values, BALANCE_CHANGE_TYPE eBalanceType);

/**
* @brief  get audio balance on the specified type
*
* @param[in] hAudioDev audio device handle 
* @param[out] pi4Values GINT32 * the balance value
* @param[in] eBalanceType the specified balace type
*
* @return GUINT32 define in GDef.h
*
* @see BALANCE_CHANGE_TYPE
*
**/
GUINT32 GGetBalance(GHANDLE hAudioDev, GINT32 *pi4Values, BALANCE_CHANGE_TYPE eBalanceType);

/*
 *  SRS CSII Coefficient Struct Type define
 */



/* switch operation value */

/* mode operation value */
typedef enum
{
    MISC_AUD_SE_CSII_MODE_CINEMA = 0,
    MISC_AUD_SE_CSII_MODE_PRO,
    MISC_AUD_SE_CSII_MODE_MUSIC,
    MISC_AUD_SE_CSII_MODE_MONO,
    MISC_AUD_SE_CSII_MODE_LCRS,
}MISC_AUD_SE_CSII_MODE_T;

typedef enum
{
	MISC_AUD_SE_CSII_OFF = 0,
	MISC_AUD_SE_CSII_ON,
	MISC_AUD_SE_CSII_SWITCH_AUTO
}MISC_AUD_SE_CSII_SWITCH_T;
			
typedef enum
{
    MISC_AUD_SE_CSII_SS_40HZ  = (1 << 0), // Speaker size is 40Hz
    MISC_AUD_SE_CSII_SS_60HZ  = (1 << 1),
    MISC_AUD_SE_CSII_SS_100HZ = (1 << 2),
    MISC_AUD_SE_CSII_SS_150HZ = (1 << 3),
    MISC_AUD_SE_CSII_SS_200HZ = (1 << 4),
    MISC_AUD_SE_CSII_SS_250HZ = (1 << 5),
    MISC_AUD_SE_CSII_SS_300HZ = (1 << 6),
    MISC_AUD_SE_CSII_SS_400HZ = (1 << 7),
}   MISC_AUD_SE_CSII_SS_T;

typedef enum
{
	MISC_AUD_SE_CSII_FOCUS_CENTER = 0,
	MISC_AUD_SE_CSII_FOCUS_FRONT,
	MISC_AUD_SE_CSII_FOCUS_REAR,
}MISC_AUD_SE_CSII_FOCUS_T;

typedef enum
{
	MISC_AUD_SE_CSII_TBSS_FRONT = 0,
	MISC_AUD_SE_CSII_TBSS_SUB,
	MISC_AUD_SE_CSII_TBSS_REAR,
}MISC_AUD_SE_CSII_TBSS_T;
typedef struct
{
	GBOOL fgSupportSrs;
	GBOOL fgSupportDolby_Prologic;
} AVC_TABLE_INFO;
GBOOL GGetSRSEfuse();
GBOOL fgIsSupportSRS();
GBOOL GGetPL2Efuse();
GBOOL fgIsSupportPL2();
GUINT32 GGetRegInit(AVC_TABLE_INFO* use_table_info);
//GUINT32 GSetAudThreshold(GHANDLE hAudioDev, MISC_AUD_THRESHOLD_T rThreshold);

#if MISC_SET_SRS
GUINT32 GSetSRSSwitch(GHANDLE hAudioDev, MISC_AUD_SE_CSII_SWITCH_T eCSIISwitch);
GUINT32 GGetSRSSwitch(GHANDLE hAudioDev, MISC_AUD_SE_CSII_SWITCH_T *peCSIISwitch);

GUINT32 GSetSRSMode(GHANDLE hAudioDev,MISC_AUD_SE_CSII_MODE_T eCSIIMode);
GUINT32 GGetSRSMode(GHANDLE hAudioDev,MISC_AUD_SE_CSII_MODE_T *peCSIIMode);

GUINT32 GSetSRSPhantom(GHANDLE hAudioDev,MISC_AUD_SE_CSII_SWITCH_T eCSIISwitch);
GUINT32 GGetSRSPhantom(GHANDLE hAudioDev,MISC_AUD_SE_CSII_SWITCH_T *peCSIISwitch);

GUINT32 GSetSRSFullBand(GHANDLE hAudioDev,MISC_AUD_SE_CSII_SWITCH_T eCSIISwitch);
GUINT32 GGetSRSFullBand(GHANDLE hAudioDev,MISC_AUD_SE_CSII_SWITCH_T *peCSIISwitch);

GUINT32 GSetSRSFocus(GHANDLE hAudioDev,MISC_AUD_SE_CSII_FOCUS_T eFocus,MISC_AUD_SE_CSII_SWITCH_T eCSIISwitch);
GUINT32 GGetSRSFocus(GHANDLE hAudioDev,MISC_AUD_SE_CSII_FOCUS_T eFocus,MISC_AUD_SE_CSII_SWITCH_T *peCSIISwitch);

GUINT32 GSetSRSTrueBass(GHANDLE hAudioDev,MISC_AUD_SE_CSII_SWITCH_T eCSIISwitch);
GUINT32 GGetSRSTrueBass(GHANDLE hAudioDev,MISC_AUD_SE_CSII_SWITCH_T *eCSIISwitch);

GUINT32 GSetSRSTrueBassSize(GHANDLE hAudioDev,MISC_AUD_SE_CSII_TBSS_T eTBSS,MISC_AUD_SE_CSII_SS_T eCSIITBSS);
GUINT32 GGetSRSTrueBassSize(GHANDLE hAudioDev,MISC_AUD_SE_CSII_TBSS_T eTBSS,MISC_AUD_SE_CSII_SS_T *peCSIITBSS);
#endif

#if MISC_CLIENT_SET_GAIN
GUINT32 GClientSetVolumeEx(GHANDLE hAudioDev, GUINT32 u4Vol);

GUINT32	GClientSetVolume(GHANDLE hAudioDev,GUINT32 u4Vol);

GUINT32 GClientGetVolume(GHANDLE hAudioDev, GUINT32 *pu4Vol);

GUINT32 GClientSetRearVolumeEx(GHANDLE hAudioDev, GUINT32 u4Vol);

GUINT32 GClientSetRearVolume(GHANDLE hAudioDev, GUINT32 u4Vol);

GUINT32 GClientGetRearVolume(GHANDLE hAudioDev, GUINT32 *pu4Vol);

GUINT32 GClientSetBalance(GHANDLE hAudioDev, GUINT32 u4Values, BALANCE_CHANGE_TYPE eBalanceType);

GUINT32 GClientGetBalance(GHANDLE hAudioDev, GUINT32 *pu4Values, BALANCE_CHANGE_TYPE eBalanceType);

GUINT32 GClientSetTestTone(GHANDLE hAudioDev, AUD_TEST_TONE_T eTestTone, MISC_TEST_TONE_TYPE_T eTestToneType);

GUINT32 GClientGetTestTone(GHANDLE hAudioDev,AUD_TEST_TONE_T *peTestTone, MISC_TEST_TONE_TYPE_T *peTestToneType);

GUINT32 GClientSetUpMix(GHANDLE hAudioDev, AUD_UPMIX_T eUpMixType, MISC_UPMIX_GAIN_T rUpmixGain);

GUINT32 GClientGetUpMix(GHANDLE hAudioDev, AUD_UPMIX_T *peUpMixType, MISC_UPMIX_GAIN_T *prUpmixGain);

GUINT32 GClientSetLoudNess(GHANDLE hAudioDev, GUINT8 uLoudNessType, MISC_LOUDNESS_GAIN_T rLoudNessGain);

GUINT32 GClientGetLoudNess(GHANDLE hAudioDev, GUINT8 *puLoudNessType, MISC_LOUDNESS_GAIN_T *prLoudNessGain);

GUINT32 GClientSetReverbType(GHANDLE hAudioDev, REVERBTYPE_T eReverbType, MISC_REVERB_COEF_T rReverbCoef);

GUINT32 GClientGetReverbType(GHANDLE hAudioDev, REVERBTYPE_T *peReverbType, MISC_REVERB_COEF_T *prReverbCoef);

GUINT32 GClientSetEQValues(GHANDLE hAudioDev, MISC_EQ_GAIN_T rEQGain, GBOOL fgEQEnable);

GUINT32 GClientGetEQValues(GHANDLE hAudioDev, MISC_EQ_GAIN_T *prEQGain);

GUINT32 GClientSetEQType(GHANDLE hAudioDev, EQTYPE_T eEQType, MISC_EQ_GAIN_T rEQTypeGain);

GUINT32 GClientGetEQType(GHANDLE hAudioDev, EQTYPE_T *peEQType, MISC_EQ_GAIN_T *prEQTypeGain);

GUINT32 GClientSetMVS(GHANDLE hAudioDev, MISC_MVS_T eMVSType, MISC_MVS_GAIN_T rMVSGain);

GUINT32 GClientGetMVS(GHANDLE hAudioDev, MISC_MVS_T *peMVSType, MISC_MVS_GAIN_T *prMVSGain);

#endif

/**
* @brief  set audio speaker layout type and size
*
* @param[in] hAudioDev audio device handle 
* @param[in] u4SpeakerLayoutType the speaker layout type
* @param[in] u4SpeakerSize the speaker size
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 GSetSpeakerLayout(GHANDLE hAudioDev, GUINT32 u4SpeakerLayoutType, GUINT32 u4SpeakerSize);

/**
* @brief  get audio speaker layout type and size
*
* @param[in] hAudioDev audio device handle 
* @param[out] pu4SpeakerLayoutType GINT32 * the speaker layout type
* @param[out] pu4SpeakerSize the speaker size
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 GGetSpeakerLayout(GHANDLE hAudioDev, GUINT32* pu4SpeakerLayoutType, GUINT32* pu4SpeakerSize);

/**
* @brief  set DRC on/off
*
* @param[in] bDRCOn on/off 
*
* @return GUINT32 define in GDef.h
* @note not in implementation
*
**/
GUINT32 GSetDRC(GBOOL bDRCOn);

/**
* @brief  set audio EQ  type
*
* @param[in] hAudioDev audio device handle 
* @param[in] eEQType the EQ type value
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 GSetEQType(GHANDLE hAudioDev, EQTYPE_T eEQType);

/**
* @brief  get audio EQ type
*
* @param[in] hAudioDev audio device handle 
* @param[out] peEQType EQTYPE_T * the EQ type value
*
* @return GUINT32 define in GDef.h
*
* @see EQTYPE_T
*
**/
GUINT32 GGetEQType(GHANDLE hAudioDev, EQTYPE_T *peEQType);

#if MISC_SET_PL2
/**
* @brief  set audio PL2  type
*
* @param[in] hAudioDev audio device handle 
* @param[in] ePL2Type the PL2 type value
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 GSetPL2(GHANDLE hAudioDev, PLII_TYPE ePL2Type);

/**
* @brief  get audio PL2 type
*
* @param[in] hAudioDev audio device handle 
* @param[out] pePL2Type PLII_TYPE * the PL2 value
*
* @return GUINT32 define in GDef.h
*
* @see PLII_TYPE
*
**/
GUINT32 GGetPL2(GHANDLE hAudioDev, PLII_TYPE *pePL2Type);
#endif

/**
* @brief set audio revervb type
*
* @param[in] hAudioDev audio device handle 
* @param[in] eReverbType the revervb type value
*
* @return GUINT32 define in GDef.h
*
*
**/
GINT32  GSetReverbType(GHANDLE hAudioDev, REVERBTYPE_T eReverbType);

/**
* @brief get audio revervb type
*
* @param[in] hAudioDev audio device handle 
* @param[out] peReverbType REVERBTYPE_T * the revervb type value
*
* @return GUINT32 define in GDef.h
*
* @see REVERBTYPE_T
*
**/
GUINT32 GGetReverbType(GHANDLE hAudioDev, REVERBTYPE_T *peReverbType);

/**
* @brief set audio upmix type
*
* @param[in] hAudioDev audio device handle 
* @param[in] eUpMixType the upmix type value
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 GSetUpMix(GHANDLE hAudioDev, AUD_UPMIX_T eUpMixType);

/**
* @brief get audio upmix type
*
* @param[in] hAudioDev audio device handle 
* @param[out] peUpMixType ONOFF_T * the upmix type value
*
* @return GUINT32 define in GDef.h
*
* @see ONOFF_T
*
**/
GUINT32 GGetUpMix(GHANDLE hAudioDev, AUD_UPMIX_T *peUpMixType);

/**
* @brief set audio loudness type
*
* @param[in] hAudioDev audio device handle 
* @param[in] uLoudNessType the loudness type value
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 GSetLoudNess(GHANDLE hAudioDev, GUINT8 uLoudNessType);

/**
* @brief get audio loudness type
*
* @param[in] hAudioDev audio device handle 
* @param[out] puLoudNessType GUINT8 * the loudness type value
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 GGetLoudNess(GHANDLE hAudioDev, GUINT8 *puLoudNessType);

/**
* @brief choose spdif output
*
* @param[in] hAudioDev audio device handle 
* @param[in] eOutType the spdif output value
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 GChooseSpdifOutput(GHANDLE hAudioDev, MISC_AUD_OUT_TYPE_T eOutType);


/**
* @brief  reset AP
*
* @return GRESULT define in GDef.h
*
*
**/

GUINT32 GAudioFuncOptionSet(GHANDLE hAudioDev, MISC_AUD_FUNC_OPTION_T rFuncOptionSet);



/**
* @brief  Set Audio Diversity Info
*
* @return GRESULT define in GSysServices.h
*
*
**/
GUINT32 GAudioSetDiversityInfo(GHANDLE hAudioDev, AUD_DEC_DIV_INFO_T *prAudDivInfo);

GRESULT AP_GReset(GVOID);

GRESULT AP_GClientReset(GVOID);

/**
* @brief  set DVP TV display type.
*
* @param[in] isCurrentSet current or default 
* @param[in] eTVDisplayType the TV display type
*
* @return GUINT32 define in GDef.h
*
* @see TVDISPLAY_T
*
**/
GUINT32 DVP_GSetDisplayType(GBOOL isCurrentSet, TVDISPLAY_T eTVDisplayType);

/**
* @brief  get DVP TV display type.
*
* @param[in] isCurrentSet current or default 
* @param[out] peTVDisplayType TVDISPLAY_T * the TV display type
*
* @return GUINT32 define in GDef.h
*
* @see TVDISPLAY_T
*
**/
GUINT32 DVP_GGetDisplayType(GBOOL isCurrentSet, TVDISPLAY_T *peTVDisplayType);

/**
* @brief  set DVP captions type.
*
* @param[in] isCurrentSet current or default 
* @param[in] eCaptionsType the captions type
*
* @return GUINT32 define in GDef.h
*
* @see ONOFF_T
*
**/
GUINT32 DVP_GSetCaptionsType(GBOOL isCurrentSet, ONOFF_T eCaptionsType);

/**
* @brief  get DVP captions type.
*
* @param[in] isCurrentSet current or default 
* @param[out] peCaptionsType ONOFF_T * the captions type
*
* @return GUINT32 define in GDef.h
*
* @see ONOFF_T
*
**/
GUINT32 DVP_GGetCaptionsType(GBOOL isCurrentSet, ONOFF_T *peCaptionsType);

/**
* @brief  set DVP screen saver type.
*
* @param[in] isCurrentSet current or default 
* @param[in] eScreenSaverType the screen saver type
*
* @return GUINT32 define in GDef.h
*
* @see ONOFF_T
*
**/
GUINT32 DVP_GSetScreenSaverType(GBOOL isCurrentSet, ONOFF_T eScreenSaverType);

/**
* @brief  get DVP screen saver type.
*
* @param[in] isCurrentSet current or default 
* @param[out] peScreenSaverType ONOFF_T * the screen saver type
*
* @return GUINT32 define in GDef.h
*
* @see ONOFF_T
*
**/
GUINT32 DVP_GGetScreenSaverType(GBOOL isCurrentSet, ONOFF_T *peScreenSaverType);

/**
* @brief  set DVP last mem type.
*
* @param[in] isCurrentSet current or default 
* @param[in] eLastMemType the last mem type
*
* @return GUINT32 define in GDef.h
*
* @see ONOFF_T
*
**/
GUINT32 DVP_GSetLastMemType(GBOOL isCurrentSet, ONOFF_T eLastMemType);

/**
* @brief  get DVP last mem type.
*
* @param[in] isCurrentSet current or default 
* @param[out] peLastMemType ONOFF_T * the last mem type
*
* @return GUINT32 define in GDef.h
*
* @see ONOFF_T
*
**/
GUINT32 DVP_GGetLastMemType(GBOOL isCurrentSet, ONOFF_T *peLastMemType);

/**
* @brief  set DVP TV type.
*
* @param[in] isCurrentSet current or default 
* @param[in] eTVType the TV type
*
* @return GUINT32 define in GDef.h
*
* @see TVTYPE_T
*
**/
GUINT32 DVP_GSetTVType(GBOOL isCurrentSet, TVTYPE_T eTVType);

/**
* @brief  get DVP TV type.
*
* @param[in] isCurrentSet current or default 
* @param[out] peTVType TVTYPE_T * the TV type
*
* @return GUINT32 define in GDef.h
*
* @see TVTYPE_T
*
**/
GUINT32 DVP_GGetTVType(GBOOL isCurrentSet, TVTYPE_T *peTVType);

/**
* @brief  set DVP PBC type.
*
* @param[in] isCurrentSet current or default 
* @param[in] ePBCType the PBC type
*
* @return GUINT32 define in GDef.h
*
* @see PBCTYPE_T
*
**/
GUINT32 DVP_GSetPBCType(GBOOL isCurrentSet, PBCTYPE_T ePBCType);

/**
* @brief  get DVP PBC type.
*
* @param[in] isCurrentSet current or default 
* @param[out] pePBCType PBCTYPE_T * the TV type
*
* @return GUINT32 define in GDef.h
*
* @see PBCTYPE_T
*
**/
GUINT32 DVP_GGetPBCType(GBOOL isCurrentSet, PBCTYPE_T *pePBCType);

/**
* @brief  set DVP audio language type.
*
* @param[in] isCurrentSet current or default 
* @param[in] eAudioLanType the audio language type
*
* @return GUINT32 define in GDef.h
*
* @see LANGUAGE_T
*
**/
GUINT32 DVP_GSetAudioLanType(GBOOL isCurrentSet, LANGUAGE_T eAudioLanType);

/**
* @brief  get DVP audio language type.
*
* @param[in] isCurrentSet current or default 
* @param[out] peAudioLanType LANGUAGE_T * the audio language type
*
* @return GUINT32 define in GDef.h
*
* @see LANGUAGE_T
*
**/
GUINT32 DVP_GGetAudioLanType(GBOOL isCurrentSet, LANGUAGE_T *peAudioLanType);

/**
* @brief  set DVP subtitle language type.
*
* @param[in] isCurrentSet current or default 
* @param[in] eSubLanType the subtitle language type
*
* @return GUINT32 define in GDef.h
*
* @see LANGUAGE_T
*
**/
GUINT32 DVP_GSetSubLanType(GBOOL isCurrentSet, LANGUAGE_T eSubLanType);

/**
* @brief  get DVP subtitle language type.
*
* @param[in] isCurrentSet current or default 
* @param[out] peSubLanType LANGUAGE_T * the subtitle language type
*
* @return GUINT32 define in GDef.h
*
* @see LANGUAGE_T
*
**/
GUINT32 DVP_GGetSubLanType(GBOOL isCurrentSet, LANGUAGE_T *peSubLanType);

/**
* @brief  set DVP menu language type.
*
* @param[in] isCurrentSet current or default 
* @param[in] eMenuLanType the menu language type
*
* @return GUINT32 define in GDef.h
*
* @see LANGUAGE_T
*
**/
GUINT32 DVP_GSetMenuLanType(GBOOL isCurrentSet, LANGUAGE_T eMenuLanType);

/**
* @brief  get DVP menu language type.
*
* @param[in] isCurrentSet current or default 
* @param[out] peMenuLanType LANGUAGE_T * the menu language type
*
* @return GUINT32 define in GDef.h
*
* @see LANGUAGE_T
*
**/
GUINT32 DVP_GGetMenuLanType(GBOOL isCurrentSet, LANGUAGE_T *peMenuLanType);

/**
* @brief  set DVP parental type.
*
* @param[in] isCurrentSet current or default 
* @param[in] eParentalType the parental type
*
* @return GUINT32 define in GDef.h
*
* @see PARENTAL_T
*
**/
GUINT32 DVP_GSetParentalType(GBOOL isCurrentSet, PARENTAL_T eParentalType);

/**
* @brief  get DVP parental type.
*
* @param[in] isCurrentSet current or default 
* @param[out] peParentalType PARENTAL_T * the parental type
*
* @return GUINT32 define in GDef.h
*
* @see PARENTAL_T
*
**/
GUINT32 DVP_GGetParentalType(GBOOL isCurrentSet, PARENTAL_T *peParentalType);

/**
* @brief  set DVP password mode type.
*
* @param[in] isCurrentSet current or default 
* @param[in] ePwdModeType the password mode type
*
* @return GUINT32 define in GDef.h
*
* @see ONOFF_T
*
**/
GUINT32 DVP_GSetPwdModeType(GBOOL isCurrentSet, ONOFF_T ePwdModeType);

/**
* @brief  get DVP password mode type.
*
* @param[in] isCurrentSet current or default 
* @param[out] pePwdModeType ONOFF_T * the password mode  type
*
* @return GUINT32 define in GDef.h
*
* @see ONOFF_T
*
**/
GUINT32 DVP_GGetPwdModeType(GBOOL isCurrentSet, ONOFF_T *pePwdModeType);

/**
* @brief set DVP password
*
* @param[in] isCurrentSet current or default 
* @param[in] pPassword GTCHAR* point to the password 
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 DVP_GSetPassword(GBOOL isCurrentSet, GTCHAR* pPassword);

/**
* @brief get DVP password
*
* @param[in] isCurrentSet current or default 
* @param[out] pPassword GTCHAR* point to the password
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 DVP_GGetPassword(GBOOL isCurrentSet, GTCHAR* pPassword);

/**
* @brief  reset DVP
*
* @return GRESULT define in GDef.h
*
*
**/
GRESULT DVP_GReset(GVOID);

/**
* @brief  set DVP audio speaker layout type and size
*
* @param[in] isCurrentSet current or default 
* @param[in] u4SpeakerLayoutType the speaker layout type
* @param[in] u4SpeakerSize the speaker size
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 DVP_GSetSpeakerLayout(GBOOL isCurrentSet, GUINT32 u4SpeakerLayoutType, GUINT32 u4SpeakerSize);

/**
* @brief  get DVP audio speaker layout type and size
*
* @param[in] isCurrentSet current or default
* @param[out] pu4SpeakerLayoutType GINT32 * the speaker layout type
* @param[out] pu4SpeakerSize the speaker size
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 DVP_GGetSpeakerLayout(GBOOL isCurrentSet, GUINT32* pu4SpeakerLayoutType, GUINT32* pu4SpeakerSize);

/**
* @brief  set DVP test tone type
*
* @param[in] isCurrentSet current or default
* @param[in] eTestToneType test tone type
*
* @return GUINT32 define in GDef.h
*
* @see ONOFF_T
*
**/
GUINT32 DVP_GSetTestToneType(GBOOL isCurrentSet, ONOFF_T eTestToneType);

/**
* @brief  get DVP test tone type
*
* @param[in] isCurrentSet current or default
* @param[out] peTestToneType ONOFF_T * test tone type
*
* @return GUINT32 define in GDef.h
*
* @see ONOFF_T
*
**/
GUINT32 DVP_GGetTestToneType(GBOOL isCurrentSet, ONOFF_T *peTestToneType);

/**
* @brief  set DVP dialog type
*
* @param[in] isCurrentSet current or default
* @param[in] uDialogType dialog type
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 DVP_GSetDialogType(GBOOL isCurrentSet, GUINT8 uDialogType);

/**
* @brief  get DVP dialog type
*
* @param[in] isCurrentSet current or default
* @param[out] puDialogType GUINT8 * dialog type
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 DVP_GGetDialogType(GBOOL isCurrentSet, GUINT8 *puDialogType);

/**
* @brief  set DVP spdif output type
*
* @param[in] isCurrentSet current or default
* @param[in] eSpdifOutputType spdif output type
*
* @return GUINT32 define in GDef.h
*
* @see SPDIFOUTPUT_T
*
**/
GUINT32 DVP_GSetSpdifOutputType( GBOOL isCurrentSet, SPDIFOUTPUT_T eSpdifOutputType);

/**
* @brief  get DVP spdif output type
*
* @param[in] isCurrentSet current or default
* @param[out] peSpdifOutputType SPDIFOUTPUT_T * spdif output type
*
* @return GUINT32 define in GDef.h
*
* @see SPDIFOUTPUT_T
*
**/
GUINT32 DVP_GGetSpdifOutputType( GBOOL isCurrentSet, SPDIFOUTPUT_T *peSpdifOutputType);

/**
* @brief  set DVP lpcm output type
*
* @param[in] isCurrentSet current or default
* @param[in] eLpcmOutType lpcm output type
*
* @return GUINT32 define in GDef.h
*
* @see LPCMOUT_T
*
**/
GUINT32 DVP_GSetLpcmOutType(GBOOL isCurrentSet, LPCMOUT_T eLpcmOutType);

/**
* @brief  get DVP lpcm output type
*
* @param[in] isCurrentSet current or default
* @param[out] peLpcmOutType LPCMOUT_T * lpcm output type
*
* @return GUINT32 define in GDef.h
*
* @see LPCMOUT_T
*
**/
GUINT32 DVP_GGetLpcmOutType(GBOOL isCurrentSet, LPCMOUT_T *peLpcmOutType);

/**
* @brief  set DVP center delay
*
* @param[in] isCurrentSet current or default
* @param[in] uCdelayCType center delay
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 DVP_GSetCdelayCType(GBOOL isCurrentSet, GUINT8 uCdelayCType);

/**
* @brief  get DVP center delay
*
* @param[in] isCurrentSet current or default
* @param[out] puCdelayCType  GUINT8 * center delay
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 DVP_GGetCdelayCType(GBOOL isCurrentSet, GUINT8 *puCdelayCType);

/**
* @brief  set DVP sub delay
*
* @param[in] isCurrentSet current or default
* @param[in] uCdelaySubType sub delay
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 DVP_GSetCdelaySubType(GBOOL isCurrentSet, GUINT8 uCdelaySubType);

/**
* @brief  get DVP sub delay
*
* @param[in] isCurrentSet current or default
* @param[out] puCdelaySubType  GUINT8 * sub delay
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 DVP_GGetCdelaySubType(GBOOL isCurrentSet, GUINT8 *puCdelaySubType);

/**
* @brief  set DVP LS delay
*
* @param[in] isCurrentSet current or default
* @param[in] uCdelayLSType LS delay
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 DVP_GSetCdelayLSType(GBOOL isCurrentSet, GUINT8 uCdelayLSType);

/**
* @brief  get DVP LS delay
*
* @param[in] isCurrentSet current or default
* @param[out] puCdelayLSType  GUINT8 * LS delay
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 DVP_GGetCdelayLSType(GBOOL isCurrentSet, GUINT8 *puCdelayLSType);

/**
* @brief  set DVP RS delay
*
* @param[in] isCurrentSet current or default
* @param[in] uCdelayRSType RS delay
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 DVP_GSetCdelayRSType(GBOOL isCurrentSet, GUINT8 uCdelayRSType);

/**
* @brief  get DVP RS delay
*
* @param[in] isCurrentSet current or default
* @param[out] puCdelayRSType  GUINT8 * RS delay
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 DVP_GGetCdelayRSType(GBOOL isCurrentSet, GUINT8 *puCdelayRSType);

/**
* @brief  set DVP EQ type
*
* @param[in] isCurrentSet current or default
* @param[in] eEQType EQ type
*
* @return GUINT32 define in GDef.h
*
* @see EQTYPE_T
*
**/
GUINT32 DVP_GSetEQType(GBOOL isCurrentSet, EQTYPE_T eEQType);

/**
* @brief  get DVP EQ type
*
* @param[in] isCurrentSet current or default
* @param[out] peEQType EQTYPE_T * EQ type
*
* @return GUINT32 define in GDef.h
*
* @see EQTYPE_T
*
**/
GUINT32 DVP_GGetEQType(GBOOL isCurrentSet, EQTYPE_T *peEQType);

#if MISC_SET_PL2
/**
* @brief  set DVP pro log type
*
* @param[in] isCurrentSet current or default
* @param[in] ePL2Type pro log type
*
* @return GUINT32 define in GDef.h
*
* @see PLII_TYPE
*
**/
GUINT32 DVP_GSetPL2(GBOOL isCurrentSet, PLII_TYPE ePL2Type);

/**
* @brief  get DVP pro log type
*
* @param[in] isCurrentSet current or default
* @param[out] pePL2Type PLII_TYPE * pro log type
*
* @return GUINT32 define in GDef.h
*
* @see PLII_TYPE
*
**/
GUINT32 DVP_GGetPL2(GBOOL isCurrentSet, PLII_TYPE *pePL2Type);
#endif

/**
* @brief  set DVP reverbrate type
*
* @param[in] isCurrentSet current or default
* @param[in] eReverbType reverbrate type
*
* @return GUINT32 define in GDef.h
*
* @see REVERBTYPE_T
*
**/
GINT32  DVP_GSetReverbType(GBOOL isCurrentSet, REVERBTYPE_T eReverbType);

/**
* @brief  get DVP reverbrate type
*
* @param[in] isCurrentSet current or default
* @param[out] peReverbType REVERBTYPE_T * reverbrate type
*
* @return GUINT32 define in GDef.h
*
* @see REVERBTYPE_T
*
**/
GINT32  DVP_GGetReverbType(GBOOL isCurrentSet, REVERBTYPE_T *peReverbType);

/**
* @brief  set DVP dual mono type
*
* @param[in] isCurrentSet current or default
* @param[in] eDualMonoType dual mono type
*
* @return GUINT32 define in GDef.h
*
* @see DUALMONO_T
*
**/
GUINT32 DVP_GSetDualMonoType(GBOOL isCurrentSet, DUALMONO_T eDualMonoType);

/**
* @brief get DVP dual mono type
*
* @param[in] isCurrentSet current or default
* @param[out] peDualMonoType DUALMONO_T * dual mono type
*
* @return GUINT32 define in GDef.h
*
* @see DUALMONO_T
*
**/
GUINT32 DVP_GGetDualMonoType(GBOOL isCurrentSet, DUALMONO_T *peDualMonoType);

/**
* @brief set DVP dynamic type
*
* @param[in] isCurrentSet current or default
* @param[in] uDynamicType dynamic type
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 DVP_GSetDynamicType(GBOOL isCurrentSet, GUINT8 uDynamicType);

/**
* @brief get DVP dynamic type
*
* @param[in] isCurrentSet current or default
* @param[out] puDynamicType dynamic type
*
* @return GUINT32 define in GDef.h
*
*
**/
GUINT32 DVP_GGetDynamicType(GBOOL isCurrentSet, GUINT8 *puDynamicType);


/**
* @brief set DVP front audio volume and don't save in regedit
*
* @param[in] u4Vol the front volume value to be set
*
* @return GUINT32 define in GDef.h
*
**/
GUINT32 DVP_GSetVolumeEx(GUINT32 u4Vol);

/**
* @brief set DVP front audio volume and save in regedit
*
* @param[in] isCurrentSet current or default 
* @param[in] u4Vol the front volume value to be set
*
* @return GUINT32 define in GDef.h
*
**/
GUINT32 DVP_GSetVolume(GBOOL isCurrentSet, GUINT32 u4Vol);

/**
* @brief get DVP front audio volume from regedit
*
* @param[in] isCurrentSet current or default 
* @param[out] pu4Vol pointer to front audio volume value
*
* @return GUINT32 define in GDef.h
*
**/
GUINT32 DVP_GGetVolume(GBOOL isCurrentSet, GUINT32 *pu4Vol);

GUINT32 DVP_GSetRearVolumeEx(GUINT32 u4Vol);

/**
* @brief set DVP rear audio volume and save in regedit
*
* @param[in] isCurrentSet current or default 
* @param[in] u4Vol the rear volume value to be set
*
* @return GUINT32 define in GDef.h
*
**/
GUINT32 DVP_GSetRearVolume(GBOOL isCurrentSet, GUINT32 u4Vol);

/**
* @brief get DVP rear audio volume from regedit
*
* @param[in] isCurrentSet current or default 
* @param[out] pu4Vol pointer to rear audio volume value
*
* @return GUINT32 define in GDef.h
*
**/
GUINT32 DVP_GGetRearVolume(GBOOL isCurrentSet, GUINT32 *pu4Vol);

/**
* @brief set DVP audio EQ on the specified band and save in regedit
*
* @param[in] isCurrentSet current or default 
* @param[in] u4Band the band value
* @param[in] i4Value the EQ value on u4Band
*
* @return GUINT32 define in GDef.h
*
**/
GUINT32 DVP_GSetEQValue(GBOOL isCurrentSet, GUINT u4Band, GINT32 i4Value);

/**
* @brief get DVP audio EQ value on the specified band
*
* @param[in] isCurrentSet current or default 
* @param[in] u4Band  the band value
* @param[out] pi4Value pointer to the EQ value on u4Band
*
* @return GUINT32 define in GDef.h
*
**/
GUINT32 DVP_GGetEQValue(GBOOL isCurrentSet, GUINT u4Band, GINT32* pi4Value);

GUINT32 DVP_GSetEQValues(GBOOL isCurrentSet, AUD_EQVALUES_T rEQValues);

GUINT32 DVP_GGetEQValues(GBOOL isCurrentSet, AUD_EQVALUES_T *prEQValues);



/**
* @brief set DVP audio balance on the specified type
*
* @param[in] isCurrentSet current or default
* @param[in] i4Values the balance value
* @param[in] eBalanceType the specified balace type
*
* @return GUINT32 define in GDef.h
*
*	@see BALANCE_CHANGE_TYPE
*
**/
GUINT32 DVP_GSetBalance(GBOOL isCurrentSet, GINT32 i4Values, BALANCE_CHANGE_TYPE eBalanceType);

/**
* @brief  get DVP audio balance on the specified type
*
* @param[in] isCurrentSet current or default 
* @param[out] pi4Values GINT32 * the balance value
* @param[in] eBalanceType the specified balace type
*
* @return GUINT32 define in GDef.h
*
* @see BALANCE_CHANGE_TYPE
*
**/
GUINT32 DVP_GGetBalance(GBOOL isCurrentSet, GINT32 *pi4Values, BALANCE_CHANGE_TYPE eBalanceType);

GUINT32 DVP_GSetBalances(GBOOL isCurrentSet, MISC_DVP_BALANCES_T rBalanceValues);

GUINT32 DVP_GGetBalances(GBOOL isCurrentSet, MISC_DVP_BALANCES_T *prBalanceValues);



/**
* @brief  set audio output
*
* @param[in] eAudCfg audio configuration 
* @param[in] eAudOut audio output
*
* @return GRESULT define in GDef.h
*
* @see AUD_CFG_ID
* @see AUD_OUT_TYPE_T
*
**/
GRESULT GSetAudOutput(AUD_CFG_ID eAudCfg, AUD_OUT_TYPE_T eAudOut);

/**
*@}
*/

GBOOL GGetAudFrontStatus();

GBOOL GGetAudRearStatus();
GUINT32 GAudSelDacType(AUD_CFG_ID eOut,AUD_DAC_TYPE_T eDacType);

#ifdef __cplusplus
}
#endif

#endif