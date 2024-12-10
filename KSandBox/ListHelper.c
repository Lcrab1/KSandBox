#include "ListHelper.h"

void SeInsertListBefore(PLIST List, void* nextNode, void* newNode)
{
	NODE* previousNode, * nextNodeCast, * currentHead, * newNodeCast;

	nextNodeCast = (NODE*)nextNode;
	newNodeCast = (NODE*)newNode;
	++List->Count;

	if (nextNodeCast == List->Head || nextNodeCast == NULL)
	{
		currentHead = List->Head;
		List->Head = newNodeCast;
		newNodeCast->Blink = NULL;
		newNodeCast->Flink = currentHead;
		if (currentHead == NULL)
		{
			List->Tail = newNodeCast;
		}
		else
		{
			currentHead->Blink = newNodeCast;
		}
	}
	else
	{
		previousNode = nextNodeCast->Blink;
		previousNode->Flink = newNodeCast;
		newNodeCast->Blink = previousNode;
		nextNodeCast->Flink = newNodeCast;
		newNodeCast->Flink = nextNodeCast;
	}
}