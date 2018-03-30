// mod-this(TalkUI)
#include "dlgparknotifyui.h"
#include "contactinfoframe.h"
#include "commonwidget.h"
#include "commonfunction.h"

// mod-UIKernel
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"

// mod-CallPark
#include "broadsoft/callpark/include/callparkcommon.h"

// mod-CommonUnits
#include "commonunits/modcommonunits.h"

// mod-MainWnd
#include "uimanager/rectdefine.h"

#include "imagemanager/modimagemanager.h"
//#include "uikernel/qtimepiece.h"
#include "devicelib/networkserver.h"

// mod-StyleUI
#ifdef _T4X
#include "baseui/t4xpicpath.h"
#else
#include "T3XStyleUI/T3XPicPath.h"
#endif
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/callinfo.h"


#include "talkuidelegate.h"

const QRect COVER_LABEL_GEOMETRY(19, 13, 106, 106);
const QRect STATUS_ICON_LAB_GEOMETRY = COVER_LABEL_GEOMETRY;
const QRect STATUS_ICON_LAB_NO_CONTACTIMAGE(18, 78, 38, 36);
const QRect STATUS_TEXT_LAB_NO_CONTACTIMAGE(58, 78, 410, 36);

CDlgParkNotifyUI::CDlgParkNotifyUI()
{
    InitData();
}

CDlgParkNotifyUI::~CDlgParkNotifyUI(void)
{
}

// 初始化
void CDlgParkNotifyUI::Init()
{
    CDlgParkNotifyBase::Init();
    m_qTimePiece.StopTimer();
    m_qTimePiece.StartTimer();

}

// 释放数据，将窗口清空回复到初始化状态
void CDlgParkNotifyUI::Uninit()
{
    //重新启动计时器
    m_qTimePiece.StopTimer();
    CDlgParkNotifyBase::Uninit();

}

//给界面设置数据
void CDlgParkNotifyUI::SetTalkData(DataLogic2UIBase * pData)
{
    //基类设置数据
    CDlgParkNotifyBase::SetTalkData(pData);

    // 入口参数检查
    DataParkNotify * pNotifyData = dynamic_cast<DataParkNotify *>(pData);
    if (NULL == pNotifyData)
    {
        return;
    }

    //设置是否显示联系人头像
    bool bIsShowContactImage = IsDisplayContactPhoto();
    m_cConInfoFrame.AdjustLayout(bIsShowContactImage);

    //获取联系人信息显示策略
    SHOW_CONTACTDETAIL_TYPE nDisplayMethod = (SHOW_CONTACTDETAIL_TYPE)
            _TalkUIDelegate.GetDisplayMethod();

    //先把联系人控件 数据清空
    m_cConInfoFrame.Clear();

    yl::string strName = pNotifyData->strDiaplayName;
    yl::string strNumber = pNotifyData->strNumber;
    yl::string strFullContactInfo = "";

    //根据这些信息获取 需要显示的信息
    ChangeNameAndNumberByInfoMethod(strName, strNumber, strFullContactInfo, nDisplayMethod);

    m_cConInfoFrame.SetContactName(strName);
    m_cConInfoFrame.SetContactNumber(strNumber);

    //m_cConInfoFrame.SetContactName(toQString(pNotifyData->strDiaplayName));
    //m_cConInfoFrame.SetContactNumber((LPCSTR)pNotifyData->strNumber);

    // 服务器下发的Notify信息没有头像信息,使用默认联系人头像
    yl::string strDefaultImage = _TalkUIDelegate.GetDefaultContactImage();
    m_cConInfoFrame.SetContactImage(strDefaultImage);

    SetCustomImageMark(strDefaultImage);
    //v80 parknotify 没有CallInfo
    //if (NULL != pNotifyData->pCallInfo)
    //{
    //  time_t tTime = netGetLocalTime() - pNotifyData->pCallInfo->m_tBornTime;
    //  m_qTimePiece.SetTime(tTime / 3600, (tTime / 60) % 60, tTime % 60);
    //  m_qTimePiece.OnFlashTimer();
    //}

    if (IsDisplayContactPhoto())
    {
        m_qParkIcon.hide();
        m_qParkTip.hide();
        m_qCoverLabel.show();
        m_qIconLabel.show();
    }
    else
    {
        m_qParkIcon.show();
        m_qParkTip.show();
        m_qCoverLabel.hide();
        m_qIconLabel.hide();
    }

    //emit ChangeTitle(TRID_CALL_PARK);
}
//
//// NetWork、Dtmf、音量条等Tip显示状态变化时的处理
//void CDlgParkNotifyWidget::OnTipChange(TALK_TIP_TYPE eTipType)
//{
//
//}


