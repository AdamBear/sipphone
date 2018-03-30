#include "dlgdisplaywallpaper.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include <list>
#include "qtcommon/qmisc.h"
#include "messagebox/modmessagebox.h"
#include "baseui/fontcolor.h"
#include "commonunits/modcommonunits.h"
#include "uikernel/qcomboboxutility.h"
#include "resource/modresource.h"
#include "keymap.h"
#include "uikernel/languagehelper.h"

namespace
{
static const int MSG_ID_DELETE_WAPPER(100);
static const int MSG_ID_AFTER_DELETE_NOTE(101);
}

Q_DECLARE_METATYPE(WallPaperData);

CDlgDisplayWallpaper::CDlgDisplayWallpaper(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCmbBoxWallpaper(NULL)
    , m_pLabPreview(NULL)
    , m_pArrowLeft(NULL)
    , m_pArrowRight(NULL)
{
#if !IF_FEATURE_T48_DSSKEY_WALLPAPER
    //初始化数据
    InitData();
#endif
}

CDlgDisplayWallpaper::~CDlgDisplayWallpaper()
{
}

//加载数据
bool CDlgDisplayWallpaper::LoadPageData()
{
    WallPaperListData ListData;
    ListData.m_listPaperData.clear();

    bool bIsSuccess = g_pDisplayController->GetWallpaperListData(&ListData);

    SETTINGUI_INFO("CDlgDisplayWallpaper::LoadPageData list size [%d]",
                   ListData.m_listPaperData.size());

    if (!bIsSuccess)
    {
        return false;
    }
    else
    {
        SetCurrentPageData(ListData);
        return true;
    }
}

//获取数据状态是否改变
bool CDlgDisplayWallpaper::IsStatusChanged()
{
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
    if (m_eType == WALLPAPAER_DSSKEY)
    {
        return GetCurrentPicture() != g_pDisplayController->GetDsskeyWallpaper();
    }
    else
#endif
    {
        return GetCurrentPicture() != g_pDisplayController->GetCurrentWallpaper();
    }
}

//保存数据
bool CDlgDisplayWallpaper::SavePageData()
{
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
    if (m_eType == WALLPAPAER_DSSKEY)
    {
        g_pDisplayController->SetDsskeyWallpaper(GetCurrentPicture());
    }
    else
#endif
    {
        g_pDisplayController->SetCurrentWallpaper(GetCurrentPicture());
    }

    return true;
}

//插槽：行焦点行变化操作
void CDlgDisplayWallpaper::OnCmbCurrentIndexChanged(int nIndex)
{
    //指针判空
    if (NULL == m_pCmbBoxWallpaper
            || NULL == m_pLabPreview)
    {
        return;
    }

    //t46内存不够 全部缓存会占用比较大内存  使用临时方案  查看的时候才把图片加载到内存中
    QPixmap pixCurrent;
    yl::string strPic = GetCurrentPicture().c_str();
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
    if (m_eType == WALLPAPAER_DSSKEY)
    {
        if (strcasecmp(strPic.c_str(), "auto") == 0)
        {
            strPic = g_pDisplayController->GetCurrentWallpaper();
        }
    }
#endif

    pixCurrent.load(strPic.c_str());
    m_pLabPreview->setPixmap(pixCurrent);

    emit RefreshSoftkey();
}

yl::string CDlgDisplayWallpaper::GetCurrentPicture()
{
    return GetCurrentData().m_strPaperPath;
}

WallPaperData CDlgDisplayWallpaper::GetCurrentData()
{
    if (NULL == m_pCmbBoxWallpaper)
    {
        return WallPaperData();
    }

    return m_pCmbBoxWallpaper->itemData(
               m_pCmbBoxWallpaper->currentIndex()).value<WallPaperData>();
}

//初始化数据，设置控件的属性、样式、数据等都在其中进行
void CDlgDisplayWallpaper::InitData()
{
    //设定滑块大小位置
    m_pCmbBoxWallpaper = new QComboBox(this);
    m_pLabPreview = new QLabel(this);

    //指针判空
    if (NULL == m_pCmbBoxWallpaper
            || m_pLabPreview == NULL)
    {
        //内存已满立即返回
        return;
    }

    m_pLabPreview->setObjectName("LabPreview");

    //向左箭头
    int iStartPointX = 5;
    m_pArrowLeft = new qUIPushButton(this);
    if (m_pArrowLeft == NULL)
    {
        return;
    }
    m_pArrowLeft->setObjectName("ArrowLeft");
    m_pArrowLeft->set3StatePic(PIC_GENERAL_BTN_PRE_PAGE,
                               PIC_GENERAL_BTN_PRE_PAGE_CLICK,
                               PIC_GENERAL_BTN_PRE_PAGE);
    m_pArrowLeft->setIconSize(QSize(56, 60));
    m_pArrowLeft->setGeometry(iStartPointX, 160, 56, 60);
    connect(m_pArrowLeft, SIGNAL(clicked()), this, SLOT(ClickLeftArrow()));

    //向右箭头
    m_pArrowRight = new qUIPushButton(this);
    if (m_pArrowRight == NULL)
    {
        return;
    }
    m_pArrowRight->setObjectName("ArrowRight");
    m_pArrowRight->set3StatePic(PIC_GENERAL_BTN_NEXT_PAGE,
                                PIC_GENERAL_BTN_NEXT_PAGE_CLICK,
                                PIC_GENERAL_BTN_NEXT_PAGE);
    m_pArrowRight->setIconSize(QSize(56, 60));
    m_pArrowRight->setGeometry(466 + iStartPointX, 160, 56, 60);
    connect(m_pArrowRight, SIGNAL(clicked()), this, SLOT(ClickRightArrow()));

    //调整图片显示位置和大小
    m_pLabPreview->setGeometry(QRect(iStartPointX + 56 + 16, 61, 466 - 56 - 16 - 16,
                                     this->height() - 61 - 13));
    //设置Combox大小
    m_pCmbBoxWallpaper->setFixedSize(245, 36);

    m_pCmbBoxWallpaper->setFocusPolicy(Qt::StrongFocus);
    m_pLabPreview->setFocusPolicy(Qt::NoFocus);
    m_pArrowLeft->setFocusPolicy(Qt::NoFocus);
    m_pArrowRight->setFocusPolicy(Qt::NoFocus);
    this->setFocusPolicy(Qt::NoFocus);

    //使Label能够完整显示图片
    m_pLabPreview->setScaledContents(true);

    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxWallpaper, Qt::black);

    m_pCmbBoxWallpaper->installEventFilter(this);
    this->installEventFilter(this);

    QWidgetPair qPair;
    QString strLab;
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
    if (m_eType == WALLPAPAER_DSSKEY)
    {
        strLab = LANG_TRANSLATE(TRID_DSSKEY_WALLPAPER);
    }
    else
#endif
    {
        strLab = LANG_TRANSLATE(TRID_WALLPAPER);
    }

    qPair.first = strLab;
    qPair.second = m_pCmbBoxWallpaper;
    m_vecWidgetPair.push_back(qPair);
    AddWidget(m_vecWidgetPair);

    m_pCmbBoxWallpaper->setCurrentIndex(-1);
    //信号插槽连接用于焦点行变化
    connect(m_pCmbBoxWallpaper, SIGNAL(currentIndexChanged(int)),
            this, SLOT(OnCmbCurrentIndexChanged(int)));
}

