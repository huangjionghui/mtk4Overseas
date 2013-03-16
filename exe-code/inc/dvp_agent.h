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
* file DVP_API.h
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
* $Revision: #107 $
* $Modtime:$
* $Log:$
*
*******************************************************************************/

#ifndef _DVP_AGENT_H_
#define _DVP_AGENT_H_

#include <GDef.h>

#include <GRect.h>
#include <GSysServices.h>
#include <ddraw.h>

#define EVT_DVP_DEVICE_TYPE    				    0x01  /**< USB /MCR /DISC*/
#define EVT_DVP_INIT_STATE     				    0x02  /**< init state,Loading...*/
#define EVT_DVP_TRAY_STATUS    				    0x03  /**< TRAY IN /TRAY OUT*/
#define EVT_DVP_DISC_TYPE      				    0x04  /**< DVD /VCD*/
#define EVT_DVP_LANGUAGE           		        0x05 /**< Chinese/English */
#define EVT_DVP_DISC_INSERT       		        0x06  /**< disc insert*/

#define EVT_DVP_STANDBY_OK                      0x08
#define EVT_DVP_PREPARE_STATE                   0x09

//File List, 0x30---0x4f
#define EVT_DVP_FL_TOTAL_COUNT                  0x30  /**< total file count*/
#define EVT_DVP_FL_FILE_PATH                    0x31  /**< :\A\B\C\D*/
#define EVT_DVP_FL_ITEM             	        0x32  /**< item info(name, type(dir/mp3/jpg...))*/
#define EVT_DVP_FL_CHG_DIR          	        0x33  /**< enter sub-dir/to parent dir*/

#define EVT_DVP_FL_MODULE                       0x35 /**< play module update*/
#define EVT_DVP_FL_MENU                         0x36 /**< file menu*/
#define EVT_DVP_FL_ACK_RESULT                   0x37 /**< file menu filter/result*/
#define EVT_DVP_FL_LIST_INFO                    0x38 /**< file menu list info(list type+cur dir index)*/
#define EVT_DVP_FL_FILE_TYPE_COUNT              0x39 /**< different type of file counts*/

//Playback info, 0x50---0xfe
#define EVT_DVP_PBC_ADO_BITRATE                 0x50  /**< mp3 bitrate*/
#define EVT_DVP_PBC_ADO_ID3_TXT                 0x51  /**< mp3 id3 text*/
#define EVT_DVP_PBC_ADO_ID3_PIC                 0x52  /**< mp3 id3 pic*/
#define EVT_DVP_PBC_ADO_LYRICS                  0x53  /**< mp3 lyrics*/
#define EVT_DVP_PBC_ADO_EQ                      0x54  /**< mp3 eq data*/
#define EVT_DVP_PBC_JPG_ROTATE_STATE            0x55  /**< jpg Right /left /top /bottom*/
#define EVT_DVP_PBC_JPG_RESOLUTION              0x56  /**< xxx * xxx*/
#define EVT_DVP_PBC_PLAY_STATE     		        0x57  /**< PAUSE /PLAY/STOP*/
#define EVT_DVP_PBC_TRACK_NUM      		        0x58  /**< CD, current track/total track*/
#define EVT_DVP_PBC_CUR_TIME       		        0x59  /**< HH:MM:SS*/
#define EVT_DVP_PBC_TOTAL_TIME     		        0x5a  /**< HH:MM:SS*/
#define EVT_DVP_PBC_PLAYING_IDX                 0x5b  /**< playing index*/
#define EVT_DVP_PBC_REPEAT_MODE    		        0x5c  /**< Repeat All/File/Folder*/
#define EVT_DVP_PBC_USER_MODE    	        0x5d  /**< Shuffle On/Off*/
#define EVT_DVP_PBC_TITLE_CHAPTER  		        0x5e /**< DVD, current title / current chapter*/
#define EVT_DVP_PBC_TITLE_CHAPTER_NUM           0x5f  /**< DVD, title count / chapter count in current title*/
#define EVT_DVP_PBC_DVDHILI_STATUS  	        0x60  /**< need send touch X/Y to dvp in dvd menu state*/
#define EVT_DVP_PBC_AUDIO           	        0x61  /**< channel, Right /Left*/
#define EVT_DVP_PBC_SUBTITLE                    0x62  /**< DVD/MP4 subtitle*/
#define EVT_DVP_PBC_ANGLE          		        0x63  /**< DVD Angle*/
#define EVT_DVP_PBC_PBC         			    0x64  /**< VCD PBC ON/OFF*/
#define EVT_DVP_CMD_RESPONSE                    0x65  /**< */
#define EVT_DVP_ERROR_OCCURED                   0x66  /**<  */
#define EVT_DVP_RIP_PROGRESS                    0x67  /**< */
#define EVT_DVP_RIP_CUR_TRACK_OK                0x68  /**<*/
#define EVT_DVP_RIP_TRK_LBA_LEN                	0x69
#define EVT_DVP_PBC_CHANNEL                     0x6a
#define EVT_DVP_PBC_FILE_PATH                   0x6b
#define	EVT_DVP_MPS_ERROR						0x6c
#define	EVT_DVP_RIP_ERROR						0x6d
#define	EVT_DVP_CHECK_PASSWORD                  0x6e
#define	EVT_DVP_PBC_FIRST_THUMBNAIL_IDX         0x6f
#define	EVT_DVP_LM_CLICK_ITEM_TYPE   			0x70
#define	EVT_DVP_SAVE_ITEM_ADDRESS               0x71
#define EVT_DVP_PBC_ZOOM						0x72 //zoom 
#define EVT_DVP_PBC_PLAYING_AUDIO_IDX      		0x73 //playing audio index 

//PT110 send to AP Event
#define	EVT_PT2AP_SWITCH_TVE_OK                0xA0						
#define	EVT_PT2AP_1ST_PIC_START				   0xA1
#define	EVT_PT2AP_CHECK_LM_RESULT				   0xA3

//CD Rip Error Event
#define	EVT_CD_RIP_NOT_ENOUGH_SPACE			   0xF2

    #define DVP_CMD_CONNECT              0x00
    #define DVP_CMD_CHG_DEVICE           0x01	/**< Disc */
    #define DVP_CMD_EJECT     		    0x02	/**< Eject*/
    //file list, 0x20---0x3f
    #define DVP_CMD_CREATE_LIST     	    0x20	/**< filter(all/audio/video/pic)*/
    #define DVP_CMD_FL_GET_ITEM          0x21	/**< get item by given index*/
    #define DVP_CMD_FL_CLICK_ITEM        0x22	/**< click item by given index*/
    #define DVP_CMD_FL_UP_LEVEL     	    0x23	/**< up level*/
    //playback control, 0x40---0x5f
    #define DVP_CMD_PBC_PLAY     	    0x40	/**< play*/
    #define DVP_CMD_PBC_PAUSE     	    0x41	/**< pause*/
    #define DVP_CMD_PBC_STOP     	    0x42	/**< stop*/
    #define DVP_CMD_PBC_FF     			0x43	/**< fast forward*/
    #define DVP_CMD_PBC_FB     			0x44	/**< fast backward*/
    #define DVP_CMD_PBC_PREV     		0x45	/**< prev*/
    #define DVP_CMD_PBC_NEXT     		0x46	/**< next*/
    #define DVP_CMD_PBC_SEEK    			0x47	/**< goto by time*/
    //function control, 0x60---0x7f
    #define DVP_CMD_FUNC_REPEAT     		0x60	/**< repeat*/
    #define DVP_CMD_FUNC_SHUFFLE     	0x61	/**< shuffle*/
    #define DVP_CMD_FUNC_AUDIO     		0x62	/**< audio*/
    #define DVP_CMD_FUNC_ANGLE     		0x63/**< angle*/
    #define DVP_CMD_FUNC_SUBTITLE     	0x64	/**< subtitle*/
    #define DVP_CMD_FUNC_TITLE     		0x65  /**< title*/
    #define DVP_CMD_FUNC_MENU     		0x66	/**< menu*/
    #define DVP_CMD_FUNC_PBC     	    0x67	/**< vcd pbc*/
    #define DVP_CMD_FUNC_ROTATE    	    0x68	/**< jpg rotate*/
    #define DVP_CMD_FUNC_DIGEST    	    0x69  /**< jpg digest*/
    //other, 0x80---0xfd
    #define DVP_CMD_TP_COORDINATE        0x80 	/**< touch panel x/y coordinate*/


