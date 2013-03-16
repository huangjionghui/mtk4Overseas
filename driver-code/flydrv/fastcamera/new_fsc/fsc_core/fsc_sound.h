#ifndef __FSC_SOUND_H__

#define __FSC_SOUND_H__


BOOL SoundInit(VOID);
BOOL SoundStart(void* psndmem,UINT32 u4sndsz);
BOOL SoundPlay(VOID);
BOOL SoundStop(VOID);
VOID SoundDeinit(VOID);

#endif

