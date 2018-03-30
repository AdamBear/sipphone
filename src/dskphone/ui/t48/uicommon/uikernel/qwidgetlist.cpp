#include "qwidgetlist.h"

#include <QtGui>
#include <QTimer>

#include "qtcommon/qmisc.h"
#include "qwidgetutility.h"

#include "imagemanager/modimagemanager.h"

#define DEFAULT_TIP_BUTTON_OFFSET   10
//最边上的图标到边缘的空隙
#define BOUNDARY_SPACING    2

CTipButton::CTipButton(QWidget * parent)
    : qUIPushButton(parent)
{
    setTextColor(Qt::white);
    setIconLeftOffSet(DEFAULT_TIP_BUTTON_OFFSET);
}

CTipButton::~CTipButton()
{
    //
}

void CTipButton::paintEvent(QPaintEvent * pPaintEvent)
{
    QStylePainter p(this);
    QPixmap pm;

    // 判断button的状态，设置对应的图片
    if (!this->isEnabled())
    {
        pm = THEME_GET_BMP(fromQString(getDisalbePic()).c_str());
    }
    else if (this->isDown())
    {
        pm = THEME_GET_BMP(fromQString(getEnablePic()).c_str());
    }
    else
    {
        pm = THEME_GET_BMP(fromQString(getNormalPic()).c_str());
    }

    QStyleOptionButton option;
    initStyleOption(&option);

    int nTextHeight = 0;
    int nOffset = 0;
    if (!text().isEmpty())
    {
        QFontMetrics fontMetrics(font());
        nTextHeight = fontMetrics.height();
        nOffset = getIconLeftOffSet();
    }

    int nPixmapY = option.rect.y() + (option.rect.height() - pm.height() - nTextHeight - nOffset) >> 1;
    nOffset = 0;
    // 画按钮背景图片
    if (!pm.isNull())
    {
        nOffset = getIconLeftOffSet();
        p.drawPixmap(option.rect.x() + (option.rect.width() - pm.width()) >> 1,
                     nPixmapY, pm);
    }

    // 若是Text为空，就没必要绘制
    if (!text().isEmpty())
    {
        int nTextY = nPixmapY + pm.height() + nOffset;
        QRect rcText = option.rect;
        rcText.setY(nTextY + 1);
        // 绘制Item的Text
        p.drawItemText(rcText, Qt::AlignHCenter | Qt::AlignTop, option.palette, this->isEnabled(),
                       this->text());
    }
}


qWidgetList::qWidgetList(QWidget * parent, ITEM_ALIGN_TYPE eAlignType,  QRect rect)
    : QFrame(parent)
    , m_eAlignType(eAlignType)
    , m_pMoveTimer(NULL)
{
    m_MoveFrame.setParent(this);
    m_MoveFrame.setObjectName("MoveFrame");
    this->setGeometry(rect);

    m_MoveFrame.setGeometry(0, 0, rect.width(), rect.height());

    m_pMoveTimer = new QTimer(this);
    if (NULL != m_pMoveTimer)
    {
        // 连接信号和槽
        connect(m_pMoveTimer, SIGNAL(timeout()), this, SLOT(OnMoveTimerOut()));
    }
}

qWidgetList::~qWidgetList()
{

}

//改变大小事件
void qWidgetList::resizeEvent(QResizeEvent * pResizeEvent)
{
    QFrame::resizeEvent(pResizeEvent);
    QRect rect = geometry();
    m_MoveFrame.setGeometry(0, 0, rect.width(), rect.height());
}

