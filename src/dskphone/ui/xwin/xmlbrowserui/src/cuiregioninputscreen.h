#ifndef CUI_REGION_INPUT_SCREEN_H
#define CUI_REGION_INPUT_SCREEN_H

#include "cuiregionbaselistscreen.h"
#include "xwindow/xlistviewadapter.h"

#if XML_BROWSER_SUPPORT_UI

struct InputItemData
{
    bool        bPassword;
    bool        bEditable;
    INPUT_TYPE  eInputType;
    yl::string  strPrompt;
    yl::string  strContent;
    yl::string  strIme;
    yl::string  strDefaultIme;
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam m_vecSoftkey;
#endif

    InputItemData()
    {
        Reset();
    }

    void Reset()
    {
        strPrompt = "";
        strContent = "";
        bPassword = false;
        strIme = "";
        strDefaultIme = "";
        bEditable = false;
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
        m_vecSoftkey.clear();
#endif
    }
};

typedef YLVector<InputItemData>   VEC_INPUT_ITEM_DATA;

class xEditTextView;
////////////////////////////////////////////////////////
class CUIRegionInputScreen : public CUIRegionBaseListScreen
{

public:
    CUIRegionInputScreen();
    ~CUIRegionInputScreen();

    // 必须实现的函数
    virtual void Update(void * pData);

    virtual void SetDefaultSoftkey(VecSoftkeyParam & softkeyParam);

    // 提交页面数据
    virtual bool GetPageData(MapPageData & PageData);

    static CUIRegionInputScreen * GetInputScreenUIRegion(CUIRegionBase * pItem);

    // 相应softkey的操作
    // softkey操作:回退一格
    void OnClickSoftKeyBackSpace();

    // softkey操作:添加.
    void OnClickSoftKeyDot();

    // softkey操作:向下一格
    void OnClickSoftKeyNextSpace();

    // softkey操作:切换输入法
    void OnClickSoftKeyChangeIME();

public:
    // 初始化列表框的布局
    virtual void InitListViewLayout(xListView * pListView);

    // 根据位置获取列表项
    virtual xListViewItem * PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem);

    // 刷新列表项数据
    virtual bool RefreshItem(xListView * pListView, xListViewItem * pItem);

    // 获取item的高度
    virtual int GetItemHeight();

public:
    // 列表项Action处理
    virtual void OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction);

protected:
    // 处理按键的事件
    virtual bool OnKeyPress(int nKey);

private:
    bool LoadInputItemData(XBInputData* pInputData);
    const VecSoftkeyParam& GetFocusSoftkey();
    const VecSoftkeyParam& GetSoftkeyByIndex(int nIndex);
    yl::string GetDefaultImeByType(INPUT_TYPE eInputType);
    yl::string GetImeByType(INPUT_TYPE eInputType);
    void UpdateImeSoftkeyInfo(VecSoftkeyParam& softkeyParam);
    void UpdateSoftkeyData();
    xEditTextView* GetFocusEditTextView();
    xEditTextView* GetEditTextViewByDataIndex(int nIndex);
    const InputItemData& GetFocusInputData();

    bool IsValidInput(INPUT_TYPE eType, yl::string strValue);

    bool IsDateValid(int iYear, int iMon, int iDay);

    // 获取item value
    const yl::string& GetItemValueByDataIndex(int nDataIndex);
    // 设置item value
    void SetItemValueByDataIndex(int nDataIndex, const yl::string& strValue);

    // 设置UI上显示的第nDataIndex项的值到保存列表中
    void SetItemDataValueByDataIndex(int nDataIndex, const yl::string & strValue);
    // 获取item data
    const yl::string & GetItemDataValueByDataIndex(int nDataIndex);

    bool CheckDataExist(int nDataIndex);

private:
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam m_vecGlobalSoftkey;
#endif
    VEC_INPUT_ITEM_DATA     m_vecItemData;      //要显示内容
    int                     m_iPreviousFocusIndex;
    bool                    m_bLineFold;        // 是否分行
};

#endif

#endif // CUI_REGION_INPUT_SCREEN_H

