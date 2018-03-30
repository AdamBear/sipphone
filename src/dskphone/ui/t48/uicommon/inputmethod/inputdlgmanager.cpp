#include <QtGui>
#include "inputdlgmanager.h"
#include "inputdialogfullen.h"
#include "inputdialogfullenshift.h"
#include "inputdialogfullnumber.h"
#include "widgetinfomanager.h"
#include "globalinputhook/modinputhook.h"

#ifdef CUSTOME_KEYBOARD
#include "inputmanagerdefine.h"
#include "keyboardxmlreader.h"
#include "keyboardlayoutmanager.h"
#include "keyboardimemanager.h"
#include "keyboardlangmanager.h"
#include "keyboardswitchmanager.h"
#include "keyboardhintmanager.h"
#include "inputdialogcustom.h"
#endif

CInputDlgManager::CInputDlgManager(void)
{
    m_pInputDlg = NULL;
    m_pInputDlg_AfterHide = NULL;
    m_byCurInputDlgType = INPUT_DLGTYPE_NONE;
    m_bInputDlgIsShow = false;
    m_byStyle = INPUT_DIALOG_NORMAL;
    m_nTimerID = 0;
    //m_pAnimation = NULL;
    m_enAnimationState = ANIMATION_STOP;
    m_bPlayAnimation = false;
    m_eEnterType = ENTER_TYPE_NEXT;
}

CInputDlgManager::~CInputDlgManager(void)
{
    m_mapInputDlgs.clear();
    m_pInputDlg = NULL;
    m_pInputDlg_AfterHide = NULL;
    m_byCurInputDlgType = INPUT_DLGTYPE_NONE;
    m_bInputDlgIsShow = false;
    m_byStyle = INPUT_DIALOG_NORMAL;
    m_nTimerID = 0;
    //m_pAnimation = NULL;
    m_enAnimationState = ANIMATION_STOP;
    m_bPlayAnimation = false;
}

bool CInputDlgManager::RegisterKeyBoardShowHookProc(KEYBOARD_SHOW_HOOK_PROC keyboardShowHook, KEYBOARD_SHOW_HOOK_MODULE priority)
{
    if (keyboardShowHook != NULL)
    {
        //查找回调指针
        KEYBOARD_SHOW_HOOK_MAP::iterator itProc = m_mapHookProc.begin();
        for (; itProc != m_mapHookProc.end(); itProc++)
        {
            KEYBOARD_SHOW_HOOK_PROC pHookProc = itProc.value();
            if (pHookProc != NULL
                    && pHookProc == keyboardShowHook)
            {
                //查找到相同回调指针，不需重复添加
                return true;
            }
        }
        //将回调指针添入列表中
        m_mapHookProc.insertMulti(priority, keyboardShowHook);
    }
    return false;
}

bool CInputDlgManager::UnRegisterKeyBoardShowHookProc(KEYBOARD_SHOW_HOOK_PROC keyboardShowHook)
{
    if (keyboardShowHook != NULL)
    {
        //查找回调指针
        KEYBOARD_SHOW_HOOK_MAP::iterator itProc = m_mapHookProc.begin();
        for (; itProc != m_mapHookProc.end(); itProc++)
        {
            KEYBOARD_SHOW_HOOK_PROC pHookProc = itProc.value();
            if (pHookProc != NULL
                    && pHookProc == keyboardShowHook)
            {
                //查找到相应的回调指针，从链表中清除
                m_mapHookProc.erase(itProc);
                return true;
            }
        }
    }
    return false;
}

void CInputDlgManager::ProcessKeyBoardShow(bool bKeyBoardShow)
{
    //外部模块注册回调调用
    KEYBOARD_SHOW_HOOK_MAP::iterator itProc = m_mapHookProc.begin();
    for (; itProc != m_mapHookProc.end(); itProc++)
    {
        KEYBOARD_SHOW_HOOK_PROC pHookProc = itProc.value();
        if (pHookProc != NULL)
        {
            if (pHookProc(bKeyBoardShow))
            {
                return;
            }
        }
    }
}

