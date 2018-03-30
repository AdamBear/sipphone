#include "settingitembase.h"
#include "setting/include/common.h"
#include "qtcommon/qmisc.h"
#include <QApplication>
#include <QStylePainter>
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/ilistitemdelegate.h"
#include "uikernel/qwidgetutility.h"
#include "customtheme/modcustomtheme.h"
#include "uimanager/moduimanager.h"
#include "inputmethod/inputmethod.h"
#include "qtcommon/xmlhelper.h"

#include "settingui/src/cdlgbasesubpage.h"

#define RADIO_ITEM_CHANGE_ICON_ON_FOCUS 1

namespace
{
#define SETTING_ITEM_LABEL_HEIGHT 24
// #define SETTING_ITEM_LABEL_WIDTH 284
// #define DEFAULT_SETTING_ITEM_TEXT_WGT_SPACING 10 //label和wgt的间距
#define SETTING_WIGET_DEFAUT_X 205  //内容控件相对item的x坐标
#define SETTING_ITEM_HEIGHT 47  //item的高度
#define DEFAULT_MENU_ENTER_POS_RIGHT 36
#define DEFAULT_MENU_ENTER_POS_WIDTH 13
#define DEFAULT_MENU_ENTER_POS_TOP 15
#define DEFAULT_MENU_ENTER_POS_HEIGHT 17
#define MENU_ICON_HEIGHT 26
#define MENU_ICON_WIDTH 29
// #define SETTING_MENU_ITEM_LABEL_WIDTH (SETTING_ITEM_LABEL_WIDTH - MENU_ICON_WIDTH -5)
// #define QWIDGETPAIRWIDTH (DEFAULT_MENU_ENTER_POS_RIGHT + DEFAULT_MENU_ENTER_POS_WIDTH + SETTING_WIGET_DEFAUT_X)  //QWidgetPair宽度 = item_length - QWIDGETPAIRWIDTH
#define SETTING_ITEM_WGT_WIDTH 246
#define DEFAULT_COLUMN_SAPCING 10
#define DEFAULT_ICON_LABEL_SAPCING 5
// #define SUBPAGE_PAGE_ITEM_COUNT 7
#define SLIDER_BAR_WIDTH 53
#define MENU_SWITCH_ARROW_SPACINE 12
#define MENU_SWITCH_WGT_WIDTH 200
#define DIRECTORY_ICON_POS_LEFT_SPACING 10
#define DIRECTORY_ICON_POS_HIGH_SPACING 9
#define DIRECTORY_ICON_POS_WIDTH 27
#define DIRECTORY_ICON_POS_HEIGHT 30
#define DIRECTORY_ICON_TOUCH_WIDTH 50
#define DIRECTORY_ICON_TOUCH_HEIGHT 50


const QColor SETTING_ITEM_NORMAL_COLOR = Qt::black;
const QColor SETTING_ITEM_PRESS_COLOR = Qt::white;
const QColor SETTING_ITEM_SELECT_COLOR = Qt::white;
static QSize RADIO_SIZE(20, 20);

#define DEFAULT_SETTING_ITEM_FONT_SIZE 16
}

/*****************************************Class  CSettingItem***************************************/
CSettingItem::CSettingItem(QWidget * parent, QString strLabel, QWidget * pWgt, QString strAction,
                           int iDataIndex, bool bAddInex/* = true*/, bool bAddMark/* = false*/, int iExtraButten/* = NONE_EXTRA*/)
    : CListItem(LIST_ITEM_TYPE_SETTING, parent)
    , m_strLabel(strLabel)
    , m_strAction(strAction)
    , m_bAddIndex(bAddInex)
    , m_bAddMark(bAddMark)
    , m_iExtraButten(iExtraButten)
    , m_btnExtra(this)
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef  QT_TOUCH_SUPPORT
    , m_bChangeStyleOnPress(true)
#endif
    , m_bChangeStyleOnSelect(true)
#endif
    , m_bConfiged(false)
{
    qWidgetUtility::setFontStyle(this, DEFAULT_SETTING_ITEM_FONT_SIZE, QFont::Normal, false);
    SetDataIndex(iDataIndex);
    Init();

    GenerateLabelText();

    m_pWgt = pWgt;
    if (NULL != m_pWgt)
    {
        m_pWgt->setParent(this);
        m_pWgt->installEventFilter(this);
    }

}

CSettingItem::CSettingItem(QWidget * parent, QWidgetPair wgtPair, bool bAddInex/* = true*/)
    : CListItem(LIST_ITEM_TYPE_SETTING, parent)
    , m_strLabel(wgtPair.first)
    , m_strAction(wgtPair.third)
    , m_bAddIndex(bAddInex && wgtPair.bAddIndex)
    , m_bAddMark(wgtPair.bAddMark)
    , m_iExtraButten(wgtPair.iExtraButten)
    , m_btnExtra(this)
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef  QT_TOUCH_SUPPORT
    , m_bChangeStyleOnPress(false)
#endif
    , m_bChangeStyleOnSelect(wgtPair.bChangeStyleOnSelect)
#endif
    , m_bConfiged(false)
{
    qWidgetUtility::setFontStyle(this, DEFAULT_SETTING_ITEM_FONT_SIZE);
    SetDataIndex(wgtPair.iIndex);
    Init();

    GenerateLabelText();

    m_pWgt = wgtPair.second;
    if (NULL != m_pWgt)
    {
        m_pWgt->setParent(this);
        m_pWgt->installEventFilter(this);
    }

}

