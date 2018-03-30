#ifndef __IMAGE_FORMAT_DEFINE_H__
#define __IMAGE_FORMAT_DEFINE_H__

#include <xwindow/xwindowdefine.h>
#include "ylstring.h"

#if !IF_ONLY_USE_GRADATION
#define IF_SUPPORT_BMP 1
#define IF_SUPPORT_JPEG 1
#define IF_SUPPORT_PNG 1
#define IF_IMAGE_FORMAT_DEBUG 0

enum IMAGE_LOADERROR_REASON
{
    IMAGE_LOADERROR_REASON_NONE = 0,
    IMAGE_LOADERROR_REASON_OVERSIZE,
    IMAGE_LOADERROR_REASON_OVERLENGTH,
    IMAGE_LOADERROR_REASON_INVALID_CONTENT,
};

const char* const kszImageFormatBMP = ".bmp";
const char* const kszImageFormatJPEG = ".jpg";
const char* const kszImageFormatPNG = ".png";

const char* const kszDefaultFormat = ".png";
#endif

#endif
