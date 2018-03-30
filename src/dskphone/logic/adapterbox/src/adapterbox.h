#ifndef __ADAPTERBOX_H__
#define __ADAPTERBOX_H__

#include "singletonclassdefine.h"
#include "adapterboxdefine.h"

class CAdapterBox
{
    DECLARE_SINGLETON_CLASS(CAdapterBox)

public:
    FunReturnToIdle             m_FunReturnToIdle;
    FunUpdateTopWnd             m_FunUpdateTopWnd;
    FunSendKey                  m_FunSendKey;

    FunTalkUIClearSessionData   m_FunTalkUIClearSessionData;
    FunTalkUISessionExit        m_FunTalkUISessionExit;
    FunTalkUIGetTalkTimeBySessionId m_FunTalkUIGetTalkTimeBySessionId;
    FunTalkUIGetInputContent        m_FunTalkUIGetInputContent;
    FunTalkUIGetInputContentEx      m_FunTalkUIGetInputContentEx;
    FunTalkUIGetInputCallID      m_FunTalkUIGetInputCallID;
    FunTalkUIGetAlreadyInputContent m_FunTalkUIGetAlreadyInputContent;
    FunTalkUIUpdateWnd              m_FunTalkUIUpdateWnd;
    FunTalkUIUpdateErrorInfo        m_FunTalkUIUpdateErrorInfo;
    FunTalkUIIsInTalk               m_FunTalkUIIsInTalk;

    FunSettingUIEnterPage           m_FunSettingUIEnterPage;

    FunDirUIEnterLocalMenuPage               m_FunDirUIEnterLocalMenuPage;

    FunIsTestMode                   m_FunIsTestMode;
    FunEnterTestMode                m_FunEnterTestMode;
    FunExitTestMode                 m_FunExitTestMode;

    FunXmlBrowserUpdateWnd          m_FunXmlBrowserUpdateWnd;
    FunXmlBrowserShowMessage        m_FunXmlBrowserShowMessage;
#if IF_FEATURE_XMLBROWSER_HOTELING
    FunXmlBrowserShowTips           m_FunXmlBrowserShowTips;
#endif
    FunXmlBrowserDestroyMessage     m_FunXmlBrowserDestroyMessage;
    FunXmlBrowserExitXBDlg          m_FunXmlBrowserExitXBDlg;
    FunXmlBrowserCtrlLight          m_FunXmlBrowserCtrlLight;

    FunShowSCAMenu                  m_FunShowSCAMenu;
    FunIsExistSCAMenu               m_FunIsExistSCAMenu;
    FunExitSCAMenu                  m_FunExitSCAMenu;
#if IF_FEATURE_SHOW_BLF_CALLID
    FunShowBLFMenu                  m_FunShowBLFMenu;
    FunIsExistBLFMenu               m_FunIsExistBLFMenu;
    FunExitBLFMenu                  m_FunExitBLFMenu;
#endif
    FunExitPickupMenu               m_FunExitPickupMenu;
    FunEnterConnectVoiceMailPage    m_FunEnterConnectVoiceMailPage;

    FunReloadIME                    m_FunReloadIME;

    FunReboot                       m_FunReboot;
    FunResetFactory                 m_FunResetFactory;
    FunXmlBrowserUpdate             m_FunXmlBrowserUpdate;
    FunIsInIdle                     m_FunIsInIdle;
    FunVoiceMailCallOut             m_FunVoiceMailCallOut;
    FunVoiceMailGetUnReadNum        m_FunVoiceMailGetUnReadNum;

    FunGetMutePicture               m_FunGetMutePicture;

    FunEnterCameraPage              m_FunEnterCameraPage;

    FunIsInTalkUIFrame              m_FunIsInTalkUIFrame;
    FunZeroOpenDialog               m_FunZeroOpenDialog;
    FunZeroEnterPRS                 m_FunZeroEnterPRS;
    FunIsInTalk                     m_FunIsInTalk;

    FunShowEdkPrompt                m_FunShowEdkPrompt;

    FunUpdateSoftKey                m_FunUpdateSoftKey;
    FunInsertSoftKey                m_FunInsertSoftKey;

    FunIsTopTipUI                   m_FunIsTopTipUI;
    FunIsTopMessageBoxUI            m_FunIsTopMessageBoxUI;
    FunHideNotifyCenter     m_FunHideNotifyCenter;
    FunIsInWizard                   m_FunIsInWizard;
    FunShowAutopResult              m_FunShowAutopResult;
    FunEnterRPS                     m_FunEnterRPS;
    FunZEROOpenDlg                  m_FunZEROOpenDlg;
    FunCreateMessageBox             m_FunCreateMessageBox;
    FunCreateMenuTips               m_FunCreateMenuTips;
    FunExitTriggers                 m_FunExitTriggers;
    FunEnterMenu                    m_FunEnterMenu;
    FunExecuteSoftkey               m_FunExecuteSoftkey;
    FunShowVolumeBar                m_FunShowVolumeBar;
    FunExitScreenSaver      m_FunExitScreenSaver;
    FunZeroAuthenticationAgain      m_FunZeroAuthenticationAgain;
    FunShowDsskeyLockPassword       m_FunShowDsskeyLockPassword;
    FunExitDsskeyDlg                m_FunExitDsskeyDlg;
    FunImageScale                   m_FunImageScale;
    FunTalkUIEnterConfWith          m_FunTalkUIEnterConfWith;
    FunUploadConfig                 m_FunUploadConfig;
    FunUpdateFirmware               m_FunUpdateFirmware;
    FunGetTotalUnReadMessage        m_FunGetTotalUnReadMessage;
    FunDsskeyLongPress              m_FunDsskeyLongPress;
};

#define _AdapterBox GET_SINGLETON_INSTANCE(CAdapterBox)

#endif // __ADAPTERBOX_H__
