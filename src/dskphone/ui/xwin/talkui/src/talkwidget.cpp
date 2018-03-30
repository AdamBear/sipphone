#include "talkwidget.h"
#include "talkuipath.h"
#include "talkuixmlpath.h"
#include "talkuixmlitemname.h"

#include "talkuicommonfunction.h"
#include "talkuilogic/talkuilogicdelegate.h"
#include "talkuiframemanager.h"

#include "conflictmanager.h"

namespace talkui
{

CTalkWidget::CTalkWidget()
    : m_bShowContactImage(false)
#if IF_TALKUI_CONTACT_PHOTO
      //头像
    , m_pContactImageFrame(NULL)
    , m_pContactImage(NULL)
    , m_pContactCover(NULL)
      //显示在头像上的状态
    , m_pStatusFrame(NULL)
    , m_pStatusIcon(NULL)
    , m_pStatusHint(NULL)
#endif
#if IF_TALKUI_CONTACT_PHOTO_ANIMATION
    , m_pContactMask(NULL)
#endif
      //普通提示
    , m_pHint1(NULL)
    , m_pHint2(NULL)
      //时间
    , m_pTalkTime(NULL)
      //无头像的状态
    , m_pNoContactStatusFrame(NULL)
    , m_pNoContactStatusIcon(NULL)
    , m_pNoContactStatusHint(NULL)
      //来电信息
    , m_pIncomingFrame(NULL)
    , m_pIncomingAnimation(NULL)
    , m_pIncomingHint(NULL)
    , m_pIncomingArrow(NULL)
    , m_pIncomingTarget(NULL)
#if IF_HD_ICON_DEPART
    , m_pHDIcon(NULL)
#endif
{

}

CTalkWidget::~CTalkWidget()
{

}

bool CTalkWidget::GetIsShowContact()
{
    return m_bShowContactImage;
}

#if IF_TALKUI_CONTACT_PHOTO
void CTalkWidget::SetContactWidget(xFrameLayout* pContactImageFrame
                                   , xImageView* pContactImage)
{
    m_pContactImageFrame = pContactImageFrame;
    m_pContactImage = pContactImage;
}

void CTalkWidget::SetContactStatusWidget(xImageView* pContactCover
        , xView* pStatusFrame
        , xImageView* pStatusIcon
        , xTextView* pStatusHint)
{
    m_pContactCover = pContactCover;
    m_pStatusFrame = pStatusFrame;
    m_pStatusIcon = pStatusIcon;
    m_pStatusHint = pStatusHint;
}
#endif

#if IF_TALKUI_CONTACT_PHOTO_ANIMATION
void CTalkWidget::SetContactMask(xImageView* pContactMask)
{
    m_pContactMask = pContactMask;
}
#endif

void CTalkWidget::SetNormalHintWidget(xTextView* pHint1, xTextView* pHint2)
{
    m_pHint1 = pHint1;
    m_pHint2 = pHint2;
}

void CTalkWidget::EnableHintScroll(bool bEnable)
{
    if (NULL != m_pHint1)
    {
        m_pHint1->EnableScroll(bEnable);
    }

    if (NULL != m_pHint2)
    {
        m_pHint2->EnableScroll(bEnable);
    }
}

void CTalkWidget::SetTimeWidget(xTimerPieceView* pTalkTime)
{
    m_pTalkTime = pTalkTime;
}

void CTalkWidget::SetStatusWidget(xView* pNoContactStatusFrame
                                  , xImageView* pNoContactStatusIcon
                                  , xTextView* pNoContactStatusHint)
{
    m_pNoContactStatusFrame = pNoContactStatusFrame;
    m_pNoContactStatusIcon = pNoContactStatusIcon;
    m_pNoContactStatusHint = pNoContactStatusHint;
}

void CTalkWidget::SetIncomingWidget(xView* pIncomingFrame
                                    , xAnimationBox* pIncomingAnimation
                                    , xTextView* pIncomingHint
                                    , xImageView* pIncomingArrow
                                    , xTextView* pIncomingTarget)
{
    m_pIncomingFrame = pIncomingFrame;
    m_pIncomingAnimation = pIncomingAnimation;
    m_pIncomingHint = pIncomingHint;
    m_pIncomingArrow = pIncomingArrow;
    m_pIncomingTarget = pIncomingTarget;
}

#if IF_HD_ICON_DEPART
void CTalkWidget::SetHDIcon(xImageView* pHDIcon)
{
    m_pHDIcon = pHDIcon;
}
#endif

void CTalkWidget::ResetWidget()
{
    m_bShowContactImage = false;

    if (NULL != m_pHint1)
    {
        m_pHint1->hide(true);
        m_pHint1->SetText("");
    }
    if (NULL != m_pHint2)
    {
        m_pHint2->hide(true);
        m_pHint2->SetText("");
    }

    EnableHintScroll(true);

#if IF_TALKUI_CONTACT_PHOTO
    if (NULL != m_pContactImageFrame)
    {
        m_pContactImageFrame->hide(true);
    }
    if (NULL != m_pContactImage)
    {
        m_pContactImage->hide(true);
        m_pContactImage->setPixmap("");
    }
    if (NULL != m_pContactCover)
    {
        m_pContactCover->hide(true);
    }
    if (NULL != m_pStatusFrame)
    {
        m_pStatusFrame->hide(true);
    }
    if (NULL != m_pStatusIcon)
    {
        m_pStatusIcon->hide(true);
        m_pStatusIcon->setPixmap("");
    }
    if (NULL != m_pStatusHint)
    {
        m_pStatusHint->hide(true);
        m_pStatusHint->SetText("");
    }
#endif

#if IF_TALKUI_CONTACT_PHOTO_ANIMATION
    if (NULL != m_pContactMask)
    {
        m_pContactMask->hide(true);
    }
#endif

    if (NULL != m_pTalkTime)
    {
        m_pTalkTime->hide(true);
        m_pTalkTime->StopTimer();
    }

    if (NULL != m_pNoContactStatusFrame)
    {
        m_pNoContactStatusFrame->hide(true);
    }
    if (NULL != m_pNoContactStatusIcon)
    {
        m_pNoContactStatusIcon->hide(true);
        m_pNoContactStatusIcon->setPixmap("");
    }
    if (NULL != m_pNoContactStatusHint)
    {
        m_pNoContactStatusHint->hide(true);
        m_pNoContactStatusHint->SetText("");
    }

    if (NULL != m_pIncomingFrame)
    {
        m_pIncomingFrame->hide(true);
    }
    if (NULL != m_pIncomingAnimation)
    {
        m_pIncomingAnimation->hide(true);
    }
    if (NULL != m_pIncomingHint)
    {
        m_pIncomingHint->hide(true);
        m_pIncomingHint->SetText("");
    }
    if (NULL != m_pIncomingArrow)
    {
        m_pIncomingArrow->hide(true);
    }
    if (NULL != m_pIncomingTarget)
    {
        m_pIncomingTarget->hide(true);
        m_pIncomingTarget->SetText("");
    }

#if IF_HD_ICON_DEPART
    if (NULL != m_pHDIcon)
    {
        m_pHDIcon->hide(true);
    }
#endif
}

bool CTalkWidget::bHaveTwoHint()
{
    return NULL != m_pHint2;
}

void CTalkWidget::showStatusHint(bool bShow)
{
#if IF_TALKUI_CONTACT_PHOTO
    if (NULL != m_pStatusFrame)
    {
        if (bShow && m_bShowContactImage)
        {
            m_pStatusFrame->show();
        }
        else
        {
            m_pStatusFrame->hide(true);
        }
    }
#endif

    if (NULL != m_pNoContactStatusFrame)
    {
        if (bShow && !m_bShowContactImage)
        {
            m_pNoContactStatusFrame->show();
        }
        else
        {
            m_pNoContactStatusFrame->hide(true);
        }
    }
}

void CTalkWidget::showTimer(bool bShow)
{
    if (NULL != m_pTalkTime)
    {
        if (bShow)
        {
            m_pTalkTime->show();
        }
        else
        {
            m_pTalkTime->hide(true);
            m_pTalkTime->StopTimer();
        }
    }
}

void CTalkWidget::showIncomingHint(bool bShow)
{
    if (NULL != m_pIncomingFrame)
    {
        if (bShow)
        {
            m_pIncomingFrame->show();
        }
        else
        {
            m_pIncomingFrame->hide(true);
        }
    }
}

void CTalkWidget::SetBaseContactInfo(yl::string strContactName, yl::string strContactNumber)
{
    if (NULL == m_pHint1)
    {
        return;
    }

    m_pHint1->show();

    //如果Hint2控件存在，则依次赋值
    if (NULL != m_pHint2)
    {
        m_pHint2->show();

        m_pHint1->SetText(strContactName);
        m_pHint2->SetText(strContactNumber);
    }
    //如果Hint2控件不存在，则整合到Hint1控件中显示
    else
    {
        if (strContactName.empty())
        {
            m_pHint1->SetText(strContactNumber);
        }
        else
        {
            m_pHint1->SetText(strContactName);
        }

//         if (strContactNumber.empty())
//         {
//             m_pHint1->SetText(strContactName);
//         }
//         else
//         {
//             m_pHint1->SetText(strContactName + " " + strContactNumber);
//         }
    }
}

void CTalkWidget::SetNameFontSize(int nSize, int nMarginBottom)
{
    if (NULL != m_pHint1)
    {
        m_pHint1->layout_marginBottom = nMarginBottom;
        m_pHint1->SetFontPointSize(nSize);
    }
}

void CTalkWidget::SetStatusInfo(yl::string strIcon, yl::string strHint)
{
#if IF_TALKUI_CONTACT_PHOTO
    if (m_bShowContactImage)
    {
        if (NULL != m_pContactCover)
        {
            m_pContactCover->show();
        }
        if (NULL != m_pStatusFrame)
        {
            m_pStatusFrame->show();
        }
        if (NULL != m_pStatusIcon)
        {
            m_pStatusIcon->show();
            m_pStatusIcon->setPixmap(strIcon);
        }

        if (NULL != m_pStatusHint)
        {
            m_pStatusHint->show();
            m_pStatusHint->SetText(strHint);
        }
    }
    else
    {
#endif
        if (NULL != m_pNoContactStatusFrame)
        {
            m_pNoContactStatusFrame->show();
        }
        if (NULL != m_pNoContactStatusIcon
                && !strIcon.empty())
        {
            m_pNoContactStatusIcon->show();
            m_pNoContactStatusIcon->setPixmap(strIcon);
        }

        if (NULL != m_pNoContactStatusHint)
        {
            m_pNoContactStatusHint->show();
            m_pNoContactStatusHint->SetText(strHint);
        }
#if IF_TALKUI_CONTACT_PHOTO
    }
#endif
}

void CTalkWidget::SetIncomingInfo(yl::string strHint, yl::string strTarget /*= ""*/)
{
    yl::string strIncommingCallNumber = strHint;
    if (configParser_GetConfigInt(kszHideCallin))
    {
        strIncommingCallNumber = _UILOGIC_LANG_TRAN(TRID_INCOMING_CALL);
    }
    else
    {
#if !IF_TALKUI_INCOMING_CALL_COMBINE
        if (_TalkUILogicDelegate.IsShowTargetInfo()
                && !strTarget.empty())
        {
            strIncommingCallNumber = strIncommingCallNumber;
        }
        else
#endif
        {
            yl::string strIncommingCallTip = _UILOGIC_LANG_TRAN(TRID_INCOMING_CALL) + yl::string(": ");
            strIncommingCallNumber = strIncommingCallTip + strIncommingCallNumber;
        }
    }

    if (NULL != m_pIncomingFrame)
    {
        m_pIncomingFrame->show();
    }
    if (NULL != m_pIncomingAnimation)
    {
        m_pIncomingAnimation->show();
    }
    if (NULL != m_pIncomingHint)
    {
        m_pIncomingHint->show();
        m_pIncomingHint->SetText(strIncommingCallNumber);
    }

#if !IF_TALKUI_INCOMING_CALL_COMBINE
    if (_TalkUILogicDelegate.IsShowTargetInfo()
            && !strTarget.empty())
    {
        if (NULL != m_pIncomingArrow)
        {
            m_pIncomingArrow->show();
        }
        if (NULL != m_pIncomingTarget)
        {
            m_pIncomingTarget->show();
            m_pIncomingTarget->SetText(strTarget);
        }
    }
#endif
}

void CTalkWidget::UpdateTalkUITime(int nSecond)
{
    if (NULL != m_pTalkTime)
    {
        m_pTalkTime->show();
        m_pTalkTime->SetStartDuration(nSecond);
        m_pTalkTime->PauseTimer();
        m_pTalkTime->ResumeTimer();
    }
}

#if IF_TALKUI_CONTACT_PHOTO
void CTalkWidget::SetPhotoInfo(yl::string strImagePath)
{
    int nDisplayMode = _TalkUILogicDelegate.IsDisplayContactPhoto();
    xPixmap pic;
    bool bIsCustomImage = JudgeCustomImage(strImagePath, pic);

    if (0 == nDisplayMode)
    {
        m_bShowContactImage = false;
    }
    else if (1 == nDisplayMode)
    {
        m_bShowContactImage = true;
        if (pic.isNull())
        {
            pic = GetPixmapByString(PIC_TALKUI_DEFAULT_CONTACT_IMAGE);
        }
    }
    else// if (2 == nDisplayMode)
    {
        m_bShowContactImage = !bIsCustomImage;
    }

    if (m_bShowContactImage)
    {
        if (NULL != m_pContactImageFrame)
        {
            m_pContactImageFrame->show();
        }

        if (NULL != m_pContactImage)
        {
            m_pContactImage->show();
            m_pContactImage->setPixmap(pic);
        }

#if IF_TALKUI_CONTACT_PHOTO_ANIMATION
        if (NULL != m_pContactMask)
        {
            m_pContactMask->show();
        }
#endif
    }
}

void CTalkWidget::SetConfPhoto()
{
    int nDisplayMode = _TalkUILogicDelegate.IsDisplayContactPhoto();

    if (0 == nDisplayMode
            || 2 == nDisplayMode)
    {
        m_bShowContactImage = false;
    }
    else if (1 == nDisplayMode)
    {
        xPixmap pic = GetPixmapByString(_TalkUIFrameManager.GetPicContactsConference());
        m_bShowContactImage = !pic.isNull();

        if (m_bShowContactImage)
        {
            if (NULL != m_pContactImageFrame)
            {
                m_pContactImageFrame->show();
            }

            if (NULL != m_pContactImage)
            {
                m_pContactImage->show();
                m_pContactImage->setPixmap(pic);
            }
        }
    }
}

bool CTalkWidget::JudgeCustomImage(const yl::string& strImage, xPixmap& pic)
{
    if (strImage.empty())
    {
        return false;
    }

    bool bResult = false;

    //如果从路径上判断已经是默认头像，则直接赋值结果，不加载图片
    if (strImage.find(PIC_CONTACTS_IMAGE_BLUETOOTH_DEFAULT, 0) != yl::string::npos
//             PIC_CONTACTS_DEFAULT_IMAGE == strImage ||
//             PIC_CONTACTS_CONFERENCE == strImage)
       )
    {
        bResult = false;
    }
    else
    {
        yl::string strPhoto = strImage;

        // 隐藏路径的格式则转换为全路径
        commonUnits_Real2Hide(strPhoto, PHFILE_TYPE_CONTACT_IMAGE);

        if (strPhoto.find("Resource:", 0) != yl::string::npos
                || strPhoto.find("Default:", 0) != yl::string::npos)
        {
            bResult = false;
        }
        else
        {
            //如果路径上判断不出，则根据图像是否可用
            {
                if (pathFileExist(strImage.c_str()))
                {
                    pic.load(strImage);

                    if (!pic.isNull()
                            && (TALK_CONTACT_IMAGE_WIDTH != pic.width()
                                || TALK_CONTACT_IMAGE_HEIGHT != pic.height()))
                    {
                        xPixmap pmResult;
                        pmResult.Create(TALK_CONTACT_IMAGE_WIDTH, TALK_CONTACT_IMAGE_HEIGHT);
                        pmResult.ScaleCopy(pic);
                        pic = pmResult;
                    }
                }

                bResult = !pic.isNull();
            }
        }
    }

    return bResult;
}

#endif

#if IF_HD_ICON_DEPART
void CTalkWidget::SetHDIconInfo(AUDIO_CODEC_TYPE eAudioCodec)
{
    bool bShow = true;
    yl::string strIcon = "";

    CONFLICT_PRIORITY ePrority = _TalkConflictManager.getCurrentShowUnit();
    if (CONFLICT_PRIORITY_VOLUMEBAR == ePrority
            || CONFLICT_PRIORITY_DTMF == ePrority
            || CONFLICT_PRIORITY_INCOMING_INFO == ePrority)
    {
        bShow = false;
    }

    if (eAudioCodec & ACT_HD)
    {
        strIcon = PIC_TALK_ICON_HD_DEPART;
    }
    else if (eAudioCodec & ACT_AMR)
    {
        strIcon = PIC_TALK_ICON_AMR;
    }
    else if (eAudioCodec & ACT_OPUS)
    {
        strIcon = PIC_TALK_ICON_OPUS;
    }
    else
    {
        strIcon = "";
    }

    if (NULL != m_pHDIcon)
    {
        if (bShow)
        {
            m_pHDIcon->show();
            m_pHDIcon->setPixmap(strIcon);
        }
        else
        {
            m_pHDIcon->hide(true);
        }
    }
}
#endif

}       //namespace talkui


