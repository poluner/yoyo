#include "utility/check_const.h"

#define ENCODE_KEY	"Xunlei_Download"
#define ENCODE_METHOD(c, k)	c^k
#define DECODE_METHOD(c, k)	c^k

// ENCODE_i 中的数字i 表示调用该宏时应传入的参数个数, 最大可加密字符串长度为10
#define ENCODE_1(a)			ENCODE_METHOD(a, ENCODE_KEY[0])
#define ENCODE_2(a, ...)	ENCODE_METHOD(a, ENCODE_KEY[1]), ENCODE_1(__VA_ARGS__)
#define ENCODE_3(a, ...)	ENCODE_METHOD(a, ENCODE_KEY[2]), ENCODE_2(__VA_ARGS__)
#define ENCODE_4(a, ...)	ENCODE_METHOD(a, ENCODE_KEY[3]), ENCODE_3(__VA_ARGS__)
#define ENCODE_5(a, ...)	ENCODE_METHOD(a, ENCODE_KEY[4]), ENCODE_4(__VA_ARGS__)
#define ENCODE_6(a, ...)	ENCODE_METHOD(a, ENCODE_KEY[5]), ENCODE_5(__VA_ARGS__)
#define ENCODE_7(a, ...)	ENCODE_METHOD(a, ENCODE_KEY[6]), ENCODE_6(__VA_ARGS__)
#define ENCODE_8(a, ...)	ENCODE_METHOD(a, ENCODE_KEY[7]), ENCODE_7(__VA_ARGS__)
#define ENCODE_9(a, ...)	ENCODE_METHOD(a, ENCODE_KEY[8]), ENCODE_8(__VA_ARGS__)
#define ENCODE_10(a, ...)	ENCODE_METHOD(a, ENCODE_KEY[9]), ENCODE_9(__VA_ARGS__)

template <int N>
static std::string decode(char (&arr)[N])
{
	for (int i=0; i<N; ++i) {
		arr[i] = DECODE_METHOD(arr[i], ENCODE_KEY[N-i-1]);
	}
	return std::string(arr, N);
}

std::string  CheckConst::getXt()
{
	char buf[] = {ENCODE_2('B','t')};
 	return decode(buf);
}

std::string  CheckConst::getxt()
{
	char buf[] = {ENCODE_2('b','t')};
 	return decode(buf);
}

std::string  CheckConst::getthunderek()
{
	char buf[] = {ENCODE_4('e','d','2','k')};
 	return decode(buf);
}

std::string  CheckConst::getthundere() {
	char buf[] = {ENCODE_5('e','m','u','l','e')};
 	return decode(buf);
}

std::string  CheckConst::getthunderE()
{
	char buf[] = {ENCODE_5('E','m','u','l','e')};
 	return decode(buf);
}

std::string  CheckConst::getthunderm()
{
	char buf[] = {ENCODE_6('m','a','g','n','e','t')};
 	return decode(buf);
}

std::string  CheckConst::getthunderM()
{
	char buf[] = {ENCODE_6('M','a','g','n','e','t')};
 	return decode(buf);
}

std::string  CheckConst::getthunderz()
{
	char buf[] = {ENCODE_7('t','o','r','r','e','n','t')};
 	return decode(buf);
}
std::string  CheckConst::getthunderZ()
{
	char buf[] = {ENCODE_7('T','o','r','r','e','n','t')};
 	return decode(buf);
}

std::string  CheckConst::getblt()
{
	char buf[] = {ENCODE_3('b','i','t')};
 	return decode(buf);
}

std::string  CheckConst::getBlt()
{
	char buf[] = {ENCODE_3('B','i','t')};
 	return decode(buf);
}

std::string CheckConst::getlfilel()
{
	char buf[] = {ENCODE_6('|','f','i','l','e','|')};
 	return decode(buf);
}

std::string CheckConst::getthunderekss()
{
	char buf[] = {ENCODE_7('e','d','2','k',':','/','/')};
 	return decode(buf);
}

std::string  CheckConst::getthundermss()
{
	char buf[] = {ENCODE_8('m','a','g','n','e','t',':','?')};
 	return decode(buf);
}

