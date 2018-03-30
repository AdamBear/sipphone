#include "xmlbrowseruiframe.h"
#include "cuiregionbase.h"
#include <QEvent>
#include <QLineEdit>
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "inputmethod/inputmethod.h"
#include "mainwnd/mainwnd.h"
#include "cuiregiontextscreen.h"
#include "cuiregioninput.h"
#include "cuiregiontextmenu.h"
#include "formattedtextscreen.h"
#include "cuiregionimagescreen.h"
#include "cuiregiontextstatus.h"
#include "cuiregiondirectory.h"
//#include "VoiceChannel/modVoiceChannel.h"
//#include "ActionUR/modActionUR.h"
//#include "Define/Common.h"
//#include "UIManager/UIManager_common.h"
#include "uimanager/moduimanager.h"

CXMLBrowserUIFrame::CXMLBrowserUIFrame(QWidget * parent/* = 0*/)
    : QFrame(parent)
    , m_pCurrentRegion(NULL)
    , m_ePageType(XB_UI_TYPE_DLG)
    , m_iTimeOut(0)
{
    InitData();
}

CXMLBrowserUIFrame::~CXMLBrowserUIFrame()
{
    // 清空界面的数据
    ClearData();
}

void CXMLBrowserUIFrame::InitData()
{
    setObjectName("XMLBrowserUIFrame");
    connect(&m_quitTimer, SIGNAL(timeout()), this, SLOT(OnQuitTimerTimeOut()));
}

// 更新界面
void CXMLBrowserUIFrame::UpdateUI(const XMLLogicToUIData & objData)
{
    XMLBROWSERUI_INFO("CXMLBrowserUIFrame::UpdateUI");
    // 清空原有的界面数据
    ClearData();

    // 退出界面的时间
    int iQuitTime = 0;

    CUIRegionBase * pRegionBase = CreateUIByType(objData.eElementType);
    if (NULL != pRegionBase)
    {
        // 设置父窗口
        pRegionBase->setParent(this);

        // 设置Region的位置
        pRegionBase->setGeometry(0, 0, this->width(), this->height());

        // 连接相关的信号槽
        connect(pRegionBase, SIGNAL(RegionAction(REGION_ACTION_TYPE)), this,
                SLOT(OnRegionAction(REGION_ACTION_TYPE)));

        // 刷新Region的数据
        pRegionBase->SetElementID(objData.nElementID);
        pRegionBase->Update(objData.pElementData);
        pRegionBase->show();

        // 取所有Region中最长退出时间为Frame的退出时间
        int iConfigQuitTime = pRegionBase->GetQuitTime();
        iQuitTime = max(iQuitTime, iConfigQuitTime);

        // 将Region的指针插入链表
        m_ArrpRegion.push_back(pRegionBase);
    }

    // 若Region为空则不作处理
    if (m_ArrpRegion.empty())
    {
        return;
    }

    if (m_ArrpRegion.size() > 0)
    {
        m_pCurrentRegion = m_ArrpRegion[0];
    }

    // 启动计时
    XMLBROWSERUI_INFO("XMLBrowser Quit Time(%d)", iQuitTime);
    ReSetTimer(iQuitTime);
}

VecSoftkeyParam & CXMLBrowserUIFrame::GetCurrentUISoftkey()
{
    if (NULL != m_pCurrentRegion)
    {
        return m_pCurrentRegion->GetSoftkey();
    }

    static VecSoftkeyParam vecSoft;
    return vecSoft;
}

