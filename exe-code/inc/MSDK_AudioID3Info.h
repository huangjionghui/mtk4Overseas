#ifndef _MSDK_AUDIOID3_H_
#define _MSDK_AUDIOID3_H_

#include <MSDK_Defs.h>

#ifdef __cplusplus
extern "C" {
#endif


BOOL  ID3Info_Init(VOID);

BOOL  ID3Info_Deinit(VOID);

BOOL GetAudioID3Info(TCHAR *pszFileName, MSDK_AUDIO_ID3INFO_T * pAudioID3Info);
BOOL GetAudioID3APicInfo(TCHAR *pszFileName, MSDK_ID3INFO_APIC_T *pID3ApicInfo);

HBITMAP GetID3ApicBitmap(TCHAR *pszFileName, E_MSDK_PIXELFORMAT ePixelFormat, UINT width, UINT height);

VOID  FreeAudioID3APicInfo(MSDK_ID3INFO_APIC_T *pAudioID3APicInfo);


#ifdef __cplusplus
}
#endif

#endif //_MSDK_LYRICS_H_


