#include "baseui/framelist.h"
#include "baseui/treelist.h"

#include "callloggroupdelegate.h"
#include "bsftcontactcontroler.h"

#include <algorithm>
#include "directorymgr.h"
#include "callloggrouptitle.h"
#include "directorycommon.h"
#include "directorylistaction.h"
#include "translateiddefine.h"
#include "grouptreeitem.h"
#include "interfacedefine.h"
#include "uikernel/languagehelper.h"
#include "moddirectoryui.h"

#if IF_FEATURE_MUTI_CALLLOG
#include "account/include/modaccount.h"
#endif

CCallLogGroupDelegate::CCallLogGroupDelegate()
    : CGroupDelegate(DIR_MOD_TYPE_CALLLOG)
{
}

CCallLogGroupDelegate::~CCallLogGroupDelegate()
{
}

CCallLogGroupDelegate * CCallLogGroupDelegate::GetGroupDelegate(CGroupDelegatePtr & pDelegate)
{
    if (pDelegate.IsEmpty() || DIR_MOD_TYPE_CALLLOG != pDelegate->GetType())
    {
        return NULL;
    }

    return (CCallLogGroupDelegate *)pDelegate.Get();
}

int CCallLogGroupDelegate::GetDefaultDirType() const
{
    return CALL_LOG_TYPE_LOCAL;
}

LRESULT CCallLogGroupDelegate::OnMessage(msgObject & refMessage)
{
    LRESULT retValue = FALSE;

    if (BROADSOFT_CALLLOG_RELOAD == refMessage.message)
    {
        RefreshBroadSoftGroup();
    }
#if IF_FEATURE_METASWITCH_CALLLOG
    else if (METASWITCH_CALLLOG_RELOAD == refMessage.message)
    {
        RefreshMetaSwitchGroup();
    }
#endif

    return retValue;
}

void CCallLogGroupDelegate::SetReadOnly(bool bReadOnly)
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();

    if (NULL == pTitle || !CCallLogGroupTitle::IsCallLogGroupTitle(pTitle))
    {
        return;
    }

    CCallLogGroupTitlePtr pGroupTitle = (CCallLogGroupTitlePtr)pTitle;

    pGroupTitle->SetBtnVisible(!bReadOnly);

#if IF_FEATURE_METASWITCH_DIRECTORY
    if (!LocalLog_IsEnable())
    {
        pGroupTitle->SetBtnVisible(false);
    }
#endif
}

// Id容错
int CCallLogGroupDelegate::AdjustGroupId(int nDirType, int nGroupId)
{
    CGroupTreeNodePtr pGroupNode = GetGroupNode(nDirType, nGroupId);

    if (NULL != pGroupNode)
    {
        return nGroupId;
    }

    if (DIRECTORY_TYPE_NONE == nDirType)
    {
        return knInvalidRId;
    }

    return kszAllContactId;
}

void CCallLogGroupDelegate::AdjustDirType(int & nDirType, int & nGroupId)
{
    if (CALL_LOG_TYPE_BROADSOFT != nDirType
            && NULL != GetTreeDataNode(CALL_LOG_GROUP_ID_ALL_NETWORK_CALL))
    {
        nDirType = CALL_LOG_TYPE_BROADSOFT;
        nGroupId = kszAllContactId;
        return;
    }

    if (CALL_LOG_TYPE_LOCAL != nDirType && NULL != GetTreeDataNode(CALL_LOG_GROUP_ID_ALL_LOCAL_CALL))
    {
        nDirType = CALL_LOG_TYPE_LOCAL;
        nGroupId = kszAllContactId;
        return;
    }

#if IF_FEATURE_METASWITCH_CALLLOG
    if (CALL_LOG_TYPE_MTSW != nDirType
            && NULL != GetTreeDataNode(CALL_LOG_GROUP_ID_ALL_METASWITCH_CALL))
    {
        nDirType = CALL_LOG_TYPE_MTSW;
        nGroupId = kszAllContactId;
        return;
    }
#endif

    nDirType = DIRECTORY_TYPE_NONE;
    nGroupId = knInvalidRId;
}

