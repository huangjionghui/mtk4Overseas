// FLYAssistDisplay.cpp : Defines the entry point for the DLL application.
//

#define GLOBAL_COMM	1

#include "FLYAssistDisplay.h"


/***************************************思域，CRV ********************************/
typedef struct {
	char CurrentDevice;
	UINT TotalTimer;
}stDVDInfo;
enum eAssistDisplayChannel CurrentAudioMode;

stDVDInfo DVDInfo;


PLAY_imfo CORRE_playing;
UINT CurrentRadioFreq;


BYTE DVDInfo_Frame[48]={
0x10,0x02,0x2A,0x30,0x03,0x00,0x03,0x00,0x00,0x05,
0xFF,0x01,0xFF,0xFF,0xFF,0xF0,0x41,0x00,0x00,0xFF,
0x11,0xFF,0xF6,0x13,0xFF,0xFF,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0xFF,0x08,0x00,0x46,0x00,0x00,
0x00,0x00,0x00,0xFF,0xFF,0x10,0x03,0xFF
};

BYTE IPODInfo_Frame[38]={
0x10,0x02,0x20,0x30,0x06,0x00,0x01,0x00,0x0A,0x03,
0xFF,0xFF,0x25,0xFF,0xFF,0x46,0xFF,0xF0,0x43,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x46,0x00,0x00,
0x00,0x00,0x00,0xf0,0xf0,0x10,0x03,0x22
};
BYTE RadioInfo_Frame[40]={
0x10,0x02,0x22,0x30,0x01,0x01,0x00,0x00,0x00,0x02,
0x06,0xFF,0x89,0x5F,0x82,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x01,0x08,0x00,0xFF,0xFF,0x04,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xA0
};

const BYTE AUXInfo_Frame[12]={
0x10,0x02,0x06,0x30,0x08,0x02,0x00,0x00,0xFF,0x10,
0x03,0xFF
};
const BYTE BTInfo_Frame[45]={
0x10,0x02,0x27,0x30,0x0B,0x12,0x00,0x00,0x01,0x02,
0xFF,0xFF,0xFF,0x09,0x0D,0x48,0x54,0x43,0x20,0x44,
0x65,0x73,0x69,0x72,0x65,0x20,0x48,0x44,0x00,0x00,
0x00,0x00,0x08,0x00,0x81,0x00,0x00,0x00,0x00,0xFF,
0x81,0x00,0x10,0x03,0x6E
};
//CRV 低配
/****************************************/
const BYTE AUXInfo_Frame_1[12]={
	0x10,0x02,0x06,0x30,0x0b,0x02,0x00,0x00,0xFF,0x10,
	0x03,0xFF
};
const BYTE BTInfo_Frame_1[45]={
	0x10,0x02,0x27,0x30,0x08,0x02,0x00,0x00,0x01,0x02,
	0xFF,0xFF,0xFF,0x09,0x0D,0x48,0x54,0x43,0x20,0x44,
	0x65,0x73,0x69,0x72,0x65,0x20,0x48,0x44,0x00,0x00,
	0x00,0x00,0x08,0x00,0x81,0x00,0x00,0x00,0x00,0xFF,
	0x81,0x00,0x10,0x03,0x6E
};
/****************************************/
const BYTE MediaInfo_Frame[76]={
0x10,0x02,0x46,0x50,0x03,0x01,0x00,0x46,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//,0x54,0x72,0x61,0x63,0x6B,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x10,0x03,0x3B
};

const BYTE TVModeChar[4]={'T','V',' ',' '};
const BYTE DVDModeChar[4]={'D','V','D',' '};
const BYTE CDCModeChar[4]={'C','D','C',' '};
const BYTE USBModeChar[4]={'U','S','B',' '};
const BYTE MP3ModeChar[4]={'M','P','3',' '};
const BYTE IPODModeChar[4]={'I','P','O','D'};
const BYTE A2DPModeChar[4]={'A','2','D','P'};

const BYTE TrackChar[5]={'T','r','a','c','k'};

const BYTE NULLChar[8]={' ',' ',' ',' ',' ',' ',' ',' '};

