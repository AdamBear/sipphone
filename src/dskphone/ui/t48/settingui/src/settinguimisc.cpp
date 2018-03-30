#include "settinguicommon.h"
#include "uicommon/uikernel/qlineeditutility.h"
#include <QLineEdit>
#include "baseui/fontcolor.h"

void SetLineEditDefaultStyle(QLineEdit * pEdit)
{
    if (NULL != pEdit)
    {
        qLineEditUtility::SetLineEditStyle(pEdit, THEME_TEXT_COLOR, THEME_EDIT_HL_COLOR,
                                           THEME_EDIT_HL_TEXT_COLOR);
    }
}
