/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2006 by GJ Business Division RICOH COMPANY,LTD. JAPAN  
 *  Copyright (C) 2007-2010 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: target_timer.c 1838 2010-07-09 15:57:51Z ertl-honda $
 */

/*
 *  timer driver (for AT91SKYEYE)
 */

#include "kernel_impl.h"
#include "time_event.h"
#include <sil.h>
#include "target_timer.h"

/*
 *  kernel timer initialization
 */
void
target_timer_initialize(intptr_t exinf)
{
    CLOCK    cyc = TO_CLOCK(TIC_NUME, TIC_DENO);

    /*
     *  kernel timer related configuration
     */
    /* configure period */
    sil_wrw_mem((void *)TIMER_1_RC, cyc);
    /* enable timer reload */
    sil_wrw_mem((void *)TIMER_1_CMR, 2 << 13);
    /* enable the timer */
    sil_wrw_mem((void *)TIMER_1_CCR, 0x0000001);
}

/*
 *  kernel timer termination
 */
void
target_timer_terminate(intptr_t exinf)
{
 
}

/*
 *  kernel timer interrupt handler
 */
void
target_timer_handler(void)
{
    i_begin_int(INTNO_TIMER);
    signal_time();                    /* signal the kernel */
    i_end_int(INTNO_TIMER);
}
