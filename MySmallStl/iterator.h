//
// Created by l50015879 on 2020/12/1.
//
#ifndef MYSMALLSTL_ITERATOR_H
#define MYSMALLSTL_ITERATOR_H

// 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数

#include<cstddef>

namespace drstl{

// 五种迭代器类型,为了使traits有能力萃取处迭代器的种类。
//这个相应的型别一定必须是一个class type，不能只是数值号码类的东西，因为编译器需要依赖它来进行重载决议。
    struct input_iterator_tag {};
    struct output_iterator_tag {};
    struct forward_iterator_tag : public input_iterator_tag {};
    struct bidrectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidrectional_iterator_tag {};


// iterator模板
//为避免写代码时挂一漏万，自行开发的迭代器最好继承自下面这个
    template <class Category, class T,class Distance = ptrdiff_t,
            class Pointer = T*,class Reference = T&> struct iterator
    {
        typedef Category    iterator_category;
        typedef T           value_type;
        typedef Pointer     pointer;
        typedef Reference   reference;
        typedef Distance    difference_type;
    };


//iterator traits
    template <class Iterator>
    struct iterator_traits{
        typedef typename Iterator::iterator_category iterator_category;
        typedef typename Iterator::value_type value_type;
        typedef typename Iterator::pointer pointer;
        typedef typename Iterator::reference reference;
        typedef typename Iterator::difference_type difference_type;
    };

//针对原生指针设计的偏特化版本
template <class T>
struct iterator_traits<T*>{
    typedef random_access_iterator_tag iterator_category;
    typedef T  value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef ptrdiff_t difference_type;
};

template <class T>
struct iterator_traits<const T*>{
    typedef random_access_iterator_tag iterator_category;
    typedef T  value_type;
    typedef const T* pointer;
    typedef const T& reference;
    typedef ptrdiff_t difference_type;
};

//这个函数可以很方便地决定某个迭代器地类型,萃取出迭代器的类型
template <class Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category{
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}

//这个函数可以很方便地决定某个迭代器地distance type，萃取出迭代器的distance type
template <class Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&){
    return static_cast<typename iterator_traits<Iterator>::difference_type *>(0);
}

//萃取某个迭代器的value_type
template <class Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&){
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

//以下函数用于计算迭代器之间的距离
//distance的input_iterator_tag 版本
template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance_dispatch(InputIterator first,InputIterator last,input_iterator_tag){
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while(first != last){
        ++first;
        ++n;
    }
    return n;
}

//distance的random_access_iterator_tag的版本
template <class RandomIteator>
typename iterator_traits<RandomIteator>::difference_type
distance_dispatch(RandomIteator first,RandomIteator last,random_access_iterator_tag){
    return last - first;
}

//针对input_iterator_tag的重载
template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance_dispatch(InputIterator first,InputIterator last){
    return distance_dispatch(first,last,iterator_category(first));
}

//以下函数用于让迭代器前进n个距离
//advance 的input_iterator_tag的版本
template <class InputIterator,class Distance>
void advance_dispatch(InputIterator& i,Distance n,input_iterator_tag){
    while(n --)
        ++i;
}

//advance的bidirectional_iterator_tag的版本,表示两边都能前进
template <class BidirectionalIterator, class Distance>
void advance_dispatch(BidirectionalIterator &i,Distance n,bidrectional_iterator_tag){
    if(n>=0)
        while (n--) ++i;
    else
        while (n++) --i;
}

//advance的random_access_iterator_tag的版本
template <class RandomIterator, class Distance>
void advance_dispatch(RandomIterator& i,Distance n,random_access_iterator_tag){
    i += n;
}

//针对输入两个参数的重载
template <class InputIterator, class Distance>
void advance_dispatch(InputIterator &i,Distance n){
    return advance_dispatch(i,n,iterator_category(i));
}

//模板类：reverse_iterator
//代表反向迭代器，使前进为后退，后退为前进
template <class Iterator>
class reverse_iterator
{
private:
    Iterator current;//记录对应的正向迭代器
public:
    //反向迭代器的5种相应型别
    typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
    typedef typename iterator_traits<Iterator>::value_type  value_type;
    typedef typename iterator_traits<Iterator>::difference_type difference_type;
    typedef typename iterator_traits<Iterator>::pointer pointer;
    typedef typename iterator_traits<Iterator>::reference reference;

    typedef Iterator    iterator_type;
    typedef reverse_iterator<Iterator>  self;

public:
    //构造函数
    reverse_iterator(){}
    explicit reverse_iterator(iterator_type i):current(i){}
    reverse_iterator(const self& rhs):current(rhs.current){}

public:
    //取出对应的正向迭代器
    iterator_type base() const
    {return current;}

    //重载操作符
    reference operator*() const
    {
        //实际对应正向迭代器的前一个位置
        auto tmp = current;
        return *--tmp;
    }

    pointer operator->() const
    {
        return &(operator*());
    }

    //前进++变为后退--
    self &operator++()
    {
        --current;
        return *this;
    }
    self operator++()
    {
        self tmp = *this;
        --current;
        return tmp;
    }
    //后退--变为前进++
    self &operator--()
    {
        ++current;
        return *this;
    }
    self operator--(int)
    {
        self tmp = *this;
        ++current;
        return tmp;
    }

    self &operator+=(difference_type n)
    {
        current -= n;
        return *this;
    }
    self operator+(difference_type n) const
    {
        return self(current - n);
    }
    self &operator-=(difference_type n)
    {
        current += n;
        return *this;
    }
    self operator-(difference_type n) const
    {
        current += n;
        return *this;
    }
    self operator-(difference_type n) const
    {
        return self (current + n);
    }
    reference operator[](difference_type n) const
    {
        return *(*this + n);
    }
};

//重载 operator-
template <class Iterator>
typename reverse_iterator<Iterator>::difference_type
operator -(const reverse_iterator<Iterator>& lhs,const reverse_iterator<Iterator>& rhs)
{
    return rhs.base() - lhs.base();
}

//重载比较操作符
template <class Iterator>
bool operator==(const reverse_iterator<Iterator>& lhs,const reverse_iterator<Iterator>& rhs)
{
    return lhs.base() == rhs.base();
}

template <class Iterator>
bool operator<(const reverse_iterator<Iterator>& lhs,const reverse_iterator<Iterator>& rhs)
{
    return lhs.base()<rhs.base();
}

template <class Iterator>
bool operator!=(const reverse_iterator<Iterator>&lhs,const reverse_iterator<Iterator>& rhs)
{
    return lhs.base()!=rhs.base();
}

template <class Iterator>
bool operator>(const reverse_iterator<Iterator>& lhs,const reverse_iterator<Iterator>& rhs)
{
    return lhs.base()>rhs.base();
}

template <class Iterator>
bool operator<=(const reverse_iterator<Iterator>& lhs,const reverse_iterator<Iterator>& rhs)
{
    return !(rhs < lhs);
}

template <class Iterator>
bool operator>=(const reverse_iterator<Iterator>& lhs,const reverse_iterator<Iterator>& rhs)
{
    return !(lhs < rhs);
}

}

#endif //MYSTL_ITERATOR_H
