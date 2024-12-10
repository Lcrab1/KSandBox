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

#endif
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

