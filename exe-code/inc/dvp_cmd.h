/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008 MediaTek Inc.
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

/*******************************************************************************
*
* Filename:
* ---------
* file dvp_cmd.h
*
* Project:
* --------
*   CNB
*
* Description:
* ------------
*
*
* Author:
* -------
*
*
*------------------------------------------------------------------------------
* $Revision: #54 $
* $Modtime:$
* $Log:$
*
*******************************************************************************/

#ifndef _DVP_CMD_H_
#define _DVP_CMD_H_

/*******************************************************************************
					AP cmd to 8032
*******************************************************************************/

//system function, 0x00---0x1f
#define AP_CMD_CONNECT           				0x00
#define AP_CMD_CHG_DEVICE     		        	0x01	// Disc
	#define DEVICE_DISC					0x00
	#define DEVICE_AIN					0x01
	#define DEVICE_USB					0x02
	#define DEVICE_MCR					0x03
	#define DEVICE_IDLE					0xff
#define AP_CMD_EJECT     						0x02	// Eject
#define AP_CMD_DVP_SET_RESET     				0x03	// DVP setup reset(init all parameters)
#define AP_CMD_DVP_SET     						0x04	// DVP setup
	#define AP_CMD_SET_TV_DISPLAY		0x00	//TV Display (SV_4_3_PS ~ SV_WIDESQZ)
	#define AP_CMD_SET_CAPTIONS			0x01	//Captions (EV_ON ~ EV_OFF)
	#define AP_CMD_SET_SCREEN_SAVER		0x02	//Screen Saver (EV_ON ~ EV_OFF)
	#define AP_CMD_SET_LAST_MEM			0x03	//Last memory (EV_ON ~ EV_OFF)
	#define AP_CMD_SET_TV_TYPE			0x04	//TV Type (SV_PAL ~ SV_NTSC)
	#define AP_CMD_SET_PBC				0x05	//PBC (EV_ON ~ EV_OFF)
	#define AP_CMD_SET_AUDIO_LAN		0x06	//Audio language (EV_ENGLISH ~ EV_LANG_OFF)
	#define AP_CMD_SET_SUB_LAN			0x07	//Subtitle language (EV_ENGLISH ~ EV_LANG_OFF)
	#define AP_CMD_SET_MENU_LAN			0x08	//Disc Menu language (EV_ENGLISH ~ EV_LANG_OFF)
	#define AP_CMD_SET_PARENTAL			0x09	//Parental (SV_PR_KID ~ SV_PR_OFF)
	#define AP_CMD_SET_PWD_MODE			0x0a	//Password Mode (EV_ON ~ EV_OFF)
	//audio setup
	#define AP_CMD_SET_DOWNMIX			0x0b	//Downmix (EV_DOWN_LTRT ~ EV_DOWN_OFF)
	#define AP_CMD_SET_F_SPEAKER		0x0c	//Front speaker (EV_LARGE ~ EV_SMALL)
	#define AP_CMD_SET_C_SPEAKER		0x0d	//Center speaker (EV_LARGE ~ EV_SPK_OFF)
	#define AP_CMD_SET_R_SPEAKER		0x0e	//Rear speaker (EV_LARGE ~ EV_SPK_OFF)
	#define AP_CMD_SET_SUBWOOFER		0x0f	//Subwoofer (EV_ON ~ EV_OFF)
	#define AP_CMD_SET_TEST_TONE		0x10	//Test Tone (EV_ON ~ EV_OFF)
	#define AP_CMD_SET_DIALOG			0x11	//Dialog (00 ~ 20)
	#define AP_CMD_SET_SPDIF_OUTPUT		0x12	//SPDIF Output (EV_AO_OFF ~ EV_MANUAL)
	#define AP_CMD_SET_LPCM_OUT			0x13	//LPCM Out (EV_48K ~ EV_96K)
	#define AP_CMD_SET_CDELAY_C			0x14	//Channel Delay-->Center (0*10 ~ MAX_CENTER_DELAY_LEN*10)
	#define AP_CMD_SET_CDELAY_SUB		0x15	//Channel Delay-->Subwoofer (0*10 ~ MAX_SUB_DELAY_LEN*10)
	#define AP_CMD_SET_CDELAY_LS		0x16	//Channel Delay-->LS (0*10 ~ MAX_LS_DELAY_LEN*10)
	#define AP_CMD_SET_CDELAY_RS		0x17	//Channel Delay-->RS (0*10 ~ MAX_RS_DELAY_LEN*10)
	#define AP_CMD_SET_EQ_TYPE			0x18	//EQ Type (EV_NONE ~ EV_SOFT)
	#define AP_CMD_SET_PROLOG_2			0x19	//PRO Logic II (EV_PROLOGIC_ON ~ EV_PROLOGIC_AUTO)
	#define AP_CMD_SET_REVERB_MODE		0x1a	//Reverb Mode (EV_NONE ~ EV_REVERB_MAX)
	#define AP_CMD_SET_DUAL_MONO		0x1b	//Dual Mono (EV_STEREO ~ EV_MIX)
	#define AP_CMD_SET_DYNAMIC			0x1c	//Dynamic (0 ~ 8)
	#define AP_CMD_SET_VOL				0x1d    //Volume: [0~100]
	#define AP_CMD_SET_BALANCE			0x1e	//Balance: [0~40]
	#define AP_CMD_SET_BASS				0x1f    //Bass Band: 0, 1  Bass Scope: [0~28]
	#define AP_CMD_SET_TREBLE			0x20    //Treble Band: 9, 10  Treble Scope: [0~40] 
	#define AP_CMD_SET_R_VOL			0x21    //Rear Volume:[0~100]
