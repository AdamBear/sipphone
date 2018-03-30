#include "model.h"
#include "cdlgdsskey.h"

#include "uikernel/languagehelper.h"
//#include "setting/dsskeycontroller.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

#include "account/include/accountmanager.h"
#include "setting/src/menu/menucontroller.h"

#include "configiddefine.h"
//#include "dssKey/dsskeyinfodef.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qcomboboxutility.h"
//#include "directory/directorydata.h"
//#include "directory/smartptr.h"
#include "contacts/directory/include/moddirinterface.h"
#include <QString>
#include "qtcommon/qmisc.cpp"
#include "uimanager/moduimanager.h"
#include "account/include/modaccount.h"
#include "commonapi/stringhelperapi.h"
#include "setting/src/commondefine.h"
#include "dsskey/include/moddsskey.h"
#include "dsskey/src/dsskeycommonfunc.h"
#include "dsskeyui/include/moddsskeyui.h"
#include "directoryui/include/moddirectoryui.h"
#include "presence/include/modpresence.h"
#include "wireless/bluetoothNew/include/modbluetooth.h"
#include "edk/include/modedk.h"

namespace
{
#define DSSKEY_KEY "dss_key"
#define DSSKEY_LINE_KEY_ITEM "line_key_item_"
#define DSSKEY_MEMEORY_KEY_ITEM "memory_key_item_"
#define DSSKEY_EXPAND_KEY_ITEM "expand_key_item_"
}

void DlgDsskey_DirectoryCallBack(yl::string strContactName, yl::string strContactNumber)
{
    CDlgDssKey * pDsseyNew = qobject_cast<CDlgDssKey *>(UIManager_GetPrivateDlg(
                                 DLG_CDlgDssKey));
    if (NULL == pDsseyNew)
    {
        return ;
    }
    QString name = toQString(strContactName);
    QString number = toQString(strContactNumber);

    pDsseyNew->writeData(name, number);
}

int CDlgDssKey::ParseIdByClickAction(const yl::string & strClickAction,
                                     const yl::string & strMask)
{
    if (strClickAction.empty() || strMask.empty())
    {
        return -1;
    }

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

    return nId - 1;
}

