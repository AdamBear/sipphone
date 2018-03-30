#include "../include/moddsskey.h"
#include "dsskeydataentrymanager.h"
#include "msgprocessor.h"
#include "dsskeydata.h"
#include "dsskeycommonfunc.h"

#include "dsskeyactionmanager.h"
#include "dsskeyaction.h"
#include "dsskeydatamodule.h"

#include <talk/talklogic/include/uiandlogic_common.h>
#include <account/include/modaccount.h>
#include <account/include/account_datadefine.h>
#include <configiddefine.h>
#include <configparser/modconfigparser.h>

#include "dsskey_common.h"
#include "dsskeyurlaction.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "devicelib/phonedevice.h"
#include "etlmsghandler/modetlmsghandler.h"

#include "common/translatehelper.h"
#include <ylstl/ylstringalgorithm.h>
#include "keyguard/include/modkeyguard.h"
#include "adapterbox/include/modadapterbox.h"
#include "setting_page_define.h"
#include "edk/include/modedk.h"
#include "dsskeyuilogic/include/moddsskeyuilogic.h"
#include "exp/include/modexp.h"
#include "talk/broadsoftsca/include/modbroadsoft_sca.h"

dsskey_callback ui_callbackfun = NULL;

// Dsskey模块初始化
bool dsskey_Init()
{
    pathCreateDirectory(DSSKEY_XML_FILE_PATH);

    RegisterMsgHandle();

    dsskeyuilogic_Init();

    _DsskeyManager.Init();

    new CDSSkeyURLAction();
    return true;
}

//程序结束时释放所有资源
bool dsskey_UnInit()
{
	exp_UnInitExp();
    UnRegisterMsgHandle();

    return true;
}

//按键处理
bool dsskey_OnDsskeyClick(int iDsskeyID, int iDssKeyIndex /* = 0*/,
                          bool bCheckNeedPassword/* = true*/)
{
    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(iDsskeyID);
    if (NULL == pData)
    {
        return false;
    }

    DssKey_Type eType = pData->GetDsskeyType();

    // 判断该dsskey的类型，其进入的界面是否是键盘锁允许的
    // 此处的判断需要想办法改？键盘锁涉及业务
    if (keyGuard_IsEnable() && keyGuard_IsLock())
    {
        switch (keyGuard_GetKeyGuardType())
        {
        case KG_ALL_KEYS:
        case KG_FUN_KEY:
            {
                if (DT_LINE != eType)
                {
                    // 强行测试是否允许dsskey，弹出解锁框
                    keyGuard_IsEnableModuld(KGM_MODULE_DSSKEY);
                    return false;
                }
            }
            break;
        case KG_MENU_KEY:
            {
                if (DT_IDLE_MENU == eType
                        || DT_KEYPAD_LOCK == eType)
                {
#if IF_BUG_21722
                    if (KG_MENU_KEY == keyGuard_GetKeyGuardType())
                    {
                        AdapterBox_SettingUIEnterPage(MENU_PAGE_MAIN_LOCK);
                        return true;
                    }
#endif
                    keyGuard_IsEnableModuld(KGM_MODULE_SETTING);
                    return false;
                }
            }
            break;
        default:
            break;
        }
    }

    if (bCheckNeedPassword
            && dsskeyuilogic_IsNeedPassword(eType))
    {
        AdapterBox_ShowDsskeyLockPassword(iDsskeyID);
        DSSKEY_INFO("dsskey[%d] eType[%d], Lock need password", iDsskeyID, eType);
        return true;
    }
    DSSKEY_INFO("dsskey[%d], onclick!", eType);

    CDSSKeyAction * pAction = CDSSKeyActionManger::GetInstance()->GetDSSKeyAction(eType);
    if (pAction)
    {
        DssKeyLogicData logicData ;
        pData->GetLogicData(logicData);

        // 不响应NA类型的dsskey
        if (logicData.eDKType <= DT_NA)
        {
            DSSKEY_INFO("dsskey id [%d] is type NA", pData->GetDsskeyDataID());
            return false;
        }

#if IF_BUG_30958
        if (DT_TIPTEL_BUTTON == pData->GetDsskeyType()
                && DMT_LINEKEY_MODULE == logicData.eType)
        {
            return pAction->OnKeyPress(iDssKeyIndex, logicData,
                                       NULL); //DT_TIPTEL_BUTTON类型，传物理按键index
        }
        else
#endif
        {
            return pAction->OnKeyPress(logicData.iDssKeyID, logicData, NULL);
        }

    }

    return false;
}

bool dsskey_OnDssKeyLongClick(int iDsskeyID)
{
    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(iDsskeyID);
    if (pData)
    {


        CDSSKeyAction * pAction = CDSSKeyActionManger::GetInstance()->GetDSSKeyAction(
                                      pData->GetDsskeyType());
        if (pAction)
        {
            DssKeyLogicData logicData ;
            pData->GetLogicData(logicData);
            return pAction->OnKeyLongPress(pData->GetDsskeyDataID(), logicData, NULL);
        }
    }
    return false;
}


// 更新dsskey状态
bool dsskey_UpdateStatus(int iDssKeyID, Dsskey_Status eStatus, Dsskey_Status eStart,
                         Dsskey_Status eEnd, bool bUpdate/* = true*/)
{
    CDSSkeyData * pDsskey = _DsskeyManager.GetDsskeyDataByID(iDssKeyID);
    if (NULL == pDsskey || pDsskey->GetDsskeyType() == DT_NA)
    {
        //DSSKEY_INFO("Update exp [%d] key [%d] failed", tDsskeyInfo.ExpIndex(), tDsskeyInfo.KeyIndex());
        return false;
    }

    pDsskey->SetLineStatus(eStatus, eStart, eEnd);
    if (bUpdate)//状态设置被设置后，需要更新状态
    {
        dsskey_refreshDSSKeyUI(iDssKeyID);
    }
    return true;
}

#ifdef IF_FEATURE_MULTIFWD
// 是否Linekey类型的按键
bool dsskey_IsLineKey(int iDssKeyID)
{
    return DMT_LINEKEY_MODULE == GetModuleType(iDssKeyID);
}

#endif

bool dsskey_IsExpModule(DSSKEY_MODULE_TYPE eModuleType)
{
    return IsEXPModule(eModuleType);
}

bool dsskey_IsSupportModuleType(DSSKEY_MODULE_TYPE eModuleType)
{
    return IsSupportModuleType(eModuleType);
}

//===========================================================================
//获取DssKey属性的状态
bool dsskey_GetData(DSSKEY_MODULE_TYPE eType, int iBeginKey, int iNum,
                    YLList<DssKeyLogicData> & listData, int iEXPIndex /*= 0*/)
{
    return _DsskeyManager.GetDataForUI(eType, iBeginKey, iNum, listData, iEXPIndex);
}

//获取DssKey属性的状态
bool dsskey_GetData(int iDsskeyID, DssKeyLogicData & sKeyData)
{
    return _DsskeyManager.GetDataForUI(iDsskeyID, sKeyData);
}

// 更新DND状态
void dsskey_UpdateDNDStatus(bool bDndOn)
{
    Dsskey_Status eStatus = DS_NON;
    if (bDndOn)
    {
        eStatus = DS_DND_USING;
    }

    _DsskeyManager.UpdateStatus(DT_DND, eStatus);
}

