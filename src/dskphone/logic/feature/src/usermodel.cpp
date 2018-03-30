#include "usermodel.h"
#include <ETLLib.hpp>
#include "path_define.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "devicelib/phonedevice.h"
#include "usermodemanager.h"
#include "feature_inc.h"
#include "commonunits/t2xpath.h"
#include "commonunits/commonunits_def.h"
#define szLevelTranslatePath            RESOURCE_PATH"LevelTranslate.xml"
#define szWebItemsLevelPath             CONFIG_USER_PATH"WebItemsLevel.cfg"
#define szItemMapFile                   FACTORY_PATH"user/ItemNameMap.csv"
#define szActionTreeRelation            FACTORY_PATH"user/ActionTreeRelation.xml"

CUserModel::CUserModel()
{
}

CUserModel::~CUserModel()
{
    UnInit();
}

// 初始化
void CUserModel::UnInit()
{
    m_mapPermissionInfo.clear();
}

yl::string CUserModel::GetItemMapFilePath()
{
    yl::string strPath = res_getPathByName(RESOURCE_PATH_ITEMMAP);
    if (!pathFileExist(strPath.c_str()))
    {
        strPath = szItemMapFile;
    }

    return strPath.c_str();
}

yl::string CUserModel::GetActionTreeFilePath()
{
    yl::string strPath = res_getPathByName(RESOURCE_PATH_ACTIONTREE);
    if (!pathFileExist(strPath.c_str()))
    {
        strPath = szActionTreeRelation;
    }

    return strPath.c_str();
}

// 初始化
void CUserModel::Init()
{
    // 加载ItemName映射表
    m_relationItemName.OpenRelationFile(GetItemMapFilePath());

    // 先读取用户配置文件
    LoadPermissionData(GetInitThreeLevelFileName().c_str());

    // 后加载父子节点表
    LoadActionTreeRelation(GetActionTreeFilePath().c_str());
}

// 根据Action获取显示权限等级
int CUserModel::GetShowLevByAction(const char * lpszAction)
{
    if (NULL == lpszAction
            || '\0' == *lpszAction)
    {
        return 0;
    }

    // 同义词映射
    yl::string strItemName = m_relationItemName.GetStandardString(lpszAction);

    ITER_PERMISSIONINFO itr = m_mapPermissionInfo.find(strItemName);
    if (itr != m_mapPermissionInfo.end())
    {
        return itr->second.m_iOpenShowLev;
    }

    return 0;
}

// 根据Action获取读写权限等级
int CUserModel::GetWReadLevByAction(const char * lpszAction)
{
    if (NULL == lpszAction
            || '\0' == *lpszAction)
    {
        return 0;
    }

    // 同义词映射
    yl::string strItemName = HandleSpecialAtcion(lpszAction);
    strItemName = m_relationItemName.GetStandardString(strItemName);

    ITER_PERMISSIONINFO itr = m_mapPermissionInfo.find(strItemName);
    if (itr != m_mapPermissionInfo.end())
    {
        return itr->second.m_iMaxWReadLev;
    }

    return 0;
}

// 用户配置是否为11：兼容网页的错
bool CUserModel::IsDoubleLev1(const char * lpszAction)
{
    if (NULL == lpszAction
            || '\0' == *lpszAction)
    {
        return false;
    }

    // 同义词映射
    yl::string strItemName = HandleSpecialAtcion(lpszAction);
    strItemName = m_relationItemName.GetStandardString(strItemName);

    ITER_PERMISSIONINFO itr = m_mapPermissionInfo.find(strItemName);
    if (itr != m_mapPermissionInfo.end())
    {
        return (1 == itr->second.m_iOpenShowLev && 1 == itr->second.m_iOpenWReadLev);
    }

    return false;
}

// 对特殊字符串进行处理
yl::string CUserModel::HandleSpecialAtcion(const char * lpszAction)
{
    if (NULL == lpszAction
            || '\0' == *lpszAction)
    {
        return "";
    }

    // 特殊字符进行处理 主要针对Ext Dsskey
    yl::string strItemName = lpszAction;
    int nFound = strItemName.find_last_of(';');
    if (nFound != yl::string::npos)
    {
        strItemName = strItemName.substr(0, nFound);
    }

    // 如line_key_item_,anonymous_call_line_,expand_key_item_
    nFound = strItemName.find_last_of('_');
    if (nFound != yl::string::npos)
    {
        int nIndex = -1;
        yl::string strSub = strItemName.substr(nFound + 1);
        if (sscanf(strSub.c_str(), "%d", &nIndex) == 1)
        {
            strItemName = strItemName.substr(0, nFound + 1);
        }
    }

    return strItemName;
}

