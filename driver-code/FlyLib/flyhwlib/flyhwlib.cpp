/*ģ�����ƣ�flyhwlib.cpp
**ģ�鹦�ܣ�Ӳ���ӿڲ���
**
**�� �� �ˣ��ƾ���
**�޸����ڣ�2012-09-01
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


/*�������ƣ�HwGpioOutput
**����������gpio��IO��value����ƽֵ
**�������ܣ�IO���
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
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

/*�������ƣ�HwGpioInput
**����������gpio��IO
**�������ܣ�IO��
**�������أ����ص�ƽֵ
**�� �� �ˣ�
**�޸����ڣ�
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

/*�������ƣ�HwI2CWrite
**����������
**�������ܣ�I2Cд
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
**/
void HwI2CWrite(i2ccfg_t *pi2c_cfg)
{
	if(gFlyHwLibInfo.pHwDrvAddrTable == NULL)
		return;

	gFlyHwLibInfo.pHwDrvAddrTable->HwCtlI2cValue(pi2c_cfg);
}

/*�������ƣ�HwI2CRead
**����������
**�������ܣ�I2C��
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
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

/*�������ƣ�HwADCGetValue
**����������iChannel��ADCͨ����value��ADCֵ
**�������ܣ�ȡ��ADC��ֵ
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
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

/*�������ƣ�HwKeyIrqEnable
**����������
**�������ܣ��ж�ʹ��
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
**/
void HwKeyIrqEnable(void *key_irq_rr,void *key_irq_rl,
					void *key_irq_lr,void *key_irq_ll,BOOL bEnable)
{
	if(gFlyHwLibInfo.pHwDrvAddrTable == NULL)
		return;

	gFlyHwLibInfo.pHwDrvAddrTable->HwKeyIrqEnable(key_irq_rr,
							key_irq_rl,key_irq_lr,key_irq_ll,bEnable);
}

/*�������ƣ�HwPwmSetFreq
**����������iNum��PWM�ڣ�iDuty��ռ�ձ�
**�������ܣ�PWM����
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
**/
void HwPwmSetFreq(UINT16 iNum, UINT16 iDuty)
{
	gFlyHwLibInfo.pHwDrvAddrTable->HwPwmSetFreq(iNum,iDuty);
}

/*�������ƣ�GetDllAddrTable
**����������
**�������ܣ�ȡ��Ӳ���Ľӿڵ�ַ
**�������أ��ɹ�ʱ����TURE
**�� �� �ˣ�
**�޸����ڣ�
**/
BOOL GetDllAddrTable(void)
{
	
	GetDllProcAddr GetProcAddr=NULL;

	//����Ӳ��DLL
	gFlyHwLibInfo.mhinst = LoadLibrary(FLY_HW_DLL_PATH);
	if (!gFlyHwLibInfo.mhinst){
		RETAILMSG(1, (TEXT("[fly hw lib] LoadLibrary hwdrv.dll err!\r\n")));
		return FALSE;
	}

	//��audio.dll�ĵ�������GetProgressAddr()ȡ��Ӳ����ĵ�ַ����
	GetProcAddr = (GetDllProcAddr)GetProcAddress(gFlyHwLibInfo.mhinst,L"HWD_GetDllProcAddr");
	if (NULL == GetProcAddr)
	{
		RETAILMSG(1,(TEXT("[fly hw lib] Error! Failed to get hwdrv.dll Proc addr list.\r\n")));
		return	FALSE;
	}

	//���õ���������ȡ�õ�ַ
	gFlyHwLibInfo.pHwDrvAddrTable = (struct HwDrvAddrTable *)GetProcAddr();
	if (NULL == gFlyHwLibInfo.pHwDrvAddrTable)
	{
		RETAILMSG(1,(TEXT("[fly hw lib] Error! Failed to get hwdrv.ll addr table.\r\n")));
		return	FALSE;
	}

	//RETAILMSG(1,(TEXT("[fly hw lib] pHwDrvAddrTable:%p.\r\n"),gFlyHwLibInfo.pHwDrvAddrTable));

	return TRUE;
}