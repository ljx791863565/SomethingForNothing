#ifndef _SDS_H_
#define _SDS_H_

// 1Mb
#define SDS_MAX_PREALLOC (1024*1024)

#include <sys/types.h>
#include <stdarg.h>

/*
 * 对比c字符串
 * sds可以高效执行长度计算 (strlen)
 * sds可以高效执行追加操作 (append)
 * 二进制安全 ？ binary-safe
 *      只关心二进制化的字符串,不关心具体格式.只会严格的按照二进制的数据存取。不会妄图已某种特殊格式解析数据
 *
 *      这里的二进制安全是指c风格字符串是以'\0'结尾的 当程序顺序读取到'\0'即表示数据读取完成 字符串里不能有别的空字符  
 *      所以c风格字符串只能用于保存文本数据而不能保存图片视频音频压缩文件等二进制数据
 *      sds将对所有的数据采用二进制方式处理 不对其数据做任何限制过滤或假设 数据存入是什么样读取就是什么样
 *      sds采用len属性来保证写入和读取的数据的结束 不采用c风格的空字符为结束标志 
 *      所以其二进制安全
 *      SDS 数据结构如下
 *
 *      sdshdr test:
 *      +-------+-------+
 *int   |free   |   0   |
 *      +-------+-------+   
 *int   |len    |   13  |
 *      +-------+-------+       --------------------------------------------------------
 *cha[] |buf    |       | -->   | r | e | d | i | s | \0| c | l | u | s | t | e | r | \0|
 *      +-------+-------+       --------------------------------------------------------
 *                          
 *      总共占用13个bit用len标志 剩余可用空间0bit用free标志 数据保存在buf的首地址开始的13bit内存中
 *
 * 追加操作时加快操作速度 降低内存分配次数 代价就是占有了更多的内存 并且这些内存不会被主动释放
 *
 * +------------------------------------------------+---------------------------------------------------+
 * |                C字符串                          |                       SDS                         |
 * +================================================+===================================================+
 * |    获取字符串长度时间复杂度O(N)                    |       获取字符串长度时间复杂度O(1)                    |
 * +------------------------------------------------+---------------------------------------------------+
 * |    API不安全，可能会造成缓冲区溢出                  |   不会造成缓冲区溢出 当缓冲区不够时自动分配内存          |
 * +------------------------------------------------+---------------------------------------------------+
 * |    修改字符串长度N次必须要执行N次内存分配            |   修改字符串长度N次最多需要N次内存分配 速度快            |
 * +------------------------------------------------+---------------------------------------------------+
 * |    只能保存文本数据 即遇到'\0'自动截断              |   二进制安全 可以保存任何二进制数据                     |
 * +------------------------------------------------+---------------------------------------------------+
 * |    完全使用<string.h>库函数                       |   只支持部分<string.h>库函数 比如strcasecmp比较       |
 * +================================================+===================================================+
 */
 
// sds 类型 Simple Dynamic String
typedef char* sds;

typedef struct sdshdr {
    // buf 已占用长度
    int len;
    // buf 剩余可用长度
    int free;
    // c99 柔性数组 不占空间
    char buf[];
} sdshdr;
#define SDSHDR (sizeof(struct sdshdr))

static inline size_t sdslen(const sds s)
{
    sdshdr *sh = (void *)(s - SDSHDR);
    return sh->len;
}

static inline size_t sdsavail(const sds s)
{
    sdshdr *sh = (void *)(s - SDSHDR);
    return sh->free;
}

// 根据传入的一段内存创建一个sds O(N)
sds sdsnewlen(const void *init, size_t initlen);

// 根据传入的一个c风格字符串创建一个sds O(N)
sds sdsnew(const char *init);

// 创建s的副本 o(N)
sds sdsdup(const sds s);

// free给定的sds O(N)
void sdsfree(const sds s);

// 在不释放s的字符串空间的情况下 将s保存的内容清空 O(1)
void sdsclear(sds s);

// 将sds长度扩大到newsize大小，未使用的空间用'\0'填充 O(N)
sds sdsgrowzero(sds s, size_t newsize);

// 追加长度为len的内容到s末尾 O(N)
sds sdscatlen(sds s, const void *t, size_t len);

// 追加字符串t到s末尾 O(N)
sds sdscat(sds s, const char *t);

// 将字符串t的前len个字符复制到s中（覆盖原s内容) O(N)
sds sdscpylen(sds s, const char *t, size_t len);

// 将字符串t内容覆盖到s o(N)
sds sdscpy(sds s, const char *t);

// 将s字符串中所有字符转换为小写 O(N)
void sdstolower(sds s)；

// 将s字符串中所有字符转换为大写 O(N)
void sdstoupper(sds s)；

// 将s两端修剪，清除cset中所有的字符 O(M*N) M为cset长度 N为sds长度 
sds sdstrim(sds s, const char cset);
#endif 