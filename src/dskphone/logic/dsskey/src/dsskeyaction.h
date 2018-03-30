#ifndef _DSSKEY_ACTION_HEADER_
#define _DSSKEY_ACTION_HEADER_
#include <ETLLib.hpp>
#include "../include/dsskey_enumtypes.h"
#include "dsskeyactionmanager.h"
struct DssKeyLogicData;
class IDsskeyImplAction;//具体机型的实现类
class CDSSKeyAction
{
public:
    typedef YLList<DssKey_Type> DsskeyTypeList;
protected://控制对象创建接口，不允许其他函数创建
    CDSSKeyAction(DssKey_Type type);
    virtual ~CDSSKeyAction(void);

    CDSSKeyAction();
protected:
    friend class CDSSKeyActionManger;
    friend CDSSKeyAction * CreateDsskeyAction(DssKey_Type dsskeyType);

    DssKey_Type m_dsskeyType;

    IDsskeyImplAction * m_pRelatedImplAction;

    DsskeyTypeList m_listListenConfigChange;
public:
    bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);

    bool OnKeyLongPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);

    bool OnUpdateUI(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);

    bool OnBinded(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);

    bool OnDebinded(int dsskeyDataID,  const DssKeyLogicData & logicData, void * pExtraData);

    bool SetImplAction(IDsskeyImplAction * pRelatedAction);

    IDsskeyImplAction* GetImplAction()
    {
        return m_pRelatedImplAction;
    }

    bool OnTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType, void * pExtraData);

    bool OnConfigChange(int dsskeyID, void * pExtraData);

    // 网页、Autop等一次修改多个Dsskey，Dsskey模块重载完所有改变的Key后，使用该函数通知Action
    bool OnKeyReloadFinish();

    DssKey_Type GetDsskeyType();

    void SetDsskeyType(DssKey_Type type);

    bool IsBindType();

    // 定时器处理
    bool OnTimer(int dsskeyID, UINT uTimerID, const DssKeyLogicData & logicData, void * pExtraData);

    void SetTimer(int dsskeyID, UINT uTimeInterval, const DssKeyLogicData & logicData,
                  void * pExtraData);

    //获取呼出号码
    bool GetCallOutNumber(int dsskeyID, yl::string & strCallOutNumber,
                          const DssKeyLogicData & logicData, void * pExtraData, int nExtType);

    //使用该函数将类型改变通知给其他类型
    bool AddChangeListenType(DssKey_Type eType);
    bool DeleteChangeListenType(DssKey_Type eType);
    const DsskeyTypeList & GetListenTypes()
    {
        return m_listListenConfigChange;
    }

    bool BeforeUpdateUI(int dsskeyDataID, DsskeyExpRefresh dsskeyRefresh = DSS_KEY_REFRESH_ALL);

#if IF_FEATRUE_PTT
    bool OnKeyRelease(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
#endif
};

#endif

