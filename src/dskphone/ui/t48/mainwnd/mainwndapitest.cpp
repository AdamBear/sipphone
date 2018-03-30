#include "mainwndapitest.h"
#include "mainwnd/mainwnd.h"

void MainWndAPITest()
{
//  void DisplayIMESoftkey(QWidget* pFocusWidget);
//  void SwitchNextIMEName(QWidget* pFocusWidget);
    // 检测异常情况：NULL
    // 界面没有IMEsoftkey：输入输入框控件、非输入框控件
    // 界面有IMEsoftkey：输入输入框控件、非输入框控件、其他界面的输入框控件

//  const CAutoVolumeBar* GetVolumeBar();
    // 没价值

//  void SetVolumeBarVisible(bool bVisible);
    // 没价值

//  void ShowRebootMessage(const QString& strText);
    // 普通字符、超长字符、空字符、需要翻译的

//  virtual bool eventFilter(QObject *pObject, QEvent *pEvent);
    // 异常：测试下空指针、测试下其他event、测试其他的object

//  void DrawSoftkey(CBaseDialog* pDialog);
    // 空指针，非顶层界面指针

//  virtual void PreReturnToIdle();
    // 没价值

//  void SetTitle(const QString& strTitle);
    // 普通字符、超长字符、空字符、需要翻译的
//  void SetTitleAccountDisplayName(const QString& strText);
    // 普通字符、超长字符、空字符、需要翻译的
//  void SetTitleAccountIcon(const QString& strIcon);
    // 存在的Icon、不存在的Icon

//  void SetTitleSelectAccountID(int nAccountID);
    // 设置正常/异常的AccountID
//  void SetTitleLayoutInfo(const TitleLayoutInfo& objTitleInfo);
    // 设置异常的Layout风格

//  QWidget* GetTitle() const;
    // 获取并判断是否为TitleBar
//  CMaskFrame* GetMaskFrame();
    // 没价值

//  bool ProcessVolumeBarKeyEvent(int nKeyCode, bool bPress);
    // 没价值

//  bool OnLineKeyClick(int iIndex);
    // 没价值

//  void UpdateBackGround();
    // idle/非idle界面调用该接口

//  void RegisterUpdateWndHook(ON_UPDATEWND_HOOK pFun);
//  void UnregisterUpdateWndHook(ON_UPDATEWND_HOOK pFun);
    // 正常：注册能否被调用到、能否正常注销
    // 异常：传入空指针、插入同样的pFun、删除不存在的fun
}