//CD Ripping
#define AP_CMD_RIP_PATH                             0xF0    /**< set mp3 file path*/

//CD_TEXT Information   add by grow.li mtk71267 2012-2-7 001
#define CDTEXT_TITLE_NAME                 0
#define CDTEXT_TRACK_NAME               1
#define CDTEXT_SINGER_NAME              2

#define AUDIO_STEREO            	    0
#define AUDIO_MONO_LEFT       	        11
#define AUDIO_MONO_RIGHT      	        12
#define AUDIO_MONO_MIXED      	        13


//EVT_DVP_PBC_ZOOM: ZOOM mode 
#define SV_ZOOM_1          0  /**< */
#define SV_ZOOM_11         1  /**< */
#define SV_ZOOM_12         2  /**< */
#define SV_ZOOM_15         3  /**< */
#define SV_ZOOM_2          4  /**< */
#define SV_ZOOM_25         5  /**< */
#define SV_ZOOM_3          6  /**< */
#define SV_ZOOM_35         7  /**< */
#define SV_ZOOM_4          8  /**< */
#define SV_ZOOM_21         9  /**< */
#define SV_ZOOM_31         10  /**< */
#define SV_ZOOM_41         11  /**< */
#define SV_ZOOM_MODE_MAX   12  /**< */

//EVT_DVP_DISC_TYPE: disc type 
#define SV_DVD_VIDEO         0  /**< */
#define SV_DVD_AUDIO         1  /**< */
#define SV_MINI_DVD          2  /**< */ 
#define SV_VCD3_0            3  /**< */
#define SV_VCD2_0            4  /**< */
#define SV_VCD1_1            5  /**< */
#define SV_VCD6_0            6  /**< */
#define SV_VCD6_1            7  /**< */
#define SV_SVCD              8  /**< */
#define SV_CVD               9  /**< */
#define SV_CDDA              10 /**< */
#define SV_HDCD              11 /**< */
#define SV_DTS_CS            12 /**< */
#define SV_CDG               13 /**< */
#define SV_PCD               14 /**< */
#define SV_FJCD              15 /**< */
#define SV_DATA              16 /**< */
#define SV_UPG               17 /**< */
#define SV_SACD              18 /**< */
#define SV_AUDIO_IN          19 /**< */  // set by 8032 only
#define SV_DVD_VR            20 /**< */
#define SV_XVCD              21 /**< */
#define SV_XSVCD             22 /**< */
#define SV_UNKNOWN           23 /**< */


//EVT_DVP_INIT_STATE: INIT_STATE
#define SV_LOADING_DISC     0  /**< */
#define SV_TRAY_OPENING     1  /**< */
#define SV_TRAY_CLOSING     2  /**< */
#define SV_TRAY_ERROR       3  /**< */
#define SV_NO_DISC          4  /**< */
#define SV_UNKNOWN_DISC     5  /**< */
#define SV_DISC_IDENTIFIED  6  /**< */
#define SV_DISC_INITED      7  /**< */
#define SV_EMD_LOADING      8  /**< */
#define SV_NO_USB      9       /**< */

//EVT_DVP_PBC_PLAY_STATE:Play Status
#define SV_STOP              1  /**< */
#define SV_PLAY              2  /**< */
#define SV_FF                3  /**< */
#define SV_SF                4  /**< */
#define SV_FR                5  /**< */
#define SV_SR                6  /**< */
#define SV_PAUSE             7  /**< */
#define SV_STEP              8  /**< */
#define SV_STEP_BACK         9  /**< */
#define SV_FREEZE            10 /**< */
#define SV_STILL             11 /**< */
#define SV_SETUP_PAUSE       12 /**< */
#define SV_STOP_RESUME       13 /**< */
/* note the definition before this can not be changed! */
#define SV_DIGEST9           14 /**< */
#define SV_DIGEST4           15 /**< */
#define SV_DIGEST            16 /**< */
#define SV_PTL_ERR           17 /**< */
#define SV_REGION_ERR        18 /**< */
#define SV_DISC_ERR          19 /**< */
#define SV_SET_TMP_PTL       20 /**< */
#define SV_PBC_INIT          21 /**< */
#define SV_WAIT_LASTMEM      22 /**< *///020731
#define SV_PRE_PLAY          23 /**< */
#define SV_AP_AUTO_STOP      24 /**< *///70932

#define SV_PRE_NEXT          30 /**< */
#define SV_PRE_PREV          31 /**< */

#define   fgIsDiscPlay(bPbcState)     (((bPbcState > SV_STOP) && (bPbcState < SV_STOP_RESUME)) || (bPbcState == SV_PRE_PLAY))

//repeat mode
#define SV_USR_STOP          1  /**< */
#define SV_REPEAT_NONE       2  /**< */
#define SV_USR_PBC_NORMAL    2  /**< */
#define SV_REPEAT_ALL        3  /**< */
#define SV_REPEAT_TRACK      4  /**< */
#define SV_REPEAT_LIST       5  /**< */
#define SV_REPEAT_TITLE      6  /**< */
#define SV_REPEAT_CHAPTER    7  /**< */
#define SV_REPEAT_SET_A      8  /**< */
#define SV_REPEAT_AB         9  /**< */
#define SV_MIC_REP_A         10 /**< */
#define SV_MIC_REP_AB        11 /**< */
#define SV_REPEAT_LAST_SP    12 /**< */
#define SV_REPEAT_INSTANT    13 /**< */
#define SV_REPEAT_ABNONE     14 /**< */
#define SV_RANDOM            15 /**< */
#define SV_SHUFFLE           16 /**< */
#define SV_PROGRAM           17 /**< */
#define SV_USR_DIGEST        18 /**< */
#define SV_SINGLE            19 /**< */
#define SV_SCAN              20 /**< */
#define SV_REPEAT_FOLDER     21 /**< */
#define SV_SHUFFLE_FOLDER    22 /**< */ 
/*      file type */
#define FTYPE_UNKNOWN               0xFF      /**< */
#define FTYPE_AC3                   0x00      /**< */
#define FTYPE_MP3                   0x01      /**< */
#define FTYPE_MP2                   0x02      /**< */
#define FTYPE_MP1                   0x03      /**< */
#define FTYPE_JPG                   0x05      /**< */
#define FTYPE_MLP                   0x06      /**< */
#define FTYPE_WMA                   0x07      /**< */
#define FTYPE_ASF                   0x08      /**< */
#define FTYPE_MPG                   0x09      /**< */
#define FTYPE_DAT                   0x0A      /**< */
#define FTYPE_VOB                   0x0B      /**< */
#define FTYPE_AAC                   0x0C      /**< */
#define FTYPE_DSD                   0x0D      /**< */
#define FTYPE_CDA                   0x0F      /**< */
#define FTYPE_DIR                   0x10      /**< directory */
#define FTYPE_DTS                   0x11      /**< */
#define FTYPE_AVI                   0x12      /**< */
#define FTYPE_TS1                   0x13      /**< for test */
#define FTYPE_MP4                   0x14      /**< */
#define FTYPE_3GP                   0x15      /**< */
#define FTYPE_TS2                   0x16      /**< for test */
#define FTYPE_DST                   0x17      /**< */
#define FTYPE_OGG                   0x18      /**< */
#define FTYPE_TS4                   0x19      /**< */
#define FTYPE_RCV                   0x1A      /**< */
#define FTYPE_VC1                   0x1B      /**< */
#define FTYPE_MKV                   0x1C      /**< */
#define FTYPE_M4A                   0x1D      /**< */
#define FTYPE_OMA                   0x1E      /**< */
#define FTYPE_RA                      0x1F    /**< */
#define FTYPE_PDIR                  0x20      /**< parent directory */

