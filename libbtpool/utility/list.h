#ifndef _SD_LIST_H_00138F8F2E70_200806260949
#define _SD_LIST_H_00138F8F2E70_200806260949

#ifdef __cplusplus
extern "C"
{
#endif

/* NO Thread-Safe */

#include "utility/errcode.h"
#include "utility/define.h"
#include "utility/sd_string.h"



typedef struct
{
    uint32_t _slip_size;
    char *_free_index;
} SLAB, *pSLAB;


typedef struct t_list_node
{
    void *_data;
    struct t_list_node *_pre_node;
    struct t_list_node *_nxt_node;
} LIST_NODE, *pLIST_NODE;

typedef struct tagLIST
{
    LIST_NODE _list_nil; /* not use pointer avoid to calling list_uninit() */
    uint32_t _list_size;
} LIST;

typedef pLIST_NODE LIST_ITERATOR;

#define LIST_BEGIN(list)		((list)._list_nil._nxt_node)
#define LIST_END(list)			(&(list)._list_nil)
#define LIST_NEXT(iterator)		((iterator)->_nxt_node)

#define LIST_RBEGIN(list)		((list)._list_nil._pre_node)
#define LIST_PRE(iterator)		((iterator)->_pre_node)

#define	LIST_VALUE(iterator)	((iterator)->_data)

typedef int32_t (*data_comparator)(void *E1, void *E2);




int32_t mpool_create_slab_impl_new(uint32_t slip_size, uint32_t min_slip_count, uint32_t invalid_offset, SLAB **slab, const char* ref_file, int32_t line);
#define mpool_create_slab(slip_size, min_slip_count, invalid_offset, ppslab) mpool_create_slab_impl_new(slip_size, min_slip_count, invalid_offset, ppslab, __FILE__, __LINE__)



int32_t mpool_free_slip_impl_new(SLAB *slab, void *slip, const char* ref_file, int32_t line);
#define mpool_free_slip(slab, slip) mpool_free_slip_impl_new(slab, slip, __FILE__, __LINE__)



int32_t mpool_get_slip_impl_new(SLAB *slab, const char* ref_file, int32_t line, void **slip);
#define mpool_get_slip(slab, slip) mpool_get_slip_impl_new(slab, __FILE__, __LINE__, slip)

int32_t mpool_destory_slab(SLAB *slab);


int32_t list_alloctor_init(void);
int32_t list_alloctor_uninit(void);

/* @Simple Function@
 * Return : void
 */
void list_init(LIST *list);

/* @Simple Function@
 * Return : the size of list
 */
uint32_t list_size(const LIST *list);


int32_t list_push(LIST *list, void *data);

/*
 * Return immediately, even if there were not any node popped.
 * If no node popped, the *data will be NULL.
 */
int32_t list_pop(LIST *list, void **data);

int32_t list_insert(LIST *list, void *data, LIST_ITERATOR insert_before);
int32_t list_erase(LIST *list, LIST_ITERATOR erase_node);


/* @Simple Function@
 * swap the head&tail of two LIST
 * Return : void
 */
void list_swap(LIST *list1, LIST *list2);


/* @Simple Function@
 * cat list2 to list1, and clear the list2
 * Return : void
 */
void list_cat_and_clear(LIST *list1, LIST *list2);


/* clear all node of list, the caller is responsable for destruction of elements */
int32_t list_clear(LIST *list);

#ifdef __cplusplus
}
#endif

#endif
