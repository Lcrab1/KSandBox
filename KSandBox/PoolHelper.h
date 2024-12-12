#pragma once
#include <fltKernel.h>
#include "LockHelper.h"
#include "ListHelper.h"

#define		KERNEL_MODE
#define		POOL_TAG 'xoBS'   


#if defined(KERNEL_MODE)

#define POOL_PAGE_SIZE          4096            // system page size
#define POOL_CELL_SIZE          16				//内存池单元格
#ifdef _WIN64
#define POOL_MASK_LEFT          0xFFFFFFFFFFFFF000UL
#else
#define POOL_MASK_LEFT          0xFFFFF000UL
#endif
#define POOL_MASK_RIGHT         0xFFF


#else /* ! KERNEL_MODE */

#define POOL_PAGE_SIZE          65536           // VirtualAlloc granularity
#define POOL_CELL_SIZE          128
#ifdef _WIN64
#define POOL_MASK_LEFT          0xFFFFFFFFFFFF0000UL
#else
#define POOL_MASK_LEFT          0xFFFF0000UL
#endif
#define POOL_MASK_RIGHT         0xFFFF
#endif

#define LARGE_CHUNK_MINIMUM     (POOL_PAGE_SIZE * 3 / 4)
#define FULL_PAGE_THRESHOLD     4

#ifndef POOL_DEBUG
#define POOL_DEBUG 0
#endif
#ifndef POOL_TIMING
#define POOL_TIMING 0
#endif

#define PAD_8(p)        (((p) + 7) & ~7)					//按照8字节向上对齐
#define PAD_CELL(p)     (((p) + POOL_CELL_SIZE - 1) & ~(POOL_CELL_SIZE - 1))	//按照POOL_CELL_SIZE向上对齐
#define NUM_CELLS(n)    (PAD_CELL(n) / POOL_CELL_SIZE)		//计算单元格数量

#define PAGE_HEADER_SIZE    PAD_CELL(sizeof(PAGE))
#define PAGE_BITMAP_SIZE    \
    PAD_CELL(((POOL_PAGE_SIZE - PAGE_HEADER_SIZE) / POOL_CELL_SIZE + 7) / 8)
#define NUMBER_PAGE_CELLS      \
    ((POOL_PAGE_SIZE - PAGE_HEADER_SIZE - PAGE_BITMAP_SIZE) / POOL_CELL_SIZE)
#define LARGE_CHUNK_SIZE    PAD_8(sizeof(LARGE_CHUNK))

typedef struct _POOL_  POOL;
typedef struct _POOL_* PPOOL;

typedef struct PAGE PAGE;


#pragma pack(1)
typedef struct PAGE {

	NODE Node;
	struct PAGE* Flink;
	PPOOL  Pool;
	ULONG  Tag;
	USHORT num_free;                    // estimated, not accurate
}PAGE, * PPAGE;

struct _POOL_
{

	ULONG Tag;

#ifdef POOL_USE_CUSTOM_LOCK

	LOCK pages_lock;
	LOCK large_chunks_lock;

#elif defined(KERNEL_MODE)

	PERESOURCE Lock;

#else /* ! KERNEL_MODE */

	volatile LONG ThreadIdentify;
	CRITICAL_SECTION Lock;

#endif

	LIST pages;                         // pages searched during allocation
	LIST full_pages;                    // full pages that are not searched
	LIST large_chunks;

	UCHAR InitializeBitmap[PAGE_BITMAP_SIZE];
};


PPOOL SeCreateMemoryPool(void);
PPOOL SeCreateMemoryPoolTagged(ULONG Tag);
PPAGE SeAllocateMemoryPage(PPOOL Pool, ULONG Tag);
void* SeAllocateMemory(ULONG NumberOfBytes, ULONG Tag);
void SeFreeMemory(void* VirtualAddress, ULONG Tag);