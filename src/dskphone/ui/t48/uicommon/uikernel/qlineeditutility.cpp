#include "qlineeditutility.h"
#include "uicommon.h"
#include "inputmethod/inputmethod.h"
#include "language/input/include/modInput.h"

qLineEditUtility::qLineEditUtility(void)
{
}

qLineEditUtility::~qLineEditUtility(void)
{
}

/************************************************************************/
/*                          Set QLineEditStyle                          */
/************************************************************************/
void qLineEditUtility::SetLineEditStyle(QLineEdit * pEdit, const QColor & color = Qt::black,
                                        const QColor & clrHightLight/* = Qt::darkBlue*/,
                                        const QColor & clrHightLightText/* = Qt::white*/)
{
    if (pEdit == NULL)
    {
        return;
    }

    pEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    pEdit->setGeometry(0, 0, DFAULT_T48_EDIT_WITH + 4, DFAULT_T48_EDIT_HEIGHT);

    QPalette pal = pEdit->palette();

    pal.setColor(QPalette::Text, color);
    pal.setColor(QPalette::Disabled, QPalette::Base, WidgetTransparentColor);
    pal.setColor(QPalette::Active, QPalette::Base, QColor());
    pal.setColor(QPalette::Highlight, clrHightLight);
    pal.setColor(QPalette::HighlightedText, clrHightLightText);

    pEdit->setPalette(pal);
}

/************************************************************************/
/*                         Set QLineEditTransparent                     */
/************************************************************************/
void qLineEditUtility::SetLineEditTransparent(QLineEdit * pEdit)
{
    if (pEdit != NULL)
    {
        pEdit->setFrame(false);
        QPalette pal = pEdit->palette();
        pal.setColor(QPalette::Base, WidgetTransparentColor);
        pEdit->setPalette(pal);
    }
}

/********************************************************
设置LineEdit在disable状态下的字体颜色
*********************************************************/
void qLineEditUtility::SetLineEditStype(QLineEdit * pEdit)
{
    if (pEdit != NULL)
    {
        QPalette palette = pEdit->palette();
        palette.setColor(QPalette::Disabled, QPalette::Text, QColor(128, 128, 128));
        pEdit->setPalette(palette);
    }
}

/********************************************************
函数名称：SetCurrentInputMethod
函数意义：设置LineEdit载入界面时的首选输入法
返回值：无返回值
*********************************************************/
void qLineEditUtility::SetCurrentInputMethod(QLineEdit * pEdit, EDIT_TYPE eType)
{
    switch (eType)
    {
    case EDIT_TYPE_CUSTOM:
        {
            qSetCurrentIME(pEdit, IME_DIY);
        }
        break;
    case EDIT_TYPE_NAME:
        {
            qSetCurrentIME(pEdit, IME_2aB);
        }
        break;
    case EDIT_TYPE_NAME_PWD:
        {
            qSetCurrentIME(pEdit, IME_2aB_Pwd);
        }
        break;
    case EDIT_TYPE_SERVER:
        {
            qSetCurrentIME(pEdit, IME_2aB);
        }
        break;
    case EDIT_TYPE_LOWER:
        {
            qSetCurrentIME(pEdit, IME_abc);
        }
        break;
    case EDIT_TYPE_NUMBER_PWD:
        {
            qSetCurrentIME(pEdit, IME_123_Pwd);
        }
        break;
    case EDIT_TYPE_PASSWORD:
        {
            qSetCurrentIME(pEdit, IME_abc_Pwd);
        }
        break;
    case EDIT_TYPE_PHONE_NUMBER:
        {
            qSetCurrentIME(pEdit, IME_123_Dial);
        }
        break;
    case EDIT_TYPE_IPV4:
        {
            qSetCurrentIME(pEdit, IME_123_IP);
        }
        break;
    case EDIT_TYPE_IP_PORT:
        {
            qSetCurrentIME(pEdit, IME_123);
        }
        break;
    case EDIT_TYPE_IPV4_NETMASK:
        {
            qSetCurrentIME(pEdit, IME_123_IP);
        }
        break;
    case EDIT_TYPE_IP_SERVER:
        {
            qSetCurrentIME(pEdit, IME_123_IP);
        }
        break;
    case EDIT_TYPE_DIAL:
        {
            qSetCurrentIME(pEdit, IME_123_Dial);
        }
        break;
    case EDIT_TYPE_MAIL:
        {
            qSetCurrentIME(pEdit, IME_2aB);
        }
        break;
    case EDIT_TYPE_NUMBER:
        {
            qSetCurrentIME(pEdit, IME_123);
        }
        break;
    case EDIT_TYPE_URL:
        {
            qSetCurrentIME(pEdit, IME_abc);
        }
        break;
    case EDIT_TYPE_DTMF:
        {
            qSetCurrentIME(pEdit, IME_123_Dial);
        }
        break;
    case EDIT_TYPE_AES:
        {
            qSetCurrentIME(pEdit, IME_2aB);
        }
        break;
    case EDIT_TYPE_UNLOCKPIN:
        {
            qSetCurrentIME(pEdit, IME_123);
        }
        break;
    case EDIT_TYPE_DSSKEY_VALUE:
        {
            qSetCurrentIME(pEdit, IME_123);
        }
        break;
    default:
        break;
    }
}

