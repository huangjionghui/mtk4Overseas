#ifndef __FLY_HW_LIB_H__
#define __FLY_HW_LIB_H__

#include <windef.h> 
#include <windows.h>
#include "flyglobaldef.h"
#include "hwapi.h"

typedef struct HwDrvAddrTable* (*GetDllProcAddr)(void);
#define  FLY_HW_DLL_PATH _T("FlyHwdrv.dll")

//GPIO
void HwGpioOutput(UINT32 gpio, BYTE value);
BYTE HwGpioInput(UINT32 gpio);

//I2C
void HwI2CWrite(i2ccfg_t *pi2c_cfg);
void HwI2CRead(i2ccfg_t *pi2c_cfg);
BOOL HwI2CSpecialClkWrite(UINT32 uClk,BYTE *pData, UINT32 size);
BOOL HwI2CSpecialClkRead(UINT32 uClk,BYTE *pData, UINT32 size);

//ADC
void HwADCGetValue(UINT32 gpio, UINT32 *value);

//key
void HwKeyIrqEnable(void *key_irq_rr,void *key_irq_rl,
			void *key_irq_lr,void *key_irq_ll,BOOL bEnable);

//pwm
void HwPwmSetFreq(UINT16 iNum,UINT16 iDuty);



BOOL GetDllAddrTable(void);

#endif