CDlgDssKey::CDlgDssKey(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCmbBoxAccountID(NULL)
    , m_pCmbBoxKeyType(NULL)
    , m_pCmbBoxType(NULL)
    , m_pEditExt(NULL)
    , m_pEditLabel(NULL)
    , m_pEditValue(NULL)
    , m_pCmbBoxIndex(NULL)
    , m_iDirectoryWriteType(0)
{
    InitData();
}

void CDlgDssKey::SetItemTypeByAction(const yl::string & strAction)
{
    int iMoudleIndex = 0;
    int iIndex = 0;

    //扩展台索引的开始位置
    int iExpIdxSplitPos = strAction.find(';');
    if (iExpIdxSplitPos != yl::string::npos)
    {
        //扩展台索引
        yl::string strExpIdx = strAction.substr(iExpIdxSplitPos + 1,
                                                strAction.length() - iExpIdxSplitPos - 1);
        if (!strExpIdx.empty())
        {
            iMoudleIndex = atoi(strExpIdx.c_str());
        }
    }

    // Memorykey
    iIndex = ParseIdByClickAction(strAction, NS_SETTING::kszCAMaskMemoryKey);
    if (iIndex == INVALID_INDEX)
    {
        // Linekey
        iIndex = ParseIdByClickAction(strAction, NS_SETTING::kszCAMaskLineKey);
        if (iIndex == INVALID_INDEX)
        {
            // 扩展台
            iIndex = ParseIdByClickAction(strAction, NS_SETTING::kszCAMaskExpandKey);
            if (iIndex == INVALID_INDEX)
            {
                SETTINGUI_ERR("SetItemTypeByAction parse failed [%s]", strAction.c_str());
                return;
            }

            m_PageInfo.m_eUIType = DSSKEY_UI_EXPKEY;
        }
        else
        {
            m_PageInfo.m_eUIType = DSSKEY_UI_LINEKEY;
        }
    }
    else
    {
        m_PageInfo.m_eUIType = DSSKEY_UI_MEMORYKEY;
    }

    m_PageInfo.SetIndex(iIndex, iMoudleIndex);
}

void CDlgDssKey::SetOpenSubPageAction(const yl::string & strSubPageAction)
{
    // 保存相应的Action
    m_strSubPageAction = strSubPageAction;

    if (!strSubPageAction.empty())
    {
        //设置当前界面的action
        SetItemTypeByAction(strSubPageAction);
    }
}

void CDlgDssKey::OnOpenDirectory(const QString & str)
{
    g_pMenuController->EnterDirFormDsskey(this);
}

void CDlgDssKey::OnMenuCallback(void * pData)
{
    // 入口参数检查
    if (NULL == pData)
    {
        return;
    }

    DsskeySettingCallbackData * pCallbackData = (DsskeySettingCallbackData *)pData;
    m_pEditLabel->setText(toQString(pCallbackData->strLab));
    m_pEditValue->setText(toQString(pCallbackData->strValue));

    g_pMenuController->ExitDirFormDsskey();
}

//进行Dss Key界面的部分初始化工作
void CDlgDssKey::InitData()
{
    //设置value和extension两个编辑框在不可编辑状态下的字体颜色
    m_pCmbBoxAccountID = new QComboBox(this);
    m_pCmbBoxKeyType = new QComboBox(this);
    m_pCmbBoxType = new QComboBox(this);
    m_pCmbBoxIndex = new QComboBox(this);
    m_pEditExt = new QLineEdit(this);
    m_pEditLabel = new QLineEdit(this);
    m_pEditValue = new QLineEdit(this);

    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxType, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxKeyType, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxAccountID, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxIndex, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditValue, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditExt, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditLabel, Qt::black);

    //为界面上各输入控件安装事件过滤器
    m_pCmbBoxType->installEventFilter(this);
    m_pCmbBoxKeyType->installEventFilter(this);
    m_pCmbBoxAccountID->installEventFilter(this);
    m_pCmbBoxIndex->installEventFilter(this);
    m_pEditValue->installEventFilter(this);
    m_pEditLabel->installEventFilter(this);
    m_pEditExt->installEventFilter(this);

    //this->installEventFilter(this);
}

void CDlgDssKey::AddCmbBoxItemData(bool bKeyEvent)
{
    QComboBox * pCombox = bKeyEvent ? m_pCmbBoxKeyType : m_pCmbBoxType;
    if (pCombox == NULL)
    {
        return;
    }

    pCombox->clear();
    VecCandidateValue & vecDsskeyPairs = bKeyEvent ? m_vecKeyEvent : m_vecKeyList;
    for (VecCandidateValue::iterator it = vecDsskeyPairs.begin(); it != vecDsskeyPairs.end(); ++it)
    {
        pCombox->addItem(LANG_TRANSLATE(it->strValue.c_str()), it->iIndex);
    }
}

void CDlgDssKey::AddAccountIDData()
{
    //获取账号总个数，进行添加
    int iAccountNum = acc_AccountNum();
    QString strIndex;
    QString strDisplayName;

    m_pCmbBoxAccountID->clear();
    for (int i = 0; i < iAccountNum; i++)
    {
        strDisplayName = LANG_TRANSLATE(TRID_LINE) + " " + strIndex.setNum(i + 1);
        m_pCmbBoxAccountID->addItem(strDisplayName, i);
    }
}

void CDlgDssKey::AddIndexData(bool bIndex/* = true*/)
{
    QComboBox * pCombox = m_pCmbBoxIndex;

    // 动态数据，所以添加是否存在显示的判断
    if (IsWidgetInList(pCombox))
    {
        pCombox->clear();
        VecCandidateValue & vecData = m_DssKeyData.m_vecData;
        for (VecCandidateValue::iterator it = vecData.begin(); it != vecData.end(); ++it)
        {
            pCombox->addItem(LANG_TRANSLATE(it->strValue.c_str()), it->iIndex);
        }

        // 强制取消编辑
        if (vecData.size() <= 1)
        {
            pCombox->setEnabled(false);
        }
    }
}

