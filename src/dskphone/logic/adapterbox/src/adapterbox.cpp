#include "adapterbox.h"
#include <ETLLib.hpp>

IMPLEMENT_GETINSTANCE(CAdapterBox)

CAdapterBox::CAdapterBox()
{
    m_FunReturnToIdle = NULL;
    m_FunUpdateTopWnd = NULL;
    m_FunUploadConfig = NULL;
    m_FunUpdateFirmware = NULL;
    m_FunSendKey = NULL;

    m_FunTalkUIClearSessionData = NULL;
    m_FunTalkUISessionExit = NULL;
    m_FunTalkUIGetTalkTimeBySessionId = NULL;
    m_FunTalkUIGetInputContent = NULL;
    m_FunTalkUIUpdateWnd = NULL;
    m_FunTalkUIUpdateErrorInfo = NULL;
    m_FunTalkUIIsInTalk = NULL;

    m_FunSettingUIEnterPage = NULL;

    m_FunDirUIEnterLocalMenuPage = NULL;

    m_FunIsTestMode = NULL;
    m_FunEnterTestMode = NULL;
    m_FunExitTestMode = NULL;

    m_FunXmlBrowserUpdateWnd = NULL;
    m_FunXmlBrowserShowMessage = NULL;
#if IF_FEATURE_XMLBROWSER_HOTELING
    m_FunXmlBrowserShowTips = NULL;
#endif
    m_FunXmlBrowserDestroyMessage = NULL;
    m_FunXmlBrowserExitXBDlg = NULL;
    m_FunXmlBrowserCtrlLight = NULL;

    m_FunShowSCAMenu = NULL;
    m_FunIsExistSCAMenu = NULL;
    m_FunExitSCAMenu = NULL;
#if IF_FEATURE_SHOW_BLF_CALLID
    m_FunShowBLFMenu = NULL;
    m_FunIsExistBLFMenu = NULL;
    m_FunExitBLFMenu = NULL;
#endif
    m_FunExitPickupMenu = NULL;

    m_FunReloadIME = NULL;
    m_FunReboot = NULL;
    m_FunResetFactory = NULL;
    m_FunXmlBrowserUpdate = NULL;

    m_FunVoiceMailCallOut = NULL;
    m_FunVoiceMailGetUnReadNum = NULL;

    m_FunEnterCameraPage = NULL;

    m_FunIsInTalkUIFrame = NULL;
    m_FunIsInIdle = NULL;
    m_FunIsInTalk = NULL;
    m_FunShowEdkPrompt = NULL;

    m_FunUpdateSoftKey = NULL;

    m_FunInsertSoftKey = NULL;

    m_FunIsTopTipUI = NULL;
    m_FunIsTopMessageBoxUI = NULL;
    m_FunIsInWizard = NULL;
    m_FunShowAutopResult = NULL;

    m_FunEnterRPS = NULL;
    m_FunZEROOpenDlg = NULL;
    m_FunCreateMessageBox = NULL;
    m_FunCreateMenuTips = NULL;
    m_FunExitTriggers = NULL;

    m_FunZeroOpenDialog = NULL;
    m_FunZeroEnterPRS = NULL;

    m_FunHideNotifyCenter = NULL;

    m_FunEnterMenu = NULL;
    m_FunExecuteSoftkey = NULL;
    m_FunShowVolumeBar = NULL;

    m_FunExitScreenSaver = NULL;
    m_FunZeroAuthenticationAgain = NULL;

    m_FunImageScale = NULL;
    m_FunShowDsskeyLockPassword = NULL;
    m_FunTalkUIEnterConfWith = NULL;
    m_FunDsskeyLongPress = NULL;
}

CAdapterBox::~CAdapterBox()
{

}
