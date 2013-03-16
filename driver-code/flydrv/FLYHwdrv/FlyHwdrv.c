/*模块名称：FlyHwdrv.c
**模块功能：硬件操作接口汇总
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
#include <assert.h>

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


struct HwInfo gHwInfo;


/*函数名称：HwCtlIOValue
**函数参数：
**函数功能：GPIO读写操作汇总
**函数返回：
**修 改 人：
**修改日期：
*/
static void HwCtlIOValue(gpiocfg_t *pgpio_cfg)
{
	//访问硬件资源，加锁
	EnterCriticalSection(&gHwInfo.CriticalSectionHwGpio);

	switch (pgpio_cfg->gpio)
	{
	case IO_AFMUTE_I:
		 SetGpio54Status(pgpio_cfg->value);
		break;

	case IO_ANT1_I:
		 SetGpio55Status(pgpio_cfg->value);
		break;

	case IO_OUT_LCD_REST:
		 SetGpio57Status(pgpio_cfg->value);
		break;

	case IO_TDA7541_SSTOP_I:
		pgpio_cfg->value = GetGpio58Status(pgpio_cfg->gpio);
		break;

	default:
		LeaveCriticalSection(&gHwInfo.CriticalSectionHwGpio);
		return;
		break;
	}

	//RETAILMSG(1, (TEXT("[HW drv] HWD IO Ctl gpio:%d,Ctl:%d,Value:%d\r\n"),
	//	pgpio_cfg->gpio,pgpio_cfg->flag,pgpio_cfg->value));

	LeaveCriticalSection(&gHwInfo.CriticalSectionHwGpio);
}

/*函数名称：HwCtlI2cValue
**函数参数：
**函数功能：I2C读写操作汇总
**函数返回：
**修 改 人：
**修改日期：
*/
static void HwCtlI2cValue(i2ccfg_t *pi2c_cfg)
{

	EnterCriticalSection(&gHwInfo.CriticalSectionHwI2c);

	switch (pi2c_cfg->flag)
	{
	case I2C_WRITE:
		I2cWrite(pi2c_cfg);
		break;

	case I2C_READ:
		I2cRead(pi2c_cfg);
		break;

	default:
		break;
	}

	LeaveCriticalSection(&gHwInfo.CriticalSectionHwI2c);
}

/*函数名称：HwCtlI2cValue
**函数参数：
**函数功能：I2C读写操作汇总
**函数返回：
**修 改 人：
**修改日期：
*/
static BOOL HwCtlI2cSpecialClk(BYTE bWrite,UINT32 uClk,BYTE *pData, UINT32 size)
{
	BOOL bRet = FALSE;

	EnterCriticalSection(&gHwInfo.CriticalSectionHwI2c);
	if (bWrite)
		bRet = I2cSpecialClkWrite(uClk,pData, size);
	else
		bRet = I2cSpecialClkRead(uClk,pData, size);

	LeaveCriticalSection(&gHwInfo.CriticalSectionHwI2c);
	
	return bRet;
}

/*函数名称：HwAdcGetValue
**函数参数：
**函数功能：ADC读取汇总
**函数返回：
**修 改 人：
**修改日期：
*/
static void HwAdcGetValue(adccfg_t *padc_cfg)
{
	padc_cfg->value = AdcGetValue(padc_cfg->channel);
}

/*函数名称：HwKeyIrqEnable
**函数参数：
**函数功能：中断操作汇总
**函数返回：
**修 改 人：
**修改日期：
*/
static void HwKeyIrqEnable(void *key_irq_rr,
		void *key_irq_rl,void *key_irq_lr,void *key_irq_ll,BOOL bEnable)
{
	EnableKeyIrq(key_irq_rr,key_irq_rl,key_irq_lr,key_irq_ll,bEnable);
}

/*函数名称：HwPwmSetFreq
**函数参数：
**函数功能：PWM操作汇总
**函数返回：
**修 改 人：
**修改日期：
*/
static void HwPwmSetFreq(UINT16 iNum,UINT16 iDuty)
{
	PWM_SetFreq(iNum, iDuty);
}

/*函数名称：HwCreateShareMmap
**函数参数：
**函数功能：创建共享内存
**函数返回：
**修 改 人：
**修改日期：
*/
static void *HwCreateShareMmap(UINT32 iSize)
{
	if (gHwInfo.pShareMmapAddr != NULL)
		return gHwInfo.pShareMmapAddr;

	gHwInfo.pShareMmapAddr =  (void*)LocalAlloc(LPTR, iSize);
	RETAILMSG(1, (TEXT("[HW drv] HW Info share mmap addr:%p\r\n"),gHwInfo.pShareMmapAddr));

	return gHwInfo.pShareMmapAddr;
}