//设置不同选项时，对应显示不同的界面。
void CDlgDssKey::OnCmbBoxTypeCurrentIndexChanged(int nIndex)
{
    IF_TimeTrace;
    int index = m_pGeneralFrame->GetFocusItemIndex();

    BlockSignals(true);

    // 切换类型时其它数据统统清除
    DssKey_Type eType = GetDsskeyType();
    int iType = DT_NA;
    GetWidgetValue(m_pCmbBoxType, iType);

    // 切换上一次的类型
    if (eType == m_OldDssKeyData.m_eType
            || (iType == DT_KEYEVENT && m_pGeneralFrame->GetFocusItemDataIndex() == 0
                && dsskeyuilogic_CheckKeyEventSet(m_PageInfo, m_OldDssKeyData.m_eType)))
    {
        m_DssKeyData = m_OldDssKeyData;

        if (iType == DT_KEYEVENT)
        {
            eType = m_DssKeyData.m_eType;
        }
    }
    else
    {
        m_DssKeyData.Reset();
        m_DssKeyData.m_eType = eType;
        dsskeyuilogic_UpdateData(m_PageInfo, m_DssKeyData);

        // Line的序号和Dsskey位置有关，不能重置
        if (eType != DT_LINE)
        {
            m_DssKeyData.m_iLineIndex = 0;
        }
    }

    m_DssKeyData.m_strLabel.clear();

    m_pCmbBoxAccountID->setCurrentIndex(
        m_pCmbBoxAccountID->findData(m_DssKeyData.m_iLineIndex, Qt::UserRole));

    // 配置控件
    SetViewByType(eType);

    UpdateData();

    UpdateWnd();

    BlockSignals(false);

    m_pGeneralFrame->SelectItemByIndex(index);
}

void CDlgDssKey::OnCmbBoxAccountIDCurrentIndexChanged(int nIndex)
{
    int index = m_pGeneralFrame->GetFocusItemIndex();

    // 修改Line可能需要实时更新，因此先获取当前页面数据
    GetPageData();

    // 切换回原有类型时
    if (m_DssKeyData.m_eType == m_OldDssKeyData.m_eType
            && m_DssKeyData.m_iLineIndex == m_OldDssKeyData.m_iLineIndex)
    {
        m_DssKeyData = m_OldDssKeyData;
    }
    else
    {
        dsskeyuilogic_UpdateData(m_PageInfo, m_DssKeyData);
        m_DssKeyData.m_strLabel.clear();
    }

    BlockSignals(true);
    // 更新数据
    UpdateData();
    BlockSignals(false);

    m_pGeneralFrame->SelectItemByIndex(index);
}

