#include "explogiccolormanager.h"

#ifdef _EXP_COLOR_SUPPORT
#include "dsklog/log.h"
#include <ylstl/ylstringalgorithm.h>
#include "etlmsghandler/modetlmsghandler.h"

#include "dsskey/include/dsskey_enumtypes.h"

#include "dsskey/include/moddsskey.h"

#include <account/include/modaccount.h>
#include "talk/fwd_dnd/include/modfwddnd.h"
#include <configiddefine.h>

#include "edk/include/edkmsgdefine.h"
#include "edk/include/modedk.h"

#include "adapterbox/include/modadapterbox.h"

#include "lamp/backlight/include/modbacklight.h"

#include "lamp/powersaving/include/powersavingmsgdefine.h"

#include "explogic.h"

#include "exp/include/modexp.h"

//获取Exp38对应的index  (bit0~bit5表示第几个按键，bit6~bit8表示第几个扩展台)
#define GetEXPIndex(i)      (((i) >> 6) & 0xf)
//获取Exp38Key对应的key index
#define GetEXPKeyIndex(i)   ((i) & 0x003f)

enum DSSKEY_AlIGN_TYPE
{
    DSSKEY_ALIGN_NORMOL = 1,
    DSSKEY_AlIGN_RIGHT,
};

CEXPLogicColorManager::CEXPLogicColorManager()
    : m_nPressedDsskeyID(EXP_KEY_NONE)
    , m_nEditingKeyIndex(EXP_KEY_NONE)
    , m_bExpProcessFirstRun(true)
    , m_bExpProcessUninit(true)
    , m_bExpExist(false)
    , m_bPageKeyFlushing(false)
    , m_nFlushStep(0)
    , m_nPageFlushTime(0)
    , m_bPageFlushing(false)
    , m_bPageFlushEnable(true)
    , m_nExpPowerSavingEnable(true)

    , m_bXwindow(true)

{
    for (int i = 0; i < MAX_EXP_NUM; ++i)
    {
        for (int j = 0; j < EXP50_PAGEKEY_COUNT; j++)
        {
            m_aPageStatus[i][j] = 0;
            for (int k = 0; k < EXP_PAGE_SIZE; k++)
            {
                m_aKeyStatus[i][j][k] = 0;
            }
        }
    }

    for (int i = 0; i < MAX_EXP_NUM; ++i)
    {
        m_nExpStatus[i] = EXP50_STATUS_NONE;
    }
    memset(m_szLabel, 0, sizeof(m_szLabel));
}

CEXPLogicColorManager::~CEXPLogicColorManager()
{

}

CEXPLogicColorManager & CEXPLogicColorManager::GetInstance()
{
    static CEXPLogicColorManager instance;
    return instance;
}

void CEXPLogicColorManager::Init()
{
    etl_RegisterMsgHandle(EXP_MSG_EXP_KEY_EVENT, EXP_MSG_EXP_KEY_EVENT, ::OnEXPEvent);
    etl_RegisterMsgHandle(DEV_MSG_EXP_PAGE_KEY, DEV_MSG_EXP_PAGE_KEY, ::OnEXPEvent);
    etl_RegisterMsgHandle(EXP_MESSAGE_SWITCH_PAGE, EXP_MESSAGE_SWITCH_PAGE, ::OnEXPEvent);
    etl_RegisterMsgHandle(EXP_MESSAGE_PROCESS_READY, EXP_MESSAGE_PROCESS_READY, ::OnEXPEvent);
    etl_RegisterMsgHandle(EXP_MSG_EXP_READY, EXP_MSG_EXP_READY, ::OnEXPEvent);
    etl_RegisterMsgHandle(EDK_MSG_LIGHTING, EDK_MSG_LIGHTING, ::OnEXPEvent);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, ::OnEXPEvent);
}

void CEXPLogicColorManager::Uninit()
{
    etl_UnregisterMsgHandle(EXP_MSG_EXP_KEY_EVENT, EXP_MSG_EXP_KEY_EVENT, ::OnEXPEvent);
    etl_UnregisterMsgHandle(DEV_MSG_EXP_PAGE_KEY, DEV_MSG_EXP_PAGE_KEY, ::OnEXPEvent);
    etl_UnregisterMsgHandle(EXP_MESSAGE_SWITCH_PAGE, EXP_MESSAGE_SWITCH_PAGE, ::OnEXPEvent);
    etl_UnregisterMsgHandle(EXP_MESSAGE_PROCESS_READY, EXP_MESSAGE_PROCESS_READY, ::OnEXPEvent);
    etl_UnregisterMsgHandle(EXP_MSG_EXP_READY, EXP_MSG_EXP_READY, ::OnEXPEvent);
    etl_UnregisterMsgHandle(EDK_MSG_LIGHTING, EDK_MSG_LIGHTING, ::OnEXPEvent);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, ::OnEXPEvent);
}