BYTE BT_NoChar[20]={' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};

BYTE BT_NoLen = 0;
BYTE PhoneNoUpdata = 0;
typedef enum{CDType=0,CDCType=1,CDC1Type=4,CDC2Type=5,CDC3Type=6,CDC4Type=7,AUXType=8,USBType=0x10}eDiscType;

typedef enum {RS_FM1=0,RS_FM2=1,RS_AM=5}eRadioSource;
typedef struct{
	BYTE Hour;				//本歌曲播放分钟时间
	BYTE Minute;			//本歌曲播放分钟时间
	BYTE Second;			// 秒时间
}TimerStruct;

typedef union{
	TimerStruct	Current;
	UINT CurTotal;
}MediaTimeUnion;
typedef struct 		   
{	
	BYTE Volume;		 
	eDiscType DiscType; 
	UINT16	Folder;    
	UINT16 Track;			//本碟第几首歌曲
	UINT16 TrackTotal;
	MediaTimeUnion Time;
	UINT	TotalTime;
	BYTE TimeBar;
}CarMediaStruct;

typedef struct
{
	BYTE timemode;//0:24小时制，1:12小时制
	BYTE hour;
	BYTE minute;
	BYTE second;
}tsTIMEINFO;
/**--------------------------------------------
保存当前收音机参数
---------------------------------------------**/
typedef struct 
{
	eRadioSource FM_AM;			
	BYTE Channel;		
	BYTE Frequency[3];
	BYTE RatioBar;
}CarBCTStruct;

CarMediaStruct CarMediaInfo,LastCarMediaInfo;
CarBCTStruct CarRadio,LastCarRadio;
tsTIMEINFO sTimeInfo,lastTimeInfo;

//10 02 05 3A 02 F1 14 05 10 03 CE 
BYTE CRC_Test[11]={0x10,0x02,0x05,0x3A,0x02,0xF1,0x14,0x05,0x10,0x03,0xFF };

BYTE TimeInfo[11]={0x10,0x02,0x05,0x3A,0x02,0xf1,0x04,0x04,0x10,0x03,0xFF};

BYTE TrackUpdata=0;
extern BYTE Bt_AudioOn;

/*****************************************************************************/

static BYTE Check_CRC(BYTE *pdata,BYTE len)
{
	BYTE Check_Sum=0;
	BYTE i;

	for(i=0;i<len;i++)
	{
		Check_Sum ^=pdata[i];		
	}
	return (Check_Sum);
}
static void ChangeTimeToNum(BYTE minute,BYTE second,BYTE *pdata)			//Fx'xx  xx'xx
{
	if((minute/10) >0)
	{
		pdata[0] = ((minute/10)<<4)+minute%10;
	}
	else
	{
		pdata[0] = minute+0xf0; 
	}
	pdata[1] = ((second/10)<<4)+second%10;
}

static void ChangeTONum(BYTE opty,UINT data,BYTE *pdata)
{
	UINT temp,div;
	BYTE	len=0;
	BYTE buff[6];
	BYTE i=0,j=0;
	static BYTE BeginBit=0;
	BYTE *p=buff;

	div = 100000;
	while (div > 0) 
	{
		if (data < div)
		{
			temp = 0;
		}
		else 
		{
			temp = data / div;
			data -= temp * div;
		}
		div /= 10;
		*p =temp ;
		p++	;
		len++;
	}
	switch(opty)
	{
		case 0:
			for(i=0;i<3;i++)
			{
				for(j=0;j<2;j++)
				{
					if((buff[i*2+j] == 0)&&(BeginBit==0))
					{
						if(j == 0)
							*pdata |= (0xF<<4) ;
						else
							*pdata |= 0xF ;
					}
					else
					{
						BeginBit=1;
						if(j == 0)
						*pdata = (*pdata & 0x0F)|(buff[i*2+j]<<4) ;
						else
						*pdata = (*pdata & 0xF0)|buff[i*2+j] ;
					}
				}
				pdata++;
			}
			BeginBit=0;
		break;
		case 1:
			for(i=0;i<6;i++)
			{
				pdata[i] = buff[i]+0x30;
				if(pdata[i]==0x30)
				{
					pdata[i] =' ';
				}
			}
			break;
	default:
		break;
	}
}
static BYTE  CalcMediaProgressBarProcessor(BYTE Opty,UINT CurVal,UINT TotalVal)
{
	BYTE ratio=0;
	BYTE progressbar=0xFF;

	switch(Opty)
	{
		case 0:	//media
			if(TotalVal == 0)
				return 0xf0;
			ratio = CurVal*100/TotalVal;
			if(ratio == 100)
			{
				progressbar = 0x00;
			}
			else if(ratio%100/10 > 0)
			{
				progressbar = (ratio%100/10<<4)+(ratio%10);
			}
			else
			{
				progressbar = 0xf0+(ratio%10);
			} 
			break;
		case 1:	//radio
			break;
		default:
			break;
	}
	return (progressbar);
}

static void MediaInfoReInit(void)
{
	if(TrackUpdata==0)
	{
	  CarMediaInfo.Track=0x00 ;
	}
	CarMediaInfo.Time.Current.Minute=0x0;
	CarMediaInfo.Time.Current.Second=0x0;
	CarMediaInfo.TimeBar=0xFF;
}

static void PackageMediaFrameInfo(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,enum eAssistDisplayChannel MediaType,CarMediaStruct MediaInfo)
{
	static enum eAssistDisplayChannel LastMediaType;
	BYTE buff[48];
	BYTE FrameBuff[150];
	BYTE *p=FrameBuff;
	BYTE frame_len=0;
	BYTE len=0; 
	BYTE i=0;
	
	
	if((LastMediaType != MediaType)&&(MediaType != eAD_BT))
	{
		MediaInfoReInit();
		LastMediaType = MediaType;
	}
	
	switch(MediaType)
	{
		case eAD_Init:

			/*for(i=0;i<11;i++)
			{
				buff[i] = CRC_Test[i];
			}
			frame_len=len=11;
		
			buff[10]=Check_CRC(&buff[2],buff[2]+1+2);
			break;*/
			return;
		case eAD_BT:   
			for(i=0;i<45;i++)
			{
				if((pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSteelAD[0]< 860)//低配版的CRV使用的是AD方向盘
					||(pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSteelAD[1]< 860))
				{
					buff[i] = BTInfo_Frame_1[i];
				}
				else
				{
					buff[i] = BTInfo_Frame[i];
				}
			}

			frame_len=len=45;	
			
			buff[8]=MediaInfo.Volume;

			if (BtCALL_IN == pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus) 
			{
				buff[9] = 3;
			}
			else if(BtCALL_OUT ==  pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus)  
			{
				buff[9] = 4;
			}
			else if(BtCALL_ON ==  pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus)  
			{
				buff[9] = 5;
			}
			else
			{
			
			}
			buff[44]=Check_CRC(&buff[2],buff[2]+1+2);
			break;
		case eAD_TV:
		case eAD_DVD:	
		case eAD_MP3:
		case eAD_A2DP:
			for(i=0;i<48;i++)
			{
				buff[i] = DVDInfo_Frame[i];
			}

			frame_len=len=48;

			if ((pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus != BtCALL_IDEL)) 
			{
				buff[4]	= 0x0B ;
			}
			else
			{
				if((eAD_DVD == MediaType)&& DVDInfo.CurrentDevice)
				{	
					buff[5]	= 0x10 ;
				}
			}
			switch(MediaType)
			{
				case eAD_TV:
					buff[4]	= 0x0A;	
					break;	
				default:
					break;
			}
			TrackUpdata=0;
			buff[8]=MediaInfo.Volume;
			
			buff[10]=0xFF;		 //Folder 高位	 暂时不用
			buff[11]=0xFF;		 //Folder 低位

			ChangeTONum(0,MediaInfo.Track,&buff[11]);
			ChangeTimeToNum(MediaInfo.Time.Current.Minute,MediaInfo.Time.Current.Second,&buff[15]);

			buff[20]=(BYTE)MediaInfo.TrackTotal;
			
			ChangeTONum(0,CarMediaInfo.TotalTime,&buff[22]);
		
			buff[43]=MediaInfo.TimeBar;

		   	buff[47]=Check_CRC(&buff[2],buff[2]+1+2);
			break;
		case eAD_iPod:
			for(i=0;i<38;i++)
			{
				buff[i] = IPODInfo_Frame[i];
			}
			frame_len=len=38;
			buff[8]=MediaInfo.Volume;
			ChangeTONum(0,MediaInfo.Track,&buff[10]);
			ChangeTONum(0,MediaInfo.TrackTotal,&buff[13]);					   
			ChangeTimeToNum(MediaInfo.Time.Current.Minute,MediaInfo.Time.Current.Second,&buff[17]);
			
			buff[33]=MediaInfo.TimeBar;
			buff[37]=Check_CRC(&buff[2],buff[2]+1+2);

			break;
		case eAD_Radio:
			for(i=0;i<40;i++)
			{
				buff[i] = RadioInfo_Frame[i];
			}
			frame_len=len=40;
		
			buff[5]=CarRadio.FM_AM;
		
			
			buff[8]=MediaInfo.Volume;	
			buff[10]=CarRadio.Channel;	
			
			if(CarRadio.FM_AM !=RS_AM)
			{
				buff[11]=CarRadio.Frequency[0];   //freq
				buff[12]=CarRadio.Frequency[1];	 
				buff[13]=CarRadio.Frequency[2];						 
			}
			else
			{
				buff[11]=0xF0 | (CarRadio.Frequency[1]>>4);
				buff[12]=(CarRadio.Frequency[1]<<4) | (CarRadio.Frequency[2]>>4);
				buff[13]=(CarRadio.Frequency[2]<<4) | 0x0F;
			}
			buff[25]=CarRadio.RatioBar;

		   	buff[39]=Check_CRC(&buff[2],buff[2]+1+2);

			break;
		case eAD_AUX:
			for(i=0;i<12;i++)
			{
				if((pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSteelAD[0]< 860)//低配版的CRV使用的是AD方向盘
					||(pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSteelAD[1]< 860))
				{
					buff[i] = AUXInfo_Frame_1[i];
				}
				else
				{
					buff[i] = AUXInfo_Frame[i];
				}
			}
			frame_len=len=12;
		
			buff[8]=MediaInfo.Volume;
			buff[11]=Check_CRC(&buff[2],buff[2]+1+2);
			break;
		default:
			return;
			break;

   }

	for(i=0;i<frame_len;i++)
	{
		*p = buff[i];	
		
		 if((i>2) && (i<buff[2]))
		 {
			 if(*p == 0x10)
			 {	 
			 	p++;
			 	*p = buff[i];	
			 	
				 len++;
			 }
		 } 
		 p++;
	}
	WriteToAssistCom(pAssistDisplayInfo,FrameBuff,len);

}
static void deal_type(BYTE data)
{	
	CORRE_playing = P_INFO_CD;
	switch(data)
	{		  
		case P_INFO_SRADIO:			 //思域
		{	
			CORRE_playing = P_INFO_SRADIO;
			break;
		}
		case P_INFO_MP3:
		{		   
			CORRE_playing = P_INFO_MP3;
			break;
		}	 
		default:break;
	}
}

static void ShowTimeInfo(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo)
{
	BYTE buff[11];
	BYTE FrameBuff[20];
	BYTE *p=FrameBuff;
	BYTE  framelen =11;
	BYTE len=0;
	BYTE i=0;
	
	len =framelen;
	memcpy(buff,TimeInfo,11);

	if(pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bTimeSetupMode == 1)
	{
		buff[4] = 0x02;	//设置模式
	}
	else
	{
		buff[4] = 0x01;//正常显示
	}

	if(sTimeInfo.timemode == 1)//12小时制
	{
		BYTE bHour=0;
		
		bHour=sTimeInfo.hour%12;

		if(bHour== 0)
		{
			bHour =12;
		}
		if(bHour/10)
		{
			buff[5] = ((bHour/10)<<4) |(bHour%10);
		}
		else
		{
			buff[5] = 0xf0+bHour;
		}
		buff[6]=((sTimeInfo.minute/10)<<4)|(sTimeInfo.minute%10);

		if(sTimeInfo.hour>12)
		{
			buff[7] = 0x06;
		}
		else
		{
			buff[7] = 0x05;
		}
	}
	else
	{
		if(sTimeInfo.hour/10)
		{
			buff[5] = ((sTimeInfo.hour/10)<<4)|(sTimeInfo.hour%10);
		}
		else
		{
			buff[5] = 0xf0+sTimeInfo.hour;
		}
		
		buff[6]=((sTimeInfo.minute/10)<<4)|(sTimeInfo.minute%10);

		buff[7] = 0x04;
	}

	buff[10] = Check_CRC(&buff[2],buff[2]+1+2);

	for(i=0;i<framelen;i++)
	{
		*p = buff[i];	
		
		 if((i>2) && (i<(buff[2]+2)))
		 {
			 if(*p == 0x10)
			 {	 
			 	p++;
			 	*p = buff[i];				 	
				 len++;
			 }
		 } 
		 p++;
	}
	WriteToAssistCom(pAssistDisplayInfo,FrameBuff,len);		
}
static void ShowCharInfo(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,enum eAssistDisplayChannel MediaType,BYTE *Opty)//
{
	BYTE buff[76];
	BYTE FrameBuff[150];
	BYTE *p=FrameBuff;
	BYTE i=0;
	BYTE frame_len=0;
	BYTE len=0;

	for(i=0;i<76;i++)
	{
		buff[i] = MediaInfo_Frame[i];
	}
	
	len=frame_len=76;

	switch(MediaType)
	{
		case eAD_BT:
			buff[4]	= 0x0B;
			buff[7]	= 0x81;
			buff[8]	= 0x09;
			memcpy(&buff[9],BT_NoChar,BT_NoLen);
			break;
		case eAD_A2DP:
			memcpy(&buff[10],A2DPModeChar,sizeof(A2DPModeChar));
			break;
		case eAD_TV:
			buff[4] = 0x0A;
			memcpy(&buff[10],TVModeChar,sizeof(TVModeChar));
			break;
		case eAD_DVD:
			if(DVDInfo.CurrentDevice)
			{	
				buff[5]	= 0x10 ;	 
				memcpy(&buff[10],USBModeChar,sizeof(USBModeChar));
			}
			else
			{
				memcpy(&buff[10],DVDModeChar,sizeof(DVDModeChar));
			}
			break;	
		case eAD_MP3: 
			memcpy(&buff[10],MP3ModeChar,sizeof(MP3ModeChar));
			break;
		case eAD_iPod:
			buff[4] = 0x06;
			memcpy(&buff[10],IPODModeChar,sizeof(IPODModeChar));
			break;
		default:
			return;
			break;
	}
	
//	buff[18] = 0x20;
//	memcpy(&buff[19],Opty,sizeof(Opty));

	buff[75]=Check_CRC(&buff[2],buff[2]+1+2);

	for(i=0;i<frame_len;i++)
	{
		*p = buff[i];	
		
		 if((i>2) && (i<buff[2]))
		 {
			 if(*p == 0x10)
			 {	 
			 	p++;
			 	*p = buff[i];	
			 	
				 len++;
			 }
		 } 
		 p++;
	}
	WriteToAssistCom(pAssistDisplayInfo,FrameBuff,len);
}

void dealCANAssignmentUpdataCRV(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BYTE cmd,BYTE data)
{
	   static BYTE HAND_phone_number[15];	
	   static BYTE phone_number_len=0;
	   
	   BYTE timebar=0xFF;
	   BYTE i = 0;
	   UINT tmp;
	   UINT FMFreq_MIN,FMFreq_MAX,AMFreq_MIN,AMFreq_MAX;
	   
	   FMFreq_MIN =pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iFreqFMMin;
	   FMFreq_MAX =pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iFreqFMMax;
	   AMFreq_MIN =pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iFreqAMMin;
	   AMFreq_MAX =pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iFreqAMMax;

	   switch(cmd)										   
	   {
			case PLAYING_TYPE:
			{	
				deal_type(data);
				break;
			}
			case TRACK_H:					//
			{
				tmp = data;
				CarMediaInfo.Track =(CarMediaInfo.Track &0x00FF)|tmp<<8;
				break;
			}
			case TRACK_L:
			{	
				tmp = data;
				CarMediaInfo.Track =(CarMediaInfo.Track &0xFF00)|tmp;		
				TrackUpdata=1;
				break;
			}
			case RPT_RDM:
			{	
				 break;
			}
			case MINUTE:
			{	
				CarMediaInfo.Time.Current.Minute = data;
				break;
			 }
			case SECOND:
			{
				CarMediaInfo.Time.Current.Second = data;
	
				timebar=CalcMediaProgressBarProcessor(0,((CarMediaInfo.Time.Current.Minute*60)+CarMediaInfo.Time.Current.Second),CarMediaInfo.TotalTime);
				CarMediaInfo.TimeBar=timebar;
				break;
			}
			case CH:
			{	
				if(data== 0)
				{
	
				}
				else if((data & 0xf0) == 0x10)
				{
					CORRE_playing = P_INFO_FM1;
	
					CarRadio.FM_AM=RS_FM1;
				}
				else if((data & 0xf0) == 0x20)
				{
					CORRE_playing = P_INFO_FM2;
					CarRadio.FM_AM=RS_FM2;
				}
				else if((data & 0xf0) == 0x30)
				{
					CORRE_playing = P_INFO_AM;
					CarRadio.FM_AM=RS_AM;
				}				
				break;
			}
			case FM1_FRQ:
			{				
				tmp=CurrentRadioFreq;				
				ChangeTONum(0,tmp,CarRadio.Frequency);
				if(FMFreq_MAX != FMFreq_MIN)
				{
					CarRadio.RatioBar = ((tmp-(FMFreq_MIN))*98)/((FMFreq_MAX)-(FMFreq_MIN));
				}
				CarRadio.FM_AM=RS_FM1;
				CORRE_playing = P_INFO_FM1;
				break;
			}
			case FM2_FRQ:
			{	
				tmp=CurrentRadioFreq;
	
				ChangeTONum(0,tmp,CarRadio.Frequency);
				CarRadio.RatioBar = ((tmp-FMFreq_MIN)*98)/(FMFreq_MAX-FMFreq_MIN);
				
				CarRadio.FM_AM=RS_FM2;		
				CORRE_playing = P_INFO_FM2;
				break;
			}
			case AM_FRQ:
			{	
				tmp=CurrentRadioFreq;
				ChangeTONum(0,tmp,CarRadio.Frequency);
				CarRadio.RatioBar = ((tmp-AMFreq_MIN)*101)/(AMFreq_MAX-AMFreq_MIN);
	
				CarRadio.FM_AM=RS_AM;
				CORRE_playing = P_INFO_AM;
				break;
			}
			case TIME_SETUP:
			{				
				break;
			}
			case VOLUME:
			{	
				CarMediaInfo.Volume = data;
				break;
			}
			case PLAYING_STATUS:
			{	
				break;
			}
			case MENU:
			{
				break;
			}
			case MODE_CHANGE:
			{
				break;
			}
			case HAND_PHONE_INF:
			{
				if(data==0x01)
				{
				
				}
				else	
				{
				}
				break;
			}
			case PHONE:
			{		
				switch(data)
				{
					case 0x0d:
					{
						phone_number_len = 0;
						PhoneNoUpdata = 1;
						break;
					}
					case 0x09:
					{
						if(phone_number_len>0)
						{
							BT_NoLen = phone_number_len;
							for(i=0;i<BT_NoLen;i++)
							{
								BT_NoChar[i] = HAND_phone_number[i];
							}
							ShowCharInfo(pAssistDisplayInfo,eAD_BT,(BYTE *)BT_NoChar);
						}
						break;				
					}
					case	'0':
					case	'1':
					case	'2':
					case	'3':
					case	'4':
					case	'5':
					case	'6':
					case	'7':
					case	'8':
					case	'9':
					case	'*':
					case	'#':	
					 HAND_phone_number[phone_number_len++]=data;
							break;
				}
			 }
			 break;
			default:break;
	   }
}

void dealCANAssignmentUpdata(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BYTE cmd,BYTE data)
{
	/*if(pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName == CAR_CRV_12)
	{
		dealCANAssignmentUpdataCRV(pAssistDisplayInfo,cmd,data);
	}*/
}
void SendDataToCivic(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo)
{
	static BYTE CarTxStep=0;
	static BYTE count=0;
	
	if((GetTickCount() - pAssistDisplayInfo->iNormalTimer)>1000)
	{	
		pAssistDisplayInfo->iNormalTimer = GetTickCount();
		CarMediaInfo.Volume &=0x7F;
		count++;
		if(count>10)
		{
			count =0;
			ShowTimeInfo(pAssistDisplayInfo);
		}
	}
	
	if((lastTimeInfo.hour != sTimeInfo.hour)
	||(lastTimeInfo.minute != sTimeInfo.minute)
	||(lastTimeInfo.second != sTimeInfo.second)
	||(lastTimeInfo.timemode !=sTimeInfo.timemode))
	{
		count =0;
		ShowTimeInfo(pAssistDisplayInfo);
		lastTimeInfo=sTimeInfo;
	}
	switch(CarTxStep)
	{
		case 0:
			if (pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus== BtCALL_IDEL)
			{
				PackageMediaFrameInfo(pAssistDisplayInfo,CurrentAudioMode,CarMediaInfo);	
			}
			else
			{
				PackageMediaFrameInfo(pAssistDisplayInfo,eAD_BT,CarMediaInfo);
			}
			CarTxStep=1;
			break;
		case 1:
			if (pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTCallStatus == BtCALL_IDEL)
			{
				if(PhoneNoUpdata == 2)
				{
					PhoneNoUpdata = 0;
					memset(BT_NoChar,0x00,sizeof(BT_NoChar));
				}
				ShowCharInfo(pAssistDisplayInfo,CurrentAudioMode,(BYTE *)NULLChar);
			}
			else											
			{
				ShowCharInfo(pAssistDisplayInfo,eAD_BT,(BYTE *)NULLChar);
				PhoneNoUpdata =	2;
			}
			CarTxStep=0;
			break;
		default:
			CarTxStep=0;
			break;
	}	
}
void  DealCivicInfoUpdataProcessor(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo)
{
	BYTE updata=0;
	BYTE i=0;

	if((CarMediaInfo.Volume&0x7F) != (LastCarMediaInfo.Volume&0x7F)) 
	{
		CarMediaInfo.Volume |=0x80;
		pAssistDisplayInfo->iNormalTimer = GetTickCount();
	    updata=1;
	}
	if(CarMediaInfo.Track != LastCarMediaInfo.Track) updata=1;
	else if(CarMediaInfo.TrackTotal != LastCarMediaInfo.TrackTotal) updata=1;
	else if(CarMediaInfo.Time.CurTotal != LastCarMediaInfo.Time.CurTotal) updata=1;
	else if(CarMediaInfo.TimeBar != LastCarMediaInfo.TimeBar) updata=1;
	else if(CarRadio.FM_AM != LastCarRadio.FM_AM)  updata=1;
	else if(CarRadio.RatioBar != LastCarRadio.RatioBar)  updata=1;
	else
	{
		for(i=0;i<3;i++)
		{
			if(CarRadio.Frequency[i] != LastCarRadio.Frequency[i]) 
			{
				updata=1;
				break;
			}
		}
	}	
	if(updata)
	{
		SetEvent(pAssistDisplayInfo->hDispatchInterThreadEvent);
		LastCarMediaInfo = CarMediaInfo;
		LastCarRadio = CarRadio;
	}
}

void DealCarLcdInfo(P_FLY_ASSISTDISPLAY_INFO pAssistDisplayInfo,BYTE *pdata,UINT len)
{
	static BYTE RadioMode;
	static BYTE track_h;
	static BYTE track_l;
	
	switch (pdata[0])
	{					  
		case 0x11:	//正在播放的媒体
			{
				CurrentAudioMode = (eAssistDisplayChannel)pdata[1];
				dealCANAssignmentUpdata(pAssistDisplayInfo,MENU,pdata[1]);
			}
			break;
		case 0x20:	//DVD曲目，title（2），track（2）
			{
				if(CurrentAudioMode == eAD_DVD)
				{
					track_l =pdata[3];
					track_h =pdata[4];
					dealCANAssignmentUpdata(pAssistDisplayInfo,TRACK_H,track_h);
					dealCANAssignmentUpdata(pAssistDisplayInfo,TRACK_L,track_l);
				}
			}
			break;
		case 0x21: //播放时间
			{
				if(CurrentAudioMode == eAD_DVD)
				{
					UINT playTime;
					playTime = (pdata[4]<<24)+(pdata[3]<<16)+(pdata[2]<<8)+(pdata[1]);
					if(playTime != 0)
					{
						dealCANAssignmentUpdata(pAssistDisplayInfo,MINUTE,playTime/60);
						dealCANAssignmentUpdata(pAssistDisplayInfo,SECOND,playTime%60);
					}
				}
			}
			break;
		case 0x22://DVD信息
			{
				if(CurrentAudioMode == eAD_DVD)
				{
					dealCANAssignmentUpdata(pAssistDisplayInfo,PLAYING_TYPE,pdata[1]);
				}
			} 
			break;
		case 0x23: //DVD播放状态
			{
				if(CurrentAudioMode == eAD_DVD)
				{
					if(pdata[1] == 0)//stop
					{
					   dealCANAssignmentUpdata(pAssistDisplayInfo,PLAYING_STATUS,1);
					}
					else if(pdata[1] == 1)
					{
						dealCANAssignmentUpdata(pAssistDisplayInfo,TRACK_H,track_h);
						dealCANAssignmentUpdata(pAssistDisplayInfo,TRACK_L,track_l);
					}
					else if(pdata[1] == 2)//pause
					{
						dealCANAssignmentUpdata(pAssistDisplayInfo,PLAYING_STATUS,0);
					}
				}
			}
			break;
		case 0x24://碟机状态
			{
				if(CurrentAudioMode == eAD_DVD)
				{
					 if(pdata[1] == 5)//loading
					 {
						  dealCANAssignmentUpdata(pAssistDisplayInfo,PLAYING_STATUS,2);
					 }
					 else if(pdata[1] == 2)
					 {
						  dealCANAssignmentUpdata(pAssistDisplayInfo,PLAYING_STATUS,4);
					 }
					 else if(pdata[1] == 0)
					 {
						  dealCANAssignmentUpdata(pAssistDisplayInfo,PLAYING_TYPE,0);			 
					 }
				}
			}
			break;					
		case 0x25:
				DVDInfo.CurrentDevice = pdata[1];
				break;
		case 0x26://DVD
				CarMediaInfo.TotalTime = (pdata[4]<<24)+(pdata[3]<<16)+(pdata[2]<<8)+pdata[1];
				break;
		case 0x30: //RADIO 信息
			{
				if(CurrentAudioMode == eAD_Radio)
				{
					dealCANAssignmentUpdata(pAssistDisplayInfo,MENU,pdata[1]+9);
					RadioMode = pdata[1];
				}
			}
			break;
		case 0x31: //RADIO FRE
			{
				if(CurrentAudioMode == eAD_Radio)
				{
					CurrentRadioFreq =	(pdata[1]<<8)+ pdata[2];
					
					if(RadioMode == 0) //FM1
					{
						 dealCANAssignmentUpdata(pAssistDisplayInfo,FM1_FRQ,pdata[1]);
					}
					else if(RadioMode == 1)//FM2
					{
						dealCANAssignmentUpdata(pAssistDisplayInfo,FM2_FRQ,pdata[1]);
					}
					else if(RadioMode == 2) //AM
					{
						dealCANAssignmentUpdata(pAssistDisplayInfo,AM_FRQ,pdata[1]);
					}
				}
			}
			break;
		case 0x32: //RADIO CH
			{
				if(CurrentAudioMode == eAD_Radio)
				{
					if(pdata[1] !=0)
					{
						pdata[1]= (RadioMode+1)<<4|pdata[1];
					}
					dealCANAssignmentUpdata(pAssistDisplayInfo,CH,pdata[1]);
				}
			}
			break;
		case 0x33://RADIO TEXT
			{
			
			}
			break;
		case 0x40:	//IPOD TRACK
			{
				if(CurrentAudioMode == eAD_iPod)
				{
					track_l =pdata[1];
					track_h =pdata[2];
					dealCANAssignmentUpdata(pAssistDisplayInfo,TRACK_H,track_h);
					dealCANAssignmentUpdata(pAssistDisplayInfo,TRACK_L,track_l);
			
				}
			}
			break;
		case 0x41: //IPOD TIME
			{
				if(CurrentAudioMode == eAD_iPod)
				{
					UINT playTime;
					playTime = (pdata[4]<<24)+(pdata[3]<<16)+(pdata[2]<<8)+(pdata[1]);
					dealCANAssignmentUpdata(pAssistDisplayInfo,MINUTE,playTime/60);
					dealCANAssignmentUpdata(pAssistDisplayInfo,SECOND,playTime%60);
				}
			}
			break;
		case 0x42:	//IPOD STATUS
			{
				if(CurrentAudioMode == eAD_iPod)
				{
					if(pdata[1] == 0)//stop
					{
					   dealCANAssignmentUpdata(pAssistDisplayInfo,PLAYING_STATUS,1);
					}
					else if(pdata[1] == 1)
					{
						dealCANAssignmentUpdata(pAssistDisplayInfo,TRACK_H,track_h);
						dealCANAssignmentUpdata(pAssistDisplayInfo,TRACK_L,track_l);
					}
					else if(pdata[1] == 2)//pause
					{
						dealCANAssignmentUpdata(pAssistDisplayInfo,PLAYING_STATUS,0);
					}
				}
			}
			break;
		case 0x43:
				CarMediaInfo.TrackTotal= (pdata[2]<<8)+pdata[1];	
				TrackUpdata=1;
				break;
		case 0x44://IPOD
				CarMediaInfo.TotalTime= (pdata[4]<<24)+(pdata[3]<<16)+(pdata[2]<<8)+pdata[1];
				break;
		case 0x50: //MP3 TRACK
			{
				if(CurrentAudioMode == eAD_MP3)
				{
					track_l =pdata[1];
					track_h =pdata[2];
					dealCANAssignmentUpdata(pAssistDisplayInfo,TRACK_H,track_h);
					dealCANAssignmentUpdata(pAssistDisplayInfo,TRACK_L,track_l);
				}			
			}
			break;
		case 0x51: //MP3 TIME
			{
				if(CurrentAudioMode == eAD_MP3)
				{
					UINT playTime;
					playTime = (pdata[4]<<24)+(pdata[3]<<16)+(pdata[2]<<8)+(pdata[1]);
					dealCANAssignmentUpdata(pAssistDisplayInfo,MINUTE,playTime/60);
					dealCANAssignmentUpdata(pAssistDisplayInfo,SECOND,playTime%60);
				}			
			}
			break;
		case 0x52:	//MP3 STATUS
			{
				if(CurrentAudioMode == eAD_MP3)
				{
					if(pdata[1] == 1)//pause
					{
						dealCANAssignmentUpdata(pAssistDisplayInfo,PLAYING_STATUS,0);
					}
					else if(pdata[1] == 0)
					{
						dealCANAssignmentUpdata(pAssistDisplayInfo,TRACK_H,track_h);
						dealCANAssignmentUpdata(pAssistDisplayInfo,TRACK_L,track_l);	
					}
				}
			}
			break;
		case 0x53:
			CarMediaInfo.TotalTime= (pdata[4]<<24)+(pdata[3]<<16)+(pdata[2]<<8)+pdata[1];
			break;
		case 0x60: //XM CH
			{
				if(CurrentAudioMode == eAD_XM)
				{
					dealCANAssignmentUpdata(pAssistDisplayInfo,CH,pdata[1]);
				}
			}
			break;
		case 0x61://XM STATUS
			{
				if(CurrentAudioMode == eAD_XM)
				{
					dealCANAssignmentUpdata(pAssistDisplayInfo,PLAYING_STATUS,pdata[1]);
				}
			}
			break;
		case 0x62: //XM TEXT
			{
			
			}
			break;
		case 0x70://BT STATUS 
			{
				if(pdata[1] > 0)
				{
					dealCANAssignmentUpdata(pAssistDisplayInfo,HAND_PHONE_INF,1);
				}
				else
				{
				   dealCANAssignmentUpdata(pAssistDisplayInfo,HAND_PHONE_INF,0);
				}
			}
			break;
		case 0x71: //BT NUMBER
			{
				BYTE i;
				for(i=0;i<len-2;i++)
				{
					dealCANAssignmentUpdata(pAssistDisplayInfo,PHONE,pdata[1+i]);
				}
			}
			break;
		case 0x80:
			{
				dealCANAssignmentUpdata(pAssistDisplayInfo,VOLUME,pdata[1]);
			}
			break;   
		case 0xa0:
			{
				if(pdata[4] == 12)	
					sTimeInfo.timemode =1;
				else
					sTimeInfo.timemode =0;
				sTimeInfo.hour = pdata[3];
				sTimeInfo.minute = pdata[2];
				sTimeInfo.second = pdata[1];
			}
			break;
		default:
			break;

	} 
	/*if(pAssistDisplayInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName == CAR_CRV_12)
	{
		DealCivicInfoUpdataProcessor(pAssistDisplayInfo);
	}*/
}