void CDlgDssKey::SetViewByType(DssKey_Type eType)
{
    IF_TimeTrace;
    // 判断当前下拉框选择的值，显示出与之相对的界面
    // 新切换界面时，无论界面是否有显示，为了避免之前界面数据的影响，都要对界面重做初始化处理
    m_pCmbBoxIndex->clear();
    m_pCmbBoxIndex->setCurrentIndex(0);

    SetAllWidgetEnable(true);

    m_pEditValue->clear();
    m_pEditExt->clear();
    m_pEditLabel->clear();

    m_pCmbBoxAccountID->hide();
    m_pCmbBoxIndex->hide();
    m_pEditExt->hide();
    m_pEditLabel->hide();
    m_pEditValue->hide();

    MakeReadyForRelayout();

    // 整个Dsskey都是只读
    DSSKEY_READONLY_MASK eReadOnlyMask = dsskeyuilogic_IsDsskeyReadOnly(m_PageInfo, m_DssKeyData);
    bool bNewType = eType != m_OldDssKeyData.m_eType;
    bool bEnabled = m_bPageEnable && (bNewType || eReadOnlyMask == READONLY_NONE);

    PushQWidgetPair(TRID_TYPE, m_pCmbBoxType);
    m_pCmbBoxType->setEnabled(bEnabled);

    if (dsskeyuilogic_CheckKeyEventSet(m_PageInfo, eType))
    {
        PushQWidgetPair(TRID_KEY_TYPE, m_pCmbBoxKeyType);
        m_pCmbBoxKeyType->setEnabled(bEnabled);

        // 重新切回来了
        if (eType == m_OldDssKeyData.m_eType)
        {
            m_pCmbBoxKeyType->setCurrentIndex(m_pCmbBoxKeyType->findData(eType, Qt::UserRole));
        }
    }
    else
    {
        m_pCmbBoxKeyType->hide();
        // 重置焦点到第一个
        m_pCmbBoxKeyType->setCurrentIndex(0);
    }

    ConfigSingleWidget(m_pCmbBoxAccountID, eType, DSSKEY_ATTR_ITEM_LINE, bEnabled);
    ConfigSingleWidget(m_pCmbBoxIndex, eType, DSSKEY_ATTR_ITEM_INDEX, bEnabled);
    ConfigSingleWidget(m_pEditLabel, eType, DSSKEY_ATTR_ITEM_LABEL, (m_bPageEnable && (bNewType
                       || eReadOnlyMask <= READONLY_EXCEPT_LABEL)));
    ConfigSingleWidget(m_pEditValue, eType, DSSKEY_ATTR_ITEM_VALUE, bEnabled);
    ConfigSingleWidget(m_pEditExt, eType, DSSKEY_ATTR_ITEM_EXT, bEnabled);

    //IF_LOG_TRACE("type=[%d] [%d] [%d]", eType, bEnabled, m_vecWidgetPair.size());

    SetSubPageEnabled(m_bPageEnable && (bNewType || eReadOnlyMask != READONLY_ALL_ITEM));

    AddWidget(m_vecWidgetPair);
}

//添加选项
void CDlgDssKey::PushQWidgetPair(LPCSTR lpName, QWidget * qWidget, int type /*= 0*/)
{
    QWidgetPair qPair;

    qPair.first = LANG_TRANSLATE(lpName);
    qPair.second = qWidget;
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1
            && DT_SPEEDDIAL == m_DssKeyData.m_eType)
    {
        m_iDirectoryWriteType = 0;
        qPair.iExtraButten = NONE_EXTRA;
    }
    else if ((type == DSSKEY_BUTTON_TYPE_DIR_LABEL || type == DSSKEY_BUTTON_TYPE_DIR_LABEL_VALUE) && qWidget == m_pEditLabel)
    {
        m_iDirectoryWriteType = type;
        qPair.iExtraButten = DIRECTORY_EXTRA;
    }
    m_vecWidgetPair.push_back(qPair);
}

bool CDlgDssKey::GetPageData()
{
    IF_TimeTrace;
    m_DssKeyData.Reset();
    //将当前页面数据保存
    m_DssKeyData.m_eType = GetDsskeyType();

    GetWidgetValue(m_pCmbBoxIndex, m_DssKeyData.m_iIndex);

    GetWidgetValue(m_pCmbBoxAccountID, m_DssKeyData.m_iLineIndex);

    GetWidgetValue(m_pEditLabel, m_DssKeyData.m_strLabel);
    GetWidgetValue(m_pEditValue, m_DssKeyData.m_strValue);
    GetWidgetValue(m_pEditExt, m_DssKeyData.m_strExtension);

    return m_DssKeyData.m_eType != DT_NONE;
}

bool CDlgDssKey::LoadPageData()
{
    IF_TimeTrace;

    // 加载逻辑层数据
    GetLogicData();

    //根据action item类型为type下拉框添加数据
    AddCmbBoxItemData(false);
    AddCmbBoxItemData(true);
    AddAccountIDData();

    // 先设置好Type
    InitDsskeyType();

    // 由当前Type设置显示的控件
    SetViewByType(m_DssKeyData.m_eType);

    // 配置其他控件
    UpdateData();

    // 重设焦点
    ResetFocus();

    // 连接信号嘈
    ConnectSignals();

    return true;
}

