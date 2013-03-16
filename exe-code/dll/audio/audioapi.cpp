#include "audioapi.h"
#include <windows.h>
#include "tlhelp32.h"
#include "common.h"
#include "flyshm.h"

#pragma comment(lib, "Toolhelp.lib")


#if MISC_CLIENT_SET_GAIN

#define LOUDNESS_MIN   0
#define LOUDNESS_MAX   20

MISC_LOUDNESS_GAIN_T  gLoudNessGain[20] =
{
	{0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000}, 
	{0x00100000,0x00e0b3d9,0x000f4e20,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e0be81,0x000f43b6,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e0c9cb,0x000f38b1,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e0d5c0,0x000f2d0a,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e0e26a,0x000f20b6,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e0efd5,0x000f13ae,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e0fe0b,0x000f05e5,0x001f5636,0x00f0a807,0x005a9c78},

	{0x00100000,0x00e10d19,0x000ef753,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e10d19,0x000ef753,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e10d19,0x000ef753,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e10d19,0x000ef753,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e10d19,0x000ef753,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e10d19,0x000ef753,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e10d19,0x000ef753,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e10d19,0x000ef753,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e10d19,0x000ef753,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e10d19,0x000ef753,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e10d19,0x000ef753,0x001f5636,0x00f0a807,0x005a9c78},
	{0x00100000,0x00e10d19,0x000ef753,0x001f5636,0x00f0a807,0x005a9c78}
};

#define MAX_LEVEL 29

GUINT32 g_ganValues[MAX_LEVEL]={
	0xFFFF3315,0xFFFF3950,0xFFFF404E,0xFFFF4827,0xFFFF50F5,0xFFFF5AD6,0xFFFF65EB,
	0xFFFF725A,0xFFFF804E,0xFFFF8FF6,0xFFFFA187,0xFFFFB53C,0xFFFFCB5A,0xFFFFE42A,
	0x00000000,0x00001F3C,0x00004248,0x0000699C,0x000095BB,0x0000C73D,0x0000FEC9,
	0x00013D1C,0x0001830A,0x0001D181,0x0002298B,0x00028C52,0x0002FB27,0x00037782,
	0x0004030A
};


GUINT32 g_dryValues[MAX_LEVEL]={
	0x00003314,0x0000394F,0x0000404D,0x00004826,0x000050F4,0x00005AD5,0x000065EA,
	0x00007259,0x0000804D,0x00008FF5,0x0000A186,0x0000B53B,0x0000CB59,0x0000E429,
	0x00010000,0x00011F3C,0x00014248,0x0001699C,0x000195BB,0x0001C73D,0x0001FEC9,
	0x00023D1C,0x0002830A,0x0002D181,0x0003298B,0x00038C52,0x0003FB27,0x00047782,
	0x0005030A
};


#define TRIM_LEVEL_MAX  41

const GUINT32 _au4TrimValue[TRIM_LEVEL_MAX] =
{
	0x0,     0x203a,  0x4074,  0x60ae,  0x80e8,  0xa122,  0xc15c,  0xe196, 
	0x101d0, 0x1220a, 0x14244, 0x1627e, 0x182b8, 0x1a2f2, 0x1c32c, 0x1e366, 
	0x203a0, 0x223da, 0x24414, 0x2644e, 0x28488, 0x28488, 0x28488, 0x28488,
	0x28488, 0x28488, 0x28488, 0x28488, 0x28488, 0x28488, 0x28488, 0x28488,
	0x28488, 0x28488, 0x28488, 0x28488, 0x28488, 0x28488, 0x28488, 0x28488,
	0x28488

	//0x0000A1E9,0x0000AB81,0x0000B5AA,0x0000C06E,0x0000CBD5,0x0000D7E9,0x0000E4B4,0x0000F241,
	//0x0001009C,0x00010FD0,0x00011FEB,0x000130FB,0x0001430D,0x00015631,0x00016A78,0x00017FF2,
	//0x000196B2,0x0001AECB,0x0001C852,0x0001E35C,0x00020000,0x00021E57,0x00023E79,0x00026083,
	//0x00028492,0x0002AAC3,0x0002D338,0x0002FE13,0x00032B77,0x00035B8C,0x00038E7B,0x0003C46E,
	//0x0003FD93,0x00043A1B,0x00047A3A,0x0004BE25,0x00050616,0x0005524B,0x0005A303,0x0005F884,
	//0x00065316
};

MISC_EQ_GAIN_T gEQTypeGain[]={
	//CFG_EQ_OFF
	{0x00010000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000},
	//CFG_EQ_ROCK
	{0x00010000,0x0002298B,0x0001830A,0x0000C73D,0x00000000,0xFFFFCB5A,0x00000000,0x00004248,0x000095BB,0x0001830A,0x0002298B},
	//CFG_EQ_POP
	{0x00010000,0xFFFFCB5A,0x00000000,0x000095BB,0x0001830A,0x0002298B,0x0001830A,0x0000C73D,0x00001F3C,0xFFFFCB5A,0xFFFFA187},
	//CFG_EQ_LIVING
	{0x00010000,0x0004030A,0x0002FB27,0x0001830A,0x00004248,0xFFFFCB5A,0x00000000,0x0000FEC9,0x0002298B,0x0001830A,0x000095BB},
	//CFG_EQ_DANCE
	{0x00010000,0x0002298B,0x0001D181,0x00013D1C,0x000095BB,0x00000000,0xFFFFB53C,0xFFFF8FF6,0xFFFFB53C,0xFFFFE42A,0x00000000},
	//CFG_EQ_CLASSICAL
	{0x00010000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0xFFFFE42A,0xFFFFCB5A,0xFFFFA187,0xFFFF8FF6},
	//CFG_EQ_SOFT
	{0x00010000,0x0000C73D,0x000095BB,0x0000699C,0x00004248,0x00001F3C,0xFFFFE42A,0xFFFFB53C,0xFFFF8FF6,0xFFFF725A,0xFFFF65EB}
};

const MISC_REVERB_COEF_T gAudSeReverbCoefTab[] = {
	// 0. off
	{
		// Gain
		0x000000,
			0x000000,
		{
			{0x00, 0x00, 0x00, 0x00},   // 32K, LSB Bank0 ... MSB Bank3 Size
			{0x00, 0x00, 0x00, 0x00},   // 44K, LSB Bank0 ... MSB Bank3 Size
			{0x00, 0x00, 0x00, 0x00},   // 48K, LSB Bank0 ... MSB Bank3 Size
		}
	},

	// 1. Living Room
	{
		// Gain
		0x180000,     // Reverb gain = 0x00 ~ 0x7FFFFF
			0x7FFFFF,     // Reverb FeedBack Gain
		{
			// Bank Size ( 0 > 1 > 2 > 3 ) = 0 ~ 79
			{0x29, 0x1F, 0x17, 0x0D},   // 32K, LSB Bank0 ... MSB Bank3 Size
			{0x29, 0x1F, 0x17, 0x0D},   // 44K, LSB Bank0 ... MSB Bank3 Size
			{0x29, 0x1F, 0x17, 0x0D},   // 48K, LSB Bank0 ... MSB Bank3 Size
		}
	},

		// 2. Hall
	{
		// Gain
		0x300000,
			0x7FFFFF,
		{
			{0x4F, 0x3B, 0x25, 0x13},   // 32K, LSB Bank0 ... MSB Bank3 Size
			{0x4F, 0x3B, 0x25, 0x13},   // 44K, LSB Bank0 ... MSB Bank3 Size
			{0x4F, 0x3B, 0x25, 0x13},   // 48K, LSB Bank0 ... MSB Bank3 Size
		}
	},

		// 3. Concert
	{
		// Gain
		0x200000,
			0x7FFFFF,
		{
			{0x4F, 0x43, 0x3B, 0x2F},   // 32K, LSB Bank0 ... MSB Bank3 Size
			{0x4F, 0x43, 0x3B, 0x2F},   // 44K, LSB Bank0 ... MSB Bank3 Size
			{0x4F, 0x43, 0x3B, 0x2F},   // 48K, LSB Bank0 ... MSB Bank3 Size
		}
	},

		// 4. Cave
	{
		// Gain
		0x600000,
			0x7FFFFF,
		{
			{0x3B, 0x2F, 0x25, 0x17},   // 32K, LSB Bank0 ... MSB Bank3 Size
			{0x3B, 0x2F, 0x25, 0x17},   // 44K, LSB Bank0 ... MSB Bank3 Size
			{0x3B, 0x2F, 0x25, 0x17},   // 48K, LSB Bank0 ... MSB Bank3 Size
		}
	},

		// 5. Bathroom
	{
		// Gain
		0x400000,
			0x7FFFFF,
		{
			{0x13, 0x11, 0x0B, 0x07},   // 32K, LSB Bank0 ... MSB Bank3 Size
			{0x13, 0x11, 0x0B, 0x07},   // 44K, LSB Bank0 ... MSB Bank3 Size
			{0x13, 0x11, 0x0B, 0x07},   // 48K, LSB Bank0 ... MSB Bank3 Size
		}
	},

		// 6. Arena
	{
		// Gain
		0x100000,
			0x7FFFFF,
		{
			{0x4F, 0x49, 0x47, 0x43},   // 32K, LSB Bank0 ... MSB Bank3 Size
			{0x4F, 0x49, 0x47, 0x43},   // 44K, LSB Bank0 ... MSB Bank3 Size
			{0x4F, 0x49, 0x47, 0x43},   // 48K, LSB Bank0 ... MSB Bank3 Size
		}
	},

};


