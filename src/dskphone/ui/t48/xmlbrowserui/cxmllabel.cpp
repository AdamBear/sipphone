#include "cxmllabel.h"
#include <QStylePainter>
#include <ETLLib.hpp>
#include "extended/xmlbrowser/include/logicanduicommon.h"
#include "textstylemanager.h"
#include "qtcommon/qmisc.h"

const QSize LAB_ICON_SIZE(32, 32);
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define XML_LABEL_DEFAULT_FIX_WIDTH 200
#define AUTO_RESIZE_VIEW_MAX_ROW_DEFAULT    2000

/************************************************************************/
/*                                                                      */
/************************************************************************/
AutoResizeLab::AutoResizeLab(QWidget * parent/* = 0*/)
    : QLabel(parent)
    , m_iCellRowHeight(0)
    , m_bOpenAutoResize(true)
    , m_iMinimumHeight(1)
    , m_bDrawIcon(false)
    , m_iFixWidth(XML_LABEL_DEFAULT_FIX_WIDTH)
{
    // 默认设置为多行显示
    this->setWordWrap(true);

    this->setObjectName("AutoResizeLab");
}

AutoResizeLab::AutoResizeLab(int iFixWidth, int iRowHeight, QWidget * parent/* = 0*/)
    : m_iFixWidth(iFixWidth)
    , m_iCellRowHeight(iRowHeight)
    , m_bOpenAutoResize(true)
    , m_iMinimumHeight(1)
    , QLabel(parent)
    , m_bDrawIcon(false)
{
    // 默认设置为多行显示
    this->setWordWrap(true);

    this->setObjectName("AutoResizeLab");
}

AutoResizeLab::~AutoResizeLab()
{

}

void AutoResizeLab::OpenAutoResize(bool bOpen)
{
    m_bOpenAutoResize = bOpen;
}

void AutoResizeLab::SetFixWidth(int iFixWidth)
{
    if (iFixWidth > 0)
    {
        m_iFixWidth = iFixWidth;
    }
}

void AutoResizeLab::SetCellRowHeight(int iCellRowHeight)
{
    if (iCellRowHeight > -1)
    {
        m_iCellRowHeight = iCellRowHeight;
    }
}

// 设置控件的最小高度
void AutoResizeLab::SetMinimumHeight(int iMinimumHeight)
{
    if (iMinimumHeight > 0)
    {
        m_iMinimumHeight = iMinimumHeight;
    }
}

// Lab中每行文字的高度
int AutoResizeLab::FontlineSpacing()
{
    return this->fontMetrics().lineSpacing();
}

// 设置图片的路径
void AutoResizeLab::SetIconPath(const yl::string & strIconPath)
{
    m_pmIcon.load(strIconPath.c_str());
}

// 是否绘制小图标
void AutoResizeLab::SetDrawIcon(bool bDraw)
{
    m_bDrawIcon = bDraw;
}

// 根据高度重新设置控件的Text
// 即让控件的Text显示在iHeight的高度内,超出的部分进行截断
void AutoResizeLab::ResetSetTextByHeight(int iHeight)
{
    // 若传入的高度大于控件的高度则说明足够显示所有text
    if (iHeight > this->height())
    {
        return;
    }

    if (iHeight < FontlineSpacing())
    {
        // 若传入的高度小于一行则设置其Text为空
        this->setText("");
        this->SetDrawIcon(false);
    }
    else
    {
        QString strTemp = this->text();
        QString strFirstRowText;
        if (m_bDrawIcon)
        {
            iHeight -= GetFirstRowHeight();
            strFirstRowText = cutTextForWidth(this->text(), this->fontMetrics(),
                                              m_iFixWidth - LAB_ICON_SIZE.width());

            // 计算出了第一行外的其他Text
            strTemp = strTemp.remove(0, strFirstRowText.length() + 1);// +1是因为尾部有个空格
        }

        int iCutIndex = 0;
        int iIndex = strTemp.indexOf(" ", 0);

        // 计算出截断前的index
        while (iIndex > 0)
        {
            if (CountTextHeight(strTemp.left(iIndex)) > iHeight)
            {
                // 若字体的高度大于输入的显示高度则跳出循环
                break;
            }
            iCutIndex = iIndex;
            iIndex = strTemp.indexOf(" ", iIndex + 1);// 寻找下一个单词的Index
        }

        if (iIndex < 0)
        {
            if (CountTextHeight(strTemp) < iHeight)
            {
                return;
            }
        }

        // 将超出的Text全部替换为空格
        int iStrLen = strTemp.size();
        QString strEmpty(iStrLen - iCutIndex, ' ');
        strTemp.replace(iCutIndex, iStrLen - iCutIndex, strEmpty);
        this->setText(strFirstRowText + " " + strTemp);
    }
}

