#include <windows.h>
#include <winuser.h>
#include <winuserm.h>
#include <ceddk.h>
#include <ceddkex.h>
#include <initguid.h>

#include "x_ckgen_3360.h"
#include "Gpio.h"
#include <pwm.h>
#include "FlyHwdrv.h"

#define BL_CTL_MODE_3 1
#define BKL_MAX_INTENSITY           (100)

void PWM_ConfigGpio()
{
	//// use GPIO125
	//PWM3SetPin(1);  
	//CKGEN_AgtOnClk(e_CLK_PWM3);
	//PWM_Select_Clock(0, CLKSRC_418M);

	//// use GPIO124  ---  LCD idle
	//PWM2SetPin(2);  
	//CKGEN_AgtOnClk(e_CLK_PWM2);
	//PWM_Select_Clock(0, CLKSRC_418M);

	//// use GPIO29  ----- FAN PWM
	//PWM4SetPin(2);  
	//CKGEN_AgtOnClk(e_CLK_PWM4);
	//PWM_Select_Clock(0, CLKSRC_418M);
}

BOOL GetConfigForPWM(PWM_config_code *pPwm, PWM_SIMP_config backlight)
{		
	BOOL rc;

	pPwm->uId = backlight.uId;
	pPwm->bPwmEn = backlight.bPwmEn;
	pPwm->bMode = SYNC_TRI_MODE;

	if((backlight.u4Freq > 41800000)||(backlight.u4Freq == 0)||(backlight.uId >= PWM_NUM_MAX))
	{
		RETAILMSG(1,(L"[BKL] freqency may too large, not support!!! \r\n"));
		rc = FALSE;
	}
	else if(backlight.u4Freq > 10 * 1000)
	{
		PWM_Select_Clock(backlight.uId, CLKSRC_418M);
		pPwm->u4PwmP = 418000000/(backlight.u4Freq * 4096);
		pPwm->u4PwmRSN = 418000000/(backlight.u4Freq * (pPwm->u4PwmP + 1)) -1;
		pPwm->u4PwmH = ((pPwm->u4PwmRSN + 1) * backlight.u4Duty / 100);
		if(pPwm->u4PwmH >= 1) 
			pPwm->u4PwmH -= 1;
		RETAILMSG(1,(TEXT("[BKL] pwm%d clk src to 418MHz... \r\n"),pPwm->uId));
		rc = TRUE;
	}
	else
	{
		PWM_Select_Clock(backlight.uId, CLKSRC_27M);
		pPwm->u4PwmP = 27000000/(backlight.u4Freq * 4096);
		pPwm->u4PwmRSN = 27000000/(backlight.u4Freq * (pPwm->u4PwmP + 1)) -1;
		pPwm->u4PwmH = ((pPwm->u4PwmRSN +1) * backlight.u4Duty / 100);
		if(pPwm->u4PwmH >= 1) 
			pPwm->u4PwmH -= 1;
		RETAILMSG(1,(TEXT("[BKL] pwm%d sel clk src to 27MHz... \r\n"),pPwm->uId));
		rc = TRUE;
	}

	return rc;

}

static void PwmLcdIdelHight(void)
{
	//demo board
	PWM2SetPin(2);  // use GPIO124
	CKGEN_AgtOnClk(e_CLK_PWM2);
	CKGEN_AgtOnClk(e_CLK_PWM3);
	PWM_Select_Clock(2, CLKSRC_418M);
	i4PWM_Set(2, TRUE, 0xd9a, 0xd9a,2, SYNC_TRI_MODE); 
}

