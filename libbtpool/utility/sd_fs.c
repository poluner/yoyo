#include "utility/define.h"
#include "utility/sd_fs.h"
#include "utility/sd_time.h"
#include "utility/errcode.h"
#include "utility/sd_string.h"
#include "utility/sd_assert.h"
#include "utility/utility.h"
#include <sys/uio.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <map>
#include <dlfcn.h>
#include "utility/log_wrapper.h"
#include "utility/define.h"
#if defined(MACOS)
#include <sys/mount.h>
#else
#include <sys/vfs.h>
#endif

LOGID_MODEULE("interface")

#define DEFAULT_FILE_MODE	(0644)
#define DEFAULT_DIR_MODE	(0777)
#define MAX_WRITE_RETRY_TIMES   5

#ifndef MAX
#define MAX(n1, n2)  ((n1) > (n2) ? (n1) : (n2))
#endif

/////////////////////////////////////////////////////////
int32_t sd_setfilepos(uint32_t file_id, uint64_t filepos)
{
    int32_t ret_val = SUCCESS;

#if defined(MACOS)
    if(lseek( file_id, filepos, SEEK_SET) == (uint64_t)(-1))
#else
    if (lseek64(file_id, filepos, SEEK_SET) == (uint64_t)(-1))
#endif
    {
        LOG_ERROR("sd_setfilepos lseek failed, fd=[%u] filepos=[%llu] errno=[%d] errInfo=[%s]",
            file_id, filepos, errno, strerror(errno));
        ret_val = errno;
    }

    return ret_val;
}

int32_t sd_filesize(uint32_t file_id, uint64_t *filesize)
{
    int32_t ret_val = SUCCESS;

    struct stat file_stat;

    *filesize = 0;
    ret_val = fstat(file_id, &file_stat);
    if (ret_val == -1)
    {
        LOG_ERROR("sd_filesize fstat failed, fd=[%u] errno=[%d] errInfo=[%s]", file_id, errno, strerror(errno));
        ret_val = errno;
    }
    else
    {
        *filesize = file_stat.st_size;
        ret_val = 0;
    }

    return ret_val;
}

#ifdef _NEW_MEM_DEBUG
std::map<uint32_t, int> s_file_open_id_map;
#endif

int32_t sd_open_ex(const char *filepath, int32_t flag, uint32_t *file_id)
{
    int32_t ret_val = SUCCESS;

    if (NULL == file_id)
    {
        LOG_ERROR("sd_open_ex file_id is null, filepath=[%s]", filepath);
        return EINVAL; /// 无效的参数
    }

    if (NULL == filepath || sd_strlen(filepath) == 0 || sd_strlen(filepath) >= MAX_FULL_PATH_LEN)
    {
        LOG_ERROR("sd_open_ex filepath invalid, filepath=[%s]", filepath);
        return EINVAL; /// 无效的参数
    }

    ret_val = open(filepath, flag, DEFAULT_FILE_MODE);
    if (ret_val == -1)
    {
        ret_val = errno;
        LOG_ERROR("sd_open_ex open failed, filepath=[%s] flag=[0x%x] errno=[%d] errinfo=[%s]", filepath, flag, errno, strerror(errno));
    }
    else
    {
        sd_assert(ret_val != 0);
        *file_id = ret_val;
        fchmod(ret_val, 0777);
        ret_val = SUCCESS;
        LOG_DEBUG("sd_open_ex open success, filepath=[%s] fd=[%d] flag=[0x%x]", filepath, *file_id, flag);

    }

    return ret_val;
}

int32_t sd_close_ex(uint32_t file_id)
{
    int32_t ret_val = SUCCESS;



//     ret_val = fchmod(file_id, 0777);
//     if (0 != ret_val)
//     {
//         LOG_ERROR("sd_close_ex fchmod failed, fd=[%u] ret_val=[%d] errno=[%d] errInfo=[%s]",
//             file_id, ret_val, errno, strerror(errno));
//     }

    ret_val = close(file_id);
    if (0 != ret_val)
    {
        ret_val = errno;
        LOG_ERROR("sd_close_ex close failed, fd=[%u] ret_val=[%d] errno=[%d] errInfo=[%s]",
            file_id, ret_val, errno, strerror(errno));
    }
    else
    {
        LOG_DEBUG("sd_close_ex success to close file, fd=[%u]", file_id);
    }

#ifdef _NEW_MEM_DEBUG
    sd_assert(s_file_open_id_map.find(file_id) != s_file_open_id_map.end());
    s_file_open_id_map.erase(file_id);
#endif

    return ret_val;
}