// 更新FWD状态
void dsskey_UpdateFWDStatus(int dsskeyID, bool bFwdOn)
{
    Dsskey_Status eStatus = DS_NON;
    if (bFwdOn)
    {
        eStatus = DS_FWD_USING;
    }
    _DsskeyManager.UpdateStatus(_DsskeyManager.GetDsskeyDataByID(dsskeyID), eStatus);
}

#ifdef IF_FEATURE_MULTIFWD
// 更新FWD状态
void dsskey_UpdateFWDStatus(yl::string & strTarget, bool bFwdOn)
{
    Dsskey_Status eStatus = DS_NON;
    _DsskeyManager.UpdateStatus(DT_FWD, eStatus);
    if (bFwdOn)
    {
        eStatus = DS_FWD_USING;
        _DsskeyManager.UpdateStatus(DT_FWD, eStatus, strTarget);
    }
}
#endif

// 是否为SCA类型
bool dsskey_IsSCATypeLinekey(int dsskeyID)
{
    CDSSkeyData * pDsskey = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pDsskey == NULL || pDsskey ->GetDsskeyType() != DT_LINE)
    {
        return false;
    }

    return acc_IsBLAEnable(pDsskey->GetRealLine());
}

// 当DND或FWD状态有改变时更新Dsskey
bool dsskey_UpdateStatusByDNDOrFWDChanged(int iAccount /*= -1*/)
{
    // 更新Linekey的显示
    _DsskeyManager.UpdateLinekeyLabel(iAccount);

    // 更新exp的显示
    if (exp_GetExpColor())
    {
        for (int iIndex = 0; iIndex < exp_GetEXPNum(); iIndex ++)
        {
            _DsskeyManager.UpdateLinekeyLabel(iAccount, exp_GetEXPModuleType(), iIndex);
        }
    }

    return true;
}

//设置linekey的callpark状态
void dsskey_ProcessCallParkNotify(int iAccountID, bool bOn)
{
    _DsskeyManager.ProcessCallParkNotify(iAccountID, bOn);
}



// 设置DSSKey数据
bool dsskey_SetDsskeyData(int dsskeyID, const DssKeyLogicData & dsskeyLogicData,
                          int iFlags/* = DSSKEY_SET_NORMAL*/)
{
    CDSSKeyDataModule * pModule = _DsskeyManager.GetModuleByDsskeyID(dsskeyID);

    if (pModule)
    {
        return pModule->SetDsskeyData(dsskeyID, dsskeyLogicData, iFlags);
    }
    return false;
}

bool dsskey_GetDsskeyStatus(int dsskeyID, YLList<Dsskey_Status> & listOutput)
{
    CDSSkeyData * pDsskey = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pDsskey)
    {
        pDsskey->GetStatusList(listOutput);
        return true;
    }
    return false;
}

Dsskey_Status dsskey_GetDsskeyStatus(int dsskeyID)
{
    CDSSkeyData * pDsskey = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pDsskey)
    {
        return pDsskey->GetStatus();
    }
    return DS_NON;
}

bool dsskey_GetAllDsskeyDataByModule(YLList<int> & listOutput, DSSKEY_MODULE_TYPE moduleType,
                                     int iModuleIndex /*= 0*/, DssKey_Type eType/* = DT_NONE*/)
{
    CDSSKeyDataModule * pModule = _DsskeyManager.GetDssKeyModule(moduleType, iModuleIndex);
    if (pModule)
    {
        for (int i = 0; i < pModule->size(); i ++)
        {
            CDSSkeyData * pDsskeyData = pModule->GetDsskeyDataByIndex(i);
            if (pDsskeyData)
            {
                if (eType != DT_NONE && pDsskeyData->GetDsskeyType() != eType)
                {
                    continue;
                }
                listOutput.push_back(pDsskeyData->GetDsskeyDataID());
            }
        }
    }
    return listOutput.size() > 0;
}

int dsskey_GetModulesAmount()
{
    return _DsskeyManager.GetModulesSize();
}

DssKey_Type dsskey_GetDsskeyType(int dsskeyID, int iFlag/* = DSS_ATTR_NONE*/)
{
    if (iFlag != DSS_ATTR_NONE)
    {
        return (DssKey_Type)CDSSkeyData::GetIntConfig(dsskeyID, ATTR_TYPE, iFlag == DSS_ATTR_FACTORY ? CONFIG_LEVEL_FACTORY : CONFIG_LEVEL_IGNORE);
    }

    CDSSkeyData * pDsskey = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pDsskey)
    {
        return pDsskey->GetDsskeyType();
    }
    return DT_NONE;
}

void dsskey_GetModulesList(YLList<int> & listOutput)
{
    _DsskeyManager.GetModulesList(listOutput);
}

bool dsskey_ReloadDSSKeyData(DSSKEY_MODULE_TYPE moduleType, int iModuleIndex /*= 0*/)
{
    int nModuleID = GetModuleIDByModuleIndex(moduleType, iModuleIndex);

    CDSSKeyDataModule * pModule = _DsskeyManager.GetModuleByModuleID(nModuleID);
    if (pModule)
    {
        return pModule->LoadFromMemory();
    }

    return false;
}

int dsskey_GetUnboundDsskeyByAccountID(int accountID)
{
    CDSSKeyDataModule * pModule = _DsskeyManager.GetDssKeyModule(DMT_LINEKEY_MODULE);
    if (pModule)
    {
        return pModule->GetUnboundDsskeyByAccountID(accountID);
    }
    return -1;
}

yl::string  dsskey_GetDsskeyValue(int dsskeyID, int iFlag/* = DSS_ATTR_NONE*/)
{
    if (iFlag != DSS_ATTR_NONE)
    {
        return CDSSkeyData::GetStringConfig(dsskeyID, ATTR_VALUE, iFlag == DSS_ATTR_FACTORY ? CONFIG_LEVEL_FACTORY : CONFIG_LEVEL_IGNORE);
    }

    CDSSkeyData * pDsskey = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pDsskey)
    {
        yl::string strOutput;
        pDsskey->GetValue(strOutput);
        return strOutput;
    }
    return "";
}

bool dsskey_IsBindType(DssKey_Type dsskeyType)
{
    CDSSKeyAction * pAction = CDSSKeyActionManger::GetInstance()->GetDSSKeyAction(dsskeyType);
    if (pAction)
    {
        return pAction->IsBindType();
    }
    return false;
}

//获取呼出号码
bool dsskey_GetCallOutNumber(int nDsskeyID, yl::string & strCallOutNumber,
                             void * pExtData/* = NULL*/,  int nExtType/* = 0*/)
{
    CDSSkeyData * pDsskey = _DsskeyManager.GetDsskeyDataByID(nDsskeyID);
    if (pDsskey)
    {
        CDSSKeyAction * pAction = CDSSKeyActionManger::GetInstance()->GetDSSKeyAction(
                                      pDsskey->GetDsskeyType());
        if (pAction)
        {
            DssKeyLogicData logicData ;
            pDsskey->GetLogicData(logicData);
            return pAction->GetCallOutNumber(pDsskey->GetDsskeyDataID(), strCallOutNumber, logicData, pExtData,
                                             nExtType);
        }

    }
    return false;
}

