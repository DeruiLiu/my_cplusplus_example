//
// Created by l50015879 on 2020/12/3.
//

#ifndef MYSMALLSTL_VECTOR_H
#define MYSMALLSTL_VECTOR_H
//这个头文件包含一个模板类 vector
//异常保证：
//drstl:vector<T> 满足基本异常保证，部分函数无异常保证，并对以下函数做强异常安全保证
//emplace
//emplace_back
//push_back
//当std::is_nothrow_move_assignable<T>::value == true时，以下函数也满足强异常保证
//reserve
//resize
//insert
#include <initializer_list>


#endif //MYSMALLSTL_VECTOR_H
