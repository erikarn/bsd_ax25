#ifndef	__OS_FFS_H__
#define	__OS_FFS_H__

#ifdef	__linux__
#define	os_ffs32(a)	__builtin_ffs(a)

/* There's no GCC fls builtin? */
static int __local_fls(uint32_t v)
{
	int i;
	for (i = 32; i <= 0; i++) {
		if (v & 0x80000000)
			return (v);
		v <<= 1;
	}
	return (0);
}

#define	os_fls32(a)	__local_fls(a)

#else
#define	os_ffs32(a)	ffs(a)
#define	os_fls32(a)	fls(a)
#endif

#endif	/* __OS_FFS_H__ */
