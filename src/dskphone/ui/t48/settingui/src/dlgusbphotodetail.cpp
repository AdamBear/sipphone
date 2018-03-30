#include "dlgusbphotodetail.h"
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
#include "usbphotolistcontrl.h"



Q_DECLARE_METATYPE(RecordFileInfo);
#define SET_AS       "set_as"
#define TEXTDELETE   "delete"
namespace
{
const int MAX_PICTURE_RESOLUTION_SIZE = 4200000;
}

CDlgUSBPhotoDetail::CDlgUSBPhotoDetail(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCmbBoxWallpaper(NULL)
    , m_pLabPreview(NULL)
    , m_pArrowLeft(NULL)
    , m_pArrowRight(NULL)
    , m_strError("")
    , m_bPictureAvailble(false)
{
    InitData();
}

CDlgUSBPhotoDetail::~CDlgUSBPhotoDetail()
{
}

//加载数据
bool CDlgUSBPhotoDetail::LoadPageData()
{
    SetCurrentPageData(g_pUSBPhotoContrl->m_vecRecordList);
    return true;
}

//获取数据状态是否改变
bool CDlgUSBPhotoDetail::IsStatusChanged()
{
    return false;
}


//插槽：行焦点行变化操作
void CDlgUSBPhotoDetail::OnCmbCurrentIndexChanged(int nIndex)
{
    g_pUSBPhotoContrl->OnChangePhoto(nIndex);
}

yl::string CDlgUSBPhotoDetail::GetCurrentPicture()
{
    return GetCurrentData().strPath + GetCurrentData().strName;
}

RecordFileInfo CDlgUSBPhotoDetail::GetCurrentData()
{
    if (NULL == m_pCmbBoxWallpaper)
    {
        return RecordFileInfo();
    }

    return m_pCmbBoxWallpaper->itemData(
               m_pCmbBoxWallpaper->currentIndex()).value<RecordFileInfo>();
}

//初始化数据，设置控件的属性、样式、数据等都在其中进行
void CDlgUSBPhotoDetail::InitData()
{
    SetWindowTitle(LANG_TRANSLATE(TRID_USB_PHOTO_DETAILS));
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
    m_pLabPreview->setAlignment(Qt::AlignCenter);
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
    strLab = LANG_TRANSLATE(TRID_USB_PHOTO);

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
void CDlgUSBPhotoDetail::SetCurrentPageData(YLVector<RecordFileInfo> & currentPageData)
{
    //取得空数据直接返回
    if (currentPageData.size() == 0)
    {
        emit BtnBackClick();
        return;
    }

    //预先置空
    QPixmap objPixmap;
    m_pLabPreview->setPixmap(objPixmap);


    //取得list迭代指针开始
    YLVector<RecordFileInfo>::iterator iter =
        currentPageData.begin();
    //取得该list链表标识
    YLVector<RecordFileInfo>::iterator listEnd  =
        currentPageData.end();

    int nCurrentIndex = g_pUSBPhotoContrl->GetCurrentIndex();
    m_pCmbBoxWallpaper->clear();

    m_pCmbBoxWallpaper->blockSignals(true);
    for (int i = 0 ; iter != listEnd; iter++, i++)
    {
        RecordFileInfo refData = *iter;
        m_pCmbBoxWallpaper->addItem(toQString(refData.strName),
                                    QVariant::fromValue(refData));

    }
    m_pCmbBoxWallpaper->blockSignals(false);

    m_pCmbBoxWallpaper->setCurrentIndex(nCurrentIndex);
    if (nCurrentIndex == 0)
    {
        //是0的时候不会触发槽函数，主动调用
        OnCmbCurrentIndexChanged(0);
    }
}

bool CDlgUSBPhotoDetail::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (NULL == m_pGeneralFrame)
    {
        return false;
    }

    yl::string strPic = GetCurrentPicture();


    if (strPic.length() > 0 && m_bPictureAvailble)
    {
        objWndData[0].m_strSoftkeyAction = SET_AS;
        objWndData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SET_AS);
        objWndData[0].m_strIconPath = PIC_SOFTKEY_SAVE;

        objWndData[1].m_strSoftkeyAction = TEXTDELETE;
        objWndData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DELETE);
        objWndData[1].m_strIconPath = PIC_SOFTKEY_REMOVE;

        objWndData[3].m_strSoftkeyAction = "";
        objWndData[3].m_strSoftkeyTitle = "";
        objWndData[3].m_strIconPath = "";
    }
    else
    {
        objWndData[0].m_strSoftkeyAction = "";
        objWndData[0].m_strSoftkeyTitle = "";
        objWndData[0].m_strIconPath = "";

        objWndData[1].m_strSoftkeyAction = "";
        objWndData[1].m_strSoftkeyTitle = "";
        objWndData[1].m_strIconPath = "";

        objWndData[3].m_strSoftkeyAction = "";
        objWndData[3].m_strSoftkeyTitle = "";
        objWndData[3].m_strIconPath = "";
    }

    return true;
}

