#include "animation.h"
#include "commonapi/systemhelperapi.h"
#include "imagemanager/modimagemanager.h"

#define DEFAULT_PLAY_INTERVAL 1000

////////////////////////////////////////////////////////////////////////////////
//构造函数
CAnimation::CAnimation(QWidget * pParent)
    : QLabel(pParent)
{
    InitData();
}

//直接通过指定目录构造CAnimation对象
CAnimation::CAnimation(yl::string strDirectoryPath, QWidget * pParent /*= NULL*/)
    : QLabel(pParent)
{
    InitData();
    //通过目录加载路径
    LoadListImage(strDirectoryPath);
}

//直接通过图片链表数据指针构造CAnimation对象
CAnimation::CAnimation(YLList<yl::string> listImage, QWidget * pParent/* = NULL*/)
    : QLabel(pParent)
{
    InitData();
    //通过外部提供的链表数据加载数据
    LoadListImage(listImage);
}

//析构函数
CAnimation::~CAnimation()
{
    //释放列表
    m_listImage.clear();
    Stop();
}

void CAnimation::InitData()
{
    //设置播放序号
    m_nPlayIndex = -1;
    //清空播放列表
    m_listImage.clear();
    //设置默认播放速度
    m_nInterval = DEFAULT_PLAY_INTERVAL;
    //设置显示图标为居中
    this->setAlignment(Qt::AlignCenter);

    //连接信号插槽，当信号到时播放一张图片
    connect(&m_timerPlay, SIGNAL(timeout()), this, SLOT(Play()));
}

//设置播放速度
void CAnimation::SetPlaySpeed(int nInterval)
{
    //设置指定播放速度
    m_nInterval = nInterval;
    //如果正在播放，那么需要先停止定时器
    //再用新设定速度开启定时器
    if (m_timerPlay.isActive())
    {
        m_timerPlay.stop();
        m_timerPlay.start(m_nInterval);
    }
}

//开始播放
void CAnimation::Play()
{
    if (m_listImage.size() == 0)
    {
        //播放列表为空，返回
        return;
    }

    if (!m_timerPlay.isActive())
    {
        //开启定时器
        m_timerPlay.start(m_nInterval);
    }

    //播放按顺序播放一张图片
    QPixmap pmAnimation = GetCurentPlayImage();
    //显示图片
    this->setPixmap(pmAnimation);
}

//停止播放
void CAnimation::Stop()
{
    //停止定时器
    m_timerPlay.stop();
}

//从指定目录加载图片
bool CAnimation::LoadListImage(yl::string strDirectoryPath)
{
    //取得动画图片目录路径
    m_strDirectory = strDirectoryPath;
    //如果直接已经有数据了，那么要先释放数据
    m_listImage.clear();

    //解析文件目录列表
    if (commonAPI_getFilesInDirectoryByExt(&m_listImage, strDirectoryPath, ".png,.jpg,.bmp,.gif"))
    {
        //加载成功
        return true;
    }
    else
    {
        //加载失败
        return false;
    }
    return false;
}

//直接加载外部链表图片
bool CAnimation::LoadListImage(YLList<yl::string> listImage)
{
    //如果直接已经有数据了，那么要先释放数据
    m_listImage.clear();

    //赋值图片列表
    m_listImage = listImage;
    return true;
}

//获得当前要播放的图片
QPixmap CAnimation::GetCurentPlayImage()
{
    //计数器加1
    m_nPlayIndex++;

    //如果计数器值超过图片数，那么播放第一张图片
    if (m_nPlayIndex >= m_listImage.size())
    {
        m_nPlayIndex = 0;
    }

    //取得链表头指针
    YLList<yl::string>::const_iterator itFind = m_listImage.begin();
    for (int nIndex = 0; itFind != m_listImage.end(); nIndex++, itFind++)
    {
        //查找指定序号图片
        if (nIndex == m_nPlayIndex)
        {
            //获得当前播放的图片路径
            yl::string strImage = *itFind;
            strImage = m_strDirectory + "/" + strImage;
            return STOCK_GET_BMP(strImage.c_str());
        }
    }
    return QPixmap();
}
