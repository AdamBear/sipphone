#ifndef _SETINGUI_RING_TONE_SETTING_DELEGATE_H_
#define _SETINGUI_RING_TONE_SETTING_DELEGATE_H_

#include "settinguiaccountbasedelegate.h"
#include "xwindow-kernel/include/xthreadtimer.h"

class CSettingUIRingToneSettingDelegate : public CSettingUIAccountDelegateBase, public xTimerHandler
{
public:
    CSettingUIRingToneSettingDelegate();
    virtual ~CSettingUIRingToneSettingDelegate();

    static CSettingUIDelegateBase * CreateRingToneSettingDelegate();
public:
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool OnAction(int eActionType, int iDataIndex);
    virtual bool IsShowVolumeBar(int& iVolumeType, int& iMin, int& iMax, yl::string& strPix);
protected:
    void AddListRing(CSettingUIPageDataBase * pPageData);
    bool IsAccount();
    void OnItemFocusChange(int nIndex);
    void OnStartPalyRing(xThreadTimer* timer, LPVOID pExtraData);
    void ResetSatus();

protected:
    VEC_STRING_DATA m_vecRingPath;
private:
    bool m_bKeyPressSound;
    xThreadTimer m_tmPalyRing;
};

#endif //_SETINGUI_ACCOUNT_RING_TONE_SETTING_DELEGATE_H_
