#include <unistd.h>

typedef struct
{
    int run_dev_rm;
    int run_rule_om;
    int run_rule_ctrl;
    int run_rule_mgr;
    int run_backdoor;
} OPTION_T;

OPTION_T l4_options = {1, 1, 1, 1, 0};

////////////////////////////////////////////////////////////////////////////////
// rule_ctrl_utest.c
////////////////////////////////////////////////////////////////////////////////
#include "rule_ctrl_utest.c"


////////////////////////////////////////////////////////////////////////////////
// rule_mgr.c
////////////////////////////////////////////////////////////////////////////////

#define shmem_data      rule_mgr_shmem_data
#define shmem_data_p    rule_mgr_shmem_data_p

#include "rule_mgr.c"
#include "rule_mgr_utest_cases.c"

#undef  shmem_data
#undef  shmem_data_p

#undef  LOG

////////////////////////////////////////////////////////////////////////////////
// l4_mgr.c
////////////////////////////////////////////////////////////////////////////////

#define shmem_data      l4_mgr_shmem_data
#define shmem_data_p    l4_mgr_shmem_data_p

#include "l4_mgr.c"
#include "l4_mgr_utest_cases.c"

#undef  shmem_data
#undef  shmem_data_p


////////////////////////////////////////////////////////////////////////////////
// l4_pmgr.c
////////////////////////////////////////////////////////////////////////////////

#define ipcmsgq_handle l4_pmgr_ipcmsgq_handle


#include "l4_pmgr.c"

#undef ipcmsgq_handle

////////////////////////////////////////////////////////////////////////////////
// cli_api_acl_qosv2.c
// cli_api_l4_ds_qosv2.c
// cli_runcfg.c
////////////////////////////////////////////////////////////////////////////////

#if (SYS_CPNT_DAI == TRUE)
//#include "dai_pmgr.c"
#endif // SYS_CPNT_DAI

#include "cli_task.h"
CLI_TASK_WorkingArea_T *CLI_TASK_GetMyWorkingArea(void)
{
    static CLI_TASK_WorkingArea_T ctrl;

    return &ctrl;
}

#include "cli_api_acl_qosv2.c"
#include "cli_api_l4_ds_qosv2.c"
#include "cli_runcfg.c"

#include "cli_ut_helper.h"

#include "cli_api_acl_qosv2_utest_cases.c"
#include "cli_api_l4_ds_qosv2_utest_cases.c"
#include "cli_runcfg_utest_cases.c"

#include "rule_om_exp_utest_cases.c"

#include "l4_backdoor.c"
#include "l4_bd_utest.c"


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int L4_MGR_UT_Init()
{
    L4_PMGR_Init();

    SYSFUN_RegisterIPCHandler(l4_pmgr_ipcmsgq_handle,
                              L4_MGR_HandleIPCReqMsg);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    TIME_RANGE_PMGR_InitiateProcessResource();
    TIME_RANGE_MGR_EnterMasterMode();

#endif /* SYS_CPNT_TIME_BASED_ACL */
    return 0;
}

int L4_MGR_UT_Main(int argc,  char *const argv[])
{
    char c;

    /* "r:" => ':' mean -r has parameter
     */
    while ((c = getopt(argc, argv, "rocmd")) != -1)
    {
        switch (c) {
            case 'r': /* Skip dev_rm */
                l4_options.run_dev_rm = 0;
                break;

            case 'o': /* Skip rule_om */
                l4_options.run_rule_om = 0;
                break;

            case 'c': /* Skip rule_ctrl */
                l4_options.run_rule_ctrl = 0;
                break;

            case 'm': /* Skip rule_mgr */
                l4_options.run_rule_mgr = 0;
                break;

            case 'd': /* Open l4_backdoor */
                l4_options.run_backdoor = 1;

            default:
                break;
        }
    }

    L4_MGR_UT_Init();

    RULE_CTRL_UT_Main(argc, argv);

    RULE_OM_EXP_UT_RunTestCases();

    RULE_MGR_UT_RunTestCases();

    L4_MGR_UT_RunTestCases();

    CLI_ACL_RunTestCases();
    CLI_DS_RunTestCases();
    
    CLI_RUNCFG_RunTestCases();

    printf("\r\nFinished testing\r\n");

    if (l4_options.run_backdoor)
    {
        char eof = 0;
        char ch[10] = {0};

        RULE_CTRL_UT_Setup();
        RULE_MGR_InitiateSystemResources();
        RULE_MGR_EnterMasterMode();

#if (UT_SUPPORT_DEVRM_BACKDOOR == TRUE)
        DEVRM_SetDebugFlags(0xff);
#endif
        RULE_OM_SetDebugFlag(7);

        while (!eof)
        {
            RULE_TYPE_PRINT("\r\n=============================");
            RULE_TYPE_PRINT("\r\n 1. dev_rm");
            RULE_TYPE_PRINT("\r\n 2. l4_backdoor");
            RULE_TYPE_PRINT("\r\n=============================");
            RULE_TYPE_PRINT("\r\n\tselect:\t");
            BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
            switch (ch[0])
            {
                case '1':
#if (UT_SUPPORT_DEVRM_BACKDOOR == TRUE)
                    _DEVRM_BackdoorMain();
#endif /* UT_SUPPORT_DEVRM_BACKDOOR */
                    break;

                case '2':
                    L4_Backdoor_CallBack();
                    break;

                default:
                    eof = 1;
                    break;
            }
        }
        RULE_CTRL_UT_Teardown();
    }

    return 0;
}


