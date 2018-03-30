#ifndef QCUTPAGE_LISTVIEW
#define QCUTPAGE_LISTVIEW

//#include "define/common.h"
#include <ETLLib/ETLLib.hpp>
#include <QtGui>
#include <QObject>

class CSwitchPageListViewItem : public QObject, public QListWidgetItem
{
    Q_OBJECT

public:
    //默认构造函数
    CSwitchPageListViewItem();

    //析构函数
    ~CSwitchPageListViewItem();

public:
    //获取Click_Action操作
    QString GetClickAction() const;

    //设置Click_Action操作
    void SetClickAction(const char * lpcsAction);

    //获取LongPress_Action操作
    QString GetLongPressAction() const;

    //设置LongPress_Action操作
    void SetLongPressAction(const char * lpcsAction);

    //取得显示名称
    QString GetShowName() const;

    //设置显示名称
    void SetShowName(const char * lpcsAction);

protected:
    //Click_Action操作
    QString     m_strClickAction;

    //LongPress_Action操作
    QString     m_strLongPressAction;

    //显示名称
    QString     m_strShowName;
};

class CSwitchPageListView : public QListWidget
{
    Q_OBJECT

public:
    //构造函数
    explicit CSwitchPageListView(QWidget * pParent = 0);

    //析构函数
    ~CSwitchPageListView();

public:
    // 设置每个Item的大小
    void SetItemSize(const QSize & itemSize);

    // 设置Item的字体大小
    void SetItemFont(const QFont & itemFont);

    // 设置每行每列显示的Item数量
    // 会自动计算网格大小
    void setMatrixGrid(int nX, int nY);

    // 显示某一页
    virtual void ShowPage(int nIndex);

    // 获取当前Item的显示名
    QString GetCurrentItemDisplayName();

    // 设置选中的Item
    void SetCurrentItem(int iItemIndex);

    // 设置选中的Item
    void SetCurrentItem(const QString & strItemClickAction);

    // 取得最后一页的序号
    int GetLastPageIndex();

    // 取得第一页的序号
    int GetFirstPageIndex();

    // 获得当前页面的Index
    int GetCurrentPageIndex();

    // 是否开启长按信号
    void EnableLongPress();

    //取得页面布局图标列数
    int GetMartrixX();

    //取得页面布局图标行数
    int GetMartrixY();

    // 设置ListView上的字体颜色
    void SetTextColor(const QColor & highlightedColor, const QColor & commonColor);

public slots:
    //切换到上一页
    void PageUp();

    //切换到下一页
    void PageDown();

protected:
    //显示出指定序列图标
    virtual void ShowItems(int iIndex, int nCount);

    //取得当前页面图标个数
    virtual int GetPageCount();

protected slots:
    // 菜单项点击时的处理
    virtual void OnItemClicked(QListWidgetItem * pItem);

    // 菜单项按下时的处理
    virtual void OnItemPressed(QListWidgetItem * pItem);

    // 长按菜单项定时器处理槽
    virtual void OnLongPressTimer();

signals:
    // 长按菜单项的信号
    void ItemLongPressed(QString strLongPressAction);

    // 点击菜单项的时候发出的信号
    void ItemClicked(QString strClickAction);

    // 当前页切换的时候的发出的信号
    void PageChange(int nCurrent);

    // 选中行改变信号
    void CurrentItemChanged(QListWidgetItem *, QListWidgetItem *);

protected:
    // 长按进入设置界面的时间
    static const int LONG_PRESS_TIME = 2000;

    // 最大菜单数限制
    static const int MAX_MENU_COUNT = 300;

    int              m_nCurrentPage;//当前显示页面的序号
    QString          m_strAction;//页面对应的Action，用于进行标示

    QTimer           m_timerLongPressed;//长按定时器

    int              m_nMartrixX;//页面布局图标列数
    int              m_nMartrixY;//页面布局图标行数

    QSize            m_itemSize;//设置Item的大小
    QFont            m_ItemFont;//设置item的字体样式

    QListWidgetItem * m_pLastPressItem; //记录上次press的item
    bool             m_bFlag;//用于标识是否长按，true--长按 false--点击

    bool m_bFirstIn;                    // 是否是第一次进界面
    QListWidgetItem * m_pPrePressItem;  // 记录上次Press的Item，和m_pLastPressItem有些不同
};

// 切页控件ListMode的基类，主要在里面实现了绘制Item的背景
class CSwitchPageList : public CSwitchPageListView
{
    Q_OBJECT

public:
    explicit CSwitchPageList(QWidget * pParent = 0);
    ~CSwitchPageList();

    void SetItemBGImagePath(LPCSTR lpszPath);// 设置背景图片的路径
    void SetAutoDrawItemBackground(bool bMode);// 是否自动绘制Item的背景

    void RollackPreItem();// 回退到上一个选中的Item
    bool IsAutoDrawItemBackground()
    {
        return m_bAutoDrawItemBackground;
    }

    int GetCurrentItemIndex() const;

    void Reset();

signals:
    void CurrentItemChange(QString strLongPressAction);

protected:
    virtual void SetPreviousItemBg(QListWidgetItem * previousItem);
    virtual void SetCurrentItemBg(QListWidgetItem * currentItem);
    virtual void OnItemClicked(QListWidgetItem * pItem);

protected slots:
    virtual void OnCurrentItemChange(QListWidgetItem * current, QListWidgetItem * previous);

private:
    LPCSTR  m_lpszBGImagePath;
    bool    m_bAutoDrawItemBackground;
    int     m_iPreItemIndex;
    int     m_iCurItemIndex;
};

#endif // QCUTPAGE_LISTVIEW