#endif 



struct audioapi_t gAudioApiInfo;


//高音
BOOL setTreble(UINT32 setValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue;
	MISC_EQ_GAIN_T eqGain;
	UINT32 uBass,uMid,uTrebleValue=setValue;

	getBass(&uBass);
	getMid(&uMid);
	if (uBass > 28)
		uBass = 28;
	if (uMid > 28)
		uMid = 28;


	eqGain.u4Gain0  = g_dryValues[14];
	eqGain.u4Gain1  = g_ganValues[uBass];
	eqGain.u4Gain2  = g_ganValues[uBass];
	eqGain.u4Gain3  = g_ganValues[uBass];
	eqGain.u4Gain4  = g_dryValues[14];
	eqGain.u4Gain5  = g_ganValues[uMid];
	eqGain.u4Gain6  = g_ganValues[uMid];
	eqGain.u4Gain7  = g_dryValues[14];
	eqGain.u4Gain8  = g_ganValues[uTrebleValue];
	eqGain.u4Gain9  = g_ganValues[uTrebleValue]; 
	eqGain.u4Gain10 = g_ganValues[uTrebleValue];

	if ((uBass==0) && (uTrebleValue==0))
	{
		u4ReturnValue = GClientSetEQValues(hAudioDev,eqGain,FALSE);

	}
	else
	{
		u4ReturnValue = GClientSetEQValues(hAudioDev,eqGain,TRUE);
	}

	GCloseAudioDev(hAudioDev);
	return u4ReturnValue;
}

BOOL getTreble(UINT32 *getValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue;

	int i = 0;
	int iIndex = 0;
	MISC_EQ_GAIN_T mEQgain; 

	//u4ReturnValue = GGetTreble(hAudioDev, &u4RegValue);
	//*getValue = (u4RegValue - AUDIO_TREBLE_MIN) / ((AUDIO_TREBLE_MAX-AUDIO_TREBLE_MIN) / (AUDIO_TREBLE_UI_MAX - AUDIO_TREBLE_UI_MIN)) + AUDIO_TREBLE_UI_MIN;
#if MISC_CLIENT_SET_GAIN
	u4ReturnValue = GClientGetEQValues(hAudioDev,&mEQgain);

	for(i=0;i < MAX_LEVEL;i++)
	{
		if (mEQgain.u4Gain10 == g_ganValues[i])
		{
			iIndex = i;
		}
	}

	if ((iIndex < 0) ||
		(iIndex > MAX_LEVEL))
	{
		*getValue = 0;
	}
	else
	{
		*getValue = iIndex - 14; 
	}

#else

	u4ReturnValue = GGetEQValue(hAudioDev, 10, (INT32*) getValue);
#endif 

	GCloseAudioDev(hAudioDev);

	//做一些处理
	*getValue = *getValue + 14;


	return u4ReturnValue;
}
//中音
BOOL setMid(UINT32 setValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue;
	
	MISC_EQ_GAIN_T eqGain;
	UINT32 uTreble,uBass,uMid =setValue;
	getTreble(&uTreble);
	getBass(&uBass);
	if (uTreble > 28)
		uTreble = 28;
	if (uBass > 28)
		uBass = 28;

	eqGain.u4Gain0  = g_dryValues[14];
	eqGain.u4Gain1  = g_ganValues[uBass];
	eqGain.u4Gain2  = g_ganValues[uBass];
	eqGain.u4Gain3  = g_ganValues[uBass];
	eqGain.u4Gain4  = g_dryValues[14];
	eqGain.u4Gain5  = g_ganValues[uMid];
	eqGain.u4Gain6  = g_ganValues[uMid];
	eqGain.u4Gain7  = g_dryValues[14];
	eqGain.u4Gain8  = g_ganValues[uTreble];
	eqGain.u4Gain9  = g_ganValues[uTreble]; 
	eqGain.u4Gain10 = g_ganValues[uTreble];
	

	if ((uMid==0) && (uTreble==0) && uBass == 0)
	{
		u4ReturnValue = GClientSetEQValues(hAudioDev,eqGain,FALSE);
	}
	else
	{
		u4ReturnValue = GClientSetEQValues(hAudioDev,eqGain,TRUE);
	}


	GCloseAudioDev(hAudioDev);
	return u4ReturnValue;
}

BOOL getMid(UINT32 *getValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue;
	MISC_EQ_GAIN_T mEQgain; 

	int  i = 0;
	int  iIndex = 0;

#if MISC_CLIENT_SET_GAIN

	u4ReturnValue = GClientGetEQValues(hAudioDev,&mEQgain);
	for(i=0;i < MAX_LEVEL;i++)
	{
		if (mEQgain.u4Gain5 == g_ganValues[i])
		{
			iIndex = i;
		}
	}

	if ((iIndex < 0) ||(iIndex > MAX_LEVEL))
	{
		*getValue = 0;
	}
	else
	{
		*getValue = iIndex - 14; 
	}
#endif 

	GCloseAudioDev(hAudioDev);

	*getValue = *getValue + 14;

	return u4ReturnValue;
}

//低音
BOOL setBass(UINT32 setValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue;
	MISC_EQ_GAIN_T eqGain;
	UINT32 uTreble,uMid,uBass =setValue;

	getTreble(&uTreble);
	getMid(&uMid);
	if (uTreble > 28)
		uTreble = 28;
	if (uMid > 28)
		uMid = 28;


	eqGain.u4Gain0  = g_dryValues[14];
	eqGain.u4Gain1  = g_ganValues[uBass];
	eqGain.u4Gain2  = g_ganValues[uBass];
	eqGain.u4Gain3  = g_ganValues[uBass];
	eqGain.u4Gain4  = g_dryValues[14];
	eqGain.u4Gain5  = g_ganValues[uMid];
	eqGain.u4Gain6  = g_ganValues[uMid];
	eqGain.u4Gain7  = g_dryValues[14];
	eqGain.u4Gain8  = g_ganValues[uTreble]; 
	eqGain.u4Gain9  = g_ganValues[uTreble]; 
	eqGain.u4Gain10 = g_ganValues[uTreble];

	if ((uBass==0) && (uTreble==0))
	{
		u4ReturnValue = GClientSetEQValues(hAudioDev,eqGain,FALSE);
	}
	else
	{
		u4ReturnValue = GClientSetEQValues(hAudioDev,eqGain,TRUE);

	}
	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}

BOOL getBass(UINT32 *getValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue;
	MISC_EQ_GAIN_T mEQgain; 

	int  i = 0;
	int  iIndex = 0;


	//u4ReturnValue = GGetBass(hAudioDev, &u4RegValue);
	//*getValue = (u4RegValue - AUDIO_BASS_MIN) / ((AUDIO_BASS_MAX-AUDIO_BASS_MIN) / (AUDIO_BASS_UI_MAX - AUDIO_BASS_UI_MIN)) + AUDIO_BASS_UI_MIN;
#if MISC_CLIENT_SET_GAIN
	//u4ReturnValue = GGetEQValue(hAudioDev, 0, (INT32*) getValue);
	//u4ReturnValue = GGetEQValue(hAudioDev, 1, (INT32*) getValue);
	u4ReturnValue = GClientGetEQValues(hAudioDev,&mEQgain);

	//*getValue = mEQgain.u4Gain1;
	for(i=0;i < MAX_LEVEL;i++)
	{
		if (mEQgain.u4Gain1 == g_ganValues[i])
		{
			iIndex = i;
		}
	}

	if ((iIndex < 0) ||
		(iIndex > MAX_LEVEL))
	{
		*getValue = 0;
	}
	else
	{
		*getValue = iIndex - 14; 
	}
#else
	u4ReturnValue = GGetEQValue(hAudioDev, 1, (INT32*) getValue);

#endif 

	GCloseAudioDev(hAudioDev);

	*getValue = *getValue + 14;

	return u4ReturnValue;
}

////平衡
BOOL setBalance(UINT32 iFL,UINT32 iFR,UINT32 iRL,UINT32 iRR)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue;

	u4ReturnValue = GClientSetBalance(hAudioDev, _au4TrimValue[iFL], BALANCE_FRONT_LEFT);
	u4ReturnValue = GClientSetBalance(hAudioDev, _au4TrimValue[iFR], BALANCE_FRONT_RIGHT);
	u4ReturnValue = GClientSetBalance(hAudioDev, _au4TrimValue[iRL], BALANCE_REAR_LEFT);
	u4ReturnValue = GClientSetBalance(hAudioDev, _au4TrimValue[iRR], BALANCE_REAR_RIGHT);

	GCloseAudioDev(hAudioDev);
	return u4ReturnValue;
}

