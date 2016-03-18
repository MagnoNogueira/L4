/*
 *  l4_bd_utest.c
 *  l4_mgr
 *
 *  Created by ezio_chen on 2015/5/26.
 *  Copyright 2015. All rights reserved.
 *
 */

/* include all standard header files for all unit test cases .c files
 */
#include <assert.h>
#define ASSERT(eq) 		assert(eq)

#ifndef _countof
#define _countof(ar)    (sizeof(ar)/sizeof(*ar))
#endif

int L4_BD_UTest_Main()
{

    RULE_CTRL_UT_Setup();
    RULE_MGR_InitiateSystemResources();
    RULE_MGR_EnterMasterMode();

    L4_Backdoor_CallBack();

    RULE_CTRL_UT_Teardown();

    return 0;
}