#ifdef CUSTOME_KEYBOARD
void CInputDlgManager::InitCustomKeyboard()
{
    INPUTMETHOD_INFO("CInputDlgManager::InitCustomKeyboard");
    //让各自定义管理模块顺序初始化
    g_KeyBoardXmlReader.LoadKeyboardXmlFiles();
    g_KeyBoardLayoutManager.InitKeyboardFromXmlFile();
    g_KeyBoardImeManager.InitKeyboardFromXmlFile();
    g_KeyBoardLangManager.InitKeyboardFromXmlFile();
}
#endif

void CInputDlgManager::Init()
{
    m_mapInputDlgs.clear();

#ifdef CUSTOME_KEYBOARD
    InitCustomKeyboard();

    //只有格式都正确，才能创建键盘
    if (!g_KeyBoardHintManager.ExistErrorHints())
    {
        //遍历Ime列表，分别创建对应Dlg
        const vecKeyboardIme& tvecKeyboardIme = g_KeyBoardImeManager.GetKeyboardIme();
        vecKeyboardIme::const_iterator iterKeyboardIme = tvecKeyboardIme.begin();
        for (; iterKeyboardIme != tvecKeyboardIme.end(); ++iterKeyboardIme)
        {
            CBaseInputDlg* pInputDlgCustom = new CInputDialogCustom((*iterKeyboardIme).m_nImeIndex);
            if (NULL != pInputDlgCustom)
            {
                pInputDlgCustom->Init();
                connect(pInputDlgCustom, SIGNAL(SignalBtnClick(int)), this, SLOT(OnBtnClick(int)));
                m_mapInputDlgs.insert(std::make_pair(pInputDlgCustom->GetDlgType(), pInputDlgCustom));
            }
        }
    }

#else
    CBaseInputDlg* pInputDlgFullEn = new CInputDialogFullEn;
    if (NULL != pInputDlgFullEn)
    {
        pInputDlgFullEn->Init();
        connect(pInputDlgFullEn, SIGNAL(SignalBtnClick(int)), this, SLOT(OnBtnClick(int)));
        m_mapInputDlgs.insert(std::make_pair(pInputDlgFullEn->GetDlgType(), pInputDlgFullEn));
    }

    CBaseInputDlg* pInputDlgFullEnCapital = new CInputDialogFullEnShift;
    if (NULL != pInputDlgFullEnCapital)
    {
        pInputDlgFullEnCapital->Init();
        connect(pInputDlgFullEnCapital, SIGNAL(SignalBtnClick(int)), this, SLOT(OnBtnClick(int)));
        m_mapInputDlgs.insert(std::make_pair(pInputDlgFullEnCapital->GetDlgType(), pInputDlgFullEnCapital));
    }

    CBaseInputDlg* pInputDlgFullNumber = new CInputDialogFullNumber;
    if (NULL != pInputDlgFullNumber)
    {
        pInputDlgFullNumber->Init();
        connect(pInputDlgFullNumber, SIGNAL(SignalBtnClick(int)), this, SLOT(OnBtnClick(int)));
        m_mapInputDlgs.insert(std::make_pair(pInputDlgFullNumber->GetDlgType(), pInputDlgFullNumber));
    }
#endif
}

bool CInputDlgManager::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (IsDlgShow() && NULL != m_pInputDlg)
    {
        return m_pInputDlg->ProcessKeyEvent(nKeyCode, bPress);
    }

    return false;
}

bool CInputDlgManager::ProcessMouseEvent(QObject* pObject, QMouseEvent *pEvent)
{
    if (pObject == NULL || NULL == pEvent)
    {
        return false;
    }
    INPUTMETHOD_INFO("CInputDlgManager::ProcessMouseEvent");
    if (IsDlgShow() && NULL != m_pInputDlg)
    {
        QPoint nPos = pEvent->globalPos();
        QRect rc = m_pInputDlg->geometry();
        QPoint nKeyBoardPos = m_pInputDlg->mapToGlobal(QPoint(0, 0));
        QRect rcwgt(nKeyBoardPos.x(), nKeyBoardPos.y(), rc.width(), rc.height());

        if (rcwgt.contains(nPos))
        {
            INPUTMETHOD_INFO("CInputDlgManager::ProcessMouseEvent return false");
            return false;
        }
        else
        {
            if (m_pInputDlg->IsInputBoxShow())
            {
                m_pInputDlg->ResetKeyBoard(false);
            }
            else
            {
                //如果在键盘外部点击，则重置键盘
                m_pInputDlg->ResetKeyBoard(true);
            }
        }
    }

    return false;
}

