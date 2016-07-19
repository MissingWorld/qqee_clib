#ifndef MM_ARRAY_H
#define MM_ARRAY_H
#ifdef _cplusplus
extern "C"{
#endif

//
//双向循环链表实现动态数组、队列
//需要外部加锁
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//原子操作
#ifdef _WIN32
	#include <windows.h>
	#define LOCK_(x) while(InterlockedExchange(&(x),1)){}
	#define UNLOCK_(x) InterlockedExchange(&(x),0);
#else
	#define LOCK_(x) while(__sync_lock_test_and_set(&(x),1)){}
	#define UNLOCK_(x) __sync_lock_release(&(x));
#endif

struct mm_array_head{
    struct mm_array_head *next,*prev;
};
static inline void MM_ARRAY_INIT(struct mm_array_head *list){
	list->prev = list->next = list;
}
//Add Node
static inline void __list_add_mm(struct mm_array_head *new,struct mm_array_head *prev,struct mm_array_head *next){
	next->prev = new;
	new->next  = next;
	new->prev  = prev;
	prev->next = new;
}
static inline void MM_ARRAY_PUSH(struct mm_array_head *new, struct mm_array_head *head){
	__list_add_mm(new, head, head->next);
}
static inline void MM_ARRAY_PUSH_TAIL(struct mm_array_head *new, struct mm_array_head *head){
	__list_add_mm(new, head->prev, head);                            
}
//Pop Node
static inline void __list_del_mm(struct mm_array_head * prev, struct mm_array_head * next){
    next->prev = prev;
    prev->next = next;
}
static inline void MM_ARRAY_DEL(struct mm_array_head *entry){
	__list_del_mm(entry->prev, entry->next);
	//MM_ARRAY_INIT(entry);
}
static inline int MM_ARRAY_IS_EMPTY(const struct mm_array_head *head){
    return head->next == head;
}

/*
int lock;
typedef struct tag_gear_list_obj{
	struct mm_array_head list;
    uint8_t active_type;
    uint8_t socket_obj_arr_id;
}gear_list_obj;
//初始化头部
struct mm_array_head head;
MM_ARRAY_INIT(&head);
//添加
	gear_list_obj*tmp=(gear_list_obj*)malloc(sizeof(gear_list_obj));
	tmp->active_type=(15-i);
	tmp->socket_obj_arr_id=((15-i));
	LOCK_(lock)
	MM_ARRAY_PUSH(&(tmp->list),&head);
	UNLOCK_(lock)
//取出
	LOCK_(lock)
	gear_list_obj*tmps=(gear_list_obj*)(head.next);
	MM_ARRAY_DEL(head.next);
	UNLOCK_(lock)
	printf("%d\t%d\n",70-i,tmps->socket_obj_arr_id );//50
	free(tmps);
	if(MM_ARRAY_IS_EMPTY(&head))break;
*/

#ifdef _cplusplus
}
#endif
#endif