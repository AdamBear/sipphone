#include "settinguibstfcallcontrolldelegate.h"

#ifdef IF_FEATURE_BROADSOFT_BASE

#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settinguilogic/src/settinguimenuitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "account/include/modaccount.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "broadsoft/executiveassistant/include/execommon.h"
#include "broadsoft/executiveassistant/include/modexecutiveassistant.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
const char * const strOpenPage = "openpage(";
}

CSettinguiBSTFCallControllDelegate::CSettinguiBSTFCallControllDelegate()
{

}

CSettinguiBSTFCallControllDelegate::~CSettinguiBSTFCallControllDelegate()
{

}

CSettingUIDelegateBase * CSettinguiBSTFCallControllDelegate::CreateBSTFCallControllDelegate()
{
    CSettinguiBSTFCallControllDelegate * pDelegate = new CSettinguiBSTFCallControllDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase * CSettinguiBSTFCallControllDelegate::LoadPageData(
    bool bReLoad /* = false */)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL != m_pPageData && bReLoad)
    {
        LoadBrosoftListItem();
    }

    return m_pPageData;
}

void CSettinguiBSTFCallControllDelegate::OnEnterPage(const yl::string & strPage, int iDataIndex)
{
    if (!IsEnableToEnterNextPage(strPage))
    {
        return;
    }

    CSettingUIItemDataBasePtr pItem = GetItemDataByDataIndex(iDataIndex);
    if (NULL != pItem)
    {
        yl::string strRealPage = strPage;
        if (kszPageIDDNDLineSelect == strPage && 0 == configParser_GetConfigInt(kszDndMode))
        {
            strRealPage = kszPageIDPhoneDND;
        }
        else if (kszPageIDCallForward == strPage && 1 == configParser_GetConfigInt(kszForwardMode))
        {
            strRealPage = kszPageIDCallForwardLineSelect;
        }

        PageOpenData opPageData(strRealPage);
        opPageData.pExtraData = pItem->GetExtraData();
        g_SetUICallBackFun.EnterPage(opPageData);
    }
}

void CSettinguiBSTFCallControllDelegate::LoadMenuItem(const yl::string & strID
        , const yl::string & strDisplayName
        , const yl::string & strPageID
        , int nAccountCount
        , const yl::string & strExtra /*= ""*/)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    yl::string strNextPage = commonAPI_FormatString("%s%s)", strOpenPage, strPageID.c_str());
    AddMenuItem(m_pPageData, strID, strDisplayName, strID, true, nAccountCount, strExtra, strNextPage);
}

void CSettinguiBSTFCallControllDelegate::LoadMoreAccountItem(const yl::string & strItemID,
        const yl::string & strPageID, const yl::string & strValue, int nAccountCount)
{
    //账号数目为1 的话附加默认账号 acc_GetDefaultAccount， 超过1则进入账号选择界面，附加参数为选中账号选择界面的ID
    if (1 == nAccountCount)
    {
        yl::string strExtraData = commonAPI_FormatString("%d", acc_GetDefaultAccount());
        LoadMenuItem(strItemID, strValue, strPageID, nAccountCount, strExtraData);
    }
    else
    {
        LoadMenuItem(strItemID, strValue, kszPageIDLineSelect, nAccountCount, strPageID);
    }
}

