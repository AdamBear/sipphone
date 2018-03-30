#include "moddsskeyui.h"
#include "clinekeyui.h"
#include "dsskeyuilogic.h"
#include "cdsskeylightlogic.h"
#include "baseui/include/t2xpicpath.h"
#include "account/include/modaccount.h"
#include "account/include/accountmanager.h"
#include "talk/talklogic/src/talklogic_inc.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/broadsoftsca/include/modbroadsoft_sca.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "presence/include/modpresence.h"
#include "../uimanager/uimanager_inc.h"
#include "../settingui/include/modsettingui.h"
#include "uimanager/basedialog.h"
#include "dsskey/include/moddsskey.h"
#include "exp/include/modexp.h"
#include "dsskey/src/dsskeycommonfunc.h"
#include "dsskey/src/dsskey_common.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "contacts/directory/include/directorymessagedefine.h"
#include "idleuilogic/include/modidleul.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "edk/include/modedk.h"
#include "settingui/include/pageiddefine.h"

#include "dsskeyuilogic/include/moddsskeyuilogic.h"
#include "settinguilogic/include/messagedefine.h"
#include "language/input/include/modInput.h"
#include "languagehelper/modlangtranslate.h"

// testmode
#include "testmode/include/modtestmode.h"

#define  MAX_BUF_LEN            256

#define  SHORTEN_DSSKEY_CHARATER_LENGTH 2

LRESULT dsskeyUI_ProcessMsg(msgObject& objMessage)
{
    DSSKEYUI_INFO("dsskeyUI_ProcessMsg.Message is %d.", objMessage.message);

    switch (objMessage.message)
    {
    case DEV_MSG_FUNKEY_EVENT:
        {
            DSSKEYUI_INFO("dsskeyUI_ProcessMsg.-----DEV_MSG_FUNKEY_EVENT>>>> dsskeyid[%d]", objMessage.lParam);
            if (exp_IsEXPKey(objMessage.lParam) && !testmode_IsTestMode())
            {
                DSSKEY_MODULE_TYPE eModule = GetModuleType(objMessage.lParam);
                bool bExpSupport = IsSupportModuleType(eModule);
                if (!bExpSupport)
                {
                    // T29不支持DMT_EXP40_MODULE
                    return 0;
                }
                // 判断确定是扩展台，调用扩展台的单击响应流程
                dsskey_OnDsskeyClick(objMessage.lParam);
            }
        }
        break;
    case DEV_MSG_ELP_KEY:
        {
            if (exp_IsEXPKey(objMessage.lParam))
            {
                DSSKEY_MODULE_TYPE eModule = GetModuleType(objMessage.lParam);
                bool bExpSupport = IsSupportModuleType(eModule);
                if (!bExpSupport)
                {
                    // T29不支持DMT_EXP40_MODULE
                    return 0;
                }

                if (dsskey_GetDsskeyType(objMessage.lParam) == DT_LINE && bsSCA_ShowSCAStatus(dsskey_GetDsskeyLine(objMessage.lParam)))
                {
                    return 1;
                }

                // 判断确定是扩展台，调用长按进入扩展台的设置界面
                dsskeyUI_OnDsskeyLongPress(objMessage.lParam);
            }
        }
        break;
    case ACD_MESSAGE_STATUS_CHANGE:
    case HOTELING_MESSAGE_STATUS_CHANGE:
        {
            // 更新Line上ACD图标和label
            dsskey_RefressDsskeyByType(DT_LINE);
        }
        break;
    case REMOTE_PHONEBOOK_RELOAD:
        {
            DSSKEYUI_INFO("Receive REMOTE_PHONEBOOK_RELOAD message");
            dsskey_RefressDsskeyByType(DT_XMLGROUP);
        }
        break;
    case LOCAL_GROUP_UPDATE:
    case LOCAL_DIRECTORY_RELOAD:
        {
            DSSKEYUI_INFO("Receive LOCAL_DIRECTORY_RELOAD message");
            dsskey_RefressDsskeyByType(DT_LOCALGROUP);
        }
        break;
    case BROADSOFT_PHONEBOOK_RELOAD:
        {
            DSSKEYUI_INFO("Receive BROADSOFT_PHONEBOOK_RELOAD message");
            dsskey_RefressDsskeyByType(DT_BSGROUP);
        }
        break;
#if FEATURE_BROADTOUCH_UC
    case PRESENCE_MESSAGE_USER_INFO_CHANGE:
    case PRESENCE_MESSAGE_USER_STATUS_CHANGE:
        {
            DSSKEYUI_INFO("Receive PRESENCE_CHANGE message");
            dsskeyUI_PresenceUserChange();
        }
        break;
#endif
    case LANGUAGE_MESSAGE_LANGUAGE_CHANGED:
        {
            // 刷新Extended
            YLList<int> lstOutput;
            YLList<int>::ListIterator itr = lstOutput.begin();
            DSSKEY_MODULE_TYPE eType = exp_GetEXPModuleType();
            dsskey_getAllDsskeys(lstOutput, eType);

            itr = lstOutput.begin();
            for (; itr != lstOutput.end(); ++itr)
            {
                dsskey_refreshDSSKeyUI(*itr);
            }

            dsskeyUI_UpdateEXPEditingText();
        }
        break;

    case ACCOUNT_DEFAULT_CHANGED:
        {
            RefreshDsskeyByAccount(objMessage.wParam, acc_GetDefaultAccount());
        }
        break;
    default:
        break;
    }

    return 1;
}

void RefreshDsskeyByAccount(int nOldAccount, int nNewAccount)
{
    if (nOldAccount == nNewAccount)
    {
        return;
    }

    DssKeyLogicData DKLogicData;
    int iMaxDSSKeyNum = (PT_T69 == devicelib_GetPhoneType() || PT_T69S == devicelib_GetPhoneType()) ? 15 : 27;
    for (int i = 0; i < iMaxDSSKeyNum; ++i)
    {
        dsskey_GetData(i, DKLogicData);

        if (DT_LINE == DKLogicData.eDKType
                && (nNewAccount == DKLogicData.iLineIndex
                    || nOldAccount == DKLogicData.iLineIndex))
        {
            dsskeyUI_RefreshDsskey(DKLogicData, DSS_KEY_REFRESH_ALL);
        }
    }
}

void dsskeyUI_UpdateEXPEditingText()
{
    yl::string strLabel = _TRAN(_LANG(TRID_EIDTING));

    exp_SetExpEditingText(strLabel.c_str());
}

void dsskeyUI_RefreshPageKey()
{
    _DssKeyUILogic.RefreshPageKey();
}

void dsskeyUI_PresenceUserChange()
{
    YLList<int> listDKData;
    dsskey_getAllDsskeys(listDKData);
    YLList<int>::iterator itBegin = listDKData.begin();
    YLList<int>::iterator itEnd = listDKData.end();
    DssKeyLogicData DKlogicData;

    for (; itBegin != itEnd; ++itBegin)
    {
        dsskey_GetData((*itBegin), DKlogicData);
        // 只刷新 设置为 账号1 的Dsskey
        if (DKlogicData.eDKType == DT_LINE && DKlogicData.iLineIndex == 0)
        {
            dsskey_refreshDSSKeyUI(DKlogicData.iDssKeyID);
            DSSKEYUI_INFO("dsskeyUI_PresenceUserChange iDssKeyID(%d)", DKlogicData.iDssKeyID);
        }
    }
}

bool dsskeyUI_IsLineKey(int nKey)
{
    int nDsskeyId = dsskeyUI_GetDsskeyIDByKeycode(nKey);
    DssKey_Type eDsskeyType = dsskey_GetDsskeyType(nDsskeyId);

    return DT_LINE == eDsskeyType;
}

static yl::string TranslateDsskeyLabel(const char *lpszText, const bool bIsExpKey)
{
    if (bIsExpKey)
    {
#ifdef IF_FEATURE_TRANSLATE_EXP
        return _LANG(lpszText);
#else
        return lpszText;
#endif
    }
    else
    {
        return _LANG(lpszText);
    }
}

namespace
{
#define LANG_TRANSLATE_DSSKEY_LABEL(lpszText, bIsExpKey)    (TranslateDsskeyLabel((lpszText), (bIsExpKey)))
}

int MapIconType(Dsskey_Icon_Type eIconType, bool bNew)
{
    if (exp_GetExpColor())
    {
        return eIconType;
    }

    if (bNew)
    {
        switch (eIconType)
        {
        case DK_ICON_UnRegistered:
            return EIT_UNREGISTERED;
        case DK_ICON_Registering:
            return EIT_REGISTERING;
        case DK_ICON_Registered:
            return EIT_REGISTERED;
        case DK_ICON_Using:
            return EIT_USING;
        case DK_ICON_Speeddial:
            return EIT_SPEEDDIAL;
        case DK_ICON_Remote_Offline:
            return EIT_REMOTE_OFFLINE;
        case DK_ICON_Remote_Talking:
            return EIT_REMOTE_TALKING;
        case DK_ICON_Remote_Online:
            return EIT_REMOTE_ONLINE;
        case DK_ICON_Remote_Ringing:
            return EIT_REMOTE_RINGING;
        case DK_ICON_DND:
            return EIT_DND;
        case DK_ICON_HOLD:
            return EIT_HOLD;
        case DK_ICON_SMS:
            return EIT_SMS;
        case DK_ICON_Triangle:
            return EIT_TRIANGLE;
        case DK_ICON_Warning:
            return EIT_WARNING;
        case DK_ICON_BLF_PARK:
            return EIT_BLF_PARK;
        case DK_ICON_BLF_CALLOUT:
            return EIT_BLF_CALLOUT;
        case DK_ICON_BLF_IDLE:
            return EIT_BLF_IDLE;
        case DK_ICON_BLF_UNKNOWN:
            return EIT_BLF_UNKNOWN;
        case DK_ICON_BLF_TALK:
            return EIT_BLF_TALK;
        case DK_ICON_BLF_RINGING:
            return EIT_BLF_RINGING;
        case DK_ICON_BLF_HOLD:
            return EIT_BLF_HOLD;
        default:
            return EIT_NONE;
        }
    }
    else
    {
        switch (eIconType)
        {
        case DK_ICON_UnRegistered:
            return EIT_UNREGISTERED;
        case DK_ICON_Registering:
            return EIT_REGISTERING;
        case DK_ICON_Registered:
            return EIT_REGISTERED;
        case DK_ICON_Using:
            return EIT_USING;
        case DK_ICON_Speeddial:
            return EIT_SPEEDDIAL;
        case DK_ICON_Remote_Offline:
            return EIT_REMOTE_OFFLINE;
        case DK_ICON_Remote_Talking:
            return EIT_REMOTE_TALKING;
        case DK_ICON_Remote_Online:
            return EIT_REMOTE_ONLINE;
        case DK_ICON_Remote_Ringing:
            return EIT_REMOTE_RINGING;
        case DK_ICON_DND:
            return EIT_DND;
        case DK_ICON_HOLD:
            return EIT_HOLD;
        case DK_ICON_SMS:
            return EIT_SMS;
        case DK_ICON_Triangle:
            return EIT_TRIANGLE;
        case DK_ICON_Warning:
            return EIT_WARNING;
        case DK_ICON_BLF_PARK:
            return EIT_REMOTE_TALKING;
        case DK_ICON_BLF_CALLOUT:
            return EIT_REMOTE_TALKING;
        case DK_ICON_BLF_IDLE:
            return EIT_REMOTE_ONLINE;
        case DK_ICON_BLF_UNKNOWN:
            return EIT_REMOTE_OFFLINE;
        case DK_ICON_BLF_TALK:
            return EIT_REMOTE_TALKING;
        case DK_ICON_BLF_RINGING:
            return EIT_REMOTE_RINGING;
        case DK_ICON_BLF_HOLD:
            return EIT_REMOTE_TALKING;
        default:
            return EIT_NONE;
        }
    }

}

