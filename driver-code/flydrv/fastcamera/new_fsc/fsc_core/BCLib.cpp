#include "stdafx.h"
#include<windows.h>
#include "SerialPort.h"

#include"platform.h"
#include "fsc_surface.h"
#include "backcar_msg.h"
#include "fsc_tvd.h"
#include "mrf.h"
#include "BCLib.h"
#include "fsc_sound.h"
#include "backcar_msg.h"


#define  BACKCAR_STATUS_RUNNING     1
#define  BACKCAR_STATUS_STOP        2


#define  BACKCAR_EVENT_NAME (L"SYSTEM/ARM2BackCarFinished")

static HANDLE  g_VideoRenderEvt = NULL;
static HWND hWnd = NULL;
static CSerialPort *g_pRevPort = NULL;
static HANDLE g_TVDResourceEnvt = NULL;
static BOOL   g_BackCarStatus =  BACKCAR_STATUS_STOP;
static BOOL   g_fARM2SwitchBackCarToARM1 = FALSE;
static UINT	  g_uTimerId = 0; 

#define FILE_DEVICE_FSC 					32768
#define FUNCTION_NOTIFY_APP_READY			2048
#define FUNCTION_NOTIFY_ARM2_STOP			2050
#define FUNCTION_NOTIFY_ARM2_BUFF_MEMSET	2051

#define IOCTL_FSC_NOTIFY_APP_READY          CTL_CODE(FILE_DEVICE_FSC, FUNCTION_NOTIFY_APP_READY, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FSC_NOTIFY_ARM2_STOP          CTL_CODE(FILE_DEVICE_FSC, FUNCTION_NOTIFY_ARM2_STOP, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FSC_NOTIFY_ARM2_BUFF_MEMSET   CTL_CODE(FILE_DEVICE_FSC, FUNCTION_NOTIFY_ARM2_BUFF_MEMSET, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define DISPLAY_IOCTL_HIDE_BACK_VIDEO       (0x00020002)

#define EVENT_BACKCAR_LISTEN_THREAD		TEXT("FLY_EVENT_NAME_BACKCAR_LISTEN_THREAD")

static HANDLE g_hEventBackcarListen;
extern "C" DWORD CameraStateCfg;
extern "C" DWORD GpioRead();
extern "C" HANDLE g_hEventBackcarEnabel; 
extern "C" void SendBackStatusToDriver(UINT32 iData);


#define BACKCAR_START_MASK          (1 << 0)
#define AV_START_MASK               (1 << 1)

HANDLE g_hBackcarAVFileMap = NULL;
DWORD *g_pfgBackcarAVStart = NULL;
HANDLE g_hBackcarAvEvent = NULL;
#define BACKCAR_AV_EVENT        _T("Backcar_AV_SYN")



BOOL NotifyVideoRenderReleaseVDPFin()
{
   SetEvent(g_VideoRenderEvt);
   return TRUE;
}


extern "C"
BOOL BCUartInit(UINT32 u4Port,UINT32 u4BaudRate)
{

   g_pRevPort = new CSerialPort;
   if(NULL == g_pRevPort)
   	return FALSE;
   
    return  g_pRevPort->InitPort(u4Port,u4BaudRate);

           
}

extern "C"
BOOL BCUartReadBlockData(BYTE *pData,UINT32 u4ByteToRead)
{
  UINT32 u4Read;
  u4Read =  g_pRevPort->ReadCommBlockData(pData,u4ByteToRead);

  if(u4Read != u4ByteToRead)
	  return FALSE;

   return TRUE;
}

extern "C"
BOOL BCUartWriteBlockData(BYTE *pData,UINT32 u4ByteToWrite)
{

  if(NULL == g_pRevPort)
  	return FALSE;
  
  g_pRevPort->WriteCommBlockData(pData,u4ByteToWrite);
  return TRUE;
}

BOOL   CreateBackCarUISurface(HWND hWnd);

void CALLBACK TimerProc(
						HWND hwnd, 
						UINT uMsg, 
						UINT idEvent, 
						DWORD dwTime 
						)
{
	BOOL bSignal = 0;

	if (g_uTimerId != 0)
	{
		::KillTimer(hwnd, g_uTimerId);
		g_uTimerId = 0;
	}
	

	if (GpioRead() == 0)
	{
		SendBackStatusToDriver(0);//没倒车
		RETAILMSG(1 , (TEXT("\r\n[Back-BCLib.cpp]:Back Wince Action:0,Video:0\r\n")));
	}
	else
	{
		bSignal = TvdSignalIsOn();
		RETAILMSG(TRUE,(TEXT("\r\n*************************Back Signal =%d  Init TVD Success--\r\n"),bSignal));

		if (bSignal)//有倒车有视频
		{
			//SendBackStatusToDriver(3);
			//RETAILMSG(1 , (TEXT("\r\n[Back-BCLib.cpp]:Back Wince Action:1,Video:1\r\n")));
		}
		else//有倒车没视频
		{
			SendBackStatusToDriver(2);
			RETAILMSG(1 , (TEXT("\r\n[Back-BCLib.cpp]:Back Wince Action:1,Video:0\r\n")));
		}
	}
}


