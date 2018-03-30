#include "softkeybardelegate.h"
#include "mainwnd/mainwnd.h"
#include "uimanager/rectdefine.h"
#include "uikernel/qsoftkeybar.h"
#include "baseui/t4xpicpath.h"
#include "uimanager/moduimanager.h"
#include "imagemanager/modimagemanager.h"
#include "include/uicommon.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "inputmethod/inputmethod.h"

#include "edk/include/modedk.h"
#include "edkui//modedkui.h"

const QRect RECT_SOFTKEYBAR(160, PHONE_HEIGHT - 90, 480, 90);//  softkey bar位置

CSoftKeyBarDelegate::CSoftKeyBarDelegate()
{
    m_pSoftkeyBar = new qSoftKeyBar();
}

CSoftKeyBarDelegate::~CSoftKeyBarDelegate()
{

}

void CSoftKeyBarDelegate::DrawWidget(CBaseDialog * pDialog)
{
    if (m_pSoftkeyBar.isNull())
    {
        return;
    }

    CArraySoftKeyBarData objSoftKeyBarData;
    bool bShowSoftkey = true;

    if (NULL != pDialog
            && pDialog == UIManager_GetTopWindow())
    {
        // 设置子界面的softkey数据
        // 不管显示或者隐藏softkey都需要刷新softkey数据
        // 用于清空上次的softkey数据
        bShowSoftkey = pDialog->IsShowSoftkey();
        if (bShowSoftkey)
        {
            pDialog->GetSoftkey(objSoftKeyBarData);
        }
    }
    else
    {
        return ;
    }

    if (!bShowSoftkey)
    {
        m_pSoftkeyBar->hide();
        return ;
    }

    if (Edk_IsEnable())
    {
        InsertEDKSoftkeyData(objSoftKeyBarData);
    }

    m_arrSoftKeyBarData.assign(objSoftKeyBarData.begin(), objSoftKeyBarData.end());

    int DlgSoftkeyType = pDialog->GetSoftkeySoftkeyType();
    switch (DlgSoftkeyType)
    {
    case STYLE_TYPE_ICONTRAY:
        {
            // 设置图标显示在文字上方
            m_pSoftkeyBar->SetSoftkeyType(STYLE_TYPE_ICONTRAY);
            QPixmap pmIcon = THEME_GET_BMP(PIC_SOFTKEY_IDLE_BACKGROUND);
            if (!pmIcon.isNull())
            {
                QRect rect ;
                rect.setRect((PHONE_WIDTH - pmIcon.width()) / 2, PHONE_HEIGHT - pmIcon.height(), pmIcon.width(),
                             pmIcon.height());
                m_pSoftkeyBar->setGeometry(rect);
            }
            // 设置软键栏的背景
            m_pSoftkeyBar->SetBackground(PIC_SOFTKEY_IDLE_BACKGROUND, "");
            // 去除按钮默认的背景
            m_pSoftkeyBar->SetAllBtn3StatePic("", PIC_SOFTKEY_IDLE_SHADOW, "");
        }
        break;
    case STYLE_TYPE_NORMAL:
        {
            // 设置图标显示居中
            m_pSoftkeyBar->SetSoftkeyType(STYLE_TYPE_DEFAULT);
            // 设置按钮默认的背景
            m_pSoftkeyBar->SetAllBtn3StatePic(PIC_THEME1_SOFTKEY, PIC_THEME1_SOFTKEY, PIC_THEME1_SOFTKEY);
            // 设置软键栏的背景
            m_pSoftkeyBar->SetBackground("", "");
        }
        break;
    case STYLE_TYPE_DIVIDE:
    default:
        {
            // 设置图标显示在文字上方
            m_pSoftkeyBar->SetSoftkeyType(STYLE_TYPE_DIVIDE);
            // 设置软键栏的背景
            QPixmap pmIcon = THEME_GET_BMP(PIC_SOFTKEY_DIVIDE_BACKGROUND);
            if (!pmIcon.isNull())
            {
                QRect rect ;
                rect.setRect((PHONE_WIDTH - pmIcon.width()) / 2, PHONE_HEIGHT - pmIcon.height() - 2, pmIcon.width(),
                             pmIcon.height());
                m_pSoftkeyBar->setGeometry(rect);
            }
            m_pSoftkeyBar->SetBackground(PIC_SOFTKEY_DIVIDE_BACKGROUND, PIC_SOFTKEY_DIVIDE_LINE);
            // 设置按钮默认的背景
            m_pSoftkeyBar->SetAllBtn3StatePic("", PIC_SOFTKEY_DIVIDE_CLICKED, "");
        }
        break;
    }
    // 刷新softkey数据
    RefreshSoftKeyBar(objSoftKeyBarData);

    // 更新软键栏按钮状态
    m_pSoftkeyBar->ResetBtnStatus();

    // 显示或隐藏softkey
    if (bShowSoftkey)
    {
        m_pSoftkeyBar->show();
    }
    else
    {
        m_pSoftkeyBar->hide();
    }

    DisplayIMESoftkey(_MainWND.focusWidget());
}

