#ifndef     __BCLIB__H___
#define    __BCLIB__H___

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _tagTransferData
{
   DWORD dwBackCarSta;
   DWORD dwDis;
}TransferData;



/**/
BOOL BCAllocateResource();
BOOL BCReleaseResource();
DWORD  BCWaitInitFin(DWORD *dwDis);


/*for Uart Lib*/
BOOL BCUartInit(UINT32 u4Port,UINT32 u4BaudRate);
BOOL BCUartReadBlockData(BYTE *pData,UINT32 u4ByteToRead);
BOOL BCUartWriteBlockData(BYTE *pData,UINT32 u4ByteToWrite);


/*for GUI Lib*/
#define RGB565(r,g,b)   ((UINT16)(r<<11 | g<<5 | b))
BOOL BCUIInit();
BOOL BCUIDrawPoint(UINT32 x,UINT32 y);
BOOL BCUIDrawLine(POINT Start,POINT End);
BOOL BCUIBitBlt(int nXDest, int nYDest, int nWidth,  int nHeight,UINT16 *pSrcData, int nXSrc, int nYSrc,UINT16 BgColor);
BOOL BCUISetBkColor(UINT16 bkColor);
BOOL BCUISetPenColor(UINT16 PenColor);
BOOL BCUIInvalidateRegion(INT32 x,INT32 y,UINT32 nWidth,UINT32 nHeight);
BOOL  BCPlaySound(void* psndmem,UINT32 u4sndsz);
BOOL   ShowBackCarUIOverlay();
#ifdef __cplusplus
}
#endif

#endif
