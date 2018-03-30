#include "dlgmulticastpaginglist.h"

#include "actiondefinecommon.h"
#include "uikernel/languagehelper.h"
#include "keymap.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "setting/include/messagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "uimanager/uimanager_common.h"
#include "uicommon/qtcommon/qmisc.h"
#include "setting/src/multicastpaginglistcontroller.h"
#include "baseui/t4xpicpath.h"
#include "settingui/include/modsettingui.h"
#include "messagebox/modmessagebox.h"
#include "dsskeyuilogic/include/moddsskeyuilogic.h"

namespace
{
enum
{
    MSGBOX_ID_CONFIRM_DELETE = 100,
    MSGBOX_ID_CONFIRM_DELETE_ALL,
    MSGBOX_ID_CONFIRM_PASSWORD,
};

#define ACTION_PAGING_TO_GROUP  "paging"
#define ACTION_EDIT             "edit"
#define ACTION_DELETE           "delete"

// 点击Item的发送操作
#define ACTION_PAGING_BY_ITEM   "paging_by_item"
}

LRESULT CDlgMulticastPaingList::OnMessage(msgObject & refObj)
{
    if (refObj.message == MULTICAST_MESSAGE_PAGING_LIST_CHANGED)
    {
        SETTINGUI_INFO("OnMessage	EMULTICAST_MESSAGE_PAGING_LIST_CHANGED");

        CDlgMulticastPaingList * pCast = qobject_cast<CDlgMulticastPaingList *>(UIManager_GetPrivateDlg(
                                             DLG_CDlgMulticastPaingList));
        if (NULL == pCast || !pCast->IsTopPage())
        {
            SETTINGUI_INFO("no instance exist!");
            return 0;
        }

        pCast->LoadPageData();
        return TRUE;
    }

    return FALSE;
}

CDlgMulticastPaingList::CDlgMulticastPaingList(QWidget * parent)
    : CDlgBaseSubPage(parent)
{
    if (m_pGeneralFrame != NULL)
    {
        m_pGeneralFrame->SetItemSelectBG(PIC_WIDGET_SELECT_BG);
        m_pGeneralFrame->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP,
                                        PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                        PIC_GENERAL_LIST_ITEM_CLICK_BG);
    }

    m_bShowPassword = dsskeyuilogic_IsNeedPassword(DT_PAGING_LIST);
}

CDlgMulticastPaingList::~CDlgMulticastPaingList()
{
}

void CDlgMulticastPaingList::Init()
{
    etl_RegisterMsgHandle(
        MULTICAST_MESSAGE_PAGING_LIST_CHANGED,
        MULTICAST_MESSAGE_PAGING_LIST_CHANGED,
        OnMessage);
}

void CDlgMulticastPaingList::Uninit()
{
    etl_UnregisterMsgHandle(
        MULTICAST_MESSAGE_PAGING_LIST_CHANGED,
        MULTICAST_MESSAGE_PAGING_LIST_CHANGED,
        OnMessage);
}

bool CDlgMulticastPaingList::LoadPageData()
{
    int iPreSelect = 0;
    if (NULL != m_pGeneralFrame)
    {
        iPreSelect = m_pGeneralFrame->GetFocusItemDataIndex();
        if (iPreSelect == -1)
        {
            iPreSelect = 0;
        }
    }

    m_pagelist.clear();
    ClearAllContent();

    if (m_bShowPassword)
    {
        return true;
    }

    if (g_pPagingListController->GetPagingList(m_pagelist))
    {
        for (int i = 0; i < m_pagelist.size(); ++i)
        {
            QWidgetPair qPair;
            qPair.first = LANG_TRANSLATE(TRID_ACCOUNT_EMPTY);
            qPair.third = ACTION_PAGING_BY_ITEM;
            qPair.bChangeStyleOnPress = true;
            qPair.bChangeStyleOnSelect = true;
            m_vecWidgetPair.push_back(qPair);
        }

        for (int j = 0; j < m_pagelist.size(); ++j)
        {
            //序号从1开始
            PagingGroupInfo info = m_pagelist[j];
            int iOrder = info.iIndex;
            if (1 <= iOrder && !info.strAddress.empty())
            {
                // Lable若为空的话，用strAddress填充
                m_vecWidgetPair[iOrder - 1].first = !info.strLabel.empty() ?
                                                    toQString(info.strLabel) : toQString(info.strAddress);
            }
        }
        AddWidget(m_vecWidgetPair, true, false, true, true);
        SetCurrentIndex(iPreSelect);
        UpdateWnd();
    }
    else
    {
        //加载失败
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOAD_FAILED), MESSAGEBOX_NOTE, SHORT_TIME);
    }
    m_CurSelect = m_pGeneralFrame->GetFocusItemDataIndex();
    return true;
}