int dsskey_GetDsskeyID(DSSKEY_MODULE_TYPE moduleType, int iModuleIndex, int keyIndex)
{
    return GetDsskeyDataID(moduleType, iModuleIndex, keyIndex);
}

bool dsskey_RefressDsskeyByType(DssKey_Type dsskeyType)
{
    _DsskeyManager.RefreshDsskeyStatusByType(dsskeyType);
    return false;
}

// 移除状态
bool dsskey_RemoveStatus(int iDsskeyID, Dsskey_Status eStart, Dsskey_Status eEnd,
                         bool bUpdate /* = true*/)
{
    CDSSkeyData * pDsskey = _DsskeyManager.GetDsskeyDataByID(iDsskeyID);
    if (NULL == pDsskey || pDsskey->GetDsskeyType() == DT_NA)
    {
        //DSSKEY_INFO("Update exp [%d] key [%d] failed", tDsskeyInfo.ExpIndex(), tDsskeyInfo.KeyIndex());
        return false;
    }

    pDsskey->RemoveLineStatus(eStart, eEnd);

    if (bUpdate)//状态设置被设置后，需要更新状态
    {
        dsskey_refreshDSSKeyUI(iDsskeyID);
    }
    return true;
}


int dsskey_GetDsskeyLine(int nDsskeyID, int iFlag/* = DSS_ATTR_NONE*/)
{
    if (iFlag != DSS_ATTR_NONE)
    {
        return CDSSkeyData::GetIntConfig(nDsskeyID, ATTR_LINE, iFlag == DSS_ATTR_FACTORY ? CONFIG_LEVEL_FACTORY : CONFIG_LEVEL_IGNORE);
    }

    CDSSkeyData * pDsskey = _DsskeyManager.GetDsskeyDataByID(nDsskeyID);
    if (pDsskey)
    {
        if (pDsskey->GetDsskeyType() == DT_LINE)
        {
            return pDsskey->GetRealLine();
        }
        else
        {
            return pDsskey->GetLineIndex();
        }

    }
    return -1;
}

bool dsskey_setUpdateFunCallback(dsskey_callback callback)
{
    ui_callbackfun = callback;
    return false;
}

bool dsskey_getProgrameKeyList(YLList<int> & listOutput, int getSize)
{
    YLList<int> listTemp;
    dsskey_GetAllDsskeyDataByModule(listTemp, DMT_PROGRAMEKEY_MODULE, 0);
    if (getSize < listTemp.size())
    {
        YLList<int>::ListIterator iter = listTemp.begin();
        int i = 0;
        for (; iter != listTemp.end(), i < getSize; iter++, i++)
        {
            listOutput.push_back(*iter);
        }
    }
    else
    {
        listOutput = listTemp;
    }
    return true;
}

bool dsskey_refreshDSSKeyUI(int dsskeyID, DsskeyExpRefresh dsskeyRefresh/* = DSS_KEY_REFRESH_ALL*/,
                            bool bCheckData/* = true*/)
{
    CDSSkeyData * pDsskey = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (NULL == pDsskey)
    {
        return false;
    }
    //DSSKEY_INFO("Refresh Dsskey[%d].", dsskeyID);

    if (bCheckData)
    {
        CDSSKeyAction * pAction = CDSSKeyActionManger::GetInstance()->GetDSSKeyAction(
                                      pDsskey->GetDsskeyType());
        if (pAction)
        {
            if (!pAction->BeforeUpdateUI(dsskeyID, dsskeyRefresh))
            {
                return false;
            }
        }
    }

    if (ui_callbackfun)
    {
        return ui_callbackfun(dsskeyID, pDsskey->GetStatus(), NULL, &dsskeyRefresh);
    }

    return false;
}

bool dsskey_getAllDsskeys(YLList<int> & listOutput, DSSKEY_MODULE_TYPE moduleType)
{
    return _DsskeyManager.GetAllDsskeyByType(listOutput, moduleType);
}

bool dsskey_getAllDsskeys(YLList<int> & listOutput)
{
    return _DsskeyManager.GetAllDsskeys(listOutput, DT_NONE, false);
}

bool dsskey_getAllDsskeys(YLList<int> & listOutput, DssKey_Type eType, bool bNative/* = false*/)
{
    return _DsskeyManager.GetAllDsskeys(listOutput, eType, bNative);
}

bool dsskey_GetPageDsskeys(YLList<int> & listOutput, DSSKEY_MODULE_TYPE moduleType,
                           int iModuleIndex /*= 0*/, int iPageNum /*= 0*/)
{
    CDSSKeyDataModule * pModule = _DsskeyManager.GetDssKeyModule(moduleType, iModuleIndex);
    if (pModule)
    {
        int iStartIndex, iEndIndex;
        dsskey_GetPageInfoByMoudle(iStartIndex, iEndIndex, moduleType, iModuleIndex, iPageNum);

        for (int i = iStartIndex; i < iEndIndex; i++)
        {
            CDSSkeyData * pDsskeyData = pModule->GetDsskeyDataByIndex(i);
            if (pDsskeyData)
            {
                listOutput.push_back(pDsskeyData->GetDsskeyDataID());
            }
        }
        return listOutput.size() > 0;
    }
    return false;
}

int dsskey_GetPageCountByMoudle(DSSKEY_MODULE_TYPE moduleType, int iModuleIndex /*= 0*/)
{
    if (!dsskey_IsSupportModuleType())
    {
        return 0;
    }

    // 获取分页页数
    switch (moduleType)
    {
    case DMT_LINEKEY_MODULE:
        {
            PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
            if (ePhoneType == PT_T19 || (DEVICELIB_IS_CP(ePhoneType)) || PT_DECT == ePhoneType)
            {
                return 0;
            }
            else if (DEVICELIB_IS_T23(ePhoneType) || PT_T61 == ePhoneType)
            {
                return 1;
            }
            else if (DEVICELIB_IS_T42(devicelib_GetPhoneType())
                     || DEVICELIB_IS_T27(devicelib_GetPhoneType())
                     || DEVICELIB_IS_T46(devicelib_GetPhoneType())
                     || DEVICELIB_IS_T69(devicelib_GetPhoneType())
                     || PT_T68 == ePhoneType
                     || PT_T29 == ePhoneType
                     || PT_T52 == ePhoneType
                     || PT_T54S == ePhoneType
                    )
            {
                return 3;
            }
            else if (PT_T49 == ePhoneType || DEVICELIB_IS_T48(devicelib_GetPhoneType()))
            {
                return 2;
            }

            return 1;
        }
    case DMT_MEMKEY_MODULE:
        return 0;
    case DMT_PROGRAMEKEY_MODULE:
        return 1;
    case DMT_EXP20_MODULE:
    case DMT_EXP38_MODULE:
    case DMT_EXP39_MODULE:
    case DMT_EXP40_MODULE:
    case DMT_EXPCM69_MODULE:
        return 2;
    case DMT_EXP50_MODULE:
        return 3;
    default:
        return 0;
    }
}

