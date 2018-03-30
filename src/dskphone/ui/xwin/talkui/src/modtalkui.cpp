#include "ylstring.h"
#include "talk/talklogic/include/modtalklogic.h"

//mod-base
#include "talkui_define.h"
#include "talkui/include/modtalkui.h"

#include "uimanager/basedialog.h"
#include "uimanager/moduimanager.h"

//mod-this(TalkUI)
#include "dlgtalkuiframe.h"
#include "dlgselectlineui.h"
//
////PickUpMenu
#include "dlgpickupmenu.h"
////SCAMenu
#include "dlgscamenu.h"

#include "dlgtalkpool.h"
#include "contacts/directory/include/moddirinterface.h"
#include "talkuicommonfunction.h"

#include "talkuiframemanager.h"

using namespace talkui;

/************************************************************************/
/* 接口  ： TalkUI_RegisterDlgClass()                                 */
/* 功能  ： 实现窗体类注册                                              */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void talkui_RegisterDlgClass()
{
    REGISTER_DLG(CDlgTalkUIFrame);
    REGISTER_DLG(CDlgSelectLineUI);
    REGISTER_DLG(CDlgPickupMenu);
    REGISTER_DLG(CDlgSCAMenu);
    REGISTER_DLG(CDlgTalkPool);

    //REGISTER_DLG(CTalkUITalking);
    //REGISTER_DLG(CTalkUIRing);
    //REGISTER_DLG(CTalkUIConnect);
    //REGISTER_DLG(CTalkUIFinish);
    //REGISTER_DLG(CTalkUIDial);
    //REGISTER_DLG(CTalkUIConf);
    //REGISTER_DLG(CTalkUIParkNotify);
    //REGISTER_DLG(CTalkUIAutoRedial);
    //REGISTER_DLG(CTalkUIRTPStatus);
    //REGISTER_DLG(CTalkUIACDCallInfo);
}

/************************************************************************/
/* 接口  ： talkui_Init()                                                */
/* 功能  ： talkUI模块初始化                                                */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                                            */
/************************************************************************/
void talkui_Init()
{
    talkui_RegisterDlgClass();
    _TalkUIFrameManager.Init();
}

/************************************************************************/
/* 接口  ： talkui_Uninit()                                              */
/* 功能  ： talkUI模块注销                                               */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                                            */
/************************************************************************/
void talkui_Uninit()
{
    _TalkUIFrameManager.Unint();
    talklogic_Uninit();
}

/************************************************************************/
/* 接口  ： talkui_SetVolumeBarVisible()                             */
/* 功能  ： 关闭开启音量条                                                */
/* 参数说明 ：[in] bool bVisible:是否开启关闭音量条                       */
/* 返回值 ： 无                                                            */
/************************************************************************/
//没有用
// void talkui_SetVolumeBarVisible(bool bVisible)
// {
//  // !!!!!,MainWnd提供C接口
// //   MainWnd_SetVolumeBarVisible(bVisible);
// }

/************************************************************************/
/* 接口  ： talkui_GetInputChar()                                        */
/* 功能  ： 获取赌赢输入法按键的第一个字符                                */
/* 参数说明 ：[in] string strIMEName:输入法的名称                        */
/* 参数说明 ：[in] int iKeyCode:按键的keyCode                           */
/* 返回值 ： 无                                                            */
/************************************************************************/
//没有用
// yl::string talkui_GetInputChar(const yl::string& strIMEName, int iKeyCode)
// {
//  return "";
//  //QString strFirstChar = qGetIMEFirstChar(strIMEName.c_str(), iKeyCode);
//  //return strFirstChar.toUtf8().data();
// }

