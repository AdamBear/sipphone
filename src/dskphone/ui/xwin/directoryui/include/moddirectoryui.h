
#ifndef __MOD_DIRECTORY_UI_H__
#define __MOD_DIRECTORY_UI_H__

#include <ylstring.h>
#include "diruilogic/include/directorycommon.h"

// 初始化Directory模块
void DM_InitData();

// 释放Directory资源
void DM_UnInitData();

// 退出Directory模块
void DM_ExitDirectory();

bool DM_EnterDirectoryUI(int nType, int nGroupId = 0);

bool DM_EnterSearch(int nDirType, int nGroupId = kszAllContactId);

bool DM_IsInCallLogBrowser();

bool DM_IsDirDssKey(int nDssKey);

// 获取Dsskey的BroadSoft组列表Id
void DM_GetDsskeyBsftGroupList(VEC_CONTACT_ID & vecGroupId);

// 获取群组名（已经翻译）
void DM_GetGroupDisplayName(DIR_TYPE eDirType, int nGroupId, yl::string & strDisplayName);

void DM_GetLdapName(yl::string & strName);

#if IF_FEATURE_METASWITCH_DIRECTORY
bool DM_AddContact(yl::string strName, yl::string strNumber, int nAction, int nDirType);
#endif

#if IF_FEATURE_FAVORITE_DIR
bool DM_AddFavoriteContact(int nGroupId = 0);
#endif

#if DIR_SUPPORT_SELECT_CONTACT
void DM_CancelSelectDirectory();
void DM_SetSelectDirectoryCallBackFun(DIR_SELECT_CONTACT_FUN pFunSelectContact);
#endif

#endif //__MOD_DIRECTORY_UI_H__