bool dsskey_IsSectionAllSameType(int iFrom, int iTo, DssKey_Type eType, DSSKEY_MODULE_TYPE moduleType, int iModuleIndex /*= 0*/)
{
    if (iFrom > iTo)
    {
        return false;
    }

    CDSSKeyDataModule * pModule = _DsskeyManager.GetDssKeyModule(moduleType, iModuleIndex);
    if (pModule)
    {
        int iSize = pModule->size();
        if (iTo >= iSize)
        {
            iTo = iSize - 1;

            if (iFrom > iTo)
            {
                return false;
            }
        }

        while (iFrom <= iTo)
        {
            CDSSkeyData* pDsskey = pModule->GetDsskeyDataByIndex(iFrom);
            if (pDsskey == NULL)
            {
                return false;
            }

            if (pDsskey->GetDsskeyType() != eType)
            {
                return false;
            }

            ++iFrom;
        }

        return true;
    }

    return false;
}

bool dsskey_IsAverageDistribution(DSSKEY_MODULE_TYPE moduleType, int iModuleIndex /*= 0*/)
{
    if (moduleType == DMT_LINEKEY_MODULE)
    {
        PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
        return !(DEVICELIB_IS_T48(devicelib_GetPhoneType()) || PT_T49 == ePhoneType);
    }

    return true;
}

int dsskey_GetPageInfoByMoudle(int & iStartIndex, int & iEndIndex, DSSKEY_MODULE_TYPE moduleType,
                               int iModuleIndex /*= 0*/, int iPageNum /*= 0*/)
{
    iStartIndex = iEndIndex = 0;

    CDSSKeyDataModule * pModule = _DsskeyManager.GetDssKeyModule(moduleType, iModuleIndex);
    if (pModule)
    {
        // 获取分页数
        int iPageCount = dsskey_GetPageCountByMoudle(moduleType, iModuleIndex);
        if (iPageCount <= 0)
        {
            return 0;
        }

        int iMoudleCount = pModule->size();
        // 只有一页的情况
        if (iPageCount == 1)
        {
            iEndIndex = iMoudleCount;
        }
        else
        {
            int iPrePageDsskeyNum = iMoudleCount / iPageCount;

            bool bAverage = dsskey_IsAverageDistribution(moduleType, iModuleIndex);
            // 非平均分布
            if (!bAverage)
            {
                iPrePageDsskeyNum = dsskey_GetMaxDsskeyNumPerPage();
            }

            iStartIndex = iPrePageDsskeyNum * iPageNum;
            // 类似T48\T49机型，最后一页个数不一样
            iEndIndex = (iPageNum == iPageCount - 1) ? iMoudleCount : (iPrePageDsskeyNum * (iPageNum + 1));

            if (bAverage && moduleType == DMT_LINEKEY_MODULE)
            {
                int* pFromIndex = NULL;
                // 第一页，如果后面页数都没有可用dssskey，则应算上翻页键
                if (iPageNum == 0)
                {
                    pFromIndex = &iEndIndex;
                }
                // 第二页，如果当前页和之后页都没有可用dssskey，则应该去除翻页键（已经在第一页了）
                else if (iPageNum == 1 && iPageCount > 2)
                {
                    pFromIndex = &iStartIndex;
                }

                if (pFromIndex && dsskey_IsSectionAllSameType(*pFromIndex, iMoudleCount, DT_NA, moduleType, iModuleIndex))
                {
                    ++(*pFromIndex);
                }
            }
        }
    }

    return iEndIndex - iStartIndex;
}

int dsskey_GetMaxDsskeyNumPerPage()
{
    int iMaxNum = 0;
    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
    switch (ePhoneType)
    {
    case PT_T19:
    case PT_CP860:
    case PT_DECT:
    case PT_CP920:
        {
            iMaxNum = 0;
        }
        break;
    case PT_T21:
        {
            iMaxNum = 2;
        }
        break;
    case PT_T23:
    case PT_T40:
    case PT_T40G:
        {
            iMaxNum = 3;
        }
        break;
    case PT_T27:
    case PT_T27S:
    case PT_T66:
    case PT_T52:
        {
            iMaxNum = 8;
        }
        break;
    case PT_T29:
        {
            iMaxNum = 10;
        }
        break;
    case PT_T61:
    case PT_T41:
    case PT_T42:
    case PT_T43S:
    case PT_T43P:
        {
            iMaxNum = 6;
        }
        break;
    case PT_T46:
    case PT_T46S:
    case PT_T54S:
        {
            iMaxNum = 10;
        }
        break;
    case PT_T48:
    case PT_T48S:
    case PT_T49:
        {
            iMaxNum = 11;
        }
        break;
    default:
        break;
    }
    return iMaxNum;
}

bool dsskey_IsLinekeyHavePageDsskey(DssKey_Type eExceptType /*= DT_NA*/)
{
    bool bShowPageKey = false;
    int iMaxNumPerPage = dsskey_GetMaxDsskeyNumPerPage();
    YLList<int> listdsskey;
    dsskey_GetAllDsskeyDataByModule(listdsskey, DMT_LINEKEY_MODULE);
    YLList<int>::ListIterator iter;
    int index = 0;
    for (iter = listdsskey.begin(); iter != listdsskey.end(); ++iter, ++index)
    {
        int iDsskeyId = *iter;
        DssKeyLogicData objData;
        dsskey_GetData(iDsskeyId, objData);

        if (DT_LINE == objData.eDKType)
        {
            if (DS_LINE_NONE < objData.iStatus
                    && objData.iStatus < DS_LINE_END && DS_LINE_DISABLE != objData.iStatus)
            {
                if (index > iMaxNumPerPage)
                {
                    bShowPageKey = true;
                    break;
                }

                int iLock = atoi(objData.strValue.c_str());
                if (iLock == 2)
                {
                    bShowPageKey = true;
                    break;
                }
            }
        }
        else if (objData.eDKType != DT_NA
                 && objData.eDKType != eExceptType)
        {
            if (index > iMaxNumPerPage)
            {
                bShowPageKey = true;
                break;
            }
        }
    }
    return bShowPageKey;
}


int dsskey_GetDSSKeyAmount()
{
    return _DsskeyManager.GetAllDsskeyNum();
}

bool dsskey_SetDsskeyStatus(int dsskeyID, Dsskey_Status status)
{
    CDSSkeyData * pDsskey = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (NULL == pDsskey || pDsskey->GetDsskeyType() == DT_NA)
    {
        return false;
    }
    return pDsskey->SetStatus(status);
}
#if IF_BUG_XMLBROWSER_EXP_LED
bool dsskey_SetDsskeyLightType(int dsskeyID, LIGHT_Type l_Type)
{
    CDSSkeyData * pDsskey = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (NULL == pDsskey)
    {
        return false;
    }
    pDsskey->SetLightType(l_Type);
    return true;
}
#endif

bool dsskey_SetDsskeyStatusByType(DssKey_Type type, Dsskey_Status status,
                                  bool bExpProcess /*= false*/)
{
    _DsskeyManager.UpdateStatus(type, status);
    return true;
}


yl::string dsskey_GetDsskeyLabel(int dsskeyID, int iFlag/* = DSS_ATTR_NONE*/)
{
    if (iFlag != DSS_ATTR_NONE)
    {
        return CDSSkeyData::GetStringConfig(dsskeyID, ATTR_LABEL, iFlag == DSS_ATTR_FACTORY ? CONFIG_LEVEL_FACTORY : CONFIG_LEVEL_IGNORE);
    }

    CDSSkeyData * pDsskey = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pDsskey)
    {
        yl::string strOutput;
        pDsskey->GetLabel(strOutput);
        return strOutput;
    }
    return "";
}

