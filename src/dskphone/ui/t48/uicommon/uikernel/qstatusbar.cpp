#include "qstatusbar.h"
#include "ui_qstatusbar.h"
#include <QTimer>
#include <time.h>
#include "qwidgetutility.h"
#include "qpushbuttonutility.h"
#include "picpath.h"
#include "qdatetimelabel.h"
#include "devicelib/phonedevice.h"

static const int TEST_STATUS_BAR = 0;

// statusBar Item之间的间距
static const int S_ITEM_MARGIN = 2;

// 判断是否长按的时间，单位为毫秒
static const int S_LONG_PRESS_TIME = 3000;

/************************************************************************/
/*                          class     STBPushButton                      */
/************************************************************************/
STBPushButton::STBPushButton(QWidget * parent, int nFlashTime) :
    qUIPushButton(parent),
    m_pTimerFlash(NULL),
    QSTBItem(nFlashTime)
{
    this->setObjectName("STBPushButton");

    // 设置为无焦点
    setFocusPolicy(Qt::NoFocus);

    // 连接信号和槽
    connect(this, SIGNAL(clicked()), this, SLOT(OnSTBPushButtonClicked()));
    connect(this, SIGNAL(pressed()), this, SLOT(OnSTBPushButtonPressed()));
    connect(this, SIGNAL(released()), this, SLOT(OnSTBPushButtonReleased()));

    // 连接计时器回调函数
    m_timerLongPress = new QTimer(this);
    if (NULL != m_timerLongPress)
    {
        connect(m_timerLongPress, SIGNAL(timeout()), this, SLOT(OnSTBPushButtonLongPress()));
    }

    //控件初始化
    InitIconButton();
}

void STBPushButton::InitIconButton()
{
    //判断是否需要闪动
    if (m_nFlashTime < 0)
    {
        return;
    }
    //连接计时器超时信号和槽
    m_pTimerFlash = new QTimer(this);               //创建定时器
    //启动定时器
    if (m_pTimerFlash != NULL)
    {
        connect(m_pTimerFlash, SIGNAL(timeout()), this,
                SLOT(OnTimeOut()));     //连接定时器超时时的响应函数

        m_pTimerFlash->start(m_nFlashTime);
    }
}

void STBPushButton::paintEvent(QPaintEvent * pe)
{
    QPainter painter(this);

    QIcon icon = this->icon();

    //改变字体颜色
    painter.setPen(Qt::white);

    //设置字体大小
    static QFont font = this->font();
    font.setPointSize(18);
    painter.setFont(font);

    QSize iconSize = icon.actualSize(QSize(32, 32));
    int x = 0;
    int y = (height() - iconSize.height()) >> 1;
    //如果ICON的高度比STBPushButton的高度还大，则调整ICON的尺寸
    if (y < 0)
    {
        y = 0;
        iconSize = QSize(height(), height());
    }

    //画图标
    QPixmap pix = icon.pixmap(iconSize, QIcon::Active, QIcon::On);
    painter.drawPixmap(x, y, pix);

    //写文字
    if (!m_strTips.isEmpty())
    {
        x += iconSize.width() + 1;
        int txW = width() - x;
        int h = height();
        // -2像素为了字和图片高度一致
        painter.drawText(x, y - 2, txW, height(), Qt::AlignLeft | Qt::AlignTop, m_strTips);
    }

    pe->accept();
}
void STBPushButton::OnTimeOut()
{
    //进行控件状态的变化
    if (this->isHidden())
    {
        this->raise();
        this->show();
    }
    else
    {
        this->hide();
    }
    if (m_pTimerFlash != NULL)
    {
        m_pTimerFlash->start(m_nFlashTime);
    }
}

void STBPushButton::OnSTBPushButtonClicked()
{
    emit clicked(this);
}

void STBPushButton::OnSTBPushButtonPressed()
{
    emit pressed(this);

    // 点击时启动长按计时器
    if (NULL != m_timerLongPress)
    {
        m_timerLongPress->start(S_LONG_PRESS_TIME);
    }
}

