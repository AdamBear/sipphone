///////////////////////////////////////////////////////////
//  Cedkmanager.h
//  Implementation of the Class CEdkDataManager
//  Created on:      21-一月-2015 15:32:25
//  Original author: dlw
///////////////////////////////////////////////////////////

#if !defined(__EDK_DATA_MANAGER_H__)
#define __EDK_DATA_MANAGER_H__

#include <singletonclassdefine.h>
#include <ETLLib.hpp>
#include "../include/edk_def.h"

class CEdkDataManager
{
    DECLARE_SINGLETON_CLASS(CEdkDataManager)
public:
    // 初始化
    void Init();
    // 消息处理
    static LRESULT OnMessage(msgObject & objMessage);
    // edk开关是否开启
    bool IsEnable();
    // 通过宏名获取宏
    // true[有找到] false[没找到]
    bool GetMacroByName(const yl::string & strName, edk_macro_data_t & sMacroData);
    // 通过弹出框的索引查找弹出框信息
    // true[有找到] false[没找到]
    bool GetPromptByIndex(int iIndex, edk_prompt_data_t & sPromptData);
    // 通过 UI 类型，获取 softkey 数据
    void GetSoftKeyDataByUIType(EDKUITYPE eUIType, listEdkSoftKey & listSoftKey);
    // 解析 EDKConfig.xml
    bool LoadEDKConfigXml();
    // 设置 Prompt
    // true[设置成功] false[设置失败]
    bool SetPrompt(const edk_prompt_data_t & sPromptData);
    // 通过softkeytype获取该softkey的acton
    bool GetSoftKeyAction(EDKSOFTKEYTYPE eType, yl::string & strAction);
    // 通过softkeytype获取该softkey的label
    bool GetSoftKeyLabel(EDKSOFTKEYTYPE eType, yl::string & strLabel);

    // 没有保存在配置文件的prompt生成临时下标,从50开始
    int GetFreePromptIndex();
    // 添加弹出框到链表
    bool AddPrompt(edk_prompt_data_t & sPromptData);
    // 通过类型获取softkey数据
    bool GetSoftkeyData(EDKSOFTKEYTYPE eType, edk_softkey_data_t & edkSoftkeyData);
    // 通过type获取Softkey ID
    bool GetSoftkeyID(EDKSOFTKEYTYPE eType, yl::string & strID);
    // 判断是否为custom softkey
    bool IsCustomSoftkey(const yl::string & strSoftkeyID);

    void Clear();

    // 通过softkey id获取type
    EDKSOFTKEYTYPE GetSoftkeyTypeByID(const yl::string & strSoftkeyID);

    // 通过softkey id获取softkey data
    bool GetSoftkeyDataByID(const yl::string & strSoftkeyID, edk_softkey_data_t & edkSoftkey);

    // 清空临时prompt
    void ClearTempPromt();
private:
    // 将UI 与softkey保存到 map 中
    bool AddDataToSoftkeyMap(const yl::string & strUIType, const edk_softkey_data_t & dataSoftKey);

    // 将 xml 中ui的类型转换成 EDKUITYPE
    EDKUITYPE MapEdkUIType(const yl::string & strUIType);

    // 将索引映射成edksoftkey类型
    EDKSOFTKEYTYPE GetEdkSoftKeyType(int iIndex);

private:
    listEdkMacro m_listMacro;
    listEdkPrompt m_listPrompt;
    listEdkSoftKey m_listSoftkey;
    mapEdkSoftKey m_mapSoftKey;
};

// 实例
#define _EDKDataMgr GET_SINGLETON_INSTANCE(CEdkDataManager)
#endif // !defined(__EDK_DATA_MANAGER_H__)