//languae  
#define   MSG_ENGLISH 0x1              /**< */
#define   MSG_FRENCH 0x2               /**< */
#define   MSG_SPANISH 0x3              /**< */
#define   MSG_CHINESE 0x4              /**< */
#define   MSG_JAPANESE 0x5             /**< */
#define   MSG_KOREAN 0x6               /**< */
#define   MSG_GERMAN 0x7               /**< */
#define   MSG_PORTUGUESE 0x8           /**< */
#define   MSG_ITALIAN 0x9              /**< */
#define   MSG_THAI 0xA                 /**< */
#define   MSG_SWEDISH 0xb              /**< */
#define   MSG_DANISH 0xc               /**< */
#define   MSG_NORWEGIAN 0xd            /**< */
#define   MSG_FINNISH 0xe              /**< */
#define   MSG_DUTCH 0xf                /**< */
#define   MSG_ICELANDIC 0x10           /**< */
#define   MSG_HEBREW 0x11              /**< */
#define   MSG_GREEK 0x12               /**< */
#define   MSG_CROATIAN 0x13            /**< */
#define   MSG_TURKISH 0x14             /**< */
#define   MSG_POLISH 0x15              /**< */
#define   MSG_HUNGARIAN 0x16           /**< */    
#define   MSG_CZECH 0x17               /**< */
#define   MSG_RUSSIAN 0x18             /**< */
#define   MSG_INDONESIAN 0x19          /**< */
#define   MSG_OFF  0xFF                /**< */


#define fgIsUsrCtrlMode(bMode) ((bMode >= SV_RANDOM) && (bMode <= SV_USR_DIGEST))
#define fgISUsrSeqPlay(bMode)  ((bMode == SV_REPEAT_NONE) || (bMode == SV_REPEAT_ABNONE))
#define fgIsRepeatMode(bMode)  ((bMode >= SV_REPEAT_ALL) && (bMode <= SV_REPEAT_CHAPTER))
#define fgIsA2BMode(bMode)     ((bMode >= SV_REPEAT_SET_A) || (bMode < SV_REPEAT_ABNONE))


//data disc/usb player mode
#define SV_DDISC_MODULE_NONE            0x0    /**< */
#define SV_DDISC_MODULE_MP3             0x1    /**< */
#define SV_DDISC_MODULE_JPG             0x2    /**< */
#define SV_DDISC_MODULE_MPG             0x3    /**< */

// On/Off
#define SV_ON               0x0       /**< */
#define SV_OFF              0x1       /**< */

#define SV_FP_DOM            1       /**< */
#define SV_VMGM_DOM          2       /**< */
#define SV_VTSM_DOM          3       /**< */
#define SV_VTSTT_DOM         4       /**< */
#define SV_ATSTT_DOM         5       /**< */
#define SV_PLAY_LIST         6       /**< */
#define SV_SELECT_LIST       7       /**< */
#define IsInDvdMenu(bDom)   ((bDom > SV_FP_DOM) && (bDom < SV_VTSTT_DOM))

//cd ripping state information
#define SV_CD_RIP_NONE    (0)          /**< */
#define SV_CD_RIP_FULL    (1)          /**< */
#define SV_CD_RIP_DONE    (2)          /**< */
#define SV_CD_RIP_FAIL    (3)          /**< */
#define SV_CD_RIP_ABORT   (4)          /**< */
#define SV_CD_RIP_EXIST   (5)          /**< */
#define SV_CD_RIP_WRITING (6)          /**< */
#define SV_CD_RIP_DISC_ERROR  (7)        /**< */
#define SV_CD_RIP_ABORT_OK    (8)        /**< */
#define SV_CD_RIP_DEV_WRITE_PROTECT  (9) /**< */
#define SV_CD_RIP_FOLDER_IS_FULL  (10)   /**< */
#define SV_CD_RIP_FILE_IS_FULL  (11)     /**< */
#define SV_CD_RIP_FAT_NOT_SUPPORT (12)   /**< */



/*  device ID for device manager (must be kept as unique and starts from 1) */
/*  for USB card slot */
#define SV_DEVID_USB_SLOT_ALL       0x0f    /**< sum of USB_SLOT_1 to USB_SLOT_6 */
#define SV_DEVID_USB_SLOT_1         0x00    /**< */
#define SV_DEVID_USB_SLOT_2         0x01    /**< */
#define SV_DEVID_USB_SLOT_3         0x02    /**< */
#define SV_DEVID_USB_SLOT_4         0x03    /**< */
#define SV_DEVID_USB_SLOT_5         0x04    /**< */
#define SV_DEVID_USB_SLOT_6         0x05    /**< */
/*  for MS Card */
#define SV_DEVID_MS_CARD            0x06    /**< */
/*  for SD Card */
#define SV_DEVID_SD_CARD            0x07    /**< */
/*  for SM Card */
#define SV_DEVID_SM_CARD            0x08    /**< */
/*  for CF Card */
#define SV_DEVID_CF_CARD            0x09    /**< */
/*  for TVD */
#define SV_DEVID_TVD                0x0A    /**< */
/*  for DVBT */
#define SV_DEVID_DVBT               0x0B    /**< */
/*  for Audio */
#define SV_DEVID_AUDIOIN            0x0C    /**< */
/*  for CD-DVD ROM */
#define SV_DEVID_CDVDROM            0x0D    /**< */
/*  for INVALID device */
#define SV_DEVID_INVALID            0xff    /**< */

//EVT_DVP_FL_MODULE
#define FLMD_NONE             0x0      /**< */
#define FLMD_MP3_PLAYER       0x1      /**< */
#define FLMD_JPEG_DECODER     0x2      /**< */
#define FLMD_MPEG_PLAYER      0x3      /**< */


//define AP_CMD_DVP_SET
#define SV_4_3_PS        0 //default
#define SV_4_3_LB        1
#define SV_WIDE          2
#define SV_WIDESQZ       3

#define EV_ON    		 0
#define EV_OFF    		 1 //default

