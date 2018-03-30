#include "cuiregionimagescreen.h"
#include "xmlbrowserui/xmlbrowseruimanager.h"
#include "xbpositiondefine.h"
//#include "commonapi/callmethod.h"
#include <ylstring.h>
//#include "commonapi/commonapi.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "uikernel/languagehelper.h"
#include "messagebox/modmessagebox.h"
#include "uimanager/moduimanager.h"
#include "uimanager/modlogicuimanager.h"
#include "xmlbrowsermessagebox.h"
#include "mainwnd/mainwnd.h"
#include "uimanager/rectdefine.h"
#include "translateiddefine.h"

#include <QImage>

CUIRegionImageScreen::CUIRegionImageScreen(QWidget * parent/* = 0*/, Qt::WFlags flags/* = 0*/)
    : CUIRegionBase(parent, flags)
    , m_imageMode(IMAGE_MODE_FULLSCREEN)
{
    setObjectName("UIRegionImageScreen");
    SetElementType(XMT_IMAGESCREEN);

    m_labImage.setParent(this);
    m_labImage.setObjectName("LabImage");

    connect(&m_timerDelayMessageBox, SIGNAL(timeout()), this, SLOT(OnDelayMessageBox()));
}

CUIRegionImageScreen::~CUIRegionImageScreen()
{
    m_timerDelayMessageBox.stop();
}

// 根据数据更新界面
void CUIRegionImageScreen::Update(void * pData)
{
    if (pData == NULL)
    {
        return;
    }

    //!!!!! XURIList 这个是做什么用的
    XBImageScreenData * pNewData = static_cast<XBImageScreenData *>(pData);

    // 设置键盘是否锁定
    m_bLock = pNewData->stAttr.bLockIn;

    // 设置图片的显示模式
    SetImageMode(pNewData->stAttr.eMode);

    //!!!!! if (!_XMLBrowserManager.IsLoadingImage())
    {
        // 如果是全屏,则修改窗口的模板为全屏显示
        if (IMAGE_MODE_FULLSCREEN == pNewData->stAttr.eMode)
        {
            m_labImage.setGeometry(0, 0, PHONE_WIDTH, PHONE_HEIGHT);
        }
        else
        {
            m_labImage.setGeometry(0, 0, this->width(), this->height());
        }
    }

    if (pNewData->listImage.size() > 0)
    {
        ////---- 目前只画一张图片
        DrawImage(m_imageMode, pNewData->listImage.front());
    }

    m_iQuitTime = pNewData->stAttr.iTimeOut;

    SetSoftkey(pNewData->vecSoftkey);
}

void CUIRegionImageScreen::SetImageMode(IMAGE_MODE eMode)
{
    m_imageMode = eMode;
}

void CUIRegionImageScreen::DrawImage(IMAGE_MODE eMode, ImageItem * pImageItem)
{
    XMLBROWSERUI_INFO("CUIRegionImageScreen::DrawImage");
    if (pImageItem == NULL)
    {
        return;
    }

    XMLBROWSERUI_INFO("Image load status [%d]", pImageItem->eLoadStatus);
    // 如果还在下载中,则不显示图片
    if (pImageItem->eLoadStatus != ILS_LOAD_OK)
    {
        if (pImageItem->eLoadStatus == ILS_DOWNLOAD
                || pImageItem->eLoadStatus == ILS_COMPRESS)
        {
            m_strNoteContent = LANG_TRANSLATE(TRID_IMAGE_LOADING_PLEASE_WAIT);
            m_timerDelayMessageBox.start(1);
            return;
        }
        // 其它是失败情况，则不显示图片即可。容错
        XMLBROWSERUI_INFO("Image load fail.");
        return;
    }

    MessageBox_RemoveAllMessageBox(MessageBox_Destory);


#if 0
    switch (eMode)
    {
    case IMAGE_MODE_REGULAR:// 正常显示模式
        {
            //调用imageScale.sh脚本实现图片压缩
            //压缩大小由显示区域决定
            yl::string strTemp = CommonAPI_FormatString(PHONE_SCRIPTS_PATH "imageScale.sh %s %d %d %s 50000000"
                                 , pImageItem->strImagePath.c_str(), this->geometry().width()
                                 , this->geometry().height(), pImageItem->strImagePath.c_str());
            system(strTemp.c_str());
        }
        break;
    case IMAGE_MODE_FULLSCREEN:// 全屏模式
        {
            //调用imageScale.sh脚本实现图片压缩
            //因为是全屏显示，所以压缩到 480x272
            yl::string strTemp = CommonAPI_FormatString(PHONE_SCRIPTS_PATH "imageScale.sh %s %d %d %s 5000000"
                                 , pImageItem->strImagePath.c_str(),
                                 , PHONE_WIDTH
                                 , PHONE_HEIGHT
                                 , pImageItem->strImagePath.c_str());
            system(strTemp.c_str());
        }
        break;

    default:
        break;
    }

    // 判断压缩结果
    chString strScaleRet = etlDumpTextFileA(TEMP_PATH "imageScale.out");
    if (strScaleRet == "2")
    {
        // 分辨率过高的提示信息
        m_strNoteContent = LANG_TRANSLATE(TRID_TOO_HIGHT_OF_RESOLUTION);
        m_timerDelayMessageBox.start(1);

        return;
    }
    else if (strScaleRet != "1")
    {
        // 压缩失败给出提示信息
        m_strNoteContent = LANG_TRANSLATE(TRID_CAN_NOT_COMPRESS_PICTURES);
        m_timerDelayMessageBox.start(1);

        return;
    }
#endif

    // 读取图片
    QPixmap image;
    image.load(pImageItem->strImagePath.c_str());

    if (image.isNull())
    {
        return;
    }

    //根据不同的显示模式设置对应的显示位置
    switch (eMode)
    {
    case IMAGE_MODE_REGULAR:// 正常显示模式
        {
            // 计算lab的位置
            QRect rect = ImageLabGeometry(image.size(), pImageItem);

            // 判断位置是否正确
            if (rect.isValid())
            {
                // 设置lab的位置
                m_labImage.setGeometry(rect);

                // 设置图片
                m_labImage.setPixmap(image);
            }
        }
        break;
    case IMAGE_MODE_FULLSCREEN:// 全屏模式
        {
            //设置背景为纯色
            QPalette pl ;
            pl.setBrush(QPalette::Background, QColor(0x00, 0xff, 0x00, 0x00));
            m_labImage.setPalette(pl);

            // 设置Lab为全屏大小
            m_labImage.setGeometry(0, 0, this->geometry().width(), this->geometry().height());

            // 设置图片
            m_labImage.setPixmap(image.scaled(QSize(m_labImage.width(), m_labImage.height())));

        }
        break;

    default:
        break;
    }
}

