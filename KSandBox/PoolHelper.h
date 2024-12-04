#pragma once
#include <fltKernel.h>
#include "LockHelper.h"
#include "ListHelper.h"

#define		KERNEL_MODE
#define		POOL_TAG 'xoBS'   


typedef struct _POOL_  POOL;
typedef struct _POOL_* PPOOL;



PPOOL SeCreateMemoryPool(void);
PPOOL SeCreateMemoryPoolTagged(ULONG Tag);