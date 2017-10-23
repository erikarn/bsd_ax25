#ifndef	__OS_TIME_H__
#define	__OS_TIME_H__

#ifdef	__linux__
#define	OS_CLOCK_MONOTONIC_FAST CLOCK_MONOTONIC_RAW
#else
#define	OS_CLOCK_MONOTONIC_FAST CLOCK_MONOTONIC_FAST
#endif

#ifdef __APPLE__
#include "os/apple/time.h"
#else
#include <sys/time.h>
#endif

#endif
