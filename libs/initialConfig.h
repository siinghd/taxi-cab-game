#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef CONFIG_H
#define CONFIG_H

#define SO_HOLES 10 
#define SO_TOP_CELLS 40 
#define SO_SOURCES 20*10/10
#define SO_CAP_MIN 1 
#define SO_CAP_MAX 1 
#define SO_TAXI  SO_SOURCES/2 
#define SO_TIMENSEC_MIN 100000000 
#define SO_TIMENSEC_MAX 300000000 
#define SO_TIMEOUT 1
#define SO_DURATION 20
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#endif /* #CONFIG_H */