#include "talkuiframetopbar.h"
#include "uikernel/qwidgetlist.h"
#include "baseui/t4xpicpath.h"

#include "imagemanager/modimagemanager.h"

const QRect QLIST_ICON_GEOMETRY(305, 10, 120, 25);
#define STATUS_ICON_WIDTH_EXTEND   40
#define STATUS_ICON_WIDTH   27
#define STATUS_ICON_HEIGHT  25

qFrameStatusImage::qFrameStatusImage(QWidget * parent/* = 0*/)
    : QFrame(parent)
{
    m_pWidgetList = new qWidgetList(this, qWidgetList::ALIGN_TYPE_RIGHT, QRect(QLIST_ICON_GEOMETRY));
}

qFrameStatusImage::~qFrameStatusImage()
{

}

//清除所有图标
void qFrameStatusImage::ClearAllIcon()
{
    if (NULL != m_pWidgetList)
    {
        m_pWidgetList->Clear();
    }
}

// 添加、删除右上角的图标
void qFrameStatusImage::AddOrDeleteIcon(const yl::string & strIconPath, bool bAdd)
{
    if (NULL == m_pWidgetList)
    {
        return;
    }

    yl::string strPath = UiPictureToChannel(strIconPath);

    if (!bAdd)
    {
        // 删除图标
        AnimationData objAnimation;
        m_pWidgetList->DeleteItem(objAnimation, strPath.c_str());

    }
    else if (!m_pWidgetList->IsItemExist(strPath))
    {
        // 添加图标
        qWidgetListItemDataLab objItemLab;
        objItemLab.itemSize = GetIconSize(strPath);
        objItemLab.strPicPath = strPath.c_str();
        objItemLab.strId      = strPath.c_str();
        m_pWidgetList->AddItem(&objItemLab, 0);
    }
}

//改变大小事件
void qFrameStatusImage::resizeEvent(QResizeEvent * pResizeEvent)
{
    QRect rect = geometry();
    if (NULL != m_pWidgetList)
    {
        m_pWidgetList->setGeometry(0, 0, rect.width(), rect.height());
    }
}

// 界面图标到通道图标的映射
yl::string qFrameStatusImage::UiPictureToChannel(const yl::string & strIconPath)
{
    if (strIconPath == PIC_ICON_HD)
    {
        return PIC_ICON_CHANNEL_HD;
    }
    else if (strIconPath == PIC_ICON_TALK_BT_UNCONNECTED)
    {
        return PIC_ICON_BLUE_TOOTH_TYPE_COMMON;
    }
    else if (strIconPath == PIC_ICON_TALK_BT_CONNECTED)
    {
        return PIC_ICON_BLUE_TOOTH_CONNECT;
    }
    else if (strIconPath == PIC_ICON_TALK_MUTE)
    {
        return PIC_ICON_RINGMUTE;
    }
    else if (strIconPath == PIC_ICON_ENCRYPT)
    {
        return PIC_ICON_CHANNEL_ENCRYPT;
    }
    else if (strIconPath == PIC_ICON_AMR)
    {
        return PIC_ICON_CHANNEL_AMR;
    }
    else if (strIconPath == PIC_ICON_OPUS)
    {
        return PIC_ICON_CHANNEL_OPUS;
    }
    else if (strIconPath == PIC_ICON_DND_SMALL)
    {
        return PIC_ICON_DND;
    }
    else
    {
        return strIconPath;
    }
}

QSize qFrameStatusImage::GetIconSize(const yl::string & strIconPath)
{
    QSize itemSize = QSize(STATUS_ICON_WIDTH, STATUS_ICON_HEIGHT);
    if (strIconPath == PIC_ICON_CHANNEL_AMR
            || strIconPath == PIC_ICON_CHANNEL_OPUS)
    {
        itemSize = QSize(STATUS_ICON_WIDTH_EXTEND, STATUS_ICON_HEIGHT);;
    }

    QPixmap pic = THEME_GET_BMP(strIconPath.c_str());
    if (!pic.isNull())
    {
        itemSize = pic.size();
    }

    return itemSize;
}