void CDlgDssKey::UpdateData()
{
    IF_TimeTrace;
    m_pEditValue->setText(toQString(m_DssKeyData.m_strValue));
    m_pEditExt->setText(toQString(m_DssKeyData.m_strExtension));
    m_pEditLabel->setText(toQString(m_DssKeyData.m_strLabel));

    // 添加动态可选数据
    AddIndexData(true);
    AddIndexData(false);

    ConfigCombox(m_pCmbBoxIndex, m_DssKeyData.m_iIndex);
}

void CDlgDssKey::InitDsskeyType()
{
    DssKey_Type eKeyEvent = m_DssKeyData.m_eType;
    if (dsskeyuilogic_CheckKeyEventSet(m_PageInfo, m_DssKeyData.m_eType))
    {
        m_pCmbBoxKeyType->setCurrentIndex(m_pCmbBoxKeyType->findData(m_DssKeyData.m_eType, Qt::UserRole));
        eKeyEvent = DT_KEYEVENT;
    }

    //http://bugfree.yealink.com/Bug.php?BugID=141442
    int index = m_pCmbBoxType->findData(eKeyEvent, Qt::UserRole);
    if (index < 0)
    {
        m_DssKeyData.Reset();
        index = 0;
    }
    m_pCmbBoxType->setCurrentIndex(index);
    m_pCmbBoxAccountID->setCurrentIndex(m_pCmbBoxAccountID->findData(m_DssKeyData.m_iLineIndex,
                                        Qt::UserRole));
}

void CDlgDssKey::Init()
{
    // 断开信号嘈
    DisconnectSignals();

    m_vecKeyEvent.clear();
    m_vecKeyList.clear();

    //为Value安装输入法
    qInstallIME(m_pEditValue, IME_ALL);
    //为Extension安装输入法
    qInstallIME(m_pEditExt, IME_ALL);
    qInstallIME(m_pEditLabel, IME_ALL);
}

void CDlgDssKey::Uninit()
{
    // 断开信号嘈
    DisconnectSignals();

    m_vecKeyEvent.clear();
    m_vecKeyList.clear();

    m_pCmbBoxType->clear();
    m_pCmbBoxKeyType->clear();
    m_pCmbBoxAccountID->clear();
    m_pCmbBoxIndex->clear();

    qUninstallIME(m_pEditValue);
    qUninstallIME(m_pEditExt);
    qUninstallIME(m_pEditLabel);
}

bool CDlgDssKey::IsSameSettingDialog(const yl::string & strAction)
{
    return strAction.find(DSSKEY_KEY) != yl::string::npos
           || strAction.find(DSSKEY_LINE_KEY_ITEM) != yl::string::npos
           || strAction.find(DSSKEY_MEMEORY_KEY_ITEM) != yl::string::npos
           || strAction.find(DSSKEY_EXPAND_KEY_ITEM) != yl::string::npos;
}

void CDlgDssKey::SetAllWidgetEnable(bool bEnable)
{
    QWPVector::iterator iter = m_vecWidgetPair.begin();
    for (; iter != m_vecWidgetPair.end(); ++iter)
    {
        QWidget * pWidget = iter->second;
        if (NULL != pWidget)
        {
            iter->second->setEnabled(bEnable);
        }
    }
}

DssKey_Type CDlgDssKey::GetDsskeyType()
{
    int iType = DT_NA;
    GetWidgetValue(m_pCmbBoxType, iType);
    if (iType == DT_KEYEVENT)
    {
        if (!GetWidgetValue(m_pCmbBoxKeyType, iType))
        {
            if (m_vecKeyEvent.size() > 0)
            {
                iType = m_vecKeyEvent[0].iIndex;
            }
            /*else
            {
                int iIndex = m_pCmbBoxType->currentIndex();
                if (m_vecKeyList.size() > 0)
                {
                    iIndex = ++iIndex % m_vecKeyList.size();
                    m_pCmbBoxType->setCurrentIndex(iIndex);
                }
            }*/
        }
    }

    return (DssKey_Type)iType;
}

void CDlgDssKey::ResetFocus()
{
    this->setFocus();

    m_pGeneralFrame->SelectItemByIndex(0);
    m_pCmbBoxType->setFocus();
}

