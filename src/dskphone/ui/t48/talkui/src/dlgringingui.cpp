// mod-this(TalkUI)
#include "dlgringingui.h"
#include "commonfunction.h"

// mod-InputMethod
#include "inputmethod/inputmethod.h"

//mod-UIKernel
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"

#include "commonwidget.h"
#include "contactinfoframe.h"
#include "talkui_define.h"
#include "talkuimanager.h"
#include "qpictureanimation.h"

//mod-uicommon
#include "uicommon/imagemanager/modimagemanager.h"

//mod-Call center
#include "broadsoft/callcenter/include/modcallcenter.h"

//mod-MainWnd
#include "uimanager/rectdefine.h"

#include "baseui/fontcolor.h"

#ifdef _T4X
#include "baseui/t4xpicpath.h"
#endif

#ifdef _T3X
#include "T3XStyleUI/T3XPicPath.h"
#endif
#include "talk/talklogic/include/callinfo.h"

#include "talk/talklogic/include/modtalklogic.h"

#include "talkuidelegate.h"


// 动画控件的位置
const QRect RECT_CONNECT_ANIMATION(67, 131, 133, 26);

// 显示来电方信息位置
const QRect RECT_IMAGE_COMINGCALL(140, 90, 16, 9);
const QRect RECT_LOCAL_NAME(160, 73, 279, 40);
const QRect RECT_LOCAL_NUMBER(160, 113, 279, 25);
const QRect RECT_IMAGE_COMINGCALL_NO_CONTACTIMAGE(18, 90, 16, 9);
const QRect RECT_LOCAL_NAME_NO_CONTACTIMAGE(38, 73, 401, 40);
const QRect RECT_LOCAL_NUMBER_NO_CONTACTIMAGE(38, 113, 401, 25);

//构造函数
CDlgRingingUI::CDlgRingingUI()
    : m_pPicAnimation(NULL)
    , m_bIsShowTargetInfo(false)
{
    InitData();
}

CDlgRingingUI::~CDlgRingingUI(void)
{
}

