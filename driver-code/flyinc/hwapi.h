/*fly硬件的接口函数表，方便操作硬件
 *
 *create:2012-07-06
 *by :hui
 *函数的实现在：C:\WINCE600\PLATFORM\MT3360\SRC\common\flyhwcomm\flyhwcomm.c
 *****************************************************************************
 *modif:
 *
 *
 *
 */
#ifndef __HW_API_H__
#define __HW_API_H__

#include <windef.h> 
#include <windows.h>
#include "flyglobaldef.h"

/*gpio管脚组
 *flag ： 设置还是取管脚电平
 *group： 组
 *gpio ： 脚
 *value： 值 
 */
typedef struct{
#define GPIO_GET     1
#define GPIO_SET     0
#define GPIO_HIGHT   1
#define GPIO_LOW     0
#define GPIO_IRQ_EN  1
#define GPIO_IRQ_DIS 0
	char flag;
	char gpio;
	char value;
	char iqr_flag;
}gpiocfg_t;


typedef struct{
	unsigned int channel;
	unsigned int freq;
	unsigned int duty;	
}pwmcfg_t;

typedef struct{
	unsigned int channel;
	unsigned int value;
}adccfg_t;


//irq struct
typedef void (*irq_handler_t)(void);

typedef struct{
#define IRQ_ENBALE     1
#define IRQ_DISABLE    0
#define IRQ_DELETE_EN  1
#define IRQ_DELETE_DIS 0
	char irq_flag;
	char enable_flag;
	char delete_flag;
	gpiocfg_t irq_gpio;
	unsigned int irq_num;
	irq_handler_t irq_handler;		
}irqcfg_t;


//flag
#define I2C_READ  0
#define I2C_WRITE 1

typedef enum{
	IIC_0_BIT,
	IIC_8_BIT,
	IIC_16_BIT,
	IIC_24_BIT,
	IIC_32_BIT
}IIC_SUB_ADDR_BIT;

typedef enum
{
	IIC_SEG_MASK_SEND_START		= 1 << 0,
	IIC_SEG_MASK_NEED_ACK		= 1 << 1,
	IIC_SEG_SEQ_DEVADDR_W		= 1 << 2,
	IIC_SEG_SEQ_DEVADDR_R		= 1 << 3,
	IIC_SEG_SEQ_REGADDR			= 1 << 4,
	IIC_SEG_SEQ_DATA			= 1 << 5,
	IIC_SEG_SEQ_MASK_SEND_STOP  = 1 << 6,
	IIC_SEG_MASK_NEED_NACK      = 1 << 7,
}IIC_SEG_CLK;

typedef struct{
	BYTE uSeg;
	BYTE uData;
}IIC_SEG_T;

typedef struct{
	char bus_id;
	unsigned int chip_addr;
	unsigned int sub_addr;
	char addr_type;
	UINT32 clk_div;
	char flag;
	char *msg_buf;
	unsigned int msg_size;
}i2ccfg_t;

#define IOCTL_HW_GPIO  1102
#define IOCTL_HW_I2C   1103


/*
//GPIO 定义
#define GPIO_OUT_AF_MUTE  54
#define GPIO_IN_SSTOP     58

#define GPIO_OUT_ANT1     55
#define GPIO_OUT_LCD_REST 57

*/

typedef void (*HwCtlIOValue_t)(gpiocfg_t *pgpio_cfg);
typedef void (*HwCtlI2cValue_t)(i2ccfg_t *pi2c_cfg);
typedef BOOL (*HwCtlI2cSpecialClk_t)(BYTE bWrite, UINT32 uClk,BYTE *pData, UINT32 size);
typedef void (*HwAdcGetValue_t)(adccfg_t *padc_cfg);
typedef void (*HwKeyIrqEnable_t)(void *key_irq_rr,
		void *key_irq_rl,void *key_irq_lr,void *key_irq_ll,BOOL bEnable);
typedef void (*HwPwmSetFreq_t)(UINT16 iNum,UINT16 iDuty);


struct HwDrvAddrTable{
	HwCtlIOValue_t		 HwCtlIOValue;
	HwCtlI2cValue_t		 HwCtlI2cValue;
	HwCtlI2cSpecialClk_t HwCtlI2cSpecialClk;
	HwAdcGetValue_t		 HwAdcGetValue;
	HwKeyIrqEnable_t     HwKeyIrqEnable;
	HwPwmSetFreq_t       HwPwmSetFreq;
};

#endif