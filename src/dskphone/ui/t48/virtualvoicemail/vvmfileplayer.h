#ifndef _C_VVM_FILE_PLAYER_H
#define _C_VVM_FILE_PLAYER_H

#include "fileplayer.h"

class CVVMFilePlayer : public CFilePlayer
{
    Q_OBJECT

public:
    CVVMFilePlayer(QObject * pParent);
    virtual ~CVVMFilePlayer();

public:
    virtual bool LoadFilePlayData(const FilePlayerData & structFilePlayerData);

protected:
    //具体调用逻辑层的接口，具体实现根据模块和机型会有所不同，根据具体情况也会做特殊处理
    virtual bool LogicPlay();
    virtual bool LogicPause();
    virtual bool LogicResume();
    virtual bool LogicStop();
    virtual bool LogicSeek(long lSeekTimePoint);

};

#endif // _C_VVM_FILE_PLAYER_H