CSettingItem::CSettingItem(QWidget * parent /*= NULL*/, IListItemAction * pAction /*= NULL*/)
    : CListItem(LIST_ITEM_TYPE_SETTING, parent, pAction)
    , m_bAddIndex(true)
    , m_bAddMark(false)
    , m_btnExtra(this)
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef  QT_TOUCH_SUPPORT
    , m_bChangeStyleOnPress(true)
#endif
    , m_bChangeStyleOnSelect(true)
#endif
    , m_bConfiged(false)
{

    qWidgetUtility::setFontStyle(this, DEFAULT_SETTING_ITEM_FONT_SIZE);
    m_pLabDisplay = NULL;
    m_pWgt = NULL;
    Init();
}

CSettingItem::~CSettingItem()
{
    m_pWgt = NULL;
}

void CSettingItem::Init()
{

    m_pLabDisplay = new QLabel(this);

    const QColor color = CustomTheme_GetCustomFontColorByKey(CUSTOM_FONT_ID_SETTING_NORMAL);

    if (NULL != m_pLabDisplay)
    {
        m_pLabDisplay->setObjectName("LabDisplay");
        QPalette palLabDisp = m_pLabDisplay->palette();
        palLabDisp.setColor(QPalette::Text, color);
        m_pLabDisplay->setPalette(palLabDisp);
    }

    QPalette pal = this->palette();
    pal.setColor(QPalette::Text, color);
    pal.setBrush(QPalette::WindowText, QBrush(color));
    this->setPalette(pal);
    setObjectName("CSettingItem");
}

// 设置控件
void CSettingItem::SetItemEnabled(bool bEnabled)
{
    if (NULL != m_pWgt)
    {
        m_pWgt->setEnabled(bEnabled);
    }
}

bool CSettingItem::IsItemEnabled()
{
    if (NULL != m_pWgt)
    {
        return m_pWgt->isEnabled();
    }

    return false;
}

// 清除内容
void CSettingItem::ClearContent()
{
    if (m_pLabDisplay)
    {
        m_pLabDisplay->setText("");
    }

    m_strLabel = "";
    m_strAction = "";

    m_pWgt = NULL;
    CListItem::ClearContent();
}

void CSettingItem::Relayout()
{
    QRect rcItem = rect();
    int nLeftBegin = 0;
    int nRightEnd = 0;

    if (NULL != m_pDelegate)
    {
        m_pDelegate->GetContentPos(nLeftBegin, nRightEnd);
    }

    if (m_pLabDisplay)
    {
        int iWidth = rcItem.right() - nRightEnd - nLeftBegin;
        if (NULL != m_pWgt)
        {
            iWidth = SETTING_WIGET_DEFAUT_X - nLeftBegin - DEFAULT_COLUMN_SAPCING;
        }
        m_pLabDisplay->setGeometry(nLeftBegin, (SETTING_ITEM_HEIGHT - SETTING_ITEM_LABEL_HEIGHT) / 2
                                   , iWidth, SETTING_ITEM_LABEL_HEIGHT);
    }

    if (NULL != m_pWgt)
    {
        int nLeft = SETTING_WIGET_DEFAUT_X;
#if IF_SETTING_ITEM_WGT_HEIGHT_FIXED
        int nTop = (rcItem.height() - m_pWgt->height()) / 2;
        QRect rcWgt(nLeft, nTop, SETTING_ITEM_WGT_WIDTH, m_pWgt->height());
        // 增加qCheckBox高度，增加触摸面积
        if (m_pWgt->inherits("qCheckBox"))
        {
            rcWgt.setHeight(this->height());
        }
#else
        QRect rcWgt(nLeft, 0, SETTING_ITEM_WGT_WIDTH, this->height());
#endif

        m_pWgt->setGeometry(rcWgt);
        m_pWgt->raise();
        if (m_iExtraButten == DIRECTORY_EXTRA)
        {
            m_iBtnExtraLeft = rcWgt.right() + DIRECTORY_ICON_POS_LEFT_SPACING;
            int nExtraTop = 0 ;
            QRect rcExtra(m_iBtnExtraLeft, nExtraTop, DIRECTORY_ICON_TOUCH_WIDTH, DIRECTORY_ICON_TOUCH_HEIGHT);
            m_btnExtra.SetRect(rcExtra);
        }
    }

}

void CSettingItem::GenerateLabelText()
{
    if (NULL == m_pLabDisplay)
    {
        return;
    }

    QString strText = "";
    if (m_bAddIndex && GetDataIndex() >= 0)
    {
        strText.append(QString::number(GetDataIndex() + 1).append(". "));
    }

    strText.append(m_strLabel);

    if (m_bAddMark)
    {
        strText.append(":");
    }

    m_pLabDisplay->setText(strText);
}

void CSettingItem::SetLabText(QString strText)
{
    if (m_pLabDisplay)
    {
        m_strLabel = strText;
        GenerateLabelText();
    }
}

void CSettingItem::hideEvent(QHideEvent * pEvent)
{
    if (m_pWgt)
    {
        m_pWgt->hide();
    }
}

