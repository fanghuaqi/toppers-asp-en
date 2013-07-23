/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2006 by GJ Business Division RICOH COMPANY,LTD. JAPAN
 *  Copyright (C) 2007-2011 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: target_config.c 2036 2011-03-03 15:11:14Z ertl-honda $
 */

/*
 * chip dependent module (for AT91SKYEYE)
 */

#include "kernel_impl.h"
#include <sil.h>
#include "target_serial.h"
#include <stdlib.h>

#ifdef TOPPERS_ENABLE_TRACE
#include <stdio.h>
static FILE *trace_log_file;
#endif /* TOPPERS_ENABLE_TRACE */

/*
 * disable flag bits for all interrupts
 */
uint32_t idf;

/*
 * current interrupt priority mask
 */
PRI ipm;
 
/*
 *  the bit pattern to check whether interrupt attribute is configed
 */
uint32_t bitpat_cfgint;

/*
 *  output string (for version check)
 */
static void
target_fput_str(char *c) {
	while(*c != '\0') {
		at91skyeye_putc(*c++);
	}
}

/*
 *  from num to char (for 4bit)
 */
static char
num_to_char(uint8_t num) {
	if (num <= 9) {
		return ('0' + num);
	}
	else {
		return ('a' + (num - 10));
	}
}

/*
 *  output version number
 */
static void
version_put(uint16_t version) {
	char c;

	c = num_to_char((version >> 12) & 0x0f);
	at91skyeye_putc(c);
	at91skyeye_putc('.');
	c = num_to_char((version >> 8) & 0x0f);
	at91skyeye_putc(c);
	c = num_to_char((version >> 4) & 0x0f);
	at91skyeye_putc(c);
	at91skyeye_putc('.');
	c = num_to_char((version) & 0x0f);
	at91skyeye_putc(c);
	at91skyeye_putc('.');
}

/*
 *  version check
 */
static void
version_check(void) {

	/* check SkyeEye version */
	if (sil_rew_mem((void *)(SKYEYE_VER_REG)) != SUPPORT_SKYEYE_VER) {
		target_fput_str("SkyEye version is mismatch!!");
		at91skyeye_putc('\n');
		target_fput_str("Suppoted version is ");
		version_put(SUPPORT_SKYEYE_VER);
		at91skyeye_putc('\n');
		target_fput_str("Tool version is ");
		version_put(sil_rew_mem((void *)(SKYEYE_VER_REG)));
		at91skyeye_putc('\n');
		target_fput_str("Kernel Exit...");
		at91skyeye_putc('\n');
		target_exit();
	}

	/* check DeviceManger Extension's version */
	if ((sil_rew_mem((void *)(DEVM_VER_REG)) & 0xfff0)
		!= (SUPPORT_DEVM_VER & 0xfff0)) {
		target_fput_str("DeviceManager Extension version is mismatch!!");
		at91skyeye_putc('\n');
		target_fput_str("Suppoted version is ");
		version_put(SUPPORT_DEVM_VER);
		at91skyeye_putc('\n');
		target_fput_str("Tool version is ");
		version_put((sil_rew_mem((void *)(DEVM_VER_REG)) & 0xfff0));
		at91skyeye_putc('\n');
		target_fput_str("Kernel Exit...");
		at91skyeye_putc('\n');
		target_exit();
	}
}

/*
 *  target dependent initialization
 */
void
target_initialize(void)
{
	/*
	 *  to use target_fput_log, initialize UART
	 */
	at91skyeye_init_uart();

	/*
	 *  version check
	 */
	//version_check();

	/*
	 *  core dependent initialization
	 */
	core_initialize();
    
	/*
	 * set all disable flag bits for all interrupts
	 */
	idf = ~0U;

	/*
	 * current interrupt priority mask is 0
	 */ 
	ipm = 0U;

	/*
	 * mask out all interrupts
	 */ 
	at91skyeye_disable_int(~0U);

	/*
	 * clear all interrupts
	 */
	at91skyeye_clear_int(~0U);

	/*
	 *  initialize the bit pattern to check whether 
	 *  interrupt attribute is configed
	 */
	bitpat_cfgint = 0U;

#ifdef TOPPERS_ENABLE_TRACE
	trace_log_file = fopen("kernel_trace.log","w");
#endif /* TOPPERS_ENABLE_TRACE */
}

/*
 *  target dependent exit processing
 */
