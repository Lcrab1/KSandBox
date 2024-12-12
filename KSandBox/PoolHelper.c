#include "PoolHelper.h"

static __int64 Pool_Alloc_Time = 0;
static __int64 PoolAllocateMemoryTime = 0;
static __int64 PoolAllocatePageTime = 0;
static __int64 Pool_Find_Cells_Time = 0;
static __int64 Pool_Get_Cells_Time = 0;
static __int64 Pool_Get_Cells_1_Time = 0;
static __int64 Pool_Get_Cells_2_Time = 0;

static __int64 Pool_Free_Time = 0;
static __int64 Pool_Free_Mem_Time = 0;
static __int64 Pool_Free_Cells_Time = 0;

void SePoolTiming(__int64* Timer)
{

}

PPOOL SeCreateMemoryPool(void)
{
	return SeCreateMemoryPoolTagged(POOL_TAG);
}
PPOOL SeCreateMemoryPoolTagged(ULONG Tag)
//Tag: POOL_TAG 'xoBS'
{
	//�������б�ǩ���ڴ��
	PPOOL Pool = NULL;
	PPAGE Page;
	UCHAR* Bitmap;
	ULONG cellIndex, byteIndex;
	ULONG Bit;

	//Ϊ��һҳ�����ڴ�
	Page = SeAllocateMemoryPage(NULL, Tag);
	if (!Page)
		return NULL;
	//����λͼ 
	Bitmap = (UCHAR*)Page + PAGE_HEADER_SIZE;
	memset(Bitmap, 0, PAGE_BITMAP_SIZE);

	cellIndex = NUMBER_PAGE_CELLS;	//��ȥ�ڴ��ͷ����λͼ֮��Ŀ��õ�Ԫ��
	while (1) {
		byteIndex = cellIndex / 8;	//cellIndex������λ�������
		Bit = 1 << (cellIndex & 7);
		if (byteIndex >= PAGE_BITMAP_SIZE)
			break;
		if (Bit == 1) {	//���cellIndex�ĺ�3λ��0
			Bitmap[byteIndex] = 0xFF;
			cellIndex += 8;
		}
		else {
			Bitmap[byteIndex] |= Bit;
			++cellIndex;
		}
	}
	Pool = (POOL*)((UCHAR*)Page + PAGE_HEADER_SIZE + PAGE_BITMAP_SIZE);
	Pool->Tag = Tag;
#ifdef POOL_USE_CUSTOM_LOCK

	Pool->PagesLock = LOCK_FREE;        //��
	Pool->LargeChunksLock = LOCK_FREE;  //��

#elif defined(KERNEL_MODE)

	Pool->Lock = ExAllocatePoolWithTag(
		NonPagedPool, sizeof(ERESOURCE), Tag);
	if (!Pool->Lock) {
		SeFreeMemory(Page, Tag);
		return NULL;
	}
	ExInitializeResourceLite(Pool->Lock);

#else /* ! KERNEL_MODE */

	InterlockedExchange(&Pool->ThreadIdentify, 0);
	InitializeCriticalSectionAndSpinCount(&Pool->Lock, 1000);
	//Lock��һ��ִ������Դ����

#endif
	memcpy(Pool->InitializeBitmap, Bitmap, PAGE_BITMAP_SIZE);

	INITIALIZE_LIST(&Pool->pages);
	SeInsertListBefore(&Pool->pages, NULL, Page);

	INITIALIZE_LIST(&Pool->full_pages);
	INITIALIZE_LIST(&Pool->large_chunks);

	//���pool�ṹ��ռ�õĵ�Ԫ
	//��������㷨���������Σ���һ�����ڳ�ʼ��λͼ����ǿ��õ�Ԫ���ڶ������ڱ��POOL�ṹ�屾��ռ�õĵ�Ԫ
	cellIndex = 0;
	while (cellIndex < NUM_CELLS(sizeof(POOL))) {
		byteIndex = cellIndex / 8;
		Bit = 1 << (cellIndex & 7);
		Bitmap[byteIndex] |= Bit;
		++cellIndex;
	}
	return Pool;
}

