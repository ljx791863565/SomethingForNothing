#include "sds.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctypes.h>

#include "zmalloc.h"

// 根据传入的一段内存创建一个sds O(N)
sds sdsnewlen(const void *init, size_t initlen)
{
    sdshdr *sh;
    // 如果传入的c风格字符串为NULL 直接使sds申请内存全为空
    if (init != NULL) {
        sh = zmalloc(SDSHDR + initlen + 1);
    } else {
        sh = zcalloc(SDSHDR + initlen + 1);
    }

    if (sh == NULL) {
        return NULL;
    }

    sh->len = initlen;
    sh->free = 0;
    if (initlen && init) {
        memcpy(sh->buf, init, initlen);
    }
    sh->buf[initlen] = '\0';
    return (char *)sh->buf;
}

// 根据传入的一个c风格字符串创建一个sds O(N)
sds sdsnew(const char *init)
{
    size_t len = (init == NULL) ? 0 : strlen(init);
    return sdsnewlen(init, len);
}

// 创建一个只保存了空字符串 "" 的sds O(N)
sds sdsempty(void)
{
    return sdsnewlen("", 0);
}

// 创建s的副本 o(N)
sds sdsdup(const sds s)
{
    return sdsnewlen(s, sdslen(s));
}

// free给定的sds O(N)
void sdsfree(const sds s)
{
    if (s == NULL) {
        return;
    }
    zfree(s - SDSHDR);
}

// 在不释放s的字符串空间的情况下 将s保存的内容清空 O(1)
void sdsclear(sds s)
{
    sdshdr *sh = (void *)(s - SDSHDR);
    sh->free += sh->len;
    sh->len = 0;
    sh->buf[0] = '\0';
}

// 对外不可见，内部使用的sds分配空间函数 确保为s分配足够再容纳addlen大小的空间
// 规则为原s剩余的free空间可以容纳addlen长度，不分配；
//      否则判断分配s已经存有内容的长度 len + addlen
//          如果这个大小大于SDS_MAX_PREALLOC 则多分配SDS_MAX_PREALLOC大小
//          否则分配为其值2倍
// O(N)
static sds sdsMakeRoomFor(sds s, size_t addlen)
{
    sdshdr *sh, *newsh;
    size_t len, newlen;

    size_t free = sdsavail(s);

    if (free > addlen) {
        return s;
    }

    len = sdslen(s);
    sh = (void *)(s - SDSHDR);
    newlen = len + addlen;

    if (newlen < SDS_MAX_PREALLOC) {
        // 如果新长度小于 SDS_MAX_PREALLOC 
        // 那么为它分配两倍于所需长度的空间
        newlen *= 2;
    } else {
        // 否则，分配长度为目前长度加上 SDS_MAX_PREALLOC
        newlen += SDS_MAX_PREALLOC;
    }

    newsh = zrealloc(sh, SDSHDR + newlen + 1);
    if (newsh == NULL) {
        return NULL;
    }

    newsh->free = newlen - len;

    return newsh->buf;
}
// 对外不可见，内部使用的sds回收空间函数，不会对s中的已有内容做任何修改，回收多余的free空间
// O(N)
static sds sdsRemoveFreeSpace(sds s)
{
    sdshdr *sh = (void *)(s - SDSHDR);
    sh = zrealloc(sh, sh->len + SDSHDR + 1);
    if (sh == NULL) {
        return NULL;
    }
    // 使得该s不会有任何free空间
    sh->free = 0;
    return sh->buf;
}

// 对外不可见，内部使用
// 返回整个s占用内存大小 O(1)
static size_t sdsAllocSize(sds s)
{
    return sdsavail(s) + sdslen(s) + SDSHDR + 1;
}

// 将sds长度扩大到newsize大小，未使用的空间用'\0'填充 O(N)
sds sdsgrowzero(sds s, size_t newsize)
{
    sdshdr *sh = (void *)(s - SDSHDR);
    size_t curlen = sh->len;
    // 如果要扩大的大小比现有len小，什么都不做
    if (newsize <= curlen) {
        return s;
    }

    // 先确保有足够的空间add
    s = sdsMakeRoomFor(s, newsize - curlen);
    if (s == NULL) {
        return NULL;
    }

    // s可能已经被重新分配，需要再次获取sh地址
    sh = (void *)(s - SDSHDR);
    memset(s + curlen, '\0', (newsize - curlen + 1));

    // 总长度为sdsMakeRoomFor之后的len和free之和
    size_t total = sh->len + sh->free;
    // 更新len
    sh->len = newsize;
    // 更新free 
    sh->free = total - sh->len;

    return s;
}

