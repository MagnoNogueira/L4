/*------------------------------------------------------------------------------
 * File_Name : L4_COS_MGR.C
 *
 * Purpose   : Provide service functions of L4-COS module
 *
 * 2003/3/31    : Jeff Kao Create
 *
 * Copyright(C)      Accton Corporation, 2002, 2003
 *
 * Note    :
 *------------------------------------------------------------------------------
 */

#include "cos_type.h"
#include "l4_cmn.h"
#include "sysfun.h"
#include "cos_vm.h"
#include "swctrl.h"
#include "pri_mgr.h"
#include "acl_type.h"

SYSFUN_DECLARE_CSC                    /* declare variables used for transition mode  */

/*------------------------------------------------------------------------------
 * 			LOCAL FUNCTION DECLARATIONS
 *------------------------------------------------------------------------------
 */
static void L4_COS_MGR_RegisterCallbackFunction(void);


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_AddFirstTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the callback function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_AddFirstTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_AddFirstTrunkMember(trunk_ifindex, member_ifindex);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_AddTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the callback function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_AddTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_AddTrunkMember(trunk_ifindex, member_ifindex);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_DelTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the callback function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_DelTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_DelTrunkMember(trunk_ifindex, member_ifindex);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_DelLastTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the callback function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_DelLastTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_DelLastTrunkMember(trunk_ifindex, member_ifindex);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}



/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_CosLportConfigAsic
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the cos function that will be registed to priority manager
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_CosLportConfigAsic(UI32_T l_port)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_CosLportConfigAsic(l_port);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}



/*------------------------------------------------------------------------------
 * Routine Name : L4_COS_MGR_RegisterCallbackFunction()
 *------------------------------------------------------------------------------
 * Function : Register Callback Function
 * Input    : None								                             +
 * Output   : None
 * Return   : None
 * Note     : There are 4 callback functions will be registered for trunk
 *------------------------------------------------------------------------------
 */
