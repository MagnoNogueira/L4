#include <stdio.h>
#include "cli_mgr.h"
#include "cli_api.h"
#include "sys_cpnt.h"
#include "l_inet.h"
#include "temp.h"

#if (SYS_CPNT_SYSLOG == TRUE)
//#include "syslog_pmgr.h"
#endif

#include "swctrl_pom.h"



#if 0
#if (SYS_CPNT_STP == SYS_CPNT_STP_TYPE_MSTP)
#include "xstp_pmgr.h"
#include "xstp_mgr.h"
#include "xstp_type.h"
#endif

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    #if (SYS_CPNT_QOS_V2 != TRUE)
        #include "l4_ds_type.h"
        #include "l4_ds_pmgr.h"
    #endif

    #include "l4_pmgr.h"
#endif
#endif

#pragma mark -
#pragma mark #if 0
#if 0
/**********************************<<GENELAL>>***********************************************/
/*change mode*/
UI32_T CLI_API_Exit(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
   switch(ctrl_P->CMenu.AccMode)
   {
   case NORMAL_EXEC_MODE:
   case PRIVILEGE_EXEC_MODE:

#if (SYS_CPNT_CLI_TERMINAL == TRUE) || (SYS_CPNT_CLI_FILTERING == TRUE)
        CLI_LIB_SetFilteringTerminalDefault(ctrl_P);
#endif  /* #if (SYS_CPNT_CLI_TERMINAL == TRUE) || (SYS_CPNT_CLI_FILTERING == TRUE) */

        if(ctrl_P->sess_type == CLI_TYPE_PROVISION)
//         CLI_MGR_Set_Provision_Flag(FALSE);
 #if (SYS_CPNT_SYSLOG == TRUE)
        {
           char  temp[30] = {0};
           char ip[L_INET_MAX_IPADDR_STR_LEN + 1] = {0};
          //UI8_T ip[16] = {0};
          SYSLOG_OM_RecordOwnerInfo_T       owner_info;

          owner_info.level = SYSLOG_LEVEL_INFO;
          owner_info.module_no = SYS_MODULE_CLI;

          owner_info.function_no = 0;
          owner_info.error_no = 1;

          /*if (ctrl_P->CMenu.RemoteTelnetClientIpAddr == 0)*/
          if (ctrl_P->CMenu.RemoteSessionType == CLI_TYPE_UART)
          {
             sprintf(temp, "Console user");
          }
          else
          {
             if (ctrl_P->CMenu.RemoteSessionType == CLI_TYPE_SSH)
             {
               // L_INET_Ntoa(ctrl_P->CMenu.RemoteTelnetClientIpAddr, ip);
                L_INET_InaddrToString((L_INET_Addr_T *)&ctrl_P->CMenu.TelnetIPAddress, L_INET_MAX_IPADDR_STR_LEN, ip);
                sprintf(temp, "%s SSH user",ip);
             }
             else
             {
                //L_INET_Ntoa(ctrl_P->CMenu.RemoteTelnetClientIpAddr, ip);
                L_INET_InaddrToString((L_INET_Addr_T *)&ctrl_P->CMenu.TelnetIPAddress, L_INET_MAX_IPADDR_STR_LEN, ip);
                sprintf(temp, "%s VTY user",ip);
             }
          }
          SYSLOG_PMGR_AddFormatMsgEntry(&owner_info, LOGOUT_PRIVILEG_MODE_MESSAGE_INDEX, temp, ctrl_P->CMenu.UserName, 0);
        }
#endif /* #if (SYS_CPNT_SYSLOG == TRUE) */
      return CLI_EXIT_SESSION;

   case PRIVILEGE_CFG_GLOBAL_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;

   case PRIVILEGE_CFG_INTERFACE_ETH_MODE:
      memset(ctrl_P->CMenu.port_id_list, 0, sizeof(ctrl_P->CMenu.port_id_list));
      ctrl_P->CMenu.unit_id = 0;/*for stacking*/
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
   case PRIVILEGE_CFG_INTERFACE_ETH0_MODE: /*wf 20090106*/
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;
#endif
#if (SYS_CPNT_CRAFT_PORT == TRUE)
   case PRIVILEGE_CFG_INTERFACE_CRAFT_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;
#endif

#if (SYS_CPNT_FILTER_THROOTTLE == TRUE)
   case PRIVILEGE_CFG_IGMP_PROFILE_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;
#endif
   case PRIVILEGE_CFG_INTERFACE_VLAN_MODE:
      ctrl_P->CMenu.vlan_ifindex = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;

#if (SYS_CPNT_IP_TUNNEL == TRUE)
 case PRIVILEGE_CFG_INTERFACE_TUNNEL_MODE:
      ctrl_P->CMenu.vlan_ifindex = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;
#endif

   case PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE:
      ctrl_P->CMenu.loopback_id = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;

   case PRIVILEGE_CFG_INTERFACE_PCHANNEL_MODE:
      ctrl_P->CMenu.pchannel_id = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;

   case PRIVILEGE_CFG_VLAN_DATABASE_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;

   case PRIVILEGE_CFG_LINE_MODE:
      ctrl_P->CMenu.line_type = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;

   case PRIVILEGE_CFG_MSTP_MODE:
#if (SYS_CPNT_STP == SYS_CPNT_STP_TYPE_MSTP)
      // XSTP_PMGR_RestartStateMachine();
#endif
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;

   case PRIVILEGE_CFG_DOMAIN_MODE:
      ctrl_P->CMenu.domain_index = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;

   case PRIVILEGE_CFG_ROUTER_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;

   case PRIVILEGE_CFG_ROUTERDVMRP_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;

   case PRIVILEGE_CFG_ROUTEROSPF_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;

   case PRIVILEGE_CFG_ROUTEROSPF6_MODE:
     ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
     break;
#if (SYS_CPNT_BGP == TRUE)
    case PRIVILEGE_CFG_ROUTER_BGP_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;
#endif
#if (SYS_CPNT_NSM_POLICY == TRUE)
    case PRIVILEGE_CFG_ROUTE_MAP_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;
#endif
#if (CLI_SUPPORT_L3_FEATURE == 1)
   case PRIVILEGE_CFG_DHCPPOOL_MODE:
      memset(ctrl_P->CMenu.Pool_name, 0, sizeof(ctrl_P->CMenu.Pool_name));
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;
#endif

#if (SYS_CPNT_ACL == TRUE)
   case PRIVILEGE_CFG_ACL_MAC_MODE:
   case PRIVILEGE_CFG_ACL_STANDARD_MODE:
   case PRIVILEGE_CFG_ACL_EXTENDED_MODE:
   case PRIVILEGE_CFG_ACL_IP_MASK_MODE:
   case PRIVILEGE_CFG_ACL_MAC_MASK_MODE:
#if (SYS_CPNT_DAI == TRUE)
   case PRIVILEGE_CFG_ACL_ARP_MODE:
#endif

#if (SYS_CPNT_ACL_IPV6 == TRUE)
   case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE:
   case PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE:
#endif
      ctrl_P->CMenu.acl_type = 0;
      ctrl_P->CMenu.acl_name[0] = 0;
      ctrl_P->CMenu.mask_mode = 0;
      ctrl_P->CMenu.mask_type = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;
#endif
#if (SYS_CPNT_AAA == TRUE)
   case PRIVILEGE_CFG_AAA_SG_MODE:
   case PRIVILEGE_CFG_AAA_SG_RADIUS_MODE:
      ctrl_P->CMenu.aaa_sg_name[0] = 0;
      ctrl_P->CMenu.aaa_sg_type = 0;
      ctrl_P->CMenu.aaa_sg_group_index = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;
#endif /*#if (SYS_CPNT_AAA == TRUE)*/

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
   case PRIVILEGE_CFG_CLASS_MAP_MODE:

#if(SYS_CPNT_QOS_V2 != TRUE)
      {
        UI32_T map_id = 0;
        L4_TYPE_ClassMapEntry_T rec_p;
        if(L4_DS_PMGR_GetClassMapIdByName(ctrl_P->CMenu.cmap_name, &map_id) != TRUE)
        {
            CLI_LIB_PrintStr("Failed to get class map id\n\r");
        }
        else
        {
            if(L4_DS_PMGR_GetClassMap(map_id, &rec_p) != TRUE)
            {
                CLI_LIB_PrintStr("Failed to get existing class map\n\r");
            }
        }
        rec_p.status = L_RSTATUS_ACTIVE;
        if (L4_DS_PMGR_SetClassMap(map_id,&rec_p)!= TRUE)
        {
            CLI_LIB_PrintStr_1((char *)"Failed to set class map %s\n\r", rec_p.name);
        }
      }
#endif 	/* #if (SYS_CPNT_QOS_V2 != TRUE) */

      ctrl_P->CMenu.cmap_name[0] = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;

   case PRIVILEGE_CFG_POLICY_MAP_MODE:

#if (SYS_CPNT_QOS_V2 != TRUE)
      {
        UI32_T map_id = 0;
        if(L4_DS_PMGR_GetPolicyMapIdByName(ctrl_P->CMenu.pmap_name, &map_id) != TRUE)
        {
            CLI_LIB_PrintStr("Failed to get policy map id\n\r");
        }

        if(L4_DS_PMGR_ActivePolicyMap(map_id) != TRUE)
        {
            CLI_LIB_PrintStr_1("Failed to active policy map %s\n\r", ctrl_P->CMenu.pmap_name);
        }
      }
#endif /* #if (SYS_CPNT_QOS_V2 != TRUE) */

      ctrl_P->CMenu.pmap_name[0] = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;

   case PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE:
      ctrl_P->CMenu.cmap_name[0] = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_POLICY_MAP_MODE;
      break;
#endif

#if(SYS_CPNT_TIME_BASED_ACL==TRUE)
   case PRIVILEGE_CFG_TIME_RANGE_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;
#endif

#if (SYS_CPNT_ERPS == TRUE)
   case PRIVILEGE_CFG_ERPS_MODE:
      ctrl_P->CMenu.erps_domain_id = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
      break;
#endif

   default:
      return CLI_ERR_INTERNAL;
   }

   return CLI_NO_ERROR;
}

