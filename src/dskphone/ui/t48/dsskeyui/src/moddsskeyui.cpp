/************************************************************************
 * FileName  : modDsskeyUI.cpp (implementation file)
 * Purpose   :
 * Date      : 2012/09/13 15:09:51
 ************************************************************************/

#include "moddsskeyui.h"
#include <ETLLib.hpp>
#include "baseui/t4xpicpath.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "account/include/accountmanager.h"
#include "keyguard/include/modkeyguard.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/broadsoftsca/include/modbroadsoft_sca.h"
#include "cdlgfulldsskeyui.h"
#include "uimanager/basedialog.h"
#include "dsskeymanager.h"
#include "settingui/include/modsettingui.h"
#include "talk/talklogic/src/talklogic_inc.h"

#include "uimanager/moduimanager.h"
// 暂时屏蔽broadsoftUC的头文件包含 - liuqy
// #include "Presence/UCContactData.h"
// #include "Presence/CPresenceContactManager.h"
#include "dsskey/include/moddsskey.h"
#include "exp/include/modexp.h"
#include "cdsskeylightlogic.h"
#include "mainwnd/mainwnd.h"
//#include "dsskeymainwndwgtdelegate.h"
#include "uicommon/messagebox/messageboxbase.h"
#include "uicommon/messagebox/modmessagebox.h"
#include "uikernel/languagehelper.h"
#include "idlescreenui/dlgidlescreen.h"
// 暂时屏蔽DirectoryUI的头文件包含 - liuqy
//#include "DirectoryUI/DirectoryMgr.h"
#include <yllist.h>

#include "dsskey/src/dsskeycommonfunc.h"
#include "dsskey/src/dsskey_common.h"
#include "dsskeyuilogic.h"
#include "commonapi/stringhelperapi.h"
#include "qtcommon/qmisc.h"

#include "contacts/directory/include/commondata.h"
#include "directoryui/include/moddirectoryui.h"
#include "dsskey/include/idsskeyimplaction.h"
#include "setting/include/modmenu.h"
#include "setting/include/messagedefine.h"
#include <devicelib/phonedevice.h>

#include "lamp/ledlamp/include/modledlamp.h"
#include "lamp/ledlamp/include/led_data_define.h"

// testmode
#include "testmode/include/modtestmode.h"
#include "testmode/src/testphonet4x.h"


#include "dsskeyperformancemanager.h"

#include "moddsskeyperformance.h"
#include "feature/include/modfeature.h"

#include "keyguard/include/modkeyguard.h"
#include "language/input/include/modInput.h"

#include "edk/include/modedk.h"

//#define DSSKEYUI_TEST
#ifdef DSSKEYUI_TEST
typedef YLList<DssKeyLogicData> Dsskey_Data_UI_List;

