#include<windows.h>
#include"BCLib.h"
#include"mrf.h"
static HANDLE  g_hMrf = NULL;

BOOL  CustomUIInit()
{

    BOOL bRet = FALSE;
	BITMAPOBJINFO BitmapInfo;
	
	if(NULL == g_hMrf)
	  g_hMrf = LoadMRF();
	if(NULL == g_hMrf)
		return FALSE;
	
	bRet = GetBitmapInfo(g_hMrf,14,&BitmapInfo);
	if(bRet)
	 BCUIBitBlt(0,0,BitmapInfo.u4Width,BitmapInfo.u4Height,(UINT16 *)GetRCObjectMemAddr(g_hMrf,(RCOBJECT *)&BitmapInfo),0,0,RGB565(0,0,0));

    ShowBackCarUIOverlay();

  
  return TRUE;
}

BOOL PlayToneSound()
{
    BOOL bRet = FALSE;
	WAVOBJINFO WavInfo;
    bRet = GetWavResnfo(g_hMrf,19,&WavInfo);

   if(bRet)
	   BCPlaySound(GetRCObjectMemAddr(g_hMrf,(RCOBJECT *)&WavInfo),WavInfo.u4SizeImage);
   
   return TRUE;
}
BOOL ShowDistance(UINT32 u4Dis)
{
    
    BOOL bRet = FALSE;
	BITMAPOBJINFO BitmapInfo;
	if(0 == u4Dis)
	{
	  return ShowStopCarMsg();
 
	}

	if(1 == u4Dis)
	{
		 PlayToneSound();
	}
	bRet = GetBitmapInfo(g_hMrf,u4Dis,&BitmapInfo);

	BCUIInvalidateRegion(0,430,240,50);
	if(bRet)
	 BCUIBitBlt(90,430,BitmapInfo.u4Width,BitmapInfo.u4Height,(UINT16 *)GetRCObjectMemAddr(g_hMrf,(RCOBJECT *)&BitmapInfo),0,0,RGB565(0,0,0));



	bRet = GetBitmapInfo(g_hMrf,18,&BitmapInfo);
	if(bRet)
	 BCUIBitBlt(120,430,BitmapInfo.u4Width,BitmapInfo.u4Height,(UINT16 *)GetRCObjectMemAddr(g_hMrf,(RCOBJECT *)&BitmapInfo),0,0,RGB565(0,0,0));

    return TRUE;

}

BOOL  ShowStopCarMsg()
{

    BOOL bRet = FALSE;
	BITMAPOBJINFO BitmapInfo;


	BCUIInvalidateRegion(0,430,240,50);

	bRet = GetBitmapInfo(g_hMrf,11,&BitmapInfo);
	if(bRet)
	BCUIBitBlt(40,430,BitmapInfo.u4Width,BitmapInfo.u4Height,(UINT16 *)GetRCObjectMemAddr(g_hMrf,(RCOBJECT *)&BitmapInfo),0,0,RGB565(0,0,0));
  

	bRet = GetBitmapInfo(g_hMrf,13,&BitmapInfo);
	if(bRet)
	BCUIBitBlt(90,430,BitmapInfo.u4Width,BitmapInfo.u4Height,(UINT16 *)GetRCObjectMemAddr(g_hMrf,(RCOBJECT *)&BitmapInfo),0,0,RGB565(0,0,0));

	bRet = GetBitmapInfo(g_hMrf,12,&BitmapInfo);
	if(bRet)
	BCUIBitBlt(140,430,BitmapInfo.u4Width,BitmapInfo.u4Height,(UINT16 *)GetRCObjectMemAddr(g_hMrf,(RCOBJECT *)&BitmapInfo),0,0,RGB565(0,0,0));

	bRet = GetBitmapInfo(g_hMrf,16,&BitmapInfo);
	if(bRet)
	BCUIBitBlt(190,430,BitmapInfo.u4Width,BitmapInfo.u4Height,(UINT16 *)GetRCObjectMemAddr(g_hMrf,(RCOBJECT *)&BitmapInfo),0,0,RGB565(0,0,0));


  return  TRUE;
}

