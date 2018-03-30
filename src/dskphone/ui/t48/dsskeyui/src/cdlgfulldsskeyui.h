#ifndef CDlgFullDsskeyUI_H
#define CDlgFullDsskeyUI_H

#include <QtGui/QDialog>
#include <ETLLib.hpp>
#include <vector>
#include "uimanager/basedialog.h"
#include "uicommon/uikernel/quipushbutton.h"
#include "dsskeyui/include/dsskeyuicommondefine.h"

// 前向声明
class qDsskeyFrame;
class CWndTemplate;
class qSoftButton;


class CDlgFullDsskeyUI : public CBaseDialog
{
    Q_OBJECT

public:
    CDlgFullDsskeyUI(QWidget * parent = 0);
    ~CDlgFullDsskeyUI();

    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();
    // 返回该窗口指针
    static CBaseDialog * CreateInstance();
    //获取dsskey frame
    qDsskeyFrame  * GetDssKeyFrame();
    //事件过滤器
    bool    eventFilter(QObject * pObject, QEvent * pEvent);
    // 获取界面的背景图片
    virtual QPixmap GetCustomBackGround();
    //http://bugfree.yealink.com/Bug.php?BugID=92722 通话的时候打开dsskeyframe不显示softkey
    virtual bool IsShowSoftkey()
    {
        return false;
    }

    virtual bool IsUseTransparency()
    {
        return true;
    }

private:
    // 初始化数据
    void InitData();
    //初始化Dsskey按钮
    void InitDsskeyFrame();

    // 更新界面背景
    void RefreshBackGroundPic();

    void DrawDsskeyBtnOnBackground();

public slots:
    // 点击了最后一个Dsskey按键
    void OnDsskeyBtnClick();

    void OnDsskeyBtnClick(int iIndex);

    bool OnTransparencyChange();

private:

    qDsskeyFrame  * m_pDsskeyFrame;

    // 背景
    QPixmap             m_pmBG;
    yl::string          m_strBGPicPath;
};



#endif // CDlgFullDsskeyUI_H
