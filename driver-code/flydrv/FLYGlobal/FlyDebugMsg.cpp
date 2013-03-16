// FLY7730.cpp : Defines the entry point for the DLL application.
//


#include "FLYGlobal.h"
#include "FILECLASS.h"

static void debugParaWriteToConfig(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	//UINT i;
	//RETAILMSG(1, (TEXT("\r\nParaName:")));
	//for (i = 0;i < pGlobalInfo->debugiParaLength;i++)
	//{
	//	RETAILMSG(1, (TEXT("%c"),pGlobalInfo->debugParaName[i]));
	//}
	//RETAILMSG(1, (TEXT(" ParaValue:%d"),pGlobalInfo->debugParaValue));

	if (0 == memcmp(pGlobalInfo->debugParaName,"FlyGlobalMsgLevel",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.debugMsgLevel = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyKeyDebugMsgLevel",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.debugMsgLevel = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyAudioMsgLevel",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAudioInfo.debugMsgLevel = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyBTMsgLevel",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalBTInfo.debugMsgLevel = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyDVDMsgLevel",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDVDInfo.debugMsgLevel = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyVideoMsgLevel",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalVideoInfo.debugMsgLevel = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlySystemMsgLevel",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.debugMsgLevel = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyRadioMsgLevel",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.debugMsgLevel = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyCarbodyMsgLevel",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.debugMsgLevel = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyXMRadioMsgLevel",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalXMRadioInfo.debugMsgLevel = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyExBoxMsgLevel",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalExBoxInfo.debugMsgLevel = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyTPMSMsgLevel",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTPMSInfo.debugMsgLevel = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyTVMsgLevel",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTVInfo.debugMsgLevel = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyAssistDisplayMsgLevel",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAssistDisplayInfo.debugMsgLevel = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyAcMsgLevel",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAcInfo.debugMsgLevel = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyFAIIIMsgLevel",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalFaiiiInfo.debugMsgLevel = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyOSDDemoMode",pGlobalInfo->debugiParaLength))
	{
		if (pGlobalInfo->debugParaValue)
		{
			pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bOSDDemoMode = TRUE;
		}
		else
		{
			pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bOSDDemoMode = FALSE;
		}
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyIngoreIPCEventMsgResponse",pGlobalInfo->debugiParaLength))
	{
		if (pGlobalInfo->debugParaValue)
		{
			pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bIngoreIPCEventMsgResponse = TRUE;
		}
		else
		{
			pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bIngoreIPCEventMsgResponse = FALSE;
		}
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"FlyACCOffLightOn",pGlobalInfo->debugiParaLength))
	{
		if (pGlobalInfo->debugParaValue)
		{
			pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bACCOffLightOn = TRUE;
		}
		else
		{
			pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bACCOffLightOn = FALSE;
		}
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"IICSpeadOnAudio",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.IICSpeadOnAudio = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"SYSTEM_RESET_USE_EXT_CONFIG",pGlobalInfo->debugiParaLength))
	{
		if (pGlobalInfo->debugParaValue)
		{
			pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSystemResetUseExtConfig = TRUE;
		}
		else
		{
			pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSystemResetUseExtConfig = FALSE;
		}
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"SYSTEM_RESET_AT_LEAST_DAYS",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSystemResetAtLeastDays = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"SYSTEM_RESET_ON_HOUR",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSystemResetOnHour = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"SYSTEM_RESET_ON_MINUTE",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSystemResetOnMinute = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"SYSTEM_RESET_INNER_MIN",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSystemResetInnerMin = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"SYSTEM_RESET_POWEROFF_MIN",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSystemResetPowerOffMin = pGlobalInfo->debugParaValue;
	}
	else if (0 == memcmp(pGlobalInfo->debugParaName,"SYSTEM_CAN_RUN_AT_LEAST",pGlobalInfo->debugiParaLength))
	{
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSystemResetCanRunLess = pGlobalInfo->debugParaValue;
	}
	else
	{
		RETAILMSG(1, (TEXT(" None Compare!")));
	}
}

