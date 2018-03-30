#include "framesearchtitle.h"
#include "uikernel/languagehelper.h"

CFrameSearchTitle::CFrameSearchTitle(CFrameList * pFrameList/* = NULL*/,
                                     IFrameTitleAction * pAction/* = NULL*/)
    : CFrameTipTitle(pFrameList, pAction)
{
    setObjectName("CFrameSearchTitle");
    m_nType = FRAME_LIST_TITLE_TYPE_SEARCH_TITLE;
}

CFrameSearchTitle::~CFrameSearchTitle(void)
{
}

bool CFrameSearchTitle::IsSearchTitle(CFrameListTitlePtr pTitle)
{
    if (NULL != pTitle && FRAME_LIST_TITLE_TYPE_SEARCH_TITLE == pTitle->GetType())
    {
        return true;
    }

    return false;
}

void CFrameSearchTitle::SetSearchAmount(int nAmount)
{
    QString strTip = LANG_TRANSLATE(TRID_SEARCH_RESULT);

    strTip += QString(": %1").arg(nAmount);

    SetTip(strTip);
}
