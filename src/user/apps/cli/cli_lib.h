#ifndef CLI_LIB_H
#define CLI_LIB_H

#include "sys_type.h"
#include "cli_def.h"

/*
 * Prompt definition by cjchu 2001/7/5
 */
#define NORMAL_EXEC_MODE_PROMPT                         ">"
#define PRIVILEGE_EXEC_MODE_PROMPT                      "#"
#define PRIVILEGE_CFG_GLOBAL_MODE_PROMPT                "(config)#"
#define PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE_PROMPT    "(config-if)#"
#define PRIVILEGE_CFG_INTERFACE_ETH_MODE_PROMPT         "(config-if)#"
#define PRIVILEGE_CFG_INTERFACE_IGMP_PROFILE_PROMPT     "(config-igmp-profile)#"
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
#define PRIVILEGE_CFG_INTERFACE_ETH0_MODE_PROMPT        "(config-if)#"
#endif
#if (SYS_CPNT_CRAFT_PORT == TRUE)
#define PRIVILEGE_CFG_INTERFACE_CRAFT_MODE_PROMPT       "(config-if)#"
#endif
#define PRIVILEGE_CFG_INTERFACE_VLAN_MODE_PROMPT        "(config-if)#"
#define PRIVILEGE_CFG_INTERFACE_PCHANNEL_MODE_PROMPT    "(config-if)#"
#define PRIVILEGE_CFG_INTERFACE_TUNNEL_MODE_PROMPT      "(config-if)#"

#if (SYS_CPNT_MSTP_SUPPORT_PVST==TRUE)
#define PRIVILEGE_CFG_VLAN_DATABASE_MODE_PROMPT         "(config-vlan-"
#else
#define PRIVILEGE_CFG_VLAN_DATABASE_MODE_PROMPT         "(config-vlan)#"
#endif
#define PRIVILEGE_CFG_MSTP_MODE_PROMPT                  "(config-mstp)#"
#define PRIVILEGE_CFG_DOMAIN_MODE_PROMPT                "(config-ether-cfm)#"
#define PRIVILEGE_CFG_LINE_MODE_PROMPT                  "(config-line)#"
#define PRIVILEGE_CFG_ROUTER_MODE_PROMPT                "(config-router)#"
#define PRIVILEGE_CFG_ROUTERDVMRP_MODE_PROMPT           "(config-router)#"
#define PRIVILEGE_CFG_ROUTEROSPF_MODE_PROMPT            "(config-router)#"
#define PRIVILEGE_CFG_DHCPPOOL_MODE_PROMPT              "(config-dhcp)#"
#if (SYS_CPNT_BGP == TRUE)
#define PRIVILEGE_CFG_ROUTER_BGP_MODE_PROMPT            "(config-router)#"
#endif
#if (SYS_CPNT_NSM_POLICY == TRUE)
#define PRIVILEGE_CFG_ROUTE_MAP_MODE_PROMPT             "(config-route-map)#"
#endif
#define PRIVILEGE_CFG_ACL_MAC_MODE_PROMPT               "(config-mac-acl)#"
#define PRIVILEGE_CFG_ACL_STANDARD_MODE_PROMPT          "(config-std-acl)#"
#define PRIVILEGE_CFG_ACL_EXTENDED_MODE_PROMPT          "(config-ext-acl)#"

#define PRIVILEGE_CFG_ACL_IP_MASK_MODE_PROMPT           "(config-ip-mask-acl)#"
#define PRIVILEGE_CFG_ACL_MAC_MASK_MODE_PROMPT          "(config-mac-mask-acl)#"
#define PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE_PROMPT     "(config-std-ipv6-acl)#"
#define PRIVILEGE_CFG_ACL_EXTEND_IPV6_MODE_PROMPT       "(config-ext-ipv6-acl)#"
#if (SYS_CPNT_DAI == TRUE)
#define PRIVILEGE_CFG_ACL_ARP_MODE_PROMPT               "(config-arp-acl)#"
#endif

