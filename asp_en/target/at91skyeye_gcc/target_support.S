/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2010 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: target_support.S 1972 2010-11-28 14:43:54Z ertl-honda $
 */

/*
 *  the assemble part of chip dependent module (for AT91SKYEYE)
 */

#define  TOPPERS_MACRO_ONLY
#define UINT_C(val)		(val)		/* macro to make uint_t constant */
#define ULONG_C(val)	(val)		/* macro to make ulong_t constant */
#include "kernel_impl.h"

/*
 *  low level target hardware initialization
 *
 *  this routine is called before memory initialization at the 
 *  startup module          
 */
        .text
        .align 2
        .global hardware_init_hook
hardware_init_hook:
        bx  lr

/*
 * interrupt request handler
 *
 * call from IRQ vector
 */
    .text
    .align 2
    .global irq_handler
irq_handler:
    /* 
     * run in IRQ mode 
     */
    /* 
     *  move back to the mode before interrupt 
     * (supervisor mode), save context
     */
    msr   cpsr, #(CPSR_SVC|CPSR_IRQ_BIT) 
    stmfd sp!, {r0-r3, ip, lr, pc} /* pc is dummy */

  	/*
     *  to get the spsr and return address, switch back to irq mode
     */
    msr   cpsr, #(CPSR_IRQ|CPSR_IRQ_BIT)
    sub   r0, lr, #4
    mrs   r1, spsr

    /*
     *  switch to supversior mode (interrupt handlers run in this mode)
     */
    msr   cpsr, #(CPSR_SVC|CPSR_IRQ_BIT) 
    str   r0, [sp, #0x18] /* save return address */
    stmfd sp!, {r1}       /* save spsr */
    mov   lr, sp          /* save stack pointer */

    /*
     *  check interrupt nested ?
     */

    ldr   r2, =excpt_nest_count /* get nest count */
    ldr   r3, [r2]
    add   r0, r3, #1            /* update nest count */
    str   r0, [r2]
    cmp   r3, #0    
    bne   irq_handler_1

    /* 
     * if it is the first interrupt, change to non-task 
	 * context stack
     */
    ldr   r0, =_kernel_istkpt
    ldr   sp, [r0]

irq_handler_1:
    stmfd sp!, {lr}     /* save the original stack pointer*/

    ldr   r1, =AIC_IVR
    ldr   r3, [r1]

    /*
     *  check interrupt source
     */
    ldr   r1, =AIC_ISR
    ldr   r3, [r1]

    /*
     *  get the interrupt priority and set the corresponding interrupt priority mask.
	 *  the original interrupt priority mask will be saved in stack.
     */
    ldr   r0, =inh_ipm_tbl      /* get the interrupt priority mask table */
    ldr   r1, [r0, r3, lsl #2]  /* r1<-interrupt priority mask   */
    ldr   r0, =ipm              /* save the original interrupt priority mask */
    ldr   r2, [r0]              
    stmfd sp!,{r2}         
    str   r1, [r0]              /* set the interrupt priority mask */

    /*
     *  set the interrupt priority mask in hardware
     * 
     * set the interrupt disable flag according to the interrupt
     * priority mask.
     * 
     *
     */
    rsb   r1, r1,  #0           /* reverse the index */
    ldr   r0, =ipm_mask_tbl     /* get the mask table  */
    ldr   r2, [r0, r1, lsl #2]  /* get the corresponding interrupt disable bits */    
    ldr   r0, =idf              /* get the original interrupt disable flag */
    ldr   r1, [r0]
    /*
     * set the corresponding interrupt disable bits in hardware
     */
    mvn   lr, #0                
    ldr   r0, =AIC_IDCR			/* interrupt disable register */
    str   lr, [r0]				/* disable all first */
    orr   r1, r1, r2            /* enable the interrupts that are not masked */
    mvn   r1, r1                /* generate the value of interrupt enable register */    
    ldr   r0, =AIC_IECR
    str   r1, [r0]

    /*
     * get the entry address of interrupt handler
     */
    ldr   r0, =inh_tbl         /* read from interrupt handers table  */
    ldr   r0, [r0, r3, lsl #2] /* r0<-the entry address of interrupt handler */    

    /*
     *  save for EOI
     */
    mov   r1, #1
    mov   r1, r1, lsl r3
    stmfd sp!, {r1} 

    stmfd sp!,{r3}       /* save inhno  */

    /* 
     * enable interrupt 
     */         
    msr   cpsr, #(CPSR_SVC)

#ifdef LOG_INH_ENTER
    stmfd sp!,{r0}
    mov   r0, r3         /* inhno is the parameter for log_inh_enter */
    bl    log_inh_enter  /* call log_inh_enter */
    ldmfd sp!,{r0}
#endif /* LOG_INH_ENTER */

    /* 
     * call interrupt handler
     */
    mov   lr, pc
    bx    r0

    ldmfd sp!,{r0}       /* recover inhno  */    
#ifdef LOG_INH_LEAVE
    bl    log_inh_leave  /* call log_inh_leave */
#endif /* LOG_INH_LEAVE */

    /*
     * disable the interrupts managed by the kernel
     */
    msr   cpsr, #(CPSR_SVC|CPSR_IRQ_BIT)

    /*
     * clear interrupt
     */
    ldmfd   sp!,{r0}
    ldr     r3, =AIC_EOI
    str     r0, [r3]

    b     target_ret_int


/*
 *
 * target dependent part of cpu exception handler
 *
 */
    .text
    .global target_exc_handler
target_exc_handler:    
    /*
     *  switch to the mode where cpu exception handlers run (supervisor
     *  mode). the interrupt lock and cpu lock are the same before cpu
     *  exception happens.
	 *  the contents of regs at this moment are as follow:
     *    r0 : lr(return address)
     *    r1 : spsr
     *    r2 : exception no
     */
    msr   cpsr, #(CPSR_SVC|CPSR_FIQ_BIT|CPSR_IRQ_BIT) 
    str   r0, [sp, #0x18] /* save the return address */
    stmfd sp!, {r1}       /* save spsr */    
    mov   lr, sp          /* get the current stack pointer */

    ldr   r0, =ipm        /* save the original ipm into stack */
    ldr   r3, [r0]              
    stmfd sp!, {r3}

    /* 
     * save excpt_nest_count into stack for context check.
	 * saving excpt_nest_count here is also to make the debug 
	 * more convinient 
     */
    ldr   r0, =excpt_nest_count
    ldr   r3, [r0]
    stmfd sp!, {r3}

    mov   r3,  sp         /* save exception frame address */

    /*
     * check whether the happened cpu exception is outside the
     * management of the kernel 
     * 
     * the cpu exception outside kernel management happened in kernel or 
	 * in interrupt lock state or in cpu lock state or in the processing
	 * of interrupt which are outside kernel management. for ARM, if one
	 * of the bits of I/F in spsr is 1, it's the cpu exception outside
	 * kernel management.
     */
    tst   r1, #(CPSR_FIQ_BIT|CPSR_IRQ_BIT) 
    bne   target_kernel_unc_exc_handler 
	/* jump to the processing of cpu exception outside kernel management */

    /*
     * check nest count
     */
    ldr   r0, =excpt_nest_count
    ldr   r1, [r0]
    add   r1, r1, #1
    str   r1, [r0]
    cmp   r1, #1
    bne   target_exc_handler_1    

    /* 
     * change to the non-task context stack if cpu exception
	 * happens in task context.
     */
    ldr  r0, =_kernel_istkpt
    ldr  sp, [r0]

target_exc_handler_1:    
    stmfd sp!, {lr}      /* save the original stack pointer */

    /* 
     * save the original ipm as it's shared by the corresponding handler
     * and exit processing.  
     */
    ldr   r0, =ipm       
    ldr   r1, [r0]              
    stmfd sp!, {r1}

    /*
     *  get the address of cpu exception handler
     */
    ldr   r0, =exch_tbl        /* read from cpu exception handers table  */
    ldr   r1, [r0, r2, lsl #2] /* r1<-entry address of cpu exception handler */

    stmfd sp!,{r2}      /* save excno */

    /* 
     * enable interrupts
     * as cpu exception will call other routines, re-enable interrupts
     */
    msr   cpsr, #(CPSR_SVC)
          
#ifdef LOG_EXC_ENTER
    stmfd sp!,{r1,r3}
    mov   r0, r2         /* excno is the parameter    */
    bl    log_exc_enter  /* call log_exc_enter */
    ldmfd sp!,{r1,r3}
#endif /* LOG_EXC_ENTER */

    /* 
     * call cpu exception handler
     * exception frame is the parameter
     */        
    mov   r0, r3
    mov   lr, pc
    bx    r1

    ldmfd sp!,{r0}       /* recover  */    
#ifdef LOG_EXC_LEAVE
    bl    log_exc_leave  /* call log_exc_leave */
#endif /* LOG_EXC_LEAVE */


/*
 *
 * the target dependent exit processing of cpu exception
 *
 */
    .global target_ret_exc
target_ret_exc:
    /*
     * disable the interrupts managed by the kerenl
     */
    msr   cpsr, #(CPSR_SVC|CPSR_IRQ_BIT)  

    .global target_ret_int        
target_ret_int: 
    /*
     * recover the original ipm
     */
    ldmfd sp!, {r1}             /* get the original ipm */
    ldr   r0, =ipm              /* recover the ipm    */
    str   r1, [r0]
    rsb   r1, r1,  #0           /* reverse the index  */
    ldr   r0, =ipm_mask_tbl     /* get the mask table */
    ldr   r2, [r0, r1, lsl #2]  /* get the corresponding interrupt disable bits */    
    ldr   r0, =idf              /* get the original interrupt disable flag */
    ldr   r1, [r0]
    
	/*
     * set the corresponding interrupt disable bits in hardware
     */
    mvn   lr, #0                
    ldr   r0, =AIC_IDCR
    str   lr, [r0]
    orr   r1, r1, r2            /* enable the interrupts that are not masked */
    mvn   r1, r1                /* generate the value of interrupt enable register */    
    ldr   r0, =AIC_IECR
    str   r1, [r0]

    /*
     * recover the stack pointer
     */
    ldmfd sp!, {r2}             /* get the original stack pointer */
    mov   sp, r2
    
    /*
     * call the common processing of ARM
     */            
    b     ret_int


/*
 *
 * the entry and exit of the cpu exception outside the management of the
 * kernel
 *
 */
target_kernel_unc_exc_handler:
    /*
     * check the context when cpu exception happens
     */
    ldr   r0, =excpt_nest_count
    ldr   r1, [r0]
    add   r1, r1, #1
    str   r1, [r0]
    cmp   r1, #1
    bne   target_kernel_unc_exc_handler_1
    
    /* 
     * change to the non-task context stack if cpu exception
	 * happens in task context.
     */
    ldr  r0, =_kernel_istkpt
    ldr  sp, [r0]

target_kernel_unc_exc_handler_1:
    stmfd sp!, {lr}     /* save the original stack pointer */
    
    /*
     *  get the entry address of cpu exception handler
     */
    ldr   r0, =exch_tbl        /* read from cpu exception handers table */
    ldr   r1, [r0, r2, lsl #2] /* r1<-entry address of cpu exception handler */

   /*
    * move back to the original state except the context
    */
    ldr   r0, [lr]             /* get the CPSR befor cpu exception */
    and   r0, r0, #(CPSR_IRQ_BIT|CPSR_FIQ_BIT)
    orr   r0, r0, #(CPSR_SVC)
    msr   cpsr, r0

    /* 
     * call cpu excetpion handler 
     * the exception frame is the parameter
     */        
    mov   r0, r3
    mov   lr, pc
    mov   pc, r1

    /*
     *  decrease the nest count
     */        
    ldr   r0, =excpt_nest_count   /* r0 <-excpt_nest_count */
    ldr   r1, [r0]
    sub   r2, r1, #1
    str   r2, [r0]

    /*
     * recover the stack pointer
     */
    ldmfd sp!, {r2}             /* get the original stack pointer */
    mov   sp, r2

    /*
     * return from cpu exception
     */
    ldmfd sp!,{r1}              /* recover CPSR */
    msr   spsr, r1              /* set cpsr as spsr */
    ldmfd sp!,{r0-r3,ip,lr,pc}^ /* recover the context, cpsr <- spsr */    
