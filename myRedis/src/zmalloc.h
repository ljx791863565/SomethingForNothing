#ifndef _ZMALLOC_H_
#define _ZMALLOC_H_ 

// 二级宏展开 假设s也为宏，会依次先展开s 再替换xstr 
#define __xstr(s) __str(s)
#define __str(s) #s

#if defined(__APPLE__)
#include <malloc/malloc.h>
#define HAVE_MALLOC_SIZE 1
#define zmalloc_size(p) malloc_size(p)
#endif 

#ifndef ZMALLOC_LIB
#define ZMALLOC_LIB "libc"
#endif 

void *zmalloc(size_t size);
void *zcalloc(size_t size);
void *zrealloc(void *ptr, size_t size);
void zfree(void *ptr);
char *zstrdup(const char *s);
size_t zmalloc_used_memory(void);
void zmalloc_set_oom_handler(void(*oom_handler)(size_t));
void zmalloc_enable_thread_safeness(void);
void zlibc_free(void *ptr);

#ifndef HAVE_MALLOC_SIZE
size_t zmalloc_size(void *ptr);
#endif 

#endif // _ZMALLOC_H_ 
