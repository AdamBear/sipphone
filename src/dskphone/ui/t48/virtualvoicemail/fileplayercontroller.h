#ifndef _C_FILE_PLAYER_CONTROLLER_H
#define _C_FILE_PLAYER_CONTROLLER_H

#include "fileplayerdefine.h"
#include "fileplayer.h"

class CFilePlayerController : public QObject
{
    Q_OBJECT

public:
    CFilePlayerController(QObject * pParent);
    virtual ~CFilePlayerController();

public:
    FILEPLAYER_STATE GetCurrentPlayState();
    FilePlayerData GetPlayerData();

private:
    void InitData();

protected:
    virtual void SetFilePlayer(CFilePlayer * pFilePlayer);

    virtual void LoadFilePlayData(const FilePlayerData & structFilePlayerData);

    //内部计时器开始计时，bStartTime表示是计时m_nSwiftStartTime还是m_nSwiftInterval
    virtual bool StartSwiftTime(bool bStartTime);
    //内部计时器停止
    virtual void StopSwiftTime();
    //计时器人为结束，即释放了长按的键
    virtual bool SwiftOver();

    //进行一次快进/快退操作
    virtual bool SwiftAction();

    virtual bool StartMove();
    virtual bool InMoving(int nPercent);
    virtual bool MoveOver(long lTargetTime);

signals:
    void PlayStateChange();

protected Q_SLOTS:
    virtual void OnSwiftTimerOut();

    virtual void OnPlayStateChange();
    virtual bool OnPlayerAction(QString strAction, QVariant var);

protected:
    CFilePlayer          *          m_pFilePlayer;

    QTimer                          m_SwiftTimer;

    //记录当前计时器是正计时StartTime还是Interval
    bool                                m_bStartTime;
    //记录是否至少快进/快退过一次
    bool                                m_bSwifted;
    //记录是在使用快进还是快退
    bool                                m_bSwiftToNext;
    //开始时间，即长按了该长度时间后，才认为是长按并需要连续快进/快退
    int                             m_nSwiftStartTime;
    //间隔时间，处于长按状态后，每隔多少时间快进/快退一次
    int                             m_nSwiftInterval;

    //记录快进/快退操作前文件的播放状态
    FILEPLAYER_STATE        m_eLastFilePlayStatus;

};

#endif //_C_FILE_PLAYER_CONTROLLER_H