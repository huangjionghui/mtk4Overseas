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
 * $Author: zeng.zhang $ 
 *
 * $Modtime: $  
 
 * $Revision: #1 $
****************************************************************************/
#ifndef	_HFMANAGER_WAVEFILE_H_
#define _HFMANAGER_WAVEFILE_H_

#include <windows.h>
//#include "oemsettings.h"

#pragma pack(push, 1)
typedef struct {
    BYTE     riff[4];          // "RIFF"
    UINT32   filesize;         // File size - 8
    BYTE     wave[4];          // "WAVE"
    BYTE     fmt[4];           // "fmt "
    UINT32   fmtsize;          // 0x10
    UINT16   wFormatTag;       // 0x01 -> PCM
    UINT16   wChannels;        // Channels
    UINT32   dwSamplesPerSec;  // Sampling Rate (samples per second)
    UINT32   dwAvgBytesPerSec; // Average Bytes per second
    UINT16   wBlockAlign;
    UINT16   nBitsPerSample;   // Bits per sample
    BYTE     data[4];          // "data"
    UINT32   datasize;         // Data Size(Byte)
} WaveHeader;

#pragma pack (pop)

class WaveFile 
{
public:
    WaveFile();
    ~WaveFile();
    
    void StartBackCarSound();
    void StopBackCarSound();

protected:
    UINT32 Close();
    UINT32 Open();
    UINT32 Create(WAVEFORMATEX *prFmt);
    UINT32 GetFormat(WAVEFORMATEX *prFmt);
    UINT32 SetFileName(char *szFileName);
    UINT32 GetDataSize();
    UINT32 GetCurrentPosition();
    UINT32 SetCurrentPosition(UINT32 u4Pos);
    UINT32 ReadData(VOID * lpData, UINT32  u4Size);
    UINT32 WriteData(VOID * lpData, UINT32  u4Size);
    
private:
    
    VOID WriteHeader();
    
    WAVEFORMATEX m_rFmt;
    FILE         *m_file;
    char         m_szFileName[125];
    UINT32       m_u4Pos;
    UINT32       m_u4Size;
    BOOL         m_fgIsRead;
    HWAVEOUT     m_hWaveOut;
    WAVEHDR      *m_prWaveHdr;
};


#endif // _HFMANAGER_WAVEFILE_H_


