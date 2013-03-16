#include "videoApi.h"
#include "flyshm.h"
#include "common.h"

struct VideoInfo sVideoInfo;

//亮度 0~100
BOOL setBrightness(UINT32 setValue)
{
	UINT32 u4RegValue = (VIDEO_BRIGHTNESS_MAX-VIDEO_BRIGHTNESS_MIN)*(setValue - VIDEO_BRIGHTNESS_UI_MIN) / (VIDEO_BRIGHTNESS_UI_MAX - VIDEO_BRIGHTNESS_UI_MIN)  + VIDEO_BRIGHTNESS_MIN;
	return GSetBrightnessLevel(u4RegValue);
}
BOOL getBrightness(UINT32* pGetValue)
{
	UINT32 u4RegValue;
	if(GGetBrightnessLevel(&u4RegValue))
	{
		if (u4RegValue < VIDEO_BRIGHTNESS_MIN )
		{
			u4RegValue = VIDEO_BRIGHTNESS_MIN;
		}
		*pGetValue = (u4RegValue - VIDEO_BRIGHTNESS_MIN) * (VIDEO_BRIGHTNESS_UI_MAX - VIDEO_BRIGHTNESS_UI_MIN)/(VIDEO_BRIGHTNESS_MAX-VIDEO_BRIGHTNESS_MIN) + VIDEO_BRIGHTNESS_UI_MIN;
		return TRUE;
	}
	else
	{
		*pGetValue = _wtoi(INI_KEY_VIDEO_BRIGHTNESS_DEFAULT);
		return FALSE;
	}
}

//对比度 0~100
BOOL setContrast(UINT32 setValue)
{
	UINT32 u4RegValue = (VIDEO_CONTRAST_MAX-VIDEO_CONTRAST_MIN) *(setValue - VIDEO_CONTRAST_UI_MIN) / (VIDEO_CONTRAST_UI_MAX - VIDEO_CONTRAST_UI_MIN)   + VIDEO_CONTRAST_MIN;
	return GSetContrastLevel(u4RegValue);
}
BOOL getContrast(UINT32* pGetValue)
{
	UINT32 u4RegValue;
	if(GGetContrastLevel(&u4RegValue))
	{
		if (u4RegValue < VIDEO_CONTRAST_MIN )
		{
			u4RegValue = VIDEO_CONTRAST_MIN;
		}
		*pGetValue = (u4RegValue - VIDEO_CONTRAST_MIN)* (VIDEO_CONTRAST_UI_MAX - VIDEO_CONTRAST_UI_MIN) / (VIDEO_CONTRAST_MAX-VIDEO_CONTRAST_MIN ) + VIDEO_CONTRAST_UI_MIN;
		return TRUE;
	}
	else
	{
		*pGetValue = _wtoi(INI_KEY_VIDEO_CONTRAST_DEFAULT);
		return FALSE;
	}
}

//色调 0~100
BOOL setHue(UINT32 setValue)
{
	UINT32 u4RegValue = (VIDEO_HUE_MAX-VIDEO_HUE_MIN) / (VIDEO_HUE_UI_MAX - VIDEO_HUE_UI_MIN) * (setValue - VIDEO_HUE_UI_MIN) + VIDEO_HUE_MIN;
	return GSetHueLevel(u4RegValue);
}
BOOL getHue(UINT32* pGetValue)
{
	UINT32 u4RegValue;
	if(GGetHueLevel(&u4RegValue))
	{
		*pGetValue = (u4RegValue - VIDEO_HUE_MIN) / ((VIDEO_HUE_MAX-VIDEO_HUE_MIN) / (VIDEO_HUE_UI_MAX - VIDEO_HUE_UI_MIN)) + VIDEO_HUE_UI_MIN;
		return TRUE;
	}
	else
	{
		*pGetValue = _wtoi(INI_KEY_VIDEO_HUE_DEFAULT);
		return FALSE;
	}
}

//包和度 0~100
BOOL setSaturation(UINT32 setValue)
{
	UINT32 u4RegValue = (VIDEO_SATUTATION_MAX-VIDEO_SATUTATION_MIN) / (VIDEO_SATUTATION_UI_MAX - VIDEO_SATUTATION_UI_MIN) * (setValue - VIDEO_SATUTATION_UI_MIN) + VIDEO_SATUTATION_MIN;
	return GSetSaturationLevel(u4RegValue);
}
BOOL getSaturation(UINT32* pGetValue)
{
	UINT32 u4RegValue;
	if(GGetSaturationLevel(&u4RegValue))
	{
		*pGetValue = (u4RegValue - VIDEO_SATUTATION_MIN) / ((VIDEO_SATUTATION_MAX-VIDEO_SATUTATION_MIN) / (VIDEO_SATUTATION_UI_MAX - VIDEO_SATUTATION_UI_MIN)) + VIDEO_SATUTATION_UI_MIN;
		return TRUE;
	}
	else
	{
		*pGetValue = _wtoi(INI_KEY_VIDEO_SATUTATION_DEFAULT);
		return FALSE;
	}
}
//背光 0~10
BOOL setBacklight(UINT32 setValue)
{
	UINT32 u4RegValue = (VIDEO_BACKLIGHT_MAX-VIDEO_BACKLIGHT_MIN) / (VIDEO_BACKLIGHT_UI_MAX - VIDEO_BACKLIGHT_UI_MIN) * (setValue - VIDEO_BACKLIGHT_UI_MIN) + VIDEO_BACKLIGHT_MIN;
	return GSetBackLightLevel(u4RegValue);
}