void STBPushButton::OnSTBPushButtonReleased()
{
    emit released(this);

    // 放开时停止长按计时器
    if (NULL != m_timerLongPress)
    {
        m_timerLongPress->stop();
    }
}
void STBPushButton::OnSTBPushButtonLongPress()
{
    // 停止长按的计时器
    if (m_timerLongPress != NULL)
    {
        m_timerLongPress->stop();
    }

    // 发出长按信号
    emit longPress(this);
}

/************************************************************************/
/*                          class     frmStatusBar                      */
/************************************************************************/
frmStatusBar::frmStatusBar(QWidget * parent)
    : QFrame(parent)
    , ui(new Ui::frmStatusBar)
    , m_nAlignType(ALIGN_TYPE_RIGHT)
    , m_pLabTip(NULL)
    , m_pBtnAccount(NULL)
    , m_pBtnMenu(NULL)
    , m_pDateTimeLabel(NULL)
{
    ui->setupUi(this);
    this->setAutoFillBackground(true);
    Init();
}

frmStatusBar::frmStatusBar(STATUS_BAR_ALIGN_TYPE alignType, QWidget * parent):
    QFrame(parent),
    ui(new Ui::frmStatusBar),
    m_nAlignType(alignType),
    m_pLabTip(NULL),
    m_pBtnAccount(NULL),
    m_pBtnMenu(NULL),
    m_pDateTimeLabel(NULL)
{
    ui->setupUi(this);
    this->setAutoFillBackground(true);
    Init();
}

frmStatusBar::~frmStatusBar()
{
    delete ui;
}

void frmStatusBar::SetAccountCaption(const QString & strCaption)
{
    // 进行指针判空
    if (NULL != m_pBtnAccount)
    {
        m_pBtnAccount->setText(strCaption);
    }
}

void frmStatusBar::setMenu3StatePic(const QString & normalPic,
                                    const QString & enablePic,
                                    const QString & disablePic)
{
    // 进行指针判空
    if (NULL != m_pBtnMenu)
    {
        m_pBtnMenu->set3StatePic(normalPic, enablePic, disablePic);
    }
}

void frmStatusBar::adjustTBPushButtonSize(STBPushButton * pBtn)
{
    if (NULL != pBtn)
    {
        if (!pBtn->m_strTips.isEmpty()
                && !pBtn->m_strIcon.isEmpty())
        {
            // 根据文字的长度
            // 由于只显示数字默认最多支持到显示999
            // 预留1像素为了美观
            pBtn->m_size.rwidth() += qMin(pBtn->m_strTips.length(), 3) * 6 + 1;
        }
    }
}
void frmStatusBar::setMenuIcon(const QString & iconPic, int nAlign)
{
    // 进行指针判空
    if (NULL == m_pBtnMenu)
    {
        return ;
    }

    // 设置Icon
    m_pBtnMenu->setIconPic(iconPic);

    // 设置Icon的对齐方式
    m_pBtnMenu->setIconPicAlign(nAlign);
}

void frmStatusBar::SetBackGroundImage(QString strImage)
{
    QPalette pal = palette();
    pal.setBrush(QPalette::Window, QBrush(QPixmap(strImage)));
    setPalette(pal);
}

void frmStatusBar::SetAlignType(STATUS_BAR_ALIGN_TYPE nType)
{
    // 判断入口参数的正确性
    if (nType < ALIGN_TYPE_BEGIN || nType > ALIGN_TYPE_END)
    {
        return ;
    }

    m_nAlignType = nType;
}

