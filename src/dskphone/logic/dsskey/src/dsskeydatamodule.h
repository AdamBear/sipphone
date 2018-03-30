#ifndef _DSSKEY_DATA_MODULE_HEADER_
#define _DSSKEY_DATA_MODULE_HEADER_
#include <ETLLib.hpp>
#include <ylvector.h>
#include <ylstl/ylstring.h>
#include "../include/dsskeystructure.h"
#include "dsskey_inner_enumtypes.h"
#include <xmlparser/pugixml.hpp>

class CDSSKeyPageProcessor;
using namespace std;

enum DSSKEY_MODULE_TYPE;
class CDSSkeyData;
class CDSSKeyDataModule
{
public:
    CDSSKeyDataModule(DSSKEY_MODULE_TYPE moduleType, int iModuleIndex);
    virtual ~CDSSKeyDataModule(void);
public:
    CDSSkeyData * GetDsskeyDataByIndex(int index);


    bool LoadFromMemory();

    bool SaveToMemory(int iDsskeyID = -1, int iFlags = 0);

    // 批量更新按键的配置，当配置文件改变时调用
    bool ReloadDssKeyAttrib(YLList<int> & lstDsskeyIndex);

    bool ReloadDssKeyAttrib(int index);

    //dssKey配置文件路径
    bool GetConfigFile(yl::string & strOutput);

    /**
     * @brief dsskey 配置数量.
     *
     * @author Song
     * @date 2017/8/14
     *
     * @param moduleType Type of the module.
     *
     * @return The support dsskey number.
     */

    static int GetSupportDsskeyNum(DSSKEY_MODULE_TYPE moduleType);

    /**
     * @brief Gets support exponent number.
     *
     * @author Song
     * @date 2017/8/14
     *
     * @return The support exponent number.
     */

    static int GetSupportExpNum();

    void Clear();

    void ResetBLFListDsskeyByLineId(int iLineId);

    bool GetEXPLPTimerStatus()
    {
        return m_bEXPKeyLPTimerIsOn;
    }

    void SetEXPLPTimerStatus(bool status);

    int size() DECLARE_ATTR;

    // 根据类型获取Dsskey列表
    void GetDsskeyListByType(YLVector<CDSSkeyData *> & vecDsskey, DssKey_Type eType);

    int m_tBePressedKey; // Exp按下的按键code（还未弹起）
    //
    bool GetDsskeyDataList(int fromIndex, int AmountToGet, YLList<CDSSkeyData * > & listOutput);

//  //获取dsskey数据所在页面的所有数据
//  void GetPageDataByDSSKey(CDSSkeyData *pDsskeyData,YLList<CDSSkeyData * > &listOutput);

    //获取当前页DSSKey数据
    bool GetCurrentPageData(YLList<CDSSkeyData *> & output);

    //获取某一页的DSSKey数据
    bool GetPageDataByPageIndex(YLList<CDSSkeyData *> & output, int pageIndex);

    //获取指定DSSKey数据所在的页面缩影
    int GetPageIndexByDSSKey(CDSSkeyData * pDsskeyData);


    DSSKEY_MODULE_TYPE GetModuleType()
    {
        return m_moduleType;
    }

    //将指定的DSSKey设置为Lock
    bool LockDsskey(CDSSkeyData * pDsskeyData, DssKeyLockType lockType, bool bOriginalLocked);

//  //根据账号获取dsskey指定为dsskeyType类型的数据
//  CDSSkeyData *GetFirstDsskeyDataByLineID(int lineIndex, DssKey_Type dsskeyType);

    //dsskey按键是否属于当前模块
    bool IsBelongDSSKey(CDSSkeyData * pDsskeyData);

    // 更新账号的图标显示，如Fwd、Dnd
    void UpdateLinekeyLabel(int iAccount = -1);

    // 获取分页数
    int GetPageAmount();

    // 进入下一个页面
    bool GotoNextPage();

    void SetPageInfo(int dsskeyIndex);

//  //是否存在被Lock的DSSKey
//  bool IsLockedKeyExist();

    bool SetDsskeyData(int iDsskeyID, const DssKeyLogicData & logicData,
                       int iFlags = DSSKEY_SET_NORMAL);

    // line状态改变消息处理
    bool OnLineStateChange(msgObject & msg);
#ifdef IF_SUPPORT_BLUETOOTH
    //处理bluetooth 状态改变消息
    bool OnBluetoothStateChange(msgObject & msg);
#endif //#ifdef IF_SUPPORT_BLUETOOTH

    //处理通知中心状态改变
    bool OnNotifyDsskeyStateChange(msgObject & msg);

    // 定时器处理
    bool OnTimer(UINT uTimerID) DECLARE_ATTR;


    int GetUnboundDsskeyByAccountID(int accountID);

    int CountsDsskeyByAccID(int iAccountID, bool bNative = false);
    // 根据Dsskey获取Dsskey的BlaIndex
    bool GetBLAIndexByDsskey(int & iIndex, int iDsskeyID, int iAccountID);

    /**
     * @brief 内存数据是否已更新.
     *
     * @author Song
     * @date 2017/8/21
     *
     * @param bDirty True to dirty.
     */

    void SetDirty(bool bDirty);

    /**
     * @brief 将模块数据重置成出厂.
     *
     * @author Song
     * @date 2017/8/22
     */

    void ResetModuleDataToFactory();

    // 保存Dsskey信息到指定文件中
    LPCSTR GetModuleName();
    bool SaveDsskeyInfoToFile(chFileStdio & doc);
    bool SaveDsskeyInfoToFile(pugi::xml_document & doc);
    bool SaveDsskeyInfoToFile(iniFile & doc);

protected:
    yl::string              m_strConfigFile;    //Dsskey配置文件路径

    //pDsskeyData对应的被lock的ID，如果不同页面的DSSKey没有被Lock或者本身为被lock的数据，则返回当前DSSKeyID
    //否则返回对应被Lock的DSSKey
    CDSSkeyData * GetRelatedLockedDsskey(CDSSkeyData * pDsskeyData);

    YLVector<CDSSkeyData *> m_vecDsskeyData;

    int m_LockKeySize;

    DSSKEY_MODULE_TYPE m_moduleType;

    int m_moduleIndex;

    bool            m_bEXPKeyLPTimerIsOn;  //EXP长按定时器是否启动

    int m_maxValidIndex;

    bool m_bIsDirty;

protected:
    //CDSSKeyPageProcessor *m_pRelatedPageProcessor;//对应的页面处理器，针对不同类型其页面处理数据不一致，保留后续扩展性

    CDSSkeyData * GetDsskeyDataByType(DssKey_Type eType);
};

#endif
