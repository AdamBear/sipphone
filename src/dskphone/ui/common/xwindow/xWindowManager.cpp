#include "xWindowManager.h"
#include "xValueHandler.h"
#include "commonapi/stringhelperapi.h"

#include "keymap.h"
#include <xwindow/xscreendefine.h>
#if IF_XWINDOW_DUMP_EXPORT
#include "viewdumpmgr.h"
#if IF_XWINDOW_AUTO_TEST
#include "modautotest.h"
#endif
#endif

#include "dsklog/log.h"
#if IF_USE_FREETYPE_LIB
#include "modfreetype.h"
#endif
#if IF_USE_DOB_FONT
#include "gDobFont.h"
#endif

#if IF_MULTI_TOUCH_ENABLE
#include "touchdriver/include/modtouchdriver.h"
#define MAX_TOUCH_EVENT_CACHE_SIZE 5
#endif

#if IF_GESTURE_ENABLE
#include "gesture/include/gesturemanager.h"
#endif

const int verWindowsObjects = 0x1002;


//////////////////////////////////////////////////////////////////////////
// class dataWindowObject
dataWindowObject::dataWindowObject()
    : magic(0)
#if IF_XKERNEL_THREAD_CHECK
    , hostThreadID(0)
#endif
    , hostView(NULL)
    , focusView(NULL)
    , visible(false)
    , overlap(false)
    , activable(false)
    , m_bWaitSetActive(false)
    , topLevel(TML_DEFAULT)// 5:system topmost; 10:user topmost window 20:normal window
    , opacity(255)
{
}

#if IF_XKERNEL_THREAD_CHECK
THREAD_ID dataWindowObject::getHostThreadId()
{
    return hostThreadID;
}
#endif

//////////////////////////////////////////////////////////////////////////
// class ViewOrWindow
bool ViewOrWindow::operator == (const ViewOrWindow& other) const
{
    return m_bWindowObject == other.m_bWindowObject && m_pViewOrWindow == other.m_pViewOrWindow;
}

ViewOrWindow::ViewOrWindow(dataWindowObject* pWindow)
{
    if (pWindow == NULL)
    {
        m_bWindowObject = false;
        m_pViewOrWindow = NULL;
    }
    else
    {
        m_bWindowObject = true;
        m_pViewOrWindow = (xView*)pWindow;
    }
}

#define TEST_VIEW_FOCUSABLE(pView)              (pView->m_bAppear && !pView->m_bDisable && pView->focusable != xView::bool_false)
#define TEST_VIEW_UNDER_POINT(pView, pt)        (pView->m_bAppear && pView->m_rectLayout.PtInRect(pt))

//////////////////////////////////////////////////////////////////////////
// class xWorkStation
#if !IF_MULTI_XKERNEL_ENABLE
xWorkStation* xWorkStation::s_pWorkStation = NULL;
#endif

xWorkStation::xWorkStation(LPCSTR lpszThreadName)
    : xThreadLocal(lpszThreadName, NULL_METHOD)
    , m_pActiveWindowObject(NULL)
    , m_pCacheActiveWindow(NULL)
    , m_pCacheFocusView(NULL)
#if IF_MOVE_FOCUS_BY_KEY
    , m_bAutoSwitchFocus(true)
#endif
    , m_bExposedRegionChanged(false)
    , m_pLayoutingView(NULL)
    , m_bLoopInRelayoutViews(false)
    , m_uRedrawMagic(0)
    , m_pScreen(NULL)
    , m_pFunCreateScreen(NULL)
    , m_pFunReleaseScreen(NULL)
    , m_pFunGetImage(NULL)
    , m_pFunSetupTextTran(NULL)
    , m_pFunReleaseTextTran(NULL)
    , m_pFunLang(NULL)
    , m_pFunTran(NULL)
    , m_pFunIsRTLText(NULL)
    , m_pFunIsRTLWText(NULL)
    , m_pFunIsRTLChar(NULL)
    , m_pFunGetRTLShowText(NULL)
    , m_pFunGetRTLShowWText(NULL)
#if IF_MULTI_TOUCH_ENABLE
    , m_pFunTouchHookProc(NULL)
#endif
#if IF_USE_DOB_FONT && IF_USE_FREETYPE_LIB
    , m_bUseDotLib(false)
#endif
    , m_nKeyLongPressTimer(KEY_LONG_PRESS_TRIGGER_TIME)
    , m_pStationStyle(NULL)
{
}

xWorkStation::~xWorkStation()
{
    CloseStation();
    chASSERT(0 == m_listDeleteViews.size());
}

#if IF_USE_DOB_FONT && IF_USE_FREETYPE_LIB
void xWorkStation::SetUseDotLib(bool bUseDotLib)
{
    m_bUseDotLib = bUseDotLib;
}

bool xWorkStation::IsUseDotLib() const
{
    return m_bUseDotLib;
}
#endif

bool xWorkStation::IsViewInList(typeViewList& listView, xView* pView)
{
    typeViewList::iterator iter = listView.begin();

    for (; iter != listView.end(); ++iter)
    {
        if (pView == (*iter))
        {
            return true;
        }
    }

    return false;
}

void xWorkStation::SetFontPath(const yl::string& strFontPath)
{
#if IF_USE_FREETYPE_LIB
    FreeType_SetFontPath(strFontPath);
#endif
#if IF_USE_DOB_FONT
    dobFont::SetFontPath(strFontPath);
#endif
}

bool xWorkStation::OpenStation(const yl::string& strStackName, int nStackBytes)
{
    m_pHostThread = g_KernelMsgMgr.GetCurrentThreadObject();

#if IF_MULTI_XKERNEL_ENABLE
    getTlsValue<xWorkStation*>(NULL) = this;
#else
    s_pWorkStation = this;
#endif

#if IF_XWINDOW_DUMP_EXPORT
    g_ViewDumpMgr.Init();
#if IF_XWINDOW_AUTO_TEST
    AutoTest_Init();
#endif
#endif

#if IF_USE_FREETYPE_LIB
    FreeType_Init();
#endif

    xWindowUI_Init();

    return true;
}

void xWorkStation::CloseStation()
{
#if IF_XKERNEL_THREAD_CHECK
    chASSERT(etlGetCurrentTaskId() == hostTaskId());
#endif
    closeAllWindow();
    onFlushRedrawRegion(*(msgObject*)NULL);
    onDeleteViews(NULL_MESSAGE);
}

bool xWorkStation::CreateStationScreen()
{
    if (NULL == m_pFunCreateScreen)
    {
        return false;
    }

    if (NULL == m_pScreen)
    {
        m_pScreen = (*m_pFunCreateScreen)();
    }

    return (NULL != m_pScreen);
}

void xWorkStation::SetScreen(xScreen* pScreen)
{
    if (NULL != pScreen)
    {
        m_pScreen = pScreen;
    }
}

bool xWorkStation::InitScreen(const char* pszConfigFile)
{
    if (NULL == pszConfigFile || NULL == m_pScreen)
    {
        return false;
    }

    if (m_pScreen->Init(pszConfigFile))
    {
        setStationLogicSize(m_pScreen->Width(), m_pScreen->Height());

#if IF_MULTI_TOUCH_ENABLE
        MTD_Init(MSG_METHOD(this, xWorkStation::onTouchEvent), getObject().m_sizeLogic);
#endif

#if IF_GESTURE_ENABLE
        g_GestureManager.Init(this);
#endif
        return true;
    }

    return false;
}

bool xWorkStation::UnInitScreen()
{
    if (NULL == m_pScreen || NULL == m_pFunReleaseScreen)
    {
        return true;
    }

    m_pScreen->UnInit();

    bool bSuc = (*m_pFunReleaseScreen)(m_pScreen);

    m_pScreen = NULL;

    return bSuc;
}

void xWorkStation::setStationLogicSize(int xPoints, int yPoints)
{
    m_stProfile.m_sizeLogic.cx = xPoints;
    m_stProfile.m_sizeLogic.cy = yPoints;
}

void xWorkStation::setStationOverlapMode(bool bOverlap)
{
    m_stProfile.m_bOverlapMode = bOverlap;
}

void xWorkStation::setStationBackground(const xBrush& brush)
{
    if (brush.isNull())
    {
        return;
    }

    m_stProfile.m_brushBackground = brush;
}

void xWorkStation::setStationViewBGColor(const xColor& col)
{
    m_stProfile.m_clrViewDefaultBG = col;
}

#if IF_XWINDOW_STATION_ALPHA
int& xWorkStation::getStationAlpha()
{
    return m_stProfile.m_nGlobalAlpha;
}

void xWorkStation::setStationAlpha(int nAlpha)
{
    m_stProfile.m_nGlobalAlpha = nAlpha;
}
#endif

void xWorkStation::setDrawableRootPath(const yl::string& strRootPath)
{
    m_strDrawablePath = strRootPath;
}

const yl::string& xWorkStation::getDrawableRootPath() const
{
    return m_strDrawablePath;
}

void xWorkStation::setLayoutRootPath(const yl::string& strRootPath)
{
    m_strLayoutPath = strRootPath;
}

#if IF_DEBUG_VIEW_MGR
void xWorkStation::dump()
{
    ViewOrWindow vw = getNextViewOrWindow(ViewOrWindow(), true);
    while (vw.hasData())
    {
        xView* pView = vw.getViewObject();
        if (pView != NULL)
        {
            pView->dump();
        }
        vw = getNextViewOrWindow(vw, true);
    }
}
#endif

void xWorkStation::PostPendTillLayout(WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
    PostWindowEvent(STATION_WINDOW_EVENT_PEND_TILL_LAYOUT, wParam, lParam);
}

void xWorkStation::PostPendTillFocus(WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
    PostWindowEvent(STATION_WINDOW_EVENT_PEND_TILL_FOCUS, wParam, lParam);
}

void xWorkStation::PostWindowEvent(int nEventType, WPARAM wParam, LPARAM lParam)
{
    LIST_STATION_WINDOW_EVENT::iterator iter = m_listWindowEvent.begin();

    for (; iter != m_listWindowEvent.end(); ++iter)
    {
        ST_STATION_WINDOW_EVENT& stEvent = *iter;

        if (nEventType == stEvent.m_nEventType)
        {
#if IF_VIEW_ASYNC_CLICK
            if (STATION_WINDOW_EVENT_CLICK == nEventType)
            {
                stEvent.m_wParam = wParam;
                stEvent.m_lParam = lParam;
            }
#endif

#if IF_KEY_REPEAT
            if (STATION_WINDOW_EVENT_REPEAT_KEY_PRESS == nEventType)
            {
                stEvent.m_wParam = wParam;
                stEvent.m_lParam = lParam;
            }
#endif
            return;
        }

        if (nEventType > stEvent.m_nEventType)
        {
            break;
        }
    }

    ST_STATION_WINDOW_EVENT stEvent(nEventType, wParam, lParam);

    if (iter != m_listWindowEvent.end())
    {
        m_listWindowEvent.insert(iter, stEvent);
    }
    else
    {
        m_listWindowEvent.push_back(stEvent);
    }

    postThreadMsgEx(STATION_WINDOW_EVENT, 0, 0,
                    MSG_METHOD(this, xWorkStation::onStationEvent));
}

void xWorkStation::dispatchKeypadEvent(int nKey, bool bPress)
{
#if IF_DEBUG_EVENT
    XWINDOW_INFO("dispatchKeypadEvent(%d, %d)", nKey, (bPress ? 1 : 0));
#endif

    postThreadMsgEx(TASK_FETCH_KEY_EVENT, (WPARAM)nKey, (LPARAM)(bPress ? 1 : 0),
                    MSG_METHOD(this, xWorkStation::onStationEvent));
}

const ST_DATA_PROFILE& xWorkStation::profile()
{
    return m_stProfile;
}

xView* xWorkStation::LoadView(const yl::string& strFileName)
{
    xml_document doc;

    if (!g_WorkStation.LoadLayout(strFileName, doc))
    {
        return NULL;
    }

    xml_node docRoot = doc.document_element();

    return LoadView(docRoot);
}

xView* xWorkStation::LoadView(xml_node& node)
{
    if (node.empty())
    {
        return NULL;
    }

    ExpandNode(node);

    yl::string strViewName = node.name();
    xView* pView = xView::getObjectByName(strViewName);

    chASSERT(NULL != pView);

    pView->loadXmlContent(node);

    return pView;
}

bool xWorkStation::LoadLayout(const yl::string& strFileName, xml_document& doc)
{
    size_t uColonPos = strFileName.find(':');

    if (yl::string::npos == uColonPos)
    {
        yl::string strFullPath = getLayoutFile(strFileName);

        return LoadLayoutDirect(strFullPath, doc);
    }

    yl::string strLayoutFile = strFileName.substr(0, uColonPos);
    yl::string strFullPath = getLayoutFile(strLayoutFile);

#if IF_SUPPORT_MACRO_LAYOUT_FILE
    yl::string strMacros = strFileName.substr(uColonPos + 1);
    LIST_STRING listFile;
    MAP_STRING mapMacros;

    if (commonAPI_splitStringBySeparator(strMacros, ';', listFile))
    {
        LIST_STRING::iterator iter = listFile.begin();

        for (; iter != listFile.end(); ++iter)
        {
            yl::string& strMacro = *iter;
            size_t uSignPos = strMacro.find('=');

            if (yl::string::npos == uSignPos)
            {
                continue;
            }

            yl::string strKey = strMacro.substr(0, uSignPos);
            yl::string strValue = strMacro.substr(uSignPos + 1);

            if (strKey.empty())
            {
                continue;
            }

            mapMacros[strKey] = strValue;
        }
    }

    if (!mapMacros.empty())
    {
        return LoadLayoutWithMacros(strFullPath, mapMacros, doc);
    }
#endif

    return LoadLayoutDirect(strFullPath, doc);
}

xPixmap xWorkStation::LoadPixmap(const yl::string& strFielName)
{
    yl::string strFullPath = getDrawableFile(strFielName);
    return xPixmap(strFullPath);
}

yl::string xWorkStation::getLayoutFile(const yl::string& strFileName)
{
    yl::string result = m_strLayoutPath + strFileName;

    return result;
}

yl::string xWorkStation::getDrawableFile(const yl::string& strFileName)
{
    yl::string result = m_strDrawablePath + strFileName;

    return result;
}

void xWorkStation::SetFunCreateScreen(FUN_CREATE_SCREEN pFunCreateScreen)
{
    m_pFunCreateScreen = pFunCreateScreen;
}

void xWorkStation::SetFunReleaseScreen(FUN_RELEASE_SCREEN pFunReleaseScreen)
{
    m_pFunReleaseScreen = pFunReleaseScreen;
}

void xWorkStation::SetGetImageFunction(FunGetImage pGetImage)
{
    m_pFunGetImage = pGetImage;
}

bool xWorkStation::GetImageByPath(const yl::string& strFile, xPixmap& value)
{
    if (NULL != m_pFunGetImage)
    {
        value = (*m_pFunGetImage)(getDrawableFile(strFile));
    }
    else
    {
        value.load(getDrawableFile(strFile));
    }

    return (!value.isNull());
}

void xWorkStation::SetFunSetupTextTran(FUN_SETUP_TEXT_TRAN pFunSetupTextTran)
{
    m_pFunSetupTextTran = pFunSetupTextTran;
}

