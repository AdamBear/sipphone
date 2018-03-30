#ifndef _SEARCH_WIDGET_ITEM_H
#define _SEARCH_WIDGET_ITEM_H

#include "contacts/directory/include/commondata.h"
#include "xwindow/xWindow.h"
#include "xwindow/xlistviewitem.h"

namespace talkui
{

///////////////////////// CSearchItemData /////////////////////////////
struct CSearchItemData
{
    CSearchItemData();

    void Reset();

    void GetSearchItemDataFromDirectorySearchData(DirectorySearchData & objDirectorySearchData);

    yl::string m_strDisplayName;
    yl::string m_strNumber;
    yl::string m_strContactImage;
    _PRESENCE_TYPE m_eContactStatus;
    bool m_bHold;
    int m_iCallID;

    //重载 = 操作符
    CSearchItemData & operator=(const CSearchItemData & objSearchItemData)
    {
        if (this == &objSearchItemData)
        {
            return *this;
        }

        this->m_strDisplayName = objSearchItemData.m_strDisplayName;
        this->m_strNumber = objSearchItemData.m_strNumber;
        this->m_strContactImage = objSearchItemData.m_strContactImage;
        this->m_eContactStatus = objSearchItemData.m_eContactStatus;
        this->m_bHold = objSearchItemData.m_bHold;
        this->m_iCallID = objSearchItemData.m_iCallID;

        return *this;
    }



};

//判断两个数据是否相同
bool IsSearchItemDataEqual(const CSearchItemData & objSearchItemData1,
                           const CSearchItemData & objSearchItemData2);


///////////////////////// CSearchWidgetItem /////////////////////////////
class CSearchWidgetItem : public xListViewItem
{
public:
    explicit CSearchWidgetItem(IListViewItemAction * pAction = NULL);
    virtual ~CSearchWidgetItem();

    static int GetSearchItemType();
    static CSearchWidgetItem * GetSearchItem(xListViewItem * pItem);

    static xListViewItem * CreateSearchItem();

    virtual void loadChildrenElements(xml_node & node);

public:
    //获取焦点item的数据, 有焦点数据返回true， 没有焦点数据返回false
    void GetContactData(yl::string & strDisplayName, yl::string & strNumber, int & iCallID);

    void GetSearchTableItemData(CSearchItemData & objSearchItemData);

    //设置数据
    void SetSearchTableItemData(const CSearchItemData & objSearchItemData);

protected:
    //设置名称和号码  这个要一块设置因为名称和号码相同是要只显示名称的
    void SetNameAndNumber(const yl::string & strName, const yl::string & strNumber);

protected:
    //重置所有信息
    void ResetAllInfo();

    void SetNameShow(bool bShow = true, bool bGone = true);
    void SetNumberShow(bool bShow = true, bool bGone = true);

protected:
    //搜索的数据
    CSearchItemData m_objSearchItemData;

protected:
    xTextView * m_pName;    //名称
    xTextView * m_pNumber;  //号码
};

}



#endif
