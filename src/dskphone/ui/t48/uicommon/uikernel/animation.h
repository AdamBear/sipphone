#ifndef ANIMATION_H
#define ANIMATION_H

#include <ylstring.h>
#include "ylstl/yllist.h"
#include <QLabel>
#include <QtGui>

class CAnimation : public QLabel
{
    Q_OBJECT

public:
    //构造函数
    CAnimation(QWidget * pParent = NULL);
    //直接通过指定目录构造CPlayLaber对象
    CAnimation(yl::string strDirectoryPath, QWidget * pParent = NULL);
    //直接通过图片链表数据指针构造CPlayLabel对象
    CAnimation(YLList<yl::string> listImage, QWidget * pParent = NULL);
    //析构函数
    ~CAnimation();

public:
    //设置播放速度
    void SetPlaySpeed(int nInterval);
    //停止播放
    void Stop();
    //从指定目录加载图片
    bool LoadListImage(yl::string strDirectoryPath);
    //直接加载外部链表图片
    bool LoadListImage(YLList<yl::string> listImage);

public slots:
    //开始播放
    void Play();

private:
    //获得当前要播放的图片
    QPixmap GetCurentPlayImage();

    //初始化数据
    void InitData();

private:
    //定时器间隔时间
    int m_nInterval;

    //定义播放时使用的定时器
    QTimer m_timerPlay;

    //播放的图片序号
    int m_nPlayIndex;

    //用于存放从路径目录加载的存放播放图片列表
    YLList<yl::string> m_listImage;

    //用于存放加动画图片的目录名称
    yl::string m_strDirectory;
};

#endif // ANIMATION_H
