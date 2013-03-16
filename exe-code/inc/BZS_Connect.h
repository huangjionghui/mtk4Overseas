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


#ifndef _BZS_CONNECT_H_
#define _BZS_CONNECT_H_
#if 0
typedef enum _tag_BT_ConnectStatus
{
    BT_CONNECT_STATUS_BTCLOSED,        // Bluetooth closed
    BT_CONNECT_STATUS_NOPAIRED,        // Bluetooth opened, but no paired devices
    BT_CONNECT_STATUS_PAIRING,        // Pairing with one remote device
    BT_CONNECT_STATUS_PAIRED,        // Paired with some remote devices, but no connection
    BT_CONNECT_STATUS_DISCONNECTING,// Disconnecting with one remote device
    BT_CONNECT_STATUS_CONNECTING,    // Connecting with one remote device
    BT_CONNECT_STATUS_CONNECTED,    // Connected with one remote device
    NUM_OF_BT_CONNECT_STATUS
} E_BT_CONNECT_STATUS_T;
#endif

typedef enum _tag_BT_ConnectStatus
{
    BT_CONNECT_STATUS_UNCONNECT=1,    // Connecting with one remote device
    BT_CONNECT_STATUS_CONNECTING,    
    BT_CONNECT_STATUS_CONNECTED,    // Connected with one remote device
    BT_CONNECT_STATUS_DISCONNECTING,    
    NUM_OF_BT_CONNECT_STATUS
} E_BT_CONNECT_STATUS_T;




typedef enum
{
    BTCS_PAIRED = 1 << 0,
    BTCS_A2DP = 1 << 1,
    BTCS_HFP = 1 << 2,
    BTCS_AVRCP = 1 << 3,
    BTCS_DUN = 1 << 4,
    BTCS_HSPG = 1 << 5,
    BTCS_PBAP = 1 << 6
};


#endif /* _BZS_CONNECT_H_ */

