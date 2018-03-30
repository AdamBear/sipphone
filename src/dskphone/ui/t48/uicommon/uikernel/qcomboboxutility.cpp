#include "qcomboboxutility.h"
#include "languagehelper.h"
#include "scrollpage.h"
#include "include/uicommon.h"
#include <QAbstractItemView>
#include <QStylePainter>
#include "qtcommon/qmisc.h"
#include "imagemanager/modimagemanager.h"
#include "baseui/t4xpicpath.h"
/****************************************************
实现新的ComboBox的类和函数
***************************************************/

CMyComboBoxDelegate::CMyComboBoxDelegate(QComboBox * pComboBox, int iHeight, QObject * parent)
    : QItemDelegate(parent)
    , m_iHeight(iHeight)
    , m_pComboBox(pComboBox)
{
    if (NULL != m_pComboBox)
    {
        connect(m_pComboBox, SIGNAL(destroyed(QObject *)), this, SLOT(OnComboBoxDestroy(QObject *)));
    }
}

CMyComboBoxDelegate::~CMyComboBoxDelegate()
{

}

void CMyComboBoxDelegate::OnComboBoxDestroy(QObject * pComboBox)
{
    if (NULL != m_pComboBox)
    {
        m_pComboBox = NULL;
        deleteLater();
    }
}

QSize CMyComboBoxDelegate::sizeHint(const QStyleOptionViewItem & option,
                                    const QModelIndex & index) const
{
    return QSize(1, m_iHeight); // the row height
}

void CMyComboBoxDelegate::drawDisplay(QPainter * painter, const QStyleOptionViewItem & option,
                                      const QRect & rect, const QString & text) const
{
    QItemDelegate::drawDisplay(painter, option, rect, LANG_PARSE(text));
}

void CMyComboBoxDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option,
                                const QModelIndex & index)const
{
    // 保存画笔
    painter->save();
    int nRow = index.row();

    // 设置选中项的背景
    QRect rect = option.rect;
    bool bHasFocus = option.state & QStyle::State_HasFocus;
    // 刷背景默认颜色为白色
    painter->fillRect(rect, QColor(0xe6, 0xe6, 0xe6));
    if (bHasFocus)
    {
        QPixmap pmSelBG; // 选中背景图片

        if (NULL != m_pComboBox)
        {
            bool bPopBelow = true;
            int nFirstRow = 0;

            QAbstractItemView * pView = m_pComboBox->view();

            if (NULL != pView && pView->isActiveWindow())
            {
                QPoint ptView = pView->mapToGlobal(QPoint(0, 0));
                QPoint ptComboBox = m_pComboBox->mapToGlobal(QPoint(0, 0));

                if (ptComboBox.y() > ptView.y())
                {
                    bPopBelow = false;
                }

                QScrollBar * pScrollBar = pView->verticalScrollBar();

                if (NULL != pScrollBar)
                {
                    nFirstRow = pScrollBar->value();
                }
            }

            if (bPopBelow)
            {
                int nMaxRow = m_pComboBox->maxVisibleItems() + 1;
                int nCount = m_pComboBox->count();

                if (nMaxRow > nCount)
                {
                    nMaxRow = nCount;
                }

                if ((nRow + 1) == (nMaxRow + nFirstRow))
                {
                    pmSelBG = THEME_GET_BMP(PIC_GENERAL_COMBO_LIST_ITEM_BG_BOTTOM);
                }
            }
            else if (nFirstRow == nRow)
            {
                pmSelBG = THEME_GET_BMP(PIC_GENERAL_COMBO_LIST_ITEM_BG_TOP);
            }
        }

        if (pmSelBG.isNull())
        {
            pmSelBG = THEME_GET_BMP(PIC_GENERAL_COMBO_LIST_ITEM_BG_MID);
        }

        painter->drawPixmap(option.rect, pmSelBG);
    }
    // 不是第一个绘制分割线
    if (0 != nRow)
    {
        painter->setPen(QColor(0xca, 0xca, 0xca));
        painter->drawLine(rect.topLeft(), rect.topRight());
    }

    //
    // 设置文字颜色
    QString text = index.data(Qt::DisplayRole).toString();
    rect.adjust(3, 0, 0, 0);
    painter->setPen(bHasFocus ? QColor(Qt::white) : QColor(Qt::black));

    //进行字符串的解析翻译及重组
    text = CutOffText(LANG_PARSE(text), painter->fontMetrics(), rect.width() - 3);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text);

    // 还原画笔
    painter->restore();
}

qComboBoxUtility::qComboBoxUtility(void)
{
}

qComboBoxUtility::~qComboBoxUtility(void)
{
}