#define AP_CMD_GET_PREPARE_STATE     			0x05	// Get prepare state
#define AP_CMD_GET_INIT_STATE     				0x06	// Get init state
#define AP_CMD_GET_DISC_TYPE     				0x07	// Get disc type
#define AP_CMD_GET_TOTAL_COUNT     				0x08	// Get total count (CD/VCD/DVD)
#define AP_CMD_GET_CUR_INDEX     				0x09	// Get current index (CD/VCD/DVD)
#define AP_CMD_GET_PLAY_STATE     				0x0A	// Get play state
#define AP_CMD_GET_REPEAT_MODE     				0x0B	// Get repeat mode
#define AP_CMD_GET_SHUFFLE_MODE     			0x0C	// Get shuffle mode
#define AP_CMD_GET_PBC     						0x0D	// Get PBC mode
#define AP_CMD_GET_AUDIO     					0x0E	// Get audio&channel
#define AP_CMD_GET_SUBTITLE     				0x0F	// Get subtitle
#define AP_CMD_GET_ANGLE     					0x10	// Get angle


//file list, 0x20---0x3f
#define AP_CMD_CREATE_LIST     					0x20	// filter(all/audio/video/pic)
	#define FILTER_ALL					0x00
	#define FILTER_ADO					0x01
	#define FILTER_VDO					0x02
	#define FILTER_PIC					0x03
#define AP_CMD_FL_GET_ITEM       				0x21	// get item by given index
#define AP_CMD_FL_CLICK_ITEM     				0x22	// click item by given index
#define AP_CMD_FL_UP_LEVEL     					0x23	// up level

//playback control, 0x40---0x5f
#define AP_CMD_PBC_PLAY     					0x40	// play
#define AP_CMD_PBC_PAUSE     					0x41	// pause
#define AP_CMD_PBC_STOP     					0x42	// stop
#define AP_CMD_PBC_FF     						0x43	// fast forward
#define AP_CMD_PBC_FB     						0x44	// fast backward
#define AP_CMD_PBC_PREV     					0x45	// prev
#define AP_CMD_PBC_NEXT     					0x46	// next
#define AP_CMD_PBC_SEEK    						0x47	// goto by time
	#define SEEK_TIME					0x00	