void dsskeyUI_ParseTest(int x)
{
    Dsskey_Data_UI_List listDsskeyData;

    // 自测模拟数据
    listDsskeyData.clear();
    DssKeyLogicData DKlogicData;
    int j = 0;
    for (int i = 0; i < 16; ++i, ++j)
    {
        switch (i)
        {
        case 0:
            {
                //DKlogicData.eLockType = DSS_KEY_LOCK_TYPE_LOCK;
                DKlogicData.eDKType = DT_LINE;
                DKlogicData.iStatus = DS_LINE_REGISTERING;
                DKlogicData.strLabel = "30";
            }
            break;
        case 1:
            {
                DKlogicData.eDKType = DT_LINE;
                DKlogicData.iStatus = DS_LINE_REGISTERED;
                DKlogicData.strLabel = "31";

            }
            break;
        case 2:
            {
                DKlogicData.eDKType = DT_LINE;
                DKlogicData.iStatus = DS_LINE_USING;
                DKlogicData.strLabel = "32";
            }
            break;
        case 3:
            {
                DKlogicData.eDKType = DT_LINE;
                DKlogicData.iStatus = DS_LINE_FWD;
                DKlogicData.strLabel = "33";
            }
            break;
        case 4:
            {
                DKlogicData.eDKType = DT_LINE;
                DKlogicData.iStatus = DS_LINE_DND;
                DKlogicData.strLabel = "34";
            }
            break;
        case 5:
            {
                DKlogicData.eDKType = DT_LINE;
                //DKlogicData.eLockType = DSS_KEY_LOCK_TYPE_FLOAT;
                DKlogicData.listAuxiliaryState.clear();
                DKlogicData.strLabel = "35";
                DKlogicData.listAuxiliaryState.push_back(DS_SESSION_CALL_IN);
            }
            break;
        case 6:
            {
                switch (x)
                {
                case 0:
                    {
                        DKlogicData.eDKType = DT_BLF;
                        DKlogicData.strLabel = "BLF";
                        DKlogicData.iStatus = DS_BLF_STATUS_IDLE;
                    }
                    break;
                case 1:
                    {
                        DKlogicData.eDKType = DT_BLF;
                        DKlogicData.strLabel = "BLF";
                        DKlogicData.iStatus = DS_BLF_STATUS_CALLOUT;
                    }
                    break;
                case 2:
                    {
                        DKlogicData.eDKType = DT_BLF;
                        DKlogicData.strLabel = "BLF";
                        DKlogicData.iStatus = DS_BLF_STATUS_RINGING;
                    }
                    break;
                case 3:
                    {
                        DKlogicData.eDKType = DT_BLF;
                        DKlogicData.strLabel = "BLF";
                        DKlogicData.iStatus = DS_BLF_STATUS_TALKING;
                    }
                    break;
                case 4:
                    {
                        DKlogicData.eDKType = DT_BLF;
                        DKlogicData.strLabel = "BLF";
                        DKlogicData.iStatus = DS_BLF_STATUS_HOLD;
                    }
                    break;
                case 5:
                    {
                        DKlogicData.eDKType = DT_BLF;
                        DKlogicData.strLabel = "BLF";
                        DKlogicData.iStatus = DS_BLF_STATUS_PARK;
                    }
                    break;
                case 6:
                    {
                        DKlogicData.eDKType = DT_BLF;
                        DKlogicData.strLabel = "BLF";
                        DKlogicData.iStatus = DS_BLF_STATUS_UNKOWN;
                    }
                    break;
                case 7:
                    {
                        DKlogicData.eDKType = DT_BLF;
                        DKlogicData.strLabel = "BLF";
                        DKlogicData.iStatus = DS_NON;
                    }
                    break;
                default:
                    {
                        DKlogicData.eDKType = DT_BLF;
                        DKlogicData.strLabel = "BLF";
                        DKlogicData.iStatus = DS_BLF_STATUS_IDLE;
                    }
                    break;
                }
            }
            break;
        case 7:
            {
                switch (x)
                {
                case 0:
                    {
                        DKlogicData.eDKType = DT_BLFLIST;
                        DKlogicData.strLabel = "BLFList";
                        DKlogicData.iStatus = DS_BLF_STATUS_IDLE;
                    }
                    break;
                case 1:
                    {
                        DKlogicData.eDKType = DT_BLFLIST;
                        DKlogicData.strLabel = "BLF";
                        DKlogicData.iStatus = DS_BLF_STATUS_CALLOUT;
                    }
                    break;
                case 2:
                    {
                        DKlogicData.eDKType = DT_BLFLIST;
                        DKlogicData.strLabel = "BLFList";
                        DKlogicData.iStatus = DS_BLF_STATUS_RINGING;
                    }
                    break;
                case 3:
                    {
                        DKlogicData.eDKType = DT_BLFLIST;
                        DKlogicData.strLabel = "BLFList";
                        DKlogicData.iStatus = DS_BLF_STATUS_TALKING;
                    }
                    break;
                case 4:
                    {
                        DKlogicData.eDKType = DT_BLFLIST;
                        DKlogicData.strLabel = "BLFList";
                        DKlogicData.iStatus = DS_BLF_STATUS_HOLD;
                    }
                    break;
                case 5:
                    {
                        DKlogicData.eDKType = DT_BLFLIST;
                        DKlogicData.strLabel = "BLFList";
                        DKlogicData.iStatus = DS_BLF_STATUS_PARK;
                    }
                    break;
                case 6:
                    {
                        DKlogicData.eDKType = DT_BLFLIST;
                        DKlogicData.strLabel = "BLFList";
                        DKlogicData.iStatus = DS_BLF_STATUS_UNKOWN;
                    }
                    break;
                case 7:
                    {
                        DKlogicData.eDKType = DT_BLFLIST;
                        DKlogicData.strLabel = "BLFList";
                        DKlogicData.iStatus = DS_NON;
                    }
                    break;
                default:
                    {
                        DKlogicData.eDKType = DT_BLFLIST;
                        DKlogicData.strLabel = "BLFList";
                        DKlogicData.iStatus = DS_BLF_STATUS_IDLE;
                    }
                    break;
                }
            }
            break;
        case 8:
            {
                switch (x)
                {
                case 0:
                    {
                        DKlogicData.eDKType = DT_INTERCOM;
                        DKlogicData.strValue = "3601";
                        DKlogicData.iStatus = DS_BLF_STATUS_IDLE;
                    }
                    break;
                case 1:
                    {
                        DKlogicData.eDKType = DT_INTERCOM;
                        DKlogicData.strValue = "3602";
                        DKlogicData.iStatus = DS_BLF_STATUS_RINGING;
                    }
                    break;
                case 2:
                    {
                        DKlogicData.eDKType = DT_INTERCOM;
                        DKlogicData.strValue = "3603";
                        DKlogicData.iStatus = DS_BLF_STATUS_TALKING;
                    }
                    break;
                case 3:
                    {
                        DKlogicData.eDKType = DT_INTERCOM;
                        DKlogicData.strValue = "3604";
                        DKlogicData.iStatus = DS_BLF_STATUS_HOLD;
                    }
                    break;
                case 4:
                    {
                        DKlogicData.eDKType = DT_INTERCOM;
                        DKlogicData.strValue = "3605";
                        DKlogicData.iStatus = DS_BLF_STATUS_UNKOWN;
                    }
                    break;
                case 5:
                    {
                        DKlogicData.eDKType = DT_INTERCOM;
                        DKlogicData.strValue = "3606";
                        DKlogicData.iStatus = DS_NON;
                    }
                    break;
                default:
                    {
                        DKlogicData.eDKType = DT_INTERCOM;
                        DKlogicData.strValue = "3607";
                        DKlogicData.iStatus = DS_BLF_STATUS_IDLE;
                    }
                    break;
                }
            }
            break;
        case 9:
            {
                switch (x)
                {
                case 0:
                    {
                        DKlogicData.eDKType = DT_CALLPARK;
                        DKlogicData.strLabel = "Call Park";
                        DKlogicData.iStatus = DS_BLF_STATUS_IDLE;
                    }
                    break;
                case 1:
                    {
                        DKlogicData.eDKType = DT_CALLPARK;
                        DKlogicData.strLabel = "Call Park";
                        DKlogicData.iStatus = DS_BLF_STATUS_RINGING;
                    }
                    break;
                case 2:
                    {
                        DKlogicData.eDKType = DT_CALLPARK;
                        DKlogicData.strLabel = "Call Park";
                        DKlogicData.iStatus = DS_BLF_STATUS_TALKING;
                    }
                    break;
                case 3:
                    {
                        DKlogicData.eDKType = DT_CALLPARK;
                        DKlogicData.strLabel = "Call Park";
                        DKlogicData.iStatus = DS_BLF_STATUS_UNKOWN;
                    }
                    break;
                case 4:
                    {
                        DKlogicData.eDKType = DT_CALLPARK;
                        DKlogicData.strLabel = "Call Park";
                        DKlogicData.iStatus = DS_NON;
                    }
                    break;
                default:
                    {
                        DKlogicData.eDKType = DT_CALLPARK;
                        DKlogicData.strLabel = "Call Park";
                        DKlogicData.iStatus = DS_BLF_STATUS_IDLE;
                    }
                    break;
                }
            }
            break;
        case 10:
            {
                switch (x)
                {
                case 0:
                    {
                        DKlogicData.eDKType = DT_ACD;
                        DKlogicData.strLabel = "ACD";
                        DKlogicData.iStatus = DS_ACD_IDLE;
                    }
                    break;
                case 1:
                    {
                        DKlogicData.eDKType = DT_ACD;
                        DKlogicData.strLabel = "ACD";
                        DKlogicData.iStatus = DS_ACD_AVAILABLE;
                    }
                    break;
                case 2:
                    {
                        DKlogicData.eDKType = DT_ACD;
                        DKlogicData.strLabel = "ACD";
                        DKlogicData.iStatus = DS_ACD_UNAVAILABLE;
                    }
                    break;
                case 3:
                    {
                        DKlogicData.eDKType = DT_ACD;
                        DKlogicData.strLabel = "ACD";
                        DKlogicData.iStatus = DS_ACD_WRAPUP;
                    }
                    break;
                default:
                    {
                        DKlogicData.eDKType = DT_ACD;
                        DKlogicData.strLabel = "ACD";
                        DKlogicData.iStatus = DS_ACD_IDLE;
                    }
                    break;
                }
            }
            break;
        case 11:
            {
                switch (x)
                {
                case 0:
                    {
                        DKlogicData.eDKType = DT_HOTELING;
                        DKlogicData.strLabel = "Hoteling";
                        DKlogicData.iStatus = DS_HOTELING_OFF;
                    }
                    break;
                case 1:
                default:
                    {
                        DKlogicData.eDKType = DT_HOTELING;
                        DKlogicData.strLabel = "Hoteling";
                        DKlogicData.iStatus = DS_HOTELING_ON;
                    }
                    break;
                }

            }
            break;
        case 12:
            {
                DKlogicData.eDKType = DT_SMS;
                DKlogicData.strLabel = "SMS";
            }
            break;
        case 13:
            {
                DKlogicData.eDKType = DT_PUBLICHOLD;
                DKlogicData.strLabel = "Public Hold";
            }
            break;
        case 14:
            {
                switch (x)
                {
                case 0:
                    {
                        DKlogicData.eDKType = DT_URL;
                        DKlogicData.iStatus = DS_URL_NETWORK_UNAVAILABLE;
                        DKlogicData.strLabel = "URL_UNOK";
                    }
                    break;
                case 1:
                    {
                        DKlogicData.eDKType = DT_URL;
                        DKlogicData.iStatus = DS_URL_OK;
                        DKlogicData.strLabel = "URL_OK";
                    }
                    break;
                case 2:
                    {
                        DKlogicData.eDKType = DT_URL;
                        DKlogicData.iStatus = DS_URL_NETWORK_UNKNOW;
                        DKlogicData.strLabel = "URL_UNKNOWN";
                    }
                    break;
                case 3:
                default:
                    {
                        DKlogicData.eDKType = DT_URL;
                        DKlogicData.iStatus = DS_NON;
                        DKlogicData.strLabel = "URL_NONE";
                    }
                    break;

                }

            }
            break;
        case 15:
            {
                DKlogicData.eDKType = DT_DND;
                DKlogicData.strLabel = "DND";
            }
            break;
        case 16:
            {
                DKlogicData.eDKType = DT_HOLD;
                DKlogicData.strLabel = "HOLD";
            }
            break;

        default:
            break;
        }
        DKlogicData.iDssKeyID = j;
        listDsskeyData.push_back(DKlogicData);
    }

    Dsskey_Data_UI_List::iterator iter = listDsskeyData.begin();
    Dsskey_Data_UI_List::iterator iterEnd = listDsskeyData.end();

    //YLVector<DssKeyLogicData*> vecKeep((unsigned int)LINEKEY_PER_PAGE_COUNT);
    for (int iIndex = 0; iter != iterEnd; ++iIndex, iter++)
    {
        DssKeyLogicData & objData = *iter;

        dsskeyUI_RefreshDsskey(objData);
    }
}
#endif

