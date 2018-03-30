#ifndef MTSW_VOICEMAIL_MANAGER_H_
#define MTSW_VOICEMAIL_MANAGER_H_

#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "singletonclassdefine.h"
#include "taskaction/taskactioncallbackbase.h"
#include "metaswitch/mtswcontrol/include/mtswactioncmdbase.h"

#include "account/include/modaccount.h"
#include "json/cjson.h"
#include "metaswitch/mtswvoicemail/include/mtswvoicemaildefine.h"

using NS_TA::CTaskAction;
using NS_TA::CTaskActionCallbackBase;


class CMTSWVoiceMailManager : public CMTSWActionCmdBase
{
    DECLARE_SINGLETON_CLASS(CMTSWVoiceMailManager)

public:
    // 信息链表.
    typedef YLList<VoicemailItemData *> ItemList;

public:

    //在这个函数里需要做的是根据错误码得出错误信息，把出错信息传递给上一层进行处理
    virtual void OnRequestError(NS_TA::TaskActionErrorType eErrCode, int nType, int nId);

    //strFilePath是保存服务器返回的数据的文件路径。在此需要解析数据，通知上一层进行处理
    virtual bool OnLoadSucProcess(CMTSWAction * pAction);

    BOOL LoadVoiceMailList();

    BOOL GetMessageList(VoiceMsgSimpListData * pData, int & nTotal);

    BOOL GetMessageDetail(VoiceMsgItemDetailData * pData, int nIndex);

    yl::string FormatDuration(int nDuration);

    ReadStatus GetMessageStatus(int nIndex = -1);

    BOOL MarkReadStatus(ReadStatus eStatus);

    void Init();
    void MyFinal();

    void SetItemIndex(int nIndex)
    {
        m_iItemIndex = nIndex;
    }
    int GetItemIndex()
    {
        return m_iItemIndex;
    }

    void SetPlayStatus(PlayStatus eStatus)
    {
        m_ePlayStatus = eStatus;
    }
    PlayStatus GetPlayStatus()
    {
        return m_ePlayStatus;
    }

    BOOL DownloadVoiceMail();
    BOOL Play();
    BOOL Pause();
    BOOL Resume();
    BOOL Stop(bool bReset = true);
    BOOL Delete();
    BOOL DeleteAll();

    BOOL DownloadNextVoiceMail();

    int GetDuration();

    int GetTotalUnRead()
    {
        return m_nTotalUnread;
    }

    void SetDownLoading(bool bLoading)
    {
        m_bDownLoading = bLoading;
    }
    bool IsDownLoading()
    {
        return m_bDownLoading;
    }

    void SetPlayLoading(bool bLoading)
    {
        m_bPlayLoading = bLoading;
    }

    void SetFromIndex(int nIndex)
    {
        m_nIndexFrom = nIndex;
    }
    int GetFromIndex()
    {
        return m_nIndexFrom;
    }

    int GetTotalCount()
    {
        return m_nTotalCount;
    }

    bool IsMessagesListEnable();

    void CancelDownload()
    {
        m_fileData.m_eDownloadStatus = FILE_DOWNLOAD_CANCELED;
    }

    time_t TransformTime2Time_t(LPCSTR strDate, BOOL bHaveSecond);

protected:

    BOOL RemoveAllMessage();

    BOOL RemoveMessage();

    bool ParseVoiceMailList(cJSON * pOjbParse);

    //BOOL GetNextUnReadMessage(VoiceMsgItemDetailData* pData, int nIndex);

private:
    VoicemailItemData * MyFindItem(int nIndex);

    VoicemailItemData * MyFindNextUnReadItem(int nIndex);

    // 向链表中插入一条记录.
    // | refItem | 是要插入记录的数据.
    // 如果插入成功则返回记录在链表中的序号, 否则返回-1.
    int MyInsertItem(const VoicemailItemData & refItem);

    // 将ItemData转换为语音信息的简单数据.
    // | pData | [out] 为buffer.
    // | refItem | 为ItemData.
    // 如果转换成功则返回true, 否则返回false.
    BOOL MyID2VMSD(VoiceMsgItemSimpData * pData, const VoicemailItemData & refItem);

    // 从联系人获取名称.
    yl::string GetNameFromContact(const char * lpszNumber);

    // 将ItemData转换为信息详情.
    // | pData | [out] 为buffer.
    // | refItem | 为ItemData.
    // 如果转换成功则返回true, 否则返回false.
    BOOL MyID2VMIDD(VoiceMsgItemDetailData * pData, const VoicemailItemData & refItem);


private:
    ItemList m_listItem;  // 信息链表.
    int m_iItemIndex; //
    int m_nTotalUnread;
    bool m_bDownLoading;
    bool m_bPlayLoading;
    int m_nIndexFrom;  // 从何处下标开始获取.
    int m_nTotalCount; //总数

    PlayStatus m_ePlayStatus; //播放状态

    VoiceMailFileData m_fileData;
};

#define _MTSWVMManager CMTSWVoiceMailManager::GetInstance()

#endif
#endif //MTSW_VOICEMAIL_MANAGER_H_