bool CEXPLogicColorManager::OnExpInsert(int nExpIndex, DSSKEY_MODULE_TYPE eExpType)
{
    DSSKEY_INFO("CEXPLogicColorManager::OnExpInsert nExpIndex = [%d]\n", nExpIndex);
    if (nExpIndex < 0)
    {
        return false;
    }

    destroyCloseDelayTimer();

    RunExpProcess();

    if (m_bXwindow && m_bExpProcessFirstRun)
    {
        ExpProcessReadyInit();
    }
    if (!m_bExpExist)
    {
        msgPostMsgToThreadEx(
            msgGetThreadByName(VP_EXP_COLOR_THREAD_NAME)
            , EXP_MESSAGE_PROCESS_RELOAD
            , 1
            , 0
            , 0
            , NULL);

        m_bExpExist = true;
    }

    SetExpInitStatus(nExpIndex, EXP50_STATUS_INITING);

    msgPostMsgToThreadEx(
        msgGetThreadByName(VP_EXP_COLOR_THREAD_NAME)
        , EXP_MESSAGE_COUNT_CHANGE
        , exp_GetExpCounts(DT_EXP_50)
        , 0
        , 0
        , NULL);

    if (!m_bExpProcessUninit)
    {
        SetExpInitStatus(nExpIndex, EXP50_STATUS_INITED);
        if (nExpIndex < MAX_EXP_NUM)
        {
            if (!exp_IsTestMode())
            {
                RefreshExp(nExpIndex);
            }

            etl_NotifyApp(false, DSK_MSG_EXT_CHANGE, eExpType, nExpIndex);
            msgBroadpostThreadMsg(DSK_MSG_EXT_CHANGE, eExpType,
                                  nExpIndex);//向所有线程广播扩展台拔插的消息
        }
    }
    return true;
}

bool CEXPLogicColorManager::OnExpRemove(int nExpIndex, DSSKEY_MODULE_TYPE eExpType)
{
    if (nExpIndex < 0)
    {
        return false;
    }

    backlight_ResetExpLightShake();

    SetExpInitStatus(nExpIndex, EXP50_STATUS_NONE);
	dsskey_RemoveModule(eExpType, nExpIndex);
    etl_NotifyApp(false, DSK_MSG_EXT_CHANGE, eExpType, nExpIndex);
    msgBroadpostThreadMsg(DSK_MSG_EXT_CHANGE, eExpType,
                          nExpIndex);//向所有线程广播扩展台拔插的消息

    DSSKEY_INFO("CEXPLogicColorManager::OnExpRemove exp_GetExpCounts(DT_EXP_50) = [%d]\n",
                exp_GetExpCounts(DT_EXP_50));

    msgPostMsgToThreadEx(
        msgGetThreadByName(VP_EXP_COLOR_THREAD_NAME)
        , EXP_MESSAGE_COUNT_CHANGE
        , exp_GetExpCounts(DT_EXP_50)
        , 0
        , 0
        , NULL);

    if (exp_GetExpCounts(DT_EXP_50) <= 0)
    {
        m_bExpExist = false;

        msgPostMsgToThreadEx(
            msgGetThreadByName(VP_EXP_COLOR_THREAD_NAME)
            , EXP_MESSAGE_PROCESS_RELOAD
            , 0
            , 0
            , 0
            , NULL);

        //      setCloseDelayTimer();
        msgObject objMessage;
        OnCloseDelayTimerOut(objMessage);
    }

    return true;
}

void CEXPLogicColorManager::ProcessEXPPageEvent(int nEXPIndex, int nPage)
{
    DSSKEY_INFO("CEXPLogicManager::ProcessEXPPageEvent EXPIndex [%d], PageIndex [%d]", nEXPIndex,
                nPage);

    if (exp_IsTestMode())
    {
        DSSKEY_INFO("CEXPLogicManager::ProcessEXPPageEvent exp is in test mode");
        return;
    }

    if (m_bExpProcessUninit)
    {
        DSSKEY_INFO("CEXPLogicManager::ProcessEXPPageEvent ExpProcessUninit");
        return;
    }

    msgPostMsgToThreadEx(
        msgGetThreadByName(VP_EXP_COLOR_THREAD_NAME)
        , EXP_MESSAGE_BG_REFRESH
        , exp_GetExpCounts(DT_EXP_50)
        , nEXPIndex
        , 0
        , NULL);

    if (GetExpInitStatus(nEXPIndex) != EXP50_STATUS_NONE)
    {
        //先进行一次发送，清掉原有队列里的内容
        SendEXPInfo();

		if (dsskey_IsDssKeyModuleExist(nEXPIndex))
		{
			AddExpFlashPageInfo(nEXPIndex);
		}

		dsskey_RefressDsskeyUIDuetoSwitchExpPage(nEXPIndex, DSS_KEY_REFRESH_ALL);

        //立刻进行发送
        SendEXPInfo();

        DSSKEY_INFO("CEXPLogicManager::ProcessEXPPageEvent current page [%d]", nPage);

        ShowEXPPageLight(nEXPIndex);
    }
}

void CEXPLogicColorManager::ProcessEXPKeyEvent(int nDsskeyID, int nStatus)
{
    if (exp_IsTestMode())
    {
        return;
    }

    if (EXP_KEY_STATUS_PRESS == nStatus)
    {
        //仅当没有其他扩展台按键按下时才记录当前按下的键
        if (EXP_KEY_NONE == m_nPressedDsskeyID)
        {
            m_nPressedDsskeyID = nDsskeyID;

            dsskey_refreshDSSKeyUI(nDsskeyID);
        }
    }
    else if (EXP_KEY_STATUS_RELEASE == nStatus)
    {
        if (nDsskeyID == m_nPressedDsskeyID)
        {
            m_nPressedDsskeyID = EXP_KEY_NONE;

            dsskey_refreshDSSKeyUI(nDsskeyID);
        }
    }
}

