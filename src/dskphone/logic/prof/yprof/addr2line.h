#ifndef __PROF_ADDR2LINE_H__
#define __PROF_ADDR2LINE_H__

#include <string>
#include <map>
#include "profdata.h"

class addr2line
{
    struct addr_bounds
    {
        PROF_ADDR_TYPE start_addr;
        PROF_ADDR_TYPE end_addr;
        addr_bounds(PROF_ADDR_TYPE StartAddr = 0,
                    PROF_ADDR_TYPE EndAddr = 0)
            : start_addr(StartAddr),
              end_addr(EndAddr)
        {}
    };
    typedef const std::string & string_in;
public:
    addr2line();
    explicit addr2line(string_in exec_name, string_in so_search_path = "", string_in maps = "");

    bool set_exec(string_in exec_name);
    bool set_so_search_path(string_in path);
    bool set_toolset_prefix(string_in toolset);
    bool load_maps(string_in maps);

    std::string getline(PROF_ADDR_TYPE addr);

protected:
    std::string get_addr_locate(PROF_ADDR_TYPE addr);
    PROF_ADDR_TYPE   get_addr_in_object(PROF_ADDR_TYPE addr);

private:
    typedef std::map<std::string, addr_bounds> ADDR_BOUNDS_MAP;
    ADDR_BOUNDS_MAP map_addr_bounds;//存放区段地址
    std::string exec_name;
    std::string so_search_path;
    std::string toolset;
};


#endif // __PROF_ADDR2LINE_H__
