#ifndef TASKACTION_CUPLOADFILEACTION_H_
#define TASKACTION_CUPLOADFILEACTION_H_

#include "networkfileaction.h"



class CUploadFileAction : public CNetworkFileAction
{
public:
    // 构造函数.
    // | strLocalFilePath | 要上传的本地文件.
    // | strServerURL | 服务器地址.
    CUploadFileAction(const yl::string & strLocalFilePath, const yl::string & strServerURL);
    virtual ~CUploadFileAction();

private:
    // 读文件
    virtual bool Read();

    // 写文件
    virtual bool Write();

};

#endif