BOOL getBalance(BYTE iIndex,UINT32 *getValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue, u4RegValue=0;
	INT32 i4ReadValue = 0;
	GUINT32 i4LeftValue;
	int i;

	//TCHAR szGetString[4];
	//getStorageValue(INI_TYPE_USER, INI_KEY_AUDIO_BALANCE_NAME, szGetString, INI_KEY_AUDIO_BALANCE_DEFAULT);
	//*getValue = _wtoi(szGetString);

	//u4ReturnValue = GGetBalance(hAudioDev, &u4RegValue);
	//*getValue = (u4RegValue - AUDIO_TREBLE_MIN) / ((AUDIO_TREBLE_MAX-AUDIO_TREBLE_MIN) / (AUDIO_TREBLE_UI_MAX - AUDIO_TREBLE_UI_MIN)) + AUDIO_TREBLE_UI_MIN;

#if MISC_CLIENT_SET_GAIN

	if (iIndex == 0)
	{
		u4ReturnValue = GClientGetBalance(hAudioDev, &i4LeftValue, BALANCE_FRONT_LEFT);
	}
	else
	{
		u4ReturnValue = GClientGetBalance(hAudioDev, &i4LeftValue, BALANCE_REAR_LEFT);
	}

	for (i=0;i<TRIM_LEVEL_MAX;i++)
	{
		if (_au4TrimValue[i] == i4LeftValue)
		{
			i4ReadValue = i;
			break;
		}

	}
#else
	u4ReturnValue = GGetBalance(hAudioDev, &i4ReadValue, BALANCE_FRONT_LEFT);
#endif 

	if(u4ReturnValue)
	{
		*getValue = i4ReadValue;
		//*getValue = (UINT32) (20-i4ReadValue);
	}

	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}


//响度
BOOL setLoudness(UINT32 setValue)
{
	//UINT32 u4ReturnValue;
	GHANDLE hAudioDev = GOpenAudioDev();

	//0~20
	UINT32 u4LoudnessValue = setValue;

#if MISC_CLIENT_SET_GAIN
	if ((LOUDNESS_MIN<=u4LoudnessValue) && (u4LoudnessValue<LOUDNESS_MAX))
	{
		GClientSetLoudNess(hAudioDev, u4LoudnessValue,gLoudNessGain[u4LoudnessValue]);
	}
#else

	GSetLoudNess(hAudioDev, u4LoudnessValue);

#endif 

	GCloseAudioDev(hAudioDev);

	return TRUE;
}

BOOL getLoudness(UINT32 *getValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue=0;
	MISC_LOUDNESS_GAIN_T uLoudNessGain;

	GUINT8 uLoudNess;

#if MISC_CLIENT_SET_GAIN
	u4ReturnValue = GClientGetLoudNess(hAudioDev, &uLoudNess,&uLoudNessGain);
	*getValue = (UINT32)uLoudNess;
#else
	u4ReturnValue = GGetLoudNess(hAudioDev, getValue);

#endif 
	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;

}

//前排音量
BOOL setFrontValume(UINT32 setValue)
{
	GUINT32 uVolGain = 0;
	UINT32 u4ReturnValue;

	GHANDLE hAudioDev = GOpenAudioDev();
	gAudioApiInfo.iPreForntVolume = setValue;//前排
	u4ReturnValue = GClientSetVolume(hAudioDev, setValue);
	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}

BOOL getFrontValume(UINT32 *getValue)
{
	UINT32 u4ReturnValue;
	GUINT32 uMode = 0;
	GUINT32 uVolGain = 0;

	GHANDLE hAudioDev = GOpenAudioDev();
	u4ReturnValue = GClientGetVolume(hAudioDev, &uVolGain);
	*getValue = uVolGain;
	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}

//EX音量
BOOL setFrontValumeEx(UINT32 setValue)
{
	GUINT32 uVolGain = 0;
	UINT32 u4ReturnValue;
	GHANDLE hAudioDev = GOpenAudioDev();

	if (setValue > 100)
		setValue = 100;

	//uVolGain = setValue*FULL_VOLUME_GAIN/MAX_VOLUME_INDEX;
	u4ReturnValue = GClientSetVolumeEx(hAudioDev, setValue);

	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}


//后排音量
BOOL setBackValume(UINT32 setValue)
{
	UINT32 u4ReturnValue;
	GUINT32 uVolGain = 0;

	GHANDLE hAudioDev = GOpenAudioDev();
	u4ReturnValue = GClientSetRearVolume(hAudioDev, setValue);
	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}
BOOL getBackValume(UINT32 *getValue)
{
	UINT32 u4ReturnValue;
	GUINT32 uMode = 0;
	GUINT32 uVolGain = 0;

	GHANDLE hAudioDev = GOpenAudioDev();
	u4ReturnValue = GClientGetRearVolume(hAudioDev, &uVolGain);
	*getValue = uVolGain;
	GCloseAudioDev(hAudioDev);

	return TRUE;
}

//EX音量
BOOL setBackValumeEx(UINT32 setValue)
{
	GUINT32 uVolGain = 0;
	UINT32 u4ReturnValue;
	GHANDLE hAudioDev = GOpenAudioDev();

	if (setValue > 100)
		setValue = 100;

	//uVolGain = setValue*FULL_VOLUME_GAIN/MAX_VOLUME_INDEX;
	u4ReturnValue = GSetRearVolumeEx(hAudioDev, setValue);

	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}


//蓝牙音量
BOOL setBTVolume(UINT32 iVolume)
{
	gAudioApiInfo.iPreBTVolume = iVolume;
	waveOutMessage(HWAVEOUT(0), 0x412, iVolume,0);
	return TRUE;
}

//蓝牙来电声音
BOOL setBTRingVolume(UINT32 iVolume)
{
#if GPS_BT_VOLUME_SET_SPK_FORNT
	gAudioApiInfo.iPreBTRingVolume = ((iVolume<<16 | iVolume) & 0xff00);
#else
	gAudioApiInfo.iPreBTRingVolume = (iVolume<<16 | iVolume);
#endif
	
	SendMsgToModules(BT_MODULE, SET_VOLUME, gAudioApiInfo.iPreBTRingVolume);
	return TRUE;
}
BOOL setIpodVolume(UINT32 iVolume)
{
	gAudioApiInfo.iPreIopdVolume = iVolume;
	SendMsgToModules(IPOD_MODULE, SET_VOLUME, gAudioApiInfo.iPreIopdVolume);
	return TRUE;;
}

BOOL setDVPValume(UINT32 setValue)
{
	gAudioApiInfo.iPreDVPVolume = setValue;
	SendMsgToModules(DVD_MODULE, SET_VOLUME, gAudioApiInfo.iPreDVPVolume);
	return TRUE;
}
static void SetGPSVolumeProcessID(UINT32 iCurGPSProcessID)
{
	//取得地图卡的进程ID
	if (iCurGPSProcessID == 0)
	{
		gAudioApiInfo.iPreGPSProcessID = 0;
	}
	else
	{
		if (gAudioApiInfo.iPreGPSProcessID != iCurGPSProcessID)
		{
			gAudioApiInfo.iPreGPSProcessID = iCurGPSProcessID;
#if GPS_BT_VOLUME_SET_SPK_FORNT
			waveOutMessage((HWAVEOUT)0, MM_SET_GPS_OUTPUT_CH, 0,GPS_BT_RING_SPKOUT_FROM_FRONT_L);									//GPS语音输出到L上
#else 
			waveOutMessage((HWAVEOUT)0, MM_SET_GPS_OUTPUT_CH, 0,GPS_BT_RING_SPKOUT_FROM_FRONT_LR);									//GPS语音输出到L+R上
#endif
			waveOutMessage((HWAVEOUT)0, WM_NAVI_SET_VOLUME_RANGE, 0, 40);							//设置分段数40
			waveOutMessage((HWAVEOUT)0, WM_NAVI_SET_VOLUME, 0,gAudioApiInfo.iPreGPSVolume);
			waveOutMessage((HWAVEOUT)0, MM_SET_GPS_PROCESSID, 0, gAudioApiInfo.iPreGPSProcessID);	//发送进程ID

		}
	}
}
//导航音量
BOOL setGPSVolume(UINT16 iVolume)
{
	UINT ret = 0;
	UINT32 iTempVolume = 0;

#if GPS_BT_VOLUME_SET_SPK_FORNT
	iTempVolume = ((iVolume<<16 | iVolume)&0xFF00);
#else
	iTempVolume = (iVolume<<16 | iVolume);
#endif

	//调节音量
	if (gAudioApiInfo.iPreGPSVolume != iTempVolume)
	{
		gAudioApiInfo.iPreGPSVolume = iTempVolume;
		if (gAudioApiInfo.iPreGPSProcessID != 0)
		{
			DBGI((TEXT("\r\nGPS volume:0x%x \r\n"),gAudioApiInfo.iPreGPSVolume));
			waveOutMessage((HWAVEOUT)0, WM_NAVI_SET_VOLUME, 0,gAudioApiInfo.iPreGPSVolume);
			//waveOutMessage((HWAVEOUT)0, MM_SET_GPS_OUTPUT_CH, 0,0);		//GPS语音输出到L+R上
		}
	}

	//没有SD卡（SD卡拔出）
	ret = GetFileAttributes(L"\\SDMEM");
	if (ret == -1)
	{
		SetGPSVolumeProcessID(0);
	}
	DBGD((TEXT("\r\nGPS process id is :0x%X SD k :%d\r\n"),gAudioApiInfo.iPreGPSProcessID,ret));

	return TRUE;
}


//MP3音量
BOOL setMP3Volume(UINT16 iVolume)
{
	gAudioApiInfo.iPreMP3Volume = iVolume;
	SendMsgToModules(MP3_MODULE, SET_VOLUME, gAudioApiInfo.iPreMP3Volume);
	return TRUE;
}

static BOOL GSetDVDSpeakerLayout(UINT16 iSpkLayout, UINT16 iSpkSize)
{
	if (gAudioApiInfo.bDvdInitStatus){
		UINT32  temp = 0;
		temp = ((iSpkLayout|temp)<<16) + iSpkSize;
		SendMsgToModules(DVD_MODULE, SET_DVDSPK, temp);
		return TRUE;
	}
	return FALSE;
}

