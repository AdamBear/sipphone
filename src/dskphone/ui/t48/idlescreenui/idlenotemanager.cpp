#include "idlenotemanager.h"
#include "baseidlepopboxinfo.h"
#include "dsklog/log.h"
#include "qtcommon/qdatatimer.h"

//mod-UIManager
#include "uimanager/uimanager_inc.h"
#include "uimanager/uimanager_header.h"

//mod-MainWnd
#include "mainwnd/mainwnd.h"
#include "mainwnd/autovolumebar.h"

#include "idlescreen/include/modidlescreen.h"
#include "uicommon/messagebox/messageboxbase.h"
#include "uicommon/messagebox/modmessagebox.h"
#include "uikernel/languagehelper.h"

#include "xmlbrowserui/modxmlbrowserui.h"

#ifndef SAFE_DELETE
#   define  SAFE_DELETE(p) if (p != NULL) {delete p; p=NULL;}
#endif

#define MAX_ICON_SIZE   5

// 内部函数定义
void NoteManager_PopboxBoxCallBackFunction(const YLVector<IdleStatusItem> & ylVecStatusList)
{
    IDLEUI_INFO("IdlePopupBox Call Back PopboxBoxCallBackFunction");
    GetIdleNoteManagerInstance().CheckPopbox();
}
CIdlePopboxManager & GetIdleNoteManagerInstance()
{
    static CIdlePopboxManager s_IdlePopboxManager;
    return s_IdlePopboxManager;
}
CIdlePopboxManager::CIdlePopboxManager()
    : m_pPopupboxProxy(NULL)
{
    // 注册逻辑层回调函数
    ////----qt 需要和逻辑层确认修改接口
    m_bLock = false;
    idleScreen_SetNotifyFunByCount(NoteManager_PopboxBoxCallBackFunction, MAX_ICON_SIZE);//
}

CIdlePopboxManager::~CIdlePopboxManager()
{
    SAFE_DELETE(m_pPopupboxProxy);//delete掉当前在显示的pop窗口代理
}
void CIdlePopboxManager::ShowPopupBox(int eStatus)
{
    // 解锁弾框
    Unlock();

    if (PS_STATE_NONE == eStatus
            || !IsCanPopup())
    {
        return;
    }
    int nPopupType = eStatus;//获取当前需要弹窗的类型
    IDLEUI_INFO("CIdleNoteManager::ShowPopupBox PopupType(%d)", nPopupType);

    if (IsShouldHideVolumebar(nPopupType))
    {
        // 隐藏音量条
        _MainWND.SetVolumeBarVisible(false);
    }

    //lcm代理类主要是用来获取信息
    CBaseIdlePopupboxProxy * pPopupboxProxy = CPopupboxProxyFactory::CreateClassByType(
                nPopupType); //根据类型来生成具体的类
    //只有NetWorKOK_BUT_CONFILICT 才会前后设置不成功 后面要改整个都改
    if (NULL != pPopupboxProxy)
    {
        pPopupboxProxy->SetType(nPopupType);
    }
    int nResult = MessageBox_Destory;
    if (NULL != pPopupboxProxy
            && pPopupboxProxy->PreparePopupboxInfo())
    {

        SAFE_DELETE(m_pPopupboxProxy);

        m_pPopupboxProxy = pPopupboxProxy;

        // 更改状态为正在弹.
        idleScreen_SetPopupStatus(nPopupType, POPUP_WINDOW_POPUPING);
        m_pPopupboxProxy->PopupBox();//
        //对话框已经整改为非模态，所以关闭对话框后的处理需要改
        //m_pPopupboxProxy->ProcessFinishPopupBox(nResult);//lcm处理模态对话框的返回，也就是关闭了这个模态对话框的后续动作
        //if (nResult == PopupBox_Destory)
        //{
        //  IDLEUI_INFO("[IdleNote] PopupBox is destoried. Reset popupbox type.");
        //  SAFE_DELETE(m_pPopupboxProxy);
        //}
    }
    else
    {
        IDLEUI_ERR(" PreparePopupboxInfo error. type[%d]", nPopupType);
        // 异常情况需要释放资源
        SAFE_DELETE(m_pPopupboxProxy);
        //没有信息可供提示
        idleScreen_ClearPopupFlag(nPopupType);
    }

    //判断xml有无弹出提示标记，解决提示闪动问题
    if (!IsTipPopup()
            && IsCanPopup()
            && nResult != MessageBox_Close)//lcm暂时留着，防止没有显示成功，恢复原来的弹出框
    {
        //检查是否还有别的弹窗。
        CheckPopbox();
    }
}

