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
 *  @(#) $Id: core_config.c 1870 2010-07-26 13:09:18Z ertl-honda $
 */

/*
 *      core-dependent module (for ARM)
 */
#include "kernel_impl.h"
#include "check.h"
#include "task.h"

/*
 *  context reference variable
 */
uint32_t excpt_nest_count;

/*
 *  core-dependent initialize
 */
void
core_initialize()
{
    /*
	 *  when kernel is starting, the kernel is in non-task context,
	 *  so set excpt_netst_count to 1
     */ 
    excpt_nest_count = 1;    
}

/*
 *  core-dependent terminate
 */
void
core_terminate(void)
{

}

/*
 *  output the status of CPU exceptions
 *
 *  this function is called in CPU exception handlers to output the
 *  status of CPU exceptions pointed by p_excinf.
 */
#ifdef SUPPORT_XLOG_SYS

void
xlog_sys(void *p_excinf)
{    
}

#endif /* SUPPORT_XLOG_SYS */

/*
 *  install a CPU exception handler into the CPU exception vector
 */ 
void
x_install_exc(EXCNO excno, FP exchdr)
{
    *(((FP*)vector_ref_tbl) + excno) = exchdr;
}

#ifndef OMIT_DEFAULT_EXC_HANDLER
/*
 * default CPU exception handler
 */
void
default_exc_handler(void){
    syslog_0(LOG_EMERG, "Unregistered Exception occurs.");
    ext_ker();
}
#endif /* OMIT_DEFAULT_EXC_HANDLER */
