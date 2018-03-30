#include "cdsskeybtnicon.h"
#include "baseui/include/t2xpicpath.h"
#include "edk/include/modedk.h"
#include "imagecache/include/modimagecache.h"
namespace
{
const char * DSSKEYBTN_ICON = "icon";
const char * DSSKEYBTN_LINE = "line";
const char * DSSKEYBTN_ICON_STATUS = "status";
const char * DSSKEYBTN_FRAME_ICON = "frmIcon";
}

yl::string GetDsskeyCutLinePic(DSS_KEY_GROUNDPHOTO_MODE ePicMode)
{
    switch (ePicMode)
    {
    case GROUNDPHOTO_MODE_NORMAL:
        {
            return PIC_THEME1_LINEKEY_CUTLINE;
        }
        break;
    case GROUNDPHOTO_MODE_HIGHLIGHT:
        {
            return PIC_THEME1_HIGHLIGHT_LINEKEY_CUTLINE;
        }
        break;
    case GROUNDPHOTO_MODE_DISALBE:
        {
            return PIC_THEME1_DISABLED_LINEKEY_CUTLINE;
        }
        break;
    case GROUNDPHOTO_MODE_USING:
        {
            return PIC_THEME1_DIAL_LINEKEY_CUTLINE;
        }
        break;
    default:
        {
            return PIC_THEME1_LINEKEY_CUTLINE;
        }
        break;
    }

    return PIC_THEME1_LINEKEY_CUTLINE;
}

CDsskeyBtnIcon::CDsskeyBtnIcon()
    : m_pIcon(NULL)
    , m_pIconStatus(NULL)
    , m_pLine(NULL)
    , m_pFrmIcon(NULL)
    , m_strCurIcon("")
    , m_strDownCurIcon("")
    , m_strPresenceIcon("")
{
    m_eShowType = (DSSKEY_SHOW_SIGICON_LEFT);
}


CDsskeyBtnIcon::~CDsskeyBtnIcon()
{
}

void CDsskeyBtnIcon::loadChildrenElements(xml_node & node)
{
    CDsskeyButton::loadChildrenElements(node);
    m_pIcon = static_cast<xImageView *>(getViewById(DSSKEYBTN_ICON));
    m_pLine = static_cast<xImageView *>(getViewById(DSSKEYBTN_LINE));
    m_pIconStatus = static_cast<xImageView *>(getViewById(DSSKEYBTN_ICON_STATUS));
    m_pFrmIcon = static_cast<xFrameLayout *>(getViewById(DSSKEYBTN_FRAME_ICON));
}

void CDsskeyBtnIcon::SetLinePixmap(const yl::string & strLinePix)
{
    if (m_pLine != NULL)
    {
        m_pLine->setPixmap(strLinePix);
    }
}

void CDsskeyBtnIcon::SetIcon(const yl::string & strIcon)
{
    if (m_pIcon != NULL)
    {
#if IS_COLOR
        xPixmap pixStatus;
        Image_LoadImage(strIcon, IMG_CLASSIFY_CUSTOM, pixStatus, true);
        if (pixStatus.isNull())
        {
            m_pIcon->setPixmap(strIcon);
        }
        else
        {
            m_pIcon->setPixmap(pixStatus);
        }
#else
        m_pIcon->setPixmap(strIcon);
#endif
        if (m_pIcon->src.isNull())
        {
            DSSKEYUI_WARN(" ====== pixmap  %s isNull====== ", strIcon.c_str());
            m_pIcon->hide(true);
        }
        else
        {
            m_pIcon->show();
        }
    }
}

void CDsskeyBtnIcon::SetStatusIcon(const yl::string & strStatusIcon)
{
    if (m_pIconStatus != NULL)
    {
        m_pIconStatus->setPixmap(strStatusIcon);
        if (m_pIconStatus->src.isNull())
        {
            m_pIconStatus->hide(true);
        }
        else
        {
            m_pIconStatus->show();
        }
    }
}

// 设置DsskeyIcon和文字为空 并且隐藏
void CDsskeyBtnIcon::SetIconAndTextEmptyHide()
{
    hide(false);
    SetIcon("");
    SetText("");
}

