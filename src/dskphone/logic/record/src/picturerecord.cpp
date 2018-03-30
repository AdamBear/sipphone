#include "record_inc.h"

#ifdef IF_USB_SUPPORT
#include "adapterbox/include/modadapterbox.h"
#include "exp/src/expdrive/deviceplugin/modexpscan.h"
CPictureRecord::CPictureRecord()
{
    m_eType = RECORD_PICTURE;
}

CPictureRecord::~CPictureRecord()
{
}

// 文件夹名
yl::string CPictureRecord::GetFolder()
{
    return "Screenshot";
}

// 额外的文件夹名
yl::string CPictureRecord::GetFolderEx()
{
    return yl::string("/");
}

// 后缀名
yl::string CPictureRecord::GetSuffix()
{
    return ".jpg";
}

// 支持的格式
yl::string CPictureRecord::GetSupportFormat()
{
    return "jpg,bmp,png,jpeg";
    //return yl::string("jpg") + yl::string(",") + yl::string("bmp") + yl::string(",") + yl::string("png");
}

// 操作执行
bool CPictureRecord::DoOperation(OPERATION_TYPE eOprType, const char * pFile/* = NULL*/,
                                 void * pExtData/* = NULL*/, int iId /*= STORAGE_DEFAULT*/)
{
    bool bSuccess = false;

    switch (eOprType)
    {
    case OPERATION_SET_DEFAULT:
        {
            bSuccess = SetDefaultWallPaper(pFile, 0);
        }
        break;
    case OPERATION_SET_EXP_WALLPAPER:
        {
            bSuccess = SetExpWallPaper(pFile);
        }
        break;
    case OPERATION_SET_SCREENSAVER:
        {
            bSuccess = SetScreenSaver(pFile);
        }
        break;
    case OPERATION_SET_DSSEKY_DEFAULT:
        {
            bSuccess = SetDefaultWallPaper(pFile, 1);
        }
        break;
    default:
        {
            bSuccess = CBaseRecord::DoOperation(eOprType, pFile, pExtData);
        }
        break;
    }

    return bSuccess;
}

bool CPictureRecord::ProcessMsg(msgObject & msg)
{
    bool bHandle = true;
    switch (msg.message)
    {
    case HOTPLUG_USB_STORAGE_STATE_CHANGING:
        {
            // usb 存储设备连接上, 则去获取文件列表信息
            if (1 == msg.wParam)
            {
                RequestLoadFileList(msg.lParam);
            }

            bHandle = false;
        }
        break;
    case RECORD_STORAGE_RECORD_RESULT:
        {
            // 截图成功 才需要处理
            if (2 != msg.wParam || !msg.lParam)
            {
                break ;
            }

            const char * pFile = (const char *)msg.GetExtraData();
            if (NULL != pFile && 0 != strlen(pFile))
            {
                AddNewFile(pFile);
            }
        }
        break;
    default:
        bHandle = false;
        break;
    }

    if (!bHandle)
    {
        CBaseRecord::ProcessMsg(msg);
    }

    return true;
}

bool CPictureRecord::IsAvaliable(unsigned long long dSize)
{
    return (dSize > MB(2));
}

// 图片处理,并且获取隐藏路径
Pic_Set_Result CPictureRecord::ProcessPicAndGetHidePath(const char * pFile,
        const yl::string & pDstDir, PHFileType eFileType,
        OPERATION_TYPE eOpType, yl::string & strHidePath)
{
    RecordFileInfo * pInfo = GetFileInfo(pFile);
    if (NULL == pInfo || !pathFileExist(pFile))
    {
        return PIC_SET_RST_FILE_NOT_EXIST;
    }
    // 大小限制不由逻辑层限制，由接口内部统一实现
    // http://bugfree.yealink.com/Bug.php?BugID=108233
    //// 容量超过5M，
    //if (pInfo->lSize > MB(5))
    //{
    //  return PIC_SET_RST_FILE_TOO_LARGE;
    //}
    // 若无此目录, 则创建
    yl::string strWallPathDir = pDstDir;
    if (!pathFileExist(strWallPathDir.c_str()))
    {
        pathCreateDirectory(strWallPathDir.c_str());
    }
    yl::string strWallPaperPath = strWallPathDir + "/" + pInfo->strName;

    int iRet = AdapterBox_ImageScale(pFile, strWallPaperPath.c_str(),
                                     (int)OPERATION_TYPE2IMAGE_TYPE(eOpType), 1280, 800);
    // 根据压缩图片的返回结果处理
    IMAGE_SCALE_RETURN_TYPE  eRet = static_cast<IMAGE_SCALE_RETURN_TYPE>(iRet);
    Pic_Set_Result eResult = PIC_SET_RST_ERROR;
    switch (eRet)
    {
    case IMAGE_SCALED_SUCCESS:
        // 成功后将路径转化为隐藏路径
        strHidePath = strWallPaperPath;
        commonUnits_Real2Hide(strHidePath, eFileType);
        eResult = PIC_SET_RST_SUCCESS;
        break;
    case IMAGE_NO_ENOUGH_SPACE:
        // data分区大小不够
        eResult = PIC_SET_RST_PARTITION_NOT_ENOUGH;
        break;
    case IMAGE_SIZE_OVER:
        // 图片本身过大，上面的 5 MB 判断用来保险
        eResult =  PIC_SET_RST_FILE_TOO_LARGE;
        break;
    case IMAGE_SCALED_FAILE:
    default:
        // 压缩失败
        break;
    }
    return eResult;
}

