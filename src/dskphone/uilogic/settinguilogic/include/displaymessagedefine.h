#ifndef DEFINE_DISPLAYMESSAGEDEFINE_H_
#define DEFINE_DISPLAYMESSAGEDEFINE_H_

#include "innermsgdefine.h"

// Display message definition.
enum DisplayMessageDefine
{
    DISPLAY_MESSAGE_BEGIN = INNER_MSG_DISPLAY_BEGIN,

    // Back light time changed. LPARAM: the back light time after changed.
    DISPLAY_BACKLIGHT_TIME_CHANGED = DISPLAY_MESSAGE_BEGIN + 1,

    // Screen saver type changed. LPARAM: the screen saver type after changed.
    DISPLAY_SCREEN_SAVER_TYPE_CHANGED = DISPLAY_MESSAGE_BEGIN + 2,

    // Screen saver interval changed. LPARAM: the screen saver interval
    // after changed.
    DISPLAY_SCREEN_SAVER_INTERVAL_CHANGED = DISPLAY_MESSAGE_BEGIN + 3,

    // Wallpaper list changed. No data send, the receiver should reload
    // the Wallpaper list.
    DISPLAY_WALLPAPER_LIST_CHANGED = DISPLAY_MESSAGE_BEGIN + 4,
    // Current Wallpaper changed. Extra: current wallpaper after change.
    DISPLAY_CURRENT_WALLPAPER_CHANGED = DISPLAY_MESSAGE_BEGIN + 5,

    // Theme list changed. No data send, the receiver should reload
    // the theme list.
    DISPLAY_THEME_LIST_CHANGED = DISPLAY_MESSAGE_BEGIN + 6,
    // Current theme changed. Extra: Current theme after change.
    DISPLAY_CURRENT_THEME_CHANGED = DISPLAY_MESSAGE_BEGIN + 7,

    // Photo list changed. No data send, the receiver should reload
    // the photo list.
    DISPLAY_PHOTO_LIST_CHANGED = DISPLAY_MESSAGE_BEGIN + 8,

    // Video list changed. No data send, the receiver should reload
    // the video list.
    DISPLAY_VIDEO_LIST_CHANGED = DISPLAY_MESSAGE_BEGIN + 9,

    DISPLAY_MESSAGE_END = INNER_MSG_DISPLAY_END
};

#endif  // DEFINE_DISPLAYMESSAGEDEFINE_H_
