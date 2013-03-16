#include "FLY6624Radio.h"

/*********************************************************************************************************/

/*author:add by huangjionghui
 *
 *6624自带的RDS
 *2012-12-26
 */

//Read address
#define RDS_READ_DATA_START    0x00
#define RDS_READ_STATUS        0x00
#define RDS_READ_LEVEL         0x01
#define RDS_READ_USN_WAM       0x02
#define RDS_READ_IFCOUNTER     0x03
#define RDS_READ_ID            0x04
#define RDS_READ_RDS_STATUS    0x05
#define RDS_READ_RDS_DTA3	   0x06
#define RDS_READ_RDS_DTA2	   0x07
#define RDS_READ_RDS_DTA1	   0x08
#define RDS_READ_RDS_DTA0	   0x09
#define RDS_READ_RDS_DATEE     0x0A
#define RDS_READ_DATA_END      11


#define ZERO(a)					memset(a,0,sizeof(a))
#define GET_RT_GROUP(buf)      (buf[3]>>4) & 0x01
#define GET_SEG_RT(buf)        buf[3] & 0x0f
#define GET_SEG_PS_DI_AF(buf)  buf[3] & 0x03
#define GET_GROUP_TYPE(buf)    (buf[2]>>4) & 0x0f
#define GET_G_AB(buf)          (buf[2]>>3) & 0x01

void RDS_ReturnPS(BYTE *p)
{
	BYTE buff[9]={0x21,0x00};
	memcpy(&buff[1],p,8);
	ReturnToUser(buff,9);
}
void RDS_ReturnRT(BYTE *p,BYTE len)
{
	BYTE buff[1+128+2]={0x22,0x00};
	memcpy(&buff[1],p,len);
	ReturnToUser(buff,1+len);
}

void RDS_ReturnPTY(BYTE PTY)
{
	BYTE buff[2] = {0x23,0x00};
	buff[1] = PTY;
	ReturnToUser(buff,2);
}
void RDS_ReturnTP(BYTE TP)
{
	BYTE buff[2] = {0x25,0x00};
	buff[1] = TP;
	ReturnToUser(buff,2);
}

void RDS_ReturnTA(BYTE TA)
{
	BYTE buff[2] = {0x26,0x00};
	buff[1] = TA;
	ReturnToUser(buff,2);
}

static void RDS_ReadDataFromDataSheet(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo, BYTE *p, UINT16 len)
{
	I2C_Read_TEF6624(pFlyRadioInfo, RDS_READ_DATA_START, p, len);
}


static BOOL GetChannelSignalquality(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BYTE noise, BYTE level)
{
	UINT16 iSignalLevel,iSignalNoise;

	iSignalLevel = level;
	iSignalNoise = (noise&0xF0) >> 4;

	DBGD((_T("\r\nTEF6624 return RDS signal quality level:%d, noise:%d"),iSignalLevel,iSignalNoise));
	
	if (AM != pFlyRadioInfo->radioInfo.eCurRadioMode){
		if ((iSignalLevel < TEF6624_STOP_FM_LEVEL) || (iSignalNoise > TEF6624_STOP_FM_NOISE))
			return FALSE;
	}else {	
		if ((iSignalLevel < TEF6624_STOP_AM_LEVEL) || (iSignalNoise > TEF6624_STOP_AM_NOISE))
			return FALSE;
	}

	return TRUE;
}

BOOL RDS_ReadRadioRDSStatus(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BYTE status)
{
	return status&0x80;
}

BOOL RDS_ReadRadioRDSDatee(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BYTE iDatee)
{
	//DBGD((TEXT("\r\n RDS read radio datee:%d"),iDatee&0x0f));

	if ((iDatee & 0X0F) > 5)
		return FALSE;
	
	return TRUE;
}

UINT32 RDS_ReadRadioRDSDatas(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BYTE *buf, UINT len)
{	
	UINT32 datas = 0;
	
	datas = buf[0];
	datas = (datas<<8)+buf[1];
	datas = (datas<<8)+buf[2];
	datas = (datas<<8)+buf[3];
	
	//DBGD((TEXT("\r\n RDS data3:%x, data2:%x, data1:%x,data0:%x datas:%x"),buf[0],buf[1],buf[2],buf[3],datas));
	return datas;
}