void CInputDlgManager::SetIsPlayKeyboardAnimation(bool bPlay)
{
    m_bPlayAnimation = bPlay;
}

void CInputDlgManager::Release()
{

}

bool CInputDlgManager::SetInputDlgByType(INPUT_DLGTYPE enType)
{
#ifdef CUSTOME_KEYBOARD
    //进行INPUT_DLGTYPE到IME_TYPE的转换
    //非Custom才需要转换
    if (INPUT_DLGTYPE_NONE < enType && enType < INPUT_DLGTYPE_CUSTOM)
    {
        enType = g_KeyBoardSwitchManager.GetNewTypeWhenLangChange(enType);
    }
#endif

    if (NULL == m_pInputDlg || enType != m_pInputDlg->GetDlgType())
    {
        std::map<INPUT_DLGTYPE, CBaseInputDlg*>::iterator it;
        it = m_mapInputDlgs.find(enType);

        CBaseInputDlg* pBaseInputDlg = NULL;
        if (it == m_mapInputDlgs.end())
        {
            pBaseInputDlg = CreatInputDlgByType(enType);
        }
        else
        {
            if (NULL == it->second)
            {
                pBaseInputDlg = CreatInputDlgByType(enType);
                m_mapInputDlgs[enType] = pBaseInputDlg;
            }
            else
            {
                pBaseInputDlg = it->second;
            }
        }

        if (NULL == pBaseInputDlg)
        {
            return false;
        }

        if (m_pInputDlg != NULL)
        {
            pBaseInputDlg->SetEnterType(m_pInputDlg->EnterType());
            HideInputDlgAfter();
        }
        m_pInputDlg = pBaseInputDlg;
    }

    return true;
}

void CInputDlgManager::SetInputDlgStyle(INPUT_DIALOG_STYLE enStyle)
{
    if (NULL != m_pInputDlg)
    {
        m_pInputDlg->SetInputDlgStyle(enStyle);
        m_byStyle = enStyle;
    }
}

void CInputDlgManager::SetEnterType(EnterKeyType eEnterType)
{
    INPUTMETHOD_INFO("SetEnterType %p eEnterType %d", m_pInputDlg, eEnterType);
    m_eEnterType = eEnterType;
    if (NULL != m_pInputDlg)
    {
        m_pInputDlg->SetEnterType(eEnterType);
    }
}

void CInputDlgManager::RaiseFullKeyboard()
{
    if (NULL != m_pInputDlg)
    {
        m_pInputDlg->raise();
    }
}

