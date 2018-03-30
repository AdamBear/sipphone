#include "qswitchpagelistview.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/quipushbutton.h"
#include "qlistwidgetutility.h"

// 用于翻页辅助计算
#define chCEIL(n, row)                  (((n)+(row)-1) - ((n)+(row)-1) % (row))
static const QColor ITEM_SELECTED_COLOR(255, 255, 255, 255);
static const QColor ITEM_UN_SELECTED_COLOR(0, 0, 0, 255);

/************************************************************************/
/*                      class CCutPageListViewItem                      */
/************************************************************************/
//构造函数
CSwitchPageListViewItem::CSwitchPageListViewItem()
{
    this->setObjectName("CSwitchPageListViewItem");
}

//析构函数
CSwitchPageListViewItem::~CSwitchPageListViewItem()
{

}

//获取Icon的Click_Action操作
QString CSwitchPageListViewItem::GetClickAction() const
{
    return m_strClickAction;
}

//设置Icon的Click_Action操作
void CSwitchPageListViewItem::SetClickAction(const char * lpcsAction)
{
    m_strClickAction = lpcsAction;
}

//获取Icon的LongPress_Action操作
QString CSwitchPageListViewItem::GetLongPressAction() const
{
    return m_strLongPressAction;
}

//设置Icon的LongPress_Action操作
void CSwitchPageListViewItem::SetLongPressAction(const char * lpcsAction)
{
    m_strLongPressAction = lpcsAction;
}

//获取显示名称
QString CSwitchPageListViewItem::GetShowName() const
{
    return m_strShowName;
}

//取得显示名称
void CSwitchPageListViewItem::SetShowName(const char * lpcsAction)
{
    m_strShowName = lpcsAction;
}

////////////////////////////////////---CMenuView---/////////////////////////////////////////////
//构造函数
CSwitchPageListView::CSwitchPageListView(QWidget * pParent /*= 0*/)
    : QListWidget(pParent)
    , m_nCurrentPage(1)
    , m_bFirstIn(true)
    , m_pPrePressItem(NULL)
    , m_pLastPressItem(NULL)
{
    setAutoFillBackground(true);

    //设置ListView透明
    QListWidgetUtility::SetQListWidgetTransparent(this);
    QListWidgetUtility::SetQListWgtNormalProperty(this);

    //按下时的信号插槽连接
    connect(this, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(OnItemPressed(QListWidgetItem *)));
    //点击图标菜单项时信号插槽连接
    connect(this, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(OnItemClicked(QListWidgetItem *)));
    //定时器信号插槽连接
    connect(&m_timerLongPressed, SIGNAL(timeout()), this, SLOT(OnLongPressTimer()));

    this->installEventFilter(this);

    SetTextColor(QColor(Qt::black), QColor(Qt::black));
}

//析构函数
CSwitchPageListView::~CSwitchPageListView()
{
    m_timerLongPressed.stop();
}

//设置Iconview的图标数量,nX * nY
void CSwitchPageListView::setMatrixGrid(int nX, int nY)
{
    // 入口参数检测
    if (nX < 1 || nY < 1)
    {
        return ;
    }

    //记录图标列数
    m_nMartrixX = nX;
    //记录图标行数
    m_nMartrixY = nY;

    //设置网格大小
    int nMartrixX = m_nMartrixX - 2 < 1 ? 1 : m_nMartrixX;
    QRect rect = geometry();
    QSize sizeGrid(rect.width() / m_nMartrixX - 2, rect.height() / m_nMartrixY - 2);
    QListWidget::setGridSize(sizeGrid);

    //初始化时显示首页
    ShowPage(1);
}

void CSwitchPageListView::SetItemSize(const QSize & itemSize)
{
    // 设置每个ItemSize
    m_itemSize = itemSize;
}

void CSwitchPageListView::SetItemFont(const QFont & itemFont)
{
    // 设置每个Item的字体样式
    m_ItemFont = itemFont;
    setFont(m_ItemFont);
}