// 获取灯类型
LIGHT_Type GetLightType(const DSS_KEY_COLOR_TYPE eColorType, const DSS_KEY_FLUSH_TYPE eFlushType)
{
    if (eColorType == LED_COLOR_OFF)
    {
        // 关灯不用管颜色
        return LT_OFF;
    }

    LIGHT_Type eLEDType = LT_NONE;

    switch (eColorType)
    {
    case LED_COLOR_TYPE_RED:
        {
            switch (eFlushType)
            {
            case REFRESH_TYPE_NO_FLUSH:
                eLEDType = LT_RED_ON;
                break;
            case REFRESH_TYPE_SLOW_FLUSH:
                eLEDType = LT_RED_FLASH_4;
                break;
            case REFRESH_TYPE_FLUSH:
                eLEDType = LT_RED_FLASH_3;
                break;
            case REFRESH_TYPE_FAST_FLUSH:
                eLEDType = LT_RED_FLASH_2;
                break;
            case REFRESH_TYPE_FASTER_FLUSH:
                eLEDType = LT_RED_FLASH_1;
                break;
            default:
                break;
            }
        }
        break;
    case LED_COLOR_TYPE_GREEN:
        {
            switch (eFlushType)
            {
            case REFRESH_TYPE_NO_FLUSH:
                eLEDType = LT_GREEN_ON;
                break;
            case REFRESH_TYPE_SLOW_FLUSH:
                eLEDType = LT_GREEN_FLASH_4;
                break;
            case REFRESH_TYPE_FLUSH:
                eLEDType = LT_GREEN_FLASH_3;
                break;
            case REFRESH_TYPE_FAST_FLUSH:
                eLEDType = LT_GREEN_FLASH_2;
                break;
            case REFRESH_TYPE_FASTER_FLUSH:
                eLEDType = LT_GREEN_FLASH_1;
                break;
            default:
                break;
            }
        }
        break;
    case LED_COLOR_TYPE_ORANGE:
        {
            switch (eFlushType)
            {
            case REFRESH_TYPE_NO_FLUSH:
                eLEDType = LT_RED_GREEN_ON;
                break;
            case REFRESH_TYPE_SLOW_FLUSH:
                eLEDType = LT_RED_GREEN_FLASH_4;
                break;
            case REFRESH_TYPE_FLUSH:
                eLEDType = LT_RED_GREEN_FLASH_3;
                break;
            case REFRESH_TYPE_FAST_FLUSH:
                eLEDType = LT_RED_GREEN_FLASH_2;
                break;
            case REFRESH_TYPE_FASTER_FLUSH:
                eLEDType = LT_RED_GREEN_FLASH_1;
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }

    return eLEDType;
}


bool dsskeyUI_RefreshCallback(int dsskeyID, Dsskey_Status dsskeyStatus,
                              const DssKeyLogicData * pLogicData,  void * pExtraData)
{
    DsskeyExpRefresh eDssRefresh = DSS_KEY_REFRESH_ALL;
    if (pExtraData != NULL)
    {
        eDssRefresh = (DsskeyExpRefresh) * (int *)pExtraData;
    }
    DssKeyLogicData DKLogicData;
    bool bResult = dsskey_GetData(dsskeyID, DKLogicData);

    if (DKLogicData.eDKType == DT_LINE)
    {
        DSSKEYUI_INFO("dsskeyUI_RefreshCallback get logic data. id[%d], get result[%d], type[%d], dsskey status[%d]",
                      dsskeyID,
                      bResult,
                      DKLogicData.eDKType,
                      DKLogicData.iStatus);
    }

    if (DMT_PROGRAMEKEY_MODULE == DKLogicData.eType)
    {
        // Programkey需要刷新数据的只是softkey内容，只需刷新界面即可
        IdleUL_UpdateSoftkey();
    }
    else
    {
        DSSKEYUI_INFO(" ====== refresh dsskey ====== ");
        // dsskey_update
        dsskeyUI_RefreshDsskey(DKLogicData, eDssRefresh);
    }

    return true;
}

bool dsskeyUI_DsskeyLightCallback(int iKeyIndex, int iColorType, int iLightStatus)
{
    DSS_KEY_COLOR_TYPE eColor = LED_COLOR_OFF;
    DSS_KEY_FLUSH_TYPE eFlash = REFRESH_TYPE_NO_FLUSH;
    bool ret = true;
    switch ((LEDCOLOR_LTYE)iColorType)
    {
    case LT_RED:
        {
            eColor = LED_COLOR_TYPE_RED;
        }
        break;
    case LT_GREEN:
        {
            eColor = LED_COLOR_TYPE_GREEN;
        }
        break;
    case LT_REDGREEN:
        {
            eColor = LED_COLOR_TYPE_ORANGE;
        }
        break;
    default:
        {
            ret = false;
        }
        break;
    }

    switch ((LEDSTATUS_TYPE)iLightStatus)
    {
    case LST_NONE:
    case LST_OFF:
        {
            eColor = LED_COLOR_OFF;
        }
        break;
    case LST_ON:
        {
            eFlash = REFRESH_TYPE_NO_FLUSH;
        }
        break;
    case LST_SLOWFLASH:
        {
            eFlash = REFRESH_TYPE_SLOW_FLUSH;
        }
        break;
    case LST_FASTFLASH:
        {
            eFlash = REFRESH_TYPE_FAST_FLUSH;
        }
        break;
    default:
        {
            ret = false;
        }
        break;
    }

    DSSKEYUI_INFO("DsskeyLightCallback : UpdateLight Index is %d", iKeyIndex);
    dsskeyUI_UpdateDsskeyLight(iKeyIndex, eColor, eFlash);
    return ret;
}

void dsskeyUI_UpdateDsskeyLight(int iIndex, DSS_KEY_COLOR_TYPE eColorType, DSS_KEY_FLUSH_TYPE eFlushType)
{
    _DssKeyUILogic.UpdateDsskeyLight(iIndex, eColorType, eFlushType);
}

void SetEDKCtrlDsskeyDisplay(DsskeyDisplayData& displayData, const EdkCtrlDsskeyDispData& objEDKDisplayData)
{
    if (!objEDKDisplayData.m_strLable.empty())
    {
        displayData.eTextMode = TEXT_MODE_FIRST_ROW;
        displayData.strFirstRowText = objEDKDisplayData.m_strLable;
        displayData.strSecRowText = "";
    }

    if (COLOR_NONE <= objEDKDisplayData.m_eColorType
            && objEDKDisplayData.m_eColorType <= COLOR_ORANGE_PIC_IN)
    {
        displayData.eLightFlushType = REFRESH_TYPE_NO_FLUSH;
        if (COLOR_RED == objEDKDisplayData.m_eColorType
                || COLOR_RED_PIC_IN == objEDKDisplayData.m_eColorType
                || COLOR_RED_PIC_OUT == objEDKDisplayData.m_eColorType)
        {
            displayData.eLightColorType = LED_COLOR_TYPE_RED;
        }
        else if (COLOR_GREEN == objEDKDisplayData.m_eColorType
                 || COLOR_GREEN_PIC_IN == objEDKDisplayData.m_eColorType
                 || COLOR_GREEN_PIC_OUT == objEDKDisplayData.m_eColorType)
        {
            displayData.eLightColorType = LED_COLOR_TYPE_GREEN;
        }
        else if (COLOR_ORANGE == objEDKDisplayData.m_eColorType
                 || COLOR_ORANGE_PIC_OUT == objEDKDisplayData.m_eColorType
                 || COLOR_ORANGE_PIC_IN == objEDKDisplayData.m_eColorType)
        {
            displayData.eLightColorType = LED_COLOR_TYPE_ORANGE;
        }
        else if (COLOR_NONE == objEDKDisplayData.m_eColorType)
        {
            displayData.eLightColorType = LED_COLOR_OFF;
        }
    }
}

void SetEDKCtrlEXPkeyDisplay(EdkCtrlDsskeyDispData& objEDKDisplayData, yl::string& strLabel, LIGHT_Type& led_status)
{
    if (!objEDKDisplayData.m_strLable.empty())
    {
        strLabel = objEDKDisplayData.m_strLable;
    }

    if (COLOR_NONE <= objEDKDisplayData.m_eColorType
            && objEDKDisplayData.m_eColorType <= COLOR_ORANGE_PIC_IN)
    {
        if (COLOR_RED == objEDKDisplayData.m_eColorType
                || COLOR_RED_PIC_IN == objEDKDisplayData.m_eColorType
                || COLOR_RED_PIC_OUT == objEDKDisplayData.m_eColorType)
        {
            led_status = LT_RED_ON;
        }
        else if (COLOR_GREEN == objEDKDisplayData.m_eColorType
                 || COLOR_GREEN_PIC_IN == objEDKDisplayData.m_eColorType
                 || COLOR_GREEN_PIC_OUT == objEDKDisplayData.m_eColorType)
        {
            led_status = LT_GREEN_ON;
        }
        else if (COLOR_ORANGE == objEDKDisplayData.m_eColorType
                 || COLOR_ORANGE_PIC_OUT == objEDKDisplayData.m_eColorType
                 || COLOR_ORANGE_PIC_IN == objEDKDisplayData.m_eColorType)
        {
            led_status = LT_RED_GREEN_ON;
        }
        else if (COLOR_NONE == objEDKDisplayData.m_eColorType)
        {
            led_status = LT_OFF;
        }
    }
}

void dsskeyUI_Init()
{
    // 初始化注册刷新回调函数到Dsskey逻辑层中
    dsskey_setUpdateFunCallback(dsskeyUI_RefreshCallback);
    g_pLinekeyUI->Init();
    _DssKeyUILogic.Init();
    _DssKeyUILogic.RefreshMultipleDsskeyData(0, _DssKeyUILogic.GetMaxNumber());
    // 注册扩展台消息
    etl_RegisterMsgHandle(DEV_MSG_FUNKEY_EVENT, \
                          DEV_MSG_FUNKEY_EVENT, dsskeyUI_ProcessMsg);
    etl_RegisterMsgHandle(DEV_MSG_ELP_KEY, \
                          DEV_MSG_ELP_KEY, dsskeyUI_ProcessMsg);
    etl_RegisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE, \
                          ACD_MESSAGE_STATUS_CHANGE, dsskeyUI_ProcessMsg);
    etl_RegisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, \
                          HOTELING_MESSAGE_STATUS_CHANGE, dsskeyUI_ProcessMsg);
    etl_RegisterMsgHandle(REMOTE_PHONEBOOK_RELOAD, \
                          REMOTE_PHONEBOOK_RELOAD, dsskeyUI_ProcessMsg);
    etl_RegisterMsgHandle(LOCAL_DIRECTORY_RELOAD, \
                          LOCAL_DIRECTORY_RELOAD, dsskeyUI_ProcessMsg);
    etl_RegisterMsgHandle(BROADSOFT_PHONEBOOK_RELOAD, \
                          BROADSOFT_PHONEBOOK_RELOAD, dsskeyUI_ProcessMsg);
    etl_RegisterMsgHandle(LOCAL_GROUP_UPDATE, \
                          LOCAL_GROUP_UPDATE, dsskeyUI_ProcessMsg);
#if FEATURE_BROADTOUCH_UC
    etl_RegisterMsgHandle(PRESENCE_MESSAGE_USER_STATUS_CHANGE, \
                          PRESENCE_MESSAGE_USER_STATUS_CHANGE, dsskeyUI_ProcessMsg);
    etl_RegisterMsgHandle(PRESENCE_MESSAGE_USER_INFO_CHANGE, \
                          PRESENCE_MESSAGE_USER_INFO_CHANGE, dsskeyUI_ProcessMsg);
#endif
    etl_RegisterMsgHandle(LANGUAGE_MESSAGE_LANGUAGE_CHANGED, \
                          LANGUAGE_MESSAGE_LANGUAGE_CHANGED, dsskeyUI_ProcessMsg);

    etl_RegisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, \
                          ACCOUNT_DEFAULT_CHANGED, dsskeyUI_ProcessMsg);

    dsskeyUI_UpdateEXPEditingText();
}

// DssKey Setting界面进入 配置界面
void dsskeyUI_EnterSetting(int iIndex)
{
    DSSKEYUI_INFO("Enter dsskeyUI_EnterDssKeyConfigPage.");
    yl::string strAction;
    strAction = commonAPI_FormatString("OpenSubPage(line_key_item_%d)", iIndex + 1);
    /*if (SettingUI_PageExist(strAction))
    {
        DSSKEYUI_INFO("ConfigPage Already Exist!!");
    }
    else*/
    {
        SettingUI_EnterPage(strAction);
    }
}

void dsskeyUI_OnDsskeyLongPress(int iDsskeyID)
{
    if (configParser_GetConfigInt(kszBlockDsskeyExtendedPress) == 1)
    {
        return;
    }

    //判断当前界面是否是键盘锁允许进入的
    //menukey下允许dsskey但是不允许设置dsskey，需要判断KGM_MODULE_SETTING而不是KGM_MODULE_DSSKEY
    if (!keyGuard_IsEnableModuld(KGM_MODULE_SETTING))
    {
        return;
    }

    int iEXPIndex = GetEXPIndex(iDsskeyID);
    int iKeyIndex = GetEXPKeyIndex(iDsskeyID);
    DSSKEY_MODULE_TYPE eType = (DSSKEY_MODULE_TYPE)GetEXPModuleType(iDsskeyID);

#if IF_FEATRUE_PTT
    if (dsskey_IsSupportLongPress(iDsskeyID))
    {
        if (dsskey_OnDsskeyClick(iDsskeyID))
        {
            dsskey_AddLongPress(iDsskeyID);
            return;
        }
    }
#endif

#ifdef DSSKEYUI_TEST
    if (iDsskeyID >= 21)
    {
        dsskeyUI_ParseTest(iDsskeyID - 21);
    }
    else
#endif
    {
        if (dsskey_GetDsskeyType(iDsskeyID) == DT_LINE
                && bsSCA_ShowSCAStatus(dsskey_GetDsskeyLine(iDsskeyID)))
        {
            return;
        }
        dsskeyUI_EnterDssKeyConfigPage(eType, iKeyIndex, iEXPIndex);
    }
}

// 长按进入DssKey配置界面
void dsskeyUI_EnterDssKeyConfigPage(DSSKEY_MODULE_TYPE eType, int iIndex, int iExpIndex /*= -1*/)
{

    // 当前在通话中，不允许进入设置界面
    if (talklogic_SessionExist())
    {
        return;
    }

    //三级权限控制不显示则不允许进入
    // TODO
    /*if (!Setting_ShouldMenuShow("dss_key_list"))
    {
        return;
    }*/

    yl::string strAction;
    DSSKEY_UI_TYPE eUIType = DSSKEY_UI_LINEKEY;
    switch (eType)
    {
    case DMT_LINEKEY_MODULE:
        {
            strAction = commonAPI_FormatString("OpenSubPage(line_key_item_%d)", iIndex + 1);
        }
        break;
    case DMT_MEMKEY_MODULE:
        {
            strAction = commonAPI_FormatString("OpenSubPage(memory_key_item_%d)", iIndex + 1);
            eUIType = DSSKEY_UI_MEMORYKEY;
        }
        break;
    default:
        {
            if (IsEXPModule(eType)
                    && iExpIndex >= 0)
            {
                strAction = commonAPI_FormatString("OpenSubPage(expand_key_item_%d;%d)", iIndex + 1, iExpIndex);
                eUIType = DSSKEY_UI_EXPKEY;
            }
        }
        break;
    }
    DSSKEYUI_INFO("BaseUI_EnterPage [%s]", strAction.c_str());
    //进入DssKey配置界面
    if (iIndex > -1
            && !strAction.empty())
    {
        DsskeyPageInfo m_PageInfo;
        m_PageInfo.m_eUIType = eUIType;
        m_PageInfo.SetIndex(iIndex, 0);

        bool bOpenSuccess = SettingUI_EnterPage(kszPageIDDsskeySetting, (void *)(&m_PageInfo));

        // 同步由扩展台打开Dsskey界面的Title
        if (bOpenSuccess && strAction.find("expand_key_item_") != yl::string::npos)
        {
            CBaseDialog* pPageFm = UIManager_GetTopWindow();
            if (NULL != pPageFm)
            {
                yl::string strTitle = _LANG(TRID_EXTENSION_MODULE) + (" ");

                char strTemp[2];
                sprintf(strTemp, "%d", iExpIndex + 1);

                strTitle += strTemp;
                strTitle += " " + _LANG(TRID_KEY) + (" ");

                int iSize = 0;
                int posStart = strAction.rfind("_");
                int posEnd = strAction.rfind(";");

                if (posStart != yl::string::npos
                        && posEnd != yl::string::npos && posStart < posEnd)
                {
                    iSize = posEnd - posStart - 1;
                    strTitle += strAction.substr(posStart + 1, iSize);
                }
                // TODO
                //_MainWND.SetTitle(strTitle, pPageFm);
            }
        }
    }
}

int dsskeyUI_GetLinekeyIndex(int nKey)
{
    //由于T4X的DssKey键的不连续性，只能一个一个的处理
    //LineKey 键值
    int iLineKeyIdx = -1;
    switch (nKey)
    {
    case PHONE_KEY_LINE1:
        iLineKeyIdx = 0;
        break;
    case PHONE_KEY_LINE2:
        iLineKeyIdx = 1;
        break;
    case PHONE_KEY_LINE3:
        iLineKeyIdx = 2;
        break;
    case PHONE_KEY_LINE4:
        iLineKeyIdx = 3;
        break;
    case PHONE_KEY_LINE5:
        iLineKeyIdx = 4;
        break;
    case PHONE_KEY_LINE6:
        iLineKeyIdx = 5;
        break;
    case PHONE_KEY_LINE7:
        iLineKeyIdx = 6;
        break;
    case PHONE_KEY_LINE8:
        iLineKeyIdx = 7;
        break;
    case PHONE_KEY_LINE9:
        iLineKeyIdx = 8;
        break;
    case PHONE_KEY_LINE10:
        iLineKeyIdx = 9;
    default:
        break;
    }//end //LineKey 键值
    return iLineKeyIdx;
}

enum BLF_LED_TYPE
{
    BLF_LED_DEFAULT = 0,        //0：默认出厂为?0，未修改过任何状态的默认状态指示逻辑（对应于features.blf_led_mode和features.blf_and_callpark_idle_led_enable配置项都为0的情况）；
    BLF_LED_MODE,               //1：对应原有配置项features.blf_led_mode=1时的指示灯逻辑状态模式；
    BLF_LED_BLF_AND_PARK_IDLE,  //2：对应原有配置项features.blf_and_callpark_idle_led_enable=1时的指示灯逻辑状态模式；
    BLF_LED_BROADSOFT,          //3：新增逻辑，对应于Broadsoft官方说明的指示灯逻辑状态模式。在Broadsoft UC one版本中默认出厂配置值即为?3；
};

