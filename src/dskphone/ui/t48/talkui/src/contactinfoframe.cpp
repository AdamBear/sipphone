//mod-this(TalkUI)
#include "contactinfoframe.h"

//mod-ImageManager
#include "imagemanager/modimagemanager.h"

//mod-UIKernel
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"

//mod-CommonUnits
#include "commonunits/modcommonunits.h"

#include "uikernel/qwidgetlist.h"
#include "uikernel/qwidgetutility.h"

#include "baseui/t4xpicpath.h"
#include "commonwidget.h"
#include "talkui_define.h"
#include "uikernel/languagehelper.h"
#include "talkuidelegate.h"

#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "customtheme/modcustomtheme.h"

//控件位置
const QRect   LAB_CONTACT_IMAGE(12, 12, 108, 108);
const QRect   LAB_CONTACT_NAME(134, 10, 300, 40);
const QRect   LAB_CONTACT_NUM(134, 50, 300, 25);
const QRect   LAB_CONTACT_DEPARTMENT(134, 75, 300, 30);
const QRect   LAB_CONTACT_NAME_NO_CONTACTIMAGE(12, 8, 422, 40);
const QRect   LAB_CONTACT_NUM_NO_CONTACTIMAGE(12, 48, 422, 25);
const QRect   LAB_CONTACT_DEPARTMENT_NO_CONTACTIMAGE(12, 75, 422, 30);
const QRect   QLIST_ICON_GEOMETRY(2, 82, 104, 24);
const QSize   QLIST_ICON_SIZE(22, 22);
const QSize   QLIST_ICON_SIZE_EXTEND(33, 22);

//////////////////////////////////////////////////////////////////////////
qLabContactImage::qLabContactImage(QWidget * parent)
    : m_pWidgetList(NULL)
{
    m_pWidgetList = new qWidgetList(this, qWidgetList::ALIGN_TYPE_RIGHT, QRect(QLIST_ICON_GEOMETRY));
    if (NULL != m_pWidgetList)
    {
        m_pWidgetList->setObjectName("WidgetList");
        qWidgetUtility::SetBackGround(m_pWidgetList, PIC_GENERAL_TALK_CONTACT_SHADING);
        m_pWidgetList->hide();
    }

    m_labCover.setParent(this);
    m_labCover.setGeometry(0, 0, LAB_CONTACT_IMAGE.width(), LAB_CONTACT_IMAGE.height());
    m_labCover.setScaledContents(true);
    m_labCover.setPixmap(THEME_GET_BMP(PIC_GENERAL_ICON_COVER));//覆盖一张半透明的黑色图片
    m_labCover.hide();

    m_pIconLabel = new IconLabel(this);
    if (NULL != m_pIconLabel)
    {
        m_pIconLabel->setGeometry(0, 0, LAB_CONTACT_IMAGE.width(), LAB_CONTACT_IMAGE.height());
        m_pIconLabel->setAlignment(Qt::AlignHCenter);
        CustomTheme_SetWidgetFontProperty(m_pIconLabel, CUSTOM_FONT_ID_TALK_NORMAL);
        m_pIconLabel->hide();
    }
}

qLabContactImage::~qLabContactImage()
{

}

// 清空控件数据
void qLabContactImage::Clear()
{
    ClearTip();

    //这边不清空图标了
//  DeleteAllIcon();

    //if (NULL != m_pWidgetList)
    //{
    //  m_pWidgetList->Clear();
    //}
}

// 设置Tip信息
void qLabContactImage::SetTipInfo(const QString & strTip, const QString & strPicPath)
{
    m_labCover.show();

    if (NULL != m_pIconLabel)
    {
        m_pIconLabel->SetTipsIcon(strPicPath);
        m_pIconLabel->SetTipsText(strTip);
        m_pIconLabel->show();
    }
}

// 清空
void qLabContactImage::ClearTip()
{
    m_labCover.hide();

    if (NULL != m_pIconLabel)
    {
        m_pIconLabel->SetTipsIcon("");
        m_pIconLabel->SetTipsText("");
        m_pIconLabel->hide();
    }
}