int CCallLogGroupDelegate::GetDefaultGroupId(int nDirType, int nGroupId)
{
#if IF_FEATURE_MUTI_CALLLOG
    if (nGroupId == -1 && BsftLog_IsMutilLogEnable())
    {
        return BsftLog_GetGroupIDByAccountID(acc_GetDefaultAccount());
    }
#endif
    return nGroupId;
}

void CCallLogGroupDelegate::Init()
{
    m_nUISelectId = 0;

    InitGroupId(CALL_LOG_TYPE_LOCAL, CALL_LOG_GROUP_ID_LOCAL_BEGIN, CALL_LOG_GROUP_ID_LOCAL_END);
    InitGroupId(CALL_LOG_TYPE_BROADSOFT, CALL_LOG_GROUP_ID_BROADSOFT_BEGIN,
                CALL_LOG_GROUP_ID_BROADSOFT_END);
#if IF_FEATURE_METASWITCH_CALLLOG
    InitGroupId(CALL_LOG_TYPE_MTSW, CALL_LOG_GROUP_ID_METASWITCH_BEGIN,
                CALL_LOG_GROUP_ID_METASWITCH_END);
#endif
    InitBroadSoftGroup();
    InitLocalGroup();
#if IF_FEATURE_METASWITCH_CALLLOG
    InitMetaSwitchGroup();
#endif

    InitGroupList();
}

void CCallLogGroupDelegate::InitLocalGroup()
{
    ResetGroupTreeId(CALL_LOG_TYPE_LOCAL);

#if IF_FEATURE_METASWITCH_CALLLOG
    // 没有配置，就不加到群组中
    if (!LocalLog_IsEnable())
    {
        return;
    }
#endif

    // 没有显示权限，就不加到群组中
    if (!CDirectoryControler::IsUserLevelShow(CALL_LOG_TYPE_LOCAL))
    {
        return ;
    }

    // 初始化根节点
    QString strRootGroup = LANG_TRANSLATE(TRID_LOCAL_CALLLOG);
    yl::string strAllContactGroup = strRootGroup.toUtf8().data();
    int nRootId = ApplyGroupTreeId(CALL_LOG_TYPE_LOCAL);
    CGroupTreeNodePtr pGroupNode = new CGroupTreeNode(nRootId, 0, true, CALL_LOG_TYPE_LOCAL,
            kszAllContactId);

    if (NULL == pGroupNode)
    {
        return;
    }

    pGroupNode->SetName(strAllContactGroup);
    m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;

    YLVector<int> vecGroupId;

    if (!Dir_GetConfigMenuIdList(vecGroupId, LOCAL_LOG))
    {
        return;
    }

    int nSize = vecGroupId.size();

    for (int i = 0; i < nSize; ++i)
    {
        int nGroupId = vecGroupId[i];

        if (kszAllContactId == nGroupId)
        {
            continue;
        }

        SmartPtr pDetail = Dir_GetDetailById(nGroupId, LOCAL_LOG);

        if (pDetail.IsEmpty())
        {
            continue;
        }

        yl::string strGroupName = pDetail->GetDisplayName();
        int nGroupTreeId = ApplyGroupTreeId(CALL_LOG_TYPE_LOCAL);
        pGroupNode = new CGroupTreeNode(nGroupTreeId, nRootId, false, CALL_LOG_TYPE_LOCAL, nGroupId);

        if (NULL != pGroupNode)
        {
            strGroupName = LANG_TRANSLATE(strGroupName.c_str()).toUtf8().data();
            pGroupNode->SetName(strGroupName);

            m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
        }
    }
}

