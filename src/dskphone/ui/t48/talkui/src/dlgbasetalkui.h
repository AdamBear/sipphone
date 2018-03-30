#ifndef __DIALOG_BASE_TALKUI_H__
#define __DIALOG_BASE_TALKUI_H__

//mod-QT
#include <QtGui>

//mod-base
#include "talkui_define.h"

//mod-UIManager
#include "uimanager/uimanager_inc.h"

//mod-Talk
#include "talk/talklogic/include/uiandlogic_common.h"

#include "customtime.h"
#include "talk/talkadapter/include/modtalkadapter.h"


//前置声明
class qSoftButton;

#ifdef _T48
class CContactInfoFrame;
#endif


//通话UI基础类，用于所有通话UI的基类
#ifndef _T2X
class CDlgBaseTalkUI: public CBaseDialog
{
    Q_OBJECT
#else
//黑白屏的类名
{
#endif

public:
    //  CDlgBaseTalkUI(void);
    //构造函数，需传递UI名与UI类型
    CDlgBaseTalkUI(const yl::string & strTalkUIName, TALK_UI_TYPE eTalkUIType);
    virtual ~CDlgBaseTalkUI(void);

    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    virtual void SetLogicData(DataLogic2UI & objData);

public:
    virtual void SetTalkData(DataLogic2UIBase * pData);

    //通道键处理
    virtual bool OnChannelKeyPress(int iKey);
    //功能键处理
    virtual bool OnFunckeyPress(int iKey, bool bEXP = FALSE);
    //数字键处理
    virtual bool OnDigitkeyPress(int iKey);
    //按键弹起处理
    virtual bool OnKeyRelease(int iKey);

    // softkey处理  GetSoftKeyActionByType统一转换成string来处理
    // 因为目前 t2x 和 t4x 的参数不不相同的 所以这么处理
    virtual bool OnSoftkeyPress(SoftKey_TYPE eSoftkey);

    //是否显示头像情况变化后重置控件位置
    virtual void ResetWidgetByContactPhotoVisibleChange() {}

    //TalkUIName相关
public:
    //获取TalkUI名称
    yl::string GetTalkUIName();
    //设置TalkUI名称
    void SetTalkUIName(yl::string strTalkUIName);

    //SessionID相关
public:
    //设置关联的SessionId
    void SetSessionId(int iSessionId);
    //获取关联的SessionId
    int GetSessionId();

    bool GetLoadCustomImage(int nIndex);

    //一个通话界面可能存在多个头像 参数代表索引
    bool IsDisplayContactPhoto(int nIndex = 0);

    //是否存在显示的头像
    bool HasDisplayContactImage();

protected:
    //设置关联Session的状态
    void SetSessionState(SESSION_STATE eSessionState);
    //显示security的弹出框
    void ShowSecurityBox(int nAccount);

public:
    //获取关联Session的状态
    SESSION_STATE GetSessionState()
    {
        return m_eSessionState;
    }


    //获取softkey相关
public:
    //设置Softkey数据Action值
    void GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL);
public:
    // 添加特定类型的softkey
    void AddSoftkey(CArraySoftKeyBarData & objSoftkeyData,
                    const yl::string & strTalkUIAction,
                    const QString & strTitle = "");

    bool IsExistBySoftkeyArrayData(CArraySoftKeyBarData & objSoftkeyData, \
                                   const yl::string & strTalkUIAction, \
                                   const QString & strTitle);

    // 设置softkey
#ifdef _T48
    void SetSoftkeyIcon(CArraySoftKeyBarData & objSoftkeyData);

    // softkey的类型
    const char * GetSoftkeyIconByAction(const yl::string & strTalkUIAction);
#endif
    // 获取配置softkey的文件名和结点名
    virtual void GetCustomSoftkeyData(yl::string & strFile, yl::string & strStatus) {};
#if IF_FEATURE_CUSTOM_SOFTKEY
    virtual void GetDynamicSoftkey(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL) {}
    void InsertSoftKey(CArraySoftKeyBarData & oldSoftkeyData, CArraySoftKeyBarData & newSoftkeyData);
    virtual void AdjustCustomSoftkey(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL);
    virtual bool IsSoftkeyAllowToShow(const yl::string & strSoftkeyAction, DataLogic2UIBase * pData = NULL);
#endif
    // 读取配置表的softkey信息
    bool LoadCustomSoftkey(CArraySoftKeyBarData & objSoftkeyData,
                           const yl::string & strFile,
                           const yl::string & strStatus);
    //设置Softkey数据Action值
    virtual void GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL) {};
    // softkey的类型转换到对用的Action
    const char * SoftkeyTypeToAction(const yl::string & strSoftkeyType);
    // 通过softkey的Action获取Title
    QString SoftkeyActionToTitle(const yl::string & strTalkUIAction);

    // 绑定softkey与action
    void BindSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData);

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    void AddUSBRecordSoftkey(CArraySoftKeyBarData & objSoftkeyData);

    // 开始USB录音
    bool StartUsbRecord();
