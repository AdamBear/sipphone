#ifndef __IMAGE_INFO_H__
#define __IMAGE_INFO_H__

#include "xwindow/xGraphic.h"
#include "imagecachedefine.h"
#include "commonunits/basedeletelater.h"


enum
{
    IMAGE_INFO_TYPE_BASE        = 0x00000000,
    IMAGE_INFO_TYPE_SCALE       = 0x01000000,
};

class CImageClassifier;
///////////////////////////////////////////////////////////////////////////////////////////////////
// 图片信息基类
class CImageInfoBase : public CBaseDeleteLater
{
public:
    explicit CImageInfoBase(int nType = IMAGE_INFO_TYPE_BASE, int nClassifyType = IMG_CLASSIFY_NONE);
    virtual ~CImageInfoBase();

public:
    int GetType()
    {
        return m_nType;
    }
    int GetClassifyType()
    {
        return m_nClassifyType;
    }
    const yl::string& GetFilePath() const
    {
        return m_strFilePath;
    }
    xPixmap& GetPixmap()
    {
        return m_pixmap;
    }
    bool IsNull() const
    {
        return m_pixmap.isNull();
    }

    void SetClassifyType(int nClassifyType);
    void SetFilePath(const yl::string& strFilePath);

    virtual bool LoadPixmap(const char* pFlow, int nFlowSize);
    virtual bool LoadPixmap(const yl::string& strFile);

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
    void DetachScreenGraphic();
#endif

#if IF_IMAGE_MEMORY_DUMP
    void Dump(int nPageSize, int& nSize, int& nMemSize);
#endif

protected:
    CImageClassifier* GetClassifier();

protected:
    int         m_nType;
    int         m_nClassifyType;
    yl::string  m_strFilePath;
    xPixmap     m_pixmap;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// 缓存的图片对象,并拉伸到指定大小
class CScaleImageInfo : public CImageInfoBase
{
public:
    explicit CScaleImageInfo(int nType = IMAGE_INFO_TYPE_SCALE);
    virtual ~CScaleImageInfo();

public:
    virtual bool LoadPixmap(const char* pFlow, int nFlowSize);
    virtual bool LoadPixmap(const yl::string& strFilePath);

private:
    bool ScaleFromPixmap(xPixmap& src, const chSize& size);
};

#endif
