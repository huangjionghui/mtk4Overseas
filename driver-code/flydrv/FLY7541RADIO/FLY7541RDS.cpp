// FLY7541Radio.cpp : Defines the entry point for the DLL application.
//

#include "FLY7541Radio.h"
#include "flyuserlib.h"

#if RADIO_RDS
/////////RDSȫ�ֱ���

struct RDS_TEMP_STRUCT
{
	BYTE rdsdec_cnt0x;
	BYTE rds_ps[2][8];// for Decode Programme Service (PS) Name  ��Ŀ����
	BYTE rds_af[8];// for Decode Alternative Frequency List��AF

	BYTE rdsdec_cnt2x;
	BYTE rds_rt_a[2][64];    // for Decode RadioText (RT) ���� GROUP_A�� TEXT_A �� TEXT_B
	BYTE rds_rt_b[32];    // for Decode RadioText (RT) ���� GROUP_B�� TEXT_A �� TEXT_B

	BYTE rds_10a[8];
	BYTE rds_10a_flag;
	BYTE rds_ptyn[8];

	BYTE rdsdec_cnt10x;
}sRDSTemp;

/*
BYTE * pty_display_table[33]=		 // ��Ŀ���͵���ʾ���� 	rdsdec_flag_pty ����ֵ �Ǵ�������±�
{
	"      None      ",
	"      News      ",
	"Current Affairs ",
	"  Information   ",
	"     Sport      ",
	"   Education    ",
	"     Drama      ",
	"    Cultures    ",
	"    Science     ",
	" Varied Speech  ",
	"   Pop Music    ",
	"   Rock Music   ",
	" Easy Listening ",
	"Light Classics M",
	"Serious Classics",
	"  Other Music   ",
	" Weather & Metr ",
	"    Finance     ",
	"Children��s Progs",
	" Social Affairs ",
	"    Religion    ",
	"    Phone In    ",
	"Travel & Touring",
	"Leisure & Hobby ",
	"   Jazz Music   ",
	" Country Music  ",
	" National Music ",
	"  Oldies Music  ",
	"   Folk Music   ",
	"  Documentary   ",
	"   Alarm Test   ",
	"Alarm - Alarm ! ",
	"    PTY Seek    "
};
*/
//////////////������

BYTE rdsdec_buf[8];// Groupe = 4 blocks ;  8 chars

#define ZERO(a)  memset(a,0,sizeof(a))
#define GET_T_AB(buf)  (buf[3]>>4) &0x01
#define GET_SEG(buf)  buf[3]&0x0f
#define GET_PTY(buf)  (buf[2]>>4)&0x0f
#define GET_G_AB(buf)  (buf[2]>>3)&0x01

