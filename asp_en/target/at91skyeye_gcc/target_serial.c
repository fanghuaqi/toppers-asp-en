/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2007 by GJ Business Division RICOH COMPANY,LTD. JAPAN  
 *  Copyright (C) 2005-2007 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: target_serial.c 1838 2010-07-09 15:57:51Z ertl-honda $
 */

/*
 *  serial IO device (SIO) driver (for AT91SKYEYE)
 */

#include <kernel.h>
#include <t_syslog.h>
#include "at91skyeye.h"
#include "target_serial.h"

/*
 *  sio port initialization block
 */
typedef struct sio_port_initialization_block 
{
    void* us_thr;
    void* us_rpr;
    void* us_rcr;
    void* us_csr;
}
SIOPINIB;

/*
 *  sio port control block
 */
struct sio_port_control_block 
{
    const SIOPINIB  *p_siopinib;  /* pointer to sio init block */
    intptr_t        exinf;        /* extend information */
    bool_t    openflag;             /* open flag */
    bool_t    sendflag;             /* send flag  */
    bool_t    getready;             /* ready to get */
    bool_t    putready;             /* ready to put */
};

/*
 * sio port initialization block table
 */
const SIOPINIB siopinib_table[TNUM_SIOP] = {
    {(void*)(USART0_THR),
     (void*)(USART0_RPR),
     (void*)(USART0_RCR),
     (void*)(USART0_CSR)
    }
};

/*
 *  sio control block table
 */
SIOPCB	siopcb_table[TNUM_SIOP];

/*
 *  macro of getting sio port control block from sio port id
 */
#define INDEX_SIOP(siopid)  ((uint_t)((siopid) - 1))
#define get_siopcb(siopid)  (&(siopcb_table[INDEX_SIOP(siopid)]))


/*
 * receive buffer
 */
static uint8_t usart_rev_buf;

void
at91skyeye_init_uart(void)
{
    /* set receive buffer address */
    sil_wrw_mem((void *)(USART0_RPR), (uint32_t)(&usart_rev_buf));
    sil_wrw_mem((void *)(USART0_RCR), 0x0001);
}

/*
 * ready to get?
 */ 
Inline bool_t
uart_getready(SIOPCB *p_siopcb)
{
    return(sil_rew_mem((void*)(p_siopcb->p_siopinib->us_rcr)) == 0);
}

/*
 * ready to put?
 */
Inline bool_t
uart_putready(SIOPCB *p_siopcb)
{
    return(true);
}

/*
 *  get received char
 */
Inline uint8_t
uart_getchar(SIOPCB *p_siopcb)
{
    char_t c;
     
    c = usart_rev_buf;
    sil_wrw_mem((void *)(p_siopcb->p_siopinib->us_rpr), (uint32_t)(&usart_rev_buf));
    sil_wrw_mem((void *)(p_siopcb->p_siopinib->us_rcr), 0x0001);
    
    return(c);
}

/*
 *  write the char
 */
Inline void
uart_putchar(SIOPCB *p_siopcb, uint8_t c)
{
    sil_wrw_mem((void*)(p_siopcb->p_siopinib->us_thr),c);
}

/*
 * enable send interrupt
 */
Inline void
uart_enable_send(SIOPCB *p_siopcb)
{
}

/*
 *  disable send interrupt
 */
Inline void
uart_disable_send(SIOPCB *p_siopcb)
{

}


/*
 *  enable receive interrupt
 */
Inline void
uart_enable_rcv(SIOPCB *p_siopcb)
{
	sil_wrw_mem((void*)(USART0_IER),0x1);
}

/*
 *  disable receive interrupt
 */
Inline void
uart_disable_rcv(SIOPCB *p_siopcb)
{
	sil_wrw_mem((void*)(USART0_IDR),0x1);
}


/*
 *  initialize sio driver
 */
