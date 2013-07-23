/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2006 by GJ Business Division RICOH COMPANY,LTD. JAPAN
 *  Copyright (C) 2007 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: target_timer.h 1838 2010-07-09 15:57:51Z ertl-honda $
 */

/*
 *  timer driver (for AT91SKYEYE)
 */

#ifndef TOPPERS_TARGET_TIMER_H
#define TOPPERS_TARGET_TIMER_H

#include <sil.h>
#include "at91skyeye.h"

/*
 *  timer interrupt related definitions
 */
#define INHNO_TIMER     IRQ_TC1      /* interrupt handlerno */
#define INTNO_TIMER     IRQ_TC1      /* interrupt no */
#define INTPRI_TIMER    -6           /* interrupt priority */
#define INTATR_TIMER    0U           /* interrupt attribute */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  CLOCK type
 */
typedef uint32_t    CLOCK;

/*
 *  the transfer from timer clock to ms and us.
 *
 */
#define TIMER_CLOCK          2048U
#define TO_CLOCK(nume, deno) (TIMER_CLOCK * (nume) / (deno))
#define TO_USEC(clock)       (((SYSUTM) clock) * 1000U / TIMER_CLOCK)


/*
 *  the max timer period (the unit is CLOCK)
 */
#define MAX_CLOCK        ((CLOCK) 0xffffffU)

/*
 *  check whetehr the current value of timer is the value before timer
 *  interrupt happened
 */
#define GET_TOLERANCE    100U    /* the possible delay (unit is CLOCK)*/
#define BEFORE_IREQ(clock) \
            ((clock) >= TO_CLOCK(TIC_NUME, TIC_DENO) - GET_TOLERANCE)

/*
 *  initialize the timer
 *
 *  initialize the timer and set the timer period.
 */
extern void    target_timer_initialize(intptr_t exinf);

/*
 *  terminate the timer
 *
 */
extern void    target_timer_terminate(intptr_t exinf);

/*
 *  get the current value of timer
 */
Inline CLOCK
target_timer_get_current(void)
{
    return(TO_CLOCK(TIC_NUME, TIC_DENO) - sil_rew_mem((void *)TIMER_1_CV));
}

/*
 *  probe timer interrupt
 */
Inline bool_t
target_timer_probe_int(void)
{
    return(x_probe_int(INTNO_TIMER));
}

/*
 *  timer interrupt handler
 */
extern void    target_timer_handler(void);

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_TARGET_TIMER_H */