bool CEXPLogicColorManager::RefreshExp(int nExpIndex, bool bRefreshStatusInfo/* = true*/)
{

    DSSKEY_INFO("CEXPLogicColorManager::RefreshExp nExpIndex = [%d]\n", nExpIndex);

    if (m_bExpProcessUninit)
    {
        return false;
    }

    ResetEXPLight(nExpIndex);

    msgPostMsgToThreadEx(
        msgGetThreadByName(VP_EXP_COLOR_THREAD_NAME)
        , EXP_MESSAGE_BG_REFRESH
        , exp_GetExpCounts(DT_EXP_50)
        , nExpIndex
        , 0
        , NULL);

    SendEXPInfo();

	if (dsskey_IsDssKeyModuleExist(nExpIndex, true))
	{
		AddExpFlashPageInfo(nExpIndex);
	}

	dsskey_RefressDsskeyUIDuetoExpUpdate(nExpIndex, true);

    if (bRefreshStatusInfo)
    {
        RefreshStatusInfo();
    }

    // 设置EXP对比度
    // 如果Backlight Time为Always Off，代表不亮灯，不亮扩展台的背光灯
    if (1 == configParser_GetConfigInt(kszBackLightTime))
    {
        return true;
    }

    ShowEXPPageLight(nExpIndex);
    SendEXPInfo();

    return true;
}

bool CEXPLogicColorManager::InitExpData()
{

    if (!m_bXwindow)
    {
        system(PHONE_SCRIPTS_PATH "expapp.sh close &");
    }

    bool bEnable = true;// 1 == configParser_GetConfigInt(kszPowerSavingEXPEnable);
    EXPPowerSavingEnable(bEnable);

    bEnable = 1 == configParser_GetConfigInt(kszExpPageLedBlfCallInEnable);
    PageKeyEDKFlushEnable(bEnable);

    EDK_DoAction(configParser_GetConfigString(kdzExpPageLedBlfCallInRule)
                 , LIGHTING_EXP_FLIP
                 , EXP_FLIP_TRIGGER);
    return true;
}


yl::string CEXPLogicManager::GetDrawThreadName()
{
    return VP_EXP_COLOR_THREAD_NAME;
}

LRESULT CEXPLogicColorManager::OnEXPEvent(msgObject & msg)
{
    switch (msg.message)
    {
    case EXP_MESSAGE_SWITCH_PAGE:
        {
            int nKeyCode = msg.lParam;

            int nExpIndex = GetEXPIndex(nKeyCode);
            int nExpPage = GetEXPKeyIndex(nKeyCode) - EXP_KEY_PAGE1;
            DSSKEY_INFO("explogicmanager:msg[EXP_MESSAGE_SWITCH_PAGE] dssId[%d] expIndex[%d] pageIndex[%d]",
                        nKeyCode, nExpIndex, nExpPage);
            ProcessEXPPageEvent(nExpIndex, nExpPage);

        }
        break;

    case EXP_MESSAGE_PROCESS_READY:
        {

            if (!m_bXwindow)
            {
                ExpProcessReadyInit();
            }

        }
        break;

    case EXP_MSG_EXP_KEY_EVENT:
        {
            int nDsskeyID = msg.wParam;
            int nStatus = msg.lParam;

            ProcessEXPKeyEvent(nDsskeyID, nStatus);
        }
        break;

    //http://bugfree.yealink.com/Bug.php?BugID=103155
    case DEV_MSG_EXP_PAGE_KEY:
        {
            AdapterBox_ExitScreenSaver();
            //http://bugfree.yealink.com/Bug.php?BugID=105990
            etl_NotifyApp(false, POWERSAVIGNMEG_NOTIFY_OPERATE, 1, 0);
        }
        break;

    case EXP_MSG_EXP_READY:
        {
            //to do
        }
        break;

    case EDK_MSG_LIGHTING:
        {
            int nSize = msg.GetExtraSize();
            int nCount = 0;
            LedRuleData * pEXPLightList = NULL;
            pEXPLightList = (LedRuleData *)msg.GetExtraData();

            if (nSize > 0 && NULL != pEXPLightList)
            {
                nCount = nSize / sizeof(LedRuleData);

                ProcessEDKEvent(pEXPLightList, nCount);
            }
        }
        break;

    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (ST_EXP_PAGE_BLF_NOTIFY == msg.wParam)
            {
                if (1 == msg.lParam)
                {
                    bool bEnable = 1 == configParser_GetConfigInt(kszExpPageLedBlfCallInEnable);
                    PageKeyEDKFlushEnable(bEnable);
                }
                else if (2 == msg.lParam)
                {
                    EDK_DoAction(configParser_GetConfigString(kdzExpPageLedBlfCallInRule)
                                 , LIGHTING_EXP_FLIP
                                 , EXP_FLIP_TRIGGER);
                }
            }
            else if (ST_EXP_WALLPAPER == msg.wParam)
            {
                yl::string strPath = configParser_GetConfigString(kszCurrentExpWallPaper);
                SetEXPBackGround(commonUnits_Hide2Real(strPath, PHFILE_TYPE_EXP_WALLPAPER_FILE).c_str(), -1);
            }
            else if (ST_EXP_POWER_SAVING == msg.wParam)
            {
                bool bEnable = true;// 1 == configParser_GetConfigInt(kszPowerSavingEXPEnable);
                EXPPowerSavingEnable(bEnable);
            }
#if IS_EXP_SUPPORT_TRANSPARENCY
            else if (ST_TRANSPARENTCY == msg.wParam)
            {
                ChangeWidgetTransparency();
            }
#endif
        }
        break;

    default:
        break;
    }

    return true;
}

