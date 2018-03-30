#ifndef __X_CONTROLS_H__
#define __X_CONTROLS_H__


#include "xWindow.h"
#include "xwindow-kernel/include/xthreadtimer.h"


USING_XKERNEL_NAMESPACE
//////////////////////////////////////////////////////////////////////////
#if IF_FLASHING_BOX_ENABLE
class XWINDOW_EXPORT xFlashingBox : public xFrameLayout, public xSlotHandler, public xTimerHandler
{
    DECLARE_VIEW_DUMP_CLASS(xFlashingBox)

public:
    virtual void loadAttributes(xml_node& node);
public:
    void setupFlashTime(int showTime, int hideTime);

private:
    void onFlashTimer(xThreadTimer* timer, LPVOID pExtraData);
    void slotLayoutChanged(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);
    void updateContentStatus(bool bOnlyCheckTimer = false);

public:
    xFlashingBox();
    ~xFlashingBox();
private:
    int             showTime;//ms
    int             hideTime;//ms
    UINT64          m_tickLastUpdate;
    xThreadTimer    m_timerFlashing;
};
#endif

//////////////////////////////////////////////////////////////////////////
#if IF_FLASHING_IMAGE_VIEW_ENABLE
class XWINDOW_EXPORT xFlashingImageView : public xImageView, public xTimerHandler
{
    DECLARE_VIEW_DUMP_CLASS(xFlashingImageView)

public:
    void setupFlashing(int nShowTime, int nFlashingInterval);
    void stopFlashing();
    void removeFlashing();

    xFlashingImageView();
private:
    void onViewFlashing(xThreadTimer* timer, LPVOID pExtraData);
private:
    int             m_nShowTime;
    int             m_nFlashingInterval;
    xThreadTimer    m_timerFlashing;
};
#endif

//////////////////////////////////////////////////////////////////////////
#if IF_ANIMATION_BOX_ENABLE
class XWINDOW_EXPORT xAnimationBox : public xView, public xSlotHandler, public xTimerHandler
{
    DECLARE_VIEW_DUMP_CLASS(xAnimationBox)

    typedef YLVector<xPixmap> VEC_PICTURE;
public:
    virtual void loadAttributes(xml_node& node);
public:
    void SetInterval(int nInterval);
    void SetAnimation(const LIST_STRING& listPic);

    virtual bool onPaintContent(xPainter& painter);

    void StopAnimation();
    void RestartAnimation(bool bReset = false);

private:
    virtual int wrapContentWidth();
    virtual int wrapContentHeight();
private:
    void OnSwitchTimer(xThreadTimer* timer, LPVOID pExtraData);
    void OnLayoutChanged(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);
    void SwitchToNextPic();
    void ParsePicToVec(const yl::string& strPic);

public:
    xAnimationBox();
    ~xAnimationBox();
private:
    VEC_PICTURE     m_vecPic;
    int             m_nInterval;//ms
    int             m_nCurrentPic;
    xThreadTimer    m_timerSwitch;
#if IF_XWINDOW_DUMP_EXPORT
    yl::string      m_strPics;
#endif
};
#endif

#endif // __X_CONTROLS_H__
