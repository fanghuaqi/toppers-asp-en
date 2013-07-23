/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2011 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: core_config.h 1836 2010-07-09 15:31:28Z ertl-honda $
 */


/*
 *      core-dependent module (for ARM)
 *
 *  this head file should not be included directly in the files except
 *  target_config.h (or the files included in target_config.h). 
 */

#ifndef TOPPERS_CORE_CONFIG_H
#define TOPPERS_CORE_CONFIG_H

/*
 *  ARM common definitions
 */
#include "arm.h"

/*
 *  definitions of error check in pass3
 */
#define CHECK_STKSZ_ALIGN	8	/* the align of stack size */
#define CHECK_FUNC_ALIGN	4	/* the align of function address */
#define CHECK_FUNC_NONNULL		/* check whether the function address is NULL */
#define CHECK_STACK_ALIGN	4	/* the align of stack */
#define CHECK_STACK_NONNULL		/* check whether the stack is NULL */
#define CHECK_MPF_ALIGN		4	/* the align of fixed-sized memory pool */
#define CHECK_MPF_NONNULL		/* check whether the fixed-sized memory pool is NULL */
#define CHECK_MB_ALIGN		4	/* the align of management block */

/*
 *  the CPSR's value of CPU LOCK
 */
#ifndef CPSR_CPULOCK
#define CPSR_CPULOCK     (CPSR_IRQ_BIT)
#endif  /* CPSR_CPULOCK */

/*
 *  the CPSR's value of INT LOCK
 */
#define CPSR_INTLOCK     (CPSR_FIQ_BIT|CPSR_IRQ_BIT)

/*
 *  the normal value of CPSR
 */
#ifndef CPSR_ALWAYS_SET
#define CPSR_ALWAYS_SET  0x00
#endif  /* CPSR_ALWAYS_SET */

/*
 *  exception numbers
 */
#define EXCH_NO_RESET     0
#define EXCH_NO_UNDEF     1
#define EXCH_NO_SWI       2
#define EXCH_NO_PABORT    3
#define EXCH_NO_DABORT    4
#define EXCH_NO_IRQ       5
#define EXCH_NO_FIQ       6

/*
 *  the number of exceptions
 */  
#define TNUM_EXCH         7

#ifndef TOPPERS_MACRO_ONLY

/*
 *  the initial value of non-task context stack
 */
#define TOPPERS_ISTKPT(istk, istksz) ((STK_T *)((char_t *)(istk) + (istksz)))

/*
 *  the definition of core-dependent inline functions
 */
#include "core_insn.h"

/*
 *  the definition of task context block
 */
typedef struct task_context_block {
    void  *sp;       /*  stack pointer */
    FP    pc;        /*  program counter */
} TSKCTXB;


/*
 *  the implementation of TOPPERS standard interrupt processing model
 *
 *  IRQ but not FIQ is managed by the kernel.
 *
 *  In the ARM core-dependent, only the management of CPU lock is
 *  implemented, other functions, such as interrupt priority mask,
 *  interrupt disable flag are required to be implemented in the
 *  target-dependent part.
 * 
 */

/*
 *  context reference variable
 */
extern uint32_t excpt_nest_count; /* the nest count of exceptions (interrupt/CPU exception) */

/*
 *  refer the context
 *  
 *  In ARM, non-task context and task-context both are under SVC mode.
 *  The context can not be identified by CPSR. So excpt_nest_cout is
 *  used to identify the context. This variable is incremented at the
 *  entry of exceptions (interrupt/CPU exception).
 */
Inline bool_t
sense_context(void)
{
    return(excpt_nest_count > 0U);
}

/*
 *  lock the CPU
 */
Inline void
x_lock_cpu(void)
{
    set_sr(current_sr() | CPSR_CPULOCK | CPSR_ALWAYS_SET);
    /* data in memory maybe modified in critical section */
    Asm("":::"memory");
}

#define t_lock_cpu()   x_lock_cpu()
#define i_lock_cpu()   x_lock_cpu()

/*
 *  unlock the CPU
 */
Inline void
x_unlock_cpu(void)
{
    /* data in memory maybe modified in critical section */
    Asm("":::"memory");
    set_sr((current_sr() & (~CPSR_CPULOCK)) | CPSR_ALWAYS_SET);
}

#define t_unlock_cpu() x_unlock_cpu()
#define i_unlock_cpu() x_unlock_cpu()

/*
 *  refer the CPU lock state
 */
Inline bool_t
x_sense_lock(void)
{
    return((current_sr() & CPSR_CPULOCK) == CPSR_CPULOCK);    
}

#define t_sense_lock()    x_sense_lock()
#define i_sense_lock()    x_sense_lock()

/*
 *  task dispatcher
 */

