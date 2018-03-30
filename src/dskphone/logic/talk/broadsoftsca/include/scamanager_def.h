#ifndef __SCA_MANAGER_DEF_H__
#define __SCA_MANAGER_DEF_H__

#include "yllist.h"
#include "ylstring.h"
#include "dsskey/include/dsskey_def.h"
#include "yllist.h"

// SCA账号分线的状态等信息
struct SCA_Info
{
    int         iIndex;     // 该账号分线的Index,从1开始计数
    int         iDsskeyIndex;// Dsskey索引
    BLF_STATUS_CODE eStatus;// 该账号分线的状态
    yl::string  strDisplay; // 如果账号分线处于通话状态,则该路通话的显示名
    yl::string  strURI;     // 如果账号分线处于通话状态,则该路通话的URI
};

// SCA状态的列表
typedef YLList<SCA_Info *> ListSCAInfo;

// SCA状态菜单所需数据
struct ScaMenuData
{
    ScaMenuData()
    {
        pScaInfoList = NULL;
        iAccount = -1;
        nReturnTime = 10000;
    }

    ListSCAInfo * pScaInfoList; //
    int      iAccount;      // 显示SCA状态的账号ID
    int          nReturnTime;   // 自动退出时间
};

#endif
