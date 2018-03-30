#include "dirmsgboxmgr.h"
#include "directorymgr.h"
#include "moddiruilogic.h"
#include "messagebox/modmessagebox.h"
#include "languagehelper/modlangtranslate.h"
#include "messagebox/selectmessagebox.h"


IMPLEMENT_GETINSTANCE(CDirMsgBoxMgr)

bool CDirMsgBoxMgr::s_bShowWaiting = false;
int CDirMsgBoxMgr::s_nWaitingAction = DIR_ACTION_NONE;
CDirMsgBoxMgr::MAP_MSG_BOX_ACTION_ID CDirMsgBoxMgr::s_mapActionId;


CDirMsgBoxMgr::CDirMsgBoxMgr()
{
}

CDirMsgBoxMgr::~CDirMsgBoxMgr()
{
}

void CDirMsgBoxMgr::Init()
{
    SetFunShowMsgBox(&CDirMsgBoxMgr::ShowMsgBox);
    SetFunPopSureMsgBox(&CDirMsgBoxMgr::PopSureMsgBox);
    SetFunShowWaiting(&CDirMsgBoxMgr::ShowWaiting);
    SetFunEndWaiting(&CDirMsgBoxMgr::EndWaiting);
}

void CDirMsgBoxMgr::UnInit()
{
    SetFunShowMsgBox(NULL);
    SetFunPopSureMsgBox(NULL);
    SetFunShowWaiting(NULL);
    SetFunEndWaiting(NULL);
}

void CDirMsgBoxMgr::ShowMsgBox(const char * pszMsg, const char * pszExtra/* = NULL*/,
                               int nAction/* = DIR_ACTION_NONE*/)
{
    if (NULL == pszMsg)
    {
        DIRUI_ERR("ShowMsgBox pszMsg is NULL");
        return;
    }

    yl::string strMsg = _LANG(pszMsg);

    if (NULL != pszExtra)
    {
        strMsg += _LANG(pszExtra);
    }

    MSGBOX_INFO stMsgInfo;

    stMsgInfo.eType = MESSAGEBOX_NOTE;
    stMsgInfo.strTitle = _LANG(TRID_NOTE);
    stMsgInfo.strNote = strMsg;
    stMsgInfo.nMsTime = SHORT_TIME;
    stMsgInfo.pAction = &g_DirMsgBoxMgr;
    stMsgInfo.iID = nAction;

    MessageBox_ShowCommonMsgBox(stMsgInfo);
}

bool CDirMsgBoxMgr::PopSureMsgBox(const char * pszTitle, const char * pszMsg, int nAction,
                                  int nId, const char * pszExtra/* = NULL*/)
{
    if (NULL == pszTitle || NULL == pszMsg)
    {
        DIRUI_ERR("PopSureMsgBox pszTitle:%s pszMsg:%s", (NULL == pszTitle ? "NULL" : pszTitle),
                  (NULL == pszMsg ? "NULL" : pszMsg));

        return false;
    }

    yl::string strMsg = _LANG(pszMsg);
    if (NULL != pszExtra)
    {
        strMsg += _LANG(pszExtra);
    }

    SetMsgBoxActionId(nAction, nId);

    MSGBOX_INFO stMsgInfo;

    stMsgInfo.eType = MESSAGEBOX_OkCancel;
    stMsgInfo.strTitle = _LANG(pszTitle);
    stMsgInfo.strNote = strMsg;
    stMsgInfo.nMsTime = -1;
    stMsgInfo.pAction = &g_DirMsgBoxMgr;
    stMsgInfo.iID = nAction;

    MessageBox_ShowCommonMsgBox(stMsgInfo);

    return true;
}

#if IF_FEATURE_FAVORITE_DIR
bool CDirMsgBoxMgr::PopSureEditMsgBox(const char * pszTitle, const char * pszMsg, int nAction, int nId)
{
    if (NULL == pszTitle || NULL == pszMsg)
    {
        DIRUI_ERR("PopSureMsgBox pszTitle:%s pszMsg:%s", (NULL == pszTitle ? "NULL" : pszTitle),
                  (NULL == pszMsg ? "NULL" : pszMsg));

        return false;
    }

    SetMsgBoxActionId(nAction, nId);

    MSGBOX_INFO stMsgInfo;

    stMsgInfo.eType = MESSAGEBOX_OkCancel;
    stMsgInfo.strTitle = _LANG(pszTitle);
    stMsgInfo.strNote = pszMsg;
    stMsgInfo.nMsTime = -1;
    stMsgInfo.pAction = &g_DirMsgBoxMgr;
    stMsgInfo.iID = nAction;

    MsgBoxSoftkey stSoftKey;
    stSoftKey.m_title = _LANG(TRID_CANCEL);
    stSoftKey.m_result = MessageBox_Cancel;
    stMsgInfo.vecSoftkey.push_back(stSoftKey);

    stSoftKey.m_title = "";
    stSoftKey.m_result = MessageBox_ERROR;
    stMsgInfo.vecSoftkey.push_back(stSoftKey);

    stSoftKey.m_title = _LANG(TRID_EDIT);
    stSoftKey.m_result = MessageBox_Edit;
    stMsgInfo.vecSoftkey.push_back(stSoftKey);

    stSoftKey.m_title = _LANG(TRID_OK);
    stSoftKey.m_result = MessageBox_OK;
    stMsgInfo.vecSoftkey.push_back(stSoftKey);

    MessageBox_ShowCommonMsgBox(stMsgInfo);

    return true;
}
#endif

