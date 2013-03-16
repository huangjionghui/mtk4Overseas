
//-------------------------------------------------------------------------
// 文件名：
// 功能：
// 备注：
//
//
//         By  李素伟
//             lisuwei_fg053rd@flyaudio.cn
//-------------------------------------------------------------------------

#ifndef UDD_SERVICE_H
#define UDD_SERVICE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#if 1

    //IO Function
#define SOC_IO_Init (pUDDApiTable->pfnSOC_IO_Init)
#define SOC_IO_Write (pUDDApiTable->pfnSOC_IO_Write)
#define SOC_IO_Read (pUDDApiTable->pfnSOC_IO_Read)



    // Driver Global
#define SOC_DriverGlobal_Get (pUDDApiTable->pfnSOC_DriverGlobal_Get)



    //io_status
#define HIGH			(1)
#define LOW				(0)

    //io_dir
#define OUTPUT  		(0x01<<0)
#define INPUT			(0x00<<0)
#define ENABLE_UP_PULL	(0x01<<1)
#define DISABLE_UP_PULL	(0x01<<1)


	typedef enum
	{
		RGB565 = 2,
		RGB888 = 4,
	} RGB_OUTPUT_TYPE;

	typedef enum
	{
		OFF,
		ON,
	} LAYER_STATUS;


	typedef struct _LAYER_CONFIG
	{
		DWORD LayerNum;
		LAYER_STATUS LayerStatus;
		BOOL IsInit;
		DWORD Width;
		DWORD Height;
		unsigned char ColorKeyEnable;
		DWORD OverlaySrcColorKeyLow;
		DWORD OverlaySrcColorKeyHigh;
		DWORD AlphaValue;
		RGB_OUTPUT_TYPE Rgb;
		DWORD dwPAddr;
		DWORD dwVAddr;
	
	} LAYERCONFIG;

typedef struct
{
    unsigned short int x;
    unsigned short int y;
} TOUCH_POINT;

/*
typedef enum   //interrupt_type
{
	GPIO_INTTYPE_HIGH_LEVEL,
	GPIO_INTTYPE_LOW_LEVEL,
	GPIO_INTTYPE_RAISING_EDGE,
	GPIO_INTTYPE_FALLING_EDGE,
	GPIO_INTTYPE_HI_LO_LEVEL,
	GPIO_INTTYPE_RISE_FALL_EDGE
}GPIO_INTTYPE;
*/
typedef struct _IO_CONFIG
{
	DWORD io_group;
	DWORD io_index;
	DWORD io_status;//读出或写入的io状态
	DWORD io_dir;	//io方向
	BOOL interrupt_en; //io中断使能
	GPIO_INTTYPE  interrupt_type;//io中断类型
} IO_CONFIG, *PIO_CONFIG;


typedef enum
{
    MEM_ALLOCATE = 0,
    MEM_FIXED = 1,
} GRT_MEM_WAY;

typedef enum
{
    NOT_CONTINUOUS = 0,
    CONTINUOUS = 1,
} MEM_STYLE;



typedef struct _FLY_DRIVER_GLOBAL
{
    //DVD
    DWORD DVD_Flag;


    //Audio




    //....



} FLY_DRIVER_GLOBAL, *PFLY_DRIVER_GLOBAL;