int CEXPLogicColorManager::SetExpIcon(int nDsskeyID, int nIcon)
{
    AddEXPUpdateData(nDsskeyID, "", nIcon, 0, configParser_GetConfigInt(kszDsskeyShowType) == 1 ? DSSKEY_AlIGN_RIGHT : DSSKEY_ALIGN_NORMOL, true, false, false);
    return 1;
}

int CEXPLogicColorManager::SetExpLabel(int nDsskeyID, const char * pStrLabel)
{
    AddEXPUpdateData(nDsskeyID, pStrLabel, 0, 0, configParser_GetConfigInt(kszDsskeyShowType) == 1 ? DSSKEY_AlIGN_RIGHT : DSSKEY_ALIGN_NORMOL, false, true, false);
    return 1;
}

int CEXPLogicColorManager::SetExpKeyDisp(int nDsskeyID, int nIcon, const char * pStrLabel,
        int nLedStatus, int nDsskeyBgType, int nDisplayFlag /*=1*/, const char * pStrIcon /*= NULL*/,
        int nStatus /*= 0*/, int nAuxiliaryStatus /*= 0*/)
{
    DSSKEY_INFO("SetExpKeyDisp, nDsskeyID[%d]", nDsskeyID);

    AddEXPUpdateData(nDsskeyID
                     , pStrLabel, nIcon, nLedStatus, nDisplayFlag
                     , true, true, true
                     , pStrIcon, nStatus
                     , true, nAuxiliaryStatus, nDsskeyBgType);

    return 0;
}

int CEXPLogicColorManager::SetExpLEDStatus(int nDsskeyID, int nLedStatus)
{
    AddEXPUpdateData(nDsskeyID
                     , "", 0, nLedStatus, configParser_GetConfigInt(kszDsskeyShowType) == 1 ? DSSKEY_AlIGN_RIGHT : DSSKEY_ALIGN_NORMOL
                     , false, false, true);
    return 0;
}

int CEXPLogicColorManager::SetExpBKLight(int nExpIndex, int nBKLight)
{
    msgPostMsgToThreadEx(
        msgGetThreadByName(VP_EXP_COLOR_THREAD_NAME)
        , EXP_MESSAGE_BACKLIGHT
        , exp_GetExpCounts(DT_ALL)
        , nExpIndex
        , sizeof(int) + 1
        , &nBKLight);
    return 1;
}

int CEXPLogicColorManager::SetEXPBackGround(const char * pStrBackGround,
        int nTransparentLevel /*= 0*/, bool bRefresh /*= true*/)
{
    int nTransLevel = nTransparentLevel;

    if (0 != nTransLevel)
    {

#if IS_EXP_SUPPORT_TRANSPARENCY
        yl::string strTransLevel = configParser_GetConfigString(kszTransparencyType);

        if (strTransLevel == szTransTypeLowTrans)
        {
            nTransLevel = 1;
        }
        else if (strTransLevel == szTransType20Percent)
        {
            nTransLevel = 2;
        }
        else if (strTransLevel == szTransType40Percent)
        {
            nTransLevel = 3;
        }
        else if (strTransLevel == szTransType60Percent)
        {
            nTransLevel = 4;
        }
        else if (strTransLevel == szTransType80Percent)
        {
            nTransLevel = 5;
        }
        else
        {
            nTransLevel = 6;
        }
#else
        nTransLevel = 6;
#endif
    }

    ExpBgData ExpBGData;

    if (NULL != pStrBackGround)
    {
        if (strlen(pStrBackGround) >= EXP_PIC_PATH_MAX_SIZE)
        {
            //为通过CppCheck:memcpy长度传入变量而不是固定值
            int lenth = EXP_PIC_PATH_MAX_SIZE - 1;
            memcpy(ExpBGData.szBackGround, pStrBackGround, lenth);
        }
        else
        {
            strcpy(ExpBGData.szBackGround, pStrBackGround);
        }
    }

    ExpBGData.nTransparentLevel = nTransLevel;

    msgPostMsgToThreadEx(
        msgGetThreadByName(VP_EXP_COLOR_THREAD_NAME)
        , EXP_MESSAGE_BG_CHANGE
        , exp_GetExpCounts(DT_EXP_50)
        , 0
        , sizeof(ExpBgData) + 1
        , &ExpBGData);

    if (!exp_IsTestMode()
            && bRefresh)
    {
        for (int i = 0; i < MAX_EXP_NUM; ++i)
        {
            if (GetExpInitStatus(i) != EXP50_STATUS_NONE)
            {
                RefreshExp(i, false);
            }
        }

        RefreshStatusInfo();
    }

    return 1;
}

int CEXPLogicColorManager::EnterExpScreenSaver(bool bEnter)
{
    msgPostMsgToThreadEx(
        msgGetThreadByName(VP_EXP_COLOR_THREAD_NAME)
        , EXP_MESSAGE_SCREENSAVER
        , exp_GetExpCounts(DT_EXP_50)
        , bEnter ? 1 : 0
        , 0
        , NULL);

    return 1;
}

int CEXPLogicColorManager::SetExpContrast(int nExpIndex, int nContrast)
{
    return 0;
}

yl::string CEXPLogicColorManager::GetDrawThreadName()
{
    return VP_EXP_COLOR_THREAD_NAME;
}

bool CEXPLogicColorManager::GetUseEXPColor() const
{
    return true;
}

