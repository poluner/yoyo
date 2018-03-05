#include "utility/list.h"
#include "utility/errcode.h"
#include "utility/sd_string.h"

static SLAB *gp_listslab = NULL;


int32_t mpool_create_slab_impl_new(uint32_t slip_size, uint32_t min_slip_count, uint32_t invalid_offset, SLAB **slab, const char* ref_file, int32_t line)
{
    *slab = (SLAB*)calloc(1, sizeof(SLAB));
    if (NULL == *slab)
    {
        return OUT_OF_MEMORY;
    }
    (*slab)->_slip_size = slip_size;

    return SUCCESS;
}


int32_t mpool_get_slip_impl_new(SLAB *slab, const char* ref_file, int32_t line, void **slip)
{
    *slip = malloc(slab->_slip_size);
    if (NULL == *slip)
    {
        return OUT_OF_MEMORY;
    }

    return SUCCESS;
}


int32_t mpool_free_slip_impl_new(SLAB *slab, void *slip, const char* ref_file, int32_t line)
{
    free(slip);
    return SUCCESS;
}


int32_t mpool_destory_slab(SLAB *slab)
{
    free(slab);

    return SUCCESS;
}


int32_t list_alloctor_init(void)
{
    int32_t ret_val = SUCCESS;

    if(!gp_listslab)
    {
        ret_val = mpool_create_slab(sizeof(LIST_NODE), MIN_LIST_MEMORY, 0, &gp_listslab);
        CHECK_VALUE(ret_val);
    }

    return ret_val;
}

int32_t list_alloctor_uninit(void)
{
    int32_t ret_val = SUCCESS;

    if(gp_listslab)
    {
        ret_val = mpool_destory_slab(gp_listslab);
        CHECK_VALUE(ret_val);
        gp_listslab = NULL;
    }

    return ret_val;
}

void list_init(LIST *list)
{
    sd_memset(list, 0, sizeof(LIST));

    list->_list_nil._nxt_node = list->_list_nil._pre_node = &list->_list_nil;
}

uint32_t list_size(const LIST *list)
{
    if(list)
        return list->_list_size;
    else
        return 0;
}

int32_t list_push(LIST *list, void *data)
{
    return list_insert(list, data, &(list->_list_nil));
}

int32_t list_pop(LIST *list, void **data)
{
    int32_t ret_val = SUCCESS;
    LIST_NODE *pnode = NULL;

    *data = NULL;

    if(list->_list_size > 0)
    {
        pnode = (LIST_NODE*)list->_list_nil._nxt_node;
        *data = (void*)pnode->_data;

        ret_val = list_erase(list, pnode);
        CHECK_VALUE(ret_val);
    }

    return ret_val;
}

int32_t list_insert(LIST *list, void *data, LIST_ITERATOR insert_before)
{
    int32_t ret_val = SUCCESS;
    LIST_NODE *new_node = NULL;

    ret_val = mpool_get_slip(gp_listslab, (void**)&new_node);
    CHECK_VALUE(ret_val);

    sd_memset(new_node, 0, sizeof(LIST_NODE));

    new_node->_data = data;

    new_node->_nxt_node = insert_before;
    new_node->_pre_node = insert_before->_pre_node;
    insert_before->_pre_node = new_node;
    new_node->_pre_node->_nxt_node = new_node;

    list->_list_size ++;

    return ret_val;
}

int32_t list_erase(LIST *list, LIST_ITERATOR erase_node)
{
    if (erase_node == &list->_list_nil)
    {
        return INVALID_ITERATOR;
    }

    erase_node->_nxt_node->_pre_node = erase_node->_pre_node;
    erase_node->_pre_node->_nxt_node = erase_node->_nxt_node;

    int32_t ret_val = mpool_free_slip(gp_listslab, erase_node);
    CHECK_VALUE(ret_val);

    list->_list_size --;

    return ret_val;
}

int32_t list_clear(LIST *list)
{
    int32_t ret_val = SUCCESS;
    if(list == NULL || list->_list_size == 0)
    {
        return SUCCESS;
    }
    LIST_NODE *node = (LIST_NODE *)list->_list_nil._nxt_node;

    for (; node != &list->_list_nil;)
    {
        node = (LIST_NODE*)node->_nxt_node;
        ret_val = mpool_free_slip(gp_listslab, (LIST_NODE *)node->_pre_node);
        CHECK_VALUE(ret_val);
    }

    list->_list_size = 0;
    list->_list_nil._nxt_node = list->_list_nil._pre_node = &list->_list_nil;
    return ret_val;
}

void list_swap(LIST *list1, LIST *list2)
{
    LIST_NODE tmp_node;
    int32_t tmp_size = 0;

    sd_memcpy(&tmp_node, &list1->_list_nil, sizeof(LIST_NODE));
    sd_memcpy(&list1->_list_nil, &list2->_list_nil, sizeof(LIST_NODE));
    sd_memcpy(&list2->_list_nil, &tmp_node, sizeof(LIST_NODE));

    tmp_size = list1->_list_size;
    list1->_list_size = list2->_list_size;
    list2->_list_size = tmp_size;

    if(list1->_list_size > 0)
    {
        list1->_list_nil._nxt_node->_pre_node = &list1->_list_nil;
        list1->_list_nil._pre_node->_nxt_node = &list1->_list_nil;
    }
    else
    {
        list1->_list_nil._nxt_node = list1->_list_nil._pre_node = &list1->_list_nil;
    }

    if(list2->_list_size > 0)
    {
        list2->_list_nil._nxt_node->_pre_node = &list2->_list_nil;
        list2->_list_nil._pre_node->_nxt_node = &list2->_list_nil;
    }
    else
    {
        list2->_list_nil._nxt_node = list2->_list_nil._pre_node = &list2->_list_nil;
    }
}

void list_cat_and_clear(LIST *list1, LIST *list2)
{
    if (list2->_list_size > 0)
    {
        list1->_list_nil._pre_node->_nxt_node = list2->_list_nil._nxt_node;
        list2->_list_nil._nxt_node->_pre_node = list1->_list_nil._pre_node;

        list1->_list_nil._pre_node = list2->_list_nil._pre_node;
        list2->_list_nil._pre_node->_nxt_node = &list1->_list_nil;

        list1->_list_size += list2->_list_size;

        /* clear list2 */
        list2->_list_nil._nxt_node = list2->_list_nil._pre_node = &list2->_list_nil;
        list2->_list_size = 0;
    }
}
