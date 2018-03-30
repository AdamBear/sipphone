#include "dataloader.h"
#include "configparser/modconfigparser.h"
#include "configiddefine.h"
#include "dsklog/log.h"

#include "settinguibaseitemdata.h"
#include "settinguibasepagedata.h"
#include <settinguilogic/include/settingdefine.h>


CDataLoader * CDataLoader::m_pInstance = NULL;

CDataLoader * CDataLoader::GetInstance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new CDataLoader();
        if (NULL != m_pInstance)
        {
            m_pInstance->Init();
        }
    }
    return m_pInstance;
}

CDataLoader::CDataLoader()
{
    m_loaderMap.clear();
}

void CDataLoader::Init()
{
}

void CDataLoader::RegisterLoadFun(const yl::string & strPageName, PageLoadFun pFun)
{
    m_loaderMap.insert(strPageName, pFun);
}

PageLoadFun CDataLoader::GetLoadFun(const yl::string & strPageName)
{
    LoaderMap::iterator iter = m_loaderMap.find(strPageName);
    if (iter == m_loaderMap.end())
    {
        return NULL;
    }

    return iter->second;
}

bool CDataLoader::LoadData(CSettingUIPageDataBase * pPageInfo, int iLineID/* = -1*/)
{
    if (NULL == pPageInfo || pPageInfo->GetPageID().empty())
    {
        return false;
    }

    PageLoadFun pFun = GetLoadFun(pPageInfo->GetPageID());
    if (NULL != pFun)
    {
        return pFun(pPageInfo, iLineID);
    }

    return NormalLoad(pPageInfo, iLineID);
}

bool CDataLoader::LoadItemData(CSettingUIItemDataBase * pItemData, int iLineID /*= -1*/)
{
    if (NULL == pItemData)
    {
        return false;
    }
    yl::string strConfigID = pItemData->GetConfigID();
    if (NULL == pItemData || strConfigID.empty())
    {
        return false;
    }

    int iPos = strConfigID.find("%d");
    yl::string strM7Config = configParser_GetCfgAttribute(strConfigID.c_str(), "M7");
    int nFirstPos = strM7Config.find("%d", 0);
    int nSecondPos = (yl::string::npos != nFirstPos) ? strM7Config.find("%d",
                     nFirstPos + 2) : (yl::string::npos);
    if (iPos != yl::string::npos)
    {
#if IF_SETTING_CONFIG_LOAD_SAVE_DEBUG
        SETTINGUI_INFO("------------------- 1 lineID[%d] config[%s] ======="
                       , iLineID, strConfigID.c_str());
#endif
        yl::string strRemain =  strConfigID.substr(iPos + 2, strConfigID.length() - iPos - 2);
        if (!strRemain.empty() && strRemain.find("%d") != yl::string::npos)
        {
            pItemData->SetValue(LoadDoubleParamconfig(strConfigID, iLineID));
        }
        else
        {
            pItemData->SetValue(LoadSingleParamconfig(strConfigID, iLineID, pItemData->GetItemID()));
        }
#if IF_SETTING_CONFIG_LOAD_SAVE_DEBUG
        SETTINGUI_INFO("------------------- 4 value[%s] ======="
                       , pItemData->GetValue().c_str());
#endif

    }
    else if (nSecondPos != yl::string::npos)
    {
        pItemData->SetValue(LoadDoubleParamconfig(strConfigID, iLineID));
    }
    else
    {
        if (iLineID < 0)
        {
            if (yl::string::npos != nFirstPos)
            {
                pItemData->SetValue(LoadSingleParamconfig(strConfigID, iLineID, pItemData->GetItemID()));
            }
            else
            {
                pItemData->SetValue(configParser_GetConfigString(strConfigID.c_str()));
            }
        }
        else
        {
            pItemData->SetValue(LoadSingleParamconfig(strConfigID, iLineID, pItemData->GetItemID()));
        }
    }
    // int类型验证
    if (WIDGET_VALUE_TYPE_INT == pItemData->GetValueType()
            && pItemData->GetValue().empty())
    {
        pItemData->SetValue("0");
    }

#if IF_SETTING_CONFIG_LOAD_SAVE_DEBUG
    SETTINGUI_INFO("---------- load dis[%s], value[%s], m7[%s], twice[%d][%d] ===="
                   , pItemData->GetDisplayName().c_str(), pItemData->GetValue().c_str()
                   , configParser_GetCfgAttribute(strConfigID.c_str(), "M7").c_str(), nSecondPos);
#endif
    return true;
}

//用configMapper读取数据
bool CDataLoader::NormalLoad(CSettingUIPageDataBase * pPageData, int iLineID/* = -1*/)
{
    if (NULL == pPageData)
    {
        return false;
    }

    int nTotal = pPageData->GetTotalItemNumber();
    for (int i = 0; i < nTotal; ++i)
    {
        CSettingUIItemDataBasePtr pItemData = pPageData->GetItemByDataIndex(i);
        LoadItemData(pItemData, iLineID);
    }

    return true;
}

#define ACCOUNT_SERVER_PARAM1 0
#define ACCOUNT_SERVER_PARAM2 1

yl::string CDataLoader::LoadDoubleParamconfig(const yl::string & strConfigId, const int iLineID)
{
#if IF_SETTING_CONFIG_LOAD_SAVE_DEBUG
    SETTINGUI_INFO("-=-=-==-=-= [CDataLoader::LoadDoubleParamconfig] config[%s], lineID[%d] ========="
                   , strConfigId.c_str(), iLineID);
#endif
    // 只有1项，直接调用返回
    return configParser_GetCfgItemStringValue(strConfigId.c_str(), iLineID, ACCOUNT_SERVER_PARAM1);
}

yl::string CDataLoader::LoadSingleParamconfig(const yl::string & strConfigId, const int iLineID,
        const yl::string & strItemID)
{
    yl::string strValue("");
    if (iLineID >= 0)
    {
        strValue = configParser_GetCfgItemStringValue(strConfigId.c_str(), iLineID);
    }
    else
    {
        if (0 == strItemID.compare("h323_gatekeeper_server1")
                || 0 == strItemID.compare("h323_gatekeeper_port1"))
        {
            strValue = configParser_GetCfgItemStringValue(strConfigId.c_str(), ACCOUNT_SERVER_PARAM1);
        }
        else
        {
            strValue = configParser_GetCfgItemStringValue(strConfigId.c_str(), ACCOUNT_SERVER_PARAM2);
        }
    }

#if IF_SETTING_CONFIG_LOAD_SAVE_DEBUG
    SETTINGUI_INFO("-=-=-==-=-= [CDataLoader::LoadSingleParamconfig] config[%s], lineID[%d], itemid[%s], getValue[%s] ========="
                   , strConfigId.c_str(), iLineID, strItemID.c_str(), strValue.c_str());
#endif

    return strValue;
}
