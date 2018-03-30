#include "baseui/editlistitem.h"
#include "baseui/comboboxlistitem.h"
#include "baseui/framelist.h"
#include "directorycommon.h"
#include "translateiddefine.h"
#include "broadsoftdetaillayout.h"
#include "localdetaillayout.h"

#include "dlgdiredit.h"

#include "contactphotoselector.h"
#include "directorymgr.h"
#include "diruihelperfactory.h"
#include "bsftcontactcontroler.h"
#include "localcontactcontroler.h"
#include "bsftcontactuihelper.h"
#include "localcontactuihelper.h"
#include "broadsoftdetaillayout.h"
#include "localdetaillayout.h"
#include "moddirectoryui.h"
#include "baseui/t4xpicpath.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "uicommon.h"
#include "keymap.h"
#include "inputmethod/inputmethod.h"
#include "uimanager/rectdefine.h"
#include "mainwnd/mainwnd.h"
#include "messagebox/modmessagebox.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qcomboboxutility.h"
#include "commonunits/commonunits_def.h"
#include <resource/modresource.h>

#if IF_FEATURE_GBBOOK
#include "genbandpabdetaillayout.h"
#include "genbandcontactuihelper.h"
#include "genbandcontactcontroler.h"
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
#include "mtswcontactuihelper.h"
#include "mtswdetaillayout.h"
#include "mtswcontactcontroler.h"
#endif

namespace
{
#define DLG_DIR_EDIT_ITEM_OFFSET 4
}

CDlgDirEdit::CDlgDirEdit(QWidget * parent/* = NULL*/)
    : CDlgDirDetail(parent)
    , m_pListItemLayout(NULL)
    , m_pComboBoxRing(NULL)
    , m_pComboBoxPhoto(NULL)
    , m_ring(this)
    , m_nFocusIndex(INVALID_ITEM_INDEX)
{
}

CDlgDirEdit::~CDlgDirEdit(void)
{
    ReleaseLayout();
}

void CDlgDirEdit::ReleaseLayout()
{
    if (NULL != m_pFrameList)
    {
        m_pFrameList->ClearContent();
    }

    ResetUIHelperLayout();

    if (NULL != m_pListItemLayout)
    {
        m_pListItemLayout->ReleaseAllItem();

        delete m_pListItemLayout;
        m_pListItemLayout = NULL;
    }

    m_pComboBoxRing = NULL;
    m_pComboBoxPhoto = NULL;
}

void CDlgDirEdit::ResetUIHelperLayout()
{
    CDirUIHelperPtr pUIHelper = g_DirUIHelperFactory.FindDirUIHelper(m_nContactType);

    if (pUIHelper.IsEmpty())
    {
        return;
    }

    if (DIRECTORY_TYPE_LOCAL == m_nContactType)
    {
        CLocalContactUIHelper * pLocalUIHelper = CLocalContactUIHelper::GetUIHelper(pUIHelper);

        if (NULL != pLocalUIHelper)
        {
            pLocalUIHelper->SetDetailLayout(NULL);
        }
    }
    else if (DIRECTORY_TYPE_BROADSOFT == m_nContactType)
    {
        CBSFTContactUIHelper * pBSFTUIHelper = CBSFTContactUIHelper::GetUIHelper(pUIHelper);

        if (NULL != pBSFTUIHelper)
        {
            pBSFTUIHelper->SetDetailLayout(NULL);
        }
    }
#if IF_FEATURE_GBBOOK
    else if (DIRECTORY_TYPE_GENBAND == m_nContactType)
    {
        CGenbandContactUIHelper * pGBUIHelper = CGenbandContactUIHelper::GetUIHelper(pUIHelper);

        if (NULL != pGBUIHelper)
        {
            pGBUIHelper->SetDetailLayout(NULL);
        }
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (DIRECTORY_TYPE_METASWICTH == m_nContactType)
    {
        CMTSWContactUIHelper * pMTSWUIHelper = CMTSWContactUIHelper::GetUIHelper(pUIHelper);

        if (NULL != pMTSWUIHelper)
        {
            pMTSWUIHelper->SetDetailLayout(NULL);
        }
    }
#endif
}

void CDlgDirEdit::InitLocalLayout()
{
    if (NULL != m_pListItemLayout)
    {
        ReleaseLayout();
    }

    CLocalDetailLayoutPtr pLocalLayout = new CLocalDetailLayout(this, false, false);

    if (NULL == pLocalLayout)
    {
        return;
    }

    m_pListItemLayout = pLocalLayout;
    pLocalLayout->Init();

    CComboBoxListItemPtr pRingComboBoxItem = pLocalLayout->GetRingComboBoxItem();

    if (NULL != pRingComboBoxItem)
    {
        m_pComboBoxRing = pRingComboBoxItem->GetComboBox();
    }

    if (NULL != m_pComboBoxRing)
    {
        connect(m_pComboBoxRing, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxRingSelect(int)));
        connect(m_pComboBoxRing, SIGNAL(highlighted(int)), this, SLOT(OnComboBoxRingSelect(int)));
    }

    CComboBoxListItemPtr pPhotoComboBoxItem = pLocalLayout->GetPhotoComboBoxItem();

    if (NULL != pPhotoComboBoxItem)
    {
        m_pComboBoxPhoto = pPhotoComboBoxItem->GetComboBox();
    }

    if (NULL != m_pComboBoxPhoto)
    {
        connect(m_pComboBoxPhoto, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxPhotoSelect(int)));
        connect(m_pComboBoxPhoto, SIGNAL(highlighted(int)), this, SLOT(OnComboBoxPhotoSelect(int)));
    }

    CDirUIHelperPtr pUIHelper = g_DirUIHelperFactory.FindDirUIHelper(m_nContactType);
    CLocalContactUIHelper * pLocalUIHelper = CLocalContactUIHelper::GetUIHelper(pUIHelper);

    if (NULL != pLocalUIHelper)
    {
        pLocalUIHelper->SetDetailLayout(pLocalLayout);
        pLocalUIHelper->SetFocusInvalidFrameList(m_pFrameList);
    }
}

