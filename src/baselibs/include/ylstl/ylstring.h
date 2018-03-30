//#define USING_STDCPP
#ifndef __YLSTL_YLSTRING_H__
#define __YLSTL_YLSTRING_H__


#include "string_like.h"

#if HAVE_STDINT_H
#include <stdint.h>
#endif //HAVE_STDINT_H

namespace yl
{
typedef yl::basic_string<char> string;
typedef yl::basic_string<wchar_t> wstring;
#if HAVE_STDINT_H
typedef yl::basic_string<uint16_t> u16string;
typedef yl::basic_string<uint32_t> u32string;
#else //HAVE_STDINT_H
typedef yl::basic_string<unsigned short> u16string;
typedef yl::basic_string<unsigned int> u32string;
#endif  //HAVE_STDINT_H
}

#endif //__YLSTL_YLSTRING_H__