BOOL RDS_AddAf2List(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo, BYTE iFreq)
{
	BYTE i;
	UINT16 freq = iFreq+875;

	if (freq > 1080 || freq < 875)
		return FALSE;

	if (pFlyRadioInfo->sTEF6624RDSInfo.sAfInfo.aflist_count >= 25)
		return FALSE;

	if (!pFlyRadioInfo->sTEF6624RDSInfo.sAfInfo.aflist_count)
		pFlyRadioInfo->sTEF6624RDSInfo.sAfInfo.aflist[pFlyRadioInfo->sTEF6624RDSInfo.sAfInfo.aflist_count++] = freq;
	else
	{
		for (i=0; i<pFlyRadioInfo->sTEF6624RDSInfo.sAfInfo.aflist_count; i++)
		{
			if (pFlyRadioInfo->sTEF6624RDSInfo.sAfInfo.aflist[i] == freq)
				return FALSE;
		}

		pFlyRadioInfo->sTEF6624RDSInfo.sAfInfo.aflist[pFlyRadioInfo->sTEF6624RDSInfo.sAfInfo.aflist_count++] = freq;
	}

	return TRUE;
}

BOOL RDS_GetCheckUpdataPI(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
	UINT16 iTemp = pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][0] << 8 
		| pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][1];
	
	if (pFlyRadioInfo->sTEF6624RDSInfo.iNewPICode != iTemp)
		pFlyRadioInfo->sTEF6624RDSInfo.iNewPICode = iTemp;

	if (!pFlyRadioInfo->sTEF6624RDSInfo.iBasePICode)//确定基准PI
	{
		pFlyRadioInfo->sTEF6624RDSInfo.iBasePICode = iTemp;
		RDS_AddAf2List(pFlyRadioInfo, pFlyRadioInfo->sTEF6624RDSInfo.iBaseFreq);
	}

	if ((pFlyRadioInfo->sTEF6624RDSInfo.iBasePICode != pFlyRadioInfo->sTEF6624RDSInfo.iNewPICode) || (!pFlyRadioInfo->sTEF6624RDSInfo.iBasePICode))
		return FALSE;

	return TRUE;
}
void RDS_GetUpdataPTY(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
	BYTE iTemp = (pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][2] &0x03) << 3
		| pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][3] >> 5;
	
	if (pFlyRadioInfo->sTEF6624RDSInfo.iProgranType != iTemp)
	{
		pFlyRadioInfo->sTEF6624RDSInfo.iProgranType = iTemp;
		RDS_ReturnPTY(iTemp);
	}
}
void RDS_GetUpdataTP(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
	BOOL bswitch  = (pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][2] >> 2) & 0x01;    
	
	if(pFlyRadioInfo->sTEF6624RDSInfo.bTrafficProgramIDTuned != bswitch)
	{
		pFlyRadioInfo->sTEF6624RDSInfo.bTrafficProgramIDTuned = bswitch;
		RDS_ReturnTP(bswitch);
	}
	
	if (14 == pFlyRadioInfo->sTEF6624RDSInfo.iRdsGroupType)
	{
		bswitch =(pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][3] >> 4) & 0x01;

		if(pFlyRadioInfo->sTEF6624RDSInfo.bTrafficProgramIDOther != bswitch)
			pFlyRadioInfo->sTEF6624RDSInfo.bTrafficProgramIDOther = bswitch;
	}
}
void RDS_GetUpdataMS(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
	BOOL bswitch = 
		(pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][3] >> 3) & 0x01;

	if(pFlyRadioInfo->sTEF6624RDSInfo.bMusicSpeechSwitch != bswitch)
	{
		pFlyRadioInfo->sTEF6624RDSInfo.bMusicSpeechSwitch = bswitch;
	}
}