bool dsskeyUI_RefreshCallback(int dsskeyID, Dsskey_Status dsskeyStatus,
                              const DssKeyLogicData * pLogicData,  void * pExtraData)
{
    DsskeyExpRefresh eDssRefresh = DSS_KEY_REFRESH_ALL;
    if (pExtraData != NULL)
    {
        eDssRefresh = (DsskeyExpRefresh) * (int *)pExtraData;
    }
    DssKeyLogicData DKLogicData;
    dsskey_GetData(dsskeyID, DKLogicData);
    // 刷新实际数据
    dsskeyUI_RefreshDsskey(DKLogicData, eDssRefresh);

    return true;
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

LRESULT dsskeyUI_ProcessMsg(msgObject & objMessage)
{
    DSSKEYUI_INFO("dsskeyUI_ProcessMsg.Message is %d.", objMessage.message);

    switch (objMessage.message)
    {
    case DEV_MSG_FUNKEY_EVENT:
        {
            DSSKEYUI_INFO("dsskeyUI_ProcessMsg.-----DEV_MSG_FUNKEY_EVENT>>>>");
            if (exp_IsEXPKey(objMessage.lParam))
            {
                if (testmode_IsTestMode())
                {
                    g_pTestPhone->ProcessKeyEvent(objMessage.lParam, true);
                }
                else
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
        }
        break;
    case DEV_MSG_ELP_KEY:
        {
            if (exp_IsEXPKey(objMessage.lParam))
            {
                DSSKEY_MODULE_TYPE eModule = GetModuleType(objMessage.lParam);
                bool bExpSupport = IsSupportModuleType(eModule);
                if (!bExpSupport || testmode_IsTestMode())
                {
                    // T29不支持DMT_EXP40_MODULE
                    return 0;
                }

                if (dsskey_GetDsskeyType(objMessage.lParam) == DT_LINE
                        && bsSCA_ShowSCAStatus(dsskey_GetDsskeyLine(objMessage.lParam)))
                {
                    return 1;
                }

#if IF_FEATURE_SHOW_BLF_CALLID
                if ((DT_BLF == dsskey_GetDsskeyType(objMessage.lParam)
                        || DT_BLFLIST == dsskey_GetDsskeyType(objMessage.lParam))
                        && configParser_GetConfigInt(kszBLFShowCallId) != 0
                        && blf_ShowBLFMenu(objMessage.lParam))
                {
                    return 1;
                }
#endif

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
    case PRESENCE_MESSAGE_USER_INFO_CHANGE:
    case PRESENCE_MESSAGE_USER_STATUS_CHANGE:
        {
            DSSKEYUI_INFO("Receive PRESENCE_CHANGE message");
            dsskeyUI_PresenceUserChange();
        }
        break;
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
        }
        break;
    case CONFIG_MSG_BCAST_CHANGED:
        {
            switch (objMessage.wParam)
            {
            case ST_DSSKEY_LENGHT:
            case ST_LANG:
                {
                    g_pDsskeyUIManager->RefreshAllDisplay();
                }
                break;
            default:
                break;
            }
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
    for (int i = 0; i < 29; ++i)
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

/************************************************************************/
/* 接口  ： dsskeyUI_RegisterDlgClass()                                 */
/* 功能  ： 实现窗体类注册                                              */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void dsskeyUI_RegisterDlgClass()
{
    REGISTER_DLG(CDlgFullDsskeyUI);
}

//初始化DssKeyUI模块
void dsskeyUI_Init()
{
    // 初始化注册刷新回调函数到Dsskey逻辑层中
    dsskey_setUpdateFunCallback(dsskeyUI_RefreshCallback);
    //CUCFavoriteAciton *pDsskeyAction = new CUCFavoriteAciton(DT_UC_FAVORITE);
    dsskeyUI_RegisterDlgClass();

    _DssKeyUILogic.SetMaxPage(1);
    _DssKeyUILogic.SetMaxNumPerPage(29);
    _DssKeyUILogic.SetMaxNumber(29);

    DP_Init();
    // 刷新所有Dsskey 数据
    _DssKeyUILogic.RefreshMultipleDsskeyData(0, 29, DMT_LINEKEY_MODULE);
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
    etl_RegisterMsgHandle(PRESENCE_MESSAGE_USER_STATUS_CHANGE, \
                          PRESENCE_MESSAGE_USER_STATUS_CHANGE, dsskeyUI_ProcessMsg);
    etl_RegisterMsgHandle(PRESENCE_MESSAGE_USER_INFO_CHANGE, \
                          PRESENCE_MESSAGE_USER_INFO_CHANGE, dsskeyUI_ProcessMsg);
    etl_RegisterMsgHandle(LANGUAGE_MESSAGE_LANGUAGE_CHANGED, \
                          LANGUAGE_MESSAGE_LANGUAGE_CHANGED, dsskeyUI_ProcessMsg);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, \
                          CONFIG_MSG_BCAST_CHANGED, dsskeyUI_ProcessMsg);

    etl_RegisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, \
                          ACCOUNT_DEFAULT_CHANGED, dsskeyUI_ProcessMsg);

}

//卸载DssKeyUI模块
void dsskeyUI_Uinit()
{
    DP_Uninit();
    // 注销消息
    etl_UnregisterMsgHandle(DEV_MSG_FUNKEY_EVENT,
                            DEV_MSG_FUNKEY_EVENT, dsskeyUI_ProcessMsg);
    etl_UnregisterMsgHandle(DEV_MSG_ELP_KEY,
                            DEV_MSG_ELP_KEY, dsskeyUI_ProcessMsg);
    etl_UnregisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE, \
                            ACD_MESSAGE_STATUS_CHANGE, dsskeyUI_ProcessMsg);
    etl_UnregisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, \
                            HOTELING_MESSAGE_STATUS_CHANGE, dsskeyUI_ProcessMsg);
    etl_UnregisterMsgHandle(REMOTE_PHONEBOOK_RELOAD, \
                            REMOTE_PHONEBOOK_RELOAD, dsskeyUI_ProcessMsg);
    etl_UnregisterMsgHandle(LOCAL_DIRECTORY_RELOAD, \
                            LOCAL_DIRECTORY_RELOAD, dsskeyUI_ProcessMsg);
    etl_UnregisterMsgHandle(BROADSOFT_PHONEBOOK_RELOAD, \
                            BROADSOFT_PHONEBOOK_RELOAD, dsskeyUI_ProcessMsg);
    etl_UnregisterMsgHandle(LOCAL_GROUP_UPDATE, \
                            LOCAL_GROUP_UPDATE, dsskeyUI_ProcessMsg);
    etl_UnregisterMsgHandle(PRESENCE_MESSAGE_USER_STATUS_CHANGE,
                            PRESENCE_MESSAGE_USER_STATUS_CHANGE, dsskeyUI_ProcessMsg);
    etl_UnregisterMsgHandle(PRESENCE_MESSAGE_USER_INFO_CHANGE,
                            PRESENCE_MESSAGE_USER_INFO_CHANGE, dsskeyUI_ProcessMsg);
    etl_UnregisterMsgHandle(LANGUAGE_MESSAGE_LANGUAGE_CHANGED,
                            LANGUAGE_MESSAGE_LANGUAGE_CHANGED, dsskeyUI_ProcessMsg);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, \
                            CONFIG_MSG_BCAST_CHANGED, dsskeyUI_ProcessMsg);

    etl_UnregisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, \
                            ACCOUNT_DEFAULT_CHANGED, dsskeyUI_ProcessMsg);
}


