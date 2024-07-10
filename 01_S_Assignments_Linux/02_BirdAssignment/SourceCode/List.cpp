#include<stdio.h>
#include<stdlib.h>
#include "List.h"

list_t* CreateList(void)
{
	list_t *pList = GetNode(vmath::mat4::identity());
	pList->pNext = pList->pPrev = pList;
	return pList;
}

int InsertAtBegining(list_t *pList, data_t new_data)
{
	g_Insert(pList, GetNode(new_data), pList->pNext);
	return LST_SUCCESS;
}

int InsertAtEnd(list_t *pList, data_t new_data)
{
	g_Insert(pList->pPrev, GetNode(new_data), pList);
	return LST_SUCCESS;
}

int InsertAfterData(list_t *pList, data_t existing_data, data_t new_data)
{
	node_t *e_node = SearchNode(pList, existing_data);
	if (!e_node)
		return DATA_NOT_FOUND;
	 
	g_Insert(e_node,GetNode(new_data),e_node->pNext);
	return LST_SUCCESS;
}

int InsertBeforeData(list_t *pList, data_t exsisting_data, data_t new_data)
{
	node_t *e_node = SearchNode(pList, exsisting_data);
	if (!e_node)
		return DATA_NOT_FOUND;

	g_Insert(e_node->pPrev, GetNode(new_data), e_node);
	return LST_SUCCESS;
}

int DeleteBegining(list_t *pList)
{
	if (IsEmpty(pList))
		return LIST_EMPTY;

	g_Delete(pList->pNext);
	return LST_SUCCESS;
}

int DeleteEnd(list_t *pList)
{
	if (IsEmpty(pList))
		return LIST_EMPTY;

	g_Delete(pList->pPrev);
	return LST_SUCCESS;
}

int DeleteData(list_t *pList, data_t existing_data)
{
	node_t *e_node = SearchNode(pList, existing_data);
	if (!e_node)
		return DATA_NOT_FOUND;
	g_Delete(e_node);
	return LST_SUCCESS;
}

int ExamineBegining(list_t *pList, data_t *pData)
{
	if (IsEmpty(pList))
		return LIST_EMPTY;
	*pData = pList->pNext->data;
	return LST_SUCCESS;
}

int ExamineEnd(list_t *pList, data_t *pData)
{
	if (IsEmpty(pList))
		return LIST_EMPTY;
	*pData = pList->pPrev->data;
	return LST_SUCCESS;
}

int ExamineAndDeleteBegining(list_t *pList, data_t *pData)
{
	if (IsEmpty(pList))
		return LIST_EMPTY;
	*pData = pList->pNext->data;
	g_Delete(pList->pNext);
	return LST_SUCCESS;
}

int ExamineAndDeleteEnd(list_t *pList, data_t *pData)
{
	if (IsEmpty(pList))
		return LIST_EMPTY;
	*pData = pList->pPrev->data;
	g_Delete(pList->pPrev);
	return LST_SUCCESS;
}

int FindData(list_t *pList, data_t pData)
{
	node_t *f_node = SearchNode(pList,pData);
	if (f_node)
		return LST_TRUE;
	return LST_FALSE;
}

void DisplayList(list_t *pList)
{
	node_t *pRun = NULL;
	for (pRun = pList->pNext; pRun != pList; pRun = pRun->pNext) 
	{
	}
}

inline bool_t IsEmpty(list_t *pList)
{
	return ((pList->pNext == pList)&&(pList->pPrev == pList));
}

len_t LengthList(list_t * pList)
{
	node_t *pRun = NULL;
	int iLength = 0;

	for (pRun = pList->pNext; pRun != pList; pRun = pRun->pNext, ++iLength);

	return iLength;
}

data_t* ToArray(list_t *pList, len_t *pLength)
{
	len_t iListLength = LengthList(pList);
	data_t *pArr = NULL;
	node_t *pRun = NULL;

	int i = 0;
	if (iListLength <= 0)
		return(NULL);
	pArr = (data_t*)XCalloc(iListLength, sizeof(data_t));

	for (pRun = pList->pNext, i = 0; pRun != pList; pRun = pRun->pNext, i++)
		pArr[i] = pRun->data;
	*pLength = iListLength;

	return pArr;
}

list_t* ToList(data_t *p_arr, len_t len)
{
	list_t *new_list = CreateList();
	int i = 0;
	for (i = 0; i < len; i++)
		InsertAtEnd(new_list, p_arr[i]);

	return new_list;
}

int DestroyList(list_t **ppList)
{
	list_t *pList = *ppList;
	node_t *pRun, *pRun_Next;

	for (pRun = pList->pNext; pRun != pList; pRun = pRun_Next)
	{
		pRun_Next = pRun->pNext;
		free(pRun);
	}
	free(pList);
	*ppList = NULL;
	return(LST_SUCCESS);
}

//List Auxillary Routines Start
static node_t* GetNode(data_t new_data)
{
	node_t *newNode = (node_t*)XCalloc(1, sizeof(node_t));
	newNode->data = new_data;
	return newNode;
}

void g_Insert(node_t *pBeg,node_t *pMid, node_t *pEnd)
{
	pMid->pNext = pEnd;
	pMid->pPrev = pBeg;
	pBeg->pNext = pMid;
	pEnd->pPrev = pMid;
}

void g_Delete(node_t *pNode)
{
	pNode->pNext->pPrev = pNode->pPrev;
	pNode->pPrev->pNext = pNode->pNext;
	free(pNode);
}

static node_t* SearchNode(list_t *pList, data_t search_data)
{
	node_t *run_node;

	for (run_node = pList->pNext; run_node!=pList; run_node= run_node->pNext)
	{
		if (run_node->data == search_data)
		{
			return run_node;
		}
	}
	return NULL;
}

//Auxillary Routines Start
static void* XCalloc(int nr_Elements, int iSizePerElement)
{
	void *ptr = NULL;
	ptr = calloc(nr_Elements, iSizePerElement);
	if (ptr==NULL)
	{
		printf("xcalloc:fatal:out of memory \n");
		exit(EXIT_FAILURE);
	}
	return ptr;
}