void CInputDlgManager::ShowInputDlg(const INPUT_DLGTYPE enType)
{
    if (enType == m_byCurInputDlgType)
    {
        return;
    }

#ifdef CUSTOME_KEYBOARD
    g_KeyBoardSwitchManager.RecordCurrentIme(enType);

    int nNextIme = g_KeyBoardSwitchManager.FindNextIme();
    if (KEYBOARD_IME_ERROR_INDEX != nNextIme)
    {
        const SKeyboardIme* pKeyboardIme = g_KeyBoardImeManager.GetImeByIndex(nNextIme);
        if (NULL != pKeyboardIme && NULL != m_pInputDlg)
        {
            m_pInputDlg->SetImeDisplayName(pKeyboardIme->m_strDisplayName);
        }
    }
#endif
    /*if ( NULL != m_pAnimation && ANIMATION_EXIST == m_enAnimationState )
    {
        m_pAnimation->stop();
        m_enAnimationState = ANIMATION_STOPING;
    }*/
    if (NULL != m_pInputDlg)
    {
        if (!m_bInputDlgIsShow && NULL == m_pInputDlg_AfterHide)
        {
            emit SignKeyboardBeforeShow();
            INPUTMETHOD_INFO("emit SignKeyboardBeforeShow()............");
            ProcessKeyBoardShow(true);
        }
        m_pInputDlg->show();
        m_pInputDlg->raise();
        m_byCurInputDlgType = m_pInputDlg->GetDlgType();
        //if ( m_bPlayAnimation && !m_bInputDlgIsShow && NULL ==m_pInputDlg_AfterHide )
        //{
        //  m_pAnimation = PlayAnimation(m_pInputDlg ,true ,ANIMATION_MOVE_GEOMETRY ,200);
        //  if ( NULL != m_pAnimation )
        //  {
        //      connect(m_pAnimation, SIGNAL(stateChanged(QAbstractAnimation::State, QAbstractAnimation::State)),
        //          this, SLOT(OnAnimationStateChange(QAbstractAnimation::State, QAbstractAnimation::State)));
        //      m_enAnimationState = ANIMATION_EXIST;
        //  }
        //}else
        {
            m_pInputDlg->setGeometry(g_iFullKeyBoardX, g_iFullKeyBoardY,
                                     g_iFullKeyBoardWidth, g_iFullKeyBoardHeight);
        }

        m_pInputDlg->SetEnterType(m_eEnterType);

        m_bInputDlgIsShow = true;
    }
}

//QAbstractAnimation* CInputDlgManager::PlayAnimation(  QWidget* pAnimWidget,
//                                                    bool IsShow,
//                                                    ANIMATION_TYPE AniType,
//                                                    int nTime)
//{
//  QPropertyAnimation* pAnimation = Animation_CreateAnimation(AniType,
//      pAnimWidget);
//  if ( NULL == pAnimation )
//  {
//      return NULL;
//  }
//  pAnimation->setDuration(nTime);
//  //QRect Rect = pAnimWidget->geometry();
//  if (IsShow)
//  {
//      pAnimation->setStartValue(QRect(0, 800, g_iFullKeyBoardWidth, g_iFullKeyBoardHeight));
//      pAnimation->setEndValue(QRect(0,g_iFullKeyBoardY,g_iFullKeyBoardWidth,g_iFullKeyBoardHeight));
//  }
//  else
//  {
//      pAnimation->setStartValue(QRect(0,g_iFullKeyBoardY,g_iFullKeyBoardWidth,g_iFullKeyBoardHeight));
//      pAnimation->setEndValue(QRect(0, 800, g_iFullKeyBoardWidth, g_iFullKeyBoardHeight));
//  }
//  pAnimation->start(QAbstractAnimation::DeleteWhenStopped);
//  return pAnimation;
//}

void CInputDlgManager::HideInputDlgAfter()
{
    if (SetAfterHideDlg())
    {
        startTimer();
    }
}

void CInputDlgManager::HideInputDlgImmediately()
{
    if (NULL != m_pInputDlg && !m_pInputDlg->isHidden())
    {
        if (NULL != m_pInputDlg_AfterHide && !m_pInputDlg_AfterHide->isHidden())
        {
            m_pInputDlg_AfterHide->hide();
            m_pInputDlg_AfterHide = NULL;
        }
        m_pInputDlg_AfterHide = m_pInputDlg;
        m_pInputDlg = NULL;
        m_bInputDlgIsShow = false;
        m_byCurInputDlgType = INPUT_DLGTYPE_NONE;
        //emit SignKeyboardHide();
        //INPUTMETHOD_INFO("emit SignKeyboardHide()............");
        DealDlgToHide();
    }
    else
    {
        if (0 != m_nTimerID)
        {
            DealDlgToHide();
        }
    }
}

bool CInputDlgManager::SetAfterHideDlg()
{
    if (NULL != m_pInputDlg)
    {
        if (!m_pInputDlg->isHidden())
        {
            stopTimer();
            if (NULL != m_pInputDlg_AfterHide && !m_pInputDlg_AfterHide->isHidden())
            {
                m_pInputDlg_AfterHide->hide();
                m_pInputDlg_AfterHide = NULL;
            }
            m_pInputDlg_AfterHide = m_pInputDlg;
            m_pInputDlg = NULL;
            m_bInputDlgIsShow = false;
            m_byCurInputDlgType = INPUT_DLGTYPE_NONE;
            //startTimer();
            return true;
        }
    }
    return false;
}