//设置联系人头像
void qLabContactImage::SetImage(const yl::string & strImagePath)
{
    TALKUI_INFO("ContactImage SetImage[%s]", strImagePath.c_str());
    m_strContactImagePath = strImagePath;

    QPixmap pmIcon;
    if (pathFileExist(m_strContactImagePath.c_str()))
    {
        pmIcon = STOCK_GET_BMP(m_strContactImagePath.c_str());
    }

    if (pmIcon.isNull())
    {
        m_strContactImagePath = PIC_CONTACTS_IMAGE_DEFAULT;
        pmIcon = THEME_GET_BMP(m_strContactImagePath.c_str());
    }

    pmIcon = pmIcon.scaled(LAB_CONTACT_IMAGE.width(), LAB_CONTACT_IMAGE.height(),
                           Qt::IgnoreAspectRatio);

    this->setPixmap(pmIcon);
}

// 添加、删除右上角的图标
void qLabContactImage::AddOrDeleteIcon(LPCSTR strIconPath, bool bAdd)
{
    if (NULL == m_pWidgetList)
    {
        return;
    }

    if (bAdd)
    {
        if (!m_pWidgetList->IsItemExist(strIconPath))
        {
            // 添加图标
            qWidgetListItemDataLab objItemLab;
            objItemLab.itemSize = GetIconSize(strIconPath);
            objItemLab.strPicPath = strIconPath;
            objItemLab.strId      = strIconPath;
            m_pWidgetList->AddItem(&objItemLab, 0);
        }
    }
    else
    {
        // 删除图标
        AnimationData objAnimation;
        m_pWidgetList->DeleteItem(objAnimation, strIconPath);
    }

    (m_pWidgetList->GetIconNum() == 0) ? m_pWidgetList->hide() : m_pWidgetList->show();
}

//清除所有 ICON
void qLabContactImage::DeleteAllIcon()
{
    if (NULL != m_pWidgetList)
    {
        m_pWidgetList->Clear();

        (m_pWidgetList->GetIconNum() == 0) ? m_pWidgetList->hide() : m_pWidgetList->show();
    }
}

QSize qLabContactImage::GetIconSize(LPCSTR strIconPath)
{
    QSize itemSize = QLIST_ICON_SIZE;
    if (strcmp(strIconPath, PIC_ICON_AMR) == 0
            || strcmp(strIconPath, PIC_ICON_OPUS) == 0)
    {
        itemSize = QLIST_ICON_SIZE_EXTEND;
    }
    return itemSize;
}

//////////////////////////////////////////////////////////////////////////
//构造函数
CContactInfoFrame::CContactInfoFrame(QWidget * parent/* = 0*/)
    : QFrame(parent)
    , m_bMergerNameAndNumber(true)
{
    InitData();
}

//析构函数
CContactInfoFrame::~CContactInfoFrame()
{

}

// 初始化界面数据
void CContactInfoFrame::InitData()
{
    m_labImage.setParent(this);
    m_labImage.setObjectName("labImage");
    m_labImage.setGeometry(LAB_CONTACT_IMAGE);

    m_labContactName.setParent(this);
    m_labContactNumber.setParent(this);
    m_labContactInfo.setParent(this);

    m_labContactName.setObjectName("labContactName");
    m_labContactNumber.setObjectName("labContactNumber");
    m_labContactInfo.setObjectName("labContactInfo");

    m_labContactName.setGeometry(LAB_CONTACT_NAME);
    m_labContactNumber.setGeometry(LAB_CONTACT_NUM);
    m_labContactInfo.setGeometry(LAB_CONTACT_DEPARTMENT);

    CustomTheme_SetWidgetFontProperty(&m_labContactName, CUSTOM_FONT_ID_TALK_CONTACT_INFO_NAME, true,
                                      QFont::Black);
    CustomTheme_SetWidgetFontProperty(&m_labContactNumber, CUSTOM_FONT_ID_TALK_CONTACT_INFO_NUMBER,
                                      true, QFont::Black);
    CustomTheme_SetWidgetFontProperty(&m_labContactInfo, CUSTOM_FONT_ID_TALK_CONTACT_INFO_INFO, true,
                                      QFont::Black);

    m_labContactName.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_labContactNumber.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_labContactInfo.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
}

