#include "cuiregionbase.h"
//#include "xmlwidgetpool.h"
#include "keymap.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "voice/include/modvoice.h"
#include "directoryui/include/moddirectoryui.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "adapterbox/include/modadapterbox.h"
#include "setting_page_define.h"


//#include "voicechannel/include/modvoicechannel.h"
#include "dsskeyui/moddsskeyui.h"
#include "dsskey/include/moddsskey.h"
#include "dsskey/src/dsskeyactionmanager.h"
#include "dsskey/src/dsskeyaction.h"
#include "xmlbrowseruicommon.h"

#if XML_BROWSER_SUPPORT_UI

namespace
{
#define LINEKEY_COUNT   27
}

CUIRegionBase::CUIRegionBase()
    : m_bLock(false)
    , m_eType(XMT_UNKNOWN)
    , m_iQuitTime(-1)
    , m_iElementID(-1)
{

}

CUIRegionBase::~CUIRegionBase()
{

}

// 处理UI的Action事件
void CUIRegionBase::OnXMLUIAction(XB_SOFTKEY_TYPE eType)
{
    // to do
}

bool CUIRegionBase::IsSoftkey(int nKey)
{
    return (nKey >= PHONE_KEY_FN1 && nKey <= PHONE_KEY_FN4);
}

// 存在通话时,XMLBroswser需要处理的按键
bool CUIRegionBase::IsProcessInTalk(int nKey)
{
    return (IsSoftkey(nKey)
            || nKey == PHONE_KEY_LEFT || nKey == PHONE_KEY_RIGHT
            || nKey == PHONE_KEY_UP || nKey == PHONE_KEY_DOWN
            || nKey == PHONE_KEY_OK
            || (nKey >= PHONE_KEY_0 && nKey <= PHONE_KEY_POUND));
}

bool CUIRegionBase::GetPageData(MapPageData & PageData)
{
    return true;
}

// 设置页面的softkey数据
void CUIRegionBase::SetSoftkey(const VecSoftkeyParam & softkeyParam)
{
    if (0 == softkeyParam.size())
    {
        SetDefaultSoftkey(m_vecSoftkey);
    }
    else
    {
        m_vecSoftkey = softkeyParam;
    }
}

VecSoftkeyParam & CUIRegionBase::GetSoftkey()
{
    return m_vecSoftkey;
}

void CUIRegionBase::SetDefaultSoftkey(VecSoftkeyParam & softkeyParam)
{
    softkeyParam.clear();

    softkeyParam.push_back(TypeToSoftInfo(XB_ST_EXIT));
}

// 设置ElementID
void CUIRegionBase::SetElementID(int iID)
{
    m_iElementID = iID;
}

// 获取ElementID
int CUIRegionBase::GetElementID()
{
    return m_iElementID;
}

