#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

class CResourceManager
{
public:
    CResourceManager(void);
    virtual ~CResourceManager(void);

public:
    //预览声音，图片，视频
    virtual void Preview() = 0;
    //播放声音，图片，视频
    virtual void Play() = 0;
    //停止播放
    virtual void Pause() = 0;
    //设置为系统使用
    virtual void SetToSystem() = 0;
    //删除铃声，图片，视频
    virtual void Delete() = 0;
    //资源删除后操作处理
    virtual void AfterDelete() = 0;
    //资源删除前操作处理
    virtual void BeforeDelete() = 0;
    //跳转到上个
    virtual void SkipLastOne() = 0;
    //跳转到下个
    virtual void SkipNextOne() = 0;
};
#endif
