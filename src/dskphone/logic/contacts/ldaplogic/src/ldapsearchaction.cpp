#include "ldapsearchaction.h"
//#include "download/error.h"
#include "ldap/yealink_ldap.h"


#if IF_BUG_24324
#include "commonapi/stringhelperapi.h"
#endif

#define YEALINK_MAX_SEARCH_SZ       50      //查询关键字的最大长度，//修改为31
#define YEALINK_MAX_HOST_NAME       100     //主机名，有可能是域名
#define YEALINK_MAX_ATTRS_NUMBER    30      //请求服务器返回的attributes的最大个数
#define YEALINK_MAX_USER_NAME       100     //用户名
#define YEALINK_MAX_PSWD_NAME       100     //密码
#define YEALINK_MAX_BASE_NAME       100     //查询的base
#define YEALINK_MAX_NAME_FILTER     100
#define YEALINK_MAX_NUMB_FILTER     100
#define YEALINK_MAX_ATTRIBUTES      100
#define YEALINK_MAX_DISPLAY_NAME    100
#define YEALINK_MAX_DISPLAY_NUMB    30
#define YEALINK_MAX_NUMB_PER_DN     20

// LDAP查询时, 默认超时时间.
const int knDefaultLdapSearchTimeout = 15 * 1000;

//////////////////////////////////////////////////////////////////////////
// Class CLdapAction
CLdapSearchAction::CLdapSearchAction()
    : CTaskAction(),
      m_pLdapId(NULL),
      m_isSort(false),
      m_hasBound(false),
      m_nMessageId(0)
{
    m_bClear = true;
    m_eTARequestType = NS_TA::TA_REQUEST_LDAP;
    m_nErrorCode = -1;
}

CLdapSearchAction::~CLdapSearchAction()
{
    Final();
}

