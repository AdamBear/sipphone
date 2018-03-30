#include "model.h"
#include "dlgpingandtracerouteui.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "uikernel/languagehelper.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "uikernel/qlineeditutility.h"
#include "baseui/framelist.h"
#include "actiondefinecommon.h"
#include "settinguicommon.h"
#include "messagebox/modmessagebox.h"
#include "diagnostics/include/moddiagnostics.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "keymap.h"
#include "dsklog/log.h"
#include "ylstl/ylstringalgorithm.h"
#include "baseui/t4xpicpath.h"


//显示结果的字体大小

#ifdef _T48
#define PING_AND_TRACEROUTE_RESULT_ITEM_WIDTH 500
#else
//t46的宽度
//显示结果的长度
#define PING_AND_TRACEROUTE_RESULT_ITEM_WIDTH 436
#endif


//等待的提示ID
#define ID_NOTE_WAIT 105
//输入url地址的提示ID
#define ID_NOTE_INPUT_URL 106
//执行检测失败
#define ID_NOTE_CHECK_ERROR ID_NOTE_INPUT_URL


//输入url地址的等待时间
#define WAITTIME_NOTE_INPUT_URL 2000
//等待提示的等待时间
#define WAITTIME_NOTE_WAIT -1
//执行检测失败的等待时间
#define WAITTIME_NOTE_CHECK_ERROR 2000

//打开ping的action
const yl::string g_strOpenPagePing("OpenSubPage(networkping)");

namespace SettingUI
{
//处理 检测结果的 回调函数
static LRESULT S_DealPingAndTracerouteMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("_DealPingAndTracerouteMsg ");

    CDlgPingAndTracerouteUI * pDlg = qobject_cast<CDlgPingAndTracerouteUI *>(UIManager_GetPrivateDlg(
                                         DLG_CDlgPingAndTracerouteUI));
    if (NULL == pDlg)
    {
        return 0;
    }

    pDlg->OnPingAndTracerouteMsg(objMessage);

    return 1;
}
}



CDlgPingAndTracerouteUI::CDlgPingAndTracerouteUI(QWidget * pParent)
    : CDlgBaseSubPage(pParent)
//,m_strDlgID("")
    , m_pEditURL(NULL)
    , m_eShowInfoType(NETWORK_CHECK_SHOWINFO_TYPE_NONE)     //当前显示的信息 类型
    , m_strCheckResult("")      //检测结果
    , m_iMaxGetSplitStringLen(0)    //获取的最大长度字符串   优化解析字符串函数使用
{
    InitData();
}

CDlgPingAndTracerouteUI::~CDlgPingAndTracerouteUI()
{

}

void CDlgPingAndTracerouteUI::InitData()
{
    m_pEditURL = new QLineEdit(this);

    //指针判空
    if (NULL == m_pEditURL)
    {
        return;
    }

    //安装输入法
    //只能使用数字输入
    qInstallIME(m_pEditURL, IME_DIGIT);

    //最大输入长度限制
    qLineEditUtility::SetMaxLength(m_pEditURL, EDIT_TYPE_URL);

    //设置输入框风格
    SetLineEditDefaultStyle(m_pEditURL);

    //为界面上各输入控件安装事件过滤器
    m_pEditURL->installEventFilter(this);

}

//初始化
void CDlgPingAndTracerouteUI::Init()
{
    CDlgBaseSubPage::Init();

    //清除控件信息
    if (NULL != m_pEditURL)
    {
        m_pEditURL->clear();
    }

    //设置初始状态
    m_eShowInfoType = NETWORK_CHECK_SHOWINFO_TYPE_NONE;

    //初始化
    m_strCheckResult = "";      //检测结果

    m_vectorCheckResults.clear();   //检测结果数组

    //注册检测结果消息
    RegisterCheckResultMsg();
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgPingAndTracerouteUI::Uninit()
{
    //注销检测结果消息
    UnRegisterCheckResultMsg();
}

bool CDlgPingAndTracerouteUI::LoadPageData()
{
    return true;
}

bool CDlgPingAndTracerouteUI::IsStatusChanged()
{
    return false;
}

bool CDlgPingAndTracerouteUI::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    return true;
}

