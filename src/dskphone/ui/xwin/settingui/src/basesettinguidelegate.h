#ifndef __BASE_SETTINGUI_DELEGATE_H__
#define __BASE_SETTINGUI_DELEGATE_H__

#include <ylsmartptr.h>
#include "ylstl/ylstring.h"
#include "uimanager/dialogviewcontroller.h"
#include "settinguilogic/src/settinguidatavalidinfo.h"
#include "xwindow/xGraphic.h"

class CSettingUIPageDataBase;
class CSettingUIItemDataBase;
class SettingDynamicConfigFilter;
class SettingDynamicItem;
class CSettingUILineEditItemData;


enum ITEM_INVALID_DATA_TYPE
{
    ITEM_INVALID_DATA_TYPE_NONE = 0,
    ITEM_INVALID_DATA_TYPE_EMPTY,
    ITEM_INVALID_DATA_TYPE_RANGE,
    ITEM_INVALID_DATA_TYPE_FORMAT,
};

class CSettingUIDelegateBase
{
public:
    CSettingUIDelegateBase();
    virtual ~CSettingUIDelegateBase();

    virtual void Init();
    virtual void UnInit();

    // 加载配置数据
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    // 从配置中重新加载单个Item数据
    virtual void LoadItemData(int nDataIndex);
    // 界面数据过滤
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    // 保存配置数据
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    //保存数据时的额外操作
    virtual void SaveExtraData();
    // 点击Back事件
    virtual bool OnBackAction();
    // 回退到Idle
    virtual bool OnBackToIdle();
    // 回退时是否检测数据修改
    virtual bool IsCheckDataWhileDropOutPage()
    {
        return false;
    }
    // 更新界面数据
    virtual void UpdatePagedata();

    // 绘制操作
public:
    virtual bool PaintContent(xPainter & p)
    {
        return false;
    }
    virtual bool FocusEvent(bool bFocusIn)
    {
        return false;
    }

    //////////////////// 事件、Softkey、按键相关 //////////////////////
public:
    //界面按键统一操作
    virtual bool OnPageAction(int eActionType, int iDataIndex);
    //界面Item统一操作
    virtual bool OnAction(int eActionType, int iDataIndex);
    virtual bool ProcessAction(const yl::string strAction, int iDataIndex);
    // 处理Softkey
    virtual bool IsSoftKeyDisplay()
    {
        return true;
    }
    void GetPageXmlSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    // 处理按键事件
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);
    // 打开界面附加操作
    virtual void InitExtraData(void * pData);
    // 处理弹出框回调
    virtual bool ProcessMsgCallBack(int nResult, int nID, void * pExtraData = NULL);
    // 当前状态是否可被移除
    virtual bool IsCurStatusRemovable()
    {
        return true;
    }
    // 创建后是否设置为活动窗口
    virtual bool IsSetActiveWndWhileCreate();
    //打开界面
    virtual void OnEnterPage(const yl::string & strPage, int iDataIndex);

    //////////////////// 公共界面属性 //////////////////////
public:
    // 是否跳过Return To Idle，包括通话、Idle
    virtual bool IsRejectReturnToIdle();
    //回到Idle前的处理函数
    virtual void OnReturnToIdle();

    ////////////////////音量条///////////////////////
    // 获取当前页面音量条的数据
    // 返回值：是否显示音量条,默认不显示音量条
    // iVolumeType | [out] 音量条的类型
    // iMin | [out] 音量条的最小值
    // iMax | [out] 音量条的最大值
    // strBGPath | [out] 音量条的背景图片
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax, yl::string & strPix)
    {
        return false;
    }

    //////////////////// Setting界面属性 //////////////////////
public:
    // 设置、获取界面显示状态
    void SetShowStatus(bool bShow)
    {
        m_bShow = bShow;
    }
    bool GetShowStatus()
    {
        return m_bShow;
    }
    // 设置、获取界面焦点项序号
    virtual int GetFocusIndex();
    virtual void SetFocusIndex(int nFocusIndex);
    // 通过itemID设置界面焦点
    virtual void SetFocusIndexByItemID(const yl::string& strItemID);

    //获取、设置界面起始项下标
    int GetPageDataIndex();
    void SetPageDataIndex(int nPageDataIndex);

    // 设置、获取界面ID
    const yl::string& GetPageID() const;
    void SetPageID(const yl::string& strPageID);
    // 设置、获取Delegate是否是绑定首次显示
    bool IsFirstBindShow();
    void SetFirstBindShow(bool bFirstBindShow);
    // 设置、获取 界面成为活动窗口时，是否重新从底层获取数据刷新界面
    bool IsRefreshWndWhileActive();
    void SetRefreshWndWhileActive(bool bRefreshWndWhileActive);
    //获取/设置成为顶层 Delegate时是否刷新数据
    bool IsRefreshDataWhenTopDelegate();
    void SetRefreshDataWhenTopDelegate(bool bRefresh);
    //在重新加载数据的时候是否重新设置焦点
    virtual bool IsResetFocusWhileReload()
    {
        return false;
    }
    //获取界面ID
    int GetPageLineID()
    {
        return m_iLineID;
    }

    yl::string GetNextPageID()
    {
        return "";
    }
    bool CheckTalking();
    bool IsExistTalking();

