#ifdef IF_FEATURE_DSSKEY

#ifndef __TALKLOGIC_DSSKEY_DSSKEYNAACTION_H__
#define __TALKLOGIC_DSSKEY_DSSKEYNAACTION_H__
#include "dsskey/include/dsskeymsgdefine.h"

class CDsskeyNAAction
    : public IDsskeyImplAction
{
public:
    CDsskeyNAAction()
        : IDsskeyImplAction(DT_NA)
    {
    }

    virtual ~CDsskeyNAAction()
    {
    }

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData)
    {
        return true;
    }


    virtual bool OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
                                    void* pExtraData)
    {

        if (pExtraData == NULL)
        {
            etl_NotifyApp(FALSE, DSSKEY_RELOAD_FINISHED, newType, oldType);
        }

        return true;
    }

};



#endif //__TALKLOGIC_DSSKEY_DSSKEYNAACTION_H__
#endif
