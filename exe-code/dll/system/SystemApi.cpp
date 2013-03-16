#include "SystemApi.h"


//±³¹â 0~10
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