static void debugParaProcessor(P_FLY_GLOBAL_INFO pGlobalInfo,BYTE iByte)
{
	if ('\r' == iByte || '\n' == iByte)
	{
		pGlobalInfo->debugParaName[pGlobalInfo->debugiParaLength] = 0x00;
		if (FALSE == pGlobalInfo->debugbReadParaName)
		{
			debugParaWriteToConfig(pGlobalInfo);
		}
		pGlobalInfo->debugbReadParaName = TRUE;
		pGlobalInfo->debugiParaLength = 0;		
		pGlobalInfo->debugParaValue = 0;
		pGlobalInfo->debugReadParaEnd = FALSE;
	}
	else if ('/' == iByte)
	{
		pGlobalInfo->debugReadParaEnd = TRUE;
	}
	else
	{
		if (!pGlobalInfo->debugReadParaEnd)
		{
			if ('=' == iByte)
			{
				pGlobalInfo->debugbReadParaName = FALSE;
			}
			else if (FALSE == pGlobalInfo->debugbReadParaName)
			{
				if (iByte >= '0' && iByte <= '9')
				{
					pGlobalInfo->debugParaValue *= 10;
					pGlobalInfo->debugParaValue += iByte - '0';
				}
			}
			else if ((iByte >= 'a' && iByte <= 'z')
				|| (iByte >= '0' && iByte <= '9')
				|| (iByte >= 'A' && iByte <= 'Z')
				|| (iByte == '_')
				)
			{
				if (pGlobalInfo->debugiParaLength < DEBUG_BUFF_LINE_LENGTH - 1)
				{
					pGlobalInfo->debugParaName[pGlobalInfo->debugiParaLength++] = iByte;
				}
			}
		}
	}
}

void debugParaReadFromFile(P_FLY_GLOBAL_INFO pGlobalInfo,BYTE iPath)//0ResidentFlash	1SDMEM
{
	FILE *file;
	UINT i;
	UINT iLength;
	BYTE *pBuf;

	return;

	if (GetFileAttributes(L"\\SDMEM\\FlyDriversDebug.txt") == -1)
	{
		RETAILMSG(1, (TEXT("\r\nGlobal ResidentFlash have not SD K or have not FlyDriversDebug.txt \r\n")));
		return;
	}


	if (0 == iPath)
	{
		if (NULL == (file = fopen("\\ResidentFlash\\FlyDriversDebug.txt","rb")))
		{
			RETAILMSG(1, (TEXT("\r\nGlobal ResidentFlash FlyDriversDebug.txt is not exist")));
			return;
		}
	}
	else if (1 == iPath)
	{
		if (NULL == (file = fopen("\\SDMEM\\FlyDriversDebug.txt","rb")))
		{
			RETAILMSG(1, (TEXT("\r\nGlobal SDMEM FlyDriversDebug.txt is not exist")));
			return;
		}
		pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bSuccessReadDebugFileFromSDMEM = TRUE;
	}

	RETAILMSG(1, (TEXT("\r\nGlobal FlyDriversDebug.txt open success")));
	pGlobalInfo->debugbReadParaName = TRUE;
	pGlobalInfo->debugiParaLength = 0;
	pGlobalInfo->debugParaValue = 0;

	fseek(file,0,SEEK_END);
	iLength = ftell(file);
	fseek(file,0,SEEK_SET);
	pBuf = new BYTE[iLength];
	fread(pBuf,1,iLength,file);
	fclose(file);

	for (i = 0;i < iLength;i++)
	{
		debugParaProcessor(pGlobalInfo,pBuf[i]);
	}
	delete []pBuf;

}

void demoStrStart(P_FLY_GLOBAL_INFO pGlobalInfo,UINT iRow)
{
	if (iRow >= OSD_DEBUG_LINES)
	{
		iRow = 0;
	}
	pGlobalInfo->iOSDDemoStrRow = iRow;
	pGlobalInfo->iOSDDemoStrLength = 0;
}

void demoStrAdd(P_FLY_GLOBAL_INFO pGlobalInfo,BYTE iAscii)
{
	if (pGlobalInfo->iOSDDemoStrLength < OSD_DEBUG_WIDTH)
	{
		pGlobalInfo->sDemoStr[pGlobalInfo->iOSDDemoStrRow][pGlobalInfo->iOSDDemoStrLength] = iAscii;
		pGlobalInfo->iOSDDemoStrLength++;
		pGlobalInfo->iDemoStrLength[pGlobalInfo->iOSDDemoStrRow] = pGlobalInfo->iOSDDemoStrLength;
	}
}

void demoOSDHostTemperature(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	INT iTemp = pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iHostTemperature;

	demoStrStart(pGlobalInfo,OSD_DEBUG_HOST_TEMPERATURE);
	demoStrAdd(pGlobalInfo,'H');
	demoStrAdd(pGlobalInfo,'o');
	demoStrAdd(pGlobalInfo,'s');
	demoStrAdd(pGlobalInfo,'t');
	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,'T');
	demoStrAdd(pGlobalInfo,'e');
	demoStrAdd(pGlobalInfo,'m');
	demoStrAdd(pGlobalInfo,'p');
	demoStrAdd(pGlobalInfo,'e');
	demoStrAdd(pGlobalInfo,'r');
	demoStrAdd(pGlobalInfo,'a');
	demoStrAdd(pGlobalInfo,'t');
	demoStrAdd(pGlobalInfo,'u');
	demoStrAdd(pGlobalInfo,'r');
	demoStrAdd(pGlobalInfo,'e');
	demoStrAdd(pGlobalInfo,':');
	if (iTemp < 0)
	{
		demoStrAdd(pGlobalInfo,'-');
		iTemp = 0 - iTemp;
	}
	demoStrAdd(pGlobalInfo,iTemp/100 + '0');
	demoStrAdd(pGlobalInfo,iTemp%100/10 + '0');
	demoStrAdd(pGlobalInfo,iTemp%10 + '0');
}

