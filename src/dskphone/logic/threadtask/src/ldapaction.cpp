#ifdef USE_LDAP_IN_THREADTASKASST___
#include "ldapaction.h"
#include "download_lib_include/ldap/yealink_ldap.h"
#include <ETLLib/ETLLib.hpp>

#ifndef IF_CUT_LDAP

// LDAP查询时, 默认超时时间.
const int knDefaultLdapSearchTimeout = 15 * 1000;
#define YEALINK_MAX_SEARCH_SZ 50

//////////////////////////////////////////////////////////////////////////
// Class CLdapAction
CLdapAction::CLdapAction()
    : CTaskAction(),
      m_pLdapId(NULL),
      m_isSort(false),
      m_hasBound(false),
      m_isOnlyOne(false),
      m_nMessageId(0),
      m_nIndexFrom(0),
      m_nCountToGet(0),
      m_nTotalCount(0)
{
    m_eTARequestType = TA_REQUEST_LDAP;
}

CLdapAction::~CLdapAction()
{
    Final();
}

bool CLdapAction::Exec()
{
    bool isSucceed = false;
    if (IsRead())  // 读取LDAP.
    {
        isSucceed = Read();
    }
    else  // 写入LDAP.
    {
        isSucceed = Write();
    }
    return isSucceed;
}

bool CLdapAction::Init()
{
    return MyBind();
}

bool CLdapAction::Final()
{
    return MyUnBind();
}

int Ldap_CmpNode(yealink_ldap_link pNode1, yealink_ldap_link pNode2)
{
    int iCmp = strcasecmp(pNode1->name, pNode2->name);
    if (iCmp == 0)
    {
        iCmp = strcasecmp(pNode1->numb, pNode2->numb);
    }
    return iCmp;
}

void QListSort(struct List ** head, struct List * end)
{
    struct List * right;
    struct List ** left_walk, **right_walk;
    struct List * pivot, *old;
    int count, left_count, right_count;
    if (*head == end)
    {
        return;
    }
    do
    {
        pivot = *head;
        left_walk = head;
        right_walk = &right;
        left_count = right_count = 0;
        //取第一个节点作为比较的基准，小于基准的在左面的子链表中，
        //大于基准的在右边的子链表中
        for (old = (*head)->Next; old != end; old = old->Next)
        {
            if (Ldap_CmpNode(old, pivot) < 0)
            {
                //小于基准,加入到左面的子链表,继续比较
                ++left_count;
                *left_walk = old;            //把该节点加入到左边的链表中，
                left_walk = &(old->Next);
            }
            else
            {
                //大于基准,加入到右边的子链表，继续比较
                ++right_count;
                *right_walk = old;
                right_walk = &(old->Next);
            }
        }
        //合并链表
        *right_walk = end;       //结束右链表
        *left_walk = pivot;      //把基准置于正确的位置上
        pivot->Next = right;     //把链表合并
        //对较小的子链表进行快排序，较大的子链表进行迭代排序。
        if (left_walk > right_walk)
        {
            QListSort(&(pivot->Next), end);
            end = pivot;
            count = left_count;

        }
        else
        {
            QListSort(head, pivot);
            head = &(pivot->Next);
            count = right_count;
        }
    }
    while (count > 1);
}

bool CLdapAction::Read()
{

    Init();

    m_listLdapItem.clear();

    // 判断条件是否满足.
    if (!m_hasBound || m_pLdapId == NULL)
    {
        SetTaskActionErrorType(TA_LDAP_FAILED_TO_BIND_SERVER);
        return false;
    }

    // 无查询号码.
    if (m_strNumber.empty())
    {
        SetTaskActionErrorType(TA_LDAP_SEARCH_STRING_IS_EMPTY);
        return false;
    }

    // 查询号码过长
    if (m_strNumber.length() >= YEALINK_MAX_SEARCH_SZ)
    {
        SetTaskActionErrorType(TA_LDAP_SEARCH_STRING_TO_LONG);
        return false;
    }

    // 开始查询.
    char szBuffer[YEALINK_MAX_SEARCH_SZ];
    memset(szBuffer, 0, YEALINK_MAX_SEARCH_SZ);
    strcpy(szBuffer, m_strNumber.c_str());
    if (yealink_ldap_search(&m_pLdapId, &m_nMessageId, szBuffer) == 0)
    {
        SetTaskActionErrorType(TA_LDAP_SEARCH_STRING_TO_LONG);
        return false;
    }

    struct timeval tv_temp;
    tv_temp.tv_sec = knDefaultLdapSearchTimeout / 1000;
    tv_temp.tv_usec = knDefaultLdapSearchTimeout % 1000 * 1000;

    // 读取查询到的结果.
    yealink_ldap_link pLdapList = NULL;
    if (yealink_ldap_result(&m_pLdapId, m_nMessageId, &tv_temp, &pLdapList) == 0)
    {
        SetTaskActionErrorType(TA_LDAP_FAILED_TO_GET_REAULTS);
        return false;
    }


    if (m_isSort)
    {
        QListSort(&pLdapList, NULL);
    }


    // 新ldap库要求
    yealink_ldap_abandon(&m_pLdapId, m_nMessageId);
    MyUnBind();

    SetTaskActionErrorType(TA_NO_ERROR);
    yealink_ldap_link pLdapNode = pLdapList;

    // 循环将查询到的结果写入到list中.
    LDAPITEMINFO lii;
    while (pLdapNode != NULL)
    {
        lii.m_strName = pLdapNode->name;
        lii.m_strPhoneNumber = pLdapNode->numb;

        if (m_isOnlyOne)
        {
            if (strcmp(m_strNumber.c_str(), pLdapNode->numb) == 0)
            {
                m_listLdapItem.push_back(lii);
                break;
            }
        }
        else
        {
            m_listLdapItem.push_back(lii);
        }

        pLdapNode = pLdapNode->Next;
    }  // while.

    if (pLdapList != NULL)
    {
        yealink_ldap_memfree(pLdapList);
    }

    // 如果需要排序, 将结果进行排序.
//     if (m_isSort)
//     {
//      SYZ_TRACE("---------------------------sort Begin----------------------------------------");
//      Sort<LDAPITEMINFO, LDAPITEMINFO::FunCompareItem>(m_listLdapItem, LDAPITEMINFO::CompareItem);
//
//      SYZ_TRACE("---------------------------sort End----------------------------------------");
//     }
}

bool CLdapAction::Write()
{
    // 不支持LDAP的写入.
    SetTaskActionErrorType(TA_LDAP_NOT_WRITABLE);
    return true;
}

bool CLdapAction::MyBind()
{
    // 只有未绑定LDAP服务器, 才去绑定.
    if (!m_hasBound)
    {
        yealink_ldap_bind(&m_pLdapId);
        m_hasBound = m_pLdapId != NULL;
    }
    return m_hasBound;
}

bool CLdapAction::MyUnBind()
{
    bool isSucceed = false;
    if (m_hasBound && m_pLdapId != NULL)
    {
        m_hasBound = TRUE;
        m_hasBound = yealink_ldap_ubind(&m_pLdapId) != 0;//SUCC;

    }
    m_pLdapId = NULL;
    m_nMessageId = 0;

    return m_hasBound;
}



#endif



#endif //USE_LDAP_IN_THREADTASKASST___

