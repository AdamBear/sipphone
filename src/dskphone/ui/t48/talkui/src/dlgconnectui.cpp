//mod-this(Talk-UI)
#include "dlgconnectui.h"
#include "commonwidget.h"
//#include "contactinfoframe.h"
#include "talkui_define.h"
#include "qpictureanimation.h"
#include "commonfunction.h"

//mod-UIKernel
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"

//mod-Talk

#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talklogic/include/callinfo.h"

//mod-uimanager
#include "uimanager/rectdefine.h"
#include "baseui/t4xpicpath.h"

#include "talk/talklogic/include/commonfunctions.h"
#include "talkuidelegate.h"


const QRect RECT_CONNECT_ANIMATION(67, 131, 136, 11);// 动画控件的位置

CDlgConnectUI::CDlgConnectUI()
    : m_pPicAnimation(NULL)
{
    InitData();
}

CDlgConnectUI::~CDlgConnectUI(void)
{
}

// 初始化
void CDlgConnectUI::Init()
{
    CDlgConnectBase::Init();

    if (NULL != m_pPicAnimation)
    {
        m_pPicAnimation->Play();
    }
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgConnectUI::Uninit()
{
    if (NULL != m_pPicAnimation)
    {
        m_pPicAnimation->Stop();
    }

    CDlgConnectBase::Uninit();
}

//给界面设置数据
void CDlgConnectUI::SetTalkData(DataLogic2UIBase * pData)
{
    //调用基类设置数据
    CDlgConnectBase::SetTalkData(pData);

    DataConnect * pConnectData = dynamic_cast<DataConnect *>(pData);
    if (pConnectData != NULL)
    {
        //设置是否显示联系人头像
        bool bIsShowContactImage = IsDisplayContactPhoto();
        m_cConInfoFrame.AdjustLayout(bIsShowContactImage);

        //获取联系人信息显示策略
        SHOW_CONTACTDETAIL_TYPE nDisplayMethod = (SHOW_CONTACTDETAIL_TYPE)
                _TalkUIDelegate.GetDisplayMethod();

        CCallInfo * pCallInfo = pConnectData->pCallInfo;
        if (pCallInfo != NULL)
        {
            yl::string strName = GetNameFromCallInfo(pCallInfo);
            yl::string strNumber = GetNumberFromCallInfo(pCallInfo);
            yl::string strFullContactInfo = GetFullContactInfoFromCallInfo(pCallInfo);

            //根据这些信息获取 需要显示的信息
            ChangeNameAndNumberByInfoMethod(strName, strNumber, strFullContactInfo, nDisplayMethod);

            m_cConInfoFrame.SetContactName(strName);
            m_cConInfoFrame.SetContactNumber(strNumber);

            yl::string strContactImage = GetContactImageFromCallInfo(pCallInfo);

            m_cConInfoFrame.SetContactImage(strContactImage);

            SetCustomImageMark(strContactImage);
        }
    }
}

// NetWork、Dtmf、音量条等Tip显示状态变化时的处理
void CDlgConnectUI::OnTipChange(TALK_TIP_TYPE eTipType)
{
    // 针对不同的Tip类型显示或者隐藏相应的控件
    bool bShowConnectAnimation = true;

    switch (eTipType)
    {
    case TALK_TIP_DTMF:
        bShowConnectAnimation = false;
        break;
    case TALK_TIP_VOLUMEBAR:
        bShowConnectAnimation = false;
        break;
    case TALK_TIP_NETWORK:
        bShowConnectAnimation = false;
        break;
    case TALK_TIP_CUSTOM:
        bShowConnectAnimation = true;
        break;
    }

    if (NULL != m_pPicAnimation)
    {
        if (bShowConnectAnimation)
        {
            m_pPicAnimation->show();
        }
        else
        {
            m_pPicAnimation->hide();
        }
    }
}

//设置区域
void CDlgConnectUI::AdjustLayout()
{
    if (NULL != m_pPicAnimation)
    {
        m_pPicAnimation->setGeometry((this->width() - RECT_CONNECT_ANIMATION.width()) / 2 + 8,
                                     this->height() - RECT_CONNECT_ANIMATION.height() - 25,
                                     RECT_CONNECT_ANIMATION.width(),
                                     RECT_CONNECT_ANIMATION.height());
    }

    CDlgConnectBase::AdjustLayout();
}

//构造函数调用初始化数据
void CDlgConnectUI::InitData()
{
    //初始化动画控件
    m_pPicAnimation = new qMultiPicAnimation(this);
    if (m_pPicAnimation != NULL)
    {
        //设置动画控件大小和位置
        m_pPicAnimation->setGeometry(RECT_CONNECT_ANIMATION);

        m_pPicAnimation->AddPicture(PIC_ANIMATION_ONE);
        m_pPicAnimation->AddPicture(PIC_ANIMATION_TWO);
        m_pPicAnimation->AddPicture(PIC_ANIMATION_THREE);
        m_pPicAnimation->AddPicture(PIC_ANIMATION_FOUR);
        m_pPicAnimation->AddPicture(PIC_ANIMATION_FIVE);
        m_pPicAnimation->AddPicture(PIC_ANIMATION_SIX);

        m_pPicAnimation->SetPlaySpeed(200);

        //默认情况不显示，所以要特意执行show()
        m_pPicAnimation->show();
    }
}
