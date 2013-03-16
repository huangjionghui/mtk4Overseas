#include <windows.h>
#include <types.h>
#include <excpt.h>
#include <tchar.h>

#include <tchddsi.h>
#include <Nkintr.h>
#include <auxadc.h>

#include <ceddk.h>
#include <Pkfuncs.h>
#include <oal_intr.h>
#include <bsp_cfg.h>
#include <x_timer.h>
#include <irqs_vector.h>

#include <x_hal_ic.h>
#include <gpio.h>
#include "x_bim_3360.h"
#include "base_regs.h"
#include "x_ckgen_3360.h"

#include "FlyHwdrv.h"


HANDLE ghKnobEvent = NULL ;
HANDLE ghKnobThread  = NULL;
DWORD  gIntrknobTurnLight	= 0;
DWORD  gIntrknobTurnRight	= 0;

DWORD  gIntrknobTurnLightL	= 0;
DWORD  gIntrknobTurnRightR	= 0;

DWORD  gIrqsTable[3] = {-1, OAL_INTR_FORCE_STATIC, 0};

typedef void (*KeyIqrXX_t)(BOOL bRValue,BOOL bLValue);

KeyIqrXX_t KeyIqrRR = NULL;
KeyIqrXX_t KeyIqrRL = NULL;
KeyIqrXX_t KeyIqrLR = NULL;
KeyIqrXX_t KeyIqrLL = NULL;

enum ekey{
	KEY_RIGHT_RIGHT=0,
	KEY_RIGHT_LIGHT,
	KEY_LIGHT_RIGHT,
	KEY_LIGHT_LIGHT
};



static void setGpioAsInterruptMoudles(void)
{
	int tmp = 0;

	//pinmux ext5~8
	tmp = IO_READ32(IO_BASE_VA, 0X68);
	tmp |= 0x1b3300;
	IO_WRITE32(IO_BASE_VA, 0X68, tmp);

	tmp = BIM_READ32(0Xa8);
	tmp |= 0x55100000;
	BIM_WRITE32(0Xa8, tmp);


	// Be sure 0x68[21:19] not equal to 3
	tmp = IO_READ32(IO_BASE_VA, 0X68);
	if (((tmp >> 19) & 0x7) == 0x3)
	{
		tmp &= ~(0x7 << 19);
		IO_WRITE32(IO_BASE_VA, 0X68, tmp);
	}
	// Be sure 0x54[10:8] not equal to 3
	tmp = IO_READ32(IO_BASE_VA, 0X54);
	if (((tmp >> 8) & 0x7) == 0x3)
	{
		tmp &= ~(0x7 << 8);
		IO_WRITE32(IO_BASE_VA, 0X54, tmp);
	}
	// Be sure 0x58[6:4] not equal to 2
	tmp = IO_READ32(IO_BASE_VA, 0X58);
	if (((tmp >> 4) & 0x7) == 0x2)
	{
		tmp &= ~(0x7 << 4);
		IO_WRITE32(IO_BASE_VA, 0X58, tmp);
	}
	// Be sure 0x5c[19:18] not equal to 1
	tmp = IO_READ32(IO_BASE_VA, 0X5c);
	if (((tmp >> 18) & 0x3) == 0x1)
	{
		tmp &= ~(0x3 << 18);
		IO_WRITE32(IO_BASE_VA, 0X5c, tmp);
	}
}

#if FLYMTK_PANEL_V2 
static void KeyIrqAction(BYTE key)
{
	switch (key)
	{
	case KEY_RIGHT_RIGHT:
		if (KeyIqrLL)//×ó×ó£¬volume-
			KeyIqrLL(FALSE,TRUE);
		break;

	case KEY_RIGHT_LIGHT:
		if (KeyIqrLR)//×óÓÒ£¬volume+
			KeyIqrLR(TRUE,FALSE);
		break;

	case KEY_LIGHT_RIGHT:
		if (KeyIqrRR)//ÓÒÓÒ,freq+
			KeyIqrRR(TRUE,FALSE);
		break;

	case KEY_LIGHT_LIGHT:
		if (KeyIqrRL)	//ÓÒ×ó£¬freq-
			KeyIqrRL(FALSE,TRUE);
		break;

	default:
		break;
	}
}
#else
static void KeyIrqAction(BYTE key)
{
	switch (key)
	{
	case KEY_RIGHT_RIGHT:
		if (KeyIqrRR)//ÓÒÓÒ,freq+
			KeyIqrRR(TRUE,FALSE);
		break;

	case KEY_RIGHT_LIGHT:
		if (KeyIqrRL)	//ÓÒ×ó£¬freq-
			KeyIqrRL(FALSE,TRUE);
		break;

	case KEY_LIGHT_RIGHT:
		if (KeyIqrLR)//×óÓÒ£¬volume+
			KeyIqrLR(TRUE,FALSE);
		break;

	case KEY_LIGHT_LIGHT:
		if (KeyIqrLL)//×ó×ó£¬volume-
			KeyIqrLL(FALSE,TRUE);
		break;

	default:
		break;
	}
}
#endif


