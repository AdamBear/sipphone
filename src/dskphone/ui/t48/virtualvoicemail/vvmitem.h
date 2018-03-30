#ifndef _C_VVM_ITEM_H
#define _C_VVM_ITEM_H

#include "settingui/src/settingitembase.h"
#include "uicommon/uikernel/qprocessbarutility.h"
#include "fileplayerdefine.h"
#include "vvmfileplayercontroller.h"

enum VVM_ITEM_NOTE_TYPE
{
    VVM_ITEM_NOTE_TYPE_ERROR,
    VVM_ITEM_NOTE_TYPE_FINISH,
    VVM_ITEM_NOTE_TYPE_STOPPED,
    VVM_ITEM_NOTE_TYPE_END
};

class CVVMItem : public CSettingItem
{
    Q_OBJECT

public:
    CVVMItem(QWidget * pParent, QWidgetPair & pair);
    virtual ~CVVMItem();

public:
    virtual void LoadItemData();

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    virtual void ResetItem();

protected:
    void StartHideTime();
    void StopHideTime();

protected:
    virtual void paintEvent(QPaintEvent * pEvent);
    virtual void Relayout();

    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

private:
    void InitData();
    void InitController();

    void ReloadItemData();

    void OnButtonsClicked(int nButtonIndex);

    void ItemPaintText(const QStylePainter & stylePainter
                       , const QRect & rctText
                       , int nFlags
                       , const QString & strText);

signals:
    void DialogAction(QString strAction);
    void DialogAction(QString strAction, QVariant var);
    void NormallyExit();

protected Q_SLOTS:
    virtual void OnItemRefreshPlayProgress();
    virtual void OnProgressBarAction(int eMouseAction);

    virtual void OnFilePlayStart();
    virtual void OnFilePlayStopped();
    virtual void OnFilePlayFinish();

    virtual void OnHideTimerOut();

protected:
    FilePlayerData                          m_PlayerData;

    QTimer                                      m_HintHideTimer;

    CProcessBarUtility         *        m_pProgressBar;
    bool                                            m_bMoving;

    CButtonWrapper                      m_btnLeft;
    CButtonWrapper                      m_btnRight;

    QString                                     m_strEmergency;
    QString                                     m_strConfidential;
    bool                                            m_bRead;

    QString                                     m_strName;
    QString                                     m_strDuration;
    QString                                     m_strReceiveDate;
    QString                                     m_strReceiveTime;

    QLabel                 *                m_pPlayCurrentTime;
    QLabel                 *                m_pPlayHintIcon;

    bool                                            m_bShowInfo;
    bool                                            m_bShowNote;
    VVM_ITEM_NOTE_TYPE              m_eNoteType;

    vecVVMItemPair                      m_vecPlayButtonsData;
};

#endif //_C_VVM_ITEM_H
