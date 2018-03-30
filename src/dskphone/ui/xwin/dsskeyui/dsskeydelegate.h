#ifndef _DSSKEY_DELEGATE_H_
#define _DSSKEY_DELEGATE_H_

#include "dsskeyuicommondefine.h"
#include "yllist.h"
// 前向声明
class CLinekeyUI;
class CDsskeyButton;

// 按钮响应实现类
class CAbstractDsskeyBtnAction
{
public:
    CAbstractDsskeyBtnAction() {};
    virtual ~CAbstractDsskeyBtnAction() {};

public:
    virtual void OnDsskeyAction(int iBtnIndex) = 0;
};


// Dsskey功能按钮点击响应实现类
class CDsskeyFeatureAction: public CAbstractDsskeyBtnAction
{
public:
    CDsskeyFeatureAction() {};
    virtual ~CDsskeyFeatureAction() {};

public:
    virtual void OnDsskeyAction(int iDsskeyID);
};


// Setting界面按钮点击响应实现类
class CDsskeySettingAction: public CAbstractDsskeyBtnAction
{
public:
    CDsskeySettingAction() {};
    virtual ~CDsskeySettingAction() {};

public:
    virtual void OnDsskeyAction(int iDsskeyID);
};

// Dsskey功能按钮长按响应实现类
class CDsskeyLongClickAction: public CAbstractDsskeyBtnAction
{
public:
    CDsskeyLongClickAction() {};
    virtual ~CDsskeyLongClickAction() {};

public:
    virtual void OnDsskeyAction(int iDsskeyID);
};

// 数据代理类
class CDsskeyDataDelegateBase
{
public:
    CDsskeyDataDelegateBase();
    virtual ~CDsskeyDataDelegateBase();

    void BindDsskeyFrame(CLinekeyUI* pLinekeyUI);

    void RefreshDsskeyData(const DsskeyDisplayData& objDisplayData);

    void RefreshDsskeyData(int iIndex, const DsskeyDisplayData& objLogicData);

    void UpdatePageInfo(int iCurPageNum, int iCurrentPageIndex, bool bVisible = false);

    void UpdatePageIcon(int iPageNum, DSS_KEY_COLOR_TYPE eColor, DSS_KEY_FLUSH_TYPE eFlush);
public:
    virtual void OnDsskeyFrameShow();
    virtual void OnDsskeyFrameHide();

protected:


protected:
    CLinekeyUI*   m_pLinekeyUI;
};


// 数据代理设置类  不被动更新
class CDsskeyDataDelegateSetting : public CDsskeyDataDelegateBase
{
public:
    CDsskeyDataDelegateSetting() {};
    ~CDsskeyDataDelegateSetting() {};

    void RefreshDsskeyData(const DssKeyLogicData& objLogicData);
public:
    void OnDsskeyFrameShow();
    void OnDsskeyFrameHide();
};

class CDsskeySubject
{
public:
    static CDsskeySubject* GetInstance();
    static void ReleaseInstance();

    void OnNotify(const DsskeyDisplayData& objDisplayData);
    void NotifyByIndex(int iIndex, const DsskeyDisplayData& objDisplayData);
    void AddDataDelegate(CDsskeyDataDelegateBase* pDataDelegate);
    void DeleteDataDelegate(CDsskeyDataDelegateBase* pDataDelegate);

    void OnNotifyPageInfo(int iCurPageNum, int iCurrentPageIndex, bool bVisible = false);
    void OnNotifyPageIcon(int iPageNum, DSS_KEY_COLOR_TYPE eColor, DSS_KEY_FLUSH_TYPE eFlush = REFRESH_TYPE_NO_FLUSH);

private:
    CDsskeySubject() {};
    ~CDsskeySubject() {};

private:
    static CDsskeySubject*                      m_pOneself;
    YLList<CDsskeyDataDelegateBase*>            m_listDelegate;
};

#define g_pDsskeySubject (CDsskeySubject::GetInstance())
#endif