#define AP_CMD_PBC_PSW_OK						0x48    //Check pwd result
    #define CHK_PTL_PWD_FAIL  0x00
	#define CHK_PTL_PWD_OK    0x01

//function control, 0x60---0x7f
#define AP_CMD_FUNC_REPEAT     					0x60	// repeat
#define AP_CMD_FUNC_PBC_CTRL     				0x61	// shuffle/random
	#define PBC_CTRL_SHUFFLE	0x00
	#define PBC_CTRL_RANDOM		0x01
#define AP_CMD_FUNC_AUDIO     					0x62	// audio
#define AP_CMD_FUNC_ANGLE     					0x63	// angle
#define AP_CMD_FUNC_SUBTITLE     				0x64	// subtitle
#define AP_CMD_FUNC_TITLE     					0x65  // title
#define AP_CMD_FUNC_MENU     					0x66	// menu
#define AP_CMD_FUNC_PBC     					0x67	// vcd pbc
#define AP_CMD_FUNC_ROTATE    					0x68	// jpg rotate
    #define RIGHT_ROTATE    0x00
		#define LEFT_ROTATE     0x01
		#define UP_ROTATE       0x02
		#define DOWN_ROTATE     0x03
#define AP_CMD_FUNC_DIGEST    					0x69  // jpg digest
#define AP_CMD_FUNC_ZOOMIN    					0x6a  // zoom in
#define AP_CMD_FUNC_ZOOMOUT    					0x6b  // zoom out
#define AP_CMD_FUNC_UP    					0x6c  // up
#define AP_CMD_FUNC_DOWN    					0x6d  // down
#define AP_CMD_FUNC_LEFT    					0x6e  // left
#define AP_CMD_FUNC_RIGHT    					0x6f  // right


#define AP_CMD_FUNC_DIGITAL_NUM    				0x70  // digital keypad number 
     #define TITLE_NUM	0x00
	 #define CHAPTER_NUM	0x01

#define AP_CMD_FUNC_DIGEST_NUMANDSTART_SETTING  0x71  // jpeg digest setting on height,width and total number
#define AP_CMD_FUNC_DIGEST_GAPANDSIZE_SETTING   0x72  // jpeg digest setting on height,width and total number


	 

	 
//other, 0x80---0xfd
#define AP_CMD_TP_COORDINATE     			0x80 	// touch panel x/y coordinate

//CD Ripping
#define AP_CMD_RIP_TRACK						0x81	// ripping current track number
#define AP_CMD_RIP_STOP							0x82	// stop ripping
#define AP_CMD_RIP_WR_OK						0x83 	// ripping data write ok
#define AP_CMD_AUD_SAMPLE_RATE_ACK              0x84 	// audio driver control
#define AP_CMD_SET_RIP_MODE 					0x85    //CD Ripping speed mode choice
	#define RIP_NORMAL_SPEED  		0x00
	#define RIP_HIGH_SPEED          0x01

#define AP_CMD_SET_REGION                       0x8a    //Set dvd region
    #define REGION_ALL              0x00
    #define REGION_USA              0x01
    #define REGION_JPN              0x02
    #define REGION_KOR              0x03
    #define REGION_AUS              0x04
    #define REGION_RUS              0x05
    #define REGION_CHN              0x06
	
//sync header
#define AP_CMD_SYNC_HEADER						0xFE 	// Sync Header

//UP Logs
#ifdef UP2AP_LOG
#define AP_CMD_UP_LOG_CHECK_OUTPUT      0x90   //check if up can write logs to dram
#endif


/*******************************************************************************
						8032 cmd to AP
*******************************************************************************/

//system, 0x00---0x2f
#define DVP_CMD_ACK			  					0x00	// ACK result(0:fail, 1:ok)
	#define ACK_FAIL					0x00
	#define ACK_OK						0x01