//显示指定序号页面
void CSwitchPageListView::ShowPage(int nIndex)
{
    // 检测nIdex的有效性
    if (nIndex < GetFirstPageIndex() || nIndex > GetLastPageIndex())
    {
        return;
    }

    //取得当前单个页面菜单item的总个数
    int nPageCount = GetPageCount();

    //如果没有菜单项，直接返回
    if (nPageCount == 0)
    {
        return;
    }

    // 发射页面改变的信号
    emit PageChange(nIndex);

    //获取当前显示的第几页面数
    m_nCurrentPage = nIndex;
    int nItemsPerPage = m_nMartrixY * m_nMartrixX;
    ShowItems((m_nCurrentPage - 1) * nItemsPerPage, nItemsPerPage);
}

// 获取当前Item的显示名
QString CSwitchPageListView::GetCurrentItemDisplayName()
{
    QListWidgetItem * pListViewItem = this->currentItem();
    if (NULL != pListViewItem)
    {
        CSwitchPageListViewItem * pItem = (CSwitchPageListViewItem *)pListViewItem;
        return pItem->GetShowName();
    }

    return QString("");
}

// 设置选中的Item
void CSwitchPageListView::SetCurrentItem(int iItemIndex)
{
    // 入口参数检测
    if (iItemIndex < 0
            || iItemIndex >= this->count())
    {
        return;
    }

    // 计算Item的当前页数
    int iItemsPerPage = m_nMartrixY * m_nMartrixX;
    int iPageCount    = iItemIndex / iItemsPerPage + 1;

    if (iPageCount != GetCurrentPageIndex())
    {
        // 显示Item的那页
        this->ShowPage(iPageCount);
    }

    // 选中相应的Item
    this->setCurrentRow(iItemIndex);

    // 第一次进才发
    if (m_bFirstIn)
    {
        m_bFirstIn = false;
        CSwitchPageListViewItem * pIconViewItem = (CSwitchPageListViewItem *)this->currentItem();
        if (pIconViewItem != NULL)
        {
            if (this->isVisible())
            {
                // 发送消息按这个顺序，不然回滚操作没有效果
                // 选中行改变
                emit CurrentItemChanged(pIconViewItem, m_pPrePressItem);
                m_pPrePressItem = pIconViewItem;
                // 向外发送click信号
                emit ItemClicked(pIconViewItem->GetClickAction());
            }
        }
    }
    else
    {
        CSwitchPageListViewItem * pIconViewItem = (CSwitchPageListViewItem *)this->currentItem();
        if (pIconViewItem != NULL)
        {
            if (this->isVisible())
            {
                emit CurrentItemChanged(pIconViewItem, m_pPrePressItem);
                m_pPrePressItem = pIconViewItem;
            }
        }
    }
}

// 设置选中的Item
void CSwitchPageListView::SetCurrentItem(const QString & strItemClickAction)
{
    // 遍历找出对应的Index
    int iIndex = 0;
    for (/* */; iIndex < this->count(); ++iIndex)
    {
        QListWidgetItem * pItem = this->item(iIndex);
        if (NULL != pItem)
        {
            CSwitchPageListViewItem * pSwitchPageItem = (CSwitchPageListViewItem *)pItem;
            if (pSwitchPageItem->GetClickAction() == strItemClickAction)
            {
                break;
            }
        }
    }

    if (iIndex < this->count())
    {
        SetCurrentItem(iIndex);
    }
}

//切换到上一页
void CSwitchPageListView::PageUp()
{
    if (m_nCurrentPage == GetFirstPageIndex())
    {
        // 若是第一一页则跳转到最后
        ShowPage(GetLastPageIndex());
    }
    else
    {
        ShowPage(m_nCurrentPage - 1);
    }
}

//切换到下一页
void CSwitchPageListView::PageDown()
{

    if (m_nCurrentPage == GetLastPageIndex())
    {
        // 若是最后一页则跳转到第一页
        ShowPage(GetFirstPageIndex());
    }
    else
    {
        ShowPage(m_nCurrentPage + 1);
    }
}

int CSwitchPageListView::GetLastPageIndex()
{
    // 最大页面就是总页面数
    return GetPageCount();
}

int CSwitchPageListView::GetFirstPageIndex()
{
    // 页面数从1开始
    return 1;
}

int CSwitchPageListView::GetCurrentPageIndex()
{
    return m_nCurrentPage;
}

