#include <assert.h>
#include <stdio.h>
#include "disable_syscpnt.h"

#include "sys_callback_init.c"
#include "xstp_pom.c"
#include "netcfg_om_ip.c"
#include "backdoor_mgr.c"

#include "sysrsc_mgr.c"
#include "sysrsc_mgr_private.c"

int main (int argc, const char * argv[])
{
    BOOL_T result;

    result = SYSRSC_MGR_CreateAndInitiateSystemResources();

    assert(result == TRUE);

    return 0;
}
