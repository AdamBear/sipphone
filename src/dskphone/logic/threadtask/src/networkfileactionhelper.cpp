#include <ETLLib/ETLLib.hpp>
#include <interfacedefine.h>
#include <configparser/modconfigparser.h>
#include <configiddefine.h>
#include "networkfileactionhelper.h"
#include "commonapi/stringhelperapi.h"
#include "commonfunction.h"
#include <devicelib/networkserver.h>
#include <stdio.h>
#include "ylstl/ylstringalgorithm.h"
#include "dl/durl_tool.h"

//Reference to : http://ccrma.stanford.edu/courses/422/projects/WaveFormat/
const char g_szWaveFormatPCMU[] =
{
    'W', 'A', 'V', 'E',     //WAVE
    'f', 'm', 't', ' ',     //"fmt "
    0x12, 0x00, 0x00, 0x00, //Subchunk1Size
    0x07, 0x00,             //AudioFormat=7 (PCMU)
    0x01, 0x00,             //Number of Channels (1), 单声道
    0x40, 0x1f, 0x00, 0x00, //SampleRate=8000
    0x40, 0x1f, 0x00, 0x00, //Byte Rate=8000
    0x01, 0x00,
};

const char g_szWaveFormatPCMA[] =
{
    'W', 'A', 'V', 'E',     //WAVE
    'f', 'm', 't', ' ',     //"fmt "
    0x12, 0x00, 0x00, 0x00, //Subchunk1Size
    0x06, 0x00,             //AudioFormat=6 (PCMA)
    0x01, 0x00,             //Number of Channels (1), 单声道
    0x40, 0x1f, 0x00, 0x00, //SampleRate=8000
    0x40, 0x1f, 0x00, 0x00, //Byte Rate=8000
    0x01, 0x00,
};

enum
{
    AUDIO_FMT_PCMU = 0,
    AUDIO_FMT_PCMA,
};

static bool CompareAudioFmt(const char* strFileBuff, int iFmt)
{
    if (NULL == strFileBuff)
    {
        return false;
    }

    if (AUDIO_FMT_PCMU == iFmt)
    {
        for (int i = 0; i < sizeof(g_szWaveFormatPCMU); i++)
        {
            if (strFileBuff[i] != g_szWaveFormatPCMU[i])
            {
                return false;
            }
        }
    }
    else if (AUDIO_FMT_PCMA == iFmt)
    {
        for (int i = 0; i < sizeof(g_szWaveFormatPCMA); i++)
        {
            if (strFileBuff[i] != g_szWaveFormatPCMA[i])
            {
                return false;
            }
        }
    }

    return true;
}

static bool IsAudioFile(const char * szFilePath)
{
    if (NULL == szFilePath)
    {
        return false;
    }

    yl::string strFileContent = yl::dump_file(szFilePath);

    const char * buffer = strFileContent.c_str();
    buffer += 8;

    return CompareAudioFmt(buffer, AUDIO_FMT_PCMU)
           || CompareAudioFmt(buffer, AUDIO_FMT_PCMA);
}

static void MD5ToAssic(unsigned char * pSzsource, /* 16 bytes */
                       char * pDest) /* 33 bytes */
{
    for (int i = 0; i < 16; i++)
    {
        snprintf((char *) &pDest[i * 2], 3, "%02x", pSzsource[i]);
    }
}

// 文件路径最大长度.
const int knMaxFilePathLengh = 500;

/**
* @下载库全局变量设置
*/
static void set_option_Init(void);

//////////////////////////////////////////////////////////////////////////
// Class CNetworkFileActionDownloadHelper
CNetworkFileActionDownloadHelper::CNetworkFileActionDownloadHelper()
{
    m_nMaxDownloadSize = knDefaultMaxDownloadSize;
    m_eDownLoadMode = MODE_NONE;
    m_bRedirect = false;
    m_uConnectTimeout = configParser_GetConfigInt(kszHttpDownLoadTimeOut);
    m_uDownloadTimeout = 0;
}

CNetworkFileActionDownloadHelper::~CNetworkFileActionDownloadHelper()
{

}

TaskActionErrorType CNetworkFileActionDownloadHelper::DownloadFile()
{
    // 格式化网络URL地址
    ParseUserInfoFromURL();

    // 获取文件的协议类型，获取下载文件类型.
    NetworkProtocolType eProtocolType = GetNetworkFileProtocolType(m_strFilePathRemote);
    if (eProtocolType <= NETWORK_PROTOCOL_NONE || eProtocolType >= NETWORK_PROTOCOL_MAX)
    {
        return TA_PROTOCOL_NOT_SUPPORTED;
    }

    // 选择相应的协议下载函数.
    int nRetVal = 0xFF;//FAIL;
    switch (eProtocolType)
    {
    case NETWORK_HTTP:
    case NETWORK_HTTPS:
        nRetVal = HttpDownload(eProtocolType, m_nMaxDownloadSize);
        break;
    case NETWORK_FTP:
    case NETWORK_TFTP:
        nRetVal = FtpDownload(eProtocolType, m_nMaxDownloadSize);
        break;
    default:
        nRetVal = 0xFF;//FAIL;
    }

    if (nRetVal == 0)
    {
        return TA_NO_ERROR;
    }
    else if (nRetVal < 0 || nRetVal > TA_ERROR_MAX)
    {
        return TA_OTHER_ERROR;
    }
    else
    {
        return (TaskActionErrorType)nRetVal;
    }

}

