/* This file is generated from core_rename.def by genrename. */

#ifndef TOPPERS_CORE_RENAME_H
#define TOPPERS_CORE_RENAME_H

/*
 *  kernel_cfg.c
 */
#define exch_tbl					_kernel_exch_tbl

/*
 *  core_support.S
 */
#define start_r						_kernel_start_r
#define ret_int						_kernel_ret_int
#define ret_exc						_kernel_ret_exc
#define exch_tbl					_kernel_exch_tbl
#define dispatch					_kernel_dispatch
#define start_dispatch				_kernel_start_dispatch
#define exit_and_dispatch			_kernel_exit_and_dispatch
#define call_exit_kernel			_kernel_call_exit_kernel
#define default_exc_handler			_kernel_default_exc_handler

/*
 *  core_config.c
 */
#define excpt_nest_count			_kernel_excpt_nest_count
#define core_initialize				_kernel_core_initialize
#define core_terminate				_kernel_core_terminate
#define x_install_exc				_kernel_x_install_exc

#ifdef TOPPERS_LABEL_ASM

/*
 *  kernel_cfg.c
 */
#define _exch_tbl					__kernel_exch_tbl

/*
 *  core_support.S
 */
#define _start_r					__kernel_start_r
#define _ret_int					__kernel_ret_int
#define _ret_exc					__kernel_ret_exc
#define _exch_tbl					__kernel_exch_tbl
#define _dispatch					__kernel_dispatch
#define _start_dispatch				__kernel_start_dispatch
#define _exit_and_dispatch			__kernel_exit_and_dispatch
#define _call_exit_kernel			__kernel_call_exit_kernel
#define _default_exc_handler		__kernel_default_exc_handler

/*
 *  core_config.c
 */
#define _excpt_nest_count			__kernel_excpt_nest_count
#define _core_initialize			__kernel_core_initialize
#define _core_terminate				__kernel_core_terminate
#define _x_install_exc				__kernel_x_install_exc

#endif /* TOPPERS_LABEL_ASM */


#endif /* TOPPERS_CORE_RENAME_H */
