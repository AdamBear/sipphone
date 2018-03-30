#include "logic_inc.h"

#ifdef IF_FEATURE_XMLBROWSER

yl::string xGetUniqueLoadName()
{
    static int s_iTempFileId = 0;
    if (s_iTempFileId >= MAX_TEMP_FILE)
    {
        s_iTempFileId = 0;
    }
    // 生成本地保存路径.
    yl::string strFormat(TEMP_PATH);
    strFormat.append(szTempFile);
    char szFileId[8] = {0};
    sprintf(szFileId, "%d", ++s_iTempFileId);
    strFormat.append(szFileId);
    strFormat.append(".xml");

    XMLBROWSER_INFO("GetFileName(%s)", strFormat.c_str());
    return strFormat;
}

yl::string GetImageFileName()
{
    static int s_iTempFileId = 0;
    if (s_iTempFileId >= MAX_TEMP_FILE)
    {
        s_iTempFileId = 0;
    }
    // 生成本地保存路径.
    yl::string strFormat(TEMP_PATH"xmlBrowser/");
    pathCreateDirectory(strFormat.c_str());

    strFormat.append("image");
    char szFileId[8] = {0};
    sprintf(szFileId, "%d", ++s_iTempFileId);
    strFormat.append(szFileId);
    strFormat.append(".dob");

    XMLBROWSER_INFO("GetFileName(%s)", strFormat.c_str());
    return strFormat;
}

//caijz到时试下t2x还需不需要这样的路径
yl::string GetRingFileName()
{
    static int s_iTempFileId = 0;
    if (s_iTempFileId >= MAX_TEMP_FILE)
    {
        s_iTempFileId = 0;
    }
    // 生成本地保存路径.
    yl::string strFormat(TEMP_PATH);
    strFormat.append("Ring");
    char szFileId[8] = {0};
    sprintf(szFileId, "%d", ++s_iTempFileId);
    strFormat.append(szFileId);
    strFormat.append(".wav");

    XMLBROWSER_INFO("GetFileName(%s)", strFormat.c_str());
    return strFormat;
}
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_GETINSTANCE(CXMLBrowserLoadMng)

CXMLBrowserLoadMng::CXMLBrowserLoadMng()
{
}

CXMLBrowserLoadMng::~CXMLBrowserLoadMng()
{

}

bool CXMLBrowserLoadMng::LoadFile(const char * lpszPath, int nOptions, LOAD_FILE_TYPE eFileType,
                                  int nElemID, void * pExtData/* = NULL*/)
{
    CHECK_NULL(lpszPath, false);
    if (0 == nOptions)
    {
        XMLBROWSER_INFO("not specified load option");
        return false;
    }

    yl::string strLoadFilePath = lpszPath;
    // 下载
    if (IS_LOAD(nOptions, OPT_DOWNLOAD))
    {
        if (!Download(strLoadFilePath, eFileType, nOptions))
        {
            XMLBROWSER_INFO("download failed");
            return false;
        }
    }
    // 解析
    else if (IS_LOAD(nOptions, OPT_XML_PARSE))
    {
        if (eFileType == XB_FILE_XML)
        {
            ParseXml(strLoadFilePath);
        }
        else
        {
            XMLBROWSER_INFO("not support file type to parse");
            return false;
        }
    }
    // 压缩
    else if (IS_LOAD(nOptions, OPT_COMPRESS))
    {
        if (eFileType == XB_FILE_IMAGE)
        {
            CHECK_NULL(pExtData, false);
            CompressImage(strLoadFilePath, *(CompressImgParam *)pExtData);
        }
        else
        {
            XMLBROWSER_INFO(" not support file type to compress");
            return false;
        }
    }

    // 添加任务到列表，是否有重得添加函数中会有判断
    AddLoadParam(strLoadFilePath, nOptions, eFileType, nElemID);
    return true;
}

bool CXMLBrowserLoadMng::CancelLoad()
{
    ReleaseLoadParam();
    return true;
}