bool CDlgUSBPhotoDetail::OnAction(const QString & strAction)
{
    if (strAction == TEXTDELETE)
    {
        g_pUSBPhotoContrl->DeleteCurrentIndex();
        return true;
    }
    else if (strAction == SET_AS)
    {
        g_pUSBPhotoContrl->SetAs(GetCurrentPicture(), GetCurrentData().strName);
    }
    return false;
}

void CDlgUSBPhotoDetail::ClickLeftArrow()
{
    int iIndex = m_pCmbBoxWallpaper->currentIndex();
    int iMax = m_pCmbBoxWallpaper->count();
    iIndex = (iIndex == 0) ? iMax - 1 : iIndex - 1;
    m_pCmbBoxWallpaper->setCurrentIndex(iIndex);
}

void CDlgUSBPhotoDetail::ClickRightArrow()
{
    int iIndex = m_pCmbBoxWallpaper->currentIndex();
    int iMax = m_pCmbBoxWallpaper->count();
    iIndex = (iIndex == iMax - 1) ? 0 : iIndex + 1;
    m_pCmbBoxWallpaper->setCurrentIndex(iIndex);
}

bool CDlgUSBPhotoDetail::eventFilter(QObject * pObject, QEvent * pEvent)
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
void CDlgUSBPhotoDetail::UpdateWnd()
{
    // 控制左右切换按钮是否显示
    m_bPageEnable ? m_pArrowLeft->show() : m_pArrowLeft->hide();
    m_bPageEnable ? m_pArrowRight->show() : m_pArrowRight->hide();
    CDlgBaseSubPage::UpdateWnd();
}

void CDlgUSBPhotoDetail::OnLoadPicture()
{
    if (NULL == m_pCmbBoxWallpaper
            || NULL == m_pLabPreview)
    {
        return;
    }
    //t46内存不够 全部缓存会占用比较大内存  使用临时方案  查看的时候才把图片加载到内存中
    //是否存在内存
    QPixmap pixCurrent;
    bool bRusult = Image_IsExistInLRUCache(GetCurrentPicture().c_str(), pixCurrent);
    if (bRusult)
    {

    }
    else
    {
        QImageReader imageReader(QString::fromLocal8Bit(toQString(GetCurrentPicture()).toUtf8().data()));
        QSize sz = imageReader.size();

        if (sz.width() <= 0 || sz.height() <= 0)
        {
            m_strError = TRID_LOAD_IMAGE_FAILURE;

        }
        else if (sz.width() * sz.height() > MAX_PICTURE_RESOLUTION_SIZE)
        {
            m_strError = TRID_PIC_SET_RST_FILE_TOO_LARGE;
        }
        else
        {
            m_strError = "";
            pixCurrent.load(GetCurrentPicture().c_str());
        }

    }

    bool bRez = ToleranceLoadImage(pixCurrent);
    if (bRez)
    {
        m_bPictureAvailble = true;
        m_pLabPreview->setPixmap(pixCurrent);
        QPixmap pixAfterScale;

        pixAfterScale = pixCurrent.scaled(m_pLabPreview->width(), m_pLabPreview->height(),
                                          Qt::KeepAspectRatio);
        m_pLabPreview->setPixmap(pixAfterScale);

        //USB图片浏览的模式下将压缩后的缩略图加载入缓存
        if (!bRusult)
        {
            Image_InsertLRUCache(GetCurrentPicture().c_str(), pixAfterScale);
        }
    }
    else
    {
        m_bPictureAvailble = false;
        m_pLabPreview->setPixmap(QPixmap());
        m_pLabPreview->setText(LANG_TRANSLATE(m_strError));
    }

    emit RefreshSoftkey();
}

