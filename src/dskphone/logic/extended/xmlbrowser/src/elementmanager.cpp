#include "logic_inc.h"
#include "xmlbrowserloadmng.h"

#ifdef IF_FEATURE_XMLBROWSER

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_GETINSTANCE(CElementManager)

CElementManager::CElementManager()
{
    m_listUIElement.clear();
    m_bisExitUI = true;
#if IF_FEATURE_XMLB_SCREENSAVER
    m_pSSElement = NULL;
#endif

}

CElementManager::~CElementManager()
{

}

// 创建新的Element并加入链表，如果出错则返回错误提示
CElementBase * CElementManager::CreateElement(XMLParseData * pData)
{
    if (pData == NULL)
    {
        return NULL;
    }

    CElementBase * pElement = CreateElementIns(pData->eType);
    if (pElement == NULL)
    {
        return NULL;
    }

    // 各Element填充数据
    pElement->InitData(pData);

    return pElement;
}

// 创建新的Element
CElementBase * CElementManager::CreateElementIns(XML_ELEMENT_TYPE eType)
{
    switch (eType)
    {
    case XMT_MENU:
        return new CElementTextMenu;
    case XMT_TEXTSCREEN:
        return new CElementTextScreen;
    case XMT_INPUT:
        return new CElementInputScreen;
    case XMT_DIRECTORY:
        return new CElementDirectory;
    case XMT_EXECUTE:
        return new CElementExecute;
    case XMT_STATUS:
        return new CElementStatus;
    case XMT_CONFIGURATION:
        return new CElementConfig;
    case XMT_IMAGEMENU:
    case XMT_IMAGESCREEN:
        return new CElementImageScreen;
    case XMT_FORMATTEDTEXTSCREEN:
        return new CElementFormattedText;
#ifdef IF_FEATURE_GENBAND
    case XMT_INTERFACE:
        return new CElementInterface;
#endif
#if IF_FEATURE_XMLB_SCREENSAVER
    case XMT_SCREENSAVER:
        {
            DeleteSSElement();
            m_pSSElement = new CElementScreenSaver;
            return m_pSSElement;
        }
#endif
    default:
        break;
    }

    return NULL;
}

// 获取对象数据
bool CElementManager::GetToUIData(XMLLogicToUIData & objToUIData)
{
    if (objToUIData.nElementID == INVALID_ELEMENT_ID)
    {
        objToUIData.nElementID = GetTopElementID();
    }

    CElementHaveUIBase * pElement = GetElement(objToUIData.nElementID);
    if (pElement == NULL)
    {
        return false;
    }

    objToUIData.eElementType = pElement->GetType();
    objToUIData.pElementData = pElement->GetData();
    return true;
}

// 事件响应
bool CElementManager::OnAction(int nElementID, XB_ACTION_TYPE eAction, void * pData)
{
    CElementHaveUIBase * pElement = GetElement(nElementID);
    if (pElement == NULL)
    {
        return false;
    }

    bool bExitUI = false;
    bool bUpdate = false;
    if (!pElement->OnAction(eAction, pData, bExitUI, bUpdate))
    {
        return false;
    }

    if (bUpdate)
    {
        _XMLBrowserManager.UpdateWnd();
    }
    return true;
}

// 从链表中删除Element
bool CElementManager::DeleteElement(int nElementID)
{
    CElementHaveUIBase * pElement = GetElement(nElementID);
    if (pElement == NULL)
    {
        return false;
    }

    // 销毁之前的处理
    pElement->BeforeDestroy();
    // 移出链表
    RemoveElement(nElementID);
    // 删除
    delete pElement;
    pElement = NULL;
    // 删除逻辑对象后通知出去
    _XMLBrowserManager.OnDeleteElement(nElementID);


    return true;
}

// 从链表中删除所有Element
void CElementManager::DeleteAllElement()
{
#if IF_FEATURE_XMLB_SCREENSAVER
    int nElementID = GetElementIDNotSS();
    while (nElementID != INVALID_ELEMENT_ID)
    {
        DeleteElement(nElementID);
        nElementID = GetElementIDNotSS();
    }

    //移除Screensaver element,但是数据不删除
    DeleteSSElementInList();
#else
    int nElementID = GetTopElementID();
    while (nElementID != INVALID_ELEMENT_ID)
    {
        DeleteElement(nElementID);
        nElementID = GetTopElementID();
    }
#endif
}

