#include "modmainwndhelp.h"
#include "titlebardelegate.h"
#include "dsskeyframedelegate.h"
#include "softkeybardelegate.h"
#include "homeandbackbtndelegate.h"
#include "autotesthelp.h"
#include "keyguardhelp.h"
#include "mainwndmessagehelp.h"
#include "mainwnd/mainwnd.h"

#include "messagebox/modmessagebox.h"

#include "uikernel/languagehelper.h"
#include "commonapi/stringhelperapi.h"
#include "include/translateiddefine.h"

#include "uicommon/qtcommon/qmisc.h"

#include "edk/include/modedk.h"
#include "edk/include/edk_def.h"

void MainWndHelp_Init()
{
    _MainWND.SetTitleDelegate(new CTitleBarDelegate());
    _MainWND.SetDsskeyDelegate(new CDsskeyFrameDelegate());
    _MainWND.SetSoftkeyDelegate(new CSoftKeyBarDelegate());
    _MainWND.SetHomeAndBackBtnDelegate(new CHomeAndBackBtnDelegate());

    //_MainWND.RegisterUpdateWndHook(AutoTestHelp);

    // 初始化键盘锁
    g_pKeyGuardHelper->KeyGuardHelp_Init();

    MainWndMessageHelp_Init();
}

void MainWndHelp_Uinit()
{
    MainWndMessageHelp_Uinit();
}

static void MainWndHelp_EDK_MessageBox_CallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    structEDKCallBack structCallBack = pMessageBox->GetExtraData().value<structEDKCallBack>();

    switch (pMessageBox->GetType())
    {
    case  MESSAGEBOX_EDK:
        {
            if (MessageBox_OK == pMessageBox->GetResult())
            {
                yl::string strValue = structCallBack.strInput.toUtf8().data();

                if (strValue.empty())
                {
                    MessageBox_ShowCommonMsgBox(WRAPPER_MSGBOX_CBFUN(MainWndHelp_EDK_MessageBox_CallBack)
                                                , LANG_TRANSLATE(TRID_EDK_PROMFT_EMPTY)
                                                , MESSAGEBOX_Cancel
                                                , 3000
                                                , ""
                                                , ""
                                                , pMessageBox->GetID()
                                                , QVariant::fromValue(structCallBack));
                }
                else
                {
                    // 输入正确，并反馈给逻辑层
                    if (NULL != structCallBack.pCallBackFun)
                    {
                        structCallBack.pCallBackFun(true, structCallBack.nID, strValue);
                    }
                }
            }
            else
            {
                if (NULL != structCallBack.pCallBackFun)
                {
                    yl::string strValue = "";
                    structCallBack.pCallBackFun(false, structCallBack.nID, strValue);
                }
            }
        }
        break;
    case MESSAGEBOX_Cancel:
        {
            MainWndHelp_EDK_Show_MessageBox(pMessageBox->GetID() - EDK_MESSAGEBOX_ID_BASE, structCallBack.nID,
                                            structCallBack.pCallBackFun);
        }
        break;
    default:
        {
            //
        }
        break;
    }
}

void MainWndHelp_EDK_Show_MessageBox(int iIndex, int nID, ActionCallBack pFun)
{
    if (!Edk_IsEnable())
    {
        return;
    }

    structEDKCallBack structCallBack;
    structCallBack.nID = nID;
    structCallBack.strInput = "";
    structCallBack.pCallBackFun = pFun;

    edk_prompt_data_t sPromptData;
    Edk_GetPromptByIndex(iIndex, sPromptData);

    MESSAGEBOX_DEFAULT_INPUT_TYPE iImeType;
    switch (sPromptData.m_eInputType)
    {
    case EDK_INPUT_TEXT:
        {
            iImeType = MessageBox_Input_Type_abc;
        }
        break;
    case EDK_INPUT_NUMERIC:
        {
            iImeType = MessageBox_Input_Type_123;
        }
        break;
    default:
        {
            iImeType = MessageBox_Input_Type_123;
        }
        break;
    }

    //弹出解锁框
    MessageBox_ShowEDKInputBox(WRAPPER_MSGBOX_CBFUN(MainWndHelp_EDK_MessageBox_CallBack)
                               , sPromptData.m_iMaxInput
                               , toQString(sPromptData.m_strLabel)
                               , iImeType
                               , sPromptData.m_bMasked
                               , EDK_MESSAGEBOX_ID_BASE + iIndex
                               , QVariant::fromValue(structCallBack));
}
