#include <QtGui>
#include "uikernel/ilistitemdelegate.h"
#include "comboboxlistitem.h"
#include "t4xpicpath.h"
#include "uicommon.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/qcomboboxutility.h"
#include "uikernel/qwidgetutility.h"

namespace
{
// 输入框位置信息
#define DEFAULT_COMBOBOX_LIST_ITEM_COMBOX_LEFT 141
#define DEFAULT_COMBOBOX_LIST_ITEM_COMBOX_TOP 1
#define DEFAULT_COMBOBOX_LIST_ITEM_COMBOX_WIDTH 212
#define DEFAULT_COMBOBOX_LIST_ITEM_COMBOX_HEIGHT 36

#define DEFAULT_COMBOBOX_LIST_ITEM_FONT_SIZE 16
}

CComboBoxListItem::CComboBoxListItem(QWidget * parent/* = NULL*/,
                                     IListItemAction * pAction/* = NULL*/)
    : CDetailListItemBase(parent, pAction)
    , m_pComboBox(NULL)
    , m_bFrameLayout(false)
    , m_bOnlyShowText(false)
    , m_strBg("")
{
    setObjectName("CComboBoxListItem");

    m_nType |= LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX;

    qWidgetUtility::setFontStyle(this, DEFAULT_COMBOBOX_LIST_ITEM_FONT_SIZE);

    m_pComboBox = new qComboBoxEx(this);

    if (NULL != m_pComboBox)
    {
        m_pComboBox->setObjectName("ComboBox");
        qComboBoxUtility::SetUpComboBox(m_pComboBox);

        m_pComboBox->setGeometry(DEFAULT_COMBOBOX_LIST_ITEM_COMBOX_LEFT,
                                 DEFAULT_COMBOBOX_LIST_ITEM_COMBOX_TOP,
                                 DEFAULT_COMBOBOX_LIST_ITEM_COMBOX_WIDTH, DEFAULT_COMBOBOX_LIST_ITEM_COMBOX_HEIGHT);

    }
}

CComboBoxListItem::~CComboBoxListItem(void)
{
}

int CComboBoxListItem::GetComboBoxListItemType()
{
    return (LIST_ITEM_TYPE_DETAIL | LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX);
}

CComboBoxListItem* CComboBoxListItem::GetComboBoxListItem(CDetailListItemBase* pItem)
{
    if (NULL == pItem || GetComboBoxListItemType() != pItem->GetType())
    {
        return NULL;
    }

    return (CComboBoxListItem*)pItem;
}

void CComboBoxListItem::SetComboBoxRect(int nLeft, int nTop, int nWidth, int nHeight)
{
    QRect rcEdit(nLeft, nTop, nWidth, nHeight);

    SetComboBoxRect(rcEdit);
}

void CComboBoxListItem::SetComboBoxRect(const QRect & rcComboBox)
{
    if (NULL != m_pComboBox)
    {
        m_pComboBox->setGeometry(rcComboBox);
    }
}

void CComboBoxListItem::SetFocus(bool bFocus)
{
    CDetailListItemBase::SetFocus(bFocus);

    if (NULL == m_pComboBox || m_bOnlyShowText)
    {
        return;
    }

    if (bFocus)
    {
        m_pComboBox->setFocus(Qt::MouseFocusReason);
    }
    else
    {
        m_pComboBox->clearFocus();
    }
}

void CComboBoxListItem::ClearValue()
{
    if (NULL != m_pComboBox)
    {
        m_pComboBox->clear();
    }
}

void CComboBoxListItem::AddValue(const char * pszValue, const QVariant & auserData)
{
    QString strValue = QLatin1String("");

    if (NULL != pszValue)
    {
        strValue = pszValue;
    }

    AddValue(strValue, auserData);
}

void CComboBoxListItem::AddValue(const QString & strValue, const QVariant & auserData)
{
    if (NULL != m_pComboBox)
    {
        m_pComboBox->addItem(strValue, auserData);
    }
}