void CDlgDirEdit::InitBroadSoftLayout()
{
    if (NULL != m_pListItemLayout)
    {
        ReleaseLayout();
    }

    CBroadSoftDetailLayoutPtr pBroadSoftLayout = new CBroadSoftDetailLayout(this);

    m_pListItemLayout = pBroadSoftLayout;

    if (NULL == m_pListItemLayout)
    {
        return;
    }

    m_pListItemLayout->Init();

    CDirUIHelperPtr pUIHelper = g_DirUIHelperFactory.FindDirUIHelper(m_nContactType);
    CBSFTContactUIHelper * pBSFTUIHelper = CBSFTContactUIHelper::GetUIHelper(pUIHelper);

    if (NULL != pBSFTUIHelper)
    {
        pBSFTUIHelper->SetDetailLayout(pBroadSoftLayout);
        pBSFTUIHelper->SetFocusInvalidFrameList(m_pFrameList);
    }
}

#if IF_FEATURE_GBBOOK
void CDlgDirEdit::InitGenbandLayout()
{
    if (NULL != m_pListItemLayout)
    {
        ReleaseLayout();
    }

    CGenbandDetailLayoutPtr pGenbandLayout = new CGenbandDetailLayout(this, false, false);

    m_pListItemLayout = pGenbandLayout;

    if (NULL == m_pListItemLayout)
    {
        return;
    }

    m_pListItemLayout->Init();

    CDirUIHelperPtr pUIHelper = g_DirUIHelperFactory.FindDirUIHelper(m_nContactType);
    CGenbandContactUIHelper * pGBUIHelper = CGenbandContactUIHelper::GetUIHelper(pUIHelper);

    if (NULL != pGBUIHelper)
    {
        pGBUIHelper->SetDetailLayout(pGenbandLayout);
        pGBUIHelper->SetFocusInvalidFrameList(m_pFrameList);
    }
}
#endif