bool dsskey_SetDsskeyType(int dsskeyID, DssKey_Type type)
{
    CDSSkeyData * pDsskey = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pDsskey)
    {
        pDsskey->SetDsskeyType(type);
        return true;
    }
    return false;
}

bool dsskey_OnBindProcess(int dsskeyID)
{
    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {

        CDSSKeyAction * pAction = CDSSKeyActionManger::GetInstance()->GetDSSKeyAction(
                                      pData->GetDsskeyType());
        if (pAction)
        {
            DssKeyLogicData logicData ;
            pData->GetLogicData(logicData);
            return pAction->OnBinded(logicData.iDssKeyID, logicData, NULL);
        }
    }
    return false;
}

bool dsskey_OnUnbindProcess(int dsskeyID, bool bUpdate/* = true*/)
{
    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {

        CDSSKeyAction * pAction = CDSSKeyActionManger::GetInstance()->GetDSSKeyAction(
                                      pData->GetDsskeyType());
        if (pAction)
        {
            DssKeyLogicData logicData ;
            pData->GetLogicData(logicData);
            return pAction->OnDebinded(logicData.iDssKeyID, logicData, (LPVOID)&bUpdate);
        }
    }
    return false;
}

yl::string dsskey_GetDsskeyExtension(int dsskeyID, int iFlag/* = DSS_ATTR_NONE*/)
{
    if (iFlag != DSS_ATTR_NONE)
    {
        return CDSSkeyData::GetStringConfig(dsskeyID, ATTR_EXTENSION, iFlag == DSS_ATTR_FACTORY ? CONFIG_LEVEL_FACTORY : CONFIG_LEVEL_IGNORE);
    }

    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {
        yl::string strOutput;
        pData->GetExtension(strOutput);
        return strOutput;
    }
    return "";
}

bool dsskey_SetDsskeyValue(int dsskeyID, const yl::string & strValue)
{
    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {

        pData->SetValue(strValue);
        return true;
    }
    return false;
}

bool dsskey_SetDsskeyLabel(int dsskeyID, const yl::string & strValue)
{
    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {
        pData->SetLabel(strValue);
        return true;
    }
    return false;
}

bool dsskey_SetDsskeyExtension(int dsskeyID, const yl::string & strValue)
{
    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {
        pData->SetExtension(strValue);
        return true;
    }
    return false;
}

bool dsskey_SetDsskeyTimerID(int dsskeyID, UINT timerID)
{
    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {
        pData->setTimerID(timerID);
        return true;
    }
    return false;
}

int dsskey_GetRelatedTimerID(int dsskeyID)
{

    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {
        return pData->getTimerID();
    }
    return 0;
}

bool dsskey_GetDssekeyExtFiled(int dsskeyID, const yl::string & strFieldName,
                               yl::string & strFieldValue)
{

    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {
        strFieldValue = pData->getExtFiledValue(strFieldName);
        return true;
    }
    return false;

}

bool dsskey_SetDssekeyExtFiled(int dsskeyID, const yl::string & strFieldName,
                               const yl::string & strFieldValue)
{

    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {
        pData->setExtFieldValue(strFieldName, strFieldValue);
        return true;
    }
    return false;

}
DSSKEY_MODULE_TYPE dsskey_GetModuleType(int dsskeyID)
{
    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {
        return pData->getDsskeyModuleType();
    }
    return DMT_NON;
}

bool dsskey_SetRemoteStatus(int dsskeyID, char remoteStatus)
{

    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {
        pData->setRemoteStatus(remoteStatus);
        return true;
    }
    return false;
}

bool dsskey_GetRemoteStatus(int dsskeyID, char & remoteStatus)
{
    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {
        remoteStatus = pData->getRemoteStatus();
        return true;
    }
    return false;
}


int dsskey_GetDsskeyInfoByKeyIndex(int iKeyIndex)
{
    CDSSKeyDataModule * pDsskeyKeyModule = NULL;
    if (iKeyIndex < FIRST_EXPKEY_INDEX)
    {
        // linekey module
        pDsskeyKeyModule = _DsskeyManager.GetDssKeyModule(DMT_LINEKEY_MODULE);
    }
    else
    {
        int iModuleIndex = (iKeyIndex - FIRST_EXPKEY_INDEX) / 40;

        iKeyIndex = iKeyIndex - FIRST_EXPKEY_INDEX - iModuleIndex * 40;
        // exp module
#if 0 //by linym
        pDsskeyKeyModule =  _DsskeyManager.GetDssKeyModule(, iModuleIndex);
#endif
    }

    if (pDsskeyKeyModule != NULL)
    {
        CDSSkeyData * pDsskeyData = pDsskeyKeyModule->GetDsskeyDataByIndex(iKeyIndex);
        if (pDsskeyData)
        {
            return pDsskeyData->GetDsskeyDataID();
        }
    }
    return -1;
}

// int dsskey_GetDsskeyIndexByID(int dsskeyID)
// {
//  int iIndex = 0;
//  // 旧T2X算法
//  if (GetEXPModuleType(dsskeyID) == DMT_MEMKEY_MODULE)
//  {
//      iIndex = GetEXPKeyIndex(dsskeyID);
//  }
//  else if (GetEXPModuleType(dsskeyID) == DMT_LINEKEY_MODULE)
//  {
//      iIndex = GetEXPKeyIndex(dsskeyID) + FIRST_LINEKEY_INDEX;
//  }
//  else if (GetEXPModuleType(dsskeyID) == DMT_EXP38_MODULE
//      || GetEXPModuleType(dsskeyID) == DMT_EXP39_MODULE
//      || GetEXPModuleType(dsskeyID) == DMT_EXP40_MODULE)
//  {
//      // 扩展台
//      iIndex = GetEXPKeyIndex(dsskeyID) + FIRST_EXPKEY_INDEX + GetEXPIndex(dsskeyID) * 40;
//  }
//
//  return iIndex;
// }

bool dsskey_IsDsskey(int iKeyCode, bool bExp)
{
    if (!bExp)
    {
        return ((iKeyCode >= PHONE_KEY_LINE1 && iKeyCode <= PHONE_KEY_LINE10)
                || (iKeyCode >= PHONE_KEY_MEMORY1 && iKeyCode <= PHONE_KEY_MEMORY10));
    }
    else
    {

    }
    return false;

}

bool dsskey_OnTimer(unsigned int uTimer)
{
    return FALSE;
}

void dsskey_GetDsskeysByAccID(YLList<int> & listOutput, int accountID)
{

    _DsskeyManager.GetAllDsskeyByAccID(listOutput, accountID);
    return;
}

bool dsskey_SetTimer(int dsskeyID, UINT timerInterval)
{

    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {
        pData->SetTimer(timerInterval);
        return true;
    }
    return false;


}

bool dsskey_KillTimer(int dsskeyID)
{
    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {
        pData->killTimer();
        return true;
    }
    return false;
}

