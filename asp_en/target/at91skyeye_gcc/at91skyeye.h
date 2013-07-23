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
 *  @(#) $Id: at91skyeye.h 1838 2010-07-09 15:57:51Z ertl-honda $
 */

#ifndef TOPPERS_AT91SKYEYE_H
#define TOPPERS_AT91SKYEYE_H

#include <sil.h>

/*
 * the macro of getting bit pattern of IRT from intno
 */
#define INTNO_BITPAT(intno) (1U << intno)

/*
 *  interrupt controller related definitions
 */
#define AIC_IVR      (0xFFFFF100)
#define AIC_ISR      (0xFFFFF108)
#define AIC_IMR      (0xFFFFF110)
#define AIC_IECR     (0xFFFFF120)
#define AIC_IDCR     (0xFFFFF124)
#define AIC_ICCR     (0xFFFFF128)
#define AIC_EOI      (0xFFFFF130)
#define AIC_IPR      (0xFFFFF10C)

#define IRQ_USART0      2
#define IRQ_TC1         5

/*
 *  timer related definitions
 */
#define TIMER_1_CCR      (0xFFFE0040)
#define TIMER_1_CMR      (0xFFFE0044)
#define TIMER_1_CV       (0xFFFE0050)
#define TIMER_1_SR       (0xFFFE0060)
#define TIMER_1_RC       (0xFFFE005C)
#define PIT_PIVR         (0xFFFFFD38)

/*
 * the registers about multiprocessor function in device manager
 * these registers are integrated with the configuration in skyeye's
 * conf file.
 */
#define MUTEX_ID_REG     (0xFFFFFF00)
#define MUTEX_CNT_REG    (0xFFFFFF04)
#define IPI_REG          (0xffffff80)

/*
 *  UART related definitions
 */
#define USART0_THR       (0xFFFD001C)  // USART Transmitter Holding Register
#define USART0_RPR       (0xFFFD0030)  // USART Receive Pointer Register
#define USART0_RCR       (0xFFFD0034)  // USART Receive Counter Register
#define USART0_CSR       (0xFFFD0014)  // USART Channel Satus Register
#define USART0_IER       (0xFFFD0008)  // USART Interrupt Enable Register
#define USART0_IDR       (0xFFFD000C)  // USART Interrupt Disable Register

/* 
 *  cycle counter related definitions
 */ 
#define CYCLE_COUNTER_REG (0xFFFFFFC0)

/*
 *  version register
 */
#define SKYEYE_VER_REG  (0xFFFE1000)
#define DEVM_VER_REG    (0xFFFE1010)

#ifndef TOPPERS_MACRO_ONLY

/*
 * IRC operation functions
 */

/*
 * mask out the interrupt request
 */
Inline void
at91skyeye_disable_int(uint32_t mask)
{
    sil_wrw_mem((void *)(AIC_IDCR), mask);
}

/*
 * unmask the interrupt request
 */
Inline void
at91skyeye_enable_int(uint32_t mask)
{
    sil_wrw_mem((void *)(AIC_IECR), mask);
}

/*
 * clear the interrupt request
 */
Inline void
at91skyeye_clear_int(uint32_t mask)
{
    sil_wrw_mem((void *)(AIC_ICCR), mask);	
}

/*
 * probe the interrupt request
 */
Inline bool_t
at91skyeye_probe_int(uint32_t mask)
{
    return((sil_rew_mem((void *)(AIC_IPR)) & mask) == mask);
}

/*
 * exit processing
 */
Inline void
at91skyeye_exit(void)
{    
}

/* 
 *  read cycle counter
 */ 
Inline uint32_t 
cycle_counter_get_current(void)
{ 
        return sil_rew_mem((void*)(CYCLE_COUNTER_REG)); 
}

/*
 *  trace log related definition
 *  use cycle counter as the log time
 */
#define TRACE_GET_TIM()   cycle_counter_get_current()

#endif /* TOPPPERS_MACRO_ONLY */


#endif /* TOPPERS_AT91SAM7S_H */
