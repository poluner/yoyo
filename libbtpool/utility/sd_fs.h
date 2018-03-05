#ifndef SD_FS_H_00138F8F2E70_200806111928
#define SD_FS_H_00138F8F2E70_200806111928

#include <string>
#include <fcntl.h>
#include <stdint.h>

/* whether if create file while it not exist. */
#define O_FS_CREATE		O_CREAT
/* read and write (default) */
#define O_FS_RDWR		O_RDWR
/* read only. */
#define O_FS_RDONLY		O_RDONLY
/* write only. */
#define O_FS_WRONLY		O_WRONLY

#define DIR_SPLIT_CHAR   '/'
#define DIR_SPLIT_STRING "/"

/* ------------------------------------------------------ */
typedef struct TAG_SD_FS_WRITEV_INFO
{
    char* m_buffer;
    uint32_t m_length;
} SD_FS_WRITEV_INFO;

#define SD_FS_MAX_WRITE_V_NUM (128)
#define SD_FS_MAX_WRITE_V_DATA_NUM (30)

int32_t sd_setfilepos(uint32_t file_id, uint64_t filepos);
int32_t sd_filesize(uint32_t file_id, uint64_t *filesize);
int32_t sd_open_ex(const char *filepath, int32_t flag, uint32_t *file_id);
int32_t sd_close_ex(uint32_t file_id);
int32_t sd_read(uint32_t file_id, char *buffer, int32_t size, uint32_t *readsize);
int32_t sd_write(uint32_t file_id,const char *buffer, int32_t size, uint32_t *writesize);
int32_t sd_write_v(uint32_t file_id,  SD_FS_WRITEV_INFO* arr, uint32_t count, uint32_t* writesize);
int32_t sd_flush(uint32_t file_id);
int32_t sd_copy_file(const char *filepath, const char *new_filepath);
int32_t sd_rename_file(const char *filepath, const char *new_filepath);
int32_t sd_delete_file(const char *filepath);

/* sd_file_exist
 * Return : whether if filepath exist
 */
bool sd_file_exist(const char *filepath);

/* sd_dir_exist
 * Return : whether if dirpath is a exist dir
 */
bool sd_dir_exist(const char *dirpath);

int32_t recursive_mkdir(const char *dirpath);

/* create directory recursively */
int32_t sd_mkdir(const char *dirpath);

/* Save the being writen data to buffer for reducing sd_write operations  */
int32_t sd_write_save_to_buffer(uint32_t file_id, char *buffer, uint32_t buffer_len,uint32_t *buffer_pos, char * p_data,uint32_t data_len);

/* 格式化文件路径中的\和/
 * 传入path编码必须兼容ASCII,例如GBK
**/
int32_t sd_format_filepath(const char *path, char *formated_path,
                          uint32_t formated_path_size, uint32_t *p_formated_path_len);

/* 格式化目录路径中的\和/，最后以分隔符结尾
* 传入path编码必须兼容ASCII,例如GBK
**/
int32_t sd_format_dirpath(const char *path, char *formated_path,
                         uint32_t formated_path_size, uint32_t *p_formated_path_len);

int32_t sd_recursive_rmdir(const char *dirpath);
int32_t sd_format_conv_dirpath(const char *path, char *formated_conv_path,
    uint32_t formated_conv_path_size, uint32_t *p_formated_conv_path_len);

/* 对path进行转码，适应不同的平台
 * 其中symbian  -> GBK; WINCE ->Unicode; Android ->UTF8 , 其他不处理
**/
int32_t sd_conv_path(const char *input,uint32_t input_len, char* output,uint32_t *output_len);

void append_path(std::string& path, const std::string& append);

/* ------------------------------------------------------ */
/*	截断文件
*/
int32_t sd_truncate(const char *filepath, uint64_t length);
void sd_check_file_id_all_closed();
const char* sd_get_file_name(const char* szFullPath);
#endif
