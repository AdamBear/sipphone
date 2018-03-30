#include "settinguibasepagedata.h"
#include "settinguibaseitemdata.h"


CSettingUIPageDataBase::CSettingUIPageDataBase(int ePageDataType)
    : m_ePageDataType(ePageDataType)
    , m_nFocusIndex(0)
    , m_nPageDataIndex(0)
    , m_bReturnToIdle(true)
    , m_bLock(false)
    , m_bNetworkChangedReboot(false)
    , m_bEnable(true)
    , m_bRefreshPrevousView(false)
    , m_strPageID("")
    , m_strPageAuthorid("")
    , m_strTitle("")
    , m_strEmptyHint("")
    , m_strAction("")
    , m_strFocusItemID("")
    , m_pSoftkeyData(NULL)
#if IF_PAGEDATA_SUPPORT_ITEM_PROPERTY
    , m_nPageItemCount(0)
    , m_nItemHeight(0)
#endif
{
}

CSettingUIPageDataBase::~CSettingUIPageDataBase()
{
    ClearItemData();
    ClearSoftkeyData();
    ClearMsgData();
}

void CSettingUIPageDataBase::ClearItemData()
{
    int nTotal = m_vecItemData.size();

    for (int i = 0; i < nTotal; ++i)
    {
        CSettingUIItemDataBase * pItemData = m_vecItemData[i];

        if (NULL != pItemData)
        {
            delete pItemData;
        }
    }

    m_vecItemData.clear();
    m_vecShowIndex.clear();
}

void CSettingUIPageDataBase::ClearSoftkeyData()
{
    if (NULL != m_pSoftkeyData)
    {
        delete m_pSoftkeyData;
        m_pSoftkeyData = NULL;
    }
}
#if IF_PAGEDATA_SUPPORT_ITEM_PROPERTY
int CSettingUIPageDataBase::GetPageItemCount()
{
    return m_nPageItemCount;
}

void CSettingUIPageDataBase::SetPageItemCount(int nPageItemCount)
{
    m_nPageItemCount = nPageItemCount;
}

int CSettingUIPageDataBase::GetItemHeight()
{
    return m_nItemHeight;
}

void CSettingUIPageDataBase::SetItemHeight(int nItemHeight)
{
    m_nItemHeight = nItemHeight;
}
#endif

void CSettingUIPageDataBase::ClearMsgData()
{
    LIST_SETTING_MSG_DATA::iterator iter = m_listPageMessageData.begin();

    for (; iter != m_listPageMessageData.end(); ++iter)
    {
        CSettingUIMessageData * pMsgData = *iter;

        if (NULL != pMsgData)
        {
            delete pMsgData;
        }
    }

    m_listPageMessageData.clear();
}

void CSettingUIPageDataBase::Reset()
{
    m_ePageDataType = -1;
    m_nFocusIndex = 0;
    m_nPageDataIndex = 0;
    m_bReturnToIdle = true;
    m_bLock = false;
    m_bNetworkChangedReboot = false;
    m_bEnable = true;
    m_strPageID = "";
    m_strTitle = "";
    m_strEmptyHint = "";
    m_strFocusItemID = "";

    ClearItemData();
    ClearSoftkeyData();
    ClearMsgData();
}

bool CSettingUIPageDataBase::SetPageData(CSettingUIPageDataBase & pageSaveData)
{
    return true;
}

bool CSettingUIPageDataBase::GetPageData(CSettingUIPageDataBase & pageDatas)
{
    if (this != &pageDatas)
    {
        pageDatas = *this;
    }
    return true;
}

bool CSettingUIPageDataBase::LoadPageData(CSettingUIPageDataBase & pageDatas)
{
    return true;
}

bool CSettingUIPageDataBase::SavePageData(const CSettingUIPageDataBase & pageDatas)
{
    return true;
}

CSettingUISoftkeyData * CSettingUIPageDataBase::GetListSoftkeyData()
{
    return m_pSoftkeyData;
}

LIST_SETTING_MSG_DATA CSettingUIPageDataBase::GetListPageMessageData()
{
    return m_listPageMessageData;
}

LIST_SETTING_MSG_DATA* CSettingUIPageDataBase::GetPointListPageMessageData()
{
    return &m_listPageMessageData;
}

bool CSettingUIPageDataBase::AddPageMessageData(CSettingUIMessageData * pMsgData)
{
    if (NULL == pMsgData)
    {
        return false;
    }

    m_listPageMessageData.push_back(pMsgData);
    return true;
}

bool CSettingUIPageDataBase::DelPageMessageData(CSettingUIMessageData * pMsgData)
{
    return true;
}

