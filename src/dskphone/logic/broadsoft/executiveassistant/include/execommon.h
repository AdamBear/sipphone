#ifndef _BW_EXECOMMON_H_
#define _BW_EXECOMMON_H_

#include <ETLLib.hpp>
#include <ylstring.h>
#include <ylvector.h>
#include <config.h>
#include "language/common/common_function.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
// 操作类型
enum ExeAssis_Action
{
    EAA_None = 0,
    EAA_Filter,
    EAA_Screen,
    EAA_Alter,
    EAA_ExeCommon,
    EAA_AssisCommon,

    EAA_SilentMonitoe,
    EAA_CallPush,
    EAA_CallInitiation,

    EAA_AutoSelect,
};

// Eexecutive Assistant 角色
enum EEA_TYPE
{
    EET_None = 0x00,
    EET_Executive = 0x01 << 1,
    EET_Assistant = 0x01 << 2,
    EET_ALL = EET_Executive | EET_Assistant,
};

#define kszExe_FilterMode_Simple                "Simple"
#define kszExe_FilterMode_Advance               "Advanced"
#define kszExe_FilterType_AllCalls              "All Calls"
#define kszExe_FilterType_AllInternalCalls      "All Internal Calls"
#define kszExe_FilterType_AllExternalCalls      "All External Calls"
#define kszExe_Screen_AlertType_Silent          "Silent"
#define kszExe_Screen_AlertType_Splash          "Ring Splash"
#define kszExe_Alter_Mode_Simulataneous         "Simultaneous"
#define kszExe_Alter_Mode_Sequential            "Sequential"

#define kszExe_Alter_CallInfo_Executive                 "Executive"
#define kszExe_Alter_CallInfo_Originator                "Originator"
#define kszExe_Alter_CallInfo_Executive_Originator      "Executive-Originator"
#define kszExe_Alter_CallInfo_Originator_Executive      "Originator-Executive"
#define kszExe_Alter_CallInfo_Custom                    "Custom"

#define kszExe_Alter_RolloverAction_VoiceMail           "Voice Messaging"
#define kszExe_Alter_RolloverAction_Forward             "Forward"
#define kszExe_Alter_RolloverAction_None                "No Answer Processing"


// 过滤模式
enum EXE_Filter_Mode
{
    EFM_None = -1,
    EFM_Simple = 0,
    EFM_Advance,
};

// 过滤规则
enum EXE_Filter_Type
{
    EFT_NONE = -1,
    EFT_All_Calls = 0,
    EFT_All_Internal_Calls,
    EFT_All_External_Calls,
};

// 铃声种类
enum EXE_Screen_Alter_Type
{
    EAT_NONE = -1,
    EAT_Silent,                // 静音
    EAT_Ring_Splash,           // 响铃
};

// 响铃模式
enum EXE_Alter_Mode
{
    EAM_NONE = -1,
    EAM_Simulataneous, // 同响
    EAM_Sequential,    // 顺序
};

// Call info
enum EXE_CallInfo_Source
{
    ECIS_NONE = -1,
    ECIS_Executive,
    ECIS_Originator,
    ECIS_Custom,

    ECIS_Executive_Originator,
    ECIS_Originator_Executive,
};


// 超时处理
enum EXE_Rollover_Action
{
    ERA_NONE = -1,
    ERA_Voice_Messaging,
    ERA_FWD_SIPURI,
    ERA_NoAnswer_Processing,
};

// 枚举值到字符串的映射关系
static const mapItem ExeFilterModeMap[] =
{
    {EFM_Simple,                kszExe_FilterMode_Simple},
    {EFM_Advance,               kszExe_FilterMode_Advance},
};

static const mapItem ExeFilterTypeMap[] =
{
    {EFT_All_Calls,             kszExe_FilterType_AllCalls},
    {EFT_All_Internal_Calls,    kszExe_FilterType_AllInternalCalls},
    {EFT_All_External_Calls,    kszExe_FilterType_AllExternalCalls},
};