bool CElementManager::LoadElement(const char * lpszPath, int nLoadOpts, bool bExit)
{
    m_bisExitUI = bExit;
    return _XMLBrowserLoadMng.LoadFile(lpszPath, nLoadOpts, XB_FILE_XML, -1);
}

// 对象加载图片
bool CElementManager::LoadImage(int nElementID, const char * lpszPath, int nLoadOpts,
                                void * pExtData /*= NULL*/)
{
    return _XMLBrowserLoadMng.LoadFile(lpszPath, nLoadOpts, XB_FILE_IMAGE, nElementID, pExtData);
}

bool CElementManager::OnLoadFile(int nLoadOpts, int nElementID, void * pTakeoutData)
{
    // 解析
    if (IS_LOAD(nLoadOpts, OPT_XML_PARSE))
    {
        if (nElementID == -1)
        {
            // 新的XML对象，是否需要退出之前的XML界面
            //return false;
        }

#if IF_BUG_32431
        if (!IsNeedCreateElement(pTakeoutData))
        {
            return true;
        }
#endif

        XMLParseData * pParseData = (XMLParseData *)pTakeoutData;
        // 创建新的Element, 此时pData是从Action中取出的，其会由具体的对象保存
        CElementBase * pElement = CreateElement(pParseData);
        if (pElement == NULL)
        {
            XMLBROWSER_WARN("create element object failed .");
            if (pParseData != NULL)
            {
                ReleaseParseData(pParseData);
                pParseData = NULL;
            }
            return false;
        }
        else
        {
#if IF_BUG_32431
            if (pParseData != NULL)
            {
                _XMLBrowserManager.SetAutoRefresh(pParseData->sRefreshData.nTimeOut,
                                                  pParseData->sRefreshData.strURL);
            }
#endif
//#if XML_BROWSER_SUPPORT_UI
            CElementHaveUIBase * pUIElement = dynamic_cast<CElementHaveUIBase *>(pElement);
            if (pUIElement != NULL)
            {
                // 退出界面在解析完再去判断是否要退出(有些非界面执行完不需要退出之前的界面)
                if (m_bisExitUI)
                {
                    _XMLBrowserManager.Exit();
                }
                AddElement(pUIElement);
                pElement->AfterCreate();

                _XMLBrowserManager.UpdateWnd();
            }
            else
//#endif
            {
                // 先执行创建后的操作
                pElement->AfterCreate();
                // 非UI对象直接删除
                delete pElement;
                pElement = NULL;
            }
        }
    }
    // 下载或压缩
    else if (IS_LOAD(nLoadOpts, OPT_COMPRESS)
             || IS_LOAD(nLoadOpts, OPT_DOWNLOAD))
    {
        if (nElementID == -1)
        {
            XMLBROWSER_WARN("Invalid element load task.");
            return false;
        }
        // 找处理的对象
        CElementBase * pElement = GetElement(nElementID);
        if (pElement == NULL)
        {
            XMLBROWSER_WARN("Cant find element !");
            return false;
        }
        bool bExit = false;
        bool bUpdate = false;
        if (IS_LOAD(nLoadOpts, OPT_COMPRESS))
        {
            pElement->OnAction(XB_ACTION_COMPRESS_DONE, pTakeoutData, bExit, bUpdate);
        }
        else
        {
            pElement->OnAction(XB_ACTION_DOWNLOAD_DONE, pTakeoutData, bExit, bUpdate);
        }

        if (bUpdate)
        {
            _XMLBrowserManager.UpdateWnd();
        }
    }
    else
    {
        return false;
    }

    return true;
}


// 加入链表
void CElementManager::AddElement(CElementHaveUIBase * pElement)
{
    if (pElement == NULL)
    {
        return;
    }

    m_cs.Lock();

    XMLBROWSER_INFO("add element [0x%X]", pElement->GetID());
    m_listUIElement.push_back(pElement);

    m_cs.Unlock();
}

// 从链表获取
int CElementManager::GetTopElementID()
{
    int nElementID = INVALID_ELEMENT_ID;

    m_cs.Lock();

    if (m_listUIElement.size() > 0)
    {
        CElementHaveUIBase * pUIElement = m_listUIElement.back();
        if (pUIElement != NULL)
        {
            nElementID = pUIElement->GetID();
        }
    }

    m_cs.Unlock();

    return nElementID;
}

