#ifndef DIRECTORY_BASEMETHOD_H_
#define DIRECTORY_BASEMETHOD_H_

#include <ylstring.h>
#include <yllist.h>
#include "interfacedefine.h"
#include "contacts/ldaplogic/include/ldapdatadefine.h"

struct BaseDirDataRecord;
struct ContactBroadSoftGroup;
struct ContactBroadSoftPersonal;
struct ContactBroadSoftCommon;
struct ContactBroadSoftEnterprise;
struct ContactBroadSoftCustom;
struct ContactBroadSoftMenu;

struct ContactRemoteMenu;
struct ContactRemoteUnit;
struct ContactRemoteHotKey;

struct LocalLogItemRecord;

struct LocalDirContactRecord;
struct BSFTCallLogRecord;

struct DirGroupRecord;

struct BaseContactRecord;

struct BSFTLogGroupRecord;
struct ContactGBMenu;

struct ContactGBBookUnit;

struct LdapContactData;

#if IF_FEATURE_METASWITCH_CALLLOG
struct MTSWCallLogRecord;
struct MTSWLogGroupRecord;
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
struct ContactMetaSwitchMenu;
struct ContactMetaSwitchContactList;
struct ContactMetaSwitchMLHG;

#endif
// 转换为menu联系人.
// | pBase | 为基类指针.
// 转换成功返回menu指针, 否则返回NULL.
ContactRemoteMenu * Base2RemoteMenu(BaseDirDataRecord * pBase);

// 转换为具体联系人.
// | pBase | 为基类指针.
// 转换成功返回menu指针, 否则返回NULL.
ContactRemoteUnit * Base2Unit(BaseDirDataRecord * pBase);

// 转换为快捷键联系人.
// | pBase | 为基类指针.
// 转换成功返回menu指针, 否则返回NULL.
ContactRemoteHotKey * Base2HotKey(BaseDirDataRecord * pBase);

// 转换为BoradSoft个人联系人.
// | pBase | 为基类指针.
// 转换成功返回BoradSoft个人联系人, 否则返回NULL.
ContactBroadSoftPersonal * Base2Personal(BaseDirDataRecord * pBase);

// 转换为BoradSoft组联系人.
// | pBase | 为基类指针.
// 转换成功返回BoradSoft组联系人, 否则返回NULL.
ContactBroadSoftGroup * Base2Group(BaseDirDataRecord * pBase);

// 转换为BoradSoft Common联系人.
// | pBase | 为基类指针.
// 转换成功返回BoradSoft common联系人, 否则返回NULL.
ContactBroadSoftCommon * Base2Common(BaseDirDataRecord * pBase);

// 转换为BoradSoft Enterprise联系人.
// | pBase | 为基类指针.
// 转换成功返回BoradSoft企业联系人, 否则返回NULL.
ContactBroadSoftEnterprise * Base2Enterprise(BaseDirDataRecord * pBase);

ContactBroadSoftCustom * Base2Custom(BaseDirDataRecord * pBase);

//组
ContactBroadSoftMenu * Base2BsftMenu(BaseDirDataRecord * pBase);

#if IF_FEATURE_METASWITCH_DIRECTORY
ContactMetaSwitchMenu * Base2MtswMenu(BaseDirDataRecord * pBase);

ContactMetaSwitchContactList * Base2MtswContact(BaseDirDataRecord * pBase);

ContactMetaSwitchMLHG * Base2MtswhMLHGContact(BaseDirDataRecord * pBase);
#endif

LocalDirContactRecord * Base2LocalContact(BaseDirDataRecord * pBase);

LdapContactData * Base2LdapContact(BaseDirDataRecord * pBase);

LocalLogItemRecord * Base2LocalLog(BaseDirDataRecord * pBase);

BSFTCallLogRecord * Base2BSFTLog(BaseDirDataRecord * pBase);

BSFTLogGroupRecord * Base2BSFTLogGroup(BaseDirDataRecord * pBase);

#if IF_FEATURE_METASWITCH_CALLLOG
MTSWCallLogRecord * Base2MTSWLog(BaseDirDataRecord * pBase);

MTSWLogGroupRecord * Base2MTSWLogGroup(BaseDirDataRecord * pBase);
#endif

DirGroupRecord * Base2DirGroup(BaseDirDataRecord * pBase);

BaseContactRecord * Base2BaseContact(BaseDirDataRecord * pBase);

#if IF_FEATURE_GBBOOK

ContactGBMenu * Base2GabMenu(BaseDirDataRecord * pBase);

ContactGBBookUnit * Base2GbUnit(BaseDirDataRecord * pBase);

#endif
// 获取网络状态是否可用.
// 如果网络状态可用则返回true, 否则返回false.
bool IsNetworkConnected();

CALL_TYPE GetLogTypeByGroupId(int nGroupId);

int GetFirstCharSortValue(const yl::string & strName);

int GetDirMaxDownloadSize();

#endif  //DIRECTORY_BASEMETHOD_H_
