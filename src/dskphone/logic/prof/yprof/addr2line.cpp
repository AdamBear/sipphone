#include "addr2line.h"
#include "stringsplit.h"


#include <stdio.h>
#include <unistd.h>

std::string get_stdout(const std::string commad)
{
    //打开命令
    FILE * ps = popen(commad.c_str(), "r");
    if (ps == 0)
    {
        printf("popen file [%d] failed\n", ps);
    }

    const size_t buffer_size = 60;
    char buffer[buffer_size];
    size_t size = 0;
    std::string ret;
    while (size = fread((void *)buffer, 1, buffer_size, ps))
    {
        ret.append(buffer, size);
    }

    pclose(ps);
    return ret;
}

std::string call_addr2line(PROF_ADDR_TYPE addr, const std::string & filename,
                           const std::string toolset = "")
{
    char buffer[1024];
    const char addr2line_cmd_format[] = "%saddr2line -e %s -f -C -p -i %x";
    sprintf(buffer, addr2line_cmd_format, toolset.c_str(), filename.c_str(), addr);
    std::string ret = get_stdout(buffer);
    std::string::size_type file_pos = ret.find(" at ");
    if (file_pos != std::string::npos)
    {
        ret.erase(file_pos);
    }
    else
    {
        //尝试在符号表里找
        const char readelf_cmd_format[] =
            "%sreadelf --dyn-syms -W %s | awk '$2==\"%0.8x\" {print $8}' | c++filt";
        sprintf(buffer, readelf_cmd_format, toolset.c_str(), filename.c_str(), addr);
        ret = get_stdout(buffer);
        trim_both(ret, " \n\r");
        if (ret.empty())
        {
            sprintf(buffer, "0x%x <Unknow Function>", addr);
            return buffer;
        }
    }
    return ret;
}

addr2line::addr2line()
{

}

addr2line::addr2line(string_in exec_name, string_in so_search_path /*= ""*/,
                     string_in maps /*= ""*/)
{
    set_exec(exec_name);
    set_so_search_path(so_search_path);
    load_maps(maps);
}

bool addr2line::set_exec(string_in exec_name)
{
    this->exec_name = exec_name;
}

bool addr2line::set_so_search_path(string_in path)
{
    this->so_search_path = path;
    if (this->so_search_path.back() != '/')
    {
        this->so_search_path.push_back('/');
    }
}

bool addr2line::load_maps(string_in maps)
{
    if (maps.empty())
    {
        return false;
    }

    SplitResult lines          = split(maps, "\r\n");

    std::string::size_type pos = std::string::npos;
    std::string strSoName;
    std::string start;
    std::string end;

    for (auto line : lines)
    {
        trim_both(line, " \r\n");
        pos = line.find_last_of('/');
        if (pos != std::string::npos)
        {
            strSoName = line.substr(pos + 1);
            if (map_addr_bounds.count(strSoName) == 0 && line.find("r-x") != std::string::npos)
            {
                start = line.substr(0, 8);
                end   = line.substr(9, 8);
                map_addr_bounds[strSoName] = addr_bounds(strtoul(start.c_str(), NULL, 16), strtoul(end.c_str(),
                                             NULL, 16));
            }
        }
    }

    return !map_addr_bounds.empty();
}

std::string addr2line::get_addr_locate(PROF_ADDR_TYPE addr)
{
    //maps未载入，返回本身
    if (map_addr_bounds.empty())
    {
        return exec_name;
    }

    for (auto & bound : map_addr_bounds)
    {
        if (addr >= bound.second.start_addr && addr < bound.second.end_addr)
        {
            if (exec_name.find(bound.first) != std::string::npos)
            {
                return exec_name;
            }
            return bound.first;
        }
    }

    return "";
}

PROF_ADDR_TYPE addr2line::get_addr_in_object(PROF_ADDR_TYPE addr)
{
    std::string object_name = get_addr_locate(addr);
    //没有找到库文件，地址超出范围
    if (object_name.empty())
    {
        return 0;
    }

    //地址在EXE本身，返回原地址
    if (exec_name.find(object_name) != std::string::npos)
    {
        return addr;
    }

    //地址在so中，返回so中的相对地址
    if (map_addr_bounds.count(object_name))
    {
        return addr - map_addr_bounds[object_name].start_addr;
    }

    return addr;
}

std::string addr2line::getline(PROF_ADDR_TYPE addr)
{
    std::string object_name = get_addr_locate(addr);
    if (exec_name.find(object_name) == std::string::npos)
    {
        object_name = so_search_path + object_name;
    }

    PROF_ADDR_TYPE addr_in_object = get_addr_in_object(addr);

    char buffer[1024];
    sprintf(buffer, "0x%x", addr);

    if (access(object_name.c_str(), F_OK) == 0 && addr_in_object != 0)
    {
        return call_addr2line(addr_in_object, object_name, toolset) + "<" + buffer + ">";
    }

    return buffer;


}

bool addr2line::set_toolset_prefix(string_in toolset)
{
    this->toolset = toolset;
}