#if (SYS_CPNT_AAA == TRUE)
#define PRIVILEGE_CFG_AAA_SG_RADIUS_MODE_PROMPT         "(config-sg-radius)#"
#define PRIVILEGE_CFG_AAA_SG_TACACS_PLUS_MODE_PROMPT    "(config-sg-tacacs+)#"
#endif /*#if (SYS_CPNT_AAA == TRUE)*/

#define PRIVILEGE_CFG_CLASS_MAP_MODE_PROMPT             "(config-cmap)#"
#define PRIVILEGE_CFG_POLICY_MAP_MODE_PROMPT            "(config-pmap)#"
#define PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE_PROMPT      "(config-pmap-c)#"

#if(SYS_CPNT_TIME_BASED_ACL==TRUE)
#define  PRIVILEGE_CFG_TIME_RANGE_MODE_PROMPT           "(config-time-range)#"
#endif

#if (SYS_CPNT_ERPS == TRUE)
#define PRIVILEGE_CFG_ERPS_MODE_PROMPT                  "(config-erps)#"
#endif

#define DEBUG_MODE_PROMPT                               "(debug)#"

#define NORMAL_EXEC_MODE_HELP_MSG                       "EXEC mode"
#define PRIVILEGE_EXEC_MODE_HELP_MSG                    "EXEC mode"
#define PRIVILEGE_CFG_GLOBAL_MODE_HELP_MSG              "Global configure mode"
#define PRIVILEGE_CFG_INTERFACE_ETH_MODE_HELP_MSG       "Privileged interface port configuration mode"
#define PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE_HELP_MSG  "Privileged interface loopback configuration mode"
#define PRIVILEGE_CFG_INTERFACE_IGMP_PROFILE_HELP_MSG   "Privileged interface IGMP profile configuration mode"
#define PRIVILEGE_CFG_INTERFACE_ETH0_MODE_HELP_MSG      "Privileged interface port0 configuration mode"
#define PRIVILEGE_CFG_INTERFACE_CRAFT_MODE_HELP_MSG     "Privileged interface craft configuration mode"
#define PRIVILEGE_CFG_INTERFACE_VLAN_MODE_HELP_MSG      "Privileged interface VLAN configuration mode"
#define PRIVILEGE_CFG_INTERFACE_PCHANNEL_MODE_HELP_MSG  "Privileged interface port channel configuration mode"
#define PRIVILEGE_CFG_INTERFACE_TUNNEL_MODE_HELP_MSG    "Privileged interface tunnel configuration mode"
#define PRIVILEGE_CFG_VLAN_DATABASE_MODE_HELP_MSG       "Virtual LAN database mode"
#define PRIVILEGE_CFG_MSTP_MODE_HELP_MSG                "MSTP configuration mode"
#define PRIVILEGE_CFG_DOMAIN_MODE_HELP_MSG              "Domain configuration mode"
#define PRIVILEGE_CFG_LINE_MODE_HELP_MSG                "Line configuration mode"
#define PRIVILEGE_CFG_ROUTER_MODE_HELP_MSG              "Router configuration mode"
#define PRIVILEGE_CFG_ROUTERDVMRP_MODE_HELP_MSG         "Router DVMRP configuration mode"
#define PRIVILEGE_CFG_ROUTEROSPF_MODE_HELP_MSG          "Router OSPF configuration mode"
#define PRIVILEGE_CFG_ROUTEROSPF6_MODE_HELP_MSG         "Router OSPF6 configuration mode"
#define PRIVILEGE_CFG_ROUTER_BGP_MODE_HELP_MSG          "Router BGP configuration mode"
#define PRIVILEGE_CFG_ROUTE_MAP_MODE_HELP_MSG           "Route-map configuration mode"
#define PRIVILEGE_CFG_DHCPPOOL_MODE_HELP_MSG            "DHCP pool configuration mode"
#define PRIVILEGE_CFG_ACL_MAC_MODE_HELP_MSG             "Access-list MAC mode"
#define PRIVILEGE_CFG_ACL_STANDARD_MODE_HELP_MSG        "Access-list IP standard mode"
#define PRIVILEGE_CFG_ACL_EXTENDED_MODE_HELP_MSG        "Access-list IP extended mode"
#define PRIVILEGE_CFG_ACL_IP_MASK_MODE_HELP_MSG         "Access-list IP mask mode"
#define PRIVILEGE_CFG_ACL_MAC_MASK_MODE_HELP_MSG        "Access-list MAC mask mode"
#define PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE_HELP_MSG   "Access-list IPv6 standard mode"
#define PRIVILEGE_CFG_ACL_EXTEND_IPV6_MODE_HELP_MSG     "Access-list IPv6 extended mode"
#define PRIVILEGE_CFG_ACL_ARP_MODE_HELP_MSG             "Access-list ARP mode"
#define PRIVILEGE_CFG_AAA_SG_RADIUS_MODE_HELP_MSG       "RADIUS server mode"
#define PRIVILEGE_CFG_AAA_SG_TACACS_PLUS_MODE_HELP_MSG  "TACACS+ server mode"
#define PRIVILEGE_CFG_CLASS_MAP_MODE_HELP_MSG           "Class map mode"
#define PRIVILEGE_CFG_POLICY_MAP_MODE_HELP_MSG          "Policy map mode"
#define PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE_HELP_MSG    "Policy map class mode"
#define PRIVILEGE_CFG_TIME_RANGE_MODE_HELP_MSG          "Time range mode"
#define PRIVILEGE_CFG_ERPS_MODE_HELP_MSG                "ERPS mode"
#define DEBUG_MODE_HELP_MSG                             "Debug mode"


