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


#ifndef _BZS_PAIRING_H_
#define _BZS_PAIRING_H_

#include "BZS_Comm.h"

#define MAX_LEN_PINCODE  4

#define DEFAULT_LOCALDEVICE_PINCODE L"0000"

typedef struct _tag_BTPairedReocrd
{
    wchar_t     wszDevName[MAX_LEN_DEVICE_NAME + 1];        // Remote Device Name
    wchar_t     wszDevAddr[MAX_LEN_DEVICE_ADDR + 1];        // Remote Device MAC Addr
    BT_ADDR     btAddr;            // Remote Device MAC Address
    UINT32      u4ServiceMask;
    UINT32      u4NumOfService;
    BOOL        fgConnected;        // Designate whether the PND is connected with this remote device
} BT_PAIREDRECORDINFO_T, *P_BT_PAIREDRECORDINFO_T;

typedef struct _tag_BTPairedRecNode
{
    BT_PAIREDRECORDINFO_T rPairRec;
    struct _tag_BTPairedRecNode *prNext;
    struct _tag_BTPairedRecNode *prPrev;
} BT_PAIREDRECORD_NODE_T, *P_BT_PAIREDRECORD_NODE_T;

typedef enum _tag_BT_Status
{
    MW_BT_STATUS_UNKNOWN,
    MW_BT_STATUS_ON,          // Bluetooth开启
    MW_BT_STATUS_OFF,         // Bluetooth关闭
    MW_NUM_OF_BT_STATUS
} E_BT_STATUS_T;


typedef enum _tag_BT_PairStatus
{
    BT_PAIR_STATUS_NOPAIRED,    // Bluetooth开启，但未配对
    BT_PAIR_STATUS_PAIRING,        // 正在配对
    BT_PAIR_STATUS_PAIRED,        // 已配对但未连接
  BT_PAIR_STATUS_PAIR_FAILED,
    NUM_OF_BT_PAIR_STATUS,
} E_BT_PAIR_STATUS_T;

typedef struct _tag_BT_RevokePinADDR
{
    BT_ADDR ba;
    struct _tag_BT_RevokePinADDR *prNext;
}E_BT_RevokePinADDR_T;


#endif /*_BZS_PAIRING_H_ */
