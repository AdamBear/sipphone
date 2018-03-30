#pragma once

#include <ylstring.h>
#include <ylvector.h>
#include <ylhashmap.h>
#include "dsskeyuilogiccommon.h"
#include "dsskeyuidataparse.h"
#include "dsskeyfunctionmanager.h"
#include "language/common/singletonbase.h"
#include "contacts/directory/include/directoryenumtypes.h"

// UI接口层
class CDsskeyUIData
    : public CDsskeyUIDataParse
    , public SingletonBase<CDsskeyUIData>
{
public:

    // 构造函数注册外部消息
    CDsskeyUIData();

    // URI索引接口
    DssKey_Type GetDsskeyTypeByURI(const yl::string & strURI);

    // 获取Label
    const yl::string & GetDefaultLabel(DssKey_Type eType);

    // UI保存数据接口
    bool SaveData(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData);

    // UI保存数据接口
    bool IsDataValid(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData,
                     CDsskeyPair & errorTips);

    // 获取UI数据
    bool GetData(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData);

    // UI获取可选数据列表（和具体的Dsskey无关，只和Dsskey类型有关）
    bool GetExtraData(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData,
                      const DssKeyLogicData & dsskeyLogicData);

    // 外部消息响应入口
    LRESULT OnMessage(msgObject & msg);

    // 是否只读
    DSSKEY_READONLY_MASK IsDsskeyReadOnly(const DsskeyPageInfo & pageInfo,
                                          const DssKeyUIData & dsskeyData);

    // 是否需要密码
    bool IsNeedPassword(DssKey_Type eType);

    // 是否支持该类型的Dsskey
    bool IsSupportType(const DsskeyPageInfo & pageInfo, DssKey_Type eType);

    // 获取单个属性
    CDsskeyItemAttr * GetItemAttr(DssKey_Type eType, DSSKEY_ATTR_ITEM eItem);

    // 查找Dsskey
    bool FindDsskeyInSet(const DsskeyPageInfo & pageInfo, DssKey_Type eType, bool bEventType);

    // 获取所有的Dsskey列表
    bool GetAllDsskey(const DsskeyPageInfo & pageInfo, VecCandidateValue & vecData, bool bEventType);

    // 获取列表
    bool GetAllDsskeyEx(DSSKEY_UI_TYPE eUIType, vecDsskeySet & vecOut);
    bool GetAllDsskeyEx(DSSKEY_UI_TYPE eUIType, vecDsskeySet * pVecIn, vecDsskeySet & vecOut);

    // 获取Label
    yl::string GetContactLabel(const DssKeyLogicData & dsskeyLogicData);

    // 纠正Dsskey类型
    bool CorrectDsskeyType(DssKeyLogicData & logicData, DSSKEY_MODULE_TYPE eModuleType, int iKeyIndex);

    // Dsskey被加载
    bool CheckVisible(DssKeyLogicData & logicData);

    // 特殊Dsskey对应的联系人分组
    DIR_TYPE GetDIRType(int iDsskeyType);

    // 保存文件
    bool SaveFile(LPCSTR lpFileName);

    // 内部函数集合，基本辅助函数
private:

    // 是否显示对应类型
    bool IsShowType(DssKey_Type eType);

    // 获取Dsskey列表
    vecDsskeySet * GetDsskeySet(DSSKEY_UI_TYPE eMoudle, bool bEventType = false);

    // 检查Linekey状态
    bool CheckLineLockStatus(const DsskeyPageInfo & pageInfo, const DssKeyLogicData & dsskeyLogicData,
                             const DssKeyLogicData & dsskeyOldData);

    // UI数据转化为逻辑层数据
    bool UIDataToLogicData(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData,
                           DssKeyLogicData & dsskeyLogicData);

    // UI数据转化为逻辑层数据
    bool LogicDataToUIData(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData,
                           DssKeyLogicData & dsskeyLogicData);

    // 三级权限写权限
    bool IsEditable(DssKey_Type eType);

    // 默认值
    bool SetDefaultValue(yl::string & strValue, DssKey_Type eType, DSSKEY_ATTR_ITEM eItem);

    // 内部函数集合，最底层的几个公用函数
private:

    // 注册函数
    void RegisterHandle();

    // 获取权限
    bool IsAuth(int iIndex, LPCSTR strAuth);

    // 重置Dsskey
    void ResetNotAvailDsskey();

    // 是否需要重置
    bool OnFunctionCheck(DssKey_Type eType);

    // 重设默认的Linekey
    void ResetDefaultDsskeyData(int iDsskeyID, DssKey_Type eType, int iFlag);

    // 获取重置数据
    bool GetDefaultDsskeyData(DssKey_Type eType, DSSKEY_MODULE_TYPE eModule, int iIndex,
                              DssKeyLogicData & logicData);

    // 静态函数集合，转换函数或者转调用外部模块函数
private:

    // 封装UI接口，获取单页Dsskey个数
    static int GetKeyPerPage();

    // 封装UI接口，获取联系人显示名
    static void GetGroupDisplayName(DIR_TYPE eDirType, int nGroupId, yl::string & strDisplayName);

public:

    // Dsskey逻辑层Moudle类型到UI moudle的转换函数
    static DSSKEY_UI_TYPE MapMoudle2UI(DSSKEY_MODULE_TYPE eMoudle);

    static DSSKEY_MODULE_TYPE MapMoudle2Logic(DSSKEY_UI_TYPE eMoudle);

private:
    int                     m_iLineKeyPerPage;   // 每一页最大的Dsskey数量
    dsskey_data_check       m_FunCheckFilter;   // 校验过滤函数
    int                     m_iConfMsgDepth;    // 屏蔽dssskey改变消息
};

#define _DsskeyUIData CDsskeyUIData::GetInstance()

