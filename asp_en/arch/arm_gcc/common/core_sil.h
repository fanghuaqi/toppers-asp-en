/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2010 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: core_sil.h 1946 2010-10-05 08:18:11Z ertl-honda $
 */

/*
 *  the core-dependent part of sil.h (for ARM)
 */

#ifndef TOPPERS_CORE_SIL_H
#define TOPPERS_CORE_SIL_H

#ifndef TOPPERS_MACRO_ONLY

#ifdef __thumb__
/*
 *  functions to operate the registers
 */

/*
 *  get the current value of status register (CPSR)
 */
extern uint32_t current_sr(void);

/*
 *  set the status register (CPSR)
 */
extern void set_sr(uint32_t sr);
#endif /* __thumb__ */

/*
 *  disable all the interrupts (FIQ and IRQ)
 */
Inline uint32_t
TOPPERS_disint(void)
{
    uint32_t  cpsr;
    uint32_t  irq_fiq_mask;

#ifndef __thumb__
    Asm("mrs  %0,CPSR" : "=r"(cpsr));
    irq_fiq_mask = cpsr & (0x40|0x80);
    cpsr |= (0x40|0x80);
    Asm("msr CPSR, %0" : : "r"(cpsr) :"memory", "cc");
#else /* __thumb__ */
    cpsr = current_sr();
    irq_fiq_mask = cpsr & (0x40|0x80);
    cpsr |= (0x40|0x80);
    set_sr(cpsr);
#endif /* __thumb__ */

    return(irq_fiq_mask);
}

/*
 *  set fiq and irq mask
 */
Inline void
TOPPERS_set_fiq_irq(uint32_t TOPPERS_irq_fiq_mask)
{
    uint32_t  cpsr;

#ifndef __thumb__
    Asm("mrs  %0,CPSR" : "=r"(cpsr));
    cpsr = cpsr & ~(0x40|0x80);
    cpsr = cpsr | (TOPPERS_irq_fiq_mask & (0x40|0x80));
    Asm("msr CPSR, %0" : : "r"(cpsr):"memory", "cc");
#else /* __thumb__ */
    cpsr = current_sr();
    cpsr = cpsr &  ~(0x40|0x80);
    cpsr = cpsr | (TOPPERS_irq_fiq_mask&  (0x40|0x80));
    set_sr(cpsr);
#endif /* __thumb__ */
}

/*
 *  lock and unlock all the interrupts
 */
#define SIL_PRE_LOC   uint32_t TOPPERS_irq_fiq_mask
#define SIL_LOC_INT() ((void)(TOPPERS_irq_fiq_mask = TOPPERS_disint()))
#define SIL_UNL_INT() (TOPPERS_set_fiq_irq(TOPPERS_irq_fiq_mask))

/*
 *  delay in nano seconds
 */
Inline void
sil_dly_nse(ulong_t dlytim)
{
    register uint32_t r0 asm("r0") = (uint32_t) dlytim;    
    Asm("bl _sil_dly_nse" : "=g"(r0) : "0"(r0) : "lr","cc");
}

#endif /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_CORE_SIL_H */