// 拷贝文件
bool CPictureRecord::Copy(const char * pDstFile, const char * pSrcFile)
{
    chFileStdio fileSrc, fileDst;
    if (!fileSrc.Open(pSrcFile, chFileStdio::modeRead)
            || !fileDst.Open(pDstFile, chFileStdio::modeCreateWrite))
    {
        return false;
    }

    // 源文件内容空, 只需创建目标文件
    if (fileSrc.GetFileLength() <= 0)
    {
        return true;
    }

    BYTE buf[4096];
    while (!fileSrc.IsEOF())
    {
        int nBytes = fileSrc.Read(buf, 1, 4096);
        if (nBytes <= 0)
        {
            return false;
        }

        if (!fileDst.Write(buf, 1, nBytes))
        {
            return false;
        }
    }
    return true;
}

// 设置话机壁纸 iWallPaperType: 0/1  idle壁纸/dsskey壁纸
bool CPictureRecord::SetDefaultWallPaper(const char * pFile, int iWallPaperType)
{
    yl::string strHidePath;
    yl::string strDstDir = USER_DATA_PATH "wallpaper";

    Pic_Set_Result eResult = ProcessPicAndGetHidePath(pFile, strDstDir, PHFILE_TYPE_WALLPAPER_FILE,
                             OPERATION_SET_DEFAULT, strHidePath);
    if (PIC_SET_RST_SUCCESS == eResult)
    {
        if (0 == iWallPaperType)
        {
            configParser_SetConfigString(kszCurrentWallPaper, strHidePath.c_str(), CONFIG_LEVEL_PHONE);
        }
        else if (1 == iWallPaperType)
        {
            configParser_SetConfigString(kszDsskeyWallPaper, strHidePath.c_str(), CONFIG_LEVEL_PHONE);
        }
    }
    etl_NotifyApp(false, RECORD_SET_DEFAULT_WALLPAPER, eResult, 0);

    return PIC_SET_RST_SUCCESS == eResult;
}

// 设置EXP壁纸
bool CPictureRecord::SetExpWallPaper(const char * pFile)
{
    yl::string strHidePath;
    yl::string strDstDir = USER_DATA_PATH "wallpaper";

    Pic_Set_Result eResult = ProcessPicAndGetHidePath(pFile, strDstDir, PHFILE_TYPE_EXP_WALLPAPER_FILE,
                             OPERATION_SET_EXP_WALLPAPER, strHidePath);
    if (PIC_SET_RST_SUCCESS == eResult)
    {
        configParser_SetConfigString(kszCurrentExpWallPaper, strHidePath.c_str(), CONFIG_LEVEL_PHONE);
    }
    etl_NotifyApp(false, RECORD_SET_EXP_WALLPAPER, eResult, 0);

    return PIC_SET_RST_SUCCESS == eResult;
}

// 设置ScreenSaver
bool CPictureRecord::SetScreenSaver(const char * pFile)
{
    yl::string strHidePath;
    yl::string strDstDir = USER_DATA_PATH "screensaver";

    Pic_Set_Result eResult = ProcessPicAndGetHidePath(pFile, strDstDir, PHFILE_TYPE_SCREENSAVER_FILE,
                             OPERATION_SET_SCREENSAVER, strHidePath);
    if (PIC_SET_RST_SUCCESS == eResult)
    {
        // 通知UI更新屏保图片
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SCREEN_SAVER_PIC, 0);
    }
    // 通知UI屏保图片设置结果, 不复用消息参数ST_SCREEN_SAVER_PIC是因为和网页使用相同
    // 如果在预览图片时,操作网页,可能造成UI不必要的提示
    etl_NotifyApp(false, RECORD_SET_SCREENSAVER, eResult, 0);

    return PIC_SET_RST_SUCCESS == eResult;
}

// 获取设备像素大小
bool CPictureRecord::GetDeviceWidthAndHeight(OPERATION_TYPE eType, int & iWidth, int & iHeight)
{
    bool bHandle = false;

    if (OPERATION_SET_DEFAULT == eType
            || OPERATION_SET_SCREENSAVER == eType)
    {
        if (PT_T54S == devicelib_GetPhoneType())
        {
            iWidth = 480;
            iHeight = 272;
            bHandle = true;
        }
        else if (PT_T52 == devicelib_GetPhoneType())
        {
            iWidth = 320;
            iHeight = 240;
            bHandle = true;
        }
    }
    else if (OPERATION_SET_EXP_WALLPAPER == eType)
    {
        // UI不支持扩展台壁纸分别设置,所以获取第一个
        if (DT_EXP_50 == exp_GetExpTypeByIndex(0))
        {
            iWidth = 480;
            iHeight = 272;
            bHandle = true;
        }
    }

    return bHandle;
}

#endif // IF_USB_RECORD
