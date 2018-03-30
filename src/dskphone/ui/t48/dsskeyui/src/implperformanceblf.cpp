#include "implperformanceblf.h"
#include "baseui/t4xpicpath.h"
#include "dsskeyperformancemanager.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "blf/include/modblf.h"
#include "dsskey/include/moddsskey.h"
#include "dsskey/src/dsskey_common.h"

#define  MAX_BUF_LEN            256

////////////////Dsskey会话相关类型的基类/////////////////////////
ImplDsskeyPerformanceBLFBase::ImplDsskeyPerformanceBLFBase(DssKey_Type type)
    : ImplDsskeyPerformanceBase(type)
{

}

ImplDsskeyPerformanceBLFBase::~ImplDsskeyPerformanceBLFBase()
{

}

// 获取灯显示
bool ImplDsskeyPerformanceBLFBase::GetLightPerformance(const DssKeyLogicData & logicData,
        DsskeyLightData & LigntData)
{
    int iBLFLedMode = configParser_GetConfigInt(kszBLFLedMode);

    DialgInfo sDiaLogInfo;
    blf_GetDialogInfoByDsskeyId(sDiaLogInfo, logicData.iDssKeyID, BLF_Manager);

    LigntData.eLightColorType = LED_COLOR_OFF;
    if (logicData.iStatus == DS_BLF_STATUS_IDLE)
    {
        if (0 == iBLFLedMode
                && sDiaLogInfo.m_nMode == 0) // blf的特殊状态
        {
            LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
        }
    }
    else if (logicData.iStatus == DS_BLF_STATUS_RINGING)
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
    }
    else if (logicData.iStatus == DS_BLF_STATUS_CALLOUT)
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
    else if (logicData.iStatus == DS_BLF_STATUS_TALKING)
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
    else if (logicData.iStatus == DS_BLF_STATUS_HOLD)
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
    else if (logicData.iStatus == DS_BLF_STATUS_PARK)
    {
        LigntData.eLightColorType = LED_COLOR_TYPE_RED;
        LigntData.eLightFlashType = REFRESH_TYPE_SLOW_FLASH;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_UNKOWN || logicData.iStatus == DS_NON)
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
    else if (logicData.iStatus == DS_BLF_STATUS_DND)
    {
        LigntData.eLightColorType = LED_COLOR_TYPE_RED;
        LigntData.eLightFlashType = REFRESH_TYPE_SLOW_FLASH;
    }
    else //有通话状态，包含有来电、通话中、未知、hold
    {
        DSSKEYUI_WARN("Dsskey BLF Status(%d) error!", logicData.iStatus);
    }

    return true;
}

