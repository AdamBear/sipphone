#ifndef _DLG_TALK_WIDGET
#define _DLG_TALK_WIDGET

#include "talkuilogic/talkuilogiccondition.h"

#include "baseui/include/timerpieceview.h"
#include "xwindow/xControls.h"

#include "talk/talklogic/include/callinfo.h"

namespace talkui
{

class CTalkWidget
{
public:
    explicit CTalkWidget();
    virtual ~CTalkWidget();

    void ResetWidget();
    bool GetIsShowContact();

public:
#if IF_TALKUI_CONTACT_PHOTO
    void SetContactWidget(xFrameLayout* pContactImageFrame, xImageView* pContactImage);
    void SetContactStatusWidget(xImageView* pContactCover, xView* pStatusFrame
                                , xImageView* pStatusIcon, xTextView* pStatusHint);
#endif

#if IF_TALKUI_CONTACT_PHOTO_ANIMATION
    void SetContactMask(xImageView* pContactMask);
#endif

    void SetNormalHintWidget(xTextView* pHint1, xTextView* pHint2);
    void EnableHintScroll(bool bEnable);

    void SetTimeWidget(xTimerPieceView* pTalkTime);
    void SetStatusWidget(xView* pNoContactStatusFrame
                         , xImageView* pNoContactStatusIcon, xTextView* pNoContactStatusHint);

    void SetIncomingWidget(xView*  pIncomingFrame
                           , xAnimationBox* pIncomingAnimation, xTextView* pIncomingHint
                           , xImageView* pIncomingArrow, xTextView* pIncomingTarget);

#if IF_HD_ICON_DEPART
    void SetHDIcon(xImageView* pHDIcon);
#endif

public:
    //设置各类提示信息
    //设置基本联系人信息，仅包含名字和号码
    void SetBaseContactInfo(yl::string strContactName, yl::string strContactNumber);

    void SetNameFontSize(int nSize, int nMarginBottom);

#if IF_TALKUI_CONTACT_PHOTO
    virtual void SetPhotoInfo(yl::string strImagePath);
    virtual void SetConfPhoto();
#endif

    void UpdateTalkUITime(int nSecond);

    void SetStatusInfo(yl::string strIcon, yl::string strHint);
    void SetIncomingInfo(yl::string strHint, yl::string strTarget = "");

#if IF_HD_ICON_DEPART
    void SetHDIconInfo(AUDIO_CODEC_TYPE eAudioCodec);
#endif

    bool bHaveTwoHint();

public:
    void showStatusHint(bool bShow);
    void showTimer(bool bShow);
    void showIncomingHint(bool bShow);

protected:
#if IF_TALKUI_CONTACT_PHOTO
    bool JudgeCustomImage(const yl::string& strImage, xPixmap& pic);
#endif

    //界面控件
private:
    bool m_bShowContactImage;

#if IF_TALKUI_CONTACT_PHOTO
    //头像
    xFrameLayout* m_pContactImageFrame;
    xImageView* m_pContactImage;
    xImageView* m_pContactCover;
    //显示在头像上的状态
    xView* m_pStatusFrame;
    xImageView* m_pStatusIcon;
    xTextView* m_pStatusHint;
#endif

#if IF_TALKUI_CONTACT_PHOTO_ANIMATION
    xImageView* m_pContactMask;
#endif

    //普通提示
    xTextView* m_pHint1;
    xTextView* m_pHint2;
    //时间
    xTimerPieceView* m_pTalkTime;
    //无头像的状态
    xView* m_pNoContactStatusFrame;
    xImageView* m_pNoContactStatusIcon;
    xTextView* m_pNoContactStatusHint;
    //来电信息
    xView*  m_pIncomingFrame;
    xAnimationBox* m_pIncomingAnimation;
    xTextView* m_pIncomingHint;
    xImageView* m_pIncomingArrow;
    xTextView* m_pIncomingTarget;

#if IF_HD_ICON_DEPART
    xImageView* m_pHDIcon;
#endif

};

}       //namespace talkui

#endif
