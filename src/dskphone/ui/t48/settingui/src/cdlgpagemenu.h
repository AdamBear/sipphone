#ifndef _CDLG_PAGE_MENU_H_
#define _CDLG_PAGE_MENU_H_

#include "cdlgbasesubpage.h"
#include "setting/include/common.h"

class CDlgPageMenu;
typedef std::vector<CDlgPageMenu *> PageMenuList;

class CDlgPageMenu : public CDlgBaseSubPage
{
public:
    //取得实例
    static CBaseDialog * CreateInstance();
    CDlgPageMenu(QWidget * parent = NULL);
    ~CDlgPageMenu();

    void Init();
    void Uninit();
    virtual bool OnAction(const QString & strAction);
    bool IsAddIcon(yl::string strClickAction);
    void SetContent(MenuItemList * pItemList);  //设置列表数据
    virtual bool LoadPageData();    // 读取页面数据
    virtual bool IsStatusChanged()
    {
        return false;   // 查看页面状态是否改变
    }
    virtual bool SavePageData()
    {
        return true;   // 保存页面数据
    }
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);
    virtual void SetData(void * pData = NULL);
    bool eventFilter(QObject * pObject, QEvent * pEvent);
    //重新显示出来的时候的操作
    virtual void OnReShow();
    //点击(选中时候按OK键)
    virtual void OnListItemClicked(QString strItemAction);

    void UpdatePageMenuFromMSG();//收到指定消息后刷新列表
private:
    bool m_bResfreshOnReshow;
};


//pagemenu实例管理类，用于三级权限改变时刷新所有的pagemnu
class CPagemenuManager
{
public:
    static CPagemenuManager & GetInstance()
    {
        static CPagemenuManager objInstance;
        return objInstance;
    }

    CPagemenuManager();
    ~CPagemenuManager();

    void AddPageMenu(CDlgPageMenu * pPageMenu);
    void DeletePageMenu(CDlgPageMenu * pPageMenu);
    PageMenuList & GetinstanceList();

public:
    PageMenuList PageMenuInstanceList;
};

#define g_PageMenuManager (CPagemenuManager::GetInstance())
#endif  //_CDLG_PAGE_MENU_H_
