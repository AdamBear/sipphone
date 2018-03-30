#include "cuiregionbase.h"
#include "xmlwidgetpool.h"
#include "keymap.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "directoryui/include/moddirectoryui.h"
#include "setting/include/modsetting.h"
#include "adapterbox/include/modadapterbox.h"
#include <setting_page_define.h>
// #include "VoiceChannel/modVoiceChannel.h"
#include "dsskeyui/include/moddsskeyui.h"
#include "dsskey/include/moddsskey.h"
#include "uicommon.h"
#include <QKeyEvent>
#include "xmlbrowseruimanager.h"
#include "voice/include/modvoice.h"

namespace
{
#define LINEKEY_COUNT   29
}

// mod-this
#include "uicommon.h"

CUIRegionBase::CUIRegionBase(QWidget * parent/* = 0*/, Qt::WFlags flags/* = 0*/)
    : m_bLock(false)
    , m_eType(XMT_UNKNOWN)
    , m_iQuitTime(-1)
    , m_iElementID(-1)
{
    setObjectName("UIRegionBase");
}

CUIRegionBase::~CUIRegionBase()
{
    // 释放所使用控件
    g_pXMLWidgetPool->PutChildrenWidget(this);
}

// 处理UI的Action事件
bool CUIRegionBase::OnXMLUIAction(XB_SOFTKEY_TYPE eType)
{
    return false;
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
void CUIRegionBase::SetSoftkey(VecSoftkeyParam & softkeyParam)
{
    if (softkeyParam.size() == 0)
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

//  softkeyParam.push_back(TypeToSoftInfo(XB_ST_EXIT));
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
    int iLineKeyIndex = dsskeyUI_GetDsskeyIDByKeycode(nKey);

#ifndef _T48
    CDssKey * pDsskeyInfo = dsskey_GetDsskeyAction(iLineKeyIndex);
    if (NULL != pDsskeyInfo)
    {
        DssKey_Type eDsskeyType = pDsskeyInfo->GetDsskeyType();
        if (eDsskeyType != DT_LINE)
        {
            if (m_bLock)
            {
                return true;
            }
            XMLBROWSERUI_INFO("Dsskey Press");
            if (eDsskeyType != DT_XMLBROWSER)
            {
                _XMLBrowserManager.Exit();
            }
            pDsskeyInfo->OnKeyPress();
            return true;
        }
    }
#endif

    if (-1 != iLineKeyIndex && iLineKeyIndex < LINEKEY_COUNT)
    {
        XMLBROWSERUI_INFO("Linekey Press");
        XDataUI2Logic objData;
        objData.wActionParam = iLineKeyIndex;
        objData.lActionParam = GetCurrentItem();
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
            int iCurrentItem = GetCurrentItem();
            if (!m_bLock
                    || m_eType == XMT_DIRECTORY
                    || m_eType == XMT_MENU)
            {
                //voice_OnChannelKeyPress(nKey);
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
            int iCurrentItem = GetCurrentItem();
            if (!m_bLock)
            {
                //voice_OnChannelKeyPress(nKey);
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
            return false;
        }
        break;
    case PHONE_KEY_CANCEL:
        {
            // 模拟按下Exit的效果
            xmlbrowser_OnAction(GetElementID(), XB_ACTION_CANCEL);
            return true;
        }
        break;
    case PHONE_KEY_MESSAGE:
        {
            if (!m_bLock)
            {
                Setting_ConnectVoiceMail();
            }
        }
        break;
    case PHONE_KEY_REDIAL:
        {
            if (!m_bLock)
            {
                int iCurrentItem = GetCurrentItem();
                return xmlbrowser_OnAction(GetElementID(), XB_ACTION_CONFIRM, (void *)&iCurrentItem);
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
    default:
        break;
    }

    return false;
}

// 处理按键的事件
bool CUIRegionBase::OnKeyPress(QObject * pObj, QEvent * pEvent)
{
    if (pObj == NULL || pEvent == NULL)
    {
        return false;
    }

    QKeyEvent * keyEvent = (QKeyEvent *)pEvent;
    int nKey = keyEvent->key();
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

    // 如果存在通话，则只处理softkey、方向键、退出键、数字键
    if (talklogic_SessionExist())
    {
        if (!IsProcessInTalk(nKey))
        {
            ////----to do
            // 其他按键都退出，这里用ReturnToDesktop，保证了XML在Talk收到按键前退出
            //ReturnToDesktop();

            // 退出XML界面后将按键交给Talk当前UI处理
            //return _objCallManager.OnKeyToTalk(pObj, pEvent);???
        }
    }

    return ProcessKey(nKey);
}


bool CUIRegionBase::RegionBaseKeypadHook(int nKeyCode, bool bPress)
{
    //键盘被锁时，重拨，免提，摘机操作不需要响应，这里要勾住消息传递
    if (bPress /*&& m_bLock*/
            && (nKeyCode == PHONE_KEY_REDIAL
                || nKeyCode == PHONE_KEY_HANDSET_OFF_HOOK
                || nKeyCode == PHONE_KEY_HANDSET_ON_HOOK
                || nKeyCode == PHONE_KEY_HANDFREE))
    {
        if (nKeyCode == PHONE_KEY_REDIAL && m_bLock)
        {
            return true;
        }
        //键盘的过滤对应到各个类型去处理
        return ProcessKey(nKeyCode);
    }

//  if (bPress && !m_bLock
//      && (nKeyCode == PHONE_KEY_HANDSET_OFF_HOOK
//      || nKeyCode == PHONE_KEY_HANDSET_ON_HOOK
//      || nKeyCode == PHONE_KEY_HANDFREE
//      || nKeyCode == PHONE_KEY_POUND
//      || nKeyCode == PHONE_KEY_STAR
//      || nKeyCode == PHONE_KEY_CANCEL))
//  {
//      return ProcessKey(nKeyCode);
//  }
    return false;
}

bool CUIRegionBase::ProcessDsskey(int iIndex)
{
    // 先判断按的是不是Dsskey

#ifndef _T48
    int iLineKeyIndex = dsskey_GetLinekeyIndex(iIndex);
    CDssKey * pDsskeyInfo = dsskey_GetDsskeyAction(iIndex);
    if (NULL != pDsskeyInfo)
    {
        DssKey_Type eDsskeyType = pDsskeyInfo->GetDsskeyType();
        if (eDsskeyType != DT_LINE)
        {
            if (m_bLock)
            {
                return true;
            }
            XMLBROWSERUI_INFO("Dsskey Press");
            if (eDsskeyType != DT_XMLBROWSER)
            {
                _XMLBrowserManager.Exit();
            }
            pDsskeyInfo->OnKeyPress();
            return true;
        }
        else
        {
            XMLBROWSERUI_INFO("Linekey Press");
            XDataUI2Logic objData;
            objData.wActionParam = iIndex;
            objData.lActionParam = GetCurrentItem();
            xmlbrowser_OnAction(GetElementID(), XB_ACTION_LINEKEY_CALLOUT, (void *)&objData);
            return true;
        }
    }
#endif

    // 先判断按的是不是Dsskey
    int iDsskeyID = dsskey_GetDsskeyID(DMT_LINEKEY_MODULE, 0, iIndex);
    if (iDsskeyID >= 0
            && iDsskeyID < LINEKEY_COUNT)
    {
        XMLBROWSERUI_INFO("Linekey Press");
        // 处理判断放到逻辑层去做
        XDataUI2Logic objData;
        objData.wActionParam = iDsskeyID;
        objData.lActionParam = GetCurrentItem();
        return xmlbrowser_OnAction(GetElementID(), XB_ACTION_LINEKEY_CALLOUT, (void *)&objData);
    }

    return false;
}

// 是否是在界面锁上时忽略的功能键
BOOL CUIRegionBase::IsIgnoredLockKey(int iKey)
{
    return (/*iKey == T28_KEY_HEADSET
        ||*/ iKey == PHONE_KEY_OK
               || iKey == PHONE_KEY_UP
               || iKey == PHONE_KEY_LEFT
               || iKey == PHONE_KEY_RIGHT
               || iKey == PHONE_KEY_DOWN
               || iKey == PHONE_KEY_CANCEL
               || iKey == PHONE_KEY_HANDSET_OFF_HOOK
               || iKey == PHONE_KEY_HANDFREE
               || (iKey >= PHONE_KEY_LINE1
                   && iKey <= PHONE_KEY_LINE10));
}
