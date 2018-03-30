#include "screensavers.h"
#include "screensaversuilogic/screensaverpainter.h"
#include "screensaversuilogic/screensavercontroller.h"
//#include "uimanager/rectdefine.h"
#include "baselibs/include/dsklog/log.h"
//#include "uikernel/languagehelper.h"
#include "settingui/include/modsettingui.h"
//#include "idlescreenui/modidlescreenui.h"
//#include "baseui/t4xcolordefine.h"

#include "xwindow/xControls.h"
#include "xwindow/xWindow.h"
#include "xwindow/xWindowManager.h"
#include "uimanager/basedialog.h"
#include "baseui/include/t2xpicpath.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include "idleuilogic/include/baseidlehandle.h"
#include "idleuilogic/include/uilogichelper.h"
#include "idlescreen/include/modidlescreen.h"
#include "xwindow/xSoftkeyBar.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "configparser/modconfigparser.h"
#include "configiddefine/configiddefine.h"
#include "idlescreenui/idleuicommon.h"
#include "idlescreenui/iconlist.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "animation/include/viewalphaanimation.h"
#include "animation/include/viewscaletoanimation.h"
#include "xwindow-kernel/include/xsignal.h"
#include "exp/include/modexp.h"
#include "modscreensavers.h"
#if IF_FEATURE_XMLB_SCREENSAVER || IS_COLOR


namespace
{
const char* SCREENSAVERS_UI_LAYOUT_FILE = "screensaver/screensavers.xml";
const char* SCREENSAVERS_NAME_FRAME_ALL = "frmall";
const char* SCREENSAVERS_NAME_FRAME_BG = "frmBG";
const char* SCREENSAVERS_NAME_LINEAR_TITLEICONS = "titleIcons";
const char* SCREENSAVERS_NAME_DATE_TIME = "clockLab";
const char* SCREENSAVERS_NAME_TEXT_LOADERRORS = "labelLoadErrors";
const char* SCREENSAVERS_NAME_SHADE = "shade";
//刚刚进入屏保时，时钟和状态图标整体的初始位置和尺寸定义
#define SCREENSAVER_RECT_LEFT               120
#define SCREENSAVER_RECT_TOP                90
#define SCREENSAVER_RECT_WIDTH              240
#define SCREENSAVER_RECT_HEIGHT             159

#if IS_COLOR
#define SCREENSAVER_DEFAULT_BACKGROUND_COLOR        xColor(0x2B, 0x2B, 0x30)
#else
#define SCREENSAVER_DEFAULT_BACKGROUND_COLOR        xColor(255, 255, 255)
#endif

#define SCREENSAVER_LOADERROR_TEXT_COLOR            COMMON_FONT_COLOR /*??????*/
#define SCREENSAVER_LOADERROR_TEXT_FONT_SIZE        34

#define SCREENSAVER_CLOCK_HM_FONT_SIZE              60
#define SCREENSAVER_CLOCK_AMPM_FONT_SIZE            20
#define SCREENSAVER_CLOCK_DATE_FONT_SIZE            20
#define SCREENSAVER_CLOCK_HM_AND_DATE_INTTERVAL     15

//产品部确认，图片切换渐变使用黑色颜色
#define SCREENSAVER_COLOR_BLACK                 xColor(0, 0, 0)

//利用ilde模块函数的配置
#define TITLE_STATUS_ICON_PADDING   2
#define TITLE_STATUS_ICON_FLASHING_SHOW_TIME    1500
#define TITLE_STATUS_ICON_FLASHING_HIDE_TIME    1500
}

CDlgScreenSavers::CDlgScreenSavers()
    : m_pFrmBG(NULL)
    , m_pClockLab(NULL)
    , m_pTitleIcons(NULL)
    , m_pLabelLoadError(NULL)
    , m_corScreenBlack(SCREENSAVER_COLOR_BLACK)
    , m_pShade(NULL)
    , m_strBGPic("")
    , m_pbrushBG()
    , m_pXMLTextScreen(NULL)
    , m_pAnimation(NULL)
    , m_bAlphaUp(true)
    , m_bIsEXPExitSync(true)
{

    ////////设置CreatWindow层////////////
    m_pWindow = m_pHostStation->createWindow(this, TML_SYSTEM);
    this->loadContent(SCREENSAVERS_UI_LAYOUT_FILE);

    InitWigdet();

}