void GetBlfLightState(DsskeyDisplayData& objDisplayData, int iStatus, int iAuxiliaryStatus)
{
    //0：默认出厂为?0，未修改过任何状态的默认状态指示逻辑（对应于features.blf_led_mode和features.blf_and_callpark_idle_led_enable配置项都为0的情况）；
    //1：对应原有配置项features.blf_led_mode=1时的指示灯逻辑状态模式；
    //2：对应原有配置项features.blf_and_callpark_idle_led_enable=1时的指示灯逻辑状态模式；
    //3：新增逻辑，对应于Broadsoft官方说明的指示灯逻辑状态模式。在Broadsoft UC one版本中默认出厂配置值即为?3；
    //4：新增逻辑，对应于TYN R运营商  T26 特制支持CS2K（genband)的BLF功能。在TYN R运营商版本中默认出厂配置值即为?4；http://192.168.1.99/Bug.php?BugID=23556
    int iBLFLedMode = configParser_GetConfigInt(kszBLFLedMode);

    DialgInfo sDiaLogInfo;
    blf_GetDialogInfoByDsskeyId(sDiaLogInfo, objDisplayData.iDsskeyID, BLF_Manager);

    objDisplayData.eLightColorType = LED_COLOR_OFF;
    switch (iStatus)
    {
    case DS_BLF_STATUS_IDLE:
        {
            if (0 == iBLFLedMode
                    && sDiaLogInfo.m_nMode == 0) // blf的特殊状态
            {
                // 没开特殊配置则idle下亮绿灯
                objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
            }
        }
        break;
    case DS_BLF_STATUS_CALLOUT:
        {
            if (sDiaLogInfo.m_nMode == 2)
            {
                objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
            }
            else
            {
                objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
            }
        }
        break;
    case DS_BLF_STATUS_RINGING:
        {
            objDisplayData.iLightPriority = DSS_KEY_LED_PRIORITY_HIGH;
            // page灯
            objDisplayData.ePageColorType = LED_COLOR_TYPE_RED;
            // page灯闪烁类型
            objDisplayData.ePageLightFlushType = REFRESH_TYPE_FAST_FLUSH;

            if (3 == iBLFLedMode)
            {
                // 绿灯快闪
                objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
                objDisplayData.eLightFlushType = REFRESH_TYPE_FASTER_FLUSH;
                // page闪绿灯
                objDisplayData.ePageColorType = LED_COLOR_TYPE_GREEN;
            }
            else if (sDiaLogInfo.m_nMode == 2)
            {
                // 绿灯普通闪烁
                objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
                objDisplayData.eLightFlushType = REFRESH_TYPE_FLUSH;
            }
            else
            {
                // 红灯快闪
                objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
                objDisplayData.eLightFlushType = REFRESH_TYPE_FASTER_FLUSH;
            }
        }
        break;
    case DS_BLF_STATUS_TALKING:
        {
            if (4 == iBLFLedMode
                    || sDiaLogInfo.m_nMode == 2)
            {
                objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
            }
            else
            {
                objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
            }
        }
        break;
    case DS_BLF_STATUS_HOLD:
        {
            if (3 == iBLFLedMode)
            {
                objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
            }
            else
            {
                // 红灯慢闪
                objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
                objDisplayData.eLightFlushType = REFRESH_TYPE_SLOW_FLUSH;
            }
        }
        break;
    case DS_BLF_STATUS_PARK:
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
            objDisplayData.eLightFlushType = REFRESH_TYPE_SLOW_FLUSH;
        }
        break;
    case DS_BLF_STATUS_UNKOWN:
        {
            if (4 == iBLFLedMode)
            {
                objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
                objDisplayData.eLightFlushType = REFRESH_TYPE_SLOW_FLUSH;
            }
        }
        break;
    case DS_BLF_STATUS_DND:
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
            objDisplayData.eLightFlushType = REFRESH_TYPE_SLOW_FLUSH;
        }
        break;
    default:
        {
            objDisplayData.eLightColorType = LED_COLOR_OFF;
        }
        break;
    }
}

void MapSCALightStatus(Dsskey_Status eStatus, DsskeyDisplayData& objLineData)
{
    bool bHaveSession = false;
    bool bHold = false;

	CSessionProxy proxy = talk_GetSessionByDsskey(objLineData.iDsskeyID);
	if (proxy && proxy.IsSingle())
	{
		bHaveSession = proxy.GetSessionState() != SESSION_PARK_NOTIFY;
		bHold = talklogic_IsHoldSession(proxy);
	}

    switch (eStatus)
    {
    case DS_BLA_STATUS_ALERTING:
        {
            objLineData.eLightColorType = LED_COLOR_TYPE_GREEN;
            objLineData.eLightFlushType = REFRESH_TYPE_FAST_FLUSH;
            objLineData.ePageColorType = LED_COLOR_TYPE_GREEN;
            objLineData.ePageLightFlushType = REFRESH_TYPE_FAST_FLUSH;
            objLineData.iLightPriority = DSS_KEY_LED_PRIORITY_SECOND_HIGH;
        }
        break;
    case DS_BLA_STATUS_SEIZED:
    case DS_BLA_STATUS_PROGRESSING:
    case DS_BLA_STATUS_ACTIVE:
        {
            objLineData.eLightColorType = bHaveSession ? LED_COLOR_TYPE_GREEN : LED_COLOR_TYPE_RED;
        }
        break;
    case DS_BLA_STATUS_BRIDGE_ACITVE:
        {
            objLineData.eLightColorType = bHaveSession ? LED_COLOR_TYPE_GREEN : LED_COLOR_TYPE_RED;
            objLineData.eLightFlushType = bHold ? REFRESH_TYPE_SLOW_FLUSH : REFRESH_TYPE_NO_FLUSH;
        }
        break;
    case DS_BLA_STATUS_BRIDGE_HELD:
        {
            objLineData.eLightColorType = bHaveSession ? LED_COLOR_TYPE_GREEN : LED_COLOR_TYPE_RED;
            objLineData.eLightFlushType = bHaveSession ? REFRESH_TYPE_SLOW_FLUSH : REFRESH_TYPE_NO_FLUSH;
            if (bHaveSession)
            {
                objLineData.ePageColorType = LED_COLOR_TYPE_GREEN;
                objLineData.ePageLightFlushType = REFRESH_TYPE_FLUSH;
                objLineData.iLightPriority = DSS_KEY_LED_PRIORITY_SECOND_HIGH;
            }
        }
        break;
    case DS_BLA_STATUS_HELD:
        {
            objLineData.eLightColorType = bHaveSession ? LED_COLOR_TYPE_GREEN : LED_COLOR_TYPE_RED;
            objLineData.eLightFlushType = REFRESH_TYPE_SLOW_FLUSH;
            objLineData.ePageColorType = LED_COLOR_TYPE_GREEN;
            objLineData.ePageLightFlushType = REFRESH_TYPE_FLUSH;
            objLineData.iLightPriority = DSS_KEY_LED_PRIORITY_SECOND_HIGH;
        }
        break;
    case DS_BLA_STATUS_HELD_PRIVATE:
        {
            objLineData.eLightColorType = bHaveSession ? LED_COLOR_TYPE_GREEN : LED_COLOR_TYPE_RED;
            objLineData.eLightFlushType = bHaveSession ? REFRESH_TYPE_SLOW_FLUSH : REFRESH_TYPE_NO_FLUSH;
            if (bHaveSession)
            {
                objLineData.ePageColorType = LED_COLOR_TYPE_GREEN;
                objLineData.ePageLightFlushType = REFRESH_TYPE_FLUSH;
                objLineData.iLightPriority = DSS_KEY_LED_PRIORITY_SECOND_HIGH;
            }
        }
        break;
    case DS_BLA_STATUS_IDLE:
    case DS_BLA_STATUS_UNKOWN:
        {
            objLineData.eLightColorType = LED_COLOR_OFF;
        }
        break;
    case DS_BLF_STATUS_PARK:
        {
            objLineData.eLightColorType = LED_COLOR_TYPE_RED;
            objLineData.eLightFlushType = REFRESH_TYPE_SLOW_FLUSH;
        }
        break;
    default:
        break;
    }

    DSSKEYUI_INFO("SCA DsskeyID[%d] Color[%d], Flush[%d]", objLineData.iDsskeyID, objLineData.eLightColorType, objLineData.eLightFlushType);
}

// line状态较多，专门一个函数处理灯状态
void Process_LineStatus(DsskeyDisplayData& objDisplayData, const DssKeyLogicData& sLogicData, bool bUseSCAStauts)
{
    YLList<int> listState = sLogicData.listAuxiliaryState;
    YLList<int>::iterator itr = listState.begin();
    YLList<int>::iterator itrEnd = listState.end();

    Dsskey_Status eTalkStatu = DS_NON;
    Dsskey_Status eSCAStatu = DS_NON;
    Dsskey_Status eACDStatu = DS_NON;
    for (; itr != itrEnd; itr++)
    {
        if (*itr > DS_SESSION_FIRST
                && *itr < DS_SESSION_END)
        {
            eTalkStatu = (Dsskey_Status)(*itr);
        }
        // blf状态和sca状态在同一个区间，如果取到blf状态覆盖sca导致图标显示异常
        else if (*itr >= DS_BLA_STATUS_IDLE
                 && *itr <= DS_BLA_STATUS_UNKOWN)
        {
            eSCAStatu = (Dsskey_Status)(*itr);
        }
        else if (*itr > DS_ACD_FIRST
                 && *itr < DS_ACD_LAST)
        {
            eACDStatu = (Dsskey_Status)(*itr);
        }
    }

    DSSKEYUI_INFO("Line Status Talk:%d, SCA:%d, ACD:%d", eTalkStatu, eSCAStatu, eACDStatu);

    switch (sLogicData.iStatus)
    {
    case DS_LINE_REGISTERED:
    //http://10.2.1.199/Bug.php?BugID=88468
    case DS_LINE_FWD:
    case DS_LINE_DND:
        {
            // page状态
            if (bUseSCAStauts)
            {
                MapSCALightStatus(eSCAStatu, objDisplayData);
            }
        }
        break;
    case DS_LINE_USING:
        {
            if (!bUseSCAStauts)
            {
                objDisplayData.eIconType = DK_ICON_Using;
            }
            // page状态
            //          objDisplayData.ePageColorType = LED_COLOR_TYPE_GREEN;

            if (eSCAStatu != DS_NON && bUseSCAStauts)
            {
                objDisplayData.ePageColorType = LED_COLOR_OFF;
                MapSCALightStatus(eSCAStatu, objDisplayData);
            }
            else if (DS_SESSION_NORMAL_USING == eTalkStatu
                     || DS_SESSION_TALK == eTalkStatu
                     || DS_SESSION_CALL_Out == eTalkStatu) // 通话状态暂时和DS_SESSION_NORMAL_USING一样
            {
                // dsskey状态
                objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
            }
            else if (DS_SESSION_HOLD == eTalkStatu)
            {
                // dsskey状态
                objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
                objDisplayData.eLightFlushType = REFRESH_TYPE_FLUSH;
                objDisplayData.iLightPriority = DSS_KEY_LED_PRIORITY_HIGH; // 非特殊状态优先级低

                objDisplayData.iLightPriority = DSS_KEY_LED_PRIORITY_HIGH;
                objDisplayData.ePageColorType = LED_COLOR_TYPE_GREEN;
                objDisplayData.ePageLightFlushType = REFRESH_TYPE_FLUSH;
            }
            else if (DS_SESSION_CALL_IN == eTalkStatu) // T46在来电中常亮红灯
            {
                // dsskey状态
                objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
                objDisplayData.eLightFlushType = REFRESH_TYPE_FAST_FLUSH;
                objDisplayData.iLightPriority = DSS_KEY_LED_PRIORITY_SECOND_HIGH;
                objDisplayData.ePageColorType = LED_COLOR_TYPE_GREEN;
                objDisplayData.ePageLightFlushType = REFRESH_TYPE_FAST_FLUSH;
                objDisplayData.iLightPriority = DSS_KEY_LED_PRIORITY_SECOND_HIGH;
#if! IS_COLOR
                objDisplayData.strIconPath = PIC_DSS_LINE_RINGING;
#endif
            }
            else if (DS_SESSION_DIAL == eTalkStatu)
            {
                // dsskey状态
                objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
            }
        }
        break;
    default:
        break;
    }

    if (DS_SESSION_CALL_Out == eTalkStatu)
    {
#if! IS_COLOR
        objDisplayData.strIconPath = PIC_DSS_LINE_CALLOUT;
#endif
    }

    objDisplayData.strFirstRowText = acc_GetAccountShowText(sLogicData.iLineIndex);

    bool bHoteling = false;
    // 判断是否已经登录上Hoteling
    bHoteling = (CallCenter_IsHotelingOn() && HOTELING_STATUS_LOGIN == CallCenter_GetHotelingStatus());
    if (!sLogicData.strLabel.empty() && !bHoteling)
    {
        // Label 不为空，使用配置的Label
        objDisplayData.strFirstRowText = sLogicData.strLabel;
    }
    DSSKEYUI_INFO("iLineIndex:%d, eColorType:%d, eFlushType:%d, eIconType:%d",
                  sLogicData.iLineIndex, objDisplayData.eLightColorType, objDisplayData.eLightFlushType, objDisplayData.eIconType);
}

void ProcessSessionStatus(DsskeyDisplayData& objDisplayData, const DssKeyLogicData& sLogicData)
{
    YLList<int> listState = sLogicData.listAuxiliaryState;
    YLList<int>::iterator itr = listState.begin();
    YLList<int>::iterator itrEnd = listState.end();

    Dsskey_Status eTalkStatu = DS_NON;
    for (; itr != itrEnd; itr++)
    {
        if (*itr > DS_SESSION_FIRST
                && *itr < DS_SESSION_END)
        {
            eTalkStatu = (Dsskey_Status)(*itr);
        }
    }
    DSSKEYUI_INFO("ProcessSessionStatus:: eTalkStatu:%d", eTalkStatu);
    switch (eTalkStatu)
    {
    case DS_SESSION_NORMAL_USING:
    case DS_SESSION_TALK:
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
        }
        break;
    case DS_SESSION_HIDE:
        {
            objDisplayData.eLightColorType = LED_COLOR_OFF;
        }
        break;
    case DS_SESSION_HOLD:
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
            objDisplayData.eLightFlushType = REFRESH_TYPE_FLUSH;
        }
        break;
    default:
        {
            objDisplayData.eLightColorType = LED_COLOR_OFF;
        }
        break;
    }
}

#define MAX_NUMBER_ATTR_COUNT 6

