#include "implperformanceintercom.h"
#include "baseui/t4xpicpath.h"
#include "dsskeyperformancemanager.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "blf/include/modblf.h"

enum BLF_LED_TYPE
{
    BLF_LED_DEFAULT = 0,        //0：默认出厂为?0，未修改过任何状态的默认状态指示逻辑（对应于features.blf_led_mode和features.blf_and_callpark_idle_led_enable配置项都为0的情况）；
    BLF_LED_MODE,               //1：对应原有配置项features.blf_led_mode=1时的指示灯逻辑状态模式；
    BLF_LED_BLF_AND_PARK_IDLE,  //2：对应原有配置项features.blf_and_callpark_idle_led_enable=1时的指示灯逻辑状态模式；
    BLF_LED_BROADSOFT,          //3：新增逻辑，对应于Broadsoft官方说明的指示灯逻辑状态模式。在Broadsoft UC one版本中默认出厂配置值即为?3；
};

////////////////Dsskey会话相关类型的基类/////////////////////////
ImplDsskeyPerformanceIntercom::ImplDsskeyPerformanceIntercom()
    : ImplDsskeyPerformanceBase(DT_INTERCOM)
{

}

ImplDsskeyPerformanceIntercom::~ImplDsskeyPerformanceIntercom()
{

}
// 获取文字显示
bool ImplDsskeyPerformanceIntercom::GetLabelPerformance(const DssKeyLogicData & logicData,
        DsskeyLabelData & LabelData)
{
    bool bRet = true;
    bool bExt = GetExtendFlag();

    if (logicData.iStatus == DS_BLF_STATUS_IDLE)
    {
        // 显示文本外部已做处理
        LabelData.eTextMode = TEXT_MODE_FIRST_ROW;
        bRet = false;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_RINGING)
    {
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_RINGING, bExt)).toUtf8().data();
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_TALKING)
    {
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_TALKING, bExt)).toUtf8().data();
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_CALLOUT)
    {
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_CALL_OUT, bExt)).toUtf8().data();
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_HOLD)
    {
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_HOLD, bExt)).toUtf8().data();
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_UNKOWN)
    {
        LabelData.eTextMode = TEXT_MODE_FIRST_ROW;
    }
    else //有通话状态，包含有来电、通话中、未知、hold
    {
        LabelData.eTextMode = TEXT_MODE_FIRST_ROW;
        bRet = false;
    }

    if (bRet)
    {
        LabelData.strFirstRowText = logicData.strLabel.empty() ? (LANG_TRANSLATE_DSSKEY_LABEL(TRID_INTERCOM,
                                    bExt)).toUtf8().data()
                                    : logicData.strLabel;
    }

    return bRet;
}
// 获取灯显示
bool ImplDsskeyPerformanceIntercom::GetLightPerformance(const DssKeyLogicData & logicData,
        DsskeyLightData & LigntData)
{
    int iBLFLedMode = configParser_GetConfigInt(kszBLFLedMode);

    DialgInfo sDiaLogInfo;

    blf_GetDialogInfoByDsskeyId(sDiaLogInfo, logicData.iDssKeyID, BLF_Manager);

    LigntData.eLightColorType = LED_COLOR_OFF;
    switch (logicData.iStatus)
    {
    case DS_BLF_STATUS_IDLE:
        {
            if (iBLFLedMode == 0
                    && sDiaLogInfo.m_nMode == 0) // blf的特殊状态
            {
                // 没开特殊配置则idle下亮绿灯
                LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
            }
        }
        break;
    case DS_BLF_STATUS_CALLOUT:
        {
            if (sDiaLogInfo.m_nMode == 2)
            {
                LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
            }
            else
            {
                LigntData.eLightColorType = LED_COLOR_TYPE_RED;
            }
        }
        break;
    case DS_BLF_STATUS_RINGING:
        {
            if (3 == iBLFLedMode)
            {
                // 绿灯快闪
                LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
                LigntData.eLightFlashType = REFRESH_TYPE_FASTER_FLASH;
            }
            else
            {
                if (sDiaLogInfo.m_nMode == 2)
                {
                    // 绿灯普通闪烁
                    LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
                    LigntData.eLightFlashType = REFRESH_TYPE_FLASH;
                }
                else
                {
                    // 红灯快闪
                    LigntData.eLightColorType = LED_COLOR_TYPE_RED;
                    LigntData.eLightFlashType = REFRESH_TYPE_FASTER_FLASH;
                }
            }

            LigntData.iLightPriority = DSS_KEY_LED_PRIORITY_HIGH;
            // page灯
            LigntData.ePageColorType = LED_COLOR_TYPE_RED;
            // page灯闪烁类型
            LigntData.ePageLightFlashType = REFRESH_TYPE_FAST_FLASH;
        }
        break;
    case DS_BLF_STATUS_TALKING:
        {
            if (4 == iBLFLedMode)
            {
                LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
            }
            else if (sDiaLogInfo.m_nMode == 2)
            {
                LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
            }
            else
            {
                LigntData.eLightColorType = LED_COLOR_TYPE_RED;
            }
        }
        break;
    case DS_BLF_STATUS_HOLD:
        {
            if (3 == iBLFLedMode)
            {
                LigntData.eLightColorType = LED_COLOR_TYPE_RED;
            }
            else
            {
                // 红灯慢闪
                LigntData.eLightColorType = LED_COLOR_TYPE_RED;
                LigntData.eLightFlashType = REFRESH_TYPE_SLOW_FLASH;
            }
        }
        break;
    case DS_BLF_STATUS_PARK:
        {
            LigntData.eLightColorType = LED_COLOR_TYPE_RED;
            LigntData.eLightFlashType = REFRESH_TYPE_SLOW_FLASH;
        }
        break;
    case DS_BLF_STATUS_UNKOWN:
        {
            if (4 == iBLFLedMode)
            {
                LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
                LigntData.eLightFlashType = REFRESH_TYPE_SLOW_FLASH;
            }
            else
            {
                LigntData.eLightColorType = LED_COLOR_OFF;
            }
        }
        break;
    default:
        {
            LigntData.eLightColorType = LED_COLOR_OFF;
        }
        break;
    }

    return true;
}

