#include "implperformanceline.h"
#include <ETLLib.hpp>
#include "baseui/t4xpicpath.h"
#include "talk/broadsoftsca/include/modbroadsoft_sca.h"
#include "account/include/accountmanager.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include <yllist.h>
#include "account/include/modaccount.h"
#include "talk/talklogic/src/talklogic_inc.h"
#include "presence/include/modpresence.h"
#include "talk/broadsoftsca/src/scamanager.h"
#include "talk/talkdsskey/include/modtalkdsskey.h"

ImplDsskeyPerformanceLine::ImplDsskeyPerformanceLine()
    : ImplDsskeyPerformanceBase(DT_LINE)
{
    m_lineMode = LINE_MODE_NORMAL;
    m_eSCAStatus = DS_NON;
    m_eTalkStatus = DS_NON;
}

ImplDsskeyPerformanceLine::~ImplDsskeyPerformanceLine()
{

}

// 获取显示内容前的操作
bool ImplDsskeyPerformanceLine::GetPerformanceBegin(const DssKeyLogicData & logicData)
{
    /////////////////获取账号类型////////////////////
    int nLineID = logicData.iLineIndex;
    if (nLineID == -1 && _AccountManager.GetUsableAccount(true) != NULL)
    {
        //如果是自动账号则主动获取其ID
        nLineID = _AccountManager.GetUsableAccount(true)->GetAccountId();
    }

    // 判断当前账号是否处于ACD状态
    m_lineMode = LINE_MODE_NORMAL;

    if (CallCenter_IsACDOn())
    {
        CSipAccount * pCurrentAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(
                                            nLineID));
        //支持ACD且开启ACD的账号是当前账号且其SipServer类型不为默认时，才显示ACD相关内容
        if (pCurrentAccount != NULL
                && nLineID == ACD_GetAccountId())
        {
            m_lineMode = LINE_MODE_ACD;
        }
    }

    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(
                                 nLineID));

    // ACD功能优先级比SCA高
    if (m_lineMode == LINE_MODE_NORMAL && pAccount != NULL
            && pAccount->IsAccountUsable()
            && acc_IsBLAEnable(pAccount->GetAccountId()))
    {
        m_lineMode = LINE_MODE_SCA;
    }
    ////////////////////获取账号状态////////////////////////

    m_eSCAStatus = DS_NON;
    m_eTalkStatus = DS_NON;

    YLList<int> listState = logicData.listAuxiliaryState;
    YLList<int>::iterator itr = listState.begin();
    YLList<int>::iterator itrEnd = listState.end();
    // 获取通话状态
    for (; itr != itrEnd; itr++)
    {
        if (*itr > DS_SESSION_FIRST
                && *itr < DS_SESSION_END)
        {
            m_eTalkStatus = (Dsskey_Status)(*itr);
        }
    }

    // 获取SCA账号状态
    if (LINE_MODE_SCA == m_lineMode)
    {
        itr = listState.begin();
        // 解析SCA状态
        for (; itr != itrEnd; itr++)
        {
            DSSKEYUI_INFO("[SCA]   itr->dataEntry is %d", (*itr));
            // blf状态和sca状态在同一个区间，如果取到blf状态覆盖sca导致图标显示异常
            if (*itr >= DS_BLA_STATUS_IDLE
                    && *itr <= DS_BLA_STATUS_UNKOWN)
            {
                m_eSCAStatus = (Dsskey_Status)(*itr);
                break;
            }
        }

        // 网页修改或液晶修改dsskey，需要取线路状态
        if (m_eSCAStatus == DS_NON)
        {
            m_eSCAStatus = dsskey_MapStatusToDsskeyStatus(bsSCA_GetStatusByDsskeyID(logicData.iDssKeyID));
        }

        m_bSCA_Hold = false;
        m_bSCA_Local = false;
        // Bridge 桥接时，需要做特殊处理，后续考虑加入逻辑层。
        if (m_eSCAStatus == DS_BLA_STATUS_BRIDGE_ACITVE)
        {
            CSessionProxy proxy =
                _BSSCAManager.GetDsskeySessionByStatus(logicData.iLineIndex, CN_BLA_STATUS_BRIDGE_ACITVE);
            if (proxy)
            {
                m_bSCA_Hold = talklogic_IsHoldSession(proxy);
            }
        }
        else if (m_eSCAStatus == DS_BLA_STATUS_BRIDGE_HELD)
        {
            CSessionProxy proxy =
                _BSSCAManager.GetDsskeySessionByStatus(logicData.iLineIndex, CN_BLA_STATUS_BRIDGE_HELD);
            if (proxy)
            {
                m_bSCA_Local = true;
            }
        }
        else if (m_eSCAStatus == DS_BLA_STATUS_HELD_PRIVATE)
        {
            CSessionProxy proxy =
                _BSSCAManager.GetDsskeySessionByStatus(logicData.iLineIndex, CN_BLA_STATUS_HELD_PRIVATE);
            if (proxy)
            {
                m_bSCA_Local = true;
            }
        }

        DSSKEYUI_INFO("[SCA]   m_eSCAStatus is %d", m_eSCAStatus);
    }

    return true;
}