//静音
BOOL setMute(UINT32 iMute)
{
	if (1 == iMute)//静音
	{
		//音量设为0
		setFrontValume(0);		//前排
		setBackValume(0);		//后排
		setBTVolume(0);			//蓝牙
		setBTRingVolume(0);		//蓝牙来电铃声
		setMP3Volume(0);		//MP3
		setDVPValume(0);		//DVD
		setIpodVolume(0);

		//静音标志置位
		gAudioApiInfo.bCurMute = TRUE;
	}
	else
	{
		//音量设回原来的
		setFrontValume(gAudioApiInfo.iPreForntVolume);		//前排
		setBackValume(gAudioApiInfo.iPreForntVolume);		//后排
		setBTVolume(gAudioApiInfo.iPreBTVolume);			//蓝牙
		setBTRingVolume(gAudioApiInfo.iPreBTRingVolume);	//蓝牙来电铃声
		setMP3Volume(gAudioApiInfo.iPreMP3Volume);			//MP3
		setDVPValume(gAudioApiInfo.iPreDVPVolume);			//DVD
		setIpodVolume(gAudioApiInfo.iPreIopdVolume);

		DBGD((TEXT("[Audio dll] Mute off: front and back:%d,bt:%d,BTring:%d,mp3:%d,dvd:%d,\r\n"),
			gAudioApiInfo.iPreForntVolume,gAudioApiInfo.iPreBTVolume,gAudioApiInfo.iPreBTRingVolume,
			gAudioApiInfo.iPreMP3Volume,gAudioApiInfo.iPreDVPVolume));

		//静音标志清0
		gAudioApiInfo.bCurMute = FALSE;
	}

	return TRUE;
}

