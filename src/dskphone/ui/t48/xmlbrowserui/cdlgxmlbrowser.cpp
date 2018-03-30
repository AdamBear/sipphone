#include "cdlgxmlbrowser.h"
#include "xmlbrowseruiframe.h"
#include "uimanager/uimanager_inc.h"
#include "uikernel/qsoftkeybar.h"
#include "qtcommon/qmisc.h"
#include "mainwnd/mainwnd.h"
#include "uikernel/languagehelper.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "uicommon.h"
#include "include/uicommon.h"
#include "globalinputhook/modinputhook.h"
#include "extended/xmlbrowser/include/logicanduicommon.h"
#include "uimanager/rectdefine.h"
#include "uikernel/qwidgetutility.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "commonapi/stringhelperapi.h"

namespace
{
#define XB_DIALOG_DEFALUT_LEFT_MARGIN 4
#define XB_DIALOG_DEFALUT_TOP_MARGIN 4
#define XB_EDGE_TRANSPARENT_HORIZON_MARGIN 8 //因图片边缘有阴影效果所以边缘要预留空白(T48)
#define XB_EDGE_TRANSPARENT_VERTICAL_MARGIN 9
}

/************************************************************************/
/*                             about CDlgXMLBrowser                     */
/************************************************************************/
CDlgXMLBrowser::CDlgXMLBrowser(QWidget * parent/* = 0*/, Qt::WFlags flags/* = 0*/)
    : CBaseDialog(parent)
    , m_pXBUIFrame(NULL)
{
    SetTopWnd();

    SetDlgGeometry(RECT_XML_BROWSER_DIALOG);

    //设置不返回Idle,退出由自己的定时器决定
    SetRejectReturnToIdle();

    InitData();
}

CDlgXMLBrowser::~CDlgXMLBrowser()
{
    //
}

// 子界面响应Home按钮的操作
void CDlgXMLBrowser::OnHomeButtonClick()
{
    xmlbrowser_OnAction(-1, XB_ACTION_EXIT_ALL);
    CBaseDialog::OnHomeButtonClick();
}

bool CDlgXMLBrowser::IsShowBackButton()
{
    // 全屏状态下不显示该按钮
    return (m_eWndType != eWT_FullScreen);
}

bool CDlgXMLBrowser::IsShowHomeButton()
{
    // 全屏状态下不显示该按钮
    return (m_eWndType != eWT_FullScreen);
}

void CDlgXMLBrowser::OnKeyboardShow()
{
    if (m_pXBUIFrame != NULL)
    {
        m_pXBUIFrame->OnKeyboardShow();
    }
}

void CDlgXMLBrowser::OnKeyboardHide()
{
    if (m_pXBUIFrame != NULL)
    {
        m_pXBUIFrame->OnKeyboardHide();
    }
}

