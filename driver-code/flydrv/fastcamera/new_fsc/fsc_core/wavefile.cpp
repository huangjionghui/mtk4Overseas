/********************************************************************************************
 *     LEGAL DISCLAIMER 
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES 
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED 
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS 
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, 
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY 
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, 
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK 
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION 
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *     
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH 
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, 
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE 
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
 *     
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS 
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.  
 ************************************************************************************************/

/******************************************************************************
*[File]                wavefile.cpp
*[Author]                  mtk40004
*[Description]
*    implementation for  wavefile class.    
******************************************************************************/

#include "wavefile.h"

BOOL g_bWaveLog = FALSE;

WaveFile::WaveFile()
{
   m_hWaveOut = NULL;
   m_prWaveHdr = NULL;
}


WaveFile::~WaveFile()
{
    if (NULL != m_prWaveHdr)
    {
        LocalFree(m_prWaveHdr);
    }
}


UINT32 WaveFile::Close()
{
    if (m_file && !m_fgIsRead)
    {
        fseek(m_file, 0, SEEK_SET);
        WriteHeader();
    }
    if (m_file)
        fclose(m_file);
    m_file = NULL;
    m_u4Pos = 0;
    m_u4Size = 0;
    return MMSYSERR_NOERROR;
}


UINT32 WaveFile::Open()
{
    WaveHeader rWave;
    if (m_file)
        return (MMSYSERR_ERROR);
    m_file = fopen(m_szFileName ,"rb" );
    if (!m_file)
        return (MMSYSERR_ERROR);
    if (sizeof(WaveHeader) == fread(&rWave, 1, sizeof(WaveHeader), m_file))
    {
        if (rWave.wFormatTag == WAVE_FORMAT_PCM)
        {
            m_u4Size = rWave.datasize;
            m_rFmt.nChannels=(WORD)rWave.wChannels;
            
            m_rFmt.wBitsPerSample=(WORD)rWave.nBitsPerSample;
            
            m_rFmt.nSamplesPerSec=rWave.dwSamplesPerSec;
            

            //m_rFmt.cbSize=rWave.
            m_rFmt.nAvgBytesPerSec=rWave.dwAvgBytesPerSec;
            m_rFmt.nBlockAlign=rWave.wBlockAlign;
            m_rFmt.wFormatTag=rWave.wFormatTag;
            
            m_u4Pos = sizeof(WaveHeader);
            m_fgIsRead = TRUE;
            RETAILMSG(g_bWaveLog, (TEXT("[WaveFile][Open] m_u4Pos=%d \r\n"),m_u4Pos));
            return (MMSYSERR_NOERROR);
        }
    }
    fclose(m_file);
    m_file = NULL;
    return MMSYSERR_ERROR;
}


UINT32 WaveFile::Create(WAVEFORMATEX *prFmt)
{
    if (m_file)
        return (MMSYSERR_ERROR);
    //m_rFmt.u4BitsPerSample = prFmt->u4BitsPerSample;
    //m_rFmt.u4Channels  = prFmt->u4Channels;
    //m_rFmt.u4SampleRate  = prFmt->u4SampleRate;
    m_rFmt.wBitsPerSample = prFmt->wBitsPerSample;
    m_rFmt.nChannels  = prFmt->nChannels;
    m_rFmt.nSamplesPerSec  = prFmt->nSamplesPerSec;

    m_file = fopen(m_szFileName ,"wb" );
    if (!m_file)
        return (MMSYSERR_ERROR);
    m_u4Size = 0;
    m_fgIsRead = FALSE;
    WriteHeader();
    m_u4Pos = sizeof(WaveHeader);
    return MMSYSERR_NOERROR;
}


UINT32 WaveFile::GetFormat(WAVEFORMATEX *prFmt)
{
    //prFmt->u4BitsPerSample = m_rFmt.u4BitsPerSample;
    //prFmt->u4Channels = m_rFmt.u4Channels ;
    //prFmt->u4SampleRate = m_rFmt.u4SampleRate;
    prFmt->wBitsPerSample = m_rFmt.wBitsPerSample;
    prFmt->nChannels  = m_rFmt.nChannels;
    prFmt->nSamplesPerSec  = m_rFmt.nSamplesPerSec;
    //prFmt->cbSize=m_rFmt.cbSize;
    prFmt->nAvgBytesPerSec=m_rFmt.nAvgBytesPerSec;
    prFmt->nBlockAlign=m_rFmt.nBlockAlign;
    prFmt->wFormatTag=m_rFmt.wFormatTag;

    return MMSYSERR_NOERROR;
}


UINT32 WaveFile::SetFileName(char *szFileName)
{
    strcpy(m_szFileName, szFileName);
	
    return MMSYSERR_NOERROR;
}


UINT32 WaveFile::GetDataSize()
{
    return m_u4Size;
}


