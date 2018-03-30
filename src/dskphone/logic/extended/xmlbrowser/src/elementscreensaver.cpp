#include "logic_inc.h"
#include "dsskey/src/dsskeyactionmanager.h"
#include "dsskey/src/dsskeyaction.h"

//// 包含远程文件下载的头文件
//#include "taskaction/modtaskaction.h"
//#include "taskaction/networkfileaction.h"

#ifdef IF_FEATURE_XMLBROWSER
#if IF_FEATURE_XMLB_SCREENSAVER

CElementScreenSaver::CElementScreenSaver()
{
}

CElementScreenSaver::~CElementScreenSaver()
{
    // 释放图片item链表
    // m_ListImage.clear();
}

// 创建之后的处理
bool CElementScreenSaver::AfterCreate()
{
    if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
    {
        // 下载图片
        DownloadImage();
    }

    return CElementHaveUIBase::AfterCreate();
}

// 销毁之前的处理
void CElementScreenSaver::BeforeDestroy()
{
    if (DSK_PHONE_TYPE_COLOR != commonUnits_GetDskPhoneType())
    {
        // 下载图片
        return;
    }

    // 退出ImageScreen时删除压缩后的图片
    List_Image_Item::iterator iter = _GetXData().listImage.begin();
    for (; iter != _GetXData().listImage.end(); iter++)
    {
        yl::string strPath = (*iter)->strImagePath;
        if (pathFileExist(strPath.c_str()))
        {
            remove(strPath.c_str());
            XMLBROWSER_INFO("delete image (%s)", strPath.c_str());
        }
    }

}

// 对UI事件的处理
bool CElementScreenSaver::OnAction(XB_ACTION_TYPE eAction, void * pData, bool & bExitWnd,
                                   bool & bUpdate, bool bDestroyOnExit/* = false*/)
{
    bool bHandled = true;
    switch (eAction)
    {
    case XB_ACTION_DOWNLOAD_DONE:
        {
            bUpdate = true;
            OnDownloadDone((const char *)pData);
        }
        break;
    case XB_ACTION_COMPRESS:
        {
            bUpdate = CompressImage((CompressImgParam *)pData);
        }
        break;
    case XB_ACTION_COMPRESS_DONE:
        {
            bUpdate = OnCompressDone((CompressImgParam *)pData);
        }
        break;
    default:
        bHandled = false;
        break;
    }

    // 其他事件交给基类处理
    if (!bHandled)
    {
        bHandled = CElementHaveUIBase::OnAction(eAction, pData, bExitWnd, bUpdate);
    }

    return bHandled;
}


// 获取Image
ImageItem * CElementScreenSaver::GetImageItem()
{
    List_Image_Item & listImage = _GetXData().listImage;
    if (listImage.size() == 0)
    {
        return NULL;
    }
    return *listImage.begin();
}

// 下载图片文件
bool CElementScreenSaver::DownloadImage()
{
    // 默认取第一张图片
    ImageItem * pImageItem = GetImageItem();
    if (pImageItem == NULL
            || !pImageItem->bUrlData)
    {
        return false;
    }
    if (_ElementManager.LoadImage(GetID(), pImageItem->strImageURL.c_str(), OPT_DOWNLOAD))
    {
        pImageItem->eLoadStatus = ILS_DOWNLOAD;
    }
    else
    {
        pImageItem->eLoadStatus = ILS_LOAD_FAIL;
    }
    return true;
}

// 下载完成的处理
bool CElementScreenSaver::OnDownloadDone(const char * cszFilePath)
{
    if (DSK_PHONE_TYPE_COLOR != commonUnits_GetDskPhoneType())
    {

        return false;
    }

    ImageItem * pImageItem = GetImageItem();
    if (pImageItem == NULL)
    {
        return false;
    }
    if (cszFilePath == NULL)
    {
        // 下载失败
        pImageItem->eLoadStatus = ILS_LOAD_FAIL;
        return true;
    }
    // 保存图片
    etlCopyFile(pImageItem->strImagePath.c_str(), cszFilePath);

    // pImageItem->eLoadStatus = ILS_LOAD_OK;

    // 此处逻辑直接压缩测试
    CompressImgParam stImgParam;
    stImgParam.strDstPath = pImageItem->strImagePath.c_str();
    stImgParam.nWidth = 480;
    stImgParam.nHeight = 272;
    stImgParam.nMaxSize = IMAGE_FILE_SIZE;

    if (!CompressImage(&stImgParam))
    {
        pImageItem->eLoadStatus = ILS_COMPRESS_FAIL;
    }

    return true;
}

// 压缩图片
bool CElementScreenSaver::CompressImage(CompressImgParam * pImgParam)
{
    if (DSK_PHONE_TYPE_COLOR != commonUnits_GetDskPhoneType())
    {

        return false;
    }

    if (pImgParam == NULL)
    {
        return false;
    }

    ImageItem * pImageItem = GetImageItem();
    if (pImageItem == NULL)
    {
        return false;
    }

    if (_ElementManager.LoadImage(GetID(), pImageItem->strImagePath.c_str(), OPT_COMPRESS, pImgParam))
    {
        pImageItem->eLoadStatus = ILS_COMPRESS;
    }
    else
    {
        pImageItem->eLoadStatus = ILS_COMPRESS_FAIL;
    }
    return true;
}

// 压缩完成
bool CElementScreenSaver::OnCompressDone(CompressImgParam * pImgParam)
{
    if (DSK_PHONE_TYPE_COLOR != commonUnits_GetDskPhoneType())
    {

        return false;
    }

    if (pImgParam == NULL)
    {
        return false;
    }

    ImageItem * pImageItem = GetImageItem();
    if (pImageItem == NULL)
    {
        return false;
    }

    // 只通知UI成功或失败，UI会保存发起时的参数
    if (pImgParam->bCompressSuc)
    {
        pImageItem->eLoadStatus = ILS_LOAD_OK;
    }
    else
    {
        pImageItem->eLoadStatus = ILS_COMPRESS_FAIL;
    }
    return true;
}

// softkey处理
bool CElementScreenSaver::ProcessSoftkey(XB_SOFTKEY_TYPE eType, int iFocusedItem, bool & bUpdate)
{
    bool bHandled = false;
    bUpdate = false;
    return bHandled;
}

#endif //#if IF_FEATURE_XMLB_SCREENSAVER

#endif // #ifdef IF_FEATURE_XMLBROWSER