void CSettingItem::SetFocus(bool bFocus)
{
    CListItem::SetFocus(bFocus);
    if (bFocus)
    {
        if (NULL != m_pWgt && !(m_pWgt->inherits("QLabel")) && m_pWgt->isEnabled())
        {
            if (FullKeyboardEnabled())
            {
                bool bHasHideWnd = false;
                QWidget* pFoucus = (parent() && parent()->inherits("QWidget")) ? ((QWidget*)parent())->focusWidget() : NULL;
                if (NULL != pFoucus && pFoucus != m_pWgt
                        && (pFoucus->inherits("QLineEdit") || pFoucus->inherits("QTextEdit")
                           ))
                {
                    SETTINGUI_INFO("CSettingItem::SetFocus hide keyboard pFoucus %p", pFoucus);
                    qSetIsShowWnd(pFoucus, false);
                    bHasHideWnd = true;
                }

                if (!bHasHideWnd && !(m_pWgt->inherits("QLineEdit") || m_pWgt->inherits("QTextEdit")
                                     ))
                {
                    CBaseDialog* pTopWnd = UIManager_GetTopWindow();
                    const QWidget* pInputWgt = GetCurrInputWgt();
                    if (pInputWgt != NULL && isChildOf(this, pTopWnd)
                            && qGetIsShowWnd())
                    {
                        const QObject *pParent = pInputWgt;
                        while (pParent != NULL)
                        {
                            if (pParent == pTopWnd)
                            {
                                QTimer::singleShot(5, this, SLOT(DelayHideKeyboard()));
                                //qSetIsShowWnd(pInputWgt, false);
                                break;
                            }
                            pParent = pParent->parent();
                        }
                    }
                }

                if (NULL == m_pWgt)
                {
                    return;
                }

                SETTINGUI_INFO("CSettingItem::SetFocus dataindex [%d] bFocus [%d]  wgt type [%s]",
                               GetDataIndex(), bFocus, m_pWgt->metaObject()->className());
            }

            m_pWgt->setFocus();

            if (FullKeyboardEnabled()
                    && (m_pWgt->inherits("QLineEdit") || m_pWgt->inherits("QTextEdit"))
                    && m_pWgt->isVisible()
                    && m_pAction != NULL)
            {
                //底层访问上层的实现可以优化
                if (m_pWgetHost != NULL && m_pWgetHost->inherits("CDlgBaseSubPage"))
                {
                    CDlgBaseSubPage* pSubPage = (CDlgBaseSubPage*)m_pWgetHost;
                    if (pSubPage->isListReady())
                    {
                        SETTINGUI_INFO("CSettingItem::SetFocus show keyboard");
                        qSetIsShowWnd(m_pWgt, true);
                    }
                }
                else
                {
                    SETTINGUI_INFO("CSettingItem::SetFocus show keyboard");
                    qSetIsShowWnd(m_pWgt, true);
                }
            }
        }
        else
        {
            if (FullKeyboardEnabled())
            {
                bool bHasHideWnd = false;
                QWidget* pFoucus = (parent() && parent()->inherits("QWidget")) ? ((QWidget*)parent())->focusWidget() : NULL;
                if (NULL != pFoucus
                        && (pFoucus->inherits("QLineEdit") || pFoucus->inherits("QTextEdit")
                           ))
                {
                    SETTINGUI_INFO("CSettingItem::SetFocus hide keyboard pFoucus %p", pFoucus);
                    qSetIsShowWnd(pFoucus, false);
                    bHasHideWnd = true;
                }

                if (!bHasHideWnd)
                {
                    CBaseDialog* pTopWnd = UIManager_GetTopWindow();
                    const QWidget* pInputWgt = GetCurrInputWgt();
                    if (pInputWgt != NULL && isChildOf(this, pTopWnd)
                            && qGetIsShowWnd())
                    {
                        const QObject *pParent = pInputWgt;
                        while (pParent != NULL)
                        {
                            if (pParent == pTopWnd)
                            {
                                QTimer::singleShot(5, this, SLOT(DelayHideKeyboard()));
                                break;
                            }
                            pParent = pParent->parent();
                        }
                    }
                }
            }

            //将焦点设置过来，防止之前选中行的控件继续保持焦点状态
            this->setFocus();
        }
    }
    else
    {
        clearFocus();

        if (NULL != m_pWgt)
        {
            m_pWgt->clearFocus();
        }
    }
}

bool CSettingItem::IsEditItem()
{
    if (m_pWgt != NULL
            && m_pWgt->isEnabled()
            && (m_pWgt->inherits("QLineEdit") || m_pWgt->inherits("QTextEdit")))
    {
        return true;
    }
    return false;
}

void CSettingItem::SetWgetHost(QWidget* pParent)
{
    m_pWgetHost = pParent;
}

void CSettingItem::DelayHideKeyboard()
{
    const QWidget* pInputWgt = GetCurrInputWgt();
    SETTINGUI_INFO("CSettingItem::SetFocus other hide keyboard");
    qSetIsShowWnd(pInputWgt, false);
}

void CSettingItem::showEvent(QShowEvent * pEvent)
{
    if (m_pWgt)
    {
        m_pWgt->show();
    }
}

