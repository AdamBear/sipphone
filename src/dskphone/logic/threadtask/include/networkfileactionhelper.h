#ifndef TASKACTION_NETWORKFILEACTIONDOWNLOADHELPER_H_
#define TASKACTION_NETWORKFILEACTIONDOWNLOADHELPER_H_

//#include "http/http.h"
//#include <string>
#include <ylstl/ylstring.h>
#include "modthreadtask.h"
#include "dl/durl.h"
#include "taskaction/modtaskaction.h"
#include "threadtask/include/commonfunction.h"

using namespace NS_TA;

// 不正确的铃声文件index.
#define knInvalidRingFileIndex -1

#define _INIT_DL_HANDLE(x, error_code)  durl_handle_t x = -1; if(durl_create(&x)!=0) return error_code;
// 默认最大下载字节.VP_MAX_DOWN_LOAD 为http库中定义的大小
#if defined(IF_COMPILE_ARM) || defined(IF_COMPILE_MIPSEL) || defined(IF_COMPILE_ARMDSPG)
const int knDefaultMaxDownloadSize = 1.5 * 1024 * 1024; //VP_MAX_DOWN_LOAD;
#elif defined(IF_COMPILE_ARMV6JEL)
const int knDefaultMaxDownloadSize = 5 * 1024 * 1024; //VP_MAX_DOWN_LOAD;
#elif defined(IF_COMPILE_ARMARAGO)
const int knDefaultMaxDownloadSize = 60 * 1024 * 1024; //VP_MAX_DOWN_LOAD;
#else
const int knDefaultMaxDownloadSize = 60 * 1024 * 1024; //VP_MAX_DOWN_LOAD;
#endif
// CNetworkFileActionDownloadHelper类.
// 该类根据传入地址, 在下载时选择正确的协议类型下载.
class CNetworkFileActionDownloadHelper
{
public:
    CNetworkFileActionDownloadHelper();
    virtual ~CNetworkFileActionDownloadHelper();

    int HttpUpload(bool bIsPut = false);
    int FtpUploadFile(int iProtocolType, bool isAppend = false, bool isDelete = true);
    // 设置远程文件路径.
    inline void SetRemoteFilePath(const yl::string & strRemoteFilePath);
    // 设置本地保存路径.
    inline void SetSavedFilePath(const yl::string & strLoacalFilePath);

    //设置附加信息
    inline void SetExtraInfo(const yl::string & strExtraInfo);
    // 设置用户名.
    inline void SetFileUsername(const yl::string & strUser);
    void GetFileUsername(yl::string & strUser);
    // 设置密码.
    inline void SetFilePassword(const yl::string & strPwd);
    void GetFilePassword(yl::string & strPwd);
    //字符编码
    inline void SetCharset(const yl::string & strCharset);
    //设置头域
    inline void SetHeader(const yl::string & strHeader);

    // 如果存在重定向，则返回重定向的地址
    void GetRedirectURL(yl::string & strServer);

    // 下载文件
    // 如果下载成功返回TA_NO_ERROR, 否则返回具体错误.
    TaskActionErrorType DownloadFile();

    //上传文件
    TaskActionErrorType TA_PostFile(bool isPut);
    TaskActionErrorType TA_DeleteFile();

    // 下载铃声文件.
    // | pnRingIndex | [out] 本地铃声文件的index.
    // | pstrRingFilePath | [out] 下载后的铃声文件路径.
    // 如果下载成功返回TA_NO_ERROR, 否则返回具体错误.
    TaskActionErrorType DownloadRingFile(int * pnRingIndex, yl::string * pstrRingFilePath);

    // 设置下载模式
    void SetDownLoadMode(TASKACTION_DL_MODE eMode);
    // 获取下载模式
    TASKACTION_DL_MODE GetDownLoadMode();
    // 设置最下载文件大小
    void SetMaxDownloadSize(int nSize);

    inline void SetDownloadTimeout(int uDownloadTimeout)
    {
        m_uDownloadTimeout = uDownloadTimeout;

    }
    inline void SetConnectTimeout(int uConnectTimeout)
    {
        m_uConnectTimeout = uConnectTimeout;
    }

    TaskActionErrorType UploadFile(bool bIsPut);

    /**
     * @brief 将文件读取到buffer.函数内部申请，需要外部释放
     *
     * @author Song
     * @date 2017/9/19
     *
     * @param          szFilePath Full pathname of the file.
     * @param [in,out] szBuff     If non-null, the buffer.
     *
     * @return The buffer in file.
     */