void
target_exit(void)
{
	extern void    software_term_hook(void);
	void (*volatile fp)(void) = software_term_hook;

	/*
	 *  use volatile to omit optimization.
	 */
	if (fp != 0) {
		(*fp)();
	}

	/*
	 *  mask out all interrupts
	 */
	at91skyeye_disable_int(~0U);

	/*
	 *  core dependent termination processing
	 */
	core_terminate();

	/*
	 *  tool chain dependent termination processing
	 */
	at91skyeye_exit();

#ifdef TOPPERS_ENABLE_TRACE
	fclose(trace_log_file);
#endif /* TOPPERS_ENABLE_TRACE */

#ifdef TOPPERS_ENABLE_GCOV_PART
	/*
	 * If GCOV PART is enabled  call _exit not to output .gcda
	 * 呼び出す．
	 */
	extern void _exit(int) NoReturn;;
	_exit(0);
#else /* TOPPERS_ENABLE_GCOV_FULL */
	exit(0);
#endif /* TOPPERS_ENABLE_GCOV_PART */
}

/*
 *  the low level output for syslog
 */
void
target_fput_log(char_t c)
{
	if (c == '\n') {
		at91skyeye_putc('\r');
	}
	at91skyeye_putc(c);
}

#ifdef TOPPERS_ENABLE_TRACE
/*
 *  output the trace log file
 */
void
target_fput_log_file(char_t c)
{
	if (c == '\n') {
		fputc('\r', trace_log_file);
	}
	fputc(c, trace_log_file);
}
#endif /* TOPPERS_ENABLE_TRACE */

/*
 *  config interrupt attribute
 *
 *  for ASP, use assert to report parameter error.
 *  for FI4 kernel, the error should be returned.
 *
 */
void
x_config_int(INTNO intno, ATR intatr, PRI intpri)
{
	assert(VALID_INTNO(intno));
	assert(TMIN_INTPRI <= intpri && intpri <= TMAX_INTPRI);

	/*
	 * set the bit pattern to check whether interrupt 
	 * attribute is configed
	 */
	bitpat_cfgint |= INTNO_BITPAT(intno);
    
	/* 
	 * disable the interrupt
	 */    
	x_disable_int(intno);

	if ((intatr & TA_ENAINT) != 0U){
		(void)x_enable_int(intno);
	}    
}

#ifndef OMIT_DEFAULT_INT_HANDLER
/*
 * default interrupt handler
 */
void
default_int_handler(void){
	syslog_0(LOG_EMERG, "Unregistered Interrupt occurs.");
	target_exit();
}
#endif /* OMIT_DEFAULT_INT_HANDLER */

/*
 *  the main entry for the initialization routine provided by the tool
 *  chain. sta_ker() will be called to start the kernel. 
 */
int
main(void)
{
	extern void software_init_hook(void);
	void (*volatile fp)(void) = software_init_hook;

	/*
	 *  call software_init_hook if not 0.
	 *
	 *  If it's necessary to do some software enviroment initialization,
	 *  such as libray, use software_init_hook.
	 *
     *  use volatile to omit optimization.
	 */
	if (fp != 0) {
		(*fp)();
	}

	/*
	 * start kernel
	 */
	sta_ker();

	return 1;
}

#ifdef TOPPERS_ENABLE_GCOV_PART
/*
 *  library functions for GCOV
 */

extern void __gcov_flush();

/*
 *  initialize GCOV 
 *  output the coverage data into .gcda. remove .gcda in SkyEye
 */
void
gcov_init(void)
{
	SIL_PRE_LOC;

	SIL_LOC_INT();
	__gcov_flush();
	sil_wrw_mem((void*)0xFFFE1020, 0);
	SIL_UNL_INT();
}

/*
 *  GCOV pause
 *  output the coverage data into .gcda, 
 *  in SkyEye, *.gcda is renamed as *.gcda.bak.
 */
void
gcov_pause(void)
{
	SIL_PRE_LOC;

	SIL_LOC_INT();
	__gcov_flush();
	sil_wrw_mem((void*)0xFFFE1024, 0);
	SIL_UNL_INT();
}

/*
 *  GCOV resume
 *  output the coverage data into .gcda, 
 *  in SkyEye, *.gcda.bak is renamed as *.gcda.
 */
void
gcov_resume(void)
{
	SIL_PRE_LOC;

	SIL_LOC_INT();
	__gcov_flush();
	sil_wrw_mem((void*)0xFFFE1028, 0);
	SIL_UNL_INT();
}

/*
 *  output GCOV
 *  output the coverage data into .gcda
 */
void
gcov_dump(void)
{
	SIL_PRE_LOC;

	SIL_LOC_INT();
	__gcov_flush();
	SIL_UNL_INT();
}
#endif /* TOPPERS_ENABLE_GCOV_PART */
