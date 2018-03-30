#include "../include/uicommon.h"
#include "common/common_data_define.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "messagebox/messageboxbase.h"

#if IS_COLOR
//#include "/src/dskphone/ui/t2x/dotwindows/imeadaptive.h"
// 彩屏头文件
#else
//#include "dskphone/ui/t2x/commonui/common.h"
//#include "dskphone/ui/t2x/popupbox/popupbox_def.h"
//#include "dskphone/ui/t2x/popupbox/ilistener.h"
//#include "dskphone/ui/t2x/dotwindows/imeadaptive.h"
//#define _LANG(str)      CLangTransUiAdaptive::TransText(str)
#endif

int UICommonAPI_MapSoftkeyAction2Type(const yl::string & strAction)
{
    int iKeyType = ST_EMPTY;

    // example
    //iKeyType = strAction == "xxxxxx" ? ST_SCAN : ST_EMPTY;

    return iKeyType;
}

yl::string UICommonAPI_MapSoftkeyType2Action(int iSoftkeyType)
{
    yl::string strAction("");

    switch (iSoftkeyType)
    {
    // example
    //case ST_EMPTY:                strAction = "xxxx"; break;
    default:
        break;
    }

    return strAction;
}

int UICommonAPI_MapOptionAction2Type(const yl::string & strAction)
{
    int iKeyType = ST_EMPTY;

    // example
    //iKeyType = strAction == "xxxxxx" ? OPTION_DETAIL : OPTION_NONE;

    return iKeyType;
}

yl::string UICommonAPI_MapOptionType2Action(int iSoftkeyType)
{
    yl::string strAction("");

    switch (iSoftkeyType)
    {
    // example
    //case OPTION_NONE:         strAction = "xxxx"; break;
    default:
        break;
    }

    return strAction;
}

yl::string UIConmonAPI_Lang(const char * strSrc)
{
#if IS_COLOR
    // 彩屏翻译
    return _UILOGIC_LANG_TRAN(strSrc);
#else
    return _UILOGIC_LANG_TRAN(strSrc);
#endif
}

yl::string UIConmonAPI_Lang(const yl::string & strSrc)
{
#if IS_COLOR
    // 彩屏翻译
    return _UILOGIC_LANG_TRAN(strSrc.c_str());
#else
    return _UILOGIC_LANG_TRAN(strSrc.c_str());
#endif
}
