#pragma once


//mtk
#include "GSysServices.h"
#include "GDef.h"
#include "MSDK_MediaGraph.h"
#include "MSDK_Global.h"
#include "MSDK_Error.h"
#include "MSDK_MediaCtrl.h"
#include "IniDefine.h"
#include "MSDK_MediaEvent.h"


#define  COLOR_SPACE_LOW_VALUE 0xF81F
#define  COLOR_SPACE_HIGH_VALUE 0xF81F



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
BOOL setVideoSource(UINT32 iVideoSource,BYTE iChannel);
BOOL VideoSourceInit(void);
BOOL vedeoSourceReleass(void);


BOOL DVDVideoSourceSelect(void);
BOOL DVDVideoSourceDelete(void);

void DeleteVideoSource(void);


BOOL IsHaveVideoSignal(BYTE iChannel);
BOOL ResetVideoSignal(BYTE iChannel);
BOOL TestVideoSignal(UINT16 iChannle);


struct VideoInfo{
	BOOL bDVDHaveVideo;
	BYTE iCurVideoSource;
	HMEDIAGRAPH m_hMediaGraph_V;

	BOOL bAUXVideoSource;
	BOOL bCDCVideoSource;
};

