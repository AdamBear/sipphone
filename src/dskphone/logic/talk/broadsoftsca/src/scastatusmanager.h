#ifndef __BSSCA_MANAGER_H__
#define __BSSCA_MANAGER_H__

#ifdef IF_FEATURE_BROADSOFT_SCA

#include "../include/scamanager_def.h"
#include "dsskey/include/dsskey_def.h"
#include "blfparse.h"
#include "talk/talklogic/include/modtalklogic.h"

class CSCAStatusManager
{
    friend class CBLFParse;

public:
    CSCAStatusManager();
    ~CSCAStatusManager();
    // 初始化类
    VOID InitBSSCAManager(int iID);
    // 解析sip转发的Notify消息,以便确认账号的分机状态信息
    BOOL ParseSCANotify(int iIndex, void * pStateArray, int iSize);
    // 获取第一个空闲线路的Index值
    int GetFirstFreeIndex();
    // 获取指定账号响铃线路
    int GetAlertingIndex();
    // 判断是否要显示Share Line通话选择界面
    BOOL BeShowSharedCalls();
    // 获取存储状态信息的链表指针
    ListSCAInfo * GetList()
    {
        return &m_listSCAInfo;
    }

    // 根据序号获取线路信息
    SCA_Info * GetScaInfoByIndex(int iIndex);
    // 根据状态获取链表中第一个处于该状态的分线信息
    SCA_Info * GetFirstInfoByStatus(BLF_STATUS_CODE eStatus, int iDsskeyIndex);
    // 根据状态获取处于该状态的本路通话
    CSessionProxy GetSessionBySCAStatus(BLF_STATUS_CODE eStatus);
    // 获取某个index的状态
    BLF_STATUS_CODE GetStatusByIndex(int iIndex);
    // 将某一index设为Idle状态
    VOID SetFreeIndex(int iIndex);
    // 更新share line账号指示灯状态信息
    VOID UpDateLightInfo();
    // 获取占线索引
    int GetBLAIndex(int & iBLAIndex, int iKeyCounts);
    // 获取dsskeyid对应的线路状态
    BLF_STATUS_CODE GetStatusByDsskeyID(int iDsskeyID);
    // BLA类型信息
    BLA_TYPE GetBlaType()
    {
        return m_eBlaType;
    }
    // 通过状态索引index
    int GetBlaIndexByStatus(BLF_STATUS_CODE eStatus, int iDsskeyIndex);

    int GetCurrentCallNum(int iAccount = 0, int nDsskeyIndex = 0);
    // 获取该索引对应状态的显示名
    BLF_STATUS_CODE GetStatus2UIShow(int iAccountId, int iIndex);
    // 释放资源
    void Release();
    // 添加SCA状态信息
    void AddSCAInfo(SCA_Info & sInfo);
    // 对应账号配置变更
    void ReloadConfig();

    void SetBlaNumer(const yl::string& str)
    {
        m_strBlaNumber = str;
    }

    yl::string& GetBlaNumer()
    {
        return m_strBlaNumber;
    }

private:
    // 清空存储账号分线信息的单链表
    VOID ClearList();
    // 初始化分配账号分线状态数组内存
    VOID AllocArray();
    // 从新分配账号分线状态数组内存
    VOID ReAllocArray();
    // 设置账号分线状态到数组
    VOID SetStatus2Array(int iIndex, BLF_STATUS_CODE eStatus);
    // 清空状态数组
    VOID ClearStatusArray();

    // 获取状态的优先级,最高优先级为1,,其次为2,然后依次类推
    int GetStatusPriority(BLF_STATUS_CODE eStatus);
    // 获取链表中获取该Dsskey亮灯优先级最高的账号分线信息
    BLF_STATUS_CODE GetHighSCAStatus(int iDsskeyId, int iLineNum);

    int GetDsskeyIndex(YLList<int> & ListDsskey, int iDsskeyIndex);

    void PrintAppearance();

    BLF_STATUS_CODE GetOldSCAStatus(int iDsskeyID);

    // 将所有分线状态都设置成指定状态
    static VOID SetAllSameStatus(BLF_STATUS_CODE * pStatus, BLF_STATUS_CODE eStatus, int isize);
    // 判断该状态是否要加入链表
    static BOOL BeAddStatus2List(BLF_STATUS_CODE eStatus);

private:
    BLA_TYPE                m_eBlaType;                 // 区分类型
    int                     m_iLineID;                  // 该管理类对应的账号
    int                     m_iAppearanceNum;           // 该账号允许的话路数目
    BLF_STATUS_CODE    *    m_peAppearanceStatus;       // 账号各个分线所处的状态头指针
    CBLFParse               m_blfParse;                 // BLF状态解析
    ListSCAInfo             m_listSCAInfo;              // 账号分线状态信息链表
    yl::string              m_strBlaNumber;
};
#endif // #ifdef IF_FEATURE_BROADSOFT_SCA
#endif // __BSSCA_MANAGER_H__
