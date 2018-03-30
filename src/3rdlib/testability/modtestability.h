#ifndef __COLLIE_LOADTESTABILITY_H__
#define __COLLIE_LOADTESTABILITY_H__

//开关用的userenv
#define userenvGuiTestability "GuiTestability"

//配置改变ID，0xff000000 + 服务端口号
static const int ST_TESTABILITY_CHANGED = 0xff000000 + 65381;


/**
* \brief 初始化测试模块，需要在进入消息循环后调用
*/
void Init_Testablity();

#endif //__COLLIE_LOADTESTABILITY_H__