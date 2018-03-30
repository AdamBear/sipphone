#include "browserdelegate.h"
#include "directorycontroler.h"
#include "bsftcontactcontroler.h"
#include "bsftcontactuihelper.h"
#include "remotecontactcontroler.h"
#include "remoteuihelper.h"
#include "contacts/directory/include/directorymessagedefine.h"
#include "baseui/framelist.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>

#if IF_FEATURE_MUTI_CALLLOG
#include "bsftcallloguihelper.h"
#include "bsftcalllogcontroler.h"
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
#include "mtswcontactcontroler.h"
#include "mtswcontactuihelper.h"
#endif

#if IF_FEATURE_GBBOOK
#include "genbandcontactcontroler.h"
#include "genbandcontactuihelper.h"
#endif

CBrowserDelegate::CBrowserDelegate()
    : CDirectoryListDelegate(DIR_UI_STATE_BROWSER)
{
}

CBrowserDelegate::~CBrowserDelegate()
{
}

CBrowserDelegate * CBrowserDelegate::GetDirDelegate(CDirectoryListDelegatePtr & pDelegate)
{
    if (pDelegate.IsEmpty() || DIR_UI_STATE_BROWSER != pDelegate->GetState())
    {
        return NULL;
    }

    return (CBrowserDelegate *)pDelegate.Get();
}

bool CBrowserDelegate::OnChangeGroup(int nGroupId, bool bRefresh/* = true*/)
{
    CDirectoryControlerPtr pControler = GetControler();

    if (pControler.IsEmpty())
    {
        return false;
    }


    int nCurGroupId = pControler->GetGroupId();

    pControler->SetGroupId(nGroupId);

    if (nCurGroupId != nGroupId)
    {
        m_nPageDataIndex = 0;
    }

    CDirUIHelperPtr pUIHelper = GetUIHelper();

    if (!pUIHelper.IsEmpty())
    {
        pUIHelper->OnGroupChange();
    }

    if (!pControler->PreLoadContact(nGroupId))
    {
        return true;
    }

    if (bRefresh)
    {
        RefreshFrameList();
    }

    return true;
}

bool CBrowserDelegate::OnBack()
{
    CDirectoryControlerPtr pControler = GetControler();
    CRemoteContactControler * pRemoteContact = CRemoteContactControler::GetControler(pControler);

    if (NULL != pRemoteContact)
    {
        int nGroupId = pRemoteContact->GetGroupId();

        if (pRemoteContact->OnBack())
        {
            CRemoteUIHelper * pRemoteUIHelper = pRemoteContact->GetRemoteUIHelper();

            if (NULL != pRemoteUIHelper)
            {
                pRemoteUIHelper->OnGroupChange();
            }

            int nIndex = pRemoteContact->GetSubMenuIndexbyId(nGroupId);

            if (nIndex < 0)
            {
                nIndex = 0;
            }

            SetPageDataIndex(nIndex);
            RefreshFrameList();
            return true;
        }
    }

    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

    if (NULL != pBSFTContact)
    {
        if (pBSFTContact->OnBack())
        {
            CBSFTContactUIHelper * pBSFTUIHelper = pBSFTContact->GetBSFTUIHelper();

            if (NULL != pBSFTUIHelper)
            {
                pBSFTUIHelper->OnGroupChange();
            }

            SetPageDataIndex(0);
            RefreshFrameList();
            return true;
        }
    }

#if IF_FEATURE_MUTI_CALLLOG
    CBSFTCallLogControler * pBSFTLog = CBSFTCallLogControler::GetControler(pControler);

    if (NULL != pBSFTLog)
    {
        if (pBSFTLog->OnBack())
        {
            CBSFTCallLogUIHelper * pBSFTLogUIHelper = pBSFTLog->GetBSFTCallLogUIHelper();

            if (NULL != pBSFTLogUIHelper)
            {
                pBSFTLogUIHelper->OnGroupChange();
            }

            SetPageDataIndex(0);
            RefreshFrameList();
            return true;
        }
    }
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
    CMTSWContactControler * pMTSWContact = CMTSWContactControler::GetControler(pControler);

    if (NULL != pMTSWContact)
    {
        if (pMTSWContact->OnBack())
        {
            CMTSWContactUIHelper * pMTSWUIHelper = pMTSWContact->GetMTSWUIHelper();

            if (NULL != pMTSWUIHelper)
            {
                pMTSWUIHelper->OnGroupChange();
            }

            SetPageDataIndex(0);
            RefreshFrameList();
            return true;
        }
    }
#endif

#if IF_FEATURE_GBBOOK
    CGenbandContactControler * pGBContact = CGenbandContactControler::GetControler(pControler);

    if (NULL != pGBContact)
    {
        if (pGBContact->OnBack())
        {
            CGenbandContactUIHelper * pGBUIHelper = pGBContact->GetGenbandUIHelper();

            if (NULL != pGBUIHelper)
            {
                pGBUIHelper->OnGroupChange();
            }

            SetPageDataIndex(0);
            RefreshFrameList();
            return true;
        }
    }
#endif

    return false;
}
