#include <QtGui>
#include "contactsearchitem.h"
#include "baseui/fontcolor.h"
#include "baseui/t4xpicpath.h"
#include "uicommon.h"
#include "imagemanager/modimagemanager.h"
#include "inputmethod/inputmethod.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qwidgetutility.h"
#include "baseui/framelistaction.h"


namespace
{
#define CONTACT_SEARCH_ITEM_POS_HEIGHT 55
#define CONTACT_SEARCH_ITEM_POS_EDIT_BG_LEFT 7
#define CONTACT_SEARCH_ITEM_POS_EDIT_BG_TOP 7
#define CONTACT_SEARCH_ITEM_POS_EDIT_BG_WIDHT 241
#define CONTACT_SEARCH_ITEM_POS_EDIT_BG_HEIGHT 41
#define CONTACT_SEARCH_ITEM_POS_EDIT_LEFT 11
#define CONTACT_SEARCH_ITEM_POS_EDIT_TOP 8
#define CONTACT_SEARCH_ITEM_POS_EDIT_WIDTH 192
#define CONTACT_SEARCH_ITEM_POS_EDIT_HEIGHT 38
#define CONTACT_SEARCH_ITEM_EDIT_FONT_SIZE 26
#define CONTACT_SEARCH_ITEM_EDIT_FONT_WEIGHT QFont::Normal
#define CONTACT_SEARCH_ITEM_EDIT_TIP_COLOR QColor(153, 153, 153)
#define CONTACT_SEARCH_ITEM_SELECT_TEXT_COLOR QColor(255, 255, 255)
#define CONTACT_SEARCH_ITEM_NORMAL_TEXT_COLOR QColor(0, 0, 0)

#define CONTACT_SEARCH_ITEM_FONT_SIZE 16
}

CContactSearchItem::CContactSearchItem(QWidget * parent/* = NULL*/,
                                       IListItemAction * pAction/* = NULL*/)
    : CListItem(LIST_ITEM_TYPE_CONTACT_SEARCH, parent, pAction)
    , m_nSearchType(0)
    , m_pEdit(NULL)
    , m_strTitle(QLatin1String(""))
    , m_btnSearch(this)
    , m_bSearch(false)
{
    setObjectName("CContactSearchItem");

    setGeometry(0, 0, 0, CONTACT_SEARCH_ITEM_POS_HEIGHT);

    qWidgetUtility::setFontStyle(this, CONTACT_SEARCH_ITEM_FONT_SIZE);
}

CContactSearchItem::~CContactSearchItem(void)
{
    ReleaseSearchEdit();
}

CContactSearchItem* CContactSearchItem::GetContactSearchItem(CListItem* pItem)
{
    if (NULL == pItem || LIST_ITEM_TYPE_CONTACT_SEARCH != pItem->GetType())
    {
        return NULL;
    }

    return (CContactSearchItem*)pItem;
}

void CContactSearchItem::SetSearchType(int nSearchType)
{
    m_nSearchType = nSearchType;
}

void CContactSearchItem::SetTitle(const QString & strTitle)
{
    m_strTitle = strTitle;
}

void CContactSearchItem::SetKey(const QString & strKey)
{
    if (NULL != m_pEdit)
    {
        m_pEdit->setText(strKey);
    }
}

int CContactSearchItem::GetSearchType() const
{
    return m_nSearchType;
}

QString CContactSearchItem::GetKey() const
{
    if (NULL != m_pEdit)
    {
        return m_pEdit->text();
    }

    return QLatin1String("");
}

QLineEdit * CContactSearchItem::GetEdit() const
{
    return m_pEdit;
}

void CContactSearchItem::SetSearch(bool bSearch)
{
    if (bSearch)
    {
        InitSearchEdit();
    }
    else
    {
        ReleaseSearchEdit();
    }
}

