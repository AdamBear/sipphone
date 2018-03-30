
#ifndef __MOD_DIRECTORY_UI_H__
#define __MOD_DIRECTORY_UI_H__

#include <ylstring.h>
#include "directorycommon.h"

class QString;
// 初始化Directory模块
void DM_InitData();

// 设置CallManager模块联系人的相关信息
void DM_SetCallManagerInfo();

// 退出Directory模块
void DM_ExitDirectory();

BOOL DM_EnterLocalMenu();

bool DM_IsSearchDirectoryExist();

#if DIR_SUPPORT_SELECT_CONTACT
bool DM_EnterDirectoryUI(int nType, int nGroupId = 0, bool bSelectState = false);
#else
bool DM_EnterDirectoryUI(int nType, int nGroupId = 0);
#endif

#if DIR_SUPPORT_SELECT_CONTACT
void DM_EnterSelectDirectoryUI(DIR_SELECT_CONTACT_FUN pFunSelectContact,
                               int nType = DIRECTORY_TYPE_LOCAL);
void DM_CancelSelectDirectory();
#endif

void DM_RetryChgPwdRuest(int nType, const yl::string & strPwd);

// 获取联系人图像名称
yl::string DM_GetContactImageName(const yl::string & strResName);

// 获取Dsskey的BroadSoft组列表Id
void DM_GetDsskeyBsftGroupList(VEC_CONTACT_ID & vecGroupId);

// 获取群组名（已经翻译）
void DM_GetGroupDisplayName(DIR_TYPE eDirType, int nGroupId, QString & strDisplayName);

void DM_GetGroupDisplayName(DIR_TYPE eDirType, int nGroupId, yl::string & strDisplayName);

void DM_GetLdapName(QString & strName);

// 添加联系人到Bsft Personal群组
void DM_EnterAddPersonalContact(const yl::string & strName, const yl::string & strNumber);

// 实现每个界面打开逻辑的注册
void DM_RegisterOpenUICheckFunc();

#if IF_FEATURE_METASWITCH_DIRECTORY
bool DM_AddDirectory(yl::string strName, yl::string strNumber, int nAction, int nDirType);
#endif

#if IF_FEATURE_FAVORITE_DIR
bool DM_EnterAddLocalContact(int nGroupId = 0);

void DM_ReturnToIdle();
#endif

#endif
