#include "autotesthelp.h"
#include "uimanager/uimanager_common.h"
#include "uimanager/basedialog.h"
#include "path_define.h"
#include "ylstl/ylstring.h"

// dialog type string
#define IDLE          "idle"
#define MENU          "menu"
#define SUBMENU       "submenu"
#define SUBPAGE       "subpage"
#define ZEROSPTOUCH   "zerosptouch"
#define TALK          "talk"
#define DIRECTORY     "directory"
#define CALLLOG       "calllog"

#define ECHO_FILE     "echo>"TEMP_PATH"currentScreen currentPage="

void AutoTestHelp(const CBaseDialog * pDialog)
{
    if (NULL == pDialog)
    {
        return;
    }

    yl::string strScreen(ECHO_FILE);
    switch (pDialog->GetWindowType())
    {
    case eWT_Idle:
        {
            strScreen += IDLE;
            system(strScreen.c_str());
        }
        break;
    case eWT_SubPage:
        {
            strScreen += SUBPAGE;
            system(strScreen.c_str());
        }
        break;
    case eWT_Talk:
        {
            strScreen += TALK;
            system(strScreen.c_str());
        }
        break;
    case eWT_Directory:
        {
            strScreen += DIRECTORY;
            system(strScreen.c_str());
        }
        break;
    case eWT_CallLog:
        {
            strScreen += CALLLOG;
            system(strScreen.c_str());
        }
        break;
    case eWT_Menu:
        {
            strScreen += MENU;
            system(strScreen.c_str());
        }
        break;
    case eWT_SubMenu:
        {
            strScreen += SUBMENU;
            system(strScreen.c_str());
        }
        break;
    case eWT_ZeroSPTouch:
        {
            strScreen += ZEROSPTOUCH;
            system(strScreen.c_str());
        }
        break;
    default:
        break;
    }
}