void demoOSDDVDSoftVersion(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	UINT i;

	demoStrStart(pGlobalInfo,OSD_DEBUG_SOFTVERSION);
	demoStrAdd(pGlobalInfo,'V');
	demoStrAdd(pGlobalInfo,'e');
	demoStrAdd(pGlobalInfo,'r');
	demoStrAdd(pGlobalInfo,'s');
	demoStrAdd(pGlobalInfo,'i');
	demoStrAdd(pGlobalInfo,'o');
	demoStrAdd(pGlobalInfo,'n');

	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersionLength)
	{
		demoStrAdd(pGlobalInfo,' ');
		demoStrAdd(pGlobalInfo,'D');
		demoStrAdd(pGlobalInfo,'V');
		demoStrAdd(pGlobalInfo,'D');
		demoStrAdd(pGlobalInfo,':');

		for (i = 0;i < pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersionLength;i++)
		{
			demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iDVDSoftwareVersion[i]);
		}
	}

	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTSoftwareVersionLength)
	{
		demoStrAdd(pGlobalInfo,' ');
		demoStrAdd(pGlobalInfo,'B');
		demoStrAdd(pGlobalInfo,'T');
		demoStrAdd(pGlobalInfo,':');
		for (UINT i = 0;i < pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTSoftwareVersionLength;i++)
		{
			demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iBTSoftwareVersion[i]);
		}
	}
}

void demoOSDDriversCompileTime(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	demoStrStart(pGlobalInfo,OSD_DEBUG_COMPILE_TIME_ERROR);
	demoStrAdd(pGlobalInfo,'D');
	demoStrAdd(pGlobalInfo,'r');
	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bCheckShellBabyError)
	{
		demoStrAdd(pGlobalInfo,'1');
	}
	else
	{
		demoStrAdd(pGlobalInfo,'i');
	}
	demoStrAdd(pGlobalInfo,'v');
	demoStrAdd(pGlobalInfo,'e');
	demoStrAdd(pGlobalInfo,'r');
	demoStrAdd(pGlobalInfo,'s');
	demoStrAdd(pGlobalInfo,':');
	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompYear/1000 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompYear%1000/100 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompYear%100/10 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompYear%10 + '0');
	demoStrAdd(pGlobalInfo,'.');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompMon/10 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompMon%10 + '0');
	demoStrAdd(pGlobalInfo,'.');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompDay/10 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompDay%10 + '0');
	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompHour/10 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompHour%10 + '0');
	demoStrAdd(pGlobalInfo,':');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompMin/10 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompMin%10 + '0');
	demoStrAdd(pGlobalInfo,':');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompSec/10 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.iDriverCompSec%10 + '0');

	//准备进入待机，断掉与LPC的IIC通信
	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bNoMoreToSendDataWhenToSleep)
	{
		demoStrAdd(pGlobalInfo,'T');
		demoStrAdd(pGlobalInfo,'o');
		demoStrAdd(pGlobalInfo,' ');
		demoStrAdd(pGlobalInfo,'S');
		demoStrAdd(pGlobalInfo,'l');
		demoStrAdd(pGlobalInfo,'e');
		demoStrAdd(pGlobalInfo,'e');
		demoStrAdd(pGlobalInfo,'p');
		demoStrAdd(pGlobalInfo,' ');
	}
}