// 重写回调函数
void CXMLBrowserLoadMng::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    if (NULL == pTaskAction)
    {
        return;
    }

#if IF_BUG_23718
    // 如果是上传返回结果，释放action
    CUploadFileAction * pUploadAction = dynamic_cast<CUploadFileAction *>(pTaskAction);
    if (pUploadAction != NULL)
    {
        return;
    }
#endif

    TaskActionRequestType eTaskReqType = pTaskAction->GetReqType();
    if (eTaskReqType == TA_REQUEST_NETWORK_FILE)
    {
        // 下载之后的处理
        OnDownloadDone(pTaskAction);
    }
    else if (eTaskReqType == TA_REQUEST_XML_BROWSER)
    {
        CXMLBrowserAction * pXMLAction = dynamic_cast<CXMLBrowserAction *>(pTaskAction);
        if (pXMLAction == NULL)
        {
            return;
        }

        if (pXMLAction->GetLoadActionType() == OPT_XML_PARSE)
        {
            // XML文件解析之后的处理
            OnParseXmlDone(pTaskAction);
        }
        else if (pXMLAction->GetLoadActionType() == OPT_COMPRESS)
        {
            // 图片压缩结束的处理
            OnCompressImageDone(pTaskAction);
        }
    }
}

//
bool CXMLBrowserLoadMng::Download(yl::string & strNetFilePath,
                                  LOAD_FILE_TYPE eLoadFileType/* = XB_FILE_XML*/, int nOptions/* = 0*/)
{
    yl::string strFileName;
    // caijz铃声名称是否有关需要验证
    if (eLoadFileType == XB_FILE_WAV)
    {
        strFileName = GetRingFileName();
    }
    else
    {
        strFileName = xGetUniqueLoadName();
    }
    XMLBROWSER_INFO("Download Filename is [%d][%s][%s]", eLoadFileType, strFileName.c_str(),
                    strNetFilePath.c_str());

#if IF_FEATURE_XMLBROWSER_HOTELING
    if (configParser_GetConfigInt(kszHotelingAuthMode) == 1
            && IS_LOAD(nOptions, OPT_HOTELING))
    {
        CNetworkFileAction * pAction = TaskAction_CreateNetworkFileAction(strFileName.c_str(), strNetFilePath.c_str());
        if (pAction == NULL)
        {
            return false;
        }

        yl::string strUser = configParser_GetConfigString(kszHotelingUserName);
        yl::string strPwd = configParser_GetConfigString(kszHotelingPassword);
        const yl::string strAuthInfo = commonAPI_FormatString("{\"UserName\": \"%s\",\"Password\": \"%s\"}", strUser.c_str(), strPwd.c_str());
        pAction->SetExtraInfo(strAuthInfo);

        strUser = configParser_GetConfigString(kszAutoProvisionUsername);
        strPwd = configParser_GetConfigString(kszAutoProvisionPassword);

        pAction->SetUsername(strUser);
        pAction->SetPassword(strPwd);
        pAction->SetSync(false);  // 设置异步.
        pAction->SetCallback(this);  // 设置回调类.
        pAction->SetHeader("application/xml;charset=UTF-8");
        pAction->SetDownLoadMode(MODE_FUZE_HOTELING_AUTH);
        pAction->SetRead(false);
        pAction->SetPut(false);
        return TaskAction_ExecAction(pAction);  // 开始请求
    }
#endif

    // 实例化CNetworkFileAction.
    CNetworkFileAction * pAction = TaskAction_CreateNetworkFileAction(strFileName.c_str(),
                                   strNetFilePath.c_str());
    if (pAction == NULL)
    {
        return false;
    }

    yl::string  strUser = configParser_GetConfigString(kszXmlBrowserUser);
    yl::string strPwd = configParser_GetConfigString(kszXmlBrowserPassword);

#if IF_FEATURE_XMLBROWSER_HOTELING
    if (IS_LOAD(nOptions, OPT_HOTELING))
    {
        strUser = configParser_GetConfigString(kszHotelingUserName);
        strPwd = configParser_GetConfigString(kszHotelingPassword);
    }
#endif

    pAction->SetUsername(strUser);
    pAction->SetPassword(strPwd);
    pAction->SetSync(false);  // 设置异步.
    pAction->SetRead(true);  // 设置读.
    pAction->SetCallback(this);  // 设置回调类.
    bool isSucceed = TaskAction_ExecAction(pAction);  // 开始请求
    return isSucceed;
}

