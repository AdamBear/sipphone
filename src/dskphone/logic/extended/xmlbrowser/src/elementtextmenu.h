#ifndef __ELEMENT_TEXT_MENU_H__
#define __ELEMENT_TEXT_MENU_H__

#ifdef IF_FEATURE_XMLBROWSER

// Element TextMenu类
class CElementTextMenu : public CElementHaveUIBase
{
public:
    CElementTextMenu();
    virtual ~CElementTextMenu();

    // 创建之后的处理
    virtual bool AfterCreate();
    // 销毁之前的处理
    virtual void BeforeDestroy();
    // 是否播放提示音
    virtual bool IsNeedPlayBeep();
    // 根据各Elment的情况更新URI
    virtual void UpdateURI(yl::string & strURI, int iFocusedItem);
    // softkey处理
    virtual bool ProcessSoftkey(XB_SOFTKEY_TYPE eType, int iFocusedItem, bool & bUpdate);
    // 获取指定控件的号码,无号码则返回空
    virtual yl::string GetItemNumberByIndex(UINT uIndex);
    // 对UI事件的处理
    virtual bool OnAction(XB_ACTION_TYPE eAction, void * pData, bool & bExitWnd, bool & bUpdate,
                          bool bDestroyOnExit = false);

private:
    // 获取焦点Item
    MenuItem * GetItem(int iFocusedItem);

    IMPLEMENT_GETXDATA(XBTextMenuData)

private:
    // 保存属性
    TextMenuAttr    m_dataAttr;
    // 保存ItemList
    List_Menu_Item  m_ListItem;
    // 传给UI的数据
    XBTextMenuData  m_dataToUI;
};

#endif // #ifdef IF_FEATURE_XMLBROWSER

#endif  // __ELEMENT_TEXT_MENU_H__
