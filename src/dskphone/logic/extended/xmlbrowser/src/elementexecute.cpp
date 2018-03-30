#include "logic_inc.h"

#ifdef IF_FEATURE_XMLBROWSER

CElementExecute::CElementExecute()
{
}

CElementExecute::~CElementExecute()
{
}

// 创建之后的处理
bool CElementExecute::AfterCreate()
{
    // 取消提示框，否则会影响按键命令的生效
    _XMLBrowserManager.ExitMessageBox();

    // 播放提示音
    if (IsNeedPlayBeep())
    {
        _XMLBrowserManager.PlayBeep();
    }

    XMLBROWSER_INFO("loop execute commands. number=[%d]", _GetXData().listItem.size());
    // 遍历ItemList，取出每个URI给URIProcesser处理
    List_Execute_Item::iterator iter = _GetXData().listItem.begin();
    for (; iter != _GetXData().listItem.end(); ++iter)
    {
        if ((*iter) == NULL)
        {
            continue;
        }
        ExecuteItem * pExecuteItem = (*iter);
        _URIProcesser.ProcessURI(pExecuteItem->strURI);
    }
    return true;
}

// 销毁之前的处理
void CElementExecute::BeforeDestroy()
{

}

// 是否播放提示音
bool CElementExecute::IsNeedPlayBeep()
{
    return _GetXData().stAttr.bBeep;
}

#endif // #ifdef IF_FEATURE_XMLBROWSER
