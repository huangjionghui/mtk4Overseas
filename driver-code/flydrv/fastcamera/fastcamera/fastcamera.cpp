/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
/*****************************************************************************
*
* Filename:
* ---------
*   dualtestdriver.c
*
* Project:
* --------
*   WINCE600
*
* Description:
* ------------
*   Source codes of dual test driver for WINCE device driver interface.
*
*
* Author:
* -------
*   Bin Yang (MTK40084)
*
*============================================================================
*             HISTORY
* Below this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
*------------------------------------------------------------------------------
* $Revision: #1 $
* $Modtime:$
* $Log:$
*
*------------------------------------------------------------------------------
* Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
*============================================================================
****************************************************************************/
#include <windows.h>
#include <nkintr.h>
#include <ceddk.h>
#include <ceddkex.h>
#include "irqs_vector.h"
#include "drv_dual.h"
#include "fsc.h"
#include "backcar_msg.h"
DBGPARAM dpCurSettings;


#define ZONE_ERROR 1


BOOL g_InterruptThreadExit = FALSE;
#define  BACKCAR_EVENT_NAME (L"SYSTEM/ARM2BackCarFinished")
HANDLE g_hBackCar;
DWORD  g_dwBackCarSysIntr;
HANDLE g_hBackCarIntrEvt;
HANDLE g_hBackIntrThread;
HANDLE g_ARM2CommEnvt;





SHAREMEM *AllocateShareMem(DWORD dwSize,DWORD *pdwPhyAddr)
{
	DMA_ADAPTER_OBJECT dmaobj;
	PVOID				pMemAddr;		// Virtual address of transfer buffer
	PHYSICAL_ADDRESS	PhyAddr;		
	
	dmaobj.ObjectSize = sizeof(DMA_ADAPTER_OBJECT);
	dmaobj.InterfaceType = Internal;
	dmaobj.BusNumber = 1;
	pMemAddr = HalAllocateCommonBuffer(&dmaobj, dwSize, &PhyAddr , FALSE);
    *pdwPhyAddr = PhyAddr.LowPart;
	
    return (SHAREMEM *)pMemAddr;
}

BOOL FreeShareMem(SHAREMEM *pShareM,DWORD dwSize,DWORD dwPhyAddr)
{
	DMA_ADAPTER_OBJECT dmaobj;
	PHYSICAL_ADDRESS	PhyAddr;		
    memset(&PhyAddr,0x0,sizeof(PHYSICAL_ADDRESS));
	dmaobj.ObjectSize = sizeof(DMA_ADAPTER_OBJECT);
	dmaobj.InterfaceType = Internal;
	dmaobj.BusNumber = 1;	
	PhyAddr.LowPart = dwPhyAddr;
	
	HalFreeCommonBuffer(&dmaobj, dwSize, PhyAddr , (PVOID)pShareM, FALSE);
	
   return TRUE;
   
}


BOOL GetARM2BackCarStatus(UCHAR *pOutBuffer,DWORD outSize,DWORD *pOutSize)
{
	UINT32	g_u4Param1,g_u4Param2,g_u4Param3;
    DWORD dwRet;
	UINT32 u4ModuleID,u4MessageID;
	HWSendMessage(MSG_COMBINE(MODULE_BCAR, MSG_WINCE_APP_READY),0, 0, 0);
			
			
	dwRet = WaitForSingleObject(g_hBackCarIntrEvt, INFINITE);
			
	HWGetMessage(MODULE_BCAR, &u4ModuleID, &g_u4Param1, &g_u4Param2, &g_u4Param3);
			
	u4MessageID = GETMESSAGEID(u4ModuleID);
	if(u4MessageID == MSG_ARM2_RESPONSE)
	   RETAILMSG(TRUE, (TEXT("[FSC][INFO]Rev ARM2 Responese\r\n")));
			
			
	memcpy(pOutBuffer,(UCHAR *)&g_u4Param1,4);
	memcpy(pOutBuffer+4,(UCHAR *)&g_u4Param2,4);
	*pOutSize = 8;
	InterruptDone(g_dwBackCarSysIntr);
	return TRUE;
}


