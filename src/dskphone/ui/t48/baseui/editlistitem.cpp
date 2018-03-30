#include <QtGui>
#include "baseui/fontcolor.h"
#include "uikernel/ilistitemdelegate.h"
#include "editlistitem.h"
#include "t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "inputmethod/inputmethod.h"
#include "uicommon.h"
#include "uikernel/qwidgetutility.h"

namespace
{
// 输入背景框位置信息
#define DEFAULT_EDIT_LIST_ITEM_EDIT_BG_LEFT 141
#define DEFAULT_EDIT_LIST_ITEM_EDIT_BG_TOP 3
#define DEFAULT_EDIT_LIST_ITEM_EDIT_BG_WIDTH 212
#define DEFAULT_EDIT_LIST_ITEM_EDIT_BG_HEIGHT 36

// 输入框位置信息
#define DEFAULT_EDIT_LIST_ITEM_EDIT_LEFT 141
#define DEFAULT_EDIT_LIST_ITEM_EDIT_TOP 3
#define DEFAULT_EDIT_LIST_ITEM_EDIT_WIDTH 212
#define DEFAULT_EDIT_LIST_ITEM_EDIT_HEIGHT 36

#define DEFAULT_EDIT_LIST_ITEM_FONT_SIZE 16
}

CEditListItem::CEditListItem(QWidget * parent/* = NULL*/, IListItemAction * pAction/* = NULL*/)
    : CDetailListItemBase(parent, pAction)
    , m_pEdit(NULL)
    , m_bFrameLayout(false)
    , m_rcEditBG(DEFAULT_EDIT_LIST_ITEM_EDIT_BG_LEFT, DEFAULT_EDIT_LIST_ITEM_EDIT_BG_TOP,
                 DEFAULT_EDIT_LIST_ITEM_EDIT_BG_WIDTH, DEFAULT_EDIT_LIST_ITEM_EDIT_BG_HEIGHT)
    , m_strBg("")
{

    setObjectName("CEditListItem");

    m_nType |= LIST_ITEM_SUB_TYPE_DETAIL_EDIT;

    qWidgetUtility::setFontStyle(this, DEFAULT_EDIT_LIST_ITEM_FONT_SIZE);

    m_pEdit = new QLineEdit(this);

    if (NULL != m_pEdit)
    {
        m_pEdit->setObjectName("Edit");
        m_pEdit->setGeometry(DEFAULT_EDIT_LIST_ITEM_EDIT_LEFT, DEFAULT_EDIT_LIST_ITEM_EDIT_TOP,
                             DEFAULT_EDIT_LIST_ITEM_EDIT_WIDTH, DEFAULT_EDIT_LIST_ITEM_EDIT_HEIGHT);

        QPalette pal = m_pEdit->palette();

        pal.setColor(QPalette::Base, WidgetTransparentColor);
        pal.setColor(QPalette::Text, THEME_TEXT_COLOR);
        pal.setColor(QPalette::Highlight, THEME_EDIT_HL_COLOR);
        pal.setColor(QPalette::HighlightedText, THEME_EDIT_HL_TEXT_COLOR);

        m_pEdit->setPalette(pal);
    }
}

CEditListItem::~CEditListItem(void)
{
    if (NULL != m_pEdit)
    {
        qUninstallIME(m_pEdit);
    }
}

int CEditListItem::GetEditListItemType()
{
    return (LIST_ITEM_TYPE_DETAIL | LIST_ITEM_SUB_TYPE_DETAIL_EDIT);
}

CEditListItem* CEditListItem::GetEditListItem(CListItemPtr pItem)
{
    if (NULL == pItem || GetEditListItemType() != pItem->GetType())
    {
        return NULL;
    }

    return (CEditListItem*)pItem;
}

void CEditListItem::SetEditRect(int nLeft, int nTop, int nWidth, int nHeight)
{
    QRect rcEdit(nLeft, nTop, nWidth, nHeight);

    SetEditRect(rcEdit);
}

void CEditListItem::SetEditRect(const QRect & rcEdit)
{
    if (NULL != m_pEdit)
    {
        m_pEdit->setGeometry(rcEdit);
    }
}

void CEditListItem::SetEditBgRect(int nLeft, int nTop, int nWidth, int nHeight)
{
    m_rcEditBG = QRect(nLeft, nTop, nWidth, nHeight);
}

