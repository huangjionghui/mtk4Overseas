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

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   $Workfile:  $ 
 *
 * Project:
 * --------
 *   GPS Pilot
 *
 * Description:
 * ------------
 *   
 *   
 * Author:
 * -------
 *   mcn03046
 *
 * Last changed:
 * ------------- 
 * $Author: jialei.shi $ 
 *
 * $Modtime: $  
 *
 * $Revision: #2 $
****************************************************************************/

#ifndef _GRECT_H_
#define _GRECT_H_

#include "GDef.h"

#ifdef __cplusplus
extern "C" {
#endif


 
/**
 * @defgroup Utilities Utilities
 * @brief Rectangles are used for the cursor clipping region, the invalid
 *        portion of the client area, an area for displaying formatted text,
 *        or the scroll area. Your applications can also use rectangles to
 *        fill, frame, or invert a portion of the client area with a given
 *        brush, and to retrieve the coordinates of a widget or a widget's
 *        client area. 
 */
/*@{*/


/**
 * @brief The GPOINT_T structure defines the x- and y- coordinates of a point. 
 */
typedef struct
{
    GINT32 i4X; /**< Specifies the x-coordinate of the point. */
    GINT32 i4Y; /**< Specifies the y-coordinate of the point.*/
} GPOINT_T;

/**
 * @brief The GSIZE_T structure specifies the width and height of a rectangle. 
 */
typedef struct
{
    GINT32 i4Width; /**< Specifies the rectangle's width. */
    GINT32 i4Height; /**< Specifies the rectangle's height. */
} GSIZE_T;

/**
 * @brief The GRECT_T structure defines the coordinates of the upper-left and
 *        lower-right corners of a rectangle.
 */
typedef struct
{
    GINT32 i4Left; /**< Specifies the x-coordinate of the upper-left corner of the rectangle. */
    GINT32 i4Top; /**< Specifies the y-coordinate of the upper-left corner of the rectangle. */
    GINT32 i4Right; /**< Specifies the x-coordinate of the lower-right corner of the rectangle. */
    GINT32 i4Bottom; /**< Specifies the y-coordinate of the lower-right corner of the rectangle. */
} GRECT_T;

/**
 * @brief The GSIZERECT_T structure defines the coordinates of the upper-left,
 *        width and height of a rectangle.
 */
typedef struct
{
    GINT32 i4X; /**< Specifies the x-coordinate of the upper-left corner of the rectangle. */
    GINT32 i4Y; /**< Specifies the y-coordinate of the upper-left corner of the rectangle. */
    GINT32 i4Width; /**< Specifies the width of the rectangle. */
    GINT32 i4Height; /**< Specifies the height of the rectangle. */
} GSIZERECT_T;


/**
 * Set the coordinates of the specified rectangle. This is equivalent to
 * assigning the left, top, right, and bottom arguments to the appropriate
 * members of the GRECT_T structure.  
 *
 * @param  prRect   -- Pointer to the GRECT_T structure that contains the rectangle to
 *                     be set.  
 * @param  i4Left   -- Specifies the x-coordinate of the rectangle's upper-left corner. 
 * @param  i4Top    -- Specifies the y-coordinate of the rectangle's upper-left corner.
 * @param  i4Right  -- Specifies the x-coordinate of the rectangle's lower-right corner. 
 * @param  i4Bottom -- Specifies the y-coordinate of the rectangle's lower-right corner.
 *
 * @return NONE
 */
GAPI GVOID GSetRect(GRECT_T *prRect, GINT32 i4Left, GINT32 i4Top, GINT32 i4Right,
                   GINT32 i4Bottom);

/**
 * Create an empty rectangle in which all coordinates are set to zero.  
 *
 * @param  prRect -- Pointer to the GRECT_T structure that contains the coordinates of the
 *                   rectangle.  
 *
 * @return NONE
 */
GAPI GVOID GSetRectEmpty(GRECT_T *prRect);

/**
 * Copy the coordinates of one rectangle to another. 
 *
 * @param  prDst -- Pointer to the GRECT_T structure that receives the logical
 *                  coordinates of the source rectangle. 
 * @param  prSrc -- Pointer to the RECT structure whose coordinates are to be
 *                  copied in logical units.
 *
 * @return NONE
 */
GAPI GVOID GCopyRect(GRECT_T *prDst, const GRECT_T *prSrc);

/**
 * Determine whether the two specified rectangles are equal by comparing the
 * coordinates of their upper-left and lower-right corners. 
 *
 * @param  prSrc1 -- Pointer to a GRECT_T structure that contains the logical
 *                   coordinates of the first rectangle.  
 * @param  prSrc2 -- Pointer to a GRECT_T structure that contains the logical
 *                   coordinates of the second rectangle. 
 *
 * @return If the two rectangles are identical, the return value is TRUE, otherwise
 *         the return value is FALSE.
 */
GAPI GBOOL GIsRectEqual(const GRECT_T *prSrc1, const GRECT_T *prSrc2);

/**
 * Determine whether the specified rectangle is empty. A empty rectangle is one that
 * has no area; that is, the coordinate of the right side is less than or equal to
 * the coordinate of the left side, or the coordinate of the bottom side is less than
 * or equal to the coordinate of the top side. 
 *
 * @param  prRect -- Pointer to a GRECT_T structure that contains the logical
 *                   coordinates of the rectangle.   
 *
 * @return If the rectangle is empty, the return value is TRUE, otherwise
 *         the return value is FALSE.
 */
GAPI GBOOL GIsRectEmpty(const GRECT_T *prRect);

/**
 * Determine whether the specified point lies within the specified rectangle. A
 * point is within a rectangle if it lies on the left or top side or is within all
 * four sides. A point on the right or bottom side is considered outside the rectangle. 
 *
 * @param  prRect -- Pointer to a GRECT_T structure that contains the specified
 *                   rectangle. 
 * @param  x      -- Specifies the x-coordinate of the specified point.
 * @param  y      -- Specifies the y-coordinate of the specified point.
 *
 * @return If the rectangle is empty, the return value is FALSE, otherwise
 *         the return value is TRUE.
 */
GAPI GBOOL GPtInRect(const GRECT_T *prRect, GINT32 x, GINT32 y);

/**
 * Determine whether the specified point lies within the specified rectangle. A
 * point is within a rectangle if it lies on the left or top side or is within all
 * four sides. A point on the right or bottom side is considered outside the rectangle. 
 *
 * @param  prRect -- Pointer to a GRECT_T structure that contains the specified
 *                   rectangle. 
 * @param  prPt   -- Specifies a GPOINT_T structure that contains the specified point.
 *
 * @return If the rectangle is empty, the return value is TRUE, otherwise
 *         the return value is FALSE.
 */
GAPI GBOOL GPtInRectEx(const GRECT_T *prRect, const GPOINT_T *prPt);

/**
 * Calculate the intersection of two source rectangles and places the coordinates of the
 * intersection rectangle into the destination rectangle. If the source rectangles do not
 * intersect, an empty rectangle (in which all coordinates are set to zero) is placed into
 * the destination rectangle.
 *
 * @param  prDst  -- Pointer to the GRECT_T structure that is to receive the intersection
 *                   of the rectangles pointed to by the prSrc1 and prSrc2 parameters. This
 *                   parameter can be NULL.
 * @param  prSrc1 -- Pointer to the GRECT_T structure that contains the first source rectangle. 
 * @param  prSrc2 -- Pointer to the GRECT_T structure that contains the second source rectangle. 
 *
 * @return If the rectangles intersect, the return value is TRUE, otherwise the return value
 *         is FALSE.
 */
GAPI GBOOL GIntersectRect(GRECT_T *prDst, const GRECT_T *prSrc1,
                         const GRECT_T *prSrc2);

/**
 * Create the union of two rectangles. The union is the smallest rectangle that contains both
 * source rectangles.
 *
 * @param  prDst  -- Pointer to the GRECT_T structure that will receive a rectangle containing
 *                   the rectangles pointed to by the prSrc1 and prSrc2 parameters. 
 * @param  prSrc1 -- Pointer to the GRECT_T structure that contains the first source rectangle.  
 * @param  prSrc2 -- Pointer to the GRECT_T structure that contains the second source rectangle. 
 *
 * @return NONE
 */
GAPI GVOID GUnionRect(GRECT_T *prDst, const GRECT_T *prSrc1, const GRECT_T *prSrc2);

/**
 * Move the specified rectangle by the specified offsets. 
 *
 * @param  prRect -- Pointer to a GRECT_T structure that contains the logical
 *                   coordinates of the rectangle to be moved. 
 * @param  dx     -- Specifies the amount to move the rectangle left or right.
 *                   This parameter must be a negative value to move the rectangle
 *                   to the left. 
 * @param  dy     -- Specifies the amount to move the rectangle up or down. This
 *                   parameter must be a negative value to move the rectangle up.
 *
 * @return NONE
 */
GAPI GVOID GOffsetRect(GRECT_T *prRect, GINT32 dx, GINT32 dy);

/**
 * Increase or decrease the width and height of the specified rectangle. The GInflateRect function
 * adds dx units to the left and right ends of the rectangle and dy units to the top and bottom. 
 * The dx and dy parameters are signed values; positive values increase the width and height, and
 * negative values decrease them.  
 *
 * @param  prRect -- Pointer to the GRECT_T structure that increases or decreases in size. 
 * @param  dx     -- Specifies the amount to increase or decrease the rectangle width. This
 *                   parameter must be negative to decrease the width.  
 * @param  dy     -- Specifies the amount to increase or decrease the rectangle height. This
 *                   parameter must be negative to decrease the height.
 *
 * @return NONE
 */
GAPI GVOID GInflateRect(GRECT_T *prRect, GINT32 dx, GINT32 dy);

/*@}*/ //end of rectangles group

#ifdef __cplusplus
}
#endif

#endif /* _GRECT_H_ */