void qLineEditUtility::SetMaxLength(QLineEdit * pEdit, const EDIT_TYPE & eType)
{
    if (pEdit == NULL)
    {
        return;
    }

    switch (eType)
    {
    case EDIT_TYPE_NAME:
    case EDIT_TYPE_NAME_PWD:
        {
            pEdit->setMaxLength(MAX_LEN_DISPLAY_NAME);
        }
        break;
    case EDIT_TYPE_SERVER:
        {
            pEdit->setMaxLength(MAX_LEN_SERVER_NAME);
        }
        break;
    case EDIT_TYPE_SIP_SERVER:
        {
            pEdit->setMaxLength(MAX_LEN_PROXY);
        }
        break;
    case EDIT_TYPE_PASSWORD:
    case EDIT_TYPE_LOWER:
        {
            pEdit->setMaxLength(MAX_LEN_PASSWORD);
        }
        break;
    case EDIT_TYPE_UNLOCKPIN:
        {
            pEdit->setMaxLength(MAX_LEN_UNLOCKPIN);
        }
        break;
    case EDIT_TYPE_PHONE_NUMBER:
        {
            pEdit->setMaxLength(MAX_LEN_PHONE_NUMBER);
        }
        break;
    case EDIT_TYPE_IPV4:
        {
            pEdit->setMaxLength(MAX_LEN_IP);
        }
        break;
    case EDIT_TYPE_IP_PORT:
        {
            pEdit->setMaxLength(MAX_LEN_PORT);
        }
        break;
    case EDIT_TYPE_IPV4_NETMASK:
        {
            pEdit->setMaxLength(MAX_LEN_IP);
        }
        break;
    case EDIT_TYPE_IP_SERVER:
        {
            pEdit->setMaxLength(MAX_LEN_IP);
        }
    case EDIT_TYPE_DIAL:
        {
            pEdit->setMaxLength(MAX_LEN_DIAL);
        }
        break;
    case EDIT_TYPE_MAIL:
        {
            pEdit->setMaxLength(MAX_LEN_MAIL);
        }
        break;
    case EDIT_TYPE_NUMBER:
    case EDIT_TYPE_NUMBER_PWD:
        {
            pEdit->setMaxLength(MAX_LEN_NUMBER);
        }
        break;
    case EDIT_TYPE_NUMBER_SHORT:
        {
            pEdit->setMaxLength(MAX_LEN_NUMBER_SHORT);
        }
        break;
    case EDIT_TYPE_URL:
        {
            pEdit->setMaxLength(MAX_LEN_URL);
        }
        break;
    case EDIT_TYPE_AES:
        {
            pEdit->setMaxLength(MAX_LEN_AES);
        }
        break;
    case EDIT_TYPE_DTMF:
        {
            pEdit->setMaxLength(MAX_LEN_DTMF);
        }
        break;
    case EDIT_TYPE_PROXY:
        {
            pEdit->setMaxLength(MAX_LEN_PROXY);
        }
        break;
    case EDIT_TYPE_NAME_SHORT:
        {
            pEdit->setMaxLength(MAX_LEN_DISPLAY_NAME_SHORT);
        }
        break;
    case MAX_LEN_PHONE_NUMBER_LONG:
        {
            pEdit->setMaxLength(MAX_LEN_PHONE_NUMBER_LONG);
        }
        break;
    case EDIT_TYPE_CODE:
        {
            pEdit->setMaxLength(MAX_LEN_PHONE_NUMBER);
        }
        break;
    case EDIT_TYPE_WIFI_SSID_PROFILE:
        {
            pEdit->setMaxLength(MAX_LEN_WIFI_SSID);
        }
        break;
    case EDIT_TYPE_WIFI_PWD:
        {
            pEdit->setMaxLength(MAX_LEN_WIFI_PWD);
        }
        break;
    case EDIT_TYPE_WIFI_WEP_PWD:
        {
            pEdit->setMaxLength(MAX_LEN_WIFI_WEP_PWD);
        }
        break;
    case EDIT_TYPE_ACCOUNT_NAME:
        {
            pEdit->setMaxLength(MAX_LEN_DISPLAY_NAME);
        }
        break;
    case EDIT_TYPE_CONTACT_NAME:
        {
            pEdit->setMaxLength(MAX_LEN_DISPLAY_NAME);
        }
        break;
    case EDIT_TYPE_CONTACT_NUMBER:
        {
            pEdit->setMaxLength(MAX_LEN_PHONE_NUMBER_LONG);
        }
        break;
    case EDIT_TYPE_DSSKEY_VALUE:
        {
            pEdit->setMaxLength(MAX_LEN_DSSKEY_VALUE);
        }
        break;
#if IF_FEATURE_FAVORITE_DIR
    case EDIT_TYPE_LOCAL_FAVORITE_INDEX:
        {
            pEdit->setMaxLength(MAX_LEN_LOCAL_FAVORITE_INDEX);
        }
        break;
#endif
    default:
        break;
    }
}