void CEditListItem::SetEditBgRect(const QRect & rcEdit)
{
    m_rcEditBG = rcEdit;
}

void CEditListItem::SetEditBg(const char * pszEditBg)
{
    if (NULL == pszEditBg)
    {
        m_strBg = "";
    }
    else
    {
        m_strBg = pszEditBg;
    }
}

void CEditListItem::SetEditBg(const yl::string & strEditBg)
{
    m_strBg = strEditBg;
}

void CEditListItem::SetEditFocus()
{
    if (NULL != m_pEdit)
    {
        m_pEdit->setFocus(Qt::MouseFocusReason);
    }
}

void CEditListItem::SetReadOnly(bool bReadOnly)
{
    if (NULL != m_pEdit)
    {
        m_pEdit->setReadOnly(bReadOnly);
    }
}

void CEditListItem::SetTitleWidth(int nTitleWidth)
{
    int nOriTitleRight = m_rcTitle.right();

    m_rcTitle.setWidth(nTitleWidth);

    int nEditBgWidth = m_rcEditBG.width();
    int nEditBgLeft = m_rcTitle.right() + m_rcEditBG.left() - nOriTitleRight;

    m_rcEditBG.setLeft(nEditBgLeft);
    m_rcEditBG.setWidth(nEditBgWidth);

    if (NULL != m_pEdit)
    {
        QRect rcEdit = m_pEdit->geometry();
        int nEditWidht = rcEdit.width();
        int nEditLeft = m_rcTitle.right() + rcEdit.left() - nOriTitleRight;

        rcEdit.setLeft(nEditLeft);
        rcEdit.setWidth(nEditWidht);
        m_pEdit->setGeometry(rcEdit);
    }
}

void CEditListItem::SetLayout(bool bFrameLayout)
{
    m_bFrameLayout = bFrameLayout;

    if (bFrameLayout)
    {
        LayoutByFrameList();
    }
    else
    {
        SetDefaultTitlePos();

        m_rcEditBG.setRect(DEFAULT_EDIT_LIST_ITEM_EDIT_BG_LEFT, DEFAULT_EDIT_LIST_ITEM_EDIT_BG_TOP,
                           DEFAULT_EDIT_LIST_ITEM_EDIT_BG_WIDTH, DEFAULT_EDIT_LIST_ITEM_EDIT_BG_HEIGHT);

        if (NULL != m_pEdit)
        {
            m_pEdit->setGeometry(DEFAULT_EDIT_LIST_ITEM_EDIT_LEFT, DEFAULT_EDIT_LIST_ITEM_EDIT_TOP,
                                 DEFAULT_EDIT_LIST_ITEM_EDIT_WIDTH, DEFAULT_EDIT_LIST_ITEM_EDIT_HEIGHT);
        }
    }
}

void CEditListItem::Relayout()
{
    if (m_bFrameLayout)
    {
        LayoutByFrameList();
    }
}

void CEditListItem::LayoutByFrameList()
{
    if (NULL == m_pDelegate)
    {
        return;
    }

    int nLeftMargin = 0;
    int nRightMargin = 0;

    m_pDelegate->GetContentPos(nLeftMargin, nRightMargin);

    QRect rcItem = rect();
    int nTitleWidth = m_rcTitle.width();
    int nTitleHeight = m_rcTitle.height();
    int nLeftOffSet = nLeftMargin - GetDefaultTitleLeft();
    int nTitleExtraWidth = nTitleWidth - GetDefaultTitleWidth();
    int nTitleTop = (rcItem.height() - m_rcTitle.height()) / 2;

    m_rcTitle.setTop(nTitleTop);
    m_rcTitle.setHeight(nTitleHeight);
    m_rcTitle.setLeft(nLeftMargin);
    m_rcTitle.setWidth(nTitleWidth);

    int nEditBGTop = (rcItem.height() - m_rcEditBG.height()) / 2;
    int nEditBGHeight = m_rcEditBG.height();
    int nEditBgWidth = m_rcEditBG.width();

    m_rcEditBG.setTop(nEditBGTop);
    m_rcEditBG.setHeight(nEditBGHeight);
    m_rcEditBG.setLeft(DEFAULT_EDIT_LIST_ITEM_EDIT_BG_LEFT + nTitleExtraWidth + nLeftOffSet);
    m_rcEditBG.setWidth(nEditBgWidth);

    if (NULL != m_pEdit)
    {
        QRect rcEdit = m_pEdit->geometry();
        int nEditHeight = rcEdit.height();
        int nEditWidth = rcEdit.width();
        int nEditTop = (rcItem.height() - rcEdit.height()) / 2;

        rcEdit.setTop(nEditTop);
        rcEdit.setHeight(nEditHeight);
        rcEdit.setLeft(DEFAULT_EDIT_LIST_ITEM_EDIT_LEFT + nTitleExtraWidth + nLeftOffSet);
        rcEdit.setWidth(nEditWidth);
        m_pEdit->setGeometry(rcEdit);
    }
}

