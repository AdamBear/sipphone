#ifndef _C_FILE_PLAYER_H
#define _C_FILE_PLAYER_H

#include <QObject>
#include <QTimer>
#include "ylstl/ylstring.h"

#include "fileplayerdefine.h"

class CFilePlayer : public QObject
{
    Q_OBJECT

public:
    CFilePlayer(QObject * pParent);
    virtual ~CFilePlayer();

private:
    void InitData();

public:
    virtual bool LoadFilePlayData(const FilePlayerData & structFilePlayerData);

    inline FILEPLAYER_STATE GetCurrentPlayState()
    {
        return m_eCurrentFilePlayStatus;
    }
    inline FilePlayerData GetPlayerData()
    {
        return m_FilePlayerData;
    }

public:
    bool Play(bool bRestart = false);
    bool Pause(bool bRealAction = true);
    bool Resume();
    bool Stop();
    bool SeekBySecond(long lSeekTimePoint);
    bool SeekByPercent(int nPercent);

    //快进快退，并没有真正去调用逻辑层接口，只是改变内部计时来影响显示
    //只有当操作完成的时候，由controller去调用SeekBySecond或者SeekByPercent
    bool SwiftNext();
    bool SwiftPre();

    //进度条拖动中
    bool InMoving(int nPercent);

protected:
    //内部计时器开始数秒，可以从非零开始
    void StartCountTime();
    //内部计时器停止数秒，当前播放时间没有置零，置零由具体需要实现的地方去实现
    void StopCountTime();

    bool JudgeTimeState();

    void ResetPlayData();

signals:
    //播放状态发生改变，包括时间变化
    void PlayStateChange();

    //播放正常完成的状态，只有正常数秒播放完才算（中间可以快进），外部需要该信号去做对应的事情
    void PlayFinish();

protected Q_SLOTS:
    virtual void OnCountTimerOut();

protected:
    //具体调用逻辑层的接口，具体实现根据模块和机型会有所不同，根据具体情况也会做特殊处理
    virtual bool LogicPlay() = 0;
    virtual bool LogicPause() = 0;
    virtual bool LogicResume() = 0;
    virtual bool LogicStop() = 0;
    virtual bool LogicSeek(long lSeekTimePoint) = 0;

protected:
    FilePlayerData                  m_FilePlayerData;

private:
    //内部计时器（数秒使用）
    QTimer                              m_CounterTimer;

    //当前播放状态
    FILEPLAYER_STATE            m_eCurrentFilePlayStatus;
};

#endif // _C_FILE_PLAYER_H