#define DVP_CMD_DEVICE_TYPE    					0x01  // USB /MCR /DISC
#define DVP_CMD_INIT_STATE     					0x02  // init state,Loading...
	#define INIT_ST_LOADING_DISC		0x00
	#define INIT_ST_TRAY_OPENING  		0x01
	#define INIT_ST_TRAY_CLOSING  		0x02
	#define INIT_ST_TRAY_ERROR    		0x03
	#define INIT_ST_NO_DISC       		0x04
	#define INIT_ST_UNKNOWN_DISC  		0x05
	#define INIT_ST_DISC_IDENTIFIED 	0x06
	#define INIT_ST_DISC_INITED   		0x07
#define DVP_CMD_TRAY_STATUS    					0x03  // TRAY IN /TRAY OUT
	#define DVP_TRAY_OUT       			0x00
	#define DVP_TRAY_IN        			0x01
	#define DVP_TRAY_MIDDLE   	 		0x02
	#define DVP_TRAY_ERROR     			0x03
#define DVP_CMD_DISC_TYPE      					0x04  // DVD /VCD
	#define DISC_DVD_VIDEO        		0x00
	#define DISC_DVD_AUDIO        		0x01
	#define DISC_MINI_DVD         		0x02
	#define DISC_VCD3_0           		0x03
	#define DISC_VCD2_0           		0x04
	#define DISC_VCD1_1           		0x05
	#define DISC_VCD6_0           		0x06
	#define DISC_VCD6_1           		0x07
	#define DISC_SVCD             		0x08
	#define DISC_CVD              		0x09
	#define DISC_CDDA             		0x0a
	#define DISC_HDCD             		0x0b
	#define DISC_DTS_CS          		0x0c
	#define DISC_CDG              		0x0d
	#define DISC_PCD              		0x0e
	#define DISC_FJCD             		0x0f
	#define DISC_DATA            		0x10
	#define DISC_UPG              		0x11
	#define DISC_SACD             		0x12
	#define DISC_AUDIO_IN        		0x13
	#define DISC_DVD_VR           		0x14
	#define DISC_XVCD             		0x15
	#define DISC_XSVCD            		0x16
	#define DISC_MTK_RM           		0x17
	#define DISC_UNKNOWN         		0x18
#define DVP_CMD_LANGUAGE           				0x05  // Chinese/English 
#define DVP_CMD_DISC_INSERT       				0x06  // disc insert
	#define INSERT_FAIL					0x00
	#define INSERT_OK					0x01
#define DVP_CMD_STARTUP_OK       				0x07  // 8032&RISC startup OK
#define DVP_CMD_STANDBY_OK       			0x08  // 8032&RISC standby OK  for STANDBY_UNTIL_AP_NOTIFY
#define DVP_CMD_PREPARE_STATE     				0x09  // prepare state,TRUE:loading OK, FALSE:Not OK.

//File List, 0x30---0x4f
#define DVP_CMD_FL_TOTAL_COUNT        			0x30  // total file count
#define DVP_CMD_FL_FILE_PATH          			0x31  // :\A\B\C\D
#define DVP_CMD_FL_ITEM             			0x32  // item info(name, type(dir/mp3/jpg...))
#define DVP_CMD_FL_CHG_DIR          			0x33  // enter sub-dir/to parent dir
#define DVP_CMD_FL_CODEC          				0x34  // codec of item name
	#define CODEC_UNICODE				0x00
	#define CODEC_OTHER					0x01
#define DVP_CMD_FL_MENU          				0x35  // auto return to file list
#define DVP_CMD_FL_ACK_RESULT     				0x36	// ACK result
	#define FILTER_RESULT				0x00
	#define CHG_DIR_RESULT 				0x01
	#define AUTO_UPDATE_DIR_REUSLT 		0x02  //grow.li 71267
#define DVP_CMD_FL_LIST_INFO        			0x37  // list info.(list type+current dir index)
#define DVP_CMD_FL_FILE_TYPE_COUNT              0x38  // current dir's different type files,include video,audio,picture


//Playback info, 0x50---0x9f
#define DVP_CMD_PBC_ADO_BITRATE       			0x50  // mp3 bitrate
	#define VBR_TRUE  					0x01
	#define VBR_FALSE 					0x00
