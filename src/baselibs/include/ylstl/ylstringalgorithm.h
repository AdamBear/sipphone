#ifndef __YLSTL_YLSTRING_ALGORITHM__
#define __YLSTL_YLSTRING_ALGORITHM__

#include "ylstring.h"
#include "ylvector.h"

#if WIN32
#ifdef ylstl_EXPORTS
#define YLSTL_DLL_DECLARRE      __declspec(dllexport)
#else
#define YLSTL_DLL_DECLARRE      __declspec(dllimport)
#endif //ylstl_EXPORTS
#else
#define YLSTL_DLL_DECLARRE
#endif //WIN32


namespace yl
{
YLSTL_DLL_DECLARRE string to_utf8(const wstring & u16str);

YLSTL_DLL_DECLARRE wstring to_utf16(const string & u8string);

YLSTL_DLL_DECLARRE string dump_file(const string & fileName);

YLSTL_DLL_DECLARRE wstring dump_utf16_file(const string & fileName);

YLSTL_DLL_DECLARRE bool write_file(const string & fileName, const string & data);

YLSTL_DLL_DECLARRE void string_replace(string & str, const string & from, const string & to);

YLSTL_DLL_DECLARRE void to_lower(string & str);

YLSTL_DLL_DECLARRE void to_upper(string & str);

YLSTL_DLL_DECLARRE void trim_both(string & str, const string & trim_str = " \t");

YLSTL_DLL_DECLARRE void trim_left(string & str, const string & trim_str = " \t");

YLSTL_DLL_DECLARRE void trim_right(string & str, const string & trim_str = " \t");

YLSTL_DLL_DECLARRE string format_string(const char * fmt, ...);

YLSTL_DLL_DECLARRE wstring format_wstring(const wchar_t * fmt, ...);

template<typename CharT>
void string_reverse(basic_string<CharT> & str)
{
    for (string::size_type pos = 0; pos < str.size() / 2; ++pos)
    {
        yl::swap(str[pos], str[str.size() - pos - 1]);
    }
}

template<typename CharT>
YLVector<basic_string<CharT> > string_split(const basic_string<CharT> & str,
        const basic_string<CharT> & tokens, bool allowEmpty = false)
{
    typedef basic_string<CharT> string_type;
    typedef YLVector<string_type> Container;

    Container result;

    typename string_type::size_type pos = 0;
    typename string_type::size_type pre_pos = 0;
    while ((pos = str.find_first_of(tokens, pos)) != string_type::npos)
    {
        string_type found;
        if (pos != pre_pos)
        {
            found = str.substr(pre_pos, pos - pre_pos);
        }
        if (!found.empty() || allowEmpty)
        {
            result.push_back(found);
        }
        pre_pos = ++pos;
    }

    if (pre_pos < str.size())
    {
        result.push_back(str.substr(pre_pos, str.size() - pre_pos));
    }

    return result;
}

}

#endif

