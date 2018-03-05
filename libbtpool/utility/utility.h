#ifndef SD_UTILITY_H_00138F8F2E70_200806182142
#define SD_UTILITY_H_00138F8F2E70_200806182142

#include <string>

#ifdef __cplusplus
extern "C"
{
#endif
#include "utility/define.h"
#include "utility/sd_time.h"
#include <errno.h>

#define SD_RAND_MAX	(2147483647)

#if defined(LINUX)
#ifndef MIN
#define MIN(n1, n2)  ((n1) < (n2) ? (n1) : (n2))
#endif
#ifndef MAX
#define MAX(n1, n2)  ((n1) > (n2) ? (n1) : (n2))
#endif

#elif defined(WINCE)
#ifndef MIN
#define MIN(n1, n2)  ((n1) < (n2) ? (n1) : (n2))
#endif
#ifndef MAX
#define MAX(n1, n2)  ((n1) > (n2) ? (n1) : (n2))
#endif

#endif

#define IS_DIGIT(chr)  ((((chr) >= '0')&&((chr) <= '9'))? TRUE : FALSE)
#define IS_ALPHA(chr)  ((((chr) >= 'a')&&((chr) <= 'z'))||(((chr) >= 'A')&&((chr) <= 'Z')))? TRUE : FALSE)
#define IS_HEX(chr)  (((((chr) >= '0')&&((chr) <= '9'))|| (((chr) >= 'a')&&((chr) <= 'f'))||(((chr) >= 'A')&&((chr) <= 'F')))? TRUE : FALSE)

/* @Simple Function@
 * Return : the binary data in network byte-order of cp
 */
uint32_t sd_inet_addr(const char *cp);

/* same as sd_inet_addr, but can be return errcode */
int32_t sd_inet_aton(const char *cp, uint32_t *nip);

/* inet_ntoa */
int32_t sd_inet_ntoa(uint32_t inp, char *out_buf, int32_t bufsize);

/* @Simple Function@
 * Return : network byte-order of h
 */
uint16_t sd_htons(uint16_t h);

/* @Simple Function@
 * Return : network byte-order of h
 */
uint32_t sd_htonl(uint32_t h);

/* @Simple Function@
 * Return : host byte-order of h
 */
uint16_t sd_ntohs(uint16_t n);

/* @Simple Function@
 * Return : host byte-order of h
 */
uint32_t sd_ntohl(uint32_t n);

/* e.g. 'A' to 41 */
int32_t char2hex( unsigned char c , char *out_buf, int32_t out_bufsize);
/* hex */
int32_t str2hex(const char *in_buf, int32_t in_bufsize, char *out_buf, int32_t out_bufsize);

//将Hex字符串转换成普通字符串
int32_t hex2str(const char* hex, int32_t hex_len, char* str, int32_t str_len);


//将十六制字符表示为十进制整数,如'A'转换为10
int32_t sd_hex_2_int(char chr);
//将十六进制字符串转换为十进制整数,不考虑大数
int32_t sd_hexstr_2_int(const char * in_buf, int32_t in_bufsize);
/* @Simple Function@
 * Return : the number described by nptr
 *
 */
int32_t sd_atoi(const char* nptr);

int32_t sd_srand(uint32_t seeds);

/* @Simple Function@
 * Return : random integer between 0 - SD_RAND_MAX
 *
 */
int32_t sd_rand(void);

/* @Simple Function@
 * Return : TRUE if equal,or FALSE
 *
 */
BOOL sd_is_bcid_equal(const uint8_t* _bcid1,uint32_t bcid1_len,const uint8_t* _bcid2,uint32_t bcid2_len);

/* @Simple Function@
 * Return : TRUE if equal,or FALSE
 *
 */
BOOL sd_is_cid_equal(const uint8_t* _cid1,const uint8_t* _cid2);

/* @Simple Function@
 * Return : TRUE if valid,or FALSE
 *
 */
BOOL sd_is_cid_valid(const uint8_t* _cid);

BOOL sd_is_bcid_valid(uint64_t filesize, uint32_t bcid_size, uint32_t block_size);


/* @Simple Function@
 * Return : the hashvalue(elf) of str
 *
 */
uint32_t sd_elf_hashvalue(const char *str, uint32_t hash_value);
uint64_t sd_generate_hash_from_size_crc_hashvalue(const uint8_t * data ,uint32_t data_size);

/* str to val */
int32_t sd_str_to_u64( const char *str, uint32_t strlen, uint64_t *val );

/* val to str */
int32_t sd_u64_to_str( uint64_t val, char *str, uint32_t strlen );
int32_t sd_u32_to_str( uint32_t val, char *str, uint32_t strlen );

int32_t sd_abs(int32_t val);

BOOL sd_data_cmp(uint8_t* i_data, uint8_t* d_data, int32_t count);
/*
	Convert the string(40 chars hex ) to content id(20 bytes in hex)
*/
int32_t sd_string_to_cid(const char * str,uint8_t *cid);
/*
	Convert the string(chars hex ) to  bytes in hex
*/
int32_t sd_string_to_hex(char * str,uint8_t * hex);


int32_t sd_i32toa(int32_t value, char *buffer, int32_t buf_len, int32_t radix);
int32_t sd_i64toa(int64_t value, char *buffer, int32_t buf_len, int32_t radix);

int32_t sd_u32toa(uint32_t value, char *buffer, int32_t buf_len, int32_t radix);
int32_t sd_u64toa(uint64_t value, char *buffer, int32_t buf_len, int32_t radix);

uint32_t sd_digit_bit_count( uint64_t value );
BOOL is_cmwap_prompt_page(const char * http_header,uint32_t header_len);

int32_t sd_calc_file_cid(const char* file_path, uint8_t * hex_cid);
int32_t sd_calc_buf_cid(const char *buf, int buf_len, uint8_t *p_cid);
uint32_t sd_calc_gcid_part_size(uint64_t file_size);
int32_t sd_calc_file_gcid(const char* file_path, uint8_t* gcid, int32_t * bcid_num, uint8_t ** bcid_buf,uint64_t * file_size);

/*xml字符串中的实体引用替换*/
int32_t sd_xml_entity_ref_replace(char* xml_str,uint32_t total_buffer_len);

int32_t sd_get_file_type_from_name(const char *p_file_name);

void sd_exit(int32_t errcode);

int32_t sd_cid_to_hex_string(const char* cid, int32_t cid_len, char* hex_cid_str, int32_t hex_cid_len);

int32_t sd_gm_time(uint32_t time_sec, TIME_t *p_time);

int32_t sd_str_to_i64_v2(const char* str, uint32_t len, int64_t* val);

int32_t sd_getaddrinfo(const char *host, char *ip, int ip_len);

uint32_t sd_get_local_ip(void);

BOOL sd_is_lan_ip(uint32_t ip);


/* IPv6 related */
int32_t sd_inet6_pton(const char *pIp6Str, void *pIp6);

int32_t sd_inet6_ntop(const void *pIp6, char *pIp6Str, int32_t strSize);

void sd_reset_bit(uint32_t* value, unsigned char pos);



#ifdef __cplusplus
}
#endif
std::string double_to_string(double num);
std::string cid_to_hex_string(const std::string& cid);
std::string cid_to_hex_string_lc(const std::string& cid);
std::string sd_get_suffix(const std::string& filename);
void make_http_respone(std::string & outStr, uint64_t filesize);
void make_error_http_header(std::string & outStr,const std::string& resourceLine, const std::string& shubLine);
bool sd_get_linux_peerid(char * buf1, int buf_len);




class SD_IPADDR;
BOOL sd_try_convert_host2address(const std::string &host, SD_IPADDR &ip);
#endif

