/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2010 by Embedded and Real-Time Systems Laboratory
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
 *   the starup module (for ARM)
 */
#define    TOPPERS_MACRO_ONLY
#define UINT_C(val)		(val)		/* macro to create uint_t constant */
#define ULONG_C(val)	(val)		/* macro to create ulong_t constant */
#include "kernel_impl.h"

    .text
    .code 32
    .align  2
    .global start
start:
    /*
     * initialize the CPU mode
     * SVC mode, IRQ/FIQ disabled
     */
    msr  cpsr, #(CPSR_SVC|CPSR_INTLOCK) 

    /*
     * initialize the stack pointer and frame pointer
 	 * need to set sl (stack top register r10)?
	 * for gcc, -mapcs is a default option, no need to set r10
     */
    ldr  r0, =_kernel_istkpt
    ldr  sp, [r0]
    mov  fp, #0        /* frame pointer for ARM mode(r11) */         
    mov  r7, #0        /* frame pointer for Thumb mode    */

    /*
     * call hardware_init_hook  (if it is not 0)
     *
	 * if necessary target-dependent hardware initialization is required,
	 * please prepare the hardware_init_hook 
     */
    ldr   r0, =hardware_init_hook
    cmp   r0, #0x00
    movne lr, pc
    bxne  r0

start_1:

#ifndef TOPPERS_OMIT_BSS_INIT
    /*
     *  clear bss section
     */        
    ldr  r1, =__bss_start
    ldr  r2, =__bss_end
    cmp  r1,r2
    bhs  start_3
    mov  r0,#0
start_2:
    str  r0,[r1],#4
    cmp  r1, r2
    blo  start_2
#endif /* TOPPERS_OMIT_BSS_INIT */
       
start_3: 

#ifndef TOPPERS_OMIT_DATA_INIT
    /*
     *  initialize the data section (if code is in ROM)
	 *  copy the data between __idata_start and __idata_end into the
	 *  area which starts from __data_start
     */
    ldr  r1, =__idata_start
    ldr  r3, =__idata_end
    cmp  r1, r3
    bhs  start_5
    ldr  r2, =__data_start
start_4:        
    ldr  r0, [r1],#4
    str  r0, [r2],#4
    cmp  r1, r3
    blo  start_4
#endif /* TOPPERS_OMIT_DATA_INIT */
	
	/*
     * call software_init_hook  (if it is not 0)
     *
	 * if necessary target-dependent software initialization is required
	 * (such as library), please prepare the software_init_hook 
     */

start_5:
    ldr   r0, =software_init_hook
    cmp   r0, #0x00
    movne lr, pc
    bxne  r0
    
    /*
     * start the kernel
     */
start_6:        
    bl   sta_ker
