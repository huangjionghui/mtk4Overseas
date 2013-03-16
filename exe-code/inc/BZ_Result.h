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
 * $Author: bin.xu $ 
 *
 * $Modtime: $  
 
 * $Revision: #2 $
****************************************************************************/


#include <windows.h>
#pragma once

typedef DWORD HF_RESULT;

#define BT_RESULT_SUCCESS                        0
#define BT_RESULT_FAIL                            0x80000001
#define BT_RESULT_ERROR_HANDLE                    0x80000002
#define BT_RESULT_INVALID_PARAMETER                0x80000003
#define BT_RESULT_UNINIT                        0x80000004
#define BT_RESULT_UNSUPPORT_AG_FEATURE            0x80000005
#define BT_RESULT_ALREADY_CONNECTED                0x80000006
#define BT_RESULT_CMD_IN_EXECUTION                0x80000007
#define BT_RESULT_RSP_ERROR                    0x80000008

// limited resource like memory, event, etc.
#define BT_RESULT_LIMITED_RESOURCE            0x80000101
#define BT_RESULT_WAIT_FAIL                    0x80000102
#define BT_RESULT_OUT_OF_MEMORY                0x80000103

// context manager error
#define BT_RESULT_USERCONTEXT_FULL            0x80001000
#define BT_RESULT_CMD_IN_PROCESS                0x80001001
#define BT_RESULT_NO_PENDING_CMD                0x80001002
#define BT_RESULT_USER_NOT_ATTACHED            0x80001003
#define BT_RESULT_CMD_EXECUTE                    0x80001004


// Manager Error
#define BT_RESULT_SOCKET_ERROR                0x80002001
#define BT_RESULT_TIMEOUT                    0x80002002
#define BT_RESULT_NO_CONNECTION                0x80002003
#define BT_RESULT_INTERRUPT_PB              0x80002004

// Proxy Error
#define BT_RESULT_INVALID_SERVICE_HANDLE        0x80009001
#define BT_RESULT_CMD_QUEUE_FULL                0x80009002
#define BT_RESULT_NOT_CONNECTED                0x80009003
#define BT_RESULT_DISCONNECTING                0x80009004

// Message queue error
#define BT_RESULT_MESSAGE_QUEUE_FULL            0x8000A001
#define BT_RESULT_MESSAGE_QUEUE_EMPTY        0x8000A002

#define BT_RESULT_ERROR_MASK                    0x80000000


typedef DWORD A2DP_RESULT;
#define A2DP_RESULT_SUCCESS                0
#define A2DP_RESULT_FAIL                0x90000001


typedef DWORD DUN_RESULT;
#define DUN_RESULT_SUCCESS                0
#define DUN_RESULT_FAIL                0xA0000001

typedef DWORD HSPG_RESULT;
#define HSPG_RESULT_SUCCESS                        0
#define HSPG_RESULT_FAIL                            0xB0000001
#define HSPG_RESULT_ERROR_HANDLE                        0xB0000002
#define HSPG_RESULT_INVALID_PARAMETER            0xB0000003
#define HSPG_RESULT_ALREADY_CONNECTED                  0xB0000004
#define HSPG_RESULT_CMD_IN_EXECUTION                  0xB0000005
#define HSPG_RESULT_SOCKET_ERROR                    0xB0000006
#define HSPG_RESULT_NOT_CONNECTED                0xB0000007
#define HSPG_RESULT_CREATE_SCO_FAIL                0xB0000008

typedef DWORD AVRCP_RESULT;
#define AVRCP_RESULT_SUCCESS            0
#define ACRCP_RESULT_FAIL                0xC0000001

typedef DWORD HSP_RESULT;
#define HSP_RESULT_SUCCESS                    0
#define HSP_RESULT_FAIL                        0xD0000001
#define HSP_RESULT_ERROR_HANDLE                0xD0000002
#define HSP_RESULT_INVALID_PARAMETER        0xD0000003
#define HSP_RESULT_ALREADY_CONNECTED        0xD0000004
#define HSP_RESULT_CMD_IN_EXECUTION            0xD0000005
#define HSP_RESULT_SOCKET_ERROR                0xD0000006
#define HSP_RESULT_NOT_CONNECTED            0xD0000007
#define HSP_RESULT_CREATE_SCO_FAIL            0xD0000008


typedef DWORD SPP_RESULT;
#define SPP_RESULT_SUCCESS                0
#define SPP_RESULT_FAIL                0xE0000001

typedef DWORD FTP_RESULT;
#define FTP_RESULT_SUCCESS            0
#define FTP_RESULT_FAIL                0xF0000001