bool frmStatusBar::AddItem(QSTBItem item[], int nSTBItemNr, bool bFix/* = false*/)
{
    bool bAddSucceed = false;
    STBPushButton * pItem = NULL;

    // 遍历数组将前面的N个Item设置上去
    for (int i = 0; i < nSTBItemNr; i++)
    {
        // 查找对应的Item是否存在
        pItem = GetStatusItemByID(item[i].m_nID);
        if (pItem == NULL)
        {
            // Item不存在，新创建一个Item
            pItem = new STBPushButton(ui->frmIconBar, item[i].m_nFlashTime);
            pItem->m_strText = item[i].m_strText;
            pItem->m_strIcon = item[i].m_strIcon;
            pItem->m_strTips = item[i].m_strTips;
            pItem->m_nID  = item[i].m_nID;
            pItem->m_size = item[i].m_size;
            pItem->m_nFlashTime = item[i].m_nFlashTime;
            adjustTBPushButtonSize(pItem);

            // 连接信号和槽
            connect(pItem, SIGNAL(pressed(qUIPushButton *)), this, SLOT(OnFrmItemPressed(qUIPushButton *)));
            connect(pItem, SIGNAL(released(qUIPushButton *)), this, SLOT(OnFrmItemReleased(qUIPushButton *)));
            connect(pItem, SIGNAL(clicked(qUIPushButton *)), this, SLOT(OnFrmItemClicked(qUIPushButton *)));
            connect(pItem, SIGNAL(longPress(qUIPushButton *)), this, SLOT(OnFrmItemLongPress(qUIPushButton *)));

            // 添加Item
            bAddSucceed &= AddItem(pItem, bFix);
        }
        else
        {
            // Item存在，更新数据即可
            pItem->m_strText = item[i].m_strText;
            pItem->m_strIcon = item[i].m_strIcon;
            pItem->m_strTips = item[i].m_strTips;
            pItem->m_size = item[i].m_size;
            pItem->m_nFlashTime = item[i].m_nFlashTime;
            QPushButtonUtility::SetButtonIcon(pItem, pItem->m_strIcon.toUtf8().data());
            bAddSucceed = true;
        }
    }

    return bAddSucceed;
}

bool frmStatusBar::AddItem(int nID, QString strText, QString strTips,
                           QString strIcon, QSize size, bool bFix /*= false*/, int nFlashTime/* = -1*/)
{
    // 查找对应的Item是否存在
    STBPushButton * pItem = GetStatusItemByID(nID);

    if (pItem == NULL)
    {
        // Item不存在，新创建一个Item
        STBPushButton * pItem = new STBPushButton(ui->frmIconBar, nFlashTime);
        pItem->m_strText = strText;
        pItem->m_strIcon = strIcon;
        pItem->m_strTips = strTips;
        pItem->m_nID  = nID;
        pItem->m_size = size;
        pItem->m_nFlashTime = nFlashTime;
        adjustTBPushButtonSize(pItem);
        // 连接信号和槽
        connect(pItem, SIGNAL(clicked(qUIPushButton *)), this, SLOT(OnFrmItemClicked(qUIPushButton *)));
        connect(pItem, SIGNAL(pressed(qUIPushButton *)), this, SLOT(OnFrmItemPressed(qUIPushButton *)));
        connect(pItem, SIGNAL(released(qUIPushButton *)), this, SLOT(OnFrmItemReleased(qUIPushButton *)));
        connect(pItem, SIGNAL(longPress(qUIPushButton *)), this, SLOT(OnFrmItemLongPress(qUIPushButton *)));

        // 添加Item
        return AddItem(pItem, bFix);
    }
    else
    {
        // Item存在，更新数据即可
        pItem->m_strText = strText;
        pItem->m_strIcon = strIcon;
        pItem->m_strTips = strTips;
        pItem->m_size = size;
        pItem->m_nFlashTime = nFlashTime;
        QPushButtonUtility::SetButtonIcon(pItem, pItem->m_strIcon.toUtf8().data());
        pItem->set3StatePic(strIcon, strIcon, strIcon);
        pItem->setTransparent(true);
        pItem->update();

        return true;
    }
}

