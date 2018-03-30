#include "dlgsetaccountringtone.h"
#include "commonunits/modcommonunits.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "qtcommon/qmisc.h"
#include "messagebox/modmessagebox.h"
#include "baseui/fontcolor.h"
#include "keymap.h"
#include "uimanager/rectdefine.h"
#include "uikernel/languagehelper.h"
#include "setting/src/soundcontroller.h"
#include "soundmanager.h"

namespace
{
enum
{
    ACCOUNT_RING_TONE = 1,
    BLF_RING_TONE,
};

const int TIME_DELAY_PLAY_RING = 500;
const int TIME_DEFAUT_DELAY_PLAY_RING = 2000;
#define kstrCommonRing  "Common"

#define OPEN_SET_BLF_RING_ACTION "OpenSubPage(blf_ring_type)"

#define INVALID_INDEX_ID -1
}

////////////////////////////////////////////////////////////////////////////////
CDlgSetAccountRingTone::CDlgSetAccountRingTone(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_nCurrentRow(0)
    , m_iAccountID(0)
    , m_nSetRingToneType(ACCOUNT_RING_TONE)
{
    m_eWndType = eWT_SubPage;
    SetWindowTitle(LANG_TRANSLATE(TRID_RING_TONES));

    m_bIsCurrentKeyRingOn = false;
    if (m_pGeneralFrame != NULL)
    {
        m_pGeneralFrame->SetDefaultFocus(false);
        m_pGeneralFrame->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG);
    }

    InitPageData();

    connect(&m_timerDelay, SIGNAL(timeout()), this, SLOT(FirstPreview()));
    connect(&m_timerDelayPlayRing, SIGNAL(timeout()), this, SLOT(OnPlaying()));

    this->installEventFilter(this);
}

CDlgSetAccountRingTone::~CDlgSetAccountRingTone()
{
    m_timerDelay.stop();
    m_timerDelayPlayRing.stop();
    Pause();
}

void CDlgSetAccountRingTone::SetData(void * pData /* = NULL */)
{
    if (NULL == pData)
    {
        return;
    }

    yl::string strAction = *(yl::string *)pData;

    if (OPEN_SET_BLF_RING_ACTION == strAction)
    {
        m_nSetRingToneType = BLF_RING_TONE;
    }
    else
    {
        m_nSetRingToneType = ACCOUNT_RING_TONE;
    }

    if (BLF_RING_TONE == m_nSetRingToneType)
    {
        SetWindowTitle(LANG_TRANSLATE(TRID_BLF_RING_TYPE));
    }
}

//加载数据
bool CDlgSetAccountRingTone::LoadPageData()
{
    this->setFocus();

    RingFileListData currentPageData;
    bool bIsSuccess =
        g_pSoundController->GetRingFileList(&currentPageData);

    if (bIsSuccess)
    {
        SetCurrentPageData(currentPageData);
    }

    return bIsSuccess;
}

//获取数据状态是否改变
bool CDlgSetAccountRingTone::IsStatusChanged()
{
    return false;
}

//保存数据
bool CDlgSetAccountRingTone::SavePageData()
{
    if (m_pGeneralFrame == NULL
            || m_nCurrentRow >= m_vecFullPath.size())
    {
        return true;
    }

    //取得当前项存放的值
    yl::string strFilePath = m_vecFullPath[m_nCurrentRow];

    //控制层调用：取得当前使用的铃声
    const yl::string strCurrentRing = GetCurrentRingPath();

    //如果选择的铃声没有变，直接返回
    if (strFilePath == strCurrentRing)
    {
        return true;
    }

    // 当设置为common,应把账号下的铃声设置为空
    if (strFilePath == kstrCommonRing)
    {
        strFilePath = "";
    }

    //调用控制层:设置为系统使用铃声
    if (!SaveCurrentRingPath(strFilePath))
    {
        //提示设置失败
        QString strContent = LANG_TRANSLATE(TRID_SET_FAILED);
        MessageBox_ShowCommonMsgBox(this, strContent,
                                    MESSAGEBOX_NOTE, SHORT_TIME,
                                    "", "", MSG_ID_SHOW_NOTE_BEFORE_BACK);
        return false;
    }

    return true;
}