yl::string CUserModel::GetInitThreeLevelFileName()
{
    yl::string strFileName = "";
    if (GET_CONFIG_PROCESS_TYPE() == CFG_SERVER_PROCESS)
    {
        strFileName = szLevelTranslatePath;
    }
    else if (GET_CONFIG_PROCESS_TYPE() == INI_FILE_PROCESS)
    {
        strFileName = szWebItemsLevelPath;
    }
    return strFileName;
}

// 加载权限数据
bool CUserModel::LoadPermissionData(const char * lpszFileName)
{
    if (NULL == lpszFileName)
    {
        return false;
    }
    if (GET_CONFIG_PROCESS_TYPE() == CFG_SERVER_PROCESS)
    {

        return LoadPermissionFromCfgserver(lpszFileName);
    }
    else if (GET_CONFIG_PROCESS_TYPE() == INI_FILE_PROCESS)
    {

        return LoadPermissionFromIni(lpszFileName);
    }
    else
    {

        return false;
    }

}

bool CUserModel::LoadPermissionFromCfgserver(const char * lpszFileName)
{
    if (NULL == lpszFileName)
    {
        return false;
    }
    xml_document docTranlate;
    if (!docTranlate.load_file(lpszFileName))
    {
        return false;
    }

    yl::string strItemName = "";
    for (xml_node nodeGui = docTranlate.child("root").child("mainpage");
            nodeGui;
            nodeGui = nodeGui.next_sibling("mainpage"))
    {
        if (strcmp(nodeGui.attribute("name").value(), "GUI") != 0)
        {
            continue;
        }

        for (xml_node nodeItem = nodeGui.child("item");
                nodeItem;
                nodeItem = nodeItem.next_sibling("item"))
        {
            //读取三级权限值
            yl::string strValue = configParser_GetLevelCfg(nodeItem.attribute("m7").value(), "");
            // 解析权限值
            CPermissionInfo info;
            ReadPermissionValue(strValue, info.m_iOpenShowLev, info.m_iOpenWReadLev);
            info.m_iMaxWReadLev = info.m_iOpenWReadLev;

            // 同义词映射
            strItemName = m_relationItemName.GetStandardString(nodeItem.attribute("key").value());
            m_mapPermissionInfo[strItemName] = info;
        }

    }

    return !m_mapPermissionInfo.empty();
}

bool CUserModel::LoadPermissionFromIni(const char * lpszFileName)
{
    if (NULL == lpszFileName)
    {
        return false;
    }

    // 读取文件
    chIniFileA file;
    if (!file.LoadFromFile(lpszFileName))
    {
        return false;
    }

    // section，链表中的元素为section指针
    yl::string strItemName = "";
    iniObjList * pCurrentSection = &file;
    iniObjList::iterator it = pCurrentSection->begin();
    for (; it != pCurrentSection->end(); ++it)
    {
        iniObject * pLine = *it;
        if (NULL == pLine)
        {
            continue;
        }

        // 不是Section
        if (pLine->type() != LT_SECTION)
        {
            continue;
        }

        // 不是GUI的Section
        if ("GUI" != pLine->key())
        {
            continue;
        }

        // key，链表中的元素为key指针
        iniSection * pSection = (iniSection *)pLine;
        iniSection::iterator itS = pSection->begin();
        for (; itS != pSection->end(); ++itS)
        {
            iniKey * pSrcKeyLine = (iniKey *)(*itS);
            if (pSrcKeyLine->type() == LT_KEY)
            {
                // 解析权限值
                CPermissionInfo info;
                ReadPermissionValue(pSrcKeyLine->value(), info.m_iOpenShowLev, info.m_iOpenWReadLev);
                info.m_iMaxWReadLev = info.m_iOpenWReadLev;

                // 同义词映射
                strItemName = m_relationItemName.GetStandardString(pSrcKeyLine->key());
                m_mapPermissionInfo[strItemName] = info;
            }
        }

        break;
    }
    return !m_mapPermissionInfo.empty();
}

