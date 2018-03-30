#ifndef _ACDCALLINFO_PROCESSOR_H
#define _ACDCALLINFO_PROCESSOR_H

#include "basetalkuiprocessor.h"


struct tagACDCallInfoUIData
{
    yl::string strWaitTime;
    yl::string strCCName;
    yl::string strCCUID;
    yl::string strQueue;
    yl::string strCallsLongest;

    bool bAnswer;                       //是否已经应答



    tagACDCallInfoUIData()
    {
        Reset();
    }

    void Reset()
    {
        strWaitTime = "";
        strCCName = "";
        strCCUID = "";
        strQueue = "";
        strCallsLongest = "";

        bAnswer = false;                        //是否已经应答
    }

};

typedef struct tagACDCallInfoUIData CACDCallInfoUIData;

class CUILogicACDCallInfoUIBase;
class CACDCallInfoProcessor : public CBaseTalkUIProcessor
{
public:
    CACDCallInfoProcessor();
    virtual ~CACDCallInfoProcessor();

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
    CACDCallInfoUIData * GetACDCallInfoUIData();

public:
    // 焦点改变.
    // | bFocused | [in] 为将要改变的焦点状态. true为获取焦点，false为失去焦点
    virtual void OnFocusChanged(bool bFocused);

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

public:
    ////获取数据
    //CACDCallInfoUIData* GetACDCallInfoUIData();

protected:
    // 是否允许自定义softkey
    virtual bool IsAllowDefineSoftkey();

protected:
    // 应答来电
    void AnswerIncomingCall(bool bChannelKey = false);

    //退出ACDCallInfo界面
    void ExitACDCallInfo();

    //开启定时器功能
    void StartACDCallInfoTimer();

    //关闭定时器功能
    void StopACDCallInfoTimer();

    //定时函数
    BOOL OnTimer(msgObject & msg);

protected:
    //保存的UI指针
    CUILogicACDCallInfoUIBase * m_pTalkUI;
    //保存的UI数据
    CACDCallInfoUIData m_objACDCallInfoUIData;

    mkThreadTimer m_objExitTimer;       //退出定时器


};


#endif