QWidget * CSoftKeyBarDelegate::GetWidget()
{
    return m_pSoftkeyBar.data();
}

void CSoftKeyBarDelegate::SetParent(QWidget * pWgt)
{
    if (!m_pSoftkeyBar.isNull())
    {
        m_pSoftkeyBar->SetParent(pWgt);
        m_pSoftkeyBar->setGeometry(RECT_SOFTKEYBAR);
    }

    setParent(pWgt);
}

bool CSoftKeyBarDelegate::ProcessKeyEvent(QObject * pObject, QKeyEvent * pEvent)
{
    if (!m_pSoftkeyBar.isNull())
    {
        return m_pSoftkeyBar->eventFilter(pObject, pEvent);
    }

    return false;
}

void CSoftKeyBarDelegate::SetVisible(bool bVisible)
{
    if (!m_pSoftkeyBar.isNull())
    {
        bVisible ? m_pSoftkeyBar->show() : m_pSoftkeyBar->hide();
    }
}

void CSoftKeyBarDelegate::Raise()
{
    if (!m_pSoftkeyBar.isNull())
    {
        m_pSoftkeyBar->raise();
    }
}

bool CSoftKeyBarDelegate::Operator(const QString & strAction, const QVariant & var)
{
    MAINWND_INFO("CSoftKeyBarDelegate::Operator[%s]", strAction.toUtf8().data());
    if (SOFTKEY_BAR_DISPLAY_IME == strAction)
    {
        if (!var.canConvert<QWidget *>())
        {
            return false;
        }

        DisplayIMESoftkey(var.value<QWidget *>());
    }
    else if (SOFTKEY_BAR_SWITCH_IME == strAction)
    {
        if (!var.canConvert<QWidget *>())
        {
            return false;
        }

        SwitchNextIMEName(var.value<QWidget *>());
    }
    else if (SOFTKEY_BAR_RESET_DOWN == strAction)
    {
        if (NULL != m_pSoftkeyBar)
        {
            m_pSoftkeyBar->setKeyRelease();
            m_pSoftkeyBar->update();
        }
    }

    return true;
}

void CSoftKeyBarDelegate::InsertEDKSoftkeyData(CArraySoftKeyBarData & arrSoftKeyBarData)
{
    if (!Edk_IsEnable())
    {
        return;
    }

    YLList<SoftKey_Data> listSoftKey;
    Edk_GetEdkSoftKeyList(listSoftKey);

    if (listSoftKey.size() == 0 || arrSoftKeyBarData.size() == 0)
    {
        return;
    }

    yl::string strSlotName = SLOT(OnSoftKeyAction(qSoftButton *));
    QObject  *  pReceiver = this;

    YLList<SoftKey_Data>::ListIterator iter = listSoftKey.begin();
    for (; iter != listSoftKey.end(); ++iter)
    {
        SoftKey_Data & dataSoftKey = *iter;

        CSoftKeyBarData SoftkeyData;
        char EDK_ID[8];
        sprintf(EDK_ID, "%d", dataSoftKey.eType);
        SoftkeyData.m_strSoftkeyAction += SOFTKEY_EDK_ACTION;
        SoftkeyData.m_strSoftkeyAction += EDK_ID;
        SoftkeyData.m_strSoftkeyTitle = toQString(Edk_GetEdkSoftKeyLabel(dataSoftKey.eType));
        SoftkeyData.m_strIconPath = PIC_SOFTKEY_EDK;
        SoftkeyData.m_strSlotName = strSlotName;
        SoftkeyData.m_pReceiver = pReceiver;

        if (dataSoftKey.iPos >= arrSoftKeyBarData.size())
        {
            arrSoftKeyBarData.push_back(SoftkeyData);
        }
        else
        {
            arrSoftKeyBarData.insert(arrSoftKeyBarData.begin() + dataSoftKey.iPos, SoftkeyData);
        }
    }
}

bool CSoftKeyBarDelegate::IsIMESoftkey(const CSoftKeyBarData& SoftKeyBarData)
{
    if (SoftKeyBarData.m_strSoftkeyAction == IME_ACTION
            || SoftKeyBarData.m_strSoftkeyAction == IME_DEL_ACTION)
    {
        return true;
    }

    return false;
}

