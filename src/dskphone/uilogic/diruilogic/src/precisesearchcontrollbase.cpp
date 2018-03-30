#include "precisesearchcontrollbase.h"
#include "directorycommon.h"

CPreciseSearchControlBase::CPreciseSearchControlBase(int nDirType, DIR_TYPE eLogicType)
    : CCommonDirCtrl(nDirType, eLogicType)
    , m_nPreciseSearchState(PRECISE_SEARCH_STATE_NONE)
{

}

CPreciseSearchControlBase::~CPreciseSearchControlBase()
{

}

CPreciseSearchControlBase* CPreciseSearchControlBase::GetPreciseSearchControl(CDirectoryControlerPtr & pControler)
{
    if (!pControler.IsEmpty())
    {
        int nType = pControler->GetType();

        if (DIRECTORY_TYPE_BROADSOFT == nType
#if IF_FEATURE_GBBOOK
                || DIRECTORY_TYPE_GENBAND == nType
#endif
           )
        {
            return (CPreciseSearchControlBase *)pControler.Get();
        }
    }

    return NULL;
}

bool CPreciseSearchControlBase::IsPreciseSearchCondition() const
{
    return (PRECISE_SEARCH_STATE_ENTER == m_nPreciseSearchState);
}

bool CPreciseSearchControlBase::IsPreciseSearchResult() const
{
    return (PRECISE_SEARCH_STATE_RESULT == m_nPreciseSearchState);
}

void CPreciseSearchControlBase::EnterPreciseSearch()
{
    m_nPreciseSearchState = PRECISE_SEARCH_STATE_ENTER;
}

int CPreciseSearchControlBase::GetPreciseSearchState() const
{
    return m_nPreciseSearchState;
}

void CPreciseSearchControlBase::AdjustPreciseState(int nState)
{
    m_nPreciseSearchState = nState;
}
