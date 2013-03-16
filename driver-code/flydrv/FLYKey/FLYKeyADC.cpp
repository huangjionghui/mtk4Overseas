// -----------------------------------------------------------------------------
// File Name    : FLYKeyADC.cpp
// Title        : FlyKeyADC Driver
// Author       : JQilin - Copyright (C) 2011
// Created      : 2011-02-25  
// Version      : 0.01
// Target MCU   : WinCE Driver
// -----------------------------------------------------------------------------
// Version History:
/*
>>>2011-04-25: V0.02 适应新的硬件
>>>2011-02-25: V0.01 first draft
*/

#include "FLYKey.h"
#include "flyhwlib.h"
#include "flyshmlib.h"


BYTE RemoteKEY_TabL[] = {
	KB_AV,KB_SEEK_INC2,KB_SEEK_DEC2,
	KB_MUTE,KB_VOL_INC,KB_VOL_DEC,
	KB_CALL_OUT,KB_CALL_REJECT,KB_CALL_INOUT,
	KB_RADIO,KB_SCAN,KB_PAUSE,KB_SLEEP,KB_FM,
	KB_SEEK_INC2,KB_SEEK_DEC2,KB_SPEECH_IDENTIFY_START,
	KB_NAVI,
};
BYTE RemoteKEY_TabS[] = {
	KB_AV,KB_SEEK_INC,KB_SEEK_DEC,
	KB_MUTE,KB_VOL_INC,KB_VOL_DEC,
	KB_CALL_OUT,KB_CALL_REJECT,KB_CALL_INOUT,
	KB_RADIO,KB_SCAN,KB_PAUSE,KB_SLEEP,KB_FM,
	KB_SEEK_INC2,KB_SEEK_DEC2,KB_SPEECH_IDENTIFY_END,
	KB_NAVI,
};

BYTE proKeyID[5];
ULONG probKeyDownTime[5];
UINT probKeyDownCount[5];
BOOL proKeyContinuePro[5];

void processorFlyAudioKey(P_FLY_KEY_INFO pKeyInfo,BYTE channel,BYTE key,BOOL bDown,BOOL bRomoteOrPanel);

static void SteelwheelStudyProc(P_FLY_KEY_INFO pKeyInfo);
static void steelwheelChangeStudyToNormal(P_FLY_KEY_INFO pKeyInfo);

void DemoKeyMessage(P_FLY_KEY_INFO pKeyInfo)
{
	BYTE buff[2];
	UINT iRemain;
	ULONG nowTimer;

	nowTimer = GetTickCount();

	//if (nowTimer - pKeyInfo->iKeyDemoInnerTime >= 500)
	if (nowTimer - pKeyInfo->iKeyDemoInnerTime >= 150)
	{
		pKeyInfo->iKeyDemoInnerTime = nowTimer;
		pKeyInfo->iKeyDemoState++;

		iRemain = pKeyInfo->iKeyDemoState % 128;//音量
		if (iRemain < 8)
		{
			buff[0] = KB_VOL_INC;
			FlyKeyReturnToUser(pKeyInfo,buff,1,FALSE);
		}
		else if (iRemain >= 64 && iRemain < 72)
		{
			buff[0] = KB_VOL_DEC;
			FlyKeyReturnToUser(pKeyInfo,buff,1,FALSE);
		}

		iRemain = pKeyInfo->iKeyDemoState % 17;//AV切换
		if (0 == iRemain)
		{
			buff[0] = KB_AV;
			FlyKeyReturnToUser(pKeyInfo,buff,1,FALSE);
		}

		iRemain = pKeyInfo->iKeyDemoState % 157;//进出碟
		if (0 == iRemain)
		{
			buff[0] = KB_DVD_OPEN;
			FlyKeyReturnToUser(pKeyInfo,buff,1,FALSE);
		}
	}
}

/*
上拉电阻：12K
键值电阻：    2.1K / 5.2K / 10K / 18.4K  / 37.5K  / 120K
7521有一个特殊电阻2.7K代替2.1K

*/
#if A4_PLAT
#define IO_4052ADSWITCH_G	0   //A4 方向盘和面板复用
#define IO_4052ADSWITCH_I	23
#endif