UI32_T CLI_API_End(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{

   if(ctrl_P->sess_type == CLI_TYPE_PROVISION)
   {
//      CLI_MGR_Set_Provision_Flag(FALSE);
//      CLI_MGR_Set_PrintProvisionMsg(TRUE);
      return CLI_EXIT_SESSION;
   }

   switch(ctrl_P->CMenu.AccMode)
   {
   case PRIVILEGE_CFG_GLOBAL_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;

   case PRIVILEGE_CFG_INTERFACE_ETH_MODE:
      memset(ctrl_P->CMenu.port_id_list, 0, sizeof(ctrl_P->CMenu.port_id_list));
      ctrl_P->CMenu.unit_id = 0;/*for stacking*/
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
   case PRIVILEGE_CFG_INTERFACE_ETH0_MODE: /*wf 20090106*/
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;
#endif
#if (SYS_CPNT_CRAFT_PORT == TRUE)
   case PRIVILEGE_CFG_INTERFACE_CRAFT_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;
#endif
#if (SYS_CPNT_FILTER_THROOTTLE == TRUE)
   case PRIVILEGE_CFG_IGMP_PROFILE_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;
#endif
   case PRIVILEGE_CFG_INTERFACE_VLAN_MODE:
      ctrl_P->CMenu.vlan_ifindex = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;
#if (SYS_CPNT_IP_TUNNEL == TRUE)
  case PRIVILEGE_CFG_INTERFACE_TUNNEL_MODE:
      ctrl_P->CMenu.vlan_ifindex = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;
#endif
   case PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE:
      ctrl_P->CMenu.loopback_id = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;

   case PRIVILEGE_CFG_INTERFACE_PCHANNEL_MODE:
      ctrl_P->CMenu.pchannel_id = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;

   case PRIVILEGE_CFG_VLAN_DATABASE_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;

   case PRIVILEGE_CFG_LINE_MODE:
      ctrl_P->CMenu.line_type = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;

   case PRIVILEGE_CFG_MSTP_MODE:
#if (SYS_CPNT_STP == SYS_CPNT_STP_TYPE_MSTP)
      // XSTP_PMGR_RestartStateMachine();
#endif
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;

   case PRIVILEGE_CFG_DOMAIN_MODE:
      ctrl_P->CMenu.domain_index = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;
   case PRIVILEGE_CFG_ROUTER_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;

   case PRIVILEGE_CFG_ROUTERDVMRP_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;

   case PRIVILEGE_CFG_ROUTEROSPF_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;

  case PRIVILEGE_CFG_ROUTEROSPF6_MODE:
     ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
     break;
#if (SYS_CPNT_BGP == TRUE)
    case PRIVILEGE_CFG_ROUTER_BGP_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;
#endif
#if (SYS_CPNT_NSM_POLICY == TRUE)
    case PRIVILEGE_CFG_ROUTE_MAP_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;
#endif
#if (CLI_SUPPORT_L3_FEATURE == 1)
   case PRIVILEGE_CFG_DHCPPOOL_MODE:
      memset(ctrl_P->CMenu.Pool_name, 0, sizeof(ctrl_P->CMenu.Pool_name));
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;
#endif

#if (SYS_CPNT_ACL == TRUE)
   case PRIVILEGE_CFG_ACL_MAC_MODE:
   case PRIVILEGE_CFG_ACL_STANDARD_MODE:
   case PRIVILEGE_CFG_ACL_EXTENDED_MODE:
   case PRIVILEGE_CFG_ACL_IP_MASK_MODE:
   case PRIVILEGE_CFG_ACL_MAC_MASK_MODE:
#if (SYS_CPNT_DAI == TRUE)
   case PRIVILEGE_CFG_ACL_ARP_MODE:
#endif

#if (SYS_CPNT_ACL_IPV6 == TRUE)
   case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE:
   case PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE:
#endif
      ctrl_P->CMenu.acl_type = 0;
      ctrl_P->CMenu.acl_name[0] = 0;
      ctrl_P->CMenu.mask_mode = 0;
      ctrl_P->CMenu.mask_type = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;
#endif
#if (SYS_CPNT_AAA == TRUE)
   case PRIVILEGE_CFG_AAA_SG_MODE:
   case PRIVILEGE_CFG_AAA_SG_RADIUS_MODE:
      ctrl_P->CMenu.aaa_sg_name[0] = 0;
      ctrl_P->CMenu.aaa_sg_type = 0;
      ctrl_P->CMenu.aaa_sg_group_index = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;
#endif /*#if (SYS_CPNT_AAA == TRUE)*/

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
   case PRIVILEGE_CFG_CLASS_MAP_MODE:

#if (SYS_CPNT_QOS_V2 != TRUE)
      {
        UI32_T map_id = 0;
        L4_TYPE_ClassMapEntry_T rec_p;
        if(L4_DS_PMGR_GetClassMapIdByName(ctrl_P->CMenu.cmap_name, &map_id) != TRUE)
        {
            CLI_LIB_PrintStr("Failed to get class map id\n\r");
        }
        else
        {
            if(L4_DS_PMGR_GetClassMap(map_id, &rec_p) != TRUE)
            {
                CLI_LIB_PrintStr("Failed to get existing class map\n\r");
            }
        }
        rec_p.status = L_RSTATUS_ACTIVE;
        if (L4_DS_PMGR_SetClassMap(map_id,&rec_p)!= TRUE)
        {
            CLI_LIB_PrintStr_1("Failed to set class map %s\n\r", rec_p.name);
        }
      }
#endif /* #if (SYS_CPNT_QOS_V2 != TRUE) */
      ctrl_P->CMenu.cmap_name[0] = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;
   case PRIVILEGE_CFG_POLICY_MAP_MODE:
   case PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE:

#if (SYS_CPNT_QOS_V2 != TRUE)
      {
        UI32_T map_id = 0;
        if(L4_DS_PMGR_GetPolicyMapIdByName(ctrl_P->CMenu.pmap_name, &map_id) != TRUE)
        {
            CLI_LIB_PrintStr("Failed to get policy map id\n\r");
        }
        if(L4_DS_PMGR_ActivePolicyMap(map_id) != TRUE)
        {
            CLI_LIB_PrintStr_1("Failed to active policy map %s\n\r", ctrl_P->CMenu.pmap_name);
        }
      }
#endif /* #if (SYS_CPNT_QOS_V2 != TRUE) */
      ctrl_P->CMenu.cmap_name[0] = 0;
      ctrl_P->CMenu.pmap_name[0] = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;
#endif

#if(SYS_CPNT_TIME_BASED_ACL==TRUE)
   case PRIVILEGE_CFG_TIME_RANGE_MODE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;
#endif

#if (SYS_CPNT_ERPS == TRUE)
   case PRIVILEGE_CFG_ERPS_MODE:
      ctrl_P->CMenu.erps_domain_id = 0;
      ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
      break;
#endif

   default:
      return CLI_ERR_INTERNAL;
   }

   return CLI_NO_ERROR;
}

UI32_T CLI_API_Quit(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
   switch(ctrl_P->CMenu.AccMode)
   {
   case NORMAL_EXEC_MODE:
   case PRIVILEGE_EXEC_MODE:
   case PRIVILEGE_CFG_DOMAIN_MODE:
      if(ctrl_P->sess_type == CLI_TYPE_PROVISION)
//         CLI_MGR_Set_Provision_Flag(FALSE);
      return CLI_EXIT_SESSION;

   default:
      return CLI_ERR_INTERNAL;
   }
   return CLI_NO_ERROR;
}

UI32_T CLI_API_Disable(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T attempt_privilege = 0;

#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
    if (arg[0])
    {
        attempt_privilege = atoi(arg[0]);
    }
#endif /* SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL */

    if (attempt_privilege <= ctrl_P->CMenu.AccessPrivilege)
    {
        switch (attempt_privilege)
        {
            case 0:
                ctrl_P->CMenu.AccMode = NORMAL_EXEC_MODE;
                break;

            case 15:
            default:
                ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
                break;
        }

        ctrl_P->CMenu.AccessPrivilege = attempt_privilege;
    }
    else
    {
        CLI_LIB_PrintStr("Failed to disable.\r\n");
    }

    return CLI_NO_ERROR;
}

UI32_T CLI_API_Enable(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T  attempt_privilege;

    if (!arg[0])
        attempt_privilege = 15;
#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
    else
        attempt_privilege = atoi(arg[0]);
#else
    else if (arg[0][0] == '1')
        attempt_privilege = 15;
    else if (arg[0][0] == '0')
        attempt_privilege = 0;
    else
        return CLI_ERR_INTERNAL;
#endif /* SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL */

    if (CLI_AUTH_CheckEnablePassword(ctrl_P, &attempt_privilege) == TRUE)
    {
        switch (attempt_privilege)
        {
            case 0:
                ctrl_P->CMenu.AccMode = NORMAL_EXEC_MODE;
                break;

            case 15:
            default:
                ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
                break;
        }

        ctrl_P->CMenu.AccessPrivilege = attempt_privilege;
    }

    return CLI_NO_ERROR;
}

#endif
#pragma mark #endif
#pragma mark -


UI32_T CLI_API_Configure(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
   ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
   return CLI_NO_ERROR;
}

#pragma mark -
#pragma mark #if 0
#if 0
UI32_T CLI_API_Help(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{

   char *firstcol[] =
   {
      "Help may be requested at any point in a command by entering",
      "a question mark '?'. If nothing matches, the help list will",
      "be empty and you must backup until entering a '?' shows the",
      "available options.",
      "Two styles of help are provided:",
      "1. Full help is available when you are ready to enter a",
      "   command argument (e.g. 'show ?') and describes each possible",
      "   argument.",
      "2. Partial help is provided when an abbreviated argument is entered",
      "   and you want to know what arguments match the input",
      "   (e.g. 'show br?'.)",
#if DEVELOP_STAGE
      "Version of CLI: 0313",
#endif
      NULL
   };
   char *secondcol[1];
   secondcol[0] = NULL;

   CLI_LIB_DisplayMsg(firstcol, secondcol, CLI_DEF_MAX_MSGNUM);

   return CLI_NO_ERROR;
}
#endif
#pragma mark #endif
#pragma mark -

BOOL_T Get_Line_Heads_From_Buffer(char *buffer, char **heads)
{
   BOOL_T is_wait_for_head = TRUE;
   UI16_T line_number = 0;

   for( ; buffer != NULL; buffer++)
   {
      switch(*buffer)
      {
      case '\n':
         *buffer = '\0';
         if(is_wait_for_head)
         {
            line_number ++;
            *heads = buffer;
            heads ++;
         }
         is_wait_for_head = TRUE;
          break;

      case '\0':
            /* CLI_LIB_PrintStr_1("line_number: %d\r\n", line_number); */
            return TRUE;

      default:
         if(is_wait_for_head)
         {
            line_number ++;
            *heads = buffer;
            heads ++;
            is_wait_for_head = FALSE;
         }
         break;
      }
   }
   return TRUE;
}

CLI_API_EthStatus_T verify_ethernet(UI32_T unit, UI32_T port, UI32_T *lport)
{
#if 0
    *lport = port;
    return CLI_API_ETH_OK;
#else
   SWCTRL_Lport_Type_T ret;
   BOOL_T is_inherit        =TRUE;
   *lport = 0;

   /*check module exist or not*/
   if(!SWCTRL_POM_UIUserPortExisting(unit, port))
      return CLI_API_ETH_NOT_PRESENT;

   /*check if this port is trunk member*/
   ret = SWCTRL_POM_UIUserPortToIfindex(unit, port, lport, &is_inherit);

   if( ret == SWCTRL_LPORT_TRUNK_PORT_MEMBER )
      return CLI_API_ETH_TRUNK_MEMBER;

   if( ret == SWCTRL_LPORT_UNKNOWN_PORT )
      return CLI_API_ETH_UNKNOWN_PORT;

   return CLI_API_ETH_OK;
 #endif
}


CLI_API_TrunkStatus_T verify_trunk(UI32_T trunk_id, UI32_T *lport)
{
#if 1
    *lport = trunk_id + SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER - 1;
    return CLI_API_TRUNK_OK;
#else
   TRK_MGR_TrunkEntry_T trunk_entry;
   *lport = 0;

   /*check trunk exist or not*/
   trunk_entry.trunk_index = trunk_id;

   if(!TRK_PMGR_GetTrunkEntry(&trunk_entry))
      return CLI_API_TRUNK_NOT_EXIST;

   if(TRK_PMGR_GetTrunkMemberCounts(trunk_id) == 0)
      return CLI_API_TRUNK_NO_MEMBER;

   if(!SWCTRL_POM_TrunkIDToLogicalPort(trunk_id, lport))
      return CLI_API_TRUNK_INVALID_RANGE;

   return CLI_API_TRUNK_OK;
#endif
}

void display_ethernet_msg(CLI_API_EthStatus_T status, UI32_T unit, UI32_T port)
{
   char buff[CLI_DEF_MAX_BUFSIZE] = {0};

#if (CLI_SUPPORT_PORT_NAME == 1)
   {
      UI32_T lport;
      char name[MAXSIZE_ifName+1] = {0};
      if( SWCTRL_LPORT_UNKNOWN_PORT == SWCTRL_POM_UIUserPortToIfindex(unit, port, &lport, &is_inherit))
      {
            return ;
      }
      CLI_LIB_Ifindex_To_Name(lport,name);

      switch(status)
      {
      case CLI_API_ETH_NOT_PRESENT:
         sprintf((char *)buff, "Ethernet %s is not present.\r\n", name);
      break;

      case CLI_API_ETH_TRUNK_MEMBER:
         sprintf(buff, "Trunk member, ethernet %s, could not be configured or displayed.\r\n", name);
         break;

      case CLI_API_ETH_UNKNOWN_PORT:
         sprintf(buff, "Ethernet %s is an unknown port.\r\n", name);
         break;

      case CLI_API_ETH_OK:
         sprintf(buff, "Ethernet %s is a normal port.\r\n", name);
         break;

      default:
         return;
      }
   }
#else
   switch(status)
   {
   case CLI_API_ETH_NOT_PRESENT:
      sprintf(buff, "Ethernet %lu/%lu is not present.\r\n", unit, port);
      break;

   case CLI_API_ETH_TRUNK_MEMBER:
      sprintf(buff, "Trunk member, ethernet %lu/%lu, could not be configured or displayed.\r\n", unit, port);
      break;

   case CLI_API_ETH_UNKNOWN_PORT:
      sprintf(buff, "Ethernet %lu/%lu is an unknown port.\r\n", unit, port);
      break;

   case CLI_API_ETH_OK:
      sprintf(buff, "Ethernet %lu/%lu is a normal port.\r\n", unit, port);
      break;

   default:
      return;
   }
#endif

   CLI_LIB_PrintStr(buff);

}

void display_trunk_msg(CLI_API_TrunkStatus_T status, UI32_T trunk_id)
{
   char buff[CLI_DEF_MAX_BUFSIZE] = {0};

   switch(status)
   {
   case CLI_API_TRUNK_NOT_EXIST:
      sprintf(buff, "Trunk %lu does not exist.\r\n", trunk_id);
      break;

   case CLI_API_TRUNK_NO_MEMBER:
      sprintf(buff, "Trunk %lu, which has no member, could not be configured or displayed.\r\n", trunk_id);
      break;

   case CLI_API_TRUNK_INVALID_RANGE:
      sprintf(buff, "Trunk ID: %lu is out of range.\r\n", trunk_id);
      break;

   case CLI_API_TRUNK_OK:
      sprintf(buff, "Trunk %lu is a normal trunk.\r\n", trunk_id);
      break;

   default:
      return;
   }

   CLI_LIB_PrintStr(buff);
}

BOOL_T str_to_nonprintable(char *str, UI8_T *om)
{
   UI32_T i;
   char   buff[3];

   if (strlen(str) == SYS_ADPT_MAX_ENCRYPTED_PASSWORD_LEN*2)
   {
      for (i = 0 ; i <SYS_ADPT_MAX_ENCRYPTED_PASSWORD_LEN; i++, om++)
      {
         buff[0] = *(str);
         buff[1] = *(str+1);
         buff[2] = 0;

         *om = (UI8_T)CLI_LIB_AtoUl(buff,16);
         str += 2;
      }
      *om = 0;
   }
   else
   {
#if (SYS_CPNT_EH == TRUE)
      EH_MGR_Handle_Exception1(SYS_MODULE_CLI, STR_TO_NONPRINTABLE, EH_TYPE_MSG_INVALID, SYSLOG_LEVEL_INFO, "password");
#endif
      return FALSE;
   }

   return TRUE;
}

#if 0
BOOL_T GetIfCounter(UI32_T ifindex, CLI_API_ShowIfCounter_T* IfCounter)
{
    BOOL_T NMTR_MGR_GetSystemWiseIfTableStats(UI32_T ifindex, SWDRV_IfTableStats_T *if_table_stats);

#if(SYS_CPNT_SYSTEM_WISE_COUNTER == TRUE)
   if(!NMTR_PMGR_GetSystemWiseIfTableStats(ifindex, &IfCounter->if_table_stats))
   {
#if (SYS_CPNT_EH == TRUE)
      CLI_API_Show_Exception_Handeler_Msg();
#else
      CLI_LIB_PrintStr("Failed to get IfTable Stats\r\n");
#endif
      return FALSE;
   }

   if(!NMTR_PMGR_GetSystemWiseIfXTableStats(ifindex, &IfCounter->if_xtable_stats))
   {
#if (SYS_CPNT_EH == TRUE)
      CLI_API_Show_Exception_Handeler_Msg();
#else
      CLI_LIB_PrintStr("Failed to get Extended IfTable Stats\r\n");
#endif
      return FALSE;
   }

   if(!NMTR_PMGR_GetSystemWiseEtherLikeStats(ifindex, &IfCounter->ether_like_stats))
   {
#if (SYS_CPNT_EH == TRUE)
      CLI_API_Show_Exception_Handeler_Msg();
#else
      CLI_LIB_PrintStr("Failed to get Ether-like Stats\r\n");
#endif
      return FALSE;
   }

   if(!NMTR_PMGR_GetSystemWiseEtherLikePause(ifindex, &IfCounter->ether_like_pause))
   {
#if (SYS_CPNT_EH == TRUE)
      CLI_API_Show_Exception_Handeler_Msg();
#else
      CLI_LIB_PrintStr("Failed to get Ether-like Pause Stats\r\n");
#endif
      return FALSE;
   }

   if(!NMTR_PMGR_GetSystemWiseRmonStats(ifindex, &IfCounter->rmon_stats))
   {
#if (SYS_CPNT_EH == TRUE)
      CLI_API_Show_Exception_Handeler_Msg();
#else
      CLI_LIB_PrintStr("Failed to get RMON Stats\r\n");
#endif
      return FALSE;
   }
#else

   if(!NMTR_PMGR_GetIfTableStats(ifindex, &IfCounter->if_table_stats))
   {
#if (SYS_CPNT_EH == TRUE)
      CLI_API_Show_Exception_Handeler_Msg();
#else
      CLI_LIB_PrintStr("Failed to get IfTable Stats\r\n");
#endif
      return FALSE;
   }

   if(!NMTR_PMGR_GetIfXTableStats(ifindex, &IfCounter->if_xtable_stats))
   {
#if (SYS_CPNT_EH == TRUE)
      CLI_API_Show_Exception_Handeler_Msg();
#else
      CLI_LIB_PrintStr("Failed to get Extended IfTable Stats\r\n");
#endif
      return FALSE;
   }

   if(!NMTR_PMGR_GetEtherLikeStats(ifindex, &IfCounter->ether_like_stats))
   {
#if (SYS_CPNT_EH == TRUE)
      CLI_API_Show_Exception_Handeler_Msg();
#else
      CLI_LIB_PrintStr("Failed to get Ether-like Stats\r\n");
#endif
      return FALSE;
   }

   if(!NMTR_PMGR_GetEtherLikePause(ifindex, &IfCounter->ether_like_pause))
   {
#if (SYS_CPNT_EH == TRUE)
      CLI_API_Show_Exception_Handeler_Msg();
#else
      CLI_LIB_PrintStr("Failed to get Ether-like Pause Stats\r\n");
#endif
      return FALSE;
   }

   if(!NMTR_PMGR_GetRmonStats(ifindex, &IfCounter->rmon_stats))
   {
#if (SYS_CPNT_EH == TRUE)
      CLI_API_Show_Exception_Handeler_Msg();
#else
      CLI_LIB_PrintStr("Failed to get RMON Stats\r\n");
#endif
      return FALSE;
   }
#endif /* #if(SYS_CPNT_SYSTEM_WISE_COUNTER == TRUE) */

   return TRUE;
}
#endif

#if (SYS_CPNT_EH == TRUE)
void CLI_API_Show_Exception_Handeler_Msg(void)
{
   UI32_T module_id;
   UI32_T function_no;
   UI32_T msg_flag;
   char  msg[CLI_DEF_MAX_MSGNUM];
   EH_MGR_Get_Exception_Info (&module_id, &function_no, &msg_flag, msg, sizeof(msg));
   /* to know whether this message is a debug message, implement as this. */
   if(EH_MGR_IS_FOR_DEBUG_MSG_PURPOSE(msg_flag))
   {
      /*debug msg do not show*/;
   }
   else
   {
      CLI_LIB_PrintStr(msg);
   }
   CLI_LIB_PrintNullStr(1);
   return;
}
void CLI_API_SetandShow_Exception_Handeler_Msg(EH_TYPE_Msg_T format_id, UI32_T function_no, char *arg)
{
   UI32_T module_id;
   UI32_T function_no1;
   UI32_T msg_flag;
   char   msg[CLI_DEF_MAX_MSGNUM];

   if (arg == NULL)
   {
      EH_MGR_Handle_Exception(SYS_MODULE_CLI, function_no, format_id, SYSLOG_LEVEL_INFO);
   }
   else
   {
      EH_MGR_Handle_Exception1(SYS_MODULE_CLI, function_no, format_id, SYSLOG_LEVEL_INFO, arg);
   }
   EH_MGR_Get_Exception_Info(&module_id, &function_no1, &msg_flag, msg, sizeof(msg));
   /* to know whether this message is a debug message, implement as this. */
   if(EH_MGR_IS_FOR_DEBUG_MSG_PURPOSE(msg_flag))
   {
      /*debug msg do not show*/;
   }
   else
   {
      CLI_LIB_PrintStr(msg);
   }
   CLI_LIB_PrintNullStr(1);
   return;
}
#endif

void CLI_API_Conver_Lower_Case(char *inStr, char *outStr)
{
    UI32_T i;
    for(i=0; inStr[i]!='\0'; i++)
    {
        if ((inStr[i] >= 'A') && (inStr[i] <= 'Z')) /*conver to lower case,ex A->a */
        {
            outStr[i] = inStr[i] + 32;
        }
        else
        {
            outStr[i] = inStr[i];
        }
    }
}


UI32_T CLI_API_Privilege(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
    UI8_T       pri_levl;
    char        *cmd_str_p;
    char        *lvl_str_p;
    BOOL_T      is_all = FALSE;

    switch (cmd_idx)
    {
    case PRIVILEGE_CFG_GLOBAL_CMD_W1_PRIVILEGE:
        if ((arg[1][0] == 'a') || (arg[1][0] == 'A'))
        {
            is_all = TRUE;
            lvl_str_p = arg[3];
            cmd_str_p = arg[4];
        }
        else
        {
            lvl_str_p = arg[2];
            cmd_str_p = arg[3];
        }

        pri_levl = atoi (lvl_str_p);

        if (FALSE == CLI_MGR_AddPrivLvlCmdEntry(arg[0], is_all, pri_levl, cmd_str_p))
        {
            CLI_LIB_PrintStr_1("Failed to set the privilege level of command %s \r\n", cmd_str_p);
        }
        break;

    case PRIVILEGE_CFG_GLOBAL_CMD_W2_NO_PRIVILEGE:
        if (NULL != arg[2])
        {
            is_all = TRUE;
            cmd_str_p = arg[2];
        }
        else
        {
            cmd_str_p = arg[1];
        }

        if (FALSE == CLI_MGR_DelPrivLvlCmdEntry(arg[0], is_all, cmd_str_p))
        {
            CLI_LIB_PrintStr_1("Failed to delete the privilege level of command %s \r\n", cmd_str_p);
        }
        break;

    default:
        return CLI_ERR_INTERNAL;
    }

#endif /* #if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE) */
    return CLI_NO_ERROR;
}

UI32_T CLI_API_Show_Privilege(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
    UI32_T line_num=0;
    char   buff[CLI_DEF_MAX_BUFSIZE] = {0};

    sprintf(buff, "Current privilege level is %ld\r\n", ctrl_P->CMenu.AccessPrivilege);
    PROCESS_MORE(buff);

#endif /* #if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE) */

    return CLI_NO_ERROR;
}

UI32_T CLI_API_Show_Privilege_Command(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
    UI32_T line_num=0;
    UI32_T                  iter_idx = 0;
    char                    mode_str [20];
    UI32_T                  len = sizeof(mode_str);
    CLI_PrivLvlCmdEntry_T   plce;
    char                    buff[CLI_DEF_MAX_BUFSIZE] = {0};

    while ( CLI_MGR_GetNextPrivilegeCmd (&iter_idx, &plce) == TRUE )
    {
        if (FALSE == CLI_LIB_GetCommandModeString(plce.cmd_mode, mode_str, &len))
            continue;

        sprintf(buff, "privilege %s%s level %d %s\r\n", mode_str,((plce.is_all == TRUE)? " all":""), plce.pri_level, plce.cmd_str);
        PROCESS_MORE(buff);
    }

#endif /* #if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE) */

    return CLI_NO_ERROR;
}

BOOL_T CLI_API_Get_Print_Interactive_Mode()
{
    return FALSE;
    //return TRUE;
}


void CLI_API_Set_Print_Interactive_Mode(BOOL_T mode)
{
    return;
}