void RDS_GetUpdataDI(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
	BOOL benable = 
		(pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][3] >> 2) & 0x01;

	if (0 == pFlyRadioInfo->sTEF6624RDSInfo.iRdsSegmentPSDiAf)
	{
		pFlyRadioInfo->sTEF6624RDSInfo.iRdsDISegment = 0;
	}

	if(pFlyRadioInfo->sTEF6624RDSInfo.iRdsDISegment == pFlyRadioInfo->sTEF6624RDSInfo.iRdsSegmentPSDiAf)
	{
		if(pFlyRadioInfo->sTEF6624RDSInfo.iTempDI[pFlyRadioInfo->sTEF6624RDSInfo.iRdsSegmentPSDiAf] != benable)
		{

			pFlyRadioInfo->sTEF6624RDSInfo.iTempDI[pFlyRadioInfo->sTEF6624RDSInfo.iRdsSegmentPSDiAf] = benable;
		}

		if (3 == pFlyRadioInfo->sTEF6624RDSInfo.iRdsSegmentPSDiAf)
		{
			pFlyRadioInfo->sTEF6624RDSInfo.iDecoderID = ((pFlyRadioInfo->sTEF6624RDSInfo.iTempDI[0] << 3) | (pFlyRadioInfo->sTEF6624RDSInfo.iTempDI[1] << 2) | (pFlyRadioInfo->sTEF6624RDSInfo.iTempDI[2] << 1) | (pFlyRadioInfo->sTEF6624RDSInfo.iTempDI[3] << 0));
		}

		pFlyRadioInfo->sTEF6624RDSInfo.iRdsDISegment++;
	}
}

void RDS_GetUpdataTA(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)	 
{
	BOOL flag;

	if (0 == pFlyRadioInfo->sTEF6624RDSInfo.iRdsGroupType)
	{
		flag = ((pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][3] >> 4) & 0x01);

		if(pFlyRadioInfo->sTEF6624RDSInfo.bTrafficAnnouncementIDTuned != flag)
		{
			pFlyRadioInfo->sTEF6624RDSInfo.bTrafficAnnouncementIDTuned = flag;
			RDS_ReturnTA(flag);
		
			//if (pFlyRadioInfo->sTEF6624RDSInfo.Traffic_Program_id_Tuned && pFlyRadioInfo->sTEF6624RDSInfo.bTrafficAnnouncementIDTuned)
			//{
			//	pFlyAllInOneInfo->pMemory_Share_Common->bRadioRDS_Traffic_Broadcast = TRUE;
			//}
			//else
			//{
			//	pFlyAllInOneInfo->pMemory_Share_Common->bRadioRDS_Traffic_Broadcast = FALSE;
			//}

			//ipcStartEvent(EVENT_RADIO_RDS_TRAFFIC_BOARDCAST_ID);
		}
	}
	else if((14 == pFlyRadioInfo->sTEF6624RDSInfo.iRdsGroupType) && (GROUP_B == pFlyRadioInfo->sTEF6624RDSInfo.iRdsGroup))
	{
		flag = ((pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][3] >> 3) & 0x01);

		if(pFlyRadioInfo->sTEF6624RDSInfo.bTrafficProgramIDOther != flag)
		{
			pFlyRadioInfo->sTEF6624RDSInfo.bTrafficProgramIDOther = flag;
		}
	}
}

void RDS_GetUpdataPS(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
	BYTE tempGroup = pFlyRadioInfo->sTEF6624RDSInfo.iRdsGroup;

	if (0 == pFlyRadioInfo->sTEF6624RDSInfo.iRdsSegmentPSDiAf)
	{
		pFlyRadioInfo->sTEF6624RDSInfo.cRdsPSSegment[tempGroup] = 0;
	}

	if(pFlyRadioInfo->sTEF6624RDSInfo.cRdsPSSegment[tempGroup] == pFlyRadioInfo->sTEF6624RDSInfo.iRdsSegmentPSDiAf)
	{
		pFlyRadioInfo->sTEF6624RDSInfo.iPreProgramServiceName[tempGroup][pFlyRadioInfo->sTEF6624RDSInfo.iRdsSegmentPSDiAf * 2 + 0] = pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][6];
		pFlyRadioInfo->sTEF6624RDSInfo.iPreProgramServiceName[tempGroup][pFlyRadioInfo->sTEF6624RDSInfo.iRdsSegmentPSDiAf * 2 + 1] = pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][7];

		if (3 == pFlyRadioInfo->sTEF6624RDSInfo.iRdsSegmentPSDiAf)
		{
			if (0 != memcmp(pFlyRadioInfo->sTEF6624RDSInfo.iPreProgramServiceName[tempGroup], pFlyRadioInfo->sTEF6624RDSInfo.iCurProgramServiceName[tempGroup], 8))
			{
				memcpy(pFlyRadioInfo->sTEF6624RDSInfo.iCurProgramServiceName[tempGroup], pFlyRadioInfo->sTEF6624RDSInfo.iPreProgramServiceName[tempGroup], 8);
			}

			RDS_ReturnPS(pFlyRadioInfo->sTEF6624RDSInfo.iCurProgramServiceName[tempGroup]);	
		}

		pFlyRadioInfo->sTEF6624RDSInfo.cRdsPSSegment[tempGroup]++;
	}
}

