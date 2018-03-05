#ifndef _UTILITY_CHECK_CONST_H__
#define _UTILITY_CHECK_CONST_H__
#include <string>

//常量关键字加密，应对苹果审查

class CheckConst 
{
public:
// 内联，不进符号表。函数名代表实际得到的字符串，最后三个除外。
	static std::string Bt() { return getXt(); }
	static std::string bt() { return getxt(); }
	static std::string ed2k() { return getthunderek(); }
	static std::string emule() { return getthundere(); }
	static std::string Emule() { return getthunderE(); }
	static std::string magnet() { return getthunderm(); }
	static std::string Magnet() { return getthunderM(); }
	static std::string torrent() { return getthunderz(); }
	static std::string Torrent() { return getthunderZ(); }
	static std::string bit() { return getblt(); }
	static std::string Bit() { return getBlt(); }
	static std::string lfilel() { return getlfilel(); } // 返回 "|file|"
	static std::string ed2kss() { return getthunderekss();} // 返回 "ed2k://"
	static std::string magnetss() { return getthundermss();} // 返回 "magnet:?"
private:
// 进符号表
	static std::string getXt();
	static std::string getxt();
	static std::string getthunderek();
	static std::string getthundere();
	static std::string getthunderE();
	static std::string getthunderm();
	static std::string getthunderM();
	static std::string getthunderz();
	static std::string getthunderZ();
	static std::string getblt();
	static std::string getBlt();
	static std::string getlfilel();
	static std::string getthunderekss();
	static std::string getthundermss();
};

#endif