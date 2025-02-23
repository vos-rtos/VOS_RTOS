/********************************************************************************************************
* 版    权: Copyright (c) 2020, VOS Open source. All rights reserved.
* 文    件: cpu.S
* 作    者: 156439848@qq.com; vincent_cws2008@gmail.com
* 版    本: VOS V1.0
* 历    史：
* --20200801：创建文件
* --20200828：添加注释
*********************************************************************************************************/

#include "vconf.h"

.syntax         unified
.thumb
.file   "cpu.S"

/********************************************************************************************************
* 函数：void SVC_Handler();
* 描述: SVC中断触发，是任务非特权级转特权级的唯一方法
* 参数: 无
* 返回：无
* 注意：无
*********************************************************************************************************/
.text
.align 2
.global SVC_Handler
.thumb
.thumb_func
SVC_Handler:
	push {lr} //注意这里如果是刚启动时，是压栈到MSP, 如果任务调用，是压栈到psp
	tst	lr, #4
	ite	eq
	mrseq	r0, msp
	mrsne	r0, psp
	//这里是比较当前进入svn是是哪个栈，传递给SVC_Handler_C的第二个参数
	mrs		r1, msp
	SUB		r1, r0
	//调用C语言的SVC
	bl	SVC_Handler_C
	pop  {pc}


/********************************************************************************************************
* 函数：void asm_ctx_switch();
* 描述: 上下文切换，主要是触发PendSV_Handler中断，在PendSV_Handler中断中进行上下文切换
* 参数: 无
* 返回：无
* 注意：无
*********************************************************************************************************/
.text
.align 2
.global asm_ctx_switch
.thumb
.thumb_func
asm_ctx_switch:
	LDR     R0, =0xE000ED04 //触发PendSV_Handler中断
	LDR     R1, =0x10000000
	STR     R1, [R0]
	BX      LR

/********************************************************************************************************
* 函数：void PendSV_Handler();
* 描述: 进行上下文切换，把就绪最高优先级任务跟正在运行的任务进行切换
* 参数: 无
* 返回：无
* 注意：无
*********************************************************************************************************/
.text
.align 2
.global PendSV_Handler
.thumb
.thumb_func
PendSV_Handler:
    MRS     r0, PRIMASK //关中断
    CPSID   I
    push  {r0}

	push {lr}
	//任务准备切换出去，这里统计cpu占用率
	bl	HookTaskSwitchOut
	//找到最高优先级并设置给pReadyTask,返回0则不合法直接退出
	bl PrepareForCortexSwitch
	pop {lr}
	cmp r0, #0
    beq INT_ENABLE

	mrs	r0, psp //保存当前上下文
	tst	lr, #0x10 //测试浮点是否需要压栈
	it eq
	vstmdbeq r0!, {s16-s31}
	mov r2, lr  //r2 为中断进入前的lr
	mrs r3, control //r3为进入前的control
	stmdb r0!, {r2-r11} //r2,r3(lr,control), r4-r11 保存到任务栈
	ldr r1, =pRunningTask //保存当前运行任务的地址
	ldr r2, [r1]
	str r0, [r2] //psp保存到pRunningTask->ptask
	ldr r3, =pReadyTask
	ldr r4, [r3]
	str r4, [r1] //pReadyTask指向的任务赋值给pRunningTask
	ldr r0, [r4] //[r4]为pReadyTask的psp指针
	mov r2, #0
	str r2, [r3] //清空pReadyTask的值
	ldmia r0!, {r2-r11} //把pReadyTask->ptask栈恢复到当前的寄存器
	mov lr, r2
	msr control, r3
	isb
	tst lr, #0x10
	it eq
	vldmiaeq r0!, {s16-s31}
	msr psp, r0

	//最高优先级任务已经切换进来，这里统计cpu占用率
	push {lr}
	bl	HookTaskSwitchIn
	pop {lr}

INT_ENABLE:
	pop {r0}
	MSR PRIMASK, R0

	//清标志VOSCtxSwtFlag
	mov r0, #0
	ldr r1, =VOSCtxSwtFlag
	str r0, [r1]

	bx lr //返回后才切换到psp

/********************************************************************************************************
* 函数：void RunFirstTask();
* 描述: 运行第一个任务，主要是配置各寄存器值，然后直接跑例程
* 参数: 无
* 返回：无
* 注意：无
*********************************************************************************************************/
.text
.align 2
.global RunFirstTask
.thumb
.thumb_func
RunFirstTask:
	//push	{lr}
	CPSID    I

    //设置PendSV_Handler为最低优先级
   	LDR     R0, =0xE000ED22
	LDR     R1, =0xFF
	STRB    R1, [R0]

	LDR 	R0, =pRunningTask
	//LDR		R1, =SVC_EXC_RETURN
	LDR		R2, [R0]
	LDR		R2, [R2] //ptask->pstack
	LDR		R3, [R2] //*ptask->pstack
	//STR		R3,	[R1] //写SVC中断的LR值
	MOV		R0, #10
    LSL		R0, R0, #2 // 10 * 4
	ADD 	R1, R2, R0
    MSR		PSP, R1    //设置PSP的值, PSP指向 r0, r0->r1->r2->r3->r12->lr->pc->xPSR

	MRS     R0, CONTROL //设置线程模式)，用户级
	ORR     R0, R0, #2 //#2:特权+PSP, #3:用户级+PSP, 刚启动，这里需要特权模式
	MSR     CONTROL, R0
	ISB

	LDMIA   SP!, {R0-R3}
	LDMIA   SP!, {R12, LR}
	LDMIA   SP!, {R3, R4} //R4:xPSR, R3: PC

	CPSIE    I
