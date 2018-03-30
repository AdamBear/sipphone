#ifndef DLGDISPLAYWALLPAPER_H
#define DLGDISPLAYWALLPAPER_H

#include <QDialog>
#include "photomanager.h"
#include "cdlgbasesubpage.h"
#include "setting/include/common.h"
#include "setting/src/displaycontroller.h"
#include "uikernel/scrollpage.h"
#include "uikernel/quipushbutton.h"

#if IF_FEATURE_T48_DSSKEY_WALLPAPER
enum WALLPAPER_TYPE
{
    WALLPAPAER_NOMAL = 0,   //Idle界面壁纸
    WALLPAPAER_DSSKEY = 1,  //Dsskey展开壁纸
};
#endif

class CDlgDisplayWallpaper : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    CDlgDisplayWallpaper(QWidget * parent = 0);
    ~CDlgDisplayWallpaper();
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgDisplayWallpaper();
    }

public:
    //加载数据
    virtual bool LoadPageData();
    //获取数据状态是否改变
    virtual bool IsStatusChanged();
    //保存数据
    virtual bool SavePageData();

    yl::string GetCurrentPicture();

    // 设置子页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    void DeleteWallpapper();

    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

    // deal with left and right btn
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 窗口显示到界面上时的操作
    virtual void UpdateWnd();
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
    // 设置小窗口数据
    virtual void SetData(void * pData = NULL);
#endif

private slots:
    //延时显示图片
    void OnCmbCurrentIndexChanged(int nIndex);
    //左右箭头点击
    void ClickLeftArrow();
    void ClickRightArrow();

private:
    WallPaperData GetCurrentData();

    //初始化数据，设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    void SetCurrentPageData(WallPaperListData & currentPageData);

    virtual bool OnAction(const QString & strAction);

private:
    QLabel     *    m_pLabPreview;

    QComboBox   *   m_pCmbBoxWallpaper;

    qUIPushButton * m_pArrowLeft;            //箭头图标
    qUIPushButton * m_pArrowRight;
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
    WALLPAPER_TYPE  m_eType;
#endif
};

#endif // DLGDISPLAYWALLPAPER_H
