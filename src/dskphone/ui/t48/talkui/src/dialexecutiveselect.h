#ifndef __DIAL_EXECUTIVE_ASSISTANT_SELECT_H__
#define __DIAL_EXECUTIVE_ASSISTANT_SELECT_H__

#include <features-define.h>
#include <config.h>

#include <string>
#include <vector>
#include "baseui/framelistdelegate.h"
#include "uicommon/messagebox/messageboxcallback.h"
#include "uicommon/messagebox/messageboxbase.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT


typedef std::vector<yl::string> VEC_DETAIL_VALUE;
typedef std::vector<QString>    VEC_DETAIL_TITLE;

class CExecutiveSelector :
    public CFrameListDelegate, public CMsgBoxCallBackAction
{
public:
    CExecutiveSelector();
    virtual ~CExecutiveSelector() {}

public:
    //弹出框回调函数
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

    void Init(int iLineId, int iSessionId);

    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

private:
    VEC_DETAIL_VALUE    m_vecData;
    VEC_DETAIL_TITLE    m_vecDspData;
    int                 m_nSelIndex;
    int                 m_iSessionId;
    int                 m_iLineId;
};
#endif

#endif
