#ifndef __PRECISE_SEARCH_CONTROLL_BASE__
#define __PRECISE_SEARCH_CONTROLL_BASE__

#include "commondirctrl.h"
enum
{
    PRECISE_SEARCH_STATE_NONE = 0,        // 非高级搜索状态
    PRECISE_SEARCH_STATE_ENTER = 1,        // 进入高级搜索状态
    PRECISE_SEARCH_STATE_WAITING = 2,        // 高级搜索等待中
    PRECISE_SEARCH_STATE_RESULT = 3,        // 高级搜索结果返回
    PRECISE_SEARCH_STATE_WAITING_VIEW_MORE = 4, // 高级搜索等待更多
};

class CPreciseSearchControlBase : public CCommonDirCtrl
{
public:
    CPreciseSearchControlBase(int nDirType, DIR_TYPE eLogicType);
    virtual ~CPreciseSearchControlBase();

    static CPreciseSearchControlBase * GetPreciseSearchControl(CDirectoryControlerPtr & pControler);
public:
    virtual bool OnBack() = 0;
    virtual void OnEnterPreciseSearch() = 0;
    virtual bool PreciseSearch() = 0;
    bool IsPreciseSearchResult() const;
    bool IsPreciseSearchCondition() const;
    void EnterPreciseSearch();
    int GetPreciseSearchState() const;
    void AdjustPreciseState(int nState);
protected:
    int m_nPreciseSearchState;
};

#endif //__PRECISE_SEARCH_CONTROLL_BASE__
