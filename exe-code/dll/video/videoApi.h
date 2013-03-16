#pragma once


//mtk
#include "GSysServices.h"
#include "GDef.h"
#include "IniDefine.h"


#define IOCTL_DISPLAY_SET_YUV_GAIN_DEFAULT  0x20021
#define IOCTL_DISPLAY_SET_YUV_GAIN			0x20020
#define DISPLAY_SET_GAMMA_DATA				0x2000D

BOOL setBrightness(UINT32 setValue);
BOOL getBrightness(UINT32* pGetValue);
BOOL setContrast(UINT32 setValue);
BOOL getContrast(UINT32* pGetValue);
BOOL setHue(UINT32 setValue);
BOOL getHue(UINT32* pGetValue);
BOOL setSaturation(UINT32 setValue);
BOOL getSaturation(UINT32* pGetValue);
BOOL setBacklight(UINT32 setValue);
BOOL getBacklight(UINT32* pGetValue);
void setYUVGain(BOOL bDefault);
BOOL setVideoSource(UINT32 iVideoSource,BYTE iChannel);
BOOL videoSourceInit(void);
BOOL videoSourceReleass(void);
void setBackVideoSigned(BOOL bSigned);
void ControlCurAudioChannel(BYTE iChannel);
void ControlAuxSignal(BOOL bSignal);



struct VideoInfo{
	BYTE iCurVideoSource;
	BYTE iCurAudioSource;
	BOOL bBackVideoSigned;
	BOOL bAuxSignal;
};