// 追加长度为len的内容到s末尾 O(N)
sds sdscatlen(sds s, const void *t, size_t len)
{
    size_t curlen = sdslen(s);
    s = sdsMakeRoomFor(s, len);
    if (s == NULL) {
        return NULL;
    }

    sdshdr *sh = (void *)(s - SDSHDR);
    memcpy(s+curlen, t, len);
    sh->len = curlen + len;
    sh->free = sh->free - len;

    s[curlen + len] = '\0';

    return s;
}

// 追加字符串t到s末尾 O(N)
sds sdscat(sds s, const char *t)
{
    return sdscatlen(s, t, strlen(t));
}

// 将字符串t的前len个字符复制到s中（覆盖原s内容) O(N)
sds sdscpylen(sds s, const char *t, size_t len)
{
    sdshdr *sh = (void *)(s - SDSHDR);
    size_t total = sh->free + sh->len;

    // 总容量也无法容纳下len，扩容
    if (total < len) {
        // 扩容大小为总共需要能容纳下len大小
        s = sdsMakeRoomFor(s, len - sh->len);
        if (s == NULL) {
            return NULL;
        }
        sh = (void *)(s - SDSHDR);
        total = sh->free + sh->len;
    }

    memcpy(s, t, len);
    s[len] = '\0';

    sh->len = len;
    sh->free = total - len;

    return s;
}

// 将字符串t内容覆盖到s o(N)
sds sdscpy(sds s, const char *t)
{
    return sdscpylen(s, t, strlen(t));
}

// 将s字符串中所有字符转换为小写 O(N)
void sdstolower(sds s)
{
    int i = 0;
    int len = sdslen(s);
    for (i = 0; i < len; ++i) {
        s[i] = tolower(s[i]);
    }
}

// 将s字符串中所有字符转换为大写 O(N)
void sdstoupper(sds s)
{
    int i = 0;
    int len = sdslen(s);
    for (i = 0; i < len; ++i) {
        s[i] = toupper(s[i]);
    }
}

// 将s两端修剪，清除cset中所有的字符 O(M*N) M为cset长度 N为sds长度 
sds sdstrim(sds s, const char cset)
{
    sdshdr = (void *)(s - SDSHDR);
    char *start, *end, *ps, *pe;
    ps = start = s;
    pe = end = s + sdslen(s) -1;

    while (ps <= end && strchr(cset, *ps)) {
        ps++;
    }
    while (pe >= start && strchr(cset, *pe)) {
        pe--;
    }
    // 修剪后的长度
    size_t len = (ps > pe) ? 0 : ((pe - ps) +1);

    // 前移
    if (sh->buf != ps) {
        memmove(sh->buf, ps, len);
    }

    sh->free = sh->free + (sh->len - len);
    sh->len = len;

    return s;
}

// 对按前后索引截取一段字符串 范围为[start, end]
// 当索引x为负数时 表示从尾部向头部方向|x|字符
// s = sdsnew("hello world");
// sdsrange(s, 1, -1);  ==> "ello world";
// sdsrange(s, -5, 10); ==> "worl";
void sdsrange(sds s, int start, int end)
{
    sdshdr *sh = (void *)(s - SDSHDR);
    size_t len = sdslen(s);

    if (start < 0) {
        start += len;
        if (start < 0) {
            start = 0;
        }
    }
    if (end < 0) {
        end += len;
        if (end < 0) {
                end = 0;
        }        
    }

    size_t newlen = (start > len) ? 0 : ((end - start) +1);
    if (newlen != 0) {
        // 确定start和end范围只能在sdslen(s)之内
        if (start >= (signed)(newlen)) {
            newlen = 0;
        } else is (end >= (signed)(newlen)) {
            end = len -1;
            newlen = (start > end) ? 0 : ((end - start) +1);
        }
    } else {
        start = 0;
    }

    // 范围截取
    if (start && newlen) {
        memmove(sh->buf, sh->buf + start, newlen);
    }
    sh->buf[newlen] = '\0';

    sh->free = sh->free + (sh->len - newlen);
    sh->len = newlen;
}