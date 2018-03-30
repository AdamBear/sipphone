#include "messageboxmanager.h"
#include "messageboxbase.h"
#include "dsklog/log.h"
#include <algorithm>
#include "uimanager/moduimanager.h"
#include "messageboxcallback.h"

#include <configiddefine.h>
#include "mainwnd/mainwnd.h"
#include "modmessagebox.h"
#include "uimanager/rectdefine.h"
#include "baseui/maskframe.h"
#include "configparser/modconfigparser.h"
#include "qtcommon/qmisc.h"
#include "uimanager/uimanager_inc.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qsoftkeybar.h"
#include "uikernel/qcheckbox.h"
#include "uicommon.h"
#include "inputmethod/inputmethod.h"
#include "globalinputhook/modinputhook.h"
#include "keyguard/include/modkeyguard.h"
#include "setting/include/modsetting.h"
// #include "cdlgselectpopbox.h"
#include "directoryui/include/moddirectoryui.h"
#include "qtcommon/qdatatimer.h"
#include "lamp/backlight/include/modbacklight.h"

// 键盘回调函数
bool CMessageBoxManager::MessageBoxHookCallbackFun(int nKeyCode, bool bPress)
{
    return _MessageBoxManager.ProcessKeyEvent(nKeyCode, bPress);
}

// 鼠标回调函数
bool CMessageBoxManager::MessageBoxMouseHookCallbackFun(QObject * pObject, QMouseEvent * pEvent)
{
    return _MessageBoxManager.ProcessMouseEvent(pObject, pEvent);
}

bool CMessageBoxManager::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    CMessageBoxBase * pMessageBox = GetTopMessageBox();
    if (NULL != pMessageBox)
    {
        return pMessageBox->ProcessKeyEvent(nKeyCode, bPress);
    }

    return false;
}


bool CMessageBoxManager::ProcessMouseEvent(QObject * pObject, QMouseEvent * pEvent)
{
    CMessageBoxBase * pMessageBox = GetTopMessageBox();
    if (NULL != pMessageBox)
    {
        return pMessageBox->eventFilter(pObject, pEvent);
    }

    return false;
}

void CMessageBoxManager::OnTopWndChangeCallbackFun(CBaseDialog * dlgPreTop, CBaseDialog * dlgNewTop)
{
    POPUPBOX_INFO("CMsgBoxManager::OnTopWndChangeCallbackFun");
    _MessageBoxManager.RemoveAllMessageBox(MessageBox_Destory);
}

bool CMessageBoxManager::MessageBoxKeyBoardShowCallbackFun(bool bKeyBoardShow)
{
    if (bKeyBoardShow)
    {
        return _MessageBoxManager.OnKeyboardShow();
    }
    else
    {
        return _MessageBoxManager.OnKeyboardHide();
    }
}

CMessageBoxManager & GetMessageBoxManagerInstance()
{
    static CMessageBoxManager s_MessageBoxManager;
    return s_MessageBoxManager;
}


CMessageBoxManager::CMessageBoxManager()
    : m_bIsKeyboardShow(false)
{
    //注册键盘事件
    inputHook_RegisterKeyPadHookProc(CMessageBoxManager::MessageBoxHookCallbackFun,
                                     HOOK_MODULE_MESSAGEBOX);

    //注册鼠标事件
    inputHook_RegisterMouseHookProc(CMessageBoxManager::MessageBoxMouseHookCallbackFun,
                                    HOOK_MODULE_MESSAGEBOX);

    UIManager_RegisterTopWndChangeCallBack(CMessageBoxManager::OnTopWndChangeCallbackFun);

    //连接灰色遮罩被点击时的处理槽
//  connect(_MainWND.GetMaskFrame(), SIGNAL(click()), this, SLOT(MaskFramePressed()));

    if (FullKeyboardEnabled())
    {
        inputMethod_RegisterKeyBoardShowHookProc(CMessageBoxManager::MessageBoxKeyBoardShowCallbackFun, KEYBOARD_SHOW_HOOK_MODULE_MESSAGEBOX);
    }

    m_vecMsgBoxInfo.clear();
}

CMessageBoxManager::~CMessageBoxManager()
{
    inputHook_UnRegisterKeyPadHookProc(CMessageBoxManager::MessageBoxHookCallbackFun);
    UIManager_UnregisterTopWndChangeCallBack(CMessageBoxManager::OnTopWndChangeCallbackFun);
    inputHook_UnRegisterMouseHookProc(MessageBoxMouseHookCallbackFun);

    inputMethod_UnRegisterKeyBoardShowHookProc(CMessageBoxManager::MessageBoxKeyBoardShowCallbackFun);
}

