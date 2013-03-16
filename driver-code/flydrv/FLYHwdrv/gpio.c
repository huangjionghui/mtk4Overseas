#include <windows.h>
#include <ethdbg.h>
#include "x_hal_io.h"
#include "x_hal_ic.h"
#include "x_gpio.h"

#include "base_regs.h"
#include "mt33xx_def.h"

// for customer
#include "bsp_cfg.h"
#ifdef BSP_HAS_CUSTOM_ONE
#include "YcUse\ycconfig.c"
#endif
#include "flygpio.h"

#define IO_BASE_VA              0xA0000000





void SetGpioModel(void)
{
	UINT32 tmp = 0;


	//GPIO54:
	//0x54[14:12] != 3
	//0x58[27:25] != 2
	//0x54[5:3] != 3
	//0x60[29:28] != 2
	//0x64[3:2] != 2
	tmp = IO_READ32(IO_BASE_VA, 0x54);
	tmp = tmp &(~(1u<<13));
	IO_WRITE32(IO_BASE_VA, 0x54, tmp);

	tmp = IO_READ32(IO_BASE_VA, 0x58);
	tmp = tmp &(~(1u<<26));
	IO_WRITE32(IO_BASE_VA, 0x58, tmp);

	tmp = IO_READ32(IO_BASE_VA, 0x54);
	tmp = tmp &(~(1u<<4));
	IO_WRITE32(IO_BASE_VA, 0x54, tmp);

	tmp = IO_READ32(IO_BASE_VA, 0x60);
	tmp = tmp &(~(1u<<29));
	IO_WRITE32(IO_BASE_VA, 0x60, tmp);

	tmp = IO_READ32(IO_BASE_VA, 0x64);
	tmp = tmp &(~(1u<<3));
	IO_WRITE32(IO_BASE_VA, 0x64, tmp);

#if FLYMTK_BOARD_V1

#else

	//GPIO 55
	tmp = 0;
	tmp = IO_READ32(IO_BASE_VA, 0x54);
	tmp = tmp &(~(1u<<16));
	IO_WRITE32(IO_BASE_VA, 0x54, tmp);

	tmp = IO_READ32(IO_BASE_VA, 0x58);
	tmp = tmp &(~(1u<<26));
	IO_WRITE32(IO_BASE_VA, 0x58, tmp);

	tmp = IO_READ32(IO_BASE_VA, 0x60);
	tmp = tmp &(~(1u<<31));
	IO_WRITE32(IO_BASE_VA, 0x60, tmp);
#endif
	

	//GPIO 57
	tmp = 0;
	tmp = IO_READ32(IO_BASE_VA, 0X240f8);
	tmp = tmp &(~(1u<<16));
	IO_WRITE32(IO_BASE_VA, 0X240f8, tmp);

	tmp = 0;
	tmp = IO_READ32(IO_BASE_VA, 0X240f4);
	tmp = tmp |((1U<<16));
	IO_WRITE32(IO_BASE_VA, 0X240f4, tmp);


	//GPIO 58
	//将GPIO58配置成为普通的GPIO口-----需配置寄存器 0X240F8[16]=0以及0x240f4[16]=1;
	tmp = 0;
	tmp = IO_READ32(IO_BASE_VA, 0X58);
	tmp = tmp &(~(1u<<26));
	IO_WRITE32(IO_BASE_VA, 0X58, tmp);
	
	tmp = 0;
	tmp = IO_READ32(IO_BASE_VA, 0X240F8);
	tmp = tmp & (~(1U<<16));
	IO_WRITE32(IO_BASE_VA, 0X240F8, tmp);

	tmp = 0;
	tmp = IO_READ32(IO_BASE_VA, 0x240f4);
	tmp = tmp | (1U<<16);
	IO_WRITE32(IO_BASE_VA, 0x240f4, tmp);
	GPIO_Config(58,INPUT,HIGH);
}

void SetGpio54Status(BOOL bHigh)
{
	if (bHigh)
		GPIO_Config(54,OUTPUT, HIGH);
	else
		GPIO_Config(54,OUTPUT, LOW);
}

void SetGpio55Status(BOOL bHigh)
{
#if FLYMTK_BOARD_V1

#else
	if (bHigh)
		GPIO_Config(55,OUTPUT, HIGH);
	else
		GPIO_Config(55,OUTPUT, LOW);
#endif
}


void SetGpio57Status(BOOL bHigh)
{
	if (bHigh)
		GPIO_Config(57,OUTPUT, HIGH);
	else
		GPIO_Config(57,OUTPUT, LOW);

}

UINT32 GetGpio58Status(INT32 gpio)
{
	return GPIO_Input(gpio);
}