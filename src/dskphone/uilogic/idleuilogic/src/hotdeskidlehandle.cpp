#include "hotdeskidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"

#include "idle/idle_data_define.h"
#include "idlescreen/include/modidlescreen.h"
#include "account/include/modaccount.h"
#include "idleuilogic/include/modidleul.h"

#include "extended/hotdesking/include/modhotdesking.h"

namespace
{
#define HOTDESK_CLEAR_PROMPT_SHOW_TIME      100
typedef YLList<yl::string>              LIST_STRING;
}


CHotdesktHandle::CHotdesktHandle(int nType /*= PS_STATE_NOTE_HOTDESK*/)
    : CBaseIdleHandle(nType)
{

}

CHotdesktHandle::~CHotdesktHandle()
{

}

bool CHotdesktHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    //弹出提示
#if IF_BUG_25349
    popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_IF_CLEAR_ALL_ACCONT_CONFIGRATION);
    yl::string strClearAccount = configParser_GetConfigString(kszHotDeskingClearAccount);
    LIST_STRING listAccountID;
    if (commonAPI_splitStringBySeparator(strClearAccount, ',', listAccountID)
            && listAccountID.size() > 0)
    {
        bool bClearAll = false;
        if (1 == listAccountID.size())
        {
            int iAccountID = atoi(strClearAccount.c_str());
            if (iAccountID > acc_AccountNum() || iAccountID <= 0)
            {
                bClearAll = true;
            }
        }

        if (!bClearAll)
        {
            popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_IF_CLEAR_YOUR_ACCONT_CONFIGRATION);
        }
    }
#else
    popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_IF_CLEAR_ALL_ACCONT_CONFIGRATION);
#endif
    popData.m_vecSoftkey[0] = ST_CANCEL;
    popData.m_vecSoftkey[1] = ST_EMPTY;
    popData.m_vecSoftkey[2] = ST_EMPTY;
    popData.m_vecSoftkey[3] = ST_OK;
    popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL; //messagebox
    return true;
}

bool CHotdesktHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    bool bRet = false;
    unsigned int nNotifyType = GetNeedClearNotifyType();
    if (ST_OK == eSoftkey
            || PHONE_KEY_OK == eKeyCode)
    {
        idleScreen_ClearNotifyTypeFlag(m_iType, nNotifyType);
        idleScreen_DropStatus(m_iType);

        PopupBoxInfo popboxInfo;
        popboxInfo.m_nTimes = HOTDESK_CLEAR_PROMPT_SHOW_TIME;
        popboxInfo.m_iType = IDLE_MSGBOX_TYPE_NOTE; //messagebox弹窗
        popboxInfo.m_strTitle = _UILOGIC_LANG_TRAN(TRID_WARNING);
        popboxInfo.m_strNote = _UILOGIC_LANG_TRAN(TRID_CLEARING_ACCOUNT_PLEASE_WAIT);
        popboxInfo.m_nId = MSGBOX_ID_HOTDESK_WAITING;

        IdleUL_PopupBox(popboxInfo);
        //进入账号配置界面
        hotDesking_ClearUserInfo();
        bRet = true;
    }
    else if (ST_CANCEL == eSoftkey || ST_EXIT == eSoftkey
             || PHONE_KEY_CANCEL == eKeyCode)
    {
        idleScreen_ClearNotifyTypeFlag(m_iType, nNotifyType);
        idleScreen_DropStatus(m_iType);
#if IF_BUG_23550_Pingan_Menu
        //进入账号配置界面
        IdleUL_EnterPageByType(IEPT_HOT_DESK_BASE_DATA);
#endif
        bRet = true;
    }

    return bRet;
}