///************************************************************************/
///* 接口  ： talkui_GetCameraMutePicture()                                      */
///* 功能  ： 获取本地视频mute时发送给对方的yuv格式的一帧图片                                */
///* 参数说明 ：[in] iResolutionType:获取的图片类型                     */
//
///* 返回值 ： 文件路径                                                         */
///************************************************************************/
//yl::string talkui_GetCameraMutePicture(int iResolutionType)
//{
//#ifndef _WIN32
//  if(!access(CAMERA_MUTE_SEND_YUV, F_OK))
//  {
//      return yl::string(CAMERA_MUTE_SEND_YUV);
//  }
//
//  //tipContent标题名称
//  //tmpImage_path 读取的需要转换的文件路径
//  //tipfn_path 转换后的YUV格式图片路径  需要输入
//  //转换成功 那么返回 YUV路径
//  if (AddTipContent(NULL, PIC_TALKUI_CAMERA_MUTE_SEND, CAMERA_MUTE_SEND_YUV))
//  {
//      return yl::string(CAMERA_MUTE_SEND_YUV);
//  }
//
//  return yl::string("");
//
//#else
//  return yl::string("");
//#endif
//
//}

/************************************************************************/
/* 接口  ： talkui_IsInTalk()                                        */
/* 功能  ： 是否顶层界面为通话界面        */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                    */
/************************************************************************/
bool talkui_IsInTalk()
{
    if (UIManager_IsInTalk())
    {
        return true;
    }

    return false;
}

/************************************************************************/
/* 接口  ： talkui_EnterPickupMenu()                                     */
/* 功能  ： 进入Pickup操作界面     */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                    */
/************************************************************************/
void talkui_EnterPickupMenu()
{
    CBaseDialog * pDlg = UIManager_GetPrivateDlg(DLG_DlgPickupMenu);
    if (pDlg == NULL)
    {
        pDlg = UIManager_CreatePrivateDlg(DLG_DlgPickupMenu);
    }

    if (pDlg != NULL)
    {
        CDlgPickupMenu * pDlgPickupMenu = (CDlgPickupMenu *)pDlg;

        pDlgPickupMenu->SetData(NULL);

        if (pDlg != UIManager_GetTopWindow())
        {
            UIManager_SetTopWindow(pDlg);
        }
        else
        {
            UIManager_UpdateWnd(pDlg);
        }
    }
}

/************************************************************************/
/* 接口  ： talkui_UpdatePickupMenu()                                        */
/* 功能  ： 更新Pickup操作界面数据       */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                */
/************************************************************************/
void talkui_UpdatePickupMenu()
{
    CBaseDialog * pDlg = UIManager_GetPrivateDlg(DLG_DlgPickupMenu);
    if (pDlg != NULL)
    {
        CDlgPickupMenu * pDlgPickupMenu = (CDlgPickupMenu *)pDlg;

        pDlgPickupMenu->SetData(NULL);


        if (pDlg != UIManager_GetTopWindow())
        {
            UIManager_SetTopWindow(pDlg);
        }
        else
        {
            UIManager_UpdateWnd(pDlg);
        }
    }
}

/************************************************************************/
/* 接口  ： talkui_ExitPickupMenu()                                      */
/* 功能  ：  退出Pickup操作界面    */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                    */
/************************************************************************/
void talkui_ExitPickupMenu()
{
    CDlgPickupMenu * pDlg = (CDlgPickupMenu *)UIManager_GetPrivateDlg(DLG_DlgPickupMenu);
    if (pDlg != NULL)
    {
        UIManager_PutPrivateDlg(pDlg);
    }
}