void CCallLogGroupDelegate::InitBroadSoftGroup(int nAccGroupID /*= -1*/)
{
    ResetGroupTreeId(CALL_LOG_TYPE_BROADSOFT);

    // 没有配置，就不加到群组中
    if (!BsftLog_IsEnable())
    {
        return;
    }

    // 没有显示权限，就不加到群组中
    if (!CDirectoryControler::IsUserLevelShow(CALL_LOG_TYPE_BROADSOFT))
    {
        return;
    }

#if IF_FEATURE_MUTI_CALLLOG
    ListAccounts listAccount =  acc_GetAvailableAccountList();

    int nSize = listAccount.size();

    YLVector<int> vecGroupId;

    int nAvailableAccountNum = acc_GetAvailableAccountCount();

    int nRootGroupID = kszAllContactId;

    if (BsftLog_IsMutilLogEnable())
    {
        int nAccountID = 0;
        //没注册账号或者只注册一个账号。
        if (nSize <= 1)
        {
            nAccountID = nSize == 1 ? acc_GetDefaultAccount() : 0;
            nRootGroupID = BsftLog_GetGroupIDByAccountID(nAccountID);
            if (!BsftLog_IsAccountGroup(nAccGroupID))
            {
                nAccGroupID = BsftLog_GetAccGroupIDByAccountID(nAccountID);
            }
            Dir_GetChildGroupByGroupID(vecGroupId, nAccGroupID, BSFT_LOG);
            nSize = vecGroupId.size();
        }
        else
        {
            nRootGroupID = kszRootId;
        }
    }
    else
    {
        Dir_GetConfigMenuIdList(vecGroupId, BSFT_LOG);
        nSize = vecGroupId.size();
    }


    int nRootId = ApplyGroupTreeId(CALL_LOG_TYPE_BROADSOFT);

    CGroupTreeNodePtr pGroupNode = new CGroupTreeNode(nRootId, 0, (nSize > 0),
            CALL_LOG_TYPE_BROADSOFT, nRootGroupID);
#else
    YLVector<int> vecGroupId;

    Dir_GetConfigMenuIdList(vecGroupId, BSFT_LOG);
    int nSize = vecGroupId.size();
    int nRootId = ApplyGroupTreeId(CALL_LOG_TYPE_BROADSOFT);
    CGroupTreeNodePtr pGroupNode = new CGroupTreeNode(nRootId, 0, (nSize > 0), CALL_LOG_TYPE_BROADSOFT,
            kszAllContactId);
#endif

    if (NULL == pGroupNode)
    {
        return;
    }

    QString strRootGroup = LANG_TRANSLATE(TRID_NETWORK_CALLLOG);

    pGroupNode->SetName(strRootGroup.toUtf8().data());
    m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;

#if IF_FEATURE_MUTI_CALLLOG
    if (BsftLog_IsMutilLogEnable() && listAccount.size() > 1)
    {
        ListAccounts::ListIterator iter = listAccount.begin();

        for (; iter != listAccount.end(); ++iter)
        {
            int nCurAccountID = *iter;
            int nLineTreeId = ApplyGroupTreeId(CALL_LOG_TYPE_BROADSOFT);

            yl::string strGroupName = commonAPI_FormatString("Line %d", nCurAccountID + 1);

            vecGroupId.clear();
            Dir_GetChildGroupByGroupID(vecGroupId, BsftLog_GetAccGroupIDByAccountID(nCurAccountID), BSFT_LOG);
            nSize = vecGroupId.size();

            int nAllGroupID = kszAllContactId;
            nAllGroupID = BsftLog_GetGroupIDByAccountID(nCurAccountID);

            CGroupTreeNodePtr pLineNode = new CGroupTreeNode(nLineTreeId, nRootId, nSize > 0,
                    CALL_LOG_TYPE_BROADSOFT, nAllGroupID);

            if (NULL != pLineNode)
            {
                strGroupName = LANG_TRANSLATE(strGroupName.c_str()).toUtf8().data();
                pLineNode->SetName(strGroupName);

                m_mapTreeNode[pLineNode->GetId()] = pLineNode;
            }


            for (int i = 0; i < nSize; ++i)
            {
                int nGroupId = vecGroupId[i];

                if (kszAllContactId == nGroupId
                        || nAllGroupID == nGroupId)
                {
                    continue;
                }

                SmartPtr pDetail = Dir_GetDetailById(nGroupId, BSFT_LOG);

                if (pDetail.IsEmpty())
                {
                    continue;
                }

                int nGroupTreeId = ApplyGroupTreeId(CALL_LOG_TYPE_BROADSOFT);
                yl::string strGroupName = pDetail->GetDisplayName();

                pGroupNode = new CGroupTreeNode(nGroupTreeId, nLineTreeId, false, CALL_LOG_TYPE_BROADSOFT,
                                                nGroupId);

                if (NULL != pGroupNode)
                {
                    strGroupName = LANG_TRANSLATE(strGroupName.c_str()).toUtf8().data();
                    pGroupNode->SetName(strGroupName);

                    m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
                }
            }

        }
    }
    else
    {
        for (int i = 0; i < nSize; ++i)
        {
            int nGroupId = vecGroupId[i];

            if (kszAllContactId == nGroupId)
            {
                continue;
            }

#if IF_FEATURE_MUTI_CALLLOG
            if (nRootGroupID == nGroupId)
            {
                continue;
            }
#endif

            SmartPtr pDetail = Dir_GetDetailById(nGroupId, BSFT_LOG);

            if (pDetail.IsEmpty())
            {
                continue;
            }

            int nGroupTreeId = ApplyGroupTreeId(CALL_LOG_TYPE_BROADSOFT);
            yl::string strGroupName = pDetail->GetDisplayName();

            pGroupNode = new CGroupTreeNode(nGroupTreeId, nRootId, false, CALL_LOG_TYPE_BROADSOFT, nGroupId);

            if (NULL != pGroupNode)
            {
                strGroupName = LANG_TRANSLATE(strGroupName.c_str()).toUtf8().data();
                pGroupNode->SetName(strGroupName);

                m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
            }
        }
    }

#else
    for (int i = 0; i < nSize; ++i)
    {
        int nGroupId = vecGroupId[i];

        if (kszAllContactId == nGroupId)
        {
            continue;
        }

        SmartPtr pDetail = Dir_GetDetailById(nGroupId, BSFT_LOG);

        if (pDetail.IsEmpty())
        {
            continue;
        }

        int nGroupTreeId = ApplyGroupTreeId(CALL_LOG_TYPE_BROADSOFT);
        yl::string strGroupName = pDetail->GetDisplayName();

        pGroupNode = new CGroupTreeNode(nGroupTreeId, nRootId, false, CALL_LOG_TYPE_BROADSOFT, nGroupId);

        if (NULL != pGroupNode)
        {
            strGroupName = LANG_TRANSLATE(strGroupName.c_str()).toUtf8().data();
            pGroupNode->SetName(strGroupName);

            m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
        }
    }
#endif
}

