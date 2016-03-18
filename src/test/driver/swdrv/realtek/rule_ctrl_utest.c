#include <assert.h>

/*=============================================================================
 * SDK
 *=============================================================================*/
#include "common/rt_type.h"
#include "hal/common/halctrl.h"
#include "rtk/acl.h"

#define _UT_CPU_PORT  RTK_MAX_NUM_OF_PORTS

rt_portinfo_t sdk_portinfo;
rt_device_t   sdk_devinfo;
hal_control_t hal_ctrl[RTK_MAX_NUM_OF_UNIT];


/*=============================================================================
 * dev_rm.c
 *=============================================================================*/
#define soc_ndev    RTK_MAX_NUM_OF_UNIT

//#include "dev_rm_lib.h"
//#include "dev_rm.h"
#include "dev_rm.c"
#include "dev_rm_utest_cases.c"
#include "dev_rm_layoff.c"
#include "dev_rm_lib.c"


#include "fp_config.c"

#undef soc_ndev


////////////////////////////////////////////////////////////////////////////////
// dev_rm_pmgr.c
////////////////////////////////////////////////////////////////////////////////

#ifndef BIT_29
#define BIT_29      	(1 << 29)
#endif

#define ipcmsgq_handle  dev_rm_pmgr_ipcmsgq_handle

#include "dev_rm_pmgr.c"

#undef ipcmsgq_handle


////////////////////////////////////////////////////////////////////////////////
// dev_swdrv.c  dev_swdrv_pmgr.c
////////////////////////////////////////////////////////////////////////////////
#include "dev_swdrv.c"
#include "dev_swdrv_pmgr.c"

////////////////////////////////////////////////////////////////////////////////
// cos_om.c
////////////////////////////////////////////////////////////////////////////////

#define shmem_data      cos_om_shmem_data
#define shmem_data_p    cos_om_shmem_data_p

#include "cos_om.c"

#undef  shmem_data
#undef  shmem_data_p


////////////////////////////////////////////////////////////////////////////////
// time_range_pmgr
////////////////////////////////////////////////////////////////////////////////
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
#include "time_range_pmgr.h"
#endif


////////////////////////////////////////////////////////////////////////////////
// rule_om.c
////////////////////////////////////////////////////////////////////////////////

#define shmem_data      rule_om_shmem_data
#define shmem_data_p    rule_om_shmem_data_p

#include "rule_om.c"

#include "rule_om_utest_cases.c"

#undef shmem_data
#undef shmem_data_p

#undef RULE_OM_INST2PTR
#undef RULE_OM_PTR2INST
#undef RULE_OM_VALIDATE_INST
#undef RULE_OM_INDEX_OF

#undef  PRINTF
#undef  LOG


////////////////////////////////////////////////////////////////////////////////
// rule_ctrl.c
////////////////////////////////////////////////////////////////////////////////

#if __WIN32__
#define fflush(stdout)
#endif

#ifndef SWDRV_FIX_STANDALONE_TYPE
#define SWDRV_FIX_STANDALONE_TYPE TRUE /* NO STACKING */
#endif

#define shmem_data      rule_ctrl_shmem_data
#define shmem_data_p    rule_ctrl_shmem_data_p

#ifndef BIT_1
#define BIT_1      	(1 << 1)
#endif

#ifndef BIT_5
#define BIT_5      	(1 << 5)
#endif


#include "rule_ctrl.c"

#include "rule_ctrl_utest_cases.c"

#undef  shmem_data
#undef  shmem_data_p

#undef  PRINTF
#undef  LOG

#define shmem_data      fp_config_shmem_data
#define shmem_data_p    fp_config_shmem_data_p



#undef  shmem_data
#undef  shmem_data_p


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int RULE_CTRL_UT_Init()
{

    unsigned int i;

    DEVRM_PMGR_InitiateProcessResource();
    SYSFUN_RegisterIPCHandler(dev_rm_pmgr_ipcmsgq_handle,
                              DEVRM_HandleIPCReqMsg);

    memset(&hal_ctrl, 0, sizeof(hal_ctrl));
    memset(&sdk_devinfo, 0, sizeof(sdk_devinfo));
    memset(&sdk_portinfo, 0, sizeof(sdk_portinfo));
    rtk_ut_acl_init();

    sdk_portinfo.cpuPort = _UT_CPU_PORT;

    sdk_portinfo.all.min = 0;
    sdk_portinfo.all.max = _UT_CPU_PORT;

    for (i = 0; i <= _UT_CPU_PORT ; i++)
    {
        RTK_PORTMASK_PORT_SET(sdk_portinfo.all.portmask, i);
        sdk_portinfo.all.portNum++;
    }

    sdk_devinfo.pPortinfo = &sdk_portinfo;
#if (SYS_HWCFG_RTK_CHIP_FAMILY == SYS_HWCFG_RTK_CHIP_RTL8380)
    sdk_devinfo.chip_family_id = RTL8380_FAMILY_ID;
#else
    sdk_devinfo.chip_family_id = RTL8390_FAMILY_ID;
#endif

    for (i = 0; i < _countof(hal_ctrl); ++i)
    {
        hal_ctrl[i].pDev_info = &sdk_devinfo;
    }



    return 0;
}

int RULE_CTRL_UT_Main(int argc, char *const argv[])
{
    RULE_CTRL_UT_Init();

    DEVRM_UT_RunTestCaese();

    RULE_OM_UT_RunTestCaese();
    RULE_CTRL_UT_RunTestCaese();

    return 0;
}





