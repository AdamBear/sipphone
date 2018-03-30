#include "settingactiontypemgr.h"
#include "common/common_data_define.h"

#include "dsklog/log.h"



CSettingActionTypeMgr * CSettingActionTypeMgr::m_pInstance = NULL;

CSettingActionTypeMgr * CSettingActionTypeMgr::GetInstance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new CSettingActionTypeMgr();
    }
    return m_pInstance;
}

CSettingActionTypeMgr::CSettingActionTypeMgr()
{
}

int CSettingActionTypeMgr::TransSoftkeyType2SettingActionType(int eSoftkeyType)
{
    int eSettingActionType = -1;
    if (ST_SAVE == eSoftkeyType)
    {
        eSettingActionType = SET_ACTION_TYPE_SAVE;
    }
    else if (ST_BACK == eSoftkeyType)
    {
        eSettingActionType = SET_ACTION_TYPE_BACK;
    }
    else if (ST_ENTER == eSoftkeyType)
    {
        eSettingActionType = SET_ACTION_TYPE_CLICK;
    }
    else if (ST_OPTION == eSoftkeyType)
    {
        eSettingActionType = SET_ACTION_TYPE_OPTION;
    }
    else if (ST_PAGING == eSoftkeyType)
    {
        eSettingActionType = SET_ACTION_TYPE_PAGING;
    }
    else if (ST_EDIT == eSoftkeyType)
    {
        eSettingActionType = SET_ACTION_TYPE_EDIT;
    }
    else if (ST_DELETE == eSoftkeyType)
    {
        eSettingActionType = SET_ACTION_TYPE_DELETE;
    }
    else if (ST_DELETEALL == eSoftkeyType)
    {
        eSettingActionType = SET_ACTION_TYPE_DELETEALL;
    }
    else if (ST_CANCEL == eSoftkeyType)
    {
        eSettingActionType = SET_ACTION_TYPE_CANCLE;
    }
    else if (ST_START == eSoftkeyType)
    {
        eSettingActionType = SET_ACTION_TYPE_START;
    }
    else if (ST_LOGOUT == eSoftkeyType)
    {
        eSettingActionType = SET_ACTION_TYPE_LOGOUT;
    }
    else if (ST_LOGIN == eSoftkeyType)
    {
        eSettingActionType = SET_ACTION_TYPE_LOGIN;
    }
    else if (ST_SKIP == eSoftkeyType)
    {
        eSettingActionType = SET_ACTION_TYPE_SKIP;
    }
    else if (ST_NEXT == eSoftkeyType)
    {
        eSettingActionType = SET_ACTION_TYPE_NEXT;
    }
#ifdef IF_SETTING_WIZARD
    else if (ST_FINISH == eSoftkeyType)
    {
        eSettingActionType = SET_ACTION_TYPE_FINISH;
    }
    else if (ST_PREVIOUS == eSoftkeyType)
    {
        eSettingActionType = SET_ACTION_TYPE_PREVIOUS;
    }
#endif

    return eSettingActionType;
}





