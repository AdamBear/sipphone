#ifndef __IDLEUI_COMMON_H__
#define __IDLEUI_COMMON_H__

#include "talk/talklogic/include/uiandlogic_common.h"
#include "idleuilogic/include/idlehandledefine.h"

// ͼ������תͼƬ·��
bool IconType2PicPath(ICON_TYPE eIconType, yl::string& strIcon);

// DT���뷽ʽת����ͨ���뷽ʽ
int GetAlignByDTAlign(int nDTAlign);

// DT���뷽ʽת����ͨ���뷽ʽ
void EnterPage(IDLE_ENTER_PAGE_TYPE_E eEnterPageType);


#endif