void xWorkStation::SetFunReleaseTextTran(FUN_RELEASE_TEXT_TRAN pFunReleaseTextTran)
{
    m_pFunReleaseTextTran = pFunReleaseTextTran;
}

void xWorkStation::SetFunLang(FUN_LANG pFunLang)
{
    m_pFunLang = pFunLang;
}

void xWorkStation::SetFunTran(FUN_TRAN pFunTran)
{
    m_pFunTran = pFunTran;
}

void xWorkStation::SetFunIsRTLText(FUN_IS_RTL_TEXT pFunIsRTLText)
{
    m_pFunIsRTLText = pFunIsRTLText;
}

void xWorkStation::SetFunIsRTLWText(FUN_IS_RTL_WTEXT pFunIsRTLWText)
{
    m_pFunIsRTLWText = pFunIsRTLWText;
}

void xWorkStation::SetFunIsRTLChar(FUN_IS_RTL_CHAR pFunIsRTLChar)
{
    m_pFunIsRTLChar = pFunIsRTLChar;
}

void xWorkStation::SetFunGetRTLShowText(FUN_GET_RTL_SHOW_TEXT pFunGetRTLShowText)
{
    m_pFunGetRTLShowText = pFunGetRTLShowText;
}

void xWorkStation::SetFunGetRTLShowWText(FUN_GET_RTL_SHOW_WTEXT pFunGetRTLShowWText)
{
    m_pFunGetRTLShowWText = pFunGetRTLShowWText;
}

bool xWorkStation::SetupTextTran(xTextTranslate* pTextObject, const yl::string& strText)
{
    if (NULL != m_pFunSetupTextTran)
    {
        return (*m_pFunSetupTextTran)(pTextObject, strText);
    }

    if (NULL != pTextObject && NULL != m_pFunTran)
    {
        pTextObject->SetTranText((*m_pFunTran)(strText));

        return true;
    }

    return false;
}

void xWorkStation::ReleaseTextTran(xTextTranslate* pTextObject)
{
    if (NULL != m_pFunReleaseTextTran)
    {
        (*m_pFunReleaseTextTran)(pTextObject);
    }
}

yl::string xWorkStation::Lang(const yl::string& strText)
{
    if (NULL != m_pFunLang)
    {
        return (*m_pFunLang)(strText);
    }

    return strText;
}

yl::string xWorkStation::Tran(const yl::string& strText)
{
    if (NULL != m_pFunTran)
    {
        return (*m_pFunTran)(strText);
    }

    return strText;
}

bool xWorkStation::IsRTLText(const yl::string& strText)
{
    if (NULL != m_pFunIsRTLText)
    {
        return (*m_pFunIsRTLText)(strText);
    }

    return false;
}

bool xWorkStation::IsRTLWText(const yl::wstring& strText)
{
    if (NULL != m_pFunIsRTLWText)
    {
        return (*m_pFunIsRTLWText)(strText);
    }

    return false;
}

bool xWorkStation::IsRTLChar(WCHAR wChar)
{
    if (NULL != m_pFunIsRTLChar)
    {
        return (*m_pFunIsRTLChar)(wChar);
    }

    return false;
}

bool xWorkStation::GetRTLShowText(const yl::string& strText, yl::string& strShowText)
{
    if (NULL != m_pFunGetRTLShowText)
    {
        return (*m_pFunGetRTLShowText)(strText, strShowText);
    }

    return false;
}

bool xWorkStation::GetRTLShowWText(yl::wstring& strShowText)
{
    if (NULL != m_pFunGetRTLShowWText)
    {
        (*m_pFunGetRTLShowWText)(strShowText);

        return true;
    }

    return false;
}

void xWorkStation::SetKeyLongPressTime(int nTime/* = KEY_LONG_PRESS_TRIGGER_TIME*/)
{
    m_nKeyLongPressTimer = nTime;
}

void xWorkStation::SetLongPressTimeByKey(int nKey, int nTime/* = KEY_LONG_PRESS_TRIGGER_TIME*/)
{
    m_nKeyLongPressTimer = nTime;
    if (m_timerLongPressed.IsTimerRuning())
    {
        m_timerLongPressed.KillTimer();
        m_timerLongPressed.SingleShot(m_nKeyLongPressTimer, (LPVOID)nKey, TIMER_METHOD(this, xWorkStation::OnKeyLongPressed));
    }
}

static void copyChildrenNode(xml_node& node, xml_node& nodeCopy)
{
    xml_node nodeChild = nodeCopy.first_child();

    while (!nodeChild.empty())
    {
        xml_node nodeNewChild = node.append_child(nodeChild.name());
        xml_attribute attr = nodeChild.first_attribute();

        while (!attr.empty())
        {
            nodeNewChild.append_attribute(attr.name(), attr.as_string());

            attr = attr.next_attribute();
        }

        copyChildrenNode(nodeNewChild, nodeChild);

        nodeChild = nodeChild.next_sibling();
    }
}

static void replaceXmlNode(xml_node& node, xml_node& nodeCopy)
{
    if (node.empty() || nodeCopy.empty())
    {
        return;
    }

    xml_node_iterator iterNodeEnd = node.end();

    while (node.begin() != iterNodeEnd)
    {
        xml_node nodeChild = node.first_child();

        node.remove_child(nodeChild);
    }

    xml_attribute_iterator iterAttrEnd = node.attributes_end();

    while (node.attributes_begin() != iterAttrEnd)
    {
        xml_attribute attr = node.first_attribute();

        node.remove_attribute(attr);
    }

    node.set_name(nodeCopy.name());

    xml_attribute attr = nodeCopy.first_attribute();

    while (!attr.empty())
    {
        node.append_attribute(attr.name(), attr.as_string());

        attr = attr.next_attribute();
    }

    copyChildrenNode(node, nodeCopy);
}

bool xWorkStation::LoadLayoutDirect(const yl::string& strFullPath, xml_document& doc)
{
    xml_parse_result objResult = doc.load_file(strFullPath.c_str());

    if (status_ok != objResult.status)
    {
        XWINDOW_ERR("LoadLayoutDirect load xml file[%s] failed.", strFullPath.c_str());
        return false;
    }

    xml_node nodeRoot = doc.document_element();

    if (nodeRoot.empty())
    {
        return false;
    }

    return ExpandNode(nodeRoot);
}

#if IF_SUPPORT_MACRO_LAYOUT_FILE
bool xWorkStation::LoadLayoutWithMacros(const yl::string& strFullPath, MAP_STRING& mapMacro, xml_document& doc)
{
    chStringA textFileContent = etlDumpTextFileA(strFullPath.c_str());
    chSplitResultA result(textFileContent);
    int nMatchCount = 0;
    static chAssicTable tabMacroEnding = ~chAssicTable("09azAZ__::", 5);
    yl::string strTextResult;

    while (1)
    {
        result = result.second.splitBy('$');
        if (result.iResult != -1)
        {
            chSplitResultA resultQuota = result.second.splitByOneOf(tabMacroEnding);
            chASSERT(resultQuota.iResult != -1);
            yl::string strKey = (const char*)resultQuota.first;
            MAP_STRING::iterator iter = mapMacro.find(strKey);

            if (iter != mapMacro.end())
            {
                yl::string& strValue = iter->second;

                strTextResult = strTextResult + (const char*)result.first + strValue.c_str();
                strTextResult += (const char*)resultQuota.first.ending()[0];
            }
            else
            {
                XWINDOW_ERR("Unknow MACRO:%s in %s", chStringA(resultQuota.first).c_str(), strFullPath.c_str());
                strTextResult += (const char*)result.first;
                strTextResult += (const char*)resultQuota.first.ending()[0];
            }
            result.second = resultQuota.second;
            nMatchCount ++;
        }
        else
        {
            if (nMatchCount != 0)
            {
                strTextResult += (const char*)result.first;
            }
            else
            {
                // NOTHING matched
                strTextResult += (const char*)textFileContent;
            }
            break;
        }
    }

    xml_parse_result objResult = doc.load(strTextResult.c_str());

    return (status_ok == objResult.status);
}
#endif

bool xWorkStation::ExpandNode(xml_node& node)
{
    if (node.empty())
    {
        return false;
    }

    ExpandEmbedNode(node);

    xml_node nodeChild = node.first_child();

    while (!nodeChild.empty())
    {
        ExpandNode(nodeChild);

        nodeChild = nodeChild.next_sibling();
    }

    return true;
}

bool xWorkStation::ExpandEmbedNode(xml_node& node)
{
    if (node.empty())
    {
        return false;
    }

    yl::string strTagName = node.name();

    if (XWINDOW_TAG_NAME_VIEW_STUB == strTagName)
    {
        yl::string layout;

        PopupAndroidString(node, XWINDOW_NAME_LAYOUT, layout);

        // replace $XXX by attribute
        chASSERT(yl::string::npos == layout.find(':')); // DO NOT use file:MACRO=X for ViewStub

        yl::string strFullPathName = getLayoutFile(layout);
        xml_document doc;

#if IF_SUPPORT_MACRO_LAYOUT_FILE
        if (node.attributes_begin() != node.attributes_end())
        {
            MAP_STRING mapString;
            xml_attribute attr = node.first_attribute();

            while (!attr.empty())
            {
                yl::string strName = attr.name();
                yl::string strValue = attr.value();

                if (!strName.empty())
                {
                    mapString[strName] = strValue;
                }

                attr = attr.next_attribute();
            }

            if (!LoadLayoutWithMacros(strFullPathName, mapString, doc))
            {
                return false;
            }
        }
        else
        {
#endif
            if (!LoadLayoutDirect(strFullPathName, doc))
            {
                return false;
            }
#if IF_SUPPORT_MACRO_LAYOUT_FILE
        }
#endif

        xml_node nodeRoot = doc.document_element();

        strTagName = nodeRoot.name();

        chASSERT(strTagName != XWINDOW_TAG_NAME_VIEW_STUB && yl::string::npos == strTagName.find('.'));

        replaceXmlNode(node, nodeRoot);

        return true;
    }

    if (yl::string::npos != strTagName.find('.'))
    {
        yl::string strLayoutFile = strTagName.c_str();
        yl::string strFullPathName = getLayoutFile(strLayoutFile);

        xml_document doc;

        if (!LoadLayoutDirect(strFullPathName, doc))
        {
            return false;
        }

        xml_node nodeRoot = doc.document_element();

        strTagName = nodeRoot.name();

        chASSERT(strTagName != XWINDOW_TAG_NAME_VIEW_STUB && yl::string::npos == strTagName.find('.'));

        replaceXmlNode(node, nodeRoot);

        return true;
    }

    return false;
}

BOOL xWorkStation::onProcessFetchKeyEvent(msgObject& msg)
{
    int nKey = (int)msg.wParam;
    bool bPress = false;

    if (1 == msg.lParam)
    {
        bPress = true;
    }

#if IF_DEBUG_EVENT
    XWINDOW_INFO("onProcessFetchKeyEvent(0x%x, %d)", nKey, msg.lParam);
#endif

    if (-1 != nKey)
    {
        onTaskKeyEvent(nKey, bPress);
    }

    return TRUE;
}

bool xWorkStation::dispatchDirectionalViewEvent(xView* pTargetView, xViewEvent* pEvent)
{
    return xViewHandler::dispatchViewEventOnHandler(pTargetView, pEvent)
           || (pTargetView->m_pStyle != NULL && pTargetView->m_pStyle->onViewEvent(pTargetView, pEvent))
           || (pTargetView->m_pHostStation->m_pStationStyle != NULL && pTargetView->m_pHostStation->m_pStationStyle->onViewEvent(pTargetView, pEvent))
           || pTargetView->onEvent(pEvent);
}

bool xWorkStation::dispatchNonDirectionalViewKeyEvent(xView* pTargetView, xViewEvent* pEvent)
{
    xView* pParent = pTargetView->m_pParent;
    return (pParent != NULL && dispatchNonDirectionalViewKeyEvent(pParent, pEvent))
           || dispatchDirectionalViewEvent(pTargetView, pEvent);
}

void xWorkStation::onTaskKeyEvent(int key, bool bPressed)
{
    if (bPressed)
    {
        m_timerLongPressed.SingleShot(m_nKeyLongPressTimer, (LPVOID)key,
                                      TIMER_METHOD(this, xWorkStation::OnKeyLongPressed));
#if IF_KEY_REPEAT
        m_timerKeyRepeat.SingleShot(KEY_REPEAT_TRIGGER_TIME, (LPVOID)key,
                                    MSG_METHOD(this, xWorkStation::OnKeyRepeat));

        ProcessKeyPress(key, false);
#else
        ProcessKeyPress(key);
#endif
    }
    else
    {
        m_timerLongPressed.KillTimer();
#if IF_KEY_REPEAT
        m_timerKeyRepeat.KillTimer();
#endif
        xView* pFocusView = getFocusView();
        if (pFocusView != NULL)
        {
            xViewEvent_key ev(VIEW_KEY_RELEASE, key, bPressed);
            dispatchNonDirectionalViewKeyEvent(pFocusView, &ev);
        }
        signalKeyEvent.emitSignal(key, 0, 0, NULL);
    }
}

#if IF_KEY_REPEAT
void xWorkStation::ProcessKeyPress(int nKey, bool bRepeat)
#else
void xWorkStation::ProcessKeyPress(int nKey)
#endif
{
    xView* pFocusView = getFocusView();

    if (NULL == pFocusView)
    {
        return;
    }

    xViewEvent_key ev(VIEW_KEY_PRESS, nKey, true);

    if (!dispatchNonDirectionalViewKeyEvent(pFocusView, &ev))
    {
#if IF_KEY_REPEAT
        if (PHONE_KEY_OK == nKey && (!bRepeat))
#else
        if (PHONE_KEY_OK == nKey)
#endif
        {
            if (pFocusView->clickable != xView::bool_false)
            {
                pFocusView->onClick();
                pFocusView->signalClicked.emitSignal(0, 0, 0, this);
            }
        }
#if IF_MOVE_FOCUS_BY_KEY
        else
        {
            MoveFocusByKey(nKey);
        }
#endif

        signalKeyEvent.emitSignal(nKey, 1, 0, this);
    }
}

void xWorkStation::OnKeyLongPressed(xThreadTimer* timer, LPVOID pExtraData)
{
    xView* pFocusView = getFocusView();
    int nKey = (int)pExtraData;

    if (pFocusView != NULL)
    {
        xViewEvent_key ev(VIEW_KEY_LONG_PRESS, nKey, true);
        dispatchNonDirectionalViewKeyEvent(pFocusView, &ev);
    }
    signalLongKeyEvent.emitSignal(nKey, 1, 0, this);
}

#if IF_KEY_REPEAT
void xWorkStation::OnKeyRepeat(xThreadTimer* timer, LPVOID pExtraData)
{
    m_timerKeyRepeat.SetTimer(KEY_REPEAT_TIME, (LPVOID)nKey,
                              MSG_METHOD(this, xWorkStation::OnKeyRepeat));

    int nKey = SET_KEY_REPEAT((int)msg.m_pMKExtraData);

    PostWindowEvent(STATION_WINDOW_EVENT_REPEAT_KEY_PRESS, (WPARAM)nKey, 0);
}
#endif

