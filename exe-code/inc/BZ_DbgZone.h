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
 *   mtk40027
 *
 * Last changed:
 * ------------- 
 * $Author: bin.xu $ 
 *
 * $Modtime: $  
 
 * $Revision: #2 $
****************************************************************************/

#ifndef __BZ_DBGZONE_H
#define __BZ_DBGZONE_H

#include <windows.h>
#include <dbgapi.h>

#define BZ_TRACE 0

#define ZONEID_FLOWCHART            1
#define ZONEID_TRACE                2
#define ZONEID_CONTEXTMANAGER       7
#define ZONEID_MANAGER              8
#define ZONEID_CONNHANDLER          9
#define ZONEID_COMMANDHANDLER       10
#define ZONEID_DECODER              11
#define ZONEID_ENCODER              12
#define ZONEID_MESSAGEQUEUE         13
#define ZONEID_WARNING              14
#define ZONEID_ERROR                15


#define ZONEMASK_FLOWCHART          (1<<ZONEID_FLOWCHART)
#define ZONEMASK_TRACE              (1<<ZONEID_TRACE)
#define ZONEMASK_CONTEXTMANAGER     (1<<ZONEID_CONTEXTMANAGER)
#define ZONEMASK_MANAGER            (1<<ZONEID_MANAGER)
#define ZONEMASK_CONNHANDLER        (1<<ZONEID_CONNHANDLER)
#define ZONEMASK_COMMANDHANDLER     (1<<ZONEID_COMMANDHANDLER)
#define ZONEMASK_DECODER            (1<<ZONEID_DECODER)
#define ZONEMASK_ENCODER            (1<<ZONEID_ENCODER)
#define ZONEMASK_MESSAGEQUEUE       (1<<ZONEID_MESSAGEQUEUE)
#define ZONEMASK_WARNING            (1<<ZONEID_WARNING)
#define ZONEMASK_ERROR              (1<<ZONEID_ERROR)


#define ZONE_FLOWCHART              DEBUGZONE(ZONEID_FLOWCHART)
#define ZONE_TRACE                  DEBUGZONE(ZONEID_TRACE)
#define ZONE_CONTEXTMANAGER         DEBUGZONE(ZONEID_CONTEXTMANAGER)
#define ZONE_MANAGER                DEBUGZONE(ZONEID_MANAGER)
#define ZONE_CONNHANDLER            DEBUGZONE(ZONEID_CONNHANDLER)
#define ZONE_COMMANDHANDLER         DEBUGZONE(ZONEID_COMMANDHANDLER)
#define ZONE_DECODER                DEBUGZONE(ZONEID_DECODER)
#define ZONE_ENCODER                DEBUGZONE(ZONEID_ENCODER)
#define ZONE_MESSAGEQUEUE           DEBUGZONE(ZONEID_MESSAGEQUEUE)
#define ZONE_WARNING                DEBUGZONE(ZONEID_WARNING)
#define ZONE_ERROR                  DEBUGZONE(ZONEID_ERROR)


#define ZONE_DECODER_FLOWCHART          (ZONE_DECODER&&ZONE_FLOWCHART)
#define ZONE_COMMANDHANDLER_FLOWCHART   (ZONE_COMMANDHANDLER&&ZONE_FLOWCHART)
#define ZONE_CONNHANDLER_FLOWCHART      (ZONE_CONNHANDLER&&ZONE_FLOWCHART)
#define ZONE_MANAGER_FLOWCHART          (ZONE_MANAGER&&ZONE_FLOWCHART)
#define ZONE_CONTEXTMANAGER_FLOWCHART   (ZONE_CONTEXTMANAGER&&ZONE_FLOWCHART)


#if 0
#define DEBUG_ERROR             0x00000001
#define DEBUG_WARN              0x00000002
#define DEBUG_OUTPUT            0x00000004
#define DEBUG_VERBOSE           0x00000008

#define DEBUG_SHELL_ALL         0x00000030
#define DEBUG_SHELL_INIT        0x00000010
#define DEBUG_SHELL_TRACE       0x00000020

#define DEBUG_HCI_TRANSPORT_ALL 0x000000c0
#define DEBUG_HCI_DUMP          0x00000040
#define DEBUG_HCI_TRANSPORT     0x00000080

#define DEBUG_HCI_ALL           0x00000f00
#define DEBUG_HCI_INIT          0x00000100
#define DEBUG_HCI_TRACE         0x00000200
#define DEBUG_HCI_PACKETS       0x00000400
#define DEBUG_HCI_CALLBACK      0x00000800

#define DEBUG_L2CAP_ALL         0x000f0000
#define DEBUG_L2CAP_INIT        0x00010000
#define DEBUG_L2CAP_TRACE       0x00020000
#define DEBUG_L2CAP_PACKETS     0x00040000
#define DEBUG_L2CAP_CALLBACK    0x00080000

#define DEBUG_RFCOMM_ALL        0x00f00000
#define DEBUG_RFCOMM_INIT       0x00100000
#define DEBUG_RFCOMM_TRACE      0x00200000
#define DEBUG_RFCOMM_PACKETS    0x00400000
#define DEBUG_RFCOMM_CALLBACK   0x00800000

#define DEBUG_SDP_ALL           0x0f000000
#define DEBUG_SDP_INIT          0x01000000
#define DEBUG_SDP_TRACE         0x02000000
#define DEBUG_SDP_PACKETS       0x04000000
#define DEBUG_SDP_CALLBACK      0x08000000

#define DEBUG_TDI_ALL           0x00f00000
#define DEBUG_TDI_INIT          0x00100000
#define DEBUG_TDI_TRACE         0x00200000
#define DEBUG_TDI_PACKETS       0x00400000
#define DEBUG_TDI_CALLBACK      0x00800000

#define DEBUG_PAN_ALL           0x0000f000
#define DEBUG_PAN_INIT          0x00001000
#define DEBUG_PAN_TRACE         0x00002000
#define DEBUG_PAN_PACKETS       0x00004000
#define DEBUG_PAN_CALLBACK      0x00008000

#define DEBUG_AVDTP_ALL         0xf0000000
#define DEBUG_AVDTP_INIT        0x10000000
#define DEBUG_AVDTP_TRACE       0x20000000
#define DEBUG_AVDTP_PACKETS     0x40000000
#define DEBUG_AVDTP_CALLBACK    0x80000000
#endif

#ifndef MTK_NOLOGGER
extern unsigned int gMask;
void DumpBuff (unsigned int cMask, __out_bcount(cBuffer) unsigned char *lpBuffer, unsigned int cBuffer);
void DumpBuffPfx (unsigned int cMask, WCHAR *lpszLinePrefix, unsigned char *lpBuffer, unsigned int cBuffer);
void DebugOut (unsigned int cMask, WCHAR *lpszFormat, ...);
void DebugInitialize (void);
void DebugUninitialize (void);
void DebugSetMask (unsigned int aMask);
void DebugSetOutput (unsigned int fConsole);
#endif

#endif
