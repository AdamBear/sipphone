#ifndef _AUTOREDIAL_PROCESSOR_H
#define _AUTOREDIAL_PROCESSOR_H

#include "basetalkuiprocessor.h"


struct tagAutoRedialUIData
{
    CContactInfo objContactInfo;        //联系人信息

    AUTOREDIAL_STATUS eAutoRedialStatus;        //自动重拨状态
    // 保存自动重拨倒计时超时秒数
    int nRedialInterval;
    //剩余的重拨次数
    int nRemainRedialTimes;
    //显示自动重拨询问信息
    yl::string strInfo;


    tagAutoRedialUIData()
    {
        Reset();
    }

    void Reset()
    {
        objContactInfo.Reset();     //联系人信息

        eAutoRedialStatus = AUTOREDIAL_IDLE;        //自动重拨状态
        // 保存自动重拨倒计时超时秒数
        nRedialInterval = 0;
        //剩余的重拨次数
        nRemainRedialTimes = 0;

        strInfo = "";
    }

};

typedef struct tagAutoRedialUIData CAutoRedialUIData;

class CUILogicAutoRedialUIBase;
class CAutoRedialProcessor : public CBaseTalkUIProcessor
{
public:
    CAutoRedialProcessor();
    virtual ~CAutoRedialProcessor();

public:
    //构造完成后 绑定了UI后调用
    virtual void Init();
    //释放前解除UI绑定前调用
    virtual void Uninit();
    //设置绑定的UI
    virtual void SetBindUI(CUILogicBaseTalkUI * pTalkUI);
    //获取绑定的UI
    virtual CUILogicBaseTalkUI * GetBindUI();

    //获取数据
    CAutoRedialUIData * GetAutoRedialUIData();

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

    //获取softkey数据信息
    virtual void GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    // softkey处理(自定义类型)
    virtual void GetSoftkey(yl::string & strFile, yl::string & strStatus);

protected:
    // 是否允许自定义softkey
    virtual bool IsAllowDefineSoftkey();

protected:
    //定时函数
    BOOL OnTimer(msgObject & msg);
    //重拨、呼出
    void OnRedial();
    //取消 redial
    void OnCancel();


protected:
    //更新界面提示语
    void UpdateAutoRedialNote();


protected:
    //保存的UI指针
    CUILogicAutoRedialUIBase * m_pTalkUI;
    //保存的UI数据
    CAutoRedialUIData m_objAutoRedialUIData;

protected:
    mkThreadTimer m_objAutoRedialTimer;     //定时器
    bool m_bIsCountDown;            //是否是在倒计时状态
};


#endif
