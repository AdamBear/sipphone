#ifndef _DSSKEY_FRAME_H_
#define _DSSKEY_FRAME_H_

#include <QFrame>
// #include <list>
// #include <map>
#include "dsskeybtnbase.h"
#include "dsskeymanager.h"
#include <ylhashmap.h>

class qUIPushButton;



class qDsskeyFrame : public QFrame
{
    Q_OBJECT

public:
    qDsskeyFrame(QWidget * parent = 0);
    ~qDsskeyFrame();

    // 清空Button,包括控件释放
    void ClearButton();

    // 添加Button
    void AddDsskeyBtn(qDsskeyButton * pDsskeyBtn);

    // 添加UIPushButton
    void AddUIPushBtn(qUIPushButton * pPushBtn, QObject * obj = NULL, const char * szMember = NULL);

    // 调整Button的布局
    void AdjustBtnGeometry();

    void SetBtnClickAction(CAbstractDsskeyBtnAction * pAction);
    void SetBtnPressAction(CAbstractDsskeyBtnAction * pAction);
    void SetBtnLongPressAction(CAbstractDsskeyBtnAction * pAction);

    void RefreshDsskeyData(const DsskeyDisplayData & objDisplayData);

    // 根据索引刷新数据
    void RefreshDsskeyData(int iIndex, const DsskeyDisplayData & objDisplayData);

    void RefreshDsskeyGroundPic(const DsskeyDisplayData & objDisplayData);

    void RefreshAllDsskey();

    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

    bool IsSupportCustomDisplay()
    {
        return m_bCustomDisplay;
    };

    void SetCustomDisplayEnable(bool bEnable)
    {
        m_bCustomDisplay = bEnable;
    };

    bool SetDisplayDelegate(int iBtnIndex, DsskeyBtnDisplayDelegete * pDelegate);

    void DebindDisplayDelegate(int iBtnIndex);

protected:


private:
    qDsskeyButton * findBtnByIndex(int iBtnIndex);

signals:
    void clickChangePageBtn();

private slots:
    void OnDsskeyBtnClick(int iIndex);
    void OnDsskeyPress(int iIndex);
    void OnDsskeyLongPress(int iIndex);

    void OnStartFlash(DSS_KEY_FLASH_TYPE eFlashType);
    void OnStopFlash(DSS_KEY_FLASH_TYPE eFlashType);

    void OnFlashTimerTimeout(DSS_KEY_FLASH_TYPE eFlashType, bool bEnable);

private:
    qUIPushButton         *         m_pChangePageBtn;
    CAbstractDsskeyBtnAction    *   m_pBtnClickAction;
    CAbstractDsskeyBtnAction    *   m_pBtnPressAction;
    CAbstractDsskeyBtnAction    *   m_pBtnLongPressAction;
    YLList<qDsskeyButton *>          m_listBtnPointer;
    bool                            m_bCustomDisplay;   // 是否支持自定义显示
};

class qSettingDsskeyFrame : public qDsskeyFrame
{
    Q_OBJECT

public:
    qSettingDsskeyFrame(QWidget * parent = 0);
    ~qSettingDsskeyFrame();
protected:
    void paintEvent(QPaintEvent * pe);

};

struct FlashTimer
{
    QTimer * pTimer;
    int     nCount;

    FlashTimer()
    {
        clear();
    }

    void clear()
    {
        pTimer = NULL;
        nCount = 0;
    }
};

typedef YLHashMap<DSS_KEY_FLASH_TYPE, FlashTimer *>  MapFlashTimer;
typedef MapFlashTimer::iterator                 IterFlashTimer;

class DsskeyTimerManager : public QObject
{
    Q_OBJECT

public:
    DsskeyTimerManager();
    ~DsskeyTimerManager();

    static DsskeyTimerManager & GetInstance();

    void StartFlash(DSS_KEY_FLASH_TYPE eFlashType);

    void StopFlash(DSS_KEY_FLASH_TYPE eFlashType);

signals:
    void FlashTimerOut(DSS_KEY_FLASH_TYPE eFlashType, bool bEnable);

private slots:
    // 不同频率的定时器处理函数
    void OnSLowTimerOut();
    void OnFlashTimerOut();
    void OnFastFlashTimerOut();
    void OnFasterFlashTimerOut();
    void OnAlternateFlashTimerOut();

private:
    MapFlashTimer       m_MapFlashTimer;
};

#endif // _DSSKEY_FRAME_H_

