#ifndef __I_FRAME_TITLE_ACTION_H__
#define __I_FRAME_TITLE_ACTION_H__

class CFrameList;
class CFrameListTitle;

enum LIST_TITLE_ACTION
{
    LIST_TITLE_ACTION_BASE      = 0,            // 标题栏ACTION
};

class IFrameTitleAction
{
public:
    IFrameTitleAction() {};
    virtual ~IFrameTitleAction() {};

public:
    // 标题框Action处理
    virtual void OnTitleAction(CFrameList * pFrameList, CFrameListTitle * pTitle, int nAction)    = 0;
};

#endif
