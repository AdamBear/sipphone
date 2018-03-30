#ifndef _NETCONFMGR_PROCESSOR_H
#define _NETCONFMGR_PROCESSOR_H

#include "basetalkuiprocessor.h"

struct tagNetConfMgrUIData
{
    talkui_vector<CContactInfo> listContactInfo;   //联系人信息列表
    talkui_vector<yl::string> m_listNetConfUserID;  //网络会议用户id

    int m_nCreatorIndex;

    tagNetConfMgrUIData()
    {
        Reset();
    }

    void Reset()
    {
        listContactInfo.clear();
        m_listNetConfUserID.clear();

        m_nCreatorIndex = -1;
    }

};

typedef struct tagNetConfMgrUIData CNetConfMgrUIData;

class CUILogicNetConfMgrUIBase;
class CNetConfMgrProcessor : public CBaseTalkUIProcessor
{
public:
    CNetConfMgrProcessor();
    virtual ~CNetConfMgrProcessor();

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
    CNetConfMgrUIData * GetNetConfMgrUIData();

private:
    //设置数据
    virtual void SetTalkData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);

private:
    virtual void GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    // 是否允许自定义softkey
    virtual bool IsAllowDefineSoftkey()
    {
        return false;
    }
    virtual void GetSoftkey(yl::string & strFile, yl::string & strStatus) {}

    virtual bool OnFunckeyPress(int iKey);

    // 数字键处理
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }

    //softKey 按键处理
    virtual bool OnSoftkeyPress(SoftKey_TYPE eSoftkey);

    //设置title
    virtual void SetTitle(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);
    //设置图标列表
    virtual void SetIcons(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);
    //给子界面设置数据
    virtual void SetTalkUIData(UPDATE_REASON eReason = UPDATE_DEFAULT);

public:
    virtual void DealRemoveMember();

protected:
    yl::string GetFocusName();
    bool GetIsFocusCreator();

protected:
    //保存的UI指针
    CUILogicNetConfMgrUIBase * m_pTalkUI;
    //保存的UI数据
    CNetConfMgrUIData m_objNetConfMgrUIData;
};

#endif
