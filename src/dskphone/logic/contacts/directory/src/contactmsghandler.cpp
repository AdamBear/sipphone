#include "contactmsghandler.h"
#include "directorymessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include <interfacedefine.h>
#include "xportmanager.h"

#include "broadsoftcontactmanager.h"
#include "broadsoftlogmanager.h"
#include "directoryshortcut.h"
#include "localcontactmanager.h"
#include "remotecontactmanager.h"
#include "locallogmanager.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "bluetooth/btkit_api.h"
#include "extended/bluetoothmessagedefine.h"
#include "bluetoothcontactmanager.h"
#include "dirlogicsync.h"
#include "service_system.h"

#include "metaswitchcontactmanager.h"
#include "mtswlogmanager.h"

#include "moddirinterface.h"

#if IF_FEATURE_GBBOOK
#include "genbandcontactmanager.h"
#endif

#if IF_FEATURE_METASWITCH
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#include "metaswitch/mtswcontrol/include/mtsw_messagedefine.h"
#endif


#include "plcmcontactmanager.h"

namespace NS_DIRECTORY
{
LRESULT OnDirectoryMessage(msgObject & refObj)
{
    DIR_INFO("Directory: message(0x%x), wparam(0x%x), lparam(0x%x)", refObj.message, refObj.wParam,
             refObj.lParam);
    if (refObj.message == DSK_MSG_GET_CONTACTCOUNT)
    {
        int iHistory = Dir_GetContactCount(refObj.lParam, DIR_TYPE(refObj.wParam));
        refObj.ReplyMessage(iHistory);
        return 0;
    }
    else if (refObj.message == DSK_MSG_DEl_LOCALLOG)
    {
        LocalLog_ClearLogs(refObj.wParam);
    }
    else if (refObj.message == DSK_MSG_SAVE_CALLLOG)
    {
        g_pLocalLogManager->SaveToFile();

        //回复网页
        refObj.ReplyMessage(0);

        return 0;
    }
#if IF_FEATURE_GBBOOK
    else if (refObj.message == SIP_SUB_ADDRESS_BOOK_NOTIFY)
    {
        if (g_pGenBandDirManager->GetIsLoaclChangeDir())
        {
            g_pGenBandDirManager->SetIsLocalChangeDir(false);
        }
        else
        {
            g_pGenBandDirManager->UpdatePabData();
            etl_NotifyApp(false, GENBAND_BOOK_RELOAD, 0, 0);
        }
    }
#endif
    else if (refObj.message == DSK_MSG_GET_CONTACT_UPDATESTATS)
    {
        refObj.ReplyMessage(g_pLocalDirManager->GetContactUpdateMark());

        if (0 != g_pLocalDirManager->GetContactUpdateMark())
        {
            g_pLocalDirManager->SetContactUpdateMark(0);
        }

        return 0;
    }
#ifdef IF_BT_SUPPORT_PHONE
    else if (refObj.message == BLUETOOTH_MSG_DIR_DONE)
    {
        if (refObj.wParam == OP_DOWNLOAD_RESULT)
        {
            //lparam 0: 下载失败/ 1： 下载成功 extra=filepath/ 2：下载中断开
            int iResult = refObj.lParam ;
            //下载成功
            if (refObj.lParam == 1
                    && NULL != refObj.GetExtraData())
            {
                yl::string strFile = (char *)refObj.GetExtraData();

                iResult = g_pBlueToothDirManager->ReLoadContact(strFile, refObj.wParam) ? 1 : 0;
            }

            etl_NotifyApp(false, BLUETOOTH_CONTACT_DOWNLOAD_RESULT, iResult, 0);
        }
        else if (refObj.wParam == OP_DISCONNECT_RESULT
                 && refObj.lParam == true)
        {
            g_pBlueToothDirManager->ProcessDisconnect();
        }
        else if (refObj.wParam == OP_CONNECT_RESULT
                 && refObj.lParam == true)
        {
            g_pBlueToothDirManager->ProcessConnected();
        }
        return 0;
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (refObj.message == MTSW_MESSAGE_LOGIN)
    {
        if (MtswDir_IsEnable())
        {
            //etl_NotifyApp(false, METASWITCH_PHONEBOOK_RELOAD, 0, 0);
            g_pMetaSwitchDirManager->PreLoad();
        }

        if (MTSWLog_IsEnable())
        {
            g_pMTSWLogManager->ReqLoadCallLog();
        }
    }
    else if (refObj.message == MTSW_MESSAGE_AUTH_ERR)
    {
        g_pMetaSwitchDirManager->PreLoad();
        g_pMTSWLogManager->PreLoad();
    }
#endif
    if (refObj.message == SYS_MSG_NOTIFY_PHY_STATUS
            || refObj.message == SYS_MSG_NOTIFY_NETWORK_STATUS
            || refObj.message == ATP_MSG_NOTIFY_AUTOP_END)
    {
        return g_pDirLogicSync->OnMessage(refObj);
    }

    //网页返回0表示成功
    LRESULT isSucceed = 0;

    bool isNeedReply = true;

    switch (refObj.wParam)
    {
    case ST_CONTACTS:

        g_pDirLogicSync->OnMessage(refObj);
        // type: 1 为重新加载组信息  2表示重新加载联系人信息
        g_pLocalDirManager->ReloadContact();

        //标记让网页同步联系人和组，考虑autop的情况
        g_pLocalDirManager->SetContactUpdateMark(3);

        // 提示UI重新加载组和成员.
        etl_NotifyApp(false, LOCAL_DIRECTORY_RELOAD, 0, 0);

        break;
    case ST_XPORT_CONTACT:

        DIR_INFO("--Local contact: begin to xport contact. (%d)", (int)refObj.lParam);

        // 防止超级搜索时，以下操作会去删除联系人记录
        //g_pLocalDirProcessor->m_cs.Lock();
        if (refObj.lParam == 1)
        {
            isSucceed = XPortFactory::ExportToPath();
            DIR_INFO("--Local contact: exported result (%d)", (int)isSucceed);
        }
        else
        {
            bool isAppend = (refObj.lParam == 2);

            isSucceed = XPortFactory::ImportByPath(
                            NULL,  // 默认路径.
                            isAppend  // 覆盖
                        );
            DIR_INFO("--Local contact: imported result (%d)", (int)isSucceed);
        }

        //g_pLocalDirProcessor->m_cs.Unlock();
        if (isSucceed == 0 && refObj.lParam != 1)
        {
            // 提示UI重新加载组和成员.
            etl_NotifyApp(false, LOCAL_DIRECTORY_RELOAD, 0, 0);
        }
        break;
    case ST_XPORT_BLACKLIST:
        {
#define kszXportBlackList   CONFIG_DATA_PATH "blacklist.xml"
            DIR_INFO("--Local contact: begin to xport blacklist. (%d)", (int)refObj.lParam);
            if (0 == refObj.lParam)
            {
                isSucceed = XPortFactory::ImportByPath(kszXportBlackList, false);
            }
            DIR_INFO("--Local contact: imported result (%d)", (int)isSucceed);
        }
        break;

#if (20 != PHONE_TYPE)
#ifdef IF_FEATURE_BROADSOFT_CONTACT
    case ST_XSI_CONFIGE:
        {
            if (refObj.lParam == 0)
            {
                //误去除
                g_pBroadSoftDirManager->ClearCustomList(true);

                int * nType = (int *)refObj.GetExtraData();
                //refObj.wParam == 1表示自己发出此消息
                if (nType == NULL || *nType != XSI_DIR_SEND)
                {
                    g_pBroadSoftDirManager->PreLoad();

                    etl_NotifyApp(false, BROADSOFT_PHONEBOOK_RELOAD, 0, 0);
                }

                if (nType == NULL || *nType != XSI_CALLLOD_SEND)
                {
                    g_pBroadSoftLogManager->ReloadConfig();
                    etl_NotifyApp(false, BROADSOFT_CALLLOG_RELOAD, 0, 0);
                }
            }
        }
        break;

    case ST_BS_PHONEBOOK:
    case ST_CUSTOM_COFIG:
        {
            if (!BsftDir_UpdateEnable()
                    && ST_CUSTOM_COFIG == refObj.wParam
                    && 1 == refObj.lParam)
            {
                g_pBroadSoftDirManager->ReqLoadContact(kszCustomId);
                // 请求custom联系人会发消息给UI,这是网页请求消息
            }
            else
            {
                // BSFT 联系人配置改变需要重新加载
                g_pBroadSoftDirManager->ClearCustomList();
                g_pBroadSoftDirManager->PreLoad();
                etl_NotifyApp(false, BROADSOFT_PHONEBOOK_RELOAD, 0, 0);
            }
        }
        break;
#endif //IF_FEATURE_BROADSOFT_CONTACT
#if IF_FEATURE_METASWITCH_DIRECTORY
    case ST_MTSW_DIR_CONFIG:
        {
            etl_NotifyApp(false, METASWITCH_PHONEBOOK_RELOAD, 0, 0);
            g_pMetaSwitchDirManager->PreLoad();
        }
        break;
    case ST_MTSW_CONFIG:
        {
            etl_NotifyApp(false, METASWITCH_PHONEBOOK_RELOAD, 0, 0);
            g_pMetaSwitchDirManager->PreLoad();

            g_pMTSWLogManager->PreLoad();
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    case ST_MTSW_LOG_CONFIG:
        {
            g_pMTSWLogManager->PreLoad();

            if (refObj.lParam == 1)
            {
                if (!LocalLog_IsEnable())
                {
                    g_pLocalLogManager->ClearLogs(kszAllContactId);
                }
                else
                {
                    g_pLocalLogManager->Init();
                }
            }
        }
        break;
#endif
    case ST_REMOTE_PHONEBOOK:
        {
            // ０表示URL配置有更改,1表示自动更新配置有更改
            if (0 == refObj.lParam)
            {
                // 远程联系人配置更新.
                DIR_INFO("Begin to reload romote config file!");
                g_pRemoteDirManager->ReloadContact();

                // 提示UI重新加载组和成员.
                etl_NotifyApp(false, REMOTE_PHONEBOOK_RELOAD, 0, 0);

                g_pDirectoryShortcut->ReloadConfigure();
                etl_NotifyApp(false, DIRECTORY_SHORTCUT_RELOAD, 0, 0);
            }
            else if (1 == refObj.lParam)
            {
                DIR_INFO("------------remote update change");
                g_pRemoteDirManager->SetAutoUpdateTimer();
            }
        }
        break;
    case ST_LDAP:
        //case ST_SUPERSEARCH:
        // 网页上超级搜索和通话快捷菜单公用一个按钮, 所以此处需要处理该消息.
        DIR_INFO("OnDirectoryMessage Handle ST_SUPERSEARCH");

        g_pDirectoryShortcut->ReloadConfigure();
        etl_NotifyApp(false, DIRECTORY_SHORTCUT_RELOAD, 0, 0);
        break;
#endif
    case ST_BLACKLIST:
        // 黑名单显示开关状态改变, 通知UI重新加载数据.
        DIR_INFO("--Contact, Black list show changed, begin to post message.");
        etl_NotifyApp(false, LOCAL_DIRECTORY_RELOAD, 0, 0);
        break;

    case ST_BS_CALLLOG:
        {
            g_pBroadSoftLogManager->ReloadConfig();

            etl_NotifyApp(false, BROADSOFT_CALLLOG_RELOAD, 0, 0);
        }
        break;

    case ST_CALLLOG_CLEAN:
        {
            CALLLOG_INFO("CCallLogController::OnMessage [CONFIG_MSG_BCAST_CHANGED] wParam[0x%x]",
                         (unsigned int)refObj.wParam);

            // 此处处理AutoP升级消息. AutoP发送此消息时, 已经清空了
            // 历史记录配置文件, 此时需要同步请空内存中的历史记录.
            g_pLocalLogManager->ClearLogs(kszAllContactId);
        }
        break;

    case ST_FAVORITE_CONFIG:
        {
            DIR_INFO("OnDirectoryMessage ST_FAVORITE_CONFIG");
            g_pDirectoryShortcut->ReloadConfigure();
            etl_NotifyApp(false, DIRECTORY_SHORTCUT_RELOAD, 0, 0);
        }
        break;

#if IF_BUG_PLCMDIR
    case ST_PLCMDIR_CONFIG:
        {
            DIR_INFO("OPLCMDIR Config change!");

            //0重新下载
            if (refObj.lParam == 0)
            {
                g_pPlcmDirManager->ReloadContact();
            }
            else//1设置定时更新时间
            {
                g_pPlcmDirManager->SetAutoUpdateTimer();
            }
        }
        break;
#endif
#if IF_FEATURE_GBBOOK
    case ST_GB_BOOK:
        {
            g_pGenBandDirManager->ReloadData();
            etl_NotifyApp(false, GENBAND_BOOK_RELOAD, 0, 0);
        }
        break;
#endif
#if IF_FEATURE_MUTI_CALLLOG
    case ST_SIPLINE:
        {
            if (BsftLog_IsMutilLogEnable())
            {
                g_pBroadSoftLogManager->ReloadConfig();
            }
        }
        break;
#endif
    case ST_LOCAL_CALLLOG_SYNC:
        {
            if (refObj.lParam == 1)
            {
                //开关从开到关，立即上传一次
                if (!g_pDirLogicSync->IsLocalLogSyncEnable())
                {
                    etl_NotifyApp(false, LOCAL_CALLLOG_UPLOAD, DEF_UIIID, 1);
                }
            }
        }
        break;
    //改变显示模式重新排序
    case ST_BS_DIR_MODE:
        {
            g_pBroadSoftDirManager->SortAllData();
        }
        break;
    case ST_CALLLOG_IMPORT:
        {
            g_pLocalLogManager->ImportCalllog();
        }
        break;
    default:
        isNeedReply = false;
        break;
    }

    if (isNeedReply)
    {
        //回复网页
        refObj.ReplyMessage(isSucceed);
    }

    DIR_INFO("Directory: -------------- refObj.lResult: %d", refObj.lResult);

    return isSucceed;
}
}  // namespace NS_DIRECTORY