void CComboBoxListItem::SetValue(const QString & strValue)
{
    if (NULL != m_pComboBox)
    {
        qComboBoxUtility::SetComboBoxSelectText(m_pComboBox, strValue);
    }
}

void CComboBoxListItem::SetItemData(const yl::string & strData, int nDefaultIndex/* = 0*/)
{
    if (NULL == m_pComboBox)
    {
        return;
    }

    int nIndex = -1;
    int nCount = m_pComboBox->count();

    for (int i = 0; i < nCount; ++i)
    {
        yl::string strItemData = m_pComboBox->itemData(i).value<yl::string>();

        if (strItemData == strData)
        {
            nIndex = i;
            break;
        }
    }

    if (nIndex >= 0)
    {
        m_pComboBox->setCurrentIndex(nIndex);
    }
    else if (nDefaultIndex >= 0 && nDefaultIndex < nCount)
    {
        m_pComboBox->setCurrentIndex(nDefaultIndex);
    }
}

void CComboBoxListItem::SetItemData(int nData, int nDefaultIndex/* = 0*/)
{
    if (NULL == m_pComboBox)
    {
        return;
    }

    int nIndex = -1;
    int nCount = m_pComboBox->count();

    for (int i = 0; i < nCount; ++i)
    {
        int nItemData = m_pComboBox->itemData(i).value<int>();

        if (nItemData == nData)
        {
            nIndex = i;
            break;
        }
    }

    if (nIndex >= 0)
    {
        m_pComboBox->setCurrentIndex(nIndex);
    }
    else if (nDefaultIndex >= 0 && nDefaultIndex < nCount)
    {
        m_pComboBox->setCurrentIndex(nDefaultIndex);
    }
}

bool CComboBoxListItem::IsHadItemData(const yl::string & strData)
{
    if (NULL == m_pComboBox)
    {
        return false;
    }

    int nCount = m_pComboBox->count();

    for (int i = 0; i < nCount; ++i)
    {
        yl::string strItemData = m_pComboBox->itemData(i).value<yl::string>();

        if (strItemData == strData)
        {
            return true;
        }
    }

    return false;
}

void CComboBoxListItem::SetCurrentIndex(int nIndex)
{
    if (NULL == m_pComboBox || nIndex < 0)
    {
        return;
    }

    m_pComboBox->setCurrentIndex(nIndex);
}

QString CComboBoxListItem::GetValue() const
{
    if (NULL != m_pComboBox)
    {
        return m_pComboBox->currentText();
    }

    return QLatin1String("");
}

qComboBoxEx * CComboBoxListItem::GetComboBox() const
{
    return m_pComboBox;
}

yl::string CComboBoxListItem::GetItemStringData() const
{
    if (NULL == m_pComboBox)
    {
        return yl::string("");
    }

    int nCurIndex = m_pComboBox->currentIndex();

    return m_pComboBox->itemData(nCurIndex).value<yl::string>();
}

int CComboBoxListItem::GetItemData() const
{
    if (NULL == m_pComboBox)
    {
        return 0;
    }

    int nCurIndex = m_pComboBox->currentIndex();

    return m_pComboBox->itemData(nCurIndex).value<int>();
}

void CComboBoxListItem::SetTitleWidth(int nTitleWidth)
{
    int nOriTitleRight = m_rcTitle.right();

    m_rcTitle.setWidth(nTitleWidth);

    if (NULL != m_pComboBox)
    {
        QRect rcComboBox = m_pComboBox->geometry();
        int nComboBoxWidth = rcComboBox.width();
        int nComboBoxLeft = m_rcTitle.right() + rcComboBox.left() - nOriTitleRight;

        rcComboBox.setLeft(nComboBoxLeft);
        rcComboBox.setWidth(nComboBoxWidth);
        m_pComboBox->setGeometry(rcComboBox);
    }
}

void CComboBoxListItem::InstallComboBoxFilter(QObject * pObject)
{
    if (NULL == pObject || NULL == m_pComboBox)
    {
        return;
    }

    m_pComboBox->installEventFilter(pObject);
}