void RDS_GetUpdataRT(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
	BYTE tempGroup;
	BYTE tempRT_G;
	BYTE temp_RT_SEG;
	BYTE temp,i;

	tempGroup   = pFlyRadioInfo->sTEF6624RDSInfo.iRdsGroup;
	tempRT_G    = GET_RT_GROUP(pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx]);
	temp_RT_SEG = GET_SEG_RT(pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx]);

	if (GROUP_A == tempGroup)
	{
		if (0 == temp_RT_SEG)
			pFlyRadioInfo->sTEF6624RDSInfo.iRdsRTSegmentA = 0;

		if (tempRT_G != pFlyRadioInfo->sTEF6624RDSInfo.iRdsPT_A_G)
		{
			pFlyRadioInfo->sTEF6624RDSInfo.iRdsPT_A_G = tempRT_G;
			pFlyRadioInfo->sTEF6624RDSInfo.iRdsRTSegmentA = 0;
		}
	}
	else if (GROUP_B == tempGroup)
	{
		if (0 == temp_RT_SEG)
		{
			pFlyRadioInfo->sTEF6624RDSInfo.iRdsRTSegmentB = 0;
		}
	}

	if (GROUP_A == tempGroup)
	{
		if (pFlyRadioInfo->sTEF6624RDSInfo.iRdsRTSegmentA == temp_RT_SEG)
		{
			temp = temp_RT_SEG * 4;
			for (i = 0; i<4; i++)
			{
				pFlyRadioInfo->sTEF6624RDSInfo.cPreRadioTextA[temp + i] = pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][4 + i];
				if ((0x0D == pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][4 + i]) || (temp + i == 63))
				{
					if (0 != memcmp(pFlyRadioInfo->sTEF6624RDSInfo.cPreRadioTextA, pFlyRadioInfo->sTEF6624RDSInfo.cCurRadioTextA, temp + i + 1))
					{
						memcpy(pFlyRadioInfo->sTEF6624RDSInfo.cCurRadioTextA, pFlyRadioInfo->sTEF6624RDSInfo.cPreRadioTextA, temp + i + 1);
						RDS_ReturnRT(pFlyRadioInfo->sTEF6624RDSInfo.cCurRadioTextA,temp + i + 1);
					}
				}
			}

			pFlyRadioInfo->sTEF6624RDSInfo.iRdsRTSegmentA++;
		}
	}
	else if (GROUP_B == tempGroup)
	{
		if (pFlyRadioInfo->sTEF6624RDSInfo.iRdsRTSegmentB == temp_RT_SEG)
		{
			temp = temp_RT_SEG * 2;
			for (i = 0; i<2; i++)
			{
				pFlyRadioInfo->sTEF6624RDSInfo.cPreRadioTextB[temp + i] = pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][6 + i];
				if ((0x0D == pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][6 + i]) || (temp + i == 31))
				{
					if (0 != memcmp(pFlyRadioInfo->sTEF6624RDSInfo.cPreRadioTextB, pFlyRadioInfo->sTEF6624RDSInfo.cCurRadioTextB, temp + i + 1))
					{
						memcpy(pFlyRadioInfo->sTEF6624RDSInfo.cCurRadioTextB, pFlyRadioInfo->sTEF6624RDSInfo.cPreRadioTextB, temp + i + 1);
						RDS_ReturnRT(pFlyRadioInfo->sTEF6624RDSInfo.cCurRadioTextB,temp + i + 1);
					}
				}
			}

			pFlyRadioInfo->sTEF6624RDSInfo.iRdsRTSegmentB++;
		}
	}
}

