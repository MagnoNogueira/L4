/* MODULE NAME: rule_om_exp.c
 * PURPOSE:
 *   Definitions of Exported OM APIs for ACL (,etc...).
 *
 * NOTES:
 *   None
 *
 * HISTORY:
 *   mm/dd/yy (A.D.)
 *   10/14/11    -- yehjunying, Create
 *
 * Copyright(C)      Accton Corporation, 2011
 */
 /* INCLUDE FILE DECLARATIONS
  */
#include <string.h>
#include "sys_bld.h"
#include "l_cvrt.h"
#include "rule_om_exp.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */
#if 1
#define RULE_OM_EXP_DBMSG(fmt, ...)

#else
#define RULE_OM_EXP_DBMSG(fmt, ...)                     \
    {                                                   \
        printf("(%5d):%s, " fmt "\r\n",                 \
            __LINE__, __FUNCTION__, ##__VA_ARGS__ );    \
    }
#endif

#define RULE_OM_EXP_IS_BAD_ACL_IDX(idx)                 ((0 >= idx) || (idx > _countof(rule_om_exp_shmem_data_p->acl_table)))
#define RULE_OM_EXP_IS_ENTRY_DESTROY(row_status)        (0 == row_status)

#define RULE_OM_EXP_IS_BAD_ACE_IDX(idx)                 ((0 >= idx) || (idx > _countof(rule_om_exp_shmem_data_p->ace_table)))
#define RULE_OM_EXP_IS_ENTRY_DESTROY(row_status)        (0 == row_status)

#define RULE_OM_EXP_PTR2INST(shm_ptr)                   \
    RULE_OM_EXP_LocalPtr2Inst(shm_ptr)

#define RULE_OM_EXP_VALIDATE_INST(in)                   \
    RULE_OM_EXP_LocalValidateInstance(in)

#define RULE_OM_EXP_INDEX_OF(in)                        \
    RULE_OM_EXP_LocalIndexOf(in)

#define RULE_OM_EXP_EnterCriticalSection()              SYSFUN_TakeSem(rule_om_exp_sem_id, SYSFUN_TIMEOUT_WAIT_FOREVER)
#define RULE_OM_EXP_LeaveCriticalSection()              SYSFUN_GiveSem(rule_om_exp_sem_id)

#define RULE_OM_EXP_LeaveCriticalSectionReturnState(state)      \
{                                                               \
    RULE_OM_EXP_LeaveCriticalSection();                         \
    return state;                                               \
}

#define RULE_OM_EXP_LeaveCriticalSectionReturnVoid()            \
{                                                               \
    RULE_OM_EXP_LeaveCriticalSection();                         \
    return;                                                     \
}

#ifndef _countof
#define _countof(ary)           (sizeof(ary)/sizeof(*ary))
#endif

#ifndef ASSERT
#define ASSERT(eq)
#endif

/* DATA TYPE DECLARATIONS
 */
typedef UI32_T (*ACE_FILTER_FP)(void  *ace_p, void *data_p);

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static UI32_T
RULE_OM_EXP_LocalGetAclIdByName(
    const char *acl_name,
    UI32_T  *acl_index
);

static RULE_TYPE_Acl_T *
RULE_OM_EXP_LocalGetAclByIndex(
    UI32_T acl_index
);

static BOOL_T
RULE_OM_EXP_LocalIsSameAclType(
    RULE_TYPE_AclType_T src_acl_type,
    RULE_TYPE_AclType_T dst_acl_type
);

static UI32_T
RULE_OM_EXP_LocalSwAclCheck(
    const char *acl_name_p,
    RULE_TYPE_AclType_T req_acl_type,
    void *data_p,
    ACE_FILTER_FP ace_filter_fp
);

static UI32_T
RULE_OM_EXP_LocalAceFilterForBgpDistrbLst(
    void *ace_p,
    void *data_p
);

static void
RULE_OM_EXP_LocalValidateInstance(
    RULE_TYPE_INSTANCE_PTR_T in
);

static UI32_T
RULE_OM_EXP_LocalIndexOf(
    void *entry
);

static void*
RULE_OM_EXP_LocalPtr2Inst(
    RULE_TYPE_SHM_POINTER_T ptr
);

/* STATIC VARIABLE DECLARATIONS
 */
static RULE_OM_ShmemData_T   *rule_om_exp_shmem_data_p;
static UI32_T                rule_om_exp_sem_id;


/* EXPORTED SUBPROGRAM BODIES
 */
/*------------------------------------------------------------------------------
 * FUNCTION NAME: RULE_OM_EXP_AttachSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE: Attach system resource for RULE_CTRL in the context of the calling
 *          process.
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
void RULE_OM_EXP_AttachSystemResources(void)
{
    rule_om_exp_shmem_data_p = (RULE_OM_ShmemData_T*)SYSRSC_MGR_GetShMem(SYSRSC_MGR_RULE_OM_SHMEM_SEGID);
    SYSFUN_GetSem(SYS_BLD_SYS_SEMAPHORE_KEY_RULE_OM, &rule_om_exp_sem_id);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_EXP_GetAclIdByNameAndType
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of ACL by it's name and type.
 * INPUT:    acl_name_p, acl_type
 * OUTPUT:   acl_index_p.
 * RETURN:   RULE_TYPE_OK/RULE_TYPE_FAIL
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_EXP_GetAclIdByNameAndType(
    char *acl_name_p,
    RULE_TYPE_AclType_T acl_type,
    UI32_T *acl_index_p)
{
    UI32_T              ret;
    RULE_TYPE_Acl_T     *acl_p;

    RULE_OM_EXP_EnterCriticalSection();

    ret = RULE_OM_EXP_LocalGetAclIdByName(acl_name_p, acl_index_p);
    if (RULE_TYPE_OK == ret)
    {
        acl_p = RULE_OM_EXP_LocalGetAclByIndex (*acl_index_p);
        if (  (NULL == acl_p)
            ||(acl_type != acl_p->acl_type)
           )
        {
            ret = RULE_TYPE_FAIL;
        }
    }

    RULE_OM_EXP_LeaveCriticalSectionReturnState(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_EXP_SwAclCheckForBgpDistrbLst
 *------------------------------------------------------------------------------
 * PURPOSE: To do software filtering with provided ACL name, and data for BGP
 *          distribute list.
 * INPUT  : acl_name_p - pointer to acl name required to do filtering
 *          data_p     - pointer to data for filtering
 * OUTPUT : None
 * RETURN : RULE_TYPE_FAIL           - unexpected failure
 *          RULE_TYPE_ACL_NONEXISTED - required ACL does not exist
 *          RULE_TYPE_MATCH_PERMIT   - permit ace matched
 *          RULE_TYPE_MATCH_DENY     - deny   ace matched
 *          RULE_TYPE_NO_MATCH       - no     ace matched
 * NOTE   : None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_EXP_SwAclCheckForBgpDistrbLst(
    const char *acl_name_p,
    RULE_OM_EXP_BgpFilterData_T *data_p)
{
    UI32_T  ret;

    RULE_OM_EXP_EnterCriticalSection();

    ret = RULE_OM_EXP_LocalSwAclCheck(
                acl_name_p,
                RULE_TYPE_IP_ACL,
                (void *) data_p,
                RULE_OM_EXP_LocalAceFilterForBgpDistrbLst);

    RULE_OM_EXP_LeaveCriticalSectionReturnState(ret);
}


/* LOCAL SUBPROGRAM BODIES
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_EXP_LocalGetAclIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of ACL by it's name.
 * INPUT:    acl_name
 * OUTPUT:   acl_index.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_EXP_LocalGetAclIdByName(
    const char *acl_name_p,
    UI32_T *acl_index_p)
{
    UI32_T  index;

    if ((NULL == acl_name_p) || (NULL == acl_index_p))
        return RULE_TYPE_FAIL;

    /* sequential search */
    for (index = 0; _countof(rule_om_exp_shmem_data_p->acl_table) > index; ++index)
    {
        if (RULE_OM_EXP_IS_ENTRY_DESTROY(rule_om_exp_shmem_data_p->acl_table[index].row_status))
            continue;

        if (0 == strcmp(acl_name_p, (char *)rule_om_exp_shmem_data_p->acl_table[index].acl_name))
        {
            *acl_index_p = index + 1; /* to 1-based index */
            return RULE_TYPE_OK;
        }
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_EXP_LocalGetAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get acl by index
 * INPUT    : acl_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_Acl_T *
RULE_OM_EXP_LocalGetAclByIndex(
    UI32_T acl_index)
{
    if (RULE_OM_EXP_IS_BAD_ACL_IDX(acl_index))
        return NULL;

    if (RULE_OM_EXP_IS_ENTRY_DESTROY(rule_om_exp_shmem_data_p->acl_table[acl_index - 1].row_status))
        return NULL;

    return &rule_om_exp_shmem_data_p->acl_table[acl_index - 1];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_EXP_LocalGetAceByIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get ace by index
 * INPUT    : ace_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_Ace_Entry_T *
RULE_OM_EXP_LocalGetAceByIndex(
    UI32_T ace_index)
{
    if (RULE_OM_EXP_IS_BAD_ACE_IDX(ace_index))
        return NULL;

    if (RULE_OM_EXP_IS_ENTRY_DESTROY(rule_om_exp_shmem_data_p->ace_table[ace_index - 1].row_status))
        return NULL;

    return &rule_om_exp_shmem_data_p->ace_table[ace_index - 1];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_EXP_LocalIsSameAclType
 *------------------------------------------------------------------------------
 * PURPOSE  : To check if source and destination acl type are the same.
 * INPUT    : src_acl_type - soruce acl type to check
 *            dst_acl_type - destination acl type to check
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_OM_EXP_LocalIsSameAclType(
    RULE_TYPE_AclType_T src_acl_type,
    RULE_TYPE_AclType_T dst_acl_type)
{
    static UI16_T acl_type_ar[RULE_TYPE_MAX_NBR_OF_ACL_TYPE] = {
            [RULE_TYPE_MAC_ACL]     = RULE_TYPE_MAC_ACL,
            [RULE_TYPE_IP_ACL]      = RULE_TYPE_IP_ACL,
            [RULE_TYPE_IP_STD_ACL]  = RULE_TYPE_IP_ACL,
            [RULE_TYPE_IP_EXT_ACL]  = RULE_TYPE_IP_ACL,
            [RULE_TYPE_MF]          = RULE_TYPE_MF,
            [RULE_TYPE_IPV6_ACL]    = RULE_TYPE_IPV6_ACL,
            [RULE_TYPE_IPV6_STD_ACL]= RULE_TYPE_IPV6_ACL,
            [RULE_TYPE_IPV6_EXT_ACL]= RULE_TYPE_IPV6_ACL,
#if (SYS_CPNT_DAI == TRUE)
            [RULE_TYPE_ARP_ACL]     = RULE_TYPE_ARP_ACL,
#endif
        };

    BOOL_T  ret = FALSE;

    if (  (src_acl_type < RULE_TYPE_MAX_NBR_OF_ACL_TYPE)
        &&(dst_acl_type < RULE_TYPE_MAX_NBR_OF_ACL_TYPE)
       )
    {
        if (acl_type_ar[src_acl_type] == acl_type_ar[dst_acl_type])
        {
            ret = TRUE;
        }
    }

    return ret;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_EXP_LocalSwAclCheck
 *------------------------------------------------------------------------------
 * PURPOSE: To do software filtering with provided ACL name, filtering data and
 *          filtering function.
 * INPUT  : req_acl_name_p - pointer to acl name required to do filtering
 *          req_acl_type   - acl type required to do filtering
 *          data_p         - pointer to data for filtering
 *          ace_filter_fp  - pointer to filter function with specified
 *                           pointer to data and ace entry
 * OUTPUT : None
 * RETURN : RULE_TYPE_FAIL           - unexpected failure
 *          RULE_TYPE_ACL_NONEXISTED - required ACL does not exist
 *          RULE_TYPE_MATCH_PERMIT   - permit ace matched
 *          RULE_TYPE_MATCH_DENY     - deny   ace matched
 *          RULE_TYPE_NO_MATCH       - no     ace matched
 * NOTE   : None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_EXP_LocalSwAclCheck(
    const char *acl_name_p,
    RULE_TYPE_AclType_T req_acl_type,
    void *data_p,
    ACE_FILTER_FP ace_filter_fp)
{
    RULE_TYPE_INSTANCE_PTR_T    ace_inst_p;
    RULE_TYPE_Acl_T             *acl_p;

    UI32_T                      acl_index, ace_index, ret;

    if (FALSE == rule_om_exp_shmem_data_p->rule_om_had_been_initialized)
    {
        return RULE_TYPE_FAIL;
    }

    ret = RULE_OM_EXP_LocalGetAclIdByName(acl_name_p, &acl_index);
    if (RULE_TYPE_OK != ret)
    {
        return RULE_TYPE_ACL_NONEXISTED;
    }

    acl_p = RULE_OM_EXP_LocalGetAclByIndex(acl_index);
    if (NULL == acl_p)
    {
        return RULE_TYPE_FAIL;
    }

    if (FALSE == RULE_OM_EXP_LocalIsSameAclType(req_acl_type,
                                                acl_p->acl_type))
    {
        return RULE_TYPE_FAIL;
    }

    ret = RULE_TYPE_NO_MATCH;

    /* check each ACE
     */
    for (ace_inst_p = RULE_OM_EXP_PTR2INST(acl_p->super.links.first_node);
         NULL != ace_inst_p;
         ace_inst_p = RULE_OM_EXP_PTR2INST(ace_inst_p->links.next))
    {
        RULE_TYPE_Ace_Entry_T *ace_p;

        ace_index = RULE_OM_EXP_INDEX_OF(ace_inst_p) + 1;

        ace_p = RULE_OM_EXP_LocalGetAceByIndex(ace_index);

        ASSERT(NULL != ace_p);

        if (NULL == ace_p)
        {
            continue;
        }

        ret = ace_filter_fp(ace_p, data_p);
        switch (ret)
        {
        case RULE_TYPE_MATCH_DENY:
        case RULE_TYPE_MATCH_PERMIT:
            RULE_OM_EXP_DBMSG("MATCH ACE ID -%ld", i);
            return ret;
        default:
            RULE_OM_EXP_DBMSG("NO MATCH ACE ID -%ld", i);
            break;
        }
    }

    /* no match rule
     */
    return ret;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_EXP_LocalAceFilterForBgpDistrbLst
 *------------------------------------------------------------------------------
 * PURPOSE: To do software filtering with provided ace entry and filtering data.
 * INPUT  : ace_p  - pointer to ace entry to do filtering
 *          data_p - pointer to data for filtering
 * OUTPUT : None
 * RETURN : RULE_TYPE_FAIL           - unexpected failure
 *          RULE_TYPE_MATCH_PERMIT   - permit ace matched
 *          RULE_TYPE_MATCH_DENY     - deny   ace matched
 *          RULE_TYPE_NO_MATCH       - no     ace matched
 * NOTE   : None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_OM_EXP_LocalAceFilterForBgpDistrbLst(
    void *ace_p,
    void *data_p)
{
    RULE_TYPE_IpAce_Entry_T         *ip_ace_p;
    RULE_OM_EXP_BgpFilterData_T     *bgp_data_p;
    UI32_T                          access;
    UI32_T                          ret = RULE_TYPE_FAIL;

    if ((NULL != ace_p) && (NULL != data_p))
    {
        access      = ((RULE_TYPE_Ace_Entry_T *) ace_p)->access;
        ip_ace_p    = &((RULE_TYPE_Ace_Entry_T *) ace_p)->u.ip;
        bgp_data_p  = (RULE_OM_EXP_BgpFilterData_T *)data_p;
        ret         = RULE_TYPE_NO_MATCH;

        /* only use source      ip/mask
         *          destination ip/mask in the ACE ecntry
         * ip/mask is in host order
         */
        /* like cisco, source ip is to filter the ip address part
         */
        if (0 != ip_ace_p->aceSourceIpAddrBitmask)
        {
            if (  (bgp_data_p->ip_addr       & ip_ace_p->aceSourceIpAddrBitmask)
                !=(ip_ace_p->aceSourceIpAddr & ip_ace_p->aceSourceIpAddrBitmask)
               )
            {
                RULE_OM_EXP_DBMSG("ACE IP ADDRESS -%08lx", ip_ace_p->aceSourceIpAddr);
                RULE_OM_EXP_DBMSG("ACE IP MASK    -%08lx", ip_ace_p->aceSourceIpAddrBitmask);
                RULE_OM_EXP_DBMSG("BGP IP ADDRESS -%08lx", bgp_data_p->ip_addr);
                RULE_OM_EXP_DBMSG("IP ADDRESS NOT MATCH");
                return ret;
            }

            RULE_OM_EXP_DBMSG("MATCH ACE SRC IP/MASK -%3d.%3d.%3d.%3d/%3d.%3d.%3d.%3d\n",
                (ip_ace_p->aceSourceIpAddr        >> 24) & 0xff,
                (ip_ace_p->aceSourceIpAddr        >> 16) & 0xff,
                (ip_ace_p->aceSourceIpAddr        >>  8) & 0xff,
                (ip_ace_p->aceSourceIpAddr        >>  0) & 0xff,
                (ip_ace_p->aceSourceIpAddrBitmask >> 24) & 0xff,
                (ip_ace_p->aceSourceIpAddrBitmask >> 16) & 0xff,
                (ip_ace_p->aceSourceIpAddrBitmask >>  8) & 0xff,
                (ip_ace_p->aceSourceIpAddrBitmask >>  0) & 0xff);
        }

        /* like cisco, destination ip is to filter the ip prefix part
         */
        if (0 != ip_ace_p->aceDestIpAddrBitmask)
        {
            if (  (bgp_data_p->ip_pfx        & ip_ace_p->aceDestIpAddrBitmask)
                !=(ip_ace_p->aceDestIpAddr   & ip_ace_p->aceDestIpAddrBitmask)
               )
            {
                RULE_OM_EXP_DBMSG("ACE IP ADDRESS -%08lx", ip_ace_p->aceDestIpAddr);
                RULE_OM_EXP_DBMSG("ACE IP MASK    -%08lx", ip_ace_p->aceDestIpAddrBitmask);
                RULE_OM_EXP_DBMSG("BGP IP ADDRESS -%08lx", bgp_data_p->ip_pfx);
                RULE_OM_EXP_DBMSG("IP PREFIX NOT MATCH");
                return ret;
            }

            RULE_OM_EXP_DBMSG("MATCH ACE DST IP/MASK -%3d.%3d.%3d.%3d/%3d.%3d.%3d.%3d\n",
                (ip_ace_p->aceDestIpAddr        >> 24) & 0xff,
                (ip_ace_p->aceDestIpAddr        >> 16) & 0xff,
                (ip_ace_p->aceDestIpAddr        >>  8) & 0xff,
                (ip_ace_p->aceDestIpAddr        >>  0) & 0xff,
                (ip_ace_p->aceDestIpAddrBitmask >> 24) & 0xff,
                (ip_ace_p->aceDestIpAddrBitmask >> 16) & 0xff,
                (ip_ace_p->aceDestIpAddrBitmask >>  8) & 0xff,
                (ip_ace_p->aceDestIpAddrBitmask >>  0) & 0xff);
        }

        ret = (RULE_TYPE_ACE_PERMIT == access) ?
              RULE_TYPE_MATCH_PERMIT : RULE_TYPE_MATCH_DENY;
    }

    RULE_OM_EXP_DBMSG("MATCH RET - %ld\n", ret);

    return ret;
}

static void
RULE_OM_EXP_LocalValidateInstance(
    RULE_TYPE_INSTANCE_PTR_T in)
{
    UI32_T offset;

    switch (in->type)
    {
        case RULE_TYPE_INST_OM_USER_ACE:
        case RULE_TYPE_INST_OM_WORK_ACE:
            offset = L_CVRT_GET_OFFSET(rule_om_exp_shmem_data_p->ace_table, in);

            ASSERT(offset < sizeof(rule_om_exp_shmem_data_p->ace_table));
            break;

        case RULE_TYPE_INST_OM_USER_ACL:
        case RULE_TYPE_INST_OM_WORK_ACL:
            offset = L_CVRT_GET_OFFSET(rule_om_exp_shmem_data_p->acl_table, in);

            ASSERT(offset < sizeof(rule_om_exp_shmem_data_p->acl_table));
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
RULE_OM_EXP_LocalIndexOf(
    void *entry)
{
    RULE_TYPE_INSTANCE_PTR_T in = (RULE_TYPE_INSTANCE_PTR_T)entry;
    UI32_T offset;

    ASSERT(NULL != in);

    RULE_OM_EXP_VALIDATE_INST(in);

    switch (in->type)
    {
        case RULE_TYPE_INST_OM_USER_ACE:
        case RULE_TYPE_INST_OM_WORK_ACE:
            offset = L_CVRT_GET_OFFSET(rule_om_exp_shmem_data_p->ace_table, in);

            ASSERT(offset < sizeof(rule_om_exp_shmem_data_p->ace_table));
            ASSERT(0 == (offset % sizeof(rule_om_exp_shmem_data_p->ace_table[0])));
            return offset / sizeof(rule_om_exp_shmem_data_p->ace_table[0]);

        case RULE_TYPE_INST_OM_USER_ACL:
        case RULE_TYPE_INST_OM_WORK_ACL:
            offset = L_CVRT_GET_OFFSET(rule_om_exp_shmem_data_p->acl_table, in);

            ASSERT(offset < sizeof(rule_om_exp_shmem_data_p->acl_table));
            ASSERT(0 == (offset % sizeof(rule_om_exp_shmem_data_p->acl_table[0])));
            return offset / sizeof(rule_om_exp_shmem_data_p->acl_table[0]);

        default:
            ASSERT(0);
            break;
    }

    ASSERT(0);
    return 0xffffffff;
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
RULE_OM_EXP_LocalPtr2Inst(
    RULE_TYPE_SHM_POINTER_T ptr)
{
    switch (ptr.type)
    {
        case RULE_TYPE_INST_OM_USER_ACE:
        case RULE_TYPE_INST_OM_WORK_ACE:
            ASSERT(ptr.offset < sizeof(rule_om_exp_shmem_data_p->ace_table));
            return L_CVRT_GET_PTR(rule_om_exp_shmem_data_p->ace_table, ptr.offset);

        case RULE_TYPE_INST_OM_USER_ACL:
        case RULE_TYPE_INST_OM_WORK_ACL:
            ASSERT(ptr.offset < sizeof(rule_om_exp_shmem_data_p->acl_table));
            return L_CVRT_GET_PTR(rule_om_exp_shmem_data_p->acl_table, ptr.offset);

        case RULE_TYPE_NIL:
            break;

        default:
            ASSERT(0);
            break;
    }

    return NULL;
}
