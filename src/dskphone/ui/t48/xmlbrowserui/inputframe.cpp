#include "inputframe.h"
#include <QKeyEvent>
#include <QLabel>
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uikernel/scrollpage.h"
#include "keymap.h"
#include "xbpositiondefine.h"
#include "uimanager/rectdefine.h"
#include "xmlbrowsermessagebox.h"
#include "extended/xmlbrowser/include/logicanduicommon.h"
#include "devicelib/networkserver.h"
#include "inputmethod/inputmethod.h"
#include "uikernel/qwidgetutility.h"
#include "uikernel/qlineeditutility.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

namespace
{
#define XB_INPUT_FRAME_LABEL_WIDTH 126
#define XB_INPUT_FRAME_EDIT_WIDTH 245
#define XB_INPUT_FRAME_EDIT_OFFSET 4
}

CXMLInputFrame::CXMLInputFrame(QWidget * pParent/* = 0*/)
    : QFrame(pParent)
    , m_eDiaplayMode(MODE_CONDENSED)
    , m_nIndex(0)
    , m_pCurrentFocus(NULL)
    , m_pNowFocus(NULL)
    , m_pParent(pParent)
    , m_bFirstShow(true)
{
    setObjectName("XMLInputFrame");
}

CXMLInputFrame::~CXMLInputFrame()
{
    ClearEdit();
}

void CXMLInputFrame::SetDisplayMode(INPUT_DISPLAY_MODE itemStyle)
{
    if (itemStyle != m_eDiaplayMode)
    {
        m_eDiaplayMode = itemStyle;
    }
}

void CXMLInputFrame::MoveUp()
{
    std::vector<LineEditItem *>::iterator it;
    QLineEdit * pLineEdit ;

    //遍历所有lineEdit，找到当前焦点所在位置
    for (it = m_arrayLineEdit.begin(); it != m_arrayLineEdit.end(); ++it)
    {
        pLineEdit = (*it)->pLineEdit;
        if (pLineEdit->hasFocus())
        {
            //清空原有焦点
            pLineEdit->clearFocus();

            //找到下一个可以编辑的lineEdit
            //不可编辑的也能获取焦点
            //do
            //{
            //如果到第一个，则返回末尾继续
            if (it == m_arrayLineEdit.begin())
            {
                it = (m_arrayLineEdit.end() - 1);

                qSetEnterType(m_arrayLineEdit.size() == 1 ? ENTER_TYPE_ICON : ENTER_TYPE_DONE);
            }
            else
            {
                --it;

                qSetEnterType(m_arrayLineEdit.size() == 1 ? ENTER_TYPE_ICON : ENTER_TYPE_NEXT);
            }
            pLineEdit = (*it)->pLineEdit;
            //} while (pLineEdit->isReadOnly());

            //设置焦点
            pLineEdit->setFocus();
            //设置lineEdit内容为全选
            pLineEdit->selectAll();

            AdjustInput(pLineEdit);

            //发送焦点改变消息给ScrollPage
            emit currentFocusWgtChange(pLineEdit->geometry());
            return;
        }
    }
}

int CXMLInputFrame::GetItemIndex()
{
    std::vector<LineEditItem *>::iterator it;
    QLineEdit * pLineEdit ;
    int iIndex = 0;

    //遍历所有lineEdit，找到当前焦点所在位置
    for (it = m_arrayLineEdit.begin(); it != m_arrayLineEdit.end(); ++it)
    {
        pLineEdit = (*it)->pLineEdit;
        //如果找到焦点，则返回其序号
        if (pLineEdit->hasFocus())
        {
            return iIndex;
        }
        iIndex++;
    }
    //未找到当前焦点所在，返回 -1
    return -1;
}