bool frmStatusBar::AddItem(STBPushButton *& pBtn, bool bFix/* = false*/)
{
    // 入口参数判断
    if (NULL == pBtn)
    {
        return false;
    }

#if TEST_STATUS_BAR
    // 测试模式下的代码
    QString strText;
    strText.sprintf("%d", pBtn->m_nID);
    pBtn->setText(strText);
#else
    // 初始化刚插入的Btn
    QPushButtonUtility::SetButtonIcon(pBtn, pBtn->m_strIcon.toUtf8().data());
    QPushButtonUtility::SetButtonTransparent(pBtn);
#endif

    adjustTBPushButtonSize(pBtn);

    bool bInsertBtn;// 判断是中间插入还是尾部追加
    if (bFix)
    {
        // 若是插入固定Item，直接位移可移动的Item
        MoveItem(m_iFixItemCount, pBtn->m_size.width() + S_ITEM_MARGIN);

        // 固定的item必然是用插入的方式
        bInsertBtn = true;
    }
    else
    {
        // 通过Bar是否足够显示所有的Item，判断使用插入还是追加的方式
        bInsertBtn = CheckInsertSTBPushButton(pBtn->m_size);
    }

    // 设置插入Item的位置
    pBtn->setGeometry(GetItemRect(pBtn->m_size, bInsertBtn));
    pBtn->show();

    /*add to data*/
    if (bInsertBtn)
    {
        // 在数据链表中的特定位置插入数据
        m_listBtnItem.insert(m_iFixItemCount, pBtn);

        // 若插入的是固定的item，固定Item数加1
        if (bFix)
        {
            SetFixItemCount(m_iFixItemCount + 1);
        }
    }
    else
    {
        // 在数据链表的尾部追加数据
        m_listBtnItem.append(pBtn);
    }

    return true;
}

bool frmStatusBar::DeleteItem(int nID, bool bFixItem/* = false*/)
{
    STBPushButton * pItem = NULL;
    STBPushButton * pDelItem = NULL;
    int nOffsetCx = 0;
    int iIndex    = 0;

    // find Key nID
    QList<STBPushButton *>::iterator it = m_listBtnItem.begin();
    for (/**/; it != m_listBtnItem.end(); ++it, ++iIndex)
    {
        pItem = *it;
        if (pItem->m_nID == nID)
        {
            pDelItem = pItem;
            ++it;
            ++iIndex;
            break;
        }
    }

    // not find
    if (NULL == pDelItem)
    {
        return false;
    }

    // 判断能否删除对应的Item
    if (bFixItem
            && iIndex > m_iFixItemCount)
    {
        // 若删除模式为删除固定的Item
        // 并且index大于m_nInsertBtnIndex(即该Item为普通Item),则无法删除
        return false;
    }
    else if (!bFixItem
             && iIndex <= m_iFixItemCount)
    {
        // 若删除模式为删除普通的Item
        // 并且index小于m_nInsertBtnIndex(即该Item为固定Item),则无法删除
        return false;
    }

    // move item
    pItem->hide();

    // 计算Item移动的偏移值
    nOffsetCx = GetItemOffsetCx(pItem->width() + S_ITEM_MARGIN);

    // 删除后对剩下的Item进行偏移
    for (/**/; it != m_listBtnItem.end(); ++it)
    {
        pItem = (*it);

        // 若移动的Item整个都在Bar内则显示该Item
        if (pItem->x() + nOffsetCx > S_ITEM_MARGIN
                && pItem->x() + nOffsetCx + pItem->width() < ui->frmIconBar->width() - S_ITEM_MARGIN)
        {
            pItem->show();
        }

        // 移动该Item，先显示再移动
        pItem->move(pItem->x() + nOffsetCx, pItem->y());
    }

    // delete pItem;
    m_listBtnItem.removeOne(pDelItem);

    // 若删除的是固定Item则，固定Item数减少
    if (bFixItem)
    {
        SetFixItemCount(m_iFixItemCount - 1);
    }

    return true;
}

bool frmStatusBar::DeleteAllItem()
{
    // 链表为空则不进行操作
    if (m_listBtnItem.size() == 0)
    {
        return true;
    }

    // 释放链表中的控件资源
    QList<STBPushButton *>::iterator it = m_listBtnItem.begin();
    for (/* */; it != m_listBtnItem.end(); ++it)
    {
        if (*it != NULL)
        {
            STBPushButton * pButton = *it;
            pButton->deleteLater();
            *it = NULL;
        }
    }

    m_listBtnItem.clear();
    m_iFixItemCount = 0;
    return true;
}

void frmStatusBar::SetFixItemCount(int nIndex)
{
    // 判断设置值的正确性后赋值
    if (nIndex >= 0
            && m_iFixItemCount != nIndex)
    {
        m_iFixItemCount = nIndex;
    }
}

