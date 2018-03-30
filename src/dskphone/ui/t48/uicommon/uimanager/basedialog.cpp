#include "basedialog.h"
//mod-UIKernel

// mod-globalInputHook
#include "keymap.h"
#include "globalinputhook/inputhook_include.h"
#include "imagemanager/modimagemanager.h"
#include "uimanager/moduimanager.h"
#include "baseui/t4xpicpath.h"
#include "keyguard/include/modkeyguard.h"
#include "rectdefine.h"

/************************************************************************/
/*                         class CSoftKeyBarData                        */
/************************************************************************/
void CSoftKeyBarData::InitData()
{
    m_bDisplay = true;
    m_strSoftkeyAction = "";
    m_strSoftkeyTitle = "";
    m_strSlotName = "";
    m_pReceiver = NULL;
}

CSoftKeyBarData & CSoftKeyBarData::operator=(const CSoftKeyBarData & other)
{
    // 证同测试
    if (this == &other)
    {
        return *this;
    }

    this->m_bDisplay = other.m_bDisplay;
    this->m_pReceiver = other.m_pReceiver;
    this->m_strSlotName = other.m_strSlotName;
    this->m_strSoftkeyTitle = other.m_strSoftkeyTitle;
    this->m_strSoftkeyAction = other.m_strSoftkeyAction;
    this->m_strIconPath = other.m_strIconPath;

    return *this;
}

/************************************************************************/
/*                         class CBaseDialog                        */
/************************************************************************/
CBaseDialog::CBaseDialog(QWidget * parent)
    : QDialog(parent)
    , m_dlgRect(RECT_FULL_SCREEN)
{
    m_eWndType = eWT_Unknown;
    m_eSoftkeyType = STYLE_TYPE_DIVIDE;
    m_nDlgFlag = 0;
    m_strTitle = "";
    m_strHint = "";
    InitSoftKeyData();
}

CBaseDialog::~CBaseDialog()
{

}

void CBaseDialog::UpdateWnd()
{

}

void CBaseDialog::OnFocusChanged(bool bFocused)
{

}

void CBaseDialog::OnReturnToIdle()
{

}

QRect CBaseDialog::GetDlgGeometry()
{
    return m_dlgRect;
}

void CBaseDialog::SetDlgGeometry(const QRect & rect)
{
    m_dlgRect = rect;
}

void CBaseDialog::SetData(void * pData/* = NULL*/)
{

}

bool CBaseDialog::eventFilter(QObject * pObject, QEvent * pEvent)
{
    return false;
}

void CBaseDialog::InitSoftKeyData()
{
    // 遍历初始化所有的Softkey
    m_SoftkeyData.clear();
    for (int iIndex = 0; iIndex != SOFTKEY_NUM; ++iIndex)
    {
        CSoftKeyBarData objSoftkeyData;
        objSoftkeyData.m_pReceiver = this;
        objSoftkeyData.m_strSlotName = SLOT(OnSoftKeyAction(qSoftButton *));

        m_SoftkeyData.push_back(objSoftkeyData);
    }
}

void CBaseDialog::paintEvent(QPaintEvent *)
{
    return ;
}

void CBaseDialog::OnSoftKeyAction(qSoftButton * pBtn)
{
    if (pBtn != NULL)
    {
//      如果不使用strAction = pBtn->m_strButtonName会导致softkey按下时死机
//      1. 产生该问题的根本原因是执行OnUIAction过程中 该函数的参数strAction（引用）被析构掉了
//      2. OnUIAction中会刷新界面， 刷新界面会重新获取softkey的相关信息， 之前旧的softkey信息会被delete掉、
//      3. 而OnUIAction函数中的参数是 softkey 的action信息的引用
        QString strAction = pBtn->m_strButtonName;
        OnAction(strAction);
    }
}

void CBaseDialog::GetSoftkey(CArraySoftKeyBarData & objSoftkeyData)
{
    objSoftkeyData = m_SoftkeyData;
}

void CBaseDialog::SetSoftKey(const CArraySoftKeyBarData & objSoftkeyData)
{
    m_SoftkeyData.clear();

    m_SoftkeyData = objSoftkeyData;
}

// 是否允许鼠标事件通过
bool CBaseDialog::AllowMouseEventOnKeyLock(QObject * pObject, QMouseEvent * pEvent)
{
    // 键盘锁开启时默认不允许鼠标事件通过
    return false;
    //如果键盘锁要分类型
//  return true;
}

QPixmap CBaseDialog::GetCustomBackGround()
{
    return THEME_GET_BMP(PIC_THEME1_OTHER_BACKGROUND);
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
            if (nKeyCode == PHONE_KEY_VOLUME_DECREASE// 音量调节键允许通过
                    || nKeyCode == PHONE_KEY_VOLUME_INCREASE
                    || nKeyCode == PHONE_KEY_HANDFREE // 允许通话相关的按键通过
                    || nKeyCode == PHONE_KEY_HANDSET
                    || nKeyCode == PHONE_KEY_HANDSET_ON_HOOK
                    || nKeyCode == PHONE_KEY_HANDSET_OFF_HOOK
                    || nKeyCode == PHONE_KEY_HEADSET_CTRL
                    || nKeyCode == PHONE_KEY_EHS_OPEN
                    || nKeyCode == PHONE_KEY_EHS_CLOSE
                    || nKeyCode == PHONE_KEY_STAR
                    || nKeyCode == PHONE_KEY_POUND
                    || nKeyCode == PHONE_KEY_OK
                    || nKeyCode == PHONE_KEY_CANCEL
                    || nKeyCode == PHONE_KEY_BACKSPACE
                    || nKeyCode == Qt::Key_Space
                    || nKeyCode == PHONE_KEY_INPUTMETHOD_CUSTOM_KEYBOARD
                    || (PHONE_KEY_LEFT <= nKeyCode && nKeyCode <= PHONE_KEY_DOWN)
                    || (PHONE_KEY_0 <= nKeyCode && nKeyCode <= PHONE_KEY_9))
                //|| (PHONE_KEY_FN1 <= nKeyCode && nKeyCode <= PHONE_KEY_FN4)
                //|| (PHONE_KEY_LEFT <= nKeyCode && nKeyCode <= PHONE_KEY_DOWN)
                //|| (PHONE_KEY_LINE1 <= nKeyCode && nKeyCode <= PHONE_KEY_LINE10) )
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

// 子界面响应Back按钮的操作
void CBaseDialog::OnBackButtonClick()
{
    // IDLE界面不能被退出,添加容错
    if (!OnAction(ACTION_BACK) && !UIManager_IsInIdle())
    {
        UIManager_PutPrivateDlg(UIManager_GetTopWindow());
    }
}

// 子界面响应Home按钮的操作
void CBaseDialog::OnHomeButtonClick()
{
    UIManager_ReturnToIdle(RETURN_IDLE_TYPE_HOME_PRESS);
}

// 设置软键图标显示类型
void CBaseDialog::SetSoftkeyType(SoftkeyType eIconType)
{
    m_eSoftkeyType = eIconType;
}

// 获取软键图标显示类型
SoftkeyType CBaseDialog::GetSoftkeySoftkeyType()
{
    return m_eSoftkeyType;
}
//仅提供给EDKUI直接跳转功能使用
bool CBaseDialog::DoAction(const QString & strAction)
{
    return OnAction(strAction);
}
