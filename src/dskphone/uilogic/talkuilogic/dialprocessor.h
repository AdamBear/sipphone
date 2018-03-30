#ifndef _DIAL_PROCESSOR_H
#define _DIAL_PROCESSOR_H

#include "basetalkuiprocessor.h"


struct tagDialUIData
{
    int m_iCurrentAccountID;                //当前的账号ID
    //当前通话routine的状态   主要是用于callparkchannelkey处理
    TALK_STATE m_eTalkState;
    //拨号界面的类型
    DIAL_UI_TYPE m_eDialUIType;

    // transfer to穿梭的目标选择
    YLList<CCallInfo *> m_listTranCallInfo;
    //会议目标选择
    YLList<CCallInfo *> listConfCallInfo;

#if IF_FEATURE_DIAL_ON_HOLD
    bool m_bAllowTran;
#endif //
    CContactInfo
    m_objTransferContactInfo;      //拨号界面是transfer时  当前通话联系人的信息


    // 已拨号码
    yl::string m_strNumber;
    // 记录进入预拨号界面的按键键值
//  int m_iKeyCode;     //这个根据逻辑层的说法 UI不用  因为逻辑层自己会把这个转成字符

    //transfer 的时候  发起者的callid
    int m_iCallIdSponsor;
    ////transfer 的时候发起者的通话信息
    //CCallInfo* m_pCallInfoSponsor;

    CALL_LINE_TYPE      m_eCallLineType;    //通话线路类型，用来区分sip、pstn、pcmode

    int m_iKeyCode;

    tagDialUIData()
    {
        Reset();
    }

    void Reset()
    {
        m_iCurrentAccountID = -1;               //当前的账号ID
        //当前通话routine的状态   主要是用于callparkchannelkey处理
        m_eTalkState = TS_UNKNOW;
        //拨号界面的类型
        m_eDialUIType = DIAL_UI_UNKNOWN;

        //穿梭数据重置
        m_listTranCallInfo.clear();

        listConfCallInfo.clear();

        m_objTransferContactInfo.Reset();       //拨号界面是transfer时  当前通话联系人的信息

        //已拨号码重置
        m_strNumber = "";

        //按键重置
        //  m_iKeyCode = PHONE_KEY_NONE;        //这个根据逻辑层的说法 UI不用  因为逻辑层自己会把这个转成字符

        //transfer 的时候  发起者的callid
        m_iCallIdSponsor = -1;

#if IF_FEATURE_DIAL_ON_HOLD
        m_bAllowTran = false;
#endif //
        //transfer 的时候发起者的通话信息
        //m_pCallInfoSponsor = NULL;

        m_eCallLineType = CALL_LINE_SIP;

        m_iKeyCode = PHONE_KEY_NONE;
    }

};

typedef struct tagDialUIData CDialUIData;

class CUILogicDialUIBase;
class CDialProcessor : public CBaseTalkUIProcessor
{
public:
    CDialProcessor();
    virtual ~CDialProcessor();

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
    //获取拨号界面类型
    DIAL_UI_TYPE GetDialUIType();

    //transfer 状态下 获取当前通话联系人信息
    CContactInfo GetTransferContactInfo();

    int GetLineId()
    {
        return m_objDialUIData.m_iCurrentAccountID;
    }

    //获取当前通话账号
    int GetSipAccount();

    //获取保存的数据
    //保存的UI数据
    CDialUIData * GetDialUIData();

    virtual bool NeedReUpdate();

public:
    //处理 删除
    void DealDelete();
    //处理cancel   可以给talkuidial调用
    void DealCancel();

    //处理 ui 一些action
    void DealUIAction(yl::string strUIAction);

    //是否允许切换账号
    //t48 / t49 在拨号和预拨号界面是允许切换账号的， 其他状态下不允许切换   就是左上角点击会出现账号列表
    //其他机型也有切换账号的功能， 是在softkey 上体现
    bool IsAllowChangeAccount();

    virtual void OnLineSelectResult(LINE_SELECT_RESULT eResult, int nSelectIndex, int nSelectID);

    //处理账号改变消息
    LRESULT OnDefaultAccountChange(msgObject & refMessage);
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

    // softkey处理(自定义类型)
    virtual void GetSoftkey(yl::string & strFile, yl::string & strStatus);