#define DVP_CMD_PBC_ADO_ID3_TXT       			0x51  // mp3 id3 text
	#define ID3_TITLE					0x00
	#define ID3_ALBUM					0x01
	#define ID3_ARTIST					0x02
	#define ID3_GENRE					0x03
	#define ID3_YEAR					0x04
	#define ID3_MAX		  				0xff
#define DVP_CMD_PBC_ADO_ID3_PIC       			0x52  // mp3 id3 pic
#define DVP_CMD_PBC_ADO_LYRICS        			0x53  // mp3 lyrics
#define DVP_CMD_PBC_ADO_EQ            			0x54  // mp3 eq data
#define DVP_CMD_PBC_JPG_ROTATE_STATE  			0x55  // jpg Right /left /top /bottom
#define DVP_CMD_PBC_JPG_RESOLUTION    			0x56  // xxx * xxx
#define DVP_CMD_PBC_PLAY_STATE     				0x57  // PAUSE /PLAY/STOP
	#define PBC_ST_STOP       			0x01
	#define PBC_ST_PLAY       			0x02
	#define PBC_ST_FF          			0x03
	#define PBC_ST_SF         			0x04
	#define PBC_ST_FR       			0x05
	#define PBC_ST_SR           		0x06
	#define PBC_ST_PAUSE       			0x07
	#define PBC_ST_STEP         		0x08
	#define PBC_ST_STEP_BACK   			0x09
	#define PBC_ST_FREEZE     			0x0a
	#define PBC_ST_STILL      			0x0b
	#define PBC_ST_SETUP_PAUSE  		0x0c
	#define PBC_ST_STOP_RESUME  		0x0d
	#define PBC_ST_DIGEST9     			0x0e
	#define PBC_ST_DIGEST4     			0x0f
	#define PBC_ST_DIGEST      			0x10
	#define PBC_ST_PTL_ERR     			0x11
	#define PBC_ST_REGION_ERR  			0x12
	#define PBC_ST_DISC_ERR    			0x13
	#define PBC_ST_SET_TMP_PTL  		0x14
	#define PBC_ST_PBC_INIT     		0x15
	#define PBC_ST_WAIT_LASTMEM 		0x16 
	#define PBC_ST_PRE_PLAY     		0x17
#define DVP_CMD_PBC_TRACK_NUM      				0x58  // CD, current track/total track
#define DVP_CMD_PBC_CUR_TIME       				0x59  // HH:MM:SS
#define DVP_CMD_PBC_TOTAL_TIME     				0x5a  // HH:MM:SS
#define DVP_CMD_PBC_PLAYING_IDX       			0x5b  // playing index
#define DVP_CMD_PBC_REPEAT_MODE    				0x5c  // Repeat All/File/Folder
#define DVP_CMD_PBC_USER_MODE    				0x5d  // User PBC control: SV_SHUFFLE/SV_RANDOM
#define DVP_CMD_PBC_TITLE_CHAPTER  				0x5e  // DVD, current title / current chapter
#define DVP_CMD_PBC_TITLE_CHAPTER_NUM 			0x5f  // DVD, title count / chapter count in current title
#define DVP_CMD_PBC_DVDHILI_STATUS  			0x60  // need send touch X/Y to dvp in dvd menu state
	#define DVD_HL_ON          			0x00
	#define DVD_HL_OFF          		0x01
#define DVP_CMD_PBC_AUDIO           			0x61  // DVD Audio
#define DVP_CMD_PBC_SUBTITLE          			0x62  // DVD/MP4 subtitle
#define DVP_CMD_PBC_ANGLE          				0x63  // DVD Angle: NS+NO 
#define DVP_CMD_PBC_PBC         				0x64  // VCD PBC ON/OFF
#define DVP_CMD_PBC_FILE_PATH     				0x65  // playing file path
#define DVP_CMD_PBC_CHANNEL           0x66  // VCD channel, Right /Left, SVCD: audio 1~audi N
#define DVP_CMD_MPS_ERROR           			0x67  // Playback error
	#define SV_E_NO_ERROR		 		0x00
	#define SV_E_A_CODEC_UNSUP	 		0x01 
	#define SV_E_V_CODEC_UNSUP	 		0x02 
	#define SV_E_V_RES_UNSUP			0x03 
	#define SV_E_NO_AUDIO				0x04 
	#define SV_E_NO_VIDEO				0x05 
	#define SV_E_V_FPS_UNSUP			0x06 
	#define SV_E_V_GMC_UNSUP			0x07 
	#define SV_E_PROFILE_UNSUP			0x08 
	#define SV_E_DRM_NOT_ALLOWED  		0x09 
	#define SV_E_UNKNOWN_ERROR  		0x0A