void CComboBoxListItem::SetLayout(bool bFrameLayout)
{
    m_bFrameLayout = bFrameLayout;

    if (bFrameLayout)
    {
        LayoutByFrameList();
    }
    else
    {
        SetDefaultTitlePos();

        if (NULL != m_pComboBox)
        {
            m_pComboBox->setGeometry(DEFAULT_COMBOBOX_LIST_ITEM_COMBOX_LEFT,
                                     DEFAULT_COMBOBOX_LIST_ITEM_COMBOX_TOP,
                                     DEFAULT_COMBOBOX_LIST_ITEM_COMBOX_WIDTH, DEFAULT_COMBOBOX_LIST_ITEM_COMBOX_HEIGHT);
        }
    }
}

bool CComboBoxListItem::IsOnlyShowText() const
{
    return m_bOnlyShowText;
}

void CComboBoxListItem::CheckOnlyShowText(bool bOnlyText)
{
    if (NULL == m_pComboBox)
    {
        return;
    }

    if (bOnlyText && m_pComboBox->count() <= 1)
    {
        m_bOnlyShowText = true;
        m_pComboBox->hide();
    }
    else
    {
        m_bOnlyShowText = false;
    }

    if (!m_bOnlyShowText)
    {
        m_pComboBox->raise();
        m_pComboBox->show();
    }
}

void CComboBoxListItem::Relayout()
{
    if (m_bFrameLayout)
    {
        LayoutByFrameList();
    }
}

void CComboBoxListItem::LayoutByFrameList()
{
    if (NULL == m_pDelegate)
    {
        return;
    }

    int nLeftMargin = 0;
    int nRightMargin = 0;

    m_pDelegate->GetContentPos(nLeftMargin, nRightMargin);

    QRect rcItem = rect();
    int nTitleHeight = m_rcTitle.height();
    int nTitleWidth = m_rcTitle.width();
    int nLeftOffSet = nLeftMargin - GetDefaultTitleLeft();
    int nTitleExtraWidth = nTitleWidth - GetDefaultTitleWidth();
    int nTitleTop = (rcItem.height() - m_rcTitle.height()) / 2;

    m_rcTitle.setTop(nTitleTop);
    m_rcTitle.setHeight(nTitleHeight);
    m_rcTitle.setLeft(nLeftMargin);
    m_rcTitle.setWidth(nTitleWidth);

    if (NULL != m_pComboBox)
    {
        QRect rcComboBox = m_pComboBox->geometry();
        int nComboBoxHeight = rcComboBox.height();
        int nComboBoxWidth = rcComboBox.width();
        int nComboBoxTop = (rcItem.height() - rcComboBox.height()) / 2;

        rcComboBox.setTop(nComboBoxTop);
        rcComboBox.setHeight(nComboBoxHeight);
        rcComboBox.setLeft(DEFAULT_COMBOBOX_LIST_ITEM_COMBOX_LEFT + nTitleExtraWidth + nLeftOffSet);
        rcComboBox.setWidth(nComboBoxWidth);

        m_pComboBox->setGeometry(rcComboBox);
    }
}

void CComboBoxListItem::mousePressEvent(QMouseEvent * pEvent)
{
    if (m_bOnlyShowText || NULL == m_pComboBox || !m_pComboBox->isEnabled())
    {
        return;
    }

    QRect rcComboBox = m_pComboBox->geometry();
    const QPoint & pt = pEvent->pos();

    if (!rcComboBox.contains(pt))
    {
        return;
    }

    CListItem::mousePressEvent(pEvent);
}

void CComboBoxListItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    // 画标题
    PaintTitle(stylePainter);

    if (m_bOnlyShowText && NULL != m_pComboBox)
    {
        QRect rcComboBox = m_pComboBox->geometry();

        stylePainter.drawItemText(rcComboBox, (Qt::AlignLeft | Qt::AlignVCenter),
                                  palette(), isEnabled(), m_pComboBox->currentText());
    }
}