void qComboBoxUtility::SetCmbGeometry(QComboBox * pCombobox, QRect rcCmb)
{
    if (NULL == pCombobox)
    {
        return;
    }

    rcCmb.setWidth(rcCmb.width() + 4);
    rcCmb.moveLeft(rcCmb.topLeft().x() - 2);
    pCombobox->setGeometry(rcCmb);
}

void qComboBoxUtility::SetComboBoxStyle(QComboBox * pComboBox, const QColor & color)
{
    if (pComboBox == NULL)
    {
        return;
    }

    QPalette pal = pComboBox->palette();
    pComboBox->setGeometry(0, 0, 250, 36);
    pal.setColor(QPalette::Text, color);
    pComboBox->setPalette(pal);
    SetUpComboBox(pComboBox);
}

// 设置ComBobox
void qComboBoxUtility::SetUpComboBox(QComboBox * pCombobox)
{
    if (pCombobox == NULL)
    {
        return ;
    }

    // 设置Combobox的位置
    SetCmbGeometry(pCombobox, pCombobox->geometry());

    // 设置每个Item的高
    pCombobox->setItemDelegate(new CMyComboBoxDelegate(pCombobox));

    // 设置最多显示几个Item
    pCombobox->setMaxVisibleItems(CMB_MAX_VISIBLE_ITEM);

    CCmbScrollBar * pScrollBar = new CCmbScrollBar();
    if (NULL != pScrollBar)
    {
        QAbstractItemView * pItemView = pCombobox->view();

        if (NULL == pItemView)
        {
            return;
        }

        QWidget * pContainer = (QWidget *)pItemView->parent();

        if (NULL != pContainer)
        {
            // Windows上会圆角会显示黑图，在话机上不会
            pContainer->setAttribute(Qt::WA_TranslucentBackground, true);
        }

        pItemView->setVerticalScrollBar(pScrollBar);// 将控件内的竖直的滚动条设置

        pScrollBar->SetBackgroundBrush(QColor(0x00, 0x00, 0x00, 0x00));// 设置背景透明为CMyScrollBar

        ViewScrollBarFilter * pFilter = new ViewScrollBarFilter(pScrollBar, pScrollBar);
        if (pFilter != NULL)
        {
            pFilter->SetSingleStep(CMB_MAX_VISIBLE_ITEM);
            pScrollBar->installEventFilter(pFilter);// 给滚动条安装特殊的过滤器
        }
    }
}

void qComboBoxUtility::SetComboBoxSelectText(QComboBox * pCombobox, const QString & strText)
{
    if (NULL == pCombobox)
    {
        return;
    }

    int nIndex = -1;
    int nCount = pCombobox->count();

    for (int i = 0; i < nCount; ++i)
    {
        QString strItemText = pCombobox->itemText(i);

        if (strItemText == strText)
        {
            nIndex = i;
            break;
        }
    }

    if (nIndex >= 0)
    {
        pCombobox->setCurrentIndex(nIndex);
    }
}

void qComboBoxUtility::SwitchItem(QComboBox * pComboBox, bool bSelectPre)
{
    if (NULL == pComboBox)
    {
        return;
    }

    int nCurIndex = pComboBox->currentIndex();
    int nCount = pComboBox->count();
    int nIndex = nCurIndex;

    if (bSelectPre)
    {
        --nIndex;
    }
    else
    {
        ++nIndex;
    }

    if (nIndex < 0)
    {
        nIndex = nCount - 1;
    }
    else if (nIndex >= nCount)
    {
        nIndex = 0;
    }

    if (nIndex >= 0 && nIndex < nCount && nIndex != nCurIndex)
    {
        pComboBox->setCurrentIndex(nIndex);
    }
}

bool qComboBoxUtility::OnLeftPressEventFilter(QObject * pObj, QEvent * pEvent,
        QWidget * pFocusWidget)
{
    if (NULL != pFocusWidget && pFocusWidget->inherits("QComboBox"))
    {
        // 有下拉框时，左键无效，也不传给其他窗口处理
        if (pFocusWidget == pObj)
        {
            SwitchItem((QComboBox *)pFocusWidget, true);
        }

        return true;
    }

    return false;
}

bool qComboBoxUtility::OnRightPressEventFilter(QObject * pObj, QEvent * pEvent,
        QWidget * pFocusWidget)
{
    if (NULL != pFocusWidget && pFocusWidget->inherits("QComboBox"))
    {
        // 有下拉框时，右键无效，也不传给其他窗口处理
        if (pFocusWidget == pObj)
        {
            SwitchItem((QComboBox *)pFocusWidget, false);
        }

        return true;
    }

    return false;
}
