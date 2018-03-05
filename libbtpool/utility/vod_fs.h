#ifndef _VOD_FS_H_
#define _VOD_FS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "utility/define.h"

/* ------------------------------------------------------ */

#define VOD_FS_MAX_PATH	(256)

#define VOD_FS_INVALID_FILE_ID	(0xFFFFFFFF)

/* whether if create file while it not exist. */
#define O_VOD_FS_CREATE		(0x1)
/* read and write (default) */
#define O_VOD_FS_RDWR		(0x0)
/* read only. */
#define O_VOD_FS_RDONLY		(0x2)
/* write only. */
#define O_VOD_FS_WRONLY		(0x4)
#define O_VOD_FS_MASK       (0xFF)

#if defined(MSTAR)
#define VOD_FILE_ENLARGE_UNIT	(1024 * 1024*1024)
#elif defined(LINUX)
#if defined(_ANDROID_LINUX)
#define VOD_FILE_ENLARGE_UNIT	(1024 * 1024)
#define VOD_FILE_ENLARGE_WRITE_UNIT	(1024 * 1024)
#else
#define VOD_FILE_ENLARGE_UNIT	(1024 * 1024)
#endif
#elif defined( __SYMBIAN32__)
#define VOD_FILE_ENLARGE_UNIT	(1024 * 1024)
#elif defined(WINCE)
#define VOD_FILE_ENLARGE_UNIT	(1024 * 1024)
#elif defined(SUNPLUS)
#define VOD_FILE_ENLARGE_UNIT	(1024 * 1024)
#elif defined(AMLOS)
#define VOD_FILE_ENLARGE_UNIT	(1024 * 128)
#else
#define VOD_FILE_ENLARGE_UNIT	(1024 * 1024)
#endif

/* 当前系统支持的目录分隔符 */
#if defined(WINCE) || defined(__SYMBIAN32__)
#define VOD_DIR_SPLIT_CHAR   '\\'
#define VOD_DIR_SPLIT_STRING "\\"
#else
#define VOD_DIR_SPLIT_CHAR   '/'
#define VOD_DIR_SPLIT_STRING "/"
#endif

char VodNewFs_format_dir_split_char(char c);

/* ------------------------------------------------------ */

#define VOD_FILE_ATTRIB_READABLE (0x0001)
#define VOD_FILE_ATTRIB_WRITEABLE (0x0002)
#define VOD_FILE_ATTRIB_EXECABLE (0x0004)

typedef struct tag_file_attrib
{
    char _name[MAX_FILE_NAME_LEN];
    BOOL _is_dir;
    _u32 _attrib;
} VOD_FILE_ATTRIB;

/* ------------------------------------------------------ */

_int32 VodNewFs_filepos(_u32 file_id, _u64 *filepos);
_int32 VodNewFs_setfilepos(_u32 file_id, _u64 filepos);
_int32 VodNewFs_filesize(_u32 file_id, _u64 *filesize);
_int32 VodNewFs_open_ex(const char *filepath, _int32 flag, _u32 *file_id);

/* VodNewFs_enlarge_file
 * 只允许扩大文件，调用此函数后，filepos位置不确定，请慎用！
 * （配合VodNewFs_pread/VodNewFs_pwrite使用，或者事先保存filepos）
 */
_int32 VodNewFs_enlarge_file(_u32 file_id, _u64 expect_filesize, _u64 *cur_filesize);
_int32 VodNewFs_close_ex(_u32 file_id);
_int32 VodNewFs_read(_u32 file_id, char *buffer, _int32 size, _u32 *readsize);
_int32 VodNewFs_write(_u32 file_id, char *buffer, _int32 size, _u32 *writesize);
_int32 VodNewFs_write_no_sync(_u32 file_id, char *buffer, _int32 size, _u32 *writesize);
_int32 VodNewFs_pread(_u32 file_id, char *buffer, _int32 size, _u64 filepos, _u32 *readsize);
_int32 VodNewFs_pwrite(_u32 file_id, char *buffer, _int32 size, _u64 filepos, _u32 *writesize);
_int32 VodNewFs_append(const char *filepath, char *buffer, _u32 size);
_int32 VodNewFs_copy_file(const char *filepath, const char *new_filepath);
_int32 VodNewFs_move_file(const char *filepath, const char *new_filepath);
_int32 VodNewFs_rename_file(const char *filepath, const char *new_filepath);
_int32 VodNewFs_delete_file(const char *filepath);
/* 删除整个目录 */
_int32 VodNewFs_delete_dir(const char *path);

/* VodNewFs_file_exist
 * Return : whether if filepath exist
 */
BOOL VodNewFs_file_exist(const char *filepath);

/* VodNewFs_dir_exist
 * Return : whether if dirpath is a exist dir
 */
BOOL VodNewFs_dir_exist(const char *dirpath);

_int32 VodNewFs_ensure_path_exist(const char* path);

_int32 VodNewFs_recursive_mkdir(char *dirpath);

