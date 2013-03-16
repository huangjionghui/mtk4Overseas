// FLYCARAMP.cpp 
//
//>>>2012-02-04: 增加原车带功放的程序	海盗

#include "FLY7419.h"


void writeCarAmpDataToMcu(P_FLY_TDA7419_INFO pTda7419Info,BYTE *p,UINT length);
#define LARGER(A, B)    ((A) >= (B)? (A):(B))


static void CarVolumeUp(P_FLY_TDA7419_INFO pTda7419Info)
{
	BYTE  buff[2] = {0x05,0x00};
	writeCarAmpDataToMcu(pTda7419Info,buff,2);
}

static void CarVolumeDown(P_FLY_TDA7419_INFO pTda7419Info)
{
	BYTE  buff[2] = {0x05,0x01};
	writeCarAmpDataToMcu(pTda7419Info,buff,2);
}

static void CarVolumeMute(P_FLY_TDA7419_INFO pTda7419Info)
{
	BYTE  buff[2] = {0x05,0x02};
	writeCarAmpDataToMcu(pTda7419Info,buff,2);
}


static void CarTrebUp(P_FLY_TDA7419_INFO pTda7419Info)
{
	BYTE  buff[2] = {0x05,0x03};
	writeCarAmpDataToMcu(pTda7419Info,buff,2);
}


static void CarTrebDown(P_FLY_TDA7419_INFO pTda7419Info)
{
	BYTE  buff[2] = {0x05,0x04};
	writeCarAmpDataToMcu(pTda7419Info,buff,2);
}


static void CarBassUp(P_FLY_TDA7419_INFO pTda7419Info)
{
	BYTE  buff[2] = {0x05,0x05};
	writeCarAmpDataToMcu(pTda7419Info,buff,2);
}


static void CarBassDown(P_FLY_TDA7419_INFO pTda7419Info)
{
	BYTE  buff[2] = {0x05,0x06};
	writeCarAmpDataToMcu(pTda7419Info,buff,2);
}


static void CarBalanceUp(P_FLY_TDA7419_INFO pTda7419Info)
{
	BYTE  buff[2] = {0x05,0x07};
	writeCarAmpDataToMcu(pTda7419Info,buff,2);
}


static void CarBalanceDown(P_FLY_TDA7419_INFO pTda7419Info)
{
	BYTE  buff[2] = {0x05,0x08};
	writeCarAmpDataToMcu(pTda7419Info,buff,2);
}

static void CarFaderUp(P_FLY_TDA7419_INFO pTda7419Info)
{
	BYTE  buff[2] = {0x05,0x09};
	writeCarAmpDataToMcu(pTda7419Info,buff,2);
}


static void CarFaderDown(P_FLY_TDA7419_INFO pTda7419Info)
{
	BYTE  buff[2] = {0x05,0x0a};
	writeCarAmpDataToMcu(pTda7419Info,buff,2);
}

static void CarAsl(P_FLY_TDA7419_INFO pTda7419Info)
{
	BYTE  buff[2] = {0x05,0x0b};
	writeCarAmpDataToMcu(pTda7419Info,buff,2);
}


static void CarPos(P_FLY_TDA7419_INFO pTda7419Info)
{
	BYTE  buff[2] = {0x05,0x0c};
	writeCarAmpDataToMcu(pTda7419Info,buff,2);
}

static void CarMidUp(P_FLY_TDA7419_INFO pTda7419Info)
{
	BYTE  buff[2] = {0x05,0x0d};
	writeCarAmpDataToMcu(pTda7419Info,buff,2);
}


static void CarMidDown(P_FLY_TDA7419_INFO pTda7419Info)
{
	BYTE  buff[2] = {0x05,0x0e};
	writeCarAmpDataToMcu(pTda7419Info,buff,2);
}


void CarVolumeControl(P_FLY_TDA7419_INFO pTda7419Info,BYTE iVolume)
{
	BYTE  buff[3] = {0x05,0x10,0x00};

	buff[2] = iVolume;
	writeCarAmpDataToMcu(pTda7419Info,buff,3);
}

static void CarTrebControl(P_FLY_TDA7419_INFO pTda7419Info,BYTE iTreb)
{
	BYTE buff[3] = {0x05,0x11,0x00};

	buff[2] = iTreb;
	writeCarAmpDataToMcu(pTda7419Info,buff,3);
}