//设置当前页面数据
void CDlgDisplayWallpaper::SetCurrentPageData(WallPaperListData & currentPageData)
{
    //取得空数据直接返回
    if (currentPageData.m_listPaperData.size() == 0)
    {
        return;
    }

    //预先置空
    QPixmap objPixmap;
    m_pLabPreview->setPixmap(objPixmap);

    yl::string strCurrentWallpaper = "";
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
    if (m_eType == WALLPAPAER_DSSKEY)
    {
        WallPaperData paperData;
        paperData.m_strResName = "Auto";
        paperData.m_strPaperDisplayName = "Auto";
        paperData.m_strPaperPath = "Auto";
        currentPageData.m_listPaperData.push_front(paperData);

        strCurrentWallpaper = g_pDisplayController->GetDsskeyWallpaper();
    }
    else
#endif
    {
        //取得当前使用的桌面背景
        strCurrentWallpaper = g_pDisplayController->GetCurrentWallpaper();
    }

    if (strCurrentWallpaper.empty())
    {
        SETTINGUI_WARN("current wallpaper is empty!");
    }

    //取得list迭代指针开始
    YLList<WallPaperData>::const_iterator iter =
        currentPageData.m_listPaperData.begin();
    //取得该list链表标识
    YLList<WallPaperData>::const_iterator listEnd  =
        currentPageData.m_listPaperData.end();

    int nCurrentIndex = 0;
    m_pCmbBoxWallpaper->clear();

    m_pCmbBoxWallpaper->blockSignals(true);
    for (int i = 0 ; iter != listEnd; iter++, i++)
    {
        const WallPaperData & refData = *iter;
        m_pCmbBoxWallpaper->addItem(toQString(refData.m_strPaperDisplayName.c_str()),
                                    QVariant::fromValue(refData));

        //如果该item值与当前使用桌面背景相等，设置标识
        if (refData.m_strPaperPath == strCurrentWallpaper)
        {
            nCurrentIndex = i;
        }
    }
    m_pCmbBoxWallpaper->blockSignals(false);

    m_pCmbBoxWallpaper->setCurrentIndex(nCurrentIndex);
    if (nCurrentIndex == 0)
    {
        //是0的时候不会触发槽函数，主动调用
        OnCmbCurrentIndexChanged(0);
    }
}