CDlgScreenSavers::~CDlgScreenSavers()
{
}

CDlgScreenSavers* CDlgScreenSavers::GetInstance()
{
    static CDlgScreenSavers p_CDlgScreenSavers ;

    return &p_CDlgScreenSavers;
}


void CDlgScreenSavers::SetScreenSaversVisible(bool bVisible)
{
    if (bVisible)
    {
        //UIManager_GetTopWindow()->hide();
        //xml屏保时this和m_pXMLTextScreen同时显示
        if (g_ScreenSaverController.GetSSPicType() != SCREENSAVER_MODE_CHANGE_TYPE_XML)
        {
            OnStatusChange();
            currentSoftKeyBar().hide();
            this->show();
        }
    }
    else
    {
        //UIManager_GetTopWindow()->show();
        currentSoftKeyBar().show();
        this->hide();

    }
}


void CDlgScreenSavers::OnXMLScreenSavers(bool bEnter)
{
    if (bEnter)
    {
        XMLLogicToUIData objData;
        bool bRefresh = xmlbrowser_GetSSData(objData);
        XBScreenSaverData * pSSData = static_cast<XBScreenSaverData *>(objData.pElementData);
        if (bRefresh && pSSData != NULL)
        {
            if (pSSData->listImage.size() != 0)
            {
                ImageItem * pImageItem = pSSData->listImage.front();
                if (pImageItem != NULL)
                {
                    switch (pImageItem->eLoadStatus)
                    {
                    case ILS_DOWNLOAD:
                    case ILS_COMPRESS:
                        {
                            //下载和解压则返回不处理等下载完成再统一加载界面内容
                            return;
                        }
                    case ILS_LOAD_OK:
                        {
                        }
                    default:
                        // 其他情况则接下去处理，不加载图片

                        break;
                    }
                }
            }
            if (m_pXMLTextScreen == NULL)
            {
                m_pXMLTextScreen = new CUIRegionFormatText(true);
            }
            if (m_pXMLTextScreen == NULL)
            {
                return;
            }
            addChildView(m_pXMLTextScreen);
            //刷新Region的数据
            m_pXMLTextScreen->SetElementID(objData.nElementID);
            m_pXMLTextScreen->Update(pSSData);
            m_pXMLTextScreen->UpdateContent();
            currentSoftKeyBar().hide();
            OnShow();
            m_pXMLTextScreen->show();
            OnStatusChange();
        }
        else
        {
            yl::string strURL = configParser_GetConfigString(kszXmlBrowserScreenSaverURL);
            if (!strURL.empty())
            {
                xmlbrowser_ScreenSaverEnter(strURL.c_str());
                return;
            }
            else
            {
#if IS_COLOR
                ////将屏保模式设置为system模式
                configParser_SetConfigInt(kszScreenType, 0, CONFIG_LEVEL_PHONE);
                etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_SCREEN_SAVER_MODE, 0);
#else
                ScreenSavers_Exit();
#endif
            }
        }
    }
    else
    {
        xmlbrowser_DeleteSSData();
        if (m_pXMLTextScreen != NULL)
        {
            m_pXMLTextScreen->hide();
            popupChildView(m_pXMLTextScreen);
            delete m_pXMLTextScreen;
            m_pXMLTextScreen = NULL;
        }
    }
}