bool qLineEditUtility::IsFormatValid(const QString & strQInput, const EDIT_TYPE & eType)
{
    yl::string strInput = strQInput.trimmed().toUtf8().data();
    switch (eType)
    {
    case EDIT_TYPE_NAME:
    case EDIT_TYPE_NAME_PWD:
        {
            return qLineEditUtility::IsValidName(strInput);
        }
        break;
    case EDIT_TYPE_SERVER:
        {
            return qLineEditUtility::IsValidServerName(strInput);
        }
        break;
    case EDIT_TYPE_SIP_SERVER:
        {
            return qLineEditUtility::IsValidSipServer(strInput);
        }
        break;
    case EDIT_TYPE_PASSWORD:
    case EDIT_TYPE_LOWER:
        {
            return qLineEditUtility::IsValidPassword(strInput);
        }
        break;
    case EDIT_TYPE_UNLOCKPIN:
        {
            return qLineEditUtility::IsValidPhoneNumber(strInput);
        }
        break;
    case EDIT_TYPE_PHONE_NUMBER:
        {
            return qLineEditUtility::IsValidPhoneNumber(strInput);
        }
        break;
    case EDIT_TYPE_IPV4:
        {
            return qLineEditUtility::IsValidIPv4(strInput);
        }
        break;
    case EDIT_TYPE_IPV6:
        {
            return qLineEditUtility::IsValidIPv6(strInput);
        }
        break;
    case EDIT_TYPE_IP_PORT:
        {
            return qLineEditUtility::IsValidPort(strInput);
        }
        break;
    case EDIT_TYPE_IPV4_NETMASK:
        {
            return qLineEditUtility::IsValidIPv4NetMask(strInput);
        }
        break;
    case EDIT_TYPE_IPV6_NETMASK:
        {
            return qLineEditUtility::IsValidIPv6NetMask(strInput);
        }
        break;
    case EDIT_TYPE_IPV6_GATEWAY:
        {
            return qLineEditUtility::IsValidIPv6GateWay(strInput);
        }
        break;
    case EDIT_TYPE_IP_SERVER:
        {
            return qLineEditUtility::IsValidServer(strInput);
        }
    case EDIT_TYPE_DIAL:
        {
            return qLineEditUtility::IsValidDial(strInput);
        }
        break;
    case EDIT_TYPE_MAIL:
        {
            return qLineEditUtility::IsValidMail(strInput);
        }
        break;
    case EDIT_TYPE_DTMF:
        {
            return qLineEditUtility::IsValidDTMF(strInput);
        }
        break;
    case EDIT_TYPE_DIAL_PLAN:
        {
            /////-----
            return true;
        }
        break;
    case EDIT_TYPE_NUMBER:
    case EDIT_TYPE_NUMBER_PWD:
        {
            return qLineEditUtility::IsValidNumber(strInput);
        }
        break;
    case EDIT_TYPE_URL:
        {
            //
            return qLineEditUtility::IsValidURL(strInput);
        }
        break;
    case EDIT_TYPE_AES:
        {
            return qLineEditUtility::IsValidAES(strInput);
        }
        break;
    case EDIT_TYPE_PROXY:
        {
            return qLineEditUtility::IsValidServerName(strInput);
        }
        break;
    case EDIT_TYPE_NAME_SHORT:
        {
            return qLineEditUtility::IsValidName(strInput);
        }
        break;
    case MAX_LEN_PHONE_NUMBER_LONG:
        {
            return qLineEditUtility::IsValidNumber(strInput);
        }
        break;
    case EDIT_TYPE_CODE:
        {
            return qLineEditUtility::IsValidOnOffCode(strInput);
        }
        break;
    case EDIT_TYPE_ACCOUNT_NAME:
        {
            return qLineEditUtility::IsValidAccountName(strInput);
        }
        break;
    case EDIT_TYPE_CONTACT_NAME:
        {
            return qLineEditUtility::IsValidContactName(strInput);
        }
        break;
    case EDIT_TYPE_CONTACT_NUMBER:
        {
            return qLineEditUtility::IsValidContactNumber(strInput);
        }
        break;
    case EDIT_TYPE_BSFT_NAME:
        {
            return qLineEditUtility::IsValidBSFTName(strInput);
        }
        break;
#if IF_FEATURE_FAVORITE_DIR
    case EDIT_TYPE_LOCAL_FAVORITE_INDEX:
        {
            return qLineEditUtility::IsValidNumber(strInput);
        }
        break;
#endif
    default:
        return true;
        break;
    }

    return true;
}

