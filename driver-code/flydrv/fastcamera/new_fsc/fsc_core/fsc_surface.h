#ifndef __FSC_SURFACE_H_
#define __FSC_SURFACE_H_

#include "tvd_drv_if.h"

#define TVD_VDO_BUF_CNT                4



BOOL  CreateBackCarSurfaces(HWND hWnd, PVDO_BUF_INFO_T pVdoBufInfo, UINT32 u4BufCnt);
BOOL  UpdateBackCarOverlay(PVDO_BUF_INFO_T pVdoBufInfo);
BOOL  DestroyBackCarVideoSurfaces(VOID);
BOOL  DestroyBackCarUISurfaces(VOID);
BOOL  DestroyBackCarSurfaces(VOID);

BOOL  HideBackCarVideoOverlay(VOID);
BOOL  HideBackCarOverlay(VOID);

BOOL  ShowBackCarOverlay();
BOOL  DetectVideoSurfaceAvailable(HWND hWnd);

#endif
