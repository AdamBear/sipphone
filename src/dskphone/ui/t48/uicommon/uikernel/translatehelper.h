#ifndef _TRANSLATE_HELPER_
#define _TRANSLATE_HELPER_

#include <ylstring.h>

enum TRAN_TEXT_TYPE
{
    TRAN_MENU_LIST,
    TRAN_MENU_ICON,
    TRAN_PAGE_BD_CALLLOG,
    TRAN_PAGE_DIR,
};

bool IsNeedTranslateByAction(TRAN_TEXT_TYPE eTranType,
                             const yl::string & strDisplayName,
                             const yl::string & strClickAction = "");

#endif