BOOL xWorkStation::onSyncActiveWindow(msgObject& msg)
{
    // sync active window
    xWindow* pOldActive = m_pCacheActiveWindow;
    xWindow* pNewActive = getActiveWindow();
    m_pCacheActiveWindow = pNewActive;

#if IF_DEBUG_EVENT
    XWINDOW_INFO("onProcessActiveEvent()");
#endif

    if (pOldActive == pNewActive)
    {
        return TRUE;
    }

    if (NULL != pOldActive)
    {
#if IF_DEBUG_EVENT
        XWINDOW_INFO("onWindowActiveEvent(%s, false)", pOldActive->viewNick().c_str());
#endif
        onWindowActive(pOldActive, false);
    }

    if (NULL != pNewActive)
    {
#if IF_DEBUG_EVENT
        XWINDOW_INFO("onWindowActiveEvent(%s, true)", pNewActive->viewNick().c_str());
#endif
        bringWindowToTop_helper(pNewActive);
        onWindowActive(pNewActive, true);
    }

#if IF_DEBUG_VIEW_FOCUS
    XWINDOW_INFO("onProcessActiveEvent OldActive:%s NewActive:%s",
                 (NULL != pOldActive ? pOldActive->viewNick().c_str() : ""),
                 (NULL != pNewActive ? pNewActive->viewNick().c_str() : ""));
#endif

    PostWindowEvent(STATION_WINDOW_EVENT_FOCUS, 0, 0);

    return TRUE;
}

void xWorkStation::setActiveWindowObject(dataWindowObject* pWindowObject)
{
#if IF_DEBUG_VIEW_MGR
    xView* pHostView = NULL;

    if (NULL != pWindowObject)
    {
        pHostView = pWindowObject->hostView;
    }

    XWINDOW_INFO("setActiveWindowObject() %s", (NULL != pHostView ? pHostView->viewNick().c_str() : ""));
#endif

    chASSERT(NULL == pWindowObject || (pWindowObject->activable && pWindowObject->visible));

    dataWindowObject* pOldActiveWindow = m_pActiveWindowObject;
    dataWindowObject* pNewActiveWindow = pWindowObject;
    bool bActiveChanged = false;

    m_pActiveWindowObject = pWindowObject;

    if (NULL != pNewActiveWindow)
    {
        pNewActiveWindow->m_bWaitSetActive = false;
    }

    if (pOldActiveWindow != pNewActiveWindow)
    {
        bActiveChanged = true;
    }

    if (bActiveChanged)
    {
        PostWindowEvent(STATION_WINDOW_EVENT_ACTIVE, 0, 0);
    }
}

dataWindowObject* xWorkStation::getActiveWindowObject()
{
    return m_pActiveWindowObject;
}

dataWindowObject* xWorkStation::getNextActivableWindow(dataWindowObject* pWindow)
{
    if (NULL == pWindow)
    {
        return NULL;
    }

    LIST_WINDOW_OBJECT::iterator iter = m_lsWindowObject.begin();
    LIST_WINDOW_OBJECT::iterator iterEnd = m_lsWindowObject.end();
    dataWindowObject* pTmpWindowObject = NULL;
    dataWindowObject* pActivableWindow = NULL;

    for (; iter != iterEnd; ++iter)
    {
        pTmpWindowObject = *iter;

        if (pWindow == pTmpWindowObject)
        {
            return pActivableWindow;
        }

        if (NULL != pTmpWindowObject && pTmpWindowObject->visible && pTmpWindowObject->activable
                && (!pTmpWindowObject->m_bWaitSetActive))
        {
            pActivableWindow = pTmpWindowObject;
        }
    }

    return NULL;
}

dataWindowObject* xWorkStation::getNextWindowObject(dataWindowObject* pWindow)
{
    if (NULL == pWindow)
    {
        return NULL;
    }

    LIST_WINDOW_OBJECT::iterator iter = m_lsWindowObject.begin();
    LIST_WINDOW_OBJECT::iterator iterEnd = m_lsWindowObject.end();
    dataWindowObject* pTmpWindowObject = NULL;

    for (; iter != iterEnd; ++iter)
    {
        pTmpWindowObject = *iter;

        if (pTmpWindowObject == pWindow)
        {
            ++iter;

            if (iter != iterEnd)
            {
                return *iter;
            }

            break;
        }
    }

    return NULL;
}

dataWindowObject* xWorkStation::getPreWindowObject(dataWindowObject* pWindow, bool bOnlyVisible)
{
    if (NULL == pWindow)
    {
        return NULL;
    }

    LIST_WINDOW_OBJECT::iterator iter = m_lsWindowObject.begin();
    LIST_WINDOW_OBJECT::iterator iterEnd = m_lsWindowObject.end();
    dataWindowObject* pPreWindowObject = NULL;
    dataWindowObject* pTmpWindowObject = NULL;

    for (; iter != iterEnd; ++iter)
    {
        pTmpWindowObject = *iter;

        if (pTmpWindowObject == pWindow)
        {
            return pPreWindowObject;
        }

        if (!bOnlyVisible || (NULL != pTmpWindowObject && pTmpWindowObject->visible))
        {
            pPreWindowObject = pTmpWindowObject;
        }
    }

    return NULL;
}

dataWindowObject* xWorkStation::getTopWindowByLevel(int nTopLevel)
{
    LIST_WINDOW_OBJECT::iterator iter = m_lsWindowObject.begin();
    LIST_WINDOW_OBJECT::iterator iterEnd = m_lsWindowObject.end();

    for (; iter != iterEnd; ++iter)
    {
        dataWindowObject* pWindowObject = *iter;

        if (NULL != pWindowObject && nTopLevel == pWindowObject->topLevel)
        {
            return pWindowObject;
        }
    }

    return NULL;
}

BOOL xWorkStation::onSyncFocusView(msgObject& msg)
{
    // sync active window
    xView* pOldFocus = m_pCacheFocusView;
    xView* pNewFocus = getFocusView();
    m_pCacheFocusView = pNewFocus;

    if (pOldFocus != pNewFocus)
    {
#if IF_DEBUG_EVENT
        XWINDOW_INFO("onSyncFocusView()");
#endif
        xView* pFocusMoveView = NULL;

        xViewEvent evLeave(VIEW_FOCUS_LEAVE);
        for (xView* pView = pOldFocus; pView != NULL; pView = pView->m_pParent)
        {
            if (!containChildView(pView, pNewFocus))
            {
                dispatchDirectionalViewEvent(pView, &evLeave);
            }
            else
            {
                pFocusMoveView = pView;
                break;
            }
        }

        xViewEvent evEnter(VIEW_FOCUS_ENTER);
        for (xView* pView = pNewFocus; pView != pFocusMoveView; pView = pView->m_pParent)
        {
            dispatchDirectionalViewEvent(pView, &evEnter);
        }

        xViewEvent_FocusMove evMove(VIEW_FOCUS_MOVE, pNewFocus, pOldFocus);
        for (xView* pView = pFocusMoveView; pView != NULL; pView = pView->m_pParent)
        {
            dispatchDirectionalViewEvent(pView, &evMove);
        }

#if IF_DEBUG_VIEW_FOCUS
        XWINDOW_INFO("onSyncFocusView OldFocus:%s NewFocus:%s",
                     (NULL != pOldFocus ? pOldFocus->viewNick().c_str() : ""),
                     (NULL != pNewFocus ? pNewFocus->viewNick().c_str() : ""));
#endif

        if (pOldFocus != NULL)
        {
#if IF_DEBUG_EVENT
            XWINDOW_INFO("onViewFocusEvent(%s, false)", pOldFocus->viewNick().c_str());
#endif
            onViewFocusEvent(pOldFocus, false);
        }
        if (pNewFocus != NULL)
        {
#if IF_DEBUG_EVENT
            XWINDOW_INFO("onViewFocusEvent(%s, true)", pNewFocus->viewNick().c_str());
#endif
            onViewFocusEvent(pNewFocus, true);
        }
    }

    return TRUE;
}

void xWorkStation::onViewFocusEvent(xView* pView, bool bFocusIn)
{
    if (NULL != pView && !pView->IsForceRelayout())
    {
        UpdateFocusWidget(pView);
    }

    signalFocusEvent.emitSignal(1, 0, 0, pView);
}

void xWorkStation::UpdateFocusWidget(xView* pView)
{
    xView* pPresentView = getFocusPresentView(pView);

    if (NULL == pPresentView)
    {
        pPresentView = pView;
    }

    if (NULL != pPresentView)
    {
        pPresentView->update(true);
    }
}

void xWorkStation::onWindowActive(xWindow* pWindow, bool bActive)
{
    xViewEvent_bool ev(VIEW_ACTIVE, bActive);
    dispatchDirectionalViewEvent(pWindow, &ev);

    signalActiveEvent.emitSignal(bActive, 0, 0, NULL);

    // stop long press event and repeat event
    m_timerLongPressed.KillTimer();
#if IF_KEY_REPEAT
    m_timerKeyRepeat.KillTimer();
#endif
}

xView* xWorkStation::getStrongFocusChild(xView* pView, bool bForward/* = true*/)
{
    if (bForward)
    {
        for (xViewIterator it = pView->child_head(); it.hasData(); ++it)
        {
            xView* pChildView = it.operator ->();

            if (NULL != pChildView && pChildView->IsEnable() && pChildView->isVisible())
            {
                xView* pFocus = getStrongFocusChild(pChildView, bForward);

                if (NULL != pFocus)
                {
                    return pFocus;
                }
            }
        }
    }
    else
    {
        for (xViewIterator it = pView->child_tail() ; it.hasData(); --it)
        {
            xView* pChildView = it.operator ->();

            if (NULL != pChildView && pChildView->IsEnable() && pChildView->isVisible())
            {
                xView* pFocus = getStrongFocusChild(pChildView, bForward);

                if (NULL != pFocus)
                {
                    return pFocus;
                }
            }
        }
    }

    if (xView::bool_true == pView->focusable)
    {
        return pView;
    }

    return NULL;
}

xView* xWorkStation::getFocusableChildView(xView* pView)
{
    // chASSERT(TEST_VIEW_FOCUSABLE(pView));
    xView* pChildStrongFocus = getStrongFocusChild(pView, true);
    return pChildStrongFocus == NULL ? pView : pChildStrongFocus;
}

xView* xWorkStation::getFocusPresentView(xView* pView)
{
    if (NULL == pView || NULL == pView->m_pParent)
    {
        return NULL;
    }

    xView* pPresentView = NULL;

    while (NULL != pView->m_pParent)
    {
        pView = pView->m_pParent;

        if (!pView->m_bFocusPresent)
        {
            break;
        }

        pPresentView = pView;
    }

    return pPresentView;
}

bool xWorkStation::checkViewFocusble(const xView* pView)
{
    if (NULL == pView)
    {
        return false;
    }

    return TEST_VIEW_FOCUSABLE(pView);
}

xWindow* xWorkStation::getActiveWindow()
{
    xWindow* pActiveWindow = NULL;

    if (NULL != m_pActiveWindowObject)
    {
#if IF_XKERNEL_THREAD_CHECK
        chASSERT(pActiveWindowObject->getHostTaskId() == etlGetCurrentTaskId());
#endif
        pActiveWindow = m_pActiveWindowObject->hostView;
    }

    return pActiveWindow;
}

xView* xWorkStation::getFocusView()
{
    xView* pFocusView = NULL;

    if (NULL != m_pActiveWindowObject)
    {
#if IF_XKERNEL_THREAD_CHECK
        chASSERT(m_pActiveWindowObject->getHostTaskId() == etlGetCurrentTaskId());
#endif
        pFocusView = m_pActiveWindowObject->focusView;
    }

    return pFocusView;
}

void xWorkStation::setFocus_helper(xView* pView)
{
    chASSERT(checkViewFocusble(pView));

#if IF_DEBUG_VIEW_FOCUS
    XWINDOW_INFO("setFocus_helper %s", (NULL != pView ? pView->viewNick().c_str() : ""));
#endif

    xView* pFocusView = getFocusView();
    xView* pRootView = pView->getRootView();
    chASSERT(pRootView->m_pWindow != NULL);// root view must has window
    pRootView->m_pWindow->focusView = pView;
    if (pRootView == getActiveWindow())
    {
        if (NULL != pFocusView && pFocusView != m_pCacheFocusView)
        {
            xViewEvent evLeave(VIEW_FOCUS_LEAVE);

            dispatchDirectionalViewEvent(pFocusView, &evLeave);
        }

        PostWindowEvent(STATION_WINDOW_EVENT_FOCUS, 0, 0);
    }
}

#if IF_MOVE_FOCUS_BY_KEY
bool xWorkStation::MoveWindowFocus(xWindow* pWindow, xView* pFocusView, int key)
{
    if (NULL == pWindow || NULL == pFocusView)
    {
        return false;
    }

    int nFocusReason = xView::FOCUS_REASON_NONE;
    bool bPrevious = false;

    switch (key)
    {
    case PHONE_KEY_UP:
        nFocusReason = xView::FOCUS_REASON_UP;
        bPrevious = true;
        break;
    case PHONE_KEY_DOWN:
        nFocusReason = xView::FOCUS_REASON_DOWN;
        break;
    case PHONE_KEY_LEFT:
        nFocusReason = xView::FOCUS_REASON_LEFT;
        bPrevious = true;
        break;
    case PHONE_KEY_RIGHT:
        nFocusReason = xView::FOCUS_REASON_RIGHT;
        break;
#if IF_PC_ENABLE
    case PC_KEY_TAB:
        nFocusReason = xView::FOCUS_REASON_NEXT;
        break;
    case PC_KEY_SHIFT_TAB:
        nFocusReason = xView::FOCUS_REASON_PREVIOUS;
        bPrevious = true;
        break;
#endif
    default:
        return false;
        break;
    }

    if (SwitchWindowFocus(pWindow, pFocusView, nFocusReason))
    {
        return true;
    }

    xViewEvent_int eventCustom(VIEW_FOCUS_CUSTOM, nFocusReason);

    if (dispatchNonDirectionalViewKeyEvent(pFocusView, &eventCustom))
    {
        return true;
    }

    if (!m_bAutoSwitchFocus)
    {
        return false;
    }

    if (NULL != pFocusView && pWindow != pFocusView)
    {
        if (bPrevious)
        {
            return focusPrevView(pFocusView, nFocusReason);
        }
        else
        {
            return focusNextView(pFocusView, nFocusReason);
        }
    }

    return false;
}

bool xWorkStation::SwitchWindowFocus(xWindow* pWindow, xView* pFocusView, int nFocusReason)
{
    if (NULL == pWindow || NULL == pFocusView)
    {
        return false;
    }

    yl::string strPropertyName;

    switch (nFocusReason)
    {
    case xView::FOCUS_REASON_LEFT:
        strPropertyName = XWINDOW_NAME_NEXT_FOCUS_LEFT;
        break;
    case xView::FOCUS_REASON_RIGHT:
        strPropertyName = XWINDOW_NAME_NEXT_FOCUS_RIGHT;
        break;
    case xView::FOCUS_REASON_UP:
        strPropertyName = XWINDOW_NAME_NEXT_FOCUS_UP;
        break;
    case xView::FOCUS_REASON_DOWN:
        strPropertyName = XWINDOW_NAME_NEXT_FOCUS_DOWN;
        break;
#if IF_PC_ENABLE
    case xView::FOCUS_REASON_PREVIOUS:
        strPropertyName = XWINDOW_NAME_NEXT_FOCUS_PREVIOUS;
        break;
    case xView::FOCUS_REASON_NEXT:
        strPropertyName = XWINDOW_NAME_NEXT_FOUCS_NEXT;
        break;
#endif
    default:
        return false;
        break;
    }

    yl::string strNextViewName = pFocusView->getStringProperty(strPropertyName, "");
    xView* pView = pFocusView;

    while (!strNextViewName.empty())
    {
        xView* pParentView = pView->parent();

        if (NULL != pParentView)
        {
            pView = pParentView->getViewById(strNextViewName);
        }
        else
        {
            pView = NULL;
        }

        if (NULL == pView)
        {
            pView = pWindow->getViewById(strNextViewName);
        }

        chASSERTx(pView != NULL, "pView != NULL, strNextViewName is %s.", strNextViewName.c_str());

        if (checkViewFocusble(pView))
        {
            pView->setFocus(nFocusReason);
            return true;
        }

        strNextViewName = pView->getStringProperty(strPropertyName, "");
    }

    return false;
}

