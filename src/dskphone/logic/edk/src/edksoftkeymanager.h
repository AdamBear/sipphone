///////////////////////////////////////////////////////////
//  CEdkSoftKeyManager.h
//  Implementation of the Class CEdkSoftKeyManager
//  Created on:      21-一月-2015 15:32:26
//  Original author: dlw
///////////////////////////////////////////////////////////

#if !defined(__EDK_SOFTKEY_MANAGER_H__)
#define __EDK_SOFTKEY_MANAGER_H__

#include "../include/edk_def.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "dsskey/include/moddsskey.h"

class CEdkSoftKeyManager
{
public:
    CEdkSoftKeyManager();
    virtual ~CEdkSoftKeyManager();

public:

    // 获取该界面需要添加的softkey
    bool GetEdkSoftKey(listEdkSoftKey & listSoftKey);

    // 获取要添加的softkey链表
    void GetEdkSoftKeyList(YLList<SoftKey_Data> & listSoftKey);
private:
    // 映射talkui的界面
    EDKUITYPE MapUIType(SESSION_STATE eState);

    // 判断Idle界面是否有其他状态
    bool IsHaveOtherStatus();

};
#endif // !defined(__EDK_SOFTKEY_MANAGER_H__)
