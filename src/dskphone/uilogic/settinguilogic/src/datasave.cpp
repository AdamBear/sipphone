#include "datasave.h"
#include "configparser/modconfigparser.h"
#include <ETLLib.hpp>
#include "etlmsghandler/modetlmsghandler.h"
#include "interfacedefine.h"
#include "dataloader.h"
#include "configiddefine.h"

#include "settinguilogic/include/common.h"
#include "commonapi/stringhelperapi.h"

#include "settinguibasepagedata.h"
#include "settinguibaseaccountpagedata.h"
#include "settinguibaseitemdata.h"
#include "settinguicheckboxitemdata.h"
#include "settinguicomboboxitemdata.h"
#include "settinguilineedititemdata.h"
#include "settinguimenuitemdata.h"
#include "settinguisoftkeydata.h"

#include "networksettingmanager.h"

//#include "rtserver/service_system.h"

CDataSave * CDataSave::m_pInstance = NULL;

CDataSave * CDataSave::GetInstance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new CDataSave();
        if (NULL != m_pInstance)
        {
            m_pInstance->Init();
        }
    }
    return m_pInstance;
}

CDataSave::CDataSave()
{
    m_saverMap.clear();
}

void CDataSave::Init()
{
}

void CDataSave::RegisterSaveFun(const yl::string & strPageName, PageSaveFun pFun)
{
    m_saverMap.insert(strPageName, pFun);
}

PageSaveFun CDataSave::GetSaveFun(const yl::string & strPageName)
{
    SaverMap::iterator iter = m_saverMap.find(strPageName);
    if (iter == m_saverMap.end())
    {
        return NULL;
    }

    return iter->second;
}

bool CDataSave::SaveData(CSettingUIPageDataBase * pPageInfo)
{
    if (NULL == pPageInfo || pPageInfo->GetPageID().empty())
    {
        return false;
    }

    PageSaveFun pFun = GetSaveFun(pPageInfo->GetPageID());
    if (NULL != pFun)
    {
        return pFun(pPageInfo);
    }

    return NormalSave(pPageInfo);
}

static bool Compare(CSettingUIMessageDataPtr lhs, CSettingUIMessageDataPtr rhs)
{
    if (NULL == lhs || NULL == rhs)
    {
        return false;
    }

    return (lhs->GetPriority() < rhs->GetPriority());
}

