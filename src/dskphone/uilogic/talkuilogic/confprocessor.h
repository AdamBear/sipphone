#ifndef _CONF_PROCESSOR_H
#define _CONF_PROCESSOR_H

#include "basetalkuiprocessor.h"

#include <yllist.h>


struct tagConfUIData
{
    YLList<CContactInfo> listContactInfo;   //联系人信息列表

//     bool bAllLocalHold;                     //是否所有路hold
//     bool bLocalHold;                        //是否本地hold
    bool bRemoteHold;                       //是否远端hold
    bool bHoldConf;                             //会议是否被hold
    bool bHasCallComming;                   //是否有新来电
    yl::string strIncomingCallNum;          //新来电号码
    int nFirstCallID;                       //第一路来电的CallID
    bool bAllowConf;                        //是否允许 出现conference 的 softkey
    bool bAllowHold;                        //是否允许出现hold
    bool bEncrypt;                          //会议是否是加密通话
    bool bAllowSecurity;                    //是否显示安全等级
    bool bAllowSplit;                       //是否允许split
    bool bAllowNewCall;                     //是否允许newcall
    bool bAllowJoin;                        //是否允许来电join
    yl::string strSecurityLevel;            //安全等级

    YLList<CCallInfo *>  listCallInfo; //成员列表
    AUDIO_CODEC_TYPE eAudioCodec;//高清类型

    tagConfUIData()
    {
        Reset();
    }

    void Reset()
    {
        listContactInfo.clear();        //联系人列表信息清除

//         bLocalHold = false;                         //是否本地hold
        bRemoteHold = false;                        //是否远端hold
        bHasCallComming = false;                    //是否有新来电
        strIncomingCallNum = "";                //新来电号码
        nFirstCallID = 0;                       //第一路来电的CallID
        bAllowConf = false;                     //是否允许 出现conference 的 softkey
        bAllowHold = false;
//         bAllLocalHold = false;
        bHoldConf = false;
        bEncrypt = false;
        bAllowSecurity = false;
        bAllowSplit = false;
        bAllowNewCall = false;
        bAllowJoin = false;
        strSecurityLevel = "";

        listCallInfo.clear();
        eAudioCodec = ACT_NONE;
    }

};

typedef struct tagConfUIData CConfUIData;

//与CallID相关的图标
struct tagCallIDWithIcons
{
    YLVector<TALKUI_ICON> vectorIcons;  //与话路相关的图标
    int iCallID;        //与话路相关的callID

    //清除所有图标
    void ClearIcons()
    {
        vectorIcons.clear();
    }

    void AddIcon(TALKUI_ICON eTalkUIIcon)
    {
        bool bHaveAddIcon = false;

        for (int i = 0; i < vectorIcons.size(); i++)
        {
            if (vectorIcons[i] == eTalkUIIcon)
            {
                bHaveAddIcon = true;
                break;
            }
        }

        //队列中没有 该图标 那么才需要插入队列
        if (!bHaveAddIcon)
        {
            vectorIcons.push_back(eTalkUIIcon);
        }
    }
};

typedef struct tagCallIDWithIcons CCallIDWithIcons;



class CUILogicConfUIBase;
class CConfProcessor : public CBaseTalkUIProcessor
{
public:
    CConfProcessor();
    virtual ~CConfProcessor();

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
    CConfUIData * GetConfData();

    //获取hold状态
    bool IsHold();

    //是否显示resume的softkey
    bool IsShowResumeSoftkey();

    //是否显示Hold的softeky
    bool IsShowHoldSoftkey();

    virtual void DealSecurityResult(const yl::string & strSecurityLevel);

public:
    //根据CallID 获取联系人信息
    CContactInfo GetContactInfoByCallID(int iCallID);


    //根据callid 获取显示图标
    virtual void GetIconListByCallID(YLList<TALKUI_ICON> & listIcons, int iCallID);

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    virtual bool IfNeedCallRecording();
#endif

protected:
    //获取当前话路的焦点联系人信息
    virtual bool GetFocusContactInfo(CContactInfo & objContactInfo);

private:
    //设置数据
    virtual void SetTalkData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);

protected:
    // 通道键处理
    virtual bool OnChannelKeyPress(int iKey);

    // 是否允许自定义softkey
    virtual bool IsAllowDefineSoftkey();
    virtual bool AllowSoftkeyShow(SoftKey_TYPE eType);

    virtual bool IsShowSecurityClassification();
    bool IsAllowRTPSoftkey();

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

    virtual void GetAlwaysBeforeSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    virtual void GetAlwaysAfterSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    virtual void RelayoutSoftkeyData(YLVector<SoftKey_TYPE> & VecSoftkey);

    //获取softkey数据信息
    virtual void GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    // softkey处理(自定义类型)
    virtual void GetSoftkey(yl::string & strFile, yl::string & strStatus);

private:
    //获取通话状态下的softkey
    void GetTalkStatusSoftKeyBarData(YLVector<SoftKey_TYPE> & VecSoftkey);
    //获取来电状态下的softkey
    void GetRingStatusSoftKeyBarData(YLVector<SoftKey_TYPE> & VecSoftkey);

    // 添加Broadsoft ACD功能的Softkey
    void SetACDSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    //判断是否多路通话同时允许显示hold
    bool IsAllowShowHoldInTalk(YLList<CCallInfo *> & listCallInfo);

    bool ProcessCallOutStatus();

protected:
    //添加与callID相关的图标
    void AddIconsByCallID(int iCallID, TALKUI_ICON eTalkUIIcon);

    //往队列中添加图标
    void AddIconToList(YLList<TALKUI_ICON> & listIcons, TALKUI_ICON eTalkUIIcon);

protected:
    //与CallID相关的图标
    YLVector<CCallIDWithIcons> m_vectorIconsWithCallID;


protected:
    //保存的UI指针
    CUILogicConfUIBase * m_pTalkUI;
    //保存的UI数据
    CConfUIData m_objConfUIData;

#if IF_TALKUI_PSTN
    bool
    m_bHasSipLine; // 会议中是否包含SIP通话（用于判断softkey等限制）
#endif

    //是否已经添加过acd的softkey
    bool                    m_bACDSoftkeySet;
    //是否已经添加过rtp的softkey
    bool                    m_bRTPSoftkeySet;
    //是否存在呼出的话路
    bool        m_bCallingRoutineExist;

};


#endif