// 从链表获取
CElementHaveUIBase * CElementManager::GetElement(int nElementID)
{
    CElementHaveUIBase * pElement = NULL;

    m_cs.Lock();

    typeListUIElement::iterator iter = m_listUIElement.begin();
    for (; iter != m_listUIElement.end(); ++iter)
    {
        if ((*iter) == NULL
                || (*iter)->GetID() != nElementID)
        {
            continue;
        }
        pElement = (*iter);
    }

    m_cs.Unlock();

    return pElement;
}

// 删除对象
bool CElementManager::RemoveElement(int nElementID)
{
    // 找到要删除的项
    m_cs.Lock();

    typeListUIElement::iterator iter = m_listUIElement.begin();
    for (; iter != m_listUIElement.end(); ++iter)
    {
        if ((*iter) == NULL
                || (*iter)->GetID() != nElementID)
        {
            continue;
        }
        XMLBROWSER_INFO("remove element [0x%X]", (*iter)->GetID());

        m_listUIElement.erase(iter);
        break;
    }

    m_cs.Unlock();

    return true;
}

// 下载语音
bool CElementManager::LoadWav(const char * lpszPath, int nLoadOpts)
{
    return _XMLBrowserLoadMng.LoadFile(lpszPath, nLoadOpts, XB_FILE_WAV, -1);
}

// 判断当前是否有XB界面的Element
BOOL CElementManager::IsHaveXBElement()
{
    return (m_listUIElement.size() != 0);
}

#if IF_FEATURE_XMLB_SCREENSAVER
bool CElementManager::GetSSData(XMLLogicToUIData & objToUIData)
{
    if (m_pSSElement == NULL)
    {
        return false;
    }

    objToUIData.nElementID = m_pSSElement->GetID();
    objToUIData.eElementType = m_pSSElement->GetType();
    objToUIData.pElementData = m_pSSElement->GetData();

    return true;
}
//获取非screensaver类型的elementid
int CElementManager::GetElementIDNotSS()
{
    int nElementID = INVALID_ELEMENT_ID;

    m_cs.Lock();

    typeListUIElement::iterator iter = m_listUIElement.begin();
    for (; iter != m_listUIElement.end(); ++iter)
    {
        if ((*iter)->GetType() == XMT_SCREENSAVER)
        {
            continue;
        }
        nElementID = (*iter)->GetID();
        break;
    }

    m_cs.Unlock();

    return nElementID;
}

// 从链表获取screensaver element
CElementHaveUIBase * CElementManager::GetScreenSaverElement()
{
    CElementHaveUIBase * pElement = NULL;

    m_cs.Lock();

    typeListUIElement::iterator iter = m_listUIElement.begin();
    for (; iter != m_listUIElement.end(); ++iter)
    {
        if ((*iter) == NULL
                || (*iter)->GetType() != XMT_SCREENSAVER)
        {
            continue;
        }
        pElement = (*iter);
    }

    m_cs.Unlock();

    return pElement;

}

//将Screensaver从m_listUIElement中去除
bool CElementManager::DeleteSSElementInList()
{
    CElementHaveUIBase * pElement = GetScreenSaverElement();
    if (pElement == NULL)
    {
        return false;
    }
    // 移出链表
    RemoveElement(pElement->GetID());

    return true;
}

//清除Screensaver缓存数据
void CElementManager::DeleteSSElement()
{
    DeleteSSElementInList();

    if (m_pSSElement != NULL)
    {
        m_pSSElement->BeforeDestroy();
        // 删除
        delete m_pSSElement;
        m_pSSElement = NULL;
    }
}
#endif //#if IF_FEATURE_XMLB_SCREENSAVER

#if IF_BUG_32431
bool CElementManager::IsNeedCreateElement(void * pTakeoutData)
{
    XMLParseData * pParseData = (XMLParseData *)pTakeoutData;
    if (pParseData == NULL)
    {
        return true;
    }

    bool bUpdate = false;
    CElementHaveUIBase * pElement = GetElement(GetTopElementID());
    if (pElement != NULL
            && pElement->UpdateData(pParseData, bUpdate))
    {
        _XMLBrowserManager.SetAutoRefresh(pParseData->sRefreshData.nTimeOut,
                                          pParseData->sRefreshData.strURL);
        if (bUpdate)
        {
            _XMLBrowserManager.UpdateWnd();
        }
        return false;
    }
    return true;
}
#endif

#endif // #ifdef IF_FEATURE_XMLBROWSER