static void CarMidControl(P_FLY_TDA7419_INFO pTda7419Info,BYTE iMid)
{
	BYTE buff[3] = {0x05,0x12,0x00};

	buff[2] = iMid;
	writeCarAmpDataToMcu(pTda7419Info,buff,3);
}

static void CarBassControl(P_FLY_TDA7419_INFO pTda7419Info,BYTE iBass)
{
	BYTE buff[3] = {0x05,0x13,0x00};

	buff[2] = iBass;
	writeCarAmpDataToMcu(pTda7419Info,buff,3);
}
static void CarBalanceControl(P_FLY_TDA7419_INFO pTda7419Info,BYTE iBal)
{
	BYTE buff[3] = {0x05,0x14,0x00};

	buff[2] = iBal;
	writeCarAmpDataToMcu(pTda7419Info,buff,3);
}

static void CarFaderControl(P_FLY_TDA7419_INFO pTda7419Info,BYTE iFad)
{
	BYTE buff[3] = {0x05,0x15,0x00};

	buff[2] = iFad;
	writeCarAmpDataToMcu(pTda7419Info,buff,3);
}
static void CarVolumeMuteValue(P_FLY_TDA7419_INFO pTda7419Info,BYTE imute)
{
	BYTE buff[3] = {0x05,0x16,0x00};

	buff[2] = imute;
	writeCarAmpDataToMcu(pTda7419Info,buff,3);
}
static void DealSoftKeyCmd(P_FLY_TDA7419_INFO pTda7419Info,BYTE *buf,UINT len)
{
	switch(buf[1])
	{
		case 0x01://MUTE
			CarVolumeMute(pTda7419Info);	
			break;
		case 0x02://Balance+
			CarBalanceUp(pTda7419Info);
			break;
		case 0x03://Balance-
			CarBalanceDown(pTda7419Info);
			break;
		case 0x04://fader+
			CarFaderUp(pTda7419Info);
			break;
		case 0x05://fader-
			CarFaderDown(pTda7419Info);
			break;
		case 0x06://treb freq
			break;
		case 0x07://treble+
			CarTrebUp(pTda7419Info);
			break;
		case 0x08://treble-
			CarTrebDown(pTda7419Info);
			break;
		case 0x09://mid freq
			break;
		case 0x0A://mid+
			CarMidUp(pTda7419Info);
			break;
		case 0x0B: //mid-
			CarMidDown(pTda7419Info);
			break;
		case 0x0C://bass freq
			break;
		case 0x0D://bass+
			CarBassUp(pTda7419Info);
			break;
		case 0x0E://bass-
			CarBassDown(pTda7419Info);
			break;
		case 0x0F://loud freq
			break;
		case 0x10://loud +
			break;
		case 0x11://loud -
			break;
		case 0x12://sub +
			break;
		case 0x13://sub -
			break;
		default:
			break;
	}
}
void DealVolumeData(P_FLY_TDA7419_INFO pTda7419Info,BYTE bAction)
{
	if(bAction == 0)
	{
		CarVolumeMute(pTda7419Info);
	}
	else if(bAction == 1)
	{
		CarVolumeUp(pTda7419Info);
	}
	else if(bAction == 2)
	{
		CarVolumeDown(pTda7419Info);
	}
}
void DealEqControlData(P_FLY_TDA7419_INFO pTda7419Info,BYTE *buf,UINT len)
{
	switch(buf[0])
	{
		case 0x03:
			DealSoftKeyCmd(pTda7419Info,buf,len);
			break; 
		case 0x11://mute
			CarVolumeMuteValue(pTda7419Info,buf[1]);	
			break;
		case 0x12://volume
			CarVolumeControl(pTda7419Info,buf[1]); 
			break;
		case 0x13://balance
			CarBalanceControl(pTda7419Info,buf[1]); 
			break;
		case 0x14://fader
			CarFaderControl(pTda7419Info,buf[1]);
			break;
		case 0x15://treb freq
			break;
		case 0x16://treb
			CarTrebControl(pTda7419Info,buf[1]);
			break;
		case 0x17://mid freq
			break;
	    case 0x18://mid
	    	CarMidControl(pTda7419Info,buf[1]);
			break;
		case 0x19 ://bass freq
			break;
		case 0x1A://bass
			CarBassControl(pTda7419Info,buf[1]);
			break;
		default:
			DBGD((TEXT("\r\nTda7419 user command unhandle %X"),buf[0]));
			break;
	}		
}

