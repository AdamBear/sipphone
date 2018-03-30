#include "xControls.h"
#include "xWindowManager.h"
#include "commonapi/stringhelperapi.h"
#include "modviewdump.h"


/////////////////////////////////////////////////////////////////////////
// class xFlashingBox
#if IF_FLASHING_BOX_ENABLE
EXPORT_VIEW_CLASS(xFlashingBox);

xFlashingBox::xFlashingBox()
{
    REGISTER_VIEW_DUMP_CLASS(xFlashingBox)

    showTime = -1;
    hideTime = -1;
    m_tickLastUpdate = etlGetTickCount();
    SLOT_CONNECT(&ownerStation(), signalLayoutChanged, this, xFlashingBox::slotLayoutChanged);
}

xFlashingBox::~xFlashingBox()
{
}

void xFlashingBox::loadAttributes(xml_node& node)
{
    xViewGroup::loadAttributes(node);

    PopupAndroidInt(node, XWINDOW_NAME_SHOW_TIME, showTime);
    PopupAndroidInt(node, XWINDOW_NAME_HIDE_TIME, hideTime);

    m_tickLastUpdate = etlGetTickCount();
}

void xFlashingBox::setupFlashTime(int showTime, int hideTime)
{
    if (this->showTime != showTime || this->hideTime != hideTime)
    {
        this->showTime = showTime;
        this->hideTime = hideTime;
        m_tickLastUpdate = etlGetTickCount();
        updateContentStatus();
    }
}

void xFlashingBox::onFlashTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    updateContentStatus();
}

void xFlashingBox::slotLayoutChanged(xSignal* /*sender*/, WPARAM /*wParam*/, LPARAM /*lParam*/, int /*nDataBytes*/, LPVOID /*pData*/)
{
    if (this->checkExposed(true))
    {
        updateContentStatus(true);
    }
    else
    {
        m_timerFlashing.KillTimer();
    }
}

// bOnlyCheckTimer为true时只更新定时器
void xFlashingBox::updateContentStatus(bool bOnlyCheckTimer/* = false*/)
{
    if (showTime <= 0)
    {
        if (!bOnlyCheckTimer)
        {
            m_pChild->show();
        }
        m_timerFlashing.KillTimer();
    }
    else if (hideTime > 0)
    {
        int offset = (int)((etlGetTickCount() - m_tickLastUpdate) % (hideTime + showTime));
        bool bShow = offset < showTime;
        if (bShow)
        {
            if (!bOnlyCheckTimer)
            {
                m_pChild->show();
            }
            m_timerFlashing.SingleShot(showTime - offset, TIMER_METHOD(this, xFlashingBox::onFlashTimer));
        }
        else
        {
            if (!bOnlyCheckTimer)
            {
                m_pChild->hide();
            }
            m_timerFlashing.SingleShot(showTime + hideTime - offset, TIMER_METHOD(this, xFlashingBox::onFlashTimer));
        }
    }
}
#endif

//////////////////////////////////////////////////////////////////////////
#if IF_FLASHING_IMAGE_VIEW_ENABLE
EXPORT_VIEW_CLASS(xFlashingImageView);
xFlashingImageView::xFlashingImageView()
{
    REGISTER_VIEW_DUMP_CLASS(xFlashingImageView)

    m_nShowTime = 0;
    m_nFlashingInterval = 0;
}

void xFlashingImageView::setupFlashing(int nShowTime, int nFlashingInterval)
{
    chASSERT(nShowTime > 0 && nFlashingInterval > 0);
    m_nShowTime = nShowTime;
    m_nFlashingInterval = nFlashingInterval;

    this->show();
    m_timerFlashing.SetTimer(nShowTime, TIMER_METHOD(this, xFlashingImageView::onViewFlashing));
}

void xFlashingImageView::stopFlashing()
{
    if (m_timerFlashing.IsTimerRuning())
    {
        m_timerFlashing.KillTimer();
    }
    this->show();
}

void xFlashingImageView::removeFlashing()
{
    if (m_timerFlashing.IsTimerRuning())
    {
        m_timerFlashing.KillTimer();
    }
    this->hide(true);
}

void xFlashingImageView::onViewFlashing(xThreadTimer* timer, LPVOID pExtraData)
{
    if (this->visibility != xView::visible)
    {
        this->show();
        m_timerFlashing.SetTimer(m_nShowTime, TIMER_METHOD(this, xFlashingImageView::onViewFlashing));
    }
    else
    {
        this->hide();
        m_timerFlashing.SetTimer(m_nFlashingInterval, TIMER_METHOD(this, xFlashingImageView::onViewFlashing));
    }
}
#endif

