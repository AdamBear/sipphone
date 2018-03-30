#ifdef __cplusplus
extern "C"
{
#endif

/*
Function: start_profile
Args:     output file name
    start write profile data to <filename>
*/
void start_profile(const char * filename);

/*
Function: stop_profile
Args:     NULL
    stop write profile data.
*/
void stop_profile();

/*
Function: dump_maps
Args:     NULL
    dump /prof/self/maps to profile.
    Should be call in main begin.
    If is not profiling, It will dump in next start_profile.
*/
void dump_maps();

#ifdef __cplusplus
}
#endif