/*函数名称：HWD_Init
**函数参数：dwContext：字符串，指向注册表中记录活动驱动程序的键
**函数功能：加载设备，在设备被加载的时候调用
**函数返回：
**修 改 人：
**修改日期：
*/
DWORD HWD_Init(DWORD dwContext)
{
	memset(&gHwInfo,0,sizeof(struct HwInfo));

	//打开I2C
	if (!OpenI2c())
	{
		RETAILMSG(1, (TEXT("[HW drv] HWD I2C Open ERROR!\r\n")));
		return 0;
	}

	InitializeCriticalSection(&gHwInfo.CriticalSectionHwGpio);
	InitializeCriticalSection(&gHwInfo.CriticalSectionHwI2c);
	SetGpioModel();
	HwInitGpioADCModule();


#if FLYMTK_BOARD_V1
	PWM_SetFreq(KEY_PWM, KEY_OFF);
#else
	PWM_SetFreq(FAN_PWM, FAN_OFF);
#endif
	
	gHwInfo.pShareMmapAddr = NULL;
	gHwInfo.bInit = TRUE;

	RETAILMSG(1,(TEXT("\r\n[HW drv] HWD init Build\r\n")));
	return 1;
}

/*函数名称：HWD_Deinit
**函数参数：XXX_Init()函数返回的设备上下文
**函数功能：释放设备，在设备被卸载的时候调用
**函数返回：返回设备卸载是否成功
**修 改 人：
**修改日期：
*/
BOOL HWD_Deinit(DWORD hDeviceContext)
{
	if (!CloseI2c())
	{
		RETAILMSG(1, (TEXT("[HW drv] HWD I2C Close ERROR!\r\n")));
		return FALSE;
	}

	DeleteCriticalSection(&gHwInfo.CriticalSectionHwGpio);
	DeleteCriticalSection(&gHwInfo.CriticalSectionHwI2c);
	RETAILMSG(1, (TEXT("[HW drv] HWD LocalFree!\r\n")));
	return TRUE;
}

/*函数名称：RegisterApiTable
**函数参数：
**函数功能：给各个函数付上地址
**函数返回：返回地址列表
**修 改 人：
**修改日期：
*/
static struct HwDrvAddrTable *RegisterApiTable(struct HwInfo *pHwInfo)
{
	assert(NULL != pHwInfo);

	pHwInfo->sHwAddrTable.HwCtlIOValue		 = HwCtlIOValue;
	pHwInfo->sHwAddrTable.HwCtlI2cValue		 = HwCtlI2cValue;
	pHwInfo->sHwAddrTable.HwCtlI2cSpecialClk = HwCtlI2cSpecialClk;
	pHwInfo->sHwAddrTable.HwAdcGetValue      = HwAdcGetValue;
	pHwInfo->sHwAddrTable.HwKeyIrqEnable     = HwKeyIrqEnable;
	pHwInfo->sHwAddrTable.HwPwmSetFreq       = HwPwmSetFreq;

	//RETAILMSG(1,(TEXT("DLL pHwInfo->sHwAddrTable:%p.\r\n"),pHwInfo->sHwAddrTable));
	
	return &pHwInfo->sHwAddrTable;
}

/*函数名称：HWD_GetDllProcAddr
**函数参数：
**函数功能：取得硬件操作的地址
**函数返回：返回地址列表
**修 改 人：
**修改日期：
*/
struct HwDrvAddrTable *HWD_GetDllProcAddr(void)
{
	return RegisterApiTable(&gHwInfo);
}

/*函数名称：HWD_CreateShareMmap
**函数参数：
**函数功能：取得共享内存的地址
**函数返回：返回地址列表
**修 改 人：
**修改日期：
*/
void *HWD_CreateShareMmap(UINT32 iSize)
{
	return HwCreateShareMmap(iSize); 
}

/*函数名称：HWD_Open
**函数参数：DWORD hDeviceContext,	 设备上下文，由XXX_Init()函数创建
			DWORD AccessCode,		 设备的访问模式，从CreateFile()函数传入
			DWORD ShareMode)	     设备的共享模式，从CreateFile()函数传入
**函数功能：打开设备进行读写 与应用程序的CreateFile()函数对应
**函数返回：返回设备的打开上下文
**修 改 人：
**修改日期：
*/
DWORD HWD_Open(DWORD hDeviceContext, DWORD AccessCode,DWORD ShareMode)	
{
	RETAILMSG(1, (TEXT("[HW drv] HWD Open ok\r\n")));
	return hDeviceContext;
}

/*函数名称：HWD_Close
**函数参数：hDeviceContext 设备的打开上下文，由XXX_Open（）函数返回 
**函数功能：关闭设备 与应用程序的CloseHandle()函数对应
**函数返回：返回设备关闭是否成功
**修 改 人：
**修改日期：
*/
BOOL HWD_Close(DWORD hDeviceContext) 
{
	RETAILMSG(1, (TEXT("[HW drv] HWD Close!\r\n")));
	return TRUE;
}