#if IS_COLOR
void CDlgScreenSavers::StartAnimation(bool bAlphaUp)
{
    m_bAlphaUp = bAlphaUp;
    StopAnimation();

    m_pAnimation = new CSequentialAnimationGroup;

    if (m_pAnimation == NULL)
    {
        return;
    }

    CViewAlphaAnimation* pAlphaAnimation = new CViewAlphaAnimation(g_WorkStation.getStationAlpha());

    if (NULL != pAlphaAnimation)
    {
        pAlphaAnimation->SetView(m_pShade);
        //pAlphaAnimation->SetViewColor(xColor(0, 0, 0));
        if (bAlphaUp)
        {
            pAlphaAnimation->SetStartAlpha(0);
            pAlphaAnimation->SetEndAlpha(255);
        }
        else
        {
            pAlphaAnimation->SetStartAlpha(255);
            pAlphaAnimation->SetEndAlpha(0);
        }
        pAlphaAnimation->SetDuration(3000);
        pAlphaAnimation->SetInterval(300);

        m_pAnimation->AddAnimation(pAlphaAnimation);
    }

    SLOT_CONNECT(m_pAnimation, signalStateChanged, this,  CDlgScreenSavers::OnAnimationStateChanged);


    m_pAnimation->Start();
}


void CDlgScreenSavers::StopAnimation()
{
    if (NULL != m_pAnimation)
    {
        m_pAnimation->Stop();
        m_pAnimation = NULL;
    }
}

void CDlgScreenSavers::OnAnimationStateChanged(xSignal* sender, WPARAM wParam, LPARAM lParam,
        int nDataBytes, LPVOID pData)
{
    CAnimationBase* pAnimation = (CAnimationBase*)wParam;

    if (NULL == m_pAnimation || pAnimation != m_pAnimation || lParam != CAnimationBase::ANIMATION_STATE_STOPPED)
    {
        return;
    }

    m_pAnimation = NULL;
    if (m_bAlphaUp)
    {
        g_ScreenSaverPainter.OnReallyChangeBG();
        StartAnimation(false);
    }
    else
    {
        g_ScreenSaverPainter.OnBGChangFinish();
    }
}
#endif

void CDlgScreenSavers::InitWigdet()
{
    m_pFrmBG = static_cast<xFrameLayout*>(getViewById(SCREENSAVERS_NAME_FRAME_BG));
    m_pClockLab = static_cast<xDateTimeView*>(getViewById(SCREENSAVERS_NAME_DATE_TIME));
    m_pTitleIcons = static_cast<xLinearLayout*>(getViewById(SCREENSAVERS_NAME_LINEAR_TITLEICONS));
    m_pLabelLoadError = static_cast<xTextView*>(getViewById(SCREENSAVERS_NAME_TEXT_LOADERRORS));
    m_pShade = static_cast<xImageView*>(getViewById(SCREENSAVERS_NAME_SHADE));

    m_pFrmBG->hide();

    OnClockFormatChange();
}

void CDlgScreenSavers::OnInit()
{
    g_ScreenSaverPainter.BlindScreenSaversUI(this);
    g_ScreenSaverController.CheckXMLScreenSavers();
    exp_EnterExpScreenSaver(true);
}

void CDlgScreenSavers::OnUnInit()
{
    OnXMLScreenSavers(false);
    g_ScreenSaverPainter.UnBlindScreenSaversUI();
    //如果节电开启，则无论如何都同步退出
    if ((configParser_GetConfigInt(kszPowerSavingEnable) == 0) && !m_bIsEXPExitSync)
    {
        //节电模式关闭并且不是同步退出的时候不退出屏保
    }
    else
    {
        exp_EnterExpScreenSaver(false);
    }

    m_bIsEXPExitSync = true;
}

void CDlgScreenSavers::OnShow()
{
    this->show();
}

void CDlgScreenSavers::OnHide()
{
    this->hide();
}

void CDlgScreenSavers::OnDeleteLater()
{
    /*this->setParent(NULL);
    this->deleteLater();*/
}

