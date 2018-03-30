#include "model.h"
#include "cdlgmystatus.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "uimanager/rectdefine.h"
#include "messagebox/modmessagebox.h"
#include "uikernel/languagehelper.h"
#include "presence/include/modpresence.h"

static const int MSGBOX_ID_SAVE_COMPLETED = 102;

CDlgMyStatus::CDlgMyStatus(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_bChangeStatus(false)
{
    InitData();
}

CDlgMyStatus::~CDlgMyStatus()
{
}

void CDlgMyStatus::InitData()
{
    SetWindowTitle(LANG_TRANSLATE(TRID_MY_STATUS));

    if (m_pGeneralFrame != NULL)
    {
        this->setFocusPolicy(Qt::StrongFocus);
        this->setFocus();

        QWidgetPair qPair;
        qPair.eType = SETTING_ITEM_RIDIO;

        qPair.first = LANG_TRANSLATE(TRID_AVAILABLE);
        m_vecWidgetPair.push_back(qPair);
        qPair.first = LANG_TRANSLATE(TRID_AWAY);
        m_vecWidgetPair.push_back(qPair);
        qPair.first = LANG_TRANSLATE(TRID_BUSY);
        m_vecWidgetPair.push_back(qPair);
        qPair.first = LANG_TRANSLATE(TRID_OFFLINE);
        m_vecWidgetPair.push_back(qPair);

        if (m_vecWidgetPair.size() > 0)
        {
            AddWidget(m_vecWidgetPair, true, false, true, false);
        }
    }

    // 设置softkey1为Cancel
    m_SoftkeyData[0].m_strSoftkeyAction = "Back";
    m_SoftkeyData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_CANCEL);

    this->installEventFilter(this);
}

int CDlgMyStatus::GetPresenceStatus()
{
    int iStatusIndex = -1;

    _PRESENCE_TYPE ePresenceType = Presence_GetLocalPresence().ePresenceType;

    if (ePresenceType == _PRESENCE_TYPE_ONLINE)
    {
        iStatusIndex = 0;
    }
    else if (ePresenceType == _PRESENCE_TYPE_AWAY)
    {
        iStatusIndex = 1;
    }
    else if (ePresenceType == _PRESENCE_TYPE_BUSY)
    {
        iStatusIndex = 2;
    }
    else
    {
        // 其他状态都已off表示
        iStatusIndex = 3;
    }

    return iStatusIndex;
}

bool CDlgMyStatus::LoadPageData()
{
    this->setFocus();

    if (m_pGeneralFrame != NULL)
    {
        int nStatusIndex = GetPresenceStatus();
        SetCurrentIndex(nStatusIndex);
        SetConfigedIndex(nStatusIndex);

        return true;
    }
    return false;
}

bool CDlgMyStatus::SavePageData()
{
    if (NULL == m_pListDelegate)
    {
        return false;
    }

    int iStatus = m_pListDelegate->GetCurrentIndex();
    SetConfigedIndex(iStatus);

    // 告知服务器修改状态
    _PRESENCE_TYPE eType = _PRESENCE_TYPE_OFFLINE;
    switch (iStatus)
    {
    case 0:
        eType = _PRESENCE_TYPE_ONLINE;
        break;
    case 1:
        eType = _PRESENCE_TYPE_AWAY;
        break;
    case 2:
        eType = _PRESENCE_TYPE_BUSY;
        break;
    case 3:
        eType = _PRESENCE_TYPE_OFFLINE;
        break;
    default:
        eType = _PRESENCE_TYPE_OFFLINE;
        break;
    }

    // 先提示弹框，再执行切换状态操作
    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_STATUS_REQUESTING), MESSAGEBOX_NOTE, 1000,
                                "", "", MSGBOX_ID_SAVE_COMPLETED);

    // 阻塞，等待调用返回
    m_bChangeStatus = Presence_ReqChangPresence(eType);

    return false;
}

bool CDlgMyStatus::eventFilter(QObject * pObject, QEvent * pEvent)
{
    // 不处理非自身的消息

    if (!CheckEventValid(pObject, pEvent))
    {
        return false;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

void CDlgMyStatus::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    switch (pMessageBox->GetID())
    {
    case MSGBOX_ID_SAVE_COMPLETED:
        {
            if (m_bChangeStatus)
            {
                emit BtnBackClick();
            }
            else
            {
                MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SET_FAILED), MESSAGEBOX_NOTE, SHORT_TIME);

                //重新设置为当前状态
                int nStatusIndex = GetPresenceStatus();
                SelectIndex(nStatusIndex);
                SetConfigedIndex(nStatusIndex);
            }
        }
        break;
    default:
        break;
    }
}