#endif

    virtual void RefreshRecordSoftKey(CArraySoftKeyBarData & objSoftkeyData) {};
    //设置类
public:
#ifdef _T48
    // 设置talkUI的位置,同时调整子页面中控件的布局
    void SetTalkUIRect(const QRect & rect);
#endif
    //设置通话UI类型
    void SetTalkUIType(TALK_UI_TYPE eTalkUIType);


    //获取类
public:
    // 根据CallInfo获取record图标类型
    ICON_TYPE GetRecordIconType(CCallInfo * pCallInfo);
    //获取通话UI类型
    TALK_UI_TYPE GetTalkUIType() const;
    // 获取record图标类型
    virtual ICON_TYPE GetRecordIconType()
    {
        return IT_UNKNOWN;
    }

    //获取通话时间
    virtual QCustomTime GetTalkTime();
    //输入所有输入框内容
    virtual bool GetInputContent(yl::string & strNum, yl::string & strDisplayname)
    {
        return false;
    }

    //// 根据callinfo获取显示名称
    //static yl::string GetNameFromCallInfo(CCallInfo* pCallInfo);

    //// 根据callinfo获取显示号码
    //static yl::string GetNumberFromCallInfo(CCallInfo* pCallInfo);

    ////根据callinfo获取fullcontactinfo
    //static yl::string GetFullContactInfoFromCallInfo(CCallInfo* pCallInfo);

public:
    ////获取TargetInfo 信息 主要是给 通话、会议 提供新来电 targetinfo 信息  获取的优先级为 Label、DisplayName、UserName
    //yl::string GetTargetInfo();

    ////获取Label  Label 不存在那么 获取GetDisplayName 函数数据
    //yl::string GetLabel(int iAccountID);

    ////获取Displayname Display 如果获取不成功 那么用 GetUserName 函数数据
    //yl::string GetDisplayName(int iAccountID);

    ////获取UserName 数据
    //yl::string GetUserName(int iAccountID);


    //询问类
public:
    // 子界面是否需要显示Tip
    virtual bool IsShowTip()
    {
        return false;
    }
    //是否显示DTMF条
    virtual bool IsShowDtmfLine() const;
    // 是否显示切换话路的滚动条
    virtual bool IsShowSessionScrollBar()
    {
        return true;
    }
    // 是否显示切换默认话路的按钮
    virtual bool IsShowSelectLineBtn()
    {
        return false;
    }
    //是否显示音量条
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                 QRect & rtVolume, yl::string & strBGPath) const;
    // 是否运行自定义softkey
    virtual bool IsAllowDefineSoftkey()
    {
        return true ;
    }
    //是否显示加密图标,rectEncrypt hold the location of encrypt label
    virtual bool IsShowEncryptTip(void * pData, QRect & rectEncrypt);
    // 能否切换Session
    virtual bool IsChangeSession()
    {
        return true;
    }
    // 是否允许键盘事件通过
    virtual bool AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress);
    // 判断是否需要播放按键音
    virtual bool IsNeedToPlayKeyTone(int iKey);
    // 判断Softkey是否需要播放按键音
    virtual bool IsSoftkeyNeedToPlayKeyTone(yl::string strSoftkeyAction);

#ifdef _T48
    // 是否允许鼠标事件通过
    virtual bool AllowMouseEventOnKeyLock(QObject * pObject, QMouseEvent * pEvent);
#endif

    // 是否允许给逻辑层发送Action
    virtual bool IsAllowSendAction(TALK_ACTION_TYPE eActionType);

    // 是否允许键盘事件通过
    virtual bool AllowSoftkeyResponseEvent(const QString & strAction);

    // 是否允许softkey事件通过
    bool AllowSoftKeyEventOnKeyLock(const QString & strAction);

protected:
    // 通道按键是否退出
    bool IsChannelKeyToExit(int iKey);
    // 是否*、#呼出键
    bool IsSendDigitKey(yl::string strEditContent, int iKey);