void sd_check_file_id_all_closed()
{
}

int32_t sd_read(uint32_t file_id, char *buffer, int32_t size, uint32_t *readsize)
{
    int32_t ret_val = SUCCESS;

    (*readsize) = 0;

    do
    {
        ret_val = read(file_id, buffer, size);
    }
    while(ret_val < 0 && errno == EINTR);

    if(ret_val >= 0)
    {
        *readsize = ret_val;
        ret_val = SUCCESS;
    }
    else
    {
        LOG_ERROR("sd_read read failed, fd=[%u] size=[%d] errno=[%d] errInfo=[%s]",
            file_id, size, errno, strerror(errno));
        ret_val = errno;
    }

    return ret_val;
}

int32_t sd_write_v(uint32_t file_id,  SD_FS_WRITEV_INFO * arr, uint32_t count, uint32_t * writesize)
{
    struct iovec iov[SD_FS_MAX_WRITE_V_NUM];

    ssize_t fullSize = 0;
    for (uint32_t i = 0; i < count; i++)
    {
        struct iovec* p = &iov[i];
        p->iov_base = arr[i].m_buffer;
        p->iov_len = arr[i].m_length;
        fullSize += arr[i].m_length;
    }

    int32_t times = 0;
    uint32_t idx = 0;
    ssize_t arrSize = 0;
    ssize_t writedSize = 0;
    do
    {
        do
        {
            if (arrSize + arr[idx].m_length > writedSize)  break;
            arrSize += arr[idx++].m_length;
        } while (idx < count);

        struct iovec* p = &iov[idx];
        p->iov_base = arr[idx].m_buffer + writedSize - arrSize;
        p->iov_len = arr[idx].m_length - (writedSize - arrSize);

        ssize_t ret = 0;
        do
        {
            ret = writev(file_id, iov + idx, count - idx);
        }while(ret < 0 && errno == EINTR);

        if (ret > 0)
        {
            writedSize += ret;
        }
        else // ret == 0
        {
            LOG_ERROR("sd_write_v writev failed, fd=[%u] ret=[%d] errno=[%d] errInfo=[%s]", file_id, ret, errno, strerror(errno));
            return errno;
        }
    } while (writedSize < fullSize && times++ < MAX_WRITE_RETRY_TIMES);

    *writesize = writedSize;
    return SUCCESS;
}

int32_t sd_write(uint32_t file_id,const char *buffer, int32_t size, uint32_t *writesize)
{
    int32_t ret_val = SUCCESS;
    *writesize = 0;

    do
    {
        ret_val = write(file_id, buffer, size);
    }
    while(ret_val < 0 && errno == EINTR);

    if(ret_val >= 0)
    {
        *writesize = ret_val;
        ret_val = SUCCESS;
        fsync(file_id);
    }
    else
    {
        ret_val = errno;
        LOG_ERROR("sd_write write failed, fd=[%u] errno=[%d] errInfo=[%s]", file_id, errno, strerror(errno));
    }

    return ret_val;
}

int32_t sd_flush(uint32_t file_id)
{
    return fsync(file_id);
}

int32_t sd_rename_file(const char *filepath, const char *new_filepath)
{
    /* 和谐一下分隔符格式及编码方式 */
    int32_t ret_val = SUCCESS;

    // 如果new_filepath文件已经存在，返回错误。以便和其他平台保持一致性
    if (sd_file_exist(new_filepath))
        return -1;

    if (rename(filepath, new_filepath) == -1)
    {
        ret_val = errno;
    }

    return ret_val;
}