void CSettingUIPageDataBase::UpdateShowIndex()
{
    m_vecShowIndex.clear();

    int nSize = m_vecItemData.size();

    for (int i = 0; i < nSize; ++i)
    {
        CSettingUIItemDataBase * pItemData = m_vecItemData[i];

        if (NULL != pItemData && pItemData->IsShow())
        {
            m_vecShowIndex.push_back(i);
        }
    }
    UpdateFocusIndex();
}

void CSettingUIPageDataBase::UpdateFocusIndex()
{
    if (!m_strFocusItemID.empty())
    {
        int nIndex = GetItemDataIndexByItemId(m_strFocusItemID);
        if (-1 != nIndex)
        {
            SetFocusIndex(nIndex);
        }
        m_strFocusItemID = "";
    }
}

bool CSettingUIPageDataBase::AddItem(CSettingUIItemDataBase* pItemData)
{
    if (NULL == pItemData)
    {
        return false;
    }

    // 检测Item项是否已存在
    int nTotal = m_vecItemData.size();
    for (int i = 0; i < nTotal; ++i)
    {
        if (NULL != m_vecItemData[i] && pItemData == m_vecItemData[i])
        {
            return false;
        }
    }

    // 读取所有数据
    m_vecItemData.push_back(pItemData);

    int nIndex = m_vecItemData.size() - 1;

    m_vecShowIndex.push_back(nIndex);

    return true;
}

bool CSettingUIPageDataBase::DelItem(CSettingUIItemDataBase * pItemData,
                                     bool bUpdateShowIndex/* = true*/)
{
    if (NULL == pItemData)
    {
        return false;
    }

    int nSize = m_vecItemData.size();

    for (int i = 0; i < nSize; ++i)
    {
        CSettingUIItemDataBase * pTmpItemData = m_vecItemData[i];

        if (pTmpItemData == pItemData)
        {
            bool bShowItemData = pItemData->IsShow();

            m_vecItemData.removeAt(i);
            delete pItemData;

            if (bShowItemData && bUpdateShowIndex)
            {
                UpdateShowIndex();
            }

            return true;
        }
    }

    return false;
}

void CSettingUIPageDataBase::DelFromItem(const yl::string & strItemId)
{
    int nSize = m_vecItemData.size();
    bool bDelete = false;

    for (int i = 0, j = 0; i < nSize && j < m_vecItemData.size(); ++i)
    {
        CSettingUIItemDataBase * pItemData = m_vecItemData[j];

        if (bDelete)
        {
            m_vecItemData.removeAt(j);

            if (NULL != pItemData)
            {
                delete pItemData;
            }

            continue;
        }

        if (NULL == pItemData || strItemId != pItemData->GetItemID())
        {
            ++j;
            continue;
        }

        ++j;
        bDelete = true;
    }

    UpdateShowIndex();
}

CSettingUIItemDataBase * CSettingUIPageDataBase::GetItemByDataIndex(int nDataIndex)
{
    if (nDataIndex < 0)
    {
        return NULL;
    }

    int nShowIndexSize = m_vecShowIndex.size();
    int nRealIndex = -1;

    if (nDataIndex >= 0 && nDataIndex < nShowIndexSize)
    {
        nRealIndex = m_vecShowIndex[nDataIndex];
    }

    if (-1 == nRealIndex)
    {
        nRealIndex = nDataIndex;
    }

    int nItemSize = m_vecItemData.size();

    if (nRealIndex < 0 || nRealIndex >= nItemSize)
    {
        return NULL;
    }

    return m_vecItemData[nRealIndex];
}

CSettingUIItemDataBase * CSettingUIPageDataBase::GetItemByItemId(const yl::string & strItemId)
{
    int nSize = m_vecItemData.size();

    for (int i = 0; i < nSize; ++i)
    {
        CSettingUIItemDataBase * pItemData = m_vecItemData[i];

        if (NULL != pItemData && pItemData->IsShow() && 0 == pItemData->GetItemID().compare(strItemId))
        {
            return pItemData;
        }
    }

    return NULL;
}

int CSettingUIPageDataBase::GetItemDataIndexByItemId(const yl::string & strItemId)
{
    int nShowIndexSize = m_vecShowIndex.size();
    int nItemSize = m_vecItemData.size();

    for (int i = 0; i < nShowIndexSize; ++i)
    {
        CSettingUIItemDataBase * pItemData = NULL;
        int nIndex = m_vecShowIndex[i];

        if (nIndex >= 0 && nIndex < nItemSize)
        {
            pItemData = m_vecItemData[nIndex];
        }

        if (NULL != pItemData && 0 == pItemData->GetItemID().compare(strItemId))
        {
            return i;
        }
    }

    return -1;
}