void RDS_GetUpdataDate(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
	int tempdate,temphour,tempmin,tempofs;
	int yd,md,d,y,m;

	if(pFlyRadioInfo->sTEF6624RDSInfo.iRdsGroup == GROUP_A)
	{
		tempdate = ((pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][3]&3)<<15) | (pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][4]<<7) | (pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][5]>>1);
		temphour = ((pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][5]&1)<<4)  | ((pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][6]>>4));
		tempmin  = ((pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][6]&0xf)<<2)| ((pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][7]>>6));
		tempofs  = (pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][7]&0x3f);

		yd = (int)(((double)tempdate-15078.2)/365.25);
		md = (int)(((((double)tempdate-14956.1)-((double)yd*365.25)))/30.6001);
		d = (int)(tempdate-14956-((double)yd*365.25)-((double)md*30.6001));

		if(md==14 || md==15) {
			y = yd + 1;
			m = md -13;
		} else {
			y = yd;
			m = md -1;
		}

		pFlyRadioInfo->sTEF6624RDSInfo.sDateInfo.rds_year = 1900 + y;
		pFlyRadioInfo->sTEF6624RDSInfo.sDateInfo.rds_month= m;
		pFlyRadioInfo->sTEF6624RDSInfo.sDateInfo.rds_day  = d + 1;
		pFlyRadioInfo->sTEF6624RDSInfo.sDateInfo.rds_weekday = (BYTE)(((long)tempdate +2)%7 + 1); 

		pFlyRadioInfo->sTEF6624RDSInfo.sDateInfo.rds_hour = temphour;
		pFlyRadioInfo->sTEF6624RDSInfo.sDateInfo.rds_min  = tempmin;
		pFlyRadioInfo->sTEF6624RDSInfo.sDateInfo.rds_ofs  = tempofs;

		//if(date != 0)
		//{
		//	printfRDS_Date();
		//}
	}
}
void RDS_RdsDecPrc0x(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BYTE Group)
{
	pFlyRadioInfo->sTEF6624RDSInfo.iRdsSegmentPSDiAf = GET_SEG_PS_DI_AF(pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx]);

	RDS_GetUpdataMS(pFlyRadioInfo);
	RDS_GetUpdataDI(pFlyRadioInfo);
	RDS_GetUpdataTA(pFlyRadioInfo);

	if(Group == GROUP_A)
	{
		RDS_AddAf2List(pFlyRadioInfo,pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][4]);
		RDS_AddAf2List(pFlyRadioInfo,pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx][5]);
	}

	RDS_GetUpdataPS(pFlyRadioInfo);
}

void RDS_RdsDecPrc2x(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BYTE Group)
{
	RDS_GetUpdataRT(pFlyRadioInfo);
}

void RDS_Rds_DecPrc4x(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo,BYTE Group)
{
	RDS_GetUpdataDate(pFlyRadioInfo);
}

void RDS_ParseRdsDataProcess(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
	pFlyRadioInfo->sTEF6624RDSInfo.iRdsGroup = 
		GET_G_AB(pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx]);
	pFlyRadioInfo->sTEF6624RDSInfo.iRdsGroupType = 
		GET_GROUP_TYPE(pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx]);
		
	if (RDS_GetCheckUpdataPI(pFlyRadioInfo))
	{
		RDS_GetUpdataPTY(pFlyRadioInfo);
		RDS_GetUpdataTP(pFlyRadioInfo);
		
		//DBGD((TEXT("\r\nRDS group type:%d"),pFlyRadioInfo->sTEF6624RDSInfo.iRdsGroupType));
		switch (pFlyRadioInfo->sTEF6624RDSInfo.iRdsGroupType)
		{
		case 0:
			RDS_RdsDecPrc0x(pFlyRadioInfo, pFlyRadioInfo->sTEF6624RDSInfo.iRdsGroup);
			break;
		
		case 2:
			RDS_RdsDecPrc2x(pFlyRadioInfo, pFlyRadioInfo->sTEF6624RDSInfo.iRdsGroup);
			break;

		case 4:
			RDS_Rds_DecPrc4x(pFlyRadioInfo, pFlyRadioInfo->sTEF6624RDSInfo.iRdsGroup);
			break;

		default:
			break;
		}
	}
}

