///////////////////////////////////////////////////////////
//  CEdkSoftKeyManager.cpp
//  Implementation of the Class CEdkSoftKeyManager
//  Created on:      21-一月-2015 15:32:26
//  Original author: dlw
///////////////////////////////////////////////////////////

#include "edksoftkeymanager.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "edkdatamanager.h"
#include "adapterbox/include/modadapterbox.h"
#include "idlescreen/include/modidlescreen.h"

#if IF_FEATURE_EDK
CEdkSoftKeyManager::CEdkSoftKeyManager()
{

}

CEdkSoftKeyManager::~CEdkSoftKeyManager()
{

}

bool CEdkSoftKeyManager::GetEdkSoftKey(listEdkSoftKey & listSoftKey)
{
    // idle界面
    if (AdapterBox_IsInIdle())
    {
        _EDKDataMgr.GetSoftKeyDataByUIType(EDK_IDLE, listSoftKey);
    }
    // talk界面
    else if (AdapterBox_IsInTalk())
    {
        SESSION_STATE eSessionState = talklogic_GetSessionStateBySessionID(talklogic_GetFocusedSessionID());
        _EDKDataMgr.GetSoftKeyDataByUIType(MapUIType(eSessionState), listSoftKey);
    }

    return false;
}

// 判断Idle界面是否有其他状态
bool CEdkSoftKeyManager::IsHaveOtherStatus()
{
#ifdef IF_NOTIFICATION_CENTER_ON_IDLE
    IdleStatusItem StateItem;// = idleScreen_GetTopStatus(SNT_TEXT_NOTIFY);
#else
    IdleStatusItem StateItem;// = idleScreen_GetTopStatusNotifyType();
#endif
    if (StateItem.m_nId == PS_STATE_MISSCALL
            || StateItem.m_nId == PS_STATE_TEXTMESSAGE
            || StateItem.m_nId == PS_STATE_CALLFORWARD
            || StateItem.m_nId == PS_STATE_VOICEMAIL
            || StateItem.m_nId == PS_STATE_NETWORK_OK_BUT_CONFLICT)
    {
        return true;
    }

    return false;
}

EDKUITYPE CEdkSoftKeyManager::MapUIType(SESSION_STATE eState)
{
    EDKUITYPE eType = EDK_UI_NON;
    switch (eState)
    {
    case SESSION_RINGBACK:
        eType = EDK_RING_BACK;
        break;
    case SESSION_RINGING:
        eType = EDK_INCOMING_CALL;
        break;
    case SESSION_CONNECTING_TRAN:
        eType = EDK_TRANSFER_CONNECTING;
        break;
    case SESSION_RINGBACK_TRAN:
        eType = EDK_TRANFER_RING_BACK;
        break;
    case SESSION_FINISHED:
        eType = EDK_CALL_FAILED;
        break;
    case SESSION_NETWORKCONF:
    case SESSION_LOCALCONF:
        {
            TALK_STATE eState = talklogic_GetFocusedSessionTalkState();
            switch (eState)
            {
            case TS_TALK:
                eType = EDK_CONFERENCED;
                break;
            case TS_HOLD:
                eType = EDK_TALK_HOLD;
                break;
            case TS_HELD:
                eType = EDK_TALK_HELD;
                break;
            default:
                break;
            }
        }
        break;
    case SESSION_NETWORKCONF_CONNECT:
    case SESSION_BLACONNECTING:
    case SESSION_CONNECTING:
        {
            eType = EDK_CONNECTING;
        }
        break;
    case SESSION_TALKING:
        {
            TALK_STATE eState = talklogic_GetFocusedSessionTalkState();
            switch (eState)
            {
            case TS_TALK:
                eType = EDK_ON_TALK;
                break;
            case TS_HOLD:
                eType = EDK_TALK_HOLD;
                break;
            case TS_HELD:
                eType = EDK_TALK_HELD;
                break;
            default:
                break;
            }
        }
        break;
    case SESSION_DIALING:
    case SESSION_PREDIAL:
    case SESSION_PRETRAN:
        {
            //未输入号码为 EDK_DIALTONE类型
            yl::string strContent = AdapterBox_TalkUIGetInputContent();
            eType = strContent.empty() ? EDK_DIALTONE : EDK_DIALING;
        }
        break;
    default:
        break;
    }

    return eType;
}

void CEdkSoftKeyManager::GetEdkSoftKeyList(YLList<SoftKey_Data> & listSoftKey)
{
    YLList<edk_softkey_data_t> listEdkSoftKey;
    GetEdkSoftKey(listEdkSoftKey);

    if (listEdkSoftKey.size() == 0)
    {
        return;
    }

    SoftKey_Data dataSoftKey;
    edk_softkey_data_t edkSoftKey;
    LISTEDKSOFTKEYITER iter = listEdkSoftKey.begin();
    for (; iter != listEdkSoftKey.end(); iter++)
    {
        edkSoftKey = *iter;
        dataSoftKey.iPos = edkSoftKey.m_iKeyPos;
        dataSoftKey.eType = edkSoftKey.m_eType;
        listSoftKey.push_back(dataSoftKey);
    }
}

#endif