//用configMapper保存数据
bool CDataSave::NormalSave(CSettingUIPageDataBase * pPageInfo)
{
    if (NULL == pPageInfo)
    {
        return false;
    }

    LIST_SETTING_MSG_DATA msgList = pPageInfo->GetListPageMessageData();

    int iLineID = -1;
    CSettingUIAccountPageDataBasePtr pAccountBase = CSettingUIAccountPageDataBase::GetAccountPageData(
                pPageInfo);
    if (NULL != pAccountBase)
    {
        iLineID = pAccountBase->GetLineID();
#if IF_SETTING_CONFIG_LOAD_SAVE_DEBUG
        SETTINGUI_INFO("-$$$$$$$$$$$$$$$$$$$$ ######### [CDataSave::NormalSave] lineID[%d] test accountPageData ============"
                       , iLineID);
#endif
    }
    bool bValueChanged = false;

    int nTotal = pPageInfo->GetTotalItemNumber();
    for (int i = 0; i < nTotal; ++i)
    {
        CSettingUIItemDataBasePtr pItemData = pPageInfo->GetItemByDataIndex(i);
        if (NULL == pItemData || pItemData->GetConfigID().empty()
                || !pItemData->IsEnable() || !pItemData->IsShow())
        {
            continue;
        }

        // 过滤项判断
        CSettingUILineEditItemDataPtr pInputItemData = CSettingUILineEditItemData::GetLineEditDataItem(
                    pItemData);
        if (NULL != pInputItemData && !pInputItemData->GetCheckBaseID().empty())
        {
            yl::string strCheckID = pInputItemData->GetCheckBaseID();
            // 获取当前界面被过滤项
            CSettingUIItemDataBasePtr pFilterItemData = pPageInfo->GetItemByItemId(strCheckID);
#if IF_SETTING_FILTER_DEBUG
            if (NULL != pFilterItemData)
            {
                SETTINGUI_INFO("------[CDataSave::NormalSave] checkID[%s], checkValue[%s], filterItemValue[%s] =======\n"
                               , strCheckID.c_str(), pInputItemData->GetCheckBaseValue().c_str()
                               , pFilterItemData->GetValue().c_str());
            }
#endif
            if (NULL != pFilterItemData
                    && 0 != pInputItemData->GetCheckBaseValue().compare(pFilterItemData->GetValue()))
            {
                continue;
            }
        }

        bool bItemValueChange = false;
        yl::string strValue = pItemData->GetValue();
        SaveItemData(pPageInfo, pItemData, bItemValueChange, false, iLineID);

        if (bItemValueChange)
        {
            bValueChanged = true;

            LIST_SETTING_MSG_DATA listMsgData = pItemData->GetListMessageData();
            LIST_SETTING_MSG_DATA::iterator iterMsg = listMsgData.begin();
            for (; iterMsg != listMsgData.end(); ++iterMsg)
            {
                CSettingUIMessageData * pItemMsg = *iterMsg;
                if (NULL == pItemMsg)
                {
                    continue;
                }
                CSettingUIMessageData * pMsgInfo = new CSettingUIMessageData(*pItemMsg);
                if (NULL == pMsgInfo)
                {
                    return false;
                }

                if (!pMsgInfo->GetWparamID().empty())
                {
                    for (int iWid = 0; iWid < nTotal; ++iWid)
                    {
                        CSettingUIItemDataBasePtr pItem = pPageInfo->GetItemByDataIndex(iWid);
                        if (NULL != pItem && pItem->GetItemID() == pMsgInfo->GetWparamID())
                        {
                            pMsgInfo->SetwParam(atoi(strValue.c_str()));
                            break;
                        }
                    }
                }

                if (!pMsgInfo->GetLparamID().empty())
                {
                    for (int iLid = 0; iLid < nTotal; ++iLid)
                    {
                        CSettingUIItemDataBasePtr pItem = pPageInfo->GetItemByDataIndex(iLid);
                        if (NULL != pItem && pItem->GetItemID() == pMsgInfo->GetLparamID())
                        {
                            pMsgInfo->SetlParam(atoi(strValue.c_str()));
                            break;
                        }
                    }
                }

                if (pMsgInfo->GetVarDataID().empty() && MESSAGE_METHOD_NOTIFYAPPEX == pMsgInfo->GetSendMethod())
                {
                    pMsgInfo->SetVarDataID(pItemData->GetItemID());
                }

                msgList.push_back(pMsgInfo);
            }
        }
    }

    //发消息
    if (msgList.size() > 0 && bValueChanged)
    {
        Sort(msgList, Compare);
        LIST_SETTING_MSG_DATA::iterator iterInfo = msgList.begin();
        for (; iterInfo != msgList.end(); ++iterInfo)
        {
            CSettingUIMessageDataPtr pMsgInfo = *iterInfo;
            if (NULL == pMsgInfo)
            {
                continue;
            }

            if ("lineid" == pMsgInfo->GetLparamID())
            {
                pMsgInfo->SetlParam(iLineID);
            }
            else
            {
                CSettingUIItemDataBasePtr pItem = pPageInfo->GetItemByItemId(pMsgInfo->GetLparamID());
                if (NULL != pItem)
                {
                    pMsgInfo->SetlParam(atoi(pItem->GetValue().c_str()));
                }
            }

            if ("lineid" == pMsgInfo->GetWparamID())
            {
                pMsgInfo->SetwParam(iLineID);
            }
            else
            {
                CSettingUIItemDataBasePtr pItem = pPageInfo->GetItemByItemId(pMsgInfo->GetWparamID());
                if (NULL != pItem)
                {
                    pMsgInfo->SetwParam(atoi(pItem->GetValue().c_str()));
                }
            }

            ProcessMessage(pMsgInfo, pPageInfo);
        }
    }

    if (pPageInfo->IsNetworkChangedReboot() && bValueChanged)
    {
        //设置网络改变标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }

    return true;
}

bool CDataSave::SaveItemData(CSettingUIPageDataBase * pPageInfo, CSettingUIItemDataBase * pItemData,
                             bool & bChange
                             , bool bSendMsg /*= true*/, int nLineID /*= -1*/)
{
    if (NULL == pItemData || pItemData->GetConfigID().empty()
            || NULL == pPageInfo)
    {
        return false;
    }

    yl::string strConfigID = pItemData->GetConfigID();
    yl::string strValue = pItemData->GetValue();
    yl::string strItemID = pItemData->GetItemID();
    int nTotal = pPageInfo->GetTotalItemNumber();
    // 查找M7串中'%d'出现次数
    yl::string strM7Config = configParser_GetCfgAttribute(strConfigID.c_str(), "M7");
    int nFirstPos = strM7Config.find("%d", 0);
    int nSecondPos = (yl::string::npos != nFirstPos) ? strM7Config.find("%d",
                     nFirstPos + 2) : (yl::string::npos);
#if IF_SETTING_CONFIG_LOAD_SAVE_DEBUG
    SETTINGUI_INFO("--------- nFirst[%d], nSecond[%d] ===="
                   , nFirstPos, nSecondPos);
#endif

    if (WIDGET_VALUE_TYPE_INT == pItemData->GetValueType())
    {
        strValue = commonAPI_FormatString("%d", atoi(strValue.c_str()));
    }

    yl::string strPreValue = "";
    if (nLineID < 0)
    {
        int iPos = strConfigID.find("%d");
        if (iPos != yl::string::npos)
        {
            yl::string strRemain =  strConfigID.substr(iPos + 2, strConfigID.length() - iPos - 2);
            if (!strRemain.empty() && strRemain.find("%d") != yl::string::npos)
            {
                strPreValue = g_DataLoader->LoadDoubleParamconfig(strConfigID, nLineID);
                SaveDoubleParamconfig(strConfigID, nLineID, strItemID, strValue);
            }
            else
            {
                SaveSingleParamconfig(strConfigID, nLineID, strItemID, strValue, strPreValue);
            }
        }
        else
        {
            strPreValue = configParser_GetConfigString(strConfigID.c_str());
            if (strPreValue != strValue)
            {
                configParser_SetConfigString(strConfigID.c_str(), strValue.c_str(), CONFIG_LEVEL_PHONE);
            }
        }
    }
    else
    {
        int iPos = strConfigID.find("%d");
        if (iPos != yl::string::npos)
        {
            yl::string strRemain =  strConfigID.substr(iPos + 2, strConfigID.length() - iPos - 2);
            if (!strRemain.empty() && strRemain.find("%d") != yl::string::npos)
            {
                strPreValue = g_DataLoader->LoadDoubleParamconfig(strConfigID, nLineID);
                SaveDoubleParamconfig(strConfigID, nLineID, strItemID, strValue);
            }
            else
            {
                strPreValue = configParser_GetCfgItemStringValue(strConfigID.c_str(), nLineID);
                configParser_SetCfgItemStringValue(strConfigID.c_str(), nLineID, strValue.c_str(),
                                                   CONFIG_LEVEL_PHONE);
            }
        }
        else if (nSecondPos != yl::string::npos)
        {
            strPreValue = g_DataLoader->LoadDoubleParamconfig(strConfigID, nLineID);
            SaveDoubleParamconfig(strConfigID, nLineID, strItemID, strValue);
        }
        else
        {
            SaveSingleParamconfig(strConfigID, nLineID, strItemID, strValue, strPreValue);
        }
    }

    if (strPreValue != strValue)
    {
#if IF_SETTING_CONFIG_LOAD_SAVE_DEBUG
        printf("------------ [CDataSave::SaveItemData] pre[%s], cur[%s] ========\n"
               , strPreValue.c_str(), strValue.c_str());
#endif
        bChange = true;
        if (bSendMsg)
        {
            LIST_SETTING_MSG_DATA listMsgData = pItemData->GetListMessageData();
            LIST_SETTING_MSG_DATA::iterator iterMsg = listMsgData.begin();
            for (; iterMsg != listMsgData.end(); ++iterMsg)
            {
                CSettingUIMessageData * pItemMsg = *iterMsg;
                if (NULL == pItemMsg)
                {
                    continue;
                }
                CSettingUIMessageData * pMsgInfo = new CSettingUIMessageData(*pItemMsg);
                if (NULL == pMsgInfo)
                {
                    return false;
                }

                if (!pMsgInfo->GetWparamID().empty())
                {
                    for (int iWid = 0; iWid < nTotal; ++iWid)
                    {
                        CSettingUIItemDataBasePtr pItem = pPageInfo->GetItemByDataIndex(iWid);
                        if (NULL != pItem && pItem->GetItemID() == pMsgInfo->GetWparamID())
                        {
                            pMsgInfo->SetwParam(atoi(strValue.c_str()));

                            if ("lineid" == pMsgInfo->GetLparamID())
                            {
                                pMsgInfo->SetlParam(nLineID);
                            }

                            if ("lineid" == pMsgInfo->GetWparamID())
                            {
                                pMsgInfo->SetwParam(nLineID);
                            }

                            ProcessMessage(pMsgInfo, pPageInfo);
                            break;
                        }
                    }
                }

                if (!pMsgInfo->GetLparamID().empty())
                {
                    for (int iLid = 0; iLid < nTotal; ++iLid)
                    {
                        CSettingUIItemDataBasePtr pItem = pPageInfo->GetItemByDataIndex(iLid);
                        if (NULL != pItem && pItem->GetItemID() == pMsgInfo->GetLparamID())
                        {
                            pMsgInfo->SetlParam(atoi(strValue.c_str()));

                            if ("lineid" == pMsgInfo->GetLparamID())
                            {
                                pMsgInfo->SetlParam(nLineID);
                            }

                            if ("lineid" == pMsgInfo->GetWparamID())
                            {
                                pMsgInfo->SetwParam(nLineID);
                            }

                            ProcessMessage(pMsgInfo, pPageInfo);
                            break;
                        }
                    }
                }
            }
        }

        if (pItemData->IsConfigChangedReboot())
        {
            //设置网络改变标记
            _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
        }
    }

    return true;
}

void GetExtraValue(CSettingUIPageDataBase * pPageInfo, yl::string & strExValue,
                   const yl::string & strVarDataID)
{
    if (NULL == pPageInfo)
    {
        return;
    }

    int nTotal = pPageInfo->GetTotalItemNumber();
    for (int iExtValue = 0; iExtValue < nTotal; ++iExtValue)
    {
        CSettingUIItemDataBasePtr pItemData = pPageInfo->GetItemByDataIndex(iExtValue);
        if (NULL != pItemData && pItemData->GetItemID() == strVarDataID)
        {
            strExValue = pItemData->GetValue();
            return;
        }
    }
}

void CDataSave::ProcessMessage(CSettingUIMessageData * msgParm, CSettingUIPageDataBase * pPageInfo)
{
    if (NULL == msgParm || NULL == pPageInfo)
    {
        return;
    }

    yl::string strDstThread = msgParm->GetDstThread();
    yl::string strVarDataID = msgParm->GetVarDataID();
    int iMsgID = msgParm->GetMsgID();
    int lParm = msgParm->GetlParam();
    int wParm = msgParm->GetwParam();

    yl::string strThread = strDstThread.empty() ? VP_AP_THREAD_NAME : strDstThread;
    DSK_TARGET_T dstThread = msgGetThreadByName(strThread.c_str());

    switch (msgParm->GetSendMethod())
    {
    case MESSAGE_METHOD_SEND:
        {
            if (strVarDataID.empty())
            {
                msgSendMsgToThread(dstThread, iMsgID, wParm, lParm);
            }
            else
            {
                yl::string strExValue = "";
                GetExtraValue(pPageInfo, strExValue, strVarDataID);

                msgSendMsgToThreadEx(dstThread, iMsgID, wParm, lParm, strExValue.length(),
                                     (void *)(strExValue.c_str()));
            }
        }
        break;
    case MESSAGE_METHOD_POST_THREAD:
        {
            msgPostMsgToThread(dstThread, iMsgID, wParm, lParm);
        }
        break;
    case MESSAGE_METHOD_BROADCAST:
        {
            if (strVarDataID.empty())
            {
                msgBroadpostThreadMsgEx(iMsgID, wParm, lParm, 0, NULL);
            }
            else
            {
                yl::string strExValue = "";
                GetExtraValue(pPageInfo, strExValue, strVarDataID);

                msgBroadpostThreadMsgEx(iMsgID, wParm, lParm, strExValue.length(),
                                        (void *)(strExValue.c_str()));
            }
        }
        break;
    case MESSAGE_METHOD_NOTIFYAPP:
        {
            etl_NotifyApp(false, iMsgID, wParm, lParm);
            SETTINGUI_INFO("Save config files, Send Messagex, ItemID[%s], MsgId[%d], Wparam[%d], Lparam[%d]",
                           strVarDataID.c_str(), iMsgID, wParm, lParm);
        }
        break;
    case MESSAGE_METHOD_NOTIFYAPPEX:
        {
            yl::string strExValue = "";
            GetExtraValue(pPageInfo, strExValue, strVarDataID);

            etl_NotifyAppEx(false, iMsgID, wParm, lParm, strExValue.length(), strExValue.c_str());
        }
        break;
    }
}


#define ACCOUNT_SERVER_PARAM1 0
#define ACCOUNT_SERVER_PARAM2 1

void CDataSave::SaveDoubleParamconfig(const yl::string & strConfigId, const int iLineID,
                                      const yl::string & strWgtID, const yl::string & strValue)
{
    configParser_SetCfgItemStringValue(strConfigId.c_str(), iLineID, ACCOUNT_SERVER_PARAM1, strValue,
                                       CONFIG_LEVEL_PHONE);
}

void CDataSave::SaveSingleParamconfig(const yl::string & strConfigId, const int iLineID,
                                      const yl::string & strItemID
                                      , const yl::string & strValue, yl::string & strPreValue)
{
#if IF_SETTING_CONFIG_LOAD_SAVE_DEBUG
    SETTINGUI_INFO("-=-=-==-=-= [CDataSave::SaveSingleParamconfig] config[%s], lineID[%d], itemid[%s], value[%s] ========="
                   , strConfigId.c_str(), iLineID, strItemID.c_str(), strValue.c_str());
#endif
    if (iLineID >= 0)
    {
        strPreValue = configParser_GetCfgItemStringValue(strConfigId.c_str(), iLineID);
        configParser_SetCfgItemStringValue(strConfigId.c_str(), iLineID, strValue.c_str(),
                                           CONFIG_LEVEL_PHONE);
    }
    else
    {
        if (0 == strItemID.compare("h323_gatekeeper_server1")
                || 0 == strItemID.compare("h323_gatekeeper_port1"))
        {
            strPreValue = configParser_GetCfgItemStringValue(strConfigId.c_str(), ACCOUNT_SERVER_PARAM1);
            configParser_SetCfgItemStringValue(strConfigId.c_str(), ACCOUNT_SERVER_PARAM1, strValue.c_str(),
                                               CONFIG_LEVEL_PHONE);
        }
        else
        {
            strPreValue = configParser_GetCfgItemStringValue(strConfigId.c_str(), ACCOUNT_SERVER_PARAM2);
            configParser_SetCfgItemStringValue(strConfigId.c_str(), ACCOUNT_SERVER_PARAM2, strValue.c_str(),
                                               CONFIG_LEVEL_PHONE);
        }
    }
}

