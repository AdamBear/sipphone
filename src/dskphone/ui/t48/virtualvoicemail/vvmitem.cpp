#include "vvmitem.h"

#include "baseui/t4xpicpath.h"
#include "uicommon/imagemanager/modimagemanager.h"
#include "uikernel/languagehelper.h"
#include "include/translateiddefine.h"

#include "qtcommon/qmisc.h"
#include "record/include/modrecord.h"
#include "setting/include/modsetting.h"

namespace
{
#define VOICEMAIL_ITEM_INFO_COUNT                               2
#define VOICEMAIL_ITEM_BUTTONS_COUNT                        2

#define VOICEMAIL_ITEM_BUTTON_LEFT_INDEX                    0
#define VOICEMAIL_ITEM_BUTTON_RIGHT_INDEX               1

#define VOICEMAIL_ITEM_HINT_TIME                                    1000

static const QString strSlash(" / ");

//左右两个按钮的实际点击区域
static const QRect rctLeftButtonRespond(0, 0, 55, 50);
static const QRect rctRightButtonRespond(422, 0, 60, 50);

//左右两个按钮的图片的位置（小于实际点击区域）
static const QRect rctLeftButton(20, 10, 30, 30);
static const QRect rctRightButton(445, 10, 30, 30);

//进度条位置
#define ITEM_PROCESS_BAR_WIDTH      380
#define ITEM_PROCESS_BAR_HEIGHT 20
static const QRect rctProcessBar(60, 7, ITEM_PROCESS_BAR_WIDTH, ITEM_PROCESS_BAR_HEIGHT);

//紧急信息位置1
static const QRect rctEmergency1(67, 6, 8, 20);
//紧急信息位置2
static const QRect rctEmergency2(73, 6, 8, 20);

//发件名称位置
static const QRect rctNameLabel(67, 7, 220, 20);
static const QRect rctNameLabelMove1(75, 7, 212, 20);
static const QRect rctNameLabelMove2(83, 7, 204, 20);

//长度信息位置
static const QRect rctDurationLabel(67, 29, 100, 20);

//接收时间信息位置
static const QRect rctReceiveDateLabel(303, 7, 130, 40);
static const QRect rctReceiveTimeLabel(303, 29, 130, 40);

static const QRect rctPlayTime(283, 29, 180, 20);

//暂停提示图标位置
static const QRect rctHintIcon(255, 29, 12, 18);

//提示文字位置
static const QRect rctHintLabel(223, 29, 80, 20);
}

CVVMItem::CVVMItem(QWidget * pParent, QWidgetPair & pair)
    : CSettingItem(pParent, pair)
    , m_pProgressBar(NULL)
    , m_bMoving(false)
    , m_btnLeft(this)
    , m_btnRight(this)
/////////////////////////
    , m_strEmergency("")
    , m_strConfidential("")
    , m_bRead(false)
/////////////////////////
    , m_strName("")
    , m_strDuration("")
    , m_strReceiveDate("")
    , m_strReceiveTime("")
/////////////////////////
    , m_pPlayCurrentTime(NULL)
    , m_pPlayHintIcon(NULL)
/////////////////////////
    , m_bShowInfo(false)
    , m_bShowNote(false)
    , m_eNoteType(VVM_ITEM_NOTE_TYPE_ERROR)
{
    InitData();
    InitController();
}

CVVMItem::~CVVMItem()
{
}

