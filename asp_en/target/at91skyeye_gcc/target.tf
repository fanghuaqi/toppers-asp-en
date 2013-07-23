$ 
$     the target dependent template in PASS2 (for AT91SKYEYE)
$ 


$ 
$  the valid intno, inhno adn cpu exception no.
$ 
$INTNO_VALID = { 0,1,...,31 }$
$INHNO_VALID = INTNO_VALID$

$ 
$  the valid intno and inhno that can be used by ATT_ISR
$INTNO_ATTISR_VALID = INTNO_VALID$
$INHNO_ATTISR_VALID = INHNO_VALID$

$ 
$  the valid inthno that can be used by DEF_INT
$ 
$INHNO_DEFINH_VALID = INHNO_VALID$

$ 
$  the valid intno and interrupt priority that can 
$  be used by CFG_INT
$ 
$INTNO_CFGINT_VALID  = INTNO_VALID$
$INTPRI_CFGINT_VALID = { -1,-2,...,-7 }$

$ 
$  the target dependent interrupt attribute bit
$ 
$TARGET_INTATR = TA_HIGHLEVEL | TA_POSEDGE | TA_LOWLEVEL$

$ 
$  include core dependent template (for ARM)
$ 
$INCLUDE"arm_gcc/common/core.tf"$

$ 
$  generate interrupt priority table
$ 
$FILE "kernel_cfg.c"$
$NL$
const PRI _kernel_inh_ipm_tbl[TNUM_INH] = {$NL$
$FOREACH inhno INHNO_VALID$ 
	$IF LENGTH(INH.INHNO[inhno])$
	  $TAB$$INT.INTPRI[inhno]$,
	$ELSE$
	  $TAB$0,
	$END$
	$SPC$$FORMAT("/* %d */", +inhno)$$NL$
$END$
$NL$};$NL$
$NL$

$ 
$ generate interrupt mask table
$ 
const uint32_t _kernel_ipm_mask_tbl[8]={$NL$
$FOREACH intpri { 0,-1,...,-7 }$
 $intmask = 0$
 $FOREACH intno (INT.ID_LIST)$
  $IF INT.INTPRI[intno] >= intpri $
	$intmask = intmask | (1 << (INT.INTNO[intno]))$
  $END$
 $END$
 $TAB$UINT32_C($FORMAT("0x%08x", intmask)$),/* Priority $+intpri$ */$NL$
$END$
$NL$
};$NL$


$ 
$  generate interrupt handler table
$ 
$NL$
const FP _kernel_inh_tbl[TNUM_INH] = {$NL$
$FOREACH inhno INHNO_VALID$ 
	$IF LENGTH(INH.INHNO[inhno])$
		$TAB$(FP)($INH.INTHDR[inhno]$),
	$ELSE$
		$TAB$(FP)(_kernel_default_int_handler),
	$END$
	$SPC$$FORMAT("/* %d */", +inhno)$$NL$
$END$
$NL$};$NL$
$NL$
