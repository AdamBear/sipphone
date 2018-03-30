#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYCUSTOMACTION_H__
#define __TALK_DSSKEY_DSSKEYCUSTOMACTION_H__

#include "dsskeytalkbaseaction.h"

class CDsskeyCustomAction
    : public CDssTalkBaseAciton
{
public:
    CDsskeyCustomAction();

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);

    virtual bool OnTimer(int dsskeyDataID, UINT timerID, const DssKeyLogicData & logicData,
                         void * pExtraData);
protected:
    char GetNextChar(int dsskeyDataID);

    yl::string GetCustomStatus();

    bool SendKeyMsg(int iKeyCode, bool bDown);

private:
    //用于记录Dtmf的当前位置 (DsskeyID, CurrentIndex)
    static YLHashMap<int, int> mapDtmfIndex;
};



#endif // __TALK_DSSKEY_DSSKEYCUSTOMACTION_H__
#endif  //IF_FEATURE_DSSKEY
