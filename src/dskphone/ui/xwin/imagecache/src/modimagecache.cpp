#include "modimagecache.h"
#include "imagecachemanager.h"


void Image_Init()
{
    g_ImageCacheMgr.Init();
}

void Image_UnInit()
{
    g_ImageCacheMgr.UnInit();
}

bool Image_RegisterClassifier(CImageClassifier* pClassifier)
{
    return g_ImageCacheMgr.RegisterClassifier(pClassifier);
}

IMAGE_SIZE_TYPE Image_CheckImageSize(const yl::string& strPath)
{
    return g_ImageCacheMgr.CheckImageSize(strPath);
}

bool Image_LoadImage(const yl::string& strPath, int nClassifyType, bool bReload/* = false*/)
{
    return g_ImageCacheMgr.LoadImage(strPath, nClassifyType, bReload);
}

bool Image_LoadImage(const yl::string& strFilePath, int nClassifyType, xPixmap& pic, bool bReload/* = false*/)
{
    return g_ImageCacheMgr.LoadImage(strFilePath, nClassifyType, pic, bReload);
}

bool Image_AddImage(const yl::string& strFilePath, int nClassifyType, const char* pFlow,
                    int nFlowSize, bool bReload/* = false*/)
{
    return g_ImageCacheMgr.AddImage(strFilePath, nClassifyType, pFlow, nFlowSize, bReload);
}

bool Image_RemoveImage(const yl::string& strFilePath, int nClassifyType/* = IMG_CLASSIFY_NONE*/)
{
    return g_ImageCacheMgr.RemoveImage(strFilePath, nClassifyType);
}

void Image_ClearImage(int nClassifyType/* = IMG_CLASSIFY_NONE*/)
{
    return g_ImageCacheMgr.ClearImage(nClassifyType);
}

xPixmap Image_GetImage(const yl::string& strFilePath)
{
    xPixmap pmImage;

    g_ImageCacheMgr.GetImage(strFilePath, pmImage);

    return pmImage;
}

bool Image_GetImage(xPixmap& pic, const yl::string& strFilePath, int nClassifyType/* = IMG_CLASSIFY_NONE*/)
{
    return g_ImageCacheMgr.GetImage(strFilePath, nClassifyType, pic);
}

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
void Image_ClearScreenGraphic(int nClassifyType/* = IMG_CLASSIFY_NONE*/)
{
    return g_ImageCacheMgr.ClearScreenGraphic(nClassifyType);
}

void Image_ClearScreenGraphic(const yl::string& strFilePath)
{
    return g_ImageCacheMgr.ClearScreenGraphic(strFilePath);
}
#endif

#if IF_IMAGE_MEMORY_DUMP
void Image_Dump(int nPageSize)
{
    return g_ImageCacheMgr.Dump(nPageSize);
}
#endif
