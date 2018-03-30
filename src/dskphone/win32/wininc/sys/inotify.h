#pragma once

struct notify_node
{
    int n_wd;
    char * n_dir;
    struct notify_node * next, *previous;
};