// 当softkey点击时调用的函数
void CXMLBrowserUIFrame::OnSoftkeyBtnClick(const SoftkeyParam & objParam)
{
    if (NULL == m_pCurrentRegion)
    {
        return;
    }

    XMLBROWSERUI_INFO("CXMLBrowserUIFrame::OnSoftkeyBtnClick eSoftkeyType(%d)", objParam.eType);

    int iElementID = m_pCurrentRegion->GetElementID();
    XMLBROWSERUI_INFO("CXMLBrowserUIFrame::OnSoftkeyBtnClick GetElementID(%d)", iElementID);

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
                iSelectItem = m_pCurrentRegion->GetCurrentItem();
            }
            xmlbrowser_OnAction(iElementID, XB_ACTION_SELECT, (void *)&iSelectItem);
        }
        break;
    case XB_ST_DIAL:
        {
            int iSelectItem = -1;
            if (NULL != m_pCurrentRegion)
            {
                iSelectItem = m_pCurrentRegion->GetCurrentItem();
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
        {
            OnClickSoftKeyDot();
        }
        break;
    case XB_ST_BACKSPACE:
        {
            OnClickSoftKeyBackSpace();
        }
        break;
    case XB_ST_NEXTSPACE:
        {
            OnClickSoftKeyNextSpace();
        }
        break;
    case XB_ST_CHANGEMODE:
        {
            OnClickSoftKeyChangeIME();
        }
        break;
    case XB_ST_FUNCTION:
        {
            if (NULL != m_pCurrentRegion
                    && (m_pCurrentRegion->GetElementType() == XMT_MENU
                        || m_pCurrentRegion->GetElementType() == XMT_INPUT))
            {
                // 处理SoftKey按键
                ActionSoftkeyData data;
                data.iFocusedItem = m_pCurrentRegion->GetCurrentItem();
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
            if (NULL != m_pCurrentRegion)
            {
                // 处理SoftKey按键
                ActionSoftkeyData data;
                data.iFocusedItem = m_pCurrentRegion->GetCurrentItem();
                data.strURI = objParam.strURI;
                xmlbrowser_OnAction(m_pCurrentRegion->GetElementID(), XB_ACTION_SOFTKEY, (void *)&data);
            }

        }
        break;
    }
}

// 清空界面数据
void CXMLBrowserUIFrame::ClearData()
{
    // 释放本地资源
    std::vector<CUIRegionBase *>::iterator it = m_ArrpRegion.begin();
    for (; it != m_ArrpRegion.end(); ++it)
    {
        CUIRegionBase * pRegion = *it;
        if (NULL != pRegion)
        {
            // 断开相关的信号槽
            disconnect(pRegion, SIGNAL(RegionAction(REGION_ACTION_TYPE)), this,
                       SLOT(OnRegionAction(REGION_ACTION_TYPE)));

            // 设置父控件为空
            pRegion->setParent(0);

            // 释放Region资源
            pRegion->deleteLater();
            pRegion = NULL;
        }
    }
    m_ArrpRegion.clear();

    m_pCurrentRegion = NULL;
}

// 启动计时器
void CXMLBrowserUIFrame::RestartTimer(int iTime)
{
    XMLBROWSERUI_INFO("CXMLBrowserUIFrame::RestartTimer(%d)", iTime);
    if (iTime > 0)
    {
        // 先停止计时
        StopTimer();

        // 重新启动计时
        m_quitTimer.start(iTime * 1000);
    }
}

// 停止计时器
void CXMLBrowserUIFrame::StopTimer()
{
    m_quitTimer.stop();
}

// 重新设置计时器
void CXMLBrowserUIFrame::ReSetTimer(int iTime)
{
    if (iTime > 0)
    {
        m_iTimeOut = iTime;
        RestartTimer(m_iTimeOut);
    }
}

void CXMLBrowserUIFrame::OnClickSoftKeyBackSpace()
{
    QWidget * pItem = this->focusWidget();
    QLineEdit * pLineEdit = qobject_cast<QLineEdit *>(pItem);
    if (pLineEdit != NULL && !pLineEdit->isReadOnly())
    {
        qOnBackspaceClicked(this->focusWidget());
    }
}

void CXMLBrowserUIFrame::OnClickSoftKeyDot()
{
    QWidget * pItem = this->focusWidget();
    QLineEdit * pLineEdit = qobject_cast<QLineEdit *>(pItem);
    if (pLineEdit != NULL && !pLineEdit->isReadOnly())
    {
        pLineEdit->insert(".");
    }
}

void CXMLBrowserUIFrame::OnClickSoftKeyNextSpace()
{
    QWidget * pItem = this->focusWidget();
    QLineEdit * pLineEdit = qobject_cast<QLineEdit *>(pItem);
    if (pLineEdit != NULL && !pLineEdit->isReadOnly())
    {
        pLineEdit->insert(" ");
    }
}

void CXMLBrowserUIFrame::OnClickSoftKeyChangeIME()
{
    _MainWND.SwitchNextIMEName(this->focusWidget());
}

void CXMLBrowserUIFrame::OnClickSoftKeySubmit()
{
    if (NULL != m_pCurrentRegion)
    {
        // 获取当前界面输入的数据
        //std::map<int, std::string> objMap = m_pCurrentRegion->GetPageData();
        YLHashMap<int, yl::string> objMap;
        bool bLegalData = m_pCurrentRegion->GetPageData(objMap);
        if (bLegalData)
        {
            //submit和按ok键confirm处理相同
            xmlbrowser_OnAction(m_pCurrentRegion->GetElementID(), XB_ACTION_CONFIRM, (void *)&objMap);
        }
    }
}

void CXMLBrowserUIFrame::OnRegionAction(REGION_ACTION_TYPE eAction)
{
    if (NULL == m_pCurrentRegion)
    {
        return ;
    }

    switch (eAction)
    {
    case REGION_ACTION_CLICK_ITEM:
        {
            int iCurrentItem = m_pCurrentRegion->GetCurrentItem();
            xmlbrowser_OnAction(m_pCurrentRegion->GetElementID(), XB_ACTION_CONFIRM, (void *)&iCurrentItem);
        }
        break;
    case  REGION_ACTION_CURRENT_ITEM_CHANGE:
        {
            int iCurrentItem = m_pCurrentRegion->GetCurrentItem();
            XMLBROWSERUI_INFO("iCurrentItem(%d)", iCurrentItem);
            xmlbrowser_OnAction(m_pCurrentRegion->GetElementID(), XB_ACTION_FOCUSCHANGE, (void *)&iCurrentItem);

            // 发射消息更新softkey
            emit ChangeSoftkey();
        }
        break;
    default:
        break;
    }
}

void CXMLBrowserUIFrame::OnQuitTimerTimeOut()
{
    // 停止计时器
    StopTimer();

    emit QuitFrame();
}

// 根据类型创建ElementUI
CUIRegionBase * CXMLBrowserUIFrame::CreateUIByType(XML_ELEMENT_TYPE eType)
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
        pRetVal = new CUIRegionInput();
        break;
    case XMT_DIRECTORY:
        pRetVal = new CUIRegionDirectory();
        break;
    case XMT_FORMATTEDTEXTSCREEN:
        pRetVal = new CDlgFormattedTextScreen();
        break;
    //  case XMT_EXECUTE: pRetVal = new ; break;
    case XMT_STATUS:
        pRetVal = new CUIRegionTextStatus;
        break;
    //  case XMT_CONFIGURATION: pRetVal = new ; break;
    case XMT_IMAGEMENU:
    case XMT_IMAGESCREEN:
        pRetVal = new CUIRegionImageScreen();
        break;
    default:
        break;
    }

    if (pRetVal != NULL)
    {
        // 设置对应的Element类型
        pRetVal->SetElementType(eType);
    }
    else
    {
        chWARNING2(false, "CXMLBrowserUIFrame::CreateUIByType error");
    }

    return pRetVal;
}