void CEXPLogicColorManager::ResetEXPLight(int nIndex)
{
    int nEXPCount = exp_GetExpCounts(DT_ALL);
    DSSKEY_INFO("ResetEXPLight %d, %d\n", nIndex, nEXPCount);

    msgPostMsgToThreadEx(
        msgGetThreadByName(VP_EXP_COLOR_THREAD_NAME)
        , EXP_MESSAGE_RESET_LIGHT
        , exp_GetExpCounts(DT_ALL)
        , nIndex
        , 0
        , NULL);

    ShowEXPPageLight(nIndex);
}

void CEXPLogicColorManager::ShowEXPPageLight(int nIndex)
{
    int nCurrentPage = exp_GetCurrentPageByIndex(nIndex);
    DSSKEY_INFO("ShowEXPPageLight %d, %d\n", nIndex, nCurrentPage);

    for (int i = 0; i < EXP50_PAGEKEY_COUNT; ++i)
    {
        SetExpLEDStatusByIndex(nIndex, EXP_KEY_PAGE1 + i, (i == nCurrentPage ? LT_GREEN_ON : LT_OFF));
    }
}

int CEXPLogicColorManager::SetExpLEDStatusByIndex(int nExpIndex, int nKeyIndex, int nLedStatus)
{
    AddEXPLightData(nExpIndex, nKeyIndex, nLedStatus);
    return 0;
}

void CEXPLogicColorManager::RunExpProcess()
{
    if (m_bExpProcessFirstRun)
    {

        if (!m_bXwindow)
        {
            system(PHONE_SCRIPTS_PATH "expapp.sh open &");
            m_bExpProcessFirstRun = false;
        }

    }
}

EXP50_INIT_STATUS CEXPLogicColorManager::GetExpInitStatus(int iIndex)
{
    if (iIndex >= MAX_EXP_NUM || iIndex < 0)
    {
        return EXP50_STATUS_NONE;
    }
    return m_nExpStatus[iIndex];
}

void CEXPLogicColorManager::SetExpInitStatus(int iIndex, EXP50_INIT_STATUS eStatus)
{
    if (iIndex < 0 || iIndex >= MAX_EXP_NUM)
    {
        return;
    }

    if (m_nExpStatus[iIndex] == eStatus)
    {
        return;
    }

    DSSKEY_INFO("set exp[%d] status to[%d]", eStatus);
    m_nExpStatus[iIndex] = eStatus;
}


void CEXPLogicColorManager::EXPFillWithColor(const EXP_FILL_COLOR & objExpColor, int nIndex/* = 0*/)
{
    msgPostMsgToThreadEx(
        msgGetThreadByName(VP_EXP_COLOR_THREAD_NAME)
        , EXP_MESSAGE_FILL_COLOR
        , exp_GetExpCounts(DT_EXP_50)
        , nIndex
        , sizeof(EXP_FILL_COLOR) + 1
        , &objExpColor);
}

void CEXPLogicColorManager::EXPFillWithPicture(const char * pStrPicture, int nIndex/* = 0*/)
{
    char szPicture[EXP_PIC_PATH_MAX_SIZE] = {0};

    if (NULL != pStrPicture)
    {
        if (strlen(pStrPicture) >= EXP_PIC_PATH_MAX_SIZE)
        {
            //为通过CppCheck:memcpy长度传入变量而不是固定值
            int lenth = EXP_PIC_PATH_MAX_SIZE - 1;
            memcpy(szPicture, pStrPicture, lenth);
        }
        else
        {
            strcpy(szPicture, pStrPicture);
        }
    }

    msgPostMsgToThreadEx(
        msgGetThreadByName(VP_EXP_COLOR_THREAD_NAME)
        , EXP_MESSAGE_FILL_PICTURE
        , exp_GetExpCounts(DT_EXP_50)
        , nIndex
        , EXP_PIC_PATH_MAX_SIZE + 1
        , szPicture);
}

void CEXPLogicColorManager::EXPDrawText(const char * pStrText, int nIndex/* = 0*/)
{
    char szText[EXP_PIC_PATH_MAX_SIZE] = {0};

    if (NULL != pStrText)
    {
        if (strlen(pStrText) >= EXP_PIC_PATH_MAX_SIZE)
        {
            //为通过CppCheck:memcpy长度传入变量而不是固定值
            int lenth = EXP_PIC_PATH_MAX_SIZE - 1;
            memcpy(szText, pStrText, lenth);
        }
        else
        {
            strcpy(szText, pStrText);
        }
    }

    msgPostMsgToThreadEx(
        msgGetThreadByName(VP_EXP_COLOR_THREAD_NAME)
        , EXP_MESSAGE_DRAW_TEXT
        , exp_GetExpCounts(DT_EXP_50)
        , nIndex
        , EXP_PIC_PATH_MAX_SIZE + 1
        , szText);
}

void CEXPLogicColorManager::EXPShowLogo(int nIndex /*= 0*/)
{
    msgPostMsgToThreadEx(
        msgGetThreadByName(VP_EXP_COLOR_THREAD_NAME)
        , EXP_MESSAGE_SHOW_LOGO
        , exp_GetExpCounts(DT_EXP_50)
        , nIndex
        , 0
        , NULL);
}


void CEXPLogicColorManager::SetEditingKey(bool bEdit, int nDsskeyID/* = -1*/)
{
    if (bEdit)
    {
        m_nEditingKeyIndex = nDsskeyID;
        dsskey_refreshDSSKeyUI(nDsskeyID);
    }
    else
    {
        int nRefreshID = m_nEditingKeyIndex;
        m_nEditingKeyIndex = -1;
        dsskey_refreshDSSKeyUI(nRefreshID);
    }
}

