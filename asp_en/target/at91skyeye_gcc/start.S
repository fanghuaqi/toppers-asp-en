/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2009 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: start.S 1867 2010-07-26 08:25:10Z ertl-honda $
 */
                
/*
 *   start up module for kernel (for ARM)
 */
#define    TOPPERS_MACRO_ONLY
#define UINT_C(val)		(val)	/* the macro to make uint_t constant */
#define ULONG_C(val)	(val)	/* the macro to make ulong_t constant */
#include "kernel_impl.h"

    .text
    .code 32
    .align  2
    .global start
start:
    /*
     * initialize processor mode 
     * supervisor mode¡¤FIQ/IRQ disabled
     */
    msr  cpsr, #(CPSR_SVC|CPSR_INTLOCK) 

    /*
     * initialize frame pointer and stack pointer
     * Is it necessary to initialize the stack limit(sl, r10) register?
     * As -mapcs is the default option in gcc, no necessary.
     */
    ldr  r0, =_kernel_istkpt
    ldr  sp, [r0]
    mov  fp, #0        /* frame pointer in ARM mode (r11) */         
    mov  r7, #0        /* frmae pointer in THUMB mode     */

    /*
     * call hardware_init_hook (if it is not 0)
     *
     * if is necessary to do some target dependent hardware
     * initialization, use hardware_init_hook.
     */
    ldr   r0, =hardware_init_hook
    cmp   r0, #0x00
    movne lr, pc
    bxne  r0

    /*
     * call the initialization routine provided by the tool chain.
     */
    b __cs3_start_c
