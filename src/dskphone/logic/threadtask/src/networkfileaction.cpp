#include "networkfileaction.h"

//#include <fstream>
#include <stdio.h>

#include "commonfunction.h"
#include "taskaction/taskactioncallbackbase.h"
#include "networkfileactionhelper.h"
#include <configparser/modconfigparser.h>
#include <configiddefine.h>
#include "ylstl/ylstringalgorithm.h"


//////////////////////////////////////////////////////////////////////////
// Class CRemoteFileAction.

// Buffer最大长度.
static const int knMaxBufferLen = 260;

CNetworkFileAction::CNetworkFileAction(const yl::string & strLocalFilePath,
                                       const yl::string & strRemoteFilePath)
    : CFileAction(strLocalFilePath),
      m_strFilePathRemote(strRemoteFilePath),
      m_strCharset("UTF-8"),
      m_strHeader(""),
      m_isCheckUrl(false),
      m_pHelper(NULL),
      m_isDelete(false),
      m_isSend(true),
      m_isPut(false),
      m_eDownLoadMode(MODE_NONE)
{
    // 初始化基类的请求类型值.
    m_eTARequestType = TA_REQUEST_NETWORK_FILE;
    m_iExtraParam = -1;

    m_nMaxDownloadSize = knDefaultMaxDownloadSize;

    m_uConnectTimeout = -1;
    m_uDownloadTimeout = -1;

    // 实例化下载助手.
    if (m_pHelper == NULL)
    {
        m_pHelper = new CNetworkFileActionDownloadHelper;
    }
}

CNetworkFileAction::~CNetworkFileAction()
{
    SAFE_DELETE(m_pHelper);
}

bool CNetworkFileAction::Exec()
{
    // 调用基类的Exec, 以决定读还是写.
    return CFileAction::Exec();
}

bool CNetworkFileAction::Read()
{
    if (m_isCheckUrl)
    {
        return CheckUrl();
    }
    // 如果下载助手未实例化, 则返回TA_MEMORY_ERROR错误.
    if (m_pHelper == NULL)
    {
        SetTaskActionErrorType(TA_MEMORY_ERROR);
        return false;
    }

    // 设置下载文件信息
    m_pHelper->SetRemoteFilePath(m_strFilePathRemote);
    m_pHelper->SetSavedFilePath(GetFilePath());
    m_pHelper->SetFileUsername(m_strUsername);
    m_pHelper->SetFilePassword(m_strPassword);
    m_pHelper->SetCharset(m_strCharset);
    m_pHelper->SetDownLoadMode(m_eDownLoadMode);
    m_pHelper->SetMaxDownloadSize(m_nMaxDownloadSize);

    if (GetConnectTimeout() >= 0)
    {
        m_pHelper->SetConnectTimeout(m_uConnectTimeout);
    }
    if (GetDownloadTimeout() < 0)
    {
        m_pHelper->SetDownloadTimeout(m_uDownloadTimeout);
    }

    // 开始下载
    TaskActionErrorType eResult = m_pHelper->DownloadFile();
    if (eResult != TA_NO_ERROR)
    {
        SetTaskActionErrorType(eResult);
        TA_INFO("download Fail");
        return false;
    }

    // 如果下载成功则将文件内容读取到buffer中.
    FILE * ifFile = fopen(GetFilePath().c_str(), "r");
    if (!ifFile)
    {
        SetTaskActionErrorType(TA_FILE_FAILED_TO_OPEN);
        return false;
    }

    // 获取文件长度.
    fseek(ifFile, 0L, SEEK_END);
    int nLength = ftell(ifFile);
    fseek(ifFile, 0L, SEEK_SET);
    if (nLength <= 0)
    {
        SetTaskActionErrorType(TA_FILE_NO_DATA_WHEN_READ);
        fclose(ifFile);
        return false;
    }

    // 将文件内容写入Buffer
    char * pszBuffer = new char [nLength + 1];
    memset(pszBuffer, 0, nLength + 1);

    // 将文件内容写入Buffer
    fread(pszBuffer, 1, nLength, ifFile);
    fclose(ifFile);

    SetFileContent(pszBuffer);
    DELETE_ARRAY(pszBuffer);
    SetTaskActionErrorType(TA_NO_ERROR);

    return true;
}

bool CNetworkFileAction::CheckUrl()
{
    // 替换特殊字符.
    yl::string strUrl = EncodeHttp(m_strFilePathRemote);

    char szBuffer[knMaxBufferLen + 1] = {0};
    strncpy(szBuffer, strUrl.c_str(), knMaxBufferLen);
    szBuffer[knMaxBufferLen] = '\0';

    // 设置结果.
    SetTaskActionErrorType(CheckURLData(szBuffer));
    return true;
}