bool CDlgPingAndTracerouteUI::SavePageData()
{
    return false;
}

//窗口显示到界面上时的操作
void CDlgPingAndTracerouteUI::UpdateWnd()
{
    //说明当前没有显示信息  那么要设置显示ip 信息
    if (NETWORK_CHECK_SHOWINFO_TYPE_NONE == m_eShowInfoType)
    {
        //显示IP
        ShowNetworkCheckIP();

        m_eShowInfoType = NETWORK_CHECK_SHOWINFO_TYPE_IP;
    }

    CDlgBaseSubPage::UpdateWnd();
}

// 是否跳过Return To Idle，包括通话、Idle
bool CDlgPingAndTracerouteUI::IsRejectReturnToIdle(RETURN_IDLE_TYPE
        eReturnIdle/* = RETURN_IDLE_TYPE_NORMAL*/)
{
    //如果是用户点击 home 那么一定可以回到idle
    if (eReturnIdle != RETURN_IDLE_TYPE_TIME_OUT)
    {
        return false;
    }

    //在等待界面  和 显示结果界面 不能returntoidle
    if ((NETWORK_CHECK_SHOWINFO_TYPE_WAIT == m_eShowInfoType)
            || (NETWORK_CHECK_SHOWINFO_TYPE_RESULT == m_eShowInfoType)
       )
    {
        return true;
    }

    return false;
}

// 设置子页面前的操作
void CDlgPingAndTracerouteUI::BeforeSetSubPage(void * pData)
{

}

// 设置子页面的SoftKey
bool CDlgPingAndTracerouteUI::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    //如果是在输入IP界面 那么最后一个softkey 要改成start
    if (NETWORK_CHECK_SHOWINFO_TYPE_IP == m_eShowInfoType)
    {
        if (objWndData.size() >= 4)
        {
            objWndData[3].m_strSoftkeyAction = ACTION_START;
            objWndData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_START);
            objWndData[3].m_strIconPath = PIC_SOFTKEY_START;
        }
    }
    else if (NETWORK_CHECK_SHOWINFO_TYPE_RESULT == m_eShowInfoType)     //显示结果界面
    {
        if (objWndData.size() >= 4)
        {
            objWndData[3].m_strSoftkeyAction = "";
            objWndData[3].m_strSoftkeyTitle = "";
        }

        if (objWndData.size() >= 3)
        {
            objWndData[2].m_strSoftkeyAction = "";
            objWndData[2].m_strSoftkeyTitle = "";
        }

        if (objWndData.size() >= 2)
        {
            objWndData[1].m_strSoftkeyAction = "";
            objWndData[1].m_strSoftkeyTitle = "";
        }

    }

    return true;
}

//处理action
bool CDlgPingAndTracerouteUI::OnAction(const QString & strAction)
{
    if (ACTION_START == strAction)
    {
        //开始网络检测
        //在该函数内部会去判断当前状态
        StartNetworkCheck();

        return true;
    }
    else if (ACTION_QUIT == strAction)      //返回
    {
        //显示结果的状态才处理
        if (NETWORK_CHECK_SHOWINFO_TYPE_RESULT == m_eShowInfoType)
        {
            //设置当前状态
            m_eShowInfoType = NETWORK_CHECK_SHOWINFO_TYPE_IP;

            //显示IP
            ShowNetworkCheckIP();

            m_strCheckResult = "";      //检测结果

            m_vectorCheckResults.clear();   //检测结果数组

            this->UpdateWnd();

            //编辑框获得焦点
            if (NULL != m_pEditURL)
            {
                //设置焦点
                m_pEditURL->setFocus();

                //删除全选
                m_pEditURL->deselect();
            }

            return true;
        }

    }

    return false;
}

