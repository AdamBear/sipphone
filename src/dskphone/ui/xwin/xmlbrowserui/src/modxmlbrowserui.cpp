#include "modxmlbrowserui.h"
#include "dlgxmlbrowser.h"

#include "uimanager/uimanager_inc.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "dsklog/log.h"
#include "uimanager/uimanager_common.h"
#include "uimanager/moduimanager.h"
#include "features-define.h"

#include "xmlbrowsermessagebox.h"
#include "xmlbrowseruimanager.h"


#ifdef IF_FEATURE_XMLBROWSER
/************************************************************************/
/* 接口  ： XmlBrowserUI_Init                                            */
/* 功能  ： 初始化XmlBrowser模块,实现窗体类注册                         */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void XmlBrowserUI_Init()
{
    REGISTER_DLG(CDlgXMLBrowser);
}

/************************************************************************/
/* 接口  ： XmlBrowserUI_UpdateWnd                                       */
/* 功能  ： 刷新窗口数据                                                */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void XmlBrowserUI_UpdateWnd()
{
    // 更新前先清提示框
    XmlBrowserUI_DestroyMessage();
    // 更新界面
    XB_UIManager.UpdateXMLBrowserWnd();
}

/************************************************************************/
/* 接口  ： XmlBrowserUI_ExitXMLDlg                                     */
/* 功能  ： 退出XmlBrowser的Dlg                                           */
/* 返回值 ： 无                                                         */
/************************************************************************/
void XmlBrowserUI_ExitXBDlg(int nElement, bool bUpdateWnd)
{
    XB_UIManager.ExitXMLBrowserDlg(nElement, bUpdateWnd);
    XmlBrowserUI_DestroyMessage();
}

/************************************************************************/
/* 接口  ： XmlBrowserUI_ShowMessage()                                 */
/* 功能  ： 弹出信息框                                                  */
/* 参数说明 ：strTitle 弹出框的Title                                   */
/*            strNote弹出框的内容                                     */
/*            uTimeOut 退出弹出框的时间                                 */
/* 返回值 ： 无                                                         */
/************************************************************************/
void XmlBrowserUI_ShowMessage(int eType, UINT uTimeOut/* = 3000*/)
{
    _XBMessageManager.ShowMessage((XB_TIP_TYPE)eType, uTimeOut);
}

/************************************************************************/
/* 接口  ： XmlBrowserUI_DestroyMessage()                               */
/* 功能  ： 销毁弹出框                                                  */
/* 参数说明 ：无                                                      */
/* 返回值 ： 无                                                         */
/************************************************************************/
void XmlBrowserUI_DestroyMessage()
{
    _XBMessageManager.DestroyMessage();
}

//获取xml弹出标记，其他模块使用
bool XmlBrowserUI_IsPopuping()
{
    if (NULL != UIManager_GetTopWindow()
            && eWT_XMLBrower == UIManager_GetTopWindow()->GetWindowType())
    {
        return true;
    }

    return false;
}
#else
/************************************************************************/
/* 接口  ： XmlBrowserUI_Init                                            */
/* 功能  ： 初始化XmlBrowser模块,实现窗体类注册                         */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void XmlBrowserUI_Init()
{

}

void XmlBrowserUI_UpdateWnd()
{

}

void XmlBrowserUI_ExitXBDlg(int nElement, bool bUpdateWnd)
{

}

void XmlBrowserUI_ShowMessage(XB_TIP_TYPE eType, UINT uTimeOut/* = 3000*/)
{

}

void XmlBrowserUI_DestroyMessage()
{

}

bool XmlBrowserUI_IsPopuping()
{
    return false;
}
#endif