bool CDlgMulticastPaingList::OnAction(const QString & strAction)
{
    if (strAction == ACTION_PAGING_TO_GROUP)
    {
        if (NULL == m_pGeneralFrame || m_pagelist.size() <= m_pGeneralFrame->GetFocusItemDataIndex()
                || NULL == g_pPagingListController)
        {
            return false;
        }
        int iIndex  =  m_pGeneralFrame->GetFocusItemDataIndex();

        OnPaging(iIndex + 1);
        return true;
    }
    else if (strAction == ACTION_EDIT
             || strAction == ACTION_ENTER)
    {
        OnEditClicked();
        return true;
    }
    else if (strAction == ACTION_DELETE)
    {
        OnDeleteClicked();
        return true;
    }
    else if (strAction == ACTION_DELETEALL)
    {
        OnDeleteAllClicked();
        return true;
    }
    else if (strAction == ACTION_PAGING_BY_ITEM)
    {
        if (NULL == m_pGeneralFrame || m_pagelist.size() <= m_pGeneralFrame->GetFocusItemDataIndex()
                || NULL == g_pPagingListController)
        {
            return false;
        }
        int iIndex  =  m_pGeneralFrame->GetFocusItemDataIndex();
        // 当前点击的是选中项，则paiging
        //http://10.2.1.199/Bug.php?BugID=86711 去除选中项是当前项的onpaing操作，
        //连续执行两次onpaging操作会导致下一次失败 lins 2016.03.11
        if (m_CurSelect != iIndex)
        {
            m_CurSelect = iIndex;
        }

        return true;
    }
    else
    {
        return false;
    }

    return false;
}

bool CDlgMulticastPaingList::OnPaging(int iIndex)
{
    //序号iIndex从1开始
    if (iIndex < 1 || iIndex > m_pagelist.size() || m_pagelist[iIndex - 1].strAddress.empty())
    {
        SETTINGUI_INFO("OnPaging to group[%d], address is empty", iIndex);
        return false;
    }

    bool bInTalk = talkLogic_IsSessionInTalking();

    char szChannel[16] = {0};
    sprintf(szChannel, "%d", m_pagelist[iIndex - 1].iChannel);
    if (talklogic_CallPagingOut(m_pagelist[iIndex - 1].strLabel.c_str(),
                                m_pagelist[iIndex - 1].strAddress.c_str(), -1, szChannel))
    {
        if (bInTalk)
        {
            emit BtnBackClick();
        }
        return true;
    }

    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_PAING_FAILED), MESSAGEBOX_NOTE, 2000);
    return false;
}

void CDlgMulticastPaingList::OnEditClicked()
{
    if (NULL == m_pGeneralFrame
            || m_pagelist.size() <= m_pGeneralFrame->GetFocusItemDataIndex()
            || m_pGeneralFrame->GetFocusItemDataIndex() < 0)
    {
        return;
    }

    int iIndex  =  m_pGeneralFrame->GetFocusItemDataIndex();

    SubMenuData subData("edit_paging_group");
    subData.pBeforeSetSubpageData = &(m_pagelist[iIndex]);
    //进入编辑页面
    emit openpage(subData);
}

void CDlgMulticastPaingList::OnDeleteClicked()
{
    int iSelectIndex = m_pGeneralFrame->GetFocusItemDataIndex();
    if (iSelectIndex < 0 || iSelectIndex > m_pagelist.size())
    {
        return;
    }

    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_DELETE_PAGING),
                                MESSAGEBOX_OkCancel, -1, "", "", MSGBOX_ID_CONFIRM_DELETE);
}

void CDlgMulticastPaingList::OnDeleteAllClicked()
{
    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_DELETE_ALL_PAGING),
                                MESSAGEBOX_OkCancel, -1, "", "", MSGBOX_ID_CONFIRM_DELETE_ALL);
}

// 设置页面的SoftKey
bool CDlgMulticastPaingList::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[0].m_strSoftkeyAction = ACTION_EDIT;
    objWndData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_EDIT);
    objWndData[0].m_strIconPath = PIC_SOFTKEY_STATUS;

    objWndData[1].m_strSoftkeyAction = ACTION_DELETE;
    objWndData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DELETE);
    objWndData[1].m_strIconPath = PIC_SOFTKEY_REMOVE;

    objWndData[2].m_strSoftkeyAction = ACTION_DELETEALL;
    objWndData[2].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DELETE_ALL);
    objWndData[2].m_strIconPath = PIC_SOFTKEY_REMOVE;

    int iIndex = m_pGeneralFrame->GetFocusItemDataIndex();

    if (m_pGeneralFrame->GetTotalItemCount() < 1 || m_vecWidgetPair.size() <= iIndex)
    {
        objWndData[3].m_strSoftkeyAction = "";
        objWndData[3].m_strSoftkeyTitle = "";
        objWndData[3].m_strIconPath = "";
    }
    else
    {
        objWndData[3].m_strSoftkeyAction = ACTION_PAGING_TO_GROUP;
        objWndData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SOFTKEY_PAGING);
        objWndData[3].m_strIconPath = PIC_SOFTKEY_SUBMIT;
    }

    return true;
}