DWORD WINAPI ThreadFlyKeyADC(LPVOID pContext)
{
	P_FLY_KEY_INFO pKeyInfo = (P_FLY_KEY_INFO)pContext;
	ULONG WaitReturn;
	BYTE i,j;
	UINT SelectADValue;
	BYTE SelectRemote;
	UINT lastADValue[5];
	BOOL longThanShortTimer[5];
	ULONG lastChangeADTimer[5];
	
	ULONG debugUartADTime = 0;

	UINT16 adctest;

	DBGD((TEXT("\r\nThreadFlyKeyADC Priority:%d"),CeGetThreadPriority(GetCurrentThread())));
	CeSetThreadPriority(GetCurrentThread(),109);
	DBGD((TEXT("\r\nThreadFlyKeyADC Priority:%d"),CeGetThreadPriority(GetCurrentThread())));

	for (i = 0;i < 5;i++)
	{
		proKeyID[i] = 0;
		probKeyDownTime[i] = 0;
		probKeyDownCount[i] = 0;
		proKeyContinuePro[i] = FALSE;
		lastADValue[i] = 0;
		longThanShortTimer[i] = 0;
		lastChangeADTimer[i] = 0;
	}

	while (!pKeyInfo->bKillDispatchKeyADCThread)
	{
		WaitReturn = WaitForSingleObject(pKeyInfo->hDispatchThreadADCEvent, KEY_SCAN_SPACE);

		//AD采集三路电压
		#if A4_PLAT
		HwGpioOutput(IO_4052ADSWITCH_G,IO_4052ADSWITCH_I,0);
		Sleep(3);
		#endif
		for (i = 0;i < 3;i++)
		{
			if (0 == i)
			{
				HwADCGetValue(PANEL_ADC_CHANNEL1,&pKeyInfo->CurrentAD[0]);
			}
			else if (1 == i)
			{
				HwADCGetValue(PANEL_ADC_CHANNEL2,&pKeyInfo->CurrentAD[1]);
			}
			else if (2 == i)
			{
				HwADCGetValue(PANEL_ADC_CHANNEL3,&pKeyInfo->CurrentAD[2]);
			}

			//if (pKeyInfo->CurrentAD[i] < 5)
			//{
			//	//DBGD((TEXT("\r\nCurrentAD[%d]=%d"),i,pKeyInfo->CurrentAD[i]));
			//	continue;
			//	
			//}

			pKeyInfo->nowTimer = GetTickCount();

			pKeyInfo->CurrentAD[i] = pKeyInfo->CurrentAD[i] >> 2;
			pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelKeyAD[i] = pKeyInfo->CurrentAD[i];
			SelectADValue = pKeyInfo->CurrentAD[i];
			adctest= SelectADValue;

			if (0x01 == pKeyInfo->tPanelTab.type)
			{
				if (
					('2' == pKeyInfo->tPanelTab.PanelName[4] && '1' == pKeyInfo->tPanelTab.PanelName[5])
					||
					('4' == pKeyInfo->tPanelTab.PanelName[4] && '8' == pKeyInfo->tPanelTab.PanelName[5])
					)
				{
					if ((SelectADValue>=(47-13))&&(SelectADValue<=(47+13))) SelectADValue=1;   //256--47		1024--188   
					else if ((SelectADValue>=(233-13))&&(SelectADValue<=(233+13))) SelectADValue=6;   //256--233		1024--931   
					else if ((SelectADValue>=(194-13))&&(SelectADValue<=(194+13))) SelectADValue=5;   //256--194		1024--776
					else if ((SelectADValue>=(155-13))&&(SelectADValue<=(155+13))) SelectADValue=4;   //256--155		1024--620
					else if ((SelectADValue>=(116-13))&&(SelectADValue<=(116+13))) SelectADValue=3;   //256--116		1024--465
					else if ((SelectADValue>=(77-13))&&(SelectADValue<=(77+13)))   SelectADValue=2;   //256--77		1024--310
					else if (/*(SelectADValue>=(10-10))&&*/(SelectADValue<=(10+13)))    SelectADValue=7;   //256--10        1024--39
					else SelectADValue=0;//无效数据。
				} 
				else
				{
					if ((SelectADValue>=(38-13))&&(SelectADValue<=(38+13)))   SelectADValue=1;   //256--38		1024--153
					else if ((SelectADValue>=(233-13))&&(SelectADValue<=(233+13))) SelectADValue=6;   //256--233		1024--931   
					else if ((SelectADValue>=(194-13))&&(SelectADValue<=(194+13))) SelectADValue=5;   //256--194		1024--776
					else if ((SelectADValue>=(155-13))&&(SelectADValue<=(155+13))) SelectADValue=4;   //256--155		1024--620
					else if ((SelectADValue>=(116-13))&&(SelectADValue<=(116+13))) SelectADValue=3;   //256--116		1024--465
					else if ((SelectADValue>=(77-13))&&(SelectADValue<=(77+13)))   SelectADValue=2;   //256--77		1024--310
					else if (/*(SelectADValue>=(10-10))&&*/(SelectADValue<=(10+13)))    SelectADValue=7;   //256--10        1024--39
					else SelectADValue=0;//无效数据。
				}

				//RETAILMSG(1,(TEXT("lastADValue[%d]=%d,SelectADValue%d,ADC=%d"), i,lastADValue[i],SelectADValue,adctest));

				if (lastADValue[i] != SelectADValue)//变化
				{
					TestLEDBlink(pKeyInfo);
					//RETAILMSG(1, (TEXT("@%d%d"),i,SelectADValue));
					if (lastADValue[i] && longThanShortTimer[i])//达到有效时间后弹起
					{

						//RETAILMSG(1,(TEXT("key_Tab_short:%d,i=%d,SelectADValue:%d"),key_Tab_short[i][lastADValue[i]-1]-1, i,SelectADValue));
						processorFlyAudioKey(pKeyInfo,i,pKeyInfo->tPanelTab.KeyValue[key_Tab_short[i][lastADValue[i]-1]-1],FALSE,FALSE);
					}
					lastADValue[i] = SelectADValue;
					lastChangeADTimer[i] = pKeyInfo->nowTimer;
					longThanShortTimer[i] = FALSE;
				}
				else if (lastADValue[i])//有效
				{
					//RETAILMSG(1,(TEXT("channel:%d, ADC=%d "), i,adctest));
					//if ((nowTimer - lastChangeADTimer[i]) >= SHORT_KEY_MIN)//达到有效时间
					//{
						longThanShortTimer[i] = TRUE;
						processorFlyAudioKey(pKeyInfo,i,pKeyInfo->tPanelTab.KeyValue[key_Tab_short[i][lastADValue[i]-1]-1],TRUE,FALSE);
						pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iKeyIndex = key_Tab_short[i][lastADValue[i]-1];
						pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iKeyValue = pKeyInfo->tPanelTab.KeyValue[key_Tab_short[i][lastADValue[i]-1]-1];
					//}
				}
								
			}
		}


		//UINT32 iTempReadAD;
		//HwADCGetValue(STEEL_ADC_CHANNEL1,&iTempReadAD);
		//DBGD((TEXT("\r\n get adc %d-adc:%d"),STEEL_ADC_CHANNEL1,iTempReadAD));
		//HwADCGetValue(STEEL_ADC_CHANNEL2,&iTempReadAD);
		//DBGD((TEXT("\r\n get adc %d-adc:%d"),STEEL_ADC_CHANNEL2,iTempReadAD));


		//按键DEMO
		if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bKeyDemoMode)
		{
			DemoKeyMessage(pKeyInfo);
		}
		#if A4_PLAT
		HwGpioOutput(IO_4052ADSWITCH_G,IO_4052ADSWITCH_I,1);
		Sleep(3);
		#endif


		if (pKeyInfo->remoteStudyStart)
		{
			SteelwheelStudyProc(pKeyInfo);
		}
		else
		{
			if(pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iCarName == CAR_CAMRY_12)
			{
				UINT temp,temp1;
				ULONG time;
				static BYTE Remote3=0;
				static ULONG Remote3Timer;
				BYTE buff[2];

				HwADCGetValue(STEEL_ADC_CHANNEL1,&temp);
				HwADCGetValue(STEEL_ADC_CHANNEL2,&temp1);
				if(temp1<30)
				{
					temp1 = 30;
				}
				if((temp1-30) <= temp && temp<= (temp1+30) && temp < 980) 
				{						
					if(100 <= temp && temp <= 260)
					{
						if (Remote3 == 0) 
						{
							Remote3 = 1; 
							buff[0] =KB_NAVI;
							FlyKeyReturnToUser(pKeyInfo,buff,1,TRUE);
							Remote3Timer = GetTickCount();
						}
						time = GetTickCount()-Remote3Timer;	
						if (time > 600 ) 
						{
							Remote3 = 0;		 
							Remote3Timer = GetTickCount();	
						}	   	  
					}
					else if(300 < temp && temp < 450)
					{		    
						if (Remote3 == 0) 
						{
							Remote3 = 1;
							Remote3Timer = GetTickCount();	
						}
						time = GetTickCount()-Remote3Timer;		
						if (time > SHORT_KEY_MIN ) 
						{
							Remote3 = 0;
							buff[0] =KB_MENU;
							FlyKeyReturnToUser(pKeyInfo,buff,1,TRUE);
							Remote3Timer = GetTickCount();	
						}
					}
					else if(500 < temp && temp < 650)
					{
						if (Remote3 == 0) 
						{
							Remote3 = 1;
							Remote3Timer = GetTickCount();	
						}
						time = GetTickCount()-Remote3Timer;	
						if (time >140 ) 
						{
							Remote3 = 0;
							buff[0] =KB_TUNE_INC;
							FlyKeyReturnToUser(pKeyInfo,buff,1,TRUE);
							Remote3Timer = GetTickCount();		
						}
					}
					else if(700 < temp && temp < 860)
					{
						if (Remote3 == 0) 
						{
							Remote3 = 1;
							Remote3Timer = GetTickCount();	
						}
						time = GetTickCount()-Remote3Timer;	
						if (time >140 ) 
						{
							Remote3 = 0;
							buff[0] =KB_TUNE_DEC;
							FlyKeyReturnToUser(pKeyInfo,buff,1,TRUE);
							Remote3Timer = GetTickCount();		
						}
					}
				}
				else
				{
					for (i = 3;i < 5;i++)
					{
						if (3 == i)
						{
							HwADCGetValue(STEEL_ADC_CHANNEL1,&pKeyInfo->CurrentAD[3]);
							SelectRemote = 1;
						}
						else if (4 == i)
						{
							HwADCGetValue(STEEL_ADC_CHANNEL2,&pKeyInfo->CurrentAD[4]);
							SelectRemote = 2;
						}

						pKeyInfo->nowTimer = GetTickCount();

						pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSteelAD[i-3] = pKeyInfo->CurrentAD[i];

						SelectADValue = pKeyInfo->CurrentAD[i];

						if ((1 == SelectRemote && SelectADValue >= pKeyInfo->remote1ADCS)
							|| (2 == SelectRemote && SelectADValue >= pKeyInfo->remote2ADCS))
						{
							SelectADValue = IRKEYTABSIZE;
						}
						else
						{
							for (j = 0;j < pKeyInfo->remoteTab.size;j++)
							{
								if (SelectRemote == pKeyInfo->remoteTab.IRKEY_Port[j])
								{
									if (SelectADValue >= pKeyInfo->remoteTab.IRKEY_AD_Min[j] && SelectADValue <= pKeyInfo->remoteTab.IRKEY_AD_Max[j])
									{
										SelectADValue = pKeyInfo->remoteTab.IRKEY_Value[j];
										break;
									}
								}
							}
							if (j == pKeyInfo->remoteTab.size)
							{
								SelectADValue = IRKEYTABSIZE;
							}
						}

						if (lastADValue[i] != SelectADValue)//变化
						{
							if ((lastADValue[i] < sizeof(RemoteKEY_TabS)) && longThanShortTimer[i])//达到有效时间后弹起
							{
								processorFlyAudioKey(pKeyInfo,i,RemoteKEY_TabS[lastADValue[i]],FALSE,TRUE);
							}
							lastADValue[i] = SelectADValue;
							lastChangeADTimer[i] = pKeyInfo->nowTimer;
							longThanShortTimer[i] = FALSE;
						}
						else if (lastADValue[i] < sizeof(RemoteKEY_TabS))//有效
						{
							if ((pKeyInfo->nowTimer - lastChangeADTimer[i]) >= SHORT_KEY_MIN)//达到有效时间
							{
								longThanShortTimer[i] = TRUE;
								processorFlyAudioKey(pKeyInfo,i,RemoteKEY_TabS[lastADValue[i]],TRUE,TRUE);
							}
						}
					}	

				}
						
			}
			else
			{
				for (i = 3;i < 5;i++)
				{
					if (3 == i)
					{
						HwADCGetValue(STEEL_ADC_CHANNEL1,&pKeyInfo->CurrentAD[3]);
						SelectRemote = 1;
					}
					else if (4 == i)
					{
						HwADCGetValue(STEEL_ADC_CHANNEL2,&pKeyInfo->CurrentAD[4]);
						SelectRemote = 2;
					}
				
					pKeyInfo->nowTimer = GetTickCount();

					pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSteelAD[i-3] = pKeyInfo->CurrentAD[i];
					SelectADValue = pKeyInfo->CurrentAD[i];

					if ((1 == SelectRemote && SelectADValue >= pKeyInfo->remote1ADCS)
						|| (2 == SelectRemote && SelectADValue >= pKeyInfo->remote2ADCS))
					{
						SelectADValue = IRKEYTABSIZE;
					}
					else
					{
						for (j = 0;j < pKeyInfo->remoteTab.size;j++)
						{
							if (SelectRemote == pKeyInfo->remoteTab.IRKEY_Port[j])
							{
								if (SelectADValue >= pKeyInfo->remoteTab.IRKEY_AD_Min[j] && SelectADValue <= pKeyInfo->remoteTab.IRKEY_AD_Max[j])
								{
									SelectADValue = pKeyInfo->remoteTab.IRKEY_Value[j];
									break;
								}
							}
						}

						if (j == pKeyInfo->remoteTab.size)
						{
							SelectADValue = IRKEYTABSIZE;
						}
					}

					if (lastADValue[i] != SelectADValue)//变化
					{
						if ((lastADValue[i] < sizeof(RemoteKEY_TabS)) && longThanShortTimer[i])//达到有效时间后弹起
						{
							processorFlyAudioKey(pKeyInfo,i,RemoteKEY_TabS[lastADValue[i]],FALSE,TRUE);
						}
						lastADValue[i] = SelectADValue;
						lastChangeADTimer[i] = pKeyInfo->nowTimer;
						longThanShortTimer[i] = FALSE;
					}
					else if (lastADValue[i] < sizeof(RemoteKEY_TabS))//有效
					{
						if ((pKeyInfo->nowTimer - lastChangeADTimer[i]) >= SHORT_KEY_MIN)//达到有效时间
						{
							longThanShortTimer[i] = TRUE;
							processorFlyAudioKey(pKeyInfo,i,RemoteKEY_TabS[lastADValue[i]],TRUE,TRUE);
						}
					}
				}
			}
		}

		if (GetTickCount() - debugUartADTime > 1000)
		{
				debugUartADTime = GetTickCount();
				DBGE((TEXT("\r\nAD1:%d AD2:%d AD3:%d AD4:%d AD5:%d")
					,pKeyInfo->CurrentAD[0]
					,pKeyInfo->CurrentAD[1]
					,pKeyInfo->CurrentAD[2]
					,pKeyInfo->CurrentAD[3]
					,pKeyInfo->CurrentAD[4]));
		}
	}
	pKeyInfo->FlyKeyADCThreadHandle = NULL;
	DBGD((TEXT("\r\nFlyKey ThreadFlyKeyADC exit")));
	return 0;
}