// 初始化
void CDlgXMLBrowser::Init()
{
    // 注册按键回调函数
    inputHook_RegisterKeyPadHookProc(XMLKeypadHookCallbackFun, HOOK_MODULE_XMLBROWSER);
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgXMLBrowser::Uninit()
{
    // 注销按键回调函数
    inputHook_UnRegisterKeyPadHookProc(XMLKeypadHookCallbackFun);
}

// 设置小窗口数据
void CDlgXMLBrowser::SetData(void * pData/* = NULL*/)
{
    if (NULL == pData)
    {
        return;
    }

    // 判断是否全屏显示
    bool bFullScreen = false;
    XMLLogicToUIData * pToUIData = (XMLLogicToUIData *)pData;
    if ((XMT_IMAGESCREEN == pToUIData->eElementType
            || XMT_IMAGEMENU == pToUIData->eElementType)
            && NULL != pToUIData->pElementData)
    {
        XBImageScreenData * pImageScreenData = (XBImageScreenData *)pToUIData->pElementData;
        bFullScreen = (IMAGE_MODE_FULLSCREEN == pImageScreenData->stAttr.eMode);
    }

    // 设置小窗口的风格
    m_eWndType = bFullScreen ? eWT_FullScreen : eWT_XMLBrower ;

    // 全屏重新设置区域
    if (bFullScreen)
    {
        SetDlgGeometry(RECT_FULL_SCREEN);
    }
    else
    {
        SetDlgGeometry(RECT_XML_BROWSER_DIALOG);
    }

    SetBackground();
}

// 设置背景
void CDlgXMLBrowser::SetBackground()
{
    //设置背景
    this->setAutoFillBackground(true);
    QPalette palette = this->palette();
    QPixmap pixBg = THEME_GET_BMP(eWT_FullScreen == m_eWndType ?
                                  "" :
                                  PIC_GENERAL_TALKBACKGROUND);
    QSize scaleSize = (eWT_FullScreen == m_eWndType) ? QSize(PHONE_WIDTH,
                      PHONE_HEIGHT) : RECT_XML_BROWSER_DIALOG.size();
    palette.setBrush(QPalette::Window, QBrush(pixBg.scaled(scaleSize)));

    this->setPalette(palette);
}

bool CDlgXMLBrowser::IsShowSoftkey()
{
    return (m_eWndType == eWT_XMLBrower);
}

bool CDlgXMLBrowser::IsShowTitle(TitleLayoutInfo & objInfo)
{
    return (m_eWndType == eWT_XMLBrower);
}

// 返回该窗口指针
CBaseDialog * CDlgXMLBrowser::CreateInstance()
{
    return new CDlgXMLBrowser();
}

// 更新界面
void CDlgXMLBrowser::UpdateUI(const XMLLogicToUIData & objData)
{
    XMLBROWSERUI_INFO("CDlgXMLBrowser::UpdateUI");
    if (NULL != m_pXBUIFrame)
    {
        m_pXBUIFrame->UpdateUI(objData);

        UpdateSoftkey();
    }
}

void CDlgXMLBrowser::UpdateSoftkey()
{
    XMLBROWSERUI_INFO("CDlgXMLBrowser::UpdateSoftkey");
    if (NULL != m_pXBUIFrame)
    {

        VecSoftkeyParam vecSoft = m_pXBUIFrame->GetCurrentUISoftkey();
        UpdateSoftkey(vecSoft);
    }
}

// 更新softkey数据
void CDlgXMLBrowser::UpdateSoftkey(VecSoftkeyParam & refSoftkeyData)
{
    XMLBROWSERUI_INFO("CDlgXMLBrowser::UpdateSoftkey :%d", refSoftkeyData.size());
    // 清空当前softkey数据
    m_SoftkeyData.clear();

    CSoftKeyBarData tempData;
    for (int i = 0; i != refSoftkeyData.size(); ++i)
    {
        const SoftkeyParam & softParam = refSoftkeyData[i];

        // softkey more由UI控件处理
        if (softParam.eType == XB_ST_MORE)
        {
            continue;
        }
        // 设置显示名
        tempData.m_strSoftkeyTitle  = LANG_TRANSLATE(toQString(softParam.strName.c_str()).toUtf8().data());

        // 设置softkey的Action
        tempData.m_strSoftkeyAction = ToUISoftkeyAction(softParam);

        tempData.m_strIconPath = SoftkeyTypeToPicPath(softParam.eType);

        tempData.m_pReceiver = this;

        tempData.m_strSlotName = SLOT(OnSoftKeyAction(qSoftButton *));

        m_SoftkeyData.push_back(tempData);
    }

    FilterIMESoftkey();

    // 设置softkey数据
    _MainWND.DrawSoftkey(this);

}

void CDlgXMLBrowser::FilterIMESoftkey()
{
    QWidget * pWgt = focusWidget();
    bool bShowIme = (NULL != pWgt && (pWgt->inherits("QLineEdit") || pWgt->inherits("QTextEdit"))
                     && pWgt->isEnabled() && !(((QLineEdit *)pWgt)->isReadOnly()));

    CArraySoftKeyBarData::iterator iter = m_SoftkeyData.begin();
    while (iter != m_SoftkeyData.end())
    {
        if (iter->m_strSoftkeyAction.empty())
        {
            iter = m_SoftkeyData.erase(iter);
            continue;
        }

        if (!bShowIme && (iter->m_strSoftkeyAction == IME_ACTION
                          || iter->m_strSoftkeyAction == IME_DEL_ACTION //这一句判断是无效的
                          || (iter->m_strSoftkeyAction).find("SoftKey:BackSpace") != yl::string::npos))
        {
            iter = m_SoftkeyData.erase(iter);
            continue;
        }
        ++iter;
    }
}

// 初始化数据
void CDlgXMLBrowser::InitData()
{
    m_pXBUIFrame = new CXMLBrowserUIFrame(this);
    if (NULL != m_pXBUIFrame)
    {
        // 设置控件位置
        m_pXBUIFrame->setGeometry(XB_DIALOG_DEFALUT_LEFT_MARGIN, XB_DIALOG_DEFALUT_TOP_MARGIN,
                                  RECT_XMLBrowser.width(), RECT_XMLBrowser.height());

        //设置背景
        SetBackground();

        // 连接相应的信号和槽
        connect(m_pXBUIFrame, SIGNAL(QuitFrame()), this, SLOT(OnXBFrameQuit()));
        connect(m_pXBUIFrame, SIGNAL(ChangeSoftkey()), this, SLOT(UpdateSoftkey()));
    }

    // 绘制背景图片
    m_pmBG = THEME_GET_BMP(PIC_THEME1_OTHER_BACKGROUND);

    QPixmap pmBtnBg = THEME_GET_BMP(PIC_DSS_BTN_NORMAL);
    QPainter painter(&m_pmBG);
    int i = 0;
    for (i = 0 ; i < 6; ++i)
    {
        painter.drawPixmap(0, 50 + 57 * i, pmBtnBg);
    }

    for (i = 0; i < 6; ++i)
    {
        painter.drawPixmap(644, 50 + 57 * i, pmBtnBg);
    }
}

bool CDlgXMLBrowser::OnAction(const QString & strAction)
{
    if (ACTION_BACK == strAction && NULL != m_pXBUIFrame)
    {
        m_pXBUIFrame->OnSoftkeyBtnClick(TypeToSoftInfo(XB_ST_EXIT));
    }
    return true;
}

void CDlgXMLBrowser::OnSoftKeyAction(qSoftButton * pSoftBtn)
{
    XMLBROWSERUI_INFO("CDlgXMLBrowser::OnSoftKeyAction");
    if (NULL != m_pXBUIFrame && NULL != pSoftBtn)
    {
        m_pXBUIFrame->OnSoftkeyBtnClick(ToLogicSoftkeyAction(pSoftBtn->m_strButtonName.toUtf8().data()));
    }
}

void CDlgXMLBrowser::OnXBFrameQuit()
{
    xmlbrowser_Exit();
    UIManager_PutPrivateDlg(this);
}

bool CDlgXMLBrowser::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (m_pXBUIFrame != NULL)
    {
        return m_pXBUIFrame->eventFilter(pObject, pEvent);
    }
    return false;
}

