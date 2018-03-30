#include "xmlbrowseruiframe.h"
#include "cuiregionbase.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "cuiregiontextscreen.h"
#include "cuiregioninputscreen.h"
#include "cuiregiontextmenu.h"
#include "cuiregionformattext.h"
#include "cuiregionimagescreen.h"
// #include "cuiregiontextstatus.h"
#include "cuiregiondirectory.h"

#if XML_BROWSER_SUPPORT_UI
using namespace yl;

namespace
{
#define TIMER_1S            1000
//#define XML_BROWSER_FRAME_XML "xmlbrowserui/xmlbrowserframe.xml"
}
////////////////////////////////////////////////////////////////////////////////////////////
EXPORT_VIEW_CLASS(xXMLBrowserUIFrame);

xXMLBrowserUIFrame::xXMLBrowserUIFrame()
    : m_pCurrentRegion(NULL)
    , m_ePageType(XB_UI_TYPE_DLG)
    , m_iTimeOut(0)
{
    InitData();
}

xXMLBrowserUIFrame::~xXMLBrowserUIFrame()
{
    UnbindOldRegion();
    // 清空界面的数据
    ClearData();
}

void xXMLBrowserUIFrame::InitData()
{
    m_ArrpRegion.clear();
}

// 更新界面
void xXMLBrowserUIFrame::UpdateUI(const XMLLogicToUIData & objData)
{
    XMLBROWSERUI_INFO("xXMLBrowserUIFrame::UpdateUI, eElementType [%d].", objData.eElementType);
    // 退出界面的时间
    int iQuitTime = 0;

    CUIRegionBase* pRegionBase = GetUIByElementID(objData.nElementID);

    if (NULL == pRegionBase)
    {
        pRegionBase = CreateUIByType(objData.eElementType);
        if (NULL == pRegionBase)
        {
            return;
        }
        addChildView(pRegionBase);
        // 将Region的指针插入链表
        m_ArrpRegion.push_back(pRegionBase);
        // 刷新Region的数据
        pRegionBase->SetElementID(objData.nElementID);
        pRegionBase->Update(objData.pElementData);
    }

    UnbindOldRegion();
    // 连接相关的信号槽
    SLOT_CONNECT(pRegionBase, m_signalAction, this, xXMLBrowserUIFrame::OnRegionAction);
    pRegionBase->show();
    pRegionBase->setFocus();

    // 取所有Region中最长退出时间为Frame的退出时间
    int iConfigQuitTime = pRegionBase->GetQuitTime();
    iQuitTime = max(iQuitTime, iConfigQuitTime);

    // 若Region为空则不作处理
    if (0 == m_ArrpRegion.size())
    {
        return;
    }

    m_pCurrentRegion = pRegionBase;

    // 启动计时
    XMLBROWSERUI_INFO("XMLBrowser Quit Time(%d)", iQuitTime);
    ReSetTimer(iQuitTime);
}

VecSoftkeyParam & xXMLBrowserUIFrame::GetCurrentUISoftkey()
{
    if (NULL != m_pCurrentRegion)
    {
        return m_pCurrentRegion->GetSoftkey();
    }

    static VecSoftkeyParam vecSoft;
    return vecSoft;
}

