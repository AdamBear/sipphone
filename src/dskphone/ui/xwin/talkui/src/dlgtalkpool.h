#ifndef _DLG_TALK_POOL_H_
#define _DLG_TALK_POOL_H_

#include "dlgtalkmenubase.h"
#include "talkpooladapter.h"

namespace talkui
{
class CDlgTalkPool : public CDlgTalkMenuBase
{
public:
    //取得实例
    static CBaseDialog * CreateInstance();

public:
    static void talkPool_OpenPage(const yl::string & strAction);

    void setIsEnterFromIdle(bool bEnterFromIdle);

public:
    CDlgTalkPool();
    virtual ~CDlgTalkPool();

public:
    // 初始化
    virtual void Init();

    // 释放数据，将窗.口清空回复到初始化状态
    virtual void Uninit();

public:
    //设置数据
    void SetData(void * pData);

    void ReloadFrameListData();

    // 是否跳过Return To Idle，包括通话、Idle
    virtual bool IsRejectReturnToIdle()
    {
        return false;
    }

public:
    static LRESULT OnTalkPoolMsg(msgObject & objMessage);

    void OnContactItemChange();

public:
    //获取Softkey数据
    virtual void GetSoftkey(CArraySoftKeyBarData & objSoftkeyData);

public:
    virtual bool onKeyPressedEvent(int key);

public:
    //处理softkey 继承自 talkbaseui
    virtual bool OnSoftkeyPress(SoftKey_TYPE eSoftType, bool bLongPress);

protected:
    //根据序号打开页面
    void OpenPageByIndex(int iSelectIndex);

private:
    void RegisterMsg();
    void UnRegisterMsg();

    // 通过Action获取显示的名字
    yl::string GetNameByAction(const yl::string & strAction);

private:
    CTalkPoolAdapter    m_objTalkPoolAdapter;

    //保存的Action相关数据
    talkui_vector<yl::string> m_vecAction;

    int                 m_iMenuDataCount;

    bool                m_bIsEnterFromIdle; //是否从IDLE界面进入Favorite（pool）界面

};

}       //namespace talkui


#endif