void frmStatusBar::Init()
{
    /*init tooltips*/
    m_pLabTip = new QLabel(this, Qt::ToolTip);

    //SetLabel(m_pLabTip);
    QPalette pal;
    if (NULL != m_pLabTip)
    {
        pal = m_pLabTip->palette();
        pal.setColor(QPalette::Window, QColor(0, 0, 0, 50));
        m_pLabTip->setPalette(pal);
        m_pLabTip->setAlignment(Qt::AlignCenter);
        m_pLabTip->setAttribute(Qt::WA_DeleteOnClose, true);
        m_pLabTip->setGeometry(600, 55, 200, 50);
    }

    // init Item list
    m_listBtnItem.clear();

    // init Account PushButton
    m_pBtnAccount = new qUIPushButton(this);
    if (NULL != m_pBtnAccount)
    {
        m_pBtnAccount->setGeometry(110, 25, 200, 25);
        connect(m_pBtnAccount, SIGNAL(clicked()), this, SLOT(OnBtnAccountClicked()));
        m_pBtnAccount->setTransparent(true);
        m_pBtnAccount->setTextAlign(Qt::AlignLeft | Qt::AlignVCenter);
        m_pBtnAccount->setFocusPolicy(Qt::NoFocus);

        QPalette pl = m_pBtnAccount->palette();
        pl.setColor(m_pBtnAccount->foregroundRole(), Qt::white);
        m_pBtnAccount->setPalette(pl);

        // 设置控件字体
        qWidgetUtility::setFontStyle(m_pBtnAccount, 18, QFont::Bold);

        //!!!!!, 在T3X上不需要显示该控件
        m_pBtnAccount->hide();
    }

    // init Menu PushButton
    m_pBtnMenu = new qUIPushButton(this);
    if (NULL != m_pBtnMenu)
    {
        m_pBtnMenu->setGeometry(0, 0, 100, 50);
        //!!!!!, 不可如此处理，应该外面传入参数
        m_pBtnMenu->hide();
        m_pBtnMenu->setIconPicAlign(Qt::AlignCenter);
        m_pBtnMenu->setFocusPolicy(Qt::NoFocus);
        connect(m_pBtnMenu, SIGNAL(clicked()), this, SLOT(OnBtnMenuClicked()));
    }

    // init Frame
    //t46
    switch (devicelib_GetPhoneType())
    {
    case PT_T46:
        {
            this->setGeometry(0, 0, 480, 34);
            ui->frmIconBar->setGeometry(480 - 185, 0, 180, 34);
        }
        break;
    case PT_T48:
        {
            this->setGeometry(0, 0, 800, 43);
            ui->frmIconBar->setGeometry(250, 0, 300, 43);
        }
        break;
    default:
        {
            this->setGeometry(QRect(1, 1, 358, 34));
            ui->frmIconBar->setGeometry(QRect(174, 0, 180, 34));
        }
        break;
    }

    this->setFrameStyle(QFrame::NoFrame);
    ui->frmIconBar->setFrameStyle(QFrame::NoFrame);

    // init qDatetimeLabel
    m_pDateTimeLabel = new qDateTimeLabel(this);
    if (m_pDateTimeLabel != NULL)
    {
        switch (devicelib_GetPhoneType())
        {
        case PT_T46:
            {
                m_pDateTimeLabel->setGeometry(QRect(0, 0, 480, 34));
            }
            break;
        case PT_T48:
            {
                m_pDateTimeLabel->setGeometry(550, 0, 250, 43);
            }
            break;
        default:
            {
                m_pDateTimeLabel->setGeometry(QRect(10, 0, 180, 30));
            }
            break;
        }

        m_pDateTimeLabel->setAlignment(Qt::AlignCenter);
        // 设置lab上的字的颜色
        pal = m_pDateTimeLabel->palette();
        pal.setColor(QPalette::WindowText, Qt::white);
        m_pDateTimeLabel->setPalette(pal);
    }

    // 初始化固定Item的数量
    m_iFixItemCount = 0;
}

void frmStatusBar::SetTimeFmt(int iDateFrm, int iTimeFrm)
{
    // 指针判空
    if (m_pDateTimeLabel != NULL)
    {
        m_pDateTimeLabel->SetTimeFmtType(iDateFrm, iTimeFrm);
    }
}