void demoOSDKeyADDisplay(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	demoStrStart(pGlobalInfo,OSD_DEBUG_KEYAD_LIST);
	demoStrAdd(pGlobalInfo,'A');
	demoStrAdd(pGlobalInfo,'D');
	demoStrAdd(pGlobalInfo,' ');

	demoStrAdd(pGlobalInfo,'1');
	demoStrAdd(pGlobalInfo,':');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelKeyAD[0] /100 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelKeyAD[0] %100 /10 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelKeyAD[0] %10 + '0');
	demoStrAdd(pGlobalInfo,' ');

	demoStrAdd(pGlobalInfo,'2');
	demoStrAdd(pGlobalInfo,':');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelKeyAD[1] /100 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelKeyAD[1] %100 /10 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelKeyAD[1] %10 + '0');
	demoStrAdd(pGlobalInfo,' ');

	demoStrAdd(pGlobalInfo,'3');
	demoStrAdd(pGlobalInfo,':');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelKeyAD[2] /100 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelKeyAD[2] %100 /10 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iPanelKeyAD[2] %10 + '0');
	demoStrAdd(pGlobalInfo,' ');

	demoStrAdd(pGlobalInfo,'4');
	demoStrAdd(pGlobalInfo,':');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSteelAD[0] /1000 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSteelAD[0] %1000 /100 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSteelAD[0] %100 /10 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSteelAD[0] %10 + '0');
	demoStrAdd(pGlobalInfo,' ');

	demoStrAdd(pGlobalInfo,'5');
	demoStrAdd(pGlobalInfo,':');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSteelAD[1] /1000 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSteelAD[1] %1000 /100 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSteelAD[1] %100 /10 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iSteelAD[1] %10 + '0');

	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,'L');
	demoStrAdd(pGlobalInfo,'i');
	demoStrAdd(pGlobalInfo,'s');
	demoStrAdd(pGlobalInfo,'t');
	demoStrAdd(pGlobalInfo,':');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iKeyIndex/100 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iKeyIndex%100/10 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iKeyIndex%10 + '0');
	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,'V');
	demoStrAdd(pGlobalInfo,'a');
	demoStrAdd(pGlobalInfo,'l');
	demoStrAdd(pGlobalInfo,'u');
	demoStrAdd(pGlobalInfo,'e');
	demoStrAdd(pGlobalInfo,':');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iKeyValue/100 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iKeyValue%100/10 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iKeyValue%10 + '0');
	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,'C');
	demoStrAdd(pGlobalInfo,'o');
	demoStrAdd(pGlobalInfo,'u');
	demoStrAdd(pGlobalInfo,'n');
	demoStrAdd(pGlobalInfo,'t');
	demoStrAdd(pGlobalInfo,':');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iKeyCount/1000 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iKeyCount%1000/100 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iKeyCount%100/10 + '0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iKeyCount%10 + '0');
}

void demoOSDBreakAndPhoneStatus(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	demoStrStart(pGlobalInfo,OSD_DEBUG_BREAK_AND_PHONE_STATUS);

	demoStrAdd(pGlobalInfo,'F');
	demoStrAdd(pGlobalInfo,'A');
	demoStrAdd(pGlobalInfo,'M');
	demoStrAdd(pGlobalInfo,'P');
	demoStrAdd(pGlobalInfo,':');
	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveFlyAudioExtAMP)
	{
		demoStrAdd(pGlobalInfo,'1');
	}
	else
	{
		demoStrAdd(pGlobalInfo,'0');
	}

	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,'T');
	demoStrAdd(pGlobalInfo,'V');
	demoStrAdd(pGlobalInfo,':');
	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTVModule)
	{
		demoStrAdd(pGlobalInfo,'1');
	}
	else
	{
		demoStrAdd(pGlobalInfo,'0');
	}

	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,'T');
	demoStrAdd(pGlobalInfo,'P');
	demoStrAdd(pGlobalInfo,'M');
	demoStrAdd(pGlobalInfo,'S');
	demoStrAdd(pGlobalInfo,':');
	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bHaveTPMSModule)
	{
		demoStrAdd(pGlobalInfo,'1');
	}
	else
	{
		demoStrAdd(pGlobalInfo,'0');
	}

	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,'B');
	demoStrAdd(pGlobalInfo,'r');
	demoStrAdd(pGlobalInfo,'e');
	demoStrAdd(pGlobalInfo,'a');
	demoStrAdd(pGlobalInfo,'k');

	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,'B');
	demoStrAdd(pGlobalInfo,'U');
	demoStrAdd(pGlobalInfo,'S');
	demoStrAdd(pGlobalInfo,':');
	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBreakStatusBUS)
	{
		demoStrAdd(pGlobalInfo,'1');
	}
	else
	{
		demoStrAdd(pGlobalInfo,'0');
	}

	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,'I');
	demoStrAdd(pGlobalInfo,'O');
	demoStrAdd(pGlobalInfo,':');
	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bBreakStatusIO)
	{
		demoStrAdd(pGlobalInfo,'1');
	}
	else
	{
		demoStrAdd(pGlobalInfo,'0');
	}

	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,'P');
	demoStrAdd(pGlobalInfo,'h');
	demoStrAdd(pGlobalInfo,'o');
	demoStrAdd(pGlobalInfo,'n');
	demoStrAdd(pGlobalInfo,'e');

	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,'B');
	demoStrAdd(pGlobalInfo,'U');
	demoStrAdd(pGlobalInfo,'S');
	demoStrAdd(pGlobalInfo,':');
	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bExtPhoneStatusBUS)
	{
		demoStrAdd(pGlobalInfo,'1');
	}
	else
	{
		demoStrAdd(pGlobalInfo,'0');
	}

	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,'I');
	demoStrAdd(pGlobalInfo,'O');
	demoStrAdd(pGlobalInfo,':');
	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bExtPhoneStatusIO)
	{
		demoStrAdd(pGlobalInfo,'1');
	}
	else
	{
		demoStrAdd(pGlobalInfo,'0');
	}
}