void CSettingItem::mousePressEvent(QMouseEvent* pEvent)
{
    QPoint ptGlobal = pEvent->globalPos();
    QPoint pt = mapFromGlobal(ptGlobal);

    if (IsFocus()
            && NULL != m_pWgt && m_pWgt->isEnabled()
            && m_pWgt->geometry().contains(pt)
            && (m_pWgt->inherits("QLineEdit")
                || m_pWgt->inherits("QTextEdit")
               ))
    {
        QRect rectWgt = m_pWgt->geometry();
        pEvent->accept();
        // 转发mouse press消息
        QPoint ptWgt(pt.x() - rectWgt.left(), pt.y() - rectWgt.top());
        QMouseEvent evtMouse(QEvent::MouseButtonPress, ptWgt, ptGlobal, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

        if (!m_pWgt->inherits("QTextEdit"))
        {
            QApplication::sendEvent(m_pWgt, &evtMouse);
        }
        else
        {
            QTextEdit* pTextWgt = static_cast<QTextEdit*>(m_pWgt);
            if (NULL != pTextWgt)
            {
                QApplication::sendEvent(pTextWgt->viewport(), &evtMouse);
            }
        }
    }

    if (NONE_EXTRA == m_iExtraButten || (!m_btnExtra.ProcessMousePress(pEvent)))
    {
        if (!ProcessMousePress(pEvent))
        {
            return;
        }
    }

    pEvent->accept();
}


#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR

#ifdef  QT_TOUCH_SUPPORT
void CSettingItem::SetChangeStyleOnPress(bool bChange)
{
    m_bChangeStyleOnPress = bChange;
}
#endif

// 设置是否反白
void CSettingItem::SetChangeStyleOnSelect(bool bChange)
{
    m_bChangeStyleOnSelect = bChange;
}
#endif  //IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR

void CSettingItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
    //解决双重选中背景的问题
    //PaintFocusBg(stylePainter);

    ApplyStyleByState(GetState());
    if (m_iExtraButten == DIRECTORY_EXTRA)
    {
        QRect rcExtra(m_iBtnExtraLeft + DIRECTORY_ICON_POS_LEFT_SPACING, DIRECTORY_ICON_POS_HIGH_SPACING, DIRECTORY_ICON_POS_WIDTH, DIRECTORY_ICON_POS_HEIGHT);
        m_btnExtra.PaintButton(stylePainter, PIC_ICON_DIRECTORY, PIC_ICON_DIRECTORY_PRESS, PIC_ICON_DIRECTORY, rcExtra);
    }
}


bool CSettingItem::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (NULL != pObj && pObj == m_pWgt && NULL != pEvent
            && pEvent->type() == QEvent::FocusIn && !IsFocus())
    {
        SelectItem(true);

        if (FullKeyboardEnabled()
                && m_pWgt->isEnabled()
                && m_pWgt->isVisible()
                && (m_pWgt->inherits("QLineEdit") || m_pWgt->inherits("QTextEdit")
                   )
                && m_pAction != NULL)
        {
            //底层访问上层的实现可以优化
            if (m_pWgetHost != NULL && m_pWgetHost->inherits("CDlgBaseSubPage"))
            {
                CDlgBaseSubPage* pSubPage = (CDlgBaseSubPage*)m_pWgetHost;
                if (pSubPage->isListReady())
                {
                    SETTINGUI_INFO("CSettingItem::eventFilter Focus in show keyboard");
                    qSetIsShowWnd(m_pWgt, true);
                }
            }
            else
            {
                SETTINGUI_INFO("CSettingItem::eventFilter Focus in show keyboard");
                qSetIsShowWnd(m_pWgt, true);
            }
        }
    }

    if (FullKeyboardEnabled()
            && NULL != pObj && (pObj == m_pWgt || pObj == m_pLabDisplay) && NULL != pEvent
            && pEvent->type() == QEvent::MouseButtonPress && IsFocus())
    {
        if (m_pWgt->isEnabled()
                && m_pWgt->isVisible()
                && (m_pWgt->inherits("QLineEdit") || m_pWgt->inherits("QTextEdit")
                   )
                && m_pAction != NULL)
        {
            qSetIsShowWnd(m_pWgt, true);
        }
    }
    return false;
}

#ifdef  QT_TOUCH_SUPPORT
void CSettingItem::SetClickBg(yl::string strClickBg)
{
    m_strClickBg = strClickBg;
}
#endif

void CSettingItem::SetConfiged(bool bConfiged)
{
    if (bConfiged != m_bConfiged)
    {
        m_bConfiged = bConfiged;
        OnConfigedChange(m_bConfiged);
    }
}

void CSettingItem::OnConfigedChange(bool)
{
    //
}

ITEM_STATE CSettingItem::GetState()
{
    if (IsDown())
    {
        if (IsFocus())
        {
            return ITEM_STATE_SELECT_PRESS;
        }
        else
        {
            return ITEM_STATE_PRESS;
        }
    }
    else if (IsFocus())
    {
        return ITEM_STATE_SELECT;
    }
    else
    {
        return ITEM_STATE_NOMAL;
    }
}

void CSettingItem::ApplyStyleByState(ITEM_STATE eState)
{
    switch (eState)
    {
    case ITEM_STATE_NOMAL:
        {
            ApplyNomalStyle();
        }
        break;
    case ITEM_STATE_PRESS:
    case ITEM_STATE_SELECT_PRESS:
        {
            ApplyPressStyle();
        }
        break;
    case ITEM_STATE_SELECT:
        {
            ApplySelectStyle();
        }
        break;
    default:
        break;
    }
}