void CDlgDssKey::ConfigSingleWidget(QWidget * pWidget, DssKey_Type eType,
                                    DSSKEY_ATTR_ITEM eAttrItem, bool bEnabled)
{
    if (pWidget == NULL || eAttrItem == DSSKEY_ATTR_ITEM_NONE || eType == DT_KEYEVENT)
    {
        return;
    }

    CDsskeyItemAttr * pAttrItem = dsskeyuilogic_GetAttrByType(eType, eAttrItem);
    if (pAttrItem == NULL || !pAttrItem->IsVisible())
    {
        return;
    }

    if (pWidget->inherits("QLineEdit"))
    {
        QLineEdit * pEdit = static_cast<QLineEdit *>(pWidget);

        qSetCurrentIME(pEdit, pAttrItem->GetIme().c_str());
        //pEdit->setMaxLength(pAttrItem->iMaxLength);
        pEdit->setEchoMode(pAttrItem->bPasswd ? QLineEdit::Password : QLineEdit::Normal);
    }

    //IF_LOG_TRACE("[%d][%d][%s]", eAttrItem, bEnabled ? pAttrItem->IsReadOnly() : bEnabled,
    //             pAttrItem->GetName().c_str());
    // Dsskey自身只读属性优先级比单个配置优先级要高
    pWidget->setEnabled(bEnabled ? !(pAttrItem->IsReadOnly()) : bEnabled);

    PushQWidgetPair(pAttrItem->GetName().c_str(), pWidget, pAttrItem->GetButtonType());
}

bool CDlgDssKey::GetWidgetValue(QComboBox * pWidget, int & iValue)
{
    if (IsWidgetInList(pWidget))
    {
        iValue = pWidget->itemData(pWidget->currentIndex(), Qt::UserRole).value<int>();
        return true;
    }

    return false;
}

bool CDlgDssKey::GetWidgetValue(QLineEdit * pWidget, yl::string & strValue)
{
    if (IsWidgetInList(pWidget))
    {
        strValue = pWidget->text().trimmed().toUtf8().data();
        return true;
    }

    return false;
}

bool CDlgDssKey::ConfigCombox(QComboBox * pWidget, int & iValue)
{
    if (pWidget == NULL || !IsWidgetInList(pWidget) || focusWidget() == pWidget)
    {
        return false;
    }

    if (pWidget->findData(iValue, Qt::UserRole) < 0)
    {
        pWidget->setCurrentIndex(0);
        iValue = pWidget->itemData(pWidget->currentIndex(), Qt::UserRole).value<int>();
        return true;
    }

    pWidget->setCurrentIndex(pWidget->findData(iValue, Qt::UserRole));
    return false;
}

void CDlgDssKey::ConnectSignals()
{
    connect(m_pCmbBoxType, SIGNAL(currentIndexChanged(int)), this,
            SLOT(OnCmbBoxTypeCurrentIndexChanged(int)));
    connect(m_pCmbBoxKeyType, SIGNAL(currentIndexChanged(int)), this,
            SLOT(OnCmbBoxTypeCurrentIndexChanged(int)));
    connect(m_pCmbBoxAccountID, SIGNAL(currentIndexChanged(int)), this,
            SLOT(OnCmbBoxAccountIDCurrentIndexChanged(int)));
    connect(m_pCmbBoxIndex, SIGNAL(currentIndexChanged(int)), this,
            SLOT(OnCmbBoxIndexCurrentIndexChanged(int)));
}

void CDlgDssKey::DisconnectSignals()
{
    disconnect(m_pCmbBoxType, SIGNAL(currentIndexChanged(int)), this,
               SLOT(OnCmbBoxTypeCurrentIndexChanged(int)));
    disconnect(m_pCmbBoxKeyType, SIGNAL(currentIndexChanged(int)), this,
               SLOT(OnCmbBoxTypeCurrentIndexChanged(int)));
    disconnect(m_pCmbBoxAccountID, SIGNAL(currentIndexChanged(int)), this,
               SLOT(OnCmbBoxAccountIDCurrentIndexChanged(int)));
    disconnect(m_pCmbBoxIndex, SIGNAL(currentIndexChanged(int)), this,
               SLOT(OnCmbBoxIndexCurrentIndexChanged(int)));
}