void xWorkStation::SetAutoSwitchFocus(bool bAutoSwitch)
{
    m_bAutoSwitchFocus = bAutoSwitch;
}

void xWorkStation::MoveFocusByKey(int key)
{
    xWindow* pActiveWindow = getActiveWindow();
    xView* pView = NULL;

    if (NULL != pActiveWindow && NULL != pActiveWindow->m_pWindow)
    {
        pView = pActiveWindow->m_pWindow->focusView;
    }

    MoveWindowFocus(pActiveWindow, pView, key);
}

#endif

void xWorkStation::releaseFocusOnViewDisappear(xView* pView)
{
#if IF_DEBUG_VIEW_FOCUS
    XWINDOW_INFO("releaseFocusOnViewDisappear %s", (NULL != pView ? pView->viewNick().c_str() : ""));
#endif

    chASSERT(pView != NULL);
    if (pView->m_pParent == NULL)
    {
        // root view
        dataWindowObject* pWindowObject = pView->m_pWindow;

        if (pView->m_bAppear && NULL != pWindowObject)
        {
            pWindowObject->visible = false;

            if (pWindowObject == m_pActiveWindowObject)
            {
                // process active & focus
                dataWindowObject * pNextWindow = getNextActivableWindow(pWindowObject);

                setActiveWindowObject(pNextWindow);
            }
        }
    }
    else
    {
        // child view
        if (pView->releaseFocusInWindow())
        {
            xView* pRootView = pView->getRootView();
            chASSERT(pRootView != NULL);

            if (pRootView->m_pWindow == m_pActiveWindowObject)
            {
                PostWindowEvent(STATION_WINDOW_EVENT_FOCUS, 0, 0);
            }
        }
    }
}

bool xWorkStation::focusNextView(xView* pView, int nFocusReason)
{
    chASSERT(pView->m_pParent != NULL);
    xView* pNextView = pView->m_pNext;
    while (pNextView != pView)
    {
        if (pNextView == NULL)
        {
            pNextView = pView->m_pParent->m_pChild;
            continue;
        }
        if (!TEST_VIEW_FOCUSABLE(pNextView))
        {
            pNextView = pNextView->m_pNext;
            continue;
        }
        pNextView->setFocus(nFocusReason);
        return true;
    }
    return false;
}

bool xWorkStation::focusPrevView(xView* pView, int nFocusReason)
{
    chASSERT(pView->m_pParent != NULL);
    xView* pPrevView = pView->m_pPrev;
    while (pPrevView != pView)
    {
        if (pPrevView == NULL)
        {
            pPrevView = pView->m_pParent->m_pLastChild;
            continue;
        }
        if (!TEST_VIEW_FOCUSABLE(pPrevView))
        {
            pPrevView = pPrevView->m_pPrev;
            continue;
        }
        pPrevView->setFocus(nFocusReason);
        return true;
    }
    return false;
}

void xWorkStation::recursiveViewClosed(xView* pView)
{
    chASSERT(pView->m_bClosed);
    xView* pChild = pView->m_pChild;
    while (pChild != NULL)
    {
        pChild->m_bClosed = true;
        recursiveViewClosed(pChild);
        pChild = pChild->m_pNext;
    }

    onViewClosed(pView);
}

void xWorkStation::recursiveViewAppear(xView* pView, bool bAppear)
{
    chASSERT(!(pView->visibility != xView::visible && pView->m_bAppear));
    pView->m_bAppear = bAppear;

    xView* pChild = pView->m_pChild;
    while (pChild != NULL)
    {
        if (bAppear != pChild->m_bAppear && pChild->visibility == xView::visible)
        {
            recursiveViewAppear(pChild, bAppear);
        }
        pChild = pChild->m_pNext;
    }

    onViewAppear(pView, bAppear);
}
/*
disable m_bDisable bDisable
    0       0         0       NothingToDo
    1       0         0       Assert
    0       1         0       m_bDisable = 0
    1       1         0       NothingToDo
    0       0         1       m_bDisable = 1
    1       0         1       Assert
    0       1         1       NothingToDo
    1       1         1       NothingToDo
*/
void xWorkStation::recursiveViewDisable(xView* pView, bool bDisable)
{
    pView->m_bDisable = bDisable;

    chASSERT(!(pView->disable && !pView->m_bDisable));
    xView* pChild = pView->m_pChild;
    while (pChild != NULL)
    {
        if (pChild->m_bDisable != bDisable && !pChild->disable) // child m_bDisable changed
        {
            recursiveViewDisable(pChild, bDisable);
        }
        pChild = pChild->m_pNext;
    }
}

void xWorkStation::onViewAppear(xView* pView, bool bAppear)
{
#if IF_XKERNEL_THREAD_CHECK
    chASSERT(chThreadLocal::IsCalledByThisThread());
#endif

    if (NULL == pView)
    {
        return;
    }

    pView->signalAppear.emitSignal((bAppear ? 1 : 0), 0, 0, 0);

    if (!bAppear)
    {
        if (pView->IsInRedrawList())
        {
            deleteRedrawRegion(pView);
        }
    }
}

void xWorkStation::deleteRedrawRegion(xView* pView)
{
    if (NULL == pView)
    {
        return;
    }

    typeRedrawRegionList::iterator it = m_listRedrawRegions.begin();
    ViewOrWindow vw(pView);

    while (it != m_listRedrawRegions.end())
    {
        ST_REDRAW_REGION_PTR pRedrawRegion = *it;

        if (!pRedrawRegion.IsEmpty() && vw == pRedrawRegion->vw)
        {
            it = m_listRedrawRegions.erase(it);
        }
        else
        {
            ++it;
        }
    }

    pView->ResetInRedrawList();
}

void xWorkStation::onViewClosed(xView* pView)
{
    if (pView->IsInRelayoutList())
    {
        m_listRelayoutViews.remove(pView);
        pView->ResetInRelayoutList();
    }

    if (pView == m_pCacheActiveWindow)
    {
        m_pCacheActiveWindow = NULL;
    }

    if (pView == m_pCacheFocusView)
    {
        m_pCacheFocusView = NULL;
    }

#if IF_MULTI_TOUCH_ENABLE
    ResetTouchCacheView(pView);
#endif

    pView->signalClosed.emitSignal(0, 0, 0, 0);
    signalViewClosed.emitSignal((WPARAM)pView, 0, 0, 0);
}

void xWorkStation::LayoutViewInBoundRect(xView* pView, const chRect& rectBound)
{
    chASSERT(m_pLayoutingView == NULL && m_objRegionOfLayoutting.empty());
    m_pLayoutingView = pView;
    m_objRegionOfLayoutting.clear();
    {
        pView->LayoutInBoundRect(rectBound);
        updateRegionOverView(pView, m_objRegionOfLayoutting);
    }
    m_pLayoutingView = NULL;
    m_objRegionOfLayoutting.clear();
}

void xWorkStation::RelayoutViewGroupContent(xViewGroup* pViewGroup)
{
    chASSERT(m_pLayoutingView == NULL && m_objRegionOfLayoutting.empty());
#if IF_DEBUG_VIEW_MGR
    XWINDOW_INFO("RelayoutViewGroupContent ViewGroup:%s", pViewGroup->viewNick().c_str());
#endif
    m_pLayoutingView = pViewGroup;
    m_objRegionOfLayoutting.clear();
    {
        chRect contentFrame = DeflateRect(pViewGroup->m_rectLayout,
                                          pViewGroup->paddingLeft,
                                          pViewGroup->paddingTop,
                                          pViewGroup->paddingRight,
                                          pViewGroup->paddingBottom);

        if (pViewGroup->IsRedrawRelayoutContent())
        {
            pViewGroup->ResetRedrawRelayoutContent();
            m_objRegionOfLayoutting |= contentFrame;
        }

        pViewGroup->LayoutContentInRect(contentFrame);
        updateRegionOverView(pViewGroup, m_objRegionOfLayoutting);
    }
    m_pLayoutingView = NULL;
    m_objRegionOfLayoutting.clear();
}

bool xWorkStation::asyncRelayoutViewContent(xView* pView)
{
    if (NULL == pView || NULL == pView->m_pChild || pView != pView->m_pChild->m_pParent)
    {
        return false;
    }

    pView->m_nRelayoutAction |= xView::RELAYOUT_ACTION_CONTENT;

    return asyncRelayoutView(pView, false);
}

bool xWorkStation::asyncRelayoutViewBound(xView* pView)
{
    if (NULL == pView)
    {
        return false;
    }

    pView->m_nRelayoutAction |= xView::RELAYOUT_ACTION_BOUND;

    return asyncRelayoutView(pView, true);
}

bool xWorkStation::asyncRelayoutView(xView* pView, bool bForceRelayout)
{
#if IF_XKERNEL_THREAD_CHECK
    chASSERT(chThreadLocal::IsCalledByThisThread());
#endif

#if IF_DEBUG_VIEW_MGR
    XWINDOW_INFO("asyncRelayoutView View:%s ForceRelayout:%d",
                 NULL == pView ? "" : pView->viewNick().c_str(), (bForceRelayout ? 1 : 0));
#endif
    // chASSERT(!m_bLoopInRelayoutViews);
    if (pView == NULL || pView->getHostWindow() == NULL)
    {
        return false;
    }

    if (bForceRelayout)
    {
        pView->m_nRelayoutAction |= xView::RELAYOUT_ACTION_FORCE;
    }

    // seek to the top-most view with force relayout
    while (pView->m_pParent != NULL && pView->m_pParent->IsForceRelayout())
    {
        pView = pView->m_pParent;
    }

    if (!pView->IsInRelayoutList())
    {
        chASSERT(!IsViewInList(m_listRelayoutViews, pView));

#if IF_DEBUG_VIEW_MGR
        XWINDOW_INFO("Push Relayout View:%s", pView->viewNick().c_str());
#endif
        m_listRelayoutViews.push_back(pView);
        pView->m_nAsyncListType |= xView::VIEW_ASYNC_LIST_TYPE_RELAYOUT;
        if (m_listRelayoutViews.size() == 1)
        {
#if IF_XWINDOW_SYNC_RELAYOUT
            g_WorkStation.sendThreadMsgEx(VIEW_EVENT_NONAME, 0, 0,
                                          MSG_METHOD(this, xWorkStation::onRelayoutView));
#else
            PostWindowEvent(STATION_WINDOW_EVENT_LAYOUT, 0, 0);
#endif
        }
    }
    return true;
}

void xWorkStation::resetChildrenLayout(xView* parent)
{
    for (xViewIterator it = parent->child_head(); it.hasData(); ++it)
    {
        xView* child = it.operator ->();

        child->m_nRelayoutAction |= xView::RELAYOUT_ACTION_FORCE;
        resetChildrenLayout(child);
    }
}

void xWorkStation::resetChildrenRelayout(xView* parent)
{
    for (xViewIterator it = parent->child_head(); it.hasData(); ++it)
    {
        xView* child = it.operator ->();

        child->ResetForceRelayout();
        resetChildrenRelayout(child);
    }
}

void xWorkStation::relayoutAllWindow()
{
    ViewOrWindow vw = getNextViewOrWindow(ViewOrWindow(), true);
    while (vw.hasData())
    {
        xView* pView = vw.getViewObject();
        if (pView != NULL)
        {
            resetChildrenLayout(pView);
            asyncRelayoutViewContent(pView);
        }
        vw = getNextViewOrWindow(vw, true);
    }
}

BOOL xWorkStation::onRelayoutView(msgObject&)
{
    chASSERT(!m_bLoopInRelayoutViews);

#if IF_DEBUG_VIEW_MGR
    XWINDOW_INFO("Begin loop in onRelayoutView views:%d m_bExposedRegionChanged:%d",
                 m_listRelayoutViews.size(), (m_bExposedRegionChanged ? 1 : 0));
#endif

    m_bLoopInRelayoutViews = true;
    UINT nRedrawCount = m_uRedrawMagic;
    while (0 != m_listRelayoutViews.size())
    {
        xView* pView = m_listRelayoutViews.back();

        m_listRelayoutViews.pop_back();

        if (NULL == pView)
        {
            continue;
        }

        pView->ResetInRelayoutList();

        if (NULL == pView->getHostWindow())
        {
            continue;
        }

        bool bBoundRelayout = pView->IsBoundRelayout();

        pView->ResetRangeRelayout();

        if (bBoundRelayout)
        {
            if (pView->m_pParent != NULL)
            {
                RelayoutViewGroupContent(pView->m_pParent);
            }
            else
            {
                RootLayoutParams::RelayoutRootView(pView);
            }
        }
        else
        {
            RelayoutViewGroupContent(static_cast<xViewGroup*>(pView));
        }
    }
    m_bLoopInRelayoutViews = false;

#if IF_DEBUG_VIEW_MGR
    XWINDOW_INFO("End loop in onRelayoutView RedrawCount:%d RedrawMagic:%d views:%d m_bExposedRegionChanged:%d",
                 nRedrawCount, m_uRedrawMagic, m_listRelayoutViews.size(), (m_bExposedRegionChanged ? 1 : 0));
#endif

    if (nRedrawCount != m_uRedrawMagic)
    {
        m_bExposedRegionChanged = true;
    }

    if (m_bExposedRegionChanged)
    {
        m_bExposedRegionChanged = false;
        signalLayoutChanged.emitSignal(0, 0, 0, 0);
        chASSERTx(!m_bExposedRegionChanged && 0 == m_listRelayoutViews.size(), "DO NOT invoke layout in signalLayoutChanged");
    }

    return TRUE;
}

void xWorkStation::updateBackground()
{
    const chRect& rcScreen = getScreenRect();
    xRegion region(rcScreen);
    ViewOrWindow vw;

    queuedRedrwRegion(vw, region, rcScreen);
}

void xWorkStation::update_helper(xView* pView, const chRect& rect)
{
    if (pView->m_bAppear)
    {
#if IF_DEBUG_PAINTER
        XWINDOW_INFO("update_helper(%s:%p) geometry:%d %d %d %d", pView->viewNick().c_str(), pView,
                     rect.left, rect.top, rect.right, rect.bottom);
#endif

        xRegion region(rect);
        if (!region.empty())
        {
            ViewOrWindow vwView(pView);
            ViewOrWindow vwNext = getNextViewOrWindow(vwView, false);
            if (upwardUpdateFrom(vwNext, region, false))
            {
                downwardUpdateFrom(vwView, region);
            }
        }
    }
}

void xWorkStation::update_helper(xView* pView)
{
    update_helper(pView, getVisibleRect(pView));
}