int32_t sd_copy_file(const char *filepath, const char *new_filepath)
{
    /* 和谐一下分隔符格式及编码方式 */
    int32_t ret_val = SUCCESS;
    LOG_DEBUG("sd_copy_file, from %s to %s", filepath, new_filepath);

#define MAX_CPFILE_DATA_BUF_LEN (1024)
    struct stat stat_buf;
    char databuf[MAX_CPFILE_DATA_BUF_LEN];
    int32_t fd1 = -1;
    int32_t fd2 = -1;
    uint32_t size = 0;

    sd_memset(&stat_buf, 0, sizeof(stat_buf));
    ret_val = lstat(filepath, &stat_buf);
    if (ret_val != SUCCESS)
    {
        LOG_DEBUG("sd_copy_file, lstat fail");
        return ret_val;
    }
    if (sd_strcmp(filepath, new_filepath) == 0)
    {
        LOG_DEBUG("sd_copy_file, from is same as to");
        return -1;
    }
    if (SUCCESS != sd_open_ex(filepath, O_FS_RDONLY, (uint32_t*)&ret_val))
    {
        LOG_DEBUG("sd_copy_file, open from file fail");
        return ret_val;
    }
    fd1 = ret_val;
    if (SUCCESS != sd_open_ex(new_filepath, O_CREAT | O_RDWR, (uint32_t*)&ret_val))
    {
        sd_close_ex(fd1);
        LOG_DEBUG("sd_copy_file, open to file fail");
        return ret_val;
    }

    fd2 = ret_val;

    ret_val = SUCCESS;
    while ((size = read(fd1, databuf, MAX_CPFILE_DATA_BUF_LEN)) != 0)
    {
        if (write(fd2, databuf, size) != size)
        {
            LOG_DEBUG("sd_copy_file, write to file fail");
            ret_val = -1;
            break;
        }
    }
    sd_close_ex(fd1);
    sd_close_ex(fd2);
#undef MAX_CPFILE_DATA_BUF_LEN

    return ret_val;
}

int32_t sd_delete_file(const char *filepath)
{
    int32_t ret_val = SUCCESS;

    LOG_DEBUG("sd_delete_file do, filepath = %s", filepath);

    if (unlink(filepath) == -1)
        ret_val = errno;

    return ret_val;

}

/**
* 如果传入is_dir为TRUE，检查filepath是否为已存在的目录
* 否则仅检查filepath是否存在，不关心其是文件还是目录
*/
static bool sd_file_exist_ex(const char *filepath, const bool is_dir)
{
    struct stat file_stat = { 0 };
    if (0 != lstat(filepath, &file_stat))
        return false;

    if (false == is_dir)
        return true;

    return S_ISDIR(file_stat.st_mode);
}

bool sd_file_exist(const char *filepath)
{
    return sd_file_exist_ex(filepath, false);
}

bool sd_dir_exist(const char *dirpath)
{
    return sd_file_exist_ex(dirpath, true);
}

//注意本函数要求dirpath格式已经和谐，编码未和谐；因为用到了sd_strrchr，同时内部调用了pure_mkdir
int32_t recursive_mkdir(const char *dirpath)
{
    int32_t ret_val = TRUE;
    char *ppos = NULL;

    char formatted_filepath[MAX_FULL_PATH_LEN];
    uint32_t formatted_filepath_len = MAX_FULL_PATH_LEN;

    ppos = sd_strrchr(dirpath, DIR_SPLIT_CHAR);
    if (!ppos)
        return BAD_DIR_PATH;

    sd_memset(formatted_filepath, 0x00, formatted_filepath_len);
    ret_val = sd_conv_path(dirpath, sd_strlen(dirpath), formatted_filepath, &formatted_filepath_len);
    CHECK_VALUE(ret_val);

    *ppos = 0;

    ret_val = mkdir(formatted_filepath, DEFAULT_DIR_MODE);

    if (ret_val < 0)
    {
        ret_val = errno;
        if (ret_val == ENOENT)
        {
            ret_val = recursive_mkdir(dirpath);
            if (ret_val == SUCCESS)
            {
                ret_val = mkdir(formatted_filepath, DEFAULT_DIR_MODE);

                if (ret_val < 0)
                {
                    ret_val = errno;
                    if (ret_val == EEXIST)
                        ret_val = SUCCESS;
                }
            }
        }
    }
    *ppos = DIR_SPLIT_CHAR;

    return ret_val;
}

