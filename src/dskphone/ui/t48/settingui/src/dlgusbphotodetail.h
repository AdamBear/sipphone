#ifndef DLGUSBPHOTODETAIL_H
#define DLGUSBPHOTODETAIL_H

#include <QDialog>
#include "photomanager.h"
#include "cdlgbasesubpage.h"
#include "setting/include/common.h"
#include "setting/src/displaycontroller.h"
#include "uikernel/scrollpage.h"
#include "uikernel/quipushbutton.h"
#include "record/include/modrecord.h"

class CDlgUSBPhotoDetail : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    CDlgUSBPhotoDetail(QWidget * parent = 0);
    ~CDlgUSBPhotoDetail();
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgUSBPhotoDetail();
    }

public:
    //加载数据
    virtual bool LoadPageData();
    //获取数据状态是否改变
    virtual bool IsStatusChanged();
    //保存数据
    virtual bool SavePageData()
    {
        return true;
    }

    yl::string GetCurrentPicture();

    // 设置子页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    void DeleteWallpapper();

    // deal with left and right btn
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 窗口显示到界面上时的操作
    virtual void UpdateWnd();

    //加载图片
    void OnLoadPicture();

    //加载图片预处理
    bool ToleranceLoadImage(QPixmap & pix);

private slots:
    //延时显示图片
    void OnCmbCurrentIndexChanged(int nIndex);
    //左右箭头点击
    void ClickLeftArrow();
    void ClickRightArrow();

private:
    RecordFileInfo GetCurrentData();

    //初始化数据，设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    void SetCurrentPageData(YLVector<RecordFileInfo> & currentPageData);

    virtual bool OnAction(const QString & strAction);

private:
    QLabel     *    m_pLabPreview;

    QComboBox   *   m_pCmbBoxWallpaper;

    qUIPushButton * m_pArrowLeft;            //箭头图标
    qUIPushButton * m_pArrowRight;
    QString    m_strError;
    bool       m_bPictureAvailble;
};

#endif // DLGUSBPHOTODETAIL_H
