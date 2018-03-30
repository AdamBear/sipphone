#ifndef __GENBAND_CONTACT_MANAGER_H__
#define __GENBAND_CONTACT_MANAGER_H__

#include "basecontactmanager.h"

#if IF_FEATURE_GBBOOK
class CGenBandContactManager : public CBaseContactManger
{

public:
    static CGenBandContactManager * GetInstance() DECLARE_ATTR;


    // 回调函数. 当一次异步读写完毕时, 会调用此函数.
    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    //GAB
    DIRECTORY_RET_CODE ReqSearchGABByType(const yl::string & strkey, GBBOOK_OPERATION_TYPE eType);

    //PAB
    DIRECTORY_RET_CODE ReqAddGroup(const yl::string & strName);

    DIRECTORY_RET_CODE ReqRemoveGroup(int nGroupId);

    DIRECTORY_RET_CODE ReqModifyGroup(int nGroupId, const yl::string & strName);

    DIRECTORY_RET_CODE ReqAddContact(ContactGBBookUnit & refData);

    DIRECTORY_RET_CODE ReqRemoveContact(int nId);

    DIRECTORY_RET_CODE ReqModifyContact(int nId, ContactGBBookUnit & refData);

    bool UpdatePabData();

    void ReloadData();

    const yl::string & GetCurentGabKey()
    {
        return m_strGabSearchkey;
    }

    GBBOOK_OPERATION_TYPE GetCurrentGabOptType()
    {
        return m_OptType;
    }

    void ClearGabSearchInfo()
    {
        m_OptType = PAB_GAB_NONE;
        m_strGabSearchkey.clear();
    }

    // 初始化.
    // 如果初始化成功则返回true, 否则返回false.
    bool Init();

    // 处理定时器消息.
    // | refMessage | 是要处理的消息.
    // 如果处理成功则返回TRUE, 否则返回FALSE.
    virtual bool OnTimeout(unsigned int uTimerID, bool bInTalk) DECLARE_ATTR;

    int IsContactNameExist(const yl::string & strName);

    int IsGroupNameExist(const yl::string & strName);

    bool GetIsLoaclChangeDir()
    {
        return m_bLocalChangeDir;
    }

    void SetIsLocalChangeDir(bool bIsLocal)
    {
        m_bLocalChangeDir = bIsLocal;
    }

protected:

    bool ReqGetContactList();

    bool ReqGroupList();

    virtual bool IsNeedShowAllGroup();

protected:

    bool PrasePabGroupList(const yl::string & strFile);

    bool PrasePabContactList(const yl::string & strFile);

    bool PraseSearchResult(const yl::string & strFile);

    xml_node AppendBodyNode(xml_document & doc, bool bIsGab = false);

    void AppendSearchContentNode(xml_node & node, const yl::string & strKey);

    void AppendStringTypeNode(xml_node & node, const yl::string & strName, const yl::string & strValue);

    void AppendGroupNode(xml_node & node, const yl::string strGroupName);

    bool MyExecPost(int nId, const yl::string & strUrl, GBBOOK_OPERATION_TYPE eType,
                    BaseDirDataRecord * pData = NULL);

    void SaveGabSearchFile(const yl::string & strkey, GBBOOK_OPERATION_TYPE eType);

    void SavePabFileByType(GBBOOK_OPERATION_TYPE eType);

    const yl::string & GetNewFile();

    void MySort(int nGroupId);


private:

    CGenBandContactManager();
    virtual ~CGenBandContactManager();

private:

    yl::string m_strGabSearchkey;

    GBBOOK_OPERATION_TYPE m_OptType;

    yl::string m_strUser;//GB userName

    yl::string m_strPassword;//GB password

    yl::string m_strReqestFile;

    IdChangeMap m_mapGroupId;

    int m_iInitTimer;

    bool m_bLocalChangeDir;

    FileDownloadStatus m_eStatus;
};

#define g_pGenBandDirManager (CGenBandContactManager::GetInstance())
#endif

#endif