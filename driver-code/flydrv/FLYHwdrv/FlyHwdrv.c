/*ģ�����ƣ�FlyHwdrv.c
**ģ�鹦�ܣ�Ӳ�������ӿڻ���
**
**�� �� �ˣ��ƾ���
**�޸����ڣ�2012-09-01
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


/*�������ƣ�HwCtlIOValue
**����������
**�������ܣ�GPIO��д��������
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
*/
static void HwCtlIOValue(gpiocfg_t *pgpio_cfg)
{
	//����Ӳ����Դ������
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

/*�������ƣ�HwCtlI2cValue
**����������
**�������ܣ�I2C��д��������
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
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

/*�������ƣ�HwCtlI2cValue
**����������
**�������ܣ�I2C��д��������
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
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

/*�������ƣ�HwAdcGetValue
**����������
**�������ܣ�ADC��ȡ����
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
*/
static void HwAdcGetValue(adccfg_t *padc_cfg)
{
	padc_cfg->value = AdcGetValue(padc_cfg->channel);
}

/*�������ƣ�HwKeyIrqEnable
**����������
**�������ܣ��жϲ�������
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
*/
static void HwKeyIrqEnable(void *key_irq_rr,
		void *key_irq_rl,void *key_irq_lr,void *key_irq_ll,BOOL bEnable)
{
	EnableKeyIrq(key_irq_rr,key_irq_rl,key_irq_lr,key_irq_ll,bEnable);
}

/*�������ƣ�HwPwmSetFreq
**����������
**�������ܣ�PWM��������
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
*/
static void HwPwmSetFreq(UINT16 iNum,UINT16 iDuty)
{
	PWM_SetFreq(iNum, iDuty);
}

/*�������ƣ�HwCreateShareMmap
**����������
**�������ܣ����������ڴ�
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
*/
static void *HwCreateShareMmap(UINT32 iSize)
{
	if (gHwInfo.pShareMmapAddr != NULL)
		return gHwInfo.pShareMmapAddr;

	gHwInfo.pShareMmapAddr =  (void*)LocalAlloc(LPTR, iSize);
	RETAILMSG(1, (TEXT("[HW drv] HW Info share mmap addr:%p\r\n"),gHwInfo.pShareMmapAddr));

	return gHwInfo.pShareMmapAddr;
}

/*�������ƣ�HWD_Init
**����������dwContext���ַ�����ָ��ע����м�¼���������ļ�
**�������ܣ������豸�����豸�����ص�ʱ�����
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
*/
DWORD HWD_Init(DWORD dwContext)
{
	memset(&gHwInfo,0,sizeof(struct HwInfo));

	//��I2C
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

/*�������ƣ�HWD_Deinit
**����������XXX_Init()�������ص��豸������
**�������ܣ��ͷ��豸�����豸��ж�ص�ʱ�����
**�������أ������豸ж���Ƿ�ɹ�
**�� �� �ˣ�
**�޸����ڣ�
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

/*�������ƣ�RegisterApiTable
**����������
**�������ܣ��������������ϵ�ַ
**�������أ����ص�ַ�б�
**�� �� �ˣ�
**�޸����ڣ�
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

/*�������ƣ�HWD_GetDllProcAddr
**����������
**�������ܣ�ȡ��Ӳ�������ĵ�ַ
**�������أ����ص�ַ�б�
**�� �� �ˣ�
**�޸����ڣ�
*/
struct HwDrvAddrTable *HWD_GetDllProcAddr(void)
{
	return RegisterApiTable(&gHwInfo);
}

/*�������ƣ�HWD_CreateShareMmap
**����������
**�������ܣ�ȡ�ù����ڴ�ĵ�ַ
**�������أ����ص�ַ�б�
**�� �� �ˣ�
**�޸����ڣ�
*/
void *HWD_CreateShareMmap(UINT32 iSize)
{
	return HwCreateShareMmap(iSize); 
}

/*�������ƣ�HWD_Open
**����������DWORD hDeviceContext,	 �豸�����ģ���XXX_Init()��������
			DWORD AccessCode,		 �豸�ķ���ģʽ����CreateFile()��������
			DWORD ShareMode)	     �豸�Ĺ���ģʽ����CreateFile()��������
**�������ܣ����豸���ж�д ��Ӧ�ó����CreateFile()������Ӧ
**�������أ������豸�Ĵ�������
**�� �� �ˣ�
**�޸����ڣ�
*/
DWORD HWD_Open(DWORD hDeviceContext, DWORD AccessCode,DWORD ShareMode)	
{
	RETAILMSG(1, (TEXT("[HW drv] HWD Open ok\r\n")));
	return hDeviceContext;
}

/*�������ƣ�HWD_Close
**����������hDeviceContext �豸�Ĵ������ģ���XXX_Open������������ 
**�������ܣ��ر��豸 ��Ӧ�ó����CloseHandle()������Ӧ
**�������أ������豸�ر��Ƿ�ɹ�
**�� �� �ˣ�
**�޸����ڣ�
*/
BOOL HWD_Close(DWORD hDeviceContext) 
{
	RETAILMSG(1, (TEXT("[HW drv] HWD Close!\r\n")));
	return TRUE;
}

/*�������ƣ�HWD_PowerUp
**����������hDeviceContext �豸�Ĵ������ģ���XXX_Open������������ 
**�������ܣ���Դ����
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
*/
VOID HWD_PowerUp(DWORD hDeviceContext)
{
	RETAILMSG(1, (TEXT("[HW drv] HWD PowerUp!\r\n")));
}

/*�������ƣ�HWD_PowerDown
**����������hDeviceContext �豸�Ĵ������ģ���XXX_Open������������ 
**�������ܣ���Դ����
**�������أ�
**�� �� �ˣ�
**�޸����ڣ�
*/
VOID HWD_PowerDown(DWORD hDeviceContext)
{
	RETAILMSG(1, (TEXT("[HW drv] HWD PowerDown!\r\n")));
}

/*�������ƣ�HWD_Read
**����������hOpenContext,		XXX_Open�������ص��豸��������
			pBuffer,			�������������ָ�룬��ȡ���ݻᱻ���ڸû�������
			Count)			    Ҫ��ȡ���ֽ���
**�������ܣ����豸�ж�ȡ����  ��Ӧ�ó���ReadFile()�����ݶ�Ӧ
**�������أ�����0��ʾ�ļ�����������-1��ʾʧ��,���ض�ȡ���ֽ�����ʾ�ɹ�
**�� �� �ˣ�
**�޸����ڣ�
*/
DWORD HWD_Read(DWORD  hOpenContext,LPVOID pBuffer,DWORD  Count)			
{
	return 0;
}

/*�������ƣ�HWD_Write
**����������hOpenContext,		XXX_Open���ص��豸��������
			pSourceBytes,		���룬ָ��Ҫд���豸�����ݵĻ���
			NumberOfBytes		�����е����ݵ��ֽ���
**�������ܣ����豸��д������  ��Ӧ�ó���WriteFile()�����ݶ�Ӧ
**�������أ�����-1��ʾʧ�ܣ�����д����ֽ�����ʾ�ɹ�
**�� �� �ˣ�
**�޸����ڣ�
*/
DWORD HWD_Write(DWORD    hOpenContext,LPCVOID  pSourceBytes,DWORD    NumberOfBytes)		
{
	return 0;
}

/*�������ƣ�HWD_Seek
**����������hOpenContext,	XXX_Open()���ص��豸��������
			Amount,			Ҫ�ƶ��ľ��룬������ʾǰ�ƣ�������ʾ����
			Type			�����е����ݵ��ֽ���
**�������ܣ��ƶ��豸�е�����ָ��  ��Ӧ�ó���SetFilePointer()�����ݶ�Ӧ
**�������أ��������ݵ���ָ��λ�ã�-1��ʾʧ��
**�� �� �ˣ�
**�޸����ڣ�
*/
DWORD HWD_Seek(DWORD  hOpenContext,	LONG   Amount,DWORD  Type)			
{
	RETAILMSG(1, (TEXT("[HW drv] HWD Seek!\r\n")));
	return 0;
}

/*�������ƣ�HWD_IOControl
**����������hOpenContext,		��XXX_Open()���ص��豸��������
			dwCode,				Ҫ���͵Ŀ����룬һ��32λ�޷�����
			pBufIn,				���룬ָ�����뻺������ָ��
			dwLenIn,			���뻺�����ĳ���
			pBufOut,			�����ָ�������������ָ��
			dwLenOut,			�������������󳤶�
			pdwActualOut)		������豸ʵ��������ֽ��� 
**�������ܣ������������Ϳ�������  ��Ӧ�ó���DeviceIoControl()�����ݶ�Ӧ
**�������أ�����ֵ��TRUE��ʾ�����ɹ���FALSE��ʾ����ʧ��
**�� �� �ˣ�
**�޸����ڣ�
*/
BOOL HWD_IOControl(DWORD  hOpenContext,	 DWORD  dwCode, PBYTE  pBufIn,
		DWORD  dwLenIn, PBYTE  pBufOut, DWORD  dwLenOut,PDWORD pdwActualOut)		
{
	RETAILMSG(1, (TEXT("\r\n[HW drv] HWD IOControl commd:%d!\r\n"),dwCode));
	return TRUE;
}

/*�������ƣ�DllEntry
**����������hinstDll: DLL�ľ������һ��EXE�ļ��ľ���������ƣ�һ�����ͨ�����ڵõ�DLL�е�һ
			Щ��Դ������Ի��򣬳���֮��һ��ûʲô�ô���
**�������ܣ���̬���ӿ����ڣ�ÿ����̬���ӿⶼ��Ҫ��������������ֻ�ڶ�̬�ⱻ���غ�ж
			��ʱ�����ã�Ҳ�����豸����������LoadLibrary����������װ���ڴ�͵���UnloadLibrary��
			����ڴ��ͷ�ʱ�����ã��������ÿ����̬���ӿ����类���õĺ�����һ��������һЩȫ�ֱ���
			�ĳ�ʼ��
**�������أ�����ֵ��TRUE��ʾ�����ɹ���FALSE��ʾ����ʧ��
**�� �� �ˣ�
**�޸����ڣ�
*/
BOOL DllEntry(HINSTANCE hinstDll,DWORD dwReason,LPVOID lpReserved)
{
	//DLL_PROCESS_ATTACH :��̬�ⱻ���أ�
	//DLL_PROCESS_DETACH :��̬�ⱻ�ͷš�
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