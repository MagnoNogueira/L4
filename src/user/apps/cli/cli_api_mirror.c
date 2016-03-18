#include "cli_api.h"
#include "cli_api_mirror.h"
#include <stdio.h>

#if (SYS_CPNT_ACL_MIRROR == TRUE)
#include "l4_pmgr.h"
#endif /* #if (SYS_CPNT_ACL_MIRROR == TRUE) */

/****************************************<<MIRROR>>******************************************/
/*change mode*/

/*------------------------------------------------------------------------------
 * ROUTINE NAME - CLI_API_Show_Port_Monitor
 *------------------------------------------------------------------------------
 * PURPOSE : This function will show port monitor at execution mode
 * INPUT   : cmd_idx -- command idx
 *           arg     -- configures arguments
 *           ctrl_P  -- working area
 * OUTPUT  : none
 * RETURN  : CLI_NO_ERROR
 * NOTE    : 1. VLAN Mirror
 *           2. MAC Mirror
 *           3. Port Mirror
 *------------------------------------------------------------------------------
 */
/*execution*/
UI32_T CLI_API_Show_Port_Monitor(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
   UI32_T unit     = 0;
   UI32_T port     = 0;
   UI32_T trunk_id = 0;
   UI32_T des_port = 0;
   SWCTRL_MirrorEntry_T mirroring_entry;
   UI32_T line_num = 0;                    /* for PROCESS_MORE() */
   char  buff[CLI_DEF_MAX_BUFSIZE] = {0}; /* for PROCESS_MORE() */

#if (SYS_CPNT_VLAN_MIRROR == TRUE)
    BOOL_T is_first = FALSE;
    SWCTRL_VlanMirrorEntry_T vlan_mirror_entry; /* vlan mirror table */
#endif /* End of #if (SYS_CPNT_VLAN_MIRROR == TRUE) */

#if (SYS_CPNT_MAC_BASED_MIRROR == TRUE)
    SWCTRL_MacAddrMirrorEntry_T    mac_addr_entry;
    UI32_T                         mac_entry_cnt = 0;
    UI32_T                         dest_port = 0;
#endif /* End of #if (SYS_CPNT_MAC_BASED_MIRROR == TRUE) */

    memset(&mirroring_entry , 0 , sizeof(SWCTRL_MirrorEntry_T));
    if (arg[0] == NULL)
    {
        PROCESS_MORE("Port Mirroring\r\n");
        PROCESS_MORE("-------------------------------------\r\n");

        while (SWCTRL_POM_GetNextMirrorEntry(&mirroring_entry))
        {
#if (CLI_SUPPORT_PORT_NAME == 1)
            {
                UI8_T name[MAXSIZE_ifName+1] = {0};

                CLI_LIB_Ifindex_To_Name(mirroring_entry.mirror_destination_port,name);
                sprintf(buff, " Destination Port (listen port): %s\r\n",name);
                PROCESS_MORE(buff);

                CLI_LIB_Ifindex_To_Name(mirroring_entry.mirror_source_port,name);
                sprintf(buff, " Source Port (monitored port):   %s\r\n",name);
                PROCESS_MORE(buff);
             }
#else
            SWCTRL_POM_LogicalPortToUserPort(mirroring_entry.mirror_destination_port, &unit, &port, &trunk_id);
            sprintf(buff, " Destination Port (listen port): Eth%lu/%2lu\r\n",unit,port);
            PROCESS_MORE(buff);
            SWCTRL_POM_LogicalPortToUserPort(mirroring_entry.mirror_source_port, &unit, &port, &trunk_id);
            sprintf(buff, " Source Port (monitored port):   Eth%lu/%2lu\r\n",unit,port);
            PROCESS_MORE(buff);
#endif

            switch(mirroring_entry.mirror_type)
            {
            case VAL_mirrorType_rx:
                PROCESS_MORE(" Mode                         :RX\r\n\r\n");
                break;

             case VAL_mirrorType_tx:
                PROCESS_MORE(" Mode                         :TX\r\n\r\n");
                break;

             case VAL_mirrorType_both:
                PROCESS_MORE(" Mode                         :RX/TX\r\n\r\n");
                break;

             default:
                /*CLI_LIB_PrintStr("Failed to get port monitor status\r\n");*/
                return CLI_NO_ERROR;
             }
      } /* end of while (SWCTRL_POM_GetNextMirrorEntry(&mirroring_entry)) */

#if (SYS_CPNT_VLAN_MIRROR == TRUE)

        /* initial vlan mirror entry and display */
        memset(&vlan_mirror_entry , 0 , sizeof(SWCTRL_VlanMirrorEntry_T));
        vlan_mirror_entry.mirror_dest_port = 0;
        vlan_mirror_entry.mirror_source_vlan = 0;
        PROCESS_MORE("\r\n");

        /* get next vlan mirror entry */
        while (SWCTRL_PMGR_GetNextVlanMirrorEntry(&vlan_mirror_entry))
        {
            if (is_first == FALSE)
            {
                PROCESS_MORE("VLAN Mirroring\r\n");
                PROCESS_MORE("-------------------------------------\r\n");
                SWCTRL_POM_LogicalPortToUserPort(vlan_mirror_entry.mirror_dest_port, &unit, &port, &trunk_id);
                sprintf(buff, " Destination Port (listen port) : Eth%lu/%2lu\r\n",unit,port);
                PROCESS_MORE(buff);
                is_first = TRUE;
            }

            sprintf(buff, " Source VLAN (monitored VLAN ID): %lu\r\n",vlan_mirror_entry.mirror_source_vlan);
            PROCESS_MORE(buff);

        }
#endif /* End of #if (SYS_CPNT_VLAN_MIRROR == TRUE) */

#if (SYS_CPNT_MAC_BASED_MIRROR == TRUE)
        memset(&mac_addr_entry , 0 , sizeof(SWCTRL_MacAddrMirrorEntry_T));
        mac_addr_entry.addr_entry_index = 0;
        PROCESS_MORE("\r\n");

        /* get next mac-addressmirror entry */
        while (SWCTRL_POM_GetNextMacAddrMirrorEntry(&mac_addr_entry))
        {
            if (mac_entry_cnt == 0)
            {
                SWCTRL_POM_GetVlanAndMacMirrorDestPort(&dest_port);
                PROCESS_MORE("MAC Address Mirroring\r\n");
                PROCESS_MORE("-------------------------------------\r\n");
                SWCTRL_POM_LogicalPortToUserPort(dest_port, &unit, &port, &trunk_id);
                sprintf(buff, " Destination Port (listen port) : Eth%lu/%2lu\r\n",unit,port);
                PROCESS_MORE(buff);
            }

            mac_entry_cnt++;

            sprintf(buff, " Source MAC address             : %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                                   mac_addr_entry.mac_addr[0],
                                   mac_addr_entry.mac_addr[1],
                                   mac_addr_entry.mac_addr[2],
                                   mac_addr_entry.mac_addr[3],
                                   mac_addr_entry.mac_addr[4],
                                   mac_addr_entry.mac_addr[5]);
            PROCESS_MORE(buff);
        }/* End of while loop */
#endif /* End of #if (SYS_CPNT_MAC_BASED_MIRROR == TRUE) */

#if (SYS_CPNT_ACL_MIRROR == TRUE)
        {
            UI32_T  acl_dest_port;
            UI32_T  acl_index;
            RULE_TYPE_UI_AclEntry_T acl_entry;
            BOOL_T  first_entry = TRUE;

            acl_dest_port = 0;

            while (RULE_TYPE_OK == L4_PMGR_ACL_GetNextAclMirrorEntry(&acl_dest_port, &acl_index, &acl_entry) )
            {
                if (TRUE == first_entry)
                {
                    PROCESS_MORE("\r\n");

                    PROCESS_MORE("ACL Mirroring\r\n");
                    PROCESS_MORE("-------------------------------------\r\n");
                    SWCTRL_POM_LogicalPortToUserPort(acl_dest_port, &unit, &port, &trunk_id);
                    sprintf(buff, " Destination Port (Listen Port) : Eth%lu/%2lu\r\n",unit,port);
                    PROCESS_MORE(buff);

                    first_entry = FALSE;
                }

                sprintf(buff, " Source Access-List             : %s\r\n", acl_entry.acl_name);
                PROCESS_MORE(buff);
            }
        }
#endif /* #if (SYS_CPNT_ACL_MIRROR == TRUE) */

    } /* end of if (arg[0] == NULL) */
    else
    {

        /*--------------------------------------------------------
         * Rule 1: Show specifies the VLAN Mirror -- *arg[0] v|V
         *--------------------------------------------------------*/
#if (SYS_CPNT_VLAN_MIRROR == TRUE)
        /* show vlan mirroring info by specifies vlan-id */
        if (*arg[0] == 'v' || *arg[0] == 'V') /* VLAN */
        {
            UI32_T vid = atoi(arg[1]);

            /* get vlan status */
            if (VLAN_POM_IsVlanExisted(vid) == FALSE)
            {
                sprintf(buff, "No such VLAN %lu\r\n", vid);
                PROCESS_MORE(buff);
                return CLI_NO_ERROR;
            }

            /* init vlan entry table */
            memset(&vlan_mirror_entry , 0 , sizeof(SWCTRL_VlanMirrorEntry_T));
            vlan_mirror_entry.mirror_dest_port = 0;
            vlan_mirror_entry.mirror_source_vlan = vid;

            /* get this vlan id will be vlan mirroring source */
            if (SWCTRL_PMGR_GetVlanMirrorEntry(&vlan_mirror_entry) == TRUE)
            {
                PROCESS_MORE("VLAN Mirroring\r\n");
                PROCESS_MORE("-------------------------------------\r\n");
                SWCTRL_POM_LogicalPortToUserPort(vlan_mirror_entry.mirror_dest_port, &unit, &port, &trunk_id);
                sprintf(buff, " Destination Port (listen port) : Eth%lu/%2lu\r\n",unit,port);
                PROCESS_MORE(buff);
                sprintf(buff, " Source VLAN (monitored VLAN ID): %lu\r\n",vlan_mirror_entry.mirror_source_vlan);
                PROCESS_MORE(buff);
            }

            return CLI_NO_ERROR;
        }/* End of if (*arg[0] == 'v' || *arg[0] == 'V') */
#endif /* End of #if (SYS_CPNT_VLAN_MIRROR == TRUE) */

        /*--------------------------------------------------------
         * Rule 2: Show specifies the MAC Mirror -- *arg[0] m|M
         *--------------------------------------------------------*/
#if (SYS_CPNT_MAC_BASED_MIRROR == TRUE)
        if (*arg[0] == 'm' || *arg[0] == 'M') /* mac-address */
        {
            UI8_T  macAddress[SYS_ADPT_MAC_ADDR_LEN] = {0};
            UI32_T dest_port = 0;

            /*MAC address*/
            CLI_LIB_ValsInMac(arg[1], macAddress);

            memset(&mac_addr_entry , 0 , sizeof(SWCTRL_MacAddrMirrorEntry_T));
            mac_addr_entry.addr_entry_index = 0;
            memcpy(mac_addr_entry.mac_addr, macAddress, SYS_ADPT_MAC_ADDR_LEN);

            /* get this vlan id will be vlan mirroring source */
            if (SWCTRL_POM_GetExactMacAddrMirrorEntry(&mac_addr_entry) == TRUE)
            {
                SWCTRL_POM_GetVlanAndMacMirrorDestPort(&dest_port);
                PROCESS_MORE("MAC Address Mirroring\r\n");
                PROCESS_MORE("-------------------------------------\r\n");
                SWCTRL_POM_LogicalPortToUserPort(dest_port, &unit, &port, &trunk_id);
                sprintf(buff, " Destination Port (listen port) : Eth%lu/%2lu\r\n",unit,port);
                PROCESS_MORE(buff);
                sprintf(buff, " Source MAC address             : %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                                   mac_addr_entry.mac_addr[0],
                                   mac_addr_entry.mac_addr[1],
                                   mac_addr_entry.mac_addr[2],
                                   mac_addr_entry.mac_addr[3],
                                   mac_addr_entry.mac_addr[4],
                                   mac_addr_entry.mac_addr[5]);
                PROCESS_MORE(buff);
            }

            return CLI_NO_ERROR;
        }/* End of if (*arg[0] == 'm' || *arg[0] == 'M') */
#endif /* End of #if (SYS_CPNT_MAC_BASED_MIRROR == TRUE) */

#if (SYS_CPNT_ACL_MIRROR == TRUE)
        if (*arg[0] == 'a' || *arg[0] == 'A')
        {
            UI32_T  acl_dest_port;
            UI32_T  acl_index;
            RULE_TYPE_UI_AclEntry_T acl_entry;
            char    *filter_acl_name = NULL;
            BOOL_T  first_entry = TRUE;

            if (arg[1])
            {
                filter_acl_name = arg[1];
            }

            acl_dest_port = 0;

            while (RULE_TYPE_OK == L4_PMGR_ACL_GetNextAclMirrorEntry(&acl_dest_port, &acl_index, &acl_entry) )
            {
                if (TRUE == first_entry)
                {
                    PROCESS_MORE("\r\n");

                    PROCESS_MORE("ACL Mirroring\r\n");
                    PROCESS_MORE("-------------------------------------\r\n");
                    SWCTRL_POM_LogicalPortToUserPort(acl_dest_port, &unit, &port, &trunk_id);
                    sprintf(buff, " Destination Port (Listen Port) : Eth%lu/%2lu\r\n",unit,port);
                    PROCESS_MORE(buff);

                    first_entry = FALSE;
                }

                if (filter_acl_name && 0 != strcmp(acl_entry.acl_name, filter_acl_name))
                {
                    continue;
                }

                sprintf(buff, " Source Access-List             : %s\r\n", acl_entry.acl_name);
                PROCESS_MORE(buff);
            }

            return CLI_NO_ERROR;
        }
#endif /* #if (SYS_CPNT_ACL_MIRROR == TRUE) */


        /*--------------------------------------------------------
         * Rule 3: Show specifies the Port Mirror -- *arg[0] e|E
         *--------------------------------------------------------*/

        PROCESS_MORE("Port Mirroring\r\n");
        PROCESS_MORE("-------------------------------------\r\n");

        /* this statement shall be get specific Ethernet port */
        if (*arg[0] == 'e' || *arg[0] == 'E') /* Ethernet Port */
        {

            UI32_T verify_unit = atoi((char*)arg[1]);
            UI32_T verify_port = atoi(strchr((char*)arg[1], '/')+1);
            CLI_API_EthStatus_T verify_ret;

#if (CLI_SUPPORT_PORT_NAME == 1)
            if (isdigit(arg[1][0]))
            {
                verify_unit = atoi(arg[1]);
                verify_port = atoi(strchr(arg[1],'/')+1);
            }
            else/*port name*/
            {
                UI32_T trunk_id = 0;
                UI32_T lport = 0;
                if (!IF_PMGR_IfnameToIfindex(arg[1], &lport))
                {
#if (SYS_CPNT_EH == TRUE)
                    CLI_API_Show_Exception_Handeler_Msg();
#else
                    CLI_LIB_PrintStr_1("%s does not exist\r\n",arg[1]);
#endif
                    return CLI_NO_ERROR;
                }
                SWCTRL_POM_LogicalPortToUserPort(lport, &verify_unit, &verify_port, &trunk_id);
            }
#endif
            if( (verify_ret = verify_ethernet(verify_unit, verify_port, &des_port)) != CLI_API_ETH_OK)
            {
                display_ethernet_msg(verify_ret, verify_unit, verify_port);
            }
            else
            {
                mirroring_entry.mirror_destination_port = des_port; /*primary key*/
                mirroring_entry.mirror_source_port = 0;             /*secondary key*/

                while (SWCTRL_POM_GetNextMirrorEntry(&mirroring_entry))
                {
                    if (mirroring_entry.mirror_destination_port != des_port)
                        break;

#if (CLI_SUPPORT_PORT_NAME == 1)
                    {
                        UI8_T name[MAXSIZE_ifName+1] = {0};
                        CLI_LIB_Ifindex_To_Name(mirroring_entry.mirror_destination_port,name);
                        CLI_LIB_PrintStr_1(" Destination Port (listen port): %s\r\n",name);
                        CLI_LIB_Ifindex_To_Name(mirroring_entry.mirror_source_port,name);
                        CLI_LIB_PrintStr_1(" Source Port (monitored port)  : %s\r\n",name);
                    }
#else
                    SWCTRL_POM_LogicalPortToUserPort(mirroring_entry.mirror_destination_port,&unit,&port,&trunk_id);
                    CLI_LIB_PrintStr_2(" Destination Port (listen port): Eth%lu/%2lu\r\n",unit,port);

                    SWCTRL_POM_LogicalPortToUserPort(mirroring_entry.mirror_source_port,&unit,&port,&trunk_id);
                    CLI_LIB_PrintStr_2(" Source Port (monitored port)  : Eth%lu/%2lu\r\n",unit,port);
#endif

                    switch(mirroring_entry.mirror_type)
                    {
                    case VAL_mirrorType_rx:
                        CLI_LIB_PrintStr(" Mode                         :RX\r\n\r\n");
                        break;

                    case VAL_mirrorType_tx:
                        CLI_LIB_PrintStr(" Mode                         :TX\r\n\r\n");
                        break;

                    case VAL_mirrorType_both:
                        CLI_LIB_PrintStr(" Mode                         :RX/TX\r\n\r\n");
                        break;

                    default:
                        /*CLI_LIB_PrintStr("Failed to get port monitor status\r\n");*/
                        return CLI_NO_ERROR;
                    } /* end of switch(mirroring_entry.mirror_type) */
                } /* end of while (SWCTRL_POM_GetNextMirrorEntry(&mirroring_entry)) */
            } /* end of else of if( (verify_ret = verify_ethernet ... */
        } /* end of if (*arg[0] == 'e' || *arg[0] == 'E') */
   }

   return CLI_NO_ERROR;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - CLI_API_Port_Monitor
 *------------------------------------------------------------------------------
 * PURPOSE : This function will set port monitor
 * INPUT   : cmd_idx -- command idx
 *           arg     -- configures arguments
 *           ctrl_P  -- working area
 * OUTPUT  : none
 * RETURN  : CLI_NO_ERROR
 * NOTE    : 1. VLAN Mirror
 *           2. MAC Mirror
 *           3. Port Mirror
 *------------------------------------------------------------------------------
 */
/*configuration*/
UI32_T CLI_API_Port_Monitor(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T i;
    UI32_T count = 0;
    UI32_T src_ifindex;
    UI32_T dest_ifindex;
    UI32_T mode;

    UI32_T verify_unit;

    UI32_T verify_src_port;
    UI32_T verify_dest_port = 0;

   CLI_API_EthStatus_T verify_ret;

    for (i = 1 ; i <= ctrl_P->sys_info.max_port_number ; i++)/*just allow a port to mirror*/
    {
        if (ctrl_P->CMenu.port_id_list[(UI32_T)((i-1)/8)]  & (1 << ( 7 - ((i-1)%8))) )
        {
            verify_dest_port = i;

            if(count++ > 1)
            {
#if (SYS_CPNT_EH == TRUE)
                CLI_API_SetandShow_Exception_Handeler_Msg(EH_TYPE_MSG_CMD_NOT_FOR_MULT_PORTS, CLI_API_PORT_MONITOR, NULL);
#else
                CLI_LIB_PrintStr("This command only can set one port\r\n");
#endif
                return CLI_NO_ERROR;
            }
        }
    }

    verify_unit = ctrl_P->CMenu.unit_id;
    if( (verify_ret = verify_ethernet(verify_unit, verify_dest_port, &dest_ifindex)) != CLI_API_ETH_OK)
    {
        display_ethernet_msg(verify_ret, verify_unit, verify_dest_port);
        return CLI_NO_ERROR;
    }

    /*--------------------------------------------------------
     * Rule 1: VLAN Mirror -- *arg[0] v|V
     *--------------------------------------------------------*/
#if (SYS_CPNT_VLAN_MIRROR == TRUE)

    if (*arg[0] == 'v' || *arg[0] == 'V') /* VLAN argument shall be used */
    {
        UI32_T vid = atoi(arg[1]);
        /* get vlan status */

        if (VLAN_POM_IsVlanExisted(vid) == FALSE)
        {
            CLI_LIB_PrintStr_1("No such VLAN %lu\r\n", vid);
            return CLI_NO_ERROR;
        }

        /* for port monitor command, set/remove vlan mirroring */
        switch(cmd_idx)
        {
            case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_PORT_MONITOR:
                /* add vlan mirror by vlan id and destination port */
                if (SWCTRL_PMGR_AddVlanMirror(vid, dest_ifindex) != TRUE)
                {
                    CLI_LIB_PrintStr("Failed to add vlan mirroring\r\n");
                }
                break;

            case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_NO_PORT_MONITOR:
                /* delete vlan mirror from vlan id and destination port */
                if (SWCTRL_PMGR_DeleteVlanMirror(vid, dest_ifindex) != TRUE)
                {
                    CLI_LIB_PrintStr("Failed to remove vlan mirroring\r\n");
                }
                break;

            default:
                return CLI_ERR_INTERNAL;
        }/* End of switch(cmd_idx) */

        /* leave this command function when is completed */
        return CLI_NO_ERROR;
    }/* End of if (*arg[0] == 'v' || *arg[0] == 'V')*/

#endif /* End of #if (SYS_CPNT_VLAN_MIRROR == TRUE) */

    /*--------------------------------------------------------
     * Rule 2: MAC Mirror -- *arg[0] m|M
     *--------------------------------------------------------*/
#if (SYS_CPNT_MAC_BASED_MIRROR == TRUE)

    if (*arg[0] == 'm' || *arg[0] == 'M')
    {
        UI8_T  macAddress[SYS_ADPT_MAC_ADDR_LEN] = {0};

        /* MAC address */
        CLI_LIB_ValsInMac(arg[1], macAddress);

        /* add/remove mac-address mirror entry */
        switch(cmd_idx)
        {
            case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_PORT_MONITOR:
                if (SWCTRL_PMGR_SetMacMirrorEntry(dest_ifindex, macAddress) == FALSE)
                {
                    CLI_LIB_PrintStr("Failed to add mac-address mirroring\r\n");
                }
                break;

            case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_NO_PORT_MONITOR:
                if (SWCTRL_PMGR_DeleteMacMirrorEntry(dest_ifindex, macAddress) == FALSE)
                {
                    CLI_LIB_PrintStr("Failed to remove mac-address mirroring\r\n");
                }
                break;

            default:
                return CLI_ERR_INTERNAL;
        }/* End of switch(cmd_idx) */

        /* leave this command function when is completed */
        return CLI_NO_ERROR;
    }/* End of if (*arg[0] == 'v' || *arg[0] == 'V')*/

#endif /* End of #if (SYS_CPNT_VLAN_MIRROR == TRUE) */

#if (SYS_CPNT_ACL_MIRROR == TRUE)

    if (*arg[0] == 'a' || *arg[0] == 'A')
    {
        switch(cmd_idx)
        {
            case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_PORT_MONITOR:
                if (RULE_TYPE_OK != L4_PMGR_ACL_AddAclMirrorEntry(dest_ifindex, arg[1]))
                {
                    CLI_LIB_PrintStr("Failed to add ACL mirroring\r\n");
                }
                break;

            case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_NO_PORT_MONITOR:
                if (RULE_TYPE_OK != L4_PMGR_ACL_RemoveAclMirrorEntry(dest_ifindex, arg[1]))
                {
                    CLI_LIB_PrintStr("Failed to remove ACL mirroring\r\n");
                }
                break;

            default:
                return CLI_ERR_INTERNAL;
        }

        return CLI_NO_ERROR;
    }

#endif /* #if (SYS_CPNT_ACL_MIRROR == TRUE) */

/*--------------------------------------------------------
 * Rule 3: Port Mirror -- *arg[0] e|E
 *--------------------------------------------------------*/

    if (*arg[0] == 'e' || *arg[0] == 'E')
    {
        /* the rest of part will set ethernet port monitor,
         * when the first *arg[0] == 'e' or 'E',
         */

        verify_unit     = atoi((char*)arg[1]);
        verify_src_port = atoi(strchr((char*)arg[1],'/')+1);

        /*2004/5/19 02:00pm pttch add for module provision*/
#if (SYS_CPNT_3COM_CLI == FALSE)
        if (ctrl_P->sess_type == CLI_TYPE_PROVISION)
        {
#if ( SYS_CPNT_UNIT_HOT_SWAP == TRUE )
            #if 0
            UI32_T ifindex;  /* modified by Jinhua Wei ,to remove warning ,becaued the variable never used */
            #endif
            UI8_T  cmd_buff[CLI_DEF_MAX_BUFSIZE+1] = {0};

            UI8_T  master_id = 0;  /* modified by Jinhua Wei ,to remove warning ,becaued the variable type defined isn't match the following function's parameter type */

            STKTPLG_POM_GetMasterUnitId( & master_id );

            /*   if the port is module port, save command in buffer
             */
            if ( TRUE == STKTPLG_POM_IsModulePort( verify_unit, verify_src_port ) )
            {
                sprintf((char*)cmd_buff,"!\ninterface ethernet %lu/%lu\n", ctrl_P->CMenu.unit_id, verify_dest_port);
                CLI_MGR_AddDeviceCfg( verify_unit + SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK, cmd_buff );

                memset(cmd_buff, 0, sizeof(cmd_buff));

                if(arg[2]!= NULL)
                    sprintf((char*)cmd_buff," port monitor %s %s %s\n!\n", arg[0], arg[1], arg[2]);
                CLI_MGR_AddDeviceCfg( verify_unit + SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK, cmd_buff );
                return CLI_NO_ERROR;
            }
#else
            UI32_T ifindex = 0;
            BOOL_T is_inherit        =TRUE;
            UI8_T  cmd_buff[CLI_DEF_MAX_BUFSIZE+1] = {0};

            SWCTRL_POM_UIUserPortToIfindex(verify_unit, verify_src_port, &ifindex, &is_inherit);

            /*if the port is module port, save command in buffer*/
            if (TRUE == CLI_MGR_IsModulePort(ifindex))
            {
                sprintf(cmd_buff,"!\ninterface ethernet %lu/%lu\n", ctrl_P->CMenu.unit_id, verify_dest_port);
                CLI_MGR_AddModuleCfg(verify_unit, cmd_buff);
                memset(cmd_buff, 0, sizeof(cmd_buff));

                if(arg[2]!= NULL)
                    sprintf(cmd_buff," port monitor %s %s %s\n!\n", arg[0], arg[1], arg[2]);
                CLI_MGR_AddModuleCfg(verify_unit, cmd_buff);
                return CLI_NO_ERROR;
            }
#endif
        } /* end of if (ctrl_P->sess_type == CLI_TYPE_PROVISION) */
#endif

#if (CLI_SUPPORT_PORT_NAME == 1)
        if (isdigit(arg[1][0]))
        {
            verify_unit = atoi(arg[1]);
            verify_src_port = atoi(strchr(arg[1],'/')+1);
        }
        else/*port name*/
        {
            UI32_T trunk_id = 0;
            if (!IF_PMGR_IfnameToIfindex(arg[1], &src_ifindex))
            {
#if (SYS_CPNT_EH == TRUE)
                CLI_API_Show_Exception_Handeler_Msg();
#else
                CLI_LIB_PrintStr_1("%s does not exist\r\n",arg[1]);
#endif
                return CLI_NO_ERROR;
            }
            SWCTRL_POM_LogicalPortToUserPort(src_ifindex, &verify_unit, &verify_src_port, &trunk_id);
        }
#endif

        verify_ret = verify_ethernet(verify_unit, verify_src_port, &src_ifindex);
        if(verify_ret == CLI_API_ETH_NOT_PRESENT || verify_ret == CLI_API_ETH_UNKNOWN_PORT)
        {
            display_ethernet_msg(verify_ret, verify_unit, verify_src_port);
            return CLI_NO_ERROR;
        }
    } /* end of if (*arg[0] == 'e' || *arg[0] == 'E') */

    mode = VAL_mirrorType_both;
    if(arg[2]!= NULL)
    {
        switch(*arg[2])
        {
        case 'r':
        case 'R':
            mode = VAL_mirrorType_rx;
            break;

        case 't':
        case 'T':
            mode = VAL_mirrorType_tx;
            break;

        case 'b':
        case 'B':
        default:
            mode = VAL_mirrorType_both;
            break;
        }
    }

    switch(cmd_idx)
    {
    case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_PORT_MONITOR:
        if (!SWCTRL_PMGR_SetMirrorStatus(src_ifindex, dest_ifindex, VAL_mirrorStatus_valid))
        {
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
#else
            CLI_LIB_PrintStr("Failed to enable mirror port\r\n");
#endif
        }
        else if (!SWCTRL_PMGR_SetMirrorType(src_ifindex, dest_ifindex, mode))
        {
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
#else
            CLI_LIB_PrintStr("Failed to set mirror port\r\n");
#endif
         /*restore*/
            SWCTRL_PMGR_SetMirrorStatus(src_ifindex, dest_ifindex, VAL_mirrorStatus_invalid);
        }
      break;

    case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_NO_PORT_MONITOR:
        if (!SWCTRL_PMGR_SetMirrorStatus(src_ifindex, dest_ifindex, VAL_mirrorStatus_invalid) )
        {
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
#else
            CLI_LIB_PrintStr("Failed to remove mirror port\r\n");
#endif
        }
        break;

    default:
        return CLI_ERR_INTERNAL;
   } /* end of switch(cmd_idx) */
   return CLI_NO_ERROR;
}
