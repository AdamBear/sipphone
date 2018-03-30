#ifndef __FRAME_SEARCH_TITLE_H__
#define __FRAME_SEARCH_TITLE_H__

#include "frametiptitle.h"

enum
{
    FRAME_LIST_TITLE_TYPE_SEARCH_TITLE          = FRAME_LIST_TITLE_TYPE_NONE + 2,
};

class CFrameSearchTitle : public CFrameTipTitle
{
    Q_OBJECT

public:
    explicit CFrameSearchTitle(CFrameList * pFrameList = NULL, IFrameTitleAction * pAction = NULL);
    virtual ~CFrameSearchTitle(void);

    static bool IsSearchTitle(CFrameListTitlePtr pTitle);

public:
    void SetSearchAmount(int nAmount);
};
typedef CFrameSearchTitle * CFrameSearchTitlePtr;

#endif