bool CXMLBrowserUIFrame::MyKeypadHook(int nKeyCode, bool bPress)
{
    CBaseDialog * pTopWnd = UIManager_GetTopWindow();
    if (NULL == pTopWnd)
    {
        return false;
    }
    else
    {
        // 顶层窗口非XMLBrowser，不处理勾到的消息
        if (!pTopWnd->inherits(DLG_CDlgXMLBrowser))
        {
            return false;
        }
    }

    if (m_pCurrentRegion != NULL)
    {
        return m_pCurrentRegion->RegionBaseKeypadHook(nKeyCode, bPress);
    }

    return false;
}

// 重写事件过滤器
bool CXMLBrowserUIFrame::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pEvent->type() == QEvent::MouseButtonPress
            || pEvent->type() == QEvent::KeyPress)
    {
        // 有按键或者鼠标点击事件
        // 重置超时定时器
        RestartTimer(m_iTimeOut);
    }

    // 将事件传递给当前的Region
    if (m_pCurrentRegion != NULL)
    {
        return m_pCurrentRegion->eventFilter(pObject, pEvent);
    }

    return false;
}

bool CXMLBrowserUIFrame::ProcessDsskey(int iIndex)
{
    if (NULL != m_pCurrentRegion)
    {
        return m_pCurrentRegion->ProcessDsskey(iIndex);
    }
    return false;
}

void CXMLBrowserUIFrame::OnKeyboardShow()
{
    std::vector<CUIRegionBase*>::iterator it = m_ArrpRegion.begin();
    for (; it != m_ArrpRegion.end(); ++it)
    {
        CUIRegionBase* pRegion = *it;
        if (NULL != pRegion)
        {
            pRegion->OnKeyboardShow();
        }
    }
}

void CXMLBrowserUIFrame::OnKeyboardHide()
{
    std::vector<CUIRegionBase*>::iterator it = m_ArrpRegion.begin();
    for (; it != m_ArrpRegion.end(); ++it)
    {
        CUIRegionBase* pRegion = *it;
        if (NULL != pRegion)
        {
            pRegion->OnKeyboardHide();
        }
    }
}
