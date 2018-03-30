#include "popmenu.h"
#include "t4xpicpath.h"
#include "uicommon.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "keymap.h"
#include "imagemanager/modimagemanager.h"
#include <QtGui>

#define OPTION_ITEM_HEIGHT  36

/////////////////////////////////////////////////////////////////
// CPopMenu
CPopMenu::CPopMenu(QWidget * parent)
    : QMenu(parent)
    , m_bAutoHide(true)
{
    this->setAutoFillBackground(true);
    QPixmap pixBkg = THEME_GET_BMP(PIC_POPMENU_BKG);
    this->setMask(pixBkg.mask());
}

CPopMenu::~CPopMenu()
{

}

void CPopMenu::showEvent(QShowEvent * e)
{
    m_bAutoHide = true;
    QMenu::showEvent(e);
}

void CPopMenu::hideEvent(QHideEvent * e)
{
    QMenu::hideEvent(e);

    if (m_bAutoHide)
    {
        OnCancelTriggered();
    }
}

void CPopMenu::keyPressEvent(QKeyEvent * e)
{
    if (e->key() == PHONE_KEY_OK)
    {
        OnOkTriggered();
    }
    else
    {
        QMenu::keyPressEvent(e);
    }
}

void CPopMenu::GetSoftKeyData(CArraySoftKeyBarData & sftKeyArr)
{
    sftKeyArr.clear();

    CSoftKeyBarData tmpSoftKey = CSoftKeyBarData();

    for (int i = 0; i < 4; ++i)
    {
        sftKeyArr.push_back(tmpSoftKey);
        sftKeyArr[i].m_strSoftkeyAction.clear();
        sftKeyArr[i].m_strSoftkeyTitle.clear();
        sftKeyArr[i].m_strSlotName.clear();
        sftKeyArr[i].m_pReceiver = NULL;
    }

    //初始化第1个softkey数据
    sftKeyArr[0].m_strSoftkeyAction = "Cancel";
    sftKeyArr[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_CANCEL);
    sftKeyArr[0].m_strSlotName = SLOT(OnCancelTriggered());
    sftKeyArr[0].m_pReceiver = this;

    //初始化第4个softkey数据
    sftKeyArr[3].m_strSoftkeyAction = "OK";
    sftKeyArr[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_OK);
    sftKeyArr[3].m_strSlotName = SLOT(OnOkTriggered());
    sftKeyArr[3].m_pReceiver = this;
}

bool CPopMenu::AddAction(const QString & strTitle, const QString & strStrAction)
{
    if (strStrAction.isEmpty()
            || m_actionMap.contains(strStrAction))
    {
        BASEUI_WARN("Action(%s) already exist", strStrAction.toUtf8().data());
        return false;
    }

    QAction * pAction = addAction(strTitle, strStrAction);

    if (NULL == pAction)
    {
        return false;
    }
    m_actionMap[strStrAction] = pAction;

    //设置默认选中项
    if (m_actionMap.size() == 1)
    {
        setDefaultAction(pAction);
    }

    return true;
}

bool CPopMenu::RemoveAction(const QString & strStrAction)
{
    if (strStrAction.isEmpty()
            || !m_actionMap.contains(strStrAction))
    {
        BASEUI_WARN("Action(%s) no exist", strStrAction.toUtf8().data());
        return false;
    }

    for (ActionSeq::Iterator iter = m_actionSeq.begin();
            iter != m_actionSeq.end(); ++iter)
    {
        if (*iter == strStrAction)
        {
            m_actionSeq.erase(iter);
            break;
        }
    }

    disconnect(this, SIGNAL(ActionTrigger(const QString &)));
    QAction * pAction = m_actionMap[strStrAction];
    this->removeAction(pAction);
    m_actionMap.remove(strStrAction);

    // 释放action内存
    if (NULL != pAction)
    {
        pAction->deleteLater();
        pAction = NULL;
    }

    return true;
}

bool CPopMenu::ClearActions()
{
    ActionMap::iterator iter;
    while (!m_actionMap.isEmpty())
    {
        iter = m_actionMap.begin();
        RemoveAction(iter.key());
    }

    return true;
}

bool CPopMenu::SetActionVisible(const QString & strStrAction, bool bVisible)
{
    if (strStrAction.isEmpty()
            || !m_actionMap.contains(strStrAction))
    {
        BASEUI_WARN("Action(%s) no exist", strStrAction.toUtf8().data());
        return false;
    }

    if (m_actionMap[strStrAction] != NULL)
    {
        m_actionMap[strStrAction]->setVisible(bVisible);
        return true;
    }

    return false;
}

void CPopMenu::Connect(QObject * pReceiver, const char * szSlot)
{
    disconnect(this, SIGNAL(ActionTrigger(const QString &)));
    if (NULL != pReceiver
            && NULL != szSlot)
    {
        connect(this, SIGNAL(ActionTrigger(const QString &)), pReceiver, szSlot);
    }
}

void CPopMenu::SetFocusAction(const QString & strAction)
{
    ActionMap::Iterator iter = m_actionMap.find(strAction);
    if (iter != m_actionMap.end())
    {
        setActiveAction(iter.value());
    }
}

void CPopMenu::FocusFirstAction()
{
    if (!m_actionSeq.empty())
    {
        SetFocusAction(m_actionSeq[0]);
    }
}

QSize CPopMenu::sizeHint() const
{
    int h = OPTION_ITEM_HEIGHT * m_actionSeq.size();
    return QSize(240, h);
}

QAction * CPopMenu::addAction(const QString & strTitle, const QString & strStrAction)
{
    QAction * pAction = new QAction(LANG_TRANSLATE(strTitle), this);
    if (pAction == NULL)
    {
        return NULL;
    }

    QMenu::addAction(pAction);

    pAction->setData(strStrAction);

    connect(pAction, SIGNAL(triggered()), this, SLOT(OnActionTriggered()));

    m_actionSeq.push_back(strStrAction);

    return pAction;
}

void CPopMenu::OnOkTriggered()
{
    QAction * pAction = activeAction();

    if (pAction != NULL)
    {
        m_bAutoHide = false;

        hide();
        emit MenuHide();

        pAction->trigger();
    }
}

void CPopMenu::OnActionTriggered()
{
    QAction * pAction = qobject_cast<QAction *>(sender());

    if (NULL != pAction)
    {
        emit ActionTrigger(pAction->data().toString());
    }
}

void CPopMenu::OnCancelTriggered()
{
    hide();

    emit CanceleTriggered();
}

/////////////////////////////////////////////////////////////////
