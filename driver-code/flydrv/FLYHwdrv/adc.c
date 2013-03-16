/*模块名称：adc.c
**模块功能:adc操作
**
**修 改 人：黄炯辉
**修改日期：2012-09-01
**/
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

DWORD gIntrAdcKey	= 0;
UINT32 g_u4EnableCalibrationLog = 0;
UINT32 g_u4EnableRtouchArch= 0;
UINT32 g_u4TouchResistorThreshold= 0;
INT32 g_i4TouchResistorOffset= 0;
INT16 g_u2ErrSampleCnt= 0;
UINT16 g_u2HighSampleRateFlag= 0;
UINT16 g_u2HWGPIO0= 0;
UINT16 g_u2HWGPIO1= 0;
UINT16 g_u2HWGPIODelay= 0;
UINT16 g_u2ADCSPL= 0;
UINT16 g_u2ADCPULLUP = 0;

UINT16 uAdcOldValue[5]={255,255,255,1024,1024};

#define ADC_SAMPLING_COUNTS  5

typedef enum 
{
	KNOB1_LEFT = 0x74,
	KNOB1_RIGHT,
	KNOB2_LEFT ,
	KNOB2_RIGHT,
	kEY_S1 = 0x78,
	kEY_S2, 
	kEY_S3, 
	kEY_S4,

} KeyValue;


#define KEYNUM   4

DWORD GET_KEY(DWORD channel, DWORD sample_value)
{
	DWORD key_value;
	DWORD offset = 0;

	offset = channel * (KEYNUM);

	if(sample_value < 0x100)
	{
		key_value = kEY_S1 + offset;
	}
	else if(sample_value < 0x4d3)
	{
		key_value = kEY_S2 + offset;
	}	
	else if(sample_value < 0x8b8)
	{
		key_value = kEY_S3 + offset;

	}
	else if(sample_value < 0xc00)
	{
		key_value = kEY_S4 + offset;
	}
	else
	{
		key_value = 0xfff;
	}

	return key_value;
}

UINT16 SelectAdcChannel(UINT16 uChannel)
{
	UINT16 temp=0xFFF;

	switch (uChannel)
	{
	case 0:
		temp = AUXADC_NORMAL_CHL2;
		break;
	case 1:
		temp = AUXADC_NORMAL_CHL1;
		break;
	case 2:
		temp = AUXADC_NORMAL_CHL3;
		break;

	case 3:
		temp = AUXADC_NORMAL_CHL4;
		break;
	case 4:
		temp = AUXADC_NORMAL_CHL0;
		break;

	case 5://收音机的AD
		temp = AUXADC_NORMAL_CHL5;
		break;

	default:
		break;
	}

	return temp;
}

UINT16 AdcGetValue(UINT16 iChannel)
{
	UINT16 uReadCounts = 0;
	UINT16 u4DataOrg   = 0;
	UINT16 uChannel    = 0;
	UINT32 uTempValue  = 0;

	uChannel = SelectAdcChannel(iChannel);
	if (uChannel == 0xFFF)
	{
		u4DataOrg = 0xFFF;
		goto Finish;
	}

	//开始采样
	do
	{
		uReadCounts++;
		Sleep(5);
		u4DataOrg = AuxGetKeyPadDat(uChannel);
		if (uReadCounts > 5)
		{
			u4DataOrg = 0xFFF;
			RETAILMSG(1, (TEXT("\r\n[HW drv] HWD ADC check is Error:channel:%d!!!\r\n"),iChannel));
			break;
		}
	}while((u4DataOrg & (1<<12)) == 0);
	u4DataOrg &= 0xFFF;

Finish:
	if (iChannel == STEEL_ADC_CHANNEL2 
		|| iChannel == STEEL_ADC_CHANNEL1)
	{
		if (u4DataOrg == 3836)//MTK 3836对应0V
			u4DataOrg = 0x0;

		if (u4DataOrg > (970<<2))
			u4DataOrg = 0xFFF;
	}

	//AD0~2  取值为0~255    AD3~4取值为0~1023
	return (u4DataOrg>>2);
}
void HwInitGpioADCModule(void)
{
	int ret;
	ret = AuxADCInitKeypad();
}