BOOL GetMCUInfo(UCHAR *pOutBuffer,DWORD outSize,DWORD *pOutSize)
{
	UINT32	g_u4Param1,g_u4Param2,g_u4Param3;
    DWORD dwRet;
	UINT32 u4ModuleID,u4MessageID;
    DWORD dwPhyAddr = 0;
    SHAREMEM *pShareM = NULL;


	
	pShareM = AllocateShareMem(4*1024,&dwPhyAddr);
	pShareM->u4Size = sizeof(SHAREMEM);
	pShareM->ShareMemT = SHAREMEM_MCU_INFO;
	pShareM->u4ResponeStatus = 0;
	pShareM->u4BufferSize= 4*1024 - (sizeof(SHAREMEM));

	
	HWSendMessage(MSG_COMBINE(MODULE_BCAR, MSG_WINCE_GET_MCU_INFO),dwPhyAddr, 0, 0);

	dwRet = WaitForSingleObject(g_hBackCarIntrEvt, INFINITE);
	HWGetMessage(MODULE_BCAR, &u4ModuleID, &g_u4Param1, &g_u4Param2, &g_u4Param3);
	u4MessageID = GETMESSAGEID(u4ModuleID);
	if(u4MessageID == MSG_ARM2_RESPONSE)
	   RETAILMSG(TRUE, (TEXT("[FSC][INFO]Rev ARM2 Responese\r\n")));
			
			
	memcpy(pOutBuffer,(UCHAR *)pShareM->Data,4);
	*pOutSize = 4;
	InterruptDone(g_dwBackCarSysIntr);
	FreeShareMem(pShareM,4*1024,dwPhyAddr);

	return TRUE;
}

BOOL GetARM2Version(UCHAR *pOutBuffer,DWORD outSize,DWORD *pOutSize)
{
	UINT32	g_u4Param1,g_u4Param2,g_u4Param3;
    DWORD dwRet;
	UINT32 u4ModuleID,u4MessageID,u4offset = sizeof(SHAREMEM);
    DWORD dwPhyAddr = 0;
    SHAREMEM *pShareM = NULL;


	
	pShareM = AllocateShareMem(4*20,&dwPhyAddr);
	pShareM->u4Size = sizeof(SHAREMEM);
	pShareM->ShareMemT = SHAREMEM_ARM2_VERSION;
	pShareM->u4ResponeStatus = 0;
	pShareM->u4BufferSize= 4*20 - (sizeof(SHAREMEM));

	
	HWSendMessage(MSG_COMBINE(MODULE_BCAR, MSG_GET_ARM2_VERSION),dwPhyAddr, u4offset, 0);

	dwRet = WaitForSingleObject(g_hBackCarIntrEvt, INFINITE);
	HWGetMessage(MODULE_BCAR, &u4ModuleID, &g_u4Param1, &g_u4Param2, &g_u4Param3);
	u4MessageID = GETMESSAGEID(u4ModuleID);
	if(u4MessageID == MSG_ARM2_RESPONSE)
	   RETAILMSG(TRUE, (TEXT("[FSC][INFO]Rev ARM2 Responese\r\n")));
			
	memcpy(pOutBuffer,(const void *)(0xAC000000+dwPhyAddr+u4offset),g_u4Param1);
	*pOutSize = g_u4Param1;
	InterruptDone(g_dwBackCarSysIntr);
	FreeShareMem(pShareM,4*20,dwPhyAddr);
	
	return TRUE;
}



static DWORD WINAPI 
DetectFSCUIReadyThread(
							VOID *pPddContext
							)
{

    HANDLE hUIReady;
	DWORD code;


	hUIReady = OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT("SYSTEM/BackCarUIReady"));
	if(NULL == hUIReady)
	{
			RETAILMSG(TRUE, (TEXT("Open SYSTEM/BackCarUIReady Failed\r\n")));
			return -1;
	}
	
	code = WaitForSingleObject(hUIReady, INFINITE);

	CloseHandle(hUIReady);
	return 0;
}
static DWORD WINAPI 
BackCarIST(
				VOID *pPddContext
				)
{
	DWORD code;
	UINT32  u4ModuleID;
    UINT32 u4MessageID;

	RETAILMSG(ZONE_ERROR, (TEXT("BackCarIST is running\r\n")));

	

	

	return 0;
}



BOOL WINAPI DllEntry(HINSTANCE hInstance, ULONG Reason, LPVOID pReserved)
{
	BOOL fRet = TRUE;
	if(Reason == DLL_PROCESS_ATTACH) 
	{
		DEBUGREGISTER(hInstance);
		DisableThreadLibraryCalls( (HMODULE) hInstance );

		
	}

	return fRet;
}