/* create directory recursively */
_int32 VodNewFs_mkdir(const char *dirpath);

/* failed if the directory is not empty */
_int32 VodNewFs_rmdir(const char *dirpath);

/* failed if the file or directory in directory is in use */
_int32 VodNewFs_recursive_rmdir(const char *dirpath);

/* 获取子目录/文件的个数与名称。
 * 如果sub_files为NULL，或sub_files_size=0，仅计算子目录的个数。
 */
_int32 VodNewFs_get_sub_files(const char *dirpath, VOD_FILE_ATTRIB sub_files[], _u32 sub_files_size, _u32 *p_sub_files_count);


/* 获取目录内所有子文件(不进入子文件夹)的总大小
 */
_int32 VodNewFs_get_sub_files_total_size(const char *dirpath,  _u64 * total_size);

/* Get file size and last modified time without opening the file */
_int32 VodNewFs_get_file_size_and_modified_time(const char * filepath,_u64 * p_file_size,_u32 * p_last_modified_time);

/* get free disk-space about path
 *  unit of free_size : K (1024 bytes)
 */
_int32 VodNewFs_get_free_disk(const char *path, _u32 *free_size);

/* get total disk-space about path
 *  unit of total_size : K (1024 bytes)
 */
_int32 VodNewFs_get_disk_space(const char *path, _u32 *total_size);

/* Check if the file is readable */
BOOL VodNewFs_is_file_readable(const char * filepath);
_int32 VodNewFs_test_path_writable(const char *path);

BOOL VodNewFs_is_file_name_valid(const char * filename);



/* Save the being writen data to buffer for reducing VodNewFs_write operations  */
_int32 VodNewFs_write_save_to_buffer(_u32 file_id, char *buffer, _u32 buffer_len,_u32 *buffer_pos, char * p_data,_u32 data_len);

/* 格式化文件路径中的\和/
 * 传入path编码必须兼容ASCII,例如GBK
**/
_int32 VodNewFs_format_filepath(const char *path, char *formated_path,
                                _u32 formated_path_size, _u32 *p_formated_path_len);

/* 格式化目录路径中的\和/，最后以分隔符结尾
* 传入path编码必须兼容ASCII,例如GBK
**/
_int32 VodNewFs_format_dirpath(const char *path, char *formated_path,
                               _u32 formated_path_size, _u32 *p_formated_path_len);

/* 对path进行转码，适应不同的平台
 * 其中symbian  -> GBK; WINCE ->Unicode; Android ->UTF8 , 其他不处理
**/
_int32 VodNewFs_conv_path(const char *input,_u32 input_len, char* output,_u32 *output_len);

/*格式化文件路径中的\和/，并针对不同平台进行转码
**/
_int32 VodNewFs_format_conv_filepath(const char *path, char *formated_path,
                                     _u32 formated_path_size, _u32 *p_formated_path_len);

/* 格式化目录路径中的\和/，并针对不同平台进行转码
**/
_int32 VodNewFs_format_conv_dirpath(const char *path, char *formated_path,
                                    _u32 formated_path_size, _u32 *p_formated_path_len);

/* ------------------------------------------------------ */
/* 文件虚拟路径
 * 虚路径格式: VOD_DIR_SPLIT_CHAR + 实路径, 如: "/a/b/c", "\c:\a\b\"。
 */

/* 是否真实文件路径 */
BOOL VodNewFs_is_realdir(const char *dir);

/* 文件虚路径转为实路径 */
_int32 VodNewFs_vdir2realdir(const char *vdir, char *realdir, _u32 realdir_size, _u32 *p_realdir_len);

/* 文件实路径转为虚路径,不支持相对路径 */
_int32 VodNewFs_realdir2vdir(const char *realdir, char *vdir, _u32 vdir_size, _u32 *p_vdir_len);

/* ------------------------------------------------------ */
/*	截断文件
*/
_int32 VodNewFs_truncate(const char *filepath, _u64 length);
_int32 VodNewFs_ftruncate(_u32 file_id, _u64 length);

/* 检查路径下是否有足够的剩余空间,need_size的单位是KB */
_int32 VodNewFs_check_enough_free_disk(char * path,_u32 need_size);

/* ------------------------------------------------------ */
#ifdef LINUX
#include <stdio.h>
typedef FILE		_FILE;
#else
typedef FILE		_FILE;
#endif

/* ------------------------------------------------------ */
/*	mode : r,r+,w,w+,a,a+  or with 'b'
*/
_int32 VodNewFs_fopen(const char *filepath, const char * mode,_FILE ** fp);
_int32 VodNewFs_fclose(_FILE *fp);

/* 读取一行或n-1个字符 */
_int32 VodNewFs_fgets(char *buf, _int32 n, _FILE *fp);

/* 写入字符串 */
_int32 VodNewFs_fputs(const char *str, _FILE *fp);

_int32 VodNewFs_is_support_create_big_file(const char* path, BOOL* result);

#ifdef __cplusplus
}
#endif

#endif