void AutoResizeLab::SetLabText(const QString & strText)
{
    // 判断是否开启自适应
    if (!m_bOpenAutoResize)
    {
        this->setText(replaceLabelText(strText, this));
        return;
    }

    int iHeight = 0;
    QString strTemp = strText;
    bool bWarpWord = this->wordWrap();

    ////----临时代码需要整理
    if (!bWarpWord)// Text不换行,即为一行显示
    {
        iHeight += CountTextHeight(strTemp);
        iHeight = max(iHeight, m_iMinimumHeight);

        this->resize(m_iFixWidth, iHeight);

        if (m_bDrawIcon)
        {
            this->setText(replaceTextByWidth(strText, this->width() - LAB_ICON_SIZE.width(),
                                             this->fontMetrics()));
        }
        else
        {
            this->setText(replaceLabelText(strText, this));
        }

        return;
    }

    if (m_bDrawIcon && LAB_ICON_SIZE.width() < m_iFixWidth)
    {
        // 获得第一行的Text
        QString strRemoveText = cutTextForWidth(strTemp, this->fontMetrics(),
                                                m_iFixWidth - LAB_ICON_SIZE.width());

        // 判断第一行能否显示全部的Text
        if (strRemoveText != strText)
        {
            // 加上第一行的高度
            iHeight += GetFirstRowHeight();

            // 移除第一行的Text
            if (strRemoveText.length() != 0)
            {
                strTemp.remove(0, strRemoveText.length());
            }
        }
    }

    // 计算Text在Lab中的显示区域的大小
    int iTextHeight = CountTextHeight(strTemp);

    if (m_iCellRowHeight == 0)
    {
        // 完全适应字体高度
        iHeight += iTextHeight;
    }
    else
    {
        // 在网格内适应字体高度
        int nCount = iTextHeight / m_iCellRowHeight + 1;
        iHeight += m_iCellRowHeight * nCount;
    }

    iHeight = max(iHeight, m_iMinimumHeight);

    this->resize(m_iFixWidth, iHeight);
    this->setText(replaceLabelText(strText, this));
}

int AutoResizeLab::GetFirstRowHeight()
{
    int iHeight = 0;
    if (m_iCellRowHeight == 0)
    {
        iHeight = FontlineSpacing();
    }
    else
    {
        iHeight = m_iCellRowHeight;
    }

    return max(iHeight, m_iMinimumHeight);
}

int AutoResizeLab::CountTextHeight(const QString & strText)
{
    int flags = this->wordWrap() ? (int)(alignment() | Qt::TextWordWrap) : (int)alignment();
    return this->fontMetrics().boundingRect(QRect(0, 0, m_iFixWidth, 30), flags, strText).height();
}

