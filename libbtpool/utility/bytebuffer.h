#ifndef SD_BYTEBUFFER_H_00138F8F2E70_200806171745
#define SD_BYTEBUFFER_H_00138F8F2E70_200806171745

#ifdef __cplusplus
extern "C"
{
#endif

#include "utility/define.h"

/* @Simple Function@
 * Return : none
 */
void bytebuffer_init(void);

/* transfer to host byte-order*/
int32_t sd_get_int64_from_bg(char **buffer, int32_t *cur_buflen, int64_t *value);
int32_t sd_get_int32_from_bg(char **buffer, int32_t *cur_buflen, int32_t *value);
int32_t sd_get_int16_from_bg(char **buffer, int32_t *cur_buflen, int16_t *value);

int32_t sd_get_int64_from_lt(char **buffer, int32_t *cur_buflen, int64_t *value);
int32_t sd_get_int32_from_lt(char **buffer, int32_t *cur_buflen, int32_t *value);
int32_t sd_get_int16_from_lt(char **buffer, int32_t *cur_buflen, int16_t *value);

int32_t sd_get_int8(char **buffer, int32_t *cur_buflen, int8_t *value);


/* Transfer from host byte-order */
int32_t sd_set_int64_to_bg(char **buffer, int32_t *cur_buflen, int64_t value);
int32_t sd_set_int32_to_bg(char **buffer, int32_t *cur_buflen, int32_t value);
int32_t sd_set_int16_to_bg(char **buffer, int32_t *cur_buflen, int16_t value);

int32_t sd_set_int64_to_lt(char **buffer, int32_t *cur_buflen, int64_t value);
int32_t sd_set_int32_to_lt(char **buffer, int32_t *cur_buflen, int32_t value);
int32_t sd_set_int16_to_lt(char **buffer, int32_t *cur_buflen, int16_t value);

int32_t sd_set_int8(char **buffer, int32_t *cur_buflen, int8_t value);

int32_t sd_get_bytes(char **buffer, int32_t *cur_buflen, char *dest_buf, int32_t dest_len);
int32_t sd_set_bytes(char **buffer, int32_t *cur_buflen,const char *src_buf, int32_t src_len);
//int32_t sd_set_bytes(char **buffer, int32_t *cur_buflen, char *dest_buf, int32_t dest_len);

int32_t sd_get_string(char **buffer, int32_t *cur_buflen, char *value, int32_t value_buffer_len, int32_t *value_len);
int32_t sd_set_string(char **buffer, int32_t *cur_buflen, char *value, int32_t value_len);

#ifdef __cplusplus
}
#endif

#endif
