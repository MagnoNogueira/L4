/* FUNCTION NAME: L4_MGR.C
* PURPOSE:
* NOTES:
*
*REASON:
*    DESCRIPTION:
*    CREATOR:
*    Date       04/07/2005
*
* Copyright(C)      Accton Corporation, 2005
*/

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "sys_cpnt.h"
#include "sys_module.h"
#include "l_stdlib.h" /* for Hton32 */
#include "sys_callback_mgr.h"
#include "rule_type.h"
#include "rule_mgr.h"
#include "rule_om.h"
#include "sysfun.h"
#include "backdoor_mgr.h"
#if (SYS_CPNT_DAI == TRUE)
#include "dai_type.h"
#endif
#include "l4_mgr.h"
#include "leaf_es3626a.h"
#include "swctrl.h"
//eli test #include "pri_mgr.h"
#include "stktplg_om.h"
#include <string.h>
#include "sys_bld.h"

#if (SYS_CPNT_COS == TRUE)
#include "cos_vm.h"
#endif
#if(SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)
#include "marker_vm.h"
#endif

/* NAMING CONSTANT DECLARATIONS
 */
/* MACRO FUNCTION DECLARATIONS
 */

#define L4_MGR_USE_CSC_CHECK_OPER_MODE(RET_VAL) \
    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE) { \
       return (RET_VAL); \
    }

#define L4_MGR_USE_CSC_CHECK_OPER_MODE_WITHOUT_RETURN_VALUE() \
    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE) { \
        SYSFUN_RELEASE_CSC(); \
        return; \
    }

#define L4_MGR_RETURN_AND_RELEASE_CSC(RET_VAL) { \
        return (RET_VAL); \
    }

#define L4_MGR_RETURN_AND_RELEASE_CSC_WITHOUT_RETUEN_VALUE() { \
        return; \
    }

#define L4_MGR_IS_BAD_LPORT(lport)              \
    ((lport < 1) || (SYS_ADPT_TOTAL_NBR_OF_LPORT < lport))

#define L4_MGR_CHECK_LPORT(lport, ret)      {   \
            if (L4_MGR_IS_BAD_LPORT(lport))     \
                return ret;                     \
    }

#define L4_MGR_LOCK()

#define L4_MGR_UNLOCK()

#ifndef ASSERT
#define ASSERT(eq)
#endif

#define L4_MGR_IS_IP_ACL(acl_type)                 ((acl_type == RULE_TYPE_IP_EXT_ACL) || \
                                                    (acl_type == RULE_TYPE_IP_STD_ACL))

#define L4_MGR_IS_TCP_ACL(acl_protocol)            (acl_protocol == RULE_TYPE_ACL_TCP_PROTOCOL)

#define L4_MGR_IS_MAC_ACL(acl_type)                (acl_type == RULE_TYPE_MAC_ACL)

#define L4_MGR_IS_IPV6_ACL(acl_type)               ((acl_type == RULE_TYPE_IPV6_EXT_ACL) || \
                                                    (acl_type == RULE_TYPE_IPV6_STD_ACL))

#define L4_MGR_IS_ARP_ACL(acl_type)                (acl_type == RULE_TYPE_ARP_ACL)


/* DATA TYPE DECLARATIONS
 */
 /* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

 /* STATIC VARIABLE DECLARATIONS */


/*  declare variables used for Transition mode  */
SYSFUN_DECLARE_CSC

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static void L4_MGR_COS_RegisterCallbackFunction(void);

/*------------------------------------------------------------------------------
 * Routine Name : L4_MGR_InitiateSystemResources
 *------------------------------------------------------------------------------
 * Function : Initialize L4's MGR .
 * Input    : None
 * Output   : None
 * Return   : TRUE/FALSE
 * Note     : None
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_InitiateSystemResources(void)
{
    BOOL_T ret=TRUE;

#if (SYS_CPNT_COS == TRUE)
    COS_VM_Initiate_System_Resources();
#endif

#if(SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)
    //MARKER_VM_Initiate_System_Resources();
#endif

#if (SYS_CPNT_QOS_V2 == TRUE)
    ret &= RULE_MGR_InitiateSystemResources();
#endif

    return ret;
}/* End of L4_MGR_InitiateSystemResources() */

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_Create_InterCSC_Relation
 *------------------------------------------------------------------------------
 * PURPOSE: This function initializes all function pointer registration operations.
 * INPUT :  None
 * OUTPUT:  None
 * RETURN:  None
 * NOTES :  None
 *------------------------------------------------------------------------------
 */
void L4_MGR_Create_InterCSC_Relation()
{
#if (SYS_CPNT_COS == TRUE)
    L4_MGR_COS_RegisterCallbackFunction();
#endif

#if (SYS_CPNT_QOS_V2 == TRUE)
    RULE_MGR_Create_InterCSC_Relation();
#endif

#if (L4_SUPPORT_ACCTON_BACKDOOR == TRUE)
    BACKDOOR_MGR_Register_SubsysBackdoorFunc_CallBack("l4",
        SYS_BLD_L4_GROUP_IPCMSGQ_KEY,L4_Backdoor_CallBack);
#endif

    return ;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_EnterMasterMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the L4 enter the master mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void L4_MGR_EnterMasterMode()
{
#if (SYS_CPNT_QOS_V2 == TRUE)

#endif

#if (SYS_CPNT_COS == TRUE)
    COS_VM_EnterMasterMode();
#endif

#if(SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)
    MARKER_VM_EnterMasterMode();
#endif

#if (SYS_CPNT_QOS_V2 == TRUE)
    {
        UI8_T                           cpu_mac[SYS_ADPT_MAC_ADDR_LEN] = {0};

        RULE_MGR_EnterMasterMode();

        if (FALSE == SWCTRL_GetCpuMac(cpu_mac))
        {
            ASSERT(0);
        }

        /* OSPFv3
         */
#if (SYS_CPNT_OSPF6 == TRUE)
        if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_OSPF6, NULL))
            printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_OSPF6 failed");
#endif

#if (SYS_CPNT_DVMRP == TRUE)
        if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_DVMRP, NULL))
            printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_DVMRP failed");
#endif
#if 0
        if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_VRRP, NULL))
            printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_VRRP failed");
#endif
        if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_L2_SLF, NULL))
            printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_L2_SLF failed");

#if 0
        if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_IP_BCAST, NULL))
            printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_IP_BCAST failed");
#endif

        if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_ALL_HOST, NULL))
            printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_ALL_HOST failed");
        if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_ALL_ROUTER, NULL))
            printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_ALL_ROUTER failed");
        if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_MY_MAC_MY_IP, NULL))
            printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_MY_MAC_MY_IP failed");
        if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_IP_OPTION, NULL))
            printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_IP_OPTION failed");

        if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_BPDU, NULL))
            printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_BPDU failed");
        if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_L2CP, NULL))
            printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_L2CP failed");

#if (SYS_CPNT_ROUTING == TRUE)
        if (RULE_TYPE_OK != RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_MY_MAC_ARP, NULL))
        {
            printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_MY_MAC_ARP failed");
        }
#endif /* SYS_CPNT_ROUTING */

#if (SYS_CPNT_CRAFT_PORT == TRUE) && (SYS_CPNT_CRAFT_PORT_MODE == SYS_CPNT_CRAFT_PORT_MODE_FRONT_PORT_CRAFT_PORT)
        if (RULE_TYPE_OK != RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_Packettype_MY_MAC_CRAFT_PORT, NULL))
        {
            printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_Packettype_MY_MAC_CRAFT_PORT failed");
        }
#endif /* SYS_CPNT_CRAFT_PORT */

    }
#endif/* #if (SYS_CPNT_QOS_V2 == TRUE) */

    /* set mgr in master mode */
    SYSFUN_ENTER_MASTER_MODE();

    return;
}/* End of L4_MGR_EnterMasterMode() */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_EnterSlaveMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the L4 enter the Slave mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void L4_MGR_EnterSlaveMode()
{
#if (SYS_CPNT_COS == TRUE)
    COS_VM_EnterSlaveMode();
#endif

#if(SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)
    MARKER_VM_EnterSlaveMode();
#endif

#if (SYS_CPNT_QOS_V2 == TRUE)
    RULE_MGR_EnterSlaveMode();
#endif

    SYSFUN_ENTER_SLAVE_MODE();
    return;
}/* End of L4_MGR_EnterSlaveMode() */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_SetTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE:  Set transition mode
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void L4_MGR_SetTransitionMode(void)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    /* BODY */
#if (SYS_CPNT_QOS_V2 == TRUE)
    RULE_MGR_SetTransitionMode();
#endif

    /* set transition flag to prevent calling request */
    SYSFUN_SET_TRANSITION_MODE();
    return;
}/* End of L4_MGR_SetTransitionMode() */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_EnterTransitionMode Mode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the L4 enter the transition mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void L4_MGR_EnterTransitionMode()
{
#if (SYS_CPNT_COS == TRUE)
    COS_VM_EnterTransitionMode();
#endif
#if(SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)
    MARKER_VM_EnterTransitionMode();
#endif

#if (SYS_CPNT_QOS_V2 == TRUE)
    RULE_MGR_EnterTransitionMode();
#endif

    /* wait other callers leave */
    SYSFUN_ENTER_TRANSITION_MODE();

    return;
}/* End of L4_MGR_EnterTransitionMode() */


/* Handle Timer Event */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_ProcessTimerEvent
 *------------------------------------------------------------------------------
 * PURPOSE: Process timer alarm event from timer
 * INPUT:   None.
 * OUTPUT:  None.
 * RETURN:  None.
 * NOTES:   called by L4_GROUP_Mgr_Thread_Function_Entry
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_ACL_ProcessTimerEvent()
{
#if 1
    BOOL_T              rm_ret = FALSE;
    L4_MGR_LOCK();
    rm_ret = RULE_MGR_ProcessEventTimer();
    L4_MGR_UNLOCK();

    return TRUE;
#else

#if (SYS_CPNT_TIME_BASED_ACL == TRUE) && 0
    BOOL_T              rm_ret = FALSE;
    static BOOL_T       real_time_is_active = FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    /* check real time is active
       if not active, return false directly.
     */

    if (FALSE == real_time_is_active)
    {
        if (TRUE == SYS_TIME_IsTimeModify())
        {
            real_time_is_active = TRUE;
        }
        else /*not find valid clock*/
        {
            L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
        }
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_ProcessEventTimer();
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
#else
    return TRUE;
#endif /* #if (SYS_CPNT_TIME_BASED_ACL == TRUE) */
#endif

}


#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
/*--------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_ProcessTimeRangeStatusChange
 *---------------------------------------------------------------------------
 * PURPOSE: Process timer alarm event from timer
 * INPUT:   isChanged_list, status_list.
 * OUTPUT:  None.
 * RETURN:  None.
 * NOTES:   called by L4_GROUP_Mgr_Thread_Function_Entry
 *---------------------------------------------------------------------------
 */
BOOL_T L4_MGR_ACL_ProcessTimeRangeStatusChange(UI8_T *isChanged_list, UI8_T *status_list)
{
    BOOL_T rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_ProcessTimeRangeStatusChange(isChanged_list, status_list);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}
#endif /* #if (SYS_CPNT_TIME_BASED_ACL  == TRUE) */

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_HandleHotInsertion
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
void L4_MGR_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default)
{
#if(SYS_CPNT_COS == TRUE)
    L4_MGR_COS_HandleHotInsertion(starting_port_ifindex,number_of_port,use_default);
#endif

#if(SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)
    L4_MGR_MARKER_HandleHotInsertion(starting_port_ifindex,number_of_port,use_default);
#endif

    RULE_MGR_HandleHotInsertion(starting_port_ifindex, number_of_port);

    /* FIXME: The rule not for L4 should be remove to another place
     */
#if (SYS_CPNT_QOS_V2 == TRUE)
#define L4_MGR_TRAP_PACKET_TO_CPU(pkt_type)                                    \
    {                                                                          \
        if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, pkt_type, NULL))   \
        {                                                                      \
            printf("[%s] Set %s failed\r\n", __FUNCTION__, #pkt_type);         \
        }                                                                      \
    }

#if (SYS_CPNT_OSPF6 == TRUE)
    L4_MGR_TRAP_PACKET_TO_CPU(RULE_TYPE_PacketType_OSPF6);
#endif /* SYS_CPNT_OSPF6 */

#if (SYS_CPNT_DVMRP == TRUE)
    L4_MGR_TRAP_PACKET_TO_CPU(RULE_TYPE_PacketType_DVMRP);
#endif /* SYS_CPNT_DVMRP */

    L4_MGR_TRAP_PACKET_TO_CPU(RULE_TYPE_PacketType_L2_SLF);
    L4_MGR_TRAP_PACKET_TO_CPU(RULE_TYPE_PacketType_ALL_HOST);
    L4_MGR_TRAP_PACKET_TO_CPU(RULE_TYPE_PacketType_ALL_ROUTER);
    L4_MGR_TRAP_PACKET_TO_CPU(RULE_TYPE_PacketType_MY_MAC_MY_IP);
    L4_MGR_TRAP_PACKET_TO_CPU(RULE_TYPE_PacketType_IP_OPTION);
    L4_MGR_TRAP_PACKET_TO_CPU(RULE_TYPE_PacketType_BPDU);
    L4_MGR_TRAP_PACKET_TO_CPU(RULE_TYPE_PacketType_L2CP);

#if (SYS_CPNT_ROUTING == TRUE)
    L4_MGR_TRAP_PACKET_TO_CPU(RULE_TYPE_PacketType_MY_MAC_ARP);
#endif /* SYS_CPNT_ROUTING */

#if (SYS_CPNT_CRAFT_PORT == TRUE) && (SYS_CPNT_CRAFT_PORT_MODE == SYS_CPNT_CRAFT_PORT_MODE_FRONT_PORT_CRAFT_PORT)
    L4_MGR_TRAP_PACKET_TO_CPU(RULE_TYPE_Packettype_MY_MAC_CRAFT_PORT);
#endif /* SYS_CPNT_CRAFT_PORT */

#undef L4_MGR_TRAP_PACKET_TO_CPU
#endif/* #if (SYS_CPNT_QOS_V2 == TRUE) */

    return;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_HandleHotRemoval
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
void L4_MGR_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port)
{
#if 0
#if(SYS_CPNT_ACL == TRUE)
    L4_ACL_MGR_HandleHotRemoval(starting_port_ifindex,number_of_port);
#endif
#endif
#if(SYS_CPNT_COS == TRUE)
    L4_MGR_COS_HandleHotRemoval(starting_port_ifindex,number_of_port);
#endif

#if(SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)
    L4_MGR_MARKER_HandleHotRemoval(starting_port_ifindex,number_of_port);
#endif
#if 0
#if(SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    L4_DS_MGR_HandleHotRemoval(starting_port_ifindex,number_of_port);
#endif
#endif

#if (SYS_CPNT_QOS_V2 == TRUE)
    RULE_MGR_HandleHotRemoval(starting_port_ifindex, number_of_port);
#endif
    return;
}

/* DiffServ port table */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_BindPort2PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to policy map.
 * INPUT:    uport_ifindex, policy_map_name, inout_profile
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     One port only can bind one policy map.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_BindPort2PolicyMap(UI32_T uport_ifindex, const char *policy_map_name, RULE_TYPE_InOutDirection_T inout_profile)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;
    UI32_T  policy_map_index;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetPolicyMapIdByName(policy_map_name, &policy_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }
    L4_MGR_LOCK();
    rm_ret = RULE_MGR_BindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index);

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    if (RULE_TYPE_OK == rm_ret)
    {
        if (RULE_TYPE_IS_CPU_PORT(uport_ifindex))
        {
            if (RULE_TYPE_OK == RULE_OM_IsBundleRateSet(policy_map_index))
            {
                rm_ret = RULE_TYPE_NOT_SUPPORT_BUNDLE_RATE_CONTORL;
            }
        }
    }
#endif

    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_UnBindPortFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Unbind a port from policy map.
 * INPUT:    uport_ifindex, policy_map_name, inout_profile
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_UnBindPortFromPolicyMap(UI32_T uport_ifindex, const char *policy_map_name, RULE_TYPE_InOutDirection_T inout_profile)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;
    UI32_T  policy_map_index;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetPolicyMapIdByName(policy_map_name, &policy_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_UnBindPortFromPolicyMap(uport_ifindex, inout_profile, policy_map_index);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_BindPort2DynamicPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to dynamic policy map
 * INPUT:    uport_ifindex, policy_map_name, inout_profile
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     1. One port only can bind one dynamic policy map.
 *           2. The dynamic policy map is got from authentication server.
 *           3. When apply dynamic policy map, the user configured policy map
 *              on this port should be disabled.
 *           4. If the port already bind a dynamic policy map, then the bind
 *              or unbind action shall be effected after the dynamic policy
 *              map be unbinded.
 *           5. The user policy map should be restored to enable when dynamic
 *              policy be unbinded.
 *           6. The changes by this function should not be saved to configuration
 *              file.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_BindPort2DynamicPolicyMap(UI32_T uport_ifindex, char *policy_map_name, RULE_TYPE_InOutDirection_T inout_profile)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;
    UI32_T  policy_map_index;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetPolicyMapIdByName(policy_map_name, &policy_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }
    L4_MGR_LOCK();
    /*Kevin:need to input inout_profile?*/
    rm_ret = RULE_MGR_BindPort2DynamicPolicyMap(uport_ifindex, policy_map_index);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_Qos_UnBindPortFromDynamicPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Unbind a port from dynamic policy map.
 * INPUT:    uport_ifindex, policy_map_name, inout_profile
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     1. One port only can bind one dynamic policy map.
 *           2. The dynamic policy map is got from authentication server.
 *           3. When apply dynamic policy map, the user configured policy map
 *              on this port should be disabled.
 *           4. If the port already bind a dynamic policy map, then the bind
 *              or unbind action shall be effected after the dynamic policy
 *              map be unbinded.
 *           5. The user policy map should be restored to enable when dynamic
 *              policy be unbinded.
 *           6. The changes by this function should not be saved to configuration
 *              file.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_Qos_UnBindPortFromDynamicPolicyMap(UI32_T uport_ifindex)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_UnBindPortFromDynamicPolicyMap(uport_ifindex);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}
#endif /* #if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetPolicyMapNameByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get binding policy map's name of port.
 * INPUT:    uport_ifinde
 * OUTPUT:   policy_map_name.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetPolicyMapNameByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    char *policy_map_name)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetPolicyMapNameByPort(uport_ifindex, inout_profile, policy_map_name);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetRunningPolicyMapNameByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running binding policy map's name of port.
 * INPUT:    uport_ifindex
 * OUTPUT:   uport_ifindex, policy_map_name.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_QoS_GetRunningPolicyMapNameByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    char *policy_map_name)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;
    UI32_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_MGR_LOCK();
    if (TRUE == RULE_OM_IsBindDynamicPolicyMapOnPort(uport_ifindex, inout_profile))
    {
        rm_ret = RULE_OM_GetUserCfgPolicyMapNameOnPort(uport_ifindex, inout_profile, policy_map_name);
    }
    else
    {
        rm_ret = RULE_MGR_GetPolicyMapNameByPort(uport_ifindex, inout_profile, policy_map_name);
    }
    L4_MGR_UNLOCK();
    if(rm_ret == RULE_TYPE_OK)
    {
        ret = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        ret = SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/* DiffServ policy-map*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetPolicyMapNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get name of policy map by it's index.
 * INPUT:    policy_map_id
 * OUTPUT:   policy_map_name.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetPolicyMapNameById(UI32_T policy_map_id, char *policy_map_name)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetPolicyMapNameById(policy_map_id, policy_map_name);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetPolicyMapIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of policy map by it's name.
 * INPUT:    policy_map_name
 * OUTPUT:   policy_map_id.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetPolicyMapIdByName(const char *policy_map_name, UI32_T *policy_map_id)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetPolicyMapIdByName(policy_map_name, policy_map_id);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_CreatePolicyMapByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Create a policy map entry by it's name
 * INPUT:    policy_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_CreatePolicyMapByName(const char *policy_map_name)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_CreatePolicyMap(policy_map_name);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_RenamePolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Rename a policy map
 * INPUT:    old_policy_map_name, new_policy_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_RenamePolicyMap(const char *old_policy_map_name, const char *new_policy_map_name)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;
    UI32_T  policy_map_index;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetPolicyMapIdByName(old_policy_map_name, &policy_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_SetPolicyMapNameById(policy_map_index, new_policy_map_name);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetPolicyMapDescByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Set description of a policy map by it's name
 * INPUT:    policy_map
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     Key: policy_map_name.
 *------------------------------------------------------------------------------
 */
 /*Wuli??*/
UI32_T L4_MGR_QoS_SetPolicyMapDescByName(const char *policy_map_name, const char *description)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;
    UI32_T  policy_map_index;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetPolicyMapIdByName(policy_map_name, &policy_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_SetPolicyMapDescById(policy_map_index, description);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_DelPolicyMapByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a policy map entry by it's name
 * INPUT:    policy_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_DelPolicyMapByName(const char *policy_map_name)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;
    UI32_T  policy_map_index;
    UI32_T  port_nbr;
    UI8_T   port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST] = {0};

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetPolicyMapIdByName(policy_map_name, &policy_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    if (RULE_TYPE_OK != RULE_OM_GetDynamicBindPortListByPolicyMapName(policy_map_name, port_list, &port_nbr))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    rm_ret = RULE_MGR_DelPolicyMapById(policy_map_index);
    L4_MGR_UNLOCK();

    SYS_CALLBACK_MGR_AnnouncePolicyMapDeleted(SYS_MODULE_ACL, (char*)policy_map_name, port_list);

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next policy map entry by it's name
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map_index, policy_map.
 * RETURN:   Error code.
 * NOTE:     Key: policy_map_index.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetNextPolicyMap(UI32_T *policy_map_index, RULE_TYPE_UI_PolicyMap_T *policy_map)
{
    UI32_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    ret = RULE_MGR_GetNextUIPolicyMap(policy_map_index, policy_map);

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextRunningPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running policy map entry.
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map_index, policy_map.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_QoS_GetNextRunningPolicyMap(UI32_T *policy_map_index, RULE_TYPE_UI_PolicyMap_T *policy_map)
{
    UI32_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    ret = RULE_MGR_GetNextUIPolicyMap(policy_map_index, policy_map);
    if (RULE_TYPE_OK != ret)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE);
    }

    L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_SUCCESS);
}

/* DiffServ policy-map element*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_AddElement2PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Add a element to policy map
 * INPUT:    policy_map_name, class_map_name, meter_entry, action_entry
 * OUTPUT:   NONE.
 * RETURN:   RULE_TYPE_OK/RULE_TYPE_FAIL/
 *           RULE_TYPE_METER_FAIL/RULE_TYPE_ACTION_FAIL
 * NOTE:     if meter_entry != NULL, set meter_entry->row_status = L4_MGR_ROWSTATUS_CREATEANDGO
 *           if action_entry != NULL, set action_entry->row_status = L4_MGR_ROWSTATUS_CREATEANDGO
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_AddElement2PolicyMap(const char *policy_map_name, const char *class_map_name, RULE_TYPE_TBParamEntry_T *meter_entry, RULE_TYPE_UI_Action_T *action_entry)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;
    UI32_T  policy_map_index;
    UI32_T  class_map_index;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetPolicyMapIdByName(policy_map_name, &policy_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_POLICY_MAP_NONEXISTED);
    }
    if(RULE_MGR_GetClassMapIdByName(class_map_name, &class_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_CLASS_MAP_NONEXISTED);
    }

    L4_MGR_LOCK();

    rm_ret = RULE_MGR_AddElement2PolicyMap(policy_map_index, class_map_index);

    if((RULE_TYPE_OK == rm_ret) && (meter_entry != NULL))
    {
            // meter_entry->meter_model = VAL_diffServMeterModel_flow; /* only support flow */
        if (RULE_TYPE_OK != RULE_MGR_SetElementMeterFromPolicyMap(
                policy_map_index, class_map_index, meter_entry))
        {
            rm_ret = RULE_TYPE_METER_FAIL;
        }
    }

    if((RULE_TYPE_OK == rm_ret) && (action_entry != NULL))
    {
        if (RULE_TYPE_OK != RULE_MGR_SetElementActionFromPolicyMap(
                policy_map_index, class_map_index, action_entry))
        {
            rm_ret = RULE_TYPE_ACTION_FAIL;
        }
    }

    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_DelElementMeterFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete meter of element from policy map
 * INPUT:    policy_map_name, class_map_name, meter_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     set meter_entry->row_status = L4_MGR_ROWSTATUS_DESTROY
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_DelElementMeterFromPolicyMap(const char *policy_map_name, const char *class_map_name, RULE_TYPE_TBParamEntry_T *meter_entry)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;
    UI32_T  policy_map_index;
    UI32_T  class_map_index;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetPolicyMapIdByName(policy_map_name, &policy_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }
    if(RULE_MGR_GetClassMapIdByName(class_map_name, &class_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }
    /* Kevin: get meter_index by (policy_map,class_map,meter_entry) */

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_DelElementMeterFromPolicyMap(policy_map_index, class_map_index, meter_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_DelElementActionFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete action of element from policy map
 * INPUT:    policy_map_name, class_map_name, action_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     set action_entry->row_status = L4_MGR_ROWSTATUS_DESTROY
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_DelElementActionFromPolicyMap(const char *policy_map_name, const char *class_map_name, RULE_TYPE_UI_Action_T *action_entry)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;
    UI32_T  policy_map_index;
    UI32_T  class_map_index;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetPolicyMapIdByName(policy_map_name, &policy_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }
    if(RULE_MGR_GetClassMapIdByName(class_map_name, &class_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_DelElementActionFromPolicyMap(policy_map_index, class_map_index, action_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_DelElementFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a element from policy map
 * INPUT:    policy_map_name, class_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_DelElementFromPolicyMap(const char *policy_map_name, const char *class_map_name)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;
    UI32_T  policy_map_index;
    UI32_T  class_map_index;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetPolicyMapIdByName(policy_map_name, &policy_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }
    if(RULE_MGR_GetClassMapIdByName(class_map_name, &class_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_DelElementFromPolicyMap(policy_map_index, class_map_index);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextElementFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next element of policy map
 * INPUT:    policy_map_index, element_entry
 * OUTPUT:   element_entry.
 * RETURN:   Error code.
 * NOTE:     Key: policy_map_index, element_entry->precedent
 *           element_entry->precedent = 0 to get first element
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetNextElementFromPolicyMap(UI32_T policy_map_index, RULE_TYPE_UI_PolicyElement_T *element_entry)
{
    UI32_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    ret = RULE_MGR_GetNextElementFromPolicyMap(policy_map_index, element_entry);

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextRunningElementFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete next running element of policy map
 * INPUT:    policy_map_index, element_entry
 * OUTPUT:   element_entry.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     Key: policy_map_index, element_entry->precedent
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_QoS_GetNextRunningElementFromPolicyMap(UI32_T policy_map_index, RULE_TYPE_UI_PolicyElement_T *element_entry)
{
    UI32_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    ret = RULE_MGR_GetNextElementFromPolicyMap(policy_map_index, element_entry);

    if (RULE_TYPE_OK != ret)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE);
    }

    L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_SUCCESS);
}

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Create/update a bundle group
 * INPUT:    policy_map_name    - policy-map name
 *           index              - bundle group index
 *           bundle_group_p     - bundle group
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     Create/update a bundle by
 *             valid_status = TRUE and index is which entry want to create/update.
 *           Delete a bundle by
 *             valid_status = TRUE and index is which entry want to delete.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_SetBundleGroup(
    const char *policy_map_name,
    RULE_TYPE_UI_BundleGroup_T *bundle_group_p)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_SetUIBundleGroup(policy_map_name, bundle_group_p);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a bundle group
 * INPUT:    policy_map_index         - policy-map name
 *           ui_bundle_group_p->index - bundle index
 *
 * OUTPUT:   ui_bundle_group_p  - bundle group object
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QoS_GetBundleGroup(
    UI32_T policy_map_index,
    RULE_TYPE_UI_BundleGroup_T *bundle_group_p)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetUIBundleGroup(policy_map_index, bundle_group_p);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next bundle group
 * INPUT:    policy_map_index         - policy-map name
 *           ui_bundle_group_p->index - bundle index
 *
 * OUTPUT:   policy_map_index   - policy-map name
 *           ui_bundle_group_p  - bundle group object
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QoS_GetNextBundleGroup(
    UI32_T *policy_map_index_p,
    RULE_TYPE_UI_BundleGroup_T *bundle_group_p)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetNextUIBundleGroup(policy_map_index_p, bundle_group_p);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}
#endif /* #if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE) */

/* DiffServ class-map */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetClassMapNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get name of class map by it's index.
 * INPUT:    class_map_index
 * OUTPUT:   class_map_name.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetClassMapNameById(UI32_T class_map_index, char *class_map_name)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetClassMapNameById(class_map_index, class_map_name);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetClassMapIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of class map by it's name.
 * INPUT:    class_map_index
 * OUTPUT:   class_map_name.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetClassMapIdByName(const char *class_map_name, UI32_T *class_map_index)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetClassMapIdByName(class_map_name, class_map_index);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_CreateClassMapByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Create a class map
 * INPUT:    class_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_CreateClassMapByName(const char *class_map_name)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_CreateClassMap(class_map_name);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_RenameClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Rename a class map
 * INPUT:    old_class_map_name, new_class_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_RenameClassMap(const char *old_class_map_name, const char *new_class_map_name)
{
    UI32_T  ret, class_map_index;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    if(RULE_MGR_GetClassMapIdByName(old_class_map_name, &class_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    ret = RULE_MGR_SetClassMapNameById(class_map_index, new_class_map_name);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a class map
 * INPUT:    class_map
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     Caller need to set:
 *               --- class_map_name
 *               --- desc
 *               --- class_map_match_type
 *               --- row_status             = L4_MGR_ROWSTATUS_CREATEANDGO
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_SetClassMap(RULE_TYPE_UI_ClassMap_T *class_map)
{
    UI32_T  ret, class_map_index;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    if ((class_map == NULL) ||
        (RULE_MGR_GetClassMapIdByName(class_map->class_map_name, &class_map_index) != RULE_TYPE_OK))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    ret = RULE_MGR_SetUIClassMap(class_map_index, class_map);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_DelClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a class map
 * INPUT:    class_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_DelClassMap(const char *class_map_name)
{
    UI32_T  class_map_index;
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetClassMapIdByName(class_map_name, &class_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_DelClassMap(class_map_index);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetClassMapByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a class map
 * INPUT:    class_map
 * OUTPUT:   class_map.
 * RETURN:   Error code.
 * NOTE:     Key: class_map->class_map_name
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetClassMapByName(RULE_TYPE_UI_ClassMap_T *class_map)
{
    UI32_T  ret, class_map_index;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    if ((class_map == NULL) ||
        (RULE_MGR_GetClassMapIdByName(class_map->class_map_name, &class_map_index) != RULE_TYPE_OK))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    ret = RULE_MGR_GetClassMapById(class_map_index, class_map);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map_index, class_map.
 * RETURN:   Error code.
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetNextClassMap(UI32_T *class_map_index, RULE_TYPE_UI_ClassMap_T *class_map)
{
    UI32_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    ret = RULE_MGR_GetNextUIClassMap(class_map_index, class_map);

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextRunningClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map_index, class_map.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_QoS_GetNextRunningClassMap(UI32_T *class_map_index, RULE_TYPE_UI_ClassMap_T *class_map)
{
    UI32_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    ret = RULE_MGR_GetNextUIClassMap(class_map_index, class_map);
    if (RULE_TYPE_OK != ret)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE);
    }

    L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_SUCCESS);
}

/* DiffServ class-map element*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_AddElement2ClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Add a element to class map
 * INPUT:    class_map_name, element_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL / NOT_ALLOW_MIX_SELECTOR)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_AddElement2ClassMap(const char *class_map_name, RULE_TYPE_UI_ClassMapElement_T *element_entry)
{
    UI32_T  class_map_index;
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetClassMapIdByName(class_map_name, &class_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_AddElement2ClassMap(class_map_index, element_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_DelElementFromClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a element from class map
 * INPUT:    class_map_name, element_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_DelElementFromClassMap(const char *class_map_name, RULE_TYPE_UI_ClassMapElement_T *element_entry)
{
    UI32_T  class_map_index;
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetClassMapIdByName(class_map_name, &class_map_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_DelElementFromClassMap(class_map_index, element_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextElementFromClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next element from class map
 * INPUT:    class_map_index, element_index
 * OUTPUT:   element_index, element_entry.
 * RETURN:   Error code.
 * NOTE:     Key:class_map_index, element_index
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetNextElementFromClassMap(UI32_T class_map_index, UI32_T *element_index, RULE_TYPE_UI_ClassMapElement_T *element_entry)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetNextElementFromClassMap(class_map_index, element_index, element_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextRunningElementFromClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running element from class map
 * INPUT:    class_map_index, element_index
 * OUTPUT:   element_index, element_entry.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     Key:class_map_index, element_index
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_QoS_GetNextRunningElementFromClassMap(UI32_T class_map_index, UI32_T *element_index, RULE_TYPE_UI_ClassMapElement_T *element_entry)
{
    UI32_T                     ret;
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetNextElementFromClassMap(class_map_index, element_index, element_entry);
    L4_MGR_UNLOCK();

    if(rm_ret == RULE_TYPE_OK)
    {
            ret= SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        ret = SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/* ======================== ACL port table ===================================*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_BindPort2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to ACL
 * INPUT:    uport_ifindex, acl_name, acl_type, ingress_flag
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     ingress_flag = FALSE implies egress
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_BindPort2Acl(
    UI32_T uport_ifindex,
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    BOOL_T ingress_flag,
    const UI8_T * time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable)
{
    UI32_T  acl_index;
    RULE_TYPE_RETURN_TYPE_T    rm_ret;
    RULE_TYPE_UI_AclEntry_T    acl_entry;

    memset(&acl_entry, 0, sizeof(acl_entry));

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == FALSE)
    if (FALSE == ingress_flag)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_INVALID_PARAMETER);
    }
#endif /* SYS_CPNT_QOS_V2_EGRESS_PORT */

#if (SYS_CPNT_ACL_COUNTER == FALSE)
    if (RULE_TYPE_COUNTER_ENABLE == counter_enable)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_INVALID_PARAMETER);
    }
#endif

    if ('\0' == acl_name[0])
    {
        acl_entry.acl_type = acl_type;
        if(RULE_MGR_GetAclByTypeAndDirection(uport_ifindex, &acl_entry, ingress_flag)!= RULE_TYPE_OK)
        {
            L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
        }
        if(RULE_MGR_GetAclIdByName(acl_entry.acl_name, &acl_index) != RULE_TYPE_OK)
        {
            L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
        }
        L4_MGR_LOCK();
        rm_ret = RULE_MGR_UnBindPortFromAcl(uport_ifindex, acl_index, acl_type, ingress_flag);
        L4_MGR_UNLOCK();
    }
    else
    {
        if(RULE_MGR_GetAclIdByName(acl_name, &acl_index) != RULE_TYPE_OK)
        {
            L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
        }
        L4_MGR_LOCK();
        rm_ret = RULE_MGR_BindPort2Acl(uport_ifindex, acl_index, acl_type, ingress_flag,
                                       time_range_name, counter_enable);
        L4_MGR_UNLOCK();
    }
    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_UnBindPortFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Unbind a port from ACL
 * INPUT:    uport_ifindex, acl_name, acl_type, ingress_flag
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_UnBindPortFromAcl(UI32_T uport_ifindex, const char *acl_name, RULE_TYPE_AclType_T acl_type, BOOL_T ingress_flag)
{
    UI32_T  acl_index;
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetAclIdByName(acl_name, &acl_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }
    L4_MGR_LOCK();
    rm_ret = RULE_MGR_UnBindPortFromAcl(uport_ifindex, acl_index, acl_type, ingress_flag);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_BindPort2DynamicAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to dynamic ACL
 * INPUT:    uport_ifindex, acl_name, acl_type, ingress_flag
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     1. One port only can bind one dynamic acl.
 *           2. The dynamic acl is got from authentication server.
 *           3. When apply dynamic acl, the user configured aclon this port
 *              should be disabled.
 *           4. If the port already bind a dynamic acl, then the bind
 *              or unbind action by user shall be effected after the dynamic
 *              acl be unbinded.
 *           5. The user acl should be restored to enable when dynamic acl be
 *              unbinded.
 *           6. The changes by this function should not be saved to configuration
 *              file.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_BindPort2DynamicAcl(UI32_T uport_ifindex, const char *acl_name, RULE_TYPE_AclType_T acl_type, BOOL_T ingress_flag)
{
    UI32_T  acl_index;
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    /* only support ingress now
     */
    if (ingress_flag == FALSE)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    if(RULE_MGR_GetAclIdByName(acl_name, &acl_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }
    L4_MGR_LOCK();
    rm_ret = RULE_MGR_BindPort2DynamicAcl(uport_ifindex, acl_index, acl_type, ingress_flag);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_UnBindPortFromDynamicAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Unbind a port from dynamic ACL
 * INPUT:    uport_ifindex, acl_name, acl_type, ingress_flag
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     1. One port only can bind one dynamic acl.
 *           2. The dynamic acl is got from authentication server.
 *           3. When apply dynamic acl, the user configured aclon this port
 *              should be disabled.
 *           4. If the port already bind a dynamic acl, then the bind
 *              or unbind action by user shall be effected after the dynamic
 *              acl be unbinded.
 *           5. The user acl should be restored to enable when dynamic acl be
 *              unbinded.
 *           6. The changes by this function should not be saved to configuration
 *              file.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_UnBindPortFromDynamicAcl(
    UI32_T uport_ifindex,
    RULE_TYPE_AclType_T acl_type,
    BOOL_T ingress_flag)
{
    UI32_T                      acl_index;
    RULE_TYPE_RETURN_TYPE_T     rm_ret;
    RULE_TYPE_UI_AclEntry_T     acl_entry;
    UI32_T                      precedence;
    RULE_TYPE_COUNTER_ENABLE_T  counter_enable;
    BOOL_T                      found = FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    /* only support ingress now
     */
    if (ingress_flag == FALSE)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    acl_index  = 0;
    precedence = 0;
    while (RULE_TYPE_OK == RULE_MGR_GetNextAclByPort(uport_ifindex,
                                                     ingress_flag?RULE_TYPE_INBOUND:RULE_TYPE_OUTBOUND,
                                                     &acl_index,
                                                     &acl_entry,
                                                     &precedence,
                                                     NULL,
                                                     &counter_enable))
    {
        if ((acl_entry.acl_type == RULE_TYPE_IP_STD_ACL || acl_entry.acl_type == RULE_TYPE_IP_EXT_ACL)
            && (acl_type == RULE_TYPE_IP_ACL))
        {
            found = TRUE;
            break;
        }
        else if ((acl_entry.acl_type == RULE_TYPE_IPV6_STD_ACL || acl_entry.acl_type == RULE_TYPE_IPV6_EXT_ACL)
            && (acl_type == RULE_TYPE_IPV6_ACL))
        {
            found = TRUE;
            break;
        }
        else if ((acl_entry.acl_type == RULE_TYPE_MAC_ACL)
            && (acl_type == RULE_TYPE_MAC_ACL))
        {
            found = TRUE;
            break;
        }
    }

    if (!found)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_OK);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_UnBindPortFromDynamicAcl(uport_ifindex, acl_index, acl_type, ingress_flag);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetAclByTypeAndDirection
 *------------------------------------------------------------------------------
 * PURPOSE:  get binding ACL by type and direction
 * INPUT:    uport_ifindex, acl_type, ingress_flag
 * OUTPUT:   acl_entry.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_GetAclByTypeAndDirection(UI32_T uport_ifindex, RULE_TYPE_UI_AclEntry_T *acl_entry,  BOOL_T ingress_flag)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(acl_entry == NULL)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetAclByTypeAndDirection(uport_ifindex, acl_entry, ingress_flag);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextAclByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next binding ACL by port
 * INPUT:    uport_ifindex, acl_index, precedence
 * OUTPUT:   acl_index, acl_entry, precedence, time_range_index.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_GetNextAclByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry,
    UI32_T *precedence,
    UI8_T *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(acl_entry == NULL)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetNextAclByPort(uport_ifindex, inout_profile, acl_index,
                                       acl_entry, precedence,
                                       time_range_name,
                                       counter_enable);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextUIAclByPortAndBindingType
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next same binding type ACL by binding type and port
 * INPUT:    precedence, uport_ifindex
 * OUTPUT:   uport_ifindex, acl_entry.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_GetNextUIAclByPortAndBindingType(UI32_T precedence, UI32_T *uport_ifindex, RULE_TYPE_UI_AclEntry_T *acl_entry, UI8_T *time_range_name)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(acl_entry == NULL)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetNextUIAclByPortAndBindingType(precedence, uport_ifindex, acl_entry, time_range_name);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextRunningAclByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running binding ACL by port
 * INPUT:    uport_ifindex, acl_entry, precedence
 * OUTPUT:   uport_ifindex, acl_entry, precedence, p_time_range_index.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_ACL_GetNextRunningAclByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry,
    UI32_T *precedence,
    UI8_T *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable)
{
    UI32_T  ret;
    RULE_TYPE_Acl_T old_acl_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    if (RULE_OM_IsBindDyanmicAclOnPort(uport_ifindex, inout_profile))
    {
        RULE_TYPE_AclType_T     start = 0;
        RULE_TYPE_AclType_T     i;
        RULE_TYPE_AclMemento_T  acl_mem;

        if (*acl_index == 0)
        {
            start = 0;
        }
        else
        {
            for (i=0; i<RULE_TYPE_MAX_NBR_OF_ACL_TYPE; ++i)
            {
                RULE_OM_GetAclMementoOnPort(uport_ifindex, inout_profile, i, &acl_mem);
                if (acl_mem.acl_index == *acl_index)
                {
                    start = i+1;
                    break;
                }
            }
        }

        for (i=start; i<RULE_TYPE_MAX_NBR_OF_ACL_TYPE; ++i)
        {
            RULE_OM_GetAclMementoOnPort(uport_ifindex, inout_profile, i, &acl_mem);
            if (acl_mem.acl_index != 0)
            {
                if (RULE_TYPE_OK != RULE_OM_GetAclByIndex(acl_mem.acl_index, &old_acl_entry))
                {
                    continue;
                }

                strcpy(acl_entry->acl_name, (char*)old_acl_entry.acl_name);
                acl_entry->acl_type     = old_acl_entry.acl_type;
                acl_entry->row_status   = old_acl_entry.row_status;

                *acl_index          = acl_mem.acl_index;
                *precedence         = acl_mem.precedence;
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
                if (time_range_name != NULL)
                {
                    strncpy((char *)time_range_name, (char *)acl_mem.time_range_name,
					SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
                    time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH] = '\0';
                }
#endif
                *counter_enable     = acl_mem.counter_enable;

                L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_SUCCESS);
            }
        }

        L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE);
    }
    else
    {
        ret = RULE_MGR_GetNextAclByPort(uport_ifindex, inout_profile, acl_index,
                                        acl_entry, precedence,
                                        time_range_name,
                                        counter_enable);
        if (RULE_TYPE_OK != ret)
        {
            L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE);
        }
        L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_SUCCESS);
    }

    /* no go here
     */
    L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE);
}

/* ACL table*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetAclNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get name of ACL by it's index.
 * INPUT:    acl_index
 * OUTPUT:   acl_name.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_GetAclNameById(UI32_T acl_index, char *acl_name)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetAclNameById(acl_index, acl_name);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetAclIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of ACL by it's name.
 * INPUT:    acl_name
 * OUTPUT:   acl_index.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_GetAclIdByName(const char *acl_name, UI32_T *acl_index)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetAclIdByName(acl_name, acl_index);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetAclIdByNameAndType
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of ACL by it's name and type.
 * INPUT:    acl_name, acl_type
 * OUTPUT:   acl_index.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_GetAclIdByNameAndType(const char *acl_name, RULE_TYPE_AclType_T acl_type, UI32_T *acl_index)
{
    RULE_TYPE_RETURN_TYPE_T     rm_ret;
    RULE_TYPE_Acl_T             acl_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();

    rm_ret = RULE_MGR_GetAclIdByName(acl_name, acl_index);

    if (RULE_TYPE_OK == rm_ret)
    {
        rm_ret = RULE_MGR_GetAclByIndex (*acl_index, &acl_entry);

        if (RULE_TYPE_OK == rm_ret)
        {
            if (acl_type != acl_entry.acl_type)
                rm_ret = RULE_TYPE_FAIL;
        }
    }

    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_CreateAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Create an ACL.
 * INPUT:    acl_name, acl_type
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_CreateAcl(const char *acl_name, RULE_TYPE_AclType_T acl_type)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_CreateAcl(acl_name, acl_type);
    L4_MGR_UNLOCK();

#if (SYS_CPNT_PBR == TRUE)
    if (rm_ret == RULE_TYPE_OK)
    {
        UI32_T acl_index;
        RULE_TYPE_RETURN_TYPE_T result;

        result = RULE_MGR_GetAclIdByName(acl_name, &acl_index);
        ASSERT(result == RULE_TYPE_OK);

        SYS_CALLBACK_MGR_L4_AddAclCallback(SYS_MODULE_ACL, acl_index, (char*)acl_name);
    }
#endif /* SYS_CPNT_PBR */

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_DelAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete an ACL.
 * INPUT:    acl_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_DelAcl(const char *acl_name)
{
    UI32_T  acl_index;
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetAclIdByName(acl_name, &acl_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_DelAcl(acl_index);
    L4_MGR_UNLOCK();

#if (SYS_CPNT_PBR == TRUE)
    if (rm_ret == RULE_TYPE_OK)
    {
        SYS_CALLBACK_MGR_L4_DeleteAclCallback(SYS_MODULE_ACL, acl_index, (char*)acl_name);
    }
#endif /* SYS_CPNT_PBR */

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_DelAclByNameAndAclType
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete an ACL by name and acl_type
 * INPUT:    acl_name, acl_type
 * OUTPUT:   NONE
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_DelAclByNameAndAclType(
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type
    )
{
    UI32_T              ret, acl_index;
    RULE_TYPE_Acl_T     acl_entry;
    UI32_T              port_nbr;
    UI8_T               port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST] = {0};

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if ((RULE_TYPE_OK != RULE_MGR_GetAclIdByName(acl_name, &acl_index)) ||
        (RULE_TYPE_OK != RULE_MGR_GetAclByIndex(acl_index, &acl_entry)) ||
        (acl_type != acl_entry.acl_type))
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();

    /* FIXIT: L4_MGR_ACL_DelAclByNameAndAclType */
    if (RULE_TYPE_OK != RULE_OM_GetDynamicBindPortListByAclName((char*)acl_name, RULE_TYPE_INBOUND, port_list, &port_nbr))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    ret = RULE_MGR_DelAcl(acl_index);

    L4_MGR_UNLOCK();

    SYS_CALLBACK_MGR_AnnounceAclDeleted(SYS_MODULE_ACL, (char*)acl_name, acl_type, port_list);

#if (SYS_CPNT_PBR == TRUE)
    if (ret == RULE_TYPE_OK)
    {
        SYS_CALLBACK_MGR_L4_DeleteAclCallback(SYS_MODULE_ACL, acl_index, (char*)acl_name);
    }
#endif /* SYS_CPNT_PBR */

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACL.
 * INPUT:    acl_entry
 * OUTPUT:   acl_entry.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_GetNextAcl(UI32_T *acl_index, RULE_TYPE_UI_AclEntry_T *acl_entry)
{
    UI32_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    ret = RULE_MGR_GetNextAcl(acl_index, acl_entry);

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextRunningAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running ACL.
 * INPUT:    acl_entry
 * OUTPUT:   acl_entry.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_ACL_GetNextRunningAcl(UI32_T *acl_index, RULE_TYPE_UI_AclEntry_T *acl_entry)
{
    UI32_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    ret = RULE_MGR_GetNextAcl(acl_index, acl_entry);
    if (RULE_TYPE_OK != ret)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE);
    }

    L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_SUCCESS);
}

/* ACL ACE*/

/*------------------------------------------------------------------------------
 * FUNCTION NAME -L4_MGR_ACL_InitAce
 *------------------------------------------------------------------------------
 * PURPOSE  : for SNMP and WWW and CLI to make a default ACE
 * INPUT    : ace_entry->ace_type
 * OUTPUT   : ace_entry
 * RETURN   : Error code
 * NOTES    : this API is used to init the ace before use it, need to set ace_entry->ace_type
 *------------------------------------------------------------------------------*/
/* wuli??*/
UI32_T L4_MGR_ACL_InitAce(RULE_TYPE_Ace_Entry_T *ace_entry)
{
    UI32_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    ret = RULE_MGR_InitAce(ace_entry);

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_ACL_InitUIAce
 *------------------------------------------------------------------------------
 * PURPOSE  : for UI to make a default UI ACE entry
 * INPUT    : ace_entry->ace_type
 * OUTPUT   : ace_entry
 * RETURN   : Error code
 * NOTES    : this API is used to init the ace before use it
 *------------------------------------------------------------------------------*/
RULE_TYPE_RETURN_TYPE_T
L4_MGR_ACL_InitUIAce(
    RULE_TYPE_UI_Ace_Entry_T *ace_entry_p)
{
    RULE_TYPE_RETURN_TYPE_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    ret = RULE_MGR_InitUIAce(ace_entry_p);

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_ACL_SetAcePatternToDefaultByType
 *------------------------------------------------------------------------------
 * PURPOSE  : For UI to initial ace.u.{mac|ip|ipv6|arp} to default
 * INPUT    : ace_type
 * OUTPUT   : ace_entry_p
 * RETURN   : Error code
 * NOTES    : Initial the ace pattern by ace_type
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_ACL_SetAcePatternToDefaultByType(
    RULE_TYPE_Ace_Entry_T *ace_entry_p,
    RULE_TYPE_AclType_T ace_type)
{
    UI32_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    ret = RULE_MGR_SetAcePatternToDefaultByType(ace_entry_p, ace_type);

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_SetUIAce2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a UI ACE to ACL.
 * INPUT:    acl_name, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_SetUIAce2Acl(
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_UI_Ace_Entry_T *ace_entry)
{
    UI32_T  acl_index;
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetAclIdByName(acl_name, &acl_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    if(ace_entry == NULL)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_SetUIAce2Acl(acl_index, acl_type, ace_entry);
    L4_MGR_UNLOCK();

#if (SYS_CPNT_PBR == TRUE)
    if (rm_ret == RULE_TYPE_OK)
    {
        SYS_CALLBACK_MGR_L4_ModifyAclCallback(SYS_MODULE_ACL, acl_index, acl_name);
    }
#endif /* SYS_CPNT_PBR */

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_DelUIAceFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a UI ACE from ACL.
 * INPUT:    acl_name, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_DelUIAceFromAcl(
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_UI_Ace_Entry_T *ace_entry)
{
    UI32_T  acl_index;
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetAclIdByName(acl_name, &acl_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    if(ace_entry == NULL)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_DelUIAceFromAcl(acl_index, acl_type, ace_entry);
    L4_MGR_UNLOCK();

#if (SYS_CPNT_PBR == TRUE)
    if (rm_ret == RULE_TYPE_OK)
    {
        SYS_CALLBACK_MGR_L4_ModifyAclCallback(SYS_MODULE_ACL, acl_index, acl_name);
    }
#endif /* SYS_CPNT_PBR */

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextUIAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next UI ACE from ACL.
 * INPUT:    acl_index, ace_index_p
 * OUTPUT:   ace_index_p, ace_p
 * RETURN:   Error code.
 * NOTE:     Key: acl_index, ace_index
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_GetNextUIAceByAcl(
    UI32_T acl_index,
    UI32_T *ace_index_p,
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetNextUIAceByAcl(acl_index, ace_index_p, ace_p);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextHardwareUIAceByAcl(
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next hardware UI ACE from ACL.
 * INPUT:    acl_index, acl_type, ace_index
 * OUTPUT:   ace_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_GetNextHardwareUIAceByAcl(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T acl_index,
    UI32_T *ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;
    RULE_TYPE_AclType_T        acl_type;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetNextHardwareUIAceByAcl(uport_ifindex, inout_profile,
                                              acl_index, &acl_type, ace_index,
                                              ace_p);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextRunningUIAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running UI ACE from ACL.
 * INPUT:    acl_index, ace_index_p, ace_entry_p
 * OUTPUT:   ace_entry_p
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_ACL_GetNextRunningUIAceByAcl(
    UI32_T acl_index,
    UI32_T *ace_index_p,
    RULE_TYPE_UI_Ace_Entry_T *ace_entry_p)
{
    UI32_T  ret;
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetNextUIAceByAcl(acl_index, ace_index_p, ace_entry_p);
    L4_MGR_UNLOCK();

    if (rm_ret == RULE_TYPE_OK)
    {
        ret = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        ret = SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_SetAce2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a ACE to ACL.
 * INPUT:    acl_name, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_SetAce2Acl(const char *acl_name, RULE_TYPE_AclType_T acl_type, RULE_TYPE_Ace_Entry_T *ace_entry)
{
    UI32_T  acl_index;
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetAclIdByName(acl_name, &acl_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    if(ace_entry == NULL)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_SetAce2Acl(acl_index, acl_type, ace_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_DelAceFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a ACE from ACL.
 * INPUT:    acl_name, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_DelAceFromAcl(const char *acl_name, RULE_TYPE_AclType_T acl_type, RULE_TYPE_Ace_Entry_T *ace_entry)
{
    UI32_T  acl_index;
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if(RULE_MGR_GetAclIdByName(acl_name, &acl_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    if(ace_entry == NULL)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_DelAceFromAcl(acl_index, acl_type, ace_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE from ACL.
 * INPUT:    acl_index, ace_index, ace_entry
 * OUTPUT:   ace_entry.
 * RETURN:   Error code.
  * NOTE:     Key: acl_index, ace_index
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_GetNextAceByAcl(UI32_T acl_index, UI32_T *ace_index, RULE_TYPE_Ace_Entry_T *ace_entry)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;
    RULE_TYPE_AclType_T       acl_type;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetNextAceByAcl(acl_index, &acl_type, ace_index, ace_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextHardwareAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE from ACL.
 * INPUT:    acl_index, ace_index, ace_entry
 * OUTPUT:   ace_entry.
 * RETURN:   Error code.
 * NOTE:     Key: acl_index, ace_index
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_GetNextHardwareAceByAcl(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T acl_index,
    UI32_T *ace_index,
    RULE_TYPE_Ace_Entry_T *ace_entry)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;
    RULE_TYPE_AclType_T        acl_type;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetNextHardwareAceByAcl(uport_ifindex, inout_profile,
                                              acl_index,
                                              &acl_type, ace_index, ace_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextMibAceByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE by port
 * INPUT:    ifindex       - interface index
 *           inout_profile - direction
 *           acl_index     - user config ACL index
 *           ace_index     - hardware ACE index
 * OUTPUT:   ace_entry     - ACE entry
 * RETURN:   Error code.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_GetNextMibAceByPort(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index_p,
    UI32_T *ace_index_p,
    RULE_TYPE_Ace_Entry_T *ace_entry_p,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable_p)
{
    RULE_TYPE_RETURN_TYPE_T    rm_ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetNextMibAceByPort(ifindex, inout_profile, acl_index_p,
                 ace_index_p, ace_entry_p, counter_enable_p);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(rm_ret);
}

#if (SYS_CPNT_ACL_COUNTER == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_ClearAclCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Clear ACL counter
 * INPUT:    acl_name   - ACL name
 *                        If this parameter is zero string, clear counter for all
 *                        ACLs.
 *                        If this parameter is zero string, ignores ace_index.
 *           ace_index  - ACE index.
 *                        If this parameter is 0xffffffff, clear counter for all
 *                        ACEs of the specified ACL.
 *           interface  - The interface.
 *                        This parameter can be one of the following values,
 *                        RULE_TYPE_INTERFACE_LPORT     - One port
 *                        RULE_TYPE_INTERFACE_UPORT     - One port
 *                        RULE_TYPE_INTERFACE_ALL_UNIT  - All port
 * OUTPUT:   none
 * RETURN:   Error code.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_ClearAclCounter(
    const char *acl_name,
    UI32_T ace_index,
    RULE_TYPE_INTERFACE_INFO_T *interface)
{
    UI32_T acl_index;
    RULE_TYPE_RETURN_TYPE_T    ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();

    if (NULL == acl_name || '\0' == acl_name[0])
    {
        acl_index = 0xffffffff;
    }
    else
    {
        ret = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        if (RULE_TYPE_OK != ret)
        {
            L4_MGR_UNLOCK();
            L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_INVALID_PARAMETER);
        }
    }

    ret = RULE_MGR_ClearAclCounter(acl_index, ace_index, interface);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}
#endif /* #if (SYS_CPNT_ACL_COUNTER == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextRunningAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running ACE from ACL.
 * INPUT:    acl_index, ace_index, ace_entry
 * OUTPUT:   ace_entry.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_ACL_GetNextRunningAceByAcl(UI32_T acl_index, UI32_T *ace_index, RULE_TYPE_Ace_Entry_T *ace_entry)
{
    UI32_T  ret;
    RULE_TYPE_RETURN_TYPE_T    rm_ret;
    RULE_TYPE_AclType_T       acl_type;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_MGR_LOCK();
    rm_ret = RULE_MGR_GetNextAceByAcl(acl_index, &acl_type, ace_index, ace_entry);
    L4_MGR_UNLOCK();

    if(rm_ret == RULE_TYPE_OK)
    {
        ret= SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        ret = SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

#if (SYS_CPNT_ACL_MIRROR == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_AddAclMirrorEntry
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
L4_MGR_ACL_AddAclMirrorEntry(
    UI32_T ifindex_dest,
    const char *acl_name)
{
    UI32_T ret;
    UI32_T acl_index;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    if (RULE_MGR_GetAclIdByName(acl_name, &acl_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    ret = RULE_MGR_AddAclMirrorEntry(ifindex_dest, acl_index);

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_RemoveAclMirrorEntry
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
L4_MGR_ACL_RemoveAclMirrorEntry(
    UI32_T ifindex_dest,
    const char *acl_name)
{
    UI32_T ret;
    UI32_T acl_index;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    if (RULE_MGR_GetAclIdByName(acl_name, &acl_index) != RULE_TYPE_OK)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    ret = RULE_MGR_RemoveAclMirrorEntry(ifindex_dest, acl_index);

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextAclMirrorEntry
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
L4_MGR_ACL_GetNextAclMirrorEntry(
    UI32_T *ifindex_dest,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry)
{
    UI32_T ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    ret = RULE_MGR_GetNextAclMirrorEntry(ifindex_dest, acl_index, acl_entry);

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextRunningAclMirrorEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  GetNext ACL mirror entry
 * INPUT:    ifindex_dest   - Destination port. Pass 0 to start.
 *           acl_index      - ACL index
 * OUTPUT:   acl_entry      - ACL entry
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_ACL_GetNextRunningAclMirrorEntry(
    UI32_T *ifindex_dest,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry)
{
    SYS_TYPE_Get_Running_Cfg_T ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);
    L4_MGR_LOCK();

    ret = RULE_MGR_GetNextAclMirrorEntry(ifindex_dest, acl_index, acl_entry);

    L4_MGR_RETURN_AND_RELEASE_CSC( ((ret == RULE_TYPE_OK) ? \
       SYS_TYPE_GET_RUNNING_CFG_SUCCESS : SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE) );
}
#endif /* #if (SYS_CPNT_ACL_MIRROR == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_GetTCAMUtilization
 *------------------------------------------------------------------------------
 * PURPOSE :  get total pce, free_pce counts, and utilization
 * INPUT   :  none
 * OUTPUT  :  tcam_util_p
 * RETURN  :  True/False
 * NOTE    :
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_GetTCAMUtilization(RULE_TYPE_TCAMUtilization_T *tcam_util_p)
{
    BOOL_T ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if (NULL == tcam_util_p)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    L4_MGR_LOCK();
    ret = RULE_MGR_GetTCAMUtilization(tcam_util_p);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_GetTcamEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : Get tcam entry
 * INPUT    : tcam_entry_p->unit
 *            tcam_entry_p->device_id
 *            tcam_entry_p->pool_id
 * OUTPUT   : tcam_util_p
 * RETURN   : TRUE/FALSE.
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_GetTcamEntry(
    RULE_TYPE_TCAM_ENTRY_T *tcam_entry_p)
{
    RULE_TYPE_RETURN_TYPE_T ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    if (NULL == tcam_entry_p)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    L4_MGR_LOCK();
    ret = RULE_MGR_GetTcamEntry(tcam_entry_p);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC((ret == RULE_TYPE_OK) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_GetNextTcamEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : Get next tcam entry
 * INPUT    : tcam_entry_p->unit
 *            tcam_entry_p->device_id
 *            tcam_entry_p->pool_id
 * OUTPUT   : tcam_util_p
 * RETURN   : TRUE/FALSE.
 * NOTE     : For get first entry,
 *            use index with tcam_entry_p->unit = 0
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_GetNextTcamEntry(
    RULE_TYPE_TCAM_ENTRY_T *tcam_entry_p)
{
    RULE_TYPE_RETURN_TYPE_T ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    if (NULL == tcam_entry_p)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    L4_MGR_LOCK();
    ret = RULE_MGR_GetNextTcamEntry(tcam_entry_p);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC((ret == RULE_TYPE_OK) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_GetSupportedTcamCapabilityBitMap
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets TCAM all supporting capability
 * INPUT    :
 * OUTPUT   : bit_map_p
 * RETURN   : Error code.
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_GetSupportedTcamCapabilityBitMap(
    RULE_TYPE_TCAM_CAP_BITMAP_T *bit_map_p)
{
    RULE_TYPE_RETURN_TYPE_T ret = RULE_TYPE_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);

    L4_MGR_LOCK();
    ret = RULE_MGR_GetSupportedTcamCapabilityBitMap(bit_map_p);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/* ========================== For CoS ===============================================*/
#if (SYS_CPNT_COS == TRUE)
/*------------------------------------------------------------------------------
 * Routine Name : L4_MGR_COS_RegisterCallbackFunction()
 *------------------------------------------------------------------------------
 * Function : Register Callback Function
 * Input    : None
 * Output   : None
 * Return   : None
 * Note     : There are 4 callback functions will be registered for trunk
 *------------------------------------------------------------------------------
 */
static void L4_MGR_COS_RegisterCallbackFunction(void)
{
#if (SYS_CPNT_COS_INTER_DSCP == TRUE)
    /* CallBack Function */
    SWCTRL_Register_TrunkMemberAdd1st_CallBack((void *)L4_MGR_COS_AddFirstTrunkMember);
    SWCTRL_Register_TrunkMemberAdd_CallBack((void *)L4_MGR_COS_AddTrunkMember);
    SWCTRL_Register_TrunkMemberDelete_CallBack((void *)L4_MGR_COS_DelTrunkMember);
    SWCTRL_Register_TrunkMemberDeleteLst_CallBack((void *)L4_MGR_COS_DelLastTrunkMember);
#endif /* #if (SYS_CPNT_COS_INTER_DSCP == TRUE) */
   //eli test PRI_MGR_RegisterCosChanged_CallBack((void *)L4_MGR_COS_CosLportConfigAsic);
    return;
} /*End of L4_MGR_COS_RegisterCallbackFunction */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_AddFirstTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the callback function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_AddFirstTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_AddFirstTrunkMember(trunk_ifindex, member_ifindex);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - LL4_MGR_COS_AddTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the callback function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_AddTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_AddTrunkMember(trunk_ifindex, member_ifindex);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the callback function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_DelTrunkMember(trunk_ifindex, member_ifindex);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelLastTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the callback function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelLastTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_DelLastTrunkMember(trunk_ifindex, member_ifindex);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_CosLportConfigAsic
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the cos function that will be registed to priority manager
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_CosLportConfigAsic(UI32_T l_port)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_CosLportConfigAsic(l_port);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetIpPrecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will enable or disable TOS/DSCP/COS mapping of system
 * INPUT:    flag -- COS_VM_ipprecedence_MAPPING_ENABLED/COS_VM_DSCP_MAPPING_ENABLED
 *                   /COS_VM_DISABLE_BOTH
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:  TOS/COS mapping and DSCP/COS mapping can not be enabled simultaneously
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SetIpPrecedenceDscpMappingStatus (COS_VM_MAPPING_FLAG_T flag)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_SetIpPrecedenceDscpMappingStatus(flag);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetIpPrecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get TOS/DSCP/COS mapping status of system
 * INPUT:    none
 * OUTPUT:   flag -- COS_MGR_ipprecedence_MAPPING_ENABLED/COS_MGR_DSCP_MAPPING_ENABLED
 *                   /COS_MGR_DISABLE_BOTH
 * RETURN:   TRUE/FALSE
 * NOTE:  tos/COS mapping and DSCP/COS mapping can not be enabled simultaneously
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetIpPrecedenceDscpMappingStatus (COS_VM_MAPPING_FLAG_T *flag)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_GetIpPrecedenceDscpMappingStatus(flag);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetRunningIpPrecedenceDscpMappingStatus
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
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetRunningIpPrecedenceDscpMappingStatus(COS_VM_MAPPING_FLAG_T *flag)
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_MGR_LOCK();
    status = COS_VM_GetRunningIpPrecedenceDscpMappingStatus(flag);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will enable or disable TcpPort/COS mapping of system
 * INPUT:    flag -- COS_MGR_MAPPING_ENABLED/COS_MGR_MAPPING_DISABLED
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SetTcpPortMappingStatus (UI32_T flag)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_SetTcpPortMappingStatus(flag);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the status of TcpPort/COS mapping of system
 * INPUT:    None
 * OUTPUT:   flag -- COS_MGR_MAPPING_ENABLED/COS_MGR_MAPPING_DISABLED
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetTcpPortMappingStatus(UI32_T *flag)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_GetTcpPortMappingStatus((COS_VM_TCPPORT_MAPPING_FLAG_T *) flag);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetRunningTcpPortMappingStatus
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
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetRunningTcpPortMappingStatus(COS_VM_TCPPORT_MAPPING_FLAG_T *flag)
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_MGR_LOCK();
    status = COS_VM_GetRunningTcpPortMappingStatus(flag);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a tos/CoS map in tos_map_table of specific
 *           logical port
 * INPUT:    l_port, tos (0..7), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SetIpPrecedenceToCos(UI32_T l_port, UI8_T tos, UI32_T cos)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_SetIpPrecedenceToCos(l_port , tos , cos);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a tos/CoS map in tos_map_table of specific
 *           logical port
 * INPUT:    l_port, tos (0..7)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetIpPrecedenceToCos(UI32_T l_port, UI8_T tos, UI32_T *cos)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_GetIpPrecedenceToCos(l_port , tos , cos);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tos/CoS map to its default value
 *           in tos_map_table of specific logical port
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelIpPrecedenceToCos(UI32_T l_port, UI8_T tos)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_DelIpPrecedenceToCos(l_port , tos);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelIpPrecedenceLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tos/CoS map to its default value
 *           in tos_map_table of specific logical port
 * INPUT:    l_port, tos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelIpPrecedenceLportAllEntry(UI32_T l_port)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_DelIpPrecedenceLportAllEntry(l_port);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a Dscp/CoS map in dscp_map_table of specific
 *           logical port
 * INPUT:    l_port, dscp (0..63), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SetDSCPToCos(UI32_T l_port, UI8_T dscp, UI32_T cos)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_SetDSCPToCos(l_port, dscp, cos);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_SetDSCPToCosStatus
 *------------------------------------------------------------------------------
 * PURPOSE: this function will set a dscp entry's status
 * INPUT:   l_port
 *          dscp, cos status
 * OUTPUT:
 * RETURN:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SetDSCPToCosStatus(UI32_T l_port, UI8_T dscp, BOOL_T status)
{
    BOOL_T ret=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = COS_VM_SetDSCPToCosStatus(l_port, dscp, status);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_GetDSCPToCosStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function will get a dscp entry's status
 * INPUT:   l_port
 *          dscp
 * OUTPUT:  cos status
 * RETURN:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetDSCPToCosStatus(UI32_T l_port, UI8_T dscp, BOOL_T *status)
{
    BOOL_T ret=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = COS_VM_GetDSCPToCosStatus(l_port, dscp, status);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_GetRunningDSCPToCosStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function will get a running dscp entry's status
 * INPUT:    l_port, dscp
 * OUTPUT:   status -- COS_MGR_MAPPING_ENABLED/COS_MGR_MAPPING_DISABLED
 * RETURN:   SYS_TYPE_GET_RUNNING_CFG_FAIL -- error (system is not in MASTER mode)
 *           SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE -- same as default
 *           SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different from default value
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetRunningDSCPToCosStatus(UI32_T l_port, UI8_T dscp, BOOL_T *status)
{
    BOOL_T ret=FALSE;
    BOOL_T is_default;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = COS_VM_GetDSCPToCosStatus(l_port, dscp, status);

    if (ret == FALSE)
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_FAIL);
    }

    ret=COS_VM_IsDSCPToCosDefaultStatus(l_port, dscp, *status, &is_default);
    L4_MGR_UNLOCK();

    if(ret == FALSE)
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_FAIL);
    }
    else
    {
        if(is_default == TRUE)
        {
            L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE);
        }
        else
        {
            L4_MGR_RETURN_AND_RELEASE_CSC(SYS_TYPE_GET_RUNNING_CFG_SUCCESS);
        }
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a Dscp/CoS map in dscp_map_table of specific
 *           logical port
 * INPUT:    l_port, dscp (0..63)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetDSCPToCos(UI32_T l_port, UI8_T dscp, UI32_T *cos)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_GetDSCPToCos(l_port, dscp, cos);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a Dscp/CoS map to its default value
 *           in dscp_map_table of specific logical port
 * INPUT:    l_port, dscp (0..63)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelDSCPToCos(UI32_T l_port, UI8_T dscp)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_DelDSCPToCos(l_port, dscp);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelDscpLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a Dscp/CoS map to its default value
 *           in dscp_map_table of specific logical port
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelDscpLportAllEntry(UI32_T l_port)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_DelDscpLportAllEntry(l_port);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetPortToCos
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
BOOL_T L4_MGR_COS_SetPortToCos(UI32_T l_port, UI16_T tcp_port, UI32_T cos)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_SetPortToCos(l_port, tcp_port, cos);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a tcp_port/CoS map in tcp_port_map_table of specific
 *           logical port
 * INPUT:    l_port, tcp_port (0..65535)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetPortToCos(UI32_T l_port, UI16_T tcp_port, UI32_T *cos)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_GetPortToCos(l_port, tcp_port, cos);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tcp_port/CoS map to its default value
 *           in tcp_port of specific logical port
 * INPUT:    l_port, tcp_port (0..65535)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelPortToCos(UI32_T l_port, UI16_T tcp_port)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_DelPortToCos(l_port, tcp_port);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelPortLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tcp_port/CoS map to its default value
 *           in tcp_port_map_table of specific logical port
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelPortLportAllEntry(UI32_T l_port)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_DelPortLportAllEntry(l_port);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetNextIpPrecedenceToCos
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
BOOL_T L4_MGR_COS_GetNextIpPrecedenceToCos(UI32_T *l_port, UI8_T *tos, UI32_T *cos)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_GetNextIpPrecedenceToCos(l_port, tos, cos);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetNextDSCPToCos
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
BOOL_T L4_MGR_COS_GetNextDSCPToCos(UI32_T *l_port, UI8_T *dscp, UI32_T *cos)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_GetNextDSCPToCos(l_port, dscp, cos);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetNextPortToCos
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
BOOL_T L4_MGR_COS_GetNextPortToCos(UI32_T *l_port, I32_T *tcp_port, UI32_T *cos)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_GetNextPortToCos(l_port, tcp_port, cos);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_GetRunningIpPrecedenceToCos
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
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetRunningIpPrecedenceToCos(UI32_T l_port, UI8_T tos, UI32_T *cos)
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_MGR_LOCK();
    status = COS_VM_GetRunningIpPrecedenceToCos(l_port , tos , cos);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_GetRunningDSCPToCos
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
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetRunningDSCPToCos(UI32_T l_port, UI8_T dscp, UI32_T *cos)
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_MGR_LOCK();
    status = COS_VM_GetRunningDSCPToCos(l_port , dscp, cos);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_GetRunningPortToCos
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
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetRunningPortToCos(UI32_T l_port, UI16_T tcp_port, UI32_T *cos)
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_MGR_LOCK();
    status = COS_VM_GetRunningPortToCos(l_port , tcp_port, cos);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_GetNextRunningIpPrecedenceToCos
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
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetNextRunningIpPrecedenceToCos(UI32_T *l_port , UI8_T *tos, UI32_T *cos)
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_MGR_LOCK();
    status = COS_VM_GetNextRunningIpPrecedenceToCos(l_port, tos, cos);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_GetNextRunningDSCPToCos
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
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetNextRunningDSCPToCos(UI32_T *l_port, UI8_T *dscp, UI32_T *cos)
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_MGR_LOCK();
    status = COS_VM_GetNextRunningDSCPToCos(l_port , dscp, cos);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_GetNextRunningPortToCos
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
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetNextRunningPortToCos(UI32_T *l_port, I32_T *tcp_port, UI32_T *cos)
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_MGR_LOCK();
    status = COS_VM_GetNextRunningPortToCos(l_port , tcp_port, cos);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_PortCopy
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will copy the source port set to destport
 * INPUT:    map_table_type, src_port, dst_port_list
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE: Since it have to support multi-thread, so it is not possible to separate
 *       parameters configuration from copy operation
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_PortCopy(UI32_T map_table_type, UI32_T src_port, UI8_T *dst_port_list)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_PortCopy(map_table_type, src_port, dst_port_list);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_AddCosEntry
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
BOOL_T L4_MGR_COS_AddCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_AddCosEntry(ifindex, cos_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelCosEntry
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
BOOL_T L4_MGR_COS_DelCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_DelCosEntry(ifindex, cos_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetCosEntryRowStatus
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
BOOL_T L4_MGR_COS_SetCosEntryRowStatus(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_SetCosEntryRowStatus(ifindex, cos_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetCosEntry
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
BOOL_T L4_MGR_COS_SetCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_SetCosEntry(ifindex, cos_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get the next ACL to cos mapping
 * INPUT:    ifindex,
 * OUTPUT:   acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     the key is (ifindex, acl_mapping->acl_name)
 *           supposely only the SNMP will call this fuction
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_GetCosEntry(ifindex, cos_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetNextCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  for CLI and WWW to get the next ACL to cos
 * INPUT:    ifindex,
 * OUTPUT:   acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     use the *ifidnex, acl_mapping->acl_name[0]='\0' to get the first entry
 *           the key is (*ifindex, acl_mapping->acl_name)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetNextCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_GetNextCosEntry(ifindex, cos_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SNMPGetNextCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to get the next acl map to cos
 * INPUT:    ifindex
 * OUTPUT:   ifindex, acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     use the *ifidnex, acl_mapping->acl_name[0]='\0' to get the first entry
 *           the key is (*ifindex, acl_mapping->acl_name)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SNMPGetNextCosEntry(UI32_T *ifindex, COS_TYPE_AclCosEntry_T *cos_entry)
{
    BOOL_T status=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    status = COS_VM_SNMPGetNextCosEntry(ifindex, cos_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  -L4_MGR_COS_GetNextRunningCosEntry
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
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetNextRunningCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry )
{
    SYS_TYPE_Get_Running_Cfg_T status=SYS_TYPE_GET_RUNNING_CFG_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_MGR_LOCK();
    status = COS_VM_GetNextRunningCosEntry(ifindex, cos_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(status);
}

/*
 obsolete part
*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetIpAclToCos
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
BOOL_T L4_MGR_COS_SetIpAclToCos(UI32_T l_port, UI8_T *acl_name, UI32_T cos)
{
   return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetMacAclToCos
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
BOOL_T L4_MGR_COS_SetMacAclToCos(UI32_T l_port, UI8_T *acl_name, UI32_T cos)
{
    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetNextAclToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a ACL for COS mapping
 *           of specific logical port
 * INPUT:
 * OUTPUT:   l_port, acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     use *lport= 0 to get the first entry
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetNextAclToCos(UI32_T *l_port, COS_AclCosMapping_T *acl_mapping)
{
    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetAclToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a ACL for COS mapping
 *           of specific logical port
 * INPUT:    l_port,
 * OUTPUT:   acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetAclToCos(UI32_T l_port, COS_AclCosMapping_T *acl_mapping)
{
    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  L4_MGR_COS_GetRunningIntfAclCosMapping
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
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetRunningIntfAclCosMapping(UI32_T if_index, COS_AclCosMapping_T *acl_mapping )
{
   return SYS_TYPE_GET_RUNNING_CFG_FAIL;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_COS_HandleHotInsertion
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
void L4_MGR_COS_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default)
{
    COS_VM_HandleHotInsertion(starting_port_ifindex, number_of_port, use_default);
    return ;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_COS_HandleHotRemoval
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
void L4_MGR_COS_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port)
{
    COS_VM_HandleHotRemoval(starting_port_ifindex, number_of_port);
    return ;
}
#endif /*#if (SYS_CPNT_COS == TRUE)*/

/* For Marker */
#if(SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_AddMarkerEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  to bind a ACL to the interface that perform remark fuction
 * INPUT:    ifindex      --
 *           marker_entry --
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE
 * NOTE:     Note MARK_TYPE_REMARK_DSCP and MARK_TYPE_REMARK_TOS_PRECEDENCE  can't coexist
 *           key is (ifindex, marker_entry.acl_name)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_AddMarkerEntry(UI32_T ifindex, MARKER_TYPE_MarkerEntry_T *marker_entry)
{
    BOOL_T ret=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = MARKER_VM_AddMarkerEntry(ifindex, marker_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_DelMarkerEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  bind the ACL to interface that performed remark function
 * INPUT:    intf     --
 *           acl_name
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE
 * NOTE:     search key is (ifindex , acl_name)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_DelMarkerEntry(UI32_T ifindex, UI8_T acl_name[], UI32_T acl_type)
{
    BOOL_T ret=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = MARKER_VM_DelMarkerEntry(ifindex, acl_name, acl_type);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_GetMarkerEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  to get the marker entry
 * INPUT:    ifindex     --
 *           marker_entry.acl_name
 * OUTPUT:   marker_entry
 * RETURN:   TRUE/FALSE
 * NOTE:     this API suppose only used by SNMP,
 *           L4_MGR_MARKER_GetNextMarkerEntry is enough for CLI and WWW
 *           the search key is (intf, marker_entry->acl_name)
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_GetMarkerEntry(UI32_T ifindex, MARKER_TYPE_MarkerEntry_T *marker_entry)
{
    BOOL_T ret=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = MARKER_VM_GetMarkerEntry(ifindex, marker_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_GetNextMarkerEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get the next ACL binding to the inteface performaned remark function
 * INPUT:    ifindex     -- logical port
 *           marker_entry.acl_name
 * OUTPUT:   ifindex
 *           marker_entry
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (*ifindex, mark_entry->acl_name)
 *           use the *ifindex=0, marker_entry->acl_name='\0' to get the frist entry
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_GetNextMarkerEntry(UI32_T ifindex, MARKER_TYPE_MarkerEntry_T *marker_entry)
{
    BOOL_T ret=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = MARKER_VM_GetNextMarkerEntry(ifindex, marker_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_SNMPGetNextMarkerEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get the next ACL binding to the inteface performaned remark function
 * INPUT:    ifindex     -- logical port
 *           marker_entry.acl_name
 * OUTPUT:   ifindex
 *           marker_entry
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (*ifindex, mark_entry->acl_name)
 *           use the *ifindex=0, marker_entry->acl_name='\0' to get the frist entry
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_SNMPGetNextMarkerEntry(UI32_T *ifindex, MARKER_TYPE_MarkerEntry_T *marker_entry)
{
    BOOL_T ret=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = MARKER_VM_SNMPGetNextMarkerEntry(ifindex, marker_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_GetNextRunningMarkerEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get marker entry
 * INPUT:    ifindex     --
 *           marker_entry.acl_name
 * OUTPUT:   marker_entry
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     the search key is (intf, marker_entry->acl_name)
 *
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_MARKER_GetNextRunningMarkerEntry(UI32_T ifindex, MARKER_TYPE_MarkerEntry_T *marker_entry)
{
    BOOL_T ret=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);

    L4_MGR_LOCK();
    ret = MARKER_VM_GetNextRunningMarkerEntry(ifindex, marker_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_SetMarkEntryRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to set the remark entry
 * INPUT:    (ifindex, acl_name) is the key
 *           row_status
 * OUTPUT :  None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_SetMarkerEntryRowStatus(UI32_T ifindex, MARKER_TYPE_MarkerEntry_T *marker_entry)
{
    BOOL_T ret=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = MARKER_VM_SetMarkerEntryRowStatus(ifindex, marker_entry, marker_entry->row_status);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_GetMarkerEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  to get the marker entry
 * INPUT:
 * OUTPUT:   marker_entry
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_GetDefaultMarkerEntry(MARKER_TYPE_MarkerEntry_T *marker_entry)
{
    BOOL_T ret=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = MARKER_VM_GetDefaultMarkerEntry(marker_entry);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_SetMarkerEntry_Action
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to set the remark entry
 * INPUT:    ifindex          -- is one of the search key
 *           marker_entry  --
 *
 * OUTPUT :
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (intf, marker_entry->acl_name)
 *           SNMP do not need to care of marker_entry->acl_type
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_SetMarkerEntry_Action(UI32_T ifindex, UI8_T acl_name[], UI32_T action)
{
    BOOL_T ret=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = MARKER_VM_SetMarkerEntry_Action(ifindex, acl_name, action);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_SetMarkerEntry_Dscp
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to set the remark entry
 * INPUT:    ifindex          -- is one of the search key
 *           marker_entry  --
 *
 * OUTPUT :
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (intf, marker_entry->acl_name)
 *           SNMP do not need to care of marker_entry->acl_type
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_SetMarkerEntry_Dscp(UI32_T ifindex, UI8_T acl_name[], UI32_T value)
{
    BOOL_T ret=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = MARKER_VM_SetMarkerEntry_Dscp(ifindex, acl_name, value);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_SetMarkerEntry_Precedence
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to set the remark entry
 * INPUT:    ifindex          -- is one of the search key
 *           marker_entry  --
 *
 * OUTPUT :
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (intf, marker_entry->acl_name)
 *           SNMP do not need to care of marker_entry->acl_type
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_SetMarkerEntry_Precedence(UI32_T ifindex, UI8_T acl_name[], UI32_T value)
{
    BOOL_T ret=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = MARKER_VM_SetMarkerEntry_Precedence(ifindex, acl_name, value);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_SetMarkerEntry_Priority
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to set the remark entry
 * INPUT:    ifindex          -- is one of the search key
 *           marker_entry  --
 *
 * OUTPUT :
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (intf, marker_entry->acl_name)
 *           SNMP do not need to care of marker_entry->acl_type
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_SetMarkerEntry_Priority(UI32_T ifindex, UI8_T acl_name[], UI32_T value)
{
    BOOL_T ret=FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = MARKER_VM_SetMarkerEntry_Priority(ifindex, acl_name, value);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_MARKER_InsertModulePort
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
void L4_MGR_MARKER_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default)
{
    /*should not do anything*/
    return ;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_MARKER_RemoveModulePort
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
void L4_MGR_MARKER_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port)
{
    UI32_T ifindex = starting_port_ifindex;
    MARKER_TYPE_MarkerEntry_T marker_entry;
    memset(&marker_entry, 0, sizeof(marker_entry));

    /*remove port attribute*/
    for (ifindex = starting_port_ifindex; ifindex <= starting_port_ifindex+number_of_port; ifindex++)
    {
        while(L4_MGR_MARKER_GetNextMarkerEntry(ifindex, &marker_entry) == TRUE)
        {
            L4_MGR_MARKER_DelMarkerEntry(ifindex, marker_entry.acl_name, marker_entry.acl_type);
        }
    }
    return ;
}
#endif /*#if(SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)*/
/* For CPU interface */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_TrapPacket2Cpu
 *------------------------------------------------------------------------------
 * PURPOSE:  trap packet to CPU or not
 * INPUT:    enable_flag, packet_type, rule_info
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_TrapPacket2Cpu(BOOL_T enable_flag, RULE_TYPE_PacketType_T packet_type, RULE_TYPE_CpuRuleInfo_T *rule_info)
{
    UI32_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    ret = RULE_MGR_TrapPacket2Cpu(enable_flag, packet_type, rule_info);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MirrorPacket2Cpu
 *------------------------------------------------------------------------------
 * PURPOSE:  copy packet to CPU or not
 * INPUT:    enable_flag, packet_type, rule_info
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_MirrorPacket2Cpu(BOOL_T enable_flag, RULE_TYPE_PacketType_T packet_type, RULE_TYPE_CpuRuleInfo_T *rule_info)
{
    UI32_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    ret = RULE_MGR_MirrorPacket2Cpu(enable_flag, packet_type, rule_info);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}


/* For MIB */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get port entry by ifindex
 * INPUT    : ifindex
 *            inout_profile - direction
 * OUTPUT   : port_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetDiffServPortEntry(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    RULE_TYPE_UI_PortEntry_T *port_entry)
{
    BOOL_T ret = FALSE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    ret = (RULE_TYPE_OK == RULE_MGR_GetUIPortEntry(ifindex, inout_profile, port_entry));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next port entry by ifindex
 * INPUT    : ifindex
 * OUTPUT   : ifindex, inout_profile, port_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetNextDiffServPortEntry(
    UI32_T *ifindex_p,
    RULE_TYPE_InOutDirection_T *inout_profile_p,
    RULE_TYPE_UI_PortEntry_T *port_entry_p)
{
    BOOL_T ret = FALSE;
    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = (RULE_TYPE_OK == RULE_MGR_GetNextUIPortEntry(ifindex_p, inout_profile_p, port_entry_p));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPortPolicyMapIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : bind port(ifindex) to policy-map(policy_map_index)
 * INPUT    : ifindex, inout_profile, policy_map_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPortPolicyMapIndex(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T policy_map_index
    )
{
    BOOL_T                      ret;
    RULE_TYPE_UI_PortEntry_T    port_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (0 != policy_map_index)
    {
        ret = (RULE_TYPE_OK == RULE_MGR_BindPort2PolicyMap(ifindex, inout_profile, policy_map_index));
    }
    else
    {
        /* get the current policy-map index bound to port (ifindex) */
        ret = (RULE_TYPE_OK == RULE_MGR_GetUIPortEntry(ifindex, inout_profile, &port_entry));

        if (TRUE == ret)
        {
            ret = (port_entry.policy_map_index == 0);

            /* if no policy-map index bound to port, do nothing */
            if (FALSE == ret)
                ret = (RULE_TYPE_OK == RULE_MGR_UnBindPortFromPolicyMap(ifindex, inout_profile, port_entry.policy_map_index));
        }
    }

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPortIngressAclIndexByAclType
 *------------------------------------------------------------------------------
 * PURPOSE  : bind port(ifindex) to ingress ACL(acl_index) of acl_type
 * INPUT    : ifindex, acl_index, acl_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static BOOL_T L4_MGR_QoS_SetDiffServPortIngressAclIndexByAclType(
    UI32_T ifindex,
    UI32_T acl_index,
    RULE_TYPE_AclType_T acl_type)
{
    BOOL_T                      ret;
    RULE_TYPE_UI_PortEntry_T    port_entry;
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    UI8_T time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
#else
    UI8_T *time_range_name = NULL;
#endif

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (0 != acl_index)
    {
        ret = (RULE_TYPE_OK == RULE_MGR_BindPort2Acl(ifindex, acl_index, acl_type,
                                                     TRUE,
                                                     time_range_name,
                                                     RULE_TYPE_COUNTER_DISABLE));
    }
    else
    {
        /* get the cuurent acl_index of acl_type bound to port (ifindex) */
        ret = (RULE_TYPE_OK == RULE_MGR_GetUIPortEntry(ifindex, RULE_TYPE_INBOUND, &port_entry));

        if (TRUE == ret)
        {
            switch (acl_type)
            {
            case RULE_TYPE_IP_ACL:
                acl_index = port_entry.ingress_ip_acl_index;
                break;

            case RULE_TYPE_MAC_ACL:
                acl_index = port_entry.ingress_mac_acl_index;
                break;

            case RULE_TYPE_IPV6_ACL:
                acl_index = port_entry.ingress_ipv6_acl_index;
                break;

            default:
                ret = FALSE;
                break;
            }

            /* do work if acl_type is ok */
            if (TRUE == ret)
            {
                ret = (acl_index == 0);

                /* if no acl_index of acl_type bound to port, do nothing */
                if (FALSE == ret)
                    ret = (RULE_TYPE_OK == RULE_MGR_UnBindPortFromAcl(ifindex, acl_index, acl_type, TRUE));
            }
        }
    }

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPortIngressIpAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : bind port(ifindex) to ingress IP ACL(acl_index)
 * INPUT    : ifindex, acl_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPortIngressIpAclIndex(UI32_T ifindex, UI32_T acl_index)
{
    return L4_MGR_QoS_SetDiffServPortIngressAclIndexByAclType (ifindex, acl_index, RULE_TYPE_IP_ACL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPortIngressIpv6AclIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : bind port(ifindex) to ingress IPv6 ACL(acl_index)
 * INPUT    : ifindex, acl_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPortIngressIpv6AclIndex(UI32_T ifindex, UI32_T acl_index)
{
    return L4_MGR_QoS_SetDiffServPortIngressAclIndexByAclType (ifindex, acl_index, RULE_TYPE_IPV6_ACL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPortIngressMacAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : bind port(ifindex) to ingress MAC ACL(acl_index)
 * INPUT    : ifindex, acl_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPortIngressMacAclIndex(UI32_T ifindex, UI32_T acl_index)
{
    return L4_MGR_QoS_SetDiffServPortIngressAclIndexByAclType (ifindex, acl_index, RULE_TYPE_MAC_ACL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServPolicyMapEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get policy-map by policy_map_index
 * INPUT    : policy_map_index
 * OUTPUT   : policy_map
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetDiffServPolicyMapEntry(UI32_T policy_map_index, RULE_TYPE_MIB_PolicyMap_T *policy_map)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    ret = (RULE_TYPE_OK == RULE_MGR_GetMibPolicyMap(policy_map_index, policy_map));

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServPolicyMapEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next policy-map by policy_map_index
 * INPUT    : policy_map_index
 * OUTPUT   : policy_map_index, policy_map
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetNextDiffServPolicyMapEntry(UI32_T *policy_map_index, RULE_TYPE_MIB_PolicyMap_T *policy_map)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    ret = (RULE_TYPE_OK == RULE_MGR_GetNextMibPolicyMap(policy_map_index, policy_map));

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPolicyMapName
 *------------------------------------------------------------------------------
 * PURPOSE  : change policy-map name by policy_map_index
 * INPUT    : policy_map_index, policy_map_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPolicyMapName(UI32_T policy_map_index, const char *policy_map_name)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetPolicyMapNameById(policy_map_index, policy_map_name));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPolicyMapDescription
 *------------------------------------------------------------------------------
 * PURPOSE  : change policy-map description by policy_map_index
 * INPUT    : policy_map_index, policy_map_desc
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPolicyMapDescription(UI32_T policy_map_index, const char *policy_map_desc)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetPolicyMapDescById(policy_map_index, policy_map_desc));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPolicyMapStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change policy-map row status by policy_map_index
 * INPUT    : policy_map_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPolicyMapStatus(UI32_T policy_map_index, UI32_T row_status)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetPolicyMapRowStatus(policy_map_index, row_status));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetPolicydiffServPolicyMapAttachCtlAction
 *------------------------------------------------------------------------------
 * PURPOSE  : dissociate/associate policy-map and policy-map element
 * INPUT    : policy_map_index, policy_element_index, attach_action
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : attach_action (1:notCombining,2:Combine,3:Discombine)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetPolicydiffServPolicyMapAttachCtlAction(UI32_T policy_map_index, UI32_T policy_element_index, UI32_T attach_action)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetPolicyMapAttachCtrl(policy_map_index, policy_element_index, attach_action));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServPolicyMapElementEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get policy-map element by policy_element_index
 * INPUT    : policy_element_index
 * OUTPUT   : policy_element
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetDiffServPolicyMapElementEntry(UI32_T policy_element_index, RULE_TYPE_PolicyElement_T *policy_element)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    ret = (RULE_TYPE_OK == RULE_MGR_GetPolicyMapElement(policy_element_index, policy_element));

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServPolicyMapElementEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next policy-map element by policy_element_index
 * INPUT    : policy_element_index
 * OUTPUT   : policy_element_index, policy_element
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetNextDiffServPolicyMapElementEntry(UI32_T *policy_element_index, RULE_TYPE_PolicyElement_T *policy_element)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    ret = (RULE_TYPE_OK == RULE_MGR_GetNextPolicyMapElement(policy_element_index, policy_element));

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPolicyMapElementClassMapIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : change class-map index by policy_element_index
 * INPUT    : policy_element_index, class_map_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPolicyMapElementClassMapIndex(UI32_T policy_element_index, UI32_T class_map_index)
{
    BOOL_T                      ret;
    RULE_TYPE_PolicyElement_T   policy_element;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetPolicyMapElement(policy_element_index, &policy_element))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    policy_element.class_map_index = class_map_index;
    ret = (RULE_TYPE_OK == RULE_MGR_SetPolicyMapElement(policy_element_index, &policy_element));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter index by policy_element_index
 * INPUT    : policy_element_index, meter_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex(UI32_T policy_element_index, UI32_T meter_index)
{
    BOOL_T                      ret;
    RULE_TYPE_PolicyElement_T   policy_element;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetPolicyMapElement(policy_element_index, &policy_element))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    policy_element.meter_index = meter_index;
    ret = (RULE_TYPE_OK == RULE_MGR_SetPolicyMapElement(policy_element_index, &policy_element));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPolicyMapElementActionIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : change action index by policy_element_index
 * INPUT    : policy_element_index, action_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPolicyMapElementActionIndex(UI32_T policy_element_index, UI32_T action_index)
{
    BOOL_T                      ret;
    RULE_TYPE_PolicyElement_T   policy_element;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetPolicyMapElement(policy_element_index, &policy_element))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    policy_element.action_index = action_index;
    ret = (RULE_TYPE_OK == RULE_MGR_SetPolicyMapElement(policy_element_index, &policy_element));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPolicyMapElementStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change policy-map element row status by policy_element_index
 * INPUT    : policy_element_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPolicyMapElementStatus(UI32_T policy_element_index, UI32_T row_status)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetPolicyMapElementRowStatus(policy_element_index, row_status));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServClassMapEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get class-map by class_map_index
 * INPUT    : class_map_index
 * OUTPUT   : class_map
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetDiffServClassMapEntry(UI32_T class_map_index, RULE_TYPE_MIB_ClassMap_T *class_map)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    ret = (RULE_TYPE_OK == RULE_MGR_GetMibClassMap(class_map_index, class_map));

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServClassMapEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next class-map by class_map_index
 * INPUT    : class_map_index
 * OUTPUT   : class_map_index, class_map
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetNextDiffServClassMapEntry(UI32_T *class_map_index, RULE_TYPE_MIB_ClassMap_T *class_map)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    ret = (RULE_TYPE_OK == RULE_MGR_GetNextMibClassMap(class_map_index, class_map));

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServClassMapName
 *------------------------------------------------------------------------------
 * PURPOSE  : change class-map name by class_map_index
 * INPUT    : class_map_index, class_map_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServClassMapName(UI32_T class_map_index, const char *class_map_name)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetClassMapNameById(class_map_index, class_map_name));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServClassMapDescription
 *------------------------------------------------------------------------------
 * PURPOSE  : change class-map description by class_map_index
 * INPUT    : class_map_index, class_map_desc
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServClassMapDescription(UI32_T class_map_index, const char *class_map_desc)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetClassMapDescById(class_map_index, class_map_desc));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServClassMapMatchType
 *------------------------------------------------------------------------------
 * PURPOSE  : change class-map matching type by class_map_index
 * INPUT    : class_map_index, match_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServClassMapMatchType(UI32_T class_map_index, UI32_T match_type)
{
    BOOL_T                  ret =TRUE;
    RULE_TYPE_ClassMap_T    class_map;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetClassMap(class_map_index, &class_map))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    /* convert match_type of MIB to
     *         match_type of core layer.
     */

    switch (match_type)
    {
    case VAL_diffServClassMapMatchType_matchAny:
        class_map.class_map_match_type = RULE_TYPE_CLASS_MAP_MATCH_ANY;
        break;

    case VAL_diffServClassMapMatchType_matchAll:
        class_map.class_map_match_type = RULE_TYPE_CLASS_MAP_MATCH_ALL;
        break;

    default:
        ret = FALSE;
    }

    if (TRUE == ret)
        ret = (RULE_TYPE_OK == RULE_MGR_SetClassMap(class_map_index, &class_map));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPolicyMapStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change class-map row status by class_map_index
 * INPUT    : class_map_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServClassMapStatus(UI32_T class_map_index, UI32_T row_status)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetClassMapRowStatus(class_map_index, row_status));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServClassMapAttachCtlAction
 *------------------------------------------------------------------------------
 * PURPOSE  : dissociate/associate class-map and class-map element
 * INPUT    : class_map_index, index_type, class_element_index, attach_action
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : index_type(??)
 *            attach_action (1:notCombining,2:Combine,3:Discombine)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServClassMapAttachCtlAction(UI32_T class_map_index, UI32_T index_type, UI32_T class_element_index, UI32_T attach_action)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetClassMapAttachCtrl(class_map_index, index_type, class_element_index, attach_action));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServAclEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get acl by acl_index
 * INPUT    : acl_index
 * OUTPUT   : acl_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetDiffServAclEntry(UI32_T acl_index, RULE_TYPE_MIB_Acl_T *acl_entry)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    ret = (RULE_TYPE_OK == RULE_MGR_GetMibAclByIndex(acl_index, acl_entry));

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServAclEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next acl by acl_index
 * INPUT    : acl_index
 * OUTPUT   : acl_index, acl_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetNextDiffServAclEntry(UI32_T *acl_index, RULE_TYPE_MIB_Acl_T *acl_entry)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    ret = (RULE_TYPE_OK == RULE_MGR_GetNextMibAclByIndex(acl_index, acl_entry));

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServAclType
 *------------------------------------------------------------------------------
 * PURPOSE  : change ACL type by acl_index
 * INPUT    : acl_index, acl_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServAclType(UI32_T acl_index, UI32_T acl_type)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetAclTypeByIndex(acl_index, acl_type));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServAclName
 *------------------------------------------------------------------------------
 * PURPOSE  : change policy-map name by acl_index
 * INPUT    : acl_index, acl_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServAclName(UI32_T acl_index, const char *acl_name)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetAclNameByIndex(acl_index, acl_name));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServAclStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change acl row status by acl_index
 * INPUT    : acl_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServAclStatus(UI32_T acl_index, UI32_T row_status)
{
    BOOL_T  ret;
#if (SYS_CPNT_PBR == TRUE)
    char acl_name[SYS_ADPT_ACL_MAX_NAME_LEN+1] = {0};

    RULE_MGR_GetAclNameById(acl_index, acl_name);
#endif /* SYS_CPNT_PBR */

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetAclRowStatus(acl_index, row_status));
    L4_MGR_UNLOCK();

#if (SYS_CPNT_PBR == TRUE)
    if (ret == TRUE)
    {
        switch (row_status)
        {
            case VAL_diffServPolicyMapStatus_active:
                SYS_CALLBACK_MGR_L4_AddAclCallback(SYS_MODULE_ACL, acl_index, acl_name);
                break;

            case VAL_diffServPolicyMapStatus_destroy:
                SYS_CALLBACK_MGR_L4_DeleteAclCallback(SYS_MODULE_ACL, acl_index, acl_name);
                break;

            default:
                /* Do nothing
                 */
                break;
        }
    }
#endif /* SYS_CPNT_PBR */

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServAclAttachCtlAction
 *------------------------------------------------------------------------------
 * PURPOSE  : dissociate/associate class-map and class-map element
 * INPUT    : acl_index, ace_type, ace_index, attach_action
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : attach_action (1:notCombining,2:Combine,3:Discombine)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServAclAttachCtlAction(UI32_T acl_index, UI32_T ace_type, UI32_T ace_index, UI32_T attach_action)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetAclAttachCtrl(acl_index, ace_type, ace_index, attach_action));
    L4_MGR_UNLOCK();

#if (SYS_CPNT_PBR == TRUE)
    if (ret == TRUE)
    {
        RULE_TYPE_MIB_Acl_T     acl_entry;
        RULE_TYPE_RETURN_TYPE_T result;

        result = RULE_MGR_GetMibAclByIndex(acl_index, &acl_entry);
        if (result == RULE_TYPE_OK &&
            acl_entry.row_status == VAL_diffServAclStatus_active)
        {
            char acl_name[SYS_ADPT_ACL_MAX_NAME_LEN+1] = {0};

            result = RULE_MGR_GetAclNameById(acl_index, acl_name);
            ASSERT(result == RULE_TYPE_OK);

            SYS_CALLBACK_MGR_L4_ModifyAclCallback(SYS_MODULE_ACL, acl_index, acl_name);
        }
    }
#endif /* SYS_CPNT_PBR */

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServIpAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get ace by ip_ace_index
 * INPUT    : ip_ace_index
 * OUTPUT   : ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetDiffServIpAceEntry(
    UI32_T ip_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if ((NULL == ace_p) ||
        (RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, ace_p)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace_p->ace_type)))
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServIpAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next ace by ip_ace_index
 * INPUT    : ip_ace_index
 * OUTPUT   : ip_ace_index, ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetNextDiffServIpAceEntry(
    UI32_T *ip_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if ((NULL == ip_ace_index) || (NULL == ace_p) ||
        (RULE_TYPE_OK != RULE_MGR_GetNextUIAceByAceTypeAndIndex(RULE_TYPE_IP_ACL, ip_ace_index, ace_p)))
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceType
 *------------------------------------------------------------------------------
 * PURPOSE  : Set ace type by ip_ace_index
 * INPUT    : ip_ace_index, mib_ace_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceType(UI32_T ip_ace_index, UI32_T mib_ace_type)
{
    BOOL_T                  ret =TRUE;
    RULE_TYPE_AclType_T     core_ace_type = RULE_TYPE_IP_STD_ACL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    switch (mib_ace_type)
    {
    case VAL_diffServIpAceType_standard:
        core_ace_type = RULE_TYPE_IP_STD_ACL;
        break;

    case VAL_diffServIpAceType_extended:
        core_ace_type = RULE_TYPE_IP_EXT_ACL;
        break;

    default:
        ret = FALSE;
        break;
    }

    if (TRUE == ret)
    {
        ret = (RULE_TYPE_OK == RULE_MGR_SetAceTypeByIndex(ip_ace_index, core_ace_type));
    }

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceAccess
 *------------------------------------------------------------------------------
 * PURPOSE  : change access type by ip_ace_index
 * INPUT    : ip_ace_index, access_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceAccess(UI32_T ip_ace_index, UI32_T access_type)
{
    BOOL_T                   ret = TRUE;
    RULE_TYPE_UI_Ace_Entry_T ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        ret = FALSE;
    }

    if (TRUE == ret)
    {
        switch (access_type)
        {
        case VAL_diffServIpAceAccess_permit:
            ace.access = RULE_TYPE_ACE_PERMIT;
            break;

        case VAL_diffServIpAceAccess_deny:
            ace.access = RULE_TYPE_ACE_DENY;
            break;

        default:
            ret = FALSE;
            break;

        }
    }

    if (TRUE == ret)
    {
        ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                             &ace,
                                                             LEAF_diffServIpAceAccess));
    }

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceSourceIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip by ip_ace_index
 * INPUT    : ip_ace_index, src_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceSourceIpAddr(UI32_T ip_ace_index, UI32_T src_ip)
{
    BOOL_T                   ret;
    RULE_TYPE_UI_Ace_Entry_T ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace.ipv4.sip.addr,
           &src_ip,
           sizeof(ace.ipv4.sip.addr));

    if (0 == ace.ipv4.sip.mask[0] && 0 == ace.ipv4.sip.mask[1] &&
        0 == ace.ipv4.sip.mask[2] && 0 == ace.ipv4.sip.mask[3])
    {
        ace.ipv4.sip.mask[0] = ace.ipv4.sip.mask[1] = ace.ipv4.sip.mask[2] = ace.ipv4.sip.mask[3] = 255;
    }

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceSourceIpAddr));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceSourceIpAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip mask by ip_ace_index
 * INPUT    : ip_ace_index, src_ip_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceSourceIpAddrBitmask(UI32_T ip_ace_index, UI32_T src_ip_mask)
{
    BOOL_T                  ret;
    RULE_TYPE_UI_Ace_Entry_T ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace.ipv4.sip.mask,
           &src_ip_mask,
           sizeof(ace.ipv4.sip.mask));

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceSourceIpAddrBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceDestIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip by ip_ace_index
 * INPUT    : ip_ace_index, dst_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceDestIpAddr(UI32_T ip_ace_index, UI32_T dst_ip)
{
    BOOL_T                   ret;
    RULE_TYPE_UI_Ace_Entry_T ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace.ipv4.dip.addr,
           &dst_ip,
           sizeof(ace.ipv4.dip.addr));

    if (0 == ace.ipv4.dip.mask[0] && 0 == ace.ipv4.dip.mask[1] &&
        0 == ace.ipv4.dip.mask[2] && 0 == ace.ipv4.dip.mask[3])
    {
        ace.ipv4.dip.mask[0] = ace.ipv4.dip.mask[1] = ace.ipv4.dip.mask[2] = ace.ipv4.dip.mask[3] = 255;
    }

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceDestIpAddr));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceDestIpAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip mask by ip_ace_index
 * INPUT    : ip_ace_index, dst_ip_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceDestIpAddrBitmask(UI32_T ip_ace_index, UI32_T dst_ip_mask)
{
    BOOL_T                   ret;
    RULE_TYPE_UI_Ace_Entry_T ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace.ipv4.dip.mask,
           &dst_ip_mask,
           sizeof(ace.ipv4.dip.mask));

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceDestIpAddrBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceProtocol
 *------------------------------------------------------------------------------
 * PURPOSE  : change protocol_type by ip_ace_index
 * INPUT    : ip_ace_index, protocol_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceProtocol(UI32_T ip_ace_index, UI32_T protocol_type)
{
    BOOL_T                  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    /* clear the controlCode and controlCodeBitmaks when the protocol is not TCP
     */
    if ((TRUE  == L4_MGR_IS_TCP_ACL(ace.ipv4.protocol.u.s.data)) &&
        (FALSE == L4_MGR_IS_TCP_ACL(protocol_type)))
    {
        ace.tcp.flags.data.u.code = 0;
        ace.tcp.flags.mask.u.code = 0;
    }

    ace.ipv4.protocol.op = (protocol_type == RULE_TYPE_UNDEF_IP_PROTOCOL) ?
                                 RULE_TYPE_IPV4_PROTOCOL_OP_NO_OPERATION :
                                 RULE_TYPE_IPV4_PROTOCOL_OP_EQUAL;
    ace.ipv4.protocol.u.s.data = protocol_type;
    ace.ipv4.protocol.u.s.mask = MAX_diffServIpAceProtocol - 1;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceProtocol));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAcePrec
 *------------------------------------------------------------------------------
 * PURPOSE  : change ip precedence by ip_ace_index
 * INPUT    : ip_ace_index, ip_precedence
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAcePrec(UI32_T ip_ace_index, UI32_T ip_precedence)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ipv4.dscp.op = (ip_precedence == RULE_TYPE_UNDEF_IP_PRECEDENCE) ?
                       RULE_TYPE_IPV4_DSCP_OP_NO_OPERATION :
                       RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE;
    ace.ipv4.dscp.u.tos.precedence = ip_precedence;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAcePrec));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceTos
 *------------------------------------------------------------------------------
 * PURPOSE  : change tos by ip_ace_index
 * INPUT    : ip_ace_index, tos
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceTos(UI32_T ip_ace_index, UI32_T tos)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ipv4.dscp.op = (tos == RULE_TYPE_UNDEF_IP_TOS) ?
                       RULE_TYPE_IPV4_DSCP_OP_NO_OPERATION :
                       RULE_TYPE_IPV4_DSCP_OP_TOS;
    ace.ipv4.dscp.u.tos.tos = tos;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceTos));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change dscp by ip_ace_index
 * INPUT    : ip_ace_index, dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceDscp(UI32_T ip_ace_index, UI32_T dscp)
{
    BOOL_T                  ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ipv4.dscp.op = (dscp == RULE_TYPE_UNDEF_IP_DSCP) ?
                       RULE_TYPE_IPV4_DSCP_OP_NO_OPERATION :
                       RULE_TYPE_IPV4_DSCP_OP_DSCP;
    ace.ipv4.dscp.u.ds = dscp;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceDscp));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceSourcePortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change source port operator by ip_ace_index
 * INPUT    : ip_ace_index, src_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceSourcePortOp(UI32_T ip_ace_index, UI32_T src_port_operator)
{
    BOOL_T                  ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.l4_common.sport.op = src_port_operator;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceSourcePortOp));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceMinSourcePort
 *------------------------------------------------------------------------------
 * PURPOSE  : change min src port by ip_ace_index
 * INPUT    : ip_ace_index, min_src_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceMinSourcePort(UI32_T ip_ace_index, UI32_T min_src_port)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (VAL_diffServIpAceSourcePortOp_range == ace.l4_common.sport.op)
    {
        ace.l4_common.sport.u.s.data = min_src_port;
    }
    else
    {
        ace.l4_common.sport.u.range.min = min_src_port;
    }

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceMinSourcePort));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceMaxSourcePort
 *------------------------------------------------------------------------------
 * PURPOSE  : change max src port by ip_ace_index
 * INPUT    : ip_ace_index, max_src_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceMaxSourcePort(UI32_T ip_ace_index, UI32_T max_src_port)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.l4_common.sport.u.range.max = max_src_port;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceMaxSourcePort));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceSourcePortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src port mask by ip_ace_index
 * INPUT    : ip_ace_index, src_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceSourcePortBitmask(UI32_T ip_ace_index, UI32_T src_port_mask)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.l4_common.sport.u.s.mask = src_port_mask;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceSourcePortBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceDestPortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port operator by ip_ace_index
 * INPUT    : ip_ace_index, dst_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceDestPortOp(UI32_T ip_ace_index, UI32_T dst_port_operator)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.l4_common.dport.op = dst_port_operator;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceDestPortOp));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceMinDestPort
 *------------------------------------------------------------------------------
 * PURPOSE  : change min dst port by ip_ace_index
 * INPUT    : ip_ace_index, min_dst_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceMinDestPort(UI32_T ip_ace_index, UI32_T min_dst_port)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;


    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (VAL_diffServIpAceDestPortOp_range == ace.l4_common.dport.op)
    {
        ace.l4_common.dport.u.s.data = min_dst_port;
    }
    else
    {
        ace.l4_common.dport.u.range.min = min_dst_port;
    }

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceMinDestPort));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceMaxDestPort
 *------------------------------------------------------------------------------
 * PURPOSE  : change max dst port by ip_ace_index
 * INPUT    : ip_ace_index, max_dst_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceMaxDestPort(UI32_T ip_ace_index, UI32_T max_dst_port)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.l4_common.dport.u.range.max = max_dst_port;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceMaxDestPort));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceDestPortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port mask by ip_ace_index
 * INPUT    : ip_ace_index, dst_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceDestPortBitmask(UI32_T ip_ace_index, UI32_T dst_port_mask)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.l4_common.dport.u.s.mask = dst_port_mask;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceDestPortBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceControlCode
 *------------------------------------------------------------------------------
 * PURPOSE  : change control_code by ip_ace_index
 * INPUT    : ip_ace_index, control_code
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceControlCode(UI32_T ip_ace_index, UI32_T control_code)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)) ||
        (FALSE == L4_MGR_IS_TCP_ACL(ace.ipv4.protocol.u.s.data)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.tcp.flags.data.u.code = control_code;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceControlCode));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceControlCodeBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change control_code mask by ip_ace_index
 * INPUT    : ip_ace_index, control_code_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceControlCodeBitmask(UI32_T ip_ace_index, UI32_T control_code_mask)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)) ||
        (FALSE == L4_MGR_IS_TCP_ACL(ace.ipv4.protocol.u.s.data)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.tcp.flags.mask.u.code = control_code_mask;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceControlCodeBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change ip ace row_status by ip_ace_index
 * INPUT    : ip_ace_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceStatus(UI32_T ip_ace_index, UI32_T row_status)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetAceRowStatus(ip_ace_index, RULE_TYPE_IP_ACL, row_status));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceTimeRange
 *------------------------------------------------------------------------------
 * PURPOSE  : change ip ace time_range by ip_ace_index
 * INPUT    : ip_ace_index, time_range_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServIpAceTimeRange(
    UI32_T ip_ace_index,
    UI8_T *time_range_name)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ip_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    strncpy((char *)ace.time_range_name, (char *)time_range_name, sizeof(ace.time_range_name) - 1);
    ace.time_range_name[sizeof(ace.time_range_name) - 1] = '\0';

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ip_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpAceTimeRange));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServMacAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get ace by mac_ace_index
 * INPUT    : mac_ace_index
 * OUTPUT   : ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QoS_GetDiffServMacAceEntry(
    UI32_T mac_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if ((NULL == ace_p) ||
        (RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, ace_p)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace_p->ace_type)))
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServMacAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next mac ace by mac_ace_index
 * INPUT    : mac_ace_index_p
 * OUTPUT   : mac_ace_index_p, ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QoS_GetNextDiffServMacAceEntry(
    UI32_T *mac_ace_index_p,
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if ((NULL == mac_ace_index_p) || (NULL == ace_p) ||
        (RULE_TYPE_OK != RULE_MGR_GetNextUIAceByAceTypeAndIndex(
                             RULE_TYPE_MAC_ACL, mac_ace_index_p, ace_p)))
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceAccess
 *------------------------------------------------------------------------------
 * PURPOSE  : change access type by mac_ace_index
 * INPUT    : mac_ace_index, access_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceAccess(UI32_T mac_ace_index, UI32_T access_type)
{
    BOOL_T                    ret = TRUE;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        ret = FALSE;
    }

    if (TRUE == ret)
    {
        switch (access_type)
        {
        case VAL_diffServMacAceAccess_permit:
            ace.access = RULE_TYPE_ACE_PERMIT;
            break;

        case VAL_diffServMacAceAccess_deny:
            ace.access = RULE_TYPE_ACE_DENY;
            break;

        default:
            ret = FALSE;
            break;
        }
    }

    if (TRUE == ret)
    {
        ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                             &ace,
                                                             LEAF_diffServMacAceAccess));
    }

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAcePktformat
 *------------------------------------------------------------------------------
 * PURPOSE  : change packet format by mac_ace_index
 * INPUT    : mac_ace_index, pkt_format
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAcePktformat(UI32_T mac_ace_index, UI32_T pkt_format)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.pkt_format = pkt_format;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAcePktformat));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceSourceMacAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src mac by mac_ace_index
 * INPUT    : mac_ace_index, src_mac
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceSourceMacAddr(UI32_T mac_ace_index, const UI8_T *src_mac)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace.ether.sa.addr, src_mac, SYS_ADPT_MAC_ADDR_LEN);
    if (0 == (ace.ether.sa.mask[0] | ace.ether.sa.mask[1] | ace.ether.sa.mask[2] |
              ace.ether.sa.mask[3] | ace.ether.sa.mask[4] | ace.ether.sa.mask[5]))
    {
        ace.ether.sa.mask[0] = ace.ether.sa.mask[1] = ace.ether.sa.mask[2] = 0xff;
        ace.ether.sa.mask[3] = ace.ether.sa.mask[4] = ace.ether.sa.mask[5] = 0xff;
    }
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceSourceMacAddr));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceSourceMacAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src mac mask by mac_ace_index
 * INPUT    : mac_ace_index, src_mac_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceSourceMacAddrBitmask(UI32_T mac_ace_index, const UI8_T *src_mac_mask)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace.ether.sa.mask, src_mac_mask, SYS_ADPT_MAC_ADDR_LEN);
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceSourceMacAddrBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestMacAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst mac by mac_ace_index
 * INPUT    : mac_ace_index, dst_mac
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceDestMacAddr(UI32_T mac_ace_index, const UI8_T *dst_mac)
{
    BOOL_T                  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace.ether.da.addr, dst_mac, SYS_ADPT_MAC_ADDR_LEN);
    if (0 == (ace.ether.da.mask[0] | ace.ether.da.mask[1] | ace.ether.da.mask[2] |
              ace.ether.da.mask[3] | ace.ether.da.mask[4] | ace.ether.da.mask[5]))
    {
        ace.ether.da.mask[0] = ace.ether.da.mask[1] = ace.ether.da.mask[2] = 0xff;
        ace.ether.da.mask[3] = ace.ether.da.mask[4] = ace.ether.da.mask[5] = 0xff;
    }
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceDestMacAddr));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestMacAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst mac mask by mac_ace_index
 * INPUT    : mac_ace_index, dst_mac_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceDestMacAddrBitmask(UI32_T mac_ace_index, const UI8_T *dst_mac_mask)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace.ether.da.mask, dst_mac_mask, SYS_ADPT_MAC_ADDR_LEN);
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceDestMacAddrBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceVidOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change vid operator by mac_ace_index
 * INPUT    : mac_ace_index, vid_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceVidOp(UI32_T mac_ace_index, UI32_T vid_operator)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ether.vid.op = vid_operator;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceVidOp));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceMinVid
 *------------------------------------------------------------------------------
 * PURPOSE  : change min vid by mac_ace_index
 * INPUT    : mac_ace_index, min_vid
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceMinVid(UI32_T mac_ace_index, UI32_T min_vid)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (ace.ether.vid.op == VAL_diffServMacAceVidOp_range)
    {
        ace.ether.vid.u.range.min = min_vid;
    }
    else
    {
        ace.ether.vid.u.s.data = min_vid;
    }
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceMinVid));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceVidBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change vid mask by mac_ace_index
 * INPUT    : mac_ace_index, vid_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceVidBitmask(UI32_T mac_ace_index, UI32_T vid_mask)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ether.vid.u.s.mask = vid_mask;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceVidBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceMaxVid
 *------------------------------------------------------------------------------
 * PURPOSE  : change max vid by mac_ace_index
 * INPUT    : mac_ace_index, max_vid
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceMaxVid(UI32_T mac_ace_index, UI32_T max_vid)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ether.vid.u.range.max = max_vid;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceMaxVid));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceEtherTypeOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change ether-type operator by mac_ace_index
 * INPUT    : mac_ace_index, ether_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceEtherTypeOp(UI32_T mac_ace_index, UI32_T ether_operator)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ether.ethertype.op = ether_operator;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceEtherTypeOp));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceEtherTypeBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change ether-type mask by mac_ace_index
 * INPUT    : mac_ace_index, ether_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceEtherTypeBitmask(UI32_T mac_ace_index, UI32_T ether_mask)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ether.ethertype.u.s.mask = ether_mask;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceEtherTypeBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceMinEtherType
 *------------------------------------------------------------------------------
 * PURPOSE  : change min ether-type by mac_ace_index
 * INPUT    : mac_ace_index, min_ether_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceMinEtherType(UI32_T mac_ace_index, UI32_T min_ether_type)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (ace.ether.ethertype.op == VAL_diffServMacAceEtherTypeOp_range)
    {
        ace.ether.ethertype.u.range.min = min_ether_type;
    }
    else
    {
        ace.ether.ethertype.u.s.data = min_ether_type;
    }
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceMinEtherType));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceMaxEtherType
 *------------------------------------------------------------------------------
 * PURPOSE  : change max ether-type by mac_ace_index
 * INPUT    : mac_ace_index, max_ether_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceMaxEtherType(UI32_T mac_ace_index, UI32_T max_ether_type)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ether.ethertype.u.range.max = max_ether_type;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceMaxEtherType));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change row status by mac_ace_index
 * INPUT    : mac_ace_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceStatus(UI32_T mac_ace_index, UI32_T row_status)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetAceRowStatus(mac_ace_index, RULE_TYPE_MAC_ACL, row_status));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceTimeRange
 *------------------------------------------------------------------------------
 * PURPOSE  : change ace time_range by mac_ace_index
 * INPUT    : mac_ace_index, time_range_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceTimeRange(
    UI32_T mac_ace_index,
    UI8_T *time_range_name)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    strncpy((char *)ace.time_range_name, (char *)time_range_name, sizeof(ace.time_range_name) - 1);
    ace.time_range_name[sizeof(ace.time_range_name) - 1] = '\0';

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceTimeRange));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceSourceIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip by mac_ace_index
 * INPUT    : mac_ace_index, addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceSourceIpAddr(
    UI32_T mac_ace_index,
    UI32_T addr)
{
    BOOL_T  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace_entry)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace_entry.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace_entry.ipv4.sip.addr,
           &addr,
           sizeof(ace_entry.ipv4.sip.addr));

    if (0 == (ace_entry.ipv4.sip.mask[0] & ace_entry.ipv4.sip.mask[1] &
              ace_entry.ipv4.sip.mask[2] & ace_entry.ipv4.sip.mask[3]))
    {
        memset(ace_entry.ipv4.sip.mask, 0xff, sizeof(ace_entry.ipv4.sip.mask));
    }


    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index, &ace_entry,
        LEAF_diffServMacAceSourceIpAddr));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceSourceIpAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip mask by mac_ace_index
 * INPUT    : ip_ace_index, addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceSourceIpAddrBitmask(
    UI32_T mac_ace_index,
    UI32_T addr)
{
    BOOL_T  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace_entry)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace_entry.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace_entry.ipv4.sip.mask,
           &addr,
           sizeof(ace_entry.ipv4.sip.mask));

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index, &ace_entry,
        LEAF_diffServMacAceSourceIpAddrBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dest ip by mac_ace_index
 * INPUT    : mac_ace_index, addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceDestIpAddr(
    UI32_T mac_ace_index,
    UI32_T addr)
{
    BOOL_T  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace_entry)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace_entry.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace_entry.ipv4.dip.addr,
           &addr,
           sizeof(ace_entry.ipv4.dip.addr));

    if (0 == (ace_entry.ipv4.dip.mask[0] & ace_entry.ipv4.dip.mask[1] &
              ace_entry.ipv4.dip.mask[2] & ace_entry.ipv4.dip.mask[3]))
    {
        memset(ace_entry.ipv4.dip.mask, 0xff, sizeof(ace_entry.ipv4.dip.mask));
    }

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index, &ace_entry,
        LEAF_diffServMacAceDestIpAddr));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestIpAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dest ip mask by mac_ace_index
 * INPUT    : ip_ace_index, addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceDestIpAddrBitmask(
    UI32_T mac_ace_index,
    UI32_T addr)
{
    BOOL_T  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace_entry)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace_entry.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace_entry.ipv4.dip.mask,
           &addr,
           sizeof(ace_entry.ipv4.dip.mask));

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index, &ace_entry,
        LEAF_diffServMacAceDestIpAddrBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMaceAceSourceIpv6Addr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip by ipv6_ace_index
 * INPUT    : mac_ace_index, src_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMaceAceSourceIpv6Addr(
    UI32_T mac_ace_index,
    const UI8_T *src_ip)
{
    BOOL_T  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace_entry)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace_entry.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace_entry.ipv6.sip.addr, src_ip, SYS_ADPT_IPV6_ADDR_LEN);
    if (ace_entry.ipv6.sip.prefix_len == 0)
    {
        ace_entry.ipv6.sip.prefix_len = RULE_TYPE_MAX_IPV6_PREFIX_LEN;
    }
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index, &ace_entry,
        LEAF_diffServMacAceSourceIpv6Addr));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceSourceIpv6AddrPrefix
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip prefix length by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_ip_prefix
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceSourceIpv6AddrPrefix(
    UI32_T mac_ace_index,
    UI32_T src_ip_prefix)
{
    BOOL_T  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace_entry)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace_entry.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace_entry.ipv6.sip.prefix_len = src_ip_prefix;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index, &ace_entry,
        LEAF_diffServMacAceSourceIpv6AddrPrefixLen));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestIpv6Addr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip by mac_ace_index
 * INPUT    : ipv6_ace_index, dst_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceDestIpv6Addr(
    UI32_T mac_ace_index,
    const UI8_T *dst_ip)
{
    BOOL_T  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace_entry)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace_entry.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace_entry.ipv6.dip.addr, dst_ip, SYS_ADPT_IPV6_ADDR_LEN);
    if (ace_entry.ipv6.dip.prefix_len == 0)
    {
        ace_entry.ipv6.dip.prefix_len = RULE_TYPE_MAX_IPV6_PREFIX_LEN;
    }
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index, &ace_entry,
        LEAF_diffServMacAceDestIpv6Addr));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestIpv6AddrPrefix
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip prefix length by mac_ace_index
 * INPUT    : ipv6_ace_index, dst_ip_prefix
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceDestIpv6AddrPrefix(
    UI32_T mac_ace_index,
    UI32_T dst_ip_prefix)
{
    BOOL_T  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace_entry)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace_entry.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace_entry.ipv6.dip.prefix_len = dst_ip_prefix;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index, &ace_entry,
        LEAF_diffServMacAceDestIpv6AddrPrefixLen));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceProtocol
 *------------------------------------------------------------------------------
 * PURPOSE  : change protocol_type by mac_ace_index
 * INPUT    : mac_ace_index, protocol_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceProtocol(
    UI32_T mac_ace_index,
    UI32_T protocol_type)
{
    BOOL_T  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace_entry)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace_entry.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (ace_entry.ether.ethertype.op == VAL_diffServMacAceEtherTypeOp_equal &&
        ace_entry.ether.ethertype.u.s.mask == MAX_diffServMacAceEtherTypeBitmask)
    {
        switch (ace_entry.ether.ethertype.u.s.data)
        {
            case RULE_TYPE_ETHERTYPE_IP:
                ace_entry.ipv4.protocol.op = (protocol_type == RULE_TYPE_UNDEF_IP_PROTOCOL) ?
                   RULE_TYPE_IPV4_PROTOCOL_OP_NO_OPERATION : RULE_TYPE_IPV4_PROTOCOL_OP_EQUAL;
                ace_entry.ipv4.protocol.u.s.data = protocol_type;
                ace_entry.ipv4.protocol.u.s.mask = 255;
                break;
            case RULE_TYPE_ETHERTYPE_IPV6:
                ace_entry.ipv6.next_header.op = (protocol_type == RULE_TYPE_UNDEF_IP_PROTOCOL) ?
                    RULE_TYPE_IPV6_NEXT_HEADER_OP_NO_OPERATION: RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL;
                ace_entry.ipv6.next_header.u.s.data = protocol_type;
                ace_entry.ipv6.next_header.u.s.mask = 255;
                break;
            default:
                L4_MGR_UNLOCK();
                L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
                break;
        }
    }

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index, &ace_entry,
        LEAF_diffServMacAceIpProtocol));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceSourcePortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change source port operator by mac_ace_index
 * INPUT    : mac_ace_index, src_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceSourcePortOp(
    UI32_T mac_ace_index,
    UI32_T src_port_operator)
{
    BOOL_T                  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace_entry)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace_entry.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace_entry.l4_common.sport.op = src_port_operator;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index, &ace_entry,
        LEAF_diffServMacAceL4SourcePortOp));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceSourcePort
 *------------------------------------------------------------------------------
 * PURPOSE  : change src port by mac_ace_index
 * INPUT    : ip_ace_index, src_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceSourcePort(
    UI32_T mac_ace_index,
    UI32_T src_port)
{
    BOOL_T  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace_entry)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace_entry.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace_entry.l4_common.sport.u.s.data = src_port;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index, &ace_entry,
        LEAF_diffServMacAceL4SourcePort));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceSourcePortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src port mask by mac_ace_index
 * INPUT    : ip_ace_index, src_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceSourcePortBitmask(
    UI32_T mac_ace_index,
    UI32_T src_port_mask)
{
    BOOL_T  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace_entry)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace_entry.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace_entry.l4_common.sport.u.s.mask = src_port_mask;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index, &ace_entry,
        LEAF_diffServMacAceL4SourcePortBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestPortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port operator by mac_ace_index
 * INPUT    : ip_ace_index, dst_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceDestPortOp(
    UI32_T mac_ace_index,
    UI32_T dst_port_operator)
{
    BOOL_T                  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace_entry)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace_entry.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }


    ace_entry.l4_common.dport.op = dst_port_operator;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index, &ace_entry,
        LEAF_diffServMacAceL4DestPortOp));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestPort
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port by mac_ace_index
 * INPUT    : ip_ace_index, dst_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceDestPort(
    UI32_T mac_ace_index,
    UI32_T dst_port)
{
    BOOL_T  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace_entry)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace_entry.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace_entry.l4_common.dport.u.s.data = dst_port;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index, &ace_entry,
        LEAF_diffServMacAceL4DestPort));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestPortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port mask by mac_ace_index
 * INPUT    : ip_ace_index, dst_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceDestPortBitmask(
    UI32_T mac_ace_index,
    UI32_T dst_port_mask)
{
    BOOL_T  ret;
    RULE_TYPE_UI_Ace_Entry_T   ace_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace_entry)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace_entry.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace_entry.l4_common.dport.u.s.mask = dst_port_mask;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index, &ace_entry,
        LEAF_diffServMacAceL4DestPortBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServIpv6AceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get ui ace by ipv6_ace_index
 * INPUT    : ipv6_ace_index
 * OUTPUT   : ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetDiffServIpv6AceEntry(
    UI32_T ipv6_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if ((NULL == ace_p) ||
        (RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ipv6_ace_index, ace_p)) ||
        (FALSE == L4_MGR_IS_IPV6_ACL(ace_p->ace_type)))
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServIpv6AceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next ui ace by ipv6_ace_index
 * INPUT    : ipv6_ace_index
 * OUTPUT   : ipv6_ace_index, ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetNextDiffServIpv6AceEntry(
    UI32_T *ipv6_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if ((NULL == ipv6_ace_index) || (NULL == ace_p) ||
        (RULE_TYPE_OK != RULE_MGR_GetNextUIAceByAceTypeAndIndex(RULE_TYPE_IPV6_ACL,
                                                                ipv6_ace_index,
                                                                ace_p)))
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceType
 *------------------------------------------------------------------------------
 * PURPOSE  : set ace type by ipv6_ace_index
 * INPUT    : ip_ace_index, mib_ace_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceType(UI32_T ipv6_ace_index, UI32_T mib_ace_type)
{
    BOOL_T                  ret = TRUE;
    RULE_TYPE_AclType_T     core_ace_type = RULE_TYPE_IPV6_STD_ACL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    switch (mib_ace_type)
    {
    case VAL_diffServIpv6AceType_standard:
        core_ace_type = RULE_TYPE_IPV6_STD_ACL;
        break;

    case VAL_diffServIpv6AceType_extended:
        core_ace_type = RULE_TYPE_IPV6_EXT_ACL;
        break;

    default:
        ret = FALSE;
        break;
    }

    if (TRUE == ret)
    {
        ret = (RULE_TYPE_OK == RULE_MGR_SetAceTypeByIndex(ipv6_ace_index, core_ace_type));
    }

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceAccess
 *------------------------------------------------------------------------------
 * PURPOSE  : change access type by ipv6_ace_index
 * INPUT    : ipv6_ace_index, access_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceAccess(UI32_T ipv6_ace_index, UI32_T access_type)
{
    BOOL_T                   ret = TRUE;
    RULE_TYPE_UI_Ace_Entry_T ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ipv6_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IPV6_ACL(ace.ace_type)))
    {
        ret = FALSE;
    }

    if (TRUE == ret)
    {
        switch (access_type)
        {
        case VAL_diffServIpv6AceAccess_permit:
            ace.access = RULE_TYPE_ACE_PERMIT;
            break;

        case VAL_diffServIpv6AceAccess_deny:
            ace.access = RULE_TYPE_ACE_DENY;
            break;

        default:
            ret = FALSE;
            break;

        }
    }

    if (TRUE == ret)
    {
        ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ipv6_ace_index,
                                                             &ace,
                                                             LEAF_diffServIpv6AceAccess));
    }

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceSourceIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceSourceIpAddr(UI32_T ipv6_ace_index, const UI8_T *src_ip)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ipv6_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IPV6_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace.ipv6.sip.addr, src_ip, SYS_ADPT_IPV6_ADDR_LEN);
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ipv6_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpv6AceSourceIpAddr));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceSourceIpAddrPrefix
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip prefix length by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_ip_prefix
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceSourceIpAddrPrefix(UI32_T ipv6_ace_index, UI32_T src_ip_prefix)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ipv6_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IPV6_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ipv6.sip.prefix_len = (UI8_T)src_ip_prefix;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ipv6_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpv6AceSourceIpAddrPrefixLen));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceDestIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dst_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceDestIpAddr(UI32_T ipv6_ace_index, const UI8_T *dst_ip)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ipv6_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IPV6_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace.ipv6.dip.addr, dst_ip, SYS_ADPT_IPV6_ADDR_LEN);
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ipv6_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpv6AceDestIpAddr));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceDestIpAddrPrefix
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip prefix length by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dst_ip_prefix
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceDestIpAddrPrefix(UI32_T ipv6_ace_index, UI32_T dst_ip_prefix)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ipv6_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IPV6_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ipv6.dip.prefix_len = (UI8_T)dst_ip_prefix;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ipv6_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpv6AceDestIpAddrPrefixLen));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceNextHeader
 *------------------------------------------------------------------------------
 * PURPOSE  : change next header by ipv6_ace_index
 * INPUT    : ipv6_ace_index, next_header
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceNextHeader(UI32_T ipv6_ace_index, UI32_T next_header)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ipv6_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IPV6_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ipv6.next_header.op = (next_header == RULE_TYPE_UNDEF_IPV6_NEXT_HEADER) ?
                                  RULE_TYPE_IPV6_NEXT_HEADER_OP_NO_OPERATION :
                                  RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL;
    ace.ipv6.next_header.u.s.data = (UI8_T)next_header;
    ace.ipv6.next_header.u.s.mask = MAX_diffServIpv6AceNextHeader - 1;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ipv6_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpv6AceNextHeader));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change dscp by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceDscp(UI32_T ipv6_ace_index, UI32_T dscp)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ipv6_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IPV6_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ipv6.traffic_class.op = (dscp == RULE_TYPE_UNDEF_IPV6_DSCP) ?
                                    RULE_TYPE_IPV6_TRAFFIC_CLASS_OP_NO_OPERATION :
                                    RULE_TYPE_IPV6_TRAFFIC_CLASS_OP_EQUAL;
    ace.ipv6.traffic_class.u.s.data = dscp << 2;
    ace.ipv6.traffic_class.u.s.mask = (MAX_diffServIpv6AceDscp - 1) << 2;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ipv6_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpv6AceDscp));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceFlowLabel
 *------------------------------------------------------------------------------
 * PURPOSE  : change flow label by ipv6_ace_index
 * INPUT    : ipv6_ace_index, flow_label
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceFlowLabel(UI32_T ipv6_ace_index, UI32_T flow_label)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ipv6_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IPV6_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ipv6.flow_label.op = (flow_label == RULE_TYPE_UNDEF_IPV6_FLOW_LABEL) ?
                                 RULE_TYPE_IPV6_FLOW_LABEL_OP_NO_OPERATION :
                                 RULE_TYPE_IPV6_FLOW_LABEL_OP_EQUAL;
    ace.ipv6.flow_label.u.s.data = flow_label;
    ace.ipv6.flow_label.u.s.mask = MAX_diffServIpv6AceFlowLabel - 1;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ipv6_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpv6AceFlowLabel));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change row status by ipv6_ace_index
 * INPUT    : ipv6_ace_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceStatus(UI32_T ipv6_ace_index, UI32_T row_status)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetAceRowStatus(ipv6_ace_index, RULE_TYPE_IPV6_ACL, row_status));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceTimeRange
 *------------------------------------------------------------------------------
 * PURPOSE  : change ace time_range by ipv6_ace_index
 * INPUT    : ipv6_ace_index, time_range_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServIpv6AceTimeRange(
    UI32_T ipv6_ace_index,
    UI8_T *time_range_name)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ipv6_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IPV6_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    strncpy((char *)ace.time_range_name, (char *)time_range_name, sizeof(ace.time_range_name) - 1);
    ace.time_range_name[sizeof(ace.time_range_name) - 1] = '\0';

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ipv6_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpv6AceTimeRange));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);}
#endif /* SYS_CPNT_TIME_BASED_ACL */

#if (SYS_CPNT_ACL_IPV6_EXT_TCP_UDP_PORT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceSourcePortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change source port operator by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServIpv6AceSourcePortOp(
    UI32_T ipv6_ace_index,
    UI32_T src_port_operator)
{
    BOOL_T                  ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ipv6_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IPV6_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.l4_common.sport.op = src_port_operator;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ipv6_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpv6AceSourcePortOp));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceSourcePort
 *------------------------------------------------------------------------------
 * PURPOSE  : change src port by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServIpv6AceSourcePort(
    UI32_T ipv6_ace_index,
    UI32_T src_port)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ipv6_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IPV6_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.l4_common.sport.u.s.data = src_port;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ipv6_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpv6AceSourcePort));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceSourcePortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src port mask by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServIpv6AceSourcePortBitmask(
    UI32_T ipv6_ace_index,
    UI32_T src_port_mask)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ipv6_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IPV6_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.l4_common.sport.u.s.mask = src_port_mask;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ipv6_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpv6AceSourcePortBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceDestPortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port operator by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dst_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServIpv6AceDestPortOp(
    UI32_T ipv6_ace_index,
    UI32_T dst_port_operator)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ipv6_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IPV6_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.l4_common.dport.op = dst_port_operator;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ipv6_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpv6AceDestPortOp));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceDestPort
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dst_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServIpv6AceDestPort(
    UI32_T ipv6_ace_index,
    UI32_T dst_port)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;


    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ipv6_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IPV6_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.l4_common.dport.u.s.data = dst_port;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ipv6_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpv6AceDestPort));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceDestPortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port mask by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dst_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServIpv6AceDestPortBitmask(
    UI32_T ipv6_ace_index,
    UI32_T dst_port_mask)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(ipv6_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_IPV6_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.l4_common.dport.u.s.mask = dst_port_mask;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(ipv6_ace_index,
                                                         &ace,
                                                         LEAF_diffServIpv6AceDestPortBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}
#endif /* SYS_CPNT_ACL_IPV6_EXT_TCP_UDP_PORT */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get action entry by action_index
 * INPUT    : action_index
 * OUTPUT   : action_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetDiffServActionEntry(UI32_T action_index, RULE_TYPE_MIB_Action_T *action_entry)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    ret = (RULE_TYPE_OK == RULE_MGR_GetMibActionEntryByIndex(action_index, action_entry));

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next action entry by action_index
 * INPUT    : action_index
 * OUTPUT   : action_index, action_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetNextDiffServActionEntry(UI32_T *action_index, RULE_TYPE_MIB_Action_T *action_entry)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    ret = (RULE_TYPE_OK == RULE_MGR_GetNextMibActionEntry(action_index, action_entry));

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionBitmap
 *------------------------------------------------------------------------------
 * PURPOSE  : change action bitmap by action_index
 * INPUT    : action_index, action_bitmap
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionBitmap(UI32_T action_index, const UI8_T *action_bitmap)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    ret = (RULE_TYPE_OK == RULE_MGR_SetActionBitmap(action_index, action_bitmap));

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionPktNewPri
 *------------------------------------------------------------------------------
 * PURPOSE  : change (green) packet new priority by action_index
 * INPUT    : action_index, pkt_new_pri
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionPktNewPri(UI32_T action_index, UI32_T pkt_new_pri)
{
    BOOL_T              ret;
    RULE_TYPE_Action_T  action_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetActionEntryByIndex(action_index, &action_entry))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (0 == (RULE_TYPE_ACTION_PKT_NEW_PRI & action_entry.action_bitmap))
    {
        /* if action is not enabled, do not allow to confiure this leaf value */
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    action_entry.pkt_new_pri = pkt_new_pri;
    ret = (RULE_TYPE_OK == RULE_MGR_SetActionEntry(action_index, &action_entry));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

BOOL_T L4_MGR_QoS_SetDiffServActionPktNewPhb(UI32_T action_index, UI32_T pkt_new_phb)
{
    BOOL_T              ret;
    RULE_TYPE_Action_T  action_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetActionEntryByIndex(action_index, &action_entry))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (0 == (RULE_TYPE_ACTION_PKT_NEW_PHB & action_entry.action_bitmap))
    {
        /* if action is not enabled, do not allow to confiure this leaf value */
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    action_entry.pkt_new_phb = pkt_new_phb;
    ret = (RULE_TYPE_OK == RULE_MGR_SetActionEntry(action_index, &action_entry));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionPktNewIpPrec
 *------------------------------------------------------------------------------
 * PURPOSE  : change (green) packet new ip precedence by action_index
 * INPUT    : action_index, pkt_new_ip_prec
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionPktNewIpPrec(UI32_T action_index, UI32_T pkt_new_ip_prec)
{
    BOOL_T              ret;
    RULE_TYPE_Action_T  action_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetActionEntryByIndex(action_index, &action_entry))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (0 == (RULE_TYPE_ACTION_PKT_NEW_TOS & action_entry.action_bitmap))
    {
        /* if action is not enabled, do not allow to confiure this leaf value */
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    action_entry.pkt_new_tos = pkt_new_ip_prec;
    ret = (RULE_TYPE_OK == RULE_MGR_SetActionEntry(action_index, &action_entry));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionPktNewDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change (in-profile) packet new dscp by action_index
 * INPUT    : action_index, pkt_new_dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionPktNewDscp(UI32_T action_index, UI32_T pkt_new_dscp)
{
    BOOL_T              ret;
    RULE_TYPE_Action_T  action_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetActionEntryByIndex(action_index, &action_entry))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (0 == (RULE_TYPE_ACTION_PKT_NEW_DSCP & action_entry.action_bitmap))
    {
        /* if action is not enabled, do not allow to confiure this leaf value */
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    action_entry.pkt_new_dscp = pkt_new_dscp;
    ret = (RULE_TYPE_OK == RULE_MGR_SetActionEntry(action_index, &action_entry));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionPktNewDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change (green) packet new dscp by action_index
 * INPUT    : action_index, pkt_new_dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionGreenPktNewDscp(UI32_T action_index, UI32_T pkt_new_dscp)
{
    BOOL_T              ret;
    RULE_TYPE_Action_T  action_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetActionEntryByIndex(action_index, &action_entry))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (0 == (RULE_TYPE_ACTION_GREEN_PKT_NEW_DSCP & action_entry.action_bitmap))
    {
        /* if action is not enabled, do not allow to confiure this leaf value */
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    action_entry.green_pkt_new_dscp = pkt_new_dscp;
    ret = (RULE_TYPE_OK == RULE_MGR_SetActionEntry(action_index, &action_entry));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionRedPktNewDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change (red) packet new dscp by action_index
 * INPUT    : action_index, pkt_new_dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionRedPktNewDscp(UI32_T action_index, UI32_T pkt_new_dscp)
{
    BOOL_T              ret;
    RULE_TYPE_Action_T  action_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetActionEntryByIndex(action_index, &action_entry))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (0 == (RULE_TYPE_ACTION_RED_PKT_NEW_DSCP & action_entry.action_bitmap))
    {
        /* if action is not enabled, do not allow to confiure this leaf value */
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    action_entry.red_pkt_new_dscp = pkt_new_dscp;
    ret = (RULE_TYPE_OK == RULE_MGR_SetActionEntry(action_index, &action_entry));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionYellowPktNewDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change (yellow) packet new dscp by action_index
 * INPUT    : action_index, pkt_new_dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionYellowPktNewDscp(UI32_T action_index, UI32_T pkt_new_dscp)
{
    BOOL_T              ret;
    RULE_TYPE_Action_T  action_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetActionEntryByIndex(action_index, &action_entry))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (0 == (RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP & action_entry.action_bitmap))
    {
        /* if action is not enabled, do not allow to confiure this leaf value */
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    action_entry.yellow_pkt_new_dscp = pkt_new_dscp;
    ret = (RULE_TYPE_OK == RULE_MGR_SetActionEntry(action_index, &action_entry));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionYellowTransmit
 *------------------------------------------------------------------------------
 * PURPOSE  : change the action of (green) packet by action_index
 * INPUT    : action_index, transmit
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionGreenTransmit(UI32_T action_index, UI32_T transmit)
{
    BOOL_T              ret;
    RULE_TYPE_Action_T  action_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetActionEntryByIndex(action_index, &action_entry))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if(transmit==2)
        action_entry.action_bitmap |= RULE_TYPE_ACTION_DROP;
    else
        action_entry.action_bitmap &= ~RULE_TYPE_ACTION_DROP;

    ret = (RULE_TYPE_OK == RULE_MGR_SetActionEntry(action_index, &action_entry));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionYellowDrop
 *------------------------------------------------------------------------------
 * PURPOSE  : change the action of (yellow) packet by action_index
 * INPUT    : action_index, yellow_drop
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionYellowDrop(UI32_T action_index, UI32_T yellow_drop)
{
    BOOL_T              ret;
    RULE_TYPE_Action_T  action_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetActionEntryByIndex(action_index, &action_entry))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if(yellow_drop==1)
        action_entry.action_bitmap |= RULE_TYPE_ACTION_YELLOW_DROP;
    else
        action_entry.action_bitmap &= ~RULE_TYPE_ACTION_YELLOW_DROP;
    ret = (RULE_TYPE_OK == RULE_MGR_SetActionEntry(action_index, &action_entry));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionRedDrop
 *------------------------------------------------------------------------------
 * PURPOSE  : change the action of (red) packet by action_index
 * INPUT    : action_index, red_drop
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionRedDrop(UI32_T action_index, UI32_T red_drop)
{
    BOOL_T              ret;
    RULE_TYPE_Action_T  action_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetActionEntryByIndex(action_index, &action_entry))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if(red_drop==1)
        action_entry.action_bitmap |= RULE_TYPE_ACTION_RED_DROP;
    else
        action_entry.action_bitmap &= ~RULE_TYPE_ACTION_RED_DROP;
    ret = (RULE_TYPE_OK == RULE_MGR_SetActionEntry(action_index, &action_entry));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionPktPri
 *------------------------------------------------------------------------------
 * PURPOSE  : Change the packet priority for bundle group
 * INPUT    : action_index, priority
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionPktPri(UI32_T action_index, UI32_T priority)
{
    BOOL_T              ret;
    RULE_TYPE_Action_T  action_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetActionEntryByIndex(action_index, &action_entry))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (0 == (RULE_TYPE_ACTION_PKT_PRI & action_entry.action_bitmap))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    action_entry.pkt_pri = priority;

    ret = (RULE_TYPE_OK == RULE_MGR_SetActionEntry(action_index, &action_entry));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionDropPrecedence
 *------------------------------------------------------------------------------
 * PURPOSE  : change (green) drop precedence by action_index
 * INPUT    : action_index, drop_precedence
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionDropPrecedence(UI32_T action_index, UI32_T drop_precedence)
{
    BOOL_T              ret;
    RULE_TYPE_Action_T  action_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetActionEntryByIndex(action_index, &action_entry))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (0 == (RULE_TYPE_ACTION_DROP_PRECEDENCE & action_entry.action_bitmap))
    {
        /* if action is not enabled, do not allow to confiure this leaf value */
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    action_entry.drop_precedence = drop_precedence;
    ret = (RULE_TYPE_OK == RULE_MGR_SetActionEntry(action_index, &action_entry));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionRedDropPrecedence
 *------------------------------------------------------------------------------
 * PURPOSE  : change (red) drop precedence by action_index
 * INPUT    : action_index, drop_precedence
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionRedDropPrecedence(UI32_T action_index, UI32_T drop_precedence)
{
    BOOL_T              ret;
    RULE_TYPE_Action_T  action_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetActionEntryByIndex(action_index, &action_entry))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (0 == (RULE_TYPE_ACTION_RED_DROP_PRECEDENCE & action_entry.action_bitmap))
    {
        /* if action is not enabled, do not allow to confiure this leaf value */
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    action_entry.red_drop_precedence = drop_precedence;
    ret = (RULE_TYPE_OK == RULE_MGR_SetActionEntry(action_index, &action_entry));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionYellowDropPrecedence
 *------------------------------------------------------------------------------
 * PURPOSE  : change (yellow) drop precedence by action_index
 * INPUT    : action_index, drop_precedence
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionYellowDropPrecedence(UI32_T action_index, UI32_T drop_precedence)
{
    BOOL_T              ret;
    RULE_TYPE_Action_T  action_entry;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetActionEntryByIndex(action_index, &action_entry))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (0 == (RULE_TYPE_ACTION_YELLOW_DROP_PRECEDENCE & action_entry.action_bitmap))
    {
        /* if action is not enabled, do not allow to confiure this leaf value */
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    action_entry.yellow_drop_precedence = drop_precedence;
    ret = (RULE_TYPE_OK == RULE_MGR_SetActionEntry(action_index, &action_entry));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change action row_status by action_index
 * INPUT    : action_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionStatus(UI32_T action_index, UI32_T row_status)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetActionRowStatus(action_index, row_status));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServMeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get meter entry by meter_index
 * INPUT    : meter_index
 * OUTPUT   : meter_param
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetDiffServMeterEntry(UI32_T meter_index, RULE_TYPE_TBParamEntry_T *meter_param)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    ret = (RULE_TYPE_OK == RULE_MGR_GetMeterEntryByIndex(meter_index, meter_param));

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServMeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next meter entry by meter_index
 * INPUT    : meter_index
 * OUTPUT   : meter_index, meter_param
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetNextDiffServMeterEntry(UI32_T *meter_index,RULE_TYPE_TBParamEntry_T *meter_param)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    ret = (RULE_TYPE_OK == RULE_MGR_GetNextMeterEntry(meter_index, meter_param));

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMeterModel
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter model by meter_index
 * INPUT    : meter_index, meter_model
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMeterModel(UI32_T meter_index, UI32_T meter_model)
{
    BOOL_T                      ret;
    RULE_TYPE_TBParamEntry_T    meter_param;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetMeterEntryByIndex(meter_index, &meter_param))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    meter_param.meter_model = meter_model;
    ret = (RULE_TYPE_OK == RULE_MGR_SetMeterEntry(meter_index, &meter_param));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMeterRate
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter rate by meter_index
 * INPUT    : meter_index, meter_rate
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMeterRate(UI32_T meter_index, UI32_T meter_rate)
{
    BOOL_T                      ret;
    RULE_TYPE_TBParamEntry_T    meter_param;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetMeterEntryByIndex(meter_index, &meter_param))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    meter_param.rate = meter_rate;
    ret = (RULE_TYPE_OK == RULE_MGR_SetMeterEntry(meter_index, &meter_param));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}


/*zhangwei add */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMeterPeakRate
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter rate by meter_index
 * INPUT    : meter_index, meter_rate
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMeterPeakRate(UI32_T meter_index, UI32_T meter_rate)
{
    BOOL_T                      ret;
    RULE_TYPE_TBParamEntry_T    meter_param;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetMeterEntryByIndex(meter_index, &meter_param))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    meter_param.peak_rate = meter_rate;
    ret = (RULE_TYPE_OK == RULE_MGR_SetMeterEntry(meter_index, &meter_param));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMeterBurstSize
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter burst size by meter_index
 * INPUT    : meter_index, burst_size
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMeterBurstSize(UI32_T meter_index, UI32_T burst_size)
{
    BOOL_T                      ret;
    RULE_TYPE_TBParamEntry_T    meter_param;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetMeterEntryByIndex(meter_index, &meter_param))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    meter_param.burst_size = burst_size;
    ret = (RULE_TYPE_OK == RULE_MGR_SetMeterEntry(meter_index, &meter_param));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*zhangwei add*/


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMeterPeakBurstSize
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter burst size by meter_index
 * INPUT    : meter_index, burst_size
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMeterPeakBurstSize(UI32_T meter_index, UI32_T burst_size)
{
    BOOL_T                      ret;
    RULE_TYPE_TBParamEntry_T    meter_param;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetMeterEntryByIndex(meter_index, &meter_param))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    meter_param.peak_burst_size = burst_size;
    ret = (RULE_TYPE_OK == RULE_MGR_SetMeterEntry(meter_index, &meter_param));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}






/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMeterInterval
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter update interval by meter_index
 * INPUT    : meter_index, meter_interval
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMeterInterval(UI32_T meter_index, UI32_T meter_interval)
{
    BOOL_T                      ret;
    RULE_TYPE_TBParamEntry_T    meter_param;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_OK != RULE_MGR_GetMeterEntryByIndex(meter_index, &meter_param))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    meter_param.interval = meter_interval;
    ret = (RULE_TYPE_OK == RULE_MGR_SetMeterEntry(meter_index, &meter_param));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServAclHwCounterEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next hardware ACE from ACL.
 * INPUT:    ifindex, direction, acl_index, ace_index
 * OUTPUT:   ace_entry_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetDiffServAclHwCounterEntry(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_index,
    UI32_T ace_index,
    RULE_TYPE_Ace_Entry_T *ace_entry_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    ASSERT(NULL != ace_entry_p);

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();

    result = RULE_MGR_GetHardwareAceByAcl(ifindex, direction, acl_index,
                                          ace_index, ace_entry_p);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(result);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServAclHwCounterEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next hardware ACE from ACL.
 * INPUT:    ifindex_p, direction_p, acl_index_p, ace_index, ace_entry_p
 * OUTPUT:   ifindex_p, direction_p, acl_index_p, ace_index, ace_entry_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetNextDiffServAclHwCounterEntry(
    UI32_T *ifindex_p,
    RULE_TYPE_InOutDirection_T *direction_p,
    UI32_T *acl_index_p,
    UI32_T *ace_index_p,
    RULE_TYPE_Ace_Entry_T *ace_entry_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T pl_idx;

    ASSERT(NULL != ifindex_p);
    ASSERT(NULL != direction_p);
    ASSERT(NULL != acl_index_p);
    ASSERT(NULL != ace_index_p);
    ASSERT(NULL != ace_entry_p);

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();

    if (0 == *ifindex_p)
    {
        pl_idx = 0;
        *direction_p = RULE_TYPE_INBOUND;
        *acl_index_p = 0;
        *ace_index_p = 0;
    }
    else
    {
        pl_idx = RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(*ifindex_p);
    }

    {
        RULE_TYPE_AclType_T ace_type;

        result = RULE_MGR_GetNextHardwareAceByAcl(*ifindex_p, *direction_p, *acl_index_p,
                                                  &ace_type, ace_index_p, ace_entry_p);
        if (RULE_TYPE_OK == result)
        {
            L4_MGR_UNLOCK();
            L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_OK);
        }
    }

get_for_egress_direction:
    for (; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        RULE_TYPE_UI_AclEntry_T acl_entry;
        UI32_T precedence = RULE_TYPE_PRECEDENCE_FIRST;
        RULE_TYPE_COUNTER_ENABLE_T counter_enable;

        *ifindex_p = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        /* Convert ACL type to precedence
         */
        {
            RULE_TYPE_Acl_T tmp_acl_entry;
            result = RULE_MGR_GetAclByIndex(*acl_index_p, &tmp_acl_entry);
            if (RULE_TYPE_OK == result)
            {
                switch (tmp_acl_entry.acl_type)
                {
                    case RULE_TYPE_MAC_ACL:
                        precedence = (RULE_TYPE_INBOUND == *direction_p) ? INGRESS_MAC_ACL_PRECEDENCE : EGRESS_MAC_ACL_PRECEDENCE;
                        break;
                    case RULE_TYPE_IP_STD_ACL:
                    case RULE_TYPE_IP_EXT_ACL:
                        precedence = (RULE_TYPE_INBOUND == *direction_p) ? INGRESS_IP_ACL_PRECEDENCE : EGRESS_IP_ACL_PRECEDENCE;
                        break;
                    case RULE_TYPE_IPV6_STD_ACL:
                    case RULE_TYPE_IPV6_EXT_ACL:
                        precedence = (RULE_TYPE_INBOUND == *direction_p) ? INGRESS_IPV6_ACL_PRECEDENCE : EGRESS_IPV6_ACL_PRECEDENCE;
                        break;
                    default:
                        ASSERT(0);
                        break;
                }
            }
        }

        while (RULE_TYPE_OK == RULE_MGR_GetNextAclByPort(*ifindex_p, *direction_p,
                                                         acl_index_p, &acl_entry, &precedence,
                                                         NULL, &counter_enable))
        {
            if (RULE_TYPE_COUNTER_ENABLE != counter_enable)
            {
                continue;
            }

            /* Get first ACE
             */
            {
                RULE_TYPE_AclType_T ace_type;

                *ace_index_p = 0;
                result = RULE_MGR_GetNextHardwareAceByAcl(*ifindex_p, *direction_p, *acl_index_p,
                                                          &ace_type, ace_index_p, ace_entry_p);
                if (RULE_TYPE_OK != result)
                {
                    continue;
                }
            }

            L4_MGR_UNLOCK();
            L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_OK);
        }

        *acl_index_p = 0;
        *ace_index_p = 0;
    }

    if (RULE_TYPE_INBOUND == *direction_p)
    {
        pl_idx = 0;
        *direction_p = RULE_TYPE_OUTBOUND;
        *acl_index_p = 0;
        *ace_index_p = 0;
        goto get_for_egress_direction;
    }

    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetAccessGroupAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Set aclIndex of AccessGroupTable
 * INPUT:    ifindex, mib_direction, mib_acl_type
 *           acl_index
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_SetAccessGroupAclIndex(
    UI32_T ifindex,
    UI32_T mib_direction,
    UI32_T mib_acl_type,
    UI32_T acl_index)
{
    RULE_TYPE_RETURN_TYPE_T ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    ret = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetAccessGroupTimeRangeName
 *------------------------------------------------------------------------------
 * PURPOSE:  Set timeRangeName of AccessGroupTable
 * INPUT:    ifindex, mib_direction, mib_acl_type
 *           time_range_name
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_SetAccessGroupTimeRangeName(
    UI32_T ifindex,
    UI32_T mib_direction,
    UI32_T mib_acl_type,
    char   *time_range_name)
{
    RULE_TYPE_RETURN_TYPE_T ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    ret = RULE_MGR_SetAccessGroupTimeRangeName(ifindex, mib_direction, mib_acl_type, time_range_name);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetAccessGroupCounterStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  Set counter_status of AccessGroupTable
 * INPUT:    ifindex, mib_direction, mib_acl_type
 *           counter_status
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_SetAccessGroupCounterStatus(
    UI32_T ifindex,
    UI32_T mib_direction,
    UI32_T mib_acl_type,
    UI32_T counter_status)
{
    RULE_TYPE_RETURN_TYPE_T ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    ret = RULE_MGR_SetAccessGroupCounterStatus(ifindex, mib_direction, mib_acl_type, counter_status);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetAccessGroupRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  Set status of AccessGroupTable
 * INPUT:    ifindex, mib_direction, mib_acl_type
 *           row_status
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_SetAccessGroupRowStatus(
    UI32_T ifindex,
    UI32_T mib_direction,
    UI32_T mib_acl_type,
    UI32_T row_status)
{
    RULE_TYPE_RETURN_TYPE_T ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    ret = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, row_status);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetAccessGroupEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get AccessGroupTable
 * INPUT:    entry_p->ifindex, entry->direction, entry->type
 * OUTPUT:   entry_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetAccessGroupEntry(
    RULE_TYPE_MIB_AccessGroupEntry_T *entry_p)
{
    RULE_TYPE_RETURN_TYPE_T ret;

    ASSERT(entry_p != NULL);

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    ret = RULE_MGR_GetAccessGroupEntry(entry_p);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextAccessGroupEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get next AccessGroupTable
 * INPUT:    entry_p->ifindex, entry->direction, entry->type
 * OUTPUT:   entry_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Using entry_p->ifindex = 0 to get first entry
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetNextAccessGroupEntry(
    RULE_TYPE_MIB_AccessGroupEntry_T *entry_p)
{
    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    while (RULE_TYPE_OK == RULE_MGR_GetNextAccessGroupEntry(entry_p))
    {
        if (TRUE == SWCTRL_LogicalPortExisting(entry_p->ifindex))
        {
            L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_OK);
        }

        if (RULE_TYPE_IS_ALL_PORTS(entry_p->ifindex) ||
            RULE_TYPE_IS_CPU_PORT(entry_p->ifindex))
        {
            L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_OK);
        }
    }

    L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetPolicyMapPortPolcyMapIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Set policy map index of diffservPolicyMapPortTable
 * INPUT:    ifindex, mib_direction
 *           policy_map_index
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_SetPolicyMapPortPolcyMapIndex(
   UI32_T ifindex,
   UI32_T mib_direction,
   UI32_T policy_map_index)
{
    RULE_TYPE_RETURN_TYPE_T ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    ret = RULE_MGR_SetPolicyMapPortPolcyMapIndex(ifindex, mib_direction, policy_map_index);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetPolicyMapPortRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  Set status of diffservPolicyMapPortTable
 * INPUT:    ifindex, mib_direction
 *           row_status
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_SetPolicyMapPortRowStatus(
   UI32_T ifindex,
   UI32_T mib_direction,
   UI32_T row_status)
{
    RULE_TYPE_RETURN_TYPE_T ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    ret = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, row_status);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetPolicyMapPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get diffservPolicyMapPortTable
 * INPUT:    entry_p->ifindex, entry->direction, entry->type
 * OUTPUT:   entry_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetPolicyMapPortEntry(
 RULE_TYPE_MIB_PolicyMapPortEntry_T *entry_p)
{
    ASSERT(entry_p != NULL);

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if (TRUE == SWCTRL_LogicalPortExisting(entry_p->ifindex) ||
        TRUE == RULE_TYPE_IS_CPU_PORT(entry_p->ifindex) ||
        TRUE == RULE_TYPE_IS_ALL_PORTS(entry_p->ifindex))
    {
        return  RULE_MGR_GetPolicyMapPortEntry(entry_p);
    }

    return RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextPolicyMapPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get next diffservPolicyMapPortTable
 * INPUT:    entry_p->ifindex, entry->direction, entry->type
 * OUTPUT:   entry_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetNextPolicyMapPortEntry(
    RULE_TYPE_MIB_PolicyMapPortEntry_T *entry_p)
{
    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    while (RULE_TYPE_OK == RULE_MGR_GetNextPolicyMapPortEntry(entry_p))
    {
        if (TRUE == SWCTRL_LogicalPortExisting(entry_p->ifindex))
        {
            L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_OK);
        }

        if (RULE_TYPE_IS_ALL_PORTS(entry_p->ifindex) ||
            RULE_TYPE_IS_CPU_PORT(entry_p->ifindex))
        {
            L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_OK);
        }
    }

    L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMeterStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter row_status by meter_index
 * INPUT    : meter_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMeterStatus(UI32_T meter_index, UI32_T row_status)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetMeterRowStatus(meter_index, row_status));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServPolicyMapHwCounter
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
UI32_T L4_MGR_QoS_GetDiffServPolicyMapHwCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_id,
    UI32_T class_id,
    RULE_TYPE_CounterRec_T *counter)
{
    UI32_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = RULE_MGR_GetQosCounter(ifindex, direction, policy_id, class_id, counter);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_ClearDiffServPolicyMapHwCounter
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
UI32_T L4_MGR_QoS_ClearDiffServPolicyMapHwCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_id,
    UI32_T class_id)
{
    UI32_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = RULE_MGR_ClearQosCounter(ifindex, direction, policy_id, class_id);
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}
#endif /*#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)*/

#if (SYS_CPNT_DAI == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_SetArpPktActionByDai
 *------------------------------------------------------------------------------
 * PURPOSE:  for ARP INSPETCION (DAI)
 *            config chip to trap ARP(BC+REPLY) packets to CPU
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_SetArpPktActionByDai()
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_AddTrapArp2Cpu(TRUE));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_RecoverArpPktActionByDai
 *------------------------------------------------------------------------------
 * PURPOSE:  for ARP INSPETCION (DAI)
 *            config chip to recover the behavior of ARP(BC+REPLY) packets to CPU
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_RecoverArpPktActionByDai()
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_AddTrapArp2Cpu(FALSE));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ArpACLCheckForDai
 *------------------------------------------------------------------------------
 * PURPOSE:  for ARP INSPETCION (DAI)
 *            to check if match the existed arp rules with arp packets
 * INPUT:    arp_acl_name - check which arp acl
 *           arp_pkt_p    - compared arp packet
 * OUTPUT:   b_acl_drop   - is drop
 * RETURN:   TRUE/FALSE
 * NOTE:     TRUE means match rule; FALSE means no match rule
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_ArpACLCheckForDai(const char *arp_acl_name, DAI_TYPE_ArpPktFormat_T *arp_pkt_p, BOOL_T *b_acl_drop)
{

    BOOL_T  ret;
    RULE_TYPE_ArpPktFormat_T  tar_arp_pkt;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if (DAI_TYPE_ARP_OP_REQUEST == arp_pkt_p->opcode)
    {
        tar_arp_pkt.opcode = RULE_TYPE_ARP_REQUEST;
    }
    else
    {
        tar_arp_pkt.opcode = RULE_TYPE_ARP_REPLY;
    }
    tar_arp_pkt.srcIp = arp_pkt_p->srcIp;
    tar_arp_pkt.dstIp = arp_pkt_p->dstIp;
    memcpy(tar_arp_pkt.srcMacAddr, arp_pkt_p->srcMacAddr, SYS_ADPT_MAC_ADDR_LEN);
    memcpy(tar_arp_pkt.dstMacAddr, arp_pkt_p->dstMacAddr, SYS_ADPT_MAC_ADDR_LEN);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_ArpACLCheckForDai(arp_acl_name, &tar_arp_pkt, b_acl_drop));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}
#endif /*end of #if (SYS_CPNT_DAI == TRUE)*/

#if (SYS_CPNT_IP_SOURCE_GUARD == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetSrcIpFilterByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : for IP Source Guard
 *            config chip to permit switching traffic which contain
 *            specified Source IP
 * INPUT    : lport, sip, vid
 *            action        -- could be:
 *                          0   L4_MGR_DO_ACTION_DELETE_RULE
 *                          1   L4_MGR_DO_ACTION_PERMIT
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_FREE_SELECTOR_UNAVAILABLE/
 *            RULE_TYPE_OK/
 *            RULE_TYPE_FAIL
 * NOTE     : 1, this API is common interface to IP Source Guard
 *               internally it call different Driver API (hrdrv, rule_ctrl... etc.)
 *            2, if chip is already configured, update/delete config from chip
 *               if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_SetSrcIpFilterByLport(UI32_T lport, UI32_T sip, UI32_T vid, UI32_T action)
{
    BOOL_T ret;

    L4_MGR_CHECK_LPORT(lport, RULE_TYPE_FAIL);

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    ret = RULE_MGR_SetSrcIpFilterByUport(
        STKTPLG_OM_IFINDEX_TO_UNIT(lport),
        STKTPLG_OM_IFINDEX_TO_PORT(lport),
        sip,
        vid,
        (action == L4_MGR_DO_ACTION_PERMIT) ? TRUE : FALSE
        );

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret? RULE_TYPE_OK : RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetSrcMacSrcIpFilterByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : for IP Source Guard
 *            config chip to permit switching traffic which contain
 *            specified Source IP and Source MAC
 * INPUT    : lport, src_mac, sip, vid
 *            action        -- could be:
 *                          0   L4_MGR_DO_ACTION_DELETE_RULE
 *                          1   L4_MGR_DO_ACTION_PERMIT
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_FREE_SELECTOR_UNAVAILABLE/
 *            RULE_TYPE_OK/
 *            RULE_TYPE_FAIL
 * NOTE     : 1, this API is common interface to IP Source Guard
 *               internally it call different Driver API (hrdrv, rule_ctrl... etc.)
 *            2, if chip is already configured, update/delete config from chip
 *               if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_SetSrcMacSrcIpFilterByLport(UI32_T lport, const UI8_T *src_mac, UI32_T sip, UI32_T vid, UI32_T action)
{
    BOOL_T ret;

    L4_MGR_CHECK_LPORT(lport, RULE_TYPE_FAIL);

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    ret = RULE_MGR_SetSrcMacSrcIpFilterByUport(
        STKTPLG_OM_IFINDEX_TO_UNIT(lport),
        STKTPLG_OM_IFINDEX_TO_PORT(lport),
        src_mac,
        sip,
        vid,
        (action == L4_MGR_DO_ACTION_PERMIT) ? TRUE : FALSE
        );

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret ? RULE_TYPE_OK : RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetIpPktActionByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : for IP Source Guard
 *            config chip to drop IP packets
 * INPUT    : lport, src_mac, sip
 *            action        -- could be:
 *                          0   L4_MGR_DO_ACTION_DELETE_RULE
 *                          4   L4_MGR_DO_ACTION_DROP
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_FREE_SELECTOR_UNAVAILABLE/
 *            RULE_TYPE_OK/
 *            RULE_TYPE_FAIL
 * NOTE     : 1, this API is common interface to IP Source Guard
 *               internally it call different Driver API (hrdrv, rule_ctrl... etc.)
 *            2, drop action in this API has lowest priority. That is,
 *               L4_MGR_SetDhcpClientPktActionByLport(...),
 *               L4_MGR_SetDhcpServerPktActionByLport(...),
 *               L4_MGR_SetSrcIpFilterByLport(...),
 *               L4_MGR_SetSrcMacSrcIpFilterByLport(...)
 *               dominate some IP packets do not be drop
 *            3, if chip is already configured, update/delete config from chip
 *               if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_SetIpPktActionByLport(UI32_T lport, UI32_T action)
{
    UI32_T ret;

    L4_MGR_CHECK_LPORT(lport, RULE_TYPE_FAIL);

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    ret = RULE_MGR_SetIpPktActionByUport(
        STKTPLG_OM_IFINDEX_TO_UNIT(lport),
        STKTPLG_OM_IFINDEX_TO_PORT(lport),
        (action==L4_MGR_DO_ACTION_DROP)?TRUE:FALSE
        );

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret ? RULE_TYPE_OK : RULE_TYPE_FAIL);
}
#endif /* #if (SYS_CPNT_IP_SOURCE_GUARD == TRUE) */

#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetSrcIpv6FilterByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : for IPv6 Source Guard
 *            config chip to permit switching traffic which contain
 *            specified Source IPv6
 * INPUT    : lport, sip6, vid
 *            action        -- could be:
 *                          0   L4_MGR_DO_ACTION_DELETE_RULE
 *                          1   L4_MGR_DO_ACTION_PERMIT
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_FREE_SELECTOR_UNAVAILABLE/
 *            RULE_TYPE_OK/
 *            RULE_TYPE_FAIL
 * NOTE     : 1, this API is common interface to IPv6 Source Guard
 *               internally it call different Driver API (hrdrv, rule_ctrl... etc.)
 *            2, if chip is already configured, update/delete config from chip
 *               if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_SetSrcIpv6FilterByLport(
    UI32_T  lport,
    UI8_T   sip6[SYS_TYPE_IPV6_ADDR_LEN],
    UI32_T  vid,
    UI32_T  action)
{
    BOOL_T ret;

    L4_MGR_CHECK_LPORT(lport, RULE_TYPE_FAIL);

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    ret = RULE_MGR_SetSrcIpv6FilterByUport(
        STKTPLG_OM_IFINDEX_TO_UNIT(lport),
        STKTPLG_OM_IFINDEX_TO_PORT(lport),
        sip6,
        vid,
        (action == L4_MGR_DO_ACTION_PERMIT) ? TRUE : FALSE
        );

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret? RULE_TYPE_OK : RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetIpv6PktActionByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : for IPv6 Source Guard
 *            config chip to drop IPv6 packets
 * INPUT    : lport
 *            action        -- could be:
 *                          0   L4_MGR_DO_ACTION_DELETE_RULE
 *                          4   L4_MGR_DO_ACTION_DROP
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_FREE_SELECTOR_UNAVAILABLE/
 *            RULE_TYPE_OK/
 *            RULE_TYPE_FAIL
 * NOTE     : 1, this API is common interface to IPv6 Source Guard
 *               internally it call different Driver API (hrdrv, rule_ctrl... etc.)
 *            2, drop action in this API has lowest priority. That is,
 *               L4_MGR_SetDhcpClientPktActionByLport(...),
 *               L4_MGR_SetDhcpServerPktActionByLport(...),
 *               L4_MGR_SetSrcIpFilterByLport(...),
 *               L4_MGR_SetSrcMacSrcIpFilterByLport(...)
 *               dominate some IP packets do not be drop
 *            3, if chip is already configured, update/delete config from chip
 *               if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_SetIpv6PktActionByLport(UI32_T lport, UI32_T action)
{
    UI32_T ret;

    L4_MGR_CHECK_LPORT(lport, RULE_TYPE_FAIL);

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();

    ret = RULE_MGR_SetIpv6PktActionByUport(
        STKTPLG_OM_IFINDEX_TO_UNIT(lport),
        STKTPLG_OM_IFINDEX_TO_PORT(lport),
        (action==L4_MGR_DO_ACTION_DROP)?TRUE:FALSE
        );

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret ? RULE_TYPE_OK : RULE_TYPE_FAIL);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetPermitIpv6LinkLocal
 *------------------------------------------------------------------------------
 * PURPOSE  : Permit all IPv6 link local packet
 * INPUT    : is_enable - TRUE / FALSE
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK / RULE_TYPE_FAIL
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_SetPermitIpv6LinkLocal(
    BOOL_T  is_enable)
{
    UI32_T ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);
    L4_MGR_LOCK();
    ret = RULE_MGR_SetPermitIpv6LinkLocal(is_enable);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

#endif /* #if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE) */

#if (SYS_CPNT_WEBAUTH == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetRedirectHTTPClientPackets
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs to redirect HTTP client packets to CPU
 *             for specified logical port. (DPORT == HTTP server)
 *   INPUT : lport, is_enable
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : None
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_SetRedirectHTTPClientPackets(UI32_T lport, BOOL_T is_enable)
{
#if (SYS_CPNT_HRDRV == TRUE)
    return FALSE;

#else
    BOOL_T  ret;

    L4_MGR_CHECK_LPORT(lport, FALSE);

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    ret = RULE_MGR_SetRedirectHTTPClientPackets(
            STKTPLG_OM_IFINDEX_TO_UNIT(lport), STKTPLG_OM_IFINDEX_TO_PORT(lport),
            is_enable);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);

#endif
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetPermitSrcIpPacketsByLport
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs to permit IP packets with specified sip
 *             for specified logical port.
 *   INPUT : lport, sip, is_enable
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : None
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_SetPermitSrcIpPacketsByLport(
    UI32_T lport, UI32_T sip, BOOL_T is_enable)
{
#if (SYS_CPNT_HRDRV == TRUE)
    return FALSE;

#else
    BOOL_T ret;

    L4_MGR_CHECK_LPORT(lport, FALSE);

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    ret = RULE_MGR_SetPermitSrcIpPacketsByUport(STKTPLG_OM_IFINDEX_TO_UNIT(lport),
                STKTPLG_OM_IFINDEX_TO_PORT(lport), sip, is_enable);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);

#endif
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetDenyIpPacketsByLport
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs to deny IP packets for specified unit/port.
 *   INPUT : lport, is_enable
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : 1, share the same pce with IP SourceGuard
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_SetDenyIpPacketsByLport(UI32_T lport, BOOL_T is_enable)
{
#if (SYS_CPNT_HRDRV == TRUE)
    return FALSE;

#else
    BOOL_T ret;

    L4_MGR_CHECK_LPORT(lport, FALSE);

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    ret = RULE_MGR_SetDenyIpPacketsByUport(STKTPLG_OM_IFINDEX_TO_UNIT(lport),
                STKTPLG_OM_IFINDEX_TO_PORT(lport), is_enable);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);

#endif
}
#endif /* #if (SYS_CPNT_WEBAUTH == TRUE) */

#if (SYS_CPNT_COS == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_EnablePortPriorityMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function uses to enable priority's config of COS
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:   NONE
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_EnablePortPriorityMode(
    UI32_T l_port,
    UI16_T priority)
{
    COS_TYPE_ERROR_CODE_E error_code;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    error_code = COS_VM_EnablePortPriorityMode(l_port, priority);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((error_code == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_DisablePortPriorityMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function uses to disable priority's config of COS
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:   NONE
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_DisablePortPriorityMode(
    UI32_T l_port,
    UI16_T priority)
{
    COS_TYPE_ERROR_CODE_E error_code;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    error_code = COS_VM_DisablePortPriorityMode(l_port, priority);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((error_code == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

//:TRUST_MODE
BOOL_T L4_MGR_QOS_SetTrustMode(UI32_T lport, UI32_T mode)
{
    BOOL_T ret = TRUE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (COS_TYPE_E_NONE != COS_VM_SetPortTrustMode(
                               lport, COS_TYPE_PRIORITY_USER, mode))
    {
        ret = FALSE;
    }

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

BOOL_T L4_MGR_QOS_GetTrustMode(UI32_T lport, UI32_T *mode)
{
    BOOL_T ret = TRUE;
    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    if(!SWCTRL_PortExisting(lport))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (COS_TYPE_E_NONE != COS_VM_GetPortTrustMode(
                               lport, COS_TYPE_PRIORITY_USER, mode))
    {
        ret = FALSE;
    }
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortPriorityTrustMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function sets the COS trust mode
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           mode     - trust mode, qos_if_trust_mode_t
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPortPriorityTrustMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T mode)
{
    COS_TYPE_ERROR_CODE_E error_code;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    error_code = COS_VM_SetPortTrustMode(l_port, priority, mode);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((error_code == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortPriorityTrustMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the COS trust mode
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:   mode     - trust mode, qos_if_trust_mode_t
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortPriorityTrustMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T *mode)
{
    COS_TYPE_ERROR_CODE_E error_code;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    error_code = COS_VM_GetPortTrustMode(l_port, priority, mode);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((error_code == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

//:COS_TO_DSCP
BOOL_T L4_MGR_QOS_SetIngressCos2Dscp(UI32_T lport,UI32_T cos,UI32_T cfi,UI32_T phb,UI32_T color)
{
    COS_TYPE_ERROR_CODE_E ret;
    const UI16_T priority = COS_TYPE_PRIORITY_USER;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if(phb == 8 && color == 8)
    {
        ret = COS_VM_ResetPortCos2InternalDscp(lport, priority, cos, cfi);
    }
    else
    {
        ret = COS_VM_SetPortCos2InternalDscp(lport, priority, cos, cfi, phb, color);
    }

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((ret == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

BOOL_T L4_MGR_QOS_GetIngressCos2Dscp(UI32_T lport,UI32_T cos,UI32_T cfi,UI32_T *phb,UI32_T *color)
{
    COS_TYPE_ERROR_CODE_E ret;
    const UI16_T priority = COS_TYPE_PRIORITY_USER;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    ret = COS_VM_GetPortCos2InternalDscp(lport, priority, cos, cfi, phb, color);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((ret == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

BOOL_T L4_MGR_QOS_GetNextIngressCos2Dscp(UI32_T *lport,UI32_T *cos,UI32_T *cfi,UI32_T *phb,UI32_T *color)
{
    COS_TYPE_ERROR_CODE_E ret;
    const UI16_T priority = COS_TYPE_PRIORITY_USER;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    ret = COS_VM_GetNextPortCos2InternalDscp(lport, priority, cos, cfi, phb, color);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((ret == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the global mapping of ingress cos to phb and
 *            color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos      - cos vlaue
 *            cfi      - cfi value
 *            phb      - phb value
 *            color    - color vlaue
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_SetCos2InternalDscp(
    UI32_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T phb,
    UI32_T color)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = COS_VM_SetCos2InternalDscp((UI16_T) priority, cos, cfi, phb, color);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the global mapping of ingress cos to phb and
 *            color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos      - cos vlaue
 *            cfi      - cfi value
 * OUTPUT   : phb_p    - phb value
 *            color_p  - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetCos2InternalDscp(
    UI32_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = COS_VM_GetCos2InternalDscp((UI16_T) priority, cos, cfi, phb_p, color_p);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next the global mapping of ingress cos to
 *            phb and color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos      - cos vlaue.
 *            cfi      - cfi value.
 * OUTPUT   : cos      -
 *            cfi      -
 *            phb      - phb value.
 *            color    - color vlaue.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using cos = COS_TYPE_UNKNOWN_COS, cfi = COS_TYPE_UNKNOWN_CFI
 *            to get first entry.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetNextCos2InternalDscp(
    UI32_T priority,
    UI32_T *cos_p,
    UI32_T *cfi_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = COS_VM_GetNextCos2InternalDscp((UI16_T) priority, cos_p,
              cfi_p, phb_p, color_p);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetActiveCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the active mapping of ingress cos to phb and
 *            color.
 * INPUT    : cos      - cos vlaue
 *            cfi      - cfi value
 * OUTPUT   : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetActiveCos2InternalDscp(
    UI32_T cos,
    UI32_T cfi,
    UI32_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;
    UI16_T priority;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = COS_VM_GetActiveCos2InternalDscp(cos, cfi, &priority,
              phb_p, color_p);

    *priority_p = (UI32_T) priority;
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextRunningCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of cos and cfi which is
 *            mapping to the input phb and color.
 * INPUT    : phb   - phb vlaue.
 *            color - color value.
 * OUTPUT   : cos_p - cos value mapping to the input (phb, color).
 *            cfi_p - cfi vlaue mapping to the input (phb, color).
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using cos = COS_TYPE_UNKNOWN_COS, cfi = COS_TYPE_UNKNOWN_CFI
 *            to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetNextRunningCos2InternalDscp(
    UI32_T phb,
    UI32_T color,
    UI32_T *cos_p,
    UI32_T *cfi_p)
{
    SYS_TYPE_Get_Running_Cfg_T ret = SYS_TYPE_GET_RUNNING_CFG_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = COS_VM_GetNextRunningCos2InternalDscp(phb, color, cos_p, cfi_p);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the per-port mapping of ingress cos to phb and
 *            color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos      - cos vlaue
 *            cfi      - cfi value
 *            phb      - phb value
 *            color    - color vlaue
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_SetPortCos2InternalDscp(
    UI32_T l_port,
    UI32_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T phb,
    UI32_T color)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = COS_VM_SetPortCos2InternalDscp(l_port, (UI16_T) priority,
              cos, cfi, phb, color);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the per-port mapping of ingress cos to phb and
 *            color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos      - cos vlaue
 *            cfi      - cfi value
 * OUTPUT   : phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortCos2InternalDscp(
    UI32_T l_port,
    UI32_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = COS_VM_GetPortCos2InternalDscp(l_port, (UI16_T) priority,
              cos, cfi, phb_p, color_p);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next the per-port mapping of ingress cos to
 *            phb and color.
 * INPUT    : l_port_p - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos_p    - cos vlaue
 *            cfi_p    - cfi value
 * OUTPUT   : l_port_p -
 *            cos_p    -
 *            cfi_p    -
 *            phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using l_port = 0, cos = COS_TYPE_UNKNOWN_COS,
 *            cfi = COS_TYPE_UNKNOWN_CFI to get first entry.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetNextPortCos2InternalDscp(
    UI32_T *l_port_p,
    UI32_T priority,
    UI32_T *cos_p,
    UI32_T *cfi_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = COS_VM_GetNextPortCos2InternalDscp(l_port_p, (UI16_T) priority,
              cos_p, cfi_p, phb_p, color_p);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortActiveCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the per-port actived mapping of ingress cos to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            cos      - cos vlaue
 *            cfi      - cfi value
 * OUTPUT   : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortActiveCos2InternalDscp(
    UI32_T l_port,
    UI32_T cos,
    UI32_T cfi,
    UI32_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;
    UI16_T priority;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = COS_VM_GetPortActiveCos2InternalDscp(l_port, cos, cfi, &priority, phb_p, color_p);

    *priority_p = (UI32_T) priority;
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextRunningPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of ingress cos to
 *            internal dscp
 * INPUT    : l_port   - Logical port id.
 *            phb   - phb vlaue.
 *            color - color value.
 *            cos_p - The start cos to get next.
 *            cfi_p - The start cfi to get next.
 * OUTPUT   : cos_p - cos value mapping to the input (phb, color).
 *            cfi_p - cfi vlaue mapping to the input (phb, color).
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using cos = COS_TYPE_UNKNOWN_COS, cfi = COS_TYPE_UNKNOWN_CFI
 *            to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetNextRunningPortCos2InternalDscp(
    UI32_T l_port,
    UI32_T phb,
    UI32_T color,
    UI32_T *cos_p,
    UI32_T *cfi_p)
{
    SYS_TYPE_Get_Running_Cfg_T ret = SYS_TYPE_GET_RUNNING_CFG_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = COS_VM_GetNextRunningPortCos2InternalDscp(l_port, phb, color,
              cos_p, cfi_p);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}


//:PRE_2_DSCP
BOOL_T L4_MGR_QOS_SetIngressPre2Dscp(UI32_T lport,UI32_T pre,UI32_T phb,UI32_T color)
{
    COS_TYPE_ERROR_CODE_E ret;
    UI16_T priority = COS_TYPE_PRIORITY_USER;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    ret = COS_VM_SetPortPre2InternalDscp(lport, priority, pre, phb, color);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((ret == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

BOOL_T L4_MGR_QOS_ResetIngressPre2Dscp(UI32_T lport)
{
    BOOL_T ret = TRUE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    ret = COS_VM_ResetIngressPre2Dscp(lport);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);

}

BOOL_T L4_MGR_QOS_GetIngressPre2Dscp(UI32_T lport,UI8_T *phb,UI8_T *color)
{
    COS_TYPE_ERROR_CODE_E ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    ret = COS_VM_GetIngressPre2Dscp(lport, phb, color);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((ret == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the mapping of ingress IP precedence to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            pre      - IP precednece vlaue
 *            phb      - phb value
 *            color    - color vlaue
 * OUTPUT   :
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_SetPortPre2InternalDscp(
    UI32_T l_port,
    UI32_T priority,
    UI32_T pre,
    UI32_T phb,
    UI32_T color)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = COS_VM_SetPortPre2InternalDscp(l_port, (UI16_T) priority,
              pre, phb, color);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the mapping of ingress IP precedence to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            pre      - IP precednece vlaue
 * OUTPUT   : phb   - phb value
 *            color - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortPre2InternalDscp(
    UI32_T l_port,
    UI32_T priority,
    UI32_T pre,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = COS_VM_GetPortPre2InternalDscp(l_port, (UI16_T) priority,
              pre, phb_p, color_p);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextPortPre2InternalDscp
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
UI32_T
L4_MGR_QOS_GetNextPortPre2InternalDscp(
    UI32_T *l_port_p,
    UI32_T priority,
    UI32_T *pre_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = COS_VM_GetNextPortPre2InternalDscp(l_port_p, (UI16_T) priority,
              pre_p, phb_p, color_p);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortActivePre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the actived mapping of ingress IP precedence to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            pre      - IP precednece vlaue
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb_p      - phb value
 *            color_p    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortActivePre2InternalDscp(
    UI32_T l_port,
    UI32_T pre,
    UI32_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;
    UI16_T priority;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = COS_VM_GetPortActivePre2InternalDscp(l_port, pre,
              &priority, phb_p, color_p);

    *priority_p = (UI32_T) priority;

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetRunningPortPre2InternalDscp
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
L4_MGR_QOS_GetRunningPortPre2InternalDscp(
    UI32_T l_port,
    UI32_T *phb_ar,
    UI32_T *color_ar)
{
    SYS_TYPE_Get_Running_Cfg_T ret = SYS_TYPE_GET_RUNNING_CFG_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();
    ret = COS_VM_GetRunningPortPre2InternalDscp(l_port, phb_ar, color_ar);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

//:DSCP_TO_DSCP
BOOL_T L4_MGR_QOS_SetIngressDscp2Dscp(UI32_T lport,UI32_T o_dscp,UI32_T phb,UI32_T color)
{
    COS_TYPE_ERROR_CODE_E ret;
    const UI16_T priority = COS_TYPE_PRIORITY_USER;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if(phb == 8 && color == 8)
    {
        ret = COS_VM_ResetIngressDscp2Dscp(lport, o_dscp);
    }
    else
    {
        ret = COS_VM_SetPortDscp2InternalDscp(lport, priority, o_dscp, phb, color);
    }

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((ret == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

BOOL_T L4_MGR_QOS_GetIngressDscp2Dscp(UI32_T lport,UI32_T dscp,UI32_T *phb,UI32_T *color)
{
    COS_TYPE_ERROR_CODE_E ret;
    const UI16_T priority = COS_TYPE_PRIORITY_USER;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE)
    if(!SWCTRL_PortExisting(lport))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }
#endif /* SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT */

    ret = COS_VM_GetPortDscp2InternalDscp(lport, priority, dscp, phb,color);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((ret == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the global mapping of ingress dscp to
 *            phb and color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp     - dscp value
 *            phb      - phb value
 *            color    - color vlaue
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_SetDscp2InternalDscp(
    UI32_T priority,
    UI32_T dscp,
    UI32_T phb,
    UI32_T color)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();
    ret = COS_VM_SetDscp2InternalDscp((UI16_T) priority, dscp, phb, color);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the global mapping of ingress dscp to
 *            phb and color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp     - dscp value
 * OUTPUT   : phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetDscp2InternalDscp(
    UI32_T priority,
    UI32_T dscp,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();
    ret = COS_VM_GetDscp2InternalDscp((UI16_T) priority, dscp, phb_p, color_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next global mapping of ingress dscp to
 *            phb and color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp_p   - dscp value
 * OUTPUT   : dscp_p   -
 *            phb_p    - phb value
 *            color_p  - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using dscp_p = COS_TYPE_UNKNOWN_DSCP to get first entry.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetNextDscp2InternalDscp(
    UI32_T priority,
    UI32_T *dscp_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();
    ret = COS_VM_GetNextDscp2InternalDscp((UI16_T) priority,
              dscp_p, phb_p, color_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetActiveDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the actived global mapping of ingress dscp to
 *            phb and color.
 * INPUT    : dscp       - dscp value
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb_p      - phb value
 *            color_p    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetActiveDscp2InternalDscp(
    UI32_T dscp,
    UI32_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;
    UI16_T priority;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = COS_VM_GetActiveDscp2InternalDscp(dscp,
              &priority, phb_p, color_p);
    *priority_p = (UI32_T) priority;

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextRunningDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of ingress dscp which is
 *            mapping to input phb and color.
 * INPUT    : phb    - phb vlaue.
 *            color  - color value.
 *            dscp_p - dscp value to get next mapping.
 * OUTPUT   : dscp_p - dscp value mapping to the input phb and color.
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using dscp = COS_TYPE_UNKNOWN_DSCP to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetNextRunningDscp2InternalDscp(
    UI32_T phb,
    UI32_T color,
    UI32_T *dscp_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();
    ret = COS_VM_GetNextRunningDscp2InternalDscp(phb, color, dscp_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the per-port mapping of ingress dscp to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp     - dscp value
 *            phb      - phb value
 *            color    - color vlaue
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_SetPortDscp2InternalDscp(
    UI32_T l_port,
    UI16_T priority,
    UI32_T dscp,
    UI32_T phb,
    UI32_T color)
{
    COS_TYPE_ERROR_CODE_E error_code = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(error_code);
    L4_MGR_LOCK();
    error_code = COS_VM_SetPortDscp2InternalDscp(l_port, priority, dscp, phb, color);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(error_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the per-port mapping of ingress dscp to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp     - dscp value
 * OUTPUT   : phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortDscp2InternalDscp(
    UI32_T l_port,
    UI32_T priority,
    UI32_T dscp,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    COS_TYPE_ERROR_CODE_E error_code = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(error_code);
    L4_MGR_LOCK();
    error_code = COS_VM_GetPortDscp2InternalDscp(l_port, priority, dscp, phb_p, color_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(error_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next per-port mapping of ingress dscp to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp_p   - dscp value
 * OUTPUT   : l_port  -
 *            dscp_p  -
 *            phb_p   - phb value
 *            color_p - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using l_port = 0, dscp_p = COS_TYPE_UNKNOWN_DSCP to get first
 *            entry.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetNextPortDscp2InternalDscp(
    UI32_T *l_port_p,
    UI32_T priority,
    UI32_T *dscp_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();
    ret = COS_VM_GetNextPortDscp2InternalDscp(l_port_p, (UI16_T) priority,
              dscp_p, phb_p, color_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortActiveDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets actived per-port mapping of ingress dscp to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            dscp     - dscp value
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_X, the actived priority.
 *            phb_p      - phb value
 *            color_p    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortActiveDscp2InternalDscp(
    UI32_T l_port,
    UI32_T dscp,
    UI32_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;
    UI16_T priority;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();
    ret = COS_VM_GetPortActiveDscp2InternalDscp(l_port, dscp,
              &priority, phb_p, color_p);
    *priority_p = (UI32_T) priority;

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextRunningPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of ingress dscp to
 *            internal dscp
 * INPUT    : l_port - Logical port id.
 *            phb    - phb vlaue.
 *            color  - color value.
 *            dscp_p - The dscp value using for get next.
 * OUTPUT   : dscp_p - dscp value mapping to the input phb and color.
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using l_port = 0, dscp = COS_TYPE_UNKNOWN_DSCP to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetNextRunningPortDscp2InternalDscp(
    UI32_T l_port,
    UI32_T phb,
    UI32_T color,
    UI32_T *dscp_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();
    ret = COS_VM_GetNextRunningPortDscp2InternalDscp(l_port, phb, color, dscp_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

//:PHB_TO_QUEUE
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Set the global mapping of ingress DSCP to Queue
 * INPUT    : phb
 *            queue - if queue == 8, set the phb mapping to default queue.
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : This API only for user config
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetIngressDscp2Queue(
    UI32_T phb,
    UI32_T queue)
{
    COS_TYPE_ERROR_CODE_E ret;
    const UI16_T priority = COS_TYPE_PRIORITY_USER;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if(queue >= SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE)
    {
        ret = COS_VM_ResetIngressDscp2Queue(phb);
    }
    else
    {
        ret = COS_VM_SetIngressPhb2Queue(priority, phb, queue);
    }

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((ret == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Set the mapping of ingress DSCP to Queue by port
 * INPUT    : l_port
 *            phb
 *            queue - if queue == 8, set the phb mapping to default queue.
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : This API only for user config
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPortIngressDscp2Queue(
    UI32_T lport,
    UI32_T phb,
    UI32_T queue)
{
    COS_TYPE_ERROR_CODE_E ret;
    const UI16_T priority = COS_TYPE_PRIORITY_USER;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if(queue >= SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE)
    {
        ret = COS_VM_ResetPortIngressDscp2Queue(lport,phb);
    }
    else
    {
        ret = COS_VM_SetPortIngressPhb2Queue(lport, priority, phb,queue);
    }

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((ret == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Get the global mapping of ingress DSCP to Queue
 * INPUT    : None
 * OUTPUT   : queue[MAX_PHB_VAL + 1]
 * RETURN   : TRUE/FALSE
 * NOTE     :
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetIngressDscp2Queue(
    UI8_T *queue)
{
    const UI16_T priority = COS_TYPE_PRIORITY_USER;
    UI32_T phb;
    COS_TYPE_ERROR_CODE_E ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    for (phb = MIN_PHB_VAL; phb <= MAX_PHB_VAL; ++phb)
    {
        UI32_T temp_queue;

        ret = COS_VM_GetIngressPhb2Queue(priority, phb, &temp_queue);
        if (ret != COS_TYPE_E_NONE)
        {
            break;
        }

        queue[phb] = temp_queue;
    }

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((ret == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Get the mapping of ingress DSCP to Queue by port
 * INPUT    : l_port
 * OUTPUT   : queue[MAX_PHB_VAL + 1]
 * RETURN   : TRUE/FALSE
 * NOTE     :
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortIngressDscp2Queue(
    UI32_T l_port,
    UI8_T *queue)
{
    COS_TYPE_ERROR_CODE_E ret;
    UI32_T phb;
    const UI16_T priority = COS_TYPE_PRIORITY_USER;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    for (phb = MIN_PHB_VAL; phb <= MAX_PHB_VAL; ++phb)
    {
        UI32_T temp_queue;

        ret = COS_VM_GetPortIngressPhb2Queue(l_port, priority, phb, &temp_queue);
        if (ret != COS_TYPE_E_NONE)
        {
            break;
        }

        queue[phb] = temp_queue;
    }

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((ret == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPriorityIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue for all Dscp
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  queue    - mapping table of dscp to queue , queue[MAX_PHB_VAL+1]
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPriorityIngressDscp2Queue(
    UI16_T priority,
    UI8_T *queue)
{
    COS_TYPE_ERROR_CODE_E error_code;
    UI32_T phb;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    for (phb = 0; phb <= MAX_PHB_VAL; ++phb)
    {
        UI32_T temp_queue;

        error_code = COS_VM_GetIngressPhb2Queue(priority, phb, &temp_queue);
        if (error_code != COS_TYPE_E_NONE)
        {
            break;
        }

        queue[phb] = (UI8_T) temp_queue;
    }
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((error_code == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME -L4_MGR_QOS_GetPortPriorityIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue for all Dscp by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  queue    - mapping table of dscp to queue , queue[MAX_PHB_VAL+1]
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Return the config of the input priority.
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortPriorityIngressDscp2Queue(
    UI32_T l_port,
    UI16_T priority,
    UI8_T *queue)
{
    COS_TYPE_ERROR_CODE_E error_code;
    UI32_T phb;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    for (phb = 0; phb <= MAX_PHB_VAL; ++phb)
    {
        UI32_T temp_queue;

        error_code = COS_VM_GetPortIngressPhb2Queue(l_port, priority, phb, &temp_queue);
        if (error_code != COS_TYPE_E_NONE)
        {
            break;
        }

        queue[phb] = (UI8_T) temp_queue;
    }
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((error_code == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Set the global mapping of PHB to queue
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - PHB value.
 *            queue    - Queue id.
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_SetIngressPhb2Queue(
    UI32_T priority,
    UI32_T phb,
    UI32_T queue)
{
    COS_TYPE_ERROR_CODE_E error_code = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(error_code);
    L4_MGR_LOCK();
    error_code = COS_VM_SetIngressPhb2Queue((UI16_T) priority, phb, queue);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(error_code);
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets the global mapping of PHB to queue
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - PHB value.
 * OUTPUT   : queue_p  - Queue id.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetIngressPhb2Queue(
    UI32_T priority,
    UI32_T phb,
    UI32_T *queue_p)
{
    COS_TYPE_ERROR_CODE_E error_code = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(error_code);
    L4_MGR_LOCK();
    error_code = COS_VM_GetIngressPhb2Queue((UI16_T) priority, phb, queue_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(error_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetActiveIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets actived global mapping of PHB to queue
 * INPUT    : phb        - PHB value.
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            queue_p    - Queue id.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetActiveIngressPhb2Queue(
    UI32_T phb,
    UI32_T *priority_p,
    UI32_T *queue_p)
{
    COS_TYPE_ERROR_CODE_E error_code = COS_TYPE_E_UNKNOWN;
    UI16_T priority;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(error_code);
    L4_MGR_LOCK();
    error_code = COS_VM_GetActiveIngressPhb2Queue(phb, &priority, queue_p);
    *priority_p = (UI32_T) priority;
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(error_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextRunningIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of PHB to queue.
 * INPUT    : queue_p - Queue id.
 *            phb_p   - PHB value.
 * OUTPUT   : phb_p -
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using phb = COS_TYPE_UNKNOWN_PHB to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetNextRunningIngressPhb2Queue(
    UI32_T queue,
    UI32_T *phb_p)
{
    SYS_TYPE_Get_Running_Cfg_T ret = SYS_TYPE_GET_RUNNING_CFG_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();
    ret = COS_VM_GetNextRunningIngressPhb2Queue(queue, phb_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Set the per-port mapping of PHB to queue.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - PHB value.
 *            queue    - Queue id.
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_SetPortIngressPhb2Queue(
    UI32_T l_port,
    UI32_T priority,
    UI32_T phb,
    UI32_T queue)
{
    COS_TYPE_ERROR_CODE_E error_code = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(error_code);
    L4_MGR_LOCK();
    error_code = COS_VM_SetPortIngressPhb2Queue(l_port, (UI16_T) priority,
                     phb, queue);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(error_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets the per-port mapping of PHB to queue
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - PHB value.
 * OUTPUT   : queue_p  - Queue id.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortIngressPhb2Queue(
    UI32_T l_port,
    UI32_T priority,
    UI32_T phb,
    UI32_T *queue_p)
{
    COS_TYPE_ERROR_CODE_E error_code = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(error_code);
    L4_MGR_LOCK();
    error_code = COS_VM_GetPortIngressPhb2Queue(l_port, (UI16_T) priority,
                     phb, queue_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(error_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets the next per-port mapping of PHB to queue
 * INPUT    : l_port_p - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb_p    - PHB value.
 * OUTPUT   : queue_p  - Queue id.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using ifindex = 0, phb = COS_TYPE_UNKNOWN_PHB to get first entry
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetNextPortIngressPhb2Queue(
    UI32_T *l_port_p,
    UI32_T priority,
    UI32_T *phb_p,
    UI32_T *queue_p)
{
    COS_TYPE_ERROR_CODE_E error_code = COS_TYPE_E_UNKNOWN;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(error_code);
    L4_MGR_LOCK();
    error_code = COS_VM_GetNextPortIngressPhb2Queue(l_port_p, (UI16_T) priority,
                     phb_p, queue_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(error_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortActiveIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets actived global mapping of PHB to queue
 * INPUT    : l_port     - Logical port id.
 *            phb        - PHB value.
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            queue_p    - Queue id.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortActiveIngressPhb2Queue(
    UI32_T l_port,
    UI32_T phb,
    UI32_T *priority_p,
    UI32_T *queue_p)
{
    COS_TYPE_ERROR_CODE_E error_code = COS_TYPE_E_UNKNOWN;
    UI16_T priority;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(error_code);
    L4_MGR_LOCK();
    error_code = COS_VM_GetPortActiveIngressPhb2Queue(l_port, phb, &priority, queue_p);
    *priority_p = (UI32_T) priority;
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(error_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextRunningPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of PHB to queue.
 * INPUT    : l_port     - Logical port id.
 *            queue_p - Queue id.
 *            phb_p   - PHB value.
 * OUTPUT   : phb_p -
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using phb = COS_TYPE_UNKNOWN_PHB to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetNextRunningPortIngressPhb2Queue(
    UI32_T l_port,
    UI32_T queue,
    UI32_T *phb_p)
{
    SYS_TYPE_Get_Running_Cfg_T ret = SYS_TYPE_GET_RUNNING_CFG_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();
    ret = COS_VM_GetNextRunningPortIngressPhb2Queue(l_port, queue, phb_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

//:DSCP_TO_COLOR
BOOL_T L4_MGR_QOS_SetIngressDscp2Color(UI32_T lport,UI32_T phb,UI32_T color)
{
    BOOL_T ret = TRUE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if(color == 4)
    {
        ret = COS_VM_ResetIngressDscp2Color(lport,phb);
    }
    else
    {
        ret = COS_VM_SetIngressDscp2Color(lport,phb,color);
    }
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);

}
BOOL_T L4_MGR_QOS_GetIngressDscp2Color(UI32_T lport,UI8_T *color)
{
    BOOL_T ret = TRUE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    if(!SWCTRL_PortExisting(lport))
        return FALSE;
    ret = COS_VM_GetIngressDscp2Color(lport,color);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);

}

BOOL_T L4_MGR_QOS_GetRunningIngressDscp2Color(UI32_T lport,UI8_T *color)
{
    BOOL_T ret = TRUE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    if(!SWCTRL_PortExisting(lport))
        return FALSE;
    ret = COS_VM_GetRunningIngressDscp2Color(lport,color);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);

}

//:DSCP_TO_COS
BOOL_T L4_MGR_QOS_SetIngressDscp2Cos(UI32_T lport,UI32_T phb,UI32_T color,UI32_T cos,UI32_T cfi)
{
    BOOL_T ret = TRUE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if(cos == 8 && cfi == 8)
    {
        ret = COS_VM_ResetIngressDscp2Cos(lport,phb,color);
    }
    else
    {
        ret = COS_VM_SetIngressDscp2Cos(lport,phb,color,cos,cfi);
    }
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);

}
BOOL_T L4_MGR_QOS_GetIngressDscp2Cos(UI32_T lport,UI32_T phb,UI32_T color,UI32_T *cos,UI32_T *cfi)
{
    BOOL_T ret = TRUE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    if(!SWCTRL_PortExisting(lport))
        return FALSE;
    ret = COS_VM_GetIngressDscp2Cos(lport,phb,color,cos,cfi);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);

}

BOOL_T L4_MGR_QOS_GetNextIngressDscp2Cos(UI32_T *lport,UI32_T *phb,UI32_T *color,UI32_T *cos,UI32_T *cfi)
{
    BOOL_T ret = TRUE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    if(!SWCTRL_PortExisting(*lport))
        return FALSE;
    ret = COS_VM_GetNextIngressDscp2Cos(lport,phb,color,cos,cfi);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);

}

BOOL_T L4_MGR_QOS_GetRunningIngressDscp2Cos(UI32_T lport,UI8_T cos,UI8_T cfi,UI8_T *l_phb,UI8_T *l_color,UI8_T *cos_phb,UI8_T *cos_color)
{
    BOOL_T ret = TRUE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    if(!SWCTRL_PortExisting(lport))
        return FALSE;
    ret = COS_VM_GetRunningIngressDscp2Cos(lport,cos,cfi,l_phb,l_color,cos_phb,cos_color);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);

}

//:IP_PORT_DSCP
BOOL_T L4_MGR_QOS_SetTCPUDP2Dscp(UI32_T ifindex,UI32_T protocol,UI32_T dport,UI32_T phb,UI32_T color,UI32_T add)
{
    BOOL_T ret = TRUE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if(FALSE == add)
    {
        ret = COS_VM_SetTCPUDP2Dscp(ifindex,protocol,dport,phb,color);
    }
    else
    {
        ret = COS_VM_ResetTCPUDP2Dscp(ifindex,protocol,dport,phb,color);
    }
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

BOOL_T L4_MGR_QOS_GetTCPUDP2Dscp(UI32_T ifindex,UI32_T protocol,UI32_T dport,UI32_T *phb,UI32_T *color)
{
    BOOL_T ret = TRUE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    ret = COS_VM_GetTCPUDP2Dscp(ifindex,protocol,dport,phb,color);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

BOOL_T L4_MGR_QOS_GetNextTCPUDP2Dscp(UI32_T *ifindex,UI32_T *protocol,UI32_T *dport,UI32_T *phb,UI32_T *color)
{
    BOOL_T ret = TRUE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    ret = COS_VM_GetNextTCPUDP2Dscp(ifindex,protocol,dport,phb,color);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

BOOL_T L4_MGR_QOS_GetRunningTCPUDP2Dscp(UI32_T ifindex,UI32_T *protocol,UI32_T *dport,UI32_T *phb,UI32_T *color)
{
    BOOL_T ret = TRUE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    ret = COS_VM_GetRunningTCPUDP2Dscp(ifindex,protocol,dport,phb,color);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

//:QUEUE_MODE
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function sets the queue mode
 * INPUT:    l_port   - logic port
 *           mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     This function will set the config on user priority
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPortEgressQueueMode(
    UI32_T l_port,
    UI32_T mode)
{
    COS_TYPE_ERROR_CODE_E result;
    const UI16_T priority = COS_TYPE_PRIORITY_USER;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    result = COS_VM_SetPortEgressQueueMode(l_port, priority, mode);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((result == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the queue mode
 * INPUT:    l_port   - logic port
 * OUTPUT:   mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortEgressQueueMode(
    UI32_T l_port,
    UI32_T *mode)
{
    COS_TYPE_ERROR_CODE_E result;
    const UI16_T priority = COS_TYPE_PRIORITY_USER;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    result = COS_VM_GetPortEgressQueueMode(l_port, priority, mode);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((result == COS_TYPE_E_NONE) ? TRUE : FALSE);
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextPortEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the next queue mode by port
 * INPUT:    ifindex_p   -
 *
 * OUTPUT:   ifindex_p -
 *           mode      - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetNextPortEgressQueueMode(
    UI32_T *ifindex_p,
    UI32_T *mode_p)
{
    COS_TYPE_ERROR_CODE_E result;
    const UI16_T priority = COS_TYPE_PRIORITY_USER;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    result = COS_VM_GetNextPortEgressQueueMode(priority, ifindex_p, mode_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((result == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPriorityEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function sets the queue mode
 * INPUT:    priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPriorityEgressQueueMode(
    UI16_T priority,
    UI32_T mode)
{
    COS_TYPE_ERROR_CODE_E error_code;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    error_code = COS_VM_SetEgressQueueMode(priority, mode);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((error_code == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPriorityEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the queue mode
 * INPUT:    priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:   mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPriorityEgressQueueMode(
    UI16_T priority,
    UI32_T *mode)
{
    COS_TYPE_ERROR_CODE_E error_code;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    error_code = COS_VM_GetEgressQueueMode(priority, mode);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((error_code == COS_TYPE_E_NONE) ? TRUE : FALSE);
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetActiveEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets actived queue mode
 * INPUT    :
 * OUTPUT   : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            mode     - COS_VM_SCHEDULING_METHOD_E, scheduling mode,
 * RETURN   : COS_TYPE_ERROR_CODE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetActiveEgressQueueMode(
    UI32_T *priority_p,
    UI32_T *mode_p)
{
    COS_TYPE_ERROR_CODE_E error_code = COS_TYPE_E_UNKNOWN;
    UI16_T priority;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(error_code);
    L4_MGR_LOCK();
    error_code = COS_VM_GetActiveEgressQueueMode(&priority, mode_p);
    *priority_p = (UI32_T) priority;
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(error_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortPriorityEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function sets the queue mode
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPortPriorityEgressQueueMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T mode)
{
    COS_TYPE_ERROR_CODE_E error_code;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    error_code = COS_VM_SetPortEgressQueueMode(l_port, priority, mode);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((error_code == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortPriorityEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the queue mode
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:   mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortPriorityEgressQueueMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T *mode)
{
    COS_TYPE_ERROR_CODE_E error_code;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    error_code = COS_VM_GetPortEgressQueueMode(l_port, priority, mode);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((error_code == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortActiveEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets per-port actived queue mode
 * INPUT    : l_port   - Logical port id.
 * OUTPUT   : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            mode     - COS_VM_SCHEDULING_METHOD_E, scheduling mode,
 * RETURN   : COS_TYPE_ERROR_CODE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortActiveEgressQueueMode(
    UI32_T l_port,
    UI32_T *priority_p,
    UI32_T *mode_p)
{
    COS_TYPE_ERROR_CODE_E error_code = COS_TYPE_E_UNKNOWN;
    UI16_T priority;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(error_code);
    L4_MGR_LOCK();
    error_code = COS_VM_GetPortActiveEgressQueueMode(l_port, &priority, mode_p);
    *priority_p = (UI32_T) priority;
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(error_code);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_ResetPortPriorityEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function resets the queue mode to default value
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_ResetPortPriorityEgressQueueMode(
    UI32_T l_port,
    UI16_T priority)
{
    COS_TYPE_ERROR_CODE_E error_code;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    error_code = COS_VM_ResetPortEgressQueueMode(l_port, priority);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((error_code == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

//:QUEUE_WEIGHT
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE:  This function sets the wrr queue weight
 * INPUT:    l_port   - logic port
 *           q_id     - queue id
 *           weight   - weight
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPortEgressWrrQueueWeight(
    UI32_T l_port,
    UI32_T q_id,
    UI32_T weight)
{
    COS_TYPE_ERROR_CODE_E ret;
    const UI16_T priority = COS_TYPE_PRIORITY_USER;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    ret = COS_VM_SetPortEgressQueueWeight(l_port, priority, q_id, weight);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((ret == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the wrr queue weight
 * INPUT:    l_port   - logic port
 *           q_id     - queue id
 * OUTPUT:   weight   - weight
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortEgressWrrQueueWeight(
    UI32_T l_port,
    UI32_T q_id,
    UI32_T *weight)
{
    BOOL_T ret = TRUE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    ret = COS_VM_GetPortEgressWrrQueueWeight(l_port, q_id, weight);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next wrr queue weight
 * INPUT:   queue_p     -
 *
 * OUTPUT:  queue_p     -
 *          weight_p    - queue weight
 * RETURN:  TRUE / FALSE
 * NOTE:    Input queue (COS_TYPE_UNKNOWN_QUEUE) to get first queue weight,
 *
 *          In hybrid mode, this function still gets the configured wrr weight
 *          for strict-queue
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetNextEgressQueueWeight(
    UI32_T *queue_p,
    UI32_T *weight_p)
{
    COS_TYPE_ERROR_CODE_E result;
    const UI16_T priority = COS_TYPE_PRIORITY_USER;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    result = COS_VM_GetNextEgressQueueWeight(priority, queue_p, weight_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(result);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextPortEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next wrr queue weight and queue status(strict/wrr)
 *          by port
 * INPUT:   ifindex_p   -
 *          queue_p     -
 *
 * OUTPUT:  ifindex_p   -
 *          queue_p     -
 *          weight      - queue weight
 * RETURN:  TRUE / FALSE
 * NOTE:    Input ifindex, queue (0, COS_TYPE_UNKNOWN_QUEUE)
 *          to get first queue weight.
 *
 *          In hybrid mode, this function still gets the configured wrr weight
 *          for strict-queue
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetNextPortEgressQueueWeight(
    UI32_T *ifindex_p,
    UI32_T *queue_p,
    UI32_T *weight_p)
{
    COS_TYPE_ERROR_CODE_E result;
    const UI16_T priority = COS_TYPE_PRIORITY_USER;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    result = COS_VM_GetNextPortEgressQueueWeight(
                 priority, ifindex_p, queue_p, weight_p);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((result == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPriorityEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE:  This function sets the wrr queue weight
 * INPUT:    priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           q_id     - queue id
 *           weight   - weight
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPriorityEgressWrrQueueWeight(
    UI16_T priority,
    UI32_T q_id,
    UI32_T weight)
{
    COS_TYPE_ERROR_CODE_E error_code;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    error_code = COS_VM_SetEgressQueueWeight(priority, q_id, weight);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((error_code == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPriorityEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the wrr queue weight
 * INPUT:    priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           q_id     - queue id
 * OUTPUT:   weight   - weighte
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPriorityEgressWrrQueueWeight(
    UI16_T priority,
    UI32_T q_id,
    UI32_T *weight)
{
    COS_TYPE_ERROR_CODE_E result;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    result = COS_VM_GetEgressQueueWeight(priority,
                                                 q_id,
                                                 weight);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((result == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetActiveQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets actived queue weight.
 * INPUT:   queue      - queue
 * OUTPUT:  priority_p - COS_TYPE_PRIORITY_XX, the actived priority of config.
 *          weight_p   - weight.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetActiveQueueWeight(
    UI32_T queue,
    UI32_T *priority_p,
    UI32_T *weight_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;
    UI16_T priority;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    ret = COS_VM_GetActiveQueueWeight(queue, &priority, weight_p);
    *priority_p = (UI32_T) priority;
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetRunningEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the config of wrr queue weight
 * INPUT:   queue     - queue id
 * OUTPUT:  weight_p    - queue weight
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetRunningEgressQueueWeight(
    UI32_T queue,
    UI32_T *weight_p)
{
    SYS_TYPE_Get_Running_Cfg_T ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);
    L4_MGR_LOCK();
    ret = COS_VM_GetRunningEgressQueueWeight(queue, weight_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortPriorityEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE:  This function sets the wrr queue weight
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           q_id     - queue id
 *           weight   - weight
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPortPriorityEgressWrrQueueWeight(
    UI32_T l_port,
    UI16_T priority,
    UI32_T q_id,
    UI32_T weight)
{
    COS_TYPE_ERROR_CODE_E error_code;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    error_code = COS_VM_SetPortEgressQueueWeight(l_port, priority, q_id, weight);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((error_code == COS_TYPE_E_NONE) ? TRUE : FALSE);
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortPriorityEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the wrr queue weight
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           q_id     - queue id
 * OUTPUT:   weight   - weighte
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortPriorityEgressWrrQueueWeight(
    UI32_T l_port,
    UI16_T priority,
    UI32_T q_id,
    UI32_T *weight)
{
    COS_TYPE_ERROR_CODE_E result;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    result = COS_VM_GetPortEgressQueueWeight(l_port, priority, q_id, weight);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((result == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortActiveQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the wrr queue weight
 * INPUT    : l_port     - logic port
 *            queue      - queue id
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            weight     - weight
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortActiveQueueWeight(
    UI32_T l_port,
    UI32_T queue,
    UI32_T *priority_p,
    UI32_T *weight_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;
    UI16_T priority;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();
    ret = COS_VM_GetPortActiveQueueWeight(l_port, queue, &priority, weight_p);
    *priority_p = (UI32_T) priority;
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_ResetPortPriorityEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE:  This function resets the wrr queue weight to default value
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           q_id     - queue id
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_ResetPortPriorityEgressWrrQueueWeight(
    UI32_T l_port,
    UI16_T priority,
    UI32_T q_id)
{
    COS_TYPE_ERROR_CODE_E error_code;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    error_code = COS_VM_ResetPortEgressQueueWeight(l_port, priority, q_id);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((error_code == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetRunningPortEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the config of wrr queue weight by port
 * INPUT:   ifindex   - logic port
 *          queue     - queue id
 * OUTPUT:  weight_p    - queue weight
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetRunningPortEgressQueueWeight(
    UI32_T ifindex,
    UI32_T queue,
    UI32_T *weight_p)
{
    SYS_TYPE_Get_Running_Cfg_T ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(SYS_TYPE_GET_RUNNING_CFG_FAIL);
    L4_MGR_LOCK();
    ret = COS_VM_GetRunningPortEgressQueueWeight(
              ifindex, queue, weight_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPriorityEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function only used for hybrid queue mode (ex: strict+wrr)
 *          to set the queue to be strict queue or wrr queue
 * INPUT:   priority      - The priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue         - Queue id
 *          status -
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPriorityEgressQueueStrictStatus(
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T status)
{
    COS_TYPE_ERROR_CODE_E result;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    result = COS_VM_SetEgressQueueStrictStatus(priority, queue, status);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((result == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortPriorityEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function only used for hybrid queue mode (ex: strict+wrr)
 *          to set the queue to be strict queue or wrr queue
 * INPUT:   ifindex
 *          priority      - The priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue         - Queue id
 *          status -
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPortPriorityEgressQueueStrictStatus(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T status)
{
    COS_TYPE_ERROR_CODE_E result;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    result = COS_VM_SetPortEgressQueueStrictStatus(ifindex,
                 priority, queue, status);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((result == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPriorityEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue status(strict/wrr)
 * INPUT:   priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue     - queue
 * OUTPUT:  status_p -
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPriorityEgressQueueStrictStatus(
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p)
{
    COS_TYPE_ERROR_CODE_E result;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    result = COS_VM_GetEgressQueueStrictStatus(priority, queue, status_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((result == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetActiveQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets actived queue status(strict/wrr)
 * INPUT:   queue      - queue
 * OUTPUT:  priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *          status_p   - COS_VM_QUEUE_STRICT_STATUS_T.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetActiveQueueStrictStatus(
    UI32_T queue,
    UI32_T *priority_p,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;
    UI16_T priority;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();
    ret = COS_VM_GetActiveEgressQueueStrictStatus(queue, &priority, status_p);
    *priority_p = (UI32_T) priority;
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortPriorityEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue status(strict/wrr)
 *          by port
 * INPUT:   ifindex   - logic port
 *          priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue     - queue id
 * OUTPUT:  status_p -
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortPriorityEgressQueueStrictStatus(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p)
{
    COS_TYPE_ERROR_CODE_E result;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();
    result = COS_VM_GetPortEgressQueueStrictStatus(ifindex,
                 priority, queue, status_p);
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC((result == COS_TYPE_E_NONE) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortActiveQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets actived queue status(strict/wrr) by port
 * INPUT:   l_port     - Logic port id.
 *          queue      - Queue id.
 * OUTPUT:  priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *          status_p   - COS_VM_QUEUE_STRICT_STATUS_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortActiveQueueStrictStatus(
    UI32_T l_port,
    UI32_T queue,
    UI32_T *priority_p,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p)
{
    COS_TYPE_ERROR_CODE_E ret = COS_TYPE_E_UNKNOWN;
    UI16_T priority;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();
    ret = COS_VM_GetPortActiveEgressQueueStrictStatus(l_port, queue, &priority, status_p);
    *priority_p = (UI32_T) priority;
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

#endif /* #if (SYS_CPNT_COS == TRUE) */

#if (SYS_CPNT_IPV6 == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_TrapLinkLocalToCPU
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs to trap my ipv6 link-local address for the vlan to CPU.
 *   INPUT : is_enable, ipv6 link-local addr, vlan_id
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : None
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_TrapLinkLocalToCPU(BOOL_T is_enable, UI8_T link_local_addr[SYS_ADPT_IPV6_ADDR_LEN], UI32_T vlan_id)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    ret = RULE_MGR_TrapLinkLocalToCPU(is_enable, link_local_addr, vlan_id);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);

}
#endif /* #if (SYS_CPNT_IPV6 == TRUE) */


/*------------------------------------------------------------------------------
 * ROUTINE NAME : L4_MGR_HandleIPCReqMsg
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Handle the ipc request message for csca mgr.
 * INPUT:
 *    ipcmsg_p  --  input request ipc message buffer
 *
 * OUTPUT:
 *    ipcmsg_p  --  output response ipc message buffer
 *
 * RETURN:
 *    TRUE  - There is a response need to send.
 *    FALSE - There is no response to send.
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_HandleIPCReqMsg(SYSFUN_Msg_T* ipcmsg_p)
{
    L4_MGR_IPCMsg_T *msg_data_p;
    UI32_T cmd;

    if(ipcmsg_p==NULL)
        return FALSE;

    msg_data_p = (L4_MGR_IPCMsg_T*)ipcmsg_p->msg_buf;
    cmd = msg_data_p->type.cmd;

    /* Every ipc request will fail when operating mode is transition mode
     */
    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_TRANSITION_MODE)
    {
        msg_data_p->type.result_ui32 = FALSE;
        ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
        goto exit;
    }

    switch(cmd)
    {
        case L4_MGR_IPCCMD_QOS_BINDPORT2POLICYMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_BindPort2PolicyMap(
                msg_data_p->data.pmapuport_inout_name.uport_ifindex,
                msg_data_p->data.pmapuport_inout_name.pmap_name,
                msg_data_p->data.pmapuport_inout_name.inout_dir);
        }
        break;

        case L4_MGR_IPCCMD_QOS_UNBINDPORTFROMPOLICYMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_UnBindPortFromPolicyMap(
                msg_data_p->data.pmapuport_inout_name.uport_ifindex,
                msg_data_p->data.pmapuport_inout_name.pmap_name,
                msg_data_p->data.pmapuport_inout_name.inout_dir);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETPOLICYMAPNAMEBYPORT:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.pmapuport_name);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetPolicyMapNameByPort(
                msg_data_p->data.pmapuport_name.uport_ifindex,
                msg_data_p->data.pmapuport_name.inout_dir,
                msg_data_p->data.pmapuport_name.pmap_name);
        }
        break;

#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
        case L4_MGR_IPCCMD_QOS_BINDPORT2_DYN_POLICYMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_BindPort2DynamicPolicyMap(
                msg_data_p->data.pmapuport_inout_name.uport_ifindex,
                msg_data_p->data.pmapuport_inout_name.pmap_name,
                msg_data_p->data.pmapuport_inout_name.inout_dir);
        }
        break;

        case L4_MGR_IPCCMD_QOS_UNBINDPORTFROM_DYN_POLICYMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_Qos_UnBindPortFromDynamicPolicyMap(
                msg_data_p->data.pmapuport_inout_name.uport_ifindex
                );
        }
        break;
#endif

        case L4_MGR_IPCCMD_QOS_GETRUNNINGPOLICYMAPNAMEBYPORT:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.pmapuport_name);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetRunningPolicyMapNameByPort(
                msg_data_p->data.pmapuport_name.uport_ifindex,
                msg_data_p->data.pmapuport_name.inout_dir,
                msg_data_p->data.pmapuport_name.pmap_name);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETPOLICYMAPIDBYNAME:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.pmapid_name);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetPolicyMapIdByName(
                msg_data_p->data.pmapid_name.pmap_name,
                &msg_data_p->data.pmapid_name.pmap_id);
        }
        break;
        case L4_MGR_IPCCMD_QOS_CREATEPOLICYMAPBYNAME:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_CreatePolicyMapByName(
                msg_data_p->data.pmapid_name.pmap_name);
        }
        break;

        case L4_MGR_IPCCMD_QOS_RENAMEPOLICYMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_RenamePolicyMap(
                msg_data_p->data.pmapname_rename.old_pmap_name,
                msg_data_p->data.pmapname_rename.new_pmap_name);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETPOLICYMAPDESCBYNAME:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_SetPolicyMapDescByName(
                msg_data_p->data.pmapname_desc.pmap_name,
                msg_data_p->data.pmapname_desc.description);
        }
        break;

        case L4_MGR_IPCCMD_QOS_DELPOLICYMAPBYNAME:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_DelPolicyMapByName(
                msg_data_p->data.pmapid_name.pmap_name);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTPOLICYMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.pmapid_ui);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetNextPolicyMap(
                &msg_data_p->data.pmapid_ui.pmap_id,
                &msg_data_p->data.pmapid_ui.pmap_ui);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTRUNNINGPOLICYMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.pmapid_ui);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetNextRunningPolicyMap(
                &msg_data_p->data.pmapid_ui.pmap_id,
                &msg_data_p->data.pmapid_ui.pmap_ui);
        }
        break;

        case L4_MGR_IPCCMD_QOS_ADDELEMENT2POLICYMAP:
        {
            RULE_TYPE_TBParamEntry_T    meter_null;
            RULE_TYPE_UI_Action_T       action_null;
            RULE_TYPE_TBParamEntry_T    *meter_p;
            RULE_TYPE_UI_Action_T       *action_p;

            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;

            memset (&meter_null,  0, sizeof (meter_null));
            memset (&action_null, 0, sizeof (action_null));

            meter_p  = &msg_data_p->data.pmap_element.meter_entry;
            action_p = &msg_data_p->data.pmap_element.action_entry;

            if (memcmp (meter_p, &meter_null, sizeof (meter_null)) == 0)
            {
                meter_p  = NULL;
            }

            if (memcmp (action_p, &action_null, sizeof (action_null)) == 0)
            {
                action_p = NULL;
            }

            msg_data_p->type.result_ui32=L4_MGR_QoS_AddElement2PolicyMap(
                 msg_data_p->data.pmap_element.pmap_name,
                 msg_data_p->data.pmap_element.cmap_name,
                 meter_p, action_p);
        }
        break;

        case L4_MGR_IPCCMD_QOS_DELELEMENTMETERFROMPOLICYMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_DelElementMeterFromPolicyMap(
                msg_data_p->data.pmap_element.pmap_name,
                msg_data_p->data.pmap_element.cmap_name,
                &msg_data_p->data.pmap_element.meter_entry);
        }
        break;

        case L4_MGR_IPCCMD_QOS_DELELEMENTACTIONFROMPOLICYMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_DelElementActionFromPolicyMap(
                msg_data_p->data.pmap_element.pmap_name,
                msg_data_p->data.pmap_element.cmap_name,
                &msg_data_p->data.pmap_element.action_entry);
        }
        break;

        case L4_MGR_IPCCMD_QOS_DELELEMENTFROMPOLICYMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_DelElementFromPolicyMap(
                msg_data_p->data.policy_class.pmap_name,
                msg_data_p->data.policy_class.cmap_name);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTELEMENTFROMPOLICYMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.pmapid_uiele);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetNextElementFromPolicyMap(
                msg_data_p->data.pmapid_uiele.pmap_id,
                &msg_data_p->data.pmapid_uiele.pele_ui);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTRUNNINGELEMENTFROMPOLICYMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.pmapid_uiele);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetNextRunningElementFromPolicyMap(
                msg_data_p->data.pmapid_uiele.pmap_id,
                &msg_data_p->data.pmapid_uiele.pele_ui);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETCLASSMAPIDBYNAME:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.cmapid_name);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetClassMapIdByName(
                msg_data_p->data.cmapid_name.cmap_name,
                &msg_data_p->data.cmapid_name.cmap_id);
        }
        break;

        case L4_MGR_IPCCMD_QOS_CREATECLASSMAPBYNAME:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_CreateClassMapByName(
                msg_data_p->data.cmapid_name.cmap_name);
        }
        break;

        case L4_MGR_IPCCMD_QOS_RENAMECLASSMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_RenameClassMap(
                msg_data_p->data.cmapname_rename.old_cmap_name,
                msg_data_p->data.cmapname_rename.new_cmap_name);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETCLASSMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_SetClassMap(
                &msg_data_p->data.cmapid_ui.cmap_ui);
        }
        break;

        case L4_MGR_IPCCMD_QOS_DELCLASSMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_DelClassMap(
                msg_data_p->data.cmapid_name.cmap_name);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETCLASSMAPBYNAME:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.cmapid_ui);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetClassMapByName(
                &msg_data_p->data.cmapid_ui.cmap_ui);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTCLASSMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.cmapid_ui);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetNextClassMap(
                &msg_data_p->data.cmapid_ui.cmap_id,
                &msg_data_p->data.cmapid_ui.cmap_ui);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTRUNNINGCLASSMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.cmapid_ui);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetNextRunningClassMap(
                &msg_data_p->data.cmapid_ui.cmap_id,
                &msg_data_p->data.cmapid_ui.cmap_ui);
        }
        break;

        case L4_MGR_IPCCMD_QOS_ADDELEMENT2CLASSMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_AddElement2ClassMap(
                msg_data_p->data.cmapname_uiele.cmap_name,
                &msg_data_p->data.cmapname_uiele.cele_ui);
        }
        break;

        case L4_MGR_IPCCMD_QOS_DELELEMENTFROMCLASSMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_DelElementFromClassMap(
                msg_data_p->data.cmapname_uiele.cmap_name,
                &msg_data_p->data.cmapname_uiele.cele_ui);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTELEMENTFROMCLASSMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.cmapid_eleid_uiele);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetNextElementFromClassMap(
                msg_data_p->data.cmapid_eleid_uiele.cmap_id,
                &msg_data_p->data.cmapid_eleid_uiele.eleid,
                &msg_data_p->data.cmapid_eleid_uiele.cele_ui);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTRUNNINGELEMENTFROMCLASSMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.cmapid_eleid_uiele);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetNextRunningElementFromClassMap(
                msg_data_p->data.cmapid_eleid_uiele.cmap_id,
                &msg_data_p->data.cmapid_eleid_uiele.eleid,
                &msg_data_p->data.cmapid_eleid_uiele.cele_ui);
        }
        break;

        case L4_MGR_IPCCMD_ACL_BINDPORT2ACL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_ACL_BindPort2Acl(
                    msg_data_p->data.acluport_type_name_flag.uport_ifindex,
                    msg_data_p->data.acluport_type_name_flag.acl_name,
                    msg_data_p->data.acluport_type_name_flag.acl_type,
                    msg_data_p->data.acluport_type_name_flag.ingress_flag,
                    msg_data_p->data.acluport_type_name_flag.time_range_name,
                    msg_data_p->data.acluport_type_name_flag.counter_enable);
        }
        break;

        case L4_MGR_IPCCMD_ACL_UNBINDPORTFROMACL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_ACL_UnBindPortFromAcl(
                msg_data_p->data.acluport_type_name_flag.uport_ifindex,
                msg_data_p->data.acluport_type_name_flag.acl_name,
                msg_data_p->data.acluport_type_name_flag.acl_type,
                msg_data_p->data.acluport_type_name_flag.ingress_flag);
        }
        break;

        case L4_MGR_IPCCMD_ACL_BIND_PORT_2_DYN_ACL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_ACL_BindPort2DynamicAcl(
                msg_data_p->data.acluport_type_name_flag.uport_ifindex,
                msg_data_p->data.acluport_type_name_flag.acl_name,
                msg_data_p->data.acluport_type_name_flag.acl_type,
                msg_data_p->data.acluport_type_name_flag.ingress_flag);
        }
        break;

        case L4_MGR_IPCCMD_ACL_UNBIND_PORT_FROM_DYN_ACL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_ACL_UnBindPortFromDynamicAcl(
                msg_data_p->data.acluport_type_name_flag.uport_ifindex,
                msg_data_p->data.acluport_type_name_flag.acl_type,
                msg_data_p->data.acluport_type_name_flag.ingress_flag);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GETNEXTACLBYPORT:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.acluport_id_ui);
            msg_data_p->type.result_ui32=L4_MGR_ACL_GetNextAclByPort(
                             msg_data_p->data.acluport_id_ui.uport_ifindex,
                             msg_data_p->data.acluport_id_ui.inout_dir,
                            &msg_data_p->data.acluport_id_ui.acl_id,
                            &msg_data_p->data.acluport_id_ui.acl_ui,
                            &msg_data_p->data.acluport_id_ui.precedence,
                            msg_data_p->data.acluport_id_ui.time_range_name,
                            &msg_data_p->data.acluport_id_ui.counter_enable);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GETNEXTUIACLBYPORTANDBINDINGTYPE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.acluport_id_ui);
            msg_data_p->type.result_ui32 = L4_MGR_ACL_GetNextUIAclByPortAndBindingType(
                msg_data_p->data.acluport_id_ui.precedence,
                &msg_data_p->data.acluport_id_ui.uport_ifindex,
                &msg_data_p->data.acluport_id_ui.acl_ui,
                msg_data_p->data.acluport_id_ui.time_range_name
                );
        }
        break;

        case L4_MGR_IPCCMD_ACL_GETNEXTRUNNINGACLBYPORT:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.acluport_id_ui);
            msg_data_p->type.result_ui32=L4_MGR_ACL_GetNextRunningAclByPort(
                             msg_data_p->data.acluport_id_ui.uport_ifindex,
                             msg_data_p->data.acluport_id_ui.inout_dir,
                            &msg_data_p->data.acluport_id_ui.acl_id,
                            &msg_data_p->data.acluport_id_ui.acl_ui,
                            &msg_data_p->data.acluport_id_ui.precedence,
                            msg_data_p->data.acluport_id_ui.time_range_name,
                            &msg_data_p->data.acluport_id_ui.counter_enable);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GETACLIDBYNAME:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.aclid_type_name);

            msg_data_p->type.result_ui32=L4_MGR_ACL_GetAclIdByName(
                msg_data_p->data.aclid_type_name.acl_name,
                &msg_data_p->data.aclid_type_name.acl_id);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GETACLIDBYNAMEANDTYPE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.aclid_type_name);

            msg_data_p->type.result_ui32=L4_MGR_ACL_GetAclIdByNameAndType(
                 msg_data_p->data.aclid_type_name.acl_name,
                 msg_data_p->data.aclid_type_name.acl_type,
                &msg_data_p->data.aclid_type_name.acl_id);
        }
        break;

        case L4_MGR_IPCCMD_ACL_CREATEACL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_ACL_CreateAcl(
                msg_data_p->data.aclid_type_name.acl_name,
                msg_data_p->data.aclid_type_name.acl_type);
        }
        break;

        case L4_MGR_IPCCMD_ACL_DELACLBYNAMEANDACLTYPE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_ACL_DelAclByNameAndAclType(
                msg_data_p->data.aclid_type_name.acl_name,
                msg_data_p->data.aclid_type_name.acl_type);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GETNEXTACL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.aclid_ui);
            msg_data_p->type.result_ui32=L4_MGR_ACL_GetNextAcl(
                &msg_data_p->data.aclid_ui.acl_id,
                &msg_data_p->data.aclid_ui.acl_ui);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GETNEXTRUNNINGACL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.aclid_ui);
            msg_data_p->type.result_ui32=L4_MGR_ACL_GetNextRunningAcl(
                &msg_data_p->data.aclid_ui.acl_id,
                &msg_data_p->data.aclid_ui.acl_ui);
        }
        break;

        case L4_MGR_IPCCMD_ACL_INITACE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.ace_entry);
            msg_data_p->type.result_ui32=L4_MGR_ACL_InitAce(
                &msg_data_p->data.ace_entry);
        }
        break;

        case L4_MGR_IPCCMD_ACL_INIT_UI_ACE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
            +sizeof(msg_data_p->data.ui_ace_entry);
            msg_data_p->type.result_ui32=L4_MGR_ACL_InitUIAce(
                &msg_data_p->data.ui_ace_entry);
        }
        break;

        case L4_MGR_IPCCMD_ACL_SET_ACE_PATTERN_TO_DEFAULT_BY_TYPE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE +
                                     sizeof(msg_data_p->data.ace_entry_type);
            msg_data_p->type.result_ui32 = L4_MGR_ACL_SetAcePatternToDefaultByType(
                &msg_data_p->data.ace_entry_type.ace_entry,
                msg_data_p->data.ace_entry_type.ace_type);
        }
        break;

        case L4_MGR_IPCCMD_ACL_SET_UI_ACE_2_ACL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_ACL_SetUIAce2Acl(
                msg_data_p->data.acltype_name_uiace.acl_name,
                msg_data_p->data.acltype_name_uiace.acl_type,
                &msg_data_p->data.acltype_name_uiace.ace);
        }
        break;

        case L4_MGR_IPCCMD_ACL_DEL_UI_ACE_FROM_ACL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_ACL_DelUIAceFromAcl(
                msg_data_p->data.acltype_name_uiace.acl_name,
                msg_data_p->data.acltype_name_uiace.acl_type,
                &msg_data_p->data.acltype_name_uiace.ace);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GET_NEXT_UI_ACE_BY_ACL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE +
                sizeof(msg_data_p->data.aclid_aceid_uiace);
            msg_data_p->type.result_ui32=L4_MGR_ACL_GetNextUIAceByAcl(
                msg_data_p->data.aclid_aceid_uiace.acl_id,
                &msg_data_p->data.aclid_aceid_uiace.ace_id,
                &msg_data_p->data.aclid_aceid_uiace.ace);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GET_NEXT_HARDWARD_UI_ACE_BY_ACL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE +
                sizeof(msg_data_p->data.port_dir_aclid_aceid_uiace);
            msg_data_p->type.result_ui32=L4_MGR_ACL_GetNextHardwareUIAceByAcl(
                msg_data_p->data.port_dir_aclid_aceid_uiace.uport_ifindex,
                msg_data_p->data.port_dir_aclid_aceid_uiace.inout_profile,
                msg_data_p->data.port_dir_aclid_aceid_uiace.acl_id,
                &msg_data_p->data.port_dir_aclid_aceid_uiace.ace_id,
                &msg_data_p->data.port_dir_aclid_aceid_uiace.ace);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GET_NEXT_RUNNING_UI_ACE_BY_ACL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE +
                sizeof(msg_data_p->data.aclid_aceid_uiace);
            msg_data_p->type.result_ui32=L4_MGR_ACL_GetNextRunningUIAceByAcl(
                msg_data_p->data.aclid_aceid_uiace.acl_id,
                &msg_data_p->data.aclid_aceid_uiace.ace_id,
                &msg_data_p->data.aclid_aceid_uiace.ace);
        }
        break;

        case L4_MGR_IPCCMD_ACL_SETACE2ACL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_ACL_SetAce2Acl(
                 msg_data_p->data.acltype_name_ace.acl_name,
                 msg_data_p->data.acltype_name_ace.acl_type,
                &msg_data_p->data.acltype_name_ace.ace_entry);
        }
        break;

        case L4_MGR_IPCCMD_ACL_DELACEFROMACL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_ACL_DelAceFromAcl(
                 msg_data_p->data.acltype_name_ace.acl_name,
                 msg_data_p->data.acltype_name_ace.acl_type,
                &msg_data_p->data.acltype_name_ace.ace_entry);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GETNEXTACEBYACL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.aclid_aceid_ace);
            msg_data_p->type.result_ui32=L4_MGR_ACL_GetNextAceByAcl(
                 msg_data_p->data.aclid_aceid_ace.acl_id,
                &msg_data_p->data.aclid_aceid_ace.ace_id,
                &msg_data_p->data.aclid_aceid_ace.ace_entry);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GET_NEXT_HARDWARE_ACE_BY_ACL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                            +sizeof(msg_data_p->data.port_dir_aclid_aceid_ace);
            msg_data_p->type.result_ui32 = L4_MGR_ACL_GetNextHardwareAceByAcl(
                        msg_data_p->data.port_dir_aclid_aceid_ace.uport_ifindex,
                        msg_data_p->data.port_dir_aclid_aceid_ace.inout_profile,
                        msg_data_p->data.port_dir_aclid_aceid_ace.acl_id,
                        &msg_data_p->data.port_dir_aclid_aceid_ace.ace_id,
                        &msg_data_p->data.port_dir_aclid_aceid_ace.ace_entry);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GET_NEXT_MIB_ACE_BY_PORT:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                            +sizeof(msg_data_p->data.port_dir_aclid_aceid_ace_counterenable);
            msg_data_p->type.result_ui32 = L4_MGR_ACL_GetNextMibAceByPort(
                        msg_data_p->data.port_dir_aclid_aceid_ace_counterenable.uport_ifindex,
                        msg_data_p->data.port_dir_aclid_aceid_ace_counterenable.inout_profile,
                        &msg_data_p->data.port_dir_aclid_aceid_ace_counterenable.acl_id,
                        &msg_data_p->data.port_dir_aclid_aceid_ace_counterenable.ace_id,
                        &msg_data_p->data.port_dir_aclid_aceid_ace_counterenable.ace_entry,
                        &msg_data_p->data.port_dir_aclid_aceid_ace_counterenable.counter_enable);
        }
        break;

#if (SYS_CPNT_ACL_COUNTER == TRUE)
        case L4_MGR_IPCCMD_ACL_CLEAR_ACL_COUNTER:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                            +sizeof(msg_data_p->data.clear_acl_counter);
            msg_data_p->type.result_ui32 = L4_MGR_ACL_ClearAclCounter(msg_data_p->data.clear_acl_counter.acl_name,
                                                                      msg_data_p->data.clear_acl_counter.ace_id,
                                                                      &msg_data_p->data.clear_acl_counter.interface);
        }
        break;
#endif /* #if (SYS_CPNT_ACL_COUNTER == TRUE) */

#if (SYS_CPNT_ACL_MIRROR == TRUE)
        case L4_MGR_IPCCMD_ACL_ADD_ACL_MIRROR_ENTRY:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE +
                                        sizeof(msg_data_p->data.port_aclname);
            msg_data_p->type.result_ui32 = L4_MGR_ACL_AddAclMirrorEntry(
                                    msg_data_p->data.port_aclname.uport_ifindex,
                                        msg_data_p->data.port_aclname.acl_name);
        }
        break;

        case L4_MGR_IPCCMD_ACL_REMOVE_ACL_MIRROR_ENTRY:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE +
                                        sizeof(msg_data_p->data.port_aclname);
            msg_data_p->type.result_ui32 = L4_MGR_ACL_RemoveAclMirrorEntry(
                                   msg_data_p->data.port_aclname.uport_ifindex,
                                        msg_data_p->data.port_aclname.acl_name);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GET_NEXT_ACL_MIRROR_ENTRY:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE +
                                   sizeof(msg_data_p->data.port_aclid_aclentry);
            msg_data_p->type.result_ui32 = L4_MGR_ACL_GetNextAclMirrorEntry(
                            &msg_data_p->data.port_aclid_aclentry.uport_ifindex,
                                &msg_data_p->data.port_aclid_aclentry.acl_index,
                               &msg_data_p->data.port_aclid_aclentry.acl_entry);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GET_NEXT_RUNNING_ACL_MIRROR_ENTRY:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE +
            sizeof(msg_data_p->data.port_aclid_aclentry);
            msg_data_p->type.result_ui32 = L4_MGR_ACL_GetNextRunningAclMirrorEntry(
                            &msg_data_p->data.port_aclid_aclentry.uport_ifindex,
                                &msg_data_p->data.port_aclid_aclentry.acl_index,
                               &msg_data_p->data.port_aclid_aclentry.acl_entry);
        }
        break;

#endif /* #if (SYS_CPNT_ACL_MIRROR == TRUE) */

        case L4_MGR_IPCCMD_ACL_GETTCAMUTILIZATION:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.tcam_entry_old);
            msg_data_p->type.result_bool=L4_MGR_GetTCAMUtilization(
                &msg_data_p->data.tcam_entry_old);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GET_TCAM_ENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.tcam_entry);
            msg_data_p->type.result_bool=L4_MGR_GetTcamEntry(
                &msg_data_p->data.tcam_entry);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GET_NEXT_TCAM_ENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.tcam_entry);
            msg_data_p->type.result_bool=L4_MGR_GetNextTcamEntry(
                &msg_data_p->data.tcam_entry);
        }
        break;

        case L4_MGR_IPCCMD_ACL_GET_SUPPORTED_TCAM_CAPABILITY_BITMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.tcam_entry);
            msg_data_p->type.result_ui32=L4_MGR_GetSupportedTcamCapabilityBitMap(
                &msg_data_p->data.tcam_entry.cap_bitmap);
        }
        break;

/* ========================================================================== */
#if (SYS_CPNT_COS == TRUE)
        case L4_MGR_IPCCMD_COS_SETIPPRECEDENCEDSCPMAPPINGSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_COS_SetIpPrecedenceDscpMappingStatus(
                msg_data_p->data.ui32_v);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETIPPRECEDENCEDSCPMAPPINGSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.ui32_v);
            msg_data_p->type.result_bool=L4_MGR_COS_GetIpPrecedenceDscpMappingStatus(
                (COS_VM_MAPPING_FLAG_T *)&msg_data_p->data.ui32_v);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETRUNNINGIPPRECEDENCEDSCPMAPPINGSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.ui32_v);
            msg_data_p->type.result_ui32=L4_MGR_COS_GetRunningIpPrecedenceDscpMappingStatus(
              (COS_VM_MAPPING_FLAG_T *) &msg_data_p->data.ui32_v);
        }
        break;

        case L4_MGR_IPCCMD_COS_SETTCPPORTMAPPINGSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_COS_SetTcpPortMappingStatus(
                msg_data_p->data.ui32_v);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETTCPPORTMAPPINGSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.ui32_v);
            msg_data_p->type.result_bool=L4_MGR_COS_GetTcpPortMappingStatus(
                &msg_data_p->data.ui32_v);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETRUNNINGTCPPORTMAPPINGSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.ui32_v);
            msg_data_p->type.result_ui32=L4_MGR_COS_GetRunningTcpPortMappingStatus(
                (COS_VM_TCPPORT_MAPPING_FLAG_T *)&msg_data_p->data.ui32_v);
        }
        break;

        case L4_MGR_IPCCMD_COS_SETIPPRECEDENCETOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_COS_SetIpPrecedenceToCos(
                msg_data_p->data.port_tos_cos.l_port,
                msg_data_p->data.port_tos_cos.tos,
                msg_data_p->data.port_tos_cos.cos);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETIPPRECEDENCETOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_tos_cos);
            msg_data_p->type.result_bool=L4_MGR_COS_GetIpPrecedenceToCos(
                msg_data_p->data.port_tos_cos.l_port,
                msg_data_p->data.port_tos_cos.tos,
                &msg_data_p->data.port_tos_cos.cos);
        }
        break;

        case L4_MGR_IPCCMD_COS_DELIPPRECEDENCETOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_COS_DelIpPrecedenceToCos(
                msg_data_p->data.port_tos.l_port,
                msg_data_p->data.port_tos.tos);
        }
        break;

        case L4_MGR_IPCCMD_COS_DELIPPRECEDENCELPORTALLENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_COS_DelIpPrecedenceLportAllEntry(
                msg_data_p->data.ui32_v);
        }
        break;

        case L4_MGR_IPCCMD_COS_SETDSCPTOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_COS_SetDSCPToCos(
                msg_data_p->data.port_dscp_cos.l_port,
                msg_data_p->data.port_dscp_cos.dscp,
                msg_data_p->data.port_dscp_cos.cos);
        }
        break;

        case L4_MGR_IPCCMD_COS_SETDSCPTOCOSSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_COS_SetDSCPToCosStatus(
                msg_data_p->data.port_dscp_status.l_port,
                msg_data_p->data.port_dscp_status.dscp,
                msg_data_p->data.port_dscp_status.status);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETDSCPTOCOSSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_dscp_status);
            msg_data_p->type.result_bool=L4_MGR_COS_GetDSCPToCosStatus(
                msg_data_p->data.port_dscp_status.l_port,
                msg_data_p->data.port_dscp_status.dscp,
                &msg_data_p->data.port_dscp_status.status);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETRUNNINGDSCPTOCOSSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_dscp_status);
            msg_data_p->type.result_ui32=L4_MGR_COS_GetRunningDSCPToCosStatus(
                msg_data_p->data.port_dscp_status.l_port,
                msg_data_p->data.port_dscp_status.dscp,
                &msg_data_p->data.port_dscp_status.status);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETDSCPTOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_dscp_cos);
            msg_data_p->type.result_bool=L4_MGR_COS_GetDSCPToCos(
                msg_data_p->data.port_dscp_cos.l_port,
                msg_data_p->data.port_dscp_cos.dscp,
                &msg_data_p->data.port_dscp_cos.cos);
        }
        break;

        case L4_MGR_IPCCMD_COS_DELDSCPTOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_COS_DelDSCPToCos(
                msg_data_p->data.port_dscp.l_port,
                msg_data_p->data.port_dscp.dscp);
        }
        break;

        case L4_MGR_IPCCMD_COS_DELDSCPLPORTALLENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_COS_DelDscpLportAllEntry(
                msg_data_p->data.ui32_v);
        }
        break;

        case L4_MGR_IPCCMD_COS_SETPORTTOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_COS_SetPortToCos(
                msg_data_p->data.port_tcp_cos.l_port,
                msg_data_p->data.port_tcp_cos.tcp_port,
                msg_data_p->data.port_tcp_cos.cos);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETPORTTOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_tcp_cos);
            msg_data_p->type.result_bool=L4_MGR_COS_GetPortToCos(
                msg_data_p->data.port_tcp_cos.l_port,
                (UI16_T)msg_data_p->data.port_tcp_cos.tcp_port,
                &msg_data_p->data.port_tcp_cos.cos);
        }
        break;

        case L4_MGR_IPCCMD_COS_DELPORTTOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_COS_DelPortToCos(
                msg_data_p->data.port_tcp.l_port,
                msg_data_p->data.port_tcp.tcp_port);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETNEXTIPPRECEDENCETOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_tos_cos);
            msg_data_p->type.result_bool=L4_MGR_COS_GetNextIpPrecedenceToCos(
                &msg_data_p->data.port_tos_cos.l_port,
                &msg_data_p->data.port_tos_cos.tos,
                &msg_data_p->data.port_tos_cos.cos);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETNEXTDSCPTOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_dscp_cos);
            msg_data_p->type.result_bool=L4_MGR_COS_GetNextDSCPToCos(
                &msg_data_p->data.port_dscp_cos.l_port,
                &msg_data_p->data.port_dscp_cos.dscp,
                &msg_data_p->data.port_dscp_cos.cos);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETNEXTPORTTOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_tcp_cos);
            msg_data_p->type.result_bool=L4_MGR_COS_GetNextPortToCos(
                &msg_data_p->data.port_tcp_cos.l_port,
                &msg_data_p->data.port_tcp_cos.tcp_port,
                &msg_data_p->data.port_tcp_cos.cos);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETRUNNINGIPPRECEDENCETOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_tos_cos);
            msg_data_p->type.result_ui32=L4_MGR_COS_GetRunningIpPrecedenceToCos(
                msg_data_p->data.port_tos_cos.l_port,
                msg_data_p->data.port_tos_cos.tos,
                &msg_data_p->data.port_tos_cos.cos);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETRUNNINGDSCPTOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_dscp_cos);
            msg_data_p->type.result_ui32=L4_MGR_COS_GetRunningDSCPToCos(
                msg_data_p->data.port_dscp_cos.l_port,
                msg_data_p->data.port_dscp_cos.dscp,
                &msg_data_p->data.port_dscp_cos.cos);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETRUNNINGPORTTOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_tcp_cos);
            msg_data_p->type.result_ui32=L4_MGR_COS_GetRunningPortToCos(
                msg_data_p->data.port_tcp_cos.l_port,
                msg_data_p->data.port_tcp_cos.tcp_port,
                &msg_data_p->data.port_tcp_cos.cos);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETNEXTRUNNINGIPPRECEDENCETOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_tos_cos);
            msg_data_p->type.result_ui32=L4_MGR_COS_GetNextRunningIpPrecedenceToCos(
                &msg_data_p->data.port_tos_cos.l_port,
                &msg_data_p->data.port_tos_cos.tos,
                &msg_data_p->data.port_tos_cos.cos);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETNEXTRUNNINGDSCPTOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_dscp_cos);
            msg_data_p->type.result_ui32=L4_MGR_COS_GetNextRunningDSCPToCos(
                &msg_data_p->data.port_dscp_cos.l_port,
                &msg_data_p->data.port_dscp_cos.dscp,
                &msg_data_p->data.port_dscp_cos.cos);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETNEXTRUNNINGPORTTOCOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_tcp_cos);
            msg_data_p->type.result_ui32=L4_MGR_COS_GetNextRunningPortToCos(
                &msg_data_p->data.port_tcp_cos.l_port,
                &msg_data_p->data.port_tcp_cos.tcp_port,
                &msg_data_p->data.port_tcp_cos.cos);
        }
        break;

        case L4_MGR_IPCCMD_COS_PORTCOPY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_COS_PortCopy(
                msg_data_p->data.maptype_port_portlist.map_table_type,
                msg_data_p->data.maptype_port_portlist.src_port,
                msg_data_p->data.maptype_port_portlist.dst_port_list);
        }
        break;

        case L4_MGR_IPCCMD_COS_ADDCOSENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_COS_AddCosEntry(
                msg_data_p->data.ifindex_cosentry.ifindex,
                &msg_data_p->data.ifindex_cosentry.cos_entry);
        }
        break;

        case L4_MGR_IPCCMD_COS_DELCOSENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_COS_DelCosEntry(
                msg_data_p->data.ifindex_cosentry.ifindex,
                &msg_data_p->data.ifindex_cosentry.cos_entry);
        }
        break;

        case L4_MGR_IPCCMD_COS_SETCOSENTRYROWSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_COS_SetCosEntryRowStatus(
                msg_data_p->data.ifindex_cosentry.ifindex,
                &msg_data_p->data.ifindex_cosentry.cos_entry);
        }
        break;

        case L4_MGR_IPCCMD_COS_SETCOSENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_COS_SetCosEntry(
                msg_data_p->data.ifindex_cosentry.ifindex,
                &msg_data_p->data.ifindex_cosentry.cos_entry);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETCOSENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.ifindex_cosentry);
            msg_data_p->type.result_bool=L4_MGR_COS_GetCosEntry(
                msg_data_p->data.ifindex_cosentry.ifindex,
                &msg_data_p->data.ifindex_cosentry.cos_entry);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETNEXTCOSENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.ifindex_cosentry);
            msg_data_p->type.result_bool=L4_MGR_COS_GetNextCosEntry(
                msg_data_p->data.ifindex_cosentry.ifindex,
                &msg_data_p->data.ifindex_cosentry.cos_entry);
        }
        break;

        case L4_MGR_IPCCMD_COS_SNMPGETNEXTCOSENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.ifindex_cosentry);
            msg_data_p->type.result_bool=L4_MGR_COS_SNMPGetNextCosEntry(
                &msg_data_p->data.ifindex_cosentry.ifindex,
                &msg_data_p->data.ifindex_cosentry.cos_entry);
        }
        break;

        case L4_MGR_IPCCMD_COS_GETNEXTRUNNINGCOSENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.ifindex_cosentry);
            msg_data_p->type.result_ui32=L4_MGR_COS_GetNextRunningCosEntry(
                msg_data_p->data.ifindex_cosentry.ifindex,
                &msg_data_p->data.ifindex_cosentry.cos_entry);
        }
        break;
#endif // #if (SYS_CPNT_COS == TRUE)
/* ========================================================================== */


#if(SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)
        case L4_MGR_IPCCMD_MARKER_SETMARKERENTRY_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_MARKER_SetMarkerEntry_Dscp(
                msg_data_p->data.index_aclname_value.ifindex,
                msg_data_p->data.index_aclname_value.acl_name,
                msg_data_p->data.index_aclname_value.value);
        }
        break;
#endif
        case L4_MGR_IPCCMD_QOS_GETDIFFSERVPORTENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.ifindex_portui);
            msg_data_p->type.result_bool=L4_MGR_QoS_GetDiffServPortEntry(
                 msg_data_p->data.ifindex_portui.ifindex,
                 msg_data_p->data.ifindex_portui.inout_dir,
                &msg_data_p->data.ifindex_portui.port_ui);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVPORTENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.ifindex_portui);
            msg_data_p->type.result_bool=L4_MGR_QoS_GetNextDiffServPortEntry(
                &msg_data_p->data.ifindex_portui.ifindex,
                &msg_data_p->data.ifindex_portui.inout_dir,
                &msg_data_p->data.ifindex_portui.port_ui);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVPORTPOLICYMAPINDEX:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServPortPolicyMapIndex(
                msg_data_p->data.ui32a1_ui32a2_ui32a3.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2_ui32a3.ui32_a2,
                msg_data_p->data.ui32a1_ui32a2_ui32a3.ui32_a3);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVPORTINGRESSIPACLINDEX:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServPortIngressIpAclIndex(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVPORTINGRESSIPV6ACLINDEX:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServPortIngressIpv6AclIndex(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVPORTINGRESSMACACLINDEX:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServPortIngressMacAclIndex(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETDIFFSERVPOLICYMAPENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.pmapid_mib);
            msg_data_p->type.result_bool=L4_MGR_QoS_GetDiffServPolicyMapEntry(
                 msg_data_p->data.pmapid_mib.pmap_id,
                &msg_data_p->data.pmapid_mib.pmap_mib);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVPOLICYMAPENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.pmapid_mib);
            msg_data_p->type.result_bool=L4_MGR_QoS_GetNextDiffServPolicyMapEntry(
                &msg_data_p->data.pmapid_mib.pmap_id,
                &msg_data_p->data.pmapid_mib.pmap_mib);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVPOLICYMAPNAME:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServPolicyMapName(
                msg_data_p->data.pmapid_name.pmap_id,
                msg_data_p->data.pmapid_name.pmap_name);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVPOLICYMAPDESCRIPTION:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServPolicyMapDescription(
                msg_data_p->data.pmapid_desc.pmap_id,
                msg_data_p->data.pmapid_desc.pmap_desc);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVPOLICYMAPSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServPolicyMapStatus(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETPOLICYDIFFSERVPOLICYMAPATTACHCTLACTION:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetPolicydiffServPolicyMapAttachCtlAction(
                msg_data_p->data.ui32a1_ui32a2_ui32a3.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2_ui32a3.ui32_a2,
                msg_data_p->data.ui32a1_ui32a2_ui32a3.ui32_a3);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVPOLICYMAPELEMENTENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.pmapeleid_ele);
            msg_data_p->type.result_bool=L4_MGR_QoS_GetNextDiffServPolicyMapElementEntry(
                &msg_data_p->data.pmapeleid_ele.pele_id,
                &msg_data_p->data.pmapeleid_ele.pele);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVPOLICYMAPELEMENTCLASSMAPINDEX:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServPolicyMapElementClassMapIndex(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVPOLICYMAPELEMENTMETERINDEX:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVPOLICYMAPELEMENTACTIONINDEX:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServPolicyMapElementActionIndex(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVPOLICYMAPELEMENTSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServPolicyMapElementStatus(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETDIFFSERVCLASSMAPENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.cmapid_mib);
            msg_data_p->type.result_bool=L4_MGR_QoS_GetDiffServClassMapEntry(
                 msg_data_p->data.cmapid_mib.cmap_id,
                &msg_data_p->data.cmapid_mib.cmap_mib);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVCLASSMAPENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.cmapid_mib);
            msg_data_p->type.result_bool=L4_MGR_QoS_GetNextDiffServClassMapEntry(
                &msg_data_p->data.cmapid_mib.cmap_id,
                &msg_data_p->data.cmapid_mib.cmap_mib);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVCLASSMAPNAME:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServClassMapName(
                msg_data_p->data.cmapid_name.cmap_id,
                msg_data_p->data.cmapid_name.cmap_name);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVCLASSMAPDESCRIPTION:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServClassMapDescription(
                msg_data_p->data.cmapid_desc.cmap_id,
                msg_data_p->data.cmapid_desc.cmap_desc);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVCLASSMAPMATCHTYPE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServClassMapMatchType(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVCLASSMAPSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServClassMapStatus(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVCLASSMAPATTACHCTLACTION:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServClassMapAttachCtlAction(
                msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a1,
                msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a2,
                msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a3,
                msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a4);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETDIFFSERVACLENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.aclid_mib);
            msg_data_p->type.result_bool=L4_MGR_QoS_GetDiffServAclEntry(
                 msg_data_p->data.aclid_mib.acl_id,
                &msg_data_p->data.aclid_mib.acl_mib);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVACLENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.aclid_mib);
            msg_data_p->type.result_bool=L4_MGR_QoS_GetNextDiffServAclEntry(
                &msg_data_p->data.aclid_mib.acl_id,
                &msg_data_p->data.aclid_mib.acl_mib);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVACLTYPE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServAclType(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVACLNAME:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServAclName(
                msg_data_p->data.aclid_type_name.acl_id,
                msg_data_p->data.aclid_type_name.acl_name);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVACLSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServAclStatus(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVACLATTACHCTLACTION:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServAclAttachCtlAction(
                msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a1,
                msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a2,
                msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a3,
                msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a4);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETDIFFSERVIPACEENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.aceid_uiace);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetDiffServIpAceEntry(
                 msg_data_p->data.aceid_uiace.ace_id,
                &msg_data_p->data.aceid_uiace.ace);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVIPACEENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.aceid_uiace);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetNextDiffServIpAceEntry(
                &msg_data_p->data.aceid_uiace.ace_id,
                &msg_data_p->data.aceid_uiace.ace);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACETYPE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceType(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEACCESS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceAccess(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACESOURCEIPADDR:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceSourceIpAddr(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACESOURCEIPADDRBITMASK:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceSourceIpAddrBitmask(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEDESTIPADDR:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceDestIpAddr(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEDESTIPADDRBITMASK:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceDestIpAddrBitmask(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEPROTOCOL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceProtocol(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEPREC:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAcePrec(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACETOS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceTos(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEDSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceDscp(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACESOURCEPORTOP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceSourcePortOp(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEMINSOURCEPORT:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceMinSourcePort(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEMAXSOURCEPORT:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceMaxSourcePort(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACESOURCEPORTBITMASK:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceSourcePortBitmask(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEDESTPORTOP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceDestPortOp(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEMINDESTPORT:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceMinDestPort(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEMAXDESTPORT:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceMaxDestPort(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEDESTPORTBITMASK:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceDestPortBitmask(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACECONTROLCODE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceControlCode(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACECONTROLCODEBITMASK:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceControlCodeBitmask(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACESTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceStatus(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_IP_ACE_TIME_RANGE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpAceTimeRange(
                msg_data_p->data.ace_timerange.ace_id,
                msg_data_p->data.ace_timerange.time_range_name);
        }
        break;
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */

        case L4_MGR_IPCCMD_QOS_GETDIFFSERVMACACEENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.aceid_uiace);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetDiffServMacAceEntry(
                msg_data_p->data.aceid_uiace.ace_id,
                &msg_data_p->data.aceid_uiace.ace);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVMACACEENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.aceid_uiace);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetNextDiffServMacAceEntry(
                &msg_data_p->data.aceid_uiace.ace_id,
                &msg_data_p->data.aceid_uiace.ace);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEACCESS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceAccess(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEPKTFORMAT:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAcePktformat(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACESOURCEMACADDR:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceSourceMacAddr(
                msg_data_p->data.aceid_mac.ace_id,
                msg_data_p->data.aceid_mac.mac);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACESOURCEMACADDRBITMASK:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceSourceMacAddrBitmask(
                msg_data_p->data.aceid_mac.ace_id,
                msg_data_p->data.aceid_mac.mac);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEDESTMACADDR:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceDestMacAddr(
                msg_data_p->data.aceid_mac.ace_id,
                msg_data_p->data.aceid_mac.mac);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEDESTMACADDRBITMASK:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceDestMacAddrBitmask(
                msg_data_p->data.aceid_mac.ace_id,
                msg_data_p->data.aceid_mac.mac);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEVIDOP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceVidOp(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEMINVID:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceMinVid(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEVIDBITMASK:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceVidBitmask(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEMAXVID:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceMaxVid(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEETHERTYPEOP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceEtherTypeOp(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEETHERTYPEBITMASK:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceEtherTypeBitmask(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEMINETHERTYPE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceMinEtherType(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEMAXETHERTYPE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceMaxEtherType(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACESTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceStatus(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_TIME_RANGE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceTimeRange(
                msg_data_p->data.ace_timerange.ace_id,
                msg_data_p->data.ace_timerange.time_range_name);
        }
        break;
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IP_SOURCE_ADDR:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceSourceIpAddr(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IP_SOURCE_MASK:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceSourceIpAddrBitmask(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IP_DEST_ADDR:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceDestIpAddr(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IP_DEST_MASK:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceDestIpAddrBitmask(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IPV6_SOURCE_ADDR:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMaceAceSourceIpv6Addr(
                msg_data_p->data.ipv6aceindex_ip.ipv6_ace_index,
                msg_data_p->data.ipv6aceindex_ip.ip);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IPV6_SOURCE_PREFIX_LEN:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceSourceIpv6AddrPrefix(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IPV6_DEST_ADDR:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceDestIpv6Addr(
                msg_data_p->data.ipv6aceindex_ip.ipv6_ace_index,
                msg_data_p->data.ipv6aceindex_ip.ip);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IPV6_DEST_PREFIX_LEN:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceDestIpv6AddrPrefix(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IP_PROTOCOL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceProtocol(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_L4_SOURCE_PORT_OP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceSourcePortOp(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_L4_SOURCE_PORT:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceSourcePort(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_L4_SOURCE_PORT_MASK:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceSourcePortBitmask(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_L4_DEST_PORT_OP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceDestPortOp(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_L4_DEST_PORT:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceDestPort(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_L4_DEST_PORT_MASK:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMacAceDestPortBitmask(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETDIFFSERVIPV6ACEENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.aceid_uiace);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetDiffServIpv6AceEntry(
                 msg_data_p->data.aceid_uiace.ace_id,
                &msg_data_p->data.aceid_uiace.ace);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVIPV6ACEENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.aceid_uiace);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetNextDiffServIpv6AceEntry(
                &msg_data_p->data.aceid_uiace.ace_id,
                &msg_data_p->data.aceid_uiace.ace);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACETYPE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceType(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACEACCESS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceAccess(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACESOURCEIPADDR:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceSourceIpAddr(
                msg_data_p->data.ipv6aceindex_ip.ipv6_ace_index,
                msg_data_p->data.ipv6aceindex_ip.ip);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACESOURCEIPADDRPREFIX:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceSourceIpAddrPrefix(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACEDESTIPADDR:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceDestIpAddr(
                msg_data_p->data.ipv6aceindex_ip.ipv6_ace_index,
                msg_data_p->data.ipv6aceindex_ip.ip);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACEDESTIPADDRPREFIX:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceDestIpAddrPrefix(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACENEXTHEADER:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceNextHeader(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACEDSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceDscp(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACEFLOWLABEL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceFlowLabel(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACESTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceStatus(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_IPV6_ACE_TIME_RANGE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceTimeRange(
                msg_data_p->data.ace_timerange.ace_id,
                msg_data_p->data.ace_timerange.time_range_name);
        }
        break;
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */

#if (SYS_CPNT_ACL_IPV6_EXT_TCP_UDP_PORT == TRUE)
        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_IPV6_ACE_SOURCE_PORT_OP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceSourcePortOp(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_IPV6_ACE_SOURCE_PORT:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceSourcePort(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_IPV6_ACE_SOURCE_PORT_BITMASK:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceSourcePortBitmask(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_IPV6_ACE_DEST_PORT_OP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceDestPortOp(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_IPV6_ACE_DEST_PORT:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceDestPort(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_IPV6_ACE_DEST_PORT_BITMASK:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServIpv6AceDestPortBitmask(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;
#endif /* SYS_CPNT_ACL_IPV6_EXT_TCP_UDP_PORT */

        case L4_MGR_IPCCMD_QOS_GETDIFFSERVACTIONENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.actid_mib);
            msg_data_p->type.result_bool=L4_MGR_QoS_GetDiffServActionEntry(
                 msg_data_p->data.actid_mib.act_id,
                &msg_data_p->data.actid_mib.act_mib);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVACTIONENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.actid_mib);
            msg_data_p->type.result_bool=L4_MGR_QoS_GetNextDiffServActionEntry(
                &msg_data_p->data.actid_mib.act_id,
                &msg_data_p->data.actid_mib.act_mib);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONBITMAP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServActionBitmap(
                msg_data_p->data.actid_bmp.act_id,
                msg_data_p->data.actid_bmp.act_bmp);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONPKTNEWPRI:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServActionPktNewPri(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONPKTNEWPHB:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServActionPktNewPhb(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONPKTNEWIPPREC:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServActionPktNewIpPrec(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONPKTNEWDSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServActionPktNewDscp(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONREDPKTNEWDSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServActionRedPktNewDscp(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;
       /*zhangwei add*/
         case L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONYELLOWPKTNEWDSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServActionYellowPktNewDscp(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_ACTION_GREEN_PKT_NEW_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServActionGreenPktNewDscp(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_ACTION_GREEN_TRANSMIT:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServActionGreenTransmit(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONYELLOWDROP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServActionYellowDrop(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONREDDROP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServActionRedDrop(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SET_DIFFSERV_ACTION_PKT_PRI:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServActionPktPri(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServActionStatus(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETDIFFSERVMETERENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.meterid_param);
            msg_data_p->type.result_bool=L4_MGR_QoS_GetDiffServMeterEntry(
                 msg_data_p->data.meterid_param.meter_id,
                &msg_data_p->data.meterid_param.meter_param);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVMETERENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.meterid_param);
            msg_data_p->type.result_bool=L4_MGR_QoS_GetNextDiffServMeterEntry(
                &msg_data_p->data.meterid_param.meter_id,
                &msg_data_p->data.meterid_param.meter_param);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMETERMODEL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMeterModel(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMETERRATE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMeterRate(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

    /*zhangwei add */
     case L4_MGR_IPCCMD_QOS_SETDIFFSERVMETERPEAKRATE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMeterPeakRate(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;
    /*zhangwei add end*/

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMETERBURSTSIZE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMeterBurstSize(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;
       /*zhangwei add */
        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMETERPEAKBURSTSIZE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMeterPeakBurstSize(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;
    /*zhangwei add end*/


        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMETERINTERVAL:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMeterInterval(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETDIFFSERVMETERSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QoS_SetDiffServMeterStatus(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)
        case L4_MGR_IPCCMD_QOS_GETDIFFSERVPOLICYMAPHWCOUNTER:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.qos_hw_counter);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetDiffServPolicyMapHwCounter(
                msg_data_p->data.qos_hw_counter.ifindex,
                msg_data_p->data.qos_hw_counter.direction,
                msg_data_p->data.qos_hw_counter.policy_map_index,
                msg_data_p->data.qos_hw_counter.class_map_index,
                &msg_data_p->data.qos_hw_counter.counter);
        }
        break;

        case L4_MGR_IPCCMD_QOS_CLEARDIFFSERVPOLICYMAPHWCOUNTER:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.qos_hw_counter);
            msg_data_p->type.result_ui32=L4_MGR_QoS_ClearDiffServPolicyMapHwCounter(
                msg_data_p->data.qos_hw_counter.ifindex,
                msg_data_p->data.qos_hw_counter.direction,
                msg_data_p->data.qos_hw_counter.policy_map_index,
                msg_data_p->data.qos_hw_counter.class_map_index);
        }
        break;
#endif /*#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)*/

        case L4_MGR_IPCCMD_QOS_GET_DIFFSERV_ACL_HW_COUNTER_ENTTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.acl_hw_counter);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetDiffServAclHwCounterEntry(
                msg_data_p->data.acl_hw_counter.ifindex,
                msg_data_p->data.acl_hw_counter.direction,
                msg_data_p->data.acl_hw_counter.acl_index,
                msg_data_p->data.acl_hw_counter.ace_index,
                &msg_data_p->data.acl_hw_counter.ace_entry);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GET_NEXT_DIFFSERV_ACL_HW_COUNTER_ENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.acl_hw_counter);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetNextDiffServAclHwCounterEntry(
                &msg_data_p->data.acl_hw_counter.ifindex,
                &msg_data_p->data.acl_hw_counter.direction,
                &msg_data_p->data.acl_hw_counter.acl_index,
                &msg_data_p->data.acl_hw_counter.ace_index,
                &msg_data_p->data.acl_hw_counter.ace_entry);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_POLICYMAPPORT_POLICYMAPINDEX:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_SetPolicyMapPortPolcyMapIndex(
                msg_data_p->data.policy_map_port_entry.ifindex,
                msg_data_p->data.policy_map_port_entry.direction,
                msg_data_p->data.policy_map_port_entry.policy_map_index);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_POLICYMAPPORT_ROWSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_SetPolicyMapPortRowStatus(
                msg_data_p->data.policy_map_port_entry.ifindex,
                msg_data_p->data.policy_map_port_entry.direction,
                msg_data_p->data.policy_map_port_entry.row_status);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GET_POLICYMAPPORT_ENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
            +sizeof(msg_data_p->data.policy_map_port_entry);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetPolicyMapPortEntry(
                 &msg_data_p->data.policy_map_port_entry);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXT_POLICYMAPPORT_ENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
            +sizeof(msg_data_p->data.policy_map_port_entry);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetNextPolicyMapPortEntry(
                 &msg_data_p->data.policy_map_port_entry);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_ACCESSGROUP_ACLINDEX:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_SetAccessGroupAclIndex(
                msg_data_p->data.access_group_entry.ifindex,
                msg_data_p->data.access_group_entry.direction,
                msg_data_p->data.access_group_entry.type,
                msg_data_p->data.access_group_entry.acl_index);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_ACCESSGROUP_TIMERANGENAME:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_SetAccessGroupTimeRangeName(
                msg_data_p->data.access_group_entry.ifindex,
                msg_data_p->data.access_group_entry.direction,
                msg_data_p->data.access_group_entry.type,
                msg_data_p->data.access_group_entry.time_range_name);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SET_ACCESSGROUP_COUNTERSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_SetAccessGroupCounterStatus(
                msg_data_p->data.access_group_entry.ifindex,
                msg_data_p->data.access_group_entry.direction,
                msg_data_p->data.access_group_entry.type,
                msg_data_p->data.access_group_entry.counter_status);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_ACCESSGROUP_ROWSTATUS:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QoS_SetAccessGroupRowStatus(
                msg_data_p->data.access_group_entry.ifindex,
                msg_data_p->data.access_group_entry.direction,
                msg_data_p->data.access_group_entry.type,
                msg_data_p->data.access_group_entry.row_status);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GET_ACCESSGROUP_ENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
            +sizeof(msg_data_p->data.access_group_entry);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetAccessGroupEntry(
                 &msg_data_p->data.access_group_entry);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETNEXT_ACCESSGROUP_ENTRY:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
            +sizeof(msg_data_p->data.access_group_entry);
            msg_data_p->type.result_ui32=L4_MGR_QoS_GetNextAccessGroupEntry(
                 &msg_data_p->data.access_group_entry);
        }
        break;

/* ========================================================================== */
#if (SYS_CPNT_COS == TRUE)
        case L4_MGR_IPCCMD_QOS_SETTRUSTMODE:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
                msg_data_p->type.result_bool=L4_MGR_QOS_SetTrustMode(
                    msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                    msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETTRUSTMODE:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                    +sizeof(msg_data_p->data.ui32a1_ui32a2);
                msg_data_p->type.result_bool=L4_MGR_QOS_GetTrustMode(
                    msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                    &msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SETINGRESSCOS2DSCP:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
                msg_data_p->type.result_bool=L4_MGR_QOS_SetIngressCos2Dscp(
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a1,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a2,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a3,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a4,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a5);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETINGRESSCOS2DSCP:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                    +sizeof(msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5);
                msg_data_p->type.result_bool=L4_MGR_QOS_GetIngressCos2Dscp(
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a1,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a2,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a3,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a4,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a5);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETNEXTINGRESSCOS2DSCP:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                    +sizeof(msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5);
                msg_data_p->type.result_bool=L4_MGR_QOS_GetNextIngressCos2Dscp(
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a1,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a2,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a3,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a4,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a5);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SET_COS_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QOS_SetCos2InternalDscp(
                msg_data_p->data.port_cos_to_dscp.priority,
                msg_data_p->data.port_cos_to_dscp.cos,
                msg_data_p->data.port_cos_to_dscp.cfi,
                msg_data_p->data.port_cos_to_dscp.phb,
                msg_data_p->data.port_cos_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_COS_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_cos_to_dscp);
            msg_data_p->type.result_ui32=L4_MGR_QOS_GetCos2InternalDscp(
                msg_data_p->data.port_cos_to_dscp.priority,
                msg_data_p->data.port_cos_to_dscp.cos,
                msg_data_p->data.port_cos_to_dscp.cfi,
                &msg_data_p->data.port_cos_to_dscp.phb,
                &msg_data_p->data.port_cos_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_NEXT_COS_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_cos_to_dscp);
            msg_data_p->type.result_ui32=L4_MGR_QOS_GetNextCos2InternalDscp(
                msg_data_p->data.port_cos_to_dscp.priority,
                &msg_data_p->data.port_cos_to_dscp.cos,
                &msg_data_p->data.port_cos_to_dscp.cfi,
                &msg_data_p->data.port_cos_to_dscp.phb,
                &msg_data_p->data.port_cos_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_ACTIVE_COS_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_cos_to_dscp);
            msg_data_p->type.result_ui32=L4_MGR_QOS_GetActiveCos2InternalDscp(
                msg_data_p->data.port_cos_to_dscp.cos,
                msg_data_p->data.port_cos_to_dscp.cfi,
                &msg_data_p->data.port_cos_to_dscp.priority,
                &msg_data_p->data.port_cos_to_dscp.phb,
                &msg_data_p->data.port_cos_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_NEXT_RUNNING_COS_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_cos_to_dscp);
            msg_data_p->type.result_ui32=L4_MGR_QOS_GetNextRunningCos2InternalDscp(
                msg_data_p->data.port_cos_to_dscp.phb,
                msg_data_p->data.port_cos_to_dscp.color,
                &msg_data_p->data.port_cos_to_dscp.cos,
                &msg_data_p->data.port_cos_to_dscp.cfi);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SET_PORT_COS_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QOS_SetPortCos2InternalDscp(
                msg_data_p->data.port_cos_to_dscp.l_port,
                msg_data_p->data.port_cos_to_dscp.priority,
                msg_data_p->data.port_cos_to_dscp.cos,
                msg_data_p->data.port_cos_to_dscp.cfi,
                msg_data_p->data.port_cos_to_dscp.phb,
                msg_data_p->data.port_cos_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PORT_COS_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_cos_to_dscp);
            msg_data_p->type.result_ui32=L4_MGR_QOS_GetPortCos2InternalDscp(
                msg_data_p->data.port_cos_to_dscp.l_port,
                msg_data_p->data.port_cos_to_dscp.priority,
                msg_data_p->data.port_cos_to_dscp.cos,
                msg_data_p->data.port_cos_to_dscp.cfi,
                &msg_data_p->data.port_cos_to_dscp.phb,
                &msg_data_p->data.port_cos_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_NEXT_PORT_COS_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_cos_to_dscp);
            msg_data_p->type.result_ui32=L4_MGR_QOS_GetNextPortCos2InternalDscp(
                &msg_data_p->data.port_cos_to_dscp.l_port,
                msg_data_p->data.port_cos_to_dscp.priority,
                &msg_data_p->data.port_cos_to_dscp.cos,
                &msg_data_p->data.port_cos_to_dscp.cfi,
                &msg_data_p->data.port_cos_to_dscp.phb,
                &msg_data_p->data.port_cos_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PORT_ACTIVE_COS_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_cos_to_dscp);
            msg_data_p->type.result_ui32=L4_MGR_QOS_GetPortActiveCos2InternalDscp(
                msg_data_p->data.port_cos_to_dscp.l_port,
                msg_data_p->data.port_cos_to_dscp.cos,
                msg_data_p->data.port_cos_to_dscp.cfi,
                &msg_data_p->data.port_cos_to_dscp.priority,
                &msg_data_p->data.port_cos_to_dscp.phb,
                &msg_data_p->data.port_cos_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_NEXT_RUNNING_PORT_COS_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_cos_to_dscp);
            msg_data_p->type.result_ui32=L4_MGR_QOS_GetNextRunningPortCos2InternalDscp(
                msg_data_p->data.port_cos_to_dscp.l_port,
                msg_data_p->data.port_cos_to_dscp.phb,
                msg_data_p->data.port_cos_to_dscp.color,
                &msg_data_p->data.port_cos_to_dscp.cos,
                &msg_data_p->data.port_cos_to_dscp.cfi);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETINGRESSPRE2DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QOS_SetIngressPre2Dscp(
                msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a1,
                msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a2,
                msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a3,
                msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a4);
        }
        break;
        case L4_MGR_IPCCMD_QOS_RESETINGRESSPRE2DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QOS_ResetIngressPre2Dscp(
                msg_data_p->data.ui32a1_ui32a2.ui32_a1);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETINGRESSPRE2DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.pre_dscp);
            msg_data_p->type.result_bool=L4_MGR_QOS_GetIngressPre2Dscp(
                msg_data_p->data.pre_dscp.lport,
                msg_data_p->data.pre_dscp.phb,
                msg_data_p->data.pre_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SET_PORT_PRE_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_QOS_SetPortPre2InternalDscp(
                msg_data_p->data.port_pre_to_dscp.l_port,
                msg_data_p->data.port_pre_to_dscp.priority,
                msg_data_p->data.port_pre_to_dscp.pre,
                msg_data_p->data.port_pre_to_dscp.phb,
                msg_data_p->data.port_pre_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PORT_PRE_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_pre_to_dscp);
            msg_data_p->type.result_ui32=L4_MGR_QOS_GetPortPre2InternalDscp(
                msg_data_p->data.port_pre_to_dscp.l_port,
                msg_data_p->data.port_pre_to_dscp.priority,
                msg_data_p->data.port_pre_to_dscp.pre,
                &msg_data_p->data.port_pre_to_dscp.phb,
                &msg_data_p->data.port_pre_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_NEXT_PORT_PRE_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_pre_to_dscp);
            msg_data_p->type.result_ui32=L4_MGR_QOS_GetNextPortPre2InternalDscp(
                &msg_data_p->data.port_pre_to_dscp.l_port,
                msg_data_p->data.port_pre_to_dscp.priority,
                &msg_data_p->data.port_pre_to_dscp.pre,
                &msg_data_p->data.port_pre_to_dscp.phb,
                &msg_data_p->data.port_pre_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PORT_ACTIVE_PRE_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_pre_to_dscp);
            msg_data_p->type.result_ui32=L4_MGR_QOS_GetPortActivePre2InternalDscp(
                msg_data_p->data.port_pre_to_dscp.l_port,
                msg_data_p->data.port_pre_to_dscp.pre,
                &msg_data_p->data.port_pre_to_dscp.priority,
                &msg_data_p->data.port_pre_to_dscp.phb,
                &msg_data_p->data.port_pre_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_RUNNING_PORT_PRE_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.port_running_pre_to_dscp);
            msg_data_p->type.result_ui32=L4_MGR_QOS_GetRunningPortPre2InternalDscp(
                msg_data_p->data.port_running_pre_to_dscp.l_port,
                msg_data_p->data.port_running_pre_to_dscp.phb_ar,
                msg_data_p->data.port_running_pre_to_dscp.color_ar);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETINGRESSDSCP2DSCP:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
                msg_data_p->type.result_bool=L4_MGR_QOS_SetIngressDscp2Dscp(
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a1,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a2,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a3,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a4);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETINGRESSDSCP2DSCP:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.u32a1_u32a2_u32a3_u32a4);
                msg_data_p->type.result_bool=L4_MGR_QOS_GetIngressDscp2Dscp(
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a1,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a2,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a3,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4.u32_a4);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SETINGRESSDSCP2QUEUE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QOS_SetIngressDscp2Queue(
                msg_data_p->data.dscp_queue.phb,
                msg_data_p->data.dscp_queue.queue[0]);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SETPORTINGRESSDSCP2QUEUE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_QOS_SetPortIngressDscp2Queue(
                msg_data_p->data.dscp_queue.lport,
                msg_data_p->data.dscp_queue.phb,
                msg_data_p->data.dscp_queue.queue[0]);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETINGRESSDSCP2QUEUE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.dscp_queue);
            msg_data_p->type.result_bool=L4_MGR_QOS_GetIngressDscp2Queue(
                msg_data_p->data.dscp_queue.queue);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETPORTINGRESSDSCP2QUEUE:
        {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.dscp_queue);
            msg_data_p->type.result_bool=L4_MGR_QOS_GetPortIngressDscp2Queue(
                msg_data_p->data.dscp_queue.lport,
                msg_data_p->data.dscp_queue.queue);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SETINGRESSDSCP2COLOR:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
                msg_data_p->type.result_bool=L4_MGR_QOS_SetIngressDscp2Color(
                    msg_data_p->data.ui32a1_ui32a2_ui32a3.ui32_a1,
                    msg_data_p->data.ui32a1_ui32a2_ui32a3.ui32_a2,
                    msg_data_p->data.ui32a1_ui32a2_ui32a3.ui32_a3);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETINGRESSDSCP2COLOR:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.dscp_color);
                msg_data_p->type.result_bool=L4_MGR_QOS_GetIngressDscp2Color(
                    msg_data_p->data.dscp_color.lport,
                    msg_data_p->data.dscp_color.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SETINGRESSDSCP2COS:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
                msg_data_p->type.result_bool=L4_MGR_QOS_SetIngressDscp2Cos(
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a1,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a2,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a3,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a4,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a5);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETINGRESSDSCP2COS:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5);
                msg_data_p->type.result_bool=L4_MGR_QOS_GetIngressDscp2Cos(
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a1,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a2,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a3,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a4,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a5);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETNEXTINGRESSDSCP2COS:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5);
                msg_data_p->type.result_bool=L4_MGR_QOS_GetNextIngressDscp2Cos(
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a1,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a2,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a3,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a4,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a5);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETRUNNINGINGRESSDSCP2COLOR:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.dscp_color);
                msg_data_p->type.result_bool=L4_MGR_QOS_GetRunningIngressDscp2Color(
                    msg_data_p->data.dscp_color.lport,
                    msg_data_p->data.dscp_color.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETRUNNINGINGRESSDSCP2COS:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                +sizeof(msg_data_p->data.dscp_cos);
                msg_data_p->type.result_bool=L4_MGR_QOS_GetRunningIngressDscp2Cos(
                    msg_data_p->data.dscp_cos.lport,
                    msg_data_p->data.dscp_cos.cos,
                    msg_data_p->data.dscp_cos.cfi,
                    &msg_data_p->data.dscp_cos.l_phb,
                    &msg_data_p->data.dscp_cos.l_color,
                    msg_data_p->data.dscp_cos.cos_phb,
                    msg_data_p->data.dscp_cos.cos_color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SETTCPUDP2DSCP:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
                msg_data_p->type.result_bool=L4_MGR_QOS_SetTCPUDP2Dscp(
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5_u32a6.u32_a1,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5_u32a6.u32_a2,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5_u32a6.u32_a3,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5_u32a6.u32_a4,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5_u32a6.u32_a5,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5_u32a6.u32_a6);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETTCPUDP2DSCP:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                    +sizeof(msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5);
                msg_data_p->type.result_bool=L4_MGR_QOS_GetTCPUDP2Dscp(
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a1,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a2,
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a3,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a4,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a5);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETNEXTTCPUDP2DSCP:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                    +sizeof(msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5);
                msg_data_p->type.result_bool=L4_MGR_QOS_GetNextTCPUDP2Dscp(
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a1,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a2,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a3,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a4,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a5);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETRUNNINGTCPUDP2DSCP:
        {
                ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE
                    +sizeof(msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5);
                msg_data_p->type.result_bool=L4_MGR_QOS_GetRunningTCPUDP2Dscp(
                    msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a1,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a2,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a3,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a4,
                    &msg_data_p->data.u32a1_u32a2_u32a3_u32a4_u32a5.u32_a5);
        }
        break;
        case L4_MGR_IPCCMD_QOS_ENABLEPORTPRIORITYMODE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool = L4_MGR_QOS_EnablePortPriorityMode(
                msg_data_p->data.port_priority.l_port,
                msg_data_p->data.port_priority.priority);
        }
        break;
        case L4_MGR_IPCCMD_QOS_DISABLEPORTPRIORITYMODE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool = L4_MGR_QOS_DisablePortPriorityMode(
                msg_data_p->data.port_priority.l_port,
                msg_data_p->data.port_priority.priority);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SET_DSCP_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32  = L4_MGR_QOS_SetDscp2InternalDscp(
                msg_data_p->data.port_dscp_to_dscp.priority,
                msg_data_p->data.port_dscp_to_dscp.dscp,
                msg_data_p->data.port_dscp_to_dscp.phb,
                msg_data_p->data.port_dscp_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_DSCP_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_dscp_to_dscp);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetDscp2InternalDscp(
                msg_data_p->data.port_dscp_to_dscp.priority,
                msg_data_p->data.port_dscp_to_dscp.dscp,
                &msg_data_p->data.port_dscp_to_dscp.phb,
                &msg_data_p->data.port_dscp_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_NEXT_DSCP_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_dscp_to_dscp);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetNextDscp2InternalDscp(
                msg_data_p->data.port_dscp_to_dscp.priority,
                &msg_data_p->data.port_dscp_to_dscp.dscp,
                &msg_data_p->data.port_dscp_to_dscp.phb,
                &msg_data_p->data.port_dscp_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_ACTIVE_DSCP_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_dscp_to_dscp);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetActiveDscp2InternalDscp(
                msg_data_p->data.port_dscp_to_dscp.dscp,
                &msg_data_p->data.port_dscp_to_dscp.priority,
                &msg_data_p->data.port_dscp_to_dscp.phb,
                &msg_data_p->data.port_dscp_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_NEXT_RUNNING_DSCP_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_dscp_to_dscp);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetNextRunningDscp2InternalDscp(
                msg_data_p->data.port_dscp_to_dscp.phb,
                msg_data_p->data.port_dscp_to_dscp.color,
                &msg_data_p->data.port_dscp_to_dscp.dscp);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SET_PORT_DSCP_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32 = L4_MGR_QOS_SetPortDscp2InternalDscp(
                msg_data_p->data.port_dscp_to_dscp.l_port,
                msg_data_p->data.port_dscp_to_dscp.priority,
                msg_data_p->data.port_dscp_to_dscp.dscp,
                msg_data_p->data.port_dscp_to_dscp.phb,
                msg_data_p->data.port_dscp_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PORT_DSCP_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_dscp_to_dscp);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetPortDscp2InternalDscp(
                msg_data_p->data.port_dscp_to_dscp.l_port,
                msg_data_p->data.port_dscp_to_dscp.priority,
                msg_data_p->data.port_dscp_to_dscp.dscp,
                &msg_data_p->data.port_dscp_to_dscp.phb,
                &msg_data_p->data.port_dscp_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_NEXT_PORT_DSCP_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_dscp_to_dscp);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetNextPortDscp2InternalDscp(
                &msg_data_p->data.port_dscp_to_dscp.l_port,
                msg_data_p->data.port_dscp_to_dscp.priority,
                &msg_data_p->data.port_dscp_to_dscp.dscp,
                &msg_data_p->data.port_dscp_to_dscp.phb,
                &msg_data_p->data.port_dscp_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PORT_ACTIVE_DSCP_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_dscp_to_dscp);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetPortActiveDscp2InternalDscp(
                msg_data_p->data.port_dscp_to_dscp.l_port,
                msg_data_p->data.port_dscp_to_dscp.dscp,
                &msg_data_p->data.port_dscp_to_dscp.priority,
                &msg_data_p->data.port_dscp_to_dscp.phb,
                &msg_data_p->data.port_dscp_to_dscp.color);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_NEXT_RUNNING_PORT_DSCP_2_INTERNAL_DSCP:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_dscp_to_dscp);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetNextRunningPortDscp2InternalDscp(
                msg_data_p->data.port_dscp_to_dscp.l_port,
                msg_data_p->data.port_dscp_to_dscp.phb,
                msg_data_p->data.port_dscp_to_dscp.color,
                &msg_data_p->data.port_dscp_to_dscp.dscp);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SETPORTPRIORITYTRUSTMODE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool = L4_MGR_QOS_SetPortPriorityTrustMode(
                msg_data_p->data.port_trust_mode.l_port,
                msg_data_p->data.port_trust_mode.priority,
                msg_data_p->data.port_trust_mode.mode);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETPORTPRIORITYTRUSTMODE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_trust_mode);
            msg_data_p->type.result_bool = L4_MGR_QOS_GetPortPriorityTrustMode(
                msg_data_p->data.port_trust_mode.l_port,
                msg_data_p->data.port_trust_mode.priority,
                &msg_data_p->data.port_trust_mode.mode);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_INGRESS_PHB_2_QUEUE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;;
            msg_data_p->type.result_ui32 = L4_MGR_QOS_SetIngressPhb2Queue(
                msg_data_p->data.port_phb_to_queue.priority,
                msg_data_p->data.port_phb_to_queue.phb,
                msg_data_p->data.port_phb_to_queue.queue);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_INGRESS_PHB_2_QUEUE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_phb_to_queue);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetIngressPhb2Queue(
                msg_data_p->data.port_phb_to_queue.priority,
                msg_data_p->data.port_phb_to_queue.phb,
                &msg_data_p->data.port_phb_to_queue.queue);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_ACTIVE_INGRESS_PHB_2_QUEUE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_phb_to_queue);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetActiveIngressPhb2Queue(
                msg_data_p->data.port_phb_to_queue.phb,
                &msg_data_p->data.port_phb_to_queue.priority,
                &msg_data_p->data.port_phb_to_queue.queue);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_NEXT_RUNNING_INGRESS_PHB_2_QUEUE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_phb_to_queue);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetNextRunningIngressPhb2Queue(
                msg_data_p->data.port_phb_to_queue.queue,
                &msg_data_p->data.port_phb_to_queue.phb);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SET_PORT_INGRESS_PHB_2_QUEUE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32 = L4_MGR_QOS_SetPortIngressPhb2Queue(
                msg_data_p->data.port_phb_to_queue.l_port,
                msg_data_p->data.port_phb_to_queue.priority,
                msg_data_p->data.port_phb_to_queue.phb,
                msg_data_p->data.port_phb_to_queue.queue);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PORT_INGRESS_PHB_2_QUEUE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_phb_to_queue);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetPortIngressPhb2Queue(
                msg_data_p->data.port_phb_to_queue.l_port,
                msg_data_p->data.port_phb_to_queue.priority,
                msg_data_p->data.port_phb_to_queue.phb,
                &msg_data_p->data.port_phb_to_queue.queue);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_NEXT_PORT_INGRESS_PHB_2_QUEUE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_phb_to_queue);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetNextPortIngressPhb2Queue(
                &msg_data_p->data.port_phb_to_queue.l_port,
                msg_data_p->data.port_phb_to_queue.priority,
                &msg_data_p->data.port_phb_to_queue.phb,
                &msg_data_p->data.port_phb_to_queue.queue);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PORT_ACTIVE_INGRESS_PHB_2_QUEUE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_phb_to_queue);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetPortActiveIngressPhb2Queue(
                msg_data_p->data.port_phb_to_queue.l_port,
                msg_data_p->data.port_phb_to_queue.phb,
                &msg_data_p->data.port_phb_to_queue.priority,
                &msg_data_p->data.port_phb_to_queue.queue);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_NEXT_RUNNING_PORT_INGRESS_PHB_2_QUEUE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_phb_to_queue);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetNextRunningPortIngressPhb2Queue(
                msg_data_p->data.port_phb_to_queue.l_port,
                msg_data_p->data.port_phb_to_queue.queue,
                &msg_data_p->data.port_phb_to_queue.phb);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GETPRIORITYINGRESSDSCP2QUEUE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.dscp_queue);
            msg_data_p->type.result_bool = L4_MGR_QOS_GetPriorityIngressDscp2Queue(
                msg_data_p->data.dscp_queue.priority,
                msg_data_p->data.dscp_queue.queue);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETPORTPRIORITYINGRESSDSCP2QUEUE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.dscp_queue);
            msg_data_p->type.result_bool = L4_MGR_QOS_GetPortPriorityIngressDscp2Queue(
                msg_data_p->data.dscp_queue.lport,
                msg_data_p->data.dscp_queue.priority,
                msg_data_p->data.dscp_queue.queue);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SETPORTEGRESSQUEUEMODE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool = L4_MGR_QOS_SetPortEgressQueueMode(
                msg_data_p->data.port_queue_mode.l_port,
                msg_data_p->data.port_queue_mode.mode);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETPORTEGRESSQUEUEMODE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_queue_mode);
            msg_data_p->type.result_bool = L4_MGR_QOS_GetPortEgressQueueMode(
                msg_data_p->data.port_queue_mode.l_port,
                &msg_data_p->data.port_queue_mode.mode);
        }
        break;

        case L4_MGR_IPCCMD_QOS_GET_NEXT_PORT_EGRESS_QUEUE_MODE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_queue_mode);
            msg_data_p->type.result_bool = L4_MGR_QOS_GetNextPortEgressQueueMode(
                &msg_data_p->data.port_queue_mode.l_port,
                &msg_data_p->data.port_queue_mode.mode);
        }
        break;

        case L4_MGR_IPCCMD_QOS_SET_PRIORITY_EGRESSQUEUEMODE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool = L4_MGR_QOS_SetPriorityEgressQueueMode(
                msg_data_p->data.port_queue_mode.priority,
                msg_data_p->data.port_queue_mode.mode);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SETPORTPRIORITYEGRESSQUEUEMODE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool = L4_MGR_QOS_SetPortPriorityEgressQueueMode(
                msg_data_p->data.port_queue_mode.l_port,
                msg_data_p->data.port_queue_mode.priority,
                msg_data_p->data.port_queue_mode.mode);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PRIORITY_EGRESS_QUEUE_MODE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_queue_mode);
            msg_data_p->type.result_bool = L4_MGR_QOS_GetPriorityEgressQueueMode(
                msg_data_p->data.port_queue_mode.priority,
                &msg_data_p->data.port_queue_mode.mode);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_ACTIVE_EGRESS_QUEUE_MODE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_queue_mode);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetActiveEgressQueueMode(
                &msg_data_p->data.port_queue_mode.priority,
                &msg_data_p->data.port_queue_mode.mode);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PORT_PRIORITY_EGRESS_QUEUE_MODE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_queue_mode);
            msg_data_p->type.result_bool = L4_MGR_QOS_GetPortPriorityEgressQueueMode(
                msg_data_p->data.port_queue_mode.l_port,
                msg_data_p->data.port_queue_mode.priority,
                &msg_data_p->data.port_queue_mode.mode);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PORT_ACTIVE_EGRESS_QUEUE_MODE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_queue_mode);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetPortActiveEgressQueueMode(
                msg_data_p->data.port_queue_mode.l_port,
                &msg_data_p->data.port_queue_mode.priority,
                &msg_data_p->data.port_queue_mode.mode);
        }
        break;
        case L4_MGR_IPCCMD_QOS_RESETPORTPRIORITYEGRESSQUEUEMODE:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool = L4_MGR_QOS_ResetPortPriorityEgressQueueMode(
                msg_data_p->data.port_queue_mode.l_port,
                msg_data_p->data.port_queue_mode.priority);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SETPORTEGRESSWRRQUEUEWEIGHT:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool = L4_MGR_QOS_SetPortEgressWrrQueueWeight(
                msg_data_p->data.port_wrr_queue_weight.l_port,
                msg_data_p->data.port_wrr_queue_weight.queue_id,
                msg_data_p->data.port_wrr_queue_weight.weight);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GETPORTEGRESSWRRQUEUEWEIGHT:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_wrr_queue_weight);
            msg_data_p->type.result_bool = L4_MGR_QOS_GetPortEgressWrrQueueWeight(
                msg_data_p->data.port_wrr_queue_weight.l_port,
                msg_data_p->data.port_wrr_queue_weight.queue_id,
                &msg_data_p->data.port_wrr_queue_weight.weight);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_NEXT_EGRESS_QUEUE_WEIGHT:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_wrr_queue_weight);
            msg_data_p->type.result_bool = L4_MGR_QOS_GetNextEgressQueueWeight(
                &msg_data_p->data.port_wrr_queue_weight.queue_id,
                &msg_data_p->data.port_wrr_queue_weight.weight);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_NEXT_PORT_EGRESS_QUEUE_WEIGHT:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_wrr_queue_weight);
            msg_data_p->type.result_bool = L4_MGR_QOS_GetNextPortEgressQueueWeight(
                &msg_data_p->data.port_wrr_queue_weight.l_port,
                &msg_data_p->data.port_wrr_queue_weight.queue_id,
                &msg_data_p->data.port_wrr_queue_weight.weight);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_RUNNING_EGRESS_QUEUE_WEIGHT:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_wrr_queue_weight);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetRunningEgressQueueWeight(
                msg_data_p->data.port_wrr_queue_weight.queue_id,
                &msg_data_p->data.port_wrr_queue_weight.weight);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_RUNNING_PORT_EGRESS_QUEUE_WEIGHT:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_wrr_queue_weight);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetRunningPortEgressQueueWeight(
                msg_data_p->data.port_wrr_queue_weight.l_port,
                msg_data_p->data.port_wrr_queue_weight.queue_id,
                &msg_data_p->data.port_wrr_queue_weight.weight);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SET_PRIORITY_EGRESS_WRR_QUEUE_WEIGHT:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool = L4_MGR_QOS_SetPriorityEgressWrrQueueWeight(
                msg_data_p->data.port_wrr_queue_weight.priority,
                msg_data_p->data.port_wrr_queue_weight.queue_id,
                msg_data_p->data.port_wrr_queue_weight.weight);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SET_PORT_PRIORITY_EGRESS_WRR_QUEUE_WEIGHT:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool = L4_MGR_QOS_SetPortPriorityEgressWrrQueueWeight(
                msg_data_p->data.port_wrr_queue_weight.l_port,
                msg_data_p->data.port_wrr_queue_weight.priority,
                msg_data_p->data.port_wrr_queue_weight.queue_id,
                msg_data_p->data.port_wrr_queue_weight.weight);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PRIORITY_EGRESS_WRR_QUEUE_WEIGHT:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_wrr_queue_weight);
            msg_data_p->type.result_bool = L4_MGR_QOS_GetPriorityEgressWrrQueueWeight(
                msg_data_p->data.port_wrr_queue_weight.priority,
                msg_data_p->data.port_wrr_queue_weight.queue_id,
                &msg_data_p->data.port_wrr_queue_weight.weight);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_ACTIVE_EGRESS_QUEUE_WEIGHT:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_wrr_queue_weight);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetActiveQueueWeight(
                msg_data_p->data.port_wrr_queue_weight.queue_id,
                &msg_data_p->data.port_wrr_queue_weight.priority,
                &msg_data_p->data.port_wrr_queue_weight.weight);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PORT_PRIORITY_EGRESS_WRR_QUEUE_WEIGHT:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_wrr_queue_weight);
            msg_data_p->type.result_bool = L4_MGR_QOS_GetPortPriorityEgressWrrQueueWeight(
                msg_data_p->data.port_wrr_queue_weight.l_port,
                msg_data_p->data.port_wrr_queue_weight.priority,
                msg_data_p->data.port_wrr_queue_weight.queue_id,
                &msg_data_p->data.port_wrr_queue_weight.weight);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PORT_ACTIVE_EGRESS_QUEUE_WEIGHT:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_wrr_queue_weight);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetPortActiveQueueWeight(
                msg_data_p->data.port_wrr_queue_weight.l_port,
                msg_data_p->data.port_wrr_queue_weight.queue_id,
                &msg_data_p->data.port_wrr_queue_weight.priority,
                &msg_data_p->data.port_wrr_queue_weight.weight);
        }
        break;
        case L4_MGR_IPCCMD_QOS_RESETPORTPRIORITYEGRESSWRRQUEUEWEIGHT:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool = L4_MGR_QOS_ResetPortPriorityEgressWrrQueueWeight(
                msg_data_p->data.port_wrr_queue_weight.l_port,
                msg_data_p->data.port_wrr_queue_weight.priority,
                msg_data_p->data.port_wrr_queue_weight.queue_id);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SET_PRIORITY_EGRESS_QUEUE_STRICT_STATUS:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool = L4_MGR_QOS_SetPriorityEgressQueueStrictStatus(
                msg_data_p->data.port_wrr_queue_weight.priority,
                msg_data_p->data.port_wrr_queue_weight.queue_id,
                msg_data_p->data.port_wrr_queue_weight.strict_status);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PRIORITY_EGRESS_QUEUE_STRICT_STATUS:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_wrr_queue_weight);
            msg_data_p->type.result_bool = L4_MGR_QOS_GetPriorityEgressQueueStrictStatus(
                msg_data_p->data.port_wrr_queue_weight.priority,
                msg_data_p->data.port_wrr_queue_weight.queue_id,
                &msg_data_p->data.port_wrr_queue_weight.strict_status);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_ACTIVE_EGRESS_QUEUE_STRICT_STATUS:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_wrr_queue_weight);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetActiveQueueStrictStatus(
                msg_data_p->data.port_wrr_queue_weight.queue_id,
                &msg_data_p->data.port_wrr_queue_weight.priority,
                &msg_data_p->data.port_wrr_queue_weight.strict_status);
        }
        break;
        case L4_MGR_IPCCMD_QOS_SET_PORT_PRIORITY_EGRESS_QUEUE_STRICT_STATUS:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool = L4_MGR_QOS_SetPortPriorityEgressQueueStrictStatus(
                msg_data_p->data.port_wrr_queue_weight.l_port,
                msg_data_p->data.port_wrr_queue_weight.priority,
                msg_data_p->data.port_wrr_queue_weight.queue_id,
                msg_data_p->data.port_wrr_queue_weight.strict_status);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PORT_PRIORITY_EGRESS_QUEUE_STRICT_STATUS:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_wrr_queue_weight);
            msg_data_p->type.result_bool = L4_MGR_QOS_GetPortPriorityEgressQueueStrictStatus(
                msg_data_p->data.port_wrr_queue_weight.l_port,
                msg_data_p->data.port_wrr_queue_weight.priority,
                msg_data_p->data.port_wrr_queue_weight.queue_id,
                &msg_data_p->data.port_wrr_queue_weight.strict_status);
        }
        break;
        case L4_MGR_IPCCMD_QOS_GET_PORT_ACTIVE_EGRESS_QUEUE_STRICT_STATUS:
        {
            ipcmsg_p->msg_size = L4_MGR_MSGBUF_TYPE_SIZE
               + sizeof(msg_data_p->data.port_wrr_queue_weight);
            msg_data_p->type.result_ui32 = L4_MGR_QOS_GetPortActiveQueueStrictStatus(
                msg_data_p->data.port_wrr_queue_weight.l_port,
                msg_data_p->data.port_wrr_queue_weight.queue_id,
                &msg_data_p->data.port_wrr_queue_weight.priority,
                &msg_data_p->data.port_wrr_queue_weight.strict_status);
        }
        break;

#endif //#if (SYS_CPNT_COS == TRUE)
/* ========================================================================== */

#if (SYS_CPNT_WEBAUTH == TRUE)
    case L4_MGR_IPCCMD_SETREDIRECTHTTPCLIENTPACKETS:
    {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_SetRedirectHTTPClientPackets(
                msg_data_p->data.port_status.lport,
                msg_data_p->data.port_status.is_enable);
    }
    break;

    case L4_MGR_IPCCMD_SETPERMITSRCIPPACKETSBYLPORT:
    {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_SetPermitSrcIpPacketsByLport(
                msg_data_p->data.port_ip_status.lport,
                msg_data_p->data.port_ip_status.sip,
                msg_data_p->data.port_ip_status.is_enable);
    }
    break;

    case L4_MGR_IPCCMD_SETDENYIPPACKETSBYLPORT:
    {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_bool=L4_MGR_SetDenyIpPacketsByLport(
                msg_data_p->data.port_status.lport,
                msg_data_p->data.port_status.is_enable);
    }
    break;
#endif /* #if (SYS_CPNT_WEBAUTH == TRUE) */

#if (SYS_CPNT_IPV6 == TRUE)
    case L4_MGR_IPCCMD_TRAPLINKLOCALTOCPU :
    {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_TrapLinkLocalToCPU(
                msg_data_p->data.traplinklocal.enable_flag,
                msg_data_p->data.traplinklocal.link_local_addr,
                msg_data_p->data.traplinklocal.vlan_id);

    }
    break;
#endif /* SYS_CPNT_IPV6 */

    case L4_MGR_IPCCMD_TRAPPACKET2CPU:
    {
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_TrapPacket2Cpu(
                msg_data_p->data.trap2cpu.enable_flag,
                msg_data_p->data.trap2cpu.packet_type,
                NULL);
    }
    break;

#if (SYS_CPNT_DAI == TRUE)
    case L4_MGR_IPCCMD_QOS_GETDIFFSERVARPACEENTRY:
        ipcmsg_p->msg_size=(sizeof(((L4_MGR_IPCMsg_T *)0)->data.aceid_uiace)
            + L4_MGR_MSGBUF_TYPE_SIZE);
        msg_data_p->type.result_ui32 = L4_MGR_QoS_GetDiffServARPAceEntry(
             msg_data_p->data.aceid_uiace.ace_id,
            &msg_data_p->data.aceid_uiace.ace);
        break;
    case L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVARPACEENTRY:
        ipcmsg_p->msg_size=(sizeof(((L4_MGR_IPCMsg_T *)0)->data.aceid_uiace)
            + L4_MGR_MSGBUF_TYPE_SIZE);
        msg_data_p->type.result_ui32 = L4_MGR_QoS_GetNextDiffServArpAceEntry(
            &msg_data_p->data.aceid_uiace.ace_id,
            &msg_data_p->data.aceid_uiace.ace);
        break;
    case L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACEACTION:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool = L4_MGR_QoS_SetDiffServArpAceAction(msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                                                                         msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        break;
    case L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACEPKTTYPE:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool = L4_MGR_QoS_SetDiffServArpAcePktType(msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                                                                         msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        break;
    case L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACESRCIPADDR:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool = L4_MGR_QoS_SetDiffServArpAceSrcIpAddr(msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                                                                         msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        break;
    case L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACESRCIPADDRMASK:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool = L4_MGR_QoS_SetDiffServArpAceSrcIpAddrMask(msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                                                                         msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        break;
    case L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACEDSTIPADDR:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool = L4_MGR_QoS_SetDiffServArpAceDstIpAddr(msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                                                                         msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        break;
    case L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACEDSTIPADDRMASK:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool = L4_MGR_QoS_SetDiffServArpAceDstIpAddrMask(msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                                                                         msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        break;
    case L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACESRCMACADDR:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool = L4_MGR_QoS_SetDiffServArpAceSrcMacAddr(msg_data_p->data.aceid_mac.ace_id,
                                                                         msg_data_p->data.aceid_mac.mac);
        break;
    case L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACESRCMACADDRMASK:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool = L4_MGR_QoS_SetDiffServArpAceSrcMacAddrMask(msg_data_p->data.aceid_mac.ace_id,
                                                                         msg_data_p->data.aceid_mac.mac);
        break;
    case L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACEDSTMACADDR:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool = L4_MGR_QoS_SetDiffServArpAceDstMacAddr(msg_data_p->data.aceid_mac.ace_id,
                                                                         msg_data_p->data.aceid_mac.mac);
        break;
    case L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACEDSTMACADDRMASK:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool = L4_MGR_QoS_SetDiffServArpAceDstMacAddrMask(msg_data_p->data.aceid_mac.ace_id,
                                                                         msg_data_p->data.aceid_mac.mac);
        break;
    case L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACESTATUS:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool =L4_MGR_QoS_SetDiffServArpAceStatus(msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                                                                         msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        break;
    case L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACELOGSTATUS:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool =L4_MGR_QoS_SetDiffServArpAceLogStatus(msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                                                                         msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        break;
#endif /* SYS_CPNT_DAI */

    case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACECOSOP:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool =L4_MGR_QoS_SetDiffServMacAceCosOp(msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                                                                         msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        break;
    case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACECOSBITMASK:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool =L4_MGR_QoS_SetDiffServMacAceCosBitmask(msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                                                                         msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        break;
    case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEMINCOS:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool =L4_MGR_QoS_SetDiffServMacAceMinCos(msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                                                                         msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        break;
    case L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEMAXCOS:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool =L4_MGR_QoS_SetDiffServMacAceMaxCos(msg_data_p->data.ui32a1_ui32a2.ui32_a1,
                                                                         msg_data_p->data.ui32a1_ui32a2.ui32_a2);
        break;

#if (SYS_CPNT_CLUSTER == TRUE)
    case L4_MGR_IPCCMD_SETCLUSTERPKTACTION:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_bool =L4_MGR_SetClusterPktAction(msg_data_p->data.bool_v);
        break;
#endif /* SYS_CPNT_CLUSTER */

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    case L4_MGR_IPCCMD_QOS_SET_BUNDLE_GROUP:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
        msg_data_p->type.result_ui32 =L4_MGR_QoS_SetBundleGroup(
            msg_data_p->data.pmap_bundle_group.pmap_name,
            &msg_data_p->data.pmap_bundle_group.bundle_group);
        break;
    case L4_MGR_IPCCMD_QOS_GET_BUNDLE_GROUP:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE+
            +sizeof(msg_data_p->data.pmap_bundle_group);
        msg_data_p->type.result_ui32 =L4_MGR_QoS_GetBundleGroup(
            msg_data_p->data.pmap_bundle_group.index,
            &msg_data_p->data.pmap_bundle_group.bundle_group);
        break;
    case L4_MGR_IPCCMD_QOS_GET_NEXT_BUNDLE_GROUP:
        ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE+
            +sizeof(msg_data_p->data.pmap_bundle_group);
        msg_data_p->type.result_ui32 =L4_MGR_QoS_GetNextBundleGroup(
            &msg_data_p->data.pmap_bundle_group.index,
            &msg_data_p->data.pmap_bundle_group.bundle_group);
        break;
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

#if (SYS_CPNT_PBR == TRUE)
        case L4_MGR_IPCCMD_PBR_BIND_ACL:
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_PBR_BindAcl(
                msg_data_p->data.pbr_acl.vlan,
                msg_data_p->data.pbr_acl.seq_num,
                msg_data_p->data.pbr_acl.acl_index,
                &msg_data_p->data.pbr_acl.pbr_action);
            break;
        case L4_MGR_IPCCMD_PBR_UNBIND_ACL:
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=L4_MGR_PBR_UnbindAcl(
                msg_data_p->data.pbr_acl.vlan,
                msg_data_p->data.pbr_acl.seq_num,
                msg_data_p->data.pbr_acl.acl_index);
            break;
#endif /* SYS_CPNT_PBR */

        default:
            ipcmsg_p->msg_size=L4_MGR_MSGBUF_TYPE_SIZE;
            msg_data_p->type.result_ui32=0;
            SYSFUN_Debug_Printf("%s(): Invalid cmd.\n", __FUNCTION__);
            return TRUE;
    }

    exit:

        /*Check sychronism or asychronism ipc. If it is sychronism(need to respond)then we return true.
         */
        if(cmd<L4_MGR_IPCCMD_FOLLOWISASYNCHRONISMIPC)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }

    return TRUE;
}

#if (SYS_CPNT_DAI == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServARPAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get ui ace by arp_ace_index
 * INPUT    : arp_ace_index
 * OUTPUT   : ace_p
 * RETURN   : Error code
 * NOTE     : for SNMP - diffServArpAceTable
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetDiffServARPAceEntry(
    UI32_T arp_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if ((NULL == ace_p) ||
        (RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(arp_ace_index, ace_p)) ||
        (FALSE == L4_MGR_IS_ARP_ACL(ace_p->ace_type)))
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServArpAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next ui ace by arp_ace_index
 * INPUT    : arp_ace_index
 * OUTPUT   : arp_ace_index, ace_p
 * RETURN   : Error code
 * NOTE     : for SNMP - diffServArpAceTable
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetNextDiffServArpAceEntry(
    UI32_T *arp_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    L4_MGR_USE_CSC_CHECK_OPER_MODE(RULE_TYPE_FAIL);

    if ((NULL == arp_ace_index) || (NULL == ace_p) ||
        (RULE_TYPE_OK != RULE_MGR_GetNextUIAceByAceTypeAndIndex(RULE_TYPE_ARP_ACL,
                                                                arp_ace_index,
                                                                ace_p)))
    {
        L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_FAIL);
    }

    L4_MGR_RETURN_AND_RELEASE_CSC(RULE_TYPE_OK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceAction
 *------------------------------------------------------------------------------
 * PURPOSE  : change action by arp_ace_index
 * INPUT    : arp_ace_index, action_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceAction
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceAction(UI32_T arp_ace_index, UI32_T action_type)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(arp_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_ARP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.access = action_type;

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(arp_ace_index,
                                                         &ace,
                                                         LEAF_diffServArpAceAction));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAcePktType
 *------------------------------------------------------------------------------
 * PURPOSE  : change pkt type by arp_ace_index
 * INPUT    : arp_ace_index, pkt_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAcePktType
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAcePktType(UI32_T arp_ace_index, UI32_T pkt_type)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(arp_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_ARP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.arp.arp_type = pkt_type;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(arp_ace_index,
                                                         &ace,
                                                         LEAF_diffServArpAcePktType));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceSrcIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip by arp_ace_index
 * INPUT    : arp_ace_index, src_ip_addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceSourceIpAddr
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceSrcIpAddr(UI32_T arp_ace_index, UI32_T src_ip_addr)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(arp_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_ARP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    {
        UI8_T *ip_addr =(UI8_T *) &src_ip_addr;

        memcpy(ace.arp.sender.ip.addr, ip_addr,
               sizeof(ace.arp.sender.ip.addr));
    }
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(arp_ace_index,
                                                         &ace,
                                                         LEAF_diffServArpAceSourceIpAddr));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceSrcIpAddrMask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip mask by arp_ace_index
 * INPUT    : arp_ace_index, src_ip_addr_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceSourceIpAddrBitmask
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceSrcIpAddrMask(UI32_T arp_ace_index, UI32_T src_ip_addr_mask)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(arp_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_ARP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    {
        UI8_T *ip_mask =(UI8_T *) &src_ip_addr_mask;

        memcpy(ace.arp.sender.ip.mask, ip_mask,
               sizeof(ace.arp.sender.ip.mask));
    }
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(arp_ace_index,
                                                         &ace,
                                                         LEAF_diffServArpAceSourceIpAddrBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceDstIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip by arp_ace_index
 * INPUT    : arp_ace_index, dst_ip_addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceDestIpAddr
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceDstIpAddr(UI32_T arp_ace_index, UI32_T dst_ip_addr)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(arp_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_ARP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    {
        UI8_T *ip_addr =(UI8_T *) &dst_ip_addr;

        memcpy(ace.arp.target.ip.addr, ip_addr,
               sizeof(ace.arp.target.ip.addr));
    }

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(arp_ace_index,
                                                         &ace,
                                                         LEAF_diffServArpAceDestIpAddr));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceSrcIpAddrMask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip mask by arp_ace_index
 * INPUT    : arp_ace_index, src_ip_addr_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceDestIpAddrBitmask
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceDstIpAddrMask(UI32_T arp_ace_index, UI32_T dst_ip_addr_mask)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(arp_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_ARP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    {
        UI8_T *ip_mask =(UI8_T *) &dst_ip_addr_mask;

        memcpy(ace.arp.target.ip.mask, ip_mask,
               sizeof(ace.arp.target.ip.mask));
    }

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(arp_ace_index,
                                                         &ace,
                                                         LEAF_diffServArpAceDestIpAddrBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceSrcMacAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src mac address by arp_ace_index
 * INPUT    : arp_ace_index, dst_mac_addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceSourceMacAddr
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceSrcMacAddr(UI32_T arp_ace_index, UI8_T *src_mac_addr)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(arp_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_ARP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace.arp.sender.mac.addr, src_mac_addr, SYS_ADPT_MAC_ADDR_LEN);

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(arp_ace_index,
                                                         &ace,
                                                         LEAF_diffServArpAceSourceMacAddr));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceSrcMacAddrMask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src mac address mask by arp_ace_index
 * INPUT    : arp_ace_index, src_mac_addr_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceSourceMacAddrBitmask
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceSrcMacAddrMask(UI32_T arp_ace_index, UI8_T *src_mac_addr_mask)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(arp_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_ARP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace.arp.sender.mac.mask, src_mac_addr_mask, SYS_ADPT_MAC_ADDR_LEN);

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(arp_ace_index,
                                                         &ace,
                                                         LEAF_diffServArpAceSourceMacAddrBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceDstMacAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst mac address by arp_ace_index
 * INPUT    : arp_ace_index, dst_mac_addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceDestMacAddr
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceDstMacAddr(UI32_T arp_ace_index, UI8_T *dst_mac_addr)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(arp_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_ARP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace.arp.target.mac.addr, dst_mac_addr, SYS_ADPT_MAC_ADDR_LEN);

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(arp_ace_index,
                                                         &ace,
                                                         LEAF_diffServArpAceDestMacAddr));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceDstMacAddrMask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst mac address mask by arp_ace_index
 * INPUT    : arp_ace_index, dst_mac_addr_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceDestMacAddrBitmask
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceDstMacAddrMask(UI32_T arp_ace_index, UI8_T *dst_mac_addr_mask)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(arp_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_ARP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    memcpy(ace.arp.target.mac.mask, dst_mac_addr_mask, SYS_ADPT_MAC_ADDR_LEN);

    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(arp_ace_index,
                                                         &ace,
                                                         LEAF_diffServArpAceDestMacAddrBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change row status by arp_ace_index
 * INPUT    : arp_ace_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceStatus
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceStatus(UI32_T arp_ace_index, UI32_T row_status)
{
    BOOL_T  ret;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);

    L4_MGR_LOCK();
    ret = (RULE_TYPE_OK == RULE_MGR_SetAceRowStatus(arp_ace_index, RULE_TYPE_ARP_ACL, row_status));
    L4_MGR_UNLOCK();

    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceLogStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change log status by arp_ace_index
 * INPUT    : arp_ace_index, log_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp -diffServArpAceLogStatus
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceLogStatus(UI32_T arp_ace_index, BOOL_T log_status)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(arp_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_ARP_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.arp.is_log = log_status;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(arp_ace_index,
                                                         &ace,
                                                         LEAF_diffServArpAceLogStatus));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

#if (SYS_CPNT_DAI_RATE_LIMIT_BY_RULE == TRUE)
/*------------------------------------------------------------------------------
* ROUTINE NAME - L4_MGR_DAI_SetArpPktActionAndRate
*------------------------------------------------------------------------------
* PURPOSE:  For ARP INSPETCION (DAI)
*            config chip to trap ARP packets to CPU and set rate limit
* INPUT:    enable    - TRUE: Add rule
*                       FALE: Delete rule
*           interface - interface information
*           rate      - packet per second
* OUTPUT:   None
* RETURN:   RULE_TYPE_OK / RULE_TYPE_FAIL
* NOTE:     None
*------------------------------------------------------------------------------
*/
RULE_TYPE_RETURN_TYPE_T
L4_MGR_DAI_SetArpPktActionAndRate(
    BOOL_T enable,
    RULE_TYPE_INTERFACE_INFO_T interface,
    UI32_T rate)
{
    RULE_TYPE_RETURN_TYPE_T ret = RULE_TYPE_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = RULE_MGR_DAI_TrapArpPacketToCpuWithRate(enable, &interface, rate);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
* ROUTINE NAME - L4_MGR_DAI_GetArpPktCounter
*------------------------------------------------------------------------------
* PURPOSE:  For ARP INSPETCION (DAI)
*            Get the ARP packet counter by port.
* INPUT:    interface_p    - Interface information
*           trunk_member_p - Trunk member list,
*                            input "NULL" if the interface is not trunk.
*           reset_counter  - TRUE: clear counter, FALSE: do nothing.
* OUTPUT:   total_cnt_p    - ARP packet counter.
* RETURN:   RULE_TYPE_OK / RULE_TYPE_FAIL
* NOTE:     None
*------------------------------------------------------------------------------
*/
RULE_TYPE_RETURN_TYPE_T
L4_MGR_DAI_GetArpPktCounter(
    RULE_TYPE_INTERFACE_INFO_T *interface_p,
    RULE_TYPE_TRUNK_MEMBER_T *trunk_member_p,
    BOOL_T reset_counter,
    UI32_T *total_cnt_p)
{
    RULE_TYPE_RETURN_TYPE_T ret = RULE_TYPE_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = RULE_MGR_DAI_GetArpPacketCounter(interface_p,  trunk_member_p,
                                           reset_counter, total_cnt_p);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}
#endif /* SYS_CPNT_DAI_RATE_LIMIT_BY_RULE */
#endif /* SYS_CPNT_DAI */

#if (SYS_CPNT_CLUSTER == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetClusterPktAction
 *------------------------------------------------------------------------------
 * PURPOSE  : for Cluster
 *            config chip to control cluster  packets
 * INPUT    : action        -- could be:
 *                          TRUE    copy cluster packet to cpu
 *                          FALSE   not copy cluster packet to cpu
 *
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTE     : if chip is already configured, update/delete config from chip
 *            if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_SetClusterPktAction(BOOL_T action)
{
    BOOL_T ret = TRUE;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(action, RULE_TYPE_PacketType_ORG_SPECIFIC, NULL))
    {
        ret = FALSE;
    }
    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceCosOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change cos operator by mac_ace_index
 * INPUT    : mac_ace_index, cos_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceCosOp(UI32_T mac_ace_index, UI32_T cos_operator)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ether.cos.op = (UI8_T)cos_operator;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceCosOp));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceCosBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change cos mask by mac_ace_index
 * INPUT    : mac_ace_index, cos_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceCosBitmask(UI32_T mac_ace_index, UI32_T cos_mask)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ether.cos.u.s.mask = (UI8_T)cos_mask;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceCosBitmask));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceMinCos
 *------------------------------------------------------------------------------
 * PURPOSE  : change min cos by mac_ace_index
 * INPUT    : mac_ace_index, min_cos
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceMinCos(UI32_T mac_ace_index, UI32_T min_cos)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    if (ace.ether.cos.op == VAL_diffServMacAceCosOp_range)
    {
        ace.ether.cos.u.range.min = (UI8_T)min_cos;
    }
    else
    {
        ace.ether.cos.u.s.data = (UI8_T)min_cos;
    }
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceMinCos));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceMaxCos
 *------------------------------------------------------------------------------
 * PURPOSE  : change max cos by mac_ace_index
 * INPUT    : mac_ace_index, max_cos
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceMaxCos(UI32_T mac_ace_index, UI32_T max_cos)
{
    BOOL_T                    ret;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(FALSE);
    L4_MGR_LOCK();

    if ((RULE_TYPE_OK != RULE_MGR_GetUIAceByIndex(mac_ace_index, &ace)) ||
        (FALSE == L4_MGR_IS_MAC_ACL(ace.ace_type)))
    {
        L4_MGR_UNLOCK();
        L4_MGR_RETURN_AND_RELEASE_CSC(FALSE);
    }

    ace.ether.cos.u.range.max = (UI8_T)max_cos;
    ret = (RULE_TYPE_OK == RULE_MGR_SetUIAceFieldByIndex(mac_ace_index,
                                                         &ace,
                                                         LEAF_diffServMacAceMaxCos));

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

#if (SYS_CPNT_PBR == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_PBR_BindPbrAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Bind ACL to vlan for PBR
 * INPUT    : vlan      - (Key) Vlan ID.
 *            seq_num   - (Key) Route map sequence number.
 *            acl_index - (Key) ACL index.
 *            action_p  - Policy based routing action.
 * OUTPUT   : none
 * RETURN   : RULE_TYPE_OK / error_code(RULE_TYPE_RETURN_TYPE_T)
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_PBR_BindAcl(
    UI32_T vlan,
    UI32_T seq_num,
    UI32_T acl_index,
    RULE_TYPE_PBR_ACTION_T *action_p)
{
    RULE_TYPE_RETURN_TYPE_T ret = RULE_TYPE_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = RULE_MGR_SetPbrAcl(TRUE, vlan, seq_num, acl_index, action_p);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_PBR_UnbindAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Unind ACL to vlan for PBR
 * INPUT    : vlan      - (Key) vlan ID
 *            seq_num   - (Key) Route map sequence number
 *            acl_index - (Key) ACL index.
 * OUTPUT   : none
 * RETURN   : RULE_TYPE_OK / error_code(RULE_TYPE_RETURN_TYPE_T)
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_PBR_UnbindAcl(
    UI32_T vlan,
    UI32_T seq_num,
    UI32_T acl_index)
{
    RULE_TYPE_RETURN_TYPE_T ret = RULE_TYPE_FAIL;

    L4_MGR_USE_CSC_CHECK_OPER_MODE(ret);
    L4_MGR_LOCK();

    ret = RULE_MGR_SetPbrAcl(FALSE, vlan, seq_num, acl_index, NULL);

    L4_MGR_UNLOCK();
    L4_MGR_RETURN_AND_RELEASE_CSC(ret);
}
#endif /* SYS_CPNT_PBR */