// 调整控件布局
// | [in] bDisplayContactPhoto | 是否显示头像
void CContactInfoFrame::AdjustLayout(bool bDisplayContactPhoto)
{
    if (bDisplayContactPhoto)
    {
        m_labContactName.setGeometry(LAB_CONTACT_NAME);
        m_labContactNumber.setGeometry(LAB_CONTACT_NUM);
        m_labContactInfo.setGeometry(LAB_CONTACT_DEPARTMENT);
        m_labImage.show();
    }
    else
    {
        m_labContactName.setGeometry(LAB_CONTACT_NAME_NO_CONTACTIMAGE);
        m_labContactNumber.setGeometry(LAB_CONTACT_NUM_NO_CONTACTIMAGE);
        m_labContactInfo.setGeometry(LAB_CONTACT_DEPARTMENT_NO_CONTACTIMAGE);
        m_labImage.hide();
    }
}

void CContactInfoFrame::Clear()
{
    m_labContactName.clear();
    m_labContactNumber.clear();
    m_labImage.Clear();
    m_labContactInfo.clear();

    m_labContactName.resetScroolTimer();
}

//设置联系人名称
void CContactInfoFrame::SetContactName(const yl::string & strName)
{
    m_labContactName.setText(toQString(strName));
}

//设置联系人名称
void CContactInfoFrame::SetContactName(const QString & strName)
{
    m_labContactName.setText(strName);

    MergerNameAndNumber();
}

//设置联系人部门
void CContactInfoFrame::SetContactInfo(const yl::string & strInfo)
{
    m_labContactInfo.setText(toQString(strInfo));
}

//设置联系人号码
void CContactInfoFrame::SetContactNumber(const yl::string & strNumber)
{
    m_labContactNumber.setText(toQString(strNumber));

    MergerNameAndNumber();
}

// 设置Merger的开关
void CContactInfoFrame::SetMergerSwitch(bool bMerger)
{
    m_bMergerNameAndNumber = bMerger;
}

// 获取Merger的开关
bool CContactInfoFrame::GetMergerSwitch()
{
    return m_bMergerNameAndNumber;
}

// 名字和号码一样的时候保留一个
void CContactInfoFrame::MergerNameAndNumber()
{
    if (!m_bMergerNameAndNumber)
    {
        return;
    }

    if (m_labContactName.text() == m_labContactNumber.text())
    {
        m_labContactNumber.hide();
    }
    else
    {
        m_labContactNumber.show();
    }
}

void CContactInfoFrame::SetContactImage(const yl::string & strImagePath)
{
    m_labImage.SetImage(strImagePath);
}

void CContactInfoFrame::AddOrDeleteIcon(LPCSTR strIconPath, bool bAdd)
{
    m_labImage.AddOrDeleteIcon(strIconPath, bAdd);
}

//清除所有 ICON
void CContactInfoFrame::DeleteAllIcon()
{
    m_labImage.DeleteAllIcon();
}

//////////////////////////////////////////////////////////////////////////
const QRect   LAB_CONFERENCE_CONTACT_NAME(134, 20, 300, 40);
const QRect   BTN_MUTE_GEOMETRY(134, 75, 38, 38);
const QRect   BTN_FARHOLD_GEOMETRY(192, 75, 38, 38);
const QRect   BTN_REMOVE_GEOMETRY(250, 75, 38, 38);

const QRect   LAB_CONFERENCE_CONTACT_NAME_NO_CANTACTIMAGE(12, 20, 435, 40);
const QRect   BTN_MUTE_GEOMETRY_NO_CANTACTIMAGE(12, 75, 38, 38);
const QRect   BTN_FARHOLD_GEOMETRY_NO_CANTACTIMAGE(70, 75, 38, 38);
const QRect   BTN_REMOVE_GEOMETRY_NO_CANTACTIMAGE(128, 75, 38, 38);

CConferenceInfoFrame::CConferenceInfoFrame(QWidget * parent)
    : QFrame(parent)
    , m_iCallID(-1)
    , m_pBtnMute(NULL)
    , m_pBtnRemove(NULL)
    , m_pBtnFarHold(NULL)