//打开DssKey界面
void dsskeyUI_OpenFullDsskeyDlg()
{
    CBaseDialog * pDsskeyDlg = UIManager_GetPrivateDlg(DLG_FullDssKey);

    if (NULL == pDsskeyDlg)
    {
        pDsskeyDlg = UIManager_CreatePrivateDlg(DLG_FullDssKey, DLG_FullDssKey);
    }

    if (NULL == pDsskeyDlg)
    {
        return;
    }

    // 将窗口设置为顶层窗口
    UIManager_SetTopWindow(pDsskeyDlg);
    // 刷新窗口
    UIManager_UpdateWnd(pDsskeyDlg);
}


EXP_ICON_TYPE MapIconType(Dsskey_Icon_Type eIconType, bool bNew)
{
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
LIGHT_Type GetLightType(const DSS_KEY_COLOR_TYPE eColorType, const DSS_KEY_FLASH_TYPE eFlashType)
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
            switch (eFlashType)
            {
            case REFRESH_TYPE_NO_FLASH:
                eLEDType = LT_RED_ON;
                break;
            case REFRESH_TYPE_SLOW_FLASH:
                eLEDType = LT_RED_FLASH_4;
                break;
            case REFRESH_TYPE_FLASH:
                eLEDType = LT_RED_FLASH_3;
                break;
            case REFRESH_TYPE_FAST_FLASH:
                eLEDType = LT_RED_FLASH_2;
                break;
            case REFRESH_TYPE_FASTER_FLASH:
                eLEDType = LT_RED_FLASH_1;
                break;
            default:
                break;
            }
        }
        break;
    case LED_COLOR_TYPE_GREEN:
        {
            switch (eFlashType)
            {
            case REFRESH_TYPE_NO_FLASH:
                eLEDType = LT_GREEN_ON;
                break;
            case REFRESH_TYPE_SLOW_FLASH:
                eLEDType = LT_GREEN_FLASH_4;
                break;
            case REFRESH_TYPE_FLASH:
                eLEDType = LT_GREEN_FLASH_3;
                break;
            case REFRESH_TYPE_FAST_FLASH:
                eLEDType = LT_GREEN_FLASH_2;
                break;
            case REFRESH_TYPE_FASTER_FLASH:
                eLEDType = LT_GREEN_FLASH_1;
                break;
            default:
                break;
            }
        }
        break;
    case LED_COLOR_TYPE_ORANGE:
        {
            switch (eFlashType)
            {
            case REFRESH_TYPE_NO_FLASH:
                eLEDType = LT_RED_GREEN_ON;
                break;
            case REFRESH_TYPE_SLOW_FLASH:
                eLEDType = LT_RED_GREEN_FLASH_4;
                break;
            case REFRESH_TYPE_FLASH:
                eLEDType = LT_RED_GREEN_FLASH_3;
                break;
            case REFRESH_TYPE_FAST_FLASH:
                eLEDType = LT_RED_GREEN_FLASH_2;
                break;
            case REFRESH_TYPE_FASTER_FLASH:
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

/************************************************************************/
/* 接口  ： dsskeyUI_RefreshDsskey                                       */
/* 功能  ： 刷新某一个Dsskey数据                                         */
/* 参数说明 ：objLogicData 类型：DssKeyData&                             */
/* 返回值 ： 无                                                             */
/************************************************************************/
void dsskeyUI_RefreshDsskey(const DssKeyLogicData & objLogicData,
                            DsskeyExpRefresh dsskeyRefresh/* = DSS_KEY_REFRESH_ALL*/)
{
    if (DMT_PROGRAMEKEY_MODULE == objLogicData.eType)
    {
        // Programkey需要刷新数据的只是softkey内容，只需刷新Idle界面的softkey即可
        g_pIdleScreenUI->OnProgramableKeyConfigChange();
        return;
    }

    DsskeyDisplayData DisplayData;
    //  bool ret = dsskeyUI_ParseLogicData(DisplayData, objLogicData);
    bool ret = DP_ParseDsskeyPerformance(DisplayData, objLogicData);

    if (!ret == true)
    {
        // 解析逻辑数据不成功
        DSSKEYUI_WARN("dsskeyUI_RefreshDsskey : dsskeyUI_ParseLogicData Failed.");
        return;
    }

    if (DMT_LINEKEY_MODULE == objLogicData.eType)
    {
        g_pDsskeyUIManager->OnNotify(DisplayData);
#if IF_BUG_XMLBROWSER_EXP_LED
        if (objLogicData.eLightType != LT_NONE)
        {
            dsskeyUI_DsskeyLightCallback(DisplayData.iDsskeyID, dsskeyUI_GetColorType(objLogicData.eLightType),
                                         dsskeyUI_GetFlushType(objLogicData.eLightType));
        }
#endif
    }
    else
    {
        if (IsEXPModule(objLogicData.eType))
        {
            //int moduleIndex = dsskey_GetModuleIndex(DisplayData.iDsskeyID);
            bool bNew = false;
            // 将Dsskey类型Icon映射为EXP的Icon类型
            char swver[128] = {0};
            int iEXPIndex = GetEXPIndex(objLogicData.iDssKeyID);
            int lcdopen = open("/dev/exp0", O_RDWR);
            exp_helper_get_swversion(lcdopen, iEXPIndex, 127, swver);
            if (lcdopen != -1)
            {
                int v1, v2, v3, v4;
                DSSKEYUI_INFO("swver is %s, iEXPIndex is %d.", swver, iEXPIndex);
                sscanf(swver, "%d.%d.%d.%d", &v1, &v2, &v3, &v4);
                // 版本号大于6表示更新了BLF新图标的版本
                if (v1 > 6)
                {
                    bNew = true;
                }
                DSSKEYUI_INFO("v1 is %d. bNew is %d.", v1, bNew);
                close(lcdopen);
            }
            // 获取对应图标
            EXP_ICON_TYPE exp_Type = MapIconType(DisplayData.eIconType, bNew);
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

            // 获取灯状态
            LIGHT_Type l_Type = GetLightType(DisplayData.eLightColorType, DisplayData.eLightFlashType);
#if IF_BUG_XMLBROWSER_EXP_LED
            if (objLogicData.eLightType != LT_NONE)
            {
                l_Type = objLogicData.eLightType;
            }
#endif

            //EDK控制的EXPKey/////////////////////
            EdkCtrlDsskeyDispData objEDKDisplayData;
            if (EDK_GetEdkCtrlDsskeyDispData(DisplayData.iDsskeyID, objEDKDisplayData))
            {
                SetEDKCtrlEXPkeyDisplay(objEDKDisplayData, strLabel, l_Type);
            }
            ///////////////////////////////////////////

            if (DSS_KEY_REFRESH_ALL == dsskeyRefresh)
            {
                if (DT_URL != DisplayData.eDssKeytype)
                {
                    exp_SetExpKeyDisp(DisplayData.iDsskeyID, exp_Type, strLabel.c_str(), l_Type, iFlage);
                }
                else
                {

                    if (LED_COLOR_RED_GREEN == DisplayData.eLightColorType)
                    {
                        g_pDssKeyLightLogic->UpdateLight(DisplayData.iDsskeyID, DisplayData.eLightColorType, \
                                                         DisplayData.eLightFlashType, DisplayData.iFlashuration);
                    }
                    else
                    {
                        exp_SetExpKeyDisp(DisplayData.iDsskeyID, exp_Type, strLabel.c_str(), l_Type, iFlage);
                        if (LED_COLOR_OFF == DisplayData.eLightColorType)
                        {
                            // 解决url扩展台灭灯由于未停止定时器，灭不掉的问题
                            g_pDssKeyLightLogic->UpdateLight(DisplayData.iDsskeyID, DisplayData.eLightColorType, \
                                                             DisplayData.eLightFlashType, DisplayData.iFlashuration);
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
                                                         DisplayData.eLightFlashType, DisplayData.iFlashuration);
                    }
                }
            }
            else if (DSS_KEY_REFRESH_ICON == dsskeyRefresh)
            {
                exp_SetExpIcon(DisplayData.iDsskeyID, exp_Type);
            }
        }
    }

}


bool dsskeyUI_GetDataForUI(int iIndex, DsskeyDisplayData & objDisplayData, int iEXPIndex/* = -1*/)
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
        iDsskeyID = dsskey_GetDsskeyID((DSSKEY_MODULE_TYPE)exp_GetEXPType(), iEXPIndex, iIndex);
    }

    // 获取Dsskey逻辑数据
    bRet = dsskey_GetData(iDsskeyID, sData);

    if (bRet)
    {
        // 转换为显示数据
        return DP_ParseDsskeyPerformance(objDisplayData, sData);
    }
    else
    {
        return false;
    }
}