// TODO handle 图片
// 处理账号功能的SCA 状态显示
void Process_LineSCA(DsskeyDisplayData& objDisplayData, const DssKeyLogicData& sLogicData, bool bExt)
{
    YLList<int> listState = sLogicData.listAuxiliaryState;
    YLList<int>::iterator itr = listState.begin();
    YLList<int>::iterator itrEnd = listState.end();
    // 解析SCA状态
    Dsskey_Status eSCAStatu = DS_NON;
    for (; itr != itrEnd; itr++)
    {
        DSSKEYUI_INFO("[SCA]   itr->dataEntry is %d", *itr);
        // blf状态和sca状态在同一个区间，如果取到blf状态覆盖sca导致图标显示异常
        if (*itr >= DS_BLA_STATUS_IDLE
                && *itr <= DS_BLA_STATUS_UNKOWN)
        {
            eSCAStatu = (Dsskey_Status)(*itr);
            break;
        }
    }

    // 网页修改或液晶修改dsskey，需要取线路状态
    if (eSCAStatu == -1)
    {
        eSCAStatu = dsskey_MapStatusToDsskeyStatus(bsSCA_GetStatusByDsskeyID(sLogicData.iDssKeyID));
    }

    DSSKEYUI_INFO("[SCA]   eSCAStatu is %d", eSCAStatu);

    yl::string strSCAIcon = "";

    switch (eSCAStatu)
    {
    case DS_BLA_STATUS_IDLE:
        {
            if (exp_GetExpColor())
            {
                objDisplayData.eIconType = DK_ICON_BLA_IDLE;
            }
            else
            {
                objDisplayData.eIconType = DK_ICON_Remote_Online;
            }

            //strSCAIcon = PIC_DSS_BLA_IDLE;
            if (TRANS_ALL == sLogicData.iTranslateLength && false == bExt)
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 (_LANG(TRID_SHARE_LINE)) : (_LANG(sLogicData.strLabel.c_str()));
            }
            else
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 TRID_SHARE_LINE : sLogicData.strLabel;
            }
        }
        break;
    case DS_BLA_STATUS_SEIZED:
        {
            if (exp_GetExpColor())
            {
                objDisplayData.eIconType = DK_ICON_BLA_SEIZED;
            }
            else
            {
                objDisplayData.eIconType = DK_ICON_Remote_Talking;
            }
            //strSCAIcon = PIC_DSS_BLA_SEIZED;
            if (TRANS_ALL == sLogicData.iTranslateLength && false == bExt)
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : (_LANG(sLogicData.strLabel.c_str()));
            }
            else
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : sLogicData.strLabel;
            }
        }
        break;
    case DS_BLA_STATUS_PROGRESSING:
        {
            if (exp_GetExpColor())
            {
                objDisplayData.eIconType = DK_ICON_BLA_PROGRESSING;
            }
            else
            {
                objDisplayData.eIconType = DK_ICON_Remote_Talking;
            }
            //strSCAIcon = PIC_DSS_BLA_PROGRESSING;
            if (TRANS_ALL == sLogicData.iTranslateLength && false == bExt)
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : (_LANG(sLogicData.strLabel.c_str()));
            }
            else
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : sLogicData.strLabel;
            }
        }
        break;
    case DS_BLA_STATUS_ALERTING:
        {
            if (exp_GetExpColor())
            {
                objDisplayData.eIconType = DK_ICON_BLA_ALERTING;
            }
            else
            {
                objDisplayData.eIconType = DK_ICON_Remote_Ringing;
            }

            //strSCAIcon = PIC_DSS_BLA_ALERTING;
            if (TRANS_ALL == sLogicData.iTranslateLength && false == bExt)
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : (_LANG(sLogicData.strLabel.c_str()));
            }
            else
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : sLogicData.strLabel;
            }
        }
        break;
    case DS_BLA_STATUS_ACTIVE:
        {
            if (exp_GetExpColor())
            {
                objDisplayData.eIconType = DK_ICON_BLA_ACTIVE;
            }
            else
            {
                objDisplayData.eIconType = DK_ICON_Remote_Talking;
            }

            //strSCAIcon = PIC_DSS_BLA_ACTIVE;
            if (TRANS_ALL == sLogicData.iTranslateLength && false == bExt)
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : (_LANG(sLogicData.strLabel.c_str()));
            }
            else
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : sLogicData.strLabel;
            }
        }
        break;
    case DS_BLA_STATUS_HELD:
        {
            if (exp_GetExpColor())
            {
                objDisplayData.eIconType = DK_ICON_BLA_HELD;
            }
            else
            {
                objDisplayData.eIconType = DK_ICON_Remote_Talking;
            }

            //strSCAIcon = PIC_DSS_BLA_HELD;
            if (TRANS_ALL == sLogicData.iTranslateLength && false == bExt)
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : (_LANG(sLogicData.strLabel.c_str()));
            }
            else
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : sLogicData.strLabel;
            }
        }
        break;
    case DS_BLA_STATUS_HELD_PRIVATE:
        {
            if (exp_GetExpColor())
            {
                objDisplayData.eIconType = DK_ICON_BLA_HELD_PRIVATE;
            }
            else
            {
                objDisplayData.eIconType = DK_ICON_Remote_Talking;
            }

            //strSCAIcon = PIC_DSS_BLA_HELD_PRIVATE;
            if (TRANS_ALL == sLogicData.iTranslateLength && false == bExt)
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : (_LANG(sLogicData.strLabel.c_str()));
            }
            else
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : sLogicData.strLabel;
            }
        }
        break;
    case DS_BLA_STATUS_BRIDGE_ACITVE:
        {
            if (exp_GetExpColor())
            {
                objDisplayData.eIconType = DK_ICON_BLA_BRIDGE_ACTIVE;
            }
            else
            {
                objDisplayData.eIconType = DK_ICON_Remote_Talking;
            }

            //strSCAIcon = PIC_DSS_BLA_BRIDGE_ACTIVE;
            if (TRANS_ALL == sLogicData.iTranslateLength && false == bExt)
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : (_LANG(sLogicData.strLabel.c_str()));
            }
            else
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : sLogicData.strLabel;
            }
        }
        break;
    case DS_BLA_STATUS_BRIDGE_HELD:
        {
            if (exp_GetExpColor())
            {
                objDisplayData.eIconType = DK_ICON_BLA_BRIDGE_HELD;
            }
            else
            {
                objDisplayData.eIconType = DK_ICON_Remote_Talking;
            }

            //strSCAIcon = PIC_DSS_BLA_BRIDGE_HELD;
            if (TRANS_ALL == sLogicData.iTranslateLength && false == bExt)
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : (_LANG(sLogicData.strLabel.c_str()));
            }
            else
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : sLogicData.strLabel;
            }
        }
        break;
    case DS_BLF_STATUS_PARK:
        {
            if (exp_GetExpColor())
            {
                objDisplayData.eIconType = DK_ICON_BLA_PARK;
            }
            else
            {
                objDisplayData.eIconType = DK_ICON_Remote_Offline;
            }
            //strSCAIcon = PIC_DSS_BLA_PARK;
            if (TRANS_ALL == sLogicData.iTranslateLength && false == bExt)
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : (_LANG(sLogicData.strLabel.c_str()));
            }
            else
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                                 sLogicData.strValue : sLogicData.strLabel;
            }
        }
        break;
    case DS_BLA_STATUS_UNKOWN:
    case DS_NON:
        {
            if (exp_GetExpColor())
            {
                objDisplayData.eIconType = DK_ICON_BLA_UNKNOWN;
            }
            else
            {
                objDisplayData.eIconType = DK_ICON_Remote_Offline;
            }
            //strSCAIcon = PIC_DSS_BLA_UNKNOWN;
            objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;

        }
        break;
    default:
        {
            DSSKEYUI_INFO("Dsskey BLA status error!");
        }
        break;
    }

    if (DS_LINE_FWD == sLogicData.iStatus)
    {
        objDisplayData.strIconPath = PIC_DSS_LINE_FORWARD;
        objDisplayData.eIconType = DK_ICON_LINE_FORWARD;
    }
    else if (DS_LINE_DND == sLogicData.iStatus)
    {
        objDisplayData.strIconPath = PIC_DSS_LINE_DND;
        objDisplayData.eIconType = DK_ICON_LINE_DND;
    }
    // http://10.2.1.199/Bug.php?BugID=93271
    else if (sLogicData.iStatus == DS_LINE_REGISTERED)
    {
#if !IS_COLOR
        objDisplayData.strIconPath = PIC_DSS_LINE_BLA;
        objDisplayData.eIconType = DK_ICON_BLA;
#endif
    }
    else
    {
        objDisplayData.strIconPath = strSCAIcon;
        //      objDisplayData.eIconType = DK_ICON_CUSTOM;
    }

}

// 处理账号功能的ACD状态显示
void Process_LineACD(DsskeyDisplayData& objDisplayData)
{
    // 处理ACD状态下的图标
    switch (ACD_GetAcdStatus())
    {
    case AS_IDLE:
        {
            objDisplayData.strIconPath = PIC_DSS_ACD_LOGOUT;
            objDisplayData.eIconType = DK_ICON_ACD_LOGOUT;
        }
        break;
#if IF_BUG_24860
    case AS_AVAILABLE:
    case AS_UNAVAILABLE:
    case AS_WRAPUP:
        {
            objDisplayData.strIconPath = PIC_DSS_ACD_AVAILABLE;
        }
        break;
#else
    case AS_AVAILABLE:
        {
            objDisplayData.strIconPath = PIC_DSS_ACD_AVAILABLE;
            objDisplayData.eIconType = DK_ICON_COLOR_ACD_AVAILABLE;
        }
        break;
    case AS_UNAVAILABLE:
        {
            objDisplayData.strIconPath = PIC_DSS_ACD_UNAVAILABLE;
            objDisplayData.eIconType = DK_ICON_COLOR_ACD_UNAVAILABLE;
        }
        break;
    case AS_WRAPUP:
        {
            objDisplayData.strIconPath = PIC_DSS_ACD_WARP_UP;
            objDisplayData.eIconType = DK_ICON_COLOR_ACD_WRAPUP;
        }
        break;
#endif
    default:
        {
            objDisplayData.strIconPath = "";
            objDisplayData.eIconType = DK_ICON_None;
        }
        break;
    }
}

// 处理普通账号功能的显示
void Process_LineNormal(DsskeyDisplayData& objDisplayData, const DssKeyLogicData& sLogicData)
{
    switch (sLogicData.iStatus)
    {
    case DS_LINE_UNREGISTERED:
        {
            objDisplayData.strIconPath = PIC_DSS_LINE_UNREGISTERED;
            objDisplayData.eIconType = DK_ICON_UnRegistered;
        }
        break;
    case DS_LINE_REGISTERING:
        {
            objDisplayData.strIconPath = PIC_DSS_LINE_REGISTERING;
            objDisplayData.eIconType = DK_ICON_Registering;
        }
        break;
    case DS_LINE_REGISTERED:
        {
            objDisplayData.strIconPath = PIC_DSS_LINE_REGISTERED;
            objDisplayData.eIconType = DK_ICON_Registered;
        }
        break;
    case DS_LINE_USING:
        {
            objDisplayData.strIconPath = PIC_DSS_LINE_USING;
            objDisplayData.eIconType = DK_ICON_Using;
        }
        break;
    case DS_LINE_NONE:
    case DS_LINE_DISABLE:
        {
            objDisplayData.strIconPath = PIC_DSS_LINE_FAIL;
            objDisplayData.eIconType = DK_ICON_LINE_FAIL;
        }
        break;
    case DS_LINE_FWD:
        {
            objDisplayData.strIconPath = PIC_DSS_LINE_FORWARD;
            objDisplayData.eIconType = DK_ICON_LINE_FORWARD;
        }
        break;
    case DS_LINE_DND:
        {
            objDisplayData.strIconPath = PIC_DSS_LINE_DND;
            objDisplayData.eIconType = DK_ICON_LINE_DND;
        }
        break;
    default:
        {
            objDisplayData.strIconPath = PIC_DSS_LINE_UNREGISTERED;
            objDisplayData.eIconType = DK_ICON_UnRegistered;
        }
        break;
    }
}

// 处理账号功能的UC显示
void Process_LineUC(DsskeyDisplayData& objDisplayData, const DssKeyLogicData& sLogicData)
{
#if FEATURE_BROADTOUCH_UC
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
            yl::string strAvatar = presenceInfo.strAvatarInfo;
            //UC账号注册成功才显示Presence的头像
            if (!strAvatar.empty() && !bUserHigherPriorityIcon)//连接失败或者为空都不设置
            {
                strAvatar = TEMP_PATH + strAvatar;
                objDisplayData.strIconPath = strAvatar;
                objDisplayData.eIconType = DK_ICON_CUSTOM;
            }
            else
            {
                // 使用外部已经设置的图标，这边不再设置
                objDisplayData.strIconPath = PIC_DSS_LINE;
            }
            //是否显示新状态图标
            bool bNewIconMode = configParser_GetConfigInt(kszPresenceIconMode) == 1;
            _PRESENCE_TYPE eStatus = (_PRESENCE_TYPE)presenceInfo.ePresenceType;
            switch (eStatus)
            {
            case _PRESENCE_TYPE_ONLINE:
                objDisplayData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_ONLINE : PIC_ICON_PRESENCE_ONLINE;
                objDisplayData.ePresenceType = PRESENCE_STATUS_ONLINE;
                break;
            case _PRESENCE_TYPE_CHATTY:
                objDisplayData.strPresenceIcon = PIC_ICON_PRESENCE_CHATTY;
                objDisplayData.ePresenceType = PRESENCE_STATUS_CHATTY;
                break;
            case _PRESENCE_TYPE_AWAY:
                objDisplayData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_AWAY : PIC_ICON_PRESENCE_AWAY;
                objDisplayData.ePresenceType = PRESENCE_STATUS_AWAY;
                break;
            case _PRESENCE_TYPE_EXTENDED_AWAY:
                objDisplayData.strPresenceIcon = PIC_ICON_PRESENCE_EXTENDEDAWAY;
                objDisplayData.ePresenceType = PRESENCE_STATUS_EXTENDEDAWAY;
                break;
            case _PRESENCE_TYPE_BUSY:
                objDisplayData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_BUSY : PIC_ICON_PRESENCE_BUSY;
                objDisplayData.ePresenceType = PRESENCE_STATUS_BUSY;
                break;
            case _PRESENCE_TYPE_OFFLINE:
                objDisplayData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_OFFLINE : PIC_ICON_PRESENCE_OFFLINE;
                objDisplayData.ePresenceType = PRESENCE_STATUS_OFFLINE;
                break;
            case _PRESENCE_TYPE_UNKNOWN:
                objDisplayData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_UNKNOWN : PIC_ICON_PRESENCE_UNKNOWN;
                objDisplayData.ePresenceType = PRESENCE_STATUS_UNKNOWN;
                break;
            default:
                objDisplayData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_OFFLINE : PIC_ICON_PRESENCE_OFFLINE;
                objDisplayData.ePresenceType = PRESENCE_STATUS_OFFLINE;
                break;
            }

#if IF_FEATURE_32591
            objDisplayData.strPresenceIcon.clear();
            objDisplayData.ePresenceType = PRESENCE_STATUS_NONE;
#endif
        }
    }
#endif
}