{
    setObjectName("CConferenceInfoFrame");
    m_labImage.setParent(this);
    m_labImage.setObjectName("labImage");
    m_labImage.setGeometry(LAB_CONTACT_IMAGE);

    QFont font;
    font.setPointSize(24);
    font.setWeight(75);
    font.setBold(true);

    font.setWeight(QFont::Black);
    m_labContactName.setFont(font);
    m_labContactName.setParent(this);
    m_labContactName.setObjectName("labContactName");
    m_labContactName.setGeometry(LAB_CONFERENCE_CONTACT_NAME);
    m_labContactName.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    SetWgtFontColor(&m_labContactName, FONT_TYPE_TALK_TEXT);

    m_pBtnMute = new qUIPushButton(this);
    if (NULL != m_pBtnMute)
    {
        m_pBtnMute->setObjectName("BtnMute");
        m_pBtnMute->setGeometry(QRect(BTN_MUTE_GEOMETRY));
        m_pBtnMute->setBtnAction(TALK_UI_ACTION_FARMUTE);
        m_pBtnMute->set3StatePic(PIC_ICON_TALK_IN_MUTE, PIC_ICON_TALK_IN_MUTE, PIC_ICON_TALK_IN_MUTE);
        connect(m_pBtnMute, SIGNAL(clicked(QString)), this, SLOT(OnBtnClicked(QString)));
    }

    m_pBtnFarHold = new qUIPushButton(this);
    if (NULL != m_pBtnFarHold)
    {
        m_pBtnFarHold->setObjectName("BtnFarHold");
        m_pBtnFarHold->setGeometry(QRect(BTN_FARHOLD_GEOMETRY));
        m_pBtnFarHold->setBtnAction(TALK_UI_ACTION_FARHOLD);
        m_pBtnFarHold->set3StatePic(PIC_SOFTKEY_HOLD, PIC_SOFTKEY_HOLD, PIC_SOFTKEY_HOLD);
        connect(m_pBtnFarHold, SIGNAL(clicked(QString)), this, SLOT(OnBtnClicked(QString)));
    }

    m_pBtnRemove = new qUIPushButton(this);
    if (NULL != m_pBtnRemove)
    {
        m_pBtnRemove->setObjectName("BtnRemove");
        m_pBtnRemove->setGeometry(QRect(BTN_REMOVE_GEOMETRY));
        m_pBtnRemove->setBtnAction(TALK_UI_ACTION_REMOVE);
        m_pBtnRemove->set3StatePic(PIC_ICON_TALK_REMOVE, PIC_ICON_TALK_REMOVE_PRESS, PIC_ICON_TALK_REMOVE);
        connect(m_pBtnRemove, SIGNAL(clicked(QString)), this, SLOT(OnBtnClicked(QString)));
    }
}

CConferenceInfoFrame::~CConferenceInfoFrame()
{

}

// 设置CallID
void CConferenceInfoFrame::SetCallID(int iCallID)
{
    m_iCallID = iCallID;
}

void CConferenceInfoFrame::Clear()
{
    m_labContactName.clear();
    m_labImage.Clear();
    SetFarMuteStatus(false);
}

void CConferenceInfoFrame::SetContactName(const yl::string & strName)
{
    m_labContactName.setText(toQString(strName.c_str()));
}

void CConferenceInfoFrame::SetContactImage(const yl::string & strImagePath)
{
    m_labImage.SetImage(strImagePath);
}

void CConferenceInfoFrame::AddOrDeleteIcon(LPCSTR strIconPath, bool bAdd)
{
    m_labImage.AddOrDeleteIcon(strIconPath, bAdd);
}

//清除所有 ICON
void CConferenceInfoFrame::DeleteAllIcon()
{
    m_labImage.DeleteAllIcon();
}

void CConferenceInfoFrame::SetTipInfo(const QString & strTip, const QString & strPicPath)
{
    m_labImage.SetTipInfo(strTip, strPicPath);
}

void CConferenceInfoFrame::ClearTip()
{
    m_labImage.ClearTip();
}