void xWorkStation::viewRectChanged(xView* pView, const chRect& rectOld, const chRect& rectNew)
{
    if (pView->m_bAppear)
    {
        chASSERT(m_pLayoutingView != NULL && (m_pLayoutingView == pView || containChildView(m_pLayoutingView, pView)));
#if IF_DEBUG_PAINTER
        XWINDOW_INFO("viewRectChanged %s:%p(%d,%d->%d,%d) => (%d,%d->%d,%d)", pView->viewNick().c_str(), pView,
                     rectOld.left, rectOld.top, rectOld.right, rectOld.bottom,
                     rectNew.left, rectNew.top, rectNew.right, rectNew.bottom);
#endif
        // if(rectOld.TopLeft() == rectNew.TopLeft()) // TBD
        m_objRegionOfLayoutting |= rectOld;
        m_objRegionOfLayoutting |= rectNew;
    }
}

void xWorkStation::exposeView(xView* pView, const chRect& rect, bool bRedrawViewOnly)
{
    chASSERT(pView->m_bAppear);
#if IF_DEBUG_VIEW_MGR || IF_DEBUG_PAINTER
    XWINDOW_INFO("exposeView:%s %d", pView->viewNick().c_str(), (bRedrawViewOnly ? 1 : 0));
#endif
    ViewOrWindow vwView(pView);
    xRegion region(rect);
    if (!region.empty())
    {
        UINT uOldMagic = m_uRedrawMagic;
        ViewOrWindow vwNext = getNextViewOrWindow(vwView, true);
        if (upwardUpdateFrom(vwNext, region, false))
        {
            ViewOrWindow vwTopMostChild(getTopMostVisibleChild(pView));
            downwardUpdateFrom(vwTopMostChild, region);
        }
        if (!bRedrawViewOnly && uOldMagic != m_uRedrawMagic)
        {
            // DO NOT changed ExposedRegion flag in update()
            m_bExposedRegionChanged = true;
        }
#if IF_DEBUG_VIEW_MGR
        XWINDOW_INFO("finish exposeView OldMagic:%d RedrawMagic:%d ExposedRegionChanged:%d",
                     uOldMagic, m_uRedrawMagic, (m_bExposedRegionChanged ? 1 : 0));
#endif
    }
}

void xWorkStation::exposeView(xView* pView, bool bRedrawViewOnly)
{
    exposeView(pView, getVisibleRect(pView), bRedrawViewOnly);
}

void xWorkStation::unexposeView(xView* pView)
{
    chASSERT(pView->m_bAppear);
#if IF_DEBUG_VIEW_MGR || IF_DEBUG_PAINTER
    XWINDOW_INFO("unexposeView:%s", pView->viewNick().c_str());
#endif
    xRegion region(getVisibleRect(pView));
    if (!region.empty())
    {
        UINT uOldMagic = m_uRedrawMagic;

        ViewOrWindow vwView(pView);
        ViewOrWindow vwNext = getNextViewOrWindow(vwView, true);
        if (upwardUpdateFrom(vwNext, region, false))
        {
            ViewOrWindow vwPrev = getPrevVisibleViewOrWindow(vwView);
            downwardUpdateFrom(vwPrev, region);
        }
        if (uOldMagic != m_uRedrawMagic)
        {
            m_bExposedRegionChanged = true;
        }
#if IF_DEBUG_VIEW_MGR
        XWINDOW_INFO("finish unexposeView OldMagic:%d RedrawMagic:%d ExposedRegionChanged:%d",
                     uOldMagic, m_uRedrawMagic, (m_bExposedRegionChanged ? 1 : 0));
#endif
    }
}

void xWorkStation::updateRegionOverView(xView* pView, xRegion& region)
{
#if IF_DEBUG_PAINTER
    dump();
#endif
    chASSERT(pView != NULL);
    if (pView->m_bAppear)
    {
        const chRect& rcScreen = getScreenRect();

        region &= rcScreen;

        if (!region.empty())
        {
            ViewOrWindow vwView(pView);
            ViewOrWindow vwNext = getNextViewOrWindow(vwView, true);
            if (upwardUpdateFrom(vwNext, region, false))
            {
                ViewOrWindow vwTopMostChild(getTopMostVisibleChild(pView));
                downwardUpdateFrom(vwTopMostChild, region);
            }
        }
    }
}

bool xWorkStation::upwardUpdateFrom(ViewOrWindow vw, xRegion& region, bool calcuRegionOnly)
{
#if IF_DEBUG_VIEW_MGR || IF_DEBUG_PAINTER
    xView* pFromView = NULL;
    bool bWindow = false;

    if (vw.isViewObject())
    {
        pFromView = vw.getViewObject();
    }
    else
    {
        dataWindowObject* pWindow = vw.getWindowObject();

        if (NULL != pWindow)
        {
            pFromView = pWindow->hostView;
        }
    }

    chRect rcRegion = region.bound();

    if (NULL != pFromView)
    {
        XWINDOW_INFO("upwardUpdateFrom View:%s Window:%d Region:%d %d %d %d calcuRegionOnly:%d",
                     pFromView->viewNick().c_str(), (bWindow ? 1 : 0), rcRegion.left, rcRegion.top,
                     rcRegion.right, rcRegion.bottom, (calcuRegionOnly ? 1 : 0));
    }
    else
    {
        XWINDOW_INFO("upwardUpdateFrom View is null Window:%d Region:%d %d %d %d calcuRegionOnly:%d",
                     (bWindow ? 1 : 0), rcRegion.left, rcRegion.top, rcRegion.right,
                     rcRegion.bottom, (calcuRegionOnly ? 1 : 0));
    }
#endif

    while (vw.hasData())
    {
        bool bSkipChildren = false;
        if (vw.isViewObject())
        {
            xView* pView = vw.getViewObject();
            chASSERT(pView != NULL);

            if (NULL == pView)
            {
                break;
            }

            chRect rcClient = pView->clientGeometry();

#if IF_DEBUG_VIEW_MGR || IF_DEBUG_PAINTER
            XWINDOW_INFO("View:%s Appear:%d Overlap:%d Rect:%d %d %d %d",
                         pView->viewNick().c_str(), (pView->m_bAppear ? 1 : 0), (pView->overlap ? 1 : 0),
                         pView->m_rectLayout.left, pView->m_rectLayout.top, pView->m_rectLayout.right,
                         pView->m_rectLayout.bottom);
#endif
            if (!pView->m_bAppear)
            {
                bSkipChildren = true;
            }
            else if (!pView->overlap)
            {
                // pView is overwrite mode
                region -= rcClient;
                if (region.empty())
                {
                    return false;
                }
                // skip all children
                bSkipChildren = true;
            }
            else if (!calcuRegionOnly)
            {
                // overlap mode
                xRegion rgnClip = region & rcClient;

                if (!rgnClip.empty())
                {
                    queuedRedrwRegion(vw, region, rcClient);
                }
            }
        }
        else
        {
            dataWindowObject* pWindow = vw.getWindowObject();

            chASSERT(pWindow != NULL);

            if (NULL == pWindow)
            {
                break;
            }

#if IF_DEBUG_VIEW_MGR || IF_DEBUG_PAINTER
            xView* pHostView = pWindow->hostView;

            XWINDOW_INFO("Window:%s visible:%d Overlap:%d Rect:%d %d %d %d",
                         (NULL != pHostView ? pHostView->viewNick().c_str() : ""),
                         pWindow->visible, pWindow->overlap, pWindow->rect.left,
                         pWindow->rect.top, pWindow->rect.right, pWindow->rect.bottom);
#endif
            if (pWindow->visible)
            {
                if (!pWindow->overlap)
                {
                    region -= pWindow->rect;
                    if (region.empty())
                    {
                        return false;
                    }
                }
                else if (!calcuRegionOnly)
                {
                    xRegion rgnClip = region & pWindow->rect;

                    if (!rgnClip.empty())
                    {
                        queuedRedrwRegion(vw, region, pWindow->rect);
                    }
                }
            }
        }
        vw = getNextViewOrWindow(vw, bSkipChildren);
    }
    return true;
}

bool xWorkStation::downwardUpdateFrom(ViewOrWindow vw, xRegion& region)
{
#if IF_DEBUG_VIEW_MGR || IF_DEBUG_PAINTER
    xView* pFromView = NULL;
    bool bWindow = false;

    if (vw.isViewObject())
    {
        pFromView = vw.getViewObject();
    }
    else
    {
        dataWindowObject* pWindow = vw.getWindowObject();

        if (NULL != pWindow)
        {
            pFromView = pWindow->hostView;
        }
    }

    chRect rcRegion = region.bound();

    if (NULL != pFromView)
    {
        XWINDOW_INFO("downwardUpdateFrom View:%s Window:%d Region:%d %d %d %d",
                     pFromView->viewNick().c_str(), (bWindow ? 1 : 0), rcRegion.left,
                     rcRegion.top, rcRegion.right, rcRegion.bottom);
    }
    else
    {
        XWINDOW_INFO("downwardUpdateFrom View is null Window:%d Region:%d %d %d %d",
                     (bWindow ? 1 : 0), rcRegion.left, rcRegion.top, rcRegion.right, rcRegion.bottom);
    }
#endif

    while (vw.hasData())
    {
        if (vw.isViewObject())
        {
            xView* pView = vw.getViewObject();
            chASSERT(pView != NULL);

            if (NULL == pView)
            {
                break;
            }

            chRect rectVisible = getVisibleRect(pView);
            xRegion subRegion = region & rectVisible;
            if (!subRegion.empty())
            {
                queuedRedrwRegion(vw, region, rectVisible);
                if (!pView->overlap)
                {
                    region = region - rectVisible;
                    if (region.empty())
                    {
                        return false;
                    }
                }
            }
        }
        else
        {
            dataWindowObject* pWindow = vw.getWindowObject();

            chASSERT(pWindow != NULL);

            if (NULL == pWindow)
            {
                break;
            }

            xRegion subRegion = region & pWindow->rect;
            if (!subRegion.empty())
            {
                queuedRedrwRegion(vw, region, pWindow->rect);
                if (!pWindow->overlap)
                {
                    region = region - pWindow->rect;
                    if (region.empty())
                    {
                        return false;
                    }
                }
            }
        }
        vw = getPrevVisibleViewOrWindow(vw);
    }

    const chRect& rcScreen = getScreenRect();
    xRegion rgnScreen = region & rcScreen;

    if (!rgnScreen.empty())
    {
        queuedRedrwRegion(vw, region, rcScreen);
    }

    region.clear();
    return false;
}

void xWorkStation::pushRedrawRegion(const ViewOrWindow& vw, const xRegion& region, const chRect& rcClip)
{
    typeRedrawRegionList::iterator it = m_listRedrawRegions.begin();

    for (; it != m_listRedrawRegions.end(); ++it)
    {
        ST_REDRAW_REGION_PTR pRedrawRegion = *it;

        if (!pRedrawRegion.IsEmpty() && vw == pRedrawRegion->vw)
        {
            break;
        }
    }

    ++m_uRedrawMagic;

#if IF_DEBUG_PAINTER
    bool bInsert = false;
#endif

    if (it != m_listRedrawRegions.end())
    {
        ST_REDRAW_REGION_PTR pRedrawRegion = *it;

        if (!pRedrawRegion.IsEmpty())
        {
            pRedrawRegion->region |= region;
            pRedrawRegion->region &= rcClip;
        }
    }
    else
    {
        // sort in z-order top is last
        for (it = m_listRedrawRegions.begin(); it != m_listRedrawRegions.end(); ++it)
        {
            ST_REDRAW_REGION_PTR pRedrawRegion = *it;

            if (!pRedrawRegion.IsEmpty() && !AOnTopOfB(vw, pRedrawRegion->vw))
            {
                break;
            }
        }

        xView* pView = vw.getViewObject();
        if (pView != NULL)
        {
            pView->m_nAsyncListType |= xView::VIEW_ASYNC_LIST_TYPE_REDRAW;
            chASSERT(pView->m_bAppear);
        }

        ST_REDRAW_REGION_PTR pRedrawRegion(new ST_REDRAW_REGION);

        if (!pRedrawRegion.IsEmpty())
        {
            pRedrawRegion->vw = vw;
            pRedrawRegion->region = region;
            pRedrawRegion->region &= rcClip;

            m_listRedrawRegions.insert(it, pRedrawRegion);
#if IF_DEBUG_PAINTER
            bInsert = true;
#endif
        }
    }

#if IF_DEBUG_PAINTER
    xView* pView = vw.getViewObject();
    chRect rcRgn = region.bound();

    if (NULL != pView)
    {
        XWINDOW_INFO("pushRedrawRegion(Insert:%d) view:%s region:%d %d %d %d", (bInsert ? 1 : 0),
                     pView->viewNick().c_str(), rcRgn.left, rcRgn.top, rcRgn.right, rcRgn.bottom);
    }
    else
    {
        XWINDOW_INFO("pushRedrawRegion(Insert:%d) window:%p region:%d %d %d %d", (bInsert ? 1 : 0),
                     vw.getWindowObject(), rcRgn.left, rcRgn.top, rcRgn.right, rcRgn.bottom);
    }
#endif
}

void xWorkStation::queuedRedrwRegion(const ViewOrWindow& vw, const xRegion& region, const chRect& rcClip)
{
#if IF_XKERNEL_THREAD_CHECK
    chASSERT(chThreadLocal::IsCalledByThisThread());
#endif
    if (!region.empty())
    {
#if IF_DEBUG_PAINTER
        xView* pView = vw.getViewObject();
        chRect rect = region.bound();
        if (pView != NULL)
        {
            XWINDOW_INFO("queuedRedrwRegion %s(%d,%d->%d,%d)@%dx%d",
                         pView->viewNick().c_str(), rect.left, rect.top,
                         rect.right, rect.bottom, rect.Width(), rect.Height());
        }
        else
        {
            XWINDOW_INFO("queuedRedrwRegion station(%d,%d->%d,%d)@%dx%d",
                         rect.left, rect.top, rect.right, rect.bottom, rect.Width(), rect.Height());
        }
#endif
        bool bNeedPostMessage = (0 == m_listRedrawRegions.size());

        pushRedrawRegion(vw, region, rcClip);

        if (bNeedPostMessage)
        {
            PostWindowEvent(STATION_WINDOW_EVENT_FLUSH_REGION, 0, 0);
        }
    }
}

#if IF_VIEW_MASK
void xWorkStation::flushMaskRegionUnderView(xView* pView)
{
    chASSERT(pView != NULL);

    while (0 != m_stackMaskRegions.size())
    {
        ST_REDRAW_REGION_PTR pRedrawRegion = m_stackMaskRegions.back();

        if (pRedrawRegion.IsEmpty())
        {
            m_stackMaskRegions.pop_back();
            continue;
        }

        xView* pParentView = pRedrawRegion->vw.getViewObject();

        if (containChildView(pParentView, pView))
        {
            break;
        }

        paintMaskView(pParentView, pRedrawRegion->region);
        m_stackMaskRegions.pop_back();
    }
}

