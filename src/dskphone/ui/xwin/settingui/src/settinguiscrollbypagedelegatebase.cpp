#include "settinguiscrollbypagedelegatebase.h"
#include "uicallbackfun.h"
#include "keymap.h"

namespace
{
#define PAGE_ITEM_COUNT_NUMBER 4
}

CSettinguiScrollByPageDelegateBase::CSettinguiScrollByPageDelegateBase()
    : m_nCurrentPage(0)
{

}

CSettinguiScrollByPageDelegateBase::~CSettinguiScrollByPageDelegateBase()
{

}

CSettingUIDelegateBase * CSettinguiScrollByPageDelegateBase::CreatScrollByPageDelegateBase()
{
    CSettinguiScrollByPageDelegateBase * pDelegate = new CSettinguiScrollByPageDelegateBase;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettinguiScrollByPageDelegateBase::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if (PHONE_KEY_UP == nKeyCode || PHONE_KEY_DOWN == nKeyCode)
    {
        int nTotalPage = GetTotalPage();

        if (nTotalPage <= 1)
        {
            return true;
        }

        if (PHONE_KEY_UP == nKeyCode)
        {
            m_nCurrentPage--;
        }
        else
        {
            m_nCurrentPage++;
        }

        if (m_nCurrentPage >= nTotalPage)
        {
            m_nCurrentPage = 0;
        }

        if (m_nCurrentPage < 0)
        {
            m_nCurrentPage = nTotalPage - 1;
        }

        g_SetUICallBackFun.RefreshUI(false, GetPageID());
        return true;
    }

    return false;
}

int CSettinguiScrollByPageDelegateBase::GetTotalPage()
{
    int nSize = m_vecText.size();
    int nTotalPage = nSize / PAGE_ITEM_COUNT_NUMBER;
    if (0 != nSize % PAGE_ITEM_COUNT_NUMBER)
    {
        nTotalPage++;
    }

    return nTotalPage;
}

int CSettinguiScrollByPageDelegateBase::getCurrentPage()
{
    return m_nCurrentPage;
}

int CSettinguiScrollByPageDelegateBase::GetTotalItemCount()
{
    return m_vecText.size();
}

yl::string CSettinguiScrollByPageDelegateBase::GetValueByIndex(int nIndex)
{
    yl::string strValue = "";

    int nRealIndex = PAGE_ITEM_COUNT_NUMBER * m_nCurrentPage + nIndex;
    if (nRealIndex < m_vecText.size() && nRealIndex >= 0)
    {
        strValue = m_vecText[nRealIndex];
    }

    return strValue;
}
