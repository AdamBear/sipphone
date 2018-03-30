#ifndef ACTIONUR_ACTIONUI_H_
#define ACTIONUR_ACTIONUI_H_

#include <ylhashmap.h>
#include <yllist.h>

#include "ETLLib.hpp"

#include "commondefine.h"

namespace NS_AU
{
// Class CActionUri, 解析网页发送过来的命令, 并执行相应操作.
// 网页命令分为三种: 功能键, 数字键, 以及其他操作(如拨号等).
// 目前网页发送的命令形式定为:
//     key=OK 当话机接收到该命令时, 将执行OK键的操作.
class CActionUri
{
    typedef YLList<yl::string> CmdList;

    typedef CmdList::iterator ListIter;
public:
    // Code type.
    enum CodeType
    {
        CODE_TYPE_NONE = 0,  // Invalid code type.
        FUNCTION_KEY = 1,  // Function key.
        DIGITAL_KEY = 2,  // Digital key.
        OPERATION = 3,  // Operation.
        CODE_TYPE_MAX
    };

    // 操作的方式.
    enum OperationType
    {
        OPERATION_TYPE_NONE = 0,  // 不正确方式.
        OPERATION_REBOOT = 1,  // Reboot.
        OPERATION_AUTOP = 2,  // autop.
        OPERATION_DND_ON = 3,  // Dnd on.
        OPERATION_DND_OFF = 4,  // Dnd off.
        OPERATION_NUMBER = 5,  // 拨号操作.
        OPEARTION_A_TRANS = 6,  // 咨询转给.
        OPERATION_B_TRANS = 7,  // 盲转.
        OPERATION_REDIAL = 8,
        OPERATION_LINEKEY_0 = 9,
        OPERATION_LINEKEY_1 = 10,  // Line key 0.
        OPERATION_LINEKEY_2 = 11,  // Line key 1.
        OPERATION_LINEKEY_3 = 12,  // Line key 2.
        OPERATION_LINEKEY_4 = 13,  // Line key 3.
        OPERATION_LINEKEY_5 = 14,  // Line key 0.
        OPERATION_LINEKEY_6 = 15,  // Line key 1.
        OPERATION_LINEKEY_7 = 16,  // Line key 2.
        OPERATION_LINEKEY_8 = 17,  // Line key 3.
        OPERATION_LINEKEY_9 = 18,  // Line key 4.
        OPERATION_LINEKEY_10 = 19,  // Line key 5.
        OPERATION_LINEKEY_11 = 20,  // Line key 6.
        OPERATION_LINEKEY_12 = 21,  // Line key 7.
        OPERATION_LINEKEY_13 = 22,  // Line key 8.
        OPERATION_LINEKEY_14 = 23,  // Line key 9.
        OPERATION_LINEKEY_15 = 24,  // Line key 10.
        OPERATION_LINEKEY_16 = 25,  // Line key 11.
        OPERATION_LINEKEY_17 = 26,  // Line key 12.
        OPERATION_LINEKEY_18 = 27,  // Line key 13.
        OPERATION_LINEKEY_19 = 28,  // Line key 14.
        OPERATION_LINEKEY_20 = 29,  // Line key 15.
        OPERATION_LINEKEY_21 = 30,  // Line key 16.
        OPERATION_LINEKEY_22 = 31,  // Line key 17.
        OPERATION_LINEKEY_23 = 32,  // Line key 17.
        OPERATION_LINEKEY_24 = 33,  // Line key 17.
        OPERATION_LINEKEY_25 = 34,  // Line key 17.
        OPERATION_LINEKEY_26 = 35,  // Line key 17.
        OPERATION_LINEKEY_27 = 36,  // Line key 17.
        OPERATION_LINEKEY_28 = 37,  // Line key 17.

        OPERATION_CALL_END,
        OPERATION_RESET,
        OPERATION_ENTER_MENU,
        OPERATION_ENTER_CONTACT,
        OPERATION_RECORD,

        OPERATION_SET_REMOTE_IP,

        OPERATION_UNHOLD_CALL,
        OPERATION_DEL_REMOTE_IP,

        OPERATION_ANSWER_CALL,

        OPERATION_AFWD_ON,
        OPERATION_AFWD_OFF,
        OPERATION_BFWD_ON,
        OPERATION_BFWD_OFF,
        OPERATION_NAFWD_ON,
        OPERATION_NAFWD_OFF,
        OPERATION_CALLWAIT_ON,  // callwaiting on.
        OPERATION_CALLWAIT_OFF,  // callwaiting off.

        OPERATION_VIEW_FORWARD,
        OPERATION_VIEW_MISSCALL,
        OPERATION_BACK_IDLE,

        OPEARTION_TYPE_MAX
    };