int32_t sd_mkdir(const char *dirpath)
{

    /* 和谐一下格式，但是不能转码，因为recursive_mkdir递归内部无和谐且需要gbk编码*/
    int32_t ret_val = SUCCESS;
    char formatted_filepath[MAX_FULL_PATH_LEN] = { 0 };
    uint32_t formatted_filepath_len = 0;
    char tmp_path[MAX_FULL_PATH_LEN];

    char converted_path[MAX_FULL_PATH_LEN];
    uint32_t converted_path_len = MAX_FULL_PATH_LEN;

    ret_val = sd_format_dirpath(dirpath, formatted_filepath, MAX_FULL_PATH_LEN, &formatted_filepath_len);
    CHECK_VALUE(ret_val);

    dirpath = formatted_filepath;

    sd_memset(converted_path, 0x00, converted_path_len);
    ret_val = sd_conv_path(dirpath, sd_strlen(dirpath), converted_path, &converted_path_len);
    CHECK_VALUE(ret_val);

    ret_val = mkdir(converted_path, DEFAULT_DIR_MODE);

    if (ret_val < 0)
    {
        ret_val = errno;

        if (ret_val == ENOENT)
        {
            sd_strncpy(tmp_path, dirpath, MAX_FULL_PATH_LEN);
            ret_val = recursive_mkdir(tmp_path);
            if (ret_val == SUCCESS)
            {
                ret_val = mkdir(converted_path, DEFAULT_DIR_MODE);
                if (ret_val < 0)
                {
                    ret_val = errno;
                    if (sd_dir_exist(converted_path))
                        ret_val = SUCCESS;
                    else
                        return BAD_DIR_PATH;
                }
            }
        }
        else if (ret_val == EEXIST)
        {
            if (sd_dir_exist(converted_path))
                ret_val = SUCCESS;
            else
                return BAD_DIR_PATH;
        }
    }

    return ret_val;
}

int32_t recursive_rmdir(char *dirpath, uint32_t path_len, uint32_t buf_len)
{
    int32_t ret_val = SUCCESS;
    uint32_t new_path_len = path_len;

    DIR* p_dir = opendir(dirpath);
    struct dirent* p_dirent_item = NULL;

    if (NULL == p_dir)
    {
        LOG_ERROR("recursive_rmdir, open dir fail: %s", dirpath);
        return 1;
    }
    // 删除当前目录里面的所有文件
    while ((p_dirent_item = readdir(p_dir)) != NULL)
    {
        uint32_t fname_len = 0;
        BOOL is_dir = FALSE;

        //过滤“.”、“..”文件
        if (p_dirent_item->d_name[0] == '.' &&
                (p_dirent_item->d_name[1] == '\0' || (p_dirent_item->d_name[1] == '.' && p_dirent_item->d_name[2] == '\0')))
        {
            continue;
        }

#ifdef _DIRENT_HAVE_D_NAMLEN
        fname_len = p_dirent_item->d_namlen;
#else
        fname_len = sd_strlen(p_dirent_item->d_name);
#endif
        //长度是否可接受
        if (fname_len > buf_len - path_len - 1)
        {
            //closedir(p_dir);
            ret_val = 2;// 超出buf长度，无法删除
            break;
        }
        new_path_len = path_len + fname_len;
        sd_memcpy(dirpath + path_len, p_dirent_item->d_name, fname_len);
        dirpath[new_path_len] = '\0';

        // 判断是否文件夹
#ifdef _DIRENT_HAVE_D_TYPE
        is_dir = (p_dirent_item->d_type == DT_DIR) ? (TRUE): (FALSE);
#else
        //查询stat
        struct stat stat_buf;

        sd_memset(&stat_buf, 0, sizeof (stat_buf));
        ret_val = lstat(dirpath, &stat_buf);
        if (ret_val != SUCCESS)
        {
            LOG_ERROR("recursive_rmdir, lstat file fail: %s", p_dirent_item->d_name);
            //closedir(p_dir);
            ret_val = 2;// 无法判断是文件还是路径，无法删除
            break;
        }
        is_dir = S_ISDIR(stat_buf.st_mode);
#endif
        // 删除当前项
        if (!is_dir)
        {
            if (unlink(dirpath) != 0)
            {
                //closedir(p_dir);
                ret_val = 2;// 删除文件失败
                break;
            }
        }
        else
        {
            dirpath[new_path_len++] = DIR_SPLIT_CHAR;
            dirpath[new_path_len] = '\0';
            ret_val = recursive_rmdir(dirpath, new_path_len, buf_len);
            if (ret_val != SUCCESS)
            {
                //closedir(p_dir);
                ret_val = ret_val + 1;// 删除第ret_val层目录失败
                break;
            }
        }
    }

    closedir(p_dir);

    // 删除当前目录
    if (SUCCESS == ret_val)
    {
        dirpath[path_len] = '\0';
        ret_val = rmdir(dirpath);
        if (ret_val != 0)
            ret_val = 1;// 删除当前目录失败返回1
    }
    return ret_val;

}

