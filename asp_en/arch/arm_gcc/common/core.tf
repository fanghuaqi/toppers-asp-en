$ 
$	core-dependent template in pass2 (for ARM)
$ 

$ 
$ valid CPU exception numbers
$ 
$EXCNO_VALID = { 1,2,3,4,6 }$

$ 
$  the CPU exception numbers that can be used in DEF_EXC 
$ 
$EXCNO_DEFEXC_VALID = EXCNO_VALID$

$ 
$  include kernel standard template file
$ 
$INCLUDE "kernel/kernel.tf"$

$ 
$  generate the table of CPU exception handlers
$ 
$FILE "kernel_cfg.c"$
$NL$
const FP _kernel_exch_tbl[TNUM_EXCH] = {$NL$
$FOREACH excno {0,1,...,6}$ 
	$IF LENGTH(EXC.EXCNO[excno])$
		$TAB$(FP)($EXC.EXCHDR[excno]$),
	$ELSE$
		$TAB$(FP)(_kernel_default_exc_handler),
	$END$
	$SPC$$FORMAT("/* %d */", +excno)$$NL$
$END$
$NL$};$NL$
$NL$

