#ifndef __PROF_YPROF_H__
#define __PROF_YPROF_H__

#include "profdata.h"
#include <set>
#include <map>
#include <vector>

struct duration_type
{
    double             call_time;
    double             real_time;
    size_t             call_count;
    std::set<uint16_t> threadid_set;

    duration_type(): call_time(0), real_time(0), call_count(0) {}
};

typedef std::vector<function_node_data>                CALL_STACK;
typedef std::map<unsigned long, CALL_STACK>     STACK_MAP;
typedef std::map<uint32_t, duration_type>              FUNCTION_TIME;



#endif

