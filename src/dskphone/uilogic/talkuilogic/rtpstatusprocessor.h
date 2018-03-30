#ifndef _RTPSTATUS_PROCESSOR_H
#define _RTPSTATUS_PROCESSOR_H

#include "basetalkuiprocessor.h"

//需要显示的rtpstatus 数据单元
struct tagRTPStatusItemData
{
    yl::string strTag;
    yl::string strValue;

    tagRTPStatusItemData()
    {
        Reset();
    }

    void Reset()
    {
        strTag = "";
        strValue = "";
    }
};

typedef struct tagRTPStatusItemData CRTPStatusData;


struct tagRTPStatusUIData
{



    tagRTPStatusUIData()
    {
        Reset();
    }

    void Reset()
    {

    }

};

typedef struct tagRTPStatusUIData CRTPStatusUIData;

class CUILogicRTPStatusUIBase;
class CRTPStatusProcessor : public CBaseTalkUIProcessor
{
public:
    CRTPStatusProcessor();
    virtual ~CRTPStatusProcessor();

public:
    //构造完成后 绑定了UI后调用
    virtual void Init();
    //释放前解除UI绑定前调用
    virtual void Uninit();
    //设置绑定的UI
    virtual void SetBindUI(CUILogicBaseTalkUI * pTalkUI);
    //获取绑定的UI
    virtual CUILogicBaseTalkUI * GetBindUI();

public:
//处理RTPStatus消息
    void OnRTPStatusMessage(msgObject & refMessage);

private:
    //设置数据
    virtual void SetTalkData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);

protected:
    // 通道键处理
    virtual bool OnChannelKeyPress(int iKey);

    //talking状态通道键处理
    virtual bool OnTalkingChannelKeyPress(int iKey);

    //会议状态 通道键处理
    virtual bool OnConferenceChannelKeyPress(int iKey);

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

    //获取softkey数据信息
    virtual void GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);


    // 是否允许自定义softkey
    virtual bool IsAllowDefineSoftkey();

    // softkey处理(自定义类型)
    virtual void GetSoftkey(yl::string & strFile, yl::string & strStatus);

protected:
    //退出RTPStatus显示
    void ExitRTPStatusUI();

    //选择查看 RTP 话路信息
    void SwitchRTPStatus();

    //更新 RTPStatus 数据
    void UpdateRTPStatusData();



protected:
    bool m_bShowSwitch;     //是否显示 RTP_SWITCH 的softkey

protected:
    //保存的UI指针
    CUILogicRTPStatusUIBase * m_pTalkUI;
    //保存的UI数据
    CRTPStatusUIData m_objRTPStatusUIData;

};


#endif
