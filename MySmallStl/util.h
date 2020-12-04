//
// Created by l50015879 on 2020/12/3.
//

#ifndef MYSMALLSTL_UTIL_H
#define MYSMALLSTL_UTIL_H
//这个文件包含一些通用工具，包括move，forward，swap函数以及pair
#include <cstddef>
#include "type_traits.h"


namespace drstl
{
    //move,右值引用,remove_reference用于移除类型的引用，返回原始类型
template <class T>
typename std::remove_reference<T>::type&& move(T&& arg) noexcept
{
    return static_cast<typename std::remove_reference<T>::tyoe&&>(arg);//返回arg的原始类型
}

//forward
template <class T>
T&& forward(typename std::remove_reference<T>::type& arg) noexcept
{
    return static_cast<T&&>(arg);
}

template <class T>
T
}
#endif //MYSMALLSTL_UTIL_H