PPAGE SeAllocateMemoryPage(PPOOL Pool, ULONG Tag)
{
	//Pool:NULL
	//Tag: POOL_TAG 'xoBS'
	PPAGE allocatedPage;
	SePoolTiming(NULL);

	allocatedPage = (PAGE*)SeAllocateMemory(POOL_PAGE_SIZE, Tag);
	if (allocatedPage)
	{

		allocatedPage->Tag = Tag;
		allocatedPage->Flink = NULL;
		allocatedPage->num_free = NUMBER_PAGE_CELLS;

		if (Pool)
		{
			UCHAR* Bitmap = (UCHAR*)allocatedPage + PAGE_HEADER_SIZE;
			memcpy(Bitmap, Pool->InitializeBitmap, PAGE_BITMAP_SIZE);
			allocatedPage->Pool = Pool;
			SeInsertListBefore(&Pool->pages, NULL, allocatedPage);
		}
	}

	SePoolTiming(&PoolAllocatePageTime);


	return allocatedPage;
}

void* SeAllocateMemory(ULONG NumberOfBytes, ULONG Tag)
{
	void* allocatedMemory;

	SePoolTiming(NULL);


#ifdef KERNEL_MODE
	allocatedMemory = ExAllocatePoolWithTag(PagedPool, NumberOfBytes, Tag);
#else
	allocatedMemory = VirtualAlloc(0, NumberOfBytes, MEM_RESERVE | MEM_COMMIT | MEM_TOP_DOWN,
		((UCHAR)Tag == 0xFF ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE));
#endif


	SePoolTiming(&PoolAllocateMemoryTime);

	return allocatedMemory;
}

void SeFreeMemory(void* VirtualAddress, ULONG Tag)
{

#ifdef KERNEL_MODE
	ExFreePoolWithTag(VirtualAddress, Tag);
#else
	if (!VirtualFree(VirtualAddress, 0, MEM_RELEASE)) {
		RaiseException(
			STATUS_ACCESS_VIOLATION,
			EXCEPTION_NONCONTINUABLE_EXCEPTION, 0, NULL);
		ExitProcess(-1);
	}
#endif
}

#ifdef POOL_USE_CUSTOM_LOCK


static const WCHAR* Pool_PagesLock_Name = L"PagesLock";
static const WCHAR* Pool_LargeChunksLock_Name = L"LargeChunksLock";


//#define POOL_LOCK(lock)
//#define POOL_UNLOCK(lock)

#define POOL_DECLARE_IRQL


#if 1

#if defined(KERNEL_MODE)
#undef  POOL_DECLARE_IRQL
#define POOL_DECLARE_IRQL KIRQL Irql;                        //Interrupt request Level
#define POOL_RAISE_IRQL KeRaiseIrql(APC_LEVEL, &Irql);       //
#define POOL_LOWER_IRQL KeLowerIrql(Irql);                   //
#else
#define POOL_RAISE_IRQL
#define POOL_LOWER_IRQL
#endif

#define POOL_LOCK(Lock)                                 \
    POOL_RAISE_IRQL                                     \
    Lock_Exclusive(&Pool->Lock, Pool_##Lock##_Name);

#define POOL_UNLOCK(Lock)                               \
    Lock_Unlock(&Pool->Lock, Pool_##Lock##_Name);       \
    POOL_LOWER_IRQL

#endif


#elif defined(KERNEL_MODE)


#define POOL_DECLARE_IRQL KIRQL Irql;
#define POOL_LOCK(dummylockname)                        \
    KeRaiseIrql(APC_LEVEL, &Irql);                      \
    ExAcquireResourceExclusiveLite(Pool->Lock, TRUE);
#define POOL_UNLOCK(dummylockname)                      \
    ExReleaseResourceLite(Pool->Lock);                  \
    KeLowerIrql(Irql);


#else /* ! KERNEL_MODE */

#define POOL_DECLARE_IRQL                               \
    const LONG ThreadIdentify = (LONG)GetCurrentThreadId();       \
    BOOLEAN IsLocked;

#define POOL_LOCK(dummylockname)                        \
    if (InterlockedCompareExchange(&Pool->ThreadIdentify, ThreadIdentify, 0) != ThreadIdentify) {   \
        IsLocked = TRUE;                                  \
        EnterCriticalSection(&Pool->Lock);              \
        Pool->ThreadIdentify = ThreadIdentify;                            \
    } else                                              \
        IsLocked = FALSE;

#define POOL_UNLOCK(dummylockname)                      \
    if (IsLocked) {                                       \
        Pool->ThreadIdentify = ThreadIdentify;                            \
        LeaveCriticalSection(&Pool->Lock);              \
        InterlockedExchange(&Pool->ThreadIdentify, 0);          \
    }

#endif