bool CDlgDisplayWallpaper::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (NULL == m_pGeneralFrame)
    {
        return false;
    }

    yl::string strPic = GetCurrentPicture().c_str();
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
    if (m_eType == WALLPAPAER_DSSKEY
            && strcasecmp(strPic.c_str(), "auto") == 0)
    {
        objWndData[1].m_strSoftkeyAction = "";
        objWndData[1].m_strSoftkeyTitle = "";
        objWndData[1].m_strIconPath = "";
        return true;
    }
#endif

    if (g_pDisplayController->CanRemoveWallpaper(strPic))
    {
        objWndData[1].m_strSoftkeyAction = "delete";
        objWndData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DELETE);
        objWndData[1].m_strIconPath = PIC_SOFTKEY_REMOVE;
    }
    else
    {
        objWndData[1].m_strSoftkeyAction = "";
        objWndData[1].m_strSoftkeyTitle = "";
        objWndData[1].m_strIconPath = "";
    }

    return true;
}

void CDlgDisplayWallpaper::DeleteWallpapper()
{
    MessageBox_ShowCommonMsgBox(this,
                                LANG_TRANSLATE(TRID_IF_DELETE_SELECTED_WALLPAPPER),
                                MESSAGEBOX_OkCancel, -1, "", "",
                                MSG_ID_DELETE_WAPPER);
}

void CDlgDisplayWallpaper::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    switch (pMessageBox->GetID())
    {
    case MSG_ID_DELETE_WAPPER:
        {
            if (pMessageBox->GetResult() == MessageBox_OK)
            {
                RemoveResutType eResult = g_pDisplayController->RemoveWallpaper(GetCurrentData().m_strPaperPath);
                SETTING_INFO("delete result code = [%d]", eResult);
                if (RRT_OK != eResult)
                {
                    SETTINGUI_WARN("Warning : remove wallpapper error");
                }
                else
                {
                    MessageBox_ShowCommonMsgBox(this,
                                                LANG_TRANSLATE(TRID_DELETE_SUCCESSED),
                                                MESSAGEBOX_NOTE, SHORT_TIME, "", "",
                                                MSG_ID_AFTER_DELETE_NOTE);
                }
            }
        }
        break;
    case MSG_ID_AFTER_DELETE_NOTE:
        {
            LoadPageData();
        }
        break;
    default:
        break;
    }
}

