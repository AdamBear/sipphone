#ifndef __IDLEUI_COMMON_H__
#define __IDLEUI_COMMON_H__

#include "talk/talklogic/include/uiandlogic_common.h"
#include "idleuilogic/include/idlehandledefine.h"

// 图标类型转图片路径
bool IconType2PicPath(ICON_TYPE eIconType, yl::string& strIcon);

// DT对齐方式转换普通对齐方式
int GetAlignByDTAlign(int nDTAlign);

// DT对齐方式转换普通对齐方式
void EnterPage(IDLE_ENTER_PAGE_TYPE_E eEnterPageType);


#endif