    //static int GetBufferInFile(const char * szFilePath, char * szBuff);

    /**
     * @brief Sets an option.
     *
     * @author Song
     * @date 2017/12/1
     *
     * @param dHandle          The handle.
     * @param iSsltrust        (Optional) Zero-based index of the ssltrust.
     * @param iMaxDownloadSize (Optional) Size of the maximum download.
     * @param iTimeOut         (Optional) Zero-based index of the time out.
     */

    static void SetOption(durl_handle_t dHandle, int iSsltrust = -1, int iMaxDownloadSize = -1, unsigned int uConnectTimeout = 0, unsigned int uDownloadTimeout = 0);

    inline void SetRedirect(bool bRedirect)
    {
        m_bRedirect = bRedirect;
    }
protected:
    TaskActionErrorType TA_PostFile(int nMethod);


    // 下载HTTP文件.
    int HttpDownload(NetworkProtocolType eProtocolType, int nMaxDownloadSize = knDefaultMaxDownloadSize);

    // 下载FTP文件.
    int FtpDownload(NetworkProtocolType eProtocolType, int nMaxDownloadSize = knDefaultMaxDownloadSize);

    // 下载铃声文件.
    // | pnRingIndex | [out] 本地铃声文件的index.
    // | pstrRingFilePath | [out] 下载后的铃声文件路径.
    // 如果下载成功返回TA_NO_ERROR, 否则返回具体错误码.
    TaskActionErrorType RingFileDownload(int * pnRingIndex, yl::string * pstrRingFilePath);

    // 替换掉http或者https下载时地址中的特殊字符.
    // | strInput | 是输入字符串.
    // 返回替换后的字符串.
    yl::string RelpaceSpecialChar(const yl::string & strInput);

    // 从URL中解析出用户名/密码信息，并将其从URL中移除
    // 返回: true--有解析出
    bool ParseUserInfoFromURL();

    TaskActionErrorType GetTaskActionError(int iCode);

    // 转换成下载库模式
    durl_http_auth_e TransDownLoadMode(TASKACTION_DL_MODE eMode);

    yl::string m_strExtraInfo;          //附加信息
    yl::string m_strFilePath;           // 本地保存路径

    yl::string m_strFilePathRemote; // 远程文件路径.

    yl::string m_strUser;               // 用户名.
    yl::string m_strPwd;                // 密码.
    yl::string m_strCharset; //字符编码
    yl::string m_strHeader;//头域

    yl::string m_strRedirectURL;
    int m_nMaxDownloadSize;                // 下载限制的文件大小
    TASKACTION_DL_MODE m_eDownLoadMode;    // 下载模式
    bool m_bRedirect;
private:
    unsigned int m_uConnectTimeout;
    unsigned int m_uDownloadTimeout;
};  // class CNetworkFileActionHelper

inline void CNetworkFileActionDownloadHelper::SetRemoteFilePath(const yl::string &
        strRemoteFilePath)
{
    if (m_strFilePathRemote != strRemoteFilePath)
    {
        m_strFilePathRemote = strRemoteFilePath;
    }
}

inline void CNetworkFileActionDownloadHelper::SetSavedFilePath(const yl::string & strLoacalFilePath)
{
    if (m_strFilePath != strLoacalFilePath)
    {
        m_strFilePath = strLoacalFilePath;
    }
}

inline void CNetworkFileActionDownloadHelper::SetExtraInfo(const yl::string & strExtraInfo)
{
    if (m_strExtraInfo != strExtraInfo)
    {
        m_strExtraInfo = strExtraInfo;
    }
}

inline void CNetworkFileActionDownloadHelper::SetFileUsername(
    const yl::string & strUser)
{
    if (m_strUser != strUser)
    {
        m_strUser = strUser;
    }
}

inline void CNetworkFileActionDownloadHelper::SetFilePassword(
    const yl::string & strPwd)
{
    if (m_strPwd != strPwd)
    {
        m_strPwd = strPwd;
    }
}

//字符编码
inline void CNetworkFileActionDownloadHelper::SetCharset(const yl::string & strCharset)
{
    m_strCharset = strCharset;
}

inline void CNetworkFileActionDownloadHelper::SetHeader(const yl::string & strHeader)
{
    m_strHeader = strHeader;
}

// 第一次使用下载助手需要一些初始化
void InitDLConf();
#endif  // TASKACTION_NETWORKFILEACTIONDOWNLOADHELPER_H_