static BYTE RDS_OddCheck(BYTE data)
{		  
	BYTE i;
	BYTE p = 0;
	for(i=0;i<8;i++)
	{
		if(data&(1<<(i)))
			p++;
	}
	return p%2;
}

static UINT16 RDS_CacluSyncCode(UINT16 rds_raw_data)
{
	BYTE bCFlag = 1;
	BYTE TempH, TempL, ACC;

	UINT16 crc = 0;

	TempH = rds_raw_data / 0x100;
	TempL = rds_raw_data % 256;

	//C9
	ACC = TempH & 0x7c;
	bCFlag = RDS_OddCheck(ACC);
	ACC = TempL & 0x3e;
	bCFlag ^= RDS_OddCheck(ACC);
	if(bCFlag) crc = 0x200;

	//C8
	ACC = TempH & 0x3e;
	bCFlag = RDS_OddCheck(ACC);				
	ACC = TempL & 0x1f;
	bCFlag ^= RDS_OddCheck(ACC);
	if(bCFlag) crc |= 0x100;

	//C7
	ACC = TempH & 0x63;
	bCFlag = RDS_OddCheck(ACC);
	ACC = TempL & 0x31;
	bCFlag ^= RDS_OddCheck(ACC);		
	if(bCFlag) crc |= 0x80;

	//C6
	ACC = TempH & 0xcd;
	bCFlag = RDS_OddCheck(ACC);
	ACC = TempL & 0xa6;
	bCFlag ^= RDS_OddCheck(ACC);
	if(bCFlag)crc |= 0x40;

	//C5
	ACC = TempH & 0xe6;
	bCFlag = RDS_OddCheck(ACC);
	ACC = TempL & 0xd3;
	bCFlag ^= RDS_OddCheck(ACC);
	if(bCFlag)crc |= 0x20;

	//C4
	ACC = TempH & 0x8f;
	bCFlag = RDS_OddCheck(ACC);
	ACC = TempL & 0x57;
	bCFlag ^= RDS_OddCheck(ACC);
	if(bCFlag) crc |= 0x10;

	//C3
	ACC = TempH & 0x3b;
	bCFlag = RDS_OddCheck(ACC);			 
	ACC = TempL & 0x95;
	bCFlag ^= RDS_OddCheck(ACC);
	if(bCFlag)crc |= 0x08;

	//C2
	ACC = TempH & 0xe1;
	bCFlag = RDS_OddCheck(ACC);
	ACC = TempL & 0xf4;
	bCFlag ^= RDS_OddCheck(ACC);
	if(bCFlag)crc |= 0x04;

	//C1
	ACC = TempH & 0xf0;
	bCFlag = RDS_OddCheck(ACC);
	ACC = TempL & 0xfa;
	bCFlag ^= RDS_OddCheck(ACC);
	if(bCFlag) crc |= 0x02;

	//C0
	ACC = TempH & 0xf8;
	bCFlag = RDS_OddCheck(ACC);
	ACC = TempL & 0x7d;
	bCFlag ^= RDS_OddCheck(ACC);
	if(bCFlag) crc |= 0x01;

	return crc;
}


void RDS_InsertRdsBuff(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo, BOOL bEnd, BYTE ibyte, UINT16 data)
{
	pFlyRadioInfo->sTEF6624RDSInfo.cRdsDataBuff[\
		pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataHx][ibyte] =  (BYTE)data;

	if (bEnd)
	{
		pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataHx++;
		if (pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataHx > RDS_DATA_LEN-1)
			pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataHx=0;
	}

	//DBGD((TEXT("\r\n RDS insert buff Hx:%d"),pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataHx));
}