void CDlgDirEdit::SetContact(int nContactType)
{
    ReleaseLayout();

    CDlgDirDetail::SetContact(nContactType);

    if (DIRECTORY_TYPE_LOCAL == nContactType)
    {
        InitLocalLayout();
    }
    else if (DIRECTORY_TYPE_BROADSOFT == nContactType)
    {
        InitBroadSoftLayout();
    }
#if IF_FEATURE_GBBOOK
    else if (DIRECTORY_TYPE_GENBAND == nContactType)
    {
        InitGenbandLayout();
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (DIRECTORY_TYPE_METASWICTH == nContactType)
    {
        InitMetaSwitchLayout(false);
    }
#endif

    NotifyCtrlFocus(false);
}

void CDlgDirEdit::SetPhotoPath(yl::string & strFilePath)
{
    if (NULL == m_pComboBoxPhoto)
    {
        return;
    }

    yl::string strName;

    if (!Res_GetNameByPath(DIR_RES_TYPE_PHOTO, strFilePath, strName))
    {
        res_toHidePath(strFilePath, PHFILE_TYPE_CONTACT_IMAGE);

        if (CONTACT_PHOTO_DEFAULT_CONTACT_IMAGE != strFilePath)
        {
            return;
        }

        strName = CONTACT_PHOTO_FILE_DEFAULT;
    }

    QString strUtf8Name = LANG_TRANSLATE(DM_GetContactImageName(strName).c_str());

    qComboBoxUtility::SetComboBoxSelectText(m_pComboBoxPhoto, strUtf8Name);
}

CEditListItem * CDlgDirEdit::GetNameEditItem()
{
    if (NULL == m_pListItemLayout)
    {
        return NULL;
    }

    if (DIRECTORY_TYPE_LOCAL == m_nContactType)
    {
        CLocalDetailLayoutPtr pLocalLayout = (CLocalDetailLayoutPtr)m_pListItemLayout;

        return pLocalLayout->GetNameEditItem();
    }
    else if (DIRECTORY_TYPE_BROADSOFT == m_nContactType)
    {
        CBroadSoftDetailLayoutPtr pBroadSoftLayout = (CBroadSoftDetailLayoutPtr)m_pListItemLayout;

        return pBroadSoftLayout->GetNameEditItem();
    }
#if IF_FEATURE_GBBOOK
    else if (DIRECTORY_TYPE_GENBAND == m_nContactType)
    {
        CGenbandDetailLayoutPtr pGenbandLayout = (CGenbandDetailLayoutPtr)m_pListItemLayout;

        return pGenbandLayout->GetNameEditItem();
    }
#endif

    return NULL;
}

bool CDlgDirEdit::GetRingPath(yl::string & strRingPath, int nIndex)
{
    QWidget * pFocusWidget = focusWidget();

    if (NULL == pFocusWidget || pFocusWidget != m_pComboBoxRing)
    {
        return false;
    }

    // 如果不在当前界面，不播放铃声
    if (!this->isVisible())
    {
        return false;
    }

    //如果在第一项，也就是Auto项，不播放铃声
    if (nIndex <= 0)
    {
        return false;
    }

    strRingPath = m_pComboBoxRing->itemData(nIndex).value<yl::string>();

    return true;
}

void CDlgDirEdit::Init()
{
    CDlgDirDetail::Init();

    m_ring.StopRingPlay();
    ReleaseLayout();
}

void CDlgDirEdit::Uninit()
{
    DIRUI_INFO("CDlgDirEdit::Uninit()");

    ReleaseLayout();

    m_ring.StopRingPlay();

    CDlgDirDetail::Uninit();
}

void CDlgDirEdit::OnKeyboardShow()
{
    SetKeyboardEnterType();
    AdjustFrameList(false);
}

void CDlgDirEdit::OnKeyboardHide()
{
    AdjustFrameList(true);
}

void CDlgDirEdit::AdjustFrameList(bool bReset)
{
    if (!FullKeyboardEnabled())
    {
        return;
    }
    if (NULL == m_pFrameList)
    {
        return;
    }

    if (bReset)
    {
        m_nLineOffset = 0;
        m_pFrameList->setGeometry(RECT_DIR_CONTACT_DETAIL);
        return;
    }
    if (DirUI_AdjustFrameList(m_pFrameList
                              , bReset
                              , RECT_DIR_CONTACT_DETAIL
                              , DLG_DIR_EDIT_ITEM_OFFSET
                              , RECT_DIRECTORY.y()))
    {
        m_nLineOffset = m_pFrameList->geometry().y() - RECT_DIR_CONTACT_DETAIL.y();
    }
}

void CDlgDirEdit::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox || !isVisible())
    {
        return;
    }

    MESSAGEBOX_TYPE eMsgBoxType = pMessageBox->GetType();
    MESSAGEBOX_RESULT eResult = pMessageBox->GetResult();
    int nAction = pMessageBox->GetID();

    switch (eMsgBoxType)
    {
    case MESSAGEBOX_SPECIAL:
        {
            if (MessageBox_Discard == eResult)
            {
                if (DIR_UI_ACTION_COMMON_BACK == nAction)
                {
                    UIManager_PutPrivateDlg(this);
                }
                else if (DIR_UI_ACTION_COMMON_HOME == nAction)
                {
                    CBaseDialog::OnHomeButtonClick();
                }
                else if (DIR_UI_ACTION_PRE_DETAIL == nAction)
                {
                    CDlgDirDetail::OnBtnPreDetail();
                }
                else if (DIR_UI_ACTION_NEXT_DETAIL == nAction)
                {
                    CDlgDirDetail::OnBtnNextDetail();
                }
            }
            else if (MessageBox_OK == eResult)
            {
                OnBtnSaveClick();
            }
        }
        break;
    case MESSAGEBOX_IMAGE_SELECT:
        {
            if (MessageBox_OK != eResult)
            {
                break;
            }

            OnChooseImage();
        }
        break;
    default:
        break;
    }
}

