/* Project NAME:  cos_om.c
* PURPOSE:
*	cos database
*
* REASON:
*      Description:
*      CREATOR:         mfhorng
*      Date             2005/8/16
*
* Copyright(C)      Accton Corporation, 2002
*/
/* INCLUDE FILE DECLARATIONS
 */
#include "sys_dflt.h"
#include "sysfun.h"
#include "cos_om.h"
#include "sysrsc_mgr.h"
#include "sys_bld.h"
#include <string.h>

//extern void *memset(void *b, int c, unsigned int len);
/* NAMING CONSTANT DECLARATIONS
 */


/* MACRO FUNCTION DECLARATIONS
 */
#define COS_OM_IS_BAD_LPORT_NO(lport)               ((0 >= lport) || (lport > SYS_ADPT_TOTAL_NBR_OF_LPORT))
#define COS_VM_IS_BAD_IP_PRECEDENCE(value)          (value >= COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES)
#define COS_VM_IS_BAD_IP_DSCP(value)                (value >= COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES)

/* DATA TYPE DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */

#define COS_OM_EnterCriticalSection() SYSFUN_TakeSem(cos_om_sem_id, SYSFUN_TIMEOUT_WAIT_FOREVER)
#define COS_OM_LeaveCriticalSection() SYSFUN_GiveSem(cos_om_sem_id)

/* STATIC VARIABLE DECLARATIONS
 */
static UI32_T cos_om_sem_id;
static COS_OM_ShmemData_T *shmem_data_p;

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_OM_InitiateSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE  : Initiate share memory resources
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
void COS_OM_InitiateSystemResources()
{
    shmem_data_p = (COS_OM_ShmemData_T *)SYSRSC_MGR_GetShMem(SYSRSC_MGR_COS_OM_SHMEM_SEGID);
    SYSFUN_INITIATE_CSC_ON_SHMEM(shmem_data_p);

	memset(shmem_data_p->precedence_vs_cos_map_table, 0, SYS_ADPT_TOTAL_NBR_OF_LPORT * COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES);
	memset(shmem_data_p->dscp_vs_cos_map_table, 0, SYS_ADPT_TOTAL_NBR_OF_LPORT * COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES);
}

/* FUNCTION NAME: COS_OM_GetShMemInfo
 *------------------------------------------------------------------------------
 * PURPOSE: Get share memory info
 *------------------------------------------------------------------------------
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 *------------------------------------------------------------------------------
 * NOTES:   
 */