void demoOSDStateError(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	//BOOL bFirstDelaySend = TRUE;
	//BOOL bFirstIICSend = TRUE;

	//demoStrStart(pGlobalInfo,OSD_DEBUG_STATE_ERROR);

	////各个驱动响应超时
	//if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bIngoreIPCEventMsgResponse
	//	&& pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bOSDDemoMode)
	//{
	//////	UINT iEventWhat;
	//////	ULONG iNowTime = GetTickCount();

	//////	for (iEventWhat = 0;iEventWhat < EVENT_GLOBAL_DATA_CHANGE_MAX;iEventWhat++)
	//////	{
	//////		if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalDataChangeWhatStartTime[iEventWhat])
	//////		{
	//////			if (iNowTime - pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalDataChangeWhatStartTime[iEventWhat] >= 20000)
	//////			{
	//////				if (bFirstDelaySend)
	//////				{
	//////					bFirstDelaySend = FALSE;
	//////					demoStrAdd(pGlobalInfo,'D');
	//////					demoStrAdd(pGlobalInfo,'r');
	//////					demoStrAdd(pGlobalInfo,'i');
	//////					demoStrAdd(pGlobalInfo,'v');
	//////					demoStrAdd(pGlobalInfo,'e');
	//////					demoStrAdd(pGlobalInfo,'r');
	//////					demoStrAdd(pGlobalInfo,' ');
	//////					demoStrAdd(pGlobalInfo,'i');
	//////					demoStrAdd(pGlobalInfo,'s');
	//////					demoStrAdd(pGlobalInfo,' ');
	//////					demoStrAdd(pGlobalInfo,'b');
	//////					demoStrAdd(pGlobalInfo,'u');
	//////					demoStrAdd(pGlobalInfo,'s');
	//////					demoStrAdd(pGlobalInfo,'y');
	//////					demoStrAdd(pGlobalInfo,'2');
	//////					demoStrAdd(pGlobalInfo,'0');
	//////					demoStrAdd(pGlobalInfo,'S');
	//////					demoStrAdd(pGlobalInfo,'?');
	//////				}
	//////				demoStrAdd(pGlobalInfo,' ');
	//////				demoStrAdd(pGlobalInfo,iEventWhat/100 + '0');
	//////				demoStrAdd(pGlobalInfo,iEventWhat%100/10 + '0');
	//////				demoStrAdd(pGlobalInfo,iEventWhat%10 + '0');
	/////*				if(pGlobalInfo->hDispatchMainThreadEvent
	////					== pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalDataChangeWhatHandle[iEventWhat])
	////				{
	////					demoStrAdd(pGlobalInfo,'F');
	////					demoStrAdd(pGlobalInfo,'G');
	////					demoStrAdd(pGlobalInfo,'B');
	////				}
	////				else if(pGlobalInfo->hHandleGlobalDVDEvent
	////					== pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalDataChangeWhatHandle[iEventWhat])
	////				{
	////					demoStrAdd(pGlobalInfo,'F');
	////					demoStrAdd(pGlobalInfo,'C');
	////					demoStrAdd(pGlobalInfo,'D');
	////				}
	////				else if(pGlobalInfo->hHandleGlobalAudioEvent
	////					== pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalDataChangeWhatHandle[iEventWhat])
	////				{
	////					demoStrAdd(pGlobalInfo,'F');
	////					demoStrAdd(pGlobalInfo,'A');
	////					demoStrAdd(pGlobalInfo,'U');
	////				}
	////				else if(pGlobalInfo->hHandleGlobalRadioEvent
	////					== pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalDataChangeWhatHandle[iEventWhat])
	////				{
	////					demoStrAdd(pGlobalInfo,'F');
	////					demoStrAdd(pGlobalInfo,'R');
	////					demoStrAdd(pGlobalInfo,'A');
	////				}
	////				else if(pGlobalInfo->hHandleGlobalVideoEvent
	////					== pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalDataChangeWhatHandle[iEventWhat])
	////				{
	////					demoStrAdd(pGlobalInfo,'F');
	////					demoStrAdd(pGlobalInfo,'V');
	////					demoStrAdd(pGlobalInfo,'O');
	////				}
	////				else if(pGlobalInfo->hHandleGlobalSystemEvent
	////					== pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalDataChangeWhatHandle[iEventWhat])
	////				{
	////					demoStrAdd(pGlobalInfo,'F');
	////					demoStrAdd(pGlobalInfo,'S');
	////					demoStrAdd(pGlobalInfo,'Y');
	////				}
	////				else if(pGlobalInfo->hHandleGlobalKeyEvent
	////					== pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalDataChangeWhatHandle[iEventWhat])
	////				{
	////					demoStrAdd(pGlobalInfo,'F');
	////					demoStrAdd(pGlobalInfo,'K');
	////					demoStrAdd(pGlobalInfo,'Y');
	////				}
	////				else if(pGlobalInfo->hHandleGlobalBTEvent
	////					== pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalDataChangeWhatHandle[iEventWhat])
	////				{
	////					demoStrAdd(pGlobalInfo,'F');
	////					demoStrAdd(pGlobalInfo,'B');
	////					demoStrAdd(pGlobalInfo,'T');
	////				}
	////				else if(pGlobalInfo->hHandleGlobalExBoxEvent
	////					== pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iGlobalDataChangeWhatHandle[iEventWhat])
	////				{
	////					demoStrAdd(pGlobalInfo,'E');
	////					demoStrAdd(pGlobalInfo,'x');
	////					demoStrAdd(pGlobalInfo,'B');
	////					demoStrAdd(pGlobalInfo,'o');
	////					demoStrAdd(pGlobalInfo,'x');
	////				}
	////				else
	////				{
	////					demoStrAdd(pGlobalInfo,'N');
	////					demoStrAdd(pGlobalInfo,'U');
	////					demoStrAdd(pGlobalInfo,'L');
	////					demoStrAdd(pGlobalInfo,'L');
	////				}*/
	////			}
	////		}
	////	}
	//}

	////驱动之间的全局结构体不能匹配
	//if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[0]
	//|| pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[1]
	//|| pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[2])
	//{
	//	demoStrAdd(pGlobalInfo,' ');
	//	demoStrAdd(pGlobalInfo,'E');
	//	demoStrAdd(pGlobalInfo,'r');
	//	demoStrAdd(pGlobalInfo,'r');
	//	demoStrAdd(pGlobalInfo,'o');
	//	demoStrAdd(pGlobalInfo,'r');
	//	demoStrAdd(pGlobalInfo,':');
	//	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[0]);
	//	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[1]);
	//	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.sErrorDriverName[2]);
	//}

	//if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnAudio >= 5
	//	|| (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bOSDDemoMode
	//		&& pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnAudio))
	//{
	//	if (bFirstIICSend)
	//	{
	//		bFirstIICSend = FALSE;
	//		demoStrAdd(pGlobalInfo,'I');demoStrAdd(pGlobalInfo,'I');demoStrAdd(pGlobalInfo,'C');
	//		demoStrAdd(pGlobalInfo,' ');
	//		demoStrAdd(pGlobalInfo,'E');demoStrAdd(pGlobalInfo,'r');demoStrAdd(pGlobalInfo,'r');demoStrAdd(pGlobalInfo,'o');demoStrAdd(pGlobalInfo,'r');
	//		demoStrAdd(pGlobalInfo,':');
	//	}

	//	demoStrAdd(pGlobalInfo,'A');demoStrAdd(pGlobalInfo,'u');demoStrAdd(pGlobalInfo,'d');demoStrAdd(pGlobalInfo,'i');demoStrAdd(pGlobalInfo,'o');demoStrAdd(pGlobalInfo,':');
	//	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnAudio >= 1000)
	//	{
	//		demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnAudio / 1000 + '0');
	//	}
	//	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnAudio >= 100)
	//	{
	//		demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnAudio % 1000 / 100 + '0');
	//	}
	//	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnAudio >= 10)
	//	{
	//		demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnAudio % 100 / 10 + '0');
	//	}
	//	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnAudio % 10 + '0');
	//}

	//if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnRadio >= 5
	//	|| (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.bOSDDemoMode
	//		&& pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnRadio))
	//{
	//	if (bFirstIICSend)
	//	{
	//		bFirstIICSend = FALSE;
	//		demoStrAdd(pGlobalInfo,'I');demoStrAdd(pGlobalInfo,'I');demoStrAdd(pGlobalInfo,'C');
	//		demoStrAdd(pGlobalInfo,' ');
	//		demoStrAdd(pGlobalInfo,'E');demoStrAdd(pGlobalInfo,'r');demoStrAdd(pGlobalInfo,'r');demoStrAdd(pGlobalInfo,'o');demoStrAdd(pGlobalInfo,'r');
	//		demoStrAdd(pGlobalInfo,':');
	//	}

	//	demoStrAdd(pGlobalInfo,'R');demoStrAdd(pGlobalInfo,'a');demoStrAdd(pGlobalInfo,'d');demoStrAdd(pGlobalInfo,'i');demoStrAdd(pGlobalInfo,'o');demoStrAdd(pGlobalInfo,':');
	//	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnRadio >= 1000)
	//	{
	//		demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnRadio / 1000 + '0');
	//	}
	//	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnRadio >= 100)
	//	{
	//		demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnRadio % 1000 / 100 + '0');
	//	}
	//	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnRadio >= 10)
	//	{
	//		demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnRadio % 100 / 10 + '0');
	//	}
	//	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnRadio % 10 + '0');
	//}

	//if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnMCU)
	//{
	//	if (bFirstIICSend)
	//	{
	//		bFirstIICSend = FALSE;
	//		demoStrAdd(pGlobalInfo,'I');demoStrAdd(pGlobalInfo,'I');demoStrAdd(pGlobalInfo,'C');
	//		demoStrAdd(pGlobalInfo,' ');
	//		demoStrAdd(pGlobalInfo,'E');demoStrAdd(pGlobalInfo,'r');demoStrAdd(pGlobalInfo,'r');demoStrAdd(pGlobalInfo,'o');demoStrAdd(pGlobalInfo,'r');
	//		demoStrAdd(pGlobalInfo,':');
	//	}

	//	demoStrAdd(pGlobalInfo,'M');demoStrAdd(pGlobalInfo,'C');demoStrAdd(pGlobalInfo,'U');demoStrAdd(pGlobalInfo,':');
	//	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnMCU >= 1000)
	//	{
	//		demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnMCU / 1000 + '0');
	//	}
	//	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnMCU >= 100)
	//	{
	//		demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnMCU % 1000 / 100 + '0');
	//	}
	//	if (pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnMCU >= 10)
	//	{
	//		demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnMCU % 100 / 10 + '0');
	//	}
	//	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.iIICErrorOnMCU % 10 + '0');
	//}
}