bool xWorkStation::pushMaskRegionOverView(ST_REDRAW_REGION_PTR pRedrawRegion, xRegion& region)
{
    chASSERT(!pRedrawRegion.IsEmpty());

    xView* pView = pRedrawRegion->vw.getViewObject();
    chASSERT(pView != NULL);

    //if pView->overlap == true; parent must in m_listRedrawRegions and parent-mask-region in m_stackMaskRegions
    if (!pView->overlap)
    {
        typeMaskRegionStack::iterator itInsertPos = m_stackMaskRegions.end();

        for (xView* pParent = pView->m_pParent; pParent != NULL; pParent = pParent->m_pParent)
        {
            if (!pParent->mask)
            {
                continue;
            }

            typeMaskRegionStack::iterator itParentInstack = m_stackMaskRegions.begin();

            for (; itParentInstack != m_stackMaskRegions.end(); ++itParentInstack)
            {
                ST_REDRAW_REGION_PTR pRedrawRegion = *itParentInstack;

                if (pRedrawRegion.IsEmpty())
                {
                    continue;
                }

                if (pRedrawRegion->vw.getViewObject() == pParent)
                {
                    break;
                }
            }

            ST_REDRAW_REGION_PTR pRedrawRegion;

            if (itParentInstack != m_stackMaskRegions.end())
            {
                pRedrawRegion = *itParentInstack;

                if (pRedrawRegion.IsEmpty())
                {
                    m_stackMaskRegions.erase(itParentInstack);
                }
            }

            if (pRedrawRegion.IsEmpty())
            {
                ST_REDRAW_REGION_PTR pRedrawRegion = ST_REDRAW_REGION_PTR(new ST_REDRAW_REGION);

                if (pRedrawRegion.IsEmpty())
                {
                    return false;
                }

                pRedrawRegion->vw = ViewOrWindow(pParent);
                pRedrawRegion->region = region;

                itInsertPos = m_stackMaskRegions.insert(itInsertPos, pRedrawRegion);
            }
            else
            {
                pRedrawRegion->region |= region;
                itInsertPos = itParentInstack;
            }
        }
    }

    if (pView->mask)
    {
        m_stackMaskRegions.push_back(pRedrawRegion);
    }

    return true;
}

void xWorkStation::flushMaskRegions()
{
    while (0 != m_stackMaskRegions.size())
    {
        ST_REDRAW_REGION_PTR pRedrawRegion = m_stackMaskRegions.back();

        if (!pRedrawRegion.IsEmpty())
        {
            xView* pParentView = pRedrawRegion->vw.getViewObject();

            paintMaskView(pParentView, pRedrawRegion->region);
        }

        m_stackMaskRegions.pop_back();
    }
}
#endif

BOOL xWorkStation::onFlushRedrawRegion(msgObject&)
{
    if (NULL == m_pScreen)
    {
        return FALSE;
    }

#if IF_DEBUG_PAINTE_TIME
    XWIN_TRACE_TIME("Begin Paint in FlushRedrawRegion");
#endif

    m_pScreen->OnBeginPaint();

    typeRedrawRegionList::iterator it = m_listRedrawRegions.begin();

    for (; it != m_listRedrawRegions.end(); ++it)
    {
        ST_REDRAW_REGION_PTR pRedrawRegion = *it;

        if (pRedrawRegion.IsEmpty())
        {
            continue;
        }

        ViewOrWindow& vw = pRedrawRegion->vw;
        xRegion& region = pRedrawRegion->region;

        if (!vw.hasData())
        {
#if IF_DEBUG_PAINTER
            chRect rect = region.bound();
            XWINDOW_INFO("background (%d,%d->%d,%d)", rect.left, rect.top, rect.right, rect.bottom);
#endif
#if IF_VIEW_MASK
            chASSERT(0 == m_stackMaskRegions.size());
#endif
            // station background
            paintStationBackground(region);
        }
        else if (vw.isViewObject())
        {
            xView* pView = vw.getViewObject();

            chASSERT(NULL != pView);

            if (NULL == pView)
            {
                continue;
            }

#if IF_VIEW_MASK
            flushMaskRegionUnderView(pView);
#endif

            region &= getVisibleRect(pView);
            paintView(pView, region);

#if IF_VIEW_MASK
            pushMaskRegionOverView(pRedrawRegion, region);
#endif
        }

        xView* pRedrawView = vw.getViewObject();
        if (pRedrawView != NULL)
        {
            pRedrawView->ResetInRedrawList();
        }
    }

#if IF_VIEW_MASK
    flushMaskRegions();
#endif
    m_listRedrawRegions.clear();

#if IF_XWINDOW_STATION_ALPHA
    if (0xff != m_stProfile.m_nGlobalAlpha)
    {
        m_pScreen->BlendGlobalAlpha(m_stProfile.m_nGlobalAlpha);
    }
#endif

    m_pScreen->OnEndPaint();

#if IF_DEBUG_PAINTE_TIME
    XWIN_TRACE_TIME("End Paint in FlushRedrawRegion");
#endif

    return TRUE;
}

void xWorkStation::paintStationBackground(const xRegion& region)
{
    xBrush& brushStation = m_stProfile.m_brushBackground;

    if (NULL == m_pScreen || brushStation.isNull())
    {
        return;
    }

    chRect rcScreen(0, 0, m_pScreen->Width(), m_pScreen->Height());
    chRect rcBound = region.bound() & rcScreen;

    if (rcBound.IsEmpty())
    {
        return;
    }

    m_pScreen->SetClipRegion(region, true);

    if (brushStation.isColor())
    {
        m_pScreen->FillRect(rcScreen, brushStation.GetColor());
    }
    else
    {
#if IF_SCREEN_SUPPORT_BG_RENDER
        m_pScreen->PaintScreenBGImage(rcScreen, brushStation.pixmap());
#else
        m_pScreen->FillRect(rcScreen, brushStation.pixmap());
#endif
    }

    xRegion rgnEmpty;

    m_pScreen->SetClipRegion(rgnEmpty);
}

bool xWorkStation::paintView(xView* pView, const xRegion& region)
{
#if IF_DEBUG_PAINTER
    XWINDOW_INFO("paintView %s:%p(%d,%d->%d,%d)@%dx%d",
                 pView->viewNick().c_str(), pView, pView->m_rectLayout.left, pView->m_rectLayout.top,
                 pView->m_rectLayout.right, pView->m_rectLayout.bottom,
                 pView->m_rectLayout.Width(), pView->m_rectLayout.Height());
#endif
    chASSERT(pView->m_bAppear);

    if (region.empty() || NULL == m_pScreen)
    {
        return false;
    }

    chRect rcScreen(0, 0, m_pScreen->Width(), m_pScreen->Height());

    xRegion rgnBG = region & pView->clientGeometry();

    rgnBG &= rcScreen;

    if (rgnBG.empty())
    {
        return false;
    }

    xPainter painterBG(m_pScreen, pView->m_rectLayout.TopLeft(), rgnBG);
    xViewEvent_paint evb(VIEW_PAINT_BACKGROUND, painterBG);

    dispatchDirectionalViewEvent(pView, &evb);

    xRegion rgnContent = region & pView->contentGeometry();

    rgnContent &= rcScreen;

    if (rgnContent.empty())
    {
        return false;
    }

    xPainter canvas(m_pScreen, pView->m_rectLayout.TopLeft(), rgnContent);
    xViewEvent_paint evc(VIEW_PAINT_CONTENT, canvas);

    dispatchDirectionalViewEvent(pView, &evc);

    return true;
}

#if IF_VIEW_MASK
bool xWorkStation::paintMaskView(xView* pView, const xRegion& region)
{
#if IF_DEBUG_PAINTER
    XWINDOW_INFO("paintMaskView %s:%p(%d,%d->%d,%d)@%dx%d",
                 pView->viewNick().c_str(), pView, pView->m_rectLayout.left, pView->m_rectLayout.top,
                 pView->m_rectLayout.right, pView->m_rectLayout.bottom,
                 pView->m_rectLayout.Width(), pView->m_rectLayout.Height());
#endif

    chASSERT(pView->m_bAppear);
    if (pView->mask && NULL != m_pScreen)
    {
        xRegion rgnPaint = region & pView->clientGeometry();

        if (rgnPaint.empty())
        {
            return false;
        }

        xPainter canvas(m_pScreen, pView->m_rectLayout.TopLeft(), rgnPaint);
        xViewEvent_paint evb(VIEW_PAINT_MASK, canvas);
        dispatchDirectionalViewEvent(pView, &evb);
    }
    return true;
}
#endif

//////////////////////////////////////////////////////////////////////////
bool xWorkStation::bringWindowToTop_helper(xWindow* pWindow)
{
    if (NULL == pWindow)
    {
        return false;
    }

#if IF_DEBUG_VIEW_MGR
    XWINDOW_INFO("bringWindowToTop_helper %s", (NULL != pWindow ? pWindow->viewNick().c_str() : ""));
#endif

    dataWindowObject* pWindowObject = pWindow->m_pWindow;

    chASSERT(NULL == pWindow->m_pParent && NULL != pWindowObject && (pWindow->m_bAppear));

    dataWindowObject* pTopWindowObject = getTopWindowByLevel(pWindowObject->topLevel);

    if (pTopWindowObject == pWindowObject)
    {
        return false;
    }

    popWindowObject(pWindowObject);
    pushWindowObject(pWindowObject);

    // 先移除已存在的视图内容，重新展开
    clearReDrawRegionByWindow(pWindow);
    exposeView(pWindow, false);

    return true;
}

void xWorkStation::clearReDrawRegionByWindow(xWindow *pWindow)
{
    if (NULL == pWindow)
    {
        return;
    }

    xView* pView = NULL;
    for (typeRedrawRegionList::iterator it = m_listRedrawRegions.begin(); it != m_listRedrawRegions.end();)
    {
        ST_REDRAW_REGION_PTR pRedrawRegion = *it;

        if (pRedrawRegion.IsEmpty())
        {
            continue;
        }

        if (pRedrawRegion->vw.isViewObject())
        {
            pView =  pRedrawRegion->vw.getViewObject();
            if (pView != pWindow && NULL != pView)
            {
                pView = pView->getRootView();
            }
        }
        else
        {
            dataWindowObject* pWindowObject = pRedrawRegion->vw.getWindowObject();
            if (NULL != pWindowObject)
            {
                pView = pWindowObject->hostView;
            }
            else
            {
                pView = NULL;
            }
        }

        if (NULL != pView && pView == pWindow)
        {
            it = m_listRedrawRegions.erase(it);
        }
        else
        {
            ++it;
        }
    }

}

xView* xWorkStation::getPrevVisibleView(xView* pView)
{
    xView* pPrev = pView->m_pPrev;
    while (pPrev != NULL && !pPrev->m_bAppear)
    {
        pPrev = pPrev->m_pPrev;
    }
    return pPrev;
}

xView* xWorkStation::getTopMostVisibleChild(xView* pView)
{
    chASSERT(pView->m_bAppear);
    while (pView->m_pLastChild != NULL)
    {
        if (!pView->m_pLastChild->m_bAppear)
        {
            xView* pVisibleChild = getPrevVisibleView(pView->m_pLastChild);
            if (pVisibleChild == NULL)
            {
                return pView;
            }
            pView = pVisibleChild;
        }
        else
        {
            pView = pView->m_pLastChild;
        }
    }
    return pView;
}

bool xWorkStation::containChildView(const xView* pParent, const xView* pChild)
{
    if (NULL == pParent || NULL == pChild)
    {
        return false;
    }

    if (pChild == pParent)
    {
        return true;
    }
    while (pChild != NULL)
    {
        if (pChild->m_pParent == pParent)
        {
            return true;
        }
        pChild = pChild->m_pParent;
    }
    return false;
}

ViewOrWindow xWorkStation::getNextViewOrWindow(const ViewOrWindow& vw, bool bSkipChildern)
{
    if (!vw.hasData())
    {
        dataWindowObject * pNextWindow = NULL;

        if (m_lsWindowObject.size() > 0)
        {
            pNextWindow = m_lsWindowObject.front();
        }

        return isLocalWindow(pNextWindow) ? ViewOrWindow(pNextWindow->hostView) : ViewOrWindow(pNextWindow);
    }

    xView* pView = vw.getViewObject();
    dataWindowObject* pWindow = NULL;
    if (pView != NULL)
    {
        if (!bSkipChildern && pView->m_pChild != NULL)
        {
            return ViewOrWindow(pView->m_pChild);
        }
        if (pView->m_pNext != NULL)
        {
            return ViewOrWindow(pView->m_pNext);
        }

        while (pView->m_pParent != NULL)
        {
            pView = pView->m_pParent;
            if (pView->m_pNext != NULL)
            {
                return ViewOrWindow(pView->m_pNext);
            }
        }

        pWindow = pView->m_pWindow;
    }
    else if (vw.isWindowObject())
    {
        pWindow = vw.getWindowObject();
    }
    chASSERT(pWindow != NULL);

    dataWindowObject* pNextWindow = getNextWindowObject(pWindow);

    if (NULL == pNextWindow)
    {
        return ViewOrWindow();
    }

    return isLocalWindow(pNextWindow) ? ViewOrWindow(pNextWindow->hostView) : ViewOrWindow(pNextWindow);
}

ViewOrWindow xWorkStation::getPrevVisibleViewOrWindow(const ViewOrWindow& vw)
{
    chASSERT(vw.hasData());
    dataWindowObject* pWindow = NULL;
    if (vw.isViewObject())
    {
        xView* pView = vw.getViewObject();
        chASSERT(NULL != pView);

        if (NULL == pView)
        {
            return ViewOrWindow();
        }

        chASSERT(pView->m_bAppear);

        xView* pVisible = getPrevVisibleView(pView);
        if (pVisible != NULL)
        {
            return ViewOrWindow(getTopMostVisibleChild(pVisible));
        }
        if (pView->m_pParent != NULL)
        {
            return ViewOrWindow(pView->m_pParent);
        }

        // view is root view
        pWindow = pView->m_pWindow;
    }
    else if (vw.isWindowObject())
    {
        pWindow = vw.getWindowObject();
    }

    chASSERT(pWindow != NULL);

    dataWindowObject* pPrevWindow = getPreWindowObject(pWindow, true);

    if (NULL == pPrevWindow)
    {
        return ViewOrWindow();
    }

    return isLocalWindow(pPrevWindow) ? ViewOrWindow(getTopMostVisibleChild(pPrevWindow->hostView)) : ViewOrWindow(pPrevWindow);
}

bool xWorkStation::AOnTopOfB(const ViewOrWindow& A, const ViewOrWindow& B)
{
    if (!A.hasData())
    {
        return false;
    }
    if (!B.hasData())
    {
        return true;
    }

    chASSERT((!(A == B)));
    dataWindowObject* pWindowA = A.isWindowObject() ? A.getWindowObject() : A.getViewObject()->getRootView()->m_pWindow;
    dataWindowObject* pWindowB = B.isWindowObject() ? B.getWindowObject() : B.getViewObject()->getRootView()->m_pWindow;
    if (pWindowA != pWindowB && NULL != pWindowB)
    {
        dataWindowObject* pPreWindow = getPreWindowObject(pWindowA, false);
        bool bResult = false;

        while (NULL != pPreWindow)
        {
            if (pPreWindow == pWindowB)
            {
                bResult = true;
                break;
            }

            pPreWindow = getPreWindowObject(pPreWindow, false);
        }

        return bResult;
    }
    else
    {
        chASSERT(!A.isWindowObject() && !B.isWindowObject());
        ViewOrWindow vw = getNextViewOrWindow(B, false);
        while (vw.hasData() && vw.getViewObject()->m_pParent != NULL)
        {
            if (vw == A)
            {
                return true;
            }
            vw = getNextViewOrWindow(vw, false);
        }
        return false;
    }
}

