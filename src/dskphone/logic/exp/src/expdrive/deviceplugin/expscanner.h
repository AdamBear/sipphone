#ifndef _EXP_SCANNER_H__
#define _EXP_SCANNER_H__

#if defined(_EXP_COLOR_SUPPORT) || defined(_EXP_BLACK_SUPPORT)
#include "ETLLib/ETLLib.hpp"
#include "baseexpmsgprocess.h"
#include "etlmsghandler/modetlmsghandler.h"

/**********************************************************************************************//**
 * @class   CScannner
 * @note    在这里解释一下各个类型：
 *
 *           ExpID: 指第几台exp
 *           KeyID/KeyCode: 指按键ID
 *           ExpType: 区分不同类型的exp
 *
 *           DsskeyID:  由上述三种数据通过移位合成的ID . 逻辑层会去使用这个ID
 *           （实际的数据类型均是int）
 *
 *           注意：这里的DsskeyID 没有按键在哪一页的信息，需要expscanner这个模块记录当前exp在哪个页面，手动加上一整页按键的数量，生成为带
 *           页数信息的DsskeyID
 *
 * @brief   A scannner.
 **************************************************************************************************/
class CExpBaseDrive;
class CScannner
{

public:

    static CScannner & GetInstance();

    static LRESULT OnMsg(msgObject & msg);


public:

    /**********************************************************************************************//**
     * @fn  void CScannner::UnInit();
     *
     * @brief   Un init exp scanner
     **************************************************************************************************/

    void UnInit();

    /**********************************************************************************************//**
     * @fn  void CScannner::Init(EXP_DRIVE_TYPE eDriveType = EXP_DRIVE_TYPE_INVALID);
     *
     * @brief   Init exp scanner.
     *
     * @param   eDriveType  (Optional) Type of the drive.
     **************************************************************************************************/

    void Init(EXP_DRIVE_TYPE eDriveType);

    /**********************************************************************************************//**
     * @fn  LRESULT CScannner::OnMsgFromScanThread(msgObject& msg);
     *
     * @brief   Executes the message from scan thread action.
     *
     * @param [in,out]  msg The message.
     *
     * @return  A LRESULT.
     **************************************************************************************************/

    LRESULT OnMsgFromScanThread(msgObject & msg);

    /**********************************************************************************************//**
     * @fn  LRESULT CScannner::OnMsgTimer(msgObject& msg);
     *
     * @brief   Executes the message timer out.
     *
     * @param [in,out]  msg The message.
     *
     * @return  A LRESULT.
     **************************************************************************************************/

    LRESULT OnMsgTimer(msgObject & msg);

    /**********************************************************************************************//**
     * @fn  DEVICE_TYPE_DEF CScannner::GetExpTypeByExpIndex(ExpID expId);
     *
     * @brief   Gets exp type by exp index.
     *
     * @param   expId   Index of the exp
     *
     * @return   enumType    The exp type
     **************************************************************************************************/

    DEVICE_TYPE_DEF GetExpTypeByExpIndex(ExpID expId);

    /**********************************************************************************************//**
     * @fn  DEVICE_TYPE_DEF CScannner::GetCurrentExpType();
     *
     * @brief   Gets current exp type.
     *
     * @return  The current exp type.
     **************************************************************************************************/

    DEVICE_TYPE_DEF GetCurrentExpType();

    /**********************************************************************************************//**
     * @fn  int CScannner::GetCurrentPageByExpIndex(ExpID expId);
     *
     * @brief   Gets current page by exp index.
     *
     * @param   expId   index for the exp.
     *
     * @return  The current page index by exp index.
     **************************************************************************************************/

    int GetCurrentPageByExpIndex(ExpID expId);

    /**********************************************************************************************//**
     * @fn  bool CScannner::SetCurrentPageByIndex(int iExpID, int iPage);
     *
     * @brief   Sets current page by exp index.
     *
     * @param   iExpID  Index for the exp.
     * @param   iPage   new index of the page you want to set
     *
     * @return  True if it succeeds, false if it fails.
     **************************************************************************************************/

    bool SetCurrentPageByIndex(int iExpID, int iPage);

    /**********************************************************************************************//**
     * @fn  int CScannner::GetConnectedExpCounts(DEVICE_TYPE_DEF eType = DT_ALL);
     *
     * @brief   Gets connected exp counts.
     *
     * @param   eType   (Optional) The exp type.
     *
     * @return  The connected exponent counts.
     **************************************************************************************************/

    int GetConnectedExpCounts(DEVICE_TYPE_DEF eType = DT_ALL);

    /**********************************************************************************************//**
     * @fn  bool CScannner::IsTestMode();
     *
     * @brief   Query if exp is in test mode.
     *
     * @return  True if test mode, false if not.
     **************************************************************************************************/

    bool IsTestMode();

    /**********************************************************************************************//**
     * @fn  bool CScannner::SetTestMode(bool bEnable);
     *
     * @brief   Sets exp to enable test mode or not.
     *
     * @param   bEnable True to enable, false to disable.
     *
     * @return  True if it succeeds, false if it fails.
     **************************************************************************************************/

    void SetTestMode(bool bEnable);

	ExpHandle GetExpHandle();

private:

    typedef YLList<CBaseExpMsgProcess *> MsgProcessFactory;
    typedef YLList<CBaseExpMsgProcess *>::iterator MsgProcessIter;

    CScannner();
    ~CScannner();
    CScannner(const CScannner &);

private:

    void RegisterMsgHandle();

    CBaseExpMsgProcess * GetMsgProcessByExpType(DEVICE_TYPE_DEF expType);

	CBaseExpMsgProcess * GetOrCreateMsgProcessByExpType(DEVICE_TYPE_DEF expType);

    void DestroyMsgProcess(DEVICE_TYPE_DEF expType);

    void InitExpByMsg(const msgObject & msg);

	bool IsAllowExpInsert(DEVICE_TYPE_DEF eType);

    inline void RegisterPrivateMsg(UINT uMsg)
    {
        etl_RegisterMsgHandle(uMsg, uMsg, OnMsg);
    }

    inline bool IsAllowMixedInsert()
    {
        return m_bEnableMixedInsert;
    }

    void RegisterCurrentExpType(DEVICE_TYPE_DEF eType)
    {
        m_CurrentExpType = eType;
    }

    inline int GetKeyCodeFromDsskeyID(DsskeyID dssId)
    {
        return (dssId & 0x3f);
    }

    inline ExpID GetExpIndexFromDssKeyID(DsskeyID dssId)
    {
        return ((dssId >> 6) & 0xf);
    }

    inline DEVICE_TYPE_DEF GetExpTypeFromDssKeyID(DsskeyID dssId)
    {
        return static_cast<DEVICE_TYPE_DEF>(dssId >> 24);
    }

private:

    CExpBaseDrive * m_ScanDrive;

    MsgProcessFactory m_MsgProcessList;

    /** @brief  Type of the current exp. */
    DEVICE_TYPE_DEF m_CurrentExpType;

    bool m_bEnableMixedInsert;

    bool m_bIsInited;

};

#define g_ExpScanner CScannner::GetInstance()

#endif
#endif // _RJ_SCANNER_H__