// 处理弹出框回调
void CDlgPingAndTracerouteUI::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL != pMessageBox)
    {
        //输入URL的提示   或者  检测失败
        if (ID_NOTE_INPUT_URL == pMessageBox->GetID() || ID_NOTE_CHECK_ERROR == pMessageBox->GetID())
        {
            //编辑框获得焦点
            if (NULL != m_pEditURL)
            {
                m_pEditURL->setFocus();
            }

            return ;
        }

        //等待的提示
        if (ID_NOTE_WAIT == pMessageBox->GetID())
        {
            if (pMessageBox->GetResult() == MessageBox_Cancel)
            {
                //取消执行检测命令
                Diagnostics_StopSysCmd();

                m_eShowInfoType = NETWORK_CHECK_SHOWINFO_TYPE_IP;       //设置为输入IP状态

                //编辑框获得焦点
                if (NULL != m_pEditURL)
                {
                    m_pEditURL->setFocus();
                }

            }
            else if (pMessageBox->GetResult() == MessageBox_Destory)    //被摧毁  一般是returntoidle
            {
                //取消执行检测命令
                Diagnostics_StopSysCmd();

                m_eShowInfoType = NETWORK_CHECK_SHOWINFO_TYPE_IP;       //设置为输入IP状态

                //编辑框获得焦点
                if (NULL != m_pEditURL)
                {
                    m_pEditURL->setFocus();
                }
            }
            else if (pMessageBox->GetResult() == MessageBox_Close)      //人为关闭
            {
                //说明是检测消息回来 直接关闭弹框了
            }
        }
    }
}

//事件过滤器
bool CDlgPingAndTracerouteUI::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL == pEvent)
    {
        return false;
    }

    //在输入ip界面
    if (NETWORK_CHECK_SHOWINFO_TYPE_IP == m_eShowInfoType)
    {
        //硬按键事件
        if (pEvent->type() == QEvent::KeyPress)
        {
            QKeyEvent * pKeyEventIP = (QKeyEvent *)pEvent;

            if (pKeyEventIP->key() == PHONE_KEY_OK)
            {
                StartNetworkCheck();

                return true;
            }
        }
    }

    //显示结果界面  如果是硬按键事件  案件之在0-9之间 那么不交给外部处理
    if (NETWORK_CHECK_SHOWINFO_TYPE_RESULT == m_eShowInfoType)
    {
        //硬按键事件
        if (pEvent->type() == QEvent::KeyPress)
        {
            //不让framelist 处理  上下按键事件
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;

            if ((pKeyEvent->key() >= PHONE_KEY_0)
                    && (pKeyEvent->key() <= PHONE_KEY_9)
               )
            {
                return true;
            }
        }
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

//处理返回消息
void CDlgPingAndTracerouteUI::OnPingAndTracerouteMsg(msgObject & objMessage)
{
    //如果当前不处于等待状态 那么直接返回
    if (NETWORK_CHECK_SHOWINFO_TYPE_WAIT != m_eShowInfoType)
    {
        return ;
    }
    //处理相关操作
    //当前状态设置为是显示结果状态
    m_eShowInfoType = NETWORK_CHECK_SHOWINFO_TYPE_RESULT;

    //关闭弹出框
    MessageBox_RemoveMessageBox(this, MessageBox_Close);

    //调用接口获取信息
    m_strCheckResult = toQString(Diagnostics_GetDiagnositcsResult());

    ShowNetworkCheckResult();       //显示返回的检测结果

    //调用刷新窗口
    this->UpdateWnd();
}

//开始网络检测
void CDlgPingAndTracerouteUI::StartNetworkCheck()
{
    //不在输入IP的界面 不用处理
    if (NETWORK_CHECK_SHOWINFO_TYPE_IP != m_eShowInfoType)
    {
        return ;
    }

    //先要输出预编辑字符
    InputPreeditContent(m_pEditURL);

    //编辑框中的字符串
    QString strEditURL("");

    if (NULL != m_pEditURL)
    {
        strEditURL = m_pEditURL->text();
    }

    //编辑框中的字符串为空
    if (strEditURL.isEmpty())
    {
        //编辑框失去焦点
        if (NULL != m_pEditURL)
        {
            m_pEditURL->clearFocus();
        }

        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_PLEASE_ENTER_AN_IP_ADDRESS_OR_DOMAIN_NAME) + QString("."), MESSAGEBOX_Cancel,
                                    WAITTIME_NOTE_INPUT_URL, "", "", ID_NOTE_INPUT_URL);

        return ;
    }

    //编辑框失去焦点
    if (NULL != m_pEditURL)
    {
        m_pEditURL->clearFocus();
    }

    DIAGNOSTICSTYPE eSysCMD = DIAGNOSTICSTYPE_NONE;

    //ping
    if (g_strOpenPagePing == m_strSubPageAction)
    {
        eSysCMD = DIAGNOSTICSTYPE_PING;
    }
    else        //trace route
    {
        eSysCMD = DIAGNOSTICSTYPE_TRACEROUTE;
    }

    yl::string strCheckURL = fromQString(strEditURL);

    //执行检测成功
    if (Diagnostics_RunSysCmd(eSysCMD, strCheckURL))
    {
        //调用成功才执行以下语句
        m_eShowInfoType = NETWORK_CHECK_SHOWINFO_TYPE_WAIT;     //设置为等待状态
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_PLEASE_WAIT) + QString("..."),
                                    MESSAGEBOX_Cancel, WAITTIME_NOTE_WAIT, "", "", ID_NOTE_WAIT);
    }
    else        //执行检测失败
    {
        //执行失败的错误信息
        QString strErrorInfo("");

        //ping
        if (g_strOpenPagePing == m_strSubPageAction)
        {
            strErrorInfo == LANG_TRANSLATE(TRID_PING_ERROR);
        }
        else        //trace route
        {
            strErrorInfo == LANG_TRANSLATE(TRID_TRACE_ROUTE_ERROR);
        }

        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_PLEASE_ENTER_AN_IP_ADDRESS_OR_DOMAIN_NAME) + QString("."), MESSAGEBOX_Cancel,
                                    WAITTIME_NOTE_CHECK_ERROR, "", "", ID_NOTE_CHECK_ERROR);
    }
}