#define SV_PAL           0
#define SV_AUTO          1
#define SV_NTSC          2 //default

#define TRUE  			 1
#define FALSE 			 0 //default

#define EV_ENGLISH       0 //audio/menu language default
#define EV_FRENCH        1
#define EV_SPANISH       2
#define EV_CHINESE       3
#define EV_JAPANESE      4
#define EV_OTHER         5 //subtitle language default
#define EV_GERMAN        6
#define EV_SWEDISH       7
#define EV_DANISH        8
#define EV_NORWEGIAN     9
#define EV_FINNISH     	 10
#define EV_DUTCH       	 11
#define EV_ICELANDIC   	 12
#define EV_PORTUGUES   	 13
#define EV_HEBREW      	 14
#define EV_GREEK       	 15
#define EV_CROATIAN    	 16
#define EV_TURKISH     	 17
#define EV_ITALIAN     	 18
#define EV_POLISH      	 20
#define EV_HUNGARIAN  	 21
#define EV_CZECH       	 22
#define EV_KOREAN      	 23
#define EV_RUSSIAN     	 24
#define EV_THAI        	 25
#define EV_INDONESIAN  	 26
#define EV_MALAY       	 27
#define EV_IRISH       	 28
#define EV_ROMANIAN    	 29
#define EV_LANG_OFF    	 30

#define SV_PR_KID     	 1
#define SV_PR_G       	 2
#define SV_PR_PG      	 3
#define SV_PR_PG13   	 4
#define SV_PR_PR5     	 5
#define SV_PR_GR     	 6
#define SV_PR_NC      	 7
#define SV_PR_ADULT   	 8 //default
#define SV_PR_OFF     	 0xF

#define EV_DOWN_LTRT     0
#define EV_DOWN_LORO     1 //default
#define EV_DOWN_VSURR    2
#define EV_DOWN_N22      3
#define EV_DOWN_OFF      4

#define EV_LARGE    	 0
#define EV_SMALL    	 1 //default
#define EV_SPK_OFF  	 2

#define EV_AO_OFF        0
#define EV_RAW           1 //default
#define EV_PCM           2
#define EV_MANUAL        3

#define EV_48K           0 //default
#define EV_96K           1


#define MAX_CENTER_DELAY_LEN   17

#define MAX_SUB_DELAY_LEN      51

#define MAX_LS_DELAY_LEN       51

#define MAX_RS_DELAY_LEN       51

#define EV_NONE     		0 //default
#define EV_ROCK     		1
#define EV_POP      		2
#define EV_LIVE     		3
#define EV_DANCE    		4
#define EV_TENCO    		5
#define EV_CLASSIC  		6
#define EV_SOFT     		7

#define EV_PL2_MD_OFF     	0//default
#define EV_PL2_MUSIC      	1
#define EV_PL2_MOVIE      	2
#define EV_PL2_EMU        	3
#define EV_PL2_MATRIX     	4
#define EV_PL2_MD_AUTO    	5
#define EV_PL2_MODE_MAX     6

#define EV_NONE     		0 //default
#define EV_CONCERT        	1
#define EV_LIVING_ROOM    	2
#define EV_HALL           	3
#define EV_BATHROOM       	4
#define EV_CAVE           	5
#define EV_ARENA          	6
#define EV_CHURCH         	7
#define EV_REVERB_MAX     	7

#define EV_STEREO         	0 //default
#define EV_DUAL_LEFT      	1
#define EV_DUAL_RIGHT     	2
#define EV_MIX            	3

//CD Rip Error
#define SUCCESS_CD_RIP		sizeof(AP2DVPCMD_T)
#define ERROR_CD_RIP_PATH_NOT_EXIT					0XF1
#define ERROR_CD_RIP_NOT_ENOUGH_SPACE				0XF2
#define ERROR_CD_RIP_USBSD_WRITE_PROTECT			0XF3
#define ERROR_CD_RIP_DEVICE_ATTACHED_NOT_FUNCTION   0xF4
#define ERROR_CD_RIP_CREATE_FILE_FAIL				0xF5

//DVP Version Info.
#define DVP_FW_VER_ADDR		(0x800010) //DVP memory from 64MB~74MB.
#define DVP_FW_VER_SZ		(4)
#define DVP_SRV_VER_ADDR	(DVP_FW_VER_ADDR+DVP_FW_VER_SZ)
#define DVP_SRV_VER_SZ		(4)

//DVP_CD_TEXT Info.
#define DVP_CD_TEXT_ADDR   (0x71D800)  //CD_TEXT information add by grow.li mtk71267 2012-2.7 001
//DVP File list type
#define FLPARM_LIST_INVALID         0xff
#define FLPARM_LIST_FOLDER          0x0
#define FLPARM_LIST_DISC            0x1
#define FLPARM_LIST_PLAYLIST        0x2
#define FLPARM_LIST_FJPIC           0x3

typedef enum
{
    DVP_PBMODE_REPEAT,         /**< */
    DVP_PBMODE_SHUFFLE,        /**< */
    DVP_PBMODE_RANDOM,         /**< */
} E_DVP_PBMODE;


typedef enum
{
    DVP_IMAGE_VIEWMODE_DIGEST      /**< */
} E_DVP_IMAGE_VIEWMODE;          /**< */

typedef enum
{
    DVP_CHK_PTL_PWD_FAIL,         /**< */
    DVP_CHK_PTL_PWD_OK         /**< */
} E_DVP_CHECK_PWD_RESULT;

#define SV_PBC_OFF                  0    /**< */
#define SV_PBC_ON                   1    /**< */
#define SV_PBC_TP_ON                2    /**< */
#define SV_PBC_TP_OFF				3

//-------for AVSwitch---------
#define SWI_INFO_SHOW        		0x00000001
#define SWI_INFO_HIDE        		0x00000002
#define SWI_INFO_DESTCOLKEY         0x00000004
#define SWI_INFO_SRCCOLKEY          0x00000008
#define SWI_INFO_ALPHA				0x00000010
#define SWI_INFO_RESTORE			0x00000020


typedef enum
{
    SWI_SOURCE_NONE,
    SWI_SOURCE_USBSD,
    SWI_SOURCE_TS,
    SWI_SOURCE_ATV,
    SWI_SOURCE_DTV,
    SWI_SOURCE_DVD,
    SWI_SOURCE_A1,
    SWI_SOURCE_A2,
    SWI_SOURCE_A3,
    SWI_SOURCE_A4,
    SWI_SOURCE_A5,
	SWI_SOURCE_V1,
	SWI_SOURCE_V2,
	SWI_SOURCE_V3,
	SWI_SOURCE_V4,
	SWI_SOURCE_V5,
} SWI_SOURCE_T;


typedef enum
{
    SWI_SINK_NONE,
    SWI_SINK_FRONT,
    SWI_SINK_REAR,
    SWI_SINK_FRONT_REAR,
} SWI_SINK_T;

typedef struct
{
	SWI_SINK_T		eSink;
	RECT 			DestRect;
	UINT32          u4Flag;
	DDOVERLAYFX     ovfx;

}SWI_SINK_INFO_T;



typedef struct _DVP_FILEITEM_INFO_T
{
    GUINT8      uType;            /**< */
    GTCHAR      szFileName[MAX_PATH + 1];  /**< */
} DVP_FILEITEM_INFO_T;