TaskActionErrorType CNetworkFileActionDownloadHelper::DownloadRingFile(int * pnRingIndex,
        yl::string * pstrRingFilePath)
{
    if (pnRingIndex == NULL || pstrRingFilePath == NULL)
    {
        return TA_OTHER_ERROR;
    }

    // 下载铃声.
    return RingFileDownload(pnRingIndex, pstrRingFilePath);
}

int CNetworkFileActionDownloadHelper::HttpDownload(NetworkProtocolType eProtocolType, int nMaxDownloadSize /* = knDefaultMaxDownloadSize */)
{
    m_strRedirectURL = "";
    // 远程路径名为空, 或者本地保存路径为空.
    if (m_strFilePathRemote.empty() || m_strFilePath.empty())
    {
        return TA_FILE_INVALID_PATH;
    }

    _INIT_DL_HANDLE(dHandle, TA_MEMORY_ERROR);

    if (nMaxDownloadSize <= 0)
    {
        nMaxDownloadSize = knDefaultMaxDownloadSize;
    }

    // T2x证书路径
    // 是否开启Https
    bool bDisableTrustCert = configParser_GetConfigInt(kszDisableHTTPSTrustCertificates) == 1;
    unsigned int ssl_trust = bDisableTrustCert ? 0 : configParser_GetConfigInt(
                                 kszHTTPSTrustCertificates);

    int iTimeOut = eProtocolType == NETWORK_HTTP ? configParser_GetConfigInt(kszHttpDownLoadTimeOut) : configParser_GetConfigInt(kszHttpsDownLoadTimeOut);

    SetOption(dHandle, ssl_trust, nMaxDownloadSize, m_uConnectTimeout, m_uDownloadTimeout);

    yl::string strRemoteCpy = EncodeHttp(m_strFilePathRemote);

    // 拷贝文件在本地存储的地址.
    char szLocalFilePath[knMaxFilePathLengh] = {0};
    _strncpy(szLocalFilePath, m_strFilePath.c_str(), knMaxFilePathLengh);


    _MallocInit(pDlInfo, durl_url_t);
    pDlInfo->http_mode = DURL_HTTP_METHOD_GET;
    _strncpy(pDlInfo->url, strRemoteCpy.c_str(), DURL_MAX_URL_LEN);
    _strncpy(pDlInfo->username, m_strUser.c_str(), DURL_MAX_USERINFO_LEN);
    _strncpy(pDlInfo->password, m_strPwd.c_str(), DURL_MAX_USERINFO_LEN);
    pDlInfo->http_auth = TransDownLoadMode(GetDownLoadMode());

    if (m_bRedirect)
    {
        pDlInfo->no_redirect = m_bRedirect;
    }

    durl_result_t      response;
    memset(&response, 0, sizeof(durl_result_t));

    TA_INFO("http download url[%s] user[%s] passwd[%s] http_autp[%d] method[%d]", pDlInfo->url, "****", "****", pDlInfo->http_auth, pDlInfo->http_mode);

    int iRet = durl_download_to_file(dHandle, &response, pDlInfo, szLocalFilePath);

    TA_INFO("http durl_download_to_file:ret [%d] code_msg[%s],code:%d", iRet, response.message, response.code);

    // 保存重定向地址
    if (0 == iRet
            && strlen(response.redirect) != 0)
    {
        m_strRedirectURL = response.redirect;
    }

    iRet = GetTaskActionError(response.code);

    durl_destroy(dHandle);
    durl_free_result(&response);
    MyDelete(pDlInfo);

    return iRet;
}

