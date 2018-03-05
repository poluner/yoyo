#ifndef SD_STRING_H_00138F8F2E70_200806121335
#define SD_STRING_H_00138F8F2E70_200806121335

#ifdef __cplusplus
extern "C"
{
#endif

#include "utility/define.h"
#include "utility/arg.h"
struct tagLIST;

int32_t sd_strncpy(char *dest, const char *src, int32_t size);

int32_t sd_vsnprintf(char *buffer, int32_t bufsize, const char *fmt, sd_va_list ap);
int32_t sd_snprintf(char *buffer, int32_t bufsize, const char *fmt, ...);


int32_t sd_fprintf(int32_t fd, const char *fmt, ...);

int32_t sd_printf(const char *fmt, ...);


/* @Simple Function@
 * Return : length of str
 */
int32_t sd_strlen(const char *str);

int32_t sd_strcat(char *dest, const char *src, int32_t n);


/* @Simple Function@
 * Return :   -1: s1<s2  0: s1==s2     1: s1>s2
 */
int32_t sd_strcmp(const char *s1, const char *s2);

int32_t sd_strncmp(const char *s1, const char *s2, int32_t n);

/* @Simple Function@
 * Return :   -1: s1<s2  0: s1==s2     1: s1>s2
 */
int32_t sd_stricmp(const char *s1, const char *s2);

/* @Simple Function@
 * only compares the first n characters of s1
 * Return :   -1: s1<s2  0: s1==s2     1: s1>s2
 */
int32_t sd_strnicmp(const char *s1, const char *s2, int32_t n);

/* @Simple Function@
 * Return : a pointer to the first occurrence of the character ch in the string dest
 *          NULL when not found
 */
char* sd_strchr(char *dest, char ch, int32_t from);

/* @Simple Function@
 * 忽略大小写的查找。
 * Return : a pointer to the first occurrence of the character ch in the string dest
 *          NULL when not found
 */
char* sd_strichr(char *dest, char ch, int32_t from);

/* @Simple Function@
 * Return : a pointer to the beginning of the sub-string
 *          NULL when not found
 */
char* sd_strstr(const char *dest, const char *search_str, int32_t from);

/* @Simple Function@
 * 忽略大小写的查找。
 * Return : a pointer to the beginning of the sub-string
 *          NULL when not found
 */
char* sd_stristr(const char *dest, const char *search_str, int32_t from);

/* @Simple Function@
 * Return : a pointer to the last occurrence of the character ch in the string dest
 *          NULL when not found
 */
char* sd_strrchr(const char *dest, char ch);

/* @Simple Function@
 * 忽略大小写的查找。
 * Return : a pointer to the last occurrence of the character ch in the string dest
 *          NULL when not found
 */
char* sd_strirchr(char *dest, char ch);

int32_t sd_memset(void *dest, int32_t c, int32_t count);

/*  The memory areas should not overlap*/
int32_t sd_memcpy(void *dest, const void *src, int32_t n);

/*  The memory areas may overlap*/
int32_t sd_memmove(void *dest, const void *src, int32_t n);
int32_t sd_memcmp(const void* src, const void* dest, uint32_t len);

/* @Simple Function@
 * Description: remove all the ' ','\t','\r' and '\n' on the head of the string.
 * Return :     0: SUCCESS     -1: ERROR
 */
int32_t sd_trim_prefix_lws( char * str );

/* @Simple Function@
 * Description: remove all the ' ','\t','\r' and '\n' on the tail of the string.
 * Return :     0: SUCCESS     -1: ERROR
 */
int32_t sd_trim_postfix_lws( char * str );

char sd_toupper(char ch);
char sd_tolower(char ch);
void sd_strtolower(char * str);

/*  指定最大字符的长度，返回其截断的子串的长度，utf-8编码 */
uint32_t sd_get_sub_utf8_str_len(char *utf8_str, uint32_t max_len);

int32_t sd_vfprintf(int32_t fd, const char *fmt, sd_va_list ap);

char* sd_strndup(const char* src, uint32_t n);

#ifdef __cplusplus
}
#endif

#endif
