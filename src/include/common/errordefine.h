#ifndef DEFINE_ERRORDEFINE_H_
#define DEFINE_ERRORDEFINE_H_

// Error definition.
enum RErrorType
{
    RERROR_TYPE_NONE = 0,  // Invalid error type.
#ifdef _T42
    OPTION_SUC = 1,
#endif

    // Contact error.
    RERROR_GROUP_EXIST = -1,  // Group already existed when adding.
    RERROR_CONTACT_EXIST = -2,  // Contact already existed when adding.
    RERROR_INVALID_GROUP_ID = -3,  // Invalid group id.
    RERROR_GROUP_NOT_EXIST = -4,  // Group not existed when deleting or updating.
    RERROR_CONTACT_NOT_EXIST = -5,  // Contact not existed when deleting or updating.
    RERROR_GROUP_CANNOT_REMOVE = -6,  // 组不可删除.
    RERROR_CONTACT_LIST_IS_FULL = -7,  // 联系人数量已经超过上限.
    RERROR_BLACK_LIST_IS_FULL = -8,  // 黑名单
    RERROR_GROUP_LIST_IS_FULL = -9,  // 组数量超过上限.
    RERROR_CONTACT_NOT_LOCAL = -10,  // 联系人不是本地联系人.
    RERROR_CONTACT_NOT_BLACK = -12,  // 联系人不是黑名单联系人.
    RERROR_CONTACT_SYNCING = -13,  // 联系人正在同步.

    RERROR_NETWORK_DISABLE = -14,

    // Memory error.
    RERROR_FAILT_TO_ALLOC_MEMORY = -50,  // Fail to alloc memery.

    // Args error.
    RERROR_INVALID_ARGS = -100,  // Invalid args.
#ifdef _T42
    RERROR_OTHER_RERROR = -101   //其他类型错误
#endif
};

#endif  // DEFINE_ERRORDEFINE_H_