// 处理BLF状态显示
void Process_BLFStatus(DsskeyDisplayData& objDisplayData, const DssKeyLogicData& sLogicData, bool bExt)
{
    int iAuxiliaryStatus = -1;
    YLList<int> listState = sLogicData.listAuxiliaryState;
    if (listState.begin() != listState.end())
    {
        iAuxiliaryStatus = *listState.begin();
    }
    GetBlfLightState(objDisplayData, sLogicData.iStatus, iAuxiliaryStatus);


    //0：默认出厂为?0，未修改过任何状态的默认状态指示逻辑（对应于features.blf_led_mode和features.blf_and_callpark_idle_led_enable配置项都为0的情况）；
    //1：对应原有配置项features.blf_led_mode=1时的指示灯逻辑状态模式；
    //2：对应原有配置项features.blf_and_callpark_idle_led_enable=1时的指示灯逻辑状态模式；
    //3：新增逻辑，对应于Broadsoft官方说明的指示灯逻辑状态模式。在Broadsoft UC one版本中默认出厂配置值即为?3；
    //4：Genband服务器, 只有Unknown/Talking/IDle三种状态
    int iBLFLedMode = configParser_GetConfigInt(kszBLFLedMode);

    DialgInfo sDiaLogInfo;

    blf_GetDialogInfoByDsskeyId(sDiaLogInfo, sLogicData.iDssKeyID, BLF_Manager);
    //监控号码没有通话状态
    DSSKEY_MODULE_TYPE eType = (DSSKEY_MODULE_TYPE)GetEXPModuleType(sLogicData.iDssKeyID);

    if (TRANS_ALL == sLogicData.iTranslateLength)
    {
        objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                         sLogicData.strValue : (LANG_TRANSLATE_DSSKEY_LABEL(sLogicData.strLabel.c_str(), bExt));
    }
    else
    {
        objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ?
                                         sLogicData.strValue : sLogicData.strLabel;
    }

    //监控号码没有通话状态
    if (sLogicData.iStatus == DS_BLF_STATUS_IDLE)
    {
        objDisplayData.eLightColorType = LED_COLOR_OFF;
        if (0 == iBLFLedMode
                && sDiaLogInfo.m_nMode == 0) // blf的特殊状态
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
        }
        objDisplayData.eIconType = DK_ICON_BLF_IDLE;
        objDisplayData.strIconPath = PIC_DSS_BLF_IDLE;
        objDisplayData.eTextMode = TEXT_MODE_FIRST_ROW;
    }
    else if (sLogicData.iStatus == DS_BLF_STATUS_RINGING)
    {
        if (3 == iBLFLedMode)
        {
            // 绿灯快闪
            objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
            objDisplayData.eLightFlushType = REFRESH_TYPE_FASTER_FLUSH;
        }
        else
        {
            if (sDiaLogInfo.m_nMode == 2)
            {
                // 绿灯普通闪烁
                objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
                objDisplayData.eLightFlushType = REFRESH_TYPE_FLUSH;
            }
            else
            {
                // 红灯快闪
                objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
                objDisplayData.eLightFlushType = REFRESH_TYPE_FASTER_FLUSH;
            }
        }
        objDisplayData.eIconType = DK_ICON_BLF_RINGING;
        objDisplayData.strIconPath = PIC_DSS_BLF_RING;
    }
    else if (sLogicData.iStatus == DS_BLF_STATUS_CALLOUT)
    {
        if (sDiaLogInfo.m_nMode == 2)
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
        }
        else
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
        }
        objDisplayData.eIconType = DK_ICON_BLF_CALLOUT;
        objDisplayData.strIconPath = PIC_DSS_BLF_CALLOUT;
    }
    else if (sLogicData.iStatus == DS_BLF_STATUS_TALKING)
    {
        if (4 == iBLFLedMode)
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
        }
        else if (sDiaLogInfo.m_nMode == 2)
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
        }
        else
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
        }
        objDisplayData.eIconType = DK_ICON_BLF_TALK;
        objDisplayData.strIconPath = PIC_DSS_BLF_TALKING;
    }
    else if (sLogicData.iStatus == DS_BLF_STATUS_HOLD)
    {
        if (3 == iBLFLedMode)
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
        }
        else
        {
            // 红灯慢闪
            objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
            objDisplayData.eLightFlushType = REFRESH_TYPE_SLOW_FLUSH;
        }
        objDisplayData.eIconType = DK_ICON_BLF_HOLD;
        objDisplayData.strIconPath = PIC_DSS_BLF_HOLD;
    }
    else if (sLogicData.iStatus == DS_BLF_STATUS_PARK)
    {
        objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
        objDisplayData.eLightFlushType = REFRESH_TYPE_SLOW_FLUSH;
        objDisplayData.eIconType = DK_ICON_BLF_PARK;
        // Park图标
        objDisplayData.strIconPath = PIC_DSS_BLF_PARK;
    }
    else if (sLogicData.iStatus == DS_BLF_STATUS_UNKOWN || sLogicData.iStatus == DS_NON)
    {
        if (4 == iBLFLedMode)
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
            objDisplayData.eLightFlushType = REFRESH_TYPE_SLOW_FLUSH;
        }
        else
        {
            objDisplayData.eLightColorType = LED_COLOR_OFF;
        }
        objDisplayData.eIconType = DK_ICON_BLF_UNKNOWN;
        objDisplayData.strIconPath = PIC_DSS_BLF_UNKNOWN;
        objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;
        objDisplayData.eTextMode = TEXT_MODE_FIRST_ROW;
    }
    else if (sLogicData.iStatus == DS_BLF_STATUS_DND)
    {
        objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
        objDisplayData.eLightFlushType = REFRESH_TYPE_SLOW_FLUSH;
        objDisplayData.eIconType = DK_ICON_BLF_TALK;
        objDisplayData.strIconPath = PIC_DSS_BLF_TALKING;
    }
    else //有通话状态，包含有来电、通话中、未知、hold
    {
        DSSKEYUI_WARN("Dsskey BLF status error!");
    }

    if (objDisplayData.strFirstRowText.empty())
    {
        objDisplayData.strFirstRowText = (LANG_TRANSLATE_DSSKEY_LABEL(dsskey_GetDefaultLabel(sLogicData.eDKType).c_str(), bExt));
    }
}

void Process_UCFavorite(DsskeyDisplayData& objDisplayData, const DssKeyLogicData& sLogicData, bool bExt)
{
    if (1 == configParser_GetConfigInt(kszEnableUC)
            && configParser_GetConfigInt(kszBroadsoftActive) == 1)
    {
        if (sLogicData.strExtension.empty())
        {
            //使用默认联系人头像
            objDisplayData.strIconPath = PIC_DSS_FAVORITES;
            objDisplayData.eIconType = DK_ICON_FAVORITES;
        }
        else
        {
            //使用用户自定义头像
            objDisplayData.strIconPath = TEMP_PATH + sLogicData.strExtension;
            objDisplayData.eIconType = DK_ICON_CUSTOM;
        }

        if (sLogicData.strLabel.empty() && sLogicData.strValue.empty())
        {
            objDisplayData.strFirstRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_FAVOURITE, bExt));
        }
        else
        {
            // UC自定义label 不必翻译
            char szBuffer[MAX_BUF_LEN] = "";
            if (sLogicData.strLabel.empty())
            {
                sprintf(szBuffer, "%s:%s", (LANG_TRANSLATE_DSSKEY_LABEL(TRID_FAVOURITE, bExt) + ":").c_str(), sLogicData.strValue.c_str());
                // 字符串结尾
                szBuffer[MAX_BUF_LEN - 1] = '\0';
                objDisplayData.strFirstRowText = szBuffer;
            }
            else
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel;
            }

            //是否显示新状态图标
            bool bNewIconMode = configParser_GetConfigInt(kszPresenceIconMode) == 1;

            if (sLogicData.iStatus == DS_UC_ONLINE)
            {
                //objDisplayData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_ONLINE : PIC_ICON_PRESENCE_ONLINE;
                objDisplayData.ePresenceType = PRESENCE_STATUS_ONLINE;
            }
            else if (sLogicData.iStatus == DS_UC_CHATTY)
            {
                //objDisplayData.strPresenceIcon = PIC_ICON_PRESENCE_CHATTY;
                objDisplayData.ePresenceType = PRESENCE_STATUS_CHATTY;
            }
            else if (sLogicData.iStatus == DS_UC_AWAY)
            {
                //objDisplayData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_AWAY : PIC_ICON_PRESENCE_AWAY;
                objDisplayData.ePresenceType = PRESENCE_STATUS_AWAY;
            }
            else if (sLogicData.iStatus == DS_UC_EXTENDED_AWAY)
            {
                //objDisplayData.strPresenceIcon = PIC_ICON_PRESENCE_EXTENDEDAWAY;
                objDisplayData.ePresenceType = PRESENCE_STATUS_EXTENDEDAWAY;
            }
            else if (sLogicData.iStatus == DS_UC_BUSY)
            {
                //objDisplayData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_BUSY : PIC_ICON_PRESENCE_BUSY;
                objDisplayData.ePresenceType = PRESENCE_STATUS_BUSY;
            }
            else if (sLogicData.iStatus == DS_UC_OFFLINE)
            {
                //objDisplayData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_OFFLINE : PIC_ICON_PRESENCE_OFFLINE;
                objDisplayData.ePresenceType = PRESENCE_STATUS_OFFLINE;
            }
            else if (sLogicData.iStatus == DS_UC_UNKNOWN)
            {
                //objDisplayData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_UNKNOWN : PIC_ICON_PRESENCE_UNKNOWN;
                objDisplayData.ePresenceType = PRESENCE_STATUS_UNKNOWN;
            }
            else
            {
                //objDisplayData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_OFFLINE : PIC_ICON_PRESENCE_OFFLINE;
                objDisplayData.ePresenceType = PRESENCE_STATUS_OFFLINE;
            }
        }

        DSSKEYUI_INFO("strPath(%s) label(%s) iStatus(%d)", objDisplayData.strIconPath.c_str(),
                      objDisplayData.strFirstRowText.c_str(), sLogicData.iStatus);

        DSSKEYUI_INFO("UC status(%d), presence path (%s)", sLogicData.iStatus, objDisplayData.strPresenceIcon.c_str());

        objDisplayData.eTextMode = TEXT_MODE_FIRST_ROW;
    }
}

void Process_CallParkStatus(DsskeyDisplayData& objDisplayData, const DssKeyLogicData& sLogicData)
{
    YLList<int> listState = sLogicData.listAuxiliaryState;
    if (sLogicData.iStatus == DS_BLF_STATUS_UNKOWN || sLogicData.iStatus == DS_NON)
    {
        // http://10.3.5.199/Bug.php?BugID=61119
        SIP_SERVER_TYPE eServerType = acc_GetServerType(sLogicData.iLineID);
        if (SST_BROADSOFT == eServerType
#if IF_FEATURE_START2START_ACD
                || eServerType == SST_START2START
#endif
           )
        {
            objDisplayData.strIconPath = PIC_DSS_CALLPARK;
            objDisplayData.eIconType = DK_ICON_CALLPARK;
        }
        else
        {
            objDisplayData.strIconPath = PIC_DSS_CALLPARK_FAIL;
            objDisplayData.eIconType = DK_ICON_CALLPARK_FAIL;
        }
    }
    else if (sLogicData.iStatus == DS_BLF_STATUS_IDLE)
    {
        BOOL bBLFLedOnInIdle = (0 == configParser_GetConfigInt(kszBLFLedMode));
        if (bBLFLedOnInIdle)
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
        }

        objDisplayData.strIconPath = PIC_DSS_CALLPARK;
        objDisplayData.eIconType = DK_ICON_CALLPARK;
    }
    else if (sLogicData.iStatus == DS_BLF_STATUS_RINGING)
    {
        objDisplayData.strIconPath = PIC_DSS_CALLPARK_RINGING;
        objDisplayData.eIconType = DK_ICON_CALLPARK_RINGING;
        objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
        objDisplayData.eLightFlushType = REFRESH_TYPE_FLUSH;
    }
    else if (sLogicData.iStatus == DS_BLF_STATUS_TALKING)
    {
        YLList<int>::iterator itr = listState.begin();
        YLList<int>::iterator itrEnd = listState.end();

        BOOL bAutoPickUp = FALSE;
        for (; itr != itrEnd; itr++)
        {
            if (DS_AUTO_PICKUP_ON == *itr)
            {
                bAutoPickUp = TRUE;
            }
        }
        if (bAutoPickUp)
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
            objDisplayData.eLightFlushType = REFRESH_TYPE_SLOW_FLUSH;
            objDisplayData.strIconPath = PIC_DSS_CALLPARK_AUTO_TALK;
            objDisplayData.eIconType = DK_ICON_CALLPARK_AUTO_TALK;
        }
        else
        {
            objDisplayData.strIconPath = PIC_DSS_CALLPARK_MUNUAL_TALK;
            objDisplayData.eIconType = DK_ICON_CALLPARK_MUNUAL_TALK;
            objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
        }
        // page灯状态
        objDisplayData.ePageColorType = LED_COLOR_TYPE_RED;
        objDisplayData.iLightPriority = DSS_KEY_LED_PRIORITY_HIGH;
    }
    else
    {
        DSSKEYUI_WARN("Dsskey CallPark status error!");
    }

    objDisplayData.eTextMode = TEXT_MODE_FIRST_ROW;
}

void Process_PagingStatus(DsskeyDisplayData& objDisplayData, const DssKeyLogicData& sLogicData, bool bExt)
{
    objDisplayData.strIconPath = PIC_DSS_PAGING;
    objDisplayData.eIconType = DK_ICON_PAGING;
    if (TRANS_ALL == sLogicData.iTranslateLength)
    {
        objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ? \
                                         sLogicData.strValue : (LANG_TRANSLATE_DSSKEY_LABEL(sLogicData.strLabel.c_str(), bExt));
    }
    else
    {
        objDisplayData.strFirstRowText = sLogicData.strLabel.empty() ? \
                                         sLogicData.strValue : sLogicData.strLabel;
    }

    objDisplayData.eTextMode = TEXT_MODE_FIRST_ROW;

    YLList<int> listState = sLogicData.listAuxiliaryState;
    YLList<int>::iterator itr = listState.begin();
    YLList<int>::iterator itrEnd = listState.end();

    Dsskey_Status eTalkStatu = DS_NON;
    for (; itr != itrEnd; itr++)
    {
        if (*itr > DS_SESSION_FIRST
                && *itr < DS_SESSION_END)
        {
            eTalkStatu = (Dsskey_Status)(*itr);
        }
    }


    if (DS_SESSION_NORMAL_USING == eTalkStatu
            || DS_SESSION_TALK == eTalkStatu
            || DS_SESSION_CALL_Out == eTalkStatu) // 通话状态暂时和DS_SESSION_NORMAL_USING一样
    {
        // dsskey状态
        objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
    }
    else if (DS_SESSION_HOLD == eTalkStatu)
    {
        // dsskey状态
        objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
        objDisplayData.eLightFlushType = REFRESH_TYPE_FLUSH;
    }

    if (eTalkStatu != DS_NON)
    {
        objDisplayData.strIconPath = PIC_DSS_LINE_USING;
        objDisplayData.eIconType = DK_ICON_Using;
        objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_USING;
    }

    else if (DS_SESSION_CALL_IN == eTalkStatu) // T46在来电中常亮红灯
    {
        // dsskey状态
        objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
        objDisplayData.eLightFlushType = REFRESH_TYPE_FAST_FLUSH;
    }
    else if (DS_SESSION_DIAL == eTalkStatu)
    {
        // dsskey状态
        objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
    }
}

void Process_IntercomStatus(DsskeyDisplayData& objDisplayData, const DssKeyLogicData& sLogicData, bool bExt)
{
    int iAuxiliaryStatus = -1;
    YLList<int> listState = sLogicData.listAuxiliaryState;
    if (listState.begin() != listState.end())
    {
        iAuxiliaryStatus = *listState.begin();
    }
    GetBlfLightState(objDisplayData, sLogicData.iStatus, iAuxiliaryStatus);
    DSSKEYUI_INFO("Intercom status is %d.", sLogicData.iStatus);
    if (0 == configParser_GetConfigInt(kszSubscribeIntercom))
    {
        if (exp_GetExpColor())
        {
            objDisplayData.eIconType = DK_ICON_INTERCOM;
        }
        else
        {
            objDisplayData.eIconType = DK_ICON_Remote_Online;
        }

        objDisplayData.strIconPath = PIC_DSS_INTERCOM;
    }
    else if (sLogicData.iStatus == DS_BLF_STATUS_IDLE)
    {
        if (exp_GetExpColor())
        {
            objDisplayData.eIconType = DK_ICON_INTERCOM;
        }
        else
        {
            objDisplayData.eIconType = DK_ICON_Remote_Online;
        }

        objDisplayData.strIconPath = PIC_DSS_INTERCOM;
        objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;
        // 显示文本外部已做处理
        objDisplayData.eTextMode = TEXT_MODE_FIRST_ROW;
    }
    else if (sLogicData.iStatus == DS_BLF_STATUS_RINGING)
    {
        if (exp_GetExpColor())
        {
            objDisplayData.eIconType = DK_ICON_INTERCOM_RINGING;
        }
        else
        {
            objDisplayData.eIconType = DK_ICON_Remote_Ringing;
        }

        objDisplayData.strIconPath = PIC_DSS_INTERCOM_RINGING;
    }
    else if (sLogicData.iStatus == DS_BLF_STATUS_TALKING || sLogicData.iStatus == DS_BLF_STATUS_CALLOUT
             || sLogicData.iStatus == DS_BLF_STATUS_DND)
    {
        if (exp_GetExpColor())
        {
            objDisplayData.eIconType = DK_ICON_INTERCOM_TALKING;
        }
        else
        {
            objDisplayData.eIconType = DK_ICON_Remote_Talking;
        }

        objDisplayData.strIconPath = PIC_DSS_INTERCOM_TALKING;
    }
    else if (sLogicData.iStatus == DS_BLF_STATUS_HOLD)
    {
        if (exp_GetExpColor())
        {
            objDisplayData.eIconType = DK_ICON_INTERCOM_HOLD;
        }
        else
        {
            objDisplayData.eIconType = DK_ICON_Remote_Talking;
        }

        objDisplayData.strIconPath = PIC_DSS_INTERCOM_HOLD;
    }
    else if (sLogicData.iStatus == DS_BLF_STATUS_UNKOWN || sLogicData.iStatus == DS_NON)
    {
        objDisplayData.eIconType = DK_ICON_Remote_Offline;
        objDisplayData.strIconPath = PIC_DSS_INTERCOM_UNKNOWN;
    }
    else //有通话状态，包含有来电、通话中、未知、hold
    {
        objDisplayData.eIconType = DK_ICON_Remote_Offline;
        objDisplayData.strIconPath = PIC_DSS_INTERCOM_UNKNOWN;
        DSSKEYUI_WARN("Dsskey BLF status error!");
    }
}

