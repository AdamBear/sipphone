#ifndef __I_LIST_ITEM_DELEGATE_H__
#define __I_LIST_ITEM_DELEGATE_H__

class CListItem;

class IListItemDelegate
{
public:
    IListItemDelegate() {};
    virtual ~IListItemDelegate() {};

public:
    // 数据索引更改时通知处理类
    virtual void OnDataIndexChange(CListItem * pItem)                        = 0;
    // 某行获得焦点
    virtual void OnItemFocusChange(CListItem * pItem, bool bPreFocus)        = 0;

    // 获取内容的位置信息
    virtual void GetContentPos(int & nLeftMargin, int & nRightMargin)         = 0;

    // 获取焦点效果图片
    virtual const char * GetFocusBg(int nDataIndex)                          = 0;

    // 获取点击效果图片
    virtual const char * GetClickBg(int nDataIndex)                          = 0;
};

#endif