#define CLI_LIB_BUF_SIZE                        1000

typedef enum CLI_AAA_ServerGroupType_E
{
    CLI_GROUP_RADIUS = 1,
    CLI_GROUP_TACACS_PLUS,
    CLI_GROUP_UNKNOWN /* for AAA_AccListEntryInterface_T, a method may map to a non-existed group */
} CLI_AAA_ServerGroupType_T;

#define MAX_TOKEN                               24
typedef enum{   CMP_AMBIGUOUS =1,
                CMP_FULL ,
                CMP_LIKE,
                CMP_NO_LIKE     }CLI_CMP_STATUS_T;

void CLI_LIB_Init();

UI16_T CLI_LIB_AddToList(UI32_T enter,UI8_T *list,UI8_T *type);
UI16_T CLI_LIB_IsNumber(UI8_T *token);
UI16_T CLI_LIB_CompareArg(UI8_T pro[][MAX_TOKEN],UI32_T count,UI8_T *token,UI8_T *anser);
UI16_T CLI_LIB_GetPassword(UI8_T *passwd);

#if (SYS_CPNT_SYSMGMT_DEFERRED_RELOAD == TRUE)
/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_ShowReloadInfo
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to show reload info
 * INPUT    : BOOL_T login - True (user login)
 * OUTPUT   :
 * RETURN   :
 * NOTE     : user login case - No messages when no reload configuration
 * ------------------------------------------------------------------------
 */
void CLI_LIB_ShowReloadInfo(BOOL_T login);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_ShowReloadNotify
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to show reload notify
 * INPUT    : CLI_TASK_WorkingArea_T *ctrl_P
 * OUTPUT   :
 * RETURN   :
 * NOTE     : ctrl_P used to display notification in every login user
 * ------------------------------------------------------------------------
 */
void CLI_LIB_ShowReloadNotify(CLI_TASK_WorkingArea_T *ctrl_P);
#endif /* SYS_CPNT_SYSMGMT_DEFERRED_RELOAD */

BOOL_T CLI_LIB_Print_CopyInProgressSign(void);

void    CLI_LIB_PrintStr(char *str);
int     CLI_LIB_Printf(const char *format, /* args*/ ...);
void    CLI_LIB_PrintLongStr(char *str);
void    CLI_LIB_PrintStrForCallback(CLI_TASK_WorkingArea_T *ctrl_P, char *str);

