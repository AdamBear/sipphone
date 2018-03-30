#ifndef __FAVORITE_SELECTOR_H__
#define __FAVORITE_SELECTOR_H__

#include <ylstring.h>
#include <vector>
#include "baseui/framelistdelegate.h"

//mod-messagebox
#include "uicommon/messagebox/messageboxcallback.h"
#include "uicommon/messagebox/messageboxbase.h"

class CFavoriteSelector : public CFrameListDelegate, public CMsgBoxCallBackAction
{
public:
    CFavoriteSelector();
    virtual ~CFavoriteSelector();

public:
    //弹出框回调函数
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

public:
    void Init();
    bool IsHadFavorites();
    bool IsCanDirectEnter();
    void DirectEnterFavorite();
    void EnterSelectedFavorite();

// IFrameListDelegate
public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

// IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

protected:
    void EnterFavorite(const yl::string & strFavoriteId);
    yl::string GetFavoriteShowName(const yl::string & strFavoriteId);

protected:
    int m_nSelIndex;

    typedef std::vector<yl::string> VEC_FAVORITES_ID;

    //保存的是名称ID
    VEC_FAVORITES_ID m_vecFavorites;
    //保存的是 显示名称
    std::vector<yl::string> m_vecDisplayName;
};

#endif
