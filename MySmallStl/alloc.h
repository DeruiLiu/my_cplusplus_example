//
// Created by l50015879 on 2020/12/2.
//

#ifndef MYSMALLSTL_ALLOC_H
#define MYSMALLSTL_ALLOC_H
//这个头文件包含一个类alloc，用于分配和回收内存，以内存池的方式实现

#include <new>
#include <cstddef>
#include <cstdio>

namespace drstl{
    //共用体:freelist
    //采用链表的方式管理内存碎片，分配与回收小内存<=4K的区块
union FreeList{
    union FreeList* next;//指向下一块内存
    char data[1];//储存本块内存的首地址
};

//不同内存范围的上调大小
//0-128每一个freelist+8
//128-256每一个fresslist+16
enum{
    EAlign128 = 8,
    EAlign256 = 16,
    EAlign512 = 32,
    EAlign1024 = 64,
    EAlign2048 = 128,
    EAlign4096 = 256
};

//小对象的内存大小
enum{
    ESmallObjectBytes = 4096
};

//free lists个数
enum{
    EFreeListNumber = 56
};
//空间配置类alloc
//当内存超过4K时，直接调用malloc，free
//当内存较小时，以内存池管理，每次配置一大块内存，并维护对应的自由链表
class alloc
{
private:
    static char * start_free;   //内存池起始位置
    static char * end_free;     //内存池结束位置
    static size_t heap_size;    //申请heap空间附加值大小