void CXMLBrowserLoadMng::OnDownloadDone(CTaskAction * pTaskAction)
{
    // 检查参数是否合法
    CNetworkFileAction * pAction = dynamic_cast<CNetworkFileAction *>(pTaskAction);
    if (pAction == NULL)
    {
        return;
    }

    xmlbrowser_CloseLoading();
    // 下载到本地的文件路径
    yl::string strXmlFilePath = pAction->GetFilePath();
    XMLBROWSER_INFO("NetworkFile download result id: %d", pTaskAction->GetTaskActionErrorType());

    // 先判断任务是否还存在
    LoadParam * pParam = GetLoadParam(pAction->GetFilePathRemote());
    if (pParam == NULL)
    {
        XMLBROWSER_INFO("abandon download result [%s]", pAction->GetFilePathRemote().c_str());
        // DeleteLoadParam(strXmlFilePath);
        // 没有LoadParam，则删除下载到的本地文件，可能调用者已经取消了此任务
        remove(strXmlFilePath.c_str());
        return;
    }

    // 将下载的临时文件保存到参数中
    pParam->strTmpPath = strXmlFilePath;
    // 是否出错
    if (pTaskAction->GetTaskActionErrorType() != NS_TA::TA_NO_ERROR)
    {
#if IF_FEATURE_XMLB_SCREENSAVER
        if (pParam->bScreenSaverType)
        {
            PressScreenSaverError();
        }
#endif
        // 提示下载失败
        _XMLBrowserManager.ExitMessageBox();
#if IF_FEATURE_XMLBROWSER_HOTELING
        if (IS_LOAD(pParam->nOptions, OPT_HOTELING))
        {
            if (_XMLBrowserManager.IsHotelingLogin())
            {
                _XMLBrowserManager.ShowMessageBox(XB_TIP_HOTELING_LOGIN_FAIL, 3000);
            }
            else
            {
                _XMLBrowserManager.ShowMessageBox(XB_TIP_HOTELING_LOGOUT_FAIL, 3000);
            }
        }
        else
#endif
        {
            _XMLBrowserManager.ShowMessageBox(XB_TIP_LOAD_FAIL, 3000);
        }
        // !通过将数据置NULL来通知对应的对象下载失败了
        _ElementManager.OnLoadFile(pParam->nOptions, pParam->nElementID, NULL);
        // 删除任务
        DeleteLoadParam(pParam->strPath);
        return;
    }

    switch (pParam->eFileType)
    {
    case XB_FILE_XML:
        {
            // xml文件大小限制
            int iFileSize = pathFileSize(strXmlFilePath.c_str());
            if (iFileSize > XML_FILE_SIZE)
            {
#if IF_FEATURE_XMLB_SCREENSAVER
                if (pParam->bScreenSaverType)
                {
                    PressScreenSaverError();
                }
#endif
                _XMLBrowserManager.ExitMessageBox();
                _XMLBrowserManager.ShowMessageBox(XB_TIP_LARGE_FILE, 3000);
                DeleteLoadParam(pParam->strPath);
                return;
            }

            // 下载成功后再开启新线程解析文件
            ParseXml(strXmlFilePath.c_str());

            // 将下载路径改为解析任务的路径，解析完成赋给自己的参数
            pParam->strPath = strXmlFilePath;
        }
        break;
    case XB_FILE_IMAGE:
        {
            // 图片
            char szTmpPath[512] = {0};
            strncpy(szTmpPath, pParam->strTmpPath.c_str(), 511);

            _ElementManager.OnLoadFile(pParam->nOptions, pParam->nElementID, (void *)szTmpPath);

            DeleteLoadParam(pParam->strPath);

            /*
            int iFileSize = pathFileSize(strXmlFilePath.c_str());
            if (iFileSize > IMAGE_FILE_SIZE)
            {
                _XMLBrowserManager.ExitMessageBox();
                _XMLBrowserManager.ShowMessageBox(XB_TIP_LARGE_IMAGE, 3000);
                // 超出限制则删除
                //_XMLBrowserManager.DeleteTempFile(strXmlFilePath.c_str());

            //             _DownloadStateManager.DeleteState(strXmlFilePath);
                return;
            }

            // 将文件路径传给UI，UI确定如何显示
            XMLBROWSER_INFO("[XmlBrowser]::Image Download Success FilePath[%s]", strXmlFilePath.c_str());
            _XMLBrowserManager.UpdateWnd();

            // 判断压缩结果（相应错误提示由UI给出）
            yl::string strScaleRet = yl::dump_file(TEMP_PATH "imageScale.out");
            if (strScaleRet != "1")
            {
                // 压缩失败就删掉图片，不再多做其他处理
                //_XMLBrowserManager.DeleteTempFile(strXmlFilePath.c_str());

            //             _DownloadStateManager.DeleteState(strXmlFilePath);
                return;
            }

            // 播放提示音
            //         if (_ElementManager.GetCurrentElement() != NULL)
            {
            //             if (_ElementManager.GetCurrentElement()->IsNeedPlayBeep())
                {
                    _XMLBrowserManager.PlayBeep();
                }
                msgSendMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), XB_MSG_DESTROY_DSSKEY, XMT_IMAGESCREEN, 0);
            }
            */
        }
        break;

    case XB_FILE_WAV:
        {
            _XMLBrowserManager.PlayWav(strXmlFilePath);
            // 删除下载任务, 下载到的铃声不能删除, 删除操作由播放完再删
            // http://10.3.5.199/Bug.php?BugID=70633
            DeleteLoadParam(pParam->strPath, false);
        }
        break;

    }
}