UINT32 WaveFile::GetCurrentPosition()
{
    if (!m_file)
        return 0;
    return (m_u4Pos - sizeof(WaveHeader));
}


UINT32 WaveFile::SetCurrentPosition(UINT32 u4Pos)
{
    if (!m_file || ( u4Pos > m_u4Size) || !m_fgIsRead)
        return (MMSYSERR_ERROR);
    m_u4Pos = u4Pos + sizeof(WaveHeader);
    fseek(m_file, m_u4Pos, SEEK_SET);
    return MMSYSERR_NOERROR;
}


UINT32 WaveFile::ReadData(VOID * lpData, UINT32  u4Size)
{
    if (!m_file || !m_fgIsRead || (u4Size > m_u4Size))
        return 0;
    if ((m_u4Pos - sizeof(WaveHeader) + u4Size) > m_u4Size)
    {
        RETAILMSG(g_bWaveLog, (TEXT("[WaveFile][ReadData] 1m_u4Pos=%d \r\n"),m_u4Pos));
        m_u4Pos = sizeof(WaveHeader);
        fseek(m_file, m_u4Pos, SEEK_SET);
    }
    RETAILMSG(g_bWaveLog, (TEXT("[WaveFile][ReadData] 2m_u4Pos=%d ,u4Size=%d\r\n"),m_u4Pos,u4Size));
    //fseek(m_file, m_u4Pos, SEEK_SET);
    u4Size = fread(lpData, 1, u4Size, m_file);
    m_u4Pos += u4Size;
    RETAILMSG(g_bWaveLog, (TEXT("[WaveFile][ReadData] 3m_u4Pos=%d ,u4Size=%d\r\n"),m_u4Pos,u4Size));
    return (u4Size);
}


UINT32 WaveFile::WriteData(VOID * lpData, UINT32  u4Size)
{
    if (!m_file || m_fgIsRead)
        return 0;
    u4Size = fwrite((VOID *)((UINT32)lpData + u4Size), 1, u4Size, m_file);
    m_u4Pos += u4Size;
    m_u4Size += u4Size;
    
    return u4Size;
}

VOID WaveFile::WriteHeader()
{
/*
    WaveHeader wh;

    if (!m_file || m_fgIsRead)
        return;
    wh.riff[0] = 'R';
    wh.riff[1] = 'I';
    wh.riff[2] = 'F';
    wh.riff[3] = 'F';
    wh.filesize = m_u4Size+ 36;
    wh.wave[0] = 'W';
    wh.wave[1] = 'A';
    wh.wave[2] = 'V';
    wh.wave[3] = 'E';
    wh.fmt[0] = 'f';
    wh.fmt[1] = 'm';
    wh.fmt[2] = 't';
    wh.fmt[3] = ' ';
    wh.fmtsize = 0x10;
    wh.wFormatTag = 0x01;
    wh.wChannels = m_rFmt.u4Channels;
    wh.dwSamplesPerSec = m_rFmt.u4SampleRate;
    wh.dwAvgBytesPerSec = m_rFmt.u4SampleRate * m_rFmt.u4Channels * (m_rFmt.u4BitsPerSample / 8);
    wh.wBlockAlign = m_rFmt.u4BitsPerSample / 8  * m_rFmt.u4Channels;;
    wh.nBitsPerSample = m_rFmt.u4BitsPerSample;
    wh.data[0] = 'd';
    wh.data[1] = 'a';
    wh.data[2] = 't';
    wh.data[3] = 'a';
    wh.datasize = m_u4Size;

    fwrite(&wh, 1 , sizeof(WaveHeader), m_file);*/
}