void CSettingItem::ApplyPressStyle()
{
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef  QT_TOUCH_SUPPORT
    if (m_bChangeStyleOnPress)
    {
        QPalette pal;
        const QColor color = CustomTheme_GetCustomFontColorByKey(CUSTOM_FONT_ID_SETTING_FOCUS);

        if (NULL != m_pWgt && m_pWgt->inherits("QLabel"))
        {
            pal = m_pWgt->palette();
            pal.setBrush(QPalette::Text, QBrush(color));
            pal.setBrush(QPalette::WindowText, QBrush(color));
            m_pWgt->setPalette(pal);
        }

        if (NULL != m_pLabDisplay)
        {
            pal = m_pLabDisplay->palette();
            pal.setBrush(QPalette::Foreground, QBrush(color));
            m_pLabDisplay->setPalette(pal);
        }
    }
    else
#endif
#endif
    {
        ApplyNomalStyle();
    }
}

void CSettingItem::ApplySelectStyle()
{
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
    if (m_bChangeStyleOnSelect)
    {
        QPalette pal;
        const QColor color = CustomTheme_GetCustomFontColorByKey(CUSTOM_FONT_ID_SETTING_FOCUS);

        if (NULL != m_pWgt && m_pWgt->inherits("QLabel"))
        {
            pal = m_pWgt->palette();
            pal.setBrush(QPalette::Text, QBrush(color));
            pal.setBrush(QPalette::WindowText, QBrush(color));
            m_pWgt->setPalette(pal);
        }

        if (NULL != m_pLabDisplay)
        {
            pal = m_pLabDisplay->palette();
            pal.setBrush(QPalette::Foreground, QBrush(color));
            m_pLabDisplay->setPalette(pal);
        }
    }
    else
#endif
    {
        ApplyNomalStyle();
    }
}

void CSettingItem::ApplyNomalStyle()
{
    QPalette pal;
    const QColor color = CustomTheme_GetCustomFontColorByKey(CUSTOM_FONT_ID_SETTING_NORMAL);

    if (NULL != m_pWgt)
    {
        pal = m_pWgt->palette();
        pal.setBrush(QPalette::Text, QBrush(color));
        pal.setBrush(QPalette::WindowText, QBrush(color));
        m_pWgt->setPalette(pal);
    }

    if (NULL != m_pLabDisplay)
    {
        pal = m_pLabDisplay->palette();
        pal.setBrush(QPalette::Foreground, QBrush(color));
        m_pLabDisplay->setPalette(pal);
    }
}



void CSettingItem::mouseReleaseEvent(QMouseEvent * pEvent)
{
    bool bEetraDown = false;
    bool bExtraClick = false;

    if (m_iExtraButten == DIRECTORY_EXTRA)
    {
        bEetraDown = m_btnExtra.IsDown();
        bExtraClick = m_btnExtra.ProcessMouseRelease(pEvent);
    }

    if (!bExtraClick)
    {
        if (bEetraDown)
        {
            m_btnExtra.ResetDown();
        }

        if (!ProcessMouseRelease(pEvent, true))
        {
            return;
        }
    }
    if (bExtraClick)
    {
        DoAction(m_iExtraButten);
    }
}

#ifdef IF_ENABLE_TESTABILITY
QString CSettingItem::GetTestInfo()
{

    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");
    if (m_iExtraButten != NONE_EXTRA)
    {
        // btn
        AddBtnNode(nodeRoot, m_btnExtra, BTN_DIRECTORY);
    }

    QString strTestinfo;
    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;

}
#endif


/*****************************************Class  CSettingMenuItem***************************************/
CSettingMenuItem::CSettingMenuItem(QWidget * parent, MenuItemData * pItemData,
                                   bool bAddIcon /* = false*/
                                   , int iDataIndex/* = -1*/, bool bAddInex/* = true*/, bool bWithStatus/* = false*/)
    : CSettingItem(parent)
    , m_btnEnter(this)
    , m_bAddIcon(bAddIcon)
    , m_bWithStatus(bWithStatus)
{
    /*
    #if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
        // 设置是否有选中效果
        SetChangeStyleOnSelect(true);
    #endif*/


    m_nType |= LIST_ITEM_SUB_TYPE_SETTING_MENU;

    m_bAddIndex = bAddInex;
    m_pWgt = NULL;
    setData(pItemData, bAddIcon);
    SetDataIndex(iDataIndex);
    GenerateLabelText();
    setObjectName("CSettingMenuItem");
}

CSettingMenuItem::CSettingMenuItem(QWidget * pParent, QWidgetPair & pair)
    : CSettingItem(pParent, pair/*, false*/)
    , m_btnEnter(this)
    , m_bAddIcon(pair.bAddIcon)
    , m_bWithStatus(pair.bWithStatus)
{
    /*
    #if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
        // 设置是否有选中效果
        SetChangeStyleOnSelect(true);
    #endif*/


    m_nType |= LIST_ITEM_SUB_TYPE_SETTING_MENU;

    if (m_bAddIcon)
    {
        yl::string strIconPath = PIC_MENU_PREFIX;
        strIconPath += pair.strIcon;
        m_iconMap = THEME_GET_BMP(strIconPath.c_str());
    }
    setObjectName("CSettingMenuItem");
}