void Process_URLStatus(DsskeyDisplayData& objDisplayData, const DssKeyLogicData& sLogicData)
{
    objDisplayData.eLightFlushType = REFRESH_TYPE_FAST_FLUSH;
    objDisplayData.iFlushuration = 5000; // 持续5秒
    switch (sLogicData.iStatus)
    {
    case DS_URL_OK:
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
            objDisplayData.strIconPath = PIC_DSS_URL_GREEN;
            objDisplayData.eIconType = DK_ICON_URL_GREEN;
        }
        break;
    case DS_URL_NETWORK_UNAVAILABLE:
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
            objDisplayData.strIconPath = PIC_DSS_URL_RED;
            objDisplayData.eIconType = DK_ICON_URL_RED;
        }
        break;
    // url 未知需红绿交替闪烁 - UI须支持该状态
    case DS_URL_NETWORK_UNKNOW:
        {
            objDisplayData.eLightColorType = LED_COLOR_RED_GREEN;
            objDisplayData.strIconPath = PIC_DSS_URL;
            objDisplayData.eIconType = DK_ICON_URL;
        }
        break;
    default:
        {
            objDisplayData.strIconPath = PIC_DSS_URL;
            objDisplayData.eIconType = DK_ICON_URL;
            objDisplayData.eLightFlushType = REFRESH_TYPE_NO_FLUSH;
            objDisplayData.iFlushuration = 0; // 持续5秒
        }
        break;
    }
    objDisplayData.eTextMode = TEXT_MODE_FIRST_ROW;
}

void Process_XMLBrowserStatus(DsskeyDisplayData& objDisplayData, const DssKeyLogicData& sLogicData)
{
    objDisplayData.eLightFlushType = REFRESH_TYPE_NO_FLUSH;
    objDisplayData.iFlushuration = 3000; // 持续3秒
    switch (sLogicData.iStatus)
    {
    case DS_XMLBROSER_LOAD_FINISH:
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
        }
        break;
    case DS_XMLBROSER_LOAD_ERROE:
        {
            objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
            objDisplayData.eLightFlushType = REFRESH_TYPE_FLUSH;
        }
        break;
    default:
        {
            objDisplayData.iFlushuration = 0;
        }
        break;
    }
}

void Process_BTLINEStatus(DsskeyDisplayData& objDisplayData, const DssKeyLogicData& sLogicData, bool bExt)
{
    yl::string strLabel = sLogicData.strLabel;
    int nBTLineStatus = sLogicData.iStatus;

    if (DS_BTLINE_USING == nBTLineStatus)
    {
        objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_HIGHLIGHT;
    }
    else
    {
        objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;
    }

    if (DS_BTLINE_OFFLINE == nBTLineStatus)
    {
        if (strLabel.empty())
        {
            objDisplayData.strFirstRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_MOBILE_DSSKEY_LABEL, bExt));
        }
        else
        {
            objDisplayData.strFirstRowText = strLabel;
        }
    }
    else if (DS_BTLINE_CONNECTTING == nBTLineStatus)
    {
        if (strLabel.empty())
        {
            objDisplayData.strFirstRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_CONNECTING, bExt));
        }
        else
        {
            objDisplayData.strFirstRowText = strLabel;
        }
    }
    else if (DS_BTLINE_ONLINE == nBTLineStatus
             || DS_BTLINE_USING == nBTLineStatus)
    {
        if (strLabel == TRID_MOBILE_DSSKEY_LABEL
                || strLabel.empty())
        {
            objDisplayData.strFirstRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_BTLINE, bExt));
        }
        else
        {
            objDisplayData.strFirstRowText = strLabel;
        }
    }
    else
    {
        objDisplayData.strFirstRowText = "";
    }

    if (sLogicData.iStatus == DS_BTLINE_USING
            || sLogicData.iStatus == DS_BTLINE_ONLINE)
    {
        objDisplayData.strIconPath = PIC_DSS_MOBILE_BLUE_GREEN;
        objDisplayData.eIconType = DK_ICON_MOBILE_BLUE_GREEN;
    }
    else if (sLogicData.iStatus == DS_BTLINE_CONNECTTING)
    {
        objDisplayData.strIconPath = PIC_DSS_MOBILE_BLUE_CONNETING;
        objDisplayData.eIconType = DK_ICON_MOBILE_BLUE_CONNETING;
    }
    else if (sLogicData.iStatus == DS_BTLINE_OFFLINE)
    {
        objDisplayData.strIconPath = PIC_DSS_MOBILE_BLUE_RED;
        objDisplayData.eIconType = DK_ICON_MOBILE_BLUE_RED;
    }
    else
    {
        objDisplayData.strIconPath = " ";
        objDisplayData.eIconType = DK_ICON_None;
    }
}

bool GetDsskeyShortenText(DsskeyDisplayData& objDisplayData, DssKeyLogicData& sLogicData)
{
    if (!sLogicData.strShortLabel.empty())
    {
        yl::string strShortLabel = sLogicData.strShortLabel;
        objDisplayData.strShortenText = strShortLabel.substr(0, SHORTEN_DSSKEY_CHARATER_LENGTH);
        return true;
    }

    if (objDisplayData.strFirstRowText.empty())
    {
        return false;
    }

    yl::string strRealText = _TRAN((objDisplayData.strFirstRowText));

    yl::string strSpecialCharacter;
    for (int nIndex = 0; nIndex < strRealText.size(); ++nIndex)
    {
        if ((strRealText.at(nIndex) >= '0' && strRealText.at(nIndex) <= '9') ||
                (strRealText.at(nIndex) >= 'A' && strRealText.at(nIndex) <= 'Z'))
        {
            strSpecialCharacter += strRealText.at(nIndex);
        }

        if (strSpecialCharacter.size() >= SHORTEN_DSSKEY_CHARATER_LENGTH)
        {
            break;
        }
    }

    if (strSpecialCharacter.empty())
    {
        objDisplayData.strShortenText = strRealText.substr(0, SHORTEN_DSSKEY_CHARATER_LENGTH);
    }
    else
    {
        objDisplayData.strShortenText = strSpecialCharacter.substr(0, SHORTEN_DSSKEY_CHARATER_LENGTH);
    }

    return true;
}

bool CheckLightChange(DsskeyDisplayData& objDisplayData, DssKeyLogicData& sLogicData)
{
    bool bChanged = false;

    if (sLogicData.eDKType != DT_LINE)
    {
        return true;
    }

    if (objDisplayData.iStatus != sLogicData.iStatus)
    {
        bChanged = true;
    }
    else
    {
        if (objDisplayData.listAuxiliaryState.size() != sLogicData.listAuxiliaryState.size())
        {
            bChanged = true;
        }
        else
        {
            YLList<int>::iterator olditr = objDisplayData.listAuxiliaryState.begin();
            YLList<int>::iterator olditrEnd = objDisplayData.listAuxiliaryState.end();
            YLList<int>::iterator newitr = sLogicData.listAuxiliaryState.begin();
            YLList<int>::iterator newitrEnd = sLogicData.listAuxiliaryState.end();
            for (; olditr != olditrEnd && newitr != newitrEnd; olditr++, newitr++)
            {
                if (*olditr != *newitr)
                {
                    bChanged = true;
                    break;
                }
            }
        }
    }

    return bChanged;

}

bool dsskeyUI_GetDataForUI(int iIndex, DsskeyDisplayData& objDisplayData, int iEXPIndex/* = -1*/)
{
    DssKeyLogicData sData;
    bool bRet = false;
    // 获取逻辑层返回的Dsskey数据
    int iDsskeyID = -1;
    if (iEXPIndex == -1)
    {
        iDsskeyID = dsskey_GetDsskeyID(DMT_LINEKEY_MODULE, iEXPIndex, iIndex);
    }
    else
    {
        iDsskeyID = dsskey_GetDsskeyID((DSSKEY_MODULE_TYPE)exp_GetExpColor(), iEXPIndex, iIndex);
    }

    // 获取Dsskey逻辑数据
    bRet = dsskey_GetData(iDsskeyID, sData);

    if (bRet)
    {
        // 转换为显示数据
        return dsskeyUI_ParseLogicData(objDisplayData, sData);
    }
    else
    {
        return false;
    }
}


bool dsskeyUI_ParseLogicData(DsskeyDisplayData & objDisplayData, const DssKeyLogicData & sLogicData)
{
    //return true;
    // 数据有效性验证
    if (sLogicData.eDKType < DT_NONE
            || sLogicData.eDKType > DT_LAST)
    {
        DSSKEYUI_WARN("ParseLogicData Dsskey eDKType error");
        return false;
    }

    DssKeyLogicData newLogicData = sLogicData;
    bool bLightChange = CheckLightChange(objDisplayData, newLogicData);
    DSSKEYUI_WARN("ParseLogicData bLightChange(%d)", bLightChange);
    DSS_KEY_COLOR_TYPE eOldColorType = objDisplayData.eLightColorType;

    // 清空状态图片，防止无状态功能也显示状态
    objDisplayData.listAuxiliaryState = sLogicData.listAuxiliaryState;
    objDisplayData.strPresenceIcon = "";
    objDisplayData.strFlushIcon1 = "";
    objDisplayData.strFlushIcon2 = "";
    // 清空闪烁状态，防止功能改变闪烁还存在
    objDisplayData.eRefreshType = REFRESH_TYPE_NO_FLUSH;
    objDisplayData.eLightColorType = LED_COLOR_OFF; // dsskey灯颜色
    objDisplayData.eLightFlushType = REFRESH_TYPE_NO_FLUSH; // dsskey灯闪烁速度
    objDisplayData.ePageColorType = LED_COLOR_OFF; // 对应的page灯状态
    objDisplayData.ePageLightFlushType = REFRESH_TYPE_NO_FLUSH;// 对应的page灯状态
    objDisplayData.iDsskeyID = sLogicData.iDssKeyID;
    objDisplayData.eIconType = DK_ICON_Default;
    objDisplayData.ePresenceType = PRESENCE_STATUS_NONE;
    objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;
    objDisplayData.eTextMode = TEXT_MODE_FIRST_ROW;
    objDisplayData.iFlushuration = 0; // 默认定时灭灯
    // 设置显示类型
    objDisplayData.eDssKeytype = sLogicData.eDKType;
    objDisplayData.iStatus = sLogicData.iStatus;

    // 判断是否扩展台 ，扩展台不翻译
    bool bExt = IsEXPModule(sLogicData.eType) && IsSupportModuleType(sLogicData.eType);

    DSSKEYUI_INFO("[%s] [%d]: Type is %d, Label is %s, iStatus is %d, ", (bExt
                  || sLogicData.eType == DMT_EXP50_MODULE) ? "Exp key" : "Line key", \
                  objDisplayData.iIndex, sLogicData.eDKType, sLogicData.strLabel.c_str(), sLogicData.iStatus);

    yl::string strDsskeyLabel;
    int iDsskeyTranslateLen = TRANS_ALL;
    bool bLabel = true;

    if (sLogicData.strLabel.empty())
    {
        // 不是使用外部设置的label
        bLabel = false;
        // 为空需手动获取默认的名称
        strDsskeyLabel = dsskey_GetDefaultLabel(sLogicData.eDKType);
        //      iDsskeyTranslateLen = strlen(sLogicData.strLabel.c_str());
        // TODO???
        iDsskeyTranslateLen = strlen(sLogicData.strLabel.c_str());
    }
    else
    {
        strDsskeyLabel = sLogicData.strLabel;
        iDsskeyTranslateLen = sLogicData.iTranslateLength;
    }

    // 普通只有一种状态的Dsskey按键，在这边处理是否需要进行翻译
    if (TRANS_ALL == iDsskeyTranslateLen)
    {
        objDisplayData.strFirstRowText = (LANG_TRANSLATE_DSSKEY_LABEL(strDsskeyLabel.c_str(), bExt));
    }
    else
    {
        objDisplayData.strFirstRowText = strDsskeyLabel;
    }

    switch (sLogicData.eDKType)
    {
    case DT_LINE:
        {
            objDisplayData.eTextMode = TEXT_MODE_FIRST_ROW;  // 设置显示行数
            if (sLogicData.iStatus == DS_LINE_NONE)
            {
                char lpLabel[128] = "";
                sprintf(lpLabel, "%s%d", "Line ", sLogicData.iLineID + 1);

                objDisplayData.strFirstRowText = (LANG_TRANSLATE_DSSKEY_LABEL(lpLabel,
                                                  bExt));// 设置显示Label

            }
            else
            {
                if (TRANS_ALL == sLogicData.iTranslateLength)
                {
                    objDisplayData.strFirstRowText = (LANG_TRANSLATE_DSSKEY_LABEL(sLogicData.strLabel.c_str(),
                                                      bExt));// 设置显示Label
                }
                else
                {
                    objDisplayData.strFirstRowText = sLogicData.strLabel;
                }
            }

            //V81Beta账号名称全局显示统一需求
            if (objDisplayData.strFirstRowText.empty())
            {
                objDisplayData.strFirstRowText = acc_GetAccountShowText(sLogicData.iLineIndex);
            }

            // 设置扩展台图标
            switch (sLogicData.iStatus)
            {
            case DS_LINE_UNREGISTERED:
                objDisplayData.eIconType = DK_ICON_UnRegistered;
                break;
            case DS_LINE_REGISTERING:
                objDisplayData.eIconType = DK_ICON_Registering;
                break;
            case DS_LINE_REGISTERED:
                objDisplayData.eIconType = DK_ICON_Registered;
                break;
            case DS_LINE_USING:
                objDisplayData.eIconType = DK_ICON_Using;
                break;
            case DS_LINE_NONE:
            case DS_LINE_DISABLE:
                objDisplayData.eIconType = DK_ICON_UnRegistered;
                break;
            case DS_LINE_FWD:
                objDisplayData.eIconType = DK_ICON_Registered;
                break; //!!!!! FWD和DND需要扩展台图标吗？
            case DS_LINE_DND:
                objDisplayData.eIconType = DK_ICON_Registered;
                break;
            default:
                objDisplayData.eIconType = DK_ICON_None;
                break;
            }

            // 获取真实的账号 ？？caijz
            int nLineID = sLogicData.iLineIndex;
            if (nLineID == -1 && _AccountManager.GetUsableAccount(true) != NULL)
            {
                //如果是自动账号则主动获取其ID
                nLineID = _AccountManager.GetUsableAccount(true)->GetAccountId();
            }

            // 判断当前账号是否处于ACD状态
            bool bUseACDStatus = false;
            bool bUseSCAStauts = false;
            if (CallCenter_IsACDOn())
            {
                CSipAccount * pCurrentAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(
                                                    nLineID));
                //支持ACD且开启ACD的账号是当前账号且其SipServer类型不为默认时，才显示ACD相关内容
                if (pCurrentAccount != NULL
                        && nLineID == ACD_GetAccountId())
                {
                    bUseACDStatus = true;
                }
            }

            CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(
                                         nLineID));
            if (pAccount != NULL
                    && pAccount->IsAccountUsable()
                    && acc_IsBLAEnable(pAccount->GetAccountId()))
            {
                bUseSCAStauts = true;
            }

            // 设置Linekey图标
            if (bUseACDStatus)
            {
                Process_LineACD(objDisplayData);
            }
            else if (bUseSCAStauts)
            {
                Process_LineSCA(objDisplayData, sLogicData, bExt);
            }
            else
            {
                Process_LineNormal(objDisplayData, sLogicData);
            }

            if (sLogicData.iStatus == DS_LINE_USING)
            {
                // 设置底色
                objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_USING;
            }
            else if (_AccountManager.GetDefaultAccountID() == nLineID)
            {
                // 默认账号使用高亮底色
                objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_HIGHLIGHT;
            }
            else
            {
                objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;
            }
            // 只有UC开启,为账号1的和XMPP也连接上的时候才设置头像和状态
            Process_LineUC(objDisplayData, sLogicData);

            if ((DS_LINE_DISABLE == sLogicData.iStatus)
                    || (DS_LINE_NONE == sLogicData.iStatus))
            {
                objDisplayData.strDownIconPath = "";
                objDisplayData.strFirstRowText = "";
                objDisplayData.strIconPath = "";
                objDisplayData.eIconType = DK_ICON_None;
                objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;
            }
            Process_LineStatus(objDisplayData, sLogicData, bUseSCAStauts);
        }
        break;
    case DT_SPEEDDIAL:
        {
            objDisplayData.eIconType = DK_ICON_Speeddial;
            objDisplayData.strIconPath = PIC_DSS_SPEED_DIAL;
            ProcessSessionStatus(objDisplayData, sLogicData);
            if (!sLogicData.strValue.empty() && !bLabel)//不是空
            {
                objDisplayData.strFirstRowText = sLogicData.strValue;
            }
        }
        break;
    case DT_BLF:
    case DT_BLFLIST: //按照需求，BLFLIST和BLF显示一样