void CEXPLogicColorManager::SetEditingText(const char * pStrLabel)
{
    if (strlen(pStrLabel) >= EXP_LABEL_MAX_SIZE)
    {
        //为通过CppCheck:memcpy长度传入变量而不是固定值
        int lenth = EXP_LABEL_MAX_SIZE - 1;
        memcpy(m_szLabel, pStrLabel, lenth);
    }
    else
    {
        strcpy(m_szLabel, pStrLabel);
    }

    if (!m_bExpProcessUninit)
    {
        SendEditingText();
    }
}

void CEXPLogicColorManager::SendEditingText()
{
    msgPostMsgToThreadEx(
        msgGetThreadByName(VP_EXP_COLOR_THREAD_NAME)
        , EXP_MESSAGE_EDITING_TEXT
        , 0
        , 0
        , EXP_LABEL_MAX_SIZE + 1
        , m_szLabel);
}

int CEXPLogicColorManager::GetLedTypeByLedRuleColorType(int nLedRuleColorType)
{
    int nLedType = LT_OFF;

    switch (nLedRuleColorType)
    {
    case COLOR_RED:
        {
            nLedType = LT_RED_ON;
        }
        break;
    case COLOR_GREEN:
        {
            nLedType = LT_GREEN_ON;
        }
        break;
    case COLOR_ORANGE:
        {
            // Exp 暂时没有橙灯业务
            // nLedType = ;
        }
        break;
    default:
        break;
    }

    return nLedType;
}


void CEXPLogicColorManager::SetPageFlushTimer()
{
    DSSKEY_INFO("CEXPLogicColorManager::setPageFlushTimer");

    m_bPageKeyFlushing = true;

    DestroyPageFlushTimer();
    if (m_nPageFlushTime > 0)
    {
        m_objPageFlushTimer.SetTimer(m_nPageFlushTime, NULL, MK_MSG_ENTRY(this,
                                     CEXPLogicColorManager::OnPageFlushTimerOut));
    }
}

void CEXPLogicColorManager::DestroyPageFlushTimer()
{
    if (m_objPageFlushTimer.IsTimerRuning())
    {
        m_objPageFlushTimer.KillTimer();
    }
}

BOOL CEXPLogicColorManager::OnPageFlushTimerOut(msgObject & objMessage)
{
    int nSize = m_listPageFlushRule.size();
    DSSKEY_INFO("CEXPLogicColorManager::OnPageFlushTimerOut Size %d, Step %d", nSize, m_nFlushStep);

    if (m_nFlushStep >= nSize)
    {
        DestroyPageFlushTimer();
        m_nFlushStep = 0;
        m_bPageKeyFlushing = false;
        return 0;
    }

    int nLightStatus = 0;
    bool bLoop = true;
    if (m_nFlushStep == (nSize - 1))
    {
        if (COLOR_LOOP == m_listPageFlushRule[m_nFlushStep].m_eColorType)
        {
            m_nFlushStep = 0;
            bLoop = true;
        }
        else
        {
            bLoop = false;
        }

        nLightStatus = GetLedTypeByLedRuleColorType(m_listPageFlushRule[m_nFlushStep].m_eColorType);
    }
    else
    {
        nLightStatus = GetLedTypeByLedRuleColorType(m_listPageFlushRule[m_nFlushStep].m_eColorType);
    }

    bool bFlush = false;
    for (int i = 0; i < MAX_EXP_NUM; ++i)
    {
        if (GetExpInitStatus(i) == EXP50_STATUS_NONE)
        {
            continue;
        }

        int nCurrentPage = exp_GetCurrentPageByIndex(i);
        for (int j = 0; j < EXP50_PAGEKEY_COUNT; ++j)
        {
            if (j != nCurrentPage && m_aPageStatus[i][j] > 0)
            {
                bFlush = true;
                DSSKEY_INFO("CEXPLogicColorManager::OnPageFlushTimerOut PageLight Page %d, Light %d", j,
                            nLightStatus);
                SetExpLEDStatusByIndex(i, j + EXP_KEY_PAGE1, nLightStatus);
            }
        }
    }

    //如果不需要循环或者已经没有翻页键需要刷，则退出
    if (!bLoop || !bFlush)
    {
        PageKeyEDKFlushSwitch(false);
    }
    else
    {
        m_nPageFlushTime = m_listPageFlushRule[m_nFlushStep].m_iDurTime;
        m_nFlushStep++;
        SetPageFlushTimer();
    }

    return true;
}

