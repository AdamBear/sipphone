#ifndef __TOOLS_STRING_SPLIT__
#define __TOOLS_STRING_SPLIT__

#include <vector>
#include <string>

typedef std::vector<std::string> SplitResult;
typedef std::vector<std::wstring> wSplitResult;

template<typename CharT>
std::vector<std::basic_string<CharT> >
split(const std::basic_string<CharT> & str,
      const std::basic_string<CharT> & sep)
{
    typedef std::basic_string<CharT> string_type;
    std::vector<string_type> ret;
    if (str.empty())
    {
        return ret;
    }

    string_type tmp;
    typename string_type::size_type pos_begin = str.find_first_not_of(sep);
    typename string_type::size_type comma_pos = 0;

    while (pos_begin != string_type::npos)
    {
        comma_pos = str.find_first_of(sep, pos_begin);
        if (comma_pos != string_type::npos)
        {
            tmp = str.substr(pos_begin, comma_pos - pos_begin);
            pos_begin = comma_pos + 1;
        }
        else
        {
            tmp = str.substr(pos_begin);
            pos_begin = comma_pos;
        }

        if (!tmp.empty())
        {
            ret.push_back(tmp);
            tmp.clear();
        }
    }
    return ret;
}

template<typename CharT>
std::vector<std::basic_string<CharT> >
split(const std::basic_string<CharT> & str,
      const char * sep)
{
    typedef std::basic_string<CharT> string_type;
    return split(str, string_type(sep));
}

inline void trim_left(std::string & str, const std::string & trim_str /*= ' '*/)
{
    std::string::size_type pos = str.find_first_not_of(trim_str);
    str.erase(0, pos);
}

inline void trim_right(std::string & str, const std::string & trim_str /*= ' '*/)
{
    std::string::size_type pos = str.find_last_not_of(trim_str);
    str.erase(pos + 1);
}

inline void trim_both(std::string & str, const std::string & trim_str /*= ' '*/)
{
    trim_left(str, trim_str);
    trim_right(str, trim_str);
}

inline std::string dump_file(const std::string & fileName)
{
    FILE * file = fopen(fileName.c_str(), "rb");
    std::string file_buffer;
    while (file)
    {
        // 获取文件长度.
        fseek(file, 0L, SEEK_END);
        int nLength = ftell(file);
        fseek(file, 0L, SEEK_SET);
        if (nLength <= 0)
        {
            fclose(file);
            break;
        }

        //读取文件到内存

        file_buffer.resize(nLength + 1);
        fread(&file_buffer[0], sizeof(std::string::value_type), nLength, file);
        fclose(file);
    }
    return file_buffer;
}


#endif
