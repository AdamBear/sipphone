#ifndef DLG_SET_ACCOUNT_RINGTONE_H
#define DLG_SET_ACCOUNT_RINGTONE_H

#include "cdlgbasesubpage.h"
#include "setting/include/common.h"

////////////////////////////////////////////////////////////////////////////////
class CDlgSetAccountRingTone : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgSetAccountRingTone(QWidget * parent = 0);
    ~CDlgSetAccountRingTone();
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgSetAccountRingTone();
    }

public:
    virtual void SetData(void * pData = NULL);
    //加载数据
    virtual bool LoadPageData();
    //获取数据状态是否改变
    virtual bool IsStatusChanged();
    //保存数据
    virtual bool SavePageData();
    //取得子页面softkey信息
    // 初始化数据
    virtual void Init();
    //解决退出铃声关闭问题
    virtual void Uninit();
    virtual bool SaveCompleted();
    virtual void BeforeSetSubPage(void * pData);

    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                 QRect & rtVolume, yl::string & strBGPath) const;

private slots:
    //当选择行改变时
    virtual void OnCurrentIndexChanged(int iDataIndex);
    void FirstPreview();

public:
    //播放铃声
    void Preview();
    //停止播放铃声
    void Pause();

    //焦点下一步
    //焦点上移一步
    //删除后焦点转移控制
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

private:

    void InitPageData();
    /*  void keyPressEvent(QKeyEvent * keyEvent);*/
    //由于标志项转移时，不能及时刷新页面，用此方式刷新table widget
    void SetCurrentPageData(RingFileListData & currentPageData);

    //设置默认字体颜色

private:
    const yl::string GetDefaultRingPath() const;
    const yl::string GetCurrentRingPath() const;
    bool SaveCurrentRingPath(const yl::string & strFilePath);
    // 非默认账号
    bool IsSetAccountNotDef();

private slots:
    void RequestPlayRing();
    void OnPlaying();

private:
    //用于记录前一次焦点行序号
    int m_nPreviousRow;
    //用于记录当前行序号
    int m_nCurrentRow;
    //用于延时两秒播放铃声的定时器，
    //当两秒内焦点变化到非列表第一铃声时，停止该启用的定时器
    QTimer m_timerDelay;
    QTimer m_timerDelayPlayRing;//延迟播放铃声，解决快速切换时等2,3秒才播放铃声问题。
    bool m_bIsCurrentKeyRingOn;//记录当前按键开关状态，解决在铃声界面保证没有按键音问题。

    std::vector<yl::string> m_vecFullPath;
    int m_iAccountID;  //账号Id

    int m_nSetRingToneType;
};

#endif // DLG_SET_ACCOUNT_RINGTONE_H
