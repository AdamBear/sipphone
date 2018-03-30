/************************************************************************
 * FileName  : CAnywhereUILogic.cpp (implementation file)
 * Purpose   :
 * Date      : 2012/12/19 11:46:49
 ************************************************************************/

#include "canywhereuilogic.h"
#include "broadsoft/anywhere/include/anywherecontroller.h"
#include "uimanager/moduimanager.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "uikernel/languagehelper.h"
#include "settingui/include/modsettingui.h"

#include "cdlganywherelocationlist.h"
#include "cdlganywherelocationedit.h"
#include "cdlganywherelocationnew.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "modsettingui.h"
#include "messagebox/modmessagebox.h"
#include "globalmodel.h"
#include "settinguicommon.h"

namespace
{
enum
{
    MSGBOX_ID_REQUEST_LOCATIONLIST = 100,
    MSGBOX_ID_REQUEST_LOCATIONDETAIL,
    MSGBOX_ID_DELETE_LOCATION,
    MSGBOX_ID_DELETE_ALLLOCATION,
    MSGBOX_ID_ADD_LOCATION,
    MSGBOX_ID_MODIFY_LOCATION,
    MSGBOX_ID_MODIFY_MUTILLOCATION,
    MSGBOX_ID_QUERY_LOCATIONEXIST,
    MSGBOX_ID_REQUEST_LOCATIONLIST_AUTHOR_ERROR,
    MSGBOX_ID_REQUEST_LOCATIONLIST_XSI_EMPTY,
};
}

//static CDlgMessageBox* gs_pMsgBox = NULL;

// 处理消息.
static LRESULT Anywhere_OnMessage(msgObject & refMessage)
{
    SETTINGUI_INFO("CAnywhereUILogic_OnMessage(MSG:%0x, wParam:%d, lParam:%d)",
                   refMessage.message, refMessage.wParam, refMessage.lParam);

    switch (refMessage.message)
    {
    case XSI_MESSAGE_REQ_FINISH:
        {
            if (XSI_ANYWHERE_GET_LOCATION_LIST == refMessage.wParam)
            {
                g_pAnywhereUILogic->ResponseLocationListDownload();
            }
            else if (XSI_ANYWHERE_GET_LOCATION_DETAIL == refMessage.wParam)
            {
                g_pAnywhereUILogic->ResponseGetLocationDetail();
            }
            else if (XSI_ANYWHERE_DELETE_LOCATION == refMessage.wParam)
            {
                g_pAnywhereUILogic->ResponseDeleteLocation();
            }
            else if (XSI_ANYWHERE_MODIFY_LOCATION == refMessage.wParam
                     || XSI_ANYWHERE_MODIFY_LOCATIONS == refMessage.wParam
                     || XSI_ANYWHERE_ADD_LOCATION == refMessage.wParam)
            {
                g_pAnywhereUILogic->ResponseEditLocation();
            }
        }
        break;
    case XSI_MESSAGE_REQ_FAIL:
        {
            if (XSI_ANYWHERE_GET_LOCATION_LIST == refMessage.wParam
                    || XSI_ANYWHERE_GET_LOCATION_DETAIL == refMessage.wParam
                    || XSI_ANYWHERE_DELETE_LOCATION == refMessage.wParam
                    || XSI_ANYWHERE_MODIFY_LOCATION == refMessage.wParam
                    || XSI_ANYWHERE_MODIFY_LOCATIONS == refMessage.wParam
                    || XSI_ANYWHERE_ADD_LOCATION == refMessage.wParam)
            {
                SXsiExtenInfo * pErrInfo = (SXsiExtenInfo *)refMessage.GetExtraData();

                if (NULL != pErrInfo)
                {
                    g_pAnywhereUILogic->ResponseActionError(refMessage.lParam, pErrInfo->m_nError);
                }
            }
        }
        break;
    default:
        break;
    }

    return TRUE;
}

CAnywhereUILogic * CAnywhereUILogic::s_pInst = NULL;

CAnywhereUILogic * CAnywhereUILogic::GetInstance()
{
    if (NULL == s_pInst)
    {
        s_pInst = new CAnywhereUILogic();
    }

    return s_pInst;
}

void CAnywhereUILogic::DestoryInstance()
{
    if (NULL != s_pInst)
    {
        delete s_pInst;
        s_pInst = NULL;
    }
}

CAnywhereUILogic::CAnywhereUILogic()
    : QObject()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN,
                          XSI_MESSAGE_END, Anywhere_OnMessage);

    connect(this, SIGNAL(requestLocationList(bool)), this, SLOT(RequestLocationList(bool)),
            Qt::QueuedConnection);

    m_strModifyPhoneNum = "";

}