void CEditListItem::ReturnAllInput()
{
    if (NULL != m_pEdit)
    {
        qReturnEdit(m_pEdit);
    }
}

void CEditListItem::SetFocus(bool bFocus)
{
    CDetailListItemBase::SetFocus(bFocus);

    if (NULL == m_pEdit || m_pEdit->isReadOnly())
    {
        return;
    }

    if (bFocus)
    {
        if (!m_pEdit->hasFocus())
        {
            m_pEdit->setFocus(Qt::MouseFocusReason);
        }
        qSetIsShowWnd(m_pEdit, true);
    }
    else
    {
        m_pEdit->clearFocus();
    }
}

void CEditListItem::SetValue(const QString & strValue)
{
    if (NULL != m_pEdit)
    {
        m_pEdit->setText(strValue);
    }
}

QString CEditListItem::GetValue() const
{
    if (NULL != m_pEdit)
    {
        return m_pEdit->text();
    }

    return QLatin1String("");
}

QLineEdit * CEditListItem::GetLineEdit() const
{
    return m_pEdit;
}

void CEditListItem::InstallEditFilter(QObject * pObject)
{
    if (NULL == pObject || NULL == m_pEdit)
    {
        return;
    }

    m_pEdit->installEventFilter(pObject);
}

void CEditListItem::SetInputInfo(EDIT_TYPE eType, LPCSTR lpszIMEName, LPCSTR lpszDefault)
{
    if (NULL == m_pEdit)
    {
        return;
    }

    qInstallIME(m_pEdit, lpszIMEName);
    qSetCurrentIME(m_pEdit, lpszDefault);

    qUseKeyBoardFromIME(m_pEdit, INPUT_DIALOG_NEWLINE, true);

    qLineEditUtility::SetMaxLength(m_pEdit, eType);
}

void CEditListItem::SetMaxLength(int nMaxLength)
{
    if (NULL == m_pEdit)
    {
        return;
    }

    m_pEdit->setMaxLength(nMaxLength);
}

void CEditListItem::mousePressEvent(QMouseEvent * pEvent)
{
    if (NULL == m_pEdit || !m_pEdit->isEnabled() || m_pEdit->isReadOnly())
    {
        return;
    }

    QRect rcEdit = m_pEdit->geometry();
    const QPoint & pt = pEvent->pos();

    if (!rcEdit.contains(pt))
    {
        return;
    }

    CListItem::mousePressEvent(pEvent);
}

void CEditListItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    // 画标题
    PaintTitle(stylePainter);

    // 画输入框背景
    PaintEdit(stylePainter);
}

void CEditListItem::PaintEdit(QStylePainter & stylePainter, const char * pszBG/* = NULL*/)
{
    if (NULL != pszBG)
    {
        QPixmap pmBG = THEME_GET_BMP(pszBG);

        if (!pmBG.isNull())
        {
            stylePainter.drawPixmap(m_rcEditBG, pmBG);
        }
    }
    else
    {
        if (m_strBg.empty())
        {
            yl::string strBg = PIC_DIRECTORY_EDIT_CONTACT_INFO;

            if (NULL != m_pEdit)
            {
                if (m_pEdit->isReadOnly())
                {
                    return;
                }

                if (m_pEdit->hasFocus())
                {
                    strBg = PIC_DIRECTORY_EDIT_CONTACT_INFO_FOCUS;
                }
            }

            QPixmap pmBG = THEME_GET_BMP(strBg.c_str());

            if (!pmBG.isNull())
            {
                stylePainter.drawPixmap(m_rcEditBG, pmBG);
            }
        }
        else
        {
            QPixmap pmBG = THEME_GET_BMP(m_strBg.c_str());

            if (!pmBG.isNull())
            {
                stylePainter.drawPixmap(m_rcEditBG, pmBG);
            }
        }
    }
}