bool qLineEditUtility::IsValidIPv4(const yl::string & strValue)
{
    QString addr(strValue.c_str());
    QString pattern("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    QRegExp expr(pattern);
    return expr.exactMatch(addr);
}

bool qLineEditUtility::IsValidIPv6(const yl::string & strValue)
{
    return netIsValidIPV6(strValue.c_str());
}

bool qLineEditUtility::IsValidPort(const yl::string & strValue)
{
    QString portNumber(strValue.c_str());
    int iPort = atoi(strValue.c_str());

    //  QString pattern("([0-100])");
    QRegExp expr("[0-6][0-9][0-9][0-9][0-9]|\\d{4}|\\d{3}|\\d{2}|[1-9]");
    return expr.exactMatch(portNumber) && (0 < iPort) && (iPort < 65536);
}

bool qLineEditUtility::IsValidIPv4NetMask(const yl::string & strValue)
{
    QString checkmask(strValue.c_str());

    QRegExp expr("(128|192|224|240|248|252|254|255)\\.(0\\.){2}0|255\\.(0|128|192|224|240|248|252|254)\\.0\\.0|255\\.255\\.(0|128|192|224|240|248|252|254)\\.0|255\\.255\\.255\\.(0|128|192|224|240|248|252|254)");
    return expr.exactMatch(checkmask);
}

bool qLineEditUtility::IsValidIPv6NetMask(const yl::string & strValue)
{
    QString checkmask(strValue.c_str());

    QRegExp expr("((\\d){1,2})|([1][0-2][0-8])");
    return expr.exactMatch(checkmask);
}

bool qLineEditUtility::IsValidIPv6GateWay(const yl::string & strValue)
{
    return netIsValidIPV6GateWay(strValue.c_str());
}

bool qLineEditUtility::IsValidServer(const yl::string & strValue)
{
    QString checkservername(strValue.c_str());

    if (checkservername.size() > 99)
    {
        return FALSE;
    }
    QRegExp expr("^-.*-$");
    return expr.exactMatch(checkservername);
}

bool qLineEditUtility::IsValidSipServer(const yl::string & strValue)
{
    return true;
}

bool qLineEditUtility::IsValidName(const yl::string & strValue)
{
    return true;
}

bool qLineEditUtility::IsValidServerName(const yl::string & strValue)
{
    //SIP Server:首个字符不能为":",：这个符号会引起被转接方显示错误
    //-不能
    QString strServer(strValue.c_str());
    QRegExp expr("^[^:].*$");
    return expr.exactMatch(strServer);
}

bool qLineEditUtility::IsValidPassword(const yl::string & strValue)
{
    // V81 SP1需求:去除密码框限制
    return true;
}

bool qLineEditUtility::IsValidPhoneNumber(const yl::string & strValue)
{
    return true;
}

bool qLineEditUtility::IsValidDial(const yl::string & strValue)
{
    //不需要
    //  if (strValue == "")
    //  {
    //      return false;
    //  }
    return true;
}

bool qLineEditUtility::IsValidMail(const yl::string & strValue)
{
    //暂无特殊要求
    return true;
}

bool qLineEditUtility::IsValidDTMF(const yl::string & strValue)
{
    //http://10.2.1.199/Bug.php?BugID=88075 添加对E的支持
    //只接受 数字/*/#
    QString strDTMF(strValue.c_str());
    QRegExp expr("[0-9A-E\\*#<>]*");
    return expr.exactMatch(strDTMF);
}

bool qLineEditUtility::IsValidNumber(const yl::string & strValue)
{
    //只接受整数
    QString strNumber(strValue.c_str());
    QRegExp expr("\\d+$");
    return expr.exactMatch(strNumber);
}

bool qLineEditUtility::IsValidURL(const yl::string & strValue)
{
    //////-----暂时只判断是否为空
    return true;
}

bool qLineEditUtility::IsValidAES(const yl::string & strValue)
{
    // V81 SP1需求:去除密码框限制
    return true;
}

bool qLineEditUtility::IsValidOnOffCode(const yl::string & strValue)
{
    return true;
}

bool qLineEditUtility::IsValidAccountName(const yl::string & strValue)
{
    //Account相关的名称不能含有!&:;<>[\]?%
    QString strAccountName(strValue.c_str());
    QString strEX = QRegExp::escape("!&:;<>[]?%\\");
    QRegExp expr("[^" + strEX + "]*");
    return expr.exactMatch(strAccountName);
}

bool qLineEditUtility::IsValidContactName(const yl::string & strValue)
{
    //contact name不能含有\<>'"
    QString strContactName(strValue.c_str());
    QString strEX = QRegExp::escape("<>'\"\\");
    QRegExp expr("[^" + strEX + "]*");
    return expr.exactMatch(strContactName);
}

bool qLineEditUtility::IsValidContactNumber(const yl::string & strValue)
{
    //contact number不能含有"\'
    QString strAccountName(strValue.c_str());
    QString strEX = QRegExp::escape("\"'\\");
    QRegExp expr("[^" + strEX + "]*");
    return expr.exactMatch(strAccountName);
}

bool qLineEditUtility::IsValidBSFTName(const yl::string & strValue)
{
    // BroadSoft Person Name不能包含/\<>'"
    QString strName(strValue.c_str());
    QString strEX = QRegExp::escape("/<>'\"\\");
    QRegExp expr("[^" + strEX + "]*");

    return expr.exactMatch(strName);
}