void PWM_SetFreq(UINT16 iNum, UINT16 iDuty)
{
	//for EVB board
	UINT32 power_PwmIndex = PWM_NUM_MAX; 
	UINT32 power_frequency = 20 * 1000; 
	UINT32 fb_PwmIndex = PWM_NUM_MAX;  
	UINT32 frequency_fb = 40 * 1000;  

	PWM_SIMP_config backlight_power;
	PWM_SIMP_config backlight_fb;
	PWM_config_code pwm_power;
	PWM_config_code pwm_fb;

	UINT32 iHigh  = 0;
	UINT32 iTotal = 0;
	UINT32 iPreDiv = 0;
	UINT32 iFreq = 0;

	if (PWM_NUM_2 == iNum)
	{
#if FLYMTK_BOARD_V1
		// use GPIO124  ---  LCD idle
		PWM2SetPin(2);  
		CKGEN_AgtOnClk(e_CLK_PWM2);
		PWM_Select_Clock(2, CLKSRC_418M);

		//for Demo board
		power_PwmIndex = PWM_NUM_2; //pwm2
		power_frequency = 20 * 1000; // 20K HZ for power
#else
		return;
#endif
	}
	else if (PWM_NUM_3 == iNum)//LCD_PWM
	{

		PWM3SetPin(1);  
		CKGEN_AgtOnClk(e_CLK_PWM3);
		PWM_Select_Clock(3, CLKSRC_418M);

		//for Demo board
		power_PwmIndex = PWM_NUM_3; //pwm2
		power_frequency = 30 * 1000; // 20K HZ for power

		//for Demo board
		fb_PwmIndex = PWM_NUM_3;  // pwm3
		frequency_fb = 30 * 1000;  // 20K HZ for fb
	}
	else if (PWM_NUM_4 == iNum)
	{
		//iDuty = (high + 1) / (total + 1) * 100;
		PWM4SetPin(2);  // use GPIO29
		CKGEN_AgtOnClk(e_CLK_PWM4);
		PWM_Select_Clock(4, CLKSRC_418M);

		if (iDuty != 0)
			iHigh = ((iDuty*(0xd9a+1))/100) - 1;
		else
			iHigh = iDuty;

		RETAILMSG(1,(TEXT("\r\n[fly drv] fan pwm high: %d.\r\n"),iHigh));
		i4PWM_Set(4, TRUE, 0xd9a, iHigh, 4, SYNC_TRI_MODE);//20K
		return;
	}
	else if (PWM_NUM_0 == iNum)//KEY_PWM
	{
		PWM0SetPin(2); // use GPIO169
		CKGEN_AgtOnClk(e_CLK_PWM0);
		PWM_Select_Clock(0, CLKSRC_418M);

		//135K hz
		power_PwmIndex = PWM_NUM_0; //pwm2
		power_frequency = 30 * 1000; // 70K HZ for power

		fb_PwmIndex = PWM_NUM_0;  // pwm4
		frequency_fb = 30 * 1000;  // 70K HZ for fb
	}	
	//backlight power config
	backlight_power.uId = power_PwmIndex;
	backlight_power.bPwmEn = TRUE;
	backlight_power.u4Freq = power_frequency;
	backlight_power.u4Duty = iDuty;
	if (PWM_NUM_0 == iNum)
		backlight_power.u4Duty = iDuty * 45 /BKL_MAX_INTENSITY; //mux Duty is 45%;



	//backlight fb config
	backlight_fb.uId = fb_PwmIndex;
	backlight_fb.bPwmEn = TRUE;
	backlight_fb.u4Freq = frequency_fb;
	backlight_fb.u4Duty = BKL_MAX_INTENSITY - iDuty;
	//if (PWM_NUM_0 == iNum)
		//backlight_fb.u4Duty = 50;//pwm4 must be 50%;


	if(power_PwmIndex >= PWM_NUM_MAX)
		return ;

	//backlight config ---->>> pwm config
	GetConfigForPWM(&pwm_power, backlight_power); //set clock source in this function
	GetConfigForPWM(&pwm_fb, backlight_fb);

	i4PWM_Set(pwm_power.uId, pwm_power.bPwmEn, 
		pwm_power.u4PwmRSN, pwm_power.u4PwmH, pwm_power.u4PwmP, SYNC_TRI_MODE);	
	
	i4PWM_Set(pwm_fb.uId, pwm_fb.bPwmEn, 
			pwm_fb.u4PwmRSN,pwm_fb.u4PwmH, pwm_fb.u4PwmP, SYNC_TRI_MODE);	


#if FLYMTK_BOARD_V1
	//改了LCD_PWM，LCD的IDLE脚也会有影响，MTK的bug
	if (PWM_NUM_3 == iNum || PWM_NUM_2 == iNum)//LCD_PWM
	{
		if (iDuty != 0)
			PwmLcdIdelHight();
	}
#endif

	//
	//RETAILMSG(1,(TEXT("[BKL]pwm%d, en:%d, RSN:%d, pwmH:%d, pwmP:%d, mode:%d iduty:%d.\r\n"),
	//	pwm_power.uId,
	//	pwm_power.bPwmEn,
	//	pwm_power.u4PwmRSN,
	//	pwm_power.u4PwmH,
	//	pwm_power.u4PwmP,
	//	pwm_power.bMode,iDuty));
	//RETAILMSG(1,(TEXT("[BKL]pwm%d, en:%d, RSN:%d, pwmH:%d, pwmP:%d, mode:%d.duty:%d.\r\n"),
	//	pwm_fb.uId,
	//	pwm_fb.bPwmEn,
	//	pwm_fb.u4PwmRSN,
	//	pwm_fb.u4PwmH,
	//	pwm_fb.u4PwmP,
	//	pwm_fb.bMode,iDuty));
}

//void SetPwmModel()
//{
//	//PWM0SetPin(2);
//	//CKGEN_AgtOnClk(e_CLK_PWM0);
//	//PWM_Select_Clock(0, CLKSRC_418M);
//	//70K hz
//	i4PWM_Set(0, TRUE, TRUE, 0xd9a/*total*/, 0x4e4/*high*/, 4 /*pre_div*/, SYNC_TRI_MODE); 
//	//total,high: max 12bit;
//	//pre_div: max 6bit(64);
//	//frequency = Clock_Src /(pre_div + 1) /(total + 1) ;
//	//iDuty = (high + 1) / (total + 1) * 100;
//}