protected:
    void SetValidHintData(CSettingUILineEditItemData * pInputItem, CSettingUIDataValidInfo & validInfo
                          , const yl::string & strDefaultHint, int nValidIndex, ITEM_INVALID_DATA_TYPE eType);

    virtual int TransStringToSoftkeyType(const yl::string & strSoftkeyAction);

    // 动态配置过滤
    void FilterDynamicConfig(SettingDynamicConfigFilter * pDynamicFilter,
                             CSettingUIItemDataBase * pItemData, int iStartIndex);
    // 过滤辅助函数
    bool CheckValue(SettingDynamicConfigFilter * pDynamicFilter, SettingDynamicItem * pDynamicItem
                    , CSettingUIItemDataBase * pItemData, const yl::string & strV);

    bool AddMenuItem(CSettingUIPageDataBase * pPageData, const yl::string & strID,
                     const yl::string & strName,
                     const yl::string & strAuthoridID = "", bool bShowIndex = true, int nAccountCount = -1,
                     const yl::string & strExtra = "",
                     const yl::string & strClickAction = "");

    // 添加基础数据项
    bool AddBaseItemData(CSettingUIPageDataBase * pPageData, const yl::string & strDisplayName,
                         const yl::string & strValue,
                         const yl::string & strID = "", bool bShowIndex = true, bool bShowColon = true);

    bool AddLineEditItemData(CSettingUIPageDataBase * pPageData, const yl::string & strID,
                             const yl::string & strName, const yl::string & strValue = "", int nMaxLength = -1,
                             const yl::string & strIme = "", const yl::string & strDefaultIme = "",
                             bool bPassword = false, const yl::string & strAuthorid = "", bool bAllowEmpty = false,
                             bool bShowIndex = true, bool bShowColon = true);

    void SetItemShow(CSettingUIPageDataBase * pPagedata, const yl::string & strItemId,
                     bool bShow = false);
    bool IsItemShow(const yl::string & strItemId);
    void SetItemEnable(CSettingUIPageDataBase * pPagedata, const yl::string & strItemId, bool bEnable);
    void HideAllItem(CSettingUIPageDataBase * pPagedata);

public:
    // 获取界面内存中数据
    CSettingUIItemDataBase * GetItemDataByDataIndex(int nDataIndex);
    // 根据Item ID获取Item数据
    CSettingUIItemDataBase * GetItemByItemId(const yl::string & strItemId);
    // 根据Item ID获取Item序号
    int GetItemDataIndexByItemId(const yl::string & strItemId);
    // 获取界面数据
    CSettingUIPageDataBase * GetPageData()
    {
        return m_pPageData;
    }
    // 获取第nDataIndex项的Extra Data
    void * GetItemExtraDataByDataIndex(int nDataIndex);
    // 获取item id
    const yl::string & GetItemIDByDataIndex(int nDataIndex);

    // 设置UI上显示的第nDataIndex项的值到保存列表中
    virtual void SetItemDataValueByDataIndex(int nDataIndex, const yl::string & strValue);
    // 设置UI上显示的第nDataIndex项的值到配置中
    virtual void SetItemDataValueToConfigByDataIndex(int nDataIndex, const yl::string & strValue);

    // 获取每页显示item的数量
    virtual int GetPageItemCount();
    // 获取每页显示item的数量
    void SetPageItemCount(int nPageItem);
    // 获取显示项个数
    int GetTotalDataCount();
    // 获取界面数据类型
    int GetPageDataType();
    // 获取界面标题
    const yl::string & GetTitleName();
    // 获取界面数据为空的提示
    const yl::string & GetEmptyHint();
    // 获取第nDataIndex项的事件
    const yl::string & GetItemActionByDataIndex(int nDataIndex);
    // 获取第nDataIndex项的Focus事件
    const yl::string & GetItemFocusActionByDataIndex(int nDataIndex);

    // 更新显示项序号
    void UpdateShowIndex();

    // 判断界面数据是否被修改过
    virtual bool IsPageValueChange(CSettingUIPageDataBase * pPagedata);
    // 判断数据是否合法
    virtual bool PageDataValid(CSettingUIPageDataBase * pPagedata, CSettingUIDataValidInfo & validInfo);

    // 获取界面绘制背景
    virtual xBrush GetDialogBackground()
    {
        return xBrush();
    }

    // 设置获取配置数据路径
    static void SetPageDataPath(const yl::string & strPath);
    static const yl::string & GetPageDataPath();

    //获取界面是否可编辑
    bool IsPageEditAble();

public:
    // ComboBox值改变响应函数
    virtual void OnComboBoxValueChange(int nIndex);
    // Edit值改变响应函数
    virtual void OnEditValueChange(int nIndex);

protected:
    CSettingUIPageDataBase* m_pPageData;            // 界面数据
    int m_iLineID;              // 界面LineID
    bool m_bShow;                // 界面是否显示
    bool m_bFirstBindShow;       // 是否是第一次绑定Delegate显示
    yl::string m_strPageID;            // 界面ID
    static yl::string m_strPageDataPath;      // 配置数据路径
    int m_nItemCountPerPage;    // 每页显示的item个数
    bool m_bBackToPreviousInterface; // 保存后是否返回到上一级界面
    bool m_bRefreshWndWhileActive;   // 界面成为活动窗口时，是否重新从底层获取数据刷新界面
    bool m_bRefreshWhenTopDelegate; //界面成为顶层Delegate时，是否重新从底层获取数据刷新界面
};

//typedef YLSmartPtr<CSettingUIDelegateBase> CSettingUIDelegateBasePtr;
typedef CSettingUIDelegateBase * CSettingUIDelegateBasePtr;

#endif

