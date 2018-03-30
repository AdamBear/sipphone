#pragma once

#include "talk/talklogic/include/sessionproxy.h"

class CSessionStateProxy
    : public CSessionProxy
{
public:

    CSessionStateProxy(CSingleSession* p = NULL);

    CSessionStateProxy(const CSessionProxy& proxy)
        : CSessionProxy(proxy)
    {}

    CSessionStateProxy(int iSessionID)
        : CSessionProxy(iSessionID)
    {}

public:
    // ui类型
    DIAL_UI_TYPE GetDialUIType() const;
    // 是否焦点
    bool IsFocusSession() const;
    // 是否处于Host
    bool IsTopestHostFocused() const;
    // 是否允许swap
    bool IsAllowSwap() const;
    // 是否允许conf
    bool IsAllowConf() const;
    // 是否允许创建
    bool IsAllowCreateConf() const;
    // 是否允许添加到conf
    bool IsAllowAddToConf() const;
    // 是否允许建立本地会议
    bool IsAllowCreateLocalConf() const;
    // 是否允许添加到网络会议
    bool IsAllowAddToNetConf(int iAccountID) const;
    // 是否允许Conf
    bool IsAllowConfWithAnother() const;
    // 是否允许执行transfer
    bool IsAllowTran() const;
    // 是否允许被Transfer
    bool BeAllowTransferTo() const;
    // 两个Session是否允许互tran
    bool IsAllowTranTo(int iSessioID) const;
    // 是否直接执行transfer流程
    bool IsTransferDirectly() const;
    // 是否允许FWD
    bool IsAllowFwd() const;
    // 是否允许创建新话路
    bool IsAllowNewCall() const;
    bool IsAllowNewCall(int iAccountID) const;
    // 是否允许进拨号
    bool IsAllowNewDail() const;
    // 是否允许用Dsskey呼出（Speeddial等）
    bool IsAllowCreateNewCall() const;
    // 是否允许hold
    bool IsAllowHold() const;

    bool IsAllowJoinLocalConf() const;

    // swap 操作
    bool SwapToAnother() const;
    // 恢复通话
    bool ShowSessionAddToConf() const;

    // 添加可加入conf的目标
    void AddConfToTarget(YLList<CCallInfo*>& list) const;

    // 添加需要穿梭的目标
    void AddTransferToTarget(YLList<CCallInfo*>& list) const;

    // 添加可以加入会议的SesssionID列表
    bool GetSessionToConf(YLVector<int>& vec) const;
    // 是否存在可以加入会议的Sesssion
    bool IsExistSessionToConf() const;

    // 获取允许作为TransferTo Target的话路
    int GetNumberOfAllowTranTo() const;

    // call decline 功能是否开启
    bool IsCallDeclineEnable() const;
    // 紧急呼叫开关
    bool IsEmergencyCalled() const;

    //可建立本地会议数是否超过会议最大支持路数
    bool IsAllowAddLocalConfNumOverMax() const;
    //对应账号允许加入网络会议的话路总数
    int GetTotalNumOfRoutineAllowAddNetConf(int nAccountID) const;
    //获取当前允许加入本地会议的话路数
    int GetAllowAddLocalConfRoutineNum() const;

    // 咨询转
    bool AttendedTransfer(const CallOut_Param& callParam, bool bTranByBLF = false) const;

    // 显示提示信息
    void ShowInfo(int iType);

    bool IsPreForword() const;

    void ExitPreFWD();

    bool SetFocus() const;

    void SetRelationInfo(CALL_RELATION eRelation /*= CR_NONE*/, const yl::string& strRelation /*= ""*/);

    bool IsKTSIgnore();

    const yl::string& GetDialText() const;
};


