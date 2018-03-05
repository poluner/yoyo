#ifndef _VOD_MEM_H_
#define _VOD_MEM_H_
#ifdef __cplusplus
extern "C"
{
#endif
#include "utility/define.h"

_int32 VodNewMem_customed_interface(_int32 fun_idx, void *fun_ptr);
_int32 VodNewMem_get_mem_from_os(_u32 memsize, void **mem);
_int32 VodNewMem_free_mem_to_os(void* mem, _u32 memsize);

#ifdef _EXTENT_MEM_FROM_OS
_int32 VodNewMem_get_extent_mem_from_os(_u32 memsize, void **mem);
_int32 VodNewMem_free_extent_mem_to_os(void* mem, _u32 memsize);
#endif
#ifdef __cplusplus
}
#endif
#endif