// 初始化
void CDlgRingingUI::Init()
{
    CDlgRingingBase::Init();

    m_bIsShowTargetInfo = false;        //初始化设置不在 公共提示区域显示提示内容

    if (m_pPicAnimation != NULL)
    {
        m_pPicAnimation->Play();
    }
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgRingingUI::Uninit()
{
    if (m_pPicAnimation != NULL)
    {
        m_pPicAnimation->Stop();
    }

    CDlgRingingBase::Uninit();
}

yl::string CDlgRingingUI::MakeViaInfo(const yl::string & strViaName,
                                      const yl::string & strViaNumber)
{
    yl::string strTemp = "";

    if (!strViaNumber.empty() && !strViaName.empty())
    {
        strTemp = strViaName + " " + strViaNumber;
    }
    else
    {
        strTemp = strViaName + strViaNumber;
    }

    return strTemp;
}

//由于联系人头像显示的变化导致的布局变化
void CDlgRingingUI::ResetWidgetByContactPhotoVisibleChange()
{
    AdjustLayout();
}

//给界面设置数据
void CDlgRingingUI::SetTalkData(DataLogic2UIBase * pData)
{
    //基类设置数据
    CDlgRingingBase::SetTalkData(pData);

    // 入口参数判断
    DataRing * pRingingtData = dynamic_cast<DataRing *>(pData);
    if (pRingingtData == NULL)
    {
        return;
    }

    m_bIsShowTargetInfo = false; // 在设置数据的时候 先设置成不在公共区域显示提示内容

    //设置是否显示联系人头像
    bool bIsShowContactImage = IsDisplayContactPhoto();
    m_cConInfoFrame.AdjustLayout(bIsShowContactImage);

    yl::string strViaInfo = "";
    bool bIsRecall = IsRecall();

    //配置文件中 需要显示via信息 而且不是Recall  那么 才需要给viainfo 赋值
    if (_TalkUIDelegate.IsShowViaInfo() && (!bIsRecall))
    {
        //http://10.2.1.199/Bug.php?BugID=88836 VIA信息根据DisplayMethod来显示
        SHOW_CONTACTDETAIL_TYPE nDisplayMethod = (SHOW_CONTACTDETAIL_TYPE)
                _TalkUIDelegate.GetDisplayMethod();
        yl::string strViaName = pRingingtData->m_strViaName;
        yl::string strViaNumber = pRingingtData->m_strViaNumber;
        yl::string strViaFullContact = pRingingtData->m_strViaFullContact;
        ChangeNameAndNumberByInfoMethod(strViaName, strViaNumber, strViaFullContact, nDisplayMethod);
        strViaInfo = MakeViaInfo(strViaName, strViaNumber);
    }

    //先清除一下 TargetInfo 信息
    ClearTargetInfo();

    CCallInfo * pCallInfo = pRingingtData->pCallInfo;
    if (NULL != pCallInfo)
    {
        yl::string strName = GetNameFromCallInfo(pCallInfo);
        yl::string strNumber = GetNumberFromCallInfo(pCallInfo);
        yl::string strFullContactInfo = GetFullContactInfoFromCallInfo(pCallInfo);
        yl::string strContactImage = GetContactImageFromCallInfo(pCallInfo);

        //更新联系人控件信息
        UpdateContactInfo(strName, strNumber, strFullContactInfo, strContactImage, bIsRecall, strViaInfo);

#if IF_BUG_26914
        if (!pRingingtData->m_strViaName.empty())
        {
            m_qLocalName.setText(toQString(pRingingtData->m_strViaName.c_str()));
            m_qLocalNumber.setText(toQString(pRingingtData->m_strViaNumber.c_str()));
            m_qImageComingCall.show();
            m_qLocalName.show();
            m_qLocalNumber.show();
        }
#endif
        //如果 需要显示TargetInfo 而且 viainfo 为空 那么才需要给 targetinfo 赋值
        if (_TalkUIDelegate.IsShowTargetInfo() && strViaInfo.empty())
        {
            yl::string strLocalName = "";
            yl::string strLocalNumber = "";
            yl::string strLocalFullContactInfo = "";

            //先根据sip 消息获取
            strLocalName = pCallInfo->m_tLocalInfo.sDisplayName;
            strLocalNumber = pCallInfo->m_tLocalInfo.sSIPName;
            strLocalFullContactInfo = pCallInfo->m_tLocalInfo.sFullContactInfo;

            if (strLocalName.empty()
                    && strLocalNumber.empty()
                    && strLocalFullContactInfo.empty()
               )
            {
                int iSipAccountID = pCallInfo->GetAccountID();

                //先获取 hoteling 账号
                strLocalName = _TalkUIDelegate.GetHotelingUserName(iSipAccountID);

                //如果获取的hoteling账号为空 那么 才需要获取其他账号信息
                if (strLocalName.empty())
                {
                    strLocalName = GetLabel(iSipAccountID);

                    strLocalNumber = GetUserName(iSipAccountID);
                }
            }

            //根据这些信息获取 需要显示的信息
            ChangeNameAndNumberByInfoMethod(strLocalName, strLocalNumber, strLocalFullContactInfo,
                                            (SHOW_CONTACTDETAIL_TYPE)_TalkUIDelegate.GetDisplayMethod());

            //在这边会改变 m_bIsShowTargetInfo 标识
            ShowTargetInfo(strLocalName, strLocalNumber);

        }

    }
}

// NetWork、Dtmf、音量条等Tip显示状态变化时的处理
void CDlgRingingUI::OnTipChange(TALK_TIP_TYPE eTipType)
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
void CDlgRingingUI::AdjustLayout()
{
    CDlgRingingBase::AdjustLayout();

    // 动画控件位置
    if (m_pPicAnimation != NULL)
    {
        int nAnimationTop = m_bIsShowTargetInfo ? this->height() - RECT_CONNECT_ANIMATION.height() - 15
                            : this->height() - RECT_CONNECT_ANIMATION.height() - 25;

        m_pPicAnimation->setGeometry((this->width() - RECT_CONNECT_ANIMATION.width()) / 2 + 8,
                                     nAnimationTop,
                                     RECT_CONNECT_ANIMATION.width(),
                                     RECT_CONNECT_ANIMATION.height());
    }

    // 来电信息控制位置
    if (IsDisplayContactPhoto())
    {
        m_qLocalName.setGeometry(RECT_LOCAL_NAME);
        m_qLocalNumber.setGeometry(RECT_LOCAL_NUMBER);

        m_qImageComingCall.setGeometry(RECT_IMAGE_COMINGCALL);
    }
    else
    {
        m_qLocalName.setGeometry(RECT_LOCAL_NAME_NO_CONTACTIMAGE);
        m_qLocalNumber.setGeometry(RECT_LOCAL_NUMBER_NO_CONTACTIMAGE);

        m_qImageComingCall.setGeometry(RECT_IMAGE_COMINGCALL_NO_CONTACTIMAGE);
    }
}

#if IF_FEATURE_NFON_RTP
bool CDlgRingingUI::OnUIAction(const QString & strAction)
{
    yl::string strUIAction = strAction.toUtf8().data();
    if (strUIAction == TALK_UI_ACTION_ANSWER)
    {
        if (m_pPicAnimation != NULL)
        {
            m_pPicAnimation->Stop();
        }

        m_qLocalName.StopScrollTimer();
        m_qLocalNumber.StopScrollTimer();
        m_cConInfoFrame.StopAllScrollTimer();
    }
    return CDlgRingingBase::OnUIAction(strAction);
}
#endif

//构造函数调用初始化数据
void CDlgRingingUI::InitData()
{
    m_bIsShowTargetInfo = false;        //初始化设置不在公共提示位置 显示内容

    //初始化动画控件
    m_pPicAnimation = new qMultiPicAnimation(this);
    if (m_pPicAnimation != NULL)
    {
        //设置动画控件大小和位置
        m_pPicAnimation->setObjectName("PicAnimation");
        m_pPicAnimation->setGeometry(RECT_CONNECT_ANIMATION);
        m_pPicAnimation->AddPicture(PIC_ANIMATION_SEVEN);
        m_pPicAnimation->AddPicture(PIC_ANIMATION_EIGHT);
        m_pPicAnimation->AddPicture(PIC_ANIMATION_NINE);
        m_pPicAnimation->SetPlaySpeed(300);

        //默认情况不显示，所以要特意执行show()
        m_pPicAnimation->show();
    }

    //来电目标图片
    m_qImageComingCall.setObjectName("ImageComingCall");
    m_qImageComingCall.setParent(this);

    m_qImageComingCall.setGeometry(RECT_IMAGE_COMINGCALL);
    m_qImageComingCall.setPixmap(THEME_GET_BMP(PIC_ICON_TO_LINE_ARROW_DOUBLE));


    QFont font;
    font.setPointSize(24);
    font.setWeight(75);
    font.setBold(true);

    //目标名称
    m_qLocalName.setObjectName("LocalName");
    m_qLocalName.setParent(this);

    m_qLocalName.setFont(font);
    m_qLocalName.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    SetWgtFontColor(&m_qLocalName, FONT_TYPE_TALK_TEXT);

    //目标号码
    m_qLocalNumber.setObjectName("LocalNumber");
    m_qLocalNumber.setParent(this);

    font.setPointSize(16);
    m_qLocalNumber.setFont(font);
    m_qLocalNumber.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    SetWgtFontColor(&m_qLocalNumber, FONT_TYPE_TALK_TEXT);

}

//清除TargetInf
void CDlgRingingUI::ClearTargetInfo()
{
    m_qImageComingCall.hide();

    m_qLocalName.hide();

    m_qLocalNumber.hide();

}

//显示 TargetInfo
void CDlgRingingUI::ShowTargetInfo(yl::string strTargetName, yl::string strTargetNumber)
{
    //不显示targetinfo 直接返回
    if (!_TalkUIDelegate.IsShowTargetInfo())
    {
        return ;
    }

    //只要有一个不为空 那么 就要显示那个图片
    if (!strTargetName.empty() || !strTargetNumber.empty())
    {
        m_qImageComingCall.show();

        //这边设置显示 targetinfo 了
        m_bIsShowTargetInfo = true;
    }
    else
    {
        m_qImageComingCall.hide();

        //这边设置不显示 targetinfo 了
        m_bIsShowTargetInfo = false;
    }

    if (!strTargetName.empty())
    {
        m_qLocalName.setText(toQString(strTargetName));
        m_qLocalName.show();
    }
    else
    {
        m_qLocalName.hide();
    }

    if (!strTargetNumber.empty())
    {
        m_qLocalNumber.setText(toQString(strTargetNumber));
        m_qLocalNumber.show();
    }
    else
    {
        m_qLocalNumber.hide();
    }

}

// 刷新联系人信息
void CDlgRingingUI::UpdateContactInfo(yl::string strName, yl::string strNumber,
                                      yl::string strFullContactInfo, yl::string strContactImage, bool bIsRecall, yl::string strViaInfo)
{
    //获取联系人信息显示策略
    SHOW_CONTACTDETAIL_TYPE nDisplayMethod = (SHOW_CONTACTDETAIL_TYPE)
            _TalkUIDelegate.GetDisplayMethod();

    //如果是Recall   有Recall 那么肯定是不可能有via信息的
    if (bIsRecall)
    {
        //根据这些信息获取 需要显示的信息
        ChangeNameAndNumberByInfoMethod(strName, strNumber, strFullContactInfo, nDisplayMethod);

        //根据产品部意见 把Recall 放在第一行  然后用户信息放在第二行显示
        strNumber = strName;
        strName = LANG_TRANSLATE(TRID_RECALL).toUtf8().data();

        //如果是Recall 那么要把 via 赋值为空  因为有Recall 信息是没有via信息的 防止后面赋值错误  所以这边做下容错 赋值为空
        strViaInfo = "";
    }
    else
    {
        //如果需要显示ViaInfo 而且有via信息
        if (_TalkUIDelegate.IsShowViaInfo() && !strViaInfo.empty())
        {
            //如果名字为空 那么先要给name 赋值一下
            if (strName.empty())
            {
                strName = strNumber;
            }
#if !IF_BUG_26914
            //如果 名称不为空 那么要加上from
            if (!strName.empty())
            {
                strName = LANG_TRANSLATE(TRID_TALK_FROM).toUtf8().data() + yl::string(":") + strName;
            }

            //如果号码不为空 那么前面要加上number
            if (!strNumber.empty())
            {
                strNumber = LANG_TRANSLATE(TRID_FROM_NUMBER).toUtf8().data() + yl::string(":") + strNumber;
            }

            strViaInfo = LANG_TRANSLATE(TRID_VIA).toUtf8().data()  + yl::string(":") + strViaInfo;
#endif
        }

        //根据这些信息获取 需要显示的信息
        ChangeNameAndNumberByInfoMethod(strName, strNumber, strFullContactInfo, nDisplayMethod);
    }

    //重置一下联系人信息
    m_cConInfoFrame.Clear();
    ////根据读取的配置信息 设置是否显示联系人头像
    //bool bShowContactImage = IsDisplayContactPhoto();
    //m_pConInfoFrame->SetIsShowContactImage(bShowContactImage);

    m_cConInfoFrame.AdjustScrollTimer();
    //设置联系人相关信息
    m_cConInfoFrame.SetContactName(strName);
    m_cConInfoFrame.SetContactNumber(strNumber);
    m_cConInfoFrame.SetContactImage(strContactImage);

    SetCustomImageMark(strContactImage);
#if !IF_BUG_26914
    m_cConInfoFrame.SetContactInfo(strViaInfo);
#endif
    ////设置联系人相关信息
    //m_pConInfoFrame->SetContactName(strName);
    //m_pConInfoFrame->SetContactNumber(strNumber);
    //m_pConInfoFrame->SetContactImage(strContactImage);

    //m_pConInfoFrame->SetContactInfo(strViaInfo);
}

