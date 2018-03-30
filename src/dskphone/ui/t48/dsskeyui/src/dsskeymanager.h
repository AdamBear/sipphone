#ifndef _DSSKEY_DELEGATE_H_
#define _DSSKEY_DELEGATE_H_

#include "dsskeyui/include/dsskeyuicommondefine.h"
#include "yllist.h"

// 前向声明
class qDsskeyFrame;
class qDsskeyButton;
class DssKeyLogicData;
class DsskeyBtnDisplayDelegete;

typedef YLList<qDsskeyFrame *> list_pDsskeyFrame;

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
// class CDsskeyDataDelegateBase
// {
// public:
//  CDsskeyDataDelegateBase();
//  virtual ~CDsskeyDataDelegateBase();
//
//  void BindDsskeyFrame(qDsskeyFrame* pDsskeyFrame);
//
//  void RefreshDsskeyData(const DsskeyDisplayData& objDisplayData);
//
//  void RefreshDsskeyData(int iIndex, const DsskeyDisplayData& objLogicData);
//
// public:
//  virtual void OnDsskeyFrameShow();
//  virtual void OnDsskeyFrameHide();
//
// protected:
//
//
// protected:
//  qDsskeyFrame*   m_pDsskeyFrame;
// };
//
//
// // 数据代理设置类  不被动更新
// class CDsskeyDataDelegateSetting : public CDsskeyDataDelegateBase
// {
// public:
//  CDsskeyDataDelegateSetting() {};
//  ~CDsskeyDataDelegateSetting() {};
//
//  void RefreshDsskeyData(const DssKeyLogicData& objLogicData);
// public:
//  void OnDsskeyFrameShow();
//  void OnDsskeyFrameHide();
// };

// Dsskey刷新观察者类
class CDsskeyFrameManager
{
public:
    static CDsskeyFrameManager * GetInstance();
    static void ReleaseInstance();

    void OnNotify(const DsskeyDisplayData & objDisplayData);
    void NotifyByIndex(int iIndex, const DsskeyDisplayData & objDisplayData);
    bool AddDsskeyFrame(qDsskeyFrame * pDsskeyFrame);
    bool DeleteDsskeyFrame(qDsskeyFrame * pDsskeyFrame);

    void RefreshDsskeyGroundPic(const DsskeyDisplayData & objDisplayData);
    bool SetDisplayDelegate(int iBtnIndex, DsskeyBtnDisplayDelegete * pDelegate);

    void DebindDisplayDelegate(int iBtnIndex);

    // 刷新所有当前在显示的Dsskey Frame
    void RefreshAllDisplay();

private:
    CDsskeyFrameManager() {};
    ~CDsskeyFrameManager() {};

private:
    static CDsskeyFrameManager           *          m_pOneself;
    list_pDsskeyFrame                               m_listpDsskeyFrame;
};

#define g_pDsskeyUIManager (CDsskeyFrameManager::GetInstance())
#endif
