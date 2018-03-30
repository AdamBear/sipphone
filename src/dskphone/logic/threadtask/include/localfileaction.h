#ifndef TASKACTION_LOCALFILEACTION_H_
#define TASKACTION_LOCALFILEACTION_H_

#include "fileaction.h"

// 本地文件读取类
class CLocalFileAction : public CFileAction
{
public:
    // 构造函数.
    // | strFilePath | 为本地文件路径
    CLocalFileAction(const yl::string & strLocalFilePath);
    virtual ~CLocalFileAction();

    // 重写
    virtual bool Exec();

    /***
     * @brief : Exec过程中产生的资源回收
     * @param : |bCallBackSucceed| 是否回调成功
     * @notice: 涉及异步, 只能操作自身资源
     */
    virtual bool ReleaseData(bool bCallBackSucceed)
    {
        return false;
    }

private:
    // 重写. 读取文件内容
    virtual bool Read();

    // 重写. 写入文件.
    virtual bool Write();

};  // class CLocalFileAction


#endif  // TASKACTION_LOCALFILEACTION_H_