void CEXPLogicColorManager::RecordAuxiliaryStatus(int nExpIndex, int nPage, int nKeyIndex,
        int nStatus)
{
    if (nExpIndex >= MAX_EXP_NUM
            || nPage >= EXP50_PAGEKEY_COUNT
            || nKeyIndex >= EXP_PAGE_SIZE)
    {
        return;
    }

    int nLastStatus = m_aKeyStatus[nExpIndex][nPage][nKeyIndex];
    m_aKeyStatus[nExpIndex][nPage][nKeyIndex] = nStatus;

    //从有到无
    if (DS_BLF_STATUS_RINGING == nLastStatus && DS_BLF_STATUS_RINGING != nStatus)
    {
        m_aPageStatus[nExpIndex][nPage]--;

        if (m_aPageStatus[nExpIndex][nPage] < 0)
        {
            m_aPageStatus[nExpIndex][nPage] = 0;
        }

        // 当无需闪灯时,且在当前界面，应该通知灭灯
        if (0 == m_aPageStatus[nExpIndex][nPage] && exp_GetCurrentPageByIndex(nExpIndex) != nPage)
        {
            SetExpLEDStatusByIndex(nExpIndex, nPage + EXP_KEY_PAGE1, LT_OFF);
        }
    }
    //从无到有
    else if (DS_BLF_STATUS_RINGING != nLastStatus && DS_BLF_STATUS_RINGING == nStatus)
    {
        m_aPageStatus[nExpIndex][nPage]++;
        if (m_aPageStatus[nExpIndex][nPage] > EXP_PAGE_SIZE)
        {
            m_aPageStatus[nExpIndex][nPage] = EXP_PAGE_SIZE;
        }
    }
    //其他情况不需要修改

    if (!m_bPageKeyFlushing && m_bPageFlushEnable)
    {
        SetPageFlushTimer();
    }
}

void CEXPLogicColorManager::AddEXPUpdateData(int nDssKeyID
        , const char * pStrLabel, int nIcon, int nLedStatus, int nDisplayFlag
        , bool bPaintIcon, bool bPaintText, bool bLight
        , const char * pStrIcon, int nStatus
        , bool bSetAuxiliaryStatus, int nAuxiliaryStatus,
        int nDsskeyBgType/* = EXP_DSS_GROUNDPHOTO_MODE_NORMAL*/)
{
    int nExpIndex = -1;
    int nRow = 0;
    int nCol = 0;
    int nPage = 0;

    GetExpInfo(nDssKeyID, nExpIndex, nRow, nPage, nCol);

    if (bSetAuxiliaryStatus)
    {
        RecordAuxiliaryStatus(nExpIndex, nPage, nRow * 2 + nCol, nAuxiliaryStatus);
    }

    //测试模式不刷业务按钮
    if (exp_IsTestMode()
            //不在当前页不刷
            || exp_GetCurrentPageByIndex(nExpIndex) != nPage)
    {
        return;
    }

    if (GetExpInitStatus(nExpIndex) != EXP50_STATUS_NONE)
    {
        ExpDisplayItemInfo ExpData;

        ExpData.m_eDataType = EXP_UPDATE_DATA_NOMAL;

        if (NULL != pStrLabel)
        {
            if (strlen(pStrLabel) >= EXP_LABEL_MAX_SIZE)
            {
                //为通过CppCheck:memcpy长度传入变量而不是固定值
                int lenth = EXP_LABEL_MAX_SIZE - 1;
                memcpy(ExpData.szLabel, pStrLabel, lenth);
            }
            else
            {
                strcpy(ExpData.szLabel, pStrLabel);
            }
        }

        ExpData.m_nDsskeyID = nDssKeyID;

        ExpData.m_nIndex = nExpIndex;
        ExpData.m_nPage = nPage;
        ExpData.m_nRol = nRow;
        ExpData.m_nCol = nCol;

        ExpData.icon = nIcon;
        ExpData.ledStatus = nLedStatus;
        ExpData.flag = nDisplayFlag;

        ExpData.bPaintIcon = bPaintIcon;
        ExpData.bPaintText = bPaintText;
        ExpData.bLight = bLight;

        ExpData.m_eBgType = (EXP_DSS_KEY_GROUNDPHOTO_MODE)nDsskeyBgType;

        if (nDssKeyID == m_nPressedDsskeyID)
        {
            ExpData.bDown = true;
        }
        else
        {
            ExpData.bDown = false;
        }

        if (nDssKeyID == m_nEditingKeyIndex)
        {
            ExpData.ledStatus = LT_GREEN_ON;
            ExpData.bEditing = true;
        }

        if (NULL != pStrIcon)
        {
            if (strlen(pStrIcon) >= EXP_ICON_PATH_MAX_SIZE)
            {
                //为通过CppCheck:memcpy长度传入变量而不是固定值
                int lenth = EXP_ICON_PATH_MAX_SIZE - 1;
                memcpy(ExpData.szIcon, pStrIcon, lenth);
            }
            else
            {
                strcpy(ExpData.szIcon, pStrIcon);
            }
        }

        ExpData.nStatus = nStatus;

        AddEXPInfo(ExpData);
    }
}

void CEXPLogicColorManager::AddExpFlashPageInfo(int nExpIndex)
{
    ExpDisplayItemInfo ExpData;

    ExpData.m_eDataType = EXP_UPDATE_DATA_FLASH_PAGE;
    ExpData.m_nIndex = nExpIndex;

    AddEXPInfo(ExpData);
}

void CEXPLogicColorManager::AddEXPLightData(int nExpIndex, int nKeyIndex, int nLedStatus)
{
    if (GetExpInitStatus(nExpIndex) != EXP50_STATUS_NONE)
    {
        ExpDisplayItemInfo ExpData;

        ExpData.ledStatus = nLedStatus;
        ExpData.bPaintIcon = false;
        ExpData.bPaintText = false;
        ExpData.bLight = true;

        ExpData.m_nIndex = nExpIndex;
        ExpData.m_nPage = 0;

        if (nKeyIndex >= EXP_KEY_PAGE1)
        {
            ExpData.m_nRol = EXP50_PAGEKEY_LINE_INDEX;
            ExpData.m_nCol = nKeyIndex - EXP_KEY_PAGE1;
        }
        else
        {
            ExpData.m_nRol = nKeyIndex / 2;
            ExpData.m_nCol = nKeyIndex % 2;
        }

        AddEXPInfo(ExpData);
    }
}

