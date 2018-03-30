#if IF_FEATURE_GENBAND_E911
#include "dlge911setlocation.h"
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "baseui/t4xpicpath.h"
#include "uikernel/languagehelper.h"
#include "include/translateiddefine.h"
#include "include/uicommon.h"
#include "uicommon/globalinputhook/modinputhook.h"
#include "account/include/modaccount.h"

#include "e911locationdelegate.h"
#include "e911address/include/e911address_def.h"
#include "e911address/include/mode911address.h"
#include "actiondefinecommon.h"
#include "modsettingui.h"
#include "e911locationcontroller.h"

namespace
{
#define ACTION_CANCEL           "Cancel"

enum
{
    SCROLL_VIEW_TYPE_NONE = 0,
    SCROLL_VIEW_TYPE_PAGE = 1,      // 数据按页滚动
    SCROLL_VIEW_TYPE_ITEM = 2,      // 数据按项滚动
};

}

CDlgE911SetLocation::CDlgE911SetLocation(QWidget *parent)
    : CDlgBaseSubPage(parent)
    , m_pE911LocationDelegate(NULL)
#ifdef _T48
    , m_bRefreshOnReshow(false)
#endif
{
    InitPageData();
}

CDlgE911SetLocation::~CDlgE911SetLocation()
{

}

void CDlgE911SetLocation::BeforeSetSubPage(void* pData)
{
    SetWindowTitle(LANG_TRANSLATE(TRID_SET_E911_LOCATION));
}

void CDlgE911SetLocation::Uninit()
{
}

void CDlgE911SetLocation::showEvent(QShowEvent* e)
{
}

bool CDlgE911SetLocation::LoadPageData()
{
    SETTINGUI_INFO("CDlgE911SetLocation::LoadPageData");

    this->setFocus();
    return true;
}

//界面初始化
void CDlgE911SetLocation::InitPageData()
{
    SETTINGUI_INFO("CDlgE911SetLocation::InitPageData");

#ifdef _T48
    m_pE911LocationDelegate = new CE911LocationDelegate();
#else
    m_pE911LocationDelegate = new CE911LocationDelegate(m_pGeneralFrame);
#endif

    if (m_pGeneralFrame != NULL && NULL != m_pE911LocationDelegate)
    {
        m_pE911LocationDelegate->setParent(this);
        m_pE911LocationDelegate->SetItemPat(this);
#ifdef _T48
        m_pE911LocationDelegate->BindFramelist(m_pGeneralFrame);
#endif
        m_pE911LocationDelegate->InitSettingListStyle();
        m_pE911LocationDelegate->SetItemParent(this);
        m_pE911LocationDelegate->PreCreateItem();
        m_pListDelegate = m_pE911LocationDelegate;
    }

    QObject::connect(m_pE911LocationDelegate, SIGNAL(listItemAction(int, QString)), this, SLOT(OnListItemAction(int, QString)));
}

bool CDlgE911SetLocation::GetSoftkeyData(CArraySoftKeyBarData& objSoftData)
{
    SETTINGUI_INFO("CDlgE911SetLocation::GetSoftkeyData");

    if (m_pE911LocationDelegate == NULL)
    {
        return false;
    }

    if (objSoftData.size() < SOFTKEY_NUM)
    {
        return true;
    }

    bool bHasPrev = m_pE911LocationDelegate->FocusItemHasParentItem();
    bool bHasNext = m_pE911LocationDelegate->FouceItemHasChildItem();
    if (bHasPrev)
    {
        objSoftData[1].m_strSoftkeyAction = ACTION_CANCEL;
        objSoftData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_CANCEL);
        objSoftData[1].m_strIconPath = PIC_SOFTKEY_CANCEL;
    }

    if (bHasNext)
    {
        objSoftData[3].m_strSoftkeyAction = ACTION_NEXT;
        objSoftData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_NEXT);
        objSoftData[3].m_strIconPath = PIC_SOFTKEY_NEXT;
    }
    else
    {
        objSoftData[3].m_strSoftkeyAction = ACTION_SAVE;
        objSoftData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SAVE);
        objSoftData[3].m_strIconPath = PIC_SOFTKEY_SAVE;
    }


    return false;
}


int CDlgE911SetLocation::ParseIdByClickAction(const yl::string& strClickAction)
{
    SETTINGUI_INFO("CDlgE911SetLocation::ParseIdByClickAction %s", strClickAction.c_str());
    if (strClickAction.empty())
    {
        return -1;
    }

    yl::string strMask = "e911_set_location_x";

    // Parse action and content.
    yl::string strAction;
    yl::string strContent;
    if (!commonAPI_ParseActionInfo(&strAction, &strContent, strClickAction))
    {
        return -1;
    }

    // Get mask without ending x.
    yl::string strMaskNoX = strMask.substr(0, strMask.length() - 1);
    size_t uPosFound = strContent.rfind(strMaskNoX);
    if (uPosFound == yl::string::npos)
    {
        return -1;
    }

    // Find id from content.
    yl::string strContentPart = strContent.substr(uPosFound);
    yl::string strNewMask = strMaskNoX + "%d";
    int nId = -1;
    if (sscanf(strContentPart.c_str(), strNewMask.c_str(), &nId) != 1)
    {
        return -1;
    }

    SETTINGUI_INFO("Parse ID = %d", nId);
    return nId;
}

void CDlgE911SetLocation::SetOpenSubPageAction(const yl::string& strSubPageAction)
{
    // 保存相应的Action
    m_strSubPageAction = strSubPageAction;

    if (!strSubPageAction.empty())
    {
        int nID = ParseIdByClickAction(strSubPageAction);
        if (m_pE911LocationDelegate != NULL)
        {
            m_pE911LocationDelegate->SetPageID(nID);
        }
    }
}

