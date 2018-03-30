#ifndef CUI_REGION_FORMAT_TEXT_H
#define CUI_REGION_FORMAT_TEXT_H

#include "cuiregionbase.h"
#include "xwindow/xlistviewadapter.h"
#include "formattextadapter.h"
#include "baseui/include/xdatetimeview.h"

//#if XML_BROWSER_SUPPORT_UI
////////////////////////////////////////////////////////
class CUIRegionFormatText : public CUIRegionBase
    , public xListViewAdapter
{

public:
    CUIRegionFormatText(bool bScreenSaver = false);
    ~CUIRegionFormatText();

    // 必须实现的函数
    virtual void Update(void * pData);

    virtual void loadChildrenElements(xml_node & node);

    virtual int GetCurrentItemIndex();

    virtual void SetDefaultSoftkey(VecSoftkeyParam & softkeyParam);

public:
    // 初始化列表框的布局
    virtual void InitListViewLayout(xListView * pListView);

    // 根据位置获取列表项
    virtual xListViewItem * PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem);

    // 刷新列表项数据
    virtual bool RefreshItem(xListView * pListView, xListViewItem * pItem);
    // 获取不等高item的高度
    virtual bool GetItemHeight(xListView* pListView, int nDataIndex, int& nItemHeight);

public:
    // 列表项Action处理
    virtual void OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction);

    virtual void UpdateContent();

    virtual void RelayoutByContent();

    // 处理按键的事件
    virtual bool OnKeyPress(int nKey);
    ///////////////////xml屏保函数//////////////////////////////////
    xLinearLayout* GetTitleIcon()
    {
        return m_pTitleIcons;
    }

    bool IsRGB(yl::string strRGB, xColor & ColorRGB);

    xColor GetXMLTextColor(yl::string strRGB, TEXT_FONT_COLOR eColorType);

    int GetAlignByImageAlign(IMAGE_VERTICAL_ALIGN eVertical, IMAGE_HORIZONTAL_ALIGN eHorizontal);


private:
    bool LoadMenuItemData(XBFormattedTextData * pData);
    bool LoadMenuItemData(XBScreenSaverData * pData);

private:
    xListView       *       m_pListHead;
    xListView       *       m_pListView;
    xListView       *       m_pListFoot;
    CFormatTextAdapter   *  m_pAdapterHead;
    CFormatTextAdapter   *  m_pAdapterFoot;

    VEC_SCROLL_ITEM_DATA    m_vecItemData;      //要显示内容
    int                     m_nSelect;
    int                     m_nHeadCount;
    int                     m_nFootCount;

    ///////////////////xml屏保参数//////////////////////////////////
    bool                    m_bScreenSaver;
    xDateTimeView*          m_pClockLab; //时钟
    xImageView*             m_pImage;   //图片
    xLinearLayout*          m_pTitleIcons;//状态图标
    xFrameLayout*           m_pFrameTop;
    xFrameLayout*           m_pFrameBottom;

};

//#endif

#endif // CUI_REGION_FORMAT_TEXT_H

