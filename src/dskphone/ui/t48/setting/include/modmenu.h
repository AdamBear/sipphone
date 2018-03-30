/************************************************************************
 * FileName  : modMenu.h (header file)
 * Purpose   :
 * Date      : 2012/06/19 16:32:42
 ************************************************************************/

#ifndef _MODMENU_HEADER_
#define _MODMENU_HEADER_

#include <ylstring.h>
#include "setting/include/common.h"

enum EMenuDefaultIconType
{
    eMDIT_LocalDirDeftMenuIcon, // 本地联系人菜单的图片名.
    eMDIT_RemoteDirDeftMenuIcon,  // 远程联系人菜单的图片名.
    eMDIT_BSFTDirDeftMenuIcon,   // BroadSoft联系人菜单的图片名.
    eMDIT_LdapDirDeftMenuIcon,   // Ldap联系人菜单的图片名.
    eMDIT_UCDirDeftMenuIcon,        // BroadTouchUC联系人菜单的图片名
    eMDIT_LocalCallLogDeftMenuIcon, // 本地CallLog菜单的图片名.
    eMDIT_BSFTCallLogDeftMenuIcon, // BroadSoftCallLog菜单的图片名.
    eMDIT_CallControlDeftMenuItemIcon, // Call Control菜单项的icon名.
    eMDIT_CallForwardDeftMenuItemIcon, // Call forward菜单项的icon名.
    eMDIT_CallWaitingDeftMenuItemIcon, // Call Waiting菜单项的icon名.
    eMDIT_AutoAnswerDeftMenuItemIcon, // Auto Answer菜单项的icon名.
    eMDIT_DssKeyDeftMenuItemIcon, // DSS Keys菜单项的icon名.
    eMDIT_DNDDeftMenuItemIcon, // DND菜单项的icon名.
    eMDIT_IntercomDeftMenuItemIcon, // Intercom菜单项的icon名.
    eMDIT_AnonymouseCallDeftMenuItemIcon, // Anonymous Call菜单项的icon名.
    eMDIT_AdvanceDeftMenuItemIcon, // Advanced菜单项的icon名.
    eMDIT_BSFTDirDeftMenuItemIcon, // Broadsoft Directory菜单项的icon名.
    eMDIT_BSFTCallLogMenuItemIcon, // Broadsoft Calllog菜单项的icon名.
    eMDIT_StatusRTPDeftMenuItemIcon,        //Status-RTP
    eMDIT_DeftMenuIconPath, //默认菜单图标的存放路径
    eMDIT_MyStatusDeftMenuItemIcon,
};

//设置Meun中用到的默认图标
void Menu_SetDefaultIcon(EMenuDefaultIconType eIconType, const yl::string & strIconFile);

//获取Meun中用到的默认图标
yl::string Menu_GetDefaultIcon(EMenuDefaultIconType eIconType);

//相应对应的点击操作
bool Menu_DoAction(const yl::string & strClickAction);

//通过操作名获取Menu的Title
const yl::string Menu_GetTitleByClickAction(const yl::string & strClickAction);

//通过操作名获取对应的MenuList内容
MenuItemListData Menu_GetMenuListByClickAction(const yl::string & strClickAction);

//根据Page类型进入对应菜单页面
bool Menu_EnterPageByType(int nPageType, int nIndex = 0, bool bEnter = true);

//根据菜单的aciton获取对应的三级权限文件中的key
yl::string Menu_GetWebItemKeyByClickAction(const yl::string strClickAction);

#endif
