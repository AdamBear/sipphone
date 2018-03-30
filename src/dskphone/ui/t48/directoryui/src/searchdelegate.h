#ifndef __SEARCH_DELEGATE_H__
#define __SEARCH_DELEGATE_H__

#include "directorycommon.h"
#include "directorylistdelegate.h"
#include "baseui/framelistdelegate.h"


class CSearchDelegate : public CDirectoryListDelegate
{
public:
    explicit CSearchDelegate();
    virtual ~CSearchDelegate();

    static CSearchDelegate * GetDirDelegate(CDirectoryListDelegatePtr & pDelegate);

public:
    bool Init(int nType, int nGroupId, const yl::string & strSearchKey);
    void OnSearchChange(const yl::string & strSearchKey);

    const yl::string & GetSearchKey() const;

protected:
    bool OnBeginSearch();

protected:
    // 搜索字符串
    yl::string m_strSearchKey;
};

#endif
