#include "cuiregionimagescreen.h"
#include "xmlbrowseruimanager.h"
#include "xmlbrowseruicommon.h"
#include "ylstring.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "messagebox/modmessagebox.h"
#include "uimanager/moduimanager.h"
#include "xmlbrowsermessagebox.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "dsklog/log.h"
#include "xwindow/xWindowManager.h"

#if XML_BROWSER_SUPPORT_UI

namespace
{
#define XML_REGION_IMAGE_XML                        "xmlbrowserui/xmlregionimage.xml"
#define XML_REGION_IMAGE_LOAD_IMAGE                 "ImgLoad"
#define XML_REGION_IAMGE_FRAME_REGULAR_HEIGHT       96
}


CUIRegionImageScreen::CUIRegionImageScreen()
    : m_eImageMode(IMAGE_MODE_FULLSCREEN)
    , m_pImage(NULL)
{
    SetElementType(XMT_IMAGESCREEN);
    loadContent(XML_REGION_IMAGE_XML);
}

CUIRegionImageScreen::~CUIRegionImageScreen()
{
    m_timerDelayMessageBox.KillTimer();
}

// 根据数据更新界面
void CUIRegionImageScreen::Update(void * pData)
{
    if (NULL == pData || NULL == m_pImage)
    {
        return;
    }

    //
    XBImageScreenData * pNewData = static_cast<XBImageScreenData *>(pData);

    // 设置键盘是否锁定
    m_bLock = pNewData->stAttr.bLockIn;

    // 设置图片的显示模式
    SetImageMode(pNewData->stAttr.eMode);

    if (pNewData->listImage.size() > 0)
    {
        ////---- 目前只画一张图片
        DrawImage(m_eImageMode, pNewData->listImage.front());
    }

    m_iQuitTime = pNewData->stAttr.iTimeOut;

    SetSoftkey(pNewData->vecSoftkey);
}

void CUIRegionImageScreen::SetImageMode(IMAGE_MODE eMode)
{
    m_eImageMode = eMode;
}

void CUIRegionImageScreen::DrawImage(IMAGE_MODE eMode, ImageItem * pImageItem)
{
    XMLBROWSERUI_INFO("CUIRegionImageScreen::DrawImage");
    if (NULL == pImageItem || NULL == m_pImage)
    {
        return;
    }

    XMLBROWSERUI_INFO("Image load status [%d]", pImageItem->eLoadStatus);
    // 如果还在下载中,则不显示图片
    if (ILS_LOAD_OK != pImageItem->eLoadStatus)
    {
        if (ILS_DOWNLOAD != pImageItem->eLoadStatus
                || ILS_COMPRESS != pImageItem->eLoadStatus)
        {
            m_strNoteContent = _LANG(TRID_IMAGE_LOADING_PLEASE_WAIT);
            m_timerDelayMessageBox.SingleShot(20, TIMER_METHOD(this, CUIRegionImageScreen::OnDelayMessageBox));
            return;
        }
        // 其它是失败情况，则不显示图片即可。容错
        XMLBROWSERUI_INFO("Image load fail.");
        return;
    }

    MessageBox_RemoveAllMessageBox(MessageBox_Destory);

    // 读取图片
    xPixmap image;

    int nWidth = width();
    int nHeight = height();

    int sizeImage;
    BOOL bRet = LoadImage(image, pImageItem->strImageURL.c_str(), pImageItem->uWidth,
                          pImageItem->uHeight, sizeImage);

    if (image.isNull() || !bRet)
    {
        m_strNoteContent = _LANG(TRID_IMAGE_LOAD_ERROR);
        m_timerDelayMessageBox.SingleShot(20, TIMER_METHOD(this, CUIRegionImageScreen::OnDelayMessageBox));
        return;
    }

    //根据不同的显示模式设置对应的显示位置
    switch (eMode)
    {
    case IMAGE_MODE_REGULAR:    // 正常显示模式
    case IMAGE_MODE_FULLSCREEN: // 全屏显示模式
    {
        // 计算lab的位置
        m_pImage->setPixmap(image);
        if (IMAGE_MODE_REGULAR == eMode)
        {
            layout_height = XML_REGION_IAMGE_FRAME_REGULAR_HEIGHT;
        }

        int nAlign = GetAlignByImageAlign(pImageItem->eVertical, pImageItem->eHorizontal);
        FrameLayoutParams * pFrameLayoutParm = FrameLayoutParams::GetLayoutParams(
                m_pImage->m_pLayoutInParent);
        if (NULL != pFrameLayoutParm)
        {
            pFrameLayoutParm->layout_gravity = nAlign;
        }
        ownerStation().asyncRelayoutViewBound(this);
    }
    break;
    default:
        break;
    }
}

int CUIRegionImageScreen::GetAlignByImageAlign(IMAGE_VERTICAL_ALIGN eVertical,
        IMAGE_HORIZONTAL_ALIGN eHorizontal)
{
    int nAlign = 0;
    // botton
    if (IMAGE_VERTICAL_ALIGN_BOTTOM == eVertical)
    {
        nAlign = nAlign | align_bottom;
    }
    // middle
    else if (IMAGE_VERTICAL_ALIGN_CENTER == eVertical)
    {
        nAlign = nAlign | align_v_center;
    }
    else
    {
        nAlign = nAlign | align_top;
    }

    // right
    if (IMAGE_HORIZONTAL_ALIGN_RIGHT == eHorizontal)
    {
        nAlign = nAlign | align_right;
    }
    // center
    else if (IMAGE_HORIZONTAL_ALIGN_CENTER == eHorizontal)
    {
        nAlign = nAlign | align_h_center;
    }
    else
    {
        nAlign = nAlign | align_left;
    }

    return nAlign;
}