typedef struct _KEY_ATTRIBUTION
{
	BYTE KeyID;
	BOOL DownAttribution;
	BOOL LongAttribution;
	UINT LongStartCount;
	UINT LongSpaceCount;
	BOOL UpAttribution;
}KEY_ATTRIBUTION, *P_KEY_ATTRIBUTION;

KEY_ATTRIBUTION tKeyAttribution[] = 
{
	{KB_MUTE,TRUE,TRUE,1200,100000,TRUE},
	{KB_TUNE_DOWN,FALSE,TRUE,1200,100000,TRUE},
	{KB_AV,TRUE,TRUE,800,1000,FALSE},
	{KB_VOL_INC,TRUE,TRUE,300,50,FALSE},
	{KB_VOL_DEC,TRUE,TRUE,300,50,FALSE},
	{KB_SPEECH_IDENTIFY_END,FALSE,TRUE,2000,100000,TRUE},
	{0,FALSE,FALSE,0,0,FALSE}
};

void processorFlyAudioKey(P_FLY_KEY_INFO pKeyInfo,BYTE channel,BYTE key,BOOL bDown,BOOL bRomoteOrPanel)
{
	BYTE buff[2];
	UINT i;

	if (key == 0)
		return;


	DBGD((TEXT("\r\nchannel:%d,Key:%02X,Down:%d\r\n"),channel,key,bDown));

	if (bDown)
	{
		if (KB_MUTE == key)
		{
			if (0 == pKeyInfo->iMutePressTime)
			{
				pKeyInfo->iMutePressTime = GetTickCount();
			}
			else
			{
				if (GetTickCount() - pKeyInfo->iMutePressTime >= 4000)
				{
					pKeyInfo->iMutePressTime = GetTickCount();
					IpcStartEvent(EVENT_GLOBAL_FORCE_RESET_ID);

					HwGpioOutput(CONTROL_IO_LED_I,EJECT_LED_ON);
					Sleep(618);
					HwGpioOutput(CONTROL_IO_LED_I,EJECT_LED_OFF);
				}
			}
		}
		else
		{
			pKeyInfo->iMutePressTime = 0;
		}
	}
	else
	{
		pKeyInfo->iMutePressTime = 0;
	}

	if (FALSE == bDown)//弹起
	{
		i = 0;
		while (1)
		{
			if (proKeyID[channel] == tKeyAttribution[i].KeyID)//符合
			{
				if (tKeyAttribution[i].UpAttribution)//有弹起特性
				{
					if (KB_MUTE == proKeyID[channel])//KB_MUTE按键处理
					{
						if (proKeyContinuePro[channel])//如果没有屏蔽
						{
							if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)//待机状态下只有一种选择
							{
								buff[0] = KB_SLEEP;
								FlyKeyReturnToUser(pKeyInfo,buff,1,bRomoteOrPanel);
							}
							else if ((pKeyInfo->nowTimer - probKeyDownTime[channel]) < tKeyAttribution[i].LongStartCount)
							{
								buff[0] = KB_MUTE;
								FlyKeyReturnToUser(pKeyInfo,buff,1,bRomoteOrPanel);
							}
							else
							{
								buff[0] = KB_SLEEP;
								FlyKeyReturnToUser(pKeyInfo,buff,1,bRomoteOrPanel);
							}
						}
					}
					else if(KB_SPEECH_IDENTIFY_END == proKeyID[channel])
					{
						if (proKeyContinuePro[channel])//如果没有屏蔽
						{
							if ((pKeyInfo->nowTimer - probKeyDownTime[channel]) < tKeyAttribution[i].LongStartCount)
							{
								buff[0] = KB_SPEECH_IDENTIFY_START;
								FlyKeyReturnToUser(pKeyInfo,buff,1,bRomoteOrPanel);
							}
						}
					}
					else
					{
						buff[0] = proKeyID[channel];
						if (KB_TUNE_DOWN == buff[0])
						{
							if (proKeyContinuePro[channel])
							{
								FlyKeyReturnToUser(pKeyInfo,buff,1,bRomoteOrPanel);
							}
						}
						else
						{
							FlyKeyReturnToUser(pKeyInfo,buff,1,bRomoteOrPanel);
						}
					}
				}
				break;
			}
			else if (0 == tKeyAttribution[i].KeyID)//表里没有则缺省
			{
				break;
			}
			i++;
		}
		probKeyDownTime[channel] = 0;//清零
		probKeyDownCount[channel] = 0;//清零
		proKeyContinuePro[channel] = FALSE;
	}
	else//按下
	{
		proKeyID[channel] = key;
		if (0 == probKeyDownTime[channel])//初次按下
		{
			proKeyContinuePro[channel] = TRUE;
			probKeyDownTime[channel] = pKeyInfo->nowTimer;
			probKeyDownCount[channel] = 0;

			i = 0;
			while (1)
			{
				if (proKeyID[channel] == tKeyAttribution[i].KeyID)//符合
				{
					if (tKeyAttribution[i].DownAttribution)//有按下特性
					{
						if (KB_MUTE == proKeyID[channel])
						{
							if (proKeyContinuePro[channel])
							{
								if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)//待机状态下只有一种选择
								{
									proKeyContinuePro[channel] = FALSE;
									buff[0] = KB_SLEEP;
									FlyKeyReturnToUser(pKeyInfo,buff,1,bRomoteOrPanel);
								}
							}
						}
						else
						{
							buff[0] = proKeyID[channel];
							FlyKeyReturnToUser(pKeyInfo,buff,1,bRomoteOrPanel);
						}
					}
					break;
				}
				else if (0 == tKeyAttribution[i].KeyID)//表里没有则缺省
				{
					buff[0] = proKeyID[channel];
					FlyKeyReturnToUser(pKeyInfo,buff,1,bRomoteOrPanel);
					break;
				}
				i++;
			}
		}
		else//连续按下
		{
			i = 0;
			while (1)
			{
				if (proKeyID[channel] == tKeyAttribution[i].KeyID)//符合
				{
					if (tKeyAttribution[i].LongAttribution)//有长按特性
					{
						if ((pKeyInfo->nowTimer - probKeyDownTime[channel]) >= tKeyAttribution[i].LongStartCount)
						{
							buff[0] = proKeyID[channel];
							if (0 == probKeyDownCount[channel])//长按启动
							{
								if (KB_MUTE == buff[0])//只做进入待机
								{
									if (proKeyContinuePro[channel])
									{
										if (!pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bStandbyStatus)
										{
											proKeyContinuePro[channel] = FALSE;
											buff[0] = KB_SLEEP;
											FlyKeyReturnToUser(pKeyInfo,buff,1,bRomoteOrPanel);
										}
									}
								}
								else if (KB_TUNE_DOWN == buff[0])
								{
									if (proKeyContinuePro[channel])
									{
										proKeyContinuePro[channel] = FALSE;
										buff[0] = KB_TUNE_DOWN_LONG;
										FlyKeyReturnToUser(pKeyInfo,buff,1,bRomoteOrPanel);
									}
								}
								else if(KB_SPEECH_IDENTIFY_END == buff[0])
								{
									if (proKeyContinuePro[channel])
									{
										proKeyContinuePro[channel] = FALSE;
										buff[0] = KB_SPEECH_IDENTIFY_END;
										FlyKeyReturnToUser(pKeyInfo,buff,1,bRomoteOrPanel);
									}
								}
								else
								{
									FlyKeyReturnToUser(pKeyInfo,buff,1,bRomoteOrPanel);
								}
								probKeyDownCount[channel]++;
							}
							else//长按连续
							{
								UINT iStepTemp = (pKeyInfo->nowTimer - probKeyDownTime[channel] - tKeyAttribution[i].LongStartCount)/tKeyAttribution[i].LongSpaceCount;
								while (probKeyDownCount[channel] <= iStepTemp)
								{
									FlyKeyReturnToUser(pKeyInfo,buff,1,bRomoteOrPanel);
									probKeyDownCount[channel]++;
								}
							}
						}
					}
					break;
				}
				else if (0 == tKeyAttribution[i].KeyID)//表里没有则缺省
				{
					break;
				}
				i++;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////方向盘按键部分

//SKE06-32
//BYTE remoteData[] = {5,1,0,750,40,2,1,263,40,2,2,512,40,1,4,230,40,1,5,0,40};
//BYTE remoteData[] = {
//5
//,0x06,0x32
//,1,0,0x02,0xee,40
//,2,1,0x01,0x07,40
//,2,2,0x02,0x00,40
//,1,4,0x00,0xe6,40
//,1,5,0x00,0x00,40};

//SKE25
//BYTE remoteData[] = {
//8
//,2,0,0,40
//,1,1,0,40
//,1,2,133,40
//,2,3,593,40
//,1,4,320,40
//,1,5,598,40
//,2,7,126,40
//,2,8,319,40};
BYTE remoteData[] = {
	8
	,0x25,0x00
	,2,0,0x00,0x00,40
	,1,1,0x00,0x00,40
	,1,2,0x00,0x85,40
	,2,3,0x02,0x51,40
	,1,4,0x01,0x40,40
	,1,5,0x02,0x56,40
	,2,7,0x00,0x7E,40
	,2,8,0x01,0x3F,40};

void SteelwheelKeyADCSInit(P_FLY_KEY_INFO pKeyInfo)
{
	UINT i;

	pKeyInfo->remote1ADCS = 0;pKeyInfo->remote2ADCS = 0;//有些车型有下拉电阻
	for (i=0;i<pKeyInfo->remoteTab.size;i++)
	{
		if (pKeyInfo->remoteTab.IRKEY_Port[i] == 1)
		{
			if (pKeyInfo->remoteTab.IRKEY_AD_Max[i] > pKeyInfo->remote1ADCS)
			{
				pKeyInfo->remote1ADCS = pKeyInfo->remoteTab.IRKEY_AD_Max[i];
			}
		}
		else if (pKeyInfo->remoteTab.IRKEY_Port[i] == 2)
		{
			if (pKeyInfo->remoteTab.IRKEY_AD_Max[i] > pKeyInfo->remote2ADCS)
			{
				pKeyInfo->remote2ADCS = pKeyInfo->remoteTab.IRKEY_AD_Max[i];
			}
		}
	}
	if(pKeyInfo->remote1ADCS == 0 || pKeyInfo->remote1ADCS > 1005)pKeyInfo->remote1ADCS = 1005;
	if(pKeyInfo->remote2ADCS == 0 || pKeyInfo->remote2ADCS > 1005)pKeyInfo->remote2ADCS = 1005;
	DBGD((TEXT("\r\nremote1ADCS:%d remote2ADCS:%d"),pKeyInfo->remote1ADCS,pKeyInfo->remote2ADCS));
}

void SteelwheelKeyDataInit(P_FLY_KEY_INFO pKeyInfo,BYTE *data)
{
	BYTE i;
	UINT advalue;
	UINT offset;
	UINT a,b;

	pKeyInfo->remoteTab.size = data[0];
	if (pKeyInfo->remoteTab.size > IRKEYTABSIZE) {
		pKeyInfo->remoteTab.size = 0;
	}
	pKeyInfo->remoteTab.CarTypeID[0] = data[1];
	pKeyInfo->remoteTab.CarTypeID[1] = data[2];

	for(i=0;i<pKeyInfo->remoteTab.size;i++) {
		if (data[i*5+3] >= 0x10) {
			pKeyInfo->remoteTab.IRKEY_Value[i] = data[i*5+1+3];
			pKeyInfo->remoteTab.IRKEY_Port[i] = data[i*5+3] - 0x10;
			advalue = (data[i*5+2+3]<<8) + data[i*5+3+3];
			offset = data[i*5+4+3];
			b = (advalue > offset)?(advalue - offset):0;
			if (((REMOTE_R_UP+REMOTE_R_BASE) * b) <= (1024*REMOTE_R_BASE)) { a = 0;}
			else { 
				a = 1024*((REMOTE_R_UP+REMOTE_R_BASE) * b - 1024*REMOTE_R_BASE);
				a = a / (REMOTE_R_BASE*b + (REMOTE_R_UP-REMOTE_R_BASE) *1024);

			}
			pKeyInfo->remoteTab.IRKEY_AD_Min[i] = a;

			b = advalue + offset;
			a = ((REMOTE_R_UP+REMOTE_R_BASE) * b > 1024*REMOTE_R_BASE)?((REMOTE_R_UP+REMOTE_R_BASE) * b - 1024*REMOTE_R_BASE):0;
			pKeyInfo->remoteTab.IRKEY_AD_Max[i] = 1024*a / (REMOTE_R_BASE*b + (REMOTE_R_UP-REMOTE_R_BASE) *1024);
		}	   //1819432960
		else {
			pKeyInfo->remoteTab.IRKEY_Value[i] = data[i*5+1+3];
			pKeyInfo->remoteTab.IRKEY_Port[i] = data[i*5+3];
			advalue = (data[i*5+2+3]<<8) + data[i*5+3+3];
			offset = data[i*5+4+3];
			pKeyInfo->remoteTab.IRKEY_AD_Min[i] = (advalue>offset)?(advalue - offset):0; 
			pKeyInfo->remoteTab.IRKEY_AD_Max[i] = advalue + offset;
		}
	}

	SteelwheelKeyADCSInit(pKeyInfo);

	for (i = 0;i < pKeyInfo->remoteTab.size;i++)
	{
		DBGD((TEXT("\r\n size:%d  Value:%x Port:%d Min:%d Max:%d ")
			,pKeyInfo->remoteTab.size
			,pKeyInfo->remoteTab.IRKEY_Value[i]
			,pKeyInfo->remoteTab.IRKEY_Port[i]
			,pKeyInfo->remoteTab.IRKEY_AD_Min[i]
			,pKeyInfo->remoteTab.IRKEY_AD_Max[i]));
	}
}

void remoteKeyInit(P_FLY_KEY_INFO pKeyInfo)
{
	SteelwheelStudyRegDataRead(pKeyInfo);
	if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRemoteUseStudyOn)
	{
		RETAILMSG(1, (TEXT("\r\nremote Use Study")));
		steelwheelChangeStudyToNormal(pKeyInfo);
	}
	else
	{
		if (pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bRemoteDataHave)
		{
			RETAILMSG(1, (TEXT("\r\nremote Use List")));
			SteelwheelKeyDataInit(pKeyInfo
				,(BYTE*)pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sRemoteData);
		}
		else
		{
			RETAILMSG(1, (TEXT("\r\nremote Use Code Default")));
			SteelwheelKeyDataInit(pKeyInfo,remoteData);
		}
	}
	pKeyInfo->remoteStudyStart = FALSE;
	pKeyInfo->remoteStudyStep = 0;
	pKeyInfo->remoteStudyCount = 0;
}

void SteelwheelStudyRegDataRead(P_FLY_KEY_INFO pKeyInfo)
{
	HKEY hKey; 
	DWORD dwDisposition;
	DWORD dwTemp,dwLen, dwType;
	DWORD Value,Port,AD;

	memset(&pKeyInfo->remoteStudyTab,0,sizeof(IRKEY_STUDY_TAB));

#if REGEDIT_NOT_READ
	return;
#endif

	//读取  方向盘学习   参数
	RegCreateKeyEx(HKEY_CURRENT_USER,L"FlyDrivers\\Key\\SteelWheelStudy",0,NULL,0,
		0,NULL,&hKey,&dwDisposition); /////////////////////////////////创建或打开注册表

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_AVbStudy",0,&dwType,(unsigned char *)&dwTemp,&dwLen))//倒车检测
	{
		if (dwTemp)
		{
			Value = KB_AV;

			dwTemp = 0;
			dwLen = 4;
			if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_AVPort",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
			{
				Port = dwTemp;

				dwTemp = 0;
				dwLen = 4;
				if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_AVAD",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
				{
					AD = dwTemp;

					pKeyInfo->remoteStudyTab.Value[KB_AV] = 1;
					pKeyInfo->remoteStudyTab.Port[KB_AV] = (BYTE)Port;
					pKeyInfo->remoteStudyTab.AD[KB_AV] = (UINT)AD;
				}
			}		
		} 
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_SEEK_INCbStudy",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
	{
		if (dwTemp)
		{
			Value = KB_AV;

			dwTemp = 0;
			dwLen = 4;
			if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_SEEK_INCPort",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
			{
				Port = dwTemp;

				dwTemp = 0;
				dwLen = 4;
				if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_SEEK_INCAD",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
				{
					AD = dwTemp;

					pKeyInfo->remoteStudyTab.Value[KB_SEEK_INC] = 1;
					pKeyInfo->remoteStudyTab.Port[KB_SEEK_INC] = (BYTE)Port;
					pKeyInfo->remoteStudyTab.AD[KB_SEEK_INC] = (UINT)AD;
				}
			}		
		} 
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_SEEK_DECbStudy",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
	{
		if (dwTemp)
		{
			Value = KB_AV;

			dwTemp = 0;
			dwLen = 4;
			if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_SEEK_DECPort",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
			{
				Port = dwTemp;

				dwTemp = 0;
				dwLen = 4;
				if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_SEEK_DECAD",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
				{
					AD = dwTemp;

					pKeyInfo->remoteStudyTab.Value[KB_SEEK_DEC] = 1;
					pKeyInfo->remoteStudyTab.Port[KB_SEEK_DEC] = (BYTE)Port;
					pKeyInfo->remoteStudyTab.AD[KB_SEEK_DEC] = (UINT)AD;
				}
			}		
		} 
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_MUTEbStudy",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
	{
		if (dwTemp)
		{
			Value = KB_AV;

			dwTemp = 0;
			dwLen = 4;
			if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_MUTEPort",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
			{
				Port = dwTemp;

				dwTemp = 0;
				dwLen = 4;
				if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_MUTEAD",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
				{
					AD = dwTemp;

					pKeyInfo->remoteStudyTab.Value[KB_MUTE] = 1;
					pKeyInfo->remoteStudyTab.Port[KB_MUTE] = (BYTE)Port;
					pKeyInfo->remoteStudyTab.AD[KB_MUTE] = (UINT)AD;
				}
			}		
		} 
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_VOL_INCbStudy",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
	{
		if (dwTemp)
		{
			Value = KB_AV;

			dwTemp = 0;
			dwLen = 4;
			if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_VOL_INCPort",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
			{
				Port = dwTemp;

				dwTemp = 0;
				dwLen = 4;
				if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_VOL_INCAD",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
				{
					AD = dwTemp;

					pKeyInfo->remoteStudyTab.Value[KB_VOL_INC] = 1;
					pKeyInfo->remoteStudyTab.Port[KB_VOL_INC] = (BYTE)Port;
					pKeyInfo->remoteStudyTab.AD[KB_VOL_INC] = (UINT)AD;
				}
			}		
		} 
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_VOL_DECbStudy",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
	{
		if (dwTemp)
		{
			Value = KB_AV;

			dwTemp = 0;
			dwLen = 4;
			if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_VOL_DECPort",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
			{
				Port = dwTemp;

				dwTemp = 0;
				dwLen = 4;
				if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_VOL_DECAD",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
				{
					AD = dwTemp;

					pKeyInfo->remoteStudyTab.Value[KB_VOL_DEC] = 1;
					pKeyInfo->remoteStudyTab.Port[KB_VOL_DEC] = (BYTE)Port;
					pKeyInfo->remoteStudyTab.AD[KB_VOL_DEC] = (UINT)AD;
				}
			}		
		} 
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_CALL_REJECTbStudy",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
	{
		if (dwTemp)
		{
			Value = KB_AV;

			dwTemp = 0;
			dwLen = 4;
			if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_CALL_REJECTPort",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
			{
				Port = dwTemp;

				dwTemp = 0;
				dwLen = 4;
				if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_CALL_REJECTAD",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
				{
					AD = dwTemp;

					pKeyInfo->remoteStudyTab.Value[KB_CALL_REJECT] = 1;
					pKeyInfo->remoteStudyTab.Port[KB_CALL_REJECT] = (BYTE)Port;
					pKeyInfo->remoteStudyTab.AD[KB_CALL_REJECT] = (UINT)AD;
				}
			}		
		} 
	}

	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_CALL_INOUTbStudy",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
	{
		if (dwTemp)
		{
			Value = KB_AV;

			dwTemp = 0;
			dwLen = 4;
			if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_CALL_INOUTPort",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
			{
				Port = dwTemp;

				dwTemp = 0;
				dwLen = 4;
				if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_CALL_INOUTAD",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
				{
					AD = dwTemp;

					pKeyInfo->remoteStudyTab.Value[KB_CALL_INOUT] = 1;
					pKeyInfo->remoteStudyTab.Port[KB_CALL_INOUT] = (BYTE)Port;
					pKeyInfo->remoteStudyTab.AD[KB_CALL_INOUT] = (UINT)AD;
				}
			}		
		} 
	}
	dwTemp = 0;
	dwLen = 4;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_NAVIbStudy",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
	{
		if (dwTemp)
		{
			Value = KB_AV;

			dwTemp = 0;
			dwLen = 4;
			if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_NAVIPort",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
			{
				Port = dwTemp;

				dwTemp = 0;
				dwLen = 4;
				if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"dwKB_NAVIAD",0,&dwType,(unsigned char *)&dwTemp,&dwLen))
				{
					AD = dwTemp;

					pKeyInfo->remoteStudyTab.Value[KB_NAVI] = 1;
					pKeyInfo->remoteStudyTab.Port[KB_NAVI] = (BYTE)Port;
					pKeyInfo->remoteStudyTab.AD[KB_NAVI] = (UINT)AD;
				}
			}		
		} 
	}
	RegCloseKey(hKey);	
}

void SteelwheelStudyRegDataWrite(P_FLY_KEY_INFO pKeyInfo)
{
	HKEY hKey; 
	DWORD dwDisposition;
	DWORD dwTemp;
	DWORD dwLen;

#if REGEDIT_NOT_SAVE
	return;
#endif

	//读取  方向盘学习   参数
	RegCreateKeyEx(HKEY_CURRENT_USER,L"FlyDrivers\\Key\\SteelWheelStudy",0,NULL,0,
		0,NULL,&hKey,&dwDisposition); /////////////////////////////////创建或打开注册表

	if (pKeyInfo->remoteStudyTab.Value[KB_AV])
	{
		dwLen = 4;
		dwTemp = 1;
		RegSetValueEx(hKey,L"dwKB_AVbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.Port[KB_AV];
		RegSetValueEx(hKey,L"dwKB_AVPort",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.AD[KB_AV];
		RegSetValueEx(hKey,L"dwKB_AVAD",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}
	else
	{
		dwLen = 4;
		dwTemp = 0;
		RegSetValueEx(hKey,L"dwKB_AVbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}

	if (pKeyInfo->remoteStudyTab.Value[KB_SEEK_INC])
	{
		dwLen = 4;
		dwTemp = 1;
		RegSetValueEx(hKey,L"dwKB_SEEK_INCbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.Port[KB_SEEK_INC];
		RegSetValueEx(hKey,L"dwKB_SEEK_INCPort",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.AD[KB_SEEK_INC];
		RegSetValueEx(hKey,L"dwKB_SEEK_INCAD",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}
	else
	{
		dwLen = 4;
		dwTemp = 0;
		RegSetValueEx(hKey,L"dwKB_SEEK_INCbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}

	if (pKeyInfo->remoteStudyTab.Value[KB_SEEK_DEC])
	{
		dwLen = 4;
		dwTemp = 1;
		RegSetValueEx(hKey,L"dwKB_SEEK_DECbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.Port[KB_SEEK_DEC];
		RegSetValueEx(hKey,L"dwKB_SEEK_DECPort",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.AD[KB_SEEK_DEC];
		RegSetValueEx(hKey,L"dwKB_SEEK_DECAD",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}
	else
	{
		dwLen = 4;
		dwTemp = 0;
		RegSetValueEx(hKey,L"dwKB_SEEK_DECbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}

	if (pKeyInfo->remoteStudyTab.Value[KB_MUTE])
	{
		dwLen = 4;
		dwTemp = 1;
		RegSetValueEx(hKey,L"dwKB_MUTEbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.Port[KB_MUTE];
		RegSetValueEx(hKey,L"dwKB_MUTEPort",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.AD[KB_MUTE];
		RegSetValueEx(hKey,L"dwKB_MUTEAD",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}
	else
	{
		dwLen = 4;
		dwTemp = 0;
		RegSetValueEx(hKey,L"dwKB_MUTEbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}

	if (pKeyInfo->remoteStudyTab.Value[KB_VOL_INC])
	{
		dwLen = 4;
		dwTemp = 1;
		RegSetValueEx(hKey,L"dwKB_VOL_INCbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.Port[KB_VOL_INC];
		RegSetValueEx(hKey,L"dwKB_VOL_INCPort",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.AD[KB_VOL_INC];
		RegSetValueEx(hKey,L"dwKB_VOL_INCAD",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}
	else
	{
		dwLen = 4;
		dwTemp = 0;
		RegSetValueEx(hKey,L"dwKB_VOL_INCbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}

	if (pKeyInfo->remoteStudyTab.Value[KB_VOL_DEC])
	{
		dwLen = 4;
		dwTemp = 1;
		RegSetValueEx(hKey,L"dwKB_VOL_DECbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.Port[KB_VOL_DEC];
		RegSetValueEx(hKey,L"dwKB_VOL_DECPort",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.AD[KB_VOL_DEC];
		RegSetValueEx(hKey,L"dwKB_VOL_DECAD",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}
	else
	{
		dwLen = 4;
		dwTemp = 0;
		RegSetValueEx(hKey,L"dwKB_VOL_DECbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}

	if (pKeyInfo->remoteStudyTab.Value[KB_CALL_REJECT])
	{
		dwLen = 4;
		dwTemp = 1;
		RegSetValueEx(hKey,L"dwKB_CALL_REJECTbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.Port[KB_CALL_REJECT];
		RegSetValueEx(hKey,L"dwKB_CALL_REJECTPort",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.AD[KB_CALL_REJECT];
		RegSetValueEx(hKey,L"dwKB_CALL_REJECTAD",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}
	else
	{
		dwLen = 4;
		dwTemp = 0;
		RegSetValueEx(hKey,L"dwKB_CALL_REJECTbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}

	if (pKeyInfo->remoteStudyTab.Value[KB_CALL_INOUT])
	{
		dwLen = 4;
		dwTemp = 1;
		RegSetValueEx(hKey,L"dwKB_CALL_INOUTbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.Port[KB_CALL_INOUT];
		RegSetValueEx(hKey,L"dwKB_CALL_INOUTPort",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.AD[KB_CALL_INOUT];
		RegSetValueEx(hKey,L"dwKB_CALL_INOUTAD",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}
	else
	{
		dwLen = 4;
		dwTemp = 0;
		RegSetValueEx(hKey,L"dwKB_CALL_INOUTbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}

	if (pKeyInfo->remoteStudyTab.Value[KB_NAVI])
	{
		dwLen = 4;
		dwTemp = 1;
		RegSetValueEx(hKey,L"dwKB_NAVIbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.Port[KB_NAVI];
		RegSetValueEx(hKey,L"dwKB_NAVIPort",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
		dwLen = 4;
		dwTemp = pKeyInfo->remoteStudyTab.AD[KB_NAVI];
		RegSetValueEx(hKey,L"dwKB_NAVIAD",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}
	else
	{
		dwLen = 4;
		dwTemp = 0;
		RegSetValueEx(hKey,L"dwKB_NAVIbStudy",0,REG_DWORD,(unsigned char *)&dwTemp,dwLen);
	}
	
	RegCloseKey(hKey);	
}

static UINT querryDifferenceSize(UINT a,UINT b)
{
	if (a >= b)
	{
		return a-b;
	}
	else
	{
		return b-a;
	}
}
static void SteelwheelStudyProc(P_FLY_KEY_INFO pKeyInfo)
{
	UINT iTempReadAD;

	if (0 == pKeyInfo->remoteStudyStep)//记录未按下的AD值
	{
		if (0 == pKeyInfo->remoteStudyCount)
		{
			pKeyInfo->remoteStudyCurrent1 = 1023;
			pKeyInfo->remoteStudyCurrent2 = 1023;
		}
		else if (pKeyInfo->remoteStudyCount > 50)//抖动太厉害，失败
		{
			pKeyInfo->remoteStudyStart = FALSE;
			IpcStartEvent(EVENT_GLOBAL_REMOTE_STUDY_PRE_RETURN_WAIT_ID);
			pKeyInfo->remoteStudyStart = FALSE;
			pKeyInfo->remoteStudyStep = 0;
			pKeyInfo->remoteStudyCount = 0;
			IpcStartEvent(EVENT_GLOBAL_REMOTE_USE_IT_ID);
			DBGD((TEXT("\r\nFlyAudio Key Steel Study Fail For Dance")));
			return;
		}
		else
		{
			HwADCGetValue(STEEL_ADC_CHANNEL1,&iTempReadAD);
			if (querryDifferenceSize(iTempReadAD,pKeyInfo->remoteStudyCurrent1) < STEEL_ADC_STUDY_DANCE)//稳定1
			{
				pKeyInfo->remoteStudyCurrent1 = iTempReadAD;
				HwADCGetValue(STEEL_ADC_CHANNEL2,&iTempReadAD);
				if (querryDifferenceSize(iTempReadAD,pKeyInfo->remoteStudyCurrent2) < STEEL_ADC_STUDY_DANCE)//稳定2
				{
					pKeyInfo->remoteStudyCurrent2 = iTempReadAD;

					pKeyInfo->remoteStudyNormalAD1 = pKeyInfo->remoteStudyCurrent1;
					pKeyInfo->remoteStudyNormalAD2 = pKeyInfo->remoteStudyCurrent2;
					pKeyInfo->remoteStudyStep = 1;//跳转，学习
					pKeyInfo->remoteStudyCount = 0;
					IpcStartEvent(EVENT_GLOBAL_REMOTE_STUDY_RETURN_START_ID);
					DBGD((TEXT("\r\nFlyAudio Key Steel Study Start Read %d %d"),pKeyInfo->remoteStudyCurrent1,pKeyInfo->remoteStudyCurrent2));
				}
				else
				{
					pKeyInfo->remoteStudyCurrent2 = iTempReadAD;
				}
			}
			else
			{
				pKeyInfo->remoteStudyCurrent1 = iTempReadAD;
			}
		}
		pKeyInfo->remoteStudyCount++;
	}
	else if (1 == pKeyInfo->remoteStudyStep)//学习按键
	{
		if (0 == pKeyInfo->remoteStudyCount)
		{
			pKeyInfo->remoteStudyCurrent1 = 1023;
			pKeyInfo->remoteStudyCurrent2 = 1023;
		}
		else
		{
			HwADCGetValue(STEEL_ADC_CHANNEL1,&iTempReadAD);
			if (querryDifferenceSize(iTempReadAD,pKeyInfo->remoteStudyCurrent1) < STEEL_ADC_STUDY_DANCE)//稳定1
			{
				pKeyInfo->remoteStudyCurrent1 = iTempReadAD;
				if (querryDifferenceSize(pKeyInfo->remoteStudyNormalAD1,pKeyInfo->remoteStudyCurrent1) > STEEL_ADC_STUDY_DISTANCE)//有差距，成功
				{
					pKeyInfo->remoteStudyTab.Value[pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyID] = 1;
					pKeyInfo->remoteStudyTab.Port[pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyID] = 1;
					pKeyInfo->remoteStudyTab.AD[pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyID] = pKeyInfo->remoteStudyCurrent1;
					pKeyInfo->remoteStudyStep = 2;
					pKeyInfo->remoteStudyCount = 0;
					IpcStartEvent(EVENT_GLOBAL_REMOTE_STUDY_PRE_RETURN_FINISH_ID);
					DBGD((TEXT("\r\nFlyAudio Key Steel Study OK1 %d"),pKeyInfo->remoteStudyCurrent1));
					return;
				}
			}
			else
			{
				pKeyInfo->remoteStudyCurrent1 = iTempReadAD;
			}

			HwADCGetValue(STEEL_ADC_CHANNEL2,&iTempReadAD);
			if (querryDifferenceSize(iTempReadAD,pKeyInfo->remoteStudyCurrent2) < STEEL_ADC_STUDY_DANCE)//稳定2
			{
				pKeyInfo->remoteStudyCurrent2 = iTempReadAD;
				if (querryDifferenceSize(pKeyInfo->remoteStudyNormalAD2,pKeyInfo->remoteStudyCurrent2) > STEEL_ADC_STUDY_DISTANCE)//有差距，成功
				{
					pKeyInfo->remoteStudyTab.Value[pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyID] = 1;
					pKeyInfo->remoteStudyTab.Port[pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyID] = 2;
					pKeyInfo->remoteStudyTab.AD[pKeyInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iRemoteStudyID] = pKeyInfo->remoteStudyCurrent2;
					pKeyInfo->remoteStudyStep = 2;
					pKeyInfo->remoteStudyCount = 0;
					IpcStartEvent(EVENT_GLOBAL_REMOTE_STUDY_PRE_RETURN_FINISH_ID);
					DBGD((TEXT("\r\nFlyAudio Key Steel Study OK2 %d"),pKeyInfo->remoteStudyCurrent2));
					return;
				}
			}
			else
			{
				pKeyInfo->remoteStudyCurrent2 = iTempReadAD;
			}
		}
		pKeyInfo->remoteStudyCount++;
	}
	else if (2 == pKeyInfo->remoteStudyStep)
	{
		if (pKeyInfo->remoteStudyCount > 5)//长时间不释放，没必要多长时间
		{
			pKeyInfo->remoteStudyStart = FALSE;
			IpcStartEvent(EVENT_GLOBAL_REMOTE_STUDY_PRE_RETURN_WAIT_ID);
			pKeyInfo->remoteStudyStart = FALSE;
			pKeyInfo->remoteStudyStep = 0;
			pKeyInfo->remoteStudyCount = 0;
			IpcStartEvent(EVENT_GLOBAL_REMOTE_USE_IT_ID);
			DBGD((TEXT("\r\nFlyAudio Key Steel Study Finish With Wait TimeOut")));
		}
		else
		{
			HwADCGetValue(STEEL_ADC_CHANNEL1,&iTempReadAD);
			if (querryDifferenceSize(iTempReadAD,pKeyInfo->remoteStudyNormalAD1) < STEEL_ADC_STUDY_DANCE)//稳定1
			{
				HwADCGetValue(STEEL_ADC_CHANNEL2,&iTempReadAD);
				if (querryDifferenceSize(iTempReadAD,pKeyInfo->remoteStudyNormalAD2) < STEEL_ADC_STUDY_DANCE)//稳定2
				{
					pKeyInfo->remoteStudyStart = FALSE;
					IpcStartEvent(EVENT_GLOBAL_REMOTE_STUDY_PRE_RETURN_WAIT_ID);
					pKeyInfo->remoteStudyStart = FALSE;
					pKeyInfo->remoteStudyStep = 0;
					pKeyInfo->remoteStudyCount = 0;
					IpcStartEvent(EVENT_GLOBAL_REMOTE_USE_IT_ID);
					DBGD((TEXT("\r\nFlyAudio Key Steel Study Finish With Buttom Up")));
				}
			}
		}
		pKeyInfo->remoteStudyCount++;
	}
}

static UINT steelWheelChangeStudyReturnIndexInTabByID(P_FLY_KEY_INFO pKeyInfo,BYTE iID)
{
	UINT i;
	for (i = 0;i < sizeof(RemoteKEY_TabS);i++)
	{
		if (iID == RemoteKEY_TabS[i])
		{
			return i;
		}
	}
	return 0;
}

static void steelwheelChangeStudyToNormal(P_FLY_KEY_INFO pKeyInfo)
{
	UINT i,j;
	UINT iWrite = 0;
	UINT iValue;
	UINT iPort;
	UINT iAD;
	UINT iChangeDance;
	UINT iTemp;

	memset(&pKeyInfo->remoteTab,0,sizeof(IRKEY_TAB));

	for (i = 0;i < IRKEYTABSIZE;i++)
	{
		if (pKeyInfo->remoteStudyTab.Value[i])
		{
			iValue = i;
			iPort = pKeyInfo->remoteStudyTab.Port[i];
			iAD = pKeyInfo->remoteStudyTab.AD[i];
			iChangeDance = STEEL_ADC_CHANGE_DANCE;
			for (j = 0;j < IRKEYTABSIZE;j++)
			{
				if (j == i)
				{
					continue;
				}
				else if (0 == pKeyInfo->remoteStudyTab.Value[j])
				{
					continue;
				}
				else if (iPort != pKeyInfo->remoteStudyTab.Port[j])
				{
					continue;
				}
				else
				{
					iTemp = querryDifferenceSize(iAD,pKeyInfo->remoteStudyTab.AD[j])/2;
					if (iChangeDance > iTemp)
					{
						iChangeDance = iTemp;
					}
				}
			}
			pKeyInfo->remoteTab.IRKEY_Value[iWrite] = steelWheelChangeStudyReturnIndexInTabByID(pKeyInfo,iValue);
			pKeyInfo->remoteTab.IRKEY_Port[iWrite] = iPort;
			pKeyInfo->remoteTab.IRKEY_AD_Min[iWrite] = (iAD>iChangeDance)?(iAD - iChangeDance):0; 
			pKeyInfo->remoteTab.IRKEY_AD_Max[iWrite] = iAD+iChangeDance;
			iWrite++;
		}
	}
	pKeyInfo->remoteTab.size = iWrite;
	pKeyInfo->remoteTab.CarTypeID[0] = 0x33;
	pKeyInfo->remoteTab.CarTypeID[1] = 0x44;

	SteelwheelKeyADCSInit(pKeyInfo);

	DBGD((TEXT("\r\nFlyAudio Key Steel Use Study")));
	DBGD((TEXT("\r\nID %x %x"),pKeyInfo->remoteTab.CarTypeID[0],pKeyInfo->remoteTab.CarTypeID[1]));
	for (i = 0;i < pKeyInfo->remoteTab.size;i++)
	{
		DBGD((TEXT("\r\nsize:%d Index %x ID %x Port %d Min %d Max %d")
			,pKeyInfo->remoteTab.size
			,pKeyInfo->remoteTab.IRKEY_Value[i]
			,RemoteKEY_TabS[pKeyInfo->remoteTab.IRKEY_Value[i]]
			,pKeyInfo->remoteTab.IRKEY_Port[i]
			,pKeyInfo->remoteTab.IRKEY_AD_Min[i]
			,pKeyInfo->remoteTab.IRKEY_AD_Max[i]));
	}
}