bool dsskey_UpdateRemoteStatusByModule(DSSKEY_MODULE_TYPE moduleType, const char * lpStatus,
                                       int num, int moduleIndex /*= 0*/)
{
    if (lpStatus)
    {
        CDSSKeyDataModule * pModule = _DsskeyManager.GetDssKeyModule(moduleType, moduleIndex);
        if (pModule)
        {
            for (int i = 0; i < num; ++i)
            {
                CDSSkeyData * pDsskey = pModule->GetDsskeyDataByIndex(i);
                if (pDsskey == NULL)
                {
                    continue;
                }
                DssKey_Type eType = pDsskey->GetDsskeyType();
                if (dsskey_IsBindType(eType))
                {
                    if (eType == DT_BLFLIST)
                    {
                        // BLF List通过UpDateBlfListRemoteStatus来更新按键状态
                        continue;
                    }
                    pDsskey->setRemoteStatus(lpStatus[i]);
                }

            }
            return true;
        }
    }
    return false;

}
void dsskey_ResetRemoteStatusByAccount(int iLineId)
{
    if (iLineId < 0
            || iLineId >= acc_AccountNum())
    {
        return;
    }

    _DsskeyManager.ResetRemoteStatusByAccount(iLineId);
}



bool dsskey_IsStatusExist(int dsskeyID, Dsskey_Status status)
{
    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {
        return pData->IsStatusExist(status);
    }
    return false;
}

bool dsskey_IsStatusCtrlByRemoteType(int dsskeyID)
{
    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData)
    {
        return pData->IsStatusCtrlByRemote();
    }
    return false;
}

bool dsskey_SetLineStatus(int dsskeyID, Dsskey_Status lineStatus)
{
    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(dsskeyID);
    if (pData && pData->GetDsskeyType() == DT_LINE && lineStatus > DS_LINE_FIRST
            && lineStatus < DS_LINE_END)
    {
        pData->UpdateLineStatus(lineStatus);
        return true;
    }
    return false;
}

int dsskey_GetModuleIndex(int dsskeyID)
{
    return ((dsskeyID >> 6) & 0xff);
}

int dsskey_GetKeyIndex(int dsskeyID)
{
    return (dsskeyID & 0x003f);
}

int dsskey_GetDsskeyByKeyIndex(int index)
{
    int iDsskeyId = -1;
    int iIndex = index;
    YLList<DSSKEY_MODULE_TYPE> typeList;
    typeList.push_back(DMT_PROGRAMEKEY_MODULE);
    typeList.push_back(DMT_LINEKEY_MODULE);
    typeList.push_back(DMT_MEMKEY_MODULE);
    for (int i = 0; i < exp_GetEXPNum(); i++)
    {
        /*typeList.push_back(DMT_EXP40_MODULE);*/
        typeList.push_back((DSSKEY_MODULE_TYPE)exp_GetEXPType());
    }

    YLList<DSSKEY_MODULE_TYPE>::ListIterator iter = typeList.begin();
    int iExpModuleIndex = 0;

    for (; iter != typeList.end(); ++iter)
    {
        if (iIndex < _DsskeyManager.GetModuleDsskeyNum(*iter))
        {
            if ((*iter) >= DMT_LINEKEY_MODULE && (*iter) <= DMT_PROGRAMEKEY_MODULE)
            {
                iDsskeyId = GetDsskeyDataID(*iter, 0, iIndex);
            }
            else
            {
                iDsskeyId = GetDsskeyDataID(*iter, iExpModuleIndex, iIndex);
            }
            break;
        }
        if (*iter == DMT_EXP38_MODULE
                || *iter == DMT_EXP39_MODULE
                || *iter == DMT_EXP40_MODULE
                || *iter == DMT_EXP50_MODULE
                || *iter == DMT_EXP20_MODULE
                || *iter == DMT_EXPCM69_MODULE)
        {
            iExpModuleIndex++;
        }
        iIndex -= _DsskeyManager.GetModuleDsskeyNum(*iter);
    }
    return iDsskeyId;
}

int dsskey_GetDsskeyIndexByID(int dsskeyID)
{
    int iIndex = 0;

    if (dsskey_GetModuleType(dsskeyID) == DMT_PROGRAMEKEY_MODULE)
    {
        iIndex = dsskey_GetKeyIndex(dsskeyID);
    }
    else if (dsskey_GetModuleType(dsskeyID) == DMT_LINEKEY_MODULE)
    {
        iIndex = dsskey_GetKeyIndex(dsskeyID) + _DsskeyManager.GetModuleDsskeyNum(DMT_PROGRAMEKEY_MODULE);
    }
    else if (dsskey_GetModuleType(dsskeyID) == DMT_MEMKEY_MODULE)
    {
        iIndex = dsskey_GetKeyIndex(dsskeyID) + _DsskeyManager.GetModuleDsskeyNum(DMT_PROGRAMEKEY_MODULE) \
                 + _DsskeyManager.GetModuleDsskeyNum(DMT_LINEKEY_MODULE);
    }
    else if (dsskey_GetModuleType(dsskeyID) == DMT_EXP38_MODULE
             || dsskey_GetModuleType(dsskeyID) == DMT_EXP39_MODULE
             || dsskey_GetModuleType(dsskeyID) == DMT_EXP40_MODULE
             || dsskey_GetModuleType(dsskeyID) == DMT_EXP50_MODULE
             || dsskey_GetModuleType(dsskeyID) == DMT_EXP20_MODULE
             || dsskey_GetModuleType(dsskeyID) == DMT_EXPCM69_MODULE)
    {
        // 扩展台
        iIndex = _DsskeyManager.GetModuleDsskeyNum(DMT_PROGRAMEKEY_MODULE) \
                 + _DsskeyManager.GetModuleDsskeyNum(DMT_LINEKEY_MODULE) + _DsskeyManager.GetModuleDsskeyNum(
                     DMT_MEMKEY_MODULE);

        for (int i = 0; i < GetEXPIndex(dsskeyID); i++)
        {
            iIndex += _DsskeyManager.GetModuleDsskeyNum(dsskey_GetModuleType(dsskeyID), i);
        }
        iIndex += GetEXPKeyIndex(dsskeyID);
    }

    return iIndex;

}

// 获取指定账号的dsskey个数
int dsskey_CountsDsskeyByAccID(int iAccountID, bool bNative/* = false*/)
{
    return _DsskeyManager.CountsDsskeyByAccID(iAccountID, bNative);
}

// 获取dsskey对应的blaindex
bool dsskey_GetBLAIndexByDsskey(int & iIndex, int iDsskeyID, int iAccountID)
{
    return _DsskeyManager.GetBLAIndexByDsskey(iIndex, iDsskeyID, iAccountID);
}

// 获取blaindex对应的dsskeyid
int dsskey_GetDsskeyByBLAIndex(int iIndex, int iAccountID)
{
    return _DsskeyManager.GetDsskeyByBLAIndex(iIndex, iAccountID);
}

bool dsskey_OnConfigChange(msgObject & msg)
{

    bool bHandled = true;

    switch (msg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        bHandled = _DsskeyManager.OnConfigChange(msg);
        break;
    default:
        return false;
    }
    return bHandled;

}

bool dsskey_SaveDsskeyInfoToFile(const yl::string & strFilePath,
                                 int iFileType /*= DSS_FILE_TYPE_M7*/, DSSKEY_MODULE_TYPE eModuleType /*= DMT_NON*/,
                                 int iModuleIndex /*= 0*/)
{
    DSSKEY_INFO("SaveDsskeyInfoToFile file[%s] type[%d] module[%d][%d]", strFilePath.c_str(), iFileType,
                eModuleType, iModuleIndex);
    bool bRet = _DsskeyManager.SaveDsskeyInfoToFile(strFilePath, iFileType,
                GetModuleIDByModuleIndex(eModuleType, iModuleIndex));
    DSSKEY_INFO("SaveDsskeyInfoToFile file[%s] type[%d] finish", strFilePath.c_str(), iFileType);
    return bRet;
}