#ifdef IF_FEATURE_GENBAND
    case DT_BLFGROUP:
#endif
        {
            Process_BLFStatus(objDisplayData, sLogicData, bExt);
        }
        break;
    case DT_VOICEMAIL:
        {
            objDisplayData.strIconPath = PIC_DSS_VOICE_MAIL;
            objDisplayData.eIconType = DK_ICON_VOICE_MAIL;
        }
        break;
    case DT_PICKUP:
        {
            // 处理绑定会话功能状态
            ProcessSessionStatus(objDisplayData, sLogicData);
            objDisplayData.strIconPath = PIC_DSS_PICKUP;
            objDisplayData.eIconType = DK_ICON_PICK_UP;
        }
        break;
    case DT_GROUPPICKUP:
        {
            // 处理绑定会话功能状态
            ProcessSessionStatus(objDisplayData, sLogicData);
            objDisplayData.strIconPath = PIC_DSS_GROUPPICKUP;
            objDisplayData.eIconType = DK_ICON_GROUP_PICK_UP;
        }
        break;
    case DT_CALLPARK:
        {
            Process_CallParkStatus(objDisplayData, sLogicData);
        }
        break;
    case DT_INTERCOM:
        {
            Process_IntercomStatus(objDisplayData, sLogicData, bExt);
        }
        break;
    case DT_CUSTOM: //DTMF
        {
            objDisplayData.strIconPath = PIC_DSS_DTMF;
            objDisplayData.eIconType = DK_ICON_DTMF;
        }
        break;
    case DT_PREFIX:
        {
            objDisplayData.strIconPath = PIC_DSS_PREFIX_ICON;
            objDisplayData.eIconType = DK_ICON_PREFIX_ICON;
            if (sLogicData.strLabel.empty())
            {

                // 预拨号由于分2段显示，在这边做处理，无须判断逻辑的翻译长度值
                char szBuffer[MAX_BUF_LEN] = "";
                sprintf(szBuffer, "%s:%s", (LANG_TRANSLATE_DSSKEY_LABEL(TRID_PREFIX, bExt)).c_str(),
                        sLogicData.strValue.c_str());
                objDisplayData.strFirstRowText = szBuffer;

            }
            else
            {
                objDisplayData.strFirstRowText = sLogicData.strLabel;
            }
        }
        break;
    case DT_LOCALGROUP:
    case DT_XMLGROUP:
    case DT_BSGROUP:
        {
            yl::string strContactName = dsskeyuilogic_GetContactLabel(sLogicData);
            objDisplayData.strIconPath = PIC_DSS_LOCALGROUP;
            objDisplayData.eIconType = DK_ICON_LOCAL_GROUP;

            if (!strContactName.empty() && !bLabel)
            {
                if (bExt)
                {
                    // 联系人群组名称获取时已经做了LANG_TRANSLATE,在扩展台必须直接LANG_PARSE进行翻译
                    // TODO PARSE?
                    //objDisplayData.strFirstRowText = (LANG_PARSE(strContactName.c_str())).toUtf8().data();
                    objDisplayData.strFirstRowText = (_TRAN(strContactName.c_str()));
                }
                else
                {
                    objDisplayData.strFirstRowText = strContactName;
                }
            }
        }
        break;
#if IF_BUG_31595
    case DT_XMLBROWSER_INFO:
#endif
    case DT_XMLBROWSER:
        {
            objDisplayData.strIconPath = PIC_DSS_XML_BROWSER;
            objDisplayData.eIconType = DK_ICON_XMLBROWSER;

#ifdef IF_FEATURE_GENBAND
            Process_XMLBrowserStatus(objDisplayData, sLogicData);
#endif
        }
        break;
    case DT_LDAP:
        {
            objDisplayData.strIconPath = PIC_DSS_LDAP;
            objDisplayData.eIconType = DK_ICON_LDAP;
        }
        break;
    case DT_FAVORITES:
        {
            objDisplayData.strIconPath = PIC_DSS_FAVORITES;
            objDisplayData.eIconType = DK_ICON_FAVORITES;
        }
        break;
    case DT_CONF:
        {
            objDisplayData.strIconPath = PIC_DSS_CONFERENCE;
            objDisplayData.eIconType = DK_ICON_CONFERENCE;
        }
        break;
    case DT_FWD:
        {
            if (sLogicData.iStatus == DS_FWD_USING)
            {
#if IF_FEATURE_32591
                objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
#else
                objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
#endif
            }
            objDisplayData.strIconPath = PIC_DSS_FORWARD;
            objDisplayData.eIconType = DK_ICON_Forward;
        }
        break;
    case DT_TRAN:
        {
            objDisplayData.strIconPath = PIC_DSS_TRANSFER;
            objDisplayData.eIconType = DK_ICON_TRANSFER;
        }
        break;
    case DT_HOLD:
        {
            objDisplayData.eIconType = DK_ICON_HOLD;
            objDisplayData.strIconPath = PIC_DSS_HOLD;
        }
        break;
    case DT_DND:
        {
#if IF_FEATURE_32591
            if (sLogicData.iStatus == DS_DND_USING)
            {
                objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
                objDisplayData.strIconPath = PIC_DSS_DND;
            }
            else
            {
                objDisplayData.eLightColorType = LED_COLOR_OFF;
                objDisplayData.strIconPath = PIC_DSS_DND_OFF;
            }
            objDisplayData.eIconType = DK_ICON_DND;
#else
            if (sLogicData.iStatus == DS_DND_USING)
            {
                objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
            }
            objDisplayData.eIconType = DK_ICON_DND;
            objDisplayData.strIconPath = PIC_DSS_DND;
#endif
        }
        break;
    case DT_REDIAL:
        {
            objDisplayData.strIconPath = PIC_DSS_REDIAL;
            objDisplayData.eIconType = DK_ICON_REDIAL;
        }
        break;
    case DT_CALLRETURN:
        {
            objDisplayData.strIconPath = PIC_DSS_CALLRETURN;
            objDisplayData.eIconType = DK_ICON_CALLRETURN;
        }
        break;
    case DT_SMS:
        {
            objDisplayData.eIconType = DK_ICON_SMS;
            objDisplayData.strIconPath = PIC_DSS_SMS;
        }
        break;
    case DT_RECORD:
    case DT_URLRECORD:
    case DT_VOICERECORD:
        {
            if (DS_RECORD_USING == sLogicData.iStatus)
            {
                objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
                objDisplayData.eLightFlushType = REFRESH_TYPE_FLUSH;
            }

            if (DS_RECORD_USING == sLogicData.iStatus)
            {
                objDisplayData.strIconPath = PIC_DSS_RECORDING;
                objDisplayData.eIconType = DK_ICON_RECORDING;
            }
            else
            {
                objDisplayData.strIconPath = PIC_DSS_RECORD;
                objDisplayData.eIconType = DK_ICON_RECORD;
            }
            objDisplayData.iLightPriority = DSS_KEY_LED_PRIORITY_LOW;

#if IF_FEATURE_KOIP
            if (sLogicData.eDKType == DT_RECORD && DS_RECORD_USING == sLogicData.iStatus)
            {
                objDisplayData.strFirstRowText = (_LANG(TRID_STOP));
            }
#endif
        }
        break;
    case DT_PAGING:
        {
            Process_PagingStatus(objDisplayData, sLogicData, bExt);
            if (!sLogicData.strValue.empty() && !bLabel)//不是空
            {
                objDisplayData.strFirstRowText = sLogicData.strValue;
            }
        }
        break;
    case DT_PAGING_LIST:
        {
            objDisplayData.strIconPath = PIC_DSS_PAGING;
            objDisplayData.eIconType = DK_ICON_PAGING;
        }
        break;
    case DT_GROUPLISTEN:
        {
            objDisplayData.strIconPath = PIC_DSS_GROUP_LISTENING;
            objDisplayData.eIconType = DK_ICON_GROUP_LISTENING;
        }
        break;
    case DT_PUBLICHOLD:
        {
            objDisplayData.eIconType = DK_ICON_HOLD;
            objDisplayData.strIconPath = PIC_DSS_PUBLIC_HOLD;
        }
        break;
    case DT_PRIVATEHOLD:
        {
            objDisplayData.eIconType = DK_ICON_HOLD;
            objDisplayData.strIconPath = PIC_DSS_PRIVATE_HOLD;
        }
        break;
    case DT_ACD:
        {
            switch (sLogicData.iStatus)
            {
#if IF_BUG_24860
            case DS_ACD_AVAILABLE:
            case DS_ACD_UNAVAILABLE:
            case DS_ACD_WRAPUP:
                {
                    objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
                    objDisplayData.strIconPath = PIC_DSS_ACD_AVAILABLE;
                    objDisplayData.eIconType = DK_ICON_COLOR_ACD_AVAILABLE;
                }
                break;
#else
            case DS_ACD_AVAILABLE:
                {
                    objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
                    objDisplayData.strIconPath = PIC_DSS_ACD_AVAILABLE;
                    objDisplayData.eIconType = DK_ICON_COLOR_ACD_AVAILABLE;
                }
                break;
            case DS_ACD_UNAVAILABLE:
                {
                    objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
                    objDisplayData.eLightFlushType = REFRESH_TYPE_FLUSH;
                    objDisplayData.strIconPath = PIC_DSS_ACD_UNAVAILABLE;
                    objDisplayData.eIconType = DK_ICON_COLOR_ACD_UNAVAILABLE;
                }
                break;
            case DS_ACD_WRAPUP:
                {
                    objDisplayData.eLightColorType = LED_COLOR_TYPE_RED;
                    objDisplayData.strIconPath = PIC_DSS_ACD_WARP_UP;
                    objDisplayData.eIconType = DK_ICON_COLOR_ACD_WRAPUP;
                }
                break;
#endif
            case DS_ACD_IDLE:
                {
                    objDisplayData.strIconPath = PIC_DSS_ACD_LOGOUT;
                    objDisplayData.eIconType = DK_ICON_ACD_LOGOUT;
                }
                break;
            default:
                {
                    objDisplayData.strIconPath = PIC_DSS_ACD;
                    objDisplayData.eIconType = DK_ICON_ACD_DEFAULT;
                }
                break;
            }
        }
        break;
    case DT_ZERO:
        {
            objDisplayData.strIconPath = PIC_DSS_ZERO_TOUCH;
            objDisplayData.eIconType = DK_ICON_ZERO_TOUCH;
        }
        break;
    case DT_URL:
        {
            Process_URLStatus(objDisplayData, sLogicData);
        }
        break;
    case DT_MEET_ME_CONFERENCE:
        {
            // 处理绑定会话功能状态
            ProcessSessionStatus(objDisplayData, sLogicData);
            objDisplayData.strIconPath = PIC_DSS_MEET_ME;
            objDisplayData.eIconType = DK_ICON_MEET_ME;
        }
        break;
    case DT_HOTELING:
        {
            if (DS_HOTELING_ON == sLogicData.iStatus)
            {
                objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
                objDisplayData.strIconPath = PIC_DSS_HOTELING_LOGININ;
                objDisplayData.eIconType = DK_ICON_HOTELING_LOGININ;
            }
            else
            {
                objDisplayData.strIconPath = PIC_DSS_HOTELING_LOGINOUT;
                objDisplayData.eIconType = DK_ICON_HOTELING_LOGINOUT;
            }
        }
        break;
    case DT_ACD_TRACE:
        {
            objDisplayData.strIconPath = PIC_DSS_ACD_TRACE;
            objDisplayData.eIconType = DK_ICON_COLOR_ACD_TRACE;
        }
        break;
    case DT_DISPCODE:
        {
            objDisplayData.strIconPath = PIC_DSS_DISPCODE;
            objDisplayData.eIconType = DK_ICON_DISPCODE;
        }
        break;
    case DT_ESCALATE:
        {
            objDisplayData.strIconPath = PIC_DSS_ESCALATE;
            objDisplayData.eIconType = DK_ICON_ESCALATE;
        }
        break;
    case DT_KEYPAD_LOCK:
        {
            objDisplayData.strIconPath = PIC_DSS_KEYPAD_LOCK;
            objDisplayData.eIconType = DK_ICON_KEYPAD_LOCK;
            objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;
        }
        break;
    case DT_BSFT_RETRIEVE_PARK:
        {
            objDisplayData.strIconPath = PIC_DSS_RETRIEVE_PARK;
            objDisplayData.eIconType = DK_ICON_RETRIEVE_PARK;
            objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;
        }
        break;
    case DT_HOTDESKING:
        {
            objDisplayData.strIconPath = PIC_DSS_HOTDESKING;
            objDisplayData.eIconType = DK_ICON_HOTDESKING;
            objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;
        }
        break;
    case DT_NETWORK_FAVORITE:
        {
            if (1 == configParser_GetConfigInt(kszEnableUC)
                    && configParser_GetConfigInt(kszBroadsoftActive) == 1)
            {
                objDisplayData.strIconPath = PIC_DSS_FAVORITES;
                objDisplayData.eIconType = DK_ICON_FAVORITES;
            }
        }
        break;
    case DT_BUDDIES:
        {
            if (1 == configParser_GetConfigInt(kszEnableUC)
                    && configParser_GetConfigInt(kszBroadsoftActive) == 1)
            {
                objDisplayData.strIconPath = PIC_DSS_BUDDIES;
                objDisplayData.eIconType = DK_ICON_BUDDIES;
            }
        }
        break;
    case DT_MY_STATUS:
        {
            if (1 == configParser_GetConfigInt(kszEnableUC)
                    && configParser_GetConfigInt(kszBroadsoftActive) == 1)
            {
                objDisplayData.strIconPath = PIC_DSS_STATUS;
                objDisplayData.eIconType = DK_ICON_STATUS;
            }
        }
        break;
    case DT_UC_FAVORITE:
        {
            Process_UCFavorite(objDisplayData, sLogicData, bExt);
        }
        break;
    case DT_CUSTOM_KEY:
        {
            objDisplayData.strIconPath = PIC_DSS_EDK;
            objDisplayData.eIconType = DK_ICON_EDK;

            yl::string strEDKLabel = Edk_GetDsskeyData(sLogicData.iDssKeyID, EDK_LABEL);

            if (!strEDKLabel.empty())
            {
                objDisplayData.strFirstRowText = strEDKLabel;
            }
            else
            {
                objDisplayData.strFirstRowText = (_TRAN(dsskey_GetDefaultLabel(
                        DT_CUSTOM_KEY).c_str()));
            }
        }
        break;
