/*
 *  config.h
 *
 *  Created by yehjunying on 2011/4/10.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef _WIN32

#define __WIN32__   1
#define __OSX__     0
#define __LINUX__   0

#else

    #if __APPLE__

    #define __WIN32__   0
    #define __OSX__     1
    #define __LINUX__   0

    #else

    #define __WIN32__   0
    #define __OSX__     0
    #define __LINUX__   1

    #endif // __APPLE__

#endif // _WIN32

#ifdef _MSC_VER
#pragma warning(disable:4100)   /* unreferenced formal parameter */
#pragma warning(disable:4127)   /* conditional expression is constant */
#endif

#define L4_UT

#endif /* __CONFIG_H__ */