BOOL setAudioThreshold(UINT32 iThreshold)
{
	//dB = 20Log(0xFFFFFF /Threshold )
	UINT32 u4ReturnValue;
	//MISC_AUD_THRESHOLD_T rThreshold;
	//GHANDLE hAudioDev = GOpenAudioDev();
	//
	//rThreshold.u4FrontThrshld    = iThreshold;
	//rThreshold.u4RearThrshld     = 0x100;
	//rThreshold.u4WaveFormThrshld = 0x3;
	//
	//
	//DBGD((TEXT("\r\n[Audio dll] Threshold: %X\r\n"),iThreshold));
	//u4ReturnValue = GSetAudThreshold(hAudioDev, rThreshold);
	//GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}

BOOL getMute(UINT32 *iMute)
{
	*iMute = gAudioApiInfo.bCurMute;
	return TRUE;
}


BYTE getEQCurType(BYTE iTemp,BOOL bGetType)
{
	BYTE iType;
	if (!bGetType)
	{
		if (0x01 == iTemp)		//缺省 (舞曲)
			iType = CFG_EQ_DANCE;
		else if (0x02 == iTemp)//古典
			iType = CFG_EQ_CLASSICAL;
		else if (0x03 == iTemp)//流行
			iType = CFG_EQ_POP;
		else if (0x04 == iTemp)//摇滚
			iType = CFG_EQ_ROCK;
		else if (0x05 == iTemp)//爵士（逼真）
			iType = CFG_EQ_LIVING;
		else if (0x06 == iTemp)//温和
			iType = CFG_EQ_SOFT;
		else
			iType=CFG_EQ_OFF;
	}
	else
	{
		if (CFG_EQ_DANCE == iTemp)		//缺省 (舞曲)
			iType = 0x01;
		else if (CFG_EQ_CLASSICAL == iTemp)//古典
			iType = 0x02;
		else if (CFG_EQ_POP == iTemp)//流行
			iType = 0x03;
		else if (CFG_EQ_ROCK == iTemp)//摇滚
			iType = 0x04;
		else if (CFG_EQ_LIVING == iTemp)//爵士（逼真）
			iType = 0x05;
		else if (CFG_EQ_SOFT == iTemp)//温和
			iType = 0x06;
		else
			iType=0x00;
	}

	return iType;
}
//EQ类型
BOOL setEQType(UINT32 setValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	GClientSetEQType(hAudioDev, (EQTYPE_T)setValue,gEQTypeGain[setValue]);
	GCloseAudioDev(hAudioDev);

	return TRUE;
}

BOOL getEQType(UINT32 *getValue)
{
	EQTYPE_T eEQType;
	GHANDLE hAudioDev = GOpenAudioDev();

	GGetEQType(hAudioDev,&eEQType);
	GCloseAudioDev(hAudioDev);

	*getValue = (UINT32)eEQType;

	return TRUE;
}

BOOL dealEQGains(MISC_EQ_GAIN_T *pEqGain,BYTE *buf,UINT16 len,BOOL bSetValue)
{
	if (len != EQ_VOLUE_TOTAL_NUM)
		return FALSE;

	if (bSetValue)
	{
		if (EQ_VOLUE_TOTAL_NUM == 10)
		{
			 pEqGain->u4Gain0  = g_dryValues[14]; //dry
			 pEqGain->u4Gain1  = g_ganValues[buf[0]]; //31.25HZ
			
			 pEqGain->u4Gain2  = g_ganValues[buf[1]]; //62.5HZ
			 pEqGain->u4Gain3  = g_ganValues[buf[2]]; //125HZ
			 pEqGain->u4Gain4  = g_ganValues[buf[3]]; //250HZ
			 pEqGain->u4Gain5  = g_ganValues[buf[4]]; //500HZ
			 pEqGain->u4Gain6  = g_ganValues[buf[5]]; //1KHZ
			 pEqGain->u4Gain7  = g_ganValues[buf[6]]; //2KHZ
			 pEqGain->u4Gain8  = g_ganValues[buf[7]]; //4KHZ
			 pEqGain->u4Gain9  = g_ganValues[buf[8]]; //8KHZ
			 pEqGain->u4Gain10 = g_ganValues[buf[9]];//16KHZ
		}
		else if (EQ_VOLUE_TOTAL_NUM == 9)
		{
			pEqGain->u4Gain0  = g_dryValues[14]; //dry
			pEqGain->u4Gain1  = g_dryValues[14]; //31.25HZ

			pEqGain->u4Gain2  = g_ganValues[buf[0]]; //62.5HZ
			pEqGain->u4Gain3  = g_ganValues[buf[1]]; //125HZ
			pEqGain->u4Gain4  = g_ganValues[buf[2]]; //250HZ
			pEqGain->u4Gain5  = g_ganValues[buf[3]]; //500HZ
			pEqGain->u4Gain6  = g_ganValues[buf[4]]; //1KHZ
			pEqGain->u4Gain7  = g_ganValues[buf[5]]; //2KHZ
			pEqGain->u4Gain8  = g_ganValues[buf[6]]; //4KHZ
			pEqGain->u4Gain9  = g_ganValues[buf[7]]; //8KHZ
			pEqGain->u4Gain10 = g_ganValues[buf[8]];//16KHZ
		}
	}

	return TRUE;
}


//EQ的值
BOOL setEQValues(MISC_EQ_GAIN_T eqGain)
{
	UINT32 u4ReturnValue;
	GHANDLE hAudioDev = GOpenAudioDev();
	u4ReturnValue =GClientSetEQValues(hAudioDev,eqGain,TRUE);
	GCloseAudioDev(hAudioDev);
	return TRUE;
}

BOOL getEQValues(AUD_EQVALUES_T *prEQValues)
{	
	GHANDLE hAudioDev = GOpenAudioDev();
	GGetEQValues(hAudioDev, prEQValues);
	GCloseAudioDev(hAudioDev);

	return TRUE;
}

//混响
BOOL setReverbType(UINT32 setValue)
{
	REVERBTYPE_T eReverbType;

	GHANDLE hAudioDev = GOpenAudioDev();
	GClientSetReverbType(hAudioDev,(REVERBTYPE_T)setValue,gAudSeReverbCoefTab[setValue]);
	GCloseAudioDev(hAudioDev);

	return TRUE;
}

BOOL getReverbType(UINT32 *getValue)
{
	REVERBTYPE_T eReverbType;

	GHANDLE hAudioDev = GOpenAudioDev();
	GGetReverbType(hAudioDev,&eReverbType);
	GCloseAudioDev(hAudioDev);
	*getValue= (BYTE)eReverbType;

	return TRUE;
}

BOOL setPL2(UINT32 setValue)
{
	UINT32 u4ReturnValue;
	GHANDLE hAudioDev = GOpenAudioDev();
	u4ReturnValue = GSetPL2(hAudioDev,(PLII_TYPE)setValue);
	GCloseAudioDev(hAudioDev);
	return u4ReturnValue;
}


BOOL setAudioDefaultFeatures(void)
{
	setEQType(CFG_EQ_OFF);
	setPL2(AUD_SE_PL2_OFF);
	setReverbType(CFG_REVERB_OFF);
	return TRUE;
}

#define  DEF_MTK_SET 0
#if  !DEF_MTK_SET

//声道扩展
BOOL setUpmix(AUD_UPMIX_T eAudUpMix)
{

	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue;

	MISC_UPMIX_GAIN_T rUpmixGain;

	if (eAudUpMix == AUD_UPMIX_OFF)
	{
		rUpmixGain.u4Gain0 = 0x00000000;
		rUpmixGain.u4Gain1 = 0x00000000;
		rUpmixGain.u4Gain2 = 0x00000000;
		rUpmixGain.u4Gain3 = 0x00000000;
		rUpmixGain.u4Gain4 = 0x00000000;
		rUpmixGain.u4Gain5 = 0x00000000;
		rUpmixGain.u4Gain6 = 0x00000000;
		rUpmixGain.u4Gain7 = 0x00000000;

		//二声道，声音从前左，前右出来
		GSetSpeakerLayout(hAudioDev, (SPEAKER_LAYOUT_STEREO), 0x1F); 
		//GSetDVDSpeakerLayout( (SPEAKER_LAYOUT_STEREO), 0x1F);
	}
	else
	{
		rUpmixGain.u4Gain0 = 0x333333;
		rUpmixGain.u4Gain1 = 0x333333;//绿色，后左

		rUpmixGain.u4Gain2 = 0x333333;
		rUpmixGain.u4Gain3 = 0x333333;//紫色，后右

		rUpmixGain.u4Gain4 = 0x666666;
		rUpmixGain.u4Gain5 = 0x666666;

		rUpmixGain.u4Gain6 = 0x666666;//白色，前左
		rUpmixGain.u4Gain7 = 0x666666;//综色，前右

		//rUpmixGain.u4Gain0 = 0x007FFFFF;
		//rUpmixGain.u4Gain1 = 0;//绿色，后左

		//rUpmixGain.u4Gain2 = 0;
		//rUpmixGain.u4Gain3 = 0x007FFFFF;//紫色，后右

		//rUpmixGain.u4Gain4 = 0x003FFFFF;
		//rUpmixGain.u4Gain5 = 0x003FFFFF;

		//rUpmixGain.u4Gain6 = 0x007FFFFF;//白色，前左
		//rUpmixGain.u4Gain7 = 0x007FFFFF;//综色，前右

		//四声道，声音从前左，前右，后左，后右出来
		GSetSpeakerLayout(hAudioDev, (SPEAKER_LAYOUT_LRCLSRS), 0x1F);
		//GSetDVDSpeakerLayout( (SPEAKER_LAYOUT_LRCLSRS), 0x1F);
	}

	u4ReturnValue = GClientSetUpMix(hAudioDev,(AUD_UPMIX_T) eAudUpMix,rUpmixGain);

	GCloseAudioDev(hAudioDev);

	return TRUE;

}

static BOOL setDownMix(UINT32 iSetValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue, u4TempSpeakerLayout;

	UINT32 u4SpeakerLayout=0, u4SpeakerSize=0;
	GGetSpeakerLayout(hAudioDev, &u4SpeakerLayout, &u4SpeakerSize);


	switch(iSetValue)
	{
	case SETTING_AUDIO_DOWNMIX_LTRT:
		u4TempSpeakerLayout = SPEAKER_LAYOUT_LR;
		break;
	case SETTING_AUDIO_DOWNMIX_STEREO:
		u4TempSpeakerLayout = SPEAKER_LAYOUT_STEREO;
		break;
	case SETTING_AUDIO_DOWNMIX_NONE:
		u4TempSpeakerLayout = SPEAKER_LAYOUT_LRCLSRS;
		break;
	}

	u4SpeakerLayout = (u4SpeakerLayout & (~SPEAKER_LAYOUT_MASK)) | u4TempSpeakerLayout;
	u4ReturnValue = GSetSpeakerLayout(hAudioDev, u4SpeakerLayout, u4SpeakerSize);
	//u4ReturnValue = GSetDVDSpeakerLayout(u4SpeakerLayout,u4SpeakerSize);
	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}


//前置喇叭
static BOOL setFrontSpeak(UINT32 iSetValue)
{
	UINT32 u4ReturnValue;
	GHANDLE hAudioDev = GOpenAudioDev();

	UINT32 u4SpeakerLayout=0, u4SpeakerSize=0;
	GGetSpeakerLayout(hAudioDev, &u4SpeakerLayout, &u4SpeakerSize);

	//TODO
	if(iSetValue == SETTING_AUDIO_FRONT_LARGE)
		u4SpeakerLayout |= (SPEAKER_LAYOUT_L_LARGE | SPEAKER_LAYOUT_R_LARGE);
	else
		u4SpeakerLayout &= (~(SPEAKER_LAYOUT_L_LARGE | SPEAKER_LAYOUT_R_LARGE));

	u4ReturnValue = GSetSpeakerLayout(hAudioDev, u4SpeakerLayout, u4SpeakerSize);
	//u4ReturnValue = GSetDVDSpeakerLayout(u4SpeakerLayout, u4SpeakerSize);
	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}

//中置喇叭
static BOOL setCenterSpeak(UINT32 iSetValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue, u4TempSpeakerLayout;

	UINT32 u4SpeakerLayout=0, u4SpeakerSize=0;
	GGetSpeakerLayout(hAudioDev, &u4SpeakerLayout, &u4SpeakerSize);


	//TODO
	switch(iSetValue)
	{
	case SETTING_AUDIO_CENTER_LARGE:
		u4SpeakerSize |= SPEAKER_LAYOUT_C_LARGE;
		u4TempSpeakerLayout = SPEAKER_LAYOUT_LRCLSRS;                                                                               
		break;

	case SETTING_AUDIO_CENTER_SMALL:
		u4SpeakerSize &= (~SPEAKER_LAYOUT_C_LARGE);
		u4TempSpeakerLayout = SPEAKER_LAYOUT_LRCLSRS;                                                                
		break;

	case SETTING_AUDIO_CENTER_NONE:      
		u4TempSpeakerLayout = SPEAKER_LAYOUT_LRLSRS;                     //Then Force to Stereo 
		break;
	}

	u4SpeakerLayout= (u4SpeakerLayout & (~SPEAKER_LAYOUT_MASK)) | u4TempSpeakerLayout;

	u4ReturnValue = GSetSpeakerLayout(hAudioDev, u4SpeakerLayout,u4SpeakerSize);
	//u4ReturnValue = GSetDVDSpeakerLayout(u4SpeakerLayout, u4SpeakerSize);
	GCloseAudioDev(hAudioDev);
	return u4ReturnValue;
}

//环绕喇叭
static BOOL setRearSpeak(UINT32 iSetValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue, u4TempSpeakerLayout;

	UINT32 u4SpeakerLayout=0, u4SpeakerSize=0;
	GGetSpeakerLayout(hAudioDev, &u4SpeakerLayout, &u4SpeakerSize);

	//TODO
	switch(iSetValue)
	{
	case SETTING_AUDIO_REAR_LARGE:
		u4SpeakerSize |= (SPEAKER_LAYOUT_LS_LARGE | SPEAKER_LAYOUT_RS_LARGE);
		u4TempSpeakerLayout = SPEAKER_LAYOUT_LRCLSRS;                                                          
		break;
	case SETTING_AUDIO_REAR_SMALL:
		u4SpeakerSize &= (~(SPEAKER_LAYOUT_LS_LARGE | SPEAKER_LAYOUT_RS_LARGE));                                            
		u4TempSpeakerLayout = SPEAKER_LAYOUT_LRCLSRS;
		break;
	case SETTING_AUDIO_REAR_NONE:									
		u4TempSpeakerLayout = SPEAKER_LAYOUT_LRC;                 
		break;
	}

	u4SpeakerLayout = (u4SpeakerLayout & (~SPEAKER_LAYOUT_MASK)) | u4TempSpeakerLayout;

	u4ReturnValue = GSetSpeakerLayout(hAudioDev, u4SpeakerLayout, u4SpeakerSize);
	//u4ReturnValue = GSetDVDSpeakerLayout(u4SpeakerLayout, u4SpeakerSize);

	GCloseAudioDev(hAudioDev);
	return u4ReturnValue;
}


//重低音
static BOOL setSubwooferSpeak(UINT32 iSetValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue;

	UINT32 u4SpeakerLayout=0, u4SpeakerSize=0;
	GGetSpeakerLayout(hAudioDev, &u4SpeakerLayout, &u4SpeakerSize);

	//TODO
	if(iSetValue == SETTING_AUDIO_SUBWOOFER_ON)
		u4SpeakerLayout |= SPEAKER_LAYOUT_SUBWOOFER;
	else
		u4SpeakerLayout &= ~SPEAKER_LAYOUT_SUBWOOFER;

	u4ReturnValue = GSetSpeakerLayout(hAudioDev, u4SpeakerLayout, u4SpeakerSize);
	//u4ReturnValue = GSetDVDSpeakerLayout(u4SpeakerLayout, u4SpeakerSize);

	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}

#else //MTK提供的方法

//声道扩展
BOOL setUpmix(AUD_UPMIX_T eAudUpMix)
{

	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue;

	MISC_UPMIX_GAIN_T rUpmixGain;

	if (eAudUpMix == AUD_UPMIX_OFF)
	{
		rUpmixGain.u4Gain0 = 0x00000000;
		rUpmixGain.u4Gain1 = 0x00000000;
		rUpmixGain.u4Gain2 = 0x00000000;
		rUpmixGain.u4Gain3 = 0x00000000;
		rUpmixGain.u4Gain4 = 0x00000000;
		rUpmixGain.u4Gain5 = 0x00000000;
		rUpmixGain.u4Gain6 = 0x00000000;
		rUpmixGain.u4Gain7 = 0x00000000;
	}
	else
	{
		rUpmixGain.u4Gain0 = 0x666666;
		rUpmixGain.u4Gain1 = 0;//绿色，后左

		rUpmixGain.u4Gain2 = 0;
		rUpmixGain.u4Gain3 = 0x666666;//紫色，后右

		rUpmixGain.u4Gain4 = 0x333333;
		rUpmixGain.u4Gain5 = 0x333333;

		rUpmixGain.u4Gain6 = 0x666666;//白色，前左
		rUpmixGain.u4Gain7 = 0x666666;//综色，前右

	}

	if (eAudUpMix == AUD_UPMIX_ON)
	{
		//四声道，声音从前左，前右，后左，后右出来
		GSetSpeakerLayout(hAudioDev, (SPEAKER_LAYOUT_LRCLSRS), 0x1F);
		//GSetDVDSpeakerLayout( (SPEAKER_LAYOUT_LRCLSRS), 0x1F);
		GClientSetBalance(hAudioDev,_au4TrimValue[20],BALANCE_FRONT_LEFT);
		GClientSetBalance(hAudioDev,_au4TrimValue[20],BALANCE_FRONT_RIGHT);
		GClientSetBalance(hAudioDev,_au4TrimValue[20],BALANCE_REAR_LEFT);
		GClientSetBalance(hAudioDev,_au4TrimValue[20],BALANCE_REAR_RIGHT);
	}
	else
	{
		//二声道，声音从前左，前右出来
		GSetSpeakerLayout(hAudioDev, (SPEAKER_LAYOUT_STEREO), 0x1F); 
		//GSetDVDSpeakerLayout( (SPEAKER_LAYOUT_STEREO), 0x1F);
		GClientSetBalance(hAudioDev,_au4TrimValue[20],BALANCE_FRONT_LEFT);
		GClientSetBalance(hAudioDev,_au4TrimValue[20],BALANCE_FRONT_RIGHT);
	}

	u4ReturnValue = GClientSetUpMix(hAudioDev,(AUD_UPMIX_T) eAudUpMix,rUpmixGain);
	GCloseAudioDev(hAudioDev);

	return TRUE;

}



static BOOL setDownMix(UINT32 iSetValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue, u4TempSpeakerLayout;

	UINT32 u4SpeakerLayout=0, u4SpeakerSize=0;
	GGetSpeakerLayout(hAudioDev, &u4SpeakerLayout, &u4SpeakerSize);

	switch(iSetValue)
	{
	case SETTING_AUDIO_DOWNMIX_LTRT:
		u4TempSpeakerLayout = SPEAKER_LAYOUT_LR;
		break;
	case SETTING_AUDIO_DOWNMIX_STEREO:
		u4TempSpeakerLayout = SPEAKER_LAYOUT_STEREO;
		break;
	case SETTING_AUDIO_DOWNMIX_NONE:
		u4TempSpeakerLayout = SPEAKER_LAYOUT_LRC;
		break;
	}

	u4SpeakerLayout = (u4SpeakerLayout & (~SPEAKER_LAYOUT_MASK)) | u4TempSpeakerLayout;
	u4ReturnValue = GSetSpeakerLayout(hAudioDev, u4SpeakerLayout, u4SpeakerSize);
	//u4ReturnValue = GSetDVDSpeakerLayout(u4SpeakerLayout,u4SpeakerSize);
	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}

//前置喇叭
static BOOL setFrontSpeak(UINT32 iSetValue)
{
	UINT32 u4ReturnValue;
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4SpeakerLayout=0, u4SpeakerSize=0;

	GGetSpeakerLayout(hAudioDev, &u4SpeakerLayout, &u4SpeakerSize);

	//TODO
	if(iSetValue == SETTING_AUDIO_FRONT_LARGE)
		u4SpeakerSize |= (SPEAKER_LAYOUT_L_LARGE | SPEAKER_LAYOUT_R_LARGE);
	else
		u4SpeakerSize &= (~(SPEAKER_LAYOUT_L_LARGE | SPEAKER_LAYOUT_R_LARGE));

	u4ReturnValue = GSetSpeakerLayout(hAudioDev, u4SpeakerLayout, u4SpeakerSize);
	//u4ReturnValue = GSetDVDSpeakerLayout(u4SpeakerLayout, u4SpeakerSize);
	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}

//中置喇叭
static BOOL setCenterSpeak(UINT32 iSetValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue;
	UINT32 u4SpeakerLayout = 0,u4SpeakerSize =0;

	GGetSpeakerLayout(hAudioDev, &u4SpeakerLayout, &u4SpeakerSize);
	u4SpeakerLayout = (u4SpeakerLayout >> 8); 

	if (iSetValue != SETTING_AUDIO_CENTER_NONE)//CNETER OPEN
	{
		if (u4SpeakerLayout) // if subwoofer open
		{
			u4SpeakerLayout = ((u4SpeakerLayout << 8) | SPEAKER_LAYOUT_LRCLSRS);
			u4SpeakerSize &= (~SPEAKER_LAYOUT_C_LARGE);
			u4SpeakerSize &= (~SPEAKER_LAYOUT_L_LARGE);
			u4SpeakerSize &= (~SPEAKER_LAYOUT_R_LARGE);
			u4SpeakerSize &= (~SPEAKER_LAYOUT_LS_LARGE);
			u4SpeakerSize &= (~SPEAKER_LAYOUT_RS_LARGE);
			GSetSpeakerLayout(hAudioDev, u4SpeakerLayout, u4SpeakerSize); //open center and keep sub woofer(open status)
			//GSetDVDSpeakerLayout( u4SpeakerLayout, u4SpeakerSize);
		}
		else //subwoofer close
		{
			u4SpeakerSize |= (SPEAKER_LAYOUT_C_LARGE);
			u4SpeakerSize |= (SPEAKER_LAYOUT_L_LARGE);
			u4SpeakerSize |= (SPEAKER_LAYOUT_R_LARGE);
			u4SpeakerSize |= (SPEAKER_LAYOUT_LS_LARGE);
			u4SpeakerSize |= (SPEAKER_LAYOUT_RS_LARGE);
			GSetSpeakerLayout(hAudioDev, SPEAKER_LAYOUT_LRCLSRS, u4SpeakerSize); //open center and keep sub woofer(close status)
			//GSetDVDSpeakerLayout( SPEAKER_LAYOUT_LRCLSRS, u4SpeakerSize);
		}
	}
	else
	{
		if (u4SpeakerLayout) //subwoofer open
		{
			u4SpeakerLayout = ((u4SpeakerLayout << 8) | SPEAKER_LAYOUT_LRLSRS);
			u4SpeakerSize &= (~SPEAKER_LAYOUT_C_LARGE);
			u4SpeakerSize &= (~SPEAKER_LAYOUT_L_LARGE);
			u4SpeakerSize &= (~SPEAKER_LAYOUT_R_LARGE);
			u4SpeakerSize &= (~SPEAKER_LAYOUT_LS_LARGE);
			u4SpeakerSize &= (~SPEAKER_LAYOUT_RS_LARGE);
			GSetSpeakerLayout(hAudioDev, u4SpeakerLayout, u4SpeakerSize); //close center and keep subwoofer(open status)
			//GSetDVDSpeakerLayout( u4SpeakerLayout, u4SpeakerSize);
		}
		else //subwoofer close
		{
			u4SpeakerSize |= (SPEAKER_LAYOUT_C_LARGE);
			u4SpeakerSize |= (SPEAKER_LAYOUT_L_LARGE);
			u4SpeakerSize |= (SPEAKER_LAYOUT_R_LARGE);
			u4SpeakerSize |= (SPEAKER_LAYOUT_LS_LARGE);
			u4SpeakerSize |= (SPEAKER_LAYOUT_RS_LARGE);
			GSetSpeakerLayout(hAudioDev, SPEAKER_LAYOUT_LRLSRS, u4SpeakerSize); //close center and keep subwoofer(close status)
			//GSetDVDSpeakerLayout( SPEAKER_LAYOUT_LRLSRS, u4SpeakerSize);
		}
	}

	GCloseAudioDev(hAudioDev);
	return u4ReturnValue;
}

//环绕喇叭
static BOOL setRearSpeak(UINT32 iSetValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue, u4TempSpeakerLayout;
	UINT32 u4SpeakerLayout=0, u4SpeakerSize=0;

	GGetSpeakerLayout(hAudioDev, &u4SpeakerLayout, &u4SpeakerSize);


	//TODO
	switch(iSetValue)
	{
	case SETTING_AUDIO_REAR_LARGE:
		u4SpeakerSize |= (SPEAKER_LAYOUT_LS_LARGE | SPEAKER_LAYOUT_RS_LARGE);
		u4TempSpeakerLayout = SPEAKER_LAYOUT_LRCLSRS;                                                          
		//	u4TempSpeakerLayout = SPEAKER_LAYOUT_LRLSRS;   
		break;
	case SETTING_AUDIO_REAR_SMALL:
		u4SpeakerSize &= (~(SPEAKER_LAYOUT_LS_LARGE | SPEAKER_LAYOUT_RS_LARGE));
		u4TempSpeakerLayout = SPEAKER_LAYOUT_LRCLSRS;                                                    
		//	u4TempSpeakerLayout = SPEAKER_LAYOUT_LRLSRS; 
		break;
	case SETTING_AUDIO_REAR_NONE:
		u4TempSpeakerLayout = SPEAKER_LAYOUT_LRC;										
		//	u4TempSpeakerLayout = SPEAKER_LAYOUT_STEREO;                     //Then Force to Stereo
		break;
	}

	u4SpeakerLayout = (u4SpeakerLayout & (~SPEAKER_LAYOUT_MASK)) | u4TempSpeakerLayout;

	u4ReturnValue = GSetSpeakerLayout(hAudioDev, u4SpeakerLayout, u4SpeakerSize);
	//u4ReturnValue = GSetDVDSpeakerLayout(u4SpeakerLayout, u4SpeakerSize);

	GCloseAudioDev(hAudioDev);
	return u4ReturnValue;
}


//重低音
static BOOL setSubwooferSpeak(UINT32 iSetValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue;
	UINT32 u4SpeakerLayout = 0,u4SpeakerSize =0;

	GGetSpeakerLayout(hAudioDev, &u4SpeakerLayout, &u4SpeakerSize);

	//TODO
	if(iSetValue == SETTING_AUDIO_SUBWOOFER_ON)
	{
		u4SpeakerSize &= (~SPEAKER_LAYOUT_C_LARGE);
		u4SpeakerSize &= (~SPEAKER_LAYOUT_L_LARGE);
		u4SpeakerSize &= (~SPEAKER_LAYOUT_R_LARGE);
		u4SpeakerSize &= (~SPEAKER_LAYOUT_LS_LARGE);
		u4SpeakerSize &= (~SPEAKER_LAYOUT_RS_LARGE);

		GSetSpeakerLayout(hAudioDev, (SPEAKER_LAYOUT_SUBWOOFER | u4SpeakerLayout), u4SpeakerSize); //open subwoofer
		//GSetDVDSpeakerLayout((SPEAKER_LAYOUT_SUBWOOFER | u4SpeakerLayout), u4SpeakerSize);
	}
	else
	{
		u4SpeakerSize |= (SPEAKER_LAYOUT_C_LARGE);
		u4SpeakerSize |= (SPEAKER_LAYOUT_L_LARGE);
		u4SpeakerSize |= (SPEAKER_LAYOUT_R_LARGE);
		u4SpeakerSize |= (SPEAKER_LAYOUT_LS_LARGE);
		u4SpeakerSize |= (SPEAKER_LAYOUT_RS_LARGE);

		GSetSpeakerLayout(hAudioDev, ((~SPEAKER_LAYOUT_SUBWOOFER) & u4SpeakerLayout), u4SpeakerSize);
		//GSetDVDSpeakerLayout( ((~SPEAKER_LAYOUT_SUBWOOFER) & u4SpeakerLayout), u4SpeakerSize);
	}

	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}

#endif

static BOOL setAudioSpdifOut(UINT32 iType)
{	
	SendMsgToModules(DVD_MODULE, SET_DVDSPDIF, iType);
	return TRUE;
}

//喇叭设置
static BOOL setSpeakOutType(UINT32 iType)
{
	switch(iType)
	{
	case SPEAK_OUT_5_1_TYPE://5.1声道 【前后排4声道（低），中置重低音（高）】
		setUpmix(AUD_UPMIX_ON);							//开
		setDownMix(SETTING_AUDIO_DOWNMIX_NONE);			//关
		setFrontSpeak(SETTING_AUDIO_FRONT_SMALL);		//前置喇叭  小
		setCenterSpeak(SETTING_AUDIO_CENTER_SMALL);		//中置喇叭  小
		setRearSpeak(SETTING_AUDIO_REAR_SMALL);			//环绕喇叭  小 
		setSubwooferSpeak(SETTING_AUDIO_SUBWOOFER_ON);	//重低音   打开
		break;

	case SPEAK_OUT_4_0_TYPE://4.0声音 
		setUpmix(AUD_UPMIX_ON);							//开
		setDownMix(SETTING_AUDIO_DOWNMIX_NONE);			//关
		setFrontSpeak(SETTING_AUDIO_FRONT_LARGE);		//前置喇叭  大
		setCenterSpeak(SETTING_AUDIO_CENTER_NONE);		//中置喇叭  关
		setRearSpeak(SETTING_AUDIO_REAR_LARGE);			//环绕喇叭  大 
		setSubwooferSpeak(SETTING_AUDIO_SUBWOOFER_OFF);	//重低音   打开
		break;


	case SPEAK_OUT_STEREO_TYPE://立体声缩混
		setUpmix(AUD_UPMIX_ON);
		setDownMix(SETTING_AUDIO_DOWNMIX_STEREO);			//LT/RT混音
		setFrontSpeak(SETTING_AUDIO_FRONT_LARGE);			//前置喇叭  大
		setCenterSpeak(SETTING_AUDIO_CENTER_NONE);			//中置喇叭  关
		setRearSpeak(SETTING_AUDIO_REAR_NONE);				//环绕喇叭  关
		setSubwooferSpeak(SETTING_AUDIO_SUBWOOFER_ON);		//重低音    开
		break;

	default:
		return FALSE;
		break;
	}

	//设置DVD
	UINT32 u4SpeakerLayout=0, u4SpeakerSize=0;
	GHANDLE hAudioDev = GOpenAudioDev();
	GGetSpeakerLayout(hAudioDev, &u4SpeakerLayout, &u4SpeakerSize);
	GSetDVDSpeakerLayout(u4SpeakerLayout,u4SpeakerSize);
	GCloseAudioDev(hAudioDev);

	return TRUE;
}

//delay
BOOL setCenterDelay(UINT32 iSetValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue;

	//TODO
	u4ReturnValue = TRUE;
	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}

BOOL getCenterDelay(UINT32* getValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();

	//TODO
	*getValue = 0;

	GCloseAudioDev(hAudioDev);
	return TRUE;
}

BOOL setSubwooferDelay(UINT32 iSetValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue;

	//TODO
	u4ReturnValue = TRUE;
	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}

BOOL getSubwooferDelay(UINT32* getValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();

	//TODO
	*getValue = 0;

	GCloseAudioDev(hAudioDev);
	return TRUE;
}

BOOL setRearLeftDelay(UINT32 iSetValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue;

	//TODO
	u4ReturnValue = TRUE;
	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}

BOOL getRearLeftDelay(UINT32* getValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();

	//TODO
	*getValue = 0;

	GCloseAudioDev(hAudioDev);
	return TRUE;
}

BOOL setRearRightDelay(UINT32 iSetValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	UINT32 u4ReturnValue;

	//TODO
	u4ReturnValue = TRUE;
	GCloseAudioDev(hAudioDev);

	return u4ReturnValue;
}

BOOL getRearRightDelay(UINT32* getValue)
{
	GHANDLE hAudioDev = GOpenAudioDev();

	//TODO
	*getValue = 0;

	GCloseAudioDev(hAudioDev);
	return TRUE;
}

BOOL setTestTone(AUD_TEST_TONE_T eTestTone, MISC_TEST_TONE_TYPE_T eTestToneType)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	BOOL RetValue = GClientSetTestTone(hAudioDev,eTestTone,eTestToneType);
	GCloseAudioDev(hAudioDev);
	return RetValue;
}

