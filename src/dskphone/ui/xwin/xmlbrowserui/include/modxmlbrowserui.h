#ifndef MOD_XMLBROWSER_UI_H
#define MOD_XMLBROWSER_UI_H

#include "extended/xmlbrowser/include/logicanduicommon.h"

/************************************************************************/
/* 接口  ： XmlBrowserUI_Init                                            */
/* 功能  ： 初始化XmlBrowser模块,实现窗体类注册                         */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void XmlBrowserUI_Init();

/************************************************************************/
/* 接口  ： XmlBrowserUI_UpdateWnd                                       */
/* 功能  ： 刷新窗口数据                                                */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void XmlBrowserUI_UpdateWnd();

/************************************************************************/
/* 接口  ： XmlBrowserUI_ExitXMLDlg                                     */
/* 功能  ： 退出XmlBrowser的指定对象的UI，
            如果nElement:INVALID_ELEMENT_ID，则退出所有UI               */
/* 参数  ： nElement [in] 退出关联的对象ID                                */
/* 返回值 ： 无                                                         */
/************************************************************************/
void XmlBrowserUI_ExitXBDlg(int nElement, bool bUpdateWnd);

/************************************************************************/
/* 接口  ： XmlBrowserUI_ShowMessage()                                  */
/* 功能  ： 弹出信息框                                                  */
/* 参数说明 ：strTitle 弹出框的Title                                   */
/*            strNote弹出框的内容                                     */
/*            uTimeOut 退出弹出框的时间                                 */
/* 返回值 ： 无                                                         */
/************************************************************************/
void XmlBrowserUI_ShowMessage(int  eType, UINT uTimeOut = 3000);

/************************************************************************/
/* 接口  ： XmlBrowserUI_DestroyMessage()                               */
/* 功能  ： 销毁弹出框                                                  */
/* 参数说明 ：无                                                      */
/* 返回值 ： 无                                                         */
/************************************************************************/
void XmlBrowserUI_DestroyMessage();

//获取xml弹出标记，其他模块使用
bool XmlBrowserUI_IsPopuping();

#endif // MOD_XMLBROWSER_UI_H