int32_t sd_recursive_rmdir(const char *dirpath)
{
    int32_t ret_val = SUCCESS;
    uint32_t formatted_conv_path_len = 0;
    char formatted_conv_path[MAX_FULL_PATH_LEN];
    uint32_t formatted_conv_path_buflen = MAX_FULL_PATH_LEN;


    sd_memset(formatted_conv_path, 0, formatted_conv_path_buflen);
    ret_val = sd_format_conv_dirpath(dirpath, formatted_conv_path, formatted_conv_path_buflen, &formatted_conv_path_len);
    CHECK_VALUE(ret_val);

    return recursive_rmdir(formatted_conv_path, formatted_conv_path_len, formatted_conv_path_buflen);
}
int32_t sd_write_save_to_buffer(uint32_t file_id, char *buffer, uint32_t buffer_len, uint32_t *buffer_pos, char * p_data, uint32_t data_len)
{
    int32_t ret = SUCCESS;
    uint32_t pos = *buffer_pos, write_size = 0;

    LOG_DEBUG("sd_write_save_to_buffer:buffer_len=%u,*buffer_pos=%u,data_len=%u", buffer_len, *buffer_pos, data_len);

    if (pos + data_len > buffer_len)
    {
        ret = sd_write(file_id, buffer, pos, &write_size);
        CHECK_VALUE(ret);
        sd_assert(pos == write_size);
        pos = 0;
    }

    if (data_len > buffer_len)
    {
        ret = sd_write(file_id, p_data, data_len, &write_size);
        CHECK_VALUE(ret);
        sd_assert(data_len == write_size);
    }
    else
    {
        sd_assert(pos + data_len <= buffer_len);
        sd_memcpy(buffer + pos, p_data, data_len);
        pos += data_len;
    }

    *buffer_pos = pos;

    return SUCCESS;
}

