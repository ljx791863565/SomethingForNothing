#include <stdio.h>
#include <stdlib.h>

void zlibc_free(void *ptr)
{
	free(ptr);
}

#include "zmalloc.h"

#include <string.h>
#include <pthread.h>

#include "config.h"

//已经使用的内存(sizeof(long)大小字节对齐),zmalloc增加 zfree减少 
//真实记录内存的占有大小
static size_t used_memory = 0;
//线程安全标志  1：不安全 0：安全
static int zmalloc_thread_safe = 0;
pthread_mutex_t used_memory_mutex = PTHREAD_MUTEX_INITIALIZER;

#ifdef HAVE_MALLOC_SIZE
#define PREFIX_SIZE (0)
#else 
#if defined(__sun) || defined(__sparc) || defined(__space__)
#define PREFIX_SIZE (sizeof(long long))
#else 
#define PREFIX_SIZE (sizeof(size_t))
#endif

#endif 

#ifdef HAVE_ATOMIC
#define update_zmalloc_stat_add(__n) __sync_add_and_fetch(&used_memory, (__n))
#define update_zmalloc_stat_sub(__n) __sync_sub_and_fetch(&used_memory, (__n))
#else 
#define update_zmalloc_stat_add(__n) do {		\
	pthread_mutex_lock(&used_memory_mutex);		\
	used_memory += (__n);						\
	pthread_mutex_unlock(&used_memory_mutex);	\
} while (0)

#define update_zmalloc_stat_sub(__n) do {		\
	pthread_mutex_lock(&used_memory_mutex);		\
	used_memory -= (__n);						\
	pthread_mutex_unlock(&used_memory_mutex);	\
} while (0)

#endif 

// 保证分配的空间是sizeof(long)的整数倍 如果不是 补齐
// 假定在64位机器上 sizeof(long) - 1 = 7 二进制 0111
// 只有在 _n 为 8 的整数倍时 _n & (sizeof(long) - 1)才为 0
// 否则将 _n 补齐为 8 的整数倍
// 当有多线程环境时 mutex保证数据安全 
#define update_zmalloc_stat_alloc(__n) do {				\
	size_t _n = (__n);									\
	if (_n&(sizeof(long)-1)) {							\
		_n += sizeof(long) - (_n&(sizeof(long) -1));	\
	}													\
	if (zmalloc_thread_safe) {							\
		update_zmalloc_stat_add(_n);					\
	} else {											\
		used_memory += _n;								\
	}													\
} while (0)


#define update_zmalloc_stat_free(__n) do {				\
	size_t _n = (__n);									\
	if (_n&(sizeof(long)-1)) {							\
		_n += sizeof(long) - (_n&(sizeof(long) -1));	\
	}													\
	if (zmalloc_thread_safe) {							\
		update_zmalloc_stat_sub(_n);					\
	} else {											\
		used_memory -= _n;								\
	}													\
} while (0)

static void zmalloc_default_oom(size_t size)
{
	fprintf(stderr, "zmalloc: Out of memory trying to allocate %zu bytes\n", size);
	fflush(stderr);
	abort();
}

static void (*zmalloc_oom_handler)(size_t) = zmalloc_default_oom;

void zmalloc_set_oom_handler(void(*oom_handler)(size_t)) 
{
	zmalloc_oom_handler = oom_handler;
}

void zmalloc_enable_thread_safeness(void)
{
	zmalloc_thread_safe = 1;
}

#ifndef HAVE_MALLOC_SIZE
size_t zmalloc_size(void *ptr)
{
	void *realptr = (char *)ptr - PREFIX_SIZE;
	size_t size = *((size_t *)realptr);
	if (size & (sizeof(long) -1)) {
		size += sizeof(long) - (size & (sizeof(long) -1));
	}	
	return size + PREFIX_SIZE;
}
#endif 

//封装了malloc，区别是在开头固定补充了PREFIX_SIZE大小的头，用于保存size大小
//申请的内存8字节对齐   以size_t大小为单位,复用malloc，可以强制转换类型
//                  zmalloc返回首地址             补齐字节到8字节对齐(如果需要)
//          8bytes     |                        |    
//      +--------------+------------------------+----+
//      | PREFIX_SIZE  |      size              |    |                                                  
//      +--------------+------------------------+----+
//      |
//      malloc返回首地址
// 更新 used_memory
void *zmalloc(size_t size)
{
	void *ptr = malloc(size + PREFIX_SIZE);
	if (!ptr) {
		zmalloc_oom_handler(size);
	}
#ifdef HAVE_MALLOC_SIZE 
	update_zmalloc_stat_alloc(zmalloc_size(ptr));
	return ptr;
#else 
	*((size_t *)ptr) = size;
	update_zmalloc_stat_alloc(size + PREFIX_SIZE);
	return (char *)ptr + PREFIX_SIZE;
#endif 
}

void *zcalloc(size_t size)
{
	void *ptr = calloc(1, size + PREFIX_SIZE);
	if (!ptr) {
		zmalloc_oom_handler(size);
	}
#ifdef HAVE_MALOC_SIZE
	update_zmalloc_stat_alloc(zmalloc_size(ptr));
	return ptr;
#else
	*((size_t *)ptr) = size;
	update_zmalloc_stat_alloc(size + PREFIX_SIZE);
	return (char *)ptr + PREFIX_SIZE;
#endif
}

void *zrealloc(void *ptr, size_t size)
{
#ifndef HAVE_MALLOC_SIZE
	void *realptr;
#endif

	size_t oldsize;
	void *newptr;

	if (ptr == NULL) {
		return zmalloc(size);
	}
#ifdef HAVE_MALLOC_SIZE
	oldsize = *((size_t *)ptr);
	newptr = realloc(ptr, size);
	if (!newptr) {
		zmalloc_oom_handler(size);
	}
	update_zmalloc_stat_free(oldsize);
	update_zmalloc_stat_alloc(zmalloc_size(newptr));
	return newptr;
#else
	realptr = (char *)ptr - PREFIX_SIZE;
	oldsize = *((size_t *)realptr);
	newptr = realloc(realptr, size + PREFIX_SIZE);
	if (!newptr) {
		zmalloc_oom_handler(size);
	}
	*((size_t *)realptr) = size;
	update_zmalloc_stat_free(oldsize);
	update_zmalloc_stat_alloc(size);
	return (char *)realptr + PREFIX_SIZE;
#endif 
}

void zfree(void *ptr)
{
#ifndef HAVA_MALLOC_SIZE
	void *realptr;
	size_t oldsize;
#endif 

	if (ptr == NULL) return;
#ifdef HAVE_MALLOC_SIZE
	update_zmalloc_stat_free(zmalloc_size(ptr));
	free(ptr);
#else
	realptr = (char *)ptr - PREFIX_SIZE;
	oldsize = *((size_t *)realptr);
	update_zmalloc_stat_free(oldsize + PREFIX_SIZE);
	free(realptr);
#endif 
}

char *zstrdup(const char *str)
{
	size_t len = strlen(str) + 1;
	char *p = zmalloc(len);
	memcpy(p, str, len);

	return p;
}

size_t zmalloc_used_memory(void)
{
	size_t um;
	if (zmalloc_thread_safe) {
#ifdef HAVE_ATOMIC
		um = __sync_add_and_fetch(&used_memory, 0);
#else 
		pthread_mutex_lock(&used_memory_mutex);
		um = used_memory;
		pthread_mutex_unlock(&used_memory_mutex);
#endif
	} else {
		um = used_memory;
	}
	return um;
}