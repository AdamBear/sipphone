#ifndef _MOD_EXP_SCANNER_H__
#define  _MOD_EXP_SCANNER_H__
#include "exp/include/exp_struct.h"

/**********************************************************************************************//**
 * @fn  void exp_Init(EXP_DRIVE_TYPE eDriveType);
 *
 * @brief   Exp init.
 *
 * @param   eDriveType  Type of the drive.
 *
 **************************************************************************************************/

void exp_Init(EXP_DRIVE_TYPE eDriveType = EXP_DRIVE_TYPE_INVALID);


/**********************************************************************************************//**
 * @fn  bool exp_IsTestMode();
 *
 * @brief   Get if  exp is test mode.
 *
 * @return  True if it is testmode, false if it is not.
 **************************************************************************************************/

bool exp_IsTestMode();


/**********************************************************************************************//**
 * @fn  int exp_GetExpCounts();
 *
 * @brief   Get connected exp counts.
 *
 * @return  An int, exp counts
 **************************************************************************************************/

int exp_GetExpCounts(DEVICE_TYPE_DEF eType = DT_ALL);

/**********************************************************************************************//**
 * @fn  DEVICE_TYPE_DEF exp_GetExpTypeByIndex(int iExpID);
 *
 * @brief   get exp type by exp index.
 *
 * @param   iExpID  Index for the exp.
 *
 * @return   DEVICE_TYPE_DEF.
 **************************************************************************************************/

DEVICE_TYPE_DEF exp_GetExpTypeByIndex(int iExpID);

/**********************************************************************************************//**
 * @fn  int exp_GetCurrentPageByIndex(int iExpID);
 *
 * @brief    get  exp current page by exp index.
 *
 * @param   iExpID  iExpID  Index for the exp.
 *
 * @return  page index
 **************************************************************************************************/

int exp_GetCurrentPageByIndex(int iExpID);

/**********************************************************************************************//**
 * @fn  bool exp_SetCurrentPageByIndex(int iExpID,int iPage);
 *
 * @brief   Exponent set current page by index.
 *
 * @param   iExpID  index of exp to be set
 * @param   iPage   page index
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

bool exp_SetCurrentPageByIndex(int iExpID, int iPage);

/**********************************************************************************************//**
 * @fn  int exp_GetEhsIndex();
 *
 * @brief    get ehs index.
 *
 * @return  An int.
 **************************************************************************************************/

int exp_GetEhsIndex();

/**********************************************************************************************//**
 * @fn  void exp_EHSCtrl(int nIndex, int nCtrlCode);
 *
 * @brief   Exp ehs control.
 *
 * @param   nIndex      The exp index.
 * @param   nCtrlCode   The control code.
 **************************************************************************************************/

void exp_EHSCtrl(int nIndex, int nCtrlCode);

/**********************************************************************************************//**
 * @fn  bool exp_InitEHSData();
 *
 * @brief   Init ehs date.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/
bool exp_InitEHSData();

/**********************************************************************************************//**
 * @fn  bool exp_SetExpTestMode(bool bEnable);
 *
 * @brief   Enable/Disable exponent test mode.
 *
 * @param   bEnable True to enable, false to disable.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

void exp_SetExpTestMode(bool bEnable);


ExpHandle exp_GetExpHandle();
#endif // !_MOD_EXP_SCANNER_H__