void CVVMItem::InitData()
{
    m_pProgressBar = new CProcessBarUtility(this);
    if (NULL == m_pProgressBar)
    {
        return;
    }
    m_pProgressBar->hide();
    m_pProgressBar->setAttribute(Qt::WA_TranslucentBackground, true);
    m_pProgressBar->SetImage(CProcessBarUtility::PCE_PROGRESSBAR, PIC_USB_AUDIO_PROCESSBAR_EMPTY,
                             PIC_USB_AUDIO_PROCESSBAR_FULL, PIC_USB_AUDIO_PROGRESSBAR_CURPOS);
    //设置焦点代理
    m_pProgressBar->setFocusProxy(this);
    m_pProgressBar->SetValue(FILE_PLAYER_EMPTY_PERCENT);
    //默认情况设置有效值
    m_pProgressBar->SetRange(FILE_PLAYER_EMPTY_PERCENT, FILE_PLAYER_FULL_PERCENT);
    //设置步长
    m_pProgressBar->SetStep(1);
    m_pProgressBar->SetSliderWidth(ITEM_PROCESS_BAR_WIDTH);
    m_pProgressBar->SetSliderHeight(ITEM_PROCESS_BAR_HEIGHT);

    m_pPlayCurrentTime = new QLabel(this);
    m_pPlayHintIcon = new QLabel(this);

    if (NULL != m_pPlayCurrentTime
            && NULL != m_pPlayHintIcon)
    {
        m_pPlayCurrentTime->hide();
        m_pPlayHintIcon->hide();

        m_pPlayHintIcon->setPixmap(THEME_GET_BMP(PIC_USB_BROWSE_AUDIO_HINT_PAUSE));
    }

#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
    // 设置是否有选中效果
    SetChangeStyleOnSelect(false);
#endif
#ifdef  QT_TOUCH_SUPPORT
    // 设置点击效果
    SetChangeStyleOnPress(true);

    SetClickBg(PIC_GENERAL_LIST_ITEM_CLICK_BG);
#endif

    Relayout();
}

void CVVMItem::InitController()
{
    connect(&g_VVMController, SIGNAL(DialogRefreshPlayInfo()), this, SLOT(OnItemRefreshPlayProgress()));
    connect(&g_VVMController, SIGNAL(PlayStateChange()), this, SLOT(OnItemRefreshPlayProgress()));
    connect(&g_VVMController, SIGNAL(FIlePlayStart()), this, SLOT(OnFilePlayStart()));
    connect(&g_VVMController, SIGNAL(FIlePlayStop()), this, SLOT(OnFilePlayStopped()));
    connect(&g_VVMController, SIGNAL(FilePlayFinish()), this, SLOT(OnFilePlayFinish()));

    connect(this, SIGNAL(DialogAction(QString)), &g_VVMController, SLOT(OnPlayerAction(QString)));
    connect(this, SIGNAL(DialogAction(QString, QVariant)), &g_VVMController,
            SLOT(OnPlayerAction(QString, QVariant)));
    connect(this, SIGNAL(NormallyExit()), &g_VVMController, SLOT(OnFilePlayFinish()));

    if (NULL != m_pProgressBar)
    {
        connect(m_pProgressBar, SIGNAL(mouseAction(int)), this, SLOT(OnProgressBarAction(int)));
    }

    m_HintHideTimer.setParent(this);
    StopHideTime();
    connect(&m_HintHideTimer, SIGNAL(timeout()), this, SLOT(OnHideTimerOut()));
}

void CVVMItem::ResetItem()
{
    m_bMoving = false;

    m_bShowInfo = true;

    m_bShowNote = false;
    m_eNoteType = VVM_ITEM_NOTE_TYPE_ERROR;

    if (NULL != m_pProgressBar)
    {
        m_pProgressBar->hide();
    }

    if (NULL != m_pPlayCurrentTime
            && NULL != m_pPlayHintIcon)
    {
        m_pPlayCurrentTime->hide();
        m_pPlayHintIcon->hide();
    }

    update();
}

void CVVMItem::LoadItemData()
{
    OnItemRefreshPlayProgress();
    ReloadItemData();
}

void CVVMItem::ReloadItemData()
{
    bool bGetPairResult = false;
    const vecVVMItemPair & vecPlayData = g_VVMController.GetPlayInfo(bGetPairResult, GetDataIndex());
    if (!bGetPairResult)
    {
        return;
    }

    if (vecPlayData.size() < VOICEMAIL_ITEM_INFO_COUNT)
    {
        return;
    }

    if (VVM_FILE_READ == vecPlayData[0].m_strAction)
    {
        m_bRead = true;
    }
    else
    {
        m_bRead = false;
    }

    m_strEmergency = vecPlayData[0].m_strTitle1;
    m_strConfidential = vecPlayData[0].m_strTitle2;

    m_strName = vecPlayData[1].m_strTitle1;
    m_strDuration = vecPlayData[1].m_strTitle2;

    m_strReceiveDate = vecPlayData[2].m_strTitle1;
    m_strReceiveTime = vecPlayData[2].m_strTitle2;
}