void CDirMsgBoxMgr::ShowWaiting(const char * pszTitle, const char * pszMsg, int nAction)
{
    if (NULL == pszTitle || NULL == pszMsg)
    {
        return;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

    if (pControler.IsEmpty())
    {
        return;
    }

    if (s_bShowWaiting)
    {
        return;
    }

    s_bShowWaiting = true;
    s_nWaitingAction = nAction;

    MSGBOX_INFO stMsgInfo;

    stMsgInfo.eType = MESSAGEBOX_Cancel;
    stMsgInfo.strTitle = _LANG(TRID_WARNING);
    stMsgInfo.strNote = _LANG(pszMsg);
    stMsgInfo.nMsTime = -1;
    stMsgInfo.pAction = &g_DirMsgBoxMgr;
    stMsgInfo.iID = nAction;

    MessageBox_ShowCommonMsgBox(stMsgInfo);
}

void CDirMsgBoxMgr::EndWaiting()
{
    MessageBox_RemoveMessageBox(&g_DirMsgBoxMgr, MessageBox_Destory, s_nWaitingAction);
}

void CDirMsgBoxMgr::ShowOptionMsgBox(VEC_OPTION_ITEM_DATA & vecOption, int nID /* = -1 */,
                                     int nSelectIndex /* = -1 */, bool bShowCheckIcon /* = false */)
{
    MSGBOX_INFO stMsgInfo;

    stMsgInfo.pAction = &g_DirMsgBoxMgr;
    stMsgInfo.eType = (MESSAGEBOX_TYPE)MESSAGEBOX_SELECTBOX;
    stMsgInfo.iID = nID;
    stMsgInfo.nMsTime = -1;

    SELECTMSGBOXINFO objSelectBoxInfo;
    objSelectBoxInfo.m_vecSelectMsgBoxItem = vecOption;
    objSelectBoxInfo.m_SelectIndex = nSelectIndex;
    objSelectBoxInfo.m_bShowCheckIcon = bShowCheckIcon;

    stMsgInfo.extraData = &objSelectBoxInfo;
    MessageBox_ShowCommonMsgBox(stMsgInfo);
}

void CDirMsgBoxMgr::SetMsgBoxActionId(int nAction, int nId)
{
    s_mapActionId[nAction] = nId;
}

int CDirMsgBoxMgr::PopMsgBoxActionId(int nAction)
{
    MAP_MSG_BOX_ACTION_ID::iterator iter = s_mapActionId.find(nAction);

    if (iter != s_mapActionId.end())
    {
        int nId = iter->second;

        s_mapActionId.erase(iter);

        return nId;
    }

    return knInvalidRId;
}

void CDirMsgBoxMgr::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    switch (pMessageBox->GetType())
    {
    case MESSAGEBOX_NOTE:
        {
            int nAction = pMessageBox->GetID();
            int nResult = pMessageBox->GetResult();

            if ((MessageBox_TimeOut == nResult || MessageBox_Cancel == nResult)
                    && -1 != nAction && DIR_ACTION_NONE != nAction)
            {
                CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

                if (!pControler.IsEmpty())
                {
                    pControler->ExecAction(nAction, INVALID_ITEM_INDEX, knInvalidRId);
                }
            }
        }
        break;
    case MESSAGEBOX_Cancel:
        {
            int nAction = pMessageBox->GetID();

            s_bShowWaiting = false;
            s_nWaitingAction = DIR_ACTION_NONE;

            if (MessageBox_Cancel == pMessageBox->GetResult() && -1 != nAction && DIR_ACTION_NONE != nAction)
            {
                CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

                if (!pControler.IsEmpty())
                {
                    pControler->CancelAction(nAction, INVALID_ITEM_INDEX, knInvalidRId);
                }
            }
        }
        break;
    case MESSAGEBOX_OkCancel:
        {
            CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
            if (pControler.IsEmpty())
            {
                return;
            }
            int nAction = pMessageBox->GetID();
            int nId = PopMsgBoxActionId(nAction);

            if (MessageBox_OK == pMessageBox->GetResult() && -1 != nAction && DIR_ACTION_NONE != nAction)
            {
                pControler->ExecAction(nAction, INVALID_ITEM_INDEX, nId);
            }
#if IF_FEATURE_FAVORITE_DIR
            else if (MessageBox_Edit == pMessageBox->GetResult()
                     && DIR_ACTION_LOCAL_COPY_TO_FAVORITE == nAction)
            {
                pControler->ExecAction(DIR_ACTION_LOCAL_EDIT_FAVORITE, INVALID_ITEM_INDEX, nId);
            }
#endif
        }
        break;
    case MESSAGEBOX_SELECTBOX:
        {
            CSelectMessageBox * pOptionMsgBox = (CSelectMessageBox *)pMessageBox;

            int nAction = pOptionMsgBox->GetFcousAction();

            if (MessageBox_OK == pOptionMsgBox->GetResult() && -1 != nAction && ST_EMPTY != nAction)
            {
                CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();

                if (!pAdapter.IsEmpty())
                {
                    pAdapter->ProcessAction(nAction);
                }
            }
        }
        break;
    default:
        break;
    }
}
