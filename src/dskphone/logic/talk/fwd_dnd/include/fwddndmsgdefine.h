#ifndef __FWD_DND_MSG_DEFINE_H__
#define __FWD_DND_MSG_DEFINE_H__

#include <ylstring.h>
#include "fwddndtypes.h"
#include "innermsgdefine.h"

/** FWD状态变更消息附加数据 */
struct FwdMessageStatusData
{
    enum
    {
        TARGET_BUFFER_SIZE = 33 // 目标号码缓冲区大小
    };

    const FWD_TYPE eType;   // 转移类型
    bool bEnabled;  // 当前状态是否启用
    char szTarget[TARGET_BUFFER_SIZE];  // 转移目标号码
    int iRingDelay; // 无应答转移响铃时间，单位：s

    FwdMessageStatusData(FWD_TYPE eType)
        : eType(eType)
        , bEnabled(false)
        , iRingDelay(0)
    {
        szTarget[0] = '\0';
    }
};

/** FWD/DND消息ID */
enum FWDDND_MESSAGE
{
    // DND状态变更
    // wParam=账号ID，全局为-1(FWDDND_ACCOUNT_GLOBAL)
    // lParam=发起变更的来源（DND_CONFIG_SOURCE_TYPE）
    // extraData=变更后是否启用（bool）
    DND_MESSAGE_STATUS_CHANGED = INNER_MSG_DND_BEGIN,

    // FWD状态变更
    // wParam=账号ID，全局为-1(FWDDND_ACCOUNT_GLOBAL)
    // lParam=发起变更的来源（FWD_CONFIG_SOURCE_TYPE）
    // extraData=变更后FWD状态（FwdMessageStatusData）
    FWD_MESSAGE_STATUS_CHANGED = INNER_MSG_FORWARD_BEGIN
};

#endif // !__FWD_DND_MSG_DEFINE_H__