void CVVMItem::OnItemRefreshPlayProgress()
{
    FILEPLAYER_STATE ePlayerState = g_VVMController.GetCurrentPlayState();
    int nCurrentFucosIndex = g_VVMController.GetCurrentFocusFileIndex();

    m_PlayerData = g_VVMController.GetPlayerData();

    bool bGetPairResult = false;
    m_vecPlayButtonsData = g_VVMController.GetPlayButtons(bGetPairResult, GetDataIndex());

    if (NULL == m_pProgressBar
            || NULL == m_pPlayCurrentTime)
    {
        return;
    }

    if (nCurrentFucosIndex == GetDataIndex()
            && (FILEPLAYER_STATE_PLAY == ePlayerState
                || FILEPLAYER_STATE_PAUSE == ePlayerState))
    {
        m_bShowInfo = false;

        if (!m_bMoving)
        {
            m_pProgressBar->show();
            m_pProgressBar->raise();
            if (0 != m_PlayerData.m_lTotalSeconds)
            {
                m_pProgressBar->SetValue(m_PlayerData.m_lCurrentSeconds * FILE_PLAYER_FULL_PERCENT /
                                         m_PlayerData.m_lTotalSeconds);
            }
            else
            {
                m_pProgressBar->SetValue(0);
            }

            QString strPlayTime = toQString(modRecord_TransTimeToString(m_PlayerData.m_lCurrentSeconds));
            strPlayTime += strSlash;
            strPlayTime += m_strDuration;
            m_pPlayCurrentTime->setText(strPlayTime);
            m_pPlayCurrentTime->show();
            m_pPlayCurrentTime->raise();

            if (FILEPLAYER_STATE_PAUSE == ePlayerState)
            {
                m_pPlayHintIcon->show();
            }
            else
            {
                m_pPlayHintIcon->hide();
            }
        }
    }
    else
    {
        m_bShowInfo = true;
        m_pProgressBar->hide();
        m_pPlayCurrentTime->hide();
        m_pPlayHintIcon->hide();
    }

    update();
}

void CVVMItem::Relayout()
{
    if (NULL != m_pProgressBar)
    {
        m_pProgressBar->setGeometry(rctProcessBar);
    }

    if (NULL != m_pPlayCurrentTime)
    {
        m_pPlayCurrentTime->setGeometry(rctPlayTime);
    }

    if (NULL != m_pPlayHintIcon)
    {
        m_pPlayHintIcon->setGeometry(rctHintIcon);
    }

    m_btnLeft.SetRect(rctLeftButtonRespond);

    m_btnRight.SetRect(rctRightButtonRespond);
}

bool CVVMItem::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL == pObject || NULL == pEvent)
    {
        return false;
    }

    if (NULL != m_pProgressBar)
    {
        m_pProgressBar->eventFilter(pObject, pEvent);
    }

    return CSettingItem::eventFilter(pObject, pEvent);
}

void CVVMItem::mousePressEvent(QMouseEvent * pEvent)
{
    if (NULL == pEvent)
    {
        return;
    }

    //左边按钮判断
    if (m_btnLeft.ProcessMousePress(pEvent))
    {
        return;
    }

    //右边按钮判断
    if (m_btnRight.ProcessMousePress(pEvent))
    {
        return;
    }

    FILEPLAYER_STATE ePlayerState = g_VVMController.GetCurrentPlayState();
    int nCurrentFucosIndex = g_VVMController.GetCurrentFocusFileIndex();

    if (nCurrentFucosIndex == GetDataIndex()
            && (FILEPLAYER_STATE_PLAY == ePlayerState
                || FILEPLAYER_STATE_PAUSE == ePlayerState))
    {
        return;
    }

    //item判断
    ProcessMousePress(pEvent);
}

void CVVMItem::mouseReleaseEvent(QMouseEvent * pEvent)
{
    if (NULL == pEvent)
    {
        return;
    }

    //左边按钮判断
    if (m_btnLeft.ProcessMouseRelease(pEvent))
    {
        OnButtonsClicked(VOICEMAIL_ITEM_BUTTON_LEFT_INDEX);
        return;
    }

    //右边按钮判断
    if (m_btnRight.ProcessMouseRelease(pEvent))
    {
        OnButtonsClicked(VOICEMAIL_ITEM_BUTTON_RIGHT_INDEX);
        return;
    }

    FILEPLAYER_STATE ePlayerState = g_VVMController.GetCurrentPlayState();
    int nCurrentFucosIndex = g_VVMController.GetCurrentFocusFileIndex();

    if (nCurrentFucosIndex == GetDataIndex()
            && (FILEPLAYER_STATE_PLAY == ePlayerState
                || FILEPLAYER_STATE_PAUSE == ePlayerState))
    {
        return;
    }

    //item判断
    ProcessMouseRelease(pEvent);
}

