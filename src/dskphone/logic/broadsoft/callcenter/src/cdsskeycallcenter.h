#ifndef __CDSSKEY_ACDTRACE_H__
#define __CDSSKEY_ACDTRACE_H__

#include "dsskey/include/dsskeystructure.h"
#include "dsskey/include/idsskeyimplaction.h"
#include "callcenter_inc.h"

class CDsskeyAcdTrace : public IDsskeyImplAction
{
public:
    CDsskeyAcdTrace();
    virtual ~CDsskeyAcdTrace();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};


class CDssKeyACD : public IDsskeyImplAction
{
public:
    CDssKeyACD();
    virtual ~CDssKeyACD();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);

    virtual bool OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
                                    void * pExtraData);

private:
#if IF_FEATURE_METASWITCH_ACD
    Dsskey_Status GetDsskeyStatusByACDStatus(ACD_STATUS eAcdStatus);
#endif
};

class CDssKeyHoteling : public IDsskeyImplAction
{
public:
    CDssKeyHoteling();
    virtual ~CDssKeyHoteling();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);

    virtual bool OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
                                    void * pExtraData);

private:
    Dsskey_Status GetDsskeyStatusByHotelStatus(HOTELING_STATUS eHotelStatus);
};


class CDsskeyDispositionCode : public IDsskeyImplAction
{
public:
    CDsskeyDispositionCode();
    virtual ~CDsskeyDispositionCode();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

class CDsskeyEmergencyEscalation : public IDsskeyImplAction
{
public:
    CDsskeyEmergencyEscalation();
    virtual ~CDsskeyEmergencyEscalation();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

#endif
