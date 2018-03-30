#include "configfiltermanager.h"
#include "path_define.h"
#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"
#include "dsklog/log.h"
#include "configparser/modconfigparser.h"

namespace
{
const char * const kszFilterName = "settingConfigFilter.xml";
const int MAX_LOOP_TIME = 10000;        // 最大循环次数

const char * const kszStaticConfig = "staticconfig";
const char * const kszDynamicConfig = "dynamicconfig";

const char * const kszAuthorityID = "authorid";
const char * const kszFilterType = "filtertype";
const char * const kszValueType = "valuetype";
const char * const kszVisiable = "visiable";
const char * const kszUnVisiable = "unvisiable";
const char * const kszUnEditable = "uneditable";
const char * const kszValue = "value";
const char * const kszConfig = "config";
const char * const kszFilterID = "id";
}

CConfigFilterMgr * CConfigFilterMgr::m_pInstance = NULL;

CConfigFilterMgr * CConfigFilterMgr::GetInstance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new CConfigFilterMgr();
    }
    return m_pInstance;
}

CConfigFilterMgr::CConfigFilterMgr()
    : m_mapStaticFilter()
    , m_mapDynamicFilter()
{
}

void CConfigFilterMgr::Init()
{
    LoadConfigFilterByFileName(kszFilterName);
}

bool CConfigFilterMgr::IsAllowShowStatic(const yl::string & strAuthorityID)
{
    bool bFind = m_mapStaticFilter.find(strAuthorityID) != m_mapStaticFilter.end();
    if (bFind)
    {
        return m_mapStaticFilter[strAuthorityID]->bShow;
    }

    return true;
}

SettingDynamicConfigFilter * CConfigFilterMgr::GetAllowShowDynamic(const yl::string &
        strAuthorityID)
{
    if (m_mapDynamicFilter.find(strAuthorityID) != m_mapDynamicFilter.end())
    {
        return m_mapDynamicFilter[strAuthorityID];
    }

    return NULL;
}

bool CConfigFilterMgr::DynamicFilter(SettingDynamicConfigFilter * pDynamicFilter,
                                     const yl::string & strConfig
                                     , const yl::string & strAuthority, const yl::string & strValue
                                     , bool & bShow, bool & bEditable)
{
    bool bResult = false;
    if (NULL == pDynamicFilter)
    {
        return bResult;
    }

    // 显示、隐藏过滤
    if ((SETTING_FILTER_TYPE_UNVISIABLE & pDynamicFilter->eFilterType)
            || (SETTING_FILTER_TYPE_VISIABLE & pDynamicFilter->eFilterType))
    {
        SETTING_FILTER_TYPE eTempType = (SETTING_FILTER_TYPE_UNVISIABLE & pDynamicFilter->eFilterType)
                                        ? SETTING_FILTER_TYPE_UNVISIABLE : SETTING_FILTER_TYPE_VISIABLE;
        LIST_DYNAMIC_ITEM_PTR::iterator iter = pDynamicFilter->listFilter.begin();
        for (; iter != pDynamicFilter->listFilter.end() && !bResult; ++iter)
        {
            SettingDynamicItem * pDynamicItem = *iter;
            if (NULL != pDynamicItem)
            {
                bResult = bResult || DealWithDynamicItemFilter(pDynamicItem, strConfig, strAuthority, strValue
                          , bShow, bEditable, eTempType);
            }
        }
    }
    // 是否可编辑过滤
    if (SETTING_FILTER_TYPE_UNEDITABLE & pDynamicFilter->eFilterType)
    {
        bool bTempResult = false;
        LIST_DYNAMIC_ITEM_PTR::iterator iter = pDynamicFilter->listEditFilter.begin();
        for (; iter != pDynamicFilter->listEditFilter.end() && !bTempResult; ++iter)
        {
            SettingDynamicItem * pDynamicItem = *iter;
            if (NULL != pDynamicItem)
            {
                bTempResult = DealWithDynamicItemFilter(pDynamicItem, strConfig, strAuthority, strValue
                                                        , bShow, bEditable, SETTING_FILTER_TYPE_UNEDITABLE);
            }
        }

        bResult = bResult || bTempResult;
    }

    return bResult;
}

