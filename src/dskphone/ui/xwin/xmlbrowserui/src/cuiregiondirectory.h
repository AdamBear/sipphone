#ifndef CUI_REGION_DIRECTORY_H
#define CUI_REGION_DIRECTORY_H

#include "cuiregionbaselistscreen.h"
#include "xwindow/xlistviewadapter.h"

#if XML_BROWSER_SUPPORT_UI

struct DirecotryItemData
{
    bool        m_bIsWrap;
    yl::string  m_strText;
    int         m_nIndex;   //item的索引
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam m_vecSoftkey;
#endif

    DirecotryItemData()
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

typedef YLVector<DirecotryItemData>   VEC_DIRECTORY_ITEM_DATA;
class xEditTextView;

////////////////////////////////////////////////////////
#ifdef _WIN32
class CUIRegionDirectory : public CUIRegionBaseListScreen, public xSlotHandler, public chMsgProcesser
#else
class CUIRegionDirectory : public CUIRegionBaseListScreen, public xSlotHandler, public CMKProcessor
#endif
{

public:
    CUIRegionDirectory();
    ~CUIRegionDirectory();

    // 必须实现的函数
    virtual void Update(void * pData);

    virtual void SetDefaultSoftkey(VecSoftkeyParam & softkeyParam);

    // 处理按键的事件
    virtual bool OnKeyPress(int nKey);

    virtual void Init();

    virtual void UnInit();

public:
    // 初始化列表框的布局
    virtual void InitListViewLayout(xListView * pListView);

    // 根据位置获取列表项
    virtual xListViewItem * PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem);

    // 刷新列表项数据
    virtual bool RefreshItem(xListView * pListView, xListViewItem * pItem);

    // 加载xml布局文件
    virtual void loadChildrenElements(xml_node & node);

    // 根据显示内容调整布局
    virtual void RelayoutByContent();

private:
    bool LoadMenuItemData(XBDirectoryData * pDirectoryData);
    const VecSoftkeyParam & GetFocusSoftkey();
    const VecSoftkeyParam & GetSoftkeyByIndex(int nIndex);

protected:
#if IF_FEATURE_XMLDIRECTORY
    virtual bool onFocusMoveEvent(xView * pNewFocus, xView * pOldFocus);
    void slotOnIMEChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);
    void OnKeyWordChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

    // 停止搜索定时器
    void StopSearchTime();

    // 开启搜索定时器
    void StartSearchTime();

    //当搜索定时器时间到，开始搜索
    BOOL OnTimeOut(msgObject& msg);
#endif
#if IF_FEATURE_XMLDIRECTORY
    void Search();
    // 处理UI的Action事件
    virtual void OnXMLUIAction(XB_SOFTKEY_TYPE eType);

    // 获取页面的softkey数据
    virtual VecSoftkeyParam & GetSoftkey();
#endif

private:
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam m_vecGlobalSoftkey;
#if IF_FEATURE_XMLDIRECTORY
    VecSoftkeyParam   m_vecSearchSoftkey;
#endif
#endif
    VEC_DIRECTORY_ITEM_DATA     m_vecItemData;      //要显示内容
#if IF_FEATURE_XMLDIRECTORY
    xEditTextView    *    m_pInput;      // input控件
    mkThreadTimer     m_SearchTimer;
    int               m_iSearchDelay;
    bool              m_bInputShow;     // input控件是否显示
#endif
};

#endif

#endif // CUI_REGION_DIRECTORY_H

