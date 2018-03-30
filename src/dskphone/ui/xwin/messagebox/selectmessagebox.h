#ifndef __SELECT_MESSAGEBOX_H__
#define __SELECT_MESSAGEBOX_H__

#include "messageboxbase.h"
#include "xwindow/xlistviewadapter.h"
#include "xwindow/xListView.h"

#include "baseui/include/selectitem.h"
#include "baseui/include/baseuicommon.h"

enum
{
    MESSAGEBOX_SELECTBOX = MESSAGEBOX_END + 3,
};

enum SELECTBOX_STYLE
{
    SELECTBOX_STYLE_AUTO,
    SELECTBOX_STYLE_OPTIONBOX,
    SELECTBOX_STYLE_WINDOW,
};

struct SELECTMSGBOXINFO
{
    VEC_OPTION_ITEM_DATA m_vecSelectMsgBoxItem;
    bool                                m_bShowCheckIcon;
    int                                 m_SelectIndex;
    SELECTBOX_STYLE m_eStyle;

    SELECTMSGBOXINFO()
    {
        m_vecSelectMsgBoxItem.clear();
        m_bShowCheckIcon = false;
        m_SelectIndex = -1;
        m_eStyle = SELECTBOX_STYLE_AUTO;
    }
};

class CSelectMessageBox : public CMessageBoxBase, public xListViewAdapter
{
public:
    CSelectMessageBox();
    ~CSelectMessageBox();

    static CMessageBoxBase * CreateSelectMsgBox(const MSGBOX_INFO & stMsgInfo);

public:
    virtual void loadChildrenElements(xml_node & node);

protected:
    virtual void Init();
    virtual void Uninit();

public:
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

    virtual void SetTitleBar(const yl::string & strTitle);

    virtual void SetSelectInfo(SELECTMSGBOXINFO * pSelectBoxInfo);

    virtual void SetWidgetFocus();

    int GetFcousAction();

protected:
    void SetDefaultSoftKey(int nType);

    virtual void ExitMessageBox(MESSAGEBOX_RESULT eResult);

    //-----------listView----------------------
public:
    virtual void InitListViewLayout(xListView * pListView);

public:
    // 根据位置获取列表项
    virtual xListViewItem * PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem);
    // 刷新列表项数据
    virtual bool RefreshItem(xListView * pListView, xListViewItem * pItem);
    // 数据清空
    virtual void OnCleanContent(xListView * pListView);

    // IListViewItemAction
public:
    virtual void OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction);
    //-----------listView----------------------

protected:
    virtual bool OnSoftkeyAction(int nType, bool bLongPress);
protected:
    xTextView   *   m_pTitleBar;
    xListView   *   m_pListView;

private:
    //保存数据
    SELECTMSGBOXINFO m_vecSelectItemInfo;
    int                     m_nFocusAction;

    bool m_bShowCheckIcon;
};

#endif // __COMMON_MESSAGEBOX_H__