CAnywhereUILogic::~CAnywhereUILogic()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN,
                            XSI_MESSAGE_END, Anywhere_OnMessage);
}

void CAnywhereUILogic::Exit()
{
    AnyWhere_CancelAllRequest();
    //BaseUI_DestroyPage();
}

bool CAnywhereUILogic::IsAnywhereActive()
{
    //CDlgAnywherePage* pDlg = getDlgAnywherePage();
    //CDlgSubMenu* pSubMenu = g_pSubMenuPointManager->GetSubMenuPoint();
    //// 由于处理消息的时候有可能存在CDlgAnywherePage是CurrentPage还未显示出来
    //// 就收到消息了,所以用SubMenu来判断
    //return pDlg != NULL && pSubMenu != NULL && !pSubMenu->isHidden();

    return IsLocationListPageActive() || IsNewLocationPageActive() || IsLocationEditPageActive();
}

bool CAnywhereUILogic::IsLocationListPageActive()
{
    CDlgAnyWhereLocationList * pDlg = getDlgLocationListPage();
    // 由于处理消息的时候有可能存在LocationList是CurrentPage还未显示出来就收到消息了
    return pDlg != NULL && pDlg->isActiveWindow();
}
bool CAnywhereUILogic::IsNewLocationPageActive()
{
    CDlgAnyWhereLocationNew * pDlg = getDlgLocationNewPage();
    // 由于处理消息的时候有可能存在LocationList是CurrentPage还未显示出来就收到消息了
    return pDlg != NULL && pDlg->isActiveWindow();
}

bool CAnywhereUILogic::IsLocationEditPageActive()
{
    CDlgAnyWhereLocationEdit * pDlg = getDlgLocationEditPage();
    // 由于处理消息的时候有可能存在LocationList是CurrentPage还未显示出来就收到消息了
    return pDlg != NULL && pDlg->isActiveWindow();
}

QString CAnywhereUILogic::FormatContryCodePhoneNumber(const yl::string & strCountryCode,
        const yl::string & strPhoneNum)
{
    QString strResult;
    if (!strCountryCode.empty())
    {
        strResult.append("+");
        strResult.append(strCountryCode.c_str());
        strResult.append("-");
    }
    strResult.append(strPhoneNum.c_str());

    return strResult;
}

bool CAnywhereUILogic::SplitCountryCodePhoneNumber(const QString & strCountPhone,
        yl::string & strCountryCode,
        yl::string & strPhoneNum)
{
    strPhoneNum.clear();
    strCountryCode.clear();

    if (strCountPhone.isEmpty())
    {
        return false;
    }

#if 1
    strPhoneNum = strCountPhone.toUtf8().data();
    return true;
#else
    int pos = strCountPhone.indexOf("-");
    if (pos > 0)
    {
        //格式：+countrycode-phonenumber

        strPhoneNum = strCountPhone.right(strCountPhone.size() - pos - 1).toUtf8().data();

        if (strCountPhone[0] == '+')
        {
            strCountryCode = strCountPhone.mid(1, pos - 1).toUtf8().data();
        }
        else
        {
            strCountryCode = strCountPhone.left(pos).toUtf8().data();
        }

        return true;
    }
    else
    {
        //没有countrycode
        if (strCountPhone[0] == '+')
        {
            return false;
        }
        else
        {
            strPhoneNum = strCountPhone.toUtf8().data();
            return true;
        }
    }
#endif
}

//处理XSI密码
void CAnywhereUILogic::OnSubmitXSIPassword(const yl::string & strPswd)
{
    int lineId = AnyWhere_GetLineID();
    BWCallControl_SetAccountXSIPassword(strPswd.c_str(), lineId);
    QTimer::singleShot(20, this, SLOT(RequestLocationList()));
}

void CAnywhereUILogic::OnCancelInputXSIPassword()
{
    BackToPrePage();
}

bool CAnywhereUILogic::BackToPrePage()
{
    CDlgAnyWhereLocationList * pDlgList = getDlgLocationListPage();
    if (pDlgList != NULL)
    {
        //回到上一层界面
        pDlgList->OnCustomBack();
    }
    else
    {
        CDlgAnyWhereLocationEdit * pDlgEdit = NULL;
        // 获取编辑界面
        pDlgEdit = getDlgLocationEditPage();
        if (pDlgEdit != NULL)
        {
            //回到上一层界面
            pDlgEdit->OnCustomBack();
        }
        else
        {
            CDlgAnyWhereLocationNew * pDlgNew = NULL;
            // 获取新建页面
            pDlgNew = getDlgLocationNewPage();
            if (pDlgNew != NULL)
            {
                //回到上一层界面
                pDlgNew->OnCustomBack();
            }
        }
    }
    return true;
}

