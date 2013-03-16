#ifndef _MEDIAPLAY_H
#define _MEDIAPLAY_H
//message id

#define DLL 

DLL int MediaPlayerInit(void);
DLL void MediaPlayerExit(void);
// int Media_Play(tchar_t *pFileName,filepos_t FileOffset,filepos_t FileSize, char Mode, int offset ,int length,void (*endfunc)(void),char XorValue,void * pRect, bool_t IsAudio,void *hWnd)
// 多媒体播放函数,可以播放声音,视频文件
// Input: 
// pFileName:	文件名
// FileOffset:	有效数据的起始位置 
// FileSize:	文件的大小(包含 FILEOFFSET,可以设置值来控制播放结束) 0:表示实际大小
// Mode:		定位模式 0 用时间定位(msec) 1 文件位置定位
// Offset:		起始位置(时间或位置)
// Length:		播放长度(时间或位置)
// endfunc:		回调函数,由调用者带入, 当媒体播放完时将调用该函数
// XorValue:	解密用的关键值
// pRect:		视频播放时的位置(x,y,width,height)
// IsAudio:		视频音频标记
// hWnd:		视频显示窗口的句柄
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