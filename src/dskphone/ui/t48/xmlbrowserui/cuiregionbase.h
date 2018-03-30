#ifndef ELEMENT_UI_BASE_H
#define ELEMENT_UI_BASE_H

#include <QFrame>
#include <ylstring.h>
#include <map>
#include "extended/xmlbrowser/include/logicanduicommon.h"
#include "globalinputhook/ckeymap.h"
#include "picpath.h"
#include "modxmlbrowserui.h"

// RegionAction
enum REGION_ACTION_TYPE
{
    REGION_ACTION_CURRENT_ITEM_CHANGE,
    REGION_ACTION_CLICK_ITEM,
};

typedef YLHashMap<int, yl::string> MapPageData;

class CUIRegionBase : public QFrame
{
    Q_OBJECT

public:
    CUIRegionBase(QWidget * parent = 0, Qt::WFlags flags = 0);
    ~CUIRegionBase();

    // 根据数据更新界面
    virtual void Update(void * pData) = 0;

    // 事件过滤器
    virtual bool eventFilter(QObject * pObj, QEvent * pEvent) = 0;

    // 处理UI的Action事件
    virtual bool OnXMLUIAction(XB_SOFTKEY_TYPE eType);

    // 设置Element类型
    inline void SetElementType(XML_ELEMENT_TYPE eType);

    // 获取Element类型
    inline XML_ELEMENT_TYPE GetElementType();

    // 获取退出的时间
    inline int GetQuitTime();

    // 返回当前的Item
    virtual int GetCurrentItem()
    {
        return -1;
    }

    virtual bool RegionBaseKeypadHook(int nKeyCode, bool bPress);

    // 提交页面数据
    virtual bool GetPageData(MapPageData & PageData);

    // 设置页面的softkey数据
    void SetSoftkey(VecSoftkeyParam & softkeyParam);

    // 获取页面的softkey数据
    virtual VecSoftkeyParam & GetSoftkey();

    // 设置ElementID
    void SetElementID(int iID);

    // 获取ElementID
    int GetElementID();

    //处理Dsskey,处理完返回true，否则返回false
    bool ProcessDsskey(int iIndex);
    // 是否是在界面锁上时忽略的功能键
    BOOL IsIgnoredLockKey(int iKey);

    virtual  void OnKeyboardShow() {}
    virtual  void OnKeyboardHide() {}

protected:
    // 处理按键的事件
    bool OnKeyPress(QObject * pObj, QEvent * pEvent);

    virtual bool ProcessKey(int nKey);

private:
    // 判断是不是Softkey的按键
    static bool IsSoftkey(int nKey);

    // 存在通话时,XMLBroswser需要处理的按键
    static bool IsProcessInTalk(int nKey);

    // 获取页面的默认softkey数据
    virtual void SetDefaultSoftkey(VecSoftkeyParam & softkeyParam);

signals:
    // RegionAction改变需要发射的信号
    void RegionAction(REGION_ACTION_TYPE eAction);

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

#endif // ELEMENT_UI_BASE_H