bool CAnywhereUILogic::RequestLocationList(bool bShowMsgBox/* = true*/)
{
    int nLineId = AnyWhere_GetLineID();
    if (!BWCallControl_IsAccountXSIEnable(nLineId))
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_XSI_INFORMATION_IS_INCOMPLETE),
                                    MESSAGEBOX_NOTE, 1500, "", "",
                                    MSGBOX_ID_REQUEST_LOCATIONLIST_XSI_EMPTY);
        return false;
    }

    if (ProcessMsgBoxPassword(nLineId))
    {
        return false;
    }

    if (!AnyWhere_RequestAllLocationList())
    {
        return false;
    }

    if (bShowMsgBox && MessageBox_GetMessageBoxCount() == 0)
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel, -1, "", "",
                                    MSGBOX_ID_REQUEST_LOCATIONLIST);
    }

    return true;
}

bool CAnywhereUILogic::RequestAddLocation(const SAYWLocation & sLoct, bool bShowMsgBox/* = true*/)
{
    AnyWhere_RequestAddLocation(sLoct);
    if (bShowMsgBox)
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_SAVING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel, -1, "", "",
                                    MSGBOX_ID_ADD_LOCATION);
    }

    return true;
}

bool CAnywhereUILogic::RequestModifyLocation(const yl::string & strOriginalPhoneNumber
        , const SAYWLocation & sLoct, bool bShowMsgBox/* = true*/)
{
    m_strModifyPhoneNum = "";
    if (strOriginalPhoneNumber != sLoct.m_strPhoneNum)
    {
        AnyWhere_RequestAddLocation(sLoct, true);
        m_strModifyPhoneNum = strOriginalPhoneNumber;
    }
    else
    {
        AnyWhere_RequestModifyLocation(strOriginalPhoneNumber, sLoct);
    }

    if (bShowMsgBox)
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_SAVING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel, -1, "", "",
                                    MSGBOX_ID_MODIFY_LOCATION);
    }

    return true;
}

bool CAnywhereUILogic::RequestDeleteLocation(const yl::string & strPhoneNumber,
        bool bShowMsgBox/* = true*/)
{
    m_strDelNum = strPhoneNumber;
    AnyWhere_RequestDeleteLocation(strPhoneNumber);
    if (bShowMsgBox)
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_DELETING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel, -1, "", "",
                                    MSGBOX_ID_DELETE_LOCATION);
    }

    return true;
}

bool CAnywhereUILogic::RequestDeleteAllLocation(bool bShowMsgBox/* = true*/)
{
    m_strDelNum.clear();
    AnyWhere_RequestDeleteAllLocation();
    if (bShowMsgBox)
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_DELETING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel, -1, "", "",
                                    MSGBOX_ID_DELETE_ALLLOCATION);
    }

    return true;
}

bool CAnywhereUILogic::RequestModifyMutilLocations(CStringVector & strOriginalPhoneNumberList,
        SAYWLocationArray & arry, bool bShowMsgBox /*= true*/)
{
    bool bExecModify = AnyWhere_RequestModifyMutilLocations(strOriginalPhoneNumberList, arry);
    if (bShowMsgBox)
    {
        //是否有执行修改动作，如果没有则提示一秒提示框
        if (!bExecModify)
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SAVING_PLEASE_WAIT), MESSAGEBOX_NOTE,
                                        SHORT_TIME);
        }
        else
        {
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_SAVING_PLEASE_WAIT),
                                        MESSAGEBOX_Cancel, -1, "", "",
                                        MSGBOX_ID_MODIFY_MUTILLOCATION);
        }
    }

    return true;
}

bool CAnywhereUILogic::RequestLocationDetail(const yl::string & strPhoneNumber,
        bool bShowMsgBox /*= true*/)
{
    AnyWhere_RequestLocationDetail(strPhoneNumber);
    if (bShowMsgBox)
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel, -1, "", "",
                                    MSGBOX_ID_REQUEST_LOCATIONDETAIL);
    }

    return true;
}

bool CAnywhereUILogic::RequestQueryLocationExist(const yl::string & strPhoneNumber,
        bool bShowMsgBox /*= true*/)
{
    AnyWhere_CheckNumberExit(strPhoneNumber.c_str());
    if (bShowMsgBox)
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_QUERYING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel, -1, "", "",
                                    MSGBOX_ID_QUERY_LOCATIONEXIST);
    }

    return true;
}