//得到焦点编辑框的输入类型
INPUT_TYPE CXMLInputFrame::GetFoucsItemInputType()
{
    std::vector<LineEditItem *>::iterator it;
    QLineEdit * pLineEdit ;

    //遍历所有lineEdit，找到当前焦点所在位置
    for (it = m_arrayLineEdit.begin(); it != m_arrayLineEdit.end(); ++it)
    {
        pLineEdit = (*it)->pLineEdit;
        //如果找到焦点，则返回其序号
        if (pLineEdit->hasFocus())
        {
            return (*it)->eInputType;
        }
    }
    //未找到当前焦点所在，返回 -1
    return (INPUT_TYPE) - 1;
}

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
VecSoftkeyParam CXMLInputFrame::GetFocusSoftkey()
{
    std::vector<LineEditItem *>::iterator it;
    QLineEdit * pLineEdit ;
    VecSoftkeyParam vTempSoftkey;
    //遍历所有lineEdit，找到当前焦点所在位置
    for (it = m_arrayLineEdit.begin(); it != m_arrayLineEdit.end(); ++it)
    {
        pLineEdit = (*it)->pLineEdit;
        //如果找到焦点，则返回对应的softkey
        if (pLineEdit->hasFocus())
        {
            vTempSoftkey = (*it)->vecSoftkey;
        }
    }
    return vTempSoftkey;
}
#endif

// 是否是回车键触发向下
// 返回值 是否是最后一个控件
bool CXMLInputFrame::MoveDown(bool bIsEnterKey /* = false */)
{
    std::vector<LineEditItem*>::iterator it;
    QLineEdit* pLineEdit;

    //遍历所有lineEdit，找到当前焦点所在位置
    for (it = m_arrayLineEdit.begin(); it != m_arrayLineEdit.end(); ++it)
    {
        pLineEdit = (*it)->pLineEdit;
        if (pLineEdit->hasFocus())
        {
            //清除原有焦点
            pLineEdit->clearFocus();

            //找到下一个可以编辑的lineEdit
            //不可编辑的也能设置焦点
            //do
            //{
            //如果到末尾则返回第一个继续
            if (it == (m_arrayLineEdit.end() - 1))
            {
                if (bIsEnterKey && FullKeyboardEnabled())
                {
                    return true;
                }
                it = m_arrayLineEdit.begin();
                qSetEnterType(m_arrayLineEdit.size() == 1 ? ENTER_TYPE_ICON : ENTER_TYPE_NEXT);
            }
            else
            {
                ++it;
                if (FullKeyboardEnabled())
                {
                    // 最后一个控件 设置回车键文字
                    if (/*bIsEnterKey &&*/ it == (m_arrayLineEdit.end() - 1))
                    {
                        qSetEnterType(m_arrayLineEdit.size() == 1 ? ENTER_TYPE_ICON : ENTER_TYPE_DONE);
                    }
                    else
                    {
                        qSetEnterType(m_arrayLineEdit.size() == 1 ? ENTER_TYPE_ICON : ENTER_TYPE_NEXT);
                    }
                }
            }
            pLineEdit = (*it)->pLineEdit;
            //} while (pLineEdit->isReadOnly());

            //设置选中的获得焦点
            pLineEdit->setFocus();
            //设置选中的lineEdit内容为全选状态
            pLineEdit->selectAll();

            AdjustInput(pLineEdit);

            //发送焦点位置改变消息
            emit currentFocusWgtChange(pLineEdit->geometry());

            return false;
        }
    }

    return false;
}

void CXMLInputFrame::SetTextColor(QWidget * pWidget, QColor color)
{
    QPalette pal = pWidget->palette();
    pal.setColor(QPalette::Text, color);
    pWidget->setPalette(pal);
}