bool CDlgDirEdit::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (NULL == pEvent)
    {
        return false;
    }

    if (MessageBox_GetMessageBoxCount() > 0)
    {
        return false;
    }

    //处理铃声事件
    if (m_ring.IsRinging())
    {
        QWidget * pFocusWidget = focusWidget();

        if (pFocusWidget != m_pComboBoxRing)
        {
            m_ring.StopRingPlay();
        }
    }

    switch (pEvent->type())
    {
    case QEvent::FocusIn://焦点集中事件
        {
            if (OnFocusPressInEventFilter(pObj, pEvent))
            {
                return true;
            }
        }
        break;
    case QEvent::FocusOut:
        {
            OnCtrlFocusChanged();
        }
        break;
    case QEvent::KeyPress://键盘输入事件
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            int nKey = pKeyEvent->key();

            switch (nKey)
            {
            case PHONE_KEY_OK:
                {
                    if (OnEnterPressEventFilter(pObj, pEvent))
                    {
                        return true;
                    }
                }
                break;
            case PHONE_KEY_CANCEL:
                {
                    QWidget * pFocusWidget = focusWidget();

                    if (NULL != pFocusWidget && pFocusWidget->inherits("QComboBox")
                            && pFocusWidget != pObj)
                    {
                        QComboBox * pComboBox = (QComboBox *)pFocusWidget;

                        pComboBox->hidePopup();
                        return true;
                    }

                    if (NULL != pFocusWidget && pFocusWidget->inherits("QLineEdit"))
                    {
                        qSetIsShowWnd(pFocusWidget, false);
                    }

                    if (ProcessInputs(DIR_UI_ACTION_COMMON_BACK))
                    {
                        return true;
                    }
                }
                break;
            case PHONE_KEY_LEFT:
                {
                    if (OnLeftPressEventFilter(pObj, pEvent))
                    {
                        return true;
                    }
                }
                break;
            case PHONE_KEY_RIGHT:
                {
                    if (OnRightPressEventFilter(pObj, pEvent))
                    {
                        return true;
                    }
                }
                break;
            case PHONE_KEY_UP:
                {
                    bool bFilter = false;

                    if (OnUpPressEventFilter(pObj, pEvent, bFilter))
                    {
                        return bFilter;
                    }
                }
                break;
            case PHONE_KEY_DOWN:
                {
                    bool bFilter = false;

                    if (OnDownPressEventFilter(pObj, pEvent, bFilter))
                    {
                        return bFilter;
                    }
                }
                break;
            case PHONE_KEY_STAR:
            case PHONE_KEY_POUND:
                {
                    // 编辑状态下不能进入拔号
                    QWidget * pFocusWidget = focusWidget();

                    if (NULL != pFocusWidget && (pFocusWidget->inherits("QLineEdit")
                                                 || pFocusWidget->inherits("QComboBox")))
                    {
                        return false;
                    }
                }
                break;
            case PHONE_KEY_VOLUME_INCREASE:
            case PHONE_KEY_VOLUME_DECREASE:
                {
                    if (m_ring.IsRinging())
                    {
                        m_ring.AdjustVolume(PHONE_KEY_VOLUME_INCREASE == nKey);
                    }

                    return false;
                }
                break;
            default:
                break;
            }
        }
        break;
    case QEvent::MouseButtonPress:
        {
            if (OnMousePressEventFilter(pObj, pEvent))
            {
                return true;
            }
        }
        break;
    case QEvent::MouseButtonRelease:
        {
            if (pObj->inherits("QLineEdit"))
            {
                AdjustFrameList(false);
            }
        }
        break;
    default:
        break;
    }

    return CDlgDirDetail::eventFilter(pObj, pEvent);
}

bool CDlgDirEdit::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax, QRect & rtVolume,
                                  yl::string & strBGPath) const
{
    if (NULL != m_pComboBoxRing && focusWidget() == m_pComboBoxRing)
    {
        int nIndex = m_pComboBoxRing->currentIndex();

        //如果在第一项，也就是Auto项，不播放铃声
        if (0 == nIndex)
        {
            return false;
        }

        iMin = 0;
        iMax = 15;
        return true;
    }

    return false;
}

bool CDlgDirEdit::OnFocusPressInEventFilter(QObject * pObj, QEvent * pEvent)
{
    QWidget * pFocusWidget = focusWidget();

    if (pFocusWidget == m_pComboBoxRing)
    {
        OnPlayRingInFocus();
    }
    else if (pFocusWidget == m_pComboBoxPhoto)
    {
        OnComboBoxPhotoInFocus();
    }

    if (FullKeyboardEnabled() && NULL != m_pListItemLayout)
    {
        int nFocusIndex = m_nFocusIndex;
        m_nFocusIndex = m_pListItemLayout->GetFocusIndex();

        if (nFocusIndex != m_nFocusIndex)
        {
            CDetailListItemBase* pFocusItem = m_pListItemLayout->GetDetailListItem(m_nFocusIndex);
            CDetailListItemBase* pLastFocusItem = m_pListItemLayout->GetDetailListItem(nFocusIndex);
            CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(pFocusItem);
            CEditListItemPtr pLastEditItem = CEditListItem::GetEditListItem(pLastFocusItem);

            if (NULL == pEditItem && NULL != pLastEditItem)
            {
                qSetIsShowWnd(pLastEditItem->GetLineEdit(), false);
            }
        }
    }

    OnCtrlFocusChanged();

    return false;
}

bool CDlgDirEdit::OnEnterPressEventFilter(QObject * pObj, QEvent * pEvent)
{
    bool bSave = false;
    QWidget* pFocusWidget = focusWidget();

    if (NULL != pFocusWidget)
    {
        if (pFocusWidget->inherits("QComboBox"))
        {
            if (pFocusWidget == pObj)
            {
                bSave = true;
            }
            else
            {
                return true;
            }
        }
        else if (pFocusWidget->inherits("QLineEdit"))
        {
            bSave = true;
        }
    }

    if (bSave)
    {
        OnBtnSaveClick();
        return true;
    }

    return false;
}

