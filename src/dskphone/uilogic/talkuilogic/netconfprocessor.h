#ifndef _NETCONF_PROCESSOR_H
#define _NETCONF_PROCESSOR_H

#include "talkingprocessor.h"

struct tagNetConfUIData
{
    tagTalkingUIData m_TalkingUIData;
    talkui_vector<yl::string> m_listNetUserInfo;

    tagNetConfUIData()
    {
        Reset();
    }

    void Reset()
    {
        m_TalkingUIData.Reset();

        m_listNetUserInfo.clear();
    }

};

typedef struct tagNetConfUIData CNetConfUIData;

class CUILogicNetConfUIBase;
class CNetConfProcessor : public CTalkingProcessor
{
public:
    CNetConfProcessor();
    virtual ~CNetConfProcessor();

public:
    //构造完成后 绑定了UI后调用
    virtual void Init();
    //释放前解除UI绑定前调用
    virtual void Uninit();
    //设置绑定的UI
    virtual void SetBindUI(CUILogicBaseTalkUI * pTalkUI);
    //获取绑定的UI
    virtual CUILogicBaseTalkUI * GetBindUI();

    //获取保存的netconf数据
    CNetConfUIData * GetNetConfUIData();

private:
    //设置数据
    virtual void SetTalkData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);

protected:
    //获取通话状态的softkey
    virtual void GetTalkStatusSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

private:
    //softKey 按键处理
    virtual bool OnSoftkeyPress(SoftKey_TYPE eSoftkey);

    //设置title
    virtual void SetTitle(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);
    //给子界面设置数据
    virtual void SetTalkUIData(UPDATE_REASON eReason = UPDATE_DEFAULT);

protected:
    //保存的UI指针
    CUILogicNetConfUIBase * m_pTalkUI;
    //保存的UI数据
    CNetConfUIData m_objNetConfUIData;
};

#endif