void CDlgDssKey::BlockSignals(bool bBlock)
{
    m_pCmbBoxType->blockSignals(bBlock);
    m_pCmbBoxKeyType->blockSignals(bBlock);
    m_pCmbBoxAccountID->blockSignals(bBlock);
    m_pCmbBoxIndex->blockSignals(bBlock);
}

void CDlgDssKey::OnCmbBoxIndexCurrentIndexChanged(int nIndex)
{
    // 可选需要改变，暂时不需要处理
}

void CDlgDssKey::GetLogicData()
{
    dsskeyuilogic_GetData(m_PageInfo, m_DssKeyData);
    m_OldDssKeyData = m_DssKeyData;

    dsskeyuilogic_GetAllDsskey(m_PageInfo, m_vecKeyEvent, true);
    dsskeyuilogic_GetAllDsskey(m_PageInfo, m_vecKeyList, false);
}

bool CDlgDssKey::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    GetPageData();

    // 给出保存提示
    CDsskeyPair errorTips;
    if (dsskeyuilogic_IsDataValid(m_PageInfo, m_DssKeyData, errorTips))
    {
        return true;
    }
    else
    {
        //SETTINGUI_INFO("[%d][%s]", errorTips.iIndex, errorTips.strValue.c_str());
        // 逻辑层错误提示转化为UI层错误提示信息
        struct WidgetItemPair
        {
            QWidget    *    pWidgit;
            int             iAttr;
        };

        const static WidgetItemPair s_arr_widget_pair[] =
        {
            { m_pEditLabel,         DSSKEY_ATTR_ITEM_LABEL      },
            { m_pEditValue,         DSSKEY_ATTR_ITEM_VALUE      },
            { m_pEditExt,           DSSKEY_ATTR_ITEM_EXT        },
            { m_pCmbBoxType,        DSSKEY_ATTR_ITEM_TYPE       },
            { m_pCmbBoxKeyType,     DSSKEY_ATTR_ITEM_KEYEVENT   },
            { m_pCmbBoxAccountID,   DSSKEY_ATTR_ITEM_LINE       },
            { m_pCmbBoxIndex,       DSSKEY_ATTR_ITEM_INDEX      },
        };

        QWidget * pFocusWidget = NULL;
        int iSize = sizeof(s_arr_widget_pair) / sizeof(s_arr_widget_pair[0]);
        for (int i = 0; i < iSize; ++i)
        {
            if (s_arr_widget_pair[i].iAttr == errorTips.iIndex)
            {
                pFocusWidget =  s_arr_widget_pair[i].pWidgit;
                break;
            }
        }

        if (pFocusWidget != NULL)
        {
            // 因架构限制当前只能标志输入控件为非法控件
            objErrorContent.pLineEdit = pFocusWidget->inherits("QLineEdit")
                                        ? static_cast<QLineEdit *>(pFocusWidget) : NULL;
        }

        objErrorContent.strErrorContent = LANG_TRANSLATE(errorTips.strValue.c_str());
    }

    return false;
}

bool CDlgDssKey::SavePageData()
{
    return dsskeyuilogic_SaveData(m_PageInfo, m_DssKeyData);
}

void CDlgDssKey::OnExtraAction(int eActionType)
{
    if (eActionType == DIRECTORY_EXTRA)
    {
        DM_EnterSelectDirectoryUI(DlgDsskey_DirectoryCallBack);
    }
}

void CDlgDssKey::writeData(QString name, QString number)
{
    if (m_iDirectoryWriteType == DSSKEY_BUTTON_TYPE_NONE)
    {
        return;
    }

    if (m_iDirectoryWriteType == DSSKEY_BUTTON_TYPE_DIR_LABEL)
    {
        m_pEditLabel->setText(number);
    }
    else if (m_iDirectoryWriteType == DSSKEY_BUTTON_TYPE_DIR_LABEL_VALUE)
    {
        m_pEditLabel->setText(name);
        m_pEditValue->setText(number);
    }

}