// 设置DsskeyIcon和文字为空 但是显示
void CDsskeyBtnIcon::SetIconAndTextEmptyShow()
{
    SetIcon("");
    SetText("");
    m_eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;

    show();
}

void  CDsskeyBtnIcon::UpdateDisplayStatus(const DsskeyDisplayData& sData)
{
    bool bLinekeyBGWhileNoContent = m_bShowWithoutContent;

    m_nDsskeyID = sData.iDsskeyID;
    if (IsShowNoLabelDsskey())
    {
        //改变状态到no label只需要改变m_eDisplayMode即可，不需要改变成员属性
    }
    else if (sData.iDsskeyID > -1
             && (bLinekeyBGWhileNoContent ? sData.eDssKeytype >= DT_NA : sData.eDssKeytype > DT_NA)
             && sData.eDssKeytype < DT_LAST)
    {
        if (sData.eDssKeytype == DT_NA && bLinekeyBGWhileNoContent)
        {
            SetIconAndTextEmptyShow();
        }
        else if (DT_LINE == sData.eDssKeytype
                 && (sData.iStatus <= DS_LINE_NONE
                     || sData.iStatus >= DS_LINE_END || DS_LINE_DISABLE == sData.iStatus))
        {
            if (bLinekeyBGWhileNoContent)
            {
                SetIconAndTextEmptyShow();
            }
            else
            {
                SetIconAndTextEmptyHide();
            }
        }
        //http://10.2.1.199/Bug.php?BugID=86699 添加EDK开关关闭类型为cutomkey的特殊情况判断
        else if (DT_CUSTOM_KEY == sData.eDssKeytype
                 && (!Edk_IsEnable()))
        {
            if (bLinekeyBGWhileNoContent)
            {
                SetIconAndTextEmptyShow();
            }
            else
            {
                SetIconAndTextEmptyHide();
            }
        }
        else
        {
            //更新背景

            show();

            m_strCurIcon = sData.strIconPath;
            SetIcon(m_strCurIcon);

            m_strDownCurIcon = sData.strDownIconPath;

            m_strPresenceIcon = sData.strPresenceIcon;
            SetStatusIcon(m_strPresenceIcon);

            m_eDssKeyType = sData.eDssKeytype;

            SetShortenText(sData.strShortenText);
            SetText(sData.strFirstRowText);

            m_etextMode = sData.eTextMode;
            m_eGroundPhoto = sData.eGroundPhoto;
            DSSKEYUI_INFO("m_eGroundPhoto is %d, m_etextMode is %d.", m_eGroundPhoto, m_etextMode);

            //m_strShortenText = toQString(sData.strShortenText);
            //m_LastLang = g_pLanguageController->GetCurrentLanguage();
            //CreateTextPiccache(m_pmTextPiccache);
        }
    }
    else
    {
        if (!bLinekeyBGWhileNoContent)
        {
            hide(false);
        }
        SetIcon("");
        SetText("");
    }
#if IF_BUG_24956
    //话机自动更新的dsskey状态，一定不会出现背景图片闪烁
    m_bFlushGround = false;
#endif

    ChangeDisplay();
}

void CDsskeyBtnIcon::ShowHideIcon()
{
    NULLReturnVoid(m_pIcon);
    NULLReturnVoid(m_pIconStatus);
    NULLReturnVoid(m_pFrmIcon);

    if (devicelib_GetPhoneType() == PT_T52)
    {
        if (IsShowTwoCharsDsskey()
                || IsShowNoLabelDsskey())
        {
            m_pFrmIcon->hide(true);
            m_pIconStatus->hide(true);
        }
        else
        {
            m_pFrmIcon->show();

            if (m_pIconStatus->src.isNull())
            {
                m_pIconStatus->hide(true);
            }
            else
            {
                m_pIconStatus->show();
            }
        }
    }

}

void CDsskeyBtnIcon::ChangeDisplay()
{
    yl::string strPixLine = GetDsskeyCutLinePic(m_eGroundPhoto);
    SetLinePixmap(strPixLine);
    ShowHideIcon();
    CDsskeyButton::ChangeDisplay();
}