BOOL getBacklight(UINT32* pGetValue)
{
	UINT32 u4RegValue;
#if 1
	if(GGetBackLightLevel(&u4RegValue))
	{
		*pGetValue = (u4RegValue - VIDEO_BACKLIGHT_MIN) / ((VIDEO_BACKLIGHT_MAX-VIDEO_BACKLIGHT_MIN) / (VIDEO_BACKLIGHT_UI_MAX - VIDEO_BACKLIGHT_UI_MIN)) + VIDEO_BACKLIGHT_UI_MIN;
		return TRUE;
	}
	else
#endif
	{
		*pGetValue = _wtoi(INI_KEY_VIDEO_BACKLIGHT_DEFAULT);
		return FALSE;
	}
}


static BOOL ExVideoSourceDelete(void)
{
	if (sVideoInfo.m_hMediaGraph_V != NULL)
	{
		MediaCtrl_StopAvin(sVideoInfo.m_hMediaGraph_V);
		MediaGraph_Clear(sVideoInfo.m_hMediaGraph_V);
		//MediaGraph_Release(gAudioApiInfo.hMediaGraph_A);
	}

	return TRUE;
}

static BOOL VideoInfoInit(void)
{
	sVideoInfo.m_hMediaGraph_V = NULL;
	sVideoInfo.iCurVideoSource = InitAV;
	sVideoInfo.bDVDHaveVideo = TRUE;



	sVideoInfo.bAUXVideoSource = TRUE;
	sVideoInfo.bCDCVideoSource = TRUE;
	return TRUE;
}

BOOL VideoSourceInit(void)
{
	VideoInfoInit();

	MRESULT     rt;
	rt = MSDK_Init();
	if (S_MSDK_OK != rt)
	{
		RETAILMSG(1, (TEXT("[Video DLL]: MSDK_Init error\r\n")));
		return FALSE;
	}    

	//取得一个视频句柄
	rt = MediaGraph_Create(&sVideoInfo.m_hMediaGraph_V);
	if (S_MSDK_OK != rt)
	{
		RETAILMSG(1, (TEXT("[Video DLL]: MediaGraph_Create Video error\r\n")));
		return FALSE;
	}    
	return TRUE;
}

BOOL vedeoSourceReleass(void)
{
	MediaGraph_Release(sVideoInfo.m_hMediaGraph_V);
	sVideoInfo.m_hMediaGraph_V = NULL;
	return TRUE;
}

void DeletePreVideoSource(BYTE iChannel)
{
	if (sVideoInfo.iCurVideoSource == MediaCD 
		&&iChannel == InitAV)
	{
		SendMsgToModules(DVD_MODULE, SET_VIDEO, DVD_OFF);
	}
	
	if (iChannel != MediaCD) 
	{
		ExVideoSourceDelete();
		//SendMsgToDVDMoudle(0x12, 0x00);
	}
}