void AutoResizeLab::paintEvent(QPaintEvent * pEvent)
{
    if (m_bDrawIcon)
    {
        QStylePainter p(this);
        Qt::Alignment m_eAlign = this->alignment();
        bool isAlignLeft = (m_eAlign & Qt::AlignLeft) == Qt::AlignLeft;
        bool isAlignRight = (m_eAlign & Qt::AlignRight) == Qt::AlignRight;

        // 获得第一行的Text
        QString strRemoveText = cutTextForWidth(this->text(), this->fontMetrics(),
                                                m_iFixWidth - LAB_ICON_SIZE.width());
        int nWidth = this->fontMetrics().width(strRemoveText);// 获得Text的宽度
        nWidth += LAB_ICON_SIZE.width();// 加上Icon的宽度

        // 计算绘制Icon的X轴坐标
        int x = 0;
        if (isAlignLeft)
        {
            x = 0;
        }
        else if (isAlignRight)
        {
            x = m_iFixWidth - nWidth;
        }
        else
        {
            x = (m_iFixWidth - nWidth) / 2;
        }

        if (x >= 0)
        {
            ////----
            p.drawPixmap(x, (GetFirstRowHeight() - LAB_ICON_SIZE.width()) / 2,
                         LAB_ICON_SIZE.width(), LAB_ICON_SIZE.height(), m_pmIcon);// 绘制Icon

            // 绘制第一行的文字
            p.drawText(x + LAB_ICON_SIZE.width(), 0,
                       this->width() - x - LAB_ICON_SIZE.width(),
                       GetFirstRowHeight(), Qt::AlignLeft | Qt::AlignVCenter, strRemoveText);

            // 绘制剩下的Text
            strRemoveText = this->text().remove(0, strRemoveText.length() + 1);// +1是因为尾部有个空格
            if (!strRemoveText.isEmpty())
            {
                p.drawText(0, GetFirstRowHeight(), this->width(), this->height(), m_eAlign | Qt::TextWordWrap,
                           strRemoveText);
            }
        }
        else
        {
            chWARNING2(false, "AutoResizeLab::paintEvent count icon pos error!");
        }
    }
    else
    {
        QLabel::paintEvent(pEvent);
    }
}

/************************************************************************/
/*                     about  AutoResizeListView                        */
/************************************************************************/
AutoResizeListView::AutoResizeListView(QWidget * parent/* = 0*/)
    : m_nItemFixWidth(0)
    , QFrame(parent)
    , m_bWrap(false)
    , m_iRowHeight(DEFAULT_ROW_HEIGHT)
    , m_iItemMiniHeight(1)
    , m_iMaxRow(AUTO_RESIZE_VIEW_MAX_ROW_DEFAULT)
{

}

AutoResizeListView::AutoResizeListView(int ItemFixWidth, QWidget * parent/* = 0*/)
    : m_nItemFixWidth(ItemFixWidth)
    , QFrame(parent)
    , m_bWrap(false)
    , m_iRowHeight(DEFAULT_ROW_HEIGHT)
    , m_iItemMiniHeight(1)
    , m_iMaxRow(AUTO_RESIZE_VIEW_MAX_ROW_DEFAULT)
{

}

AutoResizeListView::~AutoResizeListView()
{

}

void AutoResizeListView::SetViewData(const list<LineData *> & listLineData,
                                     bool bClear/* = true*/)
{
    list<LabDisplayData> listDisplayData;
    list<LineData *>::const_iterator it = listLineData.begin();
    for (/* */; it != listLineData.end(); ++it)
    {
        if (*it != NULL)
        {
            LineData * pLineData = *it;
            LabDisplayData objDisplayData;
            objDisplayData.fontAlignment = TypeToAlignmentFlag(pLineData->iAlign);//!!!!!
            objDisplayData.fontColor     = TypeToColor(pLineData->eColor);
            objDisplayData.iFontSize     = TypeToFontPointSize(pLineData->eFont);
            objDisplayData.strText       = pLineData->strText;
            listDisplayData.push_back(objDisplayData);
        }
    }

    SetViewData(listDisplayData, bClear);
}

void AutoResizeListView::SetViewData(const list<StatusItem> & listStatusData,
                                     bool bClear/* = true*/)
{
    list<LabDisplayData> listDisplayData;
    list<StatusItem>::const_iterator it = listStatusData.begin();
    for (/* */; it != listStatusData.end(); ++it)
    {
        StatusItem objStatusItem = *it;
        LabDisplayData objDisplayData;
        objDisplayData.fontAlignment = TypeToAlignmentFlag(objStatusItem.iAlign);
        objDisplayData.fontColor     = TypeToColor(objStatusItem.eColor);
        objDisplayData.iFontSize     = TypeToFontPointSize(objStatusItem.eSize);
        objDisplayData.strIconPath   = objStatusItem.strIconPath;
        objDisplayData.strText       = objStatusItem.strMsg;
        listDisplayData.push_back(objDisplayData);
    }

    SetViewData(listDisplayData, bClear);
}

