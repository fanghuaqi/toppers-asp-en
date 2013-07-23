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
 *  @(#) $Id: target_serial.h 1838 2010-07-09 15:57:51Z ertl-honda $
 */

/*
 *  serial IO device (SIO) driver (for AT91SKYEYE)
 */

#ifndef TOPPERS_TARGET_SERIAL_H
#define TOPPERS_TARGET_SERIAL_H

#include "at91skyeye.h"

/*
 *  sio interrupt no.
 */
#define INHNO_SIO        IRQ_USART0	/* interrupt handler no. */
#define INTNO_SIO        IRQ_USART0	/* interrupt no. */
#define INTPRI_SIO       -2        	/* interrupt priority */
#define INTATR_SIO       0U       	/* interrupt attribute */

/*
 *  the number of sio port
 */
#define TNUM_SIOP		1		/* the number of supported sio port */


#ifndef TOPPERS_MACRO_ONLY

/*
 *  sio port control block
 */
typedef struct sio_port_control_block	SIOPCB;

/*
 *  the id no. of call back routine
 */
#define SIO_RDY_SND	1U		/* ready to send callback */
#define SIO_RDY_RCV	2U		/* ready to receive callback */

/*
 * the initialization for the log output at kernel startup (in usart.h)
 */
extern  void at91skyeye_init_uart(void);

/*
 *  the polling output of uart
 */
Inline void
at91skyeye_putc(char_t c)
{
    sil_wrw_mem((void *)USART0_THR, c);
}

/*
 *  initialize sio driver
 */
extern void sio_initialize(intptr_t exinf);

/*
 *  open sio port
 */
extern SIOPCB *sio_opn_por(ID siopid, intptr_t exinf);

/*
 *  close sio port
 */
extern void sio_cls_por(SIOPCB *p_siopcb);

/*
 *  sio port interrupt handler
 */
extern void sio_handler(void);

/*
 *  sio port send char
 */
extern bool_t sio_snd_chr(SIOPCB *siopcb, char_t c);

/*
 *  sio port receive char
 */
extern int_t sio_rcv_chr(SIOPCB *siopcb);

/*
 *  enable callback routine of sio port
 */
extern void sio_ena_cbr(SIOPCB *siopcb, uint_t cbrtn);

/*
 *  disable callback routine of sio port
 */
extern void sio_dis_cbr(SIOPCB *siopcb, uint_t cbrtn);

/*
 *  ready to send callback in interrupt
 */
extern void sio_irdy_snd(intptr_t exinf);

/*
 *  ready to receive callback in interrupt
 */
extern void sio_irdy_rcv(intptr_t exinf);

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_TARGET_SERIAL_H */