// 获取图标显示
bool ImplDsskeyPerformanceIntercom::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    DSSKEYUI_INFO("Intercom status is %d.", logicData.iStatus);
    if (0 == configParser_GetConfigInt(kszSubscribeIntercom))
    {
        IconData.eIconType = DK_ICON_Remote_Online;
        IconData.strIconPath = PIC_DSS_INTERCOM;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_IDLE)
    {
        IconData.eIconType = DK_ICON_Remote_Online;
        IconData.strIconPath = PIC_DSS_INTERCOM;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_RINGING)
    {
        IconData.eIconType = DK_ICON_Remote_Ringing;
        IconData.strIconPath = PIC_DSS_INTERCOM_RINGING;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_TALKING || logicData.iStatus == DS_BLF_STATUS_CALLOUT)
    {
        IconData.eIconType = DK_ICON_Remote_Talking;
        IconData.strIconPath = PIC_DSS_INTERCOM_TALKING;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_HOLD)
    {
        IconData.eIconType = DK_ICON_Remote_Talking;
        IconData.strIconPath = PIC_DSS_INTERCOM_HOLD;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_UNKOWN || logicData.iStatus == DS_NON)
    {
        IconData.eIconType = DK_ICON_Remote_Offline;
        IconData.strIconPath = PIC_DSS_INTERCOM_UNKNOWN;
    }
    else //有通话状态，包含有来电、通话中、未知、hold
    {
        IconData.eIconType = DK_ICON_Remote_Offline;
        IconData.strIconPath = PIC_DSS_INTERCOM_UNKNOWN;
        DSSKEYUI_WARN("Dsskey BLF status error!");
    }
    return true;
}
