#ifndef __DIR_DSS_KEY_H__
#define __DIR_DSS_KEY_H__

#include "dsskey/include/dsskeystructure.h"
#include "dsskey/include/idsskeyimplaction.h"


// DT_XMLGROUP
class CDssKeyActionXMLGroup : public IDsskeyImplAction
{
public:
    CDssKeyActionXMLGroup();
    virtual ~CDssKeyActionXMLGroup();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

// DT_IDLE_CALLLIST
class CDssKeyActionIdleCallList : public IDsskeyImplAction
{
public:
    CDssKeyActionIdleCallList();
    virtual ~CDssKeyActionIdleCallList();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

// DT_LDAP
class CDssKeyActionLDAP : public IDsskeyImplAction
{
public:
    CDssKeyActionLDAP();
    virtual ~CDssKeyActionLDAP();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

// DT_LOCALDIR
class CDssKeyActionLocalDir : public IDsskeyImplAction
{
public:
    CDssKeyActionLocalDir();
    virtual ~CDssKeyActionLocalDir();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

// DT_BSDIR
class CDssKeyActionBSDir : public IDsskeyImplAction
{
public:
    CDssKeyActionBSDir();
    virtual ~CDssKeyActionBSDir();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

// DT_LOCALGROUP
class CDssKeyActionLocalGroup : public IDsskeyImplAction
{
public:
    CDssKeyActionLocalGroup();
    virtual ~CDssKeyActionLocalGroup();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

// DT_BSGROUP
class CDssKeyActionBSGroup : public IDsskeyImplAction
{
public:
    CDssKeyActionBSGroup();
    virtual ~CDssKeyActionBSGroup();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

// DT_IDLE_REMOTE
class CDssKeyActionIdleRemote : public IDsskeyImplAction
{
public:
    CDssKeyActionIdleRemote();
    virtual ~CDssKeyActionIdleRemote();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

// DT_FAVORITES
class CDssKeyActionFavrites : public IDsskeyImplAction
{
public:
    explicit CDssKeyActionFavrites(DssKey_Type type);
    virtual ~CDssKeyActionFavrites();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

#if FEATURE_BROADTOUCH_UC
// DT_NETWORK_FAVORITE
class CDssKeyActionNetworkFavorite : public IDsskeyImplAction
{
public:
    CDssKeyActionNetworkFavorite();
    virtual ~CDssKeyActionNetworkFavorite();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

// DT_UC_FAVORITE
class CDssKeyUCFavorite : public IDsskeyImplAction
{
public:
    CDssKeyUCFavorite();
    virtual ~CDssKeyUCFavorite();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
    virtual bool OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
                                    void * pExtraData);
};

// DT_BUDDIES
class CDssKeyBuddies : public IDsskeyImplAction
{
public:
    CDssKeyBuddies();
    virtual ~CDssKeyBuddies();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};
#endif

// DT_REDIAL
class CDsskeyRedialAciton: public IDsskeyImplAction
{
public:
    CDsskeyRedialAciton();
    virtual ~CDsskeyRedialAciton();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

#if IF_BUG_14856
// DT_MISSED_CALL
class CDssKeyActionMissedCall : public IDsskeyImplAction
{
public:
    CDssKeyActionMissedCall();
    virtual ~CDssKeyActionMissedCall();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};
#endif

#if IF_BUG_PLCMDIR
class CDssKeyActionPlcmDir : public IDsskeyImplAction
{
public:
    CDssKeyActionPlcmDir();
    virtual ~CDssKeyActionPlcmDir();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
class CDssKeyActionMTSWDir : public IDsskeyImplAction
{
public:
    CDssKeyActionMTSWDir();
    virtual ~CDssKeyActionMTSWDir();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};
#endif

#ifdef IF_FEATURE_PSTN
// DT_LOCAL_FAVOTITE
class CDssKeyActionLocalFavorite : public IDsskeyImplAction
{
public:
    CDssKeyActionLocalFavorite();
    virtual ~CDssKeyActionLocalFavorite();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};
#endif

#endif
