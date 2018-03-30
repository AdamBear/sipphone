#ifndef __PICTURE_RECORD_H__
#define __PICTURE_RECORD_H__

#ifdef IF_USB_SUPPORT
#include "baserecord.h"
#include "../include/recordmessagedefine.h"
#include "commonunits/commonunits_def.h"
class CPictureRecord : public CBaseRecord
{

public:
    CPictureRecord();
    virtual ~CPictureRecord();

    // 操作执行
    virtual bool DoOperation(OPERATION_TYPE eOprType, const char * pFile = NULL,
                             void * pExtData = NULL, int iId = STORAGE_DEFAULT);

    // 消息处理
    virtual bool ProcessMsg(msgObject & msg);

    // 用来判断空间是否足够
    virtual bool IsAvaliable(unsigned long long dSize);

private:
    // 文件夹名
    virtual yl::string GetFolder();

    // 额外的文件夹名
    virtual yl::string GetFolderEx();

    // 后缀名
    virtual yl::string GetSuffix();

    // 支持的格式
    virtual yl::string GetSupportFormat();

    // 图片处理,并且获取隐藏路径
    Pic_Set_Result ProcessPicAndGetHidePath(const char * pFile, const yl::string & pDstDir,
                                            PHFileType eFileType, OPERATION_TYPE eOpType, yl::string & strHidePath);

    // 拷贝文件
    bool Copy(const char * pDstFile, const char * pSrcFile);

    // 设置话机壁纸 iWallPaperType: 0/1  idle壁纸/dsskey壁纸
    bool SetDefaultWallPaper(const char * pFile, int iWallPaperType);

    // 设置EXP壁纸
    bool SetExpWallPaper(const char * pFile);

    // 设置ScreenSaver
    bool SetScreenSaver(const char * pFile);

    // 获取设备像素大小
    bool GetDeviceWidthAndHeight(OPERATION_TYPE eType, int & iWidth, int & iHeight);
    inline IMAGE_SCALED_TYPE  OPERATION_TYPE2IMAGE_TYPE(OPERATION_TYPE eType)
    {
        switch (eType)
        {
        case OPERATION_SET_SCREENSAVER:
        case OPERATION_SET_DEFAULT:
        case OPERATION_SET_EXP_WALLPAPER:
            return WALL_PAPER_IMAGE;
        default:
            return NOMAL_IMAGE;
        }
    };
};

#endif // IF_USB_RECORD
#endif // __PICTURE_RECORD_H__