int CNetworkFileActionDownloadHelper::FtpDownload(NetworkProtocolType eProtocolType, int
        nMaxDownloadSize /* = knDefaultMaxDownloadSize */)
{
    m_strRedirectURL = "";
    TA_INFO("FtpDownload protocoltype[%d] m_strFilePathRemote[%s]", eProtocolType, m_strFilePathRemote.c_str());

    // 远程路径名为空, 或者本地保存路径为空.
    if (m_strFilePathRemote.empty() || m_strFilePath.empty())
    {
        return TA_FILE_INVALID_PATH;
    }

    _INIT_DL_HANDLE(dHandle, TA_MEMORY_ERROR);

    // 获取服务器等信息.
    yl::string strUser = "";
    yl::string strPwd = "";
    yl::string strDefaultPort = eProtocolType == NETWORK_FTP ? "21" : "69";
    yl::string strRemoteFile = "";
    yl::string strPort = "";
    yl::string strServer = "";
    ParserURLPath(m_strFilePathRemote, strUser, strPwd, strServer, strRemoteFile, strPort, strDefaultPort);

    // 调用该函数之前已经解析过url,这里的m_strFilePathRemote已不包含用户名和密码，需把之前的用户名和密码再赋上
    strUser = m_strUser;
    strPwd = m_strPwd;

    // 拷贝文件在本地存储的地址.
    char szLocalFilePath[knMaxFilePathLengh] = {0};
    _strncpy(szLocalFilePath, m_strFilePath.c_str(), knMaxFilePathLengh);

    if (nMaxDownloadSize <= 0)
    {
        nMaxDownloadSize = knDefaultMaxDownloadSize;
    }
    SetOption(dHandle, -1, nMaxDownloadSize, m_uConnectTimeout, m_uDownloadTimeout);

    //需要拼接url server:port/file
    strRemoteFile = eProtocolType == NETWORK_FTP ? commonAPI_FormatString("ftp://%s:%s/%s", strServer.c_str(), strDefaultPort.c_str(), strRemoteFile.c_str())
                    : commonAPI_FormatString("tftp://%s:%s/%s", strServer.c_str(), strDefaultPort.c_str(), strRemoteFile.c_str());

    TA_INFO("ftp download [RemoteFile[%s], LocalFilePath[%s], user = %s",
            strRemoteFile.c_str(), szLocalFilePath, strUser.c_str());


    _MallocInit(pUrlInfo, durl_url_t);
    durl_result_t response;
    memset(&response, 0, sizeof(response));

    _strncpy(pUrlInfo->url, strRemoteFile.c_str(), DURL_MAX_URL_LEN);
    _strncpy(pUrlInfo->username, strUser.c_str(), DURL_MAX_USERINFO_LEN);
    _strncpy(pUrlInfo->password, strPwd.c_str(), DURL_MAX_USERINFO_LEN);

    int iRet = durl_download_to_file(dHandle, &response, pUrlInfo, szLocalFilePath);

    TA_INFO("ftp download:ret [%d] code_msg[%s],code:%d", iRet, response.message, response.code);

    iRet = response.code;

    durl_destroy(dHandle);
    durl_free_result(&response);
    MyDelete(pUrlInfo);

    if (NETWORK_FTP == eProtocolType
            &&  DURL_ERR_FTP_WRONG_PASSWD == iRet)
    {
        return TA_HTTP_DOWNLOAD_UNAUTHORIZED;
    }

    return iRet;
}

TaskActionErrorType CNetworkFileActionDownloadHelper::RingFileDownload(int * pnRingIndex,
        yl::string * pstrRingFilePath)
{
    if (pnRingIndex == NULL || pstrRingFilePath == NULL)
    {
        return TA_OTHER_ERROR;
    }

    _INIT_DL_HANDLE(dHandle, TA_MEMORY_ERROR);

    int nMaxFileSize = 200 * 1024;

    durl_set_option(dHandle, DURL_OPTION_MAX_FILESIZE, nMaxFileSize);

    TA_INFO("RingFileDownload [%s]", m_strFilePathRemote.c_str());

    // 复制铃声文件远程路径.
    yl::string strRingFileUrl = m_strFilePathRemote;
    char szRingFileLocalPath[knMaxFilePathLengh] = {0};

    //计算文件名
    unsigned char uMD5Buffer[16] = {0};
    char szHash[33] = {0};

    durl_md5_encrypt((unsigned char *)strRingFileUrl.c_str(), uMD5Buffer, 16);
    MD5ToAssic(uMD5Buffer, szHash);
    snprintf(szRingFileLocalPath, sizeof(szRingFileLocalPath), TEMP_PATH "%s.wav", szHash);

    _MallocInit(pUrlInfo, durl_url_t);
    _strncpy(pUrlInfo->url, strRingFileUrl.c_str(), DURL_MAX_URL_LEN);

    durl_result_t response;
    memset(&response, 0, sizeof(response));

    int nRetVal = durl_download_to_file(dHandle, &response, pUrlInfo, szRingFileLocalPath);

    TA_INFO("durl_download_to_file:ret [%d] code_msg[%s],code:%d local file path [%s]", nRetVal, response.message, response.code, szRingFileLocalPath);

    TaskActionErrorType eResult = TA_OTHER_ERROR;

    if (IsAudioFile(szRingFileLocalPath))  //下载到了铃声文件
    {
        eResult = TA_NO_ERROR;
        *pstrRingFilePath = szRingFileLocalPath;
        *pnRingIndex = knInvalidRingFileIndex;
    }
    else  // 没有下载到铃声, 并且也没有返回铃声index.
    {
        *pstrRingFilePath = "";
        *pnRingIndex = knInvalidRingFileIndex;
        eResult = TA_OTHER_ERROR;
    }

    durl_destroy(dHandle);
    durl_free_result(&response);
    MyDelete(pUrlInfo);
    return eResult;
}

