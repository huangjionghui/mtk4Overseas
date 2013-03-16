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

#include "FlyHwdrv.h"
#include "i2c.h"

extern struct HwInfo gHwInfo;

BOOL OpenI2c(void)
{
	//_T（“I2C1:”）
	gHwInfo.hI2cHandle = _I2C_Open();
	if(NULL == gHwInfo.hI2cHandle)
	{
		RETAILMSG(1, (TEXT("\r\n[HW drv] Open I2C1 Error!\r\n")));
		return FALSE;
	}

	return TRUE;
}

BOOL CloseI2c(void)
{
	if (NULL != gHwInfo.hI2cHandle)
		_I2C_Close(gHwInfo.hI2cHandle);

	return TRUE;
}

BOOL I2cWrite(i2ccfg_t *pi2c_cfg)
{
	//UINT16 i;

	SIF_CONFIG I2cConfig;

	if (NULL == gHwInfo.hI2cHandle)
	{
		RETAILMSG(1, (TEXT("\r\n[HW drv] Write I2C1 hI2cHandle is NULL!\r\n")));
		return FALSE;
	}

	if (NULL == pi2c_cfg->msg_buf)
	{
		RETAILMSG(1, (TEXT("\r\n[HW drv] Write I2C1 i2ccfg_t msg_buf is NULL!\r\n")));
		return FALSE;
	}
	
	I2cConfig.i2cMode      = I2C1_MASTER;
	I2cConfig.u4ClkDiv     = pi2c_cfg->clk_div;
	I2cConfig.ucAddrType   = pi2c_cfg->addr_type;
	I2cConfig.ucDev        = (pi2c_cfg->chip_addr&0xFF)>>1;	//从设备地址	//7位
	I2cConfig.u4Addr       = pi2c_cfg->sub_addr;	//次设备地址	
	I2cConfig.pBuffer      = pi2c_cfg->msg_buf;
	I2cConfig.transfer_len = pi2c_cfg->msg_size;

	////debug
	//RETAILMSG(1, (TEXT("\r\n[HW drv] Write I2C1 chip-addr:%X ,sub_addr:%X,addrtype:%d data:"),
	//	pi2c_cfg->chip_addr,pi2c_cfg->sub_addr,pi2c_cfg->addr_type));
	//for (i=0; i<I2cConfig.transfer_len; i++)
	//	RETAILMSG(1, (TEXT("%02X "),I2cConfig.pBuffer[i]));

	return _I2C_RanAddrWrite(gHwInfo.hI2cHandle, &I2cConfig);
}

BOOL I2cRead(i2ccfg_t *pi2c_cfg)
{
	SIF_CONFIG I2cConfig;

	if (NULL == gHwInfo.hI2cHandle)
	{
		RETAILMSG(1, (TEXT("\r\n[HW drv] Read I2C1 hI2cHandle is NULL!\r\n")));
		return FALSE;
	}

	if (NULL == pi2c_cfg->msg_buf)
	{
		RETAILMSG(1, (TEXT("\r\n[HW drv] Read I2C1 i2ccfg_t msg_buf is NULL!\r\n")));
		return FALSE;
	}

	I2cConfig.i2cMode      = I2C1_MASTER;
	I2cConfig.u4ClkDiv     = pi2c_cfg->clk_div;
	I2cConfig.ucAddrType   = pi2c_cfg->addr_type;
	I2cConfig.ucDev        = (pi2c_cfg->chip_addr&0xFF)>>1;	//从设备地址	//7位
	I2cConfig.u4Addr       = pi2c_cfg->sub_addr;	//次设备地址	
	I2cConfig.pBuffer      = pi2c_cfg->msg_buf;
	I2cConfig.transfer_len = pi2c_cfg->msg_size;


	return _I2C_RanAddrRead(gHwInfo.hI2cHandle, 
		&I2cConfig,pi2c_cfg->msg_buf,pi2c_cfg->msg_size);

	return TRUE;
}

BOOL I2cSpecialClkWrite(UINT32 uClk,BYTE *pData, UINT32 size)
{
	UINT i=0;

	if (NULL == gHwInfo.hI2cHandle)
	{
		RETAILMSG(1, (TEXT("\r\n[HW drv] I2cSpecialClkWrite hI2cHandle is NULL!\r\n")));
		return FALSE;
	}

	if (NULL == pData)
	{
		RETAILMSG(1, (TEXT("\r\n[HW drv] I2cSpecialClkWrite pData is NULL!\r\n")));
		return FALSE;
	}

	if(!_I2C_SetClock(gHwInfo.hI2cHandle, 0x1E))//时钟
	{
		RETAILMSG(1, (TEXT("\r\n[HW drv] IICWrite _I2C_SetClock err!\r\n")));
		return FALSE;
	}
	
	RETAILMSG(1, (TEXT("\r\n[HW drv] IICWrite:")));
	for (i=0; i<size; i++)
	{
		RETAILMSG(1, (TEXT("%02X "),pData[i]));
	}

	return _I2C_SegCmd(gHwInfo.hI2cHandle, (UCHAR*)pData,size);
}

BOOL I2cSpecialClkRead(UINT32 uClk, BYTE *pData, UINT32 size)
{
	if (NULL == gHwInfo.hI2cHandle)
	{
		RETAILMSG(1, (TEXT("\r\n[HW drv] I2cSpecialClkRead hI2cHandle is NULL!\r\n")));
		return FALSE;
	}

	if (NULL == pData)
	{
		RETAILMSG(1, (TEXT("\r\n[HW drv] I2cSpecialClkRead pData is NULL!\r\n")));
		return FALSE;
	}

	if(!_I2C_SetClock(gHwInfo.hI2cHandle, uClk))//时钟
	{
		RETAILMSG(1, (TEXT("\r\n[HW drv] IICRead _I2C_SetClock err!\r\n")));
		return FALSE;
	}

	return _I2C_SegCmd(gHwInfo.hI2cHandle, (UCHAR*)pData,size);
}