#if (TASK_LEVEL)
	//设置回用户模式
	ORR     R2, R2, #3 //#2:特权+PSP, #3:用户级+PSP, 刚启动，这里需要特权模式
	MSR     CONTROL, R2
	ISB
#endif
	//设置系统运行标志
    LDR     R2, =VOSRunning
    MOVS    R1, #1
    STRB    R1, [R2]

	//跳转到 任务
	BX		R3 //R3: PC


/********************************************************************************************************
* 函数：u32 __local_irq_save();
* 描述: 关中断，并返回关中断前的中断状态，用于多层中断嵌套
* 参数: 无
* 返回：返回关中断前的中断状态
* 注意：无
*********************************************************************************************************/
.text
.align 2
.global __local_irq_save
.thumb
.thumb_func
__local_irq_save:
    MRS     R0, PRIMASK
    CPSID   I
    ldr 	R1, =vos_dis_irq_counter //计数嵌套层数
    ldr		R2, [R1]
    add		R2, #1 //加1
    str		R2, [R1]
    BX      LR

/********************************************************************************************************
* 函数：void __local_irq_restore(u32 old);
* 描述: 开中断，并返回恢复中断前的中断状态，用于多层中断嵌套
* 参数:
* [1] old: u32 __local_irq_save()返回值作为参数
* 返回：返回关中断前的中断状态
* 注意：无
*********************************************************************************************************/
.text
.align 2
.global __local_irq_restore
.thumb
.thumb_func
__local_irq_restore:
	ldr 	R1, =vos_dis_irq_counter //计数嵌套层数
    ldr		R2, [R1]
    sub		R2, #1 //减1
    str		R2, [R1]
    MSR     PRIMASK, R0
    BX      LR

/********************************************************************************************************
* 函数：void local_irq_disable();
* 描述: 关中断
* 参数: 无
* 返回：无
* 注意：无
*********************************************************************************************************/
.text
.align 2
.global local_irq_disable
.thumb
.thumb_func
local_irq_disable:
    cpsid	i
    bx      lr

/********************************************************************************************************
* 函数：void local_irq_enable();
* 描述: 开中断
* 参数: 无
* 返回：无
* 注意：无
*********************************************************************************************************/
.text
.align 2
.global local_irq_enable
.thumb
.thumb_func
local_irq_enable:
    cpsie	i
    bx      lr

/********************************************************************************************************
* 函数：void VOSDelayUsAsm(u32 us, u32 cpu_mhz);
* 描述: 汇编编写延时函数
* 参数:
* [1] ms: 延时毫秒数
* 返回：无
* 注意：无
*********************************************************************************************************/
.text
.align 2
.global VOSDelayUsAsm
.thumb
.thumb_func
VOSDelayUsAsm:
	add r1, #4 //168MHz: 1us执行168条指令 + 4条指令
loop_xus:
	mov r2, r1
	asr r2, r2, #2 //除4，因为内循环消耗4个指令, 加空指令为凑够4个指令
loop_1us:
	subs r2, #1
	nop
	bne loop_1us
	subs r0, #1
	bne loop_xus
	bx lr

/********************************************************************************************************
* 函数：void vos_sys_call();
* 描述: 5号 SVC中断
* 参数:
* 返回：无
* 注意：无
*********************************************************************************************************/
.text
.align 2
.global vos_sys_call
.thumb
.thumb_func
vos_sys_call:
    svc		5  //VOS_SVC_NUM_SYSCALL
    bx      lr

/********************************************************************************************************
* 函数：u32 vos_privileged_save();
* 描述: 6号 SVC中断， 中断关闭保存和特权切换保存
* 参数:
* 返回：SVC中断后返回值为中断前的特权状态
* 注意：无
*********************************************************************************************************/
.text
.align 2
.global vos_privileged_save
.thumb
.thumb_func
vos_privileged_save:
	svc 	6 //VOS_SVC_PRIVILEGED_MODE
    BX      LR

/********************************************************************************************************
* 函数：void vos_privileged_restore(u32 old);
* 描述: 中断和特权恢复
* 参数:
* [1] old: 原来的特权和中断状态
* 返回：无
* 注意：无
*********************************************************************************************************/
.text
.align 2
.global vos_privileged_restore
.thumb
.thumb_func
vos_privileged_restore:
	MRS		R1, CONTROL //特权模式下获取control
	ORR		R2, R1, R0
	MSR     CONTROL, R2
    BX      LR

/********************************************************************************************************
* 函数：void __switch_privileged_mode();
* 描述: 切换特权
* 参数: 无
* 返回：无
* 注意：无
*********************************************************************************************************/
.text
.align 2
.global __switch_privileged_mode
.thumb
.thumb_func
__switch_privileged_mode:
	MRS     R0, CONTROL //只能读出control[0]位，control[1]位拥有是0，因为svn中断模式
	BIC		R1, R0, #1	//control[0]=0 //特权模式
	MSR     CONTROL, R1
	ISB
	BX		LR



