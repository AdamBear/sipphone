#ifndef __IMAGE_CLASSIFIER_H__
#define __IMAGE_CLASSIFIER_H__

#include <ETLLib/ETLLib.hpp>
#include "imagecachedefine.h"
#include "imageinfo.h"

enum
{
    IMAGE_CLASSIFIER_TYPE_BASE                  = 0x00000000,
    IMAGE_CLASSIFIER_TYPE_SCALE                 = 0x01000000,
    IMAGE_CLASSIFIER_TYPE_SCALE_BY_PROPORTION   = 0x01010000,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// 基本图片分类器
class CImageClassifier
{
public:
    explicit CImageClassifier(int nType = IMAGE_CLASSIFIER_TYPE_BASE);
    virtual ~CImageClassifier();

    enum
    {
        IMG_ACTION_NONE                             = 0x00000000,
        IMG_ACTION_DELETE_FILE_WHEN_FREE            = 0x00000001,
        IMG_ACTION_CONVERT_DEFAULT_FORMAT           = 0x00000002,
    };

public:
    int GetType() const
    {
        return m_nType;
    }
    int GetImageClassifyType() const
    {
        return m_nImageClassifyType;
    }
    int GetCacheType() const
    {
        return m_nCacheType;
    }
    int GetAction() const
    {
        return m_nAction;
    }
    bool IsAsyncLoad() const
    {
        return m_bAsyncLoad;
    }

    void SetImageClassifyType(int nClassifyType);
    void SetCacheType(int nCacheType);
    void SetAction(int nAction);
    void SetAsyncLoad(bool bAsync);

    virtual CImageInfoBase* AllocImageInfo();
    virtual void OnLoadImageInfo(CImageInfoBase* pImageInfo);
    virtual void OnFreeImageInfo(CImageInfoBase* pImageInfo);

protected:
    int m_nType;
    int m_nImageClassifyType;
    int m_nCacheType;
    int m_nAction;
    bool m_bAsyncLoad;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// 缩放到指定图片大小的分类器
class CScaleImageClassifier : public CImageClassifier
{
public:
    explicit CScaleImageClassifier(int nType = IMAGE_CLASSIFIER_TYPE_SCALE);
    virtual ~CScaleImageClassifier();

    static int GetScaleImageClassifierType();
    static CScaleImageClassifier* GetScalImageClassifier(CImageClassifier* pClassifier);

public:
    chSize GetImageSize() const
    {
        return m_sizeImage;
    }

    void SetImageSize(int nWidth, int nHeight);
    void SetImageSize(const chSize& size);

    virtual CImageInfoBase* AllocImageInfo();

    virtual chSize GetScaleSize(const xPixmap& pm);

    void SetUpdateFile(bool bUpdate);

    bool IsUpdateFile()
    {
        return m_bUpdate;
    }

protected:
    chSize m_sizeImage;
    bool m_bUpdate;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// 超过设定最大值按等比例缩放图片大小的分类器
class CProportionImageClassifier : public CScaleImageClassifier
{
public:
    explicit CProportionImageClassifier(int nType = IMAGE_CLASSIFIER_TYPE_SCALE_BY_PROPORTION);
    virtual ~CProportionImageClassifier();

    static int GetProportionImageClassifierType();
    static CProportionImageClassifier* GetPropertionImageClassifier(CImageClassifier* pClassifier);

public:
    virtual chSize GetScaleSize(const xPixmap& pm);
};

#endif