void CSettinguiBSTFCallControllDelegate::LoadBrosoftListItem()
{
    if (NULL == m_pPageData)
    {
        return ;
    }

    yl::string strPageID;
    int iRegAccNum = acc_GetRegisteredAccountNum();

    if (0 == iRegAccNum)
    {
        int nAccountId = -1;
        yl::string strExtra = commonAPI_FormatString("%d", nAccountId);
#if (!IF_BUG_23784)
        if (0 == configParser_GetConfigInt(kszForwardForbidden))
        {
            strPageID = configParser_GetConfigInt(kszForwardMode) == 1 ? kszPageIDCallForwardLineSelect :
                        kszPageIDCallForward;
            LoadMenuItem(BROADSOFT_CALLCONTROL_FORWARD, _LANG(TRID_CALL_FORWARD), strPageID, iRegAccNum,
                         strExtra);
        }
#if !IF_BUG_24554
#ifndef IF_FEATURE_PSTN_DND
        if (netIsCableConnected())
#endif
        {
            if (0 == configParser_GetConfigInt(kszDndForbidden))
            {
                // 开启bsft dnd选项
                LoadMenuItem(BROADSOFT_CALLCONTROL_DND, _LANG(TRID_DO_NOT_DISTURB), kszPageIDDNDLineSelect,
                             iRegAccNum, strExtra);
            }
        }
#endif
#endif   //IF_BUG_23784
    }
    else
    {
#ifdef IF_FEATURE_BROADSOFT_ANYWHERE
        LoadMoreAccountItem(BROADSOFT_CALLCONTROL_ANYWHERE, kszPageIDAnyWhereLocationList,
                            _LANG(TRID_ANYWHERE), iRegAccNum);
#endif
#ifdef IF_FEATURE_BROADSOFT_MOBILITY
        LoadMoreAccountItem(BROADSOFT_CALLCONTROL_MOBILITY, kszPageIDCallControllMobility,
                            _LANG(TRID_MOBILITY), iRegAccNum);
#endif //IF_FEATURE_BROADSOFT_MOBILITY

#ifdef IF_FEATURE_BROADSOFT_REMOTE_OFFICE
        LoadMoreAccountItem(BROADSOFT_CALLCONTROL_REMOTEOFFICE,  kszPageIDCallControllRemoteOffice,
                            _LANG(TRID_REMOTE_OFFICE), iRegAccNum);
#endif

#if (!IF_BUG_23784)
        if (0 == configParser_GetConfigInt(kszForwardForbidden))
        {
            yl::string strExtra = commonAPI_FormatString("%d", -1);
            strPageID = kszPageIDCallForward;
            if (1 != iRegAccNum)
            {
                if (1 == configParser_GetConfigInt(kszForwardMode))
                {
                    strPageID = kszPageIDCallForwardLineSelect;
                }
                else
                {
                    strPageID = kszPageIDCallForward;
                    // 全局的参数使用单账户的方式，否则数据转换Extra会有问题。
                    iRegAccNum = 1;
                }
            }
            LoadMenuItem(BROADSOFT_CALLCONTROL_FORWARD, _LANG(TRID_CALL_FORWARD), strPageID, iRegAccNum,
                         strExtra);
        }
#endif  // IF_BUG_23784

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
        LoadMoreAccountItem(BROADSOFT_CALLCONTROL_FORWARD_NOT_REACHABLE, kszPageIDCallControllCFNR,
                            _LANG(TRID_FORWARD_NOT_REACHABLE), iRegAccNum);
#endif

#if (!IF_BUG_23784 && !IF_BUG_24554)
#ifndef IF_FEATURE_PSTN_DND
        if (netIsCableConnected())
#endif
        {
            if (0 == configParser_GetConfigInt(kszDndForbidden))
            {
                int nAccountId = -1;
                yl::string strExtra = commonAPI_FormatString("%d", nAccountId);
                // 开启bsft dnd选项
                LoadMenuItem(BROADSOFT_CALLCONTROL_DND, _LANG(TRID_DO_NOT_DISTURB), kszPageIDDNDLineSelect,
                             iRegAccNum, strExtra);
            }
        }
#endif  // IF_BUG_23784

#ifdef IF_FEATURE_BROADSOFT_CLID
        LoadMoreAccountItem(BROADSOFT_CALLCONTROL_ANONYMOUSCALL, kszPageIDCallControllHideNumber,
                            _LANG(TRID_HIDE_NUMBER), iRegAccNum);
        LoadMoreAccountItem(BROADSOFT_CALLCONTROL_ANONYMOUSREJECTION,
                            kszPageIDCallControllAnonymoRrejection, _LANG(TRID_ANONYMOUS_REJECTION), iRegAccNum);
#endif
        LoadMoreAccountItem(BROADSOFT_CALLCONTROL_SIMULTANEOUSRING,
                            kszPageIDCallControllSimultaneousRingList, _LANG(TRID_SIMULTANEOUS_RING_PERSONAL), iRegAccNum);

#ifdef IF_FEATURE_BROADSOFT_GROUP_NIGHT_FORWARD
        LoadMoreAccountItem(BROADSOFT_CALLCONTROL_GNF, kszPageIDCallControllGNF,
                            _LANG(TRID_GROUP_NIGHT_FORWARD), iRegAccNum);
#endif

#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
        if (1 == configParser_GetConfigInt(kszCallWaitingMode))
        {
            LoadMoreAccountItem(BROADSOFT_CALLCONTROL_CALLWAITING, kszPageIDCallControllCallWaiting,
                                _LANG(TRID_CALL_WAITING), iRegAccNum);
        }
#endif

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE
        LoadMoreAccountItem(BROADSOFT_CALLCONTROL_FORWARD_SELECTIVE, kszPageIDCallControllForwardSelective,
                            _LANG(TRID_FORWARD_SELECTIVE), iRegAccNum);
#endif
    }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    //ACD开启才添加ACD State和ACD Init State两个选项
    if (CallCenter_IsACDOn() && ACD_BROADSOFT == Get_ACDType())
    {
        LoadMenuItem(BROADSOFT_CALLCONTROL_INITACDSTATE, _LANG(TRID_INITAIL_ACD_STATE),
                     kszPageIDCallControllInitialACDStatus, -1);
        LoadMenuItem(BROADSOFT_CALLCONTROL_SETACDSTATE, _LANG(TRID_ACD_STATE),
                     kszPageIDCallControllSetACDStatus, -1);
    }
#endif //IF_FEATURE_BROADSOFT_CALL_CENTER

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    AddExecutiveAssistantCallControlItem();
#endif

    CreateSecurityClassfication();
}

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
void CSettinguiBSTFCallControllDelegate::AddExecutiveAssistantCallControlItem()
{
    if (NULL == m_pPageData)
    {
        return;
    }

    EEA_TYPE eType = EET_Executive;
    int iAvailAccount = BWExeAssis_GetAccountCountByType(eType);
    if (1 == iAvailAccount)
    {
        int iAccountID = BWExeAssis_GetFirstUsableAccountByType(eType);
        yl::string strExtraData = commonAPI_FormatString("%d", iAccountID);
        LoadMenuItem(BROADSOFT_CALLCONTROL_EXECUTIVE, _LANG(TRID_EXECUTIVE),
                     kszPageIDCallControllExecutiveDetails, iAvailAccount, strExtraData);
    }
    else if (iAvailAccount > 1)
    {
        // 账号选择页面
        LoadMenuItem(BROADSOFT_CALLCONTROL_EXECUTIVE, _LANG(TRID_EXECUTIVE), kszPageIDLineSelect,
                     iAvailAccount, kszPageIDCallControllExecutiveDetails);
    }

    eType = EET_Assistant;
    iAvailAccount = BWExeAssis_GetAccountCountByType(eType);
    if (1 == iAvailAccount)
    {
        int iAccountID = BWExeAssis_GetFirstUsableAccountByType(eType);
        yl::string strExtraData = commonAPI_FormatString("%d", iAccountID);
        LoadMenuItem(BROADSOFT_CALLCONTROL_EXECUTIVE_ASSISTANT, _LANG(TRID_EXECUTIVE_ASSISTANT),
                     kszPageIDCallControllAssistantDetails, iAvailAccount, strExtraData);
    }
    else if (iAvailAccount > 1)
    {
        LoadMenuItem(BROADSOFT_CALLCONTROL_EXECUTIVE_ASSISTANT, _LANG(TRID_EXECUTIVE_ASSISTANT),
                     kszPageIDLineSelect, iAvailAccount, kszPageIDCallControllAssistantDetails);
    }
}
#endif