void CSoftKeyBarDelegate::RefreshSoftKeyBar(const CArraySoftKeyBarData & arrSoftKeyBarData)
{
    if (m_pSoftkeyBar.isNull())
    {
        return;
    }

    // 清空当前软键数据缓存，保证为零能正确绘制软键栏
    m_pSoftkeyBar->Clear();

    // 数据正确性验证
    if (arrSoftKeyBarData.size() == 0)
    {
        return;
    }

    // 设置softkey数据
    int i = 0;
    CArraySoftKeyBarData::const_iterator iter;
    CSoftKeyBarData SoftKeyBarData;
    for (i = 0, iter = arrSoftKeyBarData.begin();
            iter != arrSoftKeyBarData.end();  ++iter)
    {
        SoftKeyBarData = (*iter);
        // 由于原始窗口默认有4个按钮，为了使已有的dialog界面尽量少改动，这边进行按钮过滤
        if (((SoftKeyBarData.m_strSoftkeyTitle != "") || (SoftKeyBarData.m_strSoftkeyAction != ""))
                && (!FullKeyboardEnabled() || !IsIMESoftkey(SoftKeyBarData))
           )
        {
            RefreshSoftKeyBar(i, *iter);
            ++i;
        }
    }
}

void CSoftKeyBarDelegate::RefreshSoftKeyBar(int iIndex, const CSoftKeyBarData & objSoftkeyBarData)
{
    if (m_pSoftkeyBar.isNull() || iIndex < 0)
    {
        return;
    }

    // 不管是否隐藏都要刷新数据,用于清除上次的softkey数据
    // 在softkeybar上添加该Button

    //根据提示模式是否开启显示不同的值
    if (EDK_IsTipModeEnable())
    {
        yl::string strHintTmp = EDKUI_GetHintNameBySoftkeyAction(objSoftkeyBarData.m_strSoftkeyAction);

        //避免m_strSoftkeyTitle后置为空的softkey在提示模式上显示
        if (objSoftkeyBarData.m_strSoftkeyTitle.isEmpty())
        {
            strHintTmp = "";
        }

        m_pSoftkeyBar->SetButton(iIndex, objSoftkeyBarData.m_strSoftkeyAction.c_str(), \
                                 strHintTmp.c_str(), objSoftkeyBarData.m_pReceiver, objSoftkeyBarData.m_strSlotName.c_str());
    }
    else
    {
        m_pSoftkeyBar->SetButton(iIndex, objSoftkeyBarData.m_strSoftkeyAction.c_str(), \
                                 objSoftkeyBarData.m_strSoftkeyTitle, objSoftkeyBarData.m_pReceiver,
                                 objSoftkeyBarData.m_strSlotName.c_str());
    }

    m_pSoftkeyBar->SetIconPath(iIndex, objSoftkeyBarData.m_strIconPath);

    // 判断该softkey是否隐藏
    if (objSoftkeyBarData.m_bDisplay)
    {
        m_pSoftkeyBar->SetButtonShow(iIndex);
    }
    else
    {
        // 隐藏softkey的话不对其进行其他设置
        m_pSoftkeyBar->SetButtonHide(iIndex);
    }
}

void CSoftKeyBarDelegate::DisplayIMESoftkey(QWidget * pWidget)
{
    // 入口参数判断
    if (NULL == pWidget)
    {
        return;
    }

    CBaseDialog * pBaseDlg = UIManager_GetTopWindow(); // 获取当前小窗口指针
    if (NULL == pBaseDlg)
    {
        return;
    }

    // 判断控件是否为当前小窗口或弹出框的控件
    if (!isChildOf(pWidget, pBaseDlg))
    {
        return;
    }

    // 判断是否需要显示IME信息
    bool bDisplayIME = false;
    bool bDisplaySwitch = false;
    if ((pWidget->inherits("QLineEdit")
            || pWidget->inherits("QTextEdit"))
            && pWidget->isEnabled())
    {
        bDisplayIME = true;
    }
    else if (pWidget->inherits("QComboBox")
             && pWidget->isEnabled())
    {
        bDisplaySwitch = true;
    }

    if (FullKeyboardEnabled())
    {
        bDisplayIME = false;
    }

    // 获取窗口的softkey数据
    CArraySoftKeyBarData arrSoftkeyBarData;

//  pBaseDlg->GetSoftkey(arrSoftkeyBarData);    // 获取小窗口的数据
    arrSoftkeyBarData.assign(m_arrSoftKeyBarData.begin(), m_arrSoftKeyBarData.end());

    // http://10.2.1.199/Bug.php?BugID=48082 IME刷新时，添加过滤为空的操作（防止出现空的Softkey）
    int nSoftkeyCount = 0;
    for (int iIndex = 0 ; iIndex < arrSoftkeyBarData.size(); ++iIndex)
    {
        if (FullKeyboardEnabled())
        {
            arrSoftkeyBarData[iIndex].m_bDisplay = false;
        }

        if (arrSoftkeyBarData[iIndex].m_strSoftkeyAction == IME_ACTION)
        {
            if (bDisplayIME)
            {
                arrSoftkeyBarData[iIndex].m_strSoftkeyTitle = qGetCurrentIMETitle(pWidget) ;
                //暂不做显示剩余输入法
                //              arrSoftkeyBarData[iIndex].m_strSoftkeyTitle.append(",");
                //              arrSoftkeyBarData[iIndex].m_strSoftkeyTitle.append(toQString(qGetAllIMEName(pWidget)).toUtf8().data());
            }
            else
            {
                arrSoftkeyBarData[iIndex].m_strSoftkeyTitle = "";
            }

            if (!FullKeyboardEnabled())
            {
                RefreshSoftKeyBar(nSoftkeyCount, arrSoftkeyBarData[iIndex]);// 刷新界面的softkey
            }
        }
        else if (arrSoftkeyBarData[iIndex].m_strSoftkeyAction == IME_DEL_ACTION)
        {
            if (bDisplayIME)
            {
                arrSoftkeyBarData[iIndex].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DELETE);
            }
            else if (bDisplaySwitch)
            {
                arrSoftkeyBarData[iIndex].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SWITCH);
            }
            else
            {
                arrSoftkeyBarData[iIndex].m_strSoftkeyTitle = "";
            }

            if (!FullKeyboardEnabled() || bDisplaySwitch)
            {
                RefreshSoftKeyBar(nSoftkeyCount, arrSoftkeyBarData[iIndex]);// 刷新界面的softkey
            }
        }
        else
        {
            // none
        }
        // 由于原始窗口默认有4个按钮，为了使已有的dialog界面尽量少改动，这边进行按钮过滤
        if ((arrSoftkeyBarData[iIndex].m_strSoftkeyTitle != "")
                || (arrSoftkeyBarData[iIndex].m_strSoftkeyAction != ""))
        {
            ++nSoftkeyCount;
        }
    }
}