void demoOSDOpenStatus(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	BOOL bHaveSendInit = FALSE;
	BOOL bHaveSendOpen = FALSE;

	demoStrStart(pGlobalInfo,OSD_DEBUG_INIT_OPEN_STATUS);

	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.bInit)
	{
		bHaveSendInit = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'G');demoStrAdd(pGlobalInfo,'B');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.bInit)
	{
		bHaveSendInit = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'C');demoStrAdd(pGlobalInfo,'B');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.bInit)
	{
		bHaveSendInit = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'S');demoStrAdd(pGlobalInfo,'Y');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.bInit)
	{
		bHaveSendInit = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'K');demoStrAdd(pGlobalInfo,'Y');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalBTInfo.bInit)
	{
		bHaveSendInit = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'B');demoStrAdd(pGlobalInfo,'T');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDVDInfo.bInit)
	{
		bHaveSendInit = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'C');demoStrAdd(pGlobalInfo,'D');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAudioInfo.bInit)
	{
		bHaveSendInit = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'A');demoStrAdd(pGlobalInfo,'U');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalVideoInfo.bInit)
	{
		bHaveSendInit = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'V');demoStrAdd(pGlobalInfo,'O');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.bInit)
	{
		bHaveSendInit = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'R');demoStrAdd(pGlobalInfo,'A');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAssistDisplayInfo.bInit)
	{
		bHaveSendInit = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'A');demoStrAdd(pGlobalInfo,'D');demoStrAdd(pGlobalInfo,' ');
	}
	//if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalXMRadioInfo.bInit)
	//{
	//	bHaveSendInit = TRUE;
	//	demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'S');demoStrAdd(pGlobalInfo,'R');demoStrAdd(pGlobalInfo,' ');
	//}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTPMSInfo.bInit)
	{
		bHaveSendInit = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'T');demoStrAdd(pGlobalInfo,'P');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTVInfo.bInit)
	{
		bHaveSendInit = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'T');demoStrAdd(pGlobalInfo,'V');demoStrAdd(pGlobalInfo,' ');
	}

	if (bHaveSendInit)
	{
		demoStrAdd(pGlobalInfo,'N');demoStrAdd(pGlobalInfo,'o');demoStrAdd(pGlobalInfo,'t');
		demoStrAdd(pGlobalInfo,' ');demoStrAdd(pGlobalInfo,'I');demoStrAdd(pGlobalInfo,'n');demoStrAdd(pGlobalInfo,'i');demoStrAdd(pGlobalInfo,'t');
	}

	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDriver.bOpen)
	{
		bHaveSendOpen = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'G');demoStrAdd(pGlobalInfo,'B');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalCarbodyInfo.bOpen)
	{
		bHaveSendOpen = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'C');demoStrAdd(pGlobalInfo,'B');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalSystemInfo.bOpen)
	{
		bHaveSendOpen = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'S');demoStrAdd(pGlobalInfo,'Y');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalKeyInfo.bOpen)
	{
		bHaveSendOpen = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'K');demoStrAdd(pGlobalInfo,'Y');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalBTInfo.bOpen)
	{
		bHaveSendOpen = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'B');demoStrAdd(pGlobalInfo,'T');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalDVDInfo.bOpen)
	{
		bHaveSendOpen = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'C');demoStrAdd(pGlobalInfo,'D');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAudioInfo.bOpen)
	{
		bHaveSendOpen = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'A');demoStrAdd(pGlobalInfo,'U');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalVideoInfo.bOpen)
	{
		bHaveSendOpen = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'V');demoStrAdd(pGlobalInfo,'O');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalRadioInfo.bOpen)
	{
		bHaveSendOpen = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'R');demoStrAdd(pGlobalInfo,'A');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalAssistDisplayInfo.bOpen)
	{
		bHaveSendOpen = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'A');demoStrAdd(pGlobalInfo,'D');demoStrAdd(pGlobalInfo,' ');
	}
	//if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalXMRadioInfo.bOpen)
	//{
	//	bHaveSendOpen = TRUE;
	//	demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'S');demoStrAdd(pGlobalInfo,'R');demoStrAdd(pGlobalInfo,' ');
	//}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTPMSInfo.bOpen)
	{
		bHaveSendOpen = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'T');demoStrAdd(pGlobalInfo,'P');demoStrAdd(pGlobalInfo,' ');
	}
	if (!pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlyGlobalTVInfo.bOpen)
	{
		bHaveSendOpen = TRUE;
		demoStrAdd(pGlobalInfo,'F');demoStrAdd(pGlobalInfo,'T');demoStrAdd(pGlobalInfo,'V');demoStrAdd(pGlobalInfo,' ');
	}

	if (bHaveSendOpen)
	{
		demoStrAdd(pGlobalInfo,'N');demoStrAdd(pGlobalInfo,'o');demoStrAdd(pGlobalInfo,'t');
		demoStrAdd(pGlobalInfo,' ');demoStrAdd(pGlobalInfo,'O');demoStrAdd(pGlobalInfo,'p');demoStrAdd(pGlobalInfo,'e');demoStrAdd(pGlobalInfo,'n');
	}
}