//显示检测ip
void CDlgPingAndTracerouteUI::ShowNetworkCheckIP()
{
    //先清除控件信息
    MakeReadyForRelayout();

    if (NULL != m_pGeneralFrame)
    {
        //设置滚动类型为按项滚动
        m_pGeneralFrame->SetScrollViewType(CFrameList::SCROLL_VIEW_TYPE_ITEM);
    }

    QWidgetPair qPair;
    QString strLab;

    //ping
    if (g_strOpenPagePing == m_strSubPageAction)
    {
        strLab = LANG_TRANSLATE(TRID_PING_IP_OR_URL);
    }
    else        //trace route
    {
        strLab = LANG_TRANSLATE(TRID_TRACE_ROUTE_IP_OR_URL);
    }

    qPair.first = strLab;
    qPair.second = m_pEditURL;
    qPair.bAddIndex = true;         //添加序号
    //t48 不绘制背景
    //qPair.bPaintFocusBG = true;       //焦点的时候绘制背景

    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
}

//显示检测结果
void CDlgPingAndTracerouteUI::ShowNetworkCheckResult()
{
    //先清除控件信息
    MakeReadyForRelayout();

    if (NULL != m_pGeneralFrame)
    {
        //设置属性为翻页
        m_pGeneralFrame->SetScrollViewType(CFrameList::SCROLL_VIEW_TYPE_PAGE);
    }

    //显示结果的字体
    QFont fontSplit;
    if (NULL != m_pGeneralFrame)
    {
        fontSplit = m_pGeneralFrame->font();
    }

    //最后一个参数是表示遇到strSplit 就分段（该字符串不会被包含）
    int iSplitCount = SplitStringByFont(m_strCheckResult, fontSplit,
                                        PING_AND_TRACEROUTE_RESULT_ITEM_WIDTH, m_vectorCheckResults);

    for (int i = 0; i < iSplitCount; i++)
    {
        QWidgetPair qPair;
        QString strLab;

        strLab = m_vectorCheckResults[i];
        qPair.first = strLab;
        qPair.eType = SETTING_ITEM_NORMAL;
        qPair.bAddIndex = false;        //不添加序号
        qPair.bAddMark = false;         //末尾不加冒号(:)
        //t48  不绘制背景
        //qPair.bPaintFocusBG = false;      //焦点的时候不绘制背景

        m_vecWidgetPair.push_back(qPair);
    }

    AddWidget(m_vecWidgetPair);
}