bool dsskeyUI_DestroyDsskeyDlg()
{
    // 在T3X平台上没有该界面
    // 退出Dsskey All UI界面
    CBaseDialog * pTopWnd = UIManager_GetTopWindow();
    DSSKEYUI_INFO("Enter dsskeyUI_DestroyDsskeyDlg.");
    if (pTopWnd->inherits("CDlgFullDsskeyUI"))
    {
        // Idle界面不新建窗口，直接显示隐藏Dsskey
        pTopWnd->OnBackButtonClick();
    }
    return true;
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

// 目前仅支持linekey数据的获取
bool dsskeyUI_GetDsskeyDataByKeycode(int iKeyCode, DssKeyLogicData & LogicData)
{
    if (iKeyCode < PHONE_KEY_LINE1 || iKeyCode > PHONE_KEY_LINE10)
    {
        return false;
    }
    int iDsskeyID = dsskeyUI_GetDsskeyIDByKeycode(iKeyCode);
    return dsskey_GetData(iDsskeyID, LogicData);
}

// DssKey Setting界面进入 配置界面
void dsskeyUI_EnterSetting(int iIndex)
{
    DSSKEYUI_INFO("Enter dsskeyUI_EnterDssKeyConfigPage.");
    yl::string strAction;
    strAction = commonAPI_FormatString("OpenSubPage(line_key_item_%d)", iIndex + 1);

    if (SettingUI_PageExist(strAction))
    {
        DSSKEYUI_INFO("ConfigPage Already Exist!!");
    }
    else
    {
        SettingUI_OpenPage(SubMenuData(strAction));
    }
}

void dsskeyUI_OnDsskeyLongPress(int iDsskeyID)
{
    if (configParser_GetConfigInt(kszBlockDsskeyExtendedPress) == 1)
    {
        return;
    }

    if (!keyGuard_IsEnableModuld(KGM_MODULE_SETTING))
    {
        return;
    }

    int iEXPIndex = GetEXPIndex(iDsskeyID);
    int iKeyIndex = GetEXPKeyIndex(iDsskeyID);
    DSSKEY_MODULE_TYPE eType = (DSSKEY_MODULE_TYPE)GetEXPModuleType(iDsskeyID);

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

#if IF_FEATURE_SHOW_BLF_CALLID
        if ((DT_BLF == dsskey_GetDsskeyType(iDsskeyID)
                || DT_BLFLIST == dsskey_GetDsskeyType(iDsskeyID))
                && configParser_GetConfigInt(kszBLFShowCallId) != 0
                && blf_ShowBLFMenu(iDsskeyID))
        {
            return;
        }
#endif

#if IF_FEATURE_FAVORITE_DIR
        if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1
                && configParser_GetConfigInt(kszDsskeyConfigMode) == 1)
        {
            if (AdapterBox_TalkUIIsInTalk())
            {
                //通话中不能进入配置界面
            }
            else
            {
                DM_EnterAddLocalContact();
            }
        }
        else
#endif
        {
            dsskeyUI_EnterDssKeyConfigPage(eType, iKeyIndex, iEXPIndex);
        }
    }
}

