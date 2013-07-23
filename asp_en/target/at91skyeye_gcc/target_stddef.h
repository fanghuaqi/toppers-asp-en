/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2008 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
 *	(1) When this software is used in the form of source code, the above
 *    	copyright notice, this use conditions, and the disclaimer shown
 *    	below must be retained in the source code without modification.
 *
 *	(2) When this software is redistributed in the forms usable for the
 *    	development of other software, such as in library form, the above
 *    	copyright notice, this use conditions, and the disclaimer shown
 *    	below must be shown without modification in the document provided
 *    	with the redistributed software, such as the user manual.
 *
 *	(3) When this software is redistributed in the forms unusable for the
 *    	development of other software, such as the case when the software
 *    	is embedded in a piece of equipment, either of the following two
 *   	 conditions must be satisfied:
 *
 *  	(a) The above copyright notice, this use conditions, and the
 *         	disclaimer shown below must be shown without modification in
 *     		the document provided with the redistributed software, such as
 *      	the user manual.
 *
 * 		(b) How the software is to be redistributed must be reported to the
 *     		TOPPERS Project according to the procedure described
 *     		separately.
 *
 *	(4) The above copyright holders and the TOPPERS Project are exempt
 *    	from responsibility for any type of damage directly or indirectly
 *   	caused from the use of this software and are indemnified by any
 *    	users or end users of this software from any and all causes of
 *    	action whatsoever.
 *
 *	THIS SOFTWARE IS PROVIDED "AS IS." THE ABOVE COPYRIGHT HOLDERS AND
 *	THE TOPPERS PROJECT DISCLAIM ANY EXPRESS OR IMPLIED WARRANTIES,
 *	INCLUDING, BUT NOT LIMITED TO, ITS APPLICABILITY TO A PARTICULAR
 *	PURPOSE. IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS AND THE
 *	TOPPERS PROJECT BE LIABLE FOR ANY TYPE OF DAMAGE DIRECTLY OR
 *	INDIRECTLY CAUSED FROM THE USE OF THIS SOFTWARE.
 * 
 *  @(#) $Id: target_stddef.h 1838 2010-07-09 15:57:51Z ertl-honda $
 */

/*
 *  the chip dependent part of t_stddef.h (for AT91SKYEYE)
 *
 *  This file is included in t_stddef.h. Othe files should not include it
 *  directly. 
 */

#ifndef TOPPERS_TARGET_STDDEF_H
#define TOPPERS_TARGET_STDDEF_H

/*
 *  the target identify macro
 */
#define TOPPERS_AT91SKYEYE               /* system name */

/*
 *  common definitions of tool chain
 * 
 *  If stdint.h is prepared in tool chain, delete TOPPERS_STDINT_TYPE1
 *  and include stdint.h  
 */
#define TOPPERS_STDINT_TYPE1
#define TOPPERS_STDFLOAT_TYPE1
#include "gcc/tool_stddef.h"

/*
 *  core common definitions
 */
#include "arm_gcc/common/core_stddef.h"


#ifndef TOPPERS_MACRO_ONLY

/*
 *  the assert abort processing (to be done)
 */
Inline void
TOPPERS_assert_abort(void)
{    
}

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_TARGET_STDDEF_H */