BOOL getTestTone(AUD_TEST_TONE_T *peTestTone,MISC_TEST_TONE_TYPE_T *peTestToneType)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	BOOL RetValue = GClientGetTestTone(hAudioDev, peTestTone, peTestToneType);
	GCloseAudioDev(hAudioDev);
	return RetValue;
}

BOOL setAudFeature(AUD_DEC_FEATURE_INFO_T eAudFeatur)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	BOOL RetValue = GSetAudFeature(hAudioDev, eAudFeatur);
	GCloseAudioDev(hAudioDev);
	return RetValue;
}

BOOL setMVS(MISC_MVS_T eMVSType, MISC_MVS_GAIN_T rMVSGain)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	BOOL RetValue = GClientSetMVS(hAudioDev, eMVSType, rMVSGain);
	GCloseAudioDev(hAudioDev);
	return RetValue;
}
BOOL getMVS(MISC_MVS_T *peMVSType, MISC_MVS_GAIN_T *prMVSGain)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	BOOL RetValue = GClientGetMVS(hAudioDev,  peMVSType, prMVSGain);
	GCloseAudioDev(hAudioDev);
	return RetValue;
}

BOOL setDRC(BOOL bDRCOn)
{
	 GSetDRC(bDRCOn);
	 return TRUE;
}