//注册检测结果消息
void CDlgPingAndTracerouteUI::RegisterCheckResultMsg()
{
    etl_RegisterMsgHandle(DIAGNOISTICMSG_FINISH,
                          DIAGNOISTICMSG_FINISH, SettingUI::S_DealPingAndTracerouteMsg);
}

//注销检测结果消息
void CDlgPingAndTracerouteUI::UnRegisterCheckResultMsg()
{
    etl_UnregisterMsgHandle(DIAGNOISTICMSG_FINISH,
                            DIAGNOISTICMSG_FINISH, SettingUI::S_DealPingAndTracerouteMsg);
}

//返回值是获取到的字符串长度
int CDlgPingAndTracerouteUI::GetStringByWidth(const QString & strInData, const QFontMetrics & fm,
        int iWidth, QString & strGet)
{
    //先清空数据
    strGet = "";

    //没有长度直接返回0
    if (iWidth <= 0)
    {
        return 0;
    }

    //输入的字符串为空 那么直接返回0
    if (strInData.isEmpty())
    {
        return 0;
    }

    //输入字符串的长度
    int iInDataLength = strInData.length();

    //获取的字符串长度
    int iGetDataLength = 1;

    //获取的字符串宽度
    int iStringFontWidth = 0;

    //之前没有获取过字符串
    if (0 >= m_iMaxGetSplitStringLen)
    {
        for (iGetDataLength = 1; iGetDataLength <= iInDataLength; iGetDataLength++)
        {
            iStringFontWidth = fm.width(strInData.left(iGetDataLength));
            if (iStringFontWidth > iWidth)
            {
                iGetDataLength--;

                break;
            }
        }
    }
    else        //之前已经有获取过   现在要在之前的基础上继续获取
    {
        iGetDataLength = m_iMaxGetSplitStringLen;

        if (iGetDataLength > iInDataLength)
        {
            iGetDataLength = iInDataLength;
        }

        iStringFontWidth = fm.width(strInData.left(
                                        iGetDataLength));         //获取截取字符串的宽度
        if (iStringFontWidth > iWidth)      //比控件的宽度大
        {
            for (iGetDataLength--; iGetDataLength > 0; iGetDataLength--)
            {
                iStringFontWidth = fm.width(strInData.left(iGetDataLength));
                if (iStringFontWidth < iWidth)
                {
                    break;
                }
            }
        }
        else if (iGetDataLength <
                 iInDataLength) //比控件的宽度小  且比输入的字符串长度小  （如果长度比输入字符串的长度还大  那么直接到下一步就可以了）
        {
            for (iGetDataLength++; iGetDataLength <= iInDataLength; iGetDataLength++)
            {
                iStringFontWidth = fm.width(strInData.left(iGetDataLength));
                if (iStringFontWidth > iWidth)
                {
                    iGetDataLength--;

                    break;
                }
            }
        }

    }

    //如果是超过输入字符串的长度  那么获取的字符串长度变成输入字符串长度
    if (iGetDataLength > iInDataLength)
    {
        iGetDataLength = iInDataLength;
    }

    //每次都保存最大的解析字符串长度
    if (iGetDataLength > m_iMaxGetSplitStringLen)
    {
        m_iMaxGetSplitStringLen = iGetDataLength;
    }

    //判断是否单词截断
    if ((iGetDataLength > 0) && (iGetDataLength < iInDataLength))
    {
        if ((strInData.at(iGetDataLength - 1) != ' ') && (strInData.at(iGetDataLength - 1) != '\n'))
        {
            //有 (iGetDataLength < iInDataLength)  的限制  所以下一个肯定还有
            if ((strInData.at(iGetDataLength) != ' ') && (strInData.at(iGetDataLength) != '\n'))
            {
                int iGetDataLengthBak =
                    iGetDataLength;     //先备份一下  因为如果前面都没有找到的话 这边还是需要返回完整的一串
                //往回递减  直到出现空格或者回车
                for (iGetDataLength--; iGetDataLength > 0; iGetDataLength--)
                {
                    if ((strInData.at(iGetDataLength - 1) == ' ') || (strInData.at(iGetDataLength - 1) == '\n'))
                    {
                        break;
                    }
                }

                // 如果前面都没有找到的话 这边还是需要返回完整的一串
                if (iGetDataLength <= 0)
                {
                    iGetDataLength = iGetDataLengthBak;
                }
            }

        }

    }

    if (iGetDataLength > 0)
    {
        strGet = strInData.left(iGetDataLength);
    }
    else
    {
        strGet = "";

        return 0;
    }

    return iGetDataLength;
}