// 当softkey点击时调用的函数
void xXMLBrowserUIFrame::OnSoftkeyBtnClick(const SoftkeyParam & objParam)
{
    if (NULL == m_pCurrentRegion)
    {
        return;
    }

    int iElementID = m_pCurrentRegion->GetElementID();
    XMLBROWSERUI_INFO("xXMLBrowserUIFrame::OnSoftkeyBtnClick eSoftkeyType(%d), GetElementID(%d)",
                      objParam.eType, iElementID);

    switch (objParam.eType)
    {
    case XB_ST_EXIT:
        {
            xmlbrowser_OnAction(iElementID, XB_ACTION_EXIT);
        }
        break;
    case XB_ST_SELECT:
        {
            int iSelectItem = -1;
            if (NULL != m_pCurrentRegion)
            {
                iSelectItem = m_pCurrentRegion->GetCurrentItemIndex();
            }
            xmlbrowser_OnAction(iElementID, XB_ACTION_SELECT, (void *)&iSelectItem);
        }
        break;
    case XB_ST_DIAL:
        {
            int iSelectItem = -1;
            if (NULL != m_pCurrentRegion)
            {
                iSelectItem = m_pCurrentRegion->GetCurrentItemIndex();
            }
            xmlbrowser_OnAction(iElementID, XB_ACTION_DIAL, (void *)&iSelectItem);
        }
        break;
    case XB_ST_SUBMIT:
        {
            OnClickSoftKeySubmit();
        }
        break;
    case XB_ST_PREVIOUS:
        {
            xmlbrowser_OnAction(iElementID, XB_ACTION_PREVIOUS);
        }
        break;
    case XB_ST_NEXT:
        {
            xmlbrowser_OnAction(iElementID, XB_ACTION_NEXT);
        }
        break;
    case XB_ST_DOT:
    case XB_ST_BACKSPACE:
    case XB_ST_NEXTSPACE:
    case XB_ST_CHANGEMODE:
        {
            CUIRegionInputScreen * pInputUIRegion = CUIRegionInputScreen::GetInputScreenUIRegion(
                    m_pCurrentRegion);
            if (NULL == pInputUIRegion)
            {
                return;
            }

            switch (objParam.eType)
            {
            case XB_ST_DOT:
                {
                    pInputUIRegion->OnClickSoftKeyDot();
                }
                break;
            case XB_ST_BACKSPACE:
                {
                    pInputUIRegion->OnClickSoftKeyBackSpace();
                }
                break;
            case XB_ST_NEXTSPACE:
                {
                    pInputUIRegion->OnClickSoftKeyNextSpace();
                }
                break;
            case XB_ST_CHANGEMODE:
                {
                    pInputUIRegion->OnClickSoftKeyChangeIME();
                }
                break;
            default:
                break;
            }
        }
        break;
    case XB_ST_FUNCTION:
        {
            if (NULL != m_pCurrentRegion
                    && (XMT_MENU == m_pCurrentRegion->GetElementType()
                        || XMT_INPUT == m_pCurrentRegion->GetElementType()))
            {
                // 处理SoftKey按键
                ActionSoftkeyData data;
                data.iFocusedItem = m_pCurrentRegion->GetCurrentItemIndex();
                data.strURI = objParam.strURI;
                xmlbrowser_OnAction(iElementID, XB_ACTION_URI, (void *)&data);
            }
            else
            {
                xmlbrowser_OnAction(iElementID, XB_ACTION_URI, (void *)objParam.strURI.c_str());
            }
        }
        break;
#if IF_FEATURE_XMLDIRECTORY
    case XB_ST_SEARCH:
        {
            if (NULL != m_pCurrentRegion)
            {
                m_pCurrentRegion->OnXMLUIAction(XB_ST_SEARCH);
            }
        }
        break;
#endif
    default:
        {
            // 处理SoftKey按键
            ActionSoftkeyData data;
            data.iFocusedItem = m_pCurrentRegion->GetCurrentItemIndex();
            data.strURI = objParam.strURI;
            xmlbrowser_OnAction(m_pCurrentRegion->GetElementID(), XB_ACTION_SOFTKEY, (void *)&data);
        }
        break;
    }
}

// 清空界面数据
void xXMLBrowserUIFrame::ClearData()
{
    // 释放本地资源
    VEC_UIREGION::iterator it = m_ArrpRegion.begin();
    for (; it != m_ArrpRegion.end(); ++it)
    {
        CUIRegionBase * pRegion = *it;
        if (NULL != pRegion)
        {
            popupChildView(pRegion);
            // 释放Region资源
            delete pRegion;
            pRegion = NULL;
        }
    }
    m_ArrpRegion.clear();

    m_pCurrentRegion = NULL;
}

void xXMLBrowserUIFrame::UnbindOldRegion()
{
    // 解绑旧界面资源
    VEC_UIREGION::iterator it = m_ArrpRegion.begin();
    for (; it != m_ArrpRegion.end(); ++it)
    {
        CUIRegionBase * pRegion = *it;
        if (NULL != pRegion && pRegion == m_pCurrentRegion)
        {
            pRegion->hide();
            // 断开相关的信号槽
            SLOT_DISCONNECT(pRegion, m_signalAction, this, xXMLBrowserUIFrame::OnRegionAction);
            m_pCurrentRegion = NULL;
            break;
        }
    }
}

// 启动计时器
void xXMLBrowserUIFrame::RestartTimer(int iTime)
{
    XMLBROWSERUI_INFO("xXMLBrowserUIFrame::RestartTimer(%d)", iTime);
    if (iTime > 0)
    {
        // 先停止计时
        StopTimer();
        // 重新启动计时
        m_timerQuit.SingleShot(iTime * TIMER_1S, TIMER_METHOD(this, xXMLBrowserUIFrame::OnQuitTimerTimeOut));
    }
}

// 停止计时器
void xXMLBrowserUIFrame::StopTimer()
{
    m_timerQuit.KillTimer();
}

// 重新设置计时器
void xXMLBrowserUIFrame::ReSetTimer(int iTime)
{
    if (iTime > 0)
    {
        m_iTimeOut = iTime;
        RestartTimer(m_iTimeOut);
    }
}

void xXMLBrowserUIFrame::OnClickSoftKeySubmit()
{
    if (NULL != m_pCurrentRegion)
    {
        // 获取当前界面输入的数据
        MapPageData objMap;
        bool bLegalData = m_pCurrentRegion->GetPageData(objMap);
        if (bLegalData)
        {
            //submit和按ok键confirm处理相同
            xmlbrowser_OnAction(m_pCurrentRegion->GetElementID(), XB_ACTION_CONFIRM, (void *)&objMap);
        }
    }
}