BOOL setSpkAudioType(BOOL bSubOn)
{
	//接下来设置5.1声道
	//setAudFeature(AUD_DEC_PROLOGICII);
	//setAudioDefaultFeatures();

	setAudioSpdifOut(SETTING_AUDIO_SPDIF_PCM);

	if (bSubOn){
		setSpeakOutType(SPEAK_OUT_5_1_TYPE);////5.1  有重低音
	}else{
		setSpeakOutType(SPEAK_OUT_4_0_TYPE);
	}
	return TRUE;
}

void setBackVideoSigned(BOOL bSined)
{
	gAudioApiInfo.bBackVideoSigned = bSined;
}

//判断先前在哪个通道，对应的停掉此通道的声音
BOOL DeletePreAudioSource(BYTE iChannel)
{
	if (gAudioApiInfo.iPreAudioSource == InitAV)
	{
		if (iChannel != MediaCD)
		{
			SendMsgToModules(MP3_MODULE,SET_AUDIO,MP3_EXIT);
		}
	}

	if (gAudioApiInfo.iPreAudioSource == MediaCD)//如果以前是DVD
	{
		if (iChannel == BT)
			SendMsgToModules(DVD_MODULE,SET_AUDIO,DVD_BT_OFF);//BT来电/通话时手动再次切入DVD
		else
			SendMsgToModules(DVD_MODULE,SET_AUDIO,DVD_AUDIO_OFF);
		
		Sleep(200);
		SendMsgToModules(MP3_MODULE,SET_AUDIO,MP3_EXIT);
	}
	else if (gAudioApiInfo.iPreAudioSource == A2DP)
	{
		if (iChannel != BT)
		{
			gAudioApiInfo.bA2DPMustFalseFlag = FALSE;
			SendMsgToModules(BT_MODULE,SET_AUDIO,BT_A2DP_OFF);
			Sleep(200);//要先等A2DP得FALSE掉
		}
	}
	else if (gAudioApiInfo.iPreAudioSource == BT)
	{
		if (gAudioApiInfo.bA2DPMustFalseFlag && iChannel != A2DP)//以前在A2DP通道
		{
			gAudioApiInfo.bA2DPMustFalseFlag = FALSE;
			SendMsgToModules(BT_MODULE,SET_AUDIO,BT_A2DP_OFF);
			Sleep(200);//要先等A2DP得FALSE掉
		}
	}
	else//外部声道
	{
		SendMsgToModules(MP3_MODULE,SET_AUDIO,MP3_OFF);
	}

	return TRUE;
}

