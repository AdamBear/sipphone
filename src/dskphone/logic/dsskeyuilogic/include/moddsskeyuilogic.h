#pragma once

#include "dsskey/include/moddsskey.h"
#include "dsskeyuilogic/include/dsskeyuilogiccommon.h"

/************************************************************************/
// 函数描述  ： dsskeyuilogic模块初始化
// 参数说明 ： 无
// 返回值 ： 无
/************************************************************************/
void dsskeyuilogic_Init();

/************************************************************************/
// 函数描述  ： 获取dsskey显示属性
// 参数说明 ： [eType]: dsskey类型   [eAttr]: 属性类型
// 返回值 ： CDsskeyItemAttr*
/************************************************************************/
CDsskeyItemAttr * dsskeyuilogic_GetAttrByType(DssKey_Type eType, DSSKEY_ATTR_ITEM eAttr);

/************************************************************************/
// 函数描述  ： 获取dsskey显示名称
// 参数说明 ： [pageInfo]: 页面属性   [vecData]: key Pairs     [bEventType] 是否是KeyEvent
// 返回值 ： 是否获取成功
/************************************************************************/
bool dsskeyuilogic_GetAllDsskey(const DsskeyPageInfo & pageInfo, VecCandidateValue & vecData,
                                bool bEventType);

/************************************************************************/
// 函数描述  ： UI保存dsskey
// 参数说明 ： [pageInfo]: 页面属性   [dsskeyData]: 页面数据
// 返回值 ： 是否保存成功
/************************************************************************/
bool dsskeyuilogic_SaveData(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData);

/************************************************************************/
// 函数描述  ： 校验dsskey数据合法性
// 参数说明 ： [pageInfo]: 页面属性   [dsskeyData]: 页面数据     [errorTips]: 错误消息
// 返回值 ： 是否合法
/************************************************************************/
bool dsskeyuilogic_IsDataValid(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData,
                               CDsskeyPair & errorTips);

/************************************************************************/
// 函数描述  ： 获取dsskey逻辑层数据（UI数据结构）
// 参数说明 ： [pageInfo]: 页面属性   [dsskeyData]: 页面数据
// 返回值 ： 是否获取成功
/************************************************************************/
bool dsskeyuilogic_GetData(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData);

/************************************************************************/
// 函数描述  ： 更新dsskey逻辑层数据（UI数据结构）
// 参数说明 ： [pageInfo]: 页面属性   [dsskeyData]: 页面数据
// 返回值 ： 是否更新成功
/************************************************************************/
bool dsskeyuilogic_UpdateData(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData);

/************************************************************************/
// 函数描述  ： 检测是否是KeyEvent类型（不是的话会重置为NA）
// 参数说明 ： [pageInfo]: 页面属性      [eType]: Dsskey类型
// 返回值 ： 是、否
/************************************************************************/
bool dsskeyuilogic_CheckKeyEventSet(const DsskeyPageInfo & pageInfo, DssKey_Type eType);

/************************************************************************/
// 函数描述  ： dsskey页面是否只读
// 参数说明 ： [pageInfo]: 页面属性    [dsskeyData]: 页面数据
// 返回值 ： 是否只读、部分只读、全部只读
/************************************************************************/
DSSKEY_READONLY_MASK dsskeyuilogic_IsDsskeyReadOnly(const DsskeyPageInfo & pageInfo,
        const DssKeyUIData & dsskeyData);

/************************************************************************/
// 函数描述  ： 获取联系人相关Dsskey的Label显示名
// 参数说明 ： [dsskeyLogicData]: 逻辑层数据
// 返回值 ： Label显示名
/************************************************************************/
yl::string dsskeyuilogic_GetContactLabel(const DssKeyLogicData & dsskeyLogicData);

/************************************************************************/
// 函数描述  ： 纠正错误的Dsskey
// 参数说明 ： [logicData]: Dsskey数据   [eModuleType]: 模块      [iKeyIndex]: key序号
// 返回值 ： 是否有纠正
/************************************************************************/
bool dsskeyuilogic_CorrectDsskeyType(DssKeyLogicData & logicData, DSSKEY_MODULE_TYPE eModuleType,
                                     int iKeyIndex);


/************************************************************************/
// 函数描述  ： dsskey加载时候的处理函数
// 参数说明 ： [logicData]: Dsskey数据
// 返回值 ： 是否有做处理
/************************************************************************/
bool dsskeyuilogic_OnDsskeyLoad(DssKeyLogicData & logicData);

/************************************************************************/
// 函数描述  ： dsskey使用时是否需要密码
// 参数说明 ： [eType]: Dsskey类型
// 返回值 ： 是否需要
/************************************************************************/
bool dsskeyuilogic_IsNeedPassword(DssKey_Type eType);