//返回值是获取的字符串组数
int CDlgPingAndTracerouteUI::GetSplitStringsByWidth(const QString & strInData,
        const QFontMetrics & fm, int iWidth, std::vector<QString> & vectorSplitStrings)
{
    //输入字符串为空 直接返回0
    if (strInData.isEmpty())
    {
        return 0;
    }

    //长度太小 那么直接入队列  并且返回1
    if (iWidth <= 0)
    {
        vectorSplitStrings.push_back(strInData);

        return 1;
    }

    QString strGet = "";

    int iGetStringLength = GetStringByWidth(strInData, fm, iWidth, strGet);

    //没有获取到字符串  说明宽度太短了  直接把当前的字符串插入队列就可以了
    if (0 >= iGetStringLength)
    {
        vectorSplitStrings.push_back(strInData);

        return 1;
    }

    //有获取到  那么放入队列
    vectorSplitStrings.push_back(strGet);

    //获取剩下的字符串长度
    int iLeftLength = strInData.length() - iGetStringLength;
    //如果剩下的字符串长度 <= 0  那么直接返回1就可以了
    if (iLeftLength <= 0)
    {
        return 1;
    }

    //获取剩下部分的字符串
    QString strLeft = strInData.right(iLeftLength);

    //如果获取到的剩下部分的字符串  与传入的字符串长度相同 那么直接返回1
    if ((strLeft.length() >= strInData.length()) || (strLeft.length() <= 0))
    {
        return 1;
    }

    //循环调用 获取剩下的字符串
    return GetSplitStringsByWidth(strLeft, fm, iWidth, vectorSplitStrings) + 1;
}

//最后一个参数是表示遇到strSplit 就分段（该字符串不会被包含）
int CDlgPingAndTracerouteUI::SplitStringByFont(const QString & strInData, const QFont & fontSplit,
        int iWidth, std::vector<QString> & vectorSplitStrings, const QString & strSplit)
{
    //先清除字符串数组中的数据
    vectorSplitStrings.clear();

    //输入字符串为空 那么返回
    if (strInData.isEmpty())
    {
        return 0;
    }

    //宽度太小 返回
    if (iWidth <= 0)
    {
        return 0;
    }

    //保存分隔后的字符串数组
    QStringList strListSplit;

    //如果可以拆分 直接拆分
    if (!strSplit.isEmpty())
    {
        strListSplit = strInData.split(strSplit);
    }
    else    //不可以拆分  那么直接当做第一个元素
    {
        strListSplit.append(strInData);
    }

    //拆分的字符串个数
    int iStringCount = strListSplit.size();

    //拆分后字符串个数<=0 直接返回
    if (iStringCount <= 0)
    {
        return 0;
    }

    QFontMetrics fm(fontSplit);

    //获取到的分隔字符串个数
    int iSplitStringCount = 0;

    for (int i = 0; i < iStringCount; i++)
    {
        QString strAt = strListSplit.at(i);
        //为空的话就不需要获取了
        if (strAt.isEmpty())
        {
            continue;
        }

        //返回值是获取的字符串组数
        iSplitStringCount += GetSplitStringsByWidth(strAt, fm, iWidth, vectorSplitStrings);
    }

    return iSplitStringCount;
}

