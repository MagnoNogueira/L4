/* MODULE NAME:  sysrsc_mgr.c
 * PURPOSE:
 *    This csc is designed to manage all system-wised resource such as shared
 *    memory and system-wised semaphores.
 *
 * NOTES:
 *    The creation of system-wised resource will be done through SYSINIT program.
 *    SYSRSC_MGR_CreateAndInitiateSystemResources() can be called by SYSINIT only.
 *    SYSRSC_MGR_AttachSystemResources() is needed to call in the initialization
 *    stage of each process to attach the system resource to the calling process
 *    context.
 *
 *    Functions in this file can be linked to any binaries.
 *
 * HISTORY
 *    31/5/2007 - Charlie Chen, Created
 *
 * Copyright(C)      Accton Corporation, 2007
 */

/* INCLUDE FILE DECLARATIONS
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sysrsc_mgr.h"
#include "sys_bld.h"
#include "sys_cpnt.h"
#include "sysfun.h"
#include "backdoor_mgr.h"

#if (SYS_CPNT_BUFFERMGMT == TRUE)
#include "buffermgmt_init.h"
#endif

//#include "sys_callback_init.h"

#if (SYS_CPNT_SWDRV == TRUE)
#include "swdrv.h"
#include "swdrv_init.h"
#endif

#if (SYS_CPNT_AMTRDRV == TRUE)
#include "amtrdrv_mgr.h"
#endif

#if (SYS_CPNT_NMTRDRV == TRUE)
#include "nmtrdrv.h"
#endif

#if (SYS_CPNT_NICDRV == TRUE)
#include "dev_nicdrv_om.h"
#include "lan.h"
#endif

#if (SYS_CPNT_ISCDRV == TRUE)
#include "isc_init.h"
#endif

#if (SYS_CPNT_FLASHDRV == TRUE)
#include "fs_init.h"
#endif
#if (SYS_CPNT_COS == TRUE)
#include "cos_om.h"
#endif
#if (SYS_CPNT_QOS_V2 == TRUE)
#include "rule_ctrl.h"
#include "rule_om.h"
#endif

#if (SYS_CPNT_LEDDRV == TRUE)
#include "leddrv_init.h"
#endif

#if (SYS_CPNT_SYSDRV == TRUE)
#include "sysdrv_init.h"
#endif

/* shumin.wang added for ipfilter 2008 */
#if (SYS_CPNT_MGMT_IP_FLT == TRUE)
#include "mgmt_ip_flt.h"
#endif

#if (SYS_CPNT_SYS_TIME == TRUE)
#include "sys_time_init.h"
#endif

#if (SYS_CPNT_SWDRVL3 == TRUE)
#include "swdrvl3_init.h"
#endif

#if (SYS_CPNT_SWDRVL4 == TRUE)
#include "swdrvl4.h"
#endif

#if (SYS_CPNT_CFGDB == TRUE)
#include "cfgdb_init.h"
#endif

//#include "cmgr.h"

//#if (SYS_CPNT_CFM == TRUE)
//#include "cfm_om.h"
//#endif

#if (SYS_CPNT_PRIVATE_VLAN == TRUE)
#include "pvlan_om.h"
#endif

//#include "netcfg_om_ip.h"

#if (SYS_CPNT_RSPAN == TRUE)
#include "rspan_om.h"
#endif

#if (SYS_CPNT_POE == TRUE)
#include "poedrv_init.h"
#endif

#if (SYS_CPNT_AMTR == TRUE)
#include "amtr_om.h"
#endif

//#if (SYS_CPNT_ERPS == TRUE)
//#include "erps_om.h"
//#endif
#if (SYS_CPNT_TIME_RANGE == TRUE)
#include "time_range_om.h"
#endif /* SYS_CPNT_TIME_RANGE */

extern void SNMP_PMGR_GetShMemInfo(SYSRSC_MGR_SEGID_T *segid, UI32_T *seglen);
extern void SNMP_PMGR_InitiateSystemResources(void);
extern void STKTPLG_SHOM_AttachSystemResources(void);
extern void XSTP_SHARE_OM_AttachSystemResources(void);
extern void VLAN_OM_AttachSystemResources(void);
extern void VLAN_OM_1V_AttachSystemResources(void);
extern void SYSCTRL_XOR_MGR_AttachSystemResources(void);

/*
#define SYSRSC_MGR_DEBUG
*/
/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

