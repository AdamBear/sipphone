#ifndef ELEMENT_UI_BASE_H
#define ELEMENT_UI_BASE_H

#include <ylstring.h>
#include <ylhashmap.h>
#include "extended/xmlbrowser/include/logicanduicommon.h"
#include "keymap.h"
#include "xmlbrowserui/include/modxmlbrowserui.h"
#include "xwindow/xWindow.h"
#include "xwindow-kernel/include/xsignal.h"

#if XML_BROWSER_SUPPORT_UI

static VecSoftkeyParam sVecSoftParam;

// RegionAction
enum REGION_ACTION_TYPE
{
    REGION_ACTION_CURRENT_ITEM_CHANGE,
    REGION_ACTION_CLICK_ITEM,
};

typedef YLHashMap<int, yl::string> MapPageData;

class CUIRegionBase : public xFrameLayout
{

public:
    CUIRegionBase();
    ~CUIRegionBase();

    // 根据数据更新界面
    virtual void Update(void * pData) {};

    // 刷新界面内容
    virtual void UpdateContent() {};

    // 处理UI的Action事件
    virtual void OnXMLUIAction(XB_SOFTKEY_TYPE eType);

    // 设置Element类型
    inline void SetElementType(XML_ELEMENT_TYPE eType);

    // 获取Element类型
    inline XML_ELEMENT_TYPE GetElementType();

    // 获取退出的时间
    inline int GetQuitTime();

    // 返回当前的Item
    virtual int GetCurrentItemIndex()
    {
        return -1;
    }

    virtual bool RegionBaseKeypadHook(int nKeyCode, bool bPress);

    // 提交页面数据
    virtual bool GetPageData(MapPageData & PageData);

    // 设置页面的softkey数据
    void SetSoftkey(const VecSoftkeyParam & softkeyParam);

    // 获取页面的softkey数据
    virtual VecSoftkeyParam & GetSoftkey();

    // 设置ElementID
    void SetElementID(int iID);

    // 获取ElementID
    int GetElementID();

    // 获取页面的默认softkey数据
    virtual void SetDefaultSoftkey(VecSoftkeyParam & softkeyParam);

    virtual void Init() {}

    virtual void UnInit() {}

protected:
    // 处理按键的事件
    virtual bool OnKeyPress(int nKey);

    bool ProcessKey(int nKey);

    // 是否是在界面锁上时忽略的功能键
    BOOL IsIgnoredLockKey(int iKey);

private:
    // 判断是不是Softkey的按键
    static bool IsSoftkey(int nKey);

    // 存在通话时,XMLBroswser需要处理的按键
    static bool IsProcessInTalk(int nKey);

public:
    xSignal            m_signalAction;

protected:
    XML_ELEMENT_TYPE   m_eType; // 对应Element类型
    VecSoftkeyParam    m_vecSoftkey;// Region对应的softkey数据
    bool               m_bLock; // 是否锁定键盘
    int                m_iQuitTime;// 退出的时间
    int                m_iElementID;// 对应的logicElementID
};

// 设置Element类型
inline void CUIRegionBase::SetElementType(XML_ELEMENT_TYPE eType)
{
    m_eType = eType;
}

// 获取Element类型
inline XML_ELEMENT_TYPE CUIRegionBase::GetElementType()
{
    return m_eType;
}

inline int CUIRegionBase::GetQuitTime()
{
    return m_iQuitTime;
}
#endif

#endif // ELEMENT_UI_BASE_H