bool CLdapSearchAction::Exec()
{
    LDAP_INFO("CLdapSearchAction::Exec()");
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

void CLdapSearchAction::SetClearDataFlag(bool bClear)
{
    m_bClear = bClear;
}

bool CLdapSearchAction::Final()
{
    if (m_bClear)
    {
        for (LdapItemListIter iter = m_listLdapItem.begin(); iter != m_listLdapItem.end();
                iter++)
        {
            LdapContactData * pData = *iter;

            if (pData != NULL)
            {
                delete pData;
                pData = NULL;
            }
        }
    }

    //释放数据
    return MyUnBind();
}

int Ldap_CmpNode(yealink_ldap_link_user pNode1, yealink_ldap_link_user pNode2)
{
    if (pNode1 == NULL || pNode2 == NULL)
    {
        return 0;
    }

    int iCmp = strcasecmp(pNode1->name, pNode2->name);
    if (iCmp == 0)
    {
        yealink_ldap_link_num pLdapNumNode1 = pNode1->num_list;
        yealink_ldap_link_num pLdapNumNode2 = pNode2->num_list;
        if (pLdapNumNode1 != NULL && pLdapNumNode2 != NULL)
        {
            iCmp = strcasecmp(pLdapNumNode1->numb, pLdapNumNode2->numb);
        }
    }
    return iCmp;
}

void QListSort(struct List_user ** head, struct List_user * end)
{
    struct List_user * right;
    struct List_user ** left_walk, **right_walk;
    struct List_user * pivot, *old;
    int count, left_count, right_count;

    if (*head == end || *head == NULL)
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
            if (old == NULL)
            {
                continue;
            }

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

bool CLdapSearchAction::Read()
{
    LDAP_INFO("CLdapSearchAction::Read() begin");

    MyBind();

    // 判断条件是否满足.
    if (!m_hasBound || m_pLdapId == NULL)
    {
        SetTaskActionErrorType(NS_TA::TA_LDAP_FAILED_TO_BIND_SERVER);
        return false;
    }

    // 查询号码过长
    if (m_strKey.length() >= YEALINK_MAX_SEARCH_SZ)
    {
        SetTaskActionErrorType(NS_TA::TA_LDAP_SEARCH_STRING_TO_LONG);
        return false;
    }

    // 开始查询.
    // 当搜索关键字为空时，获取所有信息，不为空时按关键字搜索
    struct timeval tv_temp;
    tv_temp.tv_sec = knDefaultLdapSearchTimeout / 1000;
    tv_temp.tv_usec = knDefaultLdapSearchTimeout % 1000 * 1000;
    yealink_ldap_link_user pLdapList = NULL;

    char szBuffer[YEALINK_MAX_SEARCH_SZ];
    memset(szBuffer, 0, YEALINK_MAX_SEARCH_SZ);
    strcpy(szBuffer, m_strKey.c_str());
    char search_key[] = { 0x01, 0x00 }; //默认搜索使用

    if (yealink_ldap_search(&m_pLdapId, &m_nMessageId, m_strKey.empty() ? search_key : szBuffer) == 0)
    {
        SetTaskActionErrorType(NS_TA::TA_LDAP_SEARCH_STRING_TO_LONG);
        return false;
    }


    // 读取查询到的结果.
    if (yealink_ldap_result_intergated(&m_pLdapId, m_nMessageId, &tv_temp, &pLdapList) == 0)
    {
        SetTaskActionErrorType(NS_TA::TA_LDAP_FAILED_TO_GET_REAULTS);
        return false;
    }


    if (m_isSort)
    {
        QListSort(&pLdapList, NULL);
    }

    SetTaskActionErrorType(NS_TA::TA_NO_ERROR);
    yealink_ldap_link_user pLdapNode = pLdapList;

    // 循环将查询到的结果写入到list中.
    while (pLdapNode != NULL)
    {
        LdapContactData * pData = new LdapContactData;

        pData->m_strDisplayName = pLdapNode->name;
        yealink_ldap_link_num pLdapNumNode = pLdapNode->num_list;

        for (; pLdapNumNode != NULL; pLdapNumNode = pLdapNumNode->Next)
        {
            stNumber stNum;
            stNum.strName = pLdapNumNode->attr;
            stNum.strValue = pLdapNumNode->numb;
            pData->m_listNumber.push_back(stNum);
        }

#if IF_BUG_24324
        YLList<yl::string> listContent;

        commonAPI_splitStringBySeparator(pData->m_strName, ';', listContent);

        int i = 0;

        for (YLList<yl::string>::iterator iter = listContent.begin(); iter != listContent.end(); iter++)
        {
            if (i == 0)
            {
                pData->m_strName = *iter;
            }
            else if (i == 1)
            {
                pData->m_strCompany = *iter;
            }
            else if (i == 2)
            {
                pData->m_strDepartment = *iter;
            }
            else
            {
                break;
            }
            i++;
        }
#endif


        m_listLdapItem.push_back(pData);


        pLdapNode = pLdapNode->Next;
    }  // while.


    if (pLdapList != NULL)
    {
        yealink_ldap_memfree_integrated(pLdapList);
    }

    MyUnBind();

    LDAP_INFO("CLdapSearchAction::Read() End");
    return true;
}

bool CLdapSearchAction::Write()
{
    // 不支持LDAP的写入.
    SetTaskActionErrorType(NS_TA::TA_LDAP_NOT_WRITABLE);
    return true;
}

bool CLdapSearchAction::MyBind()
{
    // 只有未绑定LDAP服务器, 才去绑定.
    if (!m_hasBound)
    {
        m_nErrorCode = yealink_ldap_bind(&m_pLdapId);
        m_hasBound = m_pLdapId != NULL;
    }

    return m_hasBound;
}

bool CLdapSearchAction::MyUnBind()
{
    if (m_hasBound && m_pLdapId != NULL)
    {
        m_hasBound = yealink_ldap_ubind(&m_pLdapId) != SUCC;
    }
    m_pLdapId = NULL;
    m_nMessageId = 0;

    return m_hasBound;
}

