#ifndef PHOTO_MANAGER_H
#define PHOTO_MANAGER_H

#include "resourcemanager.h"

class CPhotoManager: public CResourceManager
{
public:
    CPhotoManager();
    virtual ~CPhotoManager();

public:
    //预览图片
    virtual void Preview() {}
    //播放图片
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
    //virtual void SkipLastOne(){}
    virtual void Prev() {}
    //跳转到下个
    //virtual void SkipNextOne(){}
    virtual void Next() {}

    //设置全屏模式
    virtual void SetFullScreenModel(bool bIsFullScreen) = 0;
};
#endif

