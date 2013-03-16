#ifndef _MEDIAPLAY_H
#define _MEDIAPLAY_H
//message id

#define DLL 

DLL int MediaPlayerInit(void);
DLL void MediaPlayerExit(void);
// int Media_Play(tchar_t *pFileName,filepos_t FileOffset,filepos_t FileSize, char Mode, int offset ,int length,void (*endfunc)(void),char XorValue,void * pRect, bool_t IsAudio,void *hWnd)
// ��ý�岥�ź���,���Բ�������,��Ƶ�ļ�
// Input: 
// pFileName:	�ļ���
// FileOffset:	��Ч���ݵ���ʼλ�� 
// FileSize:	�ļ��Ĵ�С(���� FILEOFFSET,��������ֵ�����Ʋ��Ž���) 0:��ʾʵ�ʴ�С
// Mode:		��λģʽ 0 ��ʱ�䶨λ(msec) 1 �ļ�λ�ö�λ
// Offset:		��ʼλ��(ʱ���λ��)
// Length:		���ų���(ʱ���λ��)
// endfunc:		�ص�����,�ɵ����ߴ���, ��ý�岥����ʱ�����øú���
// XorValue:	�����õĹؼ�ֵ
// pRect:		��Ƶ����ʱ��λ��(x,y,width,height)
// IsAudio:		��Ƶ��Ƶ���
// hWnd:		��Ƶ��ʾ���ڵľ��
// Return:		ERR_NONE success others faild;
//
//DLL int Media_Play(tchar_t *pFileName,filepos_t FileOffset,filepos_t FileSize, char Mode, int offset ,int length,void (*endfunc)(void),char XorValue,void * pRect, bool_t IsAudio,void *hWnd);
DLL void Media_Resize(void *pRect);
DLL void Media_Paint(void);
DLL int Media_Pause(void);
DLL int Media_Resume(void);
DLL int Media_Stop(void);
DLL int Media_GetCurrentTime(void);
DLL int Media_GetDuration(void);
//DLL int Media_SetBackround(bool_t bBackround);
DLL int Media_GetVolume(void);
DLL void Media_SetVolume(int Volume);
//DLL tchar_t Media_Title();

//DLL int MP3_Play(tchar_t *pFileName,filepos_t FileOffset,filepos_t FileSize, char Mode, int offset ,int length,void (*endfunc)(void),char XorValue);


#endif