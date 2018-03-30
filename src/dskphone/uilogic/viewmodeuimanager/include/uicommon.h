#ifndef _UI_COMMON_H_
#define _UI_COMMON_H_

#include "ylstring.h"
#include "../../viewmodeuimanager/include/viewmodeuidefine.h"

// softkey action转换为softkey type
int UICommonAPI_MapSoftkeyAction2Type(const yl::string & strAction);

// softkey type转换为softkey action
yl::string UICommonAPI_MapSoftkeyType2Action(int iSoftkeyType);

// option action转换为softkey type
int UICommonAPI_MapOptionAction2Type(const yl::string & strAction);

// option type转换为softkey action
yl::string UICommonAPI_MapOptionType2Action(int iSoftkeyType);

// 获取翻译
yl::string UIConmonAPI_Lang(const char * strSrc);
yl::string UIConmonAPI_Lang(const yl::string & strSrc);
#endif //_UI_COMMON_H_