bool CMessageBoxManager::AddMessageBox(CMessageBoxBase * pMessageBox,
                                       CMsgBoxCallBackAction * pCallBack)
{
    POPUPBOX_INFO("CMsgBoxManager::AddMessageBox");
    if (NULL == pMessageBox)
    {
        return false;
    }

    // CallBackAction和弹出框是一对多的关系
    ItMsgInfo it = m_vecMsgBoxInfo.begin();
    for (; it != m_vecMsgBoxInfo.end(); ++it)
    {
        if (pMessageBox == it->m_pMsgBox)
        {
            POPUPBOX_WARN("messageBox is exit!");
            return false;
        }
    }

    m_vecMsgBoxInfo.push_back(MsgBoxInfo(pMessageBox, pCallBack));

    UpdateWindow();
    return true;
}

bool CMessageBoxManager::RemoveTopMessageBox(MESSAGEBOX_RESULT nResultType)
{
    POPUPBOX_INFO("CMsgBoxManager::RemoveTopMessageBox ResultType[%d]", nResultType);
    if (m_vecMsgBoxInfo.empty())
    {
        POPUPBOX_WARN("msgbox empty!");
        return false;
    }

    return RemoveMessageBox(m_vecMsgBoxInfo.back().m_pMsgBox, nResultType, true);
}

void CMessageBoxManager::MaskFramePressed()
{
    RemoveTopMessageBox(MessageBox_Cancel);
}

bool CMessageBoxManager::RemoveMessageBox(CMsgBoxCallBackAction * pCallBack,
        MESSAGEBOX_RESULT nResultType,
        bool bCallBack/* = false*/,
        int iID/* = -1*/)
{
    POPUPBOX_INFO("RemoveMessageBox By CallBackAction  ResultType[%d], bCallBack[%d]",
                  nResultType, bCallBack);
    bool bRemove = false;

    // 入口参数判断
    if (NULL == pCallBack || m_vecMsgBoxInfo.empty())
    {
        return bRemove;
    }

    // 判断删除该msgbox是否需要刷新
    CMessageBoxBase * topMsgBox = m_vecMsgBoxInfo.back().m_pMsgBox;
    bool bUpdate = (m_vecMsgBoxInfo.back().m_pCallBackAction == pCallBack)
                   || (topMsgBox != NULL);

    // 清除该窗口
    CEqualMsgCBAction ObjFunEqual(pCallBack, iID);
    ItMsgInfo it = find_if(m_vecMsgBoxInfo.begin(),
                           m_vecMsgBoxInfo.end(),
                           ObjFunEqual);

    while (it != m_vecMsgBoxInfo.end())
    {
        MsgBoxInfo msgboxInfo = *it;
        m_vecMsgBoxInfo.erase(it);
        ProcessMsgBoxExit(msgboxInfo, nResultType, bCallBack);
        bRemove = true;

        it = find_if(m_vecMsgBoxInfo.begin(),
                     m_vecMsgBoxInfo.end(),
                     ObjFunEqual);
    }

    if (bUpdate && bRemove)
    {
        UpdateWindow();
    }

    return bRemove;
}


bool CMessageBoxManager::RemoveMessageBox(CMessageBoxBase * pMessageBox,
        MESSAGEBOX_RESULT nResultType,
        bool bCallBack/* = false*/)
{
    POPUPBOX_INFO("RemoveMessageBox By MessageBox ResultType[%d], bCallBack[%d]",
                  nResultType, bCallBack);

    bool bRemove = false;

    // 入口参数判断
    if (NULL == pMessageBox || m_vecMsgBoxInfo.empty())
    {
        return bRemove;
    }

    // 判断删除该msgbox是否需要刷新
    CMessageBoxBase * topMsgBox = m_vecMsgBoxInfo.back().m_pMsgBox;
    bool bUpdate = (topMsgBox == pMessageBox)
                   || (topMsgBox != NULL);

    // 清除该窗口
    CEqualMsgBox ObjFunEqual(pMessageBox);
    ItMsgInfo it = find_if(m_vecMsgBoxInfo.begin(),
                           m_vecMsgBoxInfo.end(),
                           ObjFunEqual);

    while (it != m_vecMsgBoxInfo.end())
    {
        MsgBoxInfo msgboxInfo = *it;
        m_vecMsgBoxInfo.erase(it);
        ProcessMsgBoxExit(msgboxInfo, nResultType, bCallBack);
        bRemove = true;

        it = find_if(m_vecMsgBoxInfo.begin(),
                     m_vecMsgBoxInfo.end(),
                     ObjFunEqual);
    }

    if (bUpdate && bRemove)
    {
        UpdateWindow();
    }

    return bRemove;
}

