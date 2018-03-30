#include "modmessageui.h"
#include "cmsguilogic.h"
#include "cdlgmessageedit.h"
#include "cdlgmessageframe.h"
#include "cdlgmessageview.h"
#include "setting/src/textmessagecontroller.h"
#include "uimanager/uimanager_inc.h"

/************************************************************************/
/* 接口  ： MessageUI_RegisterDlgClass()                                 */
/* 功能  ： 实现窗体类注册                                              */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void MessageUI_RegisterDlgClass()
{
    REGISTER_DLG(CDlgMessageEdit);
    REGISTER_DLG(CDlgMessageFrame);
    REGISTER_DLG(CDlgMessageView);
}

/************************************************************************/
/* 接口  ： MessageUI_GotoNewMessageDlg()                                                            */
/* 功能  ： 进入新建消息界面                                         */
/* 参数说明 ：                                                                                                     */
/* 返回值 ： 失败--false 成功--true                                                                     */
/************************************************************************/
void MessageUI_GotoNewMessageDlg()
{
    g_pMsgUILogic->EnterNewMessage();
}

/************************************************************************/
/* 接口  ：MessageUI_GotoInboxMessageDlg                                    */
/* 功能  ： 实现从目录状态进入消息详情界面                              */
/* 参数说明 ：bIsOnlyOne   类型bool true--只有一条新消息          */
/* 返回值 ： 无                                                             */
/************************************************************************/
void MessageUI_GotoInboxMessageDlg(bool bIsOnlyOne)
{
    //打开第一个Message item
    int iIdx = -1;
    if (g_pTextMessageController != NULL
            && (iIdx = g_pTextMessageController->GetFirstUnReadInMessageIndex()) > -1)
    {
        //换成直接打开界面
        if (bIsOnlyOne)
        {
            //进入短信详情
            g_pMsgUILogic->EnterViewMessage(iIdx);
        }
        else
        {
            //进入短信列表
            g_pMsgUILogic->EnterMsgListBox(iIdx);
        }
    }
}

//收到新消息
void MessageUI_ReceivedNewMessage()
{
    g_pMsgUILogic->UpdateMsgList();
}

