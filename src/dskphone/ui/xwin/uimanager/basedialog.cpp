#include "basedialog.h"

//#include "keyguard/include/modkeyguard.h"
#include "uimanager/moduimanager.h"
#include "keyguard/include/modkeyguard.h"
#include "voice/include/modvoice.h"
#include "keymap.h"

/************************************************************************/
/*                         class CBaseDialog                        */
/************************************************************************/
CBaseDialog::CBaseDialog(xView * pDialogView, EWindowType eWndType)
    : CDialogViewController(pDialogView)
    , m_eWndType(eWndType)
    , m_strDialogName("")
{
    m_SoftkeyData.clear();
}

CBaseDialog::~CBaseDialog()
{

}

void CBaseDialog::SetDialogName(const yl::string & strDialogName)
{
    m_strDialogName = strDialogName;
}

const yl::string & CBaseDialog::GetDialogName() const
{
    return m_strDialogName;
}

bool CBaseDialog::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax, yl::string & strPix)
{
    return false;
}

bool CBaseDialog::OnSoftkeyAction(int iType, bool bLongPress)
{
    return false;
}

bool CBaseDialog::IsShowDsskey(bool & bExpend)
{
    bExpend = false;
    return m_bDefaultShowDsskey;
}

// 是否允许键盘事件通过
bool CBaseDialog::AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress)
{
    // 键盘锁开启的时候才进行判断
    if (!keyGuard_IsLock())
    {
        return true;
    }

    switch (keyGuard_GetKeyGuardType())
    {
    // menu key 只锁定函数入口不锁定按键
    case KG_MENU_KEY:
        {
            return true;
        }
        break;
    case KG_ALL_KEYS:
    case KG_FUN_KEY:
        {
            if (PHONE_KEY_VOLUME_DECREASE == nKeyCode// 音量调节键允许通过
                    || PHONE_KEY_VOLUME_INCREASE == nKeyCode
                    || voice_IsChannelKey(nKeyCode) // 声道键
                    || PHONE_KEY_STAR == nKeyCode
                    || PHONE_KEY_POUND == nKeyCode
                    || PHONE_KEY_OK == nKeyCode
                    || PHONE_KEY_CANCEL == nKeyCode
                    || (PHONE_KEY_0 <= nKeyCode && nKeyCode <= PHONE_KEY_9)
                    //这里如果不允许softkey通过的话，idle界面上的弹出框的softkey就有可能直接被拦截
                    || (PHONE_KEY_FN1 <= nKeyCode && nKeyCode <= PHONE_KEY_FN4)
                    || (PHONE_KEY_UP == nKeyCode || PHONE_KEY_DOWN == nKeyCode))
            {
                return true;
            }
        }
        break;
    default:
        break;
    }
    // 键盘锁开启时默认不允许其他键盘事件通过
    return false;
}

void CBaseDialog::RefreshSoftkey()
{
    CArraySoftKeyBarData objSoftkeyData;
    GetSoftkey(objSoftkeyData);
    SetSoftKey(objSoftkeyData);
}