EXTERN_C DWORD FSC_Init(void *pContext)
{
	DWORD irqs[4], size;

	//clear register
	//fgClearGroup(); // It clear all share registers including ARM2 AEC. Don't do that( mtk40004)
	g_hBackCar = OpenEvent(EVENT_ALL_ACCESS,FALSE,BACKCAR_EVENT_NAME);
	if(g_hBackCar == NULL)
	{

		g_hBackCar = CreateEvent(0, FALSE, FALSE, BACKCAR_EVENT_NAME);
	}



	// Prepare irq list
	irqs[0] = -1;
	irqs[1] = 0;
	irqs[2] = VECTOR_INT_DUAL2;
	size = 3 * sizeof(DWORD);

	// request sysIntr
	if (!KernelIoControl(
		IOCTL_HAL_REQUEST_SYSINTR, irqs, size,
		&g_dwBackCarSysIntr, sizeof(g_dwBackCarSysIntr), NULL
		))
	{
		RETAILMSG(ZONE_ERROR, (TEXT("FSC_Init() IOCTL_HAL_REQUEST_SYSTINR call failed\r\n")));
		return FALSE;
	}

	RETAILMSG(ZONE_ERROR, (TEXT("FSC_Init() g_dwBackCarSysIntr = %d !\r\n"), g_dwBackCarSysIntr));

	// Create interrupt event
	g_hBackCarIntrEvt = CreateEvent(0, FALSE, FALSE, NULL);
	if ( g_hBackCarIntrEvt == NULL )
	{
		DEBUGMSG(ZONE_ERROR, (TEXT("FSC_Init() Error creating interrupt event\r\n")));
		return FALSE;
	}

	

	// Initialize interrupt
	if (!InterruptInitialize(g_dwBackCarSysIntr, g_hBackCarIntrEvt, NULL, 0))
	{
		RETAILMSG(ZONE_ERROR, (TEXT("FSC_Init() Interrupt initialization failed\r\n")));
		return FALSE;
	}

	// create the sleep control IST thread
	g_hBackIntrThread = CreateThread(NULL, 0, BackCarIST, NULL, 0, NULL);
	if (g_hBackIntrThread == NULL) {
		InterruptDisable(g_dwBackCarSysIntr);
		if (g_hBackCarIntrEvt) {
			CloseHandle(g_hBackCarIntrEvt);
			g_hBackCarIntrEvt = NULL;
		}

		RETAILMSG(ZONE_ERROR, (TEXT("FSC_Init() failed for creating sleep control IST thread\r\n")));
		return FALSE;
	}

	
	g_ARM2CommEnvt = CreateEvent(NULL,FALSE, FALSE, NULL);
	if(NULL == g_ARM2CommEnvt)
	{
		
		RETAILMSG(TRUE, (TEXT("CreateEvent ARM2 COMM Event Failed\r\n")));
		return FALSE;
	}



	//Create WINCE Ready detect Thread
	CreateThread(NULL, 0, DetectFSCUIReadyThread, NULL, 0, NULL);

    return ((DWORD)1);
}   /* FSC_Init() */

EXTERN_C BOOL FSC_Open(DWORD hDeviceContext, DWORD AccessMode, DWORD ShareMode )
{	
	DEBUGMSG(ZONE_ERROR, (TEXT("FSC_Open\r\n")));
    return TRUE;
}   /* DUA_Open() */


EXTERN_C BOOL FSC_Close(DWORD hDeviceContext)
{
	DEBUGMSG(ZONE_ERROR, (TEXT("FSC_Close\r\n")));
    return TRUE;
}   /* DUA_Close() */


EXTERN_C BOOL 
FSC_IOControl(
              DWORD context,
              DWORD code,
              UCHAR *pInBuffer,
              DWORD inSize,
              UCHAR *pOutBuffer,
              DWORD outSize,
              DWORD *pOutSize)

{
    	

    UINT32 u4McuInfoCode;
	BOOL  bRet = TRUE;
	DWORD retCode;
	
	switch (code)
	{
		case IOCTL_FSC_NOTIFY_APP_READY:
		{
		   //GetMCUInfo(pOutBuffer,outSize,pOutSize);
		   GetARM2BackCarStatus(pOutBuffer,outSize,pOutSize);
		}
		break;
		case IOCTL_FSC_GET_ARM2_VERSION:
		{
		   	RETAILMSG(TRUE, (TEXT("[FSC_IOControl] GetARM2Version\r\n")));
           		GetARM2Version(pOutBuffer,outSize,pOutSize);
		}
		break;
		case IOCTL_FSC_NOTIFY_ARM2_STOP:
		{
			RETAILMSG(TRUE, (TEXT("[FSC_IOControl] Notify ARM2 video stop\r\n")));
			HWSendMessage(MSG_COMBINE(MODULE_BCAR, MSG_NOTIFY_ARM2_STOP),0, 0, 0);
		}
		break;
		case IOCTL_FSC_NOTIFY_ARM2_BUFF_MEMSET:
		{
			HWSendMessage(MSG_COMBINE(MODULE_BCAR, MSG_NOTIFY_ARM2_BUFF_MEMSET),0, 0, 0);
		}
		break;
		default:
			RETAILMSG(TRUE, (TEXT("[FSC][INFO] Unsupport IOCTL code : %d\r\n"), code));
			break;
    }


	
    return (TRUE);
}   



EXTERN_C void FSC_Deinit(DWORD hDeviceContext)
{
	DEBUGMSG(ZONE_ERROR, (TEXT("FSC_Deinit\r\n")));

    
}   

