#ifndef __DIRECTORY_UI_COMMON_H__
#define __DIRECTORY_UI_COMMON_H__


#include "ylstring.h"
#include "ylvector.h"
#include "common/common_data_define.h"

enum
{
    BROWSER_PAGE_TYPE_NONE          = 0,    // 默认等级
    BROWSER_PAGE_TYPE_MENU          = 1,    // 菜单
#if IF_DIR_UI_SWITCH_GROUP_IN_BROWSE
    BROWSER_PAGE_TYPE_GROUP_SWITCH  = 2,    // 可以切换的群组
#endif
    BROWSER_PAGE_TYPE_NORMAL        = 3,    // 普通状态
};

typedef YLVector<yl::string> VEC_GROUP_TITLE;

extern const char * DIR_UI_LAYOUT_FILE_DIRECTORY;
extern const char * DIR_UI_LAYOUT_FILE_SEARCH;
extern const char * DIR_UI_LAYOUT_FILE_CUSTOM_PHOTO;
extern const char * DIR_UI_ID_NORMAL_TITLE;
extern const char * DIR_UI_ID_TAG_TITLE;
extern const char * DIR_UI_ID_TITLE_PAGE_INFO;
extern const char * DIR_UI_LAYOUT_FILE_CONTACT_ITEM;
extern const char * DIR_UI_LAYOUT_FILE_CALLLOG_ITEM;
extern const char * DIR_UI_ID_TXT_NAME;
extern const char * DIR_UI_ID_TXT_INFO;
extern const char * DIR_UI_ID_PIC_ICON;

enum SoftKey_TYPE_DIR
{
    ST_DELETE_GROUP         = 601,
    ST_DELETE_ALL_GROUP,
    ST_MOVE_CONTACT,
    ST_MOVE_TO_BLACKLIST,
    ST_COPY_TO_LOCAL_GROUP,
    ST_COPY_TO_BLACKLIST,
    ST_COPY_TO_PERSONAL,
    ST_BSFT_ENTER_PRECISE_SEARCH,
    ST_BSFT_PRECISE_SEARCH,
#ifdef IF_FEATURE_FAVORITE_DIR
    ST_REMOVE_FROM_FAVORITE,
    ST_ADD_TO_FAVORITE,
#endif
#if IF_FEATURE_GBBOOK
    ST_GBBOOK_PRECISE_SEARCH_MODE_CHOOSE,
    ST_GBBOOK_PRECISE_SEARCH_NAME,
    ST_GBBOOK_PRECISE_SEARCH_FIRST_NAME,
    ST_GBBOOK_PRECISE_SEARCH_LAST_NAME,
    ST_GBBOOK_PRECISE_SEARCH_NUMBER,
    ST_GBBOOK_PRECISE_SEARCH,
    ST_GBBOOK_ENTER_GROUP_MANAGER,
    ST_GBBOOK_EDIT_GROUP,
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    ST_MTSW_SORT,
    ST_MTSW_ADD_TO_LOCAL,
#endif
};

void DIRUI_GetNameIME(yl::string & strSupportIME, yl::string & strIME);
void DIRUI_GetSearchIME(yl::string & strSupportIME, yl::string & strIME);

#endif