bool CDlgDirEdit::OnMousePressEventFilter(QObject * pObj, QEvent * pEvent)
{
    QWidget* pFocusWidget = focusWidget();

    SetKeyboardEnterType();

    if (NULL == pFocusWidget || NULL == pObj || pFocusWidget == pObj)
    {
        return false;
    }

    if (!pObj->inherits("qButtonItem"))
    {
        bool bComboBoxFocus = pFocusWidget->inherits("QComboBox");
        bool bEditFocus = pFocusWidget->inherits("QLineEdit");

        if (bComboBoxFocus)
        {
            QComboBox* pComboBox = (QComboBox*)pFocusWidget;
            QAbstractItemView* pView = pComboBox->view();

            if (NULL != pView)
            {
                QScrollBar* pScrollBar = pView->verticalScrollBar();

                if (pView == pObj || pScrollBar == pObj || pObj->parent() == pView)
                {
                    // 下拉框弹出时点中下拉框内的范围
                    bComboBoxFocus = false;
                }
                else if (pView->isVisible())
                {
                    // 点击其他地方
                    return true;
                }
            }
        }

        if ((bComboBoxFocus || bEditFocus)
                && (!pObj->inherits("QComboBox") && !pObj->inherits("QLineEdit"))
                && (!FullKeyboardEnabled()
                    || !(pObj->inherits("CBaseInputDlg")
                         || pObj->inherits("CInputBoxButton")
                        )
                   )
           )
        {
            pFocusWidget->clearFocus();
            m_nFocusIndex = INVALID_ITEM_INDEX;

            if (bComboBoxFocus)
            {
                OnCtrlFocusChanged();
            }
        }
    }

    return false;
}

bool CDlgDirEdit::OnLeftPressEventFilter(QObject * pObj, QEvent * pEvent)
{
    QWidget * pFocusWidget = focusWidget();

    if (qComboBoxUtility::OnLeftPressEventFilter(pObj, pEvent, pFocusWidget))
    {
        return true;
    }

    if (NULL != pFocusWidget && pFocusWidget->inherits("QLineEdit"))
    {
        return false;
    }

    return CDlgDirDetail::OnLeftPressEventFilter(pObj, pEvent);
}

bool CDlgDirEdit::OnRightPressEventFilter(QObject * pObj, QEvent * pEvent)
{
    QWidget * pFocusWidget = focusWidget();

    if (qComboBoxUtility::OnRightPressEventFilter(pObj, pEvent, pFocusWidget))
    {
        return true;
    }

    if (NULL != pFocusWidget && pFocusWidget->inherits("QLineEdit"))
    {
        return false;
    }

    return CDlgDirDetail::OnRightPressEventFilter(pObj, pEvent);
}

bool CDlgDirEdit::OnDownPressEventFilter(QObject * pObj, QEvent * pEvent, bool & bFilter)
{
    QWidget * pFocusWidget = focusWidget();

    if (NULL != m_pListItemLayout && NULL != pFocusWidget)
    {
        bool bSwitchFocus = false;

        if (pFocusWidget->inherits("QLineEdit"))
        {
            bSwitchFocus = true;
        }
        else if (pFocusWidget->inherits("QComboBox"))
        {
            if (pFocusWidget != pObj)
            {
                bFilter = false;
                return true;
            }

            bSwitchFocus = true;
        }

        if (bSwitchFocus)
        {
            if (m_pListItemLayout->IsLastItemFocus())
            {
                if (NULL != m_pFrameList)
                {
                    m_pFrameList->OnNotifyScrollNext();
                }

                m_pListItemLayout->SetFirstItemFocus();
            }
            else
            {
                m_pListItemLayout->SwitchFocusToNext();
            }

            bFilter = true;

            SetKeyboardEnterType();

            AdjustFrameList(false);

            return true;
        }
    }

    bFilter = false;
    return false;
}

bool CDlgDirEdit::OnUpPressEventFilter(QObject * pObj, QEvent * pEvent, bool & bFilter)
{
    QWidget * pFocusWidget = focusWidget();

    if (NULL != m_pListItemLayout && NULL != pFocusWidget)
    {
        bool bSwitchFocus = false;

        if (pFocusWidget->inherits("QLineEdit"))
        {
            bSwitchFocus = true;
        }
        else if (pFocusWidget->inherits("QComboBox"))
        {
            if (pFocusWidget != pObj)
            {
                bFilter = false;
                return true;
            }

            bSwitchFocus = true;
        }

        if (bSwitchFocus)
        {
            if (m_pListItemLayout->IsFirstItemFocus())
            {
                if (NULL != m_pFrameList)
                {
                    m_pFrameList->OnNotifyScrollPrevious();
                }

                m_pListItemLayout->SetLastItemFocus();
            }
            else
            {
                m_pListItemLayout->SwitchFocusToPre();
            }

            bFilter = true;

            SetKeyboardEnterType();

            AdjustFrameList(false);

            return true;
        }
    }

    bFilter = false;
    return false;
}

void CDlgDirEdit::OnBtnPreDetail()
{
    if (!ProcessInputs(DIR_UI_ACTION_PRE_DETAIL))
    {
        CDlgDirDetail::OnBtnPreDetail();
    }
}

void CDlgDirEdit::OnBtnNextDetail()
{
    if (!ProcessInputs(DIR_UI_ACTION_NEXT_DETAIL))
    {
        CDlgDirDetail::OnBtnNextDetail();
    }
}