int frmStatusBar::GetTimeFmt()
{
    if (m_pDateTimeLabel != NULL)
    {
        m_pDateTimeLabel->GetTimeFmt();
    }
    return -1;
}

int frmStatusBar::GetDateFrm()
{
    if (m_pDateTimeLabel != NULL)
    {
        m_pDateTimeLabel->GetDateFmt();
    }
    return -1;
}

int frmStatusBar::GetItemOffsetCx(int nItemWidth)
{
    // 根据对齐方式计算偏移值
    int nOffsetCx = 0;
    switch (m_nAlignType)
    {
    case ALIGN_TYPE_LEFT:
        {
            // 居左则偏移值要反转
            nOffsetCx = -nItemWidth;
        }
        break;

    case ALIGN_TYPE_CENTER:
        {

        }
        break;

    case ALIGN_TYPE_RIGHT:
        {
            nOffsetCx = nItemWidth;
        }
        break;

    case ALIGN_TYPE_UNKNOW:
    default:
        {
            break;
        }
    }

    return nOffsetCx;
}

STBPushButton * frmStatusBar::GetStatusItemByID(int nID)
{
    STBPushButton * pSTBBtn = NULL;

    // 遍历链表查找特定ID的Item
    QList<STBPushButton *>::iterator it = m_listBtnItem.begin();
    for (/**/; it != m_listBtnItem.end(); ++it)
    {
        pSTBBtn = *it;
        if (pSTBBtn->m_nID == nID)
        {
            break;
        }

        // 重新赋值，防止找不到Item的时候返回末端的指针
        pSTBBtn = NULL;
    }

    return pSTBBtn;
}

bool frmStatusBar::CheckInsertSTBPushButton(const QSize & size)
{
    int nAllItemWidth = 0;
    int nOffsetCx = 0;

    // 计算已有Item的总长度
    QList<STBPushButton *>::iterator it = m_listBtnItem.begin();
    for (/**/; it != m_listBtnItem.end(); ++it)
    {
        STBPushButton *& item = (*it);
        nAllItemWidth += item->m_size.width();
        nAllItemWidth += S_ITEM_MARGIN;
    }

    // 计算添加Item后的总长度
    nAllItemWidth += size.width() + S_ITEM_MARGIN;

    if (nAllItemWidth > ui->frmIconBar->width())
    {
        // 如果Item总长度超过Bar的长度，所有Icon均向后移动
        MoveItem(m_iFixItemCount, size.width() + S_ITEM_MARGIN);
        return true;
    }

    return false;
}

QRect frmStatusBar::GetItemRect(QSize & size, bool bInsert)
{
    int nIconX = 0;
    int nIconY = 0;
    int nAllFrontItemWidth = S_ITEM_MARGIN;

    /*Get Width*/
    QList<STBPushButton *>::iterator it = m_listBtnItem.begin();

    if (bInsert)
    {
        // 如果是以插入的方式添加Item，只需计算FixItem后面的位置
        for (int i = 0; i < m_iFixItemCount && it != m_listBtnItem.end(); ++it, i++)
        {
            STBPushButton *& item = (*it);
            nAllFrontItemWidth += item->m_size.width();
            nAllFrontItemWidth += S_ITEM_MARGIN;
        }
    }
    else
    {
        // 如果是以追加的方式添加Item，需要计算所有的item
        for (/**/; it != m_listBtnItem.end(); ++it)
        {
            STBPushButton *& item = (*it);
            nAllFrontItemWidth += item->m_size.width();
            nAllFrontItemWidth += S_ITEM_MARGIN;
        }
    }

    // 计算插入IconY的坐标
    // 右移一位，相当于除2，位移操作效率比除法高
    nIconY = (ui->frmIconBar->height() - size.height()) >> 1;
    nIconY = nIconY < 0 ? 0 : nIconY;

    // 根据对齐方式对Item的X轴坐标进行修正
    switch (m_nAlignType)
    {
    case ALIGN_TYPE_LEFT:
        {
            nIconX = nAllFrontItemWidth;
            return QRect(nIconX, nIconY, size.width(), size.height());
        }
        break;
    case ALIGN_TYPE_CENTER:
        {
            return QRect(0, 0, 0, 0);
        }
        break;
    case ALIGN_TYPE_RIGHT:
        {
            nIconX = ui->frmIconBar->width() - nAllFrontItemWidth - size.width();
            return QRect(nIconX, nIconY, size.width(), size.height());
        }
        break;
    case ALIGN_TYPE_UNKNOW:
    default:
        {
            return QRect(0, 0, 0, 0);
            break;
        }
    }
}

