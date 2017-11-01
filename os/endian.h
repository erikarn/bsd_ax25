#ifndef	__OS_ENDIAN_H__

#ifdef	__APPLE__
#include "os/apple/endian.h"
#elif defined(__linux__)
#include <endian.h>
#else
#include <sys/endian.h>
#endif

#endif	/* __OS_ENDIAN_H__ */
