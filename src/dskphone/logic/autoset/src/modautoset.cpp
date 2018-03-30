#ifdef IF_FEATURE_AUTOTEST
#include "autoset_inc.h"

// 自动设置初始化
void autoset_Init()
{
    _AutoSetManager.Init();
}

void autoset_CheckAutoSetAfterTalk()
{
    _AutoSetManager.DelayUpdate();
}

#else

void autoset_Init()
{
}

void autoset_CheckAutoSetAfterTalk()
{
}

#endif  // IF_FEATURE_AUTOTEST