// 添加Item
void qWidgetList::AddItem(const qWidgetListItemDataBase * pItem, int iIndex/* = -1*/)
{
    // 入口参数检查
    if (NULL == pItem)
    {
        chWARNING2(false, "qWidgetList::AddItem param error");
        return;
    }

    QWidget * pWidget = CreateWidgetByType(pItem);
    if (NULL == pWidget)
    {
        chWARNING2(false, "qWidgetView::AddItem Create Widget Error");
        return;
    }

    // 寻找插入Item的位置
    IterWidget it = m_listWidget.begin();
    int iInitOffset = 0;
    bool bMove = true;
    if (iIndex >= m_listWidget.size() || iIndex < 0)
    {
        // iIndex大于链表的size,或者是负数
        // 插入最后一个Item的后面
        it = m_listWidget.end();// 取得末端迭代器
        bMove = false;
        iInitOffset = CountAllWidgetLength();
    }
    else
    {
        // 寻找插入的迭代器
        int iCount = 0;

        //水平对齐
        if (m_eAlignType == ALIGN_TYPE_LEFT || m_eAlignType == ALIGN_TYPE_RIGHT)
        {
            iInitOffset += it->pWidget->width();
            while (it != m_listWidget.end() && iCount != iIndex)
            {
                iInitOffset += it->pWidget->width();
                ++it;
                ++iCount;
            }

            // 因为插在最后widget前面
            // 减去最后一个widget的长度
            if (it != m_listWidget.end())
            {
                iInitOffset -= it->pWidget->width();
            }
        }
        //垂直对齐
        else
        {
            iInitOffset += it->pWidget->height();
            while (it != m_listWidget.end() && iCount != iIndex)
            {
                iInitOffset += it->pWidget->height();
                ++it;
                ++iCount;
            }

            // 因为插在最后widget前面
            // 减去最后一个widget的长度
            if (it != m_listWidget.end())
            {
                iInitOffset -= it->pWidget->height();
            }
        }

        bMove = true;
    }

    //给末端留一些空白
    iInitOffset += BOUNDARY_SPACING;
    // 根据类型计算初始的位置
    switch (m_eAlignType)
    {
    case ALIGN_TYPE_LEFT:
        {
            pWidget->setGeometry(iInitOffset, 0, pWidget->width(), pWidget->height());
        }
        break;
    case ALIGN_TYPE_RIGHT:
        {
            pWidget->setGeometry(m_MoveFrame.width() - iInitOffset - pWidget->width(),
                                 (m_MoveFrame.height() - pWidget->height()) / 2, pWidget->width(), pWidget->height());
        }
        break;
    case ALIGN_TYPE_TOP:
        {
            pWidget->setGeometry(0, iInitOffset, pWidget->width(), pWidget->height());
        }
        break;
    case ALIGN_TYPE_BOTTOM:
        {
            pWidget->setGeometry(0, m_MoveFrame.height() - iInitOffset - pWidget->height(), pWidget->width(),
                                 pWidget->height());
        }
        break;
    default:
        break;
    }

    if (bMove)
    {
        //水平对齐
        if (m_eAlignType == ALIGN_TYPE_LEFT || m_eAlignType == ALIGN_TYPE_RIGHT)
        {
            // 移动插入位置后面的item
            MoveItem(it, pItem->itemSize.width(), pItem->objAnimationData);
        }
        //垂直对齐
        else
        {
            MoveItem(it, pItem->itemSize.height(), pItem->objAnimationData);
        }

    }

    // 创建Item数据
    qWidgetListItemInfo objItem;
    objItem.pWidget = pWidget;
    objItem.objData.strId = pItem->strId;
    objItem.objData.eItemType = pItem->eItemType;
    objItem.objData.objAnimationData = pItem->objAnimationData;
    objItem.objData.itemSize = pItem->itemSize;
    objItem.objData.appendData = pItem->appendData;

    m_listWidget.insert(it, objItem);

    // 显示插入的Item
    ShowInsertItem(pWidget, pItem->objAnimationData);
}

// 判断某个Item是否存在
bool qWidgetList::IsItemExist(const yl::string & strId)
{
    // 找到对应Id的迭代器
    IterWidget it = m_listWidget.begin();
    for (/* */; it != m_listWidget.end(); ++it)
    {
        if (it->objData.strId == strId.c_str())
        {
            return true;
        }
    }

    return false;
}

// 删除Item
void qWidgetList::DeleteItem(const AnimationData & objAnimationData, int iIndex /*= -1*/)
{
    if (m_listWidget.empty())
    {
        return;
    }

    // 寻找删除Item的位置
    IterWidget it = m_listWidget.begin();
    if (iIndex >= m_listWidget.size() || iIndex < 0)
    {
        // iIndex大于链表的size,或者是负数
        // 默认删除最后一个
        it = m_listWidget.end();// 取得末端迭代器

        if (m_listWidget.size() > 0)
        {
            --it;// 取得最后一个Item的迭代器
        }
    }
    else
    {
        // 寻找删除的迭代器
        int iCount = 0;
        while (it != m_listWidget.end() && iCount != iIndex)
        {
            ++it;
            ++iCount;
        }
    }

    DeleteItem(objAnimationData, it);
}

// 删除对应ID的Item
void qWidgetList::DeleteItem(const AnimationData & objAnimationData, const yl::string & strId)
{
    if (m_listWidget.empty())
    {
        return;
    }

    // 找到对应Id的迭代器
    IterWidget it = m_listWidget.begin();
    for (/* */; it != m_listWidget.end(); ++it)
    {
        if (it->objData.strId == strId.c_str())
        {
            break;
        }
    }

    DeleteItem(objAnimationData, it);
}

