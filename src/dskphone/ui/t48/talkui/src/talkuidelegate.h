#ifndef TALKUI_DELEGATE_H
#define TALKUI_DELEGATE_H
#include <ylstring.h>
#include "dlgbasetalkui.h"

#define IF_TALKUI_DEBUG 0


class CTalkUIDelegate
{
private:
    CTalkUIDelegate(void);
public:
    virtual ~CTalkUIDelegate(void);

public:
    static CTalkUIDelegate & GetInstance();

public:
    //更新通道图标
    void UpdateChannelIcon();
    //设置通道图标图片
    void SetChannelIcon(const yl::string & strChannelIcon);
    //更新 通话左上角 title
    void UpdateTitleInfo(yl::string & strTtileInfo);
    //更新 其他图标  静音、加密、HD、蓝牙、录音
    void AddTitleIcon(const yl::string & strIconPath, bool bAdd);
    ////清除所有其他图标
    //void DeleteAllTitleIcon();

    // 更新蓝牙图标
    void RefreshBlueToothIcon();

    //获取弹出框个数
    int GetMessageBoxCount();

    //移除所有弹出框
    void RemoveAllMessageBox();

    //移除对话框相关的弹出框
    void RemoveBaseTalkuiMessageBox(CDlgBaseTalkUI * pBaseTalkUI);

    //弹出普通弹出提示框
    void ShowNoteMessageBox(const QString & strNote, int nMsTime = -1);

    //判断该图片是否是默认图片
    bool IsDefaultContactPhoto(yl::string strImg);

public:
    //是否显示swap 的softkey
    bool IsShowSwapSoftkey();

    //获取显示策略
    int GetDisplayMethod();

    //在有name和number的情况下 是否优先显示name
    bool IsShowNameFirst();

    //是否显示Via 中转信息
    bool IsShowViaInfo();

    //是否显示TargetInfo 信息
    bool IsShowTargetInfo();

    //获取在拨号界面优先显示的 信息  1=Label 2=DisplayName 3=UserName 默认为3
    int GetDisplayMethodOnDialing();

    //是否呼出按键 播放按键音
    bool IsVolumeSendKeySoundOn();

    //获取联系人默认头像路径
    yl::string GetDefaultContactImage();

    //获取通话、会议 新来电目标账号ID
    int GetTalkingIncomingCallTargetAccountId();

    //从逻辑层获取hoteling 账号
    yl::string GetHotelingUserName(int iAccountID);

    //从逻辑层获取Label
    yl::string GetLabelFromLogic(int iAccountID);

    //从逻辑层获取Displayname
    yl::string GetDisplayNameFromLogic(int iAccountID);

    //从逻辑层获取UserName
    yl::string GetUserNameFromLogic(int iAccountID);

    // 滚动时间间隔
    int GetScrollTimer();

public:
    /************************************************************************
    ** 函数描述:   获取账号的全名 User@Domain
    ** 参数：      账号ID
    ** 返回:       服务器名
    ************************************************************************/
    yl::string GetFullName(int iAccountID);

public:
    // 是否*、#呼出键
    int GetKeyAsSend();

    //是否是配置中的呼出键
    bool IsDigitkeyAsSend(int iKey);

    //这个配置的意思 应该是能不能呼出## 这么理解
    int IsEnableSendPoundKey();

    //在通话界面 有新来电的时候 是否改变softkey
    bool IsSoftkeyChangedCallIncome();

public:
    //播放按键音
    void PlayKeyTone(int iKey);

};

#define _TalkUIDelegate CTalkUIDelegate::GetInstance()


#endif
