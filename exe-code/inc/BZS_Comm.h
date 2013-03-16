  /*****************************************************************************
  *  Copyright Statement:
  *  --------------------
  *  This software is protected by copyright and the information contained
  *  herein is confidential. The software may not be copied and the information
  *  contained herein may not be used or disclosed except with the written
  *  permission of MediaTek Inc.   2008 MediaTek Inc.
  *
  *  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
  *  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
  *  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
  *  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
  *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
  *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
  *  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
  *  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
  *  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
  *  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
  *  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
  *  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
  *
  *  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
  *  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
  *  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
  *  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
  *  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
  *
  *  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
  *  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
  *  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
  *  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
  *  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
  *
  *****************************************************************************/ 
 
 /*****************************************************************************
  *
  * szFileName:
  * ---------
  *   $Workfile:  $ 
  *
  * Project:
  * --------
  *   MTKBluetooth
  *
  * Description:
  * ------------
  *   
  *   
  * Author:
  * -------
  *   mtk40390
  *
  * Last changed:
  * ------------- 
  * $Author: xiaozhou.huang $ 
  *
  * $Modtime: $  
  
  * $Revision: #8 $
 ****************************************************************************/
 
 
#ifndef __BZS_COMM_H_
#define __BZS_COMM_H_
 //
 //  BlueZone Common Structure definition
 //
#include "BZ_Error.h"
#include "Winsock2.h"
#include "Bt_api.h"
#include "Bthapi.h"
 //#include "BZS_Pairing.h"
  
#define MAX_LEN_BTPATH  256
#define MAX_LEN_DEVICE_NAME 32
#define MAX_LEN_DEVICE_ADDR 20
 
#define MAX_LEN_OF_FILE_PATH   256
 
#define MAX_BT_DEVICE_NAME_SIZE 32

#define BT_SERVICE_READY_MSG (WM_USER+0xFFF0)

#define FIXED_VOLUME_LEVEL 1
 
 
 /// ref: WPARAM as UINT_PTR(DWORD, 4B). LPARAM as LONG_PTR(LONG, 8B)
 /**
 * @brief Response Callback prototype of BlueMan
 */ 
 typedef LRESULT (*BMRspCallback) (LPVOID lpvThreadParam, UINT wMsg, WPARAM wParam, LPARAM lParam);
 /**
 * @brief Inidcation Callback prototype of BlueMan
 */
 typedef LRESULT (*BMIndCallback) (LPVOID lpvThreadParam, UINT wMsg, WPARAM wParam, LPARAM lParam);
 
 
 /// SDP Service Structures ////////////////////////////////////////////////////////////////////////
 /* JIAN
 
 */
 typedef void (* P_BTCALLBACK)(DWORD dwIndID, WPARAM wParam, LPARAM lParam);
 
 
 
 
 
 typedef struct btpathinfo
 {
     wchar_t wszDataPathFile[MAX_LEN_OF_FILE_PATH];
     wchar_t wszRefDataPathFile[MAX_LEN_OF_FILE_PATH];
 } BT_FILEPATH_INFO_T, *P_BT_FILEPATH_INFO_T;
 
 
 /**
 * Set up security 
 */
 typedef struct _tag_SECURITY_T
 {
     BT_ADDR  ba;
     unsigned short wcName[MAX_LEN_DEVICE_NAME + 1];
     UCHAR    ucPassword[20];
 } BZ_SECURITY_T;
 
 typedef struct _tag_SETPINCODE_T
 {
     UINT32   u4AddrCnt;
     UCHAR    ucPassword[20];
     UINT8    u1btAddr[1];
 } BZ_SETPIN_PARAM_T;

 typedef struct _tag_SETLINKEY_T
 {
	BT_ADDR btAddr;
	BYTE 	pu1Data[16];
 } BZ_SETKEY_PARAM_T;
 
 typedef enum
 {
   PHONE_APP,
   DEVICE_APP,   
 } E_BLUEZONE_MW_STATE_T;
  
 typedef struct _tag_BZMWCONTEXT
 {
     E_BLUEZONE_MW_STATE_T eState;
     LPVOID pUserParam;
     LPVOID pUserContext; ///< user context got from BlueCastle
     P_BTCALLBACK pfnIndCallback;
 }BZMWCONTEXT_T, *P_BZMWCONTEXT_T;

 
 typedef struct _tag_DEVICEENTRY
 {
     BT_ADDR ba;
     BYTE cFlag;
     DWORD cod;
     unsigned short name[MAX_BT_DEVICE_NAME_SIZE];
 } SDP_DEVICEENTRY_T; /// a entry to record device's info
 
