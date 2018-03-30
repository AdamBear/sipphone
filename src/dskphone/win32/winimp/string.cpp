#include "common.h"

bool parseString(std::vector<std::string> & vec, const std::string & src,
                 const std::string & filter)
{
    if (!src.empty())
    {
        size_t begpos = 0;
        size_t found = src.find_first_of(filter, begpos);
        while (found != std::string::npos)
        {
            if (begpos < found)
            {
                vec.push_back(src.substr(begpos, found - begpos));
            }

            begpos = found + 1;
            found = src.find_first_of(filter, begpos);
        }

        if (begpos < src.size())
        {
            vec.push_back(src.substr(begpos));
        }
    }

    return !vec.empty();
}

std::string formatString(const char * formate, ...)
{
    std::string strOut("");

    if (formate == NULL)
    {
        return strOut;
    }

    va_list arg_ptr;
    va_start(arg_ptr, formate);

    int len = vsnprintf(NULL, 0, formate, arg_ptr);
    if (len == 0)
    {
        return strOut;
    }
    ++len;
    char * buffer = (char *)::GlobalAlloc(GPTR, len);
    if (buffer == NULL)
    {
        return strOut;
    }
    vsnprintf(buffer, len, formate, arg_ptr);
    strOut = buffer;
    ::GlobalFree(buffer);

    va_end(arg_ptr);
    return strOut;
}

void to_upper(std::string & str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void to_lower(std::string & str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void delete_char(std::string & str, char ch)
{
    str.erase(std::remove_if(str.begin(), str.end(), std::bind2nd(std::equal_to<char>(), ch)),
              str.end());
}

bool start_with(const std::string & str, const std::string & start)
{
    if (str.empty() && start.empty())
    {
        return true;
    }
    return str.compare(0, start.size(), start) == 0;
}

bool end_with(const std::string & str, const std::string & end)
{
    if (str.empty() && end.empty())
    {
        return true;
    }
    return str.compare(str.size() - end.size(), end.size(), end) == 0;
}

bool istart_with(const std::string & str, const std::string & start)
{
    std::string strLower(str);
    std::string strLowerStart(start);
    to_lower(strLower);
    to_lower(strLowerStart);
    return start_with(strLower, strLowerStart);
}

bool iend_with(const std::string & str, const std::string & end)
{
    std::string strLower(str);
    std::string strLowerEnd(end);
    to_lower(strLower);
    to_lower(strLowerEnd);
    return end_with(strLower, strLowerEnd);
}

void replaceString(std::string & str, const std::string & from, const std::string & to)
{
    if (from == to)
    {
        return;
    }

    std::string::size_type pos = str.find(from);
    while (pos != std::string::npos)
    {
        str.replace(pos, from.size(), to);
        // from the begin, sample in: "ababcd" "abcd" "cd"  out: cd
        pos = str.find(from);
    }
}

void trim_both(std::string & str, const std::string & trim_str /*= " \t"*/)
{
    std::string::size_type pos = str.find_first_not_of(trim_str);
    if (pos != std::string::npos)
    {
        str.erase(0, pos);
    }
    pos = str.find_last_not_of(trim_str);
    if (pos != std::string::npos)
    {
        str.erase(pos + 1);
    }
}

std::wstring WAPIString2Wstring(const std::string & str)
{
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
    wchar_t * buffer = (wchar_t *)::GlobalAlloc(GPTR, (len + 1) * sizeof(wchar_t));
    if (!buffer)
    {
        return L"";
    }
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
    std::wstring strOut(buffer, len);
    ::GlobalFree(buffer);
    return strOut;
}

std::string WAPIWstring2String(const std::wstring & wstr)
{
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, 0);
    char * buffer = (char *)::GlobalAlloc(GPTR, (len + 1) * sizeof(char));
    if (!buffer)
    {
        return "";
    }
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, 0);
    std::string strOut(buffer, len);
    ::GlobalFree(buffer);
    return strOut;
}