void CDlgSetAccountRingTone::Init()
{
    // 进入界面延时播放声音
    m_timerDelay.start(TIME_DELAY_PLAY_RING);
}

void CDlgSetAccountRingTone::Uninit()
{
    //停止铃声播放
    Pause();
    m_timerDelayPlayRing.stop();
}

bool CDlgSetAccountRingTone::SaveCompleted()
{
    return true;
}

void CDlgSetAccountRingTone::InitPageData()
{
    if (m_pGeneralFrame != NULL)
    {
        this->setFocusPolicy(Qt::StrongFocus);
        this->setFocus();
    }

    //关闭二级菜单切页控件
    m_bCloseVerticalScrollBar = true;
}

//插槽：当选择的铃声改变时执行的操作
void CDlgSetAccountRingTone::OnCurrentIndexChanged(int iDataIndex)
{
    CDlgBaseSubPage::OnCurrentIndexChanged(iDataIndex);
    if (m_nCurrentRow == iDataIndex)
    {
        return;
    }
    //取得当前行号
    m_nPreviousRow = m_nCurrentRow;
    m_nCurrentRow = iDataIndex;

    RequestPlayRing();//铃声请求播放
}

void CDlgSetAccountRingTone::RequestPlayRing()
{
    //如果时器正在活跃状态，那么先关闭，然后重新启动计时器。
    if (m_timerDelayPlayRing.isActive())
    {
        m_timerDelayPlayRing.stop();
    }
    Pause();
    m_timerDelayPlayRing.start(TIME_DELAY_PLAY_RING);
}

void CDlgSetAccountRingTone::OnPlaying()
{
    Preview();
}

//播放铃声
void CDlgSetAccountRingTone::Preview()
{
    if (m_nCurrentRow < m_vecFullPath.size())
    {
        m_timerDelayPlayRing.stop();
        //取得当前要播放的铃声路径
        yl::string strRingtoneFilePath = m_vecFullPath[m_nCurrentRow];

        if (strRingtoneFilePath == kstrCommonRing)
        {
            strRingtoneFilePath = g_pSoundController->GetCommonRingPath();
        }

        //播放当前要播放的铃声
        g_pSoundController->PlayRing(strRingtoneFilePath);
    }
}

//停止播放铃声
void CDlgSetAccountRingTone::Pause()
{
    g_pSoundController->StopRing();
}

bool CDlgSetAccountRingTone::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL == pObject || NULL == pEvent)
    {
        return false;
    }
    switch (pEvent->type())
    {
    case QEvent::Show:
        {
            //获取按键音当前状态
            SoundGeneralData objData;
            if (NULL != g_pSoundController)
            {
                m_bIsCurrentKeyRingOn = g_pSoundController->GetGeneralData(&objData) ? objData.m_isKeypressSoundOn :
                                        false;
                objData.m_isKeypressSoundOn = false;
                //关闭按键音
                g_pSoundController->SetGeneralData(objData, false); //不记录到autop不覆盖列表
            }
        }
        break;
    case  QEvent::Hide:
        {
            //还原按键音
            SoundGeneralData objData;
            if (NULL != g_pSoundController)
            {
                objData.m_isKeypressSoundOn = m_bIsCurrentKeyRingOn;
                g_pSoundController->SetGeneralData(objData, false); //不记录到autop不覆盖列表
            }
            //停止播放音
            Pause();
        }
        break;
    default:
        break;
    }
    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

void CDlgSetAccountRingTone::FirstPreview()
{
    if (m_pGeneralFrame == NULL)
    {
        return;
    }

    //刚进入界面，播放当前铃声
    if (m_timerDelay.isActive())
    {
        m_timerDelay.stop();

        //取得铃声路径
        if (m_pGeneralFrame->GetFocusItemDataIndex() < m_vecFullPath.size())
        {
            yl::string strRingtone = m_vecFullPath[m_pGeneralFrame->GetFocusItemDataIndex()];

            if (strRingtone == kstrCommonRing)
            {
                strRingtone = g_pSoundController->GetCommonRingPath();
            }

            g_pSoundController->PlayRing(strRingtone);
        }
    }
}

