#ifndef __DSSKEYUILOGIC_H__
#define __DSSKEYUILOGIC_H__
#include "dsskeyui/dsskeyuicommondefine.h"
#include <yllist.h>
#include <ylvector.h>
//#include "dsskeyframe.h"

struct SLightMeta
{
    int m_iDssKeyID;
    int m_iPriority;
    DSS_KEY_COLOR_TYPE m_eColorType;
    DSS_KEY_FLUSH_TYPE m_eFlushType;
};

struct FLOAT_INFO
{
    int iDsskeyID;// 发起float的dsskey
    int iPage;// float back的页面
};

typedef YLVector<DsskeyDisplayData> vec_DsskeyDispData;
typedef YLVector<DsskeyDisplayData *> vec_pDsskeyDispData;

class CDssKeyUILogic
{
public:
    static CDssKeyUILogic & GetInstance();
    // 初始化界面数据
    void Init();
    // 释放界面数据
    void Uninit();
    // 刷新界面数据
    bool UpdateLineKeyStatus(DssKeyLogicData & objData);
    // 刷新多个连续的Dsskey数据
    void RefreshMultipleDsskeyData(int iBegin, int num);
    // 将第iIndex个button锁定为iDsskeyIndex
    bool LockButton(int iDsskeyID);
    // 解锁指定的Dsskey对应的Button
    bool UnLockButton(int iDsskeyID);
    // Float到指定dsskey所在的页
    bool FloatByDsskey(int iDsskeyID);
    // 刷新翻页键
    void RefreshPageKey();
    // 点击pagekey
    BOOL OnPageButtonPress();
    // 获取DsskeyID
    int GetDsskeyID(int nBtnIndex);
    // 切换page提示功能开关
    void SwitchPageTip(BOOL bEnable);
    // 判断长按键是否可用
    BOOL ExtendedKeyEnable(int iKey);
    // 将linkey按键index根据当前页转换成逻辑index
    int HardwareKeyToLogicIndex(int iButtonIndex);
    // 是否显示PageButton
    BOOL IsPageButtonEnable()
    {
        return m_iPageTotal > 1;
    };
    void RefreshDssKeyUI(BOOL bForce = FALSE);
    // 指定刷新某个Dsskey
    void RefreshDssKeyBtn(int index, BOOL bForce = FALSE);
    void Forbid(BOOL bForbidden);

    // 获取每页Dsskey个数
    int GetNumPerPage();

    void UpdateDsskeyLight(int iIndex, DSS_KEY_COLOR_TYPE eColorType, DSS_KEY_FLUSH_TYPE eFlushType);
    void SetMaxNumPerPage(int iMaxNumPerPage);
    int GetMaxNumPerPage() const;
    void SetMaxPage(int iMaxPage);
    void SetMaxNumber(int iMaxNumber);
    int GetMaxNumber() const;
    // 是否界面上的Dsskey
    bool IsDsskey(int iKey);

    // 添加可控制的Dsskey显示模块控制指针
    //void AddFrameHandle(qDsskeyFrame * pFrame);

    void UpdatePageMode();

    DSS_KEY_COLOR_TYPE GetColorType(const LIGHT_Type eLightType);
    DSS_KEY_FLUSH_TYPE GetFlushType(const LIGHT_Type eLightType);
private:
    //回退到float前的页页
    bool RollBackToBeforeFloatPage(int iDsskeyID);
    //显示iPage页,iPage从0开始
    void GoToPage(int iPage);
    // 计算当前每页包含的linekey数量,页数
    void UpdatePageInfo();
    // 更新page状态
    void UpdatePageStatus(DsskeyDisplayData & objDisplayData);
    // 将dsskey状态映射成page状态
    void MapToPageStatus(DsskeyDisplayData & outPageData, const DsskeyDisplayData & objDisplayData);
    // 刷新灯状态
    void RefreshLineKeyLight();
    // 重新更新当前要显示的页码数
    void UpdatePageNum();
private:
    // 保存各个linekey状态
    vec_DsskeyDispData  m_vecDsskeyInfos;
    // 保存当前需要显示的LineKey
    vec_pDsskeyDispData m_vecCurrentShow;
    // 用于保存pagebutton的信息
    vec_DsskeyDispData  m_vecPageButtonInfo;
    YLList<FLOAT_INFO>  m_listFloatInfo;
    // 当前页,从0开始
    int                 m_iCurrentPageIndex;
    int
    m_iLinenumPerPage;  // 每一页包含的linkey数,当只有前10个有效Dsskey时,一页有10个，否则为9个
    int                 m_iCurPageNum;      // 当前显示页码个数
    int                 m_iMaxPage;         // 可显示的页码最大个数
    int                 m_iMaxNumber;       // 当前Dsskey总数量
    int                 m_iPageTotal;       // 当前Dsskey数量需要的页码总数
    int                 m_iMaxNumPerPage;   // 每一页包含的最大linkey数
    unsigned int        m_iUpdateCount;     // 刷新Dsskey次数

    int                 m_iPageIndex;       // 翻页键对应的索引
private:
    CDssKeyUILogic();
    ~CDssKeyUILogic();
    BOOL m_bEnablePageTips;
    BOOL m_bForbidden;
    BOOL m_bFullPageMode;
};

#define _DssKeyUILogic CDssKeyUILogic::GetInstance()
#endif