yl::string CNetworkFileActionDownloadHelper::RelpaceSpecialChar(const yl::string & strInput)
{
    yl::string strRet;

    struct stTemp
    {
        yl::string m_strSrc;  // 源字符串.
        yl::string m_strDest;  // 目标字符串.
    };

    static const stTemp stReplaceList[] =
    {
        {"%", "%25"},
        {" ", "%10"},
        {"\"", "%22"},
        {"<", "%3C"},
        {">", "%3E"},
        {"[", "%5B"},
        {"\\", "%5C"},
        {"]", "%5D"},
        {"^", "%5E"},
        {"`", "%60"},
        {"`", "%60"},
        {"{", "%7B"},
        {"|", "%7C"},
        {"}", "%7D"}
    };

    // 替换各个字符.
    strRet = strInput;
    for (size_t i = 0;
            i < sizeof(stReplaceList) / sizeof(stReplaceList[0]);
            ++i)
    {
        strRet = translateString(strRet, stReplaceList[i].m_strSrc, stReplaceList[i].m_strDest);
    }

    return strRet;
}


TaskActionErrorType CNetworkFileActionDownloadHelper::TA_PostFile(bool isPut)
{
    if (isPut)
    {
        return TA_PostFile(DURL_HTTP_METHOD_PUT);
    }
    else
    {
        return TA_PostFile(DURL_HTTP_METHOD_POST);
    }
}

TaskActionErrorType CNetworkFileActionDownloadHelper::TA_DeleteFile()
{
    return TA_PostFile(DURL_HTTP_METHOD_DELETE);
}

TaskActionErrorType CNetworkFileActionDownloadHelper::TA_PostFile(int nMethod)
{
    TA_INFO("CNetworkFileActionDownloadHelperEx::TA_PostFile[%d]", nMethod);
    _INIT_DL_HANDLE(dHandle, TA_MEMORY_ERROR);

    // T2x证书路径
    // 是否开启Https
    bool bDisableTrustCert = configParser_GetConfigInt(kszDisableHTTPSTrustCertificates) == 1;
    unsigned int ssl_trust = bDisableTrustCert ? 0 : configParser_GetConfigInt(
                                 kszHTTPSTrustCertificates);
    SetOption(dHandle, ssl_trust, -1, m_uConnectTimeout, m_uDownloadTimeout);

    yl::string strRemoteCpy = EncodeHttp(m_strFilePathRemote);

    int length = 0;

    char phead[512] = {0};

    yl::string strBody = "";
    //读取文件内容
    if (nMethod != DURL_HTTP_METHOD_DELETE)
    {
        strBody = yl::dump_file(m_strFilePath.c_str());
    }

    length = strBody.size();

    if (length < 0)
    {
        length = 0;
    }

    if (GetDownLoadMode() == MODE_FUZE_HOTELING_AUTH)
    {
        durl_set_option(dHandle, DURL_OPTION_ADDITION_BODY, m_strExtraInfo.c_str());
        length += m_strExtraInfo.length();
    }

    if (!m_strHeader.empty())
    {
        sprintf(phead, "Content-Type:%s\r\n", m_strHeader.c_str());
    }
    else
    {
        sprintf(phead, "Content-Type:application/xml;charset=%s\r\nContent-Length:%d\r\n",
                m_strCharset.c_str(), length);

        if (GetDownLoadMode() == MODE_BASIC_AUTH)
        {
            sprintf(phead + strlen(phead), "%s\r\n", "SOAPAction: NA");
        }
    }
    //
    TA_INFO("PostFile:remotePath[%s] Header:[%s] AuthMode=[%d][%d]",
            strRemoteCpy.c_str(), phead, GetDownLoadMode(), TransDownLoadMode(GetDownLoadMode()));

    // 初始化参数.
    _MallocInit(pUrl_info, durl_url_t);

    durl_result_t response;
    memset(&response, 0, sizeof(durl_result_t));

    _strncpy(pUrl_info->url, strRemoteCpy.c_str(), DURL_MAX_URL_LEN);
    _strncpy(pUrl_info->username, m_strUser.c_str(), DURL_MAX_USERINFO_LEN);
    _strncpy(pUrl_info->password, m_strPwd.c_str(), DURL_MAX_USERINFO_LEN);

    pUrl_info->http_mode = (durl_http_method_e)nMethod;
    pUrl_info->body = (char *)strBody.c_str();
    pUrl_info->http_auth = TransDownLoadMode(GetDownLoadMode());
    pUrl_info->header = phead;
    pUrl_info->recv_len = knDefaultMaxDownloadSize;

    int iRet = -1;
    if (DURL_HTTP_METHOD_PUT == nMethod)
    {
        iRet = durl_upload(dHandle, &response, pUrl_info, m_strFilePath.c_str(), NULL);
    }
    else
    {
        iRet = durl_download_to_file(dHandle, &response, pUrl_info, m_strFilePath.c_str());
    }

    TA_INFO("http durl_download_to_file:ret [%d] code_msg[%s],code:%d", iRet, response.message, response.code);

    int iCode = response.code;

    durl_destroy(dHandle);
    durl_free_result(&response);
    MyDelete(pUrl_info);

    if (DURL_SUCCESS == iRet)
    {
        return TA_NO_ERROR;
    }
    else
    {
        iRet = iCode;
    }

    if (iRet == DURL_ERR_AUTHORIZED)
    {
        return TA_HTTP_DOWNLOAD_UNAUTHORIZED;
    }
    else if (iRet < 0 || iRet > TA_ERROR_MAX)
    {
        return TA_OTHER_ERROR;
    }
    else
    {
        return (TaskActionErrorType)iRet;
    }
}