// 获取文字显示
bool ImplDsskeyPerformanceLine::GetLabelPerformance(const DssKeyLogicData & logicData,
        DsskeyLabelData & LabelData)
{
    LabelData.eTextMode = TEXT_MODE_FIRST_ROW;   // 设置显示行数

    if (LINE_MODE_SCA == m_lineMode)
    {
        // SCA文本需要单独获取，因为显示两行文字
        Process_LineSCALabel(LabelData, logicData);
    }

    // 根据优先级获取账号文本
    LabelData.strFirstRowText = acc_GetAccountShowText(logicData.iLineIndex);
    bool bHoteling = false;
    // 判断是否已经登录上Hoteling
    bHoteling = (CallCenter_IsHotelingOn() && HOTELING_STATUS_LOGIN == CallCenter_GetHotelingStatus());
    if (!logicData.strLabel.empty() && !bHoteling)
    {
        // Label不为空且非hoteling登陆状态，使用配置的Label
        LabelData.strFirstRowText = logicData.strLabel;
    }

    // 账号状态不可用时不显示文本
    if ((logicData.iStatus <= DS_LINE_NONE
            || logicData.iStatus >= DS_LINE_END || DS_LINE_DISABLE == logicData.iStatus))
    {
        LabelData.strFirstRowText = "";
    }

#if IF_BUG_25367
    commonUnits_FormatNumber(LabelData.strFirstRowText);
#endif

    return true;
}

// 获取图标显示
bool ImplDsskeyPerformanceLine::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    // 设置扩展台图标
    switch (logicData.iStatus)
    {
    case DS_LINE_UNREGISTERED:
        IconData.eIconType = DK_ICON_UnRegistered;
        break;
    case DS_LINE_REGISTERING:
        IconData.eIconType = DK_ICON_Registering;
        break;
    case DS_LINE_REGISTERED:
        IconData.eIconType = DK_ICON_Registered;
        break;
    case DS_LINE_USING:
        IconData.eIconType = DK_ICON_Using;
        break;
    case DS_LINE_NONE:
    case DS_LINE_DISABLE:
        IconData.eIconType = DK_ICON_UnRegistered;
        break;
    case DS_LINE_FWD:
        IconData.eIconType = DK_ICON_Registered;
        break; //!!!!! FWD和DND需要扩展台图标吗？
    case DS_LINE_DND:
        IconData.eIconType = DK_ICON_Registered;
        break;
    default:
        IconData.eIconType = DK_ICON_None;
        break;
    }

    switch (m_lineMode)
    {
    case LINE_MODE_ACD:
        {
            Process_LineACDIcon(IconData);
        }
        break;
    case LINE_MODE_SCA:
        {
            Process_LineSCAIcon(IconData, logicData);
        }
        break;
    case LINE_MODE_NORMAL:
    default:
        {
            Process_LineNormalIcon(IconData, logicData);
        }
        break;
    }

    Process_LineUCIcon(IconData, logicData);

    if ((logicData.iStatus <= DS_LINE_NONE
            || logicData.iStatus >= DS_LINE_END || DS_LINE_DISABLE == logicData.iStatus))
    {
        IconData.strDownIconPath = "";
        IconData.strIconPath = "";
        IconData.strFlashIcon1 = "";
        IconData.strFlashIcon2 = "";
        IconData.eRefreshType = REFRESH_TYPE_NO_FLASH;
        IconData.eIconType = DK_ICON_None;
    }
    return true;
}