void CVVMItem::OnButtonsClicked(int nButtonIndex)
{
    bool bGetPairResult = false;
    const vecVVMItemPair & vecPlayButtonsData = g_VVMController.GetPlayButtons(bGetPairResult,
            GetDataIndex());
    if (!bGetPairResult)
    {
        return;
    }

    if (vecPlayButtonsData.size() < VOICEMAIL_ITEM_BUTTONS_COUNT
            || vecPlayButtonsData.size() < nButtonIndex)
    {
        return;
    }

    const QString & strAction = vecPlayButtonsData[nButtonIndex].m_strAction;

    if ("" == strAction)
    {
        return;
    }
    else
    {
        emit DialogAction(strAction, QVariant::fromValue<int>(GetDataIndex()));
    }
}

void CVVMItem::OnProgressBarAction(int eMouseAction)
{
    if (QEvent::MouseButtonPress == eMouseAction)
    {
        m_bMoving = true;
        QString strAction = FILEPLAYER_ACTION_SEEK_PERCENT_START;
        if (NULL != m_pProgressBar)
        {
            emit DialogAction(strAction, QVariant::fromValue<int>(m_pProgressBar->GetValue()));
        }
    }
//  else if (QEvent::MouseMove == eMouseAction)
//  {
//      strAction = FILEPLAYER_ACTION_SEEK_PERCENT_MOVE;
//      if (NULL != m_pProgressBar)
//      {
//          emit DialogAction(strAction, QVariant::fromValue<int>(m_pProgressBar->GetValue()));
//      }
//      return;
//  }
    else if (QEvent::MouseButtonRelease == eMouseAction)
    {
        m_bMoving = false;
        QString strAction = FILEPLAYER_ACTION_SEEK_PERCENT_OVER;
        if (NULL != m_pProgressBar)
        {
            emit DialogAction(strAction, QVariant::fromValue<int>(m_pProgressBar->GetValue()));
        }
    }
}

void CVVMItem::OnFilePlayStart()
{
    m_bMoving = false;
    int nCurrentFucosIndex = g_VVMController.GetCurrentFocusFileIndex();

    //如果开始播放，就隐藏info
    if (nCurrentFucosIndex == GetDataIndex())
    {
        m_bShowInfo = false;
    }

    OnItemRefreshPlayProgress();
}

void CVVMItem::OnFilePlayStopped()
{
    m_bMoving = false;
    int nCurrentFucosIndex = g_VVMController.GetCurrentFocusFileIndex();

    //如果停止了，要显示stop提示
    if (!m_bShowNote && nCurrentFucosIndex == GetDataIndex())
    {
        m_bShowNote = true;
        m_eNoteType = VVM_ITEM_NOTE_TYPE_STOPPED;

        //如果被停止，标记为已读
        m_bRead = true;

        StartHideTime();
    }

    OnItemRefreshPlayProgress();
}

void CVVMItem::OnFilePlayFinish()
{
    m_bMoving = false;
    int nCurrentFucosIndex = g_VVMController.GetCurrentFocusFileIndex();

    //如果当前播放完成，则显示finish提示
    if (!m_bShowNote && nCurrentFucosIndex == GetDataIndex())
    {
        m_bRead = true;
        m_bShowNote = true;
        m_eNoteType = VVM_ITEM_NOTE_TYPE_FINISH;
        StartHideTime();
        update();
    }

    OnItemRefreshPlayProgress();
}

void CVVMItem::StartHideTime()
{
    StopHideTime();
    m_HintHideTimer.start(VOICEMAIL_ITEM_HINT_TIME);
}

void CVVMItem::StopHideTime()
{
    if (m_HintHideTimer.isActive())
    {
        m_HintHideTimer.stop();
    }
}

void CVVMItem::OnHideTimerOut()
{
    m_bShowNote = false;
    m_eNoteType = VVM_ITEM_NOTE_TYPE_ERROR;
    update();
}

void CVVMItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    PaintDownBg(stylePainter, m_strClickBg.empty() ? NULL : m_strClickBg.c_str());

    if (m_vecPlayButtonsData.size() >= VOICEMAIL_ITEM_BUTTONS_COUNT)
    {
        if (!m_vecPlayButtonsData[0].m_strTitle1.isEmpty()
                && !m_vecPlayButtonsData[0].m_strTitle2.isEmpty())
        {
            m_btnLeft.PaintButton(stylePainter
                                  , m_vecPlayButtonsData[0].m_strTitle1.toUtf8().data()
                                  , m_vecPlayButtonsData[0].m_strTitle2.toUtf8().data()
                                  , "", rctLeftButton);
        }
        if (!m_vecPlayButtonsData[1].m_strTitle1.isEmpty()
                && !m_vecPlayButtonsData[1].m_strTitle2.isEmpty())
        {
            m_btnRight.PaintButton(stylePainter
                                   , m_vecPlayButtonsData[1].m_strTitle1.toUtf8().data()
                                   , m_vecPlayButtonsData[1].m_strTitle2.toUtf8().data()
                                   , "", rctRightButton);
        }
    }

    if (m_bShowInfo)
    {
        QPen penNormal = stylePainter.pen();
        QPen penRed = penNormal;
        penRed.setColor(FILE_PLAYER_COLOR_RED);
        stylePainter.setPen(penRed);

        QPen penName = penNormal;
        if (m_bRead)
        {
            penName.setColor(FILE_PLAYER_COLOR_BLACK);
        }
        else
        {
            penName.setColor(FILE_PLAYER_COLOR_BLUE);
        }

        if ("" == m_strEmergency && "" == m_strConfidential)
        {
            stylePainter.setPen(penName);
            stylePainter.drawText(rctNameLabel
                                  , Qt::AlignLeft & Qt::AlignHCenter
                                  , CutOffText(LANG_PARSE(m_strName), stylePainter.fontMetrics(), rctNameLabel.width() - 3));
        }
        else if ("" != m_strEmergency && "" != m_strConfidential)
        {
            stylePainter.drawText(rctEmergency1, Qt::AlignLeft & Qt::AlignHCenter, m_strEmergency);
            stylePainter.drawText(rctEmergency2, Qt::AlignLeft & Qt::AlignHCenter, m_strConfidential);

            stylePainter.setPen(penName);
            stylePainter.drawText(rctNameLabelMove2
                                  , Qt::AlignLeft & Qt::AlignHCenter
                                  , CutOffText(LANG_PARSE(m_strName), stylePainter.fontMetrics(), rctNameLabelMove2.width() - 3));
        }
        else
        {
            if ("" != m_strEmergency && "" == m_strConfidential)
            {
                stylePainter.drawText(rctEmergency1, Qt::AlignLeft & Qt::AlignHCenter, m_strEmergency);
            }
            else if ("" == m_strEmergency && "" != m_strConfidential)
            {
                stylePainter.drawText(rctEmergency1, Qt::AlignLeft & Qt::AlignHCenter, m_strConfidential);
            }

            stylePainter.setPen(penName);
            stylePainter.drawText(rctNameLabelMove1
                                  , Qt::AlignLeft & Qt::AlignHCenter
                                  , CutOffText(LANG_PARSE(m_strName), stylePainter.fontMetrics(), rctNameLabelMove1.width() - 3));
        }

        stylePainter.setPen(penNormal);

        stylePainter.drawText(rctDurationLabel, Qt::AlignLeft & Qt::AlignHCenter,
                              LANG_PARSE(m_strDuration));
        stylePainter.drawText(rctReceiveDateLabel, Qt::AlignLeft & Qt::AlignHCenter,
                              LANG_PARSE(m_strReceiveDate));
        stylePainter.drawText(rctReceiveTimeLabel, Qt::AlignLeft & Qt::AlignHCenter,
                              LANG_PARSE(m_strReceiveTime));
    }

    if (m_bShowNote)
    {
        if (VVM_ITEM_NOTE_TYPE_FINISH == m_eNoteType)
        {
            stylePainter.drawText(rctHintLabel, Qt::AlignLeft & Qt::AlignHCenter,
                                  LANG_PARSE(LANG_TRANSLATE(TRID_USB_HINT_FINISHED)));
        }
        else if (VVM_ITEM_NOTE_TYPE_STOPPED == m_eNoteType)
        {
            stylePainter.drawText(rctHintLabel, Qt::AlignLeft & Qt::AlignHCenter,
                                  LANG_PARSE(LANG_TRANSLATE(TRID_USB_HINT_STOPPED)));
        }
    }
}