QRect CUIRegionImageScreen::ImageLabGeometry(const QSize & imageSize, ImageItem * pImageItem)
{
    if (pImageItem == NULL)
    {
        return QRect(0, 0, 0, 0);
    }

    QRect rect;

    if (imageSize.width() >= this->width())
    {
        rect.setX(0);
    }
    else
    {
        switch (pImageItem->eHorizontal)
        {
        case IMAGE_HORIZONTAL_ALIGN_LEFT: // 图片水平左对齐
            {
                rect.setX(0);
            }
            break;
        case IMAGE_HORIZONTAL_ALIGN_CENTER: // 图片水平居中对齐
            {
                rect.setX((this->width() - imageSize.width()) / 2);
            }
            break;
        case IMAGE_HORIZONTAL_ALIGN_RIGHT: // 图片水平右对齐
            {
                rect.setX(this->width() - imageSize.width());
            }
            break;

        default:
            break;
        }
    }

    if (imageSize.height() >= this->height())
    {
        rect.setY(0);
    }
    else
    {
        switch (pImageItem->eVertical)
        {
        case IMAGE_VERTICAL_ALIGN_TOP: // 图片顶端对齐
            {
                rect.setY(0);
            }
            break;

        case IMAGE_VERTICAL_ALIGN_CENTER: // 图片垂直居中
            {
                rect.setY((this->height() - imageSize.height()) / 2);
            }
            break;

        case IMAGE_VERTICAL_ALIGN_BOTTOM: // 图片底端对齐
            {
                rect.setY(this->height() - imageSize.height());
            }
            break;

        default:
            break;
        }
    }

    rect.setWidth(imageSize.width());
    rect.setHeight(imageSize.height());

    XMLBROWSERUI_INFO("******************qt about ImageScreen log***********************************");
    XMLBROWSERUI_INFO("RegionSize(width = %d, height = %d)", this->width(), this->height());

    XMLBROWSERUI_INFO("rect(x = %d, y = %d, width = %d, height = %d)",
                      rect.x(), rect.y(), rect.width(), rect.height());

    XMLBROWSERUI_INFO("imageSize(width = %d, height = %d)",
                      imageSize.width(), imageSize.height());
    XMLBROWSERUI_INFO("******************qt about ImageScreen log***********************************");
    return rect;
}

void CUIRegionImageScreen::OnDelayMessageBox()
{
    m_timerDelayMessageBox.stop();

    CBaseDialog * pXBDialog = UIManager_GetPrivateDlg("CDlgXMLBrowser");
    //在talkui界面不弹窗,xmlbrowser 已经存在 但是不是顶层窗口时不弹窗
    bool bBlock = UIManager_IsInTalk() ||
                  (NULL != pXBDialog && pXBDialog != UIManager_GetTopWindow());

    if (bBlock)
    {
        return;
    }

    MessageBox_RemoveAllMessageBox(MessageBox_Destory);

    MessageBox_ShowCommonMsgBox(&_XBMessageManager, LANG_TRANSLATE(m_strNoteContent),
                                MESSAGEBOX_Cancel);
}

// 事件过滤器
bool CUIRegionImageScreen::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (!this->isActiveWindow())
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::KeyPress:
        {
            // 按键交给基类统一处理
            if (OnKeyPress(pObj, pEvent))
            {
                return true;
            }

            QKeyEvent * keyEvent = static_cast<QKeyEvent *>(pEvent);
            int iKey = keyEvent->key();
            switch (iKey)
            {
            case PHONE_KEY_OK:
                {
                    //发出按下ok键的信号
                    emit RegionAction(REGION_ACTION_CLICK_ITEM);
                    return true;
                }
                break;
            case PHONE_KEY_1:
            case PHONE_KEY_2:
            case PHONE_KEY_3:
            case PHONE_KEY_4:
            case PHONE_KEY_5:
            case PHONE_KEY_6:
            case PHONE_KEY_7:
            case PHONE_KEY_8:
            case PHONE_KEY_9:
            case PHONE_KEY_0:
                {
                    // 0
                    int iDgtKey = '0';
                    iDgtKey += (iKey - PHONE_KEY_0);
                    xmlbrowser_OnAction(GetElementID(), XB_ACTION_DIGITKEY, &iDgtKey);
                    return true;
                }
                break;
            case PHONE_KEY_POUND:
                {
                    int iDgtKey = '#';
                    xmlbrowser_OnAction(GetElementID(), XB_ACTION_DIGITKEY, &iDgtKey);
                    return true;
                }
                break;
            case PHONE_KEY_STAR:
                {
                    int iDgtKey = '*';
                    xmlbrowser_OnAction(GetElementID(), XB_ACTION_DIGITKEY, &iDgtKey);
                    return true;
                }
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }

    return false;
}
