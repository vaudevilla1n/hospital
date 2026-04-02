#pragma once

#define countof(a)	(sizeof(a) / sizeof((a)[0]))
#define lengthof(a)	(countof(a) - 1)