void
sio_initialize(intptr_t exinf)
{
    SIOPCB  *p_siopcb;
    uint_t  i;

    /*
     *  initialize sio port control block
     */
    for (p_siopcb = siopcb_table, i = 0; i < TNUM_SIOP; p_siopcb++, i++) {
        p_siopcb->p_siopinib = &(siopinib_table[i]);
        p_siopcb->openflag = false;
        p_siopcb->sendflag = false;
    }
}

/*
 * open the uart port of at91skyeye
 */
SIOPCB *
at91skyeye_uart_opn_por(SIOPCB *p_siopcb, intptr_t exinf)
{
    const SIOPINIB  *p_siopinib;
    p_siopinib = p_siopcb->p_siopinib;
   
    p_siopcb->exinf = exinf;
    p_siopcb->getready = p_siopcb->putready = false;
    p_siopcb->openflag = true;

    return(p_siopcb);
}


/*
 *  open sio port
 */
SIOPCB *
sio_opn_por(ID siopid, intptr_t exinf)
{
    SIOPCB  *p_siopcb = get_siopcb(siopid);
    bool_t    opnflg;
    ER      ercd;

    /*
     *  read open flag
     */
    opnflg = p_siopcb->openflag;

    /*
     *  the device dependent open
     */
    at91skyeye_uart_opn_por(p_siopcb, exinf);

    /*
     *  enable sio interrupt
     */
    if (!opnflg) {
        ercd = ena_int(INTNO_SIO);
        assert(ercd == E_OK);
    }
    return(p_siopcb);
}

/*
 *  close sio port
 */
void
sio_cls_por(SIOPCB *p_siopcb)
{
    /*
     * device dependent close
     */
    const SIOPINIB  *p_siopinib;

    p_siopinib = p_siopcb->p_siopinib;
    p_siopcb->openflag = false;
    
    /*
     *  disable sio port interrupt
     */
    dis_int(INTNO_SIO);
}

/*
 *  sio port interrupt handler
 */
void
sio_handler0(void)
{
    SIOPCB *p_siopcb = &(siopcb_table[0]);

    if (uart_getready(p_siopcb)) {
        /*
         *  call receive callback routine
         */
        sio_irdy_rcv(p_siopcb->exinf);
    }
    if (uart_putready(p_siopcb)) {
        /*
         *  call send callback routine
         */
        sio_irdy_snd(p_siopcb->exinf);
    }    
}


void
sio_handler1(void)
{
    SIOPCB *p_siopcb = &(siopcb_table[0]);
    
    if (uart_getready(p_siopcb)) {
        /*
         *  call receive callback routine
         */
        sio_irdy_rcv(p_siopcb->exinf);
    }
    if (uart_putready(p_siopcb)) {
        /*
         *  call send callback routine
         */
        sio_irdy_snd(p_siopcb->exinf);
    }    
}



/*
 *  sio port send char
 */
bool_t
sio_snd_chr(SIOPCB *siopcb, char_t c)
{    
    if (uart_putready(siopcb)){
        uart_putchar(siopcb, c);
        return(true);
    }
    return(false);
}

/*
 *  sio port receive char
 */
int_t
sio_rcv_chr(SIOPCB *siopcb)
{
    if (uart_getready(siopcb)) {
        return((int_t)(uint8_t) uart_getchar(siopcb));
    }
    return(-1);
}

/*
 *  enable callback routine of sio port
 */
void
sio_ena_cbr(SIOPCB *siopcb, uint_t cbrtn)
{
    switch (cbrtn) {
        case SIO_RDY_SND:
            uart_enable_send(siopcb);
            break;
        case SIO_RDY_RCV:
            uart_enable_rcv(siopcb);
            break;
    }
}

/*
 *  disable callback routine of sio port
 */
void
sio_dis_cbr(SIOPCB *siopcb, uint_t cbrtn)
{
    switch (cbrtn) {
        case SIO_RDY_SND:
            uart_disable_send(siopcb);
            break;
        case SIO_RDY_RCV:
            uart_disable_rcv(siopcb);
            break;
    }
}