#if IF_FEATURE_METASWITCH_CALLLOG
void CCallLogGroupDelegate::InitMetaSwitchGroup()
{
    ResetGroupTreeId(CALL_LOG_TYPE_MTSW);

    // 没有配置，就不加到群组中
    if (!MTSWLog_IsEnable())
    {
        return;
    }

    // 没有显示权限，就不加到群组中
    if (!CDirectoryControler::IsUserLevelShow(CALL_LOG_TYPE_MTSW))
    {
        return;
    }

    YLVector<int> vecGroupId;

    Dir_GetConfigMenuIdList(vecGroupId, MTSW_LOG);

    int nSize = vecGroupId.size();
    int nRootId = ApplyGroupTreeId(CALL_LOG_TYPE_MTSW);
    CGroupTreeNodePtr pGroupNode = new CGroupTreeNode(nRootId, 0, (nSize > 0), CALL_LOG_TYPE_MTSW,
            kszAllContactId);

    if (NULL == pGroupNode)
    {
        return;
    }

    QString strRootGroup = LANG_TRANSLATE(TRID_MTSW_CALLLOG);

    pGroupNode->SetName(strRootGroup.toUtf8().data());
    m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;

    for (int i = 0; i < nSize; ++i)
    {
        int nGroupId = vecGroupId[i];

        if (kszAllContactId == nGroupId)
        {
            continue;
        }

        SmartPtr pDetail = Dir_GetDetailById(nGroupId, MTSW_LOG);

        if (pDetail.IsEmpty())
        {
            continue;
        }

        int nGroupTreeId = ApplyGroupTreeId(CALL_LOG_TYPE_MTSW);
        yl::string strGroupName = pDetail->GetDisplayName();

        pGroupNode = new CGroupTreeNode(nGroupTreeId, nRootId, false, CALL_LOG_TYPE_MTSW, nGroupId);

        if (NULL != pGroupNode)
        {
            strGroupName = LANG_TRANSLATE(strGroupName.c_str()).toUtf8().data();
            pGroupNode->SetName(strGroupName);

            m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
        }
    }
}
#endif