bool CDlgXMLBrowser::MyKeypadHook(int nKeyCode, bool bPress)
{
    if (m_pXBUIFrame != NULL)
    {
        return m_pXBUIFrame->MyKeypadHook(nKeyCode, bPress);
    }
    return false;
}

bool XMLKeypadHookCallbackFun(int nKeyCode, bool bPress)
{
    CDlgXMLBrowser * pDlgXMLBrowser = qobject_cast<CDlgXMLBrowser *>(UIManager_GetPrivateDlg(
                                          DLG_CDlgXMLBrowser));
    if (NULL != pDlgXMLBrowser && UIManager_GetTopWindow() == pDlgXMLBrowser)
    {
        return pDlgXMLBrowser->MyKeypadHook(nKeyCode, bPress);
    }
    return false;
}

// 重写大小改变事件
void CDlgXMLBrowser::resizeEvent(QResizeEvent * pEvent)
{
    // 同步Frame和Dlg的大小
    if (NULL != m_pXBUIFrame)
    {
        QSize objSize = pEvent->size();
        XMLBROWSERUI_INFO("size(%d, %d)", objSize.width(), objSize.height());
        if (eWT_FullScreen == m_eWndType)
        {
            m_pXBUIFrame->setGeometry(0, 0, objSize.width(), objSize.height());
        }
        else
        {
            m_pXBUIFrame->setGeometry(XB_DIALOG_DEFALUT_LEFT_MARGIN,
                                      XB_DIALOG_DEFALUT_TOP_MARGIN,
                                      objSize.width() - XB_EDGE_TRANSPARENT_HORIZON_MARGIN,
                                      objSize.height() - XB_EDGE_TRANSPARENT_VERTICAL_MARGIN);
        }
    }
}

yl::string CDlgXMLBrowser::ToUISoftkeyAction(const SoftkeyParam & objParam)
{
    if (objParam.eType == XB_ST_CHANGEMODE)
    {
        return IME_ACTION;
    }
    else
    {
        // URI为空，表示Action为空，不应返回0_，，这样Softkey会显示，导致Bug：http://10.2.1.199/Bug.php?BugID=48126
        if (objParam.strURI.empty())
        {
            return "";
        }
        else
        {
            return commonAPI_FormatString("%d_%s", (int)objParam.eType, objParam.strURI.c_str());
        }
    }
}

SoftkeyParam CDlgXMLBrowser::ToLogicSoftkeyAction(const yl::string & strUIAction)
{
    SoftkeyParam tempParam;

    if (IME_ACTION == strUIAction)
    {
        tempParam.eType = XB_ST_CHANGEMODE;
        tempParam.strURI = "";
    }
    else
    {
        int iPos = strUIAction.find_first_of('_');
        if (iPos != yl::string::npos)
        {
            tempParam.eType = (XB_SOFTKEY_TYPE)atoi(strUIAction.substr(0, iPos).c_str());
            tempParam.strURI = strUIAction.substr(iPos + 1, strUIAction.length() - 1);
        }
    }

    return tempParam;
}

QPixmap CDlgXMLBrowser::GetCustomBackGround()
{
    if (m_eWndType == eWT_XMLBrower)
    {
        return m_pmBG;
    }

    // 绘制背景图片
    QPixmap pmWithnotTitle = THEME_GET_BMP(PIC_THEME1_OTHER_BACKGROUND_WITHNOT_TITLE);

    QPixmap pmBtnBg = THEME_GET_BMP(PIC_DSS_BTN_NORMAL);
    QPainter painter(&pmWithnotTitle);
    int i = 0;
    for (i = 0 ; i < 6; ++i)
    {
        painter.drawPixmap(0, 50 + 57 * i, pmBtnBg);
    }

    for (i = 0; i < 6; ++i)
    {
        painter.drawPixmap(644, 50 + 57 * i, pmBtnBg);
    }

    return pmWithnotTitle;
}

bool CDlgXMLBrowser::ProcessDsskey(int iIndex)
{
    if (NULL != m_pXBUIFrame)
    {
        return m_pXBUIFrame->ProcessDsskey(iIndex);
    }
    return false;
}
