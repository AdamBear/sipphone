//DSSKey行为实现接口，各机型在此实现对机型业务区别的控制，必须用new出来
#ifndef _DSSKEY_IMPL_ACTION_HEADER_
#define  _DSSKEY_IMPL_ACTION_HEADER_

#include "dsskey_enumtypes.h"
#include <ETLLib.hpp>
#include <ylstl/ylstring.h>
struct DssKeyLogicData;
class IDsskeyImplAction
{
public:
    IDsskeyImplAction(DssKey_Type type);

    virtual ~IDsskeyImplAction();

    inline DssKey_Type GetDsskeyType()
    {
        return m_type;
    }

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData) = 0;

    virtual bool OnKeyLongPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData)
    {
        return false;
    };
#if IF_FEATRUE_PTT
    virtual bool OnKeyRelease(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData)
    {
        return false;
    };
#endif
    virtual bool OnBindedAction(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData)
    {
        return false;
    };

    virtual bool OnDebinded(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData)
    {
        return false;
    };

    virtual bool OnTimer(int dsskeyDataID, UINT timerID, const DssKeyLogicData & logicData,
                         void * pExtraData)
    {
        return false;
    }

    virtual bool IsBindType()
    {
        return false;
    }

    virtual bool IsRelateTalk()
    {
        return false;
    }

    virtual void SetTimer(int dsskeyDataID, UINT uTimeInterval, const DssKeyLogicData& logicData,
                          void* pExtraData) {};

    // 获取呼出号码
    virtual bool GetCallOutNumber(int dsskeyID, yl::string & strCallOutNumber,
                                  const DssKeyLogicData & logicData, void * pExtraData, int nExtType = 0)
    {
        return false;
    };

    //类型改变处理，pExtraData为非空时为重新载入配置文件时变化，为NULL时则是内部接口修改配置
    virtual bool OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
                                    void * pExtraData)
    {
        //printf("OnDsskeyTypeModify [%d][%d] [%d] [%d]\n", m_type,dsskeyID, oldType, newType);
        return false;
    }

    //除类型改变外的其余配置更新
    //pExtraData 旧的数据
    virtual bool OnConfigChange(int dsskeyID,  void * pExtraData)
    {
        return false;
    }

    // 网页、Autop等一次修改多个Dsskey，Dsskey模块重载完所有改变的Key后，使用该函数通知Action
    virtual bool OnKeyReloadFinish()
    {
        return false;
    };

    virtual bool BeforeUpdateUI(int dsskeyDataID, DsskeyExpRefresh dsskeyRefresh = DSS_KEY_REFRESH_ALL)
    {
        return true;
    }

protected:
    static IDsskeyImplAction* GetImplAction(DssKey_Type eType);

private:
    DssKey_Type m_type;
};

#endif
