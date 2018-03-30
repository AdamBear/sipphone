#pragma once

#define CASE_VALE_RET(value) case value: return #value

typedef const char* ListenerName;

enum
{
    SIG_METHOD_NONE        = 0,
    SIG_METHOD_POST        = 0x01 << 0,
    SIG_METHOD_SEND        = 0x01 << 1,
    SIG_METHOD_REQUEST     = 0x01 << 2,
    SIG_METHOD_DEF         = SIG_METHOD_POST | SIG_METHOD_SEND,
};

enum
{
    SIG_RET_BREAK = 0x01 << 10,
};

enum
{
    SIG_PRIORITY_HIGHEST,
    SIG_PRIORITY_HIGH,
    SIG_PRIORITY_NORMAL,
    SIG_PRIORITY_LOW,
    SIG_PRIORITY_LOWEST,
};

enum TALK_SIGNAL_TYPE
{
    TALK_SIG_UNKOWN,

    TALK_SIG_SESSION_PRE_ROUTINE_CHANGED,       //
    TALK_SIG_SESSION_ROUTINE_CHANGED,           // routine切换
    TALK_SIG_SESSION_FARHOLD_CHANGE,            // far hold
    TALK_SIG_SESSION_FARMUTE_CHANGE,            // far mute
    TALK_SIG_SESSION_HOLD_CHANGE,               // hold
    TALK_SIG_SESSION_HELD_CHANGE,               // held
    TALK_SIG_SESSION_IPVPPARAM_CHANGE,          // 音频参数
    TALK_SIG_SESSION_SIPPARAM_CHANGE,           // sip参数变更
    TALK_SIG_SESSION_ACCOUNT_CHANGE,            // 账号变更
    TALK_SIG_SESSION_CALLID_CHANGE,             // CallID改变
    TALK_SIG_SESSION_REMOTE_INFO_CHANGE,        // 远端通话信息更变
    TALK_SIG_SESSION_CALLID_IMAGE_CHANGE,       // 联系人头像变更
    TALK_SIG_SESSION_FOCUS_CHANGE,              // UI焦点变更
    TALK_SIG_GET_DATA_TO_UI,                    // 获取刷新数据
    TALK_SIG_SESSION_TEXT_CHANGE,               // 拨号数据变更
    TALK_SIG_ACTION_EVENT,                      // UI事件
    TALK_SIG_SESSION_INFO_CHANGED,              // 界面显示元素(含多种子元素)
    TALK_SIG_SESSION_CONTRACT_CHANGED,          // 联系人变更  TODO
    TALK_SIG_SESSION_HOST_CHANGE,               // host关系改变
    TALK_SIG_SESSION_HIDE_CHANGE,               // hide状态改变
    TALK_SIG_SUB_SESSION_ACTIVED,               // 多路中的子话路被激活
    TALK_SIG_RECV_PAGE,                         // 收到Pagin请求
    TALK_SIG_SESSION_PRE_CREATE,                // Session创建
    TALK_SIG_SESSION_AFTER_CREATE,              // Session创建并初始化完成
    TALK_SIG_SESSION_PRE_DELETE,                // Session开始进入析构流程
    TALK_SIG_SESSION_AFTER_DELETE,              // Session析构完成
    TALK_SIG_SESSION_TRANSFER,                  // 执行transfer操作
    TALK_SIG_SESSION_DTMF_FINISH,               // 一串DTMF发送完了
    TALK_SIG_SESSION_SAVE_LOG,                  // 历史记录保存
    TALK_SIG_DIAL_CALLOUT,                      // 执行呼出流程
    TALK_SIG_BEFORE_CALLOUT,                    // 尝试使用焦点话路呼出 ProcessBeforeCallOut
    TALK_SIG_PROCESS_CALLOUT,                   // 呼出前的操作
    TALK_SIG_SAVE_NO_TALK_LOG,                  // 来电未接起时保存历史记录
    TALK_SIG_SESSION_RETRIEVEPARK,              // Retrievepark操作
    TALK_SIG_SESSION_ACTIVED,                   // 话路被激活（非焦点）
    TALK_SIG_IDLE_TO_TALK,                      // 从idle状态进入任意通话状态
    TALK_SIG_TALK_TO_IDLE,                      // 所有话路退出，恢复idle状态
    TALK_SIG_SESSION_REJECT,                    // 来电过程中手动拒接
    TALK_SIG_SESSION_PRE_START_TALK,            // 即将开始通话
    TALK_SIG_SESSION_AFTER_START_TALK,          // 已经开始通话
    TALK_SIG_SESSION_STOP_TALK,                 // 结束通话
    TALK_SIG_SESSION_MUTE_CHANGE,               // Mute
    TALK_SIG_SESSION_ANSWER,                    // 接听来电
    TALK_SIG_SESSION_HANDUP,                    // 挂断
    TALK_SIG_SESSION_ADD_TO_NET_CONF,           // 指定Session被加入网络会议
    TALK_SIG_SESSION_FWD,                       // 执行fwd流程
    TALK_SIG_SESSION_SIP_MESSAGE,               // sip消息
    TALK_SIG_CALL_SEND_REMOTE_DTMF,             // 发送DTMF
    TALK_SIG_SESSION_REFUSE_CALL,               // refuse流程
    TALK_SIG_SESSION_MAKE_CALL,                 // 发起invite
    TALK_SIG_SESSION_PLAY_BUSY_TONE,            // 播忙音
    TALK_SIG_SESSION_DIAL_TONE,                 // 播拨号音
    TALK_SIG_SESSION_TRY_ENTER_FINISH,          // 进finish
    TALK_SIG_SESSION_PLAY_RINGING,              // 播铃声
    TALK_SIG_SESSION_PLAY_CONF_TONE,            // 播放会议音
    TALK_SIG_SESSION_ATTENDED_TRANSFER_FAIL,    // 转移失败
    TALK_SIG_SESSION_TALK_STATE_CHANGE,         // 通话状态改变
    TALK_SIG_SESSION_RESET_TALK,                // 重置通话

    TALK_SIG_FILTER_BEGIN = 512,                // filter sig begin
    TALK_FLT_SESSION_INCOME,                    // 来电
    TALK_FLT_SESSION_REFUSE,                    // 来电处理过程中自动拒接
    TALK_FLT_SESSION_CREATE,                    // 创建
    TALK_FLT_SESSION_DELETE,                    // 销毁
    TALK_FLT_ENTER_DIAL,                        // 进拨号
};