#ifdef IF_BT_SUPPORT_PHONE
    case DT_BTLINE:
        {
            Process_BTLINEStatus(objDisplayData, sLogicData, bExt);
        }
        break;
#endif
#if IF_BUG_14856
    case DT_MISSED_CALL:
        {
            objDisplayData.strIconPath = PIC_DSS_LOCALGROUP;
            objDisplayData.eIconType = DK_ICON_LOCAL_GROUP;
        }
        break;
#endif
#if IF_BUG_PLCMDIR
    case DT_PLCM_PHONEBOOK:
        {
            objDisplayData.strIconPath = PIC_DSS_LOCALGROUP;
            objDisplayData.eIconType = DK_ICON_DIRECTORY;
        }
        break;
#endif
    case DT_EXP_SWITCH:
        break;
    case DT_GET_BALANCE:
#ifdef IF_FEATURE_GETBALANCE
        {
            if (1 == configParser_GetConfigInt(kszGetBalanceEnable))
            {
                objDisplayData.eIconType = DK_ICON_GET_BALANCE;
                objDisplayData.eDssKeytype = sLogicData.eDKType;
                objDisplayData.strIconPath = PIC_DSS_GET_BALANCE;
                objDisplayData.eTextMode = TEXT_MODE_FIRST_ROW;
                //if (!sLogicData.strValue.empty() && !bLabel)//不是空
                //{
                //  objDisplayData.strFirstRowText = sLogicData.strValue;
                //}
            }
        }
#endif
        break;
#if IF_FEATURE_SRTP
    case DT_SRTP:
        {
            objDisplayData.strIconPath = PIC_DSS_ENCRYPT_ICON;
            if (DS_SRTP_COMPLUSORY == objDisplayData.iStatus)
            {
                objDisplayData.eLightColorType = LED_COLOR_TYPE_GREEN;
            }
            else
            {
                objDisplayData.eLightColorType = LED_COLOR_OFF;
            }
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_VOICEMAIL
    case DT_MESSAGE_LIST:
        {
            objDisplayData.strIconPath = PIC_DSS_MTSW_MESSAGELIST;
            objDisplayData.eIconType = DK_ICON_METASWITCH_MESSAGE_LIST;
        }
        break;
#endif
#if IF_FEATURE_METASWITCH
    case DT_MTSW_LOGOUT:
        {
            objDisplayData.strIconPath = PIC_DSS_MTSW_LOGOUT;
            objDisplayData.eIconType = DK_ICON_METASWITCH_LOGOUT;
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    case DT_MTSWDIR:
        {
            objDisplayData.strIconPath = PIC_DSS_LOCALGROUP;
            objDisplayData.eIconType = DK_ICON_LOCAL_GROUP;
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    case DT_IDLE_CALLLIST:
        {
            objDisplayData.strIconPath = PIC_DSS_LOCALGROUP;
            objDisplayData.eIconType = DK_ICON_LOCAL_GROUP;
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_CALLPARK
    case DT_MTSW_CALLPARK:
        {
            objDisplayData.strIconPath = PIC_DSS_TALK;
            objDisplayData.eIconType = DK_ICON_TALK;
        }
        break;
#endif
    case DT_NA:
    default:
        {
            // 设置一个空路径，让按钮不加载图标
            objDisplayData.strIconPath = "";
            objDisplayData.strFirstRowText = "";
            objDisplayData.eIconType = DK_ICON_None;
            objDisplayData.eTextMode = TEXT_MODE_FIRST_ROW;
        }
        break;
    }

    GetDsskeyShortenText(objDisplayData, newLogicData);

    if (objDisplayData.strIconPath.empty())
    {
        objDisplayData.strIconPath = PIC_DSS_TALK;
        objDisplayData.eIconType = DK_ICON_None;
    }

// 状态无变化，则不改变灯状态
    if (!bLightChange)
    {
        objDisplayData.eLightColorType = eOldColorType;
    }

//EDK控制的DSSKey/////////////////////
    EdkCtrlDsskeyDispData objEDKDisplayData;
    if (EDK_GetEdkCtrlDsskeyDispData(sLogicData.iDssKeyID, objEDKDisplayData))
    {
        SetEDKCtrlDsskeyDisplay(objDisplayData, objEDKDisplayData);
    }
    ///////////////////////////////////////////

    DSSKEYUI_INFO("Dsskey[%d] Display label is [%s]", objDisplayData.iIndex,
                  objDisplayData.strFirstRowText.c_str());
    DSSKEYUI_INFO("Dsskey Icon [%s], PresenceIcon[%s]", objDisplayData.strIconPath.c_str(),
                  objDisplayData.strPresenceIcon.c_str());
    return  true;
}

// 目前仅支持linekey和Programkey的转换
int dsskeyUI_GetDsskeyIDByKeycode(int iKeyCode)
{
    int iIndex = -1;
    int iDsskeyID = -1;
    DSSKEY_MODULE_TYPE eDMT_Type = DMT_NON;
    switch (iKeyCode)
    {
    case PHONE_KEY_FN1:
        iIndex = 0;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    case PHONE_KEY_FN2:
        iIndex = 1;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    case PHONE_KEY_FN3:
        iIndex = 2;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    case PHONE_KEY_FN4:
        iIndex = 3;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    case PHONE_KEY_UP:
        iIndex = 4;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    case PHONE_KEY_DOWN:
        iIndex = 5;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    case PHONE_KEY_LEFT:
        iIndex = 6;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    case PHONE_KEY_RIGHT:
        iIndex = 7;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    case PHONE_KEY_OK:
        iIndex = 8;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    case PHONE_KEY_CANCEL:
        iIndex = 9;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    // conf for t3x
    case PHONE_KEY_CONFERENCE:
        iIndex = 10;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    case PHONE_KEY_HOLD:
        iIndex = 11;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    case PHONE_KEY_MUTE:
        iIndex = 12;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    case PHONE_KEY_TRANSFER:
        iIndex = 13;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    case PHONE_KEY_HISTORY:
        iIndex = 14;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    // T66 Menu做成可配置
    case PHONE_KEY_MENU:
        iIndex = 15;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    case PHONE_KEY_REDIAL:
        iIndex = 16;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    case PHONE_KEY_MESSAGE:
        iIndex = 17;
        eDMT_Type = DMT_PROGRAMEKEY_MODULE;
        break;
    case PHONE_KEY_LINE1:
    case PHONE_KEY_LINE2:
    case PHONE_KEY_LINE3:
    case PHONE_KEY_LINE4:
    case PHONE_KEY_LINE5:
    case PHONE_KEY_LINE6:
        {
            iIndex = iKeyCode - PHONE_KEY_LINE1;
            eDMT_Type = DMT_LINEKEY_MODULE;
        }
        break;
    default:
        break;
    }

    if (iIndex < 0)
    {
        // 找不到对应的映射值
        return -1;
    }

    iDsskeyID = dsskey_GetDsskeyID(eDMT_Type, 0, iIndex);
    return iDsskeyID;
}

#if IF_FEATURE_T42_LINEKEY_SECONDLINE
void dsskeyUI_ResetLinekeyTimer()
{
    _DsskeyUI->ResetLinekeyTimer();
}
#endif

/************************************************************************/
/* 接口  ： dsskeyUI_RefreshDsskey                                       */
/* 功能  ： 刷新某一个Dsskey数据                                         */
/* 参数说明 ：objLogicData 类型：DssKeyData&                             */
/* 返回值 ： 无                                                             */
/************************************************************************/
void dsskeyUI_RefreshDsskey(DssKeyLogicData& objLogicData, DsskeyExpRefresh dsskeyRefresh/* = DSS_KEY_REFRESH_ALL*/)
{
    DSSKEYUI_INFO("dsskeyUI_RefreshDsskey :: iDssKeyID is %d dsskeyRefresh: %d.", objLogicData.iDssKeyID, dsskeyRefresh);
    if (DMT_LINEKEY_MODULE == objLogicData.eType)
    {
        _DssKeyUILogic.UpdateLineKeyStatus(objLogicData);
    }
    else
    {
        DsskeyDisplayData DisplayData;
        bool ret = dsskeyUI_ParseLogicData(DisplayData, objLogicData);

        if (!ret == true)
        {
            // 解析逻辑数据不成功
            DSSKEYUI_WARN("dsskeyUI_RefreshDsskey : dsskeyUI_ParseLogicData Failed.");
            return;
        }

        if (IsEXPModule(objLogicData.eType))
        {
            int moduleIndex = dsskey_GetModuleIndex(DisplayData.iDsskeyID);
            bool bNew = false;
            if (!exp_GetExpColor())
            {
                // 将Dsskey类型Icon映射为EXP的Icon类型
                char swver[128] = { 0 };
                int iEXPIndex = GetEXPIndex(objLogicData.iDssKeyID);
                int lcdopen = open("/dev/exp0", O_RDWR);
                exp_helper_get_swversion(lcdopen, iEXPIndex, 127, swver);
                if (lcdopen != -1)
                {
                    int v1, v2, v3, v4;
                    //DSSKEYUI_INFO("swver is %s, iEXPIndex is %d.", swver, iEXPIndex);
                    sscanf(swver, "%d.%d.%d.%d", &v1, &v2, &v3, &v4);
                    // 版本号大于6表示更新了BLF新图标的版本
                    if (v1 > 6)
                    {
                        bNew = true;
                    }
                    //DSSKEYUI_INFO("v1 is %d. bNew is %d.", v1, bNew);
                    close(lcdopen);
                }
            }
            // 获取对应图标
            int exp_Type = MapIconType(DisplayData.eIconType, bNew);
            // 获取显示字符串
            yl::string strLabel = DisplayData.strFirstRowText;
            if (objLogicData.eDKType == DT_NA)
            {
                strLabel = "";
            }

            // 希伯来语需要反转
            yl::wstring wstrTmp = Lang_Format(strLabel);
            strLabel = yl::to_utf8(wstrTmp);

            int iFlage = 1;
            if (objLogicData.eDKType == DT_EXP_SWITCH)
            {
                iFlage = EXP_MSGFL_CENTER | EXP_MSGFL_REVERSE;// switch反选居中
            }

            if (exp_GetExpColor())
            {
                if (DT_LINE == DisplayData.eDssKeytype
                        && (DisplayData.iStatus <= DS_LINE_NONE
                            || DisplayData.iStatus >= DS_LINE_END
                            || DS_LINE_DISABLE == DisplayData.iStatus))
                {
                    strLabel = "";
                }
                else
                {
                    strLabel = _LANG(DisplayData.strFirstRowText.c_str());
                }
            }

            // 获取灯状态
            LIGHT_Type l_Type = GetLightType(DisplayData.eLightColorType, DisplayData.eLightFlushType);
#if IF_BUG_XMLBROWSER_EXP_LED
            if (objLogicData.eLightType != LT_NONE)
            {
                l_Type = objLogicData.eLightType;
            }
#endif

            //EDK控制的EXPKey/////////////////////

            //TODO EDK
            /*EdkCtrlDsskeyDispData objEDKDisplayData;
            if (EDK_GetEdkCtrlDsskeyDispData(DisplayData.iDsskeyID, objEDKDisplayData))
            {
                SetEDKCtrlEXPkeyDisplay(objEDKDisplayData, strLabel, l_Type);
            }*/
            ///////////////////////////////////////////

            // TODO?
            if (DSS_KEY_REFRESH_LED != dsskeyRefresh)
                //if (DSS_KEY_REFRESH_ALL == dsskeyRefresh)
            {
                if (DT_URL != DisplayData.eDssKeytype)
                {
                    exp_SetExpKeyDisp(DisplayData.iDsskeyID
                                      , exp_Type
                                      , strLabel.c_str()
                                      , l_Type
                                      , iFlage
                                      , DisplayData.eGroundPhoto
                                      , DisplayData.eIconType == DK_ICON_CUSTOM ? DisplayData.strIconPath.c_str() : ""
                                      , DisplayData.ePresenceType
                                      , DisplayData.iStatus);
                }
                else
                {

                    if (LED_COLOR_RED_GREEN == DisplayData.eLightColorType)
                    {
                        if (exp_GetExpColor())
                        {
                            exp_SetExpKeyDisp(DisplayData.iDsskeyID
                                              , exp_Type
                                              , strLabel.c_str()
                                              , l_Type
                                              , iFlage
                                              , DisplayData.eGroundPhoto
                                              , DisplayData.eIconType == DK_ICON_CUSTOM ? DisplayData.strIconPath.c_str() : ""
                                              , DisplayData.ePresenceType
                                              , DisplayData.iStatus);
                        }
                        g_pDssKeyLightLogic->UpdateLight(DisplayData.iDsskeyID, DisplayData.eLightColorType, \
                                                         DisplayData.eLightFlushType, DisplayData.iFlushuration);
                    }
                    else
                    {
                        exp_SetExpKeyDisp(DisplayData.iDsskeyID
                                          , exp_Type
                                          , strLabel.c_str()
                                          , l_Type
                                          , iFlage
                                          , DisplayData.eGroundPhoto
                                          , DisplayData.eIconType == DK_ICON_CUSTOM ? DisplayData.strIconPath.c_str() : ""
                                          , DisplayData.ePresenceType
                                          , DisplayData.iStatus);

                        if (LED_COLOR_OFF == DisplayData.eLightColorType)
                        {
                            // 解决url扩展台灭灯由于未停止定时器，灭不掉的问题
                            g_pDssKeyLightLogic->UpdateLight(DisplayData.iDsskeyID, DisplayData.eLightColorType, \
                                                             DisplayData.eLightFlushType, DisplayData.iFlushuration);
                        }
                    }
                }
            }
            else if (DSS_KEY_REFRESH_LED == dsskeyRefresh)
            {
#if !IF_BUG_34565
                if (DT_URL != DisplayData.eDssKeytype)
#endif
                {
                    // 只刷新灯时，URL不刷新，URL只在点击时刷新全部数据时刷新灯
                    exp_SetExpLEDStatus(DisplayData.iDsskeyID, l_Type);
                }
                else
                {
                    if (LED_COLOR_OFF == DisplayData.eLightColorType)
                    {
                        g_pDssKeyLightLogic->UpdateLight(DisplayData.iDsskeyID, DisplayData.eLightColorType,
                                                         DisplayData.eLightFlushType, DisplayData.iFlushuration);
                    }
                }
            }
            //          else if (DSS_KEY_REFRESH_ICON == dsskeyRefresh)
            //          {
            //              dsskey_SetExpIcon(DisplayData.iDsskeyID, exp_Type);
            //          }
        }
    }
}

void dsskeyUI_RefreshButton(int iIndex, const DsskeyDisplayData& objDisplayData)
{
    g_pLinekeyUI->RefreshDsskeyData(iIndex, objDisplayData);
}
