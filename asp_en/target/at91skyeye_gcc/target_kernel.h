/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2006-2007 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
 *	The above copyright holders grant permission gratis to use,
 *	duplicate, modify, or redistribute (hereafter called use) this
 *	software (including the one made by modifying this software),
 *	provided that the following four conditions (1) through (4) are
 *	satisfied.
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
 *  @(#) $Id: target_kernel.h 1838 2010-07-09 15:57:51Z ertl-honda $
 */

/*
 *  the chip dependent part of kernel.h (for AT91SKYEYE)
 *
 *  This file is included in kernel.h. Othe files should not include it
 *  directly. Before this file is included, t_stddef.h should also be 
 *  included
 */

#ifndef TOPPERS_TARGET_KERNEL_H
#define TOPPERS_TARGET_KERNEL_H

/*
 *  the core dependent common definitions (for ARM)
 */
#include "arm_gcc/common/core_kernel.h"

/*
 *  the supported kernel functions
 */
#define TOPPERS_TARGET_SUPPORT_ENA_INT		/* support ena_int */
#define TOPPERS_TARGET_SUPPORT_DIS_INT		/* support dis_int */
#define TOPPERS_TARGET_SUPPORT_GET_UTM		/* support get_utm */
/*
 *  the range of interrupt priority
 */
#define TMIN_INTPRI        (-7)        /* the min value of interrupt priority (highest priority)*/
#define TMAX_INTPRI        (-1)        /* the max value of interrupt priority (lowest priority)*/

/*
 *  time tick definitions
 */
#define TIC_NUME     1U            /* the numerator of time tick */
#define TIC_DENO     1U            /* the denominator of time tick*/

/*
 *  interrupt attribute
 */
#define	TA_POSEDGE		0x04U		/* positive edge trig */
#define	TA_LOWLEVEL		0x00U		/* low level trig */
#define	TA_HIGHLEVEL	0x08U		/* high level trig */

/* 
 *  trace log related definition
 *  use cycle counter as log timer
 */ 
#define TRACE_GET_TIM()   cycle_counter_get_current()

#endif /* TOPPERS_TARGET_KERNEL_H */
