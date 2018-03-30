#ifndef __DATA_SAVE_H__
#define __DATA_SAVE_H__

#include "ylstl/ylhashmap.h"
#include "settinguicommondata.h"

class CSettingUIPageDataBase;
class CSettingUIMessageData;
class CSettingUIItemDataBase;

typedef YLHashMap<yl::string, PageSaveFun> SaverMap;
class CDataSave //: public CMsgBoxCallBackAction
{
public:
    static CDataSave * GetInstance();

    void Init();
    // 保存界面数据到配置中
    bool SaveData(CSettingUIPageDataBase * pPageInfo);
    // 保存单个项的数据到配置中，并发消息通知外面。
    // 如果是保存界面，bSendMsg需要传入false，界面后面会一起发
    bool SaveItemData(CSettingUIPageDataBase * pPageInfo, CSettingUIItemDataBase * pItemInfo,
                      bool & bChange
                      , bool bSendMsg = true, int nLineID = -1);

    // 注册特殊界面保存方式
    void RegisterSaveFun(const yl::string & strPageName, PageSaveFun pFun);
    // 获取生成函数指针
    PageSaveFun GetSaveFun(const yl::string & strPageName);

public:
    //virtual void MessageBoxCallBack(CMessageBoxBase* pMessageBox);

private:
    CDataSave();
    bool NormalSave(CSettingUIPageDataBase * pPageInfo);

    void ProcessMessage(CSettingUIMessageData * msgParm, CSettingUIPageDataBase * pPageInfo);
    void SaveDoubleParamconfig(const yl::string & strConfigId, const int iLineID,
                               const yl::string & strWgtID, const yl::string & strValue);
    void SaveSingleParamconfig(const yl::string & strConfigId, const int iLineID,
                               const yl::string & strItemID
                               , const yl::string & strValue, yl::string & strPreValue);
private:
    static CDataSave * m_pInstance;
    SaverMap m_saverMap;
};

#define g_DataSaver (CDataSave::GetInstance())

#endif  //DATA_SAVE_H