void    CLI_LIB_PutChar(char ch);
void    CLI_LIB_PrintNullStr(int line_num);
void    CLI_LIB_EraseDisp();
void    CLI_LIB_InitialHistBuf(CLI_TASK_WorkingArea_T *ctrl_P);
UI16_T  CLI_LIB_IsHistBufEmpty(CLI_TASK_WorkingArea_T *ctrl_P);
UI16_T  CLI_LIB_AddHistBuf(char *s, CLI_TASK_WorkingArea_T *ctrl_P);
UI16_T  CLI_LIB_GetHistHeadIdx(CLI_TASK_WorkingArea_T *ctrl_P);
UI16_T  CLI_LIB_GetHistPrevIdx(UI16_T *currIdx, CLI_TASK_WorkingArea_T *ctrl_P);
UI16_T  CLI_LIB_GetHistNextIdx(UI16_T *currIdx, CLI_TASK_WorkingArea_T *ctrl_P);
UI16_T  CLI_LIB_GetHistTailIdx(CLI_TASK_WorkingArea_T *ctrl_P);
char*  CLI_LIB_GetHistBuf(UI16_T index, CLI_TASK_WorkingArea_T *ctrl_P);
void    CLI_LIB_InitialEditMode(CLI_TASK_WorkingArea_T *ctrl_P);
void    CLI_LIB_EnableEditMode(CLI_TASK_WorkingArea_T *ctrl_P);
void    CLI_LIB_DisableEditMode(CLI_TASK_WorkingArea_T *ctrl_P);
UI16_T  CLI_LIB_GetEditModeStatus(CLI_TASK_WorkingArea_T *ctrl_P);
void    CLI_LIB_InitialDelBuf(CLI_TASK_WorkingArea_T *ctrl_P);
UI16_T  CLI_LIB_AddDelBuf(char *s, UI16_T sidx, UI16_T eidx, CLI_TASK_WorkingArea_T *ctrl_P);
UI16_T  CLI_LIB_GetDelBufFinalIdx(UI16_T *idx, CLI_TASK_WorkingArea_T *ctrl_P);
UI16_T  CLI_LIB_GetDelBufPrevIdx(UI16_T currIdx, CLI_TASK_WorkingArea_T *ctrl_P);
char*  CLI_LIB_GetDelBuf(UI16_T index,  CLI_TASK_WorkingArea_T *ctrl_P);
UI16_T  CLI_LIB_IsDelBufEmpty(CLI_TASK_WorkingArea_T *ctrl_P);
UI16_T  CLI_LIB_GetTermDispWidth(CLI_TASK_WorkingArea_T *ctrl_P);
void    CLI_LIB_ShowPromptMsg(CLI_TASK_WorkingArea_T *ctrl_P);
UI16_T  CLI_LIB_CheckNullStr(char *buff);
UI16_T  CLI_LIB_RedisplayCmd(char *buf, UI16_T str_idx, UI16_T disp_width, UI16_T type, CLI_TASK_WorkingArea_T *ctrl_P);
void    CLI_LIB_PrintFStr(char *str, UI16_T sidx, UI16_T eidx);
void    CLI_LIB_ScrollAt(char *old_buf, char *buf, UI16_T sidx, I16_T si, I16_T old_shift, I16_T new_shift, UI16_T disp_width);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_ScrollForBS
 * ------------------------------------------------------------------------
 * PURPOSE  : Process Command Line scroll for BS/DEL key
 *
 * INPUT    : *old_buf, *buf, sidx, si, old_shift, new_shift, disp_width
 * OUTPUT   :
 * RETURN   : new_shift
 * NOTE     :
 * ------------------------------------------------------------------------
 */
I16_T    CLI_LIB_ScrollForBS(char *old_buf, char *buf, UI16_T sidx, I16_T si, I16_T old_shift, I16_T new_shift, UI16_T disp_width);