BOOL TestVideoSignal(UINT16 iChannle)
{
	MRESULT     rt = 0;
	MSDK_AVIN_SINK_INFO_T rSinkInfo_V;
	BOOL bIsSignalReady = FALSE;

	//SetRect(&rSinkInfo_V.rFrontDestRect,-50, -50, -50, -50);
	//rSinkInfo_V.eSink = MSDK_SINK_FRONT;
	//rSinkInfo_V.u4FrontFlag = DDOVER_HIDE; 
	//rSinkInfo_V.rFrontOvfx.dckSrcColorkey.dwColorSpaceHighValue = 0;
	//rSinkInfo_V.rFrontOvfx.dckSrcColorkey.dwColorSpaceLowValue = 0; 

	////MediaCtrl_StopAvin(sVideoInfo.m_hMediaGraph_V);
	////MediaGraph_Clear(sVideoInfo.m_hMediaGraph_V);
	//rt = MediaGraph_SetVideoInSource(sVideoInfo.m_hMediaGraph_V, (E_MSDK_AVIN_VINDEX)iChannle, &rSinkInfo_V);
	//if (S_MSDK_OK != rt)
	//{
	//	RETAILMSG(1,(TEXT("[Video DLL] set video in source err.\r\n")));
	//	return FALSE;
	//}	

	Sleep(200);
	MediaGraph_VdoInGetIsSignalReady(sVideoInfo.m_hMediaGraph_V,&bIsSignalReady);
	if (!bIsSignalReady)
	{
		sVideoInfo.bAUXVideoSource = FALSE;
		RETAILMSG(1,(TEXT("\r\nVIDEO SOURCE IS NOT FIND\r\n")));
	}
	else
	{
		//SetRect(&rSinkInfo_V.rFrontDestRect,0, 0, 800, 480);
		//rSinkInfo_V.eSink = MSDK_SINK_FRONT;
		//rSinkInfo_V.u4FrontFlag = DDOVER_SHOW | DDOVER_KEYSRCOVERRIDE; 
		//rSinkInfo_V.rFrontOvfx.dckSrcColorkey.dwColorSpaceHighValue = 0;
		//rSinkInfo_V.rFrontOvfx.dckSrcColorkey.dwColorSpaceLowValue = 0; 
		//MediaGraph_SetVideoInInfo(sVideoInfo.m_hMediaGraph_V, &rSinkInfo_V);
		sVideoInfo.bAUXVideoSource = TRUE;
		RETAILMSG(1,(TEXT("\r\nVIDEO SOURCE IS  FIND\r\n")));
	}

	return sVideoInfo.bAUXVideoSource;
}
BOOL IsHaveVideoSignal(BYTE iChannel)
{
	BOOL bSignal = FALSE;

	if (iChannel == AUX)
	{
		///bSignal = TRUE;
		bSignal = sVideoInfo.bAUXVideoSource;
	}
	//else if (iChannel == CDC)
	//{
	//	bSignal = sVideoInfo.bCDCVideoSource;
	//}

	return bSignal;
}

BOOL ResetVideoSignal(BYTE iChannel)
{
	if (iChannel == AUX)
		sVideoInfo.bAUXVideoSource = TRUE;
	else if (iChannel == CDC)
		sVideoInfo.bCDCVideoSource = TRUE;

	return TRUE;
}

//视频切换
BOOL setVideoSource(UINT32 iVideoSource, BYTE iChannel)
{
	MRESULT     rt = 0;
	MSDK_AVIN_SINK_INFO_T rSinkInfo_V;
	BOOL bIsSignalReady = FALSE;

	DeletePreVideoSource(iChannel);
	
	if (iVideoSource == S_NEXTER_SOURCE) 
	{
		if (iChannel == InitAV && sVideoInfo.iCurVideoSource == MediaCD)
		{
		}
		else if (iChannel == MediaCD)
		{
			SendMsgToModules(DVD_MODULE, SET_VIDEO, DVD_ON);
		}
	}
	else
	{
		SetRect(&rSinkInfo_V.rFrontDestRect,0, 0, 800, 480);
		rSinkInfo_V.eSink = MSDK_SINK_FRONT;
		rSinkInfo_V.u4FrontFlag = DDOVER_SHOW | DDOVER_KEYSRCOVERRIDE; 
		rSinkInfo_V.rFrontOvfx.dckSrcColorkey.dwColorSpaceHighValue = 0;
		rSinkInfo_V.rFrontOvfx.dckSrcColorkey.dwColorSpaceLowValue = 0; 

		rt = MediaGraph_SetVideoInSource(sVideoInfo.m_hMediaGraph_V, (E_MSDK_AVIN_VINDEX)iVideoSource, &rSinkInfo_V);
		if (S_MSDK_OK != rt)
		{
			RETAILMSG(1,(TEXT("[Video DLL] set video in source err.\r\n")));
			return FALSE;
		}	

		if (iChannel == AUX)
		{	
			Sleep(200);
			MediaGraph_VdoInGetIsSignalReady(sVideoInfo.m_hMediaGraph_V,&bIsSignalReady);
			if (!bIsSignalReady)
			{
				sVideoInfo.bAUXVideoSource = FALSE;
				RETAILMSG(1,(TEXT("\r\nVIDEO SOURCE IS NOT FIND\r\n")));
			}
			else
			{
				sVideoInfo.bAUXVideoSource = TRUE;
				RETAILMSG(1,(TEXT("\r\nVIDEO SOURCE IS  FIND\r\n")));
			}
		}
	

		//SetRect(&rSinkInfo_V.rFrontDestRect,0, 0, 800, 480);
		//rSinkInfo_V.eSink = MSDK_SINK_FRONT;
		//rSinkInfo_V.u4FrontFlag = DDOVER_SHOW | DDOVER_KEYSRCOVERRIDE; 
		//rSinkInfo_V.rFrontOvfx.dckSrcColorkey.dwColorSpaceHighValue = 0;
		//rSinkInfo_V.rFrontOvfx.dckSrcColorkey.dwColorSpaceLowValue = 0; 
		//MediaGraph_SetVideoInInfo(sVideoInfo.m_hMediaGraph_V, &rSinkInfo_V);
	}

	sVideoInfo.iCurVideoSource = iChannel;

	return TRUE;
}