void CSettinguiBSTFCallControllDelegate::CreateSecurityClassfication()
{
    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        if (!acc_IsAccountAvailable(i))
        {
            continue;
        }

        if (configParser_GetCfgItemIntValue(kszAccountEnableSecurityClassification, i) != 1)
        {
            continue;
        }

        LoadMenuItem(BROADSOFT_CALLCONTROL_SECURITYCLASSIFICATION, _LANG(TRID_SECURITY_CLASSIFICATION),
                     kszPageIDSecurityClassfication, 1, "");
    }
}

bool CSettinguiBSTFCallControllDelegate::IsEnableToEnterNextPage(const yl::string & strPageID)
{
    if (acc_GetRegisteredAccountNum() != 1)
    {
        return true;
    }

    if (kszPageIDAnyWhereLocationList == strPageID
            || kszPageIDCallControllRemoteOffice == strPageID
            || kszPageIDCallControllHideNumber == strPageID
            || kszPageIDCallControllAnonymoRrejection == strPageID
            || kszPageIDCallControllGNF == strPageID
            || kszPageIDCallControllCFNR == strPageID
            || kszPageIDCallControllMobility == strPageID
            || kszPageIDCallControllCallWaiting == strPageID
            || kszPageIDCallControllCallRecord == strPageID
            || kszPageIDCallControllForwardSelective == strPageID
            || kszPageIDCallControllSimultaneousRingList == strPageID
            || kszPageIDCallControllAssistantDetails == strPageID
            || kszPageIDCallControllExecutiveDetails == strPageID)
    {
        // 判断xsi信息是否完整
        int nLineId = acc_GetDefaultAccount();
        if (!BWCallControl_IsAccountXSIEnable(nLineId))
        {
            g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_WARNING),
                                                   _LANG(TRID_XSI_INFORMATION_IS_INCOMPLETE)
                                                   , SETTING_MESSAGEBOX_NOTE, "", "", 1500);
            return false;
        }
    }

    return true;
}

#endif // IF_FEATURE_BROADSOFT_BASE
