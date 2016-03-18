/* FUNCTION NAME: RULE_OM.C
* PURPOSE:
*   save DiffServ, ACL database(OM)
*
* NOTES:
*   1, object's index (e.g. class-map index, acl index) count from 1 (1-based) and we store objects
*      in arrays (zero-based). Therefore, The gap between array index and object index is 1.
*      That is, object index (A) is stored at array (A - 1).
*
*REASON:
*    DESCRIPTION:
*    CREATOR:       pttch
*    Date 2005/5/1
*
* Copyright(C)      Accton Corporation, 2005
*/

/* INCLUDE FILE DECLARATIONS
 */
#include <string.h> /* for memcpy, etc */
#include <stdio.h>
#include "sys_type.h"
#include "sys_bld.h"
#include "sys_cpnt.h"
#include "sys_module.h"
#include "sysfun.h"

/*#include "ipi_porting.h"*/
#include "l_stdlib.h" /* for htons */
#include "l_cvrt.h"

#include "backdoor_mgr.h"
#include "l_mm.h" /* for RULE_CTRL_USE_REF_LIST */

#include "rule_type.h"
#include "rule_om.h"
#include "rule_ctrl.h"

#include "leaf_es3626a.h"
#include "stktplg_om.h" /* to avoid include swctrl.h */

#if (SYS_CPNT_TIME_BASED_ACL== TRUE)
#include "time_range_type.h"
#endif

/* NAMING CONSTANT DECLARATIONS
 */
#define RULE_OM_DFLT_METER_RATE                         0
#define RULE_OM_DFLT_METER_BURST_SIZE                   0
#define RULE_OM_DFLT_METER_INTERVAL                     0

/* Reference list. If SYS_CPNT_COS_INTER_DSCP == FALSE,
 * we need to rebind the MF rule when dscp-to-cos map changed.
 */
#if (SYS_CPNT_COS_INTER_DSCP == TRUE)
#define RULE_CTRL_USE_REF_LIST                          FALSE
#else
#define RULE_CTRL_USE_REF_LIST                          TRUE
#endif

/* these two constants are used for the array allocation. */
#define RULE_OM_MAX_NBR_OF_IP_PRECEDENCE_ENTRY          (RULE_TYPE_MAX_OF_IP_PRECEDENCE + 1)
#define RULE_OM_MAX_NBR_OF_IP_DSCP_ENTRY                (RULE_TYPE_MAX_OF_IP_DSCP + 1)

/* MACRO FUNCTION DECLARATIONS
 */
#define RULE_OM_IS_BAD_POLICY_MAP_IDX(idx)          ((0 >= idx) || (idx > SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP))
#define RULE_OM_IS_VALID_POLICY_MAP_IDX(idx)        ((0 < idx) && (idx <= SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP))

#define RULE_OM_IS_BAD_POLICY_ELEMENT_IDX(idx)      ((0 >= idx) || (idx > SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS))
#define RULE_OM_IS_VALID_POLICY_ELEMENT_IDX(idx)    ((0 < idx) && (idx <= SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS))

#define RULE_OM_IS_BAD_CLASS_MAP_IDX(idx)           ((0 >= idx) || (idx > SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS_MAP))
#define RULE_OM_IS_VALID_CLASS_MAP_IDX(idx)         ((0 < idx) && (idx <= SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS_MAP))

#define RULE_OM_IS_BAD_ACTION_IDX(idx)              ((0 >= idx) || (idx > SYS_ADPT_DIFFSERV_MAX_NBR_OF_ACTIONS))
#define RULE_OM_IS_VALID_ACTION_IDX(idx)            ((0 < idx) && (idx <= SYS_ADPT_DIFFSERV_MAX_NBR_OF_ACTIONS))

#define RULE_OM_IS_BAD_METER_IDX(idx)               ((0 >= idx) || (idx > SYS_ADPT_DIFFSERV_MAX_NBR_OF_METER))
#define RULE_OM_IS_VALID_METER_IDX(idx)             ((0 < idx) && (idx <= SYS_ADPT_DIFFSERV_MAX_NBR_OF_METER))

#define RULE_OM_IS_BAD_ACL_IDX(idx)                 ((0 >= idx) || (idx > _countof(shmem_data_p->acl_table)))
#define RULE_OM_IS_VALID_ACL_IDX(idx)               ((0 < idx) && (idx <= _countof(shmem_data_p->acl_table)))

#define RULE_OM_IS_BAD_ACE_IDX(idx)                 ((0 >= idx) || (idx > _countof(shmem_data_p->ace_table)))
#define RULE_OM_IS_VALID_ACE_IDX(idx)               ((0 < idx) && (idx <= _countof(shmem_data_p->ace_table)))

#define RULE_OM_IS_BAD_UPORT_IFIDX(idx)             ((0 >= idx) || (idx > SYS_ADPT_TOTAL_NBR_OF_LPORT))

#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
#define RULE_OM_IS_BAD_MATCH_TYPE(type)             ((RULE_TYPE_CLASS_MAP_MATCH_ANY != type) && \
                                                     (RULE_TYPE_CLASS_MAP_MATCH_ALL != type))
#else
#define RULE_OM_IS_BAD_MATCH_TYPE(type)             ((RULE_TYPE_CLASS_MAP_MATCH_ANY != type))
#endif /* SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL */

#define RULE_OM_IS_BAD_UNIT_ID(id)                  ((0 >= id) || (id > SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK))
#define RULE_OM_IS_BAD_DEVICE_ID(id)                (id >= SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT)

#define RULE_OM_IS_BAD_IP_PRECEDENCE(value)         (value > RULE_TYPE_MAX_OF_IP_PRECEDENCE)
#define RULE_OM_IS_BAD_IP_DSCP(value)               (value > RULE_TYPE_MAX_OF_IP_DSCP)

#define RULE_OM_IS_ENTRY_ACTIVE(row_status)         (VAL_diffServPolicyMapStatus_active == row_status)
#define RULE_OM_IS_ENTRY_AVAILABLE(row_status)      ((VAL_diffServPolicyMapStatus_active == row_status) || \
                                                     (VAL_diffServPolicyMapStatus_notReady == row_status))
#define RULE_OM_IS_ENTRY_DESTROY(row_status)        (0 == row_status)
#define RULE_OM_SET_ENTRY_ACTIVE(row_status)        {row_status = VAL_diffServPolicyMapStatus_active;}
#define RULE_OM_SET_ENTRY_NOTREADY(row_status)      {row_status = VAL_diffServPolicyMapStatus_notReady;}
#define RULE_OM_SET_ENTRY_DESTROY(row_status)       {row_status = 0;}

#define RULE_OM_IS_IP_ACL(acl_type)                 ((acl_type == RULE_TYPE_IP_EXT_ACL) || \
                                                     (acl_type == RULE_TYPE_IP_STD_ACL))

#define RULE_OM_IS_MAC_ACL(acl_type)                (acl_type == RULE_TYPE_MAC_ACL)

#define RULE_OM_IS_IPV6_ACL(acl_type)               ((acl_type == RULE_TYPE_IPV6_EXT_ACL) || \
                                                     (acl_type == RULE_TYPE_IPV6_STD_ACL))

#define RULE_OM_IS_ARP_ACL(acl_type)                (acl_type == RULE_TYPE_ARP_ACL)

/* whether bno-th bit in bmp is 1 or 0 (bmp is an UI8_T array, bno is a zero-based bit index) */
#define RULE_OM_IS_BIT_ON(bmp, bno)                 (bmp[bno/8] & (1 << (7 - bno%8)))
#define RULE_OM_IS_BIT_OFF(bmp, bno)                (!RULE_OM_IS_BIT_ON(bmp, bno))
#define RULE_OM_SET_BIT_ON(bmp, bno)                {bmp[bno/8] |= (1 << (7 - bno%8));}
#define RULE_OM_SET_BIT_OFF(bmp, bno)               {bmp[bno/8] &= ~(1 << (7 - bno%8));}

#define RULE_OM_INST2PTR(in)                        \
    RULE_OM_LocalInst2Ptr(in)

#define RULE_OM_PTR2INST(shm_ptr)                   \
    RULE_OM_LocalPtr2Inst(shm_ptr)

#define RULE_OM_VALIDATE_INST(in)                   \
    RULE_OM_LocalValidateInstance(in)

#define RULE_OM_INDEX_OF(in)                        \
    RULE_OM_LocalIndexOf(in)

#define LOG(fmt, ...)                               \
    RULE_TYPE_LOG(RULE_OM_GetDebugFlag() & RULE_OM_DEBUG_OM_ERR, fmt, ##__VA_ARGS__)

#define RULE_OM_EnterCriticalSection() SYSFUN_TakeSem(rule_om_sem_id, SYSFUN_TIMEOUT_WAIT_FOREVER)
#define RULE_OM_LeaveCriticalSection() SYSFUN_GiveSem(rule_om_sem_id)

#define RULE_OM_LeaveCriticalSectionReturnState(state)  \
{                                                       \
    RULE_OM_LeaveCriticalSection();                     \
    return state;                                       \
}

#define RULE_OM_LeaveCriticalSectionReturnVoid()        \
{                                                       \
    RULE_OM_LeaveCriticalSection();                     \
    return;                                             \
}

#define RULE_OM_IS_ZERO_MAC(mac)                                 \
    (0 == (mac[0] | mac[1] | mac[2] | mac[3] | mac[4] | mac[5]))

#define RULE_OM_IS_ZERO_IPV4_ADDR(addr)                          \
    (0 == (addr[0] | addr[1] | addr[2] | addr[3]))

#define RULE_OM_IS_ZERO_IPV6_ADDR(addr)                          \
    (0 == (addr[0]  | addr[1]  | addr[2]  | addr[3] |            \
           addr[4]  | addr[5]  | addr[6]  | addr[7] |            \
           addr[8]  | addr[9]  | addr[10] | addr[11] |           \
           addr[12] | addr[13] | addr[14] | addr[15]))

#ifndef _countof
#define _countof(ary)           (sizeof(ary)/sizeof(*ary))
#endif

#ifndef ASSERT
#define ASSERT(eq)
#endif

/* DATA TYPE DECLARATIONS
 */

#if (RULE_CTRL_USE_REF_LIST == TRUE)
typedef struct RULE_OM_CosReferenceEntry_S
{
    UI16_T  policy_map_index;   /* UI16_T is large enough */
    UI16_T  class_map_index;

    struct RULE_OM_CosReferenceEntry_S  *prev_entry;
    struct RULE_OM_CosReferenceEntry_S  *next_entry;
} RULE_OM_CosReferenceEntry_T; /* a double linked-list entry */
#endif

/* LOCAL SUBPROGRAM DECLARATIONS
 */

static void
RULE_OM_LocalSetPolicyMapInstance(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_inst_p
);

static RULE_TYPE_CLASS_INSTANCE_PTR_T
RULE_OM_LocalGetPolicyMapInstance(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T direction
);

static UI32_T
RULE_OM_Local_GetNextPort_BindPolicyMap(
    UI32_T  policy_map_index,
    UI32_T  *uport_ifindex_p,
    RULE_TYPE_InOutDirection_T *inout_profile_p
);

static UI32_T
RULE_OM_Local_GetNextPort_BindClassMap(
    UI32_T  class_map_index,
    UI32_T  *uport_ifindex_p,
    RULE_TYPE_InOutDirection_T  *inout_profile_p,
    UI32_T  *policy_map_index_p
);

static UI32_T
RULE_OM_LocalGetClassMapElementIndex(
    UI32_T class_map_index,
    RULE_TYPE_UI_ClassMapElement_T *element_entry,
    UI32_T *element_index
);

static RULE_TYPE_ClassMap_T *RULE_OM_LocalGetClassMapByIndex(UI32_T class_map_index);

static UI32_T RULE_OM_LocalSearchPolicyMapByName(const char *policy_map_name, UI32_T *policy_map_index);
static RULE_TYPE_PolicyMap_T *RULE_OM_LocalGetPolicyMapByIndex(UI32_T policy_map_index);

static UI32_T
RULE_OM_LocalPolicyMapPtrToIndex(
    RULE_TYPE_PolicyMap_T *policy_map_p
);

static RULE_TYPE_PolicyMap_T *
RULE_OM_LocalGetNextPolicyMap(
    UI32_T *policy_map_index
);

static RULE_TYPE_PolicyMap_T *
RULE_OM_LocalGetNextPolicyMapByClassMapIndex(
    UI32_T class_map_index,
    UI32_T *policy_map_index_p
);

static RULE_TYPE_PolicyMap_T *
RULE_OM_LocalGetNextPolicyMapByMeterIndex(
    UI32_T meter_index,
    UI32_T *policy_map_index_p
);

static RULE_TYPE_PolicyMap_T *
RULE_OM_LocalGetNextPolicyMapByActionIndex(
    UI32_T action_index,
    UI32_T *policy_map_index_p
);

static BOOL_T RULE_OM_LocalAllocatePolicyMapElement(UI32_T *policy_map_element_index);
static void RULE_OM_LocalFreePolicyMapElement(UI32_T pmap_ele_index);
static RULE_TYPE_PolicyElement_T *RULE_OM_LocalGetPolicyMapElementByIndex(UI32_T policy_map_element_index);
#if 0
static RULE_TYPE_PolicyElement_T *RULE_OM_LocalGetPolicyMapElementByActionIndex(UI32_T action_index);
#endif
static RULE_TYPE_PolicyElement_T *RULE_OM_LocalGetPolicyMapElement(UI32_T policy_map_index, UI32_T class_map_index);

static BOOL_T
RULE_OM_LocalAllocateMeterIndex(
    UI32_T *meter_index
);

static void RULE_OM_LocalFreeMeter(UI32_T meter_index);
static RULE_TYPE_TBParamEntry_T *RULE_OM_LocalGetMeterEntryByIndex(UI32_T meter_index);

static UI32_T
RULE_OM_LocalGetActionIndexFromPolicyMapElement(
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T *action_index
);

static BOOL_T
RULE_OM_LocalAllocate_Action_Index(
    UI32_T *action_index
);

static void
RULE_OM_LocalFreeAction(
    UI32_T action_index
);

static RULE_TYPE_Action_T *
RULE_OM_LocalGetActionEntryByIndex(
    UI32_T action_index
);

static UI32_T
RULE_OM_LocalCreateMf(
    const RULE_TYPE_MfEntry_T *mf_entry,
    UI32_T *ace_index
);

static UI32_T
RULE_OM_LocalConvertMf2Ace(
    const RULE_TYPE_MfEntry_T *mf_entry_p,
    RULE_TYPE_Ace_Entry_T *ace_entry_p
);

static UI32_T
RULE_OM_LocalAllocateAclWithRange(
    UI32_T min_index,
    UI32_T max_index,
    UI32_T *acl_index
);

static void RULE_OM_LocalFreeAcl(RULE_TYPE_Acl_T *acl_entry);
static RULE_TYPE_Acl_T *RULE_OM_LocalGetAclByIndex(UI32_T acl_index);

static UI32_T
RULE_OM_LocalDelAcl(
    UI32_T acl_index
);

static UI32_T
RULE_OM_LocalGetAclIdByName(
    const char *acl_name,
    UI32_T *acl_index
);

static UI32_T
RULE_OM_LocalGetNextAclByIndex(
    UI32_T *acl_index,
    RULE_TYPE_Acl_T *acl_entry
);

static UI32_T
RULE_OM_LocalGetNextAceByAcl(
    const RULE_TYPE_Acl_T *acl_p,
    UI32_T *ace_index,
    RULE_TYPE_Ace_Entry_T **next_ace_entry_pp
);

static UI32_T
RULE_OM_LocalAllocateAceEntryWithRange(
    UI32_T min_index,
    UI32_T max_index,
    UI32_T *ace_index
);

static void
RULE_OM_LocalFreeAceEntry(
    UI32_T ace_index
);

static BOOL_T
RULE_OM_LocalIsSameAce(
    const RULE_TYPE_Acl_T *acl_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry,
    UI32_T *ace_index
);

static BOOL_T
RULE_OM_LocalIsSameAceWithoutAccessField(
    const RULE_TYPE_Ace_Entry_T *src_p,
    const RULE_TYPE_Ace_Entry_T *dst_p
);

static UI32_T
RULE_OM_LocalSetAceEntryToDefaultValue(
    RULE_TYPE_Ace_Entry_T *ace_entry
);

static UI32_T
RULE_OM_LocalAddAce2Acl(
    RULE_TYPE_Acl_T *acl_p,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_Ace_Entry_T *ace_entry
);

static UI32_T
RULE_OM_LocalCopyAcl(
    UI32_T  src_acl_index,
    UI32_T  dst_acl_index
);

static UI32_T
RULE_OM_LocalDelAllAceFromAcl(
    RULE_TYPE_Acl_T *acl_p
);

static UI32_T
RULE_OM_LocalDelAceFromAcl(
    RULE_TYPE_Acl_T *acl_p,
    UI32_T ace_index
);

static BOOL_T
RULE_OM_LocalUpdateAction(
    UI32_T action_index,
    RULE_TYPE_Action_T *action_entry
);

static RULE_TYPE_Ace_Entry_T *RULE_OM_LocalGetAceByIndex(UI32_T ace_index);

static BOOL_T
RULE_OM_LocalIsValidIfindex(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction
);

static BOOL_T
RULE_OM_LocalGetNextInterface(
    UI32_T *ifindex_p,
    RULE_TYPE_InOutDirection_T *direction_p
);

static RULE_TYPE_PortEntry_T *
RULE_OM_LocalGetPortEntryByIfIndex(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile
);

static RULE_TYPE_ACL_MEMENTO_PTR_T
RULE_OM_LocalGetPortAclByType(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_AclType_T acl_type
);

static RULE_TYPE_ACL_MEMENTO_PTR_T
RULE_OM_LocalGetPortAclByPrecedence(
    UI32_T ifindex,
    UI32_T precedence
);

static RULE_TYPE_ACL_MEMENTO_PTR_T
RULE_OM_LocalGetNextPortAclByPrecedence(
    UI32_T ifindex,
    UI32_T *precedence_p
);

static RULE_TYPE_ACL_MEMENTO_PTR_T
RULE_OM_LocalGetNextPortAclByDirectionAndPrecedence(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T *precedence_p
);

static UI32_T RULE_OM_LocalGetPolicyMapElementPrecedence(UI32_T policy_map_index, UI32_T class_map_index, UI32_T *precedence);
static UI32_T RULE_OM_LocalGetClassMapElementPrecedence(UI32_T class_map_index, RULE_TYPE_AclType_T class_type, UI32_T class_index, UI32_T *precedence);

static BOOL_T
RULE_OM_LocalIsValidElementTypeOflassMap(
    RULE_TYPE_ClassMap_T *class_map_p,
    RULE_TYPE_AclType_T element_type
);

static UI32_T
RULE_OM_LocalIsAclHaveAce(
    RULE_TYPE_Acl_T *acl_p,
    UI32_T ace_index
);

static UI32_T
RULE_OM_LocalValidateMatchAllClassMapElement(
    UI32_T class_map_index,
    const RULE_TYPE_ClassMap_T *class_map_p,
    RULE_TYPE_UI_ClassMapElement_T *element_entry
);

static UI32_T
RULE_OM_LocalValidateAce(
    const RULE_TYPE_Ace_Entry_T *ace_p
);

static UI32_T
RULE_OM_LocalValidateIpAce(
    const RULE_TYPE_Ace_Entry_T *ace_p
);

static UI32_T
RULE_OM_LocalValidateMacAce(
    const RULE_TYPE_Ace_Entry_T *ace_p
);

static UI32_T
RULE_OM_LocalValidateIpv6Ace(
    const RULE_TYPE_Ace_Entry_T *ace_p
);

static UI32_T RULE_OM_LocalCheckAceCollisionToAcl(const RULE_TYPE_Ace_Entry_T *ace_entry, const RULE_TYPE_Acl_T *acl_entry);
static UI32_T RULE_OM_LocalCheckIpAceCollisionToAcl(const RULE_TYPE_IpAce_Entry_T *ip_ace, const RULE_TYPE_Acl_T *acl_entry);
static UI32_T RULE_OM_LocalCheckMacAceCollisionToAcl(const RULE_TYPE_MacAce_Entry_T *mac_ace, const RULE_TYPE_Acl_T *acl_entry);
static UI32_T RULE_OM_LocalCheckIpv6AceCollisionToAcl(const RULE_TYPE_Ipv6Ace_Entry_T *ipv6_ace, const RULE_TYPE_Acl_T *acl_entry);
static UI32_T RULE_OM_LocalCheckActionCollision(UI32_T action_bitmap);

static UI32_T
RULE_OM_LocalValidateMeter(
    const RULE_TYPE_TBParamEntry_T *meter_entry
);

static UI32_T
RULE_OM_LocalExtraValidateMeter(
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T meter_index,
    const RULE_TYPE_TBParamEntry_T *meter_entry
);

static UI32_T
RULE_OM_LocalValidateAction(
    const RULE_TYPE_Action_T *action_entry
);

static UI32_T
RULE_OM_LocalExtraValidateAction(
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T action_index,
    const RULE_TYPE_UI_Action_T *action_entry
);

static UI32_T
RULE_OM_LocalValidateAceField(
    const RULE_TYPE_Ace_Entry_T *ace_entry,
    UI32_T field_index,
    UI32_T owner
);

static RULE_TYPE_RETURN_TYPE_T
RULE_OM_LocalValidateAceAccessField(
    const RULE_TYPE_Ace_Entry_T *ace_entry_p,
    UI32_T field_index
);

static UI32_T
RULE_OM_LocalValidateIpAceFieldForACL(
    const RULE_TYPE_IpAce_Entry_T *ace_entry,
    RULE_TYPE_AclType_T ace_type,
    UI32_T field_index
);

static UI32_T
RULE_OM_LocalValidateMacAceFieldForACL(
    const RULE_TYPE_MacAce_Entry_T *ace_entry,
    RULE_TYPE_AclType_T ace_type,
    UI32_T field_index
);

static UI32_T
RULE_OM_LocalValidateMacAceMixedFieldForACL(
    const RULE_TYPE_Ace_Entry_T *ace_entry,
    RULE_TYPE_AclType_T ace_type,
    UI32_T field_index
);

#if (SYS_CPNT_DAI == TRUE)
static UI32_T
RULE_OM_LocalValidateArpAceFieldForACL(
    const RULE_TYPE_ArpAce_Entry_T *ace_entry,
    UI32_T field_index
);
#endif /* SYS_CPNT_DAI */

static UI32_T
RULE_OM_LocalValidateIpv6AceFieldForACL(
    const RULE_TYPE_Ipv6Ace_Entry_T *ace_entry,
    RULE_TYPE_AclType_T ace_type,
    UI32_T field_index
);

static UI32_T RULE_OM_LocalValidateIpAceFieldForMF(const RULE_TYPE_IpAce_Entry_T *ace_entry, UI32_T field_index);
static UI32_T RULE_OM_LocalValidateMacAceFieldForMF(const RULE_TYPE_MacAce_Entry_T *ace_entry, UI32_T field_index);
static UI32_T RULE_OM_LocalValidateIpv6AceFieldForMF(const RULE_TYPE_Ipv6Ace_Entry_T *ace_entry, UI32_T field_index);

static UI32_T RULE_OM_LocalRemoveAndCompactPolicyMapElement(RULE_TYPE_PolicyMap_T *policy_map, UI32_T remove_index);
static UI32_T RULE_OM_LocalRemoveAndCompactClassMapElement(RULE_TYPE_ClassMap_T *class_map, UI32_T remove_index);

static UI32_T RULE_OM_LocalClassifyUIAcl(const RULE_TYPE_UI_AclEntry_T *acl_entry, RULE_TYPE_AclType_T *class_type);
static UI32_T RULE_OM_LocalClassifyUIMf(const RULE_TYPE_MfEntry_T *mf_entry, RULE_TYPE_AclType_T *class_type);

static UI32_T
RULE_OM_LocalGetUIAclIndex(
    UI32_T class_id
);

static UI32_T RULE_OM_LocalConvert2UIPortEntry(const RULE_TYPE_PortEntry_T* src, RULE_TYPE_UI_PortEntry_T* dst);
static UI32_T RULE_OM_LocalConvert2UIPolicyMap(const RULE_TYPE_PolicyMap_T* src, RULE_TYPE_UI_PolicyMap_T* dst);
static UI32_T RULE_OM_LocalConvert2UIPolicyMapElement(const RULE_TYPE_PolicyElement_T* src, RULE_TYPE_UI_PolicyElement_T* dst);
static UI32_T RULE_OM_LocalConvert2UIClassMap(const RULE_TYPE_ClassMap_T* src, RULE_TYPE_UI_ClassMap_T* dst);
static UI32_T RULE_OM_LocalConvert2UIClassMapElement(const RULE_TYPE_ClassMap_T* src, UI32_T element_index, RULE_TYPE_UI_ClassMapElement_T* dst);
static UI32_T RULE_OM_LocalConvert2UIAction(const RULE_TYPE_Action_T* src, RULE_TYPE_UI_Action_T* dst);
static UI32_T RULE_OM_LocalConvert2UIAcl(const RULE_TYPE_Acl_T* src, RULE_TYPE_UI_AclEntry_T* dst);

static UI32_T RULE_OM_LocalConvert2MibPolicyMap(const RULE_TYPE_PolicyMap_T* src, RULE_TYPE_MIB_PolicyMap_T* dst);
static UI32_T RULE_OM_LocalConvert2MibClassMap(const RULE_TYPE_ClassMap_T* src, RULE_TYPE_MIB_ClassMap_T* dst);

static UI32_T
RULE_OM_LocalConvert2MibAcl(
    const RULE_TYPE_Acl_T* src,
    RULE_TYPE_MIB_Acl_T* dst
);

static UI32_T RULE_OM_LocalConvert2MibAction(const RULE_TYPE_Action_T* src, RULE_TYPE_MIB_Action_T* dst);

static UI32_T RULE_OM_LocalAddIpDscpReference(UI32_T ip_dscp, UI32_T policy_map_index, UI32_T class_map_index);
static UI32_T RULE_OM_LocalAddIpPrecedenceReference(UI32_T ip_precedence, UI32_T policy_map_index, UI32_T class_map_index);

#if (RULE_CTRL_USE_REF_LIST == TRUE)
static UI32_T RULE_OM_LocalIsExitedIpDscpReference(UI32_T ip_dscp, UI32_T policy_map_index, UI32_T class_map_index);
static UI32_T RULE_OM_LocalIsExitedIpPrecedenceReference(UI32_T ip_precedence, UI32_T policy_map_index, UI32_T class_map_index);
#endif

static void RULE_OM_LocalFreeIpDscpReference(UI32_T ip_dscp, UI32_T policy_map_index, UI32_T class_map_index);
static void RULE_OM_LocalFreeIpPrecedenceReference(UI32_T ip_precedence, UI32_T policy_map_index, UI32_T class_map_index);

static void
RULE_OM_LocalRemoveCosReference(
    UI32_T policy_map_index,
    UI32_T class_map_index
);

#if (SYS_CPNT_DAI == TRUE)
static UI32_T RULE_OM_LocalValidateArpAce(const RULE_TYPE_ArpAce_Entry_T *arp_ace);
#endif /* SYS_CPNT_DAI */

static UI32_T
RULE_OM_LocalIsExistPolicyMapName(
    const char *policy_map_name
);

static UI32_T
RULE_OM_LocalGetPolicyMapIndexByElementIndex(
    UI32_T policy_element_index,
    UI32_T *policy_map_index
);

static UI32_T
RULE_OM_LocalGetPolicyMapElementIndexByMeter(
    UI32_T meter_index,
    UI32_T *policy_element_index
);

static UI32_T
RULE_OM_LocalGetClassMapIndexByName(
    const char *class_map_name,
    UI32_T *class_map_index
);

static UI32_T
RULE_OM_LocalClassifyAcl(
    const RULE_TYPE_Acl_T *acl_entry,
    RULE_TYPE_AclType_T *class_type
);

static UI32_T
RULE_OM_LocalClassifyClassMap(
    const RULE_TYPE_ClassMap_T *class_map,
    RULE_TYPE_AclType_T *class_type
);

static UI32_T
RULE_OM_LocalConvertToAction(
    const RULE_TYPE_UI_Action_T *src_action,
    RULE_TYPE_Action_T *dst_action
);

static UI32_T
RULE_OM_LocalGetPolicyMapElementIndexByAction(
    UI32_T action_index,
    UI32_T *policy_element_index
);

static BOOL_T
RULE_OM_LocalIsAclTypeMatched(
    RULE_TYPE_AclType_T src_type,
    RULE_TYPE_AclType_T dst_type
);

static UI32_T
RULE_OM_LocalGetAclIndexByAce(
    UI32_T ace_index,
    UI32_T *acl_index,
    RULE_TYPE_Acl_T **acl_pp
);

static UI32_T
RULE_OM_LocalGetClassMapIndexByMf(
    UI32_T mf_index,
    UI32_T *class_map_index
);

static UI32_T
RULE_OM_LocalClassifyMf(
    const RULE_TYPE_Ace_Entry_T *mf_entry,
    RULE_TYPE_AclType_T *class_type
);

static UI32_T
RULE_OM_LocalClassifyAce(
    const RULE_TYPE_Ace_Entry_T *ace_entry,
    RULE_TYPE_AclType_T *class_type
);

static UI32_T
RULE_OM_LocalSetInProfileActionToDefault(
    UI32_T action_index
);

static UI32_T
RULE_OM_LocalSetOutOfProfileActionToDefault(
    UI32_T action_index
);

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
static UI32_T
RULE_OM_LocalSetClassActionToDefault(
    UI32_T action_index
);
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

static UI32_T
RULE_OM_LocalSetOutOfProfileActionToNull(
    UI32_T action_index
);

static UI32_T
RULE_OM_LocalNotify(
    RULE_TYPE_NOTIFY_MESSAGE_T message,
    void *param1
);

static UI32_T
RULE_OM_LocalCountBitsSet(
    UI32_T value
);

static RULE_TYPE_RETURN_TYPE_T
RULE_OM_LocalAclTypeToPortAclType(
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_AclType_T *port_acl_type_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_OM_LocalPrecedenceToPortAclType(
    UI32_T precedence,
    RULE_TYPE_AclType_T *port_acl_type_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_OM_LocalPrecedenceToDirection(
    UI32_T precedence,
    RULE_TYPE_InOutDirection_T *direction_p
);

static void
RULE_OM_LocalValidateInstance(
    RULE_TYPE_INSTANCE_PTR_T in
);

static UI32_T
RULE_OM_LocalIndexOf(
    void *entry
);

static RULE_TYPE_SHM_POINTER_T
RULE_OM_LocalInst2Ptr(
    void *entry
);

static void*
RULE_OM_LocalPtr2Inst(
    RULE_TYPE_SHM_POINTER_T ptr
);

static void
RULE_OM_LL_LocalInsertAfter(
    RULE_TYPE_SHM_POINTER_T list_ptr,
    RULE_TYPE_SHM_POINTER_T node_ptr,
    RULE_TYPE_SHM_POINTER_T new_node_ptr
);

static void
RULE_OM_LL_LocalInsertBefore(
    RULE_TYPE_SHM_POINTER_T list_ptr,
    RULE_TYPE_SHM_POINTER_T node_ptr,
    RULE_TYPE_SHM_POINTER_T new_node_ptr
);

static void
RULE_OM_LL_LocalInsertBeginning(
    RULE_TYPE_SHM_POINTER_T list_ptr,
    RULE_TYPE_SHM_POINTER_T new_node_ptr
);

static void
RULE_OM_LL_LocalInsertEnd(
    RULE_TYPE_SHM_POINTER_T list_ptr,
    RULE_TYPE_SHM_POINTER_T new_node_ptr
);

static void
RULE_OM_LL_LocalRemove(
    RULE_TYPE_SHM_POINTER_T list_ptr,
    RULE_TYPE_SHM_POINTER_T node_ptr
);

/* STATIC VARIABLE DECLARATIONS
 */
/* marvell uses profile to modify the value
   we may not need to maintain the reference lists.
 */
#if (RULE_CTRL_USE_REF_LIST == TRUE)
/* array index is IP DSCP vlaue, array value is a linked-list of rule info.
 * use this variable to record which FP rules reference this IP DSCP value.
 */
static RULE_OM_CosReferenceEntry_T*     dscp_reference_list[RULE_OM_MAX_NBR_OF_IP_DSCP_ENTRY];

/* array index is IP Precedence vlaue, array value is a linked-list of rule info.
 * use this variable to record which FP rules reference this IP Precedence value.
 */
static RULE_OM_CosReferenceEntry_T*     precedence_reference_list[RULE_OM_MAX_NBR_OF_IP_PRECEDENCE_ENTRY];
#endif /* #if (RULE_CTRL_USE_REF_LIST == TRUE) */

static UI32_T                rule_debug_flag;
static UI32_T                rule_om_sem_id;

static RULE_OM_ShmemData_T   *shmem_data_p;


/*------------------------------------------------------------------------------
 * FUNCTION NAME: RULE_OM_AttachSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE: Attach system resource for RULE_OM in the context of the calling
 *          process.
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
void RULE_OM_AttachSystemResources(void)
{
    shmem_data_p = (RULE_OM_ShmemData_T*)SYSRSC_MGR_GetShMem(SYSRSC_MGR_RULE_OM_SHMEM_SEGID);
    SYSFUN_GetSem(SYS_BLD_SYS_SEMAPHORE_KEY_RULE_OM, &rule_om_sem_id);
}

/* FUNCTION NAME: RULE_OM_GetShMemInfo
 *------------------------------------------------------------------------------
 * PURPOSE: Get share memory info
 *------------------------------------------------------------------------------
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 *------------------------------------------------------------------------------
 * NOTES:
 */
void RULE_OM_GetShMemInfo(SYSRSC_MGR_SEGID_T *segid_p, UI32_T *seglen_p)
{
    *segid_p = SYSRSC_MGR_RULE_OM_SHMEM_SEGID;
    *seglen_p = sizeof(RULE_OM_ShmemData_T);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_InitiateSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE  : Initial resource
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_OM_InitiateSystemResources()
{
    shmem_data_p = (RULE_OM_ShmemData_T *)SYSRSC_MGR_GetShMem(SYSRSC_MGR_RULE_OM_SHMEM_SEGID);
    SYSFUN_INITIATE_CSC_ON_SHMEM(shmem_data_p);

    return RULE_OM_Initialize();
}

/* EXPORTED SUBPROGRAM BODIES */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Initialize
 *------------------------------------------------------------------------------
 * PURPOSE:  initialize om
 * INPUT:    none
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_OM_Initialize()
{
    RULE_OM_EnterCriticalSection();

    memset(shmem_data_p, 0, sizeof(*shmem_data_p));

    shmem_data_p->rule_om_had_been_initialized = FALSE;

#if (RULE_CTRL_USE_REF_LIST == TRUE)
    if (TRUE == shmem_data_p->rule_om_had_been_initialized)
    {
        UI32_T                          index;
        RULE_OM_CosReferenceEntry_T     *entry_iter_p, *next_entry_p;

        for (index = 0; RULE_OM_MAX_NBR_OF_IP_DSCP_ENTRY > index; index++)
        {
            for (entry_iter_p = dscp_reference_list[index]; NULL != entry_iter_p; entry_iter_p = next_entry_p)
            {
                next_entry_p = entry_iter_p->next_entry;
                L_MM_Free(entry_iter_p);
            }
        }

        for (index = 0; RULE_OM_MAX_NBR_OF_IP_PRECEDENCE_ENTRY > index; index++)
        {
            for (entry_iter_p = precedence_reference_list[index]; NULL != entry_iter_p; entry_iter_p = next_entry_p)
            {
                next_entry_p = entry_iter_p->next_entry;
                L_MM_Free(entry_iter_p);
            }
        }
    }

    memset(dscp_reference_list, 0, sizeof(dscp_reference_list));
    memset(precedence_reference_list, 0, sizeof(precedence_reference_list));
#endif /* #if (RULE_CTRL_USE_REF_LIST == TRUE) */

    shmem_data_p->rule_om_had_been_initialized = TRUE;

    RULE_OM_LeaveCriticalSectionReturnState(TRUE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_HandleHotRemoval
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will clear the port OM of the module ports when
 *            the option module is removed.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     removed
 *            number_of_port        -- the number of ports on the removed
 *                                     module
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : Only one module is removed at a time.
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_OM_HandleHotRemoval(
    UI32_T starting_port_ifindex,
    UI32_T number_of_port)
{
    RULE_TYPE_PortEntry_T       *port_entry_p;
    UI32_T                      ifindex;
    UI32_T                      i;
    UI32_T                      end_port_ifindex;
    UI32_T                      starting_unit;
    UI32_T                      end_unit;
    RULE_TYPE_InOutDirection_T  direction;

    for (ifindex = starting_port_ifindex; ifindex < starting_port_ifindex + number_of_port; ifindex++)
    {
        for (direction = RULE_TYPE_INBOUND; direction <= RULE_TYPE_OUTBOUND; direction++)
        {
            port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, direction);
            if (NULL == port_entry_p)
            {
                continue;
            }

            // FIXME: Remove instance_ptr
            port_entry_p->policy_map_instance_ptr = NULL;

            memset(port_entry_p->bind_acl, 0, sizeof(port_entry_p->bind_acl));
            port_entry_p->policy_map_index = 0;
            RULE_OM_SET_ENTRY_DESTROY(port_entry_p->policy_map_row_status);

            for (i = 0; i < _countof(shmem_data_p->acl_table); i++)
            {
                RULE_OM_SET_BIT_OFF(shmem_data_p->acl_table[i].filtering_in_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex));
                RULE_OM_SET_BIT_OFF(shmem_data_p->acl_table[i].filtering_out_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex));
            }

            for (i = 0; i < SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP; i++)
            {
                if (RULE_OM_IS_BIT_ON(shmem_data_p->policy_map_table[i].qos_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex)))
                {
                    RULE_OM_SET_BIT_OFF(shmem_data_p->policy_map_table[i].qos_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex));
                    (shmem_data_p->policy_map_table[i].port_count)--;
                }
            }
        }
    }

    /* clean filtering_acl_instance_ref_count of acl
     */
    end_port_ifindex = starting_port_ifindex + (number_of_port-1);

    starting_unit = STKTPLG_OM_IFINDEX_TO_UNIT(starting_port_ifindex);
    end_unit = STKTPLG_OM_IFINDEX_TO_UNIT(end_port_ifindex);

    for (i = 0; i < _countof(shmem_data_p->acl_table); ++ i)
    {
        UI32_T unit, device_id;

        for (unit = starting_unit; unit <= end_unit; ++ unit)
        {
            for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; device_id++)
            {
                shmem_data_p->acl_table[i].filtering_acl_instance_ref_count[unit-1][device_id] = 0;
            }
        }
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_BindPort2PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to policy map.
 * INPUT:    uport_ifindex, policy_map_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_BindPort2PolicyMap(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T policy_map_index)
{
    RULE_TYPE_PortEntry_T   *port_entry_p;
    RULE_TYPE_PolicyMap_T   *policy_map_p;
    UI8_T                   *qos_port_list;

    RULE_OM_EnterCriticalSection();

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, inout_profile);
    if (NULL == port_entry_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad ifindex */
    }
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */
    }
    port_entry_p->policy_map_index = policy_map_index;
    RULE_OM_SET_ENTRY_ACTIVE(port_entry_p->policy_map_row_status);

    qos_port_list = (RULE_TYPE_INBOUND == inout_profile) ? &policy_map_p->qos_port_list[0] :
                                                           &policy_map_p->egress_qos_port_list[0];

    if (RULE_OM_IS_BIT_OFF(qos_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex)))
    {
        (policy_map_p->port_count)++;
        RULE_OM_SET_BIT_ON(qos_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex));
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_UnBindPort2PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Unbind a port from policy map.
 * INPUT:    uport_ifindex, policy_map_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     port_entry_p->policy_map_row_status will be set to notReady
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_UnBindPort2PolicyMap(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T policy_map_index)
{
    RULE_TYPE_PortEntry_T   *port_entry_p;
    RULE_TYPE_PolicyMap_T   *policy_map_p;
    UI8_T                   *qos_port_list;

    RULE_OM_EnterCriticalSection();

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, inout_profile);
    if (NULL == port_entry_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad uport_ifindex */
    }
    if (port_entry_p->policy_map_index != policy_map_index)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* this port didn't bind to specified policy-map */
    }

    if (TRUE != RULE_OM_IS_ENTRY_ACTIVE(port_entry_p->policy_map_row_status))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */
    }

    RULE_OM_SET_ENTRY_NOTREADY(port_entry_p->policy_map_row_status);

#if 1/*marvell only*/
    RULE_OM_LocalSetPolicyMapInstance(ifindex, inout_profile, NULL);
#endif

    qos_port_list = (RULE_TYPE_INBOUND == inout_profile) ? &policy_map_p->qos_port_list[0] :
                                                           &policy_map_p->egress_qos_port_list[0];

    if (RULE_OM_IS_BIT_ON(qos_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex)))
    {
        (policy_map_p->port_count)--;
        RULE_OM_SET_BIT_OFF(qos_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex));
    }

    if (0 == policy_map_p->port_count) /* no more port bind to this policy-map */
    {
        UI32_T                      element_cnt;
        RULE_TYPE_PolicyElement_T   *policy_element_p;

        /* the unbind sequence is:
         *  1, chip config
         *  2, om setting
         * therefore, CoS reference can't remove in RULE_MGR_LocalRemoveChipPolicyMapConfig()
         */
        for (element_cnt = 0; element_cnt < policy_map_p->element_count; element_cnt++)
        {
            policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_map_p->policy_map_element_index_list[element_cnt]);
            if (NULL == policy_element_p)
            {
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */
            }

            RULE_OM_LocalRemoveCosReference(policy_map_index, policy_element_p->class_map_index);
        }
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_IsBindDynamicPolicyMapOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Check the specified port whether bind a dynamic policy map.
 * INPUT:    uport_ifindex
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_OM_IsBindDynamicPolicyMapOnPort(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile)
{
#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
    RULE_TYPE_PortEntry_T   *port_entry_p;
    BOOL_T                  ret;

    RULE_OM_EnterCriticalSection();
    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, inout_profile);

    if (NULL == port_entry_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(FALSE);
    }

    ret = port_entry_p->bind_dynamic_policy_map;
    RULE_OM_LeaveCriticalSectionReturnState(ret);
#else
    return FALSE;
#endif /* SYS_CPNT_NETACCESS_DYNAMIC_QOS */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetBindDynamicPolicyMapStatusOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Set bind bynamic policy map status.
 * INPUT:    uport_ifindex, status
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_OM_SetBindDynamicPolicyMapStatusOnPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    BOOL_T on_off)
{
#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
    RULE_TYPE_PortEntry_T   *port_entry_p;

    RULE_OM_EnterCriticalSection();
    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, inout_profile);

    if (NULL == port_entry_p)
    {
        ASSERT(0);
        RULE_OM_LeaveCriticalSectionReturnState(FALSE);
    }

    port_entry_p->bind_dynamic_policy_map = on_off;
    RULE_OM_LeaveCriticalSectionReturnState(TRUE);
#else
    return FALSE;
#endif /* SYS_CPNT_NETACCESS_DYNAMIC_QOS */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetUserCfgPolicyMapIndexOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Set user configured policy map index.
 * INPUT:    uport_ifindex, policy_map_index
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_OM_SetUserCfgPolicyMapIndexOnPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T policy_map_index)
{
#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
    RULE_TYPE_PortEntry_T   *port_entry_p;

    RULE_OM_EnterCriticalSection();
    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, inout_profile);

    if (NULL == port_entry_p)
    {
        ASSERT(0);
        RULE_OM_LeaveCriticalSectionReturnState(FALSE);
    }

    port_entry_p->backup_policy_map_index = policy_map_index;

    if (policy_map_index == 0)
    {
        RULE_OM_SET_ENTRY_DESTROY(port_entry_p->backup_policy_map_row_status);
    }
    else
    {
        RULE_OM_SET_ENTRY_ACTIVE(port_entry_p->backup_policy_map_row_status);
    }

    RULE_OM_LeaveCriticalSectionReturnState(TRUE);
#else
    return FALSE;
#endif /* SYS_CPNT_NETACCESS_DYNAMIC_QOS */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetUserCfgPolicyMapNameOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get user configured policy map index.
 * INPUT:    uport_ifindex
 * OUTPUT:   NONE.
 * RETURN:   policy map index.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetUserCfgPolicyMapNameOnPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    char *policy_map_name)
{
#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
    RULE_TYPE_PortEntry_T   *port_entry_p;
    RULE_TYPE_PolicyMap_T   *policy_map_p;

    RULE_OM_EnterCriticalSection();
    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, inout_profile);

    if (NULL == port_entry_p)
    {
        ASSERT(0);
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    if (port_entry_p->backup_policy_map_index == 0)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(port_entry_p->backup_policy_map_index);
    if (NULL == policy_map_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    memcpy(policy_map_name, policy_map_p->name, SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
    policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0'; /* force to end a string */

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
#else
    return RULE_TYPE_FAIL;
#endif /* SYS_CPNT_NETACCESS_DYNAMIC_QOS */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetBindDynamicAclStatusOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Set binding bynamic acl status.
 * INPUT:    uport_ifindex, on_off
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
// FIXIT: Do we should support dynamic ACL on egress port ?
UI32_T RULE_OM_SetBindDynamicAclStatusOnPort(UI32_T uport_ifindex, BOOL_T on_off)
{
#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)

    RULE_TYPE_PortEntry_T   *port_entry_p;

    RULE_OM_EnterCriticalSection();
    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, RULE_TYPE_INBOUND);
    if (NULL == port_entry_p)
    {
        ASSERT(0);
        RULE_OM_LeaveCriticalSectionReturnState(FALSE);
    }

    port_entry_p->bind_dynamic_acl = on_off;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
#else
    return RULE_TYPE_FAIL;
#endif /* SYS_CPNT_NETACCESS_DYNAMIC_QOS */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_IsBindDyanmicAclOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Check the specified port whether bind a dynamic acl.
 * INPUT:    uport_ifindex
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_OM_IsBindDyanmicAclOnPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile)
{
#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)

    RULE_TYPE_PortEntry_T   *port_entry_p;
    BOOL_T                  ret;

    RULE_OM_EnterCriticalSection();
    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, inout_profile);
    if (NULL == port_entry_p)
    {
        ASSERT(0);
        RULE_OM_LeaveCriticalSectionReturnState(FALSE);
    }

    ret = port_entry_p->bind_dynamic_acl;

    RULE_OM_LeaveCriticalSectionReturnState(ret);
#else
    return FALSE;
#endif /* SYS_CPNT_NETACCESS_DYNAMIC_QOS */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetAclMementoOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Set user configured acl index.
 * INPUT:    uport_ifindex, acl_index, time_range_index
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_SetAclMementoOnPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    RULE_TYPE_AclType_T acl_type,
    const RULE_TYPE_AclMemento_T *acl_mem_p)
{
#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)

    RULE_TYPE_PortEntry_T   *port_entry_p;

    RULE_OM_EnterCriticalSection();
    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, inout_profile);
    if (NULL == port_entry_p)
    {
        ASSERT(0);
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    if (acl_mem_p)
    {
        memcpy(&port_entry_p->acl_memento[acl_type], acl_mem_p, sizeof(port_entry_p->acl_memento[acl_type]));
        RULE_OM_SET_ENTRY_ACTIVE(port_entry_p->acl_memento[acl_type].row_status);
    }
    else
    {
        memset(&port_entry_p->acl_memento[acl_type], 0, sizeof(port_entry_p->acl_memento[acl_type]));
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
#else
    return RULE_TYPE_FAIL;
#endif /* SYS_CPNT_NETACCESS_DYNAMIC_QOS */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAclMementoOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get user configured acl index that bind on the port.
 * INPUT:    uport_ifindex
 * OUTPUT:   NONE.
 * RETURN:   acl index.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetAclMementoOnPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_AclMemento_T *acl_mem_p)
{
#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
    RULE_TYPE_PortEntry_T   *port_entry_p;

    RULE_OM_EnterCriticalSection();
    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, inout_profile);
    if (NULL == port_entry_p)
    {
        ASSERT(0);
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    memcpy(acl_mem_p, &port_entry_p->acl_memento[acl_type], sizeof(RULE_TYPE_AclMemento_T));

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
#else
    acl_mem_p->acl_index = 0;
    return RULE_TYPE_OK;
#endif /* SYS_CPNT_NETACCESS_DYNAMIC_QOS */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetDynamicBindPortListByAclName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get the port list that bind the specified acl name with
 *           dynamic type.
 * INPUT:    acl_name
 * OUTPUT:   port_list, port_nbr
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetDynamicBindPortListByAclName(
    const char* acl_name,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI8_T port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST],
    UI32_T *port_nbr)
{
    UI32_T          acl_index;
    UI32_T          port_cnt;
    RULE_TYPE_Acl_T *acl_entry_p;

    if (RULE_TYPE_OK != RULE_OM_LocalGetAclIdByName(acl_name, &acl_index))
    {
        return RULE_TYPE_FAIL;
    }

    acl_entry_p = RULE_OM_LocalGetAclByIndex(acl_index);

    if ((NULL == port_list) || (NULL == port_nbr) || (NULL == acl_entry_p))
    {
        return RULE_TYPE_FAIL;
    }

    if (RULE_TYPE_INBOUND == inout_profile)
    {
        memcpy(port_list, acl_entry_p->filtering_in_port_list, RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST);
    }
    else
    {
        memcpy(port_list, acl_entry_p->filtering_out_port_list, RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST);
    }

    for (port_cnt = 0; port_cnt < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ port_cnt)
    {
        if (RULE_OM_IS_BIT_OFF(port_list, port_cnt))
        {
            continue;
        }

        if (TRUE == RULE_OM_IsBindDyanmicAclOnPort(RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(port_cnt),
                                                   inout_profile))
        {
            continue;
        }

        RULE_OM_SET_BIT_OFF(port_list, port_cnt);
    }


    *port_nbr = 0;
    for (port_cnt = 0; port_cnt < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ port_cnt)
    {
        if (RULE_OM_IS_BIT_ON(port_list, port_cnt))
        {
            ++(*port_nbr);
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetDynamicBindPortListByPolicyMapName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get the port list that bind the specified policy map name with
 *           dynamic type.
 * INPUT:    acl_name
 * OUTPUT:   port_list, port_nbr
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetDynamicBindPortListByPolicyMapName(
    const char* policy_map_name,
    UI8_T port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST],
    UI32_T *port_nbr)
{
    UI32_T                  policy_map_index;
    UI32_T                  port_cnt;
    RULE_TYPE_PolicyMap_T   *policy_map_entry_p;

    if (RULE_TYPE_OK != RULE_OM_GetPolicyMapIdByName(policy_map_name, &policy_map_index))
    {
        return RULE_TYPE_FAIL;
    }

    policy_map_entry_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);

    if ((NULL == port_list) || (NULL == port_nbr) || (NULL == policy_map_entry_p))
    {
        return RULE_TYPE_FAIL;
    }

    //
    // FIXME: Fix to support egress
    //
    memcpy(port_list, policy_map_entry_p->qos_port_list, RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST);

    for (port_cnt = 0; port_cnt < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ port_cnt)
    {
        if (RULE_OM_IS_BIT_OFF(port_list, port_cnt))
        {
            continue;
        }

        //
        // FIXME: Do we should support dynamic ACL on egress port ?
        //
        if (TRUE == RULE_OM_IsBindDynamicPolicyMapOnPort(RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(port_cnt),
                                                         RULE_TYPE_INBOUND))
        {
            continue;
        }

        RULE_OM_SET_BIT_OFF(port_list, port_cnt);
    }


    *port_nbr = 0;
    for (port_cnt = 0; port_cnt < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ port_cnt)
    {
        if (RULE_OM_IS_BIT_ON(port_list, port_cnt))
        {
            ++(*port_nbr);
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPortEntryAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Set acl index on bind acl in port entry
 * INPUT:    ifindex, direction, acl_type
 *           acl_index
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     For mib using, it only could config when the
 *           bind_acl status is notReady.
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_SetPortEntryAclIndex(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_AclType_T acl_type,
    UI32_T acl_index)
{
    RULE_TYPE_PortEntry_T *port_entry_p;
    RULE_TYPE_AclMemento_T *bind_acl_p;

    RULE_OM_EnterCriticalSection();

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, direction);
    if (NULL == port_entry_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    ASSERT(acl_type < _countof(port_entry_p->bind_acl));

    bind_acl_p = &port_entry_p->bind_acl[acl_type];

#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
    if (TRUE == port_entry_p->bind_dynamic_acl)
    {
        bind_acl_p = &port_entry_p->acl_memento[acl_type];
    }
#endif /* (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE) */

    if (RULE_OM_IS_ENTRY_DESTROY(bind_acl_p->row_status))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    ASSERT(FALSE == RULE_OM_IS_ENTRY_ACTIVE(bind_acl_p->row_status));

    bind_acl_p->acl_index = acl_index;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPortEntryAclTimeRange
 *------------------------------------------------------------------------------
 * PURPOSE:  Set time_range_index on bind acl in port entry
 * INPUT:    ifindex, direction, acl_type
 *           time_range_index
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     For mib using, it only could config when the
 *           bind_acl status is notReady.
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_SetPortEntryAclTimeRange(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_AclType_T acl_type,
    UI8_T *time_range_name,
    UI32_T time_range_index)
{
    RULE_TYPE_PortEntry_T *port_entry_p;
    RULE_TYPE_AclMemento_T *bind_acl_p;

    RULE_OM_EnterCriticalSection();

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, direction);
    if (NULL == port_entry_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    ASSERT(acl_type < _countof(port_entry_p->bind_acl));

#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
    if (TRUE == port_entry_p->bind_dynamic_acl)
    {
        bind_acl_p = &port_entry_p->acl_memento[acl_type];
    }
#endif /* (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE) */

    bind_acl_p = &port_entry_p->bind_acl[acl_type];

    if (RULE_OM_IS_ENTRY_DESTROY(bind_acl_p->row_status))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    ASSERT(FALSE == RULE_OM_IS_ENTRY_ACTIVE(bind_acl_p->row_status));

    if (time_range_name != NULL)
    {
        strncpy(bind_acl_p->time_range_name, time_range_name, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
        bind_acl_p->time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH] = '\0';
    }

    bind_acl_p->time_range_index = time_range_index;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPortEntryAclCounterEnable
 *------------------------------------------------------------------------------
 * PURPOSE:  Set counter_enable on bind acl in port entry
 * INPUT:    ifindex, direction, acl_type
 *           time_range_index
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     For mib using, it only could config when the
 *           bind_acl status is notReady.
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_SetPortEntryAclCounterEnable(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable)
{
    RULE_TYPE_PortEntry_T *port_entry_p;
    RULE_TYPE_AclMemento_T *bind_acl_p;

    RULE_OM_EnterCriticalSection();

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, direction);
    if (NULL == port_entry_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    ASSERT(acl_type < _countof(port_entry_p->bind_acl));

    bind_acl_p = &port_entry_p->bind_acl[acl_type];

#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
    if (TRUE == port_entry_p->bind_dynamic_acl)
    {
        bind_acl_p = &port_entry_p->acl_memento[acl_type];
    }
#endif /* (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE) */

    if (RULE_OM_IS_ENTRY_DESTROY(bind_acl_p->row_status))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    ASSERT(FALSE == RULE_OM_IS_ENTRY_ACTIVE(bind_acl_p->row_status));

    bind_acl_p->counter_enable = counter_enable;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPortEntryAclRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  Set acl index on bind acl in port entry
 * INPUT:    ifindex, direction, acl_type
 *           time_range_index
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     For mib using, it only could config when the
 *           bind_acl status is notReady.
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_SetPortEntryAclRowStatus(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_AclType_T acl_type,
    UI32_T mib_row_status)
{
    RULE_TYPE_PortEntry_T *port_entry_p;
    RULE_TYPE_AclMemento_T *bind_acl_p;

    RULE_OM_EnterCriticalSection();

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, direction);
    if (NULL == port_entry_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    ASSERT(acl_type < _countof(port_entry_p->bind_acl));

    bind_acl_p = &port_entry_p->bind_acl[acl_type];

#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
    if (TRUE == port_entry_p->bind_dynamic_acl)
    {
        bind_acl_p = &port_entry_p->acl_memento[acl_type];
    }
#endif /* (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE) */

    if (RULE_OM_IS_ENTRY_DESTROY(bind_acl_p->row_status))
    {
        bind_acl_p->acl_index = 0;
        bind_acl_p->time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
        bind_acl_p->counter_enable = RULE_TYPE_COUNTER_DISABLE;
    }

    switch (mib_row_status)
    {
        case VAL_diffServAccessGroupStatus_destroy:
            ASSERT(TRUE != RULE_OM_IS_ENTRY_ACTIVE(bind_acl_p->row_status));
            RULE_OM_SET_ENTRY_DESTROY(bind_acl_p->row_status);
            bind_acl_p->acl_index = 0;
            bind_acl_p->time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
            bind_acl_p->counter_enable = RULE_TYPE_COUNTER_DISABLE;
            break;

        case VAL_diffServAccessGroupStatus_notReady:
            RULE_OM_SET_ENTRY_NOTREADY(bind_acl_p->row_status);
            break;

        case VAL_diffServAccessGroupStatus_active:
            RULE_OM_SET_ENTRY_ACTIVE(bind_acl_p->row_status);
            break;

        default:
            ASSERT(0);
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPortBindedAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get bind acl on port entry
 * INPUT:    ifindex, direction, acl_type
 * OUTPUT:   bind_acl_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Only get success when bind_acl.row_status is avtiev/notReady
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_GetPortBindedAcl(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_AclMemento_T *bind_acl_p)
{
    RULE_TYPE_PortEntry_T *port_entry_p;
    RULE_TYPE_AclMemento_T *local_bind_acl_p;

    RULE_OM_EnterCriticalSection();

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, direction);
    if (NULL == port_entry_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    ASSERT(acl_type < _countof(port_entry_p->bind_acl));

    local_bind_acl_p = &port_entry_p->bind_acl[acl_type];

#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
    if (TRUE == port_entry_p->bind_dynamic_acl)
    {
        local_bind_acl_p = &port_entry_p->acl_memento[acl_type];
    }
#endif /* (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE) */

    if (RULE_OM_IS_ENTRY_DESTROY(local_bind_acl_p->row_status))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    ASSERT(TRUE == RULE_OM_IS_ENTRY_AVAILABLE(local_bind_acl_p->row_status));

    memcpy(bind_acl_p, local_bind_acl_p, sizeof(*bind_acl_p));

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPortEntryPolicyMapIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Set policy_index on port entry
 * INPUT:    ifindex, direction
 *           policy_index
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     For mib using, it only could config when the
 *           policy_map_status is notReady.
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_SetPortEntryPolicyMapIndex(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_index)
{
    RULE_TYPE_PortEntry_T *port_entry_p;

    RULE_OM_EnterCriticalSection();

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, direction);
    if (NULL == port_entry_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
    if (TRUE == port_entry_p->bind_dynamic_policy_map)
    {
        if (RULE_OM_IS_ENTRY_DESTROY(port_entry_p->backup_policy_map_row_status))
        {
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
        }

        ASSERT(FALSE == RULE_OM_IS_ENTRY_ACTIVE(port_entry_p->backup_policy_map_row_status));

        port_entry_p->backup_policy_map_index = policy_index;

        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }
#endif /* (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE) */

    if (RULE_OM_IS_ENTRY_DESTROY(port_entry_p->policy_map_row_status))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    ASSERT(FALSE == RULE_OM_IS_ENTRY_ACTIVE(port_entry_p->policy_map_row_status));

    port_entry_p->policy_map_index = policy_index;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPortEntryPolicyMapRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  Set policy_index on port entry
 * INPUT:    ifindex, direction
 *           policy_index
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     For mib using, only coulde set destroy/notReady.
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_SetPortEntryPolicyMapRowStatus(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T mib_row_status)
{
    RULE_TYPE_PortEntry_T *port_entry_p;
    UI32_T                *policy_map_row_status_p;
    UI16_T                *policy_map_index_p;

    RULE_OM_EnterCriticalSection();

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, direction);
    if (NULL == port_entry_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    policy_map_row_status_p = &port_entry_p->policy_map_row_status;
    policy_map_index_p = &port_entry_p->policy_map_index;

#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
    if (TRUE == port_entry_p->bind_dynamic_policy_map)
    {
        policy_map_row_status_p = &port_entry_p->backup_policy_map_row_status;
        policy_map_index_p = &port_entry_p->backup_policy_map_index;
    }
#endif /* (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE) */

    if (RULE_OM_IS_ENTRY_DESTROY(*policy_map_row_status_p))
    {
        *policy_map_index_p = 0;
    }

    switch (mib_row_status)
    {
        case VAL_diffServPolicyMapPortPortStatus_destroy:
            ASSERT(TRUE != RULE_OM_IS_ENTRY_ACTIVE(*policy_map_row_status_p));
            RULE_OM_SET_ENTRY_DESTROY(*policy_map_row_status_p);
            *policy_map_index_p = 0;
            break;

        case VAL_diffServPolicyMapPortPortStatus_notReady:
            RULE_OM_SET_ENTRY_NOTREADY(*policy_map_row_status_p);
            break;

        case VAL_diffServPolicyMapPortPortStatus_active:
            RULE_OM_SET_ENTRY_ACTIVE(*policy_map_row_status_p);
            break;

        default:
            ASSERT(0);
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPortBindedPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get policy_map_index, policy_map_status on port entry
 * INPUT:    ifindex, direction
 * OUTPUT:   policy_map_index, policy_map_status
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Only get success when policy_map_row_status is avtiev/notReady
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_GetPortBindedPolicyMap(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T *policy_map_index_p,
    UI32_T *policy_map_row_status_p)
{
    RULE_TYPE_PortEntry_T *port_entry_p;
    UI32_T                *local_policy_map_row_status_p;
    UI16_T                *local_policy_map_index_p;

    ASSERT(policy_map_index_p != NULL);
    ASSERT(policy_map_row_status_p != NULL);

    RULE_OM_EnterCriticalSection();

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, direction);
    if (NULL == port_entry_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    local_policy_map_row_status_p = &port_entry_p->policy_map_row_status;
    local_policy_map_index_p = &port_entry_p->policy_map_index;

#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
    if (TRUE == port_entry_p->bind_dynamic_policy_map)
    {
        local_policy_map_row_status_p = &port_entry_p->backup_policy_map_row_status;
        local_policy_map_index_p = &port_entry_p->backup_policy_map_index;
    }
#endif /* (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE) */

    if (RULE_OM_IS_ENTRY_DESTROY(*local_policy_map_row_status_p))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    ASSERT(TRUE == RULE_OM_IS_ENTRY_AVAILABLE(*local_policy_map_row_status_p));

    *policy_map_index_p = *local_policy_map_index_p;
    *policy_map_row_status_p = *local_policy_map_row_status_p;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Create_PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Create Policy_map and return index of this name
 * INPUT:    policy_map_name
 * OUTPUT:   policy_map_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Create_PolicyMap(const char *policy_map_name, UI32_T *policy_map_index)
{
    UI32_T                  index, name_len;
    RULE_TYPE_PolicyMap_T   *policy_map_p;

    if ((NULL == policy_map_name) || (NULL == policy_map_index))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    /* check parameters */
    name_len = strlen(policy_map_name);
    if ((0 >= name_len) || (SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH < name_len))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (NULL != strpbrk(policy_map_name, RULE_TYPE_INVALIDATE_CHAR_OF_DS_NAME))
    {
        return RULE_TYPE_E_INVALID_CHARACTER;
    }

    /* When users issue CLI command: policy-map "name", CLI always invoke this API indirectly.
     * Therefore, return EXISTED, not FAIL to create.
     */

    RULE_OM_EnterCriticalSection();

    if (RULE_TYPE_OK == RULE_OM_LocalIsExistPolicyMapName(policy_map_name))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_POLICY_MAP_EXISTED);
    }

    for (index = 0; SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP > index; ++index)
    {
        policy_map_p = &shmem_data_p->policy_map_table[index];

        if (RULE_OM_IS_ENTRY_DESTROY(policy_map_p->row_status))
        {
            RULE_OM_SET_ENTRY_ACTIVE(policy_map_p->row_status);
            strncpy((char *)policy_map_p->name, (char *)policy_map_name, SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
            policy_map_p->name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0'; /* force end a string */

            *policy_map_index = index + 1; /* to 1-based index */
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
        }
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_CreatePolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  create policy_map on specified index
 * INPUT:    policy_map_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_CreatePolicyMap(UI32_T policy_map_index, BOOL_T is_active)
{
#define MAX_UI32_STR_LEN        10  /* 0xffffffff */

    RULE_TYPE_PolicyMap_T       *policy_map_p;
    char                        buf[MAX_UI32_STR_LEN + 1] = {0};

    if (RULE_OM_IS_BAD_POLICY_MAP_IDX(policy_map_index))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    RULE_OM_EnterCriticalSection();
    policy_map_p = &shmem_data_p->policy_map_table[policy_map_index - 1]; /* to zero-based array index */
    if (RULE_OM_IS_ENTRY_AVAILABLE(policy_map_p->row_status))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* already created */

    memset(policy_map_p, 0, sizeof(RULE_TYPE_PolicyMap_T));

    sprintf(buf, "%lu", policy_map_index);
    strncpy(policy_map_p->name, buf, sizeof(policy_map_p->name)-1);
    policy_map_p->name[ sizeof(policy_map_p->name)-1 ] = '\0';

    if (RULE_TYPE_OK == RULE_OM_LocalIsExistPolicyMapName(policy_map_p->name))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_POLICY_MAP_EXISTED);
    }

    if (TRUE == is_active)
    {
        RULE_OM_SET_ENTRY_ACTIVE(policy_map_p->row_status);
    }
    else
    {
        RULE_OM_SET_ENTRY_NOTREADY(policy_map_p->row_status);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);

#undef MAX_UI32_STR_LEN
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Delete_PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete Policy_map by index
 * INPUT:    policy_map_index
 * OUTPUT:   none.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Delete_PolicyMap(UI32_T policy_map_index)
{
    UI32_T                      index;
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    if (RULE_OM_IS_BAD_POLICY_MAP_IDX(policy_map_index))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    RULE_OM_EnterCriticalSection();
    policy_map_p = &shmem_data_p->policy_map_table[policy_map_index - 1]; /* to zero-based array index */

    /* Release policy-map element and other resources */
    for (index =0; index <policy_map_p->element_count; index++)
    {
        policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_map_p->policy_map_element_index_list[index]);
        if (NULL == policy_element_p)
        {
            /* should not go here */
            continue;
        }

        RULE_OM_LocalFreeMeter(policy_element_p->meter_index);
        RULE_OM_LocalFreeAction(policy_element_p->action_index);

        /* reset policy element */
        policy_element_p->meter_index = 0;
        policy_element_p->action_index = 0;
        policy_element_p->class_map_index = 0;
        RULE_OM_SET_ENTRY_DESTROY(policy_element_p->row_status);
    }

    /* Relase policy-map */
    memset(policy_map_p, 0, sizeof(RULE_TYPE_PolicyMap_T));
    RULE_OM_SET_ENTRY_DESTROY(policy_map_p->row_status);

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMap(UI32_T policy_map_index, RULE_TYPE_PolicyMap_T *policy_map)
{
    RULE_TYPE_PolicyMap_T       *policy_map_p;

    if (NULL == policy_map)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    memcpy(policy_map, policy_map_p, sizeof(RULE_TYPE_PolicyMap_T));
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetMibPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a mib policy map entry by its index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetMibPolicyMap(UI32_T policy_map_index, RULE_TYPE_MIB_PolicyMap_T *policy_map)
{
    RULE_TYPE_PolicyMap_T   *policy_map_p;
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    if (NULL == policy_map)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    ret_code = RULE_OM_LocalConvert2MibPolicyMap(policy_map_p, policy_map);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Use *policy_map_index=0 to get first
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextPolicyMap(UI32_T *policy_map_index, RULE_TYPE_PolicyMap_T *policy_map)
{
    RULE_TYPE_PolicyMap_T   *policy_map_p;
    UI32_T                  next_policy_map_index;

    if ((NULL == policy_map_index) || (NULL == policy_map))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    RULE_OM_EnterCriticalSection();
    for (next_policy_map_index = *policy_map_index + 1;
        SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP >= next_policy_map_index; ++next_policy_map_index)
    {
        policy_map_p = &shmem_data_p->policy_map_table[next_policy_map_index - 1]; /* to zero-based array index */

        if (RULE_OM_IS_ENTRY_DESTROY(policy_map_p->row_status))
            continue;

        *policy_map_index = next_policy_map_index;
        memcpy(policy_map, policy_map_p, sizeof(RULE_TYPE_PolicyMap_T));
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextMibPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next mib policy map entry by its index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Use *policy_map_index=0 to get first
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextMibPolicyMap(UI32_T *policy_map_index, RULE_TYPE_MIB_PolicyMap_T *policy_map)
{
    RULE_TYPE_PolicyMap_T   *policy_map_p;
    UI32_T                  next_policy_map_index;
    RULE_TYPE_RETURN_TYPE_T ret_code;

    if ((NULL == policy_map_index) || (NULL == policy_map))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    RULE_OM_EnterCriticalSection();
    for (next_policy_map_index = *policy_map_index + 1;
        SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP >= next_policy_map_index; ++next_policy_map_index)
    {
        policy_map_p = &shmem_data_p->policy_map_table[next_policy_map_index - 1]; /* to zero-based array index */

        if (RULE_OM_IS_ENTRY_DESTROY(policy_map_p->row_status))
            continue;

        *policy_map_index = next_policy_map_index;
        ret_code = RULE_OM_LocalConvert2MibPolicyMap(policy_map_p, policy_map);
        RULE_OM_LeaveCriticalSectionReturnState(ret_code);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUIPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Use *policy_map_index=0 to get first
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextUIPolicyMap(UI32_T *policy_map_index, RULE_TYPE_UI_PolicyMap_T *policy_map)
{
    RULE_TYPE_PolicyMap_T   *policy_map_p;
    UI32_T                  next_policy_map_index;
    RULE_TYPE_RETURN_TYPE_T ret_code;

    if ((NULL == policy_map_index) || (NULL == policy_map))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    RULE_OM_EnterCriticalSection();
    for (next_policy_map_index = *policy_map_index + 1;
        SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP >= next_policy_map_index; ++next_policy_map_index)
    {
        policy_map_p = &shmem_data_p->policy_map_table[next_policy_map_index - 1]; /* to zero-based array index */

        if (RULE_OM_IS_ENTRY_DESTROY(policy_map_p->row_status))
            continue;

        *policy_map_index = next_policy_map_index;
        ret_code = RULE_OM_LocalConvert2UIPolicyMap(policy_map_p, policy_map);
        RULE_OM_LeaveCriticalSectionReturnState(ret_code);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextPolicyMapByClassMapIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next policy map entry by class map index
 * INPUT:    class_map_index    - class map index
 *           policy_map_index_p - policy map index. pass 0 to start
 * OUTPUT:   policy_map_index_p - policy map index.
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextPolicyMapByClassMapIndex(
    UI32_T class_map_index,
    UI32_T *policy_map_index_p)
{
    RULE_TYPE_PolicyMap_T   *policy_map_p;
    UI32_T                  ret;

    RULE_OM_EnterCriticalSection();

    policy_map_p = RULE_OM_LocalGetNextPolicyMapByClassMapIndex(class_map_index, policy_map_index_p);
    ret = policy_map_p ? RULE_TYPE_OK : RULE_TYPE_FAIL;

    RULE_OM_LeaveCriticalSectionReturnState(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  get policy map row status by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   row_status
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapRowStatus(UI32_T policy_map_index, UI32_T *row_status)
{
    RULE_TYPE_PolicyMap_T       *policy_map_p;

    if (NULL == row_status)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    *row_status = policy_map_p->row_status;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_BindElement2PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE  : bind policy-map element to policy-map
 * INPUT    : policy_map_index, policy_element_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_BindElement2PolicyMap(UI32_T policy_map_index, UI32_T policy_element_index)
{
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;
    UI32_T                      found_index;

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_element_index);
    if (NULL == policy_element_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (FALSE == RULE_OM_IS_ENTRY_ACTIVE(policy_element_p->row_status))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* do not allow to associate with non-active entry */

    if (RULE_TYPE_OK == RULE_OM_LocalGetPolicyMapIndexByElementIndex(policy_element_index, &found_index))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* this element is already own by some one */

    if (SYS_ADPT_DIFFSERV_MAX_CLASS_MAP_NBR_OF_POLICY_MAP <= policy_map_p->element_count)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* policy-map is full */

    policy_map_p->policy_map_element_index_list[policy_map_p->element_count] = policy_element_index;
    policy_map_p->element_count++;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_UnbindElementFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE  : unbind policy-map element from policy-map
 * INPUT    : policy_map_index, policy_element_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_UnbindElementFromPolicyMap(UI32_T policy_map_index, UI32_T policy_element_index)
{
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;
    UI32_T                      found_index, element_precedence;

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_element_index);
    if (NULL == policy_element_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (FALSE == RULE_OM_IS_ENTRY_ACTIVE(policy_element_p->row_status))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* do not allow to associate with non-active entry */

    if (RULE_TYPE_OK != RULE_OM_LocalGetPolicyMapIndexByElementIndex(policy_element_index, &found_index))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* this element doesn't be own by some one */

    if (RULE_TYPE_OK != RULE_OM_LocalGetPolicyMapElementPrecedence(policy_map_index, policy_element_p->class_map_index, &element_precedence))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (RULE_TYPE_OK != RULE_OM_LocalRemoveAndCompactPolicyMapElement(policy_map_p, element_precedence))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMapIndexByName
 *------------------------------------------------------------------------------
 * PURPOSE:  get class-map index by class map name
 * INPUT:    class_map_name
 * OUTPUT:   class_map_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMapIndexByName(const char *class_map_name, UI32_T *class_map_index)
{
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    RULE_OM_EnterCriticalSection();
    ret_code = RULE_OM_LocalGetClassMapIndexByName(class_map_name, class_map_index);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMapIndexByMf
 *------------------------------------------------------------------------------
 * PURPOSE  : get class-map index by mf index
 * INPUT    : mf_index
 * OUTPUT   : class_map_index
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMapIndexByMf(UI32_T mf_index, UI32_T *class_map_index)
{
    RULE_TYPE_RETURN_TYPE_T  ret_code;
    RULE_OM_EnterCriticalSection();
    ret_code = RULE_OM_LocalGetClassMapIndexByMf(mf_index, class_map_index);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMapIndexByMeterIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get class-map index by meter index
 * INPUT    : meter_idx
 * OUTPUT   : cmap_idx
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMapIndexByMeterIndex(UI32_T meter_idx, UI32_T *cmap_idx)
{
    UI32_T                      pmap_e_idx;
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    RULE_OM_EnterCriticalSection();

    if (RULE_TYPE_OK != RULE_OM_LocalGetPolicyMapElementIndexByMeter (meter_idx, &pmap_e_idx))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(pmap_e_idx);
    if (NULL != policy_element_p)
    {
        *cmap_idx = policy_element_p->class_map_index;
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Create_ClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Create Class_map and return index of this name
 * INPUT:    class_map_name     - class-map name
 *           match_type         - match type
 * OUTPUT:   class_map_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Create_ClassMap(
    const char *class_map_name,
    RULE_TYPE_MatchType_T match_type,
    UI32_T *class_map_index)
{
    UI32_T  index, name_len;

    if ((NULL == class_map_name) ||
        (match_type < RULE_TYPE_CLASS_MAP_MATCH_NONE) ||
        (RULE_TYPE_CLASS_MAP_MATCH_LAST < match_type) ||
        (NULL == class_map_index))
    {
        return RULE_TYPE_FAIL;
    }

    name_len = strlen(class_map_name);
    if ((0 >= name_len) || (SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH < name_len))
    {
        return RULE_TYPE_FAIL;
    }

    if (NULL != strpbrk(class_map_name, RULE_TYPE_INVALIDATE_CHAR_OF_DS_NAME))
    {
        return RULE_TYPE_E_INVALID_CHARACTER;
    }

#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL != TRUE)
    if (RULE_TYPE_CLASS_MAP_MATCH_ALL == match_type)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }
#endif /* SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL */

    RULE_OM_EnterCriticalSection();

    /* When users issue CLI command: class-map "name" in global configuration mode,
     * CLI always invoke this API indirectly. Therefore, return EXISTED, not FAIL to create.
     */
    if (RULE_TYPE_OK == RULE_OM_LocalGetClassMapIndexByName(class_map_name, class_map_index))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_CLASS_MAP_EXISTED);

    for (index = 0; SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS_MAP > index; ++index)
    {
        if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->class_map_table[index].row_status))
        {
            RULE_OM_SET_ENTRY_ACTIVE(shmem_data_p->class_map_table[index].row_status);

            strncpy((char *)shmem_data_p->class_map_table[index].class_map_name, (char *)class_map_name, SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
            shmem_data_p->class_map_table[index].class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0'; /* force end a string */

            shmem_data_p->class_map_table[index].class_map_match_type = match_type;
            *class_map_index = index + 1; /* to 1-based index */
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
        }
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
 }

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Delete_ClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete class-map by index
 * INPUT:    class_map_index
 * OUTPUT:   none.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Delete_ClassMap(UI32_T class_map_index)
{
    UI32_T                  index;
    RULE_TYPE_ClassMap_T    *class_map_p;
    RULE_TYPE_Acl_T         *acl_p;

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    /* cleanup class-map elements in class table */
    for (index = 0; class_map_p->class_element_count > index; ++index)
    {
        switch (class_map_p->class_type[index])
        {
        case RULE_TYPE_CLASS_MF:
            /* delete a multi-filter (ACE) from ACE table */
            RULE_OM_LocalFreeAceEntry(class_map_p->class_index_list[index]);
            break;

        case RULE_TYPE_CLASS_ACL:
            /* remove acl related information */
            acl_p = RULE_OM_LocalGetAclByIndex (class_map_p->class_index_list[index]);

            if (NULL != acl_p)
            {
                RULE_OM_SET_BIT_OFF(acl_p->class_map_bit_list, (class_map_index - 1));
                acl_p->class_map_count--;
            }
            break;

        default:
            /* should not occur */
            continue;
        }
    }

    /* re-initialize class-map entry */
    memset(class_map_p, 0, sizeof(RULE_TYPE_ClassMap_T));
    RULE_OM_SET_ENTRY_DESTROY(class_map_p->row_status);

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Get_PolicyMapListByClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get policy_map_list
 * INPUT:    class_map_index
 * OUTPUT:   policy_map_list.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     if no policy-map reference to this class-map, return FAIL
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Get_PolicyMapListByClassMap(UI32_T class_map_index, UI8_T *policymaplist, UI32_T *policymap_nbr)
{
    UI32_T                      i = 0, j = 0;
    UI32_T                      policymap_num = 0;
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    if ((NULL == policymaplist) || (NULL == policymap_nbr))
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    for (i = 0; i < SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP; i++)
    {
        policy_map_p = &shmem_data_p->policy_map_table[i];

        if (RULE_OM_IS_ENTRY_DESTROY(policy_map_p->row_status))
            continue;

        for (j = 0; j < policy_map_p->element_count; j++)
        {
            policy_element_p = &shmem_data_p->policy_map_element_table[policy_map_p->policy_map_element_index_list[j] - 1]; /* to zero-based */
            if (class_map_index == policy_element_p->class_map_index)
            {
                policymap_num++;
                RULE_OM_SET_BIT_ON(policymaplist, i);
            }
        }
    }
    RULE_OM_LeaveCriticalSection();

    if (0 == policymap_num)
    {
        /*do not have list*/
        return RULE_TYPE_FAIL;
    }
    else
    {
        *policymap_nbr = policymap_num;
        return RULE_TYPE_OK;
    }
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Get_PortListByPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get port_list by policy_map_list
 * INPUT:    policy_map_list
 * OUTPUT:   port_list, port_nbr
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     if no port bound to this policy-map, return FAIL
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_Get_PortListByPolicyMap(
    UI8_T *policymaplist,
    BOOL_T ingress_flag, // FIXME: change to direction
    UI8_T *port_list,
    UI32_T *port_nbr)
{
    UI32_T                  policy_cnt, port_cnt;
    RULE_TYPE_PolicyMap_T   *policy_map_p;

    if ((NULL == policymaplist) || (NULL == port_list) || (NULL == port_nbr))
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();

    for (policy_cnt = 0; SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP > policy_cnt; ++policy_cnt)
    {
        UI8_T  *qos_port_list;

        UI32_T max_byte_cnt;
        UI32_T byte_cnt;

        if (RULE_OM_IS_BIT_OFF(policymaplist, policy_cnt))
            continue;

        policy_map_p = &shmem_data_p->policy_map_table[policy_cnt];
        qos_port_list = ingress_flag ? policy_map_p->qos_port_list :
                                       policy_map_p->egress_qos_port_list;
        max_byte_cnt = ingress_flag ? _countof(policy_map_p->qos_port_list) :
                                      _countof(policy_map_p->egress_qos_port_list);

        for (byte_cnt = 0; byte_cnt < max_byte_cnt; ++ byte_cnt)
        {
            /* suppose port_list already been ZeroMemory before call this function */
            port_list[byte_cnt] |= qos_port_list[byte_cnt];
        }
    }

    RULE_OM_LeaveCriticalSection();

    *port_nbr = 0;
    for (port_cnt = 0; port_cnt < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ port_cnt)
    {
        if (RULE_OM_IS_BIT_ON(port_list, port_cnt))
        {
            ++(*port_nbr);
        }
    }

    if (0 == *port_nbr) /* no port bind to these policy-map */
        return RULE_TYPE_FAIL;

    return RULE_TYPE_OK;

}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextPort_BindPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE  : Enumerate all interface which have bound the specified policy-map.
 * INPUT    : policy_map_index      - policy-map index
 *            uport_ifindex_p       - uport ifindex; pass 0 to start.
 * OUTPUT   : uport_ifindex_p       - uport ifindex
 *            inout_profile_p       - direction
 * RETURN   : RULE_TYPE_OK if get an interface; else if error or no more interface
 * NOTES    : Sequence of the enumerating are
 *            uport_inindex -> inout_profile
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextPort_BindPolicyMap(
    UI32_T  policy_map_index,
    UI32_T  *uport_ifindex_p,
    RULE_TYPE_InOutDirection_T *inout_profile_p)
{
    UI32_T  res;

    if ((NULL == uport_ifindex_p) || (NULL == inout_profile_p))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    RULE_OM_EnterCriticalSection();

    res = RULE_OM_Local_GetNextPort_BindPolicyMap(policy_map_index,
                                                  uport_ifindex_p,
                                                  inout_profile_p);

    RULE_OM_LeaveCriticalSectionReturnState(res);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Get_PortListByClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : get port_list by class_map_index
 * INPUT    : class_map_index
 * OUTPUT   : port_list, port_nbr
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : return FAIL if there is no port bound to
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_Get_PortListByClassMap(
    UI32_T class_map_index,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI8_T *port_list,
    UI32_T *port_nbr)
{
    UI32_T                      policy_map_cnt, policy_element_cnt;
    UI32_T                      port_cnt;
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    if ((NULL == port_list) || (NULL == port_nbr))
        return RULE_TYPE_FAIL;

    memset(port_list, 0, RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST);

    RULE_OM_EnterCriticalSection();

    /* search every policy-map element */
    for (policy_map_cnt = 0; SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP > policy_map_cnt; ++policy_map_cnt)
    {
        policy_map_p = &shmem_data_p->policy_map_table[policy_map_cnt];

        if (RULE_OM_IS_ENTRY_DESTROY(policy_map_p->row_status))
            continue;

        for (policy_element_cnt = 0; policy_element_cnt < policy_map_p->element_count; ++policy_element_cnt)
        {
            UI8_T *qos_port_list;
            UI32_T max_byte;
            UI32_T byte_cnt;

            policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(
                                policy_map_p->policy_map_element_index_list[policy_element_cnt]);
            if (NULL == policy_element_p)
                continue; /* should not go here */

            if (class_map_index != policy_element_p->class_map_index)
                continue;

            qos_port_list =(RULE_TYPE_INBOUND == inout_profile) ? policy_map_p->qos_port_list :
                                                                  policy_map_p->egress_qos_port_list;

            max_byte = (RULE_TYPE_INBOUND == inout_profile) ? _countof(policy_map_p->qos_port_list) :
                                                              _countof(policy_map_p->egress_qos_port_list);

            for (byte_cnt = 0; byte_cnt < max_byte; ++byte_cnt)
            {
                port_list[byte_cnt] |= qos_port_list[byte_cnt];
            }

            break;
        }
    }

    RULE_OM_LeaveCriticalSection();

    /* count the port number in port list */
    *port_nbr = 0;
    for (port_cnt = 0; port_cnt < RULE_TYPE_TOTAL_NBR_OF_PORT; ++port_cnt)
    {
        if (RULE_OM_IS_BIT_ON(port_list, port_cnt))
        {
            ++(*port_nbr);
        }
    }

    if (0 == *port_nbr) /* no port bind to these policy-map */
        return RULE_TYPE_FAIL;

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Get_PortListByClassMapList
 *------------------------------------------------------------------------------
 * PURPOSE  : get port_list by class_map_list
 * INPUT    : class_map_list
 * OUTPUT   : port_list, port_nbr
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : return FAIL if there is no port bound to
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Get_PortListByClassMapList(const UI8_T *class_map_list, UI8_T *port_list, UI32_T *port_nbr)
{
    UI32_T                      policy_map_cnt, policy_element_cnt;
    UI32_T                      byte_cnt, port_cnt;
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    if ((NULL == class_map_list) || (NULL == port_list) || (NULL == port_nbr))
        return RULE_TYPE_FAIL;

//
// FIXME: No support egress
//
//    ASSERT(0);

    memset(port_list, 0, RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST);

    RULE_OM_EnterCriticalSection();

    /* search every policy-map element */
    for (policy_map_cnt = 0; SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP > policy_map_cnt; ++policy_map_cnt)
    {
        policy_map_p = &shmem_data_p->policy_map_table[policy_map_cnt];

        if (RULE_OM_IS_ENTRY_DESTROY(policy_map_p->row_status))
            continue;

        for (policy_element_cnt = 0; policy_element_cnt < policy_map_p->element_count; ++policy_element_cnt)
        {
            policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(
                                policy_map_p->policy_map_element_index_list[policy_element_cnt]);
            if (NULL == policy_element_p)
                continue; /* should not go here */

            if (RULE_OM_IS_BIT_OFF(class_map_list, (policy_element_p->class_map_index - 1)))
                continue;

            // FIXME: Should support egress here, or not to use this function
            for (byte_cnt = 0; byte_cnt < _countof(policy_map_p->qos_port_list); ++ byte_cnt)
            {
                port_list[byte_cnt] |= policy_map_p->qos_port_list[byte_cnt];
            }

            break;
        }
    }

    RULE_OM_LeaveCriticalSection();

    /* count the port number in port list */
    *port_nbr = 0;
    for (port_cnt = 0; port_cnt < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ port_cnt)
    {
        if (RULE_OM_IS_BIT_ON(port_list, port_cnt))
        {
            ++(*port_nbr);
        }
    }

    if (0 == *port_nbr) /* no port bind to these policy-map */
        return RULE_TYPE_FAIL;

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextPort_BindClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : Enumerate all interface which have bound the specified class-map.
 * INPUT    : class_map_index       - class-map index
 *            uport_ifindex_p       - uport ifindex; pass 0 to start.
 * OUTPUT   : uport_ifindex_p       - uport ifindex
 *            inout_profile_p       - direction
 *            policy_map_index_p    - policy-map index
 * RETURN   : RULE_TYPE_OK if get an interface; else if error or no more interface
 * NOTES    : Sequence of the enumerating are
 *            uport_inindex -> inout_profile -> policy_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextPort_BindClassMap(
    UI32_T  class_map_index,
    UI32_T  *uport_ifindex_p,
    RULE_TYPE_InOutDirection_T  *inout_profile_p,
    UI32_T  *policy_map_index_p)
{
    UI32_T res;

    if (NULL == policy_map_index_p ||
        NULL == uport_ifindex_p ||
        NULL == inout_profile_p)
    {
        return FALSE;
    }

    RULE_OM_EnterCriticalSection();

    res = RULE_OM_Local_GetNextPort_BindClassMap(class_map_index,
                                                 uport_ifindex_p,
                                                 inout_profile_p,
                                                 policy_map_index_p);

    RULE_OM_LeaveCriticalSectionReturnState(res);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  Get class map element by element index
 * INPUT:    class_map_index, element_index
 * OUTPUT:   element_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMapElement(UI32_T class_map_index, UI32_T element_index, RULE_TYPE_UI_ClassMapElement_T *element_entry)
{
    RULE_TYPE_ClassMap_T            *class_map_p;
    RULE_TYPE_RETURN_TYPE_T         ret_code;

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    ret_code = RULE_OM_LocalConvert2UIClassMapElement(class_map_p, element_index, element_entry);

    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMapElementClassIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : according to element_entry contents to get class-index in specified class-map
 * INPUT    : class_map_index, element_entry
 * OUTPUT   : class_index (MF, ACL index)
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMapElementClassIndex(UI32_T class_map_index, RULE_TYPE_UI_ClassMapElement_T *element_entry, UI32_T *class_index)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    UI32_T                  element_index;

    RULE_OM_EnterCriticalSection();

    if (RULE_TYPE_OK != RULE_OM_LocalGetClassMapElementIndex(class_map_index, element_entry, &element_index))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (NULL == class_index)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    *class_index = class_map_p->class_index_list[element_index - 1]; /* to array index */

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

// FIXIT: OM should have not KB for hardware, the check will failed on some chip configuration.
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Add_ClassMapElement_To_ClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Add ClassMapElement to ClassMap by index
 * INPUT:    class_map_index, element_entry
 * OUTPUT:   class_index
 * RETURN:   Error code (OK / FAIL / NOT_ALLOW_MIX_SELECTOR)
 * NOTE:     class_index == acl index or ace index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Add_ClassMapElement_To_ClassMap(UI32_T class_map_index, RULE_TYPE_UI_ClassMapElement_T *element_entry, UI32_T *class_index)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    UI32_T                  element_index;

    if ((NULL == element_entry) || (NULL == class_index))
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (SYS_ADPT_DIFFSERV_MAX_CLASS_NBR_OF_CLASS_MAP <= class_map_p->class_element_count)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* class-map is full */

    if (RULE_TYPE_OK == RULE_OM_LocalGetClassMapElementIndex(class_map_index, element_entry, &element_index))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* don't allow class-element duplicate */


    if (RULE_TYPE_OK != RULE_OM_LocalValidateMatchAllClassMapElement(class_map_index, class_map_p, element_entry))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    switch (element_entry->class_type)
    {
        case RULE_TYPE_CLASS_ACL:
        {
            RULE_TYPE_AclType_T     acl_type;
            RULE_TYPE_Acl_T         *acl_p;

            if (RULE_TYPE_OK != RULE_OM_LocalGetAclIdByName(element_entry->element.acl_name, class_index))
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

            acl_p = RULE_OM_LocalGetAclByIndex(*class_index);
            if ((NULL == acl_p) ||
                (RULE_TYPE_OK != RULE_OM_LocalClassifyAcl(acl_p, &acl_type)))
            {
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* should not go here */
            }

            if (FALSE == RULE_OM_LocalIsValidElementTypeOflassMap(class_map_p, acl_type))
            {
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_NOT_ALLOW_MIX_SELECTOR);
            }

            RULE_OM_SET_BIT_ON(acl_p->class_map_bit_list, (class_map_index - 1));
            acl_p->class_map_count++;
            break;
        }
        case RULE_TYPE_CLASS_MF:
        {
            RULE_TYPE_AclType_T     ui_mf_type;

            if (RULE_TYPE_OK != RULE_OM_LocalClassifyUIMf(&element_entry->element.mf_entry, &ui_mf_type))
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* should not go here */

            if (FALSE == RULE_OM_LocalIsValidElementTypeOflassMap(class_map_p, ui_mf_type))
            {
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_NOT_ALLOW_MIX_SELECTOR);
            }

            if (RULE_TYPE_OK != RULE_OM_LocalCreateMf(&element_entry->element.mf_entry, class_index))
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
            break;
        }
        default: /* should not go here */
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    class_map_p->class_type[class_map_p->class_element_count] = element_entry->class_type;
    class_map_p->class_index_list[class_map_p->class_element_count] = *class_index;
    class_map_p->class_element_count++;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

// TODO: add vs remove
//       create vs destroy
// This function will do "removeClassMapElementFromClassMap" for ACL
// but for MF will       "removeClassMapElementFromClassMapAndDestroy"
// All core function should use "pointer" not id or index
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Remove_ClassMapElement_From_ClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Add ClassMapElement to ClassMap by index
 * INPUT:    class_map_index, element_entry
 * OUTPUT:   None
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Remove_ClassMapElement_From_ClassMap(UI32_T class_map_index, RULE_TYPE_UI_ClassMapElement_T *element_entry)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    UI32_T                  element_index;
    UI32_T                  acl_id = 0;
    RULE_TYPE_RETURN_TYPE_T ret_code;
    if (NULL == element_entry)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (RULE_TYPE_OK != RULE_OM_LocalGetClassMapElementIndex(class_map_index, element_entry, &element_index))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* elemenet entry not found */

    switch (element_entry->class_type)
    {
        case RULE_TYPE_CLASS_MF:
            /* delete a multi-filter (ACE) from ACE table */
            RULE_OM_LocalFreeAceEntry(class_map_p->class_index_list[element_index - 1]); /* to array index */
            break;

        case RULE_TYPE_CLASS_ACL:
            /* remove acl related information */
            if (RULE_TYPE_OK != RULE_OM_LocalGetAclIdByName(element_entry->element.acl_name, &acl_id))
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
            RULE_OM_SET_BIT_OFF(shmem_data_p->acl_table[acl_id-1].class_map_bit_list, (class_map_index - 1));
            shmem_data_p->acl_table[acl_id-1].class_map_count--;
            break;

        default:
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* should not go here */
    }

    ret_code = RULE_OM_LocalRemoveAndCompactClassMapElement(class_map_p, element_index);

    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RemoveClassMapElementByAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : remove a class-map element according to acl index
 * INPUT    : class_map_index, acl_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_RemoveClassMapElementByAclIndex(UI32_T class_map_index, UI32_T acl_index)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    UI32_T                  index;
    RULE_TYPE_Acl_T         *acl_p;
    RULE_TYPE_RETURN_TYPE_T ret_code;

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    for (index = 0; index < class_map_p->class_element_count; ++index)
    {
        if (RULE_TYPE_CLASS_ACL != class_map_p->class_type[index])
            continue;

        if (acl_index == class_map_p->class_index_list[index])
            break; /* found */
    }

    if (index >= class_map_p->class_element_count)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* not found */

    /* remove acl related information */
    RULE_OM_SET_BIT_OFF(acl_p->class_map_bit_list, (class_map_index - 1));
    --(acl_p->class_map_count);

    ret_code = RULE_OM_LocalRemoveAndCompactClassMapElement(class_map_p, (index + 1)); /* to 1-based */

    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RemoveClassMapElementByMfIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : remove a class-map element according to mf index
 * INPUT    : class_map_index, mf_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_RemoveClassMapElementByMfIndex(UI32_T class_map_index, UI32_T mf_index)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    UI32_T                  index;
    RULE_TYPE_Ace_Entry_T   *ace_p;
    RULE_TYPE_RETURN_TYPE_T ret_code;

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    ace_p = RULE_OM_LocalGetAceByIndex(mf_index);
    if (NULL == ace_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    for (index = 0; index < class_map_p->class_element_count; ++index)
    {
        if (RULE_TYPE_CLASS_MF != class_map_p->class_type[index])
            continue;

        if (mf_index == class_map_p->class_index_list[index])
            break; /* found */
    }

    if (index >= class_map_p->class_element_count)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* not found */

    ret_code = RULE_OM_LocalRemoveAndCompactClassMapElement(class_map_p, (index + 1)); /* to 1-based */
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Add_PolicyMapElement_To_PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Add PolicyMapElement to PolicyMap by index
 * INPUT:    policy_map_index, class_map_index
 * OUTPUT:   None
 * RETURN:   Error code (OK / FAIL / EXISTED)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Add_PolicyMapElement_To_PolicyMap(UI32_T policy_map_index, UI32_T class_map_index)
{
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_ClassMap_T        *class_map_p;
    UI32_T                      policy_map_element_index = 0;
    RULE_TYPE_PolicyElement_T   *policy_map_element_p;

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (FALSE == RULE_OM_IS_ENTRY_ACTIVE(class_map_p->row_status))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* do not allow to associate with non-active entry */

    /* When users issue CLI command: class-map "name" in policy-map configuration mode,
     * CLI always invoke this API indirectly. Therefore, return EXISTED, not FAIL to create.
     */
    policy_map_element_p = RULE_OM_LocalGetPolicyMapElement(policy_map_index, class_map_index);
    if (NULL != policy_map_element_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_EXISTED);

    if (SYS_ADPT_DIFFSERV_MAX_CLASS_MAP_NBR_OF_POLICY_MAP <= policy_map_p->element_count)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* policy-map is full */

    if (FALSE == RULE_OM_LocalAllocatePolicyMapElement(&policy_map_element_index))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* no more unused space */

    policy_map_element_p = &shmem_data_p->policy_map_element_table[policy_map_element_index - 1]; /* to zero-based array index */
    policy_map_element_p->class_map_index = class_map_index;
    policy_map_element_p->meter_index = 0;
    policy_map_element_p->action_index = 0;

    if ((FALSE == RULE_OM_LocalAllocate_Action_Index(&policy_map_element_p->action_index)) ||
        (RULE_TYPE_OK != RULE_OM_LocalSetInProfileActionToDefault(policy_map_element_p->action_index)))
    {
        RULE_OM_SET_ENTRY_DESTROY(policy_map_element_p->row_status);
        RULE_OM_LocalFreeAction(policy_map_element_p->action_index);
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* no more unused space */
    }

    policy_map_p->policy_map_element_index_list[policy_map_p->element_count] = policy_map_element_index;
    policy_map_p->element_count++;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Set_MeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Set Meter entry
 * INPUT:    policy_map_index, class_map_index, meter_entry
 * OUTPUT:   None
 * RETURN:   Error code (OK / FAIL / SAME_CFG)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Set_MeterEntry(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_TBParamEntry_T *meter_entry)
{
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;
    RULE_TYPE_TBParamEntry_T    *meter_p;
    RULE_TYPE_Action_T          *action_p;
    UI32_T                      meter_index = 0;
    RULE_TYPE_RETURN_TYPE_T     ret_code;

    RULE_OM_EnterCriticalSection();

    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    policy_element_p = RULE_OM_LocalGetPolicyMapElement(policy_map_index, class_map_index);
    if (NULL == policy_element_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    #if 0
    if (RULE_TYPE_OK != RULE_OM_LocalValidateMeter(meter_entry))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad parameters */
    #endif

    if (RULE_TYPE_OK != RULE_OM_LocalExtraValidateMeter(policy_map_index,
                                                        policy_element_p->class_map_index,
                                                        policy_element_p->meter_index,
                                                        meter_entry))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    action_p = RULE_OM_LocalGetActionEntryByIndex(policy_element_p->action_index);
    if (NULL == action_p)
    {
        if ((FALSE == RULE_OM_LocalAllocate_Action_Index(&policy_element_p->action_index)) ||
            (RULE_TYPE_OK != RULE_OM_LocalSetInProfileActionToDefault(policy_element_p->action_index)))
        {
            RULE_OM_SET_ENTRY_DESTROY(policy_element_p->row_status);
            RULE_OM_LocalFreeAction(policy_element_p->action_index);
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* no more unused space */
        }
        else
            action_p = RULE_OM_LocalGetActionEntryByIndex(policy_element_p->action_index);
    }

    meter_p = RULE_OM_LocalGetMeterEntryByIndex(policy_element_p->meter_index);

    if (NULL != meter_p) /* if meter exist, update it */
    {
        if ((meter_p->meter_model == meter_entry->meter_model) &&
            (meter_p->rate == meter_entry->rate) &&
            (meter_p->burst_size == meter_entry->burst_size) &&
            (meter_p->peak_rate == meter_entry->peak_rate) &&
            (meter_p->peak_burst_size == meter_entry->peak_burst_size) &&
            (meter_p->interval == meter_entry->interval))
        {
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_SAME_CFG); /* new meter == original meter */
        }

        meter_p->meter_model = meter_entry->meter_model;
        meter_p->rate = meter_entry->rate;
        meter_p->burst_size = meter_entry->burst_size;
        meter_p->interval = meter_entry->interval;
        meter_p->peak_rate = meter_entry->peak_rate;
        meter_p->peak_burst_size = meter_entry->peak_burst_size;

        RULE_OM_LocalNotify(RULE_TYPE_NOTIFY_METER_CHANGED, &policy_element_p->meter_index);
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }

    /* create meter */
    if (FALSE == RULE_OM_LocalAllocateMeterIndex(&meter_index))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* no more meter space */

    meter_p = &shmem_data_p->meter_table[meter_index - 1]; /* to zero-based array index */

    meter_p->meter_model = meter_entry->meter_model;
    meter_p->rate = meter_entry->rate;
    meter_p->burst_size = meter_entry->burst_size;
    meter_p->interval = meter_entry->interval;
    meter_p->peak_rate = meter_entry->peak_rate;
    meter_p->peak_burst_size = meter_entry->peak_burst_size;
    RULE_OM_SET_ENTRY_ACTIVE(meter_p->row_status);

    /* associate meter to policy-map */
    policy_element_p->meter_index = meter_index;

    RULE_OM_LocalNotify(RULE_TYPE_NOTIFY_METER_CHANGED, &policy_element_p->meter_index);

    /* when create a new meter, SHALL give default out of profile action */
    ret_code = RULE_OM_LocalSetOutOfProfileActionToDefault(policy_element_p->action_index);

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    ret_code = RULE_OM_LocalSetClassActionToDefault(policy_element_p->action_index);
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

    action_p->action_bitmap |= RULE_TYPE_ACTION_DROP_PRECEDENCE;
    action_p->action_bitmap |= RULE_TYPE_ACTION_YELLOW_DROP_PRECEDENCE;
    action_p->action_bitmap |= RULE_TYPE_ACTION_RED_DROP_PRECEDENCE;

    if(RULE_TYPE_METER_MODE_FLOW == meter_entry->meter_model)
        action_p->action_bitmap &= ~RULE_TYPE_ACTION_YELLOW_DROP_PRECEDENCE;

    /* when create a new meter, SHALL give default out of profile action */
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Remove_MeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Remove Meter entry
 * INPUT:    policy_map_index, class_map_index
 * OUTPUT:   None
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Remove_MeterEntry(UI32_T policy_map_index, UI32_T class_map_index)
{
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;
    RULE_TYPE_Action_T          *action_p;

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    policy_element_p = RULE_OM_LocalGetPolicyMapElement(policy_map_index, class_map_index);
    if (NULL == policy_element_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    action_p = RULE_OM_LocalGetActionEntryByIndex(policy_element_p->action_index);
    if (NULL == action_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    /* once meter is deleted clear out of profile action */
    RULE_OM_LocalSetOutOfProfileActionToNull(policy_element_p->action_index);

    if (RULE_OM_IS_VALID_METER_IDX(policy_element_p->meter_index))
    {
        RULE_OM_LocalFreeMeter(policy_element_p->meter_index);
        policy_element_p->meter_index = 0; /* unbind meter to policy-map element */
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }
    else
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_CreateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  create meter on specified index
 * INPUT:    meter_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_CreateMeter(UI32_T meter_index, BOOL_T is_active)
{
    RULE_TYPE_TBParamEntry_T    *meter_p;

    if (RULE_OM_IS_BAD_METER_IDX(meter_index))
        return RULE_TYPE_FAIL; /* bad index */

    RULE_OM_EnterCriticalSection();
    meter_p = &shmem_data_p->meter_table[meter_index - 1]; /* to zero-based array index */
    if (RULE_OM_IS_ENTRY_AVAILABLE(meter_p->row_status))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* already created */

    memset(meter_p, 0, sizeof(RULE_TYPE_TBParamEntry_T));

    meter_p->meter_model = VAL_diffServMeterModel_default;

    meter_p->rate = RULE_OM_DFLT_METER_RATE;
    meter_p->burst_size = RULE_OM_DFLT_METER_BURST_SIZE;
    meter_p->peak_rate = RULE_OM_DFLT_METER_RATE;
    meter_p->peak_burst_size = RULE_OM_DFLT_METER_BURST_SIZE;
    meter_p->interval = RULE_OM_DFLT_METER_INTERVAL;

    if (TRUE == is_active)
    {
        RULE_OM_SET_ENTRY_ACTIVE(meter_p->row_status);
    }
    else
    {
        RULE_OM_SET_ENTRY_NOTREADY(meter_p->row_status);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DestroyMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  destroy meter by index
 * INPUT:    meter_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     rule mgr SHALL remove meter from policy-map element first
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DestroyMeter(UI32_T meter_index)
{
    RULE_TYPE_TBParamEntry_T    *meter_p;

    RULE_OM_EnterCriticalSection();
    meter_p = RULE_OM_LocalGetMeterEntryByIndex(meter_index);
    if (NULL == meter_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* not exist */

    RULE_OM_LocalFreeMeter(meter_index);
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetMeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  set meter entry by meter_index
 * INPUT:    meter_index, meter_entry
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetMeterEntry(UI32_T meter_index, const RULE_TYPE_TBParamEntry_T *meter_entry)
{
    RULE_TYPE_TBParamEntry_T    *meter_p;
    UI32_T                      row_status;

    if (NULL == meter_entry)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    meter_p = RULE_OM_LocalGetMeterEntryByIndex(meter_index);

    if (NULL == meter_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    row_status = meter_p->row_status;
    memcpy(meter_p, meter_entry, sizeof(RULE_TYPE_TBParamEntry_T));
    meter_p->row_status = row_status;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetMeterRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set meter row status by it's index
 * INPUT:    meter_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetMeterRowStatus(UI32_T meter_index, BOOL_T is_active)
{
    RULE_TYPE_TBParamEntry_T    *meter_p;

    RULE_OM_EnterCriticalSection();
    meter_p = RULE_OM_LocalGetMeterEntryByIndex(meter_index);
    if (NULL == meter_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (TRUE == is_active)
    {
        if (RULE_TYPE_OK != RULE_OM_LocalValidateMeter(meter_p))
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad parameters */
        RULE_OM_SET_ENTRY_ACTIVE(meter_p->row_status);
    }
    else
    {
        RULE_OM_SET_ENTRY_NOTREADY(meter_p->row_status);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetMeterEntryByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  get Meter entry by meter_index
 * INPUT:    meter_index
 * OUTPUT:   meter_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetMeterEntryByIndex(UI32_T meter_index, RULE_TYPE_TBParamEntry_T *meter_entry)
{
    RULE_TYPE_TBParamEntry_T    *meter_p;

    if (NULL == meter_entry)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    meter_p = RULE_OM_LocalGetMeterEntryByIndex(meter_index);
    if (NULL == meter_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    memcpy(meter_entry, meter_p, sizeof(RULE_TYPE_TBParamEntry_T));
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextMeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get next meter entry by meter_index
 * INPUT:    meter_index
 * OUTPUT:   meter_index, meter_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextMeterEntry(UI32_T *meter_index, RULE_TYPE_TBParamEntry_T *meter_entry)
{
    UI32_T  index;

    if ((NULL == meter_index) || (NULL == meter_entry))
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    for (index = *meter_index; SYS_ADPT_DIFFSERV_MAX_NBR_OF_METER > index; ++index)
    {
        if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->meter_table[index].row_status))
            continue;

        *meter_index = index + 1; /* to 1-based */
        memcpy(meter_entry, &shmem_data_p->meter_table[index], sizeof(RULE_TYPE_TBParamEntry_T));
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetMeterRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  get meter row status by it's index
 * INPUT:    action_index
 * OUTPUT:   row_status
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetMeterRowStatus(UI32_T meter_index, UI32_T *row_status)
{
    RULE_TYPE_TBParamEntry_T    *meter_p;

    if (NULL == row_status)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    meter_p = RULE_OM_LocalGetMeterEntryByIndex(meter_index);
    if (NULL == meter_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    *row_status = meter_p->row_status;
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Set_ActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Set action entry
 * INPUT:    policy_map_index, class_map_index, action_entry
 * OUTPUT:   None
 * RETURN:   Error code (OK / FAIL / SAME_CFG)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Set_ActionEntry(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_Action_T *action_entry)
{
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;
    RULE_TYPE_Action_T          *action_p;
    UI32_T                      action_index = 0, org_row_status;

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    policy_element_p = RULE_OM_LocalGetPolicyMapElement(policy_map_index, class_map_index);
    if (NULL == policy_element_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    /* if action exist, update it */
    if (RULE_OM_IS_VALID_ACTION_IDX(policy_element_p->action_index))
    {
        action_p = &shmem_data_p->action_table[policy_element_p->action_index - 1]; /* to zero-based array index */

        if (0 == memcmp(action_p, action_entry, sizeof(RULE_TYPE_Action_T)))
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_SAME_CFG); /* new action == original action */

        RULE_OM_LocalUpdateAction(policy_element_p->action_index, action_entry);
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }

    /* create action */
    if (FALSE == RULE_OM_LocalAllocate_Action_Index(&action_index))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* no more action space */

    action_p = &shmem_data_p->action_table[action_index - 1]; /* to zero-based array index */
    org_row_status = action_p->row_status;
    memcpy(action_p, action_entry, sizeof(RULE_TYPE_Action_T));
    action_p->row_status = org_row_status;

    /* associate action to policy-map */
    policy_element_p->action_index = action_index;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetUIActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Set action entry
 * INPUT:    policy_map_index, class_map_index, action_entry
 * OUTPUT:   None
 * RETURN:   Error code (OK / FAIL / SAME_CFG)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetUIActionEntry(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_UI_Action_T *action_entry)
{
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;
    RULE_TYPE_Action_T          *action_p;
    UI32_T                      action_index = 0;
    RULE_TYPE_Action_T          tmp_action;

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    policy_element_p = RULE_OM_LocalGetPolicyMapElement(policy_map_index, class_map_index);
    if (NULL == policy_element_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (RULE_TYPE_FAIL == RULE_OM_LocalConvertToAction(action_entry, &tmp_action))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    if (RULE_TYPE_OK != RULE_OM_LocalExtraValidateAction(policy_map_index,
                                                         policy_element_p->class_map_index,
                                                         policy_element_p->action_index,
                                                         action_entry))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    /* if action exist, update it */
    if (RULE_OM_IS_VALID_ACTION_IDX(policy_element_p->action_index))
    {
        action_p = &shmem_data_p->action_table[policy_element_p->action_index - 1]; /* to zero-based array index */

        if (0 == memcmp(action_p, &tmp_action, sizeof(RULE_TYPE_Action_T)))
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_SAME_CFG); /* new action == original action */

        RULE_OM_LocalUpdateAction(policy_element_p->action_index, &tmp_action);
    }
    else
    {
        /* create action */
        if (FALSE == RULE_OM_LocalAllocate_Action_Index(&action_index))
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* no more action space */

        action_p = &shmem_data_p->action_table[action_index - 1]; /* to zero-based array index */
        RULE_OM_LocalUpdateAction(action_index, &tmp_action);

        /* associate action to policy-map */
        policy_element_p->action_index = action_index;
    }

    RULE_OM_LocalNotify(RULE_TYPE_NOTIFY_ACTION_CHANGED, &policy_element_p->action_index);
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Get_ActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get action entry
 * INPUT:    policy_map_index, class_map_index, action_entry
 * OUTPUT:   None
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Get_ActionEntry(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_Action_T *action_entry)
{
    UI32_T action_index = 0;

    if (NULL == action_entry)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    if (RULE_TYPE_OK == RULE_OM_LocalGetActionIndexFromPolicyMapElement(policy_map_index, class_map_index, &action_index))
    {
        memcpy(action_entry, &(shmem_data_p->action_table[action_index - 1]), sizeof(RULE_TYPE_Action_T)); /* to zero-based array index */
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetUIActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get action entry
 * INPUT:    policy_map_index, class_map_index, action_entry
 * OUTPUT:   None
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetUIActionEntry(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_UI_Action_T *action_entry)
{
    UI32_T action_index = 0;
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    if (NULL == action_entry)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    if (RULE_TYPE_OK != RULE_OM_LocalGetActionIndexFromPolicyMapElement(policy_map_index, class_map_index, &action_index))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    ret_code = RULE_OM_LocalConvert2UIAction(&(shmem_data_p->action_table[action_index - 1]), action_entry);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_CreateAction
 *------------------------------------------------------------------------------
 * PURPOSE:  create action on specified index
 * INPUT:    action_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_CreateAction(UI32_T action_index, BOOL_T is_active)
{
    RULE_TYPE_Action_T       *action_p;
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    if (RULE_OM_IS_BAD_ACTION_IDX(action_index))
        return RULE_TYPE_FAIL; /* bad index */

    RULE_OM_EnterCriticalSection();
    action_p = &shmem_data_p->action_table[action_index - 1]; /* to zero-based array index */
    if (RULE_OM_IS_ENTRY_AVAILABLE(action_p->row_status))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* already created */

    memset(action_p, 0, sizeof(RULE_TYPE_Action_T));

    if (TRUE == is_active)
    {
        RULE_OM_SET_ENTRY_ACTIVE(action_p->row_status);
    }
    else
    {
        RULE_OM_SET_ENTRY_NOTREADY(action_p->row_status);
    }

    ret_code = RULE_OM_LocalSetInProfileActionToDefault(action_index);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DestroyAction
 *------------------------------------------------------------------------------
 * PURPOSE:  destroy action by index
 * INPUT:    action_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     rule mgr SHALL remove action from policy-map element first
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DestroyAction(UI32_T action_index)
{
    RULE_TYPE_Action_T    *action_p;

    RULE_OM_EnterCriticalSection();
    action_p = RULE_OM_LocalGetActionEntryByIndex(action_index);
    if (NULL == action_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK); /* not exist */

    RULE_OM_LocalFreeAction(action_index);
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  set action entry by action_index
 * INPUT:    action_index, action_entry
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetActionEntry(UI32_T action_index, const RULE_TYPE_Action_T *action_entry)
{
    RULE_TYPE_Action_T      *action_p;
    UI32_T                  row_status;

    if (NULL == action_entry)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    action_p = RULE_OM_LocalGetActionEntryByIndex(action_index);
    if (NULL == action_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */


    /* following fields' value MUST be reserved */
    row_status = action_p->row_status;

    /* copy then restore reserved fields' value */
    memcpy(action_p, action_entry, sizeof(RULE_TYPE_Action_T));

    action_p->row_status = row_status;
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetActionRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set action row status by it's index
 * INPUT:    action_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetActionRowStatus(UI32_T action_index, BOOL_T is_active)
{
    RULE_TYPE_Action_T    *action_p;

    RULE_OM_LeaveCriticalSection();
    action_p = RULE_OM_LocalGetActionEntryByIndex(action_index);
    if (NULL == action_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (TRUE == is_active)
    {
        if (RULE_TYPE_OK != RULE_OM_LocalValidateAction(action_p))
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad parameters */
        RULE_OM_SET_ENTRY_ACTIVE(action_p->row_status);
    }
    else
    {
        RULE_OM_SET_ENTRY_NOTREADY(action_p->row_status);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetInProfileActionToDefault
 *------------------------------------------------------------------------------
 * PURPOSE:  set in-profile action to default value by action_index
 * INPUT:    action_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetInProfileActionToDefault(UI32_T action_index)
{
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    RULE_OM_EnterCriticalSection();
    ret_code = RULE_OM_LocalSetInProfileActionToDefault(action_index);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetOutOfProfileActionToDefault
 *------------------------------------------------------------------------------
 * PURPOSE:  set out-of-profile action to default value by action_index
 * INPUT:    action_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetOutOfProfileActionToDefault(UI32_T action_index)
{
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    RULE_OM_EnterCriticalSection();
    ret_code = RULE_OM_LocalSetOutOfProfileActionToDefault(action_index);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetClassActionToDefault
 *------------------------------------------------------------------------------
 * PURPOSE:  set class action to default value by action_index
 * INPUT:    action_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetClassActionToDefault(UI32_T action_index)
{
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    RULE_OM_EnterCriticalSection();
    ret_code = RULE_OM_LocalSetClassActionToDefault(action_index);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetOutOfProfileActionToNull
 *------------------------------------------------------------------------------
 * PURPOSE:  set out-of-profile action to null value by action_index
 * INPUT:    action_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetOutOfProfileActionToNull(UI32_T action_index)
{
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    RULE_OM_EnterCriticalSection();
    ret_code = RULE_OM_LocalSetOutOfProfileActionToNull(action_index);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetActionEntryByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  get action entry by action_index
 * INPUT:    action_index
 * OUTPUT:   action_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetActionEntryByIndex(UI32_T action_index, RULE_TYPE_Action_T *action_entry)
{
    RULE_TYPE_Action_T    *action_p;

    if (NULL == action_entry)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    action_p = RULE_OM_LocalGetActionEntryByIndex(action_index);
    if (NULL == action_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    memcpy(action_entry, action_p, sizeof(RULE_TYPE_Action_T));
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetMibActionEntryByIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get mib action entry by action_index
 * INPUT    : action_index
 * OUTPUT   : action_entry
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetMibActionEntryByIndex(UI32_T action_index, RULE_TYPE_MIB_Action_T *action_entry)
{
    RULE_TYPE_Action_T       *action_p;
    RULE_TYPE_RETURN_TYPE_T  ret_code;
    if (NULL == action_entry)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    action_p = RULE_OM_LocalGetActionEntryByIndex(action_index);
    if (NULL == action_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    ret_code = RULE_OM_LocalConvert2MibAction(action_p, action_entry);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get next action entry by action_index
 * INPUT:    action_index
 * OUTPUT:   action_index, action_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextActionEntry(UI32_T *action_index, RULE_TYPE_Action_T *action_entry)
{
    UI32_T  index;

    if ((NULL == action_index) || (NULL == action_entry))
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    for (index = *action_index; SYS_ADPT_DIFFSERV_MAX_NBR_OF_ACTIONS > index; ++index)
    {
        if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->action_table[index].row_status))
            continue;

        *action_index = index + 1; /* to 1-based */
        memcpy(action_entry, &shmem_data_p->action_table[index], sizeof(RULE_TYPE_Action_T));
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextMibActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next mib action entry by action_index
 * INPUT    : action_index
 * OUTPUT   : action_index, action_entry
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextMibActionEntry(UI32_T *action_index, RULE_TYPE_MIB_Action_T *action_entry)
{
    UI32_T                   index;
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    if ((NULL == action_index) || (NULL == action_entry))
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    for (index = *action_index; SYS_ADPT_DIFFSERV_MAX_NBR_OF_ACTIONS > index; ++index)
    {
        if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->action_table[index].row_status))
            continue;

        *action_index = index + 1; /* to 1-based */
        ret_code = RULE_OM_LocalConvert2MibAction(&shmem_data_p->action_table[index], action_entry);
        RULE_OM_LeaveCriticalSectionReturnState(ret_code);
    }
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DupActionEntryWithNewActionBitmap
 *------------------------------------------------------------------------------
 * PURPOSE  : will duplicate action entry of action_index and
 *            change the data according to the new action_bitmap
 * INPUT    : action_index, action_bitmap
 * OUTPUT   : action_p
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DupActionEntryWithNewActionBitmap(UI32_T action_index, const UI8_T *action_bitmap, RULE_TYPE_Action_T *dst_action_p)
{
    RULE_TYPE_Action_T          *src_action_p;
    UI32_T                      core_layer_bitmap = 0;

    RULE_OM_EnterCriticalSection();

    src_action_p = RULE_OM_LocalGetActionEntryByIndex(action_index);
    if ((NULL == src_action_p) || (NULL == dst_action_p))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad action index */

    /* translate mib bitmap to core layer bitmap */
    if (RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionPktNewPri))
        core_layer_bitmap |= RULE_TYPE_ACTION_PKT_NEW_PRI;

    if (RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionPktNewPhb))
        core_layer_bitmap |= RULE_TYPE_ACTION_PKT_NEW_PHB;

    if (RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionPktNewIpPrec))
        core_layer_bitmap |= RULE_TYPE_ACTION_PKT_NEW_TOS;

    if (RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionPktNewDscp))
        core_layer_bitmap |= RULE_TYPE_ACTION_PKT_NEW_DSCP;

    if (RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionGreenPktNewDscp))
        core_layer_bitmap |= RULE_TYPE_ACTION_GREEN_PKT_NEW_DSCP;

    if (RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionGreenPktTransmit))
        core_layer_bitmap |= RULE_TYPE_ACTION_GREEN_PKT_TRANSMIT;

    if (RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionRedPktNewDscp))
        core_layer_bitmap |= RULE_TYPE_ACTION_RED_PKT_NEW_DSCP;

    if (RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionRedPktDrop))
        core_layer_bitmap |= RULE_TYPE_ACTION_RED_DROP;

     if (RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionYellowPktNewDscp))
        core_layer_bitmap |= RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP;

/*
     not support this bit
     if (RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionDrop))
        core_layer_bitmap |= RULE_TYPE_ACTION_DROP;
*/

     if (RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionYellowPktDrop))
        core_layer_bitmap |= RULE_TYPE_ACTION_YELLOW_DROP;

    /* check core_layer_bitmap valid or not */
    if (RULE_TYPE_OK != RULE_OM_LocalCheckActionCollision(core_layer_bitmap))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }
#if 0
    policy_element_p = RULE_OM_LocalGetPolicyMapElementByActionIndex(action_index);
    if ((NULL != policy_element_p) &&
        (RULE_OM_IS_VALID_METER_IDX(policy_element_p->meter_index)))
    {
        /* if meter exist, out-of-profile action MUST exist */
        if ((0 == (RULE_TYPE_ACTION_RED_PKT_NEW_DSCP & core_layer_bitmap)) &&
            (0 == (RULE_TYPE_ACTION_RED_DROP & core_layer_bitmap)))
        {
            LOG("failure caused by meter(%lu) exist", policy_element_p->meter_index);
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
        }
    }
#endif

/* Why ?
    if (RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionGreenPktTransmit)||
    RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionGreenPktNewDscp)||
    RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionYellowPktNewDscp)||
    RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionYellowPktDrop)||
    RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionRedPktNewDscp)||
    RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionRedPktDrop))
    {
        if (!((RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionGreenPktTransmit)||RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionGreenPktNewDscp))&&
        (RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionRedPktNewDscp)||RULE_OM_IS_BIT_ON(action_bitmap, VAL_diffServActionList_actionRedPktDrop))))
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }
*/

    /* duplicate the entry and correct data */
    memcpy (dst_action_p, src_action_p, sizeof (RULE_TYPE_Action_T));
    dst_action_p->action_bitmap = core_layer_bitmap;

    /* reset un-configured action to default */
    if (0 == (RULE_TYPE_ACTION_PKT_NEW_PRI & core_layer_bitmap))
        dst_action_p->pkt_new_pri = RULE_TYPE_ACTION_DEFAULT_PRI;

    if (0 == (RULE_TYPE_ACTION_PKT_NEW_PHB & core_layer_bitmap))
        dst_action_p->pkt_new_phb = RULE_TYPE_ACTION_DEFAULT_PHB;

    if (0 == (RULE_TYPE_ACTION_PKT_NEW_TOS & core_layer_bitmap))
        dst_action_p->pkt_new_tos = 0;

    if (0 == (RULE_TYPE_ACTION_PKT_NEW_DSCP & core_layer_bitmap))
        dst_action_p->pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;

    if (0 == (RULE_TYPE_ACTION_GREEN_PKT_NEW_DSCP & core_layer_bitmap))
        dst_action_p->green_pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;

    if (0 == (RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP & core_layer_bitmap))
        dst_action_p->yellow_pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;

    if (0 == (RULE_TYPE_ACTION_RED_PKT_NEW_DSCP & core_layer_bitmap))
        dst_action_p->red_pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;

    if (0 == (RULE_TYPE_ACTION_DROP_PRECEDENCE & core_layer_bitmap))
        dst_action_p->drop_precedence = 0;

    if (0 == (RULE_TYPE_ACTION_RED_DROP_PRECEDENCE & core_layer_bitmap))
        dst_action_p->red_drop_precedence = 0;

    if (0 == (RULE_TYPE_ACTION_YELLOW_DROP_PRECEDENCE & core_layer_bitmap))
        dst_action_p->yellow_drop_precedence = 0;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetActionRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  get action row status by it's index
 * INPUT:    action_index
 * OUTPUT:   row_status
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetActionRowStatus(UI32_T action_index, UI32_T *row_status)
{
    RULE_TYPE_Action_T    *action_p;

    if (NULL == row_status)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    action_p = RULE_OM_LocalGetActionEntryByIndex(action_index);
    if (NULL == action_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    *row_status = action_p->row_status;
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_ConvertToAction
 *------------------------------------------------------------------------------
 * PURPOSE  : translate src_action to dst_action
 * INPUT    : src_action
 * OUTPUT   : dst_action
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_ConvertToAction(const RULE_TYPE_UI_Action_T *src_action, RULE_TYPE_Action_T *dst_action)
{
    return RULE_OM_LocalConvertToAction(src_action, dst_action);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPortPolicyMapIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  get the policy-map index bound to a port by ifindex and ingress flag
 * INPUT:    ifindex, ingress_flag
 * OUTPUT:   pmap_index
 * RETURN:   Error code.
 * NOTE:     return RULE_TYPE_FAIL if no policy-map bound to that port.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPortPolicyMapIndex(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *pmap_idx)
{
    RULE_TYPE_PortEntry_T   *port_entry_p;

    if (NULL == pmap_idx)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, inout_profile);
    if (NULL == port_entry_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad ifindex */

    if (TRUE != RULE_OM_IS_ENTRY_ACTIVE(port_entry_p->policy_map_row_status))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    if (port_entry_p->policy_map_index != 0)
    {
        *pmap_idx = port_entry_p->policy_map_index;
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapNameByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get binding policy map's name of port.
 * INPUT:    uport_ifinde
 * OUTPUT:   policy_map_name.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapNameByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    char *policy_map_name)
{
    RULE_TYPE_PortEntry_T *port_entry_p;
    RULE_TYPE_PolicyMap_T       *policy_map_p;

    RULE_OM_EnterCriticalSection();

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, inout_profile);

    if (NULL == port_entry_p || 0 == port_entry_p->policy_map_index)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    if (TRUE != RULE_OM_IS_ENTRY_ACTIVE(port_entry_p->policy_map_row_status))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(port_entry_p->policy_map_index);

    if (NULL == policy_map_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    memcpy(policy_map_name, policy_map_p->name, SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
    policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0'; /* force to end a string */

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get name of policy map by it's index.
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map_name.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapNameById(UI32_T policy_map_index, char *policy_map_name)
{
    RULE_TYPE_PolicyMap_T       *policy_map_p;

    if (NULL == policy_map_name)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    memcpy(policy_map_name, policy_map_p->name, SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
    policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0'; /* force to end a string */
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of policy map by it's name.
 * INPUT:    policy_map_name
 * OUTPUT:   policy_map_index.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapIdByName(const char *policy_map_name, UI32_T *policy_map_index)
{
    UI32_T i = 0;

    if ((policy_map_name == NULL) || (NULL == policy_map_index))
    {
        return RULE_TYPE_FAIL;
    }

    RULE_OM_EnterCriticalSection();
    for (i = 0; i < SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP; i++)
    {
        if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->policy_map_table[i].row_status))
            continue;

        if (0 == strcmp((char *)policy_map_name, (char *)shmem_data_p->policy_map_table[i].name))
        {
            *policy_map_index = i + 1; /* to 1-based index */
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
        }
    }
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapIndexByElementIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  get index of policy map by policy-map element index
 * INPUT:    policy_element_index
 * OUTPUT:   policy_map_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapIndexByElementIndex(UI32_T policy_element_index, UI32_T *policy_map_index)
{
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    RULE_OM_EnterCriticalSection();
    ret_code = RULE_OM_LocalGetPolicyMapIndexByElementIndex(policy_element_index, policy_map_index);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPolicyMapNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  set policy map name by it's index
 * INPUT:    policy_map_index, policy_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetPolicyMapNameById(UI32_T policy_map_index, const char *policy_map_name)
{
    UI32_T                      name_len, found_index;
    RULE_TYPE_PolicyMap_T       *policy_map_p;

    if (NULL == policy_map_name)
        return RULE_TYPE_FAIL;

    /* check name length */
    name_len = strlen(policy_map_name);
    if ((0 >= name_len) || (sizeof(policy_map_p->name) <= name_len))
    {
        return RULE_TYPE_FAIL; /* bad name length */
    }

    if (NULL != strpbrk(policy_map_name, RULE_TYPE_INVALIDATE_CHAR_OF_DS_NAME))
    {
        return RULE_TYPE_E_INVALID_CHARACTER;
    }

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    /* check policy_map_name */
    if (RULE_TYPE_OK == RULE_OM_LocalSearchPolicyMapByName(policy_map_name, &found_index))
    {
        if (found_index != policy_map_index)
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_POLICY_MAP_EXISTED); /* policy-map name can't duplicate */

        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK); /* new name == original name */
    }

    memcpy(policy_map_p->name, policy_map_name, SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
    policy_map_p->name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0'; /* force to end a string */
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPolicyMapDescById
 *------------------------------------------------------------------------------
 * PURPOSE:  set policy map description by it's index
 * INPUT:    policy_map_index, policy_map_desc
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetPolicyMapDescById(UI32_T policy_map_index, const char *policy_map_desc)
{
    UI32_T                      desc_len;
    RULE_TYPE_PolicyMap_T       *policy_map_p;

    if (NULL == policy_map_desc)
        return RULE_TYPE_FAIL;

    /* check desc length (desc can be a empty string)
     */
    desc_len = strlen(policy_map_desc);
    if (sizeof(policy_map_p->desc) <= desc_len)
    {
        return RULE_TYPE_FAIL;
    }

    if (NULL != strpbrk(policy_map_desc, RULE_TYPE_INVALIDATE_CHAR_OF_DS_DESC))
    {
        return RULE_TYPE_E_INVALID_CHARACTER;
    }

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    memcpy(policy_map_p->desc, policy_map_desc, SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH);
    policy_map_p->desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH] = '\0'; /* force to end a string */

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPolicyMapRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set policy map row status by it's index
 * INPUT:    policy_map_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetPolicyMapRowStatus(UI32_T policy_map_index, BOOL_T is_active)
{
    RULE_TYPE_PolicyMap_T    *policy_map_p;

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (TRUE == is_active)
    {
        RULE_OM_SET_ENTRY_ACTIVE(policy_map_p->row_status);
    }
    else
    {
        RULE_OM_SET_ENTRY_NOTREADY(policy_map_p->row_status);
    }
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapByIndex(UI32_T policy_map_index, RULE_TYPE_PolicyMap_T *policy_map)
{
    RULE_TYPE_PolicyMap_T       *policy_map_p;

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    memcpy(policy_map, policy_map_p, sizeof(RULE_TYPE_PolicyMap_T));
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DeletePolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete PolicyMap Element
 * INPUT:    policy_map_index, class_map_index
 * OUTPUT:   None.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DeletePolicyMapElement(UI32_T policy_map_index, UI32_T class_map_index)
{
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;
    UI32_T                      element_precedence;
    RULE_TYPE_RETURN_TYPE_T     ret_code;

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (RULE_TYPE_OK != RULE_OM_LocalGetPolicyMapElementPrecedence(policy_map_index, class_map_index, &element_precedence))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    policy_element_p = RULE_OM_LocalGetPolicyMapElement(policy_map_index, class_map_index);
    if (NULL == policy_element_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    RULE_OM_LocalFreeMeter(policy_element_p->meter_index);
    RULE_OM_LocalFreeAction(policy_element_p->action_index);

    /* reset policy element */
    RULE_OM_SET_ENTRY_DESTROY(policy_element_p->row_status);
    policy_element_p->meter_index = 0;
    policy_element_p->action_index = 0;
    policy_element_p->class_map_index = 0;

    ret_code = RULE_OM_LocalRemoveAndCompactPolicyMapElement(policy_map_p, element_precedence);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextElementFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next element of policy map
 * INPUT:    policy_map_index, element_entry->precedence
 * OUTPUT:   element_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: policy_map_index, element_entry->precedence
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextElementFromPolicyMap(UI32_T policy_map_index, RULE_TYPE_UI_PolicyElement_T *element_entry)
{
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;
    UI32_T                      next_precedence;

    if (NULL == element_entry)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    next_precedence = element_entry->precedence + 1;

    /* precedence is 1-based index */
    if ((0 >= next_precedence) || (next_precedence > policy_map_p->element_count))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* there is no next precedence */

    policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(
                        policy_map_p->policy_map_element_index_list[next_precedence - 1]); /* to zero-based */

    if (NULL == policy_element_p)
    {
        /* should not go here */
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */
    }

    if (RULE_TYPE_OK != RULE_OM_LocalConvert2UIPolicyMapElement(policy_element_p, element_entry))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    element_entry->precedence = next_precedence;
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_CreatePolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  create policy-map element on specified index
 * INPUT:    policy_element_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_CreatePolicyMapElement(UI32_T policy_element_index, BOOL_T is_active)
{
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    if (RULE_OM_IS_BAD_POLICY_ELEMENT_IDX(policy_element_index))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    RULE_OM_EnterCriticalSection();
    policy_element_p = &shmem_data_p->policy_map_element_table[policy_element_index - 1]; /* to zero-based array index */
    if (RULE_OM_IS_ENTRY_AVAILABLE(policy_element_p->row_status))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* already created */

    memset(policy_element_p, 0, sizeof(RULE_TYPE_PolicyElement_T));

    if (TRUE == is_active)
    {
        RULE_OM_SET_ENTRY_ACTIVE(policy_element_p->row_status);
    }
    else
    {
        RULE_OM_SET_ENTRY_NOTREADY(policy_element_p->row_status);
    }
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DestroyPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  destroy policy-map element by index
 * INPUT:    policy_element_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     rule mgr SHALL remove element from policy-map first
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DestroyPolicyMapElement(UI32_T policy_element_index)
{
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    RULE_OM_EnterCriticalSection();
    policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_element_index);
    if (NULL == policy_element_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* not exist */

    RULE_OM_LocalFreePolicyMapElement (policy_element_index);
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  set policy-map element by index
 * INPUT:    policy_element_index, policy_element
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetPolicyMapElement(UI32_T policy_element_index, const RULE_TYPE_PolicyElement_T *policy_element)
{
    RULE_TYPE_PolicyElement_T   *policy_element_p;
    BOOL_T                      change_to_not_ready = FALSE;;

    if (NULL == policy_element)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_element_index);
    if (NULL == policy_element_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    /* class-map index has been modified, need to check class-map */
    if (policy_element_p->class_map_index != policy_element->class_map_index)
    {
        if (0 != policy_element->class_map_index)
        {
            RULE_TYPE_ClassMap_T    *class_map_p;

            class_map_p = RULE_OM_LocalGetClassMapByIndex(policy_element->class_map_index);
            if (NULL == class_map_p)
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

            if (FALSE == RULE_OM_IS_ENTRY_ACTIVE(class_map_p->row_status))
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* do not allow to associate with non-active entry */
        }
        else
        {
            change_to_not_ready = TRUE; /* an active policy-map element SHALL have a valid class-map index */
        }

        policy_element_p->class_map_index = policy_element->class_map_index;
    }

    /* meter index has been modified, need to check meter */
    if (policy_element_p->meter_index != policy_element->meter_index)
    {
        if (0 != policy_element->meter_index)
        {
            RULE_TYPE_TBParamEntry_T    *meter_p;
            UI32_T                      found_index;

            meter_p = RULE_OM_LocalGetMeterEntryByIndex(policy_element->meter_index);
            if (NULL == meter_p)
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

            if (FALSE == RULE_OM_IS_ENTRY_ACTIVE(meter_p->row_status))
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* do not allow to associate with non-active entry */

            if (RULE_TYPE_OK == RULE_OM_LocalGetPolicyMapElementIndexByMeter(policy_element->meter_index, &found_index))
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* this element is already own by some one */
        }

        policy_element_p->meter_index = policy_element->meter_index;
    }

    /* action index has been modified, need to check action */
    if (policy_element_p->action_index != policy_element->action_index)
    {
        if (0 != policy_element->action_index)
        {
            RULE_TYPE_Action_T    *action_p;
            UI32_T                found_index;

            action_p = RULE_OM_LocalGetActionEntryByIndex(policy_element->action_index);
            if (NULL == action_p)
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK); /* not exist */

            if (FALSE == RULE_OM_IS_ENTRY_ACTIVE(action_p->row_status))
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* do not allow to associate with non-active entry */

            if (RULE_TYPE_OK == RULE_OM_LocalGetPolicyMapElementIndexByAction(policy_element->action_index, &found_index))
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* this element is already own by some one */
        }
        else
        {
            change_to_not_ready = TRUE; /* an active policy-map element SHALL have a valid action index */
        }

        policy_element_p->action_index = policy_element->action_index;
    }

    if (TRUE == change_to_not_ready)
    {
        RULE_OM_SET_ENTRY_NOTREADY(policy_element_p->row_status);
    }
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPolicyMapElementRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set policy-map element row status by it's index
 * INPUT:    policy_element_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetPolicyMapElementRowStatus(UI32_T policy_element_index, BOOL_T is_active)
{
    RULE_TYPE_PolicyElement_T  *policy_element_p;
    RULE_TYPE_Action_T         action_entry;
    RULE_TYPE_TBParamEntry_T   meter_entry;

    policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_element_index);
    if (NULL == policy_element_p)
    {
        LOG("Error policy_element_index=%lu", policy_element_index);
        return RULE_TYPE_FAIL;
    }

    if (TRUE == is_active)
    {
        UI32_T set_action_bitmap;

        if(FALSE == RULE_OM_GetActionEntryByIndex(policy_element_p->action_index, &action_entry))
        {
            LOG("Error action_index=%lu", policy_element_p->action_index);
            return RULE_TYPE_FAIL;
        }

        set_action_bitmap = action_entry.action_bitmap & RULE_TYPE_IN_PROFILE_ACTION;
        if (set_action_bitmap != 0
            && set_action_bitmap != RULE_TYPE_ACTION_PKT_NEW_PRI
            && set_action_bitmap != RULE_TYPE_ACTION_PKT_NEW_TOS
            && set_action_bitmap != RULE_TYPE_ACTION_PKT_NEW_DSCP
            && set_action_bitmap != RULE_TYPE_ACTION_PKT_NEW_PHB)
        {
            LOG("'set {cos | ip dscp | phb | ...}' action shall only has one");
            return RULE_TYPE_FAIL;
        }

        if (policy_element_p->meter_index != 0)
        {
            if(FALSE == RULE_OM_GetMeterEntryByIndex(policy_element_p->meter_index, &meter_entry))
            {
                LOG("Error meter_index=%lu", policy_element_p->meter_index);
                return RULE_TYPE_FAIL;
            }

            switch (meter_entry.meter_model)
            {
                case RULE_TYPE_METER_MODE_DEFAULT:
                case RULE_TYPE_METER_MODE_FLOW:
                    if (action_entry.action_bitmap & RULE_TYPE_YELLOW_ACTION)
                    {
                        LOG("Shall not set yellow action");
                        return RULE_TYPE_FAIL;
                    }
                    break;

#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
                case RULE_TYPE_METER_MODE_RATE:
                    if (action_entry.action_bitmap & RULE_TYPE_OUT_OF_PROFILE_ACTION)
                    {
                        LOG("Shall not set out_of_profile action");
                        return RULE_TYPE_FAIL;
                    }
                    break;
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

                case RULE_TYPE_METER_MODE_TRTCM_COLOR_BLIND:
                case RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE:
                case RULE_TYPE_METER_MODE_SRTCM_COLOR_BLIND:
                case RULE_TYPE_METER_MODE_SRTCM_COLOR_AWARE:
                    if (!(action_entry.action_bitmap & RULE_TYPE_YELLOW_ACTION))
                    {
                        LOG("Shall set yellow action");
                        return RULE_TYPE_FAIL;
                    }
                    break;

                default:
                    LOG("Undefined meter_mode");
                    return RULE_TYPE_FAIL;
            }
        }

        RULE_OM_SET_ENTRY_ACTIVE(policy_element_p->row_status);
    }
    else
    {
        RULE_OM_SET_ENTRY_NOTREADY(policy_element_p->row_status);
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  get policy-map element by index
 * INPUT:    policy_element_index
 * OUTPUT:   policy_element
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapElement(UI32_T policy_element_index, RULE_TYPE_PolicyElement_T *policy_element)
{
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    if (NULL == policy_element)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_element_index);
    if (NULL == policy_element_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    memcpy(policy_element, policy_element_p, sizeof(RULE_TYPE_PolicyElement_T));
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapElementFromPolicyMapByClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  get policy-map element from specified policy-map by class-map index
 * INPUT:    policy_map_index, class_map_index
 * OUTPUT:   policy_element
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapElementFromPolicyMapByClassMap(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_PolicyElement_T *policy_element)
{
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    UI32_T                      index;
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    if (NULL == policy_element)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    for (index = 0; index < policy_map_p->element_count; ++index)
    {
        policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_map_p->policy_map_element_index_list[index]);
        if (NULL == policy_element_p)
            continue; /* should not go here */

        if (policy_element_p->class_map_index != class_map_index)
            continue;

        /* found */
        memcpy(policy_element, policy_element_p, sizeof(RULE_TYPE_PolicyElement_T));
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_GetPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE  : get policy-map element by policy_map_index and class_map_index
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetPolicyMapElementEntry(
    UI32_T policy_map_index,
    UI32_T class_map_index,
    RULE_TYPE_PolicyElement_T *entry_p)
{
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    RULE_OM_EnterCriticalSection();

    policy_element_p = RULE_OM_LocalGetPolicyMapElement(policy_map_index, class_map_index);
    if (NULL == policy_element_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    memcpy(entry_p, policy_element_p, sizeof(*entry_p));

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapElementIndexByMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  get index of policy-map element by meter index
 * INPUT:    meter_index
 * OUTPUT:   policy_element_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     return OK only if we can find a policy-map element index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapElementIndexByMeter(UI32_T meter_index, UI32_T *policy_element_index)
{
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    RULE_OM_EnterCriticalSection();
    ret_code = RULE_OM_LocalGetPolicyMapElementIndexByMeter(meter_index, policy_element_index);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapElementIndexByAction
 *------------------------------------------------------------------------------
 * PURPOSE:  get index of policy-map element by action index
 * INPUT:    action_index
 * OUTPUT:   policy_element_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapElementIndexByAction(UI32_T action_index, UI32_T *policy_element_index)
{
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    RULE_OM_EnterCriticalSection();
    ret_code = RULE_OM_LocalGetPolicyMapElementIndexByAction(action_index, policy_element_index);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapElementRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  get policy map element row status by it's index
 * INPUT:    policy_element_index
 * OUTPUT:   row_status
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapElementRowStatus(UI32_T policy_element_index, UI32_T *row_status)
{
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    if (NULL == row_status)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_element_index);
    if (NULL == policy_element_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    *row_status = policy_element_p->row_status;
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  get policy-map element by index
 * INPUT:    policy_element_index
 * OUTPUT:   policy_element_index, policy_element
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextPolicyMapElement(UI32_T *policy_element_index, RULE_TYPE_PolicyElement_T *policy_element)
{
    UI32_T                      index;

    if ((NULL == policy_element_index) || (NULL == policy_element))
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    for (index = *policy_element_index; SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS > index; ++index)
    {
        if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->policy_map_element_table[index].row_status))
            continue;

        *policy_element_index = index + 1; /* to 1-based */
        memcpy(policy_element, &shmem_data_p->policy_map_element_table[index], sizeof(RULE_TYPE_PolicyElement_T));
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map_index, class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextClassMap(UI32_T *class_map_index, RULE_TYPE_ClassMap_T *class_map)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    UI32_T                  next_class_map_index;

    if ((NULL == class_map_index) || (NULL == class_map))
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    for (next_class_map_index = *class_map_index + 1;
        SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS_MAP >= next_class_map_index; ++next_class_map_index)
    {
        class_map_p = &shmem_data_p->class_map_table[next_class_map_index - 1]; /* to zero-based array index */

        if (RULE_OM_IS_ENTRY_AVAILABLE(class_map_p->row_status))
        {
            *class_map_index = next_class_map_index;
            memcpy(class_map, class_map_p, sizeof(RULE_TYPE_ClassMap_T));
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
        }
    }
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextMibClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next mib class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map_index, class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextMibClassMap(UI32_T *class_map_index, RULE_TYPE_MIB_ClassMap_T *class_map)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    UI32_T                  next_class_map_index;
    RULE_TYPE_RETURN_TYPE_T ret_code;
    if ((NULL == class_map_index) || (NULL == class_map))
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    for (next_class_map_index = *class_map_index + 1;
        SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS_MAP >= next_class_map_index; ++next_class_map_index)
    {
        class_map_p = &shmem_data_p->class_map_table[next_class_map_index - 1]; /* to zero-based array index */

        if (RULE_OM_IS_ENTRY_AVAILABLE(class_map_p->row_status))
        {
            *class_map_index = next_class_map_index;
            ret_code = RULE_OM_LocalConvert2MibClassMap(class_map_p, class_map);
            RULE_OM_LeaveCriticalSectionReturnState(ret_code);
        }
    }
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUIClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map_index, class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextUIClassMap(UI32_T *class_map_index, RULE_TYPE_UI_ClassMap_T *class_map)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    UI32_T                  next_class_map_index;
    RULE_TYPE_RETURN_TYPE_T ret_code;

    if ((NULL == class_map_index) || (NULL == class_map))
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    for (next_class_map_index = *class_map_index + 1;
        SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS_MAP >= next_class_map_index; ++next_class_map_index)
    {
        class_map_p = &shmem_data_p->class_map_table[next_class_map_index - 1]; /* to zero-based array index */

        if (RULE_OM_IS_ENTRY_AVAILABLE(class_map_p->row_status))
        {
            *class_map_index = next_class_map_index;
            ret_code = RULE_OM_LocalConvert2UIClassMap(class_map_p, class_map);
            RULE_OM_LeaveCriticalSectionReturnState(ret_code);
        }
    }
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMapNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get name of class map by it's index.
 * INPUT:    class_map_id
 * OUTPUT:   class_map_name.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMapNameById(UI32_T class_map_id, char *class_map_name)
{
    RULE_TYPE_ClassMap_T    *class_map_p;

    if (NULL == class_map_name)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_id);

    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    memcpy(class_map_name, class_map_p->class_map_name, SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
    class_map_p->class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0'; /* force to end a string */
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMapRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  get class-map row status by it's index
 * INPUT:    class_map_index
 * OUTPUT:   row_status
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMapRowStatus(UI32_T class_map_index, UI32_T *row_status)
{
    RULE_TYPE_ClassMap_T       *class_map_p;

    if (NULL == row_status)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    *row_status = class_map_p->row_status;
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_CreateClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  create class-map on specified index
 * INPUT:    class_map_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_CreateClassMap(UI32_T class_map_index, BOOL_T is_active)
{
#define MAX_UI32_STR_LEN        10  /* 0xffffffff */

    RULE_TYPE_ClassMap_T       *class_map_p;
    UI32_T                      found_index;
    char                        buf[MAX_UI32_STR_LEN + 1] = {0};

    if (RULE_OM_IS_BAD_CLASS_MAP_IDX(class_map_index))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    RULE_OM_EnterCriticalSection();
    class_map_p = &shmem_data_p->class_map_table[class_map_index - 1]; /* to zero-based array index */
    if (RULE_OM_IS_ENTRY_AVAILABLE(class_map_p->row_status))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* already created */

    memset(class_map_p, 0, sizeof(RULE_TYPE_ClassMap_T));

    /* default name == index */
    sprintf(buf, "%lu", class_map_index);
    strncpy(class_map_p->class_map_name, buf, sizeof(class_map_p->class_map_name)-1);
    class_map_p->class_map_name[ sizeof(class_map_p->class_map_name)-1 ] = '\0';

    /* default match-type */
    class_map_p->class_map_match_type = RULE_TYPE_CLASS_MAP_MATCH_ANY;

    /* default name should not duplicate */
    if (RULE_TYPE_OK == RULE_OM_LocalGetClassMapIndexByName(class_map_p->class_map_name, &found_index))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* the default name is already used by others */

    if (TRUE == is_active)
    {
        RULE_OM_SET_ENTRY_ACTIVE(class_map_p->row_status);
    }
    else
    {
        RULE_OM_SET_ENTRY_NOTREADY(class_map_p->row_status);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);

#undef MAX_UI32_STR_LEN
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DestroyClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : destroy class-map by index
 * INPUT    : class_map_index
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : 1, rule mgr SHALL remove class-map from policy-map element first
 *            2, didn't destroy its elements (MF or ACL)
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DestroyClassMap(UI32_T class_map_index)
{
    RULE_TYPE_ClassMap_T        *class_map_p;

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* not exist */

    memset(class_map_p, 0, sizeof(RULE_TYPE_ClassMap_T));
    RULE_OM_SET_ENTRY_DESTROY(class_map_p->row_status);
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a class map entry by it's index
 * INPUT:    class_map_id, class_map
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetClassMap(UI32_T class_map_index, RULE_TYPE_ClassMap_T *class_map)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    UI32_T                  found_index;
    UI32_T                  name_len;
    UI32_T                  desc_len;

    if (NULL == class_map)
        return RULE_TYPE_FAIL;

    /* check match type
     */
    if (RULE_OM_IS_BAD_MATCH_TYPE(class_map->class_map_match_type))
        return RULE_TYPE_FAIL;

    /* check name
     */
    name_len = strlen(class_map->class_map_name);
    if ((0 >= name_len) || (sizeof(class_map->class_map_name) <= name_len))
    {
        return RULE_TYPE_FAIL;
    }

    if (NULL != strpbrk(class_map->class_map_name, RULE_TYPE_INVALIDATE_CHAR_OF_DS_NAME))
    {
        return RULE_TYPE_E_INVALID_CHARACTER;
    }

    /* check description
     */
    desc_len = strlen(class_map->desc);
    if (sizeof(class_map->desc) <= desc_len)
    {
        return RULE_TYPE_FAIL;
    }

    if (NULL != strpbrk(class_map->desc, RULE_TYPE_INVALIDATE_CHAR_OF_DS_DESC))
    {
        return RULE_TYPE_E_INVALID_CHARACTER;
    }

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    /* check class_map_name */
    if (RULE_TYPE_OK == RULE_OM_LocalGetClassMapIndexByName(class_map->class_map_name, &found_index))
    {
        if (found_index != class_map_index)
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* class-map name can't duplicate */
    }

    memcpy(class_map_p->class_map_name, class_map->class_map_name, SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
    memcpy(class_map_p->desc, class_map->desc, SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH);
    class_map_p->desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH] = '\0'; /* force to end a string */

    class_map_p->class_map_match_type = class_map->class_map_match_type;
    class_map_p->row_status = class_map->row_status; /* copy row status ? */
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetUIClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a class map entry by it's index
 * INPUT:    class_map_id, class_map
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetUIClassMap(UI32_T class_map_index, const RULE_TYPE_UI_ClassMap_T *class_map)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    UI32_T                  found_index;
    UI32_T                  name_len;
    UI32_T                  desc_len;

    if (NULL == class_map)
        return RULE_TYPE_FAIL;

    if (RULE_OM_IS_BAD_MATCH_TYPE(class_map->class_map_match_type))
        return RULE_TYPE_INVALID_PARAMETER;

    /* check name
     */
    name_len = strlen(class_map->class_map_name);
    if ((0 >= name_len) || (sizeof(class_map->class_map_name) <= name_len))
    {
        return RULE_TYPE_FAIL; /* bad name length */
    }

    if (NULL != strpbrk(class_map->class_map_name, RULE_TYPE_INVALIDATE_CHAR_OF_DS_NAME))
    {
        return RULE_TYPE_E_INVALID_CHARACTER;
    }

    /* check description
     */
    desc_len = strlen(class_map->desc);
    if (sizeof(class_map->desc) <= desc_len)
    {
        return RULE_TYPE_FAIL;
    }

    if (NULL != strpbrk(class_map->desc, RULE_TYPE_INVALIDATE_CHAR_OF_DS_DESC))
    {
        return RULE_TYPE_E_INVALID_CHARACTER;
    }

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    /* check class_map_name */
    if (RULE_TYPE_OK == RULE_OM_LocalGetClassMapIndexByName(class_map->class_map_name, &found_index))
    {
        if (found_index != class_map_index)
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* class-map name can't duplicate */
    }

    memcpy(class_map_p->class_map_name, class_map->class_map_name, SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
    class_map_p->class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0'; /* force to end a string */

    memcpy(class_map_p->desc, class_map->desc,SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH);
    class_map_p->desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH] = '\0'; /* force to end a string */

    class_map_p->class_map_match_type = class_map->class_map_match_type;
    /* class_map_p->row_status = class_map->row_status; do not copy row status */
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetClassMapNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  set class map name by it's index
 * INPUT:    class_map_index, class_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetClassMapNameById(UI32_T class_map_index, const char *class_map_name)
{
    UI32_T                      name_len, found_index;
    RULE_TYPE_ClassMap_T        *class_map_p;
/*
    UI8_T                   policy_map_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_POLICY_MAP_LIST];
    UI32_T                  policy_map_nbr = 0;
*/
    if (NULL == class_map_name)
        return RULE_TYPE_FAIL;

    name_len = strlen(class_map_name);
    if ((0 >= name_len) || (SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH < name_len))
    {
        return RULE_TYPE_FAIL;
    }

    if (NULL != strpbrk(class_map_name, RULE_TYPE_INVALIDATE_CHAR_OF_DS_NAME))
    {
        return RULE_TYPE_E_INVALID_CHARACTER;
    }

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

/* Can not change class-map name when it have be bound to a policy-map.
   We should not have this limitation
    if (RULE_TYPE_OK == RULE_OM_Get_PolicyMapListByClassMap(class_map_index, policy_map_list, &policy_map_nbr))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
*/
    /* check class_map_name */
    if (RULE_TYPE_OK == RULE_OM_LocalGetClassMapIndexByName(class_map_name, &found_index))
    {
        if (found_index != class_map_index)
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_CLASS_MAP_EXISTED); /* class-map name can't duplicate */

        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK); /* new name == original name */
    }

    /* check name length */
    name_len = strlen((char *)class_map_name);
    if ((0 >= name_len) || (SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH < name_len))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad name length */

    memcpy(class_map_p->class_map_name, class_map_name, SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
    class_map_p->class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0'; /* force to end a string */
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetClassMapDescById
 *------------------------------------------------------------------------------
 * PURPOSE:  set class map description by it's index
 * INPUT:    class_map_index, class_map_desc
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetClassMapDescById(UI32_T class_map_index, const char *class_map_desc)
{
    UI32_T                      desc_len;
    RULE_TYPE_ClassMap_T        *class_map_p;

    if (NULL == class_map_desc)
        return RULE_TYPE_FAIL;

    /* check desc length (desc can be a empty string)
     */
    desc_len = strlen(class_map_desc);
    if (sizeof(class_map_p->desc) <= desc_len)
    {
        return RULE_TYPE_FAIL;
    }

    if (NULL != strpbrk(class_map_desc, RULE_TYPE_INVALIDATE_CHAR_OF_DS_DESC))
    {
        return RULE_TYPE_E_INVALID_CHARACTER;
    }

    RULE_OM_EnterCriticalSection();

    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    memcpy(class_map_p->desc, class_map_desc, SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH);
    class_map_p->desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH] = '\0'; /* force to end a string */

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetClassMapRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set class map row status by index
 * INPUT:    class_map_index, is_active
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetClassMapRowStatus(UI32_T class_map_index, BOOL_T is_active)
{
    RULE_TYPE_ClassMap_T        *class_map_p;

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (TRUE == is_active)
    {
        RULE_OM_SET_ENTRY_ACTIVE(class_map_p->row_status);
    }
    else
    {
        RULE_OM_SET_ENTRY_NOTREADY(class_map_p->row_status);
    }
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_BindElement2ClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : bind class-map element to class-map
 * INPUT    : class_map_index, class_type, class_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : class_index MAY be an ACL index or a MF index, depends on class_type
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_BindElement2ClassMap(UI32_T class_map_index, RULE_TYPE_ClassType_T class_type, UI32_T class_index)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    UI32_T                  found_index;

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (SYS_ADPT_DIFFSERV_MAX_CLASS_NBR_OF_CLASS_MAP <= class_map_p->class_element_count)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* class-map is full */

    /* check type and class index */
    switch (class_type)
    {
        case RULE_TYPE_CLASS_MF:
        {
            RULE_TYPE_Ace_Entry_T   *ace_p;
            RULE_TYPE_AclType_T      mf_type;

            ace_p = RULE_OM_LocalGetAceByIndex(class_index);

            if ((NULL == ace_p) ||
                (FALSE == RULE_OM_IS_ENTRY_ACTIVE(ace_p->row_status)) ||
                (RULE_TYPE_OK == RULE_OM_LocalGetClassMapIndexByMf(class_index, &found_index)) ||
                (RULE_TYPE_OK == RULE_OM_LocalGetAclIndexByAce(class_index, &found_index, NULL)) ||
                (RULE_TYPE_OK != RULE_OM_LocalClassifyMf(ace_p, &mf_type)))
            {
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
            }

            if (FALSE == RULE_OM_LocalIsValidElementTypeOflassMap(class_map_p, mf_type))
            {
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_NOT_ALLOW_MIX_SELECTOR);
            }

            break;
        }
        case RULE_TYPE_CLASS_ACL:
        {
            RULE_TYPE_Acl_T         *acl_p;
            RULE_TYPE_AclType_T     acl_type;

            acl_p = RULE_OM_LocalGetAclByIndex(class_index);

            if ((NULL == acl_p) ||
                (FALSE == RULE_OM_IS_ENTRY_ACTIVE(acl_p->row_status)) ||
                (RULE_TYPE_OK != RULE_OM_LocalClassifyAcl(acl_p, &acl_type)))
            {
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
            }

            if (FALSE == RULE_OM_LocalIsValidElementTypeOflassMap(class_map_p, acl_type))
            {
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_NOT_ALLOW_MIX_SELECTOR);
            }

            RULE_OM_SET_BIT_ON(acl_p->class_map_bit_list, (class_map_index - 1));
            acl_p->class_map_count++;

            break;
        }
        default:
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad type */
    }

    class_map_p->class_type[class_map_p->class_element_count] = class_type;
    class_map_p->class_index_list[class_map_p->class_element_count] = class_index;
    class_map_p->class_element_count++;
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_UnbindElementFromClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : unbind class-map element from class-map
 * INPUT    : class_map_index, class_type, class_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : class_index MAY be an ACL index or a MF index, depends on class_type
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_UnbindElementFromClassMap(UI32_T class_map_index, RULE_TYPE_ClassType_T class_type, UI32_T class_index)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    UI32_T                  element_precedence;

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (RULE_TYPE_OK != RULE_OM_LocalGetClassMapElementPrecedence(class_map_index, class_type, class_index, &element_precedence))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (RULE_TYPE_OK != RULE_OM_LocalRemoveAndCompactClassMapElement(class_map_p, element_precedence))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    if (RULE_TYPE_CLASS_ACL == class_type)
    {
        RULE_TYPE_Acl_T         *acl_p;

        acl_p = RULE_OM_LocalGetAclByIndex(class_index);
        if (NULL == acl_p)
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* should not go here */

        RULE_OM_SET_BIT_OFF(acl_p->class_map_bit_list, (class_map_index - 1));
        --(acl_p->class_map_count);
    }
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMapById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMapById(UI32_T class_map_index, RULE_TYPE_UI_ClassMap_T *class_map)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    if (NULL == class_map)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    ret_code = RULE_OM_LocalConvert2UIClassMap(class_map_p, class_map);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  get a class map by its index
 * INPUT:    class_map_index
 * OUTPUT:   class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMap(UI32_T class_map_index, RULE_TYPE_ClassMap_T *class_map)
{
    RULE_TYPE_ClassMap_T    *class_map_p;

    if (NULL == class_map)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    memcpy(class_map, class_map_p, sizeof(RULE_TYPE_ClassMap_T));
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetMibClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a mib class map by its index
 * INPUT:    class_map_index
 * OUTPUT:   class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetMibClassMap(UI32_T class_map_index, RULE_TYPE_MIB_ClassMap_T *class_map)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    UI32_T ret_code = RULE_TYPE_FAIL;

    if (NULL == class_map)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();

    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    ret_code = RULE_OM_LocalConvert2MibClassMap(class_map_p, class_map);

    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_CreateAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Create an ACL.
 * INPUT:    acl_name, acl_type
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_CreateAcl(
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type)
{
    UI32_T              acl_index, name_len;
    RULE_TYPE_Acl_T     *acl_p;
    UI32_T              ret;

    RULE_OM_EnterCriticalSection();
    if (RULE_TYPE_OK == RULE_OM_LocalGetAclIdByName(acl_name, &acl_index))
    {
        acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
        if (NULL == acl_p)
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

        if (acl_type != acl_p->acl_type)
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* wrong type */

        /* When users issue CLI command: accesss-list "acl-type" "name", CLI always invoke this API indirectly.
         * Therefore, return EXISTED, not FAIL to create.
         */
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_ACL_EXISTED);
    }

    /* check parameters */
    name_len = strlen((char *)acl_name);
    if ((0 >= name_len) || (SYS_ADPT_ACL_MAX_NAME_LEN < name_len))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    switch (acl_type)
    {
        case RULE_TYPE_MAC_ACL:
        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
        case RULE_TYPE_MF:
        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_DAI == TRUE)
        case RULE_TYPE_ARP_ACL:
#endif
            break;

        default:
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    ret = RULE_OM_LocalAllocateAclWithRange(0, SYS_ADPT_MAX_NBRS_OF_ACL, &acl_index);
    if (ret != RULE_TYPE_OK)
    {
        RULE_OM_LeaveCriticalSectionReturnState(ret);
    }

    acl_p = &shmem_data_p->acl_table[acl_index - 1]; /* to zero-based array index */

    acl_p->acl_type = acl_type;
    RULE_OM_SET_ENTRY_ACTIVE(acl_p->row_status);

    strncpy(acl_p->acl_name, acl_name, SYS_ADPT_ACL_MAX_NAME_LEN);
    acl_p->acl_name[SYS_ADPT_ACL_MAX_NAME_LEN] = '\0'; /* force to end a string */

    acl_p->super.type = RULE_TYPE_INST_OM_USER_ACL;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_CreateAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  create acl on specified index
 * INPUT:    acl_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_CreateAclByIndex(UI32_T acl_index, BOOL_T is_active)
{
#define MAX_UI32_STR_LEN        10  /* 0xffffffff */

    RULE_TYPE_Acl_T     *acl_p;
    UI32_T              found_index;
    char                buf[MAX_UI32_STR_LEN + 1];

    if (RULE_OM_IS_BAD_ACL_IDX(acl_index))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    RULE_OM_EnterCriticalSection();
    acl_p = &shmem_data_p->acl_table[acl_index - 1]; /* to zero-based array index */
    if (RULE_OM_IS_ENTRY_AVAILABLE(acl_p->row_status))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_ACL_EXISTED);

    memset(acl_p, 0, sizeof(RULE_TYPE_Acl_T));

    /* default name == index */
    sprintf(buf, "%lu", acl_index);
    strncpy(acl_p->acl_name, buf, sizeof(acl_p->acl_name)-1);
    acl_p->acl_name[ sizeof(acl_p->acl_name)-1 ] = '\0';

    if (RULE_TYPE_OK == RULE_OM_LocalGetAclIdByName(acl_p->acl_name, &found_index))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_ACL_EXISTED); /* the default name is already used by others */

    acl_p->acl_type = RULE_TYPE_MAC_ACL; /* default ACL type is MAC */

    if (TRUE == is_active)
    {
        RULE_OM_SET_ENTRY_ACTIVE(acl_p->row_status);
    }
    else
    {
        RULE_OM_SET_ENTRY_NOTREADY(acl_p->row_status);
    }

    acl_p->super.type = RULE_TYPE_INST_OM_USER_ACL;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);

#undef MAX_UI32_STR_LEN
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DestroyAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : destroy acl by index
 * INPUT    : acl_index
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : 1, rule mgr SHALL remove acl from class-map and unbind ports first
 *            2, didn't destroy its elements (ACEs)
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DestroyAcl(UI32_T acl_index)
{
    RULE_TYPE_Acl_T     *acl_p;

    RULE_OM_EnterCriticalSection();
    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* not exist */

    RULE_OM_LocalFreeAcl(acl_p);
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetAclTypeByIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : set acl type by it's index
 * INPUT    : acl_index, acl_type
 * OUTPUT   : none.
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : 1, rule mgr SHALL remove acl from class-map and unbind ports first
 *            2, if acl type is changed, its ace list will be clear (unbind --
 *               all ACE(s) no longer belong to this ACL)
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetAclTypeByIndex(UI32_T acl_index, RULE_TYPE_AclType_T acl_type)
{
    RULE_TYPE_Acl_T         *acl_p;

    RULE_OM_EnterCriticalSection();
    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    switch (acl_type)
    {
        case RULE_TYPE_MAC_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:

#if (SYS_CPNT_DAI == TRUE)
        case RULE_TYPE_ARP_ACL:
#endif /* #if (SYS_CPNT_DAI == TRUE) */

            break;
        default:
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* wrong type */
    }

    memset(&acl_p->super.links.first_node, 0, sizeof(acl_p->super.links.first_node));
    memset(&acl_p->super.links.last_node, 0, sizeof(acl_p->super.links.last_node));

    acl_p->acl_type = acl_type;
    acl_p->ace_count = 0;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetAclNameByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  set acl name by it's index
 * INPUT:    acl_index, acl_name
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetAclNameByIndex(UI32_T acl_index, const char *acl_name)
{
    UI32_T                      name_len, found_index;
    RULE_TYPE_Acl_T             *acl_p;

    if (NULL == acl_name)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    /* check acl_name */
    if (RULE_TYPE_OK == RULE_OM_LocalGetAclIdByName(acl_name, &found_index))
    {
        if (found_index != acl_index)
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* acl name can't duplicate */

        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK); /* new name == original name */
    }

    /* check name length */
    name_len = strlen((char *)acl_name);
    if ((0 >= name_len) || (SYS_ADPT_ACL_MAX_NAME_LEN < name_len))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad name length */

    memcpy(acl_p->acl_name, acl_name, SYS_ADPT_ACL_MAX_NAME_LEN);
    acl_p->acl_name[SYS_ADPT_ACL_MAX_NAME_LEN] = '\0'; /* force to end a string */

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetAclRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set acl row status by index
 * INPUT:    acl_index, is_active
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetAclRowStatus(UI32_T acl_index, BOOL_T is_active)
{
    RULE_TYPE_Acl_T             *acl_p;

    RULE_OM_EnterCriticalSection();
    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (TRUE == is_active)
    {
        RULE_OM_SET_ENTRY_ACTIVE(acl_p->row_status);
    }
    else
    {
        RULE_OM_SET_ENTRY_NOTREADY(acl_p->row_status);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_BindElement2Acl
 *------------------------------------------------------------------------------
 * PURPOSE  : bind an ace to an acl
 * INPUT    : acl_index, ace_type, ace_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_BindElement2Acl(UI32_T acl_index, RULE_TYPE_AclType_T ace_type, UI32_T ace_index)
{
    RULE_TYPE_Acl_T         *acl_p;
    RULE_TYPE_Ace_Entry_T   *ace_p;
    RULE_TYPE_AclType_T     acl_type;
    UI32_T                  found_index;

    RULE_OM_EnterCriticalSection();
    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if ((NULL == acl_p) ||
        (RULE_TYPE_OK != RULE_OM_LocalClassifyAcl(acl_p, &acl_type)))
    {
        LOG("acl_p is null or RULE_OM_LocalClassifyAcl failed.");
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    ace_p = RULE_OM_LocalGetAceByIndex(ace_index);
    if ((NULL == ace_p) ||
        (FALSE == RULE_OM_Is_AclType_Matched (ace_type, ace_p->ace_type)) ||
        (FALSE == RULE_OM_IS_ENTRY_ACTIVE(ace_p->row_status)))
    {
        LOG("ace_p is null or wrong ace_type or ace is not active.");
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    /* Let RULE_TYPE_IPV6_ACL match with RULE_TYPE_IPV6_STD_ACL or RULE_TYPE_IPV6_EXT_ACL */
    if (FALSE == RULE_OM_Is_AclType_Matched (acl_type, ace_type))
    {
        LOG("acl_type and ace_type are not matched.");
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* wrong type */
    }

    if (SYS_ADPT_MAX_NBRS_OF_ACE <= acl_p->ace_count)
    {
        LOG("acl is full.");
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* acl is full */
    }

    if (RULE_TYPE_OK != RULE_OM_LocalCheckAceCollisionToAcl(ace_p, acl_p))
    {
        LOG("RULE_OM_LocalCheckAceCollisionToAcl failed.");
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    if (RULE_TYPE_OK == RULE_OM_LocalGetAclIndexByAce(ace_index, &found_index, NULL))
    {
        if (found_index == acl_index)
        {
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);    /* this element is already owned by the same acl */
        }
        else
        {
            LOG("ace <%ld> is owned by another acl <%ld>.", ace_index, found_index);

            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);  /* this element is already owned by another acl  */
        }
    }

    if (RULE_TYPE_OK == RULE_OM_LocalGetClassMapIndexByMf(ace_index, &found_index))
    {
        LOG("ace is owned by a class-map.");
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);      /* this element is already owned by a class-map */
    }

    RULE_OM_LL_LocalInsertEnd(RULE_OM_INST2PTR(acl_p), RULE_OM_INST2PTR(ace_p));
    ++(acl_p->ace_count);

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_UnbindElementFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : unbind an ace from an acl
 * INPUT    : acl_index, ace_type, ace_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_UnbindElementFromAcl(
    UI32_T acl_index,
    RULE_TYPE_AclType_T ace_type,
    UI32_T ace_index)
{
    RULE_TYPE_Acl_T         *acl_p;
    RULE_TYPE_Ace_Entry_T   *ace_p;

    RULE_OM_EnterCriticalSection();
    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
    {
        LOG("acl_p is null or RULE_OM_LocalClassifyAcl failed.");

        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    ace_p = RULE_OM_LocalGetAceByIndex(ace_index);
    if ((NULL == ace_p) ||
        (FALSE == RULE_OM_LocalIsAclTypeMatched (ace_type, ace_p->ace_type)) ||
        (FALSE == RULE_OM_IS_ENTRY_ACTIVE(ace_p->row_status)))
    {
        LOG("ace_p is null or wrong ace_type or ace is not active.");

        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    if (RULE_TYPE_OK != RULE_OM_LocalIsAclHaveAce(acl_p, ace_index))
    {
        LOG("ace <%ld> is not owned by acl <%ld>.", ace_index, acl_index);

        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */
    }

    RULE_OM_LL_LocalRemove(RULE_OM_INST2PTR(acl_p), RULE_OM_INST2PTR(ace_p));

    ASSERT(0 < acl_p->ace_count);

    --(acl_p->ace_count);

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DelAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete an ACL.
 * INPUT:    acl_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DelAcl(UI32_T acl_index)
{
    UI32_T  result;

    RULE_OM_EnterCriticalSection();
    result = RULE_OM_LocalDelAcl(acl_index);
    RULE_OM_LeaveCriticalSectionReturnState(result);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get ACL by acl_index
 * INPUT:    acl_index
 * OUTPUT:   acl_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetAclByIndex(UI32_T acl_index, RULE_TYPE_Acl_T *acl_entry)
{
    RULE_TYPE_Acl_T     *acl_p;

    if (NULL == acl_entry)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();

    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    memcpy(acl_entry, acl_p, sizeof(RULE_TYPE_Acl_T));

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetMibAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get MIB ACL by acl_index
 * INPUT:    acl_index
 * OUTPUT:   acl_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetMibAclByIndex(UI32_T acl_index, RULE_TYPE_MIB_Acl_T *acl_entry)
{
    RULE_TYPE_Acl_T          *acl_p;
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    if (NULL == acl_entry)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();

    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) || (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
    if (TRUE == acl_p->flag_hide)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
#endif /* #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) || (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE) */

    ret_code = RULE_OM_LocalConvert2MibAcl(acl_p, acl_entry);

    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACL by acl_index
 * INPUT:    acl_index
 * OUTPUT:   acl_index, acl_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextAclByIndex(UI32_T *acl_index, RULE_TYPE_Acl_T *acl_entry)
{
    UI32_T ret;

    RULE_OM_EnterCriticalSection();

    ret = RULE_OM_LocalGetNextAclByIndex(acl_index, acl_entry);

    RULE_OM_LeaveCriticalSectionReturnState(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextMibAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next MIB ACL by acl_index
 * INPUT:    acl_index
 * OUTPUT:   acl_index, acl_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextMibAclByIndex(UI32_T *acl_index, RULE_TYPE_MIB_Acl_T *acl_entry)
{
    RULE_TYPE_Acl_T          *acl_p;
    UI32_T                   next_acl_index;
    RULE_TYPE_RETURN_TYPE_T  ret_code;
    if ((NULL == acl_index) || (NULL == acl_entry))
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    for (next_acl_index = *acl_index + 1; _countof(shmem_data_p->acl_table) >= next_acl_index; ++next_acl_index)
    {
        acl_p = &shmem_data_p->acl_table[next_acl_index - 1]; /* to zero-based array index */

        if (RULE_OM_IS_ENTRY_AVAILABLE(acl_p->row_status))
        {
#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) || (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
            if (TRUE == acl_p->flag_hide)
                continue;
#endif /* #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) || (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE) */

            *acl_index = next_acl_index;
            ret_code = RULE_OM_LocalConvert2MibAcl(acl_p, acl_entry);
            RULE_OM_LeaveCriticalSectionReturnState(ret_code);
        }
    }
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACL which (acl_type == acl_entry->acl_type)
 * INPUT:    acl_index, acl_entry->acl_type
 * OUTPUT:   acl_index, acl_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextAcl(UI32_T *acl_index, RULE_TYPE_Acl_T *acl_entry)
{
    RULE_TYPE_Acl_T     *acl_p;
    UI32_T              next_acl_index;

    if ((NULL == acl_index) || (NULL == acl_entry))
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    for (next_acl_index = *acl_index + 1; _countof(shmem_data_p->acl_table) >= next_acl_index; ++next_acl_index)
    {
        acl_p = &shmem_data_p->acl_table[next_acl_index - 1]; /* to zero-based array index */

        if (RULE_OM_IS_ENTRY_DESTROY(acl_p->row_status))
            continue;

        if (acl_p->acl_type != acl_entry->acl_type)
            continue;

        /* if entry exist and acl_type match */
        *acl_index = next_acl_index;
        memcpy(acl_entry, acl_p, sizeof(RULE_TYPE_Acl_T));
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUIAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACL which (acl_type == acl_entry->acl_type)
 * INPUT:    acl_index, acl_entry->acl_type
 * OUTPUT:   acl_index, acl_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextUIAcl(UI32_T *acl_index, RULE_TYPE_UI_AclEntry_T *acl_entry)
{
    RULE_TYPE_Acl_T          *acl_p;
    UI32_T                   next_acl_index;
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    if ((NULL == acl_index) || (NULL == acl_entry))
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    for (next_acl_index = *acl_index + 1; _countof(shmem_data_p->acl_table) >= next_acl_index; ++next_acl_index)
    {
        acl_p = &shmem_data_p->acl_table[next_acl_index - 1]; /* to zero-based array index */

        if (RULE_OM_IS_ENTRY_DESTROY(acl_p->row_status))
            continue;

        if (acl_p->acl_type != acl_entry->acl_type)
            continue;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) || (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
        if (TRUE == acl_p->flag_hide)
            continue;
#endif /* #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) || (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE) */

        /* if entry exist and acl_type match */
        *acl_index = next_acl_index;
        ret_code = RULE_OM_LocalConvert2UIAcl(acl_p, acl_entry);
        RULE_OM_LeaveCriticalSectionReturnState(ret_code);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAclNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get name of ACL by it's index.
 * INPUT:    acl_index
 * OUTPUT:   acl_name.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetAclNameById(UI32_T acl_index, char *acl_name)
{
    if (RULE_OM_IS_BAD_ACL_IDX(acl_index))
        return RULE_TYPE_FAIL;

    if (NULL == acl_name)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();

    strncpy((char *)acl_name, (char *)shmem_data_p->acl_table[acl_index - 1].acl_name, SYS_ADPT_ACL_MAX_NAME_LEN);
    acl_name[SYS_ADPT_ACL_MAX_NAME_LEN] = '\0';

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAclIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of ACL by it's name.
 * INPUT:    acl_name
 * OUTPUT:   acl_index.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetAclIdByName(const char *acl_name, UI32_T *acl_index)
{
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    RULE_OM_EnterCriticalSection();
    ret_code = RULE_OM_LocalGetAclIdByName(acl_name, acl_index);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAclIndexByAce
 *------------------------------------------------------------------------------
 * PURPOSE:  get acl index by ace_index
 * INPUT:    ace_index
 * OUTPUT:   acl_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetAclIndexByAce(UI32_T ace_index, UI32_T *acl_index)
{
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    RULE_OM_EnterCriticalSection();
    ret_code = RULE_OM_LocalGetAclIndexByAce(ace_index, acl_index, NULL);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAclTypeByAce
 *------------------------------------------------------------------------------
 * PURPOSE:  get acl type by ace_index
 * INPUT:    ace_index
 * OUTPUT:   acl_type
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetAclTypeByAce(
    UI32_T ace_index,
    RULE_TYPE_AclType_T *acl_type)
{
    RULE_TYPE_Acl_T         *acl_p;

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T                  acl_index;

    if (NULL == acl_type)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();

    result = RULE_OM_LocalGetAclIndexByAce(ace_index, &acl_index, &acl_p);

    if (RULE_TYPE_OK == result)
    {
        *acl_type = acl_p->acl_type;
    }

    RULE_OM_LeaveCriticalSectionReturnState(result);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAclRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  get acl row status by it's index
 * INPUT:    acl_index
 * OUTPUT:   row_status
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetAclRowStatus(UI32_T acl_index, UI32_T *row_status)
{
    RULE_TYPE_Acl_T         *acl_p;

    if (NULL == row_status)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    *row_status = acl_p->row_status;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_AddAce2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a ACE to ACL.
 * INPUT:    acl_index, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_AddAce2Acl(
    UI32_T acl_index,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_Ace_Entry_T *ace_entry)
{
    RULE_TYPE_Acl_T *acl_p;
    UI32_T ret;

    RULE_OM_EnterCriticalSection();

    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_ACL_NONEXISTED);
    }

    ret = RULE_OM_LocalAddAce2Acl(acl_p, acl_type, ace_entry);
    RULE_OM_LeaveCriticalSectionReturnState(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNumberOfPermitAcesByAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get number of permit aces in an ACL
 * INPUT    : acl_index
 * OUTPUT   : permit_qty
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNumberOfPermitAcesByAclIndex(
    UI32_T acl_index,
    UI32_T *permit_qty)
{
    RULE_TYPE_Ace_Entry_T   *ace_entry_p;
    RULE_TYPE_Acl_T         *acl_entry_p;

    UI32_T                  ace_index;

    if (NULL == permit_qty)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    acl_entry_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_entry_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    *permit_qty = 0;

    for (ace_index = 0;
         RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_entry_p, &ace_index, &ace_entry_p); )
    {
        if (RULE_TYPE_ACE_PERMIT == ace_entry_p->access)
        {
            (*permit_qty)++;
        }
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DelAceFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Del a ACE from ACL.
 * INPUT:    acl_index, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_DelAceFromAcl(
    UI32_T acl_index,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_Ace_Entry_T *ace_entry)
{
    RULE_TYPE_Acl_T *acl_p;
    UI32_T ace_index = 0;

    RULE_OM_EnterCriticalSection();

    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_ACL_NONEXISTED);
    }

    if (FALSE == RULE_OM_LocalIsSameAce(acl_p, ace_entry, &ace_index))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    if (RULE_TYPE_OK == RULE_OM_LocalDelAceFromAcl(acl_p, ace_index))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPortAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  get port bounded ACL by acl type and ingress flag
 * INPUT:    ifindex, acl_type, ingress_flag
 * OUTPUT:   acl_index, time_range_name
 * RETURN:   Error code.
 * NOTE:     (acl_type, ingress_flag) pair e.g.
 *           (IP, ingress), (IP, egress), (MAC, ingress), (MAC, egress), ...
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetPortAclIndex(
    UI32_T ifindex,
    RULE_TYPE_AclType_T acl_type,
    BOOL_T ingress_flag,
    UI32_T *acl_index,
    UI8_T *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable)
{
    RULE_TYPE_AclMemento_T *bind_acl_p;

    ASSERT(NULL != acl_index);
    ASSERT(NULL != counter_enable);

    RULE_OM_EnterCriticalSection();

    *counter_enable = RULE_TYPE_COUNTER_DISABLE;

    bind_acl_p = RULE_OM_LocalGetPortAclByType(ifindex,
                                               ingress_flag ? RULE_TYPE_INBOUND : RULE_TYPE_OUTBOUND,
                                               acl_type);
    if (NULL == bind_acl_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* not existed */
    }

    *acl_index = bind_acl_p->acl_index;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    if (time_range_name != NULL)
    {
        strncpy((char *) time_range_name, (char *) bind_acl_p->time_range_name, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
        time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH] = '\0';
    }
#endif /* SYS_CPNT_TIME_BASED_ACL */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
    *counter_enable = bind_acl_p->counter_enable;
#endif /* SYS_CPNT_ACL_COUNTER */

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextAclByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next binding ACL by port
 * INPUT:    uport_ifindex, acl_index, precedence
 * OUTPUT:   acl_index, acl_entry, precedence, time_range_name.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextAclByPort(
    UI32_T ifindex,
    UI32_T *acl_index,
    RULE_TYPE_Acl_T *acl_entry,
    UI32_T *precedence,
    UI8_T *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable)
{
    RULE_TYPE_Acl_T             *acl_p;
    RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

    ASSERT(NULL != acl_index);
    ASSERT(NULL != acl_entry);
    ASSERT(NULL != precedence);
    ASSERT(NULL != counter_enable);

    *counter_enable     = RULE_TYPE_COUNTER_DISABLE;

    RULE_OM_EnterCriticalSection();
    bind_acl_p = RULE_OM_LocalGetNextPortAclByPrecedence(ifindex, precedence);
    if (NULL == bind_acl_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* no next acl */
    }

    acl_p = RULE_OM_LocalGetAclByIndex(bind_acl_p->acl_index);
    if (NULL == acl_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    *acl_index = bind_acl_p->acl_index;
    memcpy(acl_entry, acl_p, sizeof(RULE_TYPE_Acl_T));

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    if (time_range_name != NULL)
    {
        strncpy((char *) time_range_name, (char *) bind_acl_p->time_range_name, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
        time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH] = '\0';
    }
#endif /* SYS_CPNT_TIME_BASED_ACL */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
    *counter_enable = bind_acl_p->counter_enable;
#endif /* SYS_CPNT_ACL_COUNTER */

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetUIAclByTypeAndDirection
 *------------------------------------------------------------------------------
 * PURPOSE:  get binding ACL by type and direction
 * INPUT:    uport_ifindex, acl_entry->acl->type, ingress_flag
 * OUTPUT:   acl_entry.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetUIAclByTypeAndDirection(UI32_T ifindex, RULE_TYPE_UI_AclEntry_T *acl_entry, BOOL_T ingress_flag)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_Acl_T *acl_p;
    RULE_TYPE_AclType_T class_type;

    RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

    ASSERT(NULL != acl_entry);

    RULE_OM_EnterCriticalSection();

    result = RULE_OM_LocalClassifyUIAcl(acl_entry, &class_type);
    if (RULE_TYPE_OK != result)
    {
        RULE_OM_LeaveCriticalSectionReturnState(result);
    }

    bind_acl_p = RULE_OM_LocalGetPortAclByType(ifindex, ingress_flag ? RULE_TYPE_INBOUND : RULE_TYPE_OUTBOUND, class_type);
    if (NULL == bind_acl_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_INVALID_PARAMETER);
    }

    acl_p = RULE_OM_LocalGetAclByIndex(bind_acl_p->acl_index);
    ASSERT(NULL != acl_p);
    if (NULL == acl_p)
    {
        LOG("<Error> Internal error");
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    result = RULE_OM_LocalConvert2UIAcl(acl_p, acl_entry);
    RULE_OM_LeaveCriticalSection();
    return result;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUIAclByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next binding ACL by port
 * INPUT:    uport_ifindex, acl_index, precedence
 * OUTPUT:   acl_index, precedence, acl_entry, time_range_name.
 * RETURN:   Error code.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextUIAclByPort(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry,
    UI32_T *precedence,
    UI8_T *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable)
{
    RULE_TYPE_Acl_T             *acl_p;
    RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;
    RULE_TYPE_RETURN_TYPE_T     ret_code;

    ASSERT(NULL != acl_index);
    ASSERT(NULL != acl_entry);
    ASSERT(NULL != precedence);
    ASSERT(NULL != counter_enable);

    *counter_enable = RULE_TYPE_COUNTER_DISABLE;

    RULE_OM_EnterCriticalSection();

    bind_acl_p = RULE_OM_LocalGetNextPortAclByDirectionAndPrecedence(ifindex, inout_profile, precedence);
    if (NULL == bind_acl_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* no next acl */
    }

    acl_p = RULE_OM_LocalGetAclByIndex(bind_acl_p->acl_index);
    if (NULL == acl_p)
    {
        ASSERT(0);
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    *acl_index = bind_acl_p->acl_index;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    if (time_range_name != NULL)
    {
        strncpy((char *)time_range_name, (char *)bind_acl_p->time_range_name, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
        time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH] = '\0';
    }
#endif /* SYS_CPNT_TIME_BASED_ACL */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
    *counter_enable = bind_acl_p->counter_enable;
#endif /* SYS_CPNT_ACL_COUNTER */

    ret_code = RULE_OM_LocalConvert2UIAcl(acl_p, acl_entry);

    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUIAclByPortAndBindingType
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next same binding type ACL by port and binding type.
 * INPUT:    precedence, uport_ifindex
 * OUTPUT:   uport_index, acl_entry.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextUIAclByPortAndBindingType(UI32_T precedence, UI32_T *ifindex_p, RULE_TYPE_UI_AclEntry_T *acl_entry_p, UI8_T *time_range_name)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_InOutDirection_T direction;

    UI32_T pl_idx;

    ASSERT(NULL != ifindex_p);
    ASSERT(NULL != acl_entry_p);

    result = RULE_OM_LocalPrecedenceToDirection(precedence, &direction);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    if (0 == *ifindex_p)
    {
        pl_idx = 0;
    }
    else
    {
        pl_idx = RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(*ifindex_p);
        ++ pl_idx;
    }

    RULE_OM_EnterCriticalSection();
    for (; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        RULE_TYPE_PortEntry_T       *port_entry_p;
        RULE_TYPE_Acl_T             *acl_p;
        RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

        *ifindex_p = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(*ifindex_p, direction);
        if (NULL == port_entry_p)
        {
            continue;
        }

        bind_acl_p = RULE_OM_LocalGetPortAclByPrecedence(*ifindex_p, precedence);

        if (NULL == bind_acl_p)
        {
            continue;
        }

        if (TRUE != RULE_OM_IS_ENTRY_ACTIVE(bind_acl_p->row_status))
        {
            continue;
        }

        acl_p = RULE_OM_LocalGetAclByIndex(bind_acl_p->acl_index);
        if (NULL == acl_p)
        {
            ASSERT(0);
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
        }

 #if (SYS_CPNT_TIME_BASED_ACL == TRUE)
        if (time_range_name != NULL)
        {
            strncpy((char *)time_range_name, (char *)bind_acl_p->time_range_name,
                                 SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
            time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH] = '\0';
        }
#endif /* SYS_CPNT_TIME_BASED_ACL */

        result = RULE_OM_LocalConvert2UIAcl(acl_p, acl_entry_p);
        RULE_OM_LeaveCriticalSectionReturnState(result);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_GetNextPort_BindAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Enumerate all interface which have bind the specified ACL
 * INPUT    : acl_index         - ACL index
 *            uport_ifindex_p   - uport ifindex; pass 0 to start.
 * OUTPUT   : uport_ifindex_p   - uport ifindex
 *            inout_profile_p   - direction
 * RETURN   : RULE_TYPE_OK if get an interface; else if error or no more interface
 * NOTES    : Sequence of the enumerating are
 *            uport_inindex -> inout_profile
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextPort_BindAcl(
    UI32_T  acl_index,
    UI32_T  *uport_ifindex_p,
    RULE_TYPE_InOutDirection_T *inout_profile_p)
{
    UI32_T                      start_ifindex;
    RULE_TYPE_InOutDirection_T  start_direction;

    RULE_TYPE_Acl_T     *acl_p;
    UI32_T              i;

    RULE_OM_EnterCriticalSection();

    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);

    if (NULL == acl_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    if (0 == *uport_ifindex_p)
    {
        start_ifindex   = 1;
        start_direction = RULE_TYPE_INBOUND;
    }
    else
    {
        start_ifindex   = (*uport_ifindex_p+1);
        start_direction = *inout_profile_p;
    }

    if (RULE_TYPE_INBOUND == start_direction)
    {
    //
    // FIXME: Not support all port
    //
        *inout_profile_p = RULE_TYPE_INBOUND;
        for (i=start_ifindex; i<=SYS_ADPT_TOTAL_NBR_OF_LPORT; i++)
        {
            if (RULE_OM_IS_BIT_ON(acl_p->filtering_in_port_list, (i-1)))
            {
                *uport_ifindex_p = i;
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
            }
        }

        *inout_profile_p = RULE_TYPE_OUTBOUND;
        for (i=1; i<=SYS_ADPT_TOTAL_NBR_OF_LPORT; i++)
        {
            if (RULE_OM_IS_BIT_ON(acl_p->filtering_out_port_list, (i-1)))
            {
                *uport_ifindex_p = i;
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
            }
        }
    }
    else
    {
    //
    // FIXME: Not support all port
    //
        *inout_profile_p = RULE_TYPE_OUTBOUND;
        for (i=start_ifindex; i<=SYS_ADPT_TOTAL_NBR_OF_LPORT; i++)
        {
            if (RULE_OM_IS_BIT_ON(acl_p->filtering_out_port_list, (i-1)))
            {
                *uport_ifindex_p = i;
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
            }
        }
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_GetNextPort_BindClsMapAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Enumerate all interface which have bind the specified ACL through
 *            class-map and policy-map.
 * INPUT    : acl_index             - ACL index
 *            uport_ifindex_p       - uport ifindex; pass 0 to start.
 * OUTPUT   : uport_ifindex_p       - uport ifindex
 *            inout_profile_p       - direction
 *            policy_map_index_p    - policy-map index
 *            class_map_index_p     - class-map index
 * RETURN   : RULE_TYPE_OK if get an interface; else if error or no more interface
 * NOTES    : Sequence of the enumerating are
 *            uport_inindex -> inout_profile -> policy_map_index -> class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextPort_BindClsMapAcl(
    UI32_T  acl_index,
    UI32_T  *uport_ifindex_p,
    RULE_TYPE_InOutDirection_T  *inout_profile_p,
    UI32_T  *policy_map_index_p,
    UI32_T  *class_map_index_p)
{
    RULE_TYPE_Acl_T *acl_p;

    if (NULL == uport_ifindex_p ||
        NULL == inout_profile_p ||
        NULL == policy_map_index_p ||
        NULL == class_map_index_p)
    {
        return RULE_TYPE_FAIL;
    }

    RULE_OM_EnterCriticalSection();

    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);

    if (NULL == acl_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    if (0 == *uport_ifindex_p)
    {
        *class_map_index_p = 1;
    }

    for (; (*class_map_index_p)<=SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS_MAP; (*class_map_index_p)++)
    {
        if (RULE_OM_IS_BIT_ON(acl_p->class_map_bit_list, (*class_map_index_p-1) ))
        {
            UI32_T res;

            res = RULE_OM_Local_GetNextPort_BindClassMap(*class_map_index_p,
                                                         uport_ifindex_p,
                                                         inout_profile_p,
                                                         policy_map_index_p);
            if (RULE_TYPE_OK == res)
            {
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
            }

            (*uport_ifindex_p) = 0;
        }
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_InitAce
 *------------------------------------------------------------------------------
 * PURPOSE  : for SNMP and WWW and CLI to make a default ACE
 * INPUT    : ace_entry->ace_type
 * OUTPUT   : ace_entry
 * RETURN   : Error code
 * NOTES    : this API is used to init the ace before use it, need to set ace_entry->ace_type
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_InitAce(RULE_TYPE_Ace_Entry_T *ace_entry)
{
    return RULE_OM_LocalSetAceEntryToDefaultValue(ace_entry);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_InitUIAce
 *------------------------------------------------------------------------------
 * PURPOSE  : for UI to make a default UI ACE entry
 * INPUT    : ace_entry->ace_type
 * OUTPUT   : ace_entry
 * RETURN   : Error code
 * NOTES    : this API is used to init the ace before use it
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_InitUIAce(
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    RULE_TYPE_AclType_T ace_type;
    UI32_T row_status;

    ace_type = ace_p->ace_type;
    row_status = ace_p->row_status;

    memset(ace_p, 0, sizeof(*ace_p));

    ace_p->ace_type = ace_type;
    ace_p->row_status = row_status;

    ace_p->access = RULE_TYPE_ACE_PERMIT;

    ace_p->pkt_format = VAL_aclMacAcePktformat_any;
    ace_p->ether.vid.op = VAL_diffServMacAceVidOp_noOperator;
    ace_p->ether.cos.op = VAL_diffServMacAceCosOp_noOperator;
    ace_p->ether.ethertype.op = VAL_diffServMacAceEtherTypeOp_noOperator;

    ace_p->ipv4.protocol.op = RULE_TYPE_IPV4_PROTOCOL_OP_NO_OPERATION;
    ace_p->ipv4.dscp.op = RULE_TYPE_IPV4_DSCP_OP_NO_OPERATION;

    ace_p->ipv6.next_header.op = RULE_TYPE_IPV6_NEXT_HEADER_OP_NO_OPERATION;
    ace_p->ipv6.traffic_class.op = RULE_TYPE_IPV6_TRAFFIC_CLASS_OP_NO_OPERATION;
    ace_p->ipv6.flow_label.op = RULE_TYPE_IPV6_FLOW_LABEL_OP_NO_OPERATION;

    ace_p->icmp.icmp_type.op = RULE_TYPE_ICMP_ICMPTYPE_OP_NO_OPERATION;

    ace_p->tcp.flags.data.u.code = RULE_TYPE_UNDEF_IP_CONTROL_CODE;
    
    ace_p->l4_common.sport.op = VAL_diffServIpAceSourcePortOp_noOperator;
    ace_p->l4_common.dport.op = VAL_diffServIpAceDestPortOp_noOperator;

#if (SYS_CPNT_DAI == TRUE)
    ace_p->arp.is_log = FALSE;
#endif /* SYS_CPNT_DAI */

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    ace_p->time_range_index              = RULE_TYPE_UNDEF_TIME_RANGE;
#endif /* end of #if (SYS_CPNT_TIME_BASED_ACL == TRUE) */

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_SetAcePatternToDefaultByType
 *------------------------------------------------------------------------------
 * PURPOSE  : For UI to initial ace.u.{mac|ip|ipv6|arp} to default
 * INPUT    : ace_type
 * OUTPUT   : ace_entry_p
 * RETURN   : Error code
 * NOTES    : Initial the ace pattern by ace_type
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_SetAcePatternToDefaultByType(
    RULE_TYPE_Ace_Entry_T *ace_entry_p,
    RULE_TYPE_AclType_T ace_type)
{
    ASSERT(ace_entry_p != NULL);

    switch (ace_type)
    {
        case RULE_TYPE_MAC_ACL:
        {
            RULE_TYPE_MacAce_Entry_T    *mac_ace_p;

            mac_ace_p = &ace_entry_p->u.mac;
            memset(mac_ace_p, 0, sizeof(*mac_ace_p));

            mac_ace_p->aceVidOp       = VAL_diffServMacAceVidOp_noOperator;
            mac_ace_p->aceEtherTypeOp = VAL_diffServMacAceEtherTypeOp_noOperator;
            mac_ace_p->aceCosOp       = VAL_diffServMacAceCosOp_noOperator;
            mac_ace_p->acePktformat   = VAL_diffServMacAcePktformat_any;
            break;
        }

        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_EXT_ACL:
        case RULE_TYPE_IP_STD_ACL:
        {
            RULE_TYPE_IpAce_Entry_T     *ip_ace_p;

            ip_ace_p = &ace_entry_p->u.ip;
            memset(ip_ace_p, 0, sizeof(*ip_ace_p));

            ip_ace_p->aceProtocol     = RULE_TYPE_UNDEF_IP_PROTOCOL;
            ip_ace_p->acePrec         = RULE_TYPE_UNDEF_IP_PRECEDENCE;
            ip_ace_p->aceTos          = RULE_TYPE_UNDEF_IP_TOS;
            ip_ace_p->aceDscp         = RULE_TYPE_UNDEF_IP_DSCP;
            ip_ace_p->aceSourcePortOp = VAL_diffServIpAceSourcePortOp_noOperator;
            ip_ace_p->aceDestPortOp   = VAL_diffServIpAceDestPortOp_noOperator;
            ip_ace_p->aceIcmpType     = RULE_TYPE_UNDEF_ICMP_TYPE;

            break;
        }

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        {
            RULE_TYPE_Ipv6Ace_Entry_T   *ipv6_ace_p;

            ipv6_ace_p = &ace_entry_p->u.ipv6;
            memset(ipv6_ace_p, 0, sizeof(*ipv6_ace_p));

            ipv6_ace_p->aceNextHeader   = RULE_TYPE_UNDEF_IPV6_NEXT_HEADER;
            ipv6_ace_p->aceDscp         = RULE_TYPE_UNDEF_IPV6_DSCP;
            ipv6_ace_p->aceFlowLabel    = RULE_TYPE_UNDEF_IPV6_FLOW_LABEL;
            ipv6_ace_p->aceSourcePortOp = VAL_diffServIpAceSourcePortOp_noOperator;
            ipv6_ace_p->aceDestPortOp   = VAL_diffServIpAceDestPortOp_noOperator;
            break;
        }
#if (SYS_CPNT_DAI == TRUE)
        case RULE_TYPE_ARP_ACL:
        {
            RULE_TYPE_ArpAce_Entry_T    *arp_ace_p;

            arp_ace_p = &ace_entry_p->u.arp;
            memset(arp_ace_p, 0, sizeof(*arp_ace_p));

            arp_ace_p->aceSenderIpAddrBitmask = 0;
            arp_ace_p->aceTargetIpAddrBitmask = 0;
            memset (arp_ace_p->aceSenderMacAddrBitmask, 0, SYS_ADPT_MAC_ADDR_LEN);
            memset (arp_ace_p->aceTargetMacAddrBitmask, 0, SYS_ADPT_MAC_ADDR_LEN);
            arp_ace_p->aceIsLog = FALSE;
            break;
        }
#endif /* SYS_CPNT_DAI */

        default: /* should not go here */
            ASSERT(0);
            return RULE_TYPE_FAIL;
    };

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_IsDefaultIpACE
 *------------------------------------------------------------------------------
 * PURPOSE  : Check if an ACE is default ACE
 * INPUT    : ip_ace
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTES    : This API associates with RULE_OM_LocalSetAceEntryToDefaultValue,
              if the latter is changed, this API should be changed.
 *------------------------------------------------------------------------------
 */

BOOL_T RULE_OM_IsDefaultIpACE(const RULE_TYPE_IpAce_Entry_T *ip_ace)
{
    UI32_T bit = 0;

    if (NULL == ip_ace)
        return FALSE;

    if(RULE_TYPE_UNDEF_IP_PROTOCOL != ip_ace->aceProtocol)
        return FALSE;

    if(RULE_TYPE_UNDEF_IP_PRECEDENCE != ip_ace->acePrec)
        return FALSE;

    if(RULE_TYPE_UNDEF_IP_TOS != ip_ace->aceTos)
        return FALSE;

    if(RULE_TYPE_UNDEF_IP_DSCP != ip_ace->aceDscp)
        return FALSE;

    if(VAL_diffServIpAceSourcePortOp_noOperator != ip_ace->aceSourcePortOp)
        return FALSE;

    if(MAX_diffServIpAceSourcePortBitmask != ip_ace->aceSourcePortBitmask)
        return FALSE;

    if(MAX_diffServIpAceDestPortBitmask != ip_ace->aceDestPortBitmask)
        return FALSE;

    if(VAL_diffServIpAceDestPortOp_noOperator != ip_ace->aceDestPortOp)
        return FALSE;

    if(RULE_TYPE_UNDEF_IP_CONTROL_CODE != ip_ace->aceControlCode)
        return FALSE;

    if(MIN_diffServIpAceControlCodeBitmask != ip_ace->aceControlCodeBitmask)
        return FALSE;

    if(256 != ip_ace->aceIcmpType)
        return FALSE;

    if(0 != ip_ace->aceIcmpCode)
        return FALSE;

    if(3  != ip_ace->aceFragmentInfo)
        return FALSE;

    if(1 != ip_ace->aceIpOptionOp)
        return FALSE;

    if(255 != ip_ace->aceMinIpOption)
        return FALSE;

    if(255 != ip_ace->aceMaxIpOption)
        return FALSE;

    bit = ip_ace->aceSourceIpAddr | ip_ace->aceSourceIpAddrBitmask |
            ip_ace->aceDestIpAddr | ip_ace->aceDestIpAddrBitmask |
            ip_ace->aceMinSourcePort | ip_ace->aceMaxSourcePort |
            ip_ace->aceMinDestPort | ip_ace->aceMaxDestPort;

    if(0 != bit)
        return FALSE;

    return TRUE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_IsMatchAnyAnyAce
 *------------------------------------------------------------------------------
 * PURPOSE  : Check ACE whther is "permit any any" or "deny any any".
 * INPUT    : ace_index	- ACE index
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTES    : None
 * NOTE:    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_OM_IsMatchAnyAnyAce(
    UI32_T ace_index)
{
    RULE_TYPE_Ace_Entry_T *ace_entry_p;
    RULE_TYPE_Ace_Entry_T dflt_entry;

    RULE_OM_EnterCriticalSection();

    ace_entry_p = RULE_OM_LocalGetAceByIndex(ace_index);
    if (NULL == ace_entry_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(FALSE);
    }

    dflt_entry.ace_type = ace_entry_p->ace_type;
    RULE_OM_LocalSetAceEntryToDefaultValue(&dflt_entry);

    if (TRUE == RULE_OM_LocalIsSameAceWithoutAccessField(ace_entry_p, &dflt_entry))
    {
        RULE_OM_LeaveCriticalSectionReturnState(TRUE);
    }

    RULE_OM_LeaveCriticalSectionReturnState(FALSE);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_ResetAceEntryByNewAceType
 *------------------------------------------------------------------------------
 * PURPOSE  : Reset unsupport fields of ace to default value according to ace_type
 * INPUT    : ace_entry, new_ace_type
 * OUTPUT   : none
 * RETURN   : Error code
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_ResetAceEntryByNewAceType(UI32_T ace_index, RULE_TYPE_AclType_T new_ace_type)
{
    RULE_TYPE_Ace_Entry_T       *ace_p, new_ace_entry;

    RULE_OM_EnterCriticalSection();

    ace_p = RULE_OM_LocalGetAceByIndex(ace_index);
    if (NULL == ace_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);  /* bad index */

    new_ace_entry.ace_type   = new_ace_type;
    new_ace_entry.row_status = ace_p -> row_status;

    if ( RULE_TYPE_OK != RULE_OM_InitAce (&new_ace_entry) )
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    new_ace_entry.access = ace_p->access;

    switch (ace_p -> ace_type)
    {
        case RULE_TYPE_IP_EXT_ACL: /* EXT -> STD */
            new_ace_entry.u.ip.aceSourceIpAddr        = ace_p->u.ip.aceSourceIpAddr;
            new_ace_entry.u.ip.aceSourceIpAddrBitmask = ace_p->u.ip.aceSourceIpAddrBitmask;

            memcpy(&ace_p->u, &new_ace_entry.u, sizeof(ace_p->u));
            break;

        case RULE_TYPE_IP_STD_ACL: /* STD -> EXT */
        case RULE_TYPE_MAC_ACL:    /* MAC -> MAC */
            break;

        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            memcpy(&ace_p->u, &new_ace_entry.u, sizeof(ace_p->u));
            break;

        default: /* should not go here */
            ASSERT(0);
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    ace_p->ace_type = new_ace_type;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUIAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next UI ACE for ACL
 * INPUT:    acl_index, ace_index
 * OUTPUT:   ace_index, ace_p
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextUIAceByAcl(
    UI32_T acl_index,
    UI32_T *ace_index_p,
    RULE_TYPE_UI_Ace_Entry_T *ui_ace_p)
{
    RULE_TYPE_Acl_T         *acl_p;
    RULE_TYPE_Ace_Entry_T   *tmp_ace_entry_p;

    RULE_TYPE_RETURN_TYPE_T result;

    RULE_OM_EnterCriticalSection();

    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_ACL_NONEXISTED);
    }

    if (0 == acl_p->ace_count)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_EMPTY_ACL);
    }

    result = RULE_OM_LocalGetNextAceByAcl(acl_p, ace_index_p, &tmp_ace_entry_p);
    if (RULE_TYPE_OK == result)
    {
        result = RULE_OM_ConvertAce2UIAce(tmp_ace_entry_p, ui_ace_p);
    }

    RULE_OM_LeaveCriticalSectionReturnState(result);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetUIAceByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get ACE by its index
 * INPUT:    ace_index
 * OUTPUT:   ace_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetUIAceByIndex(
    UI32_T ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    RULE_TYPE_Ace_Entry_T   *ace_entry_p;

    if (NULL == ace_p)
    {
        return RULE_TYPE_FAIL;
    }

    RULE_OM_EnterCriticalSection();
    ace_entry_p = RULE_OM_LocalGetAceByIndex(ace_index);
    if (NULL == ace_entry_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */
    }

    if (RULE_TYPE_FAIL == RULE_OM_ConvertAce2UIAce(ace_entry_p, ace_p))
    {
        LOG("Convert to ui ace fail");
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUIAceByAceTypeAndIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next UI ACE by its index and ace type
 * INPUT:    ace_index_p, ace_type (RULE_TYPE_IP_ACL, RULE_TYPE_MAC_ACL, RULE_TYPE_IPV6_ACL)
 * OUTPUT:   ace_index_p, ace_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextUIAceByAceTypeAndIndex(
    UI32_T ace_type,
    UI32_T *ace_index_p,
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    RULE_TYPE_Ace_Entry_T   *ace_entry_p;
    UI32_T                  next_ace_index;

    if ((NULL == ace_index_p) || (NULL == ace_p))
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();

    /* This function is used for SNMP only.
     * So only list all user created ACE only.
     */
    for (next_ace_index = *ace_index_p + 1; SYS_ADPT_MAX_NBRS_OF_ACE_OF_SYSTEM >= next_ace_index; ++next_ace_index)
    {
        ace_entry_p = &shmem_data_p->ace_table[next_ace_index - 1]; /* to zero-based array index */

        /* if entry exist and ace_type match */
        if (TRUE == RULE_OM_IS_ENTRY_AVAILABLE(ace_entry_p->row_status))
        {
            switch (ace_type)
            {
                case RULE_TYPE_IP_ACL:
                    if (FALSE == RULE_OM_IS_IP_ACL(ace_entry_p->ace_type))
                        continue;
                    break;

                case RULE_TYPE_MAC_ACL:
                    if (FALSE == RULE_OM_IS_MAC_ACL(ace_entry_p->ace_type))
                        continue;
                    break;

                case RULE_TYPE_IPV6_ACL:
                    if (FALSE == RULE_OM_IS_IPV6_ACL(ace_entry_p->ace_type))
                        continue;
                    break;

#if (SYS_CPNT_DAI == TRUE)
                case RULE_TYPE_ARP_ACL:
                    if (FALSE == RULE_OM_IS_ARP_ACL(ace_entry_p->ace_type))
                        continue;
                    break;
#endif /* #if (SYS_CPNT_DAI == TRUE) */

                default:
                    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
            }

            *ace_index_p = next_ace_index;

            if (RULE_TYPE_OK != RULE_OM_ConvertAce2UIAce(ace_entry_p, ace_p))
            {
                LOG("Convert to ui ace fail");
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
            }

            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
        }
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE for ACL
 * INPUT:    acl_index, ace_index
 * OUTPUT:   ace_index, acl_type, ace_entry.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextAceByAcl(
    UI32_T acl_index,
    RULE_TYPE_AclType_T *ace_type,
    UI32_T *ace_index,
    RULE_TYPE_Ace_Entry_T *ace_entry)
{
    RULE_TYPE_Acl_T         *acl_p;
    RULE_TYPE_Ace_Entry_T   *tmp_ace_entry_p;

    RULE_TYPE_RETURN_TYPE_T result;

    RULE_OM_EnterCriticalSection();

    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_ACL_NONEXISTED);
    }

    if (0 == acl_p->ace_count)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_EMPTY_ACL);
    }

    result = RULE_OM_LocalGetNextAceByAcl(acl_p, ace_index, &tmp_ace_entry_p);
    if (RULE_TYPE_OK == result)
    {
        memcpy(ace_entry, tmp_ace_entry_p, sizeof(*ace_entry));
        *ace_type = ace_entry->ace_type;
    }

    RULE_OM_LeaveCriticalSectionReturnState(result);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetLastAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get last ACE for ACL
 * INPUT:    acl_index, ace_index
 * OUTPUT:   ace_index, acl_type, ace_entry.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetLastAceByAcl(
    UI32_T acl_index,
    RULE_TYPE_AclType_T *ace_type,
    UI32_T *ace_index,
    RULE_TYPE_Ace_Entry_T *ace_entry)
{
    RULE_TYPE_Acl_T *acl_p;
    RULE_TYPE_Ace_Entry_T *last_ace_entry_p;

    if (NULL == ace_type ||
        NULL == ace_index ||
        NULL == ace_entry)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    RULE_OM_EnterCriticalSection();

    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_ACL_NONEXISTED);
    }

    if (0 == acl_p->ace_count)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_EMPTY_ACL);
    }

    ASSERT(RULE_TYPE_NIL != acl_p->super.links.last_node.type);

    last_ace_entry_p = RULE_OM_PTR2INST(acl_p->super.links.last_node);

    ASSERT(NULL != last_ace_entry_p);

    memcpy(ace_entry, last_ace_entry_p, sizeof(*ace_entry));
    *ace_index = RULE_OM_INDEX_OF(last_ace_entry_p) + 1;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_CreateAce
 *------------------------------------------------------------------------------
 * PURPOSE:  Create ace on specified index
 * INPUT:    ace_index, ace_tye, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_CreateAce(UI32_T ace_index, RULE_TYPE_AclType_T ace_type, BOOL_T is_active)
{
    RULE_TYPE_Ace_Entry_T       *ace_p;

    if (RULE_OM_IS_BAD_ACE_IDX(ace_index))
        return RULE_TYPE_FAIL; /* bad index */

    RULE_OM_EnterCriticalSection();
    ace_p = &shmem_data_p->ace_table[ace_index - 1]; /* to zero-based array index */
    if (RULE_OM_IS_ENTRY_AVAILABLE(ace_p->row_status))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* already created */

    /* set ace_type to default ace_type */
    switch (ace_type)
    {
    case RULE_TYPE_IP_ACL:
        ace_type = RULE_TYPE_IP_EXT_ACL;
        break;

    case RULE_TYPE_IPV6_ACL:
        ace_type = RULE_TYPE_IPV6_EXT_ACL;
        break;

    case RULE_TYPE_MAC_ACL:
    case RULE_TYPE_IP_STD_ACL:
    case RULE_TYPE_IP_EXT_ACL:
    case RULE_TYPE_IPV6_STD_ACL:
    case RULE_TYPE_IPV6_EXT_ACL:
        break;

#if (SYS_CPNT_DAI == TRUE)
    case RULE_TYPE_ARP_ACL:
        break;
#endif /* #if (SYS_CPNT_DAI == TRUE) */

    default:
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    /* initial the ace by ace_type */
    ace_p -> ace_type = ace_type;
    if (RULE_TYPE_OK != RULE_OM_LocalSetAceEntryToDefaultValue(ace_p))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    if (TRUE == is_active)
    {
        RULE_OM_SET_ENTRY_ACTIVE(ace_p->row_status);
    }
    else
    {
        RULE_OM_SET_ENTRY_NOTREADY(ace_p->row_status);
    }

    ace_p->super.type = RULE_TYPE_INST_OM_USER_ACE;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DestroyAce
 *------------------------------------------------------------------------------
 * PURPOSE  : destroy ace by index
 * INPUT    : ace_index
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : rule mgr SHALL remove ace from acl/class-map first
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DestroyAce(UI32_T ace_index)
{
    RULE_TYPE_Ace_Entry_T       *ace_p;

    RULE_OM_EnterCriticalSection();
    ace_p = RULE_OM_LocalGetAceByIndex(ace_index);
    if (NULL == ace_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* not exist */

    RULE_OM_LocalFreeAceEntry(ace_index);
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetAceFieldByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Set ACE field by its index
 * INPUT:    ace_index, ace_entry, field_index (which leaf), owner (ACL/MF)
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_SetAceFieldByIndex(
    UI32_T ace_index,
    const RULE_TYPE_Ace_Entry_T *ace_entry,
    UI32_T field_index,
    UI32_T owner)
{
    RULE_TYPE_Ace_Entry_T   *ace_p;
    RULE_TYPE_INSTANCE_T    super;
    UI32_T                  row_status;

    if (NULL == ace_entry)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    ace_p = RULE_OM_LocalGetAceByIndex(ace_index);
    if (NULL == ace_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);  /* bad index */

    if (memcmp (ace_p, ace_entry, sizeof (RULE_TYPE_Ace_Entry_T)) == 0)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);    /* same data */

    if (RULE_TYPE_OK != RULE_OM_LocalValidateAceField(ace_entry, field_index, owner))
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);  /* bad parameters */

    row_status = ace_p->row_status;
    super = ace_p->super;

    memcpy(ace_p, ace_entry, sizeof(RULE_TYPE_Ace_Entry_T));

    ace_p->row_status = row_status;
    ace_p->super = super;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetAceRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  Set ace row status by index
 * INPUT:    ace_index, is_active
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetAceRowStatus(UI32_T ace_index, BOOL_T is_active)
{
    RULE_TYPE_Ace_Entry_T       *ace_p;

    RULE_OM_EnterCriticalSection();
    ace_p = RULE_OM_LocalGetAceByIndex(ace_index);
    if (NULL == ace_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    if (TRUE == is_active)
    {
        if(RULE_TYPE_OK != RULE_OM_LocalValidateAce(ace_p))
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

        RULE_OM_SET_ENTRY_ACTIVE(ace_p->row_status);
    }
    else
    {
        RULE_OM_SET_ENTRY_NOTREADY(ace_p->row_status);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAceByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get ACE by its index
 * INPUT:    ace_index
 * OUTPUT:   ace_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetAceByIndex(UI32_T ace_index, RULE_TYPE_Ace_Entry_T *ace_entry)
{
    RULE_TYPE_Ace_Entry_T   *ace_entry_p;

    if (NULL == ace_entry)
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    ace_entry_p = RULE_OM_LocalGetAceByIndex(ace_index);
    if (NULL == ace_entry_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    memcpy(ace_entry, ace_entry_p, sizeof(RULE_TYPE_Ace_Entry_T));
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextMibAceByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE by port
 * INPUT:    ifindex       - interface index
 *           inout_profile - direction
 *           acl_index_p   - user config ACL index
 *           ace_index_p   - hardware ACE index
 * OUTPUT:   ace_entry_p   - ACE entry
 * RETURN:   Error code.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextMibAceByPort(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index_p,
    UI32_T *ace_index_p,
    RULE_TYPE_Ace_Entry_T *ace_entry_p,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable_p)
{
    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAceIndexFromAclByAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : search an ace match ace_entry in specified acl
 * INPUT    : acl_index, ace_entry
 * OUTPUT   : ace_index
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetAceIndexFromAclByAceEntry(
    UI32_T acl_index,
    const RULE_TYPE_Ace_Entry_T *ace_entry,
    UI32_T *ace_index)
{
    RULE_TYPE_Acl_T *acl_p;

    RULE_OM_EnterCriticalSection();

    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_ACL_NONEXISTED);
    }

    if (FALSE == RULE_OM_LocalIsSameAce(acl_p, ace_entry, ace_index))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextAceByAceTypeAndIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE by its index and ace type
 * INPUT:    ace_index, ace_type (RULE_TYPE_IP_ACL, RULE_TYPE_MAC_ACL, RULE_TYPE_IPV6_ACL)
 * OUTPUT:   ace_index, ace_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextAceByAceTypeAndIndex(UI32_T ace_type, UI32_T *ace_index, RULE_TYPE_Ace_Entry_T *ace_entry)
{
    RULE_TYPE_Ace_Entry_T   *ace_entry_p;
    UI32_T                  next_ace_index;

    if ((NULL == ace_index) || (NULL == ace_entry))
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();

    /* This function is used for SNMP only.
     * So only list all user created ACE only.
     */
    for (next_ace_index = *ace_index + 1; /*_countof(shmem_data_p->ace_table)*/SYS_ADPT_MAX_NBRS_OF_ACE_OF_SYSTEM >= next_ace_index; ++next_ace_index)
    {
        ace_entry_p = &shmem_data_p->ace_table[next_ace_index - 1]; /* to zero-based array index */

        /* if entry exist and ace_type match */
        if (TRUE == RULE_OM_IS_ENTRY_AVAILABLE(ace_entry_p->row_status))
        {
            switch (ace_type)
            {
            case RULE_TYPE_IP_ACL:
                if (FALSE == RULE_OM_IS_IP_ACL(ace_entry_p->ace_type))
                    continue;
                break;

            case RULE_TYPE_MAC_ACL:
                if (FALSE == RULE_OM_IS_MAC_ACL(ace_entry_p->ace_type))
                    continue;
                break;

            case RULE_TYPE_IPV6_ACL:
                if (FALSE == RULE_OM_IS_IPV6_ACL(ace_entry_p->ace_type))
                    continue;
                break;

#if (SYS_CPNT_DAI == TRUE)
            case RULE_TYPE_ARP_ACL:
                if (FALSE == RULE_OM_IS_ARP_ACL(ace_entry_p->ace_type))
                    continue;
                break;
#endif /* #if (SYS_CPNT_DAI == TRUE) */

            default:
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
            }

            *ace_index = next_ace_index;
            memcpy(ace_entry, ace_entry_p, sizeof(RULE_TYPE_Ace_Entry_T));
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
        }
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAceTypeAndRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  get ace type and row status by it's index
 * INPUT:    ace_index
 * OUTPUT:   ace_type, row_status
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetAceTypeAndRowStatus(UI32_T ace_index, RULE_TYPE_AclType_T *ace_type, UI32_T *row_status)
{
    RULE_TYPE_Ace_Entry_T   *ace_entry_p;

    if ((NULL == ace_type) || (NULL == row_status))
        return RULE_TYPE_FAIL;

    RULE_OM_EnterCriticalSection();
    ace_entry_p = RULE_OM_LocalGetAceByIndex(ace_index);
    if (NULL == ace_entry_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    *ace_type = ace_entry_p->ace_type;
    *row_status = ace_entry_p->row_status;
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Record the ACL information on port
 * INPUT    : ifindex, direction, acl_index, time_range_index, time_range_name, counter
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_SetAcl(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_index,
    UI32_T time_range_index,
    UI8_T *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T counter)
{
    RULE_TYPE_PortEntry_T   *port_entry_p;
    RULE_TYPE_AclMemento_T  *bind_acl_p;

    RULE_TYPE_Acl_T         *acl_entry_p;
    RULE_TYPE_AclType_T     port_acl_type;

    RULE_OM_EnterCriticalSection();

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, direction);
    if (NULL == port_entry_p)
    {
        LOG("<Error> Invalid parameter");
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_INVALID_PARAMETER);
    }

    acl_entry_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_entry_p)
    {
        LOG("<Error> Invalid parameter");
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_INVALID_PARAMETER);
    }

    switch (acl_entry_p->acl_type)
    {
        case RULE_TYPE_MAC_ACL:
            port_acl_type = RULE_TYPE_MAC_ACL;
            break;

        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            port_acl_type = RULE_TYPE_IP_ACL;
            break;

        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            port_acl_type = RULE_TYPE_IPV6_ACL;
            break;

        default:
            LOG("<Error> Internal error");
            ASSERT(0);
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
            break;
    }

    ASSERT(port_acl_type < _countof(port_entry_p->bind_acl));

    bind_acl_p = &port_entry_p->bind_acl[port_acl_type];

    ASSERT(TRUE != RULE_OM_IS_ENTRY_ACTIVE(bind_acl_p->row_status));

    /* Update to port entry
     */
    bind_acl_p->acl_index = acl_index;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    bind_acl_p->time_range_index = time_range_index;
    strncpy((char *) bind_acl_p->time_range_name, (char *) time_range_name,
		SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
    bind_acl_p->time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH] = '\0';
#endif

    bind_acl_p->counter_enable = counter;
    RULE_OM_SET_ENTRY_ACTIVE(bind_acl_p->row_status);

    /* Update to ACL entry
     */
    if (RULE_TYPE_INBOUND == direction)
    {
        RULE_OM_SET_BIT_ON(acl_entry_p->filtering_in_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex));
    }
    else
    {
        RULE_OM_SET_BIT_ON(acl_entry_p->filtering_out_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex));
    }

//#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
//    if (TRUE == acl_entry_p->flag_aggregate_done &&
//        0 != acl_entry_p->aggregate_acl_index)
//    {
//        RULE_TYPE_Acl_T *agg_acl_entry_p;
//
//        agg_acl_entry_p = RULE_OM_LocalGetAclByIndex(acl_entry_p->aggregate_acl_index);
//        ASSERT(NULL != agg_acl_entry_p);
//
//        memcpy(agg_acl_entry_p->filtering_in_port_list,
//               acl_entry_p->filtering_in_port_list,
//               sizeof(agg_acl_entry_p->filtering_in_port_list));
//
//        memcpy(agg_acl_entry_p->filtering_out_port_list,
//               acl_entry_p->filtering_out_port_list,
//               sizeof(agg_acl_entry_p->filtering_out_port_list));
//    }
//#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_ResetAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Reset the ACL information on port
 * INPUT    : ifindex, direction, acl_index
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : port_entry_p->bind_acl[port_acl_type].row_status will be set to
 *            notReady
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_ResetAcl(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_index)
{
    RULE_TYPE_PortEntry_T   *port_entry_p;
    RULE_TYPE_AclMemento_T  *bind_acl_p;

    RULE_TYPE_Acl_T         *acl_entry_p;
    RULE_TYPE_AclType_T     port_acl_type;

    RULE_OM_EnterCriticalSection();

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, direction);
    if (NULL == port_entry_p)
    {
        LOG("<Error> Invalid parameter");
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_INVALID_PARAMETER);
    }

    acl_entry_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_entry_p)
    {
        LOG("<Error> Invalid parameter");
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_INVALID_PARAMETER);
    }

    switch (acl_entry_p->acl_type)
    {
        case RULE_TYPE_MAC_ACL:
            port_acl_type = RULE_TYPE_MAC_ACL;
            break;

        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            port_acl_type = RULE_TYPE_IP_ACL;
            break;

        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            port_acl_type = RULE_TYPE_IPV6_ACL;
            break;

        default:
            LOG("<Error> Internal error");
            ASSERT(0);
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
            break;
    }

    ASSERT(port_acl_type < _countof(port_entry_p->bind_acl));

    bind_acl_p = &port_entry_p->bind_acl[port_acl_type];

    ASSERT(acl_index == bind_acl_p->acl_index);

    /* Update to port entry
     */
    RULE_OM_SET_ENTRY_NOTREADY(bind_acl_p->row_status);

    /* Update to ACL entry
     */
    if (RULE_TYPE_INBOUND == direction)
    {
        RULE_OM_SET_BIT_OFF(acl_entry_p->filtering_in_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex));
    }
    else
    {
        RULE_OM_SET_BIT_OFF(acl_entry_p->filtering_out_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex));
    }

//#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
//    if (TRUE == acl_entry_p->flag_aggregate_done &&
//        0 != acl_entry_p->aggregate_acl_index)
//    {
//        RULE_TYPE_Acl_T *agg_acl_entry_p;
//
//        agg_acl_entry_p = RULE_OM_LocalGetAclByIndex(acl_entry_p->aggregate_acl_index);
//        ASSERT(NULL != agg_acl_entry_p);
//
//        memcpy(agg_acl_entry_p->filtering_in_port_list,
//               acl_entry_p->filtering_in_port_list,
//               sizeof(agg_acl_entry_p->filtering_in_port_list));
//
//        memcpy(agg_acl_entry_p->filtering_out_port_list,
//               acl_entry_p->filtering_out_port_list,
//               sizeof(agg_acl_entry_p->filtering_out_port_list));
//    }
//#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

#if (SYS_CPNT_ACL_MIRROR == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_AddAclMirrorEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Add ACL mirror entry
 * INPUT:    ifindex_dest   - Destination port
 *           acl_name       - Source ACL name
 * OUTPUT:   NONE
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_AddAclMirrorEntry(
    UI32_T ifindex_dest,
    UI32_T acl_index)
{
    RULE_TYPE_Acl_T *acl_p;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    RULE_TYPE_Acl_T *agg_acl_p;
#endif

    RULE_OM_EnterCriticalSection();

    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    acl_p->mirror.enable = RULE_TYPE_MIRROR_ENABLE;
    acl_p->mirror.dest_ifindex = ifindex_dest;

    /* Sync to working ACL ...
     * OTHER ATTRIBUTES !?
     */
#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    if (TRUE == acl_p->flag_aggregate_done)
    {
        agg_acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
        if (NULL != agg_acl_p)
        {
            ASSERT(sizeof(agg_acl_p->mirror) == 8);
            memcpy(&agg_acl_p->mirror, &acl_p->mirror, sizeof(agg_acl_p->mirror));
        }
    }
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RemoveAclMirrorEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Remove ACL mirror entry
 * INPUT:    ifindex_dest   - Destination port
 *           acl_name       - Source ACL name
 * OUTPUT:   NONE
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_RemoveAclMirrorEntry(
    UI32_T ifindex_dest,
    UI32_T acl_index)
{
    RULE_TYPE_Acl_T *acl_p;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    RULE_TYPE_Acl_T *agg_acl_p;
#endif

    RULE_OM_EnterCriticalSection();

    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    acl_p->mirror.enable = RULE_TYPE_MIRROR_DISABLE;
    acl_p->mirror.dest_ifindex = 0;

    /* Sync to working ACL ...
     * OTHER ATTRIBUTES !?
     */
#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    if (TRUE == acl_p->flag_aggregate_done)
    {
        agg_acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
        if (NULL != agg_acl_p)
        {
            ASSERT(sizeof(agg_acl_p->mirror) == 8);
            memcpy(&agg_acl_p->mirror, &acl_p->mirror, sizeof(agg_acl_p->mirror));
        }
    }
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextAclMirrorEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  GetNext ACL mirror entry
 * INPUT:    ifindex_dest   - Destination port. Pass 0 to start.
 *           acl_index      - ACL index
 * OUTPUT:   acl_entry      - ACL entry
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextAclMirrorEntry(
    UI32_T *ifindex_dest,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry)
{
    RULE_TYPE_Acl_T tmp_acl_entry;

    RULE_OM_EnterCriticalSection();

    if (0 == *ifindex_dest)
    {
        *acl_index = 0;
    }

    while (RULE_TYPE_OK == RULE_OM_LocalGetNextAclByIndex(acl_index,
                                                                &tmp_acl_entry))
    {
    #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        if (tmp_acl_entry.flag_hide == TRUE)
        {
            continue;
        }
    #endif

        if (tmp_acl_entry.mirror.enable == RULE_TYPE_MIRROR_ENABLE)
        {
            *ifindex_dest = tmp_acl_entry.mirror.dest_ifindex;

            RULE_OM_LocalConvert2UIAcl(&tmp_acl_entry, acl_entry);

            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
        }
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}
#endif /* #if (SYS_CPNT_ACL_MIRROR == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Get_PortEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  (temporary function) Get port entry
 * INPUT:    uport_ifindex
 * OUTPUT:   port_entry
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Get_PortEntry(UI32_T uport_ifindex, RULE_TYPE_PortEntry_T *port_entry)
{
    RULE_TYPE_PortEntry_T   *port_entry_p;

    RULE_OM_EnterCriticalSection();
    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, RULE_TYPE_INBOUND);
    if (NULL == port_entry_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad ifindex */

    memcpy(port_entry, port_entry_p, sizeof(RULE_TYPE_PortEntry_T));
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetUIPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Get port entry by ifindex
 * INPUT:    ifindex
 *           inout_profile  - direction
 * OUTPUT:   port_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetUIPortEntry(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    RULE_TYPE_UI_PortEntry_T *port_entry)
{
    RULE_TYPE_PortEntry_T   *port_entry_p;
    RULE_TYPE_RETURN_TYPE_T  ret_code;

    RULE_OM_EnterCriticalSection();
    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, inout_profile);
    if (NULL == port_entry_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad ifindex */

    ret_code = RULE_OM_LocalConvert2UIPortEntry(port_entry_p, port_entry);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Get_PortEntryEx
 *------------------------------------------------------------------------------
 * PURPOSE:  (temporary function) Get port entry
 * INPUT:    uport_ifindex
 *           inout_profile - direction
 * OUTPUT:   port_entry
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_Get_PortEntryEx(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    RULE_TYPE_PortEntry_T *port_entry)
{
    RULE_TYPE_PortEntry_T   *port_entry_p;

    RULE_OM_EnterCriticalSection();
    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, inout_profile);
    if (NULL == port_entry_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad ifindex */

    memcpy(port_entry, port_entry_p, sizeof(RULE_TYPE_PortEntry_T));
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_TakeSnapshot
 *------------------------------------------------------------------------------
 * PURPOSE:  Take a snapshot of rule OM
 * INPUT:    snapshot_p  - Snapshot
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
void RULE_OM_TakeSnapshot(
    RULE_OM_Snapshot_T *snapshot_p)
{
    memcpy(snapshot_p->rule_mgr_port_table, shmem_data_p->rule_mgr_port_table,
                                       sizeof(snapshot_p->rule_mgr_port_table));

    memcpy(snapshot_p->egress_port_table, shmem_data_p->egress_port_table,
                                         sizeof(snapshot_p->egress_port_table));

    memcpy(snapshot_p->acl_table, shmem_data_p->acl_table,
                                                 sizeof(snapshot_p->acl_table));
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RevertSnapshot
 *------------------------------------------------------------------------------
 * PURPOSE:  Revert to snapshot
 * INPUT:    snapshot_p  - Snapshot
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
void RULE_OM_RevertSnapshot(
    RULE_OM_Snapshot_T *snapshot_p)
{
    memcpy(shmem_data_p->rule_mgr_port_table, snapshot_p->rule_mgr_port_table,
                                       sizeof(snapshot_p->rule_mgr_port_table));

    memcpy(shmem_data_p->egress_port_table, snapshot_p->egress_port_table,
                                         sizeof(snapshot_p->egress_port_table));

    memcpy(shmem_data_p->acl_table, snapshot_p->acl_table,
                                                 sizeof(snapshot_p->acl_table));
}
#endif /* SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUportIfindex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next user port ifindex
 * INPUT:    ifindex        - ifindex, pass 0 to start
 * OUTPUT:   ifindex        - ifindex
 *           inout_profile  - direction
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_GetNextUportIfindex(
    UI32_T *ifindex_p,
    RULE_TYPE_InOutDirection_T *inout_profile_p)
{
    if (0 == (*ifindex_p))
    {
        (*inout_profile_p) = RULE_TYPE_INBOUND;
        ++(*ifindex_p);
    }
    else
    {
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        if (RULE_TYPE_INBOUND == (*inout_profile_p))
        {
            (*inout_profile_p) = RULE_TYPE_OUTBOUND;
        }
        else if (RULE_TYPE_OUTBOUND == (*inout_profile_p))
        {
            (*inout_profile_p) = RULE_TYPE_INBOUND;
            ++(*ifindex_p);
        }
#else
        ++(*ifindex_p);
#endif
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUIPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next port entry by ifindex
 * INPUT:    ifindex_p, direction_p
 * OUTPUT:   ifindex, port_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Using ifindex = 0 to get first.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextUIPortEntry(
    UI32_T *ifindex_p,
    RULE_TYPE_InOutDirection_T *direction_p,
    RULE_TYPE_UI_PortEntry_T *ui_port_entry_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    while(TRUE == RULE_OM_LocalGetNextInterface(ifindex_p, direction_p))
    {
        result = RULE_OM_GetUIPortEntry(*ifindex_p, *direction_p, ui_port_entry_p);
        if (result == RULE_TYPE_OK)
        {
           return result;
        }
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPolicyMapInstance
 *------------------------------------------------------------------------------
 * PURPOSE:  Assign policy map instance
 * INPUT:    port_entry_p, policy_index, class_index, policy_inst_p
 * OUTPUT:   NONE
 * RETURN:   NONE
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
void
RULE_OM_SetPolicyMapInstance(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_inst_p)
{
    RULE_OM_EnterCriticalSection();
    RULE_OM_LocalSetPolicyMapInstance(ifindex, direction, policy_inst_p);
    RULE_OM_LeaveCriticalSection();
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapInstance
 *------------------------------------------------------------------------------
 * PURPOSE:  Get policy map instance
 * INPUT:    port_entry_p, policy_index, class_index
 * OUTPUT:   The pointer of policy map instance
 * RETURN:   NONE
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
RULE_TYPE_CLASS_INSTANCE_PTR_T
RULE_OM_GetPolicyMapInstance(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction)
{
    RULE_TYPE_CLASS_INSTANCE_PTR_T instance_p;

    RULE_OM_EnterCriticalSection();
    instance_p = RULE_OM_LocalGetPolicyMapInstance(ifindex, direction);
    RULE_OM_LeaveCriticalSectionReturnState(instance_p);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Get_AceEntryByID
 *------------------------------------------------------------------------------
 * PURPOSE:  Get ace entry by index
 * INPUT:    ace_index
 * OUTPUT:   ace_entry
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Get_AceEntryByID(UI32_T ace_index, RULE_TYPE_Ace_Entry_T *ace_entry)
{
    RULE_TYPE_Ace_Entry_T         *ace_p;

    RULE_OM_EnterCriticalSection();
    ace_p = RULE_OM_LocalGetAceByIndex(ace_index);
    if (NULL == ace_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    memcpy(ace_entry, ace_p, sizeof(RULE_TYPE_Ace_Entry_T));
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_ClassifyClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : get class type according to class-map
 * INPUT    : class_map
 * OUTPUT   : class_type
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : class_type could be MAC/IP/IPv6-std/IPv6-ext because they use different selector
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_ClassifyClassMap(const RULE_TYPE_ClassMap_T *class_map, RULE_TYPE_AclType_T *class_type)
{
    return RULE_OM_LocalClassifyClassMap(class_map, class_type);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_ClassifyClassMapByIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get class type according to class-map index
 * INPUT    : class_map_index
 * OUTPUT   : class_type
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : class_type could be MAC/IP/IPv6 because they use different selector
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_ClassifyClassMapByIndex(UI32_T class_map_index, RULE_TYPE_AclType_T *class_type)
{
    RULE_TYPE_ClassMap_T       *class_map_p;
    RULE_TYPE_RETURN_TYPE_T     ret_code;

    RULE_OM_EnterCriticalSection();
    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);

    ret_code = RULE_OM_LocalClassifyClassMap(class_map_p, class_type);
    RULE_OM_LeaveCriticalSectionReturnState(ret_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_AddCosReference
 *------------------------------------------------------------------------------
 * PURPOSE  : add cos reference if action reference to COS
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : keep the policy-map elements' information if their actions reference to COS
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_AddCosReference(UI32_T policy_map_index, UI32_T class_map_index)
{
    RULE_TYPE_PolicyElement_T   *policy_element_p;
    RULE_TYPE_Action_T          *action_p;

    RULE_OM_EnterCriticalSection();

    policy_element_p = RULE_OM_LocalGetPolicyMapElement(policy_map_index, class_map_index);
    if (NULL == policy_element_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    action_p = RULE_OM_LocalGetActionEntryByIndex(policy_element_p->action_index);
    if (NULL == action_p)
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL); /* bad index */

    /* NOTICE:
     *  different actions may exist at the same time
     *  therefore, do not use if - else if statement
     */
    if (action_p->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_TOS)
    {
        if (RULE_TYPE_OK != RULE_OM_LocalAddIpPrecedenceReference(action_p->pkt_new_tos, policy_map_index, class_map_index))
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    if (action_p->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_DSCP)
    {
        if (RULE_TYPE_OK != RULE_OM_LocalAddIpDscpReference(action_p->pkt_new_dscp, policy_map_index, class_map_index))
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    if (action_p->action_bitmap & RULE_TYPE_ACTION_GREEN_PKT_NEW_DSCP)
    {
        if (RULE_TYPE_OK != RULE_OM_LocalAddIpDscpReference(action_p->green_pkt_new_dscp, policy_map_index, class_map_index))
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    if (action_p->action_bitmap & RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP)
    {
        if (RULE_TYPE_OK != RULE_OM_LocalAddIpDscpReference(action_p->yellow_pkt_new_dscp, policy_map_index, class_map_index))
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    if (action_p->action_bitmap & RULE_TYPE_ACTION_RED_PKT_NEW_DSCP)
    {
        if (RULE_TYPE_OK != RULE_OM_LocalAddIpDscpReference(action_p->red_pkt_new_dscp, policy_map_index, class_map_index))
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RemoveCosReference
 *------------------------------------------------------------------------------
 * PURPOSE  : remove cos reference if action reference to COS
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
void RULE_OM_RemoveCosReference(UI32_T policy_map_index, UI32_T class_map_index)
{
    RULE_OM_EnterCriticalSection();
    RULE_OM_LocalRemoveCosReference(policy_map_index, class_map_index);
    RULE_OM_LeaveCriticalSection();

}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextIpDscpReference
 *------------------------------------------------------------------------------
 * PURPOSE  : get next IP DSCP reference
 * INPUT    : ip_dscp, index
 * OUTPUT   : policy_map_index, class_map_index
 * RETURN   : error code (OK / FAIL)
 * NOTE     : *index = 0 implies get first entry
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextIpDscpReference(UI8_T ip_dscp, UI32_T *index, UI32_T *policy_map_index, UI32_T *class_map_index)
{
#if (RULE_CTRL_USE_REF_LIST == TRUE)
    UI32_T                          loop_counter;
    RULE_OM_CosReferenceEntry_T     *entry_iter_p;

    if ((NULL == index) || (NULL == policy_map_index) || (NULL == class_map_index))
        return RULE_TYPE_FAIL;

    if (RULE_OM_IS_BAD_IP_DSCP(ip_dscp))
        return RULE_TYPE_FAIL;

    loop_counter = 0;

    for (entry_iter_p = dscp_reference_list[ip_dscp]; NULL != entry_iter_p;
        entry_iter_p = entry_iter_p->next_entry)
    {
        loop_counter++;
        if (loop_counter <= *index)
            continue;

        *index = loop_counter;
        *policy_map_index = entry_iter_p->policy_map_index;
        *class_map_index = entry_iter_p->class_map_index;

        return RULE_TYPE_OK;
    }
#endif /* #if (RULE_CTRL_USE_REF_LIST == TRUE) */

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextIpPrecedenceReference
 *------------------------------------------------------------------------------
 * PURPOSE  : get next IP Precedence reference
 * INPUT    : ip_precedence, index
 * OUTPUT   : policy_map_index, class_map_index
 * RETURN   : error code (OK / FAIL)
 * NOTE     : *index = 0 implies get first entry
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextIpPrecedenceReference(UI8_T ip_precedence, UI32_T *index, UI32_T *policy_map_index, UI32_T *class_map_index)
{
#if (RULE_CTRL_USE_REF_LIST == TRUE)
    UI32_T                          loop_counter;
    RULE_OM_CosReferenceEntry_T     *entry_iter_p;

    if ((NULL == index) || (NULL == policy_map_index) || (NULL == class_map_index))
        return RULE_TYPE_FAIL;

    if (RULE_OM_IS_BAD_IP_PRECEDENCE(ip_precedence))
        return RULE_TYPE_FAIL;

    loop_counter = 0;

    for (entry_iter_p = precedence_reference_list[ip_precedence]; NULL != entry_iter_p;
        entry_iter_p = entry_iter_p->next_entry)
    {
        loop_counter++;
        if (loop_counter <= *index)
            continue;

        *index = loop_counter;
        *policy_map_index = entry_iter_p->policy_map_index;
        *class_map_index = entry_iter_p->class_map_index;

        return RULE_TYPE_OK;
    }
#endif /* #if (RULE_CTRL_USE_REF_LIST == TRUE) */

    return RULE_TYPE_FAIL;
}

#if (SYS_CPNT_DAI == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_ArpACLCheckForDai
 *------------------------------------------------------------------------------
 * PURPOSE:  for ARP INSPETCION (DAI)
 *            to check if match the existed arp rules with arp packets
 * INPUT:    arp_acl_name - check which arp acl
 *           arp_pkt_p    - compared arp packet
 * OUTPUT:   b_acl_drop   - is drop
 * RETURN:   RULE_TYPE_OK/RULE_TYPE_FAIL
 * NOTE:     RULE_TYPE_OK means match rule,
 *           RULE_TYPE_FAIL means no match rule.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_ArpACLCheckForDai(
    const char *arp_acl_name,
    RULE_TYPE_ArpPktFormat_T *arp_pkt_p,
    BOOL_T *b_acl_drop)
{
    UI32_T acl_index, ace_index, len;

    RULE_TYPE_ArpAce_Entry_T *arp_ace_p;
    RULE_TYPE_Acl_T *acl_p;
    RULE_TYPE_Ace_Entry_T *ace_entry_p;

    BOOL_T b_check_mac = TRUE;

    *b_acl_drop = FALSE;

    RULE_OM_EnterCriticalSection();

    if (RULE_OM_LocalGetAclIdByName(arp_acl_name, &acl_index) != RULE_TYPE_OK)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    ASSERT(NULL != acl_p);

    if (RULE_TYPE_ARP_ACL != acl_p->acl_type)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    if (0 == acl_p->ace_count)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_EMPTY_ACL);
    }

    for (ace_index = 0;
         RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_entry_p); )
    {
        arp_ace_p = &ace_entry_p->u.arp;

        b_check_mac = TRUE;

        /* 1. check opcode
         */
        if ( (arp_ace_p->aceArpType != arp_pkt_p->opcode) && ( RULE_TYPE_ARP_BOTH != arp_ace_p->aceArpType) )
        {
            continue;
        }

        /* 2. check Src IP
         */
        if ( (arp_pkt_p->srcIp           & arp_ace_p->aceSenderIpAddrBitmask) !=
             (arp_ace_p->aceSenderIpAddr & arp_ace_p->aceSenderIpAddrBitmask) )
        {
            continue;
        }

        /* 3. check Src MAC
         */
        for ( len=0; len<SYS_ADPT_MAC_ADDR_LEN; len++ )
        {
            if ( (arp_pkt_p->srcMacAddr[len]        & arp_ace_p->aceSenderMacAddrBitmask[len]) !=
                 (arp_ace_p->aceSenderMacAddr[len]  & arp_ace_p->aceSenderMacAddrBitmask[len]) )
            {
                b_check_mac = FALSE;
                break;
            }
        }

        if ( FALSE == b_check_mac )
        {
            continue;
        }

        /* 4. check Dst IP
         */
        if ( (arp_pkt_p->dstIp           & arp_ace_p->aceTargetIpAddrBitmask) !=
             (arp_ace_p->aceTargetIpAddr & arp_ace_p->aceTargetIpAddrBitmask) )
        {
            continue;
        }

        /* 5. check Dst MAC
         */
        for ( len=0; len<SYS_ADPT_MAC_ADDR_LEN; len++ )
        {
            if ( (arp_pkt_p->dstMacAddr[len]        & arp_ace_p->aceTargetMacAddrBitmask[len]) !=
                 (arp_ace_p->aceTargetMacAddr[len]  & arp_ace_p->aceTargetMacAddrBitmask[len]) )
            {
                b_check_mac = FALSE;
                break;
            }
        }

        if ( FALSE == b_check_mac )
        {
            continue;
        }

        /* match rule
         */
        if ( RULE_TYPE_ACE_DENY == ace_entry_p->access )
        {
            /* deny
             */
            *b_acl_drop = TRUE;
        }
        else
        {
            /* permit
             */
            *b_acl_drop = FALSE;
        }

        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }

    /* no match rule
     */
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}
#endif /* #if (SYS_CPNT_DAI == TRUE) */

/*------------------------------------------------------------------------------
 * FUNCTION NAME:  RULE_OM_SetDebugFlag
 *------------------------------------------------------------------------------
 * PURPOSE  : set backdoor debug flag
 * INPUT    : debug_flag
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
void RULE_OM_SetDebugFlag(UI32_T debug_flag)
{
    rule_debug_flag = debug_flag;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME:  RULE_OM_GetDebugFlag
 *------------------------------------------------------------------------------
 * PURPOSE  : get backdoor debug flag
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : debug flag
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetDebugFlag()
{
    return rule_debug_flag;
}

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) || (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalCreateWorkingAcl
 *------------------------------------------------------------------------------
 * PURPOSE: Create working ACL
 * INPUT  : acl_type    - ACL type
 * OUTPUT : acl_index_p - ACL index
 * RETURN : Error code (OK / FAIL)
 * NOTE   : none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalCreateWorkingAcl(
    RULE_TYPE_AclType_T acl_type,
    UI32_T *acl_index_p)
{
    static UI32_T working_acl_index = 1;

    UI32_T res;
    RULE_TYPE_Acl_T *acl_p;
    char acl_name[SYS_ADPT_ACL_MAX_NAME_LEN + 1];

    if (NULL == acl_index_p)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    do
    {
        UI32_T temp;

        sprintf(acl_name, "~w%05lu", working_acl_index++);

        res = RULE_OM_LocalGetAclIdByName(acl_name,
                                          &temp);
    } while (res == RULE_TYPE_OK);

    res = RULE_OM_LocalAllocateAclWithRange(SYS_ADPT_MAX_NBRS_OF_ACL, RULE_TYPE_MAX_NBRS_OF_ACL, acl_index_p);
    if (res != RULE_TYPE_OK)
    {
        *acl_index_p = 0;
        return res;
    }

    acl_p = RULE_OM_LocalGetAclByIndex(*acl_index_p);
    acl_p->acl_type  = acl_type;
    RULE_OM_SET_ENTRY_ACTIVE(acl_p->row_status);
    strncpy((char *)acl_p->acl_name, acl_name, SYS_ADPT_ACL_MAX_NAME_LEN);
    acl_p->acl_name[SYS_ADPT_ACL_MAX_NAME_LEN] = '\0';

    acl_p->flag_hide = TRUE;

    return RULE_TYPE_OK;
}
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE || SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL */

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalCountDifferBitNumber
 *------------------------------------------------------------------------------
 * PURPOSE: Count the differ bit number for two memeory
 * INPUT  : val1      - memory 1
 *          mask1     - mask of memory 1
 *          val2      - memory 2
 *          mask2     - mask of memory 2
 *          byte_size - how many size in byte
 * OUTPUT : none
 * RETURN : differ bit number
 * NOTE   : none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalCountDifferBitNumber(
    const UI8_T   *val1,
    const UI8_T   *mask1,
    const UI8_T   *val2,
    const UI8_T   *mask2,
    UI32_T  byte_size)
{
    UI32_T  i;
    UI32_T  res;

    if (0 == byte_size)
    {
        return 0;
    }

    res = 0;
    for (i = 0; i < byte_size; ++ i)
    {
        UI8_T   byte1 = val1[i] & mask1[i];
        UI8_T   byte2 = val2[i] & mask2[i];
        UI8_T   v     = byte1 ^ byte2;

        for (; v; ++res)
        {
            v &= v -1;
        }
    }

    return res;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalMergerValMask
 *------------------------------------------------------------------------------
 * PURPOSE: Merger value and mask
 * INPUT  : val1      - memory 1
 *          mask1     - mask of memory 1
 *          val2      - memory 2
 *          mask2     - mask of memory 2
 *          byte_size - how many size in byte
 * OUTPUT : val2      - memory 2
 *          mask2     - mask of memory 2
 * RETURN : Error code (OK / FAIL)
 * NOTE   : none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalMergerValMask(
    const UI8_T *val1,
    const UI8_T *mask1,
    UI8_T       *val2,
    UI8_T       *mask2,
    UI32_T      byte_size)
{
    UI32_T  i;
    UI32_T  res;

    res = 0;
    for (i = 0; i < byte_size; ++i)
    {
        UI8_T   byte1 = val1[i] & mask1[i];
        UI8_T   byte2 = val2[i] & mask2[i];
        UI8_T   v     = byte1 ^ byte2;

        if (v)
        {
            val2[i]  &= (~v);
            mask2[i] &= (~v);
            break;
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalCompressMacAce
 *------------------------------------------------------------------------------
 * PURPOSE: Compress MAC ACE.
 * INPUT  : src_ace_p - ACE
 *          agg_ace_p - ACE
 * OUTPUT : agg_ace_p - ACE
 * RETURN : RULE_TYPE_OK, src_ace_p can be compressed into agg_ace_p. Else
            the two ACE can not be compressed.
 * NOTE   : none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalCompressMacAce(
    RULE_TYPE_MacAce_Entry_T   *src_ace_p,
    RULE_TYPE_MacAce_Entry_T   *agg_ace_p)
{
    UI32_T  count;
    RULE_TYPE_MacAce_Entry_T    new_ace;

    memcpy(&new_ace, agg_ace_p, sizeof(new_ace));

    if (src_ace_p->acePktformat != agg_ace_p->acePktformat)
    {
        return RULE_TYPE_FAIL;
    }

    if (0 != memcmp(src_ace_p->aceDestMacAddrBitmask,
                    agg_ace_p->aceDestMacAddrBitmask,
                    sizeof(src_ace_p->aceDestMacAddrBitmask)))
    {
        return RULE_TYPE_FAIL;
    }

    count = RULE_OM_LocalCountDifferBitNumber(src_ace_p->aceDestMacAddr,
                                              src_ace_p->aceDestMacAddrBitmask,
                                              agg_ace_p->aceDestMacAddr,
                                              agg_ace_p->aceDestMacAddrBitmask,
                                              sizeof(src_ace_p->aceDestMacAddr));
    if (1 < count)
    {
        return RULE_TYPE_FAIL;
    }
    else if (1 == count)
    {
        RULE_OM_LocalMergerValMask(src_ace_p->aceDestMacAddr,
                                   src_ace_p->aceDestMacAddrBitmask,
                                   new_ace.aceDestMacAddr,
                                   new_ace.aceDestMacAddrBitmask,
                                   sizeof(src_ace_p->aceDestMacAddr));
    }

    if (0 != memcmp(src_ace_p->aceSourceMacAddrBitmask,
                    agg_ace_p->aceSourceMacAddrBitmask,
                    sizeof(src_ace_p->aceSourceMacAddrBitmask)))
    {
        return RULE_TYPE_FAIL;
    }

    count = RULE_OM_LocalCountDifferBitNumber(src_ace_p->aceSourceMacAddr,
                                              src_ace_p->aceSourceMacAddrBitmask,
                                              agg_ace_p->aceSourceMacAddr,
                                              agg_ace_p->aceSourceMacAddrBitmask,
                                              sizeof(src_ace_p->aceSourceMacAddr));
    if (1 < count)
    {
        return RULE_TYPE_FAIL;
    }
    else if (1 == count)
    {
        RULE_OM_LocalMergerValMask(src_ace_p->aceSourceMacAddr,
                                   src_ace_p->aceSourceMacAddrBitmask,
                                   new_ace.aceSourceMacAddr,
                                   new_ace.aceSourceMacAddrBitmask,
                                   sizeof(src_ace_p->aceSourceMacAddr));
    }

    if (src_ace_p->aceVidOp != agg_ace_p->aceVidOp)
    {
        return RULE_TYPE_FAIL;
    }

    if (VAL_diffServMacAceVidOp_equal == src_ace_p->aceVidOp)
    {
        if (0 != memcmp(&src_ace_p->aceVidBitmask,
                        &agg_ace_p->aceVidBitmask,
                        sizeof(src_ace_p->aceVidBitmask)))
        {
            return RULE_TYPE_FAIL;
        }

        count = RULE_OM_LocalCountDifferBitNumber((UI8_T*)&src_ace_p->aceMinVid,
                                                  (UI8_T*)&src_ace_p->aceVidBitmask,
                                                  (UI8_T*)&agg_ace_p->aceMinVid,
                                                  (UI8_T*)&agg_ace_p->aceVidBitmask,
                                                  sizeof(src_ace_p->aceMinVid));
        if (1 < count)
        {
            return RULE_TYPE_FAIL;
        }
        else if (1 == count)
        {
            RULE_OM_LocalMergerValMask((UI8_T*)&src_ace_p->aceMinVid,
                                       (UI8_T*)&src_ace_p->aceVidBitmask,
                                       (UI8_T*)&new_ace.aceMinVid,
                                       (UI8_T*)&new_ace.aceVidBitmask,
                                       sizeof(src_ace_p->aceMinVid));
        }
    }
    else if (VAL_diffServMacAceEtherTypeOp_range == src_ace_p->aceEtherTypeOp)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceEtherTypeOp != agg_ace_p->aceEtherTypeOp)
    {
        return RULE_TYPE_FAIL;
    }

    if (VAL_diffServMacAceEtherTypeOp_equal == src_ace_p->aceEtherTypeOp)
    {
        if (0 != memcmp(&src_ace_p->aceEtherTypeBitmask,
                        &agg_ace_p->aceEtherTypeBitmask,
                        sizeof(src_ace_p->aceEtherTypeBitmask)))
        {
            return RULE_TYPE_FAIL;
        }

        count = RULE_OM_LocalCountDifferBitNumber((UI8_T*)&src_ace_p->aceMinEtherType,
                                                  (UI8_T*)&src_ace_p->aceEtherTypeBitmask,
                                                  (UI8_T*)&agg_ace_p->aceMinEtherType,
                                                  (UI8_T*)&agg_ace_p->aceEtherTypeBitmask,
                                                  sizeof(src_ace_p->aceMinEtherType));
        if (1 < count)
        {
            return RULE_TYPE_FAIL;
        }
        else if (1 == count)
        {
            RULE_OM_LocalMergerValMask((UI8_T*)&src_ace_p->aceMinEtherType,
                                       (UI8_T*)&src_ace_p->aceEtherTypeBitmask,
                                       (UI8_T*)&new_ace.aceMinEtherType,
                                       (UI8_T*)&new_ace.aceEtherTypeBitmask,
                                       sizeof(src_ace_p->aceMinEtherType));
        }
    }
    else if (VAL_diffServMacAceEtherTypeOp_range == src_ace_p->aceEtherTypeOp)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceCoSOp != agg_ace_p->aceCoSOp)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceCoS != agg_ace_p->aceCoS)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceCFIOp != agg_ace_p->aceCFIOp)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceCFI != agg_ace_p->aceCFI)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceCosOp != agg_ace_p->aceCosOp)
    {
        return RULE_TYPE_FAIL;
    }

    if (VAL_diffServMacAceCosOp_equal == src_ace_p->aceCosOp)
    {
        if (src_ace_p->aceCosBitmask != agg_ace_p->aceCosBitmask)
        {
            return RULE_TYPE_FAIL;
        }

        count = RULE_OM_LocalCountDifferBitNumber(&src_ace_p->aceMinCos,
                                                  &src_ace_p->aceCosBitmask,
                                                  &agg_ace_p->aceMinCos,
                                                  &agg_ace_p->aceCosBitmask,
                                                  sizeof(src_ace_p->aceMinCos));
        if (1 < count)
        {
            return RULE_TYPE_FAIL;
        }
        else if (1 == count)
        {
            RULE_OM_LocalMergerValMask(&src_ace_p->aceMinCos,
                                       &src_ace_p->aceCosBitmask,
                                       &new_ace.aceMinCos,
                                       &new_ace.aceCosBitmask,
                                       sizeof(src_ace_p->aceMinCos));
        }
    }
    else if (VAL_diffServMacAceCosOp_range == src_ace_p->aceCosOp)
    {
        return RULE_TYPE_FAIL;
    }

    /* DO NOT MERGER SOURCE PORT, THE PORT NUMBER SHOULD BE USED TO
     * CONVERTE BACK TO PHY-PORT FOR CHIP
     */
    if (src_ace_p->aceSourcePortOp != agg_ace_p->aceSourcePortOp)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceMinSourcePort != agg_ace_p->aceMinSourcePort)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceMaxSourcePort != agg_ace_p->aceMaxSourcePort)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceSourcePortBitmask != agg_ace_p->aceSourcePortBitmask)
    {
        return RULE_TYPE_FAIL;
    }

    memcpy(agg_ace_p, &new_ace, sizeof(*agg_ace_p));
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalCompressIpAce
 *------------------------------------------------------------------------------
 * PURPOSE: Compress IP ACE.
 * INPUT  : src_ace_p - ACE
 *          agg_ace_p - ACE
 * OUTPUT : agg_ace_p - ACE
 * RETURN : RULE_TYPE_OK, src_ace_p can be compressed into agg_ace_p. Else
            the two ACE can not be compressed.
 * NOTE   : none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalCompressIpAce(
    RULE_TYPE_IpAce_Entry_T   *src_ace_p,
    RULE_TYPE_IpAce_Entry_T   *agg_ace_p)
{
    UI32_T  count;
    RULE_TYPE_IpAce_Entry_T    new_ace;

    memcpy(&new_ace, agg_ace_p, sizeof(new_ace));

    if (src_ace_p->acePrecedence != agg_ace_p->acePrecedence)
    {
        return RULE_TYPE_FAIL;
    }

    if (0 != memcmp(&src_ace_p->aceSourceIpAddrBitmask,
                    &agg_ace_p->aceSourceIpAddrBitmask,
                    sizeof(src_ace_p->aceSourceIpAddrBitmask)))
    {
        return RULE_TYPE_FAIL;
    }

    count = RULE_OM_LocalCountDifferBitNumber((UI8_T*)&src_ace_p->aceSourceIpAddr,
                                              (UI8_T*)&src_ace_p->aceSourceIpAddrBitmask,
                                              (UI8_T*)&agg_ace_p->aceSourceIpAddr,
                                              (UI8_T*)&agg_ace_p->aceSourceIpAddrBitmask,
                                              sizeof(src_ace_p->aceSourceIpAddr));

    if (1 < count)
    {
        return RULE_TYPE_FAIL;
    }
    else if (1 == count)
    {
        RULE_OM_LocalMergerValMask((UI8_T*)&src_ace_p->aceSourceIpAddr,
                                   (UI8_T*)&src_ace_p->aceSourceIpAddrBitmask,
                                   (UI8_T*)&new_ace.aceSourceIpAddr,
                                   (UI8_T*)&new_ace.aceSourceIpAddrBitmask,
                                   sizeof(src_ace_p->aceSourceIpAddr));

    }

    if (0 != memcmp(&src_ace_p->aceDestIpAddrBitmask,
                    &agg_ace_p->aceDestIpAddrBitmask,
                    sizeof(src_ace_p->aceDestIpAddrBitmask)))
    {
        return RULE_TYPE_FAIL;
    }

    count = RULE_OM_LocalCountDifferBitNumber((UI8_T*)&src_ace_p->aceDestIpAddr,
                                              (UI8_T*)&src_ace_p->aceDestIpAddrBitmask,
                                              (UI8_T*)&agg_ace_p->aceDestIpAddr,
                                              (UI8_T*)&agg_ace_p->aceDestIpAddrBitmask,
                                              sizeof(src_ace_p->aceDestIpAddr));
    if (1 < count)
    {
        return RULE_TYPE_FAIL;
    }
    else if (1 == count)
    {
        RULE_OM_LocalMergerValMask((UI8_T*)&src_ace_p->aceDestIpAddr,
                                   (UI8_T*)&src_ace_p->aceDestIpAddrBitmask,
                                   (UI8_T*)&new_ace.aceDestIpAddr,
                                   (UI8_T*)&new_ace.aceDestIpAddrBitmask,
                                   sizeof(src_ace_p->aceDestIpAddr));
    }

    if (src_ace_p->aceProtocol != agg_ace_p->aceProtocol)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->acePrec != agg_ace_p->acePrec)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceTos != agg_ace_p->aceTos)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceDscp != agg_ace_p->aceDscp)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceDestPortOp != agg_ace_p->aceDestPortOp)
    {
        return RULE_TYPE_FAIL;
    }

    if (VAL_diffServIpAceDestPortOp_equal == src_ace_p->aceDestPortOp)
    {
        if (src_ace_p->aceDestPortBitmask != agg_ace_p->aceDestPortBitmask)
        {
            return RULE_TYPE_FAIL;
        }

        count = RULE_OM_LocalCountDifferBitNumber((UI8_T*)&src_ace_p->aceMinDestPort,
                                                  (UI8_T*)&src_ace_p->aceDestPortBitmask,
                                                  (UI8_T*)&agg_ace_p->aceMinDestPort,
                                                  (UI8_T*)&agg_ace_p->aceDestPortBitmask,
                                                  sizeof(src_ace_p->aceMinDestPort));
        if (1 < count)
        {
            return RULE_TYPE_FAIL;
        }
        else if (1 == count)
        {
            RULE_OM_LocalMergerValMask((UI8_T*)&src_ace_p->aceMinDestPort,
                                       (UI8_T*)&src_ace_p->aceDestPortBitmask,
                                       (UI8_T*)&new_ace.aceMinDestPort,
                                       (UI8_T*)&new_ace.aceDestPortBitmask,
                                       sizeof(src_ace_p->aceMinDestPort));
        }
    }
    else if (VAL_diffServIpAceDestPortOp_range == src_ace_p->aceDestPortOp)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceSourcePortOp != agg_ace_p->aceSourcePortOp)
    {
        return RULE_TYPE_FAIL;
    }

    if (VAL_diffServIpAceSourcePortOp_equal == src_ace_p->aceSourcePortOp)
    {
        if (src_ace_p->aceSourcePortBitmask != agg_ace_p->aceSourcePortBitmask)
        {
            return RULE_TYPE_FAIL;
        }

        count = RULE_OM_LocalCountDifferBitNumber((UI8_T*)&src_ace_p->aceMinSourcePort,
                                                  (UI8_T*)&src_ace_p->aceSourcePortBitmask,
                                                  (UI8_T*)&agg_ace_p->aceMinSourcePort,
                                                  (UI8_T*)&agg_ace_p->aceSourcePortBitmask,
                                                  sizeof(src_ace_p->aceMinSourcePort));
        if (1 < count)
        {
            return RULE_TYPE_FAIL;
        }
        else if (1 == count)
        {
            RULE_OM_LocalMergerValMask((UI8_T*)&src_ace_p->aceMinSourcePort,
                                       (UI8_T*)&src_ace_p->aceSourcePortBitmask,
                                       (UI8_T*)&new_ace.aceMinSourcePort,
                                       (UI8_T*)&new_ace.aceSourcePortBitmask,
                                       sizeof(src_ace_p->aceMinSourcePort));
        }
    }
    else if (VAL_diffServIpAceSourcePortOp_range == src_ace_p->aceSourcePortOp)
    {
        return RULE_TYPE_FAIL;
    }

    count = RULE_OM_LocalCountDifferBitNumber((UI8_T*)&src_ace_p->aceControlCode,
                                              (UI8_T*)&src_ace_p->aceControlCodeBitmask,
                                              (UI8_T*)&agg_ace_p->aceControlCode,
                                              (UI8_T*)&agg_ace_p->aceControlCodeBitmask,
                                              sizeof(src_ace_p->aceControlCode));
    if (1 < count)
    {
        return RULE_TYPE_FAIL;
    }
    else if (1 == count)
    {
        RULE_OM_LocalMergerValMask((UI8_T*)&src_ace_p->aceControlCode,
                                   (UI8_T*)&src_ace_p->aceControlCodeBitmask,
                                   (UI8_T*)&new_ace.aceControlCode,
                                   (UI8_T*)&new_ace.aceControlCodeBitmask,
                                   sizeof(src_ace_p->aceControlCode));
    }

    if (src_ace_p->aceFragmentInfo != agg_ace_p->aceFragmentInfo)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceIpOptionOp != agg_ace_p->aceIpOptionOp)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceMinIpOption != agg_ace_p->aceMinIpOption)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceMaxIpOption != agg_ace_p->aceMaxIpOption)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceIcmpType != agg_ace_p->aceIcmpType)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceIcmpCode != agg_ace_p->aceIcmpCode)
    {
        return RULE_TYPE_FAIL;
    }

    memcpy(agg_ace_p, &new_ace, sizeof(*agg_ace_p));
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalConvertIpv6Prefix2Mask
 *------------------------------------------------------------------------------
 * PURPOSE: Convert prefix to mask
 * INPUT  : prefix_length - prefix length
 * OUTPUT : mask          - mask. The mask should be a 16 byte memory.
 * RETURN : Error code
 * NOTE   : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_OM_LocalConvertIpv6Prefix2Mask(
    UI8_T prefix_length,
    UI8_T *mask)
{
    UI32_T eigth_bits, one_bit, i;

    eigth_bits = prefix_length/8;
    for(i=0;i<eigth_bits;i++)
    {
        mask[i] = 0xff;
    }

    one_bit = prefix_length%8;
    if(one_bit != 0)
    {
        mask[eigth_bits]=0;
        for(i=0;i<8;i++)
        {
            if(i < one_bit)
            {
                mask[eigth_bits] = mask[eigth_bits]|(1<<i);
            }
            else
            {
                mask[eigth_bits] = mask[eigth_bits] << 1;
            }
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalConvertIpv6Mask2Prefix
 *------------------------------------------------------------------------------
 * PURPOSE: Convert mask to prefix length
 * INPUT  : mask          - mask. The mask should be a 16 byte memory.
 * OUTPUT : prefix_length_p - prefix length
 * RETURN : Error code
 * NOTE   : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_OM_LocalConvertIpv6Mask2Prefix(
    const UI8_T *mask,
    UI8_T *prefix_length_p)
{
    UI32_T byte_cnt, bit_cnt;

    ASSERT(mask != NULL);
    ASSERT(prefix_length_p != NULL);

    *prefix_length_p = 0;

    for (byte_cnt = 0; byte_cnt < SYS_ADPT_IPV6_ADDR_LEN; byte_cnt++)
    {
        if (mask[byte_cnt] == 0xff)
        {
            *prefix_length_p += 8;
        }
        else
        {
            for (bit_cnt = 7; bit_cnt > 0 ; bit_cnt--)
            {
                if (0 == (mask[byte_cnt] & (1 << bit_cnt)))
                {
                    break;
                }

                 *prefix_length_p += 1;
            }

            break;
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalCompressIpv6Ace
 *------------------------------------------------------------------------------
 * PURPOSE: Compress IP6 ACE.
 * INPUT  : src_ace_p - ACE
 *          agg_ace_p - ACE
 * OUTPUT : agg_ace_p - ACE
 * RETURN : RULE_TYPE_OK, src_ace_p can be compressed into agg_ace_p. Else
            the two ACE can not be compressed.
 * NOTE   : none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalCompressIpv6Ace(
    RULE_TYPE_Ipv6Ace_Entry_T   *src_ace_p,
    RULE_TYPE_Ipv6Ace_Entry_T   *agg_ace_p)
{
    UI32_T  count;
    RULE_TYPE_Ipv6Ace_Entry_T    new_ace;

    memcpy(&new_ace, agg_ace_p, sizeof(new_ace));

    if (src_ace_p->aceDscp != agg_ace_p->aceDscp)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceFlowLabel != agg_ace_p->aceFlowLabel)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_ace_p->aceNextHeader != agg_ace_p->aceNextHeader)
    {
        return RULE_TYPE_FAIL;
    }

    if (0 != memcmp(&src_ace_p->aceDestIpAddrBitmask,
                    &agg_ace_p->aceDestIpAddrBitmask,
                    sizeof(src_ace_p->aceDestIpAddrBitmask)))
    {
        return RULE_TYPE_FAIL;
    }

    count = RULE_OM_LocalCountDifferBitNumber(src_ace_p->aceDestIpAddr,
                                              src_ace_p->aceDestIpAddrBitmask,
                                              agg_ace_p->aceDestIpAddr,
                                              agg_ace_p->aceDestIpAddrBitmask,
                                              sizeof(src_ace_p->aceDestIpAddr));

    if (1 < count)
    {
        return RULE_TYPE_FAIL;
    }
    else if (1 == count)
    {
        RULE_OM_LocalMergerValMask((UI8_T*)&src_ace_p->aceDestIpAddr,
                                   (UI8_T*)&src_ace_p->aceDestIpAddrBitmask,
                                   (UI8_T*)&new_ace.aceDestIpAddr,
                                   (UI8_T*)&new_ace.aceDestIpAddrBitmask,
                                   sizeof(src_ace_p->aceDestIpAddr));
    }

    if (0 != memcmp(&src_ace_p->aceSourceIpAddrBitmask,
                    &agg_ace_p->aceSourceIpAddrBitmask,
                    sizeof(src_ace_p->aceSourceIpAddrBitmask)))
    {
        return RULE_TYPE_FAIL;
    }

    count = RULE_OM_LocalCountDifferBitNumber(src_ace_p->aceSourceIpAddr,
                                              src_ace_p->aceSourceIpAddrBitmask,
                                              agg_ace_p->aceSourceIpAddr,
                                              agg_ace_p->aceSourceIpAddrBitmask,
                                              sizeof(src_ace_p->aceSourceIpAddr));

    if (1 < count)
    {
        return RULE_TYPE_FAIL;
    }
    else if (1 == count)
    {
        RULE_OM_LocalMergerValMask((UI8_T*)&src_ace_p->aceSourceIpAddr,
                                   (UI8_T*)&src_ace_p->aceSourceIpAddrBitmask,
                                   (UI8_T*)&new_ace.aceSourceIpAddr,
                                   (UI8_T*)&new_ace.aceSourceIpAddrBitmask,
                                   sizeof(src_ace_p->aceSourceIpAddr));
    }

    memcpy(agg_ace_p, &new_ace, sizeof(*agg_ace_p));
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalCompressAce
 *------------------------------------------------------------------------------
 * PURPOSE:  Compress two ACE.
 * INPUT  :  src_ace_p  - ACE
 *           agg_ace_p  - ACE
 * OUTPUT :  agg_ace_p  - Compressed ACE
 * RETURN :  RULE_TYPE_OK, two ACE be compressed into agg_ace_p;
 *           else, two ACE can't be compressed.
 * NOTE   :  none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalCompressAce(
    RULE_TYPE_Ace_Entry_T   *src_ace_p,
    RULE_TYPE_Ace_Entry_T   *agg_ace_p)
{
    UI32_T                  res;

    switch (src_ace_p->ace_type)
    {
        case RULE_TYPE_MAC_ACL:
            break;

        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            if (agg_ace_p->ace_type == RULE_TYPE_IPV6_ACL ||
                agg_ace_p->ace_type == RULE_TYPE_IPV6_STD_ACL ||
                agg_ace_p->ace_type == RULE_TYPE_IPV6_EXT_ACL)
            {
                return RULE_TYPE_FAIL;
            }

            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            if (agg_ace_p->ace_type == RULE_TYPE_IP_ACL ||
                agg_ace_p->ace_type == RULE_TYPE_IP_STD_ACL ||
                agg_ace_p->ace_type == RULE_TYPE_IP_EXT_ACL)
            {
                return RULE_TYPE_FAIL;
            }

            break;
    }

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    if (strcmp((char *)src_ace_p->time_range_name, (char *)agg_ace_p->time_range_name))
    {
        return RULE_TYPE_FAIL;
    }
#endif /* SYS_CPNT_TIME_BASED_ACL */

    if (src_ace_p->access != agg_ace_p->access)
    {
        return RULE_TYPE_FAIL;
    }

    switch (src_ace_p->ace_type)
    {
        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            if (agg_ace_p->ace_type == RULE_TYPE_IP_ACL ||
                agg_ace_p->ace_type == RULE_TYPE_IP_STD_ACL ||
                agg_ace_p->ace_type == RULE_TYPE_IP_EXT_ACL)
            {
#if (RULE_TYPE_ACE_MAC_IP_MIX == TRUE)
                res = RULE_OM_LocalCompressMacAce(&src_ace_p->u.mac,
                                                &agg_ace_p->u.mac);
                if (RULE_TYPE_OK != res)
                {
                    return res;
                }
#endif /* RULE_TYPE_ACE_MAC_IP_MIX */

                res = RULE_OM_LocalCompressIpAce(&src_ace_p->u.ip,
                                                 &agg_ace_p->u.ip);
                if (RULE_TYPE_OK != res)
                {
                    return res;
                }
            }

            break;

        case RULE_TYPE_MAC_ACL:
            {
                res = RULE_OM_LocalCompressMacAce(&src_ace_p->u.mac,
                                                &agg_ace_p->u.mac);
                if (RULE_TYPE_OK != res)
                {
                    return res;
                }
            }
            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            if (agg_ace_p->ace_type == RULE_TYPE_IPV6_ACL ||
                agg_ace_p->ace_type == RULE_TYPE_IPV6_STD_ACL ||
                agg_ace_p->ace_type == RULE_TYPE_IPV6_EXT_ACL)
            {
#if (RULE_TYPE_ACE_MAC_IP_MIX == TRUE)
                res = RULE_OM_LocalCompressMacAce(&src_ace_p->u.mac,
                                                &agg_ace_p->u.mac);
                if (RULE_TYPE_OK != res)
                {
                    return res;
                }
#endif /* RULE_TYPE_ACE_MAC_IP_MIX */

                res = RULE_OM_LocalCompressIpv6Ace(&src_ace_p->u.ipv6,
                                                   &agg_ace_p->u.ipv6);
                if (RULE_TYPE_OK != res)
                {
                    return res;
                }
            }

            break;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalCompressAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Compress all ACE on the specified ACL
 * INPUT  :  acl_index  - ACL index
 * OUTPUT :  none
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalCompressAcl(
    UI32_T acl_index)
{
    RULE_TYPE_Acl_T *acl_p = RULE_OM_LocalGetAclByIndex(acl_index);

    if (NULL == acl_p)
    {
        return RULE_TYPE_ACL_NONEXISTED;
    }

    /* Merger ACE in aggregate ACL until can not found any ACE can be mergerd
     */
    if (2 <= acl_p->ace_count)
    {
        BOOL_T continue_compress;

        do
        {
            RULE_TYPE_INSTANCE_PTR_T    ace_inst_p;
            RULE_TYPE_Ace_Entry_T       *ace1_p = NULL;
            RULE_TYPE_Ace_Entry_T       *ace2_p = NULL;

            UI32_T                      ace1_index = 0;
            UI32_T                      ace2_index = 0;

            continue_compress = FALSE;

            for (ace_inst_p = RULE_OM_PTR2INST(acl_p->super.links.last_node);
                 NULL != ace_inst_p;
                 ace_inst_p = RULE_OM_PTR2INST(ace_inst_p->links.prev))
            {
                if (NULL == ace2_p)
                {
                    ace2_index = RULE_OM_INDEX_OF(ace_inst_p) + 1;
                    ace2_p = RULE_OM_LocalGetAceByIndex( ace2_index );
                }
                else if (NULL == ace1_p)
                {
                    ace1_index = RULE_OM_INDEX_OF(ace_inst_p) + 1;
                    ace1_p = RULE_OM_LocalGetAceByIndex( ace1_index );
                }

                if (NULL != ace1_p && NULL != ace2_p)
                {
                    if (RULE_TYPE_OK == RULE_OM_LocalCompressAce(ace2_p, ace1_p))
                    {
                        RULE_OM_LocalDelAceFromAcl(acl_p, ace2_index);

                        ace2_index = 0;
                        ace1_index = 0;

                        ace1_p = NULL;
                        ace2_p = NULL;
                        continue_compress = TRUE;
                    }
                    else
                    {
                        ace2_index = ace1_index;
                        ace1_index = 0;

                        ace2_p = ace1_p;
                        ace1_p = NULL;
                    }
                }
            }
        }while (TRUE == continue_compress);
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RebuildAggregateAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Rebuild aggregate ACL
 * INPUT  :  acl_index  - ACL index
 * OUTPUT :  none
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  none
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_RebuildAggregateAcl(
    UI32_T acl_index)
{
    RULE_TYPE_Acl_T *acl_p;
    RULE_TYPE_Acl_T *agg_acl_p;

    UI32_T  res;

    RULE_OM_EnterCriticalSection();

    acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_ACL_NONEXISTED);
    }

    acl_p->flag_aggregate_done = FALSE;
    agg_acl_p = RULE_OM_LocalGetAclByIndex( acl_p->aggregate_acl_index );

    if (NULL == agg_acl_p)
    {
        res = RULE_OM_LocalCreateWorkingAcl(acl_p->acl_type, &acl_p->aggregate_acl_index);
        if (RULE_TYPE_OK != res)
        {
            RULE_OM_LeaveCriticalSectionReturnState(res);
        }

        agg_acl_p = RULE_OM_LocalGetAclByIndex( acl_p->aggregate_acl_index );
        agg_acl_p->owner.class_id = acl_index;
        agg_acl_p->owner.class_type = RULE_TYPE_CLASS_ACL;
    }
    else
    {
        RULE_OM_LocalDelAllAceFromAcl(agg_acl_p);
    }

    /* Merger ALL ACE into Aggregate ACL
     */
    {
        RULE_TYPE_Ace_Entry_T   aggregate_ace;
        RULE_TYPE_Ace_Entry_T   *aggregate_ace_p = NULL;
        RULE_TYPE_Ace_Entry_T   *ace_p = NULL;

        UI32_T ace_index;

        for (ace_index = 0;
             RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); )
        {
            if (NULL == aggregate_ace_p)
            {
                aggregate_ace_p = &aggregate_ace;
                memcpy(aggregate_ace_p, ace_p, sizeof(*aggregate_ace_p));
            }
            else
            {
                if (RULE_TYPE_OK != RULE_OM_LocalCompressAce(ace_p, aggregate_ace_p))
                {
                    res = RULE_OM_LocalAddAce2Acl(agg_acl_p,
                                                  acl_p->acl_type,
                                                  aggregate_ace_p);
                    if (RULE_TYPE_OK != res)
                    {
                        LOG("RULE_OM_AddAce2Acl failed. "
                            "No enough ACE for aggregating operation.");
                        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
                    }

                    memcpy(aggregate_ace_p, ace_p, sizeof(*aggregate_ace_p));
                }
            }
        }

        if (NULL != aggregate_ace_p)
        {
            res = RULE_OM_LocalAddAce2Acl(agg_acl_p,
                                          acl_p->acl_type,
                                          aggregate_ace_p);
            if (RULE_TYPE_OK != res)
            {
                LOG("RULE_OM_AddAce2Acl failed. "
                    "No enough ACE for aggregating operation.");
                RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
            }
        }
    }

    RULE_OM_LocalCompressAcl(acl_p->aggregate_acl_index);
    acl_p->flag_aggregate_done = TRUE;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

// FIXME: REMOVE THIS FUNCTION !!
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetUserAclIndexOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Set user config ACL index on port
 * INPUT  :  ifIndex    - interface index
 *           dirction   - traffic direction
 *           acl_index  - ACL index
 *           acl_type   - ACL type
 * OUTPUT :  none
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  This funciton used to set the user configured ACL index on port,
 *           then you can easily get the user ACL index(Not eggregate ACL index)
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_SetUserAclIndexOnPort(
    UI32_T ifIndex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_index,
    RULE_TYPE_AclType_T acl_type)
{
    RULE_TYPE_PortEntry_T *port_entry_p;
    int precedence;

    RULE_OM_EnterCriticalSection();

    LOG("Input ifindex: %lu, direction: %s, acl_index: %lu, acl_type: %d",
        ifIndex,
        (direction == RULE_TYPE_INBOUND) ? "In" : "Out",
        acl_index,
        acl_type);

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifIndex, direction);

    if (NULL == port_entry_p)
    {
        LOG("Fail get port entry by ifindex");
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    switch (acl_type)
    {
        case RULE_TYPE_MAC_ACL:

            precedence = INGRESS_MAC_ACL_PRECEDENCE;
            break;
        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            precedence = INGRESS_IP_ACL_PRECEDENCE;
            break;
        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            precedence = INGRESS_IPV6_ACL_PRECEDENCE;
            break;
        default :
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
            break;
    }

    port_entry_p->user_config_acl_index[precedence] = acl_index;

    LOG("user config acl index in precedence[%lu] = %lu", precedence, acl_index);
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

// FIXME: REMOVE THIS FUNCTION !!
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RemoveUserAclIndexOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Set user config ACL index on port
 * INPUT  :  ifIndex    - interface index
 *           dirction   - traffic direction
 *           acl_index  - ACL index
 *           acl_type   - ACL type
 * OUTPUT :  none
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  This funciton used to remove the user configured ACL index on port.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_RemoveUserAclIndexOnPort(
    UI32_T ifIndex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_index,
    RULE_TYPE_AclType_T acl_type)
{
    RULE_TYPE_PortEntry_T *port_entry_p;
    int precedence;

    RULE_OM_EnterCriticalSection();

    LOG("Input ifindex: %lu, direction: %s, acl_index: %lu, acl_type: %d",
        ifIndex,
        (direction == RULE_TYPE_INBOUND) ? "In" : "Out",
        acl_index,
        acl_type);

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifIndex, direction);

    if (NULL == port_entry_p || 0 == port_entry_p->policy_map_index)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    if (TRUE != RULE_OM_IS_ENTRY_ACTIVE(port_entry_p->policy_map_row_status))
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    switch (acl_type)
    {
        case RULE_TYPE_MAC_ACL:

            precedence = INGRESS_MAC_ACL_PRECEDENCE;
            break;
        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            precedence = INGRESS_IP_ACL_PRECEDENCE;
            break;
        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            precedence = INGRESS_IPV6_ACL_PRECEDENCE;
            break;
        default :
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    if (acl_index == port_entry_p->user_config_acl_index[precedence])
    {
        port_entry_p->user_config_acl_index[precedence] = 0;
    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}
#endif /* #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) */

#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalMergerMfAce
 *------------------------------------------------------------------------------
 * PURPOSE:  Merger MF ACE into aggregate ACE
 * INPUT  :  src_ace_p - ACE
 *           agg_ace_p - ACE
 * OUTPUT :  agg_ace_p - ACE
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalMergerMfAce(
    RULE_TYPE_Ace_Entry_T   *src_ace_p,
    RULE_TYPE_Ace_Entry_T   *agg_ace_p)
{
    switch (src_ace_p->ace_type)
    {
        case RULE_TYPE_MAC_ACL:
            break;

        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            if (agg_ace_p->ace_type == RULE_TYPE_IPV6_ACL ||
                agg_ace_p->ace_type == RULE_TYPE_IPV6_STD_ACL ||
                agg_ace_p->ace_type == RULE_TYPE_IPV6_EXT_ACL)
            {
                return RULE_TYPE_FAIL;
            }

            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            if (agg_ace_p->ace_type == RULE_TYPE_IP_ACL ||
                agg_ace_p->ace_type == RULE_TYPE_IP_STD_ACL ||
                agg_ace_p->ace_type == RULE_TYPE_IP_EXT_ACL)
            {
                return RULE_TYPE_FAIL;
            }

            break;
    }

    agg_ace_p->access = src_ace_p->access;

#define _IS_NULL_MAC(mac) \
    (mac[0]==0 && mac[1]==0 && mac[2]==0 && mac[3]==0 && mac[4]==0 && mac[5]==0)

    switch (src_ace_p->ace_type)
    {
        case RULE_TYPE_MAC_ACL:
            if (!_IS_NULL_MAC(src_ace_p->u.mac.aceSourceMacAddrBitmask))
            {
                memcpy(agg_ace_p->u.mac.aceSourceMacAddr,
                       src_ace_p->u.mac.aceSourceMacAddr,
                       sizeof(agg_ace_p->u.mac.aceSourceMacAddr));
                memcpy(agg_ace_p->u.mac.aceSourceMacAddrBitmask,
                       src_ace_p->u.mac.aceSourceMacAddrBitmask,
                       sizeof(agg_ace_p->u.mac.aceSourceMacAddrBitmask));
            }

            if (!_IS_NULL_MAC(src_ace_p->u.mac.aceDestMacAddrBitmask))
            {
                memcpy(agg_ace_p->u.mac.aceDestMacAddr,
                       src_ace_p->u.mac.aceDestMacAddr,
                       sizeof(agg_ace_p->u.mac.aceDestMacAddr));
                memcpy(agg_ace_p->u.mac.aceDestMacAddrBitmask,
                       src_ace_p->u.mac.aceDestMacAddrBitmask,
                       sizeof(agg_ace_p->u.mac.aceDestMacAddrBitmask));
            }

            if (VAL_diffServMacAceVidOp_equal == src_ace_p->u.mac.aceVidOp)
            {
                agg_ace_p->u.mac.aceVidOp  = src_ace_p->u.mac.aceVidOp;
                agg_ace_p->u.mac.aceMinVid = src_ace_p->u.mac.aceMinVid;
                agg_ace_p->u.mac.aceMaxVid = src_ace_p->u.mac.aceMaxVid;
                agg_ace_p->u.mac.aceVidBitmask = src_ace_p->u.mac.aceVidBitmask;
            }

            if (VAL_diffServMacAceCosOp_equal == src_ace_p->u.mac.aceCosOp)
            {
                agg_ace_p->u.mac.aceCosOp   = src_ace_p->u.mac.aceCosOp;
                agg_ace_p->u.mac.aceMinCos  = src_ace_p->u.mac.aceMinCos;
                agg_ace_p->u.mac.aceMaxCos  = src_ace_p->u.mac.aceMaxCos;
                agg_ace_p->u.mac.aceCosBitmask = src_ace_p->u.mac.aceCosBitmask;
            }

            if (VAL_diffServMacAceSourcePortOp_equal == src_ace_p->u.mac.aceSourcePortOp)
            {
                agg_ace_p->u.mac.aceSourcePortOp   = src_ace_p->u.mac.aceSourcePortOp;
                agg_ace_p->u.mac.aceMinSourcePort  = src_ace_p->u.mac.aceMinSourcePort;
                agg_ace_p->u.mac.aceMaxSourcePort  = src_ace_p->u.mac.aceMaxSourcePort;
                agg_ace_p->u.mac.aceSourcePortBitmask = src_ace_p->u.mac.aceSourcePortBitmask;
            }

            break;

        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            if (RULE_TYPE_UNDEF_IP_PRECEDENCE != src_ace_p->u.ip.acePrec)
            {
                agg_ace_p->u.ip.acePrec = src_ace_p->u.ip.acePrec;
                agg_ace_p->ace_type = RULE_TYPE_IP_EXT_ACL;
            }

            if (RULE_TYPE_UNDEF_IP_TOS != src_ace_p->u.ip.aceTos)
            {
                agg_ace_p->u.ip.aceTos = src_ace_p->u.ip.aceTos;
                agg_ace_p->ace_type = RULE_TYPE_IP_EXT_ACL;
            }

            if (RULE_TYPE_UNDEF_IP_DSCP != src_ace_p->u.ip.aceDscp)
            {
                agg_ace_p->u.ip.aceDscp = src_ace_p->u.ip.aceDscp;
                agg_ace_p->ace_type = RULE_TYPE_IP_EXT_ACL;
            }
            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            if (RULE_TYPE_UNDEF_IP_DSCP != src_ace_p->u.ipv6.aceDscp)
            {
                agg_ace_p->u.ipv6.aceDscp = src_ace_p->u.ipv6.aceDscp;
                agg_ace_p->ace_type = RULE_TYPE_IPV6_EXT_ACL;
            }

            break;
    }

    return RULE_TYPE_OK;

#undef _IS_NULL_MAC
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalGetAclFromClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get ACL from a class-map and it have one permit ACE at least
 * INPUT  :  class_map_p - class-map
 * OUTPUT :  acl_p       - ACL
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalGetAclFromClassMap(
    RULE_TYPE_ClassMap_T *class_map_p,
    RULE_TYPE_Acl_T **acl_p)
{
    UI32_T i;

    *acl_p = NULL;
    for (i=0; i<class_map_p->class_element_count; i++)
    {
        if (RULE_TYPE_CLASS_ACL == class_map_p->class_type[i])
        {
            *acl_p = RULE_OM_LocalGetAclByIndex( class_map_p->class_index_list[i] );
            return RULE_TYPE_OK;
        }
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalBuildMatchAllClassMapRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Build aggregation ACL for match-all class-map
 * INPUT  :  class_map_index    - index of class-map
 *           acl_index_p        - index of aggregation ACL
 * OUTPUT :  none
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalBuildMatchAllClassMapRule(
    RULE_TYPE_ClassMap_T *class_map_p,
    UI32_T  agg_acl_index)
{
    UI32_T  agg_ace_index;
    UI32_T  res;

    RULE_TYPE_Acl_T *agg_acl_p = RULE_OM_LocalGetAclByIndex(agg_acl_index);
    RULE_TYPE_Acl_T *cls_acl_p = NULL;
    RULE_TYPE_Ace_Entry_T *agg_ace_p;

    RULE_OM_LocalGetAclFromClassMap(class_map_p, &cls_acl_p);

    /* Copy ACE from class-map ACL to aggregation ACL
     */
    if (NULL != cls_acl_p)
    {
        UI32_T cls_acl_index;

        RULE_OM_LocalGetAclIdByName(cls_acl_p->acl_name, &cls_acl_index);
        res = RULE_OM_LocalCopyAcl(cls_acl_index, agg_acl_index);
        if (RULE_TYPE_OK != res)
        {
            return res;
        }
    }
    else
    {
        UI32_T j;

        for (j=0; j<class_map_p->class_element_count; ++j)
        {
            RULE_TYPE_Ace_Entry_T *mf_ace_p = NULL;

            if (RULE_TYPE_CLASS_MF != class_map_p->class_type[j])
            {
                continue;
            }

            mf_ace_p = RULE_OM_LocalGetAceByIndex( class_map_p->class_index_list[j] );

            /* Because the woking ACL is just a container to save all aggregated
             * ACE. So we change it's type is need.
             */
            agg_acl_p->acl_type = mf_ace_p->ace_type;

            res = RULE_OM_LocalAddAce2Acl(agg_acl_p, mf_ace_p->ace_type, mf_ace_p);
            if (RULE_TYPE_OK != res)
            {
                return res;
            }
            break;
        }
    }

    /* Merger field of MF into aggregation ACL
     */
    for (agg_ace_index = 0;
         RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(agg_acl_p, &agg_ace_index, &agg_ace_p); )
    {
        UI32_T j;

        for (j=0; j<class_map_p->class_element_count; ++j)
        {
            RULE_TYPE_Ace_Entry_T *mf_ace_p = NULL;

            if (RULE_TYPE_CLASS_MF != class_map_p->class_type[j])
            {
                continue;
            }

            mf_ace_p = RULE_OM_LocalGetAceByIndex( class_map_p->class_index_list[j] );
            res = RULE_OM_LocalMergerMfAce(mf_ace_p, agg_ace_p);

            if (RULE_TYPE_OK != res)
            {
                return res;
            }
        }
    }

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    RULE_OM_LocalCompressAcl( agg_acl_index );
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_BuildMatchAllClassMapRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Build aggregation ACL for match-all class-map
 * INPUT  :  class_map_index    - index of class-map
 * OUTPUT :  acl_index_p        - index of aggregation ACL
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_BuildMatchAllClassMapRule(
    UI32_T class_map_index,
    UI32_T *acl_index_p)
{
    UI32_T  res;
    UI32_T  agg_acl_index = 0;
    RULE_TYPE_ClassMap_T *class_map_p;
    RULE_TYPE_Acl_T *agg_acl_p = NULL;
    RULE_TYPE_Acl_T *cls_acl_p = NULL;

    if (NULL == acl_index_p)
    {
        return RULE_TYPE_FAIL;
    }

    RULE_OM_EnterCriticalSection();

    *acl_index_p = 0;

    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_CLASS_NONEXISTED);
    }

    if (TRUE == class_map_p->class_ext.flag_aggregate_index_valid)
    {
        *acl_index_p = class_map_p->class_ext.aggregate_index;
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }

    RULE_OM_LocalGetAclFromClassMap(class_map_p, &cls_acl_p);

    res = RULE_OM_LocalCreateWorkingAcl(RULE_TYPE_MAC_ACL, &agg_acl_index);
    if (RULE_TYPE_OK != res)
    {
        RULE_OM_LeaveCriticalSectionReturnState(res);
    }

    agg_acl_p = RULE_OM_LocalGetAclByIndex( agg_acl_index );
    agg_acl_p->owner.class_id   = class_map_index;
    agg_acl_p->owner.class_type = RULE_TYPE_CLASS_CLASS_MAP;

    agg_acl_p->class_map_count = 1;
    RULE_OM_SET_BIT_ON(agg_acl_p->class_map_bit_list, (class_map_index-1));

    res = RULE_OM_LocalBuildMatchAllClassMapRule(class_map_p, agg_acl_index);
    if (RULE_TYPE_OK != res)
    {
        RULE_OM_LocalDelAcl( agg_acl_index );
        RULE_OM_LeaveCriticalSectionReturnState(res);
    }

    class_map_p->class_ext.aggregate_index = agg_acl_index;
    class_map_p->class_ext.flag_aggregate_index_valid = TRUE;

    *acl_index_p = agg_acl_index;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RebuildMatchAllClassMapRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Rebuild aggregation ACL for match-all class-map
 * INPUT  :  class_map_index    - index of class-map
 * OUTPUT :  acl_index_p        - index of aggregation ACL
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  none
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_RebuildMatchAllClassMapRule(
    UI32_T class_map_index,
    UI32_T *acl_index_p)
{
    RULE_TYPE_Acl_T *agg_acl_p;
    RULE_TYPE_ClassMap_T *class_map_p;

    UI32_T  res;
    UI32_T  agg_acl_index = 0;

    if (NULL == acl_index_p)
    {
        return RULE_TYPE_FAIL;
    }

    RULE_OM_EnterCriticalSection();

    *acl_index_p = 0;

    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_CLASS_NONEXISTED);
    }

    if (TRUE == class_map_p->class_ext.flag_aggregate_index_valid)
    {
        agg_acl_p = RULE_OM_LocalGetAclByIndex(class_map_p->class_ext.aggregate_index);

        ASSERT(NULL != agg_acl_p);

        RULE_OM_LocalDelAllAceFromAcl(agg_acl_p);
    }
    else
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
    }

    agg_acl_index = class_map_p->class_ext.aggregate_index;

    agg_acl_p = RULE_OM_LocalGetAclByIndex(agg_acl_index);

    ASSERT(NULL != agg_acl_p);

    res = RULE_OM_LocalBuildMatchAllClassMapRule(class_map_p, agg_acl_index);
    if (RULE_TYPE_OK != res)
    {
        RULE_OM_LocalDelAllAceFromAcl(agg_acl_p );
        RULE_OM_LeaveCriticalSectionReturnState(res);
    }

    *acl_index_p = agg_acl_index;
    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DelMatchAllClassMapRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy aggregation ACL of the match-all class-map
 * INPUT  :  class_map_index    - index of class-map
 * OUTPUT :  none
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  The aggregation ACL will be destroyed when it have no any
 *           referenced instances.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DelMatchAllClassMapRule(
    UI32_T class_map_index)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    UI32_T  agg_acl_index;

    RULE_OM_EnterCriticalSection();

    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_CLASS_NONEXISTED);
    }

    if (FALSE == class_map_p->class_ext.flag_aggregate_index_valid)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
    }

    agg_acl_index = class_map_p->class_ext.aggregate_index;

    RULE_OM_LocalDelAcl(agg_acl_index);

    class_map_p->class_ext.flag_aggregate_index_valid = FALSE;
    class_map_p->class_ext.aggregate_index = 0;

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}
#endif /* #if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalValidateMatchAllClassMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  Validating the element of class-map when insert it into a class-map
 * INPUT:    class_map_index    - index of class-map
 *           class_map_p        - object of class-map
 *           element_entry      - object of class-map element
 * OUTPUT:   None
 * RETURN:   Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_LocalValidateMatchAllClassMapElement(
    UI32_T class_map_index,
    const RULE_TYPE_ClassMap_T *class_map_p,
    RULE_TYPE_UI_ClassMapElement_T *element_entry)
{
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)

    UI32_T result;
    UI32_T i;

    ASSERT(NULL != class_map_p);
    ASSERT(NULL != element_entry);

    if (RULE_TYPE_CLASS_MAP_MATCH_ALL != class_map_p->class_map_match_type)
    {
        return RULE_TYPE_OK;
    }

    /* Only can bind one ACL or one type of MF in a match-all class-map
     */
    switch (element_entry->class_type)
    {
        case RULE_TYPE_CLASS_ACL:
            for (i=0; i<class_map_p->class_element_count; ++i)
            {
                if (RULE_TYPE_CLASS_ACL == class_map_p->class_type[i])
                {
                    LOG("One accept most one ACL in a match-all class-map");
                    return RULE_TYPE_FAIL;
                }
            }
            break;

        case RULE_TYPE_CLASS_MF:
            for (i=0; i<class_map_p->class_element_count; ++i)
            {
                RULE_TYPE_UI_ClassMapElement_T om_class_element;

                result = RULE_OM_LocalConvert2UIClassMapElement(class_map_p, i+1, &om_class_element);

                if (RULE_TYPE_OK != result)
                {
                    LOG("Convert ACE to class-map-element failed");
                    return result;
                }

                if (element_entry->element.mf_entry.mf_type == om_class_element.element.mf_entry.mf_type)
                {
                    LOG("Can not add MF with the same type in a match-all class-map");
                    return RULE_TYPE_FAIL;
                }
            }
            break;

        default:
            ASSERT(0);
            return RULE_TYPE_FAIL;
    }

#endif /* SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL */

    return RULE_TYPE_OK;
}

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalValidateUIBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Validates UI bundle group
 * INPUT:    policy_map_index   - policy-map index
 *           ui_bundle_group_p  - UI bundle group
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalValidateUIBundleGroup(
    UI32_T policy_map_index,
    const RULE_TYPE_UI_BundleGroup_T *ui_bundle_group_p)
{
    UI32_T ret;
    UI32_T i;

    ASSERT(NULL != ui_bundle_group_p);

    if (0 == ui_bundle_group_p->index ||
        SYS_ADPT_DIFFSERV_MAX_BUNDLE_GROUP_OF_POLICY_MAP < ui_bundle_group_p->index)
    {
        LOG("Invalid bundle group index %lu", ui_bundle_group_p->index);
        return RULE_TYPE_INVALID_PARAMETER;
    }

    /* For delete case
     */
    if (FALSE == ui_bundle_group_p->valid_status)
    {
        return RULE_TYPE_OK;
    }

    if (ui_bundle_group_p->class_map_count == 0 ||
        _countof(ui_bundle_group_p->class_map_name_list) < ui_bundle_group_p->class_map_count)
    {
        LOG("Invalid class-map count %lu", ui_bundle_group_p->class_map_count);
        return RULE_TYPE_INVALID_PARAMETER;
    }

    for (i = 0; i < ui_bundle_group_p->class_map_count; ++i)
    {
        const char *class_map_name = ui_bundle_group_p->class_map_name_list[i];
        UI32_T class_map_index;

        ret = RULE_OM_LocalGetClassMapIndexByName(class_map_name, &class_map_index);
        if (RULE_TYPE_OK != ret)
        {
            return ret;
        }

        /* Scan all bundle-group to find the duplicated class-map
         */
        {
            RULE_TYPE_PolicyMap_T   *policy_map_p;
            RULE_TYPE_BundleGroup_T *test_bundle_group_p;
            UI32_T                  test_class_map_index;
            UI32_T                  bi;
            UI32_T                  ci;

            policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
            if (NULL == policy_map_p)
            {
                LOG("Invalid policy-map index %lu", policy_map_index);
                return RULE_TYPE_INVALID_PARAMETER;
            }

            for (bi = 0; bi < _countof(policy_map_p->bundle_group); ++bi)
            {
                test_bundle_group_p = &policy_map_p->bundle_group[bi];

                if (FALSE == test_bundle_group_p->valid_status)
                {
                    continue;
                }

                if (bi+1 == ui_bundle_group_p->index)
                {
                    continue;
                }

                for (ci = 0; ci < test_bundle_group_p->class_map_count; ++ci)
                {
                    test_class_map_index = test_bundle_group_p->class_map_index_list[ci];

                    if (test_class_map_index == class_map_index)
                    {
                        LOG("Class-map %s exist on bundle-group %lu already", class_map_name, bi+1);
                        return RULE_TYPE_INVALID_PARAMETER;
                    }
                }
            }
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalGetClassBandwidth
 *------------------------------------------------------------------------------
 * PURPOSE:  Get class bandwidth
 * INPUT:    policy_map_index   - policy-map index
 *           class_map_index    - class-map index
 * OUTPUT:   cir_kbps           - class's bandwidth
 *           burst_size_p       - class's burst size
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalGetClassBandwidth(
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T *cir_kbps_p,
    UI32_T *burst_size_p)
{
    RULE_TYPE_PolicyElement_T   *policy_element_p;
    RULE_TYPE_TBParamEntry_T    *meter_entry_p;
    RULE_TYPE_Action_T          *action_p;

    ASSERT(NULL != cir_kbps_p);

    policy_element_p = RULE_OM_LocalGetPolicyMapElement(policy_map_index, class_map_index);
    if (NULL == policy_element_p)
    {
        LOG("Invalid policy-map index(%lu)", policy_map_index);
        return RULE_TYPE_INVALID_PARAMETER;
    }

    meter_entry_p = RULE_OM_LocalGetMeterEntryByIndex(policy_element_p->meter_index);
    if (NULL == meter_entry_p)
    {
        LOG("Invalid meter index(%lu)", policy_element_p->meter_index);
        return RULE_TYPE_INVALID_PARAMETER;
    }

    action_p = RULE_OM_LocalGetActionEntryByIndex(policy_element_p->action_index);
    if (NULL == action_p)
    {
        LOG("Invalid action index(%lu)", policy_element_p->action_index);
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_METER_MODE_FLOW != meter_entry_p->meter_model)
    {
        LOG("Invalid meter mode(%lu)", meter_entry_p->meter_model);
        return RULE_TYPE_E_INVALID_METER_MODE;
    }

    if (0 == (RULE_TYPE_ACTION_RED_DROP & action_p->action_bitmap))
    {
        LOG("Invalid action(%lu)", action_p->action_bitmap);
        return RULE_TYPE_E_INVALID_ACTION;
    }

    *cir_kbps_p     = meter_entry_p->rate;
    *burst_size_p   = meter_entry_p->burst_size;
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalGetClassPriority
 *------------------------------------------------------------------------------
 * PURPOSE:  Get class priority
 * INPUT:    policy_map_index   - policy-map index
 *           class_map_index    - class-map index
 * OUTPUT:   priority           - class's priority
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalGetClassPriority(
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T *priority_p)
{
    RULE_TYPE_PolicyElement_T   *policy_element_p;
    RULE_TYPE_Action_T          *action_p;

    ASSERT(NULL != priority_p);

    policy_element_p = RULE_OM_LocalGetPolicyMapElement(policy_map_index, class_map_index);
    if (NULL == policy_element_p)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    action_p = RULE_OM_LocalGetActionEntryByIndex(policy_element_p->action_index);
    if (NULL == action_p)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    *priority_p = action_p->pkt_pri;
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalInsertNewClass
 *------------------------------------------------------------------------------
 * PURPOSE:  Insert new class into bundle group
 * INPUT:    bundle_group_p     - bundle group object
 *           class_map_index    - class-map index
 *           priority           - class's priority
 *           bandwidth          - bandwidth (kbps)
 *           burst_size         - burst_size (bytes)
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalInsertNewClass(
    RULE_TYPE_BundleGroup_T *bundle_group_p,
    UI32_T class_map_index,
    UI32_T priority,
    UI32_T bandwidth,
    UI32_T burst_size)
{
    UI32_T i;
    RULE_TYPE_ChildBundleGroup_T *child_bundle = NULL;
    RULE_TYPE_Class_T            *class_p = NULL;

    ASSERT(NULL != bundle_group_p);

    for (i = 0; i < bundle_group_p->child_bundle_count; ++i)
    {
        if (bundle_group_p->child_bundle_list[i].priority == priority)
        {
            child_bundle = &bundle_group_p->child_bundle_list[i];
            break;
        }
    }

    if (NULL == child_bundle)
    {
        if (bundle_group_p->child_bundle_count == _countof(bundle_group_p->child_bundle_list))
        {
            LOG("Not available entry for new child bundle for priority %lu", priority);
            return RULE_TYPE_FAIL;
        }

        child_bundle = &bundle_group_p->child_bundle_list[ bundle_group_p->child_bundle_count ++ ];
        memset(child_bundle, 0, sizeof(*child_bundle));
    }

    ASSERT(NULL != child_bundle);

    for (i = 0; i < child_bundle->class_count; ++i)
    {
        if (child_bundle->class_list[i].class_map_index == class_map_index)
        {
            return RULE_TYPE_OK;
        }
    }

    if (NULL == class_p)
    {
        if (child_bundle->class_count == _countof(child_bundle->class_list))
        {
            LOG("Not available entry for new class-map %lu", class_map_index);
            return RULE_TYPE_FAIL;
        }

        class_p = &child_bundle->class_list[child_bundle->class_count ++];
        memset(class_p, 0, sizeof(*class_p));
    }

    bundle_group_p->class_map_index_list[ bundle_group_p->class_map_count++ ] = class_map_index;

    child_bundle->priority          = priority;

    class_p->class_map_index        = class_map_index;
    class_p->priority               = priority;
    class_p->bandwidth_kbps         = bandwidth;
    class_p->burst_size_byte        = burst_size;
    class_p->flow.bandwidth_kbps    = bandwidth;

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalCompareChildBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Callback function for comparing two child-bundle-group object
 * INPUT:    a                  - child bundle group object
 *           b                  - child bundle group object
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static int
RULE_OM_LocalCompareChildBundleGroup(const void *a, const void *b)
{
    const RULE_TYPE_ChildBundleGroup_T *child_a = (const RULE_TYPE_ChildBundleGroup_T *)a;
    const RULE_TYPE_ChildBundleGroup_T *child_b = (const RULE_TYPE_ChildBundleGroup_T *)b;

    return child_b->priority - child_a->priority;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalConvertFromUIBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Convert bundle group from UI
 * INPUT:    policy_map_index   - policy-map index
 *           ui_bundle_group_p  - UI bundle group object
 *           bundle_group_p     - bundle group object
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalConvertFromUIBundleGroup(
    UI32_T policy_map_index,
    const RULE_TYPE_UI_BundleGroup_T *ui_bundle_group_p,
    RULE_TYPE_BundleGroup_T *bundle_group_p)
{
    UI32_T ret;
    UI32_T i;

    ASSERT(NULL != ui_bundle_group_p);
    ASSERT(NULL != bundle_group_p);

    memset(bundle_group_p, 0, sizeof(*bundle_group_p));

    bundle_group_p->valid_status = ui_bundle_group_p->valid_status;

    if (FALSE == ui_bundle_group_p->valid_status)
    {
        return RULE_TYPE_OK;
    }

    bundle_group_p->bundle_rate_kbps = ui_bundle_group_p->bundle_rate_kbps;

    for (i = 0; i < ui_bundle_group_p->class_map_count; ++i)
    {
        const char *class_map_name = ui_bundle_group_p->class_map_name_list[i];
        UI32_T class_map_index;
        UI32_T cir;
        UI32_T burst_size;
        UI32_T priority;

        ret = RULE_OM_LocalGetClassMapIndexByName(class_map_name, &class_map_index);
        if (RULE_TYPE_OK != ret)
        {
            LOG("Class-map index %lu is not exist", class_map_index);
            return ret;
        }

        ret = RULE_OM_LocalGetClassBandwidth(policy_map_index, class_map_index, &cir, &burst_size);
        if (RULE_TYPE_OK != ret)
        {
            LOG("Failed to get the bandwidth of policy-map index %lu, class-map index %lu",
                policy_map_index, class_map_index);
            return ret;
        }

        ret = RULE_OM_LocalGetClassPriority(policy_map_index, class_map_index, &priority);
        if (RULE_TYPE_OK != ret)
        {
            LOG("Failed to get the class priority of policy-map index %lu, class-map index %lu",
                policy_map_index, class_map_index);
            return ret;
        }

        ret = RULE_OM_LocalInsertNewClass(bundle_group_p, class_map_index, priority, cir, burst_size);
        if (RULE_TYPE_OK != ret)
        {
            LOG("Failed to insert new class into bundle");
            return ret;
        }
    }

    ASSERT(bundle_group_p->child_bundle_count <= bundle_group_p->class_map_count);

    qsort(bundle_group_p->child_bundle_list,
          bundle_group_p->child_bundle_count,
          sizeof(bundle_group_p->child_bundle_list[0]),
          RULE_OM_LocalCompareChildBundleGroup);

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalConvert2UIBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Convert bundle group for UI
 * INPUT:    bundle_group_p     - bundle group object
 *           index              - bundle group index
 *           ui_bundle_group_p  - UI bundle group object
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalConvert2UIBundleGroup(
    const RULE_TYPE_BundleGroup_T *bundle_group_p,
    UI32_T index,
    RULE_TYPE_UI_BundleGroup_T *ui_bundle_group_p)
{
    UI32_T i;

    memset(ui_bundle_group_p, 0, sizeof(*ui_bundle_group_p));

    ui_bundle_group_p->valid_status = bundle_group_p->valid_status;

    if (FALSE == bundle_group_p->valid_status)
    {
        return RULE_TYPE_OK;
    }

    ui_bundle_group_p->bundle_rate_kbps = bundle_group_p->bundle_rate_kbps;

    ASSERT(ui_bundle_group_p->class_map_count < _countof(ui_bundle_group_p->class_map_name_list));

    ui_bundle_group_p->index = index;
    ui_bundle_group_p->class_map_count = bundle_group_p->class_map_count;

    for (i = 0; i < ui_bundle_group_p->class_map_count; ++i)
    {
        UI32_T class_map_index = bundle_group_p->class_map_index_list[i];
        RULE_TYPE_ClassMap_T *class_map;

        class_map = RULE_OM_LocalGetClassMapByIndex(class_map_index);
        if (NULL == class_map)
        {
            LOG("Class-map index %lu is not exist", class_map_index);
            return RULE_TYPE_FAIL;
        }

        strncpy(ui_bundle_group_p->class_map_name_list[i], class_map->class_map_name, sizeof(ui_bundle_group_p->class_map_name_list[i])-1);
        ui_bundle_group_p->class_map_name_list[i][ sizeof(ui_bundle_group_p->class_map_name_list[i])-1 ] = '\0';
    }

    return RULE_TYPE_OK;
}

static UI32_T
RULE_OM_LocalIsClassMapInBundleGroup(
    UI32_T policy_map_index,
    UI32_T class_map_index)
{
    const RULE_TYPE_PolicyMap_T *policy_map_p;
    UI32_T                      bundle_count;
    UI32_T                      class_count;

    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
    {
        LOG("Invalid policy-map index %lu", policy_map_index);
        return RULE_TYPE_FAIL;
    }

    for (bundle_count = 0; bundle_count < _countof(policy_map_p->bundle_group); ++bundle_count)
    {
        const RULE_TYPE_BundleGroup_T *bundle_group_p = &policy_map_p->bundle_group[bundle_count];

        if (TRUE != bundle_group_p->valid_status)
        {
            continue;
        }

        for (class_count = 0; class_count < bundle_group_p->class_map_count; ++class_count)
        {
            if (class_map_index == bundle_group_p->class_map_index_list[class_count])
            {
                return RULE_TYPE_OK;
            }
        }
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalRebuildBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Rebuild all bundle group in specified policy-map
 * INPUT:    policy_map_p       - policy-map
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalRebuildBundleGroup(
    RULE_TYPE_PolicyMap_T *policy_map_p)
{
    UI32_T                      i;

    for (i = 0; i < _countof(policy_map_p->bundle_group); ++i)
    {
        RULE_TYPE_BundleGroup_T     *bundle_group_p;
        RULE_TYPE_UI_BundleGroup_T  ui_bundle_group;
        UI32_T                      ret;

        bundle_group_p = &policy_map_p->bundle_group[i];

        if (TRUE != bundle_group_p->valid_status)
        {
            continue;
        }

        ret = RULE_OM_LocalConvert2UIBundleGroup(bundle_group_p, i+1, &ui_bundle_group);
        if (RULE_TYPE_OK != ret)
        {
            return ret;
        }

        ret = RULE_OM_LocalConvertFromUIBundleGroup(RULE_OM_LocalPolicyMapPtrToIndex(policy_map_p),
                                                    &ui_bundle_group,
                                                    bundle_group_p);
        if (RULE_TYPE_OK != ret)
        {
            return ret;
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetUIBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Create/update a bundle group
 * INPUT:    policy_map_name    - policy-map name
 *           index              - bundle group index
 *           ui_bundle_group_p  - bundle group object
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     Create/update a bundle by
 *             valid_status = TRUE and index is which entry want to create/update.
 *           Delete a bundle by
 *             valid_status = TRUE and index is which entry want to delete.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_SetUIBundleGroup(
    UI32_T policy_map_index,
    RULE_TYPE_UI_BundleGroup_T *ui_bundle_group_p)
{
    UI32_T index;
    RULE_TYPE_PolicyMap_T *policy_map_p;
    UI32_T ret;
    RULE_TYPE_BundleGroup_T bundle_group;

    RULE_OM_EnterCriticalSection();

    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
    {
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_POLICY_MAP_NONEXISTED);
    }

    ret = RULE_OM_LocalValidateUIBundleGroup(policy_map_index, ui_bundle_group_p);
    if (RULE_TYPE_OK != ret)
    {
        RULE_OM_LeaveCriticalSectionReturnState(ret);
    }

    ret = RULE_OM_LocalConvertFromUIBundleGroup(policy_map_index,
                                                ui_bundle_group_p,
                                                &bundle_group);
    if (RULE_TYPE_OK != ret)
    {
        RULE_OM_LeaveCriticalSectionReturnState(ret);
    }

    index = ui_bundle_group_p->index - 1;   /* 0-based */

    ASSERT(index < _countof(policy_map_p->bundle_group));

    memcpy(&policy_map_p->bundle_group[index],
           &bundle_group,
           sizeof(policy_map_p->bundle_group[index]));

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RebuildBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Rebuild all bundle group in specified policy-map
 * INPUT:    policy_map_index   - policy-map index
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_RebuildBundleGroup(
    UI32_T policy_map_index)
{
    RULE_TYPE_PolicyMap_T *policy_map_p;
    UI32_T ret;

    RULE_OM_EnterCriticalSection();

    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
    {
        LOG("Invalid policy-map index %lu", policy_map_index);
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_INVALID_PARAMETER);
    }

    ret = RULE_OM_LocalRebuildBundleGroup(policy_map_p);

    RULE_OM_LeaveCriticalSectionReturnState(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_IsClassMapInBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Does the class map in bundle group by specified policy map
 * INPUT:    policy_map_index   - policy-map index
 *           class_map_index    - class-map index
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_IsClassMapInBundleGroup(
    UI32_T policy_map_index,
    UI32_T class_map_index)
{
    UI32_T ret;

    RULE_OM_EnterCriticalSection();

    ret = RULE_OM_LocalIsClassMapInBundleGroup(policy_map_index, class_map_index);

    RULE_OM_LeaveCriticalSectionReturnState(ret);
}

UI32_T
RULE_OM_IsBundleRateSet(
    UI32_T policy_map_index)
{
    RULE_TYPE_PolicyMap_T *policy_map_p;
    UI32_T ret;
    UI32_T i;

    RULE_OM_EnterCriticalSection();

    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
    {
        LOG("Invalid policy-map index %lu", policy_map_index);
        RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_INVALID_PARAMETER);
    }

    for (i = 0; i < SYS_ADPT_DIFFSERV_MAX_BUNDLE_GROUP_OF_POLICY_MAP; ++i)
    {
         if (TRUE == policy_map_p->bundle_group[i].valid_status)
        {
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
        }

    }

    RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_FAIL);
}
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

/* LOCAL SUBPROGRAM BODIES */


/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalSetPolicyMapInstance
 *------------------------------------------------------------------------------
 * PURPOSE:  Assign policy map instance
 * INPUT:    port_entry_p, policy_index, class_index, policy_inst_p
 * OUTPUT:   NONE
 * RETURN:   NONE
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
static void
RULE_OM_LocalSetPolicyMapInstance(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_inst_p)
{
    RULE_TYPE_PortEntry_T *port_entry_p;

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, direction);

    ASSERT(NULL != port_entry_p);

    if (NULL != port_entry_p)
    {
        port_entry_p->policy_map_instance_ptr = policy_inst_p;
    }
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalGetPolicyMapInstance
 *------------------------------------------------------------------------------
 * PURPOSE:  Get policy map instance
 * INPUT:    port_entry_p, policy_index, class_index
 * OUTPUT:   The pointer of policy map instance
 * RETURN:   NONE
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_CLASS_INSTANCE_PTR_T
RULE_OM_LocalGetPolicyMapInstance(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T direction)
{
    RULE_TYPE_PortEntry_T *port_entry_p;

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, direction);

    if (NULL != port_entry_p)
    {
        return port_entry_p->policy_map_instance_ptr;
    }

    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Local_GetNextPort_BindPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE  : Enumerate all interface which have bound the specified class-map.
 * INPUT    : policy_map_index      - policy-map index
 *            uport_ifindex_p       - uport ifindex; pass 0 to start.
 * OUTPUT   : uport_ifindex_p       - uport ifindex
 *            inout_profile_p       - direction
 * RETURN   : RULE_TYPE_OK if get an interface; else if error or no more interface
 * NOTES    : Sequence of the enumerating are
 *            uport_inindex -> inout_profile
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_Local_GetNextPort_BindPolicyMap(
    UI32_T  policy_map_index,
    UI32_T  *uport_ifindex_p,
    RULE_TYPE_InOutDirection_T *inout_profile_p)
{
    UI32_T                      start_ifindex;
    RULE_TYPE_InOutDirection_T  start_direction;

    RULE_TYPE_PolicyMap_T       *policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    UI32_T                      i;

    if (NULL == policy_map_p)
    {
        return RULE_TYPE_FAIL;
    }

//
// FIXME: No supoort all port here
//
    if (0 == *uport_ifindex_p)
    {
        start_ifindex   = 1;
        start_direction = RULE_TYPE_INBOUND;
    }
    else
    {
        start_ifindex   = (*uport_ifindex_p+1);
        start_direction = *inout_profile_p;
    }

    if (RULE_TYPE_INBOUND == start_direction)
    {
    //
    // FIXME: Not support all port
    //
        *inout_profile_p = RULE_TYPE_INBOUND;
        for (i=start_ifindex; i<=SYS_ADPT_TOTAL_NBR_OF_LPORT; i++)
        {
            if (RULE_OM_IS_BIT_ON(policy_map_p->qos_port_list, (i-1)))
            {
                *uport_ifindex_p = i;
                return RULE_TYPE_OK;
            }
        }

        *inout_profile_p = RULE_TYPE_OUTBOUND;
        for (i=1; i<=SYS_ADPT_TOTAL_NBR_OF_LPORT; i++)
        {
            if (RULE_OM_IS_BIT_ON(policy_map_p->egress_qos_port_list, (i-1)))
            {
                *uport_ifindex_p = i;
                return RULE_TYPE_OK;
            }
        }
    }
    else
    {
    //
    // FIXME: Not support all port
    //
        *inout_profile_p = RULE_TYPE_OUTBOUND;
        for (i=start_ifindex; i<=SYS_ADPT_TOTAL_NBR_OF_LPORT; i++)
        {
            if (RULE_OM_IS_BIT_ON(policy_map_p->egress_qos_port_list, (i-1)))
            {
                *uport_ifindex_p = i;
                return RULE_TYPE_OK;
            }
        }
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Local_GetNextPort_BindClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : Enumerate all interface which have bound the specified class-map.
 * INPUT    : class_map_index       - class-map index
 *            uport_ifindex_p       - uport ifindex; pass 0 to start.
 * OUTPUT   : uport_ifindex_p       - uport ifindex
 *            inout_profile_p       - direction
 *            policy_map_index_p    - policy-map index
 * RETURN   : RULE_TYPE_OK if get an interface; else if error or no more interface
 * NOTES    : Sequence of the enumerating are
 *            uport_inindex -> inout_profile -> policy_map_index
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_Local_GetNextPort_BindClassMap(
    UI32_T  class_map_index,
    UI32_T  *uport_ifindex_p,
    RULE_TYPE_InOutDirection_T  *inout_profile_p,
    UI32_T  *policy_map_index_p)
{
    if (0 == *uport_ifindex_p)
    {
        *policy_map_index_p = 1;
        *uport_ifindex_p = 0;
    }

    for (; *policy_map_index_p<=SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP; (*policy_map_index_p)++)
    {
        if (NULL == RULE_OM_LocalGetPolicyMapElement(*policy_map_index_p, class_map_index))
        {
            continue;
        }

        if (RULE_TYPE_OK == RULE_OM_Local_GetNextPort_BindPolicyMap(*policy_map_index_p,
                                                                    uport_ifindex_p,
                                                                    inout_profile_p))
        {
            return RULE_TYPE_OK;
        }

        *uport_ifindex_p = 0;
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalGetClassMapElementIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  search class-map element by element_entry
 * INPUT:    class_map_index, element_entry
 * OUTPUT:   element_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalGetClassMapElementIndex(UI32_T class_map_index, RULE_TYPE_UI_ClassMapElement_T *element_entry, UI32_T *element_index)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    UI32_T                  index;

    if ((NULL == element_entry) || (NULL == element_index))
        return RULE_TYPE_FAIL;

    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        return RULE_TYPE_FAIL; /* bad index */

    for (index = 0; class_map_p->class_element_count > index; ++index)
    {
        if (class_map_p->class_type[index] != element_entry->class_type)
            continue;

        switch (element_entry->class_type)
        {
            case RULE_TYPE_CLASS_ACL:
            {
                RULE_TYPE_Acl_T     *acl_p;

                acl_p = RULE_OM_LocalGetAclByIndex(class_map_p->class_index_list[index]);
                if (NULL == acl_p)
                    return RULE_TYPE_FAIL; /* should not go here */

                if (0 != strcmp((char *)element_entry->element.acl_name, (char *)acl_p->acl_name))
                    continue;
                /*match ACL exist*/
                break;
            }
            case RULE_TYPE_CLASS_MF:
            {
                RULE_TYPE_Ace_Entry_T   *ace_entry_p;

                ace_entry_p = RULE_OM_LocalGetAceByIndex(class_map_p->class_index_list[index]);
                if (NULL == ace_entry_p)
                    return RULE_TYPE_FAIL; /* should not go here */

                switch (element_entry->element.mf_entry.mf_type)
                {
                    case RULE_TYPE_MF_PRECEDENCE:
                        if ((RULE_TYPE_IP_EXT_ACL != ace_entry_p->ace_type) ||
                            (element_entry->element.mf_entry.mf_value != ace_entry_p->u.ip.acePrec))
                            continue;
                        /*match precedence exist*/
                        break;

                    case RULE_TYPE_MF_DSCP:
                        if ((RULE_TYPE_IP_EXT_ACL != ace_entry_p->ace_type) ||
                            (element_entry->element.mf_entry.mf_value != ace_entry_p->u.ip.aceDscp))
                            continue;
                        /*match DSCP exist*/
                        break;

                    case RULE_TYPE_MF_DSCP_IPV6:
                        if ((RULE_TYPE_IPV6_EXT_ACL != ace_entry_p->ace_type) ||
                            (element_entry->element.mf_entry.mf_value != ace_entry_p->u.ipv6.aceDscp))
                            continue;
                        /*match IPv6 DSCP exist*/
                        break;

                    case RULE_TYPE_MF_VLAN:
                        if ((RULE_TYPE_MAC_ACL != ace_entry_p->ace_type) ||
                            (VAL_diffServMacAceVidOp_equal != ace_entry_p->u.mac.aceVidOp) ||
                            (element_entry->element.mf_entry.mf_value != ace_entry_p->u.mac.aceMinVid))
                            continue;
                        /*match VLAN exist*/
                        break;

                    case RULE_TYPE_MF_COS:
                        if ((RULE_TYPE_MAC_ACL != ace_entry_p->ace_type) ||
                            (VAL_diffServMacAceCosOp_equal != ace_entry_p->u.mac.aceCosOp) ||
                            (element_entry->element.mf_entry.mf_value != ace_entry_p->u.mac.aceMinCos))
                            continue;
                        /*match COS exist*/
                        break;

                    case RULE_TYPE_MF_SOURCE_PORT:
                        if ((RULE_TYPE_MAC_ACL != ace_entry_p->ace_type) ||
                            (VAL_diffServMacAceSourcePortOp_equal != ace_entry_p->u.mac.aceSourcePortOp) ||
                            (element_entry->element.mf_entry.mf_value != ace_entry_p->u.mac.aceMinSourcePort))
                            continue;
                        /*match source-port exist*/
                        break;

                    default: /* should not go here */
                        continue;
                }
                break;
            }
            default: /* should not go here */
                continue;
        }

        /* exactly match element entry */
        *element_index = index + 1; /* to 1-based index */
        return RULE_TYPE_OK;
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalAllocateMeterIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  search an unused meter entry and allocate it
 * INPUT:    none
 * OUTPUT:   meter_index
 * RETURN:   TRUE -- succeeded / FALSE -- failed
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
static BOOL_T RULE_OM_LocalAllocateMeterIndex(UI32_T *meter_index)
{
    UI32_T i = 0;

    for (i = 0; i < SYS_ADPT_DIFFSERV_MAX_NBR_OF_METER; i++)
    {
        if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->meter_table[i].row_status))
        {
            memset(&shmem_data_p->meter_table[i], 0, sizeof(RULE_TYPE_TBParamEntry_T));
            RULE_OM_SET_ENTRY_ACTIVE(shmem_data_p->meter_table[i].row_status);

            *meter_index = i + 1; /* to 1-based index */
            return TRUE;
        }
    }
    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalGetActionIndexFromPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  get action index by policy_map_index and class_map_index
 * INPUT:    policy_map_index, class_map_index
 * OUTPUT:   action_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalGetActionIndexFromPolicyMapElement(UI32_T policy_map_index, UI32_T class_map_index, UI32_T *action_index)
{
    UI32_T                      i = 0;
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    if (NULL == action_index)
        return RULE_TYPE_FAIL;

    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        return RULE_TYPE_FAIL; /* bad index */

    for (i = 0 ;i < policy_map_p->element_count; i++)
    {
        policy_element_p = &shmem_data_p->policy_map_element_table[policy_map_p->policy_map_element_index_list[i] - 1]; /* to zero-based */
        if (policy_element_p->class_map_index == class_map_index)
        {
            *action_index = policy_element_p->action_index;
            return RULE_TYPE_OK;
        }
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalAllocate_Action_Index
 *------------------------------------------------------------------------------
 * PURPOSE:  search an unused action entry and allocate it
 * INPUT:    none
 * OUTPUT:   action_index
 * RETURN:   TRUE -- succeeded / FALSE -- failed
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
static BOOL_T RULE_OM_LocalAllocate_Action_Index(UI32_T *action_index)
{
    UI32_T i = 0;

    if (NULL == action_index)
        return FALSE;

    for (i = 0; i < SYS_ADPT_DIFFSERV_MAX_NBR_OF_ACTIONS; i++)
    {
        if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->action_table[i].row_status))
        {
            memset(&shmem_data_p->action_table[i], 0, sizeof(RULE_TYPE_Action_T));
            RULE_OM_SET_ENTRY_ACTIVE(shmem_data_p->action_table[i].row_status);
            *action_index = i + 1; /* to 1-based index */
            return TRUE;
        }
    }
    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalFreeAction
 *------------------------------------------------------------------------------
 * PURPOSE  : free action buffer
 * INPUT    : action_index
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static void RULE_OM_LocalFreeAction(UI32_T action_index)
{
    RULE_TYPE_Action_T    *action_p;

    if (RULE_OM_IS_BAD_ACTION_IDX(action_index))
        return;

    action_p = &shmem_data_p->action_table[action_index - 1]; /* to zero-based array index */

    memset(action_p, 0, sizeof(RULE_TYPE_Action_T));
    RULE_OM_SET_ENTRY_DESTROY(action_p->row_status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetClassMapByIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get policy-map by index
 * INPUT    : policy_map_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_ClassMap_T *RULE_OM_LocalGetClassMapByIndex(UI32_T class_map_index)
{
    if (RULE_OM_IS_BAD_CLASS_MAP_IDX(class_map_index))
        return NULL;

    if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->class_map_table[class_map_index - 1].row_status)) /* to array index */
        return NULL;

    return &shmem_data_p->class_map_table[class_map_index - 1];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalSearchPolicyMapByName
 *------------------------------------------------------------------------------
 * PURPOSE  : search policy-map by name
 * INPUT    : policy_map_name
 * OUTPUT   : policy_map_index
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalSearchPolicyMapByName(const char *policy_map_name, UI32_T *policy_map_index)
{
    UI32_T  index;

    if ((NULL == policy_map_name) || (NULL == policy_map_index))
        return RULE_TYPE_FAIL;

    for (index = 0; SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP > index; ++index)
    {
        if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->policy_map_table[index].row_status))
            continue;

        if (strcmp((char *)policy_map_name, (char *)(shmem_data_p->policy_map_table[index].name)) != 0)
            continue;

        *policy_map_index = index + 1; /* to 1-based index */
        return RULE_TYPE_OK;
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetPolicyMapByIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get policy-map by index
 * INPUT    : policy_map_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_PolicyMap_T *RULE_OM_LocalGetPolicyMapByIndex(UI32_T policy_map_index)
{
    if (RULE_OM_IS_BAD_POLICY_MAP_IDX(policy_map_index))
        return NULL;

    if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->policy_map_table[policy_map_index - 1].row_status)) /* to array index */
        return NULL;

    return &shmem_data_p->policy_map_table[policy_map_index - 1];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalPolicyMapPtrToIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get policy-map index from pointer
 * INPUT    : action_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalPolicyMapPtrToIndex(
    RULE_TYPE_PolicyMap_T *policy_map_p)
{
    ASSERT(shmem_data_p->policy_map_table <= policy_map_p);

    return (policy_map_p - &shmem_data_p->policy_map_table[0]) + 1;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalGetNextPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Use *policy_map_index=0 to get first
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_PolicyMap_T *
RULE_OM_LocalGetNextPolicyMap(
    UI32_T *policy_map_index)
{
    RULE_TYPE_PolicyMap_T   *policy_map_p;
    UI32_T                  next_policy_map_index;

    if (NULL == policy_map_index)
    {
        return NULL;
    }

    for (next_policy_map_index = *policy_map_index + 1;
         SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP >= next_policy_map_index; ++next_policy_map_index)
    {
        policy_map_p = &shmem_data_p->policy_map_table[next_policy_map_index - 1]; /* to zero-based array index */

        if (RULE_OM_IS_ENTRY_DESTROY(policy_map_p->row_status))
            continue;

        *policy_map_index = next_policy_map_index;

        return policy_map_p;
    }

    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetNextPolicyMapByClassMapIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get next policy-map by class-map index
 * INPUT    : action_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_PolicyMap_T *
RULE_OM_LocalGetNextPolicyMapByClassMapIndex(
    UI32_T class_map_index,
    UI32_T *policy_map_index_p)
{
    UI32_T                      element_cnt;
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    while (NULL != (policy_map_p = RULE_OM_LocalGetNextPolicyMap(policy_map_index_p)))
    {
        for (element_cnt = 0; element_cnt < policy_map_p->element_count; ++element_cnt)
        {
            policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_map_p->policy_map_element_index_list[element_cnt]);
            if (NULL == policy_element_p)
            {
                continue;
            }

            if (policy_element_p->class_map_index == class_map_index)
            {
                return policy_map_p;
            }
        }
    }

    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetNextPolicyMapByMeterIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get next policy-map by meter index
 * INPUT    : action_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_PolicyMap_T *
RULE_OM_LocalGetNextPolicyMapByMeterIndex(
    UI32_T meter_index,
    UI32_T *policy_map_index_p)
{
    UI32_T                      element_cnt;
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    while (NULL != (policy_map_p = RULE_OM_LocalGetNextPolicyMap(policy_map_index_p)))
    {
        for (element_cnt = 0; element_cnt < policy_map_p->element_count; ++element_cnt)
        {
            policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_map_p->policy_map_element_index_list[element_cnt]);
            if (NULL == policy_element_p)
            {
                continue;
            }

            if (policy_element_p->meter_index == meter_index)
            {
                return policy_map_p;
            }
        }
    }

    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetNextPolicyMapByActionIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get next policy-map by action index
 * INPUT    : action_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_PolicyMap_T *
RULE_OM_LocalGetNextPolicyMapByActionIndex(
    UI32_T action_index,
    UI32_T *policy_map_index_p)
{
    UI32_T                      element_cnt;
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    while (NULL != (policy_map_p = RULE_OM_LocalGetNextPolicyMap(policy_map_index_p)))
    {
        for (element_cnt = 0; element_cnt < policy_map_p->element_count; ++element_cnt)
        {
            policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_map_p->policy_map_element_index_list[element_cnt]);
            if (NULL == policy_element_p)
            {
                continue;
            }

            if (policy_element_p->action_index == action_index)
            {
                return policy_map_p;
            }
        }
    }

    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalFreePolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE  : free policy-map element entry
 * INPUT    : pmap_ele_index
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static void RULE_OM_LocalFreePolicyMapElement(UI32_T pmap_ele_index)
{
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    if (RULE_OM_IS_BAD_POLICY_ELEMENT_IDX(pmap_ele_index))
        return;

    policy_element_p = &shmem_data_p->policy_map_element_table[pmap_ele_index - 1]; /* to zero-based array index */

    memset(policy_element_p, 0, sizeof(RULE_TYPE_PolicyElement_T));
    RULE_OM_SET_ENTRY_DESTROY(policy_element_p->row_status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalAllocatePolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE  : search an unused entry and allocate it
 * INPUT    : none
 * OUTPUT   : policy_map_element_index
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static BOOL_T RULE_OM_LocalAllocatePolicyMapElement(UI32_T *policy_map_element_index)
{
    UI32_T  index;

    if (NULL == policy_map_element_index)
        return FALSE;

    for (index = 0; SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS > index; ++index)
    {
        if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->policy_map_element_table[index].row_status))
        {
            memset(&shmem_data_p->policy_map_element_table[index], 0, sizeof(RULE_TYPE_PolicyElement_T));
            RULE_OM_SET_ENTRY_ACTIVE(shmem_data_p->policy_map_element_table[index].row_status);

            *policy_map_element_index = index + 1; /* to 1-based index */
            return TRUE;
        }
    }
    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetPolicyMapElementByIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get policy-map element by policy_map_element_index
 * INPUT    : policy_map_element_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_PolicyElement_T *RULE_OM_LocalGetPolicyMapElementByIndex(UI32_T policy_map_element_index)
{
    if (RULE_OM_IS_BAD_POLICY_ELEMENT_IDX(policy_map_element_index))
        return NULL;

    if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->policy_map_element_table[policy_map_element_index - 1].row_status)) /* to array index */
        return NULL;

    return &shmem_data_p->policy_map_element_table[policy_map_element_index - 1]; /* to zero-based array index */
}
#if 0
/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetPolicyMapElementByActionIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get policy-map element by action_index
 * INPUT    : action_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_PolicyElement_T *RULE_OM_LocalGetPolicyMapElementByActionIndex(UI32_T action_index)
{
    RULE_TYPE_PolicyElement_T   *policy_element_p;
    UI32_T                      policy_element_cnt;

    if (RULE_OM_IS_BAD_ACTION_IDX(action_index))
        return NULL;

    /* search every policy-map element */
    for (policy_element_cnt = 0; SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS > policy_element_cnt; ++policy_element_cnt)
    {
        policy_element_p = &shmem_data_p->policy_map_element_table[policy_element_cnt];

        if (RULE_OM_IS_ENTRY_DESTROY(policy_element_p->row_status))
            continue;

        if (action_index != policy_element_p->action_index)
            continue;

        return policy_element_p;
    }

    return NULL;
}
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE  : get policy-map element by policy_map_index and class_map_index
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_PolicyElement_T *RULE_OM_LocalGetPolicyMapElement(UI32_T policy_map_index, UI32_T class_map_index)
{
    UI32_T                      index = 0;
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        return NULL;

    for (index = 0; index < policy_map_p->element_count; ++index)
    {
        policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_map_p->policy_map_element_index_list[index]);
        if (NULL == policy_element_p)
        {
            /* should not go here */
            continue;
        }

        if (policy_element_p->class_map_index == class_map_index)
            return policy_element_p; /* found */
    }

    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalFreeMeter
 *------------------------------------------------------------------------------
 * PURPOSE  : free meter buffer
 * INPUT    : meter_index
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static void RULE_OM_LocalFreeMeter(UI32_T meter_index)
{
    RULE_TYPE_TBParamEntry_T    *meter_p;

    if (RULE_OM_IS_BAD_METER_IDX(meter_index))
        return;

    meter_p = &shmem_data_p->meter_table[meter_index - 1]; /* to zero-based array index */

    meter_p->rate = 0;
    meter_p->burst_size = 0;
    meter_p->interval = 0;
    RULE_OM_SET_ENTRY_DESTROY(meter_p->row_status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetMeterEntryByIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get meter by meter_index
 * INPUT    : meter_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_TBParamEntry_T *RULE_OM_LocalGetMeterEntryByIndex(UI32_T meter_index)
{
    if (RULE_OM_IS_BAD_METER_IDX(meter_index))
        return NULL;

    if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->meter_table[meter_index - 1].row_status)) /* to array index */
        return NULL;

    return &shmem_data_p->meter_table[meter_index - 1]; /* to zero-based array index */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetActionEntryByIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get action by action_index
 * INPUT    : action_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_Action_T *RULE_OM_LocalGetActionEntryByIndex(UI32_T action_index)
{
    if (RULE_OM_IS_BAD_ACTION_IDX(action_index))
        return NULL;

    if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->action_table[action_index - 1].row_status)) /* to array index */
        return NULL;

    return &shmem_data_p->action_table[action_index - 1]; /* to zero-based array index */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalCreateMf
 *------------------------------------------------------------------------------
 * PURPOSE  : create a multi-filter by mf_entry
 * INPUT    : mf_entry
 * OUTPUT   : ace_index
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalCreateMf(const RULE_TYPE_MfEntry_T *mf_entry, UI32_T *ace_index)
{
    RULE_TYPE_Ace_Entry_T   *ace_entry_p;
    RULE_TYPE_RETURN_TYPE_T result;

    if ((NULL == mf_entry) || (NULL == ace_index))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    result = RULE_OM_LocalAllocateAceEntryWithRange(0, SYS_ADPT_MAX_NBRS_OF_ACE_OF_SYSTEM, ace_index);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    ace_entry_p = &shmem_data_p->ace_table[*ace_index - 1]; /* to zero-based array index */

    result = RULE_OM_LocalConvertMf2Ace(mf_entry, ace_entry_p);
    if (RULE_TYPE_OK != result)
    {
        RULE_OM_LocalFreeAceEntry(*ace_index);
        *ace_index = 0;
        return result;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalConvertMf2Ace
 *------------------------------------------------------------------------------
 * PURPOSE  : Convert MF entry to ACE
 * INPUT    : mf_entry_p    - MF entry
 * OUTPUT   : ace_entry_p   - ACE
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalConvertMf2Ace(
    const RULE_TYPE_MfEntry_T *mf_entry_p,
    RULE_TYPE_Ace_Entry_T *ace_entry_p)
{
    ASSERT(NULL != mf_entry_p && NULL != ace_entry_p);

    switch (mf_entry_p->mf_type)
    {
        case RULE_TYPE_MF_PRECEDENCE:
            ace_entry_p->ace_type = RULE_TYPE_IP_EXT_ACL;
            RULE_OM_LocalSetAceEntryToDefaultValue(ace_entry_p);
            ace_entry_p->u.ip.acePrec = mf_entry_p->mf_value;
            break;

        case RULE_TYPE_MF_DSCP:
            ace_entry_p->ace_type = RULE_TYPE_IP_EXT_ACL;
            RULE_OM_LocalSetAceEntryToDefaultValue(ace_entry_p);
            ace_entry_p->u.ip.aceDscp = mf_entry_p->mf_value;
            break;

        case RULE_TYPE_MF_VLAN:
            ace_entry_p->ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_LocalSetAceEntryToDefaultValue(ace_entry_p);

            ace_entry_p->u.mac.aceVidOp  = VAL_diffServMacAceVidOp_equal;
            ace_entry_p->u.mac.aceMinVid = mf_entry_p->mf_value;
            ace_entry_p->u.mac.aceVidBitmask = MAX_diffServMacAceVidBitmask;
            break;

#if (SYS_CPNT_ACL_IPV6_EXT_TRAFFIC_CLASS == TRUE)
        case RULE_TYPE_MF_DSCP_IPV6:
            ace_entry_p->ace_type = RULE_TYPE_IPV6_EXT_ACL;
            RULE_OM_LocalSetAceEntryToDefaultValue(ace_entry_p);
            ace_entry_p->u.ipv6.aceDscp = mf_entry_p->mf_value;
            break;
#endif /* SYS_CPNT_ACL_IPV6_EXT_TRAFFIC_CLASS */

#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_COS == TRUE)
        case RULE_TYPE_MF_COS:
            ace_entry_p->ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_LocalSetAceEntryToDefaultValue(ace_entry_p);

            ace_entry_p->u.mac.aceCosOp = VAL_diffServMacAceCosOp_equal;
            ace_entry_p->u.mac.aceMinCos = mf_entry_p->mf_value;
            ace_entry_p->u.mac.aceCosBitmask = MAX_diffServMacAceCosBitmask;
            break;
#endif /* SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_COS */

#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_SOURCE_PORT == TRUE)
        case RULE_TYPE_MF_SOURCE_PORT:
            ace_entry_p->ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_LocalSetAceEntryToDefaultValue(ace_entry_p);

            ace_entry_p->u.mac.aceSourcePortOp = VAL_diffServMacAceSourcePortOp_equal;
            ace_entry_p->u.mac.aceMinSourcePort = mf_entry_p->mf_value;
            ace_entry_p->u.mac.aceSourcePortBitmask = 0xff;
            break;
#endif /* SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_SOURCE_PORT */

        default:
            return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalAllocateAclWithRange
 *------------------------------------------------------------------------------
 * PURPOSE  : allocate ACL from min and max index
 * INPUT    : min_index - min index
 *            max_index - max index
 * OUTPUT   : acl_index
 * RETURN   : Error code
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalAllocateAclWithRange(
    UI32_T min_index,
    UI32_T max_index,
    UI32_T *acl_index)
{
    UI32_T index;

    if (NULL == acl_index ||
        _countof(shmem_data_p->acl_table) < min_index ||
        _countof(shmem_data_p->acl_table) < max_index ||
        max_index < min_index)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    for (index = min_index; index < max_index; ++index)
    {
        if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->acl_table[index].row_status))
        {
            memset(&shmem_data_p->acl_table[index], 0, sizeof(shmem_data_p->acl_table[index]));

            RULE_OM_SET_ENTRY_ACTIVE(shmem_data_p->acl_table[index].row_status);
            shmem_data_p->acl_table[index].super.type = RULE_TYPE_INST_OM_USER_ACL;

            *acl_index = index + 1;
            return RULE_TYPE_OK;
        }
    }

    return RULE_TYPE_ACL_FULLED;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalFreeAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : free acl buffer
 * INPUT    : acl_p
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static void RULE_OM_LocalFreeAcl(RULE_TYPE_Acl_T *acl_entry)
{
    if (NULL == acl_entry)
        return;

    memset(acl_entry, 0, sizeof(RULE_TYPE_Acl_T));
    RULE_OM_SET_ENTRY_DESTROY(acl_entry->row_status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get acl by index
 * INPUT    : acl_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_Acl_T *RULE_OM_LocalGetAclByIndex(UI32_T acl_index)
{
    if (RULE_OM_IS_BAD_ACL_IDX(acl_index))
        return NULL;

    if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->acl_table[acl_index - 1].row_status)) /* to array index */
        return NULL;

    return &shmem_data_p->acl_table[acl_index - 1];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalDelAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete an ACL.
 * INPUT:    acl_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalDelAcl(
    UI32_T acl_index)
{
    RULE_TYPE_Acl_T     *acl_p;

    if (RULE_OM_IS_BAD_ACL_IDX(acl_index))
    {
        return RULE_TYPE_FAIL;
    }

    acl_p = &shmem_data_p->acl_table[acl_index - 1];

    if (FALSE == acl_p->row_status)
    {
        return RULE_TYPE_FAIL;
    }

    RULE_OM_LocalDelAllAceFromAcl(acl_p);

    RULE_OM_LocalFreeAcl(acl_p);

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalGetAclIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of ACL by it's name.
 * INPUT:    acl_name
 * OUTPUT:   acl_index.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalGetAclIdByName(const char *acl_name, UI32_T *acl_index)
{
    UI32_T  index;

    if ((NULL == acl_name) || (NULL == acl_index))
        return RULE_TYPE_FAIL;

    /* sequential search */
    for (index = 0; _countof(shmem_data_p->acl_table) > index; ++index)
    {
        if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->acl_table[index].row_status))
            continue;

        if (0 == strcmp((char *)acl_name, (char *)shmem_data_p->acl_table[index].acl_name))
        {
            *acl_index = index + 1; /* to 1-based index */
            return RULE_TYPE_OK;
        }
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalGetNextAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACL by acl_index
 * INPUT:    acl_index
 * OUTPUT:   acl_index, acl_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalGetNextAclByIndex(
    UI32_T *acl_index,
    RULE_TYPE_Acl_T *acl_entry)
{
    RULE_TYPE_Acl_T     *acl_p;
    UI32_T              next_acl_index;

    if ((NULL == acl_index) || (NULL == acl_entry))
        return RULE_TYPE_FAIL;

    for (next_acl_index = *acl_index + 1; _countof(shmem_data_p->acl_table) >= next_acl_index; ++next_acl_index)
    {
        acl_p = &shmem_data_p->acl_table[next_acl_index - 1]; /* to zero-based array index */

        if (RULE_OM_IS_ENTRY_AVAILABLE(acl_p->row_status))
        {
            *acl_index = next_acl_index;
            memcpy(acl_entry, acl_p, sizeof(RULE_TYPE_Acl_T));
            return RULE_TYPE_OK;
        }
    }
    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalGetNextAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE for ACL
 * INPUT:    acl_p, ace_index
 * OUTPUT:   ace_index, acl_type, ace_entry.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalGetNextAceByAcl(
    const RULE_TYPE_Acl_T *acl_p,
    UI32_T *ace_index,
    RULE_TYPE_Ace_Entry_T **next_ace_entry_pp)
{
    RULE_TYPE_Ace_Entry_T   *ace_entry_p;

    if (0 == *ace_index && 0 < acl_p->ace_count)
    {
        ASSERT(RULE_TYPE_NIL != acl_p->super.links.first_node.type);

        *next_ace_entry_pp = RULE_OM_PTR2INST(acl_p->super.links.first_node);

        ASSERT(NULL != *next_ace_entry_pp);

        *ace_index = RULE_OM_INDEX_OF(*next_ace_entry_pp) + 1;

        return RULE_TYPE_OK;
    }

    ace_entry_p = RULE_OM_LocalGetAceByIndex(*ace_index);
    if (NULL == ace_entry_p)
    {
        return RULE_TYPE_FAIL;
    }

    if (RULE_TYPE_NIL != ace_entry_p->super.links.next.type)
    {
        *next_ace_entry_pp = RULE_OM_PTR2INST(ace_entry_p->super.links.next);

        ASSERT(NULL != *next_ace_entry_pp);

        *ace_index = RULE_OM_INDEX_OF(*next_ace_entry_pp) + 1;

        return RULE_TYPE_OK;
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalAllocateAceEntryWithRange
 *------------------------------------------------------------------------------
 * PURPOSE  : allocate an ace entry from ace buffer with min and max index
 * INPUT    : min_index - min index
 *            max_index - max index
 * OUTPUT   : ace_index
 * RETURN   : Error code
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalAllocateAceEntryWithRange(
    UI32_T min_index,
    UI32_T max_index,
    UI32_T *ace_index)
{
    UI32_T  index;

    if (_countof(shmem_data_p->ace_table) < min_index ||
        _countof(shmem_data_p->ace_table) < max_index ||
        max_index < min_index ||
        NULL == ace_index)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    /* sequential search */
    for (index = min_index; index < max_index; ++index)
    {
        if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->ace_table[index].row_status))
        {
            memset(&shmem_data_p->ace_table[index], 0, sizeof(shmem_data_p->ace_table[index]));

            RULE_OM_SET_ENTRY_ACTIVE(shmem_data_p->ace_table[index].row_status);
            shmem_data_p->ace_table[index].super.type = RULE_TYPE_INST_OM_USER_ACE;

            *ace_index = index + 1; /* translate to 1-based index */
            return RULE_TYPE_OK;
        }
    }

    return RULE_TYPE_ACE_FULLED;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalFreeAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : free ace buffer
 * INPUT    : ace_index
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static void RULE_OM_LocalFreeAceEntry(UI32_T ace_index)
{
    if (RULE_OM_IS_BAD_ACE_IDX(ace_index))
        return;

    memset(&shmem_data_p->ace_table[ace_index - 1], 0, sizeof(RULE_TYPE_Ace_Entry_T));
    RULE_OM_SET_ENTRY_DESTROY(shmem_data_p->ace_table[ace_index - 1].row_status);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_LocalSetAceEntryToDefaultValue
 *------------------------------------------------------------------------------
 * PURPOSE  : make ace to default value according to ace_type
 * INPUT    : ace_entry
 * OUTPUT   : none
 * RETURN   : Error code
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalSetAceEntryToDefaultValue(RULE_TYPE_Ace_Entry_T *ace_entry)
{
    RULE_TYPE_IpAce_Entry_T     *ip_ace;
    RULE_TYPE_MacAce_Entry_T    *mac_ace;
#if (SYS_CPNT_DAI == TRUE)
    RULE_TYPE_ArpAce_Entry_T    *arp_ace;
#endif
    RULE_TYPE_AclType_T         ace_type;
    UI32_T                      row_status;

    if (NULL == ace_entry)
        return RULE_TYPE_FAIL;

    ace_type = ace_entry->ace_type; /* keep original before ZeroMemory */
    row_status = ace_entry->row_status;

    memset(ace_entry, 0, sizeof(RULE_TYPE_Ace_Entry_T));
    ace_entry->ace_type = ace_type;
    ace_entry->row_status = row_status;

    ace_entry->access = RULE_TYPE_ACE_PERMIT;

    switch (ace_entry->ace_type)
    {
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
        case RULE_TYPE_IP_ACL:

            ip_ace = &ace_entry->u.ip;
            ip_ace->aceProtocol = RULE_TYPE_UNDEF_IP_PROTOCOL;
            ip_ace->acePrec = RULE_TYPE_UNDEF_IP_PRECEDENCE;
            ip_ace->aceTos = RULE_TYPE_UNDEF_IP_TOS;
            ip_ace->aceDscp = RULE_TYPE_UNDEF_IP_DSCP;
            ip_ace->aceSourcePortOp = VAL_diffServIpAceSourcePortOp_noOperator;
            ip_ace->aceSourcePortBitmask = MAX_diffServIpAceSourcePortBitmask;
            ip_ace->aceDestPortBitmask = MAX_diffServIpAceDestPortBitmask;;
            ip_ace->aceDestPortOp = VAL_diffServIpAceDestPortOp_noOperator;
            ip_ace->aceControlCode = RULE_TYPE_UNDEF_IP_CONTROL_CODE;

            /*tenderbulk.w*/
            /*these defaulted values are ported from 430 initialization */
            ip_ace->aceIcmpType=256;
            ip_ace->aceIcmpCode=0;
            ip_ace->aceFragmentInfo=3 ;
            #if 1
            ip_ace->aceIpOptionOp=1;
            ip_ace->aceMinIpOption=255;
            ip_ace->aceMaxIpOption=255;
            #endif
//
//
// Use Recursive Call here ??
//
//

#if (RULE_TYPE_ACE_MAC_IP_MIX == TRUE)
            goto set_mac_ace_to_default;
#else
            break;
#endif /* RULE_TYPE_ACE_MAC_IP_MIX */

        case RULE_TYPE_MAC_ACL:
set_mac_ace_to_default:
            mac_ace = &ace_entry->u.mac;
            mac_ace->aceVidOp = VAL_diffServMacAceVidOp_noOperator;
            mac_ace->aceVidBitmask = MAX_diffServMacAceVidBitmask;
            mac_ace->aceEtherTypeOp = VAL_diffServMacAceEtherTypeOp_noOperator;
            mac_ace->aceEtherTypeBitmask = MAX_diffServMacAceEtherTypeBitmask;
            mac_ace->aceCosOp = VAL_diffServMacAceCosOp_noOperator;
            mac_ace->acePktformat = VAL_diffServMacAcePktformat_any ;

            /*these defualted values are ported from 430 initialization,tenderbulk.w
             */
            mac_ace->aceCoSOp=1;
            mac_ace->aceCoS=6;
            mac_ace->aceCFIOp=1;
            mac_ace->aceCFI=7;
            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
        {
            RULE_TYPE_Ipv6Ace_Entry_T   *ipv6_ace;

            ipv6_ace = &ace_entry->u.ipv6;
            ipv6_ace->aceNextHeader = RULE_TYPE_UNDEF_IPV6_NEXT_HEADER;
            ipv6_ace->aceDscp = RULE_TYPE_UNDEF_IPV6_DSCP;
            ipv6_ace->aceFlowLabel = RULE_TYPE_UNDEF_IPV6_FLOW_LABEL;
            ipv6_ace->aceSourcePortOp = VAL_diffServIpv6AceSourcePortOp_noOperator;
            ipv6_ace->aceDestPortOp = VAL_diffServIpv6AceDestPortOp_noOperator;

#if (RULE_TYPE_ACE_MAC_IP_MIX == TRUE)
            goto set_mac_ace_to_default;
#else
            break;
#endif /* RULE_TYPE_ACE_MAC_IP_MIX */
        }

#if (SYS_CPNT_DAI == TRUE)
        case RULE_TYPE_ARP_ACL:
            arp_ace = &ace_entry->u.arp;
            arp_ace->aceSenderIpAddrBitmask = 0;
            arp_ace->aceTargetIpAddrBitmask = 0;
            memset (arp_ace->aceSenderMacAddrBitmask, 0, SYS_ADPT_MAC_ADDR_LEN);
            memset (arp_ace->aceTargetMacAddrBitmask, 0, SYS_ADPT_MAC_ADDR_LEN);
            arp_ace->aceIsLog = FALSE;
            break;
#endif /* SYS_CPNT_DAI */

        default: /* should not go here */
            return RULE_TYPE_FAIL;
    }

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    ace_entry->time_range_index              = RULE_TYPE_UNDEF_TIME_RANGE;
    memset (ace_entry->time_range_name, 0, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
#endif /* end of #if (SYS_CPNT_TIME_BASED_ACL == TRUE) */

    return RULE_TYPE_OK;
}


static BOOL_T
RULE_OM_LocalIsSameAce(
    const RULE_TYPE_Acl_T *acl_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p,
    UI32_T *ace_index_p)
{
    RULE_TYPE_Ace_Entry_T   *w_ace_entry_p;

    RULE_TYPE_Ace_Entry_T cmp_entry;

    ASSERT(NULL != acl_p);

    memcpy(&cmp_entry, ace_entry_p, sizeof(cmp_entry));

    for (*ace_index_p = 0;
         RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, ace_index_p, &w_ace_entry_p);)
    {
        if (w_ace_entry_p->ace_type != ace_entry_p->ace_type)
        {
            continue;
        }

        if (w_ace_entry_p->access != ace_entry_p->access)
        {
            continue;
        }

        switch (w_ace_entry_p->ace_type)
        {
        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            if (0 == memcmp(&(w_ace_entry_p->u.ip),
                            &(cmp_entry.u.ip),
                            sizeof(RULE_TYPE_IpAce_Entry_T)))
            {
                return TRUE;
            }
            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            if (0 == memcmp(&(w_ace_entry_p->u.ipv6),
                            &(cmp_entry.u.ipv6),
                            sizeof(RULE_TYPE_Ipv6Ace_Entry_T)))
            {
                return TRUE;
            }
            break;

        case RULE_TYPE_MAC_ACL:
            if (0 == memcmp(&(w_ace_entry_p->u.mac),
                            &(cmp_entry.u.mac),
                            sizeof(RULE_TYPE_MacAce_Entry_T)))
            {
                return TRUE;
            }
            break;

#if (SYS_CPNT_DAI == TRUE)
        case RULE_TYPE_ARP_ACL:
            if (0 == memcmp(&(w_ace_entry_p->u.arp), &(cmp_entry.u.arp), sizeof(RULE_TYPE_ArpAce_Entry_T)))
            {
                return TRUE;
            }
            break;
#endif /* SYS_CPNT_DAI */

        default:
             break;
        }
    }

    return FALSE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_LocalIsSameAceWithoutAccessField
 *------------------------------------------------------------------------------
 * PURPOSE : Compare two ACE without access field.
 *   INPUT : src_p	- ACE 1
 *           dst_p  - ACE 2
 *  OUTPUT : None
 *  RETURN : TRUE/FALSE
 *    NOTE :
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_OM_LocalIsSameAceWithoutAccessField(
    const RULE_TYPE_Ace_Entry_T *src_p,
    const RULE_TYPE_Ace_Entry_T *dst_p)
{
    if (NULL == src_p|| NULL == dst_p)
    {
        return FALSE;
    }

    if (src_p->ace_type != dst_p->ace_type)
    {
        return FALSE;
    }

    switch (src_p->ace_type)
    {
        case RULE_TYPE_MAC_ACL:

        compare_mac_ace:
            if (0 == memcmp (&src_p->u.mac, &dst_p->u.mac, sizeof(dst_p->u.mac)))
            {
                return TRUE;
            }

            break;

        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            if (0 == memcmp (&src_p->u.ip, &dst_p->u.ip, sizeof(dst_p->u.ip)))
            {
#if (RULE_TYPE_ACE_MAC_IP_MIX == TRUE)
                goto compare_mac_ace;
#else
                return TRUE;
#endif
            }

            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            if (0 == memcmp (&src_p->u.ipv6,
                             &dst_p->u.ipv6,
                             sizeof(dst_p->u.ipv6)))
            {
#if (RULE_TYPE_ACE_MAC_IP_MIX == TRUE)
                goto compare_mac_ace;
#else
                return TRUE;
#endif
            }

            break;

        default:
            break;
    }

    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalAddAce2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a ACE to ACL.
 * INPUT:    acl_index, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_LocalAddAce2Acl(
    RULE_TYPE_Acl_T *acl_p,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_Ace_Entry_T *ace_entry)
{
    UI32_T                  ace_index = 0;
    RULE_TYPE_AclType_T     classified_acl_type, classified_ace_type;
    UI32_T                  ret;

    ASSERT(NULL != acl_p);
    ASSERT(NULL != ace_entry);

    if (SYS_ADPT_MAX_NBRS_OF_ACE <= acl_p->ace_count)
    {
        return RULE_TYPE_FAIL;
    }

    if ((RULE_TYPE_OK != RULE_OM_LocalClassifyAcl(acl_p, &classified_acl_type)) ||
        (RULE_TYPE_OK != RULE_OM_LocalClassifyAce(ace_entry, &classified_ace_type)))
    {
        return RULE_TYPE_FAIL;
    }

    if ((acl_type != acl_p->acl_type) || (classified_ace_type != classified_acl_type))
    {
        return RULE_TYPE_FAIL; /* wrong type */
    }

    if(RULE_TYPE_OK != RULE_OM_LocalValidateAce(ace_entry))
    {
        return RULE_TYPE_FAIL;
    }

    if (TRUE == RULE_OM_LocalIsSameAce(acl_p, ace_entry, &ace_index))
    {
        return RULE_TYPE_FAIL;
    }

    /* 0 ... SYS_ADPT_MAX_NBRS_OF_ACE_OF_SYSTEM ... RULE_TYPE_MAX_NBRS_OF_ACE_OF_SYSTEM
     * |_________ for user ________||_____________for internal used___________________|
     *       (flag_hide == FALSE)                (flag_hide == TRUE)
     */
#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) || (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
    if (acl_p->flag_hide == TRUE)
    {
        ret = RULE_OM_LocalAllocateAceEntryWithRange(SYS_ADPT_MAX_NBRS_OF_ACE_OF_SYSTEM,
            RULE_TYPE_MAX_NBRS_OF_ACE_OF_SYSTEM, &ace_index);
    }
    else
#endif /* #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) || (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE) */
    {
        ret = RULE_OM_LocalAllocateAceEntryWithRange(0,
            SYS_ADPT_MAX_NBRS_OF_ACE_OF_SYSTEM, &ace_index);
    }

    if (ret != RULE_TYPE_OK)
    {
        return ret;
    }

    {
        RULE_TYPE_INSTANCE_T super = shmem_data_p->ace_table[ace_index-1].super;

        memcpy(&(shmem_data_p->ace_table[ace_index-1]), ace_entry, sizeof(RULE_TYPE_Ace_Entry_T));
        RULE_OM_SET_ENTRY_ACTIVE(shmem_data_p->ace_table[ace_index-1].row_status);

        shmem_data_p->ace_table[ace_index-1].super = super;
    }

    {
        RULE_TYPE_Ace_Entry_T *ace_entry_p;

        ace_entry_p = RULE_OM_LocalGetAceByIndex(ace_index);
        ASSERT(NULL != ace_entry_p);

        RULE_OM_LL_LocalInsertEnd(RULE_OM_INST2PTR(acl_p), RULE_OM_INST2PTR(ace_entry_p));
    }

    ++(acl_p->ace_count);

    return  RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalCopyAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Copy ACL
 * INPUT  :  src_acl_index - source ACL index
 *           dst_acl_index - destination ACL index
 * OUTPUT :  none
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  The function will remove all ACE in destination ACL.
 *           If successed, destination ACL have the same ACE and type with
 *           source.
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalCopyAcl(
    UI32_T  src_acl_index,
    UI32_T  dst_acl_index)
{
    RULE_TYPE_Acl_T *src_acl_p = RULE_OM_LocalGetAclByIndex(src_acl_index);
    RULE_TYPE_Acl_T *dst_acl_p = RULE_OM_LocalGetAclByIndex(dst_acl_index);
    RULE_TYPE_Ace_Entry_T *src_ace_p;

    UI32_T res;
    UI32_T src_ace_index;

    if (NULL == src_acl_p || NULL == dst_acl_p)
    {
        return RULE_TYPE_FAIL;
    }

    if (src_acl_p == dst_acl_p)
    {
        return RULE_TYPE_OK;
    }

    RULE_OM_LocalDelAllAceFromAcl(dst_acl_p);

    dst_acl_p->acl_type = src_acl_p->acl_type;

    for (src_ace_index = 0;
         RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(src_acl_p, &src_ace_index, &src_ace_p); )
    {
        res = RULE_OM_LocalAddAce2Acl(dst_acl_p, src_ace_p->ace_type, src_ace_p);

        if (RULE_TYPE_OK != res)
        {
            goto copy_acl_failed;
        }
    }

    return RULE_TYPE_OK;

copy_acl_failed:
    RULE_OM_LocalDelAllAceFromAcl(dst_acl_p);
    return res;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalDelAllAceFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete all ACE in an ACL
 * INPUT  :  acl_index - ACL index
 * OUTPUT :  none
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalDelAllAceFromAcl(
    RULE_TYPE_Acl_T *acl_p)
{
    RULE_TYPE_INSTANCE_PTR_T ace_inst_p;

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T  ace_index;

    ASSERT(NULL != acl_p);

    if (NULL == acl_p)
    {
        return RULE_TYPE_ACL_NONEXISTED;
    }

    for (ace_inst_p = RULE_OM_PTR2INST(acl_p->super.links.last_node);
         NULL != ace_inst_p;
         ace_inst_p = RULE_OM_PTR2INST(acl_p->super.links.last_node))
    {
        ace_index = RULE_OM_INDEX_OF(ace_inst_p) + 1;

        result = RULE_OM_LocalDelAceFromAcl(acl_p, ace_index);
        if (RULE_TYPE_OK != result)
        {
            return result;
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalDelAceFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : remove the specified ace and move up the following ace from acl
 * INPUT    : acl_index, ace_index
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalDelAceFromAcl(
    RULE_TYPE_Acl_T *acl_p,
    UI32_T ace_index)
{
    RULE_TYPE_Ace_Entry_T   *ace_entry_p;

    ASSERT(NULL != acl_p);

    if (RULE_TYPE_OK != RULE_OM_LocalIsAclHaveAce(acl_p, ace_index))
        return RULE_TYPE_FAIL; /* bad index */

    ace_entry_p = RULE_OM_LocalGetAceByIndex(ace_index);
    ASSERT(NULL != ace_entry_p);

    RULE_OM_SET_ENTRY_DESTROY(ace_entry_p->row_status);

    RULE_OM_LL_LocalRemove(RULE_OM_INST2PTR(acl_p), RULE_OM_INST2PTR(ace_entry_p));

    ASSERT(0 < acl_p->ace_count);

    --(acl_p->ace_count);

    return RULE_TYPE_OK;
}

static BOOL_T RULE_OM_LocalUpdateAction(UI32_T action_index, RULE_TYPE_Action_T *action_entry)
{
    RULE_TYPE_Action_T  *action_p;

    action_p = &shmem_data_p->action_table[action_index - 1];
    /*in action*/
    if (action_entry->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_PRI)
    {
        action_p->action_bitmap |= RULE_TYPE_ACTION_PKT_NEW_PRI;
        action_p->action_bitmap &= (~(RULE_TYPE_ACTION_PKT_NEW_TOS));
        action_p->action_bitmap &= (~(RULE_TYPE_ACTION_PKT_NEW_DSCP));
        action_p->action_bitmap &= (~(RULE_TYPE_ACTION_PKT_NEW_PHB));
        action_p->pkt_new_pri = action_entry->pkt_new_pri;
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_PHB)
    {
        action_p->action_bitmap |= RULE_TYPE_ACTION_PKT_NEW_PHB;
        action_p->action_bitmap &= (~(RULE_TYPE_ACTION_PKT_NEW_TOS));
        action_p->action_bitmap &= (~(RULE_TYPE_ACTION_PKT_NEW_DSCP));
        action_p->action_bitmap &= (~(RULE_TYPE_ACTION_PKT_NEW_PRI));
        action_p->pkt_new_phb = action_entry->pkt_new_phb;
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_TOS)
    {
        action_p->action_bitmap |= RULE_TYPE_ACTION_PKT_NEW_TOS;
        action_p->action_bitmap &= (~(RULE_TYPE_ACTION_PKT_NEW_PRI));
        action_p->action_bitmap &= (~(RULE_TYPE_ACTION_PKT_NEW_DSCP));
        action_p->action_bitmap &= (~(RULE_TYPE_ACTION_PKT_NEW_PHB));
        action_p->pkt_new_tos = action_entry->pkt_new_tos;
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_DSCP)
    {
        action_p->action_bitmap |= RULE_TYPE_ACTION_PKT_NEW_DSCP;
        action_p->action_bitmap &= (~(RULE_TYPE_ACTION_PKT_NEW_PRI));
        action_p->action_bitmap &= (~(RULE_TYPE_ACTION_PKT_NEW_TOS));
        action_p->action_bitmap &= (~(RULE_TYPE_ACTION_PKT_NEW_PHB));
        action_p->pkt_new_dscp = action_entry->pkt_new_dscp;
    }

    /*out action */
    if (action_entry->action_bitmap & RULE_TYPE_ACTION_RED_DROP)
    {
        action_p->action_bitmap |= RULE_TYPE_ACTION_RED_DROP;
        action_p->action_bitmap &= ~RULE_TYPE_ACTION_RED_PKT_NEW_DSCP;
    }
    if (action_entry->action_bitmap & RULE_TYPE_ACTION_RED_PKT_NEW_DSCP)
    {
        action_p->action_bitmap |= RULE_TYPE_ACTION_RED_PKT_NEW_DSCP;
        action_p->action_bitmap &= ~RULE_TYPE_ACTION_RED_DROP;
        action_p->red_pkt_new_dscp = action_entry->red_pkt_new_dscp;
    }

    /*conform action */
    if (action_entry->action_bitmap & RULE_TYPE_ACTION_GREEN_PKT_TRANSMIT)
    {
        action_p->action_bitmap &= ~RULE_TYPE_GREEN_ACTION;
        action_p->action_bitmap |= RULE_TYPE_ACTION_GREEN_PKT_TRANSMIT;
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_GREEN_PKT_DROP)
    {
        action_p->action_bitmap &= ~RULE_TYPE_GREEN_ACTION;
        action_p->action_bitmap |= RULE_TYPE_ACTION_GREEN_PKT_DROP;
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_GREEN_PKT_NEW_DSCP)
    {
        action_p->action_bitmap &= ~RULE_TYPE_GREEN_ACTION;
        action_p->action_bitmap |= RULE_TYPE_ACTION_GREEN_PKT_NEW_DSCP;
        action_p->green_pkt_new_dscp = action_entry->green_pkt_new_dscp;
    }

    /*exceed action */
    if (action_entry->action_bitmap & RULE_TYPE_ACTION_YELLOW_DROP)
    {
        action_p->action_bitmap |= RULE_TYPE_ACTION_YELLOW_DROP;
        action_p->action_bitmap &= ~RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP;
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP)
    {
        action_p->action_bitmap |= RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP;
        action_p->action_bitmap &= ~RULE_TYPE_ACTION_YELLOW_DROP;
        action_p->yellow_pkt_new_dscp = action_entry->yellow_pkt_new_dscp;
    }

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    if (action_entry->action_bitmap & RULE_TYPE_ACTION_PKT_PRI)
    {
        action_p->action_bitmap |= RULE_TYPE_ACTION_PKT_PRI;
        action_p->pkt_pri = action_entry->pkt_pri;
    }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetAceByIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get ace by index
 * INPUT    : ace_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_Ace_Entry_T *RULE_OM_LocalGetAceByIndex(UI32_T ace_index)
{
    if (RULE_OM_IS_BAD_ACE_IDX(ace_index))
        return NULL;

    if (RULE_OM_IS_ENTRY_DESTROY(shmem_data_p->ace_table[ace_index - 1].row_status)) /* to array index */
        return NULL;

    return &shmem_data_p->ace_table[ace_index - 1];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalIsValidIfindex
 *------------------------------------------------------------------------------
 * PURPOSE  : Check the ifindex with direciton is validated
 * INPUT    : ifindex, inout_profile
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_OM_LocalIsValidIfindex(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction)
{
    if (RULE_TYPE_OK != RULE_TYPE_VALIDATE_IFINDEX(ifindex))
    {
        return FALSE;
    }

    if (RULE_TYPE_INBOUND != direction
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        && RULE_TYPE_OUTBOUND != direction)
#else
        )
#endif /* SYS_CPNT_QOS_V2_EGRESS_PORT */
    {
        return FALSE;
    }

    if (RULE_TYPE_IS_ALL_PORTS(ifindex))
    {
#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
        /* Global ACL only support on ingress
         */
        if (direction == RULE_TYPE_OUTBOUND)
        {
            return FALSE;
        }
#else
        return FALSE;
#endif /* (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE) */
    }

    if (RULE_TYPE_IS_CPU_PORT(ifindex))
    {
#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        /* Control plane only support egress
         */
        if (direction == RULE_TYPE_INBOUND)
        {
            return FALSE;
        }
#else
        return FALSE;
#endif /* (SYS_CPNT_QOS_V2_CPU_PORT == TRUE) */
    }

    if (RULE_TYPE_IS_TRUNK(ifindex))
    {
        if (direction == RULE_TYPE_OUTBOUND)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetNextInterface
 *------------------------------------------------------------------------------
 * PURPOSE  : Get next ifindex and direction
 * INPUT    : ifindex_p, direction_p
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : Using ifindex = 0 to get first.
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_OM_LocalGetNextInterface(
    UI32_T *ifindex_p,
    RULE_TYPE_InOutDirection_T *direction_p)
{
    RULE_TYPE_InOutDirection_T max_direction;
    UI32_T pl_index;

    ASSERT(ifindex_p != NULL);
    ASSERT(direction_p != NULL);

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
    max_direction = RULE_TYPE_OUTBOUND;
#else
    max_direction = RULE_TYPE_INBOUND;
#endif /* SYS_CPNT_QOS_V2_EGRESS_PORT */

    if (*ifindex_p == 0)
    {
        pl_index = 0;
        *direction_p = RULE_TYPE_INBOUND;
    }
    else
    {
        pl_index = RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(*ifindex_p);

        *direction_p = (*direction_p == RULE_TYPE_UNSPEC_DIRECTION)
                       ? RULE_TYPE_INBOUND : (*direction_p + 1);
    }

    for (; pl_index < RULE_TYPE_TOTAL_NBR_OF_PORT; ++pl_index)
    {
        for (; *direction_p <= max_direction; ++(*direction_p))
        {
            *ifindex_p = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_index);
            if (TRUE == RULE_OM_LocalIsValidIfindex(*ifindex_p, *direction_p))
            {
                return TRUE;
            }
        }

        *direction_p = RULE_TYPE_INBOUND;
    }

    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetPortEntryByIfIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get port entry by ifindex
 * INPUT    : ifindex
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_PortEntry_T*
RULE_OM_LocalGetPortEntryByIfIndex(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile)
{
    UI32_T ar_index = RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex);

    if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, inout_profile))
    {
        return NULL;
    }

    return (inout_profile ==  RULE_TYPE_INBOUND) ?
        &shmem_data_p->rule_mgr_port_table[ar_index] :
        &shmem_data_p->egress_port_table[ar_index];
}

static RULE_TYPE_ACL_MEMENTO_PTR_T
RULE_OM_LocalGetPortAclByType(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_AclType_T acl_type)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_PortEntry_T *port_entry_p;

    RULE_TYPE_AclType_T port_acl_type;

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(ifindex, direction);
    if (NULL == port_entry_p)
    {
        return NULL;
    }

    result = RULE_OM_LocalAclTypeToPortAclType(acl_type, &port_acl_type);
    if (RULE_TYPE_OK != result)
    {
        return NULL;
    }

    if (TRUE != RULE_OM_IS_ENTRY_ACTIVE(port_entry_p->bind_acl[port_acl_type].row_status))
    {
        return NULL;
    }

    if (0 == port_entry_p->bind_acl[port_acl_type].acl_index)
    {
        return NULL;
    }

    return &port_entry_p->bind_acl[port_acl_type];
}

static RULE_TYPE_ACL_MEMENTO_PTR_T
RULE_OM_LocalGetPortAclByPrecedence(
    UI32_T ifindex,
    UI32_T precedence)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_InOutDirection_T direction;
    RULE_TYPE_AclType_T port_acl_type;

    result = RULE_OM_LocalPrecedenceToDirection(precedence, &direction);
    if (RULE_TYPE_OK != result)
    {
        return NULL;
    }

    result = RULE_OM_LocalPrecedenceToPortAclType(precedence, &port_acl_type);
    if (RULE_TYPE_OK != result)
    {
        return NULL;
    }

    return RULE_OM_LocalGetPortAclByType(ifindex, direction, port_acl_type);
}

static RULE_TYPE_ACL_MEMENTO_PTR_T
RULE_OM_LocalGetNextPortAclByPrecedence(
    UI32_T ifindex,
    UI32_T *precedence_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    ASSERT(NULL != precedence_p);

    for (++(*precedence_p); *precedence_p <= RULE_TYPE_PRECEDENCE_LAST; ++ (*precedence_p))
    {
        RULE_TYPE_AclType_T port_acl_type;

        result = RULE_OM_LocalPrecedenceToPortAclType(*precedence_p, &port_acl_type);
        if (RULE_TYPE_OK == result)
        {
            RULE_TYPE_InOutDirection_T direction;
            RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

            result = RULE_OM_LocalPrecedenceToDirection(*precedence_p, &direction);
            ASSERT(RULE_TYPE_OK == result);

            bind_acl_p = RULE_OM_LocalGetPortAclByType(ifindex, direction, port_acl_type);
            if (NULL != bind_acl_p)
            {
                return bind_acl_p;
            }
        }
    }

    return NULL;
}

static RULE_TYPE_ACL_MEMENTO_PTR_T
RULE_OM_LocalGetNextPortAclByDirectionAndPrecedence(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T *precedence_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    ASSERT(NULL != precedence_p);

    if (RULE_TYPE_INBOUND != direction &&
        RULE_TYPE_OUTBOUND != direction)
    {
        return NULL;
    }

    for (++(*precedence_p); *precedence_p <= RULE_TYPE_PRECEDENCE_LAST; ++ (*precedence_p))
    {
        RULE_TYPE_InOutDirection_T dir;

        result = RULE_OM_LocalPrecedenceToDirection(*precedence_p, &dir);
        if (RULE_TYPE_OK == result)
        {
            RULE_TYPE_AclType_T port_acl_type;
            RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

            if (direction != dir)
            {
                continue;
            }

            result = RULE_OM_LocalPrecedenceToPortAclType(*precedence_p, &port_acl_type);
            ASSERT(RULE_TYPE_OK == result);

            bind_acl_p = RULE_OM_LocalGetPortAclByType(ifindex, direction, port_acl_type);
            if (NULL != bind_acl_p)
            {
                return bind_acl_p;
            }
        }
    }

    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetPolicyMapElementPrecedence
 *------------------------------------------------------------------------------
 * PURPOSE  : get class-map precedence in policy-map
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : precedence
 * RETURN   : error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalGetPolicyMapElementPrecedence(UI32_T policy_map_index, UI32_T class_map_index, UI32_T *precedence)
{
    UI32_T                      index;
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    RULE_TYPE_PolicyElement_T   *policy_element_p;

    if (NULL == precedence)
        return RULE_TYPE_FAIL;

    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
        return RULE_TYPE_FAIL;

    for (index = 0; index < policy_map_p->element_count; ++index)
    {
        policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_map_p->policy_map_element_index_list[index]);
        if (NULL == policy_element_p)
            continue; /* should not go here */

        if (policy_element_p->class_map_index != class_map_index)
            continue;

        *precedence = index + 1; /* to 1-based index */
        return RULE_TYPE_OK;
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetClassMapElementPrecedence
 *------------------------------------------------------------------------------
 * PURPOSE  : get class precedence in class-map
 * INPUT    : class_map_index, class_type, class_index
 * OUTPUT   : precedence
 * RETURN   : error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalGetClassMapElementPrecedence(UI32_T class_map_index, RULE_TYPE_AclType_T class_type, UI32_T class_index, UI32_T *precedence)
{
    UI32_T                      index;
    RULE_TYPE_ClassMap_T       *class_map_p;

    if (NULL == precedence)
        return RULE_TYPE_FAIL;

    class_map_p = RULE_OM_LocalGetClassMapByIndex(class_map_index);
    if (NULL == class_map_p)
        return RULE_TYPE_FAIL;

    for (index = 0; index < class_map_p->class_element_count; ++index)
    {
        if ((class_type != class_map_p->class_type[index]) ||
            (class_index != class_map_p->class_index_list[index]))
        {
            continue;
        }

        *precedence = index + 1; /* to 1-based index */
        return RULE_TYPE_OK;
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalIsValidElementTypeOflassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : Check class-map element type is allowed to add into
 *            class-map
 * INPUT    : acl_index, ace_index
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_OM_LocalIsValidElementTypeOflassMap(
    RULE_TYPE_ClassMap_T *class_map_p,
    RULE_TYPE_AclType_T element_type)
{
/*
 * JunYing: We SHOULD NOT check this issue here.
 * It is knowledge for HW. But OM should be HW independent
 */
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MIXED_MODE != TRUE)
    RULE_TYPE_AclType_T class_map_type;
    RULE_TYPE_RETURN_TYPE_T result;

    result = RULE_OM_LocalClassifyClassMap(class_map_p, &class_map_type);
    if (RULE_TYPE_OK == result)
    {
        if (element_type != class_map_type)
        {
            return FALSE;
        }
    }
#endif /* (SYS_CPNT_QOS_V2_CLASS_MAP_MIXED_MODE != TRUE) */

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalIsAclHaveAce
 *------------------------------------------------------------------------------
 * PURPOSE  : Check ACL have own the specified ACE
 * INPUT    : acl_index, ace_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalIsAclHaveAce(
    RULE_TYPE_Acl_T *acl_p,
    UI32_T ace_index)
{
    RULE_TYPE_INSTANCE_PTR_T    ace_inst_p;

    UI32_T                      in_acl_ace_index;

    ASSERT(NULL != acl_p);

    for (ace_inst_p = RULE_OM_PTR2INST(acl_p->super.links.first_node);
         NULL != ace_inst_p;
         ace_inst_p = RULE_OM_PTR2INST(ace_inst_p->links.next))
    {
        in_acl_ace_index = RULE_OM_INDEX_OF(ace_inst_p) + 1;

        if (in_acl_ace_index == ace_index)
        {
            return RULE_TYPE_OK;
        }
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalValidateAce
 *------------------------------------------------------------------------------
 * PURPOSE  : check ace parameters
 * INPUT    : ace_p
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalValidateAce(const RULE_TYPE_Ace_Entry_T *ace_p)
{
    if (NULL == ace_p)
        return RULE_TYPE_FAIL;

    switch (ace_p->ace_type)
    {
        case RULE_TYPE_MAC_ACL:
        return RULE_OM_LocalValidateMacAce(ace_p);

        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
        return RULE_OM_LocalValidateIpAce(ace_p);

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
        return RULE_OM_LocalValidateIpv6Ace(ace_p);

#if (SYS_CPNT_DAI == TRUE)
        case RULE_TYPE_ARP_ACL:
        return RULE_OM_LocalValidateArpAce(&ace_p->u.arp);
#endif
        case RULE_TYPE_MF:
        default:
            break; /* should not go here */
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalValidateIpAce
 *------------------------------------------------------------------------------
 * PURPOSE  : check ip ace parameters
 * INPUT    : ip_ace_p
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalValidateIpAce(const RULE_TYPE_Ace_Entry_T *ace_p)
{
#define FIELD(val)  {#val, val}

    struct
    {
        char *strval;
        UI32_T val;
    } fields[] =
        {
            FIELD(LEAF_diffServIpAceType),
            FIELD(LEAF_diffServIpAceAccess),
            FIELD(LEAF_diffServIpAceSourceIpAddr),
            FIELD(LEAF_diffServIpAceSourceIpAddrBitmask),
            FIELD(LEAF_diffServIpAceDestIpAddr),
            FIELD(LEAF_diffServIpAceDestIpAddrBitmask),
            FIELD(LEAF_diffServIpAceProtocol),
            FIELD(LEAF_diffServIpAcePrec),
            FIELD(LEAF_diffServIpAceTos),
            FIELD(LEAF_diffServIpAceDscp),
            FIELD(LEAF_diffServIpAceSourcePortOp),
            FIELD(LEAF_diffServIpAceMinSourcePort),
            FIELD(LEAF_diffServIpAceMaxSourcePort),
            FIELD(LEAF_diffServIpAceSourcePortBitmask),
            FIELD(LEAF_diffServIpAceDestPortOp),
            FIELD(LEAF_diffServIpAceMinDestPort),
            FIELD(LEAF_diffServIpAceMaxDestPort),
            FIELD(LEAF_diffServIpAceDestPortBitmask),
            FIELD(LEAF_diffServIpAceControlCode),
            FIELD(LEAF_diffServIpAceControlCodeBitmask),
        };
    UI32_T i;
    UI32_T res;
    RULE_TYPE_Ace_Entry_T dflt_ace_entry;
    const RULE_TYPE_IpAce_Entry_T *ip_ace_p = &ace_p->u.ip;
    RULE_TYPE_IpAce_Entry_T *dflt_ip_ace_p;

    for (i=0; i<sizeof(fields)/sizeof(*fields); ++i)
    {
        res = RULE_OM_LocalValidateIpAceFieldForACL(ip_ace_p, ace_p->ace_type, fields[i].val);
        if (RULE_TYPE_OK != res)
        {
            LOG("Field %s(%lu) failed", fields[i].strval, fields[i].val);
            return res;
        }
    }

    /* Check each ACE by high level
     */
    dflt_ace_entry.ace_type = ace_p->ace_type;

    RULE_OM_LocalSetAceEntryToDefaultValue(&dflt_ace_entry);

    dflt_ip_ace_p = &dflt_ace_entry.u.ip;

    if(ip_ace_p->aceSourcePortOp == VAL_diffServIpAceSourcePortOp_noOperator)
    {
        if ((ip_ace_p->aceMinSourcePort != dflt_ip_ace_p->aceMaxSourcePort) ||
            (ip_ace_p->aceSourcePortBitmask != dflt_ip_ace_p->aceSourcePortBitmask))
        {
            LOG("Inconsistent L4 source port fields");
            return RULE_TYPE_FAIL;
        }
    }

    if(ip_ace_p->aceDestPortOp == VAL_diffServIpAceDestPortOp_noOperator)
    {
        if((ip_ace_p->aceMinDestPort != dflt_ip_ace_p->aceMinDestPort) ||
            (ip_ace_p->aceDestPortBitmask != dflt_ip_ace_p->aceDestPortBitmask))
        {
            LOG("Inconsistent DSCP fields");
            return RULE_TYPE_FAIL;
        }
    }

    /* If the protocol is not TCP, the controlCode and controlCodeBitmask MUST be default value
     */
    if(ip_ace_p->aceProtocol != RULE_TYPE_ACL_TCP_PROTOCOL)
    {
        if(ip_ace_p->aceControlCodeBitmask != 0)
        {
            LOG("Inconsistent protocol fields");
            return RULE_TYPE_FAIL;
        }
    }

    return RULE_TYPE_OK;
#undef FIELD
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalValidateMacAce
 *------------------------------------------------------------------------------
 * PURPOSE  : check mac ace parameters
 * INPUT    : mac_ace_p
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalValidateMacAce(const RULE_TYPE_Ace_Entry_T *ace_p)
{
#define FIELD(val)  {#val, val}

    struct
    {
        char *strval;
        UI32_T val;
    } fields[] =
        {
            FIELD(LEAF_diffServMacAceAccess),
            FIELD(LEAF_diffServMacAcePktformat),
            FIELD(LEAF_diffServMacAceSourceMacAddr),
            FIELD(LEAF_diffServMacAceSourceMacAddrBitmask),
            FIELD(LEAF_diffServMacAceDestMacAddr),
            FIELD(LEAF_diffServMacAceDestMacAddr),
            FIELD(LEAF_diffServMacAceDestMacAddrBitmask),
            FIELD(LEAF_diffServMacAceVidOp),
            FIELD(LEAF_diffServMacAceMinVid),
            FIELD(LEAF_diffServMacAceVidBitmask),
            FIELD(LEAF_diffServMacAceMaxVid),
            FIELD(LEAF_diffServMacAceEtherTypeOp),
            FIELD(LEAF_diffServMacAceEtherTypeBitmask),
            FIELD(LEAF_diffServMacAceMinEtherType),
            FIELD(LEAF_diffServMacAceMaxEtherType),
            FIELD(LEAF_diffServMacAceCosOp),
            FIELD(LEAF_diffServMacAceCosBitmask),
            FIELD(LEAF_diffServMacAceMinCos),
            FIELD(LEAF_diffServMacAceMaxCos),
        };

    UI32_T i;
    UI32_T res;
    RULE_TYPE_Ace_Entry_T dflt_ace_entry;
    const RULE_TYPE_MacAce_Entry_T *mac_ace_p = &ace_p->u.mac;
    RULE_TYPE_MacAce_Entry_T *dflt_mac_ace_p;

    for (i = 0; i < sizeof(fields)/sizeof(*fields); ++i)
    {
        res = RULE_OM_LocalValidateMacAceFieldForACL(mac_ace_p, ace_p->ace_type, fields[i].val);
        if (RULE_TYPE_OK != res)
        {
            LOG("Field %s(%lu) failed", fields[i].strval, fields[i].val);
            return res;
        }
    }

    /* Check each ACE by high level
     */
    dflt_ace_entry.ace_type = ace_p->ace_type;

    RULE_OM_LocalSetAceEntryToDefaultValue(&dflt_ace_entry);

    dflt_mac_ace_p = &dflt_ace_entry.u.mac;

    switch (mac_ace_p->acePktformat)
    {
    case VAL_diffServMacAcePktformat_untagged802Dot3:
        /* 802.3: no eth
         */
        if ((mac_ace_p->aceEtherTypeOp != VAL_diffServMacAceEtherTypeOp_noOperator) ||
            (mac_ace_p->aceVidOp != VAL_diffServMacAceVidOp_noOperator) ||
            (mac_ace_p->aceCosOp != VAL_diffServMacAceCosOp_noOperator))
        {
            LOG("Cannot mix ether-type, VID, or CoS with untagged-802.3");
            return RULE_TYPE_FAIL;
        }
        break;

        /* untagged: no vid/ cos
         */
    case VAL_diffServMacAcePktformat_untagged_Eth2:
        if ((mac_ace_p->aceVidOp != VAL_diffServMacAceVidOp_noOperator) ||
            (mac_ace_p->aceCosOp != VAL_diffServMacAceCosOp_noOperator))
        {
            LOG("Cannot mix VID, or CoS with untagged-eth2");
            return RULE_TYPE_FAIL;
        }
        break;

    case VAL_diffServMacAcePktformat_tagggedEth2:
        break;

    case VAL_diffServMacAcePktformat_tagged802Dot3:
        /* 802.3: no eth
         */
        if (mac_ace_p->aceEtherTypeOp != VAL_diffServMacAceEtherTypeOp_noOperator)
        {
            LOG("Cannot mix ether-type with tagged-802.3");
            return RULE_TYPE_FAIL;
        }
        break;
    }

    if (mac_ace_p->aceVidOp == VAL_diffServMacAceVidOp_noOperator)
    {
        if ((mac_ace_p->aceMinVid != dflt_mac_ace_p->aceMinVid) ||
            (mac_ace_p->aceVidBitmask != dflt_mac_ace_p->aceVidBitmask))
        {
            LOG("Inconsistent VID fields");
            return RULE_TYPE_FAIL;
        }
    }

    if (mac_ace_p->aceEtherTypeOp == VAL_diffServMacAceEtherTypeOp_noOperator)
    {
        if ((mac_ace_p->aceMinEtherType != dflt_mac_ace_p->aceMinEtherType) ||
            (mac_ace_p->aceEtherTypeBitmask != dflt_mac_ace_p->aceEtherTypeBitmask))
        {
            LOG("Inconsistent ether-type fields");
            return RULE_TYPE_FAIL;
        }
    }

    if (mac_ace_p->aceCosOp == VAL_diffServMacAceCosOp_noOperator)
    {
        if ((mac_ace_p->aceMinCos != dflt_mac_ace_p->aceMinCos) ||
            (mac_ace_p->aceMaxCos != dflt_mac_ace_p->aceMaxCos))
        {
            LOG("Inconsistent CoS fields");
            return RULE_TYPE_FAIL;
        }
    }

    if (mac_ace_p->aceSourcePortOp == VAL_diffServMacAceSourcePortOp_noOperator)
    {
        if ((mac_ace_p->aceMinSourcePort != dflt_mac_ace_p->aceMinSourcePort) ||
            (mac_ace_p->aceMaxSourcePort != dflt_mac_ace_p->aceMaxSourcePort))
        {
            LOG("Inconsistent eth source port fields");
            return RULE_TYPE_FAIL;
        }
    }

    return RULE_TYPE_OK;
#undef FIELD
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalValidateIpv6Ace
 *------------------------------------------------------------------------------
 * PURPOSE  : check ipv6 ace parameters
 * INPUT    : ipv6_ace_p
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalValidateIpv6Ace(const RULE_TYPE_Ace_Entry_T *ace_p)
{
#define FIELD(val)  {#val, val}

    struct
    {
        char *strval;
        UI32_T val;
    } fields[] =
    {
        FIELD(LEAF_diffServIpv6AceSourceIpAddr),
        FIELD(LEAF_diffServIpv6AceSourceIpAddrPrefixLen),
        FIELD(LEAF_diffServIpv6AceDestIpAddr),
        FIELD(LEAF_diffServIpv6AceDestIpAddrPrefixLen),
        FIELD(LEAF_diffServIpv6AceNextHeader),
        FIELD(LEAF_diffServIpv6AceDscp),
        FIELD(LEAF_diffServIpv6AceFlowLabel),
        FIELD(LEAF_diffServIpv6AceSourcePortOp),
        FIELD(LEAF_diffServIpv6AceSourcePort),
        FIELD(LEAF_diffServIpv6AceSourcePortBitmask),
        FIELD(LEAF_diffServIpv6AceDestPortOp),
        FIELD(LEAF_diffServIpv6AceDestPort),
        FIELD(LEAF_diffServIpv6AceDestPortBitmask),
    };

    UI32_T i;
    UI32_T res;
    const RULE_TYPE_Ipv6Ace_Entry_T *ipv6_ace_p = &ace_p->u.ipv6;

    for (i=0; i<sizeof(fields)/sizeof(*fields); ++i)
    {
        res = RULE_OM_LocalValidateIpv6AceFieldForACL(ipv6_ace_p, ace_p->ace_type, fields[i].val);
        if (RULE_TYPE_OK != res)
        {
            LOG("Field %s(%lu) failed", fields[i].strval, fields[i].val);
            return res;
        }
    }

    /* currently no need to check */
    return RULE_TYPE_OK;

#undef FIELD
}

#if (SYS_CPNT_DAI == TRUE)
static UI32_T RULE_OM_LocalValidateArpAce(const RULE_TYPE_ArpAce_Entry_T *arp_ace)
{
    /* currently no need to check
     */
    return RULE_TYPE_OK;
}
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalCheckAceCollisionToAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : check whether an ace entry come into collision with the ace entris in acl
 * INPUT    : ace_entry, acl_entry
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------*/
static UI32_T RULE_OM_LocalCheckAceCollisionToAcl(const RULE_TYPE_Ace_Entry_T *ace_entry, const RULE_TYPE_Acl_T *acl_entry)
{
    if (NULL == ace_entry)
        return RULE_TYPE_FAIL;

    switch (ace_entry->ace_type)
    {
        case RULE_TYPE_MAC_ACL:
            return RULE_OM_LocalCheckMacAceCollisionToAcl(&ace_entry->u.mac, acl_entry);

        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
#if (SYS_CPNT_DAI == TRUE)
        case RULE_TYPE_ARP_ACL:
#endif
            return RULE_OM_LocalCheckIpAceCollisionToAcl(&ace_entry->u.ip, acl_entry);

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            return RULE_OM_LocalCheckIpv6AceCollisionToAcl(&ace_entry->u.ipv6, acl_entry);

        case RULE_TYPE_MF:
        default:
            break; /* should not go here */
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalCheckIpAceCollisionToAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : check whether an ip ace entry come into collision with the ace entris in acl
 * INPUT    : ip_ace, acl_entry
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : OK -- mean pass, FAIL -- mean will cause collision
 *------------------------------------------------------------------------------*/
static UI32_T RULE_OM_LocalCheckIpAceCollisionToAcl(const RULE_TYPE_IpAce_Entry_T *ip_ace, const RULE_TYPE_Acl_T *acl_entry)
{
    /* currently no need to check */
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalCheckMacAceCollisionToAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : check whether an mac ace entry come into collision with the ace entris in acl
 * INPUT    : mac_ace, acl_entry
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : OK -- mean pass, FAIL -- mean will cause collision
 *------------------------------------------------------------------------------*/
static UI32_T RULE_OM_LocalCheckMacAceCollisionToAcl(const RULE_TYPE_MacAce_Entry_T *mac_ace, const RULE_TYPE_Acl_T *acl_entry)
{
    /* currently no need to check */
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalCheckIpv6AceCollisionToAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : check whether an ipv6 ace entry come into collision with the ace entris in acl
 * INPUT    : ipv6_ace, acl_entry
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : OK -- mean pass, FAIL -- mean will cause collision
 *------------------------------------------------------------------------------*/
static UI32_T RULE_OM_LocalCheckIpv6AceCollisionToAcl(const RULE_TYPE_Ipv6Ace_Entry_T *ipv6_ace, const RULE_TYPE_Acl_T *acl_entry)
{
    /* currently no need to check */
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalCheckActionCollision
 *------------------------------------------------------------------------------
 * PURPOSE  : check whether the action bitmap is valid or not
 * INPUT    : action_bitmap
 * OUTPUT   : none
 * RETURN   : OK -- valid / FAIL -- occur collision
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalCheckActionCollision(UI32_T action_bitmap)
{
#define SUPPORTED_ACTION    \
    (RULE_TYPE_IN_PROFILE_ACTION | RULE_TYPE_OUT_OF_PROFILE_ACTION)

    UI8_T   cnt;

#if (SYS_CPNT_QOS_DIFFSERV_CLASS_SET_ACTION_COS & SYS_CPNT_QOS_DIFFSERV_CLASS_SET_ACTION_MODE)
    BOOL_T  have_set_cos = TRUE;
#else
    BOOL_T  have_set_cos = FALSE;
#endif

#if (SYS_CPNT_QOS_DIFFSERV_CLASS_SET_ACTION_PHB & SYS_CPNT_QOS_DIFFSERV_CLASS_SET_ACTION_MODE)
    BOOL_T  have_set_phb = TRUE;
#else
    BOOL_T  have_set_phb = FALSE;
#endif

#if (SYS_CPNT_QOS_DIFFSERV_CLASS_SET_ACTION_IP_DSCP & SYS_CPNT_QOS_DIFFSERV_CLASS_SET_ACTION_MODE)
    BOOL_T  have_set_ip_dscp = TRUE;
#else
    BOOL_T  have_set_ip_dscp = FALSE;
#endif

#if (SYS_CPNT_QOS_DIFFSERV_CLASS_SET_ACTION_IP_PRECEDENCE & SYS_CPNT_QOS_DIFFSERV_CLASS_SET_ACTION_MODE)
    BOOL_T  have_set_ip_prec = TRUE;
#else
    BOOL_T  have_set_ip_prec = FALSE;
#endif


    /* check bitmap by SYS_CPNT
     */
    if ((FALSE == have_set_cos) &&
        (RULE_TYPE_ACTION_PKT_NEW_PRI & action_bitmap))
    {
        LOG("no support NewPri bit");
        return RULE_TYPE_FAIL;
    }

    if ((FALSE == have_set_phb) &&
        (RULE_TYPE_ACTION_PKT_NEW_PHB & action_bitmap))
    {
        LOG("no support NewPHB bit");
        return RULE_TYPE_FAIL;
    }

    if ((FALSE == have_set_ip_dscp) &&
        (RULE_TYPE_ACTION_PKT_NEW_DSCP & action_bitmap))
    {
        LOG("no support NewDSCP bit");
        return RULE_TYPE_FAIL;
    }

    if ((FALSE == have_set_ip_prec) &&
        (RULE_TYPE_ACTION_PKT_NEW_TOS & action_bitmap))
    {
        LOG("no support NewTOS bit");
        return RULE_TYPE_FAIL;
    }

#ifdef SYS_CPNT_QOS_DIFFSERV_CONFORM_ACTION_MODIFY_DSCP
    if ((FALSE == SYS_CPNT_QOS_DIFFSERV_CONFORM_ACTION_MODIFY_DSCP) &&
        (RULE_TYPE_ACTION_GREEN_PKT_NEW_DSCP & action_bitmap))
    {
        LOG("no support GpNewDSCP bit");
        return RULE_TYPE_FAIL;
    }
#endif

    cnt = RULE_OM_LocalCountBitsSet(action_bitmap & RULE_TYPE_IN_PROFILE_ACTION);

#if 0
    /* pkt_new_pri, pkt_new_tos, pkt_new_dscp are conflict */
    cnt = 0;
    if (RULE_TYPE_ACTION_PKT_NEW_PRI & action_bitmap)
        ++cnt;

    if (RULE_TYPE_ACTION_PKT_NEW_PHB & action_bitmap)
        ++cnt;

    if (RULE_TYPE_ACTION_PKT_NEW_TOS & action_bitmap)
        ++cnt;

    if (RULE_TYPE_ACTION_PKT_NEW_DSCP & action_bitmap)
        ++cnt;
#endif

    if (1 < cnt)
    {
        LOG("in profile action(0x%08lx) conflicted", action_bitmap & RULE_TYPE_IN_PROFILE_ACTION);
        return RULE_TYPE_FAIL;
    }

    cnt = RULE_OM_LocalCountBitsSet(action_bitmap & RULE_TYPE_GREEN_ACTION);
    if (1 < cnt)
    {
        LOG("green action(0x%08lx) conflicted", action_bitmap & RULE_TYPE_GREEN_ACTION);
        return RULE_TYPE_FAIL;
    }

    /* yellow_drop and yellow_pkt_new_dscp are conflict */
    if ((RULE_TYPE_ACTION_YELLOW_DROP & action_bitmap) &&
        (RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP & action_bitmap))
    {
        LOG("yellow action(0x%08lx) conflicted", action_bitmap & RULE_TYPE_YELLOW_ACTION);
        return RULE_TYPE_FAIL;
    }

    /* red_drop and red_pkt_new_dscp are conflict */
    if ((RULE_TYPE_ACTION_RED_DROP & action_bitmap) &&
        (RULE_TYPE_ACTION_RED_PKT_NEW_DSCP & action_bitmap))
    {
        LOG("green action(0x%08lx) conflicted", action_bitmap & RULE_TYPE_RED_ACTION);
        return RULE_TYPE_FAIL;
    }

    action_bitmap &= ~(SUPPORTED_ACTION);
    if (0 < action_bitmap)
    {
        /* action_bitmap contains some unsupported actions */
        LOG("action(0x%08lx) contains unsupported bit", action_bitmap & SUPPORTED_ACTION);
        return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;

#undef SUPPORTED_ACTION
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalValidateMeter
 *------------------------------------------------------------------------------
 * PURPOSE  : check meter parameters
 * INPUT    : meter_entry
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL / INVALID_PARAMETER)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalValidateMeter(const RULE_TYPE_TBParamEntry_T *meter_entry)
{
    if (NULL == meter_entry)
        return RULE_TYPE_INVALID_PARAMETER;

    switch (meter_entry->meter_model)
    {
        case RULE_TYPE_METER_MODE_DEFAULT:
            break;

        case RULE_TYPE_METER_MODE_FLOW:
            if ((SYS_ADPT_DIFFSERV_MIN_POLICE_RATE > meter_entry->rate) ||
                (SYS_ADPT_DIFFSERV_MAX_POLICE_RATE < meter_entry->rate) ||
                (SYS_ADPT_DIFFSERV_MIN_POLICE_BURST > meter_entry->burst_size) ||
                (SYS_ADPT_DIFFSERV_MAX_POLICE_BURST < meter_entry->burst_size))
            {
                return RULE_TYPE_INVALID_PARAMETER;
            }
            break;

        case RULE_TYPE_METER_MODE_SRTCM_COLOR_AWARE:
        case RULE_TYPE_METER_MODE_SRTCM_COLOR_BLIND:
            if ((SYS_ADPT_DIFFSERV_MIN_POLICE_RATE > meter_entry->rate) ||
                (SYS_ADPT_DIFFSERV_MAX_POLICE_RATE < meter_entry->rate)||
                (SYS_ADPT_DIFFSERV_MIN_POLICE_BURST > meter_entry->burst_size) ||
                (SYS_ADPT_DIFFSERV_MAX_POLICE_BURST < meter_entry->burst_size) ||
                (SYS_ADPT_DIFFSERV_MIN_POLICE_BURST > meter_entry->peak_burst_size) ||
                (SYS_ADPT_DIFFSERV_MAX_POLICE_BURST < meter_entry->peak_burst_size))
            {
                return RULE_TYPE_INVALID_PARAMETER;
            }
            break;
        case RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE:
        case RULE_TYPE_METER_MODE_TRTCM_COLOR_BLIND:

            if ((SYS_ADPT_DIFFSERV_MIN_POLICE_RATE > meter_entry->rate) ||
                (SYS_ADPT_DIFFSERV_MAX_POLICE_RATE < meter_entry->rate) ||
                (SYS_ADPT_DIFFSERV_MIN_POLICE_BURST > meter_entry->burst_size) ||
                (SYS_ADPT_DIFFSERV_MAX_POLICE_BURST < meter_entry->burst_size) ||
                (SYS_ADPT_DIFFSERV_MIN_POLICE_RATE > meter_entry->peak_rate) ||
                (SYS_ADPT_DIFFSERV_MAX_POLICE_RATE < meter_entry->peak_rate) ||
                (SYS_ADPT_DIFFSERV_MIN_POLICE_BURST > meter_entry->peak_burst_size) ||
                (SYS_ADPT_DIFFSERV_MAX_POLICE_BURST < meter_entry->peak_burst_size))
            {
                return RULE_TYPE_INVALID_PARAMETER;
            }
            break;

        case RULE_TYPE_METER_MODE_RATE:
            if ((SYS_ADPT_DIFFSERV_MIN_POLICE_RATE > meter_entry->rate) ||
                (SYS_ADPT_DIFFSERV_MAX_POLICE_RATE < meter_entry->rate))
            {
                return RULE_TYPE_INVALID_PARAMETER;
            }
            break;

        default:
            return RULE_TYPE_FAIL;
    }

    /* dont care interval and row_status */

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalExtraValidateMeter
 *------------------------------------------------------------------------------
 * PURPOSE  : check meter parameters
 * INPUT    : policy_map_index  - policy-map index
 *            class_map_index   - class-map index
 *            meter_entry       - meter entry
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalExtraValidateMeter(
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T meter_index,
    const RULE_TYPE_TBParamEntry_T *meter_entry)
{
    UI32_T ret = RULE_TYPE_OK;

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    if (RULE_TYPE_OK == RULE_OM_LocalIsClassMapInBundleGroup(policy_map_index, class_map_index))
    {
        if (VAL_diffServMeterModel_flow == meter_entry->meter_model)
        {
            ret = RULE_TYPE_OK;
        }
        else
        {
            ret = RULE_TYPE_E_INVALID_METER_MODE;
        }
    }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

    return ret;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalValidateAction
 *------------------------------------------------------------------------------
 * PURPOSE  : check action parameters
 * INPUT    : action_entry
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalValidateAction(const RULE_TYPE_Action_T *action_entry)
{
    if (NULL == action_entry)
        return RULE_TYPE_FAIL;

#if !(SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    if (action_entry->action_bitmap == 0)
        return RULE_TYPE_FAIL;
#endif /* !(SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE) */

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_PRI)
    {
        if ((MIN_diffServActionPktNewPri > action_entry->pkt_new_pri) ||
            (MAX_diffServActionPktNewPri < action_entry->pkt_new_pri))
            return RULE_TYPE_FAIL;
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_PHB)
    {
        if ((MIN_diffServActionPktNewPhb > action_entry->pkt_new_phb) ||
            (MAX_diffServActionPktNewPhb < action_entry->pkt_new_phb))
            return RULE_TYPE_FAIL;
    }
#if 0
    if (action_entry->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_TOS)
    {
        if ((MIN_diffServActionPktNewIpPrec > action_entry->pkt_new_tos) ||
            (MAX_diffServActionPktNewIpPrec < action_entry->pkt_new_tos))
            return RULE_TYPE_FAIL;
    }
#endif
    if (action_entry->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_DSCP)
    {
        if ((MIN_diffServActionPktNewDscp > action_entry->pkt_new_dscp) ||
            (MAX_diffServActionPktNewDscp < action_entry->pkt_new_dscp))
            return RULE_TYPE_FAIL;
    }


    if (action_entry->action_bitmap & RULE_TYPE_ACTION_RED_PKT_NEW_DSCP)
    {
        if ((MIN_diffServActionRedPktNewDscp > action_entry->red_pkt_new_dscp) ||
            (MAX_diffServActionRedPktNewDscp < action_entry->red_pkt_new_dscp))
            return RULE_TYPE_FAIL;
    }

    if (action_entry->action_bitmap & RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP)
    {
        if ((MIN_diffServActionRedPktNewDscp > action_entry->yellow_pkt_new_dscp) ||
            (MAX_diffServActionRedPktNewDscp < action_entry->yellow_pkt_new_dscp))
            return RULE_TYPE_FAIL;
    }
    /* dont care yellow_pkt_new_dscp, drop_precedence, red_drop_precedence, yellow_drop_precedence,
     * and row_status
     */

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalExtraValidateAction
 *------------------------------------------------------------------------------
 * PURPOSE  : check action parameters
 * INPUT    : policy_map_index  - policy-map index
 *            class_map_index   - class-map index
 *            action_index      - action index
 *            action_entry      - action entry
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalExtraValidateAction(
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T action_index,
    const RULE_TYPE_UI_Action_T *action_entry)
{
    UI32_T ret = RULE_TYPE_OK;

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    if (RULE_TYPE_OK == RULE_OM_LocalIsClassMapInBundleGroup(policy_map_index, class_map_index))
    {
        if (VIOLATE_ACTION_INVALID != action_entry->violate_action_type)
        {
            if (VIOLATE_ACTION_DROP == action_entry->violate_action_type)
            {
                ret = RULE_TYPE_OK;
            }
            else
            {
                ret = RULE_TYPE_E_INVALID_ACTION;
            }
        }
    }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

    return ret;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalValidateAceField
 *------------------------------------------------------------------------------
 * PURPOSE  : check ace parameters
 * INPUT    : ace_entry, owner
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalValidateAceField(const RULE_TYPE_Ace_Entry_T *ace_entry, UI32_T field_index, UI32_T owner)
{
    if (NULL == ace_entry)
        return RULE_TYPE_FAIL;

    if (RULE_TYPE_OK != RULE_OM_LocalValidateAceAccessField(ace_entry, field_index))
    {
        return RULE_TYPE_FAIL;
    }

    if (RULE_TYPE_ACE_OWNED_BY_CLASS_MAP == owner)
    {
        switch (ace_entry->ace_type)
        {
            case RULE_TYPE_MAC_ACL:
                return RULE_OM_LocalValidateMacAceFieldForMF(&ace_entry->u.mac, field_index);

            case RULE_TYPE_IP_STD_ACL:
            case RULE_TYPE_IP_EXT_ACL:
                return RULE_OM_LocalValidateIpAceFieldForMF(&ace_entry->u.ip, field_index);

            case RULE_TYPE_IPV6_ACL:
            case RULE_TYPE_IPV6_EXT_ACL:
                return RULE_OM_LocalValidateIpv6AceFieldForMF(&ace_entry->u.ipv6, field_index);

            default:
                break; /* should not go here */
        }
    }
    else
    {
        switch (ace_entry->ace_type)
        {
            case RULE_TYPE_MAC_ACL:
                {
                    RULE_TYPE_RETURN_TYPE_T result;

                    result = RULE_OM_LocalValidateMacAceFieldForACL(
                                 &ace_entry->u.mac, ace_entry->ace_type, field_index);
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
                    if (result != RULE_TYPE_OK)
                    {
                        result = RULE_OM_LocalValidateMacAceMixedFieldForACL(
                                     ace_entry, ace_entry->ace_type, field_index);
                    }
#endif /* SYS_CPNT_ACL_MAC_IP_MIX */
                    return result;

                }
                break;

            case RULE_TYPE_IP_STD_ACL:
            case RULE_TYPE_IP_EXT_ACL:
                return RULE_OM_LocalValidateIpAceFieldForACL(&ace_entry->u.ip, ace_entry->ace_type, field_index);

            case RULE_TYPE_IPV6_STD_ACL:
            case RULE_TYPE_IPV6_EXT_ACL:
                return RULE_OM_LocalValidateIpv6AceFieldForACL(&ace_entry->u.ipv6, ace_entry->ace_type, field_index);

#if (SYS_CPNT_DAI == TRUE)
            case RULE_TYPE_ARP_ACL:
                return RULE_OM_LocalValidateArpAceFieldForACL(&ace_entry->u.arp, field_index);
#endif

            default:
                break; /* should not go here */
        }
    }
    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalValidateAceAccessField
 *------------------------------------------------------------------------------
 * PURPOSE  : Check ACE access field
 * INPUT    : ace_entry_p, field_index (which leaf)
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_OM_LocalValidateAceAccessField(
    const RULE_TYPE_Ace_Entry_T *ace_entry_p,
    UI32_T field_index)
{

    switch (ace_entry_p->ace_type)
    {
        case RULE_TYPE_MAC_ACL:
            if (field_index != LEAF_diffServMacAceAccess)
            {
                return RULE_TYPE_OK;
            }
            break;
        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            if (field_index != LEAF_diffServIpAceAccess)
            {
                return RULE_TYPE_OK;
            }
            break;
        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            if (field_index != LEAF_diffServIpv6AceAccess)
            {
                return RULE_TYPE_OK;
            }
            break;

#if (SYS_CPNT_DAI == TRUE)
        case RULE_TYPE_ARP_ACL:
            if (field_index != LEAF_diffServArpAceAction)
            {
                return RULE_TYPE_OK;
            }
            break;
#endif
        default:
            break;
    }

    switch (ace_entry_p->access)
    {
        case RULE_TYPE_ACE_PERMIT:
        case RULE_TYPE_ACE_DENY:
            break;
        default:
            return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalValidateIpAceFieldForACL
 *------------------------------------------------------------------------------
 * PURPOSE  : Check ip ace parameters for ACE owned by ACL
 * INPUT    : ace_entry, ace_type (EXT/STD), field_index (which leaf)
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalValidateIpAceFieldForACL(const RULE_TYPE_IpAce_Entry_T *ace_entry, RULE_TYPE_AclType_T ace_type, UI32_T field_index)
{
    RULE_TYPE_Ace_Entry_T dflt_ace_entry;
    RULE_TYPE_IpAce_Entry_T *dflt_ip_ace_entry = &dflt_ace_entry.u.ip;

    dflt_ace_entry.ace_type = ace_type;

    RULE_OM_LocalSetAceEntryToDefaultValue(&dflt_ace_entry);

    /*Note: ignore minimum value if minimum value is 0 since all field are unsigned. */
    switch (field_index)
    {
    case LEAF_diffServIpAceType:
        /* No necessary to check when change ace_type.
         */

    case LEAF_diffServIpAceSourceIpAddr:
    case LEAF_diffServIpAceSourceIpAddrBitmask:
        /* Both IP STD/EXT ACE can use this field.
         */
        break;

    case LEAF_diffServIpAceAccess:
        break;

    case LEAF_diffServIpAceDestIpAddr:
        if (ace_entry->aceDestIpAddr == dflt_ip_ace_entry->aceDestIpAddr)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IP_EXT_ACL)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServIpAceDestIpAddrBitmask:
        if (ace_entry->aceDestIpAddrBitmask == dflt_ip_ace_entry->aceDestIpAddrBitmask)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IP_EXT_ACL)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServIpAceProtocol:
        if (ace_entry->aceProtocol == dflt_ip_ace_entry->aceProtocol)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IP_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (ace_entry->aceProtocol > MAX_diffServIpAceProtocol)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServIpAcePrec:
        if (ace_entry->acePrec == dflt_ip_ace_entry->acePrec)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IP_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (ace_entry->acePrec > MAX_diffServIpAceProtocol)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServIpAceTos:
        if (ace_entry->aceTos == dflt_ip_ace_entry->aceTos)
        {
            break;
        }

#if (SYS_CPNT_ACL_IP_EXT_TOS == TRUE)
        if (ace_type != RULE_TYPE_IP_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (ace_entry->aceTos > MAX_diffServIpAceTos)
            return RULE_TYPE_FAIL;
#else
        return RULE_TYPE_FAIL;
#endif
        break;

    case LEAF_diffServIpAceDscp:
        if (ace_entry->aceDscp == dflt_ip_ace_entry->aceDscp)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IP_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (ace_entry->aceDscp > MAX_diffServIpAceDscp)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServIpAceSourcePortOp:
        if (ace_entry->aceSourcePortOp == dflt_ip_ace_entry->aceSourcePortOp)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IP_EXT_ACL)
            return RULE_TYPE_FAIL;

        switch (ace_entry->aceSourcePortOp)
        {
        case VAL_diffServIpAceSourcePortOp_noOperator:
        case VAL_diffServIpAceSourcePortOp_equal:
            break;

        case VAL_diffServIpAceSourcePortOp_range:
        default:
            return RULE_TYPE_FAIL;
        }
        break;

    case LEAF_diffServIpAceMinSourcePort:
        if (ace_entry->aceMinSourcePort == dflt_ip_ace_entry->aceMinSourcePort)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IP_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (ace_entry->aceMinSourcePort > MAX_diffServIpAceMinSourcePort)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServIpAceMaxSourcePort:
        if (ace_entry->aceMaxSourcePort == dflt_ip_ace_entry->aceMaxSourcePort)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IP_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (ace_entry->aceMaxSourcePort > MAX_diffServIpAceMaxSourcePort)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServIpAceSourcePortBitmask:
        if (ace_entry->aceSourcePortBitmask == dflt_ip_ace_entry->aceSourcePortBitmask)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IP_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (ace_entry->aceSourcePortBitmask > MAX_diffServIpAceSourcePortBitmask)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServIpAceDestPortOp:
        if (ace_entry->aceDestPortOp == dflt_ip_ace_entry->aceDestPortOp)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IP_EXT_ACL)
            return RULE_TYPE_FAIL;

        switch (ace_entry->aceDestPortOp)
        {
        case VAL_diffServIpAceDestPortOp_noOperator:
        case VAL_diffServIpAceDestPortOp_equal:
            break;

        case VAL_diffServIpAceDestPortOp_range:
        default:
            return RULE_TYPE_FAIL;
        }
        break;

    case LEAF_diffServIpAceMinDestPort:
        if (ace_entry->aceMinDestPort == dflt_ip_ace_entry->aceMinDestPort)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IP_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (ace_entry->aceMinDestPort > MAX_diffServIpAceMinDestPort)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServIpAceMaxDestPort:
        if (ace_entry->aceMaxDestPort == dflt_ip_ace_entry->aceMaxDestPort)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IP_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (ace_entry->aceMaxDestPort > MAX_diffServIpAceMaxDestPort)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServIpAceDestPortBitmask:
        if (ace_entry->aceDestPortBitmask == dflt_ip_ace_entry->aceDestPortBitmask)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IP_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (ace_entry->aceDestPortBitmask > MAX_diffServIpAceDestPortBitmask)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServIpAceControlCode:
        if (ace_entry->aceControlCode == dflt_ip_ace_entry->aceControlCode)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IP_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (ace_entry->aceControlCode > MAX_diffServIpAceControlCode)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServIpAceControlCodeBitmask:
        if (ace_entry->aceControlCodeBitmask == dflt_ip_ace_entry->aceControlCodeBitmask)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IP_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (ace_entry->aceControlCodeBitmask > MAX_diffServIpAceControlCodeBitmask)
            return RULE_TYPE_FAIL;
        break;

    default:
        return FALSE;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalValidateMacAceFieldForACL
 *------------------------------------------------------------------------------
 * PURPOSE  : Check mac ace parameters for ACE owned by ACL
 * INPUT    : ace_entry, field_index (which leaf)
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalValidateMacAceFieldForACL(const RULE_TYPE_MacAce_Entry_T *ace_entry, RULE_TYPE_AclType_T ace_type, UI32_T field_index)
{
    RULE_TYPE_Ace_Entry_T dflt_ace_entry;
    RULE_TYPE_MacAce_Entry_T *dflt_mac_ace_entry = &dflt_ace_entry.u.mac;

    dflt_ace_entry.ace_type = ace_type;

    RULE_OM_LocalSetAceEntryToDefaultValue(&dflt_ace_entry);

    /*Note: ignore minimum value if minimum value is 0 since all field are unsigned. */
    switch (field_index)
    {
    case LEAF_diffServMacAceAccess:
        break;

    case LEAF_diffServMacAcePktformat:
        if (ace_entry->acePktformat == dflt_mac_ace_entry->acePktformat)
        {
            break;
        }

        switch (ace_entry->acePktformat)
        {
        case VAL_diffServMacAcePktformat_any:
        case VAL_diffServMacAcePktformat_untagged_Eth2:
        case VAL_diffServMacAcePktformat_untagged802Dot3:
        case VAL_diffServMacAcePktformat_tagggedEth2:
        case VAL_diffServMacAcePktformat_tagged802Dot3:
            break;

        default:
            return RULE_TYPE_FAIL;
        }
        break;

    case LEAF_diffServMacAceSourceMacAddr:
        break;

    case LEAF_diffServMacAceSourceMacAddrBitmask:
        break;

    case LEAF_diffServMacAceDestMacAddr:
        break;

    case LEAF_diffServMacAceDestMacAddrBitmask:
        break;

    case LEAF_diffServMacAceVidOp:
        if (ace_entry->aceVidOp == dflt_mac_ace_entry->aceVidOp)
        {
            break;
        }

        switch (ace_entry->aceVidOp)
        {
        case VAL_diffServMacAceVidOp_noOperator:
        case VAL_diffServMacAceVidOp_equal:
            break;

        case VAL_diffServMacAceVidOp_range:
        default:
            return RULE_TYPE_FAIL;
        }
        break;

    case LEAF_diffServMacAceMinVid:
        if (ace_entry->aceMinVid == dflt_mac_ace_entry->aceMinVid)
        {
            break;
        }

        if ((ace_entry->aceMinVid < MIN_diffServMacAceMinVid) ||
            (ace_entry->aceMinVid > MAX_diffServMacAceMinVid))
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServMacAceVidBitmask:
        if (ace_entry->aceVidBitmask == dflt_mac_ace_entry->aceVidBitmask)
        {
            break;
        }

        if (ace_entry->aceVidBitmask > MAX_diffServMacAceVidBitmask)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServMacAceMaxVid:
        if (ace_entry->aceMaxVid == dflt_mac_ace_entry->aceMaxVid)
        {
            break;
        }

        if ((ace_entry->aceMaxVid < MIN_diffServMacAceMinVid) ||
            (ace_entry->aceMaxVid > MAX_diffServMacAceMinVid))
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServMacAceEtherTypeOp:
        if (ace_entry->aceEtherTypeOp == dflt_mac_ace_entry->aceEtherTypeOp)
        {
            break;
        }

        switch (ace_entry->aceEtherTypeOp)
        {
        case VAL_diffServMacAceEtherTypeOp_noOperator:
        case VAL_diffServMacAceEtherTypeOp_equal:
            break;

        case VAL_diffServMacAceEtherTypeOp_range:
        default:
            return RULE_TYPE_FAIL;
        }
        break;

    case LEAF_diffServMacAceEtherTypeBitmask:
        if (ace_entry->aceEtherTypeBitmask == dflt_mac_ace_entry->aceEtherTypeBitmask)
        {
            break;
        }

        if (ace_entry->aceEtherTypeBitmask > MAX_diffServMacAceEtherTypeBitmask)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServMacAceMinEtherType:
        if (ace_entry->aceMinEtherType == dflt_mac_ace_entry->aceMinEtherType)
        {
            break;
        }

        if ((ace_entry->aceMinEtherType < MIN_diffServMacAceMinEtherType) ||
            (ace_entry->aceMinEtherType > MAX_diffServMacAceMinEtherType))
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServMacAceMaxEtherType:
        if (ace_entry->aceMaxEtherType == dflt_mac_ace_entry->aceMaxEtherType)
        {
            break;
        }

        if ((ace_entry->aceMaxEtherType < MIN_diffServMacAceMinEtherType) ||
            (ace_entry->aceMaxEtherType > MAX_diffServMacAceMinEtherType))
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServMacAceCosOp:
        if (ace_entry->aceCosOp == dflt_mac_ace_entry->aceCosOp)
        {
            break;
        }

        switch (ace_entry->aceCosOp)
        {
        case VAL_diffServMacAceCosOp_noOperator:
        case VAL_diffServMacAceCosOp_equal:
            break;

        case VAL_diffServMacAceCosOp_range:
        default:
            return RULE_TYPE_FAIL;
        }
        break;

    case LEAF_diffServMacAceCosBitmask:
        if (ace_entry->aceCosBitmask == dflt_mac_ace_entry->aceCosBitmask)
        {
            break;
        }

        if (ace_entry->aceCosBitmask > MAX_diffServMacAceCosBitmask)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServMacAceMinCos:
        if (ace_entry->aceMinCos == dflt_mac_ace_entry->aceMaxCos)
        {
            break;
        }

        if ((ace_entry->aceMinCos < MIN_diffServMacAceMinCos) ||
            (ace_entry->aceMinCos > MAX_diffServMacAceMinCos))
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServMacAceMaxCos:
        if (ace_entry->aceMaxCos == dflt_mac_ace_entry->aceMaxCos)
        {
            break;
        }

        if ((ace_entry->aceMaxCos < MIN_diffServMacAceMaxCos) ||
            (ace_entry->aceMaxCos > MAX_diffServMacAceMaxCos))
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServMacAceStatus:
        break;

    default:
        return RULE_TYPE_FAIL;
        break;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalValidateMacAceMixedFieldForACL
 *------------------------------------------------------------------------------
 * PURPOSE  : Check mac ace parameters for ACE owned by ACL
 * INPUT    : ace_entry, field_index (which leaf)
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalValidateMacAceMixedFieldForACL(
    const RULE_TYPE_Ace_Entry_T *ace_entry_p,
    RULE_TYPE_AclType_T ace_type,
    UI32_T field_index)
{
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
    RULE_TYPE_Ace_Entry_T dflt_ace_entry;

    dflt_ace_entry.ace_type = ace_type;

    RULE_OM_LocalSetAceEntryToDefaultValue(&dflt_ace_entry);

    if (ace_entry_p->u.mac.aceEtherTypeOp == VAL_diffServMacAceEtherTypeOp_equal &&
        ace_entry_p->u.mac.aceEtherTypeBitmask == MAX_diffServMacAceEtherTypeBitmask)
    {
        switch (ace_entry_p->u.mac.aceMinEtherType)
        {
            case RULE_TYPE_ETHERTYPE_IP:
                switch (field_index)
                {
                    case LEAF_diffServMacAceSourceIpAddr:
                    case LEAF_diffServMacAceSourceIpAddrBitmask:
                    case LEAF_diffServMacAceDestIpAddr:
                    case LEAF_diffServMacAceDestIpAddrBitmask:
                        break;

                    case LEAF_diffServMacAceIpProtocol:
                        if (ace_entry_p->u.ip.aceProtocol >
                            MAX_diffServMacAceProtocol)
                        {
                            return RULE_TYPE_FAIL;
                        }
                        break;

                    case LEAF_diffServMacAceL4SourcePortOp:
                        if (ace_entry_p->u.ip.aceSourcePortOp > VAL_diffServMacAceSourcePortOp_equal)
                        {
                            return RULE_TYPE_FAIL;
                        }
                        break;

                    case LEAF_diffServMacAceL4SourcePort:
                        if (ace_entry_p->u.ip.aceMinSourcePort > MAX_diffServMacAceL4SourcePort)
                        {
                            return RULE_TYPE_FAIL;
                        }
                        break;
                    case LEAF_diffServMacAceL4SourcePortBitmask:
                        if (ace_entry_p->u.ip.aceMinSourcePort > MAX_diffServMacAceL4SourcePortBitmask)
                        {
                            return RULE_TYPE_FAIL;
                        }
                        break;
                    case LEAF_diffServMacAceL4DestPortOp:
                        if (ace_entry_p->u.ip.aceDestPortOp > VAL_diffServMacAceDestPortOp_equal)
                        {
                            return RULE_TYPE_FAIL;
                        }
                        break;

                    case LEAF_diffServMacAceL4DestPort:
                        if (ace_entry_p->u.ip.aceMinDestPort > MAX_diffServMacAceL4DestPort)
                        {
                            return RULE_TYPE_FAIL;
                        }
                        break;

                    case LEAF_diffServMacAceL4DestPortBitmask:
                        if (ace_entry_p->u.ip.aceDestPortBitmask > MAX_diffServMacAceL4DestPortBitmask)
                        {
                            return RULE_TYPE_FAIL;
                        }
                        break;

                    default:
                        return RULE_TYPE_FAIL;
                }
                break;

            case RULE_TYPE_ETHERTYPE_IPV6:
                switch (field_index)
                {
                    case LEAF_diffServMacAceSourceIpv6Addr:
                        break;

                    case LEAF_diffServMacAceSourceIpv6AddrPrefixLen:
                        {
                            UI8_T prefix_len;
                            RULE_TYPE_RETURN_TYPE_T result;

                            result = RULE_OM_LocalConvertIpv6Mask2Prefix(ace_entry_p->u.ipv6.aceSourceIpAddrBitmask,
                                                                         &prefix_len);
                            ASSERT(result == RULE_TYPE_OK);

                            if (MAX_diffServIpv6AceSourceIpAddrPrefixLen < prefix_len)
                            {
                                return RULE_TYPE_FAIL;
                            }

                        }
                        break;

                    case LEAF_diffServMacAceDestIpv6Addr:
                        break;

                    case LEAF_diffServMacAceDestIpv6AddrPrefixLen:
                        {
                            UI8_T prefix_len;
                            RULE_TYPE_RETURN_TYPE_T result;

                            result = RULE_OM_LocalConvertIpv6Mask2Prefix(ace_entry_p->u.ipv6.aceDestIpAddrBitmask,
                                                                         &prefix_len);
                            ASSERT(result == RULE_TYPE_OK);

                            if (SYS_ADPT_ACL_MAX_DST_IPV6_PREFIX_LEN < prefix_len)
                            {
                                return RULE_TYPE_FAIL;
                            }
                        }
                        break;

                    case LEAF_diffServMacAceIpProtocol:
                        if (ace_entry_p->u.ipv6.aceNextHeader >
                            MAX_diffServMacAceProtocol)
                        {
                            return RULE_TYPE_FAIL;
                        }
                        break;

                    case LEAF_diffServMacAceL4SourcePortOp:
                        if (ace_entry_p->u.ipv6.aceSourcePortOp > VAL_diffServMacAceSourcePortOp_equal)
                        {
                            return RULE_TYPE_FAIL;
                        }
                        break;

                    case LEAF_diffServMacAceL4SourcePort:
                        if (ace_entry_p->u.ipv6.aceMinSourcePort > MAX_diffServMacAceL4SourcePort)
                        {
                            return RULE_TYPE_FAIL;
                        }
                        break;
                    case LEAF_diffServMacAceL4SourcePortBitmask:
                        if (ace_entry_p->u.ipv6.aceMinSourcePort > MAX_diffServMacAceL4SourcePortBitmask)
                        {
                            return RULE_TYPE_FAIL;
                        }
                        break;
                    case LEAF_diffServMacAceL4DestPortOp:
                        if (ace_entry_p->u.ipv6.aceDestPortOp > VAL_diffServMacAceSourcePortOp_equal)
                        {
                            return RULE_TYPE_FAIL;
                        }
                        break;

                    case LEAF_diffServMacAceL4DestPort:
                        if (ace_entry_p->u.ipv6.aceMinDestPort > MAX_diffServMacAceL4DestPort)
                        {
                            return RULE_TYPE_FAIL;
                        }
                        break;

                    case LEAF_diffServMacAceL4DestPortBitmask:
                        if (ace_entry_p->u.ipv6.aceDestPortBitmask > MAX_diffServMacAceL4DestPortBitmask)
                        {
                            return RULE_TYPE_FAIL;
                        }
                        break;

                    default:
                        return RULE_TYPE_FAIL;
                }
                break;

            default:
                return RULE_TYPE_FAIL;
        }
    }
    else
    {
        return RULE_TYPE_FAIL;
    }

#endif /* SYS_CPNT_ACL_MAC_IP_MIX */
    return RULE_TYPE_OK;
}

#if (SYS_CPNT_DAI == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalValidateArpAceFieldForACL
 *------------------------------------------------------------------------------
 * PURPOSE  : Check arp ace parameters for ACE owned by ACL
 * INPUT    : ace_entry, field_index (which leaf)
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalValidateArpAceFieldForACL(const RULE_TYPE_ArpAce_Entry_T *ace_entry, UI32_T field_index)
{
    switch (field_index)
    {
    case LEAF_diffServArpAceAction:
        break;

    case LEAF_diffServArpAcePktType:
        switch (ace_entry -> aceArpType)
        {
        case VAL_diffServArpAcePktType_request:
        case VAL_diffServArpAcePktType_response:
        case VAL_diffServArpAcePktType_both:
            break;

        default:
            return RULE_TYPE_FAIL;
        }
        break;

    case LEAF_diffServArpAceSourceIpAddr:
        break;

    case LEAF_diffServArpAceSourceIpAddrBitmask:
        break;

    case LEAF_diffServArpAceDestIpAddr:
        break;

    case LEAF_diffServArpAceDestIpAddrBitmask:
        break;

    case LEAF_diffServArpAceSourceMacAddr:
        break;

    case LEAF_diffServArpAceSourceMacAddrBitmask:
        break;

    case LEAF_diffServArpAceDestMacAddr:
        break;

    case LEAF_diffServArpAceDestMacAddrBitmask:
        break;

    case LEAF_diffServArpAceLogStatus:
        break;

    case LEAF_diffServArpAceStatus:
        break;

    default:
        return FALSE;
        break;
    }

    return RULE_TYPE_OK;
}
#endif /* #if (SYS_CPNT_DAI == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalValidateIpv6AceFieldForACL
 *------------------------------------------------------------------------------
 * PURPOSE  : Check ipv6 ace parameters for ACE owned by ACL
 * INPUT    : ace_entry, ace_type (EXT/STD), field_index (which leaf)
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalValidateIpv6AceFieldForACL(const RULE_TYPE_Ipv6Ace_Entry_T *ace_entry, RULE_TYPE_AclType_T ace_type, UI32_T field_index)
{
    RULE_TYPE_Ace_Entry_T dflt_ace_entry;

    dflt_ace_entry.ace_type = ace_type;

    RULE_OM_LocalSetAceEntryToDefaultValue(&dflt_ace_entry);

    /*Note: ignore minimum value if minimum value is 0 since all field are unsigned. */
    switch (field_index)
    {
    /*
     * below fields can be configured on std/ext mode
     */

    case LEAF_diffServIpv6AceAccess:
        break;

    case LEAF_diffServIpv6AceSourceIpAddr:

        if (0 == memcmp(ace_entry->aceSourceIpAddr, dflt_ace_entry.u.ipv6.aceSourceIpAddr, sizeof(ace_entry->aceSourceIpAddr)))
        {
            break;
        }

#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
#else
        if (ace_type != RULE_TYPE_IPV6_STD_ACL)
            return RULE_TYPE_FAIL;
#endif /* SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR */
        break;

    case LEAF_diffServIpv6AceSourceIpAddrPrefixLen:
        {
            UI8_T prefix_len;
            RULE_TYPE_RETURN_TYPE_T result;

            result = RULE_OM_LocalConvertIpv6Mask2Prefix(ace_entry->aceSourceIpAddrBitmask,
                                                         &prefix_len);
            ASSERT(result == RULE_TYPE_OK);

            if (prefix_len == MIN_diffServIpv6AceSourceIpAddrPrefixLen)
            {
                break;
            }

#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
            if (MAX_diffServIpv6AceSourceIpAddrPrefixLen < prefix_len)
            {
                return RULE_TYPE_FAIL;
            }
#else
            if (ace_type != RULE_TYPE_IPV6_STD_ACL)
            {
                return RULE_TYPE_FAIL;
            }
#endif /* SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR */

        }

        break;

    /*
     * below fields can be configured on ext mode
     */

    case LEAF_diffServIpv6AceDestIpAddr:

        if (0 == memcmp(ace_entry->aceDestIpAddr, dflt_ace_entry.u.ipv6.aceDestIpAddr, sizeof(ace_entry->aceDestIpAddr)))
        {
            break;
        }

#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
        if (ace_type != RULE_TYPE_IPV6_EXT_ACL)
            return RULE_TYPE_FAIL;
#else
        return RULE_TYPE_FAIL;
#endif /* SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR */
        break;

    case LEAF_diffServIpv6AceDestIpAddrPrefixLen:
        {
            UI8_T prefix_len;
            RULE_TYPE_RETURN_TYPE_T result;

            result = RULE_OM_LocalConvertIpv6Mask2Prefix(ace_entry->aceDestIpAddrBitmask,
                                                         &prefix_len);
            ASSERT(result == RULE_TYPE_OK);

            if (prefix_len == MIN_diffServIpv6AceDestIpAddrPrefixLen)
            {
                break;
            }

#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
            if (ace_type != RULE_TYPE_IPV6_EXT_ACL)
            {
                return RULE_TYPE_FAIL;
            }

            if (SYS_ADPT_ACL_MAX_DST_IPV6_PREFIX_LEN < prefix_len )
            {
                return RULE_TYPE_FAIL;
            }
#else
            return RULE_TYPE_FAIL;
#endif /* SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR */
        }
        break;

    case LEAF_diffServIpv6AceNextHeader:

        if (ace_entry->aceNextHeader == dflt_ace_entry.u.ipv6.aceNextHeader)
        {
            break;
        }

#if (SYS_CPNT_ACL_IPV6_EXT_NEXT_HEADER == TRUE)
        if (ace_type != RULE_TYPE_IPV6_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (ace_entry->aceNextHeader > MAX_diffServIpv6AceNextHeader)
            return RULE_TYPE_FAIL;
#else
        return RULE_TYPE_FAIL;
#endif /* SYS_CPNT_ACL_IPV6_EXT_NEXT_HEADER */

        break;

    case LEAF_diffServIpv6AceDscp:

        if (ace_entry->aceDscp == dflt_ace_entry.u.ipv6.aceDscp)
        {
            break;
        }

#if (SYS_CPNT_ACL_IPV6_EXT_TRAFFIC_CLASS == TRUE)
        if (ace_type != RULE_TYPE_IPV6_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (ace_entry->aceDscp > MAX_diffServIpv6AceDscp)
            return RULE_TYPE_FAIL;
#else
        return RULE_TYPE_FAIL;
#endif /* SYS_CPNT_ACL_IPV6_EXT_TRAFFIC_CLASS */

        break;

    case LEAF_diffServIpv6AceFlowLabel:

        if (ace_entry->aceFlowLabel == dflt_ace_entry.u.ipv6.aceFlowLabel)
        {
            break;
        }

#if (SYS_CPNT_ACL_IPV6_EXT_FLOW_LABEL == TRUE)
        if (ace_type != RULE_TYPE_IPV6_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (ace_entry->aceFlowLabel > MAX_diffServIpv6AceFlowLabel)
            return RULE_TYPE_FAIL;
        break;
#else
        return RULE_TYPE_FAIL;
#endif /* SYS_CPNT_ACL_IPV6_EXT_FLOW_LABEL */

    case LEAF_diffServIpv6AceType:
        break;

    case LEAF_diffServIpv6AceSourcePortOp:
        if (ace_entry->aceSourcePortOp == dflt_ace_entry.u.ipv6.aceSourcePortOp)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IPV6_EXT_ACL)
            return RULE_TYPE_FAIL;

        switch (ace_entry->aceSourcePortOp)
        {
            case VAL_diffServIpv6AceSourcePortOp_noOperator:
            case VAL_diffServIpv6AceSourcePortOp_equal:
                break;
            default:
                return RULE_TYPE_FAIL;
        }
        break;

    case LEAF_diffServIpv6AceSourcePort:
        if (ace_entry->aceMinSourcePort == dflt_ace_entry.u.ipv6.aceMinSourcePort)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IPV6_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (MAX_diffServIpv6AceSourcePort < ace_entry->aceMinSourcePort)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServIpv6AceSourcePortBitmask:
        if (ace_entry->aceSourcePortBitmask == dflt_ace_entry.u.ipv6.aceSourcePortBitmask)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IPV6_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (MAX_diffServIpv6AceSourcePortBitmask < ace_entry->aceSourcePortBitmask)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServIpv6AceDestPortOp:
       if (ace_entry->aceDestPortOp == dflt_ace_entry.u.ipv6.aceDestPortOp)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IPV6_EXT_ACL)
            return RULE_TYPE_FAIL;

        switch (ace_entry->aceDestPortOp)
        {
            case VAL_diffServIpv6AceDestPortOp_noOperator:
            case VAL_diffServIpv6AceDestPortOp_equal:
                break;
            default:
                return RULE_TYPE_FAIL;
        }
        break;

    case LEAF_diffServIpv6AceDestPort:
        if (ace_entry->aceMinDestPort == dflt_ace_entry.u.ipv6.aceMinDestPort)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IPV6_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (MAX_diffServIpv6AceDestPort < ace_entry->aceMinDestPort)
            return RULE_TYPE_FAIL;
        break;

    case LEAF_diffServIpv6AceDestPortBitmask:
        if (ace_entry->aceDestPortBitmask == dflt_ace_entry.u.ipv6.aceDestPortBitmask)
        {
            break;
        }

        if (ace_type != RULE_TYPE_IPV6_EXT_ACL)
            return RULE_TYPE_FAIL;

        if (MAX_diffServIpv6AceDestPortBitmask < ace_entry->aceDestPortBitmask)
            return RULE_TYPE_FAIL;
        break;

    default:
            return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalValidateIpAceFieldForMF
 *------------------------------------------------------------------------------
 * PURPOSE  : Check ip ace parameters for ACE owned by CLASS-MAP
 * INPUT    : ace_entry, field_index (which leaf)
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalValidateIpAceFieldForMF(const RULE_TYPE_IpAce_Entry_T *ace_entry, UI32_T field_index)
{
    /*Note: ignore minimum value if minimum value is 0 since all field are unsigned. */
    switch (field_index)
    {
    /* need to improve for mutual exclusive condition */
    case LEAF_diffServIpAcePrec:
        if (ace_entry -> acePrec > MAX_diffServIpAcePrec)
            return RULE_TYPE_FAIL;

        break;

    case LEAF_diffServIpAceDscp:
        if (ace_entry -> aceDscp > MAX_diffServIpAceDscp)
            return RULE_TYPE_FAIL;

        break;

    default:
        return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalValidateMacAceForMF
 *------------------------------------------------------------------------------
 * PURPOSE  : Check mac ace parameters for ACE owned by CLASS-MAP
 * INPUT    : ace_entry, field_index (which leaf)
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalValidateMacAceFieldForMF(const RULE_TYPE_MacAce_Entry_T *ace_entry, UI32_T field_index)
{
    /*Note: ignore minimum value if minimum value is 0 since all field are unsigned. */
    switch (field_index)
    {
    case LEAF_diffServMacAceMinVid:
        if ((ace_entry -> aceMinVid < MIN_diffServMacAceMinVid) ||
            (ace_entry -> aceMinVid > MAX_diffServMacAceMinVid))
            return RULE_TYPE_FAIL;

        break;

    default:
        return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalValidateIpv6AceForMF
 *------------------------------------------------------------------------------
 * PURPOSE  : Check ipv6 ace parameters for ACE owned by CLASS-MAP
 * INPUT    : ace_entry, field_index (which leaf)
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalValidateIpv6AceFieldForMF(const RULE_TYPE_Ipv6Ace_Entry_T *ace_entry, UI32_T field_index)
{
    /*Note: ignore minimum value if minimum value is 0 since all field are unsigned. */
    switch (field_index)
    {
    case LEAF_diffServIpv6AceDscp:
        if (ace_entry -> aceDscp > MAX_diffServIpv6AceDscp)
            return RULE_TYPE_FAIL;

        break;

    default:
        return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalRemoveAndCompactPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE  : remove the element with specified index and move up the following elements
 * INPUT    : class_map, remove_index
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalRemoveAndCompactPolicyMapElement(RULE_TYPE_PolicyMap_T *policy_map, UI32_T remove_index)
{
    UI32_T      index;

    if (NULL == policy_map)
        return RULE_TYPE_FAIL;

    if ((0 >= remove_index) || (remove_index > policy_map->element_count))
        return RULE_TYPE_FAIL; /* bad index */

    /* compact element list
     * if element_entry is the last entry, will not step into this for-loop
     * (because the index of last entry == class_element_count)
     */
    for (index = remove_index; index < policy_map->element_count; ++index)
    {
        /* copy next element to previous element */
        policy_map->policy_map_element_index_list[index - 1] = policy_map->policy_map_element_index_list[index];
    }

    --(policy_map->element_count);

    /* ensure the last element is clear */
    policy_map->policy_map_element_index_list[policy_map->element_count] = 0;

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalRemoveAndCompactClassMapElement
 *------------------------------------------------------------------------------
 * PURPOSE  : remove the element with specified index and move up the following elements
 * INPUT    : class_map, remove_index
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalRemoveAndCompactClassMapElement(RULE_TYPE_ClassMap_T *class_map, UI32_T remove_index)
{
    UI32_T      index;

    if (NULL == class_map)
        return RULE_TYPE_FAIL;

    if ((0 >= remove_index) || (remove_index > class_map->class_element_count))
        return RULE_TYPE_FAIL; /* bad index */

    /* compact element list
     * if element_entry is the last entry, will not step into this for-loop
     * (because the index of last entry == class_element_count)
     */
    for (index = remove_index; index < class_map->class_element_count; ++index)
    {
        /* copy next element to previous element */
        class_map->class_type[index - 1] = class_map->class_type[index];
        class_map->class_index_list[index - 1] = class_map->class_index_list[index];
    }

    --(class_map->class_element_count);

    /* ensure the last element is clear */
    class_map->class_type[class_map->class_element_count] = 0;
    class_map->class_index_list[class_map->class_element_count] = 0;

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalClassifyUIAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : get class type according to acl
 * INPUT    : acl_entry
 * OUTPUT   : class_type
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : class_type could be MAC/IP/IPv6 because they use different selector
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalClassifyUIAcl(const RULE_TYPE_UI_AclEntry_T *acl_entry, RULE_TYPE_AclType_T *class_type)
{
    if ((NULL == acl_entry) || (NULL == class_type))
        return RULE_TYPE_FAIL;

    switch(acl_entry->acl_type)
    {
        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            *class_type = RULE_TYPE_IP_ACL;
            break;
        case RULE_TYPE_MAC_ACL:
            *class_type = RULE_TYPE_MAC_ACL;
            break;
        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            *class_type = RULE_TYPE_IPV6_ACL;
            break;
        case RULE_TYPE_MF:
        default:
            return RULE_TYPE_FAIL; /* should not go here */
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalClassifyUIMf
 *------------------------------------------------------------------------------
 * PURPOSE  : get class type according to mf
 * INPUT    : mf_entry
 * OUTPUT   : class_type
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : class_type could be MAC/IP/IPv6 because they use different selector
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalClassifyUIMf(const RULE_TYPE_MfEntry_T *mf_entry, RULE_TYPE_AclType_T *class_type)
{
    RULE_TYPE_Ace_Entry_T ace_entry;
    RULE_TYPE_RETURN_TYPE_T result;

    ASSERT(NULL != mf_entry && NULL != class_type);

    result = RULE_OM_LocalConvertMf2Ace(mf_entry, &ace_entry);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    return RULE_OM_LocalClassifyAce(&ace_entry, class_type);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalGetUIAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : translate class ID to UI ACL index
 * INPUT    : class_id
 * OUTPUT   : none
 * RETURN   : ACL index if successeed; else 0
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalGetUIAclIndex(UI32_T class_id)
{
#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) || (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
    RULE_TYPE_Acl_T *acl_p;

    acl_p = RULE_OM_LocalGetAclByIndex(class_id);
    if (NULL == acl_p)
    {
        return 0;
    }

    if (acl_p->flag_hide)
    {
        if (RULE_TYPE_CLASS_ACL == acl_p->owner.class_type)
        {
            return acl_p->owner.class_id;
        }
        else
        {
            return 0;
        }
    }
#endif

    return class_id;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalConvert2UIPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : translate port entry to UI port entry
 * INPUT    : src
 * OUTPUT   : dst
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalConvert2UIPortEntry(
    const RULE_TYPE_PortEntry_T* src,
    RULE_TYPE_UI_PortEntry_T* dst)
{
    if ((NULL == src) || (NULL == dst))
        return RULE_TYPE_FAIL;

    memset(dst, 0, sizeof(RULE_TYPE_UI_PortEntry_T));

    dst->ingress_mac_acl_index = src->bind_acl[RULE_TYPE_MAC_ACL].acl_index;
    dst->ingress_ip_acl_index = src->bind_acl[RULE_TYPE_IP_ACL].acl_index;
    dst->ingress_ipv6_acl_index = src->bind_acl[RULE_TYPE_IPV6_ACL].acl_index;

    dst->policy_map_index = src->policy_map_index;

    dst->bind_dynamic_acl        = FALSE;
    dst->bind_dynamic_policy_map = FALSE;

    dst->user_cfg_ingress_mac_acl_index = dst->ingress_mac_acl_index;
    dst->user_cfg_ingress_ip_acl_index  = dst->ingress_ip_acl_index;
    dst->user_cfg_ingress_ipv6_acl_index= dst->ingress_ipv6_acl_index;
    dst->user_cfg_policy_map_index      = dst->policy_map_index;

#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
    dst->bind_dynamic_acl        = src->bind_dynamic_acl;
    dst->bind_dynamic_policy_map = src->bind_dynamic_policy_map;

    if (dst->bind_dynamic_acl)
    {
        dst->user_cfg_ingress_mac_acl_index = src->acl_memento[RULE_TYPE_MAC_ACL].acl_index;
        dst->user_cfg_ingress_ip_acl_index  = src->acl_memento[RULE_TYPE_IP_ACL].acl_index;
        dst->user_cfg_ingress_ipv6_acl_index= src->acl_memento[RULE_TYPE_IPV6_ACL].acl_index;
    }

    if (dst->bind_dynamic_policy_map)
    {
        dst->user_cfg_policy_map_index  = src->backup_policy_map_index;
    }
#endif /* #if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE) */

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalConvert2UIPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE  : translate policy-map to UI policy-map
 * INPUT    : src
 * OUTPUT   : dst
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalConvert2UIPolicyMap(const RULE_TYPE_PolicyMap_T* src, RULE_TYPE_UI_PolicyMap_T* dst)
{
    if ((NULL == src) || (NULL == dst))
        return RULE_TYPE_FAIL;

    memcpy(dst->name, src->name, SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
    dst->name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0'; /* force to end a string */

    memcpy(dst->desc, src->desc, SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH);
    dst->desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH] = '\0';

    dst->row_status = src->row_status;

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    {
        UI32_T bundle_group_index;
        UI32_T ret;

        for (bundle_group_index = 0; bundle_group_index < _countof(src->bundle_group); ++bundle_group_index)
        {
            ret = RULE_OM_LocalConvert2UIBundleGroup(&src->bundle_group[bundle_group_index], bundle_group_index+1, &dst->bundle_group[bundle_group_index]);

            if (RULE_TYPE_OK != ret)
            {
                return ret;
            }
        }
    }
#endif /* #if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE) */

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalConvert2UIPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE  : translate policy-map element to UI policy-map element
 * INPUT    : src
 * OUTPUT   : dst
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalConvert2UIPolicyMapElement(const RULE_TYPE_PolicyElement_T* src, RULE_TYPE_UI_PolicyElement_T* dst)
{
    RULE_TYPE_ClassMap_T        *class_map_p;
    RULE_TYPE_TBParamEntry_T    *meter_p;
    RULE_TYPE_Action_T          *action_p;

    if ((NULL == src) || (NULL == dst))
        return RULE_TYPE_FAIL;

    memset(dst, 0, sizeof(RULE_TYPE_UI_PolicyElement_T));

    class_map_p = RULE_OM_LocalGetClassMapByIndex(src->class_map_index);
    if (NULL != class_map_p) /* class-map may not exist for MIB */
    {
        memcpy(dst->class_map_name, class_map_p->class_map_name, SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
        dst->class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0'; /* force to end a string */
    }

    meter_p = RULE_OM_LocalGetMeterEntryByIndex(src->meter_index);
    if (NULL != meter_p) /* meter is not always existed */
        memcpy(&dst->meter_entry, meter_p, sizeof(RULE_TYPE_TBParamEntry_T));

    action_p = RULE_OM_LocalGetActionEntryByIndex(src->action_index);
    if (NULL != action_p) /* action may not exist for MIB */
    {
        RULE_TYPE_AclType_T     class_map_type;

        if (RULE_TYPE_OK != RULE_OM_LocalConvert2UIAction(action_p, &dst->action_entry))
            return RULE_TYPE_FAIL;

        if (RULE_TYPE_OK == RULE_OM_LocalClassifyClassMap(class_map_p, &class_map_type))
        {
            if ((IN_ACTION_NEW_IP_DSCP == dst->action_entry.in_action_type) &&
                (RULE_TYPE_IPV6_ACL == class_map_type))
            {
                dst->action_entry.in_action_type = IN_ACTION_NEW_IPV6_DSCP; /* convert to IN_ACTION_NEW_IPV6_DSCP */
            }
        }
    }

    dst->row_status = src->row_status;

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalConvert2UIClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : translate class-map to UI class-map
 * INPUT    : src
 * OUTPUT   : dst
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalConvert2UIClassMap(const RULE_TYPE_ClassMap_T* src, RULE_TYPE_UI_ClassMap_T* dst)
{
    if ((NULL == src) || (NULL == dst))
        return RULE_TYPE_FAIL;

    memcpy(dst->class_map_name, src->class_map_name, SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
    dst->class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0'; /* force to end a string */

    memcpy(dst->desc, src->desc,SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH);
    dst->desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH] = '\0'; /* force to end a string */

    dst->class_map_match_type = src->class_map_match_type;
    dst->row_status = src->row_status;

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalConvert2UIClassMapElement
 *------------------------------------------------------------------------------
 * PURPOSE  : translate class-map element to UI class-map element
 * INPUT    : src
 * OUTPUT   : dst
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalConvert2UIClassMapElement(const RULE_TYPE_ClassMap_T* src, UI32_T element_index, RULE_TYPE_UI_ClassMapElement_T* dst)
{
    UI32_T  class_index;

    if ((NULL == src) || (NULL == dst))
        return RULE_TYPE_FAIL;

    /*element_index is 1-based index */
    if ((0 >= element_index) || (element_index > src->class_element_count))
        return RULE_TYPE_FAIL;

    dst->class_type = src->class_type[element_index - 1]; /* to zero-based array index */
    class_index = src->class_index_list[element_index - 1];

    switch (dst->class_type)
    {
        case RULE_TYPE_CLASS_ACL:
        {
            RULE_TYPE_Acl_T     *acl_p;

            acl_p = RULE_OM_LocalGetAclByIndex(class_index);
            if (NULL == acl_p)
                return RULE_TYPE_FAIL; /* should not go here */

            memcpy(dst->element.acl_name, acl_p->acl_name, SYS_ADPT_ACL_MAX_NAME_LEN);
            dst->element.acl_name[SYS_ADPT_ACL_MAX_NAME_LEN] = '\0'; /* force to end a string */
            break;
        }
        case RULE_TYPE_CLASS_MF:
        {
            RULE_TYPE_Ace_Entry_T   *ace_p;

            ace_p = RULE_OM_LocalGetAceByIndex(class_index);
            if (NULL == ace_p)
                return RULE_TYPE_FAIL; /* should not go here */

            switch (ace_p->ace_type)
            {
                case RULE_TYPE_IP_EXT_ACL:
                    if (ace_p->u.ip.acePrec != RULE_TYPE_UNDEF_IP_PRECEDENCE) /*default value --- meaning not set*/
                    {
                        dst->element.mf_entry.mf_type = RULE_TYPE_MF_PRECEDENCE;
                        dst->element.mf_entry.mf_value = ace_p->u.ip.acePrec;
                    }
                    else if (ace_p->u.ip.aceDscp != RULE_TYPE_UNDEF_IP_DSCP) /*default value --- meaning not set*/
                    {
                        dst->element.mf_entry.mf_type = RULE_TYPE_MF_DSCP;
                        dst->element.mf_entry.mf_value = ace_p->u.ip.aceDscp;
                    }
                    else
                    {
                        /* No IP MF value */
                        return RULE_TYPE_FAIL;
                    }
                    break;

                case RULE_TYPE_MAC_ACL:
                    if (ace_p->u.mac.aceVidOp == VAL_diffServMacAceVidOp_equal) /*default value --- meaning not set*/
                    {
                        dst->element.mf_entry.mf_type = RULE_TYPE_MF_VLAN;
                        dst->element.mf_entry.mf_value = ace_p->u.mac.aceMinVid;
                    }
                    else if (ace_p->u.mac.aceCosOp == VAL_diffServMacAceCosOp_equal)
                    {
                        dst->element.mf_entry.mf_type = RULE_TYPE_MF_COS;
                        dst->element.mf_entry.mf_value = ace_p->u.mac.aceMinCos;
                    }
                    else if (ace_p->u.mac.aceSourcePortOp == VAL_diffServMacAceSourcePortOp_equal)
                    {
                        dst->element.mf_entry.mf_type = RULE_TYPE_MF_SOURCE_PORT;
                        dst->element.mf_entry.mf_value = ace_p->u.mac.aceMinSourcePort;
                    }
                    else
                    {
                        /* No MAC MF value */
                        return RULE_TYPE_FAIL;
                    }
                    break;

                case RULE_TYPE_IPV6_EXT_ACL:
                    if (ace_p->u.ipv6.aceDscp != RULE_TYPE_UNDEF_IPV6_DSCP) /*default value --- meaning not set*/
                    {
                        dst->element.mf_entry.mf_type = RULE_TYPE_MF_DSCP_IPV6;
                        dst->element.mf_entry.mf_value = ace_p->u.ipv6.aceDscp;
                    }
                    else
                    {
                        return RULE_TYPE_FAIL; /* should not go here */
                    }
                    break;

                case RULE_TYPE_IP_ACL:
                case RULE_TYPE_IP_STD_ACL:
                case RULE_TYPE_MF:
                case RULE_TYPE_IPV6_ACL:
                case RULE_TYPE_IPV6_STD_ACL:
                default:
                    return RULE_TYPE_FAIL; /* should not go here */
            }
            break;
        }
        default:
            return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalConvert2UIAction
 *------------------------------------------------------------------------------
 * PURPOSE  : translate action to UI action
 * INPUT    : src
 * OUTPUT   : dst
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalConvert2UIAction(const RULE_TYPE_Action_T* src, RULE_TYPE_UI_Action_T* dst)
{
    if ((NULL == src) || (NULL == dst))
        return RULE_TYPE_FAIL;

    /*convert conform action*/
    if (src->action_bitmap & RULE_TYPE_ACTION_GREEN_PKT_TRANSMIT)
    {
        dst->confirm_action_type = CONFIRM_ACTION_TRANSMIT;
        dst->confirm_action_value = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;
    }
    else if (src->action_bitmap & RULE_TYPE_ACTION_GREEN_PKT_DROP)
    {
        dst->confirm_action_type = CONFIRM_ACTION_DROP;
        dst->confirm_action_value = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;
    }
    else if (src->action_bitmap & RULE_TYPE_ACTION_GREEN_PKT_NEW_DSCP)
    {
        dst->confirm_action_type = CONFIRM_ACTION_REMARK_DSCP_TRANSMIT;
        dst->confirm_action_value = src->green_pkt_new_dscp;
    }
    else /* no out action */
    {
        dst->confirm_action_type = CONFIRM_ACTION_INVALID;
        dst->confirm_action_value = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;
    }

    /*convert exceed action*/
    if (src->action_bitmap & RULE_TYPE_ACTION_YELLOW_DROP)
    {
        dst->exceed_action_type = EXCEED_ACTION_DROP;
        dst->exceed_action_value = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;
    }
    else if (src->action_bitmap & RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP)
    {
        dst->exceed_action_type = EXCEED_ACTION_REMARK_DSCP_TRANSMIT;
        dst->exceed_action_value = src->yellow_pkt_new_dscp;
    }
    else /* no out action */
    {
        dst->exceed_action_type = EXCEED_ACTION_INVALID;
        dst->exceed_action_value = 0;
    }

    /* convert out action */
    if (src->action_bitmap & RULE_TYPE_ACTION_RED_DROP)
    {
        dst->violate_action_type = VIOLATE_ACTION_DROP;
        dst->violate_action_value = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;
    }
    else if (src->action_bitmap & RULE_TYPE_ACTION_RED_PKT_NEW_DSCP)
    {
        dst->violate_action_type = VIOLATE_ACTION_REMARK_DSCP_TRANSMIT;
        dst->violate_action_value = src->red_pkt_new_dscp;
    }
    else /* no out action */
    {
        dst->violate_action_type = VIOLATE_ACTION_INVALID;
        dst->violate_action_value = 0;
    }

    /* convert in action */
    if (src->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_PRI)
    {
        dst->in_action_type = IN_ACTION_NEW_COS;
        dst->in_action_value = src->pkt_new_pri;
    }
    else if (src->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_PHB)
    {
        dst->in_action_type = IN_ACTION_NEW_PHB;
        dst->in_action_value = src->pkt_new_phb;
    }
    else if (src->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_TOS)
    {
        dst->in_action_type = IN_ACTION_NEW_IP_PRECEDENCE;
        dst->in_action_value = src->pkt_new_tos;
    }
    else if (src->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_DSCP)
    {
        dst->in_action_type = IN_ACTION_NEW_IP_DSCP;
        dst->in_action_value = src->pkt_new_dscp;
    }
    else /* no in action */
    {
        dst->in_action_type = IN_ACTION_INVALID;
        dst->in_action_value = 0;
    }

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    if (src->action_bitmap & RULE_TYPE_ACTION_PKT_PRI)
    {
        dst->class_action_type  = CLASS_ACTION_SET_PRIORITY;
        dst->class_action_value = src->pkt_pri;
    }
    else
    {
        dst->class_action_type  = CLASS_ACTION_INVALID;
        dst->class_action_value = 0;
    }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalConvert2UIAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : translate acl to UI acl
 * INPUT    : src
 * OUTPUT   : dst
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalConvert2UIAcl(const RULE_TYPE_Acl_T* src, RULE_TYPE_UI_AclEntry_T* dst)
{
    ASSERT(NULL != src);
    ASSERT(NULL != dst);

    strncpy(dst->acl_name, src->acl_name, sizeof(dst->acl_name) - 1);
    dst->acl_name[sizeof(dst->acl_name) - 1] = '\0';

    dst->acl_type = src->acl_type;
    dst->row_status = src->row_status;

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalConvert2MibPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE  : translate policy-map to mib policy-map
 * INPUT    : src
 * OUTPUT   : dst
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalConvert2MibPolicyMap(const RULE_TYPE_PolicyMap_T* src, RULE_TYPE_MIB_PolicyMap_T* dst)
{
#define MAX_BYTES_PER_ELEMENT           2 /* two bytes one element */

    UI32_T  element_cnt;
    UI16_T  network_byte_order_ui16;

    if ((NULL == src) || (NULL == dst))
        return RULE_TYPE_FAIL;

    if ((MAX_BYTES_PER_ELEMENT * src->element_count) > MAXSIZE_diffServPolicyMapElementIndexList)
    {
        /* buffer is not big enough to put data */
        return RULE_TYPE_FAIL;
    }

    memcpy(dst->name, src->name, SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
    dst->name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0';

    memcpy(dst->desc, src->desc, SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH);
    dst->desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH] = '\0';

    dst->size_of_element_index_list = src->element_count * MAX_BYTES_PER_ELEMENT; /* because two bytes one element */
    dst->row_status = src->row_status;

    for (element_cnt = 0; element_cnt < src->element_count; element_cnt++)
    {
        /* fill policy_map_element_index
         * use two bytes stand for one element
         * (element index should not more than 65535 so there is no over-flow problem)
         */
        network_byte_order_ui16 = L_STDLIB_Hton16 (src->policy_map_element_index_list[element_cnt]);
        memcpy(&dst->policy_map_element_index_list[element_cnt * MAX_BYTES_PER_ELEMENT], &network_byte_order_ui16, sizeof(UI16_T));
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalConvert2MibClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : translate class-map to mib class-map
 * INPUT    : src
 * OUTPUT   : dst
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalConvert2MibClassMap(const RULE_TYPE_ClassMap_T* src, RULE_TYPE_MIB_ClassMap_T* dst)
{
#define MAX_BYTES_PER_ELEMENT           2 /* two bytes one element */

    UI32_T  element_cnt;
    UI16_T  class_type, network_byte_order_ui16;

    if ((NULL == src) || (NULL == dst))
        return RULE_TYPE_FAIL;

    if (((MAX_BYTES_PER_ELEMENT * src->class_element_count) > MAXSIZE_diffServClassMapElementIndexTypeList) ||
        ((MAX_BYTES_PER_ELEMENT * src->class_element_count) > MAXSIZE_diffServClassMapElementIndexList))
    {
        /* buffer is not big enough to put data */
        return RULE_TYPE_FAIL;
    }

    memcpy(dst->class_map_name, src->class_map_name, SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
    dst->class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0';

    memcpy(dst->desc, src->desc, SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH);
    dst->desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH] = '\0';

    switch (src->class_map_match_type)
    {
        case RULE_TYPE_CLASS_MAP_MATCH_ANY:
            dst->class_map_match_type = VAL_diffServClassMapMatchType_matchAny;
            break;

        case RULE_TYPE_CLASS_MAP_MATCH_ALL:
            dst->class_map_match_type = VAL_diffServClassMapMatchType_matchAll;
            break;

        default:
            return RULE_TYPE_FAIL;
    }

    /* NOTE:
     *       should not use (UI16_T *) pointer to get the address of class_type/class_index_list
     *       because the address of (UI16_T *) need to start from the boundrt of 2's multiple
     *       for some CPU type.
     */
    for (element_cnt = 0; element_cnt < src->class_element_count; ++element_cnt)
    {
        {
            RULE_TYPE_AclType_T     classified_type;

            switch (src->class_type[element_cnt])
            {
                case RULE_TYPE_CLASS_MF:
                {
                    RULE_TYPE_Ace_Entry_T   *ace_p;

                    ace_p = RULE_OM_LocalGetAceByIndex(src->class_index_list[element_cnt]);
                    if ((NULL == ace_p) ||
                        (RULE_TYPE_OK != RULE_OM_LocalClassifyMf(ace_p, &classified_type)))
                    {
                        ASSERT(0);
                        return RULE_TYPE_FAIL;
                    }
                    break;
                }
                case RULE_TYPE_CLASS_ACL:
                {
                    RULE_TYPE_Acl_T         *acl_p;

                    acl_p = RULE_OM_LocalGetAclByIndex(src->class_index_list[element_cnt]);
                    if ((NULL == acl_p) ||
                        (RULE_TYPE_OK != RULE_OM_LocalClassifyAcl(acl_p, &classified_type)))
                    {
                        ASSERT(0);
                        return RULE_TYPE_FAIL;
                    }
                    break;
                }
                default:
                    return RULE_TYPE_FAIL;
            }

            switch (classified_type)
            {
                case RULE_TYPE_MAC_ACL:
                    class_type = VAL_diffServClassMapAttachCtlElementIndexType_macAce;
                    break;

                case RULE_TYPE_IP_ACL:
                    class_type = VAL_diffServClassMapAttachCtlElementIndexType_ipAce;
                    break;

                case RULE_TYPE_IPV6_STD_ACL:
                case RULE_TYPE_IPV6_EXT_ACL:
                    class_type = VAL_diffServClassMapAttachCtlElementIndexType_ipv6Ace;
                    break;

                default:
                    ASSERT(0);
                    return RULE_TYPE_FAIL;
            }
        }

        /* fill class_type */
        switch (src->class_type[element_cnt])
        {
            case RULE_TYPE_CLASS_MF:
                network_byte_order_ui16 = L_STDLIB_Hton16 (class_type);
                break;

            case RULE_TYPE_CLASS_ACL:
                network_byte_order_ui16 = L_STDLIB_Hton16 (VAL_diffServClassMapAttachCtlElementIndexType_acl);
                break;

            default:
                ASSERT(0);
                return RULE_TYPE_FAIL;
        }
        memcpy(&dst->class_type[element_cnt * MAX_BYTES_PER_ELEMENT], &network_byte_order_ui16, sizeof(UI16_T));

        /* fill class_index
         * use two bytes stand for one element
         * (class index should not more than 65535 so there is no over-flow problem)
         */
        network_byte_order_ui16 = L_STDLIB_Hton16 (src->class_index_list[element_cnt]);
        memcpy(&dst->class_index_list[element_cnt * MAX_BYTES_PER_ELEMENT], &network_byte_order_ui16, sizeof(UI16_T));
    }

    dst->size_of_list = src->class_element_count * MAX_BYTES_PER_ELEMENT; /* because two bytes one element */
    dst->row_status = src->row_status;

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalConvert2MibClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : translate class-map to mib class-map
 * INPUT    : src
 * OUTPUT   : dst
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalConvert2MibAcl(
    const RULE_TYPE_Acl_T* src,
    RULE_TYPE_MIB_Acl_T* dst)
{
#define MAX_BYTES_PER_ACE_INDEX         2 /* two bytes one element */

    RULE_TYPE_Ace_Entry_T *ace_entry_p;

    UI32_T  ace_index;
    UI32_T  ace_cnt;
    UI16_T  network_byte_order_ui16;

    if ((NULL == src) || (NULL == dst))
        return RULE_TYPE_FAIL;

    if ((MAX_BYTES_PER_ACE_INDEX * src->ace_count) > MAXSIZE_diffServAclAceIndexList)
    {
        /* buffer is not big enough to put data */
        return RULE_TYPE_FAIL;
    }

    memcpy(dst->acl_name, src->acl_name, SYS_ADPT_ACL_MAX_NAME_LEN);
    dst->acl_name[SYS_ADPT_ACL_MAX_NAME_LEN] = '\0';

    switch (src->acl_type)
    {
        case RULE_TYPE_MAC_ACL:
            dst->acl_type = VAL_diffServAclType_mac;
            break;
        case RULE_TYPE_IP_STD_ACL:
            dst->acl_type = VAL_diffServAclType_ipstandard;
            break;
        case RULE_TYPE_IP_EXT_ACL:
            dst->acl_type = VAL_diffServAclType_ipextended;
            break;
        case RULE_TYPE_IPV6_STD_ACL:
            dst->acl_type = VAL_diffServAclType_ipv6standard;
            break;
        case RULE_TYPE_IPV6_EXT_ACL:
            dst->acl_type = VAL_diffServAclType_ipv6extended;
            break;

#if (SYS_CPNT_DAI == TRUE)
        case RULE_TYPE_ARP_ACL:
            dst->acl_type = VAL_diffServAclType_arp;
            break;
#endif /* #if (SYS_CPNT_DAI == TRUE) */

        default:
            return RULE_TYPE_FAIL;
    }

    /* NOTE:
     *       should not use (UI16_T *) pointer to get the address of ace_index_list
     *       because the address of (UI16_T *) need to start from the boundrt of 2's multiple
     *       for some CPU type.
     */
    for (ace_index = 0, ace_cnt = 0;
         RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(src, &ace_index, &ace_entry_p); ++ace_cnt)
    {
        /* fill ace_index
         * use two bytes stand for one element
         * (ace index should not more than 65535 so there is no over-flow problem)
         */
        network_byte_order_ui16 = L_STDLIB_Hton16 ( ace_index );
        memcpy(&dst->ace_index_list[ace_cnt * MAX_BYTES_PER_ACE_INDEX], &network_byte_order_ui16, sizeof(UI16_T));
    }

    dst->size_of_ace_index_list = src->ace_count * MAX_BYTES_PER_ACE_INDEX; /* because two bytes one element */
    dst->row_status = src->row_status;

    return RULE_TYPE_OK;

#undef MAX_BYTES_PER_ACE_INDEX
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalConvert2MibAction
 *------------------------------------------------------------------------------
 * PURPOSE  : translate action entry to mib action entry
 * INPUT    : src
 * OUTPUT   : dst
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalConvert2MibAction(const RULE_TYPE_Action_T* src, RULE_TYPE_MIB_Action_T* dst)
{
    if ((NULL == src) || (NULL == dst))
        return RULE_TYPE_FAIL;

    /* translate core layer bitmap 2 mib bitmap */
    if (RULE_TYPE_ACTION_PKT_NEW_PRI & src->action_bitmap)
        RULE_OM_SET_BIT_ON(dst->action_bitmap, VAL_diffServActionList_actionPktNewPri);

    if (RULE_TYPE_ACTION_PKT_NEW_PHB & src->action_bitmap)
        RULE_OM_SET_BIT_ON(dst->action_bitmap, VAL_diffServActionList_actionPktNewPhb);

    if (RULE_TYPE_ACTION_PKT_NEW_TOS & src->action_bitmap)
        RULE_OM_SET_BIT_ON(dst->action_bitmap, VAL_diffServActionList_actionPktNewIpPrec);

    if (RULE_TYPE_ACTION_PKT_NEW_DSCP & src->action_bitmap)
        RULE_OM_SET_BIT_ON(dst->action_bitmap, VAL_diffServActionList_actionPktNewDscp);

    if (RULE_TYPE_ACTION_GREEN_PKT_NEW_DSCP & src->action_bitmap)
        RULE_OM_SET_BIT_ON(dst->action_bitmap, VAL_diffServActionList_actionGreenPktNewDscp);

    if (RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP & src->action_bitmap)
        RULE_OM_SET_BIT_ON(dst->action_bitmap, VAL_diffServActionList_actionYellowPktNewDscp);

    if (RULE_TYPE_ACTION_RED_PKT_NEW_DSCP & src->action_bitmap)
        RULE_OM_SET_BIT_ON(dst->action_bitmap, VAL_diffServActionList_actionRedPktNewDscp);

    if (RULE_TYPE_ACTION_YELLOW_DROP & src->action_bitmap)
        RULE_OM_SET_BIT_ON(dst->action_bitmap, VAL_diffServActionList_actionYellowPktDrop);

    if (RULE_TYPE_ACTION_RED_DROP & src->action_bitmap)
        RULE_OM_SET_BIT_ON(dst->action_bitmap, VAL_diffServActionList_actionRedPktDrop);

    if (RULE_TYPE_ACTION_GREEN_PKT_TRANSMIT & src->action_bitmap)
    {
        RULE_OM_SET_BIT_ON(dst->action_bitmap, VAL_diffServActionList_actionGreenPktTransmit);
        dst->green_pkt_transmit = VAL_diffServActionGreenPktTransmit_enabled;
    }
    else
        dst->green_pkt_transmit = VAL_diffServActionGreenPktTransmit_disabled;
#if 0
/*zhangwei add*/

    if(!((RULE_TYPE_ACTION_PKT_NEW_DSCP & src->action_bitmap)
        || (RULE_TYPE_ACTION_DROP & src->action_bitmap)))
    {
        RULE_OM_SET_BIT_ON(dst->action_bitmap, VAL_diffServActionList_actionGreenPktTransmit);
        dst->green_pkt_transmit = VAL_diffServActionRedPktDrop_enabled;
    }
    else
        dst->green_pkt_transmit = VAL_diffServActionRedPktDrop_disabled;
#endif
    dst->pkt_new_pri = src->pkt_new_pri;
    dst->pkt_phb = src->pkt_new_phb;
    dst->pkt_new_dscp = src->pkt_new_dscp;
    dst->yellow_pkt_new_dscp = src->yellow_pkt_new_dscp;
    dst->red_pkt_new_dscp = src->red_pkt_new_dscp;

    if (RULE_TYPE_ACTION_RED_DROP & src->action_bitmap)
        dst->red_drop = VAL_diffServActionRedPktDrop_enabled;
    else
        dst->red_drop = VAL_diffServActionRedPktDrop_disabled;

 if (RULE_TYPE_ACTION_YELLOW_DROP & src->action_bitmap)
    dst->yellow_drop = VAL_diffServActionYellowPktDrop_enabled;
else
    dst->yellow_drop = VAL_diffServActionYellowPktDrop_disabled;

#if 0
    dst->drop_precedence = src->drop_precedence;
    dst->red_drop_precedence = src->red_drop_precedence;
    dst->yellow_drop_precedence = src->yellow_drop_precedence;
#endif

    dst->row_status = src->row_status;

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalAddIpDscpReference
 *------------------------------------------------------------------------------
 * PURPOSE  : allocate IP DSCP reference entry and add it to list
 * INPUT    : ip_dscp, policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalAddIpDscpReference(UI32_T ip_dscp, UI32_T policy_map_index, UI32_T class_map_index)
{
#if (RULE_CTRL_USE_REF_LIST == TRUE)
    RULE_OM_CosReferenceEntry_T     *head_of_list_p, *new_entry_p;

    if (RULE_OM_IS_BAD_IP_DSCP(ip_dscp))
        return RULE_TYPE_FAIL;

    if (RULE_TYPE_OK == RULE_OM_LocalIsExitedIpDscpReference(ip_dscp, policy_map_index, class_map_index))
        return RULE_TYPE_OK; /* no need to add twice */

    /* allocate memory */
    new_entry_p = (RULE_OM_CosReferenceEntry_T*)L_MM_Malloc(sizeof(RULE_OM_CosReferenceEntry_T), L_MM_USER_ID2(SYS_MODULE_RULE_CTRL, 0));
    if (NULL == new_entry_p)
        return RULE_TYPE_FAIL;

    memset(new_entry_p, 0, sizeof(RULE_OM_CosReferenceEntry_T));
    new_entry_p->policy_map_index = policy_map_index;
    new_entry_p->class_map_index = class_map_index;

    /* insert this new entry into first */
    head_of_list_p = dscp_reference_list[ip_dscp];
    if (NULL != head_of_list_p)
    {
        head_of_list_p->prev_entry = new_entry_p;
        new_entry_p->next_entry = head_of_list_p;
    }

    dscp_reference_list[ip_dscp] = new_entry_p; /* new head if linked list */
#endif /* #if (RULE_CTRL_USE_REF_LIST == TRUE) */

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalAddIpPrecedenceReference
 *------------------------------------------------------------------------------
 * PURPOSE  : allocate IP Precedence reference entry and add it to list
 * INPUT    : ip_precedence, policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalAddIpPrecedenceReference(UI32_T ip_precedence, UI32_T policy_map_index, UI32_T class_map_index)
{
#if (RULE_CTRL_USE_REF_LIST == TRUE)
    RULE_OM_CosReferenceEntry_T     *head_of_list_p, *new_entry_p;

    if (RULE_OM_IS_BAD_IP_PRECEDENCE(ip_precedence))
        return RULE_TYPE_FAIL;

    if (RULE_TYPE_OK == RULE_OM_LocalIsExitedIpPrecedenceReference(ip_precedence, policy_map_index, class_map_index))
        return RULE_TYPE_OK; /* no need to add twice */

    /* allocate memory */
    new_entry_p = (RULE_OM_CosReferenceEntry_T*)L_MM_Malloc(sizeof(RULE_OM_CosReferenceEntry_T), L_MM_USER_ID2(SYS_MODULE_RULE_CTRL, 0));
    if (NULL == new_entry_p)
        return RULE_TYPE_FAIL;

    memset(new_entry_p, 0, sizeof(RULE_OM_CosReferenceEntry_T));
    new_entry_p->policy_map_index = policy_map_index;
    new_entry_p->class_map_index = class_map_index;

    /* insert this new entry into first */
    head_of_list_p = precedence_reference_list[ip_precedence];
    if (NULL != head_of_list_p)
    {
        head_of_list_p->prev_entry = new_entry_p;
        new_entry_p->next_entry = head_of_list_p;
    }

    precedence_reference_list[ip_precedence] = new_entry_p; /* new head if linked list */
#endif /* #if (RULE_CTRL_USE_REF_LIST == TRUE) */

    return RULE_TYPE_OK;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalIsExistPolicyMapName
 *------------------------------------------------------------------------------
 * PURPOSE:  Check the policy map name is used or not.
 * INPUT:    policy_map_name
 * OUTPUT:   NONE.
 * RETURN:   Duplicate --> RULE_TYPE_OK/ Not Duplicate --> RULE_TYPE_FAIL.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalIsExistPolicyMapName(const char *policy_map_name)
{
    UI32_T  policy_map_index;
    BOOL_T  ret;

    ret = RULE_OM_LocalSearchPolicyMapByName(policy_map_name, &policy_map_index);

    return ret;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalGetPolicyMapIndexByElementIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  get index of policy map by policy-map element index
 * INPUT:    policy_element_index
 * OUTPUT:   policy_map_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalGetPolicyMapIndexByElementIndex(UI32_T policy_element_index, UI32_T *policy_map_index)
{
    RULE_TYPE_PolicyMap_T   *policy_map_p;
    UI32_T                  policy_map_cnt, element_cnt;

    /* search every policy-map and its elements */
    for (policy_map_cnt = 0; SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP > policy_map_cnt; ++policy_map_cnt)
    {
        policy_map_p = &shmem_data_p->policy_map_table[policy_map_cnt];

        if (RULE_OM_IS_ENTRY_DESTROY(policy_map_p->row_status))
            continue;

        for (element_cnt = 0; element_cnt < policy_map_p->element_count; ++element_cnt)
        {
            if (policy_element_index != policy_map_p->policy_map_element_index_list[element_cnt])
                continue;

            *policy_map_index = policy_map_cnt + 1; /* to 1-based */
            return RULE_TYPE_OK;
        }
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalGetPolicyMapElementIndexByMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  get index of policy-map element by meter index
 * INPUT:    meter_index
 * OUTPUT:   policy_element_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     return OK only if we can find a policy-map element index
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalGetPolicyMapElementIndexByMeter(UI32_T meter_index, UI32_T *policy_element_index)
{
    RULE_TYPE_PolicyElement_T   *policy_element_p;
    UI32_T                      policy_element_cnt;

    if (RULE_OM_IS_BAD_METER_IDX(meter_index))
        return RULE_TYPE_FAIL;

    if (NULL == policy_element_index)
        return RULE_TYPE_FAIL;

    /* search every policy-map element */
    for (policy_element_cnt = 0; SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS > policy_element_cnt; ++policy_element_cnt)
    {
        policy_element_p = &shmem_data_p->policy_map_element_table[policy_element_cnt];

        if (RULE_OM_IS_ENTRY_DESTROY(policy_element_p->row_status))
            continue;

        if (meter_index != policy_element_p->meter_index)
            continue;

        *policy_element_index = policy_element_cnt + 1; /* to 1-based */
        return RULE_TYPE_OK;
    }

    return RULE_TYPE_FAIL;
}

#if (RULE_CTRL_USE_REF_LIST == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalIsExitedIpDscpReference
 *------------------------------------------------------------------------------
 * PURPOSE  : search IP DSCP reference by (ip_dscp, policy_map_index, class_map_index)
 * INPUT    : ip_dscp, policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : OK -- existed  / FAIL -- not existed
 * NOTES    : none
 *------------------------------------------------------------------------------*/
static UI32_T RULE_OM_LocalIsExitedIpDscpReference(UI32_T ip_dscp, UI32_T policy_map_index, UI32_T class_map_index)
{
#if (RULE_CTRL_USE_REF_LIST == TRUE)
    RULE_OM_CosReferenceEntry_T     *entry_iter_p;

    if (RULE_OM_IS_BAD_IP_DSCP(ip_dscp))
        return RULE_TYPE_FAIL;

    for (entry_iter_p = dscp_reference_list[ip_dscp]; NULL != entry_iter_p;
        entry_iter_p = entry_iter_p->next_entry)
    {
        if ((policy_map_index == entry_iter_p->policy_map_index) &&
            (class_map_index == entry_iter_p->class_map_index))
        {
            return RULE_TYPE_OK;
        }
    }
#endif /* #if (RULE_CTRL_USE_REF_LIST == TRUE) */

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalIsExitedIpPrecedenceReference
 *------------------------------------------------------------------------------
 * PURPOSE  : search IP Precedence reference by (ip_precedence, policy_map_index, class_map_index)
 * INPUT    : ip_precedence, policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : OK -- existed  / FAIL -- not existed
 * NOTES    : none
 *------------------------------------------------------------------------------*/

static UI32_T RULE_OM_LocalIsExitedIpPrecedenceReference(UI32_T ip_precedence, UI32_T policy_map_index, UI32_T class_map_index)
{
#if (RULE_CTRL_USE_REF_LIST == TRUE)
    RULE_OM_CosReferenceEntry_T     *entry_iter_p;

    if (RULE_OM_IS_BAD_IP_PRECEDENCE(ip_precedence))
        return RULE_TYPE_FAIL;

    for (entry_iter_p = precedence_reference_list[ip_precedence]; NULL != entry_iter_p;
        entry_iter_p = entry_iter_p->next_entry)
    {
        if ((policy_map_index == entry_iter_p->policy_map_index) &&
            (class_map_index == entry_iter_p->class_map_index))
        {
            return RULE_TYPE_OK;
        }
    }
#endif /* #if (RULE_CTRL_USE_REF_LIST == TRUE) */

    return RULE_TYPE_FAIL;
}
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalFreeIpDscpReference
 *------------------------------------------------------------------------------
 * PURPOSE  : search IP DSCP reference entry and free its memory
 * INPUT    : ip_dscp, policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *------------------------------------------------------------------------------*/
static void RULE_OM_LocalFreeIpDscpReference(UI32_T ip_dscp, UI32_T policy_map_index, UI32_T class_map_index)
{
#if (RULE_CTRL_USE_REF_LIST == TRUE)
    RULE_OM_CosReferenceEntry_T     *entry_iter_p;

    if (RULE_OM_IS_BAD_IP_DSCP(ip_dscp))
        return;

    for (entry_iter_p = dscp_reference_list[ip_dscp]; NULL != entry_iter_p;
        entry_iter_p = entry_iter_p->next_entry)
    {
        if ((policy_map_index == entry_iter_p->policy_map_index) &&
            (class_map_index == entry_iter_p->class_map_index))
        {
            if (entry_iter_p == dscp_reference_list[ip_dscp])
                dscp_reference_list[ip_dscp] = entry_iter_p->next_entry; /* update head of linked list */

            if (NULL != entry_iter_p->prev_entry)
                entry_iter_p->prev_entry->next_entry = entry_iter_p->next_entry;

            if (NULL != entry_iter_p->next_entry)
                entry_iter_p->next_entry->prev_entry = entry_iter_p->prev_entry;

            L_MM_Free(entry_iter_p);

            return; /* one IP DSCP one (policy_map_index, class_map_index) pair */
        }
    }
#endif /* #if (RULE_CTRL_USE_REF_LIST == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalFreeIpPrecedenceReference
 *------------------------------------------------------------------------------
 * PURPOSE  : search IP Precedence reference entry and free its memory
 * INPUT    : ip_precedence, policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *------------------------------------------------------------------------------*/
static void RULE_OM_LocalFreeIpPrecedenceReference(UI32_T ip_precedence, UI32_T policy_map_index, UI32_T class_map_index)
{
#if (RULE_CTRL_USE_REF_LIST == TRUE)
    RULE_OM_CosReferenceEntry_T     *entry_iter_p;

    if (RULE_OM_IS_BAD_IP_PRECEDENCE(ip_precedence))
        return;

    for (entry_iter_p = precedence_reference_list[ip_precedence]; NULL != entry_iter_p;
        entry_iter_p = entry_iter_p->next_entry)
    {
        if ((policy_map_index == entry_iter_p->policy_map_index) &&
            (class_map_index == entry_iter_p->class_map_index))
        {
            if (entry_iter_p == precedence_reference_list[ip_precedence])
                precedence_reference_list[ip_precedence] = entry_iter_p->next_entry; /* update head of linked list */

            if (NULL != entry_iter_p->prev_entry)
                entry_iter_p->prev_entry->next_entry = entry_iter_p->next_entry;

            if (NULL != entry_iter_p->next_entry)
                entry_iter_p->next_entry->prev_entry = entry_iter_p->prev_entry;

            L_MM_Free(entry_iter_p);

            return; /* one IP Precedence one (policy_map_index, class_map_index) pair */
        }
    }
#endif /* #if (RULE_CTRL_USE_REF_LIST == TRUE) */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalRemoveCosReference
 *------------------------------------------------------------------------------
 * PURPOSE  : remove cos reference if action reference to COS
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static void RULE_OM_LocalRemoveCosReference(UI32_T policy_map_index, UI32_T class_map_index)
{
    RULE_TYPE_PolicyElement_T   *policy_element_p;
    RULE_TYPE_Action_T          *action_p;

    policy_element_p = RULE_OM_LocalGetPolicyMapElement(policy_map_index, class_map_index);
    if (NULL == policy_element_p)
        return; /* bad index */

    action_p = RULE_OM_LocalGetActionEntryByIndex(policy_element_p->action_index);
    if (NULL == action_p)
        return; /* bad index */

    /* NOTICE:
     *  different actions may exist at the same time
     *  therefore, do not use if - else if statement
     */
    if (action_p->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_TOS)
    {
        RULE_OM_LocalFreeIpPrecedenceReference(action_p->pkt_new_tos, policy_map_index, class_map_index);
    }

    if (action_p->action_bitmap & RULE_TYPE_ACTION_PKT_NEW_DSCP)
    {
        RULE_OM_LocalFreeIpDscpReference(action_p->pkt_new_dscp, policy_map_index, class_map_index);
    }

    if (action_p->action_bitmap & RULE_TYPE_ACTION_GREEN_PKT_NEW_DSCP)
    {
        RULE_OM_LocalFreeIpDscpReference(action_p->green_pkt_new_dscp, policy_map_index, class_map_index);
    }

    if (action_p->action_bitmap & RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP)
    {
        RULE_OM_LocalFreeIpDscpReference(action_p->yellow_pkt_new_dscp, policy_map_index, class_map_index);
    }

    if (action_p->action_bitmap & RULE_TYPE_ACTION_RED_PKT_NEW_DSCP)
    {
        RULE_OM_LocalFreeIpDscpReference(action_p->red_pkt_new_dscp, policy_map_index, class_map_index);
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalGetClassMapIndexByName
 *------------------------------------------------------------------------------
 * PURPOSE:  get class-map index by class map name
 * INPUT:    class_map_name
 * OUTPUT:   class_map_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalGetClassMapIndexByName(const char *class_map_name, UI32_T *class_map_index)
{
    UI32_T                  index;
    RULE_TYPE_ClassMap_T    *class_map_p;

    if ((NULL == class_map_index) || (class_map_name == NULL))
        return RULE_TYPE_FAIL;

    for (index = 0; SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS_MAP > index; ++index)
    {
        class_map_p = &shmem_data_p->class_map_table[index];

        if (RULE_OM_IS_ENTRY_DESTROY(class_map_p->row_status))
            continue;

        if (strcmp(class_map_name, class_map_p->class_map_name) == 0)
        {
            *class_map_index = index + 1; /* to 1-based index */
            return RULE_TYPE_OK;
        }
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalClassifyAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : get class type according to acl
 * INPUT    : acl_entry
 * OUTPUT   : class_type
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : class_type could be MAC/IP/IPv6-std/IPv6-ext because they use different selector
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalClassifyAcl(const RULE_TYPE_Acl_T *acl_entry, RULE_TYPE_AclType_T *class_type)
{
    if ((NULL == acl_entry) || (NULL == class_type))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    switch(acl_entry->acl_type)
    {
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            *class_type = RULE_TYPE_IP_ACL;
            break;
        case RULE_TYPE_MAC_ACL:
            *class_type = RULE_TYPE_MAC_ACL;
            break;
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            /* because IPv6 standard and extended ACL use different selector */
            *class_type = acl_entry->acl_type;
            break;
#if (SYS_CPNT_DAI == TRUE)
	 case RULE_TYPE_ARP_ACL:
            *class_type = RULE_TYPE_ARP_ACL;
            break;
#endif
        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_MF:
        case RULE_TYPE_IPV6_ACL:
        default:
            return RULE_TYPE_FAIL; /* should not go here */
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalClassifyClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : get class type according to class-map
 * INPUT    : class_map
 * OUTPUT   : class_type
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : class_type could be MAC/IP/IPv6-std/IPv6-ext because they use different selector
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalClassifyClassMap(const RULE_TYPE_ClassMap_T *class_map, RULE_TYPE_AclType_T *class_type)
{
    if (NULL == class_map)
        return RULE_TYPE_FAIL;

    if (0 >= class_map->class_element_count)
        return RULE_TYPE_FAIL; /* has no element, unclassified */

    /* use first element to classsify this class-map */
    switch (class_map->class_type[0])
    {
        case RULE_TYPE_CLASS_MF:
            return RULE_OM_LocalClassifyMf(RULE_OM_LocalGetAceByIndex(class_map->class_index_list[0]), class_type);

        case RULE_TYPE_CLASS_ACL:
            return RULE_OM_LocalClassifyAcl(RULE_OM_LocalGetAclByIndex(class_map->class_index_list[0]), class_type);

        default:
            break; /* should not go here */
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalConvertToAction
 *------------------------------------------------------------------------------
 * PURPOSE  : translate src_action to dst_action
 * INPUT    : src_action
 * OUTPUT   : dst_action
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalConvertToAction(const RULE_TYPE_UI_Action_T *src_action, RULE_TYPE_Action_T *dst_action)
{
    if ((NULL == dst_action) || (NULL == src_action))
        return RULE_TYPE_FAIL;

    memset(dst_action, 0, sizeof(RULE_TYPE_Action_T));

    switch (src_action->in_action_type)
    {
        case IN_ACTION_NEW_COS:
            dst_action->action_bitmap |= RULE_TYPE_ACTION_PKT_NEW_PRI;
            dst_action->pkt_new_pri = src_action->in_action_value;
            break;

        case IN_ACTION_NEW_PHB:
            dst_action->action_bitmap |= RULE_TYPE_ACTION_PKT_NEW_PHB;
            dst_action->pkt_new_phb = src_action->in_action_value;
            break;

        case IN_ACTION_NEW_IP_DSCP:
        case IN_ACTION_NEW_IPV6_DSCP:
            dst_action->action_bitmap |= RULE_TYPE_ACTION_PKT_NEW_DSCP;
            dst_action->pkt_new_dscp = src_action->in_action_value;
            break;

        case IN_ACTION_NEW_IP_PRECEDENCE:
            dst_action->action_bitmap |= RULE_TYPE_ACTION_PKT_NEW_TOS;
            dst_action->pkt_new_tos = src_action->in_action_value;
            break;

        case IN_ACTION_INVALID: /*no in action*/
            break;

        default:
            printf("\r\n[%s:%d] invalid in_action_type(%d)", __FUNCTION__, __LINE__,
                src_action->in_action_type);
            break;
    }

    switch (src_action->confirm_action_type)
    {
        case CONFIRM_ACTION_TRANSMIT:
            dst_action->action_bitmap |= RULE_TYPE_ACTION_GREEN_PKT_TRANSMIT;
            dst_action->green_pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;
            break;

        case CONFIRM_ACTION_DROP:
            dst_action->action_bitmap |= RULE_TYPE_ACTION_GREEN_PKT_DROP;
            dst_action->green_pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;
            break;

        case CONFIRM_ACTION_REMARK_DSCP_TRANSMIT:
            dst_action->action_bitmap |= RULE_TYPE_ACTION_GREEN_PKT_NEW_DSCP;
            dst_action->green_pkt_new_dscp = src_action->confirm_action_value;
            break;

        case CONFIRM_ACTION_INVALID: /*no out action*/
            break;

        default:
            printf("\r\n[%s:%d] invalid confirm_action_type(%d)", __FUNCTION__, __LINE__,
                src_action->confirm_action_type);
            break;
    }

    switch (src_action->exceed_action_type)
    {
        case EXCEED_ACTION_DROP:
            dst_action->action_bitmap |= RULE_TYPE_ACTION_YELLOW_DROP;
            dst_action->yellow_pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;
            break;

        case EXCEED_ACTION_REMARK_DSCP_TRANSMIT:
            dst_action->action_bitmap |= RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP;
            dst_action->yellow_pkt_new_dscp = src_action->exceed_action_value;
            break;

        case EXCEED_ACTION_INVALID: /*no out action*/
            dst_action->yellow_pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;
            break;

        default:
            printf("\r\n[%s:%d] invalid exceed_action_type(%d)", __FUNCTION__, __LINE__,
                src_action->exceed_action_type);
            break;
    }

    switch (src_action->violate_action_type)
    {
        case VIOLATE_ACTION_DROP:
            dst_action->action_bitmap |= RULE_TYPE_ACTION_RED_DROP;
            dst_action->red_pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;
            break;

        case VIOLATE_ACTION_REMARK_DSCP_TRANSMIT:
            dst_action->action_bitmap |= RULE_TYPE_ACTION_RED_PKT_NEW_DSCP;
            dst_action->red_pkt_new_dscp = src_action->violate_action_value;
            break;

        case VIOLATE_ACTION_INVALID: /*no out action*/
            dst_action->red_pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;
            break;

        default:
            printf("\r\n[%s:%d] invalid violate_action_type(%d)\r\n", __FUNCTION__, __LINE__,
                src_action->violate_action_type);
            break;
    }

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    switch (src_action->class_action_type)
    {
        case CLASS_ACTION_SET_PRIORITY:
            dst_action->action_bitmap |= RULE_TYPE_ACTION_PKT_PRI;
            dst_action->pkt_pri = src_action->class_action_value;
            break;

        case CLASS_ACTION_INVALID:
            break;

        default:
            return RULE_TYPE_FAIL;
    }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalGetPolicyMapElementIndexByAction
 *------------------------------------------------------------------------------
 * PURPOSE:  get index of policy-map element by action index
 * INPUT:    action_index
 * OUTPUT:   policy_element_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalGetPolicyMapElementIndexByAction(UI32_T action_index, UI32_T *policy_element_index)
{
    RULE_TYPE_PolicyElement_T   *policy_element_p;
    UI32_T                      policy_element_cnt;

    if (RULE_OM_IS_BAD_ACTION_IDX(action_index))
        return RULE_TYPE_FAIL;

    if (NULL == policy_element_index)
        return RULE_TYPE_FAIL;

    /* search every policy-map element */
    for (policy_element_cnt = 0; SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS > policy_element_cnt; ++policy_element_cnt)
    {
        policy_element_p = &shmem_data_p->policy_map_element_table[policy_element_cnt];

        if (RULE_OM_IS_ENTRY_DESTROY(policy_element_p->row_status))
            continue;

        if (action_index != policy_element_p->action_index)
            continue;

        *policy_element_index = policy_element_cnt + 1; /* to 1-based */
        return RULE_TYPE_OK;
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_LocalIsAclTypeMatched
 *------------------------------------------------------------------------------
 * PURPOSE  : Test if two acltypes are matched
 * INPUT    : src_type, dst_type
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static BOOL_T RULE_OM_LocalIsAclTypeMatched(RULE_TYPE_AclType_T src_type, RULE_TYPE_AclType_T dst_type)
{
    switch (src_type)
    {
    case RULE_TYPE_IP_ACL:
    case RULE_TYPE_IP_STD_ACL:
    case RULE_TYPE_IP_EXT_ACL:
        src_type = RULE_TYPE_IP_ACL;
        break;

    case RULE_TYPE_IPV6_ACL:
    case RULE_TYPE_IPV6_STD_ACL:
    case RULE_TYPE_IPV6_EXT_ACL:
        src_type = RULE_TYPE_IPV6_ACL;
        break;

    default:
        break;
    }

    switch (dst_type)
    {
    case RULE_TYPE_IP_ACL:
    case RULE_TYPE_IP_STD_ACL:
    case RULE_TYPE_IP_EXT_ACL:
        dst_type = RULE_TYPE_IP_ACL;
        break;

    case RULE_TYPE_IPV6_ACL:
    case RULE_TYPE_IPV6_STD_ACL:
    case RULE_TYPE_IPV6_EXT_ACL:
        dst_type = RULE_TYPE_IPV6_ACL;
        break;

    default:
        break;
    }

    return (src_type == dst_type);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalGetAclIndexByAce
 *------------------------------------------------------------------------------
 * PURPOSE:  get acl index by ace_index
 * INPUT:    ace_index
 * OUTPUT:   acl_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalGetAclIndexByAce(
    UI32_T ace_index,
    UI32_T *acl_index,
    RULE_TYPE_Acl_T **acl_pp)
{
    RULE_TYPE_Acl_T         *acl_p;
    RULE_TYPE_Ace_Entry_T   *ace_entry_p;

    UI32_T                  acl_cnt;
    UI32_T                  w_ace_index;

    if (NULL == acl_index)
        return RULE_TYPE_FAIL;

    for (acl_cnt = 0; _countof(shmem_data_p->acl_table) > acl_cnt; ++acl_cnt)
    {
        acl_p = &shmem_data_p->acl_table[acl_cnt];

        if (RULE_OM_IS_ENTRY_DESTROY(acl_p->row_status))
            continue;

        for (w_ace_index = 0;
             RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &w_ace_index, &ace_entry_p);)
        {
            if (ace_index == w_ace_index)
            {
                *acl_index = acl_cnt + 1; /* to 1-based index */

                if (NULL != acl_pp)
                {
                    *acl_pp = acl_p;
                }

                return RULE_TYPE_OK;
            }
        }
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalGetClassMapIndexByMf
 *------------------------------------------------------------------------------
 * PURPOSE  : get class-map index by mf index
 * INPUT    : mf_index
 * OUTPUT   : class_map_index
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalGetClassMapIndexByMf(UI32_T mf_index, UI32_T *class_map_index)
{
    RULE_TYPE_ClassMap_T    *class_map_p;
    UI32_T                  class_map_cnt, element_cnt;

    /* search every class-map and its elements */
    for (class_map_cnt = 0; SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS_MAP > class_map_cnt; ++class_map_cnt)
    {
        class_map_p = &shmem_data_p->class_map_table[class_map_cnt];

        if (RULE_OM_IS_ENTRY_DESTROY(class_map_p->row_status))
            continue;

        for (element_cnt = 0; element_cnt < class_map_p->class_element_count; ++element_cnt)
        {
            if ((RULE_TYPE_CLASS_MF != class_map_p->class_type[element_cnt]) ||
                (mf_index != class_map_p->class_index_list[element_cnt]))
            {
                continue;
            }

            *class_map_index = class_map_cnt + 1; /* to 1-based */
            RULE_OM_LeaveCriticalSectionReturnState(RULE_TYPE_OK);
        }
    }
    return RULE_TYPE_FAIL;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalClassifyMf
 *------------------------------------------------------------------------------
 * PURPOSE  : get class type according to mf
 * INPUT    : mf_entry
 * OUTPUT   : class_type
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : class_type could be MAC/IP/IPv6-std/IPv6-ext because they use different selector
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalClassifyMf(const RULE_TYPE_Ace_Entry_T *mf_entry, RULE_TYPE_AclType_T *class_type)
{
    if ((NULL == mf_entry) || (NULL == class_type))
        return RULE_TYPE_FAIL;

    switch (mf_entry->ace_type)
    {
        case RULE_TYPE_IP_EXT_ACL:
            *class_type = RULE_TYPE_IP_ACL;
            break;
        case RULE_TYPE_MAC_ACL:
            *class_type = RULE_TYPE_MAC_ACL;
            break;
        case RULE_TYPE_IPV6_EXT_ACL:
            /* because IPv6 standard and extended ACL use different selector */
            *class_type = RULE_TYPE_IPV6_EXT_ACL;
            break;
        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_MF:
        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        default:
            return RULE_TYPE_FAIL; /* should not go here */
    }

    return RULE_TYPE_OK;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalClassifyAce
 *------------------------------------------------------------------------------
 * PURPOSE  : get class type according to ace
 * INPUT    : ace_entry
 * OUTPUT   : class_type
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : class_type could be MAC/IP/IPv6-std/IPv6-ext because they use different selector
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalClassifyAce(const RULE_TYPE_Ace_Entry_T *ace_entry, RULE_TYPE_AclType_T *class_type)
{
    if ((NULL == ace_entry) || (NULL == class_type))
        return RULE_TYPE_FAIL;

    switch(ace_entry->ace_type)
    {
        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            *class_type = RULE_TYPE_IP_ACL;
            break;
        case RULE_TYPE_MAC_ACL:
            *class_type = RULE_TYPE_MAC_ACL;
            break;
        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            /* because IPv6 standard and extended ACL use different selector */
            *class_type = ace_entry->ace_type;
            break;
#if (SYS_CPNT_DAI == TRUE)
	 case RULE_TYPE_ARP_ACL:
            *class_type = RULE_TYPE_ARP_ACL;
            break;
#endif
        case RULE_TYPE_MF:
        default:
            return RULE_TYPE_FAIL; /* should not go here */
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalSetInProfileActionToDefault
 *------------------------------------------------------------------------------
 * PURPOSE:  set in-profile action to default value by action_index
 * INPUT:    action_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalSetInProfileActionToDefault(UI32_T action_index)
{
    RULE_TYPE_Action_T    *action_p;
    RULE_TYPE_Action_T    old_action;

    action_p = RULE_OM_LocalGetActionEntryByIndex(action_index);
    if (NULL == action_p)
        return RULE_TYPE_FAIL;

    memcpy(&old_action, action_p, sizeof(old_action));

    /* clean up green action */
    action_p->action_bitmap &= ~RULE_TYPE_ACTION_PKT_NEW_PRI;
    action_p->action_bitmap &= ~RULE_TYPE_ACTION_PKT_NEW_DSCP;
    action_p->action_bitmap &= ~RULE_TYPE_ACTION_PKT_NEW_TOS;
    action_p->action_bitmap &= ~RULE_TYPE_ACTION_PKT_NEW_PHB;

    action_p->pkt_new_pri = RULE_TYPE_ACTION_DEFAULT_PRI;
    action_p->pkt_new_phb = RULE_TYPE_ACTION_DEFAULT_PHB;
    action_p->pkt_new_tos = 0;
    action_p->pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;

    if (RULE_TYPE_OK != RULE_OM_LocalNotify(RULE_TYPE_NOTIFY_ACTION_CHANGED, &action_index))
    {
        memcpy(action_p, &old_action, sizeof(*action_p));
        return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalSetOutOfProfileActionToDefault
 *------------------------------------------------------------------------------
 * PURPOSE:  set out-of-profile action to default value by action_index
 * INPUT:    action_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalSetOutOfProfileActionToDefault(UI32_T action_index)
{
    RULE_TYPE_Action_T    *action_p;
    RULE_TYPE_Action_T    old_action;

    action_p = RULE_OM_LocalGetActionEntryByIndex(action_index);
    if (NULL == action_p)
        return RULE_TYPE_FAIL;

    memcpy(&old_action, action_p, sizeof(old_action));

    /* clear out-of-profile action */
    action_p->action_bitmap &= ~(RULE_TYPE_OUT_OF_PROFILE_ACTION);

    action_p->green_pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;
    action_p->yellow_pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;
    action_p->red_pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;

    /* default to drop */
    action_p->action_bitmap &= ~RULE_TYPE_ACTION_RED_DROP;
    action_p->action_bitmap &= ~RULE_TYPE_ACTION_YELLOW_DROP;
    action_p->action_bitmap &= ~RULE_TYPE_ACTION_DROP;

    if (RULE_TYPE_OK != RULE_OM_LocalNotify(RULE_TYPE_NOTIFY_ACTION_CHANGED, &action_index))
    {
        memcpy(action_p, &old_action, sizeof(*action_p));
        return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalSetClassActionToDefault
 *------------------------------------------------------------------------------
 * PURPOSE:  set class action to default value by action_index
 * INPUT:    action_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalSetClassActionToDefault(UI32_T action_index)
{
    RULE_TYPE_Action_T    *action_p;
    RULE_TYPE_Action_T    old_action;

    action_p = RULE_OM_LocalGetActionEntryByIndex(action_index);
    if (NULL == action_p)
        return RULE_TYPE_FAIL;

    memcpy(&old_action, action_p, sizeof(old_action));

    action_p->action_bitmap &= ~RULE_TYPE_ACTION_PKT_PRI;
    action_p->pkt_pri = 0;

    if (RULE_TYPE_OK != RULE_OM_LocalNotify(RULE_TYPE_NOTIFY_ACTION_CHANGED, &action_index))
    {
        memcpy(action_p, &old_action, sizeof(*action_p));
        return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalSetOutOfProfileActionToNull
 *------------------------------------------------------------------------------
 * PURPOSE:  set out-of-profile action to null value by action_index
 * INPUT:    action_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalSetOutOfProfileActionToNull(UI32_T action_index)
{
    RULE_TYPE_Action_T    *action_p;
    RULE_TYPE_Action_T    old_action;

    action_p = RULE_OM_LocalGetActionEntryByIndex(action_index);
    if (NULL == action_p)
        return RULE_TYPE_FAIL;

    memcpy(&old_action, action_p, sizeof(old_action));

    /* clear red action */
    action_p->action_bitmap &= ~(RULE_TYPE_OUT_OF_PROFILE_ACTION);

    action_p->green_pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;
    action_p->yellow_pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;
    action_p->red_pkt_new_dscp = RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP;

    if (RULE_TYPE_OK != RULE_OM_LocalNotify(RULE_TYPE_NOTIFY_ACTION_CHANGED, &action_index))
    {
        memcpy(action_p, &old_action, sizeof(*action_p));
        return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalNotify
 *------------------------------------------------------------------------------
 * PURPOSE:  Nofify event
 * INPUT:    message            - message
 *           param1             - A pointer to the identifier of the object
 *                                sending the message. Exactly value depend on
 *                                message.
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalNotify(
    RULE_TYPE_NOTIFY_MESSAGE_T message,
    void *param1)
{
    switch (message)
    {
        case RULE_TYPE_NOTIFY_METER_CHANGED:
        {
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
            UI32_T                  meter_index = *(UI32_T *)param1;
            UI32_T                  policy_map_index;
            RULE_TYPE_PolicyMap_T   *policy_map_p;

            policy_map_index = 0;
            while (NULL != (policy_map_p = RULE_OM_LocalGetNextPolicyMapByMeterIndex(meter_index, &policy_map_index)))
            {
                UI32_T ret;

                ret = RULE_OM_LocalRebuildBundleGroup(policy_map_p);

                if (RULE_TYPE_OK != ret)
                {
                    return ret;
                }
            }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */
            break;
        }

        case RULE_TYPE_NOTIFY_ACTION_CHANGED:
        {
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
            UI32_T action_index = *(UI32_T *)param1;
            UI32_T                  policy_map_index;
            RULE_TYPE_PolicyMap_T   *policy_map_p;

            policy_map_index = 0;
            while (NULL != (policy_map_p = RULE_OM_LocalGetNextPolicyMapByActionIndex(action_index, &policy_map_index)))
            {
                UI32_T ret;

                ret = RULE_OM_LocalRebuildBundleGroup(policy_map_p);

                if (RULE_TYPE_OK != ret)
                {
                    return ret;
                }
            }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */
            break;
        }

        default:
            LOG("Unknow message %lu", message);
            break;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_Is_AclType_Matched
 *------------------------------------------------------------------------------
 * PURPOSE  : Test if two acltypes are matched
 * INPUT    : src_type, dst_type
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_OM_Is_AclType_Matched(RULE_TYPE_AclType_T src_type, RULE_TYPE_AclType_T dst_type)
{
    switch (src_type)
    {
    case RULE_TYPE_IP_ACL:
    case RULE_TYPE_IP_STD_ACL:
    case RULE_TYPE_IP_EXT_ACL:
        src_type = RULE_TYPE_IP_ACL;
        break;

    case RULE_TYPE_IPV6_ACL:
    case RULE_TYPE_IPV6_STD_ACL:
    case RULE_TYPE_IPV6_EXT_ACL:
        src_type = RULE_TYPE_IPV6_ACL;
        break;

    default:
        break;
    }

    switch (dst_type)
    {
    case RULE_TYPE_IP_ACL:
    case RULE_TYPE_IP_STD_ACL:
    case RULE_TYPE_IP_EXT_ACL:
        dst_type = RULE_TYPE_IP_ACL;
        break;

    case RULE_TYPE_IPV6_ACL:
    case RULE_TYPE_IPV6_STD_ACL:
    case RULE_TYPE_IPV6_EXT_ACL:
        dst_type = RULE_TYPE_IPV6_ACL;
        break;

    default:
        break;
    }

    return (src_type == dst_type);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_LocalCountBitsSet
 *------------------------------------------------------------------------------
 * PURPOSE  : Counting bits set
 * INPUT    : bits set
 * OUTPUT   : none
 * RETURN   : how many bits are ON
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_OM_LocalCountBitsSet(UI32_T value)
{
    UI32_T count = 0;

    for(; value != 0; count++)
        value &= value - 1;

    return count;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_OM_LocalAclTypeToPortAclType(
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_AclType_T *port_acl_type_p)
{
    switch (acl_type)
    {
        case RULE_TYPE_MAC_ACL:
            *port_acl_type_p = RULE_TYPE_MAC_ACL;
            break;

        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            *port_acl_type_p = RULE_TYPE_IP_ACL;
            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            *port_acl_type_p = RULE_TYPE_IPV6_ACL;
            break;

        default:
            return RULE_TYPE_INVALID_PARAMETER;
    }

    return RULE_TYPE_OK;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_OM_LocalPrecedenceToPortAclType(
    UI32_T precedence,
    RULE_TYPE_AclType_T *port_acl_type_p)
{
    switch (precedence)
    {
        case INGRESS_MAC_ACL_PRECEDENCE:
        case EGRESS_MAC_ACL_PRECEDENCE:
            *port_acl_type_p = RULE_TYPE_MAC_ACL;
            break;

        case INGRESS_IP_ACL_PRECEDENCE:
        case EGRESS_IP_ACL_PRECEDENCE:
            *port_acl_type_p = RULE_TYPE_IP_ACL;
            break;

        case INGRESS_IPV6_ACL_PRECEDENCE:
        case EGRESS_IPV6_ACL_PRECEDENCE:
            *port_acl_type_p = RULE_TYPE_IPV6_ACL;
            break;

        default:
            return RULE_TYPE_INVALID_PARAMETER;
    }

    return RULE_TYPE_OK;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_OM_LocalPrecedenceToDirection(
    UI32_T precedence,
    RULE_TYPE_InOutDirection_T *direction_p)
{
    ASSERT(NULL != direction_p);

    switch (precedence)
    {
        case INGRESS_MAC_ACL_PRECEDENCE:
        case INGRESS_IP_ACL_PRECEDENCE:
        case INGRESS_IPV6_ACL_PRECEDENCE:
            *direction_p = RULE_TYPE_INBOUND;
            break;

        case EGRESS_MAC_ACL_PRECEDENCE:
        case EGRESS_IP_ACL_PRECEDENCE:
        case EGRESS_IPV6_ACL_PRECEDENCE:
            *direction_p = RULE_TYPE_OUTBOUND;
            break;

        default:
            return RULE_TYPE_INVALID_PARAMETER;
    }

    return RULE_TYPE_OK;
}

static void
RULE_OM_LocalValidateInstance(
    RULE_TYPE_INSTANCE_PTR_T in)
{
    UI32_T offset;

    switch (in->type)
    {
        case RULE_TYPE_INST_OM_USER_ACE:
        case RULE_TYPE_INST_OM_WORK_ACE:
            offset = L_CVRT_GET_OFFSET(shmem_data_p->ace_table, in);

            ASSERT(offset < sizeof(shmem_data_p->ace_table));
            break;

        case RULE_TYPE_INST_OM_USER_ACL:
        case RULE_TYPE_INST_OM_WORK_ACL:
            offset = L_CVRT_GET_OFFSET(shmem_data_p->acl_table, in);

            ASSERT(offset < sizeof(shmem_data_p->acl_table));
            break;

        case RULE_TYPE_NIL:
            break;

        default:
            ASSERT(0);
            break;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LocalIndexOf
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_LocalIndexOf(
    void *entry)
{
    RULE_TYPE_INSTANCE_PTR_T in = (RULE_TYPE_INSTANCE_PTR_T)entry;
    UI32_T offset;

    ASSERT(NULL != in);

    RULE_OM_VALIDATE_INST(in);

    switch (in->type)
    {
        case RULE_TYPE_INST_OM_USER_ACE:
        case RULE_TYPE_INST_OM_WORK_ACE:
            offset = L_CVRT_GET_OFFSET(shmem_data_p->ace_table, in);

            ASSERT(offset < sizeof(shmem_data_p->ace_table));
            ASSERT(0 == (offset % sizeof(shmem_data_p->ace_table[0])));
            return offset / sizeof(shmem_data_p->ace_table[0]);

        case RULE_TYPE_INST_OM_USER_ACL:
        case RULE_TYPE_INST_OM_WORK_ACL:
            offset = L_CVRT_GET_OFFSET(shmem_data_p->acl_table, in);

            ASSERT(offset < sizeof(shmem_data_p->acl_table));
            ASSERT(0 == (offset % sizeof(shmem_data_p->acl_table[0])));
            return offset / sizeof(shmem_data_p->acl_table[0]);

        default:
            ASSERT(0);
            break;
    }

    ASSERT(0);
    return 0xffffffff;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_LocalInst2Ptr
 *------------------------------------------------------------------------------
 * PURPOSE  : Convert instance to shared memory pointer
 * INPUT    : type - instance type
 *            in   - instance
 * OUTPUT   : None
 * RETURN   : shared memory pointer
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_SHM_POINTER_T
RULE_OM_LocalInst2Ptr(
    void *entry)
{
    RULE_TYPE_INSTANCE_PTR_T in = (RULE_TYPE_INSTANCE_PTR_T) entry;
    RULE_TYPE_SHM_POINTER_T ptr = {0};

    RULE_OM_VALIDATE_INST(in);

    switch (in->type)
    {
        case RULE_TYPE_INST_OM_USER_ACE:
        case RULE_TYPE_INST_OM_WORK_ACE:
            ptr.offset = L_CVRT_GET_OFFSET(shmem_data_p->ace_table, in);

            ASSERT(ptr.offset < sizeof(shmem_data_p->ace_table));
            break;

        case RULE_TYPE_INST_OM_USER_ACL:
        case RULE_TYPE_INST_OM_WORK_ACL:
            ptr.offset = L_CVRT_GET_OFFSET(shmem_data_p->acl_table, in);

            ASSERT(ptr.offset < sizeof(shmem_data_p->acl_table));
            break;

        default:
            ASSERT(0);
            break;
    }

    ptr.type = in->type;

    return ptr;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_LocalPtr2Inst
 *------------------------------------------------------------------------------
 * PURPOSE  : Convert shared memory pointer instance
 * INPUT    : ptr  - shared memory pointer
 * OUTPUT   : None
 * RETURN   : instance
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static void*
RULE_OM_LocalPtr2Inst(
    RULE_TYPE_SHM_POINTER_T ptr)
{
    switch (ptr.type)
    {
        case RULE_TYPE_INST_OM_USER_ACE:
        case RULE_TYPE_INST_OM_WORK_ACE:
            ASSERT(ptr.offset < sizeof(shmem_data_p->ace_table));
            return L_CVRT_GET_PTR(shmem_data_p->ace_table, ptr.offset);

        case RULE_TYPE_INST_OM_USER_ACL:
        case RULE_TYPE_INST_OM_WORK_ACL:
            ASSERT(ptr.offset < sizeof(shmem_data_p->acl_table));
            return L_CVRT_GET_PTR(shmem_data_p->acl_table, ptr.offset);

        case RULE_TYPE_NIL:
            break;

        default:
            ASSERT(0);
            break;
    }

    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LL_LocalInsertAfter
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
RULE_OM_LL_LocalInsertAfter(
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

    list = RULE_OM_PTR2INST(list_ptr);
    node = RULE_OM_PTR2INST(node_ptr);
    new_node = RULE_OM_PTR2INST(new_node_ptr);

    RULE_OM_VALIDATE_INST(list);
    RULE_OM_VALIDATE_INST(node);
    RULE_OM_VALIDATE_INST(new_node);

    node = RULE_OM_PTR2INST(node_ptr);
    new_node = RULE_OM_PTR2INST(new_node_ptr);

    new_node->links.prev = node_ptr;
    new_node->links.next = node->links.next;

    if (RULE_TYPE_NIL == node->links.next.type)
    {
        list->links.last_node = new_node_ptr;
    }
    else
    {
        RULE_TYPE_INSTANCE_T *next = RULE_OM_PTR2INST(node_ptr);

        RULE_OM_VALIDATE_INST(next);

        next->links.prev = new_node_ptr;
    }

    node->links.next = new_node_ptr;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LL_LocalInsertBefore
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
RULE_OM_LL_LocalInsertBefore(
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

    list = RULE_OM_PTR2INST(list_ptr);
    node = RULE_OM_PTR2INST(node_ptr);
    new_node = RULE_OM_PTR2INST(new_node_ptr);

    RULE_OM_VALIDATE_INST(list);
    RULE_OM_VALIDATE_INST(node);
    RULE_OM_VALIDATE_INST(new_node);

    node = RULE_OM_PTR2INST(node_ptr);
    new_node = RULE_OM_PTR2INST(new_node_ptr);

    new_node->links.prev = node->links.prev;
    new_node->links.next = node_ptr;

    if (RULE_TYPE_NIL == node->links.prev.type)
    {
        list->links.first_node = new_node_ptr;
    }
    else
    {
        RULE_TYPE_INSTANCE_T *prev = RULE_OM_PTR2INST(node->links.prev);

        RULE_OM_VALIDATE_INST(prev);
        prev->links.next = new_node_ptr;
    }

    node->links.prev = new_node_ptr;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LL_LocalInsertBeginning
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
RULE_OM_LL_LocalInsertBeginning(
    RULE_TYPE_SHM_POINTER_T list_ptr,
    RULE_TYPE_SHM_POINTER_T new_node_ptr)
{
    RULE_TYPE_INSTANCE_T *list;
    RULE_TYPE_INSTANCE_T *new_node;

    ASSERT(RULE_TYPE_NIL != list_ptr.type);
    ASSERT(RULE_TYPE_NIL != new_node_ptr.type);

    list = RULE_OM_PTR2INST(list_ptr);
    new_node = RULE_OM_PTR2INST(new_node_ptr);

    RULE_OM_VALIDATE_INST(list);
    RULE_OM_VALIDATE_INST(new_node);

    new_node = RULE_OM_PTR2INST(new_node_ptr);

    if (RULE_TYPE_NIL == list->links.first_node.type)
    {
        list->links.first_node = new_node_ptr;
        list->links.last_node  = new_node_ptr;

        memset(&new_node->links.prev, 0, sizeof(new_node->links.prev));
        memset(&new_node->links.next, 0, sizeof(new_node->links.next));
    }
    else
    {
        RULE_OM_LL_LocalInsertBefore(list_ptr, list->links.first_node, new_node_ptr);
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LL_LocalInsertEnd
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
RULE_OM_LL_LocalInsertEnd(
    RULE_TYPE_SHM_POINTER_T list_ptr,
    RULE_TYPE_SHM_POINTER_T new_node_ptr)
{
    RULE_TYPE_INSTANCE_T *list;
    RULE_TYPE_INSTANCE_T *new_node;

    ASSERT(RULE_TYPE_NIL != list_ptr.type);
    ASSERT(RULE_TYPE_NIL != new_node_ptr.type);

    list = RULE_OM_PTR2INST(list_ptr);
    new_node = RULE_OM_PTR2INST(new_node_ptr);

    RULE_OM_VALIDATE_INST(list);
    RULE_OM_VALIDATE_INST(new_node);

    new_node = RULE_OM_PTR2INST(new_node_ptr);

    if (RULE_TYPE_NIL == list->links.last_node.type)
    {
        RULE_OM_LL_LocalInsertBeginning(list_ptr, new_node_ptr);
    }
    else
    {
        RULE_OM_LL_LocalInsertAfter(list_ptr, list->links.last_node, new_node_ptr);
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_LL_LocalRemove
 *------------------------------------------------------------------------------
 * PURPOSE  : Remove node
 * INPUT    : list_ptr      - list
 *            new_node_ptr  - new node
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_OM_LL_LocalRemove(
    RULE_TYPE_SHM_POINTER_T list_ptr,
    RULE_TYPE_SHM_POINTER_T node_ptr)
{
    RULE_TYPE_INSTANCE_T *list;
    RULE_TYPE_INSTANCE_T *node;

    ASSERT(RULE_TYPE_NIL != list_ptr.type);
    ASSERT(RULE_TYPE_NIL != node_ptr.type);

    list = RULE_OM_PTR2INST(list_ptr);
    node = RULE_OM_PTR2INST(node_ptr);

    RULE_OM_VALIDATE_INST(list);
    RULE_OM_VALIDATE_INST(node);

    if (RULE_TYPE_NIL == node->links.prev.type)
    {
        ASSERT(RULE_OM_PTR2INST(list->links.first_node) == node);
        list->links.first_node = node->links.next;
    }
    else
    {
        RULE_TYPE_INSTANCE_T *prev = RULE_OM_PTR2INST(node->links.prev);

        RULE_OM_VALIDATE_INST(prev);

        ASSERT(RULE_OM_PTR2INST(prev->links.next) == node);
        prev->links.next = node->links.next;
    }

    if (RULE_TYPE_NIL == node->links.next.type)
    {
        ASSERT(RULE_OM_PTR2INST(list->links.last_node) == node);
        list->links.last_node = node->links.prev;
    }
    else
    {
        RULE_TYPE_INSTANCE_T *next = RULE_OM_PTR2INST(node->links.next);

        RULE_OM_VALIDATE_INST(next);

        ASSERT(RULE_OM_PTR2INST(next->links.prev) == node);
        next->links.prev = node->links.prev;
    }

    memset(&node->links.prev, 0, sizeof(node->links.prev));
    memset(&node->links.next, 0, sizeof(node->links.next));
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_ConvertUIAce2Ace
 *------------------------------------------------------------------------------
 * PURPOSE  : Convert RULE_TYPE_UI_Ace_Entry_T to RULE_TYPE_Ace_Entry_T
 * INPUT    : ui_ace_p
 * OUTPUT   : ace_p
 * RETURN   : RULE_TYPE_RETURN_TYPE_T
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_ConvertUIAce2Ace(
    const RULE_TYPE_UI_Ace_Entry_T *ui_ace_p,
    RULE_TYPE_Ace_Entry_T *ace_p)
{
    UI32_T ethertype;

    ASSERT(NULL != ui_ace_p);
    ASSERT(NULL != ace_p);

    if (NULL == ui_ace_p ||
        NULL == ace_p)
    {
        return RULE_TYPE_FAIL;
    }

    /* Init */
    ace_p->ace_type = ui_ace_p->ace_type;
    RULE_OM_InitAce(ace_p);

    ace_p->access = ui_ace_p->access;
    ace_p->row_status = ui_ace_p->row_status;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    strncpy((char *)ace_p->time_range_name,
            (char *)ui_ace_p->time_range_name,
            SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
    ace_p->time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH] = '\0';
    ace_p->time_range_index = ui_ace_p->time_range_index;
#endif /* SYS_CPNT_TIME_BASED_ACL */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
    ace_p->counter = ui_ace_p->counter;
#endif /* SYS_CPNT_ACL_COUNTER */

#if (SYS_CPNT_DAI == TRUE)
    /* Only for ARP ace */
    if (ui_ace_p->ace_type == RULE_TYPE_ARP_ACL)
    {
        ace_p->u.arp.aceArpType = ui_ace_p->arp.arp_type;
        ace_p->u.arp.aceIsLog = ui_ace_p->arp.is_log;

        if (!RULE_OM_IS_ZERO_MAC(ui_ace_p->arp.sender.mac.mask) ||
            !RULE_OM_IS_ZERO_MAC(ui_ace_p->arp.sender.mac.addr))
        {
            memcpy(ace_p->u.arp.aceSenderMacAddr,
                   ui_ace_p->arp.sender.mac.addr,
                   sizeof(ace_p->u.arp.aceSenderMacAddr));

            memcpy(ace_p->u.arp.aceSenderMacAddrBitmask,
                   ui_ace_p->arp.sender.mac.mask,
                   sizeof(ace_p->u.arp.aceSenderMacAddrBitmask));
        }

        if (!RULE_OM_IS_ZERO_MAC(ui_ace_p->arp.target.mac.mask) ||
            !RULE_OM_IS_ZERO_MAC(ui_ace_p->arp.target.mac.addr))
        {
            memcpy(ace_p->u.arp.aceTargetMacAddr,
                   ui_ace_p->arp.target.mac.addr,
                   sizeof(ace_p->u.arp.aceTargetMacAddr));

            memcpy(ace_p->u.arp.aceTargetMacAddrBitmask,
                   ui_ace_p->arp.target.mac.mask,
                   sizeof(ace_p->u.arp.aceTargetMacAddrBitmask));
        }

        if (!RULE_OM_IS_ZERO_IPV4_ADDR(ui_ace_p->arp.sender.ip.mask) ||
            !RULE_OM_IS_ZERO_IPV4_ADDR(ui_ace_p->arp.sender.ip.addr))
        {
            UI32_T *ip_addr = (UI32_T *)& ui_ace_p->arp.sender.ip.addr;
            UI32_T *ip_mask = (UI32_T *)& ui_ace_p->arp.sender.ip.mask;

            ace_p->u.arp.aceSenderIpAddr = *ip_addr;
            ace_p->u.arp.aceSenderIpAddrBitmask = *ip_mask;
        }

        if (!RULE_OM_IS_ZERO_IPV4_ADDR(ui_ace_p->arp.target.ip.mask) ||
            !RULE_OM_IS_ZERO_IPV4_ADDR(ui_ace_p->arp.target.ip.addr))
        {
            UI32_T *ip_addr = (UI32_T *)& ui_ace_p->arp.target.ip.addr;
            UI32_T *ip_mask = (UI32_T *)& ui_ace_p->arp.target.ip.mask;

            ace_p->u.arp.aceTargetIpAddr = *ip_addr;
            ace_p->u.arp.aceTargetIpAddrBitmask = *ip_mask;
        }

        return RULE_TYPE_OK;
    }
#endif /* SYS_CPNT_DAI */

    ethertype = 0;

    /* packet format */
    if (VAL_aclMacAcePktformat_any != ui_ace_p->pkt_format)
    {
        ace_p->u.mac.acePktformat = ui_ace_p->pkt_format;
    }

    /* SA, DA */
    if (FALSE == RULE_OM_IS_ZERO_MAC(ui_ace_p->ether.sa.mask))
    {
        memcpy(ace_p->u.mac.aceSourceMacAddr,
               ui_ace_p->ether.sa.addr,
               sizeof(ace_p->u.mac.aceSourceMacAddr));

        memcpy(ace_p->u.mac.aceSourceMacAddrBitmask,
               ui_ace_p->ether.sa.mask,
               sizeof(ace_p->u.mac.aceSourceMacAddrBitmask));
    }

    if (FALSE == RULE_OM_IS_ZERO_MAC(ui_ace_p->ether.da.mask))
    {
        memcpy(ace_p->u.mac.aceDestMacAddr,
               ui_ace_p->ether.da.addr,
               sizeof(ace_p->u.mac.aceDestMacAddr));

        memcpy(ace_p->u.mac.aceDestMacAddrBitmask,
               ui_ace_p->ether.da.mask,
               sizeof(ace_p->u.mac.aceDestMacAddrBitmask));
    }

    /* vid */
    if (VAL_diffServMacAceVidOp_equal == ui_ace_p->ether.vid.op)
    {
        ace_p->u.mac.aceVidOp = VAL_diffServMacAceVidOp_equal;
        ace_p->u.mac.aceMinVid = ui_ace_p->ether.vid.u.s.data;
        ace_p->u.mac.aceVidBitmask = ui_ace_p->ether.vid.u.s.mask;
    }
    else if (VAL_diffServMacAceVidOp_range == ui_ace_p->ether.vid.op)
    {
        ace_p->u.mac.aceVidOp = VAL_diffServMacAceVidOp_range;
        ace_p->u.mac.aceMinVid = ui_ace_p->ether.vid.u.range.min;
        ace_p->u.mac.aceMaxVid = ui_ace_p->ether.vid.u.range.max;
    }

    /* cos */
    if (VAL_diffServMacAceCosOp_equal == ui_ace_p->ether.cos.op)
    {
        ace_p->u.mac.aceCosOp = VAL_diffServMacAceCosOp_equal;
        ace_p->u.mac.aceMinCos = ui_ace_p->ether.cos.u.s.data;
        ace_p->u.mac.aceCosBitmask = ui_ace_p->ether.cos.u.s.mask;
    }
    else if (VAL_diffServMacAceSourcePortOp_range == ui_ace_p->ether.cos.op)
    {
        ace_p->u.mac.aceCosOp = VAL_diffServMacAceSourcePortOp_range;
        ace_p->u.mac.aceMinCos = ui_ace_p->ether.cos.u.range.min;
        ace_p->u.mac.aceMaxCos = ui_ace_p->ether.cos.u.range.max;
    }

    /* ethertype */
    if (VAL_diffServMacAceEtherTypeOp_equal == ui_ace_p->ether.ethertype.op)
    {
        ethertype = ui_ace_p->ether.ethertype.u.s.data;

        ace_p->u.mac.aceEtherTypeOp = VAL_diffServMacAceEtherTypeOp_equal;
        ace_p->u.mac.aceMinEtherType = ethertype;
        ace_p->u.mac.aceEtherTypeBitmask = MAX_diffServMacAceEtherTypeBitmask;

        if (ui_ace_p->ether.ethertype.u.s.mask == MAX_diffServMacAceEtherTypeBitmask)
        {
#if (RULE_TYPE_ACE_MAC_IP_MIX == TRUE)
            /* L3 init,
             */
            switch (ethertype)
            {
                case RULE_TYPE_ETHERTYPE_IP:
                    RULE_OM_SetAcePatternToDefaultByType(ace_p, RULE_TYPE_IP_ACL);
                    break;
                case RULE_TYPE_ETHERTYPE_IPV6:
                    RULE_OM_SetAcePatternToDefaultByType(ace_p, RULE_TYPE_IPV6_ACL);
                    break;
                default:
                    break;
            }
#endif /* RULE_TYPE_ACE_MAC_IP_MIX */
        }
        else
        {
            ace_p->u.mac.aceEtherTypeBitmask = ui_ace_p->ether.ethertype.u.s.mask;

            /* not convert l4 port */
            ethertype = 0;
        }
    }
    else if (VAL_diffServMacAceEtherTypeOp_range == ui_ace_p->ether.ethertype.op)
    {
        ace_p->u.mac.aceEtherTypeOp = VAL_diffServMacAceEtherTypeOp_range;
        ace_p->u.mac.aceMinEtherType = ui_ace_p->ether.ethertype.u.range.min;
        ace_p->u.mac.aceMaxEtherType = ui_ace_p->ether.ethertype.u.range.max;
    }

    /* IPv4 */
    if (FALSE == RULE_OM_IS_ZERO_IPV4_ADDR(ui_ace_p->ipv4.sip.mask))
    {
        /* net byte order
         */
        UI32_T *ip_addr = (UI32_T *) &ui_ace_p->ipv4.sip.addr;
        UI32_T *ip_mask = (UI32_T *) &ui_ace_p->ipv4.sip.mask;

        ace_p->u.ip.aceSourceIpAddr = *ip_addr;
        ace_p->u.ip.aceSourceIpAddrBitmask = *ip_mask;
    }

    if (FALSE == RULE_OM_IS_ZERO_IPV4_ADDR(ui_ace_p->ipv4.dip.mask))
    {
        /* net byte order
         */
        UI32_T *ip_addr = (UI32_T *) &ui_ace_p->ipv4.dip.addr;
        UI32_T *ip_mask = (UI32_T *) &ui_ace_p->ipv4.dip.mask;

        ace_p->u.ip.aceDestIpAddr = *ip_addr;
        ace_p->u.ip.aceDestIpAddrBitmask = *ip_mask;
    }

    if (RULE_TYPE_IPV4_PROTOCOL_OP_EQUAL == ui_ace_p->ipv4.protocol.op)
    {
        ace_p->u.ip.aceProtocol = ui_ace_p->ipv4.protocol.u.s.data;
    }

    /* DSCP/ToS */
    switch (ui_ace_p->ipv4.dscp.op)
    {
        case RULE_TYPE_IPV4_DSCP_OP_DSCP:
            ace_p->u.ip.aceDscp = ui_ace_p->ipv4.dscp.u.ds;
            break;
        case RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE:
            ace_p->u.ip.acePrec = ui_ace_p->ipv4.dscp.u.tos.precedence;
            break;
        case RULE_TYPE_IPV4_DSCP_OP_TOS:
            ace_p->u.ip.aceTos = ui_ace_p->ipv4.dscp.u.tos.tos;
            break;
        case RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE_AND_TOS:
            ace_p->u.ip.acePrec = ui_ace_p->ipv4.dscp.u.tos.precedence;
            ace_p->u.ip.aceTos = ui_ace_p->ipv4.dscp.u.tos.tos;
            break;
        case RULE_TYPE_IPV4_DSCP_OP_NO_OPERATION:
        default:
            break;
    }

    /* IPv6 */
    if (0 != ui_ace_p->ipv6.sip.prefix_len)
    {
        RULE_TYPE_RETURN_TYPE_T result;

        memcpy(ace_p->u.ipv6.aceSourceIpAddr,
               ui_ace_p->ipv6.sip.addr,
               sizeof(ace_p->u.ipv6.aceSourceIpAddr));

        result = RULE_OM_LocalConvertIpv6Prefix2Mask(ui_ace_p->ipv6.sip.prefix_len,
                                                     ace_p->u.ipv6.aceSourceIpAddrBitmask);
        ASSERT(result == RULE_TYPE_OK);
    }

    if (0 != ui_ace_p->ipv6.dip.prefix_len)
    {
        RULE_TYPE_RETURN_TYPE_T result;

        memcpy(ace_p->u.ipv6.aceDestIpAddr,
               ui_ace_p->ipv6.dip.addr,
               sizeof(ace_p->u.ipv6.aceDestIpAddr));

        result = RULE_OM_LocalConvertIpv6Prefix2Mask(ui_ace_p->ipv6.dip.prefix_len,
                                                     ace_p->u.ipv6.aceDestIpAddrBitmask);
        ASSERT(result == RULE_TYPE_OK);
    }

    if (RULE_TYPE_IPV6_TRAFFIC_CLASS_OP_EQUAL == ui_ace_p->ipv6.traffic_class.op)
    {
        ace_p->u.ipv6.aceDscp = ui_ace_p->ipv6.traffic_class.u.s.data >> 2;
    }

    if (RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL == ui_ace_p->ipv6.next_header.op)
    {
        ace_p->u.ipv6.aceNextHeader = ui_ace_p->ipv6.next_header.u.s.data;
    }

    if (RULE_TYPE_IPV6_FLOW_LABEL_OP_EQUAL == ui_ace_p->ipv6.flow_label.op)
    {
        ace_p->u.ipv6.aceFlowLabel = ui_ace_p->ipv6.flow_label.u.s.data;
    }

    /* ICMP */
    if (RULE_TYPE_ICMP_ICMPTYPE_OP_EQUAL == ui_ace_p->icmp.icmp_type.op)
    {
        ace_p->u.ip.aceIcmpType = ui_ace_p->icmp.icmp_type.u.s.data;
    }

    if (RULE_TYPE_ICMP_ICMPTYPE_OP_EQUAL == ui_ace_p->icmp.icmp_type.op)
    {
        ace_p->u.ip.aceIcmpType = ui_ace_p->icmp.icmp_type.u.s.data;
    }

    /* tcp */
    if (MIN_diffServIpAceControlCodeBitmask != ui_ace_p->tcp.flags.mask.u.code ||
        RULE_TYPE_UNDEF_IP_CONTROL_CODE != ui_ace_p->tcp.flags.data.u.code)
    {
        ace_p->u.ip.aceControlCode = ui_ace_p->tcp.flags.data.u.code;
        ace_p->u.ip.aceControlCodeBitmask = ui_ace_p->tcp.flags.mask.u.code;
    }

    /* l4-port */
    /* MAC+IP/IPv6 ACL is already set the ethetype */
    if (ui_ace_p->ace_type == RULE_TYPE_IP_EXT_ACL)
    {
        ethertype = RULE_TYPE_ETHERTYPE_IP;
    }
    else if (ui_ace_p->ace_type == RULE_TYPE_IPV6_EXT_ACL)
    {
        ethertype = RULE_TYPE_ETHERTYPE_IPV6;
    }

    switch (ethertype)
    {
        case RULE_TYPE_ETHERTYPE_IP:
            if (VAL_diffServIpAceSourcePortOp_equal == ui_ace_p->l4_common.sport.op)
            {
                ace_p->u.ip.aceSourcePortOp = VAL_diffServIpAceSourcePortOp_equal;
                ace_p->u.ip.aceMinSourcePort = ui_ace_p->l4_common.sport.u.s.data;
                ace_p->u.ip.aceSourcePortBitmask = ui_ace_p->l4_common.sport.u.s.mask;
            }
            else if (VAL_diffServIpAceSourcePortOp_range == ui_ace_p->l4_common.sport.op)
            {
                ace_p->u.ip.aceSourcePortOp = VAL_diffServIpAceSourcePortOp_range;
                ace_p->u.ip.aceMinSourcePort = ui_ace_p->l4_common.sport.u.range.min;
                ace_p->u.ip.aceMaxSourcePort = ui_ace_p->l4_common.sport.u.range.max;
            }

            if (VAL_diffServIpAceDestPortOp_equal == ui_ace_p->l4_common.dport.op)
            {
                ace_p->u.ip.aceDestPortOp = VAL_diffServIpAceDestPortOp_equal;
                ace_p->u.ip.aceMinDestPort = ui_ace_p->l4_common.dport.u.s.data;
                ace_p->u.ip.aceDestPortBitmask = ui_ace_p->l4_common.dport.u.s.mask;
            }
            else if (VAL_diffServIpAceDestPortOp_range == ui_ace_p->l4_common.dport.op)
            {
                ace_p->u.ip.aceDestPortOp = VAL_diffServIpAceDestPortOp_range;
                ace_p->u.ip.aceMinDestPort = ui_ace_p->l4_common.dport.u.range.min;
                ace_p->u.ip.aceMaxDestPort = ui_ace_p->l4_common.dport.u.range.max;
            }

            break;
        case RULE_TYPE_ETHERTYPE_IPV6:
            if (VAL_diffServIpAceSourcePortOp_equal == ui_ace_p->l4_common.sport.op)
            {
                ace_p->u.ipv6.aceSourcePortOp = VAL_diffServIpAceSourcePortOp_equal;
                ace_p->u.ipv6.aceMinSourcePort = ui_ace_p->l4_common.sport.u.s.data;
                ace_p->u.ipv6.aceSourcePortBitmask = ui_ace_p->l4_common.sport.u.s.mask;
            }
            else if (VAL_diffServIpAceSourcePortOp_range == ui_ace_p->l4_common.sport.op)
            {
                ace_p->u.ipv6.aceSourcePortOp = VAL_diffServIpAceSourcePortOp_range;
                ace_p->u.ipv6.aceMinSourcePort = ui_ace_p->l4_common.sport.u.range.min;
                ace_p->u.ipv6.aceMaxSourcePort = ui_ace_p->l4_common.sport.u.range.max;
            }

            if (VAL_diffServIpAceDestPortOp_equal == ui_ace_p->l4_common.dport.op)
            {
                ace_p->u.ipv6.aceDestPortOp = VAL_diffServIpAceDestPortOp_equal;
                ace_p->u.ipv6.aceMinDestPort = ui_ace_p->l4_common.dport.u.s.data;
                ace_p->u.ipv6.aceDestPortBitmask = ui_ace_p->l4_common.dport.u.s.mask;
            }
            else if (VAL_diffServIpAceDestPortOp_range == ui_ace_p->l4_common.dport.op)
            {
                ace_p->u.ipv6.aceDestPortOp = VAL_diffServIpAceDestPortOp_range;
                ace_p->u.ipv6.aceMinDestPort = ui_ace_p->l4_common.dport.u.range.min;
                ace_p->u.ipv6.aceMaxDestPort = ui_ace_p->l4_common.dport.u.range.max;
            }

            break;
        default:
            break;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_ConvertAce2UIAce
 *------------------------------------------------------------------------------
 * PURPOSE  : Convert RULE_TYPE_Ace_Entry_T to RULE_TYPE_UI_Ace_Entry_T
 * INPUT    : ace_p
 * OUTPUT   : ui_ace_p
 * RETURN   : RULE_TYPE_RETURN_TYPE_T
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_ConvertAce2UIAce(
    const RULE_TYPE_Ace_Entry_T *ace_p,
    RULE_TYPE_UI_Ace_Entry_T *ui_ace_p)
{
#define MAC_QUALIFY  0x01
#define IPV4_QUALIFY 0x02
#define IPV6_QUALIFY 0x04

    UI8_T ace_qualify = 0;

    ASSERT(NULL != ui_ace_p);
    ASSERT(NULL != ace_p);

    ui_ace_p->ace_type = ace_p->ace_type;
    RULE_OM_InitUIAce(ui_ace_p);

    ui_ace_p->access = ace_p->access;
    ui_ace_p->row_status = ace_p->row_status;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    strncpy((char *)ui_ace_p->time_range_name,
            (char *)ace_p->time_range_name,
            SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
    ui_ace_p->time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH] = '\0';
    ui_ace_p->time_range_index = ace_p->time_range_index;
#endif /* SYS_CPNT_TIME_BASED_ACL */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
    ui_ace_p->counter = ace_p->counter;
#endif /* SYS_CPNT_ACL_COUNTER */

    switch (ace_p->ace_type)
    {
        case RULE_TYPE_MAC_ACL:
            ace_qualify |= MAC_QUALIFY;
#if (RULE_TYPE_ACE_MAC_IP_MIX == TRUE)
            if ((ace_p->u.mac.aceEtherTypeOp == VAL_diffServMacAceEtherTypeOp_equal) &&
                (ace_p->u.mac.aceEtherTypeBitmask == MAX_diffServMacAceMinEtherType))
            {
                switch (ace_p->u.mac.aceMinEtherType)
                {
                    case RULE_TYPE_ETHERTYPE_IP:
                        ace_qualify |= IPV4_QUALIFY;
                        break;
                    case RULE_TYPE_ETHERTYPE_IPV6:
                        ace_qualify |= IPV6_QUALIFY;
                        break;
                    default:
                        break;
                }
            }
#endif /* RULE_TYPE_ACE_MAC_IP_MIX */
            break;

        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            ace_qualify |= IPV4_QUALIFY;

#if (RULE_TYPE_ACE_MAC_IP_MIX == TRUE)
            ace_qualify |= MAC_QUALIFY;
#endif /* RULE_TYPE_ACE_MAC_IP_MIX */
            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            ace_qualify |= IPV6_QUALIFY;

#if (RULE_TYPE_ACE_MAC_IP_MIX == TRUE)
            ace_qualify |= MAC_QUALIFY;
#endif /* RULE_TYPE_ACE_MAC_IP_MIX */
            break;

#if (SYS_CPNT_DAI == TRUE)
        case RULE_TYPE_ARP_ACL:
            /* Only for ARP ace */
            ui_ace_p->arp.arp_type = ace_p->u.arp.aceArpType;
            ui_ace_p->arp.is_log = ace_p->u.arp.aceIsLog;

            if (!RULE_OM_IS_ZERO_MAC(ace_p->u.arp.aceSenderMacAddrBitmask) ||
                !RULE_OM_IS_ZERO_MAC(ace_p->u.arp.aceSenderMacAddr))
            {
                memcpy(ui_ace_p->arp.sender.mac.addr,
                       ace_p->u.arp.aceSenderMacAddr,
                       sizeof(ui_ace_p->arp.sender.mac.addr));

                memcpy(ui_ace_p->arp.sender.mac.mask,
                       ace_p->u.arp.aceSenderMacAddrBitmask,
                       sizeof(ui_ace_p->arp.sender.mac.mask));
            }

            if (!RULE_OM_IS_ZERO_MAC(ace_p->u.arp.aceTargetMacAddrBitmask) ||
                !RULE_OM_IS_ZERO_MAC(ace_p->u.arp.aceTargetMacAddr))
            {
                memcpy(ui_ace_p->arp.target.mac.addr,
                       ace_p->u.arp.aceTargetMacAddr,
                       sizeof(ui_ace_p->arp.target.mac.addr));

                memcpy(ui_ace_p->arp.target.mac.mask,
                       ace_p->u.arp.aceTargetMacAddrBitmask,
                       sizeof(ui_ace_p->arp.target.mac.mask));
            }

            if (0 != ace_p->u.arp.aceSenderIpAddrBitmask ||
                0 != ace_p->u.arp.aceSenderIpAddr)
            {
                UI8_T *ip_addr =(UI8_T *) &ace_p->u.arp.aceSenderIpAddr;
                UI8_T *ip_mask =(UI8_T *) &ace_p->u.arp.aceSenderIpAddrBitmask;

                memcpy(ui_ace_p->arp.sender.ip.addr, ip_addr,
                       sizeof(ui_ace_p->arp.sender.ip.addr));
                memcpy(ui_ace_p->arp.sender.ip.mask, ip_mask,
                       sizeof(ui_ace_p->arp.sender.ip.mask));
            }

            if (0 != ace_p->u.arp.aceTargetIpAddrBitmask ||
                0 != ace_p->u.arp.aceTargetIpAddr)
            {
                memcpy(ui_ace_p->arp.target.ip.addr,
                       &ace_p->u.arp.aceTargetIpAddr,
                       sizeof(ui_ace_p->arp.target.ip.addr));
                memcpy(ui_ace_p->arp.target.ip.mask,
                       &ace_p->u.arp.aceTargetIpAddrBitmask,
                       sizeof(ui_ace_p->arp.target.ip.mask));
            }
            return RULE_TYPE_OK;
            break;
#endif /* SYS_CPNT_DAI */

        default:
            return RULE_TYPE_FAIL;
    }

    if (ace_qualify & MAC_QUALIFY)
    {
        if (FALSE == RULE_OM_IS_ZERO_MAC(ace_p->u.mac.aceSourceMacAddrBitmask))
        {
            memcpy(ui_ace_p->ether.sa.addr,
                   ace_p->u.mac.aceSourceMacAddr,
                   sizeof(ui_ace_p->ether.sa.addr));

            memcpy(ui_ace_p->ether.sa.mask,
                   ace_p->u.mac.aceSourceMacAddrBitmask,
                   sizeof(ui_ace_p->ether.sa.mask));
        }

        if (FALSE == RULE_OM_IS_ZERO_MAC(ace_p->u.mac.aceDestMacAddrBitmask))
        {
            memcpy(ui_ace_p->ether.da.addr,
                   ace_p->u.mac.aceDestMacAddr,
                   sizeof(ui_ace_p->ether.da.addr));

            memcpy(ui_ace_p->ether.da.mask,
                   ace_p->u.mac.aceDestMacAddrBitmask,
                   sizeof(ui_ace_p->ether.da.mask));
        }

        /* packet format */
        if (VAL_aclMacAcePktformat_any != ace_p->u.mac.acePktformat)
        {
            ui_ace_p->pkt_format = ace_p->u.mac.acePktformat;
        }

        /* vid */
        if (VAL_diffServMacAceVidOp_equal == ace_p->u.mac.aceVidOp)
        {
            ui_ace_p->ether.vid.op = VAL_diffServMacAceVidOp_equal;
            ui_ace_p->ether.vid.u.s.data = ace_p->u.mac.aceMinVid;
            ui_ace_p->ether.vid.u.s.mask = ace_p->u.mac.aceVidBitmask;
        }
        else if (VAL_diffServMacAceVidOp_range == ace_p->u.mac.aceVidOp)
        {
            ui_ace_p->ether.vid.op = VAL_diffServMacAceVidOp_range;
            ui_ace_p->ether.vid.u.range.min = ace_p->u.mac.aceMinVid;
            ui_ace_p->ether.vid.u.range.max = ace_p->u.mac.aceMaxVid;
        }

        /* cos */
        if (VAL_diffServMacAceCosOp_equal == ace_p->u.mac.aceCosOp)
        {
            ui_ace_p->ether.cos.op = VAL_diffServMacAceCosOp_equal;
            ui_ace_p->ether.cos.u.s.data = ace_p->u.mac.aceMinCos;
            ui_ace_p->ether.cos.u.s.mask = ace_p->u.mac.aceCosBitmask;
        }
        else if (VAL_diffServMacAceCosOp_range == ace_p->u.mac.aceCosOp)
        {
            ui_ace_p->ether.cos.op = VAL_diffServMacAceCosOp_range;
            ui_ace_p->ether.cos.u.range.min = ace_p->u.mac.aceMinCos;
            ui_ace_p->ether.cos.u.range.max = ace_p->u.mac.aceMaxCos;
        }

        /* ethertype */
        if (VAL_diffServMacAceEtherTypeOp_equal == ace_p->u.mac.aceEtherTypeOp)
        {
            ui_ace_p->ether.ethertype.op = VAL_diffServMacAceEtherTypeOp_equal;
            ui_ace_p->ether.ethertype.u.s.data = ace_p->u.mac.aceMinEtherType;
            ui_ace_p->ether.ethertype.u.s.mask = ace_p->u.mac.aceEtherTypeBitmask;
        }
        else if (VAL_diffServMacAceEtherTypeOp_range == ace_p->u.mac.aceEtherTypeOp)
        {
            ui_ace_p->ether.ethertype.op = VAL_diffServMacAceEtherTypeOp_range;
            ui_ace_p->ether.ethertype.u.range.min = ace_p->u.mac.aceMinEtherType;
            ui_ace_p->ether.ethertype.u.range.max = ace_p->u.mac.aceMaxEtherType;
        }
    }

    if (ace_qualify & IPV4_QUALIFY)
    {
        if (0 != ace_p->u.ip.aceSourceIpAddrBitmask)
        {
            memcpy(ui_ace_p->ipv4.sip.addr,
                   &ace_p->u.ip.aceSourceIpAddr,
                   sizeof(ui_ace_p->ipv4.sip.addr));
            memcpy(ui_ace_p->ipv4.sip.mask,
                   &ace_p->u.ip.aceSourceIpAddrBitmask,
                   sizeof(ui_ace_p->ipv4.sip.mask));
        }

        if (0 != ace_p->u.ip.aceDestIpAddrBitmask)
        {
            memcpy(ui_ace_p->ipv4.dip.addr,
                   &ace_p->u.ip.aceDestIpAddr,
                   sizeof(ui_ace_p->ipv4.dip.addr));
            memcpy(ui_ace_p->ipv4.dip.mask,
                   &ace_p->u.ip.aceDestIpAddrBitmask,
                   sizeof(ui_ace_p->ipv4.dip.mask));
        }

        if (RULE_TYPE_UNDEF_IP_PROTOCOL != ace_p->u.ip.aceProtocol)
        {
            ui_ace_p->ipv4.protocol.op = RULE_TYPE_IPV4_PROTOCOL_OP_EQUAL;
            ui_ace_p->ipv4.protocol.u.s.data = ace_p->u.ip.aceProtocol;
            ui_ace_p->ipv4.protocol.u.s.mask = MAX_aclIpAceProtocol - 1;
        }

        if (RULE_TYPE_UNDEF_IP_PRECEDENCE != ace_p->u.ip.acePrec)
        {
            if (RULE_TYPE_UNDEF_IP_TOS != ace_p->u.ip.aceTos)
            {
                ui_ace_p->ipv4.dscp.op = RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE_AND_TOS;
            }
            else
            {
                ui_ace_p->ipv4.dscp.op = RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE;
            }

            ui_ace_p->ipv4.dscp.u.tos.precedence = ace_p->u.ip.acePrec;
        }

        if (RULE_TYPE_UNDEF_IP_TOS != ace_p->u.ip.aceTos)
        {
            if (RULE_TYPE_UNDEF_IP_PRECEDENCE != ace_p->u.ip.acePrec)
            {
                ui_ace_p->ipv4.dscp.op = RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE_AND_TOS;
            }
            else
            {
                ui_ace_p->ipv4.dscp.op = RULE_TYPE_IPV4_DSCP_OP_TOS;
            }

            ui_ace_p->ipv4.dscp.u.tos.tos = ace_p->u.ip.aceTos;
        }

        if (RULE_TYPE_UNDEF_IP_DSCP != ace_p->u.ip.aceDscp)
        {
            ui_ace_p->ipv4.dscp.op = RULE_TYPE_IPV4_DSCP_OP_DSCP;
            ui_ace_p->ipv4.dscp.u.ds = ace_p->u.ip.aceDscp;
        }

        if (RULE_TYPE_UNDEF_ICMP_TYPE != ace_p->u.ip.aceIcmpType)
        {
            ui_ace_p->icmp.icmp_type.op = RULE_TYPE_ICMP_ICMPTYPE_OP_EQUAL;
            ui_ace_p->icmp.icmp_type.u.s.data = ace_p->u.ip.aceIcmpType;
            ui_ace_p->icmp.icmp_type.u.s.mask = 0xff;
        }

        if (MIN_diffServIpAceControlCodeBitmask != ace_p->u.ip.aceControlCodeBitmask ||
            RULE_TYPE_UNDEF_IP_CONTROL_CODE != ace_p->u.ip.aceControlCode)
        {
            ui_ace_p->tcp.flags.data.u.code = ace_p->u.ip.aceControlCode;
            ui_ace_p->tcp.flags.mask.u.code = ace_p->u.ip.aceControlCodeBitmask;
        }

        /* source port */
        if (VAL_diffServIpAceSourcePortOp_equal == ace_p->u.ip.aceSourcePortOp)
        {
            ui_ace_p->l4_common.sport.op = VAL_diffServIpAceSourcePortOp_equal;
            ui_ace_p->l4_common.sport.u.s.data = ace_p->u.ip.aceMinSourcePort;
            ui_ace_p->l4_common.sport.u.s.mask = ace_p->u.ip.aceSourcePortBitmask;
        }
        else if (VAL_diffServIpAceSourcePortOp_range == ace_p->u.ip.aceSourcePortOp)
        {
            ui_ace_p->l4_common.sport.op = VAL_diffServIpAceSourcePortOp_range;
            ui_ace_p->l4_common.sport.u.range.min = ace_p->u.ip.aceMinSourcePort;
            ui_ace_p->l4_common.sport.u.range.max = ace_p->u.ip.aceMaxSourcePort;
        }

        /* destination port */
        if (VAL_diffServIpAceSourcePortOp_equal == ace_p->u.ip.aceDestPortOp)
        {
            ui_ace_p->l4_common.dport.op = VAL_diffServIpAceDestPortOp_equal;
            ui_ace_p->l4_common.dport.u.s.data = ace_p->u.ip.aceMinDestPort;
            ui_ace_p->l4_common.dport.u.s.mask = ace_p->u.ip.aceDestPortBitmask;
        }
        else if (VAL_diffServIpAceSourcePortOp_range == ace_p->u.ip.aceDestPortOp)
        {
            ui_ace_p->l4_common.dport.op = VAL_diffServIpAceDestPortOp_range;
            ui_ace_p->l4_common.dport.u.range.min = ace_p->u.ip.aceMinDestPort;
            ui_ace_p->l4_common.dport.u.range.max = ace_p->u.ip.aceMaxDestPort;
        }
    }

    if (ace_qualify & IPV6_QUALIFY)
    {
        if (FALSE == RULE_OM_IS_ZERO_IPV6_ADDR(ace_p->u.ipv6.aceSourceIpAddrBitmask))
        {
            RULE_TYPE_RETURN_TYPE_T result;

            memcpy(ui_ace_p->ipv6.sip.addr,
                   ace_p->u.ipv6.aceSourceIpAddr,
                   sizeof(ui_ace_p->ipv6.sip.addr));

            result = RULE_OM_LocalConvertIpv6Mask2Prefix(ace_p->u.ipv6.aceSourceIpAddrBitmask,
                                                         &ui_ace_p->ipv6.sip.prefix_len);
            ASSERT(result == RULE_TYPE_OK);
        }

        if (FALSE == RULE_OM_IS_ZERO_IPV6_ADDR(ace_p->u.ipv6.aceDestIpAddrBitmask))
        {
            RULE_TYPE_RETURN_TYPE_T result;

            memcpy(ui_ace_p->ipv6.dip.addr,
                   ace_p->u.ipv6.aceDestIpAddr,
                   sizeof(ui_ace_p->ipv6.dip.addr));

            result = RULE_OM_LocalConvertIpv6Mask2Prefix(ace_p->u.ipv6.aceDestIpAddrBitmask,
                                                         &ui_ace_p->ipv6.dip.prefix_len);
            ASSERT(result == RULE_TYPE_OK);
        }

        if (RULE_TYPE_UNDEF_IPV6_DSCP != ace_p->u.ipv6.aceDscp)
        {
            ui_ace_p->ipv6.traffic_class.op = RULE_TYPE_IPV6_TRAFFIC_CLASS_OP_EQUAL;
            ui_ace_p->ipv6.traffic_class.u.s.data = ace_p->u.ipv6.aceDscp << 2;
            ui_ace_p->ipv6.traffic_class.u.s.mask = (MAX_diffServIpv6AceDscp - 1) << 2;
        }

        if (RULE_TYPE_UNDEF_IPV6_NEXT_HEADER != ace_p->u.ipv6.aceNextHeader)
        {
            ui_ace_p->ipv6.next_header.op = RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL;
            ui_ace_p->ipv6.next_header.u.s.data = ace_p->u.ipv6.aceNextHeader;
            ui_ace_p->ipv6.next_header.u.s.mask = MAX_diffServIpv6AceNextHeader - 1;
        }

        if (RULE_TYPE_UNDEF_IPV6_FLOW_LABEL != ace_p->u.ipv6.aceFlowLabel)
        {
            ui_ace_p->ipv6.flow_label.op = RULE_TYPE_IPV6_FLOW_LABEL_OP_EQUAL;
            ui_ace_p->ipv6.flow_label.u.s.data = ace_p->u.ipv6.aceFlowLabel;
            ui_ace_p->ipv6.flow_label.u.s.mask = MAX_diffServIpv6AceFlowLabel - 1;
        }

        /* source port */
        if (VAL_diffServIpAceSourcePortOp_equal == ace_p->u.ipv6.aceSourcePortOp)
        {
            ui_ace_p->l4_common.sport.op = VAL_diffServIpAceSourcePortOp_equal;
            ui_ace_p->l4_common.sport.u.s.data = ace_p->u.ipv6.aceMinSourcePort;
            ui_ace_p->l4_common.sport.u.s.mask = ace_p->u.ipv6.aceSourcePortBitmask;
        }
        else if (VAL_diffServIpAceSourcePortOp_range == ace_p->u.ipv6.aceSourcePortOp)
        {
            ui_ace_p->l4_common.sport.op = VAL_diffServIpAceSourcePortOp_range;
            ui_ace_p->l4_common.sport.u.range.min = ace_p->u.ipv6.aceMinSourcePort;
            ui_ace_p->l4_common.sport.u.range.max = ace_p->u.ipv6.aceMaxSourcePort;
        }

        /* destination port */
        if (VAL_diffServIpAceSourcePortOp_equal == ace_p->u.ipv6.aceDestPortOp)
        {
            ui_ace_p->l4_common.dport.op = VAL_diffServIpAceDestPortOp_equal;
            ui_ace_p->l4_common.dport.u.s.data = ace_p->u.ipv6.aceMinDestPort;
            ui_ace_p->l4_common.dport.u.s.mask = ace_p->u.ipv6.aceDestPortBitmask;
        }
        else if (VAL_diffServIpAceSourcePortOp_range == ace_p->u.ipv6.aceDestPortOp)
        {
            ui_ace_p->l4_common.dport.op = VAL_diffServIpAceDestPortOp_range;
            ui_ace_p->l4_common.dport.u.range.min = ace_p->u.ipv6.aceMinDestPort;
            ui_ace_p->l4_common.dport.u.range.max = ace_p->u.ipv6.aceMaxDestPort;
        }
    }

#undef MAC_QUALIFY
#undef IPV4_QUALIFY
#undef IPV6_QUALIFY
    return RULE_TYPE_OK;
}