static void L4_COS_MGR_RegisterCallbackFunction(void)
{
    /* CallBack Function */
    SWCTRL_Register_TrunkMemberAdd1st_CallBack((void *)L4_COS_MGR_AddFirstTrunkMember);
    SWCTRL_Register_TrunkMemberAdd_CallBack((void *)L4_COS_MGR_AddTrunkMember);
    SWCTRL_Register_TrunkMemberDelete_CallBack((void *)L4_COS_MGR_DelTrunkMember);
    SWCTRL_Register_TrunkMemberDeleteLst_CallBack((void *)L4_COS_MGR_DelLastTrunkMember);
    PRI_MGR_RegisterCosChanged_CallBack((void *)L4_COS_MGR_CosLportConfigAsic);

} /*End of L4_COS_MGR_RegisterCallbackFunction */

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_COS_MGR_Initiate_System_Resources
 *------------------------------------------------------------------------------
 * PURPOSE:
 * INPUT : None
 * OUTPUT: None
 * RETURN: TRUE/FALSE
 * NOTES :
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_Initiate_System_Resources()
{
    COS_VM_Initiate_System_Resources();
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_COS_MGR_Create_InterCSC_Relation
 *------------------------------------------------------------------------------
 * PURPOSE: This function initializes all function pointer registration operations.
 * INPUT :  None
 * OUTPUT:  None
 * RETURN:  None
 * NOTES :
 *------------------------------------------------------------------------------
 */
void L4_COS_MGR_Create_InterCSC_Relation(void)
{
    L4_COS_MGR_RegisterCallbackFunction();
    COS_VM_Create_InterCSC_Relation();
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_SetTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the L4_COS_MGR to enter transition mdoe
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void L4_COS_MGR_SetTransitionMode()
{
    SYSFUN_SET_TRANSITION_MODE();
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -L4_COS_MGR_EnterMasterMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the L4_COS_MGR enter the master mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void L4_COS_MGR_EnterMasterMode()
{
    COS_VM_EnterMasterMode();
    SYSFUN_ENTER_MASTER_MODE();
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -L4_COS_MGR_EnterSlaveMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the L4_COS_MGR enter the slave mode
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void L4_COS_MGR_EnterSlaveMode()
{
    SYSFUN_ENTER_SLAVE_MODE();
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_EnterTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the L4_COS_MGR to enter transition mdoe
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void L4_COS_MGR_EnterTransitionMode()
{
    /* wait other callers leave */
    SYSFUN_ENTER_TRANSITION_MODE();

    COS_VM_EnterTransitionMode();
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_SetIpPrecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will enable or disable TOS/DSCP/COS mapping of system
 * INPUT:    flag -- COS_VM_ipprecedence_MAPPING_ENABLED/COS_VM_DSCP_MAPPING_ENABLED
 *                   /COS_VM_DISABLE_BOTH
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:  TOS/COS mapping and DSCP/COS mapping can not be enabled simultaneously
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_SetIpPrecedenceDscpMappingStatus (COS_VM_MAPPING_FLAG_T flag)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_SetIpPrecedenceDscpMappingStatus(flag);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_GetIpPrecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get TOS/DSCP/COS mapping status of system
 * INPUT:    none
 * OUTPUT:   flag -- COS_MGR_ipprecedence_MAPPING_ENABLED/COS_MGR_DSCP_MAPPING_ENABLED
 *                   /COS_MGR_DISABLE_BOTH
 * RETURN:   TRUE/FALSE
 * NOTE:  tos/COS mapping and DSCP/COS mapping can not be enabled simultaneously
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_GetIpPrecedenceDscpMappingStatus (COS_VM_MAPPING_FLAG_T *flag)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_GetIpPrecedenceDscpMappingStatus(flag);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_GetRunningIpPrecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the status of tos/DSCP/COS mapping of system
 * INPUT:    None
 * OUTPUT:   flag -- COS_MGR_MAPPING_ENABLED/COS_MGR_MAPPING_DISABLED
 * RETURN:   SYS_TYPE_GET_RUNNING_CFG_FAIL -- error (system is not in MASTER mode)
 *           SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE -- same as default
 *           SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different from default value
 * NOTE: default value is COS_MGR_MAPPING_DISABLED
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_COS_MGR_GetRunningIpPrecedenceDscpMappingStatus(COS_VM_MAPPING_FLAG_T *flag)
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    SYSFUN_USE_CSC(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_GetIpPrecedenceDscpMappingStatus(flag);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_SetTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will enable or disable TcpPort/COS mapping of system
 * INPUT:    flag -- COS_MGR_MAPPING_ENABLED/COS_MGR_MAPPING_DISABLED
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_SetTcpPortMappingStatus (COS_VM_TCPPORT_MAPPING_FLAG_T flag)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_SetTcpPortMappingStatus(flag);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_GetTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the status of TcpPort/COS mapping of system
 * INPUT:    None
 * OUTPUT:   flag -- COS_MGR_MAPPING_ENABLED/COS_MGR_MAPPING_DISABLED
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_GetTcpPortMappingStatus (COS_VM_TCPPORT_MAPPING_FLAG_T *flag)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_GetTcpPortMappingStatus(flag);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_GetRunningTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the status of TcpPort/COS mapping of system
 * INPUT:    None
 * OUTPUT:   flag -- COS_MGR_MAPPING_ENABLED/COS_MGR_MAPPING_DISABLED
 * RETURN:   SYS_TYPE_GET_RUNNING_CFG_FAIL -- error (system is not in MASTER mode)
 *           SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE -- same as default
 *           SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different from default value
 * NOTE: default value is COS_MGR_MAPPING_DISABLED
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_COS_MGR_GetRunningTcpPortMappingStatus (COS_VM_TCPPORT_MAPPING_FLAG_T *flag)
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    SYSFUN_USE_CSC(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_GetRunningTcpPortMappingStatus(flag);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_SetIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a tos/CoS map in tos_map_table of specific
 *           logical port
 * INPUT:    l_port, tos (0..7), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_SetIpPrecedenceToCos(UI32_T l_port , UI8_T tos , UI32_T cos)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_SetIpPrecedenceToCos(l_port , tos , cos);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_GetIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a tos/CoS map in tos_map_table of specific
 *           logical port
 * INPUT:    l_port, tos (0..7)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_GetIpPrecedenceToCos(UI32_T l_port, UI8_T tos, UI32_T *cos)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_GetIpPrecedenceToCos(l_port , tos , cos);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_DelIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tos/CoS map to its default value
 *           in tos_map_table of specific logical port
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_DelIpPrecedenceToCos(UI32_T l_port, UI8_T tos)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_DelIpPrecedenceToCos(l_port , tos);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_DelIpPrecedenceLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tos/CoS map to its default value
 *           in tos_map_table of specific logical port
 * INPUT:    l_port, tos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_DelIpPrecedenceLportAllEntry(UI32_T l_port)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_DelIpPrecedenceLportAllEntry(l_port);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_SetDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a Dscp/CoS map in dscp_map_table of specific
 *           logical port
 * INPUT:    l_port, dscp (0..63), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_SetDSCPToCos(UI32_T l_port, UI8_T dscp, UI32_T cos)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_SetDSCPToCos(l_port, dscp, cos);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
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
BOOL_T L4_COS_MGR_SetDSCPToCosStatus(UI32_T l_port, UI8_T dscp, BOOL_T status)
{
    BOOL_T ret=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        ret = COS_VM_SetDSCPToCosStatus(l_port, dscp, status);
    }

    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return ret;
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
BOOL_T L4_COS_MGR_GetDSCPToCosStatus(UI32_T l_port, UI8_T dscp, BOOL_T *status)
{
    BOOL_T ret=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        ret = COS_VM_GetDSCPToCosStatus(l_port, dscp, status);
    }

    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return ret;
}
/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_COS_MGR_GetRunningDSCPToCosStatus
 *------------------------------------------------------------------------------
 * PURPOSE: this function will set a dscp entry's status
 * INPUT:   l_port
 *          dscp, cos , status
 * OUTPUT:
 * RETURN: SYS_TYPE_Get_Running_Cfg_T
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_COS_MGR_GetRunningDSCPToCosStatus(UI32_T l_port, UI8_T dscp, BOOL_T *status)
{
    BOOL_T ret=FALSE;
    BOOL_T is_default;

    SYSFUN_USE_CSC(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        ret = COS_VM_GetDSCPToCosStatus(l_port, dscp, status);
    }


    if (ret == FALSE)
    {
        SYSFUN_RELEASE_CSC();
        L4_CMN_LeaveCriticalSection();
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    ret=COS_VM_IsDSCPToCosDefaultStatus(l_port, dscp, *status, &is_default);
    L4_CMN_LeaveCriticalSection();

    if(ret == FALSE)
    {
        SYSFUN_RELEASE_CSC();
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;

    }
    else
    {

        if(is_default == TRUE)
        {
            SYSFUN_RELEASE_CSC();
            return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
        }
        else
        {
            SYSFUN_RELEASE_CSC();
            return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
        }
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_GetDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a Dscp/CoS map in dscp_map_table of specific
 *           logical port
 * INPUT:    l_port, dscp (0..63)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_GetDSCPToCos(UI32_T l_port, UI8_T dscp, UI32_T *cos)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_GetDSCPToCos(l_port, dscp, cos);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_DelDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a Dscp/CoS map to its default value
 *           in dscp_map_table of specific logical port
 * INPUT:    l_port, dscp (0..63)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_DelDSCPToCos(UI32_T l_port, UI8_T dscp)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_DelDSCPToCos(l_port, dscp);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_DelDscpLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a Dscp/CoS map to its default value
 *           in dscp_map_table of specific logical port
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_DelDscpLportAllEntry(UI32_T l_port)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_DelDscpLportAllEntry(l_port);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_SetPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a TCP Port/CoS map in tcp_port_map_table
 *           of specific logical port
 * INPUT:    l_port, tcp port (1..65535), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE: 1. For GetNext functions, the entries in table need to be sorted
 *          according to tcp_port number
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_SetPortToCos(UI32_T l_port, UI16_T tcp_port, UI32_T cos)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_SetPortToCos(l_port, tcp_port, cos);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_GetPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a tcp_port/CoS map in tcp_port_map_table of specific
 *           logical port
 * INPUT:    l_port, tcp_port (0..65535)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_GetPortToCos(UI32_T l_port, UI16_T tcp_port, UI32_T *cos)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_GetPortToCos(l_port, tcp_port, cos);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_DelPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tcp_port/CoS map to its default value
 *           in tcp_port of specific logical port
 * INPUT:    l_port, tcp_port (0..65535)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_DelPortToCos(UI32_T l_port, UI16_T tcp_port)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_DelPortToCos(l_port, tcp_port);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_DelPortLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tcp_port/CoS map to its default value
 *           in tcp_port_map_table of specific logical port
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_DelPortLportAllEntry(UI32_T l_port)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_DelPortLportAllEntry(l_port);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_GetNextIpPrecedenceToCos
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
BOOL_T L4_COS_MGR_GetNextIpPrecedenceToCos(UI32_T *l_port, UI8_T *tos, UI32_T *cos)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_GetNextIpPrecedenceToCos(l_port, tos, cos);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_GetNextDSCPToCos
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
BOOL_T L4_COS_MGR_GetNextDSCPToCos(UI32_T *l_port, UI8_T *dscp, UI32_T *cos)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status =COS_VM_GetNextDSCPToCos(l_port, dscp, cos);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_GetNextPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the value of indexed entry
 *           in tcp_port_map_table of specific port and output next index
 * INPUT:    l_port -- logical port number
 *           tcp_port -- dscp index
 *           cos -- cos value
 * OUTPUT:   l_port, tcp_port -- next index
 * RETURN:   True/False
 * NOTE:     The returned value will base on the database of core layer
 *           rather than the database of ASIC
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_GetNextPortToCos(UI32_T *l_port , UI16_T *tcp_port , UI32_T *cos)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status =COS_VM_GetNextPortToCos(l_port, tcp_port, cos);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_COS_MGR_GetRunningIpPrecedenceToCos
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
SYS_TYPE_Get_Running_Cfg_T L4_COS_MGR_GetRunningIpPrecedenceToCos(UI32_T l_port , UI8_T tos, UI32_T *cos)
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    SYSFUN_USE_CSC(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_GetRunningIpPrecedenceToCos(l_port , tos , cos);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_COS_MGR_GetRunningDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default DSCP/COS mapping is successfully retrieved.
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
SYS_TYPE_Get_Running_Cfg_T L4_COS_MGR_GetRunningDSCPToCos(UI32_T l_port , UI8_T dscp, UI32_T *cos)
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    SYSFUN_USE_CSC(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_GetRunningDSCPToCos(l_port , dscp, cos);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_COS_MGR_GetRunningPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default TCPPort/COS mapping is successfully retrieved.
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
SYS_TYPE_Get_Running_Cfg_T L4_COS_MGR_GetRunningPortToCos(UI32_T l_port , UI16_T tcp_port, UI32_T *cos)
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    SYSFUN_USE_CSC(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_GetRunningPortToCos(l_port , tcp_port, cos);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_COS_MGR_GetNextRunningIpPrecedenceToCos
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

SYS_TYPE_Get_Running_Cfg_T L4_COS_MGR_GetNextRunningIpPrecedenceToCos(UI32_T *l_port , UI8_T *tos, UI32_T *cos)
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    SYSFUN_USE_CSC(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_GetNextRunningIpPrecedenceToCos(l_port, tos, cos);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_COS_MGR_GetNextRunningDSCPToCos
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
SYS_TYPE_Get_Running_Cfg_T L4_COS_MGR_GetNextRunningDSCPToCos(UI32_T *l_port , UI8_T *dscp, UI32_T *cos)
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    SYSFUN_USE_CSC(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_GetNextRunningDSCPToCos(l_port , dscp, cos);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_COS_MGR_GetNextRunningPortToCos
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
SYS_TYPE_Get_Running_Cfg_T L4_COS_MGR_GetNextRunningPortToCos(UI32_T *l_port , UI16_T *tcp_port, UI32_T *cos)
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    SYSFUN_USE_CSC(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_GetNextRunningPortToCos(l_port , tcp_port, cos);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}





/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_PortCopy
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will copy the source port set to destport
 * INPUT:    map_table_type, src_port, dst_port_list
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE: Since it have to support multi-thread, so it is not possible to separate
 *       parameters configuration from copy operation
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_PortCopy(UI32_T map_table_type, UI32_T src_port, UI8_T *dst_port_list)
{
    BOOL_T status=FALSE;

    SYSFUN_USE_CSC(FALSE);

    L4_CMN_EnterCriticalSection();

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        status = COS_VM_PortCopy(map_table_type, src_port, dst_port_list);
    }
    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return status;
}



/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_AddCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    ifindex, acl name, cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     the key is cos_entry.acl_name
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_AddCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
    BOOL_T ret;

    SYSFUN_USE_CSC(FALSE);
    L4_CMN_EnterCriticalSection();
    if(SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        ret = COS_VM_AddCosEntry(ifindex, cos_entry);
    }

    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return ret;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_DelCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    ifindex, acl name, cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     the key is cos_entry.acl_name
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_DelCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
    BOOL_T ret;

    SYSFUN_USE_CSC(FALSE);
    L4_CMN_EnterCriticalSection();
    if(SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        ret = COS_VM_DelCosEntry(ifindex, cos_entry);
    }

    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return ret;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_SetIpCosEntryRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    ifindex, cos_entry
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     the key is (ifindex, cos_entry.acl_name)
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_SetCosEntryRowStatus(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
    BOOL_T ret;

    SYSFUN_USE_CSC(FALSE);
    L4_CMN_EnterCriticalSection();
    if(SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        ret = COS_VM_SetCosEntryRowStatus(ifindex, cos_entry);
    }

    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return ret;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_SetCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    ifindex, cos_entry
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     the key is (ifindex, cos_entry.acl_name)
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_SetCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
    BOOL_T ret;

    SYSFUN_USE_CSC(FALSE);
    L4_CMN_EnterCriticalSection();
    if(SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        ret = COS_VM_SetCosEntry(ifindex, cos_entry);
    }

    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return ret;
}




/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_GetCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get the next ACL to cos mapping
 * INPUT:    ifindex,
 * OUTPUT:   acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     the key is (ifindex, acl_mapping->acl_name)
 *           supposely only the SNMP will call this fuction
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_GetCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
    BOOL_T ret;

    SYSFUN_USE_CSC(FALSE);
    L4_CMN_EnterCriticalSection();
    if(SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        ret = COS_VM_GetCosEntry(ifindex, cos_entry);
    }

    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return ret;
}




/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_GetNextCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  for CLI and WWW to get the next ACL to cos
 * INPUT:    ifindex,
 * OUTPUT:   acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     use the *ifidnex, acl_mapping->acl_name[0]='\0' to get the first entry
 *           the key is (*ifindex, acl_mapping->acl_name)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_GetNextCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
    BOOL_T ret;

    SYSFUN_USE_CSC(FALSE);
    L4_CMN_EnterCriticalSection();
    if(SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        ret = COS_VM_GetNextCosEntry(ifindex, cos_entry);
    }

    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return ret;
}



/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_SNMPGetNextCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to get the next acl map to cos
 * INPUT:    ifindex
 * OUTPUT:   ifindex, acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     use the *ifidnex, acl_mapping->acl_name[0]='\0' to get the first entry
 *           the key is (*ifindex, acl_mapping->acl_name)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_SNMPGetNextCosEntry(UI32_T *ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
    BOOL_T ret;

    SYSFUN_USE_CSC(FALSE);
    L4_CMN_EnterCriticalSection();
    if(SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        ret = COS_VM_SNMPGetNextCosEntry(ifindex, cos_entry);
    }

    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return ret;
}




/*------------------------------------------------------------------------------
 * ROUTINE NAME  -L4_COS_MGR_GetNextRunningCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  for CLI to get the port acl binding
 * INPUT  :  if_index, entry
 * OUTPUT :  entry
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES  :  use the ifidnex, acl_mapping->acl_name[0]='\0' to get the first entry
 *           the key is (ifindex, acl_mapping->acl_name)
 *
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_COS_MGR_GetNextRunningCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
    UI32_T ret;

    SYSFUN_USE_CSC(FALSE);
    L4_CMN_EnterCriticalSection();
    if(SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_MASTER_MODE)
    {
        ret = COS_VM_GetNextRunningCosEntry(ifindex, cos_entry);
    }

    L4_CMN_LeaveCriticalSection();
    SYSFUN_RELEASE_CSC();
    return ret;
}



/*
 obsolete part
*/

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_SetIpAclToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    l_port, acl name, cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE: 1. if strlen(acl_name) == 0 , means remove ACL binding, don't care cos parameter.
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_SetIpAclToCos(UI32_T l_port, UI8_T *acl_name, UI32_T cos)
{
   return FALSE;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_SetMacAclToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    l_port, acl name, cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE: 1. if strlen(acl_name) == 0 , means remove ACL binding, don't care cos parameter.
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_SetMacAclToCos(UI32_T l_port, UI8_T *acl_name, UI32_T cos)
{
    return FALSE;
}




/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_GetAclToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a ACL for COS mapping
 *           of specific logical port
 * INPUT:
 * OUTPUT:   l_port, acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     use *lport= 0 to get the first entry
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_GetNextAclToCos(UI32_T *l_port, COS_AclCosMapping_T *acl_mapping)
{

    return FALSE;
}



/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_COS_MGR_GetAclToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a ACL for COS mapping
 *           of specific logical port
 * INPUT:    l_port,
 * OUTPUT:   acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
BOOL_T L4_COS_MGR_GetAclToCos(UI32_T l_port, COS_AclCosMapping_T *acl_mapping)
{
    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  -COS_VM_GetRunningIntfAclCosMapping
 *------------------------------------------------------------------------------
 * PURPOSE:  for CLI to get the port acl binding
 * INPUT  :  if_index, entry
 * OUTPUT :  entry
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES  :  if entry->in_ip_acl ='\0' means disable, same as the other variable
 *
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_COS_MGR_GetRunningIntfAclCosMapping(UI32_T if_index, COS_AclCosMapping_T *acl_mapping )
{
   return SYS_TYPE_GET_RUNNING_CFG_FAIL;
}


/*pttch new*/
/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_COS_MGR_HandleHotInsertion
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
void L4_COS_MGR_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default)
{
    COS_VM_HandleHotInsertion(starting_port_ifindex, number_of_port, use_default);
    return ;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_COS_MGR_HandleHotRemoval
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
void L4_COS_MGR_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port)
{
    COS_VM_HandleHotRemoval(starting_port_ifindex, number_of_port);
    return ;
}


