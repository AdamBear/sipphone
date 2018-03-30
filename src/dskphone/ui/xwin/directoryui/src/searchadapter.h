#ifndef __SEARCH_ADAPTER_H__
#define __SEARCH_ADAPTER_H__

#include "directorycommon.h"
#include "directoryadapter.h"


class CSearchAdapter : public CDirectoryAdapter
{
public:
    explicit CSearchAdapter();
    virtual ~CSearchAdapter();

    static CSearchAdapter * GetSearchAdapter(CDirectoryAdapterPtr & pAdapter);

public:
    virtual bool Init(int nDirType, int nGroupId = knInvalidRId);

    virtual bool IsShowPageInfo();

    virtual bool GetTitle(yl::string & strTitle);

    virtual bool GetStateSoftKey(xListViewItem * pFocusItem, CArraySoftKeyBarData & vecSoftKey);

    virtual void InitListViewLayout(xListView * pListView);

    virtual bool PrecheckDataIndex(int & nDataIndex);

    void OnSearchChange(const yl::string & strSearchKey);

    int GetSearchGroupId() const;
    const yl::string & GetSearchKey() const;

protected:
    bool OnBeginSearch();

protected:
    // 搜索群组Id
    int m_nSearchGroupId;
    // 搜索字符串
    yl::string m_strSearchKey;
};

#endif
