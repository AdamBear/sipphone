#ifndef __ELEMENT_SCREENSAVER_H__
#define __ELEMENT_SCREENSAVER_H__

#ifdef IF_FEATURE_XMLBROWSER
#if IF_FEATURE_XMLB_SCREENSAVER

// Element ScreenSaver类
class CElementScreenSaver : public CElementHaveUIBase
{
public:
    CElementScreenSaver();
    virtual ~CElementScreenSaver();

    // 创建之后的处理
    virtual bool AfterCreate();
    // 销毁之前的处理
    virtual void BeforeDestroy();
    // 是否播放提示音
    virtual bool IsNeedPlayBeep()
    {
        return false;
    }
    // 对UI事件的处理
    virtual bool OnAction(XB_ACTION_TYPE eAction, void * pData, bool & bExitWnd, bool & bUpdate,
                          bool bDestroyOnExit = false);
    // softkey处理
    virtual bool ProcessSoftkey(XB_SOFTKEY_TYPE eType, int iFocusedItem, bool & bUpdate);

private:

    // 获取Image
    ImageItem * GetImageItem();
    // 下载图片文件
    bool DownloadImage();
    // 下载完成的处理
    bool OnDownloadDone(const char * cszFilePath);

    // 压缩图片
    bool CompressImage(CompressImgParam * pImgParam);
    // 压缩完成
    bool OnCompressDone(CompressImgParam * pImgParam);

    IMPLEMENT_GETXDATA(XBScreenSaverData);

private:
    // 传给UI的数据
    XBScreenSaverData   m_dataToUI;
};

#endif // #if IF_FEATURE_XMLB_SCREENSAVER
#endif // #ifdef IF_FEATURE_XMLBROWSER
#endif  // __ELEMENT_IMAGE_SCREEN_H__
