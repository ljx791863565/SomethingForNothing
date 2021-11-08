#ifndef _CONFIG_H_
#define _CONFIG_H_ 

#ifdef __APPLE__ 
#include <AvailabilityMacros.h>
#endif 

#if (__i386 || __amd64) && __GUNC__
#define GNUC__VERSION (__GNUC__ * 10000 + __GUNC__ * 100 + __GUNC_PATCHLEVEL__)

#if (GNUC__VERSION >= 40100) || defined (__clang__)
#define HAVE_ATOMIC
#endif 

#endif 
#endif // _CONFIG_H_ 