void CUIRegionImageScreen::OnDelayMessageBox(xThreadTimer* timer, LPVOID pExtraData)
{
    CBaseDialog * pXBDialog = UIManager_GetPrivateDlg("CDlgXMLBrowser");
    //在talkui界面不弹窗,xmlbrowser 已经存在 但是不是顶层窗口时不弹窗
    bool bBlock = UIManager_IsInTalk() ||
                  (NULL != pXBDialog && pXBDialog != UIManager_GetTopWindow());

    if (bBlock)
    {
        return;
    }

    MSGBOX_INFO stMsgInfo;
    stMsgInfo.strTitle = _LANG(TRID_WARNING);
    stMsgInfo.strNote = _LANG(m_strNoteContent);
    stMsgInfo.pAction = &_XBMessageManager;
    stMsgInfo.eType = MESSAGEBOX_Cancel;
    stMsgInfo.nMsTime = 3000;
    MessageBox_ShowCommonMsgBox(stMsgInfo);
}

// 事件过滤器
bool CUIRegionImageScreen::OnKeyPress(int nKey)
{
    // 按键交给基类统一处理
    if (CUIRegionBase::OnKeyPress(nKey))
    {
        return true;
    }

    switch (nKey)
    {
    case PHONE_KEY_1:
    case PHONE_KEY_2:
    case PHONE_KEY_3:
    case PHONE_KEY_4:
    case PHONE_KEY_5:
    case PHONE_KEY_6:
    case PHONE_KEY_7:
    case PHONE_KEY_8:
    case PHONE_KEY_9:
    case PHONE_KEY_0:
    {
        // 0
        int iDgtKey = '0';
        iDgtKey += (nKey - PHONE_KEY_0);
        xmlbrowser_OnAction(GetElementID(), XB_ACTION_DIGITKEY, &iDgtKey);
        return true;
    }
    break;
    case PHONE_KEY_POUND:
    {
        int iDgtKey = '#';
        xmlbrowser_OnAction(GetElementID(), XB_ACTION_DIGITKEY, &iDgtKey);
        return true;
    }
    break;
    case PHONE_KEY_STAR:
    {
        int iDgtKey = '*';
        xmlbrowser_OnAction(GetElementID(), XB_ACTION_DIGITKEY, &iDgtKey);
        return true;
    }
    break;
    default:
        break;
    }

    return false;
}

BOOL CUIRegionImageScreen::LoadImage(xPixmap & pmImage, const char * lpszImagePath, UINT uWidth,
                                     UINT uHeight, int & sizeImage)
{
    int iPixelNum = uWidth * uHeight;

    chTempByteArray TBAImage = etlText2Bin((LPCSTR)lpszImagePath);

    int nSizeImage = TBAImage.GetSize();

    // 如果没图片，直接返回
    if (0 == nSizeImage || 0 == uHeight)
    {
        return FALSE;
    }

    // 如果是奇数，则需要+1，比如像素为59*81
    if (iPixelNum % 2 != 0)
    {
        iPixelNum = iPixelNum + 1;
    }

    if (0 >= iPixelNum)
    {
        return FALSE;
    }

    sizeImage = iPixelNum;

    int iTemp = iPixelNum / nSizeImage;
    switch (iTemp)
    {
#if IF_USE_GRADATION
    case 2: //.dob图片格式
    {
        if (iPixelNum == 2 * nSizeImage)
        {
            pmImage.LoadFromDobData(TBAImage.GetBuffer(), uWidth, uHeight);
        }
        else
        {
            return FALSE;
        }
    }
    break;
#endif
    case 8: //Aastra图片格式
    {
        int tmp = 0;
        iTemp = iPixelNum >> 3;

        for (int i = 0; i < iTemp && i < nSizeImage; ++i)
        {
            for (int j = 0; j < 8; ++j)
            {
                if (TBAImage[i] & (0x01 << j))
                {
                    tmp = (i << 3) + 7 - j;
                    tmp = uWidth * (tmp % uHeight) + tmp / uHeight;
                    //m_pBYTEImage[tmp] = 0x0f;
                }
            }
        }
    }
    break;
    default:
    {
        XMLBROWSERUI_WARN("Image format error!");
    }
    break;
    }
    return TRUE;
}

void CUIRegionImageScreen::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);
    m_pImage = static_cast<xImageView *>(getViewById(XML_REGION_IMAGE_LOAD_IMAGE));
}

void CUIRegionImageScreen::SetDefaultSoftkey(VecSoftkeyParam & softkeyParam)
{
    softkeyParam.clear();

    softkeyParam.push_back(TypeToSoftInfo(XB_ST_EXIT));
    softkeyParam.push_back(TypeToSoftInfo(XB_ST_EMPTY));
    softkeyParam.push_back(TypeToSoftInfo(XB_ST_EMPTY));
    softkeyParam.push_back(TypeToSoftInfo(XB_ST_EMPTY));
}

#endif