void COS_OM_GetShMemInfo(SYSRSC_MGR_SEGID_T *segid_p, UI32_T *seglen_p)
{
    *segid_p = SYSRSC_MGR_COS_OM_SHMEM_SEGID;
    *seglen_p = sizeof(COS_OM_ShmemData_T);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME: COS_OM_AttachSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE: Attach system resource for COS_OM in the context of the calling
 *          process.
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------*/
void COS_OM_AttachSystemResources(void)
{
    shmem_data_p = (COS_OM_ShmemData_T*)SYSRSC_MGR_GetShMem(SYSRSC_MGR_COS_OM_SHMEM_SEGID);
    SYSFUN_GetSem(SYS_BLD_SYS_SEMAPHORE_KEY_COS_OM, &cos_om_sem_id);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME: COS_OM_EnterMasterMode
 *------------------------------------------------------------------------------
 * PURPOSE: Enter master mode
 * INPUT:   none
 * OUTPUT:  none
 * RETUEN:  none
 * NOTES:   none
 *------------------------------------------------------------------------------*/
void COS_OM_EnterMasterMode (void)
{
    SYSFUN_ENTER_MASTER_MODE_ON_SHMEM(shmem_data_p);
    return;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME: COS_OM_EnterSlaveMode
 *------------------------------------------------------------------------------
 * PURPOSE: Enter slave mode
 * INPUT:   none
 * OUTPUT:  none
 * RETUEN:  none
 * NOTES:   none
 *------------------------------------------------------------------------------*/
void COS_OM_EnterSlaveMode (void)
{
    SYSFUN_ENTER_SLAVE_MODE_ON_SHMEM(shmem_data_p);
    return;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME: COS_OM_ProvisionComplete
 *------------------------------------------------------------------------------
 * PURPOSE: All provision commands are settle down.
 * INPUT:   none
 * OUTPUT:  none
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------*/
void COS_OM_ProvisionComplete(void)
{
    return;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME: COS_OM_SetTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE: Sets to temporary transition mode
 * INPUT:   none
 * OUTPUT:  none
 * RETUEN:  none
 * NOTES:   none
 *------------------------------------------------------------------------------*/
void COS_OM_SetTransitionMode (void)
{
    SYSFUN_SET_TRANSITION_MODE_ON_SHMEM(shmem_data_p);
    return;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME: COS_OM_EnterTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE: Enter transition mode
 * INPUT:   none
 * OUTPUT:  none
 * RETUEN:  none
 * NOTES:   none
 *------------------------------------------------------------------------------*/
void COS_OM_EnterTransitionMode (void)
{
    SYSFUN_ENTER_TRANSITION_MODE_ON_SHMEM(shmem_data_p);
    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_OM_SetIpPrecedence2CosValueByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : set ip precedence to cos mapping by lport
 * INPUT    : lport, ip_precedence, cos
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
BOOL_T COS_OM_SetIpPrecedence2CosValueByLport(UI32_T lport, UI8_T ip_precedence, UI8_T cos)
{
    if ((TRUE == COS_OM_IS_BAD_LPORT_NO(lport)) ||
        (TRUE == COS_VM_IS_BAD_IP_PRECEDENCE(ip_precedence)))
    {
        printf("\r\n[COS_OM_SetIpPrecedence2CosValueByLport] fail to set lport(%lu) precedence(%u) cos value",
            lport, ip_precedence);
        return FALSE;
    }

	COS_OM_EnterCriticalSection();
    shmem_data_p->precedence_vs_cos_map_table[lport - 1][ip_precedence] = cos;
	COS_OM_LeaveCriticalSection();
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_OM_GetIpPrecedence2CosValueByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : get ip precedence to cos mapping by lport
 * INPUT    : lport, ip_precedence
 * OUTPUT   : cos
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
BOOL_T COS_OM_GetIpPrecedence2CosValueByLport(UI32_T lport, UI8_T ip_precedence, UI8_T *cos)
{
    if ((TRUE == COS_OM_IS_BAD_LPORT_NO(lport)) ||
        (TRUE == COS_VM_IS_BAD_IP_PRECEDENCE(ip_precedence)) ||
        (NULL == cos))
    {
        printf("\r\n[COS_OM_GetIpPrecedence2CosValueByLport] fail to get lport(%lu) precedence(%u) cos value",
            lport, ip_precedence);
        return FALSE;
    }

	COS_OM_EnterCriticalSection();
    *cos = shmem_data_p->precedence_vs_cos_map_table[lport - 1][ip_precedence];
	COS_OM_LeaveCriticalSection();
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_OM_SetIpDscp2CosValueByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : set ip dscp to cos mapping by lport
 * INPUT    : lport, ip_dscp, cos
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
BOOL_T COS_OM_SetIpDscp2CosValueByLport(UI32_T lport, UI8_T ip_dscp, UI8_T cos)
{
    if ((TRUE == COS_OM_IS_BAD_LPORT_NO(lport)) ||
        (TRUE == COS_VM_IS_BAD_IP_DSCP(ip_dscp)))
    {
        printf("\r\n[COS_OM_SetIpDscp2CosValueByLport] fail to set lport(%lu) dscp(%u) cos value",
            lport, ip_dscp);
        return FALSE;
    }

	COS_OM_EnterCriticalSection();
    shmem_data_p->dscp_vs_cos_map_table[lport - 1][ip_dscp] = cos;
	COS_OM_LeaveCriticalSection();
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_OM_GetIpDscp2CosValueByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : get ip dscp to cos mapping by lport
 * INPUT    : lport, ip_dscp
 * OUTPUT   : cos
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
BOOL_T COS_OM_GetIpDscp2CosValueByLport(UI32_T lport, UI8_T ip_dscp, UI8_T *cos)
{
    if ((TRUE == COS_OM_IS_BAD_LPORT_NO(lport)) ||
        (TRUE == COS_VM_IS_BAD_IP_DSCP(ip_dscp)) ||
        (NULL == cos))
    {
        printf("\r\n[COS_OM_GetIpDscp2CosValueByLport] fail to get lport(%lu) dscp(%u) cos value",
            lport, ip_dscp);
        return FALSE;
    }

	COS_OM_EnterCriticalSection();
    *cos = shmem_data_p->dscp_vs_cos_map_table[lport - 1][ip_dscp];
	COS_OM_LeaveCriticalSection();
    return TRUE;
}

