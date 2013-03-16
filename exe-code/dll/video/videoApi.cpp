#include "videoApi.h"
#include "flyshm.h"
#include "common.h"

struct VideoInfo sVideoInfo;

const INT32 gDisplayGain[64] = {
	0,	 0,	  0,   0,	0,	 1,	  2,   2,
	3,   4,   5,   6,   8,   9,   11,  13,
	14,  16,  18,  21,  23,  25,  28,  31,
	34,  37,  40,  43,  46,  50,  53,  57,
	61,  65,  69,  74,  78,  83,  87,  92,
	97,  102, 107, 113, 118, 124, 129, 135,
	141, 147, 154, 160, 167, 173, 180, 187,
	194, 201, 209, 216, 224, 232, 240, 248
};

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

//亮度 0~100
BOOL setBrightness(UINT32 setValue)
{
	//UINT32 getValue;
	//getBrightness(&getValue);
	//if (getValue == setValue)
	//	return FALSE;

	UINT32 u4RegValue = (VIDEO_BRIGHTNESS_MAX-VIDEO_BRIGHTNESS_MIN)*(setValue - VIDEO_BRIGHTNESS_UI_MIN) / (VIDEO_BRIGHTNESS_UI_MAX - VIDEO_BRIGHTNESS_UI_MIN)  + VIDEO_BRIGHTNESS_MIN;
	return GSetBrightnessLevel(u4RegValue);
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

//对比度 0~100
BOOL setContrast(UINT32 setValue)
{
	//UINT32 getValue;
	//getContrast(&getValue);
	//if (getValue == setValue)
	//	return FALSE;

	UINT32 u4RegValue = (VIDEO_CONTRAST_MAX-VIDEO_CONTRAST_MIN) *(setValue - VIDEO_CONTRAST_UI_MIN) / (VIDEO_CONTRAST_UI_MAX - VIDEO_CONTRAST_UI_MIN)   + VIDEO_CONTRAST_MIN;
	return GSetContrastLevel(u4RegValue);
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

//色调 0~100
BOOL setHue(UINT32 setValue)
{
	//UINT32 getValue;
	//getHue(&getValue);
	//if (getValue == setValue)
	//	return FALSE;

	UINT32 u4RegValue = (VIDEO_HUE_MAX-VIDEO_HUE_MIN) / (VIDEO_HUE_UI_MAX - VIDEO_HUE_UI_MIN) * (setValue - VIDEO_HUE_UI_MIN) + VIDEO_HUE_MIN;
	return GSetHueLevel(u4RegValue);
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

//包和度 0~100
BOOL setSaturation(UINT32 setValue)
{
	//UINT32 getValue;
	//getSaturation(&getValue);
	//if (getValue == setValue)
	//	return FALSE;

	UINT32 u4RegValue = (VIDEO_SATUTATION_MAX-VIDEO_SATUTATION_MIN) / (VIDEO_SATUTATION_UI_MAX - VIDEO_SATUTATION_UI_MIN) * (setValue - VIDEO_SATUTATION_UI_MIN) + VIDEO_SATUTATION_MIN;
	return GSetSaturationLevel(u4RegValue);
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

//背光 0~10
BOOL setBacklight(UINT32 setValue)
{
	//UINT32 getValue;
	//getBacklight(&getValue);
	//if (getValue == setValue)
	//	return FALSE;

	UINT32 u4RegValue = (VIDEO_BACKLIGHT_MAX-VIDEO_BACKLIGHT_MIN) / (VIDEO_BACKLIGHT_UI_MAX - VIDEO_BACKLIGHT_UI_MIN) * (setValue - VIDEO_BACKLIGHT_UI_MIN) + VIDEO_BACKLIGHT_MIN;
	return GSetBackLightLevel(u4RegValue);
}

void setBackVideoSigned(BOOL bSigned)
{
	sVideoInfo.bBackVideoSigned = bSigned;
}

//设置YUVgain
void setYUVGain(BOOL bDefault)
{
	HDC hdc = GetDC(NULL);	//set YUV gain

	if (bDefault)
	{
		//这个接口是设置成默认的YUV值的
		ExtEscape(hdc, IOCTL_DISPLAY_SET_YUV_GAIN_DEFAULT, 0, NULL, 0, NULL);
	}
	else
	{
		unsigned int gain[3] = {0xD0, 0xA0, 0xA0};//基本上好了
		ExtEscape(hdc, IOCTL_DISPLAY_SET_YUV_GAIN, sizeof(gain), (LPCSTR)gain, 0, NULL);
	}

	ReleaseDC(NULL, hdc);
}

//设置DISPLAY gain
static void setDisplayGain(void)
{
	HDC hdcDisplay = GetDC(NULL);
	ExtEscape(hdcDisplay,  DISPLAY_SET_GAMMA_DATA, 0, (LPCSTR)gDisplayGain, 0, NULL);
	ReleaseDC(NULL, hdcDisplay);
}

static BOOL VideoInfoInit(void)
{
	sVideoInfo.iCurVideoSource = InitAV;
	sVideoInfo.iCurAudioSource = InitAV;
	sVideoInfo.bBackVideoSigned = FALSE;
	return TRUE;
}
void ControlCurAudioChannel(BYTE iChannel)
{
	sVideoInfo.iCurAudioSource = iChannel;
}

static BOOL DeleteAUXDependOnBackCar(BYTE iChannel)
{
	//音频在AUX通道，AUX有视频
	if (sVideoInfo.iCurVideoSource == BACK && iChannel == AUX)//倒车退出
	{
		SendMsgToModules(MP3_MODULE, SET_VIDEO, MP3_VIDEO_BACK_OFF);
		return TRUE;
	}
	else if (sVideoInfo.iCurVideoSource == AUX && iChannel == BACK)//倒车进入
	{
		SendMsgToModules(MP3_MODULE, SET_VIDEO, MP3_VIDEO_BACK_ON);
		return TRUE;
	}
	//音频在AUX通道，AUX无视频
	else if (sVideoInfo.iCurAudioSource == AUX)
	{	
		if (sVideoInfo.iCurVideoSource == InitAV && iChannel == BACK)
		{
			SendMsgToModules(MP3_MODULE, SET_VIDEO, MP3_VIDEO_BACK_ON);	
			return TRUE;
		}
		else if (sVideoInfo.iCurVideoSource == BACK && iChannel == InitAV)
		{
			SendMsgToModules(MP3_MODULE, SET_VIDEO, MP3_VIDEO_BACK_OFF);
			return TRUE;
		}
	}

	//Sleep(1000);
	return FALSE;
}

void ControlAuxSignal(BOOL bSignal)
{
	sVideoInfo.bAuxSignal = bSignal;
}
//清除以前的视频源
static BOOL DeleteVideoSource(BYTE iChannel)
{
	//if (sVideoInfo.iCurVideoSource == MediaCD)
	//	SendMsgToModules(DVD_MODULE, SET_VIDEO, DVD_VIDEO_OFF);
	//else if (sVideoInfo.iCurVideoSource == BACK)
	//{
	//	SendMsgToModules(MP3_MODULE, SET_VIDEO, MP3_VIDEO_BACK_OFF);
	//}
	//else //MP3模块
	//{
	//	SendMsgToModules(MP3_MODULE, SET_VIDEO, MP3_OFF);
	//}

	//if (sVideoInfo.iCurVideoSource == MediaCD)
	//	SendMsgToModules(DVD_MODULE, SET_VIDEO, DVD_VIDEO_OFF);
	//else if (sVideoInfo.iCurVideoSource == AUX && iChannel == BACK)//倒车进入
	//{
	//	SendMsgToModules(MP3_MODULE, SET_VIDEO, MP3_VIDEO_BACK_ON);
	//	SendMsgToModules(MP3_MODULE, SET_VIDEO, MP3_OFF);
	//}
	//else if (sVideoInfo.iCurVideoSource == BACK && iChannel == AUX)//倒车退出
	//{
	//	SendMsgToModules(MP3_MODULE, SET_VIDEO, MP3_VIDEO_BACK_OFF);
	//}
	//else //MP3模块
	//{
	//	SendMsgToModules(MP3_MODULE, SET_VIDEO, MP3_OFF);
	//}
	
	BOOL bReturnValue;

	if (DeleteAUXDependOnBackCar(iChannel))
	{
		bReturnValue = FALSE;
	}
	else
	{
		if (sVideoInfo.iCurVideoSource == MediaCD)
			SendMsgToModules(DVD_MODULE, SET_VIDEO, DVD_VIDEO_OFF);
		else //MP3模块
		{
			SendMsgToModules(MP3_MODULE, SET_VIDEO, MP3_OFF);
		}
		bReturnValue = TRUE;
	}

	return bReturnValue;
}

BOOL videoSourceInit(void)
{
	VideoInfoInit();

	return TRUE;
}

BOOL videoSourceReleass(void)
{
	return TRUE;
}

//视频切换
BOOL setVideoSource(UINT32 iVideoSource, BYTE iChannel)
{
	if(!DeleteVideoSource(iChannel))
	{
		sVideoInfo.iCurVideoSource = iChannel;
		return TRUE;
	}
	
	if (iVideoSource == S_NEXTER_SOURCE) 
	{
		if (iChannel == MediaCD)
		{
			SendMsgToModules(DVD_MODULE, SET_VIDEO, DVD_VIDEO_ON);
		}
		else if (iChannel == MediaMP3)
		{
			SendMsgToModules(MP3_MODULE, SET_VIDEO, MP3_VIDEO_MP3);
		}
		else if (iChannel == InitAV)
		{
			SendMsgToModules(MP3_MODULE, SET_VIDEO, MP3_OFF);
		}
		else if (iChannel == VAP)
		{
			SendMsgToModules(MP3_MODULE, SET_VIDEO, MP3_VIDEO_VAP);
		}
		else if (iChannel == BACK)
		{
			//SendMsgToModules(MP3_MODULE, SET_VIDEO, MP3_VIDEO_BACK_ON);
		}
	}
	else //AVIN切入
	{
		SendMsgToModules(MP3_MODULE, SET_VIDEO, iVideoSource);
	}

	sVideoInfo.iCurVideoSource = iChannel;

	return TRUE;
}