void CCallLogGroupDelegate::InitGroupList()
{
    m_vecTreeId.clear();

    MAP_TREE_DATA_NODE::iterator iter = m_mapTreeNode.begin();

    for (; iter != m_mapTreeNode.end(); ++iter)
    {
        CGroupTreeNodePtr pGroupNode = (CGroupTreeNodePtr)iter->second;

        if (NULL == pGroupNode || 0 != pGroupNode->GetParentId())
        {
            continue;
        }

        int nType = pGroupNode->GetType();
        // BSFT CallLog不进入Setting
        if (CALL_LOG_TYPE_BROADSOFT == nType && m_bSetting)
        {
            continue;
        }

#if IF_FEATURE_METASWITCH_CALLLOG
        if (CALL_LOG_TYPE_MTSW == nType && m_bSetting)
        {
            continue;
        }
#endif

        m_vecTreeId.push_back(pGroupNode->GetId());
    }
}

void CCallLogGroupDelegate::RefreshLocalGroup(int nSelGroupId)
{
    // 默认节点
    if (0 == nSelGroupId)
    {
        CGroupTreeNodePtr pGroupNode = (CGroupTreeNodePtr)GetTreeDataNode(CALL_LOG_GROUP_ID_ALL_LOCAL_CALL);

        if (NULL != pGroupNode)
        {
            nSelGroupId = pGroupNode->GetGroupId();
        }
    }

    // 初始化节点
    ClearGroup(CALL_LOG_GROUP_ID_LOCAL_BEGIN, CALL_LOG_GROUP_ID_LOCAL_END);
    InitLocalGroup();

    // 初始化界面节点ID
    CGroupTreeNodePtr pGroupNode = (CGroupTreeNodePtr)GetTreeDataNode(CALL_LOG_GROUP_ID_ALL_LOCAL_CALL);

    if (NULL == pGroupNode)
    {
        return;
    }

    m_vecTreeId.push_back((int)CALL_LOG_GROUP_ID_ALL_LOCAL_CALL);

    std::sort(m_vecTreeId.begin(), m_vecTreeId.end(), std::less<int>());

    ExpandNode(pGroupNode);

    // 设定选中项
    CGroupTreeNodePtr pSelectGroupNode = GetGroupNode(CALL_LOG_TYPE_LOCAL, nSelGroupId);

    if (NULL != pSelectGroupNode)
    {
        m_nUISelectId = pSelectGroupNode->GetId();
    }
    else
    {
        m_nUISelectId = pGroupNode->GetId();
    }

    // 刷新列表
    ReLocateCurrentPage();
}

#if IF_FEATURE_MUTI_CALLLOG
void CCallLogGroupDelegate::RefreshBroadSoftGroup(int nAccGroupID)
{
    // 获取刷新前的界面信息
    bool bExpand = IsExpand(CALL_LOG_GROUP_ID_ALL_NETWORK_CALL);
    int nGroupSelectId = GetGroupSelectId(CALL_LOG_TYPE_BROADSOFT);

    // 初始化节点
    ClearGroup(CALL_LOG_GROUP_ID_BROADSOFT_BEGIN, CALL_LOG_GROUP_ID_BROADSOFT_END);

    InitBroadSoftGroup(nAccGroupID);

    // 初始化界面节点ID
    if (!m_bSetting)
    {
        // 初始化界面节点ID
        if (!ReInitGroupList(CALL_LOG_TYPE_BROADSOFT, CALL_LOG_GROUP_ID_ALL_NETWORK_CALL, bExpand,
                             nGroupSelectId)
                && (m_nUISelectId >= CALL_LOG_GROUP_ID_BROADSOFT_BEGIN
                    && m_nUISelectId <= CALL_LOG_GROUP_ID_BROADSOFT_END))
        {
            m_nUISelectId = CALL_LOG_GROUP_ID_ALL_LOCAL_CALL;
        }
    }
    else if (m_nUISelectId >= CALL_LOG_GROUP_ID_BROADSOFT_BEGIN
             && m_nUISelectId <= CALL_LOG_GROUP_ID_BROADSOFT_END)
    {
        m_nUISelectId = CALL_LOG_GROUP_ID_ALL_LOCAL_CALL;
    }

    // 刷新列表
    ReLocateCurrentPage();
}
#endif

