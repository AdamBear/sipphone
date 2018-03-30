#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "resourcemanager.h"

class CSoundManager : public CResourceManager
{
public:
    CSoundManager();
    virtual ~CSoundManager();

public:
    //预览声音
    virtual void Preview() {}
    //播放声音
    virtual void Play() {}
    //停止播放
    virtual void Pause() {}
    //删除
    virtual void Delete() {}
    //资源删除后操作处理
    virtual void AfterDelete() {}
    //资源删除前操作处理
    virtual void BeforeDelete() {}
    //设置为系统使用
    virtual void SetToSystem() {}
    //跳转到上个
    virtual void SkipLastOne() {}
    //跳转到下个
    virtual void SkipNextOne() {}

};

#endif // SOUNDMANAGER_H
