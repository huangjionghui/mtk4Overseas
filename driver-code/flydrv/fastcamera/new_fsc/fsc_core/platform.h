#ifndef _BC_PLATFORM_H_
#define _BC_PLATFORM_H_
#include<windows.h>




//for display

#define RGB_565(r,g,b)   ((UINT16)(r<<11 | g<<5 | b))
#define DIS_WIDTH     800
#define DIS_HEIGHT    480
BOOL FSTC_GUIInit(UINT16 *pFrameBuf);

BOOL DisplayPathInit(VOID *pFramBuffer,UINT32 u4Width,UINT32 u4Heigth);
BOOL DisplayInit();
BOOL  BC_DrawPoint(UINT32 x,UINT32 y);
BOOL   BC_DrawLine(POINT Start,POINT End);
BOOL BC_BitBlt(int nXDest, int nYDest, int nWidth,  int nHeight,
	UINT16 *pSrcData, int nXSrc, int nYSrc,UINT16 BgColor);

BOOL  BC_SetBkColor(UINT16 bkColor);
BOOL BC_SetPenColor(UINT16 PenColor);

BOOL BC_InvalidateRegion(INT32 x,INT32 y,UINT32 nWidth,UINT32 nHeight);

DWORD WINAPI BackCarThread(VOID *pPddContext );
DWORD WINAPI BackCarListenThread(VOID *pPddContext );

#endif




