void xWorkStation::show_helper(xView* pView, int nShowCmd/* = xView::SHOW_CMD_DEFAULT*/)
{
    if (NULL == pView || pView->m_bClosed)
    {
        XWINDOW_WARN("show_helper %p fail.", pView);

        return;
    }

#if IF_DEBUG_VIEW_MGR || IF_DEBUG_PAINTER
    XWINDOW_INFO("show_helper:%s ShowCmd:%d", pView->viewNick().c_str(), nShowCmd);

#if IF_DEBUG_VIEW_MGR
    pView->dump();
#endif
#endif

    bool bShowRootView = pView->m_pParent == NULL;
    bool bReShowRootView = false;

    // show root view: create window object in need
    if (bShowRootView && NULL == pView->m_pWindow)
    {
        bool bSetActive = (xView::SHOW_CMD_SET_ACTIVE == (xView::SHOW_CMD_SET_ACTIVE & nShowCmd));

        pView->m_pWindow = createWindow(pView, TML_DEFAULT, !bSetActive);
    }

    bool bFromDisappearToAppear = !pView->m_bAppear && (bShowRootView || pView->m_pParent->m_bAppear);
    pView->visibility = xView::visible;
    // from hide to show
    if (bFromDisappearToAppear)
    {
        recursiveViewAppear(pView, true);

        dataWindowObject* pWindow = pView->m_pWindow;
        bool bExposed = false;

        if (bShowRootView && NULL != pWindow)
        {
            pWindow->visible = true;

            if (xView::SHOW_CMD_BRING_TO_TOP == (xView::SHOW_CMD_BRING_TO_TOP & nShowCmd))
            {
                // bring to top and active root view
                bExposed = bringWindowToTop_helper(pView);
            }

            if (xView::SHOW_CMD_SET_ACTIVE == (xView::SHOW_CMD_SET_ACTIVE & nShowCmd)
                    && pWindow->activable)
            {
                setActiveWindowObject(pView->m_pWindow);
            }
        }

        if (!bExposed)
        {
            exposeView(pView, false);
        }
    }
}

void xWorkStation::hide_helper(xView* pView, bool bGone)
{
#if IF_DEBUG_VIEW_MGR || IF_DEBUG_PAINTER
    XWINDOW_INFO("hide_helper:%s Gone:%d", pView->viewNick().c_str(), (bGone ? 1 : 0));
#endif

#if IF_GESTURE_ENABLE
    g_GestureManager.CancelGesture(pView);
#endif

    if (pView->m_bAppear)
    {
        releaseFocusOnViewDisappear(pView);
        unexposeView(pView);
        recursiveViewAppear(pView, false);
    }

    pView->visibility = bGone ? xView::gone : xView::invisible;

    if (pView->m_pWindow != NULL)
    {
        pView->m_pWindow->visible = false;
    }

#if IF_MULTI_TOUCH_ENABLE
    ResetTouchCacheView(pView);
#endif
}

// way to destroy view
// 1.pView->close();
// 2.delete pView/View object on stack;
// 3.close then delete/view on stack
void xWorkStation::close_helper(xView* pView, bool bDeleteLater)
{
#if IF_DEBUG_VIEW_MGR || IF_DEBUG_PAINTER
    XWINDOW_INFO("close_helper:%s DeleteLater:%d", pView->viewNick().c_str(), (bDeleteLater ? 1 : 0));
#endif

    if (pView->m_bClosed)
    {
        return;
    }
    pView->m_bClosed = true;

#if IF_GESTURE_ENABLE
    g_GestureManager.ClearGesture(pView);
#endif

    if (pView->m_pParent != NULL)
    {
        // close child view
        pView->m_pParent->popupChildView(pView);
    }
    else
    {
        // close root view
        if (pView->m_bAppear)
        {
            releaseFocusOnViewDisappear(pView);
            unexposeView(pView);
            recursiveViewAppear(pView, false);
        }

        dataWindowObject* pWindowObject = pView->m_pWindow;

        if (NULL != pWindowObject)
        {
            // window
            popWindowObject(pWindowObject);
            freeWindowObject(pWindowObject);

            pView->m_pWindow = NULL;
        }
    }

    recursiveViewClosed(pView);

    if (bDeleteLater)
    {
        // push to delete queue
        chASSERT(!IsViewInList(m_listDeleteViews, pView) && !pView->IsInDeleteList());
        m_listDeleteViews.push_front(pView);
        pView->m_nAsyncListType |= xView::VIEW_ASYNC_LIST_TYPE_DELETE;
        if (m_listDeleteViews.size() == 1)
        {
            PostWindowEvent(STATION_WINDOW_EVENT_DELETE_VIEW, 0, 0);
        }
    }
}

dataWindowObject* xWorkStation::createWindow(xView* pRootView, BYTE nTopMostLevel, bool bWaitSetActive/* = false*/)
{
    if (NULL == pRootView || pRootView->m_bClosed)
    {
        XWINDOW_WARN("createWindow %p fail.", pRootView);

        return NULL;
    }

#if IF_DEBUG_VIEW_MGR
    XWINDOW_INFO("createWindow:%s TopMostLevel:%d WaitActive:%d Window:%p", pRootView->viewNick().c_str(),
                 (int)nTopMostLevel, (bWaitSetActive ? 1 : 0), pRootView->m_pWindow);
#endif

    if (NULL != pRootView->m_pWindow)
    {
        return pRootView->m_pWindow;
    }

    LayoutParamsInParent* pLayoutInParent = pRootView->getLayoutInParent();
    RootLayoutParams* pRootLayoutParams = RootLayoutParams::GetLayoutParams(pLayoutInParent);
    chASSERT(NULL != pRootLayoutParams);

    if (NULL == pRootLayoutParams)
    {
        return NULL;
    }

    dataWindowObject * pWindow = allocWindowObject();

    if (NULL == pWindow)
    {
        return NULL;
    }

    pWindow->topLevel = nTopMostLevel;
    pWindow->hostView = pRootView;
    pWindow->rect = pRootView->m_rectLayout;
    pWindow->activable = !pRootView->disable && pRootLayoutParams->activable;
    pWindow->m_bWaitSetActive = bWaitSetActive;
    // DO NOT set focusable="false" for root view
    chASSERT(pRootView->focusable != xView::bool_false);
    pWindow->focusView = getFocusableChildView(pRootView);

    pWindow->visible = false;
    pWindow->overlap = pRootView->overlap;
    pRootView->m_pWindow = pWindow;

    pushWindowObject(pWindow);

    pRootLayoutParams->RelayoutRootView();

    return pWindow;
}

dataWindowObject* xWorkStation::allocWindowObject()
{
#if IF_XWINDOW_CACHE_WINDOW_OBJECT
    if (m_lsCacheWindowObject.size() > 0)
    {
        dataWindowObject* pWindowObject = m_lsCacheWindowObject.back();

        m_lsCacheWindowObject.pop_back();

        return pWindowObject;
    }
#endif

    dataWindowObject* pWindowObject = new dataWindowObject;

    return pWindowObject;
}

void xWorkStation::freeWindowObject(dataWindowObject* pWindowObject)
{
    if (NULL == pWindowObject)
    {
        return;
    }

#if IF_XWINDOW_CACHE_WINDOW_OBJECT
    if (m_lsCacheWindowObject.size() < XWINDOW_OBJECT_CACHE_MAX_COUNT)
    {
        m_lsCacheWindowObject.push_front(pWindowObject);

        return;
    }
#endif

    delete pWindowObject;
    pWindowObject = NULL;
}

void xWorkStation::pushWindowObject(dataWindowObject* pWindowObject)
{
    if (NULL == pWindowObject)
    {
        return;
    }

    LIST_WINDOW_OBJECT::iterator iter = m_lsWindowObject.begin();

    for (; iter != m_lsWindowObject.end(); ++iter)
    {
        dataWindowObject* pTmpWindowObject = *iter;

        if (NULL != pTmpWindowObject && pTmpWindowObject->topLevel < pWindowObject->topLevel)
        {
            m_lsWindowObject.insert(iter, pWindowObject);
            return;
        }
    }

    m_lsWindowObject.push_back(pWindowObject);
}

void xWorkStation::popWindowObject(dataWindowObject* pWindowObject)
{
    if (NULL == pWindowObject)
    {
        return;
    }

    LIST_WINDOW_OBJECT::iterator iter = m_lsWindowObject.begin();

    for (; iter != m_lsWindowObject.end(); ++iter)
    {
        dataWindowObject* pTmpWindowObject = *iter;

        if (pTmpWindowObject == pWindowObject)
        {
            m_lsWindowObject.erase(iter);
            return;
        }
    }
}

static bool hasChildView(xView* pParent, const xView* pTarget)
{
    xView* pChild = pParent->m_pChild;
    while (pChild != NULL)
    {
        if (pChild == pTarget || hasChildView(pChild, pTarget))
        {
            return true;
        }
        pChild = pChild->m_pNext;
    }
    return false;
}

bool xWorkStation::isLocalWindow(dataWindowObject* pWindow)
{
#if IF_XKERNEL_THREAD_CHECK
    return pWindow != NULL && pWindow->getHostTaskId() == hostTaskId();
#else
    return NULL != pWindow;
#endif
}

chRect xWorkStation::getVisibleRect(const xView* pView)
{
    chRect result = pView->clientGeometry();

    while (pView->m_pParent != NULL)
    {
        result &= DeflateRect(pView->m_pParent->m_rectLayout, pView->m_pParent->paddingLeft, pView->m_pParent->paddingTop, pView->m_pParent->paddingRight, pView->m_pParent->paddingBottom);
        pView = pView->m_pParent;
    }

    const chRect& rcScreen = getScreenRect();

    result &= rcScreen;

    return result;
}

const chRect& xWorkStation::getScreenRect()
{
    if (m_rcScreen.IsEmpty() && NULL != m_pScreen)
    {
        m_rcScreen.SetRect(0, 0, m_pScreen->Width(), m_pScreen->Height());
    }

    return m_rcScreen;
}

#if IF_SCREEN_SUPPORT_DEV_PIXEL_SCALE
int xWorkStation::getScreenDevPixelX(int nX)
{
    if (NULL != m_pScreen)
    {
        return m_pScreen->GetDevPixelX(nX);
    }

    return nX;
}

int xWorkStation::getScreenDevPixelY(int nY)
{
    if (NULL != m_pScreen)
    {
        return m_pScreen->GetDevPixelY(nY);
    }

    return nY;
}
#endif

int xWorkStation::width()
{
    if (NULL != m_pScreen)
    {
        return m_pScreen->Width();
    }

    return 0;
}

int xWorkStation::height()
{
    if (NULL != m_pScreen)
    {
        return m_pScreen->Height();
    }

    return 0;
}

void xWorkStation::closeAllWindow()
{
    // from bottom to top
    ViewOrWindow vw = getNextViewOrWindow(ViewOrWindow(), true);
    while (vw.hasData())
    {
        xView* pView = vw.getViewObject();
        vw = getNextViewOrWindow(vw, true);
        if (pView != NULL)
        {
            pView->close();
        }
    }
}

#if IF_VIEW_ASYNC_CLICK
void xWorkStation::AsyncClick(xView* pView)
{
    PostWindowEvent(STATION_WINDOW_EVENT_CLICK, (WPARAM)pView, 0);
}

BOOL xWorkStation::onSyncClick(msgObject& msg)
{
    xView* pView = (xView*)msg.wParam;

    if (NULL != pView && pView->containFocus())
    {
        pView->onClick();
        pView->signalClicked.emitSignal(0, 0, 0, NULL);
    }

    return TRUE;
}
#endif

#if IF_MULTI_TOUCH_ENABLE
void xWorkStation::SetTouchHookProc(TOUCH_HOOK_PROC pTouchHookProc)
{
    m_pFunTouchHookProc = pTouchHookProc;
}

bool xWorkStation::CheckViewActiveAndFocus(xView* pView, xView* pFocusView)
{
    if (NULL == pView || NULL == pFocusView)
    {
        return false;
    }

    xView* pRootView = pView->getRootView();

    if (NULL == pRootView)
    {
        return false;
    }

    dataWindowObject* pWindow = pRootView->m_pWindow;

    if (NULL == pWindow)
    {
        return false;
    }

    if (pWindow != g_WorkStation.getActiveWindowObject())
    {
        g_WorkStation.setActiveWindowObject(pWindow);
    }

    // 如果是焦点控件，在Active消息之后，立即再发一次focus事件，保证delay click消息正确
    if (pFocusView == pWindow->focusView)
    {
        setFocus_helper(pFocusView);
    }
    else if (pView == pWindow->focusView)
    {
        setFocus_helper(pView);
    }
    else
    {
        if (pView == pFocusView)
        {
            pFocusView->setFocus();
        }
        else if (checkViewFocusble(pFocusView) && (!containChildView(pFocusView, pWindow->focusView)))
        {
            setFocus_helper(pFocusView);
        }
    }

    return true;
}

BOOL xWorkStation::onTouchEvent(msgObject&)
{
#if IF_DEBUG_TOUCH_POINT
    XWINDOW_INFO("onTouchEvent");
#endif

    CTouchPoints* pTouchPoint = MTD_PopupTouchPoints();

    if (NULL != pTouchPoint)
    {
#if IF_DEBUG_TOUCH_POINT
        pTouchPoint->Dump();
#endif

        if (NULL != m_pFunTouchHookProc && (*m_pFunTouchHookProc)(pTouchPoint))
        {
            MTD_ReleaseTouchPoints(pTouchPoint);

            return TRUE;
        }

        const LIST_TOUCH_POINT& lsTouchPoint = pTouchPoint->GetTouchPoint();

        dispatchTouchBeganEvent(lsTouchPoint);

        LIST_TOUCH_POINT_ID lsPointID;

        dispatchTouchEvent(TOUCH_POINT_STATE_MOVED, VIEW_TOUCH_MOVED, lsTouchPoint, lsPointID);

        lsPointID.clear();

        dispatchTouchEvent(TOUCH_POINT_STATE_RELEASED, VIEW_TOUCH_ENDED, lsTouchPoint, lsPointID);
        dispatchTouchEvent(TOUCH_POINT_STATE_CANCEL, VIEW_TOUCH_CANCEL, lsTouchPoint, lsPointID);
        ResetTouchCacheView(lsPointID);

#if IF_GESTURE_ENABLE
        LIST_TOUCH_POINT::const_iterator iter = lsTouchPoint.begin();
        LIST_TOUCH_POINT::const_iterator iterEnd = lsTouchPoint.end();

        for (; iter != iterEnd; ++iter)
        {
            ST_TOUCH_POINT* pTouchPoint = (*iter).Get();

            if (NULL == pTouchPoint || TOUCH_POINT_PROCESS == (TOUCH_POINT_PROCESS & pTouchPoint->nState)
                    || IsTouchPointOnView(pTouchPoint->nPosX, pTouchPoint->nPosY))
            {
                continue;
            }

            OnDisengagedPoint(pTouchPoint);
        }
#endif
    }

    MTD_ReleaseTouchPoints(pTouchPoint);

    return TRUE;
}