bool CConfigFilterMgr::DealWithDynamicItemFilter(SettingDynamicItem * pDynamicItem,
        const yl::string & strConfig, const yl::string & strAuthority, const yl::string & strValue,
        bool & bShow, bool & bEditable, SETTING_FILTER_TYPE eType)
{
    bool bResult = false;
    if (NULL == pDynamicItem || strValue.empty())
    {
        return bResult;
    }

    yl::string strFilterValue = strValue;
    if (!((SETTING_CONFIG_TYPE_CONFIG == pDynamicItem->eType
            && 0 == pDynamicItem->strId.compare(strConfig))
            || (SETTING_CONFIG_TYPE_AUTHORITY == pDynamicItem->eType
                && 0 == pDynamicItem->strId.compare(strAuthority))))
    {
        return bResult;
    }

    LIST_STRING::iterator it = pDynamicItem->listValue.begin();
    for (; it != pDynamicItem->listValue.end() && !bResult; ++it)
    {
        yl::string strFilter = *it;
        bool bR = (0 == strFilterValue.compare(strFilter.trim_both()));
        if (SETTING_FILTER_TYPE_VISIABLE & eType)
        {
            if (!bR)
            {
                bShow = false;
                bResult = true;
            }
        }
        else if (SETTING_FILTER_TYPE_UNVISIABLE & eType)
        {
            if (bR)
            {
                bShow = false;
                bResult = true;
            }
        }
        if (SETTING_FILTER_TYPE_UNEDITABLE & eType)
        {
            if (bR)
            {
                bEditable = false;
                bResult = true;
            }
        }
    }

    return bResult;
}

bool CConfigFilterMgr::LoadConfigFilterByFileName(const char * pFileName)
{
    if (NULL == pFileName)
    {
        return FALSE;
    }

    // 加载xml文件,初始化数据pagedata
    yl::string strFileName(RESOURCE_PATH);
    strFileName += pFileName;

    if (!pathFileExist(strFileName.c_str()))
    {
        SETTINGUI_INFO("File [%s] not exsit", strFileName.c_str());
        return FALSE;
    }

    xml_document doc;
    doc.load_file(strFileName.c_str());
    xml_node nodeRoot = doc.first_child();
    if (!nodeRoot)
    {
        return FALSE;
    }

    ParseStaticConfigFilter(nodeRoot);

    xml_node nextNode = nodeRoot.next_sibling();
    if (nextNode)
    {
        ParseDynamicConfigFilter(nextNode);
    }

    return TRUE;
}

// 静态过滤项
void CConfigFilterMgr::ParseStaticConfigFilter(const xml_node & node)
{
    int nCnt = 0;
    xml_node nodeChildItem = node.first_child();
    for (; nodeChildItem && nCnt < MAX_LOOP_TIME; ++nCnt)
    {
        SettingStaticConfigFilter * pStaticCfgFilter = new SettingStaticConfigFilter();
        if ((NULL == pStaticCfgFilter))
        {
            continue;
        }
        pStaticCfgFilter->strAuthorityID = nodeChildItem.attribute(kszAuthorityID).as_string();
        yl::string strFilterType = nodeChildItem.attribute(kszFilterType).as_string();
        int eType = (0 == strFilterType.compare(kszVisiable));
        pStaticCfgFilter->bShow = ParseConfigItem(nodeChildItem, eType);

        m_mapStaticFilter.insert(pStaticCfgFilter->strAuthorityID, pStaticCfgFilter);

        nodeChildItem = nodeChildItem.next_sibling();
    }
}

bool CConfigFilterMgr::ParseConfigItem(const xml_node & node, int eType)
{
    int nCnt = 0;
    xml_node nodeChildItem = node.first_child();
    for (; nodeChildItem && nCnt < MAX_LOOP_TIME; ++nCnt)
    {
        yl::string strName = nodeChildItem.name();
        if (0 == strName.compare(kszConfig))
        {
            yl::string strConfigID = nodeChildItem.attribute(kszFilterID).as_string();
            yl::string strVType = nodeChildItem.attribute(kszValueType).as_string();
            bool bR = (bool)eType;
            if ("int" == strVType.trim_both())
            {
                int iV = nodeChildItem.attribute(kszValue).as_int();
                bR = (iV == configParser_GetConfigInt(strConfigID.c_str()));
            }
            else if ("string" == strVType.trim_both())
            {
                yl::string strValue = nodeChildItem.attribute(kszValue).as_string();
                bR = (strValue == configParser_GetConfigString(strConfigID.c_str()));
            }
            // 隐藏项(visiable时用与；unvisiable时用或)
            if ((1 == eType && !bR) || (0 == eType && bR))
            {
                return false;
            }
        }

        nodeChildItem = nodeChildItem.next_sibling();
    }

    return true;
}

