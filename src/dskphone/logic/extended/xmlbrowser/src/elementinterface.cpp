#ifdef IF_FEATURE_GENBAND

#include "logic_inc.h"
#include "threadtask/include/commonfunction.h"
#include "dl/durl.h"
#ifdef IF_FEATURE_XMLBROWSER

CElementInterface::CElementInterface(void)
{
}

CElementInterface::~CElementInterface(void)
{
}

bool CElementInterface::AfterCreate()
{
    // 播放提示音
    if (IsNeedPlayBeep())
    {
        _XMLBrowserManager.PlayBeep();
    }

    //发送请求到服务器
    PostInterfaceMessage();

    //结果提示
    ResultTip();
}

bool CElementInterface::IsNeedPlayBeep()
{
    return  _GetXData().bBeep;
}

void CElementInterface::PostInterfaceMessage()
{
    // 文件路径最大长度.
    const int knMaxFilePathLengh = 500;

    // 拷贝远程文件地址.
    yl::string strUrl = _GetXData().strUrl;

    TA_INFO("PostFile: \n remotePath[%s]", strUrl.c_str());

    durl_http_method_e iMethod = DURL_HTTP_METHOD_GET;
    yl::string strMethod = _GetXData().strMethod;
    if (strcmp("Get", strMethod.c_str()) == 0)
    {
        iMethod = DURL_HTTP_METHOD_GET;
    }
    else if (strcmp("Post", strMethod.c_str()) == 0)
    {
        iMethod = DURL_HTTP_METHOD_POST;
    }
    else if (strcmp("Put", strMethod.c_str()) == 0)
    {
        iMethod = DURL_HTTP_METHOD_PUT;
    }
    else if (strcmp("Delete", strMethod.c_str()) == 0)
    {
        iMethod = DURL_HTTP_METHOD_DELETE;
    }

    // 初始化参数.
    _MallocInit(pUrl_info, durl_url_t);
    durl_result_t response;
    memset(&response, 0, sizeof(durl_result_t));

    _strncpy(pUrl_info->url, strUrl.c_str(), DURL_MAX_URL_LEN);
    pUrl_info->http_mode = iMethod;
    yl::string strbody = _GetXData().strRequestData;
    yl::string strHead = _GetXData().strHttpHeaders;
    char * pBody = new char[4096];
    char * pHead = new char[4096];
    if (!strbody.empty())
    {
        strcpy(pBody, (char *)strbody.c_str());
        pUrl_info->body = pBody;
    }

    if (!strHead.empty())
    {
        strcpy(pHead, (char *)strHead.c_str());
        pUrl_info->header = pHead;
    }

    XMLBROWSER_INFO("PostInterfaceMessage method[%d] url[%s]", iMethod, strUrl.c_str());

    int iRet = durl_open(0, &response, pUrl_info);

    XMLBROWSER_INFO("PostInterfaceMessage durl open ret[%d] response code[%d] respones mesg[%s]", iRet, response.code, response.message);

    durl_close(0);
    durl_free_result(&response);
    MyDelete(pUrl_info);

    delete []pBody;
    delete []pHead;

    if (iRet != DURL_SUCCESS)
    {
        iRet = response.code;
        if (iRet == DURL_ERR_AUTHORIZED)
        {
            iRet = TA_HTTP_DOWNLOAD_UNAUTHORIZED;
        }
    }

    m_iErrorCode = iRet;
}

void CElementInterface::ResultTip()
{
    // 取消提示框
    _XMLBrowserManager.ExitMessageBox();

    if (m_iErrorCode == 0)
    {
        _XMLBrowserManager.ShowMessageBox(XB_TIP_ACTIVATED, 3000);

        dsskey_SetDsskeyStatus(GetDsskeyId(), DS_XMLBROSER_LOAD_FINISH);
    }
    else
    {
        _XMLBrowserManager.ShowMessageBox(XB_TIP_NOT_ACTIVATED, 3000);
        dsskey_SetDsskeyStatus(GetDsskeyId(), DS_XMLBROSER_LOAD_ERROE);
    }

    dsskey_refreshDSSKeyUI(GetDsskeyId());

    //在这边要设置一个定时器
    dsskey_SetTimer(GetDsskeyId(), 3000);
}

void CElementInterface::BeforeDestroy()
{

}

#endif // #ifdef IF_FEATURE_XMLBROWSER

#endif // #ifdef IF_FEATURE_GENBAND