typedef struct _DVP_AUDIO_
{
	INT8 iCurrentAudioChannel;
	UINT8 uAudioChannelCount;
	UINT8 uCurrentAudioLanguage;
	UINT8 uCurrentAudioEncodeCode;
	UINT8 uCurrentAudioIndex;
}DVP_Audio;

typedef struct _DVP_SUBTITLE_
{
	UINT8 u1CurrentSubtitleIndex;
	UINT8 u1TotalSubtitleCount;
	UINT8 iCurrentSubtitleLanguage;
}DVP_Subtitle;

typedef struct _DVP_ID3TEXT_
{
	TCHAR szTitle[62];
	TCHAR szAlbum[62];
	TCHAR szArtist[62];
	TCHAR szGenre[4];
	TCHAR szYear[10];
	GBOOL fgExist;
}DVP_ID3Text;

#define MAX_DVP_LRC_LINE_NUM 	200

typedef struct _DVP_LYRICS_
{
	GUINT32  lrcItemIndex;
	GUINT32  lrcItemPts;
	GUINT32  ptsLen;	
	GWCHAR	*lrcItemText;
	GUINT32  textLen;
}DVP_Lyrics;
#define MAX_DVP_SPECTRUM_NUM  16

typedef struct _DVP_SPECTRUM_
{
	GUINT8  szLongTermValue[MAX_DVP_SPECTRUM_NUM];
	GUINT8  szShortTermValue[MAX_DVP_SPECTRUM_NUM];
	GUINT32 u4DataLen;
}DVP_Spectrum;

typedef enum
{
	DVP_OUT_FRONT,
	DVP_OUT_REAR,
	DVP_OUT_FRONT_REAR,
}DVP_Out;

typedef enum
{
	LAYER_MIX_DISABLE,
	LAYER_MIX_ENABLE
}LAYER_MIX_TYPE;

typedef enum
{
	TVE_AP,
	TVE_DVP
}TVE_CTRL;

typedef enum
{
	CVBS_OFF,
	CVBS_ON
}CVBS_CTRL;

typedef enum
{
    DVP_8032_LOG,         
    DVP_PT110_LOG         
} E_DVP_LOG_TYPE;

typedef struct _DVP_LOG_SET_
{
	E_DVP_LOG_TYPE	eLogType;
	GBOOL			fgDVPLog;
}DVP_LOG_SET;

typedef GRESULT (*PFN_DVPEVENT_LISTENER)(GUINT32 u4Evt, GUINT32 u4Param1, GUINT32 u4Param2,
                                         GUINT32 u4CustomData);

