/* Project NAME:  cos_om.h
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
#ifndef	COS_OM_H
#define	COS_OM_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_adpt.h"
#include "sysfun.h"
//#include "sysrsc_mgr.h"
/* NAMING CONSTANT DECLARATIONS
 */
#define COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES                   8
#define COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES                         64

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */
typedef struct
{
    SYSFUN_DECLARE_CSC_ON_SHMEM

    /* use two dimention array to save cos value, first dimention is (logical port - 1),
     * second dimention is ip precedence value. therefore array element [0][0] implies the cos value
     * of (port 1, precedence 0)
     */
    UI8_T  precedence_vs_cos_map_table[SYS_ADPT_TOTAL_NBR_OF_LPORT][COS_OM_ADPT_MAX_IP_PRECEDENCE_MAP_ENTRIES];
 
    /* use two dimention array to save cos value, first dimention is (logical port - 1),
     * second dimention is ip dscp value. therefore array element [0][0] implies the cos value
     * of (port 1, dscp 0)
     */
    UI8_T  dscp_vs_cos_map_table[SYS_ADPT_TOTAL_NBR_OF_LPORT][COS_OM_ADPT_MAX_IP_DSCP_MAP_ENTRIES];
}COS_OM_ShmemData_T;


/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/* ---------------------------------------------------------------------
 * ROUTINE NAME  - COS_OM_InitiateSystemResources
 * ---------------------------------------------------------------------
 * PURPOSE  : Initiate share memory resources
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
void COS_OM_InitiateSystemResources() ;

/* FUNCTION NAME: COS_OM_GetShMemInfo
 *----------------------------------------------------------------------------------
 * PURPOSE: Get share memory info
 *----------------------------------------------------------------------------------
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 *----------------------------------------------------------------------------------
 * NOTES:   
 */
//void COS_OM_GetShMemInfo(SYSRSC_MGR_SEGID_T *segid_p, UI32_T *seglen_p) ;

/*---------------------------------------------------------------------------------
 * FUNCTION NAME: COS_OM_AttachSystemResources
 *---------------------------------------------------------------------------------
 * PURPOSE: Attach system resource for COS_OM in the context of the calling
 *          process.
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *---------------------------------------------------------------------------------*/
void COS_OM_AttachSystemResources(void) ;

/*---------------------------------------------------------------------------------
 * FUNCTION NAME: COS_OM_EnterMasterMode
 *---------------------------------------------------------------------------------
 * PURPOSE: Enter master mode
 * INPUT:   none
 * OUTPUT:  none
 * RETUEN:  none
 * NOTES:   none
 *---------------------------------------------------------------------------------*/
void COS_OM_EnterMasterMode (void) ;

/*---------------------------------------------------------------------------------
 * FUNCTION NAME: COS_OM_EnterSlaveMode
 *---------------------------------------------------------------------------------
 * PURPOSE: Enter slave mode
 * INPUT:   none
 * OUTPUT:  none
 * RETUEN:  none
 * NOTES:   none
 *---------------------------------------------------------------------------------*/
void COS_OM_EnterSlaveMode (void) ;

/*---------------------------------------------------------------------------------
 * FUNCTION NAME: COS_OM_ProvisionComplete
 *---------------------------------------------------------------------------------
 * PURPOSE: All provision commands are settle down.
 * INPUT:   none
 * OUTPUT:  none
 * RETUEN:  None
 * NOTES:
 *---------------------------------------------------------------------------------*/
void COS_OM_ProvisionComplete(void) ;

/*---------------------------------------------------------------------------------
 * FUNCTION NAME: COS_OM_SetTransitionMode
 *---------------------------------------------------------------------------------
 * PURPOSE: Sets to temporary transition mode
 * INPUT:   none
 * OUTPUT:  none
 * RETUEN:  none
 * NOTES:   none
 *---------------------------------------------------------------------------------*/
void COS_OM_SetTransitionMode (void) ;

/*---------------------------------------------------------------------------------
 * FUNCTION NAME: COS_OM_EnterTransitionMode
 *---------------------------------------------------------------------------------
 * PURPOSE: Enter transition mode
 * INPUT:   none
 * OUTPUT:  none
 * RETUEN:  none
 * NOTES:   none
 *---------------------------------------------------------------------------------*/
void COS_OM_EnterTransitionMode (void) ;

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - COS_OM_SetIpPrecedence2CosValueByLport
 * ---------------------------------------------------------------------
 * PURPOSE  : set ip precedence to cos mapping by lport
 * INPUT    : lport, ip_precedence, cos
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T COS_OM_SetIpPrecedence2CosValueByLport(UI32_T lport, UI8_T ip_precedence, UI8_T cos);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - COS_OM_GetIpPrecedence2CosValueByLport
 * ---------------------------------------------------------------------
 * PURPOSE  : get ip precedence to cos mapping by lport
 * INPUT    : lport, ip_precedence
 * OUTPUT   : cos
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T COS_OM_GetIpPrecedence2CosValueByLport(UI32_T lport, UI8_T ip_precedence, UI8_T *cos);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - COS_OM_SetIpDscp2CosValueByLport
 * ---------------------------------------------------------------------
 * PURPOSE  : set ip dscp to cos mapping by lport
 * INPUT    : lport, ip_dscp, cos
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T COS_OM_SetIpDscp2CosValueByLport(UI32_T lport, UI8_T ip_dscp, UI8_T cos);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - COS_OM_GetIpDscp2CosValueByLport
 * ---------------------------------------------------------------------
 * PURPOSE  : get ip dscp to cos by lport
 * INPUT    : lport, ip_dscp
 * OUTPUT   : cos
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T COS_OM_GetIpDscp2CosValueByLport(UI32_T lport, UI8_T ip_dscp, UI8_T *cos);


#endif /* COS_OM_H */