//删除后的回调
void CAnywhereUILogic::ResponseDeleteLocation()
{
    MessageBox_RemoveTopMessageBox(MessageBox_OK);

    m_strDelNum.clear();

#ifdef  _T48
    //t48可能是在edit界面请求删除，要退出eidt界面
    CDlgAnyWhereLocationEdit * pDlgEdit = getDlgLocationEditPage();
    if (NULL != pDlgEdit)
    {
        pDlgEdit->OnCustomBack();
        //返回之后locationlist页面会自动请求刷新
        return;
    }
#endif

    CDlgAnyWhereLocationList * pDlg = getDlgLocationListPage();
    if (pDlg == NULL)
    {
        return;
    }

    //重新获取数据刷新界面
    emit requestLocationList(true);
}

void CAnywhereUILogic::ResponseEditLocation()
{
    // 由于重新请求数据的时候服务器有概率失败
    // 休眠1秒容错
    etlSleep(1000);

    if (!m_strModifyPhoneNum.empty())
    {
        AnyWhere_RequestDeleteLocation(m_strModifyPhoneNum);
        m_strModifyPhoneNum = "";
        return;
    }

    //销毁弹出框同时退出add或edit界面
    MessageBox_RemoveTopMessageBox(MessageBox_OK);

    //T48不会在list界面修改开关，回到list的时候会自动请求数据，无需刷新
#ifndef _T48
    //如果是LocationList界面修改结果返回，重新获取数据
    CDlgAnyWhereLocationList * pDlg = getDlgLocationListPage();
    if (pDlg != NULL)
    {
        RequestLocationList(true);
    }
#endif
}

//号码已存在时的处理
void CAnywhereUILogic::ResponseQueryLocationExist(bool bExist)
{
    SETTINGUI_INFO(" ResponseQueryLocationExist ");
    //号码已存在的情况只会出现在新建界面，编辑界面的号码是不可编辑的

    CDlgAnyWhereLocationNew * pDlg = getDlgLocationNewPage();

    if (pDlg != NULL
            && bExist
            && pDlg->isActiveWindow())
    {
        pDlg->NotifyLocationAlreadyExist();
    }
}

void CAnywhereUILogic::ResponseGetLocationDetail()
{
    MessageBox_RemoveTopMessageBox(MessageBox_OK);

    CDlgAnyWhereLocationEdit * pDlg = getDlgLocationEditPage();

    if (pDlg != NULL)
    {
        SAYWLocation sLocat;
        AnyWhere_GetLocationDetailQueryResult(sLocat);
        pDlg->SetDetailData(sLocat);
    }
}

void CAnywhereUILogic::ResponseActionError(int iLineId, int errCode)
{
    MessageBox_RemoveTopMessageBox(MessageBox_Destory);

    switch (errCode)
    {
    case 0: //号码已存在
        ResponseQueryLocationExist(true);
        return;
    case 401://验证失败
        {
#if IF_BUG_22977
            ProcessMsgBoxPassword(iLineId);
#else
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_YOUR_ACCOUNT_UNAUTHORIZED), MESSAGEBOX_NOTE,
                                        2000);
#endif
            return;
        }
        break;
    default:
        break;
    }

    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_FAILED_PLEASE_RETRY), MESSAGEBOX_NOTE, 2000);
}

void CAnywhereUILogic::ResponseLocationListDownload()
{
    MessageBox_RemoveTopMessageBox(MessageBox_OK);
    CDlgAnyWhereLocationList * pDlg = getDlgLocationListPage();

    if (pDlg != NULL)
    {
        //pDlg->Init();
        SAYWLocationArray arry;
        AnyWhere_GetAllLocationsQueryResultList(arry);
        SETTINGUI_INFO("CAnywhereUILogic::ResponseLocationListDownload: before setpagedata!");

        pDlg->SetLocationList(arry);
    }
}

int CAnywhereUILogic::GetCachedLocationCount() const
{
    return AnyWhere_GetCachedLocationCount();
}

bool CAnywhereUILogic::IsEnableAddLocation() const
{
    return GetCachedLocationCount() < 100;
}

//设置当前LineID
void CAnywhereUILogic::ChangeDefaultAccountID(int nLineID)
{
    AnyWhere_SetLineID(nLineID);
}

//获取location列表界面指针
CDlgAnyWhereLocationList * CAnywhereUILogic::getDlgLocationListPage()
{
    // 获取界面指针
    CDlgAnyWhereLocationList * pAnyList = qobject_cast<CDlgAnyWhereLocationList *>
                                          (UIManager_GetPrivateDlg(DLG_CDlgAnyWhereList));
    return pAnyList;
}

