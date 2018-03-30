/************************************************************************
 * FileName  : MsgUILogic.h (header file)
 * Purpose   :
 * Date      : 2012/06/06 9:53:13
 ************************************************************************/

#ifndef _MSGUILOGIC_HEADER_
#define _MSGUILOGIC_HEADER_

#include <string>
#include <set>
#include "cdlgmessageedit.h"

// Click action definition.
// In box click action.
#define kszCAInBox "CDlgMessageFrame_inbox"
// Send box click action.
#define kszCASentBox "CDlgMessageFrame_sentbox"
// Out box click action.
#define kszCAOutBox "CDlgMessageFrame_outbox"
// Draft box click action.
#define kszCADraftBox "CDlgMessageFrame_draftbox"

class CMsgUILogic
{
public:
    static CMsgUILogic * GetInstance();

    static void ReleaseInstance();

public:
    //获取当前邮件箱类型
    yl::string GetBoxAction()
    {
        return m_strMsgBoxAction;
    };
    //设置当前进入的邮件箱类型
    void SetBoxAction(const yl::string & strAction)
    {
        m_strMsgBoxAction = strAction;
    };

    //当前短信id
    int GetCurrentMsgId()
    {
        return m_iMsgCurrentId;
    };
    void SetCurrentMsgId(int iMsgid)
    {
        m_iMsgCurrentId = iMsgid;
    };

    //用来标识当前进入的view窗口是当单独打开的还是从menu中打开的
    bool GetIfOnePage()
    {
        return m_bOnePage;
    };
    void SetOnePage(bool isOnePage)
    {
        m_bOnePage = isOnePage;
    };

    //加载第page页的短信列表
    bool LoadMsgList(int page);

    bool GetMsgList(int iPage, TextMsgSimpListData * sMsgList);

    //更新短信列表
    bool UpdateMsgList();

    //进入新消息界面
    void EnterNewMessage();
    //进入Read Message界面
    void EnterViewMessage(int iMsgIdx);
    void EnterMsgListBox(int iMsgIdx);

    //退出
    void ExitMessageDetail();

    /////////////////////////////////////
    //删除一条短信
    void DeleteMessage(int iMsgIdx);

    //清空指定收件箱中的短信
    void ClearCurrentMsgBoxMessages();

    //发送短信
    bool DoSendMessage(const QString & strToAddr, int nFromAccountId, CDlgMessageEdit * pWnd = NULL);

    //保存草稿
    void SaveDraftMessage(int iMsgIdx, bool bUpdateCont = true, bool bUpdateAddrs = true,
                          CDlgMessageEdit * pWnd = NULL);

    // Get message detail info by click action and index.
    // | pData | [out] is the buffer.
    // | strClickAction | is the click action.
    // | nIndex | is the index the message in the group.
    // Return true if Retrieved successfully, otherwise return false.
    bool GetMessageDetail(TextMsgItemDetailData * pData,
                          const yl::string & strClickAction, int nIndex) const;

private:
    CMsgUILogic();
    ~CMsgUILogic();

private:
    static CMsgUILogic * s_pInstance;

private:
    yl::string m_strMsgBoxAction;//收件箱Action
    int m_iMsgListPageIdx; //短信列表当前页数
    int m_iMsgCurrentId; //当前短信id
    bool m_bOnePage;//当前view窗口是否从外部打开
};

#define g_pMsgUILogic (CMsgUILogic::GetInstance())

#endif
