#ifndef TASKACTION_RINGFILEACTION_H_
#define TASKACTION_RINGFILEACTION_H_

#include <ylstl/ylstring.h>
#include "taskaction/taskaction.h"

using namespace NS_TA;
class CNetworkFileActionDownloadHelper;

// CRingFlieAction类, 获取铃声文件.
// CRingFileAction下载铃声文件到本地, 或者找到铃声文件的index, 但并不打开铃声文件.
class CRingFileAction : public CTaskAction
{
public:
    // 构造函数.
    // | strFilePath | 是铃声文件的地址.
    explicit CRingFileAction(const yl::string & strFilePath);
    virtual ~CRingFileAction();

    // 重写.
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

    // 获取铃声文件下载完毕后的保存路径.
    inline yl::string GetLocalSavedPath() const;

    // 获取铃声文件index.
    inline int GetRingIndex() const;

    //设置Info
    void SetExtraInfo(const yl::string & strExtraInfo);

private:
    // 下载铃声文件.
    // 如果成功将铃声文件下载到本地则返回true, 反之返回false.
    bool Download();

    // 上传铃声文件.
    // 如果Download返回true, 同时铃声index | m_nRingIndex | 又不等于knInvalidRingFileIndex,
    // 则表示成功下载了铃声文件.
    bool Upload();

    yl::string m_strExtraInfo;          //附加的Info信息
    yl::string m_strFilePath;  // 铃声文件的路径, 它是铃声的URL路径.
    yl::string m_strLocalSavedPath;  // 本地保存路径.
    int m_nRingIndex;  // 铃声文件的index.
    CNetworkFileActionDownloadHelper * m_pHelper; // 下载助手.
};  // class CRingFileAction

inline yl::string CRingFileAction::GetLocalSavedPath() const
{
    return m_strLocalSavedPath;
}

inline int CRingFileAction::GetRingIndex() const
{
    return m_nRingIndex;
}


#endif  // TASKACTION_RINGFILEACTION_H_