void CSettingMenuItem::AddIcon(LPCSTR strIconPath)
{
    if (strcmp(strIconPath, "") == 0)
    {
        return;
    }

    m_iconMap = THEME_GET_BMP(strIconPath);
    if (m_iconMap.isNull())
    {
        return;
    }

    m_bAddIcon = true;
    Relayout();
}

CSettingMenuItem::~CSettingMenuItem()
{
    //
}

int CSettingMenuItem::GetSettingMenuItemType()
{
    return (LIST_ITEM_TYPE_SETTING | LIST_ITEM_SUB_TYPE_SETTING_MENU);
}

void CSettingMenuItem::setData(MenuItemData * pItemData, bool bAddIcon/*= false*/)
{
    if (bAddIcon)
    {
        yl::string strIconPath = PIC_MENU_PREFIX;
        strIconPath += pItemData->m_strIconName;
        m_iconMap = THEME_GET_BMP(strIconPath.c_str());
    }
    m_strAction = toQString(pItemData->m_strClickAction.c_str());
    SetLabText(toQString(pItemData->m_strDisplayName.c_str()));
}

void CSettingMenuItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
    //解决双重选中背景的问题
    //PaintFocusBg(stylePainter);
#ifdef  QT_TOUCH_SUPPORT
    PaintDownBg(stylePainter, m_strClickBg.empty() ? NULL : m_strClickBg.c_str());
#endif
    ApplyStyleByState(GetState());

    int nLeftBegin = 0;
    int nRightEnd = 0;

    if (NULL != m_pDelegate)
    {
        m_pDelegate->GetContentPos(nLeftBegin, nRightEnd);
    }
    if (m_bAddIcon && !m_iconMap.isNull())
    {
        QSize iconSize = m_iconMap.size();
        stylePainter.drawPixmap(nLeftBegin, (rect().height() - MENU_ICON_HEIGHT) / 2
                                , iconSize.width() < MENU_ICON_WIDTH ? iconSize.width() : MENU_ICON_WIDTH
                                , iconSize.height() < MENU_ICON_HEIGHT ? iconSize.height() : MENU_ICON_HEIGHT, m_iconMap);
    }

    m_btnEnter.PaintButton(stylePainter, PIC_GENERAL_LIST_ITEM_ENTER,
                           PIC_GENERAL_LIST_ITEM_ENTER_CLICK, PIC_GENERAL_LIST_ITEM_ENTER);
}

