/* MODULE NAME:  sysrsc_mgr.h
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
 * HISTORY
 *    31/5/2007 - Charlie Chen, Created
 *
 * Copyright(C)      Accton Corporation, 2007
 */
#ifndef SYSRSC_MGR_H
#define SYSRSC_MGR_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"

/* NAMING CONSTANT DECLARATIONS
 */
/* shared memory segment id enum
 */
typedef enum
{
    SYSRSC_MGR_BUFFERMGMT_SHMEM_SEGID = 0,
    SYSRSC_MGR_SYS_CALLBACK_SHMEM_SEGID,
    SYSRSC_MGR_SWDRV_SHMEM_SEGID,
    SYSRSC_MGR_SYSDRV_SHMEM_SEGID,
    SYSRSC_MGR_SYS_TIME_SHMEM_SEGID,
    SYSRSC_MGR_AMTRDRV_SHMEM_SEGID,
    SYSRSC_MGR_DEV_NICDRV_SHMEM_SEGID,
    SYSRSC_MGR_LANDRV_SHMEM_SEGID,
    SYSRSC_MGR_ISCDRV_SHMEM_SEGID,
    SYSRSC_MGR_FLASHDRV_SHMEM_SEGID,
    SYSRSC_MGR_NMTRDRV_SHMEM_SEGID,
    SYSRSC_MGR_COS_OM_SHMEM_SEGID,
    SYSRSC_MGR_IPFILTER_SHMEM_SEGID,
    SYSRSC_MGR_LEDDRV_SHMEM_SEGID,
    SYSRSC_MGR_SWDRVL3_SHMEM_SEGID,
    SYSRSC_MGR_SWDRVL4_SHMEM_SEGID,
    SYSRSC_MGR_RULE_OM_SHMEM_SEGID,
    SYSRSC_MGR_RULE_CTRL_SHMEM_SEGID,
    SYSRSC_MGR_CFGDB_SHMEM_SEGID,
    SYSRSC_MGR_STKTPLG_SHMEM_SEGID,
    SYSRSC_MGR_SYSCALLBACK_SHMEM_SEGID,
    SYSRSC_MGR_CFM_OM_SHMEM_SEGID,
    SYSRSC_MGR_XSTP_SHMEM_SEGID,
    SYSRSC_MGR_VLAN_SHMEM_SEGID,
    SYSRSC_MGR_SNMP_TRAP_SHMEM_SEGID,
    SYSRSC_MGR_NETCFG_OM_IP_SHMEM_SEGID,
    SYSRSC_MGR_PRIVATE_VLAN_SHMEM_SEGID,
    SYSRSC_MGR_RSPAN_SHMEM_SEGID,
    SYSRSC_MGR_AMTR_MGR_SHMEM_SEGID,
    SYSRSC_MGR_POEDRV_SHMEM_SEGID,
    SYSRSC_MGR_ERPS_OM_SHMEM_SEGID,
    SYSRSC_MGR_BACKDOOR_MGR_SHMEM_SEGID,
    SYSRSC_MGR_SW_WATCHDOG_SHMEM_SEGID,
    SYSRSC_MGR_DOS_SHMEM_SEGID,
    SYSRSC_MGR_LBD_OM_SHMEM_SEGID,
    SYSRSC_MGR_SWCTRL_SHMEM_SEGID,
    SYSRSC_MGR_TIME_RANGE_OM_SHMEM_SEGID,
    SYSRSC_MGR_MAX_SEGID /* for validation check */
} SYSRSC_MGR_SEGID_T;

/* MACRO FUNCTION DECLARATIONS
 */
#define SYSRSC_MGR_SHMEM_HEADER_SIZE(num_of_seg) ((UI32_T)(((SYSRSC_MGR_ShMemHeader_T*)0)->seg_descs) + sizeof(SYSRSC_MGR_ShMemDesc_T)*(num_of_seg))
#define SYSRSC_MGR_SHMEM_GET_SEG_START_ADDR(header_p, index) (void*)(((UI8_T*)(header_p)) + ((header_p)->seg_descs)[index].offset)

/* DATA TYPE DECLARATIONS
 */
/* the structure used to define shared memory segment information
 */
typedef struct
{
    UI32_T offset;    /* offset from the base address of the shared memory */
    UI32_T seg_id;    /* segment id */
} SYSRSC_MGR_ShMemDesc_T;

/* the header of the shared memory
 */
typedef struct
{
    UI32_T num_of_seg;              /* the number of element in seg_descs */
    UI32_T total_seg_len;           /* total segment length(excluding header) */
    SYSRSC_MGR_ShMemDesc_T seg_descs[1]; /* the size of the array is specified by num_of_seg */
} SYSRSC_MGR_ShMemHeader_T;

/* Shared Memory Layout:
 * +--- Shared memory base address
 * |
 * v
 * +-----------------------------+------------------------------+
 * |    SYSRSC_MGR_ShMemHeader_T | Segment1                     |
 * +-----------------------------+------------------------------+
 * | Segment1(cont.)| Segment2                                  |
 * +----------------+-------------------------------------------+
 * | Segment2(cont.)                                            |
 * +------------------------------------------------------------+
 * |   ...                                                      |
 * +------------------------------------------------------------+
 */

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME : SYSRSC_MGR_CreateAndInitiateSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Create system-wised resource and initiate them.
 *
 * INPUT:
 *    None.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    TRUE  --  Success
 *    FALSE --  Error
 *
 * NOTES:
 *    1.This api shall only be called in SYSINT program
 *    2.This api will call functions related to create and initiate system
 *      resource provided by CSCs. Presently, the functions listed below will be
 *      called in this api:
 *         XXX_GetShMemInfo()
 *         XXX_InitiateSystemResources()
 *------------------------------------------------------------------------------
 */
BOOL_T SYSRSC_MGR_CreateAndInitiateSystemResources(void);

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
BOOL_T SYSRSC_MGR_AttachSystemResources(void);

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
void* SYSRSC_MGR_GetShMem(SYSRSC_MGR_SEGID_T seg_id);

#endif    /* End of SYSRSC_MGR_H */