// 长按进入DssKey配置界面
void dsskeyUI_EnterDssKeyConfigPage(DSSKEY_MODULE_TYPE eType, int iIndex, int iExpIndex /*= -1*/)
{
    // 三级权限控制不显示则不允许进入
    if (!feature_UserModeConfigItemShow("dsskey_frame"))
    {
        return ;
    }

    // 当前在通话中，不允许进入设置界面
    if (talklogic_SessionExist())
    {
        return;
    }

    DSSKEYUI_INFO("Enter dsskeyUI_EnterDssKeyConfigPage.");
    yl::string strAction;
    // 解析打开哪种Dsskey界面
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
        }
        break;
    default:
        {
            if (IsEXPModule(eType)
                    && iExpIndex >= 0)
            {
                strAction = commonAPI_FormatString("OpenSubPage(expand_key_item_%d;%d)", iIndex + 1, iExpIndex);
            }
        }
        break;
    }

//  BaseUI_CreateSubMenuWithSubPage(SubMenuData("OpenMenu(call_feature_list.xml)"), SubMenuData(strAction)
//      , true, "OpenSubPage(dsskey_frame)");

    if (SettingUI_PageExist(strAction.c_str()))
    {
        DSSKEYUI_INFO("ConfigPage Already Exist!!");
        return;
    }

    bool bOpenSuccess = SettingUI_OpenPage(strAction);

    CBaseDialog * pTopWnd = UIManager_GetTopWindow();
    if (bOpenSuccess && pTopWnd != NULL && pTopWnd->inherits(DLG_CSettingWindow))
    {
        // 扩展台要单独设置标题
        if (IsEXPModule(eType)
                && iExpIndex >= 0)
        {
            QString strTitle = LANG_TRANSLATE(TRID_EXTENSION_MODULE);

            strTitle = strTitle + " " + QString::number(iExpIndex + 1);

            strTitle = strTitle + " " + LANG_TRANSLATE(TRID_KEY) + " ";

            int iSize = 0;
            int posStart = strAction.rfind("_");
            int posEnd = strAction.rfind(";");

            if (posStart != yl::string::npos
                    && posEnd != yl::string::npos && posStart < posEnd)
            {
                iSize = posEnd - posStart - 1;
                strTitle = strTitle + toQString(strAction.substr(posStart + 1, iSize).c_str());
            }
            _MainWND.SetTitle(strTitle);
        }
    }
}