// 获取灯显示
bool ImplDsskeyPerformanceLine::GetLightPerformance(const DssKeyLogicData & logicData,
        DsskeyLightData & LigntData)
{
    switch (logicData.iStatus)
    {
    case DS_LINE_REGISTERED:
        {
            // page状态
            Process_LineSCALight(LigntData, logicData);
        }
        break;
    case DS_LINE_USING:
        {
            if (m_eSCAStatus != DS_NON)
            {
                LigntData.ePageColorType = LED_COLOR_OFF;
                Process_LineSCALight(LigntData, logicData);
            }
            else if (DS_SESSION_NORMAL_USING == m_eTalkStatus
                     || DS_SESSION_TALK == m_eTalkStatus
                     || DS_SESSION_CALL_Out == m_eTalkStatus) // 通话状态暂时和DS_SESSION_NORMAL_USING一样
            {
                // dsskey状态
                LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
            }
            else if (DS_SESSION_HOLD == m_eTalkStatus)
            {
                // dsskey状态
                LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
                LigntData.eLightFlashType = REFRESH_TYPE_FLASH;
                LigntData.ePageColorType = LED_COLOR_TYPE_GREEN;
                LigntData.ePageLightFlashType = REFRESH_TYPE_FLASH;
                LigntData.iLightPriority = DSS_KEY_LED_PRIORITY_HIGH;// 非特殊状态优先级低
            }
            else if (DS_SESSION_CALL_IN == m_eTalkStatus) // T46在来电中常亮红灯
            {
                // dsskey状态
                LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
                LigntData.eLightFlashType = REFRESH_TYPE_FAST_FLASH;
                LigntData.ePageColorType = LED_COLOR_TYPE_GREEN;
                LigntData.ePageLightFlashType = REFRESH_TYPE_FAST_FLASH;
                LigntData.iLightPriority = DSS_KEY_LED_PRIORITY_SECOND_HIGH;
            }
            else if (DS_SESSION_DIAL == m_eTalkStatus)
            {
                // dsskey状态
                LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
            }
        }
        break;
    default:
        break;
    }
    return true;
}

// 获取背景显示
bool ImplDsskeyPerformanceLine::GetBkgroundPerformance(const DssKeyLogicData & logicData,
        DsskeyBkgroundData & BkData)
{
    if (logicData.iStatus == DS_LINE_USING)
    {
        // 设置底色
        BkData.eGroundPhoto = GROUNDPHOTO_MODE_USING;
    }
#if IF_BUG_25487
    else if (_AccountManager.GetDefaultAccountID() == logicData.iLineIndex)
    {
        // 默认账号使用高亮底色
        BkData.eGroundPhoto = GROUNDPHOTO_MODE_HIGHLIGHT;
    }
#endif
    else
    {
        BkData.eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;
    }
    return true;
}


/////////////////////////////private/////////////////////////////
// 处理账号功能的ACD状态图标显示
void ImplDsskeyPerformanceLine::Process_LineACDIcon(DsskeyIconData & IconData)
{
    // 处理ACD状态下的图标
    switch (ACD_GetAcdStatus())
    {
    case AS_IDLE:
        {
            IconData.strIconPath = PIC_DSS_ACD_LOGOUT;
        }
        break;
    case AS_AVAILABLE:
        {
            IconData.strIconPath = PIC_DSS_ACD_AVAILABLE;
        }
        break;
    case AS_UNAVAILABLE:
        {
            IconData.strIconPath = PIC_DSS_ACD_UNAVAILABLE;
        }
        break;
    case AS_WRAPUP:
        {
            IconData.strIconPath = PIC_DSS_ACD_WARP_UP;
        }
        break;
    default:
        {
            IconData.strIconPath = "";
        }
        break;
    }
}