xBrush CDlgScreenSavers::GetBackGroundBrush()
{
    IDLEUI_INFO("ScreenSavers Get BackGround Pic = %s", m_strBGPic.c_str());

    xBrush picBG_Brush;
    xPixmap picBG;


    if (!m_strBGPic.empty())
    {
        picBG.load(m_strBGPic.c_str()); //= STOCK_GET_BMP(strBGPic.c_str());

    }

    if (m_strBGPic.empty() || picBG.isNull())
    {
        IDLEUI_INFO("Dlg ScreenSavers GetCustomBackGround, return default color BG");

        //m_pLabelLoadError->show();
        //m_pClockLab->hide();
        //m_pTitleIcons->hide();

        xPixmap picDefayltBG_Pixmap;
        picDefayltBG_Pixmap.Create(this->layout_width, this->layout_height);
        xBrush picDefayltBG(picDefayltBG_Pixmap);
        picDefayltBG.setColor(SCREENSAVER_DEFAULT_BACKGROUND_COLOR);

        return picDefayltBG;
    }
    else
    {
        //m_pLabelLoadError->hide();
        //m_pClockLab->show();
        //m_pTitleIcons->show();

        if (picBG.width() != this->layout_width || picBG.height() != this->layout_height)
        {
            IDLEUI_INFO("pic size is not fetch phone");
            /*xPixmap pic_Scale;
            pic_Scale.Create(this->layout_width, this->layout_height);
            picBG.ScaleCopy(pic_Scale);*/

        }
        picBG_Brush.setPixmap(picBG);
        return picBG_Brush;
    }
}

//根据状态变化改变图标栏
void CDlgScreenSavers::OnStatusChange(YLList<IdleStatusItem>& StatusList)
{
    YLList<IdleStatusItem>::ListIterator iter = StatusList.begin();
    CBaseIdleHandle* pBaseIdleHandle = NULL;
    IdleIconInfo iconInfo;
    YLVector<IdleIconInfo> IconList;
    for (; iter != StatusList.end(); ++iter)
    {
        IdleStatusItem& StatusItem = (*iter);
        iconInfo.m_strHint = StatusItem.m_strHint;
        pBaseIdleHandle = GET_IDLE_HANDLE(StatusItem.m_nId);
        if (NULL != pBaseIdleHandle)
        {
            pBaseIdleHandle->GetIconInfo(iconInfo);
        }
        IconList.push_back(iconInfo);
    }
    UpdateIcon(IconList);
}


void CDlgScreenSavers::OnStatusChange()
{
    YLList<IdleStatusItem> ylIdleStatus = idleScreen_GetStatusList(SNT_ICON);
    OnStatusChange(ylIdleStatus);
}

void CDlgScreenSavers::OnClockFormatChange()
{
    if (NULL != m_pClockLab)
    {
        m_pClockLab->Init();
        m_pClockLab->SetTimeFormat(SettingUILogic_GetCurTimeFormatType());
        m_pClockLab->SetDateFormat(SettingUILogic_GetCurDateFormatType());
    }
}

//时钟移动
void CDlgScreenSavers::OnMoveClock(int nLeft, int nTop)
{
    IDLEUI_INFO("ScreenSavers OnMoveClock, nLeft = %d, nTop = %d", nLeft, nTop);
    m_pFrmBG->layout_marginTop = nTop;
    m_pFrmBG->layout_marginLeft = nLeft;

    this->onChildrenLayoutChanged();
    this->update(true);
}

void CDlgScreenSavers::OnGetClockAndScreenRect(int &nClockWidth, int &nClockHeight, int &nScreenWidth, int &nScreenHeight)
{
    nClockWidth = SCREENSAVER_RECT_WIDTH;
    nClockHeight = SCREENSAVER_RECT_HEIGHT;
    nScreenWidth = this->layout_width;
    nScreenHeight = this->layout_height;

}

//刷新界面
void CDlgScreenSavers::OnUpdateWnd(ScreenUiLogicData & ScreenData)
{
    m_UiLogicData = ScreenData;

#if IS_COLOR
    if (m_UiLogicData.strBGPic != m_strBGPic)
#endif
    {
        m_strBGPic = m_UiLogicData.strBGPic;
        m_pbrushBG = GetBackGroundBrush();
    }

    // 如果节电状态，则绘制黑色图片
    if (m_UiLogicData.bPowerSaving)
    {
        xColor rbg(m_UiLogicData.uBgColorRgb, m_UiLogicData.uBgColorRgb, m_UiLogicData.uBgColorRgb);
        m_corScreenBlack = rbg;
        m_pShade->setBackground(m_corScreenBlack);
        m_pShade->update(true);

    }
    else
    {
        this->background = m_pbrushBG;

        m_pShade->update(true);
    }


    this->update(true);

}

