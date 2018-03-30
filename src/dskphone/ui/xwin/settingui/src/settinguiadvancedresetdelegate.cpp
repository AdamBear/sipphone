#include "settinguiadvancedresetdelegate.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "messagebox/modmessagebox.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/include/settinguicommondata.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "modsettingui.h"
#include "service_system.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "commonunits/modcommonunits.h"
#include "autopcontrol/include/modautopcontrol.h"

namespace
{
#define MSGBOX_ID_RESET_LOCAL_SETTINGS 0
#define MSGBOX_ID_RESET_WEB_SETTINGS 1
#define MSGBOX_ID_RESET_STATIC_SETTINGS 3
#define MSGBOX_ID_RESET_NON_STATIC_SETTINGS 4
#define MSGBOX_ID_RESET_USERDATA_AND_CONFIG 5
#define MSGBOX_ID_RESET_FACTORY 6
#define MSGBOX_ID_RET_END 7
// 延迟60s
#define     RESET_DELAY_TIME        60000
}

CSettingUIAdvancedResetDelegate::CSettingUIAdvancedResetDelegate()
    : m_nID(-1)
{

}

CSettingUIAdvancedResetDelegate::~CSettingUIAdvancedResetDelegate()
{

}

CSettingUIDelegateBase * CSettingUIAdvancedResetDelegate::CreateAdvancedResetDelegate()
{
    CSettingUIAdvancedResetDelegate * pResetDelegate = new CSettingUIAdvancedResetDelegate;

    return pResetDelegate;
}

bool CSettingUIAdvancedResetDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /* = false */)
{
    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);

    if (NULL != pPagedata)
    {
#if IF_BUG_21893
        bool bShowWeb = true;
        bool bShow = true;
        if (1 == configParser_GetConfigInt(kszAutoProvisionCustomProtect))
        {
            if (0 == configParser_GetConfigInt(kszAutopSeparateWebConfig))
            {
                bShowWeb = false;
            }
            bShow = true;
        }
        else
        {
            bShow = false;
            bShowWeb = false;
        }

        SetItemShow(pPagedata, RESETLOCALCONFIG, bShow);
        SetItemShow(pPagedata, RESETWEBCONFIG, bShowWeb);
        SetItemShow(pPagedata, RESETNONSTATICCONFIG, bShow);
        SetItemShow(pPagedata, RESETSTATICCONFIG, bShow);
        SetItemShow(pPagedata, RESET_USERDATA_USERCFG, bShow);
#endif
    }

    return bResult;
}

bool CSettingUIAdvancedResetDelegate::ProcessAction(const yl::string strAction, int iDataIndex)
{
    // 重置
    if (0 == strAction.compare(NS_SET::kszActionReset))
    {
        if (CheckTalking())
        {
            return true;
        }

        yl::string strTitle = "";
        yl::string strNote = "";
        int nID = -1;
        yl::string strItemID = GetItemIDByDataIndex(iDataIndex);

        if (GetResetInfoByItemID(strItemID, nID, strTitle, strNote))
        {
            g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, strTitle, strNote,
                                                   SETTING_MESSAGEBOX_OkCancel, "", "", -1, nID);
        }

        return true;
    }

    return CSettingUIDelegateBase::ProcessAction(strAction, iDataIndex);
}

bool CSettingUIAdvancedResetDelegate::ProcessMsgCallBack(int nResult, int nID,
        void * pExtraData/* = NULL*/)
{
    if (SETTING_MessageBox_OK == nResult)
    {
        if (FACSTATE_ENABLE != commUnits_GetResetFactoryState())
        {
            g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, "", _LANG(TRID_TESTMODE_FAIL_NA),
                                                   SETTING_MESSAGEBOX_NOTE, "", "", SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME);
            return false;
        }

        if (MSGBOX_ID_RESET_LOCAL_SETTINGS == nID
                || MSGBOX_ID_RESET_WEB_SETTINGS == nID
                || MSGBOX_ID_RESET_NON_STATIC_SETTINGS == nID
                || MSGBOX_ID_RESET_STATIC_SETTINGS == nID)
        {
            // http://bugfree.yealink.com//Bug.php?BugID=108653
            if (AUTOP_POWER_ON_END == Autop_GetAutopStatus())
            {
                yl::string strNote = GetResttingStringByID(nID);
                if (!strNote.empty())
                {
                    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, "", strNote,
                                                           SETTING_MESSAGEBOX_NOTE, "", "",
                                                           SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME, MSGBOX_ID_RET_END);
                }
                m_nID = nID;
                g_SettingMsgBoxMgr.AddAsyncOperate(this);
            }
            else
            {
                g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, "", _LANG(TRID_RESET_FAIL_PLEASE_RETRY_LATER),
                                                       SETTING_MESSAGEBOX_NOTE, "", "",
                                                       SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME);
            }
            return true;
        }
        else if (MSGBOX_ID_RESET_USERDATA_AND_CONFIG == nID)
        {
            etl_NotifyApp(true, SYS_MSG_RESET_FACTORY, 0, RESET_OPTION_USERDATA_USERCFG);
            return true;
        }
        else if (MSGBOX_ID_RESET_FACTORY == nID)
        {
            SettingUI_Reset(RESET_OPTION_ALL);
            return true;
        }
    }
    else if (MSGBOX_ID_RET_END == nID && SETTING_MessageBox_TimeOut == nResult)
    {
        g_SetUICallBackFun.FallBackToPrevousPage();
        return true;
    }

    return CSettingUIDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

