#ifndef DLGTEXTMENU_H
#define DLGTEXTMENU_H

#include <QtGui>

#include "baselabel.h"

#include "cuiregionbase.h"

#include <QScrollBar>

#include "baseui/iscrollnotify.h"

class CPageCtrl;
struct TextMenuItemData
{
    bool m_bIsWrap;
    QString m_strText;
    int m_nIndex;   //item的索引
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam m_vecSoftkey;
#endif

    TextMenuItemData()
    {
        Reset();
    }

    void Reset()
    {
        m_bIsWrap = false;
        m_strText = "";
        m_nIndex = 0;
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
        m_vecSoftkey.clear();
#endif
    }
};

/* class TextMenuItem */
class CTextMenuItemIS : public CBaseLabel
{
    Q_OBJECT
public:
    CTextMenuItemIS(QWidget * parent = 0);

    ~CTextMenuItemIS();

    void SetItemIndex(int nIndex);

    int GetItemIndex();

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    void SetItemSoftkey(VecSoftkeyParam vecSoftkey)
    {
        m_vecSoftkey = vecSoftkey;
    }

    VecSoftkeyParam GetItemSoftkey()
    {
        return m_vecSoftkey;
    }
#endif

    //设置点击背景-全路径
    void SetSelectedBg(QString strSelectBg)
    {
        m_strSelectBg = strSelectBg;
    }

signals:
    //当被点击时，发出此信号，nIndex为其索引
    void ItemClicked(int nIndex);

public slots:
    void OnItemClick();

protected:
    bool eventFilter(QObject * pObj, QEvent * pEvent);
    void paintEvent(QPaintEvent * pEvent);

private:
    int     m_nIndex;       //索引标记
    bool m_bSelected;
    QString m_strSelectBg;
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam m_vecSoftkey;
#endif
};

//////////////////////////////////

/* class TextMenuFrame */
/************************************
-----TextMenuItem的使用
功能:
    给出一个字符链表，在TextMenuItem上显示
    并对多个TextMenuItem进行布局管理
    如滚动条的添加，点击响应事件，显示效果等
************************************/
class CFrmTextMenu : public QFrame
{
    Q_OBJECT

public:
    CFrmTextMenu(QWidget * parent = 0);

    ~CFrmTextMenu();

public:
    void SetData(XBTextMenuData * pData);

    void SetData(XBDirectoryData * pData);

    //设置显示序号
    //void SetShowIndex(bool bIsShowIndex = true);

    //获取当前焦点所在Item的索引
    int GetCurrentIndex();

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam GetFocusSoftkey();
#endif

    bool eventFilter(QObject * pObj, QEvent * pEvent);

    //设置索引号对应的Item获取焦点(0-7)
    void SetFocusItem(int nItemIndex = 0);

    void SetSelectedItemBg(QString strPicPath);

#if IF_FEATURE_XMLDIRECTORY
    bool CurrIsFirstItem()
    {
        return m_nCurrentItem == 0 && m_nCurrentPage == 1;
    };
    bool CurrIsLastItem()
    {
        return m_nCurrentItem == (m_vecItemData.size() - 1) && m_nCurrentPage == m_nSumPage;
    };
    void SetFirstPageFocusItem()
    {
        m_nCurrentPage = 1;
        UpdatePageData(m_nCurrentPage);
        SetFocusItem(0);
        SelectItemIndex(m_vecTextMenuItem[0]->GetItemIndex());
    }
    void SetLastPageFocusItem()
    {
        m_nCurrentPage = m_nSumPage;
        UpdatePageData(m_nCurrentPage);
        SetFocusItem(GetItemNumByPage(m_nCurrentPage) - 1);
        SelectItemIndex(m_vecTextMenuItem[GetItemNumByPage(m_nCurrentPage) - 1]->GetItemIndex());
    }
    int GetItmeSize()
    {
        return m_vecItemData.size();
    }
#endif

signals:
    void PageChanged(int, int);

    void ClickItem(int);

    void CurrentItemChange(int iIndex);

public slots:
    //向上翻页
    void OnLastPage();
    //向下翻页
    void OnNextPage();

    //点击
    void OnItemClick(int nIndex);

private:
    //数据的初始化:计算总页数，每页显示条数
    bool InitData();

    //根据页数刷新界面数据
    void UpdatePageData(int nCurrentPage = 1);

    //设置数据后，根据默认索引计算默认页号及所在Item的序号
    void GetDefaultData(int nDefaultIndex, int & nDefaultPage, int & nDefaultItem);

    void InitTextMenuItem();        //初始化界面的Item

    void ClearTextMenuItem();       //清除界面已有的Item显示

    int GetItemNumByPage(int nCurrentPage);     //获取指定页Item个数

    //根据当前页数，获取本页需要的数据及对应显示的高度，存入容器
    void GetPageItemData(int nCurrentPage, std::vector<TextMenuItemData> & vecPageItemData,
                         std::vector<int> & vecPageItemHeight);

    //选择索引
    void SelectItemIndex(int nIndex);

protected:
    void paintEvent(QPaintEvent * pEvent);

private:
    std::vector<CTextMenuItemIS *>      m_vecTextMenuItem;  //每条Item

    std::vector<TextMenuItemData>       m_vecItemData;      //要显示内容

    int     m_nSumPage;     //总页数

    int     m_nCurrentPage;     //当前所在页

    std::vector<int>            m_vecPageItem;      //每页显示项数

    std::vector<int>        m_vecItemHeight;        //每个Item的高度

    int     m_nCurrentItem;     //当前选中的Item的索引

    int     m_nCurrentFocusItem;        //当前焦点所在item序号(0-7)
#if IF_FEATURE_XMLDIRECTORY
    bool m_bHasSearchDlg;
#endif
};
////////////////////////////////////////////////////////
class CUIRegionTextMenu : public CUIRegionBase, public IScrollNotify
{
    Q_OBJECT

public:
    CUIRegionTextMenu(QWidget * parent = 0, Qt::WFlags flags = 0);
    ~CUIRegionTextMenu();

    //必须实现的函数
    void Update(void * pData);

    bool eventFilter(QObject * pObj, QEvent * pEvent);

    int GetCurrentItem();

    virtual bool RegionBaseKeypadHook(int nKeyCode, bool bPress);

    // 通知向前滚动
    virtual void OnNotifyScrollPrevious();
    // 通知向后滚动
    virtual void OnNotifyScrollNext();

protected slots:
    void OnPageChanged(int nCurrentPage, int nSumPage);

    //某个Item被点击/按下Ok
    void OnClickItem(int nIndex);

    void OnCurrentItemChange(int iIndex);

private:
    virtual void SetDefaultSoftkey(VecSoftkeyParam & softkeyParam);

private:
    /*  Ui::CUIRegionTextMenuClass ui;*/
    CTextMenuItemIS  *  m_pLabTitle;

    CFrmTextMenu * m_pTextMenu;

    CPageCtrl * m_pPageCtrl;

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    bool              m_bDefSoftKey; //是否是默认softkey
    VecSoftkeyParam m_vecGlobalSoftkey;
#endif
};

#endif // DLGTEXTMENU_H