    //获取总是存在的softkey（不受softkey layout配置控制）
    //处于最前
    virtual void GetAlwaysBeforeSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);
    //处于最后
    virtual void GetAlwaysAfterSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    virtual void RelayoutSoftkeyData(YLVector<SoftKey_TYPE> & VecSoftkey);

protected:
    // 是否允许自定义softkey
    virtual bool IsAllowDefineSoftkey();
    //是否允许某种类型softkey
    virtual bool AllowSoftkeyShow(SoftKey_TYPE eType);

    // 判断action是否需要播放按键音
    //硬按键也需要播放按键音
    //如果硬按键 和action 都判断要播放按键音的 那么 硬按键不播放了  统一由action播放
    //目前该函数 仅有 拨号界面使用
    virtual bool IsSoftkeyNeedToPlayKeyTone(SoftKey_TYPE softkeyType);

    //实际判断硬按键是否播放按键音
    bool IsNeedToPlayKeyTone(yl::string strEditContent, int iKey);

public:
    // 判断是否需要播放按键音
    //action 也需要判断是否播放按键音
    //如果硬按键 和action 都判断要播放按键音的 那么 硬按键不播放了  统一由action播放
    virtual bool IsNeedToPlayKeyTone(int iKey);

protected:
    //普通拨号界面通道键处理
    virtual bool OnNormalChannelKeyPress(int iKey);
    //Forward 状态下 按键处理
    virtual bool OnForwardChannelKeyPress(int iKey);
    //callpark 状态下 按键处理
    virtual bool OnCallParkChannelKeyPress(int iKey);
    // 解析号码与名字
    bool ParseDialNumber(LPCSTR lpszDial, DataDial2Logic & objData);

protected:
    //获取转接通话ID
    int GetShuttleCallID();
    //编辑框是否为空
    bool IsEditLineEmpty();
    //获取编辑框数据
    yl::string GetLineEditData();
    //联系人控件是否有选中联系人
    bool IsSearchTableSelectItem();
    //让UI输出预编辑字符
    void InputLineEditData();
    //获取号码 和联系人名称
    bool GetInputContent(yl::string & strNum, yl::string & strDisplayname);
    //把联系人控件中的联系人信息设置到编辑框中
    void SetLineEditDataBySearchTable();

    void SelectSelectLine();

#if IF_TALKUI_EXECUTIVE
    void SelectCallInitiationLine();
#endif

public:
    //处理 发送事件
    void OnSend(bool bAudioDial = true);
    //处理transfer
    void OnTransfer();
    //处理park
    void OnPark();
    //处理GPark
    void OnGPark();
    //处理RETRIEVE_PARK
    void OnRetrievePark();
    //处理DPickUp
    void OnDPickUp();
    //处理GPickUp
    void OnGPickUp();

public:
    //编辑框字符变化处理函数
    void OnTextChange(yl::string strText);

private:
    bool IsBTAccountAvailable();
    bool IsPSTNAccountAvailable();

    yl::string GetAccountTitleText(DataDial * pDialData);

protected:
    bool IsAllowGPickUpSoftkey();
    bool IsAllowDPickUpSoftkey();
    bool IsAllowZPickUpSoftkey();
    bool IsAllowRetrieveParkSoftkey();

protected:
    //上次保存的 搜索字符串变换数据
    //用来判断是否刷新softkey  拨号界面 从编辑框有字符到无字符    从无字符到有字符  需要刷新softkey
    //以防多次刷新softkey 出现bug， 例有两页softkey的时候， 当前处于第二页  如果输入数字那么会造成刷新softkey变成第一页
    yl::string m_strLastKey;

protected:
    //保存的UI指针
    CUILogicDialUIBase * m_pTalkUI;
    //保存的UI数据
    CDialUIData m_objDialUIData;

    vecLineSelectData m_vecLineSelectData;
#if IF_TALKUI_EXECUTIVE
    vecLineSelectData m_vecCallInitiationData;
#endif

    bool                    m_bGPickUpSoftkeySet;
    bool                    m_bRetrieveSoftkeySet;
    bool                    m_bDPickUpSoftkeySet;
    bool                    m_bZPickUpSoftkeySet;

};


#endif