/*
 *  dispatch to the task with the highest precedence (core_support.s)
 *  
 *  dispatch is called by the service calls executed in task context
 *  when CPU is locked, dispatch is enabled and interrupt priority mask
 *  is released. 
 */
extern void dispatch(void);

/*
 *  start dispatch (core_support.s)
 *
 *  start_dispatch is called at the start of the kernel when all
 *  interrupts are disabled.
 */
extern void start_dispatch(void) NoReturn;

/*
 *  exit and dispatch (core_support.s)
 *
 *  exit_and_dispatch is called by ext_tsk in task context
 *  when CPU is locked, dispatch is enabled and interrupt priority mask
 *  is released.
 */
extern void exit_and_dispatch(void) NoReturn;

/*
 *  kernel exit (core_support.s)
 *
 * 	call_exit_kernel is called when the kernel is terminated. It will
 * 	switch to the non-task context and call exit_kernel.
 */
extern void call_exit_kernel(void) NoReturn;

     
/*
 *  the initialization of task context
 *
 *  start_r is called when the task comes into the read state from
 *  dormant state. At this moment, the task stack is not used.
 *
 *  As TCB is not defined here, activate_context is implemented in macro
 *  but not inline function.
 */
extern void    start_r(void);

#define activate_context(p_tcb)                                         \
{                                                                       \
    (p_tcb)->tskctxb.sp = (void *)((char_t *)((p_tcb)->p_tinib->stk)    \
                                        + (p_tcb)->p_tinib->stksz);     \
    (p_tcb)->tskctxb.pc = (FP)start_r;                                  \
}

/*
 *  don't use calltex
 */
#define OMIT_CALLTEX
     
/*
 *  CPU exception handler table
 */
extern const FP exch_tbl[TNUM_EXCH];

/*
 * don't use the standard exception initialization routine in the
 * target-independent part.
 */
#define OMIT_INITIALIZE_EXCEPTION

/*
 * CPU exception initialize
 */
Inline void
initialize_exception(void)
{
}

/*
 *  refer the status of system when CPU exceptions happened
 */

/*
 *  refer the context when CPU exceptions happened
 *
 *  when CPU exceptions happened in task context, return false or return
 *  true.
 */
Inline bool_t
exc_sense_context(void *p_excinf)
{
    return((*((uint32_t *) p_excinf)) != 0U);
}

/*
 *  get the value of interrupt priority mask when CPU exceptions
 *  happened
 */
Inline PRI
exc_get_ipm(void *p_excinf)
{
    return((PRI)(*(((uint32_t *)(p_excinf)) + 1)));
}

/*
 *  get the value of cpsr when CPU exceptions happened
 */
Inline uint32_t
exc_get_sr(void *p_excinf){
    return(*(((uint32_t *)(p_excinf)) + 2));    
}

/*
 *  CPU lock?
 */
Inline bool_t
exc_sense_lock(void *p_excinf){
    return((exc_get_sr(p_excinf) & CPSR_CPULOCK) == CPSR_CPULOCK);
}

/*
 * interrupt lock?
 */
Inline bool_t
exc_sense_int_lock(void *p_excinf){
    return((exc_get_sr(p_excinf) & CPSR_INTLOCK) == CPSR_INTLOCK);
}

/*
 *  refer the context and interrupt priority mask when CPU exceptions
 *  happened.
 *  
 *  when CPU exceptions happened, if it is in task context, interrupts
 *  are not locked, CPU is not locked and interrupt priority mask is
 *  released, return true or return false.
 *  (when CPU exceptions happened in the interrupts that are not
 *  managed by kernel, return false).
 *
 */
Inline bool_t
exc_sense_intmask(void *p_excinf)
{
    return(!exc_sense_context(p_excinf)
             && (exc_get_ipm(p_excinf) == 0U)
               && !exc_sense_lock(p_excinf)
                 && !exc_sense_int_lock(p_excinf));
}

/*
 *  core-dependent initialize
 */
extern void core_initialize(void);

/*
 *  core-dependent terminate
 */
extern void core_terminate(void);

/*
 *  CPU exception vector reference table
 *  (start.S)
 */
extern void *vector_ref_tbl;

/*
 *  install a CPU exception handler into the CPU exception vector
 */ 
extern void x_install_exc(EXCNO excno, FP exchdr);

/*
 *  CPU exception handler
 */

/*
 *  UNDEF exception handler (core_support.s)
 */
extern void undef_handler(void);

/*
 *  SWI exception handler (core_support.s)
 */
extern void swi_handler(void);

/*
 *  PREFETCH exception handler (core_support.s)
 */
extern void prefetch_handler(void);

/*
 *  DATAABORT exception handler (core_support.s)
 */
extern void data_abort_handler(void);

/*
 *  FIQ exception handler (core_support.s)
 */
extern void fiq_handler(void);

/*
 *  default exception handler
 */
extern void default_exc_handler(void);

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_CORE_CONFIG_H */