bool CDlgUSBPhotoDetail::ToleranceLoadImage(QPixmap & pix)
{
    //图片如果是超出规定大小则不允许格式容错加载
    if (m_strError == TRID_PIC_SET_RST_FILE_TOO_LARGE)
    {
        return false;
    }

    //对文件后缀与实际格式不符的文件进行容错
    if (pix.isNull())
    {
        QString strRealImagePath = QString::fromLocal8Bit(toQString(GetCurrentPicture()).toUtf8().data());
        QString strFileType = QFileInfo(strRealImagePath).suffix().toLower();

        // 图片实际扩展名跟保存扩展名不一致时，容错方案
        if (strFileType != "jpg")
        {
            QImageReader imageReader(strRealImagePath, "jpg");
            QSize sz = imageReader.size();

            if (sz.width() <= 0 || sz.height() <= 0)
            {
                m_strError = TRID_LOAD_IMAGE_FAILURE;
            }
            else if (sz.width() * sz.height() > MAX_PICTURE_RESOLUTION_SIZE)
            {
                m_strError = TRID_PIC_SET_RST_FILE_TOO_LARGE;
                return false;
            }
            else
            {
                return pix.load(strRealImagePath, "JPG");
            }
        }

        if (strFileType != "jpeg")
        {
            QImageReader imageReader(strRealImagePath, "jpeg");
            QSize sz = imageReader.size();

            if (sz.width() <= 0 || sz.height() <= 0)
            {
                m_strError = TRID_LOAD_IMAGE_FAILURE;
            }
            else if (sz.width() * sz.height() > MAX_PICTURE_RESOLUTION_SIZE)
            {
                m_strError = TRID_PIC_SET_RST_FILE_TOO_LARGE;
                return false;
            }
            else
            {
                return pix.load(strRealImagePath, "JPEG");
            }
        }

        if (strFileType != "png")
        {
            QImageReader imageReader(strRealImagePath, "png");
            QSize sz = imageReader.size();

            if (sz.width() <= 0 || sz.height() <= 0)
            {
                m_strError = TRID_LOAD_IMAGE_FAILURE;
            }
            else if (sz.width() * sz.height() > MAX_PICTURE_RESOLUTION_SIZE)
            {
                m_strError = TRID_PIC_SET_RST_FILE_TOO_LARGE;
                return false;
            }
            else
            {
                return pix.load(strRealImagePath, "PNG");
            }
        }

        if (strFileType != "bmp")
        {
            QImageReader imageReader(strRealImagePath, "bmp");
            QSize sz = imageReader.size();

            if (sz.width() <= 0 || sz.height() <= 0)
            {
                m_strError = TRID_LOAD_IMAGE_FAILURE;
            }
            else if (sz.width() * sz.height() > MAX_PICTURE_RESOLUTION_SIZE)
            {
                m_strError = TRID_PIC_SET_RST_FILE_TOO_LARGE;
                return false;
            }
            else
            {
                return pix.load(strRealImagePath, "BMP");
            }
        }
        return false;
    }

    return true;
}