I16_T   CLI_LIB_ScrollToLeft(char *old_buf, char *buf, I16_T old_si, I16_T si, I16_T old_shift, UI16_T disp_width);
I16_T   CLI_LIB_ScrollToRight(char *old_buf, char *buf, I16_T old_si, I16_T si, I16_T old_shift, UI16_T disp_width);
I16_T   CLI_LIB_ScrollToEnd(char *s1, char *s2, I16_T si, I16_T shift1, UI16_T disp_width);
I16_T   CLI_LIB_StrCmp(const char *s1, const char *s2, UI16_T *num);
UI16_T  CLI_LIB_StrECmp(char *s1, char *s2, UI16_T sidx, UI16_T eidx, I16_T shift1, I16_T shift2);
UI16_T  CLI_LIB_StrNCmp(char *s1, char *s2, UI16_T sidx1, UI16_T eidx1, UI16_T sidx2, UI16_T eidx2);
void    CLI_LIB_ShowErrMsg(UI16_T err_code, char *buff, UI16_T str_idx, CLI_TASK_WorkingArea_T *ctrl_P);
#if (SYS_CPNT_3COM_CLI == TRUE)
void    CLI_LIB_ShowQuHelp(UI16_T cmd_idx, UI16_T arg_idx, UI16_T final);
#else
void    CLI_LIB_ShowQuHelp(UI16_T cmd_idx, UI16_T arg_idx, UI16_T final);
#endif
void    CLI_LIB_ShowQuType3Msg(char *words[], UI16_T cnt);
UI16_T  CLI_LIB_DisplayMsg(char **firstColumn, char **secondColumn, UI32_T disp_line_num);
UI16_T  CLI_LIB_ProcMoreFeature();
UI16_T  CLI_LIB_PrintStrLine(char *str, UI16_T line_num);
UI16_T  CLI_LIB_CheckLineData(char *token, UI16_T *err_idx);
/* UI16_T  CLI_LIB_CheckNonData(UI8_T *token, UI8_T *arg_idx, UI16_T *match_idx); */
UI16_T  CLI_LIB_CheckNonData(char *token, UI16_T *arg_idx, UI16_T *match_idx);
/* UI16_T  CLI_LIB_CheckWordData(UI8_T *token, UI8_T arg_idx); */
UI16_T  CLI_LIB_CheckWordData(char *token, UI16_T arg_idx);
UI16_T  CLI_LIB_CheckIPAddr(char *tkn_buf, UI16_T *idx);
UI32_T  CLI_LIB_AtoUl(char *s, int radix);
I32_T  CLI_LIB_Atol(char *s, int radix);
int     CLI_LIB_AtoIp(char *s, UI8_T *ip);
int     CLI_LIB_AtoEa(char *s, char *en);
char*   CLI_LIB_ReverseStr(char *s);
void    CLI_LIB_UltoA(UI32_T n, char *s);
void    CLI_LIB_Reverse4Byte(UI8_T *ip_buf);
UI8_T   CLI_LIB_WaitNextPage(void);
UI16_T  CLI_LIB_CheckHexData(UI8_T *tkn_buf, UI16_T *idx);
UI8_T   CLI_LIB_HextoUI32_T(char *hexstr, UI32_T *data);
const   CMDLIST *CLI_LIB_Get_Cmd_List();
char  *CLI_LIB_Get_Token (char *s, char *Token, char delemiters[]);
void    CLI_LIB_PrintNullPrompt(CLI_TASK_WorkingArea_T *ctrl_P);  //cjchu 2001/7/5

BOOL_T  CLI_LIB_EAtoStr(char *mac, char *String);   //cjchu 2001/8/2
UI8_T   *CLI_LIB_get_send_buffer(UI8_T session_index);
void    HexAddrToStr(UI32_T HexAddr, char *OutStr, BOOL_T is_reverse);
UI64_T  CLI_LIB_AtoUll(char *in_str);
BOOL_T  CLI_LIB_Get_Lower_Upper_Value(char *buff, UI32_T *lower_val, UI32_T *upper_val, UI32_T *err_idx);
BOOL_T CLI_LIB_Bitmap_To_List(char *bitmap, char *list, UI32_T buff_len, UI32_T bit_numbers, BOOL_T is_first_bit_use);
BOOL_T CLI_LIB_Stack_Bitmap_To_List(char *bitmap, UI32_T *unit_id, char *list, UI32_T buffer_len, UI32_T bit_numbers, BOOL_T is_first_bit_use); //talor 2004-08-26
UI32_T  CLI_LIB_Get_DOTDATA_1000Times_Value(char *data, UI8_T data_len);

