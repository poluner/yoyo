#ifndef _DATA_OUT_PUT_H__
#define _DATA_OUT_PUT_H__
#include "utility/define.h"

class IResource;
class IDataManagerWriteData
{
public:
    // 调用后，调用方再拥有缓冲区使用权限，而且缓冲区必须是IDataMemoryManager一次分配的.
    virtual int32_t OutputDataAndReleaseBuffer(IResource* presource, char** ppbuffer, uint64_t file_pos,  uint32_t length) = 0;
    virtual ~IDataManagerWriteData() {};
    virtual int32_t OutputThunderzFile(IResource* presource, char** ppbuffer, uint64_t file_pos,  uint32_t length, const char * url_host)
    {return 0;}
};


#endif