static const mapItem ExeScreenAlterTypeMap[] =
{
    {EAT_Silent,                kszExe_Screen_AlertType_Silent},
    {EAT_Ring_Splash,           kszExe_Screen_AlertType_Splash},
};

static const mapItem ExeAlterModeMap[] =
{
    {EAM_Simulataneous,         kszExe_Alter_Mode_Simulataneous},
    {EAM_Sequential,            kszExe_Alter_Mode_Sequential},
};

static const mapItem ExeRolloverActionMap[] =
{
    {ERA_Voice_Messaging,       kszExe_Alter_RolloverAction_VoiceMail},
    {ERA_FWD_SIPURI,            kszExe_Alter_RolloverAction_Forward},
    {ERA_NoAnswer_Processing,   kszExe_Alter_RolloverAction_None},
};

static const mapItem ExeCallInfoSrcMap[] =
{
    {ECIS_Executive,            kszExe_Alter_CallInfo_Executive},
    {ECIS_Originator,           kszExe_Alter_CallInfo_Originator},
    {ECIS_Executive_Originator, kszExe_Alter_CallInfo_Executive_Originator},
    {ECIS_Originator_Executive, kszExe_Alter_CallInfo_Originator_Executive},
    {ECIS_Custom,               kszExe_Alter_CallInfo_Custom},
};

struct CExeAssisInspector
{
    CExeAssisInspector(ExeAssis_Action eData) : eActionType(eData) {}

    // 获取类型
    ExeAssis_Action GetAction()
    {
        return eActionType;
    }
    ExeAssis_Action GetAction() const
    {
        return eActionType;
    }
private:
    ExeAssis_Action  eActionType;
};

struct CExeFilterInfo : public CExeAssisInspector
{
    struct CCriteria
    {
        yl::string strName;
        yl::string strUri;
        bool       bActive;

        CCriteria() : bActive(false) {}
    };

    typedef YLVector<CCriteria>    CriteriaList;
    typedef CriteriaList::iterator CriteriaListIt;

    bool                bSwitchOn;
    CriteriaList        tCriteriaList;
    EXE_Filter_Mode     eFilterMode;
    EXE_Filter_Type     eFilterType;

    CExeFilterInfo() : CExeAssisInspector(EAA_Filter)
    {
        bSwitchOn = false;
        eFilterMode = EFM_None;
        eFilterType = EFT_NONE;
    }

    // for armv6jel
    bool operator== (const CExeFilterInfo & other)
    {
        return bSwitchOn == other.bSwitchOn
               && eFilterMode == other.eFilterMode
               && eFilterType == other.eFilterType;
    }

    bool operator!= (const CExeFilterInfo & other)
    {
        return !(operator==(other));
    }

    EXE_Filter_Mode SetFilterMode(const yl::string & str)
    {
        return eFilterMode = (EXE_Filter_Mode)Str2KeyEx(str, ExeFilterModeMap);
    }

    const char * GetFilterMode()
    {
        return Key2StrEx(eFilterMode, ExeFilterModeMap);
    }

    static const char * GetFilterMode(EXE_Filter_Mode eMode)
    {
        return Key2StrEx(eMode, ExeFilterModeMap);
    }

    EXE_Filter_Type SetFilterType(const yl::string & str)
    {
        return eFilterType = (EXE_Filter_Type)Str2KeyEx(str, ExeFilterTypeMap);
    }

    const char * GetFilterType()
    {
        return Key2StrEx(eFilterType, ExeFilterTypeMap);
    }

    static const char * GetFilterType(EXE_Filter_Type eType)
    {
        return Key2StrEx(eType, ExeFilterTypeMap);
    }
};

struct CExeScreenInfo : public CExeAssisInspector
{
    bool                        bScreeningOn;
    EXE_Screen_Alter_Type       eAlterType;
    bool                        bAlertMobility;
    bool                        bAlertAnywhere;
    bool                        bAlertSCA;