#define DVP_CMD_CHECK_PASSWORD           		0x68  // CHECK PASSWORD
#define DVP_CMD_PBC_FIRST_THUMBNAIL_IDX			0x69  // playing index
#define DVP_CMD_PBC_ZOOM          				0x6a   //zoom 
#define DVP_CMD_UP_LOG_DATA_OUTPUT                 0x70    // 8032 send logs to ap
#define DVP_CMD_SAVE_ITEM_ADDRESS                  0x71    // 8032 send file list dram start address, add by grow.li mtk71267 001
#define DVP_CMD_LMCLICK_ITEM_TYPE               0x72
	#define AUTO_PLAY			0x00
	#define LAST_MEMORY_PLAY	0x01
#define DVP_CMD_PBC_PLAYING_AUDIO_IDX			0x73  // playing audio index


//CD ripping&other, 0xa0---0xfe
#define DVP_CMD_RIP_TRK_LBA_LEN      			0xA0  // CD, track number, track lba length
#define DVP_CMD_RIP_DATA_READY     				0xA1	// ripping data ready, AP can get and write to device.
#define DVP_CMD_RIP_TRK_PROGRESS    			0xA2	// ripping track progress.
#define DVP_CMD_RIP_TRK_OK     					0xA3	// finish ripping current track.
#define DVP_CMD_AUD_SAMPLE_RATE      			0xA4	// audio driver control.
#define DVP_CMD_RIP_ERR     					0xA5	// ripping error state
	#define RIP_NO_ERR					0x00
	#define RIP_DISC_ERR				0x01

//sync header
#define DVP_CMD_SYNC_HEADER_H					0xff 	// Sync Header
#define DVP_CMD_SYNC_HEADER_L					0xfe 	// Sync Header


/*******************************************************************************
						AP cmd to PT110
*******************************************************************************/
#define AP2PT_DVP_MIX2AP_WR_CHANNEL             0x00  //DVD Video + SPU + OSD mix to AP pannel use write channel
#define AP2PT_DVP_MIX2AP_HW_PATH                0x01  //DVD Video + SPU + OSD mix to AP pannel use HW Path
#define AP2PT_AP_MIX2DVP                        0x02  //AP VDO_R/FMT_R + OSD_7 + OSD_8 mix to DVD TV
#define AP2PT_4LAYER_MIX                        0x03  //DVD Video + SPU + OSD + AP UI(OSD_8)mix to DVD TV
#define AP2PT_AP_UI_REPLACE_DVP_OSD             0x04  //DVD Video + SPU + AP UI(OSD_8)mix to DVD TV
#define AP2PT_SWITCH_TVE                        0x05  //switch tve to ap or dvp
#define AP2PT_CVBS_CTRL 					   	0x06  //CVBS On/Off control

#define AP2PT_CHECK_LASTMEMORY                  0x08

/*******************************************************************************
						PT110 cmd to AP
*******************************************************************************/
#define PT2AP_SWITCH_TVE_OK						0x00
	#define TVE_AP						0x00
	#define TVE_DVP						0x01
#define PT2AP_DVD_1ST_PIC_START					0x01 //dvd 1st pic show.
#define PT2AP_RIP_DATA_READY 					0xa1  //Rip Data Ready

#define PT2AP_CHECK_LM_RESULT						0x02


#endif //_DVP_CMD_H_

