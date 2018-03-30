#ifndef _PRESENCE_MODEL_H_
#define _PRESENCE_MODEL_H_

#if FEATURE_BROADTOUCH_UC

#include "presence_inc.h"

#include "taskaction/modtaskaction.h"
#include "taskaction/taskactioncallbackbase.h"
//#include "TaskAction/requestrosteraction.h"
#include "basecontactdata.h"
#include "xmpp/include/libxmpp.h"
#ifdef UC_TEST
#include "Test.h"
#endif
using namespace NS_TA;

#ifdef  __cplusplus
extern "C" {
#endif
#include "xmpp/include/libxmpp.h"
#ifdef  __cplusplus
};
#endif

#ifdef UC_TEST
int push_handler(char * xml_stream, size_t stream_len);
int push_handler_presence(char * xml_stream, size_t stream_len);
#endif

class CUCContactData : public CBaseContactData, public CTaskActionCallbackBase
{
public:

    static CUCContactData * GetInstance();

    //  重新所有联系人信息
    bool RefreshAllInfo();

    //处理presence的xml流
    //处理状态改变
    //如头像特征码改变则重新获取头像(Gtalk)
    bool ProcessPresenceStream(const yl::string & strXmlStream);

    //返回本地自己的Presence信息
    const PresenceInfo & GetLocalPresence();

    // 刷新头像信息
    bool RefreshAvatar(const yl::string & strContactId);

    //处理roster改变,联系人增删，备注名改变
    bool RefreshRosterInfo(const yl::string & strRenameInfo, RefreshType & nRefreshType);

    // 回调函数
    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    //获取自己的头像
    void ReqMyAvatar();

    //获取favorite 联系人
    bool ReqAddressbookList(bool bRefreshAll = false);

    //订阅联系人，订阅服务器才会推送联系人变更消息下来
    void ReqRosterAll();

    //请求话机端状态改变
    bool ReqChangPresence(_PRESENCE_TYPE eType);

    bool IsIniting()
    {
        return m_bIsIniting;
    }

    //获取联系人头像
    bool ReqContactAvatar(PresenceInfo & refData);

    inline bool IsLocked()
    {
        return m_bLock;
    }

    // 刷新观察者对象的Presence状态
    void RefreshViewerPresence();

    BOOL OnTimeOut(msgObject & objMessage);

    BOOL OnFavoriteTimeOut(msgObject & objMessage);

    bool GetImageByNumber(yl::string & strPath, const yl::string & strNumber);

    bool CheckReConnect();

    BOOL OnReconnectMessage(msgObject & refObj);

    void SetAllowChangeStatus(bool bAllowChangePresence);

    bool IsAllowChangeStatus();

    bool SetXmppFeaturesAppend();
    bool SetXmppFeaturesManual();
    static void SetXmppLog(int iModuleID, int iLogLevel);

private:

    CUCContactData();
    virtual ~CUCContactData();

    _PRESENCE_TYPE GetPrensenceTypeByString(const yl::string & strPresence);

    // 解析card数据，获取头像和号码
    bool ParseCardData(PresenceInfo & objData, const yl::string & strCardXmlInfo);

    // 将base64数据生成以id为名称，format为格式的图片
    yl::string GetAvatar(const yl::string & strBase64Data, const yl:: string & strContactID,
                         const yl::string & strPicFormat);

    //添加到Map中
    void AddCommonData(PresenceInfo & refData);

    // 获取资源描述符,该符号随机生成必须和之前生成的不一致
    yl::string GetConnectURI();

    // 判断ID是否为用户ID
    bool IsUserJID(const yl::string & strID);

    // 设置用户的Presence数据
    void SetUserPresenceData(const PresenceInfo & presenceData);

    //根据Jid判断是否是Favorite联系人
    bool IsFavoriteContact(const yl::string & strJid);

    //解析Favorite联系人
    void ParseAddressbookXml(const yl::string & strContactsXml, bool bRefreshAll = false);

    //请求所有联系人状态
    void ReqAllPresence();

    bool InsertIdToGroupList(const yl::string & strContactId, ListContactID & refList,
                             bool bSort = false);

    void RemoveContactInGroup(GroupKey & key);

    void SetGroupKey(GroupItem & refData);

    bool ParseUpdateMessage(xml_document & objXmlDoc);

    chCriticalSection m_cs;

private:
    static CUCContactData * m_Instace;


    XMPP_CONN_INFO   *  m_connInfo;//xmpp请求是存放账号信息,调试信息等级等信息

    XMPP_CONN * m_conn;

    yl::string m_strUserID;//账号
    yl::string m_strPassword;//密码

    yl::string m_strLogId;

    PresenceInfo    m_objLocalPresenceData;         //本地Presence状态

    YLHashMap<yl::string, yl::string> m_mapContacts;

    bool m_bIsIniting;

    bool m_bLock;

    mkThreadTimer   m_RefreshTimer;

    mkThreadTimer   m_favoriteTimer;

    mkThreadTimer   m_ReconnectTimer;

    int m_nPreGroupSize;

    bool m_bAllowChangePresence;

    friend class CPresenceContactManager;

    yl::string  m_strLastUpdateTime;

};

#define g_pUCPresence (CUCContactData::GetInstance())

#endif//FEATURE_BROADTOUCH_UC

#endif