bool CMessageBoxManager::RemoveAllMessageBox(MESSAGEBOX_RESULT nResultType)
{
    POPUPBOX_INFO("CMsgBoxManager::RemoveAllMessageBox ResultType[%d]", nResultType);
    if (m_vecMsgBoxInfo.empty())
    {
        return true;
    }

    while (!m_vecMsgBoxInfo.empty())
    {
        MsgBoxInfo msgboxInfo = m_vecMsgBoxInfo.back();
        m_vecMsgBoxInfo.pop_back();
        ProcessMsgBoxExit(msgboxInfo, nResultType);
    }

    // 刷新窗口
    UpdateWindow();
    return true;
}

int CMessageBoxManager::GetMessageBoxCount()
{
    return m_vecMsgBoxInfo.size();
}


void CMessageBoxManager::UpdateWindow()
{
    _MainWND.ResetDownSoftkey();

    if (m_vecMsgBoxInfo.empty())
    {
        POPUPBOX_INFO("CMsgBoxManager Clear MsgBox Info");
        _MainWND.SetGrayShow(false);
    }
    else
    {
        POPUPBOX_INFO("CMsgBoxManager Update MsgBox Info");

        if (FullKeyboardEnabled())
        {
            ItMsgInfo it = m_vecMsgBoxInfo.begin();
            for (; it != m_vecMsgBoxInfo.end(); ++it)
            {
                CMessageBoxBase* pMessageBox = it->m_pMsgBox;
                if (NULL == pMessageBox)
                {
                    continue;
                }

                QRect MsgBoxRect = pMessageBox->geometry();

                if (//这些类型的框不能根据全键盘移动
                    MESSAGEBOX_LINE_SELECT != pMessageBox->GetType()
                    && MESSAGEBOX_IMAGE_SELECT != pMessageBox->GetType())
                {
                    if (m_bIsKeyboardShow)
                    {
                        pMessageBox->setGeometry(MsgBoxRect.left(), (g_iFullKeyBoardY - MsgBoxRect.height()) / 2, MsgBoxRect.width(), MsgBoxRect.height());
                    }
                    else
                    {
                        pMessageBox->setGeometry(MsgBoxRect.left(), (PHONE_HEIGHT - MsgBoxRect.height()) / 2, MsgBoxRect.width(), MsgBoxRect.height());
                    }
                }
            }
        }

        int iTopMsgBoxIndex = m_vecMsgBoxInfo.size() - 1;
        CMessageBoxBase * pMessageBox = m_vecMsgBoxInfo[iTopMsgBoxIndex].m_pMsgBox;
        if (NULL != pMessageBox)
        {
            CMsgBoxCallBackAction * pMessageAction = m_vecMsgBoxInfo[iTopMsgBoxIndex].m_pCallBackAction;

            if (NULL == pMessageAction || pMessageAction->IsUrgentMsgBox(pMessageBox))
            {
                backlight_Reset();
            }

            _MainWND.SetGrayShow(true);
            qRaiseFullKeyboard();
            pMessageBox->show();
        }
    }
}

bool CMessageBoxManager::ProcessMsgBoxExit(const MsgBoxInfo & msgboxInfo,
        MESSAGEBOX_RESULT nResultType,
        bool bCallBack)
{
    POPUPBOX_INFO("CMsgBoxManager::ProcessMsgBoxExit nResultType[%d]", nResultType);
    // 参数判断
    if (NULL == msgboxInfo.m_pMsgBox)
    {
        return false;
    }

    // 设置关闭原因
    msgboxInfo.m_pMsgBox->SetResult(nResultType);

    // 回调处理
    if (NULL != msgboxInfo.m_pCallBackAction
            && bCallBack)
    {
        msgboxInfo.m_pCallBackAction->MessageBoxCallBack(msgboxInfo.m_pMsgBox);
    }

    // 延迟删除
    msgboxInfo.m_pMsgBox->hide();
    msgboxInfo.m_pMsgBox->deleteLater();
    return true;
}

CMessageBoxBase * CMessageBoxManager::GetTopMessageBox()
{
    return m_vecMsgBoxInfo.empty() ? NULL : m_vecMsgBoxInfo.back().m_pMsgBox;
}

bool CMessageBoxManager::OnKeyboardShow()
{
    m_bIsKeyboardShow = true;
    if (GetMessageBoxCount() > 0)
    {
        UpdateWindow();
        return false;
    }
    else
    {
        return false;
    }
    return false;
}

bool CMessageBoxManager::OnKeyboardHide()
{
    m_bIsKeyboardShow = false;
    if (GetMessageBoxCount() > 0)
    {
        UpdateWindow();
        return false;
    }
    else
    {
        return false;
    }
    return false;
}