static DWORD Knob_Thread( VOID *pPddContext )
{
	UINT32 tmp = 0;
	UINT32 udir = 0;
	BYTE   key = 0;

	while(1)
	{
		WaitForSingleObject(ghKnobEvent, INFINITE); 

		//·½°¸¶þ
		if(BIM_READ32(0X230)&0x4)//ÓÒ±ß-Æµµã
		{
			GPIO_InOut_Sel(107,0);
			udir = GPIO_Input(107);

			tmp = 0;
			tmp = BIM_READ32(0xa8);
			if (((tmp & (0x1<<26))>>26) == 1)
			{
				if(1 == udir)
					key = KEY_RIGHT_LIGHT;
				else
					key = KEY_RIGHT_RIGHT;

				tmp = tmp & ~(0x1<<26);
			}
			else
			{
				if(1 == udir)
					key = KEY_RIGHT_RIGHT;
				else
					key = KEY_RIGHT_LIGHT;

				tmp = tmp | (0x1<<26);
			}
			BIM_WRITE32(0xA8, tmp);
		}
		else//×ó±ß-ÒôÁ¿
		{
			GPIO_InOut_Sel(106,0);
			udir = GPIO_Input(106);

			tmp = 0;
			tmp = BIM_READ32(0xa8);
			if (((tmp & (0x1<<28))>>28) == 1)
			{
				if(1 == udir)
					key = KEY_LIGHT_LIGHT;
				else
					key = KEY_LIGHT_RIGHT;

				tmp = tmp & ~(0x1<<28);
			}
			else
			{
				if(1 == udir)
					key = KEY_LIGHT_RIGHT;
				else
					key = KEY_LIGHT_LIGHT;

				tmp = tmp | (0x1<<28);
			}
			BIM_WRITE32(0xA8, tmp);
		}
		
		InterruptDone(gIntrknobTurnLight);
		InterruptDone(gIntrknobTurnRight);
		InterruptDone(gIntrknobTurnLightL);
		InterruptDone(gIntrknobTurnRightR);

		KeyIrqAction(key);
	}

	if(ghKnobEvent)
		CloseHandle(ghKnobEvent);

	return 0;
}

BOOL InitKeyInterreptModules(void)
{
	setGpioAsInterruptMoudles();

	gIrqsTable[2] = VECTOR_EXT6;
	if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, 
		gIrqsTable, 3*sizeof(DWORD), &gIntrknobTurnLight, sizeof(UINT32), NULL))
	{
		RETAILMSG(1, (TEXT("[HW irq]Knob_Init : Can't get VECTOR_EXT6\r\n")));
		gIntrknobTurnLight = SYSINTR_UNDEFINED;
		return FALSE;
	}

	gIrqsTable[2] = VECTOR_EXT7;
	if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, 
		gIrqsTable, 3*sizeof(DWORD), &gIntrknobTurnRight, sizeof(UINT32), NULL))
	{
		RETAILMSG(1, (TEXT("[HW irq]Knob_Init : Can't get VECTOR_EXT7\r\n")));
		gIntrknobTurnRight = SYSINTR_UNDEFINED;
		return FALSE;
	}

	gIrqsTable[2] = VECTOR_EXT5;//4
	if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, 
		gIrqsTable, 3*sizeof(DWORD), &gIntrknobTurnLightL, sizeof(UINT32), NULL))
	{
		RETAILMSG(1, (TEXT("[HW irq]Knob_Init : Can't get VECTOR_EXT7\r\n")));
		gIntrknobTurnRight = SYSINTR_UNDEFINED;
		return FALSE;
	}

	gIrqsTable[2] = VECTOR_EXT8;//7
	if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, 
		gIrqsTable, 3*sizeof(DWORD), &gIntrknobTurnRightR, sizeof(UINT32), NULL))
	{
		RETAILMSG(1, (TEXT("[HW irq]Knob_Init : Can't get VECTOR_EXT7\r\n")));
		gIntrknobTurnRight = SYSINTR_UNDEFINED;
		return FALSE;
	}

	ghKnobEvent = CreateEvent( NULL, FALSE, FALSE, NULL);
	if ( !InterruptInitialize(gIntrknobTurnLight,ghKnobEvent,NULL,0))
	{
		RETAILMSG(1, (TEXT("[HW irq]Error initializing interrupt\n\r")));
		return FALSE;
	}

	if ( !InterruptInitialize(gIntrknobTurnRight,ghKnobEvent,NULL,0))
	{
		RETAILMSG(1, (TEXT("[HW irq]Error initializing interrupt\n\r")));
		return FALSE;
	}

	if ( !InterruptInitialize(gIntrknobTurnLightL,ghKnobEvent,NULL,0))
	{
		RETAILMSG(1, (TEXT("[HW irq]Error initializing interrupt\n\r")));
		return FALSE;
	}

	if ( !InterruptInitialize(gIntrknobTurnRightR,ghKnobEvent,NULL,0))
	{
		RETAILMSG(1, (TEXT("[HW irq]Error initializing interrupt\n\r")));
		return FALSE;
	}

	InterruptDone(gIntrknobTurnLight);
	InterruptDone(gIntrknobTurnRight);
	InterruptDone(gIntrknobTurnLightL);
	InterruptDone(gIntrknobTurnRightR);

	ghKnobThread = CreateThread(NULL, 0, Knob_Thread, NULL, 0, NULL);
	CeSetThreadPriority(ghKnobThread, 40);
	if (ghKnobThread == NULL) 
	{
		RETAILMSG(1, (TEXT("[HW irq]failed for creating thread! \r\n")));
		return FALSE;
	}
	
	return TRUE;
}

void EnableKeyIrq(void *key_irq_rr,
	void *key_irq_rl,void *key_irq_lr,void *key_irq_ll,BOOL bEnable)
{
	if (bEnable)
	{
		KeyIqrRR = (KeyIqrXX_t)key_irq_rr;
		KeyIqrRL = (KeyIqrXX_t)key_irq_rl;
		KeyIqrLR = (KeyIqrXX_t)key_irq_lr;
		KeyIqrLL = (KeyIqrXX_t)key_irq_ll;
		InitKeyInterreptModules();
	}
	else
	{
		KeyIqrRR = NULL;
		KeyIqrRL = NULL;
		KeyIqrLR = NULL;
		KeyIqrLL = NULL;
	}
}