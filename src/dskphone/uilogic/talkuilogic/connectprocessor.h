#ifndef _CONNECT_PROCESSOR_H
#define _CONNECT_PROCESSOR_H

#include "basetalkuiprocessor.h"


struct tagConnectUIData
{
    CContactInfo m_objContactInfo;          //联系人信息

    bool                m_bLocalHold;       // 是否处于Hold对方状态
    bool                m_bRemoteHold;      // 是否处于对方Hold状态
    bool                m_bVideoMute;           //是否视频静音
    bool                m_bAudioMute;       //是否音频静音

    tagConnectUIData()
    {
        Reset();
    }

    void Reset()
    {
        m_objContactInfo.Reset();               //联系人信息重置

        m_bLocalHold = false;       // 是否处于Hold对方状态
        m_bRemoteHold = false;      // 是否处于对方Hold状态
        m_bVideoMute = false;           //是否视频静音
        m_bAudioMute = false;       //是否音频静音
    }

};

typedef struct tagConnectUIData CConnectUIData;

class CUILogicConnectUIBase;
class CConnectProcessor : public CBaseTalkUIProcessor
{
public:
    CConnectProcessor();
    ~CConnectProcessor();

public:
    //构造完成后 绑定了UI后调用
    virtual void Init();
    //释放前解除UI绑定前调用
    virtual void Uninit();
    //设置绑定的UI
    virtual void SetBindUI(CUILogicBaseTalkUI * pTalkUI);
    //获取绑定的UI
    virtual CUILogicBaseTalkUI * GetBindUI();

    //获取callid
    int GetCallID();
    //是否是视频通话
    bool IsVideoTalk();

    //获取数据
    CConnectUIData * GetConnectUIData();

private:
    //设置数据
    virtual void SetTalkData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);

protected:
    // 通道键处理
    virtual bool OnChannelKeyPress(int iKey);

private:
    // 功能键处理
    virtual bool OnFunckeyPress(int iKey);

    // 数字键处理
    virtual bool OnDigitkeyPress(int iKey);

    //softKey 按键处理
    virtual bool OnSoftkeyPress(SoftKey_TYPE eSoftkey);

    //设置title
    virtual void SetTitle(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);
    //设置图标列表
    virtual void SetIcons(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);
    //给子界面设置数据
    virtual void SetTalkUIData(UPDATE_REASON eReason = UPDATE_DEFAULT);

private:
    //获取softkey数据信息
    virtual void GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    //获取蓝牙softkey数据信息
    void GetBlueToothConnectSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    // 是否允许自定义softkey
    virtual bool IsAllowDefineSoftkey();

    // softkey处理(自定义类型)
    virtual void GetSoftkey(yl::string & strFile, yl::string & strStatus);


protected:
    //保存的UI指针
    CUILogicConnectUIBase * m_pTalkUI;
    //保存的UI数据
    CConnectUIData m_objConnectUIData;


};


#endif
