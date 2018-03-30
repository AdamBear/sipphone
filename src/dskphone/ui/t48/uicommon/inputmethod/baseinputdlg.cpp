#include "baseinputdlg.h"
#include "mainwnd/modmainwnd.h"
#include "uimanager/moduimanager.h"
#include "uikernel/languagehelper.h"
#include "translateiddefine.h"
#include "mainwnd/modmainwnd.h"
#include "uimanager/basemainwnd.h"

//竖直间隔
#define KEYBOARD_V_SEPE     17
//水平间隔
#define KEYBOARD_H_SEPE     15


CBaseInputDlg::CBaseInputDlg(void)
    : m_pInputButtonEnter(NULL)
    , m_bIfOneButtonDown(false)
    , m_nButtonLine(-1)
    , m_nButtonID(-1)
    , m_eEnterType(ENTER_TYPE_NEXT)
{
    m_pInputButtonEnter = NULL;
}

CBaseInputDlg::~CBaseInputDlg(void)
{
    m_pInputButtonEnter = NULL;
}

void CBaseInputDlg::Init()
{
    NewButton();
    SetPos();
    LoadBkGroundPic();
    SetInputDlgStyle(INPUT_DIALOG_ENTER);
    setFocusPolicy(Qt::NoFocus);
}

void CBaseInputDlg::SetPos()
{
    this->setWindowFlags(Qt::Tool);
    this->setParent(UIManager_GetMainWnd());

    this->setGeometry(g_iFullKeyBoardX, g_iFullKeyBoardY,
                      g_iFullKeyBoardWidth, g_iFullKeyBoardHeight);
    this->hide();

}

CInputButton* CBaseInputDlg::CreateButton(QRect qRect, int iKeyCode, QString qstrNormalText, QString qstrPressText, QColor qColorNormalText, QColor qColorPressText, QColor qColorNormalBackGround, QColor qColorPressBackGround, QString qstrNormalIcon, QString qstrPressIcon, KButtonType kButtonType)
{
    CInputButton* v_pInputButton = NULL;
    if (kButtonType == KButtonType_LongPress)
    {
        v_pInputButton = new CLongPressButton(this);
    }
    else
    {
        v_pInputButton = new CInputButton(this);
    }
    if (NULL == v_pInputButton)
    {
        return NULL;
    }

    v_pInputButton->SetParent(this);

    //记录按钮相对键盘的位置
    v_pInputButton->m_rctButton = qRect;
    //记录按钮相对全局的位置
    QRect rctGlobel = QRect(g_iFullKeyBoardX + qRect.left() - (KEYBOARD_V_SEPE / 2)
                            , g_iFullKeyBoardY + qRect.top() - (KEYBOARD_H_SEPE / 2)
                            , qRect.width() + KEYBOARD_V_SEPE
                            , qRect.height() + KEYBOARD_H_SEPE);
    v_pInputButton->m_rctGlobalButton = rctGlobel;

    v_pInputButton->setIntBtnAction(iKeyCode);

    v_pInputButton->SetNormalText(qstrNormalText);
    v_pInputButton->SetPressText(qstrPressText);

    v_pInputButton->setNormalTextColor(qColorNormalText);
    v_pInputButton->SetPressTextColor(qColorPressText);

    v_pInputButton->SetNormalBackGround(qColorNormalBackGround);
    v_pInputButton->SetPressBackGround(qColorPressBackGround);

    v_pInputButton->setIconPic(qstrNormalIcon);
    v_pInputButton->SetPressIcon(qstrPressIcon);

    v_pInputButton->setIconPicAlign(Qt::AlignCenter);

    if (Qt::Key_Return == iKeyCode ||
            PHONE_KEY_INPUTMETHOD_HIDEFULLKEYBOARD == iKeyCode ||
            Qt::Key_Shift == iKeyCode ||
            Qt::Key_Control == iKeyCode)
    {
        //http://10.3.5.199/Bug.php?BugID=74066
        //由于按下时响应导致全键盘收起后抬起事件最终被界面处理
        //而该界面的处理方法是点击非输入框区域就清除焦点
        //所以这些按键应抬起响应
        v_pInputButton->SetIsResponePress(false);
//      connect(v_pInputButton, SIGNAL(released(int)), this, SLOT(OnBtnClick(int)));
    }
    else
    {
        //所有按钮都采用按下响应
        v_pInputButton->SetIsResponePress(true);
//      connect(v_pInputButton, SIGNAL(pressed(int)), this, SLOT(OnBtnClick(int)));
    }

    if (kButtonType == KButtonType_LongPress)
    {
        connect(v_pInputButton, SIGNAL(SignalLongPress(int)), this, SLOT(OnBtnClick(int)));
    }

    return v_pInputButton;
}

