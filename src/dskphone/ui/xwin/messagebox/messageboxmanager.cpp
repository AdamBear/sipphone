#include "messageboxmanager.h"
#include "messageboxcache.h"
#include "baseui/include/baseuicommon.h"
#include "commonmessagebox.h"
#include "notemessagebox.h"
#include "passwordmessagebox.h"
#include "middlemessagebox.h"

#if IF_MULTI_WORKSTATION_ENBALE
IMPLEMENT_MULTI_WORKSTATION_INSTANCE(CMessageBoxManager)
#else
IMPLEMENT_WORKSTATION_INSTANCE(CMessageBoxManager)
#endif

// 键盘回调函数
bool CMessageBoxManager::MessageBoxHookCallbackFun(int nKeyCode, bool bPress)
{
    return _MessageBoxManager.ProcessKeyEvent(nKeyCode, bPress);
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

void CMessageBoxManager::OnTopWndChangeCallbackFun(const CTopWndChangeInfo & objInfo)
{
    if (!objInfo.m_bBeforeTopWndChange)
    {
        _MessageBoxManager.RemoveAllMessageBox(MessageBox_Destory);
    }
}

CMessageBoxManager::CMessageBoxManager()
{
    //注册键盘事件
    inputHook_RegisterKeyPadHookProc(CMessageBoxManager::MessageBoxHookCallbackFun,
                                     HOOK_MODULE_MESSAGEBOX);

    UIManager_RegisterTopWndChangeCallBack(CMessageBoxManager::OnTopWndChangeCallbackFun);

    m_vecMsgBoxInfo.clear();
}

CMessageBoxManager::~CMessageBoxManager()
{
    inputHook_UnRegisterKeyPadHookProc(HOOK_MODULE_MESSAGEBOX);
    UIManager_UnregisterTopWndChangeCallBack(CMessageBoxManager::OnTopWndChangeCallbackFun);
}

bool CMessageBoxManager::AddMessageBox(CMessageBoxBase * pMessageBox,
                                       CMsgBoxCallBackAction * pCallBack)
{
    if (NULL == pMessageBox)
    {
        return false;
    }

    signalMessageBoxShow.emitSignal(0, 0, 0, NULL);

#ifdef IF_MSG_BOX_EXIT_OTHER_MSGBOX
    //如果已经存在的顶层弹出框”单独存在“的属性为真，则将其移除
    CMessageBoxBase * pTopMsgBox = GetTopMessageBox();
    if (NULL != pTopMsgBox && pTopMsgBox->GetIsExistOnly())
    {
        RemoveTopMessageBox(MessageBox_Destory);
    }

    //如果正要弹的弹出框”单独存在“的属性为真，则将其它所有弹出框移除
    if (pMessageBox->GetIsExistOnly())
    {
        RemoveAllMessageBox(MessageBox_Destory);
    }
#endif

    // CallBackAction和弹出框是一对多的关系
    ItMsgInfo it = m_vecMsgBoxInfo.begin();
    for (; it != m_vecMsgBoxInfo.end(); ++it)
    {
        //如果该弹出框已经弹出，则不再弹
        if (pMessageBox == it->m_pMsgBox)
        {
            return false;
        }
//      else
//      {
//          it->m_pMsgBox->hide();
//      }
    }

    m_vecMsgBoxInfo.push_back(MsgBoxInfo(pMessageBox, pCallBack));

    UpdateWindow();
    return true;
}

bool CMessageBoxManager::RemoveTopMessageBox(MESSAGEBOX_RESULT nResultType)
{
    int nMsgBoxCount = m_vecMsgBoxInfo.size();
    if (0 == nMsgBoxCount)
    {
        return false;
    }

    return RemoveMessageBox(m_vecMsgBoxInfo.at(nMsgBoxCount - 1).m_pMsgBox, nResultType, true);
}

bool CMessageBoxManager::RemoveMessageBox(CMsgBoxCallBackAction * pCallBack,
        MESSAGEBOX_RESULT nResultType,
        bool bCallBack/* = true*/,
        int iID/* = -1*/)
{
    bool bRemove = false;
    int nMsgBoxCount = m_vecMsgBoxInfo.size();

    // 入口参数判断
    if (NULL == pCallBack || 0 == nMsgBoxCount)
    {
        return bRemove;
    }

    // 判断删除该msgbox是否需要刷新
    CMessageBoxBase * topMsgBox = m_vecMsgBoxInfo.at(nMsgBoxCount - 1).m_pMsgBox;
    bool bUpdate = (m_vecMsgBoxInfo.at(nMsgBoxCount - 1).m_pCallBackAction == pCallBack)
                   || (topMsgBox != NULL);

    // 清除该窗口
    for (int i = 0; i < m_vecMsgBoxInfo.size();)
    {
        MsgBoxInfo msgboxInfo = m_vecMsgBoxInfo.at(i);
        if (msgboxInfo.m_pCallBackAction == pCallBack
                && (-1 == iID
                    || (NULL != msgboxInfo.m_pMsgBox
                        && iID == msgboxInfo.m_pMsgBox->GetID())))
        {
            m_vecMsgBoxInfo.removeAt(i);
            ProcessMsgBoxExit(msgboxInfo, nResultType, bCallBack);
            bRemove = true;
            continue;
        }
        else
        {
            ++i;
        }
    }

    if (bUpdate && bRemove)
    {
        UpdateWindow();
    }

    return bRemove;
}


bool CMessageBoxManager::RemoveMessageBox(CMessageBoxBase * pMessageBox,
        MESSAGEBOX_RESULT nResultType,
        bool bCallBack/* = true*/)
{
    bool bRemove = false;
    int nMsgBoxCount = m_vecMsgBoxInfo.size();

    // 入口参数判断
    if (NULL == pMessageBox || 0 == nMsgBoxCount)
    {
        return bRemove;
    }

    pMessageBox->SetIsRemovedByManager(true);

    // 判断删除该msgbox是否需要刷新
    CMessageBoxBase * topMsgBox = m_vecMsgBoxInfo.at(nMsgBoxCount - 1).m_pMsgBox;
    bool bUpdate = (topMsgBox == pMessageBox)
                   || (topMsgBox != NULL);

    // 清除该窗口
    for (int i = 0; i < m_vecMsgBoxInfo.size();)
    {
        MsgBoxInfo msgboxInfo = m_vecMsgBoxInfo.at(i);
        if (msgboxInfo.m_pMsgBox == pMessageBox)
        {
            m_vecMsgBoxInfo.removeAt(i);
            ProcessMsgBoxExit(msgboxInfo, nResultType, bCallBack);
            bRemove = true;
            continue;
        }
        else
        {
            ++i;
        }
    }

    if (bUpdate && bRemove)
    {
        UpdateWindow();
    }

    return bRemove;
}

bool CMessageBoxManager::RemoveAllMessageBox(MESSAGEBOX_RESULT nResultType)
{
    int nMsgBoxCount = m_vecMsgBoxInfo.size();
    if (0 == nMsgBoxCount)
    {
        return true;
    }

    while (0 != nMsgBoxCount)
    {
        MsgBoxInfo msgboxInfo = m_vecMsgBoxInfo.at(nMsgBoxCount - 1);
        m_vecMsgBoxInfo.removeAt(nMsgBoxCount - 1);
        ProcessMsgBoxExit(msgboxInfo, nResultType, true, false);
        nMsgBoxCount = m_vecMsgBoxInfo.size();
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

    CMessageBoxBase * pMessageBox = GetTopMessageBox();
    if (NULL != pMessageBox)
    {
        ItMsgInfo it = m_vecMsgBoxInfo.begin();
        for (; it != m_vecMsgBoxInfo.end(); ++it)
        {
            CMessageBoxBase * pMsgBox = it->m_pMsgBox;
            if (NULL == pMsgBox || pMessageBox == pMsgBox)
            {
                continue;;
            }

            if (pMsgBox->IsShowAlways())
            {
                pMsgBox->show();
            }
            else
            {
                pMsgBox->hide();
            }
        }

        pMessageBox->show();
        pMessageBox->SetWidgetFocus();
    }
}

bool CMessageBoxManager::ProcessMsgBoxExit(const MsgBoxInfo & msgboxInfo,
        MESSAGEBOX_RESULT nResultType,
        bool bCallBack,
        bool bPlayAnimation)
{
    // 参数判断
    if (NULL == msgboxInfo.m_pMsgBox)
    {
        return false;
    }

    // 设置关闭原因
    msgboxInfo.m_pMsgBox->SetResult(nResultType);

    msgboxInfo.m_pMsgBox->hide();

    // 回调处理
    if (NULL != msgboxInfo.m_pCallBackAction
            && bCallBack)
    {
        msgboxInfo.m_pCallBackAction->MessageBoxCallBack(msgboxInfo.m_pMsgBox);
    }

    // 延迟删除
    msgboxInfo.m_pMsgBox->DeleteThisBox();
    //放回缓存
//  _MessageBoxCache.PutMessageBox(msgboxInfo.m_pMsgBox);

    return true;
}

CMessageBoxBase * CMessageBoxManager::GetTopMessageBox()
{
    int nMsgBoxCount = m_vecMsgBoxInfo.size();
    if (0 == nMsgBoxCount)
    {
        return NULL;
    }
    else
    {
        return m_vecMsgBoxInfo.at(nMsgBoxCount - 1).m_pMsgBox;
    }

    return NULL;
}

bool CMessageBoxManager::RemoveMessageBoxType(MESSAGEBOX_TYPE nType,
        MESSAGEBOX_RESULT eResult/* = MessageBox_Cancel*/)
{
    int nMsgBoxCount = m_vecMsgBoxInfo.size();
    if (0 == nMsgBoxCount)
    {
        return false;
    }

    int nCount = 0;

    // CallBackAction和弹出框是一对多的关系
    ItMsgInfo it = m_vecMsgBoxInfo.begin();
    for (; it != m_vecMsgBoxInfo.end() && 0 != m_vecMsgBoxInfo.size(); ++it)
    {
        //如果该弹出框已经弹出，则不再弹
        if (NULL != it->m_pMsgBox && nType == it->m_pMsgBox->GetType())
        {
            RemoveMessageBox(it->m_pMsgBox, eResult);
            //从头开始数起
            it = m_vecMsgBoxInfo.begin();
            ++nCount;
        }
    }

    if (nCount > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void CMessageBoxManager::RegisterMsgBox(MESSAGEBOX_TYPE eMsgBoxType,
                                        FUN_CREATE_MSGBOX pFunCreateMsgBox)
{
    if (MESSAGEBOX_Error == eMsgBoxType || NULL == pFunCreateMsgBox)
    {
        return;
    }

    chASSERTx(m_mapCreateFun.find(eMsgBoxType) == m_mapCreateFun.end(),
              "repeat register same type MsgBox:%0x", eMsgBoxType);

    m_mapCreateFun[eMsgBoxType] = pFunCreateMsgBox;
}

CMessageBoxBase * CMessageBoxManager::ApplyMsgBox(const MSGBOX_INFO & stMsgInfo)
{
    MESSAGEBOX_TYPE eMsgBoxType = stMsgInfo.eType;

    MAP_CREATE_MSGBOX_FUN::iterator iter = m_mapCreateFun.find(eMsgBoxType);
    FUN_CREATE_MSGBOX pFunCreateMsgBox = NULL;

    if (iter != m_mapCreateFun.end())
    {
        pFunCreateMsgBox = iter->second;
    }

    if (NULL == pFunCreateMsgBox)
    {
        return ApplyDefaultMsgBox(stMsgInfo);
    }

    return (*pFunCreateMsgBox)(stMsgInfo);
}

CMessageBoxBase * CMessageBoxManager::ApplyDefaultMsgBox(const MSGBOX_INFO & stMsgInfo)
{
    CMessageBoxBase * pMsgBox = NULL;

    switch (stMsgInfo.eType)
    {
    case MESSAGEBOX_NOTE:
        pMsgBox = CNoteMessageBox::CreateNoteMsgBox(stMsgInfo);
        break;
    case MESSAGEBOX_Cancel:
    case MESSAGEBOX_OkCancel:
        pMsgBox = CCommonMessageBox::CreateCommonMsgBox(stMsgInfo);
        break;
    case MESSAGEBOX_PWD:
        pMsgBox = CPasswordMessageBox::CreatePasswordMsgBox(stMsgInfo);
        break;
    case MESSAGEBOX_MIDDLE:
        pMsgBox = CMiddleMessageBox::CreateMiddleMsgBox(stMsgInfo);
        break;
    default:
        break;
    }

    return pMsgBox;
}
