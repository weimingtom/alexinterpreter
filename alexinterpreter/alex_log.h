#ifndef _ALEX_LOG_H_
#define _ALEX_LOG_H_
#include "stdio.h"
#include <stdarg.h>

#define LOG




#ifdef LOG
	#define print printf	
#else
#define print(...)

#endif

#endif