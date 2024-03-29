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
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 *  @(#) $Id: target_support.S 1972 2010-11-28 14:43:54Z ertl-honda $
 */

/*
 *  チップ依存モジュール アセンブリ言語部（AT91SKYEYE用）
 */

#define  TOPPERS_MACRO_ONLY
#define UINT_C(val)		(val)		/* uint_t型の定数を作るマクロ */
#define ULONG_C(val)	(val)		/* ulong_t型の定数を作るマクロ */
#include "kernel_impl.h"

/*
 *  低レベルのターゲットシステム依存の初期化
 *
 *  スタートアップモジュールの中で，メモリ初期化の前に呼び出される．          
 */
        .text
        .align 2
        .global hardware_init_hook
hardware_init_hook:
        bx  lr

/*
 * 割込みハンドラ
 *
 * IRQ 例外ベクタから呼び出される
 */
    .text
    .align 2
    .global irq_handler
irq_handler:
    /* 
     * IRQモードで実行される
     */
    /* 
     *  割込み前のモード(スーパーバイザーモード)へ
     *  移行し，コンテキストを保存する
     */
    msr   cpsr, #(CPSR_SVC|CPSR_IRQ_BIT) 
    stmfd sp!, {r0-r3, ip, lr, pc} /* pcはダミー */

    /*
     * spsrと戻り番地を取得するためにIRQモードへ
     */
    msr   cpsr, #(CPSR_IRQ|CPSR_IRQ_BIT)
    sub   r0, lr, #4
    mrs   r1, spsr

    /*
     *  割込みハンドラ実行時のモード（スーパーバイザーモード）に
     */
    msr   cpsr, #(CPSR_SVC|CPSR_IRQ_BIT) 
    str   r0, [sp, #0x18] /* 戻り番地をスタックに */
    stmfd sp!, {r1}       /* spsrをスタックに保存 */
    mov   lr, sp          /* この時点のスタックを復帰のため取得 */

    /*
     *  多重割込みか判定
     */

    ldr   r2, =excpt_nest_count /* 例外・割込みネスト回数を取得 */
    ldr   r3, [r2]
    add   r0, r3, #1            /* 例外・割込みネスト回数を更新 */
    str   r0, [r2]
    cmp   r3, #0    
    bne   irq_handler_1

    /* 
     * 一段目の例外（割込み）ならスタックを変更する
     */
    ldr   r0, =_kernel_istkpt
    ldr   sp, [r0]

irq_handler_1:
    stmfd sp!, {lr}     /* 復帰用のスタックポインタの保存 */

    ldr   r1, =AIC_IVR
    ldr   r3, [r1]

    /*
     *  割り込み要因の判定
     */
    ldr   r1, =AIC_ISR
    ldr   r3, [r1]

    /*
     *  割込み要因の割込み優先度を求め(モデル上の)割込み優先度マスクをセット
     *  する．またその際，ハンドラ実行前の(モデル上の)割込み優先度マスクを
     *  保存する．
     */
    ldr   r0, =inh_ipm_tbl      /* 割込み優先度を取得 */
    ldr   r1, [r0, r3, lsl #2]  /* r1<-割込み優先度   */
    ldr   r0, =ipm              /* 割込み発生前の割込み優先度マスクをスタックに保存 */
    ldr   r2, [r0]              
    stmfd sp!,{r2}         
    str   r1, [r0]              /* (モデル上の)割込み優先度マスクをセット */

    /*
     * (モデル上の)割込み優先度マスクの設定
     * 
     * 受け付けた割込みの割込み優先度に設定された割込み要求禁止フラグの
     * テーブルの値と(モデル上の)各割込みの割込み要求禁止フラグの状態を
     * 保持した変数の値とのORをIRCの割込み要求禁止フラグにセットする．
     */
    rsb   r1, r1,  #0           /* インデックスとなるように反転 */
    ldr   r0, =ipm_mask_tbl     /* (モデル上)の割込み優先度を実現するための */
    ldr   r2, [r0, r1, lsl #2]  /* 割込み要求禁止フラグを取得 */    
    ldr   r0, =idf              /* 各割込みの割込み要求禁止フラグの状態を取得 */
    ldr   r1, [r0]
    /*
     * 各割込みの割込み要求禁止フラグの状態と(モデル上)の割込み優先度のを実現する
     * ための割込み要求禁止フラグの状態のORをとり，それの否定を求めることにより，
     * IRCの割込み許可レジスタへの設定値を生成し設定する．
     */
    mvn   lr, #0                
    ldr   r0, =AIC_IDCR
    str   lr, [r0]
    orr   r1, r1, r2            /* マスク指定されていない割込みの許可 */
    mvn   r1, r1                /* 設定値を生成 */    
    ldr   r0, =AIC_IECR
    str   r1, [r0]

    /*
     *  割込みハンドラの起動番地を取得
     */
    ldr   r0, =inh_tbl         /* 割込みハンドラテーブルの読み出し  */
    ldr   r0, [r0, r3, lsl #2] /* r0<-割込みハンドラ            */    

    /*
     *  EOIのために保存
     */
    mov   r1, #1
    mov   r1, r1, lsl r3
    stmfd sp!, {r1} 

    stmfd sp!,{r3}       /* inhno を保存  */

    /* 
     * 割り込み許可 
     */         
    msr   cpsr, #(CPSR_SVC)

#ifdef LOG_INH_ENTER
    stmfd sp!,{r0}
    mov   r0, r3         /* inhno をパラメータに    */
    bl    log_inh_enter  /* log_inh_enterを呼び出す */
    ldmfd sp!,{r0}
#endif /* LOG_INH_ENTER */

    /* 
     * 割込みハンドラの呼び出し
     */
    mov   lr, pc
    bx    r0

    ldmfd sp!,{r0}       /* inhno を復帰  */    
#ifdef LOG_INH_LEAVE
    bl    log_inh_leave  /* log_inh_leaveを呼び出す */
#endif /* LOG_INH_LEAVE */

    /*
     * カーネル管理の割込みを禁止する
     */
    msr   cpsr, #(CPSR_SVC|CPSR_IRQ_BIT)

    /*
     * 割込みクリア
     */
    ldmfd   sp!,{r0}
    ldr     r3, =AIC_EOI
    str     r0, [r3]

    b     target_ret_int


/*
 *
 * ターゲット依存の例外入口処理
 *
 */
    .text
    .global target_exc_handler
target_exc_handler:    
    /*
     *  例外実行のモード（スーパーバイザーモード）に
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     *  この時点のレジスタの内容は以下の通り．
     *    r0 : lr(戻り先)
     *    r1 : spsr
     *    r2 : 例外番号
     */
    msr   cpsr, #(CPSR_SVC|CPSR_FIQ_BIT|CPSR_IRQ_BIT) 
    str   r0, [sp, #0x18] /* 戻り番地をスタックに */
    stmfd sp!, {r1}       /* spsrをスタックに保存 */    
    mov   lr, sp          /* この時点のスタックを復帰のため取得 */

    ldr   r0, =ipm        /* 割込み発生前の割込み優先度マスクをスタックに保存 */
    ldr   r3, [r0]              
    stmfd sp!, {r3}

    /* 
     * コンテキスト判定のため，excpt_nest_count をスタックに保存．
     * スタックに保存せず，現在のexcpt_nest_countを-1すると取得できるが，
     * スタックに積んでおいた方がデバッグ等が行いやすいので，スタックに
     * 保存する．
     */
    ldr   r0, =excpt_nest_count
    ldr   r3, [r0]
    stmfd sp!, {r3}

    mov   r3,  sp         /* 例外フレーム番地を保存 */

    /*
     * カーネル管理外のCPU例外か判定する
     * 
     * カーネル管理外のCPU例外は，カーネル実行中，全割込みロック状態，
     * CPUロック状態，カーネル管理外の割込みハンドラ実行中に発生した
     * CPU例外である．ARMの場合は，SPSRのI/Fフラグのいずれかが'1'なら，
     * 該当する．
     */
    tst   r1, #(CPSR_FIQ_BIT|CPSR_IRQ_BIT) 
    bne   target_kernel_unc_exc_handler /* カーネル管理外のCPU例外の処理へ */

    /*
     * 割込み発生時のコンテキストを判定
     */
    ldr   r0, =excpt_nest_count
    ldr   r1, [r0]
    add   r1, r1, #1
    str   r1, [r0]
    cmp   r1, #1
    bne   target_exc_handler_1    

    /* 
     * タスクコンテキストでCPU例外が発生した場合
     * スタックを非タスクコンテキストに切り替える．
     */
    ldr  r0, =_kernel_istkpt
    ldr  sp, [r0]

target_exc_handler_1:    
    stmfd sp!, {lr}     /* 復帰用のスタックポインタの保存 */

    /* 
     * 割込み発生前の割込み優先度マスクをスタックに保存 
     * 割込みハンドラと出口ルーチンを共有するために保存
     */
    ldr   r0, =ipm       
    ldr   r1, [r0]              
    stmfd sp!, {r1}

    /*
     *  CPU例外ハンドラの起動番地を取得
     */
    ldr   r0, =exch_tbl        /* 割込みハンドラテーブルの読み出し */
    ldr   r1, [r0, r2, lsl #2] /* r1<-例外ハンドラ                 */

    stmfd sp!,{r2}      /* excno を保存 */

    /* 
     * 割り込み許可
     * CPU管理外の例外ハンドラは別ルーチンで呼び出すため，単純にSVCモード
     * にするだけでよい．
     */
    msr   cpsr, #(CPSR_SVC)
          
#ifdef LOG_EXC_ENTER
    stmfd sp!,{r1,r3}
    mov   r0, r2         /* excno をパラメータに    */
    bl    log_exc_enter  /* log_exc_enterを呼び出す */
    ldmfd sp!,{r1,r3}
#endif /* LOG_EXC_ENTER */

    /* 
     * CPU例外ハンドラの呼び出し
     * 例外フレームの先頭を引数として渡す
     */        
    mov   r0, r3
    mov   lr, pc
    bx    r1

    ldmfd sp!,{r0}       /* excno を復帰  */    
#ifdef LOG_EXC_LEAVE
    bl    log_exc_leave  /* log_exc_leaveを呼び出す */
#endif /* LOG_EXC_LEAVE */


/*
 *
 * ターゲット依存の例外・割込みの出口処理
 *
 */
    .global target_ret_exc
target_ret_exc:
    /*
     * カーネル管理の割込みを禁止する
     */
    msr   cpsr, #(CPSR_SVC|CPSR_IRQ_BIT)  

    .global target_ret_int        
target_ret_int: 
    /*
     * 割込み優先度マスクを元に戻す
     */
    ldmfd sp!, {r1}             /* 元の割込み優先度マスクを取得 */
    ldr   r0, =ipm              /* 割込み優先度マスクを復帰     */
    str   r1, [r0]
    rsb   r1, r1,  #0           /* インデックスとなるように反転 */
    ldr   r0, =ipm_mask_tbl     /* (モデル上)の割込み優先度のを実現するための */
    ldr   r2, [r0, r1, lsl #2]  /* 割込み要求禁止フラグを取得 */    
    ldr   r0, =idf              /* 各割込みの割込み要求禁止フラグの状態を取得 */
    ldr   r1, [r0]
    /*
     * 各割込みの割込み要求禁止フラグの状態と(モデル上)の割込み優先度のを実現する
     * ための割込み要求禁止フラグの状態のORをとり，それの否定を求めることにより，
     * IRCの割込み許可レジスタへの設定値を生成し設定する．
     */
    mvn   lr, #0                
    ldr   r0, =AIC_IDCR
    str   lr, [r0]
    orr   r1, r1, r2            /* マスク指定されていない割込みの許可 */
    mvn   r1, r1                /* 設定値を生成 */    
    ldr   r0, =AIC_IECR
    str   r1, [r0]

    /*
     * スタックポインタの復帰
     */
    ldmfd sp!, {r2}             /* 元のスタックポインタを取得 */
    mov   sp, r2
    
    /*
     * 後の処理はARM依存部で実行
     */            
    b     ret_int


/*
 *
 * カーネル管理外のCPU例外の出入口処理
 *
 */
target_kernel_unc_exc_handler:
    /*
     * 割込み発生時のコンテキストを判定
     */
    ldr   r0, =excpt_nest_count
    ldr   r1, [r0]
    add   r1, r1, #1
    str   r1, [r0]
    cmp   r1, #1
    bne   target_kernel_unc_exc_handler_1
    
    /* 
     * タスクコンテキストでCPU例外が発生した場合
     * スタックを非タスクコンテキストに切り替える．
     */
    ldr  r0, =_kernel_istkpt
    ldr  sp, [r0]

target_kernel_unc_exc_handler_1:
    stmfd sp!, {lr}     /* 復帰用のスタックポインタの保存 */
    
    /*
     *  CPU例外ハンドラの起動番地を取得
     */
    ldr   r0, =exch_tbl        /* 割込みハンドラテーブルの読み出し */
    ldr   r1, [r0, r2, lsl #2] /* r1<-例外ハンドラ                 */

   /*
    * システム状態（コンテキストは除く）を，CPU例外発生時の状態へ
    */
    ldr   r0, [lr]             /* CPU例外発生前のCPSRの取得 */
    and   r0, r0, #(CPSR_IRQ_BIT|CPSR_FIQ_BIT)
    orr   r0, r0, #(CPSR_SVC)
    msr   cpsr, r0

    /* 
     * CPU例外ハンドラの呼び出し
     * 例外フレームの先頭を引数として渡す
     */        
    mov   r0, r3
    mov   lr, pc
    mov   pc, r1

    /*
     *  例外・割込みのネストカウント（excpt_nest_count)のデクリメント
     */        
    ldr   r0, =excpt_nest_count   /* r0 <-excpt_nest_count */
    ldr   r1, [r0]
    sub   r2, r1, #1
    str   r2, [r0]

    /*
     * スタックポインタの復帰
     */
    ldmfd sp!, {r2}             /* 元のスタックポインタを取得 */
    mov   sp, r2

    /*
     * CPU例外からの復帰
     */
    ldmfd sp!,{r1}              /* CPSRの復帰処理 */
    msr   spsr, r1              /* 戻り先のcpsrをspsrに設定 */
    ldmfd sp!,{r0-r3,ip,lr,pc}^ /* コンテキストの復帰，^付きなので、cpsr <- spsr */