//////////////////////////////////////////////////////////////////////////
#if IF_ANIMATION_BOX_ENABLE
EXPORT_VIEW_CLASS(xAnimationBox);
xAnimationBox::xAnimationBox()
{
    REGISTER_VIEW_DUMP_CLASS(xAnimationBox)

    m_nInterval = 500;
    m_nCurrentPic = 0;
    SLOT_CONNECT(&ownerStation(), signalLayoutChanged, this, xAnimationBox::OnLayoutChanged);
}

xAnimationBox::~xAnimationBox()
{
    SLOT_DISCONNECT(&ownerStation(), signalLayoutChanged, this, xAnimationBox::OnLayoutChanged);
}

void xAnimationBox::loadAttributes(xml_node& node)
{
    xView::loadAttributes(node);

    PopupAndroidInt(node, XWINDOW_NAME_INTERVAL, m_nInterval);

    yl::string strPics = "";

    PopupAndroidString(node, XWINDOW_NAME_PICS, strPics);

    ParsePicToVec(strPics);
}

int xAnimationBox::wrapContentWidth()
{
    int nSize = m_vecPic.size();
    int nMaxWidth = 0;

    for (int i = 0; i < nSize; ++i)
    {
        int nWidth = m_vecPic[i].width();

        if (nWidth > nMaxWidth)
        {
            nMaxWidth = nWidth;
        }
    }

    return nMaxWidth;
}

int xAnimationBox::wrapContentHeight()
{
    int nSize = m_vecPic.size();
    int nMaxHeight = 0;

    for (int i = 0; i < nSize; ++i)
    {
        int nHeight = m_vecPic[i].height();

        if (nHeight > nMaxHeight)
        {
            nMaxHeight = nHeight;
        }
    }

    return nMaxHeight;
}

void xAnimationBox::SetInterval(int nInterval)
{
    m_nInterval = nInterval;
}

void xAnimationBox::SetAnimation(const LIST_STRING& listPic)
{
    m_timerSwitch.KillTimer();
    m_nCurrentPic = 0;

    LIST_STRING::const_iterator iter = listPic.begin();

    for (; iter != listPic.end(); ++iter)
    {
        const yl::string& strPic = *iter;

#if IF_XWINDOW_DUMP_EXPORT
        if (!m_strPics.empty())
        {
            m_strPics += ",";
        }

        m_strPics += strPic;
#endif

        xPixmap pmPic;
        g_WorkStation.GetImageByPath(strPic, pmPic);

        if (!pmPic.isNull())
        {
            m_vecPic.push_back(pmPic);
        }
    }

    onContentChanged();
}

void xAnimationBox::StopAnimation()
{
    m_timerSwitch.KillTimer();
}

void xAnimationBox::RestartAnimation(bool bReset/* = false*/)
{
    if (bReset)
    {
        m_nCurrentPic = 0;

        update();
    }

    m_timerSwitch.SetTimer(m_nInterval, TIMER_METHOD(this, xAnimationBox::OnSwitchTimer));
}

void xAnimationBox::OnSwitchTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    SwitchToNextPic();
}

void xAnimationBox::OnLayoutChanged(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
{
    if (this->checkExposed(true))
    {
        if (!m_timerSwitch.IsTimerRuning() && m_nInterval > 0)
        {
            m_timerSwitch.SetTimer(m_nInterval, TIMER_METHOD(this, xAnimationBox::OnSwitchTimer));
            update();
        }
    }
    else
    {
        m_nCurrentPic = 0;
        m_timerSwitch.KillTimer();
    }
}

void xAnimationBox::SwitchToNextPic()
{
    int nSize = m_vecPic.size();

    if (nSize > 0)
    {
        update();

        ++m_nCurrentPic;
        m_nCurrentPic = m_nCurrentPic % nSize;
    }
}

void xAnimationBox::ParsePicToVec(const yl::string& strPic)
{
#if IF_XWINDOW_DUMP_EXPORT
    m_strPics = strPic;
#endif

    yl::string strYLSrc(strPic.c_str());
    const char* strSeparater = ",";

    int iFound = strYLSrc.find_first_of(strSeparater);
    while (iFound != yl::string::npos)
    {
        yl::string strSub = strYLSrc.substr(0, iFound);
        strYLSrc = strYLSrc.substr(iFound + 1);

        m_vecPic.push_back(xPixmap(strSub.c_str()));
        iFound = strYLSrc.find_first_of(strSeparater);
    }
    if (!strYLSrc.empty())
    {
        m_vecPic.push_back(xPixmap(strYLSrc.c_str()));
    }
}

bool xAnimationBox::onPaintContent(xPainter& painter)
{
    int nSize = m_vecPic.size();
    xPixmap pmPic;

    if (m_nCurrentPic >= 0 && m_nCurrentPic < nSize)
    {
        pmPic = m_vecPic[m_nCurrentPic];
    }

    if (!pmPic.isNull())
    {
        painter.drawItemPixmap(contentRect(), pmPic);
    }

    return true;
}
#endif
