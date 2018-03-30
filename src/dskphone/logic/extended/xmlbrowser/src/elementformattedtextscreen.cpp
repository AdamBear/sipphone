#include "logic_inc.h"
#include "dsskey/src/dsskeyactionmanager.h"
#include "dsskey/src/dsskeyaction.h"

#ifdef IF_FEATURE_XMLBROWSER

CElementFormattedText::CElementFormattedText()
{
}

CElementFormattedText::~CElementFormattedText()
{
}

// 创建之后的处理
bool CElementFormattedText::AfterCreate()
{
    return CElementHaveUIBase::AfterCreate();
}

// 销毁之前的处理
void CElementFormattedText::BeforeDestroy()
{

}

// 是否播放提示音
bool CElementFormattedText::IsNeedPlayBeep()
{
    return _GetXData().stAttr.bBeep;
}

// softkey处理
bool CElementFormattedText::ProcessSoftkey(XB_SOFTKEY_TYPE eType, int iFocusedItem, bool & bUpdate)
{
    bool bHandled = false;
    bUpdate = false;
    return bHandled;
}

// 对UI事件的处理
bool CElementFormattedText::OnAction(XB_ACTION_TYPE eAction, void * pData, bool & bExitWnd,
                                     bool & bUpdate, bool bDestroyOnExit/* = false*/)
{
    bool bHandled = false;
    switch (eAction)
    {
    case XB_ACTION_CONFIRM:
        {
            // doneAction
            bUpdate = _GetXData().stAttr.bDestroyOnExit;
            ProcessLegalURL(_GetXData().stAttr.strDoneAction, _GetXData().stAttr.bDestroyOnExit);
            bHandled = true;
        }
        break;
    case XB_ACTION_LINEKEY_CALLOUT:
        {
            // 上锁时不响应，没上锁进入预拨号
            if (_GetXData().stAttr.bLockIn)
            {
                return true;
            }
            else
            {
                XDataUI2Logic * pUIData = (XDataUI2Logic *) pData;
                CHECK_NULL(pUIData, false);
                if (DT_LINE == dsskey_GetDsskeyType(pUIData->wActionParam))
                {
                    _XMLBrowserManager.Exit();
                    return false;
                }
                //return true;
            }
        }
        break;
    case XB_ACTION_CANCEL:
        {
            // 如果cancel值不为空，执行URL，为空退出界面
            if (!_GetXData().stAttr.strCancelAction.empty())
            {
                // cancelAction:执行cancel的URL
                bUpdate = _GetXData().stAttr.bDestroyOnExit;
                ProcessLegalURL(_GetXData().stAttr.strCancelAction, _GetXData().stAttr.bDestroyOnExit);
                bHandled = true;
            }
        }
        break;
    default:
        break;
    }

    // 其他事件交给基类处理
    if (!bHandled)
    {
        bHandled = CElementHaveUIBase::OnAction(eAction, pData, bExitWnd, bUpdate,
                                                _GetXData().stAttr.bDestroyOnExit);
    }

    return bHandled;
}

#endif // #ifdef IF_FEATURE_XMLBROWSER