void CSettingMenuItem::Relayout()
{
    QRect rcItem = rect();
    int nLeftBegin = 0;
    int nRightEnd = 0;
    int nbtnEnterLeft = rcItem.right() - DEFAULT_MENU_ENTER_POS_RIGHT - DEFAULT_MENU_ENTER_POS_WIDTH;

    if (NULL != m_pDelegate)
    {
        m_pDelegate->GetContentPos(nLeftBegin, nRightEnd);
    }

    if (m_bAddIcon && (!m_iconMap.isNull()))
    {
        nLeftBegin += MENU_ICON_WIDTH + DEFAULT_ICON_LABEL_SAPCING;
    }

    if (NULL != m_pWgt)
    {
        if (m_pLabDisplay)
        {
            m_pLabDisplay->setGeometry(nLeftBegin, (SETTING_ITEM_HEIGHT - SETTING_ITEM_LABEL_HEIGHT) / 2
                                       , SETTING_WIGET_DEFAUT_X - DEFAULT_COLUMN_SAPCING - nLeftBegin, SETTING_ITEM_LABEL_HEIGHT);
        }

        if (m_bWithStatus && m_pWgt->inherits("QLabel"))
        {
            ((QLabel *)m_pWgt)->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
        int nLeft = m_bWithStatus ? nbtnEnterLeft - MENU_SWITCH_ARROW_SPACINE - MENU_SWITCH_WGT_WIDTH :
                    SETTING_WIGET_DEFAUT_X;
#if IF_SETTING_ITEM_WGT_HEIGHT_FIXED
        int nTop = (rcItem.height() - m_pWgt->height()) / 2;
        QRect rcWgt(nLeft, nTop, m_bWithStatus ? MENU_SWITCH_WGT_WIDTH : SETTING_ITEM_WGT_WIDTH -
                    SLIDER_BAR_WIDTH
                    , m_pWgt->height());
#else
        QRect rcWgt(nLeft, 0, m_bWithStatus ? MENU_SWITCH_WGT_WIDTH : SETTING_ITEM_WGT_WIDTH -
                    SLIDER_BAR_WIDTH
                    , this->height());
#endif
        m_pWgt->setGeometry(rcWgt);
    }
    else
    {
        if (m_pLabDisplay)
        {
            m_pLabDisplay->setGeometry(nLeftBegin, (SETTING_ITEM_HEIGHT - SETTING_ITEM_LABEL_HEIGHT) / 2
                                       , nbtnEnterLeft - DEFAULT_COLUMN_SAPCING - nLeftBegin, SETTING_ITEM_LABEL_HEIGHT);
        }
    }


    int nTop = rcItem.top() + DEFAULT_MENU_ENTER_POS_TOP;
    QRect rcBtnEnter(nbtnEnterLeft, nTop, DEFAULT_MENU_ENTER_POS_WIDTH, DEFAULT_MENU_ENTER_POS_HEIGHT);

    m_btnEnter.SetRect(rcBtnEnter);
}

/*void CSettingMenuItem::SetClickBg()
{
    //
}*/

/*****************************************Class  CSettingRadioItem***************************************/
CSettingRadioItem::CSettingRadioItem(QWidget * parent, QString strDisplayName, QString strAction,
                                     int iDataIndex/* = -1*/, bool bAddIndex/* = true*/)
    : CSettingItem(parent, strDisplayName, NULL, strAction, iDataIndex, bAddIndex, false)
{
    m_nType |= LIST_ITEM_SUB_TYPE_SETTING_RADIO;
    setObjectName("CSettingRadioItem");
}

CSettingRadioItem::CSettingRadioItem(QWidget * parent/* = NULL*/,
                                     IListItemAction * pAction/* = NULL*/)
    : CSettingItem(parent, pAction)
{
    m_nType |= LIST_ITEM_SUB_TYPE_SETTING_RADIO;

    Init();
    setObjectName("CSettingRadioItem");
}

int CSettingRadioItem::GetSettingRadioItemType()
{
    return (LIST_ITEM_TYPE_SETTING | LIST_ITEM_SUB_TYPE_SETTING_RADIO);
}

void CSettingRadioItem::Init()
{
    m_pIconLab = new QLabel(this);
    if (NULL != m_pIconLab)
    {
        m_pIconLab->setObjectName("IconLab");
        m_pIconLab->setAutoFillBackground(true);
        QPalette pal = m_pIconLab->palette();
        pal.setBrush(QPalette::Window, QBrush(THEME_GET_BMP(PIC_RADIO_OFF)));
        m_pIconLab->setPalette(pal);
    }
}

CSettingRadioItem::CSettingRadioItem(QWidget * pParent, QWidgetPair & pair)
    : CSettingItem(pParent, pair, false)
{
    m_pWgt = NULL;
    m_nType |= LIST_ITEM_SUB_TYPE_SETTING_RADIO;

    Init();
    setObjectName("CSettingRadioItem");
}

void CSettingRadioItem::Relayout()
{
    int nLeftBegin = 0;
    int nRightEnd = 0;

    if (NULL != m_pDelegate)
    {
        m_pDelegate->GetContentPos(nLeftBegin, nRightEnd);
    }

    if (NULL != m_pIconLab)
    {
        m_pIconLab->setGeometry(nLeftBegin, (this->height() - RADIO_SIZE.height()) / 2,
                                RADIO_SIZE.width(), RADIO_SIZE.height());
    }

    if (NULL != m_pLabDisplay && NULL != m_pIconLab)
    {
        QRect rectDisp = rect();
        rectDisp.setX(nLeftBegin + m_pIconLab->rect().right() + DEFAULT_COLUMN_SAPCING);
        rectDisp.setY((SETTING_ITEM_HEIGHT - SETTING_ITEM_LABEL_HEIGHT) / 2);
        rectDisp.setWidth(this->rect().right() - nRightEnd - rectDisp.x());
        rectDisp.setHeight(SETTING_ITEM_LABEL_HEIGHT);

        m_pLabDisplay->setGeometry(rectDisp);
    }
}

void CSettingRadioItem::SetFocus(bool bFocus)
{
    CSettingItem::SetFocus(bFocus);
    UpdateIcon();
    update();
}

void CSettingRadioItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
    //解决双重选中背景的问题
    //PaintFocusBg(stylePainter);
#ifdef  QT_TOUCH_SUPPORT
    PaintDownBg(stylePainter, m_strClickBg.empty() ? NULL : m_strClickBg.c_str());
#endif
    ApplyStyleByState(GetState());
}

void CSettingRadioItem::OnConfigedChange(bool bConfiged)
{
    UpdateIcon();
}

void CSettingRadioItem::SetItemEnabled(bool bEnabled)
{
    this->setEnabled(bEnabled);
    UpdateIcon();
}

void CSettingRadioItem::UpdateIcon()
{
    //设置单选按钮样式
    if (NULL != m_pIconLab)
    {
        bool bEnabled = IsItemEnabled();
        yl::string strPic = PIC_RADIO_OFF;
#if RADIO_ITEM_CHANGE_ICON_ON_FOCUS
        if (IsFocus())
#else
        if (IsConfiged())
#endif
        {
            strPic = PIC_RADIO_ON;
        }
        else if (!bEnabled)
        {
            strPic = PIC_RADIO_DISABLE;
        }

        QPalette pal = m_pIconLab->palette();
        pal.setBrush(QPalette::Window, QBrush(THEME_GET_BMP(strPic.c_str())));
        m_pIconLab->setPalette(pal);
    }
}

bool CSettingRadioItem::IsItemEnabled()
{
    return this->isEnabled();
}

#if IF_FEATURE_METASWITCH_ACD
/*****************************************Class  CMTSWAcdListItem***************************************/
CMTSWAcdListItem::CMTSWAcdListItem(QWidget * parent, MenuItemData * pItemData,
                                   bool bAddIcon /* = false*/
                                   , int iDataIndex/* = -1*/, bool bAddInex/* = true*/, bool bWithStatus/* = false*/)
    : CSettingMenuItem(parent, pItemData, bAddIcon, iDataIndex, bAddInex, bWithStatus)
{

    m_nType |= LIST_ITEM_SUB_TYPE_SETTING_MTSWACDLIST;
}

