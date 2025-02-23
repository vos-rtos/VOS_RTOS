/********************************************************************************************************
* 版    权: Copyright (c) 2020, VOS Open source. All rights reserved.
* 文    件: stack_test.c
* 作    者: 156439848@qq.com; vincent_cws2008@gmail.com
* 版    本: VOS V1.0
* 历    史：
* --20200801：创建文件
* --20200828：添加注释
*********************************************************************************************************/
#include "vos.h"



static void task_stack_test(void *param)
{
	u8 buf[18*4];
	memset(buf, 0x55 , sizeof(buf));
	while(TestExitFlagGet() == 0) {
		VOSDelayUs(1000*1000);
	}
}

static u32 temp[50];//被破坏的地方
static long long task_stack[12]; //至少的18个u32

void stack_test()
{
	memset(temp, 0x44, sizeof(temp));
	kprintf("test stack overflow check!\r\n");
	s32 task_id;
	task_id = VOSTaskCreate(task_stack_test, 0, task_stack, sizeof(task_stack), TASK_PRIO_NORMAL, "task_stack_test");
	while (TestExitFlagGet() == 0) {
		VOSTaskDelay(1*1000);
	}
}