// 设置Lab数据
void AutoResizeListView::SetViewData(const list<LabDisplayData> & listDisplayData,
                                     bool bClear/* = true*/)
{
    if (bClear)
    {
        ClearListData();
    }

    int iCountHeight = 0;// 用于计算已经插入的Lab的高度
    int iRowCount = 0;
    list<LabDisplayData>::const_iterator dataCIter = listDisplayData.begin();
    labeliter labIter = m_ListLabel.begin();

    // 先在已经存在的Lab上添加Text
    for (/**/; dataCIter != listDisplayData.end() && labIter != m_ListLabel.end()
             && iRowCount <= m_iMaxRow; ++dataCIter, ++labIter, iRowCount++)
    {
        AutoResizeLab * pResizeLab = *labIter;
        SetLabStyle(*dataCIter, pResizeLab);

        pResizeLab->setGeometry(0, 0 + iCountHeight, pResizeLab->width(), pResizeLab->height());
        iCountHeight += pResizeLab->height();
    }

    // 判断已经存在的Lab是否够用
    //数据未到最后一项，说明lab不够用，需要重新添加lab及计算高度
    if (dataCIter != listDisplayData.end())
    {
        while (dataCIter != listDisplayData.end() && iRowCount <= m_iMaxRow)
        {
            AutoResizeLab * pResizeLab = new AutoResizeLab(m_nItemFixWidth, m_iRowHeight, this);
            if (pResizeLab != NULL)
            {
                iRowCount++;
                SetLabStyle(*dataCIter, pResizeLab);
                pResizeLab->setGeometry(0, 0 + iCountHeight, pResizeLab->width(), pResizeLab->height());
                iCountHeight += pResizeLab->height();
                m_ListLabel.push_back(pResizeLab);
                ++dataCIter;
                pResizeLab->show();

                //添加了Label之后，要设置其大小
            }
        }
    }
    else if (labIter != m_ListLabel.end())
    {
        int nCountDeleteItem = 0;
        while (labIter != m_ListLabel.end())
        {
            AutoResizeLab * pResizeLab = *labIter;
            pResizeLab->deleteLater();
            *labIter = NULL;
            ++nCountDeleteItem;
            ++labIter;
        }

        while (nCountDeleteItem != 0)
        {
            m_ListLabel.pop_back();
            --nCountDeleteItem;
        }
    }
    else
    {
        // 如果走入这个条件,说明lab数正好够用不用做其他处理
    }

    // 重新设置控件的高度
    this->resize(this->width(), iCountHeight);
}

void AutoResizeListView::SetLabStyle(const LabDisplayData & objDisplayData, AutoResizeLab * pLabel)
{
    if (NULL == pLabel)
    {
        return;
    }

    // 设置文字的对齐方式
    pLabel->setAlignment(objDisplayData.fontAlignment);

    // 文字的颜色
    QPalette pal = pLabel->palette();
    pal.setBrush(QPalette::WindowText, QBrush(objDisplayData.fontColor));
    pLabel->setPalette(pal);

    // 设置文字的大小
    QFont labFont = pLabel->font();
    labFont.setPointSize(objDisplayData.iFontSize);
    pLabel->setFont(labFont);

    // 设置是否打断
    pLabel->setWordWrap(m_bWrap);
    pLabel->SetMinimumHeight(m_iItemMiniHeight);// 设置最小的行高

    if (objDisplayData.strIconPath.empty())
    {
        pLabel->SetDrawIcon(false);
    }
    else
    {
        pLabel->SetIconPath(objDisplayData.strIconPath);
        pLabel->SetDrawIcon(true);
    }

    // 设置文本
    pLabel->SetLabText(toQString(objDisplayData.strText.c_str()));
}