void  CLI_LIB_MSB_To_LSB(char *data, UI32_T data_len);
#if (CLI_SUPPORT_PORT_NAME == 1)
void CLI_LIB_Ifindex_To_Name(UI32_T ifindex, UI8_T *name);
#endif

#if (CLI_SUPPORT_QUOTE_STRING_FEATURE == TRUE)
BOOL_T CLI_LIB_Check_Is_QuoteStr(char *str);
BOOL_T CLI_LIB_Str_Add_Quote(char *in_str,char *out_str);
BOOL_T CLI_LIB_Str_Remove_Quote(char *str);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_Is_Str_Enclosed_withQuote
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to check if string starts with quote.
 *            If start with quote and not end with quote, add quote at end
 * INPUT    : in_str
 * OUTPUT   : out_str
 * RETURN   : TRUE : start with quote, will be enclosed with quote finally
 *            FALSE : not start with quote
 * NOTE     : if buffer have invalid char, function will return FALSE
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_LIB_Is_Str_Enclosed_withQuote(char *in_str, char *out_str);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_Str_Quote_Start
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to check if string starts with quote
 *
 * INPUT    : in_str
 * OUTPUT   : out_str
 * RETURN   : TRUE/FALSE
 * NOTE     : if buffer have invalid char, function will return FALSE
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_LIB_Str_Quote_Start(UI8_T *in_str, UI8_T *out_str);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_Check_Is_Legal_CharString
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to check if string is legal CharString
 *
 * INPUT    : str
 * OUTPUT   : error_index
 * RETURN   : TRUE/FALSE
 * NOTE     : if buffer have invalid char, function will return FALSE
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_LIB_Check_Is_Legal_CharString(char *str, UI16_T *error_index);

#endif

BOOL_T CLI_LIB_Portlist_To_Userport(char *bitmap, char *list,UI32_T port_member_status);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_ReadLine_From_XferBuf
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to get a command line from buffer
 *
 * INPUT    : xbuf_p
 * OUTPUT   : cmd_buf
 * RETURN   : TRUE/FALSE
 * NOTE     : if buffer have invalid char, function will return FALSE
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_LIB_ReadLine_From_XferBuf(char *cmd_buf, char **xbuf_p);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_IsProvision
 * ------------------------------------------------------------------------
 * PURPOSE  : Check the CLI session whether is in provision
 * INPUT    : ctrl_P    - A pointer of CLI working area
 * OUTPUT   : None
 * RETURN   : TRUE, in provision; FALSE, not in provision
 * NOTE     : The input parameters should be invalidated by caller
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_LIB_IsProvision(
    CLI_TASK_WorkingArea_T *ctrl_P
    );

/*-----------------------local utility-----------------------------------*/
/* PURPOSE: Get pointer of each part in MAC address
 * INPUT: s:  CLI input mac string(xx-xx-xx-xx-xx-xx or xxxxxxxxxxxx)
 * OUTPUT: Vals: fixed six bye mac address
 * RETUEN: TRUE:Convert success
 *         FALSE:Conver fail
 * NOTE:
 */
BOOL_T CLI_LIB_ValsInMac(char *s, UI8_T *Vals);

/*-----------------------local utility-----------------------------------*/
/* PURPOSE: Check the input char is lession16
 * INPUT: c:  input chat to check
 * OUTPUT: none
 * RETUEN: TRUE:is lession 16
 *         FALSE:not lession 16
 * NOTE:
 */
BOOL_T CLI_LIB_IsLess16(char c);