// 处理普通账号功能的图标显示
void ImplDsskeyPerformanceLine::Process_LineNormalIcon(DsskeyIconData & IconData,
        const DssKeyLogicData & logicData)
{
    switch (logicData.iStatus)
    {
    case DS_LINE_UNREGISTERED:
        {
            IconData.strIconPath = PIC_DSS_LINE_UNREGISTERED;
        }
        break;
    case DS_LINE_REGISTERING:
        {
            IconData.strIconPath = PIC_DSS_LINE_REGISTERED;
            IconData.eRefreshType = REFRESH_TYPE_SLOW_FLASH;
        }
        break;
    case DS_LINE_REGISTERED:
        {
            IconData.strIconPath = PIC_DSS_LINE_REGISTERED;
        }
        break;
    case DS_LINE_USING:
        {
            IconData.strIconPath = PIC_DSS_LINE_USING;
        }
        break;
    case DS_LINE_NONE:
    case DS_LINE_DISABLE:
        {
            IconData.strIconPath = PIC_DSS_LINE_FAIL;
        }
        break;
    case DS_LINE_FWD:
        {
            IconData.strIconPath = PIC_DSS_LINE_FORWARD;
        }
        break;
    case DS_LINE_DND:
        {
            IconData.strIconPath = PIC_DSS_LINE_DND;
        }
        break;
    default:
        {
            IconData.strIconPath = PIC_DSS_LINE_UNREGISTERED;
        }
        break;
    }
}

// 处理SCA账号功能的图标显示
void ImplDsskeyPerformanceLine::Process_LineSCAIcon(DsskeyIconData & IconData,
        const DssKeyLogicData & sLogicData)
{

    yl::string strSCAIcon = "";

    IconData.eRefreshType = REFRESH_TYPE_NO_FLASH;

    if (m_eSCAStatus == DS_BLA_STATUS_IDLE)
    {
        IconData.eIconType = DK_ICON_Remote_Online;
        strSCAIcon = PIC_DSS_BLA_IDLE;
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_SEIZED)
    {
        IconData.eIconType = DK_ICON_Remote_Talking;
        strSCAIcon = PIC_DSS_BLA_SEIZED;
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_PROGRESSING)
    {
        IconData.eRefreshType = REFRESH_TYPE_SLOW_FLASH;
        IconData.eIconType = DK_ICON_Remote_Talking;
        strSCAIcon = PIC_DSS_BLA_PROGRESSING;
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_ALERTING)
    {
        IconData.eRefreshType = REFRESH_TYPE_SLOW_FLASH;
        IconData.eIconType = DK_ICON_Remote_Ringing;
        strSCAIcon = PIC_DSS_BLA_ALERTING;
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_ACTIVE)
    {
        IconData.eIconType = DK_ICON_Remote_Talking;
        strSCAIcon = PIC_DSS_BLA_ACTIVE;
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_HELD)
    {
        IconData.eIconType = DK_ICON_Remote_Talking;
        strSCAIcon = PIC_DSS_BLA_HELD;
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_HELD_PRIVATE)
    {
        IconData.eIconType = DK_ICON_Remote_Talking;
        if (m_bSCA_Local)
        {
            strSCAIcon = PIC_DSS_BLA_HELD_PRIVATE;
        }
        else
        {
            strSCAIcon = PIC_DSS_BLA_ACTIVE;
        }
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_BRIDGE_ACITVE)
    {
        IconData.eIconType = DK_ICON_Remote_Talking;
        if (m_bSCA_Hold)
        {
            strSCAIcon = PIC_DSS_BLA_BRIDGE_HELD;
        }
        else
        {
            strSCAIcon = PIC_DSS_BLA_BRIDGE_ACTIVE;
        }
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_BRIDGE_HELD)
    {
        IconData.eIconType = DK_ICON_Remote_Talking;
        if (m_bSCA_Local)
        {
            strSCAIcon = PIC_DSS_BLA_BRIDGE_HELD;
        }
        else
        {
            strSCAIcon = PIC_DSS_BLA_BRIDGE_ACTIVE;
        }
    }
    else if (m_eSCAStatus == DS_BLF_STATUS_PARK)
    {
        IconData.eIconType = DK_ICON_Remote_Offline;
        strSCAIcon = PIC_DSS_BLA_PARK;
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_UNKOWN
             || m_eSCAStatus == DS_NON)
    {
        IconData.eIconType = DK_ICON_Remote_Offline;
        strSCAIcon = PIC_DSS_BLA_UNKNOWN;
    }
    else
    {
        DSSKEYUI_WARN("Dsskey SCA Icon error!");
    }


    if (DS_LINE_FWD == sLogicData.iStatus)
    {
        IconData.strIconPath = PIC_DSS_LINE_FORWARD;
    }
    else if (DS_LINE_DND == sLogicData.iStatus)
    {
        IconData.strIconPath = PIC_DSS_LINE_DND;
    }
    else
    {
        IconData.strIconPath = strSCAIcon;
    }
}

