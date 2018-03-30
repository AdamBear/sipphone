#include "logic_inc.h"
#include "dsskey/src/dsskeyactionmanager.h"
#include "dsskey/src/dsskeyaction.h"

// 包含远程文件下载的头文件
#include "taskaction/modtaskaction.h"
#include "threadtask/include/networkfileaction.h"

#ifdef IF_FEATURE_XMLBROWSER

CElementImageScreen::CElementImageScreen()
{
}

CElementImageScreen::~CElementImageScreen()
{
    // 释放图片item链表
    // m_ListImage.clear();
}

// 创建之后的处理
bool CElementImageScreen::AfterCreate()
{
#if IF_BUG_32431
    m_bAutoRefresh = false;
#endif
    if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
    {
        // 下载图片
        DownloadImage();
    }


    return CElementHaveUIBase::AfterCreate();

}

// 销毁之前的处理
void CElementImageScreen::BeforeDestroy()
{
	if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
	{
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
}

// 是否播放提示音
bool CElementImageScreen::IsNeedPlayBeep()
{
    return _GetXData().stAttr.bBeep;
}

// softkey处理
bool CElementImageScreen::ProcessSoftkey(XB_SOFTKEY_TYPE eType, int iFocusedItem, bool & bUpdate)
{
    bool bHandled = false;
    bUpdate = false;
    return bHandled;
}

// 对UI事件的处理
bool CElementImageScreen::OnAction(XB_ACTION_TYPE eAction, void * pData, bool & bExitWnd,
                                   bool & bUpdate, bool bDestroyOnExit/* = false*/)
{
    bool bHandled = true;
    switch (eAction)
    {
    case XB_ACTION_DOWNLOAD_DONE:
        {
            bUpdate = OnDownloadDone((const char *)pData);
        }
        break;
    case XB_ACTION_COMPRESS:
        {
#if IF_BUG_32431
            bUpdate = CompressImage((CompressImgParam *)pData) && !m_bAutoRefresh;
#else
            bUpdate = CompressImage((CompressImgParam *)pData);
#endif
        }
        break;
    case XB_ACTION_COMPRESS_DONE:
        {
            bUpdate = OnCompressDone((CompressImgParam *)pData);
        }
        break;
    case XB_ACTION_CONFIRM:
        {
            // doneAction
            bUpdate = _GetXData().stAttr.bDestroyOnExit;
            ProcessLegalURL(_GetXData().stAttr.strDoneAction, _GetXData().stAttr.bDestroyOnExit);
        }
        break;
    case XB_ACTION_DIGITKEY:
        {
            if (pData == NULL)
            {
                XMLBROWSER_WARN("error! no digit key data.");
                return false;
            }
            int iKey = *(int *)pData;

            bUpdate = false;
            XURIList & refURIList = _GetXData().stURLList;

            List_URI_Item::iterator iter = refURIList.listItem.begin();
            List_URI_Item::iterator iterEnd = refURIList.listItem.end();
            for (; iter != iterEnd; iter++)
            {
                if (*iter == NULL)
                {
                    continue;
                }
                XURIItem * pItem = *iter;
                if (pItem->strKey[0] == (char)iKey)
                {
                    //url值为空不去下载
                    if (pItem->strURI.empty())
                    {
                        continue;
                    }
                    yl::string strURI = pItem->strURI;
                    if (strstr(pItem->strURI.c_str(), "://") == NULL)
                    {
                        // 如果未指定协议，则尝试增加Base前缀
                        strURI = refURIList.strBase + pItem->strURI;
                    }
                    //_URIProcesser.ProcessURI(strURI);
                    bUpdate = _GetXData().stAttr.bDestroyOnExit;
                    GotoURL(strURI, _GetXData().stAttr.bDestroyOnExit);
                    bHandled = true;
                    break;
                }
            }
        }
        break;
    case XB_ACTION_LINEKEY_CALLOUT:
        {
            // 上锁时不响应，没上锁进入预拨号 （彩屏话机键不会传下去，需在xmlbrowser界面做处理）
            if (_GetXData().stAttr.bLockIn)
            {
                return true;
            }
            else
            {
                XDataUI2Logic * pUIData = (XDataUI2Logic *) pData;
                CHECK_NULL(pUIData, false);
                if (DT_LINE == dsskey_GetDsskeyType(pUIData->wActionParam))
                {
                    _XMLBrowserManager.Exit();
                    return false;
                }
                //return true;
            }
        }
        break;
    case XB_ACTION_CANCEL:
        {
            // 如果cancel值不为空，执行URL，为空退出界面
            if (!_GetXData().stAttr.strCancelAction.empty())
            {
                // cancelAction:执行cancel的URL
                bUpdate = _GetXData().stAttr.bDestroyOnExit;
                ProcessLegalURL(_GetXData().stAttr.strCancelAction, _GetXData().stAttr.bDestroyOnExit);
            }
            else
            {
                bHandled = false;
            }
        }
        break;
    default:
        bHandled = false;
        break;
    }

    // 其他事件交给基类处理
    if (!bHandled)
    {
        bHandled = CElementHaveUIBase::OnAction(eAction, pData, bExitWnd, bUpdate,
                                                _GetXData().stAttr.bDestroyOnExit);
    }

    return bHandled;
}


// 获取Image
ImageItem * CElementImageScreen::GetImageItem()
{
    List_Image_Item & listImage = _GetXData().listImage;
    if (listImage.size() == 0)
    {
        return NULL;
    }
    return *(listImage.begin());
}

// 下载图片文件
bool CElementImageScreen::DownloadImage()
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
bool CElementImageScreen::OnDownloadDone(const char * cszFilePath)
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
        return false;
    }
    // 保存图片
    etlCopyFile(pImageItem->strImagePath.c_str(), cszFilePath);

    // pImageItem->eLoadStatus = ILS_LOAD_OK;

    // 此处逻辑直接压缩测试
    CompressImgParam stImgParam;
    stImgParam.strDstPath = pImageItem->strImagePath.c_str();
#ifdef _T49
    stImgParam.nWidth = 837;
    stImgParam.nHeight = 523;
#else
    stImgParam.nWidth = 480;
    stImgParam.nHeight = 272;
#endif
    stImgParam.nMaxSize = IMAGE_FILE_SIZE;

    if (!CompressImage(&stImgParam))
    {
        pImageItem->eLoadStatus = ILS_COMPRESS_FAIL;
        return true;
    }

    return false;
}

// 压缩图片
bool CElementImageScreen::CompressImage(CompressImgParam * pImgParam)
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
bool CElementImageScreen::OnCompressDone(CompressImgParam * pImgParam)
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

#if IF_BUG_32431
bool CElementImageScreen::UpdateData(XMLParseData * pParseData, bool & bUpdate)
{
    if (pParseData == NULL)
    {
        return false;
    }

    if (pParseData->eType == XMT_IMAGESCREEN
            && !pParseData->sRefreshData.strURL.empty()
            && pParseData->sRefreshData.nTimeOut > 0)
    {
        BeforeDestroy();
        InitData(pParseData);
        if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
        {
            DownloadImage();
        }

        bUpdate = false;
        m_bAutoRefresh = true;
        return true;
    }

    return false;
}
#endif

#endif // #ifdef IF_FEATURE_XMLBROWSER