void CConferenceInfoFrame::SetFarMuteStatus(bool bEnable)
{
    if (NULL == m_pBtnMute)
    {
        return;
    }

    if (bEnable)
    {
        m_pBtnMute->setBtnAction(TALK_UI_ACTION_UNMUTE);
        m_pBtnMute->set3StatePic(PIC_ICON_MUTE_PRESS, PIC_ICON_MUTE_PRESS, PIC_ICON_MUTE_PRESS);
        m_labImage.SetTipInfo(LANG_TRANSLATE(TRID_FAR_MUTE));
    }
    else
    {
        m_pBtnMute->setBtnAction(TALK_UI_ACTION_FARMUTE);
        m_pBtnMute->set3StatePic(PIC_ICON_TALK_IN_MUTE, PIC_ICON_TALK_IN_MUTE, PIC_ICON_TALK_IN_MUTE);
        m_labImage.ClearTip();
    }

    m_pBtnMute->update();
}

// 设置FarHold的状态
void CConferenceInfoFrame::SetFarHoldStatus(bool bEnable)
{
    if (NULL == m_pBtnFarHold)
    {
        return;
    }

    if (bEnable)
    {
        m_pBtnFarHold->setBtnAction(TALK_UI_ACTION_RESUME);
        m_pBtnFarHold->set3StatePic(PIC_ICON_FARHOLD_DOWN, PIC_ICON_FARHOLD_DOWN, PIC_ICON_FARHOLD_DOWN);
        //m_labImage.SetTipInfo(LANG_TRANSLATE(TRID_FARHOLD));
    }
    else
    {
        m_pBtnFarHold->setBtnAction(TALK_UI_ACTION_FARHOLD);
        m_pBtnFarHold->set3StatePic(PIC_ICON_FARHOLD, PIC_ICON_FARHOLD, PIC_ICON_FARHOLD);
        //m_labImage.ClearTip();
    }

    m_pBtnFarHold->update();
}

void CConferenceInfoFrame::AdjustLayout(bool bDisplayContactPhoto)
{
    if (bDisplayContactPhoto)
    {
        m_labContactName.setGeometry(LAB_CONFERENCE_CONTACT_NAME);
        m_pBtnMute->setGeometry(BTN_MUTE_GEOMETRY);
        m_pBtnFarHold->setGeometry(BTN_FARHOLD_GEOMETRY);
        m_pBtnRemove->setGeometry(BTN_REMOVE_GEOMETRY);
        m_labImage.show();
    }
    else
    {
        m_labContactName.setGeometry(LAB_CONFERENCE_CONTACT_NAME_NO_CANTACTIMAGE);
        m_pBtnMute->setGeometry(BTN_MUTE_GEOMETRY_NO_CANTACTIMAGE);
        m_pBtnFarHold->setGeometry(BTN_FARHOLD_GEOMETRY_NO_CANTACTIMAGE);
        m_pBtnRemove->setGeometry(BTN_REMOVE_GEOMETRY_NO_CANTACTIMAGE);
        m_labImage.hide();
    }
}

void CConferenceInfoFrame::OnBtnClicked(QString strBtnAction)
{
    TALKUI_INFO("emit BtnClicked[%d] [%s]", m_iCallID, strBtnAction.toUtf8().data());
    emit BtnClicked(this, strBtnAction);
}

#if IF_FEATURE_NFON_RTP
void CContactInfoFrame::StopAllScrollTimer()
{
    m_labContactName.StopScrollTimer();
    m_labContactInfo.StopScrollTimer();
    m_labContactNumber.StopScrollTimer();
}
#endif

void CContactInfoFrame::AdjustScrollTimer()
{
    m_labContactName.setScroolTimer(_TalkUIDelegate.GetScrollTimer());
    m_labContactNumber.setScroolTimer(_TalkUIDelegate.GetScrollTimer());
}

// 设置是否显示FarMute、FarHold
void CConferenceInfoFrame::SetFarMuteHoldShow(bool bShow)
{
    if (bShow)
    {
        m_pBtnFarHold->show();
        m_pBtnMute->show();
    }
    else
    {
        m_pBtnFarHold->hide();
        m_pBtnMute->hide();
    }
}
