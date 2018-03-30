#ifndef _MODCOMMON_H
#define _MODCOMMON_H
#include "presence/include/presence_inc.h"
#include "commonunits/modfileoperate.h"

#if FEATURE_BROADTOUCH_UC

class CPresenceRequestAction;

yl::string GetSubString(const yl::string & strSource, const yl::string & strLeft,
                        const yl::string & strRight);

void ReSetGroupKey(GroupKey & keyGroup);

bool CompareItem(GroupKey & lhs,
                 GroupKey & rhs);

//创建roster请求action
CPresenceRequestAction * TaskAction_CreatePresenceRequestAction();


#endif

#endif // _MODCOMMON_H