    static FreeList* free_list[EFreeListNumber];    //自由链表
public:
    static void* allocate(size_t n);
    static void deallocate(void* p,size_t n);
    static void *reallocate(void *p,size_t old_size,size_t new_size);

private:
    static size_t M_align(size_t bytes);
    static size_t M_round_up(size_t bytes);
    static size_t M_freelist_index(size_t bytes);
    static void* M_refill(size_t n);
    static char* M_chunk_alloc(size_t size,size_t &nobj);
};

//静态成员变量初始化
char* alloc::start_free = 0;
char* alloc::end_free = 0;
size_t alloc::heap_size = 0;
FreeList* alloc::free_list[EFreeListNumber]={
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0
};
//分配大小为n的空间
static void* alloc::allocate(size_t n) {
    FreeList* my_free_list;
    FreeList* result;
    if(n > static_cast<size_t>(ESmallObjectBytes))  return std::malloc(n);
    my_free_list = free_list[M_freelist_index(n)];
    result = my_free_list;//表示这个free_list[i]下侧没有链接空闲块
    if(result == nullptr){
        void *r = M_refill(M_round_up(n));
        return r;
    }
    my_free_list = result->next;
    return result;
}
//释放p指向的大小为n的空间，p不能为0
inline void alloc::deallocate(void *p, size_t n) {
    if(n > static_cast<size_t>(ESmallObjectBytes)){
        std::free(p);
        return;
    }
    FreeList* q = reinterpret_cast<FreeList*>(p);
    FreeList* my_free_list;
    my_free_list = free_list[M_freelist_index(n)];
    q->next = my_free_list;
    my_free_list = q;
}

inline void* alloc::reallocate(void *p, size_t old_size, size_t new_size) {
    deallocate(p,old_size);
    p = allocate(new_size);
    return p;
}

//重新填写free list
//返回一个大小为n的对象，并且有时候会为适当的free list增加节点
void* alloc::M_refill(size_t n) {
    size_t nblock = 10;
    char* c = M_chunk_alloc(n,nblock);//尝试取得nblock个区块作为free list的新节点，nblock是引用
    FreeList* my_free_list;
    FreeList* result, *cur,*next;
    //如果只有一个区块，就把这个区块返回给调用者，free list没有增加新节点
    if(nblock == 1) return c;
    //否则把一个区块给调用者，剩下的纳入free list作为新节点
    my_free_list = free_list[M_freelist_index(n)];
    result = (FreeList *)c;//c返回的是起始的地址
    my_free_list = next = (FreeList *)(c + n);
    //以下将free list的各个节点串起来
    for(size_t i = 1; ; i ++){
        cur = next;
        next = (FreeList*)((char *)next + n);
        if(nblock - 1 == i){
            cur ->next == nullptr;
            break;
        }
        else{
            cur -> next = next;
        }
    }
    return result;
}
//从内存池中取空间给free list使用，条件不允许时，会调整nblock
char* alloc::M_chunk_alloc(size_t size, size_t &nblock) {
    char* result;
    size_t need_bytes = size * nblock;
    size_t pool_bytes = end_free - start_free;
    //如果内存池剩余大小完全满足需求量，返回它
    if(pool_bytes > need_bytes){
        result = start_free;
        start_free += need_bytes;
        return result;
    }
    //如果内存池剩余大小不能完全满足需求量，但至少可以分配一个或一个以上的区块，就返回它
    else if(pool_bytes >= size){
        nblock = pool_bytes/size;
        need_bytes = size * nblock;
        result = start_free;
        start_free += need_bytes;
        return result;
    }

    //如果内存池剩余大小连一个都不能满足
    else{
        if(pool_bytes > 0){
            //如果内存池还有剩余，把剩余的空间加入到free list中,即表示内存池还有一些零头，把这些内存配给适当的free list
            FreeList* my_free_list = free_list[M_freelist_index(pool_bytes)];//试着让内存池中的残余零头还有利用价值
            //调整free list，将内存池中的残余空间编入
            ((FreeList*)start_free)->next = my_free_list;
            my_free_list = (FreeList*)start_free;
        }
        //申请heap空间，用来补充内存池
        size_t bytes_to_get = (need_bytes << 1) + M_round_up(heap_size >> 4);//右移4位表示*16
        start_free = (char *)std::malloc(bytes_to_get);
        if(!start_free){
            //heap空间也不够
            FreeList* my_free_list, *p;
            //试着查找有无未用的块，且区块足够大的free list
            for(size_t i = size; i <= ESmallObjectBytes; i += M_align(i)){
                my_free_list = free_list[M_freelist_index(i)];//比如heap中没有128bytes的块，但是发现申请了一个256的块没有使用
                p = my_free_list;
                if(p){//表示free_list中尚有未用区块
                    //调整free_list以释放未用区块
                    my_free_list = p->next;
                    start_free = (char *)p;
                    end_free = start_free + i;
                    return M_chunk_alloc(size,nblock);
                }
            }
            std::printf("out of memory\n");
            end_free = nullptr;
            throw std::bad_alloc();
        }
        end_free = start_free + bytes_to_get;
        heap_size += bytes_to_get;
        //递归调用自己，为了修正nblock
        return M_chunk_alloc(size,nblock);
    }
}

//bytes对应的上调大小
inline size_t alloc::M_align(size_t bytes) {
        if(bytes <= 128)
            return EAlign128;
        else  if(bytes <= 256)
            return EAlign256;
        else if(bytes <= 512)
            return EAlign512;
        else if(bytes <= 1024)
            return EAlign1024;
        else if(bytes <= 2048)
            return EAlign2048;
        else if(bytes <= 4096)
            return EAlign4096;
}
//将bytes上调至对应区间大小，比如30调至32，50调至64
inline size_t alloc::M_round_up(size_t bytes) {
        return ((bytes + M_align(bytes) - 1) & ~(M_align(bytes) - 1));//啥意思，看不懂
}

//根据区块大小，选择第n个free_lists
inline size_t alloc::M_freelist_index(size_t bytes) {
    if(bytes <= 128)
        return bytes/EAlign128 - 1;
    else  if(bytes <= 256)
        return 16 + (bytes - 128)/EAlign256 - 1;
    else if(bytes <= 512)
        return 24 + (bytes - 256)/EAlign512 - 1;
    else if(bytes <= 1024)
        return 32 + (bytes - 512)/EAlign1024 - 1;
    else if(bytes <= 2048)
        return 40 + (bytes - 1024)/EAlign2048 - 1;
    else if(bytes <= 4096)
        return 48 + (bytes - 2048)/EAlign4096 - 1;
}

}

#endif //MYSMALLSTL_ALLOC_H