/* FUNCTION NAME:   CLI_LIB_ConvertSecretKeyByLBase64
 * PURPOSE: convert secret key argument according to ctrl_p's sess_type
 * INPUT:   ctrl_p          -- CLI working area
 *          src_key_p       -- CLI secret argument
 *          dst_buf_size    -- how many bytes of dst_key_p
 * OUTPUT:  dst_key_p       -- output buffer after conversion
 * RETURN:  TRUE -- succeeded / FALSE -- failed
 * NOTES:   None
 */
BOOL_T CLI_LIB_ConvertSecretKeyByLBase64(
            CLI_TASK_WorkingArea_T *ctrl_p,
            char *src_key_p,
            char *dst_key_p,
            UI32_T dst_buf_size);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_Find_Tab_String
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to find the minimum matched arg
 *
 * INPUT    : buff => Command line buffer
              word_buf => string to match
              arg_idx => argument index
              word_len => length of word_buf
              str_idx => The start string index of final matched word
 * OUTPUT   : min_len
 * RETURN   : TRUE/FALSE
 * NOTE     : if buffer have invalid char, function will return FALSE
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_LIB_Find_Tab_Arg(char *buff, char *word_buf, UI16_T str_idx, UI16_T arg_idx, UI16_T word_len, UI16_T *min_len);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - CLI_LIB_SplitString
 * -------------------------------------------------------------------------
 * FUNCTION: Split string to array
 * INPUT   : str            - Input string. After call this function the
 *                            context of the string will be modified.
 *           argc           - String array size.
 * OUTPUT  : argv           - String array.
 *           argc           - How many string be saved in array.
 * RETURN  : TRUE/FALSE
 * NOTE    : If string array is too small, some part of input string will lost.
 * -------------------------------------------------------------------------
 */
BOOL_T
CLI_LIB_SplitString(char * str,
                    char *argv[],
                    UI32_T *argc);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - CLI_LIB_JoinStringArray
 * -------------------------------------------------------------------------
 * FUNCTION: Join string array
 * INPUT   : argv           - String array.
 *           argc           - String array size.
 *           separator      - Separator.
 *           buf_size       - Buffer size of result parameter. Pass result
 *                            parameter is NULL to calculate how many size
 *                            is need for result.
 * OUTPUT  : result         - Result string.
 *           buf_size       - How many size is need for result.
 * RETURN  : TRUE/FALSE
 * NOTE    : None.
 * -------------------------------------------------------------------------
 */
BOOL_T
CLI_LIB_JoinStringArray(char * argv[],
                        UI32_T argc,
                        const char * separator,
                        char * result,
                        UI32_T *buf_size_p);

int
CLI_LIB_DumpCommandTree(const char * mode_string);

/* PURPOSE: Get command table in each mode for privilege
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  Command table
 * NOTE:
 */
CMDLIST *CLI_LIB_Get_Cmd_List_By_Privilege(UI8_T  privilegeMode);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - CLI_LIB_GetCommandMode
 * -------------------------------------------------------------------------
 * FUNCTION: Get access mode
 * INPUT   : fnd_str
 * OUTPUT  : access_mode    - access mode
 * RETURN  : TRUE/FALSE
 * NOTE    : None
 * -------------------------------------------------------------------------
 */
BOOL_T
CLI_LIB_GetCommandMode(const char *fnd_str,
                       enum CLI_ACCESS_MODE_E *access_mode_p);


/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_GetNextCommandMode
 * ------------------------------------------------------------------------
 * PURPOSE  : Enumerates all command mode and string of mode.
 * INPUT    : key_p     - Index value. Pass it as 0 to start enumberate.
 *            buff_size - The buffer size of mode_str.
 * OUTPUT   : mode_p    - Command mode.
 *            mode_str  - String of command mode.
 * RETURN   : TRUE if output a command mode; otherwise return FALSE
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
BOOL_T
CLI_LIB_GetNextCommandMode(UI32_T *key_p,
                           enum CLI_ACCESS_MODE_E *mode_p,
                           char *mode_str,
                           UI32_T buff_size);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - CLI_LIB_GetCommandModeString
 * -------------------------------------------------------------------------
 * FUNCTION: Get command mode string.
 * INPUT   : mode           - Command mode.
 *           buff_size_p    - The buffer size of mode_str. Pass 0 to test
 *                            how many size need for the string of mode.
 * OUTPUT  : mode_size      - String of mode.
 * RETURN  : TRUE/FALSE
 * NOTE    : None
 * -------------------------------------------------------------------------
 */