void CCallLogGroupDelegate::RefreshBroadSoftGroup()
{
    // 获取刷新前的界面信息
    bool bExpand = IsExpand(CALL_LOG_GROUP_ID_ALL_NETWORK_CALL);
    int nGroupSelectId = GetGroupSelectId(CALL_LOG_TYPE_BROADSOFT);

    // 初始化节点
    ClearGroup(CALL_LOG_GROUP_ID_BROADSOFT_BEGIN, CALL_LOG_GROUP_ID_BROADSOFT_END);

    InitBroadSoftGroup();

    // 初始化界面节点ID
    if (!m_bSetting)
    {
        // 初始化界面节点ID
        if (!ReInitGroupList(CALL_LOG_TYPE_BROADSOFT, CALL_LOG_GROUP_ID_ALL_NETWORK_CALL, bExpand,
                             nGroupSelectId)
                && (m_nUISelectId >= CALL_LOG_GROUP_ID_BROADSOFT_BEGIN
                    && m_nUISelectId <= CALL_LOG_GROUP_ID_BROADSOFT_END))
        {
            m_nUISelectId = CALL_LOG_GROUP_ID_ALL_LOCAL_CALL;
        }
    }
    else if (m_nUISelectId >= CALL_LOG_GROUP_ID_BROADSOFT_BEGIN
             && m_nUISelectId <= CALL_LOG_GROUP_ID_BROADSOFT_END)
    {
        m_nUISelectId = CALL_LOG_GROUP_ID_ALL_LOCAL_CALL;
    }

    // 刷新列表
    ReLocateCurrentPage();
    ReSelectGroup();
}

#if IF_FEATURE_METASWITCH_CALLLOG
void CCallLogGroupDelegate::RefreshMetaSwitchGroup()
{
    // 获取刷新前的界面信息
    bool bExpand = IsExpand(CALL_LOG_GROUP_ID_ALL_METASWITCH_CALL);
    int nGroupSelectId = GetGroupSelectId(CALL_LOG_TYPE_MTSW);

    // 初始化节点
    ClearGroup(CALL_LOG_GROUP_ID_METASWITCH_BEGIN, CALL_LOG_GROUP_ID_METASWITCH_END);
    InitMetaSwitchGroup();


    // 初始化界面节点ID
    if (!m_bSetting)
    {
        // 初始化界面节点ID
        if (!ReInitGroupList(CALL_LOG_TYPE_MTSW, CALL_LOG_GROUP_ID_ALL_METASWITCH_CALL, bExpand,
                             nGroupSelectId)
                && (m_nUISelectId >= CALL_LOG_GROUP_ID_METASWITCH_BEGIN
                    && m_nUISelectId <= CALL_LOG_GROUP_ID_METASWITCH_END))
        {
            m_nUISelectId = CALL_LOG_GROUP_ID_ALL_LOCAL_CALL;
        }
    }
    else if (m_nUISelectId >= CALL_LOG_GROUP_ID_METASWITCH_BEGIN
             && m_nUISelectId <= CALL_LOG_GROUP_ID_METASWITCH_END)
    {
        m_nUISelectId = CALL_LOG_GROUP_ID_ALL_LOCAL_CALL;
    }

    // 刷新列表
    ReLocateCurrentPage();
    ReSelectGroup();
}
#endif