bool dsskeyUI_SetCustomBtnDisplay(int iDsskeyID, DsskeyBtnDisplayDelegete * pDelegate)
{
    bool bRet = false;
    if (NULL == pDelegate)
    {
        DSSKEYUI_WARN("Cann't Set a Null Custom Button Display.");
    }
    else
    {
        DSSKEY_MODULE_TYPE eModuleType = dsskey_GetModuleType(iDsskeyID);
        if (DMT_LINEKEY_MODULE == eModuleType)
        {
            int index = dsskey_GetKeyIndex(iDsskeyID);
            bRet = g_pDsskeyUIManager->SetDisplayDelegate(index, pDelegate);
        }
        else
        {
            DSSKEYUI_WARN("Only Linekey Support Set Custom Button Display.");
        }
    }
    return bRet;
}

void dsskeyUI_DebindCustomBtnDisplay(int iDsskeyID)
{
    DSSKEY_MODULE_TYPE eModuleType = dsskey_GetModuleType(iDsskeyID);
    if (DMT_LINEKEY_MODULE == eModuleType)
    {
        int index = dsskey_GetKeyIndex(iDsskeyID);
        g_pDsskeyUIManager->DebindDisplayDelegate(index);
    }
    else
    {
        DSSKEYUI_WARN("Only Linekey Support Dind Custom Button Display.");
    }
}