void CDlgScreenSavers::OnShowClock(bool bshow)
{
    if (bshow)
    {
        m_pFrmBG->show();


    }
    else
    {
        m_pFrmBG->hide();
    }
}


//////////////T2X的idle界面Title状态变化方法///////////////////////////////////////


bool CDlgScreenSavers::addTitleIcon(const yl::string & strFileName, bool bFlashIcon, xLinearLayout* pTitleIcons)
{
    if (NULL == pTitleIcons)
    {
        return false;
    }

    xImageView* pImage = static_cast<xImageView*>(pTitleIcons->getViewById(strFileName));

    if (NULL != pImage)
    {
        IDLEUI_INFO("addTitleIcon file [%s] already extist", strFileName.c_str());
        return false;
    }

    IDLEUI_INFO("addTitleIcon file [%s]", strFileName.c_str());

    pImage = new xImageView();
    pImage->layout_width = LayoutParams::wrap_content;
    pImage->layout_height = LayoutParams::wrap_content;
    pImage->setPixmap(strFileName);

    if (bFlashIcon)
    {
        xFlashingBox* pFlashBox = new xFlashingBox();
        pFlashBox->m_strId = strFileName;
        pFlashBox->layout_width = LayoutParams::wrap_content;
        pFlashBox->layout_height = LayoutParams::wrap_content;
        pFlashBox->paddingLeft = TITLE_STATUS_ICON_PADDING;
        pFlashBox->addChildView(pImage);

        FrameLayoutParams* layoutInParent = FrameLayoutParams::GetLayoutParams(pImage->m_pLayoutInParent);
        if (layoutInParent != NULL)
        {
            layoutInParent->layout_gravity = gravity_center;
        }


        pTitleIcons->addChildView(pFlashBox);

        pFlashBox->setupFlashTime(TITLE_STATUS_ICON_FLASHING_SHOW_TIME, TITLE_STATUS_ICON_FLASHING_HIDE_TIME);
    }
    else
    {
        pImage->m_strId = strFileName;
        pImage->paddingLeft = TITLE_STATUS_ICON_PADDING;
        pTitleIcons->addChildView(pImage);
    }
    pTitleIcons->show();
    return true;
}

void CDlgScreenSavers::UpdateIcon(const YLVector<IdleIconInfo>& Statuslist)
{
    ClearIcon();
    for (int i = 0; i < Statuslist.size(); ++i)
    {
        const IdleIconInfo& iconInfo = Statuslist[i];
        yl::string strIcon;
        //if (/*IconType2PicPath((ICON_TYPE)iconInfo.m_eIconType, strIcon)*/)
        //{
        if (m_pTitleIcons != NULL)
        {
            g_pIconList->AddIcon(iconInfo.m_eIconType, iconInfo.m_bFlash, iconInfo.m_strHint, m_pTitleIcons);
            //addTitleIcon(strIcon, iconInfo.m_bFlash, m_pTitleIcons);
        }
        if (m_pXMLTextScreen != NULL && m_pXMLTextScreen->GetTitleIcon() != NULL)
        {
            g_pIconList->AddIcon(iconInfo.m_eIconType, iconInfo.m_bFlash, iconInfo.m_strHint, m_pXMLTextScreen->GetTitleIcon());
            //addTitleIcon(strIcon, iconInfo.m_bFlash, m_pXMLTextScreen->GetTitleIcon());
        }
        //}
    }
}

bool CDlgScreenSavers::ClearIcon()
{
    if (NULL == m_pTitleIcons)
    {
        return false;
    }


    m_pTitleIcons->closeAllChildren();

    if (m_pXMLTextScreen != NULL && m_pXMLTextScreen->GetTitleIcon() != NULL)
    {
        m_pXMLTextScreen->GetTitleIcon()->closeAllChildren();
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////
#endif //IF_FEATURE_XMLB_SCREENSAVER || IS_COLOR