bool CDlgDisplayWallpaper::OnAction(const QString & strAction)
{
    if (strAction == "delete")
    {
        DeleteWallpapper();
        return true;
    }
    return false;
}

void CDlgDisplayWallpaper::ClickLeftArrow()
{
    int iIndex = m_pCmbBoxWallpaper->currentIndex();
    int iMax = m_pCmbBoxWallpaper->count();
    iIndex = (iIndex == 0) ? iMax - 1 : iIndex - 1;
    m_pCmbBoxWallpaper->setCurrentIndex(iIndex);
}

void CDlgDisplayWallpaper::ClickRightArrow()
{
    int iIndex = m_pCmbBoxWallpaper->currentIndex();
    int iMax = m_pCmbBoxWallpaper->count();
    iIndex = (iIndex == iMax - 1) ? 0 : iIndex + 1;
    m_pCmbBoxWallpaper->setCurrentIndex(iIndex);
}

bool CDlgDisplayWallpaper::eventFilter(QObject * pObject, QEvent * pEvent)
{
    switch (pEvent->type())
    {
    case QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_LEFT:
                {
                    if (m_pCmbBoxWallpaper == NULL
                            || !m_bPageEnable)
                    {
                        return false;
                    }
                    QWidget * pFocusWidget = focusWidget();
                    //左右键切换combobox选项
                    if (pFocusWidget != m_pCmbBoxWallpaper)
                    {
                        if (m_pCmbBoxWallpaper != NULL)
                        {
                            int nTotalItem = m_pCmbBoxWallpaper->count();
                            int nCurrentItem = m_pCmbBoxWallpaper->currentIndex();
                            if (nCurrentItem == 0)
                            {
                                m_pCmbBoxWallpaper->setCurrentIndex(nTotalItem - 1);
                            }
                            else
                            {
                                m_pCmbBoxWallpaper->setCurrentIndex(nCurrentItem - 1);
                            }
                            return true;
                        }
                    }
                }
                break;

            case PHONE_KEY_RIGHT:
                {
                    if (m_pCmbBoxWallpaper == NULL
                            || !m_bPageEnable)
                    {
                        return false;
                    }
                    QWidget * pFocusWidget = focusWidget();
                    if (pFocusWidget != m_pCmbBoxWallpaper)
                    {
                        if (m_pCmbBoxWallpaper != NULL)
                        {
                            int nTotalItem = m_pCmbBoxWallpaper->count();
                            int nCurrentItem = m_pCmbBoxWallpaper->currentIndex();
                            if (nCurrentItem == nTotalItem - 1)
                            {
                                m_pCmbBoxWallpaper->setCurrentIndex(0);
                            }
                            else
                            {
                                m_pCmbBoxWallpaper->setCurrentIndex(nCurrentItem + 1);
                            }
                            return true;
                        }
                    }
                }
                break;

            default:
                break;
            }
        }
        break;
    case QEvent::FocusOut:
        {
            if (m_pCmbBoxWallpaper != NULL && focusWidget() != m_pCmbBoxWallpaper)
            {
                m_pCmbBoxWallpaper->setFocus();
            }
        }
        break;
    default:
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

// 窗口显示到界面上时的操作
void CDlgDisplayWallpaper::UpdateWnd()
{
    // 控制左右切换按钮是否显示
    m_bPageEnable ? m_pArrowLeft->show() : m_pArrowLeft->hide();
    m_bPageEnable ? m_pArrowRight->show() : m_pArrowRight->hide();

    CDlgBaseSubPage::UpdateWnd();
}

#if IF_FEATURE_T48_DSSKEY_WALLPAPER
// 设置小窗口数据
void CDlgDisplayWallpaper::SetData(void * pData /* = NULL */)
{
    if (strcmp(m_strSubPageAction.c_str(), "OpenSubPage(display_dsskey_wallpaper)") == 0)
    {
        m_eType = WALLPAPAER_DSSKEY;
    }
    else
    {
        m_eType = WALLPAPAER_NOMAL;
    }

    InitData();
}
#endif