    CExeScreenInfo() : CExeAssisInspector(EAA_Screen)
    {
        bScreeningOn = false;
        eAlterType = EAT_NONE;
        bAlertMobility = false;
        bAlertAnywhere = false;
        bAlertSCA = false;
    }

    // for armv6jel
    bool operator== (const CExeScreenInfo & other)
    {
        return memcmp(this, &other, sizeof(CExeScreenInfo)) == 0;
    }

    bool operator!= (const CExeScreenInfo & other)
    {
        return !(operator==(other));
    }

    EXE_Screen_Alter_Type SetAlterType(const yl::string & str)
    {
        return eAlterType = (EXE_Screen_Alter_Type)Str2KeyEx(str, ExeScreenAlterTypeMap);
    }

    const char * GetAlterType()
    {
        return Key2StrEx(eAlterType, ExeScreenAlterTypeMap);
    }

    static const char * GetAlterType(EXE_Screen_Alter_Type eType)
    {
        return Key2StrEx(eType, ExeScreenAlterTypeMap);
    }
};

struct CExeAlterInfo : public CExeAssisInspector
{
    EXE_Alter_Mode      eAlterMode;
    int                 iAdvanceToNextAssistantAfter;

    EXE_CallInfo_Source eCallInfoNameSource;
    EXE_CallInfo_Source eCallInfoNumberSource;
    yl::string          strCustomCallName;
    yl::string          strUnicodeCustomCallName;
    yl::string          strCustomCallNumber;

    int                 iCallPushRecallAfter; // [2, 20]
    int                 iRolloverAfter;       // s
    EXE_Rollover_Action eRolloverAction;
    yl::string          strRolloverTo;

    CExeAlterInfo() : CExeAssisInspector(EAA_Alter)
    {
        eAlterMode = EAM_NONE;
        iAdvanceToNextAssistantAfter = 2;
        iCallPushRecallAfter = 2;
        eCallInfoNameSource = ECIS_NONE;
        eCallInfoNumberSource = ECIS_NONE;
        iRolloverAfter = 30;
        eRolloverAction = ERA_NONE;
    }

    // for armv6jel
    bool operator== (const CExeAlterInfo & other)
    {
        return eAlterMode == other.eAlterMode
               && iAdvanceToNextAssistantAfter == other.iAdvanceToNextAssistantAfter
               && eCallInfoNameSource == other.eCallInfoNameSource
               && eCallInfoNumberSource == other.eCallInfoNumberSource
               && strCustomCallName == other.strCustomCallName
               && strUnicodeCustomCallName == other.strUnicodeCustomCallName
               && strCustomCallNumber == other.strCustomCallNumber
               && iCallPushRecallAfter == other.iCallPushRecallAfter
               && iRolloverAfter == other.iRolloverAfter
               && strRolloverTo == other.strRolloverTo
               && eRolloverAction == other.eRolloverAction;
    }

    bool operator!= (const CExeAlterInfo & other)
    {
        return !(operator==(other));
    }

    EXE_Alter_Mode SetAlterMode(const yl::string & str)
    {
        return eAlterMode = (EXE_Alter_Mode)Str2KeyEx(str, ExeAlterModeMap);
    }

    const char * GetAlterMode()
    {
        return Key2StrEx(eAlterMode, ExeAlterModeMap);
    }

    static const char * GetAlterMode(EXE_Alter_Mode eMode)
    {
        return Key2StrEx(eMode, ExeAlterModeMap);
    }

    EXE_Rollover_Action SetRolloverAction(const yl::string & str)
    {
        return eRolloverAction = (EXE_Rollover_Action)Str2KeyEx(str, ExeRolloverActionMap);
    }

    const char * GetRolloverAction()
    {
        return Key2StrEx(eRolloverAction, ExeRolloverActionMap);
    }

    static const char * GetRolloverAction(EXE_Rollover_Action eAction)
    {
        return Key2StrEx(eAction, ExeRolloverActionMap);
    }

