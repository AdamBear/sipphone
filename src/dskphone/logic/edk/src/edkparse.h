///////////////////////////////////////////////////////////
//  CEdkParse.h
//  Implementation of the Class CEdkParse
//  Created on:      21-一月-2015 15:32:26
//  Original author: dlw
///////////////////////////////////////////////////////////

#if !defined(__EDK_PARSER_H__)
#define __EDK_PARSER_H__

#include "edkmanager.h"
#include "edkdatamanager.h"
#include <singletonclassdefine.h>
#include "../include/edk_def.h"

enum ACTOINPARSETYPE
{
    PARSE_NON = -1,
    PARSE_NORMAL,
    PARSE_LABEL,
};

class CEdkParse
{
    DECLARE_SINGLETON_CLASS(CEdkParse)
public:

    // 执行命令流
    bool DoAction();

    // 执行设置Label的操作
    bool DoLabelAction();

    // 解析命令
    bool ParseAction(const yl::string & strAction, int iDsskeyId = -1);

    yl::string ParseActionForType(const yl::string & strAction, EDKACTIONTYPE eType);

private:
    // 获取宏宏命令的action
    yl::string GetMacroAction(const yl::string & strAction);

    bool ParseCommand(const yl::string & strAction, YLList<int> & listMacro,
                      YLList<yl::string> & listSoftkey,
                      bool bNeedInterruptAfterFault = true, int iDsskeyId = -1);

    // 解析已生成的命令列表，合并相同命令
    bool MergeCommand();

    // 对每个类型进行解析
    bool ParseActionString(yl::string & strAction, YLList<int> & listMacro,
                           YLList<yl::string> & listSoftkey, int iDsskeyId = -1);

    // 判断当前命令是否完整
    bool IsActionComplete();

    // 命令是否需要用$包含
    bool IsActionWithDollar(const yl::string & strAction);

    // 命令是否合法
    bool IsActionValid(const yl::string & strAction);

    // 检查是否有嵌套调用宏
    bool CheckMacroReCall(int iIndex, YLList<int> & listMacro);

    // 检查是否有嵌套调用custom softkey
    bool CheckSoftkeyReCall(const yl::string & strSoftkeyID, YLList<yl::string> & listSoftkey);
    // 解析Type actoin
    bool ParseTypeAction(yl::string & strAction, edk_action_data_t & edkAction);
    // 解析command action
    bool ParseCommandAction(yl::string & strAction, edk_action_data_t & edkAction);
    // 解析prompt action
    bool ParsePromptAction(yl::string & strAction, edk_action_data_t & edkAction);
    // 删除指定类型的action
    bool RemoveActionByType(EDKACTIONTYPE eActionType);
    // 获取第一个指定类型的action
    bool GetActionByType(EDKACTIONTYPE eActionType, edk_action_data_t & edkActionData);
    // 初始化结构体
    void InitObjAction();
private:
    //CEdkDataManager _EDKDataMgr;

    // 解析完生成的命令结构体
    EdkActionInfo m_objActionInfo;

    int m_iId;
};

// 实例
#define _EDKParser GET_SINGLETON_INSTANCE(CEdkParse)

#endif // !defined(__EDK_PARSER_H__)