void demoOSDOtherInfo(P_FLY_GLOBAL_INFO pGlobalInfo)
{
	demoStrStart(pGlobalInfo,OSD_DEBUG_OTHER_INFO);

	demoStrAdd(pGlobalInfo,'H');
	demoStrAdd(pGlobalInfo,'o');
	demoStrAdd(pGlobalInfo,'s');
	demoStrAdd(pGlobalInfo,'t');
	demoStrAdd(pGlobalInfo,':');
#if SHICHAN_SHICHAN
	demoStrAdd(pGlobalInfo,'1');
#else
	demoStrAdd(pGlobalInfo,'0');
#endif

	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,'N');
	demoStrAdd(pGlobalInfo,'a');
	demoStrAdd(pGlobalInfo,'v');
	demoStrAdd(pGlobalInfo,'i');
	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,'M');
	demoStrAdd(pGlobalInfo,'i');
	demoStrAdd(pGlobalInfo,'x');
	demoStrAdd(pGlobalInfo,':');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.GPSSpeaker+'0');

	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,'A');
	demoStrAdd(pGlobalInfo,'u');
	demoStrAdd(pGlobalInfo,'d');
	demoStrAdd(pGlobalInfo,'i');
	demoStrAdd(pGlobalInfo,'o');
	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,'I');
	demoStrAdd(pGlobalInfo,'n');
	demoStrAdd(pGlobalInfo,':');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eAudioInput/10+'0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eAudioInput%10+'0');
	demoStrAdd(pGlobalInfo,' ');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eCurAudioInput/10+'0');
	demoStrAdd(pGlobalInfo,pGlobalInfo->pFlyShmGlobalInfo->sFlyDriverGlobalInfo.FlySystemRunningInfo.eCurAudioInput%10+'0');
}