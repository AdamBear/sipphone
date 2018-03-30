#if IF_FEATURE_METASWITCH_ACD
#include "cdlgmtswacdselectdispcode.h"
#include "messagebox/modmessagebox.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"

#include "baseui/modbaseui.h"
#include "keymap.h"
#include "uikernel/qcomboboxutility.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "uimanager/moduimanager.h"
#include "baseui/t4xpicpath.h"

#include "metaswitch_inc.h"


CDlgMTSWAcdSelectDispCode::CDlgMTSWAcdSelectDispCode(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pComboCode(NULL)
    , m_pComboQueue(NULL)
{
    InitData();
}

CDlgMTSWAcdSelectDispCode::~CDlgMTSWAcdSelectDispCode()
{
}

void CDlgMTSWAcdSelectDispCode::InitData()
{
    //设置标题
    m_strTitle = LANG_TRANSLATE(TRID_MTSW_ACD_LEAVE_WRAPUP);

    m_pComboCode = new QComboBox(this);
    m_pComboQueue = new QComboBox(this);

    //指针判空
    if (m_pComboCode == NULL
            || m_pComboQueue == NULL)
    {
        return;
    }

    qComboBoxUtility::SetComboBoxStyle(m_pComboCode, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pComboQueue, Qt::black);

    m_vecWidgetPair.clear();
    QString strLab = "";
    QWidgetPair qPair;

    strLab = LANG_TRANSLATE(TRID_MTSW_DISPCODE_QUEUE);
    qPair.first = strLab;
    qPair.second = m_pComboQueue;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_MTSW_DISPCODE_CODE);
    qPair.first = strLab;
    qPair.second = m_pComboCode;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair, true, true);

    // 设置下拉框数据
    int iMLHGIndex = 0;
    yl::string strLastAnseredMLHG = MTSWACD_GetLastAnseredMHLG();

    // 获取MLHG数据
    MTSWACD_GetMLHGList(m_vecMLGHData);

    for (int i = 0; i < m_vecMLGHData.size(); ++i)
    {
        if (strLastAnseredMLHG == m_vecMLGHData[i].strName)
        {
            iMLHGIndex = i;
        }
        m_pComboQueue->addItem(toQString(m_vecMLGHData[i].strName.c_str()), i);
    }

    m_pComboQueue->setCurrentIndex(iMLHGIndex);
    OnCmbBoxQueueCurrentIndexChanged(iMLHGIndex);

    //监测queue下拉框选项的改变
    QObject::connect(m_pComboQueue, SIGNAL(currentIndexChanged(int)), this,
                     SLOT(OnCmbBoxQueueCurrentIndexChanged(int)));

    m_pComboCode->installEventFilter(this);
    m_pComboQueue->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

void CDlgMTSWAcdSelectDispCode::GetDispCode()
{

}


// 设置页面的SoftKey
bool CDlgMTSWAcdSelectDispCode::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[3].m_strSoftkeyAction = ACTION_OK;
    objWndData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_OK);
    objWndData[3].m_strIconPath = PIC_SOFTKEY_SAVE;

    return true;
}

// 设置子页面不可编辑时的SoftKey
bool CDlgMTSWAcdSelectDispCode::GetPageDisableSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (NULL == m_pGeneralFrame)
    {
        return false;
    }

    return true;
}

bool CDlgMTSWAcdSelectDispCode::LoadPageData()
{
    return false;
}

bool CDlgMTSWAcdSelectDispCode::SavePageData()
{
    return false;
}

void CDlgMTSWAcdSelectDispCode::showEvent(QShowEvent * e)
{
    OnShow();
}

void CDlgMTSWAcdSelectDispCode::OnShow()
{
}

bool CDlgMTSWAcdSelectDispCode::OnAction(const QString & strAction)
{
    bool bHandled = true;

    if (strAction == ACTION_OK)
    {
        yl::string strMlhgName, strDispCode;

        int nMlhgNameIndex = m_pComboQueue->itemData(m_pComboQueue->currentIndex(),
                             Qt::UserRole).value<int>();
        int nDispCodeIndex = m_pComboCode->itemData(m_pComboCode->currentIndex(),
                             Qt::UserRole).value<int>();

        if (nMlhgNameIndex >= 0
                && nMlhgNameIndex < m_vecMLGHData.size())
        {
            strMlhgName = m_vecMLGHData[nMlhgNameIndex].strName;

            if (MTSWACD_IsDispCodeRequired(strMlhgName)
                    && nDispCodeIndex == 0)
            {
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_MTSW_ACD_SELECT_DISPCODE),
                                            MESSAGEBOX_NOTE, 1500);

                return true;
            }
        }

        nDispCodeIndex --;
        if (nDispCodeIndex >= 0 && nDispCodeIndex < m_vecDispCode.size())
        {
            strDispCode = m_vecDispCode[nDispCodeIndex];
        }
        else
        {
            strDispCode = "";
        }

        MTSWACD_SetDispCode(strDispCode, strMlhgName);

        emit BtnBackClick();
    }
    else
    {
        bHandled = false;
    }

    return bHandled;
}

// 设置子页面前的操作
void CDlgMTSWAcdSelectDispCode::BeforeSetSubPage(void * pData)
{
}

bool CDlgMTSWAcdSelectDispCode::eventFilter(QObject * pObject, QEvent * pEvent)
{
    // 判断事件类型
    switch (pEvent->type())
    {
    case  QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_OK:
                {
                    OnAction(ACTION_OK);
                    return true;
                }
                break;
            default:
                break;
            }
        }
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

bool CDlgMTSWAcdSelectDispCode::IsStatusChanged()
{
    return false;
}

void CDlgMTSWAcdSelectDispCode::OnCmbBoxQueueCurrentIndexChanged(int nIndex)
{
    m_pComboCode->clear();
    m_vecDispCode.clear();

    if (nIndex < m_vecMLGHData.size())
    {
        // 获取code列表
        MTSWACD_GetDispCodeList(m_vecDispCode, m_vecMLGHData[nIndex].strName);
        if (m_vecDispCode.size() == 0)
        {
            m_pComboCode->addItem(LANG_TRANSLATE(TRID_SOFTKEY_MTSW_ACD_NODISPCODE), 0);
        }
        else
        {
            if (MTSWACD_IsDispCodeRequired(m_vecMLGHData[nIndex].strName))
            {
                m_pComboCode->addItem(LANG_TRANSLATE(TRID_SOFTKEY_MTSW_ACD_SELDISPCODE), 0);
            }
            else
            {
                m_pComboCode->addItem(LANG_TRANSLATE(TRID_SOFTKEY_MTSW_ACD_SELDISPCODE_OPTIONAL), 0);
            }

            for (int j = 1; j <= m_vecDispCode.size(); ++j)
            {
                m_pComboCode->addItem(toQString(m_vecDispCode[j - 1].c_str()), j);
            }
        }
    }

    UpdateWnd();
}
#endif