TaskActionErrorType CNetworkFileActionDownloadHelper::UploadFile(bool bIsPut)
{
    ParseUserInfoFromURL();
    NetworkProtocolType eProtocolType = GetNetworkFileProtocolType(m_strFilePathRemote);
    if (eProtocolType <= NETWORK_PROTOCOL_NONE || eProtocolType >= NETWORK_PROTOCOL_MAX)
    {
        TA_INFO("DownloadFile: TA_PROTOCOL_NOT_SUPPORTED");
        return TA_PROTOCOL_NOT_SUPPORTED;
    }
    TA_INFO("UploadFile: eProtocolType = %d", eProtocolType);
    int nRetVal = -1;
    switch (eProtocolType)
    {
    case NETWORK_HTTP:
    case NETWORK_HTTPS:
        nRetVal = HttpUpload(bIsPut);
        break;
    case NETWORK_TFTP:
    case NETWORK_FTP:
        nRetVal = FtpUploadFile(eProtocolType);
        break;
    default:
        nRetVal = -1;
    }
    if (nRetVal == 0)
    {
        return TA_NO_ERROR;
    }
    else if (nRetVal < 0 || nRetVal > TA_ERROR_MAX)
    {
        return TA_OTHER_ERROR;
    }
    else
    {
        return (TaskActionErrorType)nRetVal;
    }
}

void CNetworkFileActionDownloadHelper::SetOption(durl_handle_t dHandle, int iSsltrust/* = -1*/, int iMaxDownloadSize/* = -1*/, unsigned int uConnectTimeOut/* = 0*/, unsigned int uDownloadTimeout/* = 0*/)
{
    TA_INFO("set download option ssl trust[%d],maxdownload size[%d] connect timeout[%d],download timeout[%d]", iSsltrust, iMaxDownloadSize, uConnectTimeOut, uDownloadTimeout);

    if (iSsltrust != -1)
    {
        durl_set_option(dHandle, DURL_OPTION_SSL_VERIFY_PEER, iSsltrust);
    }

    if (iMaxDownloadSize != -1)
    {
        durl_set_option(dHandle, DURL_OPTION_MAX_FILESIZE, iMaxDownloadSize);
    }

    durl_set_option(dHandle, DURL_OPTION_CONNECT_TIMEOUT, uConnectTimeOut);
    durl_set_option(dHandle, DURL_OPTION_OPERART_TIMEOUT, uDownloadTimeout);
}

