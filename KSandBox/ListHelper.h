#pragma once

#include <fltKernel.h>


typedef struct _NODE_ {
	struct _NODE_* Flink;
	struct _NODE_* Blink;
}NODE, * PNODE;

typedef struct _LIST_
{
	PNODE Head;
	PNODE Tail;
	int Count;
#ifdef LIST_WITH_MEMORY_MANAGEMENT
	//POOL *pool;
	//int elem_len;
#endif
} LIST, * PLIST;



#ifndef LIST_WITH_MEMORY_MANAGEMENT
#define INITIALIZE_LIST(PList) (memset((PList),0,sizeof(LIST)))
#endif

#define GET_LIST_HEAD(PList) ((void *)(((LIST *)(PList))->Head))
#define GET_LIST_TAIL(PList) ((void *)(((LIST *)(PList))->Tail))
#define GET_LIST_COUNT(PList)  (((LIST *)(PList))->Count)
#define GET_LIST_FLINK(PList) ((void *)(((NODE*)(PList))->Flink))
#define GET_LIST_BLINK(PList) ((void *)(((NODE*)(PList))->Blink))