void CSoftKeyBarDelegate::SwitchNextIMEName(QWidget * pFocusWidget)
{
    // 入口参数判断
    if (NULL == pFocusWidget
            || (!pFocusWidget->inherits("QLineEdit") && !pFocusWidget->inherits("QTextEdit")))
    {
        return;
    }

    // 获取顶层小窗口指针
    CBaseDialog * pBaseDlg = UIManager_GetTopWindow(); // 获取当前小窗口指针
    if (NULL == pBaseDlg)
    {
        return;
    }

    // 判断控件是否为当前小窗口的控件
    // 判断控件是否为当前小窗口或弹出框的控件
    if (!isChildOf(pFocusWidget, pBaseDlg))
    {
        return;
    }

    //!!!!!
    // 获取窗口的softkey数据
    CArraySoftKeyBarData arrSoftkeyBarData;

//  pBaseDlg->GetSoftkey(arrSoftkeyBarData);    // 获取小窗口的数据
    arrSoftkeyBarData.assign(m_arrSoftKeyBarData.begin(), m_arrSoftKeyBarData.end());

    // http://10.2.1.199/Bug.php?BugID=48082 IME刷新时，添加过滤为空的操作（防止出现空的Softkey）
    int nSoftkeyCount = 0;
    for (int iIndex = 0; iIndex < arrSoftkeyBarData.size(); ++iIndex)
    {
        if (arrSoftkeyBarData[iIndex].m_strSoftkeyAction == IME_ACTION)
        {
            // 修改本地的softkeyData,使其和qSoftKeyBar中的同步
            arrSoftkeyBarData[iIndex].m_strSoftkeyTitle = qShift2NextIME(pFocusWidget);
            //暂不做显示剩余输入法
            //          arrSoftkeyBarData[iIndex].m_strSoftkeyTitle.append(",");
            //          arrSoftkeyBarData[iIndex].m_strSoftkeyTitle.append(toQString(qGetAllIMEName(pFocusWidget)).toUtf8().data());
            RefreshSoftKeyBar(nSoftkeyCount, arrSoftkeyBarData[iIndex]);
        }
        // 由于原始窗口默认有4个按钮，为了使已有的dialog界面尽量少改动，这边进行按钮过滤
        if ((arrSoftkeyBarData[iIndex].m_strSoftkeyTitle != "")
                || (arrSoftkeyBarData[iIndex].m_strSoftkeyAction != ""))
        {
            ++nSoftkeyCount;
        }
    }
}

void CSoftKeyBarDelegate::OnSoftKeyAction(qSoftButton * pBtn)
{
    if (pBtn != NULL && Edk_IsEnable() && pBtn->m_strButtonName.contains(SOFTKEY_EDK_ACTION))
    {
        //取出第四个到最后的字符，并转换位数字
        int nEDKType = (pBtn->m_strButtonName.mid(SOFTKEY_EDK_ACTION_STRING_LENGTH, -1)).toInt();
        Edk_OnEdkSoftKeyPress((EDKSOFTKEYTYPE)nEDKType);
    }
}