void CCallLogGroupDelegate::RefreshTreeListItem(int nCount, int nIndex)
{
    CALLLOGUI_DEBUG("RefreshTreeListItem(Count=%d Index=%d)", nCount, nIndex);

    CTreeList * pTreeList = (CTreeList *)m_pFrameList;

    if (NULL == pTreeList)
    {
        return;
    }

    int nMaxPageItemCount = pTreeList->GetMaxPageItemCount();

    if (0 == nCount)
    {
        nCount = nMaxPageItemCount;
    }

    int nSize = m_vecTreeId.size();

    if (nCount > nSize)
    {
        nCount = nSize;
    }

    if (nMaxPageItemCount > 0)
    {
        if (nIndex >= nSize)
        {
            nIndex = nSize - (nSize % nMaxPageItemCount);
            nCount = nSize - nIndex;
        }
    }
    else
    {
        nIndex = 0;
    }

    m_nPageDataIndex = nIndex;
    pTreeList->SetTotalDataCount(nSize);

    int nItemIndex = 0;
    int nSelectIndex = INVALID_ITEM_INDEX;
    VEC_TREE_ID::iterator iter = m_vecTreeId.begin();

    if (nIndex > 0)
    {
        std::advance(iter, nIndex);
    }

    CALLLOGUI_DEBUG("RefreshTreeListItem(Count=%d Index=%d) TreeIdSize=%d", nCount, nIndex,
                    (int)m_vecTreeId.size());

    for (; nItemIndex < nCount && iter != m_vecTreeId.end(); ++iter, ++nItemIndex)
    {
        CGroupTreeNodePtr pGroupNode = (CGroupTreeNodePtr)GetTreeDataNode(*iter);

        if (NULL == pGroupNode)
        {
            continue;
        }

        RefreshItem(nItemIndex, pGroupNode);

        if (pGroupNode->GetId() == m_nUISelectId)
        {
            nSelectIndex = nItemIndex;
        }
    }

    int nItemCount = pTreeList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemCount; ++i)
    {
        pTreeList->DelItemByIndex(nItemIndex);
    }

    pTreeList->SetItemFocusByIndex(nSelectIndex);
    pTreeList->update();
}

bool CCallLogGroupDelegate::RefreshItem(int nItemIndex, CGroupTreeNodePtr pGroupNode)
{
    CTreeList * pTreeList = (CTreeList *)m_pFrameList;

    if (NULL == pTreeList || NULL == pGroupNode)
    {
        return false;
    }

    bool bAdd = false;
    CGroupTreeItemPtr pGroupTreeItem = (CGroupTreeItemPtr)pTreeList->GetItemByIndex(nItemIndex);

    if (NULL == pGroupTreeItem)
    {
        pGroupTreeItem = new CGroupTreeItem;

        if (NULL == pGroupTreeItem)
        {
            return false;
        }
        pGroupTreeItem->setObjectName(QString("GroupTreeItem%1").arg(nItemIndex));
        bAdd = true;
    }

    pGroupTreeItem->ClearContact();

    // CListItem
    pGroupTreeItem->SetDataIndex(m_nPageDataIndex + nItemIndex);

    // CTreeItem
    pGroupTreeItem->SetHadChildren(pGroupNode->IsHadChildren());

    // 目前只有两级，可以这样判断
    pGroupTreeItem->SetExpand(pGroupNode->IsExpanded());

    if (0 != pGroupNode->GetParentId())
    {
        pGroupTreeItem->SetLevel(TREE_ITEM_LEVEL_SECOND);
    }
    else
    {
        pGroupTreeItem->SetLevel(TREE_ITEM_LEVEL_FIRST);
    }

    pGroupTreeItem->SetId(pGroupNode->GetId());
    pGroupTreeItem->SetAction(this);

    // CGroupTreeItem
    pGroupTreeItem->SetGroupName(pGroupNode->GetName());

    if (bAdd && !pTreeList->AddItem(pGroupTreeItem))
    {
        pGroupTreeItem->Release();

        return false;
    }

    return true;
}