#ifdef SYSRSC_MGR_DEBUG
#define SYSRSC_DBG(...) printf(__VA_ARGS__)
#else
#define SYSRSC_DBG(...)
#endif

/* DATA TYPE DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static void   SYSRSC_MGR_AttachAllCSCSystemResources(void);

/* STATIC VARIABLE DECLARATIONS
 */
static SYSRSC_MGR_ShMemHeader_T *shmem_header_p=NULL;
/* EXPORTED SUBPROGRAM BODIES
 */

void SYSRSC_MGR_SetShMemHeaderPtr(SYSRSC_MGR_ShMemHeader_T* ptr)
{
    shmem_header_p=ptr;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : SYSRSC_MGR_AttachSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This api will invoke all XXX_AttachSystemResources provided by CSCs that
 *    need to use system resource.
 *
 * INPUT:
 *    None.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    TRUE  --  Sucess
 *    FALSE --  Error
 *
 * NOTES:
 *    1.This api shall only be called in the initialization stage of all
 *      processes.
 *------------------------------------------------------------------------------
 */
BOOL_T SYSRSC_MGR_AttachSystemResources(void)
{
    UI32_T shmid;
    UI32_T total_shmem_len;

    /* attach the shared memory:
     * It might be a little tricky here because we need to attach the shared
     * memory twice. The first time to attach is to get the shared memory
     * header part so we can know the total length of the shared memory.
     * The second time will attach the whole shared memory.
     */


    /* attach the shared memory header part
     */
    SYSRSC_DBG("Invoke SYSFUN_CreateShMem\r\n");
    if(SYSFUN_OK!=SYSFUN_CreateShMem(SYS_BLD_SHMEM_KEY_SYSRSC,
        sizeof(SYSRSC_MGR_ShMemHeader_T),
        &shmid))
    {
        printf("%s: SYSFUN_CreateShMem fail.\n", __FUNCTION__);
        return FALSE;
    }
    SYSRSC_DBG("Invoke SYSFUN_AttachShMem\r\n");
    shmem_header_p = (SYSRSC_MGR_ShMemHeader_T*)SYSFUN_AttachShMem(shmid);
    if(shmem_header_p==NULL)
    {
        printf("%s: SYSFUN_AttachShMem(1st) fail.\n", __FUNCTION__);
        return FALSE;
    }

    total_shmem_len = SYSRSC_MGR_SHMEM_HEADER_SIZE(shmem_header_p->num_of_seg) +
                      shmem_header_p->total_seg_len;

    SYSRSC_DBG("Invoke SYSFUN_DetachShMem\r\n");
    SYSFUN_DetachShMem(shmem_header_p);

    /* attach the whole shared memory
     */
    SYSRSC_DBG("Invoke SYSFUN_CreateShMem\r\n");
    if(SYSFUN_OK!=SYSFUN_CreateShMem(SYS_BLD_SHMEM_KEY_SYSRSC,
        total_shmem_len,
        &shmid))
    {
        printf("%s: SYSFUN_CreateShMem fail.\n", __FUNCTION__);
        return FALSE;
    }
    SYSRSC_DBG("Invoke SYSFUN_AttachShMem\r\n");
    shmem_header_p = (SYSRSC_MGR_ShMemHeader_T*)SYSFUN_AttachShMem(shmid);
    if(shmem_header_p==NULL)
    {
        printf("%s: SYSFUN_AttachShMem(2nd) fail.\n", __FUNCTION__);
        return FALSE;
    }

    SYSRSC_MGR_AttachAllCSCSystemResources();
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : SYSRSC_MGR_GetShMem
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Get the starting address of the shared memory by segment id.
 *
 * INPUT:
 *    seg_id    --  segment id of the shared memory to be got
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Non-null  -- Success
 *    NULL      -- Error
 * NOTES:
 *    1.The segment id must have been exported by XXX_GetShMemInfo()
 *    2.This api can be used by all processes to get the logical address of the
 *      request seg_id in the context of the calling process.
 *    3.seg_id shall be a enum defined in sysrc_mgr.h
 *------------------------------------------------------------------------------
 */
void* SYSRSC_MGR_GetShMem(SYSRSC_MGR_SEGID_T seg_id)
{
    UI32_T i;

    if(shmem_header_p==NULL)
    {
        printf("%s: shmem_header_p is null\n", __FUNCTION__);
        return NULL;
    }

    if(seg_id>=SYSRSC_MGR_MAX_SEGID)
    {
        SYSFUN_Debug_Printf("%s: invalid seg_id(%d)\n", __FUNCTION__, (int)seg_id);
        return NULL;
    }

    /* look for seg_id
     */
    for(i=0; i<shmem_header_p->num_of_seg; i++)
    {
        if(shmem_header_p->seg_descs[i].seg_id==seg_id)
            break;
    }

    if(i>shmem_header_p->num_of_seg)
    {
        SYSFUN_Debug_Printf("%s: seg_id not found(%d)\n", __FUNCTION__, (int)seg_id);
        return NULL;
    }

    return SYSRSC_MGR_SHMEM_GET_SEG_START_ADDR(shmem_header_p, i);
}

/* LOCAL SUBPROGRAM BODIES
 */

/*------------------------------------------------------------------------------
 * ROUTINE NAME : SYSRSC_MGR_AttachAllCSCSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This function will call XXX_AttachSystemResources() provided by CSCs
 *    that need to attach system resource.
 *
 * INPUT:
 *    None.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    None.
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
static void SYSRSC_MGR_AttachAllCSCSystemResources(void)
{
    /* invoke AttachSystemResources
     */
#if (SYS_CPNT_BUFFERMGMT ==TRUE)
    SYSRSC_DBG("Invoke BUFFERMGMT_INIT_AttachSystemResources\r\n");
    BUFFERMGMT_INIT_AttachSystemResources();
#endif

    SYSRSC_DBG("Invoke SYS_CALLBACK_INIT_AttachSystemResources\r\n");
//    SYS_CALLBACK_INIT_AttachSystemResources();

#if (SYS_CPNT_SWDRV == TRUE)
    SYSRSC_DBG("Invoke SWDRV_INIT_AttachSystemResources\r\n");
    SWDRV_INIT_AttachSystemResources();
#endif

#if (SYS_CPNT_AMTRDRV == TRUE)
    SYSRSC_DBG("%s: Invoke AMTRDRV_MGR_AttachSystemResources()\r\n", __FUNCTION__);
    AMTRDRV_MGR_AttachSystemResources();
#endif

#if (SYS_CPNT_NMTRDRV == TRUE)
    SYSRSC_DBG("%s: Invoke NMTRDRV_AttachSystemResources()\r\n", __FUNCTION__);
    NMTRDRV_AttachSystemResources();
#endif

#if (SYS_CPNT_NICDRV == TRUE)
    SYSRSC_DBG("%s: Invoke DEV_NICDRV_OM_AttachSystemResources()\r\n", __FUNCTION__);
    DEV_NICDRV_OM_AttachSystemResources();
    SYSRSC_DBG("%s: Invoke LAN_AttachSystemResources()\r\n", __FUNCTION__);
    LAN_AttachSystemResources();
#endif

#if (SYS_CPNT_ISCDRV == TRUE)
    SYSRSC_DBG("%s: Invoke ISC_INIT_AttachSystemResources()\r\n", __FUNCTION__);
//    ISC_INIT_AttachSystemResources();
#endif

#if (SYS_CPNT_FLASHDRV == TRUE)
    SYSRSC_DBG("%s: Invoke FLASHDRV_INIT_AttachSystemResources()\r\n", __FUNCTION__);
    FLASHDRV_INIT_AttachSystemResources();
#endif

#if (SYS_CPNT_COS == TRUE)
    SYSRSC_DBG("%s: Invoke COS_OM_AttachSystemResources()\r\n", __FUNCTION__);
    COS_OM_AttachSystemResources();
#endif
#if (SYS_CPNT_QOS_V2 == TRUE)
    SYSRSC_DBG("%s: Invoke RULE_OM_AttachSystemResources()\r\n", __FUNCTION__);
    RULE_OM_AttachSystemResources();
    SYSRSC_DBG("%s: Invoke RULE_CTRL_AttachSystemResources()\r\n", __FUNCTION__);
    RULE_CTRL_AttachSystemResources();
#endif

#if (SYS_CPNT_LEDDRV == TRUE)
    SYSRSC_DBG("%s: Invoke LEDDRV_INIT_AttachSystemResources()\r\n", __FUNCTION__);
    LEDDRV_INIT_AttachSystemResources();
#endif

#if (SYS_CPNT_SYSDRV == TRUE)
    SYSRSC_DBG("%s: Invoke SYSDRV_INIT_AttachSystemResources()\r\n", __FUNCTION__);
    SYSDRV_INIT_AttachSystemResources();
#endif

#if (SYS_CPNT_MGMT_IP_FLT == TRUE)
    SYSRSC_DBG("%s: Invoke MGMT_IP_FLT_AttachSystemResources()\r\n", __FUNCTION__);
    MGMT_IP_FLT_AttachSystemResources();
#endif

#if (SYS_CPNT_SYS_TIME == TRUE)
    SYSRSC_DBG("%s: Invoke SYS_TIME_INIT_AttachSystemResources()\r\n", __FUNCTION__);
    SYS_TIME_INIT_AttachSystemResources();
#endif

#if (SYS_CPNT_SWDRVL3 == TRUE)
    SYSRSC_DBG("%s: Invoke SWDRVL3_INIT_AttachSystemResources()\r\n", __FUNCTION__);
    SWDRVL3_INIT_AttachSystemResources();
#endif

#if (SYS_CPNT_SWDRVL4 == TRUE)
    SYSRSC_DBG("%s: Invoke SWDRVL4_AttachSystemResources()\r\n", __FUNCTION__);
    SWDRVL4_AttachSystemResources();
#endif

#if (SYS_CPNT_POE == TRUE)
    SYSRSC_DBG("%s: Invoke POEDRV_INIT_AttachSystemResources()\r\n", __FUNCTION__);
    POEDRV_INIT_AttachSystemResources();
#endif

#if (SYS_CPNT_CFGDB == TRUE)
    SYSRSC_DBG("%s: Invoke CFGDB_INIT_AttachSystemResources()\r\n", __FUNCTION__);
    CFGDB_INIT_AttachSystemResources();
#endif

//#if (SYS_CPNT_CFM == TRUE)
//    SYSRSC_DBG("%s: Invoke CFM_OM_AttachSystemResources()\r\n", __FUNCTION__);
//    CFM_OM_AttachSystemResources();
//#endif
#if (SYS_CPNT_STACKING == TRUE)
    SYSRSC_DBG("%s: Invoke STKTPLG_SHOM_AttachSystemResources()\r\n", __FUNCTION__);
    STKTPLG_SHOM_AttachSystemResources();
#endif

    SYSRSC_DBG("%s: Invoke CMGR_AttachSystemResources()\r\n", __FUNCTION__);
//    CMGR_AttachSystemResources();

/*maybe 1 is not a good define , but can not find a better one*/
#if (1)
    SYSRSC_DBG("%s: Invoke XSTP_SHARE_OM_AttachSystemResources()\r\n", __FUNCTION__);
//    XSTP_SHARE_OM_AttachSystemResources();
#endif

#if (SYS_CPNT_VLAN == TRUE)
    SYSRSC_DBG("%s: Invoke VLAN_OM_AttachSystemResources()\r\n", __FUNCTION__);
    VLAN_OM_AttachSystemResources();

    #if (SYS_CPNT_PROTOCOL_VLAN == TRUE)
    SYSRSC_DBG("%s: Invoke VLAN_OM_1V_AttachSystemResources()\r\n", __FUNCTION__);
    VLAN_OM_1V_AttachSystemResources();
    #endif
#endif

#if(SYS_CPNT_PRIVATE_VLAN == TRUE)
    SYSRSC_DBG("%s: Invoke PVLAN_OM_AttachSystemResources()\r\n", __FUNCTION__);
    PVLAN_OM_AttachSystemResources();
#endif

#if (SYS_CPNT_SNMP == TRUE)
    SNMP_PMGR_InitiateSystemResources();
#endif

//    NETCFG_OM_IP_AttachSystemResources();

#if (SYS_CPNT_RSPAN == TRUE)
    RSPAN_OM_AttachSystemResources();
#endif

#if (SYS_CPNT_AMTR == TRUE)
    AMTR_OM_AttachSystemResources();
#endif

//#if (SYS_CPNT_ERPS == TRUE)
//    SYSRSC_DBG("%s: Invoke ERPS_OM_AttachSystemResources()\r\n", __FUNCTION__);
//    ERPS_OM_AttachSystemResources();
//#endif

#if (SYS_CPNT_SYSCTRL_XOR == TRUE)
    SYSCTRL_XOR_MGR_AttachSystemResources();
#endif

#if (SYS_CPNT_TIME_RANGE == TRUE)
    TIME_RANGE_OM_AttachSystemResources();
#endif /* SYS_CPNT_TIME_RANGE */

    SYSRSC_DBG("%s: Invoke BACKDOOR_MGR_AttachSystemResources()\r\n", __FUNCTION__);
    BACKDOOR_MGR_AttachSystemResources();
}