// 动态过滤项
void CConfigFilterMgr::ParseDynamicConfigFilter(const xml_node & node)
{
    int nCnt = 0;
    xml_node nodeChildItem = node.first_child();
    for (; nodeChildItem && nCnt < MAX_LOOP_TIME; ++nCnt)
    {
        yl::string strAuthorityID = nodeChildItem.attribute(kszAuthorityID).as_string();
        SettingDynamicConfigFilter * pDynamicCfgFilter = m_mapDynamicFilter[strAuthorityID];
        bool bExist = true;
        if (NULL == pDynamicCfgFilter)
        {
            bExist = false;
            pDynamicCfgFilter = new SettingDynamicConfigFilter();
            if (NULL == pDynamicCfgFilter)
            {
                continue;
            }
            pDynamicCfgFilter->strAuthorityID = strAuthorityID;
        }
        SETTING_FILTER_TYPE eFilterType = pDynamicCfgFilter->eFilterType;
        eFilterType = (SETTING_FILTER_TYPE)((int)eFilterType | (int)StringFilterType2Enum(
                                                nodeChildItem.attribute(kszFilterType).as_string()));
        pDynamicCfgFilter->eFilterType = (SETTING_FILTER_TYPE)eFilterType;

        ParseConfigItem(nodeChildItem, pDynamicCfgFilter);

        if (!bExist)
        {
            m_mapDynamicFilter.insert(pDynamicCfgFilter->strAuthorityID, pDynamicCfgFilter);
        }

        nodeChildItem = nodeChildItem.next_sibling();
    }
}

void CConfigFilterMgr::ParseConfigItem(const xml_node & node, SettingDynamicConfigFilter * pDynamic)
{
    if (NULL == pDynamic)
    {
        return;
    }

    int nCnt = 0;
    xml_node nodeChild = node.first_child();
    for (; nodeChild && nCnt < MAX_LOOP_TIME; ++nCnt)
    {
        SettingDynamicItem * pDynamicItem = new SettingDynamicItem();
        if (NULL != pDynamicItem)
        {
            yl::string strName = nodeChild.name();
            pDynamicItem->eType = (0 == strName.compare(kszConfig))
                                  ? SETTING_CONFIG_TYPE_CONFIG : SETTING_CONFIG_TYPE_AUTHORITY;
            pDynamicItem->strId = nodeChild.attribute(kszFilterID).as_string();
            pDynamicItem->strValueType = nodeChild.attribute(kszValueType).as_string();
            commonAPI_splitStringBySeparator(nodeChild.attribute(kszValue).as_string(), ','
                                             , pDynamicItem->listValue);

            if (SETTING_FILTER_TYPE_UNEDITABLE & pDynamic->eFilterType)
            {
                pDynamic->listEditFilter.push_back(pDynamicItem);
            }
            if ((SETTING_FILTER_TYPE_VISIABLE & pDynamic->eFilterType)
                    || (SETTING_FILTER_TYPE_UNVISIABLE & pDynamic->eFilterType))
            {
                pDynamic->listFilter.push_back(pDynamicItem);
            }
        }

        nodeChild = nodeChild.next_sibling();
    }
}

SETTING_FILTER_TYPE CConfigFilterMgr::StringFilterType2Enum(const yl::string & strFilterType)
{
    if (0 == strFilterType.compare(kszVisiable))
    {
        return SETTING_FILTER_TYPE_VISIABLE;
    }
    else if (0 == strFilterType.compare(kszUnVisiable))
    {
        return SETTING_FILTER_TYPE_UNVISIABLE;
    }
    else if (0 == strFilterType.compare(kszUnEditable))
    {
        return SETTING_FILTER_TYPE_UNEDITABLE;
    }
    else
    {
        return SETTING_FILTER_TYPE_BASE;
    }
}


