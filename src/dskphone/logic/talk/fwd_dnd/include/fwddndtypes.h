#ifndef __FWD_DND_TYPES_H__
#define __FWD_DND_TYPES_H__

//No Answer Forword 响铃间隔总数
#define  TOTAL_RING_INTERVAL  20
enum
{
    FWDDND_ACCOUNT_GLOBAL = -1  // FWD/DND全局模式账号ID
};

/** FWD/DND模式类型. */
enum FWDDND_MODE
{
    FWDDND_MODE_GLOBAL = 0, // FWD/DND全局模式(Phone模式)
    FWDDND_MODE_ACCOUNT // FWD/DND账号模式(Custom模式)
};

/** FWD类型 */
enum FWD_TYPE
{
    FWD_TYPE_MANUAL = 0,    // 手动转移
    FWD_TYPE_ALWAYS = 1 << 0,   // 无条件转移
    FWD_TYPE_BUSY = 1 << 1, // 遇忙转移
    FWD_TYPE_NO_ANSWER = 1 << 2,    // 无应答转移
    FWD_TYPE_ALL = FWD_TYPE_ALWAYS | FWD_TYPE_BUSY | FWD_TYPE_NO_ANSWER // 任何转移类型
};

/** DND配置来源类型，用于区分发起配置修改的来源 */
enum DND_CONFIG_SOURCE_TYPE
{
    DND_CFG_SRC_TYPE_COMMON, // 通用来源修改DND配置
    DND_CFG_SRC_TYPE_STARTUP, // 启动时通知DND状态
    DND_CFG_SRC_TYPE_SHUTDOWN, // 关闭时通知DND状态
    DND_CFG_SRC_TYPE_SYNC // 服务器同步DND配置
};

/** FWD配置来源类型，用于区分发起配置修改的来源 */
enum FWD_CONFIG_SOURCE_TYPE
{
    FWD_CFG_SRC_TYPE_COMMON, // 通用来源修改FWD配置
    FWD_CFG_SRC_TYPE_STARTUP, // 启动时通知FWD状态
    FWD_CFG_SRC_TYPE_SHUTDOWN, // 关闭时通知FWD状态
    FWD_CFG_SRC_TYPE_SYNC, // 服务器同步FWD配置
    FWD_CFG_SRC_TYPE_DSSKEY // DSSKey修改FWD配置
};

#endif // !__FWD_DND_TYPES_H__