//关闭弹窗 //lcm这个和外面的关系起来了

void CIdlePopboxManager::ClosePopupBox(int nCloseReason)
{

    if (IsTipPopup())//lcm当前有对话框在显示
    {
        //换新接口
        IDLEUI_INFO("CIdleNoteManager::ClosePopupBox[%d], count[%d]", nCloseReason,
                    MessageBox_GetMessageBoxCount());
        MessageBox_RemoveAllMessageBox((MESSAGEBOX_RESULT)
                                       nCloseReason);//lcm这边会直接关闭最顶层弹出框，最新的还未显示

        if (idleScreen_GetPopupStatus(GetCurrentPopupType()) == POPUP_WINDOW_POPUPING)//lcm当前正在显示
        {
            idleScreen_SetPopupStatus(GetCurrentPopupType(),
                                      POPUP_WINDOW_POPUP);//lcm设置此类型的弹出框为已经弹出。
        }
    }
}

//检查是否还有别的弹窗。  lcm每次状态改变回调都会调用这个函数，明显需要先检查是否需要弹出框
void CIdlePopboxManager::CheckPopbox()
{
    IDLEUI_INFO("CIdlePopboxManager::CheckPopbox");
    if (!IsCanPopup())//lcm是否能够弹窗，是否是idle界面，是否被lock，l
    {
        IDLEUI_INFO("CIdleNoteManager::CheckPopbox can not popup msgbox!");
        return;
    }

    // T46容错，解决重启话机后弹出框没有焦点的问题
    if (NULL == QApplication::activeWindow())
    {
        IDLEUI_INFO("CheckPopbox setActiveWindow");
        QApplication::setActiveWindow(&_MainWND);
    }

    IdleStatusItem item = idleScreen_GetTopStatusPopType();
    PhoneState eStatus = (PhoneState)item.m_nId;
    IDLEUI_INFO("CheckPopbox The PopupState is   %d.", eStatus);

    //对网络失败状态进行防抖
    if (!CheckNetWorkFailed(eStatus))//前后两次的网络失败提示是一样的
    {

        IDLEUI_INFO("CheckNetWorkFailed fail !");
        return ;
    }


    //符合条件下调用控制层弹窗
    if (eStatus != PS_STATE_NONE)
    {

        ClosePopupBox(MessageBox_Close);
        // 弹出过程不处理刷新
        Lock();

        // 不延时处理会导致下个界面未绘制完成就弹出模态对话框导致界面异常
        /*QDataTimer::singleShot(10, this, SLOT(ShowPopupBox(const QVariant&)),
            QVariant::fromValue((int)eStatus));*/
        ShowPopupBox(eStatus);
    }
    else//顶层的弹出状态消失，关闭正在弹出的提示框
    {
        ClosePopupBox(MessageBox_Close);
    }
}
bool CIdlePopboxManager::CheckNetWorkFailed(int eStatus)
{
    IDLEUI_INFO(" CIdlePopboxManager::CheckNetWorkFailed ");
    if (PS_STATE_NETWORK_FAILED == eStatus && m_pPopupboxProxy != NULL &&
            PS_STATE_NETWORK_FAILED == m_pPopupboxProxy->GetType())
    {
        NETWORK_STATUS CNWState = idleScreen_GetNetworkStatus();
        if (NS_ALL_IP_CONFLICT == CNWState)
        {
            if (((CNetworkWorkFailedPopboxProxy *)m_pPopupboxProxy)->GetPopBoxData().m_strNote ==
                    LANG_TRANSLATE(TRID_ALL_IP_CONFLICT))
            {
                return  false;//当前正在显示的弹出框
            }

        }
        else if (NS_IPV4_CONFLICT == CNWState)
        {
            if (((CNetworkWorkFailedPopboxProxy *)m_pPopupboxProxy)->GetPopBoxData().m_strNote ==
                    LANG_TRANSLATE(TRID_IPV4_CONFLICT))
            {
                return  false;//当前正在显示的弹出框
            }
        }
        else if (NS_IPV6_CONFLICT == CNWState)
        {
            if (((CNetworkWorkFailedPopboxProxy *)m_pPopupboxProxy)->GetPopBoxData().m_strNote ==
                    LANG_TRANSLATE(TRID_IPV6_CONFLICT))
            {
                return  false;//当前正在显示的弹出框
            }
        }
        else if (NS_INVALID_MAC == CNWState)
        {
            if (((CNetworkWorkFailedPopboxProxy *)m_pPopupboxProxy)->GetPopBoxData().m_strNote ==
                    LANG_TRANSLATE(TRID_NEED_UPDATE_MAC))
            {
                return false;//当前正在显示的弹出框
            }
        }
        else if (NS_UPDATING == CNWState)
        {
            if (((CNetworkWorkFailedPopboxProxy *)m_pPopupboxProxy)->GetPopBoxData().m_strNote ==
                    LANG_TRANSLATE(TRID_UPDATING_NETWORK))
            {
                return false;//当前正在显示的弹出框
            }
        }
        else if (NS_REQUEST_IP == CNWState)
        {
            if (((CNetworkWorkFailedPopboxProxy *)m_pPopupboxProxy)->GetPopBoxData().m_strNote ==
                    LANG_TRANSLATE(TRID_OBTAINING_IP_ADDRESS))
            {
                return false;//当前正在显示的弹出框
            }
        }
        else
        {
            if (((CNetworkWorkFailedPopboxProxy *)m_pPopupboxProxy)->GetPopBoxData().m_strNote ==
                    LANG_TRANSLATE(TRID_NETWORK_UNAVAILABLE))
            {
                return false;//当前正在显示的弹出框
            }
        }
    }
    return true;
}

