#ifndef _version_
#define _version_

#ifndef BIG_VERSION
#define BIG_VERSION 2
#define REQUIRE_VERSION 0802
#define BUILD_TYPE 239
#define BUILD_NUM 889
#endif


#ifndef REQUIRE_VERSION
#define REQUIRE_VERSION 0802
#endif

#ifndef BUILD_TYPE
#define BUILD_TYPE 239
#endif

#ifndef BUILD_NUM
#define BUILD_NUM 1
#endif

#define MKSTR(s) # s
#define __VERSION(__v) MKSTR(__v)
#define _VERSION(bv, rv, bt, bn) __VERSION(bv ## . ## rv ## . ## bt ## . ## bn)
#define VERSION(bv, rv, bt, bn) _VERSION(bv, rv, bt, bn)

//濡?2.2.101
#define XL_THUNDER_SDK_VERSION VERSION(BIG_VERSION, REQUIRE_VERSION, BUILD_TYPE, BUILD_NUM)

#endif