// 解析XML
bool CXMLBrowserLoadMng::ParseXml(const yl::string & strPath)
{
    // const char *_strFilePath = (lpszPath == NULL) ? szXmlBrowser : lpszPath;
    // 实例化TaskAction.
    CXMLParseAction * pAction = new CXMLParseAction(strPath.c_str());
    if (pAction == NULL)
    {
        return false;
    }

    pAction->SetSync(false);    // 设置异步.
    pAction->SetCallback(this); // 设置回调类.
    bool isSucceed = TaskAction_ExecAction(pAction);  // 开始请求.
    return isSucceed;
}

// 解析完成
void CXMLBrowserLoadMng::OnParseXmlDone(CTaskAction * pTaskAction)
{
    // 检查参数是否合法
    CXMLParseAction * pAction = dynamic_cast<CXMLParseAction *>(pTaskAction);
    if (pAction == NULL)
    {
        return;
    }

    yl::string strXmlFilePath = pAction->GetFileName();
    LoadParam * pParam = GetLoadParam(strXmlFilePath);
    if (pParam != NULL)
    {
        pParam->strTmpPath = strXmlFilePath;
    }

    // 判断状态
    if (pAction->GetErrorCode() != CXMLParseAction::ERR_OK)
    {
#if IF_FEATURE_XMLB_SCREENSAVER
        if (pParam != NULL
                && pParam->bScreenSaverType)
        {
            PressScreenSaverError();
        }
#endif
        XMLBROWSER_INFO("Parse XML Error. [%d]", pAction->GetErrorCode());
        _XMLBrowserManager.ShowMessageBox(XB_TIP_PARSE_FAIL, 3000);
        DeleteLoadParam(strXmlFilePath);
        return;
    }

    if (pParam != NULL)
    {
        XMLParseData * pParseData = pAction->TakeoutData();
        pParseData->nDsskeyId = pParam->nDsskeyId;

#if IF_FEATURE_XMLB_SCREENSAVER
        //url下载的内容不是屏保格式内容则不再解析
        if (pParam->bScreenSaverType &&
                pParseData->eType != XMT_SCREENSAVER)
        {
            PressScreenSaverError();

            ReleaseParseData(pParseData);
            pParseData = NULL;
            DeleteLoadParam(strXmlFilePath);
            return;
        }
#endif

        if (!_ElementManager.OnLoadFile(pParam->nOptions, pParam->nElementID, pParseData))
        {
            if (pParseData != NULL)
            {
                ReleaseParseData(pParseData);
                pParseData = NULL;
            }
        }
    }
    else
    {
        XMLBROWSER_INFO("abandon parsed task [%s]", strXmlFilePath.c_str());
    }

    DeleteLoadParam(strXmlFilePath);
}