int32_t sd_format_filepath(const char *path, char *formated_path,
                          uint32_t formated_path_size, uint32_t *p_formated_path_len)
{
    int32_t ret_val = SUCCESS;
    uint32_t path_len = sd_strlen(path);
    uint32_t i = 0, j = 0;

    if (path == NULL || formated_path == NULL
            || path_len == 0 || path_len >= MAX_FULL_PATH_LEN || p_formated_path_len == 0)
    {
        return INVALID_ARGUMENT;
    }

    sd_memset(formated_path, 0, formated_path_size);
    for (i = 0; i < path_len; ++i)
    {
        if (formated_path_size < j + 1)
        {
            return BUFFER_OVERFLOW;
        }

        if (path[i] == '/' || path[i] == '\\')
        {
            formated_path[j] = DIR_SPLIT_CHAR;

            // 去掉重复的'/'
            while (i + 1 < path_len)
            {
                if (path[i + 1] == '/' || path[i + 1] == '\\')
                {
                    i++;
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            formated_path[j] = path[i];
        }

        j++;
    }

    *p_formated_path_len = j;


    return ret_val;
}

int32_t sd_format_dirpath(const char *path, char *formated_path,
                         uint32_t formated_path_size, uint32_t *p_formated_path_len)
{
    uint32_t format_len = 0;
    int32_t ret = sd_format_filepath(path, formated_path, formated_path_size, &format_len);
    if (ret != SUCCESS)
    {
        return ret;
    }

    if (format_len == 0)
    {
        return FILE_INVALID_PARA;
    }

    // 必须以 DIR_SPLIT_CHAR 结尾。
    if (formated_path[format_len - 1] != DIR_SPLIT_CHAR)
    {
        if (formated_path_size < format_len + 2)
        {
            return FILE_INVALID_PARA;
        }
        formated_path[format_len] = DIR_SPLIT_CHAR;
        formated_path[format_len + 1] = '\0';
        format_len++;
    }

    *p_formated_path_len = format_len;
    return SUCCESS;
}

// 对path进行转码，适应不同的平台
// 其中symbian  -> GBK; WINCE ->Unicode; Android ->UTF8
// 其他不处理
int32_t sd_conv_path(const char *p_input, uint32_t input_len, char* p_output, uint32_t *p_output_len)
{
    //todo[lvfei]:先直接返回，以后有需求再实现
    *p_output_len = input_len;
    sd_memcpy(p_output, p_input, input_len);
    return 0;
}


int32_t sd_format_conv_dirpath(const char *path, char *formated_conv_path,
                              uint32_t formated_conv_path_size, uint32_t *p_formated_conv_path_len)
{
    int32_t ret = 0;
    uint32_t path_len = sd_strlen(path);
    char formatted_path[MAX_FULL_PATH_LEN];
    uint32_t formatted_path_len = 0;

    if (NULL == path || NULL == formated_conv_path
            || 0 == path_len || path_len >= MAX_FULL_PATH_LEN || NULL == p_formated_conv_path_len)
    {
        return INVALID_ARGUMENT;
    }

    sd_memset(formatted_path, 0, MAX_FULL_PATH_LEN);
    //格式化路径分隔符
    ret = sd_format_dirpath(path, formatted_path, MAX_FULL_PATH_LEN, &formatted_path_len);
    CHECK_VALUE(ret);

    // 按平台进行转码
    *p_formated_conv_path_len = formated_conv_path_size;
    ret = sd_conv_path(formatted_path, formatted_path_len, formated_conv_path, p_formated_conv_path_len);
    CHECK_VALUE(ret);

    return SUCCESS;
}

void append_path(std::string& path, const std::string& append)
{
    if (path[path.length()-1] != DIR_SPLIT_CHAR)
    {
        path += DIR_SPLIT_CHAR;
    }
    path.append(append);
}

int32_t sd_truncate(const char *filepath, uint64_t length)
{
#ifdef _ANDROID_LINUX
    /// android上不支持大于4g的文件裁剪
    if ((length >> 32) > 0)
    {
        LOG_ERROR("sd_truncate android not support >= 4G file, filename=[%s] length=[%llu]", filepath, length);
        sd_assert(false);
        return EFBIG;
    }
#endif /// _ANDROID_LINUX

    int32_t ret_val = SUCCESS;
    do
    {
        ret_val = truncate(filepath, length);
    }
    while (ret_val < 0 && errno == EINTR);

    if(ret_val < 0)
    {
        ret_val = errno;
        LOG_ERROR("sd_truncate truncate failed, filename=[%s] length=[%llu] errno=[%d] errInfo=[%s]",
            filepath, length, errno, strerror(errno));
        return ret_val;
    }
    else
    {
        LOG_DEBUG("sd_truncate truncate success, filename=[%s] length=[%llu]", filepath, length);
        return SUCCESS;
    }
}

const char* sd_get_file_name(const char* szFullPath)
{
    if (NULL == szFullPath)
        return NULL;

    const char* szEnd1 = strrchr(szFullPath, '/');
    if (szEnd1) szEnd1++;

    const char* szEnd2 = strrchr(szFullPath, '\\');
    if (szEnd2) szEnd2++;

    if ((NULL == szEnd1) && (NULL == szEnd2))
    {
        return szFullPath;
    }

    return MAX(szEnd1, szEnd2);
}
