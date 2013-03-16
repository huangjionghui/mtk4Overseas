#ifndef __FLY_GPIO_DEF_H__
#define __FLY_GPIO_DEF_H__


#define  MTK_BOARD 1
#define  A4_PLAT 0


#include "flyglobaldef.h"

#define  FLYMTK_BOARD_V1 0	//第一版程序

#define  FLYMTK_PANEL_V2 0  //新的07面板



//设为0，无效
#if MTK_BOARD
//key
#define CONTROL_IO_LED_I	     	 0		//9V LED LPC控
#define CONTROL_IO_LCDIDLE_I		124     //LCD 使能脚

#define PANEL_ADC_CHANNEL1			0        
#define PANEL_ADC_CHANNEL2			1
#define PANEL_ADC_CHANNEL3			4     
//#define STEEL_ADC_CHANNEL1			2       //REMO2
//#define STEEL_ADC_CHANNEL2			3       //REMO1
#define STEEL_ADC_CHANNEL1			3       //REMO1
#define STEEL_ADC_CHANNEL2			2       //REMO2
#define RADIO_ADC_CHANNEL5			5		//radio AD

#define ENCODER_L1_I	0
#define ENCODER_L2_I	0
#define ENCODER_R1_I    0
#define ENCODER_R2_I	0

//5150		
#define IO_4052VIDEOSWITCH_I		0

//7419
#define IO_4052_A_I					0
#define IO_4052_B_I					0
#define IO_7386_MUTE_I				0

//7541
#define IO_TDA7541_SSTOP_I		58
#define IO_AFMUTE_I				54
#define IO_ANT1_I				55	
#define IO_ANT2_I				0

#define IO_OUT_LCD_REST         57


//CarBody
#define IIC_ISR_PIN_I	0

//bt
#define CONTROL_IO_BTPOWER_I	0
#define CONTROL_IO_BTRESET_I	0
#define CONTROL_IO_BTCE_I		0

//DVD
#define CONTROL_IO_DVDRESET_I	0

//system
#define BREAK_DETECT_I	0
#define PHONE_DETECT_I	0
#define SPEED_CHECK_I	0

#define CONTROL_IO_FAN_I	0
#define CONTROL_IO_PANELPEN_I	0



//PWM
enum
{
	PWM_NUM_0 = 0,
	PWM_NUM_1,
	PWM_NUM_2,
	PWM_NUM_3,
	PWM_NUM_4,
	PWM_NUM_5,	
	PWM_NUM_MAX,
};


#define LCD_IDLE PWM_NUM_2
#define LCD_PWM  PWM_NUM_3
#define KEY_PWM  PWM_NUM_0

#if FLYMTK_BOARD_V1
#define FAN_PWM  PWM_NUM_4
#define FAN_ON   100
#define FAN_OFF  0
#define FAN_PWN_DUTY 80
#else
#define FAN_PWM  PWM_NUM_0
#define FAN_ON   0
#define FAN_OFF  100
#define FAN_PWN_DUTY (100-80)
#endif

#define LCD_IDLE_ON 100
#define LCD_IDLE_OFF 0

#define LCD_ON    100
#define LCD_OFF   0

#define KEY_ON   0
#define KEY_OFF  100



#endif


#endif 