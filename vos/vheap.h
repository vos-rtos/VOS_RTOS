/********************************************************************************************************
* 版    权: Copyright (c) 2020, VOS Open source. All rights reserved.
* 文    件: vheap.h
* 作    者: 156439848@qq.com; vincent_cws2008@gmail.com
* 版    本: VOS V1.0
* 历    史:无
*********************************************************************************************************/

#ifndef __VHeap_H__
#define __VHeap_H__

#include "vtype.h"

struct StVMemHeap;

void VHeapMgrInit();
void VHeapMgrAdd(struct StVMemHeap *pheap);
void VHeapMgrDel(struct StVMemHeap *pheap);
struct StVMemHeap *VHeapFindByName(s8 *name);

void *VHeapMgrGetPageBaseAddr(void *and_addr, u8 *heap_name, s32 len);

void *vmalloc(u32 size);
void vfree(void *ptr);
void *vrealloc(void *ptr, u32 size);
void *vcalloc(u32 nitems, u32 size);

void *vmalloc_spec(u32 size, s8 *heap_name); //指定某个通用堆里分配内存，vfree同用一个

#endif
