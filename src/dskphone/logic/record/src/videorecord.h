#ifndef __VIDEO_RECORD_H__
#define __VIDEO_RECORD_H__

#ifdef IF_SUPPORT_VIDEO
#include "baserecord.h"

class CVideoRecord : public CAudioRecord
{
public:
    CVideoRecord();
    virtual ~CVideoRecord();

    // 操作执行
    virtual bool DoOperation(OPERATION_TYPE eOprType, const char * pFile = NULL,
                             void * pExtData = NULL, int iId = STORAGE_DEFAULT);

    // 空间转换成时长
    virtual long GetAvailableTime(unsigned long long dSize);

    // 消息处理
    virtual bool ProcessMsg(msgObject & msg);

private:
    // 文件夹名
    virtual yl::string GetFolder();

    // 额外的文件夹名
    virtual yl::string GetFolderEx();

    // 后缀名
    virtual yl::string GetSuffix();

    // 支持的格式
    virtual yl::string GetSupportFormat();
};
#endif // #ifdef IF_SUPPORT_VIDEO
#endif // __VIDEO_RECORD_H__
