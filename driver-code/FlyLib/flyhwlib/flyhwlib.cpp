/*模块名称：flyhwlib.cpp
**模块功能：硬件接口操作
**
**修 改 人：黄炯辉
**修改日期：2012-09-01
**/
#include <windows.h>
#include <commctrl.h>
#include <assert.h>
#include "flyhwlib.h"

struct FlyHwLibInfo{
	HINSTANCE mhinst;
	struct HwDrvAddrTable *pHwDrvAddrTable;
};

struct FlyHwLibInfo gFlyHwLibInfo;


/*函数名称：HwGpioOutput
**函数参数：gpio：IO，value：电平值
**函数功能：IO输出
**函数返回：
**修 改 人：
**修改日期：
**/
void HwGpioOutput(UINT32 gpio, BYTE value)
{
	if(gFlyHwLibInfo.pHwDrvAddrTable == NULL)
		return;

	gpiocfg_t gpio_cfg;
	gpio_cfg.flag  = GPIO_SET;
	gpio_cfg.gpio  = gpio;
	gpio_cfg.value = value;
	gFlyHwLibInfo.pHwDrvAddrTable->HwCtlIOValue(&gpio_cfg);
}

/*函数名称：HwGpioInput
**函数参数：gpio：IO
**函数功能：IO读
**函数返回：返回电平值
**修 改 人：
**修改日期：
**/
BYTE HwGpioInput(UINT32 gpio)
{
	if(gFlyHwLibInfo.pHwDrvAddrTable == NULL)
		return 0;

	gpiocfg_t gpio_cfg;
	gpio_cfg.flag  = GPIO_GET;
	gpio_cfg.gpio  = gpio;
	gpio_cfg.value = 0;
	gFlyHwLibInfo.pHwDrvAddrTable->HwCtlIOValue(&gpio_cfg);

	return gpio_cfg.value;
}

/*函数名称：HwI2CWrite
**函数参数：
**函数功能：I2C写
**函数返回：
**修 改 人：
**修改日期：
**/
void HwI2CWrite(i2ccfg_t *pi2c_cfg)
{
	if(gFlyHwLibInfo.pHwDrvAddrTable == NULL)
		return;

	gFlyHwLibInfo.pHwDrvAddrTable->HwCtlI2cValue(pi2c_cfg);
}

/*函数名称：HwI2CRead
**函数参数：
**函数功能：I2C读
**函数返回：
**修 改 人：
**修改日期：
**/
void HwI2CRead(i2ccfg_t *pi2c_cfg)
{
	if(gFlyHwLibInfo.pHwDrvAddrTable == NULL)
		return;

	gFlyHwLibInfo.pHwDrvAddrTable->HwCtlI2cValue(pi2c_cfg);
}

BOOL HwI2CSpecialClkWrite(UINT32 uClk,BYTE *pData, UINT32 size)
{
	if(gFlyHwLibInfo.pHwDrvAddrTable == NULL)
		return FALSE;
	
	return gFlyHwLibInfo.pHwDrvAddrTable->HwCtlI2cSpecialClk(TRUE,uClk, pData, size);
}
BOOL HwI2CSpecialClkRead(UINT32 uClk,BYTE *pData, UINT32 size)
{
	if(gFlyHwLibInfo.pHwDrvAddrTable == NULL)
		return FALSE;

	return gFlyHwLibInfo.pHwDrvAddrTable->HwCtlI2cSpecialClk(FALSE,uClk,pData, size);
}

/*函数名称：HwADCGetValue
**函数参数：iChannel：ADC通道，value：ADC值
**函数功能：取得ADC的值
**函数返回：
**修 改 人：
**修改日期：
**/
void HwADCGetValue(UINT32 iChannel, UINT32 *value)
{
	if(gFlyHwLibInfo.pHwDrvAddrTable == NULL)
		return;

	adccfg_t adc_cfg;
	adc_cfg.channel = iChannel;
	adc_cfg.value   = 0;
	gFlyHwLibInfo.pHwDrvAddrTable->HwAdcGetValue(&adc_cfg);
	*value = adc_cfg.value;
}

/*函数名称：HwKeyIrqEnable
**函数参数：
**函数功能：中断使能
**函数返回：
**修 改 人：
**修改日期：
**/
void HwKeyIrqEnable(void *key_irq_rr,void *key_irq_rl,
					void *key_irq_lr,void *key_irq_ll,BOOL bEnable)
{
	if(gFlyHwLibInfo.pHwDrvAddrTable == NULL)
		return;

	gFlyHwLibInfo.pHwDrvAddrTable->HwKeyIrqEnable(key_irq_rr,
							key_irq_rl,key_irq_lr,key_irq_ll,bEnable);
}

/*函数名称：HwPwmSetFreq
**函数参数：iNum：PWM口，iDuty：占空比
**函数功能：PWM设置
**函数返回：
**修 改 人：
**修改日期：
**/
void HwPwmSetFreq(UINT16 iNum, UINT16 iDuty)
{
	gFlyHwLibInfo.pHwDrvAddrTable->HwPwmSetFreq(iNum,iDuty);
}

/*函数名称：GetDllAddrTable
**函数参数：
**函数功能：取得硬件的接口地址
**函数返回：成功时返回TURE
**修 改 人：
**修改日期：
**/
BOOL GetDllAddrTable(void)
{
	
	GetDllProcAddr GetProcAddr=NULL;

	//加载硬件DLL
	gFlyHwLibInfo.mhinst = LoadLibrary(FLY_HW_DLL_PATH);
	if (!gFlyHwLibInfo.mhinst){
		RETAILMSG(1, (TEXT("[fly hw lib] LoadLibrary hwdrv.dll err!\r\n")));
		return FALSE;
	}

	//从audio.dll的导出函数GetProgressAddr()取得硬件表的地址函数
	GetProcAddr = (GetDllProcAddr)GetProcAddress(gFlyHwLibInfo.mhinst,L"HWD_GetDllProcAddr");
	if (NULL == GetProcAddr)
	{
		RETAILMSG(1,(TEXT("[fly hw lib] Error! Failed to get hwdrv.dll Proc addr list.\r\n")));
		return	FALSE;
	}

	//调用导出函数，取得地址
	gFlyHwLibInfo.pHwDrvAddrTable = (struct HwDrvAddrTable *)GetProcAddr();
	if (NULL == gFlyHwLibInfo.pHwDrvAddrTable)
	{
		RETAILMSG(1,(TEXT("[fly hw lib] Error! Failed to get hwdrv.ll addr table.\r\n")));
		return	FALSE;
	}

	//RETAILMSG(1,(TEXT("[fly hw lib] pHwDrvAddrTable:%p.\r\n"),gFlyHwLibInfo.pHwDrvAddrTable));

	return TRUE;
}