// 删除对应迭代器的Item
void qWidgetList::DeleteItem(const AnimationData & objAnimationData, IterWidget it)
{
    if (m_listWidget.empty() || it == m_listWidget.end())
    {
        return;
    }

    bool bMove = true;
    IterWidget itEndItem = m_listWidget.end();
    --itEndItem;// 取得最后一个Item的迭代器
    if (itEndItem == it)
    {
        bMove = false;// 删除的是最后一个Item,不需要播放删除动画
    }

    if (it != m_listWidget.end() && NULL != it->pWidget)
    {
        // to do ： 暂时没有隐藏动画效果
        it->pWidget->hide();

        if (bMove)
        {
            // 移动插入位置后面的item
            IterWidget iterTemp = it;
            //水平对齐
            if (m_eAlignType == ALIGN_TYPE_LEFT || m_eAlignType == ALIGN_TYPE_RIGHT)
            {
                MoveItem(++iterTemp, 0 - it->pWidget->width(), objAnimationData);
            }
            //垂直对齐
            else
            {
                MoveItem(++iterTemp, 0 - it->pWidget->height(), objAnimationData);
            }

        }

        // 释放资源
        it->pWidget->deleteLater();
        it->pWidget = NULL;
    }

    m_listWidget.erase(it);
}

// 将iterBegin以后的Item移动iMoveRange的距离
void qWidgetList::MoveItem(IterWidget iterBegin, int iMoveRange,
                           const AnimationData & objAnimationData)
{
    if (objAnimationData.eOtherItemAnimation == OTHERS_ITEM_NONE)
    {
        CountMoveOffset(iMoveRange, 1);
    }
    else if (objAnimationData.eOtherItemAnimation == OTHERS_ITEM_MOVE)
    {
        CountMoveOffset(iMoveRange, MOVE_TIME);
    }
    else
    {
        return;
    }

    // 临时代码,先改为阻塞播放动画
    while (m_listOffsetData.size() > 0)
    {
        list<OffSetData>::iterator ItOffsetData = m_listOffsetData.begin();
        IterWidget itTemp;
        itTemp = iterBegin;
        for (/* */; itTemp != m_listWidget.end(); ++itTemp)
        {
            QWidget * pWidget = itTemp->pWidget;
            if (NULL != pWidget)
            {
                QPoint objPoint = pWidget->geometry().topLeft();
                objPoint.setX(objPoint.x() + ItOffsetData->iXOffSet);
                objPoint.setY(objPoint.y() + ItOffsetData->iYOffSet);
                pWidget->setGeometry(objPoint.x(), objPoint.y(), pWidget->width(), pWidget->height());
            }
            //this->repaint();
        }
        m_listOffsetData.pop_front();
        //Sleep(MOVE_INTERVAL);
    }
}

// 计算所有类型的长度
int qWidgetList::CountAllWidgetLength()
{
    IterWidget it = m_listWidget.begin();
    int iAllWidgetLenth = 0;
    int iLen = 0;
    for (; it != m_listWidget.end(); ++it)
    {
        QWidget * pWidget = it->pWidget;
        if (pWidget != NULL)
        {
            switch (m_eAlignType)
            {
            case ALIGN_TYPE_LEFT:
            case ALIGN_TYPE_RIGHT:
                {
                    iLen = pWidget->width();
                }
                break;
            case ALIGN_TYPE_TOP:
            case ALIGN_TYPE_BOTTOM:
                {
                    iLen = pWidget->height();
                }
                break;
            default:
                break;
            }

            iAllWidgetLenth += iLen;
        }
    }

    return iAllWidgetLenth;
}

// 根据移动的次数计算每次移动的距离
void qWidgetList::CountMoveOffset(int iMoveRange, int iMoveTime)
{
    if (iMoveTime <= 0)
    {
        return;
    }

    int iOffset = iMoveRange / iMoveTime;// 每次的偏移距离
    int iLose   = iMoveRange - iOffset * iMoveTime; // 整除产生的值损失
    OffSetData objOffsetData;

    // 根据类型计算偏移值
    switch (m_eAlignType)
    {
    case ALIGN_TYPE_LEFT:
        {
            objOffsetData.iXOffSet = iOffset;
            objOffsetData.iYOffSet = 0;
        }
        break;
    case ALIGN_TYPE_RIGHT:
        {
            objOffsetData.iXOffSet = -iOffset;
            objOffsetData.iYOffSet = 0;
        }
        break;
    case ALIGN_TYPE_TOP:
        {
            objOffsetData.iXOffSet = 0;
            objOffsetData.iYOffSet = iOffset;
        }
        break;
    case ALIGN_TYPE_BOTTOM:
        {
            objOffsetData.iXOffSet = 0;
            objOffsetData.iYOffSet = -iOffset;
        }
        break;
    default:
        break;
    }

    m_listOffsetData.clear();
    while (iMoveTime > 0)
    {
        if (iMoveTime == 1)
        {
            // 对最后一次的移动距离进行补偿
            switch (m_eAlignType)
            {
            case ALIGN_TYPE_LEFT:
                {
                    objOffsetData.iXOffSet += iLose;
                }
                break;
            case ALIGN_TYPE_RIGHT:
                {
                    objOffsetData.iXOffSet -= iLose;
                }
                break;
            case ALIGN_TYPE_TOP:
                {
                    objOffsetData.iYOffSet += iLose;
                }
                break;
            case ALIGN_TYPE_BOTTOM:
                {
                    objOffsetData.iYOffSet -= iLose;
                }
                break;
            default:
                break;
            }
        }
        m_listOffsetData.push_back(objOffsetData);
        --iMoveTime;
    }
}