void CCallLogGroupDelegate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem,
        int nAction)
{
    CTreeList * pTreeList = (CTreeList *)pFrameList;
    CGroupTreeItemPtr pTreeItem = (CGroupTreeItemPtr)pItem;

    if (NULL == pTreeList || NULL == pTreeItem)
    {
        return;
    }

    int nTreeId = pTreeItem->GetId();
    CGroupTreeNodePtr pGroupNode = (CGroupTreeNodePtr)GetTreeDataNode(nTreeId);

    if (NULL == pGroupNode)
    {
        return;
    }

    if (LIST_ITEM_ACTION_SELECT == nAction || LIST_ITEM_ACTION_FOCUS == nAction)
    {
        CGroupDelegatePtr pGroupDelegate = g_DirectoryMgr.GetGroupDelegate();

        if (pGroupDelegate.Get() != this)
        {
            CTreeListDelegate::OnItemAction(pFrameList, pItem, nAction);
            return;
        }

        int nDirType = pGroupNode->GetType();
        int nGroupId = pGroupNode->GetGroupId();

        if (g_DirectoryMgr.IsGroupShow(nDirType, nGroupId)
                || g_DirectoryMgr.OnGroupClick(nDirType, nGroupId))
        {
            CTreeListDelegate::OnItemAction(pFrameList, pItem, nAction);
        }
        else if (LIST_ITEM_ACTION_FOCUS != nAction)
        {
            pTreeList->SetItemFocusByIndex(INVALID_ITEM_INDEX);

            int nSelIndex = pTreeList->GetIndexById(m_nUISelectId);

            if (INVALID_ITEM_INDEX != nSelIndex)
            {
                pTreeList->SetItemFocusByIndex(nSelIndex);
            }
        }
    }
}

#if DIR_SUPPORT_NET_CALLLOG_DELETE
void CCallLogGroupDelegate::OnChangeGroup(int nDirType)
{
    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();
    if (NULL != pTitle && CCallLogGroupTitle::IsCallLogGroupTitle(pTitle))
    {
        CCallLogGroupTitlePtr pGroupTitle = (CCallLogGroupTitlePtr)pTitle;
        if (configParser_GetConfigInt(kszBsftLogDeleteEnable) == 1)
        {
            pGroupTitle->SetDeleteState(CALL_LOG_TYPE_BROADSOFT == nDirType);
        }
    }
}
#endif

void CCallLogGroupDelegate::OnTitleAction(CFrameList * pFrameList, CFrameListTitle * pTitle,
        int nAction)
{
    if (NULL == pFrameList || pFrameList != m_pFrameList || NULL == pTitle)
    {
        return;
    }

    if (LIST_TITLE_ACTION_DIR_SETTING == nAction)
    {
        g_DirectoryMgr.OnSettingClick();
    }
#if DIR_SUPPORT_NET_CALLLOG_DELETE
    else if (LIST_TITLE_ACTION_DIR_CLEAR_CROUP == nAction)
    {
        if (configParser_GetConfigInt(kszBsftLogDeleteEnable) == 1)
        {
            g_DirectoryMgr.OnClearGroupClick();
        }
    }
#endif
}

#if IF_FEATURE_MUTI_CALLLOG
bool CCallLogGroupDelegate::IsIdenticalTypeNote(int nParentID, int nTreeID)
{
    if (nParentID >= CALL_LOG_GROUP_ID_BROADSOFT_BEGIN && nParentID <= CALL_LOG_GROUP_ID_BROADSOFT_END)
    {
        return nTreeID >= CALL_LOG_GROUP_ID_BROADSOFT_BEGIN && nTreeID <= CALL_LOG_GROUP_ID_BROADSOFT_END;
    }
    else if (nParentID >= CALL_LOG_GROUP_ID_LOCAL_BEGIN && nParentID <= CALL_LOG_GROUP_ID_LOCAL_END)
    {
        return nTreeID >= CALL_LOG_GROUP_ID_LOCAL_BEGIN && nTreeID <= CALL_LOG_GROUP_ID_LOCAL_END;
    }
    return false;
}
#endif