    EXE_CallInfo_Source SetCallInfoName(const yl::string & str)
    {
        return eCallInfoNameSource = (EXE_CallInfo_Source)Str2KeyEx(str, ExeCallInfoSrcMap);
    }

    EXE_CallInfo_Source SetCallInfoNumber(const yl::string & str)
    {
        return eCallInfoNumberSource = (EXE_CallInfo_Source)Str2KeyEx(str, ExeCallInfoSrcMap);
    }

    const char * GetCallInfoName()
    {
        return Key2StrEx(eCallInfoNameSource, ExeCallInfoSrcMap);
    }

    const char * GetCallInfoNumber()
    {
        return Key2StrEx(eCallInfoNumberSource, ExeCallInfoSrcMap);
    }

    static const char * GetCallInfo(EXE_CallInfo_Source eInfo)
    {
        return Key2StrEx(eInfo, ExeCallInfoSrcMap);
    }
};

// 描述
struct CUserName
{
    yl::string strUserId;
    yl::string strUserName;
    yl::string strDN;
    yl::string strExt;

    yl::string strFirstName;
    yl::string strLastName;
    yl::string strUri;
};

// 用户属性
struct CUserItem
{
    int        iPriority;
    bool       bOptIn;
    bool       bFilterOn;

    CUserName  ItemName;

    operator bool() const
    {
        return !ItemName.strUserId.empty();
    }

    bool operator==(const CUserItem & other) const
    {
        return ItemName.strUserId == other.ItemName.strUserId
               && ItemName.strUri == other.ItemName.strUri;
    }

    bool operator< (const CUserItem & other)
    {
        return iPriority < other.iPriority;
    }

    CUserItem() : iPriority(0), bOptIn(true) {}
};

typedef YLVector<CUserItem> ExeAssisList;
typedef ExeAssisList::iterator ExeAssisListIt;

// Executive、Assistant的公共数据
struct CExeAssisBase
{
    CExeFilterInfo  tFilter;
    CExeScreenInfo  tScreen;
    CExeAlterInfo   tAlter;
};

struct CExeCommon : public CExeAssisInspector
{
    bool              bAllowOpt;
    ExeAssisList      listData;

    CExeCommon() : CExeAssisInspector(EAA_ExeCommon), bAllowOpt(false) {}
};

struct CAssCommon : public CExeAssisInspector
{
    bool              bDivertOn;
    yl::string        strDivertNumber;
    ExeAssisList      listData;

    CAssCommon() : CExeAssisInspector(EAA_AssisCommon), bDivertOn(false) {}
};

struct CExecutive_Data : public CExeAssisBase
{
    CExeCommon        tCommonData;
};

struct CAssistant_Data : public CExeAssisBase
{
    CAssCommon      tCommonData;
};

// 所有的 exe assis 数据存储位置
struct ExeAssisData
{
    ExeAssisData() : eType(EET_None) {}

    bool IsExecutive() const
    {
        return (eType & EET_Executive);
    }
    bool IsAssistant() const
    {
        return (eType & EET_Assistant);
    }
    bool IsValid() const
    {
        return (eType & EET_ALL);
    }
    bool IsInvalid() const
    {
        return !IsValid();
    }

    bool SetType(int iType)
    {
        if (iType & EET_Executive)
        {
            eType |= EET_Executive;
        }
        if (iType & EET_Assistant)
        {
            eType |= EET_Assistant;
        }

        return IsValid();
    }

    void Clean(int iType = EET_ALL)
    {
        if (iType & EET_Executive)
        {
            eType &= ~EET_Executive;
        }
        if (iType & EET_Assistant)
        {
            eType &= ~EET_Assistant;
        }
    }

    // 同一个账号可能即是Executive又是Assistant
    CExecutive_Data      tExeData;
    CAssistant_Data      tAssData;
    int                  eType;
};

#endif // IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
#endif  //_BW_EXECOMMON_H_
