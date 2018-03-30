#ifndef __DLG_USB_AUDIO_PLAYER_H__
#define __DLG_USB_AUDIO_PLAYER_H__


#include "basesettingdialog.h"

#ifdef IF_USB_SUPPORT
enum
{
    SETTTING_DIALOG_SUB_TYPE_USB_PLAYER     = 0x03000000,
};

class xTimerPieceView;
class xTextView;
class xProgressBar;

class CDlgUSBAudioPlayer : public xLinearLayout, public CSettingDialogBase, public xTimerHandler
{
public:
    CDlgUSBAudioPlayer();
    virtual ~CDlgUSBAudioPlayer();

    static CBaseDialog * CreateInstance();
    static CDlgUSBAudioPlayer * GetDlgUSBAudioPlayer(CSettingDialogBase * pDlg);

public:
    virtual void Uninit();

    virtual void OnFocusChanged(bool bFocused);

    virtual void loadChildrenElements(xml_node & node);

    virtual bool IsRejectReturnToIdle();

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

    virtual bool onKeyPressedEvent(int key);
    virtual bool onKeyReleasedEvent(int key);

    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax, yl::string & strPix);

public:
    virtual void RefreshUI(bool bReLoadData = false);
    // 刷新Softkey
    virtual void RefreshSoftkey();

    void OnDelayPlay(xThreadTimer* timer, LPVOID pExtraData);

    void Pause();
    void Resume();

    void PlaySeek(bool bForward, bool bRepeate);
    void OnSeekTimer(xThreadTimer* timer, LPVOID pExtraData);

    void OnStop();
    void OnDelayExit(xThreadTimer* timer, LPVOID pExtraData);

    void OnIconSoftkeyLongPress(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                LPVOID pData);
    void OnTimerChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

protected:
    void StartDelayAction(int nAction);
    void RefreshAudioInfo();
    void UpdateStatusText();
    void ResetDelayAction();

    bool CheckPlayEnd();

protected:
    enum
    {
        TIMER_ACTION_TYPE_NONE          = 0,
        TIMER_ACTION_TYPE_PLAY          = 1,
        TIMER_ACTION_TYPE_EXIST         = 2,
        TIMER_ACTION_TYPE_PLAY_LEFT     = 3,
        TIMER_ACTION_TYPE_PLAY_RIGHT    = 4,
    };

    xThreadTimer        m_ActionTimer;
    int                 m_nAction;
    bool                m_bSelfExit;

    xTimerPieceView  *  m_pTimerPiece;
    xTextView     *     m_pTextStatus;
    xTextView     *     m_pTextTotalTime;
    xProgressBar    *   m_pProgressBar;
    bool                m_bPressed;
};
#endif

#endif