// 处理SCA账号功能的文本显示
void ImplDsskeyPerformanceLine::Process_LineSCALabel(DsskeyLabelData & LabelData,
        const DssKeyLogicData & sLogicData)
{
    bool bExt = GetExtendFlag();
    if (m_eSCAStatus == DS_BLA_STATUS_IDLE)
    {

        LabelData.strFirstRowText = sLogicData.strLabel.empty() ?
                                    (LANG_TRANSLATE_DSSKEY_LABEL(TRID_SHARE_LINE, bExt)).toUtf8().data() : sLogicData.strLabel;

        //  LabelData.strSecRowText = sLogicData.strValue;
        LabelData.eTextMode = TEXT_MODE_FIRST_ROW;
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_SEIZED)
    {
        LabelData.strFirstRowText = sLogicData.strLabel.empty() ?
                                    sLogicData.strValue : sLogicData.strLabel;
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_SEIZING, bExt)).toUtf8().data();
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_PROGRESSING)
    {
        LabelData.strFirstRowText = sLogicData.strLabel.empty() ?
                                    sLogicData.strValue : sLogicData.strLabel;
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_CALLING, bExt)).toUtf8().data();
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_ALERTING)
    {
        LabelData.strFirstRowText = sLogicData.strLabel.empty() ?
                                    sLogicData.strValue : sLogicData.strLabel;
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_RINGING, bExt)).toUtf8().data();
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_ACTIVE)
    {
        LabelData.strFirstRowText = sLogicData.strLabel.empty() ?
                                    sLogicData.strValue : sLogicData.strLabel;
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_TALKING, bExt)).toUtf8().data();
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_HELD)
    {
        LabelData.strFirstRowText = sLogicData.strLabel.empty() ?
                                    sLogicData.strValue : sLogicData.strLabel;
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_PUBLIC_HOLD, bExt)).toUtf8().data();
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_HELD_PRIVATE)
    {
        LabelData.strFirstRowText = sLogicData.strLabel.empty() ?
                                    sLogicData.strValue : sLogicData.strLabel;
        if (m_bSCA_Local)
        {
            LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_RRIVATE_HOLD, bExt)).toUtf8().data();
        }
        else
        {
            LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_TALKING, bExt)).toUtf8().data();
        }
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_BRIDGE_ACITVE)
    {
        LabelData.strFirstRowText = sLogicData.strLabel.empty() ?
                                    sLogicData.strValue : sLogicData.strLabel;
        if (m_bSCA_Hold)
        {
            LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_PUBLIC_HOLD, bExt)).toUtf8().data();
        }
        else
        {
            LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_TALKING, bExt)).toUtf8().data();
        }

        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_BRIDGE_HELD)
    {
        LabelData.strFirstRowText = sLogicData.strLabel.empty() ?
                                    sLogicData.strValue : sLogicData.strLabel;
        if (m_bSCA_Local)
        {
            LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_PUBLIC_HOLD, bExt)).toUtf8().data();
        }
        else
        {
            LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_TALKING, bExt)).toUtf8().data();
        }
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
    }
    else if (m_eSCAStatus == DS_BLF_STATUS_PARK)
    {
        LabelData.strFirstRowText = sLogicData.strLabel.empty() ?
                                    sLogicData.strValue : sLogicData.strLabel;
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_PARK_CALL, bExt)).toUtf8().data();
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
    }
    else if (m_eSCAStatus == DS_BLA_STATUS_UNKOWN
             || m_eSCAStatus == DS_NON)
    {
        LabelData.strFirstRowText = sLogicData.strLabel.empty() ?
                                    sLogicData.strValue : sLogicData.strLabel;
        LabelData.eTextMode = TEXT_MODE_FIRST_ROW;
    }
    else
    {
        DSSKEYUI_WARN("Dsskey SCA Label error!");
    }
}