void CBaseInputDlg::SetInputDlgStyle(INPUT_DIALOG_STYLE enStyle)
{
    if (NULL == m_pInputButtonEnter)
    {
        return;
    }
    if (INPUT_DIALOG_NEWLINE == enStyle)
    {
        //m_pInputButtonEnter->setIconPath(g_qstrFullKeyBoardEnterIcon);
        m_pInputButtonEnter->setIconPic(g_qstrFullKeyBoardNewLine);
//      SetEnterBtnStyle(enStyle);
    }
    else if (INPUT_DIALOG_DIAL == enStyle)
    {
        //qstrBtnPixmap = g_qstrFullKeyBoardNumSend;
    }
    else if (INPUT_DIALOG_SEARCH == enStyle)
    {
        m_pInputButtonEnter->setIconPic(g_qstrFullKeyBoardSearchIcon);
    }
    else
    {
        m_pInputButtonEnter->setIconPic(g_qstrFullKeyBoardEnterIcon);
    }
}

void CBaseInputDlg::OnBtnClick(int iKeyCode)
{
    emit SignalBtnClick(iKeyCode);
}

void CBaseInputDlg::paintEvent(QPaintEvent* pe)
{
    QStylePainter p(this);

    if (!m_qCurrPixMap.isNull())
    {
        p.drawPixmap(0,
                     0, m_qCurrPixMap);
    }

    if (m_bIfOneButtonDown
            && m_nButtonLine > -1
            && m_nButtonLine < VIRTUAL_KEYBOARD_LINE_COUNT
            && m_nButtonID > -1
            && m_nButtonID < m_arrButtonVec[m_nButtonLine].size())
    {
        CInputButton*   pInputButton = m_arrButtonVec[m_nButtonLine][m_nButtonID];
        if (NULL != pInputButton)
        {
            pInputButton->paintInputButton(p);
        }
    }

    if (NULL != m_pInputButtonEnter)
    {
        m_pInputButtonEnter->paintInputButton(p);
    }
}

void CBaseInputDlg::SetEnterBtnStyle(INPUT_DIALOG_STYLE enStyle)
{
    if (NULL == m_pInputButtonEnter)
    {
        return;
    }
    if (INPUT_DIALOG_NEWLINE == enStyle)
    {
        // 换行不用图标
        m_pInputButtonEnter->setIconPic("");
        m_pInputButtonEnter->SetNormalText(LANG_TRANSLATE(TRID_INPUTMETHOD_ENTER_NEXTLINE));
        m_pInputButtonEnter->SetPressText(LANG_TRANSLATE(TRID_INPUTMETHOD_ENTER_NEXTLINE));
        m_pInputButtonEnter->setNormalTextColor(Qt::white);
        m_pInputButtonEnter->SetPressTextColor(Qt::white);
    }

}

bool CBaseInputDlg::event(QEvent* pEvent)
{
    if (NULL != pEvent
            && (pEvent->type() == QEvent::MouseButtonPress
                || pEvent->type() == QEvent::MouseButtonRelease))
    {
        QMouseEvent* pMouseEvent = static_cast<QMouseEvent*>(pEvent);
        ProcessMouseEvent(pMouseEvent);
        return true;
    }

    return QWidget::event(pEvent);
}