bool dsskey_ListenOtherTypeChange(DssKey_Type eRecvType, DssKey_Type eChangeType)
{
    CDSSKeyAction * pChangeAction = CDSSKeyActionManger::GetInstance()->GetDSSKeyAction(eChangeType);
    if (pChangeAction)
    {
        return pChangeAction->AddChangeListenType(eRecvType);
    }

    return false;
}

bool dsskey_SaveModuleDataToFile(DSSKEY_MODULE_TYPE eModuleType, int nModuleIndex)
{
    CDSSKeyDataModule * pModule = _DsskeyManager.GetDssKeyModule(eModuleType, nModuleIndex);

    if (pModule)
    {
        return pModule->SaveToMemory();
    }

    return false;
}

bool dsskey_IsSupportModuleType()
{
    return IsSupportModuleType(exp_GetEXPModuleType());
}

void dsskey_ResetDsskeyStatus(int iDsskeyID)
{
    dsskey_KillTimer(iDsskeyID);
    dsskey_SetDsskeyStatus(iDsskeyID, DS_NON);
    dsskey_refreshDSSKeyUI(iDsskeyID);
}

Dsskey_Status dsskey_MapStatusToDsskeyStatus(BLF_STATUS_CODE eStatus)
{
    switch (eStatus)
    {
    case CN_BLF_STATUS_IDLE:
        return DS_BLF_STATUS_IDLE;
    case CN_BLF_STATUS_RINGING:
        return DS_BLF_STATUS_RINGING;
    case CN_BLF_STATUS_CALLOUT:
        return DS_BLF_STATUS_CALLOUT;
    case CN_BLF_STATUS_TALKING:
        return DS_BLF_STATUS_TALKING;
    case CN_BLF_STATUS_UNKOWN:
        return DS_BLF_STATUS_UNKOWN;
    case CN_BLF_STATUS_HOLD:
        return DS_BLF_STATUS_HOLD;
    case CN_BLF_STATUS_PARK:
        return DS_BLF_STATUS_PARK;
    case CN_BLF_STATUS_DND:
        return DS_BLF_STATUS_DND;

    case CN_BLA_STATUS_IDLE:
        return DS_BLA_STATUS_IDLE;
    case CN_BLA_STATUS_SEIZED:
        return DS_BLA_STATUS_SEIZED;
    case CN_BLA_STATUS_PROGRESSING:
        return DS_BLA_STATUS_PROGRESSING;
    case CN_BLA_STATUS_ALERTING:
        return DS_BLA_STATUS_ALERTING;
    case CN_BLA_STATUS_ACTIVE:
        return DS_BLA_STATUS_ACTIVE;
    case CN_BLA_STATUS_HELD:
        return DS_BLA_STATUS_HELD;
    case CN_BLA_STATUS_HELD_PRIVATE:
        return DS_BLA_STATUS_HELD_PRIVATE;
    case CN_BLA_STATUS_BRIDGE_ACITVE:
        return DS_BLA_STATUS_BRIDGE_ACITVE;
    case CN_BLA_STATUS_BRIDGE_HELD:
        return DS_BLA_STATUS_BRIDGE_HELD;
    default:
        return DS_BLF_STATUS_UNKOWN;
    }

    return DS_BLF_STATUS_UNKOWN;
}

#if IF_BUG_31595
void dsskey_ShowXMLBrowserInfo(yl::string strXMLInfo)
{
    _DsskeyManager.ShowXMLBrowserInfo(strXMLInfo);
}

int dsskey_GetDsskeyIDByKeyIndex(int index)
{
    if (index < MAX_LINEKEY_NUM)
    {
        return index;
    }

    int iDsskeyId = -1;
    int iIndex = index - MAX_LINEKEY_NUM;
    YLList<DSSKEY_MODULE_TYPE> typeList;
    for (int i = 0; i < exp_GetEXPNum(); i++)
    {
        typeList.push_back((DSSKEY_MODULE_TYPE)exp_GetEXPType());
    }

    YLList<DSSKEY_MODULE_TYPE>::ListIterator iter = typeList.begin();
    int iExpModuleIndex = 0;

    for (; iter != typeList.end(); ++iter)
    {
        int iTotalCount = _DsskeyManager.GetModuleDsskeyNum(*iter);

        if (iIndex < iTotalCount)
        {
            iDsskeyId = GetDsskeyDataID(*iter, iExpModuleIndex, iIndex);
            break;
        }
        if (IsEXPModule(*iter))
        {
            iExpModuleIndex++;
        }
        iIndex -= iTotalCount;
    }
    return iDsskeyId;
}
#endif

void dsskey_GetDsskeysByType(YLList<int> & listOutput, DssKey_Type eType,
                             bool bPositiveSequence/* = true*/)
{
    if (bPositiveSequence)
    {
        dsskey_GetAllDsskeyDataByModule(listOutput, DMT_LINEKEY_MODULE, 0, eType);
        for (int i = 0; i < exp_GetEXPNum(); ++i)
        {
            dsskey_GetAllDsskeyDataByModule(listOutput, exp_GetEXPModuleType(), i, eType);
        }
    }
    else
    {
        for (int i = 0; i < exp_GetEXPNum(); ++i)
        {
            dsskey_GetAllDsskeyDataByModule(listOutput, exp_GetEXPModuleType(), i, eType);
        }
        dsskey_GetAllDsskeyDataByModule(listOutput, DMT_LINEKEY_MODULE, 0, eType);
    }

}

#if IF_FEATURE_SRTP
void dsskey_UpdateSRTPStatus()
{
    YLList<int> listAccounts;
    listAccounts = acc_GetAvailableAccountList();
    bool bSRTPComplusory = listAccounts.size() > 0;

    for (YLList<int>::iterator itBeg = listAccounts.begin(); itBeg != listAccounts.end(); ++itBeg)
    {
        int iAccount = *itBeg;

        if (configParser_GetCfgItemIntValue(kszAccountSrtpEncryption, iAccount) != SRTP_COMPLUSORY)
        {
            bSRTPComplusory = false;
            break;
        }
    }

    Dsskey_Status eStatus = DS_NON;
    if (bSRTPComplusory)
    {
        eStatus = DS_SRTP_COMPLUSORY;
    }

    _DsskeyManager.UpdateStatus(DT_SRTP, eStatus);
}
#endif

#if IF_FEATRUE_PTT
bool dsskey_IsSupportLongPress(int iDsskeyID)
{
    DssKeyLogicData objData;
    // 获取Dsskey数据
    dsskey_GetData(iDsskeyID, objData);
    if (DT_INTERCOM == objData.eDKType
            && configParser_GetConfigInt(kszMTSWIntercomPTTEnable) == 1
            && configParser_GetConfigInt(kszMetaSwitchActive) != 0)
    {
        return true;
    }

    return false;
}