void  frmStatusBar::MoveItem(int iIndex, int iX)
{
    QList<STBPushButton *>::iterator it = m_listBtnItem.begin();
    for (int i = 0; it != m_listBtnItem.end(); ++it, i++)
    {
        // iIndex之前的Item不进行位移
        if (i < iIndex)
        {
            continue;
        }

        // 根据对齐的方式，修正偏移的位置
        int nOffsetCx = GetItemOffsetCx(iX);

        STBPushButton *& item = (*it);

        // 如果坐标超出了Bar的范围则移动后隐藏该图标
        if (item->x() - nOffsetCx < S_ITEM_MARGIN
                || item->x() - nOffsetCx + item->width() > ui->frmIconBar->width() - S_ITEM_MARGIN)
        {
            item->hide();
        }

        // 移动图标，先隐藏再移动
        item->move(item->x() - nOffsetCx, item->y());
    }
}

void frmStatusBar::OnBtnAccountClicked()
{
    emit clickedAccount(m_pBtnAccount);
}

void frmStatusBar::OnFrmItemClicked(qUIPushButton * pBtnItem)
{
    // 强制转换为子类指针
    STBPushButton * pItem = NULL;
    if (pBtnItem != NULL && pBtnItem->inherits("STBPushButton"))
    {
        pItem = (STBPushButton *)pBtnItem;
    }

    // 转换失败不进行后续操作
    if (NULL == pItem)
    {
        return ;
    }

    // 发出信号
    emit clickedItem(pItem);
}

void frmStatusBar::OnBtnMenuClicked()
{
    emit clickedMenu(m_pBtnMenu);
}

void frmStatusBar::OnFrmItemPressed(qUIPushButton * pBtnItem)
{
    // 强制转换为子类指针
    STBPushButton * pItem = NULL;
    if (pBtnItem != NULL && pBtnItem->inherits("STBPushButton"))
    {
        pItem = (STBPushButton *)pBtnItem;
    }

    // 转换失败不进行后续操作
    if (NULL == pItem)
    {
        return ;
    }

    ////----屏蔽Tip的功能
    // 点击Item相应的操作
//  if (m_pLabTip && !pItem->m_strTips.isEmpty())
//  {
//      QRect rcBtn = pItem->geometry();
//      QRect rcFrm = ui->frmIconBar->geometry();
//      QFont font1;
//      font1.setFamily(QString::fromUtf8("Tahoma"));
//      font1.setPointSize(18);
//      m_pLabTip->setFont(font1);
//      m_pLabTip->setGeometry(600,55,200,50);
//      m_pLabTip->move(rcFrm.left() + rcBtn.left() + rcBtn.width()/2 -100,60);
//      m_pLabTip->setAlignment(Qt::AlignCenter);
//      m_pLabTip->setText(pItem->m_strTips);
//      m_pLabTip->show();
//  }
}

void frmStatusBar::OnFrmItemReleased(qUIPushButton * pBtnItem)
{
    if (m_pLabTip)
    {
        m_pLabTip->hide();
    }
}

void frmStatusBar::OnFrmItemLongPress(qUIPushButton * pBtnItem)
{
    // 强制转换为子类指针
    STBPushButton * pItem = NULL;
    if (pBtnItem != NULL && pBtnItem->inherits("STBPushButton"))
    {
        pItem = (STBPushButton *)pBtnItem;
    }

    // 转换失败不进行后续操作
    if (NULL == pItem)
    {
        return ;
    }

    // 发出信号
    emit longPressItem(pItem);
}

qUIPushButton * frmStatusBar::GetBtnMenu()
{
    return m_pBtnMenu;
}
