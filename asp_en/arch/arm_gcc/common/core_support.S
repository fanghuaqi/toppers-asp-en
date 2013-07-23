/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: core_support.S 1973 2010-11-29 15:41:15Z ertl-honda $
 */

/*
 *        core-dependent part in assemble language (for ARM)
 */

#define TOPPERS_MACRO_ONLY
#define TOPPERS_ASM_MACRO
#define UINT_C(val)		(val)		/* macro to create uint_t constant */
#define ULONG_C(val)	(val)		/* macro to create ulong_t constant */
#include "kernel_impl.h"
#include "offset.h"

/*
 * exception vector
 */
    .section .vector,"a"
    .global vector_table
vector_table:
    ldr pc, reset_vector       /* RESET		*/
    ldr pc, undef_vector       /* UNDEF		*/
    ldr pc, swi_vector         /* SWI		*/
    ldr pc, prefech_vector     /* PREFETCH ABORT	*/
    ldr pc, data_abort_vector  /* DATA ABORT   		*/
    ldr pc, reset_vector	   /* reserved 			*/
    ldr pc, irq_vector         /* IRQ             	*/
    ldr pc, fiq_vector         /* FIQ             	*/


/*
 *  vector reference table, referred by instructions in exception vector
 */
    .global vector_ref_tbl
vector_ref_tbl:
reset_vector:
    .long   start
undef_vector:
    .long   undef_handler
swi_vector:
    .long   swi_handler
prefech_vector:
    .long   prefetch_handler
data_abort_vector:
    .long   data_abort_handler
irq_vector:
    .long   irq_handler
fiq_vector:
    .long   fiq_handler

/*
 *  task dispatcher
 *
 */
    .text
    .align 2
    .globl dispatch
