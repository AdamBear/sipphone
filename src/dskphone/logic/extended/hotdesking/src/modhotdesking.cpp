#include "hotdesking_inc.h"
#include "adapterbox/include/modadapterbox.h"
#include "dsskeyhotdeskingaction.h"

#ifdef IF_FEATURE_HOTDESKING
// UI给Logic发送UI事件
void hotDesking_Enter(LPCSTR lpURL /*= NULL*/, int iKeyCode /*= -1*/)
{
    if (NULL == lpURL)
    {
        _HotDesking.StartLocalHotDesking();
    }
#ifdef IF_ADVANCE_HOTDESKING
    else
    {
        _HotDesking.StartRemoteHotDesking(lpURL, iKeyCode);
    }
#endif  //IF_ADVANCE_HOTDESKING
}

void hotDesking_Menu_Enter()
{
    if (!talklogic_SessionExist())
    {
        // 先退出其他界面
#if IF_BUG_23550_Pingan_Menu
#else
        AdapterBox_ReturnToIdle();
#endif
#ifdef IF_ADVANCE_HOTDESKING
        yl::string strURL = configParser_GetConfigString(kszHotDeskingURL);
        if (strURL.empty())
        {
            hotDesking_Enter(NULL, -1);
        }
        else
        {
            hotDesking_Enter(strURL.c_str(), -1);
        }
#else
        hotDesking_Enter(NULL, -1);
#endif//IF_ADVANCE_HOTDESKING   
    }
}

#ifdef IF_ADVANCE_HOTDESKING
// 判断是否处于高级hotdesking模式
BOOL hotDesking_IsAdvMode()
{
    return _HotDesking.IsAdvMode();
}

/*******************************************************************
** 函数描述:   更改Hotdesking当前状态
********************************************************************/
VOID hotDesking_SetAdvMode(BOOL bIsAdvMode)
{
    _HotDesking.SetAdvMode(bIsAdvMode);
}

/*******************************************************************
** 函数描述:   判断是否使用HotDesking的softkey
**
** 返回:       TRUE:使用hotdesking的softkey,否则不使用
********************************************************************/
BOOL hotDesking_UseHotDeskingSoftKey()
{
    return (_HotDesking.IsAdvMode() && _HotDeskingSoftKey.GetKeyNum() > 0);
}

/*******************************************************************
** 函数描述:   判断是否处于高级hotdesking模式
**            [in] 按键值
**            [out] 按键对应的名字
** 返回:       无
********************************************************************/
void hotDesking_GetHotDeskingkeyName(int iKeyIndex, yl::string & strKeyName)
{
    int iKey = iKeyIndex - PHONE_KEY_FN1;
    if (iKey < 0)
    {
        return;
    }

    _HotDeskingSoftKey.GetSoftKeyTextByIndex(iKey, strKeyName);
}

// 高级模式下的hotdesking按键处理
BOOL hotDesking_OnSoftKey(int iKeyIndex)
{
    _HotDeskingSoftKey.On_FuncKeyPress(iKeyIndex);
    return TRUE;
}

/*******************************************************************
** 函数描述:   HotDeskingSoftkey对sip消息的响应
********************************************************************/
BOOL hotDesking_OnSipMessage(msgObject & objMessage)
{
    return _HotDeskingSoftKey.OnSIPMessage(objMessage);
}
#endif  //IF_ADVANCE_HOTDESKING

/*******************************************************************
** 函数描述:        hotdesking初始化
********************************************************************/
VOID hotDesking_Init()
{
    new CDsskeyHotDeskingAction();

    _HotDesking.Init();
}

/*******************************************************************
** 函数描述:        检查是否进入hotdesking login wizard
********************************************************************/
BOOL hotDesking_EnterLoginWizard()
{
    return _HotDesking.EnterLoginWizard();
}

/*******************************************************************
** 函数描述:        注册向导是否生效
********************************************************************/
BOOL hotDesking_IsLogonWizard()
{
    return _HotDesking.IsLogonWizard();
}

VOID hotDesking_ClearUserInfo()
{
    _HotDesking.ClearUserInfo();
}

#else//IF_FEATURE_HOTDESKING

// UI给Logic发送UI事件
void hotDesking_Enter(LPCSTR lpURL /*= NULL*/, int iKeyCode /*= -1*/)
{

}

void hotDesking_Menu_Enter()
{

}

#ifdef IF_ADVANCE_HOTDESKING
// 判断是否处于高级hotdesking模式
BOOL hotDesking_IsAdvMode()
{
    return FALSE;
}

/*******************************************************************
** 函数描述:   更改Hotdesking当前状态
********************************************************************/
VOID hotDesking_SetAdvMode(BOOL bIsAdvMode)
{

}

/*******************************************************************
** 函数描述:   判断是否使用HotDesking的softkey
**
** 返回:       TRUE:使用hotdesking的softkey,否则不使用
********************************************************************/
BOOL hotDesking_UseHotDeskingSoftKey()
{
    return FALSE;
}

/*******************************************************************
** 函数描述:   判断是否处于高级hotdesking模式
**            [in] 按键值
**            [out] 按键对应的名字
** 返回:       无
********************************************************************/
void hotDesking_GetHotDeskingkeyName(int iKeyIndex, yl::string & strKeyName)
{

}

// 高级模式下的hotdesking按键处理
BOOL hotDesking_OnSoftKey(int iKeyIndex)
{
    return FALSE;
}

/*******************************************************************
** 函数描述:   HotDeskingSoftkey对sip消息的响应
********************************************************************/
BOOL hotDesking_OnSipMessage(msgObject & objMessage)
{
    return FALSE;
}
#endif  //IF_ADVANCE_HOTDESKING

/*******************************************************************
** 函数描述:        hotdesking初始化
********************************************************************/
VOID hotDesking_Init()
{

}

/*******************************************************************
** 函数描述:        注册向导是否生效
********************************************************************/
BOOL hotDesking_EnterLoginWizard()
{
    return FALSE;
}

/*******************************************************************
** 函数描述:        注册向导是否生效
********************************************************************/
BOOL hotDesking_IsLogonWizard()
{
    return FALSE;
}

VOID hotDesking_ClearUserInfo()
{
}
#endif // #ifdef IF_FEATURE_HOTDESKING
