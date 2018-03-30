#ifndef __ETL_RegistryHelper_H__
#define __ETL_RegistryHelper_H__
//////////////////////////////////////////////////////////////////////////
// class LCFileInfo
#ifndef __ETL_H__
#error ETLRegistry.h requires ETL.h to be included first
#endif

#define  IF_ETL_LEVEL_MODIFYCFG      1

namespace ETL_RegistryHelper
{

class offset_pointer
{
public:
    void * get_pData() const;
    void set_pData(const void * pData);
protected:
    POINTER         offset_pData;
};

enum CONFIG_LEVEL_TYPE
{
    CONFIG_LEVEL_BEG      = 0,
    CONFIG_LEVEL_DEFAULT  = 1 << 0,     // 不区分等级
    CONFIG_LEVEL_IGNORE   = 1 << 1,     // 该配置无等级
    CONFIG_LEVEL_PHONE    = 1 << 2,     // 读写话机等级,值小优先级高
    CONFIG_LEVEL_WEB      = 1 << 3,     // 读写网页等级,值小优先级高
    CONFIG_LEVEL_AUTOP    = 1 << 4,     // 读写autop等级,值小优先级高
    CONFIG_LEVEL_FACTORY  = 1 << 5,     // 读写出厂出厂等级,值小优先级高
    CONFIG_LEVEL_UPDATING = 1 << 6,
    CONFIG_LEVEL_END      = 1 << 7
};

#define _IS_VALID_LEVEL(x) ((x <= CONFIG_LEVEL_BEG) || (x >= CONFIG_LEVEL_END))

#define _LEVEL_EQUAL(x,y) (((x & y) == x) || (x & y) == y)  //只要包含就相等

#define _LEVEL_ISUPDATING(x) ((x & CONFIG_LEVEL_UPDATING) == CONFIG_LEVEL_UPDATING)

#define _LEVEL_REMOVEUPDATING(x) (x = (CONFIG_LEVEL_TYPE)(x &~ CONFIG_LEVEL_UPDATING));

#define KEY_CHANGE_NONE    0
#define KEY_CHANGE_MODIFY  1 << 0
#define KEY_CHANGE_ADD     1 << 1

#if IF_USE_REGISTRY
class KeyInfo
{
public:
    chShareStringA      strValue;
    CONFIG_LEVEL_TYPE   eType;

    KeyInfo()
    {
        eType = CONFIG_LEVEL_FACTORY;
    }

    KeyInfo & operator = (const KeyInfo & key)
    {
        if (this != &key)
        {
            strValue = (LPCSTR) key.strValue;
            eType = key.eType;
        }
        return *this;
    }
};

//////////////////////////////////////////////////////////////////////////
// class regKey
class regKey : public chObjList_pool_share< KeyInfo >
{
public:
    LPCSTR  GetValue(CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);
    BOOL    GetValue(CONFIG_LEVEL_TYPE eType, LPCSTR & strValue);
    int     SetValue(LPCSTR lpszValue, CONFIG_LEVEL_TYPE eType);

    BOOL    UpdateType(CONFIG_LEVEL_TYPE eNewType);

    CONFIG_LEVEL_TYPE GetCurrentLevel();

    KeyInfo * FindKeyObject(CONFIG_LEVEL_TYPE eType);
    BOOL     AddKeyObject(CONFIG_LEVEL_TYPE eType, LPCSTR lpszValue);
    BOOL     RemoveKeyObject(CONFIG_LEVEL_TYPE eType);
public:
    regKey();
public:
    chShareStringA      m_strKeyName;
    chShareStringA      m_strValue;
    int                 m_nChagedCount;
    CONFIG_LEVEL_TYPE   m_eType;
protected:
    int   SetListValue(LPCSTR lpszValue, CONFIG_LEVEL_TYPE eType);
private:
    BOOL                m_bValueUndefined;
};

//////////////////////////////////////////////////////////////////////////
// class regSection
class regDocument;
class regSection : public chObjList_pool_share< regKey >
{
    //typedef chObjList_pool_share< msgEntryPair >            typeTaskList;
public:
    regKey * FindKeyObject(const phraseConstTextA & strKeyName);
    regKey & GetKeyObject(const phraseConstTextA & strKeyName);

    regDocument * GetHostDocument() const;
    void SetHostDocument(const regDocument * pDocument);

//    msgEntryPair * AddMonitorTask(TASK_ID idTask, chMsgProcesser * pMsgReceiver, msgEntry pMeth);
//    void RemoveMonitorTask(TASK_ID idTask, chMsgProcesser * pMsgReceiver, msgEntry pMeth);
//    void RemoveMonitorTask(msgEntryPair * pEntryObject);
//private:
//    void ClearGhostTask();
public:
    regSection();
public:
    chShareStringA      m_strSectionName;
    //typeTaskList        m_listMonitoringTask;
    BOOL                m_bMonitorActived;
    int                 m_nChagedCount;
    POINTER             m_offsetDocument;
};

//////////////////////////////////////////////////////////////////////////
// class regDocument
class regDocument : public chObjList_pool_share< regSection >
{
public:
    regSection * FindSectionObject(const phraseConstTextA & strSectionName);
    regSection & GetSectionObject(const phraseConstTextA & strSectionName);

    BOOL CreateFromFile(LPCSTR lpszFileName);
public:
    regDocument();
public:
    chShareStringA      m_strDocumentName;
    int                 m_nChagedCount;
};

//////////////////////////////////////////////////////////////////////////
// class regDocumentManager
class regDocumentManager : public ipcObjectHeader
    , public baseStaticPoolAlloc
    , public chObjList_pool_share< regDocument >
{
public:
    regDocument * FindDocumentObject(const phraseConstTextA & strDocumentName);
    regDocument & GetDocumentObject(const phraseConstTextA & strDocumentName);
public:
    regDocumentManager();
public:
    TASK_ID             m_idDaemonTask;
    BOOL                m_bGlobalProtect;
};

typedef ipcObjectFrame<regDocumentManager, ipcMapFile, ipcMutex>    ipcSystemRegistryObject;
class uCRegistryHelper : public ipcSystemRegistryObject
{
public:
    uCRegistryHelper();
    ~uCRegistryHelper();
};

DECLARE_GLOBLE_OBJECT(uCRegistryHelper);
#define g_uCRegistryHelper      REFERENCE_GLOBLE_OBJECT(uCRegistryHelper)
#define g_bRegistryProtect      g_uCRegistryHelper.getObject().m_bGlobalProtect

////////////////////////////////////////////////////////////////////////////
//// class chRegistryMonitor
//class chRegistryMonitor : public chMapList<regSection *, msgEntryPair *>
//{
//    typedef chMapList<regSection *, msgEntryPair *>                   typeSessionMap;
//public:
//    msgEntryPair * MonitorSection(LPCSTR lpszDocumentName, LPCSTR lpszSectionName,
//                                  chMsgProcesser * pMsgReceiver, msgEntry pMeth);
//    void UnmonitorSection(LPCSTR lpszDocumentName, LPCSTR lpszSectionName);
//    void UnmonitorAllSection();
//};
#endif // IF_USE_REGISTRY

};
using namespace ETL_RegistryHelper;

#if IF_EXPORT_SYMBOLS
#include "ETLRegistry.cpp"
#endif

#endif //__ETL_RegistryHelper_H__