bool CIdlePopboxManager::IsShouldHideVolumebar(int nStatus)
{
    if (PS_STATE_PICKUP_MENU == nStatus)
    {
        return false;
    }

    return true;
}

// 是否允许弹出
bool CIdlePopboxManager::IsCanPopup()
{
    bool bCanPopup = UIManager_IsInIdle() && !Islock() && !XmlBrowserUI_IsPopuping();
    return bCanPopup;
}

bool CIdlePopboxManager::IsTipPopup() //是否有idle弹出的弹窗在显示
{
    return ((MessageBox_GetMessageBoxCount() > 0) && (m_pPopupboxProxy != NULL));
}

int CIdlePopboxManager::GetCurrentPopupType()//当前正在显示的弹窗的类型。
{
    return (NULL == m_pPopupboxProxy ?
            PS_STATE_NONE : m_pPopupboxProxy->GetType());
}

void CIdlePopboxManager::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (pMessageBox == NULL || m_pPopupboxProxy == NULL)
    {
        return ;
    }

    //将上PopUp()函数后面的对关闭的处理都整下来
    int nResult = pMessageBox->GetResult();
    IDLEUI_INFO("CIdlePopboxManager::MessageBoxCallBack  nResult %d ", nResult);
    m_pPopupboxProxy->ProcessFinishPopupBox(nResult);
    SAFE_DELETE(m_pPopupboxProxy);//关闭弹窗之后将代理销毁掉
    //按ok canel 等操作关闭一个弹出框之后需要重新检查是否还要弹出框，
    //人为的closePopup（MessageBox_Close）和通话关闭MessageBox_Destory是不要检测，
    //因为调用这两者的意图就是清除界面上的所有弹出框

    if (!IsTipPopup()
            && IsCanPopup()
            && nResult != MessageBox_Close && nResult != MessageBox_Destory)
    {
        //检查是否还有别的弹窗
        CheckPopbox();
    }
}
bool CIdlePopboxManager::IsUrgentMsgBox(CMessageBoxBase * pMessageBox)
{
    if (pMessageBox == NULL || m_pPopupboxProxy == NULL)
    {
        return true;
    }

    return m_pPopupboxProxy->IsUrgentMsgBox(pMessageBox->GetID());
}