extern "C"
BOOL BCAllocateResource()
{
	static BOOL fgFirst = TRUE;
	HANDLE hBackCar = OpenEvent(EVENT_ALL_ACCESS,FALSE,BACKCAR_START_EVENT);
	if(hBackCar == NULL)
	{
		
		hBackCar = CreateEvent(0, FALSE, FALSE, BACKCAR_START_EVENT);
	}
	
	if(hBackCar == NULL)
	{
		RETAILMSG(TRUE, (TEXT("\r\n[Back-BCLib.cpp]Warning  Create BackCar Start Event Failed:%s\r\n"),BACKCAR_START_EVENT));
	}
	else
	{    
		RETAILMSG(TRUE,(TEXT("\r\n[Back-BCLib.cpp]  Set Event Data .FSC_BACKCAR_STATUS_START:%s\r\n"),BACKCAR_START_EVENT));
        SetEventData(hBackCar,FSC_BACKCAR_STATUS_START);
        SetEvent(hBackCar);
	}

	PostMessage(HWND_BROADCAST,WM_FSC_BACKCAR,FSC_BACKCAR_STATUS_START,0);
	
	RETAILMSG(1, (TEXT("[BCLIB]backcar create filemap\r\n")));
	if (NULL == g_hBackcarAVFileMap)
	{
		 g_hBackcarAVFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
                    0, sizeof(DWORD), L"BackCar0"); //only for pri channel now
        if (g_hBackcarAVFileMap != NULL)
        {
            g_pfgBackcarAVStart = (DWORD *)MapViewOfFile(g_hBackcarAVFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
            if(NULL == g_pfgBackcarAVStart)
            {
                CloseHandle(g_hBackcarAVFileMap);
                g_hBackcarAVFileMap = NULL;
                RETAILMSG(1, (TEXT("[BCLIB]: CreateFileMapping share with Avswitch failed!\r\n")));
            }
            else
            {
                *g_pfgBackcarAVStart  &= ~(BACKCAR_START_MASK);
            }
        }
	}

    if (NULL == g_hBackcarAvEvent)
    {
        g_hBackcarAvEvent = CreateEvent(NULL, TRUE, FALSE, BACKCAR_AV_EVENT);
    }
		
	//CreateBackCarUISurface(hWnd);	
	/********************************************************/
	//ShowWindow(hWnd,TRUE);

    //SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE); 
    //SetActiveWindow(hWnd);
    //SetForegroundWindow(hWnd);
	/*************************************************************///modify by jdp 20120907
	#if 0
	ShowBackCarUIOverlay();
	#endif


	g_BackCarStatus =  BACKCAR_STATUS_RUNNING;
	if(!g_fARM2SwitchBackCarToARM1)
	{
		RETAILMSG(TRUE,(TEXT("\r\n[Back-BCLib.cpp] Back setEvent g_TVDResourceEnvt")));
		//SetEvent(g_TVDResourceEnvt);
		//Do tvd Init
		if(fgFirst)
		{
			fgFirst = FALSE;
			TvdPreInit(hWnd);
		}

		Sleep(10);
		TvdInit(hWnd);

		if (g_uTimerId != 0)
		{
			::KillTimer(NULL, g_uTimerId);
			g_uTimerId = 0;
		}
		g_uTimerId = ::SetTimer(NULL, 1, 500, TimerProc);		
	}

    CloseHandle(hBackCar);
	return TRUE;

}
#if 0
extern "C"
BOOL BCReleaseResource()
{
	HANDLE hBackCar = NULL;
	HANDLE hFSCDriver = NULL;
	DWORD	dwARM2Status,dwSize;
	g_BackCarStatus =  BACKCAR_STATUS_STOP;

	DestroyBackCarSurfaces();
	//TvdStop(FALSE);
	
	//SoundStop();
	if(g_fARM2SwitchBackCarToARM1)
	{
		hFSCDriver = CreateFile(L"FSC1:",
								GENERIC_READ|GENERIC_WRITE,
								0,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
		DeviceIoControl ( hFSCDriver,
						IOCTL_FSC_NOTIFY_ARM2_STOP,
						NULL,
						0,
						&dwARM2Status,
						sizeof(DWORD),
						&dwSize,
						NULL
						);
		//RETAILMSG(TRUE, (TEXT("[fastcarmer APP]Call  ExtEscape\r\n")));
		//ExtEscape(GetDC(NULL), DISPLAY_IOCTL_HIDE_BACK_VIDEO, 0, NULL,0, NULL);
		g_fARM2SwitchBackCarToARM1 = FALSE;
		CloseHandle(hFSCDriver);
	}
	else
	{
		TvdStop(FALSE);
	}

	ShowWindow( hWnd,FALSE);
	PostMessage(HWND_BROADCAST,WM_FSC_BACKCAR,FSC_BACKCAR_STATUS_FIN,0);
	

	hBackCar = CreateEvent(0, FALSE, FALSE, BACKCAR_START_EVENT);
	
	if(hBackCar == NULL)
	{
		RETAILMSG(TRUE, (TEXT("[fastcarmer APP]Warning  Create BackCar Start Event Failed\r\n")));
	}
	else
	{    
         SetEventData(hBackCar,FSC_BACKCAR_STATUS_FIN);
         SetEvent(hBackCar);
	   CloseHandle(hBackCar);
	}
	return TRUE;
}
#endif
extern "C"
BOOL BCReleaseResource()
{
	HANDLE hBackCar = NULL;
	HANDLE hFSCDriver = NULL;
	DWORD	dwARM2Status,dwSize;
	g_BackCarStatus =  BACKCAR_STATUS_STOP;
		
	if (g_pfgBackcarAVStart != NULL)
	{
	    *g_pfgBackcarAVStart  &= ~(BACKCAR_START_MASK);
	    UnmapViewOfFile((HANDLE)g_pfgBackcarAVStart);
	    g_pfgBackcarAVStart = NULL;
	}
	
	if (g_hBackcarAVFileMap != NULL)
	{
	    CloseHandle(g_hBackcarAVFileMap);
	    g_hBackcarAVFileMap = NULL;
	}


	if (g_hBackcarAvEvent)
	{
		CloseHandle(g_hBackcarAvEvent);
		g_hBackcarAvEvent = NULL;
	}


	DestroyBackCarSurfaces();
	//TvdStop(FALSE);
	
	//SoundStop();
	if(g_fARM2SwitchBackCarToARM1)
	{
		hFSCDriver = CreateFile(L"FSC1:",
								GENERIC_READ|GENERIC_WRITE,
								0,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
		DeviceIoControl ( hFSCDriver,
						IOCTL_FSC_NOTIFY_ARM2_STOP,
						NULL,
						0,
						&dwARM2Status,
						sizeof(DWORD),
						&dwSize,
						NULL
						);
		//RETAILMSG(TRUE, (TEXT("[fastcarmer APP]Call  ExtEscape\r\n")));
		//ExtEscape(GetDC(NULL), DISPLAY_IOCTL_HIDE_BACK_VIDEO, 0, NULL,0, NULL);
		g_fARM2SwitchBackCarToARM1 = FALSE;
		CloseHandle(hFSCDriver);
	}
	else
	{
		TvdStop(FALSE);
	}

	//ShowWindow( hWnd,FALSE);				//modify owl 2012-11-14(del)
	PostMessage(HWND_BROADCAST,WM_FSC_BACKCAR,FSC_BACKCAR_STATUS_FIN,0);
	
	hBackCar = OpenEvent(EVENT_ALL_ACCESS,FALSE,BACKCAR_START_EVENT);
	if(hBackCar == NULL)
	{

		hBackCar = CreateEvent(0, FALSE, FALSE, BACKCAR_START_EVENT);
	}
		
	if(hBackCar == NULL)
	{
		RETAILMSG(TRUE, (TEXT("[fastcarmer APP]Warning  Create BackCar Start Event Failed\r\n")));
	}
	else
	{    
		SetEventData(hBackCar,FSC_BACKCAR_STATUS_FIN);
		SetEvent(hBackCar);
		CloseHandle(hBackCar);
	}
	return TRUE;
}


extern "C"
DWORD  BCWaitInitFin(DWORD *dwDis)
{
	TransferData TsSta;
	DWORD	dwARM2Status,dwSize;
	HANDLE hFSCDriver = NULL;
	BOOL bRet = FALSE;
		
	g_VideoRenderEvt = CreateEvent(NULL,FALSE, FALSE, NULL);
	if(NULL == g_VideoRenderEvt)
	{
		
		RETAILMSG(TRUE, (TEXT("CreateEvent VideoRender Event Failed\r\n")));
		return 0;
	}
	
	g_TVDResourceEnvt = CreateEvent(NULL,FALSE, FALSE, NULL);
	if(NULL == g_TVDResourceEnvt)
	{
		RETAILMSG(TRUE, (TEXT("CreateEvent TVD resource Event Failed\r\n")));
		return 0;
	}

	hFSCDriver = CreateFile(L"FSC1:",
								GENERIC_READ|GENERIC_WRITE,
								0,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
	if(INVALID_HANDLE_VALUE == hFSCDriver)
	{
		RETAILMSG(1, (L"Failed to open FSC1:\r\n"));
		return 0;
	}
		
	bRet = DeviceIoControl ( hFSCDriver,
							IOCTL_FSC_NOTIFY_APP_READY,
							NULL,
							0,
							&TsSta,
							sizeof(DWORD),
							&dwSize,
							NULL
							);
	*dwDis = TsSta.dwDis;
	CloseHandle(hFSCDriver);
    
	//ShowWindow(hWnd,FALSE);  	//modify owl 2012-11-14
	if(ARM2_STATUS_BACKING_CAR != TsSta.dwBackCarSta){
			//RETAILMSG(TRUE, (TEXT("[fastcarmer APP]Call  ExtEscape\r\n")));
			//ExtEscape(GetDC(NULL), DISPLAY_IOCTL_HIDE_BACK_VIDEO, 0, NULL,0, NULL)
	}
	else{
		g_fARM2SwitchBackCarToARM1 = TRUE;
	}
	return TsSta.dwBackCarSta;
}


extern "C"
BOOL BCUIInit()
{
   return TRUE;
}

extern "C"
BOOL BCUIDrawPoint(UINT32 x,UINT32 y)
{

    
	return  BC_DrawPoint(x,y);
    
   
}
extern "C"
BOOL BCUIDrawLine(POINT Start,POINT End)
{
   
   return   BC_DrawLine(Start,End);
}
extern "C"
BOOL BCUIBitBlt(int nXDest, int nYDest, int nWidth,  int nHeight,UINT16 *pSrcData, int nXSrc, int nYSrc,UINT16 BgColor)
{

   return BC_BitBlt(nXDest,  nYDest,  nWidth,   nHeight, pSrcData,  nXSrc,  nYSrc, BgColor);
}

extern "C"
BOOL BCUISetBkColor(UINT16 bkColor)
{
   return BC_SetBkColor(bkColor);
  
}

extern "C"
BOOL BCUISetPenColor(UINT16 PenColor)
{
  return BC_SetPenColor(PenColor);
}


extern "C"
BOOL BCUIInvalidateRegion(INT32 x,INT32 y,UINT32 nWidth,UINT32 nHeight)
{

   return BC_InvalidateRegion(x,y,nWidth,nHeight);
   
}



extern "C"
BOOL  BCPlaySound(void* psndmem,UINT32 u4sndsz)
{


   SoundInit();
   SoundStart(psndmem,u4sndsz);
   SoundPlay();

   return TRUE;
}

//HANDLE      	  g_hEventBackSig = NULL;
//#define  FLY_EVENT_NAME_CAMERA_SIG TEXT("FLY_BACK_CAMERA_ON_OFF")


//{
//	SetEventData(g_hEventBackSig, iData);
//	SetEvent(g_hEventBackSig);
//}

DWORD WINAPI FetchBackCarResourceThread(VOID *pPddContext )
{
	static BOOL fgFirst = TRUE;
	DWORD code = 0;
	BOOL fTVDInitRet = FALSE;
	BOOL fVDOInitRet = FALSE;
	BOOL bSignal = FALSE;    

	//g_hEventBackSig = OpenEvent(EVENT_ALL_ACCESS , FALSE , FLY_EVENT_NAME_CAMERA_SIG);//OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT("SYSTEM/BackCarAppReady"));
	//if(g_hEventBackSig==NULL)
	//{
	//	g_hEventBackSig = CreateEvent(NULL, FALSE, FALSE, FLY_EVENT_NAME_CAMERA_SIG);
	//}
	
	RETAILMSG(TRUE, (TEXT("\r\n[FSC]FetchBackCarResourceThread start........\r\n")));

	while(TRUE)
	{

		code = WaitForSingleObject(g_TVDResourceEnvt, INFINITE);
		//code = WaitForSingleObject(g_VideoRenderEvt, 1*1000);
		if(WAIT_TIMEOUT == code)
			RETAILMSG(TRUE, (TEXT("[FSC]Warning Wait APP Release time-out\r\n")));

		continue;

		if(g_BackCarStatus ==  BACKCAR_STATUS_RUNNING)
		{
			if(fgFirst)
			{
				fgFirst = FALSE;
				TvdPreInit(hWnd);
			}
            
			Sleep(10);
			fTVDInitRet = TvdInit(hWnd);

            /*for (int i = 0; i < 3; i++)
            {
                fTVDInitRet = TvdInit(hWnd);
                if (fTVDInitRet)
                {
                    break;
                }
                else
                {
                    Sleep(100);
                }
            }*/

			if(fTVDInitRet)
			{
				
				/*Sleep(500);

				if (GpioRead() == 0)
				{
					SendBackStatusToDriver(0);//没倒车
					RETAILMSG(1 , (TEXT("\r\n[Back-BCLib.cpp]:Back Wince Action:0,Video:0\r\n")));
				}
				else
				{
					bSignal = TvdSignalIsOn();
					RETAILMSG(TRUE,(TEXT("\r\n*************************Back Signal =%d  Init TVD Success--\r\n"),bSignal));
					
					if (bSignal)//有倒车有视频
					{
						//SendBackStatusToDriver(3);
						//RETAILMSG(1 , (TEXT("\r\n[Back-BCLib.cpp]:Back Wince Action:1,Video:1\r\n")));
					}
					else//有倒车没视频
					{
						SendBackStatusToDriver(2);
						RETAILMSG(1 , (TEXT("\r\n[Back-BCLib.cpp]:Back Wince Action:1,Video:0\r\n")));
					}
				}*/

				/*
				if (GpioRead() == 0)
				{

					//DestroyBackCarSurfaces();
					BCReleaseResource();
					//TvdStop(FALSE);
					RETAILMSG(TRUE, (TEXT("Stop TVD success!!\r\n")));
				}
				*/
				RETAILMSG(TRUE, (TEXT("Init TVD success\r\n")));
			}
			else
			{
				RETAILMSG(TRUE, (TEXT("Init TVD Failed\r\n")));
			}
		}
		//else
		//{
		//	//RETAILMSG(TRUE, (TEXT("\r\n g_BackCarStatus ==  BACKCAR_STATUS_STOP\r\n")));
		//}

	}
	return 0;
}


extern "C"
DWORD  BCCustomEntry();

DWORD WINAPI BackCarThread(VOID *pPddContext )
{
	hWnd = (HWND)pPddContext;
	#ifndef BSP_ARM2 
	//RETAILMSG(TRUE, (TEXT("[fastcarmer APP]Call BSP_ARM2  ExtEscape\r\n")));
	//ExtEscape(GetDC(NULL), DISPLAY_IOCTL_HIDE_BACK_VIDEO, 0, NULL,0, NULL);
	return TRUE;
	#endif
	 
	// TvdPreInit(hWnd);
	if(INVALID_HANDLE_VALUE == CreateThread(NULL, 0, FetchBackCarResourceThread, (LPVOID)hWnd, 0, NULL))
	{
		RETAILMSG(1,(TEXT("create FetchBackcarResourceThread failed!\r\n")));
		return -1;
	}
	
	BCCustomEntry();
	return 0;
}

extern "C"
BOOL Arm2BuffMemset()
{

	HANDLE hFSCDriver = NULL;
	DWORD	dwARM2Status,dwSize;

    hFSCDriver = CreateFile(L"FSC1:",
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);
	DeviceIoControl ( hFSCDriver,
					IOCTL_FSC_NOTIFY_ARM2_BUFF_MEMSET,
					NULL,
					0,
					&dwARM2Status,
					sizeof(DWORD),
					&dwSize,
					NULL
					);
	CloseHandle(hFSCDriver);


   return TRUE;
  
}


DWORD WINAPI BackCarListenThread(VOID *pPddContext )
{
    DWORD ret = 0;
	g_hEventBackcarListen = CreateEvent(NULL, FALSE, TRUE, EVENT_BACKCAR_LISTEN_THREAD);
	while(TRUE)
	{
		ret = WaitForSingleObject(g_hEventBackcarListen, INFINITE);
		//CameraStateCfg= GetEventData(g_hEventBackcarListen);	
		//SetEvent(g_hEventBackcarEnabel);
		//RETAILMSG(1 , (TEXT("\r\nGet Back Enable:%d\r\n"),CameraStateCfg));
	}
}