bool dsskey_OnDsskeyRelease(int iDsskeyID)
{
    CDSSkeyData * pData = _DsskeyManager.GetDsskeyDataByID(iDsskeyID);

    if (pData)
    {
        CDSSKeyAction * pAction = CDSSKeyActionManger::GetInstance()->GetDSSKeyAction(
                                      pData->GetDsskeyType());
        if (pAction)
        {
            DssKeyLogicData logicData ;
            pData->GetLogicData(logicData);
            return pAction->OnKeyRelease(logicData.iDssKeyID, logicData, NULL);
        }
    }
    return false;

}

bool dsskey_AddLongPress(int iDsskeyID)
{
    return _DsskeyManager.AddLongPress(iDsskeyID);
}

bool dsskey_RemoveLongPress(int iDsskeyID)
{
    return _DsskeyManager.RemoveLongPress(iDsskeyID);
}

bool dsskey_IsExistLongPress()
{
    return _DsskeyManager.IsExistLongPress();
}
#endif

bool dsskey_IsExpSupportSwitchKey(DSSKEY_MODULE_TYPE eExpType /*= DMT_NON*/)
{
    DSSKEY_MODULE_TYPE eMoudle = eExpType != DMT_NON ? eExpType : exp_GetEXPModuleType();
    return eMoudle == DMT_EXP20_MODULE || eMoudle == DMT_EXP39_MODULE || eMoudle == DMT_EXPCM69_MODULE;
}

bool dsskey_ReloadDSSKeyData(DssKey_Type eType/* = DT_NONE*/)
{
    return _DsskeyManager.ReloadModifyDSSKeyData(eType);
}

void dsskey_ResetModuleDataToFactory(DSSKEY_MODULE_TYPE eModuleType, int iModuleIndex)
{
    CDSSKeyDataModule * pModule = _DsskeyManager.GetDssKeyModule(eModuleType, iModuleIndex);

    if (NULL != pModule)
    {
        pModule->ResetModuleDataToFactory();
    }
}

void dsskey_UpdateLineStatus(int iDsskeyID)
{
    int iRealLineID = dsskey_GetDsskeyLine(iDsskeyID);
    Dsskey_Status eStatus = MapLineState(acc_GetState(iRealLineID), iRealLineID);
    dsskey_SetLineStatus(iDsskeyID, eStatus);

    if (acc_IsBLAEnable(iRealLineID))
    {
        BLF_STATUS_CODE eStatus = bsSCA_GetStatusByDsskeyID(iDsskeyID);
        Dsskey_Status eDsskeyStatus = dsskey_MapStatusToDsskeyStatus(eStatus);
        dsskey_UpdateStatus(iDsskeyID, eDsskeyStatus, DS_BLF_FIRST, DS_BLF_LAST, false);
    }
}


void dsskey_RemoveModule(DSSKEY_MODULE_TYPE eModuleType, int iEXPIndex /*= 0*/)
{
	_DsskeyManager.BeforeRemoveModule(eModuleType, iEXPIndex);
	_DsskeyManager.RemoveModule(eModuleType, iEXPIndex);
}

bool dsskey_IsDssKeyModuleExist(int iEXPIndex /* = 0*/, bool bReset /*= false*/)
{
	DSSKEY_MODULE_TYPE moduleType = (DSSKEY_MODULE_TYPE)exp_GetExpTypeByIndex(iEXPIndex);
	CDSSKeyDataModule * pModule = _DsskeyManager.GetDssKeyModule(moduleType, iEXPIndex);

	if (bReset && NULL == pModule)
	{
		_DsskeyManager.ResetModuleDsskeyData(moduleType, iEXPIndex);
		pModule = _DsskeyManager.GetDssKeyModule(moduleType, iEXPIndex);
	}

	return pModule == NULL ? false : true;
}

void dsskey_RefressDsskeyUIDuetoSwitchExpPage(int iEXPIndex /*= -1*/, DsskeyExpRefresh dsskeyRefresh /* = DSS_KEY_REFRESH_ALL*/)
{
	DSSKEY_MODULE_TYPE moduleType = (DSSKEY_MODULE_TYPE)exp_GetExpTypeByIndex(iEXPIndex);
	CDSSKeyDataModule * pModule = _DsskeyManager.GetDssKeyModule(moduleType, iEXPIndex);
		
	if (pModule == NULL)
	{
		DSSKEY_INFO("CEXPLogicManager::ProcessEXPPageEvent pModule not exist");
	}
	else
	{
		DSSKEY_INFO("CEXPLogicManager::ProcessEXPPageEvent pModule size [%d]", pModule->size());

		for (int i = 0; i < pModule->size(); i++)
		{
			CDSSkeyData * pDsskeyData = pModule->GetDsskeyDataByIndex(i);

			if (pDsskeyData == NULL)
			{
				continue;
			}

			int iDsskeyID = pDsskeyData->GetDsskeyDataID();

			if (exp_IsExpKeyInCurrentPage(iDsskeyID))
			{
				dsskey_refreshDSSKeyUI(iDsskeyID, dsskeyRefresh);
			}
		}
	}
}

void dsskey_RefressDsskeyUIDuetoExpUpdate(int iEXPIndex /*= -1*/, bool bColorExp /*= false*/)
{
	DSSKEY_MODULE_TYPE moduleType = (DSSKEY_MODULE_TYPE)exp_GetExpTypeByIndex(iEXPIndex);
	CDSSKeyDataModule * pModule = _DsskeyManager.GetDssKeyModule(moduleType, iEXPIndex);

	if (NULL == pModule)
	{
		_DsskeyManager.ResetModuleDsskeyData(moduleType, iEXPIndex);
		pModule = _DsskeyManager.GetDssKeyModule(moduleType, iEXPIndex);
	}

	if (NULL == pModule)
	{
		DSSKEY_INFO("CEXPLogicManager::RefreshExp pModule not exist");
	}
	else
	{
		DSSKEY_INFO("CEXPLogicManager::RefreshExp pModule Size = [%d]\n", pModule->size());
		for (int i = 0; i < pModule->size(); i++)
		{
			CDSSkeyData * pDsskeyData = pModule->GetDsskeyDataByIndex(i);
			if (pDsskeyData)
			{
				int dsskeyID = pDsskeyData->GetDsskeyDataID();

				// 重新刷新账号状态
				// http://10.2.1.199/Bug.php?BugID=103338
				if (pDsskeyData->GetDsskeyType() == DT_LINE)
				{
					int iLineID = dsskey_GetDsskeyLine(dsskeyID);
					bool bResult = dsskey_SetLineStatus(dsskeyID, MapLineState(acc_GetState(iLineID), iLineID));

					if (bColorExp)
					{
						// sub test
						DssKeyLogicData DKLogicData;
						DSSKEY_INFO("line id[%d], Account state[%d], line status[%d]",
							iLineID,
							acc_GetState(iLineID),
							MapLineState(acc_GetState(iLineID), iLineID));
						DSSKEY_INFO("dsskey id:[%d], set result[%d], get result[%d] dsskey status[%d], curr page?[%d]",
							dsskeyID,
							bResult,
							dsskey_GetData(dsskeyID, DKLogicData),
							DKLogicData.iStatus,
							exp_IsExpKeyInCurrentPage(dsskeyID));
					}

				}

				if (bColorExp && !exp_IsExpKeyInCurrentPage(dsskeyID))
				{
					continue;
				}

				dsskey_refreshDSSKeyUI(dsskeyID);
			}
		}
	}
}