void CSettingUIPageDataBase::SetItemDataValueByDataIndex(int nDataIndex,
        const yl::string & strValue)
{
    int nShowIndexSize = m_vecShowIndex.size();
    int nRealIndex = -1;

    if (nDataIndex >= 0 && nDataIndex < nShowIndexSize)
    {
        nRealIndex = m_vecShowIndex[nDataIndex];
    }

    if (-1 == nRealIndex)
    {
        nRealIndex = nDataIndex;
    }

    CSettingUIItemDataBase * pItemData = NULL;

    if (nRealIndex >= 0 && nRealIndex < m_vecItemData.size())
    {
        pItemData = m_vecItemData[nRealIndex];
    }

    if (NULL != pItemData)
    {
        pItemData->SetValue(strValue);
    }
}

int CSettingUIPageDataBase::GetPageDataType()
{
    return m_ePageDataType;
}

void CSettingUIPageDataBase::SetPageDataType(int ePageDataType)
{
    m_ePageDataType = ePageDataType;
}

int CSettingUIPageDataBase::GetFocusIndex()
{
    return m_nFocusIndex;
}

void CSettingUIPageDataBase::SetFocusIndex(int nFocusIndex)
{
    m_nFocusIndex = nFocusIndex;
}

void CSettingUIPageDataBase::SetFocusIndexByItemID(const yl::string& strItemID)
{
    m_strFocusItemID = strItemID;
}

void CSettingUIPageDataBase::SetPageDataIndex(int nPageDataIndex)
{
    m_nPageDataIndex = nPageDataIndex;
}

int CSettingUIPageDataBase::GetPageDataIndex()
{
    return m_nPageDataIndex;
}

int CSettingUIPageDataBase::GetTotalItemNumber()
{
    return m_vecShowIndex.size();
}

bool CSettingUIPageDataBase::IsReturnToIdle()
{
    return m_bReturnToIdle;
}

void CSettingUIPageDataBase::SetReturnToIdle(bool bReturnToIdlee)
{
    m_bReturnToIdle = bReturnToIdlee;
}

bool CSettingUIPageDataBase::IsLock()
{
    return m_bLock;
}

void CSettingUIPageDataBase::SetLock(bool bLock)
{
    m_bLock = bLock;
}

bool CSettingUIPageDataBase::IsNetworkChangedReboot()
{
    return m_bNetworkChangedReboot;
}

void CSettingUIPageDataBase::SetNetworkChangedReboot(bool bNetworkChangedReboot)
{
    m_bNetworkChangedReboot = bNetworkChangedReboot;
}

bool CSettingUIPageDataBase::IsEnable()
{
    return m_bEnable;
}

void CSettingUIPageDataBase::SetEnable(bool bEnable)
{
    m_bEnable = bEnable;
}

bool CSettingUIPageDataBase::IsRefreshPrevousView()
{
    return m_bRefreshPrevousView;
}

void CSettingUIPageDataBase::SetRefreshPrevousView(bool bRefresh)
{
    m_bRefreshPrevousView = bRefresh;
}

const yl::string & CSettingUIPageDataBase::GetPageID()
{
    return m_strPageID;
}

void CSettingUIPageDataBase::SetPageID(const yl::string & strPageID)
{
    m_strPageID = strPageID;
}

const yl::string & CSettingUIPageDataBase::GetPageAuthorid()
{
    return m_strPageAuthorid;
}

void CSettingUIPageDataBase::SetPageAuthorid(const yl::string & strPageAuthorid)
{
    m_strPageAuthorid = strPageAuthorid;
}

const yl::string & CSettingUIPageDataBase::GetPageTitle()
{
    return m_strTitle;
}

void CSettingUIPageDataBase::SetPageTitle(const yl::string & strPageTitle)
{
    m_strTitle = strPageTitle;
}

const yl::string & CSettingUIPageDataBase::GetEmptyHint()
{
    return m_strEmptyHint;
}

void CSettingUIPageDataBase::SetEmptyHint(const yl::string & strHint)
{
    m_strEmptyHint = strHint;
}

const yl::string & CSettingUIPageDataBase::GetAction()
{
    return m_strAction;
}

void CSettingUIPageDataBase::SetAction(const yl::string & strAction)
{
    m_strAction = strAction;
}

int CSettingUIPageDataBase::GetTotalItemNumberInMenory()
{
    return m_vecItemData.size();
}

CSettingUIItemDataBase * CSettingUIPageDataBase::GetItemByDataIndexFromMenory(int nDataIndex)
{
    int nSize = m_vecItemData.size();

    if (nDataIndex < 0 || nDataIndex >= nSize)
    {
        return NULL;
    }

    return m_vecItemData[nDataIndex];
}