CBaseInputDlg* CInputDlgManager::CreatInputDlgByType(INPUT_DLGTYPE enType)
{
    CBaseInputDlg* pBaseInputDlg = NULL;
#ifndef CUSTOME_KEYBOARD
    switch (enType)
    {
    case INPUT_DLGTYPE_NUM:
        pBaseInputDlg = new CInputDialogFullNumber;
        break;
    case INPUT_DLGTYPE_EN:
        pBaseInputDlg = new CInputDialogFullEn;
        break;
    case INPUT_DLGTYPE_ENCAPITAL:
        pBaseInputDlg = new CInputDialogFullEnShift;
        break;
    default:
        break;
    }
#else
    int nImeCount = g_KeyBoardImeManager.GetImeCount();
    int nImeIndex = (int)enType - (int)INPUT_DLGTYPE_CUSTOM;
    if (INPUT_DLGTYPE_NONE != enType && nImeIndex > 0 && nImeIndex < nImeCount)
    {
        pBaseInputDlg = new CInputDialogCustom(nImeIndex);
    }
#endif
    if (pBaseInputDlg != NULL)
    {
        pBaseInputDlg->Init();
        connect(pBaseInputDlg, SIGNAL(SignalBtnClick(int)), this, SLOT(OnBtnClick(int)), Qt::DirectConnection);
        m_mapInputDlgs.insert(std::make_pair(enType, pBaseInputDlg));
        return pBaseInputDlg;
    }
    return NULL;
}

void CInputDlgManager::OnBtnClick(int iKeyCode)
{
    if (NULL == m_pInputDlg)
    {
        return;
    }

#ifdef CUSTOME_KEYBOARD
    ProcessCustomBtnClick();
#else
    if (Qt::Key_Shift == iKeyCode)
    {
        if (m_pInputDlg->GetDlgType() == INPUT_DLGTYPE_EN)
        {
            ChangeDlgType(INPUT_DLGTYPE_ENCAPITAL);
        }
        else if (m_pInputDlg->GetDlgType() == INPUT_DLGTYPE_ENCAPITAL)
        {
            ChangeDlgType(INPUT_DLGTYPE_EN);
        }
    }
    else if (Qt::Key_Control == iKeyCode)
    {
        if (m_pInputDlg->GetDlgType() == INPUT_DLGTYPE_NUM)
        {
            ChangeDlgType(INPUT_DLGTYPE_EN);
        }
        else
        {
            ChangeDlgType(INPUT_DLGTYPE_NUM);
        }
    }
    else if (PHONE_KEY_INPUTMETHOD_HIDEFULLKEYBOARD == iKeyCode)
    {
        HideInputDlgImmediately();
    }
    else
    {
        // 发射模拟按键
        inputHook_FullKb_Sendkey(iKeyCode, true);
        inputHook_FullKb_Sendkey(iKeyCode, false);
    }
#endif
}

void CInputDlgManager::ChangeDlgType(INPUT_DLGTYPE enType)
{
    if (!g_WidgetInfoManager.IsCanTransTo(enType))
    {
        return;
    }
    CBaseInputDlg* pBaseInputDlg = m_pInputDlg;
    if (SetInputDlgByType(enType))
    {
        if (pBaseInputDlg != m_pInputDlg)
        {
            SetInputDlgStyle(m_byStyle);
            ShowInputDlg(enType);

            emit SignChangeDlg(enType);
            if (pBaseInputDlg != NULL)
            {
                // T48全键盘闪烁
                //pBaseInputDlg->hide();
            }
        }
    }
}

//这边不立即隐藏是防止  在不同的控件（界面）之间切换的时候 可能出现的  全键盘先隐藏， 然后在显示的问题（全键盘闪跳）
void CInputDlgManager::startTimer()
{
    // 先停止计时器
    stopTimer();

    // 开启计时并记录ID
    m_nTimerID = QObject::startTimer(100);
}