//取得页面布局图标列数
int CSwitchPageListView::GetMartrixX()
{
    return m_nMartrixX;
}

//取得页面布局图标行数
int CSwitchPageListView::GetMartrixY()
{
    return m_nMartrixY;
}

void CSwitchPageListView::SetTextColor(const QColor & highlightedColor, const QColor & commonColor)
{
    // 设置界面的高亮
    QPalette pal = this->palette();
    pal.setBrush(QPalette::Inactive, QPalette::HighlightedText, QBrush(highlightedColor));
    QBrush commonBrush(commonColor);
    commonBrush.setStyle(Qt::SolidPattern);
    pal.setBrush(QPalette::Active, QPalette::WindowText, commonBrush);
    pal.setBrush(QPalette::Active, QPalette::Text, commonBrush);
    pal.setBrush(QPalette::Active, QPalette::ButtonText, commonBrush);
    pal.setBrush(QPalette::Active, QPalette::ToolTipText, commonBrush);
    pal.setBrush(QPalette::Inactive, QPalette::WindowText, commonBrush);
    pal.setBrush(QPalette::Inactive, QPalette::Text, commonBrush);
    pal.setBrush(QPalette::Inactive, QPalette::ButtonText, commonBrush);
    pal.setBrush(QPalette::Inactive, QPalette::ToolTipText, commonBrush);
    pal.setBrush(QPalette::Disabled, QPalette::HighlightedText, commonBrush);
    pal.setBrush(QPalette::Disabled, QPalette::ToolTipText, commonBrush);
    this->setPalette(pal);
}

//显示出指定item
void CSwitchPageListView::ShowItems(int iIndex, int nCount)
{
    //取得当前页面显示item的最后一项的序号
    int iLastItem = (iIndex + nCount) - 1;
    //item总数
    int nTotalCount = count();
    for (int i = 0; i < nTotalCount; i++)
    {
        //把除了要当前页面要显示的item项，其余都隐藏掉
        QListWidget::setRowHidden(i, i < iIndex || i > iLastItem);
    }
}

// 取得Page的页数
int CSwitchPageListView::GetPageCount()
{
    //取得单个页面最多显示的item数
    int nPageSize = m_nMartrixY * m_nMartrixX;
    //取得所有菜单item项个数
    int nTotalCount = count();
    //防止被除数为0
    if (nPageSize == 0)
    {
        return 0;
    }

    return chCEIL(nTotalCount, nPageSize) / nPageSize;
}

//菜单项点击时的处理
void CSwitchPageListView::OnItemClicked(QListWidgetItem * pItem)
{
    //m_bFalg 为false时表示点击状态
    if (!m_bFlag)
    {
        m_timerLongPressed.stop();
    }

    CSwitchPageListViewItem * pIconViewItem = (CSwitchPageListViewItem *)pItem;
    if (pIconViewItem != NULL)
    {
        if (this->isVisible())
        {
            // 发送消息按这个顺序，不然回滚操作没有效果
            // 发送行改变信息
            emit CurrentItemChanged(pItem, m_pPrePressItem);
            m_pPrePressItem = pItem;
            //向外发送click信号
            emit ItemClicked(pIconViewItem->GetClickAction());
        }
    }
}

//菜单项按下时的处理
void CSwitchPageListView::OnItemPressed(QListWidgetItem * pItem)
{
    //定时器状态标识初始化为false,表示定时活跃状态
    m_bFlag = false;

    //启动定时器
    m_timerLongPressed.start(LONG_PRESS_TIME);

    //记录当前的点下item项
    m_pLastPressItem = pItem;
}

//长按菜单项定时器处理槽
void CSwitchPageListView::OnLongPressTimer()
{
    //停止定时器
    m_timerLongPressed.stop();

    //取得当前点击菜单项
    QListWidgetItem * pItem = currentItem();

    //设置m_bFlag为长按状态值
    m_bFlag = true;

    //长按同一个item时发出长按信号
    if (m_pLastPressItem == pItem)
    {
        CSwitchPageListViewItem * pIconViewItem = (CSwitchPageListViewItem *)pItem;
        if (pIconViewItem != NULL)
        {
            // 处于激活状态才向外发生信号
            if (this->isVisible())
            {
                //向外发送long press信号
                emit ItemLongPressed(pIconViewItem->GetLongPressAction());
            }
        }
    }
    else
    {
        if (pItem != NULL)
        {
            //取消点击状态
            pItem->setSelected(FALSE);
        }
    }

    m_pLastPressItem = NULL;
}

