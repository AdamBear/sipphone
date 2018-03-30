//#ifdef _USE_SKIA

#ifndef PIC_PATH_H
#define PIC_PATH_H
#include "path_define.h"
#include "ETLLib.hpp"

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
#define     PIC_COMMON_PATH     "images/"

#define     THEME_PREFIX                    "Theme#"

#define     PIC_ANIMATION_PREFIX        THEME_PREFIX    "animation@"
#define     PIC_ARROWS_PREFIX           THEME_PREFIX    "arrows@"
#define     PIC_CONTACTS_PREFIX         THEME_PREFIX    "contacts@"
#define     PIC_DSS_PREFIX                  THEME_PREFIX    "dss@"
#define     PIC_DSSBG_PREFIX                THEME_PREFIX    "dssbg@"
#define     PIC_GENERAL_PREFIX          THEME_PREFIX    "general@"
#define     PIC_ICON_PREFIX                 THEME_PREFIX    "icon@"
#define     PIC_MENU_PREFIX             THEME_PREFIX    "menu@"
#define     PIC_MSGBOX_PREFIX           THEME_PREFIX    "msgbox@"
#define     PIC_SCROLL_PREFIX               THEME_PREFIX    "scroll@"
#define     PIC_VOLUME_PREFIX           THEME_PREFIX    "volume@"
#define     PIC_WIDGET_PREFIX               THEME_PREFIX    "widget@"
#define     PIC_WIFI_PREFIX                 THEME_PREFIX    "wifi@"
#define     PIC_THEME1_PREFIX               THEME_PREFIX    "Theme1@"

// 定义general图片路径
#define  PIC_GENERAL_SELECT_BG1     PIC_GENERAL_PREFIX "gen_select_bg1.png"

// 定义arrow图片路径
#define     PIC_ARROW_UP1                   PIC_ARROWS_PREFIX "arrows_up1.png"
#define     PIC_ARROW_UP2                   PIC_ARROWS_PREFIX "arrows_up2.png"
#define     PIC_ARROW_DOWN1         PIC_ARROWS_PREFIX "arrows_down1.png"
#define     PIC_ARROW_DOWN2         PIC_ARROWS_PREFIX "arrows_down2.png"

// 定义msgBox图片路径
#define     PIC_MSGBOX_BG1              PIC_MSGBOX_PREFIX "msgbox1.9.png"
#define     PIC_MSGBOX_ICON             PIC_MSGBOX_PREFIX "msgbox_icon.png"
#define     PIC_MSGBOX_CUTLINE          PIC_MSGBOX_PREFIX "msgbox_cutline.png"
#define     PIC_MSGBOX_PASSWORDICON     PIC_MSGBOX_PREFIX "msgbox_passwordicon.png"

#define     PIC_MENU_ACCOUNT            PIC_MENU_PREFIX "menu_account.png"
#define     PIC_MENU_AA                     PIC_MENU_PREFIX "menu_aa.png"
#define     PIC_MENU_UC_CONTACT     PIC_MENU_PREFIX "menu_uc_contact.png"

#define     PIC_ICON_CALL_IN                PIC_ICON_PREFIX "icon_call_in.png"

// 定义volume图片路径
#define     PIC_VOLUME_FULL             PIC_VOLUME_PREFIX "volume_full.png"
#define     PIC_VOLUME_EMPTY            PIC_VOLUME_PREFIX "volume_empty.png"
#define     PIC_VOLUME_BG               PIC_VOLUME_PREFIX "volume_background.png"
#define     PIC_VOLUME_SLIDER           PIC_VOLUME_PREFIX "volume_slider.png"

#endif//PIC_PATH_H

//#endif

