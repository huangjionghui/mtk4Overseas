#include"platform.h"



UINT16 *g_pFrameBuf = NULL;

UINT16 g_bkColor = 0;
UINT16 g_PenColor = 0xFFFF;




BOOL FSTC_GUIInit(UINT16 *pFrameBuf)
{
	
	

	POINT Start = {0,0};
    POINT End = {DIS_WIDTH,DIS_HEIGHT};


	g_pFrameBuf = pFrameBuf;

	return TRUE;
}


BOOL  BC_DrawPoint(UINT32 x,UINT32 y)
{

	if(x>DIS_WIDTH || y>DIS_HEIGHT || x < 0 || y<0)
		return FALSE;

	g_pFrameBuf[y*DIS_WIDTH + x] = g_PenColor;

	return TRUE;

}


static int BC_Sing(float d)
{
    int ret;
	if(d > 0)
		ret = 1;
	else if(d < 0)
		ret = -1;
	else
		ret = 0;

	return ret;
}

BOOL   BC_DrawLine(POINT Start,POINT End)
{
   INT32  u4Length;
   float  DeltaX,DeltaY,x,y;
   INT32 i;

   if(abs(Start.x)>DIS_WIDTH || abs(Start.y)>DIS_HEIGHT || abs(End.x)>DIS_WIDTH || abs(End.y>DIS_HEIGHT))
   	return FALSE;

   u4Length = abs(Start.x - End.x);

   if(abs(Start.y-End.y) > u4Length)
	   u4Length = abs(Start.y-End.y);

   DeltaX = ((float)( End.x  - Start.x))/u4Length;
   DeltaY =  ((float)(End.y  - Start.y))/u4Length;


   x = (float)Start.x + 0.5*BC_Sing(DeltaX);
   y = (float)Start.y + 0.5*BC_Sing(DeltaY);

  for(i = 0; i< u4Length ; i++)
  {
	  BC_DrawPoint((UINT32)x +1,(UINT32)y);
	  BC_DrawPoint((UINT32)x -1,(UINT32)y);
	  BC_DrawPoint((UINT32)x,(UINT32)y +1);
	  BC_DrawPoint((UINT32)x,(UINT32)y -1);
	 x = x+ DeltaX;
	 y = y +DeltaY;
  }

 return TRUE;

}
BOOL BC_BitBlt(int nXDest, int nYDest, int nWidth,  int nHeight, UINT16 *pSrcData, int nXSrc, int nYSrc,UINT16 BgColor)
{
   int i,j, cnt;
   cnt = 0;
   for(i = 0;i<nHeight;i++)
   {
	   for(j = 0;j<nWidth;j++,cnt++)
	   {


		   if(pSrcData[cnt] == BgColor)
			   continue;
		   g_pFrameBuf[(i+nYDest)*DIS_WIDTH + j+nXDest] =pSrcData[cnt];
		
	   }

   }
   return TRUE;

}
BOOL  BC_SetBkColor(UINT16 bkColor)
{
	UINT32 i;
	g_bkColor = bkColor;
	for(i = 0; i<DIS_WIDTH*DIS_HEIGHT-1;i++)
	{
		//g_pFrameBuf[i] = 0x4565;
		g_pFrameBuf[i] = bkColor;
	}


    return TRUE;
}
BOOL BC_SetPenColor(UINT16 PenColor)
{

  g_PenColor = PenColor;
  return TRUE;
}

BOOL BC_InvalidateRegion(INT32 x,INT32 y,UINT32 nWidth,UINT32 nHeight)
{
   UINT32 i,j, cnt;
   cnt = 0;
   for(i = 0;i<nHeight;i++)
   {
	   for(j = 0;j<nWidth;j++,cnt++)
	   {

		   g_pFrameBuf[(i+y)*DIS_WIDTH + j+x] =g_bkColor;
		
	   }

   }
   return TRUE;

}


