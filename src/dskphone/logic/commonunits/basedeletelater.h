#ifndef __BASE_DELETE_LATER_H__
#define __BASE_DELETE_LATER_H__

#if 1
#include "talk/talkproxy/include/deletelater.h"

typedef app::CBaseDeleteLater  CBaseDeleteLater;

#else
// 需要延迟删除的基类
class CBaseDeleteLater
{
public:
    CBaseDeleteLater();
    virtual ~CBaseDeleteLater();

    // 延迟删除
    virtual void deleteLater();
};
#endif

#endif // __BASE_DELETE_LATER_H__