void xWorkStation::dispatchTouchBeganEvent(const LIST_TOUCH_POINT& lsTouchPoint)
{
    LIST_TOUCH_POINT::const_iterator iter = lsTouchPoint.begin();
    LIST_TOUCH_POINT::const_iterator iterEnd = lsTouchPoint.end();
    LIST_TOUCH_EVENT lsTouchEvent;

    for (; iter != iterEnd; ++iter)
    {
        ST_TOUCH_POINT* pTouchPoint = (*iter).Get();

        if (NULL == pTouchPoint || TOUCH_POINT_STATE_PRESSED != pTouchPoint->nState)
        {
            continue;
        }

        int nPointID = pTouchPoint->nID;
        xView* pAnchorView = getTouchAnchorViewByPos(pTouchPoint->nPosX, pTouchPoint->nPosY);

        if (NULL == pAnchorView || !pAnchorView->touchable)
        {
            continue;
        }

        m_mapCacheTouchView[nPointID] = pAnchorView;

        xTouchEvent* pTouchEvent = getAnchorViewEvent(pAnchorView, lsTouchEvent);

        if (NULL == pTouchEvent)
        {
            pTouchEvent = allocTouchEvent(VIEW_TOUCH_BEGAN, pAnchorView);

            if (NULL == pTouchEvent)
            {
                continue;
            }

            lsTouchEvent.push_back(pTouchEvent);
        }

        pTouchPoint->nState |= TOUCH_POINT_PROCESS;

        pTouchEvent->AddTouchPoint(pTouchPoint);
    }

    dispatchTouchEvent(lsTouchEvent);
    clearTouchEventList(lsTouchEvent);
}

void xWorkStation::dispatchTouchEvent(int nPointState, int nEventType, const LIST_TOUCH_POINT& lsTouchPoint, LIST_TOUCH_POINT_ID& lsPointID)
{
    LIST_TOUCH_POINT::const_iterator iter = lsTouchPoint.begin();
    LIST_TOUCH_EVENT lsTouchEvent;

    for (; iter != lsTouchPoint.end(); ++iter)
    {
        ST_TOUCH_POINT* pTouchPoint = (*iter).Get();

        if (NULL == pTouchPoint || nPointState != pTouchPoint->nState)
        {
            continue;
        }

        int nPointID = pTouchPoint->nID;
        xView* pAnchorView = getTouchAnchorViewByID(nPointID);

        if (NULL == pAnchorView || !pAnchorView->touchable)
        {
            continue;
        }

        lsPointID.push_back(nPointID);

        xTouchEvent* pTouchEvent = getAnchorViewEvent(pAnchorView, lsTouchEvent);

        if (NULL == pTouchEvent)
        {
            pTouchEvent = allocTouchEvent(nEventType, pAnchorView);

            if (NULL == pTouchEvent)
            {
                continue;
            }

            lsTouchEvent.push_back(pTouchEvent);
        }

        pTouchPoint->nState |= TOUCH_POINT_PROCESS;

        pTouchEvent->AddTouchPoint(pTouchPoint);
    }

    dispatchTouchEvent(lsTouchEvent);
    clearTouchEventList(lsTouchEvent);
}

void xWorkStation::dispatchTouchEvent(LIST_TOUCH_EVENT& lsTouchEvent)
{
    LIST_TOUCH_EVENT::iterator iter = lsTouchEvent.begin();

    for (; iter != lsTouchEvent.end(); ++iter)
    {
        xTouchEvent* pTouchEvent = *iter;

        if (NULL == pTouchEvent || NULL == pTouchEvent->m_pAnchorView)
        {
            continue;
        }

        dispatchNonDirectionalViewKeyEvent(pTouchEvent->m_pAnchorView, pTouchEvent);
#if IF_GESTURE_ENABLE
        g_GestureManager.ProcessTouchEvent(pTouchEvent);
#endif
    }
}

xView* xWorkStation::getTouchAnchorViewByPos(int nX, int nY)
{
    chPoint ptTouch(nX, nY);
    xView* pAnchorView = NULL;

    ipcWindowStackObject::Lock();

    dataWindowList& windowList = getObject();

    for (dataWindowList::iterator it = windowList.tail(); it.hasData(); --it)
    {
        dataWindowObject* pWindow = it.operator ->();

        if (NULL == pWindow || !pWindow->visible || NULL == pWindow->hostView)
        {
            continue;
        }

        pAnchorView = getTouchAnchorView(pWindow->hostView, ptTouch);

        if (pWindow->rect.PtInRect(ptTouch))
        {
            break;
        }
    }

    ipcWindowStackObject::Unlock();

    return pAnchorView;
}

xView* xWorkStation::getTouchAnchorView(xView* pView, const chPoint& ptTouch)
{
    if (NULL == pView || !pView->m_rectLayout.PtInRect(ptTouch) || pView->disable || !pView->checkVisible())
    {
        return NULL;
    }

    for (xViewIterator itView = pView->xViewTree::child_head(); itView.hasData(); ++itView)
    {
        xView* pChildView = itView.operator ->();
        xView* pAnchorView = getTouchAnchorView(pChildView, ptTouch);

        if (NULL != pAnchorView)
        {
            return pAnchorView;
        }
    }

    if (pView->touchable)
    {
        return pView;
    }

    return NULL;
}

bool xWorkStation::IsTouchPointOnView(int nX, int nY)
{
    dataWindowObject* pAnchorWindow = NULL;
    chPoint ptTouch(nX, nY);

    ipcWindowStackObject::Lock();

    dataWindowList& windowList = getObject();

    for (dataWindowList::iterator it = windowList.tail(); it.hasData(); --it)
    {
        dataWindowObject* pWindow = it.operator ->();

        if (NULL == pWindow || !pWindow->visible || NULL == pWindow->hostView)
        {
            continue;
        }

        if (pWindow->rect.PtInRect(ptTouch))
        {
            pAnchorWindow = pWindow;
            break;
        }
    }

    ipcWindowStackObject::Unlock();

    return (NULL != pAnchorWindow);
}

bool xWorkStation::OnDisengagedPoint(ST_TOUCH_POINT* pTouchPoint)
{
    return false;
}

xView* xWorkStation::getTouchAnchorViewByID(int nPointID)
{
    MAP_TOUCH_CACHE_VIEW::iterator iter = m_mapCacheTouchView.find(nPointID);

    if (iter != m_mapCacheTouchView.end())
    {
        return iter->second;
    }

    return NULL;
}

xTouchEvent* xWorkStation::allocTouchEvent(int nEventType, xView* pAnchorView)
{
    xTouchEvent* pTouchEvent = NULL;

    if (m_lsCacheTouchEvent.size() > 0)
    {
        pTouchEvent = m_lsCacheTouchEvent.front();

        m_lsCacheTouchEvent.pop_front();

        if (NULL != pTouchEvent)
        {
            pTouchEvent->type = nEventType;
            pTouchEvent->m_pAnchorView = pAnchorView;
        }
    }
    else
    {
        pTouchEvent = new xTouchEvent(nEventType, pAnchorView);
    }

    return pTouchEvent;
}

void xWorkStation::recycleTouchEvent(xTouchEvent* pTouchEvent)
{
    if (NULL == pTouchEvent)
    {
        return;
    }

    pTouchEvent->m_listPoint.clear();

    if (m_lsCacheTouchEvent.size() < MAX_TOUCH_EVENT_CACHE_SIZE)
    {
        m_lsCacheTouchEvent.push_back(pTouchEvent);
    }
    else
    {
        delete pTouchEvent;
    }
}

xTouchEvent* xWorkStation::getAnchorViewEvent(xView* pAnchorView, LIST_TOUCH_EVENT& lsTouchEvent)
{
    LIST_TOUCH_EVENT::iterator iter = lsTouchEvent.begin();

    for (; iter != lsTouchEvent.end(); ++iter)
    {
        xTouchEvent* pTouchEvent = *iter;

        if (NULL != pTouchEvent && pAnchorView == pTouchEvent->m_pAnchorView)
        {
            return pTouchEvent;
        }
    }

    return NULL;
}

void xWorkStation::clearTouchEventList(LIST_TOUCH_EVENT& lsTouchEvent)
{
    LIST_TOUCH_EVENT::iterator iter = lsTouchEvent.begin();

    for (; iter != lsTouchEvent.end(); ++iter)
    {
        recycleTouchEvent(*iter);
    }

    lsTouchEvent.clear();
}

void xWorkStation::ResetTouchCacheView(const LIST_TOUCH_POINT_ID& lsPointID)
{
    LIST_TOUCH_POINT_ID::const_iterator iter = lsPointID.begin();
    LIST_TOUCH_POINT_ID::const_iterator iterEnd = lsPointID.end();

    for (; iter != iterEnd; ++iter)
    {
        int nPointID = *iter;

        MAP_TOUCH_CACHE_VIEW::iterator iterView = m_mapCacheTouchView.find(nPointID);

        if (iterView != m_mapCacheTouchView.end())
        {
            m_mapCacheTouchView.erase(iterView);
        }
    }
}

void xWorkStation::ResetTouchCacheView(xView* pAnchorView)
{
    if (NULL == pAnchorView)
    {
        return;
    }

    MAP_TOUCH_CACHE_VIEW::iterator iter = m_mapCacheTouchView.begin();
    MAP_TOUCH_CACHE_VIEW mapHoldTouchView;

    // ToDo:后续发送Cancel消息
    while (iter != m_mapCacheTouchView.end())
    {
        xView* pView = iter->second;

        if (pView == pAnchorView)
        {
            int nTouchPointID = iter->first;
            xView* pNewHeldView = g_GestureManager.FindTouchPointHoldView(pView, nTouchPointID);

            if (NULL != pNewHeldView)
            {
                mapHoldTouchView[nTouchPointID] = pNewHeldView;
            }

            iter = m_mapCacheTouchView.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    iter = mapHoldTouchView.begin();

    for (; iter != mapHoldTouchView.end(); ++iter)
    {
        int nTouchPointID = iter->first;
        xView* pView = iter->second;

        if (NULL != pView)
        {
            m_mapCacheTouchView[nTouchPointID] = pView;
        }
    }
}
#endif

BOOL xWorkStation::onStationEvent(msgObject& msg)
{
    switch (msg.message)
    {
    case STATION_WINDOW_EVENT:
        {
            return onWindowEvent(msg);
        }
        break;
    case TASK_FETCH_KEY_EVENT:
        {
            return onProcessFetchKeyEvent(msg);
        }
        break;
    case STATION_RESIZE_EVENT:
        {
            onStationResized(width(), height());

            return TRUE;
        }
        break;
    default:
        break;
    }

    return FALSE;
}

BOOL xWorkStation::onWindowEvent(msgObject& msg)
{
    if (m_listWindowEvent.size() <= 0)
    {
        return FALSE;
    }

    ST_STATION_WINDOW_EVENT stEvent = m_listWindowEvent.back();

    m_listWindowEvent.pop_back();

    msg.wParam = stEvent.m_wParam;
    msg.lParam = stEvent.m_lParam;

    switch (stEvent.m_nEventType)
    {
    case STATION_WINDOW_EVENT_LAYOUT:
        {
            onRelayoutView(msg);
        }
        break;
    case STATION_WINDOW_EVENT_PEND_TILL_LAYOUT:
        {
            signalPendTillLayout.emitSignal(msg.wParam, msg.lParam, 0, NULL);
        }
        break;
    case STATION_WINDOW_EVENT_ACTIVE:
        {
            onSyncActiveWindow(msg);
        }
        break;
    case STATION_WINDOW_EVENT_FOCUS:
        {
            onSyncFocusView(msg);
        }
        break;
    case STATION_WINDOW_EVENT_PEND_TILL_FOCUS:
        {
            signalPendTillFocus.emitSignal(msg.wParam, msg.lParam, 0, NULL);
        }
        break;
#if IF_VIEW_ASYNC_CLICK
    case STATION_WINDOW_EVENT_CLICK:
        {
            onSyncClick(msg);
        }
        break;
#endif
    case STATION_WINDOW_EVENT_FLUSH_REGION:
        {
            onFlushRedrawRegion(msg);
        }
        break;
#if IF_KEY_REPEAT
    case STATION_WINDOW_EVENT_REPEAT_KEY_PRESS:
        {
            int nKey = (int)msg.wParam;

            ProcessKeyPress(nKey, true);
        }
        break;
#endif
    case STATION_WINDOW_EVENT_DELETE_VIEW:
        {
            onDeleteViews(msg);
        }
        break;
    default:
        return FALSE;
        break;
    }

    return TRUE;
}

void xWorkStation::onStationResized(int nOldWidth, int nOldHeight)
{
    ViewOrWindow vw = getNextViewOrWindow(ViewOrWindow(), true);
    while (vw.hasData())
    {
        if (vw.isViewObject())
        {
            xView* pView = vw.getViewObject();
            chASSERT(pView != NULL);

            if (NULL == pView)
            {
                break;
            }

            if (pView->m_bAppear)
            {
                vw = ViewOrWindow(getTopMostVisibleChild(pView));
                break;
            }
        }
        vw = getNextViewOrWindow(vw, true);
    }

    const chRect& rcScreen = getScreenRect();

    xRegion region(rcScreen);

    region = region - chRect(0, 0, nOldWidth, nOldHeight);

    downwardUpdateFrom(vw, region);
    // downwardUpdateFrom(vw, region);

    vw = getNextViewOrWindow(ViewOrWindow(), true);
    while (vw.hasData())
    {
        xView* pView = vw.getViewObject();
        vw = getNextViewOrWindow(vw, true);
        RootLayoutParams::RelayoutRootView(pView);
    }
}

BOOL xWorkStation::onDeleteViews(msgObject& msg)
{
    while (0 != m_listDeleteViews.size())
    {
        xView* pView = m_listDeleteViews.back();
        chASSERT(pView->m_bClosed);

        m_listDeleteViews.pop_back();
        pView->ResetInDeleteList();

        delete pView;
    }
    return TRUE;
}

#if IF_XWINDOW_DUMP_EXPORT
bool xWorkStation::DumpAllVisibleWindow(xml_document& doc)
{
    LIST_WINDOW_OBJECT::iterator iter = m_lsWindowObject.begin();
    LIST_WINDOW_OBJECT::iterator iterEnd = m_lsWindowObject.end();

    for (; iter != iterEnd; ++iter)
    {
        dataWindowObject * pWindow = *iter;

        if (NULL == pWindow || !pWindow->visible)
        {
            continue;
        }

        xView* pFocusView = NULL;
        bool bActive = false;

        if (pWindow == m_pActiveWindowObject)
        {
            pFocusView = pWindow->focusView;
            bActive = true;
        }

        g_ViewDumpMgr.DumpView(pWindow->hostView, pFocusView, bActive, doc);
    }

    return true;
}

bool xWorkStation::ScreenCapture(xPixmap& pmScreen)
{
    if (NULL == m_pScreen)
    {
        return false;
    }

    return m_pScreen->Capture(pmScreen);
}

bool xWorkStation::InputText(yl::string& strText)
{
    xView* pFocusView = getFocusView();
    xTextView* pTextView = g_ViewDumpMgr.GetTextView(pFocusView);

    if (NULL == pTextView)
    {
        return false;
    }

    pTextView->SetInitText(strText);

    return true;
}
#endif

//////////////////////////////////////////////////////////////////////////
xWorkStation& xWorkStation::getCurrentStation()
{
#if IF_MULTI_XKERNEL_ENABLE
    xWorkStation* pStation = getTlsValue<xWorkStation*>(NULL);

    chASSERT(NULL != pStation);

    return *pStation;
#else
    chASSERT(NULL != s_pWorkStation);

    return *s_pWorkStation;
#endif
}