// 获取文字显示
bool ImplDsskeyPerformanceBLFBase::GetLabelPerformance(const DssKeyLogicData & logicData,
        DsskeyLabelData & LabelData)
{
    bool bExt = GetExtendFlag();
    bool bRet = true;
    //监控号码没有通话状态
    if (logicData.iStatus == DS_BLF_STATUS_IDLE)
    {
        LabelData.eTextMode = TEXT_MODE_FIRST_ROW;

        char szBuffer[MAX_BUF_LEN] = "";
        if (logicData.strLabel.empty())
        {
            sprintf(szBuffer, "%s:%s", (LANG_TRANSLATE_DSSKEY_LABEL(TRID_BLF, bExt)).toUtf8().data(),
                    logicData.strValue.c_str());
            szBuffer[MAX_BUF_LEN - 1] = '\0';
            LabelData.strFirstRowText = szBuffer;
        }
        else
        {
            LabelData.strFirstRowText = logicData.strLabel;
        }

    }
    else if (logicData.iStatus == DS_BLF_STATUS_RINGING)
    {
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
        // 无Label时，第一行默认是显示号码
        LabelData.strFirstRowText = logicData.strLabel.empty() ?
                                    logicData.strValue : logicData.strLabel;
#if IF_BUG_34339
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_BLF_RINGING, bExt)).toUtf8().data();
#else
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_RINGING, bExt)).toUtf8().data();
#endif
    }
    else if (logicData.iStatus == DS_BLF_STATUS_CALLOUT)
    {
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
        LabelData.strFirstRowText = logicData.strLabel.empty() ?
                                    logicData.strValue : logicData.strLabel;
#if IF_BUG_34339
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_BLF_CALL_OUT, bExt)).toUtf8().data();
#else
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_CALL_OUT, bExt)).toUtf8().data();
#endif
    }
    else if (logicData.iStatus == DS_BLF_STATUS_TALKING)
    {
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
        LabelData.strFirstRowText = logicData.strLabel.empty() ?
                                    logicData.strValue : logicData.strLabel;
#if IF_BUG_34339
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_BLF_TALKING, bExt)).toUtf8().data();
#else
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_TALKING, bExt)).toUtf8().data();
#endif
    }
    else if (logicData.iStatus == DS_BLF_STATUS_HOLD)
    {
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
        LabelData.strFirstRowText = logicData.strLabel.empty() ?
                                    logicData.strValue : logicData.strLabel;
#if IF_BUG_34339
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_BLF_HOLD, bExt)).toUtf8().data();
#else
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_HOLD, bExt)).toUtf8().data();
#endif
    }
    else if (logicData.iStatus == DS_BLF_STATUS_PARK)
    {
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
        LabelData.strFirstRowText = logicData.strLabel.empty() ?
                                    logicData.strValue : logicData.strLabel;
#if IF_BUG_34339
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_BLF_PARK_CALL, bExt)).toUtf8().data();
#else
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_PARK_CALL, bExt)).toUtf8().data();
#endif
    }
    else if (logicData.iStatus == DS_BLF_STATUS_UNKOWN || logicData.iStatus == DS_NON)
    {
        LabelData.eTextMode = TEXT_MODE_FIRST_ROW;
        LabelData.strFirstRowText = logicData.strLabel.empty() ?
                                    logicData.strValue : logicData.strLabel;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_DND)
    {
        LabelData.eTextMode = TEXT_MODE_SEC_ROW;
        LabelData.strFirstRowText = logicData.strLabel.empty() ?
                                    logicData.strValue : logicData.strLabel;
        LabelData.strSecRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_DND, bExt)).toUtf8().data();
    }
    else //有通话状态，包含有来电、通话中、未知、hold
    {
        DSSKEYUI_WARN("Dsskey BLF status error!");
        bRet = false;
    }

    // 自行配制BLFlist，无法配置value的情况
    if (LabelData.strFirstRowText.empty())
    {
        LabelData.strFirstRowText = (LANG_TRANSLATE_DSSKEY_LABEL(dsskey_GetDefaultLabel(
                                         logicData.eDKType).c_str(), bExt)).toUtf8().data();
    }
    return bRet;
}

// 获取图标显示
bool ImplDsskeyPerformanceBLFBase::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    //监控号码没有通话状态
    if (logicData.iStatus == DS_BLF_STATUS_IDLE)
    {
        IconData.eIconType = DK_ICON_BLF_IDLE;
        IconData.strIconPath = PIC_DSS_BLF_IDLE;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_RINGING)
    {
        IconData.eRefreshType = REFRESH_TYPE_SLOW_FLASH;
        IconData.eIconType = DK_ICON_BLF_RINGING;
        IconData.strIconPath = PIC_DSS_BLF_RING;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_CALLOUT)
    {
        IconData.eRefreshType = REFRESH_TYPE_SLOW_FLASH;
        IconData.eIconType = DK_ICON_BLF_CALLOUT;
        IconData.strIconPath = PIC_DSS_BLF_CALLOUT;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_TALKING)
    {
        IconData.eIconType = DK_ICON_BLF_TALK;
        IconData.strIconPath = PIC_DSS_BLF_TALKING;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_HOLD)
    {
        IconData.eIconType = DK_ICON_BLF_HOLD;
        IconData.strIconPath = PIC_DSS_BLF_HOLD;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_PARK)
    {
        IconData.eIconType = DK_ICON_BLF_PARK;
        IconData.strIconPath = PIC_DSS_BLF_PARK;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_UNKOWN || logicData.iStatus == DS_NON)
    {
        IconData.eIconType = DK_ICON_BLF_UNKNOWN;
        IconData.strIconPath = PIC_DSS_BLF_UNKNOWN;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_DND)
    {
        IconData.eRefreshType = REFRESH_TYPE_SLOW_FLASH;
        IconData.eIconType = DK_ICON_BLF_TALK;
        IconData.strIconPath = PIC_DSS_BLF_TALKING;
    }
    else //有通话状态，包含有来电、通话中、未知、hold
    {
        DSSKEYUI_WARN("Dsskey BLF status error!");
    }

    return true;
}