dispatch:
    /*  
	 *  the pre-conditions of this routine are task context, CPU is
	 *  locked, dispatch is enabled, and all interrupt priorities are
	 *  unmasked.
	 *		
     */
    stmfd sp!, {r4 - r11,lr}           /* save registers */
    ldr   r0, =p_runtsk                /* read p_runtsk */
    ldr   r1, [r0]
    str   sp, [r1,#TCB_sp]             /* save stack pointer */
    adr   r2, dispatch_r
    str   r2, [r1,#TCB_pc]             /* save return address */
    b     dispatcher

dispatch_r:
    ldmfd sp!,{r4 - r11,lr}     /* recover registers */
    /*
     * whether to call the task exception routine
	 * as it is jumped from dispatch, TCB is already in r1
     */
    ldrb  r0,[r1,#TCB_enatex]
    tst   r0,#TCB_enatex_mask
    beq   dispatch_r_1          /* if enatex is false, return */
    ldr   r0,[r1,#TCB_texptn]   /* if texptn is 0, return */
    tst   r0,r0                 
    beq   dispatch_r_1          
    ldr   r1, =ipmflg           /* if ipmflg is false, return */
    ldr   r0, [r1]
    tst   r0,r0
    bne   call_texrtn           /* call the task exception routine */
dispatch_r_1:
    bx    lr


/*
 *  start dispatch (cpu_support.S)
 */
    .globl start_dispatch
start_dispatch:
    /*  
     *  this routine is called in the non-task conext during the startup of the kernel
	 *  , and all the interrupts are locked.
     *
     *  when the dispatcher is called, the cpu is locked, all interrupt
     *  priority mask are cleared, no nest exception (CPU exception/interrupt).
	 *  In target_initialize, all interrupt priority mask should be cleared, cpu should be
	 *  locked, the interrupts outside the kernel such as fiq can be
	 *  enabled. As the disdsp is initialized to be false, task dispatch
	 *  is enabled.
     *  
     */    
    msr   cpsr, #(CPSR_SVC|CPSR_CPULOCK|CPSR_ALWAYS_SET) /* lock cpu */
    ldr   r2, =excpt_nest_count            /* set the exception nest count to 0 */
    mov   r0, #0                           
    str   r0, [r2]
    b     dispatcher_0

/*
 *  exit and dispatch
 */
    .globl exit_and_dispatch
exit_and_dispatch:
    /*  go to dispatcher */    

/*
 *  dispatcher
 */
dispatcher:
    /*
     *  before dispatcher is called, task context | cpu locked | dispatch enabled | all interrupt priority masks are cleared 
	 *  should be satisfied. In this routine, the processor will jump
	 *  into the entry of next to run task
     *
     *  i.e. supervisor mode, IRQ disabled, disdsp is false, dspflg
     *  is true.
     */
#ifdef LOG_DSP_ENTER
    ldr   r1, =p_runtsk     /* p_runtsk is the parameter of log_dsp_enter */
    ldr   r0, [r1]        
    bl    log_dsp_enter
#endif /* LOG_DSP_ENTER */
dispatcher_0:
    ldr   r0, =p_schedtsk   /* p_runtsk = p_schedtsk */
    ldr   r1, [r0]
    ldr   r2, =p_runtsk   
    str   r1, [r2]        
    cmp   r1, #0            /* if p_runtsk is NULL, jump to dispatcher_1 */
    beq   dispatcher_1             
    ldr   sp, [r1,#TCB_sp]  /* recover task stack */
#ifdef LOG_DSP_LEAVE
    mov   r0, r1           /* p_runtsk is the parameter of log_dsp_leave */
    mov   r4, r1            /* as r1 is scratch register, save it */
    bl    log_dsp_leave
    mov   r1, r4
#endif /* LOG_DSP_LEAVE */
    ldr   r4, [r1,#TCB_pc]  /* get return address  */    
    bx    r4
dispatcher_1:
    /*
     * unlock the CPU, and be ready to switch to non-task context
     */
    ldr   r0, =_kernel_istk            /* switch to non-task context */
    ldr   sp, [r0]    
    ldr   r1, =_kernel_istksz
    ldr   r3, [r1]
    add   sp, sp, r3
    ldr   r2, =reqflg                  /* r2 <- reqflg */
    ldr   r3, =excpt_nest_count        /* r3 <-excpt_nest_count */
    mov   r0, #0
    mov   r1, #1
dispatcher_2:
    /*
     *  enable all interrupts, run in non-task context to wait for
     *  interrupts.
     *  
	 *  the switch to no-task context one hand is to solve the problem
	 *  of which stack the interrupt processing should use, the other
	 *  hand is to prevent task dispatch in the interrupt handlers.  
     *  
	 *  the processings of enable interrupts and wait for interrupts
	 *  must be atomic. If not, at the moment that interrupts are
	 *  enabled, the interrupts may come out, no matter whether a task
	 *  dispatch request exists, the processor will wait for interrupts 
	 *  after return. So even if a task dispatch request exists, no real
	 *  task dispatch will happen.
     *
     *  when the processor is waiting for interrupts, p_runtsk must be
     *  NULL (=0). If not, when iget_tid is called in interrupt
     *  handlers, there is a conflict.
     *
     *  dependent on target, the processor may get into low power mode
     *  when it is waiting for interrupts. If TOPPERS_CUSTOM_IDLE is
     *  defined,  the assemble macro, toppers_asm_custom_idle, defined
     *  by the target will be called. In this macro, r0, r1, r2, r3, sp
     *  cannot be used.
     *
     */
    str   r1, [r3]                          /* excpt_nest_count = 1 */

#ifdef TOPPERS_CUSTOM_IDLE
    toppers_asm_custom_idle
#else
    msr   cpsr, #(CPSR_SVC|CPSR_ALWAYS_SET) /* enable interrupts (supervisor mode) */
    nop
    msr   cpsr, #(CPSR_SVC|CPSR_CPULOCK|CPSR_ALWAYS_SET) /* lock cpu (supervisor mode) */
#endif /* TOPPERS_CUSTOM_IDLE */

    ldr   r6, [r2]        /* if reqflg is false, jump to dispatcher_2 */
    cmp   r6, #0          
    beq   dispatcher_2                 
    str   r0, [r2]        /* reqflg = 0 */
    str   r0, [r3]        /* excpt_nest_count = 0 */
    b     dispatcher_0


/*
 *  kernel exit routine
 *
 *  switch to the mode and stack of no-task context．
 */
    .globl call_exit_kernel
call_exit_kernel:
    msr  cpsr, #(CPSR_SVC|CPSR_ALWAYS_SET|CPSR_IRQ_BIT|CPSR_FIQ_BIT) 
    ldr  r0, =_kernel_istkpt       /* switch no-task context stack */
    ldr  sp, [r0]
    b     exit_kernel


/*
 *  task startup routine
 *
 *  As this routine is called in dispatcher，TCB address is r1
 */
    .text
    .globl start_r
start_r:
    msr   cpsr, #(CPSR_SVC|CPSR_ALWAYS_SET) /* unlock the cpu */
    ldr   lr, =ext_tsk                      /* set return address */
    ldr   r2, [r1, #TCB_p_tinib]            /* load p_runtsk->p_tinib into r2  */
    ldr   r0, [r2, #TINIB_exinf]            /* load exinf into r0  */
    ldr   r1, [r2, #TINIB_task]             /* jump to task entry */
    bx    r1 


/*
 *  the exit processing of interrupt handlers and cpu exception handlers
 *
 *  ret_int is called when the processor returns from interrupt handlers
 *  and cpu exception handlers.
 *  After interrupt handlers are called and the target defined processing 
 *  is done, disable all the interrupts managed by the kernel, recover
 *  the stack before interrupt, ret_int is called. 
 *
 */
    .text
    .global ret_int
    .global ret_exc
ret_int:
ret_exc:
    /*
     *  decrease the nest count of interrupt processing and exception
     *  processing
     */        
    ldr   r0, =excpt_nest_count   /* r0 <-excpt_nest_count */
    ldr   r1, [r0]
    sub   r2, r1, #1
    str   r2, [r0]
    cmp   r2, #0                  /* if the return context is non-task context */
    bne   ret_int_1               /* return directly                   */

    /*
     *  the interrupts are disabled before reqflg is checked. If not
     *  ,  a task dispatch request may come out in the interrupt
     *  handler, the task dispatch will not happen immediately.
     */
    ldr   r0, =reqflg
    ldr   r1, [r0]
    cmp   r1, #0                  /* if reqflg is true, jume to ret_int_2 */
    bne   ret_int_2
ret_int_1:
    /*
     *  return from interrupt processing，CPU should be unlocked
     *  As the IRQ bit of CPSR is used as the CPU lock bit in ARM, 
     *  the processor can return directly here.
     */         
    ldmfd sp!,{r1}              /* recover CPSR */
    msr   spsr, r1              /* set spsr  */
    ldmfd sp!,{r0-r3,ip,lr,pc}^ /* recover the context and cpsr <- spsr */

    
ret_int_2:
ret_int_3:
    /*
     *
   	 *  If the return context is task context, the scratch regs will be
   	 *  saved in task stack. The processor will be in supervisor mode
   	 *  and all the interrupts managed by the kernel will be disabled.
     */    
    ldr   r0, =reqflg    /* reqflg = false*/
    mov   r1, #0
    str   r1, [r0]

    /*
     *  lock cpu, the interrupt priority mask before interrupts will be
     *  recovered.
     *
     *  As dispatched or call_texrtn may be called, cpu must be locked.
     *
     *  As the IRQ bit of CPSR is used as the CPU lock bit in ARM,
     *  nothing to do here, 
     *
     *  As interrupt priority mask is different for different targets,
     *  the interrupt priority mask before interrupts should be
     *  recovered in the target defined processing after interrupt 
	 *  handlers is called
     */
    
    /*
     *  if dspflg is false or p_runtsk is the same as p_schedtsk, task 
	 *  dispatch will not happen.  Because reqflg may be set true even
	 *  if no task dispatch is necessary when task exception routine is
	 *  called, the check here is necessary.
	 *
     */
    ldr   r0, =p_runtsk       /* load p_runtsk into r1 */
    ldr   r1, [r0]
    ldr   r0, =dspflg
    ldr   r2, [r0]
    cmp   r2, #0              /* if dspflg is false, jump to ret_int_4 */
    beq   ret_int_4
    ldr   r0, =p_schedtsk     /* load p_schedtsk into r2 */
    ldr   r2, [r0]
    cmp   r1, r2              /* if p_runtsk is the same as p_schedtsk */
    beq   ret_int_4           /*  jump to ret_int_4 */
    stmfd sp!, {r4-r11}       /* save the rest regs */
    str   sp, [r1,#TCB_sp]    /* save task stack */
    adr   r0, ret_int_r       /* save return address */
    str   r0, [r1,#TCB_pc]
    b     dispatcher

ret_int_r: 
    ldmfd sp!, {r4-r11}       /* recover regs */    
ret_int_4:
    /*
     *  if enatex is true, texptn is not 0, and ipmflg is true, task
     *  exception routine will be called.
     *  as the code here is called in dispatcher，TCB's address is in r1.
     */
    ldrb  r0, [r1,#TCB_enatex]
    tst   r0, #TCB_enatex_mask
    beq   ret_int_5              /* if enatex is false, jump to  ret_int5 */
    ldr   r0, [r1,#TCB_texptn]   /* load texptn                */
    tst   r0, r0                 /* if texptn is 0, jump to ret_int5 */
    beq   ret_int_5
    ldr   r1, =ipmflg           /* if ipmflg is false, jump to ret_int5 */
    ldr   r0, [r1]
    tst   r0,r0
    blne  call_texrtn            /* call task exception routine   */
ret_int_5:
    /*
     *  return from interrupt processing，CPU should be unlocked
     *  As the IRQ bit of CPSR is used as the CPU lock bit in ARM, 
     *  the processor can return directly here.
     */        
    ldmfd sp!, {r0}             /* recover spsr */
    msr   spsr,r0               /* set spsr */
    ldmfd sp!,{r0-r3,ip,lr,pc}^ /* return to task, cpsr <- spsr */    
    

/*
 * CPU exception handlers
 *
 * CPU exception handers run in non-task context
 * 
 */

/*
 *  UNDEF exception handler
 */
    .text
    .align 2
    .global undef_handler
undef_handler:
    /* 
     *  switch to the mode where task is running (supervisor mode)
     */
    msr   cpsr, #(CPSR_SVC|CPSR_CPULOCK|CPSR_ALWAYS_SET) 
    stmfd sp!, {r0-r3,ip,lr,pc} /* pc is dummy  */

   	/*
     *  to get the spsr and return address, switch back to undef mode
     */
    msr   cpsr, #(CPSR_UND|CPSR_CPULOCK|CPSR_ALWAYS_SET)
    mov   r0, lr
    mrs   r1, spsr    
    mov   r2, #EXCH_NO_UNDEF
    b     target_exc_handler


/*
 *  SWI exception handler
 */
    .text
    .align 2
    .global swi_handler
swi_handler:
    /* 
     *  switch to the mode where task is running (supervisor mode)
     */
    msr   cpsr, #(CPSR_SVC|CPSR_CPULOCK|CPSR_ALWAYS_SET) 
    stmfd sp!, {r0-r3,ip,lr,pc} /* pc is dummy */
    mov   r0, lr
    mrs   r1, spsr    
    mov   r2, #EXCH_NO_SWI
    b     target_exc_handler


/*
 *  PREFETCH ABORT exception handler
 */
    .text
    .align 2
    .global prefetch_handler
prefetch_handler:
    /* 
     *  switch to the mode where task is running (supervisor mode)
     */
    msr   cpsr, #(CPSR_SVC|CPSR_CPULOCK|CPSR_ALWAYS_SET) 
    stmfd sp!, {r0-r3,ip,lr,pc} /* pc is dummy */

   	/*
     *  to get the spsr and return address, switch back to prefetch
     *  abort mode
     */
    msr   cpsr, #(CPSR_ABT|CPSR_CPULOCK|CPSR_ALWAYS_SET)
    mov   r0, lr
    mrs   r1, spsr    
    mov   r2, #EXCH_NO_PABORT
    b     target_exc_handler


/*
 *  DATA ABORT exception handler
 */
    .text
    .align 2
    .global data_abort_handler
data_abort_handler:
    /* 
     *  switch to the mode where task is running (supervisor mode)
     */
    msr   cpsr, #(CPSR_SVC|CPSR_CPULOCK|CPSR_ALWAYS_SET) 
    stmfd sp!, {r0-r3,ip,lr,pc} /* pc is dummy */

    /*
     *  to get the spsr and return address, switch back to data abort mode
     */
    msr   cpsr, #(CPSR_ABT|CPSR_CPULOCK|CPSR_ALWAYS_SET)
    mov   r0, lr
    mrs   r1, spsr    
    mov   r2, #EXCH_NO_DABORT
    b     target_exc_handler


#ifndef TARGET_FIQ_HANDLER
/*
 *  FIQ exception handler
 */
    .text
    .align 2
    .global fiq_handler
fiq_handler:
    /* 
     *  switch to the mode where task is running (supervisor mode)
     */
    msr   cpsr, #(CPSR_SVC|CPSR_FIQ_BIT|CPSR_CPULOCK|CPSR_ALWAYS_SET) 
    stmfd sp!, {r0-r3,ip,lr,pc} /* pc is dummy */

    /*
     *  to get the spsr and return address, switch back to FIQ mode
     */
    msr   cpsr, #(CPSR_FIQ|CPSR_FIQ_BIT|CPSR_CPULOCK|CPSR_ALWAYS_SET)
    mov   r0, lr
    mrs   r1, spsr    
    mov   r2, #EXCH_NO_FIQ
    b     target_exc_handler
#endif /* TARGET_FIQ_HANDLER */

/*
 *  delay in nano seconds
 */
    .globl _sil_dly_nse
_sil_dly_nse:
    sub   r0, r0, #SIL_DLY_TIM1
    cmp   r0, #0
    bgt   _sil_dly_nse1
    bxle  lr
_sil_dly_nse1:
    sub   r0, r0, #SIL_DLY_TIM2
    cmp   r0, #0
    bgt   _sil_dly_nse1
    bxle  lr

#ifdef __thumb__
    .text
    .align 2
    .global current_sr
current_sr:
    mrs   r0, cpsr
    bx    lr

   .global set_sr
set_sr:
    msr   cpsr, r0
    bx    lr
#endif /* __thumb__ */