protected:
    //// 名字号码显示方式
    //// 配置中有几种显示方式 0.只显示名称 1.只显示号码 2.名称+号码都显示 3.号码+名称显示 4. 显示FullContactInfo
    //// 1. 只有strName有值 存放的是名称
    //// 2. 只有strName有值 存放的是号码
    //// 3. strName 和 strNumber 都有值  strName存放的是名称 strNumber存放的是号码
    //// 4. strName 和 strNumber 都有值  strName存放的是号码 strNumber 存放的是名称
    //// 5. strName有值 存放的是 FullInfo
    ////第四个参数 是显示联系人详情策略
    //// 第五个参数是 联系人姓名和号码相同时 是否隐藏号码  默认是隐藏的
    ////这个函数 是用于 两个控件显示 联系人详情的时候 使用的
    ////最后 strName 对应联系人姓名控件   strNumber 对应于联系人号码控件
    //void ChangeNameAndNumberByInfoMethod(yl::string& strName, yl::string& strNumber, yl::string& strFullContactInfo, SHOW_CONTACTDETAIL_TYPE showContactDetailType, bool bIsMergerNameAndNumber = true);

    //// 来电显示方式 只有一个名称控件
    ////执行该函数后  strName 用于给名称控件 赋值
    ////显示 name + number  或者 Number + name 时候  用空格隔开
    //void ChangeNameAndNumberByInfoMethodOnlyShowName(yl::string& strName, yl::string& strNumber, yl::string& strFullContactInfo, SHOW_CONTACTDETAIL_TYPE showContactDetailType, bool bIsMergerNameAndNumber = true);

    //执行类
    // 显示安全级别
    void ChangeNameAndNumberBySecurityLevel(yl::string & strName, yl::string & strNumber);

public:
#ifdef _T48
    void ExitTalkUI();
#endif
    // NetWork、Dtmf、音量条等Tip显变化时的处理
    virtual void OnTipChange(TALK_TIP_TYPE eTipType);
    // 焦点切换
    virtual void OnFocusChanged(bool bFocus);
    // 添加或者删除上的Icon
    // strIconPath | Icon的路径
    // bAdd | true 添加  false删除
#ifdef _T48
    virtual void AddOrDeleteIcon(LPCSTR strIconPath, bool bAdd);
    //删除所有图标
    virtual void DeleteAllIcon();
#endif
    // 切换通话账号
    void ChangeAccount(int iAccountID, bool bRefresh/* = true*/);

#ifdef _T48
    // 调整控件布局
    virtual void AdjustLayout();
#endif

public:
    //音量条变化调用函数
    virtual void OnTalkVolumnValueChange();

public:
    //UI界面请求操作
    bool SendUIRequestAction(TALK_ACTION_TYPE eActionType, WPARAM wActionParam = 0,
                             LPARAM lActionParam = 0, void * pExtraData = NULL);

public:
    ////TalkWidget回调相关
    //static void s_TALKWIDGET_CALLBACK(void* pData, TALKWIDGET_CALLBACK_TYPE talkWidgetCallBackType);
    //virtual void OnTalkWidgetAction(TALKWIDGET_CALLBACK_TYPE talkWidgetCallBackType);
//protected:
//  CBaseTalkLogic* m_pTalkLogic;

protected:
    int m_iSessionID;
    yl::string m_strTalkUIName;
    std::vector<bool> m_vecContactCustomImage;    //所有头像自定义头像加载情况
    //如果为true表示加载自定义图片成功

#ifndef _T2X    //ndef _T2X开始

public:
    //处理UI事件
    virtual bool OnUIAction(const QString & strAction);

    //处理USB事件
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    bool OnUIUSBAction(const QString & strAction);
#endif

public slots:
    //softkey按下响应事件槽
    virtual void OnSoftkeyClicked(qSoftButton * pSoftButton);

signals:
    // 改变Title
    void ChangeTitle(const yl::string & strTitle);
    // 刷新Softkey
    void RequestRefreshSoftkey();

    // 改变正在使用的账号信息
    void ChangeUserAccount(const yl::string & strAccountNum,
                           int iAccountID);

    // 添加或者删除Title上的Icon
    // strIconPath | Icon的路径
    // bAdd | true 添加  false删除
    void AddTitleIcon(const yl::string & strIconPath, bool bAdd);

    // 切换话路
    void SwitchSession();

protected:
//#ifdef _T48
//  // 显示用户信息
//  CContactInfoFrame*  m_pConInfoFrame;
//#endif

    //关联Session的状态
    SESSION_STATE       m_eSessionState;

    int                 m_nCallID;

    //TalkUI的类型
    TALK_UI_TYPE    m_eTalkUIType;

    bool    m_bLoadContactPhoto;        //是否成功加载到外部头像
#else

#endif      //ndef _T2X结束


#ifndef _T2X

protected:
    // 对swap操作进行加锁操作
    static void LockSwap();
    static bool IsSwapLock();

    // 是否对swap操作进行上锁
    // 34227的容错方案,防止用户过快的进行swap操作
    static bool     m_bLockSwap;

protected slots:
    // 对swap操作进行解锁操作
    static void UnlockSwap();

#endif

};

#endif
