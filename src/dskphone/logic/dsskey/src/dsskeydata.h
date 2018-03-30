#ifndef _DSSKEY_DSSKEYDATA_HEADER_
#define _DSSKEY_DSSKEYDATA_HEADER_

#include "../include/dsskeystructure.h"
#include "../include/dsskey_enumtypes.h"
#include "../include/dsskey_def.h"
#include "dsskey_inner_enumtypes.h"

#include <ETLLib.hpp>
#include <xmlparser/xmlparser.hpp>
#include <string.h>
#include <ylstl/ylstring.h>
#include "globalmodel.h"
//fix me
using namespace std;
class CDSSKeyAction;

#if 0
// 直接比较字符指针在debug模式下存在问题，跨平台存在问题
typedef YLHashMap<LPCSTR, LPCSTR> CFG_ITEM_MAP;
#else
typedef YLHashMap<yl::string, LPCSTR> CFG_ITEM_MAP;
#endif

class CDSSkeyData
{
public:
    CDSSkeyData(void);
    ~CDSSkeyData(void);

    bool ParserFromMemory(DSSKEY_MODULE_TYPE eModuleType, int iKeyIndex, int iModuleIndex = -1);

    bool ToMemory(DSSKEY_MODULE_TYPE eModuleType, int iKeyIndex, int iModuleIndex = -1, int iFlag = DSSKEY_SET_RELOAD);

    bool SetDsskeyAction(DssKey_Type dsskeyType);

    bool CorrectDsskeyType(DSSKEY_MODULE_TYPE eModuleType, int iKeyIndex);

    int  GetDsskeyDataID();

    void SetDSSKeyDataID(int dsskeyDataID);

    void GetLogicData(DssKeyLogicData& dataOutput);

    DssKeyLogicData& GetLogicData();

    void SetDsskeyType(DssKey_Type eType);

#if IF_BUG_XMLBROWSER_EXP_LED
    void SetLightType(LIGHT_Type l_Type);
    void ClearLightType();
#endif

    DssKey_Type GetDsskeyType();

    bool OnKeyPress();

    bool OnKeyLongPress();

    bool UpdateStatus();

    bool SetStatus(Dsskey_Status eStatus);

    bool UpdateRemoteStatus(char chRemoteStatus);

    int GetLineIndex();

    void SetLabel(const yl::string& strLabel);

    void SetShortLabel(const yl::string& strShortLabel);

    void SetValue(const yl::string& strValue);

    void SetExtension(const yl::string& strExtension);

    void GetValue(yl::string& strOutput);

    void GetLabel(yl::string& strOutput);

    void GetExtension(yl::string& strOutput);

    Dsskey_Status GetStatus();

    void GetStatusList(YLList<Dsskey_Status>& listOutput);

    // 该按键是否根据对方状态来控制灯
    bool IsStatusCtrlByRemote();

    DssKeyLockType GetLockType();

    bool IsStatusExist(Dsskey_Status status);

    // 定时器处理
    bool OnTimer(UINT uTimerID) DECLARE_ATTR;


    bool OnConfigChange(void* pEtraData);

    bool OnTypeModify(DssKey_Type oldType, DssKey_Type newType, void* pExtraData);

    // 设置定时器
    void SetTimer(UINT uTimeInterval);

    void killTimer();


    void SetLineIndex(int lineIndex);

    void SetRelatedLock(bool bValue);

    bool GetRelatedLock()
    {
        return m_bRelatedLock;
    }

    void SetLineStatus(Dsskey_Status eDsskeyStatus, Dsskey_Status eFirst, Dsskey_Status eEnd);

    void UpdateLineStatus(Dsskey_Status eDsskeyStatus);

    void RemoveLineStatus(Dsskey_Status eFirst, Dsskey_Status eEnd);

    bool SetLockType(DssKeyLockType lockType);

    // 设置数据
    void SetData(const DssKeyLogicData& dsskeyLogicData);

    int GetRealLine();

    bool IsLocked();

    DSS_KEY_PAGE_BUTTON_PRIORITY GetPageButtonPriority();

    //检查lineid的有效性，并处理auto的情况
    void CorrectRealBindAccount();

    //清除状态
    void ClearStatus();

    int GetPickupStatus();


    UINT getTimerID();

    void setTimerID(UINT timerID);

    int setRemoteStatus(char remoteStatus);

    char getRemoteStatus()
    {
        return m_chRemoteStatus;
    }

    DSSKEY_MODULE_TYPE getDsskeyModuleType();

    void setDsskeyModuleType(DSSKEY_MODULE_TYPE type);

    yl::string getExtFiledValue(const yl::string& strFiledName);

    void setExtFieldValue(const yl::string& strFiledName, const yl::string& strFiledValue);

    static CFG_ITEM_MAP* GetDsskeyConfigIDMapData(DSSKEY_MODULE_TYPE eModuleType);

    static void SetIntConfig(DSSKEY_MODULE_TYPE eModuleType, const char* szAttr, int iKeyIndex,
                             int iModuleIndex, int iDefValue, CONFIG_LEVEL_TYPE eLevel = CONFIG_LEVEL_PHONE);

    static void SetStringConfig(DSSKEY_MODULE_TYPE eModuleType, const char* szAttr, int iKeyIndex,
                                int iModuleIndex, const yl::string& strValue, CONFIG_LEVEL_TYPE eLevel = CONFIG_LEVEL_PHONE);

    static int GetIntConfig(DSSKEY_MODULE_TYPE eModuleType, const char* szAttr, int iKeyIndex,
                            int iModuleIndex, int iDefValue, CONFIG_LEVEL_TYPE eLevel = CONFIG_LEVEL_IGNORE);

    static yl::string GetStringConfig(DSSKEY_MODULE_TYPE eModuleType, const char* szAttr,
                                      int iKeyIndex,
                                      int iModuleIndex, const yl::string& strDefValue, CONFIG_LEVEL_TYPE eLevel = CONFIG_LEVEL_IGNORE);
    static LPCSTR GetConfigID(DSSKEY_MODULE_TYPE eModuleType, const char* szAttr);

    static yl::string GetStringConfig(int iDsskeyID, const char* szAttr, CONFIG_LEVEL_TYPE eLevel);

    static int GetIntConfig(int iDsskeyID, const char* szAttr, CONFIG_LEVEL_TYPE eLevel);

protected:
    // 将blf状态转换成Dsskey_Status
    Dsskey_Status MapStatusToDsskeyStatus(int iStatus);

    DSS_KEY_PAGE_BUTTON_PRIORITY GetLinePriority();

    static void initDsskeyMapData();

private:

    int m_iRealLineID;//真实绑定的账号,设置为Auto的时候需要

    DssKeyLogicData m_logicData;

    Dsskey_Status m_lineStatus;

    int m_iDsskeyID;

    char m_chRemoteStatus;

    YLList<Dsskey_Status> m_listStatus;

    bool m_bRelatedLock;

    UINT timerID;

    YLHashMap<yl::string, yl::string>* m_pExtFileds;

    static CFG_ITEM_MAP s_mapLinekey;
    static CFG_ITEM_MAP s_mapProgramkey;
    static CFG_ITEM_MAP s_mapExpkey;
};

#endif