bool CSettingUIAdvancedResetDelegate::GetResetInfoByItemID(const yl::string & strItemID,
        int & nMsgBoxID, yl::string & strTitle, yl::string & strNote)
{
    if (RESETLOCALCONFIG == strItemID)
    {
        strNote = _LANG(TRID_IF_RESET_LOCAL_SETTING);
        nMsgBoxID = MSGBOX_ID_RESET_LOCAL_SETTINGS;
    }
    else if (RESETWEBCONFIG == strItemID)
    {
        strNote = _LANG(TRID_IF_RESET_WEB_SETTING);
        nMsgBoxID = MSGBOX_ID_RESET_WEB_SETTINGS;
    }
    else if (RESETNONSTATICCONFIG == strItemID)
    {
        strNote = _LANG(TRID_IF_RESET_NDEVICE_SETTING);
        nMsgBoxID = MSGBOX_ID_RESET_NON_STATIC_SETTINGS;
    }
    else if (RESETSTATICCONFIG == strItemID)
    {
        strNote = _LANG(TRID_IF_RESET_DEVICE_SETTING);
        nMsgBoxID = MSGBOX_ID_RESET_STATIC_SETTINGS;
    }
    else if (RESET_USERDATA_USERCFG == strItemID)
    {
        strNote = _LANG(TRID_IF_RESET_USERDATA_LOCALCONFIG);
        nMsgBoxID = MSGBOX_ID_RESET_USERDATA_AND_CONFIG;
    }
    else if (RESETTOFACTORY == strItemID)
    {
#if IF_BUG_23072_TRANSLATE
        strTitle = _LANG(TRID_T20_TIP_RESET_TO_FACTORY_SETTINGS_PART1);
        strNote = _LANG(TRID_T20_TIP_RESET_TO_FACTORY_SETTINGS_PART2);
#else
        strNote = _LANG(TRID_IF_RESET_TO_FACTORY_SETTING);
#endif
        nMsgBoxID = MSGBOX_ID_RESET_FACTORY;
    }
    else
    {
        return false;
    }

    return true;
}

yl::string CSettingUIAdvancedResetDelegate::GetResttingStringByID(int nID)
{
    yl::string strNote = "";
    if (MSGBOX_ID_RESET_LOCAL_SETTINGS == nID)
    {
        strNote = _LANG(TRID_RESETLOCAL_PLEASE_WAIT);
    }
    else if (MSGBOX_ID_RESET_WEB_SETTINGS == nID)
    {
        strNote = _LANG(TRID_RESETWEB_PLEASE_WAIT);
    }
    else if (MSGBOX_ID_RESET_NON_STATIC_SETTINGS == nID)
    {
        strNote = _LANG(TRID_RESETNDEVICE_PLEASE_WAIT);
    }
    else if (MSGBOX_ID_RESET_STATIC_SETTINGS == nID)
    {
        strNote = _LANG(TRID_RESETDEVICE_PLEASE_WAIT);
    }

    return strNote;
}

void CSettingUIAdvancedResetDelegate::AsyncOperateCallBack()
{
    msgSendMsgToThreadTimeoutEx(msgGetThreadByName(ATP_NAMAGE_NAME), ATP_MSG_REQ_CLEAR_CFG,
                                m_nID, 0, 0, NULL, RESET_DELAY_TIME);
    m_nID = -1;
}
