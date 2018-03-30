#ifndef C_TALK_NORMAL_ITEM
#define C_TALK_NORMAL_ITEM

#include "xwindow/xWindow.h"
#include "xwindow/xlistviewitem.h"

namespace talkui
{

class CTalkNormalItem : public xListViewItem
{
public:
    explicit CTalkNormalItem(IListViewItemAction * pAction = NULL);
    virtual ~CTalkNormalItem();

public:
    static int GetNormalItemType();
    static CTalkNormalItem * GetNormalItem(xListViewItem * pItem);

    static xListViewItem * CreateNormalItem();

    virtual void loadChildrenElements(xml_node & node);

public:
    //设置显示内容
    void SetShowText(const yl::string & strText);
    void SetTextAlign(int nAlign);

protected:
    xTextView * m_pText;    //显示的内容

};
}

#endif //C_TALK_NORMAL_ITEM