// 处理账号功能的SCA状态灯显示
void ImplDsskeyPerformanceLine::Process_LineSCALight(DsskeyLightData & LightData,
        const DssKeyLogicData & sLogicData)
{
    bool bHaveSession = false;
    bool bHold = false;

    CSessionProxy proxy = talk_GetSessionByDsskey(sLogicData.iDssKeyID);
    if (proxy && proxy.IsSingle())
    {
        bHaveSession = proxy.GetSessionState() != SESSION_PARK_NOTIFY;
        bHold = talklogic_IsHoldSession(proxy);
    }

    switch (m_eSCAStatus)
    {
    case DS_BLA_STATUS_ALERTING:
        {
            LightData.eLightColorType = LED_COLOR_TYPE_GREEN;
            LightData.eLightFlashType = REFRESH_TYPE_FAST_FLASH;
            // 考虑有翻页键灯的情况
            LightData.ePageColorType = LED_COLOR_TYPE_GREEN;
            LightData.ePageLightFlashType = REFRESH_TYPE_FAST_FLASH;
            LightData.iLightPriority = DSS_KEY_LED_PRIORITY_SECOND_HIGH;
        }
        break;
    case DS_BLA_STATUS_SEIZED:
    case DS_BLA_STATUS_PROGRESSING:
    case DS_BLA_STATUS_ACTIVE:
        {
            LightData.eLightColorType = bHaveSession ? LED_COLOR_TYPE_GREEN : LED_COLOR_TYPE_RED;
        }
        break;
    case DS_BLA_STATUS_BRIDGE_ACITVE:
        {
            LightData.eLightColorType = bHaveSession ? LED_COLOR_TYPE_GREEN : LED_COLOR_TYPE_RED;
            LightData.eLightFlashType = bHold ? REFRESH_TYPE_SLOW_FLASH : REFRESH_TYPE_NO_FLASH;
            // 考虑有翻页键灯的情况
            if (bHaveSession && bHold)
            {
                LightData.ePageColorType = LightData.eLightColorType;
                LightData.ePageLightFlashType = LightData.eLightFlashType;
                LightData.iLightPriority = DSS_KEY_LED_PRIORITY_HIGH; // 非特殊状态优先级低
            }
        }
        break;
    case DS_BLA_STATUS_BRIDGE_HELD:
        {
            LightData.eLightColorType = bHaveSession ? LED_COLOR_TYPE_GREEN : LED_COLOR_TYPE_RED;
            LightData.eLightFlashType = bHaveSession ? REFRESH_TYPE_SLOW_FLASH : REFRESH_TYPE_NO_FLASH;
            // 考虑有翻页键灯的情况
            if (bHaveSession)
            {
                LightData.ePageColorType = LightData.eLightColorType;
                LightData.ePageLightFlashType = LightData.eLightFlashType;
                LightData.iLightPriority = DSS_KEY_LED_PRIORITY_HIGH; // 非特殊状态优先级低
            }
        }
        break;
    case DS_BLA_STATUS_HELD:
        {
            LightData.eLightColorType = bHaveSession ? LED_COLOR_TYPE_GREEN : LED_COLOR_TYPE_RED;
            LightData.eLightFlashType = REFRESH_TYPE_SLOW_FLASH;
            // 考虑有翻页键灯的情况
            if (bHaveSession)
            {
                LightData.ePageColorType = LightData.eLightColorType;
                LightData.ePageLightFlashType = LightData.eLightFlashType;
                LightData.iLightPriority = DSS_KEY_LED_PRIORITY_HIGH; // 非特殊状态优先级低
            }
        }
        break;
    case DS_BLA_STATUS_HELD_PRIVATE:
        {
            LightData.eLightColorType = bHaveSession ? LED_COLOR_TYPE_GREEN : LED_COLOR_TYPE_RED;
            LightData.eLightFlashType = bHaveSession ? REFRESH_TYPE_SLOW_FLASH : REFRESH_TYPE_NO_FLASH;
            // 考虑有翻页键灯的情况
            if (bHaveSession)
            {
                LightData.ePageColorType = LightData.eLightColorType;
                LightData.ePageLightFlashType = LightData.eLightFlashType;
                LightData.iLightPriority = DSS_KEY_LED_PRIORITY_HIGH; // 非特殊状态优先级低
            }
        }
        break;
    case DS_BLA_STATUS_IDLE:
    case DS_BLA_STATUS_UNKOWN:
        {
            LightData.eLightColorType = LED_COLOR_OFF;
        }
        break;
    case DS_BLF_STATUS_PARK:
        {
            LightData.eLightColorType = LED_COLOR_TYPE_RED;
            LightData.eLightFlashType = REFRESH_TYPE_SLOW_FLASH;
        }
        break;
    default:
        break;
    }

    DSSKEYUI_INFO("SCA DsskeyID[%d] Color[%d], Flash[%d]", sLogicData.iDssKeyID,
                  LightData.eLightColorType, LightData.eLightFlashType);
}

