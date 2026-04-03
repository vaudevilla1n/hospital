#pragma once

#include <stdio.h>
#include <stdlib.h>

#define countof(a)	(sizeof(a) / sizeof((a)[0]))
#define lengthof(a)	(countof(a) - 1)

#define lit(s)	lengthof(s), (s)

#define nullptr	NULL

#define assert_ptr(p, alloc_name)	\
	({ void *t = (p); if (!t) { perror(alloc_name); abort(); } t})

#define xcalloc(n, sz)	assert_ptr(calloc(n, sz), "calloc")
