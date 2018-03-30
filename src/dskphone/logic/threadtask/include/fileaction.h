#ifndef TASKACTION_FILEACTION_H_
#define TASKACTION_FILEACTION_H_

#include "taskaction/taskaction.h"


using  namespace NS_TA;

// 文件读写类, 该类不可直接使用, 需要使用具体的子类.
class CFileAction : public CTaskAction
{
public:
    // 获取buffer中的内容.
    inline const yl::string & GetFileContent() const;
    // 设置buffer中的内容.
    // | strContent | 是要设置的文件内容.
    inline void SetFileContent(const yl::string & strContent);

    // 获取文件路径.
    inline const yl::string & GetFilePath() const;
    // 设置文件路径.
    // | strPath | 是要设置的文件路径.
    inline void SetFilePath(const yl::string & strPath);

protected:
    CFileAction(const yl::string & strFilePath);
    virtual ~CFileAction();

    // 重写.
    virtual bool Exec();

    /***
     * @brief : Exec过程中产生的资源回收
     * @param : |bCallBackSucceed| 是否回调成功
     * @notice: 涉及异步, 只能操作自身资源
     */
    virtual bool ReleaseData(bool bCallBackSucceed) = 0;

    // 读文件
    virtual bool Read() = 0;

    // 写文件
    virtual bool Write() = 0;

private:
    yl::string m_strFilePath;  // 本地路径
    yl::string m_strFileContent;  // 文件内容存放的buffer.
};

inline const yl::string & CFileAction::GetFileContent() const
{
    return m_strFileContent;
}

inline void CFileAction::SetFileContent(const yl::string & strContent)
{
    if (m_strFileContent != strContent)
    {
        m_strFileContent = strContent;
    }
}

inline const yl::string & CFileAction::GetFilePath() const
{
    return m_strFilePath;
}

inline void CFileAction::SetFilePath(const yl::string & strPath)
{
    if (m_strFilePath != strPath)
    {
        m_strFilePath = strPath;
    }
}



#endif  // TASKACTION_FILEACTION_H_
