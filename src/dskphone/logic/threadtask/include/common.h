#ifndef THREAD_TASK_COMMON_H_
#define THREAD_TASK_COMMON_H_

#include "taskaction/common.h"

// taskaction 中使用的下载模式
enum TASKACTION_DL_MODE
{
    MODE_NONE = 0,
    MODE_MUTIL_AUTH,
    MODE_SIP_XSI_AUTH,
    MODE_BASIC_AUTH,
    MODE_BASIC_XSI_AUTH,
    MODE_FUZE_HOTELING_AUTH,
};

// LDAP元素数据
struct LDAPITEMINFO
{
    yl::string m_strName;
    yl::string m_strPhoneNumber;

    typedef bool (*FunCompareItem)(const LDAPITEMINFO & lhs, const LDAPITEMINFO & rhs);
    // Override operator <
    static bool CompareItem(const LDAPITEMINFO & lhs, const LDAPITEMINFO & rhs)
    {
        // 首比名称, 次比号码.
        if (lhs.m_strName < rhs.m_strName)
        {
            return true;
        }
        else if (lhs.m_strName > rhs.m_strName)
        {
            return false;
        }
        else
        {
            return lhs.m_strPhoneNumber < rhs.m_strPhoneNumber;
        }
    }
};

#endif  // THREAD_TASK_COMMON_H_