void UpdateRDS_PI(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	UINT16 temp;
	temp  = (rdsdec_buf[0]<<8) | rdsdec_buf[1];          // 16bit

#if RADIO_RDS_DEBUG_MSG
	RETAILMSG(1, (TEXT("\r\nRDS PI:%x"),temp));
#endif

	if(pTDA7541RadioInfo->RDSInfo.rdsdec_flag_pi != temp)
	{
		pTDA7541RadioInfo->RDSInfo.rdsdec_flag_pi = temp;
	}
	if(0 == pTDA7541RadioInfo->RDSInfo.BaseReferencePI)//ȷ����׼PI
	{
		pTDA7541RadioInfo->RDSInfo.BaseReferencePI = temp;
	}
}
void printfRDS_PTY(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)//20100427 ��Ŀ����
{
	BYTE buff[] = {RADIORDS_ID_PTY,0x00};
	BYTE temp;
	temp  = ((rdsdec_buf[2]&3)<<3) | (rdsdec_buf[3]>>5); // 5bit


#if RADIO_RDS_DEBUG_MSG
	RETAILMSG(1, (TEXT("\r\nRDS PTY:%x"),temp));
#endif

	if(pTDA7541RadioInfo->RDSInfo.rdsdec_flag_pty != temp)
	{
		pTDA7541RadioInfo->RDSInfo.rdsdec_flag_pty = temp;
		buff[1] = temp;
		ReturnToUser(buff,2);
	}
}
void printfRDS_TP(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{	
	BYTE temp;
	temp  = (rdsdec_buf[2]>>2)&1;                        // 1bit

#if RADIO_RDS_DEBUG_MSG
	RETAILMSG(1, (TEXT("\r\nRDS TP FLAG:%x"),temp));
#endif

	if(pTDA7541RadioInfo->RDSInfo.rdsdec_flag_tp != temp)
	{
		pTDA7541RadioInfo->RDSInfo.rdsdec_flag_tp = temp;
	}
} 
void printfRDS_MS_DI(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	BYTE temp;
	temp = (rdsdec_buf[3]>>2)&0x01; // 1bit

#if RADIO_RDS_DEBUG_MSG
	RETAILMSG(1, (TEXT("\r\nRDS DI:%x    %x"),rdsdec_buf[3]&0x03,temp));
#endif
	if(pTDA7541RadioInfo->RDSInfo.rdsdec_flag_di[rdsdec_buf[3]&0x03] != temp)
	{

		pTDA7541RadioInfo->RDSInfo.rdsdec_flag_di[rdsdec_buf[3]&0x03] = temp;
	}

	temp = (rdsdec_buf[3]>>3)&0x01; // 1bit

#if RADIO_RDS_DEBUG_MSG
	RETAILMSG(1, (TEXT("\r\nRDS MS:%x"),temp));
#endif
	if(pTDA7541RadioInfo->RDSInfo.rdsdec_flag_ms != temp)
	{
		pTDA7541RadioInfo->RDSInfo.rdsdec_flag_ms = temp;
	}
}
void printfRDS_PS(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE *p)//20100427	 Ƶ������
{
	BYTE buff[9];
	BYTE i;
	buff[0] = RADIORDS_ID_PS;
	for(i = 0; i < 8; i++)
	{
		buff[i+1] = p[i];
	}
	ReturnToUser(buff,9);
}	
void printfRDS_RT(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE *p,BYTE len) //RadioText
{
	BYTE buff[65];
	BYTE i;
	buff[0] = RADIORDS_ID_RT;
	for(i = 0;i < len;i++)
	{
		buff[i+1] = p[i];
	} 
	ReturnToUser(buff,len+1);
}


void printfRDS_Date(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{

}
void printfRDS_PTYN(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE *p)
{

}

void printfRDS_TA(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)	  //20100427 ��ͨ��Ϣ
{
	BYTE buff[3] = {RADIORDS_ID_TA,0x00};
	if(((rdsdec_buf[3]>>4)&0x01) && ((rdsdec_buf[2]>>2)&0x01))//��ʼTA�㲥
	{
		if(pTDA7541RadioInfo->RDSInfo.rdsdec_flag_ta == 0)
		{
			pTDA7541RadioInfo->RDSInfo.rdsdec_flag_ta = 0x01;

			buff[1] = 0x01;
			ReturnToUser(buff,2);
		}
	}
	else
	{
		if(pTDA7541RadioInfo->RDSInfo.rdsdec_flag_ta)
		{
			pTDA7541RadioInfo->RDSInfo.rdsdec_flag_ta = 0x00;

			buff[1] = 0x00;
			ReturnToUser(buff,2);		
		}	
	}

}
void printfRDS_TA_Spec(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)	  //20100427 ��ͨ��Ϣ
{
	BYTE buff[3] = {RADIORDS_ID_TA,0x00};
	if(((rdsdec_buf[3]>>3)&0x03) == 0x03)//��ʼTA�㲥
	{
		if(pTDA7541RadioInfo->RDSInfo.rdsdec_flag_ta == 0)
		{
			pTDA7541RadioInfo->RDSInfo.rdsdec_flag_ta = 0x03;

			buff[1] = 0x01;
			ReturnToUser(buff,3);
		}
	}
	else
	{
		if(pTDA7541RadioInfo->RDSInfo.rdsdec_flag_ta)
		{
			pTDA7541RadioInfo->RDSInfo.rdsdec_flag_ta = 0x00;

			buff[1] = 0x00;
			ReturnToUser(buff,2);		
		}	
	}
}

void RDSParaInit(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)
{
	RETAILMSG(1, (TEXT("\r\nFLY7541Radio RDS para init!")));

	memset(&pTDA7541RadioInfo->RDSInfo,0,sizeof(_FLY_RDS_INFO));
}

//////////////////////////RDS���ݽ��մ���
//=============================================================================
// RDS Decode Type 0x
//-----------------------------------------------------------------------------
// Parameter(s)
//  fab : Type A or B flag
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================

void rdsdec_prc_0x(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)
{
	BYTE seg = GET_SEG(rdsdec_buf) & 3;

	// Data Order check
	if(seg==0) { sRDSTemp.rdsdec_cnt0x=0; }
	else if(sRDSTemp.rdsdec_cnt0x!=seg) {return;}

	printfRDS_TA(pTDA7541RadioInfo);

	sRDSTemp.rds_ps[fab][seg*2+0] = rdsdec_buf[6];
	sRDSTemp.rds_ps[fab][seg*2+1] = rdsdec_buf[7];

	if(fab==GROUP_A){
		sRDSTemp.rds_af[seg*2+0] = rdsdec_buf[4];
		sRDSTemp.rds_af[seg*2+1] = rdsdec_buf[5];
	}

	if(seg==3)// Build Complete
	{
#if RADIO_RDS_DEBUG_MSG
		UINT i;
		RETAILMSG(1, (TEXT("\r\nRDS PS Name:")));
		for (i = 0;i < 8;i++)
		{
			RETAILMSG(1, (TEXT("%c"),sRDSTemp.rds_ps[fab][i]));
		}
		if(fab == GROUP_A)
		{
			RETAILMSG(1, (TEXT("\r\nRDS AF List:")));
			for (i = 0;i < 8;i++)
			{
				RETAILMSG(1, (TEXT(" %d"),sRDSTemp.rds_af[i]+875));
			}
		}
#endif 
		memcpy(pTDA7541RadioInfo->RDSInfo.rdsdec_ps[fab],sRDSTemp.rds_ps[fab],8);
		if(memcmp(pTDA7541RadioInfo->RDSInfo.rdsdec_ps_dis,sRDSTemp.rds_ps[fab],8))
		{
			memcpy(pTDA7541RadioInfo->RDSInfo.rdsdec_ps_dis,sRDSTemp.rds_ps[fab],8);
			printfRDS_PS(pTDA7541RadioInfo,pTDA7541RadioInfo->RDSInfo.rdsdec_ps_dis);		
		}
		if(fab==GROUP_A)
		{
			if(memcmp(pTDA7541RadioInfo->RDSInfo.rdsdec_af,sRDSTemp.rds_af,8))//���б仯
			{				
				memcpy(pTDA7541RadioInfo->RDSInfo.rdsdec_af,sRDSTemp.rds_af,8);
			}
		}
	}
	sRDSTemp.rdsdec_cnt0x++;
}


//=============================================================================
// RDS Decode Type 1A and 1B
//  Program Item Number and slow labeling codes
//-----------------------------------------------------------------------------
// Parameter(s)
//  fab : Type A or B flag
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================
void rdsdec_prc_1x(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)
{
	UINT16 blk[4];
	BYTE  vc;
	//	BYTE r_page,la;
	//	BYTE  day,hour,min;
	UINT16 label;

	blk[0] = (rdsdec_buf[0]<<8) | rdsdec_buf[1];
	blk[1] = (rdsdec_buf[2]<<8) | rdsdec_buf[3];
	blk[2] = (rdsdec_buf[4]<<8) | rdsdec_buf[5];
	blk[3] = (rdsdec_buf[6]<<8) | rdsdec_buf[7];

	//	r_page = blk[1]&0x1F;
	//	la     = blk[2]&0x8000;    // Linkage Actuator
	vc     =(blk[2]>>12)&0x07; // Variant Code

	//	day = (blk[3]>>11) & 0x1F;
	//	hour= (blk[3]>>6) & 0x1f;
	//	min = 0x3f & blk[3];

	if(fab==GROUP_A){
		label = blk[2]&0x0FFF;

		switch(vc){
		case 0: // Extended Country Code
			label = blk[2]&0x00FF;
			pTDA7541RadioInfo->RDSInfo.rds_ecc = label;
			break;

		case 1: // TMC identification
			break;

		case 2: // Paging identification
			break;

		case 3: // Language codes
			pTDA7541RadioInfo->RDSInfo.rds_language_code =	label;
			break;

		case 4: // not assigned
		case 5: // not assigned
			break;

		case 6: // For use by broadcasters
			break;

		case 7: // Identification of EWS channnel
			break;
		}
	}
}



//=============================================================================
// RDS Decode 2A and 2B
//  Radio Text (2A has 64 charactors,2B has 32 charactors)
//-----------------------------------------------------------------------------
// Parameter(s)
//  fab : Type A or B flag
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================
void rdsdec_prc_2x(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)
{
	BYTE b   = GET_T_AB(rdsdec_buf);
	BYTE seg = GET_SEG(rdsdec_buf);
	BYTE ofs;
	BYTE i;

	// Data Order check
	if(seg==0) {sRDSTemp.rdsdec_cnt2x = 0;}
	else if(sRDSTemp.rdsdec_cnt2x!=seg) {return;}

	if(fab==GROUP_A)
	{
		ofs=seg*4;
		for(i = 0;i < 4;i++)
		{
			sRDSTemp.rds_rt_a[b][ofs+i]=rdsdec_buf[4+i];
			if(rdsdec_buf[4+i] == 0x0D || ofs+i == 63)//0x0D���������ߴﵽ�����Ŀ
			{
#if RADIO_RDS_DEBUG_MSG
				UINT j;
				RETAILMSG(1, (TEXT("\r\nRDS RadioText:")));
				j = 0;
				while (sRDSTemp.rds_rt_a[b][j] != 0x0D && j < 64)
				{
					RETAILMSG(1, (TEXT(" %c"),sRDSTemp.rds_rt_a[b][i]));
					j++;
				}
#endif 
				memcpy(pTDA7541RadioInfo->RDSInfo.rdsdec_rt_a[b],sRDSTemp.rds_rt_a[b],ofs+i+1);
				if(memcmp(pTDA7541RadioInfo->RDSInfo.rdsdec_rt_a_dis,sRDSTemp.rds_rt_a[b],ofs+i+1))
				{
					memcpy(pTDA7541RadioInfo->RDSInfo.rdsdec_rt_a_dis,sRDSTemp.rds_rt_a[b],ofs+i+1);
					printfRDS_RT(pTDA7541RadioInfo,pTDA7541RadioInfo->RDSInfo.rdsdec_rt_a_dis,ofs+i+1);
				}
			}	
		}
	} 
	else 
	{
		ofs=seg*2;
		for(i = 0; i < 2; i++)
		{
			sRDSTemp.rds_rt_b[ofs+i]=rdsdec_buf[6+i];
			if(rdsdec_buf[6+i] == 0x0D || ofs+i == 31)//0x0D���������ߴﵽ�����Ŀ
			{
#if RADIO_RDS_DEBUG_MSG
				UINT j;
				RETAILMSG(1, (TEXT("\r\nRDS RadioText:")));
				j = 0;
				while (sRDSTemp.rds_rt_b[j] != 0x0D && j < 32)
				{
					RETAILMSG(1, (TEXT(" %c"),sRDSTemp.rds_rt_b[i]));
					j++;
				}
#endif 
				if(memcmp(pTDA7541RadioInfo->RDSInfo.rdsdec_rt_b,sRDSTemp.rds_rt_b,ofs+i+1))
				{
					memcpy(pTDA7541RadioInfo->RDSInfo.rdsdec_rt_b,sRDSTemp.rds_rt_b,ofs+i+1);
					printfRDS_RT(pTDA7541RadioInfo,pTDA7541RadioInfo->RDSInfo.rdsdec_rt_b,ofs+i+1);
				}
			}	
		}
	}
	sRDSTemp.rdsdec_cnt2x++;
}



//=============================================================================
// RDS Decode type 3A or 3B
//  3A : Application identification for Open data
//  3B : Open Data Application
//-----------------------------------------------------------------------------
// Parameter(s)
//  fab : Type A or B flag
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================
void rdsdec_prc_3x(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)
{
	//	blk[0] = (rdsdec_buf[0]<<8) | rdsdec_buf[1];
	//	blk[1] = (rdsdec_buf[2]<<8) | rdsdec_buf[3];
	//	blk[2] = (rdsdec_buf[4]<<8) | rdsdec_buf[5];
	//	blk[3] = (rdsdec_buf[6]<<8) | rdsdec_buf[7];


	if(fab==GROUP_A){

	}

}



//=============================================================================
// RDS Decode type 4A and 4B
//  4A : "Clock-time and date"
//  4B : Open data application
//-----------------------------------------------------------------------------
// Parameter(s)
//  fab : Type A or B flag
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================
void rdsdec_prc_4x(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)
{
	if(fab==GROUP_A) {
		//		char buf[64];
		int date,hour,min,ofs;
		int yd,md,d,y,m;

		date = ((rdsdec_buf[3]&3)<<15) | (rdsdec_buf[4]<<7) | (rdsdec_buf[5]>>1);
		hour = ((rdsdec_buf[5]&1)<<4)  | ((rdsdec_buf[6]>>4));
		min  = ((rdsdec_buf[6]&0xf)<<2)| ((rdsdec_buf[7]>>6));
		ofs  = (rdsdec_buf[7]&0x3f);

		//		if(rdsdec_buf[7]&0x20) ofs=-ofs;

		yd = (int)(((double)date-15078.2)/365.25);
		md = (int)(((((double)date-14956.1)-((double)yd*365.25)))/30.6001);
		d = (int)(date-14956-((double)yd*365.25)-((double)md*30.6001));

		if(md==14 || md==15) {
			y = yd + 1;
			m = md -13;
		} else {
			y = yd;
			m = md -1;
		}

		pTDA7541RadioInfo->RDSInfo.rds_clock_year = 1900 + y;
		pTDA7541RadioInfo->RDSInfo.rds_clock_month= m;
		pTDA7541RadioInfo->RDSInfo.rds_clock_day  = d + 1;
		pTDA7541RadioInfo->RDSInfo.rds_clock_wd = (BYTE)(((long)date +2)%7 + 1); 

		pTDA7541RadioInfo->RDSInfo.rds_clock_hour = hour;
		pTDA7541RadioInfo->RDSInfo.rds_clock_min  = min;
		pTDA7541RadioInfo->RDSInfo.rds_clock_ofs  = ofs;

#if RADIO_RDS_DEBUG_MSG
		RETAILMSG(1, (TEXT("\r\nRDS Time:Y:%dM:%dD:%dW:%dH:%dM:%dO:%d")
			,pTDA7541RadioInfo->RDSInfo.rds_clock_year
			,pTDA7541RadioInfo->RDSInfo.rds_clock_month
			,pTDA7541RadioInfo->RDSInfo.rds_clock_day
			,pTDA7541RadioInfo->RDSInfo.rds_clock_wd
			,pTDA7541RadioInfo->RDSInfo.rds_clock_hour
			,pTDA7541RadioInfo->RDSInfo.rds_clock_min
			,pTDA7541RadioInfo->RDSInfo.rds_clock_ofs));
#endif

		if(date != 0)
		{
			printfRDS_Date(pTDA7541RadioInfo);
		}
	}
}
//=============================================================================
// RDS Decode 5A and 5B 
//  5A and 5B : Transparent data channels or ODA
//-----------------------------------------------------------------------------
// Parameter(s)
//  fab : Type A or B flag
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================
void rdsdec_prc_5x(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)
{

}


//=============================================================================
// RDS Decode 6A and 6B
// In-house applications of ODA
//-----------------------------------------------------------------------------
// Parameter(s)
//  fab : Type A or B flag
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================
void rdsdec_prc_6x(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)
{
}


//=============================================================================
// RDS Decode 7A and 7B
// 7A : Radio Paging or ODA
// 7B : Open data application
//-----------------------------------------------------------------------------
// Parameter(s)
//  fab : Type A or B flag
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================
void rdsdec_prc_7x(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)
{
}



//=============================================================================
// RDS Decode 8A and 8B
// Traffic Message Channes or ODA
//-----------------------------------------------------------------------------
// Parameter(s)
//  fab : Type A or B flag
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================
void rdsdec_prc_8x(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)	//TMC
{
}



//=============================================================================
// RDS Decode 9A and 9B
// Emergency warining systems or ODA
//-----------------------------------------------------------------------------
// Parameter(s)
//  fab : Type A or B flag
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================
void rdsdec_prc_9x(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)	//EWS�ƺ��в�ͬ��׼��ODA
{
}

//=============================================================================
// RDS Decode 10A and 10B
// 10A : Program Type Name
// 10B : Open data
//-----------------------------------------------------------------------------
// Parameter(s)
//  fab : Type A or B flag
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================
void rdsdec_prc_10x(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)
{
	BYTE ofs;
	BYTE b;

	if(fab==GROUP_A)
	{
		ofs = (rdsdec_buf[3]&0x01)*4;
		b = (rdsdec_buf[3]>>4)&0x01;

		if(ofs == 0){sRDSTemp.rdsdec_cnt10x = 0;}
		else if(sRDSTemp.rdsdec_cnt10x == 0){return;	}
		sRDSTemp.rdsdec_cnt10x++;

		sRDSTemp.rds_ptyn[ofs+0] = rdsdec_buf[4];
		sRDSTemp.rds_ptyn[ofs+1] = rdsdec_buf[5];
		sRDSTemp.rds_ptyn[ofs+2] = rdsdec_buf[6];
		sRDSTemp.rds_ptyn[ofs+3] = rdsdec_buf[7];


		if(ofs==4 && sRDSTemp.rdsdec_cnt10x==2)
		{
			if(memcmp(pTDA7541RadioInfo->RDSInfo.rdsdec_ptyn[b],sRDSTemp.rds_ptyn,8))
			{
				memcpy(pTDA7541RadioInfo->RDSInfo.rdsdec_ptyn[b],sRDSTemp.rds_ptyn,8);
				printfRDS_PTYN(pTDA7541RadioInfo,pTDA7541RadioInfo->RDSInfo.rdsdec_ptyn[b]);
			}
		}
	}
	else
	{
	}
}

#ifdef DEF_SUPPORT_11	//For Open Data Application
//=============================================================================
// RDS Decode 11A and 11B
// Open Data Application
//-----------------------------------------------------------------------------
// Parameter(s)
//  fab : Type A or B flag
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================
void rdsdec_prc_11x(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)
{
}
#endif

#ifdef DEF_SUPPORT_12	//For Open Data Application
//=============================================================================
// RDS Decode 12A and 12B
// Open Data Application
//-----------------------------------------------------------------------------
// Parameter(s)
//  fab : Type A or B flag
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================
void rdsdec_prc_12x(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)
{
}
#endif


#ifdef DEF_SUPPORT_13	//For Open Data Application
//=============================================================================
// RDS Decode 13A and 13B
// Enhanced Radio Paging or ODA
//-----------------------------------------------------------------------------
// Parameter(s)
//  fab : Type A or B flag
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================
void rdsdec_prc_13x(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)
{
	if(fab==GROUP_A){
		BYTE sty = rdsdec_buf[3]&0x07;
		BYTE inf = rdsdec_buf[3]&0x08;

		switch(sty){
		case 0: // address notification bits 24...0,when only 25bits are used 
			break;

		case 1: // address notification bits 49..25,when 50bits are used
			break;

		case 2: // adress notification bits 24..0,when 50bits are used
			break;

		case 3: // Reserved for Value Added Services system information
			break;

		case 4: // Reserved for future use
		case 5:
		case 6:
		case 7:
			break;
		}

		host_rds_update(PTY_13A);
	}
}
#endif

#ifdef DEF_SUPPORT_14	//�ȴ�����

UINT16 rds_14a[16];
BYTE  rds_14a_flag;
//=============================================================================
// RDS Decode 14A and 14B
// 14A : Enhanced Other Networks informations
//-----------------------------------------------------------------------------
// Parameter(s)
//  fab : Type A or B flag
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================
void rdsdec_prc_14x(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)
{
	UINT16 pi_tn;
	UINT16 pi_on;
	BYTE tp_tn;
	BYTE tp_on;

	pi_tn = (rdsdec_buf[0]<<8) | rdsdec_buf[1];
	pi_on = (rdsdec_buf[6]<<8) | rdsdec_buf[7];
	tp_tn = (rdsdec_buf[2]&0x08)?1:0;
	tp_on = (rdsdec_buf[2]&0x04)?1:0;

	if(fab==GROUP_A) {
		BYTE vc = rdsdec_buf[3]&0x0f;

		if(vc==0) {
			rds_14a_flag=0;
		}

		rds_14a[vc]=(rdsdec_buf[4]<<8) | rdsdec_buf[5];
		rds_14a_flag++;

		if(rds_14a_flag==16 && vc==15){
			host_rds_update(PTY_14A);
		}
	}
	else {
		host_rds_update(PTY_14B);
	}

}
#endif

void rdsdec_prc_14x_spec(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)
{
	if(fab == GROUP_B)
	{
		printfRDS_TA_Spec(pTDA7541RadioInfo);
	}
}

#ifdef DEF_SUPPORT_15	//���������ط��������
BYTE rds_15a[8];
BYTE rds_15a_flag;
//=============================================================================
// RDS Decode 15A and 15B
// Fast basic tuning and switching information
//-----------------------------------------------------------------------------
// Parameter(s)
//  fab : Type A or B flag
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================
void rdsdec_prc_15x(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo,BYTE fab)
{
	if(fab==GROUP_A){
		BYTE ps = (rdsdec_buf[3]&1)*4;

		if(ps==0){
			rds_15a_flag=0;
		}

		rds_15a[ps+0]=rdsdec_buf[4];
		rds_15a[ps+1]=rdsdec_buf[5];
		rds_15a[ps+2]=rdsdec_buf[6];
		rds_15a[ps+3]=rdsdec_buf[7];
		rds_15a_flag++;

		if(rds_15a_flag==2 && ps){
			host_rds_update(PTY_15A);
		}
	}
}
#endif
//=============================================================================
// RDS Decode Processing
//-----------------------------------------------------------------------------
// Parameter(s)
//  none
//-----------------------------------------------------------------------------
// Return Value
//  none
//=============================================================================
void rdsdec_process(P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo)//������յ���������һ������
{
	BYTE pty = GET_PTY(rdsdec_buf);
	BYTE fab = GET_G_AB(rdsdec_buf);

	// ALL GROUP
	UpdateRDS_PI(pTDA7541RadioInfo);
	printfRDS_PTY(pTDA7541RadioInfo);
	printfRDS_TP(pTDA7541RadioInfo);
	// MS,TA,DI
	if(pty==0 || (pty==15 && fab==GROUP_B)) {
		printfRDS_MS_DI(pTDA7541RadioInfo);
	}
	//		rdsdec_flag_ta = (rdsdec_buf[3]>>4)&1; // 1bit

	// EON TA 
	if(pty==14) {
		pTDA7541RadioInfo->RDSInfo.rdsdec_flag_eon = 1;	
	}

	pTDA7541RadioInfo->RDSInfo.rdsdec_flag_recv[fab] |= (1<<pty); // Set received flag


	switch(pty){

	case 0: rdsdec_prc_0x(pTDA7541RadioInfo,fab); break;

	case 1: rdsdec_prc_1x(pTDA7541RadioInfo,fab); break;

	case 2: rdsdec_prc_2x(pTDA7541RadioInfo,fab); break;

	case 3: rdsdec_prc_3x(pTDA7541RadioInfo,fab); break;

	case 4: rdsdec_prc_4x(pTDA7541RadioInfo,fab); break;

	case 5: rdsdec_prc_5x(pTDA7541RadioInfo,fab); break;

	case 6: rdsdec_prc_6x(pTDA7541RadioInfo,fab); break;

	case 7: rdsdec_prc_7x(pTDA7541RadioInfo,fab); break;

	case 8: rdsdec_prc_8x(pTDA7541RadioInfo,fab); break;

	case 9: rdsdec_prc_9x(pTDA7541RadioInfo,fab); break;

	case 10: rdsdec_prc_10x(pTDA7541RadioInfo,fab); break;

		/*case 11: rdsdec_prc_11x(pTDA7541RadioInfo,fab); break;

		case 12: rdsdec_prc_12x(pTDA7541RadioInfo,fab); break;

		case 13: rdsdec_prc_13x(pTDA7541RadioInfo,fab); break;

		case 14: rdsdec_prc_14x(pTDA7541RadioInfo,fab); break;

		case 15: rdsdec_prc_15x(pTDA7541RadioInfo,fab); break; */

	case 14: rdsdec_prc_14x_spec(pTDA7541RadioInfo,fab); break;//�ر�棬��TA�ź�

	default :
		break;

	}
}

DWORD WINAPI TDA7541RadioRDSRecThread(LPVOID pContext)
{
	DWORD dwWaitReturn;
	DWORD dwRes;
	DWORD dwMsgFlag;
	P_FLY_TDA7541_RADIO_INFO pTDA7541RadioInfo = (P_FLY_TDA7541_RADIO_INFO)pContext;

#if RADIO_RDS_DISPLAY_DEBUG
	BOOL bbbbbbbb;
	BYTE eeeeeeee[] = {
		0x57,0x65,0x6C,0x63,0x6F,0x6D,0x65,0x20,0x74,0x6F,0x20,0x67,0x75,0x61,0x6E,0x67,
		0x7A,0x68,0x6F,0x75,0x21,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
		0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
		0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x4A,0x51,0x69,0x6C,0x69,0x6E
	};
	BYTE cccccccc[] = {
		0xB9,0xE3,0xD6,0xDD,0xBB,0xB6,0xD3,0xAD,0xC4,0xFA,0x21,0x20,0x20,0x20,0x20,0x20,
		0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
		0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
		0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0xBD,0xAF,0xC6,0xE6,0xC1,0xD6
	};
	BYTE ssssssss[] = {0x57,0x65,0x6C,0x63,0x6F,0x6D,0x65};
#endif
	RETAILMSG(1, (TEXT("\r\nFlyAudio TDA7541RadioRDSRecThread Start!")));

	while (!pTDA7541RadioInfo->bKillDispatchRDSRecThread)
	{
#if RADIO_RDS_DISPLAY_DEBUG
		dwWaitReturn = WaitForSingleObject(pTDA7541RadioInfo->hMsgQueueToRadioCreate,1000);
#else
		dwWaitReturn = WaitForSingleObject(pTDA7541RadioInfo->hMsgQueueToRadioCreate,INFINITE);
#endif
		if (!pTDA7541RadioInfo->bPowerUp)
		{
			continue;
		}
		DBGD((TEXT("+")));
		ReadMsgQueue(pTDA7541RadioInfo->hMsgQueueToRadioCreate, rdsdec_buf, FLY_MSG_QUEUE_TO_RADIO_LENGTH, &dwRes, 0, &dwMsgFlag);

		if (*pTDA7541RadioInfo->radioInfo.pCurRadioFreq == *pTDA7541RadioInfo->radioInfo.pPreRadioFreq)
		{
			rdsdec_process(pTDA7541RadioInfo);//������յ���������һ������
		}

#if RADIO_RDS_DISPLAY_DEBUG
		if (bbbbbbbb)
		{
			bbbbbbbb = FALSE;
			printfRDS_RT(pTDA7541RadioInfo,eeeeeeee,16*4);
			printfRDS_PS(pTDA7541RadioInfo,ssssssss);
		}
		else
		{
			bbbbbbbb = TRUE;
			printfRDS_RT(pTDA7541RadioInfo,cccccccc,16*4);
			printfRDS_PS(pTDA7541RadioInfo,ssssssss);
		}
#endif
	}
	pTDA7541RadioInfo->TDA7541RadioRDSRecThreadHandle = NULL;
	RETAILMSG(1, (TEXT("\r\nFlyAudio TDA7541RadioRDSRecThread exit!")));
	return 0;
}

#endif