int CNetworkFileActionDownloadHelper::HttpUpload(bool bIsPut/* = false*/)
{
    m_strRedirectURL = "";
    if (m_strFilePathRemote.empty() || m_strFilePath.empty())
    {
        return TA_FILE_INVALID_PATH;
    }
    TA_INFO("Http start [%s] upload file '%s' to '%s'.", bIsPut ? "put" : "post",
            m_strFilePath.c_str(), m_strFilePathRemote.c_str());

    durl_result_t ret_response;
    memset(&ret_response, 0, sizeof(durl_result_t));

    _MallocInit(url_info, durl_url_t);

    url_info->http_mode = bIsPut ? DURL_HTTP_METHOD_PUT : DURL_HTTP_METHOD_POST;
    _strncpy(url_info->url, m_strFilePathRemote.c_str(), DURL_MAX_URL_LEN);
    _strncpy(url_info->username, m_strUser.c_str(), DURL_MAX_USERINFO_LEN);
    _strncpy(url_info->password, m_strPwd.c_str(), DURL_MAX_USERINFO_LEN);

    //下载库内部规则：
    //    http post上传：规则为：http ://10.10.10.10/upload，指定的文件名为优先使用to_filename，如果to_filename为空则用form_name（去除绝对路径）,注该情况下服务器可能会要求重定向请求
    //    http put 上传：规则为：http ://10.10.10.10/upload，如果to_filename不为空，则URL为http://10.10.10.10/upload + / + to_filename，如果为空则表示直接put到http://10.10.10.10/upload
    //    ftp      上传：规则为：ftp ://10.10.10.10/upload + / + to_filename为空？为空使用form_name(去除绝对路径) ： 不为空使用to_filename
    //    tftp     上传：规则为：tftp ://10.10.10.10/upload + / + to_filename为空？为空使用form_name(去除绝对路径) ： 不为空使用to_filename
    int iRet = durl_upload(DURL_GLOBAL_HANDLE, &ret_response, url_info, m_strFilePath.c_str(), NULL);

    TA_INFO("durl_upload ret_response = %d, %s\n", ret_response.code,
            ret_response.message);



    if (0 == iRet && strlen(ret_response.redirect) != 0)
    {
        m_strRedirectURL = ret_response.redirect;
    }

    iRet = ret_response.code;
    MyDelete(url_info);

    durl_free_result(&ret_response);

    if (iRet == DURL_ERR_AUTHORIZED)
    {
        return TA_HTTP_DOWNLOAD_UNAUTHORIZED;
    }
    return iRet;
}

int CNetworkFileActionDownloadHelper::FtpUploadFile(int iProtocolType, bool isAppend/* = false*/,
        bool isDelete/* = true*/)
{
    if (m_strFilePathRemote.empty() || m_strFilePath.empty())
    {
        return TA_FILE_INVALID_PATH;
    }

    if (iProtocolType != NETWORK_FTP
            && iProtocolType != NETWORK_TFTP)
    {
        TA_WARN("eProtocolType[%d] error", iProtocolType);
        return TA_PROTOCOL_NOT_SUPPORTED;
    }

    _INIT_DL_HANDLE(dHandle, TA_MEMORY_ERROR);
    // 获取服务器等信息.
    yl::string strUser = "";
    yl::string strPwd = "";
    yl::string strDefaultPort = (iProtocolType == NETWORK_FTP) ? "21" : "69";
    yl::string strRemoteFile = "";
    yl::string strPort = "";
    yl::string strServer = "";
    ParserURLPath(m_strFilePathRemote, strUser, strPwd, strServer, strRemoteFile, strPort,
                  strDefaultPort);

    // 调用该函数之前已经解析过url,这里的m_strFilePathRemote已不包含用户名和密码，需把之前的用户名和密码再赋上
    strUser = m_strUser;
    strPwd = m_strPwd;

    /* 超时时间设置 */
    SetOption(dHandle, -1, -1, m_uConnectTimeout, m_uDownloadTimeout);

    /* upload file */

    yl::string strUrl = iProtocolType == NETWORK_FTP ? commonAPI_FormatString("ftp://%s:%s/", strServer.c_str(), strPort.c_str()) : commonAPI_FormatString("tftp://%s:%s/", strServer.c_str(), strPort.c_str());

    TA_INFO("dl_[%s]_send_file[RemoteFile[%s],LocalFilePath[%s], user = %s isAppend =[%d] isDelete=[%d]", (iProtocolType == NETWORK_FTP) ? "ftp" : "tftp"
            , strRemoteFile.c_str()
            , m_strFilePath.c_str()
            , strUser.c_str()
            , isAppend
            , isDelete
           );

    TA_INFO("upload url [%s] from path[%s] to path[%s]", strUrl.c_str(), m_strFilePath.c_str(), strRemoteFile.c_str());

    _MallocInit(pUrl_Info, durl_url_t);
    _strncpy(pUrl_Info->url, strUrl.c_str(), DURL_MAX_URL_LEN);
    _strncpy(pUrl_Info->username, strUser.c_str(), DURL_MAX_USERINFO_LEN);
    _strncpy(pUrl_Info->password, strPwd.c_str(), DURL_MAX_USERINFO_LEN);
    pUrl_Info->ftp_mode = isAppend ? DURL_FTP_METHOD_APPEND : DURL_FTP_METHOD_STOR;
    pUrl_Info->ftp_limit = isDelete ? DURL_FTP_LIMIT_DELETE : DURL_FTP_LIMIT_STOP;

    durl_result_t response;
    memset(&response, 0, sizeof(durl_result_t));

    int nRet = durl_upload(dHandle, &response, pUrl_Info, m_strFilePath.c_str(), strRemoteFile.c_str());

    TA_INFO("dl_upload: return %d\nsend_file code[%d] code_msg:%s", nRet, response.code, response.message);

    nRet = response.code;

    durl_destroy(dHandle);
    durl_free_result(&response);
    MyDelete(pUrl_Info);

    return nRet;
}