//这边不立即隐藏是防止  在不同的控件（界面）之间切换的时候 可能出现的  全键盘先隐藏， 然后在显示的问题（全键盘闪跳）
void CInputDlgManager::stopTimer()
{
    // 判断计时器是否激活
    if (m_nTimerID != 0)
    {
        QObject::killTimer(m_nTimerID);
        m_nTimerID = 0;
    }
}

//这边不立即隐藏是防止  在不同的控件（界面）之间切换的时候 可能出现的  全键盘先隐藏， 然后在显示的问题（全键盘闪跳）
void CInputDlgManager::timerEvent(QTimerEvent* pEvent)
{
    DealDlgToHide();
}

void CInputDlgManager::DealDlgToHide()
{
    stopTimer();
    if (NULL != m_pInputDlg_AfterHide)
    {
        //一旦发生隐藏，则需要重置键盘
        m_pInputDlg_AfterHide->ResetKeyBoard(true);

        if (m_pInputDlg_AfterHide == m_pInputDlg && m_bInputDlgIsShow)
        {
            m_pInputDlg_AfterHide = NULL;
            return;
        }

        /*if ( m_bPlayAnimation && NULL == m_pInputDlg )
        {
            if ( NULL != m_pAnimation && ANIMATION_EXIST == m_enAnimationState )
            {
                m_pAnimation->stop();
                m_enAnimationState = ANIMATION_STOPING;
            }
            m_pAnimation = PlayAnimation(m_pInputDlg_AfterHide ,false ,ANIMATION_MOVE_GEOMETRY ,200);
            if ( NULL != m_pAnimation )
            {
                connect(m_pAnimation, SIGNAL(stateChanged(QAbstractAnimation::State, QAbstractAnimation::State)),
                    this, SLOT(OnAnimationStateChange(QAbstractAnimation::State, QAbstractAnimation::State)));
                m_enAnimationState = ANIMATION_EXIST;
            }
        }
        else*/
        {
            m_pInputDlg_AfterHide->hide();
            m_pInputDlg_AfterHide = NULL;
        }
        if (NULL == m_pInputDlg || m_pInputDlg->isHidden())
        {
            emit SignKeyboardHide();
            INPUTMETHOD_INFO("	emit SignKeyboardHide()............");
            SetEnterType(ENTER_TYPE_NEXT);
            ProcessKeyBoardShow(false);
        }
    }
}

bool CInputDlgManager::IsDlgShow()
{
    if (NULL != m_pInputDlg)
    {
        if (!m_pInputDlg->isHidden())
        {
            return true;
        }
        if (NULL != m_pInputDlg_AfterHide)
        {
            if (!m_pInputDlg_AfterHide->isHidden())
            {
                return true;
            }
            return false;
        }
        return false;
    }
    return false;
}


//void CInputDlgManager::OnAnimationStateChange(QAbstractAnimation::State newState,
//                                          QAbstractAnimation::State oldState)
//{
//  if (newState == QAbstractAnimation::Stopped)
//  {
//      if (NULL != m_pInputDlg_AfterHide)
//      {
//          m_pInputDlg_AfterHide->hide();
//          m_pInputDlg_AfterHide = NULL;
//      }
//      if ( NULL != m_pAnimation )
//      {
//          m_enAnimationState = ANIMATION_STOP;
//          disconnect(m_pAnimation, SIGNAL(stateChanged(QAbstractAnimation::State, QAbstractAnimation::State)),
//              this, SLOT(OnAnimationStateChange(QAbstractAnimation::State, QAbstractAnimation::State)));
//
//          m_pAnimation = NULL;
//      }
#ifdef CUSTOME_KEYBOARD

void CInputDlgManager::ProcessCustomEnter()
{
    if (m_byStyle != INPUT_DIALOG_NEWLINE)
    {
        inputHook_FullKb_Sendkey(Qt::Key_Return, true);
        inputHook_FullKb_Sendkey(Qt::Key_Return, false);
    }
    else
    {
        if (m_eEnterType == ENTER_TYPE_NEXT)
        {
            inputHook_SendKey(Qt::Key_Down, true);
            inputHook_SendKey(Qt::Key_Down, false);
        }
        else
        {
            inputHook_FullKb_Sendkey(Qt::Key_Return, true);
            inputHook_FullKb_Sendkey(Qt::Key_Return, false);
        }
    }
}

