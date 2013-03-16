#ifndef _MSDK_DVXMENU_H_
#define _MSDK_DVXMENU_H_

#include <MSDK_Defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
*@addtogroup MediaInfo
*@{
*/

//action types for divx menu file playback
typedef enum
{
    MSDK_ACTION_NONE       = 0x00,/**<action type unknown    */
    MSDK_ACTION_ENTER      = 0x01,/**<enter key press		   */
    MSDK_ACTION_LEFT       = 0x02,/**<left  arrow key press	   */
    MSDK_ACTION_RIGHT      = 0x03,/**<right  arrow key press  */
    MSDK_ACTION_UP         = 0x04,/**<up arrow  key press	   */
    MSDK_ACTION_DOWN       = 0x05,/**<down arrow key press  */
    MSDK_ACTION_CLICK      = 0x06 /**<mouse click			   */
}MSDK_ACTION_TYPE_T;

typedef struct 
{
    MSDK_ACTION_TYPE_T eActionType;/**<MSDK_ACTION_TYPE_T action type*/
    INT32           i4ClickX;	   /**<click position x-axis value		*/
    INT32           i4ClickY;	   /**<click position y-axis value		*/
} MSDK_ACTION_PARAM_T;


/**
*	@brief This method judge video file type(divx menu or not). 
*
*	@param[in] 	hDvxMenuInfo  divx menu info handle.
*	@param[out]  	pbMenu  true if divx menu file. 
*	
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_QueryInterface()
*	@see	msdk_error.h
*
**/
MRESULT     DvxMenuInfo_IsDvxMenuFile(HDVXMENUINFO hDvxMenuInfo, GBOOL *pbMenu);

/**
*	@brief This method judge divx menu file play state(menu show or normal play). 
*
*	@param[in] 	hDvxMenuInfo  divx menu info handle.
*	@param[out]  	pbMenuPlay  true if menu show and background play,false is normal play. 
*	
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_QueryInterface()
*	@see	msdk_error.h
*
**/
MRESULT     DvxMenuInfo_IsDvxMenuPlay(HDVXMENUINFO hDvxMenuInfo, GBOOL *pbMenuPlay);


/**
*	@brief This method execute action for user operation. 
*
*	@param[in] 	hDvxMenuInfo  divx menu info handle.
*	@param[out]  	prActionParam  MSDK_ACTION_PARAM_T pointer pass action parameter to called. 
*	
*	@return	MRESULT  define in msdk_error.h
*			   
*	@see	MediaGraph_QueryInterface()
*	@see	msdk_error.h
*	@see	MSDK_ACTION_PARAM_T
**/
MRESULT     DvxMenuInfo_DoAction(HDVXMENUINFO hDvxMenuInfo, MSDK_ACTION_PARAM_T *prActionParam);
/**
*@}
*/

#ifdef __cplusplus
}
#endif

#endif //_MSDK_DVXMENU_H_
