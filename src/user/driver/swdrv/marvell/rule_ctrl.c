/* FUNCTION NAME: RULE_CTRL.H
 * PURPOSE:
 *   manage Logical Rule(om) and Physical Rule(Chip) mapping
 *
 * NOTES:
 *
 * REASON:
 *    DESCRIPTION:
 *    CREATOR:       pttch
 *    Date 2005/5/1
 *
 * Copyright(C)      Accton Corporation, 2005
 */

// TODO: remove function RULE_CTRL_Service by RULE_CTRL_WA_PermitDhcp ....

// FIXME: fix _class_inst_pool_buf to support multi-device
// FIXME: fix _rule_inst_pool_buf to support multi-device
//        to allocate entry from _rule_inst_pool_buf should also include unit/device
//        to replace stack_free_nbr_pce / stack_PCE_bitmap

// TODO: add isc for DEVRM_PMGR_AllocateRule at
//       RULE_CTRL_LocalReserveLogicalRuleIdForGlobalAce !!

// TODO: add upper function to call RULE_CTRL_LocalSetRule to process multiple
//       unit and device issue

// TODO: check ' UI32_T device_id = 3; '

// TODO: check why we need to pass unit/port to
//       RULE_CTRL_LocalAllocateLogicalRuleID ? should be unit/device_id

// TODO: RULE_CTRL_LocalSetRuleForPacketTypeByGlobal --> not all unit !!
//       use rule_storage instead of ace_entry/rule attribute ...

// FIXME: fix DEVRM_MeterInitial by add parameter device_id

// TODO: DEVRM_CHECK_DEVICE_ID should able to return FALSE, not only assert
// TODO: DEVRM_Backdoor_DumpPclCfgTbl mush show all port configuration,
//       include trunk port / craft port

// TODO: pass parameter lookup_num on bind PCL ?
// TODO: replace all 'ingress_flag' parameter for private function on rule ctrl

// TODO: Add test case for RULE_TYPE_GLOBAL_ACE_IP_BCAST, RULE_TYPE_GLOBAL_ACE_VRRP,
//       and RULE_CTRL_PORT_RIP

/* INCLUDE FILE DECLARATIONS
 */
#include <string.h>

#include "sys_type.h"
#include "leaf_es3626a.h"
#include "sys_module.h"
#include "sys_bld.h"

#include "sys_cpnt.h"
#include "sys_hwcfg.h"
#include "sys_dflt.h"

#include "l_stdlib.h"
#include "l_mm.h"
#include "l_inet.h"
#include "l_cvrt.h"
#include "l_pt.h"
#include "sysfun.h"

#include "backdoor_mgr.h"

#include "rule_type.h"
#include "rule_config.h"
#include "rule_lib_array.h"
#include "rule_ctrl.h"
#include "rule_om.h"

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
#include "time_range_type.h"
#include "time_range_om.h"
#endif

#include "swdrv.h"
#include "dev_rm.h"
#include "dev_rm_pmgr.h"
#include "dev_rm_lib.h"
#include "dev_nicdrv_lib.h"
#include "stktplg_pom.h" /* to avoid include swctrl.h */

#if (SYS_CPNT_CRAFT_PORT == TRUE) && (SYS_CPNT_CRAFT_PORT_MODE == SYS_CPNT_CRAFT_PORT_MODE_FRONT_PORT_CRAFT_PORT)
#define RULE_CTRL_HAVE_CRAFT_PORT   TRUE
#else
#define RULE_CTRL_HAVE_CRAFT_PORT   FALSE
#endif

#if (RULE_CTRL_HAVE_CRAFT_PORT == TRUE)
#include "stktplg_board.h"
#endif /* RULE_CTRL_HAVE_CRAFT_PORT */

#include "dev_swdrv_pmgr.h"

#if (SYS_CPNT_STACKING == TRUE)
#include "isc.h"
#endif /* SYS_CPNT_STACKING */

/**************************************************

 NAMING CONSTANT DECLARATIONS

 **************************************************/

#define RULE_CTRL_TIME_OUT                                  1000 /* 10 sec */  /* time to wait for ISC reply */
#define RULE_CTRL_RETRY_TIMES                               2
#define RULE_CTRL_ISC_MAX_NBR_OF_ACTION                     4 /* in-action, out-action, chain-action, meter-action */

#define RULE_CTRL_NO_SHARE_METER_ENTRY_ID                   0xFFFFFFFF /* an in-valid rule id to share meter */

#define RULE_CTRL_MAX_TCAM_POOL_NUMBER                      1

/**************************************************

 MACRO FUNCTION DECLARATIONS

 **************************************************/

/* Use semaphore to protect critical section, but why some apis use,
   and some apis do not use. Becasue exported apis from rule_ctrl only
   called by L4, and L4 will protect critical section. But there
   are exceptions for special requirement to let swdrv call rule_ctrl
   directly and some database need to protect to prevent data corruption.
 */
#define RULE_CTRL_EnterCriticalSection()                    SYSFUN_TakeSem(rule_ctrl_sem_id, SYSFUN_TIMEOUT_WAIT_FOREVER)
#define RULE_CTRL_LeaveCriticalSection()                    SYSFUN_GiveSem(rule_ctrl_sem_id)


#define RULE_CTRL_LeaveCriticalSectionReturnState(state)    \
{                                                           \
    SYSFUN_GiveSem(rule_ctrl_sem_id);                       \
    return state;                                           \
}

#define RULE_CTRL_LeaveCriticalSectionReturnVoid()          \
{                                                           \
    SYSFUN_GiveSem(rule_ctrl_sem_id);                       \
    return;                                                 \
}

// FIXME: RULE_CTRL_IS_BIT_ON was not correct.
//        The return value is not a boolean value.
/* whether bno-th bit in bmp is 1 or 0 (bmp is an UI8_T array, bno is a zero-based bit index) */
#define RULE_CTRL_IS_BIT_ON(bmp, bno)                       (bmp[(bno)/8] & (1 << (7 - (bno)%8)))
#define RULE_CTRL_IS_BIT_OFF(bmp, bno)                      (!RULE_CTRL_IS_BIT_ON(bmp, bno))
#define RULE_CTRL_SET_BIT_ON(bmp, bno)                      {bmp[(bno)/8] |= (1 << (7 - (bno)%8));}
#define RULE_CTRL_SET_BIT_OFF(bmp, bno)                     {bmp[(bno)/8] &= ~(1 << (7 - (bno)%8));}

#define RULE_CTRL_MAX_NBR_OF_PCE_PER_UNIT                   DEVRM_ADPT_MAX_NBR_OF_RULE_PER_UNIT
#define RULE_CTRL_TOTAL_NBR_OF_BYTE_FOR_1BIT_PCE_LIST       ((RULE_CTRL_MAX_NBR_OF_PCE_PER_UNIT + 7) / 8)

#define RULE_CTRL_IS_CONTAINER_TYPE(type)                   \
    (                                                       \
        RULE_TYPE_INST_CONTAINER == type                    \
    )

#define RULE_CTRL_IS_CLASS_INSTANCE_TYPE(type)              \
    (                                                       \
        RULE_TYPE_INST_POLICY_MAP == type ||                \
        RULE_TYPE_INST_CLASS_MAP == type ||                 \
        RULE_TYPE_INST_ACL == type ||                       \
        RULE_TYPE_INST_MF_ACE == type ||                    \
        RULE_TYPE_INST_ACE == type ||                       \
        RULE_TYPE_INST_CPU_IF == type ||                    \
        RULE_TYPE_INST_PBR == type                          \
    )

#define RULE_CTRL_IS_RULE_INSTANCE_TYPE(type)               \
    (                                                       \
        RULE_TYPE_INST_RULE == type                         \
    )

#ifdef RULE_CTRL_INSTANCE_DEBUG_ON
    #define RULE_CTRL_IN_VALIDATE_PTR(in)                   \
        ( RULE_CTRL_IN_Priv_ValidatePtr((RULE_TYPE_INSTANCE_T *)in) )

    #define RULE_CTRL_IN_VALIDATE_RULE_INST(in)             \
        ( RULE_CTRL_IN_Priv_ValidateRuleInstance(in) )

    #define RULE_CTRL_IN_VALIDATE_CLASS_INST(in)            \
        ( RULE_CTRL_IN_Priv_ValidateClassInstance(in) )
#else
    #define RULE_CTRL_IN_VALIDATE_PTR(in_p)
    #define RULE_CTRL_IN_VALIDATE_RULE_INST(in)
    #define RULE_CTRL_IN_VALIDATE_CLASS_INST(in)
#endif /* RULE_CTRL_INSTANCE_DEBUG_ON */


#define PRINTF(fmt, ...)                                                    \
    {                                                                       \
        BACKDOOR_MGR_Printf(fmt, ##__VA_ARGS__);                            \
    }

#define LOG(fmt, ...)                                                       \
    {                                                                       \
        UI32_T flag = RULE_OM_GetDebugFlag();                               \
        RULE_TYPE_LOG(flag & RULE_OM_DEBUG_CTRL_ERR, fmt, ##__VA_ARGS__);   \
    }

#define LOG_IF(interface, fmt, ...)                                         \
    {                                                                       \
        switch ((interface).type)                                           \
        {                                                                   \
            case RULE_TYPE_INTERFACE_UPORT:                                 \
                LOG("direction = %s, iftype = %s, uport = %lu/%lu, "        \
                    fmt,                                                    \
                    RULE_TYPE_DIRECTION_STR((interface).direction),         \
                    RULE_TYPE_INTERFACE_TYPE_STR((interface).type),         \
                    (interface).uport.unit, (interface).uport.port,         \
                    ##__VA_ARGS__);                                         \
                break;                                                      \
            case RULE_TYPE_INTERFACE_TRUNK:                                 \
                LOG("direction = %s, iftype = %s, trunkId = %lu, "          \
                    fmt,                                                    \
                    RULE_TYPE_DIRECTION_STR((interface).direction),         \
                    RULE_TYPE_INTERFACE_TYPE_STR((interface).type),         \
                    (interface).trunk_id,                                   \
                    ##__VA_ARGS__);                                         \
                break;                                                      \
            case RULE_TYPE_INTERFACE_UNIT:                                  \
                LOG("direction = %s, iftype = %s, unit = %lu/%lu, "         \
                    fmt,                                                    \
                    RULE_TYPE_DIRECTION_STR((interface).direction),         \
                    RULE_TYPE_INTERFACE_TYPE_STR((interface).type),         \
                    (interface).unit,                                       \
                    ##__VA_ARGS__);                                         \
                break;                                                      \
            default:                                                        \
                LOG("direction = %s, iftype = %s, " fmt,                    \
                    RULE_TYPE_DIRECTION_STR((interface).direction),         \
                    RULE_TYPE_INTERFACE_TYPE_STR((interface).type),         \
                    ##__VA_ARGS__);                                         \
                break;                                                      \
        }                                                                   \
    }

#define RULE_CTRL_EACH_UNIT_DEVICE(interface_p, fn, ...)                    \
({                                                                          \
    RULE_TYPE_RETURN_TYPE_T result__ = RULE_TYPE_FAIL;                      \
    if (RULE_TYPE_INTERFACE_UPORT == interface_p->type)                     \
    {                                                                       \
        UI32_T module_id__, phy_port__;                                     \
        UI32_T device_id__;                                                 \
        BOOL_T b_result__;                                                  \
        b_result__ = DEV_SWDRV_PMGR_Logical2PhyDevicePortID(                \
                        interface_p->uport.unit,                            \
                        interface_p->uport.port,                            \
                        &module_id__,                                       \
                        &device_id__,                                       \
                        &phy_port__);                                       \
        ASSERT(TRUE == b_result__);                                         \
        result__ = fn(interface_p->uport.unit, device_id__,                 \
                        ##__VA_ARGS__);                                     \
    }                                                                       \
    if (RULE_TYPE_INTERFACE_ALL_UNIT == interface_p->type ||                \
        RULE_TYPE_INTERFACE_TRUNK == interface_p->type ||                   \
             RULE_TYPE_INTERFACE_CPU_PORT == interface_p->type)             \
    {                                                                       \
        UI32_T unit__;                                                      \
        for (unit__ = 1; unit__ <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;      \
                ++ unit__)                                                  \
        {                                                                   \
            UI32_T num_of_device__;                                         \
            UI32_T device_id__;                                             \
            if (!STKTPLG_OM_UnitExist(unit__))                              \
            {                                                               \
                continue;                                                   \
            }                                                               \
            num_of_device__ = RULE_CTRL_GetNumberOfChips(unit__);           \
            for (device_id__ = 0; device_id__ < num_of_device__;            \
                    ++ device_id__)                                         \
            {                                                               \
                result__ = fn(unit__, device_id__,                          \
                                ##__VA_ARGS__);                             \
                if (RULE_TYPE_OK != result__)                               \
                {                                                           \
                    unit__ = SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK+1;          \
                    break;                                                  \
                }                                                           \
            }                                                               \
        }                                                                   \
    }                                                                       \
    if (RULE_TYPE_INTERFACE_CRAFT_PORT == interface_p->type)                \
    {                                                                       \
        UI32_T unit__;                                                      \
        for (unit__ = 1; unit__ <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;      \
                ++ unit__)                                                  \
        {                                                                   \
            UI8_T craft_phy_port__;                                         \
            UI8_T device_id__;                                              \
            if (!STKTPLG_OM_UnitExist(unit__))                              \
            {                                                               \
                continue;                                                   \
            }                                                               \
            result__ = RULE_CTRL_Priv_GetCraftPortInfo(&device_id__,        \
                                           &craft_phy_port__);              \
            if (RULE_TYPE_OK != result__)                                   \
            {                                                               \
                unit__ = SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK+1;              \
                break;                                                      \
            }                                                               \
            result__ = fn(unit__, device_id__,                              \
                            ##__VA_ARGS__);                                 \
            if (RULE_TYPE_OK != result__)                                   \
            {                                                               \
                unit__ = SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK+1;              \
                break;                                                      \
            }                                                               \
        }                                                                   \
    }                                                                       \
    result__;                                                               \
})

#ifndef _countof
#define _countof(_Array) (sizeof(_Array)/sizeof(_Array[0]))
#endif

#ifndef ASSERT
#define ASSERT(eq)
#endif

/**************************************************

 DATA TYPE DECLARATIONS

 **************************************************/

typedef struct
{
    RULE_TYPE_INTERFACE_INFO_T  interface;

    DEVRM_PCL_LOOKUP_NUMBER_T   lookup_num;

    UI32_T                      pcl_id;
    UI32_T                      pcl_id_mask;
} RULE_CTRL_PCL_CFG_T;

typedef struct
{
    RULE_TYPE_SHM_POINTER_T     class_inst_ptr;
    UI16_T                      lookup_rule_cnt[DEVRM_PCL_LOOKUP_MAX + 1];
} RULE_CTRL_IF_ENTRY_T, *RULE_CTRL_IF_ENTRY_PTR_T;

typedef struct
{
    DEVRM_AceEntry_T            rule_pattern;
    DEVRM_ACTION_T              action;
} RULE_CTRL_RULE_ENTRY_T, *RULE_CTRL_RULE_ENTRY_PTR_T;

typedef struct
{
    UI32_T unit;
    UI32_T device_id;
} RULE_CTRL_UNIT_DEVICE_T, *RULE_CTRL_UNIT_DEVICE_PTR_T;

typedef struct
{
    RULE_TYPE_INSTANCE_TYPE_T   type;
    UI32_T                      id;
} RULE_CTRL_INSTANCE_TYPE_ID_T, *RULE_CTRL_INSTANCE_TYPE_ID_PTR_T;

typedef struct
{
    UI32_T                      indent;
} RULE_CTRL_DUMP_CLASS_INSTANCE_PARAM_T, *RULE_CTRL_DUMP_CLASS_INSTANCE_PARAM_PTR_T;

typedef struct
{
    UI32_T total;
    UI32_T each_type[RULE_TYPE_PACKET_MAX_TYPE];
} RULE_CTRL_UTILIZATION_DEVICE_T, *RULE_CTRL_UTILIZATION_DEVICE_PTR_T;

typedef struct
{
    UI32_T total;
    RULE_CTRL_UTILIZATION_DEVICE_T  device[SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT];
} RULE_CTRL_UTILIZATION_UNIT_T, *RULE_CTRL_UTILIZATION_UNIT_PTR_T;

typedef struct
{
    UI32_T total;

    UI32_T each_type[RULE_TYPE_PACKET_MAX_TYPE];

    RULE_CTRL_UTILIZATION_UNIT_T unit[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK];

} RULE_CTRL_UTILIZATION_T, *RULE_CTRL_UTILIZATION_PTR_T;

typedef struct
{
    /** Bit for installing/uninstall rule
     * 1: Install rule;
     * 0: uninstall.
     */
    UI32_T  INSTALL : 1;

    /** Bit for force installing rule
     * 1: Reinstall rule if bound
     * 0: Do nothing
     */
    UI32_T  FORCE_INSTALL : 1;
} RULE_CTRL_OPT_FLAGS_T;

#if (SYS_CPNT_STACKING == TRUE)
typedef enum
{
    RULE_CTRL_DEVRM_SETRULE                 = 0,
    RULE_CTRL_DEVRM_DESTROYRULE             = 1,
    RULE_CTRL_DEVRM_UPDATEMETER             = 2,
    RULE_CTRL_DEVRM_SHAREMETER              = 3,
    RULE_CTRL_DEVRM_GETCOUNTER              = 4,
    RULE_CTRL_DEVRM_CLEARCOUNTER            = 5,
    RULE_CTRL_DEVRM_GETDEVINFO              = 6,
    RULE_CTRL_DEVRM_BINDPCLTOPORT           = 7,
    RULE_CTRL_DEVRM_UNBINDPCLTOPORT         = 8,
    RULE_CTRL_DEVRM_BINDPCLTOTRUNKPORT      = 9,
    RULE_CTRL_DEVRM_UNBINDPCLTOTRUNKPORT    = 10,
    RULE_CTRL_NBR_OF_SERVICE_ID
}RULE_CTRL_ServicesID_T;

#define RULECTRLINFO info.rule_ctrl_info
#define CPUINFO      info.cpu_interface_info

typedef struct
{
    UI32_T              ServiceID;      /* Service ID  */
    UI32_T              unit;           /* stack id (unit number) */
    UI32_T              port;           /* port number */
    UI32_T              option_module;  /* option module */
    UI32_T              option_num_port;
    union
    {
        /*rule ctrl info*/
        struct
        {
            UI32_T                  device_id;
            UI32_T                  src_dev_rule_id;

            union
            {
                UI32_T              phy_port;
                UI32_T              trunk_id;
            };

            union
            {
                UI32_T              selector_index; /* group ID / PCL ID
                                                     */
                UI32_T              pcl_id;
            };

            UI32_T                  vir_group_id;   /* vir_group_id=0 means in the
                                                     * same PCL this VG is lowest priority
                                                     */
            int                     rule_pri;
            UI32_T                  dev_rule_id;

            RULE_TYPE_InOutDirection_T  direction;
            DEVRM_PCL_LOOKUP_NUMBER_T   lookup_num;

            DEVRM_AceEntry_T            ace_entry;

            union
            {
                DEVRM_ACTION_T          dev_action;
                DEVRM_POLICER_METER_T   dev_meter;
            };
        }rule_ctrl_info;

        /*cpu interface*/
        struct
        {
            BOOL_T                     enable_flag;
            RULE_TYPE_Packet2CPUType_T packet_type;
            UI8_T                      da[6];
            UI32_T                     dip;
        }cpu_interface_info;

    }info;
} RULE_CTRL_IscBuf_T;

typedef struct
{
    union
    {
        BOOL_T b_result;
    } return_value;

    union
    {
        struct
        {
            UI32_T    green_cnt;
            UI32_T    yellow_cnt;
            UI32_T    red_cnt;
        } acl_counter;

        DEVRM_DevInfo_T device_info;
    } data;
} RULE_CTRL_IscReplyBuf_T;

typedef BOOL_T (*RULE_CTRL_IscServiceFunc_T) (ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);

#endif /* SYS_CPNT_STACKING */

static BOOL_T
RULE_CTRL_WebAuthRedirHttp(
    BOOL_T enable,
    void *param_p
);

static BOOL_T
RULE_CTRL_WebAuthPermitDhcpClient(
    BOOL_T enable,
    void *param_p
);

static BOOL_T
RULE_CTRL_WebAuthPermitDns(
    BOOL_T enable,
    void *param_p
);

static BOOL_T
RULE_CTRL_WebAuthPermitSip(
    BOOL_T enable,
    void *param_p
);

static BOOL_T
RULE_CTRL_WebAuthDenyIp(
    BOOL_T enable,
    void *param_p
);

static BOOL_T
RULE_CTRL_IpSrcGuardPermitVidSip(
    BOOL_T enable,
    void *param_p
);

static BOOL_T
RULE_CTRL_IpSrcGuardPermitSaVidSip(
    BOOL_T enable,
    void *param_p
);

static BOOL_T
RULE_CTRL_IpSrcGuardDenyIp(
    BOOL_T enable,
    void *param_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IPV6SG_PermitIp6Host
 *------------------------------------------------------------------------------
 * PURPOSE:  Permit IPv6 packet for IPv6SG
 * INPUT:    enable          - TRUE:  permit
 *                             FALSE: no permit
 *           param_p         - pointer to a RULE_CTRL_UnitPortVidSip6_T
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_IPV6SG_PermitIp6Host(
    BOOL_T enable,
    void *param_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IPV6SG_DenyAnyIp6Packet
 *------------------------------------------------------------------------------
 * PURPOSE:  Deny any IPv6 address for IPv6 SG
 * INPUT:    enable          - TRUE:  deny
 *                             FALSE: no deny
 *           param_p         - pointer to a RULE_CTRL_UnitPort_T
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_IPV6SG_DenyAnyIp6Packet(
    BOOL_T enable,
    void *param_p
);

// FIXME: No need this way. Remove it.
typedef BOOL_T (*RULE_CTRL_Service_func_T)(BOOL_T enable, void *param_p);
RULE_CTRL_Service_func_T service_funcs[] =
{
    RULE_CTRL_WebAuthRedirHttp,         /* RULE_CTRL_WEBAUTH_REDIR_HTTP     */
    RULE_CTRL_WebAuthPermitDhcpClient,  /* RULE_CTRL_WEBAUTH_PERMIT_DHCPC   */
    RULE_CTRL_WebAuthPermitDns,         /* RULE_CTRL_WEBAUTH_PERMIT_DNS     */
    RULE_CTRL_WebAuthPermitSip,         /* RULE_CTRL_WEBAUTH_PERMIT_SIP */
    RULE_CTRL_WebAuthDenyIp,            /* RULE_CTRL_WEBAUTH_DENY_IP */
    RULE_CTRL_IpSrcGuardPermitVidSip,   /* RULE_CTRL_IP_SOURCE_GUARD_PERMIT_VID_SIP */
    RULE_CTRL_IpSrcGuardPermitSaVidSip, /* RULE_CTRL_IP_SOURCE_GUARD_PERMIT_SA_VID_SIP */
    RULE_CTRL_IpSrcGuardDenyIp,         /* RULE_CTRL_IP_SOURCE_GUARD_DENY_IP */
    RULE_CTRL_IPV6SG_PermitIp6Host,     /* RULE_CTRL_IPV6_SOURCE_GUARD_PERMIT_VID_SIP6 */
    RULE_CTRL_IPV6SG_DenyAnyIp6Packet,  /* RULE_CTRL_IPV6_SOURCE_GUARD_DENY_IP6 */
    NULL                                /* RULE_CTRL_MAX_SERVICE_ID */
};

typedef struct
{
    UI32_T                  total_rule;
    UI32_T                  free_rule;
} RULE_CTRL_ResourceInfo_T;

typedef struct
{
    UI32_T                  rule_nbr[RULE_TYPE_MAX_NBR_OF_ACL_TYPE];
    UI32_T                  total_nbr;
} RULE_CTRL_RULE_STAT_T;

typedef struct
{
    RULE_TYPE_RETURN_TYPE_T
    (*add_rule)
    (
        const RULE_TYPE_INTERFACE_INFO_T *interface_p,
        UI32_T remapped_pkt_type,
        const DEVRM_AceEntry_T *rule_pattern_p,
        const DEVRM_ACTION_T *dev_action_p,
        BOOL_T is_active
    );

    RULE_TYPE_RETURN_TYPE_T
    (*del_rule)
    (
        const RULE_TYPE_INTERFACE_INFO_T *interface_p,
        UI32_T remapped_pkt_type,
        const DEVRM_AceEntry_T *rule_pattern_p
    );

} RULE_CTRL_OPERATION_T;



/* LOCAL SUBPROGRAM DECLARATIONS
 */
static BOOL_T
RULE_CTRL_LocalInitialize();

#if (SYS_CPNT_BPDU_LOOPBACK_RATELIMIT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalConfigBpduPacket
 *------------------------------------------------------------------------------
 * PURPOSE  : set up rule-pattern and action for BPDU rate-limit rule
 * INPUT    : rate_limit - TRUE: limit rate
 *                        FALSE: not limit rate
 * OUTPUT   : rule_pattern
 *            dev_action
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static BOOL_T RULE_CTRL_LocalConfigBpduPacket(
    BOOL_T rate_limit, 
    DEVRM_AceEntry_T *rule_pattern,
    DEVRM_ACTION_T *dev_action
);
#endif

static BOOL_T
RULE_CTRL_UpdateDevInfo();

static UI32_T
RULE_CTRL_GetNumberOfChips(
    UI32_T unit
);

static BOOL_T
RULE_CTRL_GetLookupNum(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T pcl_id,
    DEVRM_PCL_LOOKUP_NUMBER_T *lookup_num_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_BindPclToPort(
    BOOL_T is_bind,
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T pcl_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_BindPclToGlobal(
    BOOL_T is_bind,
    UI32_T pcl_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_BindPclToCraftPort(
    BOOL_T is_bind,
    UI32_T pcl_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_BindPclToCpuPort(
    BOOL_T is_bind,
    UI32_T pcl_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_BindPclToTrunkPort(
    BOOL_T is_bind,
    UI32_T trunk_id,
    UI32_T pcl_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_BindPclToUserPort(
    BOOL_T is_bind,
    UI32_T unit,
    UI32_T port,
    UI32_T pcl_id
);

static BOOL_T
RULE_CTRL_LocalBindPclToPort(
    UI32_T unit,
    UI32_T device_id,
    UI32_T phy_port,
    UI32_T pcl_id
);

static BOOL_T
RULE_CTRL_LocalUnbindPclToPort(
    UI32_T unit,
    UI32_T device_id,
    UI32_T phy_port,
    UI32_T pcl_id
);

static BOOL_T
RULE_CTRL_LocalBindPclToTrunkPort(
    UI32_T unit,
    UI32_T device_id,
    UI32_T trunk_id,
    UI32_T pcl_id
);

static BOOL_T
RULE_CTRL_LocalUnbindPclToTrunkPort(
    UI32_T unit,
    UI32_T device_id,
    UI32_T trunk_id,
    UI32_T pcl_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_GetPclConfig(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num,
    RULE_CTRL_PCL_CFG_T *pcl_cfg
);

// ???: Remove this function
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_ConvertPacketTypeToPclConfig(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T remapped_pkt_type,
    RULE_CTRL_PCL_CFG_T *pcl_cfg_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_ConvertPacketTypeToAceType(
    UI32_T remapped_pkt_type,
    RULE_TYPE_INTERFACE_TYPE_E *interface_type_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_ConvertPacketTypeToToDirectionLookupNum(
    UI32_T remapped_pkt_type,
    RULE_TYPE_InOutDirection_T *direction_p,
    DEVRM_PCL_LOOKUP_NUMBER_T *lookup_num_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_ConvertPacketTypeToVirtualGroupId(
    UI32_T remapped_pkt_type,
    UI32_T *vir_group_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddCpuIfRule(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T remapped_pkt_type,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p,
    BOOL_T is_active
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalDelCpuIfRule(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T remapped_pkt_type,
    const DEVRM_AceEntry_T *rule_pattern_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddRule_To_Dev(
    UI32_T unit,
    UI32_T device_id,
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T remapped_pkt_type,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *class_inst_obj_p,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p,
    BOOL_T is_active,
    int priority
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddQosRule_To_Dev(
    UI32_T unit,
    UI32_T device_id,
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T remapped_pkt_type,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *class_map_inst_obj_p,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *ace_obj_p,
    UI32_T ace_index,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p,
    UI32_T meter_id,
    const RULE_TYPE_TBParamEntry_T *meter_entry_p,
    UI32_T action_id,
    const RULE_TYPE_Action_T *action_entry_p,
    int priority
);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalUpdateTimeACLRelationRulePerInterface(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI8_T *state_change_list
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalUpdateTimeACLRelationRulePerInterface_ACL(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI8_T *state_change_list,
    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_class_inst_p
);
#endif /* SYS_CPNT_TIME_BASED_ACL */

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(
    RULE_CTRL_OPT_FLAGS_T flag,
    UI32_T remapped_pkt_type,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p,
    const RULE_CTRL_OPTIONS_T *options_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetRuleForPacketTypeByUport(
    RULE_CTRL_OPT_FLAGS_T flag,
    UI32_T unit,
    UI32_T port,
    UI32_T remapped_pkt_type,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetRuleForPacketTypeByTrunk(
    RULE_CTRL_OPT_FLAGS_T flags,
    UI32_T trunk_id,
    UI32_T remapped_pkt_type,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetRuleForPacketTypeByTrunkPort(
    RULE_CTRL_OPT_FLAGS_T flags,
    UI32_T lport,
    UI32_T remapped_pkt_type,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_SetRuleForPacketType_Core(
    RULE_CTRL_OPT_FLAGS_T flags,
    RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T remapped_pkt_type,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p,
    const RULE_CTRL_OPTIONS_T *options_p,
    const RULE_CTRL_OPERATION_T *optbl
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_CnvPacketTypeAndAction2Ace(
    UI32_T remapped_pkt_type,
    const RULE_CTRL_OPTIONS_T *options_p,
    DEVRM_AceEntry_T *rule_pattern_p,
    DEVRM_ACTION_T *dev_action_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetAcl(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI32_T acl_id,
    const RULE_TYPE_Acl_T *acl_entry_p,
    BOOL_T is_add,
    UI32_T time_range_index,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertAce2Dev(
    const RULE_TYPE_Ace_Entry_T *ace_entry_p,
    DEVRM_AceEntry_T *dev_ace_entry_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertMacAce2DevAce(
    const RULE_TYPE_MacAce_Entry_T *mac_ace,
    DEVRM_AceEntry_T *dev_ace
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertIpAce2DevAce(
    const RULE_TYPE_IpAce_Entry_T *ip_ace,
    DEVRM_AceEntry_T *dev_ace
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertIpv6Ace2DevAce(
    const RULE_TYPE_Ipv6Ace_Entry_T *ipv6_ace,
    DEVRM_AceEntry_T *dev_ace
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertIpv6Ace2ExtDevAce(
    const RULE_TYPE_Ipv6Ace_Entry_T *ipv6_ace,
    DEVRM_AceEntry_T *dev_ace
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertIpv6Ace2StdDevAce(
    const RULE_TYPE_Ipv6Ace_Entry_T *ipv6_ace,
    DEVRM_AceEntry_T *dev_ace
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetACL2DEV(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI32_T acl_id,
    const RULE_TYPE_Acl_T *acl_entry_p,
    UI32_T time_range_index,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveACLFromDEV(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI32_T acl_id,
    const RULE_TYPE_Acl_T *acl_entry_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalFreeRulesByRuleGroup(
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalFreeRuleInRuleGroup(
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p,
    RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p
);

static UI32_T
RULE_CTRL_Priv_SetPolicyMapInstance(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p
);

static RULE_TYPE_CLASS_INSTANCE_PTR_T
RULE_CTRL_Priv_GetPolicyMapInstance(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddAce2DEV(
    UI32_T acl_index,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddAce2DevForAclPurpose(
    UI32_T acl_index,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p
);

static BOOL_T
RULE_CTRL_LocalCheckRuleInActiveByTimeRange(
    UI32_T ace_time_range_id,
    UI32_T acl_time_range_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddAce2DevForQosPurpose(
    UI32_T acl_index,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p
);

static void
RULE_CTRL_LocalAddAce2DevForQosPurpose_Hello(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveAceFromDEV(
    UI32_T acl_id,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveAceFromDevForAclPurpose(
    UI32_T acl_index,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveAceFromDevForQosPurpose(
    UI32_T acl_index,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_GetAcePriorityInAcl(
    UI32_T acl_index,
    UI32_T ace_index,
    int *priority_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_GetClassMapElementPriorityInClassMap(
    UI32_T policy_map_index,
    UI32_T class_map_index,
    RULE_TYPE_ClassType_T class_type,
    UI16_T class_id,
    int *priority_p
);

static void
RULE_CTRL_IN_LL_Priv_Remove(
    RULE_TYPE_SHM_POINTER_T list_ptr,
    RULE_TYPE_SHM_POINTER_T node_ptr
);

static void
RULE_CTRL_HotRemoval_FreeUportRule(
    UI32_T ifindex
);

static void
RULE_CTRL_HotRemoval_FreeRuleInstanceOfUnit(
    UI32_T unit,
    RULE_TYPE_SHM_POINTER_T ptr
);

static void
RULE_CTRL_HotRemoval_FreeUnitRuleByIfindex(
    UI32_T ifindex,
    UI32_T unit
);

static void
RULE_CTRL_Priv_MaskAceEntry(
    RULE_TYPE_Ace_Entry_T *ace_entry
);


#pragma mark -
#pragma mark Instance Functions

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_InitInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Initialize instance table
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Initialize before any instance operations
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_InitInstance();

#ifdef RULE_CTRL_INSTANCE_DEBUG_ON
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ValidateClassInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Validate class instance
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only for debug
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ValidateClassInstance(
    const RULE_TYPE_CLASS_INSTANCE_T *in
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ValidateRuleInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Validate rule instance
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only for debug
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ValidateRuleInstance(
    const RULE_TYPE_RULE_INSTANCE_T *in
);
#endif /* RULE_CTRL_INSTANCE_DEBUG_ON */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_IndexOf
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_CTRL_IN_IndexOf(
    RULE_TYPE_INSTANCE_T *in_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_AtIndex
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
void *
RULE_CTRL_IN_AtIndex(
    RULE_TYPE_INSTANCE_TYPE_T type,
    UI32_T index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_IndexOfClassInstance
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_CTRL_IN_IndexOfClassInstance(
    RULE_TYPE_CLASS_INSTANCE_T *in_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_IndexOfRuleInstance
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_CTRL_IN_IndexOfRuleInstance(
    RULE_TYPE_RULE_INSTANCE_T *in_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Ptr2Instance
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
void *
RULE_CTRL_IN_Ptr2Instance(
    RULE_TYPE_SHM_POINTER_T ptr
);

RULE_TYPE_SHM_POINTER_T
RULE_CTRL_IN_Instance2Ptr(
    const RULE_TYPE_INSTANCE_PTR_T in
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_Container_AddInstance(
    RULE_TYPE_CONTAINER_OBJECT_T *this,
    const RULE_TYPE_INSTANCE_PTR_T in
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_Container_RemoveInstance(
    RULE_TYPE_CONTAINER_OBJECT_T *this,
    const RULE_TYPE_INSTANCE_PTR_T in
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Create instance
 * INPUT    : base_addr - base address of entry table
 *            type      - type of instance
 * OUTPUT   : None
 * RETURN   : Pointer of instance entry
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_SHM_POINTER_T
RULE_CTRL_IN_Priv_CreateInstance(
    void *base_addr,
    RULE_TYPE_INSTANCE_TYPE_T type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy instance
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_DestroyInstance(
    RULE_TYPE_SHM_POINTER_T ptr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateClassInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Create class instance
 * INPUT    : type  - type of instance
 * OUTPUT   : None
 * RETURN   : Pointer of instance entry
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_SHM_POINTER_T
RULE_CTRL_IN_Priv_CreateClassInstance(
    RULE_TYPE_INSTANCE_TYPE_T type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateRuleInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Create rule instance
 * INPUT    : type  - type of instance
 * OUTPUT   : None
 * RETURN   : Pointer of instance entry
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_SHM_POINTER_T
RULE_CTRL_IN_Priv_CreateRuleInstance(
    RULE_TYPE_INSTANCE_TYPE_T type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyClassInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy class instance
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : All child instance also be destroied
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_DestroyClassInstance(
    RULE_TYPE_SHM_POINTER_T ptr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyRuleInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy rule instance
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_DestroyRuleInstance(
    RULE_TYPE_SHM_POINTER_T ptr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructContainerObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct container object from class instance
 * INPUT    : in    - source container
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructContainerObject(
    const RULE_TYPE_CONTAINER_T *in,
    RULE_TYPE_CONTAINER_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateContainerObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Create container object
 * INPUT    : type  - type of instance
 * OUTPUT   : out   - instance object
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_CreateContainerObject(
    RULE_TYPE_CONTAINER_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyContainerObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy container
 * INPUT    : this  - instance object
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : All child instance also be destroied
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_DestroyContainerObject(
    RULE_TYPE_CONTAINER_OBJECT_T *this
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct class instance object from pointer of instance entry
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(
    RULE_TYPE_SHM_POINTER_T ptr,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructClassInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct class instance object from class instance
 * INPUT    : in    - source class instance
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructClassInstanceObject(
    const RULE_TYPE_CLASS_INSTANCE_T *in,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateClassInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Create class instance object
 * INPUT    : type  - type of instance
 * OUTPUT   : out   - instance object
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_CreateClassInstanceObject(
    RULE_TYPE_INSTANCE_TYPE_T type,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyClassInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy class instance
 * INPUT    : this  - instance object
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : All child instance also be destroied
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_DestroyClassInstanceObject(
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructRuleInstanceObjectFromPointer
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct rule instance object from pointer of instance entry
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructRuleInstanceObjectFromPointer(
    RULE_TYPE_SHM_POINTER_T ptr,
    RULE_TYPE_RULE_INSTANCE_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructRuleInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct rule instance object from rule instance
 * INPUT    : in    - source rule instance
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructRuleInstanceObject(
    RULE_TYPE_RULE_INSTANCE_T *in,
    RULE_TYPE_RULE_INSTANCE_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateRuleInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Create rule instance object
 * INPUT    : type  - type of instance
 * OUTPUT   : out   - instance object
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_CreateRuleInstanceObject(
    RULE_TYPE_INSTANCE_TYPE_T type,
    RULE_TYPE_RULE_INSTANCE_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyRuleInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy rule instance
 * INPUT    : this  - instance object
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_DestroyRuleInstanceObject(
    RULE_TYPE_RULE_INSTANCE_OBJECT_T *this
);

static void
RULE_CTRL_IN_Priv_Walk(
    RULE_TYPE_INSTANCE_PTR_T in,
    void(*fn)(RULE_TYPE_INSTANCE_PTR_T, void*),
    void *cooike
);

static RULE_TYPE_INSTANCE_PTR_T
RULE_CTRL_IN_Priv_FindIf(
    RULE_TYPE_INSTANCE_PTR_T in,
    BOOL_T (*fn)(RULE_TYPE_INSTANCE_PTR_T, void*),
    void *cookie
);

static UI32_T
RULE_CTRL_IN_Priv_CountIf(
    RULE_TYPE_INSTANCE_PTR_T in,
    BOOL_T (*fn)(RULE_TYPE_INSTANCE_PTR_T, void*),
    void *cookie
);

#pragma mark End of Instance Functions
#pragma mark -

static UI32_T
RULE_CTRL_LocalAllocateLogicalRuleID(
    UI32_T unit,
    UI32_T device_id,
    UI32_T rule_type
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalFreeLogicalRuleID(
    UI32_T unit,
    UI32_T device_id,
    UI32_T rule_type,
    UI32_T rule_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAllocateAndSetRule(
    UI32_T unit,
    UI32_T device_id,
    const RULE_CTRL_PCL_CFG_T *pcl_cfg_p,
    UI32_T vir_group_id,
    int rule_priority,
    UI32_T dev_rule_id,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalCheckPolicyMapOnBinding(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    const RULE_TYPE_PolicyMap_T *policy_map_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetPolicyMap2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    const RULE_TYPE_PolicyMap_T *policy_map_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemovePolicyMap2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    const RULE_TYPE_PolicyMap_T *policy_map_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetClassMap2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    const RULE_TYPE_PolicyMap_T *policy_map_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    UI32_T class_map_index,
    UI32_T meter_id,
    UI32_T action_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveClassMap2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T  policy_map_index,
    const RULE_TYPE_PolicyMap_T *policy_map_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    UI32_T  class_map_index,
    UI32_T  meter_id,
    UI32_T  action_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSyncClassMap2DEV(
    UI32_T policy_map_index,
    UI32_T class_map_index
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetQoSACL2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T acl_id,
    UI32_T meter_id,
    UI32_T action_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveQoSACL2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T acl_id,
    UI32_T meter_id,
    UI32_T action_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetMF2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T mf_index,
    UI32_T meter_id,
    UI32_T action_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveMF2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T mf_index,
    UI32_T meter_id,
    UI32_T action_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertMeter2Dev(
    const RULE_TYPE_TBParamEntry_T *meter_entry,
    DEVRM_ACTION_T *dev_action_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Ifindex_To_Interface(
    UI32_T ifindex,
    RULE_TYPE_INTERFACE_INFO_T *interface_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Interface_To_Ifindex(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T *ifindex_p
);

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalGetMeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : Get associating meter entry with class-map
 * INPUT    : policy_map_index  - policy-map index
 *            class_map_index   - class-map index
 * OUTPUT   : meter_p           - meter entry
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalGetMeterEntry(
    UI32_T policy_map_index,
    UI32_T class_map_index,
    RULE_TYPE_TBParamEntry_T *meter_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalUpdateMeterCIR
 *------------------------------------------------------------------------------
 * PURPOSE  : Update meter's CIR
 * INPUT    : class_instance_p  - class instance object
 *            rule_instance_p   - rule instance object
 *            cookie            - point to RULE_CTRL_SET_CIR_PARAMETER_T
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_FAIL
 * NOTE     : Update meeter for first rule in class-map only.
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalUpdateMeterCIR(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p,
    RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p,
    void *cookie
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalGetInputCount
 *------------------------------------------------------------------------------
 * PURPOSE  : Get input count from device per each flow in class-map
 * INPUT    : class_instance_p  - class instance object
 *            rule_instance_p   - rule instance object
 *            cookie            - no used
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalGetInputCount(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p,
    RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p,
    void *cookie
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalConvertCountToRate
 *------------------------------------------------------------------------------
 * PURPOSE  : Convert count to rate per each flow in class-map
 * INPUT    : class_instance_p  - class instance object
 *            rule_instance_p   - rule instance object
 *            cookie            - point to a UI32_T means total rate
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertCountToRate(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p,
    RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p,
    void *cookie
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalGetBundleInputRate
 *------------------------------------------------------------------------------
 * PURPOSE  : Get input rate per each flow in bundle group
 * INPUT    : ifindex           - ifindex
 *            inout_profile     - direction
 *            policy_map_index  - policy-map index
 *            bundle_group_p    - bundle group
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalGetBundleInputRate(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    RULE_TYPE_BundleGroup_T *bundle_group_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalCalcBundleNewRate
 *------------------------------------------------------------------------------
 * PURPOSE  : Calculatores new bandwidth per flow in a bundle group
 * INPUT    : bundle_group_p    - bundle group
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalCalcBundleNewRate(
    RULE_TYPE_BundleGroup_T *bundle_group_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalUpdateBundleAsNewRate
 *------------------------------------------------------------------------------
 * PURPOSE  : Updates meter CIR to device per flow in bundle group
 * INPUT    : ifindex           - ifindex
 *            inout_profile     - direction
 *            policy_map_index  - policy-map index
 *            bundle_group_p    - bundle group
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalUpdateBundleAsNewRate(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    RULE_TYPE_BundleGroup_T *bundle_group_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalResetBundleRate
 *------------------------------------------------------------------------------
 * PURPOSE  : Reset meter CIR to device per flow in bundle group
 * INPUT    : ifindex           - ifindex
 *            inout_profile     - direction
 *            policy_map_index  - policy-map index
 *            bundle_group_p    - bundle group
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalResetBundleRate(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    RULE_TYPE_BundleGroup_T *bundle_group_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalBundleMain
 *------------------------------------------------------------------------------
 * PURPOSE  : Updates meter CIR to device per flow in bundle group
 * INPUT    : ifindex           - ifindex
 *            inout_profile     - direction
 *            policy_map_index  - policy-map index
 *            bundle_group_p    - bundle group
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalBundleMain(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    RULE_TYPE_BundleGroup_T *bundle_group_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalBundleRateControlProcessTimerEvent
 *------------------------------------------------------------------------------
 * PURPOSE  : Process timer event for BRC (Bundle Rate Control)
 * INPUT    : ifindex   - ifindex
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_LocalBundleRateControlProcessTimerEvent();
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */


static BOOL_T RULE_CTRL_LocalIsNullMac(const UI8_T *mac);

typedef RULE_TYPE_RETURN_TYPE_T
(*RULE_CTRL_RuleInstanceFn_T) (
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p,
    RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p,
    void *cookie
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalEachRuleInClassMap(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    UI32_T class_map_index,
    RULE_CTRL_RuleInstanceFn_T fn,
    void *cookie
);

static UI32_T
RULE_CTRL_LocalGetKeyModeByACLType(
    UI32_T acl_type,
    BOOL_T is_qos
);

static UI32_T
RULE_CTRL_LocalGetRuleSizeByKeyMode(
    UI32_T key_mode
);

/*end pttch add for marvell*/

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertAction2DEV(
    const RULE_TYPE_Action_T *action_entry,
    DEVRM_ACTION_T *dev_action_p
);

static BOOL_T
RULE_CTRL_LocalSetRule(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T vir_group_id,
    int rule_priority,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry,
    const DEVRM_ACTION_T *dev_action_p
);

static BOOL_T
RULE_CTRL_LocalRealSetRule(
    UI32_T  device_id,
    UI32_T  pcl_id,
    UI32_T  vir_group_id,
    int     rule_priority,
    UI32_T  dev_rule_id,
    DEVRM_AceEntry_T    *ace_entry,
    const DEVRM_ACTION_T *dev_action_p
);

static BOOL_T
RULE_CTRL_LocalDestroyRule(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id
);

static BOOL_T
RULE_CTRL_LocalRealDestroyRule(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id
);

static BOOL_T
RULE_CTRL_LocalUpdateMeter(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    const DEVRM_POLICER_METER_T *dev_meter_p
);

static BOOL_T
RULE_CTRL_LocalRealUpdateMeter(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    const DEVRM_POLICER_METER_T *dev_meter_p
);

static BOOL_T
RULE_CTRL_LocalShareMeter(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T src_rule_id,
    UI32_T dev_rule_id
);

static BOOL_T
RULE_CTRL_LocalRealShareMeter(
    UI32_T device_id,
    UI32_T src_rule_id,
    UI32_T dev_rule_id
);

static BOOL_T
RULE_CTRL_LocalGetCounter(
    UI32_T unit,
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    UI32_T *green_cnt_p,
    UI32_T *yellow_cnt_p,
    UI32_T *red_cnt_p
);

static BOOL_T
RULE_CTRL_LocalRealGetCounter(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    UI32_T *green_cnt_p,
    UI32_T *yellow_cnt_p,
    UI32_T *red_cnt_p
);

static BOOL_T
RULE_CTRL_LocalClearCounter(
    UI32_T unit,
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id
);

static BOOL_T
RULE_CTRL_LocalRealClearCounter(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id
);

static BOOL_T
RULE_CTRL_LocalGetDevInfo(
    UI32_T unit,
    DEVRM_DevInfo_T *dev_info_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalGetFirstRuleInstanceOfQos
 *------------------------------------------------------------------------------
 * PURPOSE:  Get first rule instance
 * INPUT:    ifindex - port index
 *           direction - ingress or egress
 *           policy_id - policy-map id
 *           class_id - class-map id
 * OUTPUT:   rule_inst_iter - first rule iterator
 * RETURN:   Error code
 * NOTE: None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalGetFirstRuleInstanceOfQos(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_id,
    UI32_T class_id,
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *rule_inst_iter
);

static BOOL_T
RULE_CTRL_LocalRealGetDevInfo(
    DEVRM_DevInfo_T *dev_info_p
);

static BOOL_T
RULE_CTRL_LocalRealBindPclToPort(
    UI32_T device_id,
    UI32_T phy_port,
    UI32_T pcl_id
);

static BOOL_T
RULE_CTRL_LocalRealUnbindPclToPort(
    UI32_T device_id,
    UI32_T phy_port,
    UI32_T pcl_id
);

static BOOL_T
RULE_CTRL_LocalRealBindPclToTrunkPort(
    UI32_T device_id,
    UI32_T trunk_id,
    UI32_T pcl_id
);

static BOOL_T
RULE_CTRL_LocalRealUnbindPclToTrunkPort(
    UI32_T device_id,
    UI32_T trunk_id,
    UI32_T pcl_id
);

static UI32_T
RULE_CTRL_LocalGetFirstDevRuleIDofOneClassMap(
    const RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p,
    UI32_T unit,
    UI32_T device_id
);

static BOOL_T
RULE_CTRL_Priv_HaveMatchAllClassMapBeReferred(
    UI32_T class_map_index
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_IfEntry_Index_To_Interface(
    UI32_T idx,
    RULE_TYPE_INTERFACE_INFO_PTR_T interface_p
);

static RULE_CTRL_IF_ENTRY_PTR_T
RULE_CTRL_Priv_GetIfEntry(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p
);

static RULE_TYPE_CLASS_INSTANCE_PTR_T
RULE_CTRL_Priv_GetClassInstanceOfRuleEntry(
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *class_inst_obj_p,
    const DEVRM_AceEntry_T *rule_pattern_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_CheckRuleQuota(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    RULE_CONFIG_MVL_PACKET_TYPE_T remapped_pkt_type,
    UI32_T nbr_of_rule
);

RULE_CTRL_RULE_ENTRY_PTR_T
RULE_CTRL_Priv_GetRuleEntryOfRule(
    const RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p
);

BOOL_T
RULE_CTRL_Priv_CreateIfRuleRererence(
    UI32_T unit,
    UI32_T device_id,
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T lookup_num,
    UI32_T pcl_id
);

BOOL_T
RULE_CTRL_Priv_DestroyIfRuleRererence(
    UI32_T unit,
    UI32_T device_id,
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T lookup_num,
    UI32_T pcl_id
);

BOOL_T
RULE_CTRL_Priv_IsMatchTypeId(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie
);

BOOL_T
RULE_CTRL_Priv_IsRuleInstanceMatchUnitDevice(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie
);

BOOL_T
RULE_CTRL_Priv_IsClassInstanceMatchId(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie
);

BOOL_T
RULE_CTRL_Priv_IsClassInstanceMatchTypeId(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie
);

BOOL_T
RULE_CTRL_Priv_IsClassInstanceMatchType(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie
);

BOOL_T
RULE_CTRL_Priv_IsAceInstance(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie
);

static UI32_T
RULE_CTRL_Priv_CountOfAceInstance(
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p,
    UI32_T id
);

static RULE_TYPE_INSTANCE_PTR_T
RULE_CTRL_Priv_GetFirstChildIf(
    RULE_TYPE_INSTANCE_PTR_T in,
    BOOL_T (*fn)(RULE_TYPE_INSTANCE_PTR_T, void*),
    void *cookie
);

static RULE_TYPE_INSTANCE_PTR_T
RULE_CTRL_Priv_GetNextSiblingIf(
    RULE_TYPE_INSTANCE_PTR_T in,
    BOOL_T (*fn)(RULE_TYPE_INSTANCE_PTR_T, void*),
    void *cookie
);

static UI32_T
RULE_CTRL_LocalIsFreePceAvailable(
    UI32_T unit,
    UI32_T device_id,
    UI32_T key_mode,
    UI32_T *rule_size
);

static void
RULE_CTRL_RemapPacketType(
    RULE_TYPE_PacketType_T original_pkt_type,
    UI32_T *remapped_pkt_type
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_GetCraftPortInfo(
    UI8_T *device_id_p,
    UI8_T *device_port_id
);

void
RULE_CTRL_Priv_DumpRuleEntry(
    const RULE_CTRL_RULE_ENTRY_PTR_T rule_entry_p,
    UI32_T indent
);

void
RULE_CTRL_Priv_DumpClassInstance(
    const RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie
);

static void
RULE_CTRL_Priv_GetUtilization(
    RULE_CTRL_UTILIZATION_PTR_T util_p
);

static void
RULE_CTRL_GetTcamPoolCapabilityBitMap(
    UI32_T pool_id,
    RULE_TYPE_TCAM_CAP_BITMAP_T *bit_map_p
);

#if (SYS_CPNT_PBR == TRUE)
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_ConvertPBRAction2Dev(
    const RULE_TYPE_PBR_ACTION_T *action_p,
    DEVRM_ACTION_T *dev_action_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddPbrRule_To_Dev(
    UI32_T unit,
    UI32_T device_id,
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T remapped_pkt_type,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *ace_inst_obj_p,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p,
    int priority
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetPbrACL2DEV(
    RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T vlan,
    UI32_T seq_num,
    UI32_T acl_id,
    const RULE_TYPE_PBR_ACTION_T *action_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemovePbrACL2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T vlan,
    UI32_T seq_num,
    UI32_T acl_id
);
#endif /* SYS_CPNT_PBR */

#if (SYS_CPNT_STACKING == TRUE)
BOOL_T RULE_CTRL_Service_Callback(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
static BOOL_T RULE_CTRL_Slave_Set_Rule(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
static BOOL_T RULE_CTRL_Slave_Destroy_Rule(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
static BOOL_T RULE_CTRL_Slave_Update_Meter(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
static BOOL_T RULE_CTRL_Slave_Share_Meter(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
static BOOL_T RULE_CTRL_Slave_Get_Couner(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
static BOOL_T RULE_CTRL_Slave_Clear_Counter(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
static BOOL_T RULE_CTRL_Slave_Get_Dev_Info(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
static BOOL_T RULE_CTRL_Slave_Bind_Pcl_To_Port(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
static BOOL_T RULE_CTRL_Slave_Unbind_Pcl_To_Port(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
static BOOL_T RULE_CTRL_Slave_Bind_Pcl_To_TrunkPort(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
static BOOL_T RULE_CTRL_Slave_Unbind_Pcl_To_TrunkPort(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
#endif  /* SYS_CPNT_STACKING */


/* STATIC VARIABLE DECLARATIONS
 */
#if (SYS_CPNT_STACKING == TRUE)
/* service function table
 */
static RULE_CTRL_IscServiceFunc_T RULE_CTRL_func_tab[] =
{
    RULE_CTRL_Slave_Set_Rule,               /* RULE_CTRL_DEVRM_SETRULE */
    RULE_CTRL_Slave_Destroy_Rule,           /* RULE_CTRL_DEVRM_DESTROYRULE */
    RULE_CTRL_Slave_Update_Meter,           /* RULE_CTRL_DEVRM_UPDATEMETER */
    RULE_CTRL_Slave_Share_Meter,            /* RULE_CTRL_DEVRM_SHAREMETER */
    RULE_CTRL_Slave_Get_Couner,             /* RULE_CTRL_DEVRM_GETCOUNTER */
    RULE_CTRL_Slave_Clear_Counter,          /* RULE_CTRL_DEVRM_CLEARCOUNTER */
    RULE_CTRL_Slave_Get_Dev_Info,           /* RULE_CTRL_DEVRM_GETDEVINFO */
    RULE_CTRL_Slave_Bind_Pcl_To_Port,       /* RULE_CTRL_DEVRM_BINDPCLTOPORT */
    RULE_CTRL_Slave_Unbind_Pcl_To_Port,     /* RULE_CTRL_DEVRM_UNBINDPCLTOPORT */
    RULE_CTRL_Slave_Bind_Pcl_To_TrunkPort,  /* RULE_CTRL_DEVRM_BINDPCLTOTRUNKPORT */
    RULE_CTRL_Slave_Unbind_Pcl_To_TrunkPort,/* RULE_CTRL_DEVRM_UNBINDPCLTOTRUNKPORT */
};
#endif  /* SYS_CPNT_STACKING */

// FIXME: Remove this constant
#define RULE_CTRL_GLOBAL_RULE_CNT                                           \
    (RULE_TYPE_GLBL_ACE_LST(RULE_TYPE_LST_RULE_CNT_SUM) 0)

#define RULE_CTRL_CRAFT_PORT_RULE_CNT                                       \
    (RULE_TYPE_CRAFT_PORT_ACE_LST(RULE_TYPE_LST_RULE_CNT_SUM) 0)

#define RULE_CTRL_NORMAL_PORT_RULE_CNT                                      \
    (RULE_TYPE_DFLT_ACE_LST(RULE_TYPE_LST_RULE_CNT_SUM) 0)


#define RULE_CTRL_SAFT_ARRAY_INDEX_OF(_StrArray, _Index)                    \
    (_Index < _countof(_StrArray) ? _StrArray[_Index] : "(invalid)")

#define RULE_CTRL_PACKET_TYPE_STR(pk_type)                                  \
    (RULE_CTRL_SAFT_ARRAY_INDEX_OF(rule_ctrl_packet_type_str, pk_type))

#define RULE_CTRL_REMAPPED_PACKET_TYPE_STR(remapped_pkt_type)               \
    (RULE_CTRL_SAFT_ARRAY_INDEX_OF(rule_ctrl_remapped_pkt_type_str, remapped_pkt_type))

#pragma mark -
#pragma mark Shared memory data definition
#pragma mark typedef struct {

typedef struct
{
    SYSFUN_DECLARE_CSC_ON_SHMEM

    L_PT_ShMem_Descriptor_T     _container_pool;
    L_PT_ShMem_Descriptor_T     _class_inst_pool;
    L_PT_ShMem_Descriptor_T     _rule_inst_pool;

    /* ACL (ingress and egress)
     */
    RULE_TYPE_CONTAINER_T       _container_pool_buf[SYS_ADPT_TOTAL_NBR_OF_LPORT * (2)];

    /* The worst case to use the class instance will be
     * policy-map --> class-map --> ACL --> ACE --> rule.
     * One rule will need 4 additional class instances.
     * Excludes policy-map, it also need triple class instance for a ACE.
     * But it waste memory, so here only prepare 1.5 times
     * class instance for all system.
     */
    RULE_TYPE_CLASS_INSTANCE_T  _class_inst_pool_buf[(RULE_CTRL_MAX_NBR_OF_PCE_PER_UNIT * 3 / 2) +
                                                     (SYS_ADPT_TOTAL_NBR_OF_LPORT * 2)];
    //
    // FIXME: Must change to many pool per unit and device. Not a big pool
    //
    RULE_TYPE_RULE_INSTANCE_T   _rule_inst_pool_buf[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
                                                    SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
                                                    RULE_CTRL_MAX_NBR_OF_PCE_PER_UNIT];

    DEVRM_DevInfo_T             dev_info[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK];

    enum
    {
        RULE_CTRL_UNIT_1_INDEX = 0,

        RULE_CTRL_UPORT_1_INDEX = RULE_CTRL_UNIT_1_INDEX + 1,
        RULE_CTRL_TOTAL_NBR_OF_UPORT = (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
                                        SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT),

        RULE_CTRL_TRUNK_PORT_1_INDEX = (RULE_CTRL_UPORT_1_INDEX +
                                        RULE_CTRL_TOTAL_NBR_OF_UPORT),
        RULE_CTRL_TOTAL_NBR_OF_TRUNK_PORT = SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM,

        RULE_CTRL_CRAFT_PORT_1_INDEX = (RULE_CTRL_TRUNK_PORT_1_INDEX +
                                        RULE_CTRL_TOTAL_NBR_OF_TRUNK_PORT),
        RULE_CTRL_TOTAL_NBR_OF_CRAFT_PORT = 1,

        RULE_CTRL_CPU_PORT_1_INDEX = (RULE_CTRL_CRAFT_PORT_1_INDEX +
                                      RULE_CTRL_TOTAL_NBR_OF_CRAFT_PORT),
        RULE_CTRL_TOTAL_NBR_OF_CPU_PORT = 1,

        /** Each direction have
         *   global(1) + uport + trunk port + cpu port(1) + craft port(1)
         *
         * If #ifEntryPerDirection == 10 Then
         *   index of global = 0, 10
         *   first index of uport = 1, 11
         */
        RULE_CTRL_TOTAL_NBR_OF_IF_ENTRY_PER_DIRECTION = (1 +
                                                         RULE_CTRL_TOTAL_NBR_OF_UPORT +
                                                         RULE_CTRL_TOTAL_NBR_OF_TRUNK_PORT +
                                                         RULE_CTRL_TOTAL_NBR_OF_CPU_PORT +
                                                         RULE_CTRL_TOTAL_NBR_OF_CRAFT_PORT),
    };

    RULE_CTRL_IF_ENTRY_T        if_table[RULE_CTRL_TOTAL_NBR_OF_IF_ENTRY_PER_DIRECTION * 2];

    //
    // FIXME: 1-to-1 map from _rule_inst_pool_buf
    //
    RULE_CTRL_RULE_ENTRY_T      rule_table[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
                                           SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
                                           RULE_CTRL_MAX_NBR_OF_PCE_PER_UNIT];

    /** TCAM mode
     * No use now. TODO: Remove it later.
     */
    UI32_T  global_tcam_key_mode;

    /* Should be replace by _rule_inst_pool_buf
     */
    UI32_T  stack_free_nbr_pce[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK]
                              [SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT];
    UI8_T   stack_PCE_bitmap[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK]
                            [SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT]
                            [RULE_CTRL_TOTAL_NBR_OF_BYTE_FOR_1BIT_PCE_LIST];

#if (SYS_CPNT_ERPS == TRUE)
    UI8_T   raps_pbmp[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK][SYS_ADPT_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST];
    UI8_T   raps_pcnt;
#endif /* SYS_CPNT_ERPS */

#if (SYS_CPNT_BPDU_LOOPBACK_RATELIMIT == TRUE)
    BOOL_T  bpdu_loopback_status_per_port[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK][SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT];
    BOOL_T  bpdu_loopback_status_per_trunk[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK][SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM];
#endif
} RULE_CTRL_ShmemData_T;

#pragma mark }RULE_CTRL_ShmemData_T;
#pragma mark -

/* semaphore id
 */
static UI32_T   rule_ctrl_sem_id;

static RULE_CTRL_ShmemData_T   *shmem_data_p;

static const char *rule_ctrl_packet_type_str[] =
{
    RULE_TYPE_PacketType_STRING_ARRAY
};

static const char *rule_ctrl_remapped_pkt_type_str[] =
{
#define PACKET_TYPE(pktType, ifType, remapAceType, quota)  #pktType,

    RULE_CONFIG_PACKET_TYPE_TABLE

#undef PACKET_TYPE
};

/*------------------------------------------------------------------------------
 * FUNCTION NAME: RULE_CTRL_AttachSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE: Attach system resource for RULE_CTRL in the context of the calling
 *          process.
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_AttachSystemResources(void)
{
    shmem_data_p = (RULE_CTRL_ShmemData_T*)SYSRSC_MGR_GetShMem(SYSRSC_MGR_RULE_CTRL_SHMEM_SEGID);
    SYSFUN_GetSem(SYS_BLD_SYS_SEMAPHORE_KEY_RULE_CTRL, &rule_ctrl_sem_id);
}

/* FUNCTION NAME: RULE_CTRL_GetShMemInfo
 *------------------------------------------------------------------------------
 * PURPOSE: Get share memory info
 *------------------------------------------------------------------------------
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_GetShMemInfo(SYSRSC_MGR_SEGID_T *segid_p, UI32_T *seglen_p)
{
    *segid_p = SYSRSC_MGR_RULE_CTRL_SHMEM_SEGID;
    *seglen_p = sizeof(RULE_CTRL_ShmemData_T);
}

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_InitiateSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE  : Initial resource
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_InitiateSystemResources()
{
    shmem_data_p = (RULE_CTRL_ShmemData_T *)SYSRSC_MGR_GetShMem(SYSRSC_MGR_RULE_CTRL_SHMEM_SEGID);
    SYSFUN_INITIATE_CSC_ON_SHMEM(shmem_data_p);

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Create_InterCSC_Relation
 *------------------------------------------------------------------------------
 * PURPOSE  : This function initializes all function pointer registration operations.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_Create_InterCSC_Relation()
{

}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_EnterMasterMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will make the rule ctrl enter the master mode.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_EnterMasterMode()
{
    BOOL_T b_result;

    DEVRM_PMGR_FieldInit();

    b_result = RULE_CTRL_LocalInitialize();

    if (TRUE != b_result)
    {
        PRINTF("initialize error");
    }

    SYSFUN_ENTER_MASTER_MODE_ON_SHMEM(shmem_data_p);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_EnterSlaveMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will make the rule ctrl enter the slave mode.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_EnterSlaveMode()
{
    BOOL_T  ret = TRUE;

    DEVRM_PMGR_FieldInit();

    ret &= RULE_CTRL_LocalInitialize();

    if (FALSE == ret)
        printf("\r\n[RULE_CTRL_EnterSlaveMode] initialize error\r\n");

    SYSFUN_ENTER_SLAVE_MODE_ON_SHMEM(shmem_data_p);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE:  Set transition mode
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_SetTransitionMode()
{
    SYSFUN_SET_TRANSITION_MODE_ON_SHMEM(shmem_data_p);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_EnterTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the rule ctrl enter the transition mode.
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_EnterTransitionMode()
{
    SYSFUN_ENTER_TRANSITION_MODE_ON_SHMEM(shmem_data_p);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_HandleHotInsertion
 *------------------------------------------------------------------------------
 * PURPOSE:  .
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port)
{
    RULE_CTRL_UpdateDevInfo();

    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_HandleHotRemoval
 *------------------------------------------------------------------------------
 * PURPOSE:  .
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_HandleHotRemoval(
    UI32_T starting_port_ifindex,
    UI32_T number_of_port)
{
    UI32_T  ifindex;
    UI32_T  end_port_ifindex;
    UI32_T  unit, tmp_unit;

    end_port_ifindex = starting_port_ifindex + number_of_port - 1;

    unit = tmp_unit = 0;

    LOG("starting_port_ifindex: %lu, num_of_port: %lu, unit: %lu",
        starting_port_ifindex, number_of_port, unit);

    RULE_CTRL_UpdateDevInfo();

    RULE_CTRL_EnterCriticalSection();

    for (ifindex = starting_port_ifindex; ifindex <= end_port_ifindex; ++ifindex)
    {
#define TOTAL_NBR_OF_ETHER (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK * SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT)

        if (ifindex < SYS_ADPT_ETHER_1_IF_INDEX_NUMBER ||
            (SYS_ADPT_ETHER_1_IF_INDEX_NUMBER + TOTAL_NBR_OF_ETHER - 1) < ifindex)
        {
            ASSERT(0);
            continue;
        }
#undef TOTAL_NBR_OF_ETHER

        /* Uport
         */
        RULE_CTRL_HotRemoval_FreeUportRule(ifindex);

        unit = STKTPLG_OM_IFINDEX_TO_UNIT(ifindex);

        if (unit != tmp_unit &&
            !STKTPLG_OM_UnitExist(unit))
        {
            /* Global rule
             */
            RULE_CTRL_HotRemoval_FreeUnitRuleByIfindex(RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER,
                                                       unit);

            /* Trunk rule
             */
            {
                UI32_T trunk_ifindex;

                for (trunk_ifindex = SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER;
                     trunk_ifindex < (SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER + SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM - 1);
                     ++trunk_ifindex)
                {
                    RULE_CTRL_HotRemoval_FreeUnitRuleByIfindex(trunk_ifindex, unit);
                }
            }
        }

        tmp_unit = unit;

        {
            RULE_TYPE_RETURN_TYPE_T result;
            RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;
            RULE_TYPE_INTERFACE_INFO_T interface;

            result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
            if (RULE_TYPE_OK == result)
            {
                if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
                if(NULL == if_entry_p)
                {
                    ASSERT(0);
                    LOG("fail get if_entry_p");
                }

                memset(if_entry_p->lookup_rule_cnt, 0, sizeof(if_entry_p->lookup_rule_cnt));
            }

        }
    }

    RULE_CTRL_LeaveCriticalSectionReturnVoid();
}

void
RULE_CTRL_HotRemoval_FreeRuleGroup(
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p)
{
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T   class_inst_obj;

    if (NULL != class_inst_p &&
        RULE_TYPE_NIL != class_inst_p->super.type)
    {
        ASSERT(RULE_TYPE_INST_ACL == class_inst_p->super.type ||
               RULE_TYPE_INST_POLICY_MAP == class_inst_p->super.type);

        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_inst_p, &class_inst_obj);

        RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&class_inst_obj);
    }
}

static void
RULE_CTRL_HotRemoval_FreeUportRule(
    UI32_T ifindex)
{
    RULE_CTRL_IF_ENTRY_PTR_T           if_entry_p;
    RULE_TYPE_INTERFACE_INFO_T         interface;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T  if_class_obj;

    memset(&interface, 0, sizeof(interface));

    RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

    for (interface.direction = RULE_TYPE_INBOUND;
         interface.direction <= RULE_TYPE_OUTBOUND;
         ++ interface.direction)
    {
        if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
        if (NULL == if_entry_p)
        {
            LOG("Error to get interface entry");
            continue;
        }

        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                  &if_class_obj);

        {
            RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T agg_class_inst_iter;

            if_class_obj.get_element_iterator(&if_class_obj, &agg_class_inst_iter);

            if (0 == agg_class_inst_iter.count(&agg_class_inst_iter))
            {
                continue;
            }
        }

        {
            RULE_TYPE_CLASS_INSTANCE_PTR_T              if_class_inst_p;
            RULE_TYPE_CLASS_INSTANCE_OBJECT_T           class_inst_obj;
            RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;
            RULE_TYPE_INSTANCE_PTR_T                    rule_inst_p;

            if_class_inst_p = if_class_obj.inst;

            RULE_CTRL_IN_Priv_ConstructClassInstanceObject(if_class_inst_p,
                                                           &class_inst_obj);
            class_inst_obj.get_element_iterator(&class_inst_obj, &rule_inst_iter);

            rule_inst_p = rule_inst_iter.first(&rule_inst_iter);
            if (rule_inst_p)
            {
                for (rule_inst_iter.first(&rule_inst_iter);
                     NULL != rule_inst_iter.get_instance(&rule_inst_iter);
                     rule_inst_iter.next(&rule_inst_iter))
                {
                    RULE_TYPE_CLASS_INSTANCE_PTR_T      class_inst_p;
                    RULE_TYPE_CLASS_INSTANCE_OBJECT_T   class_inst_obj;

                    class_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T) rule_inst_iter.get_instance(&rule_inst_iter);


                    if (NULL != class_inst_p &&
                        RULE_TYPE_NIL != class_inst_p->super.type)
                    {
                        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_inst_p, &class_inst_obj);

                        if_class_obj.remove_class_obj(&if_class_obj, &class_inst_obj);
                        RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&class_inst_obj);

                    }
                }
            }
        }
    }
}

static void
RULE_CTRL_HotRemoval_FreeRuleInstanceOfUnit(
    UI32_T unit,
    RULE_TYPE_SHM_POINTER_T ptr)
{
    RULE_TYPE_CLASS_INSTANCE_T                  *in;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T           this;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T iter;

    in = RULE_CTRL_IN_Ptr2Instance(ptr);

    RULE_CTRL_IN_VALIDATE_CLASS_INST(in);

    LOG("pkt-type: %s", RULE_CTRL_REMAPPED_PACKET_TYPE_STR(in->mvl_pkt_type));

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(in, &this);
    this.get_element_iterator(&this, &iter);

    for (iter.first(&iter); NULL != iter.get_instance(&iter);)
    {
        RULE_TYPE_SHM_POINTER_T child = iter.get_ptr(&iter);

        if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(child.type))
        {
            RULE_CTRL_HotRemoval_FreeRuleInstanceOfUnit(unit, child);
        }
        else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(child.type))
        {
            RULE_TYPE_RULE_INSTANCE_T                   *rule_inst;
            RULE_TYPE_RULE_INSTANCE_OBJECT_T            rule_inst_obj;

            rule_inst = RULE_CTRL_IN_Ptr2Instance(child);

            RULE_CTRL_IN_Priv_ConstructRuleInstanceObject(rule_inst, &rule_inst_obj);

            if (rule_inst_obj.inst->dev_rule_info.unit == unit)
            {
                LOG("Remove rule instance on unit %lu", unit);
                RULE_CTRL_IN_LL_Priv_Remove(ptr, child);

                RULE_CTRL_IN_Priv_DestroyRuleInstance(child);

                iter.last(&iter);
                continue;
            }
        }
        else
        {
            ASSERT(0);
        }

        iter.next(&iter);
    }
}

static void
RULE_CTRL_HotRemoval_FreeUnitRuleByIfindex(
    UI32_T ifindex,
    UI32_T unit)
{
    RULE_TYPE_INTERFACE_INFO_T interface;
    RULE_TYPE_RETURN_TYPE_T    result;

    result = RULE_CTRL_Ifindex_To_Interface(ifindex,
                                            &interface);
    ASSERT(result == RULE_TYPE_OK);

    for (interface.direction = RULE_TYPE_INBOUND;
         interface.direction <= RULE_TYPE_OUTBOUND;
         ++ interface.direction)
    {
        RULE_CTRL_IF_ENTRY_PTR_T           if_entry_p;
        RULE_TYPE_CLASS_INSTANCE_OBJECT_T  if_class_obj;

        if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
        if (NULL == if_entry_p)
        {
            LOG("Error to get interface entry");
            continue;
        }

        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                  &if_class_obj);

        {
            RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T agg_class_inst_iter;

            if_class_obj.get_element_iterator(&if_class_obj, &agg_class_inst_iter);

            if (0 == agg_class_inst_iter.count(&agg_class_inst_iter))
            {
                continue;
            }
        }

        {
            RULE_TYPE_CLASS_INSTANCE_PTR_T               if_class_inst_p;
            RULE_TYPE_CLASS_INSTANCE_OBJECT_T            if_class_inst_obj;
            RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T  rule_inst_iter;
            RULE_TYPE_INSTANCE_PTR_T                     rule_inst_p;

            if_class_inst_p = if_class_obj.inst;

            RULE_CTRL_IN_Priv_ConstructClassInstanceObject(if_class_inst_p,
                                                           &if_class_inst_obj);

            if_class_inst_obj.get_element_iterator(&if_class_inst_obj, &rule_inst_iter);

            rule_inst_p = rule_inst_iter.first(&rule_inst_iter);

            if (rule_inst_p)
            {
                for (rule_inst_iter.first(&rule_inst_iter);
                     NULL != rule_inst_iter.get_instance(&rule_inst_iter);
                     rule_inst_iter.next(&rule_inst_iter))
                {
                    RULE_TYPE_CLASS_INSTANCE_PTR_T      class_inst_p;
                    RULE_TYPE_CLASS_INSTANCE_OBJECT_T   class_inst_obj;

                    class_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T) rule_inst_iter.get_instance(&rule_inst_iter);

                    if (NULL != class_inst_p &&
                        RULE_TYPE_NIL != class_inst_p->super.type)
                    {
                        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_inst_p, &class_inst_obj);

                        RULE_CTRL_HotRemoval_FreeRuleInstanceOfUnit(unit, class_inst_obj.ptr);
                    }
                }
            }
        }
    }
    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_Priv_MaskAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : rule value do '&' with mask
 * INPUT    : ace_entry
 * OUTPUT   : ace_entry
 * RETURN   : None
 * NOTES    : 
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_Priv_MaskAceEntry(
    RULE_TYPE_Ace_Entry_T *ace_entry)
{
    UI32_T i = 0;

    switch(ace_entry->ace_type)
    {
        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
        mask_ip:
            ace_entry->u.ip.aceSourceIpAddr = ace_entry->u.ip.aceSourceIpAddr & ace_entry->u.ip.aceSourceIpAddrBitmask;
            ace_entry->u.ip.aceDestIpAddr = ace_entry->u.ip.aceDestIpAddr & ace_entry->u.ip.aceDestIpAddrBitmask;
            ace_entry->u.ip.aceMinSourcePort = ace_entry->u.ip.aceMinSourcePort & ace_entry->u.ip.aceSourcePortBitmask;
            ace_entry->u.ip.aceMaxSourcePort = ace_entry->u.ip.aceMaxSourcePort & ace_entry->u.ip.aceSourcePortBitmask;
            ace_entry->u.ip.aceMinDestPort = ace_entry->u.ip.aceMinDestPort & ace_entry->u.ip.aceDestPortBitmask;
            ace_entry->u.ip.aceMaxDestPort = ace_entry->u.ip.aceMaxDestPort & ace_entry->u.ip.aceDestPortBitmask;
            ace_entry->u.ip.aceControlCode = ace_entry->u.ip.aceControlCode & ace_entry->u.ip.aceControlCodeBitmask;
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
            if (ace_entry->ace_type != RULE_TYPE_MAC_ACL)
            {
                goto mask_mac;
            }
#endif /* SYS_CPNT_ACL_MAC_IP_MIX */
            break;
        case RULE_TYPE_MAC_ACL:
        mask_mac:
            ace_entry->u.mac.aceMinVid = ace_entry->u.mac.aceMinVid & ace_entry->u.mac.aceVidBitmask;
            ace_entry->u.mac.aceMaxVid = ace_entry->u.mac.aceMaxVid & ace_entry->u.mac.aceVidBitmask;
            ace_entry->u.mac.aceMinEtherType = ace_entry->u.mac.aceMinEtherType & ace_entry->u.mac.aceEtherTypeBitmask;
            ace_entry->u.mac.aceMaxEtherType = ace_entry->u.mac.aceMaxEtherType & ace_entry->u.mac.aceEtherTypeBitmask;
            for (i = 0; i<SYS_ADPT_MAC_ADDR_LEN;i++)
            {
                ace_entry->u.mac.aceSourceMacAddr[i] = ace_entry->u.mac.aceSourceMacAddr[i] & ace_entry->u.mac.aceSourceMacAddrBitmask[i];
                ace_entry->u.mac.aceDestMacAddr[i] = ace_entry->u.mac.aceDestMacAddr[i] & ace_entry->u.mac.aceDestMacAddrBitmask[i];
            }
            ace_entry->u.mac.aceMinCos = ace_entry->u.mac.aceMinCos & ace_entry->u.mac.aceCosBitmask;
            ace_entry->u.mac.aceMaxCos = ace_entry->u.mac.aceMaxCos & ace_entry->u.mac.aceCosBitmask;
            ace_entry->u.mac.aceMinSourcePort = ace_entry->u.mac.aceMinSourcePort & ace_entry->u.mac.aceSourcePortBitmask;
            ace_entry->u.mac.aceMaxSourcePort = ace_entry->u.mac.aceMaxSourcePort & ace_entry->u.mac.aceSourcePortBitmask;
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
            if (ace_entry->ace_type == RULE_TYPE_MAC_ACL &&
                ace_entry->u.mac.aceEtherTypeOp == VAL_diffServMacAceEtherTypeOp_equal &&
                ace_entry->u.mac.aceEtherTypeBitmask == MAX_diffServMacAceEtherTypeBitmask &&
                ace_entry->u.mac.aceMinEtherType == RULE_TYPE_ETHERTYPE_IP)
            {
                goto mask_ip;
            }
#endif /* SYS_CPNT_ACL_MAC_IP_MIX */
            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
            goto mask_mac;
#endif /* SYS_CPNT_ACL_MAC_IP_MIX */
            break;

#if (SYS_CPNT_DAI == TRUE)
        case RULE_TYPE_ARP_ACL:
            ace_entry->u.arp.aceTargetIpAddr = ace_entry->u.arp.aceTargetIpAddr & ace_entry->u.arp.aceTargetIpAddrBitmask;
            ace_entry->u.arp.aceSenderIpAddr = ace_entry->u.arp.aceSenderIpAddr & ace_entry->u.arp.aceSenderIpAddrBitmask;
            for (i = 0; i<SYS_ADPT_MAC_ADDR_LEN;i++)
            {
                ace_entry->u.arp.aceSenderMacAddr[i] = ace_entry->u.arp.aceSenderMacAddr[i] & ace_entry->u.arp.aceSenderMacAddrBitmask[i];
                ace_entry->u.arp.aceTargetMacAddr[i] = ace_entry->u.arp.aceTargetMacAddr[i] & ace_entry->u.arp.aceTargetMacAddrBitmask[i];
            }
            break;
#endif
        case RULE_TYPE_MF:
        default:
            break ; /* should not go here */
    }
}

/* Handle Timer Event */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_ProcessTimerEvent
 *------------------------------------------------------------------------------
 * PURPOSE: Process timer alarm event from timer
 * INPUT:  None
 * OUTPUT: None
 * RETURN: TRUE/FALSE
 * NOTES:  None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_ProcessTimerEvent()
{
    if (SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(shmem_data_p) != SYS_TYPE_STACKING_MASTER_MODE)
    {
        return TRUE;
    }

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    RULE_CTRL_LocalBundleRateControlProcessTimerEvent();
#endif /* #if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE) */

    return TRUE;
} /* END FUNCTION - BOOL_T RULE_CTRL_ProcessTimerEvent() */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_FieldInit
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the rule ctrl clear the chip of the slave.
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     Not implement
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_FieldInit()
{
    return;
}

#pragma mark -
#pragma mark ACL / Diffserv Functions

/** ACL / Diffserv Functions
 */

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_AddClassMapConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : add class-map config on chip for every ports bound with specific policy-map and class-map
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : 1, if there is nothing to config, return TRUE (do nothing)
 *            2, this func used when a policy-map already config to chip and dynamically add a class-map
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_AddClassMapConfig(UI32_T policy_map_index, UI32_T class_map_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSyncClassMap2DEV(policy_map_index, class_map_index);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Failed to update meter");
        RULE_CTRL_LeaveCriticalSectionReturnState(result);
    }

    RULE_CTRL_LeaveCriticalSection();

    result = RULE_OM_AddCosReference(policy_map_index, class_map_index);
    if (RULE_TYPE_OK != result)
        return result;

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_AddClassMapElementConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : add class-map element config on chip for every ports bound
 *            with specific class-map
 * INPUT    : class_map_index, class_type, class_index
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is nothing to config, return TRUE (do nothing)
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_AddClassMapElementConfig(UI32_T class_map_index, RULE_TYPE_ClassType_T class_type, UI32_T class_index)
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T i;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    if(class_type == RULE_TYPE_CLASS_ACL)
    {
        RULE_TYPE_Acl_T acl_entry;

        result = RULE_OM_GetAclByIndex(class_index, &acl_entry);
        if (RULE_TYPE_OK != result)
        {
            return result;
        }
        if (TRUE == acl_entry.flag_aggregate_done)
        {
            class_index = acl_entry.aggregate_acl_index;
        }
    }
#endif

    RULE_CTRL_EnterCriticalSection();

    for (i = 0; i < _countof(shmem_data_p->if_table); ++ i)
    {
        RULE_TYPE_INTERFACE_INFO_T interface;
        //
        // --== Duplicated code start
        //
        RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

        RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

        RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;
        RULE_CTRL_INSTANCE_TYPE_ID_T find_param;
        //
        // --== End of duplicated code
        //

        RULE_CTRL_Priv_IfEntry_Index_To_Interface(i, &interface);

        //
        // --== Duplicated code start
        //
        if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
        ASSERT(NULL != if_entry_p);

        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                  &if_class_obj);

        memset(&find_param, 0, sizeof(find_param));

        find_param.type = RULE_TYPE_INST_POLICY_MAP;

        policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                           RULE_CTRL_Priv_IsClassInstanceMatchType,
                                                                                           &find_param);
        //
        // --== End of duplicated code
        //

        while (NULL != policy_map_inst_p)
        {
            RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p;
            RULE_CTRL_INSTANCE_TYPE_ID_T class_map_find_param;

            memset(&class_map_find_param, 0, sizeof(class_map_find_param));

            class_map_find_param.type = RULE_TYPE_INST_CLASS_MAP;
            class_map_find_param.id = class_map_index;

            /** assumption: a class-map can not bind in a policy-map twice
             */
            class_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_IN_Priv_FindIf((RULE_TYPE_INSTANCE_PTR_T)policy_map_inst_p,
                                                                                        RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                        &class_map_find_param);
            if (NULL != class_map_inst_p)
            {
                RULE_TYPE_PolicyElement_T policy_element;

                ASSERT(class_map_inst_p->id == class_map_index);

                result = RULE_OM_GetPolicyMapElementEntry(policy_map_inst_p->id, class_map_inst_p->id, &policy_element);
                ASSERT(RULE_TYPE_OK == result);

                if (RULE_TYPE_OK != result)
                {
                    RULE_CTRL_LeaveCriticalSection();
                    goto undo;
                }

                ASSERT(policy_element.class_map_index == class_map_index);

                if (RULE_TYPE_CLASS_MF == class_type)
                {
                    result = RULE_CTRL_LocalSetMF2DEV(&interface, policy_map_inst_p->id, class_map_inst_p->id,
                                                      class_index, policy_element.meter_index, policy_element.action_index);
                    if (RULE_TYPE_OK != result)
                    {
                        RULE_CTRL_LeaveCriticalSection();
                        goto undo;
                    }
                }
                else if (RULE_TYPE_CLASS_ACL == class_type)
                {
                    result = RULE_CTRL_LocalSetQoSACL2DEV(&interface, policy_map_inst_p->id, class_map_inst_p->id,
                                                          class_index, policy_element.meter_index, policy_element.action_index);
                    if (RULE_TYPE_OK != result)
                    {
                        RULE_CTRL_LeaveCriticalSection();
                        goto undo;
                    }
                }
                else
                {
                    RULE_CTRL_LeaveCriticalSection();
                    ASSERT(0);
                    goto undo;
                }
            }

            policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetNextSiblingIf((RULE_TYPE_INSTANCE_PTR_T)policy_map_inst_p,
                                                                                                RULE_CTRL_Priv_IsClassInstanceMatchType,
                                                                                                &find_param);
        }
    }

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK;

undo:
    {
        RULE_TYPE_RETURN_TYPE_T temp_result;

        temp_result = RULE_CTRL_RemoveClassMapElementConfig(class_map_index, class_type, class_index);
        ASSERT(RULE_TYPE_OK == temp_result);
    }

    ASSERT(RULE_TYPE_OK != result);

    return result;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_AddMeterConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : add meter config on chip for every ports bound with specific policy-map and class-map
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is nothing to config, return TRUE (do nothing)
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_AddMeterConfig(UI32_T policy_map_index, UI32_T class_map_index)
{
    RULE_TYPE_RETURN_TYPE_T     result;

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSyncClassMap2DEV(policy_map_index, class_map_index);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Failed to update meter");
        RULE_CTRL_LeaveCriticalSectionReturnState(result);
    }

    RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_UpdateMeterConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : update meter config on chip for every ports bound with specific policy-map and class-map
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is nothing to config, return TRUE (do nothing)
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_UpdateMeterConfig(UI32_T policy_map_index, UI32_T class_map_index)
{
    RULE_TYPE_RETURN_TYPE_T     result;

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSyncClassMap2DEV(policy_map_index, class_map_index);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Failed to update meter");
        RULE_CTRL_LeaveCriticalSectionReturnState(result);
    }

    RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_UpdateActionConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : update action config on chip for every ports bound with specific policy-map and class-map
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is nothing to config, return TRUE (do nothing)
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_UpdateActionConfig(UI32_T policy_map_index, UI32_T class_map_index)
{
    RULE_TYPE_RETURN_TYPE_T     result;

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSyncClassMap2DEV(policy_map_index, class_map_index);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Failed to update action");
        RULE_CTRL_LeaveCriticalSectionReturnState(result);
    }

    RULE_CTRL_LeaveCriticalSection();

    /* remove cos reference first because action may reference COS no more */
    RULE_OM_RemoveCosReference(policy_map_index, class_map_index);

    result = RULE_OM_AddCosReference(policy_map_index, class_map_index);
    if (RULE_TYPE_OK != result)
        return result;

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_RemoveClassMapConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : remove class-map config on chip for every ports bound with specific policy-map and class-map
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is nothing to free, return TRUE (do nothing)
 *            free_cos_reference = TRUE implies need to free CoS reference
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_RemoveClassMapConfig(UI32_T policy_map_index, UI32_T class_map_index, BOOL_T free_cos_reference)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i;
    //
    // --== Duplicated code start
    //
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;
    RULE_CTRL_INSTANCE_TYPE_ID_T param;
    //
    // --== End of duplicated code
    //

    RULE_TYPE_PolicyMap_T       policy_map;

    RULE_TYPE_ClassMap_T        class_map;
    RULE_TYPE_PolicyElement_T   policy_element;

    result = RULE_OM_GetPolicyMap(policy_map_index, &policy_map);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    result = RULE_OM_GetClassMap(class_map_index, &class_map);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    result = RULE_OM_GetPolicyMapElementFromPolicyMapByClassMap(policy_map_index, class_map_index, &policy_element);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    RULE_CTRL_EnterCriticalSection();

    for (i = 0; i < _countof(shmem_data_p->if_table); ++ i)
    {
        RULE_TYPE_INTERFACE_INFO_T interface;

        RULE_CTRL_Priv_IfEntry_Index_To_Interface(i, &interface);

        //
        // --== Duplicated code start
        //
        if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
        ASSERT(NULL != if_entry_p);

        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                  &if_class_obj);

        memset(&param, 0, sizeof(param));

        param.type = RULE_TYPE_INST_POLICY_MAP;
        param.id = policy_map_index;

        policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                           RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                           &param);
        //
        // --== End of duplicated code
        //
        if (NULL == policy_map_inst_p)
        {
            continue;
        }

        result = RULE_CTRL_LocalRemoveClassMap2DEV(&interface, policy_map_index, &policy_map,
                                                   policy_map_inst_p,
                                                   policy_element.class_map_index,
                                                   policy_element.meter_index,
                                                   policy_element.action_index);
        ASSERT(RULE_TYPE_OK == result);
        if (RULE_TYPE_OK != result)
        {
            continue;
        }
    }

    RULE_CTRL_LeaveCriticalSection();

    if (TRUE == free_cos_reference)
        RULE_OM_RemoveCosReference(policy_map_index, class_map_index);

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_RemoveClassMapElementConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : remove class-map element config on chip for every ports bound with specific class-map
 * INPUT    : class_map_index, class_type, class_index
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is nothing to free, return TRUE (do nothing)
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_RemoveClassMapElementConfig(UI32_T class_map_index, RULE_TYPE_ClassType_T class_type, UI32_T class_index)
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T i;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    if(class_type == RULE_TYPE_CLASS_ACL)
    {
        RULE_TYPE_Acl_T acl_entry;

        result = RULE_OM_GetAclByIndex(class_index, &acl_entry);
        if (RULE_TYPE_OK != result)
        {
            return result;
        }
        if (TRUE == acl_entry.flag_aggregate_done)
        {
            class_index = acl_entry.aggregate_acl_index;
        }
    }
#endif

    RULE_CTRL_EnterCriticalSection();

    for (i = 0; i < _countof(shmem_data_p->if_table); ++ i)
    {
        RULE_TYPE_INTERFACE_INFO_T interface;
        //
        // --== Duplicated code start
        //
        RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

        RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

        RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;
        RULE_CTRL_INSTANCE_TYPE_ID_T find_param;
        //
        // --== End of duplicated code
        //

        RULE_CTRL_Priv_IfEntry_Index_To_Interface(i, &interface);

        //
        // --== Duplicated code start
        //
        if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
        ASSERT(NULL != if_entry_p);

        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                  &if_class_obj);

        memset(&find_param, 0, sizeof(find_param));

        find_param.type = RULE_TYPE_INST_POLICY_MAP;

        policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                           RULE_CTRL_Priv_IsClassInstanceMatchType,
                                                                                           &find_param);
        //
        // --== End of duplicated code
        //

        while (NULL != policy_map_inst_p)
        {
            RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p;
            RULE_CTRL_INSTANCE_TYPE_ID_T class_map_find_param;

            memset(&class_map_find_param, 0, sizeof(class_map_find_param));

            class_map_find_param.type = RULE_TYPE_INST_CLASS_MAP;
            class_map_find_param.id = class_map_index;

            /** assumption: a class-map can not bind in a policy-map twice
             */
            class_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_IN_Priv_FindIf((RULE_TYPE_INSTANCE_PTR_T)policy_map_inst_p,
                                                                                        RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                        &class_map_find_param);
            if (NULL != class_map_inst_p)
            {
                RULE_TYPE_PolicyElement_T policy_element;

                ASSERT(class_map_inst_p->id == class_map_index);

                result = RULE_OM_GetPolicyMapElementEntry(policy_map_inst_p->id, class_map_inst_p->id, &policy_element);
                ASSERT(RULE_TYPE_OK == result);

                if (RULE_TYPE_OK != result)
                {
                    continue;
                }

                ASSERT(policy_element.class_map_index == class_map_index);

                if (RULE_TYPE_CLASS_MF == class_type)
                {
                    result = RULE_CTRL_LocalRemoveMF2DEV(&interface, policy_map_inst_p->id, class_map_inst_p->id,
                                                      class_index, policy_element.meter_index, policy_element.action_index);
                    ASSERT(RULE_TYPE_OK == result);
                }
                else if (RULE_TYPE_CLASS_ACL == class_type)
                {
                    result = RULE_CTRL_LocalRemoveQoSACL2DEV(&interface, policy_map_inst_p->id, class_map_inst_p->id,
                                                             class_index, policy_element.meter_index, policy_element.action_index);
                    ASSERT(RULE_TYPE_OK == result);
                }
                else
                {
                    ASSERT(0);
                }
            }

            policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetNextSiblingIf((RULE_TYPE_INSTANCE_PTR_T)policy_map_inst_p,
                                                                                                RULE_CTRL_Priv_IsClassInstanceMatchType,
                                                                                                &find_param);
        }
    }

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_RemoveMeterConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : remove meter config on chip for every ports bound with specific policy-map and class-map
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is nothing to free, return TRUE (do nothing)
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_RemoveMeterConfig(UI32_T policy_map_index, UI32_T class_map_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSyncClassMap2DEV(policy_map_index, class_map_index);
    if (RULE_TYPE_OK != result)
    {
        RULE_CTRL_LeaveCriticalSectionReturnState(result);
    }

    RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/* rule */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetAce
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a ACE entry.
 * INPUT:    acl_id, rule_entry
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_SetAce(
    UI32_T acl_id,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p,
    BOOL_T is_add)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_Acl_T         acl_entry;

    result = RULE_OM_GetAclByIndex(acl_id, &acl_entry);
    if (RULE_TYPE_OK != result)
        return result;

    RULE_CTRL_EnterCriticalSection();

    if (is_add == TRUE)
    {
        result = RULE_CTRL_LocalAddAce2DEV(acl_id, &acl_entry, ace_entry_p);
    }
    else
    {
        result = RULE_CTRL_LocalRemoveAceFromDEV(acl_id, &acl_entry, ace_entry_p);
    }

    RULE_CTRL_LeaveCriticalSectionReturnState(result);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetQoSAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a rule entry.
 * INPUT:    unit, port, acl_id, meter_id, action_id, is_add
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_SetQoSAcl(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T acl_id,
    UI32_T meter_id,
    UI32_T action_id,
    BOOL_T is_add)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_INTERFACE_INFO_T interface;

    memset(&interface, 0, sizeof(interface));

    result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    interface.direction = direction;

    if (RULE_TYPE_INTERFACE_UPORT != interface.type &&
        RULE_TYPE_INTERFACE_TRUNK != interface.type &&
        RULE_TYPE_INTERFACE_CPU_PORT != interface.type &&
        RULE_TYPE_INTERFACE_ALL_UNIT != interface.type)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_INBOUND != interface.direction &&
        RULE_TYPE_OUTBOUND != interface.direction)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_INTERFACE_UPORT == interface.type)
    {
        UI32_T module_id;
        UI32_T device_id;
        UI32_T phy_port;

        module_id = 0;
        device_id = 0;
        phy_port  = 0;

        if (FALSE == DEV_SWDRV_PMGR_Logical2PhyDevicePortID(interface.uport.unit,
                                                            interface.uport.port,
                                                            &module_id,
                                                            &device_id,
                                                            &phy_port))
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_CPU_PORT == interface.type)
    {
        if (RULE_TYPE_OUTBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_TRUNK == interface.type)
    {
        if (RULE_TYPE_INBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_ALL_UNIT == interface.type)
    {
        if (RULE_TYPE_INBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    {
        RULE_TYPE_Acl_T acl_entry;

        result = RULE_OM_GetAclByIndex(acl_id, &acl_entry);
        if (RULE_TYPE_OK != result)
        {
            return result;
        }
        if (TRUE == acl_entry.flag_aggregate_done)
        {
            acl_id = acl_entry.aggregate_acl_index;
        }
    }
#endif

    RULE_CTRL_EnterCriticalSection();

    if (TRUE == is_add)
    {
        result = RULE_CTRL_LocalSetQoSACL2DEV(&interface,
                                              policy_map_index,
                                              class_map_index,
                                              acl_id,
                                              meter_id,
                                              action_id);
    }
    else
    {
        result = RULE_CTRL_LocalRemoveQoSACL2DEV(&interface,
                                                 policy_map_index,
                                                 class_map_index,
                                                 acl_id,
                                                 meter_id,
                                                 action_id);
    }

    RULE_CTRL_LeaveCriticalSectionReturnState(result);
}

#if (SYS_CPNT_PBR == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_ConvertPBRAction2Dev
 *------------------------------------------------------------------------------
 * PURPOSE:  Conver PBR action to DEVRM action
 * INPUT:    action_p
 * OUTPUT:   dev_action_p
 * RETURN:   RULE_TYPE_OK / Error code (RULE_TYPE_RETURN_TYPE_T)
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_ConvertPBRAction2Dev(
    const RULE_TYPE_PBR_ACTION_T *action_p,
    DEVRM_ACTION_T *dev_action_p)
{
    DEVRM_LIB_ActionInit(dev_action_p);

    switch (action_p->cmd)
    {
        case RULE_TYPE_PBR_PACKET_CMD_PERMIT:
            dev_action_p->pkt_cmd = DEVRM_PKT_CMD_FORWARD;
            break;

        case RULE_TYPE_PBR_PACKET_CMD_DENY:
            dev_action_p->pkt_cmd = DEVRM_PKT_CMD_HARD_DROP;
            break;

        default:
            ASSERT(0);
            return RULE_TYPE_FAIL;
    }

    if (action_p->redirect.action == RULE_TYPE_PBR_REDIRECT_ACTION_ROUNT)
    {
        ASSERT(action_p->cmd == RULE_TYPE_PBR_PACKET_CMD_PERMIT);

        if (action_p->redirect.ltt_index.column != 0)
        {
            ASSERT(0);
            /* Bobcate has hw bug,
             * the ltt_index.colunm only support 0
             */
            return RULE_TYPE_INVALID_PARAMETER;
        }

        dev_action_p->redirect.mode = DEVRM_REDIRECT_MODE_ROUTE;
        dev_action_p->redirect.ltt_index.row = action_p->redirect.ltt_index.row;
        dev_action_p->redirect.ltt_index.column = action_p->redirect.ltt_index.column;
    }

    if (action_p->qos.dscp.is_modified == TRUE)
    {
        dev_action_p->qos.modify_dscp = DEVRM_MODIFY_CMD_ENABLE;
        dev_action_p->qos.dscp = (UI8_T) action_p->qos.dscp.value;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalAddPbrRule_To_Dev
 *------------------------------------------------------------------------------
 * PURPOSE:  Add PBR rule to device
 * INPUT:    unit, device_id, interface_p, remapped_pkt_type
 *           ace_inst_obj_p - Parent ace instance object for this rule.
 *           rule_pattern_p - The DEVRM_AceEntry for adding to device.
 *           dev_action_p   - The DEVRM_ACTION for adding to device.
 *           priority       - Rule priority (Max number mean higher priority.)
 * OUTPUT:   None.
 * RETURN:   RULE_TYPE_OK / Error code (RULE_TYPE_RETURN_TYPE_T)
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddPbrRule_To_Dev(
    UI32_T unit,
    UI32_T device_id,
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T remapped_pkt_type,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *ace_inst_obj_p,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p,
    int priority)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_AclType_T acl_key_mode = RULE_TYPE_IP_EXT_ACL;
    UI32_T rule_index;
    UI32_T vir_group_id;

    RULE_CTRL_PCL_CFG_T pcl_cfg;
    RULE_TYPE_RULE_INSTANCE_OBJECT_T rule_inst_obj;

    ASSERT(dev_action_p != NULL);

    result = RULE_CTRL_Priv_ConvertPacketTypeToPclConfig(interface_p,
                                                         remapped_pkt_type,
                                                         &pcl_cfg);
    ASSERT(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_ConvertPacketTypeToVirtualGroupId(remapped_pkt_type,
                                                              &vir_group_id);
    ASSERT(RULE_TYPE_OK == result);

    result = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &rule_inst_obj);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Failed to create rule instance");
        result = RULE_TYPE_E_INSUFFICIENT_RULE_INSTANCE;
        goto undo;
    }

    rule_inst_obj.inst->mvl_rule_index = RULE_CTRL_IN_IndexOfRuleInstance(rule_inst_obj.inst);
    rule_inst_obj.inst->dev_rule_info.rule_id = 0;
    rule_inst_obj.inst->active = FALSE;

    ace_inst_obj_p->add_rule_obj(ace_inst_obj_p, &rule_inst_obj);

    rule_index = RULE_CTRL_LocalAllocateLogicalRuleID(unit, device_id, acl_key_mode);
    if (0 == rule_index)
    {
        LOG("<Error> Failed to allocate logical rule ID");
        result = RULE_TYPE_INSUFFICIENT_RESOURCE;
        goto undo;
    }

    rule_inst_obj.inst->dev_rule_info.unit             = unit;
    rule_inst_obj.inst->dev_rule_info.device           = device_id;
    rule_inst_obj.inst->dev_rule_info.lookup_num       = pcl_cfg.lookup_num;
    rule_inst_obj.inst->dev_rule_info.pcl_id           = pcl_cfg.pcl_id;
    rule_inst_obj.inst->dev_rule_info.pcl_id_mask      = pcl_cfg.pcl_id_mask;
    rule_inst_obj.inst->dev_rule_info.virtual_group_id = vir_group_id;
    rule_inst_obj.inst->dev_rule_info.priority         = priority;
    rule_inst_obj.inst->dev_rule_info.rule_id          = rule_index;

    memcpy(&rule_inst_obj.inst->dev_rule_info.interface,
           interface_p,
           sizeof(rule_inst_obj.inst->dev_rule_info.interface));

    {
        result = RULE_CTRL_LocalAllocateAndSetRule(rule_inst_obj.inst->dev_rule_info.unit,
                                                   rule_inst_obj.inst->dev_rule_info.device,
                                                   &pcl_cfg,
                                                   rule_inst_obj.inst->dev_rule_info.virtual_group_id,
                                                   rule_inst_obj.inst->dev_rule_info.priority,
                                                   rule_inst_obj.inst->dev_rule_info.rule_id,
                                                   rule_pattern_p,
                                                   dev_action_p);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Failed to allocate and set rule on dev");
            goto undo;
        }

        rule_inst_obj.inst->active = TRUE;

        /**
         * Saving rule pattern into rule table
         */
        ASSERT(sizeof(shmem_data_p->rule_table[0].rule_pattern) == sizeof(*rule_pattern_p));
        memcpy(&shmem_data_p->rule_table[rule_inst_obj.inst->mvl_rule_index].rule_pattern,
               rule_pattern_p, sizeof(*rule_pattern_p));

        ASSERT(sizeof(shmem_data_p->rule_table[0].action) == sizeof(*dev_action_p));
        memcpy(&shmem_data_p->rule_table[rule_inst_obj.inst->mvl_rule_index].action,
               dev_action_p, sizeof(*dev_action_p));
    }

    return RULE_TYPE_OK;

undo:
    return result;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalSetPbrACL2DEV
 *------------------------------------------------------------------------------
 * PURPOSE:  Set ACL to device for PBR
 * INPUT:    interface_p - The interface for binding.
 *           vlan        - The vlan for binding.
 *           acl_id      - The ACL id for binding.
 *           action_p    - The PBR action for this ACL.
 * OUTPUT:   None.
 * RETURN:   RULE_TYPE_OK / Error code (RULE_TYPE_RETURN_TYPE_T)
 * NOTE:     Now using global ACL + filter vlan to apply the ACL on vlan.
 *
 *           This function is using to maintain the duplication database for PBR.
 *           The duplication structure is as following:
 *             Interface instance (interface = global interface)
 *              +- PBR instance (id = Vlan ID)
 *                   +- {PBR class map instance}* (id = sequence number)
 *                        +- {ACL instance}* (id = ACL ID)
 *                             +- [ACE instance]* (id = ACE ID)
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetPbrACL2DEV(
    RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T vlan,
    UI32_T seq_num,
    UI32_T acl_id,
    const RULE_TYPE_PBR_ACTION_T *action_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T ace_index;
    UI32_T remapped_pkt_type;

    RULE_TYPE_Acl_T       acl_entry;
    RULE_TYPE_Ace_Entry_T ace_entry;
    RULE_TYPE_AclType_T   ace_type;

    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T pbr_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T pbr_inst_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T pbr_class_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T pbr_class_inst_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;

    int priority;

    LOG_IF(*interface_p, "vlan = %lu, seqence_number = %lu, acl_id = %lu",
           vlan, seq_num, acl_id);

    ASSERT(NULL != interface_p);
    ASSERT(RULE_TYPE_INTERFACE_ALL_UNIT == interface_p->type);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction);

    remapped_pkt_type = RULE_TYPE_PACKET_PBR;

    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    /* RULE_TYPE_INST_PBR
     */
    memset(&param, 0, sizeof(param));
    param.type = RULE_TYPE_INST_PBR;
    param.id = vlan;
    pbr_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                &param);
    if (NULL == pbr_inst_p)
    {
        result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_PBR,
                                                             &pbr_inst_obj);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Failed to create pbr instance");
            goto undo;
        }

        pbr_inst_p = pbr_inst_obj.inst;

        ASSERT(NULL != pbr_inst_p);

        pbr_inst_p->id = vlan;
        pbr_inst_p->mvl_pkt_type = RULE_TYPE_PACKET_NIL;

        if_class_obj.add_class_obj(&if_class_obj, &pbr_inst_obj);
    }
    else
    {
        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(pbr_inst_p,
                                                       &pbr_inst_obj);
    }

    /* RULE_TYPE_INST_CLASS_MAP
     */
    memset(&param, 0, sizeof(param));
    param.type = RULE_TYPE_INST_CLASS_MAP;
    param.id = seq_num;
    pbr_class_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) pbr_inst_obj.inst,
                                                                                RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                &param);
    if (NULL == pbr_class_inst_p)
    {
        result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_CLASS_MAP,
                                                             &pbr_class_inst_obj);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Failed to create pbr-class instance");
            goto undo;
        }

        pbr_class_inst_p = pbr_class_inst_obj.inst;

        ASSERT(NULL != pbr_class_inst_p);

        pbr_class_inst_p->id = seq_num;
        pbr_class_inst_p->mvl_pkt_type = RULE_TYPE_PACKET_NIL;

        ASSERT(NULL != pbr_inst_p);
        ASSERT(pbr_inst_p == pbr_inst_obj.inst);

        pbr_inst_obj.add_class_obj(&pbr_inst_obj, &pbr_class_inst_obj);
    }
    else
    {
        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(pbr_class_inst_p,
                                                       &pbr_class_inst_obj);
    }

    /* RULE_TYPE_INST_ACL
     */
    memset(&param, 0, sizeof(param));
    param.type = RULE_TYPE_INST_ACL;
    param.id = acl_id;
    acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) pbr_class_inst_obj.inst,
                                                                                RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                &param);
    if (NULL == acl_inst_p)
    {
        DEVRM_ACTION_T  dev_action;
        UI8_T cpu_mac[SYS_ADPT_MAC_ADDR_LEN];

        STKTPLG_POM_GetLocalUnitBaseMac(cpu_mac);

        /**
         * Create ACL instance
         */
        {
            result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl_inst_obj);
            if (RULE_TYPE_OK != result)
            {
                LOG("<Error> Failed to create ACL instance");
                goto undo;
            }

            acl_inst_p = acl_inst_obj.inst;
            ASSERT(NULL != acl_inst_p);
            acl_inst_p->acl_id = acl_id;
            acl_inst_p->mvl_pkt_type = RULE_TYPE_PACKET_NIL;
            pbr_class_inst_obj.add_class_obj(&pbr_class_inst_obj, &acl_inst_obj);
        }

        result = RULE_CTRL_Priv_ConvertPBRAction2Dev(action_p, &dev_action);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Failed to convert action to dev_action");
            goto undo;
        }

        ace_index = 0;
        while (RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_id, &ace_type, &ace_index, &ace_entry))
        {
            RULE_TYPE_CLASS_INSTANCE_PTR_T    ace_inst_p;
            RULE_TYPE_CLASS_INSTANCE_OBJECT_T ace_inst_obj;
            DEVRM_AceEntry_T rule_pattern;

            if (RULE_TYPE_ACE_DENY == ace_entry.access)
            {
                continue;
            }

            /* Fill rule_pattern
             */
            {
                DEVRM_LIB_RulePatternInit(&rule_pattern);
                result = RULE_CTRL_LocalConvertAce2Dev(&ace_entry, &rule_pattern);
                ASSERT(RULE_TYPE_OK == result);

                /* (+) Apply to vlan
                 * (+) PBR shall only work for route packet (DA = CPU_MAC)
                 */
                rule_pattern.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_VID;
                rule_pattern.outvlan_data = vlan;
                rule_pattern.outvlan_mask = MAX_diffServMacAceVidBitmask;

                rule_pattern.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_MACDA;
                memcpy(rule_pattern.dstmac_data, cpu_mac, SYS_ADPT_MAC_ADDR_LEN);
                memset(rule_pattern.dstmac_mask, 0xff, SYS_ADPT_MAC_ADDR_LEN);
            }

            /**
             * Create ACE instance
             */
            {
                result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACE, &ace_inst_obj);
                if (RULE_TYPE_OK != result)
                {
                    LOG("<Error> Failed to create ACE instance");
                    goto undo;
                }
                ace_inst_p = ace_inst_obj.inst;
                ASSERT(NULL != ace_inst_p);
                ace_inst_p->id = ace_index;
                ace_inst_p->mvl_pkt_type = remapped_pkt_type;
                acl_inst_obj.add_class_obj(&acl_inst_obj, &ace_inst_obj);
            }

            priority = seq_num * -1;
            result = RULE_CTRL_EACH_UNIT_DEVICE(interface_p,
                                                RULE_CTRL_LocalAddPbrRule_To_Dev,
                                                interface_p,
                                                remapped_pkt_type,
                                                &ace_inst_obj,
                                                &rule_pattern,
                                                &dev_action,
                                                priority);
            if (RULE_TYPE_OK != result)
            {
                goto undo;
            }
        }
    }
    else
    {
        LOG("Already bind this ACL");
    }

    return RULE_TYPE_OK;

undo:

    {
        RULE_TYPE_RETURN_TYPE_T temp_result;

        temp_result = RULE_CTRL_LocalRemovePbrACL2DEV(interface_p,
                                                      vlan,
                                                      seq_num,
                                                      acl_id);
        ASSERT(RULE_TYPE_OK == temp_result);
    }

    ASSERT(RULE_TYPE_OK != result);
    return result;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalRemovePbrACL2DEV
 *------------------------------------------------------------------------------
 * PURPOSE:  Remove ACL from device for PBR
 * INPUT:    interface_p - The interface binded.
 *           vlan        - The vlan for binded.
 *           acl_id      - The binded ACL id.
 * OUTPUT:   None.
 * RETURN:   RULE_TYPE_OK / Error code (RULE_TYPE_RETURN_TYPE_T)
 * NOTE:
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemovePbrACL2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T vlan,
    UI32_T seq_num,
    UI32_T acl_id)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T pbr_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T pbr_inst_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T pbr_class_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T pbr_class_inst_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;

    ASSERT(NULL != interface_p);
    ASSERT(RULE_TYPE_INTERFACE_ALL_UNIT == interface_p->type);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction);

    LOG_IF(*interface_p, "vlan = %lu, seq_num = %lu, aclId = %lu",
           vlan, seq_num, acl_id);

    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    /* Find pbr_instance
     */
    memset(&param, 0, sizeof(param));
    param.type = RULE_TYPE_INST_PBR;
    param.id = vlan;
    pbr_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                &param);
    if (NULL == pbr_inst_p)
    {
        return RULE_TYPE_OK;
    }

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(pbr_inst_p, &pbr_inst_obj);

    /* Find pbr_class_instance
     */
    memset(&param, 0, sizeof(param));
    param.type = RULE_TYPE_INST_CLASS_MAP;
    param.id = seq_num;
    pbr_class_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) pbr_inst_obj.inst,
                                                                                      RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                      &param);
    if (NULL == pbr_class_inst_p)
    {
        goto undo_pbr_instance;
    }

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(pbr_class_inst_p, &pbr_class_inst_obj);

    /* Find acl_instance
     */
    memset(&param, 0, sizeof(param));
    param.type = RULE_TYPE_INST_ACL;
    param.id = acl_id;
    acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) pbr_class_inst_obj.inst,
                                                                                RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                &param);
    if (NULL != acl_inst_p)
    {
        result = RULE_CTRL_LocalFreeRulesByRuleGroup(acl_inst_p);
        ASSERT(RULE_TYPE_OK == result);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Failed to free rules by rule group, acl_id = %lu", acl_id);
            return result;
        }

        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p, &acl_inst_obj);

        pbr_class_inst_obj.remove_class_obj(&pbr_class_inst_obj, &acl_inst_obj);
        RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&acl_inst_obj);

        acl_inst_p = NULL;
        acl_inst_obj.inst = NULL;
    }

    /* If no any acl in pbr-class-map, remove it
     */
    if (RULE_TYPE_NIL == pbr_class_inst_obj.inst->super.links.first_node.type)
    {
        pbr_inst_obj.remove_class_obj(&pbr_inst_obj, &pbr_class_inst_obj);
        RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&pbr_class_inst_obj);

        pbr_class_inst_p = NULL;
        pbr_class_inst_obj.inst = NULL;
    }

undo_pbr_instance:

    ASSERT(NULL != pbr_inst_p);
    ASSERT(pbr_inst_p == pbr_inst_obj.inst);
    ASSERT(NULL != if_class_obj.inst);

    /* If no any pbr-class-map in pbr, remove it
     */
    if (RULE_TYPE_NIL == pbr_inst_obj.inst->super.links.first_node.type)
    {
        if_class_obj.remove_class_obj(&if_class_obj, &pbr_inst_obj);
        RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&pbr_inst_obj);

        pbr_inst_p = NULL;
        pbr_inst_obj.inst = NULL;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetPbrAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set ACL rules for PBR
 * INPUT:    is_add   - Add/Delete rule for PBR
 *           vlan     - The vlan for binding.
 *           acl_id   - The ACL for binding.
 *           action_p - The PBR action for this ACL.
 * OUTPUT:   None.
 * RETURN:   RULE_TYPE_OK / Error code (RULE_TYPE_RETURN_TYPE_T)
 * NOTE:     Now using global interface + filter vlan to apply the ACL on the vlan.
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_SetPbrAcl(
    BOOL_T is_add,
    UI32_T vlan,
    UI32_T seq_num,
    UI32_T acl_id,
    const RULE_TYPE_PBR_ACTION_T *action_p)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_INTERFACE_INFO_T interface;
    RULE_TYPE_Acl_T acl_entry;

    memset(&interface, 0, sizeof(interface));
    interface.type = RULE_TYPE_INTERFACE_ALL_UNIT;
    interface.direction = RULE_TYPE_INBOUND;

    RULE_CTRL_EnterCriticalSection();

    if (TRUE == is_add)
    {
        ASSERT(action_p != NULL);

        result = RULE_OM_GetAclByIndex(acl_id, &acl_entry);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Invalid parameter");
            RULE_CTRL_LeaveCriticalSectionReturnState(result);
        }
#if 0
        /* FORCE_INSTALL
         */
        result = RULE_CTRL_LocalRemovePbrACL2DEV(&interface,
                                                 vlan,
                                                 seq_num,
                                                 acl_id);
        ASSERT(result == RULE_TYPE_OK);
#endif /* 0 */

        result = RULE_CTRL_LocalSetPbrACL2DEV(&interface,
                                              vlan,
                                              seq_num,
                                              acl_id,
                                              action_p);
    }
    else
    {
        result = RULE_CTRL_LocalRemovePbrACL2DEV(&interface,
                                                 vlan,
                                                 seq_num,
                                                 acl_id);
    }

    RULE_CTRL_LeaveCriticalSectionReturnState(result);
}
#endif /* SYS_CPNT_PBR */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a rule entry.
 * INPUT:    is_ingress, unit, port, acl_id, is_trunk, is_add, time_range_index
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_SetAcl(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_id,
    BOOL_T is_add,
    UI32_T time_range_index,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_INTERFACE_INFO_T interface;

    RULE_TYPE_Acl_T acl_entry;

    memset(&interface, 0, sizeof(interface));

    result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    interface.direction = direction;

    if (RULE_TYPE_INTERFACE_UPORT != interface.type &&
        RULE_TYPE_INTERFACE_TRUNK != interface.type &&
        RULE_TYPE_INTERFACE_CPU_PORT != interface.type &&
        RULE_TYPE_INTERFACE_ALL_UNIT != interface.type)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_INBOUND != interface.direction &&
        RULE_TYPE_OUTBOUND != interface.direction)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_INTERFACE_UPORT == interface.type)
    {
        UI32_T module_id;
        UI32_T device_id;
        UI32_T phy_port;

        module_id = 0;
        device_id = 0;
        phy_port  = 0;

        if (FALSE == DEV_SWDRV_PMGR_Logical2PhyDevicePortID(interface.uport.unit,
                                                            interface.uport.port,
                                                            &module_id,
                                                            &device_id,
                                                            &phy_port))
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_CPU_PORT == interface.type)
    {
        if (RULE_TYPE_OUTBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_TRUNK == interface.type)
    {
        if (RULE_TYPE_INBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_ALL_UNIT == interface.type)
    {
        if (RULE_TYPE_INBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_OM_GetAclByIndex(acl_id, &acl_entry);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Invalid parameter");
        RULE_CTRL_LeaveCriticalSectionReturnState(result);
    }

    result = RULE_CTRL_LocalSetAcl(&interface,
                                   acl_id,
                                   &acl_entry,
                                   is_add,
                                   time_range_index,
                                   counter_enable);

    if (RULE_TYPE_OK != result)
    {
        RULE_CTRL_LeaveCriticalSectionReturnState(result);
    }

    RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_GetQosCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Get diffserv counter
 * INPUT:    ifindex - port index
 *           direction - ingress or egress
 *           policy_id - policy-map id
 *           class_id - class-map id
 * OUTPUT:   counter
 * RETURN:   Error code
 * NOTE: None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_GetQosCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_id,
    UI32_T class_id,
    RULE_TYPE_CounterRec_T *counter)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;
    RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalGetFirstRuleInstanceOfQos(ifindex, direction, policy_id, class_id, &rule_inst_iter);
    if (result != RULE_TYPE_OK)
    {
        RULE_CTRL_LeaveCriticalSectionReturnState(result);
    }

    rule_inst_iter.first(&rule_inst_iter);	
    rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T)rule_inst_iter.get_instance(&rule_inst_iter);
    if (rule_inst_p == NULL)
    {
        LOG("<Error> Can't get rule");
        RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }
    LOG("pcl_id: %lu, rule_id: %lu", rule_inst_p->dev_rule_info.pcl_id, rule_inst_p->dev_rule_info.rule_id);

    if (TRUE != RULE_CTRL_LocalGetCounter(rule_inst_p->dev_rule_info.unit,
                                             rule_inst_p->dev_rule_info.device,
                                             rule_inst_p->dev_rule_info.pcl_id,
                                             rule_inst_p->dev_rule_info.rule_id,
                                             &counter->green_packets,
                                             &counter->yellow_packets,
                                             &counter->red_packets))
    {
        RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_ClearQosCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Clear diffserv counter
 * INPUT:    ifindex - port index
 *           direction - ingress or egress
 *           policy_id - policy-map id
 *           class_id - class-map id
 * OUTPUT:   None
 * RETURN:   Error code
 * NOTE: None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_ClearQosCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_id,
    UI32_T class_id)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;
    RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalGetFirstRuleInstanceOfQos(ifindex, direction, policy_id, class_id, &rule_inst_iter);
    if (result != RULE_TYPE_OK)
    {
        if (result == RULE_TYPE_NO_HW_RULE_SET)
        {
            result = RULE_TYPE_OK;
        }
        RULE_CTRL_LeaveCriticalSectionReturnState(result);
    }

    rule_inst_iter.first(&rule_inst_iter);	
    rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T)rule_inst_iter.get_instance(&rule_inst_iter);
    if (rule_inst_p == NULL)
    {
        LOG("<Error> Can't get rule");
        RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }
    LOG("pcl_id: %lu, rule_id: %lu", rule_inst_p->dev_rule_info.pcl_id, rule_inst_p->dev_rule_info.rule_id);

    if (TRUE != RULE_CTRL_LocalClearCounter(rule_inst_p->dev_rule_info.unit,
                                             rule_inst_p->dev_rule_info.device,
                                             rule_inst_p->dev_rule_info.pcl_id,
                                             rule_inst_p->dev_rule_info.rule_id))
    {
        RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}
#endif /*#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)*/

#if (SYS_CPNT_ACL_COUNTER == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_GetAclRuleCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Get counter for ACE of ACL
 * INPUT:    acl_id     - ACL ID
 *           ace_id     - ACE ID
 * OUTPUT:   ace_entry  - ACE object
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_GetAclRuleCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_index,
    UI32_T ace_index,
    RULE_TYPE_Ace_Entry_T *ace_entry)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_INTERFACE_INFO_T interface;

    //
    // --== Duplicated code start
    //
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
    RULE_TYPE_CLASS_INSTANCE_PTR_T ace_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T ace_inst_obj;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;
    //
    // --== End of duplicated code
    //

    BOOL_T b_result;

    memset(&interface, 0, sizeof(interface));

    result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    interface.direction = direction;

    LOG_IF(interface, "AclId = %lu, AceId = %lu",
           acl_index,
           ace_index);

    if (RULE_TYPE_INTERFACE_UPORT != interface.type &&
        RULE_TYPE_INTERFACE_TRUNK != interface.type &&
        RULE_TYPE_INTERFACE_CPU_PORT != interface.type &&
        RULE_TYPE_INTERFACE_ALL_UNIT != interface.type)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_INBOUND != interface.direction &&
        RULE_TYPE_OUTBOUND != interface.direction)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_INTERFACE_UPORT == interface.type)
    {
        UI32_T module_id;
        UI32_T device_id;
        UI32_T phy_port;

        module_id = 0;
        device_id = 0;
        phy_port  = 0;

        if (FALSE == DEV_SWDRV_PMGR_Logical2PhyDevicePortID(interface.uport.unit,
                                                            interface.uport.port,
                                                            &module_id,
                                                            &device_id,
                                                            &phy_port))
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_CPU_PORT == interface.type)
    {
        if (RULE_TYPE_OUTBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_TRUNK == interface.type)
    {
        if (RULE_TYPE_INBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_ALL_UNIT == interface.type)
    {
        if (RULE_TYPE_INBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    RULE_CTRL_EnterCriticalSection();

    //
    // --== Duplicated code start
    //
    if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);
    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_ACL;
    param.id = acl_index;

    acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                &param);
    //
    // --== End of duplicated code
    //
    if (NULL == acl_inst_p ||
        RULE_TYPE_COUNTER_ENABLE != acl_inst_p->counter_enable)
    {
        RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_ACE;
    param.id = ace_index;

    ace_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) acl_inst_p,
                                                                                RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                &param);
    if (NULL == ace_inst_p)
    {
        LOG("<Error> Invalid parameter: ace_index = %lu", ace_index);
        RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_INVALID_PARAMETER);
    }

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(ace_inst_p, &ace_inst_obj);
    ace_inst_obj.get_element_iterator(&ace_inst_obj, &rule_inst_iter);

    memset(&ace_entry->counter, 0, sizeof(ace_entry->counter));

    for (rule_inst_iter.first(&rule_inst_iter);
         NULL != rule_inst_iter.get_instance(&rule_inst_iter);
         rule_inst_iter.next(&rule_inst_iter))
    {
        RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;
        RULE_TYPE_CounterRec_T *counter_p;

        rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T)rule_inst_iter.get_instance(&rule_inst_iter);

        ASSERT(rule_inst_p->current_counter_index < _countof(rule_inst_p->counter));

        counter_p = &rule_inst_p->counter[(rule_inst_p->current_counter_index + 1)
                                          % _countof(rule_inst_p->counter)];

        b_result = RULE_CTRL_LocalGetCounter(rule_inst_p->dev_rule_info.unit,
                                             rule_inst_p->dev_rule_info.device,
                                             rule_inst_p->dev_rule_info.pcl_id,
                                             rule_inst_p->dev_rule_info.rule_id,
                                             &counter_p->green_packets,
                                             &counter_p->yellow_packets,
                                             &counter_p->red_packets);

        if (FALSE == b_result)
        {
            LOG("GetCounter failed: aclId = %lu, aceId = %lu, ruleId = %lu",
                acl_index, ace_index, rule_inst_p->dev_rule_info.rule_id);
            RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
        }

        rule_inst_p->current_counter_index = (rule_inst_p->current_counter_index + 1)
                                                % _countof(rule_inst_p->counter);

        ASSERT(rule_inst_p->current_counter_index < _countof(rule_inst_p->counter));
        ASSERT(counter_p == &rule_inst_p->counter[rule_inst_p->current_counter_index]);

        counter_p->last_update_tick = SYSFUN_GetSysTick();
        counter_p->total_packets = counter_p->green_packets + counter_p->yellow_packets
                                                            + counter_p->red_packets;

        ace_entry->counter.green_packets += counter_p->green_packets;
        ace_entry->counter.yellow_packets += counter_p->yellow_packets;
        ace_entry->counter.red_packets += counter_p->red_packets;
    }

    ace_entry->counter.last_update_tick = SYSFUN_GetSysTick();
    ace_entry->counter.total_packets = ace_entry->counter.green_packets + ace_entry->counter.yellow_packets
                                                                        + ace_entry->counter.red_packets;

    RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_ClearAclRuleCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Clear counter for ACE of ACL
 * INPUT:    interface  - The interface.
 *                        The type of this parameter should be
 *                        RULE_TYPE_INTERFACE_UPORT
 *                        The direction of this parameter should be one of the
 *                        values:
 *                        RULE_TYPE_INBOUND     - Ingress direction
 *                        RULE_TYPE_OUTBOUND    - Egress direction
 *           acl_id     - ACL ID
 *           ace_id     - ACE ID
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_ClearAclRuleCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_index,
    UI32_T ace_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_INTERFACE_INFO_T interface;

    //
    // --== Duplicated code start
    //
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
    RULE_TYPE_CLASS_INSTANCE_PTR_T ace_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T ace_inst_obj;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;
    //
    // --== End of duplicated code
    //

    BOOL_T b_result;

    memset(&interface, 0, sizeof(interface));

    result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    interface.direction = direction;

    LOG_IF(interface, "AclId = %lu, AceId = %lu",
           acl_index,
           ace_index);

    if (RULE_TYPE_INTERFACE_UPORT != interface.type &&
        RULE_TYPE_INTERFACE_TRUNK != interface.type &&
        RULE_TYPE_INTERFACE_CPU_PORT != interface.type &&
        RULE_TYPE_INTERFACE_ALL_UNIT != interface.type)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_INBOUND != interface.direction &&
        RULE_TYPE_OUTBOUND != interface.direction)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_INTERFACE_UPORT == interface.type)
    {
        UI32_T module_id;
        UI32_T device_id;
        UI32_T phy_port;

        module_id = 0;
        device_id = 0;
        phy_port  = 0;

        if (FALSE == DEV_SWDRV_PMGR_Logical2PhyDevicePortID(interface.uport.unit,
                                                            interface.uport.port,
                                                            &module_id,
                                                            &device_id,
                                                            &phy_port))
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_CPU_PORT == interface.type)
    {
        if (RULE_TYPE_OUTBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_TRUNK == interface.type)
    {
        if (RULE_TYPE_INBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_ALL_UNIT == interface.type)
    {
        if (RULE_TYPE_INBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    RULE_CTRL_EnterCriticalSection();

    //
    // --== Duplicated code start
    //
    if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);
    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_ACL;
    param.id = acl_index;

    acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                &param);
    //
    // --== End of duplicated code
    //
    if (NULL == acl_inst_p ||
        RULE_TYPE_COUNTER_ENABLE != acl_inst_p->counter_enable)
    {
        RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_ACE;
    param.id = ace_index;

    ace_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) acl_inst_p,
                                                                                RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                &param);
    if (NULL == ace_inst_p)
    {
        LOG("<Error> Invalid parameter: ace_index = %lu", ace_index);
        RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_INVALID_PARAMETER);
    }

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(ace_inst_p, &ace_inst_obj);
    ace_inst_obj.get_element_iterator(&ace_inst_obj, &rule_inst_iter);

    for (rule_inst_iter.first(&rule_inst_iter);
         NULL != rule_inst_iter.get_instance(&rule_inst_iter);
         rule_inst_iter.next(&rule_inst_iter))
    {
        RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;

        rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T)rule_inst_iter.get_instance(&rule_inst_iter);

        b_result = RULE_CTRL_LocalClearCounter(rule_inst_p->dev_rule_info.unit,
                                               rule_inst_p->dev_rule_info.device,
                                               rule_inst_p->dev_rule_info.pcl_id,
                                               rule_inst_p->dev_rule_info.rule_id);

        if (FALSE == b_result)
        {
            LOG("ClearCounter failed: aclId = %lu, pclId = %lu, aceId = %lu, ruleId = %lu",
                acl_index,
                rule_inst_p->dev_rule_info.pcl_id,
                rule_inst_p->ace_id,
                rule_inst_p->dev_rule_info.rule_id);

            RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
        }
    }

    RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}
#endif /* #if (SYS_CPNT_ACL_COUNTER == TRUE) */

#if (SYS_CPNT_ACL_MIRROR == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetAclMirror
 *------------------------------------------------------------------------------
 * PURPOSE:  Apply ACL mirror
 * INPUT:    acl_id     - ACL ID
 *           ace_id     - ACE ID
 * OUTPUT:   ace_entry  - ACE object
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_SetAclMirror(
    UI32_T acl_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T if_entry_index;

    RULE_CTRL_EnterCriticalSection();

    for (if_entry_index = 0;
         if_entry_index < _countof(shmem_data_p->if_table);
         ++ if_entry_index)
    {
        RULE_TYPE_INTERFACE_INFO_T  interface;

        RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

        RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

        RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;

        RULE_CTRL_INSTANCE_TYPE_ID_T param;

        RULE_CTRL_Priv_IfEntry_Index_To_Interface(if_entry_index, &interface);

        if (RULE_TYPE_INBOUND != interface.direction)
        {
            continue;
        }

        if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
        ASSERT(NULL != if_entry_p);

        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                  &if_class_obj);

        memset(&param, 0, sizeof(param));

        param.type = RULE_TYPE_INST_ACL;
        param.id = acl_index;

        acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                    RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                    &param);
        if (NULL != acl_inst_p)
        {
            RULE_TYPE_COUNTER_ENABLE_T  counter_enable;
            UI32_T                      time_range_index;

            RULE_TYPE_Acl_T             acl_entry_new;

            counter_enable   = RULE_TYPE_COUNTER_DISABLE;
            time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
            time_range_index = acl_inst_p->time_range_index;
#endif /* SYS_CPNT_TIME_BASED_ACL */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
            counter_enable = acl_inst_p->counter_enable;
#endif /* SYS_CPNT_ACL_COUNTER */

            /* FIXME: Quick way FIXME LATER
             * Rebind ACL --> next, we should just update rule not rebind all.
             */

            result = RULE_OM_GetAclByIndex(acl_index, &acl_entry_new);
            ASSERT(RULE_TYPE_OK == result);

            result = RULE_CTRL_LocalSetAcl(&interface,
                                           acl_index,
                                           &acl_entry_new,
                                           FALSE,    /* is_add */
                                           time_range_index,
                                           counter_enable);
            /* How to recovery ?
             */
            ASSERT(RULE_TYPE_OK == result);
            if (RULE_TYPE_OK != result)
            {
                RULE_CTRL_LeaveCriticalSectionReturnState(result);
            }

            result = RULE_OM_GetAclByIndex(acl_index, &acl_entry_new);
            ASSERT(RULE_TYPE_OK == result);

            result = RULE_CTRL_LocalSetAcl(&interface,
                                           acl_index,
                                           &acl_entry_new,
                                           TRUE,     /* is_add */
                                           time_range_index,
                                           counter_enable);
            ASSERT(RULE_TYPE_OK == result);
            /* How to recovery ?
             */
            if (RULE_TYPE_OK != result)
            {
                RULE_CTRL_LeaveCriticalSectionReturnState(result);
            }
        }
    }

    RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}
#endif /* #if (SYS_CPNT_ACL_MIRROR == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetMf
 *------------------------------------------------------------------------------
 * PURPOSE  : Set a MF entry.
 * INPUT    : class_map_index, mf_index, is_add
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : is_add == FALSE implies remove MF
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_SetMf(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T mf_index,
    UI32_T meter_id,
    UI32_T action_id,
    BOOL_T is_add)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_INTERFACE_INFO_T interface;

    memset(&interface, 0, sizeof(interface));

    result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    interface.direction = direction;

    if (RULE_TYPE_INTERFACE_UPORT != interface.type &&
        RULE_TYPE_INTERFACE_TRUNK != interface.type &&
        RULE_TYPE_INTERFACE_CPU_PORT != interface.type &&
        RULE_TYPE_INTERFACE_ALL_UNIT != interface.type)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_INBOUND != interface.direction &&
        RULE_TYPE_OUTBOUND != interface.direction)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_INTERFACE_UPORT == interface.type)
    {
        UI32_T module_id;
        UI32_T device_id;
        UI32_T phy_port;

        module_id = 0;
        device_id = 0;
        phy_port  = 0;

        if (FALSE == DEV_SWDRV_PMGR_Logical2PhyDevicePortID(interface.uport.unit,
                                                            interface.uport.port,
                                                            &module_id,
                                                            &device_id,
                                                            &phy_port))
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_CPU_PORT == interface.type)
    {
        if (RULE_TYPE_OUTBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_TRUNK == interface.type)
    {
        if (RULE_TYPE_INBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_ALL_UNIT == interface.type)
    {
        if (RULE_TYPE_INBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    RULE_CTRL_EnterCriticalSection();

    if (TRUE == is_add)
    {
        result = RULE_CTRL_LocalSetMF2DEV(&interface,
                                          policy_map_index,
                                          class_map_index,
                                          mf_index,
                                          meter_id,
                                          action_id);
    }
    else
    {
        result = RULE_CTRL_LocalRemoveMF2DEV(&interface,
                                             policy_map_index,
                                             class_map_index,
                                             mf_index, meter_id, action_id);
    }

    RULE_CTRL_LeaveCriticalSectionReturnState(result);
}

///*------------------------------------------------------------------------------
// * ROUTINE NAME - RULE_CTRL_SetClassMap
// *------------------------------------------------------------------------------
// * PURPOSE  : Set rule for class-map.
// * INPUT    : unit              - unit
// *            port              - port
// *            inout_profile     - direction
// *            class_map_index   - class-map index
// *            meter_id          - meter index
// *            action_id         - action index
// *            is_add            - TRUE, set rule to chip; FALSE, remove rule
// * OUTPUT   : none
// * RETURN   : TRUE/FALSE.
// * NOTE     : none
// *------------------------------------------------------------------------------
// */
//RULE_TYPE_RETURN_TYPE_T
//RULE_CTRL_SetClassMap(
//    UI32_T ifindex,
//    RULE_TYPE_InOutDirection_T direction,
//    UI32_T policy_map_index,
//    UI32_T class_map_index,
//    UI32_T meter_id,
//    UI32_T action_id,
//    BOOL_T is_add)
//{
//    RULE_TYPE_RETURN_TYPE_T result;
//
//    RULE_TYPE_INTERFACE_INFO_T interface;
//
//    RULE_TYPE_PolicyMap_T policy_map;
//
//    memset(&interface, 0, sizeof(interface));
//
//    result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
//    if (RULE_TYPE_OK != result)
//    {
//        LOG("<Error> Invalid parameter");
//        return RULE_TYPE_INVALID_PARAMETER;
//    }
//
//    interface.direction = direction;
//
//    if (RULE_TYPE_INTERFACE_UPORT != interface.type &&
//        RULE_TYPE_INTERFACE_TRUNK != interface.type &&
//        RULE_TYPE_INTERFACE_CPU_PORT != interface.type &&
//        RULE_TYPE_INTERFACE_ALL_UNIT != interface.type)
//    {
//        LOG("<Error> Invalid parameter");
//        return RULE_TYPE_INVALID_PARAMETER;
//    }
//
//    if (RULE_TYPE_INBOUND != interface.direction &&
//        RULE_TYPE_OUTBOUND != interface.direction)
//    {
//        LOG("<Error> Invalid parameter");
//        return RULE_TYPE_INVALID_PARAMETER;
//    }
//
//    if (RULE_TYPE_INTERFACE_UPORT == interface.type)
//    {
//        UI32_T module_id;
//        UI32_T device_id;
//        UI32_T phy_port;
//
//        module_id = 0;
//        device_id = 0;
//        phy_port  = 0;
//
//        if (FALSE == DEV_SWDRV_PMGR_Logical2PhyDevicePortID(interface.uport.unit,
//                                                            interface.uport.port,
//                                                            &module_id,
//                                                            &device_id,
//                                                            &phy_port))
//        {
//            LOG("<Error> Invalid parameter");
//            return RULE_TYPE_INVALID_PARAMETER;
//        }
//    }
//    else if (RULE_TYPE_INTERFACE_CPU_PORT == interface.type)
//    {
//        if (RULE_TYPE_OUTBOUND != interface.direction)
//        {
//            LOG("<Error> Invalid parameter");
//            return RULE_TYPE_INVALID_PARAMETER;
//        }
//    }
//    else if (RULE_TYPE_INTERFACE_TRUNK == interface.type)
//    {
//        if (RULE_TYPE_INBOUND != interface.direction)
//        {
//            LOG("<Error> Invalid parameter");
//            return RULE_TYPE_INVALID_PARAMETER;
//        }
//    }
//    else if (RULE_TYPE_INTERFACE_ALL_UNIT == interface.type)
//    {
//        if (RULE_TYPE_INBOUND != interface.direction)
//        {
//            LOG("<Error> Invalid parameter");
//            return RULE_TYPE_INVALID_PARAMETER;
//        }
//    }
//
//    RULE_CTRL_EnterCriticalSection();
//
//    result = RULE_OM_GetPolicyMap(policy_map_index, &policy_map);
//    if (RULE_TYPE_OK != result)
//    {
//        LOG("<Error> Invalid parameter");
//        RULE_CTRL_LeaveCriticalSectionReturnState(result);
//    }
//
//    if (TRUE == is_add)
//    {
//        result = RULE_CTRL_LocalSetClassMap2DEV(&interface,
//                                                policy_map_index,
//                                                &policy_map,
//                                                class_map_index,
//                                                meter_id,
//                                                action_id);
//    }
//    else
//    {
//        result = RULE_CTRL_LocalRemoveClassMap2DEV(&interface,
//                                                   policy_map_index,
//                                                   &policy_map,
//                                                   class_map_index,
//                                                   meter_id,
//                                                   action_id);
//    }
//
//    RULE_CTRL_LeaveCriticalSectionReturnState(result);
//}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE  : Set rule for a policy-map.
 * INPUT    : unit              - unit
 *            port              - port
 *            inout_profile     - direction
 *            policy_map_index  - policy-map index
 *            is_add            - TRUE, set rule to chip; FALSE, remove rule
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_SetPolicyMap(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_map_index,
    BOOL_T is_add)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_INTERFACE_INFO_T interface;

    RULE_TYPE_PolicyMap_T policy_map;

    memset(&interface, 0, sizeof(interface));

    result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    interface.direction = direction;

    if (RULE_TYPE_INTERFACE_UPORT != interface.type &&
        RULE_TYPE_INTERFACE_TRUNK != interface.type &&
        RULE_TYPE_INTERFACE_CPU_PORT != interface.type &&
        RULE_TYPE_INTERFACE_ALL_UNIT != interface.type)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_INBOUND != interface.direction &&
        RULE_TYPE_OUTBOUND != interface.direction)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_INTERFACE_UPORT == interface.type)
    {
        UI32_T module_id;
        UI32_T device_id;
        UI32_T phy_port;

        module_id = 0;
        device_id = 0;
        phy_port  = 0;

        if (FALSE == DEV_SWDRV_PMGR_Logical2PhyDevicePortID(interface.uport.unit,
                                                            interface.uport.port,
                                                            &module_id,
                                                            &device_id,
                                                            &phy_port))
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_CPU_PORT == interface.type)
    {
        if (RULE_TYPE_OUTBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_TRUNK == interface.type)
    {
        if (RULE_TYPE_INBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_ALL_UNIT == interface.type)
    {
        if (RULE_TYPE_INBOUND != interface.direction)
        {
            LOG("<Error> Invalid parameter");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_OM_GetPolicyMap(policy_map_index, &policy_map);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Invalid parameter");
        RULE_CTRL_LeaveCriticalSectionReturnState(result);
    }

    if (TRUE == is_add)
    {
        result = RULE_CTRL_LocalSetPolicyMap2DEV(&interface,
                                                 policy_map_index,
                                                 &policy_map);
    }
    else
    {
        result = RULE_CTRL_LocalRemovePolicyMap2DEV(&interface,
                                                    policy_map_index,
                                                    &policy_map);
    }

    RULE_CTRL_LeaveCriticalSectionReturnState(result);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_NotifyIpDscp2CosChangeByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : caller notify rule_ctrl which dscp to cos mapping had been changed
 * INPUT    : lport, ip_dscp
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : for policy-map CLI command "set ip dscp xxx"
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_NotifyIpDscp2CosChangeByLport(UI32_T lport, UI8_T ip_dscp)
{
    UI32_T  index, policy_map_index, class_map_index;

    /* in current implementation, COS reference don't care which dscp of port */

    index = 0;
    while (RULE_TYPE_OK == RULE_OM_GetNextIpDscpReference(ip_dscp, &index, &policy_map_index, &class_map_index))
    {
        /* because DSCP to CoS value mapping has been changed,
         * undo original config first then config again (with new CoS mapping)
         */
        if ((FALSE == RULE_CTRL_RemoveClassMapConfig(policy_map_index, class_map_index, FALSE)) ||
            (FALSE == RULE_CTRL_AddClassMapConfig(policy_map_index, class_map_index)))
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_NotifyIpPrecedence2CosChangeByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : caller notify rule_ctrl which ip precedence to cos mapping had been changed
 * INPUT    : lport, ip_precedence
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : for policy-map CLI command "set ip precedence xxx"
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_NotifyIpPrecedence2CosChangeByLport(UI32_T lport, UI8_T ip_precedence)
{
    UI32_T  index, policy_map_index, class_map_index;

    /* in current implementation, COS reference don't care which dscp of port */

    index = 0;
    while (RULE_TYPE_OK == RULE_OM_GetNextIpPrecedenceReference(ip_precedence, &index, &policy_map_index, &class_map_index))
    {
        /* because IP Precedence to CoS value mapping has been changed,
         * undo original config first then config again (with new CoS mapping)
         */
        if ((FALSE == RULE_CTRL_RemoveClassMapConfig(policy_map_index, class_map_index, FALSE)) ||
            (FALSE == RULE_CTRL_AddClassMapConfig(policy_map_index, class_map_index)))
        {
            return FALSE;
        }
    }

    return TRUE;
}

#pragma mark End of ACL / Diffserv Functions
#pragma mark -

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_TrapPacket2Cpu
 *------------------------------------------------------------------------------
 * PURPOSE:  trap specified packet to CPU
 * INPUT:    enable_flag, packet_type, rule_info
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     Not implement
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_TrapPacket2Cpu(
    BOOL_T enable_flag,
    RULE_TYPE_PacketType_T packet_type,
    RULE_TYPE_CpuRuleInfo_T *rule_info)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T  remapped_pkt_type = RULE_TYPE_PACKET_MAX_TYPE;

    RULE_CTRL_OPT_FLAGS_T   flags;

    RULE_CTRL_OPTIONS_T options;
    DEVRM_ACTION_T dev_action;

    ASSERT(packet_type < RULE_TYPE_PacketType_MAX);

    LOG("enable=%s, packet type=%s(%lu), ruleInfo=%s",
        TRUE == enable_flag ? "TRUE" : "FALSE",
        RULE_CTRL_PACKET_TYPE_STR(packet_type),
        packet_type,
        NULL == rule_info ? "(nil)" : "(!nil)");

    RULE_CTRL_RemapPacketType(packet_type, &remapped_pkt_type);

    /* Some packet type will be trapped from just register configuraiton.
     * So it needn't to configure it through rule, so treated it as
     * RULE_TYPE_PACKET_MAX_TYPE and return TRUE to core layer to
     * prevent the error message printing.
     */
    if (remapped_pkt_type == RULE_TYPE_PACKET_MAX_TYPE)
    {
        return TRUE;
    }

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable_flag)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    /* Set default action. It will be overwrite by input options.
     */
    if (TRUE == enable_flag)
    {
        if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_COPY_TO_CPU))
        {
            LOG("<Error> Set packet command");
            return FALSE;
        }
    }

    memset(&options, 0, sizeof(options));

    /* Backward compatible
     */
    options.cpu_rule_info_p = rule_info;

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, remapped_pkt_type,
                NULL, &dev_action, &options);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_MirrorPacket2Cpu
 *------------------------------------------------------------------------------
 * PURPOSE:  This function is used to Copy frame to CPU.
 *           The packet is still forwarded by bridge mechenism.
 * INPUT:    enable_flag, packet_type, rule_info
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     Not implement
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_MirrorPacket2Cpu(BOOL_T enable_flag, RULE_TYPE_PacketType_T packet_type, RULE_TYPE_CpuRuleInfo_T *rule_info)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T  remapped_pkt_type = RULE_TYPE_PACKET_MAX_TYPE;

    RULE_CTRL_OPT_FLAGS_T   flags;

    DEVRM_ACTION_T dev_action;

    ASSERT(packet_type < RULE_TYPE_PacketType_MAX);

    LOG("enable=%s, packet type=%s(%lu), ruleInfo=%s",
        TRUE == enable_flag ? "TRUE" : "FALSE",
        RULE_CTRL_PACKET_TYPE_STR(packet_type),
        packet_type,
        NULL == rule_info ? "(nil)" : "(!nil)");

    RULE_CTRL_RemapPacketType(packet_type, &remapped_pkt_type);

    /* Some packet type will be trapped from just register configuraiton.
     * So it needn't to configure it through rule, so treated it as
     * RULE_TYPE_PACKET_MAX_TYPE and return TRUE to core layer to
     * prevent the error message printing.
     */
    if (remapped_pkt_type == RULE_TYPE_PACKET_MAX_TYPE)
    {
        return TRUE;
    }

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable_flag)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE == enable_flag)
    {
        if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_COPY_TO_CPU))
        {
            LOG("<Error> Set packet command");
            return FALSE;
        }
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, remapped_pkt_type,
                NULL, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_TrapPacket2CpuForSwdrv
 *------------------------------------------------------------------------------
 * PURPOSE:  trap specified packet to CPU
 * INPUT:    enable_flag, packet_type, rule_info
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE
 * NOTE:     1. dip is used as do_action for DHCPS_BC & DHCPC_BC (DHCPSNP)
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_TrapPacket2CpuForSwdrv(BOOL_T enable_flag, RULE_TYPE_PacketType_T packet_type, RULE_TYPE_CpuRuleInfo_T *rule_info)
{
    if (rule_info == NULL)
        return FALSE;
    return RULE_CTRL_TrapPacket2Cpu(enable_flag,packet_type, rule_info);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetTcpUdpPortCosMap
 *------------------------------------------------------------------------------
 * PURPOSE:  set tcp/udp destination port mapped to cos by unit and port
 * INPUT:    unit, port, is_tcp, dest_port, phb, color
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:     Not implement
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetTcpUdpPortCosMap(UI32_T unit, UI32_T port, BOOL_T is_tcp, UI16_T dest_port, UI8_T phb, UI8_T color, BOOL_T is_update)
{
    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DeleteTcpUdpPortCosMap
 *------------------------------------------------------------------------------
 * PURPOSE:  delete tcp/udp destination port mapped to cos by unit and port
 * INPUT:    unit, port, is_tcp, dest_port, phb, color
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:     Not implement
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DeleteTcpUdpPortCosMap(UI32_T unit, UI32_T port, BOOL_T is_tcp, UI16_T dest_port, UI8_T phb, UI8_T color)
{
    return FALSE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_SetMacVlanEntry
 *------------------------------------------------------------------------------
 * PURPOSE : Add PCEs for MAC based VLAN
 * INPUT   : mac_address        -- MAC address
 *           vid                -- VLAN id
 *           priority           -- priority
 *  OUTPUT : None
 *  RETURN : TRUE - OK / FALSE - FAILED
 *    NOTE : PCE will be added /updated if PCE does not exist /exists.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetMacVlanEntry(UI8_T *mac_address, UI8_T *mask, UI16_T vid, UI8_T priority)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    memset(&flags, 0, sizeof(flags));
    flags.INSTALL = 1;
    flags.FORCE_INSTALL = 1;

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifySourceMacAddress(&rule_pattern, mac_address, mask))
    {
        LOG("<Error> Qualify SA");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    /* untagged packet and priority tag (VLAN = 0)
     */
    if (TRUE != DEVRM_LIB_ActionModifyVlan(&dev_action, DEVRM_MODIFY_VLAN_CMD_FOR_UNTAGGED_ONLY, vid, FALSE))
    {
        LOG("<Error> Modify VLAN");
        return FALSE;
    }

    // FIXME: Bad sys cpnt. The priority value does need to update should decide
    //        by caller not use a sys cpnt here
#if (SYS_CPNT_MAC_VLAN_WITH_PRIORITY == TRUE)
    /* MAC VLAN should only change the priority value for untagged packet
     * (which include priority tag, the VLAN = 0).
     * But this action will change the priority value for ALL packet.
     */
    if (TRUE != DEVRM_LIB_ActionModifyPriority(&dev_action, priority))
    {
        LOG("<Error> Modify priority");
        return FALSE;
    }
#endif

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, RULE_TYPE_PACKET_MAC_BASED_VLAN,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_DeleteMacVlanEntry
 *------------------------------------------------------------------------------
 * PURPOSE : Add PCEs for MAC based VLAN
 * INPUT   : mac_address        -- MAC address
 *  OUTPUT : None
 *  RETURN : TRUE - OK / FALSE - FAILED
 *    NOTE : PCE will be added /updated if PCE does not exist /exists.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DeleteMacVlanEntry(UI8_T *mac_address, UI8_T *mask)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    memset(&flags, 0, sizeof(flags));

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifySourceMacAddress(&rule_pattern, mac_address, mask))
    {
        LOG("<Error> Qualify SA");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, RULE_TYPE_PACKET_MAC_BASED_VLAN,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_SetMacMirrorEntry
 *------------------------------------------------------------------------------
 * PURPOSE : Add PCEs for MAC based MIRROR
 * INPUT   : mac_address        -- MAC address
 * OUTPUT  : None
 * RETURN  : TRUE - OK / FALSE - FAILED
 * NOTE    : PCE will be added /updated if PCE does not exist /exists.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetMacMirrorEntry(UI8_T *mac_address)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    memset(&flags, 0, sizeof(flags));

    flags.INSTALL = 1;
    flags.FORCE_INSTALL = 1;

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifySourceMacAddress(&rule_pattern, mac_address, mask))
    {
        LOG("<Error> Qualify SA");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_MIRROR_TO_ANALYZER_PORT))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, RULE_TYPE_PACKET_MAC_BASED_MIRROR,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_DeleteMacMirrorEntry
 *------------------------------------------------------------------------------
 * PURPOSE : Add PCEs for MAC based MIRROR
 * INPUT   : mac_address        -- MAC address
 * OUTPUT  : None
 * RETURN  : TRUE - OK / FALSE - FAILED
 * NOTE    : PCE will be added /updated if PCE does not exist /exists.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DeleteMacMirrorEntry(UI8_T *mac_address)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T   flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    memset(&flags, 0, sizeof(flags));

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifySourceMacAddress(&rule_pattern, mac_address, mask))
    {
        LOG("<Error> Qualify SA");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, RULE_TYPE_PACKET_MAC_BASED_MIRROR,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_SetIpSubnetVlanEntry
 *------------------------------------------------------------------------------
 * PURPOSE : Add PCEs for IP subnet based VLAN
 * INPUT   : ip_address         -- IP address
 *           mask               -- IP address subnet
 *           vid                -- VLAN id
 *           priority           -- priority
 *  OUTPUT : None
 *  RETURN : TRUE - OK / FALSE - FAILED
 *    NOTE : PCE will be added /updated if PCE does not exist /exists.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetIpSubnetVlanEntry(UI32_T ip_address, UI32_T mask, UI16_T vid, UI8_T priority)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    memset(&flags, 0, sizeof(flags));

    flags.INSTALL = 1;
    flags.FORCE_INSTALL = 1;

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIpSourceAddress(&rule_pattern, ip_address, mask))
    {
        LOG("<Error> Qualify IP source address");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_ActionModifyVlan(&dev_action, DEVRM_MODIFY_VLAN_CMD_FOR_UNTAGGED_ONLY, vid, FALSE))
    {
        LOG("<Error> Modify VLAN");
        return FALSE;
    }

    // FIXME: Bad sys cpnt. The priority value does need to update should decide
    //        by caller not use a sys cpnt here
#if (SYS_CPNT_IP_SUBNET_VLAN_WITH_PRIORITY == TRUE)
    if (TRUE != DEVRM_LIB_ActionModifyPriority(&dev_action, priority))
    {
        LOG("<Error> Modify priority");
        return FALSE;
    }
#endif

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, RULE_TYPE_PACKET_IP_SUBNET_BASED_VLAN,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_DeleteIpSubnetVlanEntry
 *------------------------------------------------------------------------------
 * PURPOSE : Add PCEs for IP subnet based VLAN
 * INPUT   : ip_address         -- IP address
 *           mask               -- IP address subnet
 *  OUTPUT : None
 *  RETURN : TRUE - OK / FALSE - FAILED
 *    NOTE : PCE will be added /updated if PCE does not exist /exists.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DeleteIpSubnetVlanEntry(UI32_T ip_address, UI32_T mask)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T   flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    memset(&flags, 0, sizeof(flags));

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIpSourceAddress(&rule_pattern, ip_address, mask))
    {
        LOG("<Error> Qualify IP source address");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, RULE_TYPE_PACKET_IP_SUBNET_BASED_VLAN,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

#if (SYS_CPNT_IP_SUBNET_VLAN_BIND_PORT == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_SetIpSubnetVlanEntryPerPort
 *------------------------------------------------------------------------------
 * PURPOSE : Add PCEs for IP subnet based VLAN
 * INPUT   : unit               -- unit
 *           port               -- port
 *           ip_address         -- IP address
 *           mask               -- IP address subnet
 *           vid                -- VLAN id
 *           priority           -- priority
 *  OUTPUT : None
 *  RETURN : TRUE - OK / FALSE - FAILED
 *    NOTE : PCE will be added /updated if PCE does not exist /exists.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetIpSubnetVlanEntryPerPort(
    UI32_T unit,
    UI32_T port,
    UI32_T ip_address,
    UI32_T mask,
    UI16_T vid,
    UI8_T priority)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T   flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    memset(&flags, 0, sizeof(flags));
    flags.INSTALL = 1;
    flags.FORCE_INSTALL = 1;

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIpSourceAddress(&rule_pattern, ip_address, mask))
    {
        LOG("<Error> Qualify IP source address");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_ActionModifyVlan(&dev_action, DEVRM_MODIFY_VLAN_CMD_FOR_UNTAGGED_ONLY, vid, FALSE))
    {
        LOG("<Error> Modify VLAN");
        return FALSE;
    }

    // FIXME: Bad sys cpnt. The priority value does need to update should decide
    //        by caller not use a sys cpnt here
#if (SYS_CPNT_IP_SUBNET_VLAN_WITH_PRIORITY == TRUE)
    if (TRUE != DEVRM_LIB_ActionModifyPriority(&dev_action, priority))
    {
        LOG("<Error> Modify priority");
        return FALSE;
    }
#endif

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit, port, RULE_TYPE_PACKET_IP_SUBNET_BASED_VLAN_PER_PORT, &rule_pattern, &dev_action);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_DeleteIpSubnetVlanEntryPerPort
 *------------------------------------------------------------------------------
 * PURPOSE : Add PCEs for IP subnet based VLAN
 * INPUT   : unit               -- unit
 *           port               -- port
 *           ip_address         -- IP address
 *           mask               -- IP address subnet
 *  OUTPUT : None
 *  RETURN : TRUE - OK / FALSE - FAILED
 *    NOTE : PCE will be added /updated if PCE does not exist /exists.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DeleteIpSubnetVlanEntryPerPort(
    UI32_T unit,
    UI32_T port,
    UI32_T ip_address,
    UI32_T mask)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T   flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    memset(&flags, 0, sizeof(flags));

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIpSourceAddress(&rule_pattern, ip_address, mask))
    {
        LOG("<Error> Qualify IP source address");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit, port, RULE_TYPE_PACKET_IP_SUBNET_BASED_VLAN_PER_PORT, &rule_pattern, &dev_action);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}
#endif /* SYS_CPNT_IP_SUBNET_VLAN_BIND_PORT */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetMvrRecVlanEntry
 *------------------------------------------------------------------------------
 * PURPOSE : This function sets MVR Receiver VLAN Entry
 * INPUT   : src_vid            -- Soruce MVR VLAN
 *           mip                -- multicast IP address
 *           dst_vid            -- Destination MVR VLAN
 * OUTPUT  : none
 * RETURN  : TRUE               -- Success
 *           FALSE              -- Failed
 * NOTE    : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetMvrRecVlanEntry(UI16_T src_vid, UI32_T mip, UI16_T dst_vid)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T  flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    memset(&flags, 0, sizeof(flags));

    flags.INSTALL = 1;
    flags.FORCE_INSTALL = 1;

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyVlan(&rule_pattern, src_vid, 0xffff))
    {
        LOG("<Error> Qualify VLAN");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_RulePatternQualifyIpDestAddress(&rule_pattern, mip, 0xffffffff))
    {
        LOG("<Error> Qualify IP dest address");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_ActionModifyVlan(&dev_action, DEVRM_MODIFY_VLAN_CMD_FOR_ALL, dst_vid, FALSE))
    {
        LOG("<Error> Modify VLAN");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, RULE_TYPE_PACKET_MVR_REC_VLAN,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DeleteMvrRecVlanEntry
 *------------------------------------------------------------------------------
 * PURPOSE : This function deletes MVR Receiver VLAN Entry
 * INPUT   : src_vid            -- Soruce MVR VLAN
 *           mip                -- multicast IP address
 * OUTPUT  : none
 * RETURN  : TRUE               -- Success
 *           FALSE              -- Failed
 * NOTE    : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DeleteMvrRecVlanEntry(UI16_T src_vid, UI32_T mip)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T  flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    memset(&flags, 0, sizeof(flags));

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyVlan(&rule_pattern, src_vid, 0xffff))
    {
        LOG("<Error> Qualify VLAN");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_RulePatternQualifyIpDestAddress(&rule_pattern, mip, 0xffffffff))
    {
        LOG("<Error> Qualify VLAN");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, RULE_TYPE_PACKET_MVR_REC_VLAN,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetIgmpEntry
 *------------------------------------------------------------------------------
 * PURPOSE : This function sets IGMP Entry
 * INPUT   : is_add             -- TRUE, set the IGMP rule; FALSE, remove the
 *                                 rule.
 * OUTPUT  : none
 * RETURN  : TRUE               -- Success
 *           FALSE              -- Failed
 * NOTE    : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetIgmpEntry(BOOL_T is_add)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T  flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    UI32_T ip_addr;
    UI32_T mask;

    memset(&flags, 0, sizeof(flags));
    if (TRUE == is_add)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    L_INET_Aton((UI8_T *)"224.0.0.0", &ip_addr);
    L_INET_Aton((UI8_T *)"255.255.255.0", &mask);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIpDestAddress(&rule_pattern, ip_addr, mask))
    {
        LOG("<Error> Qualify IP");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_RulePatternQualifyIpProtocol(&rule_pattern, 2, 0xff))
    {
        LOG("<Error> Qualify IGMP");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, RULE_TYPE_PACKET_IGMP,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetMulticastEntry
 *------------------------------------------------------------------------------
 * PURPOSE : This function sets internet multicast Entry
 * INPUT   : unit, port         -- unit/port number
 *           src_vid            -- set the multicast rule on the specified
 *                                 VLAN id
 *           is_add             -- TRUE, set the multicast rule; FALSE, remove
 *                                 the rule
 * OUTPUT  : none
 * RETURN  : TRUE               -- Success
 *           FALSE              -- Failed
 * NOTE    : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetMulticastEntry(UI32_T unit, UI32_T port, UI32_T src_vid, BOOL_T is_add)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T   flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    UI8_T multicast_mac_addr[] = {0x01, 0x00, 0x5E, 0x00, 0x00, 0x00};
    UI8_T multicast_mac_mask[] = {0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00};

    memset(&flags, 0, sizeof(flags));
    if (TRUE == is_add)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyVlan(&rule_pattern, src_vid, 0xffff))
    {
        LOG("<Error> Qualify VLAN");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(&rule_pattern, multicast_mac_addr, multicast_mac_mask))
    {
        LOG("<Error> Qualify DA");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_HARD_DROP))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit, port, RULE_TYPE_PACKET_IP_MULTICAST_DATA, &rule_pattern, &dev_action);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

#if (SYS_CPNT_BPDU_LOOPBACK_RATELIMIT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_GetBpduRateLimitEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get rate limit on specified (unit, port/trunk)
 * INPUT    : unit  - unit number
 *            port  - port number
 *            trunk - trunk id (0 means not trunk)
 * OUTPUT   : rate_limit - TRUE: limit rate
 *                        FALSE: not limit rate
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_GetBpduRateLimitEntry(UI32_T unit, UI32_T port, UI32_T trunk, BOOL_T *rate_limit)
{
    if ( (unit == 0) || 
	   (unit > SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK) || 
	   (port > SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) || 
	   (trunk > SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM))
    {
        return FALSE;
    }

    if ((trunk == 0) && (port == 0))
    {
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();
    if (trunk == 0)
    {
        *rate_limit = shmem_data_p->bpdu_loopback_status_per_port[unit-1][port-1];
    }
    else
    {
        *rate_limit = shmem_data_p->bpdu_loopback_status_per_trunk[unit-1][trunk-1];
    }
    RULE_CTRL_LeaveCriticalSection();

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetBpduRateLimitEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : set up rate limit on specified (unit, port/trunk)
 * INPUT    : unit  - unit number
 *            port  - port number
 *            trunk - trunk id (0 means not trunk)
 *            rate_limit - TRUE: limit rate
 *                        FALSE: not limit rate
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetBpduRateLimitEntry(UI32_T unit, UI32_T port, UI32_T trunk, BOOL_T rate_limit)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_CTRL_OPT_FLAGS_T   flags;
    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    LOG("Enter RULE_CTRL_SetBpduRateLimitEntry, unit: %lu, port: %lu, trunk: %lu, rate_limit: %s",
		unit, port, trunk, (rate_limit) ? "Enable":"Disable");

    if ( (unit == 0) || 
	   (unit > SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK) || 
	   (port > SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) || 
	   (trunk > SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM))
    {
        return FALSE;
    }

    if ((trunk == 0) && (port == 0))
    {
        return FALSE;
    }

    memset(&flags, 0, sizeof(flags));
    if (TRUE == rate_limit)
    {
        /* add rule for rate-limit */
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    if (TRUE != RULE_CTRL_LocalConfigBpduPacket(rate_limit, &rule_pattern, &dev_action))
    {
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();	

    if (trunk == 0)
    {
        result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                    unit, port, RULE_TYPE_PACKET_BPDU_RATE_LIMIT_PER_PORT, &rule_pattern, &dev_action);
        shmem_data_p->bpdu_loopback_status_per_port[unit-1][port-1] = rate_limit;
    }
    else
    {
        result = RULE_CTRL_LocalSetRuleForPacketTypeByTrunk(flags,
                    trunk, RULE_TYPE_PACKET_BPDU_RATE_LIMIT_PER_TRUNK, &rule_pattern, &dev_action);
        shmem_data_p->bpdu_loopback_status_per_trunk[unit-1][trunk-1] = rate_limit;
    }

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetDenyAnyEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : set up deny any rule on specified (unit, port/trunk)
 * INPUT    : unit  - unit number
 *            port  - port number
 *            trunk - trunk id (0 means not trunk)
 *            add   - TRUE: add deny any rule
 *                    FALSE: delete deny any rule
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetDenyAnyEntry(UI32_T unit, UI32_T port, UI32_T trunk, BOOL_T is_add)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_CTRL_OPT_FLAGS_T   flags;
    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    LOG("Enter RULE_CTRL_SetDenyAnyEntry, unit: %lu, port: %lu, trunk: %lu, rate_limit: %s",
		unit, port, trunk, (is_add) ? "Enable":"Disable");

    if ( (unit == 0) || 
	   (unit > SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK) || 
	   (port > SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) || 
	   (trunk > SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM))
    {
        return FALSE;
    }

    if ((trunk == 0) && (port == 0))
    {
        return FALSE;
    }

    memset(&flags, 0, sizeof(flags));
    if (TRUE == is_add)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_HARD_DROP))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }
    
    RULE_CTRL_EnterCriticalSection();	

    if (trunk == 0)
    {
        result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                    unit, port, RULE_TYPE_PACKET_LOOP_DENY_ANY_PER_PORT, &rule_pattern, &dev_action);
    }
    else
    {
        result = RULE_CTRL_LocalSetRuleForPacketTypeByTrunk(flags,
                    trunk, RULE_TYPE_PACKET_LOOP_DENY_ANY_PER_TRUNK, &rule_pattern, &dev_action);
    }

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetGlobalBpduRateLimit
 *------------------------------------------------------------------------------
 * PURPOSE  : set up global rate limit
 * INPUT    : rate_limit - TRUE: limit rate
 *                                 FALSE: not limit rate
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetGlobalBpdu()
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_CTRL_OPT_FLAGS_T   flags;
    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    /*force reinstall new rule*/
    flags.INSTALL = 1;
    flags.FORCE_INSTALL = 1;

    if (TRUE != RULE_CTRL_LocalConfigBpduPacket(FALSE, &rule_pattern, &dev_action))
    {
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();	

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags,
                    RULE_TYPE_PACKET_GLOBAL_BPDU, &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

#endif /*#if (SYS_CPNT_BPDU_LOOPBACK_RATELIMIT == TRUE)*/

#if ((SYS_CPNT_INGRESS_RATE_LIMIT_VIA_FILTER_ENGINE == TRUE) && (SYS_CPNT_QOS_V2 == TRUE))
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalSetRateLimit
 *------------------------------------------------------------------------------
 * PURPOSE  : set up rate limit on specified (unit, port)
 * INPUT    : unit, port, k_bps, trunk_id (0 means the port didn't join any trunk)
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : XGSIII: the Src_Port_TGID of fp_tcam rule
 *                    is used for port or trunk id.
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_LocalSetRateLimit(
    BOOL_T is_enable,
    UI32_T unit,
    UI32_T port,
    UI32_T trunk_id,
    UI32_T k_bps)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    memset(&flags, 0, sizeof(flags));
    if (TRUE == is_enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_ActionMeterConfigFlow(&dev_action, k_bps, k_bps * 1024 / 8))
    {
        LOG("<Error> config meter (flow)");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_ActionRedPacketTo(&dev_action, DEVRM_PKT_CMD_HARD_DROP))
    {
        LOG("<Error> Set red packet command");
        return FALSE;
    }

    if (trunk_id == 0) 
    {
        result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags, unit, port,
                     RULE_TYPE_PACKET_RATE_LIMIT, &rule_pattern, &dev_action);
    }
    else
    {
        result = RULE_CTRL_LocalSetRuleForPacketTypeByTrunk(flags, trunk_id,
                     RULE_TYPE_PACKET_TRUNK_RATE_LIMIT, &rule_pattern, &dev_action);
    }

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetRateLimit
 *------------------------------------------------------------------------------
 * PURPOSE  : set up rate limit on specified (unit, port)
 * INPUT    : unit, port, k_bps, trunk_id (0 means the port didn't join any trunk)
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : XGSIII: the Src_Port_TGID of fp_tcam rule
 *                    is used for port or trunk id.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetRateLimit(UI32_T unit, UI32_T port, UI32_T trunk_id, UI32_T k_bps)
{
    UI32_T  my_drv_unit;

    if (FALSE == STKTPLG_POM_GetMyDriverUnit(&my_drv_unit))
        return FALSE;

    LOG("unit=%lu, port=%lu, rate=%lu kbps", unit, port, k_bps);

    if (my_drv_unit != unit) /* set up local unit only */
    {
        PRINTF("error, suppose caller should pass local unit to rule_ctrl");
        return FALSE;
    }

    return RULE_CTRL_LocalSetRateLimit(TRUE, unit, port, trunk_id, k_bps);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DeleteRateLimit
 *------------------------------------------------------------------------------
 * PURPOSE  : cancel rate limit on specified (unit, port)
 * INPUT    : unit, port
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DeleteRateLimit(UI32_T unit, UI32_T port, UI32_T trunk_id)
{
    UI32_T  my_drv_unit;

    if (FALSE == STKTPLG_POM_GetMyDriverUnit(&my_drv_unit))
        return FALSE;

    if (my_drv_unit != unit) /* set up local unit only */
    {
        PRINTF("error, suppose caller should pass local unit to rule_ctrl");
        return FALSE;
    }

    LOG("unit=%lu, port=%lu", unit, port);

    return RULE_CTRL_LocalSetRateLimit(FALSE, unit, port, trunk_id, 0);
}
#endif /* #if ((SYS_CPNT_INGRESS_RATE_LIMIT_VIA_FILTER_ENGINE == TRUE) && (SYS_CPNT_QOS_V2 == TRUE)) */

// DOTO: Remove this function
/* FUNCTION NAME - RULE_CTRL_Service
 *------------------------------------------------------------------------------
 * PURPOSE : This functoin is used to serve the rule
 *   INPUT : enable, type, param_p
 *  OUTPUT : None
 *  RETURN : RULE_TYPE_OK/ RULE_TYPE_FAIL
 *    NOTE : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_Service(BOOL_T enable, RULE_CTRL_ServiceType_T type, void *param_p)
{
    if (type >= RULE_CTRL_MAX_SERVICE_ID)
        return FALSE;

    return service_funcs[type](enable, param_p);
}

static BOOL_T
RULE_CTRL_WebAuthRedirHttp(
    BOOL_T enable,
    void *param_p)
{
#if (SYS_CPNT_WEBAUTH == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_UnitPort_T *unit_port_p = param_p;

    RULE_CTRL_OPT_FLAGS_T   flags;

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit_port_p->unit, unit_port_p->port, RULE_TYPE_PACKET_WA_REDIR_HTTP_CLIENT,
                NULL, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
#else
    return FALSE; /* Not support yet */
#endif
}

static BOOL_T
RULE_CTRL_WebAuthPermitDhcpClient(
    BOOL_T enable,
    void *param_p)
{
#if (SYS_CPNT_WEBAUTH == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_UnitPort_T *unit_port_p = param_p;

    RULE_CTRL_OPT_FLAGS_T   flags;

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit_port_p->unit, unit_port_p->port, RULE_TYPE_PACKET_WA_PERMIT_DHCP_CLIENT,
                NULL, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
#else
    return FALSE; /* Not support yet */
#endif
}

static BOOL_T
RULE_CTRL_WebAuthPermitDns(
    BOOL_T enable,
    void *param_p)
{
#if (SYS_CPNT_WEBAUTH == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_UnitPort_T *unit_port_p = param_p;

    RULE_CTRL_OPT_FLAGS_T   flags;

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit_port_p->unit, unit_port_p->port, RULE_TYPE_PACKET_WA_PERMIT_DNS_CLIENT,
                NULL, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
#else
    return FALSE; /* Not support yet */
#endif
}

static BOOL_T
RULE_CTRL_WebAuthPermitSip(
    BOOL_T enable,
    void *param_p)
{
#if (SYS_CPNT_WEBAUTH == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_UnitPortSip_T *unit_port_sip_p = param_p;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIpSourceAddress(&rule_pattern, unit_port_sip_p->sip, 0xffffffff))
    {
        LOG("<Error> Qualify IP source address");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit_port_sip_p->unit, unit_port_sip_p->port, RULE_TYPE_PACKET_SIP,
                &rule_pattern, &dev_action);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
#else
    return FALSE; /* Not support yet */
#endif
}

static BOOL_T
RULE_CTRL_WebAuthDenyIp(
    BOOL_T enable,
    void *param_p)
{
#if (SYS_CPNT_WEBAUTH == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_UnitPort_T *unit_port_p = param_p;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    RULE_CTRL_OPT_FLAGS_T   flags;

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIp(&rule_pattern))
    {
        LOG("<Error> Qualify IP");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_HARD_DROP))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit_port_p->unit, unit_port_p->port, RULE_TYPE_PACKET_WA_DENY_IP_ANY,
                &rule_pattern, &dev_action);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
#else
    return FALSE; /* Not support yet */
#endif
}

static BOOL_T
RULE_CTRL_IpSrcGuardPermitVidSip(
    BOOL_T enable,
    void *param_p)
{
#if (SYS_CPNT_IP_SOURCE_GUARD == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_UnitPortVidSip_T *vid_sip_p = param_p;

    UI32_T packet_type;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    packet_type = RULE_TYPE_PACKET_SA_SIP_VID;

#if (SYS_CPNT_IPSG_STRICT_MODE == TRUE)
    packet_type = RULE_TYPE_PACKET_IP_SG_PERMIT_HOST_STRICT;
#endif /* SYS_CPNT_IPSG_STRICT_MODE */

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIp(&rule_pattern))
    {
        LOG("<Error> Qualify IP");
        return FALSE;
    }

    if (0 != vid_sip_p->vid)
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifyVlan(&rule_pattern, vid_sip_p->vid, 0xffff))
        {
            LOG("<Error> Qualify VLAN");
            return FALSE;
        }
    }

    if (0 != vid_sip_p->sip)
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifyIpSourceAddress(&rule_pattern, vid_sip_p->sip, 0xffffffff))
        {
            LOG("<Error> Qualify IP source address");
            return FALSE;
        }
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                vid_sip_p->unit, vid_sip_p->port, packet_type,
                &rule_pattern, &dev_action);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
#else
    return FALSE;
#endif /* SYS_CPNT_IP_SOURCE_GUARD */
}

static BOOL_T
RULE_CTRL_IpSrcGuardPermitSaVidSip(
    BOOL_T enable,
    void *param_p)
{
#if (SYS_CPNT_IP_SOURCE_GUARD == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_UnitPortSaVidSip_T *sa_vid_sip_p = param_p;

    UI32_T packet_type;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    packet_type = RULE_TYPE_PACKET_SA_SIP_VID;

#if (SYS_CPNT_IPSG_STRICT_MODE == TRUE)
    packet_type = RULE_TYPE_PACKET_IP_SG_PERMIT_HOST_STRICT;
#endif /* SYS_CPNT_IPSG_STRICT_MODE */

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIp(&rule_pattern))
    {
        LOG("<Error> Qualify IP");
        return FALSE;
    }

    if (0 != sa_vid_sip_p->vid)
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifyVlan(&rule_pattern, sa_vid_sip_p->vid, 0xffff))
        {
            LOG("<Error> Qualify VLAN");
            return FALSE;
        }
    }

    if (0 != sa_vid_sip_p->sip)
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifyIpSourceAddress(&rule_pattern, sa_vid_sip_p->sip, 0xffffffff))
        {
            LOG("<Error> Qualify IP source address");
            return FALSE;
        }
    }

    if (FALSE == RULE_CTRL_LocalIsNullMac(sa_vid_sip_p->sa))
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifySourceMacAddress(&rule_pattern, sa_vid_sip_p->sa, mask))
        {
            LOG("<Error> Qualify SA");
            return FALSE;
        }
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                sa_vid_sip_p->unit, sa_vid_sip_p->port, packet_type,
                &rule_pattern, &dev_action);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
#else
    return FALSE;
#endif /* SYS_CPNT_IP_SOURCE_GUARD */
}

static BOOL_T
RULE_CTRL_IpSrcGuardDenyIp(
    BOOL_T enable,
    void *param_p)
{
#if (SYS_CPNT_IP_SOURCE_GUARD == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_UnitPort_T *unit_port_p = param_p;

    UI32_T packet_type;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    packet_type = RULE_TYPE_PACKET_IPSG_DENY_IP_ANY;

#if (SYS_CPNT_IPSG_STRICT_MODE == TRUE)
    packet_type = RULE_TYPE_PACKET_IP_SG_DENY_IP_ANY_STRICT;
#endif /* SYS_CPNT_IPSG_STRICT_MODE */

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIp(&rule_pattern))
    {
        LOG("<Error> Qualify IP");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action,
                                         (RULE_TYPE_PACKET_IP_SG_DENY_IP_ANY_STRICT == packet_type) ?
                                         DEVRM_PKT_CMD_SOFT_DROP : DEVRM_PKT_CMD_HARD_DROP))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit_port_p->unit, unit_port_p->port, packet_type,
                &rule_pattern, &dev_action);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
#else
    return FALSE; /* Not support yet */
#endif
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IPV6SG_PermitIp6Host
 *------------------------------------------------------------------------------
 * PURPOSE:  Permit IPv6 packet for IPv6SG
 * INPUT:    enable          - TRUE:  permit
 *                             FALSE: no permit
 *           param_p         - pointer to a RULE_CTRL_UnitPortVidSip6_T
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_IPV6SG_PermitIp6Host(
    BOOL_T enable,
    void *param_p)
{
#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_UnitPortVidSip6_T *vid_sip6_p = param_p;

    UI32_T packet_type;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    UI8_T mask[] =
        {
            0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff
        };

    packet_type = RULE_TYPE_PACKET_SIP6_VID;

#if (SYS_CPNT_IPSG_STRICT_MODE == TRUE)
    packet_type = RULE_TYPE_PACKET_IP6_SG_PERMIT_HOST_STRICT;
#endif /* SYS_CPNT_IPSG_STRICT_MODE */

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyVlan(&rule_pattern, vid_sip6_p->vid, 0xffff))
    {
        LOG("<Error> Qualify VLAN");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_RulePatternQualifyIpv6SourceAddress(&rule_pattern, vid_sip6_p->sip6, mask))
    {
        LOG("<Error> Qualify IPv6 source address");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                vid_sip6_p->unit, vid_sip6_p->port, packet_type,
                &rule_pattern, &dev_action);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
#else
    return FALSE;
#endif /* #if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IPV6SG_DenyAnyIp6Packet
 *------------------------------------------------------------------------------
 * PURPOSE:  Deny any IPv6 address for IPv6 SG
 * INPUT:    enable          - TRUE:  deny
 *                             FALSE: no deny
 *           param_p         - pointer to a RULE_CTRL_UnitPort_T
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_IPV6SG_DenyAnyIp6Packet(
    BOOL_T enable,
    void *param_p)
{
#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_UnitPort_T    *unit_port_p = param_p;

    UI32_T packet_type;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    packet_type = RULE_TYPE_PACKET_IP6_ANY;

#if (SYS_CPNT_IPSG_STRICT_MODE == TRUE)
    packet_type = RULE_TYPE_PACKET_IP6_SG_DENY_IP6_ANY_STRICT;
#endif /* SYS_CPNT_IPSG_STRICT_MODE */

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIpv6(&rule_pattern))
    {
        LOG("<Error> Qualify IPv6");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action,
                                         (RULE_TYPE_PACKET_IP6_SG_DENY_IP6_ANY_STRICT == packet_type) ?
                                         DEVRM_PKT_CMD_SOFT_DROP : DEVRM_PKT_CMD_HARD_DROP))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit_port_p->unit, unit_port_p->port, packet_type,
                &rule_pattern, &dev_action);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
#else
    return FALSE;
#endif /* #if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE) */
}

#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IPV6SG_PermitIp6LinkLocal
 *------------------------------------------------------------------------------
 * PURPOSE:  Permit all IPv6 link local packets
 * INPUT:    is_enable - TRUE:  permit
 *                       FALSE: no action
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK / RULE_TYPE_FAIL
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IPV6SG_PermitIp6LinkLocal(
    BOOL_T is_enable)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_CTRL_OPT_FLAGS_T flags;
    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;
    UI32_T packet_type;

    /* Link local address SIP = fe80::/64
     */
    UI8_T addr[SYS_ADPT_IPV6_ADDR_LEN] =
        {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
    UI8_T mask[SYS_ADPT_IPV6_ADDR_LEN] =
        {
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

    memset(&flags, 0, sizeof(flags));
    if (TRUE == is_enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    packet_type = RULE_TYPE_PACKET_IP6SG_PERMIT_LINKLOCAL;

#if (SYS_CPNT_IPSG_STRICT_MODE == TRUE)
    packet_type = RULE_TYPE_PACKET_IP6SG_PERMIT_LINKLOCAL_STRICT;
#endif /* SYS_CPNT_IPSG_STRICT_MODE */

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIpv6SourceAddress(&rule_pattern, addr, mask))
    {
        LOG("<Error> Qualify IPv6 source address");
        return RULE_TYPE_FAIL;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
    {
        LOG("<Error> Set packet command");
        return RULE_TYPE_FAIL;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, packet_type,
                  &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return result;
}
#endif /* #if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE) */

#if (SYS_CPNT_EFM_OAM_REMOTE_LB_ACTIVELY == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_OAM_CO_TrapOamLoopbackToCPU
 *------------------------------------------------------------------------------
 * FUNCTION : For CO side, trap EFM OAM loopback packet to CPU.
 * INPUT   : enable  -- enable/disable
 *           unit    -- which unit.
 *           port    -- which port.
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_OAM_CO_TrapOamLoopbackToCPU(
    BOOL_T  enable,
    UI32_T  unit,
    UI32_T  port)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    UI8_T mac_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    /* packet format:   ETH: 0x9000
     *                  DA : 0xffffff-ffffff
     */
    if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(&rule_pattern, mac_addr, mask))
    {
        LOG("<Error> Qualify DA");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_RulePatternQualifyEtherType(&rule_pattern, 0x9000, 0xffff))
    {
        LOG("<Error> Qualify ether type");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit, port, RULE_TYPE_PACKET_OAM_LBK, &rule_pattern, &dev_action);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}
#endif /* SYS_CPNT_EFM_OAM_REMOTE_LB_ACTIVELY */

#if (SYS_CPNT_APP_FILTER_CDP == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_AF_DenyCdpPacket
 *------------------------------------------------------------------------------
 * PURPOSE : This function enables/disables drop CDP packeage
 * INPUT   : enable - TRUE:  Enable
 *                    FALSE: Disable
 *           unit   - unit
 *           port   - port
 * OUTPUT  : None
 * RETURN  : RULE_TYPE_RETURN_TYPE_T
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_AF_DenyCdpPacket(
    BOOL_T enable,
    UI32_T unit,
    UI32_T port)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T                  packet_type = RULE_TYPE_PACKET_AF_DENY_CDP;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    UI8_T cdp_mac_addr[] = {0x01, 0x00, 0x0C, 0xCC, 0xCC, 0xCC};
    UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    LOG("unit=%lu port=%lu %s", unit, port, (enable) ? "Enable":"Disable");

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);
    if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(&rule_pattern, cdp_mac_addr, mask))
    {
        LOG("<Error> Qualify DA");
        return RULE_TYPE_FAIL;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_HARD_DROP))
    {
        LOG("<Error> Set packet command");
        return RULE_TYPE_FAIL;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit, port, packet_type, &rule_pattern, &dev_action);

    RULE_CTRL_LeaveCriticalSection();

    return result;
}
#endif /* SYS_CPNT_APP_FILTER_CDP */

#if (SYS_CPNT_APP_FILTER_PVST == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_AF_DenyPvstPacket
 *------------------------------------------------------------------------------
 * PURPOSE : This function enables/disables drop CDP packeage
 * INPUT   : enable - TRUE:  Enable
 *                    FALSE: Disable
 *           unit   - unit
 *           port   - port
 * OUTPUT  : None
 * RETURN  : RULE_TYPE_RETURN_TYPE_T
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_AF_DenyPvstPacket(
    BOOL_T enable,
    UI32_T unit,
    UI32_T port)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T                  packet_type = RULE_TYPE_PACKET_AF_DENY_PVST;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    UI8_T pvst_mac_addr[] = {0x01, 0x00, 0x0C, 0xCC, 0xCC, 0xCD};
    UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    LOG("unit=%lu port=%lu %s", unit, port, (enable) ? "Enable":"Disable");

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);
    if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(&rule_pattern, pvst_mac_addr, mask))
    {
        LOG("<Error> Qualify DA");
        return RULE_TYPE_FAIL;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_HARD_DROP))
    {
        LOG("<Error> Set packet command");
        return RULE_TYPE_FAIL;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                 unit, port, packet_type, &rule_pattern, &dev_action);

    RULE_CTRL_LeaveCriticalSection();

    return result;
}
#endif /* SYS_CPNT_PACKET_FILTER_PVST */

#if (SYS_CPNT_QINQ == TRUE)
#if (SYS_CPNT_SWCTRL_QINQ_SERVICE == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SERVICE_QINQ
 *------------------------------------------------------------------------------
 * PURPOSE:  for SERVICE QINQ
 *           config chip to add/delete an outter tag to packets with user setting.
 * INPUT:    unit, port - unit/port number
 *           cvid       - inner vid
 *           cpri       - inner pri
 *           svid       - outer vid
 *           spri       - outer pri
 *           is_enable  - add or delete the rule of S_QINQ
 * OUTPUT:   NONE
 * RETURN:   TRUE/FALSE
 * NOTE:     TRUE means success; FALSE means faile
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_Service_QinQ(
    UI32_T unit,
    UI32_T port,
    UI32_T cvid,
    UI32_T cpri,
    UI32_T svid,
    UI32_T spri,
    BOOL_T is_enable)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T   flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    memset(&flags, 0, sizeof(flags));
    if (TRUE == is_enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (cvid != 0xFFFF)
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifyVlan(&rule_pattern, cvid, MAX_diffServMacAceVidBitmask))
        {
            LOG("<Error> Qualify VLAN");
            return FALSE;
        }
    }

    if (cpri != 0xFFFF)
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifyCos(&rule_pattern, cpri, MAX_diffServMacAceCosBitmask))
        {
            LOG("<Error> Qualify CoS");
            return FALSE;
        }
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_ActionModifyVlan(&dev_action, DEVRM_MODIFY_VLAN_CMD_FOR_ALL, svid, TRUE))
    {
        LOG("<Error> Modify VLAN");
        return FALSE;
    }

    if (spri != 0xFFFF)
    {
        if (TRUE != DEVRM_LIB_ActionModifyPriority(&dev_action, spri))
        {
            LOG("<Error> Modify priority");
            return FALSE;
        }
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit, port, RULE_TYPE_PACKET_UP_VID, &rule_pattern, &dev_action);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}
#endif /*end of #if (SYS_CPNT_SWCTRL_QINQ_SERVICE == TRUE)*/

#if (SYS_CPNT_QINQ_L2PT == TRUE)
#if (SYS_CPNT_QINQ_L2PT_DA_REMARK == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetL2PTTunnelPduToCpu
 *------------------------------------------------------------------------------
 * PURPOSE : This function enables/disables trap L2PT tunnel PDU.
 * INPUT   : tunnel_da
 *           is_enable
 * OUTPUT  : None
 * RETURN  : TRUE   -- Success
 *           FALSE  -- If a given port is not available
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetL2PTTunnelPduToCpu(UI8_T *tunnel_da, BOOL_T is_enable)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    memset(&flags, 0, sizeof(flags));
    if (TRUE == is_enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(&rule_pattern, tunnel_da, mask))
    {
        LOG("<Error> Qualify DA");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    /* it has problem to install default rule on trunk now,
     *  so change to use global rule temporarily.
     */
    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, RULE_TYPE_PACKET_L2PT,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}
#endif /* (SYS_CPNT_QINQ_L2PT_DA_REMARK == TRUE) */

#if (SYS_CPNT_QINQ_L2PT_CUSTOM_PDU == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetCustomPduTrapStatusForL2PT
 *------------------------------------------------------------------------------
 * PURPOSE : This function enables/disables trap custom PDU for L2PT.
 * INPUT   : unit
 *           port
 *           id
 *           dst_mac
 *           ethertype
 *           pdu_len
 *           pdu_data
 *           pdu_mask
 *           is_enable
 * OUTPUT  : None
 * RETURN  : TRUE   -- Success
 *           FALSE  -- Failed
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetCustomPduTrapStatusForL2PT(UI32_T unit, UI32_T port, UI32_T id, UI8_T *dst_mac, UI16_T ethertype, UI32_T pdu_len, UI8_T *pdu_data, UI8_T *pdu_mask, BOOL_T is_enable)
{
#if (SYS_ADPT_QINQ_L2PT_MAX_NBR_OF_CUSTOM_PDU_PER_PORT != 1)
#warning Not implemented for SYS_ADPT_QINQ_L2PT_MAX_NBR_OF_CUSTOM_PDU_PER_PORT > 1
#endif

    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T   flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    memset(&flags, 0, sizeof(flags));
    if (TRUE == is_enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    /* NOTE
     *   1. current implementation support one entry per port only.
     *   2. filter PDU by dst mac and ethertype only.
     *
     * TODO:
     *   1. support more than one entry per port.
     *   2. filter PDU by classify frame format as follows:
     *         Ethernet II
     *         IEEE 802.3 with LLC 42-42-03
     *         IEEE 802.3 with SNAP
     */

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(&rule_pattern, dst_mac, mask))
    {
        LOG("<Error> Qualify DA");
        return FALSE;
    }

    if (ethertype != 0)
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifyEtherType(&rule_pattern, ethertype, 0xffff))
        {
            LOG("<Error> Qualify ether type");
            return FALSE;
        }
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit, port, RULE_TYPE_PACKET_L2PT_CUSTOM_PDU, &rule_pattern, &dev_action);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}
#endif /* (SYS_CPNT_QINQ_L2PT_CUSTOM_PDU == TRUE) */
#endif /* (SYS_CPNT_QINQ_L2PT == TRUE) */
#endif /*end of #if (SYS_CPNT_QINQ == TRUE)*/

#if (SYS_CPNT_PPPOE_IA == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetPPPoEDPktToCpu
 *------------------------------------------------------------------------------
 * PURPOSE : To enable/disable trap PPPoE discover packet to CPU
 *            for specified unit/port.
 * INPUT   : unit       - unit
 *           port       - port
 *           is_enable  - TRUE to enable
 * OUTPUT  : none
 * RETURN  : TRUE       - Success
 *           FALSE      - Failed
 * NOTE    : 1. there is a problem to install default rule on trunk now,
 *              so change to use global rule temporarily.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetPPPoEDPktToCpu(
    UI32_T  unit,
    UI32_T  port,
    BOOL_T  is_enable)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T   flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    memset(&flags, 0, sizeof(flags));
    if (TRUE == is_enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    /* ether_type == 0x8863
     */
    if (TRUE != DEVRM_LIB_RulePatternQualifyEtherType(&rule_pattern, 0x8863, 0xffff))
    {
        LOG("<Error> Qualify ether type");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit, port, RULE_TYPE_PACKET_PPPOED_PER_PORT, &rule_pattern, &dev_action);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

// ???: Why no compiler option for this and above function ?
//      Packet type been changed to an unique key to map interface for rule (per sys or per port)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetPPPoEDPktToCpuPerSystem
 *------------------------------------------------------------------------------
 * PURPOSE : To enable/disable trap PPPoE discover packet to CPU
 *            for entire system.
 * INPUT   : is_enable  - TRUE to enable
 * OUTPUT  : none
 * RETURN  : TRUE       - Success
 *           FALSE      - Failed
 * NOTE    : 1. there is a problem to install default rule on trunk now,
 *              so change to use global rule temporarily.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetPPPoEDPktToCpuPerSystem(
    BOOL_T  is_enable)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    memset(&flags, 0, sizeof(flags));
    if (TRUE == is_enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    /* ether_type == 0x8863
     */
    if (TRUE != DEVRM_LIB_RulePatternQualifyEtherType(&rule_pattern, 0x8863, 0xffff))
    {
        LOG("<Error> Qualify ether type");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, RULE_TYPE_PACKET_PPPOED,
                &rule_pattern, /*&action,*/ &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}
#endif /* #if (SYS_CPNT_PPPOE_IA == TRUE) */

#if (SYS_CPNT_ERPS == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetRapsPktToCpuPerSystem
 *------------------------------------------------------------------------------
 * PURPOSE : To enable/disable trap R-APS packet to CPU for entire system.
 * INPUT   : eth_type   - ether type to filter
 *           dst_mac_p  - da to filter
 *           is_enable  - TRUE to enable
 * OUTPUT  : none
 * RETURN  : TRUE       - Success
 *           FALSE      - Failed
 * NOTE    : none
 *------------------------------------------------------------------------------
 */
static BOOL_T RULE_CTRL_LocalSetRapsPktToCpuPerSystem(
    UI16_T  eth_type,
    UI8_T   *dst_mac_p,
    BOOL_T  is_enable)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T   flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0x00};

    memset(&flags, 0, sizeof(flags));
    if (TRUE == is_enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    /*  DA: 0x0119A7-000001
     * ETH: 0x8902
     *
     *  modify for v2, the last byte may be ring-id, ignored by rule
     */
    if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(&rule_pattern, dst_mac_p, mask))
    {
        LOG("<Error> Qualify DA");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_RulePatternQualifyEtherType(&rule_pattern, eth_type, 0xffff))
    {
        LOG("<Error> Qualify ether type");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, RULE_TYPE_PACKET_RAPS,
                &rule_pattern, &dev_action, NULL);

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

// ???: WHAT !!
/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_SetRapsPduToCpuByPort
 *------------------------------------------------------------------------------
 * PURPOSE : This function will enable R-APS PDU redirect to CPU rule
 *            for specified unit and port
 * INPUT   : unit      -- which unit.
 *           port      -- which port.
 *           eth_type  -- which EtherType to set
 *           dmac_p    -- which destination MAC to set
 *           is_enable -- enable/disable
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetRapsPduToCpuByPort(
    UI32_T  unit,
    UI32_T  port,
    UI16_T  eth_type,
    UI8_T   *dst_mac_p,
    BOOL_T  is_enable)
{
    BOOL_T  ret = TRUE;

    LOG("raps_pcnt/port enable: %lu/%lu",
        shmem_data_p->raps_pcnt,
        RULE_CTRL_IS_BIT_ON(shmem_data_p->raps_pbmp[unit-1], port-1));

    RULE_CTRL_EnterCriticalSection();
    if (TRUE == is_enable)
    {
        if (RULE_CTRL_IS_BIT_OFF(shmem_data_p->raps_pbmp[unit-1], port-1))
        {
            if (shmem_data_p->raps_pcnt == 0)
            {
                ret = RULE_CTRL_LocalSetRapsPktToCpuPerSystem(
                        eth_type, dst_mac_p, is_enable);
            }
            if (ret)
            {
                RULE_CTRL_SET_BIT_ON(shmem_data_p->raps_pbmp[unit-1], port-1);
                shmem_data_p->raps_pcnt++;
            }
        }
    }
    else
    {
        if (RULE_CTRL_IS_BIT_ON(shmem_data_p->raps_pbmp[unit-1], port-1))
        {
            if (shmem_data_p->raps_pcnt == 1)
            {
                ret = RULE_CTRL_LocalSetRapsPktToCpuPerSystem(
                        eth_type, dst_mac_p, is_enable);
            }
            if (ret)
            {
                RULE_CTRL_SET_BIT_OFF(shmem_data_p->raps_pbmp[unit-1], port-1)
                shmem_data_p->raps_pcnt--;
            }
        }
    }
    RULE_CTRL_LeaveCriticalSection();

    LOG("raps_pcnt/ret: %lu/%d", shmem_data_p->raps_pcnt, ret);

    return ret;
}
#endif /* #if (SYS_CPNT_ERPS == TRUE) */

#if (SYS_CPNT_IP_MULTICAST_DATA_DROP == TRUE) || (SYS_CPNT_MVR == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IGMPSNP_DenyMulticastData
 *------------------------------------------------------------------------------
 * PURPOSE : This function sets internet multicast Entry
 * INPUT   : enable - TRUE to add rule;
 *                    FALSE to remove rule.
 *           unit   - unit
 *           port   - port
 * OUTPUT  : None
 * RETURN  : TRUE   - Success
 *           FALSE  - Failed
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_IGMPSNP_DenyMulticastData(
    BOOL_T enable,
    UI32_T unit,
    UI32_T port)
{
#define RULE_CTRL_PROT_UDP          17

    /*
     ipv4
     udp
     dip = e0.00.00.00 / f0.00.00.00
     */

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T packet_type;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    UI8_T ipmc_addr[] = {0xe0, 0, 0, 0};
    UI8_T ipmc_addr_mask[] = {0xf0, 0, 0, 0};

    LOG("%s unit=%lu port=%lu", enable?"enable":"disable", unit, port);

    if (SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER <= port &&
         port <= (SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER + SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM - 1))
    {
        packet_type = RULE_TYPE_PACKET_TRUNK_IP_MULTICAST_DATA_DROP;
    }
    else
    {
        packet_type = RULE_TYPE_PACKET_IP_MULTICAST_DATA_DROP;
    }

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIpDestAddress(&rule_pattern,
                                                          L_STDLIB_Ntoh32(* (UI32_T *) ipmc_addr),
                                                          L_STDLIB_Ntoh32(* (UI32_T *) ipmc_addr_mask)))
    {
        LOG("<Error> Qualify IP dest address");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_RulePatternQualifyUdp(&rule_pattern))
    {
        LOG("<Error> Qualify UDP");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_HARD_DROP))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    if (packet_type == RULE_TYPE_PACKET_IP_MULTICAST_DATA_DROP)
    {
        result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit, port, packet_type, &rule_pattern, &dev_action);
    }
    else
    {
        result = RULE_CTRL_LocalSetRuleForPacketTypeByTrunkPort(flags,
                    port, packet_type, &rule_pattern, &dev_action);
    }

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;

#undef RULE_CTRL_PROT_UDP
}
#endif /* SYS_CPNT_IP_MULTICAST_DATA_DROP || SYS_CPNT_MVR */

#if (SYS_CPNT_IPV6_MULTICAST_DATA_DROP == TRUE) || (SYS_CPNT_MVR6 == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IGMPSNP_DenyMulticastData
 *------------------------------------------------------------------------------
 * PURPOSE : This function sets internet multicast Entry
 * INPUT   : enable - TRUE to add rule;
 *                    FALSE to remove rule.
 *           unit   - unit
 *           port   - port
 * OUTPUT  : None
 * RETURN  : TRUE   - Success
 *           FALSE  - Failed
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_IPV6_DenyMulticastData(
    BOOL_T enable,
    UI32_T unit,
    UI32_T port)
{
#define RULE_CTRL_PROT_UDP          17

    /*
    ipv6
    udp
    dip=ff00::00 / ff00::0 or da=33:33:00:00:00:00/ff:ff:00:00:00:00
     */

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T                  packet_type;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    UI8_T ip_addr[] =
    {
        0xff, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    UI8_T mask[] =
    {
        0xff, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    LOG("%s unit=%lu port=%lu", enable ? "enable" : "disable", unit, port);

    if (SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER <= port &&
         port <= (SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER + SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM - 1))
    {
        packet_type = RULE_TYPE_PACKET_TRUNK_IP6_MULTICAST_DATA_DROP;
    }
    else
    {
        packet_type = RULE_TYPE_PACKET_IP6_MULTICAST_DATA_DROP;
    }

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIpv6DestAddress(&rule_pattern, ip_addr, mask))
    {
        LOG("<Error> Qualify IPv6 dest address");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_RulePatternQualifyUdp(&rule_pattern))
    {
        LOG("<Error> Qualify UDP");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_HARD_DROP))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    if (packet_type == RULE_TYPE_PACKET_IP6_MULTICAST_DATA_DROP)
    {
        result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                unit, port, packet_type, &rule_pattern, &dev_action);
    }
    else
    {
        result = RULE_CTRL_LocalSetRuleForPacketTypeByTrunkPort(flags,
                    port, packet_type, &rule_pattern, &dev_action);
    }

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;

#undef RULE_CTRL_PROT_UDP
}
#endif /* SYS_CPNT_IPV6_MULTICAST_DATA_DROP || SYS_CPNT_MVR6 */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetCdpPktToCpu
 *------------------------------------------------------------------------------
 * PURPOSE : This function enables/disables trap CDP PDU.
 * INPUT   : is_enable
 * OUTPUT  : None
 * RETURN  : TRUE   -- Success
 *           FALSE  -- If a given port is not available
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetCdpPktToCpu(BOOL_T is_enable)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    UI8_T cdp_dst_mac[] = {0x01, 0x00, 0x0c, 0xcc, 0xcc, 0xcc};
    UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    memset(&flags, 0, sizeof(flags));
    if (TRUE == is_enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    /* filter DA 0x01000C-CCCCCC
     */
    if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(&rule_pattern, cdp_dst_mac, mask))
    {
        LOG("<Error> Qualify DA");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    /* it has problem to install default rule on trunk now,
     *  so change to use global rule temporarily.
     */
    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, RULE_TYPE_PACKET_CDP,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetPvstPktToCpu
 *------------------------------------------------------------------------------
 * PURPOSE : This function enables/disables trap CDP PDU.
 * INPUT   : is_enable
 * OUTPUT  : None
 * RETURN  : TRUE   -- Success
 *           FALSE  -- If a given port is not available
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetPvstPktToCpu(BOOL_T is_enable)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    UI8_T pvst_dst_mac[] = {0x01, 0x00, 0x0c, 0xcc, 0xcc, 0xcd};
    UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


    memset(&flags, 0, sizeof(flags));
    if (TRUE == is_enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    /* filter DA 0x01000C-CCCCCC
     */
    if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(&rule_pattern, pvst_dst_mac, mask))
    {
        LOG("<Error> Qualify DA");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, RULE_TYPE_PACKET_PVST,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

#if (SYS_CPNT_DOS == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetDosProtectionFilter
 *------------------------------------------------------------------------------
 * FUNCTION: This function will config DoS protection
 * INPUT   : type   - the type of DOS protection to config
 *           enable - TRUE to enable; FALSE to disable.
 * OUTPUT  : None
 * RETURN  : TRUE/FALSE
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetDosProtectionFilter(UI32_T type, BOOL_T enable)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T   flags;

    UI32_T pk_type;

    switch (type)
    {
        case SWDRV_DOS_FILTER_SMURF:
            pk_type = RULE_TYPE_PACKET_DOS_SMURF;
            break;

        case SWDRV_DOS_FILTER_LAND:
        case SWDRV_DOS_FILTER_TCP_NULL_SCAN:
        case SWDRV_DOS_FILTER_TCP_SYN_FIN_SCAN:
        case SWDRV_DOS_FILTER_TCP_XMAS_SCAN:
            /* not implemenented
             */
            return FALSE;

        default:
            return FALSE;
    }

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, pk_type,
                NULL, NULL, NULL);

    return RULE_TYPE_OK == result ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetDosProtectionRateLimit
 *------------------------------------------------------------------------------
 * FUNCTION: This function will config DoS protection
 * INPUT   : type   - the type of DOS protection to config
 *           rate   - rate in kbps. 0 to disable.
 * OUTPUT  : None
 * RETURN  : TRUE/FALSE
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetDosProtectionRateLimit(UI32_T type, UI32_T rate)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_OPT_FLAGS_T   flags;

    UI32_T pk_type, pk_type2;

    DEVRM_ACTION_T dev_action;

    BOOL_T is_enable;

    is_enable = rate != 0;
    pk_type = RULE_TYPE_PACKET_MAX_TYPE;
    pk_type2 = RULE_TYPE_PACKET_MAX_TYPE;

    switch (type)
    {
        case SWDRV_DOS_RATELIMIT_ECHO_CHARGEN:
            pk_type = RULE_TYPE_PACKET_DOS_CHARGEN;
            pk_type2 = RULE_TYPE_PACKET_DOS_ECHO;
            break;

        case SWDRV_DOS_RATELIMIT_TCP_FLOODING:
            pk_type = RULE_TYPE_PACKET_DOS_TCP_FLOODING;
            break;

        case SWDRV_DOS_RATELIMIT_UDP_FLOODING:
            pk_type = RULE_TYPE_PACKET_DOS_UDP_FLOODING;
            break;

        case SWDRV_DOS_RATELIMIT_WIN_NUKE:
            pk_type = RULE_TYPE_PACKET_DOS_WIN_NUKE;
            break;

        default:
            LOG("Invalid dos type = %lu", type);
            return FALSE;
    }

    memset(&flags, 0, sizeof(flags));
    if (TRUE == is_enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_ActionMeterConfigFlow(&dev_action, rate, rate * 1024 / 8))
    {
        LOG("<Error> config meter (flow)");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_ActionRedPacketTo(&dev_action, DEVRM_PKT_CMD_HARD_DROP))
    {
        LOG("<Error> Set red packet command");
        return FALSE;
    }

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, pk_type,
                NULL, &dev_action, NULL);
    if (RULE_TYPE_OK != result)
    {
        return FALSE;
    }

    if (pk_type2 < RULE_TYPE_PACKET_MAX_TYPE)
    {
        result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, pk_type2,
                    NULL, &dev_action, NULL);
        if (RULE_TYPE_OK != result)
        {
            memset(&flags, 0, sizeof(flags));
            RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, pk_type,
                    NULL, &dev_action, NULL);
            return RULE_TYPE_OK == result ? TRUE : FALSE;
        }
    }

    return TRUE;
}
#endif /* (SYS_CPNT_DOS == TRUE) */

#if (SYS_CPNT_VLAN_XLATE == TRUE && SYS_CPNT_VLAN_XLATE_BY_RULE == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetPortVlanXlate
 *------------------------------------------------------------------------------
 * FUNCTION: Set rule for VLAN translation
 * INPUT   : interface      - Which interface
 *           old_vid        - Old VLAN ID
 *           new_vid        - New VLAN ID; 0 means to remove.
 * OUTPUT  : None
 * RETURN  : RULE_TYPE_OK if succeeded; otherwise error code.
 * NOTE    : For each interface.type, following parameter of interface is needed.
 *             RULE_TYPE_INTERFACE_UPORT: type, uport, direction
 *             RULE_TYPE_INTERFACE_TRUNK: type, trunk_id, direction
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_CTRL_SetPortVlanXlate(
    RULE_TYPE_INTERFACE_INFO_T *interface,
    UI32_T old_vid,
    UI32_T new_vid)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_CTRL_OPT_FLAGS_T   flags;
    DEVRM_AceEntry_T        rule_pattern;
    DEVRM_ACTION_T          dev_action;
    UI32_T                  packet_type;

    ASSERT(NULL != interface);

    switch (interface->type)
    {
        case RULE_TYPE_INTERFACE_UPORT:
            packet_type = (RULE_TYPE_INBOUND == interface->direction)
                          ? RULE_TYPE_PACKET_VLAN_TRANSLATION
                          : RULE_TYPE_PACKET_EGRESS_VLAN_TRANSLATION;
            break;

        case RULE_TYPE_INTERFACE_TRUNK:
            if (RULE_TYPE_OUTBOUND == interface->direction)
            {
                /* Egress trunk shall bind uport rule on trunk member
                 */
                return RULE_TYPE_OK;
            }
            packet_type = RULE_TYPE_PACKET_TRUNK_VLAN_TRANSLATION;
            break;

        default:
            LOG("Invalid type=%d", interface->type);
            return RULE_TYPE_INVALID_PARAMETER;
    }

    memset(&flags, 0, sizeof(flags));
    if (0 != new_vid)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyVlan(&rule_pattern, old_vid, 0xfff))
    {
        LOG("<Error> Qualify SA");
        return RULE_TYPE_FAIL;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
    {
        LOG("<Error> Set packet command");
        return RULE_TYPE_FAIL;
    }

    if (TRUE != DEVRM_LIB_ActionModifyVlan(&dev_action, DEVRM_MODIFY_VLAN_CMD_FOR_TAGGED_ONLY, new_vid, FALSE))
    {
        LOG("<Error> Modify VLAN");
        return FALSE;
    }

    RULE_CTRL_EnterCriticalSection();

    if (interface->type == RULE_TYPE_INTERFACE_UPORT)
    {
        result = RULE_CTRL_LocalSetRuleForPacketTypeByUport(flags,
                     interface->uport.unit, interface->uport.port, packet_type,
                     &rule_pattern, &dev_action);
    }
    else
    {
        result = RULE_CTRL_LocalSetRuleForPacketTypeByTrunk(flags,
                     interface->trunk_id, packet_type,
                     &rule_pattern, &dev_action);
    }

    RULE_CTRL_LeaveCriticalSection();

    return result;
}
#endif /* SYS_CPNT_VLAN_XLATE_BY_RULE */

#if (SYS_CPNT_ND_BY_RULE == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IP6_SetNdRsPacketToCpu
 *------------------------------------------------------------------------------
 * PURPOSE  : Set rule for IPv6 ND Router solication packet.
 * INPUT    : enable    - TRUE, install rule;
 *                        FALSE, uninstall rule.
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IP6_SetNdRsPacketToCpu(
    BOOL_T enable)
{
#define RULE_CTRL_PROT_ICMP6                        58
#define RULE_CTRL_ICMP6_TYPE_ROUTER_SOLICITATION    133
    /*
     * ipv6
     * next header = 58
     * icmpv6 type = 133
     */

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T packet_type;

    RULE_CTRL_OPT_FLAGS_T flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    LOG("%s", enable ? "enable" : "disable");

    packet_type = RULE_TYPE_PACKET_IP6_ND_RS;

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIcmpv6Type(&rule_pattern, RULE_CTRL_ICMP6_TYPE_ROUTER_SOLICITATION, 0xff))
    {
        LOG("<Error> Qualify ICMPv6 type");
        return RULE_TYPE_FAIL;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
    {
        LOG("<Error> Set packet command");
        return RULE_TYPE_FAIL;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, packet_type,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return result;

#undef RULE_CTRL_PROT_ICMP6
#undef RULE_CTRL_ICMP6_TYPE_ROUTER_SOLICITATION
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IP6_SetNdRaPacketToCpu
 *------------------------------------------------------------------------------
 * PURPOSE  : Set rule for IPv6 ND Router Advertisement packet.
 * INPUT    : enable    - TRUE, install rule;
 *                        FALSE, uninstall rule.
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IP6_SetNdRaPacketToCpu(
    BOOL_T enable)
{
#define RULE_CTRL_PROT_ICMP6                        58
#define RULE_CTRL_ICMP6_TYPE_ROUTER_ADVERTISEMENT   134
    /*
     * ipv6
     * next header = 58
     * icmpv6 type = 134
     */
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T                  packet_type;

    RULE_CTRL_OPT_FLAGS_T   flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    LOG("%s", enable ? "enable" : "disable");

    packet_type = RULE_TYPE_PACKET_IP6_ND_RA;

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIcmpv6Type(&rule_pattern, RULE_CTRL_ICMP6_TYPE_ROUTER_ADVERTISEMENT, 0xff))
    {
        LOG("<Error> Qualify ICMPv6 type");
        return RULE_TYPE_FAIL;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
    {
        LOG("<Error> Set packet command");
        return RULE_TYPE_FAIL;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, packet_type,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return result;

#undef RULE_CTRL_PROT_ICMP6
#undef RULE_CTRL_ICMP6_TYPE_ROUTER_ADVERTISEMENT
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IP6_SetNdNsPacketToCpu
 *------------------------------------------------------------------------------
 * PURPOSE  : Set rule for IPv6 ND Neighbor Solicitation packet.
 * INPUT    : enable    - TRUE, install rule;
 *                        FALSE, uninstall rule.
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IP6_SetNdNsPacketToCpu(
    BOOL_T enable)
{
#define RULE_CTRL_PROT_ICMP6                        58
#define RULE_CTRL_ICMP6_TYPE_NEIGHBOR_SOLICITATION  135
    /*
     * ipv6
     * next header = 58
     * icmpv6 type = 135
     */
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T                  packet_type;

    RULE_CTRL_OPT_FLAGS_T   flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    LOG("%s", enable ? "enable" : "disable");

    packet_type = RULE_TYPE_PACKET_IP6_ND_NS;

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIcmpv6Type(&rule_pattern, RULE_CTRL_ICMP6_TYPE_NEIGHBOR_SOLICITATION, 0xff))
    {
        LOG("<Error> Qualify ICMPv6 type");
        return RULE_TYPE_FAIL;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
    {
        LOG("<Error> Set packet command");
        return RULE_TYPE_FAIL;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, packet_type,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return result;

#undef RULE_CTRL_PROT_ICMP6
#undef RULE_CTRL_ICMP6_TYPE_NEIGHBOR_SOLICITATION
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IP6_SetNdNaPacketToCpu
 *------------------------------------------------------------------------------
 * PURPOSE  : Set rule for IPv6 ND Neighbor Advertisement packet.
 * INPUT    : enable    - TRUE, install rule;
 *                        FALSE, uninstall rule.
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IP6_SetNdNaPacketToCpu(
    BOOL_T enable)
{
#define RULE_CTRL_PROT_ICMP6                        58
#define RULE_CTRL_ICMP6_TYPE_NEIGHBOR_ADVERTISEMENT 136
    /*
     * ipv6
     * next header = 58
     * icmpv6 type = 136
     */

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T                  packet_type;

    RULE_CTRL_OPT_FLAGS_T   flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    LOG("%s", enable ? "enable" : "disable");

    packet_type = RULE_TYPE_PACKET_IP6_ND_NA;

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIcmpv6Type(&rule_pattern, RULE_CTRL_ICMP6_TYPE_NEIGHBOR_ADVERTISEMENT, 0xff))
    {
        LOG("<Error> Qualify ICMPv6 type");
        return RULE_TYPE_FAIL;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
    {
        LOG("<Error> Set packet command");
        return RULE_TYPE_FAIL;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, packet_type,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return result;

#undef RULE_CTRL_PROT_ICMP6
#undef RULE_CTRL_ICMP6_TYPE_NEIGHBOR_ADVERTISEMENT
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IP6_SetNdRedirectPacketToCpu
 *------------------------------------------------------------------------------
 * PURPOSE  : Set rule for IPv6 ND Redirect packet.
 * INPUT    : enable    - TRUE, install rule;
 *                        FALSE, uninstall rule.
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IP6_SetNdRedirectPacketToCpu(
    BOOL_T enable)
{
#define RULE_CTRL_PROT_ICMP6                        58
#define RULE_CTRL_ICMP6_TYPE_REDIRECT               137
    /*
     * ipv6
     * next header = 58
     * icmpv6 type = 137
     */

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T                  packet_type;

    RULE_CTRL_OPT_FLAGS_T   flags;

    DEVRM_AceEntry_T rule_pattern;
    DEVRM_ACTION_T dev_action;

    LOG("%s", enable ? "enable" : "disable");

    packet_type = RULE_TYPE_PACKET_IP6_ND_REDIRECT;

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyIcmpv6Type(&rule_pattern, RULE_CTRL_ICMP6_TYPE_REDIRECT, 0xff))
    {
        LOG("<Error> Qualify ICMPv6 type");
        return RULE_TYPE_FAIL;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
    {
        LOG("<Error> Set packet command");
        return RULE_TYPE_FAIL;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, packet_type,
                &rule_pattern, &dev_action, NULL);

    RULE_CTRL_LeaveCriticalSection();

    return result;

#undef RULE_CTRL_PROT_ICMP6
#undef RULE_CTRL_ICMP6_TYPE_REDIRECT
}
#endif /* SYS_CPNT_ND_BY_RULE */

#if (SYS_CPNT_DAI_ARP_ACL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DAI_DropArpPacket
 *------------------------------------------------------------------------------
 * FUNCTION: This function will drop ARP packet by sender IP + source MAC
 * INPUT   : enable         - TREUE: Add the new entry to drop the specify ARP packet.
 *                            FALSE: Delete the specify entry.
 *           src_mac_addr   - Source mac address, src_mac_addr[SYS_ADPT_MAC_ADDR_LEN]
 *           src_ip_addr    - Source IP address
 * OUTPUT  : None
 * RETURN  : RULE_TYPE_RETURN_TYPE_T
 * NOTE    : The total entry for this function is
 *           SYS_ADPT_ARP_INSPECTION_MAX_NBR_OF_DENY_RULE
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_DAI_DropArpPacket(
    BOOL_T enable,
    UI8_T *src_mac_addr,
    UI32_T src_ip_addr)
{
    /* ethertype = RULE_TYPE_ETHERTYPE_ARP
     * SA
     * SIP
     */
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_CTRL_OPT_FLAGS_T   flags;
    DEVRM_AceEntry_T        rule_pattern;
    DEVRM_ACTION_T          dev_action;
    UI8_T da_mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    LOG("%s to drop DA=0x%02x%02x%02x%02x%02x%02x SIP=%lu",
        enable ? "enable" : "disable",
        src_mac_addr[0], src_mac_addr[1], src_mac_addr[2],
        src_mac_addr[3], src_mac_addr[4], src_mac_addr[5],
        src_ip_addr);

    memset(&flags, 0, sizeof(flags));
    if (TRUE == enable)
    {
        flags.INSTALL = 1;
        flags.FORCE_INSTALL = 1;
    }

    DEVRM_LIB_RulePatternInit(&rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifySourceMacAddress(&rule_pattern, src_mac_addr, da_mask))
    {
        LOG("<Error> Qualify SA");
        return RULE_TYPE_FAIL;
    }

    if (TRUE != DEVRM_LIB_RulePatternQualifyIpSourceAddress(&rule_pattern, src_ip_addr, 0xffffffff))
    {
        LOG("<Error> Qualify SIP");
        return RULE_TYPE_FAIL;
    }

    if (TRUE != DEVRM_LIB_RulePatternQualifyEtherType(&rule_pattern, RULE_TYPE_ETHERTYPE_ARP, 0xffff))
    {
        LOG("<Error> Qualify ARP ether type");
        return RULE_TYPE_FAIL;
    }

    DEVRM_LIB_ActionInit(&dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_HARD_DROP))
    {
        LOG("<Error> Set packet command");
        return RULE_TYPE_FAIL;
    }

    RULE_CTRL_EnterCriticalSection();

    result = RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(flags, RULE_TYPE_PACKET_DAI_ARP_DROP,
                                                         &rule_pattern, &dev_action, NULL);
    
    RULE_CTRL_LeaveCriticalSection();
    
    return result;

}

#endif /* SYS_CPNT_DAI_ARP_ACL */

#if (L4_SUPPORT_ACCTON_BACKDOOR == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DumpFunctionInfo
 *------------------------------------------------------------------------------
 * PURPOSE  : dump all functions' information
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : for debug (Not implement)
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_DumpFunctionInfo(void)
{
    RULE_CTRL_DumpGlobalAce_BackDoor();
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DumpResourceInfo
 *------------------------------------------------------------------------------
 * PURPOSE  : dump specified group information
 * INPUT    : unit, device_id, group_id
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : for debug (Not implement)
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DumpResourceInfo(UI32_T unit, UI32_T device_id, UI32_T group_id)
{
    RULE_CTRL_DumpDfltAceByPort_BackDoor(unit, device_id);
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DumpCpuInterfaceStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : to see the CPU interface via FP status
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : for debug (Not implement)
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_DumpCpuInterfaceStatus()
{
    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Backdoor_DiffServ_DumpClassInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : show class instance
 * INPUT    : class_instance
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : for debug
 *------------------------------------------------------------------------------
 */
void
RULE_CTRL_Backdoor_DiffServ_DumpClassInstance(
    const RULE_TYPE_CLASS_INSTANCE_PTR_T class_instance)
{
    RULE_CTRL_Priv_DumpClassInstance((RULE_TYPE_INSTANCE_PTR_T)class_instance, NULL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DumpSelectorInfo
 *------------------------------------------------------------------------------
 * PURPOSE  : dump specified selector information
 * INPUT    : unit, device_id, selector_id
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : for debug
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DumpSelectorInfo(UI32_T unit, UI32_T device_id, UI32_T selector_id)
{
    return TRUE;
}

#endif /* end of L4_SUPPORT_ACCTON_BACKDOOR == TRUE */

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_DumpGlobalAce_BackDoor
 *------------------------------------------------------------------------------
 * PURPOSE : Dump the database of global ace.
 *            (AF/DHCP Snooping/ ,etc...)
 *   INPUT : None
 *  OUTPUT : None
 *  RETURN : None
 *    NOTE : for backdoor only.
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_DumpGlobalAce_BackDoor(void)
{
    UI32_T  device_id;
    UI32_T  num_of_device;

    UI32_T  my_unit;

    char   *global_tcam_kmode_str [] = {
            "USE_MULTI_MODE",               /* SYS_CPNT_TCAM_SEARCH_KEY_MODE_USE_MULTI_MODE               */
            "USE_MAC_IPV4_IPV6_MODE6_ONLY", /* SYS_CPNT_TCAM_SEARCH_KEY_MODE_USE_MAC_IPV4_IPV6_MODE6_ONLY */
            "USE_IPV4_MODE1_ONLY"};         /* SYS_CPNT_TCAM_SEARCH_KEY_MODE_USE_IPV4_MODE1_ONLY          */

    RULE_CTRL_EnterCriticalSection();

    for (my_unit = 1; my_unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; my_unit++)
    {
        if (STKTPLG_OM_UnitExist(my_unit))
        {
            PRINTF("\r\n");
            PRINTF("Reserved Rule Num %d\r\n", RULE_CTRL_GLOBAL_RULE_CNT);

            num_of_device = RULE_CTRL_GetNumberOfChips(my_unit);

            for (device_id = 0; device_id < num_of_device; ++ device_id)
            {
                PRINTF("Unit %lu, Device %lu Free Pce Num %ld\r\n",
                       my_unit,
                       device_id,
                       shmem_data_p->stack_free_nbr_pce[my_unit-1][device_id]);
            }
        }
    }

    PRINTF("Global Tcam Mode %ld [%s]\r\n", shmem_data_p->global_tcam_key_mode,
                                             global_tcam_kmode_str [shmem_data_p->global_tcam_key_mode-1]);

    {
        RULE_TYPE_RETURN_TYPE_T result;

        UI32_T i;

        PRINTF("+-o Root\r\n");

        for (i = 0; i < _countof(shmem_data_p->if_table); ++ i)
        {
            RULE_CTRL_IF_ENTRY_PTR_T if_entry_p = &shmem_data_p->if_table[i];

            RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

            ASSERT(RULE_TYPE_NIL != if_entry_p->class_inst_ptr.type);

            if (RULE_TYPE_NIL == if_entry_p->class_inst_ptr.type)
            {
                continue;
            }

            RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                      &if_class_obj);

            {
                RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T agg_class_inst_iter;

                if_class_obj.get_element_iterator(&if_class_obj, &agg_class_inst_iter);
                if (0 == agg_class_inst_iter.count(&agg_class_inst_iter))
                {
                    continue;
                }
            }

            PRINTF("  |\r\n");
            PRINTF("  +-o IfEntry <");

            {
                RULE_TYPE_INTERFACE_INFO_T interface;

                result = RULE_CTRL_Priv_IfEntry_Index_To_Interface(i, &interface);
                ASSERT(RULE_TYPE_OK == result);

                PRINTF("direction = %s, ",
                       interface.direction == RULE_TYPE_INBOUND ? "ingress" :
                       interface.direction == RULE_TYPE_OUTBOUND ? "egress" :
                       interface.direction == RULE_TYPE_BOTH_DIRECTION ? "both" :
                       interface.direction == RULE_TYPE_UNSPEC_DIRECTION ? "unspec" : "??");

                switch (interface.type)
                {
                    case RULE_TYPE_INTERFACE_ALL_UNIT:
                    case RULE_TYPE_INTERFACE_UNIT:
                        PRINTF("iftype = global");
                        break;

                    case RULE_TYPE_INTERFACE_UPORT:
                        PRINTF("iftype = eth, uport = %lu/%lu",
                               interface.uport.unit,
                               interface.uport.port);
                        break;

                    case RULE_TYPE_INTERFACE_TRUNK:
                        PRINTF("iftype = trunk, trunkId = %lu", interface.trunk_id);
                        break;

                    case RULE_TYPE_INTERFACE_CRAFT_PORT:
                        PRINTF("iftype = craft port");
                        break;

                    case RULE_TYPE_INTERFACE_CPU_PORT:
                        PRINTF("iftype = cpu port");
                        break;

                    default:
                        ASSERT(0);
                        break;
                }
            }

            PRINTF(">\r\n");

            {
                RULE_CTRL_DUMP_CLASS_INSTANCE_PARAM_T param;

                memset(&param, 0, sizeof(param));
                param.indent = 4;

                PRINTF("%*s" "+-o instance ", param.indent, " ");

                param.indent += 2;
                RULE_CTRL_Priv_DumpClassInstance((RULE_TYPE_INSTANCE_PTR_T)if_class_obj.inst, &param);
            }

        }
    }

//        PRINTF("--------------------------------------------------------\r\n");
//        PRINTF("%-15s: / Available rule: %02ld/Free rule: %02d\r\n",
//               &rule_ctrl_global_ace_type_str[global_ace_type][21],
//               array.metadata->count_element,
//               (array.metadata->count_element - array.metadata->size));

    RULE_CTRL_LeaveCriticalSection();
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_DumpDfltAceByPort_BackDoor
 *------------------------------------------------------------------------------
 * PURPOSE : Dump the database of special usage for backdoor.
 *            (AF/DHCP Snooping/ ,etc...)
 *   INPUT : unit - unit
 *           port - port
 *  OUTPUT : None
 *  RETURN : None
 *    NOTE : for backdoor only.
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_DumpDfltAceByPort_BackDoor(UI32_T unit, UI32_T port)
{
    RULE_TYPE_INTERFACE_INFO_T interface;

    RULE_CTRL_EnterCriticalSection();

    memset(&interface, 0, sizeof(interface));

    interface.type = RULE_TYPE_INTERFACE_UPORT;
    interface.uport.unit = unit;
    interface.uport.port = port;

    for (interface.direction = RULE_TYPE_INBOUND;
         interface.direction <= RULE_TYPE_OUTBOUND;
         ++ interface.direction)
    {
        RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

        PRINTF("+-o IfEntry <");

        PRINTF("iftype = eth, uport = %lu/%lu",
               interface.uport.unit,
               interface.uport.port);

        if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
        if (NULL == if_entry_p)
        {
            PRINTF("ERROR>\r\n");
            return;
        }

        PRINTF(">\r\n");

        {
            RULE_CTRL_DUMP_CLASS_INSTANCE_PARAM_T param;

            RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;
            ASSERT(RULE_TYPE_NIL != if_entry_p->class_inst_ptr.type);

            if (RULE_TYPE_NIL == if_entry_p->class_inst_ptr.type)
            {
                return;
            }

            RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                      &if_class_obj);

            memset(&param, 0, sizeof(param));
            param.indent = 4;

            PRINTF("%*s" "+-o instance ", param.indent, " ");

            param.indent += 4;
            RULE_CTRL_Priv_DumpClassInstance((RULE_TYPE_INSTANCE_PTR_T)if_class_obj.inst, &param);
        }
    }

//        PRINTF("--------------------------------------------------------\r\n");
//        PRINTF("%-15s: / Available rule: %02ld /Free rule: %02d\r\n",
//               &rule_ctrl_dflt_ace_type_str[dflt_ace_type][19],
//               array.metadata->count_element,
//               (array.metadata->count_element - array.metadata->size));

    RULE_CTRL_LeaveCriticalSectionReturnVoid();
}

void
RULE_CTRL_InitFilter(
    RULE_CTRL_Filter_T *filter_p)
{
    memset(filter_p, 0, sizeof(*filter_p));

    filter_p->unit          = RULE_CTRL_INVALID;
    filter_p->device_id     = RULE_CTRL_INVALID;
    filter_p->group_id      = RULE_CTRL_INVALID;
    filter_p->port          = RULE_CTRL_INVALID;
    filter_p->func_type     = RULE_CTRL_INVALID;
}

void
RULE_CTRL_DumpRule(
    RULE_CTRL_Filter_T *filter_p,
    BOOL_T (*rule_printf)(char*, ...))
{
    if (rule_printf)
    {
        rule_printf("XD");
    }
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_GetPceCount
 *------------------------------------------------------------------------------
 * PURPOSE  : get tcam utilization
 * INPUT    : none
 * OUTPUT   : tcam_util_p
 * RETURN   : TRUE/FALSE.
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_GetTCAMUtilization(RULE_TYPE_TCAMUtilization_T *tcam_util_p)
{
    UI32_T unit;

    UI32_T device_id;

    UI32_T rule_size;

    ASSERT(NULL != tcam_util_p);

    if (NULL == tcam_util_p)
    {
        return FALSE;
    }

    memset(tcam_util_p, 0, sizeof(*tcam_util_p));

    switch (shmem_data_p->global_tcam_key_mode)
    {
        case SYS_CPNT_TCAM_SEARCH_KEY_MODE_USE_MAC_IPV4_IPV6_MODE6_ONLY:
            rule_size = 2;
            break;

        case SYS_CPNT_TCAM_SEARCH_KEY_MODE_USE_IPV4_MODE1_ONLY:
        case SYS_CPNT_TCAM_SEARCH_KEY_MODE_USE_MULTI_MODE:
        default:
            rule_size = 1;
            break;
    }

    RULE_CTRL_EnterCriticalSection();

    {
        typedef struct
        {
            UI32_T min;
            UI32_T max;
        } quota_t;

        typedef struct
        {
            UI32_T remapped_pkt_type;
            RULE_TYPE_INTERFACE_TYPE_E if_type;
            quota_t quota;
        } PACKET_TYPE_ENTRY_T;

        PACKET_TYPE_ENTRY_T remapped_pkt_type_tbl[] =
        {
#define PACKET_TYPE(pktType, ifType, remapAceType, quota) {pktType, ifType, {quota}},

            RULE_CONFIG_PACKET_TYPE_TABLE

#undef PACKET_TYPE
        };

        RULE_CTRL_UTILIZATION_T util;
        UI32_T mvl_pkt_type;

        RULE_CTRL_Priv_GetUtilization(&util);

        for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++ unit)
        {
            RULE_TYPE_TCAM_UNIT_UTIL_T *tcam_unit_p = &tcam_util_p->unit[unit-1];

            if (TRUE != STKTPLG_OM_UnitExist(unit))
            {
                continue;
            }

            tcam_util_p->num_of_unit ++;

            tcam_unit_p->num_of_device = RULE_CTRL_GetNumberOfChips(unit);

            ASSERT(SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT == _countof(tcam_util_p->unit[unit-1].device));
            ASSERT(tcam_unit_p->num_of_device <= _countof(tcam_util_p->unit[unit-1].device));

            for (device_id = 0; device_id < tcam_unit_p->num_of_device; ++ device_id)
            {
                RULE_TYPE_TCAM_DEVICE_UTIL_T *tcam_device_p = &tcam_unit_p->device[device_id];

                tcam_device_p->pce_total = RULE_CTRL_MAX_NBR_OF_PCE_PER_UNIT;
                tcam_device_p->pce_free = shmem_data_p->stack_free_nbr_pce[unit-1][device_id];

                tcam_device_p->pce_used = util.unit[unit - 1].device[device_id].total * rule_size;

                for (mvl_pkt_type = 0; mvl_pkt_type < _countof(remapped_pkt_type_tbl); ++ mvl_pkt_type)
                {
                    if (remapped_pkt_type_tbl[mvl_pkt_type].quota.min != 0)
                    {
                        tcam_device_p->pce_used_by_system += util.unit[unit - 1].device[device_id].each_type[mvl_pkt_type] * rule_size;
                    }
                    else
                    {
                        tcam_device_p->pce_used_by_user += util.unit[unit - 1].device[device_id].each_type[mvl_pkt_type] * rule_size;
                    }
                }

                tcam_device_p->pce_utility_rate = (tcam_device_p->pce_used * 100) / (float)tcam_device_p->pce_total;

                /*
                 * Update for TCAM unit
                 */
                tcam_unit_p->pce_total += tcam_device_p->pce_total;
                tcam_unit_p->pce_free  += tcam_device_p->pce_free;
                tcam_unit_p->pce_used  += tcam_device_p->pce_used;

                tcam_unit_p->pce_used_by_system += tcam_device_p->pce_used_by_system;
                tcam_unit_p->pce_used_by_user   += tcam_device_p->pce_used_by_user;

                tcam_unit_p->pce_utility_rate = (tcam_unit_p->pce_used * 100) / (float)tcam_unit_p->pce_total;
            }

            /*
             * Update for TCAM global
             */
            tcam_util_p->pce_total += tcam_unit_p->pce_total;
            tcam_util_p->pce_free  += tcam_unit_p->pce_free;
            tcam_util_p->pce_used  += tcam_unit_p->pce_used;

            tcam_util_p->pce_used_by_system += tcam_unit_p->pce_used_by_system;
            tcam_util_p->pce_used_by_user   += tcam_unit_p->pce_used_by_user;

            tcam_util_p->pce_utility_rate = (tcam_util_p->pce_used * 100) / (float)tcam_util_p->pce_total;
        }
    }

    RULE_CTRL_LeaveCriticalSection();

    return TRUE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_GetTcamEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : Get tcam entry
 * INPUT    : tcam_entry_p->unit
 *            tcam_entry_p->device_id
 *            tcam_entry_p->pool_id
 * OUTPUT   : tcam_util_p
 * RETURN   : OK/FAIL
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_GetTcamEntry(
    RULE_TYPE_TCAM_ENTRY_T *tcam_entry_p)
{
    typedef struct
    {
        UI32_T min;
        UI32_T max;
    } QUOTA_T;

    typedef struct
    {
        UI32_T remapped_pkt_type;
        RULE_TYPE_INTERFACE_TYPE_E if_type;
        QUOTA_T quota;
    } PACKET_TYPE_ENTRY_T;

    PACKET_TYPE_ENTRY_T remapped_pkt_type_tbl[] =
    {
#define PACKET_TYPE(pktType, ifType, remapAceType, quota) {pktType, ifType, {quota}},

        RULE_CONFIG_PACKET_TYPE_TABLE

#undef PACKET_TYPE
    };

    UI32_T unit, device_id, pool_id;
    UI32_T total_device;
    UI32_T rule_size;

    if (NULL == tcam_entry_p)
    {
        ASSERT(0);
        return RULE_TYPE_FAIL;
    }

    LOG("unit:%lu, device: %lu, pool %lu",
        tcam_entry_p->unit, tcam_entry_p->device_id, tcam_entry_p->pool_id);

    unit = tcam_entry_p->unit;
    device_id = tcam_entry_p->device_id;
    pool_id = tcam_entry_p->pool_id;

    if (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK < unit
        || TRUE != STKTPLG_OM_UnitExist(unit))
    {
        ASSERT(0);
        return RULE_TYPE_FAIL;
    }

    total_device = RULE_CTRL_GetNumberOfChips(unit);
    if (total_device <= device_id)
    {
        ASSERT(0);
        return RULE_TYPE_FAIL;
    }

    if (RULE_CTRL_MAX_TCAM_POOL_NUMBER <= pool_id)
    {
        ASSERT(0);
        return RULE_TYPE_FAIL;
    }

    tcam_entry_p->total = 0;
    tcam_entry_p->free = 0;
    tcam_entry_p->used = 0;

    switch (shmem_data_p->global_tcam_key_mode)
    {
        case SYS_CPNT_TCAM_SEARCH_KEY_MODE_USE_MAC_IPV4_IPV6_MODE6_ONLY:
            rule_size = 2;
            break;

        case SYS_CPNT_TCAM_SEARCH_KEY_MODE_USE_IPV4_MODE1_ONLY:
        case SYS_CPNT_TCAM_SEARCH_KEY_MODE_USE_MULTI_MODE:
        default:
            rule_size = 1;
            break;
    }

    {
        RULE_CTRL_UTILIZATION_T util;
        UI32_T mvl_pkt_type;

        RULE_CTRL_EnterCriticalSection();
        RULE_CTRL_Priv_GetUtilization(&util);

        tcam_entry_p->total = RULE_CTRL_MAX_NBR_OF_PCE_PER_UNIT;

        for (mvl_pkt_type = 0; mvl_pkt_type < _countof(remapped_pkt_type_tbl); ++ mvl_pkt_type)
        {
            UI32_T used_rule_number = util.unit[unit - 1].device[device_id].each_type[mvl_pkt_type];

            if ((remapped_pkt_type_tbl[mvl_pkt_type].quota.min != 0)
                && (used_rule_number < remapped_pkt_type_tbl[mvl_pkt_type].quota.min))
            {
                /* Reserved rule should be considered as used
                 */
                used_rule_number = remapped_pkt_type_tbl[mvl_pkt_type].quota.min;
            }

            tcam_entry_p->used += used_rule_number * rule_size;
        }

        tcam_entry_p->free = tcam_entry_p->total - tcam_entry_p->used;

        RULE_CTRL_LeaveCriticalSection();
    }

    RULE_CTRL_GetTcamPoolCapabilityBitMap(pool_id, &tcam_entry_p->cap_bitmap);

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_GetNextTcamEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : Get next tcam entry
 * INPUT    : tcam_entry_p->unit
 *            tcam_entry_p->device_id
 *            tcam_entry_p->pool_id
 * OUTPUT   : tcam_util_p
 * RETURN   : OK/FAIL
 * NOTE     : For get first entry,
 *            use index with tcam_entry_p->unit = 0
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_GetNextTcamEntry(
    RULE_TYPE_TCAM_ENTRY_T *tcam_entry_p)
{
    UI32_T total_device;

    UI32_T unit, device_id, pool_id;

    RULE_TYPE_RETURN_TYPE_T result;

    LOG("unit:%lu, device: %lu, pool %lu",
        tcam_entry_p->unit, tcam_entry_p->device_id, tcam_entry_p->pool_id);

    if (NULL == tcam_entry_p)
    {
        ASSERT(0);
        return RULE_TYPE_FAIL;
    }

    /* Initial the input index to get next entry
     */
    if (tcam_entry_p->unit == 0)
    {
        unit = STKTPLG_POM_GetNextUnit(&unit);
        device_id = 0;
        pool_id = 0;
    }
    else
    {
        unit = tcam_entry_p->unit;

        if (tcam_entry_p->device_id == RULE_TYPE_UNDEF_DEVICE_ID)
        {
            device_id = 0;
            pool_id = 0;
        }
        else
        {
            device_id = tcam_entry_p->device_id;
            pool_id = (tcam_entry_p->pool_id == RULE_TYPE_UNDEF_POOL_ID)
                      ? 0 : tcam_entry_p->pool_id + 1;
        }
    }

    for (; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++unit)
    {
        if (TRUE != STKTPLG_OM_UnitExist(unit))
        {
            device_id = 0;
            pool_id = 0;
            continue;
        }

        total_device = RULE_CTRL_GetNumberOfChips(unit);

        for (; device_id < total_device; ++device_id)
        {
            for (; pool_id < RULE_CTRL_MAX_TCAM_POOL_NUMBER; ++pool_id)
            {
                tcam_entry_p->unit = unit;
                tcam_entry_p->device_id = device_id;
                tcam_entry_p->pool_id = pool_id;

                result = RULE_CTRL_GetTcamEntry(tcam_entry_p);
                ASSERT(result == RULE_TYPE_OK);

                return result;
            }
            pool_id = 0;
        }

        device_id = 0;
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_GetSupportedTcamCapabilityBitMap
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets TCAM all supporting capability
 * INPUT    :
 * OUTPUT   : bit_map_p
 * RETURN   : Error code.
 * NOTE     :
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_GetSupportedTcamCapabilityBitMap(
    RULE_TYPE_TCAM_CAP_BITMAP_T *bit_map_p)
{
    int i;

    memset(bit_map_p, 0, sizeof(*bit_map_p));

    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_MAC_ACL);

    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_IPV4_ACL);
    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_IPV6_STD_ACL);
    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_IPV6_EXT_ACL);

    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_MAC_DS);
    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_IPV4_DS);
    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_IPV6_STD_DS);
    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_IPV6_EXT_DS);

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_EGRESS_MAC_ACL);
    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_EGRESS_IPV4_ACL);
    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_EGRESS_IPV6_STD_ACL);
    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_EGRESS_IPV6_EXT_ACL);

    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_EGRESS_MAC_DS);
    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_EGRESS_IPV4_DS);
    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_EGRESS_IPV6_STD_DS);
    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_EGRESS_IPV6_EXT_DS);
#endif /* SYS_CPNT_QOS_V2_EGRESS_PORT */

#if (SYS_CPNT_WEBAUTH == TRUE)
    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_WEB_AUTH);
#endif /* SYS_CPNT_WEBAUTH */

#if (SYS_CPNT_IP_SOURCE_GUARD == TRUE)
    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_IP_SOURCE_GUSRD);
#endif /* SYS_CPNT_IP_SOURCE_GUARD */

#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_IPV6_SOURCE_GUSRD);
#endif /* SYS_CPNT_IPV6_SOURCE_GUARD */

#if (SYS_CPNT_INGRESS_RATE_LIMIT_VIA_FILTER_ENGINE == TRUE)
    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_RATE_LIMITE);
#endif /* SYS_CPNT_INGRESS_RATE_LIMIT_VIA_FILTER_ENGINE */

    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_CPU_INTERFACE);

    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_LINK_LOCAL);

    return RULE_TYPE_OK;
}

/* LOCAL SUBPROGRAM BODIES */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_UpdateDevInfo
 *------------------------------------------------------------------------------
 * PURPOSE:  Update device information
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:     Invoke when unit received a enter master mode, a hot insertion,
 *           and hot removal event
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_UpdateDevInfo()
{
    UI32_T  unit;

    memset(shmem_data_p->dev_info, 0, sizeof(shmem_data_p->dev_info));

    unit = 0;

    RULE_CTRL_EnterCriticalSection();

    while (TRUE == STKTPLG_POM_GetNextDriverUnit(&unit))
    {
        if (FALSE == RULE_CTRL_LocalGetDevInfo(unit, &shmem_data_p->dev_info[unit-1]))
        {
            LOG("RULE_CTRL_GetDevInfo failed on unit(%lu)", unit);
        }

        LOG("dev_info[%lu].num_of_chips=%lu", unit-1, shmem_data_p->dev_info[unit-1].num_of_chips);
    }

    RULE_CTRL_LeaveCriticalSection();

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_GetNumberOfChips
 *------------------------------------------------------------------------------
 * PURPOSE:  Get the number of chips on the unit
 * INPUT:    unit - unit number
 * OUTPUT:   None
 * RETURN:   number of chips
 * NOTE:
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_CTRL_GetNumberOfChips(
    UI32_T unit)
{
    if (unit < 1 || SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK < unit)
    {
        return 0;
    }

    return shmem_data_p->dev_info[unit-1].num_of_chips;
}

#if (SYS_CPNT_BPDU_LOOPBACK_RATELIMIT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalConfigBpduPacket
 *------------------------------------------------------------------------------
 * PURPOSE  : set up rule-pattern and action for BPDU rate-limit rule
 * INPUT    : rate_limit - TRUE: limit rate
 *                        FALSE: not limit rate
 * OUTPUT   : rule_pattern
 *            dev_action
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static BOOL_T RULE_CTRL_LocalConfigBpduPacket(
    BOOL_T rate_limit, 
    DEVRM_AceEntry_T *rule_pattern,
    DEVRM_ACTION_T *dev_action
)
{
    UI8_T multicast_mac_addr[] = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x00};
    UI8_T multicast_mac_mask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	DEVRM_LIB_RulePatternInit(rule_pattern);

    if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(rule_pattern, multicast_mac_addr, multicast_mac_mask))
    {
        LOG("<Error> Qualify DA");
        return FALSE;
    }

    DEVRM_LIB_ActionInit(dev_action);

    if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    }

    if (TRUE != DEVRM_LIB_ActionModifyCpuCode(dev_action, DEV_NICDRV_LIB_BpduCpuCode(rate_limit)))
    {
        LOG("<Error> Set packet command");
        return FALSE;
    } 

    return TRUE;
}
#endif

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConfigGlobalRulePcl(
    UI32_T remapped_pkt_type)
{
    RULE_TYPE_AclType_T acl_key_mode = RULE_TYPE_IP_EXT_ACL;

    RULE_TYPE_RETURN_TYPE_T result;

    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num;

    RULE_TYPE_INTERFACE_INFO_T interface;
    RULE_CTRL_PCL_CFG_T pcl_cfg;

    UI32_T device_id;
    UI32_T num_of_device;

    BOOL_T b_result;

    memset(&interface, 0, sizeof(interface));

    interface.type = RULE_TYPE_INTERFACE_UNIT;
    STKTPLG_OM_GetMyUnitID(&interface.unit);

    ASSERT(0 != interface.unit);

    result = RULE_CTRL_Priv_ConvertPacketTypeToToDirectionLookupNum(remapped_pkt_type,
                                                                    &interface.direction,
                                                                    &lookup_num);
    ASSERT(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_GetPclConfig(&interface, lookup_num, &pcl_cfg);
    ASSERT(RULE_TYPE_OK == result);

    num_of_device = DEVRM_PMGR_GetNumberOfChips();

    for (device_id = 0; device_id < num_of_device; ++ device_id)
    {
        b_result = DEVRM_PMGR_CreatePcl(device_id,
                                        pcl_cfg.pcl_id,
                                        RULE_TYPE_INBOUND == interface.direction ? TRUE : FALSE,
                                        lookup_num,
                                        acl_key_mode);
        ASSERT(TRUE == b_result);
        if (TRUE != b_result)
        {
            PRINTF("%s(%d):\r\n"
                   "  Create PCL failed. pktType = %s, deviceId = %lu, pclId = %lu, dir = %s, lookupNum = %lu, keyMode = %lu\r\n",
                   __FUNCTION__,
                   __LINE__,
                   RULE_CTRL_REMAPPED_PACKET_TYPE_STR(remapped_pkt_type),
                   device_id,
                   pcl_cfg.pcl_id,
                   RULE_TYPE_INBOUND == interface.direction ? "in" : "out",
                   lookup_num,
                   acl_key_mode);
        }
    }

    return RULE_TYPE_OK;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConfigCraftPortRulePcl(
    UI32_T remapped_pkt_type)
{
#if (RULE_CTRL_HAVE_CRAFT_PORT == TRUE)
    RULE_TYPE_AclType_T acl_key_mode = RULE_TYPE_IP_EXT_ACL;

    RULE_TYPE_RETURN_TYPE_T result;

    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num;

    RULE_TYPE_INTERFACE_INFO_T interface;
    RULE_CTRL_PCL_CFG_T pcl_cfg;

    UI8_T  device_id, craft_phy_port;

    BOOL_T b_result;

    memset(&interface, 0, sizeof(interface));

    interface.type = RULE_TYPE_INTERFACE_CRAFT_PORT;
    STKTPLG_OM_GetMyUnitID(&interface.unit);

    ASSERT(0 != interface.unit);

    result = RULE_CTRL_Priv_ConvertPacketTypeToToDirectionLookupNum(remapped_pkt_type,
                                                                    &interface.direction,
                                                                    &lookup_num);
    ASSERT(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_GetPclConfig(&interface, lookup_num, &pcl_cfg);
    ASSERT(RULE_TYPE_OK == result);

    STKTPLG_BOARD_GetCraftPortInfo(&device_id, &craft_phy_port);

    b_result = DEVRM_PMGR_CreatePcl(device_id,
                                    pcl_cfg.pcl_id,
                                    RULE_TYPE_INBOUND == interface.direction ? TRUE : FALSE,
                                    lookup_num,
                                    acl_key_mode);
    ASSERT(TRUE == b_result);
    if (TRUE != b_result)
    {
        PRINTF("%s(%d):\r\n"
               "  Create PCL failed. pktType = %s, deviceId = %lu, pclId = %lu, dir = %s, lookupNum = %lu, keyMode = %lu\r\n",
               __FUNCTION__,
               __LINE__,
               RULE_CTRL_REMAPPED_PACKET_TYPE_STR(remapped_pkt_type),
               device_id,
               pcl_cfg.pcl_id,
               RULE_TYPE_INBOUND == interface.direction ? "in" : "out",
               lookup_num,
               acl_key_mode);
    }

    return RULE_TYPE_OK;
#else
    /** Failure configuration
     */
    ASSERT(0);
    return RULE_TYPE_FAIL;
#endif
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConfigCpuPortRulePcl(
    UI32_T remapped_pkt_type)
{
    RULE_TYPE_AclType_T acl_key_mode = RULE_TYPE_IP_EXT_ACL;

    RULE_TYPE_RETURN_TYPE_T result;

    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num;

    RULE_TYPE_INTERFACE_INFO_T interface;
    RULE_CTRL_PCL_CFG_T pcl_cfg;

    UI32_T num_of_device;
    UI32_T device_id;

    BOOL_T b_result;

    memset(&interface, 0, sizeof(interface));

    interface.type = RULE_TYPE_INTERFACE_CPU_PORT;
    STKTPLG_OM_GetMyUnitID(&interface.unit);

    ASSERT(0 != interface.unit);

    result = RULE_CTRL_Priv_ConvertPacketTypeToToDirectionLookupNum(remapped_pkt_type,
                                                                    &interface.direction,
                                                                    &lookup_num);
    ASSERT(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_GetPclConfig(&interface, lookup_num, &pcl_cfg);
    ASSERT(RULE_TYPE_OK == result);

    num_of_device = DEVRM_PMGR_GetNumberOfChips();
    for (device_id = 0; device_id < num_of_device; ++ device_id)
    {
        b_result = DEVRM_PMGR_CreatePcl(device_id,
                                        pcl_cfg.pcl_id,
                                        RULE_TYPE_INBOUND == interface.direction ? TRUE : FALSE,
                                        lookup_num,
                                        acl_key_mode);
        ASSERT(TRUE == b_result);
        if (TRUE != b_result)
        {
            PRINTF("%s(%d):\r\n"
                   "  Create PCL failed. pktType = %s, deviceId = %lu, pclId = %lu, dir = %s, lookupNum = %lu, keyMode = %lu\r\n",
                   __FUNCTION__,
                   __LINE__,
                   RULE_CTRL_REMAPPED_PACKET_TYPE_STR(remapped_pkt_type),
                   device_id,
                   pcl_cfg.pcl_id,
                   RULE_TYPE_INBOUND == interface.direction ? "in" : "out",
                   lookup_num,
                   acl_key_mode);
            continue;
        }
    }

    return RULE_TYPE_OK;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConfigTrunkPortRulePcl(
    UI32_T remapped_pkt_type)
{
    RULE_TYPE_AclType_T acl_key_mode = RULE_TYPE_IP_EXT_ACL;

    RULE_TYPE_RETURN_TYPE_T result;

    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num;

    RULE_TYPE_INTERFACE_INFO_T interface;
    RULE_CTRL_PCL_CFG_T pcl_cfg;

    BOOL_T b_result;

    memset(&interface, 0, sizeof(interface));

    interface.type = RULE_TYPE_INTERFACE_TRUNK;
    STKTPLG_OM_GetMyUnitID(&interface.unit);

    ASSERT(0 != interface.unit);

    result = RULE_CTRL_Priv_ConvertPacketTypeToToDirectionLookupNum(remapped_pkt_type,
                                                                    &interface.direction,
                                                                    &lookup_num);
    ASSERT(RULE_TYPE_OK == result);

    for (interface.trunk_id = 1; interface.trunk_id <= SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM; ++ interface.trunk_id)
    {
        UI32_T num_of_device = DEVRM_PMGR_GetNumberOfChips();
        UI32_T device_id;

        result = RULE_CTRL_Priv_GetPclConfig(&interface, lookup_num, &pcl_cfg);
        ASSERT(RULE_TYPE_OK == result);

        for (device_id = 0; device_id < num_of_device; ++ device_id)
        {
            b_result = DEVRM_PMGR_CreatePcl(device_id,
                                            pcl_cfg.pcl_id,
                                            RULE_TYPE_INBOUND == interface.direction ? TRUE : FALSE,
                                            lookup_num,
                                            acl_key_mode);
            ASSERT(TRUE == b_result);
            if (TRUE != b_result)
            {
                PRINTF("%s(%d):\r\n"
                       "  Create PCL failed. pktType = %s, deviceId = %lu, pclId = %lu, dir = %s, lookupNum = %lu, keyMode = %lu\r\n",
                       __FUNCTION__,
                       __LINE__,
                       RULE_CTRL_REMAPPED_PACKET_TYPE_STR(remapped_pkt_type),
                       device_id,
                       pcl_cfg.pcl_id,
                       RULE_TYPE_INBOUND == interface.direction ? "in" : "out",
                       lookup_num,
                       acl_key_mode);
            }
        }
    }

    return RULE_TYPE_OK;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConfigUserPortRulePcl(
    UI32_T remapped_pkt_type)
{
    RULE_TYPE_AclType_T acl_key_mode = RULE_TYPE_IP_EXT_ACL;

    RULE_TYPE_RETURN_TYPE_T result;

    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num;

    RULE_TYPE_INTERFACE_INFO_T interface;
    RULE_CTRL_PCL_CFG_T pcl_cfg;

    UI32_T max_port;

    BOOL_T b_result;

    memset(&interface, 0, sizeof(interface));

    interface.type = RULE_TYPE_INTERFACE_UPORT;
    STKTPLG_OM_GetMyUnitID(&interface.unit);

    ASSERT(0 != interface.unit);

    result = RULE_CTRL_Priv_ConvertPacketTypeToToDirectionLookupNum(remapped_pkt_type,
                                                                    &interface.direction,
                                                                    &lookup_num);
    ASSERT(RULE_TYPE_OK == result);

    STKTPLG_POM_GetMaxPortNumberOnBoard((UI8_T) interface.uport.unit, &max_port);
    for (interface.uport.port = 1; interface.uport.port <= max_port; ++ interface.uport.port)
    {
        UI32_T module_id, device_id, phy_port;

        b_result = DEV_SWDRV_PMGR_Logical2PhyDevicePortID(interface.uport.unit,
                                                          interface.uport.port,
                                                          &module_id,
                                                          &device_id,
                                                          &phy_port);
        ASSERT(TRUE == b_result);
        if (TRUE != b_result)
        {
            LOG("<Error> Failed to get phy info on eth %lu/%lu",
                interface.uport.unit,
                interface.uport.port);
            continue;
        }

        result = RULE_CTRL_Priv_GetPclConfig(&interface, lookup_num, &pcl_cfg);
        ASSERT(RULE_TYPE_OK == result);

        b_result = DEVRM_PMGR_CreatePcl(device_id,
                                        pcl_cfg.pcl_id,
                                        RULE_TYPE_INBOUND == interface.direction ? TRUE : FALSE,
                                        lookup_num,
                                        acl_key_mode);
        ASSERT(TRUE == b_result);
        if (TRUE != b_result)
        {
            PRINTF("%s(%d):\r\n"
                   "  Create PCL failed. pktType = %s, deviceId = %lu, pclId = %lu, dir = %s, lookupNum = %lu, keyMode = %lu\r\n",
                   __FUNCTION__,
                   __LINE__,
                   RULE_CTRL_REMAPPED_PACKET_TYPE_STR(remapped_pkt_type),
                   device_id,
                   pcl_cfg.pcl_id,
                   RULE_TYPE_INBOUND == interface.direction ? "in" : "out",
                   lookup_num,
                   acl_key_mode);
        }
    }

    return RULE_TYPE_OK;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_BindPclToPort(
    BOOL_T is_bind,
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T pcl_id)
{
    switch (interface_p->type)
    {
        case RULE_TYPE_INTERFACE_UPORT:
            return RULE_CTRL_BindPclToUserPort(is_bind,
                                               interface_p->uport.unit,
                                               interface_p->uport.port,
                                               pcl_id);

        case RULE_TYPE_INTERFACE_TRUNK:
            return RULE_CTRL_BindPclToTrunkPort(is_bind,
                                                interface_p->trunk_id,
                                                pcl_id);

        case RULE_TYPE_INTERFACE_ALL_UNIT:
            return RULE_CTRL_BindPclToGlobal(is_bind, pcl_id);

        case RULE_TYPE_INTERFACE_CPU_PORT:
            return RULE_CTRL_BindPclToCpuPort(is_bind, pcl_id);

        case RULE_TYPE_INTERFACE_CRAFT_PORT:
            return RULE_CTRL_BindPclToCraftPort(is_bind, pcl_id);
            break;

        default:
            ASSERT(0);
            break;
    }

    return RULE_TYPE_FAIL;
}

static BOOL_T
RULE_CTRL_GetLookupNum(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T pcl_id,
    DEVRM_PCL_LOOKUP_NUMBER_T *lookup_num_p)
{
    if (RULE_TYPE_INTERFACE_UPORT != interface_p->type &&
        RULE_TYPE_INTERFACE_TRUNK != interface_p->type &&
        RULE_TYPE_INTERFACE_UNIT  != interface_p->type &&
        RULE_TYPE_INTERFACE_ALL_UNIT != interface_p->type)
    {
        return FALSE;
    }

    /* PCL has 10 bit, 
     *    000   -         0        -   000000
     *  look-up -    interface     -   id
     *               (ether/trunk)
     */
    switch (pcl_id & 0x00000380)
    {
        case 0x00000080:
            *lookup_num_p = DEVRM_PCL_LOOKUP_0_0;
            break;

        case 0x00000100:
            *lookup_num_p = DEVRM_PCL_LOOKUP_0_1;
            break;

        case 0x00000180:
            *lookup_num_p = DEVRM_PCL_LOOKUP_1;
            break;

        default:
            return FALSE;
    }

    return TRUE;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_BindPclToGlobal(
    BOOL_T is_bind,
    UI32_T pcl_id)
{
    UI32_T total_port, num_of_device;
    UI32_T unit, port, device_id, trunk_id;
    UI8_T b_result;

    unit = 0;
    while (TRUE == STKTPLG_POM_GetNextDriverUnit(&unit))
    {
        STKTPLG_POM_GetMaxPortNumberOnBoard((UI8_T) unit, &total_port);
        for (port = 1; port <= total_port; ++ port)
        {
            UI32_T module_id;
            UI32_T phy_port;
            RULE_TYPE_INTERFACE_INFO_T interface;
            RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;
            DEVRM_PCL_LOOKUP_NUMBER_T lookup_num;

            memset(&interface, 0, sizeof (interface));
            interface.type = RULE_TYPE_INTERFACE_UPORT;
            interface.uport.unit = unit;
            interface.uport.port = port;

            if (FALSE == RULE_CTRL_GetLookupNum(&interface, pcl_id, &lookup_num))
            {
                ASSERT(0);
                continue;
            }

            if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
            if (if_entry_p->lookup_rule_cnt[lookup_num] != 0)
            {
                /* This port is binding to per-port PCL
                 */
                continue;
            }

            b_result = DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port);
            ASSERT(TRUE == b_result);
            if (TRUE != b_result)
            {
                LOG("<Error> Failed to get phy info on eth %lu/%lu", unit, port);
                continue;
            }

            if (is_bind == TRUE)
            {
                b_result = RULE_CTRL_LocalBindPclToPort(unit, device_id, phy_port, pcl_id);
            }
            else
            {
                b_result = RULE_CTRL_LocalUnbindPclToPort(unit, device_id, phy_port, pcl_id);
            }

            if (TRUE != b_result)
            {
                ASSERT(0);
                LOG("\r\n  Failed. unit = %lu, port = %lu, pclId = %lu\r\n",
                       unit,
                       port,
                       pcl_id);
                return RULE_TYPE_FAIL;
            }
        }

        num_of_device = RULE_CTRL_GetNumberOfChips(unit);

        for (trunk_id = 1; trunk_id <= SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM; ++ trunk_id)
        {
            RULE_TYPE_INTERFACE_INFO_T interface;
            RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;
            DEVRM_PCL_LOOKUP_NUMBER_T lookup_num;

            memset(&interface, 0, sizeof (interface));
            interface.type = RULE_TYPE_INTERFACE_TRUNK;
            interface.unit = unit;
            interface.trunk_id = trunk_id;

            if (FALSE == RULE_CTRL_GetLookupNum(&interface, pcl_id, &lookup_num))
            {
                ASSERT(0);
                continue;
            }

            if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
            if (if_entry_p->lookup_rule_cnt[lookup_num] != 0)
            {
                /* This port is binding to per-port PCL
                 */
                continue;
            }

            for (device_id = 0; device_id < num_of_device; ++ device_id)
            {
                if (is_bind == TRUE)
                {
                    b_result = RULE_CTRL_LocalBindPclToTrunkPort(unit, device_id, trunk_id, pcl_id);
                }
                else
                {
                    b_result = RULE_CTRL_LocalUnbindPclToTrunkPort(unit, device_id, trunk_id, pcl_id);
                }

                if (TRUE != b_result)
                {
                    ASSERT(0);
                    LOG("\r\n Failed. unit = %lu, deviceId = %lu, trunkId = %lu, pclId = %lu\r\n",
                        unit,
                        device_id,
                        trunk_id,
                        pcl_id);

                    return RULE_TYPE_FAIL;
                }
            }
        }
    }
    return RULE_TYPE_OK;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_BindPclToCraftPort(
    BOOL_T is_bind,
    UI32_T pcl_id)
{
#if (RULE_CTRL_HAVE_CRAFT_PORT == TRUE)
    BOOL_T b_result;

    UI8_T  device_id, craft_phy_port;
    UI32_T unit;
    RULE_TYPE_RETURN_TYPE_T result;

    STKTPLG_OM_GetMyUnitID(&unit);

    STKTPLG_BOARD_GetCraftPortInfo(&device_id, &craft_phy_port);
    result = RULE_CTRL_Priv_GetCraftPortInfo(&device_id, &craft_phy_port);
    if (result == RULE_TYPE_FAIL)
    {
        LOG("Get craft port fail");

        return RULE_TYPE_FAIL;
    }

    unit = 0;
    while (TRUE == STKTPLG_POM_GetNextDriverUnit(&unit))
    {
        b_result = RULE_CTRL_LocalBindPclToPort(unit, device_id, craft_phy_port, pcl_id);
        if (TRUE != b_result)
        {
            ASSERT(0);
            LOG("\r\n  Bind PCL failed. unit = %lu, deviceId = %lu, craft_phy_port = %lu, pclId = %lu\r\n",
                unit,
                device_id,
                craft_phy_port,
                pcl_id);
            return RULE_TYPE_FAIL;
        }

    }

    return RULE_TYPE_OK;
#else
    /** Failure configuration
     */
    ASSERT(0);
    return RULE_TYPE_FAIL;
#endif
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_BindPclToCpuPort(
    BOOL_T is_bind,
    UI32_T pcl_id)
{
    //
    // FIXME: Get CPU phy port from gateway
    //
    enum
    {
        CPU_PHY_PORT  = 63
    };

    UI32_T num_of_device;
    UI32_T unit, device_id;
    UI32_T phy_port  = CPU_PHY_PORT;
    BOOL_T b_result;

    unit = 0;
    while (TRUE == STKTPLG_POM_GetNextDriverUnit(&unit))
    {
        num_of_device = RULE_CTRL_GetNumberOfChips(unit);

        for (device_id = 0; device_id < num_of_device; ++ device_id)
        {
            if (is_bind == TRUE)
            {
                b_result = RULE_CTRL_LocalBindPclToPort(unit, device_id, phy_port, pcl_id);
            }
            else
            {
                b_result = RULE_CTRL_LocalUnbindPclToPort(unit, device_id, phy_port, pcl_id);
            }

            if (TRUE != b_result)
            {
                ASSERT(0);
                LOG("\r\n  Bind PCL failed. unit = %lu, deviceId = %lu, phy_port = %lu, pclId = %lu\r\n",
                    unit,
                    device_id,
                    phy_port,
                    pcl_id);

                return RULE_TYPE_FAIL;
            }
        }
    }

    return RULE_TYPE_OK;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_BindPclToTrunkPort(
    BOOL_T is_bind,
    UI32_T trunk_id,
    UI32_T pcl_id)
{
    UI32_T unit;
    BOOL_T b_result;

    unit = 0;
    while (TRUE == STKTPLG_POM_GetNextDriverUnit(&unit))
    {
        UI32_T num_of_device = RULE_CTRL_GetNumberOfChips(unit);
        UI32_T device_id;

        for (device_id = 0; device_id < num_of_device; ++ device_id)
        {
            if (is_bind == TRUE)
            {
                b_result = RULE_CTRL_LocalBindPclToTrunkPort(unit, device_id, trunk_id, pcl_id);
            }
            else
            {
                b_result = RULE_CTRL_LocalUnbindPclToTrunkPort(unit, device_id, trunk_id, pcl_id);
            }

            if (TRUE != b_result)
            {
                ASSERT(0);
                LOG("\r\n  Bind PCL failed. unit = %lu, deviceId = %lu, trunk_id = %lu, pclId = %lu\r\n",
                    unit,
                    device_id,
                    trunk_id,
                    pcl_id);
            }
        }
    }

    return RULE_TYPE_OK;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_BindPclToUserPort(
    BOOL_T is_bind,
    UI32_T unit,
    UI32_T port,
    UI32_T pcl_id)
{
    UI32_T module_id, device_id, phy_port;
    BOOL_T b_result;

    b_result = DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit,
                                                      port,
                                                      &module_id,
                                                      &device_id,
                                                      &phy_port);
    ASSERT(TRUE == b_result);
    if (TRUE != b_result)
    {
        LOG("<Error> Failed to get phy info on eth %lu/%lu",
            unit,
            port);
        return RULE_TYPE_FAIL;
    }

    if (is_bind == TRUE)
    {
        b_result = RULE_CTRL_LocalBindPclToPort(unit, device_id, phy_port, pcl_id);
    }
    else
    {
        b_result = RULE_CTRL_LocalUnbindPclToPort(unit, device_id, phy_port, pcl_id);
    }

    if (TRUE != b_result)
    {
        ASSERT(0);
        LOG("\r\n  deviceId = %lu, phyPort = %lu, pclId = %lu\r\n",
               device_id,
               phy_port,
               pcl_id);
        return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalCreatePcl()
{
    typedef struct
    {
        UI32_T remapped_pkt_type;
        RULE_TYPE_INTERFACE_TYPE_E if_type;
        struct {
            UI32_T min;
            UI32_T max;
        } rule_count;
    } PACKET_TYPE_ENTRY_T;

    PACKET_TYPE_ENTRY_T remapped_pkt_type_tbl[] =
    {
#define PACKET_TYPE(pktType, ifType, remapAceType, quota) {pktType, ifType, {quota}},

        RULE_CONFIG_PACKET_TYPE_TABLE

#undef PACKET_TYPE
    };

    UI32_T i;

    /* Create global rule PCL first
     */
    for (i = 0; i < _countof(remapped_pkt_type_tbl); ++ i)
    {
        PACKET_TYPE_ENTRY_T *ent = &remapped_pkt_type_tbl[i];

        if (0 == ent->rule_count.max)
        {
            continue;
        }

        if (RULE_TYPE_INTERFACE_ALL_UNIT == ent->if_type ||
            RULE_TYPE_INTERFACE_UNIT == ent->if_type)
        {
            RULE_CTRL_LocalConfigGlobalRulePcl(ent->remapped_pkt_type);
        }
    }

    /* Create PCL except global rule
     */
    for (i = 0; i < _countof(remapped_pkt_type_tbl); ++ i)
    {
        PACKET_TYPE_ENTRY_T *ent = &remapped_pkt_type_tbl[i];

        if (0 == ent->rule_count.max)
        {
            continue;
        }

        if (RULE_TYPE_INTERFACE_CRAFT_PORT == ent->if_type)
        {
            RULE_CTRL_LocalConfigCraftPortRulePcl(ent->remapped_pkt_type);
        }
        else if (RULE_TYPE_INTERFACE_CPU_PORT == ent->if_type)
        {
            RULE_CTRL_LocalConfigCpuPortRulePcl(ent->remapped_pkt_type);
        }
        else if (RULE_TYPE_INTERFACE_TRUNK == ent->if_type)
        {
            RULE_CTRL_LocalConfigTrunkPortRulePcl(ent->remapped_pkt_type);
        }
        else if (RULE_TYPE_INTERFACE_UPORT == ent->if_type)
        {
            RULE_CTRL_LocalConfigUserPortRulePcl(ent->remapped_pkt_type);
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalInitialize
 *------------------------------------------------------------------------------
 * PURPOSE  : initialize local unit driver
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_LocalInitialize()
{
    UI32_T unit;
    UI32_T device_id;

    memset(shmem_data_p, 0, sizeof(*shmem_data_p));

    RULE_CTRL_UpdateDevInfo();

    shmem_data_p->global_tcam_key_mode = SYS_CPNT_TCAM_SEARCH_KEY_MODE;

    for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++ unit)
    {
        for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; ++ device_id)
        {
            shmem_data_p->stack_free_nbr_pce[unit-1][device_id] = RULE_CTRL_MAX_NBR_OF_PCE_PER_UNIT;
        }
    }

    RULE_CTRL_IN_Priv_InitInstance();

    /* Create class instance for ifentry
     */
    {
        UI32_T i;

        for (i = 0; i < _countof(shmem_data_p->if_table); ++i)
        {
            RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;
            RULE_TYPE_CLASS_INSTANCE_PTR_T if_inst_p;

            if_entry_p = &shmem_data_p->if_table[i];

            if_entry_p->class_inst_ptr = RULE_CTRL_IN_Priv_CreateClassInstance(RULE_TYPE_INST_ACL);
            ASSERT(RULE_TYPE_NIL != if_entry_p->class_inst_ptr.type);

            if_inst_p = RULE_CTRL_IN_Ptr2Instance(if_entry_p->class_inst_ptr);
            if_inst_p->id = 0xffff;
            if_inst_p->mvl_pkt_type = RULE_TYPE_PACKET_NIL;
        }
    }

    RULE_CTRL_LocalCreatePcl();
#if (SYS_CPNT_BPDU_LOOPBACK_RATELIMIT == TRUE)
    RULE_CTRL_SetGlobalBpdu();
#endif

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_GetPclConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : initialize local unit driver
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_GetPclConfig(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num,
    RULE_CTRL_PCL_CFG_T *pcl_cfg)
{
#define RULE_CTRL_MAX_PCL_ID        0x000003ff /* 10 bit */
#define RULE_CTRL_MAX_PCL_ID_MASK   RULE_CTRL_MAX_PCL_ID

    UI32_T bit0_5 = 0;
    UI32_T bit6   = 0;
    UI32_T bit7_9 = 0;

    ASSERT(NULL != interface_p);
    ASSERT(NULL != pcl_cfg);

    memset(pcl_cfg, 0, sizeof(*pcl_cfg));

    if (RULE_TYPE_INTERFACE_UPORT == interface_p->type ||
        RULE_TYPE_INTERFACE_TRUNK == interface_p->type ||
        RULE_TYPE_INTERFACE_UNIT  == interface_p->type ||
        RULE_TYPE_INTERFACE_ALL_UNIT == interface_p->type)
    {
        if (RULE_TYPE_INTERFACE_UPORT == interface_p->type)
        {
            UI32_T module_id, device_id, phy_port;

            if (FALSE == DEV_SWDRV_PMGR_Logical2PhyDevicePortID(interface_p->uport.unit,
                                                                interface_p->uport.port,
                                                                &module_id,
                                                                &device_id,
                                                                &phy_port))
            {
                ASSERT(0);
                return RULE_TYPE_INVALID_PARAMETER;
            }

            bit0_5 = phy_port + 1;

            pcl_cfg->pcl_id_mask = 0x000003ff;
        }
        else if (RULE_TYPE_INTERFACE_TRUNK == interface_p->type)
        {
            bit0_5 = interface_p->trunk_id;

            bit6   = 0x00000040;

            pcl_cfg->pcl_id_mask = 0x000003ff;
        }
        else if (RULE_TYPE_INTERFACE_UNIT == interface_p->type ||
                 RULE_TYPE_INTERFACE_ALL_UNIT == interface_p->type)
        {
            pcl_cfg->pcl_id_mask = 0x00000380;
        }
        else
        {
            ASSERT(0);
            return RULE_TYPE_INVALID_PARAMETER;
        }

        ASSERT(bit0_5 <= 0x0000003f);

        if (RULE_TYPE_INBOUND == interface_p->direction)
        {
            if (DEVRM_PCL_LOOKUP_0_0 == lookup_num)
            {
                bit7_9 = 0x00000080;
            }
            else if (DEVRM_PCL_LOOKUP_0_1 == lookup_num)
            {
                bit7_9 = 0x00000100;
            }
            else if (DEVRM_PCL_LOOKUP_1 == lookup_num)
            {
                bit7_9 = 0x00000180;
            }
            else
            {
                ASSERT(0);
                return RULE_TYPE_INVALID_PARAMETER;
            }
        }
        else if (RULE_TYPE_OUTBOUND == interface_p->direction)
        {
            if (DEVRM_PCL_LOOKUP_0_0 == lookup_num)
            {
                bit7_9 = 0x00000200;
            }
            else
            {
                ASSERT(0);
                return RULE_TYPE_INVALID_PARAMETER;
            }
        }

        pcl_cfg->pcl_id = (bit7_9 + bit6 + bit0_5) & pcl_cfg->pcl_id_mask;
    }
    else if (RULE_TYPE_INTERFACE_CRAFT_PORT == interface_p->type)
    {
        if (RULE_TYPE_INBOUND == interface_p->direction)
        {
            if (DEVRM_PCL_LOOKUP_0_0 == lookup_num)
            {
                pcl_cfg->pcl_id         = 0x00000381;
                pcl_cfg->pcl_id_mask    = 0x000003ff;
            }
            else if (DEVRM_PCL_LOOKUP_0_1 == lookup_num)
            {
                pcl_cfg->pcl_id         = 0x00000382;
                pcl_cfg->pcl_id_mask    = 0x000003ff;
            }
            else if (DEVRM_PCL_LOOKUP_1 == lookup_num)
            {
                pcl_cfg->pcl_id         = 0x00000383;
                pcl_cfg->pcl_id_mask    = 0x000003ff;
            }
            else
            {
                ASSERT(0);
                return RULE_TYPE_INVALID_PARAMETER;
            }
        }
        else if (RULE_TYPE_OUTBOUND == interface_p->direction)
        {
            if (DEVRM_PCL_LOOKUP_0_0 == lookup_num)
            {
                pcl_cfg->pcl_id         = 0x00000384;
                pcl_cfg->pcl_id_mask    = 0x000003ff;
            }
            else
            {
                ASSERT(0);
                return RULE_TYPE_INVALID_PARAMETER;
            }
        }
        else
        {
            ASSERT(0);
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else if (RULE_TYPE_INTERFACE_CPU_PORT == interface_p->type)
    {
        if (RULE_TYPE_INBOUND == interface_p->direction)
        {
            if (DEVRM_PCL_LOOKUP_0_0 == lookup_num)
            {
                pcl_cfg->pcl_id         = 0x00000385;
                pcl_cfg->pcl_id_mask    = 0x000003ff;
            }
            else if (DEVRM_PCL_LOOKUP_0_1 == lookup_num)
            {
                pcl_cfg->pcl_id         = 0x00000386;
                pcl_cfg->pcl_id_mask    = 0x000003ff;
            }
            else if (DEVRM_PCL_LOOKUP_1 == lookup_num)
            {
                pcl_cfg->pcl_id         = 0x00000387;
                pcl_cfg->pcl_id_mask    = 0x000003ff;
            }
            else
            {
                ASSERT(0);
                return RULE_TYPE_INVALID_PARAMETER;
            }
        }
        else if (RULE_TYPE_OUTBOUND == interface_p->direction)
        {
            if (DEVRM_PCL_LOOKUP_0_0 == lookup_num)
            {
                pcl_cfg->pcl_id         = 0x00000388;
                pcl_cfg->pcl_id_mask    = 0x000003ff;
            }
            else
            {
                ASSERT(0);
                return RULE_TYPE_INVALID_PARAMETER;
            }
        }
        else
        {
            ASSERT(0);
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }
    else
    {
        ASSERT(0);
    }

    memcpy(&pcl_cfg->interface, interface_p, sizeof(pcl_cfg->interface));
    pcl_cfg->lookup_num = lookup_num;

    ASSERT(pcl_cfg->pcl_id <= RULE_CTRL_MAX_PCL_ID);
    ASSERT(pcl_cfg->pcl_id_mask <= RULE_CTRL_MAX_PCL_ID_MASK);

    return RULE_TYPE_OK;

#undef RULE_CTRL_MAX_PCL_ID
#undef RULE_CTRL_MAX_PCL_ID_MASK
}

// ???: Remove this function ?!
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_ConvertPacketTypeToPclConfig(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T remapped_pkt_type,
    RULE_CTRL_PCL_CFG_T *pcl_cfg_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    DEVRM_PCL_LOOKUP_NUMBER_T   lookup_num;

    RULE_TYPE_INTERFACE_INFO_T interface;

    ASSERT(NULL != interface_p);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction ||
           RULE_TYPE_OUTBOUND == interface_p->direction||
           RULE_TYPE_UNSPEC_DIRECTION == interface_p->direction);

    /** interface type of remapped_pkt_type should consistent with interface
     */
    {
        RULE_TYPE_INTERFACE_TYPE_E packet_type_to_if_type_tbl[] =
        {
#define PACKET_TYPE(pktType, ifType, remapAceType, quota)  ifType,

            RULE_CONFIG_PACKET_TYPE_TABLE

#undef PACKET_TYPE
        };

        packet_type_to_if_type_tbl[0] = packet_type_to_if_type_tbl[0];
        ASSERT(packet_type_to_if_type_tbl[remapped_pkt_type] == interface_p->type);
    }

    memcpy(&interface, interface_p, sizeof(interface));

    result = RULE_CTRL_Priv_ConvertPacketTypeToToDirectionLookupNum(remapped_pkt_type,
                                                                    &interface.direction,
                                                                    &lookup_num);
    ASSERT(RULE_TYPE_OK == result);

    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    if (RULE_TYPE_UNSPEC_DIRECTION != interface_p->direction)
    {
        ASSERT(0 == memcmp(interface_p, &interface, sizeof(*interface_p)));
    }

    ASSERT(RULE_TYPE_INBOUND == interface.direction ||
           RULE_TYPE_OUTBOUND == interface.direction);

    result = RULE_CTRL_Priv_GetPclConfig(&interface, lookup_num, pcl_cfg_p);

    ASSERT(0 == memcmp(&interface, &pcl_cfg_p->interface, sizeof(interface)));

    ASSERT(RULE_TYPE_OK == result);

    return result;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_ConvertPacketTypeToAceType(
    UI32_T remapped_pkt_type,
    RULE_TYPE_INTERFACE_TYPE_E *interface_type_p)
{
    typedef struct
    {
        UI32_T if_type;
    } REMAP_ACE_TYPE_ENTRY_T;

    REMAP_ACE_TYPE_ENTRY_T packet_to_ace_tbl[] =
    {
#define PACKET_TYPE(pktType, ifType, remapAceType, quota)  {ifType},

        RULE_CONFIG_PACKET_TYPE_TABLE

#undef PACKET_TYPE
    };

    ASSERT(_countof(packet_to_ace_tbl) == RULE_TYPE_PACKET_MAX_TYPE);

    if (RULE_TYPE_PACKET_MAX_TYPE <= remapped_pkt_type)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (interface_type_p)
    {
        *interface_type_p = packet_to_ace_tbl[remapped_pkt_type].if_type;
    }

    return RULE_TYPE_OK;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_ConvertPacketTypeToToDirectionLookupNum(
    UI32_T remapped_pkt_type,
    RULE_TYPE_InOutDirection_T *direction_p,
    DEVRM_PCL_LOOKUP_NUMBER_T *lookup_num_p)
{
    ASSERT(remapped_pkt_type < RULE_TYPE_PACKET_MAX_TYPE);
    ASSERT(RULE_TYPE_PACKET_INGRESS_0_0_BEGIN != remapped_pkt_type &&
           RULE_TYPE_PACKET_INGRESS_0_1_BEGIN != remapped_pkt_type &&
           RULE_TYPE_PACKET_INGRESS_1_BEGIN   != remapped_pkt_type &&
           RULE_TYPE_PACKET_EGRESS_BEGIN      != remapped_pkt_type);
    ASSERT(NULL != direction_p);
    ASSERT(NULL != lookup_num_p);

    *direction_p = (RULE_TYPE_PACKET_EGRESS_BEGIN < remapped_pkt_type) ? RULE_TYPE_OUTBOUND :
                                                                         RULE_TYPE_INBOUND;

    if (RULE_TYPE_PACKET_INGRESS_0_0_BEGIN < remapped_pkt_type && remapped_pkt_type < RULE_TYPE_PACKET_INGRESS_0_1_BEGIN)
    {
        *lookup_num_p = DEVRM_PCL_LOOKUP_0_0;
    }
    else if (RULE_TYPE_PACKET_INGRESS_0_1_BEGIN < remapped_pkt_type && remapped_pkt_type < RULE_TYPE_PACKET_INGRESS_1_BEGIN)
    {
        *lookup_num_p = DEVRM_PCL_LOOKUP_0_1;
    }
    else if (RULE_TYPE_PACKET_INGRESS_1_BEGIN < remapped_pkt_type && remapped_pkt_type < RULE_TYPE_PACKET_EGRESS_BEGIN)
    {
        *lookup_num_p = DEVRM_PCL_LOOKUP_1;
    }
    else if (RULE_TYPE_PACKET_EGRESS_BEGIN < remapped_pkt_type)
    {
        *lookup_num_p = DEVRM_PCL_LOOKUP_0_0;
    }
    else
    {
        ASSERT(0);
        return RULE_TYPE_INVALID_PARAMETER;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * Function : RULE_CTRL_Priv_ConvertPacketTypeToVirtualGroupId
 *------------------------------------------------------------------------------
 * Purpose  : Get virtual group ID and rule priority
 * INPUT    : unit          - unit number
 *            device_id     - device ID
 *            pcl_id        - PCL ID
 *            dev_rule_id   - device rule ID
 * OUTPUT   : vir_group_id  - virtual group ID
 *            rule_pri      - rule priority
 * RETURN   : TRUE/FALSE
 * NOTE     : The mapping table for virtual group ID.
 *            Virtual group ID = max means highest priority.
 *            Virtual group ID = 0 means lowest priority
 *
 *            Type                to  Virtual Group ID
 *            ------------------  --  ----------------
 *            RULE_TYPE_GLOBAL 1      max
 *            RULE_TYPE_GLOBAL 2      max-1
 *            ...
 *            RULE_TYPE_GLOBAL N
 *            RULE_TYPE_DFLT 1        ...
 *            RULE_TYPE_DFLT 2        ...
 *            ...
 *            RULE_TYPE_DFLT N        0
 *
 *            FIXME: Correct this function header
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_ConvertPacketTypeToVirtualGroupId(
    UI32_T remapped_pkt_type,
    UI32_T *vir_group_id)
{
    ASSERT(NULL != vir_group_id);

    *vir_group_id = (RULE_TYPE_PACKET_MAX_TYPE - remapped_pkt_type);

    return RULE_TYPE_OK;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddRule_To_Dev(
    UI32_T unit,
    UI32_T device_id,
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T remapped_pkt_type,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *class_inst_obj_p,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p,
    BOOL_T is_active,
    int priority)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_AclType_T acl_key_mode = RULE_TYPE_IP_EXT_ACL;

    UI32_T rule_index;
    UI32_T vir_group_id;

    RULE_CTRL_PCL_CFG_T pcl_cfg;

    RULE_TYPE_RULE_INSTANCE_OBJECT_T rule_inst_obj;

    result = RULE_CTRL_Priv_ConvertPacketTypeToPclConfig(interface_p,
                                                         remapped_pkt_type,
                                                         &pcl_cfg);
    ASSERT(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_ConvertPacketTypeToVirtualGroupId(remapped_pkt_type,
                                                              &vir_group_id);
    ASSERT(RULE_TYPE_OK == result);

    result = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &rule_inst_obj);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Failed to create rule instance");
        goto undo;
    }

    rule_inst_obj.inst->mvl_rule_index = RULE_CTRL_IN_IndexOfRuleInstance(rule_inst_obj.inst);
    rule_inst_obj.inst->dev_rule_info.rule_id = 0;
    rule_inst_obj.inst->active = FALSE;

    class_inst_obj_p->add_rule_obj(class_inst_obj_p, &rule_inst_obj);

    rule_index = RULE_CTRL_LocalAllocateLogicalRuleID(unit, device_id, acl_key_mode);
    if (0 == rule_index)
    {
        LOG("<Error> Failed to allocate logical rule ID");
        result = RULE_TYPE_INSUFFICIENT_RESOURCE;
        goto undo;
    }

    rule_inst_obj.inst->dev_rule_info.unit = unit;
    rule_inst_obj.inst->dev_rule_info.device = device_id;
    rule_inst_obj.inst->dev_rule_info.pcl_id = pcl_cfg.pcl_id;
    rule_inst_obj.inst->dev_rule_info.pcl_id_mask = pcl_cfg.pcl_id_mask;
    rule_inst_obj.inst->dev_rule_info.virtual_group_id = vir_group_id;
    rule_inst_obj.inst->dev_rule_info.priority = priority;
    rule_inst_obj.inst->dev_rule_info.rule_id = rule_index;
    rule_inst_obj.inst->dev_rule_info.lookup_num = pcl_cfg.lookup_num;

    memcpy(&rule_inst_obj.inst->dev_rule_info.interface,
           interface_p,
           sizeof(rule_inst_obj.inst->dev_rule_info.interface));

    if (TRUE == is_active)
    {
        result = RULE_CTRL_LocalAllocateAndSetRule(rule_inst_obj.inst->dev_rule_info.unit,
                                                   rule_inst_obj.inst->dev_rule_info.device,
                                                   &pcl_cfg,
                                                   rule_inst_obj.inst->dev_rule_info.virtual_group_id,
                                                   rule_inst_obj.inst->dev_rule_info.priority,
                                                   rule_inst_obj.inst->dev_rule_info.rule_id,
                                                   rule_pattern_p,
                                                   dev_action_p);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Failed to allocate and set rule on dev");
            goto undo;
        }

        rule_inst_obj.inst->active = TRUE;
    }

    /**
     * Saving rule pattern into rule table
     */
    ASSERT(sizeof(shmem_data_p->rule_table[0].rule_pattern) == sizeof(*rule_pattern_p));
    memcpy(&shmem_data_p->rule_table[rule_inst_obj.inst->mvl_rule_index].rule_pattern,
           rule_pattern_p, sizeof(*rule_pattern_p));

    ASSERT(sizeof(shmem_data_p->rule_table[0].action) == sizeof(*dev_action_p));
    memcpy(&shmem_data_p->rule_table[rule_inst_obj.inst->mvl_rule_index].action,
           dev_action_p, sizeof(*dev_action_p));

    return RULE_TYPE_OK;

undo:
    return result;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddQosRule_To_Dev(
    UI32_T unit,
    UI32_T device_id,
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T remapped_pkt_type,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *class_map_inst_obj_p,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *ace_inst_obj_p,
    UI32_T ace_index,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p,
    UI32_T meter_id,
    const RULE_TYPE_TBParamEntry_T *meter_entry_p,
    UI32_T action_id,
    const RULE_TYPE_Action_T *action_entry_p,
    int priority)
{
    //
    // --== Duplicated Code Start (RULE_CTRL_LocalAddRule_To_Dev)
    //
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_AclType_T acl_key_mode = RULE_TYPE_IP_EXT_ACL;

    UI32_T rule_index;
    UI32_T vir_group_id;

    RULE_CTRL_PCL_CFG_T pcl_cfg;

    RULE_TYPE_RULE_INSTANCE_OBJECT_T rule_inst_obj;
    //
    // --== Duplicated Code End
    //

    RULE_TYPE_AceAction_T ace_access;

    UI32_T src_dev_rule_id;
    BOOL_T share_meter;

    //
    // --== Duplicated Code Start (RULE_CTRL_LocalAddRule_To_Dev)
    //
    result = RULE_CTRL_Priv_ConvertPacketTypeToPclConfig(interface_p,
                                                         remapped_pkt_type,
                                                         &pcl_cfg);
    ASSERT(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_ConvertPacketTypeToVirtualGroupId(remapped_pkt_type,
                                                              &vir_group_id);
    ASSERT(RULE_TYPE_OK == result);

    result = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &rule_inst_obj);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Failed to create rule instance");
        goto undo;
    }

    rule_inst_obj.inst->mvl_rule_index = RULE_CTRL_IN_IndexOfRuleInstance(rule_inst_obj.inst);
    rule_inst_obj.inst->dev_rule_info.rule_id = 0;
    rule_inst_obj.inst->active = FALSE;

    ace_inst_obj_p->add_rule_obj(ace_inst_obj_p, &rule_inst_obj);

    rule_index = RULE_CTRL_LocalAllocateLogicalRuleID(unit, device_id, acl_key_mode);
    if (0 == rule_index)
    {
        LOG("<Error> Failed to allocate logical rule ID");
        result = RULE_TYPE_INSUFFICIENT_RESOURCE;
        goto undo;
    }

    rule_inst_obj.inst->dev_rule_info.unit = unit;
    rule_inst_obj.inst->dev_rule_info.device = device_id;
    rule_inst_obj.inst->dev_rule_info.pcl_id = pcl_cfg.pcl_id;
    rule_inst_obj.inst->dev_rule_info.pcl_id_mask = pcl_cfg.pcl_id_mask;
    rule_inst_obj.inst->dev_rule_info.virtual_group_id = vir_group_id;
    rule_inst_obj.inst->dev_rule_info.priority = priority;
    rule_inst_obj.inst->dev_rule_info.rule_id = rule_index;
    rule_inst_obj.inst->dev_rule_info.lookup_num = pcl_cfg.lookup_num;

    memcpy(&rule_inst_obj.inst->dev_rule_info.interface,
           interface_p,
           sizeof(rule_inst_obj.inst->dev_rule_info.interface));
    //
    // --== Duplicated Code End
    //

    /** Covert action to dev for Qos
     */

    share_meter = FALSE;
    src_dev_rule_id = RULE_CTRL_NO_SHARE_METER_ENTRY_ID;

    if ((NULL != meter_entry_p) 
#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)
	 || (interface_p->type == RULE_TYPE_INTERFACE_CPU_PORT)   /*cpu port always enable counter*/
#endif
       )
    {
        src_dev_rule_id = RULE_CTRL_LocalGetFirstDevRuleIDofOneClassMap(class_map_inst_obj_p->inst,
                                                                        rule_inst_obj.inst->dev_rule_info.unit,
                                                                        rule_inst_obj.inst->dev_rule_info.device);

        if (src_dev_rule_id != RULE_CTRL_NO_SHARE_METER_ENTRY_ID)
        {
            /* Enable sharing meter only when source rule doesn't current new created.
             */
            if (src_dev_rule_id != rule_inst_obj.inst->dev_rule_info.rule_id)
            {
                share_meter = TRUE;
            }
        }
    }

    ace_access = ace_entry_p->access;

    ASSERT(RULE_TYPE_ACE_PERMIT == ace_access ||
           RULE_TYPE_ACE_DENY == ace_access);

    {
        DEVRM_AceEntry_T rule_pattern;
        DEVRM_ACTION_T dev_action;

        DEVRM_LIB_RulePatternInit(&rule_pattern);

        result = RULE_CTRL_LocalConvertAce2Dev(ace_entry_p, &rule_pattern);
        ASSERT(RULE_TYPE_OK == result);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Failed to convert rule pattern");
            goto undo;
        }

        DEVRM_LIB_ActionInit(&dev_action);

        if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
        {
            LOG("<Error> Set packet command");
            goto undo;
        }

        if (NULL != meter_entry_p && FALSE == share_meter)
        {
            result = RULE_CTRL_LocalConvertMeter2Dev(meter_entry_p,
                                                     &dev_action);
            ASSERT(RULE_TYPE_OK == result);
            if (RULE_TYPE_OK != result)
            {
                LOG("<Error> Failed to convert meter");
                goto undo;
            }
        }

#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)
        if (FALSE == share_meter)
        {
            if (interface_p->type == RULE_TYPE_INTERFACE_CPU_PORT)  /*cpu port always enable counter*/
            {
                LOG("Counter Enable");

                if ((dev_action.policer.mode == DEVRM_POLICER_MODE_DISABLE) ||
                    (dev_action.policer.mode == DEVRM_POLICER_MODE_COUNTER_ONLY))
                {
                    if (TRUE != DEVRM_LIB_ActionMeterConfigFlow(&dev_action, 0, 0))
                    {
                        LOG("<Error> config meter (flow)");
                        return RULE_TYPE_FAIL;
                    }
                }
		
                if (TRUE != DEVRM_LIB_ActionCounterConfig(&dev_action, DEVRM_POLICER_COUNTER_MODE_PACKET))
                {
                    LOG("<Error> Modify counter");
                    return RULE_TYPE_FAIL;
                }
            }
            else
            {
                if (TRUE != DEVRM_LIB_ActionCounterConfig(&dev_action, DEVRM_POLICER_COUNTER_MODE_BYTE))
                {
                    LOG("<Error> config counter");
                    return FALSE;
                }
            }
        }
#endif /*#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)*/

        result = RULE_CTRL_LocalConvertAction2DEV(action_entry_p, &dev_action);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Failed to convert action");
            goto undo;
        }

        //
        // --== Duplicated Code Start (RULE_CTRL_LocalAddRule_To_Dev)
        //
        result = RULE_CTRL_LocalAllocateAndSetRule(rule_inst_obj.inst->dev_rule_info.unit,
                                                   rule_inst_obj.inst->dev_rule_info.device,
                                                   &pcl_cfg,
                                                   rule_inst_obj.inst->dev_rule_info.virtual_group_id,
                                                   rule_inst_obj.inst->dev_rule_info.priority,
                                                   rule_inst_obj.inst->dev_rule_info.rule_id,
                                                   &rule_pattern,
                                                   &dev_action);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Failed to allocate and set rule on dev");
            goto undo;
        }
        //
        // --== Duplicated Code End
        //

        rule_inst_obj.inst->active = TRUE;

        /**
         * Saving rule pattern into rule table
         */
        ASSERT(sizeof(shmem_data_p->rule_table[0].rule_pattern) == sizeof(rule_pattern));
        memcpy(&shmem_data_p->rule_table[rule_inst_obj.inst->mvl_rule_index].rule_pattern,
               &rule_pattern, sizeof(rule_pattern));

        ASSERT(sizeof(shmem_data_p->rule_table[0].action) == sizeof(dev_action));
        memcpy(&shmem_data_p->rule_table[rule_inst_obj.inst->mvl_rule_index].action,
               &dev_action, sizeof(dev_action));

        if (TRUE == share_meter)
        {
            if (FALSE == RULE_CTRL_LocalShareMeter(rule_inst_obj.inst->dev_rule_info.unit,
                                                   1,
                                                   rule_inst_obj.inst->dev_rule_info.device,
                                                   src_dev_rule_id,
                                                   rule_inst_obj.inst->dev_rule_info.rule_id))
            {
                // TODO: test for fail on here !!
                LOG("<Error> Failed to share meter between rules");
                goto undo;
            }
        }
    }

    rule_inst_obj.inst->meter_id = meter_id;
    rule_inst_obj.inst->action_id = action_id;

    return RULE_TYPE_OK;

undo:
    return result;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddRule_Hello2(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    RULE_CONFIG_MVL_PACKET_TYPE_T remapped_pkt_type,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *group_class_inst_obj_p,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p,
    BOOL_T is_active, UI32_T id, int priority)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T class_inst_obj;

    result = RULE_CTRL_Priv_CheckRuleQuota(interface_p, remapped_pkt_type, 1);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACE,
                                                         &class_inst_obj);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Failed to create class instance of rule entry");
        goto undo;
    }

    class_inst_obj.inst->id = id;
    class_inst_obj.inst->mvl_pkt_type = remapped_pkt_type;

    group_class_inst_obj_p->add_class_obj(group_class_inst_obj_p, &class_inst_obj);

    result = RULE_CTRL_EACH_UNIT_DEVICE(interface_p,
                                        RULE_CTRL_LocalAddRule_To_Dev,
                                        interface_p,
                                        remapped_pkt_type,
                                        &class_inst_obj,
                                        rule_pattern_p,
                                        dev_action_p,
                                        is_active,
                                        priority);

    if (RULE_TYPE_OK != result)
    {
        goto undo;
    }

    return RULE_TYPE_OK;

undo:
    ASSERT(RULE_TYPE_OK != result);
    return result;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddCpuIfRule(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T remapped_pkt_type,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p,
    BOOL_T is_active)
{
    RULE_TYPE_INSTANCE_TYPE_T class_inst_type = RULE_TYPE_INST_CPU_IF;

    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T agg_class_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T agg_class_inst_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T new_agg_class_inst_p;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;

    result = RULE_CTRL_Priv_CheckRuleQuota(interface_p, remapped_pkt_type, 1);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    //
    // --== Duplicated code start
    //
    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    memset(&param, 0, sizeof(param));

    param.type = class_inst_type;
    param.id = remapped_pkt_type;

    agg_class_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                      RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                      &param);

    if (NULL == agg_class_inst_p)
    {
        /**
         * Create aggregate class instance
         */
        result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(class_inst_type,
                                                             &agg_class_inst_obj);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Failed to create aggregate class instance");
            goto undo;
        }

        new_agg_class_inst_p = agg_class_inst_obj.inst;

        agg_class_inst_p = agg_class_inst_obj.inst;

        agg_class_inst_obj.inst->id = remapped_pkt_type;
        agg_class_inst_obj.inst->mvl_pkt_type = remapped_pkt_type;

        if_class_obj.add_class_obj(&if_class_obj, &agg_class_inst_obj);
    }
    else
    {
        new_agg_class_inst_p = NULL;

        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(agg_class_inst_p, &agg_class_inst_obj);
    }
    //
    // --== End of duplicated code
    //

    result = RULE_CTRL_LocalAddRule_Hello2(interface_p, remapped_pkt_type,
                                           &agg_class_inst_obj,
                                           rule_pattern_p,
                                           dev_action_p,
                                           is_active, 0xffff, 0xffff);
    if (RULE_TYPE_OK != result)
    {
        goto undo;
    }

    return RULE_TYPE_OK;

undo:

    {
        RULE_TYPE_RETURN_TYPE_T temp_result;

        temp_result = RULE_CTRL_LocalDelCpuIfRule(interface_p, remapped_pkt_type, rule_pattern_p);
        ASSERT(RULE_TYPE_OK == result);
    }

    ASSERT(RULE_TYPE_OK != result);
    return result;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalDelCpuIfRule(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T remapped_pkt_type,
    const DEVRM_AceEntry_T *rule_pattern_p)
{
    RULE_TYPE_INSTANCE_TYPE_T class_inst_type = RULE_TYPE_INST_CPU_IF;

    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T agg_class_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T agg_class_inst_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T class_inst_obj;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;

    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    memset(&param, 0, sizeof(param));

    param.type = class_inst_type;
    param.id = remapped_pkt_type;

    agg_class_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                      RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                      &param);

    if (NULL == agg_class_inst_p)
    {
        return RULE_TYPE_OK;
    }

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(agg_class_inst_p,
                                                   &agg_class_inst_obj);

    class_inst_p = RULE_CTRL_Priv_GetClassInstanceOfRuleEntry(&agg_class_inst_obj,
                                                              rule_pattern_p);
    if (NULL != class_inst_p)
    {
        result = RULE_CTRL_LocalFreeRulesByRuleGroup(class_inst_p);
        ASSERT(RULE_TYPE_OK == result);

        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_inst_p, &class_inst_obj);
        agg_class_inst_obj.remove_class_obj(&agg_class_inst_obj, &class_inst_obj);
        RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&class_inst_obj);

        class_inst_p = NULL;
        class_inst_obj.inst = NULL;
    }

    //
    // Remove aggreated class instance if not any rule in it
    //
    {
        RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T agg_class_inst_iter;

        agg_class_inst_obj.get_element_iterator(&agg_class_inst_obj, &agg_class_inst_iter);

        if (0 == agg_class_inst_iter.count(&agg_class_inst_iter))
        {
            if_class_obj.remove_class_obj(&if_class_obj, &agg_class_inst_obj);
            RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&agg_class_inst_obj);

            agg_class_inst_p = NULL;
            agg_class_inst_obj.inst = NULL;
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_LocalSetRuleForPacketTypeByGlobal
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs of specified packet_type for all unit/ports
 *   INPUT : remapped_pkt_type, do_action, ace_p
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : 1, pce is not shared
 *           2, update pce with new action
 *           3, pk_type is RULE_TYPE_PACKET_CLUSTER, etc...
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetRuleForPacketTypeByGlobal(
    RULE_CTRL_OPT_FLAGS_T flags,
    UI32_T remapped_pkt_type,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p,
    const RULE_CTRL_OPTIONS_T *options_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_INTERFACE_INFO_T interface;

    RULE_CTRL_OPERATION_T   optbl = {0};

    memset(&interface, 0, sizeof(interface));

    ASSERT(remapped_pkt_type < RULE_TYPE_PACKET_MAX_TYPE);

    /* Get interface type
     */
    {
        RULE_CTRL_Priv_ConvertPacketTypeToAceType(remapped_pkt_type, &interface.type);

        ASSERT(RULE_TYPE_INTERFACE_ALL_UNIT   == interface.type ||
               RULE_TYPE_INTERFACE_CRAFT_PORT == interface.type ||
               RULE_TYPE_INTERFACE_CPU_PORT == interface.type);
    }

    /* Get direction
     */
    {
        DEVRM_PCL_LOOKUP_NUMBER_T lookup_num;

        result = RULE_CTRL_Priv_ConvertPacketTypeToToDirectionLookupNum(remapped_pkt_type, &interface.direction, &lookup_num);

        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Invalid remappedPktType=%s(%lu)",
                RULE_CTRL_REMAPPED_PACKET_TYPE_STR(remapped_pkt_type),
                remapped_pkt_type);
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    if (RULE_TYPE_INTERFACE_ALL_UNIT != interface.type &&
        RULE_TYPE_INTERFACE_CRAFT_PORT != interface.type &&
        RULE_TYPE_INTERFACE_CPU_PORT != interface.type)
    {
        LOG("<Internal Error> Invalid interface type = %d", interface.type);
        return RULE_TYPE_INVALID_PARAMETER;
    }

    optbl.add_rule = RULE_CTRL_LocalAddCpuIfRule;
    optbl.del_rule = RULE_CTRL_LocalDelCpuIfRule;

    return RULE_CTRL_Priv_SetRuleForPacketType_Core(flags, &interface,
                remapped_pkt_type, rule_pattern_p, /*action_p,*/ dev_action_p, options_p, &optbl);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_LocalSetRuleForPacketTypeByUport
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs of specified packet_type
 *             for specified unit/port.
 *   INPUT : unit, port, remapped_pkt_type, do_action, ace_p
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : 1, pce is not shared
 *           2, update pce with new action
 *           3, remapped_pkt_type is RULE_TYPE_PACKET_CLUSTER, etc...
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetRuleForPacketTypeByUport(
    RULE_CTRL_OPT_FLAGS_T flags,
    UI32_T unit,
    UI32_T port,
    UI32_T remapped_pkt_type,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_INTERFACE_INFO_T interface;

    RULE_CTRL_OPERATION_T   optbl = {0};

    memset(&interface, 0, sizeof(interface));

    ASSERT(remapped_pkt_type < RULE_TYPE_PACKET_MAX_TYPE);

    interface.type = RULE_TYPE_INTERFACE_UPORT;
    interface.uport.unit = unit;
    interface.uport.port = port;

    {
        DEVRM_PCL_LOOKUP_NUMBER_T lookup_num;

        result = RULE_CTRL_Priv_ConvertPacketTypeToToDirectionLookupNum(remapped_pkt_type, &interface.direction, &lookup_num);

        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Invalid remappedPktType=%s(%lu)",
                RULE_CTRL_REMAPPED_PACKET_TYPE_STR(remapped_pkt_type),
                remapped_pkt_type);
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    optbl.add_rule = RULE_CTRL_LocalAddCpuIfRule;
    optbl.del_rule = RULE_CTRL_LocalDelCpuIfRule;

    return  RULE_CTRL_Priv_SetRuleForPacketType_Core(flags, &interface,
                remapped_pkt_type, rule_pattern_p, /*action_p,*/ dev_action_p, NULL, &optbl);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_LocalSetRuleForPacketTypeByTrunk
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs of specified packet_type
 *             for specified unit/port.
 *   INPUT : unit, port, remapped_pkt_type, do_action, ace_p
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : 1, pce is not shared
 *           2, update pce with new action
 *           3, remapped_pkt_type is RULE_TYPE_PACKET_CLUSTER, etc...
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetRuleForPacketTypeByTrunk(
    RULE_CTRL_OPT_FLAGS_T flags,
    UI32_T trunk_id,
    UI32_T remapped_pkt_type,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p)
{
    RULE_TYPE_RETURN_TYPE_T    result;
    RULE_TYPE_INTERFACE_INFO_T interface;
    RULE_CTRL_OPERATION_T      optbl = {0};

    ASSERT(remapped_pkt_type < RULE_TYPE_PACKET_MAX_TYPE);

    memset(&interface, 0, sizeof(interface));
    interface.type = RULE_TYPE_INTERFACE_TRUNK;
    interface.trunk_id = trunk_id;

    {
        DEVRM_PCL_LOOKUP_NUMBER_T lookup_num;

        result = RULE_CTRL_Priv_ConvertPacketTypeToToDirectionLookupNum(remapped_pkt_type, &interface.direction, &lookup_num);

        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Invalid remappedPktType=%s(%lu)",
                RULE_CTRL_REMAPPED_PACKET_TYPE_STR(remapped_pkt_type),
                remapped_pkt_type);
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    optbl.add_rule = RULE_CTRL_LocalAddCpuIfRule;
    optbl.del_rule = RULE_CTRL_LocalDelCpuIfRule;

    return  RULE_CTRL_Priv_SetRuleForPacketType_Core(flags, &interface,
                remapped_pkt_type, rule_pattern_p, dev_action_p, NULL, &optbl);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_LocalSetRuleForPacketTypeByTrunkPort
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs of specified packet_type
 *             for specified trunk port.
 *   INPUT : port, remapped_pkt_type, do_action, ace_p
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : 1, pce is not shared
 *           2, update pce with new action
 *           3, remapped_pkt_type is RULE_TYPE_PACKET_CLUSTER, etc...
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetRuleForPacketTypeByTrunkPort(
    RULE_CTRL_OPT_FLAGS_T flags,
    UI32_T port,
    UI32_T remapped_pkt_type,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_INTERFACE_INFO_T interface;

    RULE_CTRL_OPERATION_T   optbl = {0};

    ASSERT(remapped_pkt_type < RULE_TYPE_PACKET_MAX_TYPE);

    interface.type = RULE_TYPE_INTERFACE_TRUNK;
    interface.trunk_id = STKTPLG_OM_IFINDEX_TO_TRUNKID(port);

    {
        DEVRM_PCL_LOOKUP_NUMBER_T lookup_num;

        result = RULE_CTRL_Priv_ConvertPacketTypeToToDirectionLookupNum(remapped_pkt_type, &interface.direction, &lookup_num);

        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Invalid remappedPktType=%s(%lu)",
                RULE_CTRL_REMAPPED_PACKET_TYPE_STR(remapped_pkt_type),
                remapped_pkt_type);
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    optbl.add_rule = RULE_CTRL_LocalAddCpuIfRule;
    optbl.del_rule = RULE_CTRL_LocalDelCpuIfRule;

    return  RULE_CTRL_Priv_SetRuleForPacketType_Core(flags, &interface,
                remapped_pkt_type, rule_pattern_p, /*action_p,*/ dev_action_p, NULL, &optbl);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_Priv_SetRuleForPacketType_Core
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs of specified packet_type
 *             for specified unit/port.
 *   INPUT : unit, port, remapped_pkt_type, do_action, ace_p
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : 1, pce is not shared
 *           2, update pce with new action
 *           3, remapped_pkt_type is RULE_TYPE_PACKET_CLUSTER, etc...
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_SetRuleForPacketType_Core(
    RULE_CTRL_OPT_FLAGS_T flags,
    RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T remapped_pkt_type,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p,
    const RULE_CTRL_OPTIONS_T *options_p,
    const RULE_CTRL_OPERATION_T *optbl)
{
    RULE_TYPE_RETURN_TYPE_T ret;

    DEVRM_AceEntry_T new_rule_pattern;
    DEVRM_ACTION_T new_dev_action;

    ASSERT(NULL != interface_p);
    ASSERT(RULE_TYPE_INTERFACE_ALL_UNIT == interface_p->type ||
           RULE_TYPE_INTERFACE_CRAFT_PORT == interface_p->type ||
           RULE_TYPE_INTERFACE_UPORT == interface_p->type ||
           RULE_TYPE_INTERFACE_CPU_PORT == interface_p->type ||
           RULE_TYPE_INTERFACE_TRUNK == interface_p->type);

    ASSERT(remapped_pkt_type < RULE_TYPE_PACKET_MAX_TYPE);

    ASSERT(NULL != optbl);

    LOG_IF(*interface_p, "remappedPktType=%s(%lu)",
           RULE_CTRL_REMAPPED_PACKET_TYPE_STR(remapped_pkt_type),
           remapped_pkt_type);

    // FIXME: Don't do this convert action here.
    //        It should be put in trapXXXtoCPU which just provide
    //        packet type
    ret = RULE_CTRL_CnvPacketTypeAndAction2Ace(remapped_pkt_type, options_p,
                                               &new_rule_pattern, &new_dev_action);
    if (RULE_TYPE_OK == ret)
    {
        if (NULL == rule_pattern_p)
        {
            rule_pattern_p = &new_rule_pattern;

            /* Overwrite the default action
             */
            if (NULL == dev_action_p)
            {
                ASSERT(DEVRM_PKT_CMD_UNSPECIFIC != new_dev_action.pkt_cmd);
            }
        }

        if (DEVRM_PKT_CMD_UNSPECIFIC != new_dev_action.pkt_cmd)
        {
            dev_action_p = &new_dev_action;
        }
    }

    ASSERT(NULL != rule_pattern_p);

    if (1 == flags.INSTALL)
    {
        ASSERT(NULL != dev_action_p && DEVRM_PKT_CMD_UNSPECIFIC != dev_action_p->pkt_cmd);

        if (1 == flags.FORCE_INSTALL)
        {
            ASSERT(NULL != optbl->del_rule);
            ret = optbl->del_rule(interface_p, remapped_pkt_type, rule_pattern_p);
            if (RULE_TYPE_OK != ret)
            {
                return ret;
            }
        }

        ASSERT(NULL != optbl->add_rule);
        ret = optbl->add_rule(interface_p, remapped_pkt_type, rule_pattern_p,
                                                            dev_action_p, TRUE);
        if (RULE_TYPE_OK != ret)
        {
            return ret;
        }
    }
    else
    {
        ASSERT(NULL != optbl->del_rule);
        ret = optbl->del_rule(interface_p, remapped_pkt_type, rule_pattern_p);
        if (RULE_TYPE_OK != ret)
        {
            return ret;
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_CnvPacketTypeAndAction2Ace
 *------------------------------------------------------------------------------
 * PURPOSE : Convert packet_type & do_action to ace_entry
 *   INPUT : remapped_pkt_type, do_action, ace_p
 *  OUTPUT : None
 *  RETURN : RULE_TYPE_OK / RULE_TYPE_FAIL
 *    NOTE : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_CnvPacketTypeAndAction2Ace(
    UI32_T remapped_pkt_type,
    const RULE_CTRL_OPTIONS_T *options_p,
    DEVRM_AceEntry_T *rule_pattern_p,
    DEVRM_ACTION_T *dev_action_p)
{
    enum
    {
        RULE_CTRL_TCP_FLAG_SYN  = BIT_1,
        RULE_CTRL_TCP_FLAG_URG  = BIT_5,
    };

    ASSERT(NULL != rule_pattern_p);
    ASSERT(NULL != dev_action_p);

    DEVRM_LIB_RulePatternInit(rule_pattern_p);
    DEVRM_LIB_ActionInit(dev_action_p);

    /* From RULE_CTRL_TrapPacket2Cpu. Not sure it can
     * satifsy all cases.
     */
    if (NULL != options_p && NULL != options_p->cpu_rule_info_p)
    {
        if (TRUE == options_p->cpu_rule_info_p->common.to_cpu &&
            TRUE == options_p->cpu_rule_info_p->common.flood)
        {
            if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_COPY_TO_CPU))
            {
                LOG("<Error> Set packet command");
                return RULE_TYPE_FAIL;
            }
        }
        else if (TRUE == options_p->cpu_rule_info_p->common.to_cpu &&
                 FALSE == options_p->cpu_rule_info_p->common.flood)
        {
            if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
            {
                LOG("<Error> Set packet command");
                return RULE_TYPE_FAIL;
            }
        }
        else if (FALSE == options_p->cpu_rule_info_p->common.to_cpu &&
                 TRUE == options_p->cpu_rule_info_p->common.flood)
        {
            /* ... */
        }
        else if (FALSE == options_p->cpu_rule_info_p->common.to_cpu &&
                 FALSE == options_p->cpu_rule_info_p->common.flood)
        {
            if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_HARD_DROP))
            {
                LOG("<Error> Set packet command");
                return RULE_TYPE_FAIL;
            }
        }
    }

    switch (remapped_pkt_type)
    {
        case RULE_TYPE_PACKET_WA_PERMIT_DNS_CLIENT:
        {
#define RULE_CTRL_PORT_DNS_SERVER        53

            if (TRUE != DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p))
            {
                LOG("<Error> Qualify IP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyL4DestPort(rule_pattern_p, RULE_CTRL_PORT_DNS_SERVER, 0xffff))
            {
                LOG("<Error> Qualify L4 dest port");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_FORWARD))
            {
                LOG("<Error> Set packet command");
                return RULE_TYPE_FAIL;
            }

            break;

#undef RULE_CTRL_PORT_DNS_SERVER
        }

        case RULE_TYPE_PACKET_WA_PERMIT_DHCP_CLIENT:
        {
#define RULE_CTRL_PORT_DHCP_SERVER       67

            if (TRUE != DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p))
            {
                LOG("<Error> Qualify IP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyL4DestPort(rule_pattern_p, RULE_CTRL_PORT_DHCP_SERVER, 0xffff))
            {
                LOG("<Error> Qualify L4 dest port");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyUdp(rule_pattern_p))
            {
                LOG("<Error> Qualify UDP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_FORWARD))
            {
                LOG("<Error> Set packet command");
                return RULE_TYPE_FAIL;
            }

            break;

#undef RULE_CTRL_PORT_DHCP_SERVER
        }

        case RULE_TYPE_PACKET_DHCP_CLIENT:
        {
#define RULE_CTRL_PORT_DHCP_SERVER       67

            if (TRUE != DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p))
            {
                LOG("<Error> Qualify IP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyIpFragment(rule_pattern_p, 0, 0xff))
            {
                LOG("<Error> Qualify IP Fragment");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyL4DestPort(rule_pattern_p, RULE_CTRL_PORT_DHCP_SERVER, 0xffff))
            {
                LOG("<Error> Qualify L4 dest port");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyUdp(rule_pattern_p))
            {
                LOG("<Error> Qualify UDP");
                return RULE_TYPE_FAIL;
            }

            if (DEVRM_PKT_CMD_COPY_TO_CPU == dev_action_p->pkt_cmd ||
                DEVRM_PKT_CMD_REDIRECT_TO_CPU == dev_action_p->pkt_cmd)
            {
                if (TRUE != DEVRM_LIB_ActionModifyCpuCode(dev_action_p,
                                                          DEV_NICDRV_LIB_PktClassId(SWDRV_PKTTYPE_DHCP_CLIENT)))
                {
                    LOG("<Error> Modify CPU code");
                    return RULE_TYPE_FAIL;
                }
            }

            break;

#undef RULE_CTRL_PORT_DHCP_SERVER
        }

        case RULE_TYPE_PACKET_DHCP_SERVER:
        {
#define RULE_CTRL_PORT_DHCP_CLIENT       68

            if (TRUE != DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p))
            {
                LOG("<Error> Qualify IP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyIpFragment(rule_pattern_p, 0, 0xff))
            {
                LOG("<Error> Qualify IP Fragment");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyL4DestPort(rule_pattern_p, RULE_CTRL_PORT_DHCP_CLIENT, 0xffff))
            {
                LOG("<Error> Qualify L4 dest port");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyUdp(rule_pattern_p))
            {
                LOG("<Error> Qualify UDP");
                return RULE_TYPE_FAIL;
            }

            if (DEVRM_PKT_CMD_COPY_TO_CPU == dev_action_p->pkt_cmd ||
                DEVRM_PKT_CMD_REDIRECT_TO_CPU == dev_action_p->pkt_cmd)
            {
                if (TRUE != DEVRM_LIB_ActionModifyCpuCode(dev_action_p,
                                                          DEV_NICDRV_LIB_PktClassId(SWDRV_PKTTYPE_DHCP_SERVER)))
                {
                    LOG("<Error> Modify CPU code");
                    return RULE_TYPE_FAIL;
                }
            }

            break;

#undef RULE_CTRL_PORT_DHCP_CLIENT
        }

        case RULE_TYPE_PACKET_WA_REDIR_HTTP_CLIENT:
        {
#define RULE_CTRL_PORT_HTTP_SERVER       80

            if (TRUE != DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p))
            {
                LOG("<Error> Qualify IP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyL4DestPort(rule_pattern_p, RULE_CTRL_PORT_HTTP_SERVER, 0xffff))
            {
                LOG("<Error> Qualify L4 dest port");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
            {
                LOG("<Error> Set packet command");
                return RULE_TYPE_FAIL;
            }

            break;

#undef RULE_CTRL_PORT_HTTP_SERVER
        }

        case RULE_TYPE_PACKET_ARP:
        {
            /* ether_type == 0x806
             */

            UI32_T   ether_type = 0x806;

            if (TRUE != DEVRM_LIB_RulePatternQualifyEtherType(rule_pattern_p, ether_type, 0xffff))
            {
                LOG("<Error> Qualify ether type");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
            {
                LOG("<Error> Set packet command");
                return RULE_TYPE_FAIL;
            }

            break;
        }

        case RULE_TYPE_PACKET_ORG_SPECIFIC:
        {
            /* DA == 0x0012CF-000000
             */

            UI8_T org_dst_mac[] = {(UI8_T)(SYS_DFLT_CLUSTER_ADDR_WORD1 >> 8),
                                   (UI8_T)(SYS_DFLT_CLUSTER_ADDR_WORD1),
                                   (UI8_T)(SYS_DFLT_CLUSTER_ADDR_WORD2 >> 8),
                                   (UI8_T)(SYS_DFLT_CLUSTER_ADDR_WORD2),
                                   (UI8_T)(SYS_DFLT_CLUSTER_ADDR_WORD3 >> 8),
                                   (UI8_T)(SYS_DFLT_CLUSTER_ADDR_WORD3)};
            UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

            if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(rule_pattern_p, org_dst_mac, mask))
            {
                LOG("<Error> Qualify DA");
                return RULE_TYPE_FAIL;
            }

            break;
        }

        /* current used by LBD */
        case RULE_TYPE_PACKET_ORG_SPECIFIC1:
        {
            /* DA == 0x0012CF-000001
             */

            UI8_T org_spec1_da[] = SYS_DFLT_ORG_SPEC1_DA;
            UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
            UI8_T cpu_mac[SYS_ADPT_MAC_ADDR_LEN];

            STKTPLG_POM_GetLocalUnitBaseMac(cpu_mac);

            if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(rule_pattern_p, org_spec1_da, mask))
            {
                LOG("<Error> Qualify DA");
                return RULE_TYPE_FAIL;
            }
            if (TRUE != DEVRM_LIB_RulePatternQualifySourceMacAddress(rule_pattern_p, cpu_mac, mask))
            {
                LOG("<Error> Qualify DA");
                return RULE_TYPE_FAIL;
            }

            break;
        }

#if (SYS_CPNT_UDLD == TRUE)
        case RULE_TYPE_PACKET_ORG_SPECIFIC2:
        {
            /* DA == 0x0112CF-000003
             */

            UI8_T org_spec2_da[] = SYS_DFLT_ORG_SPEC2_DA;
            UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

            if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(rule_pattern_p, org_spec2_da, mask))
            {
                LOG("<Error> Qualify DA");
                return RULE_TYPE_FAIL;
            }

            break;
        }
#endif /* SYS_CPNT_UDLD */

        /* currently used by CSC: MLAG */
        case RULE_TYPE_PACKET_ORG_SPECIFIC3:
        {
            /* DA == 0x0112CF-000004
             */

            UI8_T org_spec3_da[] = SYS_DFLT_ORG_SPEC3_DA;
            UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

            if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(rule_pattern_p, org_spec3_da, mask))
            {
                LOG("<Error> Qualify DA");
                return RULE_TYPE_FAIL;
            }

            break;
        }

#if (SYS_CPNT_CFM == TRUE)
        case RULE_TYPE_PACKET_CFM:
        {
            /* ether_type == 0x8902
             */

            if (TRUE != DEVRM_LIB_RulePatternQualifyEtherType(rule_pattern_p, 0x8902, 0xffff))
            {
                LOG("<Error> Qualify ether type");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
            {
                LOG("<Error> Set packet command");
                return RULE_TYPE_FAIL;
            }

            break;
        }
#endif /* SYS_CPNT_CFM */

#if (SYS_CPNT_ERPS == TRUE)
        case RULE_TYPE_PACKET_RAPS:
        {
            /* DA: 0x0119A7-000001
             * ETH: 0x8902
             */

            UI8_T raps_dst_mac[] = {0x01, 0x19, 0xA7, 0x00, 0x00, 0x01};
            UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

            if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(rule_pattern_p, raps_dst_mac, mask))
            {
                LOG("<Error> Qualify DA");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyEtherType(rule_pattern_p, 0x8902, 0xffff))
            {
                LOG("<Error> Qualify ether type");
                return RULE_TYPE_FAIL;
            }

            break;
        }
#endif /* SYS_CPNT_ERPS */

        case RULE_TYPE_PACKET_IP_BCAST:
        {
            /* DA: broadcast MAC
             * ether_type == 0x800
             */

            UI8_T bcast_mac[]={0xff,0xff,0xff,0xff,0xff,0xff};
            UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

            if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(rule_pattern_p, bcast_mac, mask))
            {
                LOG("<Error> Qualify DA");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyEtherType(rule_pattern_p, 0x0800, 0xffff))
            {
                LOG("<Error> Qualify ether type");
                return RULE_TYPE_FAIL;
            }
            break;
        }

        case RULE_TYPE_PACKET_CDP:
        {
            /* DA: 0x01000c-cccccc
             */

            UI8_T cdp_dst_mac[] = {0x01, 0x00, 0x0c, 0xcc, 0xcc, 0xcc};
            UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

            if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(rule_pattern_p, cdp_dst_mac, mask))
            {
                LOG("<Error> Qualify DA");
                return RULE_TYPE_FAIL;
            }

            break;
        }

        case RULE_TYPE_PACKET_PVST:
        {
            /* DA: 0x01000c-cccccc
             */

            UI8_T pvst_dst_mac[] = {0x01, 0x00, 0x0c, 0xcc, 0xcc, 0xcd};
            UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

            if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(rule_pattern_p, pvst_dst_mac, mask))
            {
                LOG("<Error> Qualify DA");
                return RULE_TYPE_FAIL;
            }

            break;
        }

        case RULE_TYPE_PACKET_MY_MAC_ARP:
        {
            /* DA: CPU MAC
             * ether_type == 0x806
             */

            UI32_T ether_type = 0x806;
            UI8_T cpu_mac[SYS_ADPT_MAC_ADDR_LEN];
            UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

            STKTPLG_POM_GetLocalUnitBaseMac(cpu_mac);

            if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(rule_pattern_p, cpu_mac, mask))
            {
                LOG("<Error> Qualify DA");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyEtherType(rule_pattern_p, ether_type, 0xffff))
            {
                LOG("<Error> Qualify ether type");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
            {
                LOG("<Error> Set packet command");
                return RULE_TYPE_FAIL;
            }

            break;
        }

        case RULE_TYPE_PACKET_MY_MAC_CRAFT_PORT:
        {
            /* DA: CPU MAC
             */

            UI8_T cpu_mac[SYS_ADPT_MAC_ADDR_LEN];
            UI8_T mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

            STKTPLG_POM_GetLocalUnitBaseMac(cpu_mac);

            if (TRUE != DEVRM_LIB_RulePatternQualifyDestMacAddress(rule_pattern_p, cpu_mac, mask))
            {
                LOG("<Error> Qualify DA");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
            {
                LOG("<Error> Set packet command");
                return RULE_TYPE_FAIL;
            }

            break;
        }

        case RULE_TYPE_PACKET_RIP:
        {
#define RULE_CTRL_PORT_RIP               520
            /* IPv4
             * UDP
             * DPORT == 520
             */

            if (TRUE != DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p))
            {
                LOG("<Error> Qualify IP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyUdp(rule_pattern_p))
            {
                LOG("<Error> Qualify UDP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyL4DestPort(rule_pattern_p, RULE_CTRL_PORT_RIP, 0xffff))
            {
                LOG("<Error> Qualify L4 dest port");
                return RULE_TYPE_FAIL;
            }

            break;

#undef RULE_CTRL_PORT_RIP
        }

        case RULE_TYPE_PACKET_PTP_EVENT_ETH:
        {
            /* ether_type == 0x88f7
             */

            UI32_T   ether_type = 0x88f7;

            if (TRUE != DEVRM_LIB_RulePatternQualifyEtherType(rule_pattern_p, ether_type, 0xffff))
            {
                LOG("<Error> Qualify ether type");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
            {
                LOG("<Error> Set packet command");
                return RULE_TYPE_FAIL;
            }

            break;
        }

        case RULE_TYPE_PACKET_PTP_EVENT_UDP:
        {
#define RULE_CTRL_PORT_PTP_EVENT         319

            /* SPORT = DPORT == 319
             */

            if (TRUE != DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p))
            {
                LOG("<Error> Qualify IP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyL4SourcePort(rule_pattern_p, RULE_CTRL_PORT_PTP_EVENT, 0xffff))
            {
                LOG("<Error> Qualify L4 source port");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyL4DestPort(rule_pattern_p, RULE_CTRL_PORT_PTP_EVENT, 0xffff))
            {
                LOG("<Error> Qualify L4 dest port");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyUdp(rule_pattern_p))
            {
                LOG("<Error> Qualify UDP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
            {
                LOG("<Error> Set packet command");
                return RULE_TYPE_FAIL;
            }

            break;

#undef RULE_CTRL_PORT_PTP_EVENT
        }

        case RULE_TYPE_PACKET_PTP_GENERAL_UDP:
        {
#define RULE_CTRL_PORT_PTP_GENERAL       320

            /* SPORT = DPORT == 320
             */

            if (TRUE != DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p))
            {
                LOG("<Error> Qualify IP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyL4SourcePort(rule_pattern_p, RULE_CTRL_PORT_PTP_GENERAL, 0xffff))
            {
                LOG("<Error> Qualify L4 source port");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyL4DestPort(rule_pattern_p, RULE_CTRL_PORT_PTP_GENERAL, 0xffff))
            {
                LOG("<Error> Qualify L4 dest port");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyUdp(rule_pattern_p))
            {
                LOG("<Error> Qualify UDP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_REDIRECT_TO_CPU))
            {
                LOG("<Error> Set packet command");
                return RULE_TYPE_FAIL;
            }

            break;

#undef RULE_CTRL_PORT_PTP_GENERAL
        }

        case RULE_TYPE_PACKET_IGMP:
        {
#define RULE_CTRL_PROTOCOL_IGMP          2

            if (TRUE != DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p))
            {
                LOG("<Error> Qualify IP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyIpProtocol(rule_pattern_p, RULE_CTRL_PROTOCOL_IGMP, 0xff))
            {
                LOG("<Error> Qualify IGMP");
                return RULE_TYPE_FAIL;
            }

            break;

#undef RULE_CTRL_PROTOCOL_IGMP
        }

case RULE_TYPE_PACKET_VRRP:
        {
#define RULE_CTRL_PROTOCOL_VRRP         112

            if (TRUE != DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p))
            {
                LOG("<Error> Qualify IP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyIpProtocol(rule_pattern_p, RULE_CTRL_PROTOCOL_VRRP, 0xff))
            {
                LOG("<Error> Qualify VRRP");
                return RULE_TYPE_FAIL;
            }

            break;

#undef RULE_CTRL_PROTOCOL_VRRP
        }

        case RULE_TYPE_PACKET_DOS_CHARGEN:
        {
#define RULE_CTRL_PORT_CHARGEN           19

            /* DPORT == 19
             */

            if (TRUE != DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p))
            {
                LOG("<Error> Qualify IP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyL4DestPort(rule_pattern_p, RULE_CTRL_PORT_CHARGEN, 0xffff))
            {
                LOG("<Error> Qualify L4 dest port");
                return RULE_TYPE_FAIL;
            }

            break;

#undef RULE_CTRL_PORT_CHARGEN
        }

        case RULE_TYPE_PACKET_DOS_ECHO:
        {
#define RULE_CTRL_PORT_ECHO              7

            /* DPORT == 7
             */

            if (TRUE != DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p))
            {
                LOG("<Error> Qualify IP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyL4DestPort(rule_pattern_p, RULE_CTRL_PORT_ECHO, 0xffff))
            {
                LOG("<Error> Qualify L4 dest port");
                return RULE_TYPE_FAIL;
            }

            break;

#undef RULE_CTRL_PORT_ECHO
        }

        case RULE_TYPE_PACKET_DOS_TCP_FLOODING:
        {
            /* TCP flags contains SYN
             */

            if (TRUE != DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p))
            {
                LOG("<Error> Qualify IP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyTcpControlFlags(rule_pattern_p, RULE_CTRL_TCP_FLAG_SYN, RULE_CTRL_TCP_FLAG_SYN))
            {
                LOG("<Error> Qualify TCP control flags");
                return RULE_TYPE_FAIL;
            }

            break;
        }

        case RULE_TYPE_PACKET_DOS_UDP_FLOODING:
        {
            /* UDP
             */

            if (TRUE != DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p))
            {
                LOG("<Error> Qualify IP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyUdp(rule_pattern_p))
            {
                LOG("<Error> Qualify UDP");
                return RULE_TYPE_FAIL;
            }

            break;
        }

        case RULE_TYPE_PACKET_DOS_WIN_NUKE:
        {
#define RULE_CTRL_PORT_NBSS              139    /* NetBIOS Session Service */

            /* TCP flags contains URG
             * DPORT == 139
             */

            if (TRUE != DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p))
            {
                LOG("<Error> Qualify IP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyL4DestPort(rule_pattern_p, RULE_CTRL_PORT_NBSS, 0xffff))
            {
                LOG("<Error> Qualify L4 dest port");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyTcpControlFlags(rule_pattern_p, RULE_CTRL_TCP_FLAG_URG, RULE_CTRL_TCP_FLAG_URG))
            {
                LOG("<Error> Qualify TCP control flags");
                return RULE_TYPE_FAIL;
            }

            break;

#undef RULE_CTRL_PORT_NBSS
        }

        case RULE_TYPE_PACKET_DOS_SMURF:
        {
#define RULE_CTRL_ICMP_TYPE_ECHO_REQ     8

            /* DIP == 255.255.255.255
             * ICMP type == 8 (ECHO Request)
             */

            if (TRUE != DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p))
            {
                LOG("<Error> Qualify IP");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyIpDestAddress(rule_pattern_p, 0xffffffff, 0xffffffff))
            {
                LOG("<Error> Qualify IP dest address");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyIcmpType(rule_pattern_p, RULE_CTRL_ICMP_TYPE_ECHO_REQ, 0xff))
            {
                LOG("<Error> Qualify ICMP type");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_HARD_DROP))
            {
                LOG("<Error> Set packet command");
                return RULE_TYPE_FAIL;
            }

            break;

#undef RULE_CTRL_ICMP_TYPE_ECHO_REQ
        }

        case RULE_TYPE_PACKET_DHCP6_CLIENT:
        {
#define RULE_CTRL_PORT_DHCP6_SERVER      547

            if (TRUE != DEVRM_LIB_RulePatternQualifyIpv6(rule_pattern_p))
            {
                LOG("<Error> Qualify IPv6");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyL4DestPort(rule_pattern_p, RULE_CTRL_PORT_DHCP6_SERVER, 0xffff))
            {
                LOG("<Error> Qualify L4 dest port");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyUdp(rule_pattern_p))
            {
                LOG("<Error> Qualify UDP");
                return RULE_TYPE_FAIL;
            }

            if (DEVRM_PKT_CMD_COPY_TO_CPU == dev_action_p->pkt_cmd ||
                DEVRM_PKT_CMD_REDIRECT_TO_CPU == dev_action_p->pkt_cmd)
            {
                if (TRUE != DEVRM_LIB_ActionModifyCpuCode(dev_action_p,
                                                          DEV_NICDRV_LIB_PktClassId(SWDRV_PKTTYPE_DHCP6_CLIENT)))
                {
                    LOG("<Error> Modify CPU code");
                    return RULE_TYPE_FAIL;
                }
            }

            break;

#undef RULE_CTRL_PORT_DHCP6_SERVER
        }

        case RULE_TYPE_PACKET_DHCP6_SERVER:
        {
#define RULE_CTRL_PORT_DHCP6_CLIENT      546

            if (TRUE != DEVRM_LIB_RulePatternQualifyIpv6(rule_pattern_p))
            {
                LOG("<Error> Qualify IPv6");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyL4DestPort(rule_pattern_p, RULE_CTRL_PORT_DHCP6_CLIENT, 0xffff))
            {
                LOG("<Error> Qualify L4 dest port");
                return RULE_TYPE_FAIL;
            }

            if (TRUE != DEVRM_LIB_RulePatternQualifyUdp(rule_pattern_p))
            {
                LOG("<Error> Qualify UDP");
                return RULE_TYPE_FAIL;
            }

            if (DEVRM_PKT_CMD_COPY_TO_CPU == dev_action_p->pkt_cmd ||
                DEVRM_PKT_CMD_REDIRECT_TO_CPU == dev_action_p->pkt_cmd)
            {
                if (TRUE != DEVRM_LIB_ActionModifyCpuCode(dev_action_p,
                                                          DEV_NICDRV_LIB_PktClassId(SWDRV_PKTTYPE_DHCP6_SERVER)))
                {
                    LOG("<Error> Modify CPU code");
                    return RULE_TYPE_FAIL;
                }
            }

            break;

#undef RULE_CTRL_PORT_DHCP6_CLIENT
        }

        default:
            return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalSetAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a rule entry.
 * INPUT:    unit, port, acl_id, is_trunk, is_add
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetAcl(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI32_T acl_id,
    const RULE_TYPE_Acl_T *acl_entry_p,
    BOOL_T is_add,
    UI32_T time_range_index,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable)
{
    RULE_TYPE_RETURN_TYPE_T result;

    ASSERT(NULL != interface_p);
    ASSERT(interface_p->type < RULE_TYPE_INTERFACE_MAX);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction ||
           RULE_TYPE_OUTBOUND == interface_p->direction);
    ASSERT(NULL != acl_entry_p);

    if (TRUE == is_add)
    {
        result = RULE_CTRL_LocalSetACL2DEV(interface_p,
                                           acl_id,
                                           acl_entry_p,
                                           time_range_index,
                                           counter_enable);
    }
    else
    {
        result = RULE_CTRL_LocalRemoveACLFromDEV(interface_p,
                                                 acl_id,
                                                 acl_entry_p);
    }

    return result;
}

static UI32_T RULE_CTRL_LocalGetKeyModeByACLType (UI32_T acl_type, BOOL_T is_qos)
{
    UI32_T ret =0;

    switch (shmem_data_p->global_tcam_key_mode)
    {
    case SYS_CPNT_TCAM_SEARCH_KEY_MODE_USE_IPV4_MODE1_ONLY:
        ret = RULE_TYPE_IP_STD_ACL;
        break;

    case SYS_CPNT_TCAM_SEARCH_KEY_MODE_USE_MAC_IPV4_IPV6_MODE6_ONLY:
        ret = RULE_TYPE_IP_EXT_ACL;
        break;

    default:
        switch (acl_type)
        {
        case RULE_TYPE_MAC_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
        default:
            return acl_type;
        } /* switch (acl_type) */

        if (TRUE == is_qos)
        {
            ret = RULE_TYPE_IP_EXT_ACL;
        }
    } /* switch (shmem_data_p->global_tcam_key_mode) */

    return ret;
}

static UI32_T RULE_CTRL_LocalGetRuleSizeByKeyMode (UI32_T key_mode)
{
    UI32_T ret =0;

    switch (key_mode)
    {
    case RULE_TYPE_MAC_ACL:
    case RULE_TYPE_IP_STD_ACL:
        /* short */
/*
   pttch do a patch here 2009.10.06, because currently we use extended mode, so all rule size will be 2
  if we have a mix mode we need to modify this.
*/
        ret = 2;
        break;

    // TODO: Check if acl type is RULE_TYPE_IP_ACL or RULE_TYPE_IPV6_ACL
    case RULE_TYPE_IP_ACL:
    case RULE_TYPE_IPV6_ACL:
    case RULE_TYPE_IP_EXT_ACL:
    case RULE_TYPE_IPV6_STD_ACL:
    case RULE_TYPE_IPV6_EXT_ACL:
    case RULE_TYPE_MF:
        /* long */
        ret = 2;
        break;

    default:
        ASSERT(0);
        break;
    }

    return ret;
}

#pragma mark -
#pragma mark Check Resource for ACL/QoS
#pragma mark Move all check Resource function togther

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_CheckResource_AddAce2DEV(
    UI32_T acl_index,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p)
{
    UI32_T *dup_stack_free_nbr_pce = NULL;

    RULE_TYPE_InOutDirection_T direction;

    UI32_T policy_map_index;
    UI32_T class_map_index;

    UI32_T key_mode;
    UI32_T rule_size;

    UI32_T uport_ifindex;

    UI32_T unit;
    UI32_T port;

    UI32_T module_id;
    UI32_T device_id;
    UI32_T phy_port;

    BOOL_T b_result;

    ASSERT(sizeof(shmem_data_p->stack_free_nbr_pce) == sizeof(UI32_T) * SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK * SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT);
    dup_stack_free_nbr_pce = malloc(sizeof(shmem_data_p->stack_free_nbr_pce));

    if (NULL == dup_stack_free_nbr_pce)
    {
        goto undo_malloc;
    }

    memcpy(dup_stack_free_nbr_pce, shmem_data_p->stack_free_nbr_pce, sizeof(shmem_data_p->stack_free_nbr_pce));

    /* Check ACL
     */

    key_mode = RULE_CTRL_LocalGetKeyModeByACLType(acl_entry_p->acl_type, FALSE);

    rule_size = RULE_CTRL_LocalGetRuleSizeByKeyMode(key_mode);
    ASSERT(0 != rule_size);

    uport_ifindex = 0;
    while (RULE_TYPE_OK == RULE_OM_GetNextPort_BindAcl(acl_index,
                                                       &uport_ifindex,
                                                       &direction))
    {
        unit = STKTPLG_OM_IFINDEX_TO_UNIT(uport_ifindex);
        port = STKTPLG_OM_IFINDEX_TO_PORT(uport_ifindex);

        b_result = DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port);
        if (TRUE != b_result)
        {
            goto check_fail;
        }

        if (shmem_data_p->stack_free_nbr_pce[unit-1][device_id] < rule_size)
        {
            goto check_fail;
        }

        shmem_data_p->stack_free_nbr_pce[unit-1][device_id] -= rule_size;
    }

    /* Check QoS
     */
#if (SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL != TRUE)
    if (RULE_TYPE_ACE_PERMIT == ace_entry_p->access)
#endif
    {
        key_mode = RULE_CTRL_LocalGetKeyModeByACLType(acl_entry_p->acl_type, TRUE);

        rule_size = RULE_CTRL_LocalGetRuleSizeByKeyMode(key_mode);
        ASSERT(0 != rule_size);

        uport_ifindex = 0;
        while (RULE_TYPE_OK == RULE_OM_GetNextPort_BindClsMapAcl(acl_index,
                                                                 &uport_ifindex,
                                                                 &direction,
                                                                 &policy_map_index,
                                                                 &class_map_index))
        {
            unit = STKTPLG_OM_IFINDEX_TO_UNIT(uport_ifindex);
            port = STKTPLG_OM_IFINDEX_TO_PORT(uport_ifindex);

            b_result = DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port);
            if (TRUE != b_result)
            {
                goto check_fail;
            }

            if (shmem_data_p->stack_free_nbr_pce[unit-1][device_id] < rule_size)
            {
                goto check_fail;
            }

            shmem_data_p->stack_free_nbr_pce[unit-1][device_id] -= rule_size;
        }
    }

    memcpy(shmem_data_p->stack_free_nbr_pce, dup_stack_free_nbr_pce, sizeof(shmem_data_p->stack_free_nbr_pce));

    free(dup_stack_free_nbr_pce);
    dup_stack_free_nbr_pce = NULL;

    return RULE_TYPE_OK;

check_fail:
    memcpy(shmem_data_p->stack_free_nbr_pce, dup_stack_free_nbr_pce, sizeof(shmem_data_p->stack_free_nbr_pce));

undo_malloc:
    free(dup_stack_free_nbr_pce);
    dup_stack_free_nbr_pce = NULL;

    return RULE_TYPE_FAIL;
}

#pragma mark -

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalIsFreePceAvailable
 *------------------------------------------------------------------------------
 * PURPOSE  : test if a free rule is available and return the rule id
 *              if there is one
 * INPUT    : key_mode  - which key mode is used
 *            pcl_id    - which pcl to test for
 *            is_qos    - for qos or acl
 * OUTPUT   : rule_size - needed pce number for input key_mode
 * RETURN   : rule_id
 * NOTE     : return 0 implies failed
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_CTRL_LocalIsFreePceAvailable(
    UI32_T unit,
    UI32_T device_id,
    UI32_T rule_type,
    UI32_T *rule_size)
{
    UI32_T i, rule_size_tmp, rule_id =0;

    ASSERT(SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK == _countof(shmem_data_p->stack_PCE_bitmap));

    ASSERT(0 < unit && unit <= _countof(shmem_data_p->stack_PCE_bitmap));
    ASSERT(device_id < _countof(shmem_data_p->stack_PCE_bitmap[unit-1]));

    rule_size_tmp = RULE_CTRL_LocalGetRuleSizeByKeyMode(rule_type);

    ASSERT(0 < rule_size_tmp);

    if (0 < rule_size_tmp)
    {
        if (shmem_data_p->stack_free_nbr_pce[unit-1][device_id] >= rule_size_tmp)
        {
            for (i = 0; i < RULE_CTRL_MAX_NBR_OF_PCE_PER_UNIT; i++)
            {
                /*should modify to unit pttch*/
                if (RULE_CTRL_IS_BIT_OFF(shmem_data_p->stack_PCE_bitmap[unit-1][device_id], i))
                {
                    rule_id = i + 1;
                    break;
                }
            }
        }
    }

    if ((rule_id > 0) && (rule_size != NULL))
    {
        *rule_size = rule_size_tmp;
    }

    return rule_id;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalAllocateLogicalRuleID
 *------------------------------------------------------------------------------
 * PURPOSE  : find a free rule id to use
 * INPUT    : unit, device_id - not used for marvell
 *                  rule_type - which key mode is used
 *                  pcl_id    - which pcl to allocate for
 *                  is_qos    - for qos or acl
 * OUTPUT   : none
 * RETURN   : rule_id
 * NOTE     : return 0 implies failed
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_CTRL_LocalAllocateLogicalRuleID(
    UI32_T unit,
    UI32_T device_id,
    UI32_T rule_type)
{
    UI32_T rule_id;
    UI32_T rule_size;

    ASSERT(SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK == _countof(shmem_data_p->stack_PCE_bitmap));
    ASSERT(SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT == _countof(shmem_data_p->stack_PCE_bitmap[unit-1]));

    ASSERT(0 < unit && unit <= _countof(shmem_data_p->stack_PCE_bitmap));
    ASSERT(device_id < _countof(shmem_data_p->stack_PCE_bitmap[unit-1]));

    rule_id = RULE_CTRL_LocalIsFreePceAvailable(unit, device_id, rule_type, &rule_size);

    if (0 < rule_id)
    {
        ASSERT(rule_id <= (_countof(shmem_data_p->stack_PCE_bitmap[unit-1][device_id]) * 8));

        ASSERT(RULE_CTRL_IS_BIT_OFF(shmem_data_p->stack_PCE_bitmap[unit-1][device_id], rule_id - 1));

        ASSERT(0 < unit && unit <= _countof(shmem_data_p->stack_free_nbr_pce));

        ASSERT(rule_size <= shmem_data_p->stack_free_nbr_pce[unit-1][device_id]);

        RULE_CTRL_SET_BIT_ON(shmem_data_p->stack_PCE_bitmap[unit-1][device_id], (rule_id - 1));
        shmem_data_p->stack_free_nbr_pce[unit-1][device_id] -= rule_size;
    }

    return rule_id;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalFreeLogicalRuleID
 *------------------------------------------------------------------------------
 * PURPOSE  : increase free rule number and clear used-bit
 * INPUT    : unit, device_id, selector_id, rule_id
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalFreeLogicalRuleID(
    UI32_T unit,
    UI32_T device_id,
    UI32_T rule_type,
    UI32_T rule_id)
{
    UI32_T rule_size;

    ASSERT(SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK == _countof(shmem_data_p->stack_PCE_bitmap));
    ASSERT(SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT == _countof(shmem_data_p->stack_PCE_bitmap[unit-1]));

    ASSERT(0 < unit && unit <= _countof(shmem_data_p->stack_PCE_bitmap));
    ASSERT(device_id < _countof(shmem_data_p->stack_PCE_bitmap[unit-1]));

    if (0 == rule_id || RULE_CTRL_MAX_NBR_OF_PCE_PER_UNIT < rule_id)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    /* make sure the input rule_id is good
     */
    if ((rule_id > 0) && (rule_id <= RULE_CTRL_MAX_NBR_OF_PCE_PER_UNIT))
    {
        rule_size = RULE_CTRL_LocalGetRuleSizeByKeyMode(rule_type);

        ASSERT(0 < rule_size);
        ASSERT(RULE_CTRL_IS_BIT_ON(shmem_data_p->stack_PCE_bitmap[unit-1][device_id], rule_id-1));

        if ((rule_size >0) && (RULE_CTRL_IS_BIT_ON(shmem_data_p->stack_PCE_bitmap[unit-1][device_id], rule_id-1)))
        {
            RULE_CTRL_SET_BIT_OFF(shmem_data_p->stack_PCE_bitmap[unit-1][device_id], (rule_id-1));
            shmem_data_p->stack_free_nbr_pce[unit-1][device_id] += rule_size;

            return RULE_TYPE_OK;
        }
    }
    else
    {
        LOG("[RULE_CTRL_LocalFreeLogicalRuleID] invalid rule id.")
        ASSERT(0);
    }

    ASSERT(0);
    return RULE_TYPE_FAIL;
}

#pragma mark -
#pragma mark Instance Functions

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_InitInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Initialize instance table
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Initialize before any instance operations
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_InitInstance()
{
    shmem_data_p->_container_pool.buffer_offset =
    L_CVRT_GET_OFFSET(&shmem_data_p->_container_pool,
                      &shmem_data_p->_container_pool_buf);
    shmem_data_p->_container_pool.buffer_len = sizeof(shmem_data_p->_container_pool_buf);
    shmem_data_p->_container_pool.partition_len = sizeof(*shmem_data_p->_container_pool_buf);
    L_PT_ShMem_Create(&shmem_data_p->_container_pool);

    shmem_data_p->_class_inst_pool.buffer_offset =
    L_CVRT_GET_OFFSET(&shmem_data_p->_class_inst_pool,
                      &shmem_data_p->_class_inst_pool_buf);
    shmem_data_p->_class_inst_pool.buffer_len = sizeof(shmem_data_p->_class_inst_pool_buf);
    shmem_data_p->_class_inst_pool.partition_len = sizeof(*shmem_data_p->_class_inst_pool_buf);
    L_PT_ShMem_Create(&shmem_data_p->_class_inst_pool);

    shmem_data_p->_rule_inst_pool.buffer_offset =
    L_CVRT_GET_OFFSET(&shmem_data_p->_rule_inst_pool,
                      &shmem_data_p->_rule_inst_pool_buf);
    shmem_data_p->_rule_inst_pool.buffer_len = sizeof(shmem_data_p->_rule_inst_pool_buf);
    shmem_data_p->_rule_inst_pool.partition_len = sizeof(*shmem_data_p->_rule_inst_pool_buf);
    L_PT_ShMem_Create(&shmem_data_p->_rule_inst_pool);

#ifdef RULE_CTRL_INSTANCE_DEBUG_ON
    {
        UI32_T i;

        for (i = 0; i < _countof(shmem_data_p->_container_pool_buf); ++ i)
        {
            shmem_data_p->_container_pool_buf[i].check_code = _CONTAINER_CHECK_CODE;
        }

        for (i = 0; i < _countof(shmem_data_p->_class_inst_pool_buf); ++ i)
        {
            shmem_data_p->_class_inst_pool_buf[i].check_code = _CLASS_INSTANCE_CHECK_CODE;
        }

        for (i = 0; i < _countof(shmem_data_p->_rule_inst_pool_buf); ++ i)
        {
            shmem_data_p->_rule_inst_pool_buf[i].check_code = _RULE_INSTANCE_CHECK_CODE;
        }
    }
#endif /* RULE_CTRL_INSTANCE_DEBUG_ON */

}

#ifdef RULE_CTRL_INSTANCE_DEBUG_ON
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ValidateContainer
 *------------------------------------------------------------------------------
 * PURPOSE  : Validate container
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only for debug
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ValidateContainer(
    const RULE_TYPE_CONTAINER_T *in)
{
    UI32_T i;
    UI32_T f;

    ASSERT(NULL != in);

    ASSERT(TRUE == RULE_CTRL_IS_CONTAINER_TYPE(in->super.type));
    ASSERT(_CONTAINER_CHECK_CODE == in->check_code);

    ASSERT(RULE_TYPE_NIL == in->super.links.first_node.type);
    ASSERT(RULE_TYPE_NIL == in->super.links.last_node.type);

    /* The array of container shall be packed
     */
    f = _countof(in->array);
    for (i = 0; i < _countof(in->array); ++ i)
    {
        if (RULE_TYPE_NIL == in->array[i].ptr.type)
        {
            f = i;
            break;
        }
    }

    for (i = f; i < _countof(in->array); ++ i)
    {
        ASSERT(RULE_TYPE_NIL == in->array[i].ptr.type);
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ValidateClassInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Validate class instance
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only for debug
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ValidateClassInstance(
    const RULE_TYPE_CLASS_INSTANCE_T *in)
{
    ASSERT(NULL != in);

    ASSERT(TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(in->super.type));
    ASSERT(_CLASS_INSTANCE_CHECK_CODE == in->check_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ValidateRuleInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Validate rule instance
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only for debug
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ValidateRuleInstance(
    const RULE_TYPE_RULE_INSTANCE_T *in)
{
    ASSERT(NULL != in);

    ASSERT(TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(in->super.type));
    ASSERT(_RULE_INSTANCE_CHECK_CODE == in->check_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ValidatePtr
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ValidatePtr(
    RULE_TYPE_INSTANCE_T *in)
{
    ASSERT(NULL != in);

    if (TRUE == RULE_CTRL_IS_CONTAINER_TYPE(in->type))
    {
        RULE_CTRL_IN_Priv_ValidateContainer((RULE_TYPE_CONTAINER_T *) in);
    }
    else if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(in->type))
    {
        RULE_CTRL_IN_Priv_ValidateClassInstance((RULE_TYPE_CLASS_INSTANCE_T *) in);
    }
    else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(in->type))
    {
        RULE_CTRL_IN_Priv_ValidateRuleInstance((RULE_TYPE_RULE_INSTANCE_T *) in);
    }
    else
    {
        ASSERT(0);
    }
}
#endif /* RULE_CTRL_INSTANCE_DEBUG_ON */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_BaseAddress
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
void *
RULE_CTRL_IN_BaseAddress(
    RULE_TYPE_INSTANCE_TYPE_T type)
{
    if (TRUE == RULE_CTRL_IS_CONTAINER_TYPE(type))
    {
        return &shmem_data_p->_container_pool;
    }
    else if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(type))
    {
        return &shmem_data_p->_class_inst_pool;
    }
    else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(type))
    {
        return &shmem_data_p->_rule_inst_pool;
    }

    ASSERT(0);
    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_GetFirstAddress
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
void *
RULE_CTRL_IN_FirstNodeAddress(
    RULE_TYPE_INSTANCE_TYPE_T type)
{
    if (TRUE == RULE_CTRL_IS_CONTAINER_TYPE(type))
    {
        return &shmem_data_p->_container_pool_buf;
    }
    else if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(type))
    {
        return &shmem_data_p->_class_inst_pool_buf;
    }
    else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(type))
    {
        return &shmem_data_p->_rule_inst_pool_buf;
    }

    ASSERT(0);
    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_CountOf
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_CTRL_IN_CountOf(
    RULE_TYPE_INSTANCE_TYPE_T type)
{
    if (TRUE == RULE_CTRL_IS_CONTAINER_TYPE(type))
    {
        return _countof(shmem_data_p->_container_pool_buf);
    }
    else if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(type))
    {
        return _countof(shmem_data_p->_class_inst_pool_buf);
    }
    else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(type))
    {
        return _countof(shmem_data_p->_rule_inst_pool_buf);
    }

    ASSERT(0);
    return 0;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_SizeOf
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_CTRL_IN_SizeOf(
    RULE_TYPE_INSTANCE_T *in)
{
    ASSERT(NULL != in);

    if (TRUE == RULE_CTRL_IS_CONTAINER_TYPE(in->type))
    {
        return sizeof(shmem_data_p->_container_pool_buf[0]);
    }
    else if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(in->type))
    {
        return sizeof(shmem_data_p->_class_inst_pool_buf[0]);
    }
    else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(in->type))
    {
        return sizeof(shmem_data_p->_rule_inst_pool_buf[0]);
    }

    ASSERT(0);
    return 0;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_IndexOf
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_CTRL_IN_IndexOf(
    RULE_TYPE_INSTANCE_T *in_p)
{
    RULE_TYPE_SHM_POINTER_T ptr = {RULE_TYPE_NIL, 0};
    UI32_T size_desc_to_buf;

    ASSERT(NULL != in_p);

    RULE_CTRL_IN_VALIDATE_PTR(in_p);

    ptr.type = in_p->type;
    ptr.offset = L_CVRT_GET_OFFSET(RULE_CTRL_IN_BaseAddress(in_p->type), in_p);

    size_desc_to_buf = RULE_CTRL_IN_FirstNodeAddress(in_p->type) -
                                           RULE_CTRL_IN_BaseAddress(in_p->type);

    ASSERT((ptr.offset - size_desc_to_buf) % RULE_CTRL_IN_SizeOf(in_p) == 0);

    return (ptr.offset - size_desc_to_buf) / RULE_CTRL_IN_SizeOf(in_p);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_AtIndex
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
void *
RULE_CTRL_IN_AtIndex(
    RULE_TYPE_INSTANCE_TYPE_T type,
    UI32_T index)
{
    void *ary;

    ary = RULE_CTRL_IN_FirstNodeAddress(type);

    ASSERT(NULL != ary);

    /* Make sure the index was in the range
     */
    if (RULE_CTRL_IN_CountOf(type) <= index)
    {
        return NULL;
    }

    if (TRUE == RULE_CTRL_IS_CONTAINER_TYPE(type))
    {
        return &((RULE_TYPE_CONTAINER_T *)ary)[index];
    }
    else if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(type))
    {
        return &((RULE_TYPE_CLASS_INSTANCE_T *)ary)[index];
    }
    else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(type))
    {
        return &((RULE_TYPE_RULE_INSTANCE_T *)ary)[index];
    }

    ASSERT(0);
    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_IndexOfClassInstance
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_CTRL_IN_IndexOfClassInstance(
    RULE_TYPE_CLASS_INSTANCE_T *in_p)
{
    return RULE_CTRL_IN_IndexOf((RULE_TYPE_INSTANCE_T *) in_p);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_IndexOfRuleInstance
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_CTRL_IN_IndexOfRuleInstance(
    RULE_TYPE_RULE_INSTANCE_T *in_p)
{
    return RULE_CTRL_IN_IndexOf((RULE_TYPE_INSTANCE_T *) in_p);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Ptr2Instance
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
void *
RULE_CTRL_IN_Ptr2Instance(
    RULE_TYPE_SHM_POINTER_T ptr)
{
    if (RULE_TYPE_NIL == ptr.type)
    {
        return NULL;
    }

    return L_CVRT_GET_PTR(RULE_CTRL_IN_BaseAddress(ptr.type), ptr.offset);
}

RULE_TYPE_SHM_POINTER_T
RULE_CTRL_IN_Instance2Ptr(
    const RULE_TYPE_INSTANCE_PTR_T in)
{
    RULE_TYPE_SHM_POINTER_T ptr = {RULE_TYPE_NIL, 0};

    if (NULL == in)
    {
        return ptr;
    }

    RULE_CTRL_IN_VALIDATE_PTR(in);

    ptr.type = in->type;
    ptr.offset = L_CVRT_GET_OFFSET(RULE_CTRL_IN_BaseAddress(in->type), in);

    return ptr;
}

#pragma mark Linked List Functions

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Linked List Functions
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_LL_Priv_InsertAfter
 *------------------------------------------------------------------------------
 * PURPOSE  : Inserts node after the specified node
 * INPUT    : list_ptr      - list
 *            new_ptr       - inserts node after this node
 *            new_node_ptr  - new node
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_LL_Priv_InsertAfter(
    RULE_TYPE_SHM_POINTER_T list_ptr,
    RULE_TYPE_SHM_POINTER_T node_ptr,
    RULE_TYPE_SHM_POINTER_T new_node_ptr)
{
    RULE_TYPE_INSTANCE_T *list;
    RULE_TYPE_INSTANCE_T *node;
    RULE_TYPE_INSTANCE_T *new_node;

    ASSERT(RULE_TYPE_NIL != list_ptr.type);
    ASSERT(RULE_TYPE_NIL != node_ptr.type);
    ASSERT(RULE_TYPE_NIL != new_node_ptr.type);

    list = RULE_CTRL_IN_Ptr2Instance(list_ptr);
    node = RULE_CTRL_IN_Ptr2Instance(node_ptr);
    new_node = RULE_CTRL_IN_Ptr2Instance(new_node_ptr);

    RULE_CTRL_IN_VALIDATE_PTR(list);
    RULE_CTRL_IN_VALIDATE_PTR(node);
    RULE_CTRL_IN_VALIDATE_PTR(new_node);

    new_node->links.prev = node_ptr;
    new_node->links.next = node->links.next;

    if (RULE_TYPE_NIL == node->links.next.type)
    {
        list->links.last_node = new_node_ptr;
    }
    else
    {
        RULE_TYPE_INSTANCE_T *next = RULE_CTRL_IN_Ptr2Instance(node_ptr);

        RULE_CTRL_IN_VALIDATE_PTR(next);

        next->links.prev = new_node_ptr;
    }

    node->links.next = new_node_ptr;
    new_node->links.parent = list_ptr;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_LL_Priv_InsertBefore
 *------------------------------------------------------------------------------
 * PURPOSE  : Inserts node before the specified node
 * INPUT    : list_ptr      - list
 *            new_ptr       - inserts node before this node
 *            new_node_ptr  - new node
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_LL_Priv_InsertBefore(
    RULE_TYPE_SHM_POINTER_T list_ptr,
    RULE_TYPE_SHM_POINTER_T node_ptr,
    RULE_TYPE_SHM_POINTER_T new_node_ptr)
{
    RULE_TYPE_INSTANCE_T *list;
    RULE_TYPE_INSTANCE_T *node;
    RULE_TYPE_INSTANCE_T *new_node;

    ASSERT(RULE_TYPE_NIL != list_ptr.type);
    ASSERT(RULE_TYPE_NIL != node_ptr.type);
    ASSERT(RULE_TYPE_NIL != new_node_ptr.type);

    list = RULE_CTRL_IN_Ptr2Instance(list_ptr);
    node = RULE_CTRL_IN_Ptr2Instance(node_ptr);
    new_node = RULE_CTRL_IN_Ptr2Instance(new_node_ptr);

    RULE_CTRL_IN_VALIDATE_PTR(list);
    RULE_CTRL_IN_VALIDATE_PTR(node);
    RULE_CTRL_IN_VALIDATE_PTR(new_node);

    new_node->links.prev = node->links.prev;
    new_node->links.next = node_ptr;

    if (RULE_TYPE_NIL == node->links.prev.type)
    {
        list->links.first_node = new_node_ptr;
    }
    else
    {
        RULE_TYPE_INSTANCE_T *prev = RULE_CTRL_IN_Ptr2Instance(node->links.prev);

        RULE_CTRL_IN_VALIDATE_PTR(prev);
        prev->links.next = new_node_ptr;
    }

    node->links.prev = new_node_ptr;
    new_node->links.parent = list_ptr;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_LL_Priv_InsertBeginning
 *------------------------------------------------------------------------------
 * PURPOSE  : Inserts node before current first node
 * INPUT    : list_ptr      - list
 *            new_node_ptr  - new node
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_LL_Priv_InsertBeginning(
    RULE_TYPE_SHM_POINTER_T list_ptr,
    RULE_TYPE_SHM_POINTER_T new_node_ptr)
{
    RULE_TYPE_INSTANCE_T *list;
    RULE_TYPE_INSTANCE_T *new_node;

    ASSERT(RULE_TYPE_NIL != list_ptr.type);
    ASSERT(RULE_TYPE_NIL != new_node_ptr.type);

    list = RULE_CTRL_IN_Ptr2Instance(list_ptr);
    new_node = RULE_CTRL_IN_Ptr2Instance(new_node_ptr);

    RULE_CTRL_IN_VALIDATE_PTR(list);
    RULE_CTRL_IN_VALIDATE_PTR(new_node);

    if (RULE_TYPE_NIL == list->links.first_node.type)
    {
        list->links.first_node = new_node_ptr;
        list->links.last_node  = new_node_ptr;
        new_node->links.parent = list_ptr;

        memset(&new_node->links.prev, 0, sizeof(new_node->links.prev));
        memset(&new_node->links.next, 0, sizeof(new_node->links.next));
    }
    else
    {
        RULE_CTRL_IN_LL_Priv_InsertBefore(list_ptr, list->links.first_node, new_node_ptr);
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_LL_Priv_InsertEnd
 *------------------------------------------------------------------------------
 * PURPOSE  : Append node
 * INPUT    : list_ptr      - list
 *            new_node_ptr  - new node
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_LL_Priv_InsertEnd(
    RULE_TYPE_SHM_POINTER_T list_ptr,
    RULE_TYPE_SHM_POINTER_T new_node_ptr)
{
    RULE_TYPE_INSTANCE_T *list;
    RULE_TYPE_INSTANCE_T *new_node;

    ASSERT(RULE_TYPE_NIL != list_ptr.type);
    ASSERT(RULE_TYPE_NIL != new_node_ptr.type);

    list = RULE_CTRL_IN_Ptr2Instance(list_ptr);
    new_node = RULE_CTRL_IN_Ptr2Instance(new_node_ptr);

    RULE_CTRL_IN_VALIDATE_PTR(list);
    RULE_CTRL_IN_VALIDATE_PTR(new_node);

    if (RULE_TYPE_NIL == list->links.last_node.type)
    {
        RULE_CTRL_IN_LL_Priv_InsertBeginning(list_ptr, new_node_ptr);
    }
    else
    {
        RULE_CTRL_IN_LL_Priv_InsertAfter(list_ptr, list->links.last_node, new_node_ptr);
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_LL_Priv_Remove
 *------------------------------------------------------------------------------
 * PURPOSE  : Remove node
 * INPUT    : list_ptr      - list
 *            node_ptr      - removed node
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_LL_Priv_Remove(
    RULE_TYPE_SHM_POINTER_T list_ptr,
    RULE_TYPE_SHM_POINTER_T node_ptr)
{
    RULE_TYPE_INSTANCE_T *list;
    RULE_TYPE_INSTANCE_T *node;

    ASSERT(RULE_TYPE_NIL != list_ptr.type);
    ASSERT(RULE_TYPE_NIL != node_ptr.type);

    list = RULE_CTRL_IN_Ptr2Instance(list_ptr);
    node = RULE_CTRL_IN_Ptr2Instance(node_ptr);

    RULE_CTRL_IN_VALIDATE_PTR(list);
    RULE_CTRL_IN_VALIDATE_PTR(node);

    if (RULE_TYPE_NIL == node->links.prev.type)
    {
        list->links.first_node = node->links.next;
    }
    else
    {
        RULE_TYPE_INSTANCE_T *prev = RULE_CTRL_IN_Ptr2Instance(node->links.prev);

        RULE_CTRL_IN_VALIDATE_PTR(prev);
        prev->links.next = node->links.next;
    }

    if (RULE_TYPE_NIL == node->links.next.type)
    {
        list->links.last_node = node->links.prev;
    }
    else
    {
        RULE_TYPE_INSTANCE_T *next = RULE_CTRL_IN_Ptr2Instance(node->links.next);

        RULE_CTRL_IN_VALIDATE_PTR(next);
        next->links.prev = node->links.prev;
    }

    node->links.parent.type = RULE_TYPE_NIL;

    memset(&node->links.prev, 0, sizeof(node->links.prev));
    memset(&node->links.next, 0, sizeof(node->links.next));
}

#pragma mark Class/Rule Instance Functions

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Class/Rule Instance Functions
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Create instance
 * INPUT    : base_addr - base address of entry table
 *            type      - type of instance
 * OUTPUT   : None
 * RETURN   : Pointer of instance entry
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_SHM_POINTER_T
RULE_CTRL_IN_Priv_CreateInstance(
    void *base_addr,
    RULE_TYPE_INSTANCE_TYPE_T type)
{
    RULE_TYPE_SHM_POINTER_T ptr = {RULE_TYPE_NIL, 0};
    RULE_TYPE_INSTANCE_T *in;

    ASSERT(NULL != base_addr);
    ASSERT(RULE_TYPE_INST_TYPE_FIRST <= type && type <= RULE_TYPE_INST_TYPE_LAST);

    in = L_PT_ShMem_Allocate(base_addr);

    if (NULL == in)
    {
        return ptr;
    }

    memset(in, 0, sizeof(*in));

    in->type = type;

    ptr.offset = L_CVRT_GET_OFFSET(base_addr, in);
    ptr.type = type;

    return ptr;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy instance
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_DestroyInstance(
    RULE_TYPE_SHM_POINTER_T ptr)
{
    RULE_TYPE_INSTANCE_T *in;

    if (RULE_TYPE_NIL == ptr.type)
    {
        ASSERT(0);
        return;
    }

    in = RULE_CTRL_IN_Ptr2Instance(ptr);

    RULE_CTRL_IN_VALIDATE_PTR(in);

    in->type = RULE_TYPE_NIL;
    memset(&in->links, 0, sizeof(in->links));

    L_PT_ShMem_Free(RULE_CTRL_IN_BaseAddress(ptr.type), in);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateContainer
 *------------------------------------------------------------------------------
 * PURPOSE  : Create container
 * INPUT    : type  - type of instance
 * OUTPUT   : None
 * RETURN   : Pointer of instance entry
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_SHM_POINTER_T
RULE_CTRL_IN_Priv_CreateContainer()
{
    RULE_TYPE_INSTANCE_TYPE_T type = RULE_TYPE_INST_CONTAINER;

    RULE_TYPE_SHM_POINTER_T ptr = {RULE_TYPE_NIL, 0};
    RULE_TYPE_CONTAINER_T *in;
    void *base_addr = NULL;

    base_addr = RULE_CTRL_IN_BaseAddress(type);
    ptr =  RULE_CTRL_IN_Priv_CreateInstance(base_addr, type);

    if (RULE_TYPE_NIL == ptr.type)
    {
        return ptr;
    }

    in = RULE_CTRL_IN_Ptr2Instance(ptr);

    RULE_CTRL_IN_VALIDATE_PTR(in);

    return ptr;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateClassInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Create class instance
 * INPUT    : type  - type of instance
 * OUTPUT   : None
 * RETURN   : Pointer of instance entry
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_SHM_POINTER_T
RULE_CTRL_IN_Priv_CreateClassInstance(
    RULE_TYPE_INSTANCE_TYPE_T type)
{
    RULE_TYPE_SHM_POINTER_T ptr = {RULE_TYPE_NIL, 0};
    RULE_TYPE_CLASS_INSTANCE_T *in;
    void *base_addr = NULL;

    if (TRUE != RULE_CTRL_IS_CLASS_INSTANCE_TYPE(type))
    {
        return ptr;
    }

    base_addr = RULE_CTRL_IN_BaseAddress(type);
    ptr = RULE_CTRL_IN_Priv_CreateInstance(base_addr, type);

    if (RULE_TYPE_NIL == ptr.type)
    {
        return ptr;
    }

    in = RULE_CTRL_IN_Ptr2Instance(ptr);
    in->id = 0xCCCC;

    RULE_CTRL_IN_VALIDATE_CLASS_INST(in);

    return ptr;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateRuleInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Create rule instance
 * INPUT    : type  - type of instance
 * OUTPUT   : None
 * RETURN   : Pointer of instance entry
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_SHM_POINTER_T
RULE_CTRL_IN_Priv_CreateRuleInstance(
    RULE_TYPE_INSTANCE_TYPE_T type)
{
    RULE_TYPE_SHM_POINTER_T ptr = {RULE_TYPE_NIL, 0};
    RULE_TYPE_RULE_INSTANCE_T *in;
    void *base_addr = NULL;

    if (TRUE != RULE_CTRL_IS_RULE_INSTANCE_TYPE(type))
    {
        return ptr;
    }

    base_addr = RULE_CTRL_IN_BaseAddress(type);
    ptr =  RULE_CTRL_IN_Priv_CreateInstance(base_addr, type);

    if (RULE_TYPE_NIL == ptr.type)
    {
        return ptr;
    }

    in = RULE_CTRL_IN_Ptr2Instance(ptr);

    RULE_CTRL_IN_VALIDATE_RULE_INST(in);

    in->id = 0xCCCC;
    in->dev_rule_info.rule_id = 0x01020304;

    RULE_CTRL_IN_VALIDATE_PTR(in);

    return ptr;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyContainerInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy container
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : All child instance also be destroied
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_DestroyContainer(
    RULE_TYPE_SHM_POINTER_T ptr)
{
    RULE_TYPE_CONTAINER_T                   *in;
    RULE_TYPE_CONTAINER_OBJECT_T            this;
    RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T  iter;

    if (RULE_TYPE_NIL == ptr.type)
    {
        ASSERT(0);
        return;
    }

    in = RULE_CTRL_IN_Ptr2Instance(ptr);

    RULE_CTRL_IN_VALIDATE_PTR(in);

    RULE_CTRL_IN_Priv_ConstructContainerObject(in, &this);
    this.get_element_iterator(&this, &iter);

    for (iter.last(&iter); NULL != iter.get_instance(&iter); iter.last(&iter))
    {
        RULE_TYPE_SHM_POINTER_T child = iter.get_ptr(&iter);

        RULE_CTRL_IN_Priv_Container_RemoveInstance(&this, RULE_CTRL_IN_Ptr2Instance(child));

        if (TRUE == RULE_CTRL_IS_CONTAINER_TYPE(child.type))
        {
            RULE_CTRL_IN_Priv_DestroyContainer(child);
        }
        else if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(child.type))
        {
            RULE_CTRL_IN_Priv_DestroyClassInstance(child);
        }
        else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(child.type))
        {
            RULE_CTRL_IN_Priv_DestroyRuleInstance(child);
        }
        else
        {
            ASSERT(0);
        }
    }

    RULE_CTRL_IN_Priv_DestroyInstance(ptr);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyClassInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy class instance
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : All child instance also be destroied
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_DestroyClassInstance(
    RULE_TYPE_SHM_POINTER_T ptr)
{
    RULE_TYPE_CLASS_INSTANCE_T                  *in;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T           this;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T iter;

    if (RULE_TYPE_NIL == ptr.type)
    {
        ASSERT(0);
        return;
    }

    in = RULE_CTRL_IN_Ptr2Instance(ptr);

    RULE_CTRL_IN_VALIDATE_CLASS_INST(in);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(in, &this);
    this.get_element_iterator(&this, &iter);

    for (iter.last(&iter); NULL != iter.get_instance(&iter); iter.last(&iter))
    {
        RULE_TYPE_SHM_POINTER_T child = iter.get_ptr(&iter);

        RULE_CTRL_IN_LL_Priv_Remove(ptr, child);

        if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(child.type))
        {
            RULE_CTRL_IN_Priv_DestroyClassInstance(child);
        }
        else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(child.type))
        {
            RULE_CTRL_IN_Priv_DestroyRuleInstance(child);
        }
        else
        {
            ASSERT(0);
        }
    }

    in->id = 0xDDDD;

    RULE_CTRL_IN_Priv_DestroyInstance(ptr);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyRuleInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy rule instance
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_DestroyRuleInstance(
    RULE_TYPE_SHM_POINTER_T ptr)
{
    RULE_TYPE_RULE_INSTANCE_T *in;

    if (RULE_TYPE_NIL == ptr.type)
    {
        ASSERT(0);
        return;
    }

    in = RULE_CTRL_IN_Ptr2Instance(ptr);

    RULE_CTRL_IN_VALIDATE_RULE_INST(in);

    in->id = 0xDDDD;
    in->dev_rule_info.rule_id = 0x06070809;

    RULE_CTRL_IN_Priv_DestroyInstance(ptr);
}

#pragma mark Help Functions

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Help Function for operating the rule/class instances
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_Container_AddInstance(
    RULE_TYPE_CONTAINER_OBJECT_T *this,
    const RULE_TYPE_INSTANCE_PTR_T in)
{
    RULE_TYPE_SHM_POINTER_T ptr = {RULE_TYPE_NIL, 0};
    UI32_T i;
    UI32_T f = _countof(this->inst->array);

    ASSERT(NULL != this);
    ASSERT(NULL != in);

    ptr = RULE_CTRL_IN_Instance2Ptr(in);
    if (RULE_TYPE_NIL == ptr.type)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    for (i = 0; i < _countof(this->inst->array); ++ i)
    {
        if (0 == memcmp(&ptr, &this->inst->array[i].ptr, sizeof(ptr)))
        {
            return RULE_TYPE_OK;
        }

        if (f == _countof(this->inst->array))
        {
            if (RULE_TYPE_NIL == this->inst->array[i].ptr.type)
            {
                f = i;
            }
        }
    }

    if (f < _countof(this->inst->array))
    {
        this->inst->array[f].ptr = ptr;
        in->links.parent = this->ptr;
        return RULE_TYPE_OK;
    }

    return RULE_TYPE_FAIL;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_Container_RemoveInstance(
    RULE_TYPE_CONTAINER_OBJECT_T *this,
    const RULE_TYPE_INSTANCE_PTR_T in)
{
    RULE_TYPE_SHM_POINTER_T ptr = {RULE_TYPE_NIL, 0};
    UI32_T i;

    ASSERT(NULL != this);
    ASSERT(NULL != in);

    ptr = RULE_CTRL_IN_Instance2Ptr(in);
    if (RULE_TYPE_NIL == ptr.type)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    for (i = 0; i < _countof(this->inst->array); ++ i)
    {
        if (0 == memcmp(&ptr, &this->inst->array[i].ptr, sizeof(ptr)))
        {
            if (i < (_countof(this->inst->array) - 1))
            {
                memmove(&this->inst->array[i].ptr,
                        &this->inst->array[i + 1].ptr,
                        (_countof(this->inst->array) - (i + 1)) * sizeof(this->inst->array[0]));
            }

            memset(&this->inst->array[ _countof(this->inst->array)- 1 ].ptr, 0,
                   sizeof(this->inst->array[0]));

            this->inst->array[ _countof(this->inst->array)- 1 ].ptr.type = RULE_TYPE_NIL;

            in->links.parent.type = RULE_TYPE_NIL;

            return RULE_TYPE_OK;
        }
    }

    return RULE_TYPE_OK;
}

///*------------------------------------------------------------------------------
// * ROUTINE NAME -
// *------------------------------------------------------------------------------
// * PURPOSE  :
// * INPUT    :
// * OUTPUT   :
// * RETURN   :
// * NOTE     :
// *------------------------------------------------------------------------------
// */
//static RULE_TYPE_RETURN_TYPE_T
//RULE_CTRL_IN_Priv_Container_AddInstance(
//    RULE_TYPE_CONTAINER_OBJECT_T *this,
//    const RULE_TYPE_INSTANCE_PTR_T in)
//{
//    return RULE_CTRL_IN_Priv_Container_AddInstance(this, RULE_CTRL_IN_Instance2Ptr(in));
//}
//
///*------------------------------------------------------------------------------
// * ROUTINE NAME -
// *------------------------------------------------------------------------------
// * PURPOSE  :
// * INPUT    :
// * OUTPUT   :
// * RETURN   :
// * NOTE     :
// *------------------------------------------------------------------------------
// */
//static RULE_TYPE_RETURN_TYPE_T
//RULE_CTRL_IN_Priv_Container_RemoveInstance(
//    RULE_TYPE_CONTAINER_OBJECT_T *this,
//    const RULE_TYPE_INSTANCE_PTR_T in)
//{
//    ASSERT(NULL != this);
//    ASSERT(NULL != in);
//
//    return RULE_CTRL_IN_Priv_Container_RemoveInstance(this, RULE_CTRL_IN_Instance2Ptr(in));
//}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_Container_AddClassInstanceObject(
    RULE_TYPE_CONTAINER_OBJECT_T *this,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *new_obj)
{
    return RULE_CTRL_IN_Priv_Container_AddInstance(this,
                                      (RULE_TYPE_INSTANCE_PTR_T) new_obj->inst);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_Container_AddRuleInstanceObject(
    RULE_TYPE_CONTAINER_OBJECT_T *this,
    RULE_TYPE_RULE_INSTANCE_OBJECT_T *new_obj)
{
    return RULE_CTRL_IN_Priv_Container_AddInstance(this,
                                      (RULE_TYPE_INSTANCE_PTR_T) new_obj->inst);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_Container_RemoveClassInstanceObject(
    RULE_TYPE_CONTAINER_OBJECT_T *this,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *obj)
{
    return RULE_CTRL_IN_Priv_Container_RemoveInstance(this,
                                          (RULE_TYPE_INSTANCE_PTR_T) obj->inst);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_Container_RemoveRuleInstanceObject(
    RULE_TYPE_CONTAINER_OBJECT_T *this,
    RULE_TYPE_RULE_INSTANCE_OBJECT_T *obj)
{
    return RULE_CTRL_IN_Priv_Container_RemoveInstance(this,
                                          (RULE_TYPE_INSTANCE_PTR_T) obj->inst);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ClassInstanceElementIterator_UpdateInstance
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_INSTANCE_T *
RULE_CTRL_IN_Priv_ContainerElementIterator_UpdateInstance(
    RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);

    if (RULE_TYPE_NIL == this->curr_ptr.type)
    {
        this->curr_inst = NULL;
    }
    else
    {
        this->curr_inst = RULE_CTRL_IN_Ptr2Instance(this->curr_ptr);

        RULE_CTRL_IN_VALIDATE_PTR(this->curr_inst);
    }

    return this->curr_inst;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_INSTANCE_T *
RULE_CTRL_IN_Priv_ContainerElementIterator_First(
    RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    this->curr_idx = 0;

    this->curr_ptr = this->inst->array[this->curr_idx].ptr;

    return RULE_CTRL_IN_Priv_ContainerElementIterator_UpdateInstance(this);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_INSTANCE_T *
RULE_CTRL_IN_Priv_ContainerElementIterator_Last(
    RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    for (this->curr_idx = _countof(this->inst->array) - 1; 0 < this->curr_idx;  -- this->curr_idx)
    {
        if (RULE_TYPE_NIL != this->inst->array[this->curr_idx].ptr.type)
        {
            break;
        }
    }

    ASSERT(this->curr_idx < _countof(this->inst->array));

    this->curr_ptr = this->inst->array[this->curr_idx].ptr;

    return RULE_CTRL_IN_Priv_ContainerElementIterator_UpdateInstance(this);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_INSTANCE_T *
RULE_CTRL_IN_Priv_ContainerElementIterator_Next(
    RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    if (_countof(this->inst->array) <= ++ this->curr_idx)
    {
        this->curr_ptr.type = RULE_TYPE_NIL;
    }
    else
    {
        ASSERT(this->curr_idx < _countof(this->inst->array));

        this->curr_ptr = this->inst->array[this->curr_idx].ptr;
    }

    return RULE_CTRL_IN_Priv_ContainerElementIterator_UpdateInstance(this);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_INSTANCE_T *
RULE_CTRL_IN_Priv_ContainerElementIterator_Prev(
    RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    if (0 == this->curr_idx)
    {
        this->curr_ptr.type = RULE_TYPE_NIL;
    }
    else
    {
        ASSERT(this->curr_idx < _countof(this->inst->array));

        this->curr_ptr = this->inst->array[-- this->curr_idx].ptr;
    }

    return RULE_CTRL_IN_Priv_ContainerElementIterator_UpdateInstance(this);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_IN_Priv_ContainerElementIterator_EOF(
    RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    return (NULL == this->curr_inst) ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_IN_Priv_ContainerElementIterator_HasNext(
    RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    if (this->curr_idx == _countof(this->inst->array) - 1)
    {
        return FALSE;
    }

    return (RULE_TYPE_NIL != this->inst->array[this->curr_idx + 1].ptr.type) ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_IN_Priv_ContainerElementIterator_HasPrev(
    RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    if (this->curr_idx == 0)
    {
        return FALSE;
    }

    return (RULE_TYPE_NIL != this->inst->array[this->curr_idx - 1].ptr.type) ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_CTRL_IN_Priv_ContainerElementIterator_Count(
    RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this)
{
    RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T that;
    UI32_T count = 0;

    ASSERT(NULL != this);

    that = *this;

    for (that.first(&that); NULL != that.get_instance(&that); that.next(&that))
    {
        ++ count;
    }

    return count;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_SHM_POINTER_T
RULE_CTRL_IN_Priv_ContainerElementIterator_GetPtr(
    RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    return this->curr_ptr;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_INSTANCE_T *
RULE_CTRL_IN_Priv_ContainerElementIterator_GetInstance(
    RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    return this->curr_inst;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_Container_GetElementIterator(
    RULE_TYPE_CONTAINER_OBJECT_T *this,
    RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *iter)
{
    ASSERT(NULL != this);
    ASSERT(NULL != iter);

    RULE_CTRL_IN_VALIDATE_PTR(this->inst);

    memset(iter, 0, sizeof(*iter));

    iter->ptr           = this->ptr;
    iter->inst          = this->inst;
    iter->curr_ptr      = this->ptr;
    iter->curr_inst     = (RULE_TYPE_INSTANCE_T *) this->inst;

    iter->first         = RULE_CTRL_IN_Priv_ContainerElementIterator_First;
    iter->last          = RULE_CTRL_IN_Priv_ContainerElementIterator_Last;
    iter->next          = RULE_CTRL_IN_Priv_ContainerElementIterator_Next;
    iter->prev          = RULE_CTRL_IN_Priv_ContainerElementIterator_Prev;
    iter->eof           = RULE_CTRL_IN_Priv_ContainerElementIterator_EOF;
    iter->has_next      = RULE_CTRL_IN_Priv_ContainerElementIterator_HasNext;
    iter->has_prev      = RULE_CTRL_IN_Priv_ContainerElementIterator_HasPrev;
    iter->count         = RULE_CTRL_IN_Priv_ContainerElementIterator_Count;
    iter->get_ptr       = RULE_CTRL_IN_Priv_ContainerElementIterator_GetPtr;
    iter->get_instance  = RULE_CTRL_IN_Priv_ContainerElementIterator_GetInstance;

    iter->first(iter);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructContainerObjectFromPointer
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct class instance object from pointer of instance entry
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructContainerObjectFromPointer(
    RULE_TYPE_SHM_POINTER_T ptr,
    RULE_TYPE_CONTAINER_OBJECT_T *out)
{
    ASSERT(RULE_TYPE_NIL != ptr.type);

    out->ptr = ptr;
    out->inst = RULE_CTRL_IN_Ptr2Instance(out->ptr);

    RULE_CTRL_IN_VALIDATE_PTR(out->inst);

    out->add                    = RULE_CTRL_IN_Priv_Container_AddInstance;
    out->remove                 = RULE_CTRL_IN_Priv_Container_RemoveInstance;
    out->add_class_obj          = RULE_CTRL_IN_Priv_Container_AddClassInstanceObject;
    out->add_rule_obj           = RULE_CTRL_IN_Priv_Container_AddRuleInstanceObject;
    out->remove_class_obj       = RULE_CTRL_IN_Priv_Container_RemoveClassInstanceObject;
    out->remove_rule_obj        = RULE_CTRL_IN_Priv_Container_RemoveRuleInstanceObject;
    out->get_element_iterator   = RULE_CTRL_IN_Priv_Container_GetElementIterator;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructContainerObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct container object from class instance
 * INPUT    : in    - source container
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructContainerObject(
    const RULE_TYPE_CONTAINER_T *in,
    RULE_TYPE_CONTAINER_OBJECT_T *out)
{
    RULE_TYPE_SHM_POINTER_T ptr = {RULE_TYPE_NIL, 0};

    ASSERT(NULL != in);

    RULE_CTRL_IN_VALIDATE_PTR(in);

    ptr.type = in->super.type;
    ptr.offset = L_CVRT_GET_OFFSET(RULE_CTRL_IN_BaseAddress(in->super.type), in);

    RULE_CTRL_IN_Priv_ConstructContainerObjectFromPointer(ptr, out);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateContainerObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Create container object
 * INPUT    : type  - type of instance
 * OUTPUT   : out   - instance object
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_CreateContainerObject(
    RULE_TYPE_CONTAINER_OBJECT_T *out)
{
    RULE_TYPE_SHM_POINTER_T ptr;

    ASSERT(NULL != out);

    memset(out, 0, sizeof(*out));

    ptr = RULE_CTRL_IN_Priv_CreateContainer();
    if (RULE_TYPE_NIL == ptr.type)
    {
        return RULE_TYPE_FAIL;
    }

    RULE_CTRL_IN_Priv_ConstructContainerObjectFromPointer(ptr, out);
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyContainerObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy container
 * INPUT    : this  - instance object
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : All child instance also be destroied
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_DestroyContainerObject(
    RULE_TYPE_CONTAINER_OBJECT_T *this)
{
    ASSERT(NULL != this);

    RULE_CTRL_IN_Priv_DestroyContainer(this->ptr);
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ClassInstanceElementIterator_UpdateInstance
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_INSTANCE_T *
RULE_CTRL_IN_Priv_ClassInstanceElementIterator_UpdateInstance(
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);

    if (RULE_TYPE_NIL == this->curr_ptr.type)
    {
        this->curr_inst = NULL;
    }
    else
    {
        this->curr_inst = RULE_CTRL_IN_Ptr2Instance(this->curr_ptr);

        RULE_CTRL_IN_VALIDATE_PTR(this->curr_inst);
    }

    return this->curr_inst;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_INSTANCE_T *
RULE_CTRL_IN_Priv_ClassInstanceElementIterator_First(
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    this->curr_ptr = this->inst->super.links.first_node;

    return RULE_CTRL_IN_Priv_ClassInstanceElementIterator_UpdateInstance(this);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_INSTANCE_T *
RULE_CTRL_IN_Priv_ClassInstanceElementIterator_Last(
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    this->curr_ptr = this->inst->super.links.last_node;

    return RULE_CTRL_IN_Priv_ClassInstanceElementIterator_UpdateInstance(this);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_INSTANCE_T *
RULE_CTRL_IN_Priv_ClassInstanceElementIterator_Next(
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    this->curr_ptr = this->curr_inst->links.next;

    return RULE_CTRL_IN_Priv_ClassInstanceElementIterator_UpdateInstance(this);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_INSTANCE_T *
RULE_CTRL_IN_Priv_ClassInstanceElementIterator_Prev(
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    this->curr_ptr = this->curr_inst->links.prev;

    return RULE_CTRL_IN_Priv_ClassInstanceElementIterator_UpdateInstance(this);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_IN_Priv_ClassInstanceElementIterator_EOF(
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    return (NULL == this->curr_inst) ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_IN_Priv_ClassInstanceElementIterator_HasNext(
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    return (this->curr_inst && RULE_TYPE_NIL != this->curr_inst->links.next.type) ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_IN_Priv_ClassInstanceElementIterator_HasPrev(
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    return (this->curr_inst && RULE_TYPE_NIL != this->curr_inst->links.prev.type) ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_CTRL_IN_Priv_ClassInstanceElementIterator_Count(
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this)
{
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T that;
    UI32_T count = 0;

    ASSERT(NULL != this);

    that = *this;

    for (that.first(&that); NULL != that.get_instance(&that); that.next(&that))
    {
        ++ count;
    }

    return count;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_SHM_POINTER_T
RULE_CTRL_IN_Priv_ClassInstanceElementIterator_GetPtr(
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    return this->curr_ptr;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_INSTANCE_T *
RULE_CTRL_IN_Priv_ClassInstanceElementIterator_GetInstance(
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this)
{
    ASSERT(NULL != this);
    ASSERT(NULL != this->inst);

    return this->curr_inst;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_ClassInstance_AddInstance(
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this,
    const RULE_TYPE_INSTANCE_PTR_T in)
{
    ASSERT(NULL != this);
    ASSERT(NULL != in);

    RULE_CTRL_IN_LL_Priv_InsertEnd(this->ptr, RULE_CTRL_IN_Instance2Ptr(in));
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_ClassInstance_RemoveInstance(
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this,
    const RULE_TYPE_INSTANCE_PTR_T in)
{
    ASSERT(NULL != this);
    ASSERT(NULL != in);

    RULE_CTRL_IN_LL_Priv_Remove(this->ptr, RULE_CTRL_IN_Instance2Ptr(in));
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_AddClassInstanceObject(
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *new_obj)
{
    return RULE_CTRL_IN_Priv_ClassInstance_AddInstance(this,
                                                       (RULE_TYPE_INSTANCE_PTR_T) new_obj->inst);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_AddRuleInstanceObject(
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this,
    RULE_TYPE_RULE_INSTANCE_OBJECT_T *new_obj)
{
    return RULE_CTRL_IN_Priv_ClassInstance_AddInstance(this,
                                                       (RULE_TYPE_INSTANCE_PTR_T) new_obj->inst);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_RemoveClassInstanceObject(
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *obj)
{
    return RULE_CTRL_IN_Priv_ClassInstance_RemoveInstance(this,
                                                          (RULE_TYPE_INSTANCE_PTR_T) obj->inst);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_RemoveRuleInstanceObject(
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this,
    RULE_TYPE_RULE_INSTANCE_OBJECT_T *obj)
{
    return RULE_CTRL_IN_Priv_ClassInstance_RemoveInstance(this,
                                                          (RULE_TYPE_INSTANCE_PTR_T) obj->inst);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_GetElementIterator(
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this,
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *iter)
{
    ASSERT(NULL != this);
    ASSERT(NULL != iter);

    RULE_CTRL_IN_VALIDATE_PTR(this->inst);

    memset(iter, 0, sizeof(*iter));

    iter->ptr           = this->ptr;
    iter->inst          = this->inst;
    iter->curr_ptr      = this->ptr;
    iter->curr_inst     = (RULE_TYPE_INSTANCE_T *) this->inst;

    iter->first         = RULE_CTRL_IN_Priv_ClassInstanceElementIterator_First;
    iter->last          = RULE_CTRL_IN_Priv_ClassInstanceElementIterator_Last;
    iter->next          = RULE_CTRL_IN_Priv_ClassInstanceElementIterator_Next;
    iter->prev          = RULE_CTRL_IN_Priv_ClassInstanceElementIterator_Prev;
    iter->eof           = RULE_CTRL_IN_Priv_ClassInstanceElementIterator_EOF;
    iter->has_next      = RULE_CTRL_IN_Priv_ClassInstanceElementIterator_HasNext;
    iter->has_prev      = RULE_CTRL_IN_Priv_ClassInstanceElementIterator_HasPrev;
    iter->count         = RULE_CTRL_IN_Priv_ClassInstanceElementIterator_Count;
    iter->get_ptr       = RULE_CTRL_IN_Priv_ClassInstanceElementIterator_GetPtr;
    iter->get_instance  = RULE_CTRL_IN_Priv_ClassInstanceElementIterator_GetInstance;

    iter->first(iter);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct class instance object from pointer of instance entry
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(
    RULE_TYPE_SHM_POINTER_T ptr,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *out)
{
    ASSERT(RULE_TYPE_NIL != ptr.type);

    out->ptr = ptr;
    out->inst = RULE_CTRL_IN_Ptr2Instance(out->ptr);

    RULE_CTRL_IN_VALIDATE_CLASS_INST(out->inst);

    out->add                    = RULE_CTRL_IN_Priv_ClassInstance_AddInstance;
    out->remove                 = RULE_CTRL_IN_Priv_ClassInstance_RemoveInstance;
    out->add_class_obj          = RULE_CTRL_IN_Priv_AddClassInstanceObject;
    out->add_rule_obj           = RULE_CTRL_IN_Priv_AddRuleInstanceObject;
    out->remove_class_obj       = RULE_CTRL_IN_Priv_RemoveClassInstanceObject;
    out->remove_rule_obj        = RULE_CTRL_IN_Priv_RemoveRuleInstanceObject;
    out->get_element_iterator   = RULE_CTRL_IN_Priv_GetElementIterator;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructClassInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct class instance object from class instance
 * INPUT    : in    - source class instance
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructClassInstanceObject(
    const RULE_TYPE_CLASS_INSTANCE_T *in,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *out)
{
    RULE_TYPE_SHM_POINTER_T ptr = {RULE_TYPE_NIL, 0};

    ASSERT(NULL != in);

    RULE_CTRL_IN_VALIDATE_CLASS_INST(in);

    ptr.type = in->super.type;
    ptr.offset = L_CVRT_GET_OFFSET(RULE_CTRL_IN_BaseAddress(in->super.type), in);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(ptr, out);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateClassInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Create class instance object
 * INPUT    : type  - type of instance
 * OUTPUT   : out   - instance object
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_CreateClassInstanceObject(
    RULE_TYPE_INSTANCE_TYPE_T type,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *out)
{
    RULE_TYPE_SHM_POINTER_T ptr;

    ASSERT(NULL != out);

    memset(out, 0, sizeof(*out));

    ptr = RULE_CTRL_IN_Priv_CreateClassInstance(type);
    if (RULE_TYPE_NIL == ptr.type)
    {
        return RULE_TYPE_FAIL;
    }

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(ptr, out);
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyClassInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy class instance
 * INPUT    : this  - instance object
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : All child instance also be destroied
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_DestroyClassInstanceObject(
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this)
{
    ASSERT(NULL != this);

    RULE_CTRL_IN_Priv_DestroyClassInstance(this->ptr);
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructRuleInstanceObjectFromPointer
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct rule instance object from pointer of instance entry
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructRuleInstanceObjectFromPointer(
    RULE_TYPE_SHM_POINTER_T ptr,
    RULE_TYPE_RULE_INSTANCE_OBJECT_T *out)
{
    ASSERT(RULE_TYPE_NIL != ptr.type);

    out->ptr = ptr;
    out->inst = RULE_CTRL_IN_Ptr2Instance(out->ptr);

    RULE_CTRL_IN_VALIDATE_RULE_INST(out->inst);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructRuleInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct rule instance object from rule instance
 * INPUT    : in    - source rule instance
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructRuleInstanceObject(
    RULE_TYPE_RULE_INSTANCE_T *in,
    RULE_TYPE_RULE_INSTANCE_OBJECT_T *out)
{
    RULE_TYPE_SHM_POINTER_T ptr = {RULE_TYPE_NIL, 0};

    ASSERT(NULL != in);

    RULE_CTRL_IN_VALIDATE_PTR(in);

    ptr.type = in->super.type;
    ptr.offset = L_CVRT_GET_OFFSET(RULE_CTRL_IN_BaseAddress(in->super.type), in);

    RULE_CTRL_IN_Priv_ConstructRuleInstanceObjectFromPointer(ptr, out);
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateRuleInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Create rule instance object
 * INPUT    : type  - type of instance
 * OUTPUT   : out   - instance object
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_CreateRuleInstanceObject(
    RULE_TYPE_INSTANCE_TYPE_T type,
    RULE_TYPE_RULE_INSTANCE_OBJECT_T *out)
{
    RULE_TYPE_SHM_POINTER_T ptr;

    ASSERT(NULL != out);

    memset(out, 0, sizeof(*out));

    ptr = RULE_CTRL_IN_Priv_CreateRuleInstance(type);
    if (RULE_TYPE_NIL == ptr.type)
    {
        return RULE_TYPE_FAIL;
    }

    RULE_CTRL_IN_Priv_ConstructRuleInstanceObjectFromPointer(ptr, out);

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyRuleInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy rule instance
 * INPUT    : this  - instance object
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_DestroyRuleInstanceObject(
    RULE_TYPE_RULE_INSTANCE_OBJECT_T *this)
{
    ASSERT(NULL != this);

    RULE_CTRL_IN_Priv_DestroyRuleInstance(this->ptr);
    return RULE_TYPE_OK;
}

//
// Help Functions
//

//
// RULE_CTRL_IN_Priv_Walk
//
static void
RULE_CTRL_IN_Priv_Walk(
    RULE_TYPE_INSTANCE_PTR_T in,
    void(*fn)(RULE_TYPE_INSTANCE_PTR_T, void*),
    void *cooike)
{
    ASSERT(NULL != in);

    if (TRUE == RULE_CTRL_IS_CONTAINER_TYPE(in->type))
    {
        RULE_TYPE_CONTAINER_OBJECT_T obj;
        RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T iter;

        RULE_CTRL_IN_Priv_ConstructContainerObject((RULE_TYPE_CONTAINER_PTR_T) in, &obj);
        obj.get_element_iterator(&obj, &iter);

        for (iter.first(&iter); NULL != iter.get_instance(&iter); iter.next(&iter))
        {
            RULE_TYPE_INSTANCE_PTR_T child;

            child = iter.get_instance(&iter);

            RULE_CTRL_IN_Priv_Walk(child, fn, cooike);
        }
    }
    else if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(in->type))
    {
        RULE_TYPE_CLASS_INSTANCE_OBJECT_T obj;
        RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T iter;

        RULE_CTRL_IN_Priv_ConstructClassInstanceObject((RULE_TYPE_CLASS_INSTANCE_PTR_T) in, &obj);
        obj.get_element_iterator(&obj, &iter);

        for (iter.first(&iter); NULL != iter.get_instance(&iter); iter.next(&iter))
        {
            RULE_TYPE_INSTANCE_PTR_T child;

            child = iter.get_instance(&iter);

            RULE_CTRL_IN_Priv_Walk(child, fn, cooike);
        }
    }
    else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(in->type))
    {
    }
    else
    {
        ASSERT(0);
    }

    fn(in, cooike);
}

//
// RULE_CTRL_IN_Priv_FindIf
//
typedef struct
{
    BOOL_T (*fn) (RULE_TYPE_INSTANCE_PTR_T, void *);
    void *fn_param;

    RULE_TYPE_INSTANCE_PTR_T result;
} RULE_CTRL_FIND_IF_PARAM_T, *RULE_CTRL_FIND_IF_PARAM_PTR_T;

static void
RULE_CTRL_IN_Priv_FindIf_CallBack(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie)
{
    RULE_CTRL_FIND_IF_PARAM_PTR_T param = cookie;
    BOOL_T b_result;

    if (param->result)
    {
        return;
    }

    b_result = param->fn(in, param->fn_param);
    if (TRUE == b_result)
    {
        param->result = in;
    }
}

static RULE_TYPE_INSTANCE_PTR_T
RULE_CTRL_IN_Priv_FindIf(
    RULE_TYPE_INSTANCE_PTR_T in,
    BOOL_T (*fn)(RULE_TYPE_INSTANCE_PTR_T, void*),
    void *cookie)
{
    RULE_CTRL_FIND_IF_PARAM_T param = {0};

    param.fn = fn;
    param.fn_param = cookie;

    RULE_CTRL_IN_Priv_Walk(in, RULE_CTRL_IN_Priv_FindIf_CallBack, &param);

    return param.result;
}

//
// RULE_CTRL_IN_Priv_CountIf
//
typedef struct
{
    BOOL_T (*fn) (RULE_TYPE_INSTANCE_PTR_T, void *);
    void *fn_param;

    UI32_T result;
} RULE_CTRL_COUNT_IF_PARAM_T, *RULE_CTRL_COUNT_IF_PARAM_PTR_T;

static void
RULE_CTRL_IN_Priv_CountIf_CallBack(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie)
{
    RULE_CTRL_COUNT_IF_PARAM_PTR_T param = cookie;
    BOOL_T b_result;

    b_result = param->fn(in, param->fn_param);
    if (TRUE == b_result)
    {
        param->result += 1;
    }
}

static UI32_T
RULE_CTRL_IN_Priv_CountIf(
    RULE_TYPE_INSTANCE_PTR_T in,
    BOOL_T (*fn)(RULE_TYPE_INSTANCE_PTR_T, void*),
    void *cookie)
{
    RULE_CTRL_COUNT_IF_PARAM_T param = {0};

    param.fn = fn;
    param.fn_param = cookie;

    RULE_CTRL_IN_Priv_Walk(in, RULE_CTRL_IN_Priv_CountIf_CallBack, &param);

    return param.result;
}

#pragma mark End of Instance Functions
#pragma mark -

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalAllocateAndSetRule
 *------------------------------------------------------------------------------
 * PURPOSE  : install a logical rule for QoS ace to ASIC.
 *
 * INPUT    :         unit:
 *             selector_id:
 *                 rule_id: logical rule id to install.
 *               ace_entry:
 *             meter_entry:
 *            action_entry:
 *                  sm_rid: share meter id with this logical rule id.
 *
 * OUTPUT   : None
 *
 * RETURN   : TRUE/FALSE
 *
 * NOTE     : if sm_rid == RULE_CTRL_NO_SHARE_METER_ENTRY_ID
 *            we need to create a new meter for rule_id
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAllocateAndSetRule(
    UI32_T unit,
    UI32_T device_id,
    const RULE_CTRL_PCL_CFG_T *pcl_cfg_p,
    UI32_T vir_group_id,
    int rule_priority,
    UI32_T dev_rule_id,
    const DEVRM_AceEntry_T *rule_pattern_p,
    const DEVRM_ACTION_T *dev_action_p)
{
    DEVRM_AceEntry_T dev_ace_entry;
    BOOL_T b_result;
    UI32_T port = pcl_cfg_p->interface.uport.port;

    ASSERT(NULL != pcl_cfg_p);
    ASSERT(NULL != rule_pattern_p);

    memcpy(&dev_ace_entry, rule_pattern_p, sizeof(dev_ace_entry));

    {
        dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_PCL_ID;
        dev_ace_entry.pcl_id = pcl_cfg_p->pcl_id & pcl_cfg_p->pcl_id_mask;
        dev_ace_entry.pcl_id_mask = pcl_cfg_p->pcl_id_mask;
    }

    if (FALSE == RULE_CTRL_LocalSetRule(unit, port, device_id,
                                        pcl_cfg_p->pcl_id,
                                        vir_group_id,
                                        rule_priority,
                                        dev_rule_id,
                                        &dev_ace_entry,
                                        dev_action_p))
    {
        LOG("RULE_CTRL_LocalSetRule filed.");
        goto undo;
    }

    b_result = RULE_CTRL_Priv_CreateIfRuleRererence(unit, device_id,
                                                    (&pcl_cfg_p->interface),
                                                    pcl_cfg_p->lookup_num,
                                                    pcl_cfg_p->pcl_id);
    if (TRUE != b_result)
    {
        LOG("Update rule reference counter fail.");
        goto undo;
    }

    return RULE_TYPE_OK;

undo:

    return RULE_TYPE_FAIL;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertMeter2Dev(
    const RULE_TYPE_TBParamEntry_T *meter_entry,
    DEVRM_ACTION_T *dev_action_p)
{
    switch (meter_entry->meter_model)
    {
        case RULE_TYPE_METER_MODE_DEFAULT:
        case RULE_TYPE_METER_MODE_FLOW:
            if (TRUE != DEVRM_LIB_ActionMeterConfigFlow(dev_action_p,
                                                        meter_entry->rate,
                                                        meter_entry->burst_size))
            {
                LOG("<Error> config meter (flow)");
                return FALSE;
            }

            break;

        case RULE_TYPE_METER_MODE_SRTCM_COLOR_BLIND:
            if (TRUE != DEVRM_LIB_ActionMeterConfigSrTcm(dev_action_p,
                                                         DEVRM_POLICER_METER_COLOR_BLIND,
                                                         meter_entry->rate,
                                                         meter_entry->burst_size,
                                                         meter_entry->peak_burst_size))
            {
                LOG("<Error> config meter (srtcm-blind)");
                return FALSE;
            }

            break;

        case RULE_TYPE_METER_MODE_SRTCM_COLOR_AWARE:
            if (TRUE != DEVRM_LIB_ActionMeterConfigSrTcm(dev_action_p,
                                                         DEVRM_POLICER_METER_COLOR_AWARE,
                                                         meter_entry->rate,
                                                         meter_entry->burst_size,
                                                         meter_entry->peak_burst_size))
            {
                LOG("<Error> config meter (srtcm-aware)");
                return FALSE;
            }

            break;

        case RULE_TYPE_METER_MODE_TRTCM_COLOR_BLIND:
            if (TRUE != DEVRM_LIB_ActionMeterConfigTrTcm(dev_action_p,
                                                         DEVRM_POLICER_METER_COLOR_BLIND,
                                                         meter_entry->rate,
                                                         meter_entry->burst_size,
                                                         meter_entry->peak_rate,
                                                         meter_entry->peak_burst_size))
            {
                LOG("<Error> config meter (trtcm-blind)");
                return FALSE;
            }

            break;

        case RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE:
            if (TRUE != DEVRM_LIB_ActionMeterConfigTrTcm(dev_action_p,
                                                         DEVRM_POLICER_METER_COLOR_AWARE,
                                                         meter_entry->rate,
                                                         meter_entry->burst_size,
                                                         meter_entry->peak_rate,
                                                         meter_entry->peak_burst_size))
            {
                LOG("<Error> config meter (trtcm-aware)");
                return FALSE;
            }

            break;

        default:
            ASSERT(FALSE);
            break;
    }

#if (SYS_CPNT_QOS_V2_COUNTER != TRUE)
    if (TRUE != DEVRM_LIB_ActionCounterConfig(dev_action_p, DEVRM_POLICER_COUNTER_MODE_BYTE))
    {
        LOG("<Error> config counter");
        return FALSE;
    }
#endif	

    return TRUE;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Ifindex_To_Interface(
    UI32_T ifindex,
    RULE_TYPE_INTERFACE_INFO_T *interface_p)
{
    ASSERT(NULL != interface_p);

#define TOTAL_NBR_OF_ETHER (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK * SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT)

    if (SYS_ADPT_ETHER_1_IF_INDEX_NUMBER <= ifindex &&
        ifindex <= (SYS_ADPT_ETHER_1_IF_INDEX_NUMBER + TOTAL_NBR_OF_ETHER - 1))
    {
        interface_p->type = RULE_TYPE_INTERFACE_UPORT;
        interface_p->uport.unit = STKTPLG_OM_IFINDEX_TO_UNIT(ifindex);
        interface_p->uport.port = STKTPLG_OM_IFINDEX_TO_PORT(ifindex);
    }
    else if (SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER <= ifindex &&
             ifindex <= (SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER + SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM - 1))
    {
        interface_p->type = RULE_TYPE_INTERFACE_TRUNK;
        interface_p->trunk_id = STKTPLG_OM_IFINDEX_TO_TRUNKID(ifindex);
    }
    else if (SYS_ADPT_CPU_1_IF_INDEX_NUMBER == ifindex)
    {
        interface_p->type = RULE_TYPE_INTERFACE_CPU_PORT;
    }
#if (RULE_CTRL_HAVE_CRAFT_PORT == TRUE)
    else if (SYS_ADPT_CRAFT_INTERFACE_IFINDEX == ifindex)
    {
        interface_p->type = RULE_TYPE_INTERFACE_CRAFT_PORT;
    }
#endif /* RULE_CTRL_HAVE_CRAFT_PORT */
    else if (RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER == ifindex)
    {
        interface_p->type = RULE_TYPE_INTERFACE_ALL_UNIT;
    }
    else
    {
        ASSERT(0);
        return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;

#undef TOTAL_NBR_OF_ETHER
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Interface_To_Ifindex(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T *ifindex_p)
{
    ASSERT(NULL != interface_p);

    switch (interface_p->type)
    {
        case RULE_TYPE_INTERFACE_UPORT:
            *ifindex_p = STKTPLG_OM_UPORT_TO_IFINDEX(interface_p->uport.unit,
                                                     interface_p->uport.port);
            break;

        case RULE_TYPE_INTERFACE_TRUNK:
            *ifindex_p = STKTPLG_OM_TRUNKID_TO_IFINDEX(interface_p->trunk_id);
            break;

        case RULE_TYPE_INTERFACE_CPU_PORT:
            *ifindex_p = SYS_ADPT_CPU_1_IF_INDEX_NUMBER;
            break;

#if (RULE_CTRL_HAVE_CRAFT_PORT == TRUE)
        case RULE_TYPE_INTERFACE_CRAFT_PORT:
            *ifindex_p = SYS_ADPT_CRAFT_INTERFACE_IFINDEX;
            break;
#endif /* RULE_CTRL_HAVE_CRAFT_PORT */

        case RULE_TYPE_INTERFACE_ALL_UNIT:
            *ifindex_p = RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER;
            break;

        default:
            ASSERT(0);
            *ifindex_p = 0xffffffff;
            return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalConvertAce2Dev
 *------------------------------------------------------------------------------
 * PURPOSE  : convert an ace of QoS to a dev ace
 * INPUT    : unit, port, ace_entry
 * OUTPUT   : dev_ace_entry
 * RETURN   : TRUE/FALSE.
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertAce2Dev(
    const RULE_TYPE_Ace_Entry_T *ace_entry_p,
    DEVRM_AceEntry_T *dev_ace_entry_p)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_Ace_Entry_T temp_ace_entry;

    ASSERT(NULL != ace_entry_p);
    ASSERT(NULL != dev_ace_entry_p);

    memset(dev_ace_entry_p, 0, sizeof(*dev_ace_entry_p));
    memcpy(&temp_ace_entry, ace_entry_p, sizeof(*ace_entry_p));

    /* FIXEME: Need check this is neccessary for all marvell chip, or add SYS_CPNT ?
     *
     * Set data = data & mask
     *   This is because some marvell chip  doesn't auto mask the value,
     *   reference to  EPR [ES3526MAW-HPoE-7LN-00048]
     */
    RULE_CTRL_Priv_MaskAceEntry(&temp_ace_entry);

    switch (temp_ace_entry.ace_type)
    {
        case RULE_TYPE_MAC_ACL:
            result = RULE_CTRL_LocalConvertMacAce2DevAce(&temp_ace_entry.u.mac, dev_ace_entry_p);
            if (RULE_TYPE_OK != result)
            {
                break;
            }

#if (RULE_TYPE_ACE_MAC_IP_MIX == TRUE)
            if (temp_ace_entry.u.mac.aceEtherTypeOp == VAL_diffServMacAceEtherTypeOp_equal &&
                temp_ace_entry.u.mac.aceEtherTypeBitmask == MAX_diffServMacAceEtherTypeBitmask)
            {
                switch (temp_ace_entry.u.mac.aceMinEtherType)
                {
                    case RULE_TYPE_ETHERTYPE_IP:
                        result = RULE_CTRL_LocalConvertIpAce2DevAce(&temp_ace_entry.u.ip, dev_ace_entry_p);
                        break;
                    case RULE_TYPE_ETHERTYPE_IPV6:
                        result = RULE_CTRL_LocalConvertIpv6Ace2DevAce(&temp_ace_entry.u.ipv6, dev_ace_entry_p);
                        break;
                    default:
                        break;
                }
            }
#endif /* #if (RULE_TYPE_ACE_MAC_IP_MIX == TRUE) */

            break;

        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
#if (RULE_TYPE_ACE_MAC_IP_MIX == TRUE)
            result = RULE_CTRL_LocalConvertMacAce2DevAce(&temp_ace_entry.u.mac, dev_ace_entry_p);
            if (RULE_TYPE_OK != result)
            {
                break;
            }
#endif /* RULE_TYPE_ACE_MAC_IP_MIX */

            result = RULE_CTRL_LocalConvertIpAce2DevAce(&temp_ace_entry.u.ip, dev_ace_entry_p);

            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
#if (RULE_TYPE_ACE_MAC_IP_MIX == TRUE)
            result = RULE_CTRL_LocalConvertMacAce2DevAce(&temp_ace_entry.u.mac, dev_ace_entry_p);
            if (RULE_TYPE_OK != result)
            {
                break;
            }
#endif /* RULE_TYPE_ACE_MAC_IP_MIX */

            result = RULE_CTRL_LocalConvertIpv6Ace2DevAce(&temp_ace_entry.u.ipv6, dev_ace_entry_p);
            break;

        default:
            LOG("Invalid parameter ace type = %lu", temp_ace_entry.ace_type);
            ASSERT(FALSE);
            return RULE_TYPE_INVALID_PARAMETER;
    }

    return result;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalConvertMacAce2DevAce
 *------------------------------------------------------------------------------
 * PURPOSE  : convert mac ace to dev ace
 * INPUT    : mac_ace, dev_ace
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertMacAce2DevAce(
    const RULE_TYPE_MacAce_Entry_T *mac_ace,
    DEVRM_AceEntry_T *dev_ace)
{
    ASSERT(NULL != mac_ace);
    ASSERT(NULL != dev_ace);

    if ((NULL == mac_ace) || (NULL == dev_ace))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    switch (mac_ace->acePktformat)
    {
    case VAL_aclMacAcePktformat_any:
/*pttch mark*/
/*
        RULE_CTRL_ADD_FILTER_PKTFORMAT(dev_ace, 0, 0);
*/
        break;

    case VAL_aclMacAcePktformat_untagged_Eth2:
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_TAGGED;
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ENCAP;
        dev_ace->packetformat_data = ETHERTYPE_ENCAPS;
        dev_ace->is_tagged = FALSE;
        break;

    case VAL_aclMacAcePktformat_untagged802Dot3:
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_TAGGED;
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ENCAP;
        dev_ace->packetformat_data = ~ETHERTYPE_ENCAPS;
        dev_ace->is_tagged = FALSE;
        break;

    case VAL_aclMacAcePktformat_tagggedEth2:
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_TAGGED;
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ENCAP;
        dev_ace->packetformat_data = ETHERTYPE_ENCAPS;
        dev_ace->is_tagged = TRUE;
        break;

    case VAL_aclMacAcePktformat_tagged802Dot3:
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_TAGGED;
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ENCAP;
        dev_ace->packetformat_data = ~ETHERTYPE_ENCAPS;
        dev_ace->is_tagged = TRUE;
        break;

    default:
        break; /* ignore this field */
    }

    if (FALSE == RULE_CTRL_LocalIsNullMac(mac_ace->aceSourceMacAddrBitmask))
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_MACSA;
        memcpy(dev_ace->srcmac_data, mac_ace->aceSourceMacAddr, SYS_ADPT_MAC_ADDR_LEN);
        memcpy(dev_ace->srcmac_mask, mac_ace->aceSourceMacAddrBitmask, SYS_ADPT_MAC_ADDR_LEN);
    }

    if (FALSE == RULE_CTRL_LocalIsNullMac(mac_ace->aceDestMacAddrBitmask))
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_MACDA;
        memcpy(dev_ace->dstmac_data, mac_ace->aceDestMacAddr, SYS_ADPT_MAC_ADDR_LEN);
        memcpy(dev_ace->dstmac_mask, mac_ace->aceDestMacAddrBitmask, SYS_ADPT_MAC_ADDR_LEN);
    }

    if (mac_ace->aceVidOp == VAL_diffServMacAceVidOp_equal)
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_VID;
        dev_ace->outvlan_data = mac_ace->aceMinVid;
        dev_ace->outvlan_mask = mac_ace->aceVidBitmask;
/*pttch ??? marvell use in vlan*/
        dev_ace->invlan_data = mac_ace->aceMinVid;
        dev_ace->invlan_mask = mac_ace->aceVidBitmask;

    }

    if (mac_ace->aceEtherTypeOp == VAL_diffServMacAceEtherTypeOp_equal)
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ETHER;
        dev_ace->ethertype_data = mac_ace->aceMinEtherType;
        dev_ace->ethertype_mask = mac_ace->aceEtherTypeBitmask;
    }

    if (mac_ace->aceCosOp == VAL_diffServMacAceCosOp_equal)
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_VTAGUP;
        dev_ace->vtagup_data    = mac_ace->aceMinCos;
        dev_ace->vtagup_mask    = mac_ace->aceCosBitmask;

        dev_ace->is_tagged = 1;
    }

    if (mac_ace->aceSourcePortOp == VAL_diffServMacAceSourcePortOp_equal)
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_SRCPORT;
        dev_ace->srcport_data   = mac_ace->aceMinSourcePort;
        dev_ace->srcport_mask   = mac_ace->aceSourcePortBitmask;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalConvertIpAce2DevAce
 *------------------------------------------------------------------------------
 * PURPOSE  : convert ip ace to dev ace
 * INPUT    : ip_ace, dev_ace
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertIpAce2DevAce(
    const RULE_TYPE_IpAce_Entry_T *ip_ace,
    DEVRM_AceEntry_T *dev_ace)
{
    enum
    {
        RULE_CTRL_DSCP_MASK = 63
    };

    ASSERT(NULL != ip_ace);
    ASSERT(NULL != dev_ace);

    /* ES3528MO-FLF-38-00190
     * Problem : DUT reject all packet when bind a IP ACL with deny host 0.0.0.0 to port.
     */
    if (ip_ace->aceSourceIpAddrBitmask != RULE_TYPE_ACL_IP_BITMASK_ANY)
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifyIpSourceAddress(dev_ace,
                                                                ip_ace->aceSourceIpAddr,
                                                                ip_ace->aceSourceIpAddrBitmask))
        {
            LOG("<Error> Qualify IP source address");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    if (ip_ace->aceDestIpAddrBitmask != RULE_TYPE_ACL_IP_BITMASK_ANY)
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifyIpDestAddress(dev_ace,
                                                              ip_ace->aceDestIpAddr,
                                                              ip_ace->aceDestIpAddrBitmask))
        {
            LOG("<Error> Qualify IP dest address");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    if (ip_ace->aceProtocol != RULE_TYPE_UNDEF_IP_PROTOCOL)
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifyIpProtocol(dev_ace, ip_ace->aceProtocol, 0xff))
        {
            LOG("<Error> Qualify IP protocol");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    if (ip_ace->acePrec != RULE_TYPE_UNDEF_IP_PRECEDENCE)
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_PREC;
        dev_ace->prec_data = (UI8_T)ip_ace->acePrec;
        dev_ace->prec_mask = 0x07;
    }

    /** We use DSCP to enmulate TOS field. But, Marvell chip, DSCP field
     ** only have 6 bits. Bit 7~bit 2 and bit 0 is LSB.
     ** So TOS will not support on Marvell chip now.
    if (ip_ace->aceTos != RULE_TYPE_UNDEF_IP_TOS)
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_TOS;
        dev_ace->tos_data = (UI8_T)ip_ace->aceTos;
        dev_ace->tos_mask = 0x0f;
    }
     **/

    if (ip_ace->aceDscp != RULE_TYPE_UNDEF_IP_DSCP)
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifyIpDscp(dev_ace, ip_ace->aceDscp, RULE_CTRL_DSCP_MASK))
        {
            LOG("<Error> Qualify IP DSCP");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    if (ip_ace->aceSourcePortOp == VAL_diffServIpAceSourcePortOp_equal) /* currently only support this operator */
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifyL4SourcePort(dev_ace,
                                                             ip_ace->aceMinSourcePort,
                                                             ip_ace->aceSourcePortBitmask))
        {
            LOG("<Error> Qualify L4 source port");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    if (ip_ace->aceDestPortOp == VAL_diffServIpAceDestPortOp_equal) /* currently only support this operator */
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifyL4DestPort(dev_ace,
                                                           ip_ace->aceMinDestPort,
                                                           ip_ace->aceDestPortBitmask))
        {
            LOG("<Error> Qualify L4 dest port");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    if (ip_ace->aceControlCodeBitmask != 0)
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_TCPCONTROL;
        dev_ace->tcpcontrol_data = ip_ace->aceControlCode;
        dev_ace->tcpcontrol_mask = ip_ace->aceControlCodeBitmask;
    }

    if (ip_ace->aceIcmpType != RULE_TYPE_UNDEF_ICMP_TYPE)
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ICMPTYPE;
        dev_ace->icmptype_data = ip_ace->aceIcmpType;
        dev_ace->icmptype_mask = 0xff;
    }

    /* match IP packet only if keymode is RULE_TYPE_IP_EXT_ACL
     */
    if (shmem_data_p->global_tcam_key_mode == SYS_CPNT_TCAM_SEARCH_KEY_MODE_USE_MAC_IPV4_IPV6_MODE6_ONLY)
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ETHER;
        dev_ace->ethertype_data = RULE_TYPE_ETHERTYPE_IP;
        dev_ace->ethertype_mask = 0xffff;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalConvertIpv6Ace2DevAce
 *------------------------------------------------------------------------------
 * PURPOSE  : convert ipv6 ace to dev ace
 * INPUT    : ipv6_ace, dev_ace
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertIpv6Ace2DevAce(
    const RULE_TYPE_Ipv6Ace_Entry_T *ipv6_ace,
    DEVRM_AceEntry_T *dev_ace)
{
    RULE_TYPE_RETURN_TYPE_T result;

    result = RULE_CTRL_LocalConvertIpv6Ace2ExtDevAce(ipv6_ace, dev_ace);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    result = RULE_CTRL_LocalConvertIpv6Ace2StdDevAce(ipv6_ace, dev_ace);

    return result;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalConvertIpv6Ace2ExtDevAce
 *------------------------------------------------------------------------------
 * PURPOSE  : convert ipv6 ace to dev ace of ext selector
 * INPUT    : ipv6_ace, dev_ace
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertIpv6Ace2ExtDevAce(
    const RULE_TYPE_Ipv6Ace_Entry_T *ipv6_ace,
    DEVRM_AceEntry_T *dev_ace)
{
#define IS_NOT_ZERO_IP6_ADDR(addr) \
    (!(addr[0]==0 && addr[1]==0 && addr[2]==0 && addr[3]==0 &&      \
       addr[4]==0 && addr[5]==0 && addr[6]==0 && addr[7]==0 &&      \
       addr[8]==0 && addr[9]==0 && addr[10]==0 && addr[11]==0 &&    \
       addr[12]==0 && addr[13]==0 && addr[14]==0 && addr[15]==0))

    enum
    {
        RULE_CTRL_DSCP_MASK = 63
    };

    UI8_T mask[SYS_ADPT_IPV6_ADDR_LEN];

    ASSERT(NULL != ipv6_ace);
    ASSERT(NULL != dev_ace);

    /* only support ipv6 dscp, now.
     */
    if (ipv6_ace->aceDscp != RULE_TYPE_UNDEF_IPV6_DSCP)
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifyIpDscp(dev_ace, ipv6_ace->aceDscp, RULE_CTRL_DSCP_MASK))
        {
            LOG("<Error> Qualify IP DSCP");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    memcpy(mask, ipv6_ace->aceSourceIpAddrBitmask, SYS_ADPT_IPV6_ADDR_LEN);

    /* source */
    if (IS_NOT_ZERO_IP6_ADDR(mask))
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPV6SIP;
        memcpy(dev_ace->srcip6_data, ipv6_ace->aceSourceIpAddr, SYS_ADPT_IPV6_ADDR_LEN);
        memcpy(dev_ace->srcip6_mask, mask, SYS_ADPT_IPV6_ADDR_LEN);
    }

    memcpy(mask, ipv6_ace->aceDestIpAddrBitmask, SYS_ADPT_IPV6_ADDR_LEN);

    /* destination */
    if (IS_NOT_ZERO_IP6_ADDR(mask))
    {
#if (SYS_ADPT_ACL_MAX_DST_IPV6_PREFIX_LEN == 8)
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPV6DIPUPPER;
#else
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPV6DIP;
#endif
        memcpy(dev_ace->dstip6_data, ipv6_ace->aceDestIpAddr, SYS_ADPT_IPV6_ADDR_LEN);
        memcpy(dev_ace->dstip6_mask, mask, SYS_ADPT_IPV6_ADDR_LEN);
    }

    if (ipv6_ace->aceNextHeader != RULE_TYPE_UNDEF_IPV6_NEXT_HEADER)
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPPROTOCOL;
        dev_ace->ipprotocol_data = ipv6_ace->aceNextHeader;
        memset(&dev_ace->ipprotocol_mask, 0xff, sizeof(dev_ace->ipprotocol_mask));
    }

    if (ipv6_ace->aceIcmpType != RULE_TYPE_UNDEF_ICMP_TYPE)
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ICMPTYPE;
        dev_ace->icmptype_data = ipv6_ace->aceIcmpType;
        dev_ace->icmptype_mask = 0xff;
    }

    if (ipv6_ace->aceSourcePortOp == VAL_diffServIpAceSourcePortOp_equal) /* currently only support this operator */
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifyL4SourcePort(dev_ace,
                                                             ipv6_ace->aceMinSourcePort,
                                                             ipv6_ace->aceSourcePortBitmask))
        {
            LOG("<Error> Qualify L4 source port");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    if (ipv6_ace->aceDestPortOp == VAL_diffServIpAceDestPortOp_equal) /* currently only support this operator */
    {
        if (TRUE != DEVRM_LIB_RulePatternQualifyL4DestPort(dev_ace,
                                                           ipv6_ace->aceMinDestPort,
                                                           ipv6_ace->aceDestPortBitmask))
        {
            LOG("<Error> Qualify L4 dest port");
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    /* patch for keymode RULE_TYPE_IP_EXT_ACL */
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ETHER;
        dev_ace->ethertype_data = RULE_TYPE_ETHERTYPE_IPV6;
        dev_ace->ethertype_mask = 0xffff;
    }

    return RULE_TYPE_OK;

#undef IS_NOT_ZERO_IP6_ADDR
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalConvertIpv6Ace2StdDevAce
 *------------------------------------------------------------------------------
 * PURPOSE  : convert ipv6 ace to dev ace of std selector
 * INPUT    : ipv6_ace, dev_ace
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertIpv6Ace2StdDevAce(
    const RULE_TYPE_Ipv6Ace_Entry_T *ipv6_ace,
    DEVRM_AceEntry_T *dev_ace)
{
#define IS_NOT_ZERO_IP6_ADDR(addr) \
    (!(addr[0]==0 && addr[1]==0 && addr[2]==0 && addr[3]==0 &&      \
       addr[4]==0 && addr[5]==0 && addr[6]==0 && addr[7]==0 &&      \
       addr[8]==0 && addr[9]==0 && addr[10]==0 && addr[11]==0 &&    \
       addr[12]==0 && addr[13]==0 && addr[14]==0 && addr[15]==0))

    UI8_T mask[SYS_ADPT_IPV6_ADDR_LEN];

    ASSERT(NULL != ipv6_ace);
    ASSERT(NULL != dev_ace);

    if ((NULL == ipv6_ace) || (NULL == dev_ace))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    memcpy(mask, ipv6_ace->aceSourceIpAddrBitmask, SYS_ADPT_IPV6_ADDR_LEN);

    /* source */
    if (IS_NOT_ZERO_IP6_ADDR(mask))
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPV6SIP;
        memcpy(dev_ace->srcip6_data, ipv6_ace->aceSourceIpAddr, SYS_ADPT_IPV6_ADDR_LEN);
        memcpy(dev_ace->srcip6_mask, mask, SYS_ADPT_IPV6_ADDR_LEN);
    }

    /* patch for keymode RULE_TYPE_IP_EXT_ACL */
    {
        dev_ace->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ETHER;
        dev_ace->ethertype_data = RULE_TYPE_ETHERTYPE_IPV6;
        dev_ace->ethertype_mask = 0xffff;
    }

    return RULE_TYPE_OK;

#undef IS_NOT_ZERO_IP6_ADDR
}

static BOOL_T
RULE_CTRL_Priv_IsMatchAnyAnyAce(
    UI32_T acl_index,
    UI32_T ace_index)
{
    RULE_TYPE_AclType_T     last_ace_type;

    UI32_T                  last_ace_index;
    UI32_T                  result;

    RULE_TYPE_Ace_Entry_T   last_ace_entry;

    /* last rule of ACL and check the ACE is
     * permit any any or
     * deny any any
     */
    result = RULE_OM_GetLastAceByAcl(acl_index, &last_ace_type, &last_ace_index, &last_ace_entry);
    ASSERT(RULE_TYPE_OK == result);

    if (ace_index == last_ace_index &&
        TRUE == RULE_OM_IsMatchAnyAnyAce(ace_index))
    {
        return TRUE;
    }

    return FALSE;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_GetPacketTypeOfAcl(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI32_T acl_index,
    UI32_T ace_index,
    UI32_T *remapped_pkt_type_p)
{
    ASSERT(NULL != interface_p);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction ||
           RULE_TYPE_OUTBOUND == interface_p->direction);
    ASSERT(NULL != remapped_pkt_type_p);

    if (TRUE == RULE_CTRL_Priv_IsMatchAnyAnyAce(acl_index, ace_index))
    {
        switch (interface_p->type)
        {
            case RULE_TYPE_INTERFACE_TRUNK:
                *remapped_pkt_type_p = RULE_TYPE_PACKET_TRUNK_ACL_MATCH_ANY_ANY;
                break;

            case RULE_TYPE_INTERFACE_CPU_PORT:
                *remapped_pkt_type_p = RULE_TYPE_PACKET_CPU_PORT_EGRESS_ACL_MATCH_ANY_ANY;
                break;

            case RULE_TYPE_INTERFACE_ALL_UNIT:
                *remapped_pkt_type_p = RULE_TYPE_PACKET_ALL_PORTS_ACL_MATCH_ANY_ANY;

#if (SYS_CPNT_QOS_V2_ALL_PORTS_RULE_AT_LOOKUP_0_1 == TRUE)
                *remapped_pkt_type_p = RULE_TYPE_PACKET_ALL_PORTS_ACL_MATCH_ANY_ANY_0_1;
#endif /* SYS_CPNT_QOS_V2_ALL_PORTS_RULE_AT_LOOKUP_0_1 */
                break;

            default:
                *remapped_pkt_type_p = (RULE_TYPE_INBOUND == interface_p->direction) ?
                    RULE_TYPE_PACKET_ACL_MATCH_ANY_ANY :
                    RULE_TYPE_PACKET_EGRESS_ACL_MATCH_ANY_ANY;
                break;
        }
    }
    else
    {
        switch (interface_p->type)
        {
            case RULE_TYPE_INTERFACE_TRUNK:
                *remapped_pkt_type_p = RULE_TYPE_PACKET_TRUNK_ACL;
                break;

            case RULE_TYPE_INTERFACE_CPU_PORT:
                *remapped_pkt_type_p = RULE_TYPE_PACKET_CPU_PORT_EGRESS_ACL;
                break;

            case RULE_TYPE_INTERFACE_ALL_UNIT:
                *remapped_pkt_type_p = RULE_TYPE_PACKET_ALL_PORTS_ACL;

#if (SYS_CPNT_QOS_V2_ALL_PORTS_RULE_AT_LOOKUP_0_1 == TRUE)
                *remapped_pkt_type_p = RULE_TYPE_PACKET_ALL_PORTS_ACL_0_1;
#endif /* SYS_CPNT_QOS_V2_ALL_PORTS_RULE_AT_LOOKUP_0_1 */
                break;

            default:
                *remapped_pkt_type_p = (RULE_TYPE_INBOUND == interface_p->direction) ?
                    RULE_TYPE_PACKET_ACL :
                    RULE_TYPE_PACKET_EGRESS_ACL;
                break;
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalSetACL2DEV
 *------------------------------------------------------------------------------
 * PURPOSE  : Allocate and set rule for an ACL
 * INPUT    : unit, port, acl_id
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK ; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetACL2DEV(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI32_T acl_id,
    const RULE_TYPE_Acl_T *acl_entry_p,
    UI32_T time_range_index,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T ace_index;

    RULE_TYPE_AclType_T ace_type = 0;

    //
    // --== Duplicated code start
    //
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;
    //
    // --== End of duplicated code
    //

    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p = NULL;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;

    RULE_TYPE_Ace_Entry_T ace_entry;

    ASSERT(NULL != interface_p);
    ASSERT(interface_p->type < RULE_TYPE_INTERFACE_MAX);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction ||
           RULE_TYPE_OUTBOUND == interface_p->direction);
    ASSERT(NULL != acl_entry_p);

    LOG_IF(*interface_p, "aclId = %lu timeRangeId = %hu, counter = %s",
           acl_id, time_range_index, RULE_TYPE_COUNTER_ENABLE_STR(counter_enable));

    /** Due to chip limitation that it can not apply ACL and policy map
     * on egress direction at the same time.
     */
    if (RULE_TYPE_OUTBOUND == interface_p->direction)
    {
        RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;

        policy_map_inst_p = RULE_CTRL_Priv_GetPolicyMapInstance(interface_p);
        if (NULL != policy_map_inst_p)
        {
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    /** For Marvell, only support to bind one ACL per one port.
     */
    {
        if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
        ASSERT(NULL != if_entry_p);

        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                  &if_class_obj);

        memset(&param, 0, sizeof(param));

        param.type = RULE_TYPE_INST_ACL;

        acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                    RULE_CTRL_Priv_IsClassInstanceMatchType,
                                                                                    &param);
        if (NULL != acl_inst_p)
        {
            LOG("<Error> ACL (%lu) had be bound. Only support to bind one ACL per one port",
                acl_inst_p->id);
            return RULE_TYPE_FAIL;
        }
    }

    //
    // --== Duplicated code start
    //
    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_ACL;
    param.id = acl_id;

    acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                &param);
    if (NULL != acl_inst_p)
    {
        LOG("<Error> Already bind the ACL");
        return RULE_TYPE_INVALID_PARAMETER;
    }
    //
    // --== End of duplicated code
    //

    /* install a new ACL instance on device
     */
    acl_inst_p = NULL;
    acl_inst_obj.inst = NULL;
    result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl_inst_obj);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> No any class instance available");
        goto undo;
    }

    if_class_obj.add_class_obj(&if_class_obj, &acl_inst_obj);

    acl_inst_p = acl_inst_obj.inst;

    ASSERT(NULL != acl_inst_p);

    acl_inst_p->mvl_pkt_type = RULE_TYPE_PACKET_NIL;
    acl_inst_p->acl_id = acl_id;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    acl_inst_p->time_range_index = time_range_index;
#endif /* SYS_CPNT_TIME_BASED_ACL */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
    acl_inst_p->counter_enable = counter_enable;
#endif /* SYS_CPNT_ACL_COUNTER */

    for (ace_index = 0;
         RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_id, &ace_type, &ace_index, &ace_entry);)
    {
        int priority;
        UI32_T remapped_pkt_type;
        BOOL_T is_active = TRUE;

        result = RULE_CTRL_Priv_GetAcePriorityInAcl(acl_id, ace_index, &priority);
        ASSERT(RULE_TYPE_OK == result);

        RULE_CTRL_Priv_GetPacketTypeOfAcl(interface_p,
                                          acl_id,
                                          ace_index,
                                          &remapped_pkt_type);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
          is_active = RULE_CTRL_LocalCheckRuleInActiveByTimeRange(ace_entry.time_range_index, time_range_index);
#endif /* SYS_CPNT_TIME_BASED_ACL */

        {
            RULE_TYPE_AceAction_T ace_access;

            ace_access = ace_entry.access;

            ASSERT(RULE_TYPE_ACE_PERMIT == ace_access ||
                   RULE_TYPE_ACE_DENY == ace_access);

            {
                DEVRM_AceEntry_T rule_pattern;
                DEVRM_ACTION_T dev_action;

                DEVRM_LIB_RulePatternInit(&rule_pattern);

                result = RULE_CTRL_LocalConvertAce2Dev(&ace_entry, &rule_pattern);
                ASSERT(RULE_TYPE_OK == result);

                DEVRM_LIB_ActionInit(&dev_action);

                ASSERT(RULE_TYPE_ACE_PERMIT == ace_access ||
                       RULE_TYPE_ACE_DENY == ace_access);

                if (RULE_TYPE_ACE_PERMIT == ace_access)
                {
                    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
                    {
                        LOG("<Error> Set packet command");
                        goto undo;
                    }

#if (SYS_CPNT_ACL_MIRROR == TRUE)
                    if (RULE_TYPE_MIRROR_ENABLE == acl_entry_p->mirror.enable)
                    {
                        if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_MIRROR_TO_ANALYZER_PORT))
                        {
                            LOG("<Error> Set packet command");
                            goto undo;
                        }
                    }
#endif /* SYS_CPNT_ACL_MIRROR */
                }
                else
                {
                    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_HARD_DROP))
                    {
                        LOG("<Error> Set packet command");
                        goto undo;
                    }
                }

#if (SYS_CPNT_ACL_COUNTER == TRUE)
                if (RULE_TYPE_COUNTER_ENABLE == acl_inst_p->counter_enable)
                {
                    if (TRUE != DEVRM_LIB_ActionMeterConfigFlow(&dev_action, 0, 0))
                    {
                        LOG("<Error> config meter (flow)");
                        goto undo;
                    }

                    if (TRUE != DEVRM_LIB_ActionCounterConfig(&dev_action, DEVRM_POLICER_COUNTER_MODE_PACKET))
                    {
                        LOG("<Error> config counter");
                        goto undo;
                    }
                }
#endif /* SYS_CPNT_ACL_COUNTER */

                result = RULE_CTRL_LocalAddRule_Hello2(interface_p, remapped_pkt_type,
                                                       &acl_inst_obj,
                                                       &rule_pattern,
                                                       &dev_action,
                                                       is_active,
                                                       ace_index,
                                                       priority);
                if (RULE_TYPE_OK != result)
                {
                    LOG("<Error> SetAce2Dev");
                    goto undo;
                }

            }
        }
    }

    return RULE_TYPE_OK;

undo:

    {
        RULE_TYPE_RETURN_TYPE_T temp_result;

        temp_result = RULE_CTRL_LocalRemoveACLFromDEV(interface_p, acl_id, acl_entry_p);
        ASSERT(RULE_TYPE_OK == temp_result);
    }

    ASSERT(RULE_TYPE_OK != result);
    return result;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalRemoveACLFromDEV
 *------------------------------------------------------------------------------
 * PURPOSE  : Remove ACL rule
 * INPUT    : unit, port, acl_id, is_trunk
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK ; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveACLFromDEV(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI32_T acl_id,
    const RULE_TYPE_Acl_T *acl_entry_p)
{
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;

    ASSERT(NULL != interface_p);
    ASSERT(interface_p->type < RULE_TYPE_INTERFACE_MAX);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction ||
           RULE_TYPE_OUTBOUND == interface_p->direction);
    ASSERT(NULL != acl_entry_p);

    LOG_IF(*interface_p, "aclId = %lu", acl_id);

    //
    // --== Duplicated code start
    //
    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_ACL;
    param.id = acl_id;

    acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                &param);
    //
    // --== End of duplicated code
    //

    if (NULL != acl_inst_p)
    {
        RULE_CTRL_LocalFreeRulesByRuleGroup(acl_inst_p);

        ASSERT(NULL != acl_inst_p);
        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p, &acl_inst_obj);

        ASSERT(RULE_CTRL_IN_Ptr2Instance(acl_inst_p->super.links.parent) == if_class_obj.inst);

        if_class_obj.remove_class_obj(&if_class_obj, &acl_inst_obj);
        RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&acl_inst_obj);
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalAddAce2DEV
 *------------------------------------------------------------------------------
 * PURPOSE  : add a rule to chip and check whether the acl use on ACL or QoS
 * INPUT    : acl_index, acl_entry, rule_entry
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK ; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddAce2DEV(
    UI32_T acl_index,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    result = RULE_CTRL_Priv_CheckResource_AddAce2DEV(acl_index, acl_entry_p, ace_entry_p);
    if (RULE_TYPE_OK != result)
    {
        LOG("Resoure not enough");
        return result;
    }

    result = RULE_CTRL_LocalAddAce2DevForAclPurpose(acl_index, acl_entry_p, ace_entry_p);
    if (RULE_TYPE_OK != result)
        return result;

    result = RULE_CTRL_LocalAddAce2DevForQosPurpose(acl_index, acl_entry_p, ace_entry_p);
    if (RULE_TYPE_OK != result)
    {
        RULE_CTRL_LocalRemoveAceFromDEV(acl_index, acl_entry_p, ace_entry_p);
        return result;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalAddAce2DevForAclPurpose
 *------------------------------------------------------------------------------
 * PURPOSE  : add a rule to chip according to the acl's port list
 * INPUT    : acl_index, acl_entry, rule_entry
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is no port bound to this acl, return TRUE (do nothing)
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddAce2DevForAclPurpose(
    UI32_T acl_index,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T ace_index;

    UI32_T if_entry_index;

    BOOL_T is_active;

    if ((NULL == acl_entry_p) || (NULL == ace_entry_p) ||
        (RULE_TYPE_OK != RULE_OM_GetAceIndexFromAclByAceEntry(acl_index, ace_entry_p, &ace_index)))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    for (if_entry_index = 0;
         if_entry_index < _countof(shmem_data_p->if_table);
         ++ if_entry_index)
    {
        RULE_TYPE_INTERFACE_INFO_T interface;

        RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

        RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

        RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
        RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;

        RULE_CTRL_INSTANCE_TYPE_ID_T param;

        UI32_T remapped_pkt_type;

        int priority;

        RULE_TYPE_AceAction_T   ace_access;

        RULE_CTRL_Priv_IfEntry_Index_To_Interface(if_entry_index, &interface);

        //
        // --== Duplicated code start
        //
        if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
        ASSERT(NULL != if_entry_p);

        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                  &if_class_obj);

        memset(&param, 0, sizeof(param));

        param.type = RULE_TYPE_INST_ACL;
        param.id = acl_index;

        acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                    RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                    &param);

        //
        // --== End of duplicated code
        //

        if (NULL == acl_inst_p)
        {
            continue;
        }

        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p, &acl_inst_obj);

        result = RULE_CTRL_Priv_GetAcePriorityInAcl(acl_index, ace_index, &priority);
        ASSERT(RULE_TYPE_OK == result);

        result = RULE_CTRL_Priv_GetPacketTypeOfAcl(&interface,
                                                   acl_index,
                                                   ace_index,
                                                   &remapped_pkt_type);
        ASSERT(RULE_TYPE_OK == result);

        is_active = RULE_CTRL_LocalCheckRuleInActiveByTimeRange(ace_entry_p->time_range_index,
                                                                acl_inst_p->time_range_index);

        ace_access = ace_entry_p->access;

        {
            DEVRM_AceEntry_T rule_pattern;
            DEVRM_ACTION_T dev_action;

            DEVRM_LIB_RulePatternInit(&rule_pattern);

            result = RULE_CTRL_LocalConvertAce2Dev(ace_entry_p, &rule_pattern);
            ASSERT(RULE_TYPE_OK == result);

            if (RULE_TYPE_OK != result)
            {
                LOG("<Error> Failed to convert rule pattern");
                return RULE_TYPE_INVALID_PARAMETER;
            }

            DEVRM_LIB_ActionInit(&dev_action);

            ASSERT(RULE_TYPE_ACE_PERMIT == ace_access ||
                   RULE_TYPE_ACE_DENY == ace_access);

            if (RULE_TYPE_ACE_PERMIT == ace_access)
            {
                if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
                {
                    LOG("<Error> Set packet command");
                    goto undo_add_rule;
                }

#if (SYS_CPNT_ACL_MIRROR == TRUE)
                if (RULE_TYPE_MIRROR_ENABLE == acl_entry_p->mirror.enable)
                {
                    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_MIRROR_TO_ANALYZER_PORT))
                    {
                        LOG("<Error> Set packet command");
                        goto undo_add_rule;
                    }
                }
#endif /* SYS_CPNT_ACL_MIRROR */
            }
            else
            {
                if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_HARD_DROP))
                {
                    LOG("<Error> Set packet command");
                    goto undo_add_rule;
                }
            }

#if (SYS_CPNT_ACL_COUNTER == TRUE)
            if (RULE_TYPE_COUNTER_ENABLE == acl_inst_p->counter_enable)
            {
                if (TRUE != DEVRM_LIB_ActionMeterConfigFlow(&dev_action, 0, 0))
                {
                    LOG("<Error> config meter (flow)");
                    goto undo_add_rule;
                }

                if (TRUE != DEVRM_LIB_ActionCounterConfig(&dev_action, DEVRM_POLICER_COUNTER_MODE_PACKET))
                {
                    LOG("<Error> config counter");
                    goto undo_add_rule;
                }
            }
#endif /* SYS_CPNT_ACL_COUNTER */

            result = RULE_CTRL_LocalAddRule_Hello2(&interface, remapped_pkt_type,
                                                   &acl_inst_obj,
                                                   &rule_pattern,
                                                   &dev_action,
                                                   is_active,
                                                   ace_index,
                                                   priority);
            if (RULE_TYPE_OK != result)
            {
                LOG("<Error> Failed to add rule to dev");
                goto undo_add_rule;
            }
        }

    }

    return RULE_TYPE_OK;

undo_add_rule:
    {
        RULE_TYPE_RETURN_TYPE_T temp_result;
        temp_result = RULE_CTRL_LocalRemoveAceFromDEV(acl_index, acl_entry_p, ace_entry_p);
        ASSERT(RULE_TYPE_OK == temp_result);
    }

    ASSERT(RULE_TYPE_OK != result);

    return result;
}

typedef struct
{
    UI32_T acl_index;
    UI32_T ace_index;
    const RULE_TYPE_Acl_T *acl_entry_p;
    const RULE_TYPE_Ace_Entry_T *ace_entry_p;

    RULE_TYPE_INTERFACE_INFO_PTR_T interface_p;
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;

    RULE_TYPE_RETURN_TYPE_T result;
} RULE_CTRL_ADD_ACE_PARAM_T, *RULE_CTRL_ADD_ACE_PARAM_PTR_T;

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalAddAce2DevForQosPurpose
 *------------------------------------------------------------------------------
 * PURPOSE  : add a rule to chip according to the qos's port list
 * INPUT    : acl_index, acl_entry, rule_entry, port_list
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is no port bound to this qos, return TRUE (do nothing)
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddAce2DevForQosPurpose(
    UI32_T acl_index,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T if_entry_index;

    UI32_T ace_index;

    if ((NULL == acl_entry_p) || (NULL == ace_entry_p))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    /* If SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL is FALSE,
     *   we do not configure an ace with deny action to chip (for DiffServ).
     */
#if (SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL != TRUE)
    if (RULE_TYPE_ACE_DENY == ace_entry_p->access)
    {
        return RULE_TYPE_OK;
    }
#endif /* #if (SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL != TRUE) */

    result = RULE_OM_GetAceIndexFromAclByAceEntry(acl_index, ace_entry_p, &ace_index);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    for (if_entry_index = 0;
         if_entry_index < _countof(shmem_data_p->if_table);
         ++ if_entry_index)
    {
        //
        // --== Duplicated code start
        //
        RULE_TYPE_INTERFACE_INFO_T interface;

        RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

        RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;
        RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;

        RULE_CTRL_INSTANCE_TYPE_ID_T find_param;
        //
        // --== End of duplicated code
        //

        RULE_CTRL_Priv_IfEntry_Index_To_Interface(if_entry_index, &interface);
        //
        // --== Duplicated code start
        //
        if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
        ASSERT(NULL != if_entry_p);

        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                  &if_class_obj);

        memset(&find_param, 0, sizeof(find_param));

        find_param.type = RULE_TYPE_INST_POLICY_MAP;

        policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                           RULE_CTRL_Priv_IsClassInstanceMatchType,
                                                                                           &find_param);
        //
        // --== End of duplicated code
        //

        while (NULL != policy_map_inst_p)
        {
            RULE_CTRL_ADD_ACE_PARAM_T param;

            memset(&param, 0, sizeof(param));

            param.acl_index = acl_index;
            param.ace_index = ace_index;
            param.acl_entry_p = acl_entry_p;
            param.ace_entry_p = ace_entry_p;
            param.result = RULE_TYPE_OK;

            param.interface_p = &interface;
            param.policy_map_inst_p = policy_map_inst_p;

            RULE_CTRL_IN_Priv_Walk((RULE_TYPE_INSTANCE_PTR_T)policy_map_inst_p,
                                   RULE_CTRL_LocalAddAce2DevForQosPurpose_Hello,
                                   &param);

            if (RULE_TYPE_OK != param.result)
            {
                result = param.result;
                goto undo;
            }

            policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetNextSiblingIf((RULE_TYPE_INSTANCE_PTR_T)policy_map_inst_p,
                                                                                                RULE_CTRL_Priv_IsClassInstanceMatchType,
                                                                                                &find_param);
        }
    }

    return RULE_TYPE_OK;

undo:
    ASSERT(RULE_TYPE_OK != result);
    return result;
}

static void
RULE_CTRL_LocalAddAce2DevForQosPurpose_Hello(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie)
{
    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;

    RULE_CTRL_ADD_ACE_PARAM_PTR_T param_p;
    UI32_T ui_acl_index;

    param_p = (RULE_CTRL_ADD_ACE_PARAM_PTR_T)cookie;

    ASSERT(NULL != param_p->acl_entry_p);
    ASSERT(NULL != param_p->ace_entry_p);
    ASSERT(NULL != param_p->interface_p);
    ASSERT(NULL != param_p->policy_map_inst_p);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    if (param_p->acl_entry_p->flag_hide == TRUE)
    {
        ui_acl_index = param_p->acl_entry_p->owner.class_id;
    }
    else
#endif
    {
        ui_acl_index = param_p->acl_index;
    }

    acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)in;
    if (RULE_TYPE_INST_ACL == acl_inst_p->super.type &&
        acl_inst_p->id == param_p->acl_index)
    {
        RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p;

        RULE_TYPE_PolicyElement_T policy_element;

        RULE_TYPE_CLASS_INSTANCE_OBJECT_T class_map_inst_obj;
        RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;
        RULE_TYPE_CLASS_INSTANCE_OBJECT_T ace_inst_obj;

        UI32_T remapped_pkt_type;

        int priority;

        UI32_T meter_id;
        RULE_TYPE_TBParamEntry_T meter_entry;
        RULE_TYPE_Action_T action_entry;

        class_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T) RULE_CTRL_IN_Ptr2Instance(acl_inst_p->super.links.parent);
        ASSERT(NULL != class_map_inst_p);
        ASSERT(RULE_TYPE_INST_CLASS_MAP == class_map_inst_p->super.type);

        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_map_inst_p, &class_map_inst_obj);

        ASSERT(param_p->policy_map_inst_p == (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_IN_Ptr2Instance(class_map_inst_p->super.links.parent));

        param_p->result = RULE_OM_GetPolicyMapElementFromPolicyMapByClassMap(param_p->policy_map_inst_p->id,
                                                                             class_map_inst_p->id,
                                                                             &policy_element);
        ASSERT(RULE_TYPE_OK == param_p->result);

        param_p->result = RULE_CTRL_Priv_GetClassMapElementPriorityInClassMap(param_p->policy_map_inst_p->id,
                                                                              class_map_inst_p->id,
                                                                              RULE_TYPE_CLASS_ACL,
                                                                              ui_acl_index,
                                                                              &priority);
        ASSERT(RULE_TYPE_OK == param_p->result);

        param_p->result = RULE_OM_GetActionEntryByIndex(policy_element.action_index, &action_entry);
        ASSERT(RULE_TYPE_OK == param_p->result);

        param_p->result = RULE_OM_GetMeterEntryByIndex(policy_element.meter_index, &meter_entry);
        if (RULE_TYPE_OK != param_p->result)
        {
            meter_id = 0;
        }
        else
        {
            meter_id = policy_element.meter_index;
        }

        /**
         * Create ACE instance
         */
        param_p->result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACE, &ace_inst_obj);
        if (RULE_TYPE_OK != param_p->result)
        {
            LOG("<Error> Failed to create mf/ace instance");
            return;
        }

        ASSERT(NULL != ace_inst_obj.inst);

        remapped_pkt_type = (RULE_TYPE_INTERFACE_CPU_PORT == param_p->interface_p->type) ?
                                RULE_TYPE_PACKET_CPU_PORT_EGRESS_QOS :
                            (RULE_TYPE_INTERFACE_TRUNK == param_p->interface_p->type) ?
                                RULE_TYPE_PACKET_TRUNK_QOS :
                            (RULE_TYPE_INBOUND == param_p->interface_p->direction) ?
                                RULE_TYPE_PACKET_QOS :
                                RULE_TYPE_PACKET_EGRESS_QOS;

        ace_inst_obj.inst->id = param_p->ace_index;
        ace_inst_obj.inst->mvl_pkt_type = remapped_pkt_type;

        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p, &acl_inst_obj);
        acl_inst_obj.add_class_obj(&acl_inst_obj, &ace_inst_obj);

        param_p->result = RULE_CTRL_EACH_UNIT_DEVICE(param_p->interface_p,
                                                     RULE_CTRL_LocalAddQosRule_To_Dev,
                                                     param_p->interface_p,
                                                     remapped_pkt_type,
                                                     &class_map_inst_obj,
                                                     &ace_inst_obj,
                                                     param_p->ace_index,
                                                     param_p->ace_entry_p,
                                                     meter_id, 0 != meter_id ? &meter_entry : NULL,
                                                     policy_element.action_index, &action_entry,
                                                     priority);
        if (RULE_TYPE_OK != param_p->result)
        {
            return;
        }

    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalRemoveAceFromDEV
 *------------------------------------------------------------------------------
 * PURPOSE  : remove rule for ACE of ACL
 * INPUT    : acl_id - ACL index
 *            acl_entry_p - ACL
 *            ace_entry_p - ACE
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveAceFromDEV(
    UI32_T acl_id,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    result = RULE_CTRL_LocalRemoveAceFromDevForAclPurpose(acl_id, acl_entry_p, ace_entry_p);
    if (RULE_TYPE_OK != result)
        return result;

    result = RULE_CTRL_LocalRemoveAceFromDevForQosPurpose(acl_id, acl_entry_p, ace_entry_p);
    ASSERT(RULE_TYPE_OK == result);

    return result;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalRemoveAceFromDevForAclPurpose
 *------------------------------------------------------------------------------
 * PURPOSE  : remove a rule from chip according to the acl's port list
 * INPUT    : acl_index, acl_entry, rule_entry
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is no port bound to this acl, return TRUE (do nothing)
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveAceFromDevForAclPurpose(
    UI32_T acl_index,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T ace_index;

    UI32_T if_entry_index;

    if ((NULL == acl_entry_p) || (NULL == ace_entry_p) ||
        (RULE_TYPE_OK != RULE_OM_GetAceIndexFromAclByAceEntry(acl_index, ace_entry_p, &ace_index)))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    for (if_entry_index = 0;
         if_entry_index < _countof(shmem_data_p->if_table);
         ++ if_entry_index)
    {
        RULE_TYPE_INTERFACE_INFO_T interface;

        RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

        RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

        RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
        RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;

        RULE_TYPE_CLASS_INSTANCE_PTR_T ace_inst_p;
        RULE_TYPE_CLASS_INSTANCE_OBJECT_T ace_inst_obj;

        RULE_CTRL_INSTANCE_TYPE_ID_T param;

        RULE_CTRL_Priv_IfEntry_Index_To_Interface(if_entry_index, &interface);

        //
        // --== Duplicated code start
        //
        if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
        ASSERT(NULL != if_entry_p);

        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                  &if_class_obj);

        memset(&param, 0, sizeof(param));

        param.type = RULE_TYPE_INST_ACL;
        param.id = acl_index;

        acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                    RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                    &param);
        //
        // --== End of duplicated code
        //

        if (NULL == acl_inst_p)
        {
            continue;
        }

        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p, &acl_inst_obj);

        param.type = RULE_TYPE_INST_ACE;
        param.id = ace_index;

        ace_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) acl_inst_p,
                                                                                    RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                    &param);
        if (NULL != ace_inst_p)
        {
            result = RULE_CTRL_LocalFreeRulesByRuleGroup(ace_inst_p);
            ASSERT(RULE_TYPE_OK == result);

            RULE_CTRL_IN_Priv_ConstructClassInstanceObject(ace_inst_p, &ace_inst_obj);

            acl_inst_obj.remove_class_obj(&acl_inst_obj, &ace_inst_obj);
            RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&ace_inst_obj);
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalRemoveAceFromDevForQosPurpose
 *------------------------------------------------------------------------------
 * PURPOSE  : remove a rule to chip according to the qos's port list
 * INPUT    : acl_index, acl_entry, rule_entry, port_list
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is no port bound to this qos, return TRUE (do nothing)
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveAceFromDevForQosPurpose(
    UI32_T acl_index,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p)
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T if_entry_index;

    UI32_T ace_index;

    if ((NULL == acl_entry_p) || (NULL == ace_entry_p))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    result = RULE_OM_GetAceIndexFromAclByAceEntry(acl_index, ace_entry_p, &ace_index);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    for (if_entry_index = 0;
         if_entry_index < _countof(shmem_data_p->if_table);
         ++ if_entry_index)
    {
        //
        // --== Duplicated code start
        //
        RULE_TYPE_INTERFACE_INFO_T interface;

        RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

        RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;
        RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;

        RULE_CTRL_INSTANCE_TYPE_ID_T find_param;
        //
        // --== End of duplicated code
        //

        RULE_CTRL_Priv_IfEntry_Index_To_Interface(if_entry_index, &interface);
        //
        // --== Duplicated code start
        //
        if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
        ASSERT(NULL != if_entry_p);

        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                  &if_class_obj);

        memset(&find_param, 0, sizeof(find_param));

        find_param.type = RULE_TYPE_INST_POLICY_MAP;
        //
        // --== End of duplicated code
        //

        for (policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                                RULE_CTRL_Priv_IsClassInstanceMatchType,
                                                                                                &find_param);
             NULL != policy_map_inst_p;
             policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetNextSiblingIf((RULE_TYPE_INSTANCE_PTR_T)policy_map_inst_p,
                                                                                                 RULE_CTRL_Priv_IsClassInstanceMatchType,
                                                                                                 &find_param))
        {
            RULE_CTRL_INSTANCE_TYPE_ID_T find_class_map_param;
            RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p;

            memset(&find_class_map_param, 0, sizeof(find_class_map_param));

            find_class_map_param.type = RULE_TYPE_INST_CLASS_MAP;

            for (class_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) policy_map_inst_p,
                                                                                                   RULE_CTRL_Priv_IsClassInstanceMatchType,
                                                                                                   &find_class_map_param);
                 NULL != class_map_inst_p;
                 class_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetNextSiblingIf((RULE_TYPE_INSTANCE_PTR_T)class_map_inst_p,
                                                                                                    RULE_CTRL_Priv_IsClassInstanceMatchType,
                                                                                                    &find_class_map_param))
            {

                RULE_CTRL_INSTANCE_TYPE_ID_T find_acl_param;
                RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;

                memset(&find_acl_param, 0, sizeof(find_acl_param));

                find_acl_param.type = RULE_TYPE_INST_ACL;
                find_acl_param.id = acl_index;

                for (acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) class_map_inst_p,
                                                                                                 RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                                 &find_acl_param);
                     NULL != acl_inst_p;
                     acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetNextSiblingIf((RULE_TYPE_INSTANCE_PTR_T)acl_inst_p,
                                                                                                  RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                                  &find_acl_param))
                {
                    RULE_CTRL_INSTANCE_TYPE_ID_T find_ace_param;
                    RULE_TYPE_CLASS_INSTANCE_PTR_T ace_inst_p;

                    memset(&find_ace_param, 0, sizeof(find_ace_param));

                    find_ace_param.type = RULE_TYPE_INST_ACE;
                    find_ace_param.id = ace_index;

                    ace_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_IN_Priv_FindIf((RULE_TYPE_INSTANCE_PTR_T)acl_inst_p,
                                                                                          RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                          &find_ace_param);
                    if (NULL != ace_inst_p)
                    {
                        RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;
                        RULE_TYPE_CLASS_INSTANCE_OBJECT_T ace_inst_obj;

                        result = RULE_CTRL_LocalFreeRulesByRuleGroup(ace_inst_p);
                        ASSERT(RULE_TYPE_OK == result);

                        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p, &acl_inst_obj);
                        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(ace_inst_p, &ace_inst_obj);

                        acl_inst_obj.remove_class_obj(&acl_inst_obj, &ace_inst_obj);
                        RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&ace_inst_obj);
                    }
                }
            }
        }
    }

    return RULE_TYPE_OK;
}

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
/*--------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_ProcessTimeRangeStatusChange
 *---------------------------------------------------------------------------
 * PURPOSE: Process time_range status change
 * INPUT:  isChanged_list, status_list.
 * OUTPUT: None.
 * RETURN: TRUE/FALSE.
 * NOTES:  None.
 *---------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_ProcessTimeRangeStatusChange(UI8_T *isChanged_list, UI8_T *status_list)
{
    UI32_T                            i;
    UI8_T state_change_list[SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY] = {0};

    /* update time range table
     */

    RULE_CTRL_EnterCriticalSection();

    for (i = 0; i < SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY; ++i)
    {
        if (isChanged_list[(UI32_T)(i>>3)] & (1 << (7 - (i%8))))
    {
            if (status_list[(UI32_T)(i>>3)] & (1 << (7 - (i%8))))
        {
                state_change_list[i] = RULE_TYPE_TIME_RANGE_IN_ACTIVE;
            }
            else
            {
                state_change_list[i] = RULE_TYPE_TIME_RANGE_UN_ACTIVE;
            }
        }
    }

    for (i = 0; i < _countof(shmem_data_p->if_table); ++ i)
    {
        RULE_TYPE_INTERFACE_INFO_T interface;

        RULE_CTRL_Priv_IfEntry_Index_To_Interface(i, &interface);
        RULE_CTRL_LocalUpdateTimeACLRelationRulePerInterface(&interface, state_change_list);
    }

    RULE_CTRL_LeaveCriticalSection();

    return TRUE;
} /* END FUNCTION - BOOL_T RULE_CTRL_ProcessTimeRangeStatusChange() */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalUpdateTimeACLRelationRulePerInterface
 *------------------------------------------------------------------------------
 * PURPOSE: Process timer alarm event from timer
 * INPUT:  None
 * OUTPUT: None
 * RETURN: TRUE/FALSE
 * NOTES:  None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalUpdateTimeACLRelationRulePerInterface(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI8_T *state_change_list)
{
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;

    //
    // --== Duplicated code start
    //
    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_ACL;

    acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                RULE_CTRL_Priv_IsClassInstanceMatchType,
                                                                                &param);
    while (NULL != acl_inst_p)
    {
        RULE_CTRL_LocalUpdateTimeACLRelationRulePerInterface_ACL(interface_p, state_change_list, acl_inst_p);

        acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetNextSiblingIf((RULE_TYPE_INSTANCE_PTR_T)acl_inst_p,
                                                                                     RULE_CTRL_Priv_IsClassInstanceMatchType,
                                                                                     &param);
    }
    //
    // --== End of duplicated code
    //

    return RULE_TYPE_OK;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalUpdateTimeACLRelationRulePerInterface_ACL(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI8_T *state_change_list,
    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_Acl_T acl_entry;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_class_inst_obj;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T ace_class_inst_iter;

    UI8_T do_action, default_action; /* store the state of time-range-compare-list */

    BOOL_T b_result;

    result = RULE_OM_GetAclByIndex(acl_inst_p->id, &acl_entry);
    ASSERT(RULE_TYPE_OK == result);

    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Failed to get ACL entry");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    /* get default action */
    if (RULE_TYPE_UNDEF_TIME_RANGE == acl_inst_p->time_range_index)
    {
        default_action = RULE_TYPE_TIME_RANGE_IN_ACTIVE;
    }
    else
    {
        default_action = state_change_list[acl_inst_p->time_range_index];
    }

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p, &acl_class_inst_obj);
    acl_class_inst_obj.get_element_iterator(&acl_class_inst_obj, &ace_class_inst_iter);

    for (ace_class_inst_iter.first(&ace_class_inst_iter);
         NULL != ace_class_inst_iter.get_instance(&ace_class_inst_iter);
         ace_class_inst_iter.next(&ace_class_inst_iter))
    {
        RULE_TYPE_CLASS_INSTANCE_PTR_T ace_class_inst_p;
        RULE_TYPE_Ace_Entry_T ace_entry;

        ace_class_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)ace_class_inst_iter.get_instance(&ace_class_inst_iter);

        result = RULE_OM_GetAceByIndex(ace_class_inst_p->id, &ace_entry);
        ASSERT(RULE_TYPE_OK == result);

        if (RULE_TYPE_OK != result)
        {
            LOG("<Internal Error> Failed to get ACE entry");
            RULE_CTRL_Priv_DumpClassInstance((RULE_TYPE_INSTANCE_PTR_T)ace_class_inst_p, NULL);
            continue;
        }

        /* decide do which action */
        if (RULE_TYPE_UNDEF_TIME_RANGE == ace_entry.time_range_index)
        {
            do_action = default_action;
        }
        else
        {
            do_action = state_change_list[ace_entry.time_range_index];
        }

        {
            RULE_TYPE_CLASS_INSTANCE_OBJECT_T ace_class_inst_obj;
            RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;

            RULE_CTRL_IN_Priv_ConstructClassInstanceObject(ace_class_inst_p, &ace_class_inst_obj);
            ace_class_inst_obj.get_element_iterator(&ace_class_inst_obj, &rule_inst_iter);

            for (rule_inst_iter.first(&rule_inst_iter);
                 NULL != rule_inst_iter.get_instance(&rule_inst_iter);
                 rule_inst_iter.next(&rule_inst_iter))
            {
                RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;

                rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T)rule_inst_iter.get_instance(&rule_inst_iter);

                if (RULE_TYPE_TIME_RANGE_IN_ACTIVE == do_action)
                {
                    if (TRUE != rule_inst_p->active)
                    {
                        RULE_CTRL_PCL_CFG_T pcl_cfg;
                        RULE_TYPE_AceAction_T ace_access;

                        //
                        // FIXME: Wrong way here
                        //
                        memset(&pcl_cfg, 0, sizeof(pcl_cfg));
                        pcl_cfg.pcl_id = rule_inst_p->dev_rule_info.pcl_id;
                        pcl_cfg.pcl_id_mask = rule_inst_p->dev_rule_info.pcl_id_mask;
                        pcl_cfg.lookup_num = rule_inst_p->dev_rule_info.lookup_num;
                        memcpy(&pcl_cfg.interface,
                               interface_p,
                               sizeof(pcl_cfg.interface));


                        //
                        // --== Duplicated Code Start
                        //

                        ace_access = ace_entry.access;

                        //
                        // --== Duplicated Code End
                        //

                        {
                            DEVRM_AceEntry_T rule_pattern;
                            DEVRM_ACTION_T dev_action;

                            DEVRM_LIB_RulePatternInit(&rule_pattern);

                            result = RULE_CTRL_LocalConvertAce2Dev(&ace_entry, &rule_pattern);
                            ASSERT(RULE_TYPE_OK == result);
                            if (RULE_TYPE_OK != result)
                            {
                                LOG("<Error> Failed to convert rule pattern");
                                continue;
                            }

                            DEVRM_LIB_ActionInit(&dev_action);

                            ASSERT(RULE_TYPE_ACE_PERMIT == ace_access ||
                                   RULE_TYPE_ACE_DENY == ace_access);

                            if (RULE_TYPE_ACE_PERMIT == ace_access)
                            {
                                if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_FORWARD))
                                {
                                    LOG("<Error> Set packet command");
                                    continue;
                                }

#if (SYS_CPNT_ACL_MIRROR == TRUE)
                                if (RULE_TYPE_MIRROR_ENABLE == acl_entry.mirror.enable)
                                {
                                    if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_MIRROR_TO_ANALYZER_PORT))
                                    {
                                        LOG("<Error> Set packet command");
                                        continue;
                                    }
                                }
#endif /* SYS_CPNT_ACL_MIRROR */
                            }
                            else
                            {
                                if (TRUE != DEVRM_LIB_ActionPacketTo(&dev_action, DEVRM_PKT_CMD_HARD_DROP))
                                {
                                    LOG("<Error> Set packet command");
                                    continue;
                                }
                            }

#if (SYS_CPNT_ACL_COUNTER == TRUE)
                            if (RULE_TYPE_COUNTER_ENABLE == acl_inst_p->counter_enable)
                            {
                                if (TRUE != DEVRM_LIB_ActionMeterConfigFlow(&dev_action, 0, 0))
                                {
                                    LOG("<Error> config meter (flow)");
                                    continue;
                                }

                                if (TRUE != DEVRM_LIB_ActionCounterConfig(&dev_action, DEVRM_POLICER_COUNTER_MODE_PACKET))
                                {
                                    LOG("<Error> config counter");
                                    continue;
                                }
                            }
#endif /* SYS_CPNT_ACL_COUNTER */

                            result = RULE_CTRL_LocalAllocateAndSetRule(rule_inst_p->dev_rule_info.unit,
                                                                       rule_inst_p->dev_rule_info.device,
                                                                       &pcl_cfg,
                                                                       rule_inst_p->dev_rule_info.virtual_group_id,
                                                                       rule_inst_p->dev_rule_info.priority,
                                                                       rule_inst_p->dev_rule_info.rule_id,
                                                                       &rule_pattern,
                                                                       &dev_action);
                            ASSERT(RULE_TYPE_OK == result);

                            if (RULE_TYPE_OK != result)
                            {
                                LOG("<Error> Failed to allocate and set rule on dev");
                                continue;
                            }

                        }

                        rule_inst_p->active = TRUE;
                    }
                }
                else if (RULE_TYPE_TIME_RANGE_UN_ACTIVE == do_action)
                {
                    if (TRUE == rule_inst_p->active)
                    {
                        b_result = RULE_CTRL_LocalDestroyRule(rule_inst_p->dev_rule_info.unit,
                                                              1,
                                                              rule_inst_p->dev_rule_info.device,
                                                              rule_inst_p->dev_rule_info.pcl_id,
                                                              rule_inst_p->dev_rule_info.rule_id);
                        ASSERT(TRUE == b_result);

                        if (TRUE != b_result)
                        {
                            LOG("<Error> Failed to destroy rule on dev");
                            continue;
                        }

                        {
                            result = RULE_CTRL_Priv_DestroyIfRuleRererence(rule_inst_p->dev_rule_info.unit,
                                                                           rule_inst_p->dev_rule_info.device,
                                                                           &rule_inst_p->dev_rule_info.interface,
                                                                           rule_inst_p->dev_rule_info.lookup_num,
                                                                           rule_inst_p->dev_rule_info.pcl_id);
                            if (result != RULE_TYPE_OK)
                            {
                                ASSERT(0);
                                return RULE_TYPE_FAIL;
                            }
                        }

                        rule_inst_p->active = FALSE;
                    }
                }
            }
        }
    }

    return RULE_TYPE_OK;
}
#endif /* end of #if (SYS_CPNT_TIME_BASED_ACL == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalCheckRuleInActiveByTimeRange
 *------------------------------------------------------------------------------
 * PURPOSE  : Check a ace if be in-active in this moment
 * INPUT    : ace_time_range_id, acl_time_range_id
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : first check "ace_time_range_id" (first priority), if fail then
 *                  check "acl_time_range_id" (2nd priority)
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_LocalCheckRuleInActiveByTimeRange(
    UI32_T ace_time_range_id,
    UI32_T acl_time_range_id)
{
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    BOOL_T is_active = TRUE;

    /* check the state of time-range entry relation with ace : (first priority)
       if no time-range binds to ace, it will return failed.
       If it fail, then to check acl-port time-range.
     */
    if (TIME_RANGE_ERROR_TYPE_NONE == TIME_RANGE_OM_IsTimeRangeEntryActive(ace_time_range_id, &is_active))
    {
        return is_active;
    }

    /* check the state of time-range entry relation with acl : (second priority)
       if no time-range binds to acl and port, it will return failed.
     */
    if (TIME_RANGE_ERROR_TYPE_NONE == TIME_RANGE_OM_IsTimeRangeEntryActive(acl_time_range_id, &is_active))
    {
        return is_active;
    }
#endif

    /* no any time-range is relation with ace */
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalCheckPolicyMapOnBinding
 *------------------------------------------------------------------------------
 * PURPOSE  : RULE_CTRL_LocalCheckPolicyMapOnBinding
 * INPUT    : interface_p - interface
 *            policy_map_index - index of policy map
 *            policy_map_p - policy map
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalCheckPolicyMapOnBinding(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    const RULE_TYPE_PolicyMap_T *policy_map_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i;

    RULE_TYPE_CLASS_INSTANCE_PTR_T  policy_map_inst_p;

    RULE_TYPE_PolicyElement_T policy_element;
    RULE_TYPE_ClassMap_T class_map;

    /* if the port has already bind do not check and return
     */
    policy_map_inst_p = RULE_CTRL_Priv_GetPolicyMapInstance(interface_p);
    if (NULL != policy_map_inst_p)
    {
        if (policy_map_inst_p->policy_map_id == policy_map_index)
        {
            return RULE_TYPE_ALREADY_BIND;
        }

        /* Already bind another policy-map on this interface
         */
        return RULE_TYPE_FAIL;
    }

    /* Validating Hardware limitation
     */
    if (RULE_TYPE_INBOUND == interface_p->direction)
    {
        /* It can not bind a class-map with match source ethernet port on a port
         * in ingress direction.
         */
        for (i = 0; i < policy_map_p->element_count; i++)
        {
            UI32_T cx;

            result = RULE_OM_GetPolicyMapElement(policy_map_p->policy_map_element_index_list[i],
                                                 &policy_element);
            ASSERT(RULE_TYPE_OK == result);

            if (RULE_TYPE_OK != result)
            {
                LOG("GetPolicyMapElement fail. policyMapElementIndex=%lu",
                    policy_map_p->policy_map_element_index_list[i]);
                return result;
            }

            result = RULE_OM_GetClassMap(policy_element.class_map_index, &class_map);
            ASSERT(RULE_TYPE_OK == result);

            if (RULE_TYPE_OK != result)
            {
                LOG("GetClassMap fail. classMapIndex=%lu", policy_element.class_map_index);
                return result;
            }

            for (cx=0; cx < class_map.class_element_count; ++cx)
            {
                if (RULE_TYPE_CLASS_MF == class_map.class_type[cx])
                {
                    UI32_T class_id = class_map.class_index_list[cx];
                    RULE_TYPE_Ace_Entry_T ace_entry;

                    result = RULE_OM_GetAceByIndex(class_id, &ace_entry);
                    ASSERT(RULE_TYPE_OK == result);

                    if (RULE_TYPE_OK != result)
                    {
                        LOG("GetAce fail. aceIndex=%lu", class_id);
                        return result;
                    }

                    if (VAL_diffServMacAceSourcePortOp_equal == ace_entry.u.mac.aceSourcePortOp)
                    {
                        LOG("Can not bind class-map with matching source port on ingress port");
                        return RULE_TYPE_E_HW_LIMIT_MATCH_SOURCE_PORT;
                    }
                }
            }
        }
    }
    else
    {
        UI32_T ifindex;
        UI32_T                      acl_index;
        RULE_TYPE_COUNTER_ENABLE_T  counter_enable;

        result = RULE_CTRL_Interface_To_Ifindex(interface_p, &ifindex);
        ASSERT(RULE_TYPE_OK == result);

        /** For Marvell chip can not bind ACL and PolicyMap on egress port
         * at the same time.
         *
         * Why not ? Can we split ACL into match xx and match any then bind
         * ACL and policy map on the same lookup ? This side effect is small
         * than these both function can't work at the same time.
         */
        if (RULE_TYPE_OK == RULE_OM_GetPortAclIndex(ifindex, FALSE,
                                                    RULE_TYPE_MAC_ACL,
                                                    &acl_index,
                                                    NULL,
                                                    &counter_enable)    ||
            RULE_TYPE_OK == RULE_OM_GetPortAclIndex(ifindex, FALSE,
                                                    RULE_TYPE_IP_ACL,
                                                    &acl_index,
                                                    NULL,
                                                    &counter_enable)    ||
            RULE_TYPE_OK == RULE_OM_GetPortAclIndex(ifindex, FALSE,
                                                    RULE_TYPE_IPV6_ACL,
                                                    &acl_index,
                                                    NULL,
                                                    &counter_enable))
        {
            LOG("Can not bind ACL and Policy-map on egress port at the same time");
            return RULE_TYPE_INSUFFICIENT_RESOURCE;
        }

        /** For Marvell chip can not bind a class-map with remark DSCP for
         * yellow or red packet on egress port.
         * And also can not bind class-map with remaping by PHB on egress port.
         */
        for (i=0; i<policy_map_p->element_count; i++)
        {
            RULE_TYPE_Action_T action_entry;
            RULE_TYPE_TBParamEntry_T meter_entry;

            result = RULE_OM_GetPolicyMapElement(policy_map_p->policy_map_element_index_list[i],
                                                 &policy_element);
            ASSERT(RULE_TYPE_OK == result);

            if (RULE_TYPE_OK != result)
            {
                LOG("GetPolicyMapElement fail. policyMapElementIndex=%lu",
                    policy_map_p->policy_map_element_index_list[i]);
                return result;
            }

            if (RULE_TYPE_OK == RULE_OM_GetActionEntryByIndex(policy_element.action_index, &action_entry) &&
                RULE_TYPE_OK == RULE_OM_GetMeterEntryByIndex(policy_element.meter_index, &meter_entry))
            {
                if (action_entry.action_bitmap & RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP)
                {
                    LOG("Can not bind a class-map with remark DSCP for yellow packet on egress port");
                    return RULE_TYPE_E_HW_LIMIT_YELLOW_REMARK_DSCP;
                }

                if (action_entry.action_bitmap & RULE_TYPE_ACTION_RED_PKT_NEW_DSCP)
                {
                    LOG("Can not bind a class-map with remark DSCP for red packet on egress port");
                    return RULE_TYPE_E_HW_LIMIT_RED_REMARK_DSCP;
                }

                if (action_entry.action_bitmap & RULE_TYPE_ACTION_PKT_NEW_PHB)
                {
                    LOG("Can not bind a class-map with remaping by PHB on egress port");
                    return RULE_TYPE_E_HW_LIMIT_REMAP_BY_PHB;
                }
            }
        }

    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalSetPolicyMap2DEV
 *------------------------------------------------------------------------------
 * PURPOSE  : add rule for policy-map
 * INPUT    : interface_p - interface binding the policy-map
 *            policy_map_index - policy-map index
 *            policy_map_p - policy-map
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetPolicyMap2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    const RULE_TYPE_PolicyMap_T *policy_map_p)
{
    UI32_T                  px;
    RULE_TYPE_RETURN_TYPE_T result;

    //
    // --== Duplicated code start
    //
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T policy_map_inst_obj;
    RULE_CTRL_INSTANCE_TYPE_ID_T param;
    //
    // --== End of duplicated code
    //

    ASSERT(NULL != interface_p);
    ASSERT(interface_p->type < RULE_TYPE_INTERFACE_MAX);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction ||
           RULE_TYPE_OUTBOUND == interface_p->direction);
    ASSERT(NULL != policy_map_p);

    LOG_IF(*interface_p, "policyMapId = %lu", policy_map_index);

    result = RULE_CTRL_LocalCheckPolicyMapOnBinding(interface_p,
                                                         policy_map_index,
                                                         policy_map_p);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    //
    // --== Duplicated code start
    //
    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_POLICY_MAP;
    param.id = policy_map_index;

    policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                       RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                       &param);
    /** Not allow to bind the same policy map twice
     */
    if (NULL != policy_map_inst_p)
    {
        return RULE_TYPE_OK;
    }

    ASSERT(NULL == policy_map_inst_p);

    result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_POLICY_MAP,
                                                         &policy_map_inst_obj);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Failed to create policy-map instance");
        goto undo;
    }

    policy_map_inst_p = policy_map_inst_obj.inst;

    ASSERT(NULL != policy_map_inst_p);
    ASSERT(policy_map_inst_p == policy_map_inst_obj.inst);

    policy_map_inst_p->id = policy_map_index;
    policy_map_inst_p->mvl_pkt_type = RULE_TYPE_PACKET_NIL;

    if_class_obj.add_class_obj(&if_class_obj, &policy_map_inst_obj);

    //
    // Rule OM
    //
    result = RULE_CTRL_Priv_SetPolicyMapInstance(interface_p, policy_map_inst_p);
    ASSERT(RULE_TYPE_OK == result);
    //
    // --== End of duplicated code
    //

    for (px = 0; px < policy_map_p->element_count; ++px)
    {
        UI32_T policy_element_index = policy_map_p->policy_map_element_index_list[px];

        RULE_TYPE_PolicyElement_T   policy_element;

        result = RULE_OM_GetPolicyMapElement(policy_element_index, &policy_element);
        ASSERT(RULE_TYPE_OK == result);

        result = RULE_CTRL_LocalSetClassMap2DEV(interface_p,
                                                policy_map_index,
                                                policy_map_p,
                                                policy_map_inst_obj.inst,
                                                policy_element.class_map_index,
                                                policy_element.meter_index,
                                                policy_element.action_index);
        if (RULE_TYPE_OK != result)
        {
            goto undo;
        }
    }

    return RULE_TYPE_OK;

undo:

    {
        RULE_TYPE_RETURN_TYPE_T temp_result;

        temp_result = RULE_CTRL_LocalRemovePolicyMap2DEV(interface_p,
                                                         policy_map_index,
                                                         policy_map_p);
        ASSERT(RULE_TYPE_OK == temp_result);
    }

    ASSERT(RULE_TYPE_OK != result);
    return result;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalRemovePolicyMap2DEV
 *------------------------------------------------------------------------------
 * PURPOSE  : remove rule for policy-map
 * INPUT    : interface_p - interface binding the policy-map
 *            policy_map_index - policy-map index
 *            policy_map_p - policy-map
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemovePolicyMap2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    const RULE_TYPE_PolicyMap_T *policy_map_p)
{
    UI32_T                  px;
    RULE_TYPE_RETURN_TYPE_T result;

    //
    // --== Duplicated code start
    //
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T policy_map_inst_obj;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;
    //
    // --== End of duplicated code
    //

    ASSERT(NULL != interface_p);
    ASSERT(interface_p->type < RULE_TYPE_INTERFACE_MAX);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction ||
           RULE_TYPE_OUTBOUND == interface_p->direction);
    ASSERT(NULL != policy_map_p);

    LOG_IF(*interface_p, "policyMapId = %lu", policy_map_index);

    //
    // --== Duplicated code start
    //
    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_POLICY_MAP;
    param.id = policy_map_index;

    policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                       RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                       &param);

    if (NULL == policy_map_inst_p)
    {
        return RULE_TYPE_OK;
    }
    //
    // --== End of duplicated code
    //

    for (px = 0; px < policy_map_p->element_count; ++px)
    {
        UI32_T policy_element_index = policy_map_p->policy_map_element_index_list[px];
        RULE_TYPE_PolicyElement_T   policy_element;

        result = RULE_OM_GetPolicyMapElement(policy_element_index, &policy_element);
        ASSERT(RULE_TYPE_OK == result);

        result = RULE_CTRL_LocalRemoveClassMap2DEV(interface_p,
                                                   policy_map_index,
                                                   policy_map_p,
                                                   policy_map_inst_p,
                                                   policy_element.class_map_index,
                                                   policy_element.meter_index,
                                                   policy_element.action_index);
        ASSERT(RULE_TYPE_OK == result);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Internal Error> Failed to remove class-map");
        }
    }

    ASSERT(NULL != policy_map_inst_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(policy_map_inst_p, &policy_map_inst_obj);
    ASSERT(NULL != policy_map_inst_obj.inst &&
           policy_map_inst_p == policy_map_inst_obj.inst);

    if_class_obj.remove_class_obj(&if_class_obj, &policy_map_inst_obj);
    RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&policy_map_inst_obj);

    policy_map_inst_p = NULL;
    policy_map_inst_obj.inst = NULL;

    //
    // Rule OM !!
    //
    result = RULE_CTRL_Priv_SetPolicyMapInstance(interface_p, NULL);
    ASSERT(RULE_TYPE_OK == result);

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalSetClassMap2DEV
 *------------------------------------------------------------------------------
 * PURPOSE  : add rule for class-map
 * INPUT    : interface_p - interface binding the policy-map
 *            policy_map_index - policy-map index
 *            class_map_index - class-map index
 *            meter_id - meter entry index
 *            action_id - action entry index
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetClassMap2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    const RULE_TYPE_PolicyMap_T *policy_map_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    UI32_T class_map_index,
    UI32_T meter_index,
    UI32_T action_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_ClassMap_T class_map;

    //
    // --== Duplicated code start
    //
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T policy_map_inst_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T class_map_inst_obj;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;
    //
    // --== End of duplicated code
    //

    ASSERT(NULL != interface_p);
    ASSERT(interface_p->type < RULE_TYPE_INTERFACE_MAX);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction ||
           RULE_TYPE_OUTBOUND == interface_p->direction);
    ASSERT(NULL != policy_map_p);
    ASSERT(NULL != policy_map_inst_p);
    ASSERT(RULE_TYPE_NIL != policy_map_inst_p->super.type);

    LOG_IF(*interface_p, "policyMapId = %lu, classMapId = %lu, meterId = %lu, actionId = %lu",
           policy_map_index,
           class_map_index,
           meter_index,
           action_index);

    result = RULE_OM_GetClassMap(class_map_index, &class_map);

    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    //
    // --== Duplicated code start
    //
    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(policy_map_inst_p,
                                                   &policy_map_inst_obj);

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_CLASS_MAP;
    param.id = class_map_index;

    class_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) policy_map_inst_p,
                                                                                      RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                      &param);

    /** Not allow to bind the same class map twice
     */
    if (NULL != class_map_inst_p)
    {
        return RULE_TYPE_OK;
    }

    ASSERT(NULL == class_map_inst_p);

    if (NULL == class_map_inst_p)
    {
        result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_CLASS_MAP,
                                                             &class_map_inst_obj);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Failed to create class-map instance");
            goto undo;
        }

        class_map_inst_p = class_map_inst_obj.inst;

        ASSERT(NULL != class_map_inst_p);

        class_map_inst_p->id = class_map_index;
        class_map_inst_p->mvl_pkt_type = RULE_TYPE_PACKET_NIL;

        ASSERT(NULL != policy_map_inst_p);
        ASSERT(policy_map_inst_p == policy_map_inst_obj.inst);

        policy_map_inst_obj.add_class_obj(&policy_map_inst_obj, &class_map_inst_obj);
    }
    //
    // --== End of duplicated code
    //

    switch (class_map.class_map_match_type)
    {
        case RULE_TYPE_CLASS_MAP_MATCH_ANY:
        {
            UI32_T cx;

            for (cx = 0; cx < class_map.class_element_count; ++cx)
            {
                RULE_TYPE_ClassType_T class_type = class_map.class_type[cx];
                UI32_T class_id = class_map.class_index_list[cx];

                if (class_type == RULE_TYPE_CLASS_MF)
                {
                    result = RULE_CTRL_LocalSetMF2DEV(interface_p,
                                                      policy_map_index,
                                                      class_map_index,
                                                      class_id,
                                                      meter_index,
                                                      action_index);
                    if (RULE_TYPE_OK != result)
                    {
                        goto undo;
                    }
                }
                else if (class_type == RULE_TYPE_CLASS_ACL)
                {
#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
                    RULE_TYPE_Acl_T acl_entry;

                    result = RULE_OM_GetAclByIndex(class_id, &acl_entry);
                    if (RULE_TYPE_OK != result)
                    {
                        goto undo;
                    }
                    if (TRUE == acl_entry.flag_aggregate_done)
                    {
                       class_id = acl_entry.aggregate_acl_index;
        	    }
#endif
                    result = RULE_CTRL_LocalSetQoSACL2DEV(interface_p,
                                                          policy_map_index,
                                                          class_map_index,
                                                          class_id,
                                                          meter_index,
                                                          action_index);
                    if (RULE_TYPE_OK != result)
                    {
                        goto undo;
                    }
                }

            }

            break;
        }
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
        case RULE_TYPE_CLASS_MAP_MATCH_ALL:
        {
            UI32_T  agg_acl_index;

            /* Create a working ACL which aggregate all ACE.
             */
            result = RULE_OM_BuildMatchAllClassMapRule(class_map_index, &agg_acl_index);
            if (RULE_TYPE_OK != result)
            {
                goto undo;
            }

            result = RULE_CTRL_LocalSetQoSACL2DEV(interface_p,
                                                  policy_map_index,
                                                  class_map_index,
                                                  agg_acl_index,
                                                  meter_index,
                                                  action_index);
            if (RULE_TYPE_OK != result)
            {
                goto undo;
            }

            break;
        }
#endif /* #if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE) */

        default:
            LOG("<Internal Error> invalid match-type %u", class_map.class_map_match_type);
            ASSERT(0);
            return RULE_TYPE_INVALID_PARAMETER;
    }

    return RULE_TYPE_OK;

undo:
    {
        RULE_TYPE_RETURN_TYPE_T temp_result;

        temp_result = RULE_CTRL_LocalRemoveClassMap2DEV(interface_p,
                                                        policy_map_index,
                                                        policy_map_p,
                                                        policy_map_inst_p,
                                                        class_map_index,
                                                        meter_index,
                                                        action_index);
        ASSERT(RULE_TYPE_OK == temp_result);
    }

    ASSERT(RULE_TYPE_OK != result);
    return result;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalRemoveClassMap2DEV
 *------------------------------------------------------------------------------
 * PURPOSE  : Remove rule for class-map
 * INPUT    : interface_p - interface binding the policy-map
 *            policy_map_index - policy-map index
 *            class_map_index - class-map index
 *            meter_id - meter entry index
 *            action_id - action entry index
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveClassMap2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T  policy_map_index,
    const RULE_TYPE_PolicyMap_T *policy_map_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    UI32_T  class_map_index,
    UI32_T  meter_index,
    UI32_T  action_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_ClassMap_T class_map;

    //
    // --== Duplicated code start
    //
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T policy_map_inst_obj;
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T class_map_inst_obj;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;
    //
    // --== End of duplicated code
    //

    ASSERT(NULL != interface_p);
    ASSERT(interface_p->type < RULE_TYPE_INTERFACE_MAX);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction ||
           RULE_TYPE_OUTBOUND == interface_p->direction);
    ASSERT(NULL != policy_map_p);
    ASSERT(NULL != policy_map_inst_p);
    ASSERT(RULE_TYPE_NIL != policy_map_inst_p->super.type);

    LOG_IF(*interface_p, "policyMapId = %lu, classMapId = %lu, meterId = %lu, actionId = %lu",
           policy_map_index,
           class_map_index,
           meter_index,
           action_index);

    result = RULE_OM_GetClassMap(class_map_index, &class_map);

    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> failed to get class-map(%lu)", class_map_index);
        return result;
    }

    //
    // --== Duplicated code start
    //
    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(policy_map_inst_p, &policy_map_inst_obj);

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_CLASS_MAP;
    param.id = class_map_index;

    class_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) policy_map_inst_p,
                                                                                      RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                      &param);

    if (NULL == class_map_inst_p)
    {
        return RULE_TYPE_OK;
    }

    //
    // --== End of duplicated code
    //

    switch (class_map.class_map_match_type)
    {
        case RULE_TYPE_CLASS_MAP_MATCH_ANY:
        {
            UI32_T cx;

            for (cx = 0; cx < class_map.class_element_count; ++cx)
            {
                RULE_TYPE_ClassType_T class_type = class_map.class_type[cx];
                UI32_T class_id = class_map.class_index_list[cx];

                if (class_type == RULE_TYPE_CLASS_MF)
                {
                    result = RULE_CTRL_LocalRemoveMF2DEV(interface_p,
                                                         policy_map_index,
                                                         class_map_index,
                                                         class_id,
                                                         meter_index,
                                                         action_index);
                    ASSERT(RULE_TYPE_OK == result);
                    if (RULE_TYPE_OK != result)
                    {
                        return result;
                    }
                }
                else if (class_type == RULE_TYPE_CLASS_ACL)
                {
#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
                    RULE_TYPE_Acl_T acl_entry;

                    result = RULE_OM_GetAclByIndex(class_id, &acl_entry);
                    if (RULE_TYPE_OK != result)
                    {
                        return result;
                    }
                    if (TRUE == acl_entry.flag_aggregate_done)
                    {
                        class_id = acl_entry.aggregate_acl_index;
                    }
#endif

                    result = RULE_CTRL_LocalRemoveQoSACL2DEV(interface_p,
                                                             policy_map_index,
                                                             class_map_index,
                                                             class_id,
                                                             meter_index,
                                                             action_index);
                    ASSERT(RULE_TYPE_OK == result);
                    if (RULE_TYPE_OK != result)
                    {
                        return result;
                    }
                }
            }

            break;
        }

#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
        case RULE_TYPE_CLASS_MAP_MATCH_ALL:
        {
            result = RULE_CTRL_LocalRemoveQoSACL2DEV(interface_p,
                                                     policy_map_index,
                                                     class_map_index,
                                                     class_map.class_ext.aggregate_index,
                                                     meter_index,
                                                     action_index);
            ASSERT(RULE_TYPE_OK == result);
            if (RULE_TYPE_OK != result)
            {
                return result;
            }

            if (FALSE == RULE_CTRL_Priv_HaveMatchAllClassMapBeReferred(class_map_index))
            {
                RULE_OM_DelMatchAllClassMapRule(class_map_index);
            }
            break;
        }
#endif /* #if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE) */

        default:
            LOG("<Internal Error> Invalid match-type %u", class_map.class_map_match_type);
            return RULE_TYPE_INVALID_PARAMETER;
    }

    ASSERT(NULL != class_map_inst_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_map_inst_p, &class_map_inst_obj);

    ASSERT(class_map_inst_p == class_map_inst_obj.inst);
    ASSERT(NULL != policy_map_inst_p);
    ASSERT(policy_map_inst_p == policy_map_inst_obj.inst);

    policy_map_inst_obj.remove_class_obj(&policy_map_inst_obj, &class_map_inst_obj);
    RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&class_map_inst_obj);

    class_map_inst_p = NULL;
    class_map_inst_obj.inst = NULL;

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalSyncClassMap2DEV
 *------------------------------------------------------------------------------
 * PURPOSE  : Sync status of class map to device
 * INPUT    : policy_map_index - index of policy map
 *            class_map_index  - index of class map
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : This function do nothing when error occured. The caller need to
 *            consist status between device and OM for this situation.
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSyncClassMap2DEV(
    UI32_T policy_map_index,
    UI32_T class_map_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i;
    //
    // --== Duplicated code start
    //
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;
    RULE_CTRL_INSTANCE_TYPE_ID_T param;
    //
    // --== End of duplicated code
    //

    RULE_TYPE_PolicyMap_T       policy_map;

    RULE_TYPE_ClassMap_T        class_map;
    RULE_TYPE_PolicyElement_T   policy_element;

    result = RULE_OM_GetPolicyMap(policy_map_index, &policy_map);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    result = RULE_OM_GetClassMap(class_map_index, &class_map);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    result = RULE_OM_GetPolicyMapElementFromPolicyMapByClassMap(policy_map_index, class_map_index, &policy_element);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    for (i = 0; i < _countof(shmem_data_p->if_table); ++ i)
    {
        RULE_TYPE_INTERFACE_INFO_T interface;

        RULE_CTRL_Priv_IfEntry_Index_To_Interface(i, &interface);

        //
        // --== Duplicated code start
        //
        if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
        ASSERT(NULL != if_entry_p);

        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                  &if_class_obj);

        memset(&param, 0, sizeof(param));

        param.type = RULE_TYPE_INST_POLICY_MAP;
        param.id = policy_map_index;

        policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                           RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                           &param);
        //
        // --== End of duplicated code
        //
        if (NULL == policy_map_inst_p)
        {
            continue;
        }

        result = RULE_CTRL_LocalRemoveClassMap2DEV(&interface, policy_map_index, &policy_map,
                                                   policy_map_inst_p,
                                                   policy_element.class_map_index,
                                                   policy_element.meter_index,
                                                   policy_element.action_index);
        ASSERT(RULE_TYPE_OK == result);
        if (RULE_TYPE_OK != result)
        {
            return result;
        }

        result = RULE_CTRL_LocalSetClassMap2DEV(&interface, policy_map_index, &policy_map,
                                                policy_map_inst_p,
                                                policy_element.class_map_index,
                                                policy_element.meter_index,
                                                policy_element.action_index);
        if (RULE_TYPE_OK != result)
        {
            return result;
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalSetQoSACL2DEV
 *------------------------------------------------------------------------------
 * PURPOSE  : Add rule for ACL of class-map
 * INPUT    : interface_p - interface binding the policy-map
 *            policy_map_index - policy-map index
 *            class_map_index - class-map index
 *            acl_id - aggregate_acl_index
 *            meter_id - meter entry index
 *            action_id - action entry index
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetQoSACL2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T acl_id,
    UI32_T meter_id,
    UI32_T action_id)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T ace_index;
    UI32_T remapped_pkt_type;

    RULE_TYPE_AclType_T ace_type;

    //
    // --== Duplicated code start
    //
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T policy_map_inst_obj;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T class_map_inst_iter;

    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T class_map_inst_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;
    //
    // --== End of duplicated code
    //

    RULE_TYPE_Acl_T acl_entry;
    RULE_TYPE_Ace_Entry_T ace_entry;

    int priority;
    UI32_T ui_acl_index;

    RULE_TYPE_TBParamEntry_T meter_entry;
    RULE_TYPE_Action_T action_entry;

    ASSERT(NULL != interface_p);
    ASSERT(RULE_TYPE_INTERFACE_UPORT == interface_p->type ||
           RULE_TYPE_INTERFACE_TRUNK == interface_p->type ||
           RULE_TYPE_INTERFACE_CPU_PORT == interface_p->type);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction ||
           RULE_TYPE_OUTBOUND == interface_p->direction);

    LOG_IF(*interface_p, "policyMapId = %lu, classMapId = %lu, aclId = %lu, meterId = %lu, actionId = %lu",
           policy_map_index,
           class_map_index,
           acl_id,
           meter_id,
           action_id);

    policy_map_inst_p = RULE_CTRL_Priv_GetPolicyMapInstance(interface_p);
    if (NULL != policy_map_inst_p)
    {
        ASSERT(policy_map_inst_p->policy_map_id == policy_map_index);
        if (policy_map_index != policy_map_inst_p->policy_map_id)
        {
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    result = RULE_OM_GetAclByIndex(acl_id, &acl_entry);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    if (acl_entry.flag_hide == TRUE)
    {
        ui_acl_index = acl_entry.owner.class_id;
    }
    else
#endif
    {
        ui_acl_index = acl_id;
    }

    result = RULE_CTRL_Priv_GetClassMapElementPriorityInClassMap(policy_map_index,
                                                                 class_map_index,
                                                                 RULE_TYPE_CLASS_ACL,
                                                                 ui_acl_index,
                                                                 &priority);
    ASSERT(RULE_TYPE_OK == result);

    result = RULE_OM_GetActionEntryByIndex(action_id, &action_entry);
    ASSERT(RULE_TYPE_OK == result);

    result = RULE_OM_GetMeterEntryByIndex(meter_id, &meter_entry);
    if (RULE_TYPE_OK != result)
    {
        meter_id = 0;
    }

    remapped_pkt_type = (RULE_TYPE_INTERFACE_CPU_PORT == interface_p->type) ?
                            RULE_TYPE_PACKET_CPU_PORT_EGRESS_QOS :
                        (RULE_TYPE_INTERFACE_TRUNK == interface_p->type) ?
                            RULE_TYPE_PACKET_TRUNK_QOS :
                        (RULE_TYPE_INBOUND == interface_p->direction) ?
                            RULE_TYPE_PACKET_QOS :
                            RULE_TYPE_PACKET_EGRESS_QOS;

    //
    // --== Duplicated code start
    //
    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_POLICY_MAP;
    param.id = policy_map_index;

    policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                       RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                       &param);

    if (NULL == policy_map_inst_p)
    {
        // no run here !!
        ASSERT(0);

        result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_POLICY_MAP,
                                                             &policy_map_inst_obj);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Failed to create policy-map instance");
            goto undo;
        }

        policy_map_inst_p = policy_map_inst_obj.inst;

        ASSERT(NULL != policy_map_inst_p);

        policy_map_inst_p->id = policy_map_index;
        policy_map_inst_p->mvl_pkt_type = RULE_TYPE_PACKET_NIL;

        if_class_obj.add_class_obj(&if_class_obj, &policy_map_inst_obj);

        //
        // Rule OM
        //
        result = RULE_CTRL_Priv_SetPolicyMapInstance(interface_p, policy_map_inst_p);
        ASSERT(RULE_TYPE_OK == result);
    }
    else
    {
        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(policy_map_inst_p,
                                                       &policy_map_inst_obj);
    }

    policy_map_inst_obj.get_element_iterator(&policy_map_inst_obj, &class_map_inst_iter);

    for (class_map_inst_p = NULL, class_map_inst_iter.first(&class_map_inst_iter);
         NULL != class_map_inst_iter.get_instance(&class_map_inst_iter);
         class_map_inst_iter.next(&class_map_inst_iter))
    {
        class_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)class_map_inst_iter.get_instance(&class_map_inst_iter);

        if (class_map_index == class_map_inst_p->id)
        {
            break;
        }

        class_map_inst_p = NULL;
    }

    if (NULL == class_map_inst_p)
    {
        // no run here !!
        ASSERT(0);

        result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_CLASS_MAP,
                                                             &class_map_inst_obj);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Failed to create class-map instance");
            goto undo;
        }

        class_map_inst_p = class_map_inst_obj.inst;

        ASSERT(NULL != class_map_inst_p);

        class_map_inst_p->id = class_map_index;
        class_map_inst_p->mvl_pkt_type = RULE_TYPE_PACKET_NIL;

        ASSERT(NULL != policy_map_inst_p);
        ASSERT(policy_map_inst_p == policy_map_inst_obj.inst);

        policy_map_inst_obj.add_class_obj(&policy_map_inst_obj, &class_map_inst_obj);
    }
    else
    {
        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_map_inst_p,
                                                       &class_map_inst_obj);
    }
    //
    // --== End of duplicated code
    //

    ASSERT(NULL != policy_map_inst_p);
    ASSERT(policy_map_inst_p == policy_map_inst_obj.inst);
    ASSERT(NULL != class_map_inst_p);
    ASSERT(class_map_inst_p == class_map_inst_obj.inst);

    /**
     * Create ACL instance
     */
    result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl_inst_obj);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Failed to create ACL instance");
        goto undo;
    }

    acl_inst_p = acl_inst_obj.inst;

    ASSERT(NULL != acl_inst_p);

    acl_inst_p->acl_id = acl_id;
    acl_inst_p->mvl_pkt_type = RULE_TYPE_PACKET_NIL;

    /**
     * Add ACL instance into class-map
     */
    class_map_inst_obj.add_class_obj(&class_map_inst_obj, &acl_inst_obj);

    ace_index = 0;
    while (RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_id, &ace_type, &ace_index, &ace_entry))
    {
        RULE_TYPE_CLASS_INSTANCE_PTR_T ace_inst_p;
        RULE_TYPE_CLASS_INSTANCE_OBJECT_T ace_inst_obj;

        DEVRM_AceEntry_T rule_pattern;

        DEVRM_LIB_RulePatternInit(&rule_pattern);
        result = RULE_CTRL_LocalConvertAce2Dev(&ace_entry, &rule_pattern);
        ASSERT(RULE_TYPE_OK == result);

        /* If SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL is FALSE,
         *   we do not configure an ace with deny action to chip (for DiffServ).
         */
#if (SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL != TRUE)
        if (RULE_TYPE_ACE_DENY == ace_entry.access)
        {
            continue;
        }
#endif /* #if (SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL != TRUE) */

        /**
         * Create ACE instance
         */
        result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACE, &ace_inst_obj);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Failed to create MF/ACE instance");
            goto undo;
        }

        ace_inst_p = ace_inst_obj.inst;

        ASSERT(NULL != ace_inst_p);

        ace_inst_p->id = ace_index;
        ace_inst_p->mvl_pkt_type = remapped_pkt_type;

        acl_inst_obj.add_class_obj(&acl_inst_obj, &ace_inst_obj);

        result = RULE_CTRL_EACH_UNIT_DEVICE(interface_p,
                                            RULE_CTRL_LocalAddQosRule_To_Dev,
                                            interface_p,
                                            remapped_pkt_type,
                                            &class_map_inst_obj,
                                            &ace_inst_obj,
                                            ace_index, &ace_entry,
                                            meter_id, 0 != meter_id ? &meter_entry : NULL,
                                            action_id, &action_entry,
                                            priority);
        if (RULE_TYPE_OK != result)
        {
            goto undo;
        }
    }

    return RULE_TYPE_OK;

undo:

    {
        RULE_TYPE_RETURN_TYPE_T temp_result;

        temp_result = RULE_CTRL_LocalRemoveQoSACL2DEV(interface_p,
                                                      policy_map_index,
                                                      class_map_index,
                                                      acl_id,
                                                      meter_id,
                                                      action_id);
        ASSERT(RULE_TYPE_OK == temp_result);
    }

    ASSERT(RULE_TYPE_OK != result);
    return result;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalRemoveQoSACL2DEV
 *------------------------------------------------------------------------------
 * PURPOSE  : Remove rule for ACL of class-map
 * INPUT    : interface_p - interface binding the policy-map
 *            policy_map_index - policy-map index
 *            class_map_index - class-map index
 *            acl_id - aggregate_acl_index
 *            meter_id - meter entry index
 *            action_id - action entry index
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveQoSACL2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T acl_id,
    UI32_T meter_id,
    UI32_T action_id)
{
    RULE_TYPE_RETURN_TYPE_T result;

    //
    // --== Duplicated code start
    //
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T policy_map_inst_obj;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T class_map_inst_iter;

    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T class_map_inst_obj;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;
    //
    // --== End of duplicated code
    //

    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T acl_mf_ace_inst_iter;
    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;

    ASSERT(NULL != interface_p);
    ASSERT(RULE_TYPE_INTERFACE_UPORT == interface_p->type ||
           RULE_TYPE_INTERFACE_TRUNK == interface_p->type ||
           RULE_TYPE_INTERFACE_CPU_PORT == interface_p->type);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction ||
           RULE_TYPE_OUTBOUND == interface_p->direction);

    LOG_IF(*interface_p, "policyMapId = %lu, classMapId = %lu, aclId = %lu, meterId = %lu, actionId = %lu",
           policy_map_index,
           class_map_index,
           acl_id,
           meter_id,
           action_id);

    //
    // --== Duplicated code start
    //
    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_POLICY_MAP;
    param.id = policy_map_index;

    policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                       RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                       &param);
    if (NULL == policy_map_inst_p)
    {
        return RULE_TYPE_OK;
    }

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(policy_map_inst_p, &policy_map_inst_obj);
    policy_map_inst_obj.get_element_iterator(&policy_map_inst_obj, &class_map_inst_iter);

    for (class_map_inst_p = NULL, class_map_inst_iter.first(&class_map_inst_iter);
         NULL != class_map_inst_iter.get_instance(&class_map_inst_iter);
         class_map_inst_iter.next(&class_map_inst_iter))
    {
        class_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)class_map_inst_iter.get_instance(&class_map_inst_iter);

        if (class_map_index == class_map_inst_p->id)
        {
            break;
        }

        class_map_inst_p = NULL;
    }

    if (NULL == class_map_inst_p)
    {
        goto undo_policy_map_instance;
    }

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_map_inst_p, &class_map_inst_obj);
    //
    // --== End of duplicated code
    //

    ASSERT(NULL != policy_map_inst_p);
    ASSERT(policy_map_inst_p == policy_map_inst_obj.inst);
    ASSERT(NULL != class_map_inst_p);
    ASSERT(class_map_inst_p == class_map_inst_obj.inst);

    class_map_inst_obj.get_element_iterator(&class_map_inst_obj, &acl_mf_ace_inst_iter);
    for (acl_inst_p = NULL, acl_mf_ace_inst_iter.first(&acl_mf_ace_inst_iter);
         NULL != acl_mf_ace_inst_iter.get_instance(&acl_mf_ace_inst_iter);
         acl_mf_ace_inst_iter.next(&acl_mf_ace_inst_iter))
    {
        acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)acl_mf_ace_inst_iter.get_instance(&acl_mf_ace_inst_iter);

        if (RULE_TYPE_INST_ACL == acl_inst_p->super.type &&
            acl_inst_p->id == acl_id)
        {
            break;
        }

        acl_inst_p = NULL;
    }

    if (NULL != acl_inst_p)
    {
        result = RULE_CTRL_LocalFreeRulesByRuleGroup(acl_inst_p);
        ASSERT(RULE_TYPE_OK == result);
        if (RULE_TYPE_OK != result)
        {
            return result;
        }

        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p, &acl_inst_obj);

        class_map_inst_obj.remove_class_obj(&class_map_inst_obj, &acl_inst_obj);
        RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&acl_inst_obj);

        acl_inst_p = NULL;
        acl_inst_obj.inst = NULL;
    }

    //
    // --== Duplicated code start
    //
    ASSERT(NULL != class_map_inst_p);
    ASSERT(class_map_inst_p == class_map_inst_obj.inst);
    ASSERT(NULL != policy_map_inst_p);
    ASSERT(policy_map_inst_p == policy_map_inst_obj.inst);

    if (0)
    {
    //
    // If no any mf/ace or acl in class-map, remove it
    //
    //
    // First child
    //
    if (RULE_TYPE_NIL == class_map_inst_obj.inst->super.links.first_node.type)
    {
        policy_map_inst_obj.remove_class_obj(&policy_map_inst_obj, &class_map_inst_obj);
        RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&class_map_inst_obj);

        class_map_inst_p = NULL;
        class_map_inst_obj.inst = NULL;
    }
    }

undo_policy_map_instance:

    ASSERT(NULL != policy_map_inst_p);
    ASSERT(policy_map_inst_p == policy_map_inst_obj.inst);
    ASSERT(NULL != if_class_obj.inst);

    if (0)
    {
    //
    // If no any class-map in policy-map, remove it
    //
    if (RULE_TYPE_NIL == policy_map_inst_obj.inst->super.links.first_node.type)
    {
        if_class_obj.remove_class_obj(&if_class_obj, &policy_map_inst_obj);
        RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&policy_map_inst_obj);

        policy_map_inst_p = NULL;
        policy_map_inst_obj.inst = NULL;

        //
        // Rule OM !!
        //
        result = RULE_CTRL_Priv_SetPolicyMapInstance(interface_p, NULL);
        ASSERT(RULE_TYPE_OK == result);
    }
    }
    //
    // --== Duplicated code end
    //

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalSetMF2DEV
 *------------------------------------------------------------------------------
 * PURPOSE  : Add rule for MF ACE of class-map
 * INPUT    : interface_p - interface binding the policy-map
 *            policy_map_index - policy-map index
 *            class_map_index - class-map index
 *            mf_index - MF ACE index
 *            meter_id - meter entry index
 *            action_id - action entry index
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetMF2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T mf_index,
    UI32_T meter_id,
    UI32_T action_id)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T remapped_pkt_type;

    //
    // --== Duplicated code start
    //
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T policy_map_inst_obj;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T class_map_inst_iter;

    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T class_map_inst_obj;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;
    //
    // --== End of duplicated code
    //

    RULE_TYPE_CLASS_INSTANCE_PTR_T mf_ace_inst_p = NULL;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T mf_ace_inst_obj;

    RULE_TYPE_Ace_Entry_T ace_entry;

    int priority;

    RULE_TYPE_TBParamEntry_T meter_entry;
    RULE_TYPE_Action_T action_entry;

    ASSERT(NULL != interface_p);
    ASSERT(RULE_TYPE_INTERFACE_UPORT == interface_p->type ||
           RULE_TYPE_INTERFACE_TRUNK == interface_p->type ||
           RULE_TYPE_INTERFACE_CPU_PORT == interface_p->type);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction ||
           RULE_TYPE_OUTBOUND == interface_p->direction);

    LOG_IF(*interface_p, "policyMapId = %lu, classMapId = %lu, mfId = %lu, meterId = %lu, actionId = %lu",
           policy_map_index,
           class_map_index,
           mf_index,
           meter_id,
           action_id);

    policy_map_inst_p = RULE_CTRL_Priv_GetPolicyMapInstance(interface_p);
    if (NULL != policy_map_inst_p)
    {
        ASSERT(policy_map_inst_p->policy_map_id == policy_map_index);
        if (policy_map_index != policy_map_inst_p->policy_map_id)
        {
            ASSERT(0);
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    memset(&ace_entry, 0, sizeof(RULE_TYPE_Ace_Entry_T));

    result = RULE_OM_Get_AceEntryByID(mf_index, &ace_entry);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    result = RULE_CTRL_Priv_GetClassMapElementPriorityInClassMap(policy_map_index,
                                                                 class_map_index,
                                                                 RULE_TYPE_CLASS_MF,
                                                                 mf_index,
                                                                 &priority);
    ASSERT(RULE_TYPE_OK == result);

    result = RULE_OM_GetActionEntryByIndex(action_id, &action_entry);
    ASSERT(RULE_TYPE_OK == result);

    result = RULE_OM_GetMeterEntryByIndex(meter_id, &meter_entry);
    if (RULE_TYPE_OK != result)
    {
        meter_id = 0;
    }

    remapped_pkt_type = (RULE_TYPE_INTERFACE_CPU_PORT == interface_p->type) ?
                            RULE_TYPE_PACKET_CPU_PORT_EGRESS_QOS :
                        (RULE_TYPE_INTERFACE_TRUNK == interface_p->type) ?
                            RULE_TYPE_PACKET_TRUNK_QOS :
                        (RULE_TYPE_INBOUND == interface_p->direction) ?
                            RULE_TYPE_PACKET_QOS :
                            RULE_TYPE_PACKET_EGRESS_QOS;

    //
    // --== Duplicated code start
    //
    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_POLICY_MAP;
    param.id = policy_map_index;

    policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                       RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                       &param);

    if (NULL == policy_map_inst_p)
    {
        // no run here !!
        ASSERT(0);

        result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_POLICY_MAP,
                                                             &policy_map_inst_obj);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Failed to create policy-map instance");
            goto undo;
        }

        policy_map_inst_p = policy_map_inst_obj.inst;

        ASSERT(NULL != policy_map_inst_p);

        policy_map_inst_p->id = policy_map_index;
        policy_map_inst_p->mvl_pkt_type = RULE_TYPE_PACKET_NIL;

        if_class_obj.add_class_obj(&if_class_obj, &policy_map_inst_obj);

        //
        // Rule OM
        //
        result = RULE_CTRL_Priv_SetPolicyMapInstance(interface_p, policy_map_inst_p);
        ASSERT(RULE_TYPE_OK == result);
    }
    else
    {
        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(policy_map_inst_p,
                                                       &policy_map_inst_obj);
    }

    policy_map_inst_obj.get_element_iterator(&policy_map_inst_obj, &class_map_inst_iter);

    for (class_map_inst_p = NULL, class_map_inst_iter.first(&class_map_inst_iter);
         NULL != class_map_inst_iter.get_instance(&class_map_inst_iter);
         class_map_inst_iter.next(&class_map_inst_iter))
    {
        class_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)class_map_inst_iter.get_instance(&class_map_inst_iter);

        if (class_map_index == class_map_inst_p->id)
        {
            break;
        }

        class_map_inst_p = NULL;
    }

    if (NULL == class_map_inst_p)
    {
        // no run here !!
        ASSERT(0);

        result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_CLASS_MAP,
                                                             &class_map_inst_obj);
        if (RULE_TYPE_OK != result)
        {
            LOG("<Error> Failed to create class-map instance");
            goto undo;
        }

        class_map_inst_p = class_map_inst_obj.inst;

        ASSERT(NULL != class_map_inst_p);

        class_map_inst_p->id = class_map_index;
        class_map_inst_p->mvl_pkt_type = RULE_TYPE_PACKET_NIL;

        ASSERT(NULL != policy_map_inst_p);
        ASSERT(policy_map_inst_p == policy_map_inst_obj.inst);

        policy_map_inst_obj.add_class_obj(&policy_map_inst_obj, &class_map_inst_obj);
    }
    else
    {
        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_map_inst_p,
                                                       &class_map_inst_obj);
    }
    //
    // --== End of duplicated code
    //

    ASSERT(NULL != policy_map_inst_p);
    ASSERT(policy_map_inst_p == policy_map_inst_obj.inst);
    ASSERT(NULL != class_map_inst_p);
    ASSERT(class_map_inst_p == class_map_inst_obj.inst);

    /**
     * Create MF (class) instance
     */
    result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_MF_ACE, &mf_ace_inst_obj);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Failed to create MF/ACE instance");
        goto undo;
    }

    mf_ace_inst_p = mf_ace_inst_obj.inst;

    ASSERT(NULL != mf_ace_inst_p);

    mf_ace_inst_p->id = mf_index;
    mf_ace_inst_p->mvl_pkt_type = remapped_pkt_type;

    /**
     * Add MF (class) instance into class-map
     */
    class_map_inst_obj.add_class_obj(&class_map_inst_obj, &mf_ace_inst_obj);

    result = RULE_CTRL_EACH_UNIT_DEVICE(interface_p,
                                        RULE_CTRL_LocalAddQosRule_To_Dev,
                                        interface_p,
                                        remapped_pkt_type,
                                        &class_map_inst_obj,
                                        &mf_ace_inst_obj,
                                        mf_index, &ace_entry,
                                        meter_id, 0 != meter_id ? &meter_entry : NULL,
                                        action_id, &action_entry,
                                        priority);
    if (RULE_TYPE_OK != result)
    {
        goto undo;
    }

    return RULE_TYPE_OK;

undo:

    {
        RULE_TYPE_RETURN_TYPE_T temp_result;

        temp_result = RULE_CTRL_LocalRemoveMF2DEV(interface_p,
                                                  policy_map_index,
                                                  class_map_index,
                                                  mf_index,
                                                  meter_id,
                                                  action_id);
        ASSERT(RULE_TYPE_OK == temp_result);
    }

    ASSERT(RULE_TYPE_OK != result);
    return result;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalRemoveMF2DEV
 *------------------------------------------------------------------------------
 * PURPOSE  : Remove rule for MF ACE of class-map
 * INPUT    : interface_p - interface binding the policy-map
 *            policy_map_index - policy-map index
 *            class_map_index - class-map index
 *            mf_index - MF ACE index
 *            meter_id - meter entry index
 *            action_id - action entry index
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveMF2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T mf_index,
    UI32_T meter_id,
    UI32_T action_id)
{
    RULE_TYPE_RETURN_TYPE_T result;

    //
    // --== Duplicated code start
    //
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T policy_map_inst_obj;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T class_map_inst_iter;

    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T class_map_inst_obj;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;
    //
    // --== End of duplicated code
    //

    RULE_TYPE_CLASS_INSTANCE_PTR_T ace_inst_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T ace_inst_obj;

    ASSERT(NULL != interface_p);
    ASSERT(RULE_TYPE_INTERFACE_UPORT == interface_p->type ||
           RULE_TYPE_INTERFACE_TRUNK == interface_p->type ||
           RULE_TYPE_INTERFACE_CPU_PORT == interface_p->type);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction ||
           RULE_TYPE_OUTBOUND == interface_p->direction);

    LOG_IF(*interface_p, "policyMapId = %lu, classMapId = %lu, mfId = %lu, meterId = %lu, actionId = %lu",
           policy_map_index,
           class_map_index,
           mf_index,
           meter_id,
           action_id);

    //
    // --== Duplicated code start
    //
    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_POLICY_MAP;
    param.id = policy_map_index;

    policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                       RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                       &param);

    if (NULL == policy_map_inst_p)
    {
        return RULE_TYPE_OK;
    }

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(policy_map_inst_p, &policy_map_inst_obj);
    policy_map_inst_obj.get_element_iterator(&policy_map_inst_obj, &class_map_inst_iter);

    for (class_map_inst_p = NULL, class_map_inst_iter.first(&class_map_inst_iter);
         NULL != class_map_inst_iter.get_instance(&class_map_inst_iter);
         class_map_inst_iter.next(&class_map_inst_iter))
    {
        class_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)class_map_inst_iter.get_instance(&class_map_inst_iter);

        if (class_map_index == class_map_inst_p->id)
        {
            break;
        }

        class_map_inst_p = NULL;
    }

    if (NULL == class_map_inst_p)
    {
        goto undo_policy_map_instance;
    }

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_map_inst_p, &class_map_inst_obj);
    //
    // --== End of duplicated code
    //

    ASSERT(NULL != policy_map_inst_p);
    ASSERT(policy_map_inst_p == policy_map_inst_obj.inst);
    ASSERT(NULL != class_map_inst_p);
    ASSERT(class_map_inst_p == class_map_inst_obj.inst);

    param.type = RULE_TYPE_INST_MF_ACE;
    param.id = mf_index;

    ace_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_IN_Priv_FindIf((RULE_TYPE_INSTANCE_PTR_T)class_map_inst_p,
                                                                          RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                          &param);
    if (NULL != ace_inst_p)
    {
        result = RULE_CTRL_LocalFreeRulesByRuleGroup(ace_inst_p);
        ASSERT(RULE_TYPE_OK == result);
        if (RULE_TYPE_OK != result)
        {
            return result;
        }

        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(ace_inst_p, &ace_inst_obj);

        class_map_inst_obj.remove_class_obj(&class_map_inst_obj, &ace_inst_obj);
        RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&ace_inst_obj);

        ace_inst_p = NULL;
        ace_inst_obj.inst = NULL;
    }

    //
    // --== Duplicated code start
    //
    ASSERT(NULL != class_map_inst_p);
    ASSERT(class_map_inst_p == class_map_inst_obj.inst);
    ASSERT(NULL != policy_map_inst_p);
    ASSERT(policy_map_inst_p == policy_map_inst_obj.inst);

undo_policy_map_instance:

    ASSERT(NULL != policy_map_inst_p);
    ASSERT(policy_map_inst_p == policy_map_inst_obj.inst);
    ASSERT(NULL != if_class_obj.inst);

    //
    // --== Duplicated code end
    //

    return RULE_TYPE_OK;
}

#pragma mark -
#pragma mark #if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalGetMeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : Get associating meter entry with class-map
 * INPUT    : policy_map_index  - policy-map index
 *            class_map_index   - class-map index
 * OUTPUT   : meter_p           - meter entry
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalGetMeterEntry(
    UI32_T policy_map_index,
    UI32_T class_map_index,
    RULE_TYPE_TBParamEntry_T *meter_p)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_PolicyElement_T policy_element;

    ASSERT(NULL != meter_p);

    result = RULE_OM_GetPolicyMapElementEntry(policy_map_index,
                                           class_map_index,
                                           &policy_element);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    return RULE_OM_GetMeterEntryByIndex(policy_element.meter_index, meter_p);
}

typedef struct
{
    UI32_T new_cir;
    BOOL_T done;
} RULE_CTRL_SET_CIR_PARAMETER_T;

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalUpdateMeterCIR
 *------------------------------------------------------------------------------
 * PURPOSE  : Update meter's CIR
 * INPUT    : class_instance_p  - class instance object
 *            rule_instance_p   - rule instance object
 *            cookie            - point to RULE_CTRL_SET_CIR_PARAMETER_T
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_FAIL
 * NOTE     : Update meeter for first rule in class-map only.
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalUpdateMeterCIR(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p,
    RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p,
    void *cookie)
{
    RULE_CTRL_SET_CIR_PARAMETER_T *param = cookie;

    ASSERT(NULL != interface_p);
    ASSERT(NULL != policy_map_inst_p);
    ASSERT(NULL != class_map_inst_p);
    ASSERT(NULL != rule_inst_p);
    ASSERT(NULL != param);

    if (TRUE != param->done)
    {
        RULE_TYPE_RETURN_TYPE_T result;

        RULE_TYPE_TBParamEntry_T    meter_entry;

        DEVRM_ACTION_T              dev_action;

        BOOL_T b_result;

        BOOL_T b_changed;

        DEVRM_LIB_ActionInit(&dev_action);

        result = RULE_CTRL_LocalGetMeterEntry(policy_map_inst_p->id,
                                              class_map_inst_p->id,
                                              &meter_entry);
        ASSERT(RULE_TYPE_OK == result);
        if (RULE_TYPE_OK != result)
        {
            return RULE_TYPE_FAIL;
        }

        if (meter_entry.rate != param->new_cir)
        {
            b_changed = TRUE;
        }
        else
        {
            b_changed = FALSE;
        }

        meter_entry.rate = param->new_cir;

        result = RULE_CTRL_LocalConvertMeter2Dev(&meter_entry, &dev_action);
        ASSERT(RULE_TYPE_OK == result);

        b_result = RULE_CTRL_LocalUpdateMeter(rule_inst_p->dev_rule_info.unit,
                                              1,
                                              rule_inst_p->dev_rule_info.device,
                                              rule_inst_p->dev_rule_info.pcl_id,
                                              rule_inst_p->dev_rule_info.rule_id,
                                              &dev_action.policer.meter);

        if (TRUE == b_changed)
        {
            char policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
            char class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];

            RULE_OM_GetPolicyMapNameById(policy_map_inst_p->policy_map_id, policy_map_name);
            RULE_OM_GetClassMapNameById(class_map_inst_p->class_map_id, class_map_name);

            policy_map_name[sizeof(policy_map_name) - 1] = '\0';
            class_map_name[sizeof(class_map_name) - 1] = '\0';

            LOG("policy_map=%s, class_map=%s, unit=%lu, device=%lu, pcl_id=%lu, dev_rule_id=%lu, new_CIR=%lu, new_CBS=%lu, [%s]\r\n",
                policy_map_name,
                class_map_name,
                rule_inst_p->dev_rule_info.unit,
                rule_inst_p->dev_rule_info.device,
                rule_inst_p->dev_rule_info.pcl_id,
                rule_inst_p->dev_rule_info.rule_id,
                meter_entry.rate,
                meter_entry.burst_size,
                (TRUE == b_result) ? "success" : "fail");
        }

        /* Stop caller function
         */
        if (TRUE == b_result)
        {
            param->done = TRUE;
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalGetInputCount
 *------------------------------------------------------------------------------
 * PURPOSE  : Get input count from device per each flow in class-map
 * INPUT    : class_instance_p  - class instance object
 *            rule_instance_p   - rule instance object
 *            cookie            - no used
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalGetInputCount(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p,
    RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p,
    void *cookie)
{
    UI32_T cci;

    RULE_TYPE_CounterRec_T *current_counter_rec_p;

    ASSERT(NULL != interface_p);
    ASSERT(NULL != policy_map_inst_p);
    ASSERT(NULL != class_map_inst_p);
    ASSERT(NULL != rule_inst_p);

    rule_inst_p->current_counter_index =
        (rule_inst_p->current_counter_index+1) % _countof(rule_inst_p->counter);

    cci = rule_inst_p->current_counter_index;

    current_counter_rec_p = &rule_inst_p->counter[cci];

    current_counter_rec_p->last_update_tick = SYSFUN_GetSysTick();

    RULE_CTRL_LocalGetCounter(rule_inst_p->dev_rule_info.unit,
                              rule_inst_p->dev_rule_info.device,
                              rule_inst_p->dev_rule_info.pcl_id,
                              rule_inst_p->dev_rule_info.rule_id,
                              &current_counter_rec_p->green_octets,
                              &current_counter_rec_p->yellow_octets,
                              &current_counter_rec_p->red_octets);

    {
        UI32_T prev_counter_index = (rule_inst_p->current_counter_index+(_countof(rule_inst_p->counter)-1)) % _countof(rule_inst_p->counter);
        RULE_TYPE_CounterRec_T *prev_counter_rec_p = current_counter_rec_p = &rule_inst_p->counter[prev_counter_index];

        if (prev_counter_rec_p->green_octets != current_counter_rec_p->green_octets ||
            prev_counter_rec_p->yellow_octets != current_counter_rec_p->yellow_octets ||
            prev_counter_rec_p->red_octets != current_counter_rec_p->red_octets)
        {
            char policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
            char class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];

            RULE_OM_GetPolicyMapNameById(policy_map_inst_p->policy_map_id, policy_map_name);
            RULE_OM_GetClassMapNameById(class_map_inst_p->class_map_id, class_map_name);

            policy_map_name[sizeof(policy_map_name) - 1] = '\0';
            class_map_name[sizeof(class_map_name) - 1] = '\0';

            LOG("policy_map=%s, class_map=%s, unit=%lu, device=%lu, pcl_id=%lu, dev_rule_id=%lu, GB=%lu, YB=%lu, RB=%lu\r\n",
                policy_map_name,
                class_map_name,
                rule_inst_p->dev_rule_info.unit,
                rule_inst_p->dev_rule_info.device,
                rule_inst_p->dev_rule_info.pcl_id,
                rule_inst_p->dev_rule_info.rule_id,
                current_counter_rec_p->green_octets,
                current_counter_rec_p->yellow_octets,
                current_counter_rec_p->red_octets);
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalConvertCountToRate
 *------------------------------------------------------------------------------
 * PURPOSE  : Convert count to rate per each flow in class-map
 * INPUT    : class_instance_p  - class instance object
 *            rule_instance_p   - rule instance object
 *            cookie            - point to a UI32_T means total rate
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertCountToRate(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p,
    RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p,
    void *cookie)
{
    UI32_T *current_rate_kbps_p = cookie;
    UI32_T cci;
    RULE_TYPE_CounterRec_T *current_counter_rec_p;
    RULE_TYPE_CounterRec_T *last_3_counter_rec_p;
    UI32_T octets;

    ASSERT(NULL != interface_p);
    ASSERT(NULL != policy_map_inst_p);
    ASSERT(NULL != class_map_inst_p);
    ASSERT(NULL != rule_inst_p);

    cci = rule_inst_p->current_counter_index;

    current_counter_rec_p = &rule_inst_p->counter[cci];

    /* Calculate rate
     * R = (current_oct-last_3_oct) / (current_time-last_3_time)
     */
    last_3_counter_rec_p =
        &rule_inst_p->counter[ (cci + _countof(rule_inst_p->counter) - 1) % _countof(rule_inst_p->counter) ];

    /* Not get counter yet
     */
    if (0 == current_counter_rec_p->last_update_tick
        || 0 == last_3_counter_rec_p->last_update_tick)
    {
        return RULE_TYPE_OK;
    }

    octets = current_counter_rec_p->green_octets
            + current_counter_rec_p->yellow_octets
            + current_counter_rec_p->red_octets
            - last_3_counter_rec_p->green_octets
            - last_3_counter_rec_p->yellow_octets
            - last_3_counter_rec_p->red_octets
            ;

    *current_rate_kbps_p += (float)
            ((octets * 8)/1000) /
            ((current_counter_rec_p->last_update_tick - last_3_counter_rec_p->last_update_tick) / RULE_TYPE_TIMER);
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalGetBundleInputRate
 *------------------------------------------------------------------------------
 * PURPOSE  : Get input rate per each flow in bundle group
 * INPUT    : ifindex           - ifindex
 *            inout_profile     - direction
 *            policy_map_index  - policy-map index
 *            bundle_group_p    - bundle group
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalGetBundleInputRate(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    RULE_TYPE_BundleGroup_T *bundle_group_p)
{
    UI32_T                          child_bundle_index;
    UI32_T                          class_index;
    RULE_TYPE_ChildBundleGroup_T    *child_bundle_p = NULL;
    RULE_TYPE_Class_T               *class_p = NULL;

    ASSERT(NULL != interface_p);
    ASSERT(NULL != policy_map_inst_p);
    ASSERT(NULL != bundle_group_p);

    for (child_bundle_index = 0; child_bundle_index < bundle_group_p->child_bundle_count; ++child_bundle_index)
    {
        child_bundle_p = &bundle_group_p->child_bundle_list[child_bundle_index];

        /*
         * Reset total input rate
         */
        child_bundle_p->total_input_rate_kpbs = 0;

        for (class_index = 0; class_index < child_bundle_p->class_count; ++class_index)
        {
            UI32_T old_rate_kbps;

            class_p = &child_bundle_p->class_list[class_index];

            old_rate_kbps = class_p->flow.input_rate_kbps;

            RULE_CTRL_LocalEachRuleInClassMap(interface_p,
                                              policy_map_inst_p,
                                              class_p->class_map_index,
                                              RULE_CTRL_LocalConvertCountToRate,
                                              &class_p->flow.input_rate_kbps);

            if (class_p->bandwidth_kbps < class_p->flow.input_rate_kbps)
            {
                class_p->flow.input_rate_kbps = class_p->bandwidth_kbps;
            }

            if (old_rate_kbps != class_p->flow.input_rate_kbps)
            {
                char policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
                char class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];

                RULE_OM_GetPolicyMapNameById(policy_map_inst_p->policy_map_id, policy_map_name);
                RULE_OM_GetClassMapNameById(class_p->class_map_index, class_map_name);

                policy_map_name[sizeof(policy_map_name) - 1] = '\0';
                class_map_name[sizeof(class_map_name) - 1] = '\0';

                LOG("policy_map=%s, class_map=%s, input rate kpbs=%lu\r\n",
                    policy_map_name,
                    class_map_name,
                    class_p->flow.input_rate_kbps);
            }

            child_bundle_p->total_input_rate_kpbs += class_p->flow.input_rate_kbps;
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalCalcBundleNewRate
 *------------------------------------------------------------------------------
 * PURPOSE  : Calculatores new bandwidth per flow in a bundle group
 * INPUT    : bundle_group_p    - bundle group
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalCalcBundleNewRate(
    RULE_TYPE_BundleGroup_T *bundle_group_p)
{
    UI32_T                          child_bundle_index;
    UI32_T                          class_index;
    RULE_TYPE_ChildBundleGroup_T    *child_bundle_p = NULL;
    RULE_TYPE_Class_T               *class_p = NULL;
    UI32_T                          bundle_rate;

    ASSERT(NULL != bundle_group_p);

    bundle_rate = bundle_group_p->bundle_rate_kbps;

    for (child_bundle_index = 0; child_bundle_index < bundle_group_p->child_bundle_count; ++child_bundle_index)
    {
        child_bundle_p = &bundle_group_p->child_bundle_list[child_bundle_index];

        for (class_index = 0; class_index < child_bundle_p->class_count; ++class_index)
        {
            class_p = &child_bundle_p->class_list[class_index];
            UI32_T new_rate = class_p->bandwidth_kbps;

            if (child_bundle_p->total_input_rate_kpbs <= bundle_rate ||
                child_bundle_p->total_input_rate_kpbs == 0 ||
                class_p->flow.input_rate_kbps == 0)
            {
                new_rate = class_p->bandwidth_kbps;
            }
            else
            {
                new_rate = (float)(class_p->flow.input_rate_kbps) / child_bundle_p->total_input_rate_kpbs * bundle_rate;
            }

            if (class_p->bandwidth_kbps < new_rate)
            {
                new_rate = class_p->bandwidth_kbps;
            }

            class_p->flow.bandwidth_kbps = new_rate;
        }

        if (bundle_rate < child_bundle_p->total_input_rate_kpbs)
        {
            bundle_rate = 0;
        }
        else
        {
            bundle_rate -= child_bundle_p->total_input_rate_kpbs;
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalUpdateBundleAsNewRate
 *------------------------------------------------------------------------------
 * PURPOSE  : Updates meter CIR to device per flow in bundle group
 * INPUT    : ifindex           - ifindex
 *            inout_profile     - direction
 *            policy_map_index  - policy-map index
 *            bundle_group_p    - bundle group
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalUpdateBundleAsNewRate(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    RULE_TYPE_BundleGroup_T *bundle_group_p)
{
    UI32_T                          child_bundle_index;
    UI32_T                          class_index;
    UI32_T                          bundle_rate;
    RULE_TYPE_ChildBundleGroup_T    *child_bundle_p = NULL;
    RULE_TYPE_Class_T               *class_p = NULL;

    ASSERT(NULL != bundle_group_p);

    bundle_rate = bundle_group_p->bundle_rate_kbps;

    for (child_bundle_index = 0; child_bundle_index < bundle_group_p->child_bundle_count; ++child_bundle_index)
    {
        child_bundle_p = &bundle_group_p->child_bundle_list[child_bundle_index];

        for (class_index = 0; class_index < child_bundle_p->class_count; ++class_index)
        {
            RULE_CTRL_SET_CIR_PARAMETER_T param;

            class_p = &child_bundle_p->class_list[class_index];

            param.new_cir = class_p->flow.bandwidth_kbps;
            param.done = FALSE;

            RULE_CTRL_LocalEachRuleInClassMap(interface_p,
                                              policy_map_inst_p,
                                              class_p->class_map_index,
                                              RULE_CTRL_LocalUpdateMeterCIR,
                                              &param);
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalResetBundleRate
 *------------------------------------------------------------------------------
 * PURPOSE  : Reset meter CIR to device per flow in bundle group
 * INPUT    : ifindex           - ifindex
 *            inout_profile     - direction
 *            policy_map_index  - policy-map index
 *            bundle_group_p    - bundle group
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalResetBundleRate(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    RULE_TYPE_BundleGroup_T *bundle_group_p)
{
    UI32_T                          child_bundle_index;
    UI32_T                          class_index;
    UI32_T                          bundle_rate;
    RULE_TYPE_ChildBundleGroup_T    *child_bundle_p = NULL;
    RULE_TYPE_Class_T               *class_p = NULL;

    ASSERT(NULL != interface_p);
    ASSERT(NULL != policy_map_inst_p);
    ASSERT(NULL != bundle_group_p);

    bundle_rate = bundle_group_p->bundle_rate_kbps;

    for (child_bundle_index = 0; child_bundle_index < bundle_group_p->child_bundle_count; ++child_bundle_index)
    {
        child_bundle_p = &bundle_group_p->child_bundle_list[child_bundle_index];

        for (class_index = 0; class_index < child_bundle_p->class_count; ++class_index)
        {
            RULE_CTRL_SET_CIR_PARAMETER_T param;

            class_p = &child_bundle_p->class_list[class_index];

            param.new_cir = class_p->bandwidth_kbps;
            param.done = FALSE;

            RULE_CTRL_LocalEachRuleInClassMap(interface_p,
                                              policy_map_inst_p,
                                              class_p->class_map_index,
                                              RULE_CTRL_LocalUpdateMeterCIR,
                                              &param);
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalBundleMain
 *------------------------------------------------------------------------------
 * PURPOSE  : Updates meter CIR to device per flow in bundle group
 * INPUT    : ifindex           - ifindex
 *            inout_profile     - direction
 *            policy_map_index  - policy-map index
 *            bundle_group_p    - bundle group
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalBundleMain(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    RULE_TYPE_BundleGroup_T *bundle_group_p)
{
    RULE_CTRL_LocalGetBundleInputRate(interface_p,
                                      policy_map_inst_p,
                                      bundle_group_p);

    RULE_CTRL_LocalCalcBundleNewRate(bundle_group_p);

    RULE_CTRL_LocalUpdateBundleAsNewRate(interface_p,
                                         policy_map_inst_p,
                                         bundle_group_p);

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalBundleRateControlProcessTimerEvent
 *------------------------------------------------------------------------------
 * PURPOSE  : Process timer event for BRC (Bundle Rate Control)
 * INPUT    : ifindex   - ifindex
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_LocalBundleRateControlProcessTimerEvent()
{
    UI32_T if_entry_index;

    for (if_entry_index = 0; if_entry_index < _countof(shmem_data_p->if_table); ++ if_entry_index)
    {
        RULE_TYPE_RETURN_TYPE_T result;

        RULE_TYPE_INTERFACE_INFO_T interface;

        RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

        RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

        RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;

        RULE_CTRL_INSTANCE_TYPE_ID_T param;

        RULE_CTRL_Priv_IfEntry_Index_To_Interface(if_entry_index, &interface);

        if (interface.type == RULE_TYPE_INTERFACE_CPU_PORT)
        {
            continue;
        }

        if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
        ASSERT(NULL != if_entry_p);

        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                  &if_class_obj);

        memset(&param, 0, sizeof(param));

        param.type = RULE_TYPE_INST_POLICY_MAP;

        policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                           RULE_CTRL_Priv_IsClassInstanceMatchType,
                                                                                           &param);
        while (NULL != policy_map_inst_p)
        {
            UI32_T i;
            RULE_TYPE_PolicyMap_T policy_map;

            result = RULE_OM_GetPolicyMap(policy_map_inst_p->id, &policy_map);
            ASSERT(RULE_TYPE_OK == result);
            if (RULE_TYPE_OK != result)
            {
                LOG("<Internal Error> policy-map wasn't exist: policy-map index = %lu", policy_map_inst_p->id);
                continue;
            }

            for (i = 0; i < _countof(policy_map.bundle_group); ++i)
            {
                RULE_TYPE_BundleGroup_T *bundle_group_p = &policy_map.bundle_group[i];
                UI32_T class_map_index;
                UI32_T ci;

                if (TRUE != bundle_group_p->valid_status)
                {
                    continue;
                }

                for (ci = 0; ci < bundle_group_p->class_map_count; ++ci)
                {
                    class_map_index = bundle_group_p->class_map_index_list[ci];

                    RULE_CTRL_LocalEachRuleInClassMap(&interface,
                                                      policy_map_inst_p,
                                                      class_map_index,
                                                      RULE_CTRL_LocalGetInputCount,
                                                      NULL);
                }


                RULE_CTRL_LocalBundleMain(&interface,
                                          policy_map_inst_p,
                                          bundle_group_p);
            }

            policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetNextSiblingIf((RULE_TYPE_INSTANCE_PTR_T)policy_map_inst_p,
                                                                                                RULE_CTRL_Priv_IsClassInstanceMatchType,
                                                                                                &param);
        }
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DeleteBundle
 *------------------------------------------------------------------------------
 * PURPOSE  : Delete bundle group
 * INPUT    : policy_map_index      - policy-map index
 *            bundle_group_index    - bundle group index
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_CTRL_DeleteBundle(
    UI32_T policy_map_index,
    UI32_T bundle_group_index)
{
    UI32_T                      ret;
    UI32_T                      if_entry_index;
    RULE_TYPE_PolicyMap_T       policy_map;
    RULE_TYPE_BundleGroup_T     *bundle_group_p;

    if (bundle_group_index < 1 || _countof(policy_map.bundle_group) < bundle_group_index)
    {
        LOG("Invalid bundle group index %lu", bundle_group_index);
        return RULE_TYPE_INVALID_PARAMETER;
    }

    RULE_CTRL_EnterCriticalSection();

    ret = RULE_OM_GetPolicyMap(policy_map_index, &policy_map);
    if (RULE_TYPE_OK != ret)
    {
        LOG("Invalid policy-map index %lu", policy_map_index);
        RULE_CTRL_LeaveCriticalSectionReturnState(ret);
    }

    bundle_group_p = &policy_map.bundle_group[ bundle_group_index-1 ];

    if (TRUE != bundle_group_p->valid_status)
    {
        LOG("Bundle group index %lu is not exist", bundle_group_index);
        RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_INVALID_PARAMETER);
    }

    for (if_entry_index = 0; if_entry_index < _countof(shmem_data_p->if_table); ++ if_entry_index)
    {
        RULE_TYPE_INTERFACE_INFO_T interface;

        RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

        RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

        RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;

        RULE_CTRL_INSTANCE_TYPE_ID_T param;

        RULE_CTRL_Priv_IfEntry_Index_To_Interface(if_entry_index, &interface);

        if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
        ASSERT(NULL != if_entry_p);

        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                  &if_class_obj);

        memset(&param, 0, sizeof(param));

        param.type = RULE_TYPE_INST_POLICY_MAP;
        param.id = policy_map_index;

        policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                           RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                           &param);
        while (NULL != policy_map_inst_p)
        {
            ret = RULE_CTRL_LocalResetBundleRate(&interface,
                                                 policy_map_inst_p,
                                                 bundle_group_p);
            if (RULE_TYPE_OK != ret)
            {
                RULE_CTRL_LeaveCriticalSectionReturnState(ret);
            }

            policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetNextSiblingIf((RULE_TYPE_INSTANCE_PTR_T)policy_map_inst_p,
                                                                                                RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                                &param);
        }
    }

    RULE_CTRL_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

#pragma mark End of SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL
#pragma mark -

typedef struct
{
    RULE_TYPE_INTERFACE_INFO_PTR_T interface_p;
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p;

    RULE_CTRL_RuleInstanceFn_T fn;
    void *fn_param;
} RULE_CTRL_EACH_RULE_IN_CLASS_MAP_T, *RULE_CTRL_EACH_RULE_IN_CLASS_MAP_PTR_T;

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalEachRuleInClassMap_Callback
 *------------------------------------------------------------------------------
 * PURPOSE  : The adaptor function for RULE_CTRL_LocalEachRuleInClassMap
 * INPUT    : in - instance
 *            cookie - pameters
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_LocalEachRuleInClassMap_Callback(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie)
{
    if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(in->type))
    {
        RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;
        RULE_CTRL_EACH_RULE_IN_CLASS_MAP_PTR_T param_p;

        rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T)in;
        param_p = (RULE_CTRL_EACH_RULE_IN_CLASS_MAP_PTR_T)cookie;

        ASSERT(NULL != param_p);
        ASSERT(NULL != param_p->interface_p);
        ASSERT(NULL != param_p->policy_map_inst_p);
        ASSERT(NULL != param_p->class_map_inst_p);
        ASSERT(NULL != param_p->fn);

        param_p->fn(param_p->interface_p,
                    param_p->policy_map_inst_p,
                    param_p->class_map_inst_p,
                    rule_inst_p,
                    param_p->fn_param);
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalEachRuleInClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : For each rule instance in class-map do fn
 * INPUT    : interface_p - Which interface bound the class-map
 *            policy_map_inst_p - Which policy-map instance
 *            class_map_index - class-map index
 *            fn - callback, do for each rule instance
 *            cookie - callback parameters
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalEachRuleInClassMap(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    UI32_T class_map_index,
    RULE_CTRL_RuleInstanceFn_T fn,
    void *cookie)
{
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;

    ASSERT(NULL != interface_p);
    ASSERT(NULL != policy_map_inst_p);
    ASSERT(NULL != fn);

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_CLASS_MAP;
    param.id = class_map_index;

    class_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) policy_map_inst_p,
                                                                                      RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                      &param);
    ASSERT(NULL != class_map_inst_p);
    while (NULL != class_map_inst_p)
    {
        RULE_CTRL_EACH_RULE_IN_CLASS_MAP_T each_inst_param;

        memset(&each_inst_param, 0, sizeof(each_inst_param));
        each_inst_param.interface_p = interface_p;
        each_inst_param.policy_map_inst_p = policy_map_inst_p;
        each_inst_param.class_map_inst_p = class_map_inst_p;

        each_inst_param.fn = fn;
        each_inst_param.fn_param = cookie;

        RULE_CTRL_IN_Priv_Walk((RULE_TYPE_INSTANCE_PTR_T)class_map_inst_p,
                               RULE_CTRL_LocalEachRuleInClassMap_Callback,
                               &each_inst_param);

        class_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetNextSiblingIf((RULE_TYPE_INSTANCE_PTR_T)class_map_inst_p,
                                                                                            RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                            &param);
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalGetFirstDevRuleIDofOneClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : get the first logical rule id of one class-map
 * INPUT    : ifindex, cmap_idx, ingress_flag
 * OUTPUT   : None
 * RETURN   : RULE_CTRL_NO_SHARE_METER_ENTRY_ID/
 *            Logical rule id of device
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_CTRL_LocalGetFirstDevRuleIDofOneClassMap(
    const RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p,
    UI32_T unit,
    UI32_T device_id)
{
    RULE_CTRL_UNIT_DEVICE_T param;
    RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;

    memset(&param, 0, sizeof(param));
    param.unit = unit;
    param.device_id = device_id;

    rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T)RULE_CTRL_IN_Priv_FindIf((RULE_TYPE_INSTANCE_PTR_T)class_inst_p,
                                                                          RULE_CTRL_Priv_IsRuleInstanceMatchUnitDevice,
                                                                          &param);
    if (NULL != rule_inst_p)
    {
        ASSERT(TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(rule_inst_p->super.type));
        return rule_inst_p->dev_rule_info.rule_id;
    }

    return RULE_CTRL_NO_SHARE_METER_ENTRY_ID;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_HaveMatchAllClassMapBeReferred
 *------------------------------------------------------------------------------
 * PURPOSE  : Check match all class-map have be referred by other interface
 * INPUT    : agg_acl_index - aggreated ACL index
 * OUTPUT   : None
 * RETURN   : TRUE - Have been referred; FALSE - No
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_Priv_HaveMatchAllClassMapBeReferred(
    UI32_T class_map_index)
{
    UI32_T i;

    for (i = 0; i < _countof(shmem_data_p->if_table); ++ i)
    {
        RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

        RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

        RULE_TYPE_INSTANCE_PTR_T in;

        RULE_CTRL_INSTANCE_TYPE_ID_T param;

        if_entry_p = &shmem_data_p->if_table[i];

        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                  &if_class_obj);

        memset(&param, 0, sizeof(param));

        param.type = RULE_TYPE_INST_CLASS_MAP;
        param.id = class_map_index;

        in = RULE_CTRL_IN_Priv_FindIf((RULE_TYPE_INSTANCE_PTR_T)if_class_obj.inst,
                                      RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                      &param);
        if (NULL != in)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_GetAclInstanceById
 *------------------------------------------------------------------------------
 * PURPOSE  : Get ACL instance
 * INPUT    : unit      - unit
 *            port      - port
 *            direction - direction
 * OUTPUT   : None
 * RETURN   : ACL instance; NULL
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_CLASS_INSTANCE_PTR_T
RULE_CTRL_Priv_GetAclInstanceById(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T acl_index)
{
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;

    RULE_TYPE_CLASS_INSTANCE_PTR_T  acl_inst_p;

    ASSERT(NULL != interface_p);

    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_ACL;
    param.id = acl_index;

    acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                &param);
    return acl_inst_p;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_SetPolicyMapInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Set policy map instance
 * INPUT    : unit              - unit
 *            port              - port
 *            direction         - direction
 *            policy_map_inst_p - a pointer to policy map instance
 * OUTPUT   : None
 * RETURN   : policy map instance; NULL
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_CTRL_Priv_SetPolicyMapInstance(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p)
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T ifindex;

    ASSERT(NULL != interface_p);
    ASSERT(interface_p->type < RULE_TYPE_INTERFACE_MAX);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction ||
           RULE_TYPE_OUTBOUND == interface_p->direction);

    ASSERT((NULL == policy_map_inst_p) ||
           (NULL != policy_map_inst_p && NULL == RULE_CTRL_Priv_GetPolicyMapInstance(interface_p)));

    result = RULE_CTRL_Interface_To_Ifindex(interface_p, &ifindex);
    ASSERT(RULE_TYPE_OK == result);

    RULE_OM_SetPolicyMapInstance(ifindex, interface_p->direction, policy_map_inst_p);
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_GetPolicyMapInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Get policy map instance
 * INPUT    : unit      - unit
 *            port      - port
 *            direction - direction
 * OUTPUT   : None
 * RETURN   : policy map instance; NULL
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_CLASS_INSTANCE_PTR_T
RULE_CTRL_Priv_GetPolicyMapInstance(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_CLASS_INSTANCE_PTR_T  policy_map_inst_p;
    UI32_T  ifindex;

    ASSERT(NULL != interface_p);
    ASSERT(interface_p->type < RULE_TYPE_INTERFACE_MAX);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction ||
           RULE_TYPE_OUTBOUND == interface_p->direction);

    result = RULE_CTRL_Interface_To_Ifindex(interface_p, &ifindex);
    ASSERT(RULE_TYPE_OK == result);

    policy_map_inst_p = RULE_OM_GetPolicyMapInstance(ifindex, interface_p->direction);

    ASSERT(NULL == policy_map_inst_p ||
           (NULL != policy_map_inst_p && RULE_TYPE_INST_POLICY_MAP == policy_map_inst_p->super.type));

    return policy_map_inst_p;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_GetAcePriorityInAcl(
    UI32_T acl_index,
    UI32_T ace_index,
    int *priority_p)
{
    UI32_T _ace_index;
    RULE_TYPE_AclType_T ace_type;
    RULE_TYPE_Ace_Entry_T ace_entry;

    UI32_T seq_no;

    ASSERT(NULL != priority_p);

    *priority_p = -9999;

    for (seq_no = 0, _ace_index = 0;
         RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_index, &ace_type, &_ace_index, &ace_entry);
         ++ seq_no)
    {
        if (ace_index == _ace_index)
        {
            *priority_p = (-1) * seq_no;
            return RULE_TYPE_OK;
        }
    }

    ASSERT(0);
    return RULE_TYPE_INVALID_PARAMETER;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_GetClassMapElementPriorityInClassMap(
    UI32_T policy_map_index,
    UI32_T class_map_index,
    RULE_TYPE_ClassType_T class_type,
    UI16_T class_id,
    int *priority_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_PolicyMap_T policy_map;

    UI32_T cx;
    UI32_T px;

    ASSERT(NULL != priority_p);

    *priority_p = -9999;

    result = RULE_OM_GetPolicyMap(policy_map_index, &policy_map);
    ASSERT(RULE_TYPE_OK == result);

    for (px = 0; px < policy_map.element_count; ++ px)
    {
        UI32_T policy_element_index = policy_map.policy_map_element_index_list[px];
        RULE_TYPE_PolicyElement_T policy_element;

        result = RULE_OM_GetPolicyMapElement(policy_element_index, &policy_element);
        ASSERT(RULE_TYPE_OK == result);

        if (policy_element.class_map_index == class_map_index)
        {
            RULE_TYPE_ClassMap_T class_map;

            result = RULE_OM_GetClassMap(class_map_index, &class_map);
            ASSERT(RULE_TYPE_OK == result);

            if (RULE_TYPE_CLASS_MAP_MATCH_ANY == class_map.class_map_match_type)
            {
                for (cx = 0; cx < class_map.class_element_count; ++ cx)
                {
                    if (class_map.class_type[cx] == class_type &&
                        class_map.class_index_list[cx] == class_id)
                    {
                        *priority_p = -1 * ((SYS_ADPT_DIFFSERV_MAX_CLASS_MAP_NBR_OF_POLICY_MAP * px) + cx);
                        return RULE_TYPE_OK;
                    }
                }
            }
            else if (RULE_TYPE_CLASS_MAP_MATCH_ALL == class_map.class_map_match_type)
            {
                *priority_p = -1 * ((SYS_ADPT_DIFFSERV_MAX_CLASS_MAP_NBR_OF_POLICY_MAP * px) + 0);
                return RULE_TYPE_OK;
            }
            else
            {
                ASSERT(0);
            }

            ASSERT(0);
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    ASSERT(0);
    return RULE_TYPE_INVALID_PARAMETER;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalFreeRulesByRuleGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : free rules from chip and selector_info
 * INPUT    : unit, port, class_instance, acl_type, is_qos
 * OUTPUT   : None
 * RETURN   : TRUE -- succeeded / FALS -- failed
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalFreeRulesByRuleGroup(
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p)
{
    RULE_TYPE_RETURN_TYPE_T             result;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T   class_inst_obj;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;
    RULE_TYPE_RULE_INSTANCE_PTR_T       rule_inst_p;

    ASSERT(NULL != class_inst_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_inst_p, &class_inst_obj);
    class_inst_obj.get_element_iterator(&class_inst_obj, &rule_inst_iter);

    for (rule_inst_iter.last(&rule_inst_iter);
         NULL != rule_inst_iter.get_instance(&rule_inst_iter);
         rule_inst_iter.last(&rule_inst_iter))
    {
        rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T) rule_inst_iter.get_instance(&rule_inst_iter);
        ASSERT(NULL != rule_inst_p);

        if (RULE_CTRL_IS_CLASS_INSTANCE_TYPE(rule_inst_p->super.type))
        {
            RULE_TYPE_CLASS_INSTANCE_PTR_T class_rule_inst_p;
            RULE_TYPE_CLASS_INSTANCE_OBJECT_T class_rule_inst_obj;

            class_rule_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)rule_inst_p;

            result = RULE_CTRL_LocalFreeRulesByRuleGroup(class_rule_inst_p);
            ASSERT(RULE_TYPE_OK == result);
            if (RULE_TYPE_OK != result)
            {
                return result;
            }

            RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_rule_inst_p,
                                                           &class_rule_inst_obj);

            class_inst_obj.remove_class_obj(&class_inst_obj, &class_rule_inst_obj);
            RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&class_rule_inst_obj);
        }
        else if (RULE_CTRL_IS_RULE_INSTANCE_TYPE(rule_inst_p->super.type))
        {
            result = RULE_CTRL_LocalFreeRuleInRuleGroup(class_inst_p, rule_inst_p);
            ASSERT(RULE_TYPE_OK == result);
        }
        else
        {
            ASSERT(0);
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalFreeRuleInRuleGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : free a rule from chip and selector_info
 * INPUT    : unit, device_id, class_instance, dev_rule_id, ace_pos, acl_type, is_qos
 * OUTPUT   : None
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalFreeRuleInRuleGroup(
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p,
    RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p)
{
    RULE_TYPE_RETURN_TYPE_T result;
    BOOL_T b_result;

    ASSERT(NULL != class_inst_p);
    ASSERT(NULL != rule_inst_p);

    if (TRUE == rule_inst_p->active)
    {
        ASSERT(0 != rule_inst_p->dev_rule_info.unit && rule_inst_p->dev_rule_info.unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK);
        ASSERT(rule_inst_p->dev_rule_info.device < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT);
        ASSERT(0 != rule_inst_p->dev_rule_info.rule_id);

        b_result = RULE_CTRL_LocalDestroyRule(rule_inst_p->dev_rule_info.unit,
                                              1,
                                              rule_inst_p->dev_rule_info.device,
                                              rule_inst_p->dev_rule_info.pcl_id,
                                              rule_inst_p->dev_rule_info.rule_id);
        if (TRUE != b_result)
        {
            ASSERT(0);
            return RULE_TYPE_FAIL;
        }

        {
            result = RULE_CTRL_Priv_DestroyIfRuleRererence(rule_inst_p->dev_rule_info.unit,
                                                           rule_inst_p->dev_rule_info.device,
                                                           &rule_inst_p->dev_rule_info.interface,
                                                           rule_inst_p->dev_rule_info.lookup_num,
                                                           rule_inst_p->dev_rule_info.pcl_id);
            if (result != RULE_TYPE_OK)
            {
                ASSERT(0);
                return RULE_TYPE_FAIL;
            }
        }
    }

    /*should free logical rule id.*/
    if (0 != rule_inst_p->dev_rule_info.rule_id)
    {
        ASSERT(0 != rule_inst_p->dev_rule_info.unit && rule_inst_p->dev_rule_info.unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK);
        ASSERT(rule_inst_p->dev_rule_info.device < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT);
        ASSERT(0 != rule_inst_p->dev_rule_info.rule_id);

        result = RULE_CTRL_LocalFreeLogicalRuleID(rule_inst_p->dev_rule_info.unit,
                                                  rule_inst_p->dev_rule_info.device,
                                                  RULE_TYPE_IP_EXT_ACL, // Don't care this value
                                                  rule_inst_p->dev_rule_info.rule_id);
        if (RULE_TYPE_OK != result)
        {
            ASSERT(0);
            return result;
        }
    }

    {
        RULE_TYPE_CLASS_INSTANCE_OBJECT_T   class_inst_obj;
        RULE_TYPE_RULE_INSTANCE_OBJECT_T    rule_inst_obj;

        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_inst_p, &class_inst_obj);
        RULE_CTRL_IN_Priv_ConstructRuleInstanceObject(rule_inst_p, &rule_inst_obj);

        class_inst_obj.remove_rule_obj(&class_inst_obj, &rule_inst_obj);
        RULE_CTRL_IN_Priv_DestroyRuleInstanceObject(&rule_inst_obj);
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalIsNullMac
 *------------------------------------------------------------------------------
 * PURPOSE  : check whether mac is all zero
 * INPUT    : mac
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_LocalIsNullMac(
    const UI8_T *mac)
{
    UI8_T   null_mac[SYS_ADPT_MAC_ADDR_LEN];

    memset(null_mac, 0, SYS_ADPT_MAC_ADDR_LEN);

    return (0 == memcmp(mac, null_mac, SYS_ADPT_MAC_ADDR_LEN));
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalConvertAction2DEV
 *------------------------------------------------------------------------------
 * PURPOSE  : Convert action to action list
 * INPUT    : action_entry
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTE     : caller SHOULD free returned DEVRM_ActionEntry_T* memory
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalConvertAction2DEV(
    const RULE_TYPE_Action_T *action_entry,
    DEVRM_ACTION_T *dev_action_p)
{
    ASSERT(NULL != dev_action_p);

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_PRI)
    {
        LOG("Pkt-New-Priority\r\n"
            "    Data %lu", action_entry->pkt_new_pri);

        if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_FORWARD))
        {
            LOG("<Error> Set packet command");
            return RULE_TYPE_FAIL;
        }

        if (TRUE != DEVRM_LIB_ActionModifyPriority(dev_action_p, action_entry->pkt_new_pri))
        {
            LOG("<Error> Modify priority");
            return RULE_TYPE_FAIL;
        }
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_TOS)
    {
#if 0
        UI8_T   cos;
#endif

        LOG("Pkt-New-TOS\r\n"
            "    Data %lu", action_entry->pkt_new_tos);

        if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_FORWARD))
        {
            LOG("<Error> Set packet command");
            return RULE_TYPE_FAIL;
        }

        if (TRUE != DEVRM_LIB_ActionModifyPriority(dev_action_p, action_entry->pkt_new_pri))
        {
            LOG("<Error> Modify priority");
            return RULE_TYPE_FAIL;
        }

#if 0
        // ???: Use new action bitmap
        /* get ip precedence to cos value then set new-priority action */
        RULE_CTRL_CREATE_NEXT_ACTION_IF_NEED(current_dev_action_p);
        if (FALSE == COS_OM_GetIpPrecedence2CosValueByLport(lport, action_entry->pkt_new_tos, &cos))
        {
            RULE_CTRL_LocalFreeDevActionListMemory(dev_action_head_p);
            return NULL;
        }
        current_dev_action_p->action = appGlue_field_action_pkt_new_pri;
        current_dev_action_p->param0 = cos;
#endif
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_DSCP)
    {
        LOG("Pkt-New-DSCP\r\n"
            "    Data %lu", action_entry->pkt_new_dscp);

        if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_FORWARD))
        {
            LOG("<Error> Set packet command");
            return RULE_TYPE_FAIL;
        }

        if (TRUE != DEVRM_LIB_ActionModifyIpDscp(dev_action_p, action_entry->pkt_new_dscp))
        {
            LOG("<Error> Modify IP DSCP");
            return RULE_TYPE_FAIL;
        }

#if 0
        /* get ip dscp to cos value then set new-priority action */
        RULE_CTRL_CREATE_NEXT_ACTION_IF_NEED(current_dev_action_p);
        if (FALSE == COS_OM_GetIpDscp2CosValueByLport(lport, action_entry->pkt_new_dscp, &cos))
        {
            RULE_CTRL_LocalFreeDevActionListMemory(dev_action_head_p);
            return NULL;
        }
        current_dev_action_p->action = appGlue_field_action_pkt_new_pri;
        current_dev_action_p->param0 = cos;
#endif
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_DROP)
    {
        LOG("Drop");

        if (TRUE != DEVRM_LIB_ActionPacketTo(dev_action_p, DEVRM_PKT_CMD_HARD_DROP))
        {
            LOG("<Error> Set packet command");
            return RULE_TYPE_FAIL;
        }
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_DROP_PRECEDENCE)
    {
        LOG("Drop-Precedence\r\n"
            "    Data %lu", action_entry->drop_precedence);

        // Junying: ???
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_RED_DROP)
    {
        LOG("Red-Drop");

        if (TRUE != DEVRM_LIB_ActionRedPacketTo(dev_action_p, DEVRM_PKT_CMD_HARD_DROP))
        {
            LOG("<Error> Set red packet command");
            return RULE_TYPE_FAIL;
        }
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_RED_DROP_PRECEDENCE)
    {
        LOG("Red-Precedence\r\n"
            "    Data %lu", action_entry->red_drop_precedence);

        // Junying: ??
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_RED_PKT_NEW_DSCP)
    {
        LOG("Red-New-DSCP\r\n"
            "    Data %lu", action_entry->red_pkt_new_dscp);

        if (TRUE != DEVRM_LIB_ActionRedPacketTo(dev_action_p, DEVRM_PKT_CMD_FORWARD))
        {
            LOG("<Error> Set red packet command");
            return RULE_TYPE_FAIL;
        }

        if (TRUE != DEVRM_LIB_ActionRedPacketModifyIpDscp(dev_action_p, action_entry->red_pkt_new_dscp))
        {
            LOG("<Error> Modify red packet IP DSCP");
            return RULE_TYPE_FAIL;
        }
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_YELLOW_DROP)
    {
        LOG("Yellow-Drop");

        if (TRUE != DEVRM_LIB_ActionYellowPacketTo(dev_action_p, DEVRM_PKT_CMD_HARD_DROP))
        {
            LOG("<Error> Set yellow packet command");
            return RULE_TYPE_FAIL;
        }
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_YELLOW_DROP_PRECEDENCE)
    {
        LOG("Yellow-Drop-Precedence\r\n"
            "    Data %lu", action_entry->yellow_drop_precedence);

        // Junying: ???
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP)
    {
        LOG("Yellow-New-DSCP\r\n"
            "    Data %lu", action_entry->yellow_pkt_new_dscp);

        if (TRUE != DEVRM_LIB_ActionYellowPacketTo(dev_action_p, DEVRM_PKT_CMD_FORWARD))
        {
            LOG("<Error> Set yellow packet command");
            return RULE_TYPE_FAIL;
        }

        if (TRUE != DEVRM_LIB_ActionYellowPacketModifyIpDscp(dev_action_p, action_entry->yellow_pkt_new_dscp))
        {
            LOG("<Error> Modify yellow packet IP DSCP");
            return RULE_TYPE_FAIL;
        }
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_PHB)
    {
        LOG("Pkt-New-PHB\r\n",
            "  Data %lu", action_entry->pkt_new_phb);

        if (TRUE != DEVRM_LIB_ActionModifyInnerPriority(dev_action_p, action_entry->pkt_new_phb))
        {
            LOG("<Error> Modify inner priority");
            return RULE_TYPE_FAIL;
        }
    }

    return RULE_TYPE_OK;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_CheckRuleQuota(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    RULE_CONFIG_MVL_PACKET_TYPE_T remapped_pkt_type,
    UI32_T nbr_of_rule)
{
    enum
    {
        RULE_SIZE = 2,
    };

    typedef struct
    {
        RULE_TYPE_INTERFACE_TYPE_E if_type;
        UI32_T min;
        UI32_T max;
    } QUOTA_T, *QUOTA_PTR_T;

    QUOTA_T quota[] =
    {
#undef QUOTA
#define QUOTA(x, y) x, y
#define PACKET_TYPE(pktType, ifType, remapAceType, quota) {ifType, quota},

        RULE_CONFIG_PACKET_TYPE_TABLE

#undef PACKET_TYPE
    };

    QUOTA_PTR_T my_quota_p;
    UI32_T my_used_rule_cnt;

    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    ASSERT(remapped_pkt_type < _countof(quota));

    my_quota_p = &quota[remapped_pkt_type];

    if (0 == nbr_of_rule)
    {
        return RULE_TYPE_OK;
    }

    if (0 == my_quota_p->max)
    {
        return RULE_TYPE_INSUFFICIENT_RESOURCE;
    }

    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    my_used_rule_cnt = RULE_CTRL_Priv_CountOfAceInstance(if_class_obj.inst,
                                                         remapped_pkt_type);

    ASSERT(my_used_rule_cnt <= my_quota_p->max);

    /** Reached the quota
     */
    if (my_used_rule_cnt == my_quota_p->max)
    {
        return RULE_TYPE_INSUFFICIENT_RESOURCE;
    }

    /* Internal error
     */
    if (my_quota_p->max < my_used_rule_cnt)
    {
        return RULE_TYPE_INSUFFICIENT_RESOURCE;
    }

    /** Under number of guarantee
     */
    if (my_used_rule_cnt < my_quota_p->min)
    {
        return RULE_TYPE_OK;
    }

    /** Consider all reserved rule
     */
    {
        /** Only check rule resource. It means it will still fail to setup rule
         * due to other resource issue (e.g, meter, counter, ... etc)
         */
        UI32_T  dup_stack_free_nbr_pce[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK]
                                      [SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT];

        ASSERT(sizeof(dup_stack_free_nbr_pce) == sizeof(shmem_data_p->stack_free_nbr_pce));
        memcpy(dup_stack_free_nbr_pce, shmem_data_p->stack_free_nbr_pce,
                                                sizeof(dup_stack_free_nbr_pce));

        RULE_CONFIG_MVL_PACKET_TYPE_T cur_remapped_pkt_type;

        for (cur_remapped_pkt_type = 0;
             cur_remapped_pkt_type < _countof(quota);
             ++cur_remapped_pkt_type)
        {
            QUOTA_PTR_T cur_quota_p = &quota[cur_remapped_pkt_type];
            RULE_TYPE_INTERFACE_INFO_T cur_interface;
            UI32_T used_rule_cnt;

            if (0xffffffff == cur_quota_p->if_type)
            {
                continue;
            }

            if (cur_remapped_pkt_type == remapped_pkt_type)
            {
                continue;
            }

            /** Get total used number of rule
             * #available rule = quota_p->min - #used
             */
            if (0 == cur_quota_p->min)
            {
                continue;
            }

            memset(&cur_interface, 0, sizeof(cur_interface));

            {
                DEVRM_PCL_LOOKUP_NUMBER_T lookup_num;

                RULE_CTRL_Priv_ConvertPacketTypeToToDirectionLookupNum(cur_remapped_pkt_type,
                                                                       &cur_interface.direction,
                                                                       &lookup_num);
            }

            if (RULE_TYPE_INTERFACE_ALL_UNIT == cur_quota_p->if_type ||
                RULE_TYPE_INTERFACE_UNIT == cur_quota_p->if_type ||
                RULE_TYPE_INTERFACE_CRAFT_PORT == cur_quota_p->if_type ||
                RULE_TYPE_INTERFACE_CPU_PORT == cur_quota_p->if_type ||
                RULE_TYPE_INTERFACE_TRUNK == cur_quota_p->if_type)
            {
                    cur_interface.type = cur_quota_p->if_type;
            }
            else
            {
                //
                // FIXME: Add code here for reserving rule for other interface
                //        type
                //
                ASSERT(0);
                continue;
            }

            if_entry_p = RULE_CTRL_Priv_GetIfEntry(&cur_interface);
            ASSERT(NULL != if_entry_p);
            if (NULL == if_entry_p)
            {
                continue;
            }

            RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                      &if_class_obj);

            used_rule_cnt = RULE_CTRL_Priv_CountOfAceInstance(if_class_obj.inst,
                                                              cur_remapped_pkt_type);

            if (used_rule_cnt < cur_quota_p->min)
            {
                UI32_T unit;
                UI32_T device_id;
                UI32_T reserve_pce_cnt = (cur_quota_p->min - used_rule_cnt) * RULE_SIZE;

                for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++ unit)
                {
                    for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; ++ device_id)
                    {
                        if (reserve_pce_cnt <= dup_stack_free_nbr_pce[unit-1][device_id])
                        {
                            dup_stack_free_nbr_pce[unit-1][device_id] -= reserve_pce_cnt;
                        }
                        else
                        {
                            dup_stack_free_nbr_pce[unit-1][device_id] = 0;
                        }
                    }
                }
            }
        }

        /** Finial, check quota
         */
        if (RULE_TYPE_INTERFACE_ALL_UNIT == interface_p->type ||
            RULE_TYPE_INTERFACE_UNIT == interface_p->type ||
            RULE_TYPE_INTERFACE_CPU_PORT == interface_p->type ||
            RULE_TYPE_INTERFACE_CRAFT_PORT == interface_p->type ||
            RULE_TYPE_INTERFACE_TRUNK == interface_p->type)
        {
            UI32_T unit;
            UI32_T device_id;
            UI32_T req_pce_cnt = nbr_of_rule * RULE_SIZE;

            for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++ unit)
            {
                for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; ++ device_id)
                {
                    if (dup_stack_free_nbr_pce[unit-1][device_id] < req_pce_cnt)
                    {
                        return RULE_TYPE_INSUFFICIENT_RESOURCE;
                    }
                }
            }
        }
        else if (RULE_TYPE_INTERFACE_UPORT == interface_p->type)
        {
            UI32_T req_pce_cnt = nbr_of_rule * RULE_SIZE;
            UI32_T module_id, device_id, phy_port;
            BOOL_T b_result;

            b_result = DEV_SWDRV_PMGR_Logical2PhyDevicePortID(interface_p->uport.unit,
                                                              interface_p->uport.port,
                                                              &module_id,
                                                              &device_id,
                                                              &phy_port);
            ASSERT(TRUE == b_result);

            if (dup_stack_free_nbr_pce[interface_p->uport.unit-1][device_id] < req_pce_cnt)
            {
                return RULE_TYPE_INSUFFICIENT_RESOURCE;
            }
        }
        else
        {
            ASSERT(0);
            return RULE_TYPE_FAIL;
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_IfEntry_Index_To_Interface
 *------------------------------------------------------------------------------
 * PURPOSE  : Converts index of ifEntry to interface object
 * INPUT    : idx - index of ifEntry
 * OUTPUT   : interface_p - interface object
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_IfEntry_Index_To_Interface(
    UI32_T idx,
    RULE_TYPE_INTERFACE_INFO_PTR_T interface_p)
{
    ASSERT(idx < _countof(shmem_data_p->if_table));
    ASSERT(NULL != interface_p);

    if (_countof(shmem_data_p->if_table) <= idx)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    memset(interface_p, 0, sizeof(*interface_p));

    if (idx < RULE_CTRL_TOTAL_NBR_OF_IF_ENTRY_PER_DIRECTION)
    {
        interface_p->direction = RULE_TYPE_INBOUND;
    }
    else
    {
        interface_p->direction = RULE_TYPE_OUTBOUND;
    }

    if (RULE_CTRL_TOTAL_NBR_OF_IF_ENTRY_PER_DIRECTION <= idx)
    {
        idx -= RULE_CTRL_TOTAL_NBR_OF_IF_ENTRY_PER_DIRECTION;
    }

    if (RULE_CTRL_UNIT_1_INDEX == idx)
    {
        interface_p->type = RULE_TYPE_INTERFACE_ALL_UNIT;
    }
    else if (RULE_CTRL_UPORT_1_INDEX <= idx && idx < (RULE_CTRL_UPORT_1_INDEX +
                                                      RULE_CTRL_TOTAL_NBR_OF_UPORT))
    {
        interface_p->type = RULE_TYPE_INTERFACE_UPORT;
        interface_p->uport.unit = STKTPLG_OM_IFINDEX_TO_UNIT((idx - RULE_CTRL_UPORT_1_INDEX + 1));
        interface_p->uport.port = STKTPLG_OM_IFINDEX_TO_PORT((idx - RULE_CTRL_UPORT_1_INDEX + 1));
    }
    else if (RULE_CTRL_TRUNK_PORT_1_INDEX <= idx && idx < (RULE_CTRL_TRUNK_PORT_1_INDEX +
                                                           RULE_CTRL_TOTAL_NBR_OF_TRUNK_PORT))
    {
        interface_p->type = RULE_TYPE_INTERFACE_TRUNK;
        interface_p->trunk_id = idx - RULE_CTRL_TRUNK_PORT_1_INDEX + 1;
    }
    else if (RULE_CTRL_CRAFT_PORT_1_INDEX <= idx && idx < (RULE_CTRL_CRAFT_PORT_1_INDEX +
                                                           RULE_CTRL_TOTAL_NBR_OF_CRAFT_PORT))
    {
        interface_p->type = RULE_TYPE_INTERFACE_CRAFT_PORT;
    }
    else if (RULE_CTRL_CPU_PORT_1_INDEX <= idx && idx < (RULE_CTRL_CPU_PORT_1_INDEX +
                                                         RULE_CTRL_TOTAL_NBR_OF_CPU_PORT))
    {
        interface_p->type = RULE_TYPE_INTERFACE_CPU_PORT;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_GetIfEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : Get an ifEntry by interface
 * INPUT    : interface_p - interface
 * OUTPUT   : None
 * RETURN   : Pointer of ifEntry
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_CTRL_IF_ENTRY_PTR_T
RULE_CTRL_Priv_GetIfEntry(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p)
{
    UI32_T idx = 0xffffffff;

    ASSERT(NULL != interface_p);
    ASSERT(RULE_TYPE_INBOUND == interface_p->direction ||
           RULE_TYPE_OUTBOUND == interface_p->direction);

    switch (interface_p->type)
    {
        case RULE_TYPE_INTERFACE_ALL_UNIT:
        case RULE_TYPE_INTERFACE_UNIT:
            idx = RULE_CTRL_UNIT_1_INDEX;
            break;

        case RULE_TYPE_INTERFACE_UPORT:
            idx = STKTPLG_OM_UPORT_TO_IFINDEX(interface_p->uport.unit,
                                              interface_p->uport.port);
            idx -= SYS_ADPT_ETHER_1_IF_INDEX_NUMBER;
            idx += RULE_CTRL_UPORT_1_INDEX;
            break;

        case RULE_TYPE_INTERFACE_TRUNK:
            idx = STKTPLG_OM_TRUNKID_TO_IFINDEX(interface_p->trunk_id);
            idx -= SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER;
            idx += RULE_CTRL_TRUNK_PORT_1_INDEX;
            break;

        case RULE_TYPE_INTERFACE_CRAFT_PORT:
            idx = RULE_CTRL_CRAFT_PORT_1_INDEX;
            break;

        case RULE_TYPE_INTERFACE_CPU_PORT:
            idx = RULE_CTRL_CPU_PORT_1_INDEX;
            break;

        default:
            ASSERT(0);
            return NULL;
    }

    idx += (RULE_TYPE_OUTBOUND == interface_p->direction) ?
                              RULE_CTRL_TOTAL_NBR_OF_IF_ENTRY_PER_DIRECTION : 0;

    ASSERT(idx < _countof(shmem_data_p->if_table));

    return &shmem_data_p->if_table[idx];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_GetClassInstanceOfRuleEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : Get class instance of rule.
 * INPUT    : class_inst_obj_p - parent's node
 *            rule_pattern_p - key
 * OUTPUT   : None
 * RETURN   : Pointer of class instance
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_CLASS_INSTANCE_PTR_T
RULE_CTRL_Priv_GetClassInstanceOfRuleEntry(
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *class_inst_obj_p,
    const DEVRM_AceEntry_T *rule_pattern_p)
{
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;

    ASSERT(NULL != rule_pattern_p);

    class_inst_obj_p->get_element_iterator(class_inst_obj_p, &rule_inst_iter);

    for (rule_inst_iter.first(&rule_inst_iter);
         NULL != rule_inst_iter.get_instance(&rule_inst_iter);
         rule_inst_iter.next(&rule_inst_iter))
    {
        RULE_TYPE_INSTANCE_PTR_T rule_inst_p;

        rule_inst_p = rule_inst_iter.get_instance(&rule_inst_iter);

        if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(rule_inst_p->type))
        {
            RULE_TYPE_CLASS_INSTANCE_PTR_T rule_class_inst_p;
            RULE_TYPE_CLASS_INSTANCE_OBJECT_T rule_class_inst_obj;
            RULE_TYPE_CLASS_INSTANCE_PTR_T target;

            rule_class_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T) rule_inst_p;
            RULE_CTRL_IN_Priv_ConstructClassInstanceObject(rule_class_inst_p,
                                                           &rule_class_inst_obj);

            target = RULE_CTRL_Priv_GetClassInstanceOfRuleEntry(&rule_class_inst_obj,
                                                                rule_pattern_p);
            if (NULL != target)
            {
                return target;
            }
        }
        else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(rule_inst_p->type))
        {
            UI32_T mvl_rule_index;
            RULE_CTRL_RULE_ENTRY_PTR_T rule_entry_p;

            int i_result;

            mvl_rule_index = ((RULE_TYPE_RULE_INSTANCE_PTR_T) rule_inst_p)->mvl_rule_index;

            rule_entry_p = &shmem_data_p->rule_table[mvl_rule_index];

            i_result = memcmp(&rule_entry_p->rule_pattern, rule_pattern_p, sizeof(*rule_pattern_p));

            if (0 == i_result)
            {
                return class_inst_obj_p->inst;
            }
        }
        else
        {
            ASSERT(0);
        }
    }

    return NULL;
}

RULE_CTRL_RULE_ENTRY_PTR_T
RULE_CTRL_Priv_GetRuleEntryOfRule(
    const RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p)
{
    ASSERT(NULL != rule_inst_p);

    ASSERT(rule_inst_p->mvl_rule_index < _countof(shmem_data_p->rule_table));

    if (_countof(shmem_data_p->rule_table) <= rule_inst_p->mvl_rule_index)
    {
        return NULL;
    }

    return &shmem_data_p->rule_table[rule_inst_p->mvl_rule_index];
}

BOOL_T
RULE_CTRL_Priv_CreateIfRuleRererence(
    UI32_T unit,
    UI32_T device_id,
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T lookup_num,
    UI32_T pcl_id)
{
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;
    UI32_T local_unit;
    BOOL_T b_result = TRUE;
    RULE_TYPE_INTERFACE_INFO_T g_interface;
    RULE_CTRL_IF_ENTRY_PTR_T   g_if_entry_p = NULL;
    BOOL_T force_rebind = FALSE;

    STKTPLG_OM_GetMyUnitID(&local_unit);

    switch (interface_p->type)
    {
        case RULE_TYPE_INTERFACE_UPORT:
            force_rebind = FALSE;
            break;

        case RULE_TYPE_INTERFACE_TRUNK:
        case RULE_TYPE_INTERFACE_ALL_UNIT:
        case RULE_TYPE_INTERFACE_CPU_PORT:
            force_rebind = TRUE;
            if (local_unit != unit ||
                device_id != 0)
            {
                return TRUE;
            }
            break;

        case RULE_TYPE_INTERFACE_CRAFT_PORT:
            force_rebind = TRUE;
            if (local_unit != unit)
            {
                return TRUE;
            }
            break;

        default:
            ASSERT(0);
    }

    if (interface_p->type == RULE_TYPE_INTERFACE_UPORT ||
        interface_p->type == RULE_TYPE_INTERFACE_TRUNK)
    {
        memset(&g_interface, 0, sizeof(g_interface));
        g_interface.direction = interface_p->direction;
        g_interface.type = RULE_TYPE_INTERFACE_ALL_UNIT;

        g_if_entry_p = RULE_CTRL_Priv_GetIfEntry(&g_interface);
        ASSERT(g_if_entry_p != NULL);
    }

    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);

    if (force_rebind == TRUE ||
        if_entry_p->lookup_rule_cnt[lookup_num] == 0)
    {
        RULE_TYPE_RETURN_TYPE_T result;

        result = RULE_CTRL_BindPclToPort(TRUE, interface_p, pcl_id);
        if (result != RULE_TYPE_OK)
        {
            b_result = FALSE;
        }
    }

    if_entry_p->lookup_rule_cnt[lookup_num]++;

    if (g_if_entry_p != NULL)
    {
        g_if_entry_p->lookup_rule_cnt[lookup_num]++;
    }

    return b_result;
}

BOOL_T
RULE_CTRL_Priv_DestroyIfRuleRererence(
    UI32_T unit,
    UI32_T device_id,
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T lookup_num,
    UI32_T pcl_id)
{
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;
    UI32_T local_unit;

    STKTPLG_OM_GetMyUnitID(&local_unit);

    switch (interface_p->type)
    {
        case RULE_TYPE_INTERFACE_UPORT:
            break;

        case RULE_TYPE_INTERFACE_TRUNK:
        case RULE_TYPE_INTERFACE_ALL_UNIT:
        case RULE_TYPE_INTERFACE_CPU_PORT:
            if (local_unit != unit
                || device_id != 0)
            {
                return TRUE;
            }
            break;

        case RULE_TYPE_INTERFACE_CRAFT_PORT:
            if (local_unit != unit)
            {
                return TRUE;
            }
            break;

        default:
            ASSERT(0);
    }

    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);

    ASSERT(if_entry_p->lookup_rule_cnt[lookup_num] != 0);
    if (if_entry_p->lookup_rule_cnt[lookup_num] != 0)
    {
        RULE_TYPE_RETURN_TYPE_T result;

        if_entry_p->lookup_rule_cnt[lookup_num]--;

        if (interface_p->type == RULE_TYPE_INTERFACE_UPORT ||
            interface_p->type == RULE_TYPE_INTERFACE_TRUNK)
        {
            RULE_TYPE_INTERFACE_INFO_T g_interface;
            RULE_CTRL_IF_ENTRY_PTR_T   g_if_entry_p = NULL;

            memset(&g_interface, 0, sizeof(g_interface));
            g_interface.direction = interface_p->direction;
            g_interface.type = RULE_TYPE_INTERFACE_ALL_UNIT;

            g_if_entry_p = RULE_CTRL_Priv_GetIfEntry(&g_interface);
            ASSERT(g_if_entry_p != NULL);
            ASSERT(0 < g_if_entry_p->lookup_rule_cnt[lookup_num]);

            g_if_entry_p->lookup_rule_cnt[lookup_num]--;

            if (g_if_entry_p->lookup_rule_cnt[lookup_num] == 0)
            {
                result = RULE_CTRL_BindPclToPort(FALSE, interface_p, pcl_id);
                if (result != RULE_TYPE_OK)
                {
                    return FALSE;
                }
            }
        }
        else if (if_entry_p->lookup_rule_cnt[lookup_num] == 0)
        {
            result = RULE_CTRL_BindPclToPort(FALSE, interface_p, pcl_id);
            if (result != RULE_TYPE_OK)
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

BOOL_T
RULE_CTRL_Priv_IsRuleInstanceMatchUnitDevice(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie)
{
    if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(in->type))
    {
        RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;
        RULE_CTRL_UNIT_DEVICE_PTR_T param_p;

        rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T)in;
        param_p = (RULE_CTRL_UNIT_DEVICE_PTR_T)cookie;

        if (rule_inst_p->dev_rule_info.unit == param_p->unit &&
            rule_inst_p->dev_rule_info.device == param_p->device_id)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL_T
RULE_CTRL_Priv_IsClassInstanceMatchId(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie)
{
    if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(in->type))
    {
        RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p;
        UI32_T *id_p = cookie;

        class_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)in;

        if (class_inst_p->id == *id_p)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL_T
RULE_CTRL_Priv_IsClassInstanceMatchTypeId(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie)
{
    if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(in->type))
    {
        RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p;
        RULE_CTRL_INSTANCE_TYPE_ID_PTR_T param_p;

        param_p = (RULE_CTRL_INSTANCE_TYPE_ID_PTR_T)cookie;

        class_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)in;

        if (class_inst_p->super.type == param_p->type &&
            class_inst_p->id == param_p->id)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL_T
RULE_CTRL_Priv_IsClassInstanceMatchType(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie)
{
    if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(in->type))
    {
        RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p;
        RULE_CTRL_INSTANCE_TYPE_ID_PTR_T param_p;

        param_p = (RULE_CTRL_INSTANCE_TYPE_ID_PTR_T)cookie;

        class_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)in;

        if (class_inst_p->super.type == param_p->type)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL_T
RULE_CTRL_Priv_IsMatchTypeId(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie)
{
    UI32_T id;

    RULE_CTRL_INSTANCE_TYPE_ID_PTR_T param_p;

    param_p = (RULE_CTRL_INSTANCE_TYPE_ID_PTR_T)cookie;

    if (TRUE == RULE_CTRL_IS_CONTAINER_TYPE(in->type))
    {
        id = ((RULE_TYPE_CONTAINER_PTR_T) in)->id;
    }
    else if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(in->type))
    {
        id = ((RULE_TYPE_CLASS_INSTANCE_PTR_T) in)->id;
    }
    else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(in->type))
    {
        id = ((RULE_TYPE_RULE_INSTANCE_PTR_T) in)->id;
    }
    else
    {
        id = 0xffffffff;
        ASSERT(0);
    }

    return id == param_p->id ? TRUE : FALSE;
}

BOOL_T
RULE_CTRL_Priv_IsAceInstanceWithId(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie)
{
    if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(in->type))
    {
        RULE_TYPE_CLASS_INSTANCE_PTR_T ace_inst_p;
        UI32_T *id_p = (UI32_T *)cookie;

        ace_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)in;

        if ((RULE_TYPE_INST_ACE == in->type ||
             RULE_TYPE_INST_MF_ACE == in->type) &&
            ace_inst_p->mvl_pkt_type == *id_p)
        {
            return TRUE;
        }
    }

    return FALSE;
}

static UI32_T
RULE_CTRL_Priv_CountOfAceInstance(
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p,
    UI32_T id)
{
    return RULE_CTRL_IN_Priv_CountIf((RULE_TYPE_INSTANCE_PTR_T)class_inst_p,
                                     RULE_CTRL_Priv_IsAceInstanceWithId, &id);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_GetFirstChildIf
 *------------------------------------------------------------------------------
 * PURPOSE  : Get child instance that first match the compare function
 * INPUT    : in     - Parent instance ptr.
 *            fn     - Compare function(return TRUE mean matched).
 *            cookie - Parameter for compare function.
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_INSTANCE_PTR_T / NULL
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_INSTANCE_PTR_T
RULE_CTRL_Priv_GetFirstChildIf(
    RULE_TYPE_INSTANCE_PTR_T in,
    BOOL_T (*fn)(RULE_TYPE_INSTANCE_PTR_T, void*),
    void *cookie)
{
    ASSERT(NULL != in);
    ASSERT(TRUE == RULE_CTRL_IS_CONTAINER_TYPE(in->type) ||
           TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(in->type));

    if (TRUE == RULE_CTRL_IS_CONTAINER_TYPE(in->type))
    {
        RULE_TYPE_CONTAINER_OBJECT_T obj;
        RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T iter;

        RULE_CTRL_IN_Priv_ConstructContainerObject((RULE_TYPE_CONTAINER_PTR_T) in, &obj);
        obj.get_element_iterator(&obj, &iter);

        for (iter.first(&iter); NULL != iter.get_instance(&iter); iter.next(&iter))
        {
            if (TRUE == fn(iter.get_instance(&iter), cookie))
            {
                return iter.get_instance(&iter);
            }
        }
    }
    else if(TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(in->type))
    {
        RULE_TYPE_CLASS_INSTANCE_OBJECT_T obj;
        RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T iter;

        RULE_CTRL_IN_Priv_ConstructClassInstanceObject((RULE_TYPE_CLASS_INSTANCE_PTR_T) in, &obj);
        obj.get_element_iterator(&obj, &iter);

        for (iter.first(&iter); NULL != iter.get_instance(&iter); iter.next(&iter))
        {
            if (TRUE == fn(iter.get_instance(&iter), cookie))
            {
                return iter.get_instance(&iter);
            }
        }
    }

    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_GetNextSiblingIf
 *------------------------------------------------------------------------------
 * PURPOSE  : Get next sibling instance that match the compare function
 * INPUT    : in     - Parent instance ptr.
 *            fn     - Compare function(return TRUE mean matched).
 *            cookie - Parameter for compare function.
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_INSTANCE_PTR_T / NULL
 * NOTE     : Sibling mean brother/sister
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_INSTANCE_PTR_T
RULE_CTRL_Priv_GetNextSiblingIf(
    RULE_TYPE_INSTANCE_PTR_T in,
    BOOL_T (*fn)(RULE_TYPE_INSTANCE_PTR_T, void*),
    void *cookie)
{
    ASSERT(NULL != in);

    if (TRUE == RULE_CTRL_IS_CONTAINER_TYPE(in->links.parent.type))
    {
        RULE_TYPE_CONTAINER_OBJECT_T parent;
        RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T iter;

        RULE_CTRL_IN_Priv_ConstructContainerObjectFromPointer(in->links.parent, &parent);
        parent.get_element_iterator(&parent, &iter);

        /* Get the old position
         * iter = iter.indexof(in)
         */
        for (iter.first(&iter); NULL != iter.get_instance(&iter); iter.next(&iter))
        {
            if (in == iter.get_instance(&iter))
            {
                iter.next(&iter);
                break;
            }
        }

        for (; NULL != iter.get_instance(&iter); iter.next(&iter))
        {
            if (TRUE == fn(iter.get_instance(&iter), cookie))
            {
                return iter.get_instance(&iter);
            }
        }
    }
    else
    {
        RULE_TYPE_INSTANCE_PTR_T next;

        next = in;

        while (NULL != (next = RULE_CTRL_IN_Ptr2Instance(next->links.next)))
        {
            if (TRUE == fn(next, cookie))
            {
                return next;
            }
        }
    }

    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_RemapPacketType
 *------------------------------------------------------------------------------
 * PURPOSE  : This function is remapped pkt type from Original BT platform to MO
 *            Style
 * INPUT    : original_pkt_type
 * OUTPUT   : remapped_pkt_type
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static void RULE_CTRL_RemapPacketType(RULE_TYPE_PacketType_T original_pkt_type, UI32_T *remapped_pkt_type)
{
    *remapped_pkt_type = RULE_TYPE_PACKET_MAX_TYPE;

    switch (original_pkt_type)
    {
        case RULE_TYPE_PacketType_DHCP_CLIENT:
            *remapped_pkt_type = RULE_TYPE_PACKET_DHCP_CLIENT;
            break;
        case RULE_TYPE_PacketType_DHCP_SERVER:
            *remapped_pkt_type = RULE_TYPE_PACKET_DHCP_SERVER;
            break;
        case RULE_TYPE_PacketType_DHCP6_CLIENT:
            *remapped_pkt_type = RULE_TYPE_PACKET_DHCP6_CLIENT;
            break;
        case RULE_TYPE_PacketType_DHCP6_SERVER:
            *remapped_pkt_type = RULE_TYPE_PACKET_DHCP6_SERVER;
            break;
        case RULE_TYPE_PacketType_ORG_SPECIFIC:
            *remapped_pkt_type = RULE_TYPE_PACKET_ORG_SPECIFIC;
            break;
        case RULE_TYPE_PacketType_ORG_SPECIFIC1:
            *remapped_pkt_type = RULE_TYPE_PACKET_ORG_SPECIFIC1;
            break;
        case RULE_TYPE_PacketType_ORG_SPECIFIC2:
            *remapped_pkt_type = RULE_TYPE_PACKET_ORG_SPECIFIC2;
            break;
        case RULE_TYPE_PacketType_ORG_SPECIFIC3:
            *remapped_pkt_type = RULE_TYPE_PACKET_ORG_SPECIFIC3;
            break;
        case RULE_TYPE_PacketType_IGMP:
            *remapped_pkt_type = RULE_TYPE_PACKET_IGMP;
            break;

#if (SYS_CPNT_CFM == TRUE)
        case RULE_TYPE_PacketType_CFM:
            *remapped_pkt_type = RULE_TYPE_PACKET_CFM;
            break;
#endif /* SYS_CPNT_CFM */

#if (SYS_CPNT_ERPS == TRUE)
        case RULE_TYPE_PacketType_RAPS:
            *remapped_pkt_type = RULE_TYPE_PACKET_RAPS;
            break;
#endif /* SYS_CPNT_ERPS */

#if (SYS_CPNT_ROUTING == TRUE)
        case RULE_TYPE_PacketType_MY_MAC_ARP:
            *remapped_pkt_type = RULE_TYPE_PACKET_MY_MAC_ARP;
            break;
#endif /* SYS_CPNT_ROUTING */

#if (SYS_CPNT_CRAFT_PORT == TRUE) && (SYS_CPNT_CRAFT_PORT_MODE == SYS_CPNT_CRAFT_PORT_MODE_FRONT_PORT_CRAFT_PORT)
        case RULE_TYPE_Packettype_MY_MAC_CRAFT_PORT:
            *remapped_pkt_type = RULE_TYPE_PACKET_MY_MAC_CRAFT_PORT;
            break;

#endif /* SYS_CPNT_CRAFT_PORT */

#if (SYS_CPNT_RIP == TRUE)
        case RULE_TYPE_PacketType_RIP:
            *remapped_pkt_type = RULE_TYPE_PACKET_RIP;
            break;
#endif /* SYS_CPNT_RIP */

#if (SYS_CPNT_PTP == TRUE)
        case RULE_TYPE_PacketType_PTP_EVENT_ETH:
            *remapped_pkt_type = RULE_TYPE_PACKET_PTP_EVENT_ETH;
            break;

        case RULE_TYPE_PacketType_PTP_EVENT_UDP:
            *remapped_pkt_type = RULE_TYPE_PACKET_PTP_EVENT_UDP;
            break;

        case RULE_TYPE_PacketType_PTP_GENERAL_UDP:
            *remapped_pkt_type = RULE_TYPE_PACKET_PTP_GENERAL_UDP;
            break;
#endif /* SYS_CPNT_PTP */

#if (SYS_CPNT_QINQ_L2PT_CDP == TRUE) || (SYS_CPNT_QINQ_L2PT_VTP == TRUE)
        case RULE_TYPE_PacketType_CDP:
            *remapped_pkt_type = RULE_TYPE_PACKET_CDP;
            break;
#endif /* SYS_CPNT_QINQ_L2PT_CDP || SYS_CPNT_QINQ_L2PT_VTP */

#if (SYS_CPNT_QINQ_L2PT_PVST == TRUE)
        case RULE_TYPE_PacketType_PVST:
            *remapped_pkt_type = RULE_TYPE_PACKET_PVST;
            break;
#endif /* SYS_CPNT_QINQ_L2PT_PVST */

#if (SYS_CPNT_DOS_ECHO_CHARGEN == TRUE)
        case RULE_TYPE_PacketType_DOS_CHARGEN:
            *remapped_pkt_type = RULE_TYPE_PACKET_DOS_CHARGEN;
            break;

        case RULE_TYPE_PacketType_DOS_ECHO:
            *remapped_pkt_type = RULE_TYPE_PACKET_DOS_ECHO;
            break;
#endif /* SYS_CPNT_DOS_ECHO_CHARGEN */

#if (SYS_CPNT_DOS_TCP_FLOODING == TRUE)
        case RULE_TYPE_PacketType_DOS_TCP_FLOODING:
            *remapped_pkt_type = RULE_TYPE_PACKET_DOS_TCP_FLOODING;
            break;
#endif /* SYS_CPNT_DOS_TCP_FLOODING */

#if (SYS_CPNT_DOS_UDP_FLOODING == TRUE)
        case RULE_TYPE_PacketType_DOS_UDP_FLOODING:
            *remapped_pkt_type = RULE_TYPE_PACKET_DOS_UDP_FLOODING;
            break;
#endif /* SYS_CPNT_DOS_UDP_FLOODING */

#if (SYS_CPNT_DOS_WIN_NUKE == TRUE)
        case RULE_TYPE_PacketType_DOS_WIN_NUKE:
            *remapped_pkt_type = RULE_TYPE_PACKET_DOS_WIN_NUKE;
            break;
#endif /* SYS_CPNT_DOS_WIN_NUKE */

#if (SYS_CPNT_DOS_SMURF == TRUE)
        case RULE_TYPE_PacketType_DOS_SMURF:
            *remapped_pkt_type = RULE_TYPE_PACKET_DOS_SMURF;
            break;
#endif /* SYS_CPNT_DOS_SMURF */

#if (SYS_CPNT_VRRP == TRUE)
        case RULE_TYPE_PacketType_VRRP:
            *remapped_pkt_type = RULE_TYPE_PACKET_VRRP;
            break;
#endif /* SYS_CPNT_VRRP */

#if (SYS_CPNT_UDP_HELPER == TRUE)
        case RULE_TYPE_PacketType_IP_BCAST:
            *remapped_pkt_type = RULE_TYPE_PACKET_IP_BCAST;
        break;
#endif /* SYS_CPNT_UDP_HELPER */

#if (SYS_CPNT_DAI == TRUE)
        case RULE_TYPE_PacketType_ARP_REQUEST:
        case RULE_TYPE_PacketType_ARP_REPLY:
            *remapped_pkt_type = RULE_TYPE_PACKET_ARP;
            break;
#endif /* SYS_CPNT_DAI */

        default:
            break;
    }

}

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_GetCraftPortInfo(
    UI8_T *device_id_p,
    UI8_T *device_port_id)
{
#if (RULE_CTRL_HAVE_CRAFT_PORT == TRUE)
    ASSERT(NULL != device_id_p);
    ASSERT(NULL != device_port_id);

    STKTPLG_BOARD_GetCraftPortInfo(device_id_p, device_port_id);
    return RULE_TYPE_OK;
#else
    return RULE_TYPE_FAIL;
#endif
}

void
RULE_CTRL_Priv_DumpRuleEntry(
    const RULE_CTRL_RULE_ENTRY_PTR_T rule_entry_p,
    UI32_T indent)
{
    PRINTF("%*s" "+-o ruleEntry\r\n", indent, " ");

    indent += 4;
    PRINTF("%*s" "{\r\n", indent, " ");

    if (NULL == rule_entry_p)
    {
        indent += 2;
        PRINTF("nil\r\n");

        indent -= 2;
        PRINTF("%*s" "}\r\n", indent, " ");
        return;
    }

    DEVRM_LIB_RulePatternDump(&rule_entry_p->rule_pattern, BACKDOOR_MGR_Printf, indent);
    DEVRM_LIB_ActionDump(&rule_entry_p->action, BACKDOOR_MGR_Printf, indent);

    PRINTF("%*s" "}\r\n", indent, " ");
    indent -= 4;
}

void
RULE_CTRL_Priv_DumpClassInstance(
    const RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie)
{
    RULE_CTRL_DUMP_CLASS_INSTANCE_PARAM_PTR_T param_p;
    RULE_CTRL_DUMP_CLASS_INSTANCE_PARAM_T default_param;

    if (NULL != cookie)
    {
        param_p = (RULE_CTRL_DUMP_CLASS_INSTANCE_PARAM_PTR_T) cookie;
    }
    else
    {
        memset(&default_param, 0, sizeof(default_param));
        default_param.indent = 2;
        param_p = &default_param;
    }

    int indent = param_p->indent;

    if (RULE_CTRL_IS_RULE_INSTANCE_TYPE(in->type))
    {
        RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;

        rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T)in;

        PRINTF("<class = %s, type = %s>\r\n",
               RULE_CTRL_IS_RULE_INSTANCE_TYPE(in->type) == TRUE ? "rule-instance" :
               RULE_CTRL_IS_CLASS_INSTANCE_TYPE(in->type) == TRUE ? "class-instance" : "??",
               RULE_TYPE_INST_POLICY_MAP == in->type ? "policy-map" :
               RULE_TYPE_INST_CLASS_MAP == in->type ? "class-map" :
               RULE_TYPE_INST_ACL == in->type ? "acl" :
               RULE_TYPE_INST_MF_ACE == in->type ? "mf/ace" :
               RULE_TYPE_INST_CPU_IF == in->type ? "cpu/if" :
               RULE_TYPE_INST_ACE == in->type ? "ace" :
               RULE_TYPE_INST_RULE == in->type ? "rule" : "??");

        PRINTF("%*s" "{\r\n", indent, " ");

        indent += 2;

        PRINTF("%*s" "dev_rule_info.unit = %lu\r\n", indent, " ", rule_inst_p->dev_rule_info.unit);
        PRINTF("%*s" "dev_rule_info.device = %lu\r\n", indent, " ", rule_inst_p->dev_rule_info.device);
        PRINTF("%*s" "dev_rule_info.pcl_id = %lu\r\n", indent, " ", rule_inst_p->dev_rule_info.pcl_id);
        PRINTF("%*s" "dev_rule_info.pcl_id_mask = %lu\r\n", indent, " ", rule_inst_p->dev_rule_info.pcl_id_mask);
        PRINTF("%*s" "dev_rule_info.vig_group_id = %lu\r\n", indent, " ", rule_inst_p->dev_rule_info.virtual_group_id);
        PRINTF("%*s" "dev_rule_info.priority = %d\r\n", indent, " ", rule_inst_p->dev_rule_info.priority);
        PRINTF("%*s" "dev_rule_info.rule_id = %lu\r\n", indent, " ", rule_inst_p->dev_rule_info.rule_id);

        PRINTF("%*s" "mvl_rule_index = %lu\r\n", indent, " ", rule_inst_p->mvl_rule_index);
        PRINTF("%*s" "active = %s\r\n", indent, " ", rule_inst_p->active ? "true" : "false");

        {
            UI32_T green_packets = 0;
            UI32_T yellow_packets = 0;
            UI32_T red_packets = 0;

            BOOL_T b_result;

            b_result = RULE_CTRL_LocalGetCounter(rule_inst_p->dev_rule_info.unit,
                                                 rule_inst_p->dev_rule_info.device,
                                                 rule_inst_p->dev_rule_info.pcl_id,
                                                 rule_inst_p->dev_rule_info.rule_id,
                                                 &green_packets,
                                                 &yellow_packets,
                                                 &red_packets);

            if (TRUE == b_result)
            {
                PRINTF("%*s" "hit_count = %lu (gp:%lu yp:%lu rp:%lu)\r\n", indent, " ",
                       green_packets + yellow_packets + red_packets,
                       green_packets, yellow_packets, red_packets);
            }
        }

        {
            RULE_CTRL_RULE_ENTRY_PTR_T rule_entry_p;

            rule_entry_p = RULE_CTRL_Priv_GetRuleEntryOfRule(rule_inst_p);
            ASSERT(NULL != rule_entry_p);

            RULE_CTRL_Priv_DumpRuleEntry(rule_entry_p, indent);
        }

        indent -= 2;
        PRINTF("%*s" "}\r\n", indent, " ");
        param_p->indent = indent;
    }
    else
    {
        RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p;

        class_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)in;

        PRINTF("<class = %s, type = %s>\r\n",
               RULE_CTRL_IS_RULE_INSTANCE_TYPE(in->type) == TRUE ? "rule-instance" :
               RULE_CTRL_IS_CLASS_INSTANCE_TYPE(in->type) == TRUE ? "class-instance" : "??",
               RULE_TYPE_INST_POLICY_MAP == in->type ? "policy-map" :
               RULE_TYPE_INST_CLASS_MAP == in->type ? "class-map" :
               RULE_TYPE_INST_ACL == in->type ? "acl" :
               RULE_TYPE_INST_PBR == in->type ? "pbr" :
               RULE_TYPE_INST_MF_ACE == in->type ? "mf/ace" :
               RULE_TYPE_INST_CPU_IF == in->type ? "cpu/if" :
               RULE_TYPE_INST_ACE == in->type ? "ace" :
               RULE_TYPE_INST_RULE == in->type ? "rule" : "??");

        PRINTF("%*s" "{\r\n", indent, " ");

        indent += 2;

        PRINTF("%*s" "id = %hu\r\n", indent, " ", class_inst_p->id);
        PRINTF("%*s" "pkt-type = %s\r\n", indent, " ", RULE_CTRL_REMAPPED_PACKET_TYPE_STR(class_inst_p->mvl_pkt_type));

        {
            RULE_TYPE_CLASS_INSTANCE_OBJECT_T           class_inst_obj;
            RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;
            RULE_TYPE_INSTANCE_PTR_T                    rule_inst_p;

            RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_inst_p, &class_inst_obj);
            class_inst_obj.get_element_iterator(&class_inst_obj, &rule_inst_iter);

            rule_inst_p = rule_inst_iter.first(&rule_inst_iter);

            if (rule_inst_p)
            {
                PRINTF("%*s" "+-o instance\r\n", indent, " ");

                for (rule_inst_iter.first(&rule_inst_iter);
                     NULL != rule_inst_iter.get_instance(&rule_inst_iter);
                     rule_inst_iter.next(&rule_inst_iter))
                {
                    rule_inst_p = rule_inst_iter.get_instance(&rule_inst_iter);

                    PRINTF("%*s" "  |\r\n", indent, " ");
                    PRINTF("%*s" "  +-o instance ", indent, " ");
                    param_p->indent = indent + 6;

                    RULE_CTRL_Priv_DumpClassInstance(rule_inst_p, param_p);

                    param_p->indent = indent - 6;
                }
            }
        }

        indent -= 2;
        PRINTF("%*s" "}\r\n", indent, " ");
        param_p->indent = indent;
    }

}

static void
RULE_CTRL_Priv_GetUtilization_CallBack(
    RULE_TYPE_INSTANCE_PTR_T in,
    void* cookie)
{
    ASSERT(NULL != in);

    if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(in->type))
    {
        RULE_TYPE_RULE_INSTANCE_PTR_T rule_in_p = (RULE_TYPE_RULE_INSTANCE_PTR_T) in;
        RULE_TYPE_CLASS_INSTANCE_PTR_T parent_p;

        ASSERT(RULE_CTRL_IS_CLASS_INSTANCE_TYPE(in->links.parent.type) == TRUE);

        parent_p = RULE_CTRL_IN_Ptr2Instance(in->links.parent);

        ASSERT(NULL != parent_p);

        if (NULL != parent_p)
        {
            RULE_CTRL_UTILIZATION_PTR_T util_p = (RULE_CTRL_UTILIZATION_PTR_T) cookie;

            ASSERT(rule_in_p->dev_rule_info.unit <= _countof(util_p->unit));
            ASSERT(rule_in_p->dev_rule_info.device < _countof(util_p->unit[0].device));

            ASSERT(parent_p->mvl_pkt_type < _countof(util_p->each_type));
            ASSERT(parent_p->mvl_pkt_type < _countof(util_p->unit[0].device[0].each_type));

            util_p->unit[rule_in_p->dev_rule_info.unit - 1].device[rule_in_p->dev_rule_info.device].each_type[parent_p->mvl_pkt_type] += 1;
            util_p->unit[rule_in_p->dev_rule_info.unit - 1].device[rule_in_p->dev_rule_info.device].total += 1;
            util_p->unit[rule_in_p->dev_rule_info.unit - 1].total += 1;

            util_p->each_type[parent_p->mvl_pkt_type] += 1;

            util_p->total += 1;
        }
    }
}

static void
RULE_CTRL_Priv_GetUtilization(
    RULE_CTRL_UTILIZATION_PTR_T util_p)
{
    UI32_T i;

    memset(util_p, 0, sizeof(*util_p));

    for (i = 0; i < _countof(shmem_data_p->if_table); ++ i)
    {
        RULE_CTRL_IF_ENTRY_PTR_T if_entry_p = &shmem_data_p->if_table[i];

        RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

        ASSERT(RULE_TYPE_NIL != if_entry_p->class_inst_ptr.type);

        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                  &if_class_obj);

        RULE_CTRL_IN_Priv_Walk((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                               RULE_CTRL_Priv_GetUtilization_CallBack, util_p);
    }
}

static void
RULE_CTRL_GetTcamPoolCapabilityBitMap(
    UI32_T pool_id,
    RULE_TYPE_TCAM_CAP_BITMAP_T *bit_map_p)
{
    ASSERT(bit_map_p != NULL);

    /* marvell only has one pool
     */
    RULE_CTRL_GetSupportedTcamCapabilityBitMap(bit_map_p);

    RULE_CTRL_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_ALL);

    return;
}

static BOOL_T
RULE_CTRL_LocalSetRule(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T vir_group_id,
    int rule_priority,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry,
    const DEVRM_ACTION_T *dev_action_p)
{
    ASSERT(DEVRM_FIELD_QUALIFY_MASK_PCL_ID & ace_entry->entry_qualify);
    ASSERT(0 != ace_entry->pcl_id);
    ASSERT(0 != ace_entry->pcl_id_mask);

    {
#if (SYS_CPNT_STACKING == TRUE)
        RULE_CTRL_IscBuf_T   *isc_buf_p;
        L_MM_Mref_Handle_T   *mref_handle_p;
        UI32_T               pdu_len;
        UI16_T               isc_ret_val;
        UI32_T               dst_bmp = 0;
        UI32_T               drv_unit;

        if (STKTPLG_POM_IsLocalUnit(unit, port, &drv_unit) == FALSE)
        {
            dst_bmp |= BIT_VALUE(drv_unit - 1);
        }

        if (dst_bmp != 0)
        {
            mref_handle_p = L_MM_AllocateTxBuffer(sizeof(RULE_CTRL_IscBuf_T),
                                L_MM_USER_ID2(SYS_MODULE_RULE_CTRL, RULE_CTRL_DEVRM_SETRULE));
            isc_buf_p = (RULE_CTRL_IscBuf_T*)L_MM_Mref_GetPdu(mref_handle_p, &pdu_len);
            if (isc_buf_p==NULL)
            {
                SYSFUN_Debug_Printf("\r\n%s():L_MM_Mref_GetPdu() fails", __FUNCTION__);
                return FALSE;
            }

            isc_buf_p->ServiceID = RULE_CTRL_DEVRM_SETRULE;
            isc_buf_p->unit = unit;
            isc_buf_p->port = port;
            isc_buf_p->RULECTRLINFO.device_id = device_id;
            isc_buf_p->RULECTRLINFO.pcl_id = pcl_id;
            isc_buf_p->RULECTRLINFO.vir_group_id = vir_group_id;
            isc_buf_p->RULECTRLINFO.rule_pri = rule_priority;
            isc_buf_p->RULECTRLINFO.dev_rule_id = dev_rule_id;

            memcpy(&isc_buf_p->RULECTRLINFO.ace_entry, ace_entry, sizeof(DEVRM_AceEntry_T));
            isc_buf_p->RULECTRLINFO.dev_action = *dev_action_p;

            isc_ret_val=ISC_SendMcastReliable(dst_bmp, ISC_RULE_CTRL_SID,
                                              mref_handle_p, SYS_DFLT_STK_MGMT_PACKET_TO_CPU_PRIORITY,
                                              RULE_CTRL_RETRY_TIMES, RULE_CTRL_TIME_OUT, FALSE);
            if (isc_ret_val!=0)
            {
                SYSFUN_Debug_Printf("\r\n%s():ISC_SendMcastReliable() fails(ret_val=0x%x)", __FUNCTION__, isc_ret_val);
                return FALSE;   /* can't control remote unit */
            }
        }
        else
#endif  /* SYS_CPNT_STACKING */
        {
            return RULE_CTRL_LocalRealSetRule(device_id,
                                              pcl_id,
                                              vir_group_id,
                                              rule_priority,
                                              dev_rule_id,
                                              ace_entry,
                                              dev_action_p);
        }
    }

    return TRUE;
}

static BOOL_T
RULE_CTRL_LocalDestroyRule(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id)
{
#if (SYS_CPNT_STACKING == TRUE)
    RULE_CTRL_IscBuf_T* isc_buf_p;
    L_MM_Mref_Handle_T* mref_handle_p;
    UI32_T              pdu_len;
    UI16_T              isc_ret_val;
    UI16_T              dst_bmp = 0;
    UI32_T              drv_unit;

    if (STKTPLG_POM_IsLocalUnit(unit, port, &drv_unit) == FALSE)
    {
        dst_bmp |= BIT_VALUE(drv_unit - 1);
    }

    if (dst_bmp != 0)
    {
        mref_handle_p = L_MM_AllocateTxBuffer(sizeof(RULE_CTRL_IscBuf_T),
                            L_MM_USER_ID2(SYS_MODULE_RULE_CTRL, RULE_CTRL_DEVRM_DESTROYRULE));
        isc_buf_p = (RULE_CTRL_IscBuf_T*)L_MM_Mref_GetPdu(mref_handle_p, &pdu_len);
        if (isc_buf_p==NULL)
        {
            SYSFUN_Debug_Printf("\r\n%s():L_MM_Mref_GetPdu() fails", __FUNCTION__);
            return FALSE;
        }

        isc_buf_p->ServiceID = RULE_CTRL_DEVRM_DESTROYRULE;
        isc_buf_p->unit = unit;
        isc_buf_p->port = port;
        isc_buf_p->RULECTRLINFO.device_id = device_id;
        isc_buf_p->RULECTRLINFO.pcl_id    = pcl_id;
        isc_buf_p->RULECTRLINFO.dev_rule_id = dev_rule_id;

        isc_ret_val=ISC_SendMcastReliable(dst_bmp, ISC_RULE_CTRL_SID,
                                          mref_handle_p, SYS_DFLT_STK_MGMT_PACKET_TO_CPU_PRIORITY,
                                          RULE_CTRL_RETRY_TIMES, RULE_CTRL_TIME_OUT, FALSE);
        if (isc_ret_val!=0)
        {
            SYSFUN_Debug_Printf("\r\n%s():ISC_SendMcastReliable() fails(ret_val=0x%x)", __FUNCTION__, isc_ret_val);
            return FALSE;   /* can't control remote unit */
        }
    }
    else
#endif /* SYS_CPNT_STACKING */
    {
        return RULE_CTRL_LocalRealDestroyRule(device_id, pcl_id, dev_rule_id);
    }

    return TRUE;
}

static BOOL_T
RULE_CTRL_LocalUpdateMeter(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    const DEVRM_POLICER_METER_T *dev_meter_p)
{
#if (SYS_CPNT_STACKING == TRUE)
    RULE_CTRL_IscBuf_T* isc_buf_p;
    L_MM_Mref_Handle_T* mref_handle_p;
    UI32_T              pdu_len;
    UI16_T              isc_ret_val;
    UI16_T              dst_bmp = 0;
    UI32_T              drv_unit;

    if (STKTPLG_POM_IsLocalUnit(unit, port, &drv_unit) == FALSE)
    {
        dst_bmp |= BIT_VALUE(drv_unit - 1);
    }

    if (dst_bmp != 0)
    {
        mref_handle_p = L_MM_AllocateTxBuffer(sizeof(RULE_CTRL_IscBuf_T),
                            L_MM_USER_ID2(SYS_MODULE_RULE_CTRL, RULE_CTRL_DEVRM_UPDATEMETER));
        isc_buf_p = (RULE_CTRL_IscBuf_T*)L_MM_Mref_GetPdu(mref_handle_p, &pdu_len);
        if (isc_buf_p==NULL)
        {
            SYSFUN_Debug_Printf("\r\n%s():L_MM_Mref_GetPdu() fails", __FUNCTION__);
            return FALSE;
        }

        isc_buf_p->ServiceID = RULE_CTRL_DEVRM_UPDATEMETER;
        isc_buf_p->unit = unit;
        isc_buf_p->port = port;
        isc_buf_p->RULECTRLINFO.device_id = device_id;
        isc_buf_p->RULECTRLINFO.pcl_id = pcl_id;
        isc_buf_p->RULECTRLINFO.dev_rule_id = dev_rule_id;
        isc_buf_p->RULECTRLINFO.dev_meter = *dev_meter_p;

        isc_ret_val=ISC_SendMcastReliable(dst_bmp, ISC_RULE_CTRL_SID,
                                          mref_handle_p, SYS_DFLT_STK_MGMT_PACKET_TO_CPU_PRIORITY,
                                          RULE_CTRL_RETRY_TIMES, RULE_CTRL_TIME_OUT, FALSE);
        if (isc_ret_val!=0)
        {
            SYSFUN_Debug_Printf("\r\n%s():ISC_SendMcastReliable() fails(ret_val=0x%x)", __FUNCTION__, isc_ret_val);
            return FALSE;   /* can't control remote unit */
        }
    }
    else
#endif /* SYS_CPNT_STACKING */
    {
        return RULE_CTRL_LocalRealUpdateMeter(device_id, pcl_id, dev_rule_id, dev_meter_p);
    }

    return TRUE;
}

/*src*/
static BOOL_T
RULE_CTRL_LocalShareMeter(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T src_dev_rule_id,
    UI32_T dev_rule_id)
{
#if (SYS_CPNT_STACKING == TRUE)
    RULE_CTRL_IscBuf_T* isc_buf_p;
    L_MM_Mref_Handle_T* mref_handle_p;
    UI32_T              pdu_len;
    UI16_T              isc_ret_val;
    UI16_T              dst_bmp = 0;
    UI32_T              drv_unit;

    if (STKTPLG_POM_IsLocalUnit(unit, port, &drv_unit) == FALSE)
    {
        dst_bmp |= BIT_VALUE(drv_unit - 1);
    }

    if (dst_bmp != 0)
    {
        mref_handle_p = L_MM_AllocateTxBuffer(sizeof(RULE_CTRL_IscBuf_T), L_MM_USER_ID2(SYS_MODULE_RULE_CTRL, RULE_CTRL_DEVRM_SHAREMETER));
        isc_buf_p = (RULE_CTRL_IscBuf_T*)L_MM_Mref_GetPdu(mref_handle_p, &pdu_len);
        if (isc_buf_p==NULL)
        {
            SYSFUN_Debug_Printf("\r\n%s():L_MM_Mref_GetPdu() fails", __FUNCTION__);
            return FALSE;
        }

        isc_buf_p->ServiceID = RULE_CTRL_DEVRM_SHAREMETER;
        isc_buf_p->unit = unit;
        isc_buf_p->port = port;
        isc_buf_p->RULECTRLINFO.device_id = device_id;
        isc_buf_p->RULECTRLINFO.dev_rule_id = dev_rule_id;
        isc_buf_p->RULECTRLINFO.src_dev_rule_id = src_dev_rule_id;

        isc_ret_val=ISC_SendMcastReliable(dst_bmp, ISC_RULE_CTRL_SID,
                                          mref_handle_p, SYS_DFLT_STK_MGMT_PACKET_TO_CPU_PRIORITY,
                                          RULE_CTRL_RETRY_TIMES, RULE_CTRL_TIME_OUT, FALSE);
        if (isc_ret_val!=0)
        {
            SYSFUN_Debug_Printf("\r\n%s():ISC_SendMcastReliable() fails(ret_val=0x%x)", __FUNCTION__, isc_ret_val);
            return FALSE;   /* can't control remote unit */
        }
    }
    else
#endif /* SYS_CPNT_STACKING */
    {
        return RULE_CTRL_LocalRealShareMeter(device_id, src_dev_rule_id, dev_rule_id);
    }

    return TRUE;
}

static BOOL_T
RULE_CTRL_LocalGetCounter(
    UI32_T unit,
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    UI32_T *green_cnt_p,
    UI32_T *yellow_cnt_p,
    UI32_T *red_cnt_p)
{
#if (SYS_CPNT_STACKING == TRUE)
    RULE_CTRL_IscBuf_T* isc_buf_p;
    L_MM_Mref_Handle_T* mref_handle_p;
    UI32_T              pdu_len;
    UI32_T              drv_unit;

    if (STKTPLG_POM_IsLocalUnit(unit, 1, &drv_unit) == FALSE)
    {
        RULE_CTRL_IscReplyBuf_T isc_reply;

        mref_handle_p = L_MM_AllocateTxBuffer(sizeof(RULE_CTRL_IscBuf_T),
                                              L_MM_USER_ID2(SYS_MODULE_RULE_CTRL, RULE_CTRL_DEVRM_GETCOUNTER));

        isc_buf_p = (RULE_CTRL_IscBuf_T*)L_MM_Mref_GetPdu(mref_handle_p, &pdu_len);
        if (isc_buf_p==NULL)
        {
            SYSFUN_Debug_Printf("\r\n%s():L_MM_Mref_GetPdu() fails", __FUNCTION__);
            return FALSE;
        }

        isc_buf_p->ServiceID = RULE_CTRL_DEVRM_GETCOUNTER;
        isc_buf_p->unit = unit;
        isc_buf_p->RULECTRLINFO.device_id = device_id;
        isc_buf_p->RULECTRLINFO.pcl_id = pcl_id;
        isc_buf_p->RULECTRLINFO.dev_rule_id = dev_rule_id;

        if (!ISC_RemoteCall((UI8_T) drv_unit,
                            ISC_RULE_CTRL_SID,
                            mref_handle_p,
                            SYS_DFLT_STK_MGMT_PACKET_TO_CPU_PRIORITY,
                            sizeof(isc_reply),
                            (UI8_T *)&isc_reply,
                            RULE_CTRL_RETRY_TIMES,
                            RULE_CTRL_TIME_OUT))
        {
            return FALSE;
        }

        if (TRUE != isc_reply.return_value.b_result)
        {
            return FALSE;
        }

        *green_cnt_p = isc_reply.data.acl_counter.green_cnt;
        *yellow_cnt_p = isc_reply.data.acl_counter.yellow_cnt;
        *red_cnt_p = isc_reply.data.acl_counter.red_cnt;

    }
    else
#endif /* SYS_CPNT_STACKING */
    {
        return RULE_CTRL_LocalRealGetCounter(device_id,
                                             pcl_id,
                                             dev_rule_id,
                                             green_cnt_p,
                                             yellow_cnt_p,
                                             red_cnt_p);
    }

    return TRUE;
}

static BOOL_T
RULE_CTRL_LocalClearCounter(
    UI32_T unit,
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id)
{
#if (SYS_CPNT_STACKING == TRUE)
    RULE_CTRL_IscBuf_T* isc_buf_p;
    L_MM_Mref_Handle_T* mref_handle_p;
    UI32_T              pdu_len;
    UI16_T              isc_ret_val;
    UI16_T              dst_bmp = 0;
    UI32_T              drv_unit;

    if (STKTPLG_POM_IsLocalUnit(unit, 1, &drv_unit) == FALSE)
    {
        dst_bmp |= BIT_VALUE(drv_unit - 1);
    }

    if (dst_bmp != 0)
    {
        mref_handle_p = L_MM_AllocateTxBuffer(sizeof(RULE_CTRL_IscBuf_T),
                                              L_MM_USER_ID2(SYS_MODULE_RULE_CTRL, RULE_CTRL_DEVRM_CLEARCOUNTER));

        isc_buf_p = (RULE_CTRL_IscBuf_T*)L_MM_Mref_GetPdu(mref_handle_p, &pdu_len);
        if (isc_buf_p==NULL)
        {
            SYSFUN_Debug_Printf("\r\n%s():L_MM_Mref_GetPdu() fails", __FUNCTION__);
            return FALSE;
        }

        isc_buf_p->ServiceID = RULE_CTRL_DEVRM_CLEARCOUNTER;
        isc_buf_p->unit = unit;
        isc_buf_p->RULECTRLINFO.device_id = device_id;
        isc_buf_p->RULECTRLINFO.pcl_id = pcl_id;
        isc_buf_p->RULECTRLINFO.dev_rule_id = dev_rule_id;

        isc_ret_val=ISC_SendMcastReliable(dst_bmp, ISC_RULE_CTRL_SID,
                                          mref_handle_p, SYS_DFLT_STK_MGMT_PACKET_TO_CPU_PRIORITY,
                                          RULE_CTRL_RETRY_TIMES, RULE_CTRL_TIME_OUT, FALSE);
        if (isc_ret_val!=0)
        {
            SYSFUN_Debug_Printf("\r\n%s():ISC_SendMcastReliable() fails(ret_val=0x%x)", __FUNCTION__, isc_ret_val);
            return FALSE;   /* can't control remote unit */
        }
    }
    else
#endif /* SYS_CPNT_STACKING */
    {
        return RULE_CTRL_LocalRealClearCounter(device_id,
                                               pcl_id,
                                               dev_rule_id);
    }

    return TRUE;
}

static BOOL_T
RULE_CTRL_LocalGetDevInfo(
    UI32_T          unit,
    DEVRM_DevInfo_T *dev_info_p)
{
#if (SYS_CPNT_STACKING == TRUE)
    RULE_CTRL_IscBuf_T* isc_buf_p;
    L_MM_Mref_Handle_T* mref_handle_p;
    UI32_T              pdu_len;
    UI32_T              drv_unit;

    if (STKTPLG_POM_IsLocalUnit(unit, 1, &drv_unit) == FALSE)
    {
        RULE_CTRL_IscReplyBuf_T isc_reply;

        mref_handle_p = L_MM_AllocateTxBuffer(sizeof(RULE_CTRL_IscBuf_T),
                                              L_MM_USER_ID2(SYS_MODULE_RULE_CTRL, RULE_CTRL_DEVRM_GETDEVINFO));

        isc_buf_p = (RULE_CTRL_IscBuf_T*)L_MM_Mref_GetPdu(mref_handle_p, &pdu_len);
        if (isc_buf_p==NULL)
        {
            SYSFUN_Debug_Printf("\r\n%s():L_MM_Mref_GetPdu() fails", __FUNCTION__);
            return FALSE;
        }

        isc_buf_p->ServiceID = RULE_CTRL_DEVRM_GETDEVINFO;
        isc_buf_p->unit = unit;

        if (!ISC_RemoteCall((UI8_T) drv_unit,
                            ISC_RULE_CTRL_SID,
                            mref_handle_p,
                            SYS_DFLT_STK_MGMT_PACKET_TO_CPU_PRIORITY,
                            sizeof(isc_reply),
                            (UI8_T *)&isc_reply,
                            RULE_CTRL_RETRY_TIMES,
                            RULE_CTRL_TIME_OUT))
        {
            return FALSE;
        }

        if (TRUE != isc_reply.return_value.b_result)
        {
            return FALSE;
        }

        memcpy(dev_info_p, &isc_reply.data.device_info, sizeof(*dev_info_p));
    }
    else
#endif /* SYS_CPNT_STACKING */
    {
        return RULE_CTRL_LocalRealGetDevInfo(dev_info_p);
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalGetFirstRuleInstanceOfQos
 *------------------------------------------------------------------------------
 * PURPOSE:  Get first rule instance
 * INPUT:    ifindex - port index
 *           direction - ingress or egress
 *           policy_id - policy-map id
 *           class_id - class-map id
 * OUTPUT:   rule_inst_iter - first rule iterator
 * RETURN:   Error code
 * NOTE: None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalGetFirstRuleInstanceOfQos(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_id,
    UI32_T class_id,
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *rule_inst_iter)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_INTERFACE_INFO_T interface;
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;
	
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T class_map_inst_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p;

    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T acl_mf_ace_inst_iter;
    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_mf_ace_inst_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T ace_inst_obj;

    memset(&interface, 0, sizeof(interface));

    result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Invalid parameter");
        return RULE_TYPE_INVALID_PARAMETER;
    }

    interface.direction = direction;

    if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    memset(&param, 0, sizeof(param));
    param.type = RULE_TYPE_INST_POLICY_MAP;
    param.id = policy_id;
    policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                       RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                       &param);
    if (policy_map_inst_p == NULL)
    {
        LOG("<Error> Invalid parameter: policy_map_index = %lu", policy_id);
        return RULE_TYPE_INVALID_PARAMETER;
    }

    memset(&param, 0, sizeof(param));
    param.type = RULE_TYPE_INST_CLASS_MAP;
    param.id = class_id;
    class_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) policy_map_inst_p,
                                                                                      RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                      &param);
    if (class_map_inst_p == NULL)
    {
        LOG("<Error> Invalid parameter: class_map_index = %lu", class_id);
        return RULE_TYPE_INVALID_PARAMETER;
    }

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_map_inst_p, &class_map_inst_obj);
	
    class_map_inst_obj.get_element_iterator(&class_map_inst_obj, &acl_mf_ace_inst_iter);	
    acl_mf_ace_inst_iter.first(&acl_mf_ace_inst_iter);	
    acl_mf_ace_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)acl_mf_ace_inst_iter.get_instance(&acl_mf_ace_inst_iter);
    if (acl_mf_ace_inst_p == NULL)
    {
        return RULE_TYPE_NO_HW_RULE_SET;
    }

    if (acl_mf_ace_inst_p->super.type == RULE_TYPE_INST_MF_ACE)
    {	
        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_mf_ace_inst_p, &ace_inst_obj);	
    }
    else if (acl_mf_ace_inst_p->super.type == RULE_TYPE_INST_ACL)
    {
        RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;     
        RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T ace_inst_iter;
        RULE_TYPE_CLASS_INSTANCE_PTR_T ace_inst_p;

        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_mf_ace_inst_p, &acl_inst_obj);
        acl_inst_obj.get_element_iterator(&acl_inst_obj, &ace_inst_iter);
		
        ace_inst_iter.first(&ace_inst_iter);
        ace_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)ace_inst_iter.get_instance(&ace_inst_iter);
        if (ace_inst_p == NULL)
        {
            return RULE_TYPE_NO_HW_RULE_SET;
        }

        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(ace_inst_p, &ace_inst_obj);
    }
    else
    {
        return RULE_TYPE_FAIL;
    }

    ace_inst_obj.get_element_iterator(&ace_inst_obj, rule_inst_iter);
	
    return RULE_TYPE_OK;
}

static BOOL_T
RULE_CTRL_LocalBindPclToPort(
    UI32_T unit,
    UI32_T device_id,
    UI32_T phy_port,
    UI32_T pcl_id)
{
#if (SYS_CPNT_STACKING == TRUE)
    RULE_CTRL_IscBuf_T* isc_buf_p;
    L_MM_Mref_Handle_T* mref_handle_p;
    UI32_T              pdu_len;
    UI16_T              isc_ret_val;
    UI16_T              dst_bmp = 0;
    UI32_T              drv_unit;

    if (STKTPLG_POM_IsLocalUnit(unit, 1, &drv_unit) == FALSE)
    {
        dst_bmp |= BIT_VALUE(drv_unit - 1);
    }

    if (dst_bmp != 0)
    {
        mref_handle_p = L_MM_AllocateTxBuffer(sizeof(RULE_CTRL_IscBuf_T),
                                              L_MM_USER_ID2(SYS_MODULE_RULE_CTRL, RULE_CTRL_DEVRM_BINDPCLTOPORT));

        isc_buf_p = (RULE_CTRL_IscBuf_T*)L_MM_Mref_GetPdu(mref_handle_p, &pdu_len);
        if (isc_buf_p==NULL)
        {
            SYSFUN_Debug_Printf("\r\n%s():L_MM_Mref_GetPdu() fails", __FUNCTION__);
            return FALSE;
        }

        isc_buf_p->ServiceID = RULE_CTRL_DEVRM_BINDPCLTOPORT;
        isc_buf_p->unit = unit;
        isc_buf_p->RULECTRLINFO.device_id = device_id;
        isc_buf_p->RULECTRLINFO.pcl_id = pcl_id;
        isc_buf_p->RULECTRLINFO.phy_port = phy_port;

        isc_ret_val=ISC_SendMcastReliable(dst_bmp, ISC_RULE_CTRL_SID,
                                          mref_handle_p, SYS_DFLT_STK_MGMT_PACKET_TO_CPU_PRIORITY,
                                          RULE_CTRL_RETRY_TIMES, RULE_CTRL_TIME_OUT, FALSE);
        if (isc_ret_val!=0)
        {
            SYSFUN_Debug_Printf("\r\n%s():ISC_SendMcastReliable() fails(ret_val=0x%x)", __FUNCTION__, isc_ret_val);
            return FALSE;   /* can't control remote unit */
        }
    }
    else
#endif /* SYS_CPNT_STACKING */
    {
        return RULE_CTRL_LocalRealBindPclToPort(device_id, phy_port, pcl_id);
    }

    return TRUE;
}

static BOOL_T
RULE_CTRL_LocalUnbindPclToPort(
    UI32_T unit,
    UI32_T device_id,
    UI32_T phy_port,
    UI32_T pcl_id)
{
#if (SYS_CPNT_STACKING == TRUE)
    RULE_CTRL_IscBuf_T* isc_buf_p;
    L_MM_Mref_Handle_T* mref_handle_p;
    UI32_T              pdu_len;
    UI16_T              isc_ret_val;
    UI16_T              dst_bmp = 0;
    UI32_T              drv_unit;

    if (STKTPLG_POM_IsLocalUnit(unit, 1, &drv_unit) == FALSE)
    {
        dst_bmp |= BIT_VALUE(drv_unit - 1);
    }

    if (dst_bmp != 0)
    {
        mref_handle_p = L_MM_AllocateTxBuffer(sizeof(RULE_CTRL_IscBuf_T),
                                              L_MM_USER_ID2(SYS_MODULE_RULE_CTRL, RULE_CTRL_DEVRM_UNBINDPCLTOPORT));

        isc_buf_p = (RULE_CTRL_IscBuf_T*)L_MM_Mref_GetPdu(mref_handle_p, &pdu_len);
        if (isc_buf_p==NULL)
        {
            SYSFUN_Debug_Printf("\r\n%s():L_MM_Mref_GetPdu() fails", __FUNCTION__);
            return FALSE;
        }

        isc_buf_p->ServiceID = RULE_CTRL_DEVRM_UNBINDPCLTOPORT;
        isc_buf_p->unit = unit;
        isc_buf_p->RULECTRLINFO.device_id = device_id;
        isc_buf_p->RULECTRLINFO.pcl_id = pcl_id;
        isc_buf_p->RULECTRLINFO.phy_port = phy_port;

        isc_ret_val=ISC_SendMcastReliable(dst_bmp, ISC_RULE_CTRL_SID,
                                          mref_handle_p, SYS_DFLT_STK_MGMT_PACKET_TO_CPU_PRIORITY,
                                          RULE_CTRL_RETRY_TIMES, RULE_CTRL_TIME_OUT, FALSE);
        if (isc_ret_val!=0)
        {
            SYSFUN_Debug_Printf("\r\n%s():ISC_SendMcastReliable() fails(ret_val=0x%x)", __FUNCTION__, isc_ret_val);
            return FALSE;   /* can't control remote unit */
        }
    }
    else
#endif /* SYS_CPNT_STACKING */
    {
        return RULE_CTRL_LocalRealUnbindPclToPort(device_id, phy_port, pcl_id);
    }

    return TRUE;
}

static BOOL_T
RULE_CTRL_LocalBindPclToTrunkPort(
    UI32_T unit,
    UI32_T device_id,
    UI32_T trunk_id,
    UI32_T pcl_id)
{
#if (SYS_CPNT_STACKING == TRUE)
    RULE_CTRL_IscBuf_T* isc_buf_p;
    L_MM_Mref_Handle_T* mref_handle_p;
    UI32_T              pdu_len;
    UI16_T              isc_ret_val;
    UI16_T              dst_bmp = 0;
    UI32_T              drv_unit;

    if (STKTPLG_POM_IsLocalUnit(unit, 1, &drv_unit) == FALSE)
    {
        dst_bmp |= BIT_VALUE(drv_unit - 1);
    }

    if (dst_bmp != 0)
    {
        mref_handle_p = L_MM_AllocateTxBuffer(sizeof(RULE_CTRL_IscBuf_T),
                                              L_MM_USER_ID2(SYS_MODULE_RULE_CTRL, RULE_CTRL_DEVRM_BINDPCLTOTRUNKPORT));

        isc_buf_p = (RULE_CTRL_IscBuf_T*)L_MM_Mref_GetPdu(mref_handle_p, &pdu_len);
        if (isc_buf_p==NULL)
        {
            SYSFUN_Debug_Printf("\r\n%s():L_MM_Mref_GetPdu() fails", __FUNCTION__);
            return FALSE;
        }

        isc_buf_p->ServiceID = RULE_CTRL_DEVRM_BINDPCLTOTRUNKPORT;
        isc_buf_p->unit = unit;
        isc_buf_p->RULECTRLINFO.device_id = device_id;
        isc_buf_p->RULECTRLINFO.pcl_id = pcl_id;
        isc_buf_p->RULECTRLINFO.trunk_id = trunk_id;

        isc_ret_val=ISC_SendMcastReliable(dst_bmp, ISC_RULE_CTRL_SID,
                                          mref_handle_p, SYS_DFLT_STK_MGMT_PACKET_TO_CPU_PRIORITY,
                                          RULE_CTRL_RETRY_TIMES, RULE_CTRL_TIME_OUT, FALSE);
        if (isc_ret_val!=0)
        {
            SYSFUN_Debug_Printf("\r\n%s():ISC_SendMcastReliable() fails(ret_val=0x%x)", __FUNCTION__, isc_ret_val);
            return FALSE;   /* can't control remote unit */
        }
    }
    else
#endif /* SYS_CPNT_STACKING */
    {
        return RULE_CTRL_LocalRealBindPclToTrunkPort(device_id, trunk_id, pcl_id);
    }

    return TRUE;
}

static BOOL_T
RULE_CTRL_LocalUnbindPclToTrunkPort(
    UI32_T unit,
    UI32_T device_id,
    UI32_T trunk_id,
    UI32_T pcl_id)
{
#if (SYS_CPNT_STACKING == TRUE)
    RULE_CTRL_IscBuf_T* isc_buf_p;
    L_MM_Mref_Handle_T* mref_handle_p;
    UI32_T              pdu_len;
    UI16_T              isc_ret_val;
    UI16_T              dst_bmp = 0;
    UI32_T              drv_unit;

    if (STKTPLG_POM_IsLocalUnit(unit, 1, &drv_unit) == FALSE)
    {
        dst_bmp |= BIT_VALUE(drv_unit - 1);
    }

    if (dst_bmp != 0)
    {
        mref_handle_p = L_MM_AllocateTxBuffer(sizeof(RULE_CTRL_IscBuf_T),
                                              L_MM_USER_ID2(SYS_MODULE_RULE_CTRL, RULE_CTRL_DEVRM_UNBINDPCLTOTRUNKPORT));

        isc_buf_p = (RULE_CTRL_IscBuf_T*)L_MM_Mref_GetPdu(mref_handle_p, &pdu_len);
        if (isc_buf_p==NULL)
        {
            SYSFUN_Debug_Printf("\r\n%s():L_MM_Mref_GetPdu() fails", __FUNCTION__);
            return FALSE;
        }

        isc_buf_p->ServiceID = RULE_CTRL_DEVRM_UNBINDPCLTOTRUNKPORT;
        isc_buf_p->unit = unit;
        isc_buf_p->RULECTRLINFO.device_id = device_id;
        isc_buf_p->RULECTRLINFO.pcl_id = pcl_id;
        isc_buf_p->RULECTRLINFO.trunk_id = trunk_id;

        isc_ret_val=ISC_SendMcastReliable(dst_bmp, ISC_RULE_CTRL_SID,
                                          mref_handle_p, SYS_DFLT_STK_MGMT_PACKET_TO_CPU_PRIORITY,
                                          RULE_CTRL_RETRY_TIMES, RULE_CTRL_TIME_OUT, FALSE);
        if (isc_ret_val!=0)
        {
            SYSFUN_Debug_Printf("\r\n%s():ISC_SendMcastReliable() fails(ret_val=0x%x)", __FUNCTION__, isc_ret_val);
            return FALSE;   /* can't control remote unit */
        }
    }
    else
#endif /* SYS_CPNT_STACKING */
    {
        return RULE_CTRL_LocalRealUnbindPclToTrunkPort(device_id, trunk_id, pcl_id);
    }

    return TRUE;
}

static BOOL_T
RULE_CTRL_LocalRealSetRule(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T vir_group_id,
    int rule_pri,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry,
    const DEVRM_ACTION_T *dev_action_p)
{
    BOOL_T ret;

    LOG("deviceId = %lu, pclId = %lu, vgId = %lu, rulePri = %d, devRuleId = %lu",
        device_id, pcl_id, vir_group_id, rule_pri, dev_rule_id);

    ret = DEVRM_PMGR_AllocateRule(device_id, pcl_id, vir_group_id, rule_pri,
                                                                   dev_rule_id);

    if (FALSE == ret)
    {
        LOG("<Error> Failed to allocate rule.");
        return FALSE;
    }

    ret = DEVRM_PMGR_SetRule(device_id, pcl_id, dev_rule_id, ace_entry,
                                                                  dev_action_p);

    if (FALSE == ret)
    {
        LOG("<Error> Failed to set rule.");

        RULE_CTRL_LocalRealDestroyRule(device_id, pcl_id, dev_rule_id);
        return FALSE;
    }

    return TRUE;
}

static BOOL_T
RULE_CTRL_LocalRealDestroyRule(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id)
{
    BOOL_T res;

    res = DEVRM_PMGR_DestroyRule(device_id, pcl_id, dev_rule_id);

    return res;
}

static BOOL_T
RULE_CTRL_LocalRealUpdateMeter(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    const DEVRM_POLICER_METER_T *dev_meter_p)
{
    if (TRUE == (DEVRM_PMGR_UpdateMeter(device_id, pcl_id, dev_rule_id, dev_meter_p)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static BOOL_T
RULE_CTRL_LocalRealShareMeter(
    UI32_T device_id,
    UI32_T src_dev_rule_id,
    UI32_T dev_rule_id)
{
    if (TRUE == (DEVRM_PMGR_ShareMeter(device_id, src_dev_rule_id, dev_rule_id)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static BOOL_T
RULE_CTRL_LocalRealGetCounter(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    UI32_T *green_cnt_p,
    UI32_T *yellow_cnt_p,
    UI32_T *red_cnt_p)
{
    LOG("deviceId = %lu, pclId = %lu, devRuleId = %lu",
        device_id, pcl_id, dev_rule_id);

    return DEVRM_PMGR_GetCounter(device_id,
                                 pcl_id,
                                 dev_rule_id,
                                 green_cnt_p,
                                 yellow_cnt_p,
                                 red_cnt_p);
}

static BOOL_T
RULE_CTRL_LocalRealClearCounter(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id)
{
    LOG("deviceId = %lu, pclId = %lu, devRuleId = %lu",
        device_id, pcl_id, dev_rule_id);

    return DEVRM_PMGR_ClearCounter(device_id, pcl_id, dev_rule_id);
}

static BOOL_T
RULE_CTRL_LocalRealGetDevInfo(
    DEVRM_DevInfo_T *dev_info_p)
{
    return DEVRM_PMGR_GetDeviceInfo(dev_info_p);
}

static BOOL_T
RULE_CTRL_LocalRealBindPclToPort(
    UI32_T device_id,
    UI32_T phy_port,
    UI32_T pcl_id)
{
    return DEVRM_PMGR_BindPclToPort(device_id, phy_port, pcl_id);
}

static BOOL_T
RULE_CTRL_LocalRealUnbindPclToPort(
    UI32_T device_id,
    UI32_T phy_port,
    UI32_T pcl_id)
{
    return DEVRM_PMGR_UnbindPclToPort(device_id, phy_port, pcl_id);
}

static BOOL_T
RULE_CTRL_LocalRealBindPclToTrunkPort(
    UI32_T device_id,
    UI32_T trunk_id,
    UI32_T pcl_id)
{
    return DEVRM_PMGR_BindPclToTrunkPort(device_id, trunk_id, pcl_id);
}

static BOOL_T
RULE_CTRL_LocalRealUnbindPclToTrunkPort(
    UI32_T device_id,
    UI32_T trunk_id,
    UI32_T pcl_id)
{
    return DEVRM_PMGR_UnbindPclToTrunkPort(device_id, trunk_id, pcl_id);
}

#if (SYS_CPNT_STACKING == TRUE)
/*------------------------------------------------------------------------------
 * Function : RULE_CTRL_ISC_Handler
 *------------------------------------------------------------------------------
 * Purpose  : This function will manipulte all of RULE_CTRL via ISC
 * INPUT    : *key      -- key of ISC
 *            *mref_handle_p  -- transfer data
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : called by ISC Agent
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_ISC_Handler(ISC_Key_T *key, L_MM_Mref_Handle_T *mref_handle_p)
{
    RULE_CTRL_IscBuf_T *buf_p;
    BOOL_T  ret = FALSE;
    UI32_T  service;

    buf_p = (RULE_CTRL_IscBuf_T*)L_MM_Mref_GetPdu(mref_handle_p, &service); /* service is used as dummy here */
    if (buf_p==NULL)
    {
        SYSFUN_Debug_Printf("\r\n%s():L_MM_Mref_GetPdu() fails", __FUNCTION__);
        return FALSE;
    }

    service = buf_p->ServiceID;

    /*
     * Check to abort operation if callback service id(opcode) is more then
     * number of callback service on this drive.
     */
    if (service >= RULE_CTRL_NBR_OF_SERVICE_ID || RULE_CTRL_func_tab[service]==NULL)
    {
        printf("\r\nRULE CTRL: Service ID is invalid!\r\n");
    }
    else
    {
        ret = RULE_CTRL_func_tab[service](key,buf_p);
    }

    L_MM_Mref_Release(&mref_handle_p);

    return ret;
}

static BOOL_T RULE_CTRL_Slave_Set_Rule(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr)
{
    BOOL_T status;

    status = RULE_CTRL_LocalRealSetRule(ptr->RULECTRLINFO.device_id,
                    ptr->RULECTRLINFO.pcl_id,
                    ptr->RULECTRLINFO.vir_group_id,
                    ptr->RULECTRLINFO.rule_pri,
                    ptr->RULECTRLINFO.dev_rule_id,
                    &(ptr->RULECTRLINFO.ace_entry),
                    &(ptr->RULECTRLINFO.dev_action));

    if (!status)
        SYSFUN_Debug_Printf("\r\b%s():RULE_CTRL_Slave_Set_Rule fails", __FUNCTION__);
    return status;

}

static BOOL_T
RULE_CTRL_Slave_Destroy_Rule(
    ISC_Key_T *key,
    RULE_CTRL_IscBuf_T *ptr)
{
    BOOL_T status = TRUE;
    if (FALSE == RULE_CTRL_LocalRealDestroyRule(
            ptr->RULECTRLINFO.device_id,
            ptr->RULECTRLINFO.pcl_id,
            ptr->RULECTRLINFO.dev_rule_id))
    {
        status = FALSE;
        SYSFUN_Debug_Printf("\r\b%s():RULE_CTRL_Local_Destroy_Rule fails", __FUNCTION__);
    }

    return status;
}

static BOOL_T RULE_CTRL_Slave_Update_Meter(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr)
{
    BOOL_T status = TRUE;

    if (FALSE == RULE_CTRL_LocalRealUpdateMeter(ptr->RULECTRLINFO.device_id,
                                                ptr->RULECTRLINFO.pcl_id,
                                                ptr->RULECTRLINFO.dev_rule_id,
                                                &(ptr->RULECTRLINFO.dev_meter)))
    {
        status = FALSE;
        SYSFUN_Debug_Printf("\r\b%s():RULE_CTRL_Local_Update_Meter fails", __FUNCTION__);
    }

    return status;
}

static BOOL_T RULE_CTRL_Slave_Share_Meter(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr)
{
    BOOL_T status = TRUE;

    if (FALSE == RULE_CTRL_LocalRealShareMeter(ptr->RULECTRLINFO.device_id,
                                               ptr->RULECTRLINFO.src_dev_rule_id,
                                               ptr->RULECTRLINFO.dev_rule_id))
    {
        status = FALSE;
        SYSFUN_Debug_Printf("\r\b%s():RULE_CTRL_Local_Share_Meter fails", __FUNCTION__);
    }

    return status;
}

static BOOL_T RULE_CTRL_Slave_Get_Couner(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr)
{
    UI32_T                   pdu_len;
    L_MM_Mref_Handle_T      *mref_handle_p;
    RULE_CTRL_IscReplyBuf_T *isc_reply_p;

    LOG("Get_Couner\r\n"
        "  device_id: %lu, pcl_id: %lu, dev_rule_id: %lu\r\n",
        ptr->RULECTRLINFO.device_id,
        ptr->RULECTRLINFO.pcl_id,
        ptr->RULECTRLINFO.dev_rule_id);

    mref_handle_p = L_MM_AllocateTxBuffer(sizeof(RULE_CTRL_IscReplyBuf_T),
                                          L_MM_USER_ID2(SYS_MODULE_RULE_CTRL, RULE_CTRL_DEVRM_GETCOUNTER));

    isc_reply_p = (RULE_CTRL_IscReplyBuf_T*)L_MM_Mref_GetPdu(mref_handle_p, &pdu_len);
    if (isc_reply_p == NULL)
    {
        SYSFUN_Debug_Printf("\r\n%s():L_MM_Mref_GetPdu() fails", __FUNCTION__);
        return FALSE;
    }

    isc_reply_p->return_value.b_result = RULE_CTRL_LocalRealGetCounter(ptr->RULECTRLINFO.device_id,
                                                                       ptr->RULECTRLINFO.pcl_id,
                                                                       ptr->RULECTRLINFO.dev_rule_id,
                                                                       &isc_reply_p->data.acl_counter.green_cnt,
                                                                       &isc_reply_p->data.acl_counter.yellow_cnt,
                                                                       &isc_reply_p->data.acl_counter.red_cnt);

    if (FALSE == isc_reply_p->return_value.b_result)
    {
        isc_reply_p->return_value.b_result = FALSE;
        SYSFUN_Debug_Printf("\r\b%s():RULE_CTRL_Slave_Get_Couner fails", __FUNCTION__);
        LOG("RULE_CTRL_Slave_Get_Couner fails");
    }

    LOG("green: %lu, yellow: %lu, red: %lu, total: %lu\r\n",
        isc_reply_p->data.acl_counter.green_cnt,
        isc_reply_p->data.acl_counter.yellow_cnt,
        isc_reply_p->data.acl_counter.red_cnt,
        isc_reply_p->data.acl_counter.green_cnt + isc_reply_p->data.acl_counter.yellow_cnt + isc_reply_p->data.acl_counter.red_cnt);


    return ISC_RemoteReply(mref_handle_p, key);
}

static BOOL_T RULE_CTRL_Slave_Clear_Counter(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr)
{
    BOOL_T status = TRUE;

    LOG("Clear_Counter\r\n"
        "  device_id: %lu, pcl_id: %lu, dev_rule_id: %lu\r\n",
        ptr->RULECTRLINFO.device_id,
        ptr->RULECTRLINFO.pcl_id,
        ptr->RULECTRLINFO.dev_rule_id);

    if (FALSE == RULE_CTRL_LocalRealClearCounter(ptr->RULECTRLINFO.device_id,
                                                 ptr->RULECTRLINFO.pcl_id,
                                                 ptr->RULECTRLINFO.dev_rule_id))
    {
        status = FALSE;
        SYSFUN_Debug_Printf("\r\b%s():RULE_CTRL_Slave_Clear_Counter fails", __FUNCTION__);
        LOG("RULE_CTRL_Slave_Clear_Counter fails");
    }

    return status;
}

static BOOL_T
RULE_CTRL_Slave_Get_Dev_Info(
    ISC_Key_T *key,
    RULE_CTRL_IscBuf_T *ptr)
{
    UI32_T                   pdu_len;
    L_MM_Mref_Handle_T      *mref_handle_p;
    RULE_CTRL_IscReplyBuf_T *isc_reply_p;

    mref_handle_p = L_MM_AllocateTxBuffer(sizeof(RULE_CTRL_IscReplyBuf_T),
                                          L_MM_USER_ID2(SYS_MODULE_RULE_CTRL, RULE_CTRL_DEVRM_GETDEVINFO));

    isc_reply_p = (RULE_CTRL_IscReplyBuf_T*)L_MM_Mref_GetPdu(mref_handle_p, &pdu_len);
    if (isc_reply_p == NULL)
    {
        SYSFUN_Debug_Printf("\r\n%s():L_MM_Mref_GetPdu() fails", __FUNCTION__);
        return FALSE;
    }

    isc_reply_p->return_value.b_result = RULE_CTRL_LocalRealGetDevInfo(&isc_reply_p->data.device_info);

    if (FALSE == isc_reply_p->return_value.b_result)
    {
        SYSFUN_Debug_Printf("\r\b%s():RULE_CTRL_Slave_Get_Couner fails", __FUNCTION__);
    }

    return ISC_RemoteReply(mref_handle_p, key);
}

static BOOL_T
RULE_CTRL_Slave_Bind_Pcl_To_Port(
    ISC_Key_T *key,
    RULE_CTRL_IscBuf_T *ptr)
{
    BOOL_T status = TRUE;

    LOG("\r\n  device_id: %lu, pcl_id: %lu, phy_port: %lu\r\n",
        ptr->RULECTRLINFO.device_id,
        ptr->RULECTRLINFO.pcl_id,
        ptr->RULECTRLINFO.phy_port);

    if (FALSE == RULE_CTRL_LocalRealBindPclToPort(ptr->RULECTRLINFO.device_id,
                                                  ptr->RULECTRLINFO.phy_port,
                                                  ptr->RULECTRLINFO.pcl_id))
    {
        status = FALSE;
        SYSFUN_Debug_Printf("\r\b%s():RULE_CTRL_Slave_Bind_Pcl_To_Port fails", __FUNCTION__);
        LOG("fails");
    }

    return status;
}
static BOOL_T
RULE_CTRL_Slave_Unbind_Pcl_To_Port(
    ISC_Key_T *key,
    RULE_CTRL_IscBuf_T *ptr)
{
    BOOL_T status = TRUE;

    LOG("\r\n  device_id: %lu, pcl_id: %lu, phy_port: %lu\r\n",
        ptr->RULECTRLINFO.device_id,
        ptr->RULECTRLINFO.pcl_id,
        ptr->RULECTRLINFO.phy_port);

    if (FALSE == RULE_CTRL_LocalRealUnbindPclToPort(ptr->RULECTRLINFO.device_id,
                                                    ptr->RULECTRLINFO.phy_port,
                                                    ptr->RULECTRLINFO.pcl_id))
    {
        status = FALSE;
        SYSFUN_Debug_Printf("\r\b%s():RULE_CTRL_Slave_Unbind_Pcl_To_Port fails", __FUNCTION__);
        LOG("fails");
    }

    return status;
}

static BOOL_T
RULE_CTRL_Slave_Bind_Pcl_To_TrunkPort(
    ISC_Key_T *key,
    RULE_CTRL_IscBuf_T *ptr)
{
    BOOL_T status = TRUE;

    LOG("\r\n  device_id: %lu, pcl_id: %lu, trunk_id: %lu\r\n",
        ptr->RULECTRLINFO.device_id,
        ptr->RULECTRLINFO.pcl_id,
        ptr->RULECTRLINFO.trunk_id);

    if (FALSE == RULE_CTRL_LocalRealBindPclToTrunkPort(ptr->RULECTRLINFO.device_id,
                                                       ptr->RULECTRLINFO.trunk_id,
                                                       ptr->RULECTRLINFO.pcl_id))
    {
        status = FALSE;
        SYSFUN_Debug_Printf("\r\b%s():RULE_CTRL_Slave_Bind_Pcl_To_TrunkPort fails", __FUNCTION__);
        LOG("fails");
    }

    return status;
}

static BOOL_T
RULE_CTRL_Slave_Unbind_Pcl_To_TrunkPort(
    ISC_Key_T *key,
    RULE_CTRL_IscBuf_T *ptr)
{
    BOOL_T status = TRUE;

    LOG("\r\n  device_id: %lu, pcl_id: %lu, trunk_id: %lu\r\n",
        ptr->RULECTRLINFO.device_id,
        ptr->RULECTRLINFO.pcl_id,
        ptr->RULECTRLINFO.trunk_id);

    if (FALSE == RULE_CTRL_LocalRealUnbindPclToTrunkPort(ptr->RULECTRLINFO.device_id,
                                                         ptr->RULECTRLINFO.trunk_id,
                                                         ptr->RULECTRLINFO.pcl_id))
    {
        status = FALSE;
        SYSFUN_Debug_Printf("\r\b%s():RULE_CTRL_Slave_Unbind_Pcl_To_TrunkPort fails", __FUNCTION__);
        LOG("fails");
    }

    return status;
}

#endif /* #if (SYS_CPNT_STACKING == TRUE) */

