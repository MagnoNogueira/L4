/*
 *  l4_main.c
 *  l4_mgr
 *
 *  Created by yehjunying on 2011/6/16.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include "config.h"

/* include all standard header files for all unit test cases .c files
 */
#include <assert.h>
#define ASSERT(eq) 		assert(eq)

#ifndef _countof
#define _countof(ar)    (sizeof(ar)/sizeof(*ar))
#endif


#define SET_MAC(mac, b1, b2, b3, b4, b5, b6) \
    { mac[0]=b1; mac[1]=b2; mac[2]=b3; mac[3]=b4; mac[4]=b5; mac[5]=b6; }

#define SET_IPV4(ip, b1, b2, b3, b4)  \
    { ((UI8_T*)&ip)[0]=b1; ((UI8_T*)&ip)[1]=b2; ((UI8_T*)&ip)[2]=b3; ((UI8_T*)&ip)[3]=b4; }

#define SET_IPV6(ip, w1, w2, w3, w4)  \
    { ((UI32_T*)&ip)[0]=w1; ((UI32_T*)&ip)[1]=w2; ((UI32_T*)&ip)[2]=w3; ((UI32_T*)&ip)[3]=w4; }


////////////////////////////////////////////////////////////////////////////////
// l4_mgr_utest.c
////////////////////////////////////////////////////////////////////////////////

#include "l4_mgr_utest.c"


////////////////////////////////////////////////////////////////////////////////
// include all lost function for sysrsc
////////////////////////////////////////////////////////////////////////////////

#include "netcfg_om_ip.c"
#include "xstp_pom.c"
#include "dos_om.c"
#include "sys_callback_init.c"
int init()
{
    BOOL_T result;

    result = SYSRSC_MGR_CreateAndInitiateSystemResources();
    ASSERT(result == TRUE);

    result = SYSRSC_MGR_AttachSystemResources();
    ASSERT(result == TRUE);

    SYSFUN_Init();

    return 0;
}

int main(int argc,  char *const argv[])
{
    init();

    return L4_MGR_UT_Main(argc, argv);
}