void CDlgDirEdit::OnBtnImage()
{
    if (NULL == m_pListItemLayout || DETAIL_LIST_ITEM_LAYOUT_LOCAL != m_pListItemLayout->GetType()
            || DIRECTORY_TYPE_LOCAL != m_nContactType)
    {
        return;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

    if (NULL == pLocalContact || kszBlackListGroupId == pLocalContact->GetGroupId())
    {
        return;
    }

    CContactPhotoSelector * pPhotoSelector = g_DirectoryMgr.GetPhotoSelector();

    if (NULL == pPhotoSelector)
    {
        return;
    }

    pPhotoSelector->Init(m_strImage);

    MessageBox_ShowImageSelectBox(this, pPhotoSelector->GetRowCount(), pPhotoSelector);
}

void CDlgDirEdit::OnChooseImage()
{
    CContactPhotoSelector * pPhotoSelector = g_DirectoryMgr.GetPhotoSelector();

    if (NULL == pPhotoSelector)
    {
        return;
    }

    yl::string strPhotoPath = pPhotoSelector->GetPhoto();
    bool bChange = false;

    if (m_strImage != strPhotoPath)
    {
        bChange = true;
    }

    SetPhotoPath(strPhotoPath);

    if (!bChange)
    {
        return;
    }

    if (NULL != m_pComboBoxPhoto && m_pComboBoxPhoto->isVisible())
    {
        m_pComboBoxPhoto->setFocus();
    }
    else
    {
        CLocalDetailLayoutPtr pLocalLayout = (CLocalDetailLayoutPtr)m_pListItemLayout;
        CEditListItemPtr pNameItem = NULL;

        if (NULL != pLocalLayout)
        {
            pNameItem = pLocalLayout->GetNameEditItem();
        }

        if (NULL != pNameItem)
        {
            pNameItem->SetEditFocus();
        }
    }
}

bool CDlgDirEdit::OnAction(const QString & strAction)
{
    bool bProcess = true;

    if (strAction == ACTION_BACK)
    {
        OnBtnBackClick();
    }
    else if (strAction == IME_ACTION)
    {
        OnBtnIMEClick();
    }
    else if (strAction == IME_DEL_ACTION)
    {
        OnBtnDeleteClick();
    }
    else if (strAction == TRID_SAVE)
    {
        OnBtnSaveClick();
    }
    else
    {
        bProcess = false;
    }

    return bProcess;
}

void CDlgDirEdit::OnCtrlFocusChanged()
{
    QWidget * pFocusWidget = focusWidget();

    if (pFocusWidget != m_pComboBoxRing && (m_ring.IsRinging() || m_ring.IsBeginPlay()))
    {
        m_ring.StopRingPlay();
    }

    InitSoftKeyData();

    if (NULL == pFocusWidget)
    {
        _MainWND.DrawSoftkey(this);
        NotifyCtrlFocus(false);
        return;
    }

    bool bFocusNotify = true;

    if (pFocusWidget->inherits("QLineEdit"))
    {
        QLineEdit * pLineEdit = (QLineEdit *)pFocusWidget;

        if (pLineEdit->isReadOnly())
        {
            _MainWND.DrawSoftkey(this);
            bFocusNotify = false;
        }
        else
        {
            m_SoftkeyData[0].m_strSoftkeyAction = IME_ACTION;

            m_SoftkeyData[1].m_strSoftkeyAction = IME_DEL_ACTION;
            m_SoftkeyData[1].m_strIconPath = PIC_SOFTKEY_DELETE;

            m_SoftkeyData[2].m_strSoftkeyAction = TRID_SAVE;
            m_SoftkeyData[2].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SAVE);
            m_SoftkeyData[2].m_strIconPath = PIC_SOFTKEY_SAVE;

            _MainWND.DrawSoftkey(this);
            _MainWND.DisplayIMESoftkey(pFocusWidget);
        }
    }
    else if (pFocusWidget->inherits("QComboBox"))
    {
        bool bSave = true;

        if (DIRECTORY_TYPE_BROADSOFT == m_nContactType)
        {
            CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
            CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

            if (NULL != pBSFTContact)
            {
                bSave = pBSFTContact->IsCurrentPersonal();
            }
            else
            {
                bSave = false;
            }
        }

        if (bSave)
        {
            m_SoftkeyData[0].m_strSoftkeyAction = TRID_SAVE;
            m_SoftkeyData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SAVE);
            m_SoftkeyData[0].m_strIconPath = PIC_SOFTKEY_SAVE;
        }
        else
        {
            bFocusNotify = false;
        }

        _MainWND.DrawSoftkey(this);
    }
    else
    {
        _MainWND.DrawSoftkey(this);
        bFocusNotify = false;
    }

    NotifyCtrlFocus(bFocusNotify);
}

