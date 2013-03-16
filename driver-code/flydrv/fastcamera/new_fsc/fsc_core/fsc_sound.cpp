/******************************
*
*  INCLUDE FILES
*
*****************************/
#include <windows.h>
#include "fsc_sound.h"
#include <mmsystem.h>
#include "wavefile.h"

/******************************
*
*   MACRO DECLARE
*
*****************************/
#define FSC_SOUND_LOG 1

#undef VERIFY

#define VERIFY(x)		do { \
	if (!(x))	{\
	RETAILMSG(1, (TEXT("[FAST CAM SOUND]DBGCHK Failed: %s, %d!\r\n"), TEXT(__FILE__), (INT32)__LINE__)); \
	ASSERT(0);}\
	} while (0)


/******************************
*
*   GLOBAL VARABLE DECLARE
*
*****************************/
WaveFile gwavefile;


	
/******************************
*
*	Sound Play Interfaces
*
*****************************/
//reserved interface
BOOL SoundInit(VOID)
{
	RETAILMSG(FSC_SOUND_LOG, (TEXT("Enter SoundInit \r\n")));
	RETAILMSG(FSC_SOUND_LOG, (TEXT("Exit SoundInit \r\n")));

	return (TRUE);
}

//before play sound,use soundstart interface to create fsc.wav file from memory
BOOL SoundStart(void* psndmem,UINT32 u4sndsz)
{
	HANDLE hFile;
	DWORD u4Written = 0;
	
	hFile = CreateFile(TEXT("\\fsc.wav"),GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
        RETAILMSG(FSC_SOUND_LOG,(TEXT("Create fsc.wav File fail!!\r\n")));
		return FALSE;
	}
	
	if(!WriteFile(hFile,psndmem,u4sndsz,&u4Written,NULL))
	{
		RETAILMSG(FSC_SOUND_LOG,(TEXT("Write fsc.wav fail!!\r\n")));
		CloseHandle(hFile);
		return FALSE;
	}
	
	CloseHandle(hFile);

	return TRUE;
}

//play sound in aysnc mode,user can stop playing anytime with using SoundStop
BOOL SoundPlay(VOID)
{
	BOOL bRet;
#if 0	
	bRet = sndPlaySound(TEXT("\\fsc.wav"),SND_FILENAME | SND_ASYNC);
#else
	gwavefile.StopBackCarSound();
	gwavefile.StartBackCarSound();
#endif
	return bRet;
}

BOOL SoundStop(VOID)
{
	BOOL bRet;
#if 0	
	bRet = sndPlaySound(NULL,SND_ASYNC);
#else
	gwavefile.StopBackCarSound();
#endif
	return bRet;

}

VOID SoundDeinit(VOID)
{
	RETAILMSG(FSC_SOUND_LOG,(TEXT("[FAST_CAMERA-SOUND] SoundDeinit \r\n")));

	SoundStop();
}



