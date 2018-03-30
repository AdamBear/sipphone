#ifndef __ELEMENT_IMAGE_SCREEN_H__
#define __ELEMENT_IMAGE_SCREEN_H__

#ifdef IF_FEATURE_XMLBROWSER

// Element ImageScreen类
class CElementImageScreen : public CElementHaveUIBase
{
public:
    CElementImageScreen();
    virtual ~CElementImageScreen();

    // 创建之后的处理
    virtual bool AfterCreate();
    // 销毁之前的处理
    virtual void BeforeDestroy();
    // 是否播放提示音
    virtual bool IsNeedPlayBeep();
    // softkey处理
    virtual bool ProcessSoftkey(XB_SOFTKEY_TYPE eType, int iFocusedItem, bool & bUpdate);
    // 对UI事件的处理
    virtual bool OnAction(XB_ACTION_TYPE eAction, void * pData, bool & bExitWnd, bool & bUpdate,
                          bool bDestroyOnExit = false);
    // 获取图片的显示方式
    IMAGE_MODE GetImageMode()
    {
        return m_dataAttr.eMode;
    };
#if IF_BUG_32431
    virtual bool UpdateData(XMLParseData * pParseData, bool & bUpdate);
#endif

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

    IMPLEMENT_GETXDATA(XBImageScreenData);

private:
    // 保存属性
    ImageAttr   m_dataAttr;
    // 保存ImageList
    List_Image_Item m_ListImage;
    // 传给UI的数据
    XBImageScreenData   m_dataToUI;
#if IF_BUG_32431
    bool        m_bAutoRefresh;
#endif
};

#endif // #ifdef IF_FEATURE_XMLBROWSER

#endif  // __ELEMENT_IMAGE_SCREEN_H__