void xXMLBrowserUIFrame::OnRegionAction(xSignal * sender, WPARAM wParam, LPARAM lParam,
                                        int nDataBytes, LPVOID pData)
{
    if (NULL == sender || NULL == m_pCurrentRegion)
    {
        return;
    }
    switch (wParam)
    {
    case REGION_ACTION_CLICK_ITEM:
        {
            int iCurrentItem = m_pCurrentRegion->GetCurrentItemIndex();
            xmlbrowser_OnAction(m_pCurrentRegion->GetElementID(), XB_ACTION_CONFIRM, (void *)&iCurrentItem);
        }
        break;
    case REGION_ACTION_CURRENT_ITEM_CHANGE:
        {
            int iCurrentItem = m_pCurrentRegion->GetCurrentItemIndex();
            XMLBROWSERUI_INFO("iCurrentItem(%d)", iCurrentItem);
            xmlbrowser_OnAction(m_pCurrentRegion->GetElementID(), XB_ACTION_FOCUSCHANGE, (void *)&iCurrentItem);
            // 发射消息更新softkey
            m_signalFocusChange.emitSignal(1, 0, 0, NULL);
        }
        break;
    default:
        break;
    }
}

void xXMLBrowserUIFrame::OnQuitTimerTimeOut(xThreadTimer* timer, LPVOID pExtraData)
{
    // 停止计时器
    StopTimer();
    xmlbrowser_Exit();
}

// 根据类型创建ElementUI
CUIRegionBase * xXMLBrowserUIFrame::CreateUIByType(XML_ELEMENT_TYPE eType)
{
    CUIRegionBase * pRetVal = NULL;
    switch (eType)
    {
    case XMT_MENU :
        pRetVal = new CUIRegionTextMenu();
        break;
    case XMT_TEXTSCREEN:
        pRetVal = new CUIRegionTextScreen();
        break;
    case XMT_INPUT:
        pRetVal = new CUIRegionInputScreen();
        break;
    case XMT_DIRECTORY:
        pRetVal = new CUIRegionDirectory();
        break;
    case XMT_FORMATTEDTEXTSCREEN:
        pRetVal = new CUIRegionFormatText();
        break;
    case XMT_IMAGEMENU:
    case XMT_IMAGESCREEN:
        pRetVal = new CUIRegionImageScreen();
        break;
    default:
        break;
    }

    if (NULL != pRetVal)
    {
        // 设置对应的Element类型
        pRetVal->SetElementType(eType);
        pRetVal->Init();
    }
    else
    {
        chWARNING2(false, "xXMLBrowserUIFrame::CreateUIByType error");
    }

    return pRetVal;
}

bool xXMLBrowserUIFrame::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (NULL != m_pCurrentRegion)
    {
        return m_pCurrentRegion->RegionBaseKeypadHook(nKeyCode, bPress);
    }

    return false;
}

bool xXMLBrowserUIFrame::OnSoftkeyPress(int nKeyIndex)
{
    bool bHandled = false;
    if (NULL != m_pCurrentRegion)
    {
        const VecSoftkeyParam & refSoftkeyData = GetCurrentUISoftkey();
        if (nKeyIndex < 0
                || nKeyIndex >= refSoftkeyData.size())
        {
            return false;
        }

        const SoftkeyParam & refParam = refSoftkeyData[nKeyIndex];
        OnSoftkeyBtnClick(refParam);
        bHandled = true;
    }

    return bHandled;
}

CUIRegionBase* xXMLBrowserUIFrame::GetUIByElementID(int nElementID)
{
    CUIRegionBase* pRegionUI = NULL;
    // 解绑旧界面资源
    VEC_UIREGION::iterator it = m_ArrpRegion.begin();
    for (; it != m_ArrpRegion.end(); ++it)
    {
        CUIRegionBase* pRegion = *it;
        if (NULL != pRegion && pRegion->GetElementID() == nElementID)
        {
            pRegionUI = pRegion;
            break;
        }
    }
    return pRegionUI;
}

void xXMLBrowserUIFrame::RefreshUI()
{
    if (NULL == m_pCurrentRegion)
    {
        return;
    }
    m_pCurrentRegion->UpdateContent();
}

void xXMLBrowserUIFrame::ExitXmlUI(int nElementID)
{
    // 解绑旧界面资源
    for (int i = 0; i < m_ArrpRegion.size(); ++i)
    {
        CUIRegionBase* pRegion = m_ArrpRegion.at(i);
        if (NULL != pRegion && pRegion->GetElementID() == nElementID)
        {
            if (pRegion == m_pCurrentRegion)
            {
                // 断开相关的信号槽
                SLOT_DISCONNECT(pRegion, m_signalAction, this, xXMLBrowserUIFrame::OnRegionAction);
                m_pCurrentRegion = NULL;
            }

            popupChildView(pRegion);
            // 释放Region资源
            delete pRegion;
            pRegion = NULL;
            m_ArrpRegion.removeAt(i);
            break;
        }
    }
}

#endif