void CDlgDirEdit::NotifyCtrlFocus(bool bFocus)
{
    CDirUIHelperPtr pUIHelper = g_DirectoryMgr.GetActiveUIHelper();

    if (DIRECTORY_TYPE_LOCAL == m_nContactType)
    {
        CLocalContactUIHelper * pLocalUIHelper = CLocalContactUIHelper::GetUIHelper(pUIHelper);

        if (NULL != pLocalUIHelper)
        {
            pLocalUIHelper->OnDetailCtrlFocus(bFocus);
        }
    }
    else if (DIRECTORY_TYPE_BROADSOFT == m_nContactType)
    {
        CBSFTContactUIHelper * pBSFTUIHelper = CBSFTContactUIHelper::GetUIHelper(pUIHelper);

        if (NULL != pBSFTUIHelper)
        {
            pBSFTUIHelper->OnDetailCtrlFocus(bFocus);
        }
    }
#if IF_FEATURE_GBBOOK
    else if (DIRECTORY_TYPE_GENBAND == m_nContactType)
    {
        CGenbandContactUIHelper * pGenbandUIHelper = CGenbandContactUIHelper::GetUIHelper(pUIHelper);

        if (NULL != pGenbandUIHelper)
        {
            pGenbandUIHelper->OnDetailCtrlFocus(bFocus);
        }
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (DIRECTORY_TYPE_METASWICTH == m_nContactType)
    {
        CMTSWContactUIHelper * pMTSWUIHelper = CMTSWContactUIHelper::GetUIHelper(pUIHelper);

        if (NULL != pMTSWUIHelper)
        {
            pMTSWUIHelper->OnDetailCtrlFocus(bFocus);
        }
    }
#endif
}

void CDlgDirEdit::OnPlayRingInFocus()
{
    if (NULL == m_pComboBoxRing)
    {
        m_ring.StopRingPlay();
        return;
    }

    int nIndex = m_pComboBoxRing->currentIndex();

    yl::string strRingPath;

    if (!GetRingPath(strRingPath, nIndex))
    {
        m_ring.StopRingPlay();
        return;
    }

    m_ring.StartRingPlay(strRingPath, TIME_DELAY_FOCUS_RING);
}

void CDlgDirEdit::OnComboBoxPhotoInFocus()
{
    if (NULL == m_pComboBoxPhoto)
    {
        return;
    }

    OnComboBoxPhotoSelect(m_pComboBoxPhoto->currentIndex());
}

void CDlgDirEdit::OnComboBoxRingSelect(int nIndex)
{
    yl::string strRingPath;

    if (!GetRingPath(strRingPath, nIndex))
    {
        m_ring.StopRingPlay();
        return;
    }

    m_ring.StartRingPlay(strRingPath, TIME_DELAY_PLAY_RING);
}

void CDlgDirEdit::OnComboBoxPhotoSelect(int nIndex)
{
    if (NULL == m_pComboBoxPhoto)
    {
        return;
    }

    yl::string strFilePath = m_pComboBoxPhoto->itemData(nIndex).value<yl::string>();

    if (strFilePath == m_strImage)
    {
        return;
    }

    m_strImage = strFilePath;

    LoadContactImage();
    update(m_btnImage.GetRect());
}

bool CDlgDirEdit::OnBtnSaveClick()
{
    m_ring.StopRingPlay();

    if (NULL == m_pListItemLayout)
    {
        return false;
    }

    m_pListItemLayout->ReturnAllEdit();

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

    if (DIRECTORY_TYPE_LOCAL == m_nContactType)
    {
        CLocalContactControler * pLocalControler = CLocalContactControler::GetControler(pControler);

        if (NULL != pLocalControler)
        {
            return pLocalControler->OnAction(DIR_ACTION_LOCAL_EDIT_CONTACT, -1, m_nContactId);
        }
    }
    else if (DIRECTORY_TYPE_BROADSOFT == m_nContactType)
    {
        CBSFTContactControler * pBSFTControler = CBSFTContactControler::GetControler(pControler);

        if (NULL != pBSFTControler)
        {
            return pBSFTControler->OnAction(DIR_ACTION_BSFT_EDIT_PERSONAL, -1, m_nContactId);
        }
    }
#if IF_FEATURE_GBBOOK
    else if (DIRECTORY_TYPE_GENBAND == m_nContactType)
    {
        CGenbandContactControler * pGBControler = CGenbandContactControler::GetControler(pControler);

        if (NULL != pGBControler)
        {
            return pGBControler->OnAction(DIR_ACTION_GENBAND_EDIT_PAB, -1, m_nContactId);
        }
    }
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (DIRECTORY_TYPE_METASWICTH == m_nContactType)
    {
        CMTSWContactControler * pMTSWControler = CMTSWContactControler::GetControler(pControler);

        if (NULL != pMTSWControler)
        {
            return pMTSWControler->OnAction(DIR_ACTION_MTSW_EDIT_CONTACTLIST, -1, m_nContactId);
        }
    }
#endif

    SetKeyboardEnterType();
    return false;
}

void CDlgDirEdit::OnBtnIMEClick()
{
    QWidget * pFocusWidget = focusWidget();

    if (NULL == pFocusWidget)
    {
        return;
    }

    _MainWND.SwitchNextIMEName(pFocusWidget);

    CEditListItemPtr pNameEditItem = GetNameEditItem();

    // 只有Name的编辑框才开放配置默认首先输入法(Local/BroadSoft)
    if (NULL != pNameEditItem && pFocusWidget == (QWidget *)pNameEditItem->GetLineEdit())
    {
        yl::string strIMEName = fromQString(qGetCurrentIMEName(pFocusWidget));

        if (!strIMEName.empty())
        {
            configParser_SetConfigString(kszContactDefaultIME, strIMEName.c_str(), CONFIG_LEVEL_PHONE);
        }
    }
}

void CDlgDirEdit::OnBtnDeleteClick()
{
    QWidget * pFocusWidget = focusWidget();

    if (NULL == pFocusWidget)
    {
        return;
    }

    if (pFocusWidget->inherits("QLineEdit") || pFocusWidget->inherits("QTextEdit"))
    {
        //点击删除时，调用此函数，对当前焦点的文本输入框的内容进行退格删除
        qOnBackspaceClicked(this);
    }
}

void CDlgDirEdit::OnBtnBackClick()
{
    if (!ProcessInputs(DIR_UI_ACTION_COMMON_BACK))
    {
        UIManager_PutPrivateDlg(this);
    }
}

void CDlgDirEdit::OnHomeButtonClick()
{
    if (!ProcessInputs(DIR_UI_ACTION_COMMON_HOME))
    {
        CBaseDialog::OnHomeButtonClick();
    }
}

bool CDlgDirEdit::ProcessInputs(int nUIAction)
{
    if (NULL == m_pListItemLayout)
    {
        return false;
    }

    m_pListItemLayout->ReturnAllEdit();

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    bool bChange = false;

    if (DIRECTORY_TYPE_LOCAL == m_nContactType)
    {
        CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

        if (NULL != pLocalContact)
        {
            bChange = pLocalContact->IsContactChanged();
        }
    }
    else if (DIRECTORY_TYPE_BROADSOFT == m_nContactType)
    {
        CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

        if (NULL != pBSFTContact)
        {
            bChange = pBSFTContact->IsContactChanged();
        }
    }
#if IF_FEATURE_GBBOOK
    else if (DIRECTORY_TYPE_GENBAND == m_nContactType)
    {
        CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);

        if (NULL != pGenbandContact)
        {
            bChange = pGenbandContact->IsContactChanged();
        }
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (DIRECTORY_TYPE_METASWICTH == m_nContactType)
    {
        CMTSWContactControler * pMTSWControler = CMTSWContactControler::GetControler(pControler);

        if (NULL != pMTSWControler)
        {
            bChange = pMTSWControler->IsContactChanged();
        }
    }
#endif

    if (!bChange)
    {
        return false;
    }

    MsgBoxSoftkey skCancel(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel);
    MsgBoxSoftkey skDiscard(LANG_TRANSLATE(TRID_DISCARD), MessageBox_Discard);
    MsgBoxSoftkey skSave(LANG_TRANSLATE(TRID_SAVE), MessageBox_OK);
    VecMsgBoxSoftkey vecSoftKey;

    vecSoftKey.push_back(skCancel);
    vecSoftKey.push_back(skDiscard);
    vecSoftKey.push_back(skSave);

    QString strNote = LANG_TRANSLATE(TRID_DO_YOU_WANT_TO_SAVE_YOUR_CHANGES_OR_DISCARD_THEM);

    MessageBox_ShowCommonMsgBox(this, strNote, vecSoftKey, -1, nUIAction);

    return true;
}

#if IF_FEATURE_METASWITCH_DIRECTORY
void CDlgDirEdit::InitMetaSwitchLayout(bool isCopy)
{
    if (NULL != m_pListItemLayout)
    {
        ReleaseLayout();
    }

    CMTSWDetailLayoutPtr pMTSWLayout = new CMTSWDetailLayout(this, true, isCopy);

    if (NULL == pMTSWLayout)
    {
        return;
    }

    m_pListItemLayout = pMTSWLayout;
    pMTSWLayout->Init();

    CDirUIHelperPtr pUIHelper = g_DirUIHelperFactory.FindDirUIHelper(m_nContactType);
    CMTSWContactUIHelper * pMTSWUIHelper = CMTSWContactUIHelper::GetUIHelper(pUIHelper);

    if (NULL != pMTSWUIHelper)
    {
        pMTSWUIHelper->SetDetailLayout(pMTSWLayout);
        pMTSWUIHelper->SetFocusInvalidFrameList(m_pFrameList);
    }
}
#endif

void CDlgDirEdit::SetKeyboardEnterType()
{
    if (NULL != m_pFrameList)
    {
        CDetailListItemBase *item = m_pListItemLayout->GetDetailListItem(m_pListItemLayout->GetFocusIndex());
        int nCurIndex = m_pFrameList->GetIndexByItem(item);
        if (nCurIndex != INVALID_ITEM_INDEX)
        {
            if (nCurIndex == m_pFrameList->GetTotalItemCount() - 1)
            {
                qSetEnterType(ENTER_TYPE_DONE);
            }
            else
            {
                qSetEnterType(m_pListItemLayout->IsLastEditItem() ? ENTER_TYPE_DONE : ENTER_TYPE_NEXT);
            }
        }

    }
}
