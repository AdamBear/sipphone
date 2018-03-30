#include "edkkeyaction.h"
#include "edkactionmanager.h"
#include "commonunits/modcommonunits.h"
#include "adapterbox/include/modadapterbox.h"
#include "talk/talklogic/include/modtalklogic.h"
#include <setting_page_define.h>
#include "dsskey/include/moddsskey.h"
#include "exp/include/modexp.h"
#include "exp/src/expdrive/deviceplugin/modexpscan.h"

#if IF_FEATURE_EDK

CKeyAction::CKeyAction()
    : CEdkActionBase(EDK_EXCUTEKEY)
{
    YLList<int> listOutput;
    dsskey_getAllDsskeys(listOutput, DMT_LINEKEY_MODULE);

    int iIndex = 1;
    char strKeyId[30] = {0};
    for (YLList<int>::ListIterator iter = listOutput.begin();
            iter != listOutput.end();
            iter ++)
    {
        sprintf(strKeyId, "LineKey%d", iIndex);
        m_mapKeyStringToId.insert(strKeyId, *iter);
        iIndex ++;
    }
}

CKeyAction::~CKeyAction()
{
    m_mapKeyStringToId.clear();
}

bool CKeyAction::DoAction(int iActionId, edk_action_data_t & tEdkAction, bool & bResult)
{
    bool bHandle = false;
    switch (tEdkAction.m_eActionType)
    {
    case EDK_SOFTKEY:
        {
            bResult = ExcuteSoftKey(tEdkAction);
            bHandle = bResult;
        }
        break;
    case EDK_HARDKEY:
        {
            bHandle = ExcuteHardKey(iActionId, tEdkAction, bResult);
        }
        break;
    default:
        break;
    }

    return bHandle;
}

/*
* 执行硬按键
* 返回值: 是否继续执行action
* bResult: 执行结果
*/
bool CKeyAction::ExcuteHardKey(int iActionId, edk_action_data_t & tEdkAction, bool & bResult)
{
    EDK_INFO("CKeyAction::ExcuteHardKey ---- [id:%d]Action[%s]", iActionId,
             tEdkAction.m_strAction.c_str());

    // 按键字符和按键键值的对应
    struct EDKKey
    {
        const char * lpszKey;
        int  iMsg;
        int iKey;
    };

    // 硬按键定义,Linekey、Ext没有在该表
    static EDKKey mapKey[] =
    {
        { "ArrowUp",        DEV_MSG_FUNKEY_EVENT, PHONE_KEY_UP },
        { "ArrowDown",      DEV_MSG_FUNKEY_EVENT, PHONE_KEY_DOWN },
        { "ArrowLeft",      DEV_MSG_FUNKEY_EVENT, PHONE_KEY_LEFT },
        { "ArrowRight",     DEV_MSG_FUNKEY_EVENT, PHONE_KEY_RIGHT },
        { "VolUp",          DEV_MSG_FUNKEY_EVENT, PHONE_KEY_VOLUME_INCREASE },
        { "VolDown",        DEV_MSG_FUNKEY_EVENT, PHONE_KEY_VOLUME_DECREASE },
        { "Cancel",         DEV_MSG_FUNKEY_EVENT, PHONE_KEY_CANCEL },
        { "OK",             DEV_MSG_FUNKEY_EVENT, PHONE_KEY_OK },
        { "DialPad0",       DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_0 },
        { "DialPad1",       DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_1 },
        { "DialPad2",       DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_2 },
        { "DialPad3",       DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_3 },
        { "DialPad4",       DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_4 },
        { "DialPad5",       DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_5 },
        { "DialPad6",       DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_6 },
        { "DialPad7",       DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_7 },
        { "DialPad8",       DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_8 },
        { "DialPad9",       DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_9 },
        { "DialPadPound",   DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_POUND },
        { "DialPadStar",    DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_STAR },
        { "Headset",        DEV_MSG_FUNKEY_EVENT, PHONE_KEY_HEADSET_CTRL },
        { "Mute",           DEV_MSG_FUNKEY_EVENT, PHONE_KEY_MUTE },
        { "Message",        DEV_MSG_FUNKEY_EVENT, PHONE_KEY_MESSAGE },
        { "Hold",           DEV_MSG_FUNKEY_EVENT, PHONE_KEY_HOLD },
        { "Redial",         DEV_MSG_FUNKEY_EVENT, PHONE_KEY_REDIAL },
        { "Transfer",       DEV_MSG_FUNKEY_EVENT, PHONE_KEY_TRANSFER },
        { "Speaker",        DEV_MSG_FUNKEY_EVENT, PHONE_KEY_HANDFREE },
        { "Conference",     DEV_MSG_FUNKEY_EVENT, PHONE_KEY_CONFERENCE },
        { "SoftKey1",       DEV_MSG_FUNKEY_EVENT, PHONE_KEY_FN1 },
        { "SoftKey2",       DEV_MSG_FUNKEY_EVENT, PHONE_KEY_FN2 },
        { "SoftKey3",       DEV_MSG_FUNKEY_EVENT, PHONE_KEY_FN3 },
        { "SoftKey4",       DEV_MSG_FUNKEY_EVENT, PHONE_KEY_FN4 },
        // 模拟两个不存在的硬按键home、menu, 任何界面都能够执行(不能响应的界面除外,如通话)
        { "Home",           DEV_MSG_FUNKEY_EVENT, 0},
        { "Menu",           DEV_MSG_FUNKEY_EVENT, 0},
        { "",               -1, -1 } // last one
    };

    // 从数组中找对应的键值
    int i = 0;
    while (mapKey[i].iKey != -1)
    {
        if (tEdkAction.m_strAction == mapKey[i].lpszKey)
        {
            if (tEdkAction.m_strAction == "Home"
                    /*&& !talklogic_SessionExist()*/)
            {
                AdapterBox_ReturnToIdle(true);
            }
            else if (tEdkAction.m_strAction == "Menu"
                     && !talklogic_SessionExist())
            {
                AdapterBox_ReturnToIdle();
                AdapterBox_SettingUIEnterPage(MENU_PAGE_MAIN);
            }
            else
            {
                // http://10.2.1.199/Bug.php?BugID=86864
                // 音量键较特殊,按下dsskey会清除volume bar,所以发消息前需设置一下volume bar
                HandleVolKey(mapKey[i].lpszKey);
                // 模拟按键按下
                commonUnits_SendSimulateKey(mapKey[i].iMsg, mapKey[i].iKey);
                SetTimer(iActionId);
                return false;
            }
            AdapterBox_UpdateTopWnd();

            return true;
        }

        i ++;
    }

    // 硬按键表没有找到,则在linekey表查找
    int iDsskeyID = -1;
    if ((iDsskeyID = GetDsskeyIDByAction(tEdkAction.m_strAction)) != -1)
    {
        bResult = dsskey_OnDsskeyClick(iDsskeyID);
        return bResult;
    }

    return false;
}

