// mod-this(TalkUI)
#include "dlgfinishui.h"
#include "commonwidget.h"
#include "contactinfoframe.h"
#include "talkui_define.h"
#include "talkuidelegate.h"
#include "commonfunction.h"

// mod-InputMethod
#include "inputmethod/inputmethod.h"

//mod-UIKernel
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "uimanager/rectdefine.h"

//mod-CommonAPI
#include "commonapi/stringhelperapi.h"
#include "configparser/modconfigparser.h"

// mod-MainWnd
#include "uimanager/rectdefine.h"

#include "account/include/modaccount.h"
#include "talk/talklogic/include/modtalklogic.h"

// PicPath
#ifdef _T3X
#include "T3XStyleUI/T3XPicPath.h"
#endif

#ifdef _T4X
#include "baseui/t4xpicpath.h"
#endif
#include "talk/talklogic/include/callinfo.h"

//控件位置定义
const QRect CONTACT_INFO_FRAME_GEOMETRY(6, 0, RECT_TALK_FRAME.width() - 8,
                                        RECT_TALK_FRAME.height());
const QRect TIP_INFO_LABEL_GEOMETRY(12, 140, 450, 40);

CDlgFinishedUI::CDlgFinishedUI()
{
    InitData();
}

CDlgFinishedUI::~CDlgFinishedUI(void)
{
}

// 初始化
void CDlgFinishedUI::Init()
{
    CDlgFinishedBase::Init();
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgFinishedUI::Uninit()
{
    CDlgFinishedBase::Uninit();
}

//给界面设置数据
void CDlgFinishedUI::SetTalkData(DataLogic2UIBase * pData)
{
    //基类调用设置数据
    CDlgFinishedBase::SetTalkData(pData);

    DataCallFinished * pFinishData = dynamic_cast<DataCallFinished *>(pData);
    if (NULL == pFinishData)
    {
        return;
    }

    //设置是否显示联系人头像
    bool bIsShowContactImage = IsDisplayContactPhoto();
    m_cConInfoFrame.AdjustLayout(bIsShowContactImage);

    //获取联系人信息显示策略
    SHOW_CONTACTDETAIL_TYPE nDisplayMethod = (SHOW_CONTACTDETAIL_TYPE)
            _TalkUIDelegate.GetDisplayMethod();

    if (pFinishData->bError)
    {
        //隐藏通话时间提示
        m_qLabelTalkTime.hide();
        if (pFinishData->bInvalidAccount)
        {
            //若为无效帐号错误，则直接提示无效帐号
            m_qLabelInfo.SetLabText(LANG_TRANSLATE(TRID_INVALID_ACCOUNT));
        }
        else
        {
            //否则提示错误文本
            m_qLabelInfo.SetLabText(LANG_TRANSLATE(pFinishData->strInfo.c_str()));
        }
        m_qLabelInfo.show();
    }
    else
    {
        //正常通话结束情况
        //设置提示文本

        if (pFinishData->bAllowAutoRedial)
        {
            m_qLabelTalkTime.hide();
            m_qLabelInfo.show();

            if (pFinishData->redialData.eStatus == AUTOREDIAL_COUNTDOWN)
            {

                UpdateAutoRedialNote();
            }
            else
            {
                m_qLabelInfo.SetLabText(LANG_TRANSLATE(TRID_AUTO_REDIAL) + "?");
            }
        }
        else
        {
            //隐藏失败提示控件
            m_qLabelInfo.hide();

            //设置通话时间提示文本
            m_qLabelTalkTime.setText(LANG_TRANSLATE(pFinishData->strInfo.c_str()));

            //显示通话时间提示
            m_qLabelTalkTime.show();
        }
    }

    // 设置联系人信息
    CCallInfo * pCallInfo = pFinishData->pCallInfo;

    if (NULL != pCallInfo)
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

// 是否有显示提示信息
bool CDlgFinishedUI::IsShowTip()
{
    return (m_qLabelInfo.isActiveWindow());
}

// NetWork、Dtmf、音量条等Tip显示状态变化时的处理
void CDlgFinishedUI::OnTipChange(TALK_TIP_TYPE eTipType)
{

}

//设置区域
void CDlgFinishedUI::AdjustLayout()
{
    CDlgNormalTalkBase::AdjustLayout();

    m_qLabelInfo.setGeometry(TIP_INFO_LABEL_GEOMETRY.x(),
                             this->height() - TIP_INFO_LABEL_GEOMETRY.height() - 10,
                             TIP_INFO_LABEL_GEOMETRY.width(),
                             TIP_INFO_LABEL_GEOMETRY.height());

}

//构造函数调用初始化数据
void CDlgFinishedUI::InitData()
{
    //初始化通话失败图标控件
    m_qLabelInfo.setParent(this);

    //设置布局类型
    m_qLabelInfo.setAlignment(Qt::AlignCenter);
    m_qLabelInfo.setGeometry(TIP_INFO_LABEL_GEOMETRY);
    m_qLabelInfo.SetLabIcon(PIC_ICON_DECTLINE);
    m_qLabelInfo.hide();


    //初始化通话时间控件
    m_qLabelTalkTime.setParent(this);

    //设置位置
#ifdef _T4X
    m_qLabelTalkTime.setGeometry(158, 150, 340, 20);
#else
    m_qLabelTalkTime.setGeometry(260, 150, 340, 210);
#endif
    // 设置label的字体颜色
    SetWgtFontColor(&m_qLabelTalkTime, FONT_TYPE_TALK_TITLE);

}

//finish 界面使用
// 更新自动重拨提示语
void CDlgFinishedUI::UpdateAutoRedialNote()
{
    // 更新界面
    yl::string strInfo = commonAPI_FormatString("%s: %d, %s: %ds",
                         LANG_TRANSLATE(TRID_REDIAL_TIMES).toUtf8().data()
                         , m_iRemainRedialTimes, LANG_TRANSLATE(TRID_INTERVAL).toUtf8().data(), m_iRedialInterval);

    m_qLabelInfo.SetLabText(strInfo.c_str());
}