void CContactSearchItem::InitSearchEdit()
{
    if (NULL == m_pEdit)
    {
        m_pEdit = new QLineEdit(this);

        if (NULL == m_pEdit)
        {
            return;
        }

        m_pEdit->installEventFilter(this);
    }

    QPalette editPalette = m_pEdit->palette();

    editPalette.setColor(QPalette::Base, WidgetTransparentColor);
    editPalette.setColor(QPalette::Text, THEME_TEXT_COLOR);
    editPalette.setColor(QPalette::Highlight, THEME_EDIT_HL_COLOR);
    editPalette.setColor(QPalette::HighlightedText, THEME_EDIT_HL_TEXT_COLOR);

    m_pEdit->setPalette(editPalette);

    qWidgetUtility::setFontStyle(m_pEdit, CONTACT_SEARCH_ITEM_EDIT_FONT_SIZE,
                                 CONTACT_SEARCH_ITEM_EDIT_FONT_WEIGHT);

    m_pEdit->setGeometry(CONTACT_SEARCH_ITEM_POS_EDIT_LEFT, CONTACT_SEARCH_ITEM_POS_EDIT_TOP,
                         CONTACT_SEARCH_ITEM_POS_EDIT_WIDTH, CONTACT_SEARCH_ITEM_POS_EDIT_HEIGHT);

    m_pEdit->setReadOnly(false);
    m_pEdit->setText(QLatin1String(""));
    m_pEdit->show();
    m_pEdit->raise();
    m_btnSearch.SetRect(200, 0, 45, 45);
}

void CContactSearchItem::ReleaseSearchEdit()
{
    if (NULL == m_pEdit)
    {
        return;
    }

    QLineEdit * pEdit = m_pEdit;

    // 防止多次调用
    m_pEdit = NULL;

    qUninstallIME(pEdit);

    pEdit->removeEventFilter(this);
    pEdit->setParent(NULL);
    pEdit->hide();
    pEdit->deleteLater();
}

void CContactSearchItem::DelayShowKeyboard()
{
    m_bDelayShowKeyboard = true;
}

void CContactSearchItem::InstallKeyboard(INPUT_DIALOG_STYLE eStyle)
{
    if (NULL == m_pEdit)
    {
        return;
    }

    qUseKeyBoardFromIME(m_pEdit, eStyle);
}

void CContactSearchItem::SetFocus(bool bFocus)
{
    if (NULL != m_pEdit)
    {
        if (bFocus)
        {
            if (!m_pEdit->hasFocus())
            {
                m_pEdit->setFocus(Qt::MouseFocusReason);
                emit editFocus(true);
            }
            if (m_bDelayShowKeyboard)
            {
                m_bDelayShowKeyboard = false;
                qSetIsShowWnd(m_pEdit, true);
            }
        }
        else
        {
            m_pEdit->clearFocus();
            emit editFocus(false);

            QPalette pal = palette();

            pal.setColor(QPalette::WindowText, CONTACT_SEARCH_ITEM_EDIT_TIP_COLOR);
            setPalette(pal);
        }
    }
    else
    {
        QPalette pal = palette();

        if (bFocus)
        {
            pal.setColor(QPalette::WindowText, CONTACT_SEARCH_ITEM_SELECT_TEXT_COLOR);
        }
        else
        {
            pal.setColor(QPalette::WindowText, CONTACT_SEARCH_ITEM_NORMAL_TEXT_COLOR);
        }

        setPalette(pal);
    }

    CListItem::SetFocus(bFocus);
}

bool CContactSearchItem::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL != pObject && pObject == m_pEdit && NULL != pEvent)
    {
        int nType = pEvent->type();

        if (QEvent::FocusOut == nType)
        {
            m_pEdit->setText(QLatin1String(""));
        }
        else if (QEvent::FocusIn == nType && (!IsFocus()))
        {
            QFocusEvent * pFocusEvent = (QFocusEvent *)pEvent;

            DIRUI_INFO("CContactSearchItem FocusIn Reason:%d", pFocusEvent->reason());

            if (Qt::MouseFocusReason == pFocusEvent->reason())
            {
                SelectItem(true);
            }
            else
            {
                pEvent->ignore();
                return true;
            }
        }

        if (FullKeyboardEnabled() && nType == QEvent::MouseButtonPress)
        {
            if (NULL == m_pEdit || !m_pEdit->isEnabled() || m_pEdit->isReadOnly() || !m_pEdit->hasFocus())
            {
                return false;
            }
            qSetIsShowWnd(m_pEdit, true);
        }
    }

    return CListItem::eventFilter(pObject, pEvent);
}

void CContactSearchItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    QRect rcItem = rect();

    if (NULL == m_pEdit)
    {
        QRect rcTip(rcItem.left() + CONTACT_SEARCH_ITEM_POS_EDIT_LEFT,
                    rcItem.top() + CONTACT_SEARCH_ITEM_POS_EDIT_TOP,
                    CONTACT_SEARCH_ITEM_POS_EDIT_WIDTH, CONTACT_SEARCH_ITEM_POS_EDIT_HEIGHT);

        stylePainter.drawItemText(rcTip, (Qt::AlignLeft | Qt::AlignVCenter),  palette(), isEnabled(),
                                  m_strTitle);
        if (m_bSearch)
        {
            m_btnSearch.PaintButton(stylePainter, NULL, PIC_IDLE_SRCEEN_STATUS_BUTTON_CLICK);

        }
        return;
    }

    QRect rcBG(rcItem.left() + CONTACT_SEARCH_ITEM_POS_EDIT_BG_LEFT,
               rcItem.top() + CONTACT_SEARCH_ITEM_POS_EDIT_BG_TOP,
               CONTACT_SEARCH_ITEM_POS_EDIT_BG_WIDHT, CONTACT_SEARCH_ITEM_POS_EDIT_BG_HEIGHT);
    QPixmap pmBG = THEME_GET_BMP(PIC_DIRECTORY_EDIT_CONTACT_SEARCH);

    if (!pmBG.isNull())
    {
        stylePainter.drawPixmap(rcBG, pmBG);
    }

    if (!m_pEdit->hasFocus() && m_pEdit->text().isEmpty())
    {
        QRect rcTip(rcItem.left() + CONTACT_SEARCH_ITEM_POS_EDIT_LEFT,
                    rcItem.top() + CONTACT_SEARCH_ITEM_POS_EDIT_TOP,
                    CONTACT_SEARCH_ITEM_POS_EDIT_WIDTH, CONTACT_SEARCH_ITEM_POS_EDIT_HEIGHT);

        stylePainter.drawItemText(rcTip, (Qt::AlignLeft | Qt::AlignVCenter), palette(), isEnabled(),
                                  m_strTitle);
    }
    if (m_bSearch)
    {
        m_btnSearch.PaintButton(stylePainter, NULL, PIC_IDLE_SRCEEN_STATUS_BUTTON_CLICK);

    }
}

void CContactSearchItem::SetBtnSearch(bool bVisible)
{
    m_bSearch = bVisible;
}

void CContactSearchItem::mousePressEvent(QMouseEvent * pEvent)
{
    if (!m_bSearch || !(m_btnSearch.ProcessMousePress(pEvent)))
    {
        if (!ProcessMousePress(pEvent))
        {
            return;
        }
    }

    pEvent->accept();
}

void CContactSearchItem::mouseReleaseEvent(QMouseEvent * pEvent)
{
    bool bSearchDown = false;
    bool bSearchClick = false;

    if (m_bSearch)
    {
        bSearchDown = m_btnSearch.IsDown();
        bSearchClick = m_btnSearch.ProcessMouseRelease(pEvent);
    }

    if (!bSearchClick)
    {
        if (bSearchDown)
        {
            m_btnSearch.ResetDown();
        }

        if (!ProcessMouseRelease(pEvent, true))
        {
            return;
        }
    }

    if (bSearchClick)
    {
        DoAction(CONTACT_ITEM_ACTION_SEARCH);
    }
}