#endif


    //GUI Function
    typedef void 	(*pfnUDD_GUI_DrawLine)(LAYERCONFIG *playerconfig, DWORD x1, DWORD y1, DWORD x2, DWORD y2, DWORD color);
    typedef void 	(*pfnUDD_GUI_LCDFill_COLOR)(LAYERCONFIG *playerconfig, DWORD rgb_value);
    typedef void 	(*pfnUDD_GUIReadMem2Layer)(LAYERCONFIG *playerconfig, DWORD ImgAddr, unsigned int posx, unsigned int posy);
    typedef void 	(*pfnUDD_GUI_DrawRect)(LAYERCONFIG *playerconfig, DWORD x0, DWORD y0, DWORD x1, DWORD y1, DWORD lineColor, DWORD backColor);
    typedef void 	(*pfnUDD_GUI_Draw_BMP)(LAYERCONFIG *playerconfig, DWORD ImgAddr, DWORD x0, DWORD y0);

    //LCD Function
    typedef void	(*pfnUDD_Disp_LayerInit)(LAYERCONFIG *playerconfig);
    typedef void	(*pfnUDD_Disp_LayerEnable)(LAYERCONFIG *playerconfig);
    typedef void	(*pfnUDD_Disp_LayerDisable)(LAYERCONFIG *playerconfig);
    typedef void	(*pfnUDD_LCD_FlipPage)(LAYERCONFIG *playerconfig, DWORD dwPAddr, DWORD dwVAddr);
    typedef void	(*pfnUDD_SOC_LayerSetOSDAlpha)(LAYERCONFIG *playerconfig);
    typedef void	(*pfnUDD_SOC_LayerSetColorKey)(LAYERCONFIG *playerconfig);



    //SYSTEM Function
    typedef DWORD	(*pfnUDD_ReadFile2Buffer)(TCHAR *FilePath, DWORD VAddr);
    typedef void (*pfnUDD_DebugWriteNum2File)(char *FilePath, DWORD Value);

    //communication Function
    typedef DWORD	(*pfnUDD_CMM_MQ_ReadQueue)(/*HANDLE hRMsgQue*/DWORD index, DWORD BlockTime);
    typedef void	(*pfnUDD_CMM_MQ_WriteQueue)(/*HANDLE hWMsgQue,*/DWORD index, DWORD WriteData);
    typedef void	(*pfnUDD_CMM_MQ_CreateQueue)(/*TCHAR *msgq_name,DWORD index,HANDLE hRMsgQue,HANDLE hWMsgQue, */DWORD index, DWORD max_msg);

    typedef DWORD  	(*pfnUDD_CMM_Global_GetValue)(DWORD Index);
    typedef void	(*pfnUDD_CMM_Global_SetValue)(DWORD Index, DWORD Value);

    //typedef HANDLE	(*pfnUDD_OS_CreateMsgQ)(TCHAR* pMsgQName, DWORD dwMaxMsgs, DWORD dwMsgSize, BOOL fReadAccess);
    //typedef BOOL	(*pfnUDD_OS_CloseMsgQ)(HANDLE hMsgQ);
    //typedef HANDLE	(*pfnUDD_OS_OpenMsgQ)(HANDLE hSrcProc, HANDLE hMsgQ, BOOL fReadAccess);
    //typedef BOOL	(*pfnUDD_OS_ReadMsgQ)(HANDLE hMsgQ, PVOID pBuffer, DWORD cbBufSize, PDWORD pdwBytesRead, DWORD BlockTime);
    //typedef BOOL	(*pfnUDD_OS_WriteMsgQ)(HANDLE hMsgQ, PVOID pBuffer, DWORD cbBufSize, DWORD BlockTime);



    //IO Function
    typedef void	(*pfnUDD_SOC_IO_Init)(IO_CONFIG *pIoConfig);
    typedef void	(*pfnUDD_SOC_IO_Write)(IO_CONFIG *pIoConfig);
    typedef void	(*pfnUDD_SOC_IO_Read)(IO_CONFIG *pIoConfig);

    //Touch Function
    typedef void	(*pfnUDD_TCH2LCD_Position)(TOUCH_POINT *pTchpoint, DWORD *px, DWORD *py);

    //MEM Function
    typedef BOOL	(*pfnUDD_Mem_RequestBuf)(unsigned size, GRT_MEM_WAY bUse_fixed_Mem, MEM_STYLE bUse_Continuous_Mem, DWORD dwAttribute, DWORD *ppVirtAddr, DWORD *ppPhysAddr);



    // UDD
    typedef BOOL	(*pfnUDD_UDD_GetExitFlag)();
    typedef void	(*pfnUDD_UDD_LoadAdd)();
    typedef void	(*pfnUDD_UDD_LoadDec)();

    // DRIVER_GLOBAL
    typedef PFLY_DRIVER_GLOBAL (*pfnUDD_SOC_DriverGlobal_Get)();
	//printf
    typedef void (*pfnUDD_dbg_printf)(const char *fmt, ... );


    //导出的函数列表
    typedef struct UDD_API_TABLE_TAG
    {
        //GUI Function
        pfnUDD_GUI_DrawLine								pfnGUI_DrawLine;
        pfnUDD_GUI_LCDFill_COLOR						pfnGUI_LCDFill_COLOR;
        pfnUDD_GUIReadMem2Layer							pfnGUIReadMem2Layer;
        pfnUDD_GUI_DrawRect								pfnGUI_DrawRect;
        pfnUDD_GUI_Draw_BMP								pfnGUI_Draw_BMP;



        //LCD Function
        pfnUDD_Disp_LayerInit						    pfnDisp_LayerInit;
        pfnUDD_Disp_LayerEnable							pfnDisp_LayerEnable;
        pfnUDD_Disp_LayerDisable							pfnDisp_LayerDisable;
        pfnUDD_LCD_FlipPage								pfnLCD_FlipPage;
        pfnUDD_SOC_LayerSetOSDAlpha						pfnSOC_LayerSetOSDAlpha;
        pfnUDD_SOC_LayerSetColorKey						pfnSOC_LayerSetColorKey;



        //SYSTEM Function
        pfnUDD_ReadFile2Buffer 							pfnReadFile2Buffer;
        pfnUDD_DebugWriteNum2File							pfnDebugWriteNum2File;


        //communication Function
        pfnUDD_CMM_MQ_ReadQueue							pfnCMM_MQ_ReadQueue;
        pfnUDD_CMM_MQ_WriteQueue 						pfnCMM_MQ_WriteQueue;
        pfnUDD_CMM_MQ_CreateQueue 						pfnCMM_MQ_CreateQueue;

        pfnUDD_CMM_Global_GetValue						pfnCMM_Global_GetValue;
        pfnUDD_CMM_Global_SetValue 						pfnCMM_Global_SetValue;


        //pfnUDD_OS_CreateMsgQ 						pfnOS_CreateMsgQ;
        //pfnUDD_OS_CloseMsgQ 						pfnOS_CloseMsgQ;
        //pfnUDD_OS_OpenMsgQ 							pfnOS_OpenMsgQ;
        //pfnUDD_OS_ReadMsgQ 							pfnOS_ReadMsgQ;
        //pfnUDD_OS_WriteMsgQ 						pfnOS_WriteMsgQ;





        //IO Function
        pfnUDD_SOC_IO_Init                              pfnSOC_IO_Init;
        pfnUDD_SOC_IO_Write                             pfnSOC_IO_Write;
        pfnUDD_SOC_IO_Read                              pfnSOC_IO_Read;

        //Touch Function
        pfnUDD_TCH2LCD_Position						pfnTCH2LCD_Position;

        //MEM Function
        pfnUDD_Mem_RequestBuf						pfnMem_RequestBuf;



        //UDD
        pfnUDD_UDD_GetExitFlag			pfnUDD_GetExitFlag;
        pfnUDD_UDD_LoadAdd                    pfnUDD_LoadAdd;
        pfnUDD_UDD_LoadDec                    pfnUDD_LoadDec;

        // DRIVER_GLOBAL
        pfnUDD_SOC_DriverGlobal_Get        pfnSOC_DriverGlobal_Get;

	// printf
	pfnUDD_dbg_printf 	   pfndbg_printf;


    } UDD_API_TABLE, *PUDD_API_TABLE;


    typedef BOOL	(*pfnUDDGetApiTable)(UDD_API_TABLE *pApiTable);


    typedef struct __UDDGlueLoaderInfo
    {
        HINSTANCE			hUDDDllHandle;
        UDD_API_TABLE		UDDApiTable;
        pfnUDDGetApiTable 	pFNUDDGetApiTable;
    } UDDLoaderInfo, *PUDDLoaderInfo;


    ////////////////////////////
    //   一般驱动使用         //
    ////////////////////////////
    UDD_API_TABLE	*UDD_ConnectService();
    void	UDD_DisconnectService();





#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */



#endif
