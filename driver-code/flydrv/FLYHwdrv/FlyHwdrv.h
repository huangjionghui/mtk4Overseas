#ifndef __HW_DRV_H__
#define __HW_DRV_H__

#include "hwapi.h"
#include "flygpio.h"

struct HwInfo{
	BOOL bInit;
	HANDLE hI2cHandle;
	CRITICAL_SECTION CriticalSectionHwGpio;
	CRITICAL_SECTION CriticalSectionHwI2c;
	struct HwDrvAddrTable sHwAddrTable;
	void *pShareMmapAddr;
};

//gpio
SetGpioModel(void);
void SetGpio54Status(BOOL bHigh);
void SetGpio55Status(BOOL bHigh);
void SetGpio57Status(BOOL bHigh);
UINT32 GetGpio58Status(INT32 gpio);

//I2C
BOOL OpenI2c(void);
BOOL CloseI2c(void);
BOOL I2cWrite(i2ccfg_t *pi2c_cfg);
BOOL I2cRead(i2ccfg_t *pi2c_cfg);
BOOL I2cSpecialClkWrite(UINT32 uClk,BYTE *pData, UINT32 size);
BOOL I2cSpecialClkRead(UINT32 uClk,BYTE *pData, UINT32 size);

//ADC
UINT16 AdcGetValue(UINT16 iChannel);
void HwInitGpioADCModule(void);

//key IRQ
void EnableKeyIrq(void *key_irq_rr,
	 void *key_irq_rl,void *key_irq_lr,void *key_irq_ll,BOOL bEnable);


//PWM
void PWM_ConfigGpio();
void PWM_SetFreq(UINT16 iNum, UINT16 iDuty);

#endif