bool CUIRegionBase::ProcessKey(int nKey)
{
    // 先判断按的是不是Dsskey
    int iDsskeyID = dsskeyUI_GetDsskeyIDByKeycode(nKey);
    if (iDsskeyID >= 0
            && iDsskeyID < LINEKEY_COUNT)
    {
        // 处理判断放到逻辑层去做
        XDataUI2Logic objData;
        objData.wActionParam = iDsskeyID;
        objData.lActionParam = GetCurrentItemIndex();
        return xmlbrowser_OnAction(GetElementID(), XB_ACTION_LINEKEY_CALLOUT, (void *)&objData);
    }

    //通道模块处理
    voice_OnKeyPress(nKey);

    switch (nKey)
    {
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            if (!m_bLock)
            {
                xmlbrowser_OnAction(GetElementID(), XB_ACTION_EXIT_ALL);
            }

            return true;
        }
        break;
    case PHONE_KEY_HANDSET_OFF_HOOK:
        {
            int iCurrentItem = GetCurrentItemIndex();
            if (!m_bLock
                    || XMT_DIRECTORY == m_eType
                    || XMT_MENU == m_eType)
            {
                return xmlbrowser_OnAction(GetElementID(), XB_ACTION_HOOK_OFF, (void *)&iCurrentItem);
            }
            else
            {
                return true;
            }
        }
        break;
    case PHONE_KEY_HANDFREE:
        {
            int iCurrentItem = GetCurrentItemIndex();
            if (!m_bLock)
            {
                return xmlbrowser_OnAction(GetElementID(), XB_ACTION_PRESS_HANDFREE, (void *)&iCurrentItem);
            }
            else
            {
                return true;
            }
        }
        break;
    case PHONE_KEY_STAR:
    case PHONE_KEY_POUND:
        {
            if (XMT_DIRECTORY == m_eType)
            {
                int iCurrentItem = GetCurrentItemIndex();
                xmlbrowser_OnAction(GetElementID(), XB_ACTION_PRESS_STAR_AND_POUND, (void *)&iCurrentItem);
                return true;
            }

            return false;
        }
        break;
    case PHONE_KEY_CANCEL:
        {
            //模拟按下Exit的效果
            xmlbrowser_OnAction(GetElementID(), XB_ACTION_CANCEL);
            return true;
        }
        break;
    case PHONE_KEY_MESSAGE:
        {
            if (!m_bLock)
            {
                SettingUILogic_ConnectVoiceMail();
            }
        }
        break;
    case PHONE_KEY_REDIAL:
        {
            if (!m_bLock)
            {
                // 进入本地的Call
                DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL);
            }
        }
        break;
    case PHONE_KEY_HEADSET_CTRL:
        {
            if (!m_bLock)
            {
//              voice_OnKeyPress(nKey);
            }
        }
        break;
    case PHONE_KEY_TRANSFER:
        {
            if (!m_bLock)
            {
                AdapterBox_SettingUIEnterPage(MENU_PAGE_ALWAYS_FORWARD);
            }
        }
        break;
    case PHONE_KEY_OK:
        {
            int iCurrentItem = GetCurrentItemIndex();
            xmlbrowser_OnAction(GetElementID(), XB_ACTION_CONFIRM, (void *)&iCurrentItem);
        }
        break;
    default:
        break;
    }

    return false;
}

// 处理按键的事件
bool CUIRegionBase::OnKeyPress(int nKey)
{
    // softkey键由上层处理
    if (IsSoftkey(nKey))
    {
        return false;
    }
    // 锁定键盘,则屏蔽其他按键
    if (m_bLock && !IsIgnoredLockKey(nKey))
    {
        return true;
    }

    return ProcessKey(nKey);
}

// 是否是在界面锁上时忽略的功能键
BOOL CUIRegionBase::IsIgnoredLockKey(int iKey)
{
    return (PHONE_KEY_OK == iKey
            || PHONE_KEY_UP == iKey
            || PHONE_KEY_LEFT == iKey
            || PHONE_KEY_RIGHT == iKey
            || PHONE_KEY_DOWN == iKey
            || PHONE_KEY_CANCEL == iKey
            || PHONE_KEY_HANDSET_OFF_HOOK == iKey
            || PHONE_KEY_HANDFREE == iKey
            || (iKey >= PHONE_KEY_LINE1
                && iKey <= PHONE_KEY_LINE10));
}

bool CUIRegionBase::RegionBaseKeypadHook(int nKeyCode, bool bPress)
{
    if (!XmlBrowserUI_IsPopuping())
    {
        return false;
    }


    //键盘被锁时，重拨，免提，摘机操作不需要响应，这里要勾住消息传递
    if (bPress/* && m_bLock*/
            && (PHONE_KEY_REDIAL == nKeyCode
                || PHONE_KEY_HANDSET_OFF_HOOK == nKeyCode
                || PHONE_KEY_HANDSET_ON_HOOK == nKeyCode
                || PHONE_KEY_HANDFREE == nKeyCode
                || PHONE_KEY_MESSAGE == nKeyCode))
    {
        if ((PHONE_KEY_REDIAL == nKeyCode
                || PHONE_KEY_MESSAGE == nKeyCode)
                && m_bLock)
        {
            return true;
        }
        //键盘的过滤对应到各个类型去处理
        return ProcessKey(nKeyCode);
    }

    if (bPress && OnKeyPress(nKeyCode))
    {
        return true;
    }

    return false;
}
#endif
