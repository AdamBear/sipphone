/*************************
该头文件模拟C++11标准的右值引用
使用yl::move(obj)表示obj不再使用，使得一些其他对象可以直接使用obj的内存
由此衍生出转移构造函数和转移赋值函数
**************************/

#ifndef __YLSTL_YLMOVE_H__
#define __YLSTL_YLMOVE_H__


namespace yl
{

template<typename Type>
class rref
{
public:
    rref(Type & obj)
        : obj_(obj) {};

    //operator Type&(){return obj_;}
    Type * operator->()
    {
        return &obj_;
    }
    Type & operator*()
    {
        return obj_;
    }
private:
    Type & obj_;
};

template<typename Type>
rref<Type> move(Type & obj)
{
    return rref<Type>(obj);
}

template<typename Type>
rref<Type> move(const Type & obj)
{
    return rref<Type>(const_cast<Type &>(obj));
}


};


#endif


