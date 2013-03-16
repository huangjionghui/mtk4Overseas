#ifndef  __BACK_CAR_H_
#define  __BACK_CAR_H_


#define FILE_DEVICE_FSC 					32768
#define FUNCTION_NOTIFY_APP_READY			2048
#define FUNCTION_GET_ARM2_VERSION			2049
#define FUNCTION_NOTIFY_ARM2_STOP			2050
#define FUNCTION_NOTIFY_ARM2_BUFF_MEMSET	2051

#define IOCTL_FSC_NOTIFY_APP_READY          CTL_CODE(FILE_DEVICE_FSC, FUNCTION_NOTIFY_APP_READY, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FSC_GET_ARM2_VERSION          CTL_CODE(FILE_DEVICE_FSC, FUNCTION_GET_ARM2_VERSION, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FSC_NOTIFY_ARM2_STOP          CTL_CODE(FILE_DEVICE_FSC, FUNCTION_NOTIFY_ARM2_STOP, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FSC_NOTIFY_ARM2_BUFF_MEMSET   CTL_CODE(FILE_DEVICE_FSC, FUNCTION_NOTIFY_ARM2_BUFF_MEMSET, METHOD_BUFFERED, FILE_ANY_ACCESS)


#endif