CMTSWAcdListItem::CMTSWAcdListItem(QWidget * pParent, QWidgetPair & pair)
    : CSettingMenuItem(pParent, pair)
{
    m_nType |= LIST_ITEM_SUB_TYPE_SETTING_MTSWACDLIST;
}

CMTSWAcdListItem::~CMTSWAcdListItem()
{
    //
}

void CMTSWAcdListItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
    //解决双重选中背景的问题
    //PaintFocusBg(stylePainter);
#ifdef  QT_TOUCH_SUPPORT
    PaintDownBg(stylePainter, m_strClickBg.empty() ? NULL : m_strClickBg.c_str());
#endif
    ApplyStyleByState(GetState());

    int nLeftBegin = 0;
    int nRightEnd = 0;

    if (NULL != m_pDelegate)
    {
        m_pDelegate->GetContentPos(nLeftBegin, nRightEnd);
    }
    if (m_bAddIcon && !m_iconMap.isNull())
    {
        QSize iconSize = m_iconMap.size();
        stylePainter.drawPixmap(nLeftBegin, (rect().height() - MENU_ICON_HEIGHT) / 2
                                , iconSize.width() < MENU_ICON_WIDTH ? iconSize.width() : MENU_ICON_WIDTH
                                , iconSize.height() < MENU_ICON_HEIGHT ? iconSize.height() : MENU_ICON_HEIGHT, m_iconMap);
    }

    m_btnEnter.PaintButton(stylePainter, PIC_GENERAL_LIST_ITEM_ENTER,
                           PIC_GENERAL_LIST_ITEM_ENTER_CLICK, PIC_GENERAL_LIST_ITEM_ENTER);
}

void CMTSWAcdListItem::Relayout()
{
    QRect rcItem = rect();
    int nLeftBegin = 0;
    int nRightEnd = 0;
    int nbtnEnterLeft = rcItem.right() - DEFAULT_MENU_ENTER_POS_RIGHT - DEFAULT_MENU_ENTER_POS_WIDTH;

    if (NULL != m_pDelegate)
    {
        m_pDelegate->GetContentPos(nLeftBegin, nRightEnd);
    }

    if (m_bAddIcon && (!m_iconMap.isNull()))
    {
        nLeftBegin += MENU_ICON_WIDTH + DEFAULT_ICON_LABEL_SAPCING;
    }

    if (NULL != m_pWgt)
    {
        if (m_pLabDisplay)
        {
            m_pLabDisplay->setGeometry(nLeftBegin, (SETTING_ITEM_HEIGHT - SETTING_ITEM_LABEL_HEIGHT) / 2
                                       , nbtnEnterLeft * 2 / 3 - nLeftBegin, SETTING_ITEM_LABEL_HEIGHT);
        }

        int nLeft = m_pLabDisplay->rect().right() + DEFAULT_COLUMN_SAPCING;
        int nWidth = nbtnEnterLeft - nLeft - DEFAULT_COLUMN_SAPCING;

#if IF_SETTING_ITEM_WGT_HEIGHT_FIXED
        int nTop = (rcItem.height() - m_pWgt->height()) / 2;
        QRect rcWgt(nLeft, nTop, nWidth, m_pWgt->height());
#else
        QRect rcWgt(nLeft, 0, m_bWithStatus ? nWidth, this->height());
#endif
        m_pWgt->setGeometry(rcWgt);
    }
    else
    {
        if (m_pLabDisplay)
        {
            m_pLabDisplay->setGeometry(nLeftBegin, (SETTING_ITEM_HEIGHT - SETTING_ITEM_LABEL_HEIGHT) / 2
                                       , nbtnEnterLeft - DEFAULT_COLUMN_SAPCING - nLeftBegin, SETTING_ITEM_LABEL_HEIGHT);
        }
    }


    int nTop = rcItem.top() + DEFAULT_MENU_ENTER_POS_TOP;
    QRect rcBtnEnter(nbtnEnterLeft, nTop, DEFAULT_MENU_ENTER_POS_WIDTH, DEFAULT_MENU_ENTER_POS_HEIGHT);

    m_btnEnter.SetRect(rcBtnEnter);
}
#endif


#if IF_FEATURE_GENBAND_E911
/*****************************************Class  CGenbandE911ListItem***************************************/
CGenbandE911ListItem::CGenbandE911ListItem(QWidget * pParent, QWidgetPair & pair)
    : CSettingItem(pParent, pair)
{
    m_nType |= LIST_ITEM_SUB_TYPE_GENBAND_E911;
}

CGenbandE911ListItem::~CGenbandE911ListItem()
{
    //
}

void CGenbandE911ListItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
    //解决双重选中背景的问题
#ifdef  QT_TOUCH_SUPPORT
    PaintFocusBg(stylePainter, PIC_GENERAL_LIST_ITEM_CLICK_BG, rect());

    PaintDownBg(stylePainter, m_strClickBg.empty() ? NULL : m_strClickBg.c_str());
#endif
    ApplyStyleByState(GetState());
}
#endif