void qWidgetList::OnMoveTimerOut()
{

}

// 显示插入的Item
void qWidgetList::ShowInsertItem(QWidget * pInsertWidget, const AnimationData & objAnimationData)
{
    if (NULL != pInsertWidget)
    {
        QRect rect = pInsertWidget->geometry();
        pInsertWidget->show();
    }
}

// 根据Type创建控件
QWidget * qWidgetList::CreateWidgetByType(const qWidgetListItemDataBase * pItem)
{
    if (NULL == pItem)
    {
        return NULL;
    }

    if (pItem->eItemType == ITEM_TYPE_LABEL)
    {
        QLabel * pLab = new QLabel(&m_MoveFrame);
        if (NULL != pLab)
        {
            pLab->setObjectName("Lab");
            qWidgetListItemDataLab * pItemLab = (qWidgetListItemDataLab *)pItem;
            pLab->setGeometry(0, 0, pItem->itemSize.width(), pItem->itemSize.height());
            qWidgetUtility::SetBackGround(pLab, pItemLab->strPicPath.toUtf8().data());
        }

        return pLab;
    }
    else if (pItem->eItemType == ITEM_TYPE_BUTTON)
    {
        qUIPushButton * pButton = new qUIPushButton(&m_MoveFrame);
        qWidgetListItemDataButton * pItemBtn = (qWidgetListItemDataButton *)pItem;
        if (NULL != pButton)
        {
            pButton->setObjectName("Button");
            pButton->set3StatePic(pItemBtn->strNormalPic, pItemBtn->strEnablePic, pItemBtn->strDisablePic);
            pButton->setGeometry(0, 0, pItem->itemSize.width(), pItem->itemSize.height());

            // 连接外面的信号和槽
            if (!pItemBtn->strAction.isEmpty() && pItemBtn->actionReceiver != NULL
                    && pItemBtn->actionMember != NULL)
            {
                pButton->setBtnAction(pItemBtn->strAction);
                connect(pButton, SIGNAL(clicked(QString)), pItemBtn->actionReceiver, pItemBtn->actionMember);
            }
        }
        return pButton;
    }
    else if (pItem->eItemType == ITEM_TYPE_TIPBTN)
    {
        CTipButton * pTipButton = new CTipButton(&m_MoveFrame);
        qWidgetListItemDataTipButton * pItemTipBtn = (qWidgetListItemDataTipButton *)pItem;

        if (NULL != pTipButton && NULL != pItemTipBtn)
        {
            pTipButton->setObjectName("TipButton");
            pTipButton->setGeometry(0, 0, pItem->itemSize.width(), pItem->itemSize.height());
            pTipButton->set3StatePic(pItemTipBtn->strNormalPic, pItemTipBtn->strEnablePic,
                                     pItemTipBtn->strDisablePic);

            // 连接外面的信号和槽
            if (!pItemTipBtn->strAction.isEmpty() && pItemTipBtn->actionReceiver != NULL
                    && pItemTipBtn->actionMember != NULL)
            {
                pTipButton->setBtnAction(pItemTipBtn->strAction);
                connect(pTipButton, SIGNAL(clicked(QString)), pItemTipBtn->actionReceiver,
                        pItemTipBtn->actionMember);
            }

            pTipButton->setText(pItemTipBtn->strLabel);
        }
        return pTipButton;
    }
    else
    {
        chWARNING2(false, "qWidgetView::CreateWidgetByType item type error");
        return NULL;
    }
}

// 获取有几项
int qWidgetList::GetIconNum()
{
    return m_listWidget.size();
}

// 清除控件数据
void qWidgetList::Clear()
{
    IterWidget it = m_listWidget.begin();
    while (it != m_listWidget.end())
    {
        // 释放资源
        it->pWidget->deleteLater();
        it->pWidget = NULL;

        ++it;
    }

    m_listWidget.clear();
    m_listOffsetData.clear();
}
