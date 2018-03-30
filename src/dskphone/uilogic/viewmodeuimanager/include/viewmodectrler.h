#ifndef _VIEW_MODE_CTRLER_H_
#define _VIEW_MODE_CTRLER_H_

#include "viewmodeuidefine.h"
#include "common/common_data_define.h"
#include <ETLLib/ETLLib.hpp>

class CViewModeControler
{
public:
    virtual bool OnSoftKeyEvent(int iKey, int iDataID, const yl::string & strValue,
                                void * pData = NULL)
    {
        return false;
    }
    virtual bool OnSoftKeyEvent(const yl::string & strSoftkeyAction, int iDataID,
                                const yl::string & strValue, void * pData = NULL);
    virtual bool OnFunKeyEvent(int iKey, int iDataID, yl::string & strValue, void * pData = NULL)
    {
        return false;
    }
    virtual bool OnDigitKeyEvent(int iKey, int iDataID, yl::string & strValue, void * pData = NULL)
    {
        return false;
    }
    virtual bool OnOptionEvent(int iKey, int iDataID, const yl::string & strValue, void * pData = NULL)
    {
        return false;
    }
    virtual bool OnOptionEvent(const yl::string & strSoftkeyAction, int iDataID,
                               const yl::string & strValue, void * pData = NULL);
    virtual bool OnMsgBoxEvent(int iID, int iKey)
    {
        return false;
    }
    virtual yl::string GetTitle(int iUIType)
    {
        return "";
    }
    virtual bool GetDataList(int iUIType, int iPageIndex, VecViewInfo & vecInfo, int iStartPos = 0,
                             int iNumber = 911) = 0;
    virtual bool GetSoftkeyList(int iUIType, int iDataID, YLVector<int> & vecSoftkey) = 0;
    // 获取option softkey列表
    virtual bool GetOptionSoftkeyList(int iUIType, int iFocusIndex,
                                      YLVector<OPTION_TYPE> & vecOptionKey) = 0;
    // 获取数据列表大小
    virtual int  GetListSize(int iUIType, bool bOnlyData = false)
    {
        return 0;
    }
    // 获取data id在链表的位置
    virtual int  GetFocusInDataList(int iDataID)
    {
        return 0;
    }
    // 获取图片信息
    // [strPic]:图片名称前缀 [uNum]:图片数量 [uTime]:变换时间
    virtual bool GetAnimationInfo(AnimationType eType, yl::string & strPic, UINT & uNum, UINT & uTime)
    {
        return false;
    };
    // 处理页面退出后的业务
    virtual bool PageExit(int iUIType)
    {
        return false;
    };
    // 是否需要显示动画
    virtual bool IsNeedShowAnimation(int iUIType, AnimationType eType)
    {
        return false;
    };
    // 校正数据
    virtual bool CorrectViewInfo(int iUIType, const VecItemDataID2Info & vecItemDataInfo,
                                 VecViewInfo & vecInfo)
    {
        return false;
    }

protected:
    // 进入页面
    bool EnterPage(int iUIType, int iLastDataID = DATA_ID_INVALID);
    // 退出页面
    bool ExitPage();

protected:
    int                 m_iLastFocusDataID;         // 缓存上一页焦点data id
};

#endif //_VIEW_MODE_CTRLER_H_
