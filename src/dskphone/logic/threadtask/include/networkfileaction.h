#ifndef TASKACTION_NETWORKFILEACTION_H_
#define TASKACTION_NETWORKFILEACTION_H_

#include "fileaction.h"
#include "threadtask/include/common.h"

class CNetworkFileActionDownloadHelper;

// 远程文件读写类.
class CNetworkFileAction : public CFileAction
{
public:
    // 构造函数.
    // | strLocalFilePath | 是远程文件将要存放在本地的路径.
    // | strRemoteFilePath | 是远程文件的地址.
    CNetworkFileAction(const yl::string & strLocalFilePath, const yl::string & strRemoteFilePath);
    virtual ~CNetworkFileAction();

    // 获取远程文件路径
    inline const yl::string & GetFilePathRemote() const
    {
        return m_strFilePathRemote;
    }

    // 设置用户名.
    inline void SetUsername(const yl::string & strUser);
    yl::string GetUserName();
    // 设置密码.
    inline void SetPassword(const yl::string & strPassword);
    yl::string GetPassword();
    //字符编码
    inline void SetCharset(const yl::string & strCharset);
    //设置头域
    inline void SetHeader(const yl::string & strHeader);

    void GetRedirectURL(yl::string & strURL);

    // 获取是否检查Url.
    // 如果检查则返回true, 否则返回FALSE.
    inline bool IsCheckUrl() const;
    // 设置是否检查Url.
    // | bCheck | 为true去检查, 否则不检查.
    inline void SetCheckUrl(bool bCheck);

    inline bool IsSendMess2UI();
    // 设置参数
    inline void SetExtraParam(int iParam);
    // 获取参数
    inline int GetExtraParam();
    inline void SetSendMess2UI(bool isSend);

    inline void SetMaxDownloadSize(int nSize);

    inline void SetXSIAuthMode(bool bSipXsi);
    // 设置下载模式
    inline void SetDownLoadMode(TASKACTION_DL_MODE eMode);

    void SetExtraInfo(const yl::string & strExtraInfo);

    void SetRedirect(bool bRedirect);

    inline void SetDownloadTimeout(int uConnectTimeout, int uDownloadTimeout = 0);

    inline int GetConnectTimeout()
    {
        return m_uConnectTimeout;
    }
    inline int GetDownloadTimeout()
    {
        return m_uDownloadTimeout;
    }

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

    void SetDelete(bool isDelete);

    void SetPut(bool isPut);

    bool IsPut() const;

    bool IsDelete();

protected:
    // 用于dsskey中url类型,验证文件内容是否是OK还是NOK
    TaskActionErrorType CheckURLData(char * pURL);

private:
    // 重写. 读文件.
    virtual bool Read();

    // 重写. 写文件.
    virtual bool Write();

    // 检查Url是否有效.
    // 返回URL_Status状态.
    bool CheckUrl();

public:
    CNetworkFileActionDownloadHelper * GetDownloadHelper();

    yl::string m_strFilePathRemote;  // 远程文件路径地址.
    yl::string m_strUsername;  // 用户名.
    yl::string m_strPassword;  // 密码.
    yl::string m_strCharset; //字符编码
    yl::string m_strHeader;//头域
    bool m_isCheckUrl;  // 为true则是去判断Url.
    bool m_isDelete; //是否为delete方法
    bool m_isPut;
    bool m_isSend; //是否向UI发送结构
    int  m_iExtraParam; // 额外存储的参数

    int m_nMaxDownloadSize; // 设置下载的最大值
    CNetworkFileActionDownloadHelper * m_pHelper; // 下载助手.
    TASKACTION_DL_MODE m_eDownLoadMode;           // 下载模式

private:
    int m_uConnectTimeout;
    int m_uDownloadTimeout;

};  // class CRemoteFileAction
inline void CNetworkFileAction::SetSendMess2UI(bool isSend)
{
    m_isSend = isSend;
}

inline bool CNetworkFileAction::IsSendMess2UI()
{
    return  m_isSend;
}

inline bool CNetworkFileAction::IsDelete()
{
    return m_isDelete;
}

inline void CNetworkFileAction::SetDelete(bool isDelete)
{
    m_isDelete = isDelete;
}

inline void CNetworkFileAction::SetPut(bool isPut)
{
    m_isPut = isPut;
}

inline bool CNetworkFileAction::IsPut() const
{
    return m_isPut;
}

inline void CNetworkFileAction::SetUsername(const yl::string & strUser)
{
    if (m_strUsername != strUser)
    {
        m_strUsername = strUser;
    }
}

inline void CNetworkFileAction::SetPassword(const yl::string & strPassword)
{
    if (m_strPassword != strPassword)
    {
        m_strPassword = strPassword;
    }
}

inline bool CNetworkFileAction::IsCheckUrl() const
{
    return m_isCheckUrl;
}

inline void CNetworkFileAction::SetCheckUrl(bool bCheck)
{
    if (m_isCheckUrl != bCheck)
    {
        m_isCheckUrl = bCheck;
    }
}

// 设置参数
inline void CNetworkFileAction::SetExtraParam(int iParam)
{
    m_iExtraParam = iParam;
}

// 获取参数
inline int CNetworkFileAction::GetExtraParam()
{
    return m_iExtraParam;
}
//字符编码
inline void CNetworkFileAction::SetCharset(const yl::string & strCharset)
{
    m_strCharset = strCharset;
}

inline void CNetworkFileAction::SetHeader(const yl::string & strHeader)
{
    m_strHeader = strHeader;
}


inline void CNetworkFileAction::SetMaxDownloadSize(int nSize)
{
    m_nMaxDownloadSize = nSize;
}

inline void CNetworkFileAction::SetXSIAuthMode(bool bSipXsi)
{
    if (bSipXsi)
    {
        SetDownLoadMode(MODE_SIP_XSI_AUTH);
    }
}

inline void CNetworkFileAction::SetDownLoadMode(TASKACTION_DL_MODE eMode)
{
    m_eDownLoadMode = eMode;
}

inline void CNetworkFileAction::SetDownloadTimeout(int uConnectTimeout, int uDownloadTimeout /* = 0 */)
{
    m_uDownloadTimeout = uDownloadTimeout;
    m_uConnectTimeout = uConnectTimeout;
}

#endif  // TASKACTION_NETWORKFILEACTION_H_