void WaveFile::StartBackCarSound()
{
    RETAILMSG(g_bWaveLog, (TEXT("[HFP][WaveFile] StartRingSound Enter \r\n")));
    UINT32 hr;
    if (NULL != m_hWaveOut)
        return;
  
    SetFileName("\\fsc.wav");

    hr = Open();
    
    if (MMSYSERR_ERROR == hr)
    {
        RETAILMSG(g_bWaveLog, (TEXT("[HFP][WaveFile] Open ring file fail\r\n")));
        return;
    }
        
    UINT32 u4DataSize = GetDataSize();
    
    WAVEFORMATEX rFmt;
    
    GetFormat(&rFmt);
    RETAILMSG(g_bWaveLog, (TEXT("[HFP][INFO] AT_IND_RING: call sndPlaySound rFmt.cbSize=%d \r\n"), rFmt.cbSize));
    RETAILMSG(g_bWaveLog, (TEXT("[HFP][INFO] AT_IND_RING: call sndPlaySound rFmt.nAvgBytesPerSec=%d \r\n"), rFmt.nAvgBytesPerSec));
    RETAILMSG(g_bWaveLog, (TEXT("[HFP][INFO] AT_IND_RING: call sndPlaySound rFmt.nBlockAlign=%d \r\n"), rFmt.nBlockAlign));
    RETAILMSG(g_bWaveLog, (TEXT("[HFP][INFO] AT_IND_RING: call sndPlaySound rFmt.nChannels=%d \r\n"), rFmt.nChannels));
    RETAILMSG(g_bWaveLog, (TEXT("[HFP][INFO] AT_IND_RING: call sndPlaySound rFmt.nSamplesPerSec=%d \r\n"), rFmt.nSamplesPerSec));
    RETAILMSG(g_bWaveLog, (TEXT("[HFP][INFO] AT_IND_RING: call sndPlaySound rFmt.cbSize=%d \r\n"), rFmt.cbSize));
    RETAILMSG(g_bWaveLog, (TEXT("[HFP][INFO] AT_IND_RING: call sndPlaySound rFmt.wBitsPerSample=%d \r\n"), rFmt.wBitsPerSample));
    RETAILMSG(g_bWaveLog, (TEXT("[HFP][INFO] AT_IND_RING: call sndPlaySound rFmt.wFormatTag=%d \r\n"), rFmt.wFormatTag));
    
	char *buffer = NULL;
	buffer= (char*)malloc(u4DataSize + 1);

	if (NULL == buffer)
	{
		return;
	}
	memset(buffer, 0, u4DataSize + 1);

    ReadData(buffer, u4DataSize);
    hr = Close();
    if(hr != MMSYSERR_NOERROR)
    {
        RETAILMSG(g_bWaveLog, (TEXT("[HFP][WaveFile] Close ring file fail\r\n")));
        return;
    }
    RETAILMSG(g_bWaveLog, (TEXT("[HFP][INFO] AT_IND_RING: call u4DataSize=%d. \r\n"), u4DataSize));
    
    RETAILMSG(g_bWaveLog, (TEXT("[HFP][INFO] AT_IND_RING: call sndPlaySound 11. \r\n")));
    
    if (NULL == m_hWaveOut)
        waveOutOpen(&m_hWaveOut, 0, &rFmt, NULL, NULL,CALLBACK_NULL);
    
    if (NULL == m_hWaveOut)
    {
        RETAILMSG(g_bWaveLog, (TEXT("[HFP][WaveFile] StartRingSound m_hWaveOut is NULL\r\n")));
        return;
    }
        
    RETAILMSG(g_bWaveLog, (TEXT("[HFP][INFO] AT_IND_RING: call sndPlaySound 2. \r\n")));
    if (NULL == m_prWaveHdr)
    {
        m_prWaveHdr = (WAVEHDR *)LocalAlloc(LPTR, sizeof(WAVEHDR));
        if (NULL != m_prWaveHdr)
            memset(m_prWaveHdr, 0, sizeof(WAVEHDR));
    }
    if (NULL == m_prWaveHdr)
    {
        RETAILMSG(g_bWaveLog, (TEXT("[HFP][WaveFile][ERR] fail to alloc WaveHdr\r\n")));
        waveOutClose(m_hWaveOut);
        m_hWaveOut = NULL;
        return;
    }
        
    RETAILMSG(g_bWaveLog, (TEXT("[HFP][INFO] AT_IND_RING: call sndPlaySound 3. \r\n")));
    m_prWaveHdr->dwBufferLength = u4DataSize;
    m_prWaveHdr->lpData         = (char *)buffer;
    m_prWaveHdr->dwFlags        = WHDR_BEGINLOOP|WHDR_ENDLOOP;
    m_prWaveHdr->dwLoops        = 1; 
    
    waveOutPrepareHeader(m_hWaveOut, m_prWaveHdr, sizeof(WAVEHDR));
    RETAILMSG(g_bWaveLog, (TEXT("[HFP][INFO] AT_IND_RING: call sndPlaySound 4. \r\n")));

    waveOutWrite(m_hWaveOut, m_prWaveHdr, sizeof(WAVEHDR));
    RETAILMSG(g_bWaveLog, (TEXT("[HFP][INFO] AT_IND_RING: call sndPlaySound 5. \r\n")));
}

void WaveFile::StopBackCarSound()
{
    RETAILMSG(g_bWaveLog, (TEXT("[HFP][WaveFile] StopRingSound Enter \r\n")));
    if (NULL == m_hWaveOut)
    {
        RETAILMSG(g_bWaveLog, (TEXT("[HFP][WaveFile] StopRingSound m_hWaveOut is NULL\r\n")));
        return;
    }
        
    waveOutReset(m_hWaveOut);
    waveOutClose(m_hWaveOut);
    m_hWaveOut = NULL;
    if (NULL != m_prWaveHdr)
    {
        if (m_prWaveHdr->lpData != NULL)
		{
			free(m_prWaveHdr->lpData);
			m_prWaveHdr->lpData = NULL;
		}
        LocalFree(m_prWaveHdr);
        m_prWaveHdr = NULL;
    }
}