// 规格化远程URL. 如下载的用户名/密码信息，如果url中存在，则替换用户名/密码的成员
bool CNetworkFileActionDownloadHelper::ParseUserInfoFromURL()
{
    m_strFilePathRemote = commonAPI_TrimString(m_strFilePathRemote, " ");
    int nHdPos = m_strFilePathRemote.find("://");
    nHdPos = (nHdPos != yl::string::npos) ? nHdPos + 3 : 0;

    // http://user:password@hostip/filepath
    int nSlashPos = m_strFilePathRemote.find("/", nHdPos);

    // eg: lpszText = test@test:123@www.imailtone.com:80,解析出来的 strUserPin为test
    // 故修改为反向解析@标识符，@之前的为strUserPin，@之后的为hostname
    int nAtPos = m_strFilePathRemote.rfind("@", nSlashPos);
    if (nAtPos != yl::string::npos)
    {
        int nCommaPos = m_strFilePathRemote.find(":", nHdPos);
        if (nCommaPos != yl::string::npos
                && nCommaPos < nAtPos)
        {
            m_strUser = m_strFilePathRemote.substr(nHdPos, nCommaPos - nHdPos);
            m_strPwd = m_strFilePathRemote.substr(nCommaPos + 1, nAtPos - nCommaPos - 1);

            yl::string strTmp = m_strFilePathRemote.substr(0, nHdPos);
            yl::string strTmp2 = m_strFilePathRemote.substr(nAtPos + 1);
            m_strFilePathRemote = strTmp + strTmp2;

            return true;
        }
    }
    return false;
}

TaskActionErrorType CNetworkFileActionDownloadHelper::GetTaskActionError(int iCode)
{
    TaskActionErrorType eErrorType = (TaskActionErrorType) iCode;
    if (iCode == DURL_ERR_AUTHORIZED)
    {
        eErrorType = TA_HTTP_DOWNLOAD_UNAUTHORIZED;
    }
    else if (iCode == DURL_ERR_FILE_SIZE)
    {
        eErrorType = TA_FILE_EXCEED_MAX_SIZE;
    }
    else if (iCode == DURL_ERR_TOO_MANY_REDIRECTS)
    {
        eErrorType = TA_URL_REDIRECT;
    }
    else if (iCode == DURL_ERR_TFTP)
    {
        eErrorType = TA_URL_ERROR;
    }

    TA_INFO("GetTaskActionError code[%d] == error[%d]", iCode, eErrorType);

    return eErrorType;
}

// 如果存在重定向，则返回重定向的地址
void CNetworkFileActionDownloadHelper::GetRedirectURL(yl::string & strServer)
{
    strServer = m_strRedirectURL;
}

void CNetworkFileActionDownloadHelper::GetFileUsername(yl::string & strUser)
{
    strUser = m_strUser;
}

void CNetworkFileActionDownloadHelper::GetFilePassword(yl::string & strPwd)
{
    strPwd = m_strPwd;
}

void CNetworkFileActionDownloadHelper::SetMaxDownloadSize(int nSize)
{
    m_nMaxDownloadSize = nSize;
}

void CNetworkFileActionDownloadHelper::SetDownLoadMode(TASKACTION_DL_MODE eMode)
{
    m_eDownLoadMode = eMode;
}

TASKACTION_DL_MODE CNetworkFileActionDownloadHelper::GetDownLoadMode()
{
    return m_eDownLoadMode;
}

