#ifndef __AUTOSET_DEF_H__
#define __AUTOSET_DEF_H__

#include "dsskey/include/dsskey_enumtypes.h"
#include "dsskey/include/dsskeystructure.h"
#include <ylstring.h>

enum AUTOSET_TYPE
{
    // 优先级关系注意，数字越大优先级越低
    AUTOSET_NON             = 0x00,
    AUTOSET_ACCOUNT         = (0x01 << 0),          // 自动设置账号类型
    AUTOSET_SEMI_ACCOUNT    = (0x01 << 1),          // telstra手自一体autoset方案
    AUTOSET_BT              = (0x01 << 2),          // 自动设置BT类型
    AUTOSET_BLFLIST         = (0x01 << 3),          // 自动设置Blflis这t类型
    AUTOSET_UCFAVORITE      = (0x01 << 4),          // 自动设置UC常用联系人类型
    AUTOSET_FAVORITE_DIR    = (0x01 << 5),
    AUTOSET_ALL             = (0xFF), // 需要自动设置账号和Blflist类型
};

// 账号策略的具体子策略
enum AUTOSET_ACCOUNT_TYPE
{
    AUTO_ACC_NONE   = 0, // 无方案
    AUTO_ACC_AT     = 1, // 自动（完全自动挡，不写配置）
    AUTO_ACC_MT     = 2, // 手动（手动挡，纯手动操作，只在数量不等时操作，telstra方案，写配置）
    AUTO_ACC_AMT    = 3, // 手自一体（手自一体，手动优先，不写配置）
};

enum AUTOSET_SEQUENCE
{
    POSITIVE_SEQUENCE,
    NEGATIVE_SEQUENCE,
    FIRSTPAGE_SEQUENCE,
    EXP_FIRSTPATE_SEQUENCE,
};

struct BLFBaseData;
struct LocalDirContactRecord;

typedef struct autoset_dsskeydata_t
{
    autoset_dsskeydata_t() : iDssKeyID(INVALID_DSSKEY_ID)
    {
        clear();
    }

    int         iDssKeyID;          // id
    DssKey_Type eDKType;            // The DK_Type.
    int         iLineIndex;         // 账号id
    int         iStatus;                    // dsskey类型对应的状态

    yl::string  strLabel;           // The label.
    yl::string  strValue;           // 号码、URL等……
    yl::string  strExtension;       // 扩展数据
    yl::string  strShortLabel;      // ShortLabel值
    yl::string  strKey;              // key值

    void clear();

    autoset_dsskeydata_t& operator << (const DssKeyLogicData& tDsskeyData);
    autoset_dsskeydata_t& operator >> (DssKeyLogicData& tDsskeyData);
    autoset_dsskeydata_t& operator << (BLFBaseData& tBlfData);
    autoset_dsskeydata_t& operator << (LocalDirContactRecord& record);
    autoset_dsskeydata_t& operator << (DssKey_Type eType);
    autoset_dsskeydata_t& operator << (int iLineID);
} autoset_dsskey_data;

struct dsskey_change_info_t
{
    int iDsskeyID;
    int iOldLineID;
    DssKey_Type eOldType;
    DssKey_Type eNewType;
};

#define AUTOSET_STRATEGY_NAME_BLFLIST       "blf_list"
#define AUTOSET_STRATEGY_NAME_FAVORITE      "favorite"
#define AUTOSET_STRATEGY_NAME_UC_FAVORITE   "uc_favorite"

#endif  //__AUTOSET_DEF_H__