void RDS_ParseRdsDatas(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo, UINT32 iDatas)
{
	UINT16 i=0;
	UINT16 nSynCode = 0;

	for (i=32; i>0; i--)
	{

		//DBGD((TEXT("\r\n RDS parse data BitCnt:%d crc:%d Step:%d "),\
		//	pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataBitCnt, \
		//	pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataCRC, \
		//	pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataStep));


		pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo <<= 1;
		if (pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataCRC & 0x0200)
		{
			pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo |= 0x0001;
		}

		pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataCRC <<= 1;
		if (iDatas & (1<<(i-1)))
		{
			pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataCRC |= 0x0001;
		}

		pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataCRC &= 0x03FF;
		pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataBitCnt++;

		if (!pFlyRadioInfo->sTEF6624RDSInfo.bRdsDataSyn)
		{
			nSynCode  = RDS_CacluSyncCode(pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo);
			nSynCode ^= pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataCRC;

			if (nSynCode == OFFSET_A)
			{
				//DBGD((TEXT("\r\n RDS parse data datainfo:%d crc:%d"),\
				//	pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo, \
				//	pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataCRC));

				pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataStep   = 1;
				pFlyRadioInfo->sTEF6624RDSInfo.bRdsDataSyn    = TRUE;
				pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataBitCnt = 0;
				
				RDS_InsertRdsBuff(pFlyRadioInfo,FALSE,0,pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo>>8);
				RDS_InsertRdsBuff(pFlyRadioInfo,FALSE,1,pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo);
			}
		}
		else if (26 == pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataBitCnt)
		{
			pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataBitCnt = 0;
			nSynCode  = RDS_CacluSyncCode(pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo);
			nSynCode ^= pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataCRC;

			if ((OFFSET_A == nSynCode) && (0 == pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataStep))
			{
				pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataStep   = 1;
				RDS_InsertRdsBuff(pFlyRadioInfo,FALSE,0,pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo>>8);
				RDS_InsertRdsBuff(pFlyRadioInfo,FALSE,1,pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo);
			}
			else if ((OFFSET_B == nSynCode) && (1 == pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataStep))
			{
				pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataStep   = 2;
				RDS_InsertRdsBuff(pFlyRadioInfo,FALSE,2,pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo>>8);
				RDS_InsertRdsBuff(pFlyRadioInfo,FALSE,3,pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo);
			}
			else if ((OFFSET_C == nSynCode) && (2 == pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataStep))
			{
				pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataStep   = 3;
				RDS_InsertRdsBuff(pFlyRadioInfo,FALSE,4,pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo>>8);
				RDS_InsertRdsBuff(pFlyRadioInfo,FALSE,5,pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo);
			}
			else if ((OFFSET_C2 == nSynCode) && (2 == pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataStep))
			{
				pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataStep   = 3;
				RDS_InsertRdsBuff(pFlyRadioInfo,FALSE,4,pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo>>8);
				RDS_InsertRdsBuff(pFlyRadioInfo,FALSE,5,pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo);
			}
			else if ((OFFSET_D == nSynCode) && (3 == pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataStep))
			{
				pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataStep   = 0;
				RDS_InsertRdsBuff(pFlyRadioInfo,FALSE,6,pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo>>8);
				RDS_InsertRdsBuff(pFlyRadioInfo,TRUE,7,pFlyRadioInfo->sTEF6624RDSInfo.nRdsDataInfo);
			}
			else
			{
				pFlyRadioInfo->sTEF6624RDSInfo.bRdsDataSyn    = FALSE;
			}
		}
	}
}