/**
* @下载库全局变量设置
*/
static void set_option_Init(void)
{
    static unsigned int ssl_method = configParser_GetConfigInt(kszSSLMethod);
    static unsigned int use_ipv6 = (0 == configParser_GetConfigInt(kszNetworkIPAddressMode) ? 0 : 1);
    static unsigned int max_redirect = 5;

    char bufUserAgent[512] = {0};

    sys_replace_wildcards((char *)configParser_GetConfigString(kszHttpUserAgent).c_str(),
                          SYS_WILDCARD_UNSPC, bufUserAgent, sizeof(bufUserAgent) / sizeof(char));

    yl::string strUserAgent = bufUserAgent;

    if (durl_set_option(DURL_GLOBAL_HANDLE, DURL_OPTION_USER_AGENT, strUserAgent.c_str()))
    {
        TA_WARN("set DL_OPTIONS_USER_AGENT ERR!");
    }

    // 设置证书使用的路径，新下载库需求
    char bufCert[100] = {0};
    strcpy(bufCert, CONFIG_PATH"/certs");
    int iType = configParser_GetConfigInt(kszHttpsCaCert);
    if (0 == iType)
    {
        // 默认证书
        strcat(bufCert, "/phoneca");
    }
    else if (2 == iType)
    {
        // 所有证书
        strcat(bufCert, "/ca");
    }
    /* 受信证书路径设置 */
    if (durl_set_option(DURL_GLOBAL_HANDLE, DURL_OPTION_SSL_CA_TRUST_USE_PATH, bufCert))
    {
        TA_WARN("set DL_OPTIONS_SSL_CA_TRUST_PATH ERR!");
    }

    const yl::string strCertCustomPath = CONFIG_PATH"/certs/server/WebServer.pem";
    const yl::string strCertPhonePath = CONFIG_PATH"/certs/phonedev/WebServer.pem";

    yl::string strCertPath = strCertPhonePath;
    iType = configParser_GetConfigInt(kszHttpsDevCert);
    if (1 == iType) //custom 模式
    {
        // 所有证书
        strCertPath = strCertCustomPath;
        // custom 模式，证书不存在，phone_setting.reserve_certs_config.enable=1，则设置为默认路径
        if (!pathFileExist(strCertCustomPath.c_str())
                && 1 == configParser_GetConfigInt(kszReserveCertsConfig))
        {
            strCertPath = strCertPhonePath;
        }
    }


    /* 服务器证书文件设置 */
    if (durl_set_option(DURL_GLOBAL_HANDLE, DURL_OPTION_SSL_CA_SERVER_USE_FILE, strCertPath.c_str()))
    {
        TA_WARN("set DL_OPTIONS_SSL_CA_SERVER_FILE ERR!");
    }

    // 所有采用TLS的应用程序是否强制验证服务器端证书的CommonName或SubjectAltName
    int iCNValidation = configParser_GetConfigInt(kszHttpsCNValidation);
    if (durl_set_option(DURL_GLOBAL_HANDLE, DURL_OPTION_SSL_VERIFY_HOST, iCNValidation))
    {
        TA_WARN("set DL_OPTIONS_SSL_CHECK_CN ERR!");
    }

    /* SSL方法设置 */
    if (durl_set_option(DURL_GLOBAL_HANDLE, DURL_OPTION_SSL_VERSION, ssl_method))
    {
        TA_WARN("set DL_OPTIONS_SSL_METHOD ERR!");
    }

    /* 是否使用ipv6设置 */
    if (durl_set_option(DURL_GLOBAL_HANDLE, DURL_OPTION_SUPPORT_IPV6, use_ipv6))
    {
        TA_WARN("set DL_OPTIONS_USEIPV6 ERR!");
    }
    /* 最大重定向次数 */
    if (durl_set_option(DURL_GLOBAL_HANDLE, DURL_OPTION_MAX_REDIRECT, max_redirect))
    {
        TA_WARN("set DL_OPTIONS_MAX_REDIRECT ERR!");
    }

    yl::string strValue = configParser_GetConfigString(kszHttpsSSLCipherList);
    if (durl_set_option(DURL_GLOBAL_HANDLE, DURL_OPTION_SSL_CIPHER_STR, strValue.c_str()))
    {
        TA_WARN("set DL_OPTIONS_SSL_CIPHER_STR ERR!");
    }
}

void TaskActionDownloadLogCallBack(int module, int level, const char * fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    LOG_VLOG(module, level, fmt, ap);
    va_end(ap);
    return;
}

void InitDLConf()
{
    static bool bHadInit = false;
    if (!bHadInit)
    {
        durl_init(DBG_LOG_WIN, TaskActionDownloadLogCallBack);
        set_option_Init();                // 初始化选项配置
        bHadInit = true;
    }
}


durl_http_auth_e CNetworkFileActionDownloadHelper::TransDownLoadMode(TASKACTION_DL_MODE eMode)
{
    durl_http_auth_e eDownLoadMode;
    switch (eMode)
    {
    case MODE_NONE:
        eDownLoadMode = DURL_HTTP_AUTH_PAIRING;
        break;
    case MODE_MUTIL_AUTH:
        eDownLoadMode = DURL_HTTP_AUTH_PAIRING;
        break;
    case MODE_SIP_XSI_AUTH:
        eDownLoadMode = DURL_HTTP_AUTH_SIP_XSI;
        break;
    case MODE_BASIC_AUTH:
    case MODE_BASIC_XSI_AUTH:
        eDownLoadMode = DURL_HTTP_AUTH_BASIC;
        break;
    case MODE_FUZE_HOTELING_AUTH:
        eDownLoadMode = DURL_HTTP_AUTH_FUZE_HOTELING;
        break;
    default:
        eDownLoadMode = DURL_HTTP_AUTH_PAIRING;
        break;
    }
    return eDownLoadMode;
}