// 压缩图片
bool CXMLBrowserLoadMng::CompressImage(const yl::string & strPath,
                                       const CompressImgParam & refImgParam)
{
    CImageCompressAction * pAction = new CImageCompressAction(strPath, refImgParam);
    if (pAction == NULL)
    {
        return false;
    }

    pAction->SetSync(false);    // 设置异步.
    pAction->SetCallback(this); // 设置回调类.
    bool isSucceed = TaskAction_ExecAction(pAction);  // 开始请求.
    return isSucceed;
}

void CXMLBrowserLoadMng::OnCompressImageDone(CTaskAction * pTaskAction)
{
    // 检查参数是否合法
    CImageCompressAction * pAction = dynamic_cast<CImageCompressAction *>(pTaskAction);
    if (pAction == NULL)
    {
        return;
    }

    yl::string strXmlFilePath = pAction->GetImageFile();

    // 判断状态
    if (pAction->GetErrorCode() != CImageCompressAction::ERR_OK)
    {
        XMLBROWSER_INFO("Compress error [%d]", pAction->GetErrorCode());
    }

    LoadParam * pParam = GetLoadParam(strXmlFilePath);
    if (pParam != NULL)
    {
        _ElementManager.OnLoadFile(pParam->nOptions, pParam->nElementID, &pAction->GetParam());

        DeleteLoadParam(strXmlFilePath);
    }
    else
    {
        XMLBROWSER_INFO("abandon parsed task [%s]", strXmlFilePath.c_str());
    }
}

// 添加
bool CXMLBrowserLoadMng::AddLoadParam(const yl::string & strPath, int nOptions,
                                      LOAD_FILE_TYPE eFileType, int nElemID)
{
    if (GetLoadParam(strPath) != NULL)
    {
        // skip to add same load param
        return true;
    }

    LoadParam * pLoadParam = new LoadParam;
    CHECK_NULL(pLoadParam, false);

    pLoadParam->strPath = strPath;
    pLoadParam->nOptions = nOptions;
    pLoadParam->eFileType = eFileType;
    pLoadParam->nElementID = nElemID;
    pLoadParam->nDsskeyId = _XMLBrowserManager.GetEnterDsskeyId();
    // 记录后重置dsskeyID,否则后续任何使用xmlbrowser都认为是该dsskey触发
    _XMLBrowserManager.SetEnterDsskeyId(-1);
#if IF_FEATURE_XMLB_SCREENSAVER
    if (pLoadParam->strPath == configParser_GetConfigString(kszXmlBrowserScreenSaverURL))
    {
        //标记任务为xml屏保类型
        pLoadParam->bScreenSaverType = true;
    }
#endif

    m_listLoadParam.push_back(pLoadParam);
    return true;
}

