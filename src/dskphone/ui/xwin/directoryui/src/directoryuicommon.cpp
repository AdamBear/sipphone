#include "directoryuicommon.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "language/input/include/modInput.h"
#include "baseui/include/inputmethod.h"


const char * DIR_UI_LAYOUT_FILE_DIRECTORY = "directory/dlgdirectory.xml";
const char * DIR_UI_LAYOUT_FILE_SEARCH = "directory/dlgsearch.xml";
const char * DIR_UI_LAYOUT_FILE_CUSTOM_PHOTO = "directory/dlgcustomphoto.xml";
const char * DIR_UI_ID_NORMAL_TITLE = "boxTitle";
const char * DIR_UI_ID_TAG_TITLE = "boxTags";
const char * DIR_UI_ID_TITLE_PAGE_INFO = "txtPageInfo";
const char * DIR_UI_LAYOUT_FILE_CONTACT_ITEM = "directory/contactitem.xml";
const char * DIR_UI_LAYOUT_FILE_CALLLOG_ITEM = "directory/calllogitem.xml";
const char * DIR_UI_ID_TXT_NAME = "txtName";
const char * DIR_UI_ID_TXT_INFO = "txtInfo";
const char * DIR_UI_ID_PIC_ICON = "picIcon";


void DIRUI_GetNameIME(yl::string & strSupportIME, yl::string & strIME)
{
    strIME = configParser_GetConfigString(kszContactDefaultIME);

    bool bHaveCustomIme = Ime_GetCustomImeCount() > 0;

    if (strIME.empty())
    {
        strIME = bHaveCustomIme ? IME_DIY : IME_Abc;
    }

    if (bHaveCustomIme)
    {
        strSupportIME = IME_CUSTOM;
    }
    else
    {
        strSupportIME = IME_Name;
    }
}

void DIRUI_GetSearchIME(yl::string & strSupportIME, yl::string & strIME)
{
    strIME = configParser_GetConfigString(kszContactSearchDefaultIME);

    bool bCustomIme = Ime_GetCustomImeCount() > 0;

    if (strIME.empty())
    {
        strIME = bCustomIme ? IME_DIY : IME_Abc;
    }

    if (bCustomIme)
    {
        strSupportIME = IME_CUSTOM;
    }
    else
    {
        strSupportIME = IME_Name;
    }
}