/************************************************************************/
/*                      class CCutPageListModeBase                      */
/************************************************************************/
CSwitchPageList::CSwitchPageList(QWidget * pParent/* = 0*/)
    : CSwitchPageListView(pParent)
    , m_lpszBGImagePath(PIC_GENERAL_SELECT_BG1)
    , m_bAutoDrawItemBackground(true)
    , m_iPreItemIndex(-1)
    , m_iCurItemIndex(-1)
{
    //选中项变化信号插槽连接
    connect(this, SIGNAL(CurrentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(OnCurrentItemChange(QListWidgetItem *, QListWidgetItem *)));

    SetTextColor(QColor(ITEM_SELECTED_COLOR), QColor(ITEM_UN_SELECTED_COLOR));
}

CSwitchPageList::~CSwitchPageList()
{

}

void CSwitchPageList::SetItemBGImagePath(LPCSTR lpszPath)
{
    if (lpszPath != NULL)
    {
        m_lpszBGImagePath = lpszPath;
    }
}

void CSwitchPageList::SetAutoDrawItemBackground(bool bMode)
{
    m_bAutoDrawItemBackground = bMode;
}

void CSwitchPageList::RollackPreItem()
{
    //跳到相应页面
    SetCurrentItem(m_iPreItemIndex);
}

void CSwitchPageList::OnCurrentItemChange(QListWidgetItem * currentItem,
        QListWidgetItem * previousItem)
{
    if (NULL == previousItem)
    {
        m_iPreItemIndex = -1;
    }
    else
    {
        m_iPreItemIndex = this->row(previousItem);
    }

    // 判断是否绘制Item背景
    if (m_bAutoDrawItemBackground)
    {
        //未选中字体设置
        QBrush brushUnselected(ITEM_UN_SELECTED_COLOR);
        //选中字体设置
        QBrush brushSelected(ITEM_SELECTED_COLOR);

        // 取消前一个Item的背景
        if (NULL != previousItem)
        {
            SetPreviousItemBg(previousItem);
            //解决菜单文字切换颜色问题
            previousItem->setForeground(brushUnselected);
        }

        // 设置当前选中Item的背景
        if (NULL != currentItem)
        {
            // 设置当前选中行
            m_iCurItemIndex = this->row(currentItem);

            SetCurrentItemBg(currentItem);
            //解决菜单文字切换颜色问题
            currentItem->setForeground(brushSelected);

            // 不重复刷新CMenuView::OnItemClicked(QListWidgetItem *pItem)
            // 背景变的时候要相应的切页，所以发生点击信号
//          CSwitchPageListViewItem *pIconViewItem = (CSwitchPageListViewItem*)currentItem;
//          emit CurrentItemChange(pIconViewItem->GetClickAction());
        }
    }
}

void CSwitchPageList::SetPreviousItemBg(QListWidgetItem * previousItem)
{
    previousItem->setBackground(QBrush());
}

void CSwitchPageList::SetCurrentItemBg(QListWidgetItem * currentItem)
{
    QPixmap selectBg = THEME_GET_BMP(m_lpszBGImagePath);
    if (selectBg.isNull())
    {
        return;
    }
    currentItem->setBackground(QBrush(selectBg.scaled(
                                          m_itemSize)));    //设置选中item时的背景图片
}

//菜单项点击时的处理
void CSwitchPageList::OnItemClicked(QListWidgetItem * pItem)
{
    setCurrentItem(pItem);
}

int CSwitchPageList::GetCurrentItemIndex() const
{
    return m_iCurItemIndex;
}

void CSwitchPageList::Reset()
{
    this->clear();
    m_iPreItemIndex = -1;
    m_iCurItemIndex = -1;
    m_pPrePressItem = NULL;

    m_nCurrentPage = 1;
    m_bFirstIn = true;

    m_strAction = "";
    m_timerLongPressed.stop();

    m_pLastPressItem = NULL;
    m_bFlag = false;
}
