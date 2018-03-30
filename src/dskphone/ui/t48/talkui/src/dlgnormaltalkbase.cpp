#include "dlgnormaltalkbase.h"
#include "talkuidelegate.h"
#include "uicommon/uimanager/rectdefine.h"
#include "imagemanager/modimagemanager.h"

// 联系人信息控件位置
const QRect RECT_CONNECT_INFO(6, 0, RECT_TALK_FRAME.width() - 8, RECT_TALK_FRAME.height());

CDlgNormalTalkBase::CDlgNormalTalkBase(const yl::string & strTalkUIName, TALK_UI_TYPE eTalkUIType)
    : CDlgBaseTalkUI(strTalkUIName, eTalkUIType)
{
#ifdef _T48
    //设置父窗口
    m_cConInfoFrame.setObjectName("ConInfoFrame");
    m_cConInfoFrame.setParent(this);
    m_cConInfoFrame.setGeometry(RECT_CONNECT_INFO);
#endif
}

CDlgNormalTalkBase::~CDlgNormalTalkBase(void)
{

}

// 初始化
void CDlgNormalTalkBase::Init()
{
    CDlgBaseTalkUI::Init();
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgNormalTalkBase::Uninit()
{
#ifdef _T48
    m_cConInfoFrame.Clear();
#endif

    CDlgBaseTalkUI::Uninit();
}

#ifdef _T48

//添加或删除图标
void CDlgNormalTalkBase::AddOrDeleteIcon(LPCSTR strIconPath, bool bAdd)
{
    m_cConInfoFrame.AddOrDeleteIcon(strIconPath, bAdd);
}

//删除所有图标
void CDlgNormalTalkBase::DeleteAllIcon()
{
    m_cConInfoFrame.DeleteAllIcon();
}

#endif

bool CDlgNormalTalkBase::IsCustomImage(const yl::string & strImagePath)
{
    if (strImagePath.empty())
    {
        return false;
    }

    bool bCustom = true;

    //如果从路径上判断已经是默认头像，则直接赋值结果，不加载图片
    if (_TalkUIDelegate.IsDefaultContactPhoto(strImagePath))
    {
        bCustom = false;
    }
    //如果路径上判断不出，则根据图像是否可用
    else
    {
        QPixmap pmIcon;
        if (pathFileExist(strImagePath.c_str()))
        {
            pmIcon = STOCK_GET_BMP(strImagePath.c_str());
        }

        bCustom = !pmIcon.isNull();
    }

    return bCustom;
}

void CDlgNormalTalkBase::SetCustomImageMark(std::vector<bool>&  vecContactCustomImage)
{
    if (configParser_GetConfigInt(kszDisplayContactPhoto) != 2)
    {
        return;
    }

    if (vecContactCustomImage.size() != m_vecContactCustomImage.size())
    {
        m_vecContactCustomImage.clear();
        m_vecContactCustomImage = vecContactCustomImage;
        TALKUI_INFO("Custom Image Mark is Change");
        emit LoadPhotoMarkChange();
        return;
    }

    bool bChange = false;

    for (int nIndex = 0; nIndex < vecContactCustomImage.size(); ++nIndex)
    {
        if (vecContactCustomImage.at(nIndex) != m_vecContactCustomImage.at(nIndex))
        {
            bChange = true;
            break;
        }
    }

    TALKUI_INFO("Custom Image Mark is Change or not? [%d]", bChange);

    if (!bChange)
    {
        return;
    }

    m_vecContactCustomImage.clear();
    m_vecContactCustomImage = vecContactCustomImage;

    emit LoadPhotoMarkChange();
}

void CDlgNormalTalkBase::SetCustomImageMark(yl::string strImagePath)
{
    if (strImagePath.empty())
    {
        return;
    }

    std::vector<bool>  vecContactCustomImage;
    vecContactCustomImage.push_back(IsCustomImage(strImagePath));

    SetCustomImageMark(vecContactCustomImage);
}