/*
* 执行softkey
* 返回值: 执行结果
*/
bool CKeyAction::ExcuteSoftKey(edk_action_data_t & tEdkAction)
{
    EDK_INFO("CKeyAction::ExcuteSoftKey ---- Action[%s]", tEdkAction.m_strAction.c_str());

    return AdapterBox_ExecuteSoftkey(tEdkAction.m_strAction);
}

void CKeyAction::SetTimer(int iActionId)
{
    _EdkActionManager.SetTimer(true, iActionId);
}

/*
* 通过action获取dsskey id
*/
int CKeyAction::GetDsskeyIDByAction(const yl::string & strKey)
{
    if (strKey.empty())
    {
        return -1;
    }

    // Get line key id
    ITER_STRING_TO_ID_MAP iter = m_mapKeyStringToId.find(strKey);
    if (iter != m_mapKeyStringToId.end())
    {
        return iter->second;
    }

    // Get Ext id
    int iExp = -100;    // 扩展台序号
    int iKey = -100;    // 按键序号
    // Ext命令格式:Extx@y [x]:按键序号  [y]:扩展台序号, y为1时，@y可以省略
    if (sscanf(strKey.c_str(), "Ext%d@%d", &iKey, &iExp) == 2
            || sscanf(strKey.c_str(), "Ext%d", &iKey) == 1)
    {
        if (-100 == iExp)
        {
            iExp = 1;
        }

        DEVICE_TYPE_DEF eType = exp_GetExpTypeByIndex(iExp - 1);
        if (iExp < 1 || iExp > MAX_EXP_NUM
                || iKey < 1 || iKey > exp_GetExpKeyNumByType(eType))
        {
            EDK_INFO("Ext is unavailable.Ext NO.[%d], key[%d]", iExp, iKey);
            return -1;
        }

        // 代码中的index都是从0开始，配置中从1开始
        DSSKEY_MODULE_TYPE moduleType = exp_GetEXPModuleType();
        int iDsskeyID = -1;
        if ((iDsskeyID = dsskey_GetDsskeyID(moduleType, iExp - 1, iKey - 1)) != -1)
        {
            EDK_INFO("Ext NO.[%d], key[%d], dsskey id[%d].", iExp, iKey);
            return iDsskeyID;
        }
    }

    EDK_INFO("not found dsskey.");
    return -1;
}

bool CKeyAction::HandleVolKey(const yl::string & strKey)
{
    if (strKey == "VolUp" || strKey == "VolDown")
    {
        if (AdapterBox_IsInIdle()
                || AdapterBox_IsInTalk())
        {
            AdapterBox_ShowVolumeBar(true);
        }
    }

    return true;
}

#endif
