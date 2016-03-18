/*---------------------------------------------------------------------
 * File_Name : COS_VM.c
 *
 * Purpose   : Exported APIs of COS CSC
 *
 * 2002/2/9    : JJ Young Create
 * 2002/9/16   : Jeff Kao Add Enter Transition Mode
 *
 * Copyright(C)      Accton Corporation, 2002, 2003
 *
 * Note    : Designed for Mercury
 *---------------------------------------------------------------------
 */

/* INCLUDE FILE DECLARATIONS
 */
#include "stdio.h"
#include "string.h"
#include "stdlib.h"


#include "sys_type.h"
#include "sys_adpt.h"
#include "sys_cpnt.h"
#include "sys_module.h"
#include "sysfun.h"
#include "sys_dflt.h"
#include "swctrl.h"
#include "swdrvl4.h"
#include "cos_vm.h"
#include "cos_om.h"

#if((SYS_CPNT_ACL == TRUE) && (SYS_CPNT_COS_CONTAIN_CLI_MAP_ACCESS_LIST == TRUE))
#include "acl_type.h"
#include "acl_vm.h"
#endif

#include "syslog_mgr.h"
#include "syslog_pmgr.h"
#include "syslog_type.h"
#include "hrdrv.h"
#include "cos_type.h"
#include "sys_bld.h"
#include "l_threadgrp.h"
#include "backdoor_mgr.h"
#include "l_link_lst.h"
#include "l_rstatus.h"
#include "rule_mgr.h"
#include "l_sort_lst.h"
#include "l_linklist.h"
#include "rule_ctrl.h"
#include "stktplg_pom.h"
#include "sys_callback_mgr.h"

/* FIXME: not using port-list to set Dscp2Queue,
 *        add using per-port and global local funciton
 */

/* NAMING CONSTANT DECLARATIONS
 */
#ifndef ASSERT
#define ASSERT(input)
#endif

#define COS_VM_BACKDOOR FALSE

#define MAX_TCP_PORT_MAP_ENTRIES            8
#define ALL_PORT                            65535 /* For global configuration (all l_port) */
#define BACKUP                              0
#define RESTORE                             1

#define TOTAL_NBR_OF_UPORT                  (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *       \
                                            SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT)
#define TOTAL_NBR_OF_LPORT                  (TOTAL_NBR_OF_UPORT  +       \
                                            SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM)

/* the following naming constants are used to identify an un-modified fields */
#define COS_VM_UNDEF_TCP_PORT               0x0
#define COS_VM_UNDEF_COS_VALUE              0xff

#define COS_VM_MAX_PRIORITY_MODE_NUMBER     3
#define COS_VM_DFLT_USER_PRIORITY_INDEX     0

#define COS_VM_LPORT_OF_GLOBAL              (SYS_ADPT_TOTAL_NBR_OF_LPORT + 1)

/* MACRO FUNCTION DECLARATIONS
 */

#define COS_VM_CheckAsicConfig()            if (!asic_config) {                                \
                                                return FALSE; }

#define COS_VM_RunningCheckLportExisting() if (!SWCTRL_LogicalPortExisting(l_port)) {         \
                                                return SYS_TYPE_GET_RUNNING_CFG_FAIL; }

#define COS_VM_CheckLportExisting()        if (!SWCTRL_LogicalPortExisting(l_port)) {         \
                                                return FALSE; }

#define COS_VM_IS_BAD_LPORT_NO(lport)               ((0 >= lport) || (lport > TOTAL_NBR_OF_LPORT))
#define COS_VM_IS_BAD_MAPPING_IDX(idx)              ((0 >= idx) || (idx > MAX_TCP_PORT_MAP_ENTRIES))
#define COS_VM_IS_BAD_TCP_PORT_MAP_IDX(idx)         (idx >= MAX_TCP_PORT_MAP_ENTRIES)

#define COS_VM_USE_CSC_WITHOUT_RETURN_VALUE()
#define COS_VM_USE_CSC(a)
#define COS_VM_RELEASE_CSC()
#define COS_VM_IFINDEX_TO_UNIT(ifindex)             ( ((UI32_T)(((ifindex)-1)/(SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT)))+1 )

/* set the point of user config for {table_name} */
#define COS_VM_SET_PORT_TABLE_USER_CONFIG_POINT(l_port, table_name)                                \
            cos_vm_port_entry[l_port-1].table_name.active_p                                        \
                = &cos_vm_port_entry[l_port-1].config[COS_VM_DFLT_USER_PRIORITY_INDEX].table_name; \
            cos_vm_port_entry[l_port-1].table_name.active_owner_p                                  \
                = &cos_vm_port_entry[l_port-1].config[COS_VM_DFLT_USER_PRIORITY_INDEX];            \
            cos_vm_port_entry[l_port-1].table_name.config_p[COS_VM_DFLT_USER_PRIORITY_INDEX]       \
                = &cos_vm_port_entry[l_port-1].config[COS_VM_DFLT_USER_PRIORITY_INDEX].table_name

#define COS_VM_IS_VALID_COS(_cos)     (MIN_COS_VAL <= _cos && _cos <= MAX_COS_VAL)
#define COS_VM_IS_VALID_CFI(_cfi)     (MIN_CFI_VAL <= _cfi && _cfi <= MAX_CFI_VAL)
#define COS_VM_IS_VALID_DSCP(_dscp)   (MIN_DSCP_VAL <= _dscp && _dscp <= MAX_DSCP_VAL)
#define COS_VM_IS_VALID_IP_PRE(_pre)  (MIN_PRE_VAL <= _pre && _pre <= MAX_PRE_VAL)
#define COS_VM_IS_VALID_PHB(_phb)     (MIN_PHB_VAL <= _phb && _phb <= MAX_PHB_VAL)
#define COS_VM_IS_VALID_COLOR(_color) (MIN_COLOR_VAL <= _color && _color <= MAX_COLOR_VAL && _color != 2)
#define COS_VM_IS_VALID_QUEUE(_queue) (MIN_QUEUE_VAL <= _queue && _queue <= MAX_QUEUE_VAL)

#define COS_VM_IS_DEFAULT_COS_TO_DSCP_MAP(_cos, _cfi, _phb, _color) \
    (_phb == DEFAULT_COS_TO_DSCP_MAPPING[_cos][_cfi].phb && _color == DEFAULT_COS_TO_DSCP_MAPPING[_cos][_cfi].color)

#define COS_VM_IS_DEFAULT_PRE_TO_DSCP_MAP(_pre, _phb, _color) \
    (_phb == DEFAULT_PRE_TO_DSCP_MAPPING[_pre].phb && _color == DEFAULT_PRE_TO_DSCP_MAPPING[_pre].color)

#define COS_VM_IS_DEFAULT_DSCP_TO_DSCP_MAP(_dscp, _phb, _color) \
    (_phb == DEFAULT_DSCP_TO_DSCP_MAPPING[_dscp].phb && _color == DEFAULT_DSCP_TO_DSCP_MAPPING[_dscp].color)

#define COS_VM_IS_DEFAULT_PHB_TO_QUEUE_MAP(_phb, _queue) \
    (_queue == DEFAULT_DSCP_TO_QUEUE_MAPPING[_phb])


/* To find the the "enabled" and "highest priority" config of {table_name}
 * and set the config "active"
 */
#define COS_VM_RECUR_PORT_PRIORITY_CONFIG(l_port, table_name)                                     \
            {                                                                                     \
                int i;                                                                            \
                int sort_priority[COS_VM_MAX_PRIORITY_MODE_NUMBER] = {0};                         \
                UI16_T index = 0;                                                                 \
                for (i = 0; i < COS_VM_MAX_PRIORITY_MODE_NUMBER; ++i)                             \
                {                                                                                 \
                    if (TRUE == cos_vm_port_entry[l_port-1].config[i].enable &&                   \
                        TRUE == cos_vm_port_entry[l_port-1].config[i].table_name.common.modified) \
                    {                                                                             \
                        sort_priority[i] = (int) cos_vm_port_entry[l_port-1].config[i].priority;  \
                    }                                                                             \
                    else                                                                          \
                    {                                                                             \
                        sort_priority[i] = -1;                                                    \
                        continue;                                                                 \
                    }                                                                             \
                }                                                                                 \
                qsort(sort_priority, COS_VM_MAX_PRIORITY_MODE_NUMBER,                             \
                      sizeof(sort_priority[0]), (COMP_FUNC)compare_priority);                     \
                if (-1 == sort_priority[0])                                                       \
                {                                                                                 \
                    sort_priority[0] = COS_VM_PRIORITY_USER;                                      \
                }                                                                                 \
                if (COS_TYPE_E_NONE != COS_VM_PortPriorityToId(l_port, sort_priority[0], &index)) \
                {                                                                                 \
                    return COS_TYPE_E_UNKNOWN;                                                    \
                }                                                                                 \
                cos_vm_port_entry[l_port-1].table_name.active_owner_p                             \
                    = &cos_vm_port_entry[l_port-1].config[index];                                 \
                cos_vm_port_entry[l_port-1].table_name.active_p                                   \
                    = &cos_vm_port_entry[l_port-1].config[index].table_name;                      \
            }

#ifdef _MSC_VER
#define COS_VM_DEBUG_PRINTF(fmt, ...)                                       \
    if (TRUE == COS_VM_GetDebugMode())                                      \
    {                                                                       \
        printf("[%s:%d] ", __FUNCTION__, __LINE__);                         \
        printf(fmt, __VA_ARGS__);                                           \
        printf("\r\n");                                                     \
    }
#else
#define COS_VM_DEBUG_PRINTF(fmt, args...)                                  \
    if (TRUE == COS_VM_GetDebugMode())                                     \
    {                                                                      \
        printf("[%s:%d] ", __FUNCTION__, __LINE__);                        \
        printf(fmt, ##args);                                               \
        printf("\r\n");                                                    \
    }
#endif /* COS_VM_DEBUG_PRINTF */

/* Todo: make error code to string */
#define COS_VM_DEBUG_ERROR_MSG(error_code)   \
    COS_VM_DEBUG_PRINTF("Error code :%d", error_code)

/* DATA TYPE DECLARATIONS
 */
typedef struct
{
    UI8_T tos;
    UI32_T cos;
}COS_VM_LocalTosToCOSEntry_T;

typedef struct
{
    UI8_T dscp;
    UI32_T cos;
    BOOL_T status;
}COS_VM_LocalDscpToCOSEntry_T;

typedef struct
{
    UI16_T port;    /*tcp/udp port*/
    UI32_T cos;

}COS_VM_LocalPortToCOSEntry_T;

typedef struct COS_VM_PortConfigStatus_S
{
    UI8_T       config_count; /* how many TCP ports map to cos-value */
    BOOL_T      mapping_index_usage_status[MAX_TCP_PORT_MAP_ENTRIES]; /* FALSE implies not used yet */
} COS_VM_PortConfigStatus_T;

typedef struct
{
    BOOL_T modified;
} COS_VM_TABLE_COMMON_T;

typedef struct
{
  COS_VM_TABLE_COMMON_T common;
  qos_if_trust_mode_t mode;
} COS_VM_TRUST_MODE_CONFIG_T;

typedef struct
{
   COS_VM_TABLE_COMMON_T common;
   per_port_cos_dscp_t table;
} COS_VM_COS_TO_DSCP_CONFIG_T;

typedef struct
{
   COS_VM_TABLE_COMMON_T common;
   per_port_pri_queue_t  table;
} COS_VM_DSCP_TO_QUEUE_CONFIG_T;

typedef struct
{
   COS_VM_TABLE_COMMON_T common;
   per_port_pri_color_t  table;
} COS_VM_DSCP_TO_COLOR_CONFIG_T;

typedef struct
{
   COS_VM_TABLE_COMMON_T common;
   per_port_dscp_dscp_t  table;
} COS_VM_DSCP_TO_DSCP_CONFIG_T;

typedef struct
{
   COS_VM_TABLE_COMMON_T common;
   per_port_pre_dscp_t  table;
} COS_VM_PRE_TO_DSCP_CONFIG_T;

typedef struct
{
   COS_VM_TABLE_COMMON_T common;
   per_port_dscp_cos_t  table;
} COS_VM_DSCP_TO_COS_CONFIG_T;

typedef struct
{
    COS_VM_TABLE_COMMON_T common;
    UI32_T mode;
} COS_VM_QUEUE_MODE_CONFIG_T;

typedef struct
{
    COS_VM_TABLE_COMMON_T common;
    UI32_T wrr_weight[SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE];
    BOOL_T is_strict[SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE];
} COS_VM_QUEUE_WEIGHT_CONFIG_T;

typedef struct
{
    UI16_T priority;
    BOOL_T enable;

    COS_VM_TRUST_MODE_CONFIG_T    trust_mode;
    COS_VM_COS_TO_DSCP_CONFIG_T   cos_to_dscp;
    COS_VM_PRE_TO_DSCP_CONFIG_T   pre_to_dscp;
    COS_VM_DSCP_TO_COS_CONFIG_T   dscp_to_cos;
    COS_VM_DSCP_TO_DSCP_CONFIG_T  dscp_to_dscp;
    COS_VM_DSCP_TO_QUEUE_CONFIG_T dscp_to_queue;
    COS_VM_DSCP_TO_COLOR_CONFIG_T dscp_to_color;
    COS_VM_QUEUE_MODE_CONFIG_T    queue_mode;
    COS_VM_QUEUE_WEIGHT_CONFIG_T  queue_weight;
} COS_VM_CosPortConfig_T;

typedef struct
{
  COS_VM_CosPortConfig_T        *active_owner_p;
  COS_VM_COS_TO_DSCP_CONFIG_T   *active_p;
  COS_VM_COS_TO_DSCP_CONFIG_T   *config_p[COS_VM_MAX_PRIORITY_MODE_NUMBER];
} COS_VM_COS_TO_DSCP_T;

typedef struct
{
    COS_VM_CosPortConfig_T     *active_owner_p;
    COS_VM_TRUST_MODE_CONFIG_T *active_p;
    COS_VM_TRUST_MODE_CONFIG_T *config_p[COS_VM_MAX_PRIORITY_MODE_NUMBER];
} COS_VM_TRUST_MODE_T;

typedef struct
{
  COS_VM_CosPortConfig_T       *active_owner_p;
  COS_VM_QUEUE_WEIGHT_CONFIG_T *active_p;
  COS_VM_QUEUE_WEIGHT_CONFIG_T *config_p[COS_VM_MAX_PRIORITY_MODE_NUMBER];
} COS_VM_QUEUE_WEIGHT_T;

typedef struct
{
  COS_VM_CosPortConfig_T *active_owner_p;
  COS_VM_DSCP_TO_QUEUE_CONFIG_T *active_p;
  COS_VM_DSCP_TO_QUEUE_CONFIG_T *config_p[COS_VM_MAX_PRIORITY_MODE_NUMBER];
} COS_VM_DSCP_TO_QUEUE_T;

typedef struct
{
  COS_VM_CosPortConfig_T *active_owner_p;
  COS_VM_DSCP_TO_COLOR_CONFIG_T *active_p;
  COS_VM_DSCP_TO_COLOR_CONFIG_T *config_p[COS_VM_MAX_PRIORITY_MODE_NUMBER];
} COS_VM_DSCP_TO_COLOR_T;

typedef struct
{
  COS_VM_CosPortConfig_T *active_owner_p;
  COS_VM_DSCP_TO_DSCP_CONFIG_T *active_p;
  COS_VM_DSCP_TO_DSCP_CONFIG_T *config_p[COS_VM_MAX_PRIORITY_MODE_NUMBER];
} COS_VM_DSCP_TO_DSCP_T;

typedef struct
{
  COS_VM_CosPortConfig_T *active_owner_p;
  COS_VM_PRE_TO_DSCP_CONFIG_T *active_p;
  COS_VM_PRE_TO_DSCP_CONFIG_T *config_p[COS_VM_MAX_PRIORITY_MODE_NUMBER];
} COS_VM_PRE_TO_DSCP_T;

typedef struct
{
  COS_VM_CosPortConfig_T *active_owner_p;
  COS_VM_DSCP_TO_COS_CONFIG_T *active_p;
  COS_VM_DSCP_TO_COS_CONFIG_T *config_p[COS_VM_MAX_PRIORITY_MODE_NUMBER];
} COS_VM_DSCP_TO_COS_T;

typedef struct
{
  COS_VM_CosPortConfig_T *active_owner_p;
  COS_VM_QUEUE_MODE_CONFIG_T *active_p;
  COS_VM_QUEUE_MODE_CONFIG_T *config_p[COS_VM_MAX_PRIORITY_MODE_NUMBER];
} COS_VM_QUEUE_MODE_T;

typedef struct COS_VM_PortEntry_S
{
    COS_VM_CosPortConfig_T config[COS_VM_MAX_PRIORITY_MODE_NUMBER];
    COS_VM_TRUST_MODE_T    trust_mode;
    COS_VM_COS_TO_DSCP_T   cos_to_dscp;
    COS_VM_PRE_TO_DSCP_T   pre_to_dscp;
    COS_VM_DSCP_TO_COS_T   dscp_to_cos;
    COS_VM_DSCP_TO_QUEUE_T dscp_to_queue;
    COS_VM_DSCP_TO_COLOR_T dscp_to_color;
    COS_VM_DSCP_TO_DSCP_T  dscp_to_dscp;
    COS_VM_QUEUE_MODE_T    queue_mode;
    COS_VM_QUEUE_WEIGHT_T  queue_weight;
} COS_VM_PortEntry_T;

/************************** TYPE Define *****************************/
typedef int (*COMP_FUNC) (const void * ,  const void *);
/********************************************************************/


/* LOCAL SUBPROGRAM DECLARATIONS
 */

#if(COS_VM_BACKDOOR == TRUE)
static  void COS_VM_BackDoor_Menu(void);
#endif

/*************** Config Core Layer Setting to driver ****************/
#if (SYS_CPNT_COS_PER_PORT == FALSE)
    #if defined(STRATA_SWITCH) || defined(XGS_SWITCH)
    #else
        static BOOL_T ConfigAsicTosMapTable(void);
        static BOOL_T ConfigAsicDscpMapTable(void);
        static BOOL_T ConfigAsicTcpPortMapTable(void);
    #endif
#endif

#if (SYS_CPNT_COS_PER_PORT == TRUE)
static BOOL_T ConfigAsicTosDefaultMap();
#endif
#ifndef XGS_SWITCH
static BOOL_T ConfigAsicDscpDefaultMap();
#endif

static BOOL_T ConfigAsicLportTosMapTable(UI32_T l_port);
static BOOL_T ConfigAsicLportDscpMapTable(UI32_T l_port);
static BOOL_T ConfigAsicLportTcpPortMapTable(UI32_T l_port);
/********************************************************************/
/******************* For initial default setting ********************/
static void SetAllTosMapTableToDefault();
static void SetAllDscpMapTableToDefault();
static void SetAllTcpPortMapTableToDefault();
#if 0
static void SetHotInsertTosMapTableToDefault(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default);
static void SetHotInsertDscpMapTableToDefault(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default);
static void SetHotInsertTcpPortMapTableToDefault(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default);
static void SetHotInsertTosMapTable(UI32_T starting_port_ifindex, UI32_T number_of_port);
static void SetHotInsertDscpMapTable(UI32_T starting_port_ifindex, UI32_T number_of_port);
static void SetHotInsertTcpPortMapTable(UI32_T starting_port_ifindex, UI32_T number_of_port);
static void SetHotRemoveTosMapTableToDefault(UI32_T starting_port_ifindex, UI32_T number_of_port);
static void SetHotRemoveDscpMapTableToDefault(UI32_T starting_port_ifindex, UI32_T number_of_port);
static void SetHotRemoveTcpPortMapTableToDefault(UI32_T starting_port_ifindex, UI32_T number_of_port);

/********************************************************************/
#endif
/******************** For Global system setting *********************/
static void ApplyTosAllEntry(UI8_T tos,UI32_T cos);
static void ApplyDscpAllEntry(UI8_T dscp,UI32_T cos);
/********************************************************************/

/********************* Backup and Restore Lport ************************/
#if (SYS_CPNT_COS_PER_PORT == TRUE)
static void BackupRestoreLportOldTosEntry(UI32_T l_port, UI8_T choice);
static void BackupRestoreLportOldDscpEntry(UI32_T l_port, UI8_T choice);
static void BackupRestoreLportOldTcpPortEntry(UI32_T l_port, UI8_T choice);
#endif
/********************************************************************/

/********************* For Copy Port setting ************************/
static BOOL_T CopyTosMapTable(UI32_T src_port, UI32_T dst_port);
static BOOL_T CopyDscpMapTable(UI32_T src_port, UI32_T dst_port);
static BOOL_T CopyTcpPortMapTable(UI32_T src_port, UI32_T dst_port);
static BOOL_T CosPortTrunkSync(UI32_T src_port, UI32_T dst_port);
static BOOL_T CosTrunkPortSync(UI32_T src_port, UI32_T dst_port);


#if (SYS_CPNT_COS_PER_PORT == TRUE)
static void CopyTosFromTrunkPortToMemberPort(UI32_T trunk_ifindex);
static void CopyDscpFromTrunkPortToMemberPort(UI32_T trunk_ifindex);
static void CopyTcpPortFromTrunkPortToMemberPort(UI32_T trunk_ifindex);

static BOOL_T CopyDestPortIsTrunkPort(UI32_T s_port,UI32_T d_port,COS_VM_MappingTable_T choice, BOOL_T status);
#endif
/********************************************************************/

/************** Delete Last Trunk Member Port Function **************/
static void DeleteTrunkPortAllEntry(UI32_T l_port);
/********************************************************************/

/*************** Add Trunk Member Port Functions ********************/
#if (SYS_CPNT_COS_PER_PORT == TRUE)
static void   MemberPortDeleteOldAsicSetting(UI32_T trunk_ifindex, UI32_T member_ifindex);
static BOOL_T MemberPortAddNewAsicSetting(UI32_T trunk_ifindex, UI32_T member_ifindex, UI8_T choice);
static void   MemberPortStoreBackupSetting(UI32_T member_ifindex, COS_VM_MappingTable_T choice);
#endif
/********************************************************************/

/**** Deletet ifindex Old Entry when join into trunk or new port ****/
#if (SYS_CPNT_COS_PER_PORT == TRUE)
static BOOL_T DeleteTosOldAsicSetting(UI32_T source_ifindex, UI32_T dest_ifindex);
static BOOL_T DeleteDscpOldAsicSetting(UI32_T source_ifindex, UI32_T dest_ifindex);
static BOOL_T DeleteTcpportOldAsicSetting(UI32_T source_ifindex, UI32_T dest_ifindex);
static BOOL_T SearchLportTcpPortEntry(UI32_T s_port, UI16_T tcp_port, UI32_T cos);
#endif
/********************************************************************/

/*************************** Local Function *************************/
static int compare_tcp_port(TCP_Port_Map_T *p1, TCP_Port_Map_T *p2);
#if (SYS_CPNT_COS_PER_PORT == TRUE)
static BOOL_T IsLportSelected(UI32_T l_port, UI8_T port_list[]);
#endif
#if((SYS_CPNT_ACL == TRUE) && (SYS_CPNT_COS_CONTAIN_CLI_MAP_ACCESS_LIST == TRUE))
static BOOL_T COS_VM_AddAceCallback(UI32_T rid, UI32_T acl_type, UI8_T acl_name[], ACL_Ace_Entry_T *ace);
static BOOL_T COS_VM_DelAceCallback(UI32_T acl_type, UI8_T acl_name[], ACL_Ace_Entry_T *ace, BOOL_T is_last);
static BOOL_T COS_VM_DelAclCallback(UI8_T acl_name[]);
static BOOL_T COS_VM_IsAclBindToIngressIntf(UI8_T acl_name[]);
#endif
static BOOL_T COS_VM_AclCosEntryCmp(void* src_pt, void* dst_pt);
#if ((COS_VM_BACKDOOR == TRUE) || (SYS_CPNT_COS_CONTAIN_CLI_MAP_ACCESS_LIST == TRUE))
static void   COS_VM_InitDefaultEntry(COS_TYPE_AclCosEntry_T *cos_entry);
#endif

static BOOL_T ConfigAsicAllTosMap();
static BOOL_T ConfigAsicAllDscpMap();
static BOOL_T COS_VM_SetAllPortMapToASIC(BOOL_T is_add);

static TCP_Port_Map_T* COS_VM_CreateTcpPortSettingToOm(UI32_T lport, UI16_T tcp_port, UI8_T cos);
static void COS_VM_DeleteTcpPortSettingFromOm(UI32_T lport, UI32_T tcp_port_map_index);

static BOOL_T COS_VM_AllocateTcpPortMappingIndex(UI32_T lport, UI32_T *mapping_index);
static void COS_VM_FreeTcpPortMappingIndex(UI32_T lport, UI32_T mapping_index);

static COS_VM_PortConfigStatus_T* COS_VM_GetPortConfigStatus(UI32_T lport);

static COS_TYPE_ERROR_CODE_E
COS_VM_Priv_VerifyIfindex(
    UI32_T ifindex
);

static COS_TYPE_ERROR_CODE_E
COS_VM_Priv_LocalSetEgressQueueMode(
    UI32_T ifindex,
    UI16_T priority,
    COS_VM_SCHEDULING_METHOD_T mode
);

static COS_TYPE_ERROR_CODE_E
COS_VM_Priv_LocalSetEgressQueueWeight(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T q_id,
    UI32_T weight
);

static COS_TYPE_ERROR_CODE_E
COS_VM_Priv_SetEgressQueueMode(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T mode
);

static COS_TYPE_ERROR_CODE_E
COS_VM_Priv_SetEgressQueueWeight(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T queue,
    UI32_T weight
);

static COS_TYPE_ERROR_CODE_E
COS_VM_Priv_SetPre2InternalDscpForAll(
    UI32_T l_port, 
    UI16_T priority
);

static COS_TYPE_ERROR_CODE_E
COS_VM_Priv_SetDscp2InternalDscpForAll(
    UI32_T l_port, 
    UI16_T priority
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPosition
 *------------------------------------------------------------------------------
 * PURPOSE: This function transfer interface index to 8 bit port bitmap position
 * INPUT:   index   - interface index
 * OUTPUT:  list_index    -  (index-1) / 8
 *          list_position -  (index-1) % 8
 * RETURN:  None
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
static void
COS_VM_GetPosition(
    UI32_T index,
    UI32_T *list_index,
    UI32_T *list_position
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_PortPriorityToId
 *------------------------------------------------------------------------------
 * PURPOSE: This function uses to get the config table index by priority
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  id_p     - the index of config table, cos_vm_port_entry[lport-1].config[id]
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
static COS_TYPE_ERROR_CODE_E
COS_VM_PortPriorityToId(
    UI32_T l_port,
    UI16_T priority,
    UI16_T *id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetAllPortList
 *------------------------------------------------------------------------------
 * PURPOSE: Get the port list of all port (user port)
 * INPUT:   None
 * OUTPUT:  port_list - A bit map array of the port,
 *                      the array size is SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
static void
COS_VM_GetAllPortList(
    UI8_T *port_list
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetDebugMode
 *------------------------------------------------------------------------------
 * PURPOSE: Get debug mode flag
 * INPUT:   None.
 * OUTPUT:  None.
 * RETURN:  BOOL_T - debug flag status
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
static BOOL_T
COS_VM_GetDebugMode();

/* End LOCAL SUBPROGRAM DECLARATIONS
 */

/* STATIC VARIABLE DECLARATIONS
*/
/******************  database stored in core layer ******************/
/* static UI32_T cos_operation_mode; Jeff Kao mask 2002/10/23 use CSC macro */     /* master, slave, or transition mode */

static BOOL_T cos_vm_debug_flag;

static COS_VM_MAPPING_FLAG_T         tos_or_dscp_mapping;    /* ipprecedence_enabled, dscp_enabled, or both_disabled */
static COS_VM_TCPPORT_MAPPING_FLAG_T tcp_port_mapping;       /* tcpport enabled or disabled */

SYSFUN_DECLARE_CSC                    /* declare variables used for transition mode  */

BOOL_T dscp_map_status_table [TOTAL_NBR_OF_LPORT]
                             [COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES];

TCP_Port_Map_T  tcp_port_map_table [TOTAL_NBR_OF_LPORT]
                                   [MAX_TCP_PORT_MAP_ENTRIES];

static COS_VM_PortConfigStatus_T port_status_table[TOTAL_NBR_OF_LPORT];

/* trunk member indicator */
UI32_T lport_status [TOTAL_NBR_OF_LPORT];

UI8_T  backup_port_tos_map_table [COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES];

UI8_T  backup_port_dscp_map_table [COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES];

TCP_Port_Map_T  backup_tcp_port_map_table [MAX_TCP_PORT_MAP_ENTRIES];

/* backup port entry */
UI8_T  backup_port_count;

static L_LINK_LST_List_T cos_vm_tbl[SYS_ADPT_TOTAL_NBR_OF_LPORT + 1];

static SYS_TYPE_CallBack_T *dscp2cos_callback_list;

struct L_list  *TcpUdpMaplist;

static UI32_T TcpUdpMap_counter[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK] ={0};

/* All CoS config table
 */
static COS_VM_PortEntry_T cos_vm_port_entry[SWITCH_PORT_NUM + 1];

/******************* Default values of tos Map Table ****************/
const static UI8_T DEFAULT_TOS_MAP[COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES] =
{
                                            (UI8_T)SYS_DFLT_IP_PREC_0_MAPPED_COS,
                                            (UI8_T)SYS_DFLT_IP_PREC_1_MAPPED_COS,
                                            (UI8_T)SYS_DFLT_IP_PREC_2_MAPPED_COS,
                                            (UI8_T)SYS_DFLT_IP_PREC_3_MAPPED_COS,
                                            (UI8_T)SYS_DFLT_IP_PREC_4_MAPPED_COS,
                                            (UI8_T)SYS_DFLT_IP_PREC_5_MAPPED_COS,
                                            (UI8_T)SYS_DFLT_IP_PREC_6_MAPPED_COS,
                                            (UI8_T)SYS_DFLT_IP_PREC_7_MAPPED_COS
};

/******************* Default values of DSCP Map Table ***************/
const static UI8_T DEFAULT_DSCP_MAP[COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES]=
{
            (UI8_T)SYS_DFLT_DSCP_0_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_1_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_2_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_3_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_4_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_5_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_6_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_7_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_8_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_9_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_10_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_11_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_12_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_13_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_14_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_15_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_16_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_17_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_18_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_19_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_20_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_21_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_22_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_23_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_24_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_25_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_26_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_27_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_28_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_29_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_30_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_31_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_32_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_33_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_34_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_35_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_36_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_37_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_38_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_39_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_40_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_41_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_42_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_43_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_44_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_45_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_46_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_47_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_48_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_49_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_50_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_51_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_52_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_53_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_54_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_55_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_56_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_57_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_58_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_59_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_60_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_61_MAPPED_COS,
            (UI8_T)SYS_DFLT_DSCP_62_MAPPED_COS,(UI8_T)SYS_DFLT_DSCP_63_MAPPED_COS
};

const static BOOL_T DEFAULT_DSCP_STATUS[COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES]=
{
             SYS_DFLT_DSCP_0_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_1_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_2_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_3_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_4_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_5_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_6_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_7_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_8_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_9_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_10_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_11_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_12_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_13_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_14_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_15_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_16_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_17_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_18_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_19_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_20_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_21_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_22_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_23_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_24_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_25_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_26_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_27_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_28_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_29_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_30_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_31_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_32_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_33_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_34_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_35_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_36_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_37_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_38_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_39_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_40_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_41_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_42_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_43_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_44_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_45_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_46_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_47_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_48_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_49_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_50_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_51_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_52_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_53_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_54_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_55_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_56_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_57_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_58_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_59_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_60_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_61_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_62_MAPPED_COS_ENABLED,
             SYS_DFLT_DSCP_63_MAPPED_COS_ENABLED
};

/******************* Default values of Queue Mode ****************/
const static UI32_T DEFAULT_QUEUE_MODE = COS_VM_SCHEDULING_WEIGHT_ROUND_ROBIN;

/******************* Default values of Queue to default weight Mapping Table ****************/
const static UI32_T DEFAULT_WRR_QUEUE_TO_WEIGHT[SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE] =
{
             (UI32_T)SYS_DFLT_WRR_Q0_WEIGHT,
             (UI32_T)SYS_DFLT_WRR_Q1_WEIGHT,
             (UI32_T)SYS_DFLT_WRR_Q2_WEIGHT,
             (UI32_T)SYS_DFLT_WRR_Q3_WEIGHT,

#if (SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE == 8)
             (UI32_T)SYS_DFLT_WRR_Q4_WEIGHT,
             (UI32_T)SYS_DFLT_WRR_Q5_WEIGHT,
             (UI32_T)SYS_DFLT_WRR_Q6_WEIGHT,
             (UI32_T)SYS_DFLT_WRR_Q7_WEIGHT,
#endif
};

const static UI8_T DEFAULT_QOS_IF_MAPPING_MODE = COS_MAPPING_MODE;

/******************* Default values of Ingress CoS to internal DSCP Mapping Table ****************/
const static   internal_dscp_t DEFAULT_COS_TO_DSCP_MAPPING[INGRESS_COS_TO_DSCP_MAPPING_ENTRY_NMB][INGRESS_COS_TO_DSCP_MAPPING_CFI] =
{
        {{(UI8_T)SYS_DFLT_INGRESS_COS_0_TO_PHB_0, (UI8_T)SYS_DFLT_INGRESS_COS_0_TO_COLOR_0}, {(UI8_T)SYS_DFLT_INGRESS_COS_0_TO_PHB_1, (UI8_T)SYS_DFLT_INGRESS_COS_0_TO_COLOR_1}},
        {{(UI8_T)SYS_DFLT_INGRESS_COS_1_TO_PHB_0, (UI8_T)SYS_DFLT_INGRESS_COS_1_TO_COLOR_0}, {(UI8_T)SYS_DFLT_INGRESS_COS_1_TO_PHB_1, (UI8_T)SYS_DFLT_INGRESS_COS_1_TO_COLOR_1}},
        {{(UI8_T)SYS_DFLT_INGRESS_COS_2_TO_PHB_0, (UI8_T)SYS_DFLT_INGRESS_COS_2_TO_COLOR_0}, {(UI8_T)SYS_DFLT_INGRESS_COS_2_TO_PHB_1, (UI8_T)SYS_DFLT_INGRESS_COS_2_TO_COLOR_1}},
        {{(UI8_T)SYS_DFLT_INGRESS_COS_3_TO_PHB_0, (UI8_T)SYS_DFLT_INGRESS_COS_3_TO_COLOR_0}, {(UI8_T)SYS_DFLT_INGRESS_COS_3_TO_PHB_1, (UI8_T)SYS_DFLT_INGRESS_COS_1_TO_COLOR_1}},
        {{(UI8_T)SYS_DFLT_INGRESS_COS_4_TO_PHB_0, (UI8_T)SYS_DFLT_INGRESS_COS_4_TO_COLOR_0}, {(UI8_T)SYS_DFLT_INGRESS_COS_4_TO_PHB_1, (UI8_T)SYS_DFLT_INGRESS_COS_4_TO_COLOR_1}},
        {{(UI8_T)SYS_DFLT_INGRESS_COS_5_TO_PHB_0, (UI8_T)SYS_DFLT_INGRESS_COS_5_TO_COLOR_0}, {(UI8_T)SYS_DFLT_INGRESS_COS_5_TO_PHB_1, (UI8_T)SYS_DFLT_INGRESS_COS_5_TO_COLOR_1}},
        {{(UI8_T)SYS_DFLT_INGRESS_COS_6_TO_PHB_0, (UI8_T)SYS_DFLT_INGRESS_COS_6_TO_COLOR_0}, {(UI8_T)SYS_DFLT_INGRESS_COS_6_TO_PHB_1, (UI8_T)SYS_DFLT_INGRESS_COS_6_TO_COLOR_1}},
        {{(UI8_T)SYS_DFLT_INGRESS_COS_7_TO_PHB_0, (UI8_T)SYS_DFLT_INGRESS_COS_7_TO_COLOR_0}, {(UI8_T)SYS_DFLT_INGRESS_COS_7_TO_PHB_1, (UI8_T)SYS_DFLT_INGRESS_COS_7_TO_COLOR_1}}

};

/******************* Default values of Ingress IP precedence to internal DSCP Mapping Table ****************/
const static internal_dscp_t DEFAULT_PRE_TO_DSCP_MAPPING[INGRESS_PRE_TO_DSCP_MAPPING_ENTRY_NMB] =
{
            {(UI8_T)SYS_DFLT_INGRESS_PRE_0_TO_PHB, (UI8_T)SYS_DFLT_INGRESS_PRE_0_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_PRE_1_TO_PHB, (UI8_T)SYS_DFLT_INGRESS_PRE_1_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_PRE_2_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_PRE_2_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_PRE_3_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_PRE_3_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_PRE_4_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_PRE_4_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_PRE_5_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_PRE_5_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_PRE_6_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_PRE_6_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_PRE_7_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_PRE_7_TO_COLOR}
};

/******************* Default values of Ingress DSCP to internal DSCP Mapping Table ****************/
const static internal_dscp_t DEFAULT_DSCP_TO_DSCP_MAPPING[INGRESS_DSCP_TO_DSCP_MAPPING_ENTRY_NMB] =
{
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_0_TO_PHB, (UI8_T)SYS_DFLT_INGRESS_DSCP_0_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_1_TO_PHB, (UI8_T)SYS_DFLT_INGRESS_DSCP_1_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_2_TO_PHB, (UI8_T)SYS_DFLT_INGRESS_DSCP_2_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_3_TO_PHB, (UI8_T)SYS_DFLT_INGRESS_DSCP_3_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_4_TO_PHB, (UI8_T)SYS_DFLT_INGRESS_DSCP_4_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_5_TO_PHB, (UI8_T)SYS_DFLT_INGRESS_DSCP_5_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_6_TO_PHB, (UI8_T)SYS_DFLT_INGRESS_DSCP_6_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_7_TO_PHB, (UI8_T)SYS_DFLT_INGRESS_DSCP_7_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_8_TO_PHB, (UI8_T)SYS_DFLT_INGRESS_DSCP_8_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_9_TO_PHB, (UI8_T)SYS_DFLT_INGRESS_DSCP_9_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_10_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_10_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_11_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_11_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_12_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_12_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_13_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_13_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_14_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_14_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_15_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_15_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_16_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_16_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_17_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_17_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_18_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_18_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_19_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_19_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_20_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_20_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_21_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_21_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_22_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_22_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_23_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_23_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_24_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_24_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_25_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_25_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_26_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_26_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_27_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_27_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_28_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_28_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_29_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_29_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_30_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_30_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_31_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_31_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_32_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_32_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_33_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_33_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_34_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_34_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_35_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_35_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_36_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_36_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_37_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_37_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_38_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_38_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_39_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_39_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_40_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_40_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_41_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_41_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_42_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_42_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_43_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_43_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_44_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_44_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_45_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_45_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_46_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_46_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_47_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_47_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_48_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_48_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_49_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_49_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_50_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_50_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_51_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_51_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_52_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_52_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_53_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_53_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_54_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_54_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_55_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_55_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_56_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_56_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_57_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_57_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_58_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_58_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_59_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_59_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_60_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_60_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_61_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_61_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_62_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_62_TO_COLOR},
            {(UI8_T)SYS_DFLT_INGRESS_DSCP_63_TO_PHB,(UI8_T)SYS_DFLT_INGRESS_DSCP_63_TO_COLOR}

};

/******************* Default values of internal DSCP to CoS Mapping Table ****************/
const static outer_cos_t  DEFAULT_DSCP_TO_COS_MAPPING[INGRESS_DSCP_TO_COS_MAPPING_PHB*2][INGRESS_DSCP_TO_COS_MAPPING_COLOR] =
{
            {{(UI8_T)SYS_DFLT_EGRESS_DSCP_0_TO_COS, (UI8_T)SYS_DFLT_EGRESS_DSCP_0_TO_CFI}, {(UI8_T)SYS_DFLT_EGRESS_DSCP_1_TO_COS, (UI8_T)SYS_DFLT_EGRESS_DSCP_1_TO_CFI}, {(UI8_T)SYS_DFLT_EGRESS_DSCP_2_TO_COS, (UI8_T)SYS_DFLT_EGRESS_DSCP_2_TO_CFI}, {(UI8_T)SYS_DFLT_EGRESS_DSCP_3_TO_COS, (UI8_T)SYS_DFLT_EGRESS_DSCP_3_TO_CFI}},
            {{(UI8_T)SYS_DFLT_EGRESS_DSCP_4_TO_COS, (UI8_T)SYS_DFLT_EGRESS_DSCP_4_TO_CFI}, {(UI8_T)SYS_DFLT_EGRESS_DSCP_5_TO_COS, (UI8_T)SYS_DFLT_EGRESS_DSCP_5_TO_CFI}, {(UI8_T)SYS_DFLT_EGRESS_DSCP_6_TO_COS, (UI8_T)SYS_DFLT_EGRESS_DSCP_6_TO_CFI}, {(UI8_T)SYS_DFLT_EGRESS_DSCP_7_TO_COS, (UI8_T)SYS_DFLT_EGRESS_DSCP_7_TO_CFI}},
            {{(UI8_T)SYS_DFLT_EGRESS_DSCP_8_TO_COS, (UI8_T)SYS_DFLT_EGRESS_DSCP_8_TO_CFI}, {(UI8_T)SYS_DFLT_EGRESS_DSCP_9_TO_COS, (UI8_T)SYS_DFLT_EGRESS_DSCP_9_TO_CFI}, {(UI8_T)SYS_DFLT_EGRESS_DSCP_10_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_10_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_11_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_11_TO_CFI}},
            {{(UI8_T)SYS_DFLT_EGRESS_DSCP_12_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_12_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_13_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_13_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_14_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_14_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_15_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_15_TO_CFI}},
            {{(UI8_T)SYS_DFLT_EGRESS_DSCP_16_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_16_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_17_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_17_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_18_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_18_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_19_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_19_TO_CFI}},
            {{(UI8_T)SYS_DFLT_EGRESS_DSCP_20_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_20_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_21_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_21_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_22_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_22_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_23_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_23_TO_CFI}},
            {{(UI8_T)SYS_DFLT_EGRESS_DSCP_24_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_24_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_25_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_25_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_26_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_26_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_27_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_27_TO_CFI}},
            {{(UI8_T)SYS_DFLT_EGRESS_DSCP_28_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_28_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_29_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_29_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_30_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_30_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_31_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_31_TO_CFI}},
            {{(UI8_T)SYS_DFLT_EGRESS_DSCP_32_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_32_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_33_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_33_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_34_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_34_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_35_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_35_TO_CFI}},
            {{(UI8_T)SYS_DFLT_EGRESS_DSCP_36_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_36_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_37_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_37_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_38_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_38_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_39_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_39_TO_CFI}},
            {{(UI8_T)SYS_DFLT_EGRESS_DSCP_40_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_40_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_41_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_41_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_42_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_42_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_43_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_43_TO_CFI}},
            {{(UI8_T)SYS_DFLT_EGRESS_DSCP_44_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_44_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_45_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_45_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_46_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_46_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_47_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_47_TO_CFI}},
            {{(UI8_T)SYS_DFLT_EGRESS_DSCP_48_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_48_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_49_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_49_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_50_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_50_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_51_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_51_TO_CFI}},
            {{(UI8_T)SYS_DFLT_EGRESS_DSCP_52_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_52_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_53_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_53_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_54_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_54_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_55_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_55_TO_CFI}},
            {{(UI8_T)SYS_DFLT_EGRESS_DSCP_56_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_56_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_57_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_57_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_58_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_58_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_59_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_59_TO_CFI}},
            {{(UI8_T)SYS_DFLT_EGRESS_DSCP_60_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_60_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_61_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_61_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_62_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_62_TO_CFI},{(UI8_T)SYS_DFLT_EGRESS_DSCP_63_TO_COS,(UI8_T)SYS_DFLT_EGRESS_DSCP_63_TO_CFI}}
};

/******************* Default values of internal Priority to Queue Mapping Table ****************/
const static UI8_T DEFAULT_DSCP_TO_QUEUE_MAPPING[MAX_PHB_VAL + 1] =
{
            (UI8_T)SYS_DFLT_PRI_0_TO_QUEUE,
            (UI8_T)SYS_DFLT_PRI_1_TO_QUEUE,
            (UI8_T)SYS_DFLT_PRI_2_TO_QUEUE,
            (UI8_T)SYS_DFLT_PRI_3_TO_QUEUE,
            (UI8_T)SYS_DFLT_PRI_4_TO_QUEUE,
            (UI8_T)SYS_DFLT_PRI_5_TO_QUEUE,
            (UI8_T)SYS_DFLT_PRI_6_TO_QUEUE,
            (UI8_T)SYS_DFLT_PRI_7_TO_QUEUE,
};

/******************* Default values of internal Priority to default color  Mapping Table ****************/
const static UI8_T DEFAULT_DSCP_TO_COLOR_MAPPING[MAX_PHB_VAL + 1] =
{
            (UI8_T)SYS_DFLT_PRI_0_TO_COLOR,
            (UI8_T)SYS_DFLT_PRI_1_TO_COLOR,
            (UI8_T)SYS_DFLT_PRI_2_TO_COLOR,
            (UI8_T)SYS_DFLT_PRI_3_TO_COLOR,
            (UI8_T)SYS_DFLT_PRI_4_TO_COLOR,
            (UI8_T)SYS_DFLT_PRI_5_TO_COLOR,
            (UI8_T)SYS_DFLT_PRI_6_TO_COLOR,
            (UI8_T)SYS_DFLT_PRI_7_TO_COLOR,
};

/* EXPORTED SUBPROGRAM BODIES
 */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPosition
 *------------------------------------------------------------------------------
 * PURPOSE: This function transfer interface index to 8 bit port bitmap position
 * INPUT:   index   - interface index
 * OUTPUT:  list_index    -  (index-1) / 8
 *          list_position -  (index-1) % 8
 * RETURN:  None
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
static void
COS_VM_GetPosition(
    UI32_T index,
    UI32_T *list_index,
    UI32_T *list_position)
{
    *list_index =   (UI32_T)(index - 1) / 8;
    *list_position = (UI32_T)(index - 1) % 8;

    return;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - COS_VM_GetPortConfigStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : get port config status object by l_port
 * INPUT    : l_port
 * OUTPUT   : None
 * RETURN   : NULL -- failed
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static COS_VM_PortConfigStatus_T*
COS_VM_GetPortConfigStatus(
    UI32_T lport)
{
    if (COS_VM_IS_BAD_LPORT_NO(lport))
        return NULL;

    return &port_status_table[lport - 1]; /* to zero-based */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_PortPriorityToId
 *------------------------------------------------------------------------------
 * PURPOSE: This function uses to get the config table index by priority
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  id_p     - the index of config table, cos_vm_port_entry[lport-1].config[id]
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
static COS_TYPE_ERROR_CODE_E
COS_VM_PortPriorityToId(
    UI32_T l_port,
    UI16_T priority,
    UI16_T *id_p)
{
    int i;

    if (l_port < 1 ||
        l_port > (sizeof(cos_vm_port_entry)/sizeof(cos_vm_port_entry[0])))
    {
       return COS_TYPE_E_PARAMETER;
    }

    for (i = 0; i < COS_VM_MAX_PRIORITY_MODE_NUMBER; ++i)
    {
        if (TRUE ==  cos_vm_port_entry[l_port-1].config[i].enable)
        {
            if (priority == cos_vm_port_entry[l_port-1].config[i].priority)
            {
                *id_p = i;
                return COS_TYPE_E_NONE;
            }
        }
    }

    return COS_TYPE_E_CONFIG_NOT_FOUND;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetAllPortList
 *------------------------------------------------------------------------------
 * PURPOSE: Get the port list of all port (user port)
 * INPUT:   None
 * OUTPUT:  port_list - A bit map array of the port,
 *                      the array size is SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST
 * RETURN:  None
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
static void
COS_VM_GetAllPortList(
    UI8_T *port_list)
{
    UI32_T ifIndex = 0, byte = 0, shift = 0;

    memset(port_list, 0, SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST * sizeof(UI8_T));

    for(ifIndex = 1; ifIndex <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK * SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT; ifIndex++)
    {
        COS_VM_GetPosition(ifIndex, &byte, &shift);
        port_list[byte] |= ((0x01) << (7 - shift));
    }

    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetDebugMode
 *------------------------------------------------------------------------------
 * PURPOSE: Get debug mode flag
 * INPUT:   None.
 * OUTPUT:  None.
 * RETURN:  BOOL_T - debug flag status
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
static BOOL_T
COS_VM_GetDebugMode()
{
    return cos_vm_debug_flag;
}

/* END EXPORTED SUBPROGRAM BODIES
 */

int  COS_VM_TcpUdpSortCompare(void* inlist_element, void* input_element)
{
    per_port_l4_dscp_t *e1;
    per_port_l4_dscp_t *e2;
    e1 = (per_port_l4_dscp_t *)(inlist_element);
    e2 = (per_port_l4_dscp_t *)(input_element);
    if(e1->ifindex < e2->ifindex)
    {
        return -1;
    }
    else if(e1->ifindex > e2->ifindex)
    {
        return 1;
    }
    else
    {
        if(e1->protocol < e2->protocol)
        {
            return -1;
        }
        else if(e1->protocol > e2->protocol)
        {
            return  1;
        }
        else
        {
            if(e1->dport <e2->dport)
            {
                return -1;
            }
            else if (e1->dport >e2->dport)
            {
                return 1;
            }
        }
    }
    return 0;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_Initiate_System_Resources
 *------------------------------------------------------------------------------
 * PURPOSE:
 * INPUT : None
 * OUTPUT: None
 * RETURN: TRUE/FALSE
 * NOTES :
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_Initiate_System_Resources()
{
    TcpUdpMaplist = L_list_new();
    TcpUdpMaplist->cmp = (I32_T(*)(void *, void *)) COS_VM_TcpUdpSortCompare;
    return TRUE;
}

#if 0
BOOL_T COS_VM_GetElement(UI32_T ifindex,UI32_T protocol,UI32_T dport,UI32_T phb,UI32_T color, per_port_l4_dscp_t *get_data)
{
    per_port_l4_dscp_t   element_data;

    if(L_SORT_LST_Get_1st(&TcpUdpMaplist, &element_data))
    {
        do{
            if((ifindex == element_data.ifindex)&&(protocol == element_data.protocol)
                &&(dport == element_data.dport)&&(phb == element_data.phb)
                &&(color == element_data.color))
            {
                get_data = &element_data;
                return TRUE;
            }
        }while(L_SORT_LST_Get_Next(&TcpUdpMaplist, &element_data));
    }
    return FALSE;
}
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_Create_InterCSC_Relation
 *------------------------------------------------------------------------------
 * PURPOSE: This function initializes all function pointer registration operations.
 * INPUT :  None
 * OUTPUT:  None
 * RETURN:  None
 * NOTES :
 *------------------------------------------------------------------------------
 */
void COS_VM_Create_InterCSC_Relation(void)
{
#if((SYS_CPNT_ACL == TRUE) && (SYS_CPNT_COS_CONTAIN_CLI_MAP_ACCESS_LIST == TRUE))
    ACL_VM_Register_AddAce_CallBack(COS_VM_AddAceCallback);
    ACL_VM_Register_DelAce_CallBack(COS_VM_DelAceCallback);
    ACL_VM_Register_DelAcl_CallBack(COS_VM_DelAclCallback);
    ACL_VM_Register_IsAclBindingToIngressIntf(COS_VM_IsAclBindToIngressIntf);
#endif

#if(COS_VM_BACKDOOR == TRUE)
    BACKDOOR_MGR_Register_SubsysBackdoorFunc_CallBack("cos",SYS_ADPT_L2_L4_PROC_L4GROUP_IPCMSGQ_KEY,COS_VM_BackDoor_Menu);
#endif

}

/*------------------------------------------------------------------------------
 *          FUNCTION DEFINIATIONS
 *------------------------------------------------------------------------------
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME  - SetAllTosMapTableToDefault
 *------------------------------------------------------------------------------
 * PURPOSE: This function will set all tos/COS mapping to default values
 * INPUT : None
 * OUTPUT: None
 * RETURN: None
 * NOTES : This is local function and only will be called by
 *         COS_VM_EnterMasterMode()
 *------------------------------------------------------------------------------
 */
static void SetAllTosMapTableToDefault()
{
    UI32_T  lport, ip_precedence;

    for (lport = 1; TOTAL_NBR_OF_LPORT >= lport; lport++)
    {
        for (ip_precedence = 0; COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES > ip_precedence;
            ip_precedence++)
        {
            if (FALSE == COS_OM_SetIpPrecedence2CosValueByLport(lport, ip_precedence,
                DEFAULT_TOS_MAP[ip_precedence]))
            {
                return;
            }
        }
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - SetAllDscpMapTableToDefault
 *------------------------------------------------------------------------------
 * PURPOSE: This function will set all DSCP/COS mapping to default values
 * INPUT : None
 * OUTPUT: None
 * RETURN: None
 * NOTES : This is local function and only will be called by
 *         COS_VM_EnterMasterMode()
 *------------------------------------------------------------------------------
 */
static void SetAllDscpMapTableToDefault()
{
    UI32_T  i, ip_dscp;

    for (i=0; i < TOTAL_NBR_OF_LPORT; i++)
    {
        for (ip_dscp = 0; COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES > ip_dscp; ip_dscp++)
        {
            if (FALSE == COS_OM_SetIpDscp2CosValueByLport(i + 1, ip_dscp, DEFAULT_DSCP_MAP[ip_dscp]))
            {
                return;
            }
        }

        memcpy(dscp_map_status_table[i], DEFAULT_DSCP_STATUS, COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES);
    }
    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - SetAllTcpPortMapTableToDefault
 *------------------------------------------------------------------------------
 * PURPOSE: This function will set all TCP_PORT/COS mapping to default values
 * INPUT : None
 * OUTPUT: None
 * RETURN: None
 * NOTES : This is local function and only will be called by
 *         COS_VM_EnterMasterMode()
 *------------------------------------------------------------------------------
 */
static void SetAllTcpPortMapTableToDefault()
{
    UI32_T  i, j;

    for (i = 0; TOTAL_NBR_OF_LPORT > i; i++)
    {
        for (j = 0; MAX_TCP_PORT_MAP_ENTRIES > j; ++j)
        {
            tcp_port_map_table[i][j].tcp_port = COS_VM_UNDEF_TCP_PORT;
            tcp_port_map_table[i][j].cos = COS_VM_UNDEF_COS_VALUE;
            tcp_port_map_table[i][j].mapping_index = 0;
        }
    }

    memset(port_status_table, 0, sizeof(port_status_table));
}

static void SetAllUserPortListToDefaultState(UI32_T start_ifindex, UI32_T end_ifindex,BOOL_T w_chip)
{
#if (SYS_CPNT_COS_INTER_DSCP == TRUE)
    UI32_T ifindex,i,j;
    UI32_T byte, shift;
    UI8_T  ports_default[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];
    UI8_T  all_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];

    memset(ports_default,0,sizeof(ports_default));

    COS_VM_GetAllPortList(all_port_list);

    for(ifindex =  start_ifindex; ifindex <= end_ifindex; ifindex++)
    {
#define PORT_USER_CONFIG cos_vm_port_entry[ifindex-1].config[COS_VM_DFLT_USER_PRIORITY_INDEX]

        memset(&cos_vm_port_entry[ifindex-1], 0, sizeof(cos_vm_port_entry[ifindex-1]));

        if (ifindex != COS_VM_LPORT_OF_GLOBAL)
        {
            if (SWCTRL_LogicalPortExisting (ifindex))
            {
                COS_VM_GetPosition(ifindex, &byte, &shift);
                ports_default[byte] |= ((0x01) << (7 - shift));
            }
        }

        PORT_USER_CONFIG.priority = COS_VM_PRIORITY_USER;

        for(i = MIN_COS_VAL; i <= MAX_COS_VAL; i++)
        {
            for(j = 0; j < 2; j++)
            {
                PORT_USER_CONFIG.cos_to_dscp.table.CURRENT_COS_TO_DSCP_MAPPING[i][j].phb
                    = DEFAULT_COS_TO_DSCP_MAPPING[i][j].phb;
                PORT_USER_CONFIG.cos_to_dscp.table.CURRENT_COS_TO_DSCP_MAPPING[i][j].color
                    = DEFAULT_COS_TO_DSCP_MAPPING[i][j].color;
            }
        }

        for(i = 0; i < SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE; ++i)
        {
            PORT_USER_CONFIG.queue_weight.is_strict[i] = FALSE;
            PORT_USER_CONFIG.queue_weight.wrr_weight[i] = DEFAULT_WRR_QUEUE_TO_WEIGHT[i];
            PORT_USER_CONFIG.queue_mode.mode = DEFAULT_QUEUE_MODE;
        }

        for(i = MIN_PHB_VAL; i <= (MAX_PHB_VAL*2+1); i++)
        {
            for(j = 0; j < 4; j++)
            {
                PORT_USER_CONFIG.dscp_to_cos.table.CURRENT_DSCP_TO_COS_MAPPING[i/2][j].cos
                    = DEFAULT_DSCP_TO_COS_MAPPING[i][j].cos;
                PORT_USER_CONFIG.dscp_to_cos.table.CURRENT_DSCP_TO_COS_MAPPING[i/2][j].cfi
                    = DEFAULT_DSCP_TO_COS_MAPPING[i][j].cfi;
            }
        }

        for(i = MIN_PRE_VAL; i <= MAX_PRE_VAL; i++)
        {
            PORT_USER_CONFIG.pre_to_dscp.table.CURRENT_PRE_TO_DSCP_MAPPING[i].phb
                =  DEFAULT_PRE_TO_DSCP_MAPPING[i].phb;
            PORT_USER_CONFIG.pre_to_dscp.table.CURRENT_PRE_TO_DSCP_MAPPING[i].color
                =  DEFAULT_PRE_TO_DSCP_MAPPING[i].color;

            PORT_USER_CONFIG.dscp_to_queue.table.CURRENT_DSCP_TO_QUEUE_MAPPING[i]
                = DEFAULT_DSCP_TO_QUEUE_MAPPING[i];
        }

        for(i = MIN_DSCP_VAL; i <= MAX_DSCP_VAL; i++)
        {
            PORT_USER_CONFIG.dscp_to_dscp.table.CURRENT_DSCP_TO_DSCP_MAPPING[i].phb
                = DEFAULT_DSCP_TO_DSCP_MAPPING[i].phb;
            PORT_USER_CONFIG.dscp_to_dscp.table.CURRENT_DSCP_TO_DSCP_MAPPING[i].color
                = DEFAULT_DSCP_TO_DSCP_MAPPING[i].color;
        }

        PORT_USER_CONFIG.trust_mode.mode = COS_MAPPING_MODE;
        PORT_USER_CONFIG.enable = TRUE;

        COS_VM_SET_PORT_TABLE_USER_CONFIG_POINT(ifindex, trust_mode);
        COS_VM_SET_PORT_TABLE_USER_CONFIG_POINT(ifindex, cos_to_dscp);
        COS_VM_SET_PORT_TABLE_USER_CONFIG_POINT(ifindex, dscp_to_queue);
        COS_VM_SET_PORT_TABLE_USER_CONFIG_POINT(ifindex, dscp_to_color);
        COS_VM_SET_PORT_TABLE_USER_CONFIG_POINT(ifindex, dscp_to_dscp);
        COS_VM_SET_PORT_TABLE_USER_CONFIG_POINT(ifindex, dscp_to_cos);
        COS_VM_SET_PORT_TABLE_USER_CONFIG_POINT(ifindex, pre_to_dscp);
        COS_VM_SET_PORT_TABLE_USER_CONFIG_POINT(ifindex, queue_weight);
        COS_VM_SET_PORT_TABLE_USER_CONFIG_POINT(ifindex, queue_mode);

#undef PORT_USER_CONFIG
    }

    if(w_chip)
    {
        if(!COS_VM_SetPortListTrustMode(ports_default,COS_MAPPING_MODE))
        {
            /*do nothing*/;
        }

        for(i = MIN_PHB_VAL; i <= MAX_PHB_VAL; i++)
        {
#if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT)
            if(!COS_VM_SetPortListIngressDscp2Queue(ports_default, i, DEFAULT_DSCP_TO_QUEUE_MAPPING[i]))
#else /* Global */
            if(!COS_VM_SetPortListIngressDscp2Queue(all_port_list, i, DEFAULT_DSCP_TO_QUEUE_MAPPING[i]))
#endif /* #if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT) */
            {
                continue;
            }
        }

        for(i = MIN_DSCP_VAL; i <= MAX_DSCP_VAL; i++)
        {
            if(!COS_VM_SetPortListIngressDscp2Dscp(ports_default, i,
                    DEFAULT_DSCP_TO_DSCP_MAPPING[i].phb, DEFAULT_DSCP_TO_DSCP_MAPPING[i].color))
            {
                continue;
            }
        }

        for(i = MIN_COS_VAL; i <= MAX_COS_VAL; i++)
        {
            for(j= 0;j<2;j++)
            {
                if(!COS_VM_SetPortListIngressCos2Dscp(ports_default,i, j,
                        DEFAULT_COS_TO_DSCP_MAPPING[i][j].phb, DEFAULT_COS_TO_DSCP_MAPPING[i][j].color))
                {
                    continue;
                }
            }
        }

        for(i = MIN_PHB_VAL; i <= (MAX_PHB_VAL*2+1); i++)
        {
            for(j=0;j<4;j++)
            {
                if(!COS_VM_SetPortListIngressDscp2Cos(ports_default,i/2,j,
                        DEFAULT_DSCP_TO_COS_MAPPING[i][j].cos, DEFAULT_DSCP_TO_COS_MAPPING[i][j].cfi))
                {
                    continue;
                }
            }
        }

        for (i = MIN_QUEUE_VAL; i < MAX_QUEUE_VAL + 1; ++i)
        {
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)

            for(ifindex =  start_ifindex; ifindex <= end_ifindex; ifindex++)
            {
                if (!SWCTRL_SetPortWrrQueueWeight(ifindex, i, DEFAULT_WRR_QUEUE_TO_WEIGHT[i]))
                {
                    continue;
                }
            }
#else
            if (!SWCTRL_SetWrrQueueWeight(i, DEFAULT_WRR_QUEUE_TO_WEIGHT[i]))
            {
                continue;
            }
#endif /* SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTR */
        }

#if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE)
        for(ifindex =  start_ifindex; ifindex <= end_ifindex; ifindex++)
        {
            SWCTRL_SetPortPrioQueueMode(ifindex, DEFAULT_QUEUE_MODE);
        }
#else
        SWCTRL_SetPrioQueueMode(DEFAULT_QUEUE_MODE);
#endif
    }
#endif /* #if (SYS_CPNT_COS_INTER_DSCP == TRUE) */
    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_EnterMasterMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the COS_VM enter the master mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void COS_VM_EnterMasterMode()
{
    UI32_T i, j;

    COS_VM_SetDebugMode(FALSE);

    /* initialize core layer default mapping tables */
    SetAllTosMapTableToDefault();
    SetAllDscpMapTableToDefault();
    SetAllTcpPortMapTableToDefault();

#if 0 /* DanXie, Thursday, August 14, 2008 10:12:21 */
    /* initialize lport status to normal port (not trunk member) */
    memset((UI8_T *)lport_status, 0, TOTAL_NBR_OF_LPORT*sizeof(UI32_T));

#if defined(STRATA_SWITCH) || defined(XGS_SWITCH)
    /* initialize driver default mapping tables */
    /* for share the same mask only init dscp rules*/
    #if(SYS_CPNT_COS_ING_DSCP_TO_COS == TRUE)
    ConfigAsicDscpDefaultMap();
    #endif
#endif

    /* initialize status variables */
    tos_or_dscp_mapping=COS_VM_DISABLE_BOTH;
    tcp_port_mapping=COS_VM_MAPPING_DISABLED;

    /*
     3com will default enable the dscp
     */
#if (SYS_CPNT_3COM_CLI == FALSE)
    /* MUST disable them after call ConfigAsicDscpDefaultMap() */
    SWCTRL_DisableTosCosMap();
    SWCTRL_DisableDscpCosMap();
#else
    tos_or_dscp_mapping = COS_VM_DSCP_MAPPING_ENABLED;
#endif
#endif /* #if 0 */

    /*cos_operation_mode = SYS_TYPE_STACKING_MASTER_MODE; */
    /* set mgr in master mode */

    for(i = 0; i <= SYS_ADPT_TOTAL_NBR_OF_LPORT; i++)
    {
        if(L_LINK_LST_Create(&cos_vm_tbl[i], SYS_ADPT_MAX_NBRS_OF_ACL_OF_COS_PER_INTERFACE,
                             sizeof(COS_TYPE_AclCosEntry_T), COS_VM_AclCosEntryCmp, TRUE) == FALSE)
        {
            for(j =0; j < i; j++)
            {
                L_LINK_LST_Destroy(&cos_vm_tbl[j]);
            }
        }
    }

    /* Need to init global config
     * so the end_ifindex is (SYS_ADPT_TOTAL_NBR_OF_LPORT + 1)
     */
    SetAllUserPortListToDefaultState(SYS_ADPT_ETHER_1_IF_INDEX_NUMBER,
        SYS_ADPT_TOTAL_NBR_OF_LPORT + 1, TRUE);

#if 0 /* DanXie, Thursday, August 14, 2008 10:16:20 */
    L_SORT_LST_Create(&TcpUdpMaplist,
                  65535,
                  sizeof(per_port_l4_dscp_t *),
                  COS_VM_TcpUdpSortCompare);

#endif /* #if 0 */
    SYSFUN_ENTER_MASTER_MODE();


    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_EnterSlaveMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the COS_VM enter the slave mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void COS_VM_EnterSlaveMode()
{
    /* do not require any memory
     */

    UI32_T i;

    for(i = 0; i <= SYS_ADPT_TOTAL_NBR_OF_LPORT; i++)
    {
        L_LINK_LST_Create(&cos_vm_tbl[i], SYS_ADPT_MAX_NBRS_OF_ACL_OF_COS_PER_INTERFACE,
                          sizeof(COS_TYPE_AclCosEntry_T), COS_VM_AclCosEntryCmp, FALSE);

    }

    SYSFUN_ENTER_SLAVE_MODE();
    /*cos_operation_mode = SYS_TYPE_STACKING_SLAVE_MODE;*/
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set the COS_VM into the transition mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void COS_VM_SetTransitionMode()
{


    SYSFUN_SET_TRANSITION_MODE();
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_EnterTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the COS_VM enter the transition mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void COS_VM_EnterTransitionMode()
{
    UI32_T i,unit_id;
    struct L_listnode *node;
    /* wait other callers leave */
    SYSFUN_ENTER_TRANSITION_MODE();

    /*cos_operation_mode = SYS_TYPE_STACKING_TRANSITION_MODE;*/
    /* initialize core layer default mapping tables */
#if 0 /* DanXie, Thursday, August 14, 2008 10:25:05 */
    SetAllTosMapTableToDefault();
    SetAllDscpMapTableToDefault();
    SetAllTcpPortMapTableToDefault();
#endif /* #if 0 */
    /* initialize lport status to normal port (not trunk member) */
    memset((UI8_T *)lport_status, 0, TOTAL_NBR_OF_LPORT*sizeof(UI32_T));


    for(i = 0; i <= SYS_ADPT_TOTAL_NBR_OF_LPORT; i++)
    {
        L_LINK_LST_Destroy(&cos_vm_tbl[i]);
    }
    SetAllUserPortListToDefaultState(SYS_ADPT_ETHER_1_IF_INDEX_NUMBER,
        SYS_ADPT_TOTAL_NBR_OF_LPORT + 1, FALSE);

    if(NULL != TcpUdpMaplist)
    {
        while(TcpUdpMaplist->head)
        {
            node = TcpUdpMaplist->head;
            TcpUdpMaplist->head = TcpUdpMaplist->head->next;
            free(node->data);
            free(node);
        }
        TcpUdpMaplist->head = NULL;
        TcpUdpMaplist->tail = NULL;
        node = NULL;
    }

    memset(TcpUdpMap_counter, 0, sizeof(TcpUdpMap_counter));
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetOperationMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function return the current opearion mode of COS's task
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   cos_operation_mode
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Stacking_Mode_T COS_VM_GetOperationMode()
{
    /*return cos_operation_mode;*/
    return SYSFUN_GET_CSC_OPERATING_MODE();
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetIpPrecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will enable or disable TOS/DSCP to COS mapping of system
 * INPUT:    flag -- COS_VM_IPPRECEDENCE_MAPPING_ENABLED/COS_VM_DSCP_MAPPING_ENABLED
 *                   /COS_VM_DISABLE_BOTH
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:  TOS/COS mapping and DSCP/COS mapping can not be enabled simultaneously
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetIpPrecedenceDscpMappingStatus(COS_VM_MAPPING_FLAG_T flag)
{
    BOOL_T asic_config=TRUE;

    /* Do protect to avoid the situation
     * when sys_cpnt define FALSE
     * the defined false value still can be set by MIB
     * For Solberg,SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_COS is defined FALSE
     * but prioIpPrecDscpStatus can be set to 2(precedence) by MIB
     * Ted Lin 08/05/04
     */
#if (SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_COS == FALSE)
    if (flag == COS_VM_IPPRECEDENCE_MAPPING_ENABLED)
    {
        return FALSE;
    }
#endif
#if (SYS_CPNT_COS_ING_DSCP_TO_COS == FALSE)
    if (flag == COS_VM_DSCP_MAPPING_ENABLED)
    {
        return FALSE;
    }
#endif

    COS_VM_USE_CSC(FALSE);

    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        /*
            For share one mask, we only use dscp mask.
        */
        switch (flag)
        {
            case COS_VM_IPPRECEDENCE_MAPPING_ENABLED:
                SWCTRL_EnableDscpCosMap();
                tos_or_dscp_mapping=COS_VM_IPPRECEDENCE_MAPPING_ENABLED;
                ConfigAsicAllTosMap();
                break;
            case COS_VM_DSCP_MAPPING_ENABLED:
                SWCTRL_EnableDscpCosMap();
                tos_or_dscp_mapping=COS_VM_DSCP_MAPPING_ENABLED;
                ConfigAsicAllDscpMap();
                break;
            case COS_VM_DISABLE_BOTH:
                if (tos_or_dscp_mapping==COS_VM_IPPRECEDENCE_MAPPING_ENABLED)
                   SWCTRL_DisableDscpCosMap();
                if (tos_or_dscp_mapping==COS_VM_DSCP_MAPPING_ENABLED)
                   SWCTRL_DisableDscpCosMap();
                tos_or_dscp_mapping=COS_VM_DISABLE_BOTH;
                asic_config=TRUE;
                break;
            default:
                asic_config=FALSE;
                break;
        };


        COS_VM_RELEASE_CSC();
        return asic_config;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetIpPrecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get tos/DSCP/COS mapping status of system
 * INPUT:    none
 * OUTPUT:   flag -- COS_VM_IPPRECEDENCE_MAPPING_ENABLED/COS_VM_DSCP_MAPPING_ENABLED
 *                   /COS_VM_DISABLE_BOTH
 * RETURN:   TRUE/FALSE
 * NOTE:  tos/COS mapping and DSCP/COS mapping can not be enabled simultaneously
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetIpPrecedenceDscpMappingStatus(COS_VM_MAPPING_FLAG_T *flag)
{
    COS_VM_USE_CSC(FALSE);


    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {

        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        *flag = tos_or_dscp_mapping;


        COS_VM_RELEASE_CSC();
        return TRUE;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetRunningIpPrecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the status of tos/DSCP/COS mapping of system
 * INPUT:    None
 * OUTPUT:   flag -- COS_VM_MAPPING_ENABLED/COS_VM_MAPPING_DISABLED
 * RETURN:   SYS_TYPE_GET_RUNNING_CFG_FAIL -- error (system is not in MASTER mode)
 *           SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE -- same as default
 *           SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different from default value
 * NOTE: default value is COS_VM_MAPPING_DISABLED
 *------------------------------------------------------------------------------
 */

SYS_TYPE_Get_Running_Cfg_T COS_VM_GetRunningIpPrecedenceDscpMappingStatus(COS_VM_MAPPING_FLAG_T *flag)
{
    COS_VM_USE_CSC(SYS_TYPE_GET_RUNNING_CFG_FAIL);


    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {

        COS_VM_RELEASE_CSC();
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }
    else
    {
        *flag = tos_or_dscp_mapping;


        COS_VM_RELEASE_CSC();

        if (tos_or_dscp_mapping==SYS_DFLT_TOSDSCP_DEFAULT_TABLE_STATUS)
            return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
        else
            return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will enable or disable TcpPort/COS mapping of system
 * INPUT:    flag -- COS_VM_MAPPING_ENABLED/COS_VM_MAPPING_DISABLED
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetTcpPortMappingStatus(COS_VM_TCPPORT_MAPPING_FLAG_T flag)
{
    BOOL_T asic_config =FALSE;

    COS_VM_USE_CSC(FALSE);

    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        if (flag == tcp_port_mapping)
        {
            COS_VM_RELEASE_CSC();
            return TRUE;
        }

        switch (flag)
        {
            case COS_VM_MAPPING_ENABLED:

                /*add all rules*/
                asic_config = COS_VM_SetAllPortMapToASIC(TRUE);

                if (TRUE == asic_config)
                {
                    tcp_port_mapping=COS_VM_MAPPING_ENABLED;
                }
                break;

            case COS_VM_MAPPING_DISABLED:

                /*remove all rules*/
                asic_config = COS_VM_SetAllPortMapToASIC(FALSE);

                if (TRUE == asic_config)
                {
                    tcp_port_mapping=COS_VM_MAPPING_DISABLED;
                }
                break;

            default:
                break;
        };

        COS_VM_RELEASE_CSC();
        return asic_config;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the status of TcpPort/COS mapping of system
 * INPUT:    None
 * OUTPUT:   flag -- COS_VM_MAPPING_ENABLED/COS_VM_MAPPING_DISABLED
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetTcpPortMappingStatus (COS_VM_TCPPORT_MAPPING_FLAG_T *flag)
{
    COS_VM_USE_CSC(FALSE);


    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {

        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        *flag = tcp_port_mapping;


        COS_VM_RELEASE_CSC();
        return TRUE;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetRunningTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the status of TcpPort/COS mapping of system
 * INPUT:    None
 * OUTPUT:   flag -- COS_VM_MAPPING_ENABLED/COS_VM_MAPPING_DISABLED
 * RETURN:   SYS_TYPE_GET_RUNNING_CFG_FAIL -- error (system is not in MASTER mode)
 *           SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE -- same as default
 *           SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different from default value
 * NOTE: default value is COS_VM_MAPPING_DISABLED
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T COS_VM_GetRunningTcpPortMappingStatus (COS_VM_TCPPORT_MAPPING_FLAG_T *flag)
{
    COS_VM_USE_CSC(SYS_TYPE_GET_RUNNING_CFG_FAIL);


    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }
    else
    {
        *flag = tcp_port_mapping;

        COS_VM_RELEASE_CSC();
        if (tcp_port_mapping==SYS_DFLT_TCPPORT_DEFAULT_TABLE_STATUS)
           return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
        else
           return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
}

#if (SYS_CPNT_COS_PER_PORT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - ConfigAsicTosDefaultMap
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will configure the default precedence to ASIC
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T ConfigAsicTosDefaultMap()
{
    UI32_T  i,cos;
    BOOL_T  asic_config=TRUE;

    for (i=0; i<COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES; i++)
    {
        cos=(UI32_T) DEFAULT_TOS_MAP[i];
        asic_config=SWCTRL_SetPortTosCosMap(ALL_PORT, i,cos);
        if (! asic_config)
            break;
    }
    return asic_config;
}
#endif
#ifndef XGS_SWITCH
/*------------------------------------------------------------------------------
 * ROUTINE NAME - ConfigAsicDscpDefaultMap
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will configure the default dscp to ASIC
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T ConfigAsicDscpDefaultMap()
{
    UI32_T  i,cos;
    BOOL_T  asic_config=TRUE;

    for (i = 0; i < COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES; i++)
    {
        if(DEFAULT_DSCP_STATUS[i] == TRUE)
        {
            cos = (UI32_T) DEFAULT_DSCP_MAP[i];
            asic_config = SWCTRL_SetPortDscpCosMap(ALL_PORT, i,cos);
            if (!asic_config)
            {
                break;
            }
        }
    }

    return asic_config;
}
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME - ConfigAsicTosMapTable
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will configure the TosMapTable to ASIC
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
#if (SYS_CPNT_COS_PER_PORT == FALSE)
#if defined(STRATA_SWITCH) || defined(XGS_SWITCH)
#else
static BOOL_T ConfigAsicTosMapTable(void)
{
    UI32_T      l_port;
    BOOL_T      asic_config=FALSE;

    /* next configure every existing l_port which has value different from default */
    for (l_port=1; l_port<=TOTAL_NBR_OF_LPORT; l_port++)
    {
        if (! SWCTRL_LogicalPortExisting(l_port))
            continue;
        else
        {
            asic_config=ConfigAsicLportTosMapTable(l_port);
            if (! asic_config)
                return FALSE;
        }
    }
    return TRUE;
}
#endif
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - ConfigAsicLportTosMapTable
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will re-configure the ASIC's ipprecedenceMapTable
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T ConfigAsicLportTosMapTable(UI32_T l_port)
{
    UI32_T  i;
    BOOL_T  asic_config=TRUE;
    UI8_T   cos;

    if ((l_port > TOTAL_NBR_OF_LPORT)||(! SWCTRL_LogicalPortExisting(l_port)))
        return FALSE;

    for (i=0; i<COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES; i++)
    {
        if (FALSE == COS_OM_GetIpPrecedence2CosValueByLport(l_port, i, &cos))
            return FALSE;

        if (cos!=(UI32_T) DEFAULT_TOS_MAP[i])
        {
            asic_config=SWCTRL_SetPortTosCosMap(l_port,i,cos);
            if (! asic_config)
                break;
        }
    };
    return asic_config;
}

#if (SYS_CPNT_COS_PER_PORT == FALSE)
#if defined(STRATA_SWITCH) || defined(XGS_SWITCH)
#else
/*------------------------------------------------------------------------------
 * ROUTINE NAME - ConfigAsicDscpMapTable
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will re-configure the ASIC's DscpMapTable
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:  1. This function will be called to synchronize ASIC with the core layer.
 *        2. It will be called when QMAP command changes the 1p mapping table.
 *        3. It will also be called when status of mapping table is changed
 *              from DISABLED to ENABLED.
 *------------------------------------------------------------------------------
 */
static BOOL_T ConfigAsicDscpMapTable()
{
    UI32_T      l_port;
    BOOL_T      asic_config=FALSE;

    /* next configure every existing l_port which has value different from default */
    for (l_port=1; l_port<=TOTAL_NBR_OF_LPORT; l_port++)
    {
        if (! SWCTRL_LogicalPortExisting(l_port))
            continue;
        else
        {
            asic_config=ConfigAsicLportDscpMapTable(l_port);
            if (! asic_config)
                return FALSE;
        }
    }
    return TRUE;
}
#endif
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - ConfigAsicLportDscpMapTable
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will re-configure the ASIC's DscpMapTable
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T ConfigAsicLportDscpMapTable(UI32_T l_port)
{
    UI32_T  i;
    BOOL_T  asic_config=TRUE;
    UI8_T   cos;

    if ((l_port > TOTAL_NBR_OF_LPORT)||(! SWCTRL_LogicalPortExisting(l_port)))
        return FALSE;

    for (i=0; i<COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES; i++)
    {
        if (FALSE == COS_OM_GetIpDscp2CosValueByLport(l_port, i, &cos))
            return FALSE;

        if (cos!=(UI32_T) DEFAULT_DSCP_MAP[i])
        {
            asic_config=SWCTRL_SetPortDscpCosMap(l_port,i,cos);
            if (! asic_config)
                break;
        }
    };
    return asic_config;
}
#if (SYS_CPNT_COS_PER_PORT == FALSE)
#if defined(STRATA_SWITCH) || defined(XGS_SWITCH)
#else
/*------------------------------------------------------------------------------
 * ROUTINE NAME - ConfigAsicTcpPortMapTable
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will re-configure the ASIC's TcpPortMapTable
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:  1. This function will be called to synchronize ASIC with the core layer.
 *        2. It will be called when QMAP command changes the 1p mapping table.
 *        3. It will also be called when status of mapping table is changed
 *              from DISABLED to ENABLED.
 *------------------------------------------------------------------------------
 */
static BOOL_T ConfigAsicTcpPortMapTable()
{
    UI32_T  l_port;
    BOOL_T  asic_config=FALSE;

    /* configure every existing l_port */
    for (l_port=1; l_port<=TOTAL_NBR_OF_LPORT; l_port++)
    {
        if (! SWCTRL_LogicalPortExisting(l_port))
            continue;
        else
        {
            asic_config=ConfigAsicLportTcpPortMapTable(l_port);
            if (! asic_config)
                return FALSE;
        }
    }
    return TRUE;
}
#endif
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - ConfigAsicLportTcpPortMapTable
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will re-configure the ASIC's TcpPortMapTable
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T ConfigAsicLportTcpPortMapTable(UI32_T l_port)
{
    COS_VM_PortConfigStatus_T   *port_config_status;

    UI32_T  i, tcp_port, cos;
    BOOL_T  asic_config=FALSE;
    UI32_T  mapping_index;
    UI32_T  unit, port, trunk;

    port_config_status = COS_VM_GetPortConfigStatus(l_port);
    if (NULL == port_config_status)
        return FALSE;

    if (SWCTRL_LPORT_UNKNOWN_PORT == SWCTRL_LogicalPortToUserPort(l_port, &unit, &port, &trunk))
    {
        return FALSE;
    }

    for (i = 0; i < port_config_status->config_count; i++)
    {
        tcp_port=(UI32_T) tcp_port_map_table[l_port-1][i].tcp_port;
        cos=(UI32_T) tcp_port_map_table[l_port-1][i].cos;

        mapping_index=tcp_port_map_table[l_port-1][i].mapping_index;
        asic_config=RULE_MGR_SetTcpPortCosMap(unit, port, tcp_port, cos, mapping_index);
        if (! asic_config)
            return FALSE;
    };
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - ApplyTosAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will copy the cos with same tos for all l_port
 * INPUT:    tos , cos
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:
 *------------------------------------------------------------------------------
 */
static void ApplyTosAllEntry(UI8_T tos,UI32_T cos)
{
    UI32_T  lport;

    for (lport = 1; TOTAL_NBR_OF_LPORT >= lport; lport++)
    {
        if (FALSE == COS_OM_SetIpPrecedence2CosValueByLport(lport, tos, cos))
        {
            return;
        }
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - ApplyDscpAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will copy the cos with same dscp for all l_port
 * INPUT:    dscp , cos
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:
 *------------------------------------------------------------------------------
 */
static void ApplyDscpAllEntry(UI8_T dscp,UI32_T cos)
{
    UI32_T  lport;

    for (lport = 1; TOTAL_NBR_OF_LPORT >= lport; lport++)
    {
        if (FALSE == COS_OM_SetIpDscp2CosValueByLport(lport, dscp, cos))
            return;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DeleteTrunkPortAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will delete the trunk port for all entry
 * INPUT:    t_port
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     Reset the trunk port information for OM
 *------------------------------------------------------------------------------
 */
static void DeleteTrunkPortAllEntry(UI32_T t_port)
{
    UI32_T i;

    for (i=0; i<COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES; i++)
    {
        if (FALSE == COS_OM_SetIpPrecedence2CosValueByLport(t_port, i, DEFAULT_TOS_MAP[i]))
            return;
    }

    for (i=0; i<COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES; i++)
    {
        if (FALSE == COS_OM_SetIpDscp2CosValueByLport(t_port, i, DEFAULT_DSCP_MAP[i]))
            return;
    }
}

#if (SYS_CPNT_COS_PER_PORT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - MemberPortDeleteOldAsicSetting
 *------------------------------------------------------------------------------
 * PURPOSE:  The trunk of member port delete old setting
 * INPUT:    trunk_ifindex , member_ifindex
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:
 *------------------------------------------------------------------------------
 */
static void MemberPortDeleteOldAsicSetting(UI32_T trunk_ifindex,UI32_T member_ifindex)
{
    DeleteTosOldAsicSetting(trunk_ifindex,member_ifindex);
    DeleteDscpOldAsicSetting(trunk_ifindex,member_ifindex);
    DeleteTcpportOldAsicSetting(trunk_ifindex,member_ifindex);

    return;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - DeleteTosOldAsicSetting
 *------------------------------------------------------------------------------
 * PURPOSE:  per port delete TOS old setting when reconfig setting
 * INPUT:    source_ifindex , dest_ifindex
 * OUTPUT:   None
 * RETURN:   TRUE / FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T DeleteTosOldAsicSetting(UI32_T source_ifindex,UI32_T dest_ifindex)
{
    BOOL_T   asic_config=FALSE;
    UI8_T    i, src_cos, dst_cos;
    UI32_T   unit,u_port,trunk_id,j;

    for (i=(UI8_T)COS_VM_IPPRECEDENCE_MIN;i<=(UI8_T)COS_VM_IPPRECEDENCE_MAX;i++)
    {
        if ((FALSE == COS_OM_GetIpPrecedence2CosValueByLport(source_ifindex, i, &src_cos)) ||
            (FALSE == COS_OM_GetIpPrecedence2CosValueByLport(dest_ifindex, i, &dst_cos)))
        {
            return FALSE;
        }

        if ((dst_cos != src_cos) && (dst_cos != DEFAULT_TOS_MAP[i]))
        {
            if(SWCTRL_LogicalPortIsTrunkPort(dest_ifindex))
            {
                /* reset member_port old setting */
                for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
                {
                    if(lport_status[j]==dest_ifindex)
                    {
                        if ((j+1)!=dest_ifindex) /* trunk port don't exist in driver */
                        {
                            SWCTRL_LogicalPortToUserPort((j+1),&unit,&u_port,&trunk_id);
                            asic_config=SWDRVL4_DelTosCosMap(unit,u_port, i);
                            if (! asic_config)
                              return FALSE;

                            if (FALSE == COS_OM_SetIpPrecedence2CosValueByLport(j + 1, i,
                                DEFAULT_TOS_MAP[i]))
                            {
                                return FALSE;
                            }
                        }
                    }
                }
            }
            else
            {
                SWCTRL_LogicalPortToUserPort(dest_ifindex,&unit,&u_port,&trunk_id);
                asic_config=SWDRVL4_DelTosCosMap(unit,u_port, i);
                if (! asic_config)
                    return FALSE;
            }

            if (FALSE == COS_OM_SetIpPrecedence2CosValueByLport(dest_ifindex, i, DEFAULT_TOS_MAP[i]))
                return FALSE;
        } /* end if */
    } /* end for ip precedence */
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DeleteDscpOldAsicSetting
 *------------------------------------------------------------------------------
 * PURPOSE:  per port delete DSCP old setting when reconfig setting
 * INPUT:    source_ifindex, dest_ifindex
 * OUTPUT:   None
 * RETURN:   TRUE / FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T DeleteDscpOldAsicSetting(UI32_T source_ifindex, UI32_T dest_ifindex)
{
    BOOL_T  asic_config=FALSE;
    UI8_T   i, src_cos, dst_cos;
    UI32_T  unit,u_port,trunk_id,j;

    for (i=(UI8_T)COS_VM_DSCP_MIN;i<=(UI8_T)COS_VM_DSCP_MAX;i++)
    {
        if ((FALSE == COS_OM_GetIpDscp2CosValueByLport(source_ifindex, i, &src_cos)) ||
            (FALSE == COS_OM_GetIpDscp2CosValueByLport(dest_ifindex, i, &dst_cos)))
        {
            return FALSE;
        }

        if ((dst_cos != src_cos) && (dst_cos != DEFAULT_DSCP_MAP[i]))
        {
            if(SWCTRL_LogicalPortIsTrunkPort(dest_ifindex))
            {
                /* reset member_port old setting */
                for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
                {
                    if(lport_status[j]==dest_ifindex)
                    {
                        if ((j+1)!=dest_ifindex) /* trunk port don't exist in driver */
                        {
                            SWCTRL_LogicalPortToUserPort((j+1),&unit,&u_port,&trunk_id);
                            asic_config=SWDRVL4_DelDscpCosMap(unit,u_port, i);
                            if (! asic_config)
                                return FALSE;

                            if (FALSE == COS_OM_SetIpDscp2CosValueByLport(j + 1, i, DEFAULT_DSCP_MAP[i]))
                                return FALSE;
                        }
                    }
                }
            }
            else
            {
                SWCTRL_LogicalPortToUserPort(dest_ifindex,&unit,&u_port,&trunk_id);
                asic_config=SWDRVL4_DelDscpCosMap(unit,u_port, i);
                if (! asic_config)
                    return FALSE;
            }

            if (FALSE == COS_OM_SetIpDscp2CosValueByLport(dest_ifindex, i, DEFAULT_DSCP_MAP[i]))
                return FALSE;
        } /* end if */
    } /* end for ip dscp */
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - SearchLportTcpPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Search lport tcpport entry
 * INPUT:    s_port, tcpport, cos
 * OUTPUT:   None
 * RETURN:   TRUE / FALSE
 * NOTE:     TRUE  : Find the same tcpport and cos for lport
 *           FALSE : Do not find
 *------------------------------------------------------------------------------
 */
static BOOL_T SearchLportTcpPortEntry(UI32_T s_port, UI16_T tcp_port, UI32_T cos)
{
    UI8_T                       i;
    COS_VM_PortConfigStatus_T   *port_config_status;

    port_config_status = COS_VM_GetPortConfigStatus(s_port);
    if (NULL == port_config_status)
        return FALSE;

    /* search */
    for (i=0; i<port_config_status->config_count; i++)
    {
        if ((tcp_port_map_table[s_port-1][i].tcp_port==tcp_port)&&(tcp_port_map_table[s_port-1][i].cos==cos))
            return TRUE;
    }

    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DeleteTcpportOldAsicSetting
 *------------------------------------------------------------------------------
 * PURPOSE:  per port delete TCPPort old setting when reconfig setting
 * INPUT:    source_ifindex, dest_ifindex
 * OUTPUT:   None
 * RETURN:   TRUE / FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T DeleteTcpportOldAsicSetting(UI32_T source_ifindex, UI32_T dest_ifindex)
{
    UI32_T  count,member_port_count,unit,u_port,trunk_id,j,i=0;
    BOOL_T  asic_config=FALSE;

    count=tcp_port_map_table_count[dest_ifindex-1];

    while (count > 0)
    {
        if(SWCTRL_LogicalPortIsTrunkPort(dest_ifindex))
        {
            /* reset member_port old setting */
            for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
            {
                if ((lport_status[j]==dest_ifindex)&&(!SearchLportTcpPortEntry(source_ifindex,
                   tcp_port_map_table[j][i].tcp_port,tcp_port_map_table[j][i].cos)))
                {
                    member_port_count=tcp_port_map_table_count[j];

                    if ((j+1)!=dest_ifindex) /* trunk port don't exist in driver */
                    {
                        SWCTRL_LogicalPortToUserPort((j+1),&unit,&u_port,&trunk_id);
                        asic_config=SWDRVL4_DelTcpPortCosMap (unit, u_port, tcp_port_map_table[j][i].tcp_port);
                        if (asic_config==FALSE)
                            return FALSE;
                    }
                    /* delete this entry from OM and decrease its counter*/
                    tcp_port_map_table[j][i].tcp_port = COS_VM_UNDEF_TCP_PORT;
                    tcp_port_map_table[j][i].cos = COS_VM_UNDEF_COS_VALUE;
                    tcp_port_map_table_count[j]=(UI8_T)(member_port_count-1);
                    if (tcp_port_map_table_count[j] > 0)
                        qsort(tcp_port_map_table[j], member_port_count, sizeof(TCP_Port_Map_T), (COMP_FUNC)compare_tcp_port);
                }
            }
            if (SearchLportTcpPortEntry(source_ifindex,
                   tcp_port_map_table[dest_ifindex-1][i].tcp_port,tcp_port_map_table[dest_ifindex-1][i].cos))
                i++;
            count--;
        }
        else
        {
            SWCTRL_LogicalPortToUserPort(dest_ifindex,&unit,&u_port,&trunk_id);
            asic_config=SWDRVL4_DelTcpPortCosMap (unit, u_port, tcp_port_map_table[dest_ifindex-1][0].tcp_port);
            if (asic_config==FALSE)
               return FALSE;
            /* delete this entry from OM and decrease its counter*/
            tcp_port_map_table[dest_ifindex-1][0].tcp_port = COS_VM_UNDEF_TCP_PORT;
            tcp_port_map_table[dest_ifindex-1][0].cos = COS_VM_UNDEF_COS_VALUE;
            tcp_port_map_table_count[dest_ifindex-1]=(UI8_T)(count-1);
                    /* re-sort the OM */
            if (tcp_port_map_table_count[dest_ifindex-1] > 0)
                qsort(tcp_port_map_table[dest_ifindex-1], count, sizeof(TCP_Port_Map_T), (COMP_FUNC)compare_tcp_port);
            count=tcp_port_map_table_count[dest_ifindex-1];
        }

    };
    return TRUE;
}
#endif

#if (SYS_CPNT_COS_PER_PORT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - MemberPortStoreBackupSetting
 *------------------------------------------------------------------------------
 * PURPOSE:  The trunk of member port store backup setting
 * INPUT:    member_ifindex, choice
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     choice : 0 = TOS , 1 = DSCP , 2 = TCPPort
 *------------------------------------------------------------------------------
 */
static void MemberPortStoreBackupSetting(UI32_T member_ifindex, COS_VM_MappingTable_T choice)
{
    UI32_T  unit,u_port,trunk_id,i;
    BOOL_T  asic_config=FALSE;

    SWCTRL_LogicalPortToUserPort(member_ifindex,&unit,&u_port,&trunk_id);
    if (choice==COS_VM_IPPRECEDENCE_MAP_TABLE)
    {
        for (i=0; i<COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES; i++)
        {
            if (FALSE == COS_OM_SetIpPrecedence2CosValueByLport(member_ifindex, i,
                backup_port_tos_map_table[i]))
            {
                return FALSE;
            }

            if (backup_port_tos_map_table[i] != DEFAULT_TOS_MAP[i])
            {
                asic_config = SWDRVL4_SetTosCosMap(0, u_port, i, backup_port_tos_map_table[i]);
                if (! asic_config)
                    return ;
            }
        }
    }
    if (choice==COS_VM_DSCP_MAP_TABLE)
    {
        for (i=0; i<COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES; i++)
        {
            if (FALSE == COS_OM_SetIpDscp2CosValueByLport(member_ifindex, i, backup_port_dscp_map_table[i]))
                return;

            if (backup_port_dscp_map_table[i] != DEFAULT_DSCP_MAP[i])
            {
                asic_config=SWDRVL4_SetDscpCosMap(0,u_port,i,backup_port_dscp_map_table[i]);
                if (! asic_config)
                    return ;
            }
        }
    }
    if (choice==COS_VM_TCP_PORT_MAP_TABLE)
    {
        tcp_port_map_table_count[member_ifindex-1]=backup_port_count;
        for (i=0; i<backup_port_count; i++)
        {
            tcp_port_map_table[member_ifindex-1][i]=backup_tcp_port_map_table[i];
            if (i<backup_port_count)
            {
                asic_config=SWDRVL4_SetTcpPortCosMap(0,u_port,(UI32_T)backup_tcp_port_map_table[i].tcp_port,
                                                    (UI32_T)backup_tcp_port_map_table[i].cos);
                if (! asic_config)
                    return ;
            }
        }
    }
}
#endif

#if (SYS_CPNT_COS_PER_PORT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - MemberPortAddNewAsicSetting
 *------------------------------------------------------------------------------
 * PURPOSE:  The trunk of member port add new setting
 * INPUT:    trunk_ifindex , member_ifindex, choice
 * OUTPUT:   None
 * RETURN:   TRUE / FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T MemberPortAddNewAsicSetting(UI32_T trunk_ifindex, UI32_T member_ifindex, UI8_T choice)
{
    BOOL_T conifg_status=FALSE;

    if (choice==0)
    {
        CopyTosMapTable(trunk_ifindex, member_ifindex);
        conifg_status=ConfigAsicLportTosMapTable(trunk_ifindex);
        if (!conifg_status)
            return FALSE;
    }
    if (choice==1)
    {
        CopyDscpMapTable(trunk_ifindex, member_ifindex);
        conifg_status=ConfigAsicLportDscpMapTable(trunk_ifindex);
        if (!conifg_status)
            return FALSE;
    }
    if (choice==2)
    {
#if defined(STRATA_SWITCH) || defined(XGS_SWITCH)
        CopyTcpPortMapTable(trunk_ifindex, member_ifindex);
        conifg_status=ConfigAsicLportTcpPortMapTable(trunk_ifindex);
        if (!conifg_status)
            return FALSE;
#endif
    }
    return TRUE;
}
#endif

#if (SYS_CPNT_COS_PER_PORT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - CopyTosFromTrunkPortToMemberPort
 *------------------------------------------------------------------------------
 * PURPOSE:  The trunk of member port add new setting
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:
 *------------------------------------------------------------------------------
 */
static void CopyTosFromTrunkPortToMemberPort(UI32_T trunk_ifindex)
{
    UI32_T j;

    for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
        if(lport_status[j]==trunk_ifindex)
            CopyTosMapTable(trunk_ifindex,(j+1));
    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - CopyDscpFromTrunkPortToMemberPort
 *------------------------------------------------------------------------------
 * PURPOSE:  The trunk of member port add new setting
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:
 *------------------------------------------------------------------------------
 */
static void CopyDscpFromTrunkPortToMemberPort(UI32_T trunk_ifindex)
{
    UI32_T j;

    for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
        if(lport_status[j]==trunk_ifindex)
            CopyDscpMapTable(trunk_ifindex,(j+1));
    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - CopyTcpPortFromTrunkPortToMemberPort
 *------------------------------------------------------------------------------
 * PURPOSE:  The trunk of member port add new setting
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:
 *------------------------------------------------------------------------------
 */
static void CopyTcpPortFromTrunkPortToMemberPort(UI32_T trunk_ifindex)
{
    UI32_T j;

    for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
        if(lport_status[j]==trunk_ifindex)
            CopyTcpPortMapTable(trunk_ifindex,(j+1));
    return;
}
#endif
#if (SYS_CPNT_COS_PER_PORT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - BackupRestoreLportOldTosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Restore old TOS Setting for l_port
 * INPUT:    l_port , choice
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     choice : 0 =  backup
                      1 =  restore
 *------------------------------------------------------------------------------
 */
static void BackupRestoreLportOldTosEntry(UI32_T l_port, UI8_T choice)
{
    BOOL_T  asic_config=FALSE;
    UI32_T  i,j,unit,u_port,trunk_id;

    if (choice==BACKUP)
    {
        for (i=0; i<COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES; i++)
        {
            if (FALSE == COS_OM_GetIpPrecedence2CosValueByLport(l_port, i,
                &backup_port_tos_map_table[i]))
            {
                return;
            }
        }
    }

    if (choice==RESTORE)
    {
        UI8_T   original_cos;
        UI32_T  lport_iter;

        if (SWCTRL_LogicalPortIsTrunkPort(l_port))  /* trunk port restore */
        {
            for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
            {
                lport_iter = j + 1; /* to 1-based index */
                if((lport_status[j] == l_port) && (lport_iter != l_port)) /* Delete Member Port */
                {
                    SWCTRL_LogicalPortToUserPort(lport_iter,&unit,&u_port,&trunk_id);
                    for (i=0; i<COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES; i++)
                    {
                        if (FALSE == COS_OM_GetIpPrecedence2CosValueByLport(lport_iter, i, &original_cos))
                            return;

                        if (original_cos != DEFAULT_TOS_MAP[i])
                            asic_config=SWDRVL4_DelTosCosMap(0,u_port,i);

                        if (FALSE == COS_OM_SetIpPrecedence2CosValueByLport(lport_iter, i,
                            backup_port_tos_map_table[i]))
                        {
                            return;
                        }

                        if (backup_port_tos_map_table[i] != DEFAULT_TOS_MAP[i])
                        {
                            asic_config=SWDRVL4_SetTosCosMap(0,u_port,i,backup_port_tos_map_table[i]);
                            if (! asic_config)
                                return ;
                        }
                    }
                }
                if (lport_iter == l_port) /* Delete Core Layer Trunk Port */
                {
                    for (i=0; i<COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES; i++)
                    {
                        if (FALSE == COS_OM_SetIpPrecedence2CosValueByLport(lport_iter, i,
                            backup_port_tos_map_table[i]))
                        {
                            return;
                        }
                    }
                }
            } /* end for lport */
        } /* end if trunk */
        if ((SWCTRL_LogicalPortToUserPort(l_port,&unit,&u_port,&trunk_id)==SWCTRL_LPORT_TRUNK_PORT_MEMBER)/* member port restore */
           || (SWCTRL_LogicalPortToUserPort(l_port,&unit,&u_port,&trunk_id)==SWCTRL_LPORT_NORMAL_PORT))   /* normal port restore */
        {
            for (i=0; i<COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES; i++)
            {
                if (FALSE == COS_OM_GetIpPrecedence2CosValueByLport(l_port, i, &original_cos))
                    return;

                if (original_cos != DEFAULT_TOS_MAP[i])
                    SWDRVL4_DelTosCosMap(0,u_port,i);

                if (FALSE == COS_OM_SetIpPrecedence2CosValueByLport(l_port, i,
                    backup_port_tos_map_table[i]))
                {
                    return;
                }

                if (backup_port_tos_map_table[i] != DEFAULT_TOS_MAP[i])
                {
                    asic_config=SWDRVL4_SetTosCosMap(0, u_port, i, backup_port_tos_map_table[i]);
                    if (! asic_config)
                        return ;
                }
            }
        }
    } /* end if restore */
    return ;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - BackupRestoreLportOldDscpEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Restore old DSCP Setting for l_port
 * INPUT:    l_port , choice
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     choice : 0 =  backup
                      1 =  restore
 *------------------------------------------------------------------------------
 */
static void BackupRestoreLportOldDscpEntry(UI32_T l_port, UI8_T choice)
{
    BOOL_T  asic_config=FALSE;
    UI32_T  i,j,unit,u_port,trunk_id;;

    if (choice==BACKUP)
    {
        for (i=0; i<COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES; i++)
        {
            if (FALSE == COS_OM_GetIpDscp2CosValueByLport(l_port, i, &backup_port_dscp_map_table[i]))
            {
                return;
            }
        }
    }

    if (choice==RESTORE)
    {
        UI8_T   original_cos;
        UI32_T  lport_iter;

        if(SWCTRL_LogicalPortIsTrunkPort(l_port))  /* trunk port restore */
        {
            for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
            {
                lport_iter = j + 1; /* to 1-based index */
                SWCTRL_LogicalPortToUserPort(lport_iter,&unit,&u_port,&trunk_id);
                if((lport_status[j]==l_port)&&(lport_iter!=l_port)) /* Delete Member Port */
                {
                    for (i=0; i<COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES; i++)
                    {
                        if (FALSE == COS_OM_GetIpDscp2CosValueByLport(lport_iter, i, &original_cos))
                            return;

                        if (original_cos != DEFAULT_DSCP_MAP[i])
                            asic_config=SWDRVL4_DelDscpCosMap(0,u_port,i);

                        if (FALSE == COS_OM_SetIpDscp2CosValueByLport(lport_iter, i,
                            backup_port_dscp_map_table[i]))
                        {
                            return;
                        }

                        if (backup_port_dscp_map_table[i] != DEFAULT_DSCP_MAP[i])
                        {
                            asic_config=SWDRVL4_SetDscpCosMap(0,u_port,i,backup_port_dscp_map_table[i]);
                            if (! asic_config)
                                return ;
                        }
                    }
                }
                if (lport_iter==l_port) /* Delete Core Layer Trunk Port */
                {
                    for (i=0; i<COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES; i++)
                    {
                        if (FALSE == COS_OM_SetIpDscp2CosValueByLport(lport_iter, i,
                            backup_port_dscp_map_table[i]))
                        {
                            return;
                        }
                    }
                }
            } /* end for j */
        } /* if SWCTRL_LogicalPortIsTrunkPort */

        if ((SWCTRL_LogicalPortToUserPort(l_port,&unit,&u_port,&trunk_id)==SWCTRL_LPORT_TRUNK_PORT_MEMBER)/* member port restore */
           || (SWCTRL_LogicalPortToUserPort(l_port,&unit,&u_port,&trunk_id)==SWCTRL_LPORT_NORMAL_PORT))   /* normal port restore */
        {
            for (i=0; i<COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES; i++)
            {
                if (FALSE == COS_OM_GetIpDscp2CosValueByLport(l_port, i, &original_cos))
                    return;

                if (original_cos != DEFAULT_DSCP_MAP[i])
                    SWDRVL4_DelDscpCosMap(0,u_port,i);

                if (FALSE == COS_OM_SetIpDscp2CosValueByLport(l_port, i, backup_port_dscp_map_table[i]))
                {
                    return;
                }

                if (backup_port_dscp_map_table[i] != DEFAULT_DSCP_MAP[i])
                {
                    asic_config=SWDRVL4_SetDscpCosMap(0,u_port,i,backup_port_dscp_map_table[i]);
                    if (! asic_config)
                        return ;
                }
            }
        }
    } /* end if restore */
    return ;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - BackupRestoreLportOldTcpPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Restore old TcpPort Setting for l_port
 * INPUT:    l_port , choice
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     choice : 0 =  backup
 *                    1 =  restore
 *------------------------------------------------------------------------------
 */
static void BackupRestoreLportOldTcpPortEntry(UI32_T l_port, UI8_T choice)
{
    BOOL_T  asic_config=FALSE;
    UI32_T  i,j,unit,u_port,trunk_id;
    UI8_T   count=0;

    if (choice==BACKUP)
    {
        for (i=0; i<MAX_TCP_PORT_MAP_ENTRIES; i++)
            backup_tcp_port_map_table[i]=tcp_port_map_table[l_port-1][i];

        backup_port_count=tcp_port_map_table_count[l_port-1];
    }
    if (choice==RESTORE)
    {
        if(SWCTRL_LogicalPortIsTrunkPort(l_port))  /* trunk port restore */
        {
            for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
            {
                if((lport_status[j]==l_port)&&((j+1)!=l_port)) /* Delete Member Port */
                {
                    SWCTRL_LogicalPortToUserPort((j+1),&unit,&u_port,&trunk_id);
                    count=backup_port_count;
                    for (i=0; i<MAX_TCP_PORT_MAP_ENTRIES; i++)
                    {
                        asic_config=SWDRVL4_DelTcpPortCosMap(0,u_port,tcp_port_map_table[j][i].tcp_port);
                        if (asic_config)
                        {
                            tcp_port_map_table[j][i]=backup_tcp_port_map_table[i];
                            if (count > 0)
                            {
                                asic_config=SWDRVL4_SetTcpPortCosMap(0,u_port,tcp_port_map_table[j][i].tcp_port,
                                                    tcp_port_map_table[j][i].cos);
                                if (! asic_config)
                                    return ;
                                count--;
                            }
                        }
                    }
                    tcp_port_map_table_count[j]=backup_port_count;
                }
                if ((j+1)==l_port) /* Delete Core Layer Trunk Port */
                {
                    for (i=0; i<MAX_TCP_PORT_MAP_ENTRIES; i++)
                        tcp_port_map_table[j][i]=backup_tcp_port_map_table[i];

                    tcp_port_map_table_count[j]=backup_port_count;
                }
            }
        }
        if ((SWCTRL_LogicalPortToUserPort(l_port,&unit,&u_port,&trunk_id)==SWCTRL_LPORT_TRUNK_PORT_MEMBER)/* member port restore */
           || (SWCTRL_LogicalPortToUserPort(l_port,&unit,&u_port,&trunk_id)==SWCTRL_LPORT_NORMAL_PORT))   /* normal port restore */
        {
            count=tcp_port_map_table_count[l_port-1];
            for (i=0; i<count; i++)
            {
                SWDRVL4_DelTcpPortCosMap(0,u_port,(UI32_T)tcp_port_map_table[l_port-1][i].tcp_port);
                tcp_port_map_table[l_port-1][i]=backup_tcp_port_map_table[i];
                if (i<backup_port_count)
                {
                    asic_config=SWDRVL4_SetTcpPortCosMap(0,u_port,(UI32_T)backup_tcp_port_map_table[i].tcp_port,
                                                        (UI32_T)backup_tcp_port_map_table[i].cos);
                    if (! asic_config)
                        return ;
                }
            }
            tcp_port_map_table_count[l_port-1]=backup_port_count;
        }
    }
    return ;
}
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a tos/CoS map in tos_map_table of specific
 *           logical port
 * INPUT:    l_port, tos (0..7), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetIpPrecedenceToCos(UI32_T l_port , UI8_T tos , UI32_T cos)
{
#if (SYS_CPNT_COS_PER_PORT == TRUE)
    UI32_T  j;
#endif

    BOOL_T  asic_config=FALSE;

#if(SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_COS == FALSE)
    return FALSE;
#endif

    COS_VM_USE_CSC(FALSE);
    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
       UI8_T   original_cos;

        COS_VM_CheckLportExisting();

        if ((tos > COS_VM_IPPRECEDENCE_MAX)||(cos > COS_VM_COS_MAX))
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

        /* configure ASIC */
        if (FALSE == COS_OM_GetIpPrecedence2CosValueByLport(l_port, tos, &original_cos))
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

#if (SYS_CPNT_COS_PER_PORT == FALSE)
        /* if global status is not enabled,
         *   do not write to asic.
         */
        if (COS_VM_IPPRECEDENCE_MAPPING_ENABLED == tos_or_dscp_mapping)
        {
            if ((cos != DEFAULT_TOS_MAP[tos]) || (original_cos != DEFAULT_TOS_MAP[tos]))
            {
                {
                    UI32_T index;

                    for (index=0; index<8; index++)
                    {
                        asic_config=SWCTRL_SetPortDscpCosMap(l_port,(UI32_T)(tos*8+index),cos);
                    }
                }
                COS_VM_CheckAsicConfig();
            }
        }
#else
        /* if global status is not enabled,
         *   do not write to asic.
         */
        if (COS_VM_IPPRECEDENCE_MAPPING_ENABLED == tos_or_dscp_mapping)
        {
            if ((cos!=DEFAULT_TOS_MAP[tos]) || (original_cos != DEFAULT_TOS_MAP[tos]))
            {
                asic_config=SWCTRL_SetPortTosCosMap(l_port,(UI32_T)tos,cos);
                if (!asic_config)
                {
                    asic_config=SWCTRL_DelPortTosCosMap(l_port,(UI32_T)tos);
                    COS_VM_CheckAsicConfig();
                }
            }
        }
#endif /*  (SYS_CPNT_COS_PER_PORT == FALSE) */

    /* Core Layer Setting */

#if (SYS_CPNT_COS_PER_PORT == FALSE)
        ApplyTosAllEntry(tos,cos);
#else
        if(SWCTRL_LogicalPortIsTrunkPort(l_port))  /* if l_port is trunk_port */
        {
            for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
            {
                if (lport_status[j] != l_port)
                    continue;

                if (FALSE == COS_OM_SetIpPrecedence2CosValueByLport(j + 1, tos, cos))
                {
                    COS_VM_RELEASE_CSC();
                    return FALSE;
                }
            }
        }
        else
        {
            if (FALSE == COS_OM_SetIpPrecedence2CosValueByLport(l_port, tos, cos))
            {
                COS_VM_RELEASE_CSC();
                return FALSE;
            }
        }
#endif

        /* if COS mapping had been changed, MUST notify RULE_MGR to update FP rule */
        if (cos != original_cos)
        {
            if (RULE_TYPE_OK != RULE_MGR_NotifyIpPrecedence2CosChangeByLport(l_port, tos))
            {
                COS_VM_RELEASE_CSC();
                return FALSE;
            }
        }

        COS_VM_RELEASE_CSC();
        return TRUE;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a tos/CoS map in tos_map_table of specific
 *           logical port
 * INPUT:    l_port, tos (0..7)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetIpPrecedenceToCos(UI32_T l_port, UI8_T tos, UI32_T *cos)
{

#if(SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_COS == FALSE)

    return FALSE;

#else
    UI8_T   original_cos;

    COS_VM_USE_CSC(FALSE);
    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    COS_VM_CheckLportExisting(); /* lport not exist if it is a trunk member */

    if (tos > COS_VM_IPPRECEDENCE_MAX)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    if (FALSE == COS_OM_GetIpPrecedence2CosValueByLport(l_port, tos, &original_cos))
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    *cos = original_cos;

    COS_VM_RELEASE_CSC();
    return TRUE;

#endif /* SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_COS */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tos/CoS map to its default value
 *           in tos_map_table of specific logical port
 * INPUT:    l_port, tos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelIpPrecedenceToCos(UI32_T l_port , UI8_T tos)
{
#if (SYS_CPNT_COS_PER_PORT == TRUE)
    UI32_T  j;
#endif
    BOOL_T  asic_config=FALSE;

#if(SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_COS == FALSE)
    return FALSE;
#endif

    COS_VM_USE_CSC(FALSE);

    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        UI8_T   original_cos;

        COS_VM_CheckLportExisting();

        if (tos > COS_VM_IPPRECEDENCE_MAX)
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

        if (FALSE == COS_OM_GetIpPrecedence2CosValueByLport(l_port, tos, &original_cos))
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

        /* configure ASIC */

#if (SYS_CPNT_COS_PER_PORT == FALSE)
        /* if global status is not enabled,
         *   do not write to asic.
         */
        if (tos_or_dscp_mapping==COS_VM_IPPRECEDENCE_MAPPING_ENABLED)
        {
            UI32_T index;

            /* reset to default rather than delete */
            for (index=0; index<8; index++)
            {
                asic_config=SWCTRL_SetPortDscpCosMap(l_port,(UI32_T)(tos*8+index),DEFAULT_TOS_MAP[tos]);
            }
            COS_VM_CheckAsicConfig();
        }
#else
        /* if global status is not enabled,
         *   do not write to asic.
         */
        if (tos_or_dscp_mapping==COS_VM_IPPRECEDENCE_MAPPING_ENABLED)
        {
            /* delete IRULE entry if we support per-port configuration */
            asic_config=SWCTRL_DelPortTosCosMap(l_port,(UI32_T)tos);
            COS_VM_CheckAsicConfig();
        }
#endif /*  (SYS_CPNT_COS_PER_PORT == FALSE) */

        /* reset OM to default value */
#if (SYS_CPNT_COS_PER_PORT == FALSE)
        ApplyTosAllEntry(tos,DEFAULT_TOS_MAP[tos]);
#else
        if (SWCTRL_LogicalPortIsTrunkPort(l_port))  /* if l_port is trunk_port */
        {
            for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
            {
                if (lport_status[j] != l_port)
                    continue;

                if (FALSE == COS_OM_SetIpPrecedence2CosValueByLport(j + 1, tos, DEFAULT_TOS_MAP[tos]))
                {
                    COS_VM_RELEASE_CSC();
                    return FALSE;
                }
            }
        }
        else
        {
            if (FALSE == COS_OM_SetIpPrecedence2CosValueByLport(l_port, tos, DEFAULT_TOS_MAP[tos]))
            {
                COS_VM_RELEASE_CSC();
                return FALSE;
            }
        }
#endif

        /* if COS mapping had been changed, MUST notify RULE_MGR to update FP rule */
        if (DEFAULT_TOS_MAP[tos] != original_cos)
        {
            if (RULE_TYPE_OK != RULE_MGR_NotifyIpPrecedence2CosChangeByLport(l_port, tos))
            {
                COS_VM_RELEASE_CSC();
                return FALSE;
            }
        }

        COS_VM_RELEASE_CSC();
        return TRUE;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelIpPrecedenceLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tos/CoS map to its default value
 *           in tos_map_table of specific logical port
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelIpPrecedenceLportAllEntry(UI32_T l_port)
{
    UI8_T i;
#if(SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_COS == FALSE)
    return FALSE;
#endif

    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
        return FALSE;

    if (!SWCTRL_LogicalPortExisting(l_port))
        return FALSE;

    for (i=(UI8_T)COS_VM_IPPRECEDENCE_MIN;i<=(UI8_T)COS_VM_IPPRECEDENCE_MAX;i++)
        if (! COS_VM_DelIpPrecedenceToCos(l_port, i)) /* semaphore is inside */
            return FALSE;

    return TRUE;
}



/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_COS_MGR_SetDSCPToCosStatus
 *------------------------------------------------------------------------------
 * PURPOSE: this function will set a dscp entry's status
 * INPUT:   l_port
 *          dscp, cos , status
 * OUTPUT:
 * RETURN:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetDSCPToCosStatus(UI32_T l_port, UI8_T dscp, BOOL_T status)
{
#if(SYS_CPNT_COS_ING_DSCP_TO_COS == FALSE)

    return FALSE;

#else

    UI8_T   cos;
    BOOL_T  asic_config;

    COS_VM_USE_CSC(FALSE);

    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        COS_VM_CheckLportExisting();

        if ((dscp > COS_VM_DSCP_MAX)/*||(cos > COS_VM_COS_MAX)*/)
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }
    }

    dscp_map_status_table[l_port-1][dscp]=status;

    if (FALSE == COS_OM_GetIpDscp2CosValueByLport(l_port, dscp, &cos))
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    if(status == TRUE)
    {
        asic_config=SWCTRL_SetPortDscpCosMap(l_port,(UI32_T)dscp,cos);
    }
    else
    {
        asic_config=SWCTRL_DelPortDscpCosMap(l_port,(UI32_T)dscp);
    }
    COS_VM_RELEASE_CSC();
    return asic_config;

#endif /* SYS_CPNT_COS_ING_DSCP_TO_COS */

}


/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_IsDSCPToCosDefaultStatus
 *------------------------------------------------------------------------------
 * PURPOSE: this function will get a dscp entry's status
 * INPUT:   l_port
 *          dscp, cos , status
 * OUTPUT:
 * RETURN:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_IsDSCPToCosDefaultStatus(UI32_T l_port, UI8_T dscp, BOOL_T status, BOOL_T *is_default)
{
    if ((dscp > COS_VM_DSCP_MAX)/*||(cos > COS_VM_COS_MAX)*/)
    {
        return FALSE;
    }

    if(DEFAULT_DSCP_STATUS[dscp] == status)
    {
        *is_default=TRUE;
        return TRUE;
    }
    else
    {
        *is_default=FALSE;
        return TRUE;
    }


}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_GetDSCPToCosStatus
 *------------------------------------------------------------------------------
 * PURPOSE: this function will get a dscp entry's status
 * INPUT:   l_port
 *          dscp, cos , status
 * OUTPUT:
 * RETURN:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetDSCPToCosStatus(UI32_T l_port, UI8_T dscp, BOOL_T *status)
{
    /*UI32_T cos;*/

    #if(SYS_CPNT_COS_ING_DSCP_TO_COS == FALSE)
    return FALSE;
    #endif

    COS_VM_USE_CSC(FALSE);


    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {

        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        COS_VM_CheckLportExisting();

        if ((dscp > COS_VM_DSCP_MAX)/*||(cos > COS_VM_COS_MAX)*/)
        {

            COS_VM_RELEASE_CSC();
            return FALSE;
        }
    }


    *status=dscp_map_status_table[l_port-1][dscp];
    COS_VM_RELEASE_CSC();

    return TRUE;

}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a Dscp/CoS map in dscp_map_table of specific
 *           logical port
 * INPUT:    l_port, dscp (0..63), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetDSCPToCos(UI32_T l_port, UI8_T dscp, UI32_T cos)
{
    BOOL_T  asic_config=FALSE;

#if(SYS_CPNT_COS_ING_DSCP_TO_COS == FALSE)
    return FALSE;
#endif

    COS_VM_USE_CSC(FALSE);

    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        UI8_T   original_cos;

        COS_VM_CheckLportExisting();

        if ((dscp > COS_VM_DSCP_MAX)||(cos > COS_VM_COS_MAX))
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

        /* configure ASIC */

        if (FALSE == COS_OM_GetIpDscp2CosValueByLport(l_port, dscp, &original_cos))
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

#if (SYS_CPNT_COS_PER_PORT == FALSE)
        /* if global status is not enabled,
         *   do not write to asic.
         */
        if (COS_VM_DSCP_MAPPING_ENABLED == tos_or_dscp_mapping)
        {
            if ((cos != DEFAULT_DSCP_MAP[dscp]) || (original_cos != DEFAULT_DSCP_MAP[dscp]))
            {
                asic_config=SWCTRL_SetPortDscpCosMap(l_port,(UI32_T)dscp,cos);
                COS_VM_CheckAsicConfig();
            }
        }

#else
        /* if global status is not enabled,
         *   do not write to asic.
         */
        if (COS_VM_DSCP_MAPPING_ENABLED == tos_or_dscp_mapping)
        {
            if ((cos != DEFAULT_DSCP_MAP[dscp]) || (original_cos != DEFAULT_DSCP_MAP[dscp]))
            {
                asic_config=SWCTRL_SetPortDscpCosMap(l_port,(UI32_T)dscp,cos);
                if (!asic_config)
                {
                    asic_config=SWCTRL_DelPortDscpCosMap(l_port,(UI32_T)dscp);
                    COS_VM_CheckAsicConfig();
                }
            }
        }

#endif /*  (SYS_CPNT_COS_PER_PORT == FALSE) */

/* Core Layer Setting */
#if (SYS_CPNT_COS_PER_PORT == FALSE)
        ApplyDscpAllEntry(dscp,cos);
#else
        if(SWCTRL_LogicalPortIsTrunkPort(l_port))  /* if l_port is trunk_port */
        {
            for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
            {
                if (lport_status[j] != l_port)
                    continue;

                if (FALSE == COS_OM_SetIpDscp2CosValueByLport(j + 1, dscp, cos))
                {
                    COS_VM_RELEASE_CSC();
                    return FALSE;
                }
            }
        }
        else
        {
            if (FALSE == COS_OM_SetIpDscp2CosValueByLport(l_port, dscp, cos))
            {
                COS_VM_RELEASE_CSC();
                return FALSE;
            }
        }
#endif

        /* if COS mapping had been changed, MUST notify RULE_MGR to update FP rule */
        if (cos != original_cos)
        {
            if (RULE_TYPE_OK != RULE_MGR_NotifyIpDscp2CosChangeByLport(l_port, dscp))
            {
                COS_VM_RELEASE_CSC();
                return FALSE;
            }
        }

        COS_VM_RELEASE_CSC();
        return TRUE;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a Dscp/CoS map in dscp_map_table of specific
 *           logical port
 * INPUT:    l_port, dscp (0..63)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetDSCPToCos(UI32_T l_port, UI8_T dscp, UI32_T *cos)
{
#if(SYS_CPNT_COS_ING_DSCP_TO_COS == FALSE)

    return FALSE;

#else

    UI8_T   original_cos;

    COS_VM_USE_CSC(FALSE);

    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    COS_VM_CheckLportExisting(); /* lport not exist if it is a trunk member */

    if (dscp > COS_VM_DSCP_MAX)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    if (FALSE == COS_OM_GetIpDscp2CosValueByLport(l_port, dscp, &original_cos))
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    *cos = original_cos;

    COS_VM_RELEASE_CSC();
    return TRUE;

#endif /* SYS_CPNT_COS_ING_DSCP_TO_COS */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a Dscp/CoS map to its default value
 *           in dscp_map_table of specific logical port
 * INPUT:    l_port, dscp (0..63)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelDSCPToCos(UI32_T l_port, UI8_T dscp)
{
    BOOL_T  asic_config=FALSE;

#if(SYS_CPNT_COS_ING_DSCP_TO_COS == FALSE)
    return FALSE;
#endif

    COS_VM_USE_CSC(FALSE);

    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {

        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        UI8_T   original_cos;

        COS_VM_CheckLportExisting();

        if (dscp > COS_VM_DSCP_MAX)
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

        if (FALSE == COS_OM_GetIpDscp2CosValueByLport(l_port, dscp, &original_cos))
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

    /* configure ASIC */
#if (SYS_CPNT_COS_PER_PORT == FALSE)
        /* if global status is not enabled,
         *   do not write to asic.
         */
        if (tos_or_dscp_mapping==COS_VM_DSCP_MAPPING_ENABLED)
        {
            /* reset to default rather than delete */
            asic_config=SWCTRL_SetPortDscpCosMap(l_port,(UI32_T)dscp,DEFAULT_DSCP_MAP[dscp]);
            COS_VM_CheckAsicConfig();
        }
#else
        /* if global status is not enabled,
         *   do not write to asic.
         */
        if (tos_or_dscp_mapping==COS_VM_DSCP_MAPPING_ENABLED)
        {
            /* delete IRULE entry if we support per-port configuration */
            asic_config=SWCTRL_DelPortDscpCosMap(l_port,(UI32_T)dscp);
            COS_VM_CheckAsicConfig();
        }
#endif /*  (SYS_CPNT_COS_PER_PORT == FALSE) */

    /* OM reset to default value */
#if (SYS_CPNT_COS_PER_PORT == FALSE)
       ApplyDscpAllEntry(dscp,DEFAULT_DSCP_MAP[dscp]);

       /* When set prioIpDscpRestoreDefault to value 1
        * all the port IPDSCP configuration should be set to default value
        * The original spec of "restore" only restores the port mappings.
        * But 3Com later added the port's "DSCP status"
        * Therefore,add to restore DSCP status
        * Ted Lin 08/06/04
        */

       dscp_map_status_table[l_port-1][dscp]=DEFAULT_DSCP_STATUS [dscp];

#else
        if (SWCTRL_LogicalPortIsTrunkPort(l_port))  /* if l_port is trunk_port */
        {
            for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
            {
                if (lport_status[j]!=l_port)
                    continue;

                if (FALSE == COS_OM_SetIpDscp2CosValueByLport(j + 1, dscp, DEFAULT_DSCP_MAP[dscp]))
                {
                    COS_VM_RELEASE_CSC();
                    return FALSE;
                }

                dscp_map_status_table[j][dscp]=DEFAULT_DSCP_STATUS [dscp];
            }
        }
        else
        {
            if (FALSE == COS_OM_SetIpDscp2CosValueByLport(l_port, dscp, DEFAULT_DSCP_MAP[dscp]))
            {
                COS_VM_RELEASE_CSC();
                return FALSE;
            }

            dscp_map_status_table[j][dscp]=DEFAULT_DSCP_STATUS [dscp];
        }
#endif

        /* if COS mapping had been changed, MUST notify RULE_MGR to update FP rule */
        if (DEFAULT_DSCP_MAP[dscp] != original_cos)
        {
            if (RULE_TYPE_OK != RULE_MGR_NotifyIpDscp2CosChangeByLport(l_port, dscp))
            {
                COS_VM_RELEASE_CSC();
                return FALSE;
            }
        }

        COS_VM_RELEASE_CSC();
        return TRUE;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelDscpLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a Dscp/CoS map to its default value
 *           in dscp_map_table of specific logical port
 * INPUT:    l_port, dscp (0..63)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelDscpLportAllEntry(UI32_T l_port)
{
    UI8_T i;

#if(SYS_CPNT_COS_ING_DSCP_TO_COS == FALSE)
    return FALSE;
#endif


    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
        return FALSE;

    if (! SWCTRL_LogicalPortExisting(l_port))
        return FALSE;

    for (i=(UI8_T)COS_VM_DSCP_MIN; i<=(UI8_T)COS_VM_DSCP_MAX; i++)
        if (! COS_VM_DelDSCPToCos(l_port, i))
            return FALSE;

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a TCP Port/CoS map in tcp_port_map_table
 *           of specific logical port
 * INPUT:    l_port, tcp_port (0..65535), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE: 1. For GetNext functions, the entries in table need to be sorted
 *          according to tcp_port number
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetPortToCos(UI32_T l_port, UI16_T tcp_port, UI32_T cos)
{
    BOOL_T  asic_config=FALSE;
    UI32_T  i,j;
    BOOL_T  found=FALSE;

    COS_VM_PortConfigStatus_T   *port_config_status;
    TCP_Port_Map_T              *tcp_port_map_p;

#if(SYS_CPNT_COS_ING_IP_PORT_TO_COS == FALSE)
    return FALSE;
#endif

    COS_VM_USE_CSC(FALSE);
    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    COS_VM_CheckLportExisting();

#if 0
    if ((tcp_port >(UI16_T) COS_VM_TCPPORT_MAX)||(tcp_port <(UI16_T)  COS_VM_TCPPORT_MIN))
    {
       COS_VM_RELEASE_CSC();
       return FALSE;
    }
#endif

    port_config_status = COS_VM_GetPortConfigStatus(l_port);
    if (NULL == port_config_status)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    /* check if there is a entry with the same tcp_port
       if found then replace else append and sort
    */
    for (i = 0; i < port_config_status->config_count; i++)
    {
        if (tcp_port_map_table[l_port-1][i].tcp_port==tcp_port)
        {
            found=TRUE; break;
        }
    }

    if ((!found) && (port_config_status->config_count >= MAX_TCP_PORT_MAP_ENTRIES))
    {
        COS_VM_RELEASE_CSC();
        return FALSE; /* OM of this l_port is full */
    }

    if (COS_VM_MAPPING_ENABLED == tcp_port_mapping)
    {
        /* configure ASIC */
        UI32_T  unit, port, trunk;

        if (SWCTRL_LPORT_UNKNOWN_PORT == SWCTRL_LogicalPortToUserPort(l_port, &unit, &port, &trunk))
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

        if (found)
        {
            /* delete original rule */
            tcp_port_map_p = &tcp_port_map_table[l_port-1][i];

            RULE_MGR_DeleteTcpPortCosMap(unit, port, tcp_port_map_p->tcp_port, tcp_port_map_p->cos,
                tcp_port_map_p->mapping_index);
        }
        else
        {
            tcp_port_map_p = COS_VM_CreateTcpPortSettingToOm(l_port, tcp_port, cos);
            if (NULL == tcp_port_map_p)
            {
                COS_VM_RELEASE_CSC();
                return FALSE;
            }
        }

        /* add rule to chip */
        asic_config = (RULE_TYPE_OK == RULE_MGR_SetTcpPortCosMap(unit, port, tcp_port, cos,
            tcp_port_map_p->mapping_index));

        COS_VM_CheckAsicConfig();
    }

    /* Core Layer Setting */
#if (SYS_CPNT_COS_PER_PORT == FALSE)
    for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
    {
        if (found)
        {
            tcp_port_map_table[j][i].cos=(UI8_T)cos; /* replace */
        }
        else
        {
            if ((COS_VM_MAPPING_ENABLED != tcp_port_mapping) ||
                (j != (l_port - 1)))
            {
                COS_VM_CreateTcpPortSettingToOm(j + 1, tcp_port, cos);
            }
        }
    }
#else
    if (found)
    {
        tcp_port_map_table[l_port-1][i].cos=(UI8_T)cos; /* replace */
    } else {
        /* append new entry in OM and counter++ */
        tcp_port_map_table[l_port-1][count].tcp_port=tcp_port;
        tcp_port_map_table[l_port-1][count].cos=(UI8_T)cos;
        tcp_port_map_table_count[l_port-1]=(UI8_T)(count+1);
        /* re-sort OM */
        qsort(tcp_port_map_table[l_port-1], count+1, sizeof(TCP_Port_Map_T), (COMP_FUNC)compare_tcp_port);
    }

    if(SWCTRL_LogicalPortIsTrunkPort(l_port))  /* if l_port is trunk_port */
    {
        for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
        {
            if((lport_status[j]==l_port)&&((j+1)!=l_port))
            {
                if (found)
                {
                    tcp_port_map_table[j][i].cos=(UI8_T)cos; /* replace */
                } else {
                    tcp_port_map_table[j][count].tcp_port=tcp_port;
                    tcp_port_map_table[j][count].cos=(UI8_T)cos;
                    tcp_port_map_table_count[j]=(UI8_T)(count+1);
                    /* re-sort OM */
                    qsort(tcp_port_map_table[j], count+1, sizeof(TCP_Port_Map_T), (COMP_FUNC)compare_tcp_port);
                }
            }
        }
    }
#endif

    COS_VM_RELEASE_CSC();
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a tcp_port/CoS map in tcp_port_map_table of specific
 *           logical port
 * INPUT:    l_port, tcp_port (0..65535)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetPortToCos(UI32_T l_port, UI16_T tcp_port, UI32_T *cos)
{
    UI32_T  i;
    BOOL_T  found=FALSE;

    COS_VM_PortConfigStatus_T   *port_config_status;

#if(SYS_CPNT_COS_ING_IP_PORT_TO_COS == FALSE)
    return FALSE;
#endif

    COS_VM_USE_CSC(FALSE);
    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    COS_VM_CheckLportExisting();

#if 0
    if ((tcp_port >(UI16_T)COS_VM_TCPPORT_MAX)||(tcp_port <(UI16_T) COS_VM_TCPPORT_MIN))
    {
       COS_VM_RELEASE_CSC();
       return FALSE;
    }
#endif

    port_config_status = COS_VM_GetPortConfigStatus(l_port);
    if (NULL == port_config_status)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    /* search */
    for (i=0; i<port_config_status->config_count; i++)
    {
        if (tcp_port_map_table[l_port-1][i].tcp_port==tcp_port)
        {
            found=TRUE; break;
        }
    }

    if (FALSE == found)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    *cos=(UI32_T) tcp_port_map_table[l_port-1][i].cos;

    COS_VM_RELEASE_CSC();
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tcp_port/CoS map to its default value
 *           in tcp_port_map_table of specific logical port
 * INPUT:    l_port, tcp_port (0..65535)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelPortToCos(UI32_T l_port, UI16_T tcp_port)
{
    UI32_T  i,j;
    BOOL_T  found=FALSE,asic_config=FALSE;

    COS_VM_PortConfigStatus_T   *port_config_status;

#if(SYS_CPNT_COS_ING_IP_PORT_TO_COS == FALSE)
    return FALSE;
#endif

    COS_VM_USE_CSC(FALSE);
    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {

        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    COS_VM_CheckLportExisting();

#if 0
    if ((tcp_port > (UI16_T)COS_VM_TCPPORT_MAX)||(tcp_port < (UI16_T)COS_VM_TCPPORT_MIN))
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }
#endif

    port_config_status = COS_VM_GetPortConfigStatus(l_port);
    if (NULL == port_config_status)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    /* search */
    for (i = 0; i < port_config_status->config_count; i++)
    {
        if (tcp_port_map_table[l_port-1][i].tcp_port==tcp_port)
        {
            found=TRUE; break;
        }
    }

    if (!found)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    /* remove rule from chip */
    if (COS_VM_MAPPING_ENABLED == tcp_port_mapping)
    {
        UI32_T  unit, port, trunk;

        if (SWCTRL_LPORT_UNKNOWN_PORT == SWCTRL_LogicalPortToUserPort(l_port, &unit, &port, &trunk))
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

        asic_config = (RULE_TYPE_OK == RULE_MGR_DeleteTcpPortCosMap(unit, port, tcp_port,
            tcp_port_map_table[l_port-1][i].cos, tcp_port_map_table[l_port-1][i].mapping_index));

        COS_VM_CheckAsicConfig();
    }

#if (SYS_CPNT_COS_PER_PORT == FALSE)
    /* remove all ports om setting */
    for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
    {
        /* delete this entry from OM and decrease its counter*/
        COS_VM_DeleteTcpPortSettingFromOm(j + 1, i);
    }
#else
            /* delete this entry from OM and decrease its counter*/
            tcp_port_map_table[l_port-1][i].tcp_port = COS_VM_UNDEF_TCP_PORT;
            tcp_port_map_table[l_port-1][i].cos = COS_VM_UNDEF_COS_VALUE;
            tcp_port_map_table_count[l_port-1]=(UI8_T)(count-1);
            /* re-sort the OM */
            if (tcp_port_map_table_count[l_port-1] > 0)
                qsort(tcp_port_map_table[l_port-1], count, sizeof(TCP_Port_Map_T), (COMP_FUNC)compare_tcp_port);

            if (SWCTRL_LogicalPortIsTrunkPort(l_port))  /* if l_port is trunk_port */
            {
                for (j=0; j<TOTAL_NBR_OF_LPORT; j++)
                {
                    if(lport_status[j]==l_port)
                    {
                        if ((j+1)!=l_port)
                        {
                              /* delete this entry from OM and decrease its counter*/
                              tcp_port_map_table[j][i].tcp_port = COS_VM_UNDEF_TCP_PORT;
                              tcp_port_map_table[j][i].cos = COS_VM_UNDEF_COS_VALUE;
                              tcp_port_map_table_count[j]=(UI8_T)(count-1);
                              /* re-sort the OM */
                              if (tcp_port_map_table_count[j] > 0)
                                  qsort(tcp_port_map_table[j], count, sizeof(TCP_Port_Map_T), (COMP_FUNC)compare_tcp_port);
                        }
                    }
                }
            }
#endif

    COS_VM_RELEASE_CSC();
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelPortLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tcp_port/CoS map to its default value
 *           in tcp_port_map_table of specific logical port
 * INPUT:    l_port,
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */

BOOL_T COS_VM_DelPortLportAllEntry( UI32_T l_port )
{
    BOOL_T  asic_config=FALSE;

    COS_VM_PortConfigStatus_T   *port_config_status;

#if(SYS_CPNT_COS_ING_IP_PORT_TO_COS == FALSE)
    return FALSE;
#endif

    COS_VM_USE_CSC(FALSE);
    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    port_config_status = COS_VM_GetPortConfigStatus(l_port);
    if (NULL == port_config_status)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    while (port_config_status->config_count > 0)
    {
        /* if the first entry is removed, the following entries will be moved up.
           so always point to the first entry here.
         */
        asic_config=COS_VM_DelPortToCos(l_port, tcp_port_map_table[l_port-1][0].tcp_port);

        COS_VM_CheckAsicConfig();
    }

    COS_VM_RELEASE_CSC();
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - compare_tcp_port
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will compare tcpport value
 * INPUT:    p1 , p2 (TCPPort structure pointer)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static int compare_tcp_port(TCP_Port_Map_T *p1, TCP_Port_Map_T *p2)
{
    UI16_T n1, n2;

    n1=p1->tcp_port; n2=p2->tcp_port;

    if (n1>n2)
        return 1;
    else
        return -1;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the value of indexed entry
 *           in tos_map_table of specific port and output next index
 * INPUT:    l_port -- logical port number
 *           tos -- tos index
 *           cos -- cos value
 * OUTPUT:   l_port, tos -- next index
 * RETURN:   True/False
 * NOTE:     The returned value will base on the database of core layer
 *           rather than the database of ASIC
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetNextIpPrecedenceToCos(UI32_T *l_port, UI8_T *tos, UI32_T *cos)
{

#if(SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_COS == FALSE)

    return FALSE;

#else

    UI8_T   original_cos;

    COS_VM_USE_CSC(FALSE);

    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    if (*l_port==0)
    {
        /* GetFirst */
        *l_port = 1;
        while (! SWCTRL_LogicalPortExisting(*l_port))
            (*l_port)++;

        *tos = (UI8_T)COS_VM_IPPRECEDENCE_MIN;

        if (FALSE == COS_OM_GetIpPrecedence2CosValueByLport(*l_port, *tos, &original_cos))
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

        *cos = original_cos;

        COS_VM_RELEASE_CSC();
        return TRUE;
    }

    /* find next index */
    if (! SWCTRL_LogicalPortExisting(*l_port) || *tos >= (UI8_T)COS_VM_IPPRECEDENCE_MAX)
    {
        (*l_port)++;
        while (! SWCTRL_LogicalPortExisting(*l_port))
        {
            (*l_port)++;
            if (*l_port > TOTAL_NBR_OF_LPORT)
            {
               COS_VM_RELEASE_CSC();
               return FALSE;
            }
        }
        *tos=(UI8_T)COS_VM_IPPRECEDENCE_MIN;
    }
    else
    {
        (*tos)++;
    }

    /* get the value of current entry */
    if (FALSE == COS_OM_GetIpPrecedence2CosValueByLport(*l_port, *tos, &original_cos))
    {
       COS_VM_RELEASE_CSC();
       return FALSE;
    }

    *cos = original_cos;

    COS_VM_RELEASE_CSC();
    return TRUE;

#endif /* SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_COS */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetLportFirstIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the first value in tos_map_table of specific port
 * INPUT:    l_port -- logical port number
 *           tos -- tos index
 * OUTPUT:   cos
 * RETURN:   True/False
 * NOTE:     The returned value will base on the database of core layer
 *           rather than the database of ASIC
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetLportFirstIpPrecedenceToCos(UI32_T *l_port, UI8_T *tos, UI32_T *cos)
{

#if(SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_COS == FALSE)
    return FALSE;
#endif

    COS_VM_USE_CSC(FALSE);


    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {

        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        UI8_T   original_cos;

        if (*l_port > TOTAL_NBR_OF_LPORT)
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

        *tos = (UI8_T)COS_VM_IPPRECEDENCE_MIN;

        if (FALSE == COS_OM_GetIpPrecedence2CosValueByLport(*l_port, *tos, &original_cos))
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

        *cos = original_cos;

        COS_VM_RELEASE_CSC();
        return TRUE;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the value of indexed entry
 *           in dscp_map_table of specific port and output next index
 * INPUT:    l_port -- logical port number
 *           dscp -- dscp index
 *           cos -- cos value
 * OUTPUT:   l_port, dscp -- next index
 * RETURN:   True/False
 * NOTE:     The returned value will base on the database of core layer
 *           rather than the database of ASIC
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetNextDSCPToCos(UI32_T *l_port, UI8_T *dscp, UI32_T *cos)
{

#if(SYS_CPNT_COS_ING_DSCP_TO_COS == FALSE)
    return FALSE;
#endif

    COS_VM_USE_CSC(FALSE);


    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {

        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        UI8_T   original_cos;

        if (*l_port==0)
        {
            /* Get the overall first entry of map table */
            *l_port = 1;
            while (! SWCTRL_LogicalPortExisting(*l_port))
                (*l_port)++;
            *dscp = (UI8_T)COS_VM_DSCP_MIN;

            if (FALSE == COS_OM_GetIpDscp2CosValueByLport(*l_port, *dscp, &original_cos))
            {
                COS_VM_RELEASE_CSC();
                return FALSE;
            }

            *cos = original_cos;

            COS_VM_RELEASE_CSC();
            return TRUE;
        }

        /* find next index */
        if ( ! SWCTRL_LogicalPortExisting(*l_port) || *dscp >= (UI8_T)COS_VM_DSCP_MAX)
        {
            (*l_port)++;
            while (! SWCTRL_LogicalPortExisting(*l_port))
            {
                (*l_port)++;
                if (*l_port > TOTAL_NBR_OF_LPORT)
                {

                   COS_VM_RELEASE_CSC();
                   return FALSE;
                }
            };
            *dscp = (UI8_T)COS_VM_DSCP_MIN;
        } else {
            (*dscp)++;
        };
        /* get the value of current entry */
        if (FALSE == COS_OM_GetIpDscp2CosValueByLport(*l_port, *dscp, &original_cos))
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

        *cos = original_cos;

        COS_VM_RELEASE_CSC();
        return TRUE;
    }
}
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetLportFirstDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the first value in dscp_map_table of specific port
 * INPUT:    l_port -- logical port number
 *           dscp -- tos index
 * OUTPUT:   cos
 * RETURN:   True/False
 * NOTE:     The returned value will base on the database of core layer
 *           rather than the database of ASIC
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetLportFirstDSCPToCos(UI32_T *l_port, UI8_T *dscp, UI32_T *cos)
{

#if(SYS_CPNT_COS_ING_DSCP_TO_COS == FALSE)

    return FALSE;

#else

    UI8_T   original_cos;

    COS_VM_USE_CSC(FALSE);

    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    if (*l_port > TOTAL_NBR_OF_LPORT)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    if (FALSE == COS_OM_GetIpDscp2CosValueByLport(*l_port, *dscp, &original_cos))
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    *dscp = (UI8_T)COS_VM_DSCP_MIN;
    *cos = original_cos;

    COS_VM_RELEASE_CSC();
    return TRUE;

#endif /* SYS_CPNT_COS_ING_DSCP_TO_COS */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the value of indexed entry
 *           in tcp_port_map_table of specific port and output next index
 * INPUT:    l_port -- logical port number
 *           tcp_port -- tcp_port index
 *           cos -- cos value
 * OUTPUT:   l_port, tcp_port -- next index
 * RETURN:   True/False
 * NOTE:     The returned value will base on the database of core layer
 *           rather than the database of ASIC
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetNextPortToCos(UI32_T *l_port , I32_T *tcp_port , UI32_T *cos)
{
#if(SYS_CPNT_COS_ING_IP_PORT_TO_COS == TRUE)

    UI32_T                      i, ifindex;
    COS_VM_PortConfigStatus_T   *port_config_status;
    TCP_Port_Map_T              *tcp_port_map_p;

    COS_VM_USE_CSC(FALSE);
    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    if ((NULL == l_port) || (NULL == tcp_port) || (NULL == cos))
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    /* there are two search keys (lport, tcp_port)
       if lport does not exist, find next available port
       if next tcp_port doest not exist, find next availble port's first tcp port map entry
    */
    for (ifindex = *l_port; TOTAL_NBR_OF_LPORT >= ifindex; ++ifindex)
    {
        if (FALSE == SWCTRL_LogicalPortExisting(ifindex))
        {
            *tcp_port = -1;
            continue;
        }

        port_config_status = COS_VM_GetPortConfigStatus(ifindex);
        if (NULL == port_config_status)
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

        for (i=0; i < port_config_status->config_count; i++)
        {
            tcp_port_map_p = &tcp_port_map_table[ifindex - 1][i];
            if (tcp_port_map_p->tcp_port > (*tcp_port))
            {
                *l_port = ifindex;
                *tcp_port = tcp_port_map_p->tcp_port;
                *cos = tcp_port_map_p->cos;

                COS_VM_RELEASE_CSC();
                return TRUE;
            }
        }

        *tcp_port = -1;
    }

    COS_VM_RELEASE_CSC();
    return FALSE;

#else

    return FALSE;
#endif
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_GetRunningIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default tos/COS mapping is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT:   l_port
 *          tos
 * OUTPUT:  cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T COS_VM_GetRunningIpPrecedenceToCos( UI32_T l_port , UI8_T tos , UI32_T *cos)
{
    UI8_T   current_value;

#if(SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_COS == FALSE)
    return SYS_TYPE_GET_RUNNING_CFG_FAIL;
#endif

    COS_VM_USE_CSC(SYS_TYPE_GET_RUNNING_CFG_FAIL);


    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {

        COS_VM_RELEASE_CSC();
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }
    else
    {
        COS_VM_RunningCheckLportExisting(); /* lport not exist if it is a trunk member */

        if (tos > COS_VM_IPPRECEDENCE_MAX)
        {
            COS_VM_RELEASE_CSC();
            return SYS_TYPE_GET_RUNNING_CFG_FAIL;
        }

        if (FALSE == COS_OM_GetIpPrecedence2CosValueByLport(l_port, tos, &current_value))
        {
            COS_VM_RELEASE_CSC();
            return SYS_TYPE_GET_RUNNING_CFG_FAIL;
        }

        *cos=(UI32_T) current_value;

        if (current_value==DEFAULT_TOS_MAP[tos])
        {

           COS_VM_RELEASE_CSC();
           return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
        }
        else
        {

           COS_VM_RELEASE_CSC();
           return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
        }
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_GetRunningDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default DSCP/COS mapping is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT:   l_port
 *          dscp
 * OUTPUT:  cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T COS_VM_GetRunningDSCPToCos(UI32_T l_port , UI8_T dscp, UI32_T *cos)
{
    UI8_T   current_value;

#if(SYS_CPNT_COS_ING_DSCP_TO_COS == FALSE)
    return SYS_TYPE_GET_RUNNING_CFG_FAIL;
#endif

    COS_VM_USE_CSC(SYS_TYPE_GET_RUNNING_CFG_FAIL);


    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {

        COS_VM_RELEASE_CSC();
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }
    else
    {
        COS_VM_RunningCheckLportExisting(); /* lport not exist if it is a trunk member */

        if (dscp > COS_VM_DSCP_MAX)
        {
            COS_VM_RELEASE_CSC();
            return SYS_TYPE_GET_RUNNING_CFG_FAIL;
        }

        if (FALSE == COS_OM_GetIpDscp2CosValueByLport(l_port, dscp, &current_value))
        {
            COS_VM_RELEASE_CSC();
            return SYS_TYPE_GET_RUNNING_CFG_FAIL;
        }

        *cos=(UI32_T) current_value;

        if (current_value==DEFAULT_DSCP_MAP[dscp])
        {

           COS_VM_RELEASE_CSC();
           return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
        }
        else
        {

           COS_VM_RELEASE_CSC();
           return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
        }
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_GetRunningPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default TCPPort/COS mapping is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT:   l_port
 *          tcp_port
 * OUTPUT:  cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T COS_VM_GetRunningPortToCos(UI32_T l_port , UI16_T tcp_port, UI32_T *cos)
{
    UI32_T                      i;
    COS_VM_PortConfigStatus_T   *port_config_status;

#if(SYS_CPNT_COS_ING_IP_PORT_TO_COS == FALSE)
    return SYS_TYPE_GET_RUNNING_CFG_FAIL;
#endif

    COS_VM_USE_CSC(SYS_TYPE_GET_RUNNING_CFG_FAIL);
    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    COS_VM_RunningCheckLportExisting(); /* lport not exist if it is a trunk member */

#if 0
    if (tcp_port > (UI16_T)COS_VM_TCPPORT_MAX)
    {
        COS_VM_RELEASE_CSC();
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }
#endif

    port_config_status = COS_VM_GetPortConfigStatus(l_port);
    if (NULL == port_config_status)
    {
        COS_VM_RELEASE_CSC();
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    /* search */
    for (i=0; i < port_config_status->config_count; i++)
    {
        if (tcp_port_map_table[l_port-1][i].tcp_port==tcp_port)
        {
            *cos=(UI32_T) tcp_port_map_table[l_port-1][i].cos;

            COS_VM_RELEASE_CSC();
            return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
        }
    }

    COS_VM_RELEASE_CSC();
    return SYS_TYPE_GET_RUNNING_CFG_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_GetNextRunningIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function same as GetRunning but also output next index
 * INPUT:   l_port
 *          tos
 * OUTPUT:  l_port
 *          tos
 *          cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */

SYS_TYPE_Get_Running_Cfg_T COS_VM_GetNextRunningIpPrecedenceToCos(UI32_T *l_port, UI8_T *tos, UI32_T *cos)
{
    BOOL_T  get_next_ok;

#if(SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_COS == FALSE)
    return SYS_TYPE_GET_RUNNING_CFG_FAIL;
#endif

    get_next_ok=COS_VM_GetNextIpPrecedenceToCos(l_port, tos, cos);

    if (! get_next_ok)
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    else
        /* get_next_ok */
        if (*cos==DEFAULT_TOS_MAP[*tos])
           return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
        else
           return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_GetNextRunningDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function same as GetRunning but also output next index
 * INPUT:   l_port
 *          dscp
 * OUTPUT:  l_port
 *          dscp
 *          cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T COS_VM_GetNextRunningDSCPToCos(UI32_T *l_port , UI8_T *dscp, UI32_T *cos)
{
    BOOL_T  get_next_ok;

#if(SYS_CPNT_COS_ING_DSCP_TO_COS == FALSE)
    return SYS_TYPE_GET_RUNNING_CFG_FAIL;
#endif
    get_next_ok=COS_VM_GetNextDSCPToCos(l_port, dscp, cos);

    if (! get_next_ok)
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    else
    {
        /* get_next_ok */
        if (*cos==DEFAULT_DSCP_MAP[*dscp])
           return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
        else
           return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_GetNextRunningPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function same as GetRunning but also output next index
 * INPUT:   l_port
 *          tcp_port
 * OUTPUT:  l_port
 *          tcp_port
 *          cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T COS_VM_GetNextRunningPortToCos(UI32_T *l_port , I32_T *tcp_port, UI32_T *cos)
{
    BOOL_T  get_next_ok;


#if(SYS_CPNT_COS_ING_IP_PORT_TO_COS == FALSE)
    return SYS_TYPE_GET_RUNNING_CFG_FAIL;
#endif

    get_next_ok=COS_VM_GetNextPortToCos(l_port, tcp_port, cos);

    if (! get_next_ok)
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    else
        /* get_next_ok */
        return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_AddFirstTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the cos function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_AddFirstTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex)
{
    COS_VM_USE_CSC(FALSE);


    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {

        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        /* set lport status to trunk id */
        lport_status[member_ifindex-1]=trunk_ifindex;
        lport_status[trunk_ifindex-1]=trunk_ifindex;

        /* copy mapping table from l_port to t_port */
        CopyTosMapTable(member_ifindex, trunk_ifindex);
        CopyDscpMapTable(member_ifindex, trunk_ifindex);
        CopyTcpPortMapTable(member_ifindex, trunk_ifindex);
        CosPortTrunkSync(member_ifindex,trunk_ifindex);

        COS_VM_RELEASE_CSC();
        return TRUE;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_AddTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the cos function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
#if (SYS_CPNT_COS_PER_PORT == FALSE)
BOOL_T COS_VM_AddTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex)
{
    COS_VM_USE_CSC(FALSE);


    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {

        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        /* set l_port state to trunk id */
        lport_status[member_ifindex-1]=trunk_ifindex;

        CosTrunkPortSync(trunk_ifindex,member_ifindex);

        COS_VM_RELEASE_CSC();
        return TRUE;
    }
}
#else
BOOL_T COS_VM_AddTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex)
{
    BOOL_T copy_config=FALSE;
    UI8_T  i;

    COS_VM_USE_CSC(FALSE);


    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {

        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        /* set l_port state to trunk id */
        lport_status[member_ifindex-1]=trunk_ifindex;

        /* backup member_ifindex setting */
        BackupRestoreLportOldTosEntry(member_ifindex,BACKUP);
        BackupRestoreLportOldDscpEntry(member_ifindex,BACKUP);
        BackupRestoreLportOldTcpPortEntry(member_ifindex,BACKUP);

        /* delete old setting for member port from driver */
        MemberPortDeleteOldAsicSetting(trunk_ifindex,member_ifindex);

        /* add new setting for member port from driver */
        for (i=0; i<3; i++)
        {
            copy_config=MemberPortAddNewAsicSetting(trunk_ifindex,member_ifindex,i);
            if (!copy_config)
            {
                if (i==0)
                {
                    BackupRestoreLportOldTosEntry(member_ifindex,RESTORE);
                    MemberPortStoreBackupSetting(member_ifindex,COS_VM_DSCP_MAP_TABLE);
                    MemberPortStoreBackupSetting(member_ifindex,COS_VM_TCP_PORT_MAP_TABLE);
                }
                if (i==1)
                {
                    BackupRestoreLportOldTosEntry(member_ifindex,RESTORE);
                    BackupRestoreLportOldDscpEntry(member_ifindex,RESTORE);
                    MemberPortStoreBackupSetting(member_ifindex,COS_VM_TCP_PORT_MAP_TABLE);
                }
                if (i==2)
                {
                    BackupRestoreLportOldTosEntry(member_ifindex,RESTORE);
                    BackupRestoreLportOldDscpEntry(member_ifindex,RESTORE);
                    BackupRestoreLportOldTcpPortEntry(member_ifindex,RESTORE);
                }


                COS_VM_RELEASE_CSC();
                return FALSE;
            }
        }


        COS_VM_RELEASE_CSC();
        return TRUE;
    }
}
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the cos function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex)
{
    COS_VM_USE_CSC(FALSE);


    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {

        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        /* reset l_port state to non-trunk member state */
        lport_status[member_ifindex-1]=0;


        COS_VM_RELEASE_CSC();
        return TRUE;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelLastTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the cos function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelLastTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex)
{
    COS_VM_USE_CSC(FALSE);

    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        /* reset l_port and t_port state to non-trunk member state */
        lport_status[member_ifindex-1]=0;
        lport_status[trunk_ifindex-1]=0;

        DeleteTrunkPortAllEntry(trunk_ifindex);


        COS_VM_RELEASE_CSC();
        return TRUE;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_CosLportConfigAsic
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the cos function that will be registed to priority manager
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
#if defined(STRATA_SWITCH) || defined(XGS_SWITCH)
BOOL_T COS_VM_CosLportConfigAsic(UI32_T l_port)
{
    BOOL_T      asic_config=FALSE;

    asic_config=ConfigAsicLportTosMapTable(l_port);
    if (asic_config==FALSE)
       return FALSE;
    asic_config=ConfigAsicLportDscpMapTable(l_port);
    if (asic_config==FALSE)
       return FALSE;
    asic_config=ConfigAsicLportTcpPortMapTable(l_port);
    if (asic_config==FALSE)
       return FALSE;

    return TRUE;
}

#else /* Noval or others */

BOOL_T COS_VM_CosLportConfigAsic(UI32_T l_port)
{
   BOOL_T      asic_config=FALSE;

   if(tos_or_dscp_mapping==COS_VM_IPPRECEDENCE_MAPPING_ENABLED)
   {
#if (SYS_CPNT_COS_PER_PORT == FALSE)
        asic_config=ConfigAsicTosMapTable();
        if (asic_config==FALSE)
           return FALSE;
#else
        asic_config=ConfigAsicTosDefaultMap();
        if (asic_config==FALSE)
           return FALSE;
        asic_config=ConfigAsicLportTosMapTable(l_port);
        if (asic_config==FALSE)
           return FALSE;
#endif
   }

   if(tos_or_dscp_mapping==COS_VM_DSCP_MAPPING_ENABLED)
   {
#if (SYS_CPNT_COS_PER_PORT == FALSE)
        asic_config=ConfigAsicDscpMapTable();
        if (asic_config==FALSE)
           return FALSE;
#else
        asic_config=ConfigAsicDscpDefaultMap();
        if (asic_config==FALSE)
           return FALSE;
        asic_config=ConfigAsicLportDscpMapTable(l_port);
        if (asic_config==FALSE)
           return FALSE;
#endif
   }

   if(tcp_port_mapping==COS_VM_MAPPING_ENABLED)
   {

#if (SYS_CPNT_COS_PER_PORT == FALSE)
        asic_config=ConfigAsicTcpPortMapTable();
        if (asic_config==FALSE)
           return FALSE;
#else
        asic_config=ConfigAsicLportTcpPortMapTable(l_port);
        if (asic_config==FALSE)
           return FALSE;
#endif
   }
   return TRUE;
}
#endif

#if (SYS_CPNT_COS_PER_PORT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - IsLportSelected
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will check if the l_port is selected in port list
 * INPUT:    l_port
 * OUTPUT:   NULL
 * RETURN:   Yes -> TRUE / No -> FALSE
 * NOTE:  This is a local function.
 *------------------------------------------------------------------------------
 */

static BOOL_T IsLportSelected(UI32_T l_port, UI8_T port_list[])
{
    UI8_T selector;
    UI32_T bytes,shifts;

    bytes=l_port/8; shifts=l_port%8;
    if (shifts==0)
    {
       shifts=8;
       bytes--;
    }
    selector=(UI8_T)(0x80>>(shifts-1));

    if ((port_list[bytes]&selector) != 0)
        return TRUE;
    else
        return FALSE;
}
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME - CopyTosMapTable
 *              - CopyDscpMapTable
 *              - CopyTcpPortMapTable
 *------------------------------------------------------------------------------
 * PURPOSE:  These functions copy mapping table from source to destination
 *           logical port
 * INPUT:    src_port, dst_port_list
 * OUTPUT:   None
 * RETURN:   None
 * NOTE: This is a local function.
 *------------------------------------------------------------------------------
 */
static BOOL_T CopyTosMapTable(UI32_T src_port, UI32_T dst_port)
{
    UI32_T  i;
    UI8_T   src_cos;

    for (i=0; i<COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES; i++)
    {
        if (FALSE == COS_OM_GetIpPrecedence2CosValueByLport(src_port, i, &src_cos))
            return FALSE;

        if (FALSE == COS_OM_SetIpPrecedence2CosValueByLport(dst_port, i, src_cos))
            return FALSE;
    }

    return TRUE;
}

static BOOL_T CopyDscpMapTable(UI32_T src_port, UI32_T dst_port)
{
    UI32_T  i;
    UI8_T   src_cos;

    for (i=0; i<COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES; i++)
    {
        if (FALSE == COS_OM_GetIpDscp2CosValueByLport(src_port, i, &src_cos))
            return FALSE;

        if (FALSE == COS_OM_SetIpDscp2CosValueByLport(dst_port, i, src_cos))
            return FALSE;
    }

    return TRUE;
}

static BOOL_T CopyTcpPortMapTable(UI32_T src_port, UI32_T dst_port)
{
    UI32_T                      i;
    COS_VM_PortConfigStatus_T   *src_port_config_status, *dst_port_config_status;

    src_port_config_status = COS_VM_GetPortConfigStatus(src_port);
    if (NULL == src_port_config_status)
        return FALSE;

    dst_port_config_status = COS_VM_GetPortConfigStatus(dst_port);
    if (NULL == dst_port_config_status)
        return FALSE;

    for (i=0; i<MAX_TCP_PORT_MAP_ENTRIES; i++)
        tcp_port_map_table[dst_port-1][i]=tcp_port_map_table[src_port-1][i];

    memcpy(dst_port_config_status, src_port_config_status, sizeof(COS_VM_PortConfigStatus_T));
    return TRUE;
}

static BOOL_T CosPortTrunkSync(UI32_T src_port, UI32_T dst_port)
{
#if 0
    UI32_T  i,cfi,k;

    for (i = 0; i < INGRESS_DSCP_TO_DSCP_MAPPING_ENTRY_NMB; i++)
    {
        PER_PORT_DSCP_DSCP[dst_port-1].CURRENT_DSCP_TO_DSCP_MAPPING[i].phb=PER_PORT_DSCP_DSCP[src_port-1].CURRENT_DSCP_TO_DSCP_MAPPING[i].phb;
        PER_PORT_DSCP_DSCP[dst_port-1].CURRENT_DSCP_TO_DSCP_MAPPING[i].color=PER_PORT_DSCP_DSCP[src_port-1].CURRENT_DSCP_TO_DSCP_MAPPING[i].color;
    }

    CURRENT_QOS_IF_MAPPING_MODE[dst_port] = CURRENT_QOS_IF_MAPPING_MODE[src_port];

    for (i = 0; i < DSCP_TO_QUEUE_MAPPING_ENTRY_NMB; i++)
    {
        PER_PORT_PRI_QUEUE[dst_port-1].CURRENT_DSCP_TO_QUEUE_MAPPING[i] = PER_PORT_PRI_QUEUE[src_port-1].CURRENT_DSCP_TO_QUEUE_MAPPING[i];
    }

    /* ip precedence to dscp
     */
    for (i = 0; i < INGRESS_PRE_TO_DSCP_MAPPING_ENTRY_NMB; i++)
    {
        PER_PORT_PRE_DSCP[dst_port-1].CURRENT_PRE_TO_DSCP_MAPPING[i].phb = PER_PORT_PRE_DSCP[src_port-1].CURRENT_PRE_TO_DSCP_MAPPING[i].phb;
        PER_PORT_PRE_DSCP[dst_port-1].CURRENT_PRE_TO_DSCP_MAPPING[i].color = PER_PORT_PRE_DSCP[src_port-1].CURRENT_PRE_TO_DSCP_MAPPING[i].color;
    }

    /* cos +cfi to internal dscp
     */
    for (i = 0; i < INGRESS_COS_TO_DSCP_MAPPING_ENTRY_NMB; i++)
    {
        for(cfi = 0; cfi <2; cfi ++)
        {
            PER_PORT_COS_DSCP[dst_port-1].CURRENT_COS_TO_DSCP_MAPPING[i][cfi].phb = PER_PORT_COS_DSCP[src_port-1].CURRENT_COS_TO_DSCP_MAPPING[i][cfi].phb;
            PER_PORT_COS_DSCP[dst_port-1].CURRENT_COS_TO_DSCP_MAPPING[i][cfi].color = PER_PORT_COS_DSCP[src_port-1].CURRENT_COS_TO_DSCP_MAPPING[i][cfi].color;
        }

    }

    for (i = 0; i < INGRESS_DSCP_TO_COS_MAPPING_PHB; i++)
    {
        for(k = 0; k <INGRESS_DSCP_TO_COS_MAPPING_COLOR; k ++)
        {
            PER_PORT_DSCP_COS[dst_port-1].CURRENT_DSCP_TO_COS_MAPPING[i][k].cos = PER_PORT_DSCP_COS[src_port-1].CURRENT_DSCP_TO_COS_MAPPING[i][k].cos;
            PER_PORT_DSCP_COS[dst_port-1].CURRENT_DSCP_TO_COS_MAPPING[i][k].cfi = PER_PORT_DSCP_COS[src_port-1].CURRENT_DSCP_TO_COS_MAPPING[i][k].cfi;
        }
    }
#endif /* #if 0*/
    return FALSE;
}

static BOOL_T CosTrunkPortSync(UI32_T src_port, UI32_T dst_port)
{
#if 0
    UI32_T  i,j;
    /* Set current value into hardware */

    CURRENT_QOS_IF_MAPPING_MODE[dst_port] = DSCP_IF_MAPPING_MODE;
    for(i = MIN_DSCP_VAL; i <= MAX_DSCP_VAL; i++)
    {
        if(!COS_VM_SetIngressDscp2Dscp(dst_port,i,PER_PORT_DSCP_DSCP[src_port-1].CURRENT_DSCP_TO_DSCP_MAPPING[i].phb,PER_PORT_DSCP_DSCP[src_port-1].CURRENT_DSCP_TO_DSCP_MAPPING[i].color))
        {
            continue;
        }
    }
    if(!COS_VM_SetTrustMode(dst_port,CURRENT_QOS_IF_MAPPING_MODE[src_port]))
    {
        return FALSE;
    }

    for(i = MIN_PHB_VAL; i <= MAX_PHB_VAL; i++)
    {
        if(!COS_VM_SetIngressDscp2Color(dst_port, i, PER_PORT_PRI_COLOR[src_port-1].CURRENT_DSCP_TO_COLOR_MAPPING[i]))
        {
            continue;
        }

    }

    for(i = MIN_PHB_VAL; i <= MAX_PHB_VAL; i++)
    {
        if(!COS_VM_SetIngressPhb2Queue(dst_port,i,PER_PORT_PRI_QUEUE[src_port-1].CURRENT_DSCP_TO_QUEUE_MAPPING[i]))
        {
            continue;
        }
    }
    /* Set current value into hardware */
    for(i = MIN_COS_VAL; i <= MAX_COS_VAL; i++)
    {
        for(j= 0;j<2;j++)
        {
            if(!COS_VM_SetIngressCos2Dscp(dst_port,i, j,PER_PORT_COS_DSCP[src_port-1].CURRENT_COS_TO_DSCP_MAPPING[i][j].phb,PER_PORT_COS_DSCP[src_port-1].CURRENT_COS_TO_DSCP_MAPPING[i][j].color))
            {
                continue;
            }
        }
    }
     /* Set current value into hardware */
    for(i = MIN_PHB_VAL; i <= MAX_PHB_VAL; i++)
    {
        for(j=0;j<4;j++)
        {
            if(!COS_VM_SetIngressDscp2Cos(dst_port,i,j,PER_PORT_DSCP_COS[src_port-1].CURRENT_DSCP_TO_COS_MAPPING[i][j].cos, PER_PORT_DSCP_COS[src_port-1].CURRENT_DSCP_TO_COS_MAPPING[i][j].cfi))
            {
            continue;
            }
        }
    }

    if(PRECEDENCE_MAPPING_MODE == CURRENT_QOS_IF_MAPPING_MODE[src_port])
    {
        for(i = MIN_PRE_VAL; i <= MAX_PRE_VAL; i++)
        {
            if(!COS_VM_SetIngressPre2Dscp(dst_port, i, PER_PORT_PRE_DSCP[src_port-1].CURRENT_PRE_TO_DSCP_MAPPING[i].phb, PER_PORT_PRE_DSCP[src_port-1].CURRENT_PRE_TO_DSCP_MAPPING[i].color))
            {
                continue;
            }
        }
    }
#endif /* #if 0 */
    return FALSE;
}
#if (SYS_CPNT_COS_PER_PORT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - CopyDestPortIsTrunkPort
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will copy the source port set to destport
 * INPUT:    src_port, d_port, choice, status
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T CopyDestPortIsTrunkPort(UI32_T s_port, UI32_T d_port,COS_VM_MappingTable_T choice
                                      , BOOL_T status)
{
    BOOL_T asic_config=FALSE;

    COS_VM_USE_CSC(FALSE);


    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {

        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {
        if (choice==COS_VM_IPPRECEDENCE_MAP_TABLE)
        {
            BackupRestoreLportOldTosEntry(d_port,BACKUP);
            DeleteTosOldAsicSetting(s_port,d_port);
            CopyTosMapTable(s_port,d_port);
            if (status==TRUE)
                CopyTosFromTrunkPortToMemberPort(d_port);
            asic_config=ConfigAsicLportTosMapTable(d_port);
            if (!asic_config)
            {
                 BackupRestoreLportOldTosEntry(d_port,RESTORE);


                 COS_VM_RELEASE_CSC();
                 return FALSE;
            }
        }
        if (choice==COS_VM_DSCP_MAP_TABLE)
        {
            BackupRestoreLportOldDscpEntry(d_port,BACKUP);
            DeleteDscpOldAsicSetting(s_port,d_port);
            CopyDscpMapTable(s_port,d_port);
            if (status==TRUE)
                CopyDscpFromTrunkPortToMemberPort(d_port);
            asic_config=ConfigAsicLportDscpMapTable(d_port);
            if (!asic_config)
            {
                BackupRestoreLportOldDscpEntry(d_port,RESTORE);


                COS_VM_RELEASE_CSC();
                return FALSE;
            }
        }
        if (choice==COS_VM_TCP_PORT_MAP_TABLE)
        {
            BackupRestoreLportOldTcpPortEntry(d_port,BACKUP);
            DeleteTcpportOldAsicSetting(s_port,d_port);
            CopyTcpPortMapTable(s_port,d_port);
            if (status==TRUE)
                CopyTcpPortFromTrunkPortToMemberPort(d_port);
            asic_config=ConfigAsicLportTcpPortMapTable(d_port);
            if (!asic_config)
            {
                BackupRestoreLportOldTcpPortEntry(d_port,RESTORE);


                COS_VM_RELEASE_CSC();
                return FALSE;
            }
        }


        COS_VM_RELEASE_CSC();
        return TRUE;
    }
}
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_PortCopy
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will copy the source port set to destport
 * INPUT:    map_table_type, src_port, dst_port_list
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE: Since it have to support multi-thread, so it is not possible to separate
 *       parameters configuration from copy operation
 *------------------------------------------------------------------------------
 */

BOOL_T COS_VM_PortCopy(UI32_T map_table_type, UI32_T src_port, UI8_T *dst_port_list)
{
#if (SYS_CPNT_COS_PER_PORT == TRUE)
    UI32_T l_port;
    BOOL_T copy_config=TRUE;
#endif

    COS_VM_USE_CSC(FALSE);


    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {

        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    else
    {

        if ((src_port > TOTAL_NBR_OF_LPORT)||(! SWCTRL_LogicalPortExisting(src_port)))
        {

           COS_VM_RELEASE_CSC();
           return FALSE;
        }

#if (SYS_CPNT_COS_PER_PORT == FALSE)


        COS_VM_RELEASE_CSC();
        return TRUE;
#else
        for (l_port=1;l_port<=TOTAL_NBR_OF_LPORT;l_port++)
        {
            if ( IsLportSelected(l_port, dst_port_list) && (l_port != src_port))
            {
                if (! SWCTRL_LogicalPortExisting(l_port))
                    continue;
                else
                {
                    switch (map_table_type)
                    {
                        case COS_VM_IPPRECEDENCE_MAP_TABLE:


                             if (SWCTRL_LogicalPortIsTrunkPort(l_port))
                                copy_config=CopyDestPortIsTrunkPort(src_port,l_port,COS_VM_IPPRECEDENCE_MAP_TABLE,TRUE);
                             else
                                copy_config=CopyDestPortIsTrunkPort(src_port,l_port,COS_VM_IPPRECEDENCE_MAP_TABLE,FALSE);
                             if (!copy_config)
                             {
                                COS_VM_RELEASE_CSC();
                                return FALSE;
                             }
                             break;

                        case COS_VM_DSCP_MAP_TABLE:


                             if (SWCTRL_LogicalPortIsTrunkPort(l_port))
                                copy_config=CopyDestPortIsTrunkPort(src_port,l_port,COS_VM_DSCP_MAP_TABLE,TRUE);
                             else
                                copy_config=CopyDestPortIsTrunkPort(src_port,l_port,COS_VM_DSCP_MAP_TABLE,FALSE);
                             if (!copy_config)
                             {
                                COS_VM_RELEASE_CSC();
                                return FALSE;
                             }
                             break;

                        case COS_VM_TCP_PORT_MAP_TABLE:


                             if (SWCTRL_LogicalPortIsTrunkPort(l_port))
                                copy_config=CopyDestPortIsTrunkPort(src_port,l_port,COS_VM_TCP_PORT_MAP_TABLE,TRUE);
                             else
                                copy_config=CopyDestPortIsTrunkPort(src_port,l_port,COS_VM_TCP_PORT_MAP_TABLE,FALSE);
                             if (!copy_config)
                             {
                                COS_VM_RELEASE_CSC();
                                return FALSE;
                             }
                             break;
                        default :
                             break;

                     }
                }
            }
        }
        COS_VM_RELEASE_CSC();
        return copy_config;
#endif
    }
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - COS_VM_ReportSyslogMessage
 *------------------------------------------------------------------------------
 * PURPOSE  : This function is another interface for each API to use syslog
 *            to report any error message.
 * INPUT    : error_type - the type of error.  Value defined in vlan_type.h
 *            error_msg - value defined in syslog_mgr.h
 *            function_name - the specific name of API to which error occured.
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
void COS_VM_ReportSyslogMessage(UI32_T error_type, UI8_T error_msg, UI8_T *function_name)
{
    SYSLOG_OM_RecordOwnerInfo_T       owner_info;

    /* BODY */

    owner_info.level = (UI8_T)error_type;
    owner_info.module_no = SYS_MODULE_COS;

    switch(error_type)
    {
        case SYSLOG_LEVEL_WARNING:
            owner_info.function_no = (UI8_T)COS_VM_COS_OM_FUNCTION_NUMBER;
            owner_info.error_no = (UI8_T)COS_VM_COS_OM_FUNCTION_ERROR_NUMBER;
            SYSLOG_PMGR_AddFormatMsgEntry(&owner_info, FUNCTION_RETURN_FAIL_INDEX, function_name, 0, 0);
            break;

        case SYSLOG_LEVEL_CRIT:
            /* This level include Memory Allocate and Free failure and Create Task Failure
             */
            owner_info.function_no = (UI8_T)COS_VM_COS_TASK_TASK_FUNCTION_NUMBER;
            owner_info.error_no = (UI8_T)COS_VM_COS_TASK_ERROR_NUMBER;
            SYSLOG_PMGR_AddFormatMsgEntry(&owner_info, error_msg, function_name, 0, 0);
            break;

        default:
            break;

    } /* end of switch */

    return;
} /* end of COS_VM_ReportSyslogMessage() */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_MarkerEntryCmp
 *------------------------------------------------------------------------------
 * PURPOSE:  to comopar two masker_entry
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     the key is acl_name
 *------------------------------------------------------------------------------
 */

static BOOL_T COS_VM_AclCosEntryCmp(void* src_pt, void* dst_pt)
{
    COS_TYPE_AclCosEntry_T *src_maker_p, *dst_maker_p;

    src_maker_p=(COS_TYPE_AclCosEntry_T*)src_pt;
    dst_maker_p=(COS_TYPE_AclCosEntry_T*)dst_pt;

    if(strcmp(src_maker_p->acl_name, dst_maker_p->acl_name) == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

#if((SYS_CPNT_ACL == TRUE) && (SYS_CPNT_COS_CONTAIN_CLI_MAP_ACCESS_LIST == TRUE))

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_CosEntrySyntaxCheck
 *------------------------------------------------------------------------------
 * PURPOSE:  This function check the syntax of cos_entry
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     does not check acl_type field, because SNMP did not need to specify
 *           acl_type
 *------------------------------------------------------------------------------
 */
static BOOL_T COS_VM_CosEntrySyntaxCheck(void *entry)
{
    COS_TYPE_AclCosEntry_T *cos_entry;
    cos_entry=(COS_TYPE_AclCosEntry_T*)entry;

    if((cos_entry->cos > COS_TYPE_COS_MAX_RAGNE) || (cos_entry->cos < COS_TYPE_COS_MIN_RAGNE))
    {
        return FALSE;
    }

    return TRUE;

}




/*------------------------------------------------------------------------------
 * ROUTINE NAME  -COS_VM_ConvertAce_2RE
 *------------------------------------------------------------------------------
 * PURPOSE:  to abtract ace's information and record to se
 *
 * INPUT  :  ace, se , inout , iport, eport
 * OUTPUT :  se
 * RETURN :
 *
 * NOTES  : 1. this function will rely on ACL to check if ace is hardware supported.
 *------------------------------------------------------------------------------
 */
static BOOL_T COS_VM_ConvertAce_2RE(UI32_T type ,ACL_Ace_Entry_T *ace , HRDRV_TYPE_ResouceEntity_T *se)
{

#if 0/*(SYS_CPNT_HRDRV == TRUE)    */
    BOOL_T             ret;
    ACL_IpAce_Entry_T  *ip=&ace->u.ip;
    ACL_MacAce_Entry_T *mac=&ace->u.mac;
    UI8_T              mask_any[]={0, 0 , 0, 0, 0, 0};

    ret=TRUE;
    HRDRV_InitRe(se);

    switch (type)
    {
        case ACL_IP_STD_ACL:
            ret &= HRDRV_QualifyField(HRDRV_L2_ETH2_ETHERTYPE, HRDRV_OPERATOR_EQUAL, 0x0800, 0xffff, se);

            if(ip->aceSourceIpAddrBitmask != ACL_IP_BITMASK_ANY)
            {
                ret &= HRDRV_QualifyField(HRDRV_L3_IP_SIP, HRDRV_OPERATOR_EQUAL, ip->aceSourceIpAddr, ip->aceSourceIpAddrBitmask, se);
            }
            if(ip->aceDestIpAddrBitmask != ACL_IP_BITMASK_ANY)
            {
                ret &= HRDRV_QualifyField(HRDRV_L3_IP_DIP, HRDRV_OPERATOR_EQUAL, ip->aceDestIpAddr, ip->aceDestIpAddrBitmask, se);
            }

            break;

       case ACL_IP_EXT_ACL:

            ret &= HRDRV_QualifyField(HRDRV_L2_ETH2_ETHERTYPE, HRDRV_OPERATOR_EQUAL, 0x0800, 0xffff, se);

            if(ip->aceSourceIpAddrBitmask != ACL_IP_BITMASK_ANY)
            {
                ret &= HRDRV_QualifyField(HRDRV_L3_IP_SIP, HRDRV_OPERATOR_EQUAL, ip->aceSourceIpAddr, ip->aceSourceIpAddrBitmask, se);
            }
            if(ip->aceDestIpAddrBitmask != ACL_IP_BITMASK_ANY)
            {
                ret &= HRDRV_QualifyField(HRDRV_L3_IP_DIP, HRDRV_OPERATOR_EQUAL, ip->aceDestIpAddr, ip->aceDestIpAddrBitmask, se);
            }

            if(ip->aceProtocol != ACL_PROTOCOL_ANY)
            {
                ret &= HRDRV_QualifyField(HRDRV_L3_IP_PROTOCOL, HRDRV_OPERATOR_EQUAL, ip->aceProtocol, 0xff, se);
            }

            if(ip->aceTos != ACL_TOS_ANY)
            {
                ret &= HRDRV_QualifyField(HRDRV_L3_IP_TOS, HRDRV_OPERATOR_EQUAL, (ip->aceTos) << 1, 0x1e, se);
            }

            if(ip->aceDscp != ACL_DSCP_ANY)
            {
                ret &= HRDRV_QualifyField(HRDRV_L3_IP_DSCP, HRDRV_OPERATOR_EQUAL, (ip->aceDscp) << 2, 0xFC, se);
            }

            if(ip->acePrec != ACL_PRECEDENCE_ANY)
            {
                ret &= HRDRV_QualifyField(HRDRV_L3_IP_PRECEDENCE, HRDRV_OPERATOR_EQUAL, (ip->acePrec) << 5, 0xE0, se);
            }

            switch(ip->aceSourcePortOp)
            {
                case ACL_OPERATOR_NO_OPERATOR:
                    break;

                case ACL_OPERATOR_EQUAL:
                    if(ip->aceSourcePortBitmask != 0)
                    {
                        ret &= HRDRV_QualifyField(HRDRV_L4_SPORT, HRDRV_OPERATOR_EQUAL,  ip->aceMinSourcePort, ip->aceSourcePortBitmask, se);
                    }
                    break;
                case ACL_OPERATOR_RANGE:
                    ret &= HRDRV_QualifyField(HRDRV_L4_SPORT, HRDRV_OPERATOR_RANGE, ip->aceMinSourcePort, ip->aceMaxSourcePort, se);
                    break;
            }

            switch(ip->aceDestPortOp)
            {
                case ACL_OPERATOR_NO_OPERATOR:
                    break;

                case ACL_OPERATOR_EQUAL:
                    if(ip->aceDestPortBitmask != 0)
                    {
                        ret &= HRDRV_QualifyField(HRDRV_L4_DPORT, HRDRV_OPERATOR_EQUAL, ip->aceMinDestPort, ip->aceDestPortBitmask, se);
                    }
                    break;
                case ACL_OPERATOR_RANGE:
                    ret &= HRDRV_QualifyField(HRDRV_L4_DPORT, HRDRV_OPERATOR_RANGE, ip->aceMinDestPort, ip->aceMaxDestPort, se);
                    break;
            }


            if(ip->aceControlCodeBitmask != ACL_TCP_CODE_BITMASK_ANY)
            {
                ret &= HRDRV_QualifyField(HRDRV_L4_TCP_CONTROL, HRDRV_OPERATOR_EQUAL, ip->aceControlCode, ip->aceControlCodeBitmask, se);
            }


            ret &= HRDRV_QualifyDoFuncType(HRDRV_DO_IP_ACL , se );
            break;


        case ACL_MAC_ACL:
            if(memcmp(mac->aceSourceMacAddrBitmask, mask_any, 6) != 0)
            {
                ret &= HRDRV_Qualify_SMac(mac->aceSourceMacAddr, mac->aceSourceMacAddrBitmask, se);
            }
            if(memcmp(mac->aceDestMacAddrBitmask, mask_any, 6) != 0)
            {
                ret &= HRDRV_Qualify_DMac(mac->aceDestMacAddr, mac->aceDestMacAddrBitmask, se);
            }

            if(mac->acePktformat != ACL_PKT_TYPE_DONT_CARE)
            {
                ret &= HRDRV_QualifyPktformat(mac->acePktformat, se);
            }





            switch(mac->aceVidOp)
            {
                case ACL_OPERATOR_NO_OPERATOR:
                    break;

                case ACL_OPERATOR_EQUAL:
                    ret &= HRDRV_QualifyField(HRDRV_L2_Dot1p_TAG_VID, HRDRV_OPERATOR_EQUAL, mac->aceMinVid, mac->aceVidBitmask, se);

                    break;

                case ACL_OPERATOR_RANGE:
                    ret &= HRDRV_QualifyField(HRDRV_L2_Dot1p_TAG_VID, HRDRV_OPERATOR_RANGE, mac->aceMinVid, mac->aceMaxVid, se);
                    break;
            }


            switch(mac->aceEtherTypeOp)
            {
                case ACL_OPERATOR_NO_OPERATOR:
                    break;

                case ACL_OPERATOR_EQUAL:
                    ret &= HRDRV_QualifyField(HRDRV_L2_ETH2_ETHERTYPE, HRDRV_OPERATOR_EQUAL, mac->aceMinEtherType, mac->aceEtherTypeBitmask , se);

                    break;

                case ACL_OPERATOR_RANGE:
                    ret &= HRDRV_QualifyField(HRDRV_L2_ETH2_ETHERTYPE, HRDRV_OPERATOR_RANGE, mac->aceMinEtherType, mac->aceMaxEtherType , se);
                    break;
            }




            ret &= HRDRV_QualifyDoFuncType(HRDRV_DO_MAC_ACL, se);
            break;

        default :
           return FALSE;
     }


    return ret;
#else
    return FALSE;
#endif
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME  -COS_VM_ReleaseAce
 *------------------------------------------------------------------------------
 * PURPOSE:  to reserve an ace
 *
 * INPUT  :  acl_name, iport, eport
 * OUTPUT :  None
 * RETURN :  TRUE/FALSE
 *
 * NO     :
 *------------------------------------------------------------------------------
 */
static BOOL_T COS_VM_ReleaseAce(UI32_T ifindex, UI32_T acl_type, ACL_Ace_Entry_T *ace, COS_TYPE_AclCosEntry_T *cos_entry)
{

#if 0/*((SYS_CPNT_HRDRV == TRUE) && (SYS_CPNT_ACL == TRUE))*/

    BOOL_T ret=TRUE;
    UI32_T unit, port, trunk_id;
    HRDRV_TYPE_ResouceEntity_T re;


    /* so far, if last, ACL has been deleted
     */
    if(SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id) == FALSE)
    {
        return FALSE;
    }

    ret &= COS_VM_ConvertAce_2RE(acl_type, ace, &re);
    ret &= HRDRV_QualifyInProfileAction(HRDRV_IN_ACTION_SET_PRI_2COS, cos_entry->cos, &re);
    ret &= HRDRV_QualifyBindingPortAndInout(unit, port, HRDRV_INBOUND, &re);
    ret &= HRDRV_Release(&re);
    return ret;
#else
    return FALSE;
#endif

}


static BOOL_T COS_VM_ReserveAce(UI32_T rid, UI32_T ifindex, ACL_Ace_Entry_T  *ace, COS_TYPE_AclCosEntry_T *cos_entry)
{
#if 0/*((SYS_CPNT_HRDRV == TRUE) && (SYS_CPNT_ACL == TRUE))         */

    BOOL_T ret=TRUE;
    UI32_T unit, port, trunk_id;
    HRDRV_TYPE_ResouceEntity_T re;
    UI32_T acl_type;

    ACL_VM_GetAclType(cos_entry->acl_name, &acl_type);
    if(SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id) == FALSE)
    {
        return FALSE;
    }


    ret &= COS_VM_ConvertAce_2RE(acl_type, ace, &re);
    ret &= HRDRV_QualifyInProfileAction(HRDRV_IN_ACTION_SET_PRI_2COS, cos_entry->cos, &re);
    ret &= HRDRV_QualifyBindingPortAndInout(unit, port, HRDRV_INBOUND, &re);
    ret &= HRDRV_Reserve(rid, &re);
    return ret;
#else
    return TRUE;
#endif
}

static BOOL_T COS_VM_BindMarkerEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
#if 0/*((SYS_CPNT_HRDRV == TRUE) && (SYS_CPNT_ACL == TRUE))    */

     UI32_T           rid;
     ACL_Ace_Entry_T  ace;


#if(SYS_ADPT_ACL_DEFINE_MASK == TRUE)
    if(ACL_VM_IsMaskOfAclExist(cos_entry->acl_name, ACL_INGRESS_MASK) == FALSE)
    {
        return FALSE;
    }
#endif



     if(HRDRV_Alloc(&rid) == FALSE)
     {
        return FALSE;
     }

     ace.u.ip.acePrecedence=0xffffffff;
     while (ACL_VM_GetNextAce_ByPrecedence(cos_entry->acl_name, &ace) == TRUE)
     {
         if(ace.u.ip.aceAction == ACL_ACTION_DENY)
         {
            continue;
         }

         if(COS_VM_ReserveAce(rid, ifindex, &ace, cos_entry) == FALSE)
         {
              HRDRV_Abort(rid);
              return FALSE;
         }
     }


     HRDRV_Commit(rid);
     return TRUE;
#else
    return TRUE;

#endif


}

static BOOL_T COS_VM_DelCosEntryBinding(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{

#if 0/*((SYS_CPNT_HRDRV == TRUE) && (SYS_CPNT_ACL == TRUE))*/
     ACL_Ace_Entry_T  ace;
     UI32_T           acl_type;

     if(ACL_VM_GetAclType(cos_entry->acl_name, &acl_type) == FALSE)
     {
        return TRUE;
     }


     ace.u.ip.acePrecedence=0xffffffff;
     while (ACL_VM_GetNextAce_ByPrecedence(cos_entry->acl_name, &ace) == TRUE)
     {
         if(ace.u.ip.aceAction == ACL_ACTION_DENY)
         {
            continue;
         }

         COS_VM_ReleaseAce(ifindex, acl_type, &ace, cos_entry);
     }

     return TRUE;
#else
    return TRUE;

#endif
}


static BOOL_T COS_VM_GetAclType(UI8_T acl_name[], UI32_T *acl_type)
{
    UI32_T type;

    if(ACL_VM_GetAclType(acl_name, &type) == TRUE)
    {
        if(ACL_VM_IsAclActive(acl_name) == FALSE)
        {
            return FALSE;
        }

        if(ACL_TYPE_IS_IP_ACL(type) == TRUE)
        {
            *acl_type=ACL_IP_ACL;
        }
        else
        {
            *acl_type=type;
        }

        return TRUE;

    }
    else
    {
        return FALSE;
    }
}



/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_IsConflictMarkerAceInInterface
 *------------------------------------------------------------------------------
 * PURPOSE:  to see if cos and marker compoenent are conflict
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T COS_VM_IsConflictMarkerAceInInterface(UI32_T ifindex, ACL_Ace_Entry_T *ace, COS_TYPE_AclCosEntry_T *src_cos_entry)
{

    COS_TYPE_AclCosEntry_T    dst_cos_entry;
    ACL_Ace_Entry_T           dst_ace;
    UI32_T                    dst_type;
    UI32_T                    acl_type;

    if(src_cos_entry->row_status != VAL_cosEntryStatus_active)
    {
        return FALSE;
    }

    if(L_LINK_LST_Get_1st(&cos_vm_tbl[ifindex], &dst_cos_entry) == FALSE)
    {
        return FALSE;
    }
    else
    {
        if(strcmp(src_cos_entry->acl_name, dst_cos_entry.acl_name) == 0)
        {
            goto next_acl_1;
        }

        COS_VM_GetAclType(dst_cos_entry.acl_name, &dst_type);
        if(src_cos_entry->acl_type != dst_type)
        {
            goto next_acl_1;
        }


        if(dst_cos_entry.row_status != VAL_cosEntryStatus_active)
        {
            goto next_acl_1;
        }

        ACL_VM_GetAclType(src_cos_entry->acl_name, &acl_type);
        dst_ace.u.ip.acePrecedence=0xffffffff;
        while(ACL_VM_GetNextAce_ByPrecedence(dst_cos_entry.acl_name, &dst_ace) == TRUE)
        {


            /* key is
               1. ACE field same
               2. action are both permit
               3. remark action can't be the same or conflict
             */
            if((ACL_VM_IsAceSame(acl_type, ace, &dst_ace) == TRUE) &&
               (ace->u.ip.aceAction == dst_ace.u.ip.aceAction) &&
               (ace->u.ip.aceAction == ACL_ACTION_PERMIT))

            {
                if(src_cos_entry->cos != dst_cos_entry.cos)
                {
                    return TRUE;
                }
            }
        }
    }

    next_acl_1:

    while(L_LINK_LST_Get_Next(&cos_vm_tbl[ifindex], &dst_cos_entry) == TRUE)
    {
        if(strcmp(src_cos_entry->acl_name, dst_cos_entry.acl_name) == 0)
        {
            continue;

        }

        if(dst_cos_entry.row_status != VAL_cosEntryStatus_active)
        {
            continue;
        }

        /* if acl_type is diffrent, omit the conflict check
         */
        ACL_VM_GetAclType(src_cos_entry->acl_name, &acl_type);
        COS_VM_GetAclType(dst_cos_entry.acl_name, &dst_type);
        if(src_cos_entry->acl_type != acl_type)
        {
            continue;
        }

        ACL_VM_GetAclType(src_cos_entry->acl_name, &acl_type);
        dst_ace.u.ip.acePrecedence=0xffffffff;
        while(ACL_VM_GetNextAce_ByPrecedence(dst_cos_entry.acl_name, &dst_ace) == TRUE)
        {


            /* key is
               1. ACE field same
               2. action are both permit
               3. remark action can't be the same or conflict
             */
            if((ACL_VM_IsAceSame(acl_type, ace, &dst_ace) == TRUE) &&
               (ace->u.ip.aceAction == dst_ace.u.ip.aceAction) &&
               (ace->u.ip.aceAction == ACL_ACTION_PERMIT))

            {
                if(src_cos_entry->cos != dst_cos_entry.cos)
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;


}



/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_IsConflictMarkerInInterface
 *------------------------------------------------------------------------------
 * PURPOSE:  to check all binding ACL's ace to see if the marker is conflict
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:
 *
 *------------------------------------------------------------------------------
 */

static BOOL_T COS_VM_IsConflictAclInInterface(UI32_T ifindex, COS_TYPE_AclCosEntry_T *src_cos_entry)
{

    COS_TYPE_AclCosEntry_T    dst_cos_entry;
    ACL_Ace_Entry_T           dst_ace, src_ace;
    UI32_T                    dst_type;
    UI32_T                    acl_type;

    if(src_cos_entry->row_status != VAL_cosEntryStatus_active)
    {
        return FALSE;
    }

    if(L_LINK_LST_Get_1st(&cos_vm_tbl[ifindex], &dst_cos_entry) == FALSE)
    {
        return FALSE;
    }
    else
    {
        if(dst_cos_entry.row_status != VAL_cosEntryStatus_active)
        {
            goto next_acl;
        }
        if(strcmp(src_cos_entry->acl_name, dst_cos_entry.acl_name) == 0)
        {
            goto next_acl;
        }

        if(COS_VM_GetAclType(dst_cos_entry.acl_name, &dst_type) == TRUE)
        {
            if(src_cos_entry->acl_type != dst_type)
            {
                goto next_acl;
            }
        }

        ACL_VM_GetAclType(src_cos_entry->acl_name, &acl_type);
        src_ace.u.ip.acePrecedence=0xffffffff;
        while(ACL_VM_GetNextAce_ByPrecedence(src_cos_entry->acl_name, &src_ace) == TRUE)
        {
            dst_ace.u.ip.acePrecedence=0xffffffff;
            while(ACL_VM_GetNextAce_ByPrecedence(dst_cos_entry.acl_name, &dst_ace) == TRUE)
            {


                /* key is
                   1. ACE field same
                   2. action are both permit
                   3. remark action can't be the same or conflict
                 */
                if((ACL_VM_IsAceSame(acl_type, &src_ace, &dst_ace) == TRUE) &&
                   (src_ace.u.ip.aceAction == dst_ace.u.ip.aceAction) &&
                   (src_ace.u.ip.aceAction == ACL_ACTION_PERMIT))

                {
                    if(src_cos_entry->cos != dst_cos_entry.cos)
                    {
                        return FALSE;
                    }
                }
            }
        }
    }


    next_acl:

    while(L_LINK_LST_Get_Next(&cos_vm_tbl[ifindex], &dst_cos_entry) == TRUE)
    {
        /* if acl_type is diffrent, omit the conflict check
         */

        if(dst_cos_entry.row_status != VAL_cosEntryStatus_active)
        {
            goto next_acl;
        }

        if(strcmp(src_cos_entry->acl_name, dst_cos_entry.acl_name) == 0)
        {
            continue;
        }

        ACL_VM_GetAclType(src_cos_entry->acl_name, &acl_type);
        if(COS_VM_GetAclType(dst_cos_entry.acl_name, &dst_type) == TRUE)
        {
            if(src_cos_entry->acl_type != dst_type)
            {
                continue;
            }
        }

        ACL_VM_GetAclType(src_cos_entry->acl_name, &acl_type);
        src_ace.u.ip.acePrecedence=0xffffffff;
        while(ACL_VM_GetNextAce_ByPrecedence(src_cos_entry->acl_name, &src_ace) == TRUE)
        {
            dst_ace.u.ip.acePrecedence=0xffffffff;
            while(ACL_VM_GetNextAce_ByPrecedence(dst_cos_entry.acl_name, &dst_ace) == TRUE)
            {


                /* key is
                   1. ACE field same
                   2. action are both permit
                   3. remark action can't be the same or conflict
                 */
                if((ACL_VM_IsAceSame(acl_type, &src_ace, &dst_ace) == TRUE) &&
                   (src_ace.u.ip.aceAction == dst_ace.u.ip.aceAction) &&
                   (src_ace.u.ip.aceAction == ACL_ACTION_PERMIT))

                {
                    if(src_cos_entry->cos != dst_cos_entry.cos)
                    {
                        return FALSE;
                    }
                }
            }
        }
    }

    return FALSE;

}
#endif



/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_AddCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  to bind a ACL to the interface that perform remark fuction
 * INPUT:    ifindex      --
 *           cos_entry --
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE
 * NOTE:     Note MARK_TYPE_REMARK_DSCP and MARK_TYPE_REMARK_TOS_PRECEDENCE  can't coexist
 *           key is (ifindex, cos_entry.acl_name)
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_AddCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
#if((SYS_CPNT_ACL == TRUE) && (SYS_CPNT_COS_CONTAIN_CLI_MAP_ACCESS_LIST == TRUE))
    UI32_T port_type;
    UI32_T unit, port, trunk_id;
    UI32_T acl_type;
    COS_TYPE_AclCosEntry_T    old_cos_entry;


    port_type=SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);
    if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
    {
        return FALSE;
    }

    if(COS_VM_CosEntrySyntaxCheck(cos_entry) == FALSE)
    {
        return FALSE;
    }

    if((cos_entry->acl_type != ACL_IP_ACL) && (cos_entry->acl_type != ACL_MAC_ACL))
    {
        return FALSE;
    }


    cos_entry->row_status=VAL_cosEntryStatus_active;


    /* ACL must exsited
     */

    if(COS_VM_GetAclType(cos_entry->acl_name, &acl_type) == FALSE)
    {
        return FALSE;
    }
    else
    {
        if(cos_entry->acl_type != acl_type)
        {
            return FALSE;
        }
    }

    /* old entry exist , replace it
     */

    strcpy(old_cos_entry.acl_name, cos_entry->acl_name);
    if(L_LINK_LST_Get(&cos_vm_tbl[ifindex], &old_cos_entry) == TRUE)
    {
        if(old_cos_entry.row_status != VAL_cosEntryStatus_active)
        {
            return FALSE;
        }

        if(old_cos_entry.cos != cos_entry->cos)
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }


        COS_VM_DelCosEntryBinding(ifindex, &old_cos_entry);
        L_LINK_LST_Delete(&cos_vm_tbl[ifindex], &old_cos_entry);

        /* only compare the active ACE
         */

        if(COS_VM_IsConflictAclInInterface(ifindex, cos_entry) ==  TRUE)
        {
            return  FALSE;
        }

        if(COS_VM_BindMarkerEntry(ifindex, cos_entry) == FALSE)
        {
            return FALSE;
        }
        else
        {
            /* replace
             */

            L_LINK_LST_Set(&cos_vm_tbl[ifindex], cos_entry, L_LINK_LST_APPEND);

            return TRUE;
        }


    }
    /* add new entry
     */
    else
    {
         /*Add to OM first
           check if any ACE is duplicate
          */

        if(COS_VM_IsConflictAclInInterface(ifindex,  cos_entry) ==  TRUE)
        {
            return FALSE;
        }


        if(L_LINK_LST_Set(&cos_vm_tbl[ifindex], cos_entry, L_LINK_LST_APPEND) == FALSE)
        {
            return FALSE;
        }

        if(COS_VM_BindMarkerEntry(ifindex, cos_entry) == FALSE)
        {
            L_LINK_LST_Delete(&cos_vm_tbl[ifindex], cos_entry);
            return FALSE;
        }
        else
        {

            return TRUE;
        }
    }
#else
    return FALSE;
#endif

}



/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  bind the ACL to interface that performed remark function
 * INPUT:    intf     --
 *           acl_name
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE
 * NOTE:     search key is (ifindex , acl_name)
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
#if((SYS_CPNT_ACL == TRUE) && (SYS_CPNT_COS_CONTAIN_CLI_MAP_ACCESS_LIST == TRUE))
    UI32_T port_type, unit, port, trunk_id;
    UI32_T type;
    COS_TYPE_AclCosEntry_T temp;

    port_type=SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);
    if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
    {
        return FALSE;
    }

    if(strlen(cos_entry->acl_name) > SYS_ADTP_LENGTH_OF_ACL_NAME)
    {
        return FALSE;
    }


    if(COS_VM_GetAclType(cos_entry->acl_name, &type) == TRUE)
    {
        if(type != cos_entry->acl_type)
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }


    strcpy(temp.acl_name, cos_entry->acl_name);
    if(L_LINK_LST_Get(&cos_vm_tbl[ifindex], &temp) == TRUE)
    {

        /* CLI and WWW can't seen a nonactive entry
         */
        if(temp.row_status != VAL_cosEntryStatus_active)
        {
            return FALSE;
        }

        COS_VM_DelCosEntryBinding(ifindex, &temp);
        L_LINK_LST_Delete(&cos_vm_tbl[ifindex], &temp);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#else
    return FALSE;
#endif

}



/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  to get the marker entry
 * INPUT:    ifindex     --
 *           cos_entry.acl_name
 * OUTPUT:   cos_entry
 * RETURN:   TRUE/FALSE
 * NOTE:     this API suppose only used by SNMP,
 *           COS_VM_GetNextMarkerEntry is enough for CLI and WWW
 *           the search key is (intf, cos_entry->acl_name)
 *
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
#if((SYS_CPNT_ACL == TRUE) && (SYS_CPNT_COS_CONTAIN_CLI_MAP_ACCESS_LIST == TRUE))
    UI32_T port_type, unit, port, trunk_id;

    port_type=SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);
    if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
    {
        return FALSE;
    }

    if(strlen(cos_entry->acl_name) > SYS_ADTP_LENGTH_OF_ACL_NAME)
    {
        return FALSE;
    }

    if(L_LINK_LST_Get(&cos_vm_tbl[ifindex], cos_entry) == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#else
    return FALSE;
#endif


}





/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get the next ACL binding to the inteface performaned remark function
 * INPUT:    ifindex     -- logical port
 *           cos_entry.acl_name
 * OUTPUT:   ifindex
 *           cos_entry
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (ifindex, mark_entry->acl_name)
 *           cos_entry->acl_name='\0' to get the frist entry
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetNextCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
#if((SYS_CPNT_ACL == TRUE) && (SYS_CPNT_COS_CONTAIN_CLI_MAP_ACCESS_LIST == TRUE))
    UI32_T port_type, unit, port, trunk_id;

    if(strlen(cos_entry->acl_name) > SYS_ADTP_LENGTH_OF_ACL_NAME)
    {
        return FALSE;
    }

    /* to get the first entry
     */
    if(strlen(cos_entry->acl_name) == 0)
    {
        if(L_LINK_LST_Get_1st(&cos_vm_tbl[ifindex], cos_entry) == TRUE)
        {
            if(cos_entry->row_status == VAL_cosEntryStatus_active)
            {
                return TRUE;
            }
            else
            {
                 while(L_LINK_LST_Get_Next(&cos_vm_tbl[ifindex], cos_entry) == TRUE)
                 {
                    if(cos_entry->row_status == VAL_cosEntryStatus_active)
                    {
                        return TRUE;
                    }
                 }
            }


        }

        return FALSE;

    }


    port_type=SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);
    if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
    {
        return FALSE;
    }


    if(L_LINK_LST_Get(&cos_vm_tbl[ifindex], cos_entry) == TRUE)
    {
        /* must find the active entry for CLI and WWW
         */

        while(L_LINK_LST_Get_Next(&cos_vm_tbl[ifindex], cos_entry) == TRUE)
        {
            if(cos_entry->row_status == VAL_cosEntryStatus_active)
            {
                return TRUE;
            }
        }

        return FALSE;
    }
    else
    {
        return FALSE;
    }
#else
    return FALSE;
#endif

}



/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SNMPGetNextCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get the next ACL binding to the inteface performaned remark function
 * INPUT:    ifindex     -- logical port
 *           cos_entry.acl_name
 * OUTPUT:   ifindex
 *           cos_entry
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (*ifindex, mark_entry->acl_name)
 *           use the *ifindex=0, cos_entry->acl_name='\0' to get the frist entry
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SNMPGetNextCosEntry(UI32_T *ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
#if((SYS_CPNT_ACL == TRUE) && (SYS_CPNT_COS_CONTAIN_CLI_MAP_ACCESS_LIST == TRUE))
    UI32_T port_type, unit, port, trunk_id;
    UI32_T                    i;

    if(strlen(cos_entry->acl_name) > SYS_ADTP_LENGTH_OF_ACL_NAME)
    {
        return FALSE;
    }


    /* to get the first entry
     */
    if((strlen(cos_entry->acl_name) == 0) && (*ifindex == 0))
    {
        for(i= (*ifindex) + 1; i <= SYS_ADPT_TOTAL_NBR_OF_LPORT; i++)
        {
            port_type=SWCTRL_LogicalPortToUserPort(i, &unit, &port, &trunk_id);
            if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
            {
                continue;
            }

            if(L_LINK_LST_Get_1st(&cos_vm_tbl[i], cos_entry) == TRUE)
            {
                *ifindex=i;
                return TRUE;
            }
        }
        return FALSE;

    }

    /* check port type
     */

    port_type=SWCTRL_LogicalPortToUserPort(*ifindex, &unit, &port, &trunk_id);
    if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
    {
        return FALSE;
    }


    /*
     in the same interface
     */
    if(L_LINK_LST_Get(&cos_vm_tbl[*ifindex], cos_entry) == TRUE)
    {
        if(L_LINK_LST_Get_Next(&cos_vm_tbl[*ifindex], cos_entry) == TRUE)
        {
            return TRUE;
        }
        else
        {
            /*
            in the next interface
            */
            for(i= (*ifindex) + 1; i <= SYS_ADPT_TOTAL_NBR_OF_LPORT; i++)
            {
                port_type=SWCTRL_LogicalPortToUserPort(i, &unit, &port, &trunk_id);
                if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
                {
                    continue;
                }


                if(L_LINK_LST_Get_1st(&cos_vm_tbl[i], cos_entry) == TRUE)
                {
                    *ifindex=i;
                    return TRUE;
                }

            }

            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
#else
    return FALSE;
#endif

}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextRunningCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get marker entry
 * INPUT:    ifindex     --
 *           cos_entry.acl_name
 * OUTPUT:   cos_entry
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     the search key is (intf, cos_entry->acl_name)
 *
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T COS_VM_GetNextRunningCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
#if((SYS_CPNT_ACL == TRUE) && (SYS_CPNT_COS_CONTAIN_CLI_MAP_ACCESS_LIST == TRUE))
    UI32_T port_type, unit, port, trunk_id;

    if(strlen(cos_entry->acl_name) > SYS_ADTP_LENGTH_OF_ACL_NAME)
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    /* to get the first entry
     */
    if(strlen(cos_entry->acl_name) == 0)
    {
        if(L_LINK_LST_Get_1st(&cos_vm_tbl[ifindex], cos_entry) == TRUE)
        {
            if(cos_entry->row_status == VAL_cosEntryStatus_active)
            {
                return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
            }
            else
            {
                 while(L_LINK_LST_Get_Next(&cos_vm_tbl[ifindex], cos_entry) == TRUE)
                 {
                    if(cos_entry->row_status == VAL_cosEntryStatus_active)
                    {
                        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
                    }
                 }
            }


        }

        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;

    }


    port_type=SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);
    if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
    {
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }


    if(L_LINK_LST_Get(&cos_vm_tbl[ifindex], cos_entry) == TRUE)
    {
        /* must find the active entry for CLI and WWW
         */

        while(L_LINK_LST_Get_Next(&cos_vm_tbl[ifindex], cos_entry) == TRUE)
        {
            if(cos_entry->row_status == VAL_cosEntryStatus_active)
            {
                return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
            }
        }

        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }
    else
    {
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }
#else
    return SYS_TYPE_GET_RUNNING_CFG_FAIL;
#endif

}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_FSM
 *------------------------------------------------------------------------------
 * PURPOSE:  the FSM of row status
 * INPUT:    (ifindex, acl_name) is the key
 *           row_status
 * OUTPUT :  None
 * RETURN:   TRUE/FALSE
 * NOTE:     once in active state, only can be destoryed,
 *           cos_entry is used only if the row_Status=createAndgo
 *------------------------------------------------------------------------------
 */
#if((SYS_CPNT_ACL == TRUE) && (SYS_CPNT_COS_CONTAIN_CLI_MAP_ACCESS_LIST == TRUE))
static BOOL_T COS_VM_FSM(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry, UI32_T row_status)
{
    UI32_T state;
    COS_TYPE_AclCosEntry_T temp;
    ACL_Ace_Entry_T           ace;
    UI32_T                    acl_type;
    BOOL_T                    ret;

    /* temp is used to store the value in OM, or new create entry or modified entry
     */


    strcpy(temp.acl_name, cos_entry->acl_name);
    if(L_LINK_LST_Get(&cos_vm_tbl[ifindex], &temp) == TRUE)
    {

        state=temp.row_status;
        if(row_status == L_RSTATUS_SET_OTHER_COLUMN)
        {
            memcpy(&temp, cos_entry, sizeof(COS_TYPE_AclCosEntry_T));
        }
    }
    else
    {
        if((row_status == VAL_cosEntryStatus_createAndGo) || (row_status == L_RSTATUS_SET_OTHER_COLUMN))
        {
            memcpy(&temp, cos_entry, sizeof(COS_TYPE_AclCosEntry_T));
        }
        else
        {
            COS_VM_InitDefaultEntry(&temp);
            /*
            temp.acl_type=acl_type;
            */

        }

        state=ACL_aceRowStatus_notExist;
    }


    switch(L_RSTATUS_Fsm(row_status, &state, COS_VM_CosEntrySyntaxCheck, &temp) )
    {

        case L_RSTATUS_NOTEXIST_2_NOTEXIST:
            return FALSE;
            break;

        case L_RSTATUS_NOTEXIST_2_ACTIVE:
           /* bind to interafce
            */

            if(COS_VM_GetAclType(cos_entry->acl_name, &acl_type) == FALSE)
            {
                return FALSE;
            }

            temp.row_status=VAL_cosEntryStatus_active;
            temp.acl_type=acl_type;
            ace.u.ip.acePrecedence=0xffffffff;
            while(ACL_VM_GetNextAce_ByPrecedence(cos_entry->acl_name, &ace) == TRUE)
            {
                if(COS_VM_IsConflictMarkerAceInInterface(ifindex,  &ace, cos_entry) == TRUE)
                {
                    return FALSE;
                }
            }

            if(L_LINK_LST_Set(&cos_vm_tbl[ifindex], &temp, L_LINK_LST_APPEND) == FALSE)
            {
                return FALSE;
            }
            else
            {
                if(COS_VM_BindMarkerEntry(ifindex, cos_entry) == FALSE)
                {
                    L_LINK_LST_Delete(&cos_vm_tbl[ifindex], cos_entry);
                    return FALSE;
                }
                else
                {
                    return TRUE;
                }
            }

            break;


        case L_RSTATUS_NOTEXIST_2_NOTREADY:
            temp.row_status=VAL_cosEntryStatus_notReady;
            if(row_status == VAL_cosEntryStatus_createAndGo)
            {
                return FALSE;
            }
            if(row_status == VAL_cosEntryStatus_createAndWait)
            {
                ret=L_LINK_LST_Set(&cos_vm_tbl[ifindex], &temp, L_LINK_LST_APPEND);
            }
            if(row_status == L_RSTATUS_SET_OTHER_COLUMN)
            {
                ret=L_LINK_LST_Set(&cos_vm_tbl[ifindex], &temp, L_LINK_LST_APPEND);
            }

            return ret;

        break;

        case L_RSTATUS_ACTIVE_2_NOTEXIST:
            COS_VM_DelCosEntryBinding(ifindex, &temp);
            L_LINK_LST_Delete(&cos_vm_tbl[ifindex], &temp);
            return TRUE;


        case L_RSTATUS_ACTIVE_2_ACTIVE:
            /* once in active , can't modiy it
             */
            if(row_status == L_RSTATUS_SET_OTHER_COLUMN)
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }


        case L_RSTATUS_ACTIVE_2_NOTREADY:
            /* once in active , can't modiy it
             */

            return FALSE;



        case L_RSTATUS_NOTREADY_2_NOTEXIST:
            L_LINK_LST_Delete(&cos_vm_tbl[ifindex], &temp);
            return TRUE;



        case L_RSTATUS_NOTREADY_2_ACTIVE:

             /* check if ace is conflict with acl
              */
            if(COS_VM_GetAclType(cos_entry->acl_name, &acl_type) == FALSE)
            {
                return FALSE;
            }

            temp.acl_type=acl_type;
            temp.row_status=VAL_cosEntryStatus_active;
            if(COS_VM_IsConflictAclInInterface(ifindex, &temp) == TRUE)
            {
                return FALSE;
            }

            if(COS_VM_BindMarkerEntry(ifindex, &temp) == FALSE)
            {
                return FALSE;
            }
            else
            {
                L_LINK_LST_Set(&cos_vm_tbl[ifindex], &temp, L_LINK_LST_APPEND);
                return TRUE;
            }

            break;


        case L_RSTATUS_NOTREADY_2_NOTREADY:
            if((row_status==VAL_aclIpAceStatus_createAndGo) ||
               (row_status==VAL_aclIpAceStatus_active)  ||
               (row_status==VAL_aclIpAceStatus_notInService))
            {
                return FALSE;
            }

            if(row_status == L_RSTATUS_SET_OTHER_COLUMN)
            {
                temp.row_status = VAL_cosEntryStatus_notReady;
                L_LINK_LST_Set(&cos_vm_tbl[ifindex], &temp, L_LINK_LST_APPEND);
                return TRUE;
            }

            break;

        case L_RSTATUS_TRANSITION_STATE_ERROR:
            return FALSE;

        default:
            return FALSE;


    }

    return TRUE;
}
#endif


/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetCosEntryRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to set the remark entry
 * INPUT:    (ifindex, acl_name) is the key
 *           row_status
 * OUTPUT :  None
 * RETURN:   TRUE/FALSE
 * NOTE:     once in active state, only can be destoryed
 *           cos_entry is valid only the row_status=VAL_markerEntryStatus_createAndGo
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetCosEntryRowStatus(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
#if((SYS_CPNT_ACL == TRUE) && (SYS_CPNT_COS_CONTAIN_CLI_MAP_ACCESS_LIST == TRUE))
    UI32_T port_type, unit, port, trunk_id;

    port_type=SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);
    if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
    {
        return FALSE;
    }

    if(strlen(cos_entry->acl_name) > SYS_ADTP_LENGTH_OF_ACL_NAME)
    {
        return FALSE;
    }


    return COS_VM_FSM(ifindex, cos_entry, cos_entry->row_status);
#else
    return FALSE;
#endif


}



/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_InitDefaultEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  to get the initial marker entry
 * INPUT:
 * OUTPUT:   cos_entry
 * RETURN:   TRUE/FALSE
 * NOTE:     don't care the acl_name and row_status field
 *------------------------------------------------------------------------------
 */

#if ((COS_VM_BACKDOOR == TRUE) || (SYS_CPNT_COS_CONTAIN_CLI_MAP_ACCESS_LIST == TRUE))
static void COS_VM_InitDefaultEntry(COS_TYPE_AclCosEntry_T *cos_entry)
{
    cos_entry->row_status=VAL_cosEntryStatus_active;
    cos_entry->cos=0;

}
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetCosEntry_Cos
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to set the remark entry
 * INPUT:    ifindex          -- is one of the search key
 *           cos_entry  --
 *
 * OUTPUT :
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (intf, cos_entry->acl_name)
 *           SNMP do not need to care of cos_entry->acl_type
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
#if((SYS_CPNT_ACL == TRUE) && (SYS_CPNT_COS_CONTAIN_CLI_MAP_ACCESS_LIST == TRUE))
    UI32_T port_type, unit, port, trunk_id;

    port_type=SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);
    if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
    {
        return FALSE;
    }

    if(strlen(cos_entry->acl_name) > SYS_ADTP_LENGTH_OF_ACL_NAME)
    {
        return FALSE;
    }

    if(COS_VM_CosEntrySyntaxCheck(cos_entry) == FALSE)
    {
        return FALSE;
    }


    return COS_VM_FSM(ifindex, cos_entry, L_RSTATUS_SET_OTHER_COLUMN);
#else
    return FALSE;
#endif

}



/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SNMPGetNextMarkerEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get the next ACL binding to the inteface performaned remark function
 * INPUT:    ifindex     -- logical port
 *           cos_entry.acl_name
 * OUTPUT:   ifindex
 *           cos_entry
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (*ifindex, mark_entry->acl_name)
 *           use the *ifindex=0, cos_entry->acl_name='\0' to get the frist entry
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SNMPGetNextMarkerEntry(UI32_T *ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
#if(SYS_CPNT_ACL == TRUE)
    UI32_T port_type, unit, port, trunk_id;
    UI32_T                    i;

    if(strlen(cos_entry->acl_name) > SYS_ADPT_ACL_MAX_NAME_LEN)
    {
        return FALSE;
    }


    /* to get the first entry
     */
    if((strlen(cos_entry->acl_name) == 0) && (*ifindex == 0))
    {
        for(i= (*ifindex) + 1; i <= SYS_ADPT_TOTAL_NBR_OF_LPORT; i++)
        {
            port_type=SWCTRL_LogicalPortToUserPort(i, &unit, &port, &trunk_id);
            if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
            {
                continue;
            }

            if(L_LINK_LST_Get_1st(&cos_vm_tbl[i], cos_entry) == TRUE)
            {
                return TRUE;
            }
        }
        return FALSE;

    }

    /* check port type
     */

    port_type=SWCTRL_LogicalPortToUserPort(*ifindex, &unit, &port, &trunk_id);
    if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
    {
        return FALSE;
    }


    /*
     in the same interface
     */
    if(L_LINK_LST_Get(&cos_vm_tbl[*ifindex], cos_entry) == TRUE)
    {
        if(L_LINK_LST_Get_Next(&cos_vm_tbl[*ifindex], cos_entry) == TRUE)
        {
            return TRUE;
        }
        else
        {
            /*
            in the next interface
            */
            for(i= (*ifindex) + 1; i <= SYS_ADPT_TOTAL_NBR_OF_LPORT; i++)
            {
                port_type=SWCTRL_LogicalPortToUserPort(i, &unit, &port, &trunk_id);
                if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
                {
                    continue;
                }


                if(L_LINK_LST_Get_1st(&cos_vm_tbl[i], cos_entry) == TRUE)
                {
                    *ifindex=i;
                    return TRUE;
                }

            }

            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
#else
    return FALSE;
#endif

}




/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_AddAceCallback
 *------------------------------------------------------------------------------
 * PURPOSE:  call by ACL_VM to let COS_VM know a ace is going to added
 *           and ask filter to check resouce
 *           create ace
 * INPUT  :  rid, acl_name, ace
 * OUTPUT :  None
 * RETURN :  TRUE/FALSE
 * NOTES  :  1. before call this function, this ace must be added to ACL_OM first
 *------------------------------------------------------------------------------
 */
#if((SYS_CPNT_ACL == TRUE) && (SYS_CPNT_COS_CONTAIN_CLI_MAP_ACCESS_LIST == TRUE))
static BOOL_T COS_VM_AddAceCallback(UI32_T rid, UI32_T acl_type, UI8_T acl_name[], ACL_Ace_Entry_T *ace)
{


    I32_T           i ;
    COS_TYPE_AclCosEntry_T cos_entry;

    strcpy(cos_entry.acl_name, acl_name);
    for(i=1; i <= SYS_ADPT_TOTAL_NBR_OF_LPORT ; i++)
    {
        if(L_LINK_LST_Get(&cos_vm_tbl[i], &cos_entry) == TRUE)
        {
            if(cos_entry.row_status == VAL_cosEntryStatus_active)
            {
                if(COS_VM_IsConflictMarkerAceInInterface(i, ace, &cos_entry) == FALSE)
                {
                    if(COS_VM_ReserveAce(rid, i, ace, &cos_entry) == FALSE)
                    {
                        return FALSE;
                    }

                }
            }
        }


   }

   return TRUE;

}



/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_DelAceCallback
 *------------------------------------------------------------------------------
 * PURPOSE:  call by ACL_VM to let COS_VM  know a ace is going to added
 *           and ask filter to delete a ace from hardware
 *           create ace
 * INPUT  :  None
 * OUTPUT :  None
 * RETURN :  TRUE/FALSE
 * NOTES  :
 *------------------------------------------------------------------------------
 */
static BOOL_T COS_VM_DelAceCallback(UI32_T acl_type, UI8_T acl_name[], ACL_Ace_Entry_T *ace, BOOL_T is_last)
{
    I32_T           i ;
    COS_TYPE_AclCosEntry_T cos_entry;

    strcpy(cos_entry.acl_name, acl_name);
    for(i=1; i <= SYS_ADPT_TOTAL_NBR_OF_LPORT ; i++)
    {
        if(L_LINK_LST_Get(&cos_vm_tbl[i], &cos_entry) == TRUE)
        {
            /* ACL only pass the active ACE
             */
            if(cos_entry.row_status == VAL_cosEntryStatus_active)
            {
                COS_VM_ReleaseAce(i, acl_type, ace, &cos_entry);


            }

            if(is_last == TRUE)
            {
                L_LINK_LST_Delete(&cos_vm_tbl[i], &cos_entry);
            }

        }


   }

   return TRUE;

}


/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_DelAclCallback
 *------------------------------------------------------------------------------
 * PURPOSE:  call by ACL_VM to let COS_VM know a  acl is going to be deleted
 *           create ace
 * INPUT  :  acl_name
 * OUTPUT :  None
 * RETURN :  TRUE
 * NOTES  :
 *------------------------------------------------------------------------------
 */
static BOOL_T COS_VM_DelAclCallback(UI8_T acl_name[])
{

    I32_T           i ;
    COS_TYPE_AclCosEntry_T cos_entry;

    strcpy(cos_entry.acl_name, acl_name);
    for(i=1; i <= SYS_ADPT_TOTAL_NBR_OF_LPORT ; i++)
    {
        if(L_LINK_LST_Get(&cos_vm_tbl[i], &cos_entry) == TRUE)
        {
            if(cos_entry.row_status == VAL_cosEntryStatus_active)
            {
                COS_VM_DelCosEntryBinding(i, &cos_entry);

            }

            L_LINK_LST_Delete(&cos_vm_tbl[i], &cos_entry);

        }


   }

   return TRUE;

}





/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_IsAclBindToIngressIntf
 *------------------------------------------------------------------------------
 * PURPOSE:  check if any acl bind to interface
 * INPUT  :  acl_name, acl_type
 * OUTPUT :  None
 * RETURN :  TRUE
 * NOTES  :
 *------------------------------------------------------------------------------
 */
static BOOL_T COS_VM_IsAclBindToIngressIntf(UI8_T acl_name[])
{
    I32_T           i ;
    COS_TYPE_AclCosEntry_T cos_entry;

    strcpy(cos_entry.acl_name, acl_name);
    for(i=1; i <= SYS_ADPT_TOTAL_NBR_OF_LPORT ; i++)
    {
        if(L_LINK_LST_Get(&cos_vm_tbl[i], &cos_entry) == TRUE)
        {
            if(cos_entry.row_status == VAL_cosEntryStatus_active)
            {
                return TRUE;

            }


        }
   }

   return FALSE;
}
#endif


#if(COS_VM_BACKDOOR == TRUE)

void COS_VM_BackDoor_Menu()
{
    UI8_T   ch;
    UI8_T   in_str[32];
    UI32_T  max_len=32;
    I32_T   ifindex;
    COS_TYPE_AclCosEntry_T cos_entry;
    UI32_T  port_type;
    UI32_T  unit, port;
    L_THREADGRP_Handle_T tg_handle;
    UI32_T backdoor_member_id;

    /* Join thread group
     */
    tg_handle =(L_THREADGRP_Handle_T) L2_L4_PROC_COMM_GetL4GroupTGHandle();
    if(L_THREADGRP_Join(tg_handle, SYS_BLD_BACKDOOR_THREAD_PRIORITY, &backdoor_member_id)==FALSE)
    {
        printf("%s: L_THREADGRP_Join fail.\n", __FUNCTION__);
        return;
    }

    while ( 1 )
    {
        BACKDOOR_MGR_Printf("\n");

        BACKDOOR_MGR_Printf (" 1. map access-list ip  \n");
        BACKDOOR_MGR_Printf (" 2  map access-list mac  \n");
        BACKDOOR_MGR_Printf (" 3  no map access-list ip  \n");
        BACKDOOR_MGR_Printf (" 4. no map access-list mac \n");
        BACKDOOR_MGR_Printf (" 5. show map access-list \n");
        BACKDOOR_MGR_Printf (" g. get map access-list \n");
        BACKDOOR_MGR_Printf (" n. get next map access-list \n");
        BACKDOOR_MGR_Printf (" 6. snmp show access-list \n");
        BACKDOOR_MGR_Printf (" 7. set entry row_status \n");
        BACKDOOR_MGR_Printf (" 8. set tos \n");

        BACKDOOR_MGR_Printf (" x, exit \n");
        BACKDOOR_MGR_Printf ("    select =");

        COS_VM_InitDefaultEntry(&cos_entry);
        unit=1;
        BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
        ch=in_str[0];
        switch (ch)
        {

            case '1' :
                BACKDOOR_MGR_Printf("unit =");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                if(strlen(in_str) != 0)
                {
                    unit=atoi(in_str);
                }
                BACKDOOR_MGR_Printf("port=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                port=atoi(in_str);

                port_type=SWCTRL_UserPortToLogicalPort(unit, port, &ifindex);
                if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
                {
                    break;
                }



                BACKDOOR_MGR_Printf(" acl_name=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                strcpy(cos_entry.acl_name, in_str);

                BACKDOOR_MGR_Printf("cos=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                cos_entry.cos=atoi(in_str);

                cos_entry.acl_type= RULE_TYPE_IP_ACL;
                if(COS_VM_AddCosEntry(ifindex, &cos_entry) == FALSE)
                {
                    BACKDOOR_MGR_Printf(" fail ..");
                }

                break;

            case '2':
                BACKDOOR_MGR_Printf("unit =");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                if(strlen(in_str) != 0)
                {
                    unit=atoi(in_str);
                }
                BACKDOOR_MGR_Printf("port=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                port=atoi(in_str);

                port_type=SWCTRL_UserPortToLogicalPort(unit, port, &ifindex);
                if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
                {
                    break;
                }



                BACKDOOR_MGR_Printf(" acl_name=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                strcpy(cos_entry.acl_name, in_str);

                BACKDOOR_MGR_Printf("cos=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                cos_entry.cos=atoi(in_str);


                cos_entry.acl_type= RULE_TYPE_MAC_ACL;
                if(COS_VM_AddCosEntry(ifindex, &cos_entry) == FALSE)
                {
                    BACKDOOR_MGR_Printf(" fail ..");
                }

                break;

            case '3':
                BACKDOOR_MGR_Printf("unit =");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                if(strlen(in_str) != 0)
                {
                    unit=atoi(in_str);
                }
                BACKDOOR_MGR_Printf("port=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                port=atoi(in_str);

                port_type=SWCTRL_UserPortToLogicalPort(unit, port, &ifindex);
                if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
                {
                    break;
                }



                BACKDOOR_MGR_Printf(" acl_name=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                strcpy(cos_entry.acl_name, in_str);

                BACKDOOR_MGR_Printf("cos=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                cos_entry.cos=atoi(in_str);


                cos_entry.acl_type= RULE_TYPE_IP_ACL;
                if(COS_VM_DelCosEntry(ifindex, &cos_entry) == FALSE)
                {
                    BACKDOOR_MGR_Printf(" fail ..");
                }

                break;

            case '4':
               BACKDOOR_MGR_Printf("unit =");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                if(strlen(in_str) != 0)
                {
                    unit=atoi(in_str);
                }
                BACKDOOR_MGR_Printf("port=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                port=atoi(in_str);

                port_type=SWCTRL_UserPortToLogicalPort(unit, port, &ifindex);
                if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
                {
                    break;
                }



                BACKDOOR_MGR_Printf(" acl_name=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                strcpy(cos_entry.acl_name, in_str);

                BACKDOOR_MGR_Printf("cos=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                cos_entry.cos=atoi(in_str);


                cos_entry.acl_type= RULE_TYPE_MAC_ACL;
                if(COS_VM_DelCosEntry(ifindex, &cos_entry) == FALSE)
                {
                    BACKDOOR_MGR_Printf(" fail ..");
                }

                break;

            case '5':

                BACKDOOR_MGR_Printf("unit =");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                if(strlen(in_str) != 0)
                {
                    unit=atoi(in_str);
                }
                BACKDOOR_MGR_Printf("port=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                port=atoi(in_str);
                port_type=SWCTRL_UserPortToLogicalPort(unit, port, &ifindex);

                cos_entry.acl_name[0]='\0';
                while(COS_VM_GetNextCosEntry(ifindex, &cos_entry) == TRUE)
                {

                    BACKDOOR_MGR_Printf("map access_list ");
                    if(cos_entry.acl_type == RULE_TYPE_IP_ACL)
                    {
                        BACKDOOR_MGR_Printf("ip %s ", cos_entry.acl_name);
                    }
                    else
                    {
                        BACKDOOR_MGR_Printf("map %s ", cos_entry.acl_name);
                    }

                    BACKDOOR_MGR_Printf(" cos %ld", cos_entry.cos);

                    BACKDOOR_MGR_Printf(" \n");
                }

                break;

            case '6' :

                ifindex=0;
                cos_entry.acl_name[0]='\0';
                while(COS_VM_SNMPGetNextCosEntry(&ifindex, &cos_entry) == TRUE)
                {

                    BACKDOOR_MGR_Printf("ifindex %ld ", ifindex);
                    if(cos_entry.acl_type == RULE_TYPE_IP_ACL)
                    {
                        BACKDOOR_MGR_Printf("ip %s ", cos_entry.acl_name);
                    }
                    else
                    {
                        BACKDOOR_MGR_Printf("map %s ", cos_entry.acl_name);
                    }

                    BACKDOOR_MGR_Printf(" cos %ld", cos_entry.cos);

                    BACKDOOR_MGR_Printf(" \n");
                }

                break;


            case '7':

                BACKDOOR_MGR_Printf("unit =");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                if(strlen(in_str) != 0)
                {
                    unit=atoi(in_str);
                }
                BACKDOOR_MGR_Printf("port=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                port=atoi(in_str);
                port_type=SWCTRL_UserPortToLogicalPort(unit, port, &ifindex);

                BACKDOOR_MGR_Printf("acl_name =");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                strcpy(cos_entry.acl_name, in_str);
                BACKDOOR_MGR_Printf("row status, active=1, notInService=2,  notReady=3, createAndGo=4 ,createAndWait=5, destroy=6 ");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                cos_entry.row_status=atoi(in_str);
                if(cos_entry.row_status == 4)
                {
                    BACKDOOR_MGR_Printf("cos=");
                    BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                    cos_entry.cos=atoi(in_str);


                }

                if(COS_VM_SetCosEntryRowStatus(ifindex,  &cos_entry) == FALSE)
                {
                    BACKDOOR_MGR_Printf("fail \n");
                }

                break;


            case '8':
                BACKDOOR_MGR_Printf("unit =");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                if(strlen(in_str) != 0)
                {
                    unit=atoi(in_str);
                }
                BACKDOOR_MGR_Printf("port=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                port=atoi(in_str);
                port_type=SWCTRL_UserPortToLogicalPort(unit, port, &ifindex);

                BACKDOOR_MGR_Printf("acl_name =");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                strcpy(cos_entry.acl_name, in_str);
                BACKDOOR_MGR_Printf(" cos ");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                cos_entry.cos=atoi(in_str);
                if(COS_VM_SetCosEntry(ifindex,  &cos_entry) == FALSE)
                {
                    BACKDOOR_MGR_Printf("fail \n");

                }

                break;

            case 'g':
                BACKDOOR_MGR_Printf("unit =");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                if(strlen(in_str) != 0)
                {
                    unit=atoi(in_str);
                }
                BACKDOOR_MGR_Printf("port=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                port=atoi(in_str);

                port_type=SWCTRL_UserPortToLogicalPort(unit, port, &ifindex);
                if((port_type != SWCTRL_LPORT_NORMAL_PORT) && (port_type != SWCTRL_LPORT_TRUNK_PORT_MEMBER))
                {
                    break;
                }



                BACKDOOR_MGR_Printf(" acl_name=");
                BACKDOOR_MGR_RequestKeyIn(in_str,max_len);
                strcpy(cos_entry.acl_name, in_str);




                if(COS_VM_GetCosEntry(ifindex, &cos_entry) == FALSE)
                {
                    BACKDOOR_MGR_Printf(" fail ..");
                }
                else
                {
                    BACKDOOR_MGR_Printf("ifindex %ld ", ifindex);
                    if(cos_entry.acl_type == RULE_TYPE_IP_ACL)
                    {
                        BACKDOOR_MGR_Printf("ip %s ", cos_entry.acl_name);
                    }
                    else
                    {
                        BACKDOOR_MGR_Printf("mac %s ", cos_entry.acl_name);
                    }

                    BACKDOOR_MGR_Printf(" cos %ld", cos_entry.cos);

                    BACKDOOR_MGR_Printf(" \n");
                }

                break;

            case 'n':
                ifindex=0;
                cos_entry.acl_name[0]='\0';
                while(COS_VM_SNMPGetNextCosEntry(&ifindex, &cos_entry) == TRUE)
                {

                    BACKDOOR_MGR_Printf("map access_list ");
                    if(cos_entry.acl_type == RULE_TYPE_IP_ACL)
                    {
                        BACKDOOR_MGR_Printf("ip %s ", cos_entry.acl_name);
                    }
                    else
                    {
                        BACKDOOR_MGR_Printf("mac %s ", cos_entry.acl_name);
                    }

                    BACKDOOR_MGR_Printf(" cos %ld", cos_entry.cos);

                    BACKDOOR_MGR_Printf(" \n");
                }

                break;

            case 'x':
                goto exit;
        }

    }
    exit:
    L_THREADGRP_Leave(tg_handle, backdoor_member_id);
}
#endif
/*------------------------------------------------------------------------------
 * FUNCTION NAME - COS_VM_HandleHotInsertion
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will initialize the port OM of the module ports
 *            when the option module is inserted.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 *            use_default           -- the flag indicating the default
 *                                     configuration is used without further
 *                                     provision applied; TRUE if a new module
 *                                     different from the original one is
 *                                     inserted
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only one module is inserted at a time.
 *------------------------------------------------------------------------------
 */
void COS_VM_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default)
{
#if 0 /* DanXie, Thursday, August 14, 2008 10:13:52 */
    SetHotInsertTosMapTableToDefault(starting_port_ifindex, number_of_port, use_default);
    SetHotInsertDscpMapTableToDefault(starting_port_ifindex, number_of_port, use_default);
    SetHotInsertTcpPortMapTableToDefault(starting_port_ifindex, number_of_port, use_default);

    /*
    ConfigAsicDscpDefaultMap();
    ConfigAsicTosDefaultMap();
    */

#if(SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_COS == TRUE)
    SetHotInsertTosMapTable(starting_port_ifindex, number_of_port);
#endif
#if (SYS_CPNT_COS_ING_DSCP_TO_COS == TRUE)
    SetHotInsertDscpMapTable(starting_port_ifindex, number_of_port);
#endif
#if(SYS_CPNT_COS_ING_IP_PORT_TO_COS == TRUE)
    SetHotInsertTcpPortMapTable(starting_port_ifindex, number_of_port);
#endif
#endif /* #if 0 */

    SetAllUserPortListToDefaultState(starting_port_ifindex,
                                     starting_port_ifindex + number_of_port - 1,
                                     TRUE);

    return;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - COS_VM_HandleHotRemoval
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will clear the port OM of the module ports when
 *            the option module is removed.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     removed
 *            number_of_port        -- the number of ports on the removed
 *                                     module
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only one module is removed at a time.
 *------------------------------------------------------------------------------
 */
void COS_VM_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port)
{
    int i,match = 0;
    UI32_T unit;
    per_port_l4_dscp_t   *data;
    struct L_listnode *node,*temp;
#if 0 /* DanXie, Thursday, August 14, 2008 10:23:29 */
    SetHotRemoveTosMapTableToDefault(starting_port_ifindex, number_of_port);
    SetHotRemoveDscpMapTableToDefault(starting_port_ifindex, number_of_port);
    SetHotRemoveTcpPortMapTableToDefault(starting_port_ifindex, number_of_port);
#endif /* #if 0 */

    SetAllUserPortListToDefaultState(starting_port_ifindex,
                                     starting_port_ifindex + number_of_port - 1,
                                     FALSE);

    unit = COS_VM_IFINDEX_TO_UNIT(starting_port_ifindex);

    if(NULL == TcpUdpMaplist)
        return ;

    node = TcpUdpMaplist->head;
    while(node)
    {
        data = (per_port_l4_dscp_t *) node->data;
        for(i = starting_port_ifindex; i < starting_port_ifindex + number_of_port; i++)
        {
            if(i == data->ifindex)
            {
              match = 1;
              if (node->prev)
                node->prev->next = node->next;
              else
                TcpUdpMaplist->head = node->next;

              if (node->next)
                node->next->prev = node->prev;
              else
                TcpUdpMaplist->tail = node->prev;

              TcpUdpMaplist->count--;
              TcpUdpMap_counter[unit-1]--;
              temp = node;
              node = node->next;
              free(temp->data);
              free(temp);
              break;
            }
        }
        if (match == 0)
            node = node->next;
        match = 0;
    }
    return;
}

#if 0
/*------------------------------------------------------------------------------
 * ROUTINE NAME  - SetHotInsertTosMapTableToDefault
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will initialize the port OM of the module ports
 *            when the option module is inserted.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 *            use_default           -- the flag indicating the default
 *                                     configuration is used without further
 *                                     provision applied; TRUE if a new module
 *                                     different from the original one is
 *                                     inserted
 * OUTPUT: None
 * RETURN: None
 * NOTES : This is local function and only will be called by
 *         COS_VM_EnterMasterMode()
 *------------------------------------------------------------------------------
 */
static void SetHotInsertTosMapTableToDefault(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default)
{
    UI32_T  i, ip_precedence;

    for (i = 0; i < number_of_port; i++)
    {
        for (ip_precedence = 0; COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES > ip_precedence;
            ip_precedence++)
        {
            if (FALSE == COS_OM_SetIpPrecedence2CosValueByLport(starting_port_ifindex + i,
                ip_precedence, DEFAULT_TOS_MAP[ip_precedence]))
            {
                return;
            }
        }
    }
    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - SetHotInsertDscpMapTableToDefault
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will initialize the port OM of the module ports
 *            when the option module is inserted.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 *            use_default           -- the flag indicating the default
 *                                     configuration is used without further
 *                                     provision applied; TRUE if a new module
 *                                     different from the original one is
 *                                     inserted
 * OUTPUT: None
 * RETURN: None
 * NOTES : This is local function and only will be called by
 *         COS_VM_EnterMasterMode()
 *------------------------------------------------------------------------------
 */
static void SetHotInsertDscpMapTableToDefault(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default)
{
    UI32_T  i, ip_dscp;

    for (i=starting_port_ifindex-1; i < starting_port_ifindex+number_of_port; i++)
    {
        for (ip_dscp = 0; COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES > ip_dscp; ip_dscp++)
        {
            if (FALSE == COS_OM_SetIpDscp2CosValueByLport(i + 1, ip_dscp, DEFAULT_DSCP_MAP[ip_dscp]))
                return;
        }

        memcpy(dscp_map_status_table[i], DEFAULT_DSCP_STATUS, COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES);

    }
    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - SetHotInsertTcpPortMapTableToDefault
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will initialize the port OM of the module ports
 *            when the option module is inserted.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 *            use_default           -- the flag indicating the default
 *                                     configuration is used without further
 *                                     provision applied; TRUE if a new module
 *                                     different from the original one is
 *                                     inserted
 * OUTPUT: None
 * RETURN: None
 * NOTES : This is local function and only will be called by
 *         COS_VM_EnterMasterMode()
 *------------------------------------------------------------------------------
 */
static void SetHotInsertTcpPortMapTableToDefault(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default)
{
    UI32_T                      i, j;
    COS_VM_PortConfigStatus_T   *port_config_status;

    for (i=starting_port_ifindex-1; i < starting_port_ifindex+number_of_port; i++)
    {
        port_config_status = COS_VM_GetPortConfigStatus(i + 1); /* to 1-based */
        if (NULL == port_config_status)
            return;

        memset(port_config_status, 0, sizeof(COS_VM_PortConfigStatus_T));

        for (j = 0; MAX_TCP_PORT_MAP_ENTRIES > j; ++j)
        {
            tcp_port_map_table[i][j].tcp_port = COS_VM_UNDEF_TCP_PORT;
            tcp_port_map_table[i][j].cos = COS_VM_UNDEF_COS_VALUE;
            tcp_port_map_table[i][j].mapping_index = 0;
        }
    }

    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - SetHotInsertTosMapTable
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will configure the port OM of the module ports with
 *             current mapping when the option module is inserted.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 * OUTPUT: None
 * RETURN: None
 * NOTES : This is local function and only will be called by
 *         COS_VM_EnterMasterMode()
 *------------------------------------------------------------------------------
 */
static void SetHotInsertTosMapTable(UI32_T starting_port_ifindex, UI32_T number_of_port)
{
    UI32_T i, l_port, next_l_port, cos;
    UI8_T tos;
    UI32_T count = 0;
    COS_VM_LocalTosToCOSEntry_T  *table;
    l_port = 0;
    cos = 0;
    next_l_port = 0;

    table = calloc(COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES, sizeof(COS_VM_LocalTosToCOSEntry_T));
    if(NULL == table)
        return;

    SWCTRL_GetNextLogicalPort(&l_port);

    while (COS_VM_GetNextIpPrecedenceToCos(&next_l_port, &tos, &cos) == TRUE)
    {
        if(next_l_port != l_port)
            break;
        table[count].tos = tos;
        table[count].cos = cos;
        count++;

#if(SYS_CPNT_COS_PER_PORT == TRUE)
        for(i = starting_port_ifindex; i< starting_port_ifindex + number_of_port; i++)
        {
            COS_VM_SetIpPrecedenceToCos(i, tos, cos);
        }
#endif
    }

#if(SYS_CPNT_COS_PER_PORT == FALSE)
    SetAllTosMapTableToDefault();
    for(i = 0; i < count; i++)
    {
        SWCTRL_SetPortTosCosMap(l_port, table[i].tos, table[i].cos);

        if (DEFAULT_TOS_MAP[table[i].tos] != table[i].cos)
        {
           COS_VM_SetIpPrecedenceToCos(l_port, table[i].tos, table[i].cos);
        }
    }
#endif
#if(SYS_CPNT_3COM_CLI == FALSE)
    switch(tos_or_dscp_mapping)
    {
    case COS_VM_IPPRECEDENCE_MAPPING_ENABLED:
        break;

    case COS_VM_DISABLE_BOTH:
    case COS_VM_DSCP_MAPPING_ENABLED:
    default:
        SWCTRL_DisableTosCosMap();
        break;
    }
#endif
    free(table);

    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - SetHotInsertDscpMapTable
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will configure the port OM of the module ports with
 *            current mapping when the option module is inserted.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 * OUTPUT: None
 * RETURN: None
 * NOTES : This is local function and only will be called by
 *         COS_VM_EnterMasterMode()
 *------------------------------------------------------------------------------
 */
static void SetHotInsertDscpMapTable(UI32_T starting_port_ifindex, UI32_T number_of_port)
{
    UI32_T i, l_port, next_l_port, cos;
    UI8_T dscp;
    UI32_T count = 0;
    COS_VM_LocalDscpToCOSEntry_T  *table;
    l_port = 0;
    cos = 0;
    next_l_port = 0;

    table = calloc(COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES, sizeof(COS_VM_LocalDscpToCOSEntry_T));
    if(NULL == table)
        return;

    SWCTRL_GetNextLogicalPort(&l_port);

    while (COS_VM_GetNextDSCPToCos(&next_l_port, &dscp, &cos) == TRUE)
    {
        if(next_l_port != l_port)
            break;

        table[count].dscp = dscp;
        table[count].cos = cos;
        table[count].status = dscp_map_status_table[l_port-1][dscp];
        count++;

#if(SYS_CPNT_COS_PER_PORT == TRUE)
        for(i = starting_port_ifindex; i< starting_port_ifindex + number_of_port; i++)
        {
            COS_VM_SetDSCPToCos(i, dscp, cos);
        }
#endif
    }

#if(SYS_CPNT_COS_PER_PORT == FALSE)
    SetAllDscpMapTableToDefault();
    for(i = 0; i < count; i++)
    {
        if (DEFAULT_DSCP_MAP[table[i].dscp] != table[i].cos || table[i].status == TRUE)
        {
            COS_VM_SetDSCPToCos(l_port, table[i].dscp, table[i].cos);
            COS_VM_SetDSCPToCosStatus(l_port, table[i].dscp, table[i].status);
        }
    }
#endif
#if (SYS_CPNT_3COM_CLI == FALSE)
    switch(tos_or_dscp_mapping)
    {
    case COS_VM_DSCP_MAPPING_ENABLED:
        break;

    case COS_VM_DISABLE_BOTH:
    case COS_VM_IPPRECEDENCE_MAPPING_ENABLED:
    default:
        SWCTRL_DisableDscpCosMap();
        break;
    }
#endif
    free(table);

    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - SetHotInsertTcpPortMapTable
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will initialize the port OM of the module ports with
 *            current mapping when the option module is inserted.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 * OUTPUT: None
 * RETURN: None
 * NOTES : This is local function and only will be called by
 *         COS_VM_EnterMasterMode()
 *------------------------------------------------------------------------------
 */
static void SetHotInsertTcpPortMapTable(UI32_T starting_port_ifindex, UI32_T number_of_port)
{
    UI32_T  i, l_port, next_l_port, cos;
    I32_T   tcp_port;
    UI32_T  count = 0;
    COS_VM_LocalPortToCOSEntry_T  *table;

    l_port = 0;
    cos = 0;
    next_l_port = 0;

    table = calloc(MAX_TCP_PORT_MAP_ENTRIES,sizeof(COS_VM_LocalPortToCOSEntry_T));
    if(NULL == table)
        return;

    SWCTRL_GetNextLogicalPort(&l_port);

    while (COS_VM_GetNextPortToCos(&next_l_port, &tcp_port, &cos) == TRUE)
    {
        if (next_l_port != l_port)
            break;
        table[count].port=tcp_port;
        table[count].cos=cos;
        count++;
#if (SYS_CPNT_COS_PER_PORT == TRUE)
        for (i=starting_port_ifindex; i < starting_port_ifindex+number_of_port; i++)
        {   /* temp testing */
            RULE_MGR_SetTcpPortCosMap(1, i, tcp_port, cos, 1/*mapping_index*/);
            /* SWCTRL_SetPortTcpPortCosMap(i, tcp_port, cos); */
        }
#endif
    }
#if (SYS_CPNT_COS_PER_PORT == FALSE)
    SetAllTcpPortMapTableToDefault();
    for (i=0;i<count;i++)
    {
        COS_VM_SetPortToCos(l_port, table[i].port, table[i].cos);
    }
#endif
#if (SYS_CPNT_3COM_CLI == FALSE)
    switch(tcp_port_mapping)
    {
    case COS_VM_MAPPING_ENABLED:
        COS_VM_SetAllPortMapToASIC(TRUE);
        break;

    case COS_VM_MAPPING_DISABLED:
    default:
        break;
    }
#endif
    free(table);

    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - SetHotRemoveTosMapTableToDefault
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will initialize the port OM of the module ports
 *            when the option module is inserted.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 * OUTPUT: None
 * RETURN: None
 * NOTES : This is local function and only will be called by
 *         COS_VM_EnterMasterMode()
 *------------------------------------------------------------------------------
 */
static void SetHotRemoveTosMapTableToDefault(UI32_T starting_port_ifindex, UI32_T number_of_port)
{
    UI32_T  i, ip_precedence;

    for (i = 0; i < number_of_port; i++)
    {
        for (ip_precedence = 0; COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES > ip_precedence;
            ip_precedence++)
        {
            if (FALSE == COS_OM_SetIpPrecedence2CosValueByLport(starting_port_ifindex + i,
                ip_precedence, DEFAULT_TOS_MAP[ip_precedence]))
            {
                return;
            }
        }
    }
    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - SetHotRemoveDscpMapTableToDefault
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will initialize the port OM of the module ports
 *            when the option module is inserted.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 * OUTPUT: None
 * RETURN: None
 * NOTES : This is local function and only will be called by
 *         COS_VM_EnterMasterMode()
 *------------------------------------------------------------------------------
 */
static void SetHotRemoveDscpMapTableToDefault(UI32_T starting_port_ifindex, UI32_T number_of_port)
{
    UI32_T  i, ip_dscp;

    for (i=starting_port_ifindex-1; i < starting_port_ifindex+number_of_port; i++)
    {
        for (ip_dscp = 0; COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES > ip_dscp; ++ip_dscp)
        {
            if (FALSE == COS_OM_SetIpDscp2CosValueByLport(i + 1, ip_dscp, DEFAULT_DSCP_MAP[ip_dscp]))
                return;
        }

        memcpy(dscp_map_status_table[i], DEFAULT_DSCP_STATUS, COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES);

    }
    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - SetHotRemoveTcpPortMapTableToDefault
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will initialize the port OM of the module ports
 *            when the option module is inserted.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 * OUTPUT: None
 * RETURN: None
 * NOTES : This is local function and only will be called by
 *         COS_VM_EnterMasterMode()
 *------------------------------------------------------------------------------
 */
static void SetHotRemoveTcpPortMapTableToDefault(UI32_T starting_port_ifindex, UI32_T number_of_port)
{
    UI32_T                      i, j;
    COS_VM_PortConfigStatus_T   *port_config_status;

    for (i=starting_port_ifindex-1; i < starting_port_ifindex+number_of_port; i++)
    {
        port_config_status = COS_VM_GetPortConfigStatus(i + 1); /* to 1-based */
        if (NULL == port_config_status)
            return;

        memset(port_config_status, 0, sizeof(COS_VM_PortConfigStatus_T));

        for (j = 0; MAX_TCP_PORT_MAP_ENTRIES > j; ++j)
        {
            tcp_port_map_table[i][j].tcp_port = COS_VM_UNDEF_TCP_PORT;
            tcp_port_map_table[i][j].cos = COS_VM_UNDEF_COS_VALUE;
            tcp_port_map_table[i][j].mapping_index = 0;
        }
    }

    return;
}
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME - ConfigAsicAllTosMap
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will configure the default precedence to ASIC
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T ConfigAsicAllTosMap()
{
    UI32_T  i, l_port = 0, index, temp_index;
    BOOL_T  asic_config=TRUE;
    UI8_T   cos;

#if (SYS_CPNT_COS_PER_PORT == TRUE)
    for (l_port=1; l_port<=TOTAL_NBR_OF_LPORT; l_port++)
    {
        if (! SWCTRL_LogicalPortExisting(l_port))
            continue;

        for (i=0; i<COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES; i++)
        {
            for (index=0; index<8; index++)
            {
                if (FALSE == COS_OM_GetIpPrecedence2CosValueByLport(l_port, i, &cos))
                    return FALSE;

                temp_index = (i*8+index);
                asic_config=SWCTRL_SetPortDscpCosMap(l_port, temp_index, cos);
                if (! asic_config)
                    break;
            }
        }
    }
#else
    SWCTRL_GetNextLogicalPort(&l_port);
    for (i=0; i<COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES; i++)
    {
        for (index=0; index<8; index++)
        {
            if (FALSE == COS_OM_GetIpPrecedence2CosValueByLport(l_port, i, &cos))
                return FALSE;

            temp_index = (i*8+index);
            asic_config=SWCTRL_SetPortDscpCosMap(l_port, temp_index, cos);
            if (! asic_config)
                break;
        }
    }
#endif
    return asic_config;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - ConfigAsicAllDscpMap
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will configure the default dscp to ASIC
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T ConfigAsicAllDscpMap()
{
    UI32_T  i, l_port = 0;
    BOOL_T  asic_config=TRUE;
    UI8_T   cos;

#if (SYS_CPNT_COS_PER_PORT == TRUE)
    for (l_port=1; l_port<=TOTAL_NBR_OF_LPORT; l_port++)
    {
        if (! SWCTRL_LogicalPortExisting(l_port))
            continue;

        for (i=0; i<COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES; i++)
        {
            if (FALSE == COS_OM_GetIpDscp2CosValueByLport(l_port, i, &cos))
                return FALSE;

            asic_config=SWCTRL_SetPortDscpCosMap(l_port, i, cos);
            if (! asic_config)
                break;
        }
    }
#else
    SWCTRL_GetNextLogicalPort(&l_port);
    for (i=0; i<COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES; i++)
    {
        if(dscp_map_status_table[l_port-1][i] == FALSE)
            continue;

        if (FALSE == COS_OM_GetIpDscp2CosValueByLport(l_port, i, &cos))
            return FALSE;

        asic_config=SWCTRL_SetPortDscpCosMap(l_port, i, cos);
        if (! asic_config)
            break;
    }
#endif
    return asic_config;
}


/* #endif*/


static BOOL_T COS_VM_SetAllPortMapToASIC(BOOL_T is_add)
{
#if (SYS_CPNT_COS_PER_PORT == TRUE)

    return FALSE;
#else

    COS_VM_PortConfigStatus_T   *port_config_status;
    TCP_Port_Map_T              *tcp_port_map_p;
    UI32_T                      i =0, j, unit, port, trunk;

    if ((SWCTRL_LPORT_UNKNOWN_PORT == SWCTRL_GetNextLogicalPort(&i)) ||
        (SWCTRL_LPORT_UNKNOWN_PORT == SWCTRL_LogicalPortToUserPort(i, &unit, &port, &trunk)))
    {
        return FALSE;
    }

    port_config_status = COS_VM_GetPortConfigStatus(i);
    if (NULL == port_config_status)
        return FALSE;

    --i; /* to zero-based */

    for (j = 0; j < port_config_status->config_count; j++)
    {
        tcp_port_map_p = &tcp_port_map_table[i][j];

        if (TRUE == is_add)
        {
            if (RULE_TYPE_OK != RULE_MGR_SetTcpPortCosMap(unit, port, tcp_port_map_p->tcp_port,
                    tcp_port_map_p->cos, tcp_port_map_p->mapping_index))
            {
                return FALSE;
            }
        }
        else
        {
            if (RULE_TYPE_OK != RULE_MGR_DeleteTcpPortCosMap(unit, port, tcp_port_map_p->tcp_port,
                    tcp_port_map_p->cos, tcp_port_map_p->mapping_index))
            {
                return FALSE;
            }
        }
    }

    return TRUE;

#endif /* SYS_CPNT_COS_PER_PORT == TRUE */
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - COS_VM_CreateTcpPortSettingToOm
 *------------------------------------------------------------------------------
 * PURPOSE  : create and fill om setting (tcp_port_map_table) and update om value (port_config_status)
 * INPUT    : lport, tcp_port, cos
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static TCP_Port_Map_T* COS_VM_CreateTcpPortSettingToOm(UI32_T lport, UI16_T tcp_port, UI8_T cos)
{
    COS_VM_PortConfigStatus_T   *port_config_status;
    TCP_Port_Map_T              *tcp_port_map_p;
    UI32_T                      mapping_index = 0;

    port_config_status = COS_VM_GetPortConfigStatus(lport);
    if (NULL == port_config_status)
        return NULL;

    if (COS_VM_AllocateTcpPortMappingIndex(lport, &mapping_index)== FALSE)
        return NULL;

    /* append new entry (last one) */
    tcp_port_map_p = &tcp_port_map_table[lport - 1][port_config_status->config_count - 1]; /* to zero-based */

    tcp_port_map_p->tcp_port = tcp_port;
    tcp_port_map_p->cos = cos;
    tcp_port_map_p->mapping_index = mapping_index;

    /* re-sort OM */
    if (1 < port_config_status->config_count)
    {
        UI32_T  idx;

        qsort(tcp_port_map_table[lport - 1], port_config_status->config_count, sizeof(TCP_Port_Map_T),
            (COMP_FUNC)compare_tcp_port);

        /* the om is re-sorted, need to find the correct pointer again.
         */
        tcp_port_map_p = NULL;

        for (idx = 0; idx <port_config_status->config_count; idx++)
        {
            if (tcp_port_map_table[lport -1][idx].tcp_port == tcp_port)
            {
                tcp_port_map_p = &tcp_port_map_table[lport -1][idx];
                break;
            }
        }
    }

    return tcp_port_map_p;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - COS_VM_DeleteTcpPortSettingFromOm
 *------------------------------------------------------------------------------
 * PURPOSE  : delete om setting (tcp_port_map_table) and update om value (port_config_status)
 * INPUT    : lport, tcp_port_map_index
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static void COS_VM_DeleteTcpPortSettingFromOm(UI32_T lport, UI32_T tcp_port_map_index)
{
    COS_VM_PortConfigStatus_T   *port_config_status_p;
    TCP_Port_Map_T              *tcp_port_map_p;
    UI32_T                      mapping_index, array_index, index;

    if (TRUE == COS_VM_IS_BAD_TCP_PORT_MAP_IDX(tcp_port_map_index))
        return;

    port_config_status_p = COS_VM_GetPortConfigStatus(lport);
    if (NULL == port_config_status_p)
        return;

    array_index = lport - 1;

    tcp_port_map_p = &tcp_port_map_table[array_index][tcp_port_map_index];
    mapping_index = tcp_port_map_p->mapping_index; /* keep mapping index */

    /* compact tcp port map
     * if deleted entry is the last entry, will not step into this for-loop
     */
    for (index = tcp_port_map_index + 1; index < port_config_status_p->config_count; ++index)
    {
        /* copy next element to previous element */
        memcpy(&tcp_port_map_table[array_index][index - 1], &tcp_port_map_table[array_index][index], sizeof(TCP_Port_Map_T));
    }

    /* update om value ( counter, mapping usage flag ) */
    COS_VM_FreeTcpPortMappingIndex(lport, mapping_index);

    /* ensure the last entry is clear */
    tcp_port_map_p = &tcp_port_map_table[array_index][port_config_status_p->config_count];

    tcp_port_map_p->tcp_port = COS_VM_UNDEF_TCP_PORT;
    tcp_port_map_p->cos = COS_VM_UNDEF_COS_VALUE;
    tcp_port_map_p->mapping_index = 0;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - COS_VM_AllocateTcpPortMappingIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : allocate a un-used mapping index
 * INPUT    : lport
 * OUTPUT   : mapping_index
 * RETURN   : TRUE - succeeded; FALSE - failed
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T COS_VM_AllocateTcpPortMappingIndex(UI32_T lport, UI32_T *mapping_index)
{
    COS_VM_PortConfigStatus_T   *port_config_status;
    UI32_T                      i;

    if (NULL == mapping_index)
        return FALSE;

    port_config_status = COS_VM_GetPortConfigStatus(lport);
    if (NULL == port_config_status)
        return FALSE;

    for (i = 0; i < MAX_TCP_PORT_MAP_ENTRIES;i++)
    {
        if (port_config_status->mapping_index_usage_status[i]==FALSE)
        {
            port_config_status->mapping_index_usage_status[i] = TRUE;
            *mapping_index = i+1;

            ++(port_config_status->config_count);
            return TRUE;
        }
    }
    return FALSE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - COS_VM_FreeTcpPortMappingIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : free a mapping index
 * INPUT    : lport, mapping_index
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static void COS_VM_FreeTcpPortMappingIndex(UI32_T lport, UI32_T mapping_index)
{
    COS_VM_PortConfigStatus_T   *port_config_status;

    if (COS_VM_IS_BAD_MAPPING_IDX(mapping_index))
        return;

    port_config_status = COS_VM_GetPortConfigStatus(lport);
    if (NULL == port_config_status)
        return;

    port_config_status->mapping_index_usage_status[mapping_index - 1] = FALSE; /* to zero-based index */
    --(port_config_status->config_count);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_Register_Dscp2CosChanged_CallBack
 *------------------------------------------------------------------------------
 * FUNCTION: Register the call-back function when
 *             dscp/prec to cos mapping is changed.
 * INPUT   : fun -- call back function pointer
 * OUTPUT  : none
 * RETURN  : none
 * NOTE    : none
 *------------------------------------------------------------------------------
 */
void COS_VM_Register_Dscp2CosChanged_CallBack(void (*fun)(UI32_T val,
                                                          BOOL_T is_dscp))
{
    SYS_TYPE_REGISTER_CALLBACKFUN (dscp2cos_callback_list);
}

#if 0
/*------------------------------------------------------------------------------
 * FUNCTION NAME - COS_VM_Notify_Dscp2CosChanged
 *------------------------------------------------------------------------------
 * PURPOSE  : Call CallBack function when dscp/prec to cos mapping is changed.
 * INPUT    : val, is_dscp
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static void COS_VM_Notify_Dscp2CosChanged(UI32_T val, BOOL_T is_dscp)
{
    SYS_TYPE_CallBack_T  *fun_list;

    for(fun_list=dscp2cos_callback_list; fun_list; fun_list=fun_list->next)
        fun_list->func(val, is_dscp);

    return;
}
#endif

static BOOL_T COS_VM_entryCompare(per_port_l4_dscp_t *e1, per_port_l4_dscp_t *e2)
{
   if((e1->ifindex != e2->ifindex)||(e1->protocol != e2->protocol)||(e1->dport != e2->dport))
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }

}

void TcpUdp_listnode_delete (struct L_list *list,   per_port_l4_dscp_t *element)
{
    struct L_listnode *node;

    if ( (!list) || (!element) )
        return;

    for (node = list->head; node; node = node->next)
    {
        if (COS_VM_entryCompare((per_port_l4_dscp_t *)node->data,
                                element))
      {
          if (node->prev)
            node->prev->next = node->next;
          else
            list->head = node->next;

          if (node->next)
            node->next->prev = node->prev;
          else
            list->tail = node->prev;

          list->count--;
          free(node->data);
          free(node);
          return;
      }
    }
}

static BOOL_T TcpUdp_LINK_LST_ElementGet (struct L_list *list,   per_port_l4_dscp_t *element)
{
     struct L_listnode *node;
     int ret;

     for (node = list->head; node; node = node->next)
     {
         ret = COS_VM_entryCompare((per_port_l4_dscp_t *)node->data,
                                   element);
        if(TRUE == ret)
            return  TRUE;
     }
      return  FALSE;

}

static BOOL_T TcpUdp_LINK_LST_ElementGetNext (struct L_list *list,   per_port_l4_dscp_t *element)
{
     struct L_listnode *node;
     int ret;
     if(NULL == list)
        return FALSE;
     for (node = list->head; node; node = node->next)
     {
         ret = COS_VM_entryCompare((per_port_l4_dscp_t *)node->data,
                                   element);
        if(TRUE == ret)
        {

             if(node->next == 0)
             {
                 return  FALSE;
             }
             memcpy (element, node->next->data,sizeof(per_port_l4_dscp_t));
             return  TRUE;
        }
     }
      return  FALSE;
}

BOOL_T
COS_VM_ResetTCPUDP2Dscp(
    UI32_T ifindex,
    UI32_T protocol,
    UI32_T dport,
    UI32_T phb,
    UI32_T color)
{
    UI32_T unit, port, trunk_id;
    UI32_T current_phb, current_color;
    SWCTRL_Lport_Type_T  port_type;
    per_port_l4_dscp_t   elem;
    BOOL_T is_tcp;

    switch (protocol)
    {
        case tcp_protocol:
            is_tcp = TRUE;
            break;

         case udp_protocol:
            is_tcp = FALSE;
            break;

        default:
            return FALSE;
    }

    if (dport < MIN_PORT_VAL || MAX_PORT_VAL < dport)
    {
        ASSERT(0);
        return FALSE;
    }

    if (!COS_VM_IS_VALID_PHB(phb) || !COS_VM_IS_VALID_COLOR(color))
    {
        ASSERT(0);
        return FALSE;
    }

    if (FALSE == COS_VM_GetTCPUDP2Dscp(ifindex, protocol, dport, &current_phb, &current_color))
    {
        /* No this mapping
         */
        return TRUE;
    }

    elem.ifindex = ifindex;
    elem.protocol = protocol;
    elem.dport = dport;
    elem.phb = (UI8_T)current_phb;
    elem.color = (UI8_T)current_color;

    COS_VM_USE_CSC(FALSE);

    port_type = SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);
    if(port_type == SWCTRL_LPORT_TRUNK_PORT)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    if(TRUE == RULE_CTRL_DeleteTcpUdpPortCosMap(unit, port, is_tcp, dport,
                   elem.phb, elem.color))
    {
        TcpUdp_listnode_delete( TcpUdpMaplist, &elem);
        TcpUdpMap_counter[unit-1]--;
    }
    else
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    COS_VM_RELEASE_CSC();
    return TRUE;
}

BOOL_T
COS_VM_SetTCPUDP2Dscp(
    UI32_T ifindex,
    UI32_T protocol,
    UI32_T dport,
    UI32_T phb,
    UI32_T color)
{
    UI32_T unit,port,trunk_id;
    UI32_T current_phb, current_color;
    BOOL_T is_tcp, ret;
    SWCTRL_Lport_Type_T  port_type;
    per_port_l4_dscp_t   elem;
    per_port_l4_dscp_t   *tcp_udp_data;
    
    switch (protocol)
    {
        case tcp_protocol:
            is_tcp = TRUE;
            break;

         case udp_protocol:
            is_tcp = FALSE;
            break;

        default:
            return FALSE;
    }

    if (dport < MIN_PORT_VAL || MAX_PORT_VAL < dport)
    {
        ASSERT(0);
        return FALSE;
    }

    if (!COS_VM_IS_VALID_PHB(phb) || !COS_VM_IS_VALID_COLOR(color))
    {
        ASSERT(0);
        return FALSE;
    }

    elem.ifindex = ifindex;
    elem.protocol = protocol;
    elem.dport = dport;
    elem.phb = phb;
    elem.color = color;

    port_type = SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);

    if(port_type == SWCTRL_LPORT_TRUNK_PORT)
    {
        return FALSE;
    }

    ret = COS_VM_GetTCPUDP2Dscp(ifindex, protocol, dport, &current_phb, &current_color);

    COS_VM_USE_CSC(FALSE);

    if (ret == TRUE)
    {
        if (current_phb == elem.phb && current_color == elem.color)
        {
            /* Same with old mapping
             */
            COS_VM_RELEASE_CSC();
            return TRUE;
        }

        if(TRUE != RULE_CTRL_DeleteTcpUdpPortCosMap(unit, port, is_tcp, dport,
                       current_phb, current_color))
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

        TcpUdp_listnode_delete( TcpUdpMaplist, &elem);
        TcpUdpMap_counter[unit-1]--;
    }

    if (TRUE != RULE_CTRL_SetTcpUdpPortCosMap(unit, port, is_tcp, dport, phb, color, FALSE))
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    /* Add new map to database
     */
    tcp_udp_data  = (per_port_l4_dscp_t *)malloc(sizeof(per_port_l4_dscp_t));
    memcpy( tcp_udp_data, &elem ,sizeof(per_port_l4_dscp_t));
    if(NULL ==  L_listnode_add_sort(TcpUdpMaplist, tcp_udp_data))
    {
       COS_VM_RELEASE_CSC();
       return FALSE;
    }

    TcpUdpMap_counter[unit-1]++;

    COS_VM_RELEASE_CSC();
    return TRUE;
}

BOOL_T COS_VM_GetTCPUDP2Dscp(UI32_T ifindex,UI32_T protocol,UI32_T dport,UI32_T *phb,UI32_T *color)
{
    per_port_l4_dscp_t   elem;
    per_port_l4_dscp_t   *data;
    struct L_listnode *node;

    COS_VM_USE_CSC(FALSE);

    if(NULL == TcpUdpMaplist)
        return FALSE;
    node = TcpUdpMaplist->head;
    while( node )
    {
        data = (per_port_l4_dscp_t *) node->data;
        if(ifindex == data->ifindex && protocol == data->protocol && dport == data->dport)
        {
            memcpy(&elem, node->data,sizeof(per_port_l4_dscp_t));
            *phb=elem.phb;
            *color=elem.color;

            COS_VM_RELEASE_CSC();
            return TRUE;
        }
        node = node->next;
    }
    COS_VM_RELEASE_CSC();
    return FALSE;
}
BOOL_T COS_VM_GetNextTCPUDP2Dscp(UI32_T *ifindex,UI32_T *protocol,UI32_T *dport,UI32_T *phb,UI32_T *color)
{
    int ret;
    per_port_l4_dscp_t   elem;
    struct L_listnode *node;
    elem.ifindex = *ifindex;
    elem.protocol = *protocol;
    elem.dport = *dport;
    elem.phb = *phb;
    elem.color = *color;

    COS_VM_USE_CSC(FALSE);

    if(*ifindex == 0)
    {
        if ( (node = TcpUdpMaplist->head) == 0 )
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }
        memcpy (&elem, node->data,sizeof(per_port_l4_dscp_t));
    }
    else
    {
        ret = TcpUdp_LINK_LST_ElementGetNext(TcpUdpMaplist, &elem);
        if( FALSE == ret)
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }
    }

    *ifindex = elem.ifindex;
    *protocol = elem.protocol;
    *dport = elem.dport;
    *phb=elem.phb;
    *color=elem.color;

    COS_VM_RELEASE_CSC();
    return TRUE;
}

BOOL_T COS_VM_GetRunningTCPUDP2Dscp(UI32_T ifindex,UI32_T *protocol,UI32_T *dport,UI32_T *phb,UI32_T *color)
{
    per_port_l4_dscp_t   elem;
    per_port_l4_dscp_t   *data;
    struct L_listnode *node;

    COS_VM_USE_CSC(FALSE);

    if(NULL == TcpUdpMaplist)
        return FALSE;
    node = TcpUdpMaplist->head;
    while( node )
    {
        data = (per_port_l4_dscp_t *)node->data;
        if(ifindex == data->ifindex)
        {
            memcpy(&elem, node->data,sizeof(per_port_l4_dscp_t));
            *protocol = elem.protocol;
            *dport = elem.dport;
            *phb=elem.phb;
            *color=elem.color;

            COS_VM_RELEASE_CSC();
            return TRUE;
        }
        node = node->next;
    }
    COS_VM_RELEASE_CSC();
    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_EnablePortPriorityMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function uses to enable priority's config of COS
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  NONE
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_EnablePortPriorityMode (
    UI32_T l_port,
    UI16_T priority)
{
    UI16_T id;
    int i;
    COS_TYPE_ERROR_CODE_E error_code;

    COS_VM_DEBUG_PRINTF("Input l_port %lu, priority %u", l_port, priority);

    /* User config will enabled in default and has never been disabled */
    if (priority == COS_VM_PRIORITY_USER)
    {
       return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);

    /* If the priority config had enabled before,
     * return COS_TYPE_E_NONE
     */
    if (COS_TYPE_E_CONFIG_NOT_FOUND != error_code)
    {
        return error_code;
    }
    else
    {
         /* find the empty config table index */
         for (i = 0; i < COS_VM_MAX_PRIORITY_MODE_NUMBER; ++i)
         {
             if (TRUE != cos_vm_port_entry[l_port-1].config[i].enable)
             {
                 id = i;
                 break;
             }
         }

         /* The port_entry doesn't has any empty entry */
         if (i >= COS_VM_MAX_PRIORITY_MODE_NUMBER)
         {
             return COS_TYPE_E_CONFIG_FILE_FULL;
         }
    }

    memset(&cos_vm_port_entry[l_port-1].config[id], 0, sizeof(cos_vm_port_entry[l_port-1].config[id]));

    cos_vm_port_entry[l_port-1].config[id].enable = TRUE;
    cos_vm_port_entry[l_port-1].config[id].priority = priority;

    cos_vm_port_entry[l_port-1].trust_mode.config_p[id]    = &cos_vm_port_entry[l_port-1].config[id].trust_mode;
    cos_vm_port_entry[l_port-1].cos_to_dscp.config_p[id]   = &cos_vm_port_entry[l_port-1].config[id].cos_to_dscp;
    cos_vm_port_entry[l_port-1].pre_to_dscp.config_p[id]   = &cos_vm_port_entry[l_port-1].config[id].pre_to_dscp;
    cos_vm_port_entry[l_port-1].dscp_to_dscp.config_p[id]  = &cos_vm_port_entry[l_port-1].config[id].dscp_to_dscp;
    cos_vm_port_entry[l_port-1].dscp_to_queue.config_p[id] = &cos_vm_port_entry[l_port-1].config[id].dscp_to_queue;
    cos_vm_port_entry[l_port-1].dscp_to_color.config_p[id] = &cos_vm_port_entry[l_port-1].config[id].dscp_to_color;
    cos_vm_port_entry[l_port-1].dscp_to_cos.config_p[id]   = &cos_vm_port_entry[l_port-1].config[id].dscp_to_cos;
    cos_vm_port_entry[l_port-1].queue_mode.config_p[id]    = &cos_vm_port_entry[l_port-1].config[id].queue_mode;
    cos_vm_port_entry[l_port-1].queue_weight.config_p[id] = &cos_vm_port_entry[l_port-1].config[id].queue_weight;

    return COS_TYPE_E_NONE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - compare_priority
 *------------------------------------------------------------------------------
 * PURPOSE: This function will compare tcpport value
 * INPUT:   value1 : interger value 1 to compare
 *          value2 : interger value 2 to compare
 * OUTPUT:  None
 * RETURN:   1 --> value1 < value2
 *          -l --> value1 > value2
 * NOTE:    static function, used for COS_VM_DisablePortPriorityMode()
 *------------------------------------------------------------------------------
 */
static int
compare_priority(
    int *value1,
    int *value2)
{
    if (*value1 < *value2)
        return 1;
    else
        return -1;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DisablePortPriorityMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function use to disable other CSC config on COS
 * INPUT:   l_port   - logic port
 *          priority - COS_TYPE_PRIORITY_XX
 * OUTPUT:  NONE
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    NONE
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_DisablePortPriorityMode (
    UI32_T l_port,
    UI16_T priority)
{
    UI16_T id;
    BOOL_T re_value;
    COS_TYPE_ERROR_CODE_E error_code;

    if (priority == COS_VM_PRIORITY_USER)
    {
       return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        /* no config of the priority enabled */
        if (COS_TYPE_E_CONFIG_NOT_FOUND == error_code)
        {
            return COS_TYPE_E_NONE;
        }
        else
        {
            return error_code;
        }
    }

    cos_vm_port_entry[l_port-1].config[id].enable = FALSE;

    /* clear the point of COS_VM_PortEntry[l_port-1].{table_name}.config[id]
     */
    cos_vm_port_entry[l_port-1].trust_mode.config_p[id]    = NULL;
    cos_vm_port_entry[l_port-1].cos_to_dscp.config_p[id]   = NULL;
    cos_vm_port_entry[l_port-1].pre_to_dscp.config_p[id]   = NULL;
    cos_vm_port_entry[l_port-1].dscp_to_dscp.config_p[id]  = NULL;
    cos_vm_port_entry[l_port-1].dscp_to_queue.config_p[id] = NULL;
    cos_vm_port_entry[l_port-1].dscp_to_color.config_p[id] = NULL;
    cos_vm_port_entry[l_port-1].dscp_to_cos.config_p[id]   = NULL;
    cos_vm_port_entry[l_port-1].queue_mode.config_p[id]    = NULL;
    cos_vm_port_entry[l_port-1].queue_weight.config_p[id] = NULL;

    /* Checking the disabled config is current active config,
     * if yes, find the second hightest priority config set to driver
     *         and update local OM.
     */
    /* cos_to_dscp */
    if (priority == cos_vm_port_entry[l_port-1].cos_to_dscp.active_owner_p->priority)
    {
        COS_VM_RECUR_PORT_PRIORITY_CONFIG(l_port, cos_to_dscp);

        {
            int cos_index, cfi_index;
            for (cos_index = 0; cos_index < INGRESS_COS_TO_DSCP_MAPPING_ENTRY_NMB; cos_index++)
            {
                for (cfi_index = 0; cfi_index < INGRESS_COS_TO_DSCP_MAPPING_CFI; cfi_index++)
                {
                    re_value = SWCTRL_SetQosIngressCos2Dscp(l_port, cos_index, cfi_index,
                                   cos_vm_port_entry[l_port-1].cos_to_dscp.active_p->table.CURRENT_COS_TO_DSCP_MAPPING[cos_index][cfi_index].phb,
                                   cos_vm_port_entry[l_port-1].cos_to_dscp.active_p->table.CURRENT_COS_TO_DSCP_MAPPING[cos_index][cfi_index].color);
                }
            }
        }
    }

    /* trust_mode */
    if (priority == cos_vm_port_entry[l_port-1].trust_mode.active_owner_p->priority)
    {
        COS_VM_RECUR_PORT_PRIORITY_CONFIG(l_port, trust_mode);

        {
            UI32_T mode;

            switch (cos_vm_port_entry[l_port-1].trust_mode.active_p->mode)
            {
                case PRECEDENCE_MAPPING_MODE:
                    mode = DSCP_MAPPING_MODE;
                    break;
                case  DSCP_IF_MAPPING_MODE:
                    mode = DSCP_MAPPING_MODE;
                    break;
                case COS_MAPPING_MODE:
                default:
                    mode = L2_MAPPING_MODE;
                    break;
            }
            re_value = SWCTRL_SetQosTrustMode(l_port, mode);
        }
    }

    /* pre_to_dscp */
    if (priority == cos_vm_port_entry[l_port-1].pre_to_dscp.active_owner_p->priority &&
        PRECEDENCE_MAPPING_MODE == cos_vm_port_entry[l_port-1].trust_mode.active_p->mode)
    {
        COS_VM_RECUR_PORT_PRIORITY_CONFIG(l_port, pre_to_dscp);

        {
            int pre;
            for (pre = 0; pre < INGRESS_PRE_TO_DSCP_MAPPING_ENTRY_NMB; pre++)
            {
                re_value = SWCTRL_SetQosIngressPre2Dscp(l_port, pre,
                                cos_vm_port_entry[l_port-1].pre_to_dscp.active_p->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].phb,
                                cos_vm_port_entry[l_port-1].pre_to_dscp.active_p->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].color);
            }
        }
    }

    /* dscp_to_dscp */
    if (priority == cos_vm_port_entry[l_port-1].dscp_to_dscp.active_owner_p->priority &&
        PRECEDENCE_MAPPING_MODE != cos_vm_port_entry[l_port-1].trust_mode.active_p->mode)
    {
        COS_VM_RECUR_PORT_PRIORITY_CONFIG(l_port, dscp_to_dscp);

        {
            int dscp;
            for (dscp = 0; dscp < INGRESS_DSCP_TO_DSCP_MAPPING_ENTRY_NMB; dscp++)
            {
                re_value = SWCTRL_SetQosIngressDscp2Dscp(l_port, dscp,
                                cos_vm_port_entry[l_port-1].dscp_to_dscp.active_p->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].phb,
                                cos_vm_port_entry[l_port-1].dscp_to_dscp.active_p->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].color);
            }
        }
    }

    /* dscp_to_queue */
#if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT)
    if (priority == cos_vm_port_entry[l_port-1].dscp_to_queue.active_owner_p->priority)
    {
        COS_VM_RECUR_PORT_PRIORITY_CONFIG(l_port, dscp_to_queue);

        {
            int phb;
            for (phb = 0; phb <= MAX_PHB_VAL; phb++)
            {
                re_value = SWCTRL_SetQosIngressDscp2Queue(l_port, phb,
                                cos_vm_port_entry[l_port-1].dscp_to_queue.active_p->table.CURRENT_DSCP_TO_QUEUE_MAPPING[phb]);
            }
        }
    }
#endif /* #if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT) */

    /* dscp_to_color */
    if (priority == cos_vm_port_entry[l_port-1].dscp_to_color.active_owner_p->priority)
    {
        COS_VM_RECUR_PORT_PRIORITY_CONFIG(l_port, dscp_to_color);

        {
            int phb;
            for (phb = 0; phb <= MAX_PHB_VAL; phb++)
            {
                re_value = SWCTRL_SetQosIngressDscp2Color(l_port, phb,
                                cos_vm_port_entry[l_port-1].dscp_to_color.active_p->table.CURRENT_DSCP_TO_COLOR_MAPPING[phb]);
            }
        }
    }

    /* dscp_to_cos */
    if (priority == cos_vm_port_entry[l_port-1].dscp_to_cos.active_owner_p->priority)
    {
        COS_VM_RECUR_PORT_PRIORITY_CONFIG(l_port, dscp_to_cos);

        {
            int phb, color;
            for (phb = 0; phb < INGRESS_DSCP_TO_COS_MAPPING_PHB; phb++)
            {
                for (color = 0; color < INGRESS_DSCP_TO_COS_MAPPING_COLOR; color++)
                {
                    re_value = SWCTRL_SetQosIngressDscp2Cos(l_port, phb, color,
                                    cos_vm_port_entry[l_port-1].dscp_to_cos.active_p->table.CURRENT_DSCP_TO_COS_MAPPING[phb][color].cos,
                                    cos_vm_port_entry[l_port-1].dscp_to_cos.active_p->table.CURRENT_DSCP_TO_COS_MAPPING[phb][color].cfi);
                }
            }
        }
    }

    /* queue_mode */
    if (priority == cos_vm_port_entry[l_port-1].queue_mode.active_owner_p->priority)
    {
        COS_VM_RECUR_PORT_PRIORITY_CONFIG(l_port, queue_mode);

        {
#if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE)
            re_value = SWCTRL_SetPortPrioQueueMode(l_port,
                           cos_vm_port_entry[l_port-1].queue_mode.active_p->mode);
#else
            re_value = SWCTRL_SetPrioQueueMode(cos_vm_port_entry[l_port-1].queue_mode.active_p->mode);

#endif /* #if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE) */

        }
    }

    /* queue_weight */
    if (priority == cos_vm_port_entry[l_port-1].queue_weight.active_owner_p->priority)
    {
        COS_VM_RECUR_PORT_PRIORITY_CONFIG(l_port, queue_weight);

        if (COS_VM_SCHEDULING_STRICT_PRIORITY != cos_vm_port_entry[l_port-1].queue_mode.active_p->mode)
        {
            UI32_T queue;
            UI32_T weight;

            for (queue = 0; queue < SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE; queue++)
            {
                switch (cos_vm_port_entry[l_port-1].queue_mode.active_p->mode)
                {
                    case COS_VM_SCHEDULING_METHOD_DRR:
                    case COS_VM_SCHEDULING_WEIGHT_ROUND_ROBIN:
                        weight = cos_vm_port_entry[l_port-1].queue_weight.active_p->wrr_weight[queue];

#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
                        re_value = SWCTRL_SetPortWrrQueueWeight(l_port, queue, weight);
#else
                        re_value = SWCTRL_SetWrrQueueWeight(queue, weight);
#endif /* SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL */
                        break;

                    case COS_VM_SCHEDULING_METHOD_SP_WRR:
                    case COS_VM_SCHEDULING_METHOD_SP_DRR:
                        weight = (TRUE == cos_vm_port_entry[l_port-1].queue_weight.active_p->is_strict[queue])
                                 ? 0 : cos_vm_port_entry[l_port-1].queue_weight.active_p->wrr_weight[queue];

#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
                        re_value = SWCTRL_SetPortWrrQueueWeight(l_port, queue, weight);
#else
                        re_value = SWCTRL_SetWrrQueueWeight(queue, weight);
#endif /* SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL */
                        break;

                    case COS_VM_SCHEDULING_STRICT_PRIORITY:
                    default:
                        break;
                }
            }
        }
    }

    return COS_TYPE_E_NONE;
}

#if (__APPLE__ && __MACH__)
#pragma mark - TRUST_MODE
#endif
BOOL_T COS_VM_SetPortListTrustMode(UI8_T *port_list, UI32_T mode)
{
    qos_if_mapping_mode_t map_mode;
    COS_VM_USE_CSC(FALSE);

    if(COS_MAPPING_MODE== mode)
    {
        map_mode = L2_MAPPING_MODE;
    }
    else  if(PRECEDENCE_MAPPING_MODE == mode)
    {
        map_mode = DSCP_MAPPING_MODE;
    }
    else if(DSCP_IF_MAPPING_MODE == mode)
    {
        map_mode = DSCP_MAPPING_MODE;
    }
    else
    {

        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    if(!SWCTRL_SetPortListQosTrustMode(port_list,mode))
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    COS_VM_RELEASE_CSC();
    return TRUE;
}

#if (__APPLE__ && __MACH__)
#pragma mark =per-port=
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortTrustMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the COS trust mode
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          mode     - trust mode, qos_if_trust_mode_t
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortTrustMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T mode)
{
#define CONFIG              (cos_vm_port_entry[l_port-1].config[id])
#define PORT_ACTIVE_P       (cos_vm_port_entry[l_port-1].trust_mode.active_p)
#define PORT_ACTIVE_OWNER_P (cos_vm_port_entry[l_port-1].trust_mode.active_owner_p)
#define PORT_CONFIG_P       (cos_vm_port_entry[l_port-1].trust_mode.config_p[id])

    UI32_T temp_mode;
    UI16_T id;
    BOOL_T old_modified;
    COS_VM_CosPortConfig_T *old_active_owner_p;
    COS_VM_TRUST_MODE_CONFIG_T *old_active_p;
    COS_TYPE_ERROR_CODE_E error_code;
    BOOL_T re_value;

    error_code = COS_VM_Priv_VerifyIfindex(l_port);
    if (error_code != COS_TYPE_E_NONE)
    {
        return error_code;
    }

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != CONFIG.enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    switch (mode)
    {
        case COS_MAPPING_MODE:
            temp_mode = L2_MAPPING_MODE;
            break;
        case PRECEDENCE_MAPPING_MODE:
            temp_mode = DSCP_MAPPING_MODE;
            break;
        case  DSCP_IF_MAPPING_MODE:
            temp_mode = DSCP_MAPPING_MODE;
            break;
        default:
            return COS_TYPE_E_PARAMETER;
    }

    old_modified = PORT_CONFIG_P->common.modified;
    if (FALSE == PORT_CONFIG_P->common.modified)
    {
        PORT_CONFIG_P->mode = PORT_ACTIVE_P->mode;
        PORT_CONFIG_P->common.modified = TRUE;
    }
    else
    {
        /* same with old configure
         */
        if (mode == PORT_CONFIG_P->mode)
        {
            return COS_TYPE_E_NONE;
        }
    }

    old_active_owner_p = PORT_ACTIVE_OWNER_P;
    old_active_p = PORT_ACTIVE_P;

    /* Change active */
    if ( PORT_ACTIVE_OWNER_P->priority < priority)
    {
        PORT_ACTIVE_P = PORT_CONFIG_P;
        PORT_ACTIVE_OWNER_P = &CONFIG;

    }

    if (PORT_ACTIVE_OWNER_P->priority == priority)
    {
        re_value = SWCTRL_SetQosTrustMode(l_port, temp_mode);

        if (FALSE == re_value)
        {
            PORT_CONFIG_P->common.modified = old_modified;
            PORT_ACTIVE_OWNER_P = old_active_owner_p;
            PORT_ACTIVE_P = old_active_p;
            return COS_TYPE_E_SET_TO_SWCTRL;
        }

        if (mode == DSCP_IF_MAPPING_MODE)
        {
            COS_VM_Priv_SetDscp2InternalDscpForAll(l_port, priority);
        }
        else if (mode == PRECEDENCE_MAPPING_MODE)
        {
            COS_VM_Priv_SetPre2InternalDscpForAll(l_port, priority);
        }
    }

    /* update local OM before announce other CSC */
    PORT_CONFIG_P->mode = (qos_if_trust_mode_t) mode;

    SYS_CALLBACK_MGR_AnnounceCosPortConfigChanged(SYS_MODULE_COS, l_port, priority);

    return COS_TYPE_E_NONE;

#undef CONFIG
#undef PORT_ACTIVE_P
#undef PORT_ACTIVE_OWNER_P
#undef PORT_CONFIG_P
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortTrustMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the COS trust mode
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  mode     - trust mode, qos_if_trust_mode_t
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Gets the active config
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortTrustMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T *mode)
{
    UI16_T id;
    COS_TYPE_ERROR_CODE_E error_code;

    error_code = COS_VM_Priv_VerifyIfindex(l_port);
    if (error_code != COS_TYPE_E_NONE)
    {
        return error_code;
    }

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != cos_vm_port_entry[l_port-1].config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == cos_vm_port_entry[l_port-1].trust_mode.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }

    *mode = cos_vm_port_entry[l_port-1].trust_mode.config_p[id]->mode;

    return COS_TYPE_E_NONE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortActiveTrustMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the actively trust mode
 * INPUT:   l_port   - logic port
 * OUTPUT: priority - the priority of CoS config, ex: COS_TYPE_PRIORITY_XX
 *         mode     - trust mode, qos_if_trust_mode_t
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Gets the active config
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortActiveTrustMode(
    UI32_T l_port,
    UI16_T *priority_t,
    UI32_T *mode_p)
{
    COS_TYPE_ERROR_CODE_E result;

    result = COS_VM_Priv_VerifyIfindex(l_port);
    if (result != COS_TYPE_E_NONE)
    {
        return result;
    }

    COS_VM_USE_CSC(COS_TYPE_E_UNKNOWN);

    ASSERT(NULL != cos_vm_port_entry[l_port-1].trust_mode.active_p);

    *mode_p = cos_vm_port_entry[l_port-1].trust_mode.active_p->mode;
    *priority_t = cos_vm_port_entry[l_port - 1].trust_mode.active_owner_p->priority;

    COS_VM_RELEASE_CSC();
    return COS_TYPE_E_NONE;
}

#if (__APPLE__ && __MACH__)
#pragma mark - CoS_DSCP
#endif
BOOL_T COS_VM_SetPortListIngressCos2Dscp(UI8_T *port_list,UI32_T cos,UI32_T cfi,UI32_T phb,UI32_T color)
{
    /* FIXEME: Remove this API
     */
    COS_VM_USE_CSC(FALSE);

    if(!SWCTRL_SetPortListQosIngressCos2Dscp(port_list,cos, cfi, phb, color))
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    COS_VM_RELEASE_CSC();
    return TRUE;
}
BOOL_T COS_VM_ResetIngressCos2Dscp(UI32_T lport,UI32_T cos,UI32_T cfi)
{
    int ret;

    COS_VM_USE_CSC(FALSE);
    ret = COS_VM_SetPortCos2InternalDscp(lport, COS_TYPE_PRIORITY_USER, cos, cfi,
                                         DEFAULT_COS_TO_DSCP_MAPPING[cos][cfi].phb,
                                         DEFAULT_COS_TO_DSCP_MAPPING[cos][cfi].color);
    if(ret != TRUE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    COS_VM_RELEASE_CSC();
    return TRUE;
}

#if (__APPLE__ && __MACH__)
#pragma mark =Global=
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the mapping of ingress cos to internal dscp
 * INPUT:   priority - COS_TYPE_PRIORITY_XX
 *          cos      - cos queue
 *          cfi      - cos cfi
 *          phb      - phb
 *          color    - color
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetCos2InternalDscp(
    UI16_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T phb,
    UI32_T color)
{
#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else

#define CONFIG              (cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].config[id])
#define PORT_ACTIVE_P       (cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].cos_to_dscp.active_p)
#define PORT_ACTIVE_OWNER_P (cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].cos_to_dscp.active_owner_p)
#define PORT_CONFIG_P       (cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].cos_to_dscp.config_p[id])

    UI16_T id;
    BOOL_T old_modified;
    COS_VM_CosPortConfig_T *old_active_owner_p;
    COS_VM_COS_TO_DSCP_CONFIG_T *old_active_p;
    COS_TYPE_ERROR_CODE_E error_code;
    BOOL_T re_value;

    COS_VM_DEBUG_PRINTF("Input priority: %u, cos: %lu, cfi: %lu, phb: %lu, color: %lu",
                        priority, cos, cfi, phb, color);

    if (!COS_VM_IS_VALID_COS(cos) || !COS_VM_IS_VALID_CFI(cfi) ||
        !COS_VM_IS_VALID_PHB(phb) || !COS_VM_IS_VALID_COLOR(color))
    {
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(COS_VM_LPORT_OF_GLOBAL, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != CONFIG.enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    old_modified = PORT_CONFIG_P->common.modified;
    if (FALSE == PORT_CONFIG_P->common.modified)
    {
        memcpy(&PORT_CONFIG_P->table,
               &PORT_ACTIVE_P->table,
               sizeof(PORT_CONFIG_P->table));
        PORT_CONFIG_P->common.modified = TRUE;
    }
    else
    {
        /* same with old configure
         */
        if ((phb == PORT_CONFIG_P->table.CURRENT_COS_TO_DSCP_MAPPING[cos][cfi].phb) &&
            (color == PORT_CONFIG_P->table.CURRENT_COS_TO_DSCP_MAPPING[cos][cfi].color))
        {
            return COS_TYPE_E_NONE;
        }
    }

    old_active_owner_p = PORT_ACTIVE_OWNER_P;
    old_active_p = PORT_ACTIVE_P;

    /* Change active */
    if ( PORT_ACTIVE_OWNER_P->priority < priority)
    {
        PORT_ACTIVE_P = PORT_CONFIG_P;
        PORT_ACTIVE_OWNER_P = &CONFIG;
    }

    if (PORT_ACTIVE_OWNER_P->priority == priority)
    {
        UI8_T  all_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];

        /* FIXME: Switch_ctrl shall support global API
         */
        COS_VM_GetAllPortList(all_port_list);
        re_value = COS_VM_SetPortListIngressCos2Dscp(all_port_list, cos, cfi, phb, color);
        if (TRUE != re_value)
        {
            PORT_CONFIG_P->common.modified = old_modified;
            PORT_ACTIVE_OWNER_P = old_active_owner_p;
            PORT_ACTIVE_P = old_active_p;
            return COS_TYPE_E_SET_TO_SWCTRL;
        }
    }

    /* update local om before annouce other CSC */
    PORT_CONFIG_P->table.CURRENT_COS_TO_DSCP_MAPPING[cos][cfi].phb = phb;
    PORT_CONFIG_P->table.CURRENT_COS_TO_DSCP_MAPPING[cos][cfi].color = color;

    SYS_CALLBACK_MGR_AnnounceCosPortConfigChanged(SYS_MODULE_COS, COS_VM_LPORT_OF_GLOBAL, priority);

    return COS_TYPE_E_NONE;

#undef CONFIG
#undef PORT_ACTIVE_P
#undef PORT_ACTIVE_OWNER_P
#undef PORT_CONFIG_P
#endif /* SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the mapping of ingress cos to internal dscp
 * INPUT:   priority - COS_TYPE_PRIORITY_XX
 *          cos      - cos queue
 *          cfi      - cos cfi
 * OUTPUT:  phb      - phb
 *          color    - color
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    return the config by input priority
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetCos2InternalDscp(
    UI16_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T *phb,
    UI32_T *color)
{
#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else
    UI32_T l_port = COS_VM_LPORT_OF_GLOBAL;
    UI16_T id;
    COS_TYPE_ERROR_CODE_E error_code;

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (!COS_VM_IS_VALID_COS(cos) || !COS_VM_IS_VALID_CFI(cfi))
    {
        return COS_TYPE_E_PARAMETER;
    }

    if (TRUE != cos_vm_port_entry[l_port-1].config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == cos_vm_port_entry[l_port-1].cos_to_dscp.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }

    *phb = cos_vm_port_entry[l_port-1].cos_to_dscp.config_p[id]->table.CURRENT_COS_TO_DSCP_MAPPING[cos][cfi].phb;
    *color = cos_vm_port_entry[l_port-1].cos_to_dscp.config_p[id]->table.CURRENT_COS_TO_DSCP_MAPPING[cos][cfi].color;

    return COS_TYPE_E_NONE;
#endif /* SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next the mapping of ingress cos to internal dscp
 * INPUT:   priority - COS_TYPE_PRIORITY_XX
 *          cos      - cos queue
 *          cfi      - cos cfi
 *
 * OUTPUT:  cos_p -
 *          cfi_p -
 *          phb_p -
 *          color_p -
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextCos2InternalDscp(
    UI16_T priority,
    UI32_T *cos_p,
    UI32_T *cfi_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else
    ASSERT(cos_p != NULL);
    ASSERT(cfi_p != NULL);
    ASSERT(phb_p != NULL);
    ASSERT(color_p != NULL);

    if (*cos_p == COS_TYPE_UNKNOWN_COS)
    {
        *cos_p = MIN_COS_VAL;
        *cfi_p = MIN_CFI_VAL;
    }
    else if (*cfi_p == COS_TYPE_UNKNOWN_CFI)
    {
        *cfi_p = MIN_CFI_VAL;
    }
    else
    {
        *cfi_p += 1;
    }

    for (; *cos_p <= MAX_COS_VAL; ++(*cos_p))
    {
        for (; *cfi_p <= MAX_CFI_VAL; ++(*cfi_p))
        {
            COS_TYPE_ERROR_CODE_E result;

            result = COS_VM_GetCos2InternalDscp(priority, *cos_p, *cfi_p, phb_p, color_p);
            if (result == COS_TYPE_E_NONE)
            {
                return COS_TYPE_E_NONE;
            }
        }

        *cfi_p = MIN_CFI_VAL;
    }

    return COS_TYPE_E_UNKNOWN;
#endif /* SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetActiveCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets actived ingress cos to internal dscp
 * INPUT:   cos      - cos queue
 *          cfi      - cos cfi
 * OUTPUT:  priority_p - COS_TYPE_PRIORITY_XX
 *          phb        - phb
 *          color      - color
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetActiveCos2InternalDscp(
    UI32_T cos,
    UI32_T cfi,
    UI16_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else
    if (priority_p == NULL || phb_p == NULL || color_p == NULL)
    {
        return COS_TYPE_E_PARAMETER;
    }

    if (!COS_VM_IS_VALID_COS(cos) || !COS_VM_IS_VALID_CFI(cfi))
    {
        return COS_TYPE_E_PARAMETER;
    }

    *priority_p = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].cos_to_dscp.active_owner_p->priority;

    *phb_p = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].cos_to_dscp.active_p->table.CURRENT_COS_TO_DSCP_MAPPING[cos][cfi].phb;
    *color_p = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].cos_to_dscp.active_p->table.CURRENT_COS_TO_DSCP_MAPPING[cos][cfi].color;
    
    return COS_TYPE_E_NONE;
#endif /* SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_ResetCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: Resets the mapping of ingress cos to internal dscp
 * INPUT:   priority - COS_TYPE_PRIORITY_XX
 *          cos      - cos queue
 *          cfi      - cos cfi
 * OUTPUT:
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_ResetCos2InternalDscp(
    UI16_T priority,
    UI32_T cos,
    UI32_T cfi)
{
#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else
    return COS_VM_SetCos2InternalDscp(priority, cos, cfi,
                                      DEFAULT_COS_TO_DSCP_MAPPING[cos][cfi].phb,
                                      DEFAULT_COS_TO_DSCP_MAPPING[cos][cfi].color);
#endif /* SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextRunningCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: Gets next running configuration for ingress cos to internal dscp.
 * INPUT:   phb   - PHB, one value of the internal DSCP
 *          color - Color, one value of the internal DSCP
 *
 * OUTPUT:  cos_p - The <cos, cfi> value mapping to the input <phb, color>.
 *          cfi_p -
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:    Using cos = COS_TYPE_UNKOWN_COS, cfi = COS_TYPE_UNKONW_CFI to
 *          get first mapping.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetNextRunningCos2InternalDscp(
    UI32_T phb,
    UI32_T color,
    UI32_T *cos_p,
    UI32_T *cfi_p)
{
#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == FALSE)
    UI32_T cos, cfi;
    UI32_T temp_phb, temp_color;

    COS_VM_DEBUG_PRINTF("Input phb: %lu, color: %lu", phb, color);

    if (cos_p == NULL || cfi_p == NULL)
    {
        ASSERT(0);
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    if (!COS_VM_IS_VALID_PHB(phb) || !COS_VM_IS_VALID_COLOR(color))
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    cos = *cos_p;
    cfi = *cfi_p;

    while(COS_TYPE_E_NONE ==
          COS_VM_GetNextCos2InternalDscp(COS_TYPE_PRIORITY_USER, &cos, &cfi, &temp_phb, &temp_color))
    {
        if(temp_phb == phb &&
           temp_color == color &&
           !COS_VM_IS_DEFAULT_COS_TO_DSCP_MAP(cos, cfi, temp_phb, temp_color))
        {
            *cos_p = cos;
            *cfi_p = cfi;

            return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
        }
    }

    if (*cos_p == COS_TYPE_UNKNOWN_COS ||
        (*cos_p == MIN_COS_VAL && *cfi_p == COS_TYPE_UNKNOWN_CFI))
    {
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }

#endif /* (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == FALSE) */
    return SYS_TYPE_GET_RUNNING_CFG_FAIL;
}

#if (__APPLE__ && __MACH__)
#pragma mark =per-port=
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the mapping of ingress cos to internal dscp
 * INPUT:   priority - COS_TYPE_PRIORITY_XX
 *          l_port   - logic port
 *          cos      - cos queue
 *          cfi      - cos cfi
 *          phb      - phb
 *          color    - color
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortCos2InternalDscp(
    UI32_T l_port,
    UI16_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T phb,
    UI32_T color)
{
#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE)

#define CONFIG              (cos_vm_port_entry[l_port-1].config[id])
#define PORT_ACTIVE_P       (cos_vm_port_entry[l_port-1].cos_to_dscp.active_p)
#define PORT_ACTIVE_OWNER_P (cos_vm_port_entry[l_port-1].cos_to_dscp.active_owner_p)
#define PORT_CONFIG_P       (cos_vm_port_entry[l_port-1].cos_to_dscp.config_p[id])

    UI16_T id;
    BOOL_T old_modified;
    COS_VM_CosPortConfig_T *old_active_owner_p;
    COS_VM_COS_TO_DSCP_CONFIG_T *old_active_p;
    COS_TYPE_ERROR_CODE_E error_code;
    BOOL_T re_value;

    COS_VM_DEBUG_PRINTF("Input l_port: %lu, priority: %u\r\n      cos: %lu, cfi: %lu, phb: %lu, color: %lu",
        l_port, priority, cos, cfi, phb, color);

    if (!COS_VM_IS_VALID_COS(cos) || !COS_VM_IS_VALID_CFI(cfi) ||
        !COS_VM_IS_VALID_PHB(phb) || !COS_VM_IS_VALID_COLOR(color))
    {
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != CONFIG.enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    error_code = COS_VM_Priv_VerifyIfindex(l_port);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    old_modified = PORT_CONFIG_P->common.modified;
    if (FALSE == PORT_CONFIG_P->common.modified)
    {
        memcpy(&PORT_CONFIG_P->table,
               &PORT_ACTIVE_P->table,
               sizeof(PORT_CONFIG_P->table));
        PORT_CONFIG_P->common.modified = TRUE;
    }
    else
    {
        /* same with old configure
         */
         if ((phb == PORT_CONFIG_P->table.CURRENT_COS_TO_DSCP_MAPPING[cos][cfi].phb) &&
             (color == PORT_CONFIG_P->table.CURRENT_COS_TO_DSCP_MAPPING[cos][cfi].color))
         {
            return COS_TYPE_E_NONE;
         }
    }


    old_active_owner_p = PORT_ACTIVE_OWNER_P;
    old_active_p = PORT_ACTIVE_P;

    /* Change active */
    if ( PORT_ACTIVE_OWNER_P->priority < priority)
    {
        PORT_ACTIVE_P = PORT_CONFIG_P;
        PORT_ACTIVE_OWNER_P = &CONFIG;
    }

    if (PORT_ACTIVE_OWNER_P->priority == priority)
    {
        re_value = SWCTRL_SetQosIngressCos2Dscp(l_port, cos, cfi, phb, color);
        if (TRUE != re_value)
        {
            PORT_CONFIG_P->common.modified = old_modified;
            PORT_ACTIVE_OWNER_P = old_active_owner_p;
            PORT_ACTIVE_P = old_active_p;
            return COS_TYPE_E_SET_TO_SWCTRL;
        }
    }

    /* update local om before annouce other CSC */
    PORT_CONFIG_P->table.CURRENT_COS_TO_DSCP_MAPPING[cos][cfi].phb = phb;
    PORT_CONFIG_P->table.CURRENT_COS_TO_DSCP_MAPPING[cos][cfi].color = color;

    SYS_CALLBACK_MGR_AnnounceCosPortConfigChanged(SYS_MODULE_COS, l_port, priority);

    return COS_TYPE_E_NONE;

#undef CONFIG
#undef PORT_ACTIVE_P
#undef PORT_ACTIVE_OWNER_P
#undef PORT_CONFIG_P

#else
    return COS_TYPE_E_UNKNOWN;
#endif /* SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the mapping of ingress cos to internal dscp
 * INPUT:   priority - COS_TYPE_PRIORITY_XX
 *          l_port   - logic port
 *          cos      - cos queue
 *          cfi      - cos cfi
 * OUTPUT:  phb      - phb
 *          color    - color
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    return the config by input priority
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortCos2InternalDscp(
    UI32_T l_port,
    UI16_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T *phb,
    UI32_T *color)
{
#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE)
    UI16_T id;
    COS_TYPE_ERROR_CODE_E error_code;

    if (phb == NULL || color == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_Priv_VerifyIfindex(l_port);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (!COS_VM_IS_VALID_COS(cos) || !COS_VM_IS_VALID_CFI(cfi))
    {
        return COS_TYPE_E_PARAMETER;
    }

    if (TRUE != cos_vm_port_entry[l_port-1].config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == cos_vm_port_entry[l_port-1].cos_to_dscp.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }

    *phb = cos_vm_port_entry[l_port-1].cos_to_dscp.config_p[id]->table.CURRENT_COS_TO_DSCP_MAPPING[cos][cfi].phb;
    *color = cos_vm_port_entry[l_port-1].cos_to_dscp.config_p[id]->table.CURRENT_COS_TO_DSCP_MAPPING[cos][cfi].color;

    return COS_TYPE_E_NONE;
#else
    return COS_TYPE_E_UNKNOWN;
#endif /* SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next the mapping of ingress cos to internal dscp
 * INPUT:   l_port_p - logical port
 *          priority - COS_TYPE_PRIORITY_XX
 *          cos_p    -
 *          cfi_p    -
 *
 * OUTPUT:  l_port_p -
 *          cos_p    -
 *          cfi_p    -
 *          phb_p    -
 *          color_p  -
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextPortCos2InternalDscp(
    UI32_T *l_port_p,
    UI16_T priority,
    UI32_T *cos_p,
    UI32_T *cfi_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE)
    ASSERT(l_port_p != NULL);
    ASSERT(cos_p != NULL);
    ASSERT(cfi_p != NULL);
    ASSERT(phb_p != NULL);
    ASSERT(color_p != NULL);

    if (*l_port_p == 0)
    {
        *l_port_p = 1;
        *cos_p = MIN_COS_VAL;
        *cfi_p = MIN_CFI_VAL;
    }
    else if (*cos_p == COS_TYPE_UNKNOWN_COS)
    {
        *cos_p = MIN_COS_VAL;
        *cfi_p = MIN_CFI_VAL;
    }
    else if (*cfi_p == COS_TYPE_UNKNOWN_CFI)
    {
        *cfi_p = MIN_CFI_VAL;
    }
    else
    {
        *cfi_p += 1;
    }

    for (; *l_port_p <= SWITCH_PORT_NUM; ++(*l_port_p))
    {
        for (; *cos_p <= MAX_COS_VAL; ++(*cos_p))
        {
            for (; *cfi_p <= MAX_CFI_VAL; ++(*cfi_p))
            {
                COS_TYPE_ERROR_CODE_E result;

                result = COS_VM_GetPortCos2InternalDscp(*l_port_p, priority, *cos_p, *cfi_p, phb_p, color_p);
                if (result == COS_TYPE_E_NONE)
                {
                    return COS_TYPE_E_NONE;
                }
            }
            *cfi_p = MIN_CFI_VAL;
        }
        *cos_p = MIN_COS_VAL;
    }

#endif /* SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT */

    return COS_TYPE_E_UNKNOWN;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortActiveCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets actived ingress cos to internal dscp
 * INPUT:   cos      - cos queue
 *          cfi      - cos cfi
 * OUTPUT:  priority_p - COS_TYPE_PRIORITY_XX
 *          phb        - phb
 *          color      - color
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortActiveCos2InternalDscp(
    UI32_T l_port,
    UI32_T cos,
    UI32_T cfi,
    UI16_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE)
    COS_TYPE_ERROR_CODE_E result;

    if (priority_p == NULL || phb_p == NULL || color_p == NULL)
    {
        return COS_TYPE_E_PARAMETER;
    }

    result = COS_VM_Priv_VerifyIfindex(l_port);
    if (result != COS_TYPE_E_NONE)
    {
        return result;
    }

    if (!COS_VM_IS_VALID_COS(cos) || !COS_VM_IS_VALID_CFI(cfi))
    {
        return COS_TYPE_E_PARAMETER;
    }

    *priority_p = cos_vm_port_entry[l_port-1].cos_to_dscp.active_owner_p->priority;
    *phb_p = cos_vm_port_entry[l_port-1].cos_to_dscp.active_p->table.CURRENT_COS_TO_DSCP_MAPPING[cos][cfi].phb;
    *color_p = cos_vm_port_entry[l_port-1].cos_to_dscp.active_p->table.CURRENT_COS_TO_DSCP_MAPPING[cos][cfi].color;
    
    return COS_TYPE_E_NONE;
#else
    return COS_TYPE_E_UNKNOWN;
#endif /* SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_ResetPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: Resets the mapping of ingress cos to internal dscp
 * INPUT:   ifindex   - logic port index
 *          priority - COS_TYPE_PRIORITY_XX
 *          cos      - cos queue
 *          cfi      - cos cfi
 * OUTPUT:
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_ResetPortCos2InternalDscp(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T cos,
    UI32_T cfi)
{
#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE)
    return COS_VM_SetPortCos2InternalDscp(ifindex, priority, cos, cfi,
               DEFAULT_COS_TO_DSCP_MAPPING[cos][cfi].phb,
               DEFAULT_COS_TO_DSCP_MAPPING[cos][cfi].color);

#else
    return COS_TYPE_E_UNKNOWN;
#endif /* SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextRunningPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: Gets next running configuration for ingress cos to internal dscp.
 * INPUT:   l_port - Logical port index
 *          phb    - PHB, one value of the internal DSCP
 *          color  - Color, one value of the internal DSCP
 *
 * OUTPUT:  cos_p - The <cos, cfi> value mapping to the input <phb, color>.
 *          cfi_p -
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:    Using cos = COS_TYPE_UNKOWN_COS, cfi = COS_TYPE_UNKONW_CFI to
 *          get first mapping.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetNextRunningPortCos2InternalDscp(
    UI32_T l_port,
    UI32_T phb,
    UI32_T color,
    UI32_T *cos_p,
    UI32_T *cfi_p)
{
#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE)
    UI32_T cos, cfi;
    UI32_T temp_lport, temp_phb, temp_color;

    COS_VM_DEBUG_PRINTF("Input phb: %lu, color: %lu", phb, color);

    if (cos_p == NULL || cfi_p == NULL)
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    if (COS_TYPE_E_NONE != COS_VM_Priv_VerifyIfindex(l_port))
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    if (!COS_VM_IS_VALID_PHB(phb) || !COS_VM_IS_VALID_COLOR(color))
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    temp_lport = l_port;
    cos = *cos_p;
    cfi = *cfi_p;

    while (COS_TYPE_E_NONE ==
           COS_VM_GetNextPortCos2InternalDscp(&temp_lport, COS_TYPE_PRIORITY_USER, &cos, &cfi, &temp_phb, &temp_color))
    {
        if (l_port != temp_lport)
        {
            break;
        }

        if((temp_phb == phb && temp_color == color) &&
           !COS_VM_IS_DEFAULT_COS_TO_DSCP_MAP(cos, cfi, temp_phb, temp_color))
        {
            *cos_p = cos;
            *cfi_p = cfi;

            return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
        }
    }

    if (*cos_p == COS_TYPE_UNKNOWN_COS ||
        (*cos_p == MIN_COS_VAL && *cfi_p == COS_TYPE_UNKNOWN_CFI))
    {
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }

#endif /* (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE) */
    return SYS_TYPE_GET_RUNNING_CFG_FAIL;
}

#if (__APPLE__ && __MACH__)
#pragma mark - IP_PREC_DSCP
#endif
BOOL_T COS_VM_SetPortListIngressPre2Dscp(UI8_T *port_list,UI32_T pre,UI32_T phb,UI32_T color)
{
    COS_VM_USE_CSC(FALSE);

    if(!SWCTRL_SetPortListQosIngressPre2Dscp(port_list,pre,phb,color))
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    COS_VM_RELEASE_CSC();
    return TRUE;
}

BOOL_T COS_VM_GetIngressPre2Dscp(UI32_T lport, UI8_T *phb, UI8_T *color)
{
    UI32_T pre;
    COS_TYPE_ERROR_CODE_E result;

    COS_VM_USE_CSC(FALSE);

    for(pre = 0; pre <= MAX_PRE_VAL; pre++)
    {
        UI32_T temp_phb, temp_color;

        result = COS_VM_GetPortPre2InternalDscp(lport, COS_TYPE_PRIORITY_USER, pre,
                                                &temp_phb, &temp_color);
        if(result != COS_TYPE_E_NONE)
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }

        phb[pre] = temp_phb;
        color[pre] = temp_color;
    }

    COS_VM_RELEASE_CSC();
    return TRUE;
}

BOOL_T COS_VM_ResetIngressPre2Dscp(UI32_T lport)
{
    UI32_T pre, ret;

    COS_VM_USE_CSC(FALSE);
    for(pre = 0;pre <= MAX_PRE_VAL;pre++)
    {
        ret = COS_VM_SetPortPre2InternalDscp(lport, COS_TYPE_PRIORITY_USER, pre,
                                             DEFAULT_PRE_TO_DSCP_MAPPING[pre].phb,
                                             DEFAULT_PRE_TO_DSCP_MAPPING[pre].color);
        if(ret != TRUE)
        {
            COS_VM_RELEASE_CSC();
            return FALSE;
        }
    }
    COS_VM_RELEASE_CSC();
    return TRUE;
}

#if (__APPLE__ && __MACH__)
#pragma mark =Global=
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the Precedence to Dscp
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          pre      - Precedence value
 *          phb      - phb value
 *          color    - color value
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPre2InternalDscp(
    UI16_T priority,
    UI32_T pre,
    UI32_T phb,
    UI32_T color)
{
#if (SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else

#define CONFIG              (cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].config[id])
#define PORT_ACTIVE_P       (cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].pre_to_dscp.active_p)
#define PORT_ACTIVE_OWNER_P (cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].pre_to_dscp.active_owner_p)
#define PORT_CONFIG_P       (cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].pre_to_dscp.config_p[id])

    UI16_T id;
    BOOL_T old_modified;
    COS_VM_CosPortConfig_T *old_active_owner_p;
    COS_VM_PRE_TO_DSCP_CONFIG_T *old_active_p;
    COS_TYPE_ERROR_CODE_E error_code;
    BOOL_T re_value;

    if (!COS_VM_IS_VALID_IP_PRE(pre) || !COS_VM_IS_VALID_PHB(phb) || !COS_VM_IS_VALID_COLOR(color))
    {
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(COS_VM_LPORT_OF_GLOBAL, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != CONFIG.enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    old_modified = PORT_CONFIG_P->common.modified;
    if (FALSE == PORT_CONFIG_P->common.modified)
    {
        memcpy(&PORT_CONFIG_P->table,
               &PORT_ACTIVE_P->table,
               sizeof(PORT_CONFIG_P->table));
        PORT_CONFIG_P->common.modified = TRUE;
    }
    else
    {
        /* same with old configure
         */
        if ((phb == PORT_CONFIG_P->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].phb) &&
            (color == PORT_CONFIG_P->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].color))
        {
            return COS_TYPE_E_NONE;
        }
    }

    old_active_owner_p = PORT_ACTIVE_OWNER_P;
    old_active_p = PORT_ACTIVE_P;

    /* Change active */
    if (PORT_ACTIVE_OWNER_P->priority < priority)
    {
        PORT_ACTIVE_P = PORT_CONFIG_P;
        PORT_ACTIVE_OWNER_P = &CONFIG;
    }

    if (PRECEDENCE_MAPPING_MODE == CONFIG.trust_mode.mode &&
		PORT_ACTIVE_OWNER_P->priority == priority)
    {
        UI8_T  all_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];

        /* FIXEME: swctrl shall support global configured API
         */
        COS_VM_GetAllPortList(all_port_list);
        re_value = COS_VM_SetPortListIngressPre2Dscp(all_port_list, pre, phb, color);
        if (FALSE == re_value)
        {
            PORT_CONFIG_P->common.modified = old_modified;
            PORT_ACTIVE_OWNER_P = old_active_owner_p;
            PORT_ACTIVE_P = old_active_p;
            return COS_TYPE_E_SET_TO_SWCTRL;
        }
    }

    /* update local OM before announce other CSC */
    PORT_CONFIG_P->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].phb  = phb;
    PORT_CONFIG_P->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].color = color;

    SYS_CALLBACK_MGR_AnnounceCosPortConfigChanged(SYS_MODULE_COS, COS_VM_LPORT_OF_GLOBAL, priority);

#undef CONFIG
#undef PORT_ACTIVE_P
#undef PORT_ACTIVE_OWNER_P
#undef PORT_CONFIG_P

    return COS_TYPE_E_NONE;
#endif /* SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the mapping value (phb and color) by input
 *          IP precednec.
 * INPUT:   priority - The priority of COS config, ex: COS_TYPE_PRIORITY_XX.
 *          pre      - IP precedence.
 *
 * OUTPUT:  phb_p    - phb of precedence.
 *          color_p  - color of precedence.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPre2InternalDscp(
    UI16_T priority,
    UI32_T pre,
    UI32_T *phb_p,
    UI32_T *color_p)
{
#if (SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else
    UI16_T id;
    COS_TYPE_ERROR_CODE_E result;
    COS_VM_PortEntry_T *port_entry_p;

    if (phb_p == NULL || color_p == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    if (!COS_VM_IS_VALID_IP_PRE(pre))
    {
        return COS_TYPE_E_PARAMETER;
    }

    result = COS_VM_PortPriorityToId(COS_VM_LPORT_OF_GLOBAL, priority, &id);
    if (COS_TYPE_E_NONE != result)
    {
        return result;
    }

    port_entry_p = &cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1];

    if (TRUE != port_entry_p->config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == port_entry_p->pre_to_dscp.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }

    *phb_p = port_entry_p->pre_to_dscp.config_p[id]->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].phb;
    *color_p = port_entry_p->pre_to_dscp.config_p[id]->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].color;

    return COS_TYPE_E_NONE;
#endif /* SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetActivePre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the active mapping value (phb and color) by input
 *          precedence
 * INPUT:   pre      - IP precedence.
 *
 * OUTPUT:  priority_p - The priority of COS config, ex: COS_TYPE_PRIORITY_XX.
 *          phb_p    - phb of precedence.
 *          color_p  - color of precedence.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetActivePre2InternalDscp(
    UI32_T pre,
    UI16_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
#if (SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else
    COS_VM_PortEntry_T *port_entry_p = &cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1];

    if (priority_p == NULL || phb_p == NULL || color_p == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    if (!COS_VM_IS_VALID_IP_PRE(pre))
    {
        return COS_TYPE_E_PARAMETER;
    }

    *priority_p = port_entry_p->pre_to_dscp.active_owner_p->priority;
    *phb_p = port_entry_p->pre_to_dscp.active_p->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].phb;
    *color_p = port_entry_p->pre_to_dscp.active_p->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].color;
    
    return COS_TYPE_E_NONE;
#endif /* SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT */
}

#if (__APPLE__ && __MACH__)
#pragma mark =per-port=
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the Precedence to Dscp
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          pre      - Precedence value
 *          phb      - phb value
 *          color    - color value
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortPre2InternalDscp(
    UI32_T l_port,
    UI16_T priority,
    UI32_T pre,
    UI32_T phb,
    UI32_T color)
{
#define CONFIG              (cos_vm_port_entry[l_port-1].config[id])
#define PORT_ACTIVE_P       (cos_vm_port_entry[l_port-1].pre_to_dscp.active_p)
#define PORT_ACTIVE_OWNER_P (cos_vm_port_entry[l_port-1].pre_to_dscp.active_owner_p)
#define PORT_CONFIG_P       (cos_vm_port_entry[l_port-1].pre_to_dscp.config_p[id])

    UI16_T id;
    BOOL_T old_modified;
    COS_VM_CosPortConfig_T *old_active_owner_p;
    COS_VM_PRE_TO_DSCP_CONFIG_T *old_active_p;
    COS_TYPE_ERROR_CODE_E error_code;
    BOOL_T re_value;

    if (!COS_VM_IS_VALID_IP_PRE(pre) || !COS_VM_IS_VALID_PHB(phb) || !COS_VM_IS_VALID_COLOR(color))
    {
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    error_code = COS_VM_Priv_VerifyIfindex(l_port);
    if (error_code != COS_TYPE_E_NONE)
    {
        return error_code;
    }

    if (TRUE != CONFIG.enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    old_modified = PORT_CONFIG_P->common.modified;
    if (FALSE == PORT_CONFIG_P->common.modified)
    {
        memcpy(&PORT_CONFIG_P->table,
               &PORT_ACTIVE_P->table,
               sizeof(PORT_CONFIG_P->table));
        PORT_CONFIG_P->common.modified = TRUE;
    }
    else
    {
        /* same with old configure
         */
         if ((phb == PORT_CONFIG_P->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].phb) &&
             (color == PORT_CONFIG_P->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].color))
         {
            return COS_TYPE_E_NONE;
         }
    }

    old_active_owner_p = PORT_ACTIVE_OWNER_P;
    old_active_p = PORT_ACTIVE_P;

    /* Change active */
    if (PORT_ACTIVE_OWNER_P->priority < priority)
    {
        PORT_ACTIVE_P = PORT_CONFIG_P;
        PORT_ACTIVE_OWNER_P = &CONFIG;
    }

    if (PRECEDENCE_MAPPING_MODE == CONFIG.trust_mode.mode &&
		PORT_ACTIVE_OWNER_P->priority == priority)
    {
        re_value = SWCTRL_SetQosIngressPre2Dscp(l_port, pre, phb, color);

        if (FALSE == re_value)
        {
            PORT_CONFIG_P->common.modified = old_modified;
            PORT_ACTIVE_OWNER_P = old_active_owner_p;
            PORT_ACTIVE_P = old_active_p;
            return COS_TYPE_E_SET_TO_SWCTRL;
        }
    }

    /* update local OM before announce other CSC */
    PORT_CONFIG_P->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].phb  = phb;
    PORT_CONFIG_P->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].color = color;

    SYS_CALLBACK_MGR_AnnounceCosPortConfigChanged(SYS_MODULE_COS, l_port, priority);

    return COS_TYPE_E_NONE;

#undef CONFIG
#undef PORT_ACTIVE_P
#undef PORT_ACTIVE_OWNER_P
#undef PORT_CONFIG_P
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the mapping value (phb and color) by input
 *          IP precednec.
 * INPUT:   l_port   - logic port
 *          priority - The priority of COS config, ex: COS_TYPE_PRIORITY_XX.
 *          pre      - IP precedence.
 *
 * OUTPUT:  phb_p    - phb of precedence.
 *          color_p  - color of precedence.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortPre2InternalDscp(
    UI32_T l_port,
    UI16_T priority,
    UI32_T pre,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    UI16_T id;
    COS_TYPE_ERROR_CODE_E result;

    if (phb_p == NULL || color_p == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    if (COS_VM_Priv_VerifyIfindex(l_port) != COS_TYPE_E_NONE)
    {
        return COS_TYPE_E_PARAMETER;
    }

    if (!COS_VM_IS_VALID_IP_PRE(pre))
    {
        return COS_TYPE_E_PARAMETER;
    }

    result = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != result)
    {
        return result;
    }

    if (TRUE != cos_vm_port_entry[l_port-1].config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == cos_vm_port_entry[l_port-1].pre_to_dscp.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }


    *phb_p = cos_vm_port_entry[l_port-1].pre_to_dscp.config_p[id]->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].phb;
    *color_p = cos_vm_port_entry[l_port-1].pre_to_dscp.config_p[id]->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].color;

    return COS_TYPE_E_NONE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next the mapping of ingress IP precedence to
 *            phb and color.
 * INPUT    : l_port_p - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            pre      - IP precednece vlaue
 *            phb_p    - phb value
 *            color_p  - color vlaue
 * OUTPUT   : l_port_p -
 *            phb_p    -
 *            color_p  -
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using l_port = 0, pre = COS_TYPE_UNKNOWN_PRE to get first entry.
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextPortPre2InternalDscp(
    UI32_T *l_port_p,
    UI32_T priority,
    UI32_T *pre_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    if (l_port_p == NULL || pre_p == NULL || phb_p == NULL || color_p == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    if (*l_port_p == 0)
    {
        *l_port_p = 1;
        *pre_p = MIN_PRE_VAL;
    }
    else if (*pre_p == COS_TYPE_UNKNOWN_PRE)
    {
        *pre_p = MIN_PRE_VAL;
    }
    else
    {
        *pre_p = *pre_p + 1;
    }

    for (; *l_port_p <= SWITCH_PORT_NUM; ++(*l_port_p))
    {
        for (; *pre_p <= MAX_PRE_VAL; ++(*pre_p))
        {
            COS_TYPE_ERROR_CODE_E result;

            result = COS_VM_GetPortPre2InternalDscp(*l_port_p,
                         COS_TYPE_PRIORITY_USER, *pre_p, phb_p, color_p);
            if (result == COS_TYPE_E_NONE)
            {
                return COS_TYPE_E_NONE;
            }
        }

        *pre_p = MIN_PRE_VAL;
    }

    return COS_TYPE_E_UNKNOWN;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortActivePre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the active mapping value (phb and color) by input
 *          IP precednec.
 * INPUT:   l_port   - logic port
 *          priority - The priority of COS config, ex: COS_TYPE_PRIORITY_XX.
 *          pre      - IP precedence.
 *
 * OUTPUT:  priority_p - The priority of COS config, ex: COS_TYPE_PRIORITY_XX.
 *          phb_p      - phb of precedence.
 *          color_p    - color of precedence.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortActivePre2InternalDscp(
    UI32_T l_port,
    UI32_T pre,
    UI16_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    if (phb_p == NULL || color_p == NULL || priority_p == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    if (COS_VM_Priv_VerifyIfindex(l_port) != COS_TYPE_E_NONE)
    {
        return COS_TYPE_E_PARAMETER;
    }

    if (!COS_VM_IS_VALID_IP_PRE(pre))
    {
        return COS_TYPE_E_PARAMETER;
    }

    *priority_p = cos_vm_port_entry[l_port-1].pre_to_dscp.active_owner_p->priority;
    *phb_p = cos_vm_port_entry[l_port-1].pre_to_dscp.active_p->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].phb;
    *color_p = cos_vm_port_entry[l_port-1].pre_to_dscp.active_p->table.CURRENT_PRE_TO_DSCP_MAPPING[pre].color;

    return COS_TYPE_E_NONE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetRunningPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets running config of IP precedence to internal DSCP
 *          mapping table.
 * INPUT:   l_port - Logical port id.
 * OUTPUT:  phb_ar[MAX_PRE_VAL+1]   - Array for the mapping of pre to phb.
 *          color_ar[MAX_PRE_VAL+1] - Array for the mapping of pre to color.
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetRunningPortPre2InternalDscp(
    UI32_T l_port,
    UI32_T *phb_ar,
    UI32_T *color_ar)
{
#if (SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT == TRUE)
    UI32_T pre;
    BOOL_T is_modified = FALSE;

    if (phb_ar == NULL || color_ar == NULL)
    {
        ASSERT(0);
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    if (COS_TYPE_E_NONE != COS_VM_Priv_VerifyIfindex(l_port))
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    for(pre = MIN_PRE_VAL; pre <= MAX_PRE_VAL; pre++)
    {
        UI32_T cur_phb, cur_color;

        if (COS_TYPE_E_NONE !=
            COS_VM_GetPortPre2InternalDscp(l_port, COS_TYPE_PRIORITY_USER, pre, &cur_phb, &cur_color))
        {
            ASSERT(0);
            return SYS_TYPE_GET_RUNNING_CFG_FAIL;
        }

        if (!COS_VM_IS_DEFAULT_PRE_TO_DSCP_MAP(pre, cur_phb, cur_color))
        {
            is_modified = TRUE;
        }

        phb_ar[pre] = cur_phb;
        color_ar[pre] = cur_color;
    }

    if (is_modified == TRUE)
    {
        return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }

    return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
#else
    return SYS_TYPE_GET_RUNNING_CFG_FAIL;
#endif /* (SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT == TRUE) */

}

#if (__APPLE__ && __MACH__)
#pragma mark - DSCP_DSCP
#endif

BOOL_T COS_VM_SetPortListIngressDscp2Dscp(UI8_T *port_list,UI32_T o_dscp,UI32_T phb,UI32_T color)
{
    COS_VM_USE_CSC(FALSE);

    if(!SWCTRL_SetPortListQosIngressDscp2Dscp(port_list,o_dscp, phb, color))
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    COS_VM_RELEASE_CSC();
    return TRUE;
}
BOOL_T COS_VM_ResetIngressDscp2Dscp(UI32_T lport,UI32_T dscp)
{
    COS_TYPE_ERROR_CODE_E ret;
    COS_VM_USE_CSC(FALSE);

    /* todo: check is this needed */
    /* CURRENT_QOS_IF_MAPPING_MODE[lport]  = DSCP_IF_MAPPING_MODE; */

    ret = COS_VM_SetPortDscp2InternalDscp(lport, COS_TYPE_PRIORITY_USER, dscp,
                                          DEFAULT_DSCP_TO_DSCP_MAPPING[dscp].phb,
                                          DEFAULT_DSCP_TO_DSCP_MAPPING[dscp].color);
    if(ret != COS_TYPE_E_NONE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    COS_VM_RELEASE_CSC();
    return TRUE;
}

BOOL_T COS_VM_GetIngressDscp2Dscp(UI32_T l_port,UI32_T dscp,UI32_T *phb,UI32_T *color)
{
    if (l_port < 1 ||
        l_port > (sizeof(cos_vm_port_entry)/sizeof(COS_VM_PortEntry_T)))
    {
        return FALSE;
    }

    COS_VM_USE_CSC(FALSE);

    if (NULL == cos_vm_port_entry[l_port-1].dscp_to_dscp.active_p)
    {
        return FALSE;
    }

    *phb = cos_vm_port_entry[l_port-1].dscp_to_dscp.active_p->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].phb;
    *color = cos_vm_port_entry[l_port-1].dscp_to_dscp.active_p->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].color;

    COS_VM_RELEASE_CSC();
    return TRUE;
}

#if (__APPLE__ && __MACH__)
#pragma mark =global=
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the Dscp to internal Dscp
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          dscp     - DSCP value
 *          phb      - phb value
 *          color    - color value
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetDscp2InternalDscp(
    UI16_T priority,
    UI32_T dscp,
    UI32_T phb,
    UI32_T color)
{
#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else

#define CONFIG              (cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].config[id])
#define PORT_ACTIVE_P       (cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].dscp_to_dscp.active_p)
#define PORT_ACTIVE_OWNER_P (cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].dscp_to_dscp.active_owner_p)
#define PORT_CONFIG_P       (cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].dscp_to_dscp.config_p[id])

    UI16_T id;
    BOOL_T old_modified;
    COS_VM_CosPortConfig_T *old_active_owner_p;
    COS_VM_DSCP_TO_DSCP_CONFIG_T *old_active_p;
    COS_TYPE_ERROR_CODE_E error_code;
    BOOL_T re_value;

    if (!COS_VM_IS_VALID_DSCP(dscp) || !COS_VM_IS_VALID_PHB(phb) || !COS_VM_IS_VALID_COLOR(color))
    {
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(COS_VM_LPORT_OF_GLOBAL, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != CONFIG.enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    old_modified = PORT_CONFIG_P->common.modified;
    if (FALSE == PORT_CONFIG_P->common.modified)
    {
        memcpy(&PORT_CONFIG_P->table, &PORT_ACTIVE_P->table, sizeof(PORT_CONFIG_P->table));
        PORT_CONFIG_P->common.modified = TRUE;
    }
    else
    {
        /* same with old configure
         */
        if ((phb == PORT_CONFIG_P->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].phb) &&
            (color == PORT_CONFIG_P->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].color))
        {
            return COS_TYPE_E_NONE;
        }
    }

    old_active_owner_p = PORT_ACTIVE_OWNER_P;
    old_active_p = PORT_ACTIVE_P;

    /* Change active */
    if (PORT_ACTIVE_OWNER_P->priority < priority)
    {
        PORT_ACTIVE_P = PORT_CONFIG_P;
        PORT_ACTIVE_OWNER_P = &CONFIG;
    }

    if (DSCP_IF_MAPPING_MODE == CONFIG.trust_mode.mode &&
		PORT_ACTIVE_OWNER_P->priority == priority)
    {
        UI8_T  all_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];

        /* FIXEME: swctrl shall support global configured API
         */
        COS_VM_GetAllPortList(all_port_list);
        re_value = COS_VM_SetPortListIngressDscp2Dscp(all_port_list, dscp, phb, color);
        if (FALSE == re_value)
        {
            PORT_CONFIG_P->common.modified = old_modified;
            PORT_ACTIVE_OWNER_P = old_active_owner_p;
            PORT_ACTIVE_P = old_active_p;
            return COS_TYPE_E_SET_TO_SWCTRL;
        }
    }

    /* update local OM before announce other CSC */
    PORT_CONFIG_P->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].phb = phb;
    PORT_CONFIG_P->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].color = color;

    SYS_CALLBACK_MGR_AnnounceCosPortConfigChanged(SYS_MODULE_COS, COS_VM_LPORT_OF_GLOBAL, priority);

    return COS_TYPE_E_NONE;

#undef CONFIG
#undef PORT_ACTIVE_P
#undef PORT_ACTIVE_OWNER_P
#undef PORT_CONFIG_P

#endif /* SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the internal Dscp for Dscp
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          dscp     - dscp value
 * OUTPUT:  phb      - phb of dscp
 *          color    - color of dscp
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetDscp2InternalDscp(
    UI16_T priority,
    UI32_T dscp,
    UI32_T *phb_p,
    UI32_T *color_p)
{
#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else

    UI32_T l_port = COS_VM_LPORT_OF_GLOBAL;
    UI16_T id;
    COS_TYPE_ERROR_CODE_E error_code;

    if (phb_p == NULL || color_p == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    if (!COS_VM_IS_VALID_DSCP(dscp))
    {
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != cos_vm_port_entry[l_port-1].config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == cos_vm_port_entry[l_port-1].dscp_to_dscp.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }

    *phb_p = cos_vm_port_entry[l_port-1].dscp_to_dscp.config_p[id]->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].phb;
    *color_p = cos_vm_port_entry[l_port-1].dscp_to_dscp.config_p[id]->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].color;

    return COS_TYPE_E_NONE;
#endif /* SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next ingress dscp to internal dscp mapping.
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          dscp_p   - dscp value
 *
 * OUTPUT:  dscp_p     - dscp value
 *          phb_p      - phb of dscp
 *          color_p    - color of dscp
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextDscp2InternalDscp(
    UI16_T priority,
    UI32_T *dscp_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else
    if (phb_p == NULL || color_p == NULL || dscp_p == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    if (*dscp_p == COS_TYPE_UNKNOWN_DSCP)
    {
        *dscp_p = MIN_DSCP_VAL;
    }
    else
    {
        *dscp_p += 1;
    }

    for (; *dscp_p <= MAX_DSCP_VAL; ++(*dscp_p))
    {
        if (COS_TYPE_E_NONE == COS_VM_GetDscp2InternalDscp(priority,
                                   *dscp_p, phb_p, color_p))
        {
            return COS_TYPE_E_NONE;
        }

        COS_VM_DEBUG_PRINTF("Fail to get internal dscp with priority = %u, dscp = %lu",
                            priority, *dscp_p);
    }

    return COS_TYPE_E_UNKNOWN;
#endif /* SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetActiveDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the internal Dscp for Dscp
 * INPUT:   dscp     - dscp value
 *
 * OUTPUT:  priority_p - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb_p      - phb of dscp
 *          color_p    - color of dscp
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetActiveDscp2InternalDscp(
    UI32_T dscp,
    UI16_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else

    UI32_T l_port = COS_VM_LPORT_OF_GLOBAL;

    if (phb_p == NULL || color_p == NULL || priority_p == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    if (!COS_VM_IS_VALID_DSCP(dscp))
    {
        return COS_TYPE_E_PARAMETER;
    }

    *priority_p = cos_vm_port_entry[l_port-1].dscp_to_dscp.active_owner_p->priority;
    *phb_p = cos_vm_port_entry[l_port-1].dscp_to_dscp.active_p->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].phb;
    *color_p = cos_vm_port_entry[l_port-1].dscp_to_dscp.active_p->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].color;

    return COS_TYPE_E_NONE;
#endif /* SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextRunningDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next running config of dscp by input phb
 *          and color.
 * INPUT:   phb     - PHB
 *          color   - Color
 * OUTPUT:  dscp_p - ingress dscp value.
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetNextRunningDscp2InternalDscp(
    UI32_T phb,
    UI32_T color,
    UI32_T *dscp_p)
{
#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE)
    return SYS_TYPE_GET_RUNNING_CFG_FAIL;
#else
    UI32_T dscp;
    UI32_T temp_phb, temp_color;

    if (NULL == dscp_p)
    {
        ASSERT(0);
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    if (!COS_VM_IS_VALID_PHB(phb) || !COS_VM_IS_VALID_COLOR(color))
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    dscp = *dscp_p;

    while (COS_TYPE_E_NONE ==
           COS_VM_GetNextDscp2InternalDscp(COS_TYPE_PRIORITY_USER, &dscp, &temp_phb, &temp_color))
    {
        if(phb == temp_phb && color == temp_color &&
           !COS_VM_IS_DEFAULT_DSCP_TO_DSCP_MAP(dscp, temp_phb, temp_color))
        {
            *dscp_p = dscp;
            return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
        }
    }

    if (*dscp_p == COS_TYPE_UNKNOWN_DSCP)
    {
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }

#endif /* SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT */
    return SYS_TYPE_GET_RUNNING_CFG_FAIL;
}

#if (__APPLE__ && __MACH__)
#pragma mark =per-port=
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the Dscp to internal Dscp
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          dscp     - DSCP value
 *          phb      - phb value
 *          color    - color value
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortDscp2InternalDscp(
    UI32_T l_port,
    UI16_T priority,
    UI32_T dscp,
    UI32_T phb,
    UI32_T color)
{
#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE)

#define CONFIG              (cos_vm_port_entry[l_port-1].config[id])
#define PORT_ACTIVE_P       (cos_vm_port_entry[l_port-1].dscp_to_dscp.active_p)
#define PORT_ACTIVE_OWNER_P (cos_vm_port_entry[l_port-1].dscp_to_dscp.active_owner_p)
#define PORT_CONFIG_P       (cos_vm_port_entry[l_port-1].dscp_to_dscp.config_p[id])

    UI16_T id;
    BOOL_T old_modified;
    COS_VM_CosPortConfig_T *old_active_owner_p;
    COS_VM_DSCP_TO_DSCP_CONFIG_T *old_active_p;
    COS_TYPE_ERROR_CODE_E error_code;
    BOOL_T re_value;

    if (!COS_VM_IS_VALID_DSCP(dscp) || !COS_VM_IS_VALID_PHB(phb) || !COS_VM_IS_VALID_COLOR(color))
    {
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_Priv_VerifyIfindex(l_port);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != CONFIG.enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    old_modified = PORT_CONFIG_P->common.modified;
    if (FALSE == PORT_CONFIG_P->common.modified)
    {
        memcpy(&PORT_CONFIG_P->table, &PORT_ACTIVE_P->table, sizeof(PORT_CONFIG_P->table));
        PORT_CONFIG_P->common.modified = TRUE;
    }
    else
    {
        /* same with old configure
         */
         if ((phb == PORT_CONFIG_P->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].phb) &&
             (color == PORT_CONFIG_P->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].color))
         {
            return COS_TYPE_E_NONE;
         }
    }

    old_active_owner_p = PORT_ACTIVE_OWNER_P;
    old_active_p = PORT_ACTIVE_P;

    /* Change active */
    if (PORT_ACTIVE_OWNER_P->priority < priority)
    {
        PORT_ACTIVE_P = PORT_CONFIG_P;
        PORT_ACTIVE_OWNER_P = &CONFIG;
    }

    if (DSCP_IF_MAPPING_MODE == CONFIG.trust_mode.mode &&
		PORT_ACTIVE_OWNER_P->priority == priority)
    {
        re_value = SWCTRL_SetQosIngressDscp2Dscp(l_port, dscp, phb, color);
        if (FALSE == re_value)
        {
            PORT_CONFIG_P->common.modified = old_modified;
            PORT_ACTIVE_OWNER_P = old_active_owner_p;
            PORT_ACTIVE_P = old_active_p;
            return COS_TYPE_E_SET_TO_SWCTRL;
        }
    }

    /* update local OM before announce other CSC */
    PORT_CONFIG_P->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].phb = phb;
    PORT_CONFIG_P->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].color = color;

    SYS_CALLBACK_MGR_AnnounceCosPortConfigChanged(SYS_MODULE_COS, l_port, priority);

    return COS_TYPE_E_NONE;

#undef CONFIG
#undef PORT_ACTIVE_P
#undef PORT_ACTIVE_OWNER_P
#undef PORT_CONFIG_P

#else
    return COS_TYPE_E_UNKNOWN;
#endif /* SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the internal Dscp for Dscp
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          dscp     - dscp value
 * OUTPUT:  phb      - phb of dscp
 *          color    - color of dscp
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortDscp2InternalDscp(
    UI32_T l_port,
    UI16_T priority,
    UI32_T dscp,
    UI32_T *phb,
    UI32_T *color)
{
#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE)

    UI16_T id;
    COS_TYPE_ERROR_CODE_E error_code;

    if (phb == NULL || color == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_Priv_VerifyIfindex(l_port);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (!COS_VM_IS_VALID_DSCP(dscp))
    {
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != cos_vm_port_entry[l_port-1].config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == cos_vm_port_entry[l_port-1].dscp_to_dscp.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }

    *phb = cos_vm_port_entry[l_port-1].dscp_to_dscp.config_p[id]->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].phb;
    *color = cos_vm_port_entry[l_port-1].dscp_to_dscp.config_p[id]->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].color;

    return COS_TYPE_E_NONE;

#else
    return COS_TYPE_E_UNKNOWN;
#endif /* */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next ingress dscp to internal dscp mapping.
 * INPUT:   l_port_p - logical port index
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          dscp_p   - dscp value
 *
 * OUTPUT:  l_port_p   - current logical port index
 *          dscp_p     - dscp value
 *          phb_p      - phb of dscp
 *          color_p    - color of dscp
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextPortDscp2InternalDscp(
    UI32_T *l_port_p,
    UI16_T priority,
    UI32_T *dscp_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE)
    if (l_port_p == NULL || phb_p == NULL || color_p == NULL || dscp_p == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    if (*l_port_p == 0)
    {
        *l_port_p = 1;
        *dscp_p = MIN_DSCP_VAL;
    }
    else if (*dscp_p == COS_TYPE_UNKNOWN_DSCP)
    {
        *dscp_p = MIN_DSCP_VAL;
    }
    else
    {
        *dscp_p = *dscp_p + 1;
    }

    for (; *l_port_p <= SWITCH_PORT_NUM; ++(*l_port_p))
    {
        for (; *dscp_p <= MAX_DSCP_VAL; ++(*dscp_p))
        {
            if (COS_TYPE_E_NONE == COS_VM_GetPortDscp2InternalDscp(*l_port_p,
                                       priority, *dscp_p, phb_p, color_p))
            {
                return COS_TYPE_E_NONE;
            }
        }

        *dscp_p = MIN_DSCP_VAL;
    }

#endif /* SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT */

    return COS_TYPE_E_UNKNOWN;

}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortActiveDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the internal Dscp for Dscp
 * INPUT:   l_port - logical port index
 *          dscp   - dscp value
 *
 * OUTPUT:  priority_p - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb_p      - phb of dscp
 *          color_p    - color of dscp
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortActiveDscp2InternalDscp(
    UI32_T l_port,
    UI32_T dscp,
    UI16_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE)
    if (phb_p == NULL || color_p == NULL || priority_p == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    if (COS_VM_Priv_VerifyIfindex(l_port) != COS_TYPE_E_NONE)
    {
        return COS_TYPE_E_PARAMETER;
    }

    if (!COS_VM_IS_VALID_DSCP(dscp))
    {
        return COS_TYPE_E_PARAMETER;
    }

    *priority_p = cos_vm_port_entry[l_port-1].dscp_to_dscp.active_owner_p->priority;
    *phb_p = cos_vm_port_entry[l_port-1].dscp_to_dscp.active_p->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].phb;
    *color_p = cos_vm_port_entry[l_port-1].dscp_to_dscp.active_p->table.CURRENT_DSCP_TO_DSCP_MAPPING[dscp].color;

    return COS_TYPE_E_NONE;
#else
    return COS_TYPE_E_UNKNOWN;
#endif /* SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextRunningPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the running config of phb and color for internal
 *          DSCP
 * INPUT:   l_port  - Logical port id
 *          phb     - PHB
 *          color   - Color
 *          l_dscp  - The first ingress dscp whitch is used for search the mapping.
 *
 * OUTPUT:  l_dscp     -
 *          dscp_entry - ingress dscp array, dscp_entry[8]
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetNextRunningPortDscp2InternalDscp(
    UI32_T l_port,
    UI32_T phb,
    UI32_T color,
    UI32_T *dscp_p)
{
#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE)
    UI32_T dscp;
    UI32_T temp_lport, temp_phb, temp_color;

    if (NULL == dscp_p)
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    if (COS_TYPE_E_NONE != COS_VM_Priv_VerifyIfindex(l_port))
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    if (!COS_VM_IS_VALID_PHB(phb) || !COS_VM_IS_VALID_COLOR(color))
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    temp_lport = l_port;
    dscp = *dscp_p;

    while (COS_TYPE_E_NONE ==
           COS_VM_GetNextPortDscp2InternalDscp(&temp_lport, COS_TYPE_PRIORITY_USER, &dscp, &temp_phb, &temp_color))
    {
        if (temp_lport != l_port)
        {
            break;
        }

        if(phb == temp_phb && color == temp_color &&
           !COS_VM_IS_DEFAULT_DSCP_TO_DSCP_MAP(dscp, temp_phb, temp_color))
        {
            *dscp_p = dscp;
            return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
        }
    }

    if (*dscp_p == COS_TYPE_UNKNOWN_DSCP)
    {
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }
#endif /* SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT */
    return SYS_TYPE_GET_RUNNING_CFG_FAIL;
}

#if (__APPLE__ && __MACH__)
#pragma mark - PHB_QUEUE
#endif

BOOL_T
COS_VM_SetPortListIngressDscp2Queue(
    UI8_T *port_list,
    UI32_T phb,
    UI32_T queue)
{
    COS_VM_USE_CSC(FALSE);

    if(!SWCTRL_SetPortListQosIngressDscp2Queue(port_list,phb,queue))
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    COS_VM_RELEASE_CSC();
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_ResetIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: Reset the global mapping of ingress DSCP to Queue
 * INPUT:   l_port
 *          phb
 * OUTPUT:  None.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    This API only sets the user config
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_ResetIngressDscp2Queue(
    UI32_T phb)
{
    COS_TYPE_ERROR_CODE_E ret;

    COS_VM_USE_CSC(COS_TYPE_E_UNKNOWN);
    ret = COS_VM_SetIngressPhb2Queue(COS_TYPE_PRIORITY_USER, phb, DEFAULT_DSCP_TO_QUEUE_MAPPING[phb]);

    COS_VM_RELEASE_CSC();
    return ret;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_ResetPortIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: Reset the mapping of ingress DSCP to Queue by port
 * INPUT:   l_port
 *          phb
 * OUTPUT:  None.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    This API only sets the user config
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_ResetPortIngressDscp2Queue(
    UI32_T l_port,
    UI32_T phb)
{
    COS_TYPE_ERROR_CODE_E ret;

    COS_VM_USE_CSC(COS_TYPE_E_UNKNOWN);
    ret = COS_VM_SetPortIngressPhb2Queue(l_port, COS_TYPE_PRIORITY_USER,
                                         phb, DEFAULT_DSCP_TO_QUEUE_MAPPING[phb]);

    COS_VM_RELEASE_CSC();
    return ret;
}

#if (__APPLE__ && __MACH__)
#pragma mark =Global=
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the global mapping of dscp and queue
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb      - phb value
 *          queue    - queue value
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetIngressPhb2Queue(
    UI16_T priority,
    UI32_T phb,
    UI32_T queue)
{
#if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT)
    return COS_TYPE_E_UNKNOWN;
#else
#define CONFIG              (cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].config[id])
#define PORT_ACTIVE_P       (cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].dscp_to_queue.active_p)
#define PORT_ACTIVE_OWNER_P (cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].dscp_to_queue.active_owner_p)
#define PORT_CONFIG_P       (cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].dscp_to_queue.config_p[id])

    UI16_T id;
    BOOL_T old_modified;
    COS_VM_CosPortConfig_T *old_active_owner_p;
    COS_VM_DSCP_TO_QUEUE_CONFIG_T *old_active_p;
    COS_TYPE_ERROR_CODE_E error_code;
    BOOL_T re_value;

    COS_VM_DEBUG_PRINTF("Input l_port:Global, priority:%d, phb:%lu, queue:%lu",
                        priority, phb, queue);

    if (!COS_VM_IS_VALID_PHB(phb) || !COS_VM_IS_VALID_QUEUE(queue))
    {
        COS_VM_DEBUG_ERROR_MSG(COS_TYPE_E_PARAMETER);
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(COS_VM_LPORT_OF_GLOBAL, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        COS_VM_DEBUG_ERROR_MSG(error_code);
        return error_code;
    }

    if (TRUE != CONFIG.enable)
    {
        COS_VM_DEBUG_ERROR_MSG(COS_TYPE_E_PRIORITY_NOT_ENABLED);
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    old_modified = PORT_CONFIG_P->common.modified;
    if (FALSE == PORT_CONFIG_P->common.modified)
    {
        memcpy(&PORT_CONFIG_P->table,
               &PORT_ACTIVE_P->table,
               sizeof(PORT_CONFIG_P->table));
        PORT_CONFIG_P->common.modified = TRUE;
    }
    else
    {
        /* same with old configure
         */
         if (queue == PORT_CONFIG_P->table.CURRENT_DSCP_TO_QUEUE_MAPPING[phb])
         {
            return COS_TYPE_E_NONE;
         }
    }

    old_active_owner_p = PORT_ACTIVE_OWNER_P;
    old_active_p = PORT_ACTIVE_P;

    /* Change active */
    if (PORT_ACTIVE_OWNER_P->priority < priority)
    {
        PORT_ACTIVE_P = PORT_CONFIG_P;
        PORT_ACTIVE_OWNER_P = &CONFIG;
    }

    if (PORT_ACTIVE_OWNER_P->priority == priority)
    {
        UI8_T  all_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];

        COS_VM_GetAllPortList(all_port_list);
        re_value = COS_VM_SetPortListIngressDscp2Queue(all_port_list, phb, queue);
        if (TRUE != re_value)
        {
            PORT_CONFIG_P->common.modified = old_modified;
            PORT_ACTIVE_OWNER_P = old_active_owner_p;
            PORT_ACTIVE_P = old_active_p;

            COS_VM_DEBUG_ERROR_MSG(COS_TYPE_E_SET_TO_SWCTRL);
            return COS_TYPE_E_SET_TO_SWCTRL;;
        }
    }

    /* update local OM before announce other CSC */
    PORT_CONFIG_P->table.CURRENT_DSCP_TO_QUEUE_MAPPING[phb] = (UI8_T)queue;

    SYS_CALLBACK_MGR_AnnounceCosPortConfigChanged(SYS_MODULE_COS, COS_VM_LPORT_OF_GLOBAL, priority);

    return COS_TYPE_E_NONE;

#undef CONFIG
#undef PORT_ACTIVE_P
#undef PORT_ACTIVE_OWNER_P
#undef PORT_CONFIG_P
#endif /* #if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue for all Dscp
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb      - phb
 * OUTPUT:  queue_p  - The queue id mapping to the input phb.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetIngressPhb2Queue(
    UI16_T priority,
    UI32_T phb,
    UI32_T *queue_p)
{
#if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT)
    return COS_TYPE_E_UNKNOWN;
#else
    UI16_T id;
    COS_TYPE_ERROR_CODE_E error_code;

    if (queue_p == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    if (!COS_VM_IS_VALID_PHB(phb))
    {
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(COS_VM_LPORT_OF_GLOBAL, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].dscp_to_queue.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }

    *queue_p = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].dscp_to_queue.config_p[id]->table.CURRENT_DSCP_TO_QUEUE_MAPPING[phb];

    return COS_TYPE_E_NONE;
#endif /* #if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue for all Dscp
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb_p    - phb
 * OUTPUT:  phb_p    - phb
 *          queue_p  - The queue id mapping to the input phb.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Using phb = COS_TYPE_UNKNOWN_PHB to get first.
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextIngressPhb2Queue(
    UI16_T priority,
    UI32_T *phb_p,
    UI32_T *queue_p)
{
#if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT)
    return COS_TYPE_E_UNKNOWN;
#else
    if (phb_p == NULL || queue_p == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    if (*phb_p == COS_TYPE_UNKNOWN_PHB)
    {
        *phb_p = MIN_PHB_VAL;
    }
    else
    {
        *phb_p = *phb_p + 1;
    }

    for (; *phb_p <= MAX_PHB_VAL; ++(*phb_p))
    {
        if (COS_TYPE_E_NONE ==
            COS_VM_GetIngressPhb2Queue(priority, *phb_p, queue_p))
        {
            return COS_TYPE_E_NONE;
        }
    }

    return COS_TYPE_E_UNKNOWN;
#endif /* #if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetActiveIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets active config of queue mapping to the input phb.
 * INPUT:   phb - phb
 * OUTPUT:  priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue    - mapping table of dscp to queue , queue[MAX_PHB_VAL+1]
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Return the acitved configuration.
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetActiveIngressPhb2Queue(
    UI32_T phb,
    UI16_T *priority_p,
    UI32_T *queue_p)
{
#if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT)
    return COS_TYPE_E_UNKNOWN;
#else
    if (priority_p == NULL || queue_p == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    if (!COS_VM_IS_VALID_PHB(phb))
    {
        return COS_TYPE_E_PARAMETER;
    }

    *priority_p = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].dscp_to_queue.active_owner_p->priority;
    *queue_p = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].dscp_to_queue.active_p->table.CURRENT_DSCP_TO_QUEUE_MAPPING[phb];

    return COS_TYPE_E_NONE;
#endif /* #if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextRunningIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: Get next running config of queue id which is mapping to input phb.
 * INPUT:   queue - queue id.
 * OUTPUT:  phb_p - phb mapping to the input queue id.
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetNextRunningIngressPhb2Queue(
    UI32_T queue,
    UI32_T *phb_p)
{
#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT == FALSE)
    UI32_T phb;
    UI32_T temp_queue;

    if (phb_p == NULL)
    {
        ASSERT(0);
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    if (!COS_VM_IS_VALID_QUEUE(queue))
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    phb = *phb_p;

    while (COS_TYPE_E_NONE ==
           COS_VM_GetNextIngressPhb2Queue(COS_TYPE_PRIORITY_USER, &phb, &temp_queue))
    {
        if (temp_queue == queue &&
            !COS_VM_IS_DEFAULT_PHB_TO_QUEUE_MAP(phb, temp_queue))
        {
            *phb_p = phb;
            return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
        }
    }

    if (*phb_p == COS_TYPE_UNKNOWN_PHB)
    {
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }
#endif /* #if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT) */
    return SYS_TYPE_GET_RUNNING_CFG_FAIL;
}

#if (__APPLE__ && __MACH__)
#pragma mark =per-port=
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the mapping of dscp and queue by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb      - phb value
 *          queue    - queue value
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortIngressPhb2Queue(
    UI32_T l_port,
    UI16_T priority,
    UI32_T phb,
    UI32_T queue)
{
#if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT)
#define CONFIG              (cos_vm_port_entry[l_port-1].config[id])
#define PORT_ACTIVE_P       (cos_vm_port_entry[l_port-1].dscp_to_queue.active_p)
#define PORT_ACTIVE_OWNER_P (cos_vm_port_entry[l_port-1].dscp_to_queue.active_owner_p)
#define PORT_CONFIG_P       (cos_vm_port_entry[l_port-1].dscp_to_queue.config_p[id])

    UI16_T id;
    BOOL_T old_modified;
    COS_VM_CosPortConfig_T *old_active_owner_p;
    COS_VM_DSCP_TO_QUEUE_CONFIG_T *old_active_p;
    COS_TYPE_ERROR_CODE_E error_code;
    BOOL_T re_value;

    COS_VM_DEBUG_PRINTF("Input l_port:%lu, priority:%d, phb:%lu, queue:%lu",
                        l_port, priority, phb, queue);

    error_code = COS_VM_Priv_VerifyIfindex(l_port);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    error_code = COS_VM_Priv_VerifyIfindex(l_port);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (!COS_VM_IS_VALID_PHB(phb) || !COS_VM_IS_VALID_QUEUE(queue))
    {
        COS_VM_DEBUG_ERROR_MSG(COS_TYPE_E_PARAMETER);
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        COS_VM_DEBUG_ERROR_MSG(error_code);
        return error_code;
    }

    if (TRUE != CONFIG.enable)
    {
        COS_VM_DEBUG_ERROR_MSG(COS_TYPE_E_PRIORITY_NOT_ENABLED);
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    old_modified = PORT_CONFIG_P->common.modified;
    if (FALSE == PORT_CONFIG_P->common.modified)
    {
        memcpy(&PORT_CONFIG_P->table,
               &PORT_ACTIVE_P->table,
               sizeof(PORT_CONFIG_P->table));
        PORT_CONFIG_P->common.modified = TRUE;
    }
    else
    {
        /* same with old configure
         */
         if (queue == PORT_CONFIG_P->table.CURRENT_DSCP_TO_QUEUE_MAPPING[phb])
         {
            return COS_TYPE_E_NONE;
         }
    }

    old_active_owner_p = PORT_ACTIVE_OWNER_P;
    old_active_p = PORT_ACTIVE_P;

    /* Change active */
    if (PORT_ACTIVE_OWNER_P->priority < priority)
    {
        PORT_ACTIVE_P = PORT_CONFIG_P;
        PORT_ACTIVE_OWNER_P = &CONFIG;
    }

    if (PORT_ACTIVE_OWNER_P->priority == priority)
    {
        re_value = SWCTRL_SetQosIngressDscp2Queue(l_port, phb, queue);
        if (FALSE == re_value)
        {
            PORT_CONFIG_P->common.modified = old_modified;
            PORT_ACTIVE_OWNER_P = old_active_owner_p;
            PORT_ACTIVE_P = old_active_p;

            COS_VM_DEBUG_ERROR_MSG(COS_TYPE_E_SET_TO_SWCTRL);
            return COS_TYPE_E_SET_TO_SWCTRL;;
        }
    }

    /* update local OM before announce other CSC */
    PORT_CONFIG_P->table.CURRENT_DSCP_TO_QUEUE_MAPPING[phb] = queue;

    SYS_CALLBACK_MGR_AnnounceCosPortConfigChanged(SYS_MODULE_COS, l_port, priority);


    return COS_TYPE_E_NONE;

#undef CONFIG
#undef PORT_ACTIVE_P
#undef PORT_ACTIVE_OWNER_P
#undef PORT_CONFIG_P
#else
    return COS_TYPE_E_UNKNOWN;
#endif /* #if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue id mapping to the input phb
 * INPUT:   l_port - Logical port id.
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb      - phb
 * OUTPUT:  queue_p  - The queue id mapping to the input phb.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortIngressPhb2Queue(
    UI32_T l_port,
    UI16_T priority,
    UI32_T phb,
    UI32_T *queue_p)
{
#if (TRUE != SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT)
    return COS_TYPE_E_UNKNOWN;
#else
    UI16_T id;
    COS_TYPE_ERROR_CODE_E result;

    if (queue_p == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    if (!COS_VM_IS_VALID_PHB(phb))
    {
        return COS_TYPE_E_PARAMETER;
    }

    result = COS_VM_Priv_VerifyIfindex(l_port);
    if (COS_TYPE_E_NONE != result)
    {
        return result;
    }

    result = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != result)
    {
        return result;
    }

    if (TRUE != cos_vm_port_entry[l_port-1].config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == cos_vm_port_entry[l_port-1].dscp_to_queue.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }

    *queue_p = cos_vm_port_entry[l_port-1].dscp_to_queue.config_p[id]->table.CURRENT_DSCP_TO_QUEUE_MAPPING[phb];

    return COS_TYPE_E_NONE;
#endif /* #if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets nexts mapping of phb to queue
 * INPUT:   l_port_p - Logical port id.
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb_p    - phb
 *
 * OUTPUT:  lport    -
 *          phb_p    - phb
 *          queue_p  - The queue id mapping to the input phb.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Using l_port = 0, phb = COS_TYPE_UNKNOWN_PHB to get first.
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextPortIngressPhb2Queue(
    UI32_T *l_port_p,
    UI16_T priority,
    UI32_T *phb_p,
    UI32_T *queue_p)
{
#if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT)
    if (phb_p == NULL || queue_p == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    if (*l_port_p == 0)
    {
        *l_port_p = 1;
        *phb_p = MIN_PHB_VAL;
    }
    else if (*phb_p == COS_TYPE_UNKNOWN_PHB)
    {
        *phb_p = MIN_PHB_VAL;
    }
    else
    {
        *phb_p = *phb_p + 1;
    }

    for (; *l_port_p <= SWITCH_PORT_NUM; ++(*l_port_p))
    {
        for (; *phb_p <= MAX_PHB_VAL; ++(*phb_p))
        {
            if (COS_TYPE_E_NONE ==
                COS_VM_GetPortIngressPhb2Queue(*l_port_p, priority, *phb_p, queue_p))
            {
                return COS_TYPE_E_NONE;
            }
        }

        *phb_p = MIN_PHB_VAL;
    }

#endif /* SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT */
    return COS_TYPE_E_UNKNOWN;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortActiveIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the active config of queue by input phb.
 * INPUT:   l_port - Logical port id.
 *          phb      - phb
 * OUTPUT:  priority_p - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue_p    - The queue id mapping to the input phb.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortActiveIngressPhb2Queue(
    UI32_T l_port,
    UI32_T phb,
    UI16_T *priority_p,
    UI32_T *queue_p)
{
#if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT)

    if (priority_p == NULL || queue_p == NULL)
    {
        ASSERT(0);
        return COS_TYPE_E_PARAMETER;
    }

    if (!COS_VM_IS_VALID_PHB(phb))
    {
        return COS_TYPE_E_PARAMETER;
    }

    if (COS_VM_Priv_VerifyIfindex(l_port) != COS_TYPE_E_NONE)
    {
        return COS_TYPE_E_PARAMETER;
    }

    *priority_p = cos_vm_port_entry[l_port-1].dscp_to_queue.active_owner_p->priority;
    *queue_p = cos_vm_port_entry[l_port-1].dscp_to_queue.active_p->table.CURRENT_DSCP_TO_QUEUE_MAPPING[phb];

    return COS_TYPE_E_NONE;
#else
    return COS_TYPE_E_UNKNOWN;
#endif /* #if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextRunningPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: Get next running config of queue id which is mapping to input phb.
 * INPUT:   l_port - Logical port
 *          queue - queue id.
 * OUTPUT:  phb_p - phb mapping to the input queue id.
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetNextRunningPortIngressPhb2Queue(
    UI32_T l_port,
    UI32_T queue,
    UI32_T *phb_p)
{
#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT == TRUE)
    UI32_T phb;
    UI32_T temp_lport, temp_queue;

    if (phb_p == NULL)
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    temp_lport = l_port;
    phb = *phb_p;

    while (COS_TYPE_E_NONE ==
           COS_VM_GetNextPortIngressPhb2Queue(&temp_lport, COS_TYPE_PRIORITY_USER, &phb, &temp_queue))
    {
        if (l_port != temp_lport)
        {
            break;
        }

        if (temp_queue == queue &&
            !COS_VM_IS_DEFAULT_PHB_TO_QUEUE_MAP(phb, temp_queue))
        {
            *phb_p = phb;
            return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
        }
    }

    if (*phb_p == COS_TYPE_UNKNOWN_PHB)
    {
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }
#endif /* #if (TRUE == SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT) */
    return SYS_TYPE_GET_RUNNING_CFG_FAIL;
}

#if (__APPLE__ && __MACH__)
#pragma mark - DSCP_COLOR
#endif

BOOL_T COS_VM_SetIngressDscp2Color(UI32_T lport,UI32_T phb,UI32_T color)
{
    COS_VM_USE_CSC(FALSE);

    if (COS_TYPE_E_NONE != COS_VM_SetPortPriorityIngressDscp2Color(lport, COS_VM_PRIORITY_USER, phb, color))
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    COS_VM_RELEASE_CSC();
    return TRUE;
}

BOOL_T COS_VM_ResetIngressDscp2Color(UI32_T lport,UI32_T phb)
{
    UI32_T ret;
    COS_VM_USE_CSC(FALSE);
    ret = COS_VM_SetIngressDscp2Color(lport,phb,DEFAULT_DSCP_TO_COLOR_MAPPING[phb]);
    if(ret != TRUE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    COS_VM_RELEASE_CSC();
    return TRUE;
}

BOOL_T COS_VM_SetPortListIngressDscp2Color(UI8_T *port_list,UI32_T phb,UI32_T color)
{
    COS_VM_USE_CSC(FALSE);


    if(!SWCTRL_SetPortListQosIngressDscp2Color(port_list,phb,color))
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    COS_VM_RELEASE_CSC();
    return TRUE;
}
BOOL_T COS_VM_GetIngressDscp2Color(UI32_T l_port,UI8_T *color)
{
    UI32_T pre;

    if (l_port < 1 ||
        l_port > (sizeof(cos_vm_port_entry)/sizeof(COS_VM_PortEntry_T)))
    {
        return FALSE;
    }

    COS_VM_USE_CSC(FALSE);

    for(pre = 0;pre <= MAX_PHB_VAL;pre++)
    {
        *(color+pre)=  cos_vm_port_entry[l_port-1].dscp_to_color.active_p->table.CURRENT_DSCP_TO_COLOR_MAPPING[pre];
    }
    COS_VM_RELEASE_CSC();
    return TRUE;
}

BOOL_T COS_VM_GetRunningIngressDscp2Color(UI32_T lport,UI8_T *color)
{
    UI32_T pre;

    if (lport < 1 ||
        lport > (sizeof(cos_vm_port_entry)/sizeof(COS_VM_PortEntry_T)))
    {
        return FALSE;
    }

    COS_VM_USE_CSC(FALSE);
    for(pre = 0;pre <= MAX_PHB_VAL;pre++)
    {
        *(color+pre)= cos_vm_port_entry[lport-1].config[COS_VM_DFLT_USER_PRIORITY_INDEX].dscp_to_color.table.CURRENT_DSCP_TO_COLOR_MAPPING[pre];

        if(*(color+pre) == DEFAULT_DSCP_TO_COLOR_MAPPING[pre])
        {
            *(color+pre) = 4;
        }
    }
    COS_VM_RELEASE_CSC();
    return TRUE;
}

#if (__APPLE__ && __MACH__)
#pragma mark =per-port=
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortPriorityIngressDscp2Color
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the mapping of dscp and color
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb      - phb value
 *          color    - color value
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortPriorityIngressDscp2Color(
    UI32_T l_port,
    UI16_T priority,
    UI32_T phb,
    UI32_T color)
{
#define CONFIG              (cos_vm_port_entry[l_port-1].config[id])
#define PORT_ACTIVE_P       (cos_vm_port_entry[l_port-1].dscp_to_color.active_p)
#define PORT_ACTIVE_OWNER_P (cos_vm_port_entry[l_port-1].dscp_to_color.active_owner_p)
#define PORT_CONFIG_P       (cos_vm_port_entry[l_port-1].dscp_to_color.config_p[id])

    UI32_T unit,port,trunk_id;
    UI16_T id;
    BOOL_T old_modified;
    SWCTRL_Lport_Type_T  port_type;
    COS_VM_CosPortConfig_T *old_active_owner_p;
    COS_VM_DSCP_TO_COLOR_CONFIG_T *old_active_p;
    COS_TYPE_ERROR_CODE_E error_code;
    BOOL_T re_value;

    if (phb > MAX_PHB_VAL)
    {
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != CONFIG.enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    /* not support trunk now */
    port_type = SWCTRL_LogicalPortToUserPort(l_port,&unit,&port,&trunk_id);
    if(port_type == SWCTRL_LPORT_TRUNK_PORT)
    {
        return COS_TYPE_E_TRUNCK_PORT;
    }

    old_modified = PORT_CONFIG_P->common.modified;
    if (FALSE == PORT_CONFIG_P->common.modified)
    {
        memcpy(&PORT_CONFIG_P->table,
               &PORT_ACTIVE_P->table,
               sizeof(PORT_CONFIG_P->table));
        PORT_CONFIG_P->common.modified = TRUE;
    }
    else
    {
        /* same with old configure
         */
         if (color == PORT_CONFIG_P->table.CURRENT_DSCP_TO_COLOR_MAPPING[phb])
         {
            return COS_TYPE_E_NONE;
         }
    }

    old_active_owner_p = PORT_ACTIVE_OWNER_P;
    old_active_p = PORT_ACTIVE_P;

    /* Change active */
    if (PORT_ACTIVE_OWNER_P->priority < priority)
    {
        PORT_ACTIVE_P = PORT_CONFIG_P;
        PORT_ACTIVE_OWNER_P = &CONFIG;
    }

    if (PORT_ACTIVE_OWNER_P->priority == priority)
    {
        re_value = SWCTRL_SetQosIngressDscp2Color(l_port, phb, color);
        if (FALSE == re_value)
        {
            PORT_CONFIG_P->common.modified = old_modified;
            PORT_ACTIVE_OWNER_P = old_active_owner_p;
            PORT_ACTIVE_P = old_active_p;
            return COS_TYPE_E_SET_TO_SWCTRL;;
        }
    }

    /* update local OM before announce other CSC */
    PORT_CONFIG_P->table.CURRENT_DSCP_TO_COLOR_MAPPING[phb] = (UI8_T)color;

    SYS_CALLBACK_MGR_AnnounceCosPortConfigChanged(SYS_MODULE_COS, l_port, priority);

    return COS_TYPE_E_NONE;

#undef CONFIG
#undef PORT_ACTIVE_P
#undef PORT_ACTIVE_OWNER_P
#undef PORT_CONFIG_P
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortPriorityIngressDscp2Color
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the color for all Dscp
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  color    - color of Dscp , color[MAX_PHB_VAL+1]
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortPriorityIngressDscp2Color(
    UI32_T l_port,
    UI16_T priority,
    UI8_T *color)
{
    UI32_T pre;
    UI16_T id;
    COS_TYPE_ERROR_CODE_E error_code;

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != cos_vm_port_entry[l_port-1].config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == cos_vm_port_entry[l_port-1].dscp_to_color.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }

    for(pre = 0; pre <= MAX_PHB_VAL; pre++)
    {
        *(color+pre)= cos_vm_port_entry[l_port-1].dscp_to_color.config_p[id]->table.CURRENT_DSCP_TO_COLOR_MAPPING[pre];
    }

    return COS_TYPE_E_NONE;
}

#if (__APPLE__ && __MACH__)
#pragma mark - DSCP_COS
#endif

BOOL_T COS_VM_SetIngressDscp2Cos(UI32_T lport,UI32_T phb,UI32_T color,UI32_T cos,UI32_T cfi)
{
    COS_VM_USE_CSC(FALSE);

    if (COS_TYPE_E_NONE != COS_VM_SetPortPriorityIngressDscp2Cos(lport, COS_VM_PRIORITY_USER, phb, color, cos, cfi))
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }

    COS_VM_RELEASE_CSC();
    return TRUE;
}

BOOL_T COS_VM_SetPortListIngressDscp2Cos(UI8_T *port_list,UI32_T phb,UI32_T color,UI32_T cos,UI32_T cfi)
{
    COS_VM_USE_CSC(FALSE);

    if(!SWCTRL_SetPortListQosIngressDscp2Cos(port_list,phb,color,cos,cfi))
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }


    COS_VM_RELEASE_CSC();
    return TRUE;
}
BOOL_T COS_VM_ResetIngressDscp2Cos(UI32_T lport,UI32_T phb,UI32_T color)
{
    int ret;

    COS_VM_USE_CSC(FALSE);
    ret = COS_VM_SetIngressDscp2Cos(lport,phb,color, DEFAULT_DSCP_TO_COS_MAPPING[phb*2][color].cos, DEFAULT_DSCP_TO_COS_MAPPING[phb*2][color].cfi);
    if(ret != TRUE)
    {
        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    COS_VM_RELEASE_CSC();
    return TRUE;
}

BOOL_T COS_VM_GetIngressDscp2Cos(UI32_T l_port,UI32_T phb,UI32_T color,UI32_T *cos,UI32_T *cfi)
{
    if (l_port < 1 ||
        l_port > (sizeof(cos_vm_port_entry)/sizeof(COS_VM_PortEntry_T)))
    {
        return FALSE;
    }

    COS_VM_USE_CSC(FALSE);

    *cos = cos_vm_port_entry[l_port-1].dscp_to_cos.active_p->table.CURRENT_DSCP_TO_COS_MAPPING[phb][color].cos;
    *cfi = cos_vm_port_entry[l_port-1].dscp_to_cos.active_p->table.CURRENT_DSCP_TO_COS_MAPPING[phb][color].cfi;

    COS_VM_RELEASE_CSC();
    return TRUE;
}

BOOL_T COS_VM_GetNextIngressDscp2Cos(UI32_T *l_port,UI32_T *phb,UI32_T *color,UI32_T *cos,UI32_T *cfi)
{
    UI32_T ifindex, phb1, color1;
    COS_VM_USE_CSC(FALSE);
    ifindex = *l_port;
    phb1 = *phb;
    color1 = *color;

    if(ifindex<SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER &&
       phb1 <= MAX_phb_val &&
       color1 <= MAX_color_val)
    {
        if(ifindex == (SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER-1))
        {
            if(phb1<MAX_phb_val){
                if(color1<MAX_color_val)
                    color1 ++;
                else{
                    phb1 ++;
                    color1 = 0;
                }
            }
            else{
                if(color1<MAX_color_val)
                    color1 ++;
                else{
                    COS_VM_RELEASE_CSC();
                    return FALSE;
                }
            }
        }
        else{
            if(phb1<MAX_phb_val){
                if(color1<MAX_color_val)
                    color1 ++;
                else{
                    phb1 ++;
                    color1 = 0;
                }
            }
            else{
                if(color1<MAX_color_val)
                    color1 ++;
                else{
                    ifindex ++;
                    phb1 = 0;
                    color1 = 0;
                }
            }
        }
    }
    else{
        COS_VM_RELEASE_CSC();
        return FALSE;
    }
    if (color1 == 2)
        color1 = 3;

    if(SWCTRL_PortExisting(ifindex))
    {
        *l_port = ifindex;
        *phb = phb1;
        *color = color1;

        *cos = cos_vm_port_entry[ifindex-1].dscp_to_cos.active_p->table.CURRENT_DSCP_TO_COS_MAPPING[phb1][color1].cos;
        *cfi = cos_vm_port_entry[ifindex-1].dscp_to_cos.active_p->table.CURRENT_DSCP_TO_COS_MAPPING[phb1][color1].cfi;

        COS_VM_RELEASE_CSC();
        return TRUE;
    }
    else
    {
        ifindex ++;
        while(ifindex < SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER)
        {
            if(SWCTRL_PortExisting(ifindex))
            {
              *l_port = ifindex;
              *phb = 0;
              *color = 0;

              *cos = cos_vm_port_entry[ifindex-1].config[COS_VM_DFLT_USER_PRIORITY_INDEX].dscp_to_cos.table.CURRENT_DSCP_TO_COS_MAPPING[0][0].cos;
              *cfi = cos_vm_port_entry[ifindex-1].config[COS_VM_DFLT_USER_PRIORITY_INDEX].dscp_to_cos.table.CURRENT_DSCP_TO_COS_MAPPING[0][0].cfi;

              COS_VM_RELEASE_CSC();
              return TRUE;
            }
            else
                ifindex ++;
        }

        COS_VM_RELEASE_CSC();
        return FALSE;
    }
}

BOOL_T COS_VM_GetRunningIngressDscp2Cos(UI32_T lport,UI8_T cos,UI8_T cfi,UI8_T *l_phb,UI8_T *l_color,UI8_T *cos_phb,UI8_T *cos_color)
{
    UI32_T i,j,cos1,cfi1,k = 0;

    if (lport < 1 ||
        lport > (sizeof(cos_vm_port_entry)/sizeof(COS_VM_PortEntry_T)))
    {
        return FALSE;
    }

    COS_VM_USE_CSC(FALSE);

    for(i = *l_phb; i<=MAX_PHB_VAL; i++)
    {
        for(j = *l_color; j<=MAX_COLOR_VAL; j++)
        {
            if(j == 2)
            {
                continue;
            }

            cos1 = cos_vm_port_entry[lport-1].config[COS_VM_DFLT_USER_PRIORITY_INDEX].dscp_to_cos.table.CURRENT_DSCP_TO_COS_MAPPING[i][j].cos;
            cfi1 = cos_vm_port_entry[lport-1].config[COS_VM_DFLT_USER_PRIORITY_INDEX].dscp_to_cos.table.CURRENT_DSCP_TO_COS_MAPPING[i][j].cfi;

            if((cos1 != DEFAULT_DSCP_TO_COS_MAPPING[i*2][j].cos || cfi1 != DEFAULT_DSCP_TO_COS_MAPPING[i*2][j].cfi)
                && cos1 == cos && cfi1 == cfi)
            {
                *(cos_phb+k) = i;
                *(cos_color+k) = j;
                k++;
                if (k == 8)
                {
                    if(j == MAX_COLOR_VAL)
                    {
                        if (i == MAX_PHB_VAL){
                            *l_phb = 8;
                            *l_color = 4;
                        }
                        else{
                            *l_phb = i+1;
                            *l_color = 0;
                        }
                    }
                    else{
                        *l_phb = i;
                        *l_color = j+1;
                    }

                    COS_VM_RELEASE_CSC();
                    return TRUE;
                }
            }
        }
        *l_color = 0;
    }
    *l_phb = 8;
    *l_color = 4;

    if (k != 0)
    {
        for(i = k; i<8 ; i++)
        {
            *(cos_phb+i) = 8;
            *(cos_color+i) = 4;
        }
        COS_VM_RELEASE_CSC();
        return TRUE;
    }
    else{
        COS_VM_RELEASE_CSC();
        return FALSE;
    }
}

#if (__APPLE__ && __MACH__)
#pragma mark =per-port=
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortPriorityIngressDscp2Cos
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the ingress Dscp to cos
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb      - phb value
 *          color    - color vlaue
 * OUTPUT:  cos      - cos vlaue
 *          cfi      - cfi value
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortPriorityIngressDscp2Cos(
    UI32_T l_port,
    UI16_T priority,
    UI32_T phb,
    UI32_T color,
    UI32_T cos,
    UI32_T cfi)
{
#define CONFIG              (cos_vm_port_entry[l_port-1].config[id])
#define PORT_ACTIVE_P       (cos_vm_port_entry[l_port-1].dscp_to_cos.active_p)
#define PORT_ACTIVE_OWNER_P (cos_vm_port_entry[l_port-1].dscp_to_cos.active_owner_p)
#define PORT_CONFIG_P       (cos_vm_port_entry[l_port-1].dscp_to_cos.config_p[id])

    UI32_T unit,port,trunk_id;
    UI16_T id;
    BOOL_T old_modified;
    SWCTRL_Lport_Type_T  port_type;
    COS_VM_CosPortConfig_T *old_active_owner_p;
    COS_VM_DSCP_TO_COS_CONFIG_T *old_active_p;
    COS_TYPE_ERROR_CODE_E error_code;
    BOOL_T re_value;

    if (phb >= INGRESS_DSCP_TO_COS_MAPPING_PHB ||
        color >= INGRESS_DSCP_TO_COS_MAPPING_COLOR)
    {
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != CONFIG.enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    /* not support trunk now */
    port_type = SWCTRL_LogicalPortToUserPort(l_port,&unit,&port,&trunk_id);
    if(port_type == SWCTRL_LPORT_TRUNK_PORT)
    {
        return COS_TYPE_E_TRUNCK_PORT;
    }


    old_modified = PORT_CONFIG_P->common.modified;
    if (FALSE == PORT_CONFIG_P->common.modified)
    {
        memcpy(&PORT_CONFIG_P->table,
               &PORT_ACTIVE_P->table,
               sizeof(PORT_CONFIG_P->table));
        PORT_CONFIG_P->common.modified = TRUE;
    }

    old_active_owner_p = PORT_ACTIVE_OWNER_P;
    old_active_p = PORT_ACTIVE_P;

    /* Change active */
    if (PORT_ACTIVE_OWNER_P->priority < priority)
    {
        PORT_ACTIVE_P = PORT_CONFIG_P;
        PORT_ACTIVE_OWNER_P = &CONFIG;
    }
    else
    {
        /* same with old configure
         */
         if ((cos == PORT_CONFIG_P->table.CURRENT_DSCP_TO_COS_MAPPING[phb][color].cos) &&
             (cfi == PORT_CONFIG_P->table.CURRENT_DSCP_TO_COS_MAPPING[phb][color].cfi))
         {
            return COS_TYPE_E_NONE;
         }
    }

    if (PORT_ACTIVE_OWNER_P->priority == priority)
    {
        re_value = SWCTRL_SetQosIngressDscp2Cos(l_port,phb,color,cos,cfi);

        if (FALSE == re_value)
        {
            PORT_CONFIG_P->common.modified = old_modified;
            PORT_ACTIVE_OWNER_P = old_active_owner_p;
            PORT_ACTIVE_P = old_active_p;
            return COS_TYPE_E_SET_TO_SWCTRL;;
        }
    }

    /* update local OM before announce other CSC */
    PORT_CONFIG_P->table.CURRENT_DSCP_TO_COS_MAPPING[phb][color].cos = cos;
    PORT_CONFIG_P->table.CURRENT_DSCP_TO_COS_MAPPING[phb][color].cfi = cfi;

    SYS_CALLBACK_MGR_AnnounceCosPortConfigChanged(SYS_MODULE_COS, l_port, priority);

    return COS_TYPE_E_NONE;

#undef CONFIG
#undef PORT_ACTIVE_P
#undef PORT_ACTIVE_OWNER_P
#undef PORT_CONFIG_P
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortPriorityIngressDscp2Cos
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the ingress dscp to cos
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb      - phb value
 *          color    - color vlaue
 * OUTPUT:  cos      - cos vlaue
 *          cfi      - cfi value
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortPriorityIngressDscp2Cos(
    UI32_T l_port,
    UI16_T priority,
    UI32_T phb,
    UI32_T color,
    UI32_T *cos,
    UI32_T *cfi)
{
    UI16_T id;
    COS_TYPE_ERROR_CODE_E error_code;

    if (phb >= INGRESS_DSCP_TO_COS_MAPPING_PHB ||
        color >= INGRESS_DSCP_TO_COS_MAPPING_COLOR)
    {
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != cos_vm_port_entry[l_port-1].config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == cos_vm_port_entry[l_port-1].dscp_to_cos.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }

    *cos = cos_vm_port_entry[l_port-1].dscp_to_cos.config_p[id]->table.CURRENT_DSCP_TO_COS_MAPPING[phb][color].cos;
    *cfi = cos_vm_port_entry[l_port-1].dscp_to_cos.config_p[id]->table.CURRENT_DSCP_TO_COS_MAPPING[phb][color].cfi;

    return COS_TYPE_E_NONE;
}

#if (__APPLE__ && __MACH__)
#pragma mark - Queue mode
#pragma mark =Global=
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the queue mode by global
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          mode     - queue mode, COS_VM_SCHEDULING_METHOD_T
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Change mode will set all queue to be wrr queue,
 *          and set the wrr weight as old config
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetEgressQueueMode(
    UI16_T priority,
    UI32_T mode)
{
#if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else
    return COS_VM_Priv_SetEgressQueueMode(COS_VM_LPORT_OF_GLOBAL, priority, mode);
#endif /* #if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue mode by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  mode     - queue mode, COS_VM_SCHEDULING_METHOD_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetEgressQueueMode(
    UI16_T priority,
    UI32_T *mode_p)
{
#if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else
    UI16_T id;
    COS_TYPE_ERROR_CODE_E error_code;

    error_code = COS_VM_PortPriorityToId(COS_VM_LPORT_OF_GLOBAL, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_mode.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }

    *mode_p = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_mode.config_p[id]->mode;

    return COS_TYPE_E_NONE;
#endif /* #if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetActiveEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue mode
 * INPUT:
 * OUTPUT: priority_p - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *         mode_p     - queue mode, COS_VM_SCHEDULING_METHOD_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetActiveEgressQueueMode(
    UI16_T *priority_p,
    UI32_T *mode_p)
{
#if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else
    ASSERT(priority_p != NULL);
    ASSERT(mode_p != NULL);

    *priority_p = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_mode.active_owner_p->priority;
    *mode_p = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_mode.active_p->mode;

    return COS_TYPE_E_NONE;
#endif /* #if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE) */
}

#if (__APPLE__ && __MACH__)
#pragma mark =per-port=
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the queue mode by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          mode     - queue mode, COS_VM_SCHEDULING_METHOD_T
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Change mode will set all queue to be wrr queue,
 *          and set the wrr weight as old config
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortEgressQueueMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T mode)
{
#if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE)
    return COS_VM_Priv_SetEgressQueueMode(l_port, priority, mode);
#else
    return COS_VM_SetEgressQueueMode(priority, mode);
#endif /* #if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue mode by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  mode     - queue mode, COS_VM_SCHEDULING_METHOD_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortEgressQueueMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T *mode_p)
{
#if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE)
    UI16_T id;
    COS_TYPE_ERROR_CODE_E error_code;
    SWCTRL_Lport_Type_T   port_type;
    UI32_T                unit, port, trunk_id;

    error_code = COS_VM_Priv_VerifyIfindex(l_port);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != cos_vm_port_entry[l_port-1].config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == cos_vm_port_entry[l_port-1].queue_mode.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }

    /* not support trunk now */
    port_type = SWCTRL_LogicalPortToUserPort(l_port, &unit, &port, &trunk_id);
    if(port_type == SWCTRL_LPORT_TRUNK_PORT)
    {
        return COS_TYPE_E_TRUNCK_PORT;
    }

    *mode_p = cos_vm_port_entry[l_port-1].queue_mode.config_p[id]->mode;

    return COS_TYPE_E_NONE;
#else
    return COS_VM_GetEgressQueueMode(priority, mode_p);
#endif /* #if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextPortEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next queue mode by port
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  l_port_p - Logical port id
 *          mode     - queue mode, COS_VM_SCHEDULING_METHOD_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextPortEgressQueueMode(
    UI16_T priority,
    UI32_T *l_port_p,
    UI32_T *mode_p)
{
#if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE)
    if (l_port_p == NULL || mode_p == NULL)
    {
        return COS_TYPE_E_PARAMETER;
    }

    if (0 == *l_port_p)
    {
        *l_port_p = 1;
    }
    else
    {
        (*l_port_p)++;
    }

    for (; *l_port_p <= SWITCH_PORT_NUM; ++(*l_port_p))
    {
        if (COS_TYPE_E_NONE == COS_VM_GetPortEgressQueueMode(*l_port_p, priority, mode_p))
        {
            return COS_TYPE_E_NONE;
        }
    }

#endif /* #if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE) */

    return COS_TYPE_E_UNKNOWN;

}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortActiveEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue mode by port
 * INPUT:   l_port   - logic port
 *
 * OUTPUT:  priority_p - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          mode_p     - queue mode, COS_VM_SCHEDULING_METHOD_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Gets the active config
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortActiveEgressQueueMode(
    UI32_T l_port,
    UI16_T *priority_p,
    UI32_T *mode_p)
{
#if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE)
    if (COS_TYPE_E_NONE != COS_VM_Priv_VerifyIfindex(l_port))
    {
        return COS_TYPE_E_PARAMETER;
    }

    *priority_p = cos_vm_port_entry[l_port-1].queue_mode.active_owner_p->priority;
    *mode_p = cos_vm_port_entry[l_port-1].queue_mode.active_p->mode;

    return COS_TYPE_E_NONE;
#else
    return COS_TYPE_E_UNKNOWN;
#endif /* #if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_ResetPortEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function resets the queue mode to default by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_ResetPortEgressQueueMode(
    UI32_T l_port,
    UI16_T priority)
{
#if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE)
    return COS_VM_SetPortEgressQueueMode(l_port, priority, DEFAULT_QUEUE_MODE);
#else
    return COS_VM_SetEgressQueueMode(priority, DEFAULT_QUEUE_MODE);
#endif /* #if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE) */
}

#if (__APPLE__ && __MACH__)
#pragma mark - Queue weight
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the wrr queue weight by port
 * INPUT:   l_port   - logic port
 *          q_id     - queue id
 * OUTPUT:  weight   - queue weight
 * RETURN:  TRUE/FALSE
 * NOTE:    This API will show the strict queue weight as 0.
 *          Remove this function in future,
 *          replace by COS_VM_GetPortEgressQueueWeight
 *                     COS_VM_GetPortEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 */
BOOL_T
COS_VM_GetPortEgressWrrQueueWeight(
    UI32_T l_port,
    UI32_T q_id,
    UI32_T *weight)
{
    COS_VM_QUEUE_STRICT_STATUS_T strict_status;

    if (COS_TYPE_E_NONE != COS_VM_GetPortEgressQueueStrictStatus(l_port,
                               COS_VM_PRIORITY_USER, q_id, &strict_status))
    {
        return FALSE;
    }

    if (COS_VM_QUEUE_STRICT_STATUS_ENABLED == strict_status)
    {
        *weight = 0;
        return TRUE;
    }

    if (COS_TYPE_E_NONE != COS_VM_GetPortEgressQueueWeight(l_port,
                               COS_VM_PRIORITY_USER, q_id, weight))
    {
        return FALSE;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next wrr queue weight
 * INPUT:   queue_p     -
 *
 * OUTPUT:  queue_p     -
 *          weight_p    - queue weight
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Input queue (COS_TYPE_UNKNOWN_QUEUE) to get first queue weight,
 *
 *          In hybrid mode, this function still gets the configured wrr weight
 *          for strict-queue
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextEgressWrrQueueWeight(
    UI32_T *queue_p,
    UI32_T *weight_p)
{
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else

    COS_TYPE_ERROR_CODE_E  result;

    ASSERT(queue_p != NULL);
    ASSERT(weight_p != NULL);

    *queue_p = (*queue_p == COS_TYPE_UNKNOWN_QUEUE) ? 0 : ((*queue_p) + 1);

    if (MAX_QUEUE_VAL < *queue_p)
    {
        return COS_TYPE_E_PARAMETER;
    }

    result = COS_VM_GetEgressQueueWeight(
                 COS_TYPE_PRIORITY_USER, *queue_p, weight_p);

    return result;

#endif /* SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextPortEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next wrr queue weight and queue status(strict/wrr)
 *          by port
 * INPUT:   ifindex_p   -
 *          queue_p     -
 *
 * OUTPUT:  ifindex_p   -
 *          queue_p     -
 *          weight      - queue weight
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Input ifindex, queue (0, COS_TYPE_UNKNOWN_QUEUE)
 *          to get first queue weight.
 *
 *          In hybrid mode, this function still gets the configured wrr weight
 *          for strict-queue
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextPortEgressWrrQueueWeight(
    UI32_T *ifindex_p,
    UI32_T *queue_p,
    UI32_T *weight_p)
{
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    COS_TYPE_ERROR_CODE_E  result;

    ASSERT(ifindex_p != NULL);
    ASSERT(queue_p != NULL);
    ASSERT(weight_p != NULL);

    if (SWITCH_PORT_NUM < *ifindex_p)
    {
        return COS_TYPE_E_PARAMETER;
    }

    *queue_p = (*queue_p == COS_TYPE_UNKNOWN_QUEUE) ? 0 : ((*queue_p) + 1);

    if (TRUE != SWCTRL_LogicalPortExisting(*ifindex_p) || MAX_QUEUE_VAL < *queue_p)
    {
        *queue_p = MIN_QUEUE_VAL;

        if (SWCTRL_GetNextLogicalPort(ifindex_p) != SWCTRL_LPORT_NORMAL_PORT)
        {
            return COS_TYPE_E_PARAMETER;
        }
    }

    result = COS_VM_GetPortEgressQueueWeight(
                 *ifindex_p, COS_TYPE_PRIORITY_USER, *queue_p, weight_p);
    return result;

#else
    return COS_TYPE_E_UNKNOWN;
#endif /* SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetRunningEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the config of wrr queue weight
 * INPUT:   queue     - queue id
 * OUTPUT:  weight_p    - queue weight
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetRunningEgressQueueWeight(
    UI32_T queue,
    UI32_T *weight_p)
{
    COS_TYPE_ERROR_CODE_E result;
    UI32_T mode;
    UI16_T priority = COS_VM_PRIORITY_USER;

    ASSERT(weight_p != NULL);

    result = COS_VM_GetEgressQueueMode(priority, &mode);
    if (result != COS_TYPE_E_NONE )
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    if (mode == COS_VM_SCHEDULING_STRICT_PRIORITY)
    {
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }

    result = COS_VM_GetEgressQueueWeight(
                 priority, queue, weight_p);
    if (result != COS_TYPE_E_NONE)
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    if (*weight_p == DEFAULT_WRR_QUEUE_TO_WEIGHT[queue])
    {
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }

    return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetRunningPortEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the config of wrr queue weight by port
 * INPUT:   ifindex   - logic port
 *          queue     - queue id
 * OUTPUT:  weight_p    - queue weight
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetRunningPortEgressQueueWeight(
    UI32_T ifindex,
    UI32_T queue,
    UI32_T *weight_p)
{
    COS_TYPE_ERROR_CODE_E result;
    UI32_T mode;
    UI16_T priority = COS_VM_PRIORITY_USER;

    ASSERT(weight_p != NULL);

    result = COS_VM_GetPortEgressQueueMode(ifindex, priority, &mode);
    if (result != COS_TYPE_E_NONE )
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    if (mode == COS_VM_SCHEDULING_STRICT_PRIORITY)
    {
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }

    result = COS_VM_GetPortEgressQueueWeight(
                 ifindex, priority, queue, weight_p);
    if (result != COS_TYPE_E_NONE)
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    if (*weight_p == DEFAULT_WRR_QUEUE_TO_WEIGHT[queue])
    {
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }

    return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
}

#if (__APPLE__ && __MACH__)
#pragma mark =Global=
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the wrr queue weight by global
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          q_id     - queue id
 *          weight   - queue weight
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetEgressQueueWeight(
    UI16_T priority,
    UI32_T q_id,
    UI32_T weight)
{
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else
    return COS_VM_Priv_SetEgressQueueWeight(COS_VM_LPORT_OF_GLOBAL, priority, q_id, weight);
#endif /* #if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the wrr queue weight and queue status(strict/wrr)
 * INPUT:   priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue     - queue
 * OUTPUT:  weight_p     - queue weight
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetEgressQueueWeight(
    UI16_T priority,
    UI32_T queue,
    UI32_T *weight_p)
{
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else
    UI16_T id;
    COS_TYPE_ERROR_CODE_E result;

    COS_VM_DEBUG_PRINTF("Input priority %d, queue %lu", priority, queue);

    if (queue < MIN_QUEUE_VAL || MAX_QUEUE_VAL < queue)
    {
        return COS_TYPE_E_PARAMETER;
    }

    result = COS_VM_PortPriorityToId(COS_VM_LPORT_OF_GLOBAL, priority, &id);
    if (COS_TYPE_E_NONE != result)
    {
        return result;
    }

    if (TRUE != cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_weight.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }

    *weight_p = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_weight.config_p[id]->wrr_weight[queue];

    return COS_TYPE_E_NONE;
#endif /* #if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next the queue weight
 * INPUT:   priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue_p   - queue id
 *
 * OUTPUT:  queue_p   - queue id
 *          weight_p     - queue weight
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextEgressQueueWeight(
    UI16_T priority,
    UI32_T *queue_p,
    UI32_T *weight_p)
{
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else
    COS_TYPE_ERROR_CODE_E result;

    COS_VM_DEBUG_PRINTF("Input priority %d, queue %lu", priority, *queue_p);

    *queue_p = (COS_TYPE_UNKNOWN_QUEUE == *queue_p) ? MIN_QUEUE_VAL : (*queue_p + 1);

    for (; *queue_p <= MAX_QUEUE_VAL; ++(*queue_p))
    {
        result = COS_VM_GetEgressQueueWeight(priority, *queue_p, weight_p);
        if (result == COS_TYPE_E_NONE)
        {
            return COS_TYPE_E_NONE;
        }
    }

    return COS_TYPE_E_UNKNOWN;
#endif /* #if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetActiveQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the actived queue weight
 * INPUT:  queue     - queue
 *
 * OUTPUT: priority_p - the actived priority of CoS config
 *         weight_p   - queue weight
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetActiveQueueWeight(
    UI32_T queue,
    UI16_T *priority_p,
    UI32_T *weight_p)
{
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else
    if (queue < MIN_QUEUE_VAL || MAX_QUEUE_VAL < queue)
    {
        return COS_TYPE_E_PARAMETER;
    }

    *priority_p = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_weight.active_owner_p->priority;
    *weight_p = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_weight.active_p->wrr_weight[queue];

    return COS_TYPE_E_NONE;
#endif /* #if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function only used for hybrid queue mode (ex: strict+wrr)
 *          to set the queue to be strict queue or wrr queue
 * INPUT:   priority      - The priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue         - Queue id
 *          status        -
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetEgressQueueStrictStatus(
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T status)
{
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else

    UI32_T mode, weight;
    UI16_T id;
    COS_TYPE_ERROR_CODE_E result;

    COS_VM_DEBUG_PRINTF("Input queue: %lu status: %d",
                        queue, status);

    if (queue < MIN_QUEUE_VAL || MAX_QUEUE_VAL < queue)
    {
        return COS_TYPE_E_PARAMETER;
    }

    result = COS_VM_PortPriorityToId(COS_VM_LPORT_OF_GLOBAL, priority, &id);
    if (COS_TYPE_E_NONE != result)
    {
        return result;
    }

    mode = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_mode.config_p[id]->mode;
    if (mode != COS_VM_SCHEDULING_METHOD_SP_WRR &&
        mode != COS_VM_SCHEDULING_METHOD_SP_DRR)
    {
        return COS_TYPE_E_SCHEDULING_METHOD;
    }

    if (status == COS_VM_QUEUE_STRICT_STATUS_ENABLED)
    {
        weight = 0;
    }
    else
    {
        weight = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_weight.config_p[id]->wrr_weight[queue];
        cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_weight.config_p[id]->is_strict[queue] = FALSE;
    }

    return COS_VM_Priv_SetEgressQueueWeight(COS_VM_LPORT_OF_GLOBAL,
                                            priority,
                                            queue,
                                            weight);

#endif /* #if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue status(strict/wrr)
 * INPUT:   priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue     - queue
 * OUTPUT:  status_p  -
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetEgressQueueStrictStatus(
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p)
{
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else
    UI16_T id;
    COS_TYPE_ERROR_CODE_E result;

    COS_VM_DEBUG_PRINTF("Input priority %d, queue %lu", priority, queue);

    if (queue < MIN_QUEUE_VAL || MAX_QUEUE_VAL < queue)
    {
        return COS_TYPE_E_PARAMETER;
    }

    result = COS_VM_PortPriorityToId(COS_VM_LPORT_OF_GLOBAL, priority, &id);
    if (COS_TYPE_E_NONE != result)
    {
        return result;
    }

    if (TRUE != cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_weight.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }

    if (COS_VM_SCHEDULING_STRICT_PRIORITY == cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_mode.config_p[id]->mode)
    {
        *status_p = COS_VM_QUEUE_STRICT_STATUS_ENABLED;
    }
    else
    {
        BOOL_T is_strict = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_weight.config_p[id]->is_strict[queue];

        *status_p = (is_strict == TRUE)
                    ? COS_VM_QUEUE_STRICT_STATUS_ENABLED
                    : COS_VM_QUEUE_STRICT_STATUS_DISABLED;
    }

    return COS_TYPE_E_NONE;
#endif /* #if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetActiveEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the actived queue status(strict/wrr)
 * INPUT:   queue     - Queue id
 *
 * OUTPUT:  priority_p - The actived priority of COS config.
 *          status_p   - Strict/RR, COS_VM_QUEUE_STRICT_STATUS_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetActiveEgressQueueStrictStatus(
    UI32_T queue,
    UI16_T *priority_p,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p)
{
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    return COS_TYPE_E_UNKNOWN;
#else
    COS_VM_DEBUG_PRINTF("Input queue %lu", queue);

    if (queue < MIN_QUEUE_VAL || MAX_QUEUE_VAL < queue)
    {
        return COS_TYPE_E_PARAMETER;
    }

    *priority_p = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_weight.active_owner_p->priority;

    if (COS_VM_SCHEDULING_STRICT_PRIORITY == cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_mode.active_p->mode)
    {
        *status_p = COS_VM_QUEUE_STRICT_STATUS_ENABLED;
    }
    else
    {
        BOOL_T is_strict = cos_vm_port_entry[COS_VM_LPORT_OF_GLOBAL-1].queue_weight.active_p->is_strict[queue];

        *status_p = (is_strict == TRUE)
                    ? COS_VM_QUEUE_STRICT_STATUS_ENABLED
                    : COS_VM_QUEUE_STRICT_STATUS_DISABLED;
    }
    
    return COS_TYPE_E_NONE;
#endif /* #if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE) */
}

#if (__APPLE__ && __MACH__)
#pragma mark =per-port=
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the wrr queue weight by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          q_id     - queue id
 *          weight   - queue weight
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortEgressQueueWeight(
    UI32_T l_port,
    UI16_T priority,
    UI32_T q_id,
    UI32_T weight)
{
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    return COS_VM_Priv_SetEgressQueueWeight(l_port, priority, q_id, weight);
#else
    return COS_VM_SetEgressQueueWeight(priority, q_id, weight);
#endif /* #if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the wrr queue weight and queue status(strict/wrr)
 *          by port
 * INPUT:   ifindex   - logic port
 *          priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue     - queue id
 * OUTPUT:  weight_p     - queue weight
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortEgressQueueWeight(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T queue,
    UI32_T *weight_p)
{
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    UI16_T id;
    COS_TYPE_ERROR_CODE_E error_code;
    SWCTRL_Lport_Type_T   port_type;
    UI32_T                unit, port, trunk_id;

    if (queue < MIN_QUEUE_VAL || MAX_QUEUE_VAL < queue)
    {
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(ifindex, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != cos_vm_port_entry[ifindex-1].config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    port_type = SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);
    if(port_type == SWCTRL_LPORT_TRUNK_PORT)
    {
        return COS_TYPE_E_TRUNCK_PORT;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == cos_vm_port_entry[ifindex-1].queue_weight.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }

    *weight_p = cos_vm_port_entry[ifindex-1].queue_weight.config_p[id]->wrr_weight[queue];

    return COS_TYPE_E_NONE;
#else
    return COS_VM_GetEgressQueueWeight(priority, queue, weight_p);
#endif /* #if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextPortEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next wrr queue weight.
 * INPUT:   priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *
 * OUTPUT:  l_port_p  - logic port
 *          queue     - queue id
 *          weight_p  - queue weight
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Using l_port = 0 to get first.
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextPortEgressQueueWeight(
    UI16_T priority,
    UI32_T *l_port_p,
    UI32_T *queue_p,
    UI32_T *weight_p)
{
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    if (*l_port_p == 0)
    {
        *l_port_p = 1;
        *queue_p = MIN_QUEUE_VAL;
    }
    else if (*queue_p == COS_TYPE_UNKNOWN_QUEUE)
    {
        *queue_p = MIN_QUEUE_VAL;
    }
    else
    {
        *queue_p = (*queue_p) + 1;
    }

    for (; *l_port_p <= SWITCH_PORT_NUM; ++(*l_port_p))
    {
        if (COS_TYPE_E_NONE != COS_VM_Priv_VerifyIfindex(*l_port_p))
        {
            continue;
        }

        for (; *queue_p <= MAX_QUEUE_VAL; ++(*queue_p))
        {
            if (COS_TYPE_E_NONE == COS_VM_GetPortEgressQueueWeight(*l_port_p,
                    priority, *queue_p, weight_p))
            {
                return COS_TYPE_E_NONE;
            }

            COS_VM_DEBUG_PRINTF("Fail to get internal dscp with ifindex = %lu, priority = %u, queue = %lu",
                                *l_port_p, priority, *queue_p);
        }

        *queue_p = MIN_QUEUE_VAL;
    }

#endif /* #if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE) */

    return COS_TYPE_E_UNKNOWN;

}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortActiveQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the avtived queue weight
 * INPUT:   ifindex   - Logic port
 *          queue     - Queue id
 *
 * OUTPUT:  priority  - The actived priority of CoS config
 *          weight_p  - Queue weight
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortActiveQueueWeight(
    UI32_T ifindex,
    UI32_T queue,
    UI16_T *priority_p,
    UI32_T *weight_p)
{
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    COS_TYPE_ERROR_CODE_E result;


    result = COS_VM_Priv_VerifyIfindex(ifindex);
    if (result != COS_TYPE_E_NONE)
    {
        COS_VM_DEBUG_PRINTF("Error parameter with ifindex:%lu", ifindex);
        return result;
    }

    if (queue < MIN_QUEUE_VAL || MAX_QUEUE_VAL < queue)
    {
        COS_VM_DEBUG_PRINTF("Error parameter with queue:%lu", queue);
        return COS_TYPE_E_PARAMETER;
    }

    *priority_p = cos_vm_port_entry[ifindex-1].queue_weight.active_owner_p->priority;
    *weight_p = cos_vm_port_entry[ifindex-1].queue_weight.active_p->wrr_weight[queue];

    return COS_TYPE_E_NONE;
#else
    return COS_TYPE_E_UNKNOWN;
#endif /* #if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_ResetPortEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function resets the wrr queue weight to default by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          q_id     - queue id
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_ResetPortEgressQueueWeight(
    UI32_T l_port,
    UI16_T priority,
    UI32_T q_id)
{
    if (q_id >= SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE)
    {
        return COS_TYPE_E_PARAMETER;
    }

#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    return COS_VM_SetPortEgressQueueWeight(
               l_port, priority, q_id, DEFAULT_WRR_QUEUE_TO_WEIGHT[q_id]);
#else
    return COS_VM_SetEgressQueueWeight(
               priority, q_id, DEFAULT_WRR_QUEUE_TO_WEIGHT[q_id]);
#endif /* #if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function only used for hybrid queue mode (ex: strict+wrr)
 *          to set the queue to be strict queue or wrr queue
 * INPUT:   ifindex
 *          priority      - The priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue         - Queue id
 *          status        -
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortEgressQueueStrictStatus(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T status)
{
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    UI32_T mode, weight;
    UI16_T id;
    COS_TYPE_ERROR_CODE_E result;

    COS_VM_DEBUG_PRINTF("Input ifindex: %lu queue: %lu status: %d",
                        ifindex, queue, status);

    if (queue < MIN_QUEUE_VAL || MAX_QUEUE_VAL < queue)
    {
        return COS_TYPE_E_PARAMETER;
    }

    result = COS_VM_PortPriorityToId(ifindex, priority, &id);
    if (COS_TYPE_E_NONE != result)
    {
        return result;
    }

    mode = cos_vm_port_entry[ifindex-1].queue_mode.config_p[id]->mode;
    if (mode != COS_VM_SCHEDULING_METHOD_SP_WRR &&
        mode != COS_VM_SCHEDULING_METHOD_SP_DRR)
    {
        return COS_TYPE_E_SCHEDULING_METHOD;
    }

    if (status == COS_VM_QUEUE_STRICT_STATUS_ENABLED)
    {
        weight = 0;
    }
    else
    {
        weight = cos_vm_port_entry[ifindex-1].queue_weight.config_p[id]->wrr_weight[queue];
        cos_vm_port_entry[ifindex-1].queue_weight.config_p[id]->is_strict[queue] = FALSE;
    }

    return COS_VM_Priv_SetEgressQueueWeight(ifindex,
                                            priority,
                                            queue,
                                            weight);

#else
    return COS_TYPE_E_UNKNOWN;
#endif /* #if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue status(strict/wrr)
 *          by port
 * INPUT:   ifindex   - logic port
 *          priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue     - queue id
 * OUTPUT:  status_p  -
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortEgressQueueStrictStatus(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p)
{
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    UI16_T id;
    COS_TYPE_ERROR_CODE_E error_code;
    SWCTRL_Lport_Type_T   port_type;
    UI32_T                unit, port, trunk_id;

    if (queue < MIN_QUEUE_VAL || MAX_QUEUE_VAL < queue)
    {
        return COS_TYPE_E_PARAMETER;
    }

    error_code = COS_VM_PortPriorityToId(ifindex, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (TRUE != cos_vm_port_entry[ifindex-1].config[id].enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    port_type = SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);
    if(port_type == SWCTRL_LPORT_TRUNK_PORT)
    {
        return COS_TYPE_E_TRUNCK_PORT;
    }

    if (COS_VM_PRIORITY_USER != priority &&
        FALSE == cos_vm_port_entry[ifindex-1].queue_weight.config_p[id]->common.modified)
    {
        return COS_TYPE_E_PRIORITY_NOT_MODIFIED;
    }

    if (COS_VM_SCHEDULING_STRICT_PRIORITY == cos_vm_port_entry[ifindex-1].queue_mode.config_p[id]->mode)
    {
        *status_p = COS_VM_QUEUE_STRICT_STATUS_ENABLED;
    }
    else
    {
        BOOL_T is_strict = cos_vm_port_entry[ifindex-1].queue_weight.config_p[id]->is_strict[queue];

        *status_p = (is_strict == TRUE)
                           ? COS_VM_QUEUE_STRICT_STATUS_ENABLED
                           : COS_VM_QUEUE_STRICT_STATUS_DISABLED;
    }

    return COS_TYPE_E_NONE;
#else
    return COS_VM_GetEgressQueueStrictStatus(priority, queue, status_p);
#endif /* #if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortActiveEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the actived queue status(strict/wrr)
 *          by port
 * INPUT:   ifindex   - Logic port
 *          queue     - Queue id
 *
 * OUTPUT:  priority_p - The actived priority of COS config.
 *          status_p   - Strict/RR, COS_VM_QUEUE_STRICT_STATUS_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortActiveEgressQueueStrictStatus(
    UI32_T ifindex,
    UI32_T queue,
    UI16_T *priority_p,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p)
{
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
    COS_TYPE_ERROR_CODE_E result;

    result = COS_VM_Priv_VerifyIfindex(ifindex);
    if (result != COS_TYPE_E_NONE)
    {
        return result;
    }

    if (queue < MIN_QUEUE_VAL || MAX_QUEUE_VAL < queue)
    {
        return COS_TYPE_E_PARAMETER;
    }

    *priority_p = cos_vm_port_entry[ifindex-1].queue_weight.active_owner_p->priority;

    if (COS_VM_SCHEDULING_STRICT_PRIORITY == cos_vm_port_entry[ifindex-1].queue_mode.active_p->mode)
    {
        *status_p = COS_VM_QUEUE_STRICT_STATUS_ENABLED;
    }
    else
    {
        BOOL_T is_strict = cos_vm_port_entry[ifindex-1].queue_weight.active_p->is_strict[queue];

        *status_p = (is_strict == TRUE)
                    ? COS_VM_QUEUE_STRICT_STATUS_ENABLED
                    : COS_VM_QUEUE_STRICT_STATUS_DISABLED;
    }

    return COS_TYPE_E_NONE;
#else
    return COS_TYPE_E_UNKNOWN;
#endif /* #if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE) */
}

#if (__APPLE__ && __MACH__)
#pragma mark - Private function body
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_Priv_VerifyIfindex
 *------------------------------------------------------------------------------
 * PURPOSE: To verify the port type of ifindex is legal.
 * INPUT:   ifindex   - logic port
 * OUTPUT:
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Check the ifindex 
 *            1. <= SYS_ADPT_TOTAL_NBR_OF_LPORT
 *            2. Is normal port | trunk member port
 *------------------------------------------------------------------------------
 */
static COS_TYPE_ERROR_CODE_E
COS_VM_Priv_VerifyIfindex(
    UI32_T ifindex)
{
    UI32_T unit, port, trunk_id;
    SWCTRL_Lport_Type_T  port_type;

    if (ifindex < 1 ||
        SWITCH_PORT_NUM < ifindex )
    {
        return COS_TYPE_E_PARAMETER;
    }

    port_type = SWCTRL_LogicalPortToUserPort(ifindex, &unit,&port, &trunk_id);
    if (port_type == SWCTRL_LPORT_TRUNK_PORT)
    {
        return COS_TYPE_E_TRUNCK_PORT;
    }

    if(port_type == SWCTRL_LPORT_UNKNOWN_PORT)
    {
        return COS_TYPE_E_PARAMETER;
    }

    return COS_TYPE_E_NONE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_Priv_LocalSetEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: Set egress queue mode to local om and lower layer
 * INPUT:   ifindex   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          mode     - queue sceduling mode (wrr/strict/...)
 * OUTPUT:
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
static COS_TYPE_ERROR_CODE_E
COS_VM_Priv_LocalSetEgressQueueMode(
    UI32_T ifindex,
    UI16_T priority,
    COS_VM_SCHEDULING_METHOD_T mode)
{
    BOOL_T                       b_result;
    BOOL_T                       old_modified;
    UI16_T                       id;
    COS_TYPE_ERROR_CODE_E        result;

    COS_VM_CosPortConfig_T       *old_active_owner_p;
    COS_VM_QUEUE_MODE_CONFIG_T   *old_active_p;
    COS_VM_QUEUE_WEIGHT_CONFIG_T *old_queue_weight_active_p;

    COS_VM_PortEntry_T           *port_entry_p;
    COS_VM_CosPortConfig_T       *port_config_p;
    COS_VM_QUEUE_MODE_T          *config_table_p;
    COS_VM_QUEUE_MODE_CONFIG_T   *modified_config_p;

    port_entry_p = &cos_vm_port_entry[ifindex - 1];

    result = COS_VM_PortPriorityToId(ifindex, priority, &id);
    if (result != COS_TYPE_E_NONE)
    {
        return result;
    }

    port_config_p = &port_entry_p->config[id];

    config_table_p = &port_entry_p->queue_mode;

    modified_config_p = config_table_p->config_p[id];

    if (TRUE != port_config_p->enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    old_modified = modified_config_p->common.modified;
    if (FALSE == modified_config_p->common.modified)
    {
        modified_config_p->mode = config_table_p->active_p->mode;
        modified_config_p->common.modified = TRUE;

        /* Queue mode and queue weight should work in same priority
         * sync current running status to this priority's config
         */
        if (port_entry_p->queue_weight.active_owner_p->priority != priority)
        {
            memcpy(port_entry_p->queue_weight.config_p[id],
                   port_entry_p->queue_weight.active_p,
                   sizeof(*(port_entry_p->queue_weight.config_p[id])));

            port_entry_p->queue_weight.config_p[id]->common.modified = TRUE;
        }
    }
    else
    {
        /* same with old configure,
         */
        if (mode == modified_config_p->mode)
        {
            return COS_TYPE_E_NO_CHANGE;
        }
    }

    old_active_owner_p = config_table_p->active_owner_p;
    old_active_p = config_table_p->active_p;
    old_queue_weight_active_p = port_entry_p->queue_weight.active_p;

    /* Change active */
    if (config_table_p->active_owner_p->priority < priority)
    {
        config_table_p->active_p = modified_config_p;
        config_table_p->active_owner_p = port_config_p;

        port_entry_p->queue_weight.active_p = port_entry_p->queue_weight.config_p[id];
        port_entry_p->queue_weight.active_owner_p = port_config_p;

    }

    if (config_table_p->active_owner_p->priority == priority)
    {
#if (SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL == TRUE)
        b_result = SWCTRL_SetPortPrioQueueMode(ifindex, mode);
#else
        b_result = SWCTRL_SetPrioQueueMode(mode);
#endif /* SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL */

        if (FALSE == b_result)
        {
            modified_config_p->common.modified = old_modified;
            config_table_p->active_owner_p = old_active_owner_p;
            config_table_p->active_p = old_active_p;

            return COS_TYPE_E_SET_TO_SWCTRL;;
        }
    }

    /* update local OM before announce other CSC
     */
    modified_config_p->mode = mode;

    return COS_TYPE_E_NONE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_Priv_LocalSetEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: Set egress queue weight to local om and lower layer
 * INPUT:   ifindex  - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          weight   - queue weight
 * OUTPUT:
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
static COS_TYPE_ERROR_CODE_E
COS_VM_Priv_LocalSetEgressQueueWeight(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T q_id,
    UI32_T weight)
{
    UI16_T                       id;
    BOOL_T                       b_result;
    COS_TYPE_ERROR_CODE_E        result;

    BOOL_T                       old_modified;
    COS_VM_CosPortConfig_T       *old_active_owner_p;
    COS_VM_QUEUE_WEIGHT_CONFIG_T *old_active_p;
    COS_VM_QUEUE_MODE_CONFIG_T   *old_queue_mode_active_p;

    COS_VM_PortEntry_T           *port_entry_p;
    COS_VM_CosPortConfig_T       *port_config_p;
    COS_VM_QUEUE_WEIGHT_T        *config_table_p;
    COS_VM_QUEUE_WEIGHT_CONFIG_T *modified_config_p;

    COS_VM_DEBUG_PRINTF("Input ifindex: %lu, priority: %u, queue: %lu, weight: %lu",
                        ifindex, priority, q_id, weight);

    port_entry_p = &cos_vm_port_entry[ifindex - 1];

    result = COS_VM_PortPriorityToId(ifindex, priority, &id);
    if (COS_TYPE_E_NONE != result)
    {
        return result;
    }

    port_config_p = &port_entry_p->config[id];

    config_table_p = &port_entry_p->queue_weight;

    modified_config_p = config_table_p->config_p[id];

    if (TRUE != port_config_p->enable)
    {
        return COS_TYPE_E_PRIORITY_NOT_ENABLED;
    }

    if (TRUE == port_config_p->queue_mode.common.modified &&
        COS_VM_SCHEDULING_STRICT_PRIORITY == port_config_p->queue_mode.mode)
    {
        COS_VM_DEBUG_ERROR_MSG(COS_TYPE_E_SCHEDULING_METHOD);
        return COS_TYPE_E_SCHEDULING_METHOD;
    }

    old_modified = modified_config_p->common.modified;
    if (FALSE == modified_config_p->common.modified)
    {
        /* Queue mode and queue weight should work in same priority
         * sync current running status to this priority's config
         */
        if (port_entry_p->queue_mode.active_owner_p->priority != priority)
        {
            memcpy(port_entry_p->queue_mode.config_p[id],
                   port_entry_p->queue_mode.active_p,
                   sizeof(*(port_entry_p->queue_mode.config_p[id])));

            port_entry_p->queue_mode.config_p[id]->common.modified = TRUE;
        }

        memcpy(modified_config_p->wrr_weight,
               config_table_p->active_p->wrr_weight,
               sizeof(modified_config_p->wrr_weight));

        modified_config_p->common.modified = TRUE;

    }

    old_active_owner_p = config_table_p->active_owner_p;
    old_active_p = config_table_p->active_p;
    old_queue_mode_active_p = port_entry_p->queue_mode.active_p;

    /* Change active */
    if (config_table_p->active_owner_p->priority < priority)
    {
        config_table_p->active_p = modified_config_p;
        config_table_p->active_owner_p = port_config_p;

        port_entry_p->queue_mode.active_owner_p = port_config_p;
        port_entry_p->queue_mode.active_p = port_entry_p->queue_mode.config_p[id];
    }

    if (config_table_p->active_owner_p->priority == priority &&
        modified_config_p->is_strict[q_id] != TRUE)
    {
#if (SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL == TRUE)
        b_result = SWCTRL_SetPortWrrQueueWeight(ifindex, q_id, weight);
#else
        b_result = SWCTRL_SetWrrQueueWeight(q_id, weight);
#endif /* SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTR */
        if (FALSE == b_result)
        {
            modified_config_p->common.modified = old_modified;
            config_table_p->active_owner_p = old_active_owner_p;
            config_table_p->active_p = old_active_p;

            port_entry_p->queue_mode.active_owner_p = old_active_owner_p;
            port_entry_p->queue_mode.active_p = old_queue_mode_active_p;

            return COS_TYPE_E_SET_TO_SWCTRL;;
        }
    }

    /* update local OM before announce other CSC */
    if (0 == weight)
    {
        modified_config_p->is_strict[q_id] = TRUE;
    }
    else
    {
        modified_config_p->wrr_weight[q_id] = weight;
    }

    return COS_TYPE_E_NONE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_Priv_SetEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: Contorl anything need to do when change queue scheduling mode
 * INPUT:   ifindex  - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          mode     - queue sceduling mode (wrr/strict/...)
 * OUTPUT:
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    If change queue mode to hybrid mode,
 *          the weight should reset to last config wrr queue weight
 *------------------------------------------------------------------------------
 */
static COS_TYPE_ERROR_CODE_E
COS_VM_Priv_SetEgressQueueMode(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T mode)
{
    COS_TYPE_ERROR_CODE_E result;

    if (mode != COS_VM_SCHEDULING_WEIGHT_ROUND_ROBIN &&
        mode != COS_VM_SCHEDULING_STRICT_PRIORITY &&
#if (SYS_CPNT_SWCTRL_Q_MODE_DRR == TRUE)
        mode != COS_VM_SCHEDULING_METHOD_DRR &&
        mode != COS_VM_SCHEDULING_METHOD_SP_DRR &&
#endif /* SYS_CPNT_SWCTRL_Q_MODE_DRR */
        mode != COS_VM_SCHEDULING_METHOD_SP_WRR)
    {
        return COS_TYPE_E_PARAMETER;
    }

    result = COS_VM_Priv_LocalSetEgressQueueMode(ifindex, priority, (COS_VM_SCHEDULING_METHOD_T) mode);


    if (result != COS_TYPE_E_NONE && result != COS_TYPE_E_NO_CHANGE)
    {
        return result;
    }

    if (result == COS_TYPE_E_NO_CHANGE &&
        (mode == COS_VM_SCHEDULING_WEIGHT_ROUND_ROBIN ||
         mode == COS_VM_SCHEDULING_STRICT_PRIORITY))
    {
        /* No need to reset weight
         */
        return COS_TYPE_E_NONE;
    }

    {
        UI16_T  id;
        UI32_T  queue;
        UI32_T  weight;

        result = COS_VM_PortPriorityToId(ifindex, priority, &id);
        ASSERT(result == COS_TYPE_E_NONE);

        /* Reset all wrr weight to driver,
         * strict queue will be reset to wrr queue
         */
        memset(cos_vm_port_entry[ifindex - 1].queue_weight.config_p[id]->is_strict,
               0,
               sizeof(cos_vm_port_entry[ifindex - 1].queue_weight.config_p[id]->is_strict));

        if (mode != COS_VM_SCHEDULING_STRICT_PRIORITY)
        {
            for (queue = MIN_QUEUE_VAL; queue < MAX_QUEUE_VAL + 1; queue++)
            {
                weight = cos_vm_port_entry[ifindex - 1].queue_weight.config_p[id]->wrr_weight[queue];

                result = COS_VM_Priv_LocalSetEgressQueueWeight(ifindex,
                                                              priority,
                                                              queue,
                                                              weight);
                ASSERT(COS_TYPE_E_NONE == result ||
                       COS_TYPE_E_NO_CHANGE == result);
            }
        }
    }

    SYS_CALLBACK_MGR_AnnounceCosPortConfigChanged(SYS_MODULE_COS, ifindex, priority);

    return COS_TYPE_E_NONE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_Priv_SetEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: Contorl anything need to do when change queue weight
 * INPUT:   ifindex  - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          weight   - queue weight
 * OUTPUT:
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
static COS_TYPE_ERROR_CODE_E
COS_VM_Priv_SetEgressQueueWeight(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T queue,
    UI32_T weight)
{
    COS_TYPE_ERROR_CODE_E result;

    COS_VM_DEBUG_PRINTF("Input ifindex: %lu, priority: %u, queue: %lu, weight: %lu",
                        ifindex, priority, queue, weight);

    if(SYS_ADPT_MAX_RATIO_OF_WRR < weight ||
       MAX_QUEUE_VAL < queue)
    {
        return COS_TYPE_E_PARAMETER;
    }

    result = COS_VM_Priv_LocalSetEgressQueueWeight(ifindex, priority, queue, weight);

    if (result != COS_TYPE_E_NONE)
    {
        return result;
    }

    SYS_CALLBACK_MGR_AnnounceCosPortConfigChanged(SYS_MODULE_COS, ifindex, priority);

    return COS_TYPE_E_NONE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_Priv_SetPre2InternalDscpForAll
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the all Precedence to internal Dscp
 * INPUT:   l_port  - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
static COS_TYPE_ERROR_CODE_E
COS_VM_Priv_SetPre2InternalDscpForAll(UI32_T l_port, UI16_T priority)
{
#define PORT_CONFIG_P       (cos_vm_port_entry[l_port-1].pre_to_dscp.config_p[id])

    COS_TYPE_ERROR_CODE_E error_code;
    UI16_T id;
    UI32_T i;
	
    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (l_port == COS_VM_LPORT_OF_GLOBAL)
    {
        UI8_T  all_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];

        for (i=0; i<INGRESS_PRE_TO_DSCP_MAPPING_ENTRY_NMB; i++)
        {
            COS_VM_GetAllPortList(all_port_list);
            if (FALSE == COS_VM_SetPortListIngressPre2Dscp(all_port_list, i , 
				PORT_CONFIG_P->table.CURRENT_PRE_TO_DSCP_MAPPING[i].phb, 
				PORT_CONFIG_P->table.CURRENT_PRE_TO_DSCP_MAPPING[i].color))
            {
                return COS_TYPE_E_SET_TO_SWCTRL;
            }
        }
    }
    else
    {	
        for (i=0; i<INGRESS_PRE_TO_DSCP_MAPPING_ENTRY_NMB; i++)
        {
            if (FALSE == SWCTRL_SetQosIngressPre2Dscp(l_port, i , 
				PORT_CONFIG_P->table.CURRENT_PRE_TO_DSCP_MAPPING[i].phb, 
				PORT_CONFIG_P->table.CURRENT_PRE_TO_DSCP_MAPPING[i].color))
            {
                return COS_TYPE_E_SET_TO_SWCTRL;
            }        
        }
    }

#undef PORT_CONFIG_P

    return COS_TYPE_E_NONE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_Priv_SetDscp2InternalDscpForAll
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the all Dscp to internal Dscp
 * INPUT:   l_port  - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
static COS_TYPE_ERROR_CODE_E
COS_VM_Priv_SetDscp2InternalDscpForAll(UI32_T l_port, UI16_T priority)
{
#define PORT_CONFIG_P       (cos_vm_port_entry[l_port-1].dscp_to_dscp.config_p[id])

    COS_TYPE_ERROR_CODE_E error_code;
    UI16_T id;
    UI32_T i;

    error_code = COS_VM_PortPriorityToId(l_port, priority, &id);
    if (COS_TYPE_E_NONE != error_code)
    {
        return error_code;
    }

    if (l_port == COS_VM_LPORT_OF_GLOBAL)
    {
        UI8_T  all_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];

        for (i=0; i<INGRESS_DSCP_TO_DSCP_MAPPING_ENTRY_NMB; i++)
        {
            COS_VM_GetAllPortList(all_port_list);
            if (FALSE == COS_VM_SetPortListIngressDscp2Dscp(all_port_list, i , 
				PORT_CONFIG_P->table.CURRENT_DSCP_TO_DSCP_MAPPING[i].phb, 
				PORT_CONFIG_P->table.CURRENT_DSCP_TO_DSCP_MAPPING[i].color))
            {
                return COS_TYPE_E_SET_TO_SWCTRL;
            }
        }
    }
    else
    {	
        for (i=0; i<INGRESS_DSCP_TO_DSCP_MAPPING_ENTRY_NMB; i++)
        {        
            if (FALSE == SWCTRL_SetQosIngressDscp2Dscp(l_port, i , 
				PORT_CONFIG_P->table.CURRENT_DSCP_TO_DSCP_MAPPING[i].phb, 
				PORT_CONFIG_P->table.CURRENT_DSCP_TO_DSCP_MAPPING[i].color))
            {
                return COS_TYPE_E_SET_TO_SWCTRL;
            }        
        }
    }

#undef PORT_CONFIG_P

    return COS_TYPE_E_NONE;

}

/* FUNCTION FOR L4_BACKDOOR */
#if (__APPLE__ && __MACH__)
#pragma mark - Backdoor
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetDebugMode
 *------------------------------------------------------------------------------
 * PURPOSE: Set debug mode flag
 * INPUT:   enable - TRUE/FALSE
 * OUTPUT:  None.
 * RETURN:  None
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
void
COS_VM_SetDebugMode(
    BOOL_T enable)
{
    cos_vm_debug_flag = enable;
}

static BOOL_T
COS_VM_Backdoor_ShowCos2Dscp(
    UI32_T l_port,
    UI16_T priority)
{
    int i, j;
    UI16_T id;

    if (COS_TYPE_E_NONE != COS_VM_PortPriorityToId(l_port, priority, &id))
    {
        printf("\r\n===Can't find priority index===\r\n");
        return FALSE;
    }

    if (TRUE != cos_vm_port_entry[l_port-1].config[id].enable)
    {
        printf("\r\n===priority not enable===\r\n");
        return FALSE;
    }

    printf("\r\n***********CoS to DSCP***************\r\n");
    printf("\r\n===priority config===\r\n");
    if (TRUE == cos_vm_port_entry[l_port-1].config[id].cos_to_dscp.common.modified)
    {
        printf("\r\n(cos, cfi)    (phb, color)\r\n");
        for(i = MIN_COS_VAL; i <= MAX_COS_VAL; i++)
        {
            for(j = 0; j < 2; j++)
            {
                printf("(%d, %d)         (%lu,%lu) \r\n",
                    i, j,
                    cos_vm_port_entry[l_port-1].config[id].cos_to_dscp.table.CURRENT_COS_TO_DSCP_MAPPING[i][j].phb,
                    cos_vm_port_entry[l_port-1].config[id].cos_to_dscp.table.CURRENT_COS_TO_DSCP_MAPPING[i][j].color);
            }
        }
    }
    else
    {
        printf("No set yet\r\n");
    }

    printf("\r\n===active config===\r\n");
    printf("\r\n(cos, cfi)    (phb, color)\r\n");
    for(i = MIN_COS_VAL; i <= MAX_COS_VAL; i++)
    {
        for(j = 0; j < 2; j++)
        {
            printf("(%d, %d)         (%lu,%lu) \r\n",
                i, j,
                cos_vm_port_entry[l_port-1].cos_to_dscp.active_p->table.CURRENT_COS_TO_DSCP_MAPPING[i][j].phb,
                cos_vm_port_entry[l_port-1].cos_to_dscp.active_p->table.CURRENT_COS_TO_DSCP_MAPPING[i][j].color);
        }
    }
    printf("\n***************************\n");

    return TRUE;
}

static BOOL_T
COS_VM_Backdoor_ShowDscp2Dscp(
    UI32_T l_port,
    UI16_T priority)
{
    int i, j, temp_dscp;
    UI16_T id;

    if (COS_TYPE_E_NONE != COS_VM_PortPriorityToId(l_port, priority, &id))
    {
        printf("\r\n===Can't find priority index===\r\n");
        return FALSE;
    }

    if (TRUE != cos_vm_port_entry[l_port-1].config[id].enable)
    {
        printf("\r\n===priority not enable===\r\n");
        return FALSE;
    }

    printf("\r\n***********DSCP To DSCP***********");
    printf("\r\n===priority config===\r\n");
    if (TRUE == cos_vm_port_entry[l_port-1].config[id].dscp_to_dscp.common.modified)
    {
        printf("(D0 , D1)|    0     1     2     3     4     5     6     7     8     9\r\n");
        printf("-------------------------------------------------------------------------\r\n");
        for(i = MIN_DSCP_VAL/10; i <= MAX_DSCP_VAL/10; i++)
        {
            printf("%3d      |", i);
            for (j = 0; j < 10; j++)
            {
                temp_dscp = i * 10 + j;
                if (temp_dscp < 64)
                {
                    printf("(%lu,%lu )",
                           cos_vm_port_entry[l_port-1].config[id].dscp_to_dscp.table.CURRENT_DSCP_TO_DSCP_MAPPING[temp_dscp].phb,
                           cos_vm_port_entry[l_port-1].config[id].dscp_to_dscp.table.CURRENT_DSCP_TO_DSCP_MAPPING[temp_dscp].color);
                }

            }
            printf("\r\n");
        }
    }
    else
    {
        printf("No set yet\r\n");
    }

    printf("\r\n===active config===\r\n");
    printf("(D0 , D1)|    0     1     2     3     4     5     6     7     8     9\r\n");
    printf("-------------------------------------------------------------------------\r\n");
    for(i = MIN_DSCP_VAL/10; i <= MAX_DSCP_VAL/10; i++)
    {
        printf("%3d      |", i);
        for (j = 0; j < 10; j++)
        {
            temp_dscp = i * 10 + j;
            if (temp_dscp < 64)
            {
                printf("(%lu,%lu) ",
                       cos_vm_port_entry[l_port-1].dscp_to_dscp.active_p->table.CURRENT_DSCP_TO_DSCP_MAPPING[temp_dscp].phb,
                       cos_vm_port_entry[l_port-1].dscp_to_dscp.active_p->table.CURRENT_DSCP_TO_DSCP_MAPPING[temp_dscp].color);
            }

        }
        printf("\r\n");
    }
    printf("\r\n***********************************\r\n");

    return TRUE;
}

static BOOL_T
COS_VM_Backdoor_ShowDscp2Queue(
    UI32_T l_port,
    UI16_T priority)
{
    int i;
    UI16_T id;

    if (COS_TYPE_E_NONE != COS_VM_PortPriorityToId(l_port, priority, &id))
    {
        printf("\r\n===Can't find priority index===\r\n");
        return FALSE;
    }

    if (TRUE != cos_vm_port_entry[l_port-1].config[id].enable)
    {
        printf("\r\n===priority not enable===\r\n");
        return FALSE;
    }

    printf("\r\n***********DSCP to Queue***************\r\n");
    printf("\r\n===priority config===\r\n");
    if (COS_VM_PRIORITY_USER == priority ||
        TRUE == cos_vm_port_entry[l_port-1].config[id].dscp_to_queue.common.modified)
    {
        printf("(PHB, QUEUE)\r\n");
        for(i = MIN_PHB_VAL; i <= MAX_PHB_VAL; i++)
        {
            printf("(%d, %d)\r\n",
                i,
                cos_vm_port_entry[l_port-1].config[id].dscp_to_queue.table.CURRENT_DSCP_TO_QUEUE_MAPPING[i]);
        }
    }
    else
    {
        printf("No set yet\r\n");
    }

    printf("\r\n===active config===\r\n");
    printf("(PHB, QUEUE)\r\n");
    for(i = MIN_PHB_VAL; i <= MAX_PHB_VAL; i++)
    {
        printf("(%d, %d)\r\n",
            i,
            cos_vm_port_entry[l_port-1].dscp_to_queue.active_p->table.CURRENT_DSCP_TO_QUEUE_MAPPING[i]);
    }
    printf("\n***************************\n");

    return TRUE;
}

static BOOL_T
COS_VM_Backdoor_ShowQueueMode(
    UI32_T l_port,
    UI16_T priority)
{
    UI16_T id;
    char output[5][40] = {
            "COS_VM_SCHEDULING_WEIGHT_ROUND_ROBIN",
            "COS_VM_SCHEDULING_STRICT_PRIORITY",
            "COS_VM_SCHEDULING_METHOD_DRR",
            "COS_VM_SCHEDULING_METHOD_SP_WRR",
            "COS_VM_SCHEDULING_METHOD_SP_DRR"
        };

    if (COS_TYPE_E_NONE != COS_VM_PortPriorityToId(l_port, priority, &id))
    {
        printf("\r\n===Can't find priority index===\r\n");
        return FALSE;
    }

    if (TRUE != cos_vm_port_entry[l_port-1].config[id].enable)
    {
        printf("\r\n===priority not enable===\r\n");
        return FALSE;
    }

    printf("\r\n***********QueueMode***********\r\n");
    printf("\r\n===priority config===\r\n");
    if (TRUE == cos_vm_port_entry[l_port-1].config[id].queue_mode.common.modified)
    {
        printf("%s\r\n",
            output[(cos_vm_port_entry[l_port-1].queue_mode.config_p[id]->mode)-COS_VM_SCHEDULING_WEIGHT_ROUND_ROBIN]);
    }
    else
    {
        printf("No set yet\r\n");
    }

    printf("\r\n===active config===\r\n");
    printf("%s\r\n",
        output[(cos_vm_port_entry[l_port-1].queue_mode.active_p->mode)-COS_VM_SCHEDULING_WEIGHT_ROUND_ROBIN]);

    return TRUE;
}

static BOOL_T
COS_VM_Backdoor_ShowWrrQueueWeight(
    UI32_T l_port,
    UI16_T priority)
{
    UI16_T id;
    int i;

    if (COS_TYPE_E_NONE != COS_VM_PortPriorityToId(l_port, priority, &id))
    {
        printf("\r\n===Can't find priority index===\r\n");
        return FALSE;
    }

    if (TRUE != cos_vm_port_entry[l_port-1].config[id].enable)
    {
        printf("\r\n===priority not enable===\r\n");
        return FALSE;
    }

    printf("\r\n***********WrrQueueWeight***********");
    printf("\r\n===priority config===\r\n");
    if (TRUE == cos_vm_port_entry[l_port-1].config[id].queue_weight.common.modified)
    {
        printf("Queue    weight\r\n");
        for (i = 0; i < SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE; i++)
        {
            printf("%5d    %5lu\r\n", i,
                (cos_vm_port_entry[l_port-1].queue_weight.config_p[id]->is_strict[i])
                    ? 0 : cos_vm_port_entry[l_port-1].queue_weight.config_p[id]->wrr_weight[i]);
        }
    }
    else
    {
        printf("No set yet\r\n");
    }

    printf("\r\n===active config===\r\n");
    printf("Queue    weight\r\n");
    for (i = 0; i < SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE; i++)
    {
        printf("%5d    %5lu\r\n", i,
            (cos_vm_port_entry[l_port-1].queue_weight.active_p->is_strict[i])
                ? 0 : cos_vm_port_entry[l_port-1].queue_weight.active_p->wrr_weight[i]);
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_Backdoor_ShowCosMappingTable
 *------------------------------------------------------------------------------
 * PURPOSE: This function resets the wrr queue weight to default by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          table    - to show which table
 * OUTPUT:  None
 * RETURN:  TRUE/FALSE
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
BOOL_T
COS_VM_Backdoor_ShowCosMappingTable(
    UI32_T l_port,
    UI16_T priority,
    UI32_T table)
{
    switch (table)
    {
        case COS_VM_CONFIG_TABLE_COS2DSCP:
            COS_VM_Backdoor_ShowCos2Dscp(l_port, priority);
            break;
        case COS_VM_CONFIG_TABLE_DSCP2DSCP:
            COS_VM_Backdoor_ShowDscp2Dscp(l_port, priority);
            break;
        case COS_VM_CONFIG_TABLE_QUEUEMODE:
            COS_VM_Backdoor_ShowQueueMode(l_port, priority);
            break;
        case COS_VM_CONFIG_TABLE_QUEUEWEIGHT:
            COS_VM_Backdoor_ShowWrrQueueWeight(l_port, priority);
            break;
        case COS_VM_CONFIG_TABLE_DSCP2QUEUE:
            COS_VM_Backdoor_ShowDscp2Queue(l_port, priority);
            break;
    }

    return TRUE;
}


