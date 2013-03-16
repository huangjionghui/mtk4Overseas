#pragma once


/*********************system.dll audio.dll video.dll*********************************/

//音频，视频源
#define  S_NEXTER_SOURCE 0xFF //非外部的音视频源 
#define  S_DEFINE_SOURCE 0xFE
#define  S_SYSTEM_SOURCE 0xFD


#define  S_AUDIO_NO_SOURCE		0x00
#define  S_AUDIO_RADIO_SOURCE	0x01
#define  S_AUDIO_CDC_SOURCE     0x02
#define  S_AUDIO_AUX_SOURCE		0x03

#define  S_AUDIO_VAP_SOURCE     0x08
#define  S_AUDIO_MP3_SOURCE     0x10

#define	 S_VIDEO_NO_SOURCE		0x00
#define  S_VIDEO_CDC_SOURCE     0x01
#define  S_VIDEO_AUX_SOURCE     0x02
#define  S_VIDEO_BACK_SOURCE    0x03


#define  SET_VOLUME			0x10
#define  SET_AUDIO			0x11
#define  SET_VIDEO			0x12
#define  SET_DVDSPK			0x13
#define  SET_DVDSPDIF		0x14
#define  SET_VIDEO_AUDIO	0x15

//声道切换
#define  BT_A2DP_ON    0x01
#define  BT_A2DP_OFF   0x00

#define  DVD_BT_ON      (0<<8 | 0x01)//DVD在通话后切进
#define  DVD_BT_OFF		(1<<8 | 0x01)//DVD在通话时切走
#define  DVD_AUDIO_ON	(0<<8 | 0x00)//DVD正常切进
#define  DVD_AUDIO_OFF	(1<<8 | 0x00)//DVD正常切走
#define  DVD_VIDEO_ON   (0<<8 | 0x01)//DVD视频切进
#define  DVD_VIDEO_OFF  (0<<8 | 0x00)//DVD视频切走
#define  DVD_AT_BT_CHANNEL  (2<<8 | 0x01)	//在蓝牙通道
#define  DVD_NOT_BT_CHANNEL (2<<8 | 0x00)	//不在蓝牙通道

#define MP3_OFF          0x00
#define MP3_AUDIO_RADIO  0x01
#define MP3_AUDIO_CDC    0x02
#define MP3_AUDIO_AUX    0x03
#define MP3_AUDIO_MP3    0x10
#define MP3_ENTER		 0x11
#define MP3_EXIT         0x12

//视频切换
#define MP3_VIDEO_CDC            0x01
#define MP3_VIDEO_AUX			 0x02
#define MP3_VIDEO_BACK_ON        (0x03<<4|0x01)
#define MP3_VIDEO_BACK_OFF       (0x03<<4|0x00)
#define MP3_VIDEO_VAP            0x08
#define MP3_VIDEO_MP3            0X10



//gps 放大增益
enum{
	GPS_VOLUME_LEVEL_1 = 30,
	GPS_VOLUME_LEVEL_2 = 50,
	GPS_VOLUME_LEVEL_3 = 60,
};

//通道
enum audio_source{
	InitAV=0,     MediaCD,/*1*/		AV_CDC, /*2*/	RADIO,   /*3*/	
	AUX,   /*4*/  IPOD,   /*5*/		TV,     /*6*/	MediaMP3,/*7*/	
	SRADIO,/*8*/  A2DP,   /*9*/		EXT_TEL,/*10*/	GR_AUDIO,/*11*/	
	BT,	   /*12*/ BACK,	  /*13*/	GPS,    /*14*/	BT_RING, /*15*/	
	CAR_RECORDER= 0x11,  VAP
};



enum enumRadioMode{FM1=0,FM2,AM};

enum EnumSendMsgModules{
	MP3_MODULE=0,
	DVD_MODULE,
	BT_MODULE,
	IPOD_MODULE,
	VIDEO_MODULE
};

//10段EQ
#define EQ_VOLUE_TOTAL_NUM 10
#define LARGER(x, y) ((x)>=(y)?(x):(y))

/*********************system.dll audio.dll video.dll END*********************************/





/****************************************FlyServiceExe.exe*********************************/

//EXE事件
const unsigned int WM_SERICE_EXE_STA_MSG		= (WM_USER + 1104);				//给service exe事件
const unsigned int WM_MP3_MODULE_STA_MSG		= (WM_USER + 1105);				//给MP3的事件
const unsigned int WM_BT_MODULE_STA_MSG			= (WM_USER + 1106);				//给蓝牙的事件
const unsigned int WM_DVD_MODULE_STA_MSG		= (WM_USER + 1107);				//给DVD的事件
const unsigned int WM_IPOD_MODULE_STA_MSG		= (WM_USER + 1108);				//给ipod的事件



//EXE相关功能ID
//MODELE ID
enum {
	SYSTEM_DLL_MODULES=0x00,
	AUDIO_DLL_MODULES,
	VIDEO_DLL_MODULES,
	FLY_DLL_MODUES_MAX
};

//exe 命令
#define CMD_GPS_PROCESS_ID   0x00
#define CMD_MP3_PROCESS_ID   0x01
#define CMD_VIDEO_SIGNED     0x02
#define CMD_DVD_INIT_STATUS  0x03
#define CMD_DVD_AUDIO_SOURCE 0x04

#define CMD_CUR_AUDIO_SOURCE SET_VIDEO_AUDIO //告诉VIDEO.DLL当前的音频通道，为了解决在AUX上的倒车问题



//告诉FlyAudioNavi.exe，FlyServiceExe.EXE程序已经加载完成的事件
#define FLY_SERVICE_EXE_DLL_LOAD_FINISH  L"FLY_SERVICE_EXE_DLL"

/*****************************************FlyServiceExe.exe END*****************************/


//功能配置

struct board_type {
	UINT16 board_version;
	UINT16 board_author;
	UINT16 board_id;

	BYTE adc_type;			//解码方式，1为内部，0为外部
	BYTE gps_audio_channel;	//gps输出声道，0为L+R，1为L，2为R，3为静音

};


#define FLY_DEBUG 1
#define DEBUG_DLL 1
#if FLY_DEBUG
#define DBGE(string) RETAILMSG(1,string)//重要错误
#define DBGW(string) if(DEBUG_DLL) RETAILMSG(1,string)//警告
#define DBGI(string) RETAILMSG(1,string)//信息
#define DBGD(string) if(DEBUG_DLL) RETAILMSG(1,string) //调试信息
#else
#define DBGE(string) RETAILMSG(1,string)//重要错误
#define DBGW(string) DEBUGMSG(1,string) //警告
#define DBGI(string) RETAILMSG(1,string) //信息
#define DBGD(string) DEBUGMSG(1,string) //调试信息
#endif
