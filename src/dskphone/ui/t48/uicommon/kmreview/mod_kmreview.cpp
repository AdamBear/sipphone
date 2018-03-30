
#include "km_review.h"
#include "mod_kmreview.h"
#include "dsklog/log.h"

// 记录按键信息
void mod_KM_RecordKey(int nCode, bool bDown)
{
#ifdef KM_REVIEW_SUPPORT
	_KM_Manager.Record_Key(nCode, bDown);
#endif
}

// 记录鼠标信息
void mod_KM_RecordMouse(int x, int y, bool bDown)
{
#ifdef KM_REVIEW_SUPPORT
	_KM_Manager.Record_Mouse(x, y, bDown);
#endif
}

// 复位记录信息
void mod_KM_Restart()
{
#ifdef KM_REVIEW_SUPPORT
	_KM_Manager.RestartRecord();
#endif
}
