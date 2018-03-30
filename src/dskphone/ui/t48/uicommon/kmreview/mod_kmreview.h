#ifndef __MOD_KMREVIEW_H__
#define __MOD_KMREVIEW_H__

///////////////////////////////////////////////////////////////////
// 给外部使用的接口
///////////////////////////////////////////////////////////////////

// 复位记录信息
void mod_KM_Restart();

// 记录按键信息
void mod_KM_RecordKey(int nCode, bool bDown);

// 记录鼠标信息
void mod_KM_RecordMouse(int x, int y, bool bDown);

#endif //__MOD_KM_REVIEW_H__

