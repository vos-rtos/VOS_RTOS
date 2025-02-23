/********************************************************************************************************
* 版    权: Copyright (c) 2020, VOS Open source. All rights reserved.
* 文    件: syscall.h
* 作    者: 156439848@qq.com; vincent_cws2008@gmail.com
* 版    本: VOS V1.0
* 历    史：
* --20200801：创建文件
* --20200828：添加注释
*********************************************************************************************************/

#ifndef __V_SYS_CALL_H__
#define __V_SYS_CALL_H__

#define VOS_SYSCALL_MUTEX_CREAT   	(u32)(10)
#define VOS_SYSCALL_MUTEX_WAIT		(u32)(11)
#define VOS_SYSCALL_MUTEX_RELEASE	(u32)(12)
#define VOS_SYSCALL_MUTEX_DELETE	(u32)(13)

#define VOS_SYSCALL_SEM_CREATE		(u32)(14)
#define VOS_SYSCALL_SEM_WAIT		(u32)(15)
#define VOS_SYSCALL_SEM_TRY_WAIT	(u32)(16)
#define VOS_SYSCALL_SEM_RELEASE		(u32)(17)
#define VOS_SYSCALL_SEM_DELETE		(u32)(18)

#define VOS_SYSCALL_EVENT_WAIT		(u32)(19)
#define VOS_SYSCALL_EVENT_SET		(u32)(20)
#define VOS_SYSCALL_EVENT_GET		(u32)(21)
#define VOS_SYSCALL_EVENT_CLEAR		(u32)(22)

#define VOS_SYSCALL_MSGQUE_CREAT	(u32)(23)
#define VOS_SYSCALL_MSGQUE_PUT		(u32)(24)
#define VOS_SYSCALL_MSGQUE_GET		(u32)(25)
#define VOS_SYSCALL_MSGQUE_FREE		(u32)(26)

#define VOS_SYSCALL_TASK_DELAY   	(u32)(27)

#define VOS_SYSCALL_TASK_CREATE		(u32)(28)

#define VOS_SYSCALL_SCH_TAB_DEBUG	(u32)(29)

#define VOS_SYSCALL_GET_CHAR		(u32)(30)

#endif