void CInputDlgManager::ProcessCustomBtnClick()
{
    if (NULL == m_pInputDlg)
    {
        return;
    }

    //获取已知信息，并由此获取目标ImeKey
    int nImeIndex = m_pInputDlg->GetKeyboardImeIndex();
    int nInputboxIndex = m_pInputDlg->GetInputBoxIndex();
    if (nImeIndex <= KEYBOARD_IME_ERROR_INDEX || nInputboxIndex < KEYBOARD_INPUTBOX_ERROR_INDEX)
    {
        return;
    }

    const SKeyboardIme* pKeyboardIme = g_KeyBoardImeManager.GetImeByIndex(nImeIndex);
    if (NULL == pKeyboardIme)
    {
        return;
    }

    int nButtonLine = m_pInputDlg->GetCurrentKeyLine();
    int nButtonID = m_pInputDlg->GetCurrentKeyID();

    const vecKeyboardImeKeyTable& tvecImeKeyTable = pKeyboardIme->m_vecImeKey;
    if (nButtonLine < 0 || nButtonLine >= tvecImeKeyTable.size())
    {
        return;
    }

    const vecKeyboardImeKeyLine& tImeKeyLine = tvecImeKeyTable[nButtonLine];
    if (nButtonID < 0 || nButtonID >= tImeKeyLine.size())
    {
        return;
    }

    const SKeyBoardImeKey& tImeKey = tImeKeyLine[nButtonID];

    //根据ImeKey中所写的Function来进行具体操作
    if (INPUT_IME_FUNCTION_BACKSPACE == tImeKey.m_strFunction)
    {
        inputHook_FullKb_Sendkey(Qt::Key_Backspace, true);
        inputHook_FullKb_Sendkey(Qt::Key_Backspace, false);
    }
    else if (INPUT_IME_FUNCTION_SPACE == tImeKey.m_strFunction)
    {
        inputHook_FullKb_Sendkey(Qt::Key_Space, true);
        inputHook_FullKb_Sendkey(Qt::Key_Space, false);
    }
    else if (INPUT_IME_FUNCTION_ENTER == tImeKey.m_strFunction)
    {
        ProcessCustomEnter();
    }
    else if (INPUT_IME_FUNCTION_HIDE == tImeKey.m_strFunction)
    {
        HideInputDlgImmediately();
    }
    else if (INPUT_IME_FUNCTION_LEFT == tImeKey.m_strFunction)
    {
        inputHook_FullKb_Sendkey(Qt::Key_Left, true);
        inputHook_FullKb_Sendkey(Qt::Key_Left, false);
    }
    else if (INPUT_IME_FUNCTION_RIGHT == tImeKey.m_strFunction)
    {
        inputHook_FullKb_Sendkey(Qt::Key_Right, true);
        inputHook_FullKb_Sendkey(Qt::Key_Right, false);
    }
    else if (INPUT_IME_FUNCTION_UP == tImeKey.m_strFunction)
    {
        inputHook_FullKb_Sendkey(Qt::Key_Up, true);
        inputHook_FullKb_Sendkey(Qt::Key_Up, false);
    }
    else if (INPUT_IME_FUNCTION_DOWN == tImeKey.m_strFunction)
    {
        inputHook_SendKey(Qt::Key_Down, true);
        inputHook_SendKey(Qt::Key_Down, false);
    }
    else if (INPUT_IME_FUNCTION_SHIFT == tImeKey.m_strFunction)
    {
        int nTargetImeIndex = g_KeyBoardSwitchManager.SwitchToCapital(m_pInputDlg->GetDlgType());
        if (KEYBOARD_IME_ERROR_INDEX != nTargetImeIndex && nTargetImeIndex != nImeIndex)
        {
            ChangeDlgType((INPUT_DLGTYPE)(INPUT_DLGTYPE_CUSTOM + nTargetImeIndex));
        }
    }
    else if (INPUT_IME_FUNCTION_LANG == tImeKey.m_strFunction)
    {
        int nTargetImeIndex = KEYBOARD_IME_ERROR_INDEX;

        //如果nInputboxIndex小于等于KEYBOARD_INPUTBOX_ERROR_INDEX，说明没有长按弹备选框或者长按后选择了第一个
        if (nInputboxIndex <= KEYBOARD_INPUTBOX_ERROR_INDEX)
        {
            nTargetImeIndex = g_KeyBoardSwitchManager.SwitchToNextLang();
        }
        //否则根据Inputbox的序号，同时决定Lang的序号
        else
        {
            nTargetImeIndex = g_KeyBoardSwitchManager.SwitchLang(nInputboxIndex);
        }

        INPUTMETHOD_INFO("CInputDlgManager::ProcessCustomBtnClick, Lang Key Clicked, Target Ime Index = %d", nTargetImeIndex);

        //如果目标ImeIndex有效，则切换到对应Ime
        if (KEYBOARD_IME_ERROR_INDEX != nTargetImeIndex && nTargetImeIndex != nImeIndex)
        {
            ChangeDlgType((INPUT_DLGTYPE)(INPUT_DLGTYPE_CUSTOM + nTargetImeIndex));
        }
    }
    else if (INPUT_IME_FUNCTION_SWITCH == tImeKey.m_strFunction)
    {
        int nTargetImeIndex = KEYBOARD_IME_ERROR_INDEX;

        //如果nInputboxIndex小于等于KEYBOARD_INPUTBOX_ERROR_INDEX，说明没有长按弹备选框或者长按后选择了第一个
        if (nInputboxIndex <= KEYBOARD_INPUTBOX_ERROR_INDEX)
        {
            nTargetImeIndex = g_KeyBoardSwitchManager.SwitchToNextIme();
        }
        //否则根据Inputbox的序号，去获取对应的ImeIndex
        else
        {
            //获取所有语言列表
            const vecKeyboardLang& tvecKeyboardLang = g_KeyBoardLangManager.GetKeyboardLang();
            //获取当前语言列表
            const vecKeyboardImeIndex& tvecKeyboardImeIndex = tvecKeyboardLang[g_KeyBoardSwitchManager.GetCurrentLang()].m_vecImeIndex;
            //判断数据是否有效
            int nImeIndexLength = tvecKeyboardImeIndex.size();
            if (nImeIndexLength <= 1 || nInputboxIndex >= nImeIndexLength)
            {
                return;
            }

            //从当前列表的顺序中选出目标Ime的Index
            nTargetImeIndex = g_KeyBoardSwitchManager.SwitchIme(tvecKeyboardImeIndex[nInputboxIndex]);
        }

        INPUTMETHOD_INFO("CInputDlgManager::ProcessCustomBtnClick, Switch Key Clicked, Target Ime Index = %d", nTargetImeIndex);

        //如果目标ImeIndex有效，则切换到对应Ime
        if (KEYBOARD_IME_ERROR_INDEX != nTargetImeIndex && nTargetImeIndex != nImeIndex)
        {
            ChangeDlgType((INPUT_DLGTYPE)(INPUT_DLGTYPE_CUSTOM + nTargetImeIndex));
        }
    }
    else if (!tImeKey.m_strFunction.empty())
    {
        INPUTMETHOD_INFO("CInputDlgManager::ProcessCustomBtnClick, Null Function Key Clicked");
        return;
    }
    //其他全部属于输入字符
    else
    {
        INPUTMETHOD_INFO("CInputDlgManager::ProcessCustomBtnClick, Char Key Clicked");

        g_KeyBoardImeManager.SelectChar(nImeIndex, nButtonLine, nButtonID, nInputboxIndex);
        inputHook_FullKb_Sendkey(PHONE_KEY_INPUTMETHOD_CUSTOM_KEYBOARD, true);
        inputHook_FullKb_Sendkey(PHONE_KEY_INPUTMETHOD_CUSTOM_KEYBOARD, false);
    }
}
#endif

