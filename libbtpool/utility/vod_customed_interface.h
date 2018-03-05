#ifndef _VOD_CUSTOMED_INTERFACE_H_
#define _VOD_CUSTOMED_INTERFACE_H_
#ifdef __cplusplus
extern "C"
{
#endif
#include "utility/define.h"

_int32 VodNewCustomedInterface_set_ptr(_int32 fun_idx, void *fun_ptr);

BOOL VodNewCustomedInterface_is_available(_int32 fun_idx);
void* VodNewCustomedInterface_get_ptr(_int32 fun_idx);

#define VCI_IDX_COUNT			    (25)

/* function index */
#define VCI_FS_IDX_OPEN           (0)
#define VCI_FS_IDX_ENLARGE_FILE   (1)
#define VCI_FS_IDX_CLOSE          (2)
#define VCI_FS_IDX_READ           (3)
#define VCI_FS_IDX_WRITE          (4)
#define VCI_FS_IDX_PREAD          (5)
#define VCI_FS_IDX_PWRITE         (6)
#define VCI_FS_IDX_FILEPOS        (7)
#define VCI_FS_IDX_SETFILEPOS     (8)
#define VCI_FS_IDX_FILESIZE       (9)
#define VCI_FS_IDX_FREE_DISK      (10)

#define VCI_SOCKET_IDX_SET_SOCKOPT  (11)

#define VCI_MEM_IDX_GET_MEM           (12)
#define VCI_MEM_IDX_FREE_MEM          (13)
#define VCI_ZLIB_UNCOMPRESS          (14)
#define VCI_FS_IDX_GET_FILESIZE_AND_MODIFYTIME   (15)
#define VCI_FS_IDX_DELETE_FILE     (16)
#define VCI_FS_IDX_RM_DIR              (17)
#define VCI_FS_IDX_MAKE_DIR              (18)
#define VCI_FS_IDX_RENAME_FILE              (19)
#define VCI_SOCKET_IDX_CREATE  (20)
#define VCI_SOCKET_IDX_CLOSE  (21)
#define VCI_FS_IDX_FILE_EXIST          (22)
#define VCI_DNS_GET_DNS_SERVER          (23)
#define VCI_LOG_WRITE_LOG                (24)

typedef _int32 (*vci_fs_open)(char *filepath, _int32 flag, _u32 *file_id);
typedef _int32 (*vci_fs_enlarge_file)(_u32 file_id, _u64 expect_filesize, _u64 *cur_filesize);

typedef _int32 (*vci_fs_close)(_u32 file_id);

typedef _int32 (*vci_fs_read)(_u32 file_id, char *buffer, _int32 size, _u32 *readsize);
typedef _int32 (*vci_fs_write)(_u32 file_id, char *buffer, _int32 size, _u32 *writesize);

typedef _int32 (*vci_fs_pread)(_u32 file_id, char *buffer, _int32 size, _u64 filepos, _u32 *readsize);
typedef _int32 (*vci_fs_pwrite)(_u32 file_id, char *buffer, _int32 size, _u64 filepos, _u32 *writesize);

typedef _int32 (*vci_fs_filepos)(_u32 file_id, _u64 *filepos);
typedef _int32 (*vci_fs_setfilepos)(_u32 file_id, _u64 filepos);

typedef _int32 (*vci_fs_filesize)(_u32 file_id, _u64 *filesize);

typedef _int32 (*vci_fs_get_free_disk)(const char *path, _u32 *free_size);


/* socket */
typedef _int32 (*vci_socket_set_sockopt)(_u32 socket, _int32 socket_type);

typedef _int32 (*vci_socket_create)(_int32 domain, _int32 type, _int32 protocol, _u32 *sock);
typedef _int32 (*vci_socket_close)(_u32 socket);

/* memory */
typedef _int32 (*vci_mem_get_mem)(_u32 memsize, void **mem);
typedef _int32 (*vci_mem_free_mem)(void* mem, _u32 memsize);

typedef BOOL (*vci_fs_file_exist)(const char *filepath);

typedef _int32 (*vci_zlib_uncompress)( unsigned char *p_out_buffer, int *p_out_len, const unsigned char *p_in_buffer, int in_len );

typedef _int32 (*vci_fs_get_file_size_and_modified_time)(const char * filepath,_u64 * p_file_size,_u32 * p_last_modified_time);

typedef _int32 (*vci_fs_rmdir)(const char *dirpath);

typedef _int32 (*vci_fs_makedir)(const char *dirpath);

typedef _int32 (*vci_fs_delete_file)(const char *filepath);

typedef _int32 (*vci_fs_rename_file)(const char *filepath, const char *new_filepath);


typedef _int32 (*vci_dns_get_dns_server)(char *dns1, char* dns2);

typedef _int32 (*vci_log_write_log)(_u32 file_id, char *buffer, _int32 size, _u32 *writesize);

#ifdef __cplusplus
}
#endif
#endif

