#pragma once

#include "scopedtimer.h"
#include "helpclass.h"

namespace app
{

/* 延迟删除对象 */
struct CBaseDeleteLater
    : protected CScopedTimer
{
protected:
    /* 限制为堆对象 */
    CBaseDeleteLater()
        : CScopedTimer()
    {}

    virtual ~CBaseDeleteLater()
    {}

public:
    /* async delete */
    void deleteLater()
    {
        BeforeDelete();
        SetTimerOnce(10);
    }

    /* sync delete */
    void destory()
    {
        delete this;
    }

protected:
    virtual void BeforeDelete()
    {}

private:
    //! warrring: 不应该继承这个接口
    bool OnTimeOut(msgObject& msg);
};

} // namespace