bool CDlgE911SetLocation::OnAction(const QString& strAction)
{
    SETTINGUI_INFO("CDlgE911SetLocation::OnAction, strAction = %s", strAction.toUtf8().data());
    bool bHandel = true;
    if (ACTION_CANCEL == strAction)
    {
        if (m_pE911LocationDelegate != NULL)
        {
            g_E911Controller.QuitSetLoactionPage(m_pE911LocationDelegate->GetPageID());
        }
        else
        {
            emit BtnBackClick();
        }
    }
    else if (ACTION_BACK == strAction)
    {
        emit BtnBackClick();
    }
    else if (ACTION_NEXT == strAction)
    {
        OnNext();
    }
    else if (ACTION_SAVE == strAction)
    {
        OnSave();
    }
    else
    {
        bHandel = false;
    }


    return bHandel;
}

void CDlgE911SetLocation::OnDialogListUpdate(int nFocusIndex)
{
    SETTINGUI_INFO("CDlgE911SetLocation::OnDialogListUpdate, nFocusIndex = %d, IsTopPage[%d]",
                   nFocusIndex, IsTopPage());

    m_pListDelegate->SetCurrentIndex(nFocusIndex);

    if (IsTopPage())
    {
        UpdateWnd();
    }
    else
    {
        m_bRefreshOnReshow = true;
    }
}

void CDlgE911SetLocation::OnDialogListExit()
{
    SETTINGUI_INFO("CDlgE911SetLocation::OnDialogListExit");
    emit BtnBackClick();
}

#ifdef _T48
bool CDlgE911SetLocation::PorcessBackBtnClick()
{

    return false;
}

bool CDlgE911SetLocation::PorcessHomeBtnClick()
{


    emit BtnBackClick();
    UIManager_ReturnToIdle(true);
    return true;
}

bool CDlgE911SetLocation::ProcessFunctionkey(int nKeyCode, bool bPress)
{

    return true;
}

void CDlgE911SetLocation::OnDialogRefreshSoftkey()
{
    emit RefreshSoftkey();
}

bool CDlgE911SetLocation::IsRejectReturnToIdle(RETURN_IDLE_TYPE eReturnIdle/* = RETURN_IDLE_TYPE_NORMAL*/)
{
    return false;
}

void CDlgE911SetLocation::OnReShow()
{
    if (m_bRefreshOnReshow)
    {
        UpdateWnd();
        m_bRefreshOnReshow = false;
    }
    else
    {
        CDlgBaseSubPage::OnReShow();
    }
}
#endif

bool CDlgE911SetLocation::eventFilter(QObject *pObject, QEvent *pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    bool bHandel = false;

    switch (pEvent->type())
    {
    case QEvent::KeyPress:
        {
            // 转换按键
            QKeyEvent *pKeyEvent = static_cast<QKeyEvent*>(pEvent);
            int nKey = pKeyEvent->key();

            switch (nKey)
            {
            case PHONE_KEY_OK:
                {
                    if (m_pE911LocationDelegate != NULL)
                    {
                        if (m_pE911LocationDelegate->FouceItemHasChildItem())
                        {
                            OnNext();
                        }
                        else
                        {
                            OnSave();
                        }
                        bHandel = true;
                    }
                }
                break;
            case PHONE_KEY_CANCEL:
                {
                    bHandel = OnAction(ACTION_BACK);
                }
                break;
            case PHONE_KEY_DOWN:
                {
                    if (m_pGeneralFrame != NULL && m_pE911LocationDelegate != NULL)
                    {
                        m_pE911LocationDelegate->SetScrollViewType(SCROLL_VIEW_TYPE_PAGE | SCROLL_VIEW_TYPE_ITEM);
                        m_pGeneralFrame->OnNotifyScrollNext();
                        bHandel = true;
                    }
                }
                break;
            case PHONE_KEY_UP:
                {
                    if (m_pGeneralFrame != NULL && m_pE911LocationDelegate != NULL)
                    {
                        m_pE911LocationDelegate->SetScrollViewType(SCROLL_VIEW_TYPE_PAGE | SCROLL_VIEW_TYPE_ITEM);
                        m_pGeneralFrame->OnNotifyScrollPrevious();
                        bHandel = true;
                    }
                }
                break;
            default:
                break;
            }
        }
        break;
    default:

        break;
    }

    if (!bHandel)
    {
        if (m_pE911LocationDelegate != NULL)
        {
            m_pE911LocationDelegate->SetScrollViewType(SCROLL_VIEW_TYPE_PAGE);
        }
        bHandel = CDlgBaseSubPage::eventFilter(pObject, pEvent);
    }

    return bHandel;
}

void CDlgE911SetLocation::OnNext()
{
    if (m_pE911LocationDelegate != NULL)
    {
        int nID = m_pE911LocationDelegate->GetFocusItemID();
        SettingUI_EnterE911Setting(nID);
    }
}

void CDlgE911SetLocation::OnSave()
{
    int nLocationID;
    if (m_pE911LocationDelegate != NULL)
    {
        nLocationID = m_pE911LocationDelegate->GetFocusItemID();
    }
    yl::string strAddr;
    e911_GetFullLocationAddr(nLocationID, strAddr);
    e911_SetLocationID(nLocationID, strAddr);

    //退出界面
    OnAction(ACTION_CANCEL);
    //弹框
    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SAVING_CONFIG_FILE),
                                MESSAGEBOX_NOTE, 1500);

    e911_ClearListData();

}
#endif