// 用于dsskey中url类型,验证文件内容是否是OK还是NOK
TaskActionErrorType CNetworkFileAction::CheckURLData(char * pURL)
{
    if (strncmp("http://", pURL, 7) != 0
            && strncmp("https://", pURL, 8) != 0
       )
    {
        TA_WARN("Invalid url - must start with 'http://'");
        return TA_URL_ERROR;
    }
    BOOL bIsHttp = TRUE;

    _INIT_DL_HANDLE(dHandle, TA_MEMORY_ERROR);

    if (strncmp("http://", pURL, 7) == 0)
    {
        pURL += 7;
    }
    else
    {
        pURL += 8;
        bIsHttp = FALSE;
        // T2x证书路径
        // 是否开启Https
        unsigned int ssl_trust = configParser_GetConfigInt(kszHTTPSTrustCertificates);

        /* 是否启用受信任证书设置 */
        TA_INFO("ssl_trust[%d]", ssl_trust);
        durl_set_option(dHandle, DURL_OPTION_SSL_VERIFY_PEER, ssl_trust);
    }

    char * pTempURL = pURL;
    pTempURL = strchr(pTempURL, '@');
    char * pUser = NULL;
    char * pPasswd = NULL;

    // 解决用户名和密码
    if (NULL != pTempURL)
    {
        pPasswd = strchr(pURL, ':');
        if (NULL != pPasswd)
        {
            pUser = pURL;
            *pTempURL = '\0';
            *pPasswd = '\0';
            pPasswd += 1;
            pURL = pTempURL + 1;
        }
    }

    _MallocInit(urlInfo, durl_url_t);
    durl_result_t response;
    memset(&response, 0, sizeof(durl_result_t));

    char bufURL[512] = {0};

    sprintf(bufURL, "http%s://%s", bIsHttp ? "" : "s", pURL);

    yl::string strLocalFilePath = "/tmp/checkurl.tmpfile";
    urlInfo->http_mode = DURL_HTTP_METHOD_GET;

    _strncpy(urlInfo->url, bufURL, DURL_MAX_URL_LEN);
    if (NULL != pPasswd)
    {
        _strncpy(urlInfo->password, pPasswd, DURL_MAX_USERINFO_LEN);
    }
    if (NULL != pUser)
    {
        _strncpy(urlInfo->username, pUser, DURL_MAX_USERINFO_LEN);
    }

    int iRet = durl_download_to_file(dHandle, &response, urlInfo, strLocalFilePath.c_str());
    TA_INFO("check url, ret= %d result code = %d, msg = %s", iRet, response.code, response.message);

    if (0 != response.code)
    {
        durl_destroy(dHandle);
        durl_free_result(&response);
        MyDelete(urlInfo);
        return TA_URL_BLANK_NRECOGNIZED;
    }

    yl::string strBuffer = yl::dump_file(strLocalFilePath.c_str());

    int iLength = strBuffer.size();

    const char * pData = strBuffer.c_str();
    if (iLength >= 3
            && (unsigned char) strBuffer[0] == 0xEF && (unsigned char) strBuffer[1] == 0xBB
            && (unsigned char) strBuffer[2] == 0xBF)
    {
        // 跳过UTF8的标志
        pData += 3;
        iLength -= 3;
    }

    TaskActionErrorType eResult = TA_URL_ERROR;

    // 判断文件内容，必须为OK或NOK
    if (strncmp("OK", pData, 2) == 0 && iLength == 2)
    {
        eResult = TA_NO_ERROR;
    }
    else if (strncmp("NOK", pData, 3) == 0 && iLength == 3)
    {
        eResult = TA_URL_US_NOKTEXT;
    }

    remove(strLocalFilePath.c_str());
    durl_destroy(dHandle);
    durl_free_result(&response);
    MyDelete(urlInfo);
    return eResult;
}

bool CNetworkFileAction::Write()
{
    if (m_isCheckUrl)
    {
        return CheckUrl();
    }

    // 如果下载助手未实例化, 则返回TA_MEMORY_ERROR错误.
    if (m_pHelper == NULL)
    {
        SetTaskActionErrorType(TA_MEMORY_ERROR);
        return false;
    }

    m_pHelper->SetRemoteFilePath(m_strFilePathRemote);
    m_pHelper->SetSavedFilePath(GetFilePath());
    m_pHelper->SetFileUsername(m_strUsername);
    m_pHelper->SetFilePassword(m_strPassword);
    m_pHelper->SetCharset(m_strCharset);
    m_pHelper->SetHeader(m_strHeader);
    m_pHelper->SetDownLoadMode(m_eDownLoadMode);
    TaskActionErrorType eResult = TA_FILE_OPERATION_FALIED;
    if (IsDelete())
    {
        eResult = m_pHelper->TA_DeleteFile();
    }
    else
    {
        eResult = m_pHelper->TA_PostFile(IsPut());
    }

    if (eResult != TA_NO_ERROR)
    {
        SetTaskActionErrorType(eResult);
        return false;
    }

    SetTaskActionErrorType(TA_NO_ERROR);


    return true;
}

void CNetworkFileAction::GetRedirectURL(yl::string & strURL)
{
    if (NULL != m_pHelper)
    {
        m_pHelper->GetRedirectURL(strURL);
    }
}

yl::string CNetworkFileAction::GetPassword()
{
    yl::string strPwd = "";
    if (NULL != m_pHelper)
    {
        m_pHelper->GetFilePassword(strPwd);
    }
    return strPwd;
}

yl::string CNetworkFileAction::GetUserName()
{
    yl::string strUserName = "";
    if (NULL != m_pHelper)
    {
        m_pHelper->GetFileUsername(strUserName);
    }
    return strUserName;
}

CNetworkFileActionDownloadHelper * CNetworkFileAction::GetDownloadHelper()
{
    return m_pHelper;
}

void CNetworkFileAction::SetExtraInfo(const yl::string & strExtraInfo)
{
    if (m_pHelper != NULL)
    {
        m_pHelper->SetExtraInfo(strExtraInfo);
    }
}

void CNetworkFileAction::SetRedirect(bool bRedirect)
{
    if (m_pHelper != NULL)
    {
        m_pHelper->SetRedirect(bRedirect);
    }
}

