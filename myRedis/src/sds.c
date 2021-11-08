#include "sds.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zmalloc.h"

// 根据传入的一段内存创建一个sds O(N)
sds sdsnewlen(const void *init, size_t initlen)
{
    sdshdr *sh;
    // 如果传入的c风格字符串为NULL 直接使sds申请内存全为空
    if (init) {
        sh = zmalloc(SDSHDR + initlen + 1);
    } else {
        sh = zcalloc(SDSHDR + initlen + 1);
    }

    if (!sh) {
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