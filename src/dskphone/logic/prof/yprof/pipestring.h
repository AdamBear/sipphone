#include <stdio.h>
#include <string>

class pipe_string
{
public:
    pipe_string();
    explicit pipe_string(const std::string & cmd);
    ~pipe_string();

    bool open(const std::string & cmd);
    bool close();

    bool operator << (const std::string & data);
    bool operator >> (std::string & data);

    int read(void * buffer, size_t size, size_t count);
    int write(void * buffer, size_t size, size_t count);

    operator FILE * () const
    {
        return file;
    };
    operator bool() const
    {
        return file != 0;
    }

private:
    FILE * file;
};