// 读取权限值 优化于网页规则一致
// http://10.3.5.199/Bug.php?BugID=61889
void CUserModel::ReadPermissionValue(const yl::string & strPermissionInfo, int & iShowLevel,
                                     int & iWReadLevel)
{
    iShowLevel = iWReadLevel = 0;

    // 示例：" | 03 | 02 || || 3| " 解析出来是 03、02、3
    YLVector<yl::string> vecList;
    ParseString(vecList, strPermissionInfo, "| ");

    yl::string strInfo;
    if (vecList.size() > 0)
    {
        // 如果级数不够，则取最后一级
        size_t ilevel = _UserModeManager.LockLevel();
        strInfo = vecList.size() > ilevel ? vecList[ilevel] : vecList[vecList.size() - 1];
    }

    if (!strInfo.empty())
    {
        if (isdigit(strInfo[0]))
        {
            iShowLevel = strInfo[0] - '0';
        }
        if (strInfo.size() >= 2 && isdigit(strInfo[1]))
        {
            iWReadLevel = strInfo[1] - '0';
        }
    }
}

// 加载界面树形关系表
bool CUserModel::LoadActionTreeRelation(const char * lpszFileName)
{
    if (NULL == lpszFileName)
    {
        return false;
    }

    xml_document doc;
    xml_parse_result objResult = doc.load_file(lpszFileName);
    if (status_ok != objResult.status)
    {
        return false;
    }

    // 获取根节点
    xml_node nodeRoot = doc.child("relation_actiontree");
    if (!nodeRoot)
    {
        return false;
    }

    // 获取版本节点：依赖ConfigMapperId
    yl::string strConfigId  = "", strValue = "";
    for (xml_node child = nodeRoot.first_child(); child; child = child.next_sibling())
    {
        strConfigId = child.attribute("configMapperId").value();
        strValue = child.attribute("value").value();

        // "" == 公共版本  "1" == BroadSoft版本 "0" == 普通版本 (等待扩展)
        if (strConfigId.empty() || configParser_GetConfigString(strConfigId.c_str()) == strValue)
        {
            CallbackTraversal(child);
        }
    }

    return true;
}

// 递归遍历xml的所有节点
void CUserModel::CallbackTraversal(xml_node node)
{
    if (!node)
    {
        return ;
    }

    for (xml_node child = node.first_child(); child; child = child.next_sibling())
    {
        // 子节点 直接父节点
        yl::string strActionChild = m_relationItemName.GetStandardString(child.attribute("action").value());
        yl::string strActionParent = m_relationItemName.GetStandardString(node.attribute("action").value());

        // 子节点没开放 用直接父节点信息替换子节点
        if (m_mapPermissionInfo.find(strActionChild) == m_mapPermissionInfo.end())
        {
            if (m_mapPermissionInfo.find(strActionParent) != m_mapPermissionInfo.end())
            {
                m_mapPermissionInfo[strActionChild] = m_mapPermissionInfo[strActionParent];
            }
        }
        else
        {
            // m_iMaxWReadLev先初始化为直接父节点
            if (m_mapPermissionInfo.find(strActionParent) != m_mapPermissionInfo.end())
            {
                m_mapPermissionInfo[strActionChild].m_iMaxWReadLev =
                    m_mapPermissionInfo[strActionParent].m_iMaxWReadLev;
            }

            // 本身配置的值比直接父节点大
            if (m_mapPermissionInfo[strActionChild].m_iMaxWReadLev <
                    m_mapPermissionInfo[strActionChild].m_iOpenWReadLev)
            {
                m_mapPermissionInfo[strActionChild].m_iMaxWReadLev =
                    m_mapPermissionInfo[strActionChild].m_iOpenWReadLev;
            }

            // 本身配置的值比直接父节点大
            if (m_mapPermissionInfo[strActionChild].m_iOpenShowLev <
                    m_mapPermissionInfo[strActionParent].m_iOpenShowLev)
            {
                m_mapPermissionInfo[strActionChild].m_iOpenShowLev =
                    m_mapPermissionInfo[strActionParent].m_iOpenShowLev;
            }
        }

        // 有子节点继续递归
        if (child.first_child())
        {
            CallbackTraversal(child);
        }
    }
}

MAP_PERMISSIONINFO & CUserModel::GetMapPermissionInfo()
{
    return m_mapPermissionInfo;
}

