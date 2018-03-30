
#include "modcommon.h"

#include<stdio.h>
#include<stdlib.h>

#include <ETLLib.hpp>
#include "uccontactdata.h"
#include "requestrosteraction.h"
#include<sys/types.h>
#include<sys/fcntl.h>
#include<unistd.h>

#if FEATURE_BROADTOUCH_UC

yl::string GetSubString(const yl::string & strSource, const yl::string & strLeft,
                        const yl::string & strRight)
{
    int nLeft = 0;
    int nRight = strSource.length();

    if (!strLeft.empty())
    {
        int iLeftPos = strSource.find(strLeft, 0);
        if (iLeftPos != yl::string::npos)
        {
            nLeft = iLeftPos + strLeft.length();
        }
        else
        {
            return "";
        }
    }

    if (!strRight.empty())
    {
        int iRightPos = strSource.find(strRight, 0);
        if (iRightPos != yl::string::npos)
        {
            nRight = iRightPos;
        }
        else
        {
            return "";
        }
    }

    if (nLeft <= nRight)
    {
        return strSource.substr(nLeft, nRight - nLeft);
    }

    return "";
}

void ReSetGroupKey(GroupKey & keyGroup)
{
    keyGroup = -1;
}

// 比较成员.
bool CompareItem(GroupKey & lhs,
                 GroupKey & rhs)
{

    int nlType = g_pUCPresence->GetPresenceById(lhs);

    int nrType = g_pUCPresence->GetPresenceById(rhs);

    if (nlType != nrType)
    {
        return nlType < nrType;
    }

    const yl::string & strLkey = g_pUCPresence->GetSortKeyByKey(lhs);

    const yl::string & strRkey = g_pUCPresence->GetSortKeyByKey(rhs);

    if (strLkey.empty())
    {
        return false;
    }
    if (strRkey.empty())
    {
        return true;
    }

    return strcasecmp(strLkey.c_str(), strRkey.c_str()) < 0;
}

CPresenceRequestAction * TaskAction_CreatePresenceRequestAction()
{
    // 创建CRingFileAction实例.
    CPresenceRequestAction * pAction = new CPresenceRequestAction();
    return pAction;
}


#endif
