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

#define THEME1_PREFIX   "Theme1/"
#define IMAGES_PREFIX   "images/"

#define     PIC_ANIMATION_PREFIX        IMAGES_PREFIX   "animation/"
#define     PIC_ARROWS_PREFIX           IMAGES_PREFIX   "arrows/"
#define     PIC_BLUE_PREFIX             IMAGES_PREFIX   "blue/"
#define     PIC_BTN_PREFIX              IMAGES_PREFIX   "button/"
#define     PIC_CONTACTS_PREFIX         IMAGES_PREFIX   "contacts/"
#define     PIC_CONTACTS_ICON_PREFIX    IMAGES_PREFIX   "contacts/icon/"
#define     PIC_DSS_PREFIX              IMAGES_PREFIX   "dss/"
#define     PIC_DSSBG_PREFIX            IMAGES_PREFIX   "dssbg/"
#define     PIC_GENERAL_PREFIX          IMAGES_PREFIX   "general/"
#define     PIC_ICON_PREFIX             IMAGES_PREFIX   "icon/"
#define     PIC_MENU_PREFIX             IMAGES_PREFIX   "menu/"
#define     PIC_MSGBOX_PREFIX           IMAGES_PREFIX   "msgbox/"
#define     PIC_SCROLL_PREFIX           IMAGES_PREFIX   "scroll/"
#define     PIC_SOFTKEY_PREFIX          IMAGES_PREFIX   "softkey/"
#define     PIC_VOLUME_PREFIX           IMAGES_PREFIX   "volume/"
#define     PIC_WIDGET_PREFIX           IMAGES_PREFIX   "widget/"
#define     PIC_WIDGET_COMBOX_PREFIX    IMAGES_PREFIX   "widget/combox/"
#define     PIC_WIFI_PREFIX             IMAGES_PREFIX   "wifi/"
#define     PIC_WIFI_SCANNING_PREFIX    IMAGES_PREFIX   "wifi/scanning/"
#define     PIC_THEME1_PREFIX           THEME1_PREFIX

// define blue direct path

// 定义general图片路径
#define  PIC_GENERAL_SELECT_BG1     PIC_GENERAL_PREFIX "gen_select_bg1.png"

// 定义arrow图片路径
#define     PIC_ARROW_UP1                   PIC_ARROWS_PREFIX "arrows_up1.png"
#define     PIC_ARROW_UP2                   PIC_ARROWS_PREFIX "arrows_up2.png"
#define     PIC_ARROW_DOWN1             PIC_ARROWS_PREFIX "arrows_down1.png"
#define     PIC_ARROW_DOWN2             PIC_ARROWS_PREFIX "arrows_down2.png"

// 定义msgBox图片路径
#define     PIC_MSGBOX_BG1              PIC_MSGBOX_PREFIX "msgbox1.png"
#define     PIC_MSGBOX_BG4              PIC_MSGBOX_PREFIX "msgbox4.png"
#define     PIC_MSGBOX_BG_XSI_PWD       PIC_MSGBOX_PREFIX "xsi_pwd_box.png"

#define     PIC_MENU_ACCOUNT        PIC_MENU_PREFIX "menu_account.png"
#define     PIC_MENU_AA                 PIC_MENU_PREFIX "menu_aa.png"

#define     PIC_ICON_CALL_IN            PIC_ICON_PREFIX "icon_call_in.png"

// 定义volume图片路径
#define     PIC_VOLUME_FULL         PIC_VOLUME_PREFIX "volume_full.png"
#define     PIC_VOLUME_EMPTY            PIC_VOLUME_PREFIX "volume_empty.png"
#define     PIC_VOLUME_BG               PIC_VOLUME_PREFIX "volume_background.png"

#define     PIC_MENU_UC_CONTACT                         PIC_MENU_PREFIX "menu_uc_contact.png"


#endif//PIC_PATH_H