    // 命令的数据.
    struct CommandData
    {
        yl::string m_strCode;  // 命令代码.
        CodeType m_eType;  // 命令类型.
        int m_nVal;  // 命令对应的值, 为按键或者是消息id.

        CommandData()
        {
            Reset();
        }

        void Reset()
        {
            m_strCode.clear();
            m_eType = CODE_TYPE_NONE;
            m_nVal = 0;
        }
    };

    // 命令配置map.
    typedef YLHashMap<yl::string, CommandData *, STRING_HASH> CmdMap;

    // 获取单键.
    static CActionUri * GetInstance() DECLARE_ATTR;
    // 销毁单键.
    static void ReleaseInstance();

    // 消息处理函数.
    // | refObj | 是待处理的消息.
    // 如果处理成功则返回TRUE, 否则返回FALSE.
    static LRESULT OnMessage(msgObject & refObj);

    //网页请求截屏消息
    static LRESULT OnRequestCTR(msgObject & msg);

    // 定时器处理
    bool OnTimer(UINT uTimerId) DECLARE_ATTR;

    bool ProcessCmdList();

#if IF_BUG_27560
    // 获取是否隐藏通话信息
    bool GetIsHideInfo()
    {
        return m_bHideInfo;
    }

    // 设置是否隐藏通话信息
    void SetIsHideInfo(const bool & bHideInfo)
    {
        m_bHideInfo = bHideInfo;
    }
#endif

#if IF_BUG_23996
    // 判断是否为Actionuri key
    BOOL  IsActionUriKey();
    // 设置Actionuri key
    VOID SetActionUriKey(BOOL bIsActionUriKey);
#endif

private:
    // 构造函数.
    CActionUri();
    ~CActionUri();

    // 初始化.
    // 如果初始化成功则返回true, 否则返回false.
    bool MyInit();
    // 销毁.
    // 如果销毁成功则返回true, 否则返回false.
    bool MyFinal();

    // 指定命令.
    // | strCode | 是命令.
    // 如果执行成功则返回true, 否则返回false.
    bool DoCmd(const yl::string & strCode);

    // 通过命令查找对应的配置.
    // | strCode | 是命令.
    // 如果查找成功则返回配置, 否则返回NULL.
    CommandData * MyFindCmd(const yl::string & strCode);

    // 从消息结构体中解析出命令.
    // | refObj | 是消息结构体.
    // 如果解析成功则返回命令, 否则返回空.
    bool MyParseCode(msgObject & refObj);

    // 执行某个命令.
    // | stCD | 是命令的数据.
    // 如果执行成功则返回true, 否则返回false.
    bool MyDoCmd(const CommandData & stCD);

    // 执行某个操作.
    // | stCD | 是命令的数据.
    // 如果执行成功则返回true, 否则返回false.
    bool MyDoOperation(const CommandData & stCD);

    // 执行拨出号码.
    // | strCode | 是命令的数据.
    // 如果执行成功则返回true, 否则返回false.
    bool MyDoOperationNumber(const yl::string & strCode);

    // 重播.
    bool MyRedial();

    // 处理通话中盲转及咨询转等特殊的命令,如key=ATrans=10000,则该code为ATrans=10000,需要再进行处理
    bool ProcessTrans(const yl::string & strCode);

    //特殊处理设置Action URL
    bool ProcessRemoteReq(const yl::string & strCode);

    bool ProcessSetForward(yl::string & strCode);

    bool ProcessRegisterAcc(yl::string & strCode);

    bool ProcessLongPress(yl::string & strCode);

    //发送按键模拟消息
    bool SendKeyMsg(int nKeyCode);

    //格式化设置FWD的配置语句语句
    bool FormatSetFwdCfg(yl::string & strCode);
    int TransFwdCfgTypeAction(const yl::string & strFwdAction, yl::string & strFwdTypeEnable);

    bool IsURISupprot(CodeType eType, int iKeyCode);

#if IF_BUG_27560
    // 处理设置隐藏通话信息的特殊接口
    bool ProcessDisplay(const yl::string & strCode);
#endif

private:
    static CActionUri * m_pInst; // 单键实例.
    CmdMap m_mapCmd;  // 命令map表.

    bool m_bIsCmdTimeOn;

    CmdList m_ListCmd;

#if IF_BUG_27560
    bool m_bHideInfo;   //是否隐藏号码
#endif

#if IF_BUG_23996
    BOOL        m_bActionUriKey;  // ActionUri操控话机，键盘锁无效 （Poise特制）
#endif

    DISALLOW_COPY_AND_ASSIGN(CActionUri);
};

}  // namespace NS_AU

#define g_pActionUri (NS_AU::CActionUri::GetInstance())

#endif  // ACTIONUR_ACTIONUI_H_