static DWORD WINAPI TEF6624RDSMainThreadProc(LPVOID pContext)
{
	DWORD dwWaitTimer = INFINITE;
	BYTE rds_buf[RDS_READ_DATA_END];

	P_FLYRADIO_TEF6624_INFO pFlyRadioInfo = (P_FLYRADIO_TEF6624_INFO)pContext;
	pFlyRadioInfo->sTEF6624RDSInfo.iCurRadioMode = ~0;
	pFlyRadioInfo->sTEF6624RDSInfo.iCurRadioFreq = ~0;
	pFlyRadioInfo->sTEF6624RDSInfo.iTickTimes = 0;
	
	while (!pFlyRadioInfo->sTEF6624RDSInfo.bKillDispatchRDSThread)
	{
		WaitForSingleObject(pFlyRadioInfo->sTEF6624RDSInfo.hDispatchRDSThreadEvent, dwWaitTimer);
		
		if (pFlyRadioInfo->radioInfo.eCurRadioMode != pFlyRadioInfo->sTEF6624RDSInfo.iCurRadioMode
			|| *pFlyRadioInfo->radioInfo.pCurRadioFreq != pFlyRadioInfo->sTEF6624RDSInfo.iCurRadioFreq)
		{
			pFlyRadioInfo->sTEF6624RDSInfo.iCurRadioMode = pFlyRadioInfo->radioInfo.eCurRadioMode;
			pFlyRadioInfo->sTEF6624RDSInfo.iCurRadioFreq = *pFlyRadioInfo->radioInfo.pCurRadioFreq;
			dwWaitTimer = 500;
			pFlyRadioInfo->sTEF6624RDSInfo.iTickTimes = GetTickCount();
		}
		
		//if (GetTickCount() - pFlyRadioInfo->sTEF6624RDSInfo.iTickTimes > 10)
		if (pFlyRadioInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eAudioInput == RADIO)
		{
			//台稳定了
			if (pFlyRadioInfo->radioInfo.eCurRadioMode == pFlyRadioInfo->sTEF6624RDSInfo.iCurRadioMode
				&& *pFlyRadioInfo->radioInfo.pCurRadioFreq == pFlyRadioInfo->sTEF6624RDSInfo.iCurRadioFreq)
			{
				RDS_ReadDataFromDataSheet(pFlyRadioInfo, rds_buf,RDS_READ_DATA_END);
				if (RDS_ReadRadioRDSDatee(pFlyRadioInfo,rds_buf[RDS_READ_RDS_DATEE])//GetChannelSignalquality(pFlyRadioInfo,rds_buf[RDS_READ_USN_WAM],rds_buf[RDS_READ_LEVEL]) 
					&& RDS_ReadRadioRDSStatus(pFlyRadioInfo,rds_buf[RDS_READ_RDS_STATUS]))//有信号
				{
					RDS_ParseRdsDatas(pFlyRadioInfo,RDS_ReadRadioRDSDatas(pFlyRadioInfo,&rds_buf[RDS_READ_RDS_DTA3],4));

					if (pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataHx != pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx)
					{	
						//DBGI((_T("\r\nTEF6624 RDS Process")));
						RDS_ParseRdsDataProcess(pFlyRadioInfo);//处理接收到的完整的一组数据
						pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx++;
						if (pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx > (RDS_DATA_LEN-1))
							pFlyRadioInfo->sTEF6624RDSInfo.iRdsDataLx = 0;
					}

					dwWaitTimer = 20;
					pFlyRadioInfo->sTEF6624RDSInfo.iTickTimes = GetTickCount();
				}
				else
				{
					pFlyRadioInfo->sTEF6624RDSInfo.iTickTimes = GetTickCount();
				}
			}
			else
			{
				dwWaitTimer = INFINITE;
			}
		}
	}

	DBGI((_T("\r\nTEF6624 RDS MainThread Exit!")));
	return 0;
}

BOOL CreateTEF6624RDSThread(P_FLYRADIO_TEF6624_INFO pFlyRadioInfo)
{
	DWORD dwThreadID = 0;
	pFlyRadioInfo->sTEF6624RDSInfo.bKillDispatchRDSThread  = FALSE;
	pFlyRadioInfo->sTEF6624RDSInfo.hDispatchRDSThreadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	pFlyRadioInfo->sTEF6624RDSInfo.hFlyRDSThreadHandle = 
		CreateThread((LPSECURITY_ATTRIBUTES) NULL, //安全属性
		0,//初始化线程栈的大小，缺省为与主线程大小相同
		(LPTHREAD_START_ROUTINE)TEF6624RDSMainThreadProc, //线程的全局函数
		pFlyRadioInfo, //此处传入了主框架的句柄
		0, &dwThreadID);
	if (NULL == pFlyRadioInfo->sTEF6624RDSInfo.hFlyRDSThreadHandle)
	{
		pFlyRadioInfo->sTEF6624RDSInfo.bKillDispatchRDSThread = TRUE;
		return FALSE;
	}

	return TRUE;
}