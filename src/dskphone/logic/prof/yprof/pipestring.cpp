#include "pipestring.h"



pipe_string::pipe_string()
    : file(0)
{

}

pipe_string::pipe_string(const std::string & cmd)
    : file(0)
{
    open(cmd);
}

pipe_string::~pipe_string()
{
    close();
}

bool pipe_string::open(const std::string & cmd)
{
    if (file == 0)
    {
        file = ::popen(cmd.c_str(), "w");
        return file != 0;
    }
    return false;
}

bool pipe_string::close()
{
    if (file != 0)
    {
        pclose(file);
        file = 0;
    }
}

bool pipe_string::operator<<(const std::string & data)
{
    return write((void *)data.c_str(), sizeof(std::string::value_type), data.size()) == data.size();
}

bool pipe_string::operator>>(std::string & data)
{
    const size_t buffer_size = 1024;
    char buffer[buffer_size];
    size_t size = 0;
    std::string ret;
    while (size = read((void *)buffer, 1, buffer_size))
    {
        ret.append(buffer, size);
    }

    data = ret;
    return !data.empty();
}

int pipe_string::read(void * buffer, size_t size, size_t count)
{
    return fread(buffer, size, count, file) && fflush(file);;
}

int pipe_string::write(void * buffer, size_t size, size_t count)
{
    return fwrite(buffer, size, count, file) && fflush(file);;
}