// 查找
LoadParam * CXMLBrowserLoadMng::GetLoadParam(const yl::string & strPath)
{
    listLoadParam::iterator it = m_listLoadParam.begin();
    for (; it != m_listLoadParam.end(); ++it)
    {
        //CHECK_NULL(it, NULL);
        LoadParam * pParam = (*it);
        if (NULL == pParam
                || pParam->strPath != strPath)
        {
            continue;
        }
        return pParam;
    }

    return NULL;
}

// 删除
void CXMLBrowserLoadMng::DeleteLoadParam(const yl::string & strPath, bool bDeleteFile/* = true*/)
{
    listLoadParam::iterator it = m_listLoadParam.begin();
    for (; it != m_listLoadParam.end(); ++it)
    {
        LoadParam * pParam = (*it);
        if (NULL == pParam
                || pParam->strPath != strPath)
        {
            continue;
        }

        m_listLoadParam.erase(it);
        FreeParam(pParam, bDeleteFile);
        break;
    }
}

// 释放
bool CXMLBrowserLoadMng::ReleaseLoadParam()
{
    listLoadParam::iterator it = m_listLoadParam.begin();
    for (; it != m_listLoadParam.end(); ++it)
    {
        //CHECK_NULL(it, false);
        LoadParam * pParam = (*it);
        if (NULL == pParam)
        {
            continue;
        }
        FreeParam(pParam);
    }

    m_listLoadParam.clear();

    return true;
}

//
void CXMLBrowserLoadMng::FreeParam(LoadParam * pParam, bool bDeleteFile/* = true*/)
{
    if (pParam == NULL)
    {
        return;
    }
    // 此处释放时只负责删除临时文件，原文件路径由调用者传入，应该由调用者管理
    // strPath 可能是url。可能是Parse的XML文件。可能是需要压缩的图片路径
    /*if (pathFileExist(pParam->strPath.c_str()))
    {
        remove(pParam->strPath.c_str());
    }*/
    XMLBROWSER_INFO("free param remove[%s]", pParam->strTmpPath.c_str());

    if (bDeleteFile && pathFileExist(pParam->strTmpPath.c_str()))
    {
        remove(pParam->strTmpPath.c_str());
    }

    if (dsskey_GetDsskeyStatus(pParam->nDsskeyId) == DS_XMLBROSER_LOADING)
    {
        dsskey_SetDsskeyStatus(pParam->nDsskeyId, DS_XMLBROSER_IDLE);
    }

    delete pParam;
    pParam = NULL;
}

#if IF_BUG_23718
// 上传文件到服务器
bool CXMLBrowserLoadMng::UploadFile(const yl::string & strFilePath, const yl::string & strServerURL)
{
    XMLBROWSER_INFO("CXMLBrowserLoadMng::UploadFile(strFilePath = %s, strServerURL = %s)",
                    strFilePath.c_str(), strServerURL.c_str());
    if (strFilePath.empty() || strServerURL.empty())
    {
        return false;
    }

    // 实例化CNetworkFileAction.
    CUploadFileAction * pAction = TaskAction_CreateUploadFileAction(strFilePath.c_str(),
                                  strServerURL.c_str());
    if (pAction == NULL)
    {
        return false;
    }

    pAction->SetSync(false);  // 设置异步.
    pAction->SetRead(false);  // 设置读.
    pAction->SetCallback(this);  // 设置回调类.
    pAction->SetPut(true);
    pAction->SetDelete(false);

    return TaskAction_ExecAction(pAction);  // 开始请求
}
#endif

#if IF_FEATURE_XMLB_SCREENSAVER
// 处理xml屏保文件下载解析错误
void CXMLBrowserLoadMng::PressScreenSaverError()
{
    if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
    {
        //将屏保模式设置为system模式
        configParser_SetConfigInt(kszScreenType, 0, CONFIG_LEVEL_PHONE);
        etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_SCREEN_SAVER_MODE, 0);
    }
    else
    {
        //黑白屏直接退出屏保
        msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), IDLE_MSG_EXIT_SCREENSAVER, 0, 0);
    }

}
#endif

#endif // #ifdef IF_FEATURE_XMLBROWSER