bool CDlgMulticastPaingList::GetPageDisableSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[2].m_strSoftkeyAction = ACTION_ENTER;
    objWndData[2].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_ENTER);
    objWndData[2].m_strIconPath = PIC_SOFTKEY_STATUS;

    objWndData[3].m_strSoftkeyAction = ACTION_PAGING_TO_GROUP;
    objWndData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SOFTKEY_PAGING);
    objWndData[3].m_strIconPath = PIC_SOFTKEY_SUBMIT;

    return true;
}

bool CDlgMulticastPaingList::eventFilter(QObject * pObj, QEvent * pEvt)
{
    switch (pEvt->type())
    {

    case QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvt;
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_OK:
                {
                    OnAction(ACTION_PAGING_TO_GROUP);
                    return true;
                }
                break;
            case  PHONE_KEY_0:
                {
                    if (NULL != m_pGeneralFrame)
                    {
                        int iDataIndex = 9;
                        if (NULL == m_pGeneralFrame->GetItemByDataIndex(iDataIndex))
                        {
                            SetCurrentIndex(iDataIndex);
                            UpdateWnd();
                        }
                        else
                        {
                            m_pGeneralFrame->SelectItemByDataIndex(iDataIndex);
                        }
                    }

                    OnPaging(10);
                    return true;
                }
                break;
            default:
                {
                    if (PHONE_KEY_1 <= pKeyEvent->key() && pKeyEvent->key() <= PHONE_KEY_9)
                    {
                        if (NULL != m_pGeneralFrame)
                        {
                            int iDataIndex = pKeyEvent->key() - PHONE_KEY_0 - 1;
                            if (NULL == m_pGeneralFrame->GetItemByDataIndex(iDataIndex))
                            {
                                SetCurrentIndex(iDataIndex);
                                UpdateWnd();
                            }
                            else
                            {
                                m_pGeneralFrame->SelectItemByDataIndex(iDataIndex);
                            }
                        }

                        OnPaging(pKeyEvent->key() - PHONE_KEY_0);
                        return true;
                    }
                }
                break;
            }

        }
        break;
    default:
        break;
    }


    return CDlgBaseSubPage::eventFilter(pObj, pEvt);
}

void CDlgMulticastPaingList::showEvent(QShowEvent * pEvent)
{
    LoadPageData();
}

void CDlgMulticastPaingList::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    int iMsgID = pMessageBox->GetID();
    int iRet = pMessageBox->GetResult();

    if (iMsgID == MSGBOX_ID_CONFIRM_DELETE && iRet == MessageBox_OK)
    {
        int iSelectIndex = m_pGeneralFrame->GetFocusItemDataIndex();
        if (g_pPagingListController->DeleteGroup(iSelectIndex + 1))
        {
            LoadPageData();
        }
        else
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_DELETE_PAGING_FAILED), MESSAGEBOX_NOTE,
                                        SHORT_TIME);
        }
    }

    if (iMsgID == MSGBOX_ID_CONFIRM_DELETE_ALL && iRet == MessageBox_OK)
    {
        if (g_pPagingListController->DeleteAllGroup())
        {
            LoadPageData();
        }
        else
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_DELETE_PAGING_FAILED), MESSAGEBOX_NOTE,
                                        SHORT_TIME);
        }
    }

    if (iMsgID == MSGBOX_ID_CONFIRM_PASSWORD)
    {
        if (iRet == MessageBox_OK)
        {
            m_bShowPassword = false;
            LoadPageData();
        }
        else
        {
            emit BtnBackClick();
        }
    }
}

void CDlgMulticastPaingList::ExtraDialogInitData()
{
    if (m_bShowPassword
            && dsskeyuilogic_IsNeedPassword(DT_PAGING_LIST))
    {
        MessageBox_ShowDsskeyLockPasswordBox(this, QVariant::fromValue(0), MSGBOX_ID_CONFIRM_PASSWORD);
    }
}

void CDlgMulticastPaingList::BeforeSetSubPage(void * pData)
{
    if (pData == NULL)
    {
        return;
    }

    m_bShowPassword = *(bool *)pData;
}