#ifdef __cplusplus
extern "C" {
#endif
/**
*Fill the DVPInfo struct
*
*
*
*@return GRESULT return RET_OK
*/
GRESULT DVP_ParamInit(GVOID);
/**
*Fill the DVPInfo struct and create a timer and thread
*
*
*
*@return GRESULT return RET_OK
*/
GRESULT     DVP_Init(GVOID);
/**
*De-initialise DVP
*
*
*
*@return  NONE
*/
GVOID       DVP_Deinit(GVOID);

/**
*Check DVP Exist 
*
*
*
*@return GBOOL return TRUE or FALSE
*/
GBOOL DVP_CheckMediaExist(GVOID);

/**
* connect operation
*
*
*@return  NONE
*/
GVOID       DVP_Connect(GVOID);
/**
* connect PT110 operation
*
*
*
*@return  NONE
*/
GVOID       DVP_ConnectPT110(GVOID);
/**
* change device by devtype
*
*
*@param[in] uDevType dev type
*
*
*@return  NONE
*/
GVOID       DVP_ChangeDevice(GUINT8 uDevType);
/**
* set notify window
*
*
*@param[in] hWnd the window handle
*@param[in] u4Msg the message
*
*
*@return  GRESULT secceed return RET_OK,or return E_FAIL
*/
GRESULT     DVP_SetNotifyWindow(HWND hWnd, GUINT32 u4Msg);
/**
*
*Sets handle event
*
*
*@return GRESULT secceed return RET_OK,or return E_FAIL
*/
GRESULT     DVP_HandleEvent(GVOID);
/**
*
*Adds new listener
*
*
*@param[in]  pfnListener  listener to be added
*
*
*@param[in]  u4CustomData listener data
*
*
*@return GRESULT secceed return RET_OK,or return E_FAIL
*/
GRESULT     DVP_AddListener(PFN_DVPEVENT_LISTENER pfnListener, GUINT32 u4CustomData);
/**
*
*Removes specific listener
*
*
*@param[in]  pfnListener  listened to be romoved
*
*
*
*
*
*@return GRESULT secceed return RET_OK,or return E_FAIL
*/
GRESULT     DVP_RemoveListener(PFN_DVPEVENT_LISTENER pfnListener);
/**
*
*DVP_Set operation
*
*
*@param[in] uSetType set type
*@param[in] uParam0 param value
*@param[in] uParam1 param value
*@param[in] uParam2 param value
*
*
*@return GRESULT return RET_OK
*/
GRESULT DVP_Set(GUINT8 uSetType, GUINT8 uParam0, GUINT8 uParam1, GUINT8 uParam2);

/**
*
*DVP_ReSet operation
*
*
*@param[in] Address
*
*
*
*
*@return GRESULT return RET_OK
*/
GRESULT DVP_ReSet(GUINT32 Address);

/**
*
*Play operation
*
*
*@return GRESULT return RET_OK
*/
GRESULT     DVP_Play(GVOID);

GRESULT DVP_CheckPwdResult(E_DVP_CHECK_PWD_RESULT ePwdResult);

/**
*
*Pause operation
*
*
*
*@return  GRESULT return RET_OK
*/
GRESULT     DVP_Pause(GVOID);

GRESULT DVP_EnterDVDVideoPB(UINT32 u4SrcWidth, UINT32 u4SrcHeight, SWI_SINK_T eSink, GRECT_T *pDestRect);

GRESULT DVP_ExitDVDVideoPB(SWI_SINK_T eSink);

GRESULT DVP_AVInit(SWI_SINK_INFO_T *prSinkInfo);

GRESULT DVP_AVDeinit(SWI_SINK_T eSink);

GRESULT DVP_AVSwitch(SWI_SINK_INFO_T *prSinkInfo);

GRESULT DVP_SetSurfaceInfo(SWI_SINK_INFO_T *prSinkInfo);


/**
*
*Stop operation
*
*
*@return GRESULT return RET_OK
*/
GRESULT     DVP_Stop(GVOID);

/**
*
*up operation
*
*
*@return GRESULT return RET_OK
*/
GRESULT     DVP_Up(GVOID);
/**
*
*down operation
*
*
*@return GRESULT return RET_OK
*/
GRESULT     DVP_Down(GVOID);
/**
*
*left operation
*
*
*@return GRESULT return RET_OK
*/
GRESULT     DVP_Left(GVOID);
/**
*
*right operation
*
*
*@return GRESULT return RET_OK
*/
GRESULT     DVP_Right(GVOID);
/**
*
*Eject operation
*
*
*
*
*@return  GRESULT return RET_OK
*/
GRESULT     DVP_Eject(GVOID);
/**
*
*Gets disc type 
*
*
*@param[out]  puDiscType  type to be output
*
*
*
*
*
*@return GRESULT secceed return RET_OK,or return E_FAIL
*/
GRESULT     DVP_GetDiscType(GUINT8 *puDiscType);
GRESULT		DVP_SetLMFlag(GUINT8 uFlag);
GRESULT		DVP_GetLMFlag(GUINT8 *puFlag);

/**
*
*@note not implemented 
*/
GRESULT     DVP_SeekToTime(GUINT32 u4Time);
/**
*
*Transform seektofram command  to the DVP host 
*
*
*@param   u4Frame  not used
*
*
*
*
*
*@return GRESULT RET_OK
*/
GRESULT     DVP_SeekToFrame(GUINT32 u4Frame);
/**
*
*Transform play next command  to the DVP host 
*
*
*@return GRESULT RET_OK
*/
GRESULT     DVP_PlayNext(GVOID);
/**
*
*Transform play previous command  to the DVP host 
*
*
*
*@return GRESULT RET_OK
*/
GRESULT     DVP_PlayPrev(GVOID);
/***Transform fast forward command  to the DVP host 
*
*
*@return GRESULT RET_OK
*/
GRESULT     DVP_FFPlay(GVOID);   // add by liduo.zhu 71263

/***Transform fast backward command  to the DVP host 
*
*
*@return GRESULT RET_OK
*/
GRESULT     DVP_FBPlay(GVOID);   // add by liduo.zhu 71263
/**
*
*Gets duration param form a globle variable
*
*
*@param[out]  pu8Duration
*
*
*
*
*
*@return succeed return RET_OK,or return E_FALL
*/
GRESULT     DVP_SetDuration(GUINT64 u8Duration);
GRESULT     DVP_GetDuration(GUINT64 *pu8Duration);
/**
*
*Gets current position  param form a globle variable
*
*
*@param[out]  pu8Current current position
*
*
*
*
*
*@return succeed return RET_OK,or return E_FALL
*/
GRESULT     DVP_GetCurrentPosition(GUINT64 *pu8Current);
/**
*
*Gets device state  param form a globle variable
*
*
*@param[out]  puState output param
*
*
*
*
*
*@return succeed return RET_OK,or return E_FALL
*/
GRESULT     DVP_GetDevState(GUINT8 *puState);
/**
*Gets tray status  param form a globle variable
*
*
*
*@param[out]  puStatus  output param
*
*
*
*
*
*@return succeed return RET_OK,or return E_FALL
*/
GRESULT     DVP_GetTrayStatus(GUINT8 *puStatus);
/**
*
*Gets discinsert  param form a globle variable
*
*
*
*@param[out]  pfgInserted output param
*
*
*
*
*
*@return succeed return RET_OK,or return E_FALL
*/
GRESULT     DVP_IsDiscInsert(GBOOL *pfgInserted);
/**
*
*Gets PB state  param form a globle variable
*
*
*@param[out]  puState output param
*
*
*
*
*
*@return succeed return RET_OK,or return E_FALL
*/
GRESULT     DVP_SetPBState(GUINT8 uState);
GRESULT     DVP_GetPBState(GUINT8 *puState);
/**
*
*Gets track count  param form a globle variable
*
*
*@param[out]  pu2Count output param
*
*
*
*
*
*@return  succeed return RET_OK,or return E_FALL
*/
GRESULT 	DVP_SetPlayingIdx(GUINT16 u2Num);
GRESULT 	DVP_GetPlayingIdx(GUINT16 *pu2Num);
GRESULT 	DVP_SetLastPlayingIdx(GUINT16 u2Num);
GRESULT 	DVP_GetLastPlayingIdx(GUINT16 *pu2Num);
GRESULT     DVP_SetTrackCount(GUINT16 u2Count);
GRESULT     DVP_GetTrackCount(GUINT16 *pu2Count);
GRESULT		DVP_GetFileIdx(GUINT16 *pu2Num);
GRESULT		DVP_SetPlayingDirIdx(GUINT16 u2Num);
GRESULT		DVP_GetPlayingDirIdx(GUINT16 *pu2Num);
GRESULT		DVP_SetExeListType(GUINT8 uType);
GRESULT		DVP_GetExeListType(GUINT8 *puType);

//for audio only
GRESULT 	DVP_SetAudioPlayingIdx(GUINT16 u2Num);
GRESULT 	DVP_GetAudioPlayingIdx(GUINT16 *pu2Num);
GRESULT 	DVP_SetLastAudioPlayingIdx(GUINT16 u2Num);
GRESULT 	DVP_GetLastAudioPlayingIdx(GUINT16 *pu2Num);
GRESULT     DVP_SetAudioTrackCount(GUINT16 u2Count);
GRESULT     DVP_GetAudioTrackCount(GUINT16 *pu2Count);
GRESULT		DVP_GetAudioFileIdx(GUINT16 *pu2Num);
GRESULT		DVP_SetAudioPlayingDirIdx(GUINT16 u2Num);
GRESULT		DVP_GetAudioPlayingDirIdx(GUINT16 *pu2Num);
GRESULT		DVP_SetAudioExeListType(GUINT8 uType);
GRESULT		DVP_GetAudioExeListType(GUINT8 *puType);

GRESULT		DVP_SetTotDirCnt(GUINT16 u2Num);
GRESULT		DVP_GetTotDirCnt(GUINT16 *pu2Num);

/**
*Sets PB mode by writing param to the DVP host
*
*
*
*@param[in]  ePBMode  input PB mode
*
*
*
*
*
*@return succeed return RET_OK,or return E_FALL
*/
GRESULT     DVP_SetPBMode(E_DVP_PBMODE ePBMode);
/**
*
*@note not implemented 
*/
GRESULT     DVP_GetPBMode(E_DVP_PBMODE *pePBMode);
/**
*
*Gets repeat mode  param form a globle variable
*
*
*@param[out]  puMode output param
*
*
*
*
*
*@return  succeed return RET_OK,or return E_FALL
*/
GRESULT     DVP_SetRepeatMode(GUINT8 uMode);
GRESULT     DVP_GetRepeatMode(GUINT8 *puMode);
/**
*
*Gets shuffle mode  param form a globle variable
*
*
*@param[out]  puMode output param
*
*
*
*
*
*@return  succeed return RET_OK,or return E_FALL
*/
GRESULT     DVP_SetPBCUserMode(GUINT8 uMode);
GRESULT     DVP_GetPBCUserMode(GUINT8 *puMode);

GRESULT DVP_GetCurrentDVDMenuState(GUINT8 *puMenuState); 
GRESULT DVP_DVDClickMenuItem(UINT32 u4Coordinate);
GRESULT DVP_JpegDgt_GapAndSize(GUINT8 uHorizontalGap,GUINT8 uVerticalGap, GUINT8 uWidth, GUINT8 uHeight);
GRESULT DVP_JpegDgt_NumAndStartPos(GUINT8 uStartX,GUINT8 uStartY, GUINT8 uHorizontalPicNum, GUINT8 uVerticalPicNum);
/**
*Sets switch audio channel by writing param to the DVP host
*
*
*
*@param[in] iIndex packet param1
*
*
*
*
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_SwitchAudioChannel(GINT8 iIndex);
GRESULT		DVP_ClearId3ExistFlag(GVOID);
GRESULT		DVP_GetId3ExistFlag(GBOOL *pfgFlag);
GRESULT		DVP_ClearAudioId3Txt(GVOID);
GRESULT		DVP_GetCurAudioId3Txt(UINT8 uType, TCHAR *szText, GUINT32 u4MaxLen);



/**
*
*Gets audio channel count  param form a globle variable
*
*
*@param[out]  puCount output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT     DVP_SetAudioChannelCount(GUINT8 uCount);
GRESULT     DVP_GetAudioChannelCount(GUINT8 *puCount);
GRESULT DVP_GetVCDAudioChannelCnt(GUINT8 *puCount);
GRESULT DVP_SetJPGResolution(GUINT16 u2Width, GUINT16 u2Height);
GRESULT DVP_GetJPGResolution(GUINT16 *pu2Width, GUINT16 *pu2Height);
GRESULT DVP_GetVCDCurChannel(GUINT8 *piCurAudChannel);
/**
*
*Gets current audio channel  param form a globle variable
*
*
*@param[out]  piCurrentIdx output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT DVP_GetMpsError(GUINT8 *puMpsError);
GRESULT DVP_GetCurrentAudioIndex(GUINT8 *piCurrentIdx);
/**
*
*Gets current audio channel
*
*
*@param[out]  piCurrentIdx output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT     DVP_GetCurrentAudioChannel(GINT8 *piCurrentIdx);

/**
*
*Gets current audio language by index
*
*@param[in]  iIndex the index
*@param[out]  puLanguage output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT DVP_GetCurAudioLanguage(GUINT8 uIndex, GUINT8 *puLanguage);

/**
*
*Gets current audio encodecode
*
*
*@return  UINT8 current audio encodecode
*/
UINT8 DVP_GetCurAudioEncodecode(GVOID);
/**
*Sets switch subtitle command by writing param to the DVP host
*
*
*
*@param[in] iIndex packet param1
*
*
*
*
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_SwitchSubtitle(GINT8 iIndex);
/**
*
*Gets subtitle count  param form a globle variable
*
*
*@param[out]  puCount output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT     DVP_SetSubtitleCount(GUINT8 u1Count);
GRESULT     DVP_GetSubtitleCount(GUINT8 *pu1Count);
/**
*
*Gets current subtitle  param from a globle variable
*
*
*@param[out]  piCurrentIdx output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT     DVP_GetCurrentSubtitle(GUINT8 *puCurrentIdx);
/**
*
*Gets subtitle language  param form a globle variable
*
*
*@param[in]  iIndex subtitle index
*
*
*@param[out] puLanguage  output param
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT     DVP_GetSubtitleLanguage(GINT8 iIndex, GUINT8 *puLanguage);
/**
*
*@note not implemented
*/
GRESULT DVP_SwitchPBCAngle(GUINT8 uAngle);
/**
*
*Gets PBC angle count  param from a globle variable
*
*
*@param[out]  puCount output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT     DVP_GetPBCAngleCount(GUINT8 *puCount);
/**
*
*Gets current PBC angle   param from a globle variable
*
*
*@param[out]  puAngle  output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT     DVP_GetCurrentPBCAngle(GINT8 *puAngle);
/**
*
*Gets current DVD Title   param from a global variable
*
*
*@param[out]  piCurrent  output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT		DVP_GetCurrentTitle(GUINT8 *puCurrent);
/**
*
*Gets DVD Title count  param from a global variable
*
*
*@param[out]  puCount  output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT		DVP_GetTitleCount(GUINT8 *puCount);
/**
*
*Gets current DVD chapter  param from a global variable
*
*
*@param[out]  piCurrent  output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT		DVP_GetCurrentChapter(GUINT8 *puCurrent);
/**
*
*Gets DVD chapter count param from a global variable
*
*
*@param[out]  puCount  output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT		DVP_GetChapterCount(GUINT8 *puCount);
/**
*Sets goto title menu command  by writing param to the DVP host
*
*
*
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_GotoTitleMenu(GVOID);

/**
*Sets zoom in  command  by writing param to the DVP host
*
*
*
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_ZoomIn(GVOID);
/**
*Sets zoom out command  by writing param to the DVP host
*
*
*
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_ZoomOut(GVOID);
/**
*
*Gets current DVD Zoom Multiple param from a global variable
*
*
*@param[out]  puCurrent  output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT DVP_GetCurrentZoomMultiple(GUINT8 *puCurrent);
GRESULT DVP_GetFLListType(GUINT8 *puCurrent);
GRESULT DVP_GetFLCurDirIndex(GUINT16 *pu2Current);

/**
*Sets goto root menu command  by writing param to the DVP host
*
*
*
*
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_GotoRootMenu(GVOID);
/**
*Sets set PBC state  command  by writing param to the DVP host
*
*
*
*@param[in] uState  packet param1
*
*
*
*
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_SetPBCState(GUINT8 uState);
/**
*
*Gets PBC state  param from a globle variable
*
*
*@param[out]  puState output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT     DVP_GetPBCState(GUINT8 *puState);
/**
*Sets rotate image command  by writing param to the DVP host
*
*
*
*@param[in]  uAngle  packet param1
*
*
*
*
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_RotateImage(GUINT8 uAngle);
/**
*Sets image view mode
*
*
*
*@param[in]  eViewMode  view mode
*
*
*
*
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_SetImageViewMode(E_DVP_IMAGE_VIEWMODE eViewMode);

GRESULT 	DVP_ChooseDigitalNum(GUINT8 uType, GUINT8 uDigitalNum);
GRESULT 	DVP_CheckLastMemory(void);

/**
*Sets create list command  by writing param to the DVP host
*
*
*
*@param[in]  uFilterType   packet param1
*
*
*
*
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_CreateFileDatabase(GUINT8 uFilterType);
/**
*
*Gets current file count  param from a globle variable
*
*
*@param[out]  pu2Count  output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT     DVP_GetCurFileCount(GUINT16 *pu2Count);

/**
*
*Gets current dir video count  param from a globle variable
*
*
*@param[out]  pu2Count  output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT  DVP_GetCurDirVideoCount(GUINT16 *pu2Count);

/**
*
*Gets current dir audio count  param from a globle variable
*
*
*@param[out]  pu2Count  output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT  DVP_GetCurDirAudioCount(GUINT16 *pu2Count);

/**
*
*Gets current dir picture count  param from a globle variable
*
*
*@param[out]  pu2Count  output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT  DVP_GetCurDirPicCount(GUINT16 *pu2Count);


/**
*
*Gets current Current ThumbNail Fisrt Picture Index from a globle variable
*
*
*@param[out]  pu2Count  output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/

GRESULT DVP_GetThumbNailFirstIdx(GUINT16 *pu2Count);


/**
*
*Gets current file path
*
*
*@param[out]  szFilePath  current file path
*@param[in]  u4Len the max length of current file path
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT DVP_ClrCurFilePath(GVOID);
GRESULT DVP_GetCurFilePath(GTCHAR *szFilePath, GUINT32 u4Len);
/**
*Gets current directory  param from a globle variable
*
*
*@param[out] szDir output file directory
*@param[in] u4Len the length of directory
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT DVP_GetCurrentDir(GTCHAR *szDir, GUINT32 u4Len);
/**
*get file item info by index
*
*
*
*@param[in]  i2Index index
*
*
*@param[out]  prItemInfo file item info
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT     DVP_GetFileItemInfo(GINT16 i2Index, DVP_FILEITEM_INFO_T *prItemInfo);

GRESULT 	DVP_GetDramFileItemInfo(GINT16 i2Index, DVP_FILEITEM_INFO_T *prItemInfo);

/**
*
*Gets change dir index
*
*
*@param[out]  pu2Index  output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT DVP_GetChgDirIndex(GUINT16 *pu2Index);

/**
*
*change base file ID for re-get item from DVP side when get item failed.x
*
*
*@param[out]  None
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT DVP_SetBaseFileID(GINT32 i4Index);

/**
*
*Get base file ID
*
*
*@param[out]  pi4Index output param
*
*
*
*
*
*@return  GRESULT  return RET_OK
*/
GRESULT DVP_GetBaseFileID(GINT32 *pi4Index);

/**
*
*
*
*
*@param[in]  i2Index
*
*
*
*
*
*@return GRESULT return RET_OK
*/
GRESULT     DVP_ItemClick(GINT16 i2Index);
/**
*change current dir
*
*
*@param[in]  i2Index the index
*
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_ChangeCurrentDir(GINT16 i2Index);
/**
*Sets file uplevel  command  by writing param to the DVP host
*
*
*
*
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_GotoUpLevel(GVOID);
/**
*init files buffer by index
*
*
*
*@param[in]  i2Index the index
*
*
*
*
*
*@return GRESULT 
*/
GRESULT     DVP_InitFilesBuffer(GINT16 i2Index);
/**
*set MP3 file path on USB/SD
*
*
*
*@param[in]  szPath  packet param1
*
*
*@param[in]  u4Len  packet param2
*
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_SetRipPath(GTCHAR *szPath, GUINT32 u4Len);//set MP3 file path on USB/SD

GRESULT 	DVP_SetRipSpeedMode(GUINT8 uSpeedMode);

GRESULT     DVP_SetDvdRegion(GUINT8 uDvdRegionNum);

/**
*start rip of u2Track
*
*
*
*@param[in]  uTrack packet param1
*
*
*
*
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_StartRip(GUINT8 uTrack);//start rip of u2Track
/**
*stop current rip
*
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_StopRip();//stop current rip
/**
*
*@note not implemented
*/
GRESULT     DVP_GetRipProgress(GUINT32 u4Progress);//get progress of current rip

/**
*get ripping error type
*
*@param[in] puType type
*
*
*@return GRESULT  return RET_OK
*/
GRESULT		DVP_GetRipErrType(GUINT8 *puType);

/**
*get LBA length in dvp
*
*@param[in] u4TrackId track id
*@param[out] pu4TrkLbaLen LBA length
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_GetRipTrkLbaLen(GUINT32 u4TrackId, GUINT32 *pu4TrkLbaLen);

/**
*switch video mix from dvp to ap
*
*@param[in] uType the mix
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_2APMix(GUINT8 uType);
/**
*switch video mix from dvp to ap by hw path
*
*@param[in] fgMix the mix
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_Mix2APHWPath(GBOOL fgMix);
GRESULT     DVP_Mix2APWrChannel(GBOOL fgMix, GRECT_T *rRect);
/**
*clear dvp data buffer
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_ClearDataBuffer(GVOID);

GRESULT DVP_ClearDramDataBuffer(GVOID);

/**
*set audio output front or rear
*
*@param[in] eDvpOut front/rear/front+rear
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_AudOutput(DVP_Out eDvpOut);

GRESULT DVP_4LayerMix(LAYER_MIX_TYPE eLayerMixType);

/**
*switch tve to ap or dvp
*
*@param[in] eTveCtrl ap or dvp
*
*@return GRESULT  return RET_OK
*/
GRESULT     DVP_SwitchTVE(TVE_CTRL eTveCtrl);

GRESULT DVP_CVBSControl(CVBS_CTRL eCVBSCtrl);


GRESULT DVP_RearSwitch(GBOOL fgON);

/**
*get lyric item count
*
*@param[out] pu4Count lyric item count
*
*@return GRESULT  return RET_OK
*/
GRESULT 	DVP_GetLyricItemCount(GUINT32 *pu4Count);

GRESULT 	DVP_GetLyricItemPts(GUINT32 u4Index, GUINT32 *pu4Second);
/**
*get lyric item text
*
*@param[in] i4Index index in lyric item
*@param[out] szLyric out param
*@param[in] u4MaxLen the max length
*
*@return GRESULT  return RET_OK
*/
GRESULT 	DVP_GetLyricItemText(GUINT32 u4Index, GTCHAR *szLyric, GUINT32 u4MaxLen);

GBOOL       DVP_IsVideo2AP(GVOID);

GRESULT 	DVP_GetId3Pic(GUINT8 **puId3Pic, GUINT32 *u4PicLen);

GRESULT 	DVP_ClearLyricOrPicBuffer(GBOOL fgClearLyricBuffer);

GRESULT 	DVP_GetLMClickItemType(GUINT8 *puClickItemType);

GRESULT     DVP_GetFastPlaySpeed(GUINT8 *u1PlaySpeed);
GRESULT		DVP_GetDDPlayType(GUINT8 *puPlayType);

GRESULT		DVP_SetDDPlayType(GUINT8 uPlayType);

GRESULT 	DVP_AudioInfo_SetSpectrumMaxVal(UINT8 uMaxVal);

GRESULT 	DVP_AudioInfo_GetSpectrumMaxVal(UINT8 *puMaxVal);

GRESULT 	DVP_AudioInfo_GetSpectrum(UINT8 *puSValue, UINT8 *puLValue);

GRESULT 	DVP_HandleResponse(GUINT32 u4Param1, GUINT32 u4Param2);

GRESULT 	DVP_SetDVPLog(DVP_LOG_SET rDVPLog);

GRESULT 	DVP_WriteData2Dram(DWORD dwStartAddress, void *pvBufferIn, GUINT32 u4Size);

GRESULT 	DVP_ReadDataFromDram(DWORD dwStartAddress, void *pvBufferOut, GUINT32 u4Size);

GRESULT 	DVP_ReadFileInfoFromDram(DVP_FILEITEM_INFO_T *prItemInfo, GUINT32 u4ReadFileNum, GUINT32 u4ReadIndex);
GRESULT		DVP_GetFirmwareVer(GUINT8 *puData);

GRESULT		DVP_GetServoVer(GUINT8 *puData);
GRESULT		DVP_GetPrepareState(GUINT8 *puState);

GRESULT     DVP_GetCdText(UINT8 uType , UINT8 uTrack, GTCHAR *puData);  //add by grow.li mtk71267 2012-2-7 001
//AP send get cmd to DVP, and then DVP send msg to AP.
GRESULT		DVP_GetPrepareStateFromDVP(GVOID);
GRESULT		DVP_GetInitStateFromDVP(GVOID);
GRESULT		DVP_GetDiscTypeFromDVP(GVOID);
GRESULT		DVP_GetTotCntFromDVP(GVOID);
GRESULT		DVP_GetCurIndexFromDVP(GVOID);
GRESULT		DVP_GetPlayStateFromDVP(GVOID);
GRESULT		DVP_GetRepeatModeFromDVP(GVOID);
GRESULT		DVP_GetPBCUserModeFromDVP(GVOID);
GRESULT		DVP_GetPBCFromDVP(GVOID);
GRESULT		DVP_GetAudioFromDVP(GVOID);
GRESULT		DVP_GetSubtitleFromDVP(GVOID);
GRESULT		DVP_GetAngleFromDVP(GVOID);

#ifdef __cplusplus
}
#endif

#endif //_DVP_AGENT_H_