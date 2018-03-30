#include "model.h"
#include "dlgsetaeskey.h"
#include "setting/src/setaeskeycontroller.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "setting/src/callforwardcontroller.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "inputmethod/inputmethod.h"
#include "uikernel/qlineeditutility.h"
#include <ylstring.h>

CDlgSetAESKey::CDlgSetAESKey(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pEditCommon(NULL)
    , m_pEditMACOriented(NULL)
{
    InitData();
}

CDlgSetAESKey::~CDlgSetAESKey()
{

}

//通过C层获取已有配置数据，通过V层在界面上显示出来
bool CDlgSetAESKey::LoadPageData()
{
    this->setFocus();
    //设置首选输入法
    qLineEditUtility::SetCurrentInputMethod(m_pEditCommon, EDIT_TYPE_AES);
    qLineEditUtility::SetCurrentInputMethod(m_pEditMACOriented, EDIT_TYPE_AES);

    //调用控制层：加载数据
    bool bIsSuccess =
        g_pSetAesKeyController->GetAesKeyData(&m_AesKeyData);

    if (bIsSuccess)
    {
        //加载数据成功，设置当前页面数据,并返回true
        SetCurrentPageData(m_AesKeyData);
        return true;
    }
    else
    {
        //加载数据失败，返回false
        return false;
    }
}


//通过C层获取配置数据，与GetPageData值比较，有改动时返回true
bool CDlgSetAESKey::IsStatusChanged()
{
    AesKeyData currentPageData;
    GetCurrentPageData(currentPageData);
    if (ComparePageData(m_AesKeyData, currentPageData))
    {
        //前后页面数据没有改变，返回false
        return false;
    }
    else
    {
        //前后页面数据改变，返回true
        return true;
    }
}

bool CDlgSetAESKey::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (!m_pEditCommon->text().isEmpty()
            && !qLineEditUtility::IsFormatValid(m_pEditCommon->text(), EDIT_TYPE_AES))
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_COMMON_AES_KEY_IS_INVALID);
        objErrorContent.pLineEdit = m_pEditCommon;
        return false;
    }
    if (!m_pEditMACOriented->text().isEmpty()
            && !qLineEditUtility::IsFormatValid(m_pEditMACOriented->text(), EDIT_TYPE_AES))
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_MAC_ORIENTED_IS_INVALID);
        objErrorContent.pLineEdit = m_pEditMACOriented;
        return false;
    }
    return true;
}

//从V层读取当前界面的配置，通过C层保存
bool CDlgSetAESKey::SavePageData()
{
    //提起页面数据
    if (IsStatusChanged())
    {
        GetCurrentPageData(m_AesKeyData);
        //调用控制层：保存数据
        g_pSetAesKeyController->SetAesKeyData(m_AesKeyData);
    }
    return true;
}


//用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
void CDlgSetAESKey::InitData()
{
    m_pEditCommon = new QLineEdit(this);
    m_pEditMACOriented = new QLineEdit(this);
    if (NULL == m_pEditCommon
            || NULL == m_pEditMACOriented)
    {
        return;
    }

    qLineEditUtility::SetLineEditStyle(m_pEditCommon, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditMACOriented, Qt::black);

    qInstallIME(m_pEditCommon, IME_LOWER);
    qInstallIME(m_pEditMACOriented, IME_LOWER);

    qLineEditUtility::SetMaxLength(m_pEditCommon, EDIT_TYPE_AES);
    qLineEditUtility::SetMaxLength(m_pEditMACOriented, EDIT_TYPE_AES);

    //设置密码方式回显
    m_pEditCommon->setEchoMode(QLineEdit::Password);
    m_pEditMACOriented->setEchoMode(QLineEdit::Password);

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_COMMON_AES_KEY);
    qPair.first = strLab;
    qPair.second = m_pEditCommon;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_MAC_ORIENTED);
    qPair.first = strLab;
    qPair.second = m_pEditMACOriented;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    m_pEditCommon->installEventFilter(this);
    m_pEditMACOriented->installEventFilter(this);
    this->installEventFilter(this);
}


//比较页面前后数据
bool CDlgSetAESKey::ComparePageData(const AesKeyData & beforePageData,
                                    const AesKeyData & afterPageData) const
{
    if (beforePageData.m_strAesKey != afterPageData.m_strAesKey
            || beforePageData.m_strAesMacKey != afterPageData.m_strAesMacKey)
    {
        //如果不相等那么返回flase
        return false;
    }
    else
    {
        //相等返回true
        return true;
    }
}

//取得当前页面数据
AesKeyData & CDlgSetAESKey::GetCurrentPageData(AesKeyData & currentPageData) const
{
    //提取数据：Aes key
    currentPageData.m_strAesKey =
        m_pEditCommon->text().trimmed().toUtf8().data();
    //提取数据：Aes mac key
    currentPageData.m_strAesMacKey =
        m_pEditMACOriented->text().trimmed().toUtf8().data();

    return currentPageData;
}

//设置当前页面数据
void CDlgSetAESKey::SetCurrentPageData(AesKeyData & currentPageData)
{
    //设置数据：Common
    m_pEditCommon->setText(toQString(currentPageData.m_strAesKey.c_str()));
    //设置数据：MAC-Oriented
    m_pEditMACOriented->setText(toQString(currentPageData.m_strAesMacKey.c_str()));
}