/************************************************************************/
/* 接口  ： talkui_OpenPool()                                        */
/* 功能  ：  进入选择界面  */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                    */
/************************************************************************/
bool talkui_OpenPool(bool bIsEnterFromIdle /*= false*/)
{
    //先判断列表内容，若只有一个，则直接进入
    DirShortcutListData objlistData;
    objlistData.m_nIndexFrom = 0;
    objlistData.m_nCountToGet = 100;

    if (!Dir_GetShortcutList(&objlistData))
    {
        return false;
    }

#if !IF_BUG_39755
    // http://10.2.1.199/Bug.php?BugID=92495
    if (!(ExistLocalDirectory(objlistData.m_listData) && Dir_IsDirConfiged(BLUETOOTH_DIR)))
#endif
    {
        if (objlistData.m_listData.size() == 1)
        {
            DirShortcutData objData = objlistData.m_listData.front();
            // 打开界面
            CDlgTalkPool::talkPool_OpenPage(objData.m_strIdName);
            return true;
        }
    }

    //Pool窗体应为顶层窗口
    CBaseDialog * pDialog = UIManager_GetPrivateDlg(DLG_TalkPool);

    if (NULL != pDialog)
    {
        UIManager_SetTopWindow(pDialog);
        return true;
    }

    pDialog = UIManager_CreatePrivateDlg(DLG_TalkPool);
    if (pDialog != NULL)
    {
        CDlgTalkPool * pDlgTalkPool = (CDlgTalkPool *)pDialog;
        pDlgTalkPool->SetData(NULL);
        pDlgTalkPool->setIsEnterFromIdle(bIsEnterFromIdle);

        UIManager_UpdateWnd(pDialog);

        pDlgTalkPool->ReloadFrameListData();

        return true;
    }
    return false;
}

/************************************************************************/
/* 接口  ： talkui_ExitPool()                                        */
/* 功能  ：  退出选择界面  */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                    */
/************************************************************************/
void talkui_ExitPool()
{
    UIManager_PutPrivateDlg(DLG_TalkPool);
}
/************************************************************************/
/* 接口  ： talkui_EnterSCAMenu()                                        */
/* 功能  ： 进入SCA操作界面        */
/* 参数说明 ：无                                                      */
/*              [in]pData:   传给界面的数据                   */
/* 返回值 ： 无                                    */
/************************************************************************/
bool talkui_EnterSCAMenu(void * pData)
{
#if TALKUI_DEBUG_TEST
    static ScaMenuData * data = new ScaMenuData();

    pData = data;

    static ListSCAInfo listSca;  //

    for (int i = 0; i < 10; i++)
    {
        SCA_Info * pSCA = new SCA_Info();

        pSCA->iIndex = 1;       // 该账号分线的Index,从1开始计数
        pSCA->iDsskeyIndex = 2;// Dsskey索引
        pSCA->strDisplay = "name";  // 如果账号分线处于通话状态,则该路通话的显示名
        pSCA->strURI = "10.2.1.48";     // 如果账号分线处于通话状态,则该路通话的URI

        pSCA->eStatus = CN_BLF_STATUS_IDLE;// 该账号分线的状态
        listSca.push_back(pSCA);
    }

    data->pScaInfoList = &listSca;
    data->iAccount = 3;     // 显示SCA状态的账号ID
    data->nReturnTime = 20000;  // 自动退出时间

#endif

    CBaseDialog * pDlg = UIManager_GetPrivateDlg(DLG_DlgSCAMenu);
    if (pDlg == NULL)
    {
        pDlg = UIManager_CreatePrivateDlg(DLG_DlgSCAMenu);
    }

    if (pDlg != NULL)
    {
        CDlgSCAMenu * pSCAMenu = (CDlgSCAMenu *)pDlg;
        //设置数据
        pSCAMenu->SetData(pData);

        if (pDlg != UIManager_GetTopWindow())
        {
            UIManager_SetTopWindow(pDlg);
        }
        else
        {
            UIManager_UpdateWnd(pDlg);
        }
    }
    else
    {
        return false;
    }

    return true;
}

/************************************************************************/
/* 接口  ： talkui_IsExistSCAMenu()                                      */
/* 功能  ： 是否存在SCA操作界面  */
/* 参数说明 ：无                                                          */
/* 返回值 ： true-存在                                    */
/************************************************************************/
bool talkui_IsExistSCAMenu()
{
    return (UIManager_GetPrivateDlg(DLG_DlgSCAMenu) != NULL);
}

/************************************************************************/
/* 接口  ： talkui_ExitSCAMenu()                                     */
/* 功能  ：  退出SCA操作界面   */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                    */
/************************************************************************/
void talkui_ExitSCAMenu()
{
    UIManager_PutPrivateDlg(DLG_DlgSCAMenu);
}

