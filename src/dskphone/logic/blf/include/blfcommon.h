#ifndef __BLF_COMMON_H__
#define __BLF_COMMON_H__

#include "ylstring.h"
#include "yllist.h"
#include "ylvector.h"
#include "dsskey/include/dsskey_enumtypes.h"
#include "commonapi/stringhelperapi.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include "autoset/include/autoset_def.h"

enum eBLFMangaerType
{
    BLF_Base_Manager,
    BLF_Manager,
    BLFLIST_Manager
};

enum eBLFOperateType
{
    OPERATE_BLF_TYPE_NONE,      // 无操作
    OPERATE_BLF_TYPE_NEWCALL,   // 呼出监控方的操作
    OPERATE_BLF_TYPE_BTRANSFER, // 盲转给监控方的操作
    OPERATE_BLF_TYPE_CTRANSFER, // 咨询转给监控方的操作
    OPERATE_BLF_TYPE_PICKUP,    // 来电截答操作
    OPERATE_BLF_TYPE_BARGEIN,   // 加入通话操作
    OPERATE_BLF_TYPE_EDK,       // 实现EDK的操作
};

enum eBLFDisplayMethod
{
    BLF_DM_NAME = 0,
    BLF_DM_NUM = 1,
    BLF_DM_NAME_AND_NUMBER = 2,
    BLF_DM_NUMBER_AND_NAME = 3
};

class CBLFPraseAction;

//监控方列表
typedef YLList<yl::string> ListMonitor;

typedef ListMonitor::ListIterator ListMonitorIter;

int GetStatusLevels(int cStatus);

//对应于某个监控号码的其中一路会话
struct DialgInfo
{
    yl::string m_strDisplayName;
    yl::string m_strNumber;

    Dsskey_Status m_eStatus;

    yl::string m_strDialgId;

    bool m_bSipPickUp;

    int m_nMode;

    yl::string m_sSipPickUpInfo;

#if IF_FEATURE_SHOW_BLF_CALLID
    bool m_bCallIn;
#endif

    bool IsMatch(DialgInfo * pDialog);

    static bool Comp(DialgInfo * lpData, DialgInfo * rpData)
    {
        return GetStatusLevels(lpData->m_eStatus) <= GetStatusLevels(rpData->m_eStatus);
    }

    DialgInfo()
    {
        m_bSipPickUp = false;
        m_nMode = 0;
        m_eStatus = DS_BLF_STATUS_UNKOWN;
#if IF_FEATURE_SHOW_BLF_CALLID
        m_bCallIn = true;
#endif
    }
};

//对应于一个监控号码
struct BLFBaseData
{
    BLFBaseData();

    ~BLFBaseData();

    //获取最高优先显示状态
    Dsskey_Status GetShowStatus();

    int GetMode();

    void ClearDialogList();

    bool AddDialog(DialgInfo * pDialog);

    bool IsMatch(DialgInfo * pDialog);

    DialgInfo * GetDialogInfoByIndex(int index = 0);

    yl::string GetDisplayName();

    void UbindDsskeyByLine(int nLine);

    void UbindDsskeyByDsskeyId(int nId);

    void CheckBindDsskey(int iDsskeyId, int iAccountId, const yl::string & strNumber);

    //用于是否推送完整信息，用于更新话路状态
    bool m_bIsFullStatus;

    //话路信息列表
    YLList<DialgInfo *> m_ListDialgInfo;

    //关联的DsskeyId列表
    YLVector<int> m_nBindDsskeyId;

    yl::string m_strDisplayName;

    yl::string m_strNumber;

    yl::string m_strExten;

    yl::string m_strKey;

    int m_nVersion;
};

enum ePickUpOpt
{
    Opt_Add,
    Opt_Remove
};

struct RingMonitor
{
    yl::string m_strKey;

    eBLFMangaerType m_eManagerType;

    YLList<yl::string> m_lstMenu;
    YLVector<int> m_vecDsskeyID;
};

struct AccountMoitorData
{
    int m_nAccount;
    YLList<BLFBaseData *> m_listData;
};

// 创建数据解析类
CBLFPraseAction * CreateBLFPraseAction(char * strStr, bool IsPath);

// 通过DsskeyID找到此监控方的Key
yl::string GetBLFkeyByDsskeyId(int nId);

// 映射SIP的状态到自定义状态
Dsskey_Status MapSipBLFStatusToDsskeyStatus(int iStatus);

#endif