void AutoResizeListView::SetWrapWord(bool bWrap)
{
    m_bWrap = bWrap;
}

void AutoResizeListView::SetItemFixWidth(int iWidth)
{
    if (iWidth > 0 && iWidth != m_nItemFixWidth)
    {
        m_nItemFixWidth = iWidth;
        ResetItemWidth(m_nItemFixWidth);
        return;
    }
}

void AutoResizeListView::SetRowHeight(int iHeight)
{
    m_iRowHeight = iHeight;
}

void AutoResizeListView::ClearListData()
{
    // 释放链表资源
    labeliter iterlab = m_ListLabel.begin();
    while (iterlab != m_ListLabel.end())
    {
        AutoResizeLab * pLab = *iterlab;
        if (pLab != NULL)
        {
            pLab->deleteLater();
            *iterlab = NULL;
        }
        ++iterlab;
    }

    // 清空链表
    m_ListLabel.clear();
}

void AutoResizeListView::ResetItemWidth(int iWidth)
{
    if (iWidth > 0 && m_ListLabel.size() > 0)
    {
        labeliter iterlab = m_ListLabel.begin();
        while (iterlab != m_ListLabel.end())
        {
            AutoResizeLab * pLab = *iterlab;
            ++iterlab;
            if (pLab != NULL)
            {
                pLab->resize(iWidth, pLab->height());
            }
        }
    }
}

// 显示小于高度iHeight的所有Lab
void AutoResizeListView::ShowThePartLabByHeight(int iHeight)
{
    // 执行条件判断
    if (iHeight < 1 && m_ListLabel.size() == 0)
    {
        return;
    }

    int iCountHeight = 0;// 用于统计Lab的总高度
    labeliter it = m_ListLabel.begin();

    for (/* */; it != m_ListLabel.end(); ++it)
    {
        if ((*it) != NULL)
        {
            iCountHeight += (*it)->height();// 累加Lab的总高度
            if (iCountHeight > iHeight)
            {
                // 重新设置该Lab的text使去除其超出显示的部分
                (*it)->ResetSetTextByHeight((*it)->height() + iHeight - iCountHeight);
                ++it;
                break;
            }
        }
    }

    // 隐藏剩下的lab
    for (/* */; it != m_ListLabel.end(); ++it)
    {
        if ((*it) != NULL)
        {
            (*it)->hide();
        }
    }
}

// 根据高度重新设置Lab的位置和控件的高度,防止文字被截断
void AutoResizeListView::ResetLabPosByHeight(int iHeight)
{
    // 执行条件判断
    if (iHeight < 1 && m_ListLabel.size() == 0)
    {
        return;
    }

    int iCountHeight = 0;// 用于统计Lab高度
    int iOffset      = 0;// 每个Item的偏移值
    labeliter it = m_ListLabel.begin();

    for (/* */; it != m_ListLabel.end(); ++it)
    {
        if ((*it) != NULL)
        {
            QRect objRect = (*it)->geometry();

            iCountHeight += (*it)->height();// 累加Lab的总高度

            if (iCountHeight > iHeight)
            {
                // 计算每个Item的偏移值
                iOffset += iHeight - (iCountHeight - (*it)->height());
                iCountHeight = (*it)->height();
            }

            // 移动Item
            objRect.moveTop(objRect.top() + iOffset);
            (*it)->setGeometry(objRect);
        }
    }

    // 重新设置控件的高度
    this->resize(this->width(), this->height() + iOffset);
}

void AutoResizeListView::SetLabMinimumHeight(int iMinimumHeight)
{
    if (iMinimumHeight > 1)
    {
        m_iItemMiniHeight = iMinimumHeight;
    }
}


//设置最大显示行数
void AutoResizeListView::SetMaxRow(int iMaxRow)
{
    if (iMaxRow > 0)
    {
        m_iMaxRow = iMaxRow;
    }
}
