#ifndef DLGSOUNDGENERAL_H
#define DLGSOUNDGENERAL_H

#include "cdlgbasesubpage.h"
#include "setting/include/common.h"

class qCheckBox;
struct SoundGeneralData;

class CDlgSoundGeneral : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgSoundGeneral(QWidget * parent = 0);
    ~CDlgSoundGeneral();
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgSoundGeneral();
    }

public:
    //加载数据
    virtual bool LoadPageData();
    //获取数据状态是否改变
    virtual bool IsStatusChanged();
    //保存数据
    virtual bool SavePageData();
    //页面向其数据Mode取数据再次刷新到界面上
    virtual void Refresh();

private:
    //初始化数据，设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();
    //用于界面析构时调用。在里面进行一些释放资源的操作，比如注销对信息的监控等。
    virtual void ReleaseData();
    //于界面每次show()出来的时候，重置界面的一些数据,输入法的显示等
    virtual void OnViewShow();

    //拷贝页面数据
    void CopyPageData(SoundGeneralData & destPageData,
                      const SoundGeneralData & srcPageData);
    //比较页面前后数据
    bool ComparePageData(const SoundGeneralData & beforePageData,
                         const SoundGeneralData & afterPageData) const;
    //取得当前页面数据
    SoundGeneralData & GetCurrentPageData(SoundGeneralData & currentPageData) const;
    //设置当前页面数据
    void SetCurrentPageData(SoundGeneralData & currentPageData);

private:
    //用于记录加载的数据和存放要保存的数据
    SoundGeneralData m_SoundGeneralData;

    //定义send Video combo box控件
    qCheckBox * m_qCheckBoxKeySoundOn;
};

#endif // DLGSOUNDGENERAL_H