/*函数名称：HWD_PowerUp
**函数参数：hDeviceContext 设备的打开上下文，由XXX_Open（）函数返回 
**函数功能：电源挂起
**函数返回：
**修 改 人：
**修改日期：
*/
VOID HWD_PowerUp(DWORD hDeviceContext)
{
	RETAILMSG(1, (TEXT("[HW drv] HWD PowerUp!\r\n")));
}

/*函数名称：HWD_PowerDown
**函数参数：hDeviceContext 设备的打开上下文，由XXX_Open（）函数返回 
**函数功能：电源挂起
**函数返回：
**修 改 人：
**修改日期：
*/
VOID HWD_PowerDown(DWORD hDeviceContext)
{
	RETAILMSG(1, (TEXT("[HW drv] HWD PowerDown!\r\n")));
}

/*函数名称：HWD_Read
**函数参数：hOpenContext,		XXX_Open（）返回的设备打开上下文
			pBuffer,			输出，缓冲区的指针，读取数据会被放在该缓冲区内
			Count)			    要读取的字节数
**函数功能：从设备中读取数据  与应用程序ReadFile()函数据对应
**函数返回：返回0表示文件结束，返回-1表示失败,返回读取的字节数表示成功
**修 改 人：
**修改日期：
*/
DWORD HWD_Read(DWORD  hOpenContext,LPVOID pBuffer,DWORD  Count)			
{
	return 0;
}

/*函数名称：HWD_Write
**函数参数：hOpenContext,		XXX_Open返回的设备打开上下文
			pSourceBytes,		输入，指向要写入设备的数据的缓冲
			NumberOfBytes		缓冲中的数据的字节数
**函数功能：向设备中写入数据  与应用程序WriteFile()函数据对应
**函数返回：返回-1表示失败，返回写入的字节数表示成功
**修 改 人：
**修改日期：
*/
DWORD HWD_Write(DWORD    hOpenContext,LPCVOID  pSourceBytes,DWORD    NumberOfBytes)		
{
	return 0;
}

/*函数名称：HWD_Seek
**函数参数：hOpenContext,	XXX_Open()返回的设备打开上下文
			Amount,			要移动的距离，负数表示前移，正数表示后移
			Type			缓冲中的数据的字节数
**函数功能：移动设备中的数据指针  与应用程序SetFilePointer()函数据对应
**函数返回：返回数据的新指针位置，-1表示失败
**修 改 人：
**修改日期：
*/
DWORD HWD_Seek(DWORD  hOpenContext,	LONG   Amount,DWORD  Type)			
{
	RETAILMSG(1, (TEXT("[HW drv] HWD Seek!\r\n")));
	return 0;
}

/*函数名称：HWD_IOControl
**函数参数：hOpenContext,		由XXX_Open()返回的设备打开上下文
			dwCode,				要发送的控制码，一个32位无符号数
			pBufIn,				输入，指向输入缓冲区的指针
			dwLenIn,			输入缓冲区的长度
			pBufOut,			输出，指向输出缓冲区的指针
			dwLenOut,			输出缓冲区的最大长度
			pdwActualOut)		输出，设备实际输出的字节数 
**函数功能：向驱动程序发送控制命令  与应用程序DeviceIoControl()函数据对应
**函数返回：布尔值：TRUE表示操作成功，FALSE表示操作失败
**修 改 人：
**修改日期：
*/
BOOL HWD_IOControl(DWORD  hOpenContext,	 DWORD  dwCode, PBYTE  pBufIn,
		DWORD  dwLenIn, PBYTE  pBufOut, DWORD  dwLenOut,PDWORD pdwActualOut)		
{
	RETAILMSG(1, (TEXT("\r\n[HW drv] HWD IOControl commd:%d!\r\n"),dwCode));
	return TRUE;
}

/*函数名称：DllEntry
**函数参数：hinstDll: DLL的句柄，与一个EXE文件的句柄功能类似，一般可以通过它在得到DLL中的一
			些资源，例如对话框，除此之外一般没什么用处。
**函数功能：动态链接库的入口，每个动态链接库都需要输出这个函数，它只在动态库被加载和卸
			载时被调用，也就是设备管理器调用LoadLibrary而引起它被装入内存和调用UnloadLibrary将
			其从内存释放时被调用，因而它是每个动态链接库最早被调用的函数，一般用它做一些全局变量
			的初始化
**函数返回：布尔值：TRUE表示操作成功，FALSE表示操作失败
**修 改 人：
**修改日期：
*/
BOOL DllEntry(HINSTANCE hinstDll,DWORD dwReason,LPVOID lpReserved)
{
	//DLL_PROCESS_ATTACH :动态库被加载，
	//DLL_PROCESS_DETACH :动态库被释放。
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		RETAILMSG(1,(TEXT("Attach in [HW drv] IPOD DllEntry")));
		DisableThreadLibraryCalls((HMODULE)hinstDll);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		RETAILMSG(1,(TEXT("Dettach in [HW drv] IPOD DllEntry")));
	}

	RETAILMSG(1,(TEXT("Leaving in [HW drv] IPOD DllEntry")));

	return TRUE;
}