void CXMLInputFrame::AddItem(std::list<InputField *> List_Input_Field)
{
    int iSize = List_Input_Field.size();
    if (iSize == 0)
    {
        return;
    }
    int iNum = 0;
    int tag = 0;

    //根据不同的排列方式设置Frame的长度
    if (m_eDiaplayMode == MODE_NORMAL)  //分行显示
    {
        tag = iSize;
    }
    else if (m_eDiaplayMode == MODE_CONDENSED)//在同一行显示
    {
        //如果lineEdit不够一个页面，则补齐
        tag = iSize / 2;
        if (iSize % 2)
        {
            ++tag;
        }
    }

    ClearEdit();
    m_arrayLineEdit.clear();
    m_arrayLabel.clear();
    //遍历逻辑层送来的数据，生成所有的label和lineEdit
    std::list<InputField *>::iterator it;
    for (it = List_Input_Field.begin(); it != List_Input_Field.end(); ++it)
    {
        LineEditItem * pLineEditItem = new LineEditItem();
        //插入显示提示信息的label
        QLabel * pLabItem = new QLabel(this);
        //插入lineEdit
        QLineEdit * pLineEdit = new QLineEdit(this);

        if (pLabItem == NULL || pLineEdit == NULL)
        {
            break;
        }
        pLabItem->setText(toQString((*it)->strPrompt.c_str()));
        qWidgetUtility::setFontStyle(pLabItem, XB_INPUT_FRAME_DEFAULT_FONT_SIZE, QFont::Normal, false);
        SetWgtFontColor(pLabItem, FONT_TYPE_TEXT, Qt::white);
        pLabItem->setFocusPolicy(Qt::NoFocus);


        //为每一个lineEdit安装事件过滤器
        pLineEdit->installEventFilter(this);
        qWidgetUtility::setFontStyle(pLineEdit, XB_INPUT_FRAME_DEFAULT_FONT_SIZE, QFont::Normal, false);
        //为lineEdit设置属性
        SetLineEditByInputField(**it, pLineEdit);
        pLineEditItem->pLineEdit = pLineEdit;
        pLineEditItem->eInputType = (*it)->eType;

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
        pLineEditItem->vecSoftkey = (*it)->vecSoftkey;
#endif

        // 判断是否要分行显示
        //每翻一页都要加上偏移量
        int iItemWidth = (RECT_XMLBrowser.width() - XML_SCROLL_WIDTH
                          - XML_SCROLL_LEFT_SPACING - INPUT_FRAME_LEFT_MARGIN) / 2;

        int iTopMargin = INPUT_FRAME_ROW_SPACING / 2;
        if (m_eDiaplayMode == MODE_NORMAL)  //分行显示
        {
            pLabItem->setGeometry(INPUT_FRAME_LEFT_MARGIN,
                                  iTopMargin + iNum * (LINEEDIT_HEIGHT + INPUT_FRAME_ROW_SPACING)
                                  , XB_INPUT_FRAME_LABEL_WIDTH, LINEEDIT_HEIGHT);
            pLineEdit->setGeometry(INPUT_FRAME_LEFT_MARGIN,
                                   iTopMargin + (iNum + 1) * (LINEEDIT_HEIGHT + INPUT_FRAME_ROW_SPACING)
                                   , XB_INPUT_FRAME_EDIT_WIDTH, LINEEDIT_HEIGHT);
            iNum += 2;
        }
        else if (m_eDiaplayMode == MODE_CONDENSED)  //在同一行显示
        {
            pLabItem->setGeometry(INPUT_FRAME_LEFT_MARGIN
                                  , iTopMargin + iNum * (LINEEDIT_HEIGHT + INPUT_FRAME_ROW_SPACING)
                                  , XB_INPUT_FRAME_LABEL_WIDTH, LINEEDIT_HEIGHT);
            pLineEdit->setGeometry(INPUT_FRAME_LEFT_MARGIN + XB_INPUT_FRAME_LABEL_WIDTH
                                   , iTopMargin + iNum * (LINEEDIT_HEIGHT + INPUT_FRAME_ROW_SPACING)
                                   , XB_INPUT_FRAME_EDIT_WIDTH, LINEEDIT_HEIGHT);
            iNum++;
        }

        int inputAreaHeigt = (LINEEDIT_HEIGHT + INPUT_FRAME_ROW_SPACING);

        this->setGeometry(0, 0, RECT_XMLBrowser.width() - XML_SCROLL_WIDTH - XML_SCROLL_LEFT_SPACING -
                          XML_SCROLL_RIGHT_SPACING, inputAreaHeigt * iNum);
        this->setMinimumSize(RECT_XMLBrowser.width() - XML_SCROLL_WIDTH - XML_SCROLL_LEFT_SPACING -
                             XML_SCROLL_RIGHT_SPACING, inputAreaHeigt * iNum);

        //生成的所有lineEdit依次存入m_arrayLineEdit中
        m_arrayLabel.push_back(pLabItem);
        m_arrayLineEdit.push_back(pLineEditItem);
    }

    qSetEnterType(m_arrayLineEdit.size() == 1 ? ENTER_TYPE_ICON : ENTER_TYPE_NEXT);
}

bool CXMLInputFrame::GetLineEditValue(YLHashMap<int, yl::string> & PageData)
{
    m_mapLineEdit.clear();
    int inum = 0;
    std::vector<LineEditItem *>::iterator it;
    QLineEdit * pLineEdit;

    //遍历所有lineEdit，每一个的序号和值存入map中
    for (it = m_arrayLineEdit.begin(); it != m_arrayLineEdit.end(); ++it)
    {
        pLineEdit = (*it)->pLineEdit;
        //获取输入内容包括预输入的
        qReturnEdit(pLineEdit);

        if (!IsValidInput((*it)->eInputType, pLineEdit->text().toUtf8().data()))
        {
            //把焦点设在该输入框
            pLineEdit->setFocus();
            //发送焦点位置改变消息
            emit currentFocusWgtChange(pLineEdit->geometry());
            return false;
        }

        m_mapLineEdit[inum++] = pLineEdit->text().toUtf8().data();
    }
    PageData = m_mapLineEdit;
    return true;
}

// 根据InputField创建LineEdit
void CXMLInputFrame::SetLineEditByInputField(const InputField & objInputField,
        QLineEdit * pLineEdit)
{
    //入口参数检查
    if (pLineEdit == NULL)
    {
        return;
    }

    // 设置控件的ObjName
    pLineEdit->setObjectName("QLineEdit");

    // 设置是否为密码模式
    if (objInputField.bPassword)
    {
        pLineEdit->setEchoMode(QLineEdit::Password);
        pLineEdit->setAttribute(Qt::WA_InputMethodEnabled, TRUE);
    }

    // 设置默认的显示内容
    if (!objInputField.strDefault.empty())
    {
        pLineEdit->setText(toQString(objInputField.strDefault.c_str()));
    }

    // 设置是否可编辑
    //不可编辑的类型有两种：1.edit = false。2.type = empty
    if (!objInputField.bEditable || objInputField.eType == INPUT_EMPTY)
    {
        //设置value和extension两个编辑框在不可编辑状态下的字体颜色
        qLineEditUtility::SetLineEditStype(pLineEdit);
        //pLineEdit->setDisabled(true);
        pLineEdit->setReadOnly(true);

        //如果不可编辑则不需要再安装输入法和最大长度
        return;
    }

    // 设置输入法
    switch (objInputField.eType)
    {
    case INPUT_NUMBER:
    case INPUT_IP:
        {
            // 如果不设置为全部类型的话，自定义输入法时，就无法切换
            qInstallIME(pLineEdit, IME_DIGIT);
        }
        break;
    case INPUT_STRING:
        {
            qInstallIME(pLineEdit, IME_Name);

            yl::string strDefaultIME = configParser_GetConfigString(kszXmlInputStringDefaultIME);
            if (!strDefaultIME.empty())
            {
                qSetCurrentIME(pLineEdit, strDefaultIME.c_str(), 0, kszXmlInputStringDefaultIME);
            }
        }
        break;
    case INPUT_TIME_US:
    case INPUT_TIME_INT:
    case INPUT_DATE_US:
    case INPUT_DATE_INT:
        {
            qInstallIME(pLineEdit, IME_ALL);
        }
        break;
    default:
        {
            qInstallIME(pLineEdit, IME_ALL);
        }
        break;
    }

    //设置输入最大长度
    pLineEdit->setMaxLength(LineEditMaxLength);

    qUseKeyBoardFromIME(pLineEdit, INPUT_DIALOG_NEWLINE, false);
}

bool CXMLInputFrame::eventFilter(QObject * pObj, QEvent * pEvent)
{
    //入口参数检查
    if (pEvent == NULL)
    {
        return false;
    }

    //当焦点改变时进行处理
    if (pEvent->type() == QEvent::FocusIn)
    {
        QWidget* pWgt = focusWidget();
        if (pWgt != NULL)
        {
            emit FocusWgtChange();  //焦点改变，则发送信号
        }
        if (pWgt != NULL && pWgt->inherits("QLineEdit"))
        {
            QLineEdit* pLineEdit = (QLineEdit*)pWgt;
            if (!pLineEdit->isReadOnly())
            {
                pLineEdit->end(false);            //设置光标在末尾

                qSetIsShowWnd(pLineEdit, true);
            }
        }
    }

    if (FullKeyboardEnabled() && pEvent->type() == QEvent::FocusOut)
    {
        if (pObj->inherits("QLineEdit"))
        {
            QLineEdit *pEdit = static_cast<QLineEdit *>(pObj);
            qReturnEdit(pEdit);
        }
    }

    if (FullKeyboardEnabled() && pEvent->type() == QEvent::MouseButtonPress)
    {
        QWidget* pWgt = focusWidget();

        if (pWgt != NULL)
        {
            emit FocusWgtChange();  //焦点改变，则发送信号
        }
        if (pWgt != NULL && pWgt->inherits("QLineEdit"))
        {
            QLineEdit* pLineEdit = (QLineEdit*)pWgt;
            if (!pLineEdit->isReadOnly())
            {
                AdjustInput(pLineEdit);
                qSetIsShowWnd(pLineEdit, true);
                if (m_arrayLineEdit.size() != 1)
                {
                    qSetEnterType(IsLastItem(pLineEdit) ? ENTER_TYPE_DONE : ENTER_TYPE_NEXT);
                }
                else
                {
                    qSetEnterType(ENTER_TYPE_ICON);
                }
            }
        }
    }

    return false;
}

void CXMLInputFrame::SetFrameFocusItem(int iIndex)
{
    //记录默认焦点所在序号
    m_nIndex = iIndex;
}

void CXMLInputFrame::showEvent(QShowEvent * pEvent)
{
    std::vector<LineEditItem *>::iterator it;
    QLineEdit * pLineEdit;

    if (!m_bFirstShow && FullKeyboardEnabled())
    {
        return;
    }

    int nIndex = 0;
    //找到默认焦点所在的lineEdit
    for (it = m_arrayLineEdit.begin(); it != m_arrayLineEdit.end(); ++it)
    {
        pLineEdit = (*it)->pLineEdit;
        pLineEdit->setFocus();
        //不限制不可编辑的输入框
        //if (!pLineEdit->isReadOnly())
        {
            if (m_nIndex == nIndex)
            {
                pLineEdit->setFocus();   //设置默认焦点
                //发送焦点位置改变消息
                emit currentFocusWgtChange(pLineEdit->geometry());
                m_bFirstShow = false;
                return;
            }
            ++nIndex;
        }
    }
}

void CXMLInputFrame::AdjustInput(QLineEdit* pEdit, bool bReset)
{
    if (!FullKeyboardEnabled())
    {
        return;
    }
    if (bReset)
    {
        if (m_pParent->inherits("CScrollPage"))
        {
            CScrollPage* pScroll = (CScrollPage*)m_pParent;
            pScroll->RefreshCurPage();
            this->update();
        }
        return;
    }

    if (pEdit == NULL)
    {
        return;
    }

    // 判断lineedit位置
    QPoint ptEdit = mapToGlobal(pEdit->pos());
    QRect rcFrame = this->geometry();

    if (ptEdit.y() + pEdit->height() + XB_INPUT_FRAME_EDIT_OFFSET > g_iFullKeyBoardY)
    {
        rcFrame.setY(rcFrame.y() - ptEdit.y() - pEdit->height() - XB_INPUT_FRAME_EDIT_OFFSET + g_iFullKeyBoardY);
        rcFrame.setHeight(this->height());
        this->setGeometry(rcFrame);
    }
    else if (ptEdit.y() - XB_INPUT_FRAME_EDIT_OFFSET - TITLE_HEIGHT - RECT_XML_BROWSER_DIALOG.y() < 0)
    {
        rcFrame.setY(rcFrame.y() + RECT_XML_BROWSER_DIALOG.y() + TITLE_HEIGHT + XB_INPUT_FRAME_EDIT_OFFSET - ptEdit.y());
        rcFrame.setHeight(this->height());
        this->setGeometry(rcFrame);
    }

}

void CXMLInputFrame::OnKeyboardShow()
{
    //
}

void CXMLInputFrame::OnKeyboardHide()
{
    AdjustInput(NULL, true);
    update();
}

bool CXMLInputFrame::IsLastItem(QLineEdit* pEdit)
{
    if (pEdit == NULL)
    {
        return false;
    }

    std::vector<LineEditItem*>::iterator it;
    QLineEdit* pLineEdit;

    //遍历所有lineEdit，找到所在位置
    for (it = m_arrayLineEdit.begin(); it != m_arrayLineEdit.end(); ++it)
    {

        pLineEdit = (*it)->pLineEdit;
        if (pLineEdit == pEdit)
        {
            if (it == m_arrayLineEdit.end() - 1)
            {
                return  true;
            }
        }
    }
    return false;
}

bool CXMLInputFrame::ProcessKeyPress(QKeyEvent* pKeyEvent)
{
    //容错处理
    if (pKeyEvent == NULL)
    {
        return false;
    }

    //只响应上下键
    int nKey = pKeyEvent->key();
    switch (nKey)
    {
    case PHONE_KEY_UP:
        {
            MoveUp();   //调用焦点上移函数
        }
        break;
    case PHONE_KEY_DOWN:
        {
            MoveDown(); //调用焦点下移函数
        }
        break;
    case PHONE_KEY_OK:
        {
            return MoveDown(true);  //调用焦点下移函数
        }
        break;
    default:
        return true;
        break;
    }
    return false;
}

void CXMLInputFrame::keyPressEvent(QKeyEvent * pKeyEvent)
{
    //容错处理
    if (pKeyEvent == NULL)
    {
        return;
    }

    //只响应上下键
    int nKey = pKeyEvent->key();
    switch (nKey)
    {
    case PHONE_KEY_UP:
        {
            MoveUp();   //调用焦点上移函数
        }
        break;
    case PHONE_KEY_DOWN:
        {
            MoveDown(); //调用焦点下移函数
        }
        break;
    default:
        break;
    }
    return;
}

void CXMLInputFrame::paintEvent(QPaintEvent * pEevnt)
{
    //paintEvent负责给每行画分割线
    QStylePainter painter(this);
    std::vector<LineEditItem *>::iterator it;
    QLineEdit * pLineEdit;
    QLabel * pLabel;
    int iTopMargin = INPUT_FRAME_ROW_SPACING / 2;
    int iRowPerPage = this->height() / (LINEEDIT_HEIGHT + INPUT_FRAME_ROW_SPACING);
    int i = 0;

    for (it = m_arrayLineEdit.begin(); it != m_arrayLineEdit.end(); ++it, ++i)
    {
        if (m_eDiaplayMode == MODE_NORMAL && m_arrayLabel.size() > i)
        {
            pLabel = m_arrayLabel[i];
            if (NULL != pLabel && pLabel->isVisible())
            {
                QRect rect = pLabel->geometry();
                painter.drawPixmap(0, rect.y() - (INPUT_FRAME_ROW_SPACING - iTopMargin), width(), 2,
                                   THEME_GET_BMP(PIC_GENERAL_LIST_ITEM_RIGHT_LINE));
            }
        }

        pLineEdit = (*it)->pLineEdit;
        if (NULL != pLineEdit && pLineEdit->isVisible())
        {
            QRect rect = pLineEdit->geometry();
            painter.drawPixmap(0, rect.y() - (INPUT_FRAME_ROW_SPACING - iTopMargin), width(), 2,
                               THEME_GET_BMP(PIC_GENERAL_LIST_ITEM_RIGHT_LINE));
        }
    }

    if (MODE_NORMAL == m_eDiaplayMode)
    {
        i = 2 * i;
    }

    if (NULL != pLineEdit && i % iRowPerPage != 0)
    {
        //最后一项后面画线
        QRect rect = pLineEdit->geometry();
        painter.drawPixmap(0, rect.bottom() + (INPUT_FRAME_ROW_SPACING), width(), 2,
                           THEME_GET_BMP(PIC_GENERAL_LIST_ITEM_RIGHT_LINE));
    }
}

bool CXMLInputFrame::IsValidInput(INPUT_TYPE eType, yl::string strValue)
{
    if (eType == INPUT_IP)
    {
        if (!strValue.empty()
                && !netIsValidIP(strValue.c_str()))
        {
            _XBMessageManager.ShowMessage(XB_TIP_INVALID_IP, 2000);
            return FALSE;
        }
    }
    else if (eType == INPUT_DATE_INT)
    {
        // dateInt  DD/MM/YYYY 其中 MM:1-12,DD:1-31,YYYY:0000-9999    31/01/2010
        int iMon = 0, iDate = 0, iYear = 0;
        if (!strValue.empty()
                && sscanf(strValue.c_str(), "%d/%d/%d", &iDate, &iMon, &iYear) == 3
                && IsDateValid(iYear, iMon, iDate))
        {
            return TRUE;
        }
        _XBMessageManager.ShowMessage(XB_TIP_INVALID_DATE_INT, 2000);
        return FALSE;
    }
    else if (eType == INPUT_DATE_US)
    {
        // dateUS   MM/DD/YYYY 其中MM:1-12,DD:1-31,YYYY:0000-9999 12/31/2009
        int iMon = 0, iDate = 0, iYear = 0;
        if (!strValue.empty()
                && sscanf(strValue.c_str(), "%d/%d/%d", &iMon, &iDate, &iYear) == 3
                && IsDateValid(iYear, iMon, iDate))
        {
            return TRUE;
        }
        _XBMessageManager.ShowMessage(XB_TIP_INVALID_DATE_US, 2000);
        return FALSE;
    }
    else if (eType == INPUT_TIME_INT)
    {
        // timeInt  HH:MM:SS   其中HH:0-23, MM:0-59, SS:0-59  23:25:00
        int iHour = 0, iMinute = 0, iSecond = 0;
        if (!strValue.empty()
                && sscanf(strValue.c_str(), "%d:%d:%d", &iHour, &iMinute, &iSecond) == 3
                && iHour >= 0 && iHour <= 23
                && iMinute >= 0 && iMinute <= 59
                && iSecond >= 0 && iSecond <= 59)
        {
            return TRUE;
        }
        _XBMessageManager.ShowMessage(XB_TIP_INVALID_TIME_INT, 2000);
        return FALSE;
    }
    else if (eType == INPUT_TIME_US)
    {
        // timeUS  HH:MM:SS AM/PM 其中HH:1-12, MM:0-59, SS:0-59
        int iHour = 0, iMinute = 0, iSecond = 0;
        char szBuf[128] = "";
        if (!strValue.empty()
                && sscanf(strValue.c_str(), "%d:%d:%d %s", &iHour, &iMinute, &iSecond, szBuf) == 4
                && iHour >= 1 && iHour <= 12
                && iMinute >= 0 && iMinute <= 59
                && iSecond >= 0 && iSecond <= 59
                && (strcmp(szBuf, "AM") == 0 || strcmp(szBuf, "PM") == 0))
        {
            return TRUE;
        }
        _XBMessageManager.ShowMessage(XB_TIP_INVALID_TIME_US, 2000);
        return FALSE;
    }
    return TRUE;
}

BOOL CXMLInputFrame::IsDateValid(int iYear, int iMon, int iDate)
{
    int iLeapYear = 0;
    int arrMonData[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (iYear < 0
            || iYear > 10000)
    {
        return FALSE;
    }

    if ((iYear % 4 == 0 && iYear % 100 != 0)
            || iYear % 400 == 0)
    {
        iLeapYear = 1;
    }

    if (iMon < 1
            || iMon > 12)
    {
        return FALSE;
    }
    // 闰年2月多1天
    arrMonData[1] += iLeapYear;

    if (iDate < 1
            || iDate > arrMonData[iMon - 1])
    {
        return FALSE;
    }

    return TRUE;
}

void CXMLInputFrame::ClearEdit()
{
    //释放内存
    std::vector<LineEditItem *>::iterator it;
    for (it = m_arrayLineEdit.begin(); it != m_arrayLineEdit.end(); ++it)
    {
        if (NULL != *it)
        {
            if ((*it)->pLineEdit != NULL)
            {
                qUninstallIME((*it)->pLineEdit);
                (*it)->pLineEdit->deleteLater();
                (*it)->pLineEdit = NULL;
            }

            delete *it;
        }
    }

    m_arrayLineEdit.clear();
}