#if 0
 typedef enum _tag_BTCALLBACK_MODULE
 {
     BT_BTCALLBACK_MODULE_CALL,
     BT_BTCALLBACK_MODULE_PBMGR,
     BT_BTCALLBACK_MODULE_MUSIC,
     BT_BTCALLBACK_MODULE_PAIR,
     BT_BTCALLBACK_MODULE_CONNECT,
     BT_BTCALLBACK_MODULE_LOCALDEV,
     NUM_OF_BT_BTCALLBACK
 } E_BTEVTCALLBACK_MODULE_T;
#endif
 
 typedef struct _tag_BTCALLBACK_NODE
 {
     UINT32 u4ViewID;
     P_BTCALLBACK pfnCB;
     struct _tag_BTCALLBACK_NODE *prPrev;
     struct _tag_BTCALLBACK_NODE *prNext;
     BOOL   bFlag;
 } BT_VIEWCALLBACKNODE_T, *P_BT_VIEWCALLBACKNODE_T;
 
#if FIXED_VOLUME_LEVEL
 typedef enum
 {
     BT_VOLUME_LEVEL_0 = 0,
     BT_VOLUME_LEVEL_1, 
     BT_VOLUME_LEVEL_2, 
     BT_VOLUME_LEVEL_3, 
     BT_VOLUME_LEVEL_4, 
     BT_VOLUME_LEVEL_5, 
     BT_VOLUME_LEVEL_6,
     BT_VOLUME_LEVEL_7,
     BT_VOLUME_LEVEL_8,
     BT_VOLUME_LEVEL_CNT,
     BT_VOLUME_MAX_LEVEL = BT_VOLUME_LEVEL_8,
 }E_BT_VOLUME_LEVEL_T;
#else
#define BT_VOLUME_LEVEL_0 0
#endif
 
 typedef enum
 {
     SYNCML_COM,
     HFP_COM,
     SPP_COM,
     DUN_COM,
     PBAP_COM,
     MAX_CNT_OF_BT_COM
 } E_BT_COM_TYPE_T;

 
 typedef enum
 {
     BT_SYNC_UNKNOWN,
     BT_SYNC_SMS,
     BT_SYNC_CONTACT,
     BT_SYNC_NOTE,
     BT_SYNC_CALENDER,
     MAX_OF_BT_SYNC_TYPE_CNT
 } E_BT_SYNC_TYPE_T;

 
 typedef enum BT_CHARSET_TYPE_tag{
     BT_CHARSET_UNKNOW,
     BT_CHARSET_UTF8 = 1 << 0,
     BT_CHARSET_UCS2 = 1 << 1,
     BT_CHARSET_GSM = 1 << 2,
     BT_CHARSET_IRA = 1 << 3,
     BT_CHARSET_HEX = 1 << 4,
     BT_CHARSET_PCCP = 1 << 5,
     BT_CHARSET_PCDN = 1 << 6,
     BT_CHARSET_PCC437 = 1 << 7,
     BT_CHARSET_NULL = 1 << 8,
     BT_CHARSET_ASCII = 1 << 9,
     BT_CHARSET_PCCP437 = 1 << 10,
     MAX_CNT_OF_BT_CHARSET = 11
 } E_BT_CHARSET_TYPE_T;

 typedef enum 
 {
     BT_STATUS_OFF = 0, 
     BT_STATUS_ON_SCAN_OFF = 1,
     BT_STATUS_ON_SCAN_DISCOVERABLE = 2,
     BT_STATUS_ON_SCAN_CONNECTABLE = 3, 
     BT_STATUS_ON_SCAN_BOTH_DISCOVERABLE_AND_CONNECTABLE = 4, 
     BT_STATUS_UNKNOWN = 5
 }BT_STATUS;

 typedef struct
 {
	 UINT32 u4MWVersion;
	 UINT32 u4MWSubVersion;

	 UINT32 u4BMVersion;
	 UINT32 u4BMSubVersion;

	 UINT32 u4BCVersion;
	 UINT32 u4BCSubVersion;

	 UINT32 u4A2DPVersion;
	 UINT32 u4A2DPSubVersion;

	 UINT32 u4AVRCPVersion;
	 UINT32 u4AVRCPSubVersion;

	 UINT32 u4HCEVersion;
	 UINT32 u4HCECPSubVersion;
 } BT_VERSION, *P_BT_VERSION;

#define MAX_LEN_OF_PHONE_IMEI 50
 
 
#endif