bool CBaseInputDlg::ProcessMouseEvent(QMouseEvent *pMouseEvent)
{
    if (NULL == pMouseEvent)
    {
        return false;
    }

    //先判断点在哪一行
    int nLine = -1;

    //当前点在键盘内的点位置
    QPoint nGlobalPos = pMouseEvent->globalPos();

    if (RECT_VIRTUAL_KEYBOARD_1_LINE.contains(nGlobalPos))
    {
        nLine = 0;
    }
    else if (RECT_VIRTUAL_KEYBOARD_2_LINE.contains(nGlobalPos))
    {
        nLine = 1;
    }
    else if (RECT_VIRTUAL_KEYBOARD_3_LINE.contains(nGlobalPos))
    {
        nLine = 2;
    }
    else if (RECT_VIRTUAL_KEYBOARD_4_LINE.contains(nGlobalPos))
    {
        nLine = 3;
    }
    else
    {
        //如果不是点在上述区域则不处理
        return false;
    }

    //被点的按钮的ID（当前行）
    int nDownButton;
    //当前行按钮总数
    int nCount = m_arrButtonVec[nLine].size();

    int nType = pMouseEvent->type();
    if (nType == QEvent::MouseButtonPress)
    {
        //如果已经有按钮按下，则不再接受新的按钮按下
        if (m_bIfOneButtonDown)
        {
            return true;
        }

        vecButton& vecButtonRef = m_arrButtonVec[nLine];
        for (nDownButton = 0; nDownButton < nCount; ++nDownButton)
        {
            CInputButton*   pInputButton = vecButtonRef[nDownButton];

            if (NULL != pInputButton && pInputButton->InputButtonMousePress(nGlobalPos))
            {
                //记录该button信息
                m_bIfOneButtonDown = true;
                m_nButtonLine = nLine;
                m_nButtonID = nDownButton;
                pInputButton->RepaintInputButton();
                if (pInputButton->GetIsResponePress())
                {
                    OnBtnClick(pInputButton->getIntBtnAction());
                }
                return true;
            }
        }
        //如果走完整个循环都没有按钮处理，则是点到空白处了
        ResetKeyBoard(true);
    }
    else if (nType == QEvent::MouseButtonRelease)
    {
        //如果记录中没有被按下的按钮，则直接返回
        if (!m_bIfOneButtonDown)
        {
            ResetKeyBoard(true);
            return false;
        }

        //如果抬起的按钮与记录中的不在同一行，则应重置按下状态
        if ((nLine > -1 && nLine != m_nButtonLine))
        {
            ResetKeyBoard(true);
            return true;
        }

        vecButton& vecButtonRef = m_arrButtonVec[nLine];
        for (nDownButton = 0; nDownButton < nCount; ++nDownButton)
        {
            CInputButton*   pInputButton = vecButtonRef[nDownButton];
            if (NULL != pInputButton && pInputButton->InputButtonMouseRelease(nGlobalPos))
            {
                //如果抬起的点与按下的点是在同一个按钮内，则执行该按钮
                if (m_nButtonLine == nLine
                        && m_nButtonID == nDownButton)
                {
                    ResetKeyBoard(true);
                    if (!pInputButton->GetIsResponePress())
                    {
                        OnBtnClick(pInputButton->getIntBtnAction());
                    }
                    return true;
                }
                //否则重置按下状态
                else
                {
                    ResetKeyBoard(true);
                    return true;
                }
            }
        }
        //如果走完整个循环都没有按钮处理，则是点到空白处了
        ResetKeyBoard(true);
    }

    return false;
}

void CBaseInputDlg::ResetKeyBoard(bool bshowInputBox)
{
    if (m_nButtonLine > -1
            && m_nButtonLine < VIRTUAL_KEYBOARD_LINE_COUNT
            && m_nButtonID > -1
            && m_nButtonID < m_arrButtonVec[m_nButtonLine].size())
    {
        CInputButton* pInputButton = m_arrButtonVec[m_nButtonLine][m_nButtonID];
        if (NULL != pInputButton)
        {
            pInputButton->ResetInputButtonDown();
            pInputButton->RepaintInputButton();
        }
    }

    //清除button信息
    m_bIfOneButtonDown = false;
    m_nButtonLine = -1;
    m_nButtonID = -1;

    if (NULL != m_pInputButtonDelete)
    {
        m_pInputButtonDelete->ResetInputButtonDown();
    }

    update();
}