void setDVDInitStatus(BOOL bInitStauts)
{
	gAudioApiInfo.bDvdInitStatus = bInitStauts;
}

//通话中进碟
static void CheckDVDAtBTChannel(BYTE iChannel)
{
	BOOL bCurDvdAtBtChannel = FALSE;

	if (gAudioApiInfo.iPreAudioSource == MediaCD)//在DVD通道，蓝牙来电时，不做处理
		return;

	//蓝牙来电，DVD要做些特殊处理
	if (iChannel == BT)
		bCurDvdAtBtChannel = TRUE;
	else
		bCurDvdAtBtChannel = FALSE;

	if (bCurDvdAtBtChannel != gAudioApiInfo.bPreDvdAtBtChannel)
	{
		if (bCurDvdAtBtChannel)
		{
			SendMsgToModules(DVD_MODULE,SET_AUDIO,DVD_AT_BT_CHANNEL);
		}
		else
		{
			SendMsgToModules(DVD_MODULE,SET_AUDIO,DVD_NOT_BT_CHANNEL);
		}
		gAudioApiInfo.bPreDvdAtBtChannel = bCurDvdAtBtChannel;
	}
}

//切音频声道
BOOL ExAudioSourceSelect(UINT32 setValue,BYTE iChannel)
{
	SendMsgToModules(MP3_MODULE,SET_AUDIO,setValue);
	return TRUE;
}

static void InitAudioInfo(void)
{
	gAudioApiInfo.u4SpeakerLayout = SPEAKER_LAYOUT_STEREO;
	gAudioApiInfo.u4SpeakerSize  = 0;
	gAudioApiInfo.iPreAudioSource = InitAV;

	gAudioApiInfo.bCurMute = TRUE;

	gAudioApiInfo.iPreBTVolume = 0;
	gAudioApiInfo.iPreForntVolume = 0;
	gAudioApiInfo.iPreBackVolume = 0;
	gAudioApiInfo.iPreGPSVolume = 0;
	gAudioApiInfo.iPreMP3Volume = 0;
	gAudioApiInfo.iPreDVPVolume = 0;
	gAudioApiInfo.iPreIopdVolume = 0;

	//GPS进程ID
	gAudioApiInfo.iPreGPSProcessID = 0;
	gAudioApiInfo.bPreDvdAtBtChannel = FALSE;
	gAudioApiInfo.bA2DPMustFalseFlag = FALSE;

	gAudioApiInfo.bBackVideoSigned = FALSE;
	gAudioApiInfo.bDvdInitStatus = FALSE;

	//waveOutMessage((HWAVEOUT)0, MM_SET_GPS_CH_CHANGE, TRUE,GPS_BT_RING_SPKOUT_FROM_FRONT_L);		//bt语音输出到L+R上
	
}

//选择外部解码方式 --- 前后排
static void SetAudioSourceFrontDacModule(void)
{
	INT32 mode = X_AUD_DAC_EXT;
	HANDLE t_hAudDrv = NULL;
	X_AUD_DAC_TYPE_SEL_T rOpCmd;

	t_hAudDrv = CreateFile((L"ADE1:"), GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL); 
	if (t_hAudDrv == INVALID_HANDLE_VALUE || t_hAudDrv == NULL) 
	{ 
		RETAILMSG(1,(TEXT("[Audio dll] SetAudioSourceFrontDacModule Open Audio driver fail!\n")));  
	}
	else
	{
		if(mode == X_AUD_DAC_PWM)
		{
			rOpCmd.eOut = X_AUD_FRONT_REAR;
			rOpCmd.eDacType = X_AUD_DAC_PWM;
		}
		else
		{
			rOpCmd.eOut = X_AUD_FRONT_REAR;
			rOpCmd.eDacType = X_AUD_DAC_EXT;
		}
		DeviceIoControl(t_hAudDrv, CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800+0x2F, METHOD_BUFFERED, FILE_ANY_ACCESS), (void *)&rOpCmd, sizeof(X_AUD_DAC_TYPE_SEL_T), NULL, 0, NULL, NULL);
	}
	CloseHandle(t_hAudDrv);
}

void setProcessID(BYTE type, UINT32 iProcessID)
{
	if (type == CMD_GPS_PROCESS_ID)//GPS
	{
		SetGPSVolumeProcessID(iProcessID);
		RETAILMSG(1,(TEXT("\r\nGPS process id is :0x%X"),iProcessID));	
	}
}

//设定各音量的可调范围
static UINT32  CustomSetAudio(void)
{
	GHANDLE hAudioDev = GOpenAudioDev();
	MISC_AUD_FUNC_OPTION_T rFuncOptionSet;
	AUD_DEC_DIV_INFO_T rAudDivInfo;

	rFuncOptionSet.u4FuncOption0 = 0x04;		//default: 0
	rFuncOptionSet.u4FuncOption1 = 0;			//default: 0
	rFuncOptionSet.u4FuncOption2 = 0;			//default: 0
	rFuncOptionSet.u4BassCutOffFreq = 100;		//default: 100 (Hz)

#if DEBUG_AUDIO_OLD
	rFuncOptionSet.u4GainAvIn = 0x20000;	//default: 0x20000
	rFuncOptionSet.u4GainUSB  = 0x20000;	//default: 0x20000
	rFuncOptionSet.u4GainDVD  = 0x20000;	//default: 0x20000	//最大值 为 0X20000
#else 
	rFuncOptionSet.u4GainAvIn = 0x20000;	//default: 0x20000
	rFuncOptionSet.u4GainUSB  = 0x20000;	//default: 0x20000
	rFuncOptionSet.u4GainDVD  = 0x20000;	//default: 0x20000	//最大值 为 0X20000
#endif

	rFuncOptionSet.u4Reserve1 = 0;			//default: 0
	rFuncOptionSet.u4Reserve2 = 0;			//default: 0
	rFuncOptionSet.u4Reserve3 = 0;			//default: 0
	rFuncOptionSet.u4Reserve4 = 0;			//default: 0
	rFuncOptionSet.u4Reserve5 = 0;			//default: 0
	rFuncOptionSet.u4Reserve6 = 0;			//default: 0	

	GAudioFuncOptionSet(hAudioDev,rFuncOptionSet);

	//set diversity info
	rAudDivInfo.e_type = AUD_DEC_DIV_TYPE_LOGO_DOLBY;
	rAudDivInfo.u1_setting = DOLBY_DECODE_MULTI_CH_SETTING;
	GAudioSetDiversityInfo(hAudioDev, &rAudDivInfo);

	GCloseAudioDev(hAudioDev);

	return TRUE;
}

BOOL AudioSourceInit(void)
{
	InitAudioInfo();
	CustomSetAudio();
	
#if DEBUG_AUDIO_OLD
#else
	SetAudioSourceFrontDacModule();
#endif

	setAudioThreshold(DB_80L);
	return TRUE;
}

BOOL AudioSourceRelease(void)
{
	return TRUE;
}

 //切音频声道
 BOOL setAudioSourceSelect(UINT32 setValue,BYTE iChannel)
 {
	 if (gAudioApiInfo.iPreAudioSource == iChannel)
		 return FALSE;

	 DeletePreAudioSource(iChannel);
	 CheckDVDAtBTChannel(iChannel);

	 if (setValue == S_NEXTER_SOURCE)
	 {
		if (iChannel == MediaCD)
		{
			SendMsgToModules(MP3_MODULE,SET_AUDIO,MP3_ENTER);
			Sleep(200);

			if (gAudioApiInfo.iPreAudioSource == BT)
				SendMsgToModules(DVD_MODULE,SET_AUDIO,DVD_BT_ON);
			else
				SendMsgToModules(DVD_MODULE,SET_AUDIO,DVD_AUDIO_ON);
		}
		else if (iChannel == A2DP)
		{
			if (gAudioApiInfo.iPreAudioSource != BT)
			{
				gAudioApiInfo.bA2DPMustFalseFlag = TRUE;
				SendMsgToModules(BT_MODULE,SET_AUDIO,BT_A2DP_ON);
			}
		}
	 }
	 else if (setValue == S_DEFINE_SOURCE)
	 {
		DeletePreAudioSource(iChannel);
	 }
	 else
	 {
		 if (iChannel == VAP)
		 {
			if (gAudioApiInfo.bBackVideoSigned)
				setValue = setValue <<4 | 0x01; 
			else
				setValue = setValue <<4 | 0x00; 
		 }

		ExAudioSourceSelect(setValue,iChannel);
	 }

	 gAudioApiInfo.iPreAudioSource = iChannel;

	 return TRUE;
 }