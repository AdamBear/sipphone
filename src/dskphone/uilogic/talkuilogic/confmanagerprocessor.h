#ifndef _CONFMANAGER_PROCESSOR_H
#define _CONFMANAGER_PROCESSOR_H

#include "basetalkuiprocessor.h"

#include <yllist.h>


struct tagConfManagerUIData
{
    talkui_vector<CContactInfo> listContactInfo;   //联系人信息列表

    bool bLocalHold;                        //是否本地hold
    bool bRemoteHold;                       //是否远端hold
    bool bHasCallComming;                   //是否有新来电
    yl::string strIncomingCallNum;          //新来电号码
    int nFirstCallID;                       //第一路来电的CallID

    tagConfManagerUIData()
    {
        Reset();
    }

    void Reset()
    {
        listContactInfo.clear();        //联系人列表信息清除

        bLocalHold = false;                         //是否本地hold
        bRemoteHold = false;                        //是否远端hold
        bHasCallComming = false;                    //是否有新来电
        strIncomingCallNum = "";                //新来电号码
        nFirstCallID = 0;                       //第一路来电的CallID
    }

};

typedef struct tagConfManagerUIData CConfManagerUIData;

class CUILogicConfManagerUIBase;
class CConfManagerProcessor : public CBaseTalkUIProcessor
{
public:
    CConfManagerProcessor();
    virtual ~CConfManagerProcessor();

public:
    //构造完成后 绑定了UI后调用
    virtual void Init();
    //释放前解除UI绑定前调用
    virtual void Uninit();
    //设置绑定的UI
    virtual void SetBindUI(CUILogicBaseTalkUI * pTalkUI);
    //获取绑定的UI
    virtual CUILogicBaseTalkUI * GetBindUI();

    //获取 会议数据
    CConfManagerUIData * GetConfManagerUIData();

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

    virtual void RelayoutSoftkeyData(YLVector<SoftKey_TYPE> & VecSoftkey);

protected:
    // 是否允许自定义softkey
    virtual bool IsAllowDefineSoftkey();

private:
    //获取通话状态下的softkey
    void GetTalkStatusSoftKeyBarData(YLVector<SoftKey_TYPE> & VecSoftkey);
    //获取来电状态下的softkey
    void GetRingStatusSoftKeyBarData(YLVector<SoftKey_TYPE> & VecSoftkey);

    // 添加Broadsoft ACD功能的Softkey
    void SetACDSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

protected:
    //根据CallID 获取联系人信息
    bool GetContactInfoByCallID(int iCallID, CContactInfo & objContactInfo);

    //获取焦点联系人信息
    bool GetFocusContactInfo(CContactInfo & objContactInfo);


protected:
    //保存的UI指针
    CUILogicConfManagerUIBase * m_pTalkUI;
    //保存的UI数据
    CConfManagerUIData m_objConfManagerUIData;

    bool            m_bAllowNewCall;
};


#endif
