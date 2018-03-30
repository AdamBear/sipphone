#include "detaildelegate.h"
#include "directorylistaction.h"
#include "btuccontactcontroler.h"
#include "commondirctrl.h"
#include "ldapcontactcontroler.h"
#include "baseui/framelist.h"


CDetailDelegate::CDetailDelegate()
    : CDirectoryListDelegate(DIR_UI_STATE_DETAIL)
{
}

CDetailDelegate::~CDetailDelegate(void)
{
}

CDetailDelegate * CDetailDelegate::GetDirDelegate(CDirectoryListDelegatePtr & pDelegate)
{
    if (pDelegate.IsEmpty() || DIR_UI_STATE_DETAIL != pDelegate->GetState())
    {
        return NULL;
    }

    return (CDetailDelegate *)pDelegate.Get();
}

void CDetailDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    DIRUI_INFO("CDetailDelegate::OnLoadData(FrameList:%p DataIndex:%d DataCount:%d)",
               pFrameList, nDataIndex, nDataCount);

    CDirectoryControlerPtr pControler = GetControler();

    if (NULL == pFrameList || pFrameList != m_pFrameList || pControler.IsEmpty())
    {
        return;
    }

    if (!pControler->LoadDetailData(nDataIndex, nDataCount))
    {
        m_pFrameList->ClearContent(true);
    }
}

#ifdef FL_SUPPORT_TITLE
void CDetailDelegate::OnTitleAction(CFrameList * pFrameList, CFrameListTitle * pTitle, int nAction)
{
    if (NULL == pFrameList || pFrameList != m_pFrameList || NULL == pTitle)
    {
        return;
    }

    CDirectoryControlerPtr pControler = GetControler();
    CDirUIHelperPtr pUIHelper = GetUIHelper();

    if (pControler.IsEmpty() || pUIHelper.IsEmpty())
    {
        return;
    }

    int nCtrlAction = pUIHelper->GetActionByTitle(m_nState, nAction);
    int nContactId = knInvalidRId;

    CCommonDirCtrl * pCommonDirCtrl = CCommonDirCtrl::GetCommonControler(pControler);

    if (NULL != pCommonDirCtrl)
    {
        nContactId = pCommonDirCtrl->GetContactId();
    }
    else
    {
        CLDAPContactControler * pLDAPControler = CLDAPContactControler::GetControler(pControler);

        if (NULL != pLDAPControler)
        {
            nContactId = pLDAPControler->GetCurIndex();
        }
        else
        {
            CBTUCContactControler * pBTUCContact = CBTUCContactControler::GetControler(pControler);

            if (NULL != pBTUCContact)
            {
                nContactId = pBTUCContact->GetContactId();
            }
        }
    }

    if (DIR_ACTION_NONE != nCtrlAction)
    {
        pControler->OnAction(nCtrlAction, -1, nContactId);
    }
}
#endif