void CEXPLogicColorManager::ExpProcessReadyInit()
{
    m_bExpProcessUninit = false;

    if (m_bXwindow)
    {
        m_bExpProcessFirstRun = false;
    }

    msgPostMsgToThreadEx(
        msgGetThreadByName(VP_EXP_COLOR_THREAD_NAME)
        , EXP_MESSAGE_COUNT_CHANGE
        , exp_GetExpCounts(DT_EXP_50)
        , 0
        , 0
        , NULL);

    SetExpInfoSendTimer();

    // 从配置文件获取背景图路径
    yl::string strPath = configParser_GetConfigString(kszCurrentExpWallPaper);
    SetEXPBackGround(commonUnits_Hide2Real(strPath, PHFILE_TYPE_EXP_WALLPAPER_FILE).c_str(), -1, false);

    SendEditingText();

    for (int i = 0; i < MAX_EXP_NUM; ++i)
    {
        if (!exp_IsTestMode() && GetExpInitStatus(i) == EXP50_STATUS_INITING)
        {
            SetExpInitStatus(i, EXP50_STATUS_INITED);

            RefreshExp(i, false);
        }

        etl_NotifyApp(false, DSK_MSG_EXT_CHANGE, DMT_EXP50_MODULE, i);
        msgBroadpostThreadMsg(DSK_MSG_EXT_CHANGE, DMT_EXP50_MODULE,
                              i);//向所有线程广播扩展台拔插的消息
    }

    RefreshStatusInfo();
}


void CEXPLogicColorManager::setCloseDelayTimer()
{
    destroyCloseDelayTimer();
    m_objCloseDelayTimer.SetTimer(EXP_PROCESS_DELAY_CLOSE_TIME, NULL, MK_MSG_ENTRY(this,
                                  CEXPLogicColorManager::OnCloseDelayTimerOut));
}

void CEXPLogicColorManager::destroyCloseDelayTimer()
{
    if (m_objCloseDelayTimer.IsTimerRuning())
    {
        m_objCloseDelayTimer.KillTimer();
    }
}

BOOL CEXPLogicColorManager::OnCloseDelayTimerOut(msgObject & objMessage)
{

    if (!m_bXwindow)
    {
        m_bExpProcessFirstRun = true;
        m_bExpProcessUninit = true;
    }


    if (!m_bXwindow)
    {
        {
            system(PHONE_SCRIPTS_PATH "expapp.sh close &");
        }
    }

    return 0;
}



void CEXPLogicColorManager::SetIsXwindow(bool bXwindow)
{
    m_bXwindow = bXwindow;
}

#if IS_EXP_SUPPORT_TRANSPARENCY
void CEXPLogicColorManager::ChangeWidgetTransparency()
{
    yl::string strPath = configParser_GetConfigString(kszCurrentExpWallPaper);
    SetEXPBackGround(commonUnits_Hide2Real(strPath, PHFILE_TYPE_EXP_WALLPAPER_FILE).c_str(), -1);
}
#endif


void CEXPLogicColorManager::ProcessEDKEvent(LedRuleData * pEXPLightList, int nCount)
{
    m_listPageFlushRule.clear();

    for (int i = 0; i < nCount; ++i)
    {
        m_listPageFlushRule.push_back(pEXPLightList[i]);
    }

    PageKeyEDKFlushSwitch(true);
}

void CEXPLogicColorManager::PageKeyEDKFlushSwitch(bool bStart)
{
    DSSKEY_INFO("CEXPLogicManager::PageKeyEDKFlushSwitch bStart %d, m_bPageFlushing %d, m_bPageFlushEnable %d",
                bStart, m_bPageFlushing, m_bPageFlushEnable);

    if (bStart && (m_bPageFlushing || !m_bPageFlushEnable))
    {
        return;
    }

    m_bPageFlushing = bStart;

    if (bStart)
    {
        m_nFlushStep = 0;
        m_nPageFlushTime = 1;
        SetPageFlushTimer();
    }
    else
    {
        DestroyPageFlushTimer();
        m_nFlushStep = 0;
        m_bPageKeyFlushing = false;
    }
}

void CEXPLogicColorManager::PageKeyEDKFlushEnable(bool bEnable)
{
    m_bPageFlushEnable = bEnable;
    PageKeyEDKFlushSwitch(bEnable);
}


void CEXPLogicColorManager::EXPBackLightUpdate(int nLightLevel, int nIndex/* = -1*/)
{
    if (!m_nExpPowerSavingEnable && 0 == nLightLevel)
    {
        return;
    }

    int nCount = exp_GetExpCounts(DT_EXP_50);
    if (0 >= nCount)
    {
        SetExpBKLight(0, nLightLevel);
    }
    else if (-1 == nIndex)
    {
        for (int i = 0; i < nCount; ++i)
        {
            SetExpBKLight(i, nLightLevel);
        }
    }
    else if (nIndex >= 0
             && nIndex < MAX_EXP_NUM)
    {
        SetExpBKLight(nIndex, nLightLevel);
    }
}

void CEXPLogicColorManager::EXPPowerSavingEnable(bool bEnable)
{
    m_nExpPowerSavingEnable = bEnable;
    backlight_UpdateExpLight();
}
#endif