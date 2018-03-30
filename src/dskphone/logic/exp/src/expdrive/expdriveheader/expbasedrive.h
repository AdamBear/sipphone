#ifndef _EXP_BASE_DRIVE_H__
#define _EXP_BASE_DRIVE_H__

#include "expscannerdefine.h"
#include "exp_struct.h"

class CExpBaseDrive
{

public:

    CExpBaseDrive()
    {
        m_eDriveType = EXP_DRIVE_TYPE_INVALID;
    }

    /**
     * @brief Init drive.
     *
     * @author Song
     * @date 2017/8/4
     */

    virtual void InitDrive() = 0;

    /**
     * @brief Starts key scan.
     *
     * @author Song
     * @date 2017/8/4
     */

    virtual void StartKeyScan() = 0;

    /**
     * @brief Starts insert scan.
     *
     * @author Song
     * @date 2017/8/4
     */

    virtual void StartInsertScan() = 0;

    /**
     * @brief Init exponent by index.
     *
     * @author Song
     * @date 2017/8/4
     *
     * @param expId Identifier for the exponent.
     */

    virtual void InitExpByIndex(ExpID expId) = 0;

    /**
     * @brief Gets driver type.
     *
     * @author Song
     * @date 2017/8/4
     *
     * @return The driver type.
     */

    inline EXP_DRIVE_TYPE GetDriverType()
    {
        return m_eDriveType;
    }

	virtual ExpHandle  GetExpHandle() = 0;
protected:
    EXP_DRIVE_TYPE m_eDriveType;
};


#endif  //!_EXP_BASE_DRIVE_H__




