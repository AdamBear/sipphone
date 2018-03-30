#include "cdlgpreviewpicturefullscreen.h"
#include "picpath.h"
#include "uikernel/languagehelper.h"
#include "uimanager/moduimanager.h"
#include "imagemanager/modimagemanager.h"
#include "commonunits/modcommonunits.h"

#define WIDTH   800
#define HEIGHT  480


CDlgPreviewPictureFullScreen::CDlgPreviewPictureFullScreen()
    : CBaseDialog()
{
    m_labPreview = NULL;
    m_display = NULL;

    m_labPreview = new QLabel(this);

    if (m_labPreview != NULL)
    {
        m_labPreview->setGeometry(QRect(0, 0, WIDTH, HEIGHT));
        m_labPreview->setAlignment(Qt::AlignCenter);
    }


//  Qt::WindowFlags flags = 0;
//  flags |= Qt::FramelessWindowHint;
//  setWindowFlags(flags);

    //Preview();
}

CDlgPreviewPictureFullScreen::~CDlgPreviewPictureFullScreen()
{
    //!!!!!
    //设置为顶层窗口
    SetTopWnd();
}

void CDlgPreviewPictureFullScreen::Init()
{
}

void CDlgPreviewPictureFullScreen::Uninit()
{

}

void CDlgPreviewPictureFullScreen::SetData(void * pData)
{
    // 设置小窗口的风格为图像全屏预览
    //!!!!!
    m_eWndType = eWT_FullScreen;

    // 设置小窗口对应SoftKeybar的数据
    // 初始化第一个softkey数据
    m_SoftkeyData[0].m_strSoftkeyAction  = "";
    m_SoftkeyData[0].m_strSoftkeyTitle = LANG_TRANSLATE("");

    // 初始化第二个softkey数据
    m_SoftkeyData[1].m_strSoftkeyAction  = "";
    m_SoftkeyData[1].m_strSoftkeyTitle = LANG_TRANSLATE("");

    // 初始化第三个softkey数据
    m_SoftkeyData[2].m_strSoftkeyAction  = "";
    m_SoftkeyData[2].m_strSoftkeyTitle = LANG_TRANSLATE("");

    // 初始化第四个softkey数据
    m_SoftkeyData[3].m_strSoftkeyAction  = "";
    m_SoftkeyData[3].m_strSoftkeyTitle = LANG_TRANSLATE("");

    //添加信号接收对象指针
    for (int i = 0; i < 4; i++)
    {
        m_SoftkeyData[i].m_strSlotName = SLOT(OnSoftKeyClick(qSoftButton *));
        m_SoftkeyData[i].m_pReceiver = this;
    }
}


CBaseDialog * CDlgPreviewPictureFullScreen::CreateInstance()
{
    return (new CDlgPreviewPictureFullScreen());
}

void CDlgPreviewPictureFullScreen::OnSoftKeyClick(qSoftButton * pSoftBtn)
{

}

void CDlgPreviewPictureFullScreen::SetDisplay(CDlgDisplayWallpaper * val)
{
    m_display = val;
}

void CDlgPreviewPictureFullScreen::Preview()
{
    if (m_display == NULL)
    {
        return;
    }

    yl::string path = m_display->GetCurrentPicture();

    if (path.empty())
    {
        return;
    }

    // 获得图片类型
    ImagePathType eImageType = IMAGE_PATH_SYSTEM;
//  ImagePathType eImageType = (ImagePathType)CommonUnits_GetImageType(path);

    //加载图片
    QPixmap objPixmap;
    if (eImageType == IMAGE_PATH_SYSTEM || eImageType == IMAGE_PATH_VARDATA)
    {
        // 从系统资源中加载图片
        objPixmap = STOCK_GET_BMP(path.c_str());
    }
    else if (eImageType == IMAGE_PATH_USERDATA)
    {
        // 从用户资源中加载图片
        objPixmap = STOCK_GET_BMP(path.c_str());
    }
    else
    {
        // 先从用户资源中加载图片容错,并给出警告
        objPixmap = STOCK_GET_BMP(path.c_str());
        chWARNING2(false, "CDlgDisplayWallpaper Preview ImagePathType error");
    }

    m_labPreview->setPixmap(objPixmap);
}


bool CDlgPreviewPictureFullScreen::eventFilter(QObject * pObject, QEvent * pEvent)
{
    // 入口参数判断
    if (pObject == NULL || pEvent == NULL)
    {
        return false;
    }

    // 处理点击事件
    if (pEvent->type() == QEvent::MouseButtonPress)
    {
        UIManager_PutPrivateDlg(this);
        return true;
    }
    else if (pEvent->type() == QEvent::KeyPress)
    {
        switch (((QKeyEvent *)pEvent)->key())
        {
        case Qt::Key_Up:
            {
                if (m_display != NULL)
                {
                    //m_display->Prev();
                    Preview();
                }
            }
            break;
        case Qt::Key_Down:
            {
                if (m_display != NULL)
                {
                    //  m_display->Next();
                    Preview();
                }

            }
            break;
        default:
            //this->close();
            UIManager_PutPrivateDlg(this);
            break;
        }
        return true;
    }
    else
    {
        return false;
    }

}