void ImplDsskeyPerformanceLine::Process_LineUCIcon(DsskeyIconData & IconData,
        const DssKeyLogicData & sLogicData)
{
    PresenceInfo presenceInfo = Presence_GetLocalPresence();
    DSSKEYUI_INFO("iLineIndex(%d) ePresenceType(%d) strAvatar(%s)", \
                  sLogicData.iLineIndex, presenceInfo.ePresenceType, presenceInfo.strAvatarInfo.c_str());
    if (configParser_GetConfigInt(kszEnableUC) == 1
            && configParser_GetConfigInt(kszBroadsoftActive) == 1
            && sLogicData.iLineIndex == 0
            && presenceInfo.ePresenceType != _PRESENCE_TYPE_NONE)
    {
#if IF_FEATURE_32591
        // DT新需求，Linekey上的UC头像和圆点状态都不需要显示
        bool bUserHigherPriorityIcon = true;
#else
        bool bUserHigherPriorityIcon = false;
#endif
        // UC在线,并且账号可用的时候才显示头像
        if (sLogicData.iStatus == DS_LINE_REGISTERED || sLogicData.iStatus == DS_LINE_USING
                || bUserHigherPriorityIcon)
        {
            //broadsoft UC开的时候第一个linekey设置头像和presence状态图标
            yl::string strAvatar =  presenceInfo.strAvatarInfo;
            //UC账号注册成功才显示Presence的头像
            if (!strAvatar.empty() && !bUserHigherPriorityIcon)//连接失败或者为空都不设置
            {
                strAvatar = TEMP_PATH + strAvatar;
                IconData.strIconPath  =  strAvatar;
            }
#if IF_FEATURE_32591
            else if (IconData.strIconPath.empty())
#else
            else
#endif
            {
                IconData.strIconPath = PIC_DSS_LINE;
            }
            _PRESENCE_TYPE eStatus = presenceInfo.ePresenceType;

            //是否显示新状态图标
            bool bNewIconMode = configParser_GetConfigInt(kszPresenceIconMode) == 1;

            switch (eStatus)
            {
            case _PRESENCE_TYPE_ONLINE:
                IconData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_ONLINE : PIC_ICON_PRESENCE_ONLINE;
                break;
            case _PRESENCE_TYPE_CHATTY:
                IconData.strPresenceIcon = PIC_ICON_PRESENCE_CHATTY;
                break;
            case _PRESENCE_TYPE_AWAY:
                IconData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_AWAY : PIC_ICON_PRESENCE_AWAY;
                break;
            case _PRESENCE_TYPE_EXTENDED_AWAY:
                IconData.strPresenceIcon = PIC_ICON_PRESENCE_EXTENDEDAWAY;
                break;
            case _PRESENCE_TYPE_BUSY:
                IconData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_BUSY : PIC_ICON_PRESENCE_BUSY;
                break;
            case _PRESENCE_TYPE_OFFLINE:
                IconData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_OFFLINE : PIC_ICON_PRESENCE_OFFLINE;
                break;
            case _PRESENCE_TYPE_UNKNOWN:
                IconData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_OFFLINE : PIC_ICON_PRESENCE_OFFLINE;
                break;
            default:
                IconData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_OFFLINE : PIC_ICON_PRESENCE_OFFLINE;
                break;
            }

#if IF_FEATURE_32591
            IconData.strPresenceIcon.clear();
#endif
        }
    }
}

///////////////////////////////////////
