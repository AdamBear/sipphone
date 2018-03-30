#ifndef PIC_PATH_H
#define PIC_PATH_H
#include "path_define.h"
#include "ETLLib.hpp"
#include "interfacedefine.h"

#ifdef _T3X
#   ifdef _T32
#       define PIC_PATH   PHONE_THEME_PATH   "T32Image/"
#   else
#       define PIC_PATH   PHONE_THEME_PATH  "T3XImage/"
#   endif //end _T32
#endif //_T3X

#ifdef _T4X
#   define PIC_PATH   PHONE_THEME_PATH  "theme/"
#endif //_T4X

#ifdef _VPX
#   define PIC_PATH   PHONE_THEME_PATH  "image/"
#endif //_VPX

//image path
#define     PIC_COMMON_PATH     PIC_PATH  "images/"

#define     PIC_GENERAL_PATH        PIC_COMMON_PATH     "general/"
#define     PIC_VOLUME_PATH     PIC_COMMON_PATH     "volume/"
#define     PIC_ARROW_PATH        PIC_COMMON_PATH       "arrow/"
#define     PIC_BTN_PATH                PIC_COMMON_PATH     "button/"
#define     PIC_ICON_PATH           PIC_COMMON_PATH     "icon/"
#define     PIC_MSGBOX_PATH     PIC_COMMON_PATH      "msgbox/"
#define     PIC_MENU_PATH           PIC_COMMON_PATH      "menu/"

// 定义general图片路径
#define  PIC_GENERAL_SELECT_BG1     PIC_GENERAL_PATH "gen_select_bg1.png"

// 定义arrow图片路径
#define     PIC_ARROW_UP1                   PIC_ARROW_PATH "arrows_up1.png"
#define     PIC_ARROW_UP2                   PIC_ARROW_PATH "arrows_up2.png"
#define     PIC_ARROW_DOWN1             PIC_ARROW_PATH "arrows_down1.png"
#define     PIC_ARROW_DOWN2             PIC_ARROW_PATH "arrows_down2.png"

// 定义msgBox图片路径
#define     PIC_MSGBOX_BG1              PIC_MSGBOX_PATH "msgbox1.png"
#define     PIC_MSGBOX_BG4              PIC_MSGBOX_PATH "msgbox4.png"
#define     PIC_MSGBOX_BG_XSI_PWD       PIC_MSGBOX_PATH "xsi_pwd_box.png"

#define     PIC_MENU_ACCOUNT        PIC_MENU_PATH "menu_account.png"
#define     PIC_MENU_AA                 PIC_MENU_PATH "menu_aa.png"

#define     PIC_ICON_CALL_IN            PIC_ICON_PATH "icon_call_in.png"

// 定义volume图片路径
#define     PIC_VOLUME_FULL         PIC_VOLUME_PATH "volume_full.png"
#define     PIC_VOLUME_EMPTY            PIC_VOLUME_PATH "volume_empty.png"
#define     PIC_VOLUME_BG               PIC_VOLUME_PATH "volume_background.png"

#define     PIC_MENU_UC_CONTACT                         PIC_MENU_PATH "menu_uc_contact.png"

#define     PIC_ICON_PRESENCE_AWAY                   PIC_ICON_PATH "icon_presence_away.png"
#define     PIC_ICON_PRESENCE_BUSY                   PIC_ICON_PATH "icon_presence_busy.png"
#define     PIC_ICON_PRESENCE_CHATTY                 PIC_ICON_PATH "icon_presence_chatty.png"
#define     PIC_ICON_PRESENCE_EXTENDEDAWAY           PIC_ICON_PATH "icon_presence_extended_away.png"
#define     PIC_ICON_PRESENCE_OFFLINE                   PIC_ICON_PATH "icon_presence_offline.png"
#define     PIC_ICON_PRESENCE_ONLINE                   PIC_ICON_PATH "icon_presence_online.png"
#define     PIC_ICON_PRESENCE_UNKNOWN                   PIC_ICON_PATH "icon_presence_unknown.png"

//boot pic path
#define PIC_BOOT_PIC_PATH     PIC_PATH  "bootpic/"
#define PIC_BOOT_PIC_WELCOME        PIC_BOOT_PIC_PATH "t49bootlogo.jpg"

#endif//PIC_PATH_H