//设置当前页面数据
void CDlgSetAccountRingTone::SetCurrentPageData(RingFileListData & currentPageData)
{
    if (m_pGeneralFrame == NULL)
    {
        return;
    }

    ClearAllContent();

    //成员变量初始化
    m_nCurrentRow = 0;
    m_nPreviousRow = 0;

    //取得空数据直接返回
    if (currentPageData.m_listRing.size() == 0)
    {
        return;
    }

    QWidgetPair qPair;
    qPair.eType = SETTING_ITEM_RIDIO;

    m_vecFullPath.clear();
    int nCurrentIndex = INVALID_INDEX_ID;

    //控制层调用：取得当前使用的铃声
    const yl::string strCurrentRing = GetCurrentRingPath();

    // 账号页面下多一个Common配置选项
    if (IsSetAccountNotDef())
    {
        qPair.first = LANG_TRANSLATE(TRID_COMMON_RINGTONE);
        m_vecWidgetPair.push_back(qPair);
        m_vecFullPath.push_back(kstrCommonRing);

        if (strCurrentRing.empty()
                || strCurrentRing == kstrCommonRing)
        {
            nCurrentIndex = 0;
        }
    }

    YLList<RingFileData>::ListIterator iter = currentPageData.m_listRing.begin();
    YLList<RingFileData>::ListIterator listEnd  = currentPageData.m_listRing.end();
    for (int i = 0 ; iter != listEnd; iter++, i++)
    {
        RingFileData & refData = *iter;
        //取得此item数据
        QString strText = toQString(refData.m_strRingDisplayName.c_str());

        if (!strText.isEmpty())
        {
            qPair.first = strText;
            m_vecWidgetPair.push_back(qPair);
            m_vecFullPath.push_back(refData.m_strRingPath);
        }

        //如果该item值与当前使用铃声路径相等，设置当前系统使用标识
        if (refData.m_strRingPath == strCurrentRing)
        {
            nCurrentIndex = IsSetAccountNotDef() ? i + 1 : i;
        }
    }

    if (m_vecWidgetPair.size() > 0)
    {
        AddWidget(m_vecWidgetPair, false, false, true, false);
    }

    // 矫正焦点
    if (INVALID_INDEX_ID == nCurrentIndex)
    {
        yl::string strDefaultRingtone = GetDefaultRingPath();

        int nRingCounts = m_vecFullPath.size();

        for (int nIndex = 0; nIndex < nRingCounts; ++nIndex)
        {
            if (m_vecFullPath[nIndex] == strDefaultRingtone)
            {
                nCurrentIndex = nIndex;
                break;
            }
        }

        // 无法找到则聚焦到第一项
        if (INVALID_INDEX_ID == nCurrentIndex)
        {
            nCurrentIndex = 0;
        }
    }

    SetCurrentIndex(nCurrentIndex);
    SetConfigedIndex(nCurrentIndex);

    m_nPreviousRow = nCurrentIndex;
    m_nCurrentRow = nCurrentIndex;
}

bool CDlgSetAccountRingTone::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
        QRect & rtVolume, yl::string & strBGPath) const
{
    iMin = 0;
    iMax = 15;

    return true;
}

void CDlgSetAccountRingTone::BeforeSetSubPage(void * pData)
{
    if (pData == NULL)
    {
        return;
    }

    m_iAccountID = *(int *)pData;
}

const yl::string CDlgSetAccountRingTone::GetDefaultRingPath() const
{
    if (BLF_RING_TONE == m_nSetRingToneType)
    {
        return g_pSoundController->GetDefaultBlfRingPath();
    }

    return g_pSoundController->GetDefaultRingPath();
}

const yl::string CDlgSetAccountRingTone::GetCurrentRingPath() const
{
    if (BLF_RING_TONE == m_nSetRingToneType)
    {
        return g_pSoundController->GetCurrentBlfRingPath();
    }

    return g_pSoundController->GetCurrentRingPath(m_iAccountID - 1);
}

bool CDlgSetAccountRingTone::SaveCurrentRingPath(const yl::string & strFilePath)
{
    if (BLF_RING_TONE == m_nSetRingToneType)
    {
        return g_pSoundController->SetCureentBlfRingPath(strFilePath);
    }

    return g_pSoundController->SetCurrentRingPath(strFilePath, m_iAccountID - 1);
}

bool CDlgSetAccountRingTone::IsSetAccountNotDef()
{
    return m_nSetRingToneType != BLF_RING_TONE && m_iAccountID != 0;
}
