#pragma once

template<typename T>
std::string toString(const T & value)
{
    std::ostringstream ostr;
    ostr << value;
    return ostr.str();
}

template<typename T>
T fromString(const std::string & str)
{
    std::istringstream istr(str);
    T value;
    istr >> value;
    return value;
}

template <typename T>
void deleteObject(T * t)
{
    delete t;
}

template <typename T>
void freeObject(T * t)
{
    free(t);
}

template <typename T, typename _Fn>
bool is_all(T _First, T _Last, _Fn _Func)
{
    while (_First != _Last)
        if (!_Func(*_First++))
        {
            return false;
        }
    return true;
}

template <typename T, typename _Fn>
bool is_any(T _First, T _Last, _Fn _Func)
{
    while (_First != _Last)
        if (_Func(*_First++))
        {
            return true;
        }
    return false;
}
