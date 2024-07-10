#ifndef _LIST_H
#include"vmath.h"
#define _LIST_H

#define DATA_NOT_FOUND -1
#define LIST_EMPTY -2
#define LST_SUCCESS 1
#define LST_FAILURE  0
#define LST_TRUE 1
#define LST_FALSE 0

typedef vmath::mat4 data_t;
struct stNode
{
	data_t data;
	struct stNode *pPrev, *pNext;
}; 
typedef struct stNode	node_t;
typedef node_t list_t;
typedef int len_t;
typedef int flag_t;
typedef int bool_t;

//Interface Routines Start
list_t* CreateList(void);
int InsertAtBegining(list_t *pList,data_t new_data);
int InsertAtEnd(list_t *pList,data_t new_data);
int InsertBeforeData(list_t *pList, data_t exsisting_data,data_t new_data);
int InsertAfterData(list_t *pList, data_t existing_data, data_t new_data);
int DeleteBegining(list_t *pList);
int DeleteEnd(list_t *pList);
int DeleteData(list_t *pList,data_t existing_data);

int ExamineBegining(list_t *pList, data_t *pData);
int ExamineEnd(list_t *pList, data_t *pData);
int ExamineAndDeleteBegining(list_t *pList,data_t *pData);
int ExamineAndDeleteEnd( list_t *pList, data_t *pData);

int FindData(list_t *pList, data_t pData);
void DisplayList(list_t *pList);
inline bool_t IsEmpty(list_t *pList);
len_t LengthList(list_t *pList);

data_t* ToArray(list_t *pList,len_t *pLength);
list_t* ToList(data_t *p_arr, len_t len);

int DestroyList(list_t **ppList);

//List Auxillary Routines Start
static void g_Insert(node_t *pBeg,node_t *pMid,node_t *pEnd);
static void g_Delete(node_t *pNode);
static node_t* SearchNode(list_t *pList, data_t search_data);
static node_t* GetNode(data_t new_data);

//Auxillary Routines Start
static void* XCalloc(int nr_Elements, int iSizePerElement);
#endif 