bool dsskeyUI_DsskeyLightCallback(int iKeyIndex, int iColorType, int iLightStatus)
{
    DsskeyDisplayData objDisplayData;
    bool ret = true;
    objDisplayData.iIndex = iKeyIndex;
    switch ((LEDCOLOR_LTYE)iColorType)
    {
    case LT_RED:
        {
            objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_RED;
        }
        break;
    case LT_GREEN:
        {
            objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_GREEN;
        }
        break;
    case LT_REDGREEN:
        {
            objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_ORANGE;
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
            objDisplayData.eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;
        }
        break;
    case LST_ON:
        {
            objDisplayData.eRefreshType = REFRESH_TYPE_NO_FLASH;
        }
        break;
    case LST_SLOWFLASH:
        {
            objDisplayData.eRefreshType = REFRESH_TYPE_SLOW_FLASH;
        }
        break;
    case LST_FASTFLASH:
        {
            objDisplayData.eRefreshType = REFRESH_TYPE_FAST_FLASH;
        }
        break;
    default:
        {
            ret = false;
        }
        break;
    }

    DSSKEYUI_INFO("DsskeyLightCallback : UpdateLight Index is %d ,iColorType is %d, iLightStatus is %d",
                  iKeyIndex, iColorType, iLightStatus);
    dsskeyUI_CtrlDsskeyLight(objDisplayData);
    return ret;
}

void dsskeyUI_UpdateDsskeyLight(int iIndex, DSS_KEY_COLOR_TYPE eColorType,
                                DSS_KEY_FLASH_TYPE eFlashType)
{
    _DssKeyUILogic.UpdateDsskeyLight(iIndex, eColorType, eFlashType);
}

void dsskeyUI_CtrlDsskeyLight(const DsskeyDisplayData & objLogicData)
{
    g_pDsskeyUIManager->RefreshDsskeyGroundPic(objLogicData);
}

void dsskeyUI_RefreshMultDsskey(int iBegin, int iEnd, DSSKEY_MODULE_TYPE eModule)
{
    _DssKeyUILogic.RefreshMultipleDsskeyData(iBegin, iEnd, eModule);
}

void SetEDKCtrlEXPkeyDisplay(EdkCtrlDsskeyDispData & objEDKDisplayData, yl::string & strLabel,
                             LIGHT_Type & led_status)
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

bool dsskeyUI_IsLineKey(int nKey)
{
    int nDsskeyId = dsskeyUI_GetDsskeyIDByKeycode(nKey);
    DssKey_Type eDsskeyType = dsskey_GetDsskeyType(nDsskeyId);

    return DT_LINE == eDsskeyType;
}

int dsskeyUI_GetColorType(const LIGHT_Type eLightType)
{
    int eColorType = -1;
    switch (eLightType)
    {
    case LT_GREEN_ON:
    case LT_GREEN_FLASH_1:
    case LT_GREEN_FLASH_2:
    case LT_GREEN_FLASH_3:
    case LT_GREEN_FLASH_4:
        {
            eColorType = LT_GREEN;
        }
        break;
    case LT_RED_ON:
    case LT_RED_FLASH_1:
    case LT_RED_FLASH_2:
    case LT_RED_FLASH_3:
    case LT_RED_FLASH_4:
        {
            eColorType = LT_RED;
        }
        break;
    case LT_RED_GREEN_ON:
    case LT_RED_GREEN_FLASH_1:
    case LT_RED_GREEN_FLASH_2:
    case LT_RED_GREEN_FLASH_3:
    case LT_RED_GREEN_FLASH_4:
        {
            eColorType = LT_REDGREEN;
        }
        break;
    default:
        break;
    }
    return eColorType;
}

int dsskeyUI_GetFlushType(const LIGHT_Type eLightType)
{
    int eFlushType = LST_OFF;
    switch (eLightType)
    {
    case LT_RED_FLASH_1:
    case LT_GREEN_FLASH_1:
    case LT_RED_GREEN_FLASH_1:
    case LT_RED_FLASH_2:
    case LT_GREEN_FLASH_2:
    case LT_RED_GREEN_FLASH_2:
        {
            eFlushType = LST_FASTFLASH;
        }
        break;
    case LT_RED_FLASH_3:
    case LT_GREEN_FLASH_3:
    case LT_RED_GREEN_FLASH_3:
    case LT_RED_FLASH_4:
    case LT_GREEN_FLASH_4:
    case LT_RED_GREEN_FLASH_4:
        {
            eFlushType = LST_SLOWFLASH;
        }
        break;
    case LT_GREEN_ON:
    case LT_RED_ON:
    case LT_RED_GREEN_ON:
        {
            eFlushType = LST_ON;
        }
    default:
        break;
    }
    return eFlushType;
}