BOOL_T
CLI_LIB_GetCommandModeString(enum CLI_ACCESS_MODE_E mode,
                             char *mode_str,
                             UI32_T *buff_size_p);

BOOL_T
CLI_LIB_GetCommandModeStringPtr(enum CLI_ACCESS_MODE_E mode,
                            	char ** mode_str_p,
                             	char ** help_msg_p);

#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)

/* PURPOSE: set privilege of all commands following commandIdx, including children and neighbors
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  None
 * NOTE:
 */
void
CLI_LIB_Set_Commands_Privilege(UI16_T  commandIdx,
                               UI8_T  privilegeLevl,
                               CMDLIST *ui_cmd_lst);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - CLI_LIB_SetCommandPrivilege
 * -------------------------------------------------------------------------
 * FUNCTION: Set command privilege
 * INPUT   : mode           - which mode
 *           privilege      - New privilege for matched command
 *           pattern        - match pattern. Like 'show ip' or 'sh ip'
 *           incl_subcmd    - also modify for sub-command
 * OUTPUT  : None
 * RETURN  : TRUE if succeeded, or FALSE if happen an error or no command
 *           have matched by input pattern string.
 * NOTE    : None
 * -------------------------------------------------------------------------
 */
BOOL_T
CLI_LIB_SetCommandPrivilege(const char *mode,
                            UI32_T privilege,
                            const char *pattern,
                            BOOL_T incl_subcmd);

#endif /* #if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE) */


UI32_T CLI_LIB_Get_PortList(char* port_list_p, BOOL_T* port_index_p);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_MaxLineNumPerScreen
 * ------------------------------------------------------------------------
 * PURPOSE  : Get max line number of screen
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : line number
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
UI32_T CLI_LIB_MaxLineNumPerScreen();

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_SetFilteringTerminalDefault
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to set filtering and terminal fields
 *            as default values
 * INPUT    : CLI_TASK_WorkingArea_T *ctrl_p
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
void CLI_LIB_SetFilteringTerminalDefault(CLI_TASK_WorkingArea_T *ctrl_p);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_GetTerminalMaxLineNum
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to get terminal max line number
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : line number
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
UI32_T CLI_LIB_GetMaxTerminalLineNum();

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_HostStringToInaddr
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to convert host string to inet address
 * INPUT    : hoststr_p
 *            family - AF_INET(V4), AF_INET6(V6), AF_UNSPEC(V4/V6)
 * OUTPUT   : inaddr_p
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_LIB_HostStringToInaddr(UI8_T *hoststr_p, UI32_T family, L_INET_AddrIp_T *inaddr_p);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_GetUnitPortFromString
 * ------------------------------------------------------------------------
 * PURPOSE  : Get unit and port number from a PVC_DATA string
 * INPUT    : str     - a PVC_DATA string
 * OUTPUT   : unit_p  - unit
 *            port_p  - port
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_LIB_GetUnitPortFromString(const char *str, UI32_T *unit_p, UI32_T *port_p);

#if (SYS_CPNT_SW_WATCHDOG_TIMER == TRUE)
/* FUNCTION NAME - CLI_IO_SW_WatchDogRoutine
 * PURPOSE  : This function is used for the software watch dog routine.
 * INPUT    : ctrl_p
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 */
void CLI_LIB_SwWatchDogRoutine(CLI_TASK_WorkingArea_T *ctrl_p);
#endif

#endif /* #ifndef CLI_LIB_H */