//获取Location编辑界面的指针
CDlgAnyWhereLocationEdit * CAnywhereUILogic::getDlgLocationEditPage()
{
    CDlgAnyWhereLocationEdit * pAnyEdit = qobject_cast<CDlgAnyWhereLocationEdit *>
                                          (UIManager_GetPrivateDlg(DLG_CDlgAnyWhereEdit));
    return pAnyEdit;
}

//获取location新建界面的指针
CDlgAnyWhereLocationNew * CAnywhereUILogic::getDlgLocationNewPage()
{
    CDlgAnyWhereLocationNew * pAnyNew = qobject_cast<CDlgAnyWhereLocationNew *>(UIManager_GetPrivateDlg(
                                            DLG_CDlgAnyWhereNew));
    return pAnyNew;
}

// 处理密码弹窗
bool CAnywhereUILogic::ProcessMsgBoxPassword(int nLineId)
{
    if (XSI_SIPACCOUNT_NOT_AVAILABLE == CBaseHelper::GetXSIStatus(nLineId))
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_SETTUI_TIP_SIP_XSI_AUTHENTICATION_FAILED),
                                    MESSAGEBOX_NOTE, 1500, "", "",
                                    MSGBOX_ID_REQUEST_LOCATIONLIST_AUTHOR_ERROR);
        return true;
    }
    else if (XSI_AUTH_ERROR == CBaseHelper::GetXSIStatus(nLineId))
    {
        SettingUI_PopPasswordDlg(this, nLineId);
        return true;
    }
    else
    {
        return false;
    }
}

void CAnywhereUILogic::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int iRet = pMessageBox->GetResult();
    switch (pMessageBox->GetID())
    {
    case MSGBOX_ID_REQUEST_LOCATIONLIST:
        {
            if (iRet == MessageBox_Cancel)
            {
                AnyWhere_CancelAllRequest();
            }
        }
        break;
    case MSGBOX_ID_REQUEST_LOCATIONLIST_AUTHOR_ERROR:
        {
            BackToPrePage();
        }
        break;
    case MSGBOX_ID_REQUEST_LOCATIONLIST_XSI_EMPTY:
        {
            BackToPrePage();
        }
        break;
    case MSGBOX_ID_REQUEST_LOCATIONDETAIL:
        {
            if (iRet == MessageBox_Cancel)
            {
                AnyWhere_CancelAllRequest();
            }
        }
        break;
    case MSGBOX_ID_DELETE_LOCATION:
        {
            if (iRet == MessageBox_Cancel)
            {
                AnyWhere_CancelAllRequest();
            }
        }
        break;
    case MSGBOX_ID_DELETE_ALLLOCATION:
        {
            if (iRet == MessageBox_Cancel)
            {
                AnyWhere_CancelAllRequest();
            }
        }
        break;
    case MSGBOX_ID_ADD_LOCATION:
        {
            if (iRet == MessageBox_Cancel)
            {
                AnyWhere_CancelAllRequest();
            }
            else if (iRet == MessageBox_OK)
            {
                CDlgAnyWhereLocationNew * pAnyWhereNew = getDlgLocationNewPage();
                if (NULL != pAnyWhereNew)
                {
                    pAnyWhereNew->OnCustomBack();
                }
            }
        }
        break;
    case MSGBOX_ID_MODIFY_LOCATION:
        {
            if (iRet == MessageBox_Cancel)
            {
                AnyWhere_CancelAllRequest();
            }
            else if (iRet == MessageBox_OK)
            {
                CDlgAnyWhereLocationEdit * pAnyWhereEdit = getDlgLocationEditPage();
                if (NULL != pAnyWhereEdit)
                {
                    pAnyWhereEdit->OnCustomBack();
                }
            }
        }
        break;
    case MSGBOX_ID_MODIFY_MUTILLOCATION:
        {
            if (iRet == MessageBox_Cancel)
            {
                AnyWhere_CancelAllRequest();
                RequestLocationList(true);
            }
        }
        break;
    case MSGBOX_ID_QUERY_LOCATIONEXIST:
        {
            if (iRet == MessageBox_Cancel)
            {
                AnyWhere_CancelAllRequest();
            }
        }
        break;
    case MSGBOX_ID_POP_XSI_PWD:
        {
            if (iRet == MessageBox_OK)
            {
                yl::string strPwd = pMessageBox->GetExtraData().toString().toUtf8().data();
                OnSubmitXSIPassword(strPwd);
            }
            else if (iRet == MessageBox_Cancel)
            {
                OnCancelInputXSIPassword();
            }
        }
        break;
    default:
        {

        }
        break;
    }
}