//获取通话时间内容
QCustomTime CDlgParkNotifyUI::GetTalkTime()
{
    UINT uHour = 0, uMinute = 0, uSecond = 0;
    m_qTimePiece.GetTime(uHour, uMinute, uSecond);
    QCustomTime timeTalk(uHour, uMinute, uSecond);
    return timeTalk;
}

//构造函数调用初始化数据
void CDlgParkNotifyUI::InitData()
{
    // 初始化计时控件
    m_qTimePiece.setParent(this);
    m_qTimePiece.hide();

    ////显示联系人提示的信息遮罩
    //m_pCoverLabel = new QLabel(this);
    //if (NULL != m_pCoverLabel)
    //{
    //  m_pCoverLabel->setScaledContents(true);
    //  m_pCoverLabel->setGeometry(STATUS_ICON_LAB_GEOMETRY);
    //  //覆盖一张半透明的黑色图片
    //  m_pCoverLabel->setPixmap(THEME_GET_BMP(PIC_GENERAL_ICON_COVER));
    //  m_pCoverLabel->hide();
    //}

    //显示联系人提示的信息遮罩
    m_qCoverLabel.setParent(this);

    m_qCoverLabel.setScaledContents(true);
    m_qCoverLabel.setGeometry(STATUS_ICON_LAB_GEOMETRY);
    //覆盖一张半透明的黑色图片
    m_qCoverLabel.setPixmap(THEME_GET_BMP(PIC_GENERAL_ICON_COVER));
    m_qCoverLabel.hide();


    // 初始化提示图标控件
    //m_pIconLabel = new IconLabel(this);
    //if (NULL != m_pIconLabel)
    //{
    //  m_pIconLabel->setGeometry(STATUS_ICON_LAB_GEOMETRY);
    //  m_pIconLabel->setAlignment(Qt::AlignCenter);
    //  m_pIconLabel->SetTipsIcon(PIC_ICON_HOLD);
    //  m_pIconLabel->SetTipsText(LANG_TRANSLATE(TRID_CALL_PARKED));
    //  m_pIconLabel->hide();
    //}

    // 初始化提示图标控件
    m_qIconLabel.setParent(this);

    m_qIconLabel.setGeometry(STATUS_ICON_LAB_GEOMETRY);
    m_qIconLabel.setAlignment(Qt::AlignCenter);
    m_qIconLabel.SetTipsIcon(PIC_ICON_HOLD);
    m_qIconLabel.SetTipsText(LANG_TRANSLATE(TRID_CALL_PARKED));
    m_qIconLabel.hide();



    //不显示头像时使用的park图标
    //m_pParkIcon = new QLabel(this);
    //if (NULL != m_pParkIcon)
    //{
    //  m_pParkIcon->setGeometry(STATUS_ICON_LAB_NO_CONTACTIMAGE);
    //  m_pParkIcon->setPixmap(THEME_GET_BMP(PIC_ICON_HOLD));
    //  m_pParkIcon->hide();
    //}

    //不显示头像时使用的park图标
    m_qParkIcon.setParent(this);

    m_qParkIcon.setGeometry(STATUS_ICON_LAB_NO_CONTACTIMAGE);
    m_qParkIcon.setPixmap(THEME_GET_BMP(PIC_ICON_HOLD));
    m_qParkIcon.hide();

    //不显示头像时使用的park提示
    //m_pParkTip = new QLabel(this);
    //if (NULL != m_pParkTip)
    //{
    //  m_pParkTip->setGeometry(STATUS_TEXT_LAB_NO_CONTACTIMAGE);
    //  m_pParkTip->setAlignment(Qt::AlignVCenter| Qt::AlignLeft);
    //  m_pParkTip->setText(LANG_TRANSLATE(TRID_CALL_PARKED));
    //  QPalette objQPalette;
    //  objQPalette.setColor(QPalette::WindowText, Qt::white);
    //  m_pParkTip->setPalette(objQPalette);
    //  m_pParkTip->hide();
    //}

    //不显示头像时使用的park提示
    m_qParkTip.setParent(this);

    m_qParkTip.setGeometry(STATUS_TEXT_LAB_NO_CONTACTIMAGE);
    m_qParkTip.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_qParkTip.setText(LANG_TRANSLATE(TRID_CALL_PARKED));
    QPalette objQPalette;
    objQPalette.setColor(QPalette::WindowText, Qt::white);
    m_qParkTip.setPalette(objQPalette);
    m_qParkTip.hide();


}
