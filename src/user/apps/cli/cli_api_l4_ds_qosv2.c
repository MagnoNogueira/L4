#include <ctype.h>

#include "sys_cpnt.h"
#include "l4_pmgr.h"
#include "cli_api.h"
#include <rule_type.h>
#include "cos_vm.h"

static UI32_T CLI_API_Show_One_Class_Map    (UI32_T cmap_id, RULE_TYPE_UI_ClassMap_T *cmap_p, UI32_T line_num);
static UI32_T CLI_API_Show_One_Policy_Map   (RULE_TYPE_UI_PolicyElement_T *pmap_element, char *buff, UI32_T buff_size, UI32_T line_num);

#define ERR_MSG_CLASS_MAP_NONEXISTED        "Class map name %s does not exist.\r\n"
#define ERR_MSG_POLICY_MAP_NONEXISTED       "Policy map name %s does not exist.\r\n"
#define ERR_MSG_NOT_ALLOW_MIX_SELECTOR      "Failed to set match criteria (not allowed for mixing rules in a class map).\r\n"
#define ERR_MSG_FREE_SELECTOR_UNAVAILABLE   "Failed to allocate the selector to install rules.\r\n"

#ifndef ASSERT
#define ASSERT(eq)
#endif /* ASSERT */

#define CLI_DS_PRIV_FLUSH_LINE(string, line_num)                \
    {   line_num = CLI_DS_Priv_FlushLine(string, line_num);     \
        if (EXIT_SESSION_MORE == line_num ||                    \
            JUMP_OUT_MORE == line_num)                          \
        {                                                       \
            return line_num;                                    \
        }                                                       \
    }

typedef enum
{
    CLI_DS_STRING_OK,
    CLI_DS_STRING_E_INSUFFICIENT_BUFFER,
} CLI_DS_STRING_RETURN_T;

typedef struct
{
    char *buff;
    char *cur;

    UI32_T capacity;
    UI32_T used;
} CLI_DS_STRING_T;

void
CLI_DS_STRING_InitString(
    char *buff,
    UI32_T buff_size,
    CLI_DS_STRING_T *string)
{
    string->buff = buff;
    string->capacity = buff_size;

    string->used = strlen(buff);

    ASSERT(string->used < string->capacity);

    string->cur = &string->buff[ string->used ];
}

void
CLI_DS_STRING_Empty(
    CLI_DS_STRING_T *string)
{
    string->used = 0;
    string->cur = &string->buff[ string->used ];

    *string->cur = '\0';
}

CLI_DS_STRING_RETURN_T
CLI_DS_STRING_StrCat(
    CLI_DS_STRING_T *string,
    const char *str)
{
    UI32_T len = strlen(str);

    if ((1 <= len) && (1 == (string->capacity - string->used)))
    {
        return CLI_DS_STRING_E_INSUFFICIENT_BUFFER;
    }

    if ((string->capacity - string->used -1) < len)
    {
        return CLI_DS_STRING_E_INSUFFICIENT_BUFFER;
    }

    strncat(string->cur, str, string->capacity - string->used - 1);

    string->used += len;
    string->cur = &string->buff[ string->used ];

    return CLI_DS_STRING_OK;
}

CLI_DS_STRING_RETURN_T
CLI_DS_STRING_StrCatAtHead(
    CLI_DS_STRING_T *string,
    const char *str)
{
    UI32_T len = strlen(str);

    if ((1 <= len) && (1 == (string->capacity - string->used)))
    {
        return CLI_DS_STRING_E_INSUFFICIENT_BUFFER;
    }

    if ((string->capacity - string->used -1) < len)
    {
        return CLI_DS_STRING_E_INSUFFICIENT_BUFFER;
    }

    memmove(string->buff+len, string->buff, string->capacity - len);
    memcpy(string->buff, str, len);

    string->used += len;
    string->cur = &string->buff[ string->used ];

    return CLI_DS_STRING_OK;
}

CLI_DS_STRING_RETURN_T
CLI_DS_STRING_StrShift(
    CLI_DS_STRING_T *string,
    UI32_T cch)
{
    if (string->used < cch)
    {
        cch = string->used;
    }

    memmove(string->buff, string->buff+cch, string->capacity - cch);

    string->used -= cch;
    string->cur = &string->buff[ string->used ];

    return CLI_DS_STRING_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - CLI_DS_STRNCPY
 *------------------------------------------------------------------------------
 * PURPOSE:  Copy strings
 * INPUT:    src    - src string
 *           cch    - count of characters of dst string buffer(include NULL
 *                    terminate)
 * OUTPUT:   dst    - dst string
 * RETURN:   RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER - dst buffer is too small to
 *           save all characters of src string. dst have a NULL terminate at
 *           last position.
 *           RULE_TYPE_OK - success.
 * NOTE:     Example
 *           char chararray[6];
 *           UI32 res = STRNCPY(chararray, "123456", sizeof(chararray));
 *
 *           assert(res == RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER);
 *           assert(chararray[5] == '\0');
 *------------------------------------------------------------------------------
 */
CLI_DS_STRING_RETURN_T
CLI_DS_STRING_StrNCpy(
     char *dst,
     const char *src,
     UI32_T cch)
{
    dst[cch-1] = '\0';
    strncpy(dst, src, cch);

    if (dst[cch-1] != '\0')
    {
        dst[cch-1] = '\0';
        return CLI_DS_STRING_E_INSUFFICIENT_BUFFER;
    }

    return CLI_DS_STRING_OK;
}

static UI32_T
CLI_DS_Priv_FlushLine(
    CLI_DS_STRING_T *string,
    UI32_T line_num)
{
    enum
    {
        EOL = sizeof("\r\n")-1
    };

    CLI_TASK_WorkingArea_T *ctrl_P;
    UI32_T width;

    char  buff[CLI_DEF_MAX_BUFSIZE] = {0};

    ctrl_P = CLI_TASK_GetMyWorkingArea();

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
    width = ctrl_P->CMenu.width;
#else
    width = CLI_DEF_DISP_WIDTH;
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

    while (width+EOL < string->used)
    {
        memcpy(buff, string->buff, width);
        buff[width]   = '\r';
        buff[width+1] = '\n';
        buff[width+2] = '\0';

        CLI_DS_STRING_StrShift(string, width);
        CLI_DS_STRING_StrCatAtHead(string, "  ");

        PROCESS_MORE_FUNC(buff);
    }

    return line_num;
}

static void CLI_API_Init_Action(RULE_TYPE_UI_Action_T   *action)
{
    action->in_action_type = IN_ACTION_INVALID;
    action->in_action_value = 0;
    action->out_action_type = OUT_ACTION_INVALID;
    action->out_action_value = 0;
    action->confirm_action_type = CONFIRM_ACTION_INVALID;
    action->confirm_action_value = 0;
    action->exceed_action_type = EXCEED_ACTION_INVALID;
    action->exceed_action_value = 0;
    action->violate_action_type = VIOLATE_ACTION_INVALID;
    action->violate_action_value = 0;
    action->class_action_type = CLASS_ACTION_INVALID;
    action->class_action_value = 0;
}

UI32_T CLI_API_L4_DS_Class_Map(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    char *name;

    name = arg[0];

    switch (cmd_idx)
    {
        case PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP:
        {
            RULE_TYPE_UI_ClassMap_T ui_class_map;
            RULE_TYPE_MatchType_T   type = RULE_TYPE_CLASS_MAP_MATCH_ANY;

            /* match-all
             */
            if (arg[1] != NULL && (arg[1][7] == 'l' || arg[1][7] == 'L'))
            {
                type = RULE_TYPE_CLASS_MAP_MATCH_ALL;
            }

            memset(&ui_class_map, 0, sizeof(ui_class_map));

            strncpy(ui_class_map.class_map_name, name, sizeof(ui_class_map.class_map_name)-1);
            ui_class_map.class_map_name[ sizeof(ui_class_map.class_map_name)-1 ] = '\0';

            if (L4_PMGR_QoS_GetClassMapByName(&ui_class_map) != RULE_TYPE_OK)
            {
                switch (L4_PMGR_QoS_CreateClassMapByName(name))
                {
                    case RULE_TYPE_FAIL:
                        CLI_LIB_PrintStr("Failed to create new class map.\r\n");
                        return CLI_NO_ERROR;

                    case RULE_TYPE_E_INVALID_CHARACTER:
                        CLI_LIB_PrintStr("The name of class map should not include "
                                         RULE_TYPE_INVALIDATE_CHAR_OF_DS_NAME
                                         ".\r\n");
                        return CLI_NO_ERROR;

                    default:
                        break;
                }

                strncpy(ui_class_map.class_map_name, name, sizeof(ui_class_map.class_map_name)-1);
                ui_class_map.class_map_name[ sizeof(ui_class_map.class_map_name)-1 ] = '\0';

                if (L4_PMGR_QoS_GetClassMapByName (&ui_class_map) != RULE_TYPE_OK) /* should not occur */
                {
                    CLI_LIB_PrintStr("Failed to get existing class map.\r\n");
                    break;
                }

                ui_class_map.class_map_match_type = type;

                if (L4_PMGR_QoS_SetClassMap ( &ui_class_map ) != RULE_TYPE_OK)
                {
                    CLI_LIB_PrintStr_1("Failed to set class map %s.\r\n", ui_class_map.class_map_name);
                    break;
                }
            }
            else
            {
                if (arg[1] != NULL && type != ui_class_map.class_map_match_type)
                {
                    CLI_LIB_PrintStr_1("Ignored the match type \"%s\".\r\n", arg[1]);
                }
            }

            /* change to class-map configuration mode */
            strcpy(ctrl_P->CMenu.cmap_name, name);
            ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_CLASS_MAP_MODE;

            break;
        }

        case PRIVILEGE_CFG_GLOBAL_CMD_W2_NO_CLASSMAP:
            switch ( L4_PMGR_QoS_DelClassMap(name) )
            {
                case RULE_TYPE_OK:
                    break;

                case RULE_TYPE_CLASS_MAP_NONEXISTED:
                    CLI_LIB_PrintStr_1(ERR_MSG_CLASS_MAP_NONEXISTED, name);
                    break;

                default: /* RULE_TYPE_FAIL */
                    CLI_LIB_PrintStr_1("Failed to delete class map %s.\r\n", name);
                    break;
            }
            break;

        default:
            return CLI_ERR_INTERNAL;
    }
#endif

    return CLI_NO_ERROR;
}

UI32_T CLI_API_L4_DS_Class_Map_Match(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    UI32_T  lower_val = 0;
    UI32_T  upper_val = 0;
    UI32_T  err_idx;
    UI32_T  value;

    char    *op_ptr;
    char    Token[CLI_DEF_MAX_BUFSIZE] = {0};
    char    delemiters[2] = {0};

    RULE_TYPE_UI_ClassMapElement_T  rec_p;

    memset ( &rec_p, 0, sizeof(RULE_TYPE_UI_ClassMapElement_T) );

    rec_p.class_type = RULE_TYPE_CLASS_MF;

    if((arg[0][0] == 'a') || (arg[0][0] == 'A'))
    {
        /* access-list */
        strcpy(rec_p.element.acl_name , arg[1]);
        rec_p.class_type = RULE_TYPE_CLASS_ACL;


        switch(cmd_idx)
        {
            case PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH:
                switch ( L4_PMGR_QoS_AddElement2ClassMap(ctrl_P->CMenu.cmap_name, &rec_p) )
                {
                    case RULE_TYPE_OK:
                        break;

                    case RULE_TYPE_NOT_ALLOW_MIX_SELECTOR:
                        CLI_LIB_PrintStr(ERR_MSG_NOT_ALLOW_MIX_SELECTOR);
                        break;

                    case RULE_TYPE_FREE_SELECTOR_UNAVAILABLE:
                        CLI_LIB_PrintStr(ERR_MSG_FREE_SELECTOR_UNAVAILABLE);
                        break;

                    default:
                        CLI_LIB_PrintStr("Failed to set match criteria.\r\n");
                        break;
                }
                break;

            case PRIVILEGE_CFG_CLASS_MAP_CMD_W2_NO_MATCH:
                if ( L4_PMGR_QoS_DelElementFromClassMap ( ctrl_P->CMenu.cmap_name, &rec_p ) != RULE_TYPE_OK )
                {
                    CLI_LIB_PrintStr("Failed to remove match criteria.\r\n");
                }
                break;

            default:
                return CLI_ERR_INTERNAL;
        }
    }
    else if( (arg[0][0] == 'i') || (arg[0][0] == 'I') )
    {
        if( (arg[1][0] == 'd') || (arg[1][0] == 'D') )
        {
            /* IP dscp */
            op_ptr = arg[2];

            do
            {
                memset(Token, 0, CLI_DEF_MAX_BUFSIZE);

                op_ptr = CLI_LIB_Get_Token(op_ptr, Token, delemiters);

                if (!CLI_LIB_Get_Lower_Upper_Value (Token, &lower_val, &upper_val, &err_idx))
                    break;

                for(value=lower_val; value<=upper_val; value++)
                {
                    if ( (value < MIN_diffServIpAceDscp) || (value > RULE_TYPE_MAX_OF_IP_DSCP) )
                    {
                        CLI_LIB_PrintStr_1("DSCP %lu is invalid.\r\n", value);
                        continue;
                    }

                    rec_p.element.mf_entry.mf_type = RULE_TYPE_MF_DSCP;

                    /* ipv6 dscp */
                    if ( ( arg[0][2] != '\0' ) )
                    {
                        rec_p.element.mf_entry.mf_type = RULE_TYPE_MF_DSCP_IPV6;
                    }

                    rec_p.element.mf_entry.mf_value= (UI8_T) value;

                    switch(cmd_idx)
                    {
                    case PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH:

                        switch ( L4_PMGR_QoS_AddElement2ClassMap ( ctrl_P->CMenu.cmap_name, &rec_p ) )
                        {
                            case RULE_TYPE_OK:
                                break;
                            case RULE_TYPE_NOT_ALLOW_MIX_SELECTOR:
                                CLI_LIB_PrintStr(ERR_MSG_NOT_ALLOW_MIX_SELECTOR);
                                break;
                            default:
                            CLI_LIB_PrintStr("Failed to set match criteria.\r\n");
                                break;
                        }
                        break;

                    case PRIVILEGE_CFG_CLASS_MAP_CMD_W2_NO_MATCH:
                        if ( L4_PMGR_QoS_DelElementFromClassMap ( ctrl_P->CMenu.cmap_name, &rec_p ) != RULE_TYPE_OK )
                        {
                            CLI_LIB_PrintStr("Failed to remove match criteria.\r\n");
                        }
                        break;

                    default:
                        return CLI_ERR_INTERNAL;
                    }
                }

            }while(op_ptr != 0 && !isspace(*op_ptr));
        }
        else if((arg[1][0] == 'p') || (arg[1][0] == 'P'))
        {
            /* IP precedence */
            op_ptr = arg[2];

            do
            {
                memset ( Token, 0, CLI_DEF_MAX_BUFSIZE );

                op_ptr = CLI_LIB_Get_Token ( op_ptr, Token, delemiters );

                if( !CLI_LIB_Get_Lower_Upper_Value ( Token, &lower_val, &upper_val, &err_idx ) )
                    break;

                for (value=lower_val; value<=upper_val; value++)
                {
                    if ( (value < MIN_diffServIpAcePrec) || (value > RULE_TYPE_MAX_OF_IP_PRECEDENCE) )
                    {
                        CLI_LIB_PrintStr_1("Precedence %ld is invalid.\r\n",value);
                        continue;
                    }

                    rec_p.element.mf_entry.mf_type = RULE_TYPE_MF_PRECEDENCE;
                    rec_p.element.mf_entry.mf_value= (UI8_T) value;


                    switch ( cmd_idx )
                    {
                    case PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH:

                        switch ( L4_PMGR_QoS_AddElement2ClassMap ( ctrl_P->CMenu.cmap_name, &rec_p ) )
                        {
                            case RULE_TYPE_OK:
                                break;
                            case RULE_TYPE_NOT_ALLOW_MIX_SELECTOR:
                                CLI_LIB_PrintStr(ERR_MSG_NOT_ALLOW_MIX_SELECTOR);
                                break;
                            default:
                            CLI_LIB_PrintStr("Failed to set match criteria.\r\n");
                                break;
                        }
                        break;

                    case PRIVILEGE_CFG_CLASS_MAP_CMD_W2_NO_MATCH:
                        if ( L4_PMGR_QoS_DelElementFromClassMap ( ctrl_P->CMenu.cmap_name, &rec_p ) != TRUE )
                        {
                            CLI_LIB_PrintStr("Failed to remove match criteria.\r\n");
                        }
                        break;

                    default:
                        return CLI_ERR_INTERNAL;
                    }
                }

            }while ( (op_ptr != 0) && (!isspace(*op_ptr)) );
        }

    }
    else if( (arg[0][0] == 'v') || (arg[0][0] == 'V') )
    {
        /* vlan */
        op_ptr = arg[1];

        do
        {
            memset ( Token, 0, CLI_DEF_MAX_BUFSIZE );

            op_ptr = CLI_LIB_Get_Token ( op_ptr, Token, delemiters );

            if (!CLI_LIB_Get_Lower_Upper_Value ( Token, &lower_val, &upper_val, &err_idx ) )
                break;

            for(value=lower_val; value<=upper_val; value++)
            {
                rec_p.element.mf_entry.mf_type = RULE_TYPE_MF_VLAN;
                rec_p.element.mf_entry.mf_value= (UI16_T) value;

                switch(cmd_idx)
                {
                case PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH:
                    switch ( L4_PMGR_QoS_AddElement2ClassMap ( ctrl_P->CMenu.cmap_name, &rec_p ) )
                    {
                        case RULE_TYPE_OK:
                            break;
                        case RULE_TYPE_NOT_ALLOW_MIX_SELECTOR:
                            CLI_LIB_PrintStr(ERR_MSG_NOT_ALLOW_MIX_SELECTOR);
                            break;
                        default:
                        CLI_LIB_PrintStr("Failed to set match criteria.\r\n");
                            break;
                    }
                    break;

                case PRIVILEGE_CFG_CLASS_MAP_CMD_W2_NO_MATCH:
                    if ( L4_PMGR_QoS_DelElementFromClassMap ( ctrl_P->CMenu.cmap_name, &rec_p ) != TRUE )
                    {
                        CLI_LIB_PrintStr("Failed to remove match criteria.\r\n");
                    }
                    break;

                default:
                    return CLI_ERR_INTERNAL;
                }
            }

        }while ( (op_ptr != 0) && (!isspace(*op_ptr)) );
    }
    else if( (arg[0][0] == 'c') || (arg[0][0] == 'C') )
    {
        /* cos */
        op_ptr = arg[1];

        do
        {
            memset ( Token, 0, CLI_DEF_MAX_BUFSIZE );

            op_ptr = CLI_LIB_Get_Token ( op_ptr, Token, delemiters );

            if (!CLI_LIB_Get_Lower_Upper_Value ( Token, &lower_val, &upper_val, &err_idx ) )
                break;

            for(value=lower_val; value<=upper_val; value++)
            {
                rec_p.element.mf_entry.mf_type = RULE_TYPE_MF_COS;
                rec_p.element.mf_entry.mf_value= value;

                switch(cmd_idx)
                {
                case PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH:
                    switch ( L4_PMGR_QoS_AddElement2ClassMap ( ctrl_P->CMenu.cmap_name, &rec_p ) )
                    {
                        case RULE_TYPE_OK:
                            break;
                        case RULE_TYPE_NOT_ALLOW_MIX_SELECTOR:
                            CLI_LIB_PrintStr(ERR_MSG_NOT_ALLOW_MIX_SELECTOR);
                            break;
                        default:
                        CLI_LIB_PrintStr("Failed to set match criteria.\r\n");
                            break;
                    }
                    break;

                case PRIVILEGE_CFG_CLASS_MAP_CMD_W2_NO_MATCH:
                    if ( L4_PMGR_QoS_DelElementFromClassMap ( ctrl_P->CMenu.cmap_name, &rec_p ) != TRUE )
                    {
                        CLI_LIB_PrintStr("Failed to remove match criteria.\r\n");
                    }
                    break;

                default:
                    return CLI_ERR_INTERNAL;
                }
            }

        }while ( (op_ptr != 0) && (!isspace(*op_ptr)) );
    }
    else if( (arg[0][0] == 's') || (arg[0][0] == 'S') )
    {
        /* source-port */

        UI32_T unit = atoi((char*)arg[2]);
        UI32_T port = atoi(strchr((char*)arg[2],'/')+1);
        BOOL_T is_inherit = TRUE;

        SWCTRL_POM_UIUserPortToIfindex(unit, port, &value, &is_inherit);

        rec_p.element.mf_entry.mf_type = RULE_TYPE_MF_SOURCE_PORT;
        rec_p.element.mf_entry.mf_value= value;

        switch(cmd_idx)
        {
        case PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH:
            switch ( L4_PMGR_QoS_AddElement2ClassMap ( ctrl_P->CMenu.cmap_name, &rec_p ) )
            {
                case RULE_TYPE_OK:
                    break;
                case RULE_TYPE_NOT_ALLOW_MIX_SELECTOR:
                    CLI_LIB_PrintStr(ERR_MSG_NOT_ALLOW_MIX_SELECTOR);
                    break;
                default:
                CLI_LIB_PrintStr("Failed to set match criteria.\r\n");
                    break;
            }
            break;

        case PRIVILEGE_CFG_CLASS_MAP_CMD_W2_NO_MATCH:
            if ( L4_PMGR_QoS_DelElementFromClassMap ( ctrl_P->CMenu.cmap_name, &rec_p ) != TRUE )
            {
                CLI_LIB_PrintStr("Failed to remove match criteria.\r\n");
            }
            break;

        default:
            return CLI_ERR_INTERNAL;
        }
    } /* source-port */

#endif
   return CLI_NO_ERROR;
}

UI32_T CLI_API_L4_DS_Class_Map_Rename(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    UI32_T err_ret =0;

    if (strcmp(ctrl_P->CMenu.cmap_name, arg[0]) != 0)
    {
        err_ret = L4_PMGR_QoS_RenameClassMap(ctrl_P->CMenu.cmap_name, arg[0]);
        switch (err_ret)
        {
            case RULE_TYPE_OK:
                strncpy(ctrl_P->CMenu.cmap_name, arg[0], sizeof(ctrl_P->CMenu.cmap_name) - 1);
                ctrl_P->CMenu.cmap_name[sizeof(ctrl_P->CMenu.cmap_name) - 1] = '\0';
                break;

            case RULE_TYPE_CLASS_MAP_EXISTED:
                CLI_LIB_PrintStr("A class-map with this name already exists.\r\n");
                break;

            case RULE_TYPE_E_INVALID_CHARACTER:
                CLI_LIB_PrintStr("The name of class map should not include "
                                 RULE_TYPE_INVALIDATE_CHAR_OF_DS_NAME
                                 ".\r\n");
                break;

            default:
                CLI_LIB_PrintStr_1("Failed to rename class map name %s.\r\n", arg[0]);
                break;
        }
    }
#endif

   return CLI_NO_ERROR;
}

UI32_T CLI_API_L4_DS_Class_Map_Desc(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    RULE_TYPE_UI_ClassMap_T rec_p;

    strncpy (rec_p.class_map_name, ctrl_P->CMenu.cmap_name, sizeof(rec_p.class_map_name)-1 );
    rec_p.class_map_name[ sizeof(rec_p.class_map_name)-1 ] = '\0';

    if ( L4_PMGR_QoS_GetClassMapByName ( &rec_p ) != RULE_TYPE_OK ) /* should not occur */
    {
        CLI_LIB_PrintStr("Failed to get existing class map.\r\n");
        return CLI_NO_ERROR;
    }


    switch ( cmd_idx )
    {
        case PRIVILEGE_CFG_CLASS_MAP_CMD_W1_DESCRIPTION:
            if ( strcmp(rec_p.desc, arg[0]) == 0 )
            {
                break;
            }

            strncpy (rec_p.desc, arg[0], sizeof(rec_p.desc)-1 );
            rec_p.desc[ sizeof(rec_p.desc)-1 ] = '\0';

            switch ( L4_PMGR_QoS_SetClassMap ( &rec_p ) )
            {
                case RULE_TYPE_OK:
                    break;

                case RULE_TYPE_E_INVALID_CHARACTER:
                    CLI_LIB_PrintStr("The description of class map should not include "
                                     RULE_TYPE_INVALIDATE_CHAR_OF_DS_DESC
                                     ".\r\n");
                    break;

                default:
                    CLI_LIB_PrintStr_1("Failed to set class map description %s.\r\n", rec_p.class_map_name);
                    break;
            }
            break;

        case PRIVILEGE_CFG_CLASS_MAP_CMD_W2_NO_DESCRIPTION:
            rec_p.desc[0] = '\0';
            if ( L4_PMGR_QoS_SetClassMap ( &rec_p ) != RULE_TYPE_OK )
            {
                CLI_LIB_PrintStr_1("Failed to delete class map description %s.\r\n", rec_p.class_map_name);
            }
            break;
        default:
            return CLI_ERR_INTERNAL;
    }

#endif

   return CLI_NO_ERROR;
}

UI32_T CLI_API_L4_DS_Policy_Map(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    char    *pmap_name;

    RULE_TYPE_UI_PolicyMap_T rec_p;

    memset ( &rec_p, 0, sizeof(RULE_TYPE_UI_PolicyMap_T) );

    pmap_name = arg [0];

    switch ( cmd_idx )
    {
    case PRIVILEGE_CFG_GLOBAL_CMD_W1_POLICYMAP:
        switch ( L4_PMGR_QoS_CreatePolicyMapByName(pmap_name ) )
        {
        case RULE_TYPE_FAIL:
            CLI_LIB_PrintStr("Failed to create new policy map.\r\n");
            return CLI_NO_ERROR;

        case RULE_TYPE_E_INVALID_CHARACTER:
            CLI_LIB_PrintStr("The name of policy map should not include "
                             RULE_TYPE_INVALIDATE_CHAR_OF_DS_NAME
                             ".\r\n");
            return CLI_NO_ERROR;

        case RULE_TYPE_POLICY_MAP_EXISTED:
            break;

        case RULE_TYPE_OK:
            break;

        default:
            break;
        }


        /* change to policy-map configuration mode */
        strcpy(ctrl_P->CMenu.pmap_name, pmap_name);
        ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_POLICY_MAP_MODE;
        break;

    case PRIVILEGE_CFG_GLOBAL_CMD_W2_NO_POLICYMAP:

        switch ( L4_PMGR_QoS_DelPolicyMapByName(pmap_name) )
        {
        case RULE_TYPE_OK:
            break;

        case RULE_TYPE_POLICY_MAP_NONEXISTED:
            {
                CLI_LIB_PrintStr_1(ERR_MSG_POLICY_MAP_NONEXISTED, pmap_name);
            }
            break;

        default: /* RULE_TYPE_FAIL */
            {
                CLI_LIB_PrintStr_1("Failed to delete policy map %s.\r\n", pmap_name);
            }
            break;
        }
        break;

    default:
        return CLI_ERR_INTERNAL;
    }
#endif
    return CLI_NO_ERROR;
}

UI32_T CLI_API_L4_DS_Policy_Map_Class(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    char   *cmap_name;

    cmap_name = arg [0];

    switch ( cmd_idx )
    {
    case PRIVILEGE_CFG_POLICY_MAP_CMD_W1_CLASS:

        switch ( L4_PMGR_QoS_AddElement2PolicyMap(ctrl_P->CMenu.pmap_name, cmap_name, NULL, NULL ) )
        {
        case RULE_TYPE_OK:
            break;

        case RULE_TYPE_CLASS_MAP_NONEXISTED:
            CLI_LIB_PrintStr_1(ERR_MSG_CLASS_MAP_NONEXISTED, cmap_name);
            return CLI_NO_ERROR;

        case RULE_TYPE_FREE_SELECTOR_UNAVAILABLE:
            CLI_LIB_PrintStr(ERR_MSG_FREE_SELECTOR_UNAVAILABLE);
            break;

        default:
            CLI_LIB_PrintStr_1("Failed to add class map %s.\r\n", cmap_name);
            return CLI_NO_ERROR;
        }

        /* change to policy-map class configuration mode */
        strcpy(ctrl_P->CMenu.cmap_name, cmap_name );
        ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE;
        break;

    case PRIVILEGE_CFG_POLICY_MAP_CMD_W2_NO_CLASS:
        switch ( L4_PMGR_QoS_DelElementFromPolicyMap(ctrl_P->CMenu.pmap_name, cmap_name) )
        {
        case RULE_TYPE_OK:
            break;

        case RULE_TYPE_CLASS_MAP_NONEXISTED:
            CLI_LIB_PrintStr_1(ERR_MSG_CLASS_MAP_NONEXISTED, cmap_name);

            break;

        default:
            CLI_LIB_PrintStr_1("Failed to delete class map %s.\r\n", cmap_name);
            break;
        }
        break;

    default:
        return CLI_ERR_INTERNAL;
    }
#endif
   return CLI_NO_ERROR;
}

UI32_T CLI_API_L4_DS_Policy_Map_Class_Police(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    RULE_TYPE_UI_Action_T       action;
    RULE_TYPE_TBParamEntry_T    meter;

    memset ( &meter, 0, sizeof(RULE_TYPE_TBParamEntry_T) );

    /*zhangwei  add ///parse arg parameter*/
    char * meter_mode = arg[0];

    meter.row_status = VAL_diffServMeterStatus_active;

    CLI_API_Init_Action(&action);

    if(!strcmp(meter_mode, "flow"))
    {
        meter.meter_model = RULE_TYPE_METER_MODE_FLOW;
        meter.rate = atoi(arg[1]);
        meter.burst_size = atoi(arg[2]);

        /* For orig SHLinux platform, e.g., ES3510MA:
         * flow <CIR> <CBS> conform-action {transmit | <dscp>}
         *                  violate-action {drop | <dscp>}
         *
         * For new product:
         * flow <CIR> <CBS> violate-action {drop | <dscp>}
         */
        if((arg[3][0] == 'c') ||(arg[3][0] == 'C'))
        {
            if((arg[4][0] == 'T') ||(arg[4][0] == 't'))
            {
                action.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
            }
            else
            {
                action.confirm_action_type = CONFIRM_ACTION_REMARK_DSCP_TRANSMIT;
                action.confirm_action_value = atoi(arg[4]);
            }

            if((arg[5][0] == 'v') ||(arg[5][0] == 'V'))
            {
                if((arg[6][0] == 'd') ||(arg[6][0] == 'D'))
                {
                    action.violate_action_type = VIOLATE_ACTION_DROP;
                }
                else
                {
                    action.violate_action_type = VIOLATE_ACTION_REMARK_DSCP_TRANSMIT;
                    action.violate_action_value = atoi(arg[6]);
                }
            }
        }

        if((arg[3][0] == 'v') || (arg[3][0] == 'V'))
        {
            if((arg[4][0] == 'd') ||(arg[4][0] == 'D'))
            {
                action.violate_action_type = VIOLATE_ACTION_DROP;
            }
            else
            {
                action.violate_action_type = VIOLATE_ACTION_REMARK_DSCP_TRANSMIT;
                action.violate_action_value = atoi(arg[4]);
            }
        }
    }
    else if(!strcmp(meter_mode, "rate"))
    {
        /* rate <Kbps>
         */
        meter.meter_model = RULE_TYPE_METER_MODE_RATE;
        meter.rate = atoi(arg[1]);
    }
    else if(!strcmp(meter_mode, "srtcm-color-blind"))
    {
        meter.meter_model = RULE_TYPE_METER_MODE_SRTCM_COLOR_BLIND;
        meter.rate = atoi(arg[1]);
        meter.burst_size = atoi(arg[2]);
        meter.peak_burst_size= atoi(arg[3]);

        /* For orig SHLinux platform, e.g., ES3510MA:
         * srtcm-color-blind <CIR> <CBS> <EBS> conform-action {transmit | <dscp>}
         *                                     exceed-action  {drop | <dscp>}
         *                                     violate-action {drop | <dscp>}
         *
         * For new product:
         * srtcm-color-blind <CIR> <CBS> <EBS> exceed-action  {drop | <dscp>}
         *                                     violate-action {drop | <dscp>}
         */
        if((arg[4][0] == 'c' )||( arg[4][0] == 'C'))
        {
            if((arg[5][0] == 't' ) || (arg[5][0] == 'T' ))
            {
                action.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
            }
            else
            {
                action.confirm_action_type = CONFIRM_ACTION_REMARK_DSCP_TRANSMIT ;
                action.confirm_action_value = atoi(arg[5]);
            }

            if((arg[6][0] == 'e') ||(arg[6][0] == 'E'))
            {
                if((arg[7][0] == 'd') ||(arg[7][0] == 'D'))
                {
                    action.exceed_action_type = EXCEED_ACTION_DROP;
                }
                else
                {
                    action.exceed_action_type = EXCEED_ACTION_REMARK_DSCP_TRANSMIT;
                    action.exceed_action_value = atoi(arg[7]);
                }
            }
            if((arg[8][0] == 'v') ||(arg[8][0] == 'V'))
            {
                if((arg[9][0] == 'd') ||(arg[9][0] == 'D'))
                {
                    action.violate_action_type= VIOLATE_ACTION_DROP;
                }
                else
                {
                    action.violate_action_type = VIOLATE_ACTION_REMARK_DSCP_TRANSMIT;
                    action.violate_action_value = atoi(arg[9]);
                }
            }
        }

        if((arg[4][0] == 'e') ||(arg[4][0] == 'E'))
        {
            if((arg[5][0] == 'd') ||(arg[5][0] == 'D'))
            {
                action.exceed_action_type = EXCEED_ACTION_DROP;
            }
            else
            {
                action.exceed_action_type = EXCEED_ACTION_REMARK_DSCP_TRANSMIT;
                action.exceed_action_value = atoi(arg[5]);
            }
        }
        if((arg[6][0] == 'v') ||(arg[6][0] == 'V'))
        {
            if((arg[7][0] == 'd') ||(arg[7][0] == 'D'))
            {
                action.violate_action_type= VIOLATE_ACTION_DROP;
            }
            else
            {
                action.violate_action_type = VIOLATE_ACTION_REMARK_DSCP_TRANSMIT;
                action.violate_action_value = atoi(arg[7]);
            }
        }
    }
    else if(!strcmp(meter_mode, "srtcm-color-aware"))
    {
        meter.meter_model = RULE_TYPE_METER_MODE_SRTCM_COLOR_AWARE;
        meter.rate = atoi(arg[1]);
        meter.burst_size = atoi(arg[2]);
        meter.peak_burst_size= atoi(arg[3]);

        /* For orig SHLinux platform, e.g., ES3510MA:
         * srtcm-color-aware <CIR> <CBS> <EBS> conform-action {transmit | <dscp>}
         *                                     exceed-action  {drop | <dscp>}
         *                                     violate-action {drop | <dscp>}
         *
         * For new product:
         * srtcm-color-aware <CIR> <CBS> <EBS> exceed-action  {drop | <dscp>}
         *                                     violate-action {drop | <dscp>}
         */
        if((arg[4][0] == 'c' )||( arg[4][0] == 'C'))
        {
            if((arg[5][0] == 't' ) || (arg[5][0] == 'T' ))
            {
                action.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
            }
            else
            {
                action.confirm_action_type = CONFIRM_ACTION_REMARK_DSCP_TRANSMIT ;
                action.confirm_action_value = atoi(arg[5]);
            }

            if((arg[6][0] == 'e') ||(arg[6][0] == 'E'))
            {
                if((arg[7][0] == 'd') ||(arg[7][0] == 'D'))
                {
                    action.exceed_action_type = EXCEED_ACTION_DROP;
                }
                else
                {
                    action.exceed_action_type = EXCEED_ACTION_REMARK_DSCP_TRANSMIT;
                    action.exceed_action_value = atoi(arg[7]);
                }
            }
            if((arg[8][0] == 'v') ||(arg[8][0] == 'V'))
            {
                if((arg[9][0] == 'd') ||(arg[9][0] == 'D'))
                {
                    action.violate_action_type= VIOLATE_ACTION_DROP;
                }
                else
                {
                    action.violate_action_type = VIOLATE_ACTION_REMARK_DSCP_TRANSMIT;
                    action.violate_action_value = atoi(arg[9]);
                }
            }
        }

        if((arg[4][0] == 'e') ||(arg[4][0] == 'E'))
        {
            if((arg[5][0] == 'd') ||(arg[5][0] == 'D'))
            {
                action.exceed_action_type = EXCEED_ACTION_DROP;
            }
            else
            {
                action.exceed_action_type = EXCEED_ACTION_REMARK_DSCP_TRANSMIT;
                action.exceed_action_value = atoi(arg[5]);
            }
        }
        if((arg[6][0] == 'v') ||(arg[6][0] == 'V'))
        {
            if((arg[7][0] == 'd') ||(arg[7][0] == 'D'))
            {
                action.violate_action_type= VIOLATE_ACTION_DROP;
            }
            else
            {
                action.violate_action_type = VIOLATE_ACTION_REMARK_DSCP_TRANSMIT;
                action.violate_action_value = atoi(arg[7]);
            }
        }
    }
    else if(!strcmp(meter_mode, "trtcm-color-blind"))
    {
        meter.meter_model = RULE_TYPE_METER_MODE_TRTCM_COLOR_BLIND;
        meter.rate = atoi(arg[1]);
        meter.burst_size = atoi(arg[2]);
        meter.peak_rate = atoi(arg[3]);
        meter.peak_burst_size= atoi(arg[4]);

        /* For orig SHLinux platform, e.g., ES3510MA:
         * trtcm-color-blind <CIR> <CBS> <PIR> <PBS> conform-action {transmit | <dscp>}
         *                                           exceed-action  {drop | <dscp>}
         *                                           violate-action {drop | <dscp>}
         *
         * For new product:
         * trtcm-color-blind <CIR> <CBS> <PIR> <PBS> exceed-action  {drop | <dscp>}
         *                                           violate-action {drop | <dscp>}
         */
        if((arg[5][0] == 'c' )||( arg[5][0] == 'C'))
        {
            if((arg[6][0] == 't' ) || (arg[6][0] == 'T' ))
            {
                action.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
            }
            else
            {
                action.confirm_action_type = CONFIRM_ACTION_REMARK_DSCP_TRANSMIT ;
                action.confirm_action_value = atoi(arg[6]);
            }
            if((arg[7][0] == 'e') ||(arg[7][0] == 'E'))
            {
                if((arg[8][0] == 'd') ||(arg[8][0] == 'D'))
                {
                    action.exceed_action_type = EXCEED_ACTION_DROP;
                }
                else
                {
                    action.exceed_action_type = EXCEED_ACTION_REMARK_DSCP_TRANSMIT;
                    action.exceed_action_value = atoi(arg[8]);
                }
            }
            if((arg[9][0] == 'v') ||(arg[9][0] == 'V'))
            {
                if((arg[10][0] == 'd') ||(arg[10][0] == 'D'))
                {
                    action.violate_action_type= VIOLATE_ACTION_DROP;
                }
                else
                {
                    action.violate_action_type = VIOLATE_ACTION_REMARK_DSCP_TRANSMIT;
                    action.violate_action_value = atoi(arg[10]);
                }
            }
        }

        if((arg[5][0] == 'e') ||(arg[5][0] == 'E'))
        {
            if((arg[6][0] == 'd') ||(arg[6][0] == 'D'))
            {
                action.exceed_action_type = EXCEED_ACTION_DROP;
            }
            else
            {
                action.exceed_action_type = EXCEED_ACTION_REMARK_DSCP_TRANSMIT;
                action.exceed_action_value = atoi(arg[6]);
            }
        }
        if((arg[7][0] == 'v') ||(arg[7][0] == 'V'))
        {
            if((arg[8][0] == 'd') ||(arg[8][0] == 'D'))
            {
                action.violate_action_type= VIOLATE_ACTION_DROP;
            }
            else
            {
                action.violate_action_type = VIOLATE_ACTION_REMARK_DSCP_TRANSMIT;
                action.violate_action_value = atoi(arg[8]);
            }
        }
    }
    else if(!strcmp(meter_mode, "trtcm-color-aware"))
    {
        meter.meter_model = RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE;
        meter.rate = atoi(arg[1]);
        meter.burst_size = atoi(arg[2]);
        meter.peak_rate = atoi(arg[3]);
        meter.peak_burst_size= atoi(arg[4]);

        /* For orig SHLinux platform, e.g., ES3510MA:
         * trtcm-color-aware <CIR> <CBS> <PIR> <PBS> conform-action {transmit | <dscp>}
         *                                           exceed-action  {drop | <dscp>}
         *                                           violate-action {drop | <dscp>}
         *
         * For new product:
         * trtcm-color-aware <CIR> <CBS> <PIR> <PBS> exceed-action  {drop | <dscp>}
         *                                           violate-action {drop | <dscp>}
         */
        if((arg[5][0] == 'c' )||( arg[5][0] == 'C'))
        {
            if((arg[6][0] == 't' ) || (arg[6][0] == 'T' ))
            {
                action.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
            }
            else
            {
                action.confirm_action_type = CONFIRM_ACTION_REMARK_DSCP_TRANSMIT ;
                action.confirm_action_value = atoi(arg[6]);
            }
            if((arg[7][0] == 'e') ||(arg[7][0] == 'E'))
            {
                if((arg[8][0] == 'd') ||(arg[8][0] == 'D'))
                {
                    action.exceed_action_type = EXCEED_ACTION_DROP;
                }
                else
                {
                    action.exceed_action_type = EXCEED_ACTION_REMARK_DSCP_TRANSMIT;
                    action.exceed_action_value = atoi(arg[8]);
                }
            }
            if((arg[9][0] == 'v') ||(arg[9][0] == 'V'))
            {
                if((arg[10][0] == 'd') ||(arg[10][0] == 'D'))
                {
                    action.violate_action_type= VIOLATE_ACTION_DROP;
                }
                else
                {
                    action.violate_action_type = VIOLATE_ACTION_REMARK_DSCP_TRANSMIT;
                    action.violate_action_value = atoi(arg[10]);
                }
            }
        }

        if((arg[5][0] == 'e') ||(arg[5][0] == 'E'))
        {
            if((arg[6][0] == 'd') ||(arg[6][0] == 'D'))
            {
                action.exceed_action_type = EXCEED_ACTION_DROP;
            }
            else
            {
                action.exceed_action_type = EXCEED_ACTION_REMARK_DSCP_TRANSMIT;
                action.exceed_action_value = atoi(arg[6]);
            }
        }
        if((arg[7][0] == 'v') ||(arg[7][0] == 'V'))
        {
            if((arg[8][0] == 'd') ||(arg[8][0] == 'D'))
            {
                action.violate_action_type= VIOLATE_ACTION_DROP;
            }
            else
            {
                action.violate_action_type = VIOLATE_ACTION_REMARK_DSCP_TRANSMIT;
                action.violate_action_value = atoi(arg[8]);
            }
        }
    }

    switch ( cmd_idx )
    {
    case PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_POLICE:

        switch ( L4_PMGR_QoS_AddElement2PolicyMap ( ctrl_P->CMenu.pmap_name, ctrl_P->CMenu.cmap_name, &meter, &action ) )
        {
        case RULE_TYPE_OK:
            break;

        case RULE_TYPE_METER_FAIL:
            CLI_LIB_PrintStr("Failed to define a meter.\r\n");
            return CLI_NO_ERROR;

        case RULE_TYPE_ACTION_FAIL:
            CLI_LIB_PrintStr("Failed to define an action.\r\n");
            return CLI_NO_ERROR;

        default:
            CLI_LIB_PrintStr("Failed to set a policer.\r\n");
            return CLI_NO_ERROR;
        }
        break;

    case PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W2_NO_POLICE:
        if ( L4_PMGR_QoS_DelElementActionFromPolicyMap ( ctrl_P->CMenu.pmap_name, ctrl_P->CMenu.cmap_name, &action ) != RULE_TYPE_OK )
        {
            CLI_LIB_PrintStr("Failed to remove an action.\r\n");
            return CLI_NO_ERROR;
        }

        if ( L4_PMGR_QoS_DelElementMeterFromPolicyMap ( ctrl_P->CMenu.pmap_name, ctrl_P->CMenu.cmap_name, &meter ) != RULE_TYPE_OK )
        {
            CLI_LIB_PrintStr("Failed to remove a meter.\r\n");
        }
        break;

    default:
        return CLI_ERR_INTERNAL;
    }
#endif

   return CLI_NO_ERROR;
}

UI32_T CLI_API_L4_DS_Policy_Map_Class_Priority(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    RULE_TYPE_UI_Action_T action;

    CLI_API_Init_Action(&action);

    action.class_action_type    = CLASS_ACTION_SET_PRIORITY;

    switch (cmd_idx)
    {
        case PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_PRIORITY:

            action.class_action_value   = atoi(arg[0]);

            if ( L4_PMGR_QoS_AddElement2PolicyMap ( ctrl_P->CMenu.pmap_name, ctrl_P->CMenu.cmap_name, NULL, &action ) != RULE_TYPE_OK )
            {
                CLI_LIB_PrintStr("Failed to add a traffic classification.\r\n");
            }
            break;

        case PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W2_NO_PRIORITY:

            if ( L4_PMGR_QoS_DelElementActionFromPolicyMap ( ctrl_P->CMenu.pmap_name, ctrl_P->CMenu.cmap_name, &action ) != RULE_TYPE_OK )
            {
                CLI_LIB_PrintStr("Failed to remove a traffic classification.\r\n");
            }
            break;

        default:
            return CLI_ERR_INTERNAL;
    }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

    return CLI_NO_ERROR;
}

UI32_T CLI_API_L4_DS_Policy_Map_Class_Set(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    RULE_TYPE_UI_Action_T   action;

    CLI_API_Init_Action(&action);

    if ( (arg[0][0] == 'c') || (arg[0][0] == 'C') )
    {
        /* cos */
        action.in_action_type = IN_ACTION_NEW_COS;
        action.in_action_value= atoi(arg[1]);
    }
    else if(('p' == arg[0][0]) || ('P' == arg[0][0]))
    {
        action.in_action_type = IN_ACTION_NEW_PHB;
        action.in_action_value= atoi(arg[1]);
    }
    else if(('i' == arg[0][0]) || ('I' == arg[0][0])) /* ip */
    {
        if (('d' == arg[1][0]) || ('D' == arg[1][0]))
        {
            /*dscp*/
            action.in_action_type = IN_ACTION_NEW_IP_DSCP;
            action.in_action_value= atoi(arg[2]);
        }
        else if (('p' == arg[1][0]) || ('P' == arg[1][0]))
        {
            /*precedence*/
            action.in_action_type = IN_ACTION_NEW_IP_PRECEDENCE;
            action.in_action_value= atoi(arg[2]);
        }
    }

/*
    2008-08-05, Jinfeng Chen:
    Not supported again.
 */
#if 0
    else if ( (arg[0][0] == 'i') || (arg[0][0] == 'I') )
    {
        if ( arg [0][2] == '\0' )
        {
            /* IP */
            if ( (arg[1][0] == 'd') || (arg[1][0] == 'D') )
            {
            #if 0
                /* dscp */
                action.in_action_type = IN_ACTION_NEW_IP_DSCP;
                action.in_action_value= atoi(arg[2] );
            #endif
            }
            else if ( (arg[1][0] == 'p') || (arg[1][0] == 'P') )
            {
                /* precedence */
                action.in_action_type = IN_ACTION_NEW_IP_PRECEDENCE;
                action.in_action_value= atoi(arg[2] );
            }
        }
#if (SYS_CPNT_IPV6 == TRUE)
        else
        {   /* IPv6 */
                /* dscp */
                action.in_action_type = IN_ACTION_NEW_IPV6_DSCP;
                action.in_action_value= atoi(arg[2] );
        }
#endif /* #if (SYS_CPNT_IPV6 == TRUE) */
    }
#endif

    switch ( cmd_idx )
    {
    case PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_SET:
        if ( L4_PMGR_QoS_AddElement2PolicyMap ( ctrl_P->CMenu.pmap_name, ctrl_P->CMenu.cmap_name, NULL, &action ) != RULE_TYPE_OK )
        {
            CLI_LIB_PrintStr("Failed to add a traffic classification.\r\n");
        }
        break;

    case PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W2_NO_SET:
        if ( L4_PMGR_QoS_DelElementActionFromPolicyMap ( ctrl_P->CMenu.pmap_name, ctrl_P->CMenu.cmap_name, &action ) != RULE_TYPE_OK )
        {
            CLI_LIB_PrintStr("Failed to remove a traffic classification.\r\n");
        }
        break;

    default:
        return CLI_ERR_INTERNAL;
    }
#endif

   return CLI_NO_ERROR;
}

UI32_T CLI_API_L4_DS_Policy_Map_Bundle(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    UI32_T i;
    UI32_T cnt;
    UI32_T ret;
    RULE_TYPE_UI_BundleGroup_T  bundle_group;

    memset(&bundle_group, 0, sizeof(bundle_group));

    if (arg[0])
    {
        bundle_group.index = atoi(arg[0]);
    }
    else
    {
        return CLI_ERR_INTERNAL;
    }

    switch ( cmd_idx )
    {
        case PRIVILEGE_CFG_POLICY_MAP_CMD_W1_BUNDLE:

            if (arg[1])
            {
                bundle_group.bundle_rate_kbps = atoi(arg[1]);
            }
            else
            {
                return CLI_ERR_INTERNAL;
            }

            i = 2;
            while (arg[i])
            {
                cnt = bundle_group.class_map_count;
                strncpy((char*)bundle_group.class_map_name_list[cnt],
                        arg[i],
                        SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
                bundle_group.class_map_name_list[cnt][SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0';
                ++bundle_group.class_map_count;

                ++i;
            }

            bundle_group.valid_status = TRUE;

            ret = L4_PMGR_QoS_SetBundleGroup(ctrl_P->CMenu.pmap_name,
                                             &bundle_group);
            if (RULE_TYPE_OK != ret)
            {
                CLI_LIB_PrintStr("Failed to set bundle group.\r\n");
            }

            break;

        case PRIVILEGE_CFG_POLICY_MAP_CMD_W2_NO_BUNDLE:

            bundle_group.valid_status = FALSE;

            ret = L4_PMGR_QoS_SetBundleGroup(ctrl_P->CMenu.pmap_name,
                                             &bundle_group);
            if (RULE_TYPE_OK != ret)
            {
                CLI_LIB_PrintStr("Failed to set bundle group.\r\n");
            }

            break;

        default:
            return CLI_ERR_INTERNAL;
    }

#endif /* #if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE) */
    return CLI_NO_ERROR;
}

UI32_T CLI_API_L4_DS_Policy_Map_Rename(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    switch ( L4_PMGR_QoS_RenamePolicyMap(ctrl_P->CMenu.pmap_name, arg [0] ) )
    {
        case RULE_TYPE_OK:
            strncpy(ctrl_P->CMenu.pmap_name, arg[0], sizeof(ctrl_P->CMenu.pmap_name) - 1);
            ctrl_P->CMenu.pmap_name[sizeof(ctrl_P->CMenu.pmap_name) - 1] = '\0';
            break;

        case RULE_TYPE_POLICY_MAP_EXISTED:
            CLI_LIB_PrintStr("A policy map with this name already exists.\r\n");
            break;

        case RULE_TYPE_E_INVALID_CHARACTER:
            CLI_LIB_PrintStr("The name of policy map should not include "
                             RULE_TYPE_INVALIDATE_CHAR_OF_DS_NAME
                             ".\r\n");
            break;

        case RULE_TYPE_FAIL:
        default:
            CLI_LIB_PrintStr_1("Failed to set policy map name %s.\r\n", arg[0]);
            break;
    }
#endif

   return CLI_NO_ERROR;
}

UI32_T CLI_API_L4_DS_Policy_Map_Desc(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)

    switch ( cmd_idx )
    {
    case PRIVILEGE_CFG_POLICY_MAP_CMD_W1_DESCRIPTION:
        switch ( L4_PMGR_QoS_SetPolicyMapDescByName ( ctrl_P->CMenu.pmap_name, arg[0] ) )
        {
            case RULE_TYPE_OK:
                break;

            case RULE_TYPE_E_INVALID_CHARACTER:
                CLI_LIB_PrintStr("The description of policy map should not include "
                                 RULE_TYPE_INVALIDATE_CHAR_OF_DS_DESC
                                 ".\r\n");
                break;

            default:
                CLI_LIB_PrintStr_1("Failed to set policy map description %s.\r\n", arg [0]);
                break;
        }
        break;

    case PRIVILEGE_CFG_POLICY_MAP_CMD_W2_NO_DESCRIPTION:

        if ( L4_PMGR_QoS_SetPolicyMapDescByName(ctrl_P->CMenu.pmap_name, "\0" ) != RULE_TYPE_OK )
        {
            CLI_LIB_PrintStr_1("Failed to delete policy map description %s.\r\n", arg [0]);
        }
        break;

    default:
        return CLI_ERR_INTERNAL;
    }
#endif

   return CLI_NO_ERROR;
}

UI32_T CLI_API_L4_DS_Service_Policy_Eth(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    char *pmap_name;

    UI32_T ifindex = 0;
    UI32_T verify_unit = ctrl_P->CMenu.unit_id;
    UI32_T verify_port, i;
    CLI_API_EthStatus_T         verify_ret;

    RULE_TYPE_InOutDirection_T  direction = RULE_TYPE_INBOUND;

    char   dir_str [12] = "ingress";

    if ( (arg[0][0] == 'i') || (arg[0][0] == 'I') )
    {
        /* input */
        direction = RULE_TYPE_INBOUND;
    }
    else if ( (arg[0][0] == 'o') || (arg[0][0] == 'O') )
    {
        /* output */
        direction = RULE_TYPE_OUTBOUND;
        strcpy(dir_str, "egress");
    }

    pmap_name = arg [1];

    switch ( cmd_idx )
    {
    case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W1_SERVICEPOLICY:

        for (i = 1; i <= ctrl_P->sys_info.max_port_number; i++)
        {
            if (ctrl_P->CMenu.port_id_list[(UI32_T)((i-1)/8)]   & (1 << ( 7 - ((i-1)%8))) )
            {
                verify_port = i;

                verify_ret = verify_ethernet ( verify_unit, verify_port, &ifindex );

                /*do not check if this port is trunk member or not   */
                /*and semantic check must be completed in LACP itself*/
                if ( verify_ret == CLI_API_ETH_NOT_PRESENT || verify_ret == CLI_API_ETH_UNKNOWN_PORT)
                {
                    display_ethernet_msg (verify_ret, verify_unit, verify_port);
                    continue;
                }
                else
                {
                    switch ( L4_PMGR_QoS_BindPort2PolicyMap(ifindex, pmap_name, direction ) )
                    {
                    case RULE_TYPE_OK:
                        break;

                    case RULE_TYPE_FREE_SELECTOR_UNAVAILABLE:
                        CLI_LIB_PrintStr(ERR_MSG_FREE_SELECTOR_UNAVAILABLE);
                        break;

                    default:
                        CLI_LIB_PrintStr_N("Binding error on eth %lu/%lu.\r\n",
                                           verify_unit, verify_port);
                        break;
                    }

                }
            }
        }
        break;

    case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_NO_SERVICEPOLICY:

        for (i = 1; i <= ctrl_P->sys_info.max_port_number; i++)
        {
            if (ctrl_P->CMenu.port_id_list[(UI32_T)((i-1)/8)]   & (1 << ( 7 - ((i-1)%8))) )
            {
                verify_port = i;

                verify_ret = verify_ethernet (verify_unit, verify_port, &ifindex);

                /*do not check if this port is trunk member or not   */
                /*and semantic check must be completed in LACP itself*/
                if ( verify_ret == CLI_API_ETH_NOT_PRESENT || verify_ret == CLI_API_ETH_UNKNOWN_PORT )
                {
                    display_ethernet_msg (verify_ret, verify_unit, verify_port);
                    continue;
                }
                else if ( L4_PMGR_QoS_UnBindPortFromPolicyMap(ifindex, pmap_name, direction) != RULE_TYPE_OK )
                {
                     CLI_LIB_PrintStr_N("Unbinding error on eth %lu/%lu.\r\n",
                                        verify_unit, verify_port);
                }
            }
        }
        break;

    default:
        return CLI_ERR_INTERNAL;
    }
#endif

   return CLI_NO_ERROR;
}

/** service-policy {input | output} <policy-map-name>
 */
UI32_T CLI_API_L4_DS_Service_Policy_Pch(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS_V2_TRUNK_PORT == TRUE)
    char *pmap_name;

    UI32_T ifindex = 0;
    UI32_T verify_trunk_id = ctrl_P->CMenu.pchannel_id;
    CLI_API_EthStatus_T verify_ret;

    RULE_TYPE_InOutDirection_T  direction = RULE_TYPE_INBOUND;

    char   dir_str [12] = "ingress";

    if( (verify_ret = verify_trunk(verify_trunk_id, &ifindex)) != CLI_API_TRUNK_OK)
    {
        display_trunk_msg(verify_ret, verify_trunk_id);
        return CLI_NO_ERROR;
    }

    if ( (arg[0][0] == 'i') || (arg[0][0] == 'I') )
    {
        /* input */
        direction = RULE_TYPE_INBOUND;
    }
    else if ( (arg[0][0] == 'o') || (arg[0][0] == 'O') )
    {
        /* output */
        direction = RULE_TYPE_OUTBOUND;
        strcpy(dir_str, "egress");
    }

    pmap_name = arg [1];

    switch ( cmd_idx )
    {
    case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W1_SERVICEPOLICY:

        switch ( L4_PMGR_QoS_BindPort2PolicyMap(ifindex, pmap_name, direction ) )
        {
        case RULE_TYPE_OK:
            break;

        case RULE_TYPE_FREE_SELECTOR_UNAVAILABLE:
            CLI_LIB_PrintStr(ERR_MSG_FREE_SELECTOR_UNAVAILABLE);
            break;

        default:
            CLI_LIB_PrintStr("Bind Error\r\n");
            break;
        }

        break;

    case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W2_NO_SERVICEPOLICY:

        if ( L4_PMGR_QoS_UnBindPortFromPolicyMap(ifindex, pmap_name, direction) != RULE_TYPE_OK )
        {
            CLI_LIB_PrintStr("Unbind Error\r\n");
        }

        break;

    default:
        return CLI_ERR_INTERNAL;
    }
#endif /* SYS_CPNT_QOS_V2_TRUNK_PORT */

   return CLI_NO_ERROR;
}

/** service-policy {input} <policy-map-name>
 */
UI32_T CLI_API_L4_DS_Service_Policy_Control_Plane(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;

    char *pmap_name;

    UI32_T ifindex;

    RULE_TYPE_InOutDirection_T  direction = RULE_TYPE_INBOUND;

    if (arg[0] == NULL || arg[1] == NULL)
    {
        return CLI_ERR_INTERNAL;
    }

    /** The control plane means (phy) CPU port at egress direction
     */
    if ( (arg[0][0] == 'i') || (arg[0][0] == 'I') )
    {
        /* input */
        direction = RULE_TYPE_OUTBOUND;
    }
    else
    {
        return CLI_ERR_INTERNAL;
    }

    pmap_name = arg [1];

    ifindex = SYS_ADPT_CPU_1_IF_INDEX_NUMBER;

    switch (cmd_idx)
    {
        case PRIVILEGE_CFG_CONTROL_PLANE_CMD_LIST_CMD_W1_SERVICEPOLICY:

            result = L4_PMGR_QoS_BindPort2PolicyMap(ifindex, pmap_name, direction);
            switch (result)
            {
            case RULE_TYPE_OK:
                break;

            case RULE_TYPE_FREE_SELECTOR_UNAVAILABLE:
                CLI_LIB_PrintStr(ERR_MSG_FREE_SELECTOR_UNAVAILABLE);
                break;

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
            case RULE_TYPE_NOT_SUPPORT_BUNDLE_RATE_CONTORL :
                CLI_LIB_PrintStr("The bundle setting will not work on control-plane\r\n");
                break;
#endif

            default:
                CLI_LIB_PrintStr("Bind Error\r\n");
                break;
            }

            break;

        case PRIVILEGE_CFG_CONTROL_PLANE_CMD_LIST_CMD_W2_NO_SERVICEPOLICY:

            result = L4_PMGR_QoS_UnBindPortFromPolicyMap(ifindex, pmap_name, direction);
            if (RULE_TYPE_OK != result)
            {
                CLI_LIB_PrintStr("Unbind Error\r\n");
            }

            break;

    default:
        return CLI_ERR_INTERNAL;
    }
#endif /* SYS_CPNT_QOS_V2_CPU_PORT */

   return CLI_NO_ERROR;
}

UI32_T CLI_API_Clear_Policy_Map_Hw_Counter(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)
    UI32_T  ifindex;
    UI32_T  arg_pos=0;
    RULE_TYPE_InOutDirection_T direction;
    char    pmap_name [SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH +1];
    char    cmap_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH +1] = {0};

    switch(cmd_idx)
    {
        case PRIVILEGE_EXEC_CMD_W3_CLEAR_POLICYMAP_CONTROLPLANE:
            ifindex = SYS_ADPT_CPU_1_IF_INDEX_NUMBER;
            if ( (arg[arg_pos][0] == 'i') || (arg[arg_pos][0] == 'I') )
            {
                direction = RULE_TYPE_OUTBOUND;
            }
            else
            {
                return CLI_ERR_INTERNAL;
            }
            break;

        default:
            return CLI_ERR_INTERNAL;
    }

    /*Get name of policy-map */		
    if ( L4_PMGR_QoS_GetPolicyMapNameByPort(ifindex, direction, pmap_name) != RULE_TYPE_OK )
    {
        return CLI_NO_ERROR;
    }
		
    arg_pos++;
    /*[class class-map-name] hardware counters*/
    if (arg[arg_pos] != NULL)
    {
        UI32_T pmap_id, cmap_id;
        RULE_TYPE_UI_PolicyElement_T    pmap_element;

        if (RULE_TYPE_OK != L4_PMGR_QoS_GetPolicyMapIdByName(pmap_name, &pmap_id))
        {
            CLI_LIB_PrintStr("Fail to get policy-map.\r\n");
            return CLI_NO_ERROR;
        }

        memset (&pmap_element, 0, sizeof(RULE_TYPE_UI_PolicyElement_T));
        while (L4_PMGR_QoS_GetNextElementFromPolicyMap (pmap_id, &pmap_element) == RULE_TYPE_OK && 
			                                              pmap_element.row_status == VAL_diffServPolicyMapElementStatus_active)
        {
            if (arg[arg_pos][0] == 'c' || arg[arg_pos][0] == 'C') /*class*/
            {
                if ( strcmp(arg[arg_pos+1], pmap_element.class_map_name) != 0 )
                {
                    continue;
                }
            }

            if (RULE_TYPE_OK !=L4_PMGR_QoS_GetClassMapIdByName(pmap_element.class_map_name, &cmap_id))
            {
                CLI_LIB_PrintStr("Fail to get class-map.\r\n");
                return CLI_NO_ERROR;
            }

            if (RULE_TYPE_OK !=L4_PMGR_QoS_ClearDiffServPolicyMapHwCounter(ifindex, direction, pmap_id, cmap_id))
            {
                CLI_LIB_PrintStr("Fail to clear counter.\r\n");
                return CLI_NO_ERROR;
            }
        }
    }
    else
    {
        return CLI_ERR_INTERNAL;
    }
#endif /*#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)*/

    return CLI_NO_ERROR;
}

UI32_T CLI_API_Show_Class_Map(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    UI32_T line_num = 0;
    UI32_T cmap_id = 0;
    UI32_T found  = 0;

    RULE_TYPE_UI_ClassMap_T cmap;


    while ( L4_PMGR_QoS_GetNextClassMap ( &cmap_id, &cmap ) == RULE_TYPE_OK && cmap.row_status == VAL_diffServClassMapStatus_active)
    {
        if ( arg[0] != NULL )
        {
            if ( strcmp(arg[0], cmap.class_map_name) == 0 )
            {
                line_num = CLI_API_Show_One_Class_Map ( cmap_id, &cmap, line_num );
                found = 1;
            }
        }
        else
        {
            line_num = CLI_API_Show_One_Class_Map ( cmap_id, &cmap, line_num );


            if ( line_num == JUMP_OUT_MORE )
                break;
        }
    }

    if ( (arg[0] != NULL) && (found == 0) )
    {
        CLI_LIB_PrintStr_1(ERR_MSG_CLASS_MAP_NONEXISTED, arg [0]);
    }

#endif

    return CLI_NO_ERROR;
}

static UI32_T CLI_API_Show_One_Class_Map (UI32_T cmap_id, RULE_TYPE_UI_ClassMap_T *cmap_p, UI32_T line_num)
{
    UI32_T  cmap_eid =0;
    char    buff[CLI_DEF_MAX_BUFSIZE] = {0};

    RULE_TYPE_UI_ClassMapElement_T rec;

    memset ( &rec, 0, sizeof(RULE_TYPE_UI_ClassMapElement_T) );

    sprintf(buff, "Class Map %s %s\r\n",
            ( cmap_p->class_map_match_type == RULE_TYPE_CLASS_MAP_MATCH_ANY ) ? ("match-any"):("match-all"),
              cmap_p->class_map_name );
    PROCESS_MORE_FUNC ( buff );
    sprintf(buff, "Description: %s\r\n", cmap_p->desc);
    PROCESS_MORE_FUNC(buff);

    /* get element */
    while ( L4_PMGR_QoS_GetNextElementFromClassMap ( cmap_id, &cmap_eid, &rec ) == RULE_TYPE_OK )
    {
        if ( rec.class_type == RULE_TYPE_CLASS_ACL )
        {
            /* acl */
            sprintf(buff, " Match access-list %s\r\n", rec.element.acl_name );
            PROCESS_MORE_FUNC ( buff );
        }
        else if( rec.class_type == RULE_TYPE_CLASS_MF )
        {
            PROCESS_MORE_FUNC ( buff );

            switch ( rec.element.mf_entry.mf_type )
            {
            case RULE_TYPE_MF_PRECEDENCE:
                /* ip precedence */
                sprintf(buff, " Match IP Precedence %lu\r\n", rec.element.mf_entry.mf_value );
                PROCESS_MORE_FUNC ( buff );
                break;

            case RULE_TYPE_MF_DSCP:
                /* ip dscp */
                sprintf(buff, " Match IP DSCP %lu\r\n", rec.element.mf_entry.mf_value );
                PROCESS_MORE_FUNC ( buff );
                break;

            case RULE_TYPE_MF_VLAN:
                /* vlan */
                sprintf(buff, " Match VLAN %lu\r\n", rec.element.mf_entry.mf_value );
                PROCESS_MORE_FUNC ( buff );
                break;

#if (SYS_CPNT_IPV6 == TRUE)
            case RULE_TYPE_MF_DSCP_IPV6:
                sprintf(buff," Match IPv6 DSCP %lu\r\n", rec.element.mf_entry.mf_value );
                PROCESS_MORE_FUNC ( buff );
                break;
#endif /* #if (SYS_CPNT_IPV6 == TRUE) */

            case RULE_TYPE_MF_COS:
                sprintf(buff, " Match CoS %lu\r\n", rec.element.mf_entry.mf_value );
                PROCESS_MORE_FUNC ( buff );
                break;

            case RULE_TYPE_MF_SOURCE_PORT:
                {
                    UI32_T unit, port, trunk_id;

                    SWCTRL_POM_LogicalPortToUserPort(rec.element.mf_entry.mf_value,
                                                     &unit, &port, &trunk_id);

                    sprintf(buff, " Match source-port ethernet %lu/%lu\r\n", unit, port );
                    PROCESS_MORE_FUNC ( buff );
                }
                break;
            }
        }
    }

    sprintf(buff, "\r\n" );
    PROCESS_MORE_FUNC ( buff );

    return line_num;
}

UI32_T CLI_API_Show_Policy_Map(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    UI32_T  line_num = 0;
    UI32_T  pmap_id = 0;
    char    buff[CLI_DEF_MAX_BUFSIZE] = {0};
    UI32_T  found  = 0;

    CLI_DS_STRING_T                 string;
    RULE_TYPE_UI_PolicyMap_T        pmap;
    RULE_TYPE_UI_PolicyElement_T    pmap_element;

    memset ( &pmap, 0, sizeof(RULE_TYPE_UI_PolicyMap_T) );
    while ( L4_PMGR_QoS_GetNextPolicyMap (&pmap_id, &pmap) == RULE_TYPE_OK && pmap.row_status == VAL_diffServPolicyMapStatus_active)
    {
        if( arg[0] != NULL )
        {
            if ( strcmp(arg[0], pmap.name) != 0 )
                continue;
        }

        found = 1;

        SYSFUN_Snprintf(buff, sizeof(buff), "Policy Map %s\r\n", pmap.name);
        buff[sizeof(buff)-1] = '\0';

        CLI_DS_STRING_InitString(buff, sizeof(buff), &string);

        CLI_DS_PRIV_FLUSH_LINE(&string, line_num);
        PROCESS_MORE(buff);

        SYSFUN_Snprintf(buff, sizeof(buff), "Description: %s\r\n", pmap.desc);
        buff[sizeof(buff)-1] = '\0';

        CLI_DS_PRIV_FLUSH_LINE(&string, line_num);
        PROCESS_MORE(buff);

        memset ( &pmap_element, 0, sizeof(RULE_TYPE_UI_PolicyElement_T) );
        while ( L4_PMGR_QoS_GetNextElementFromPolicyMap ( pmap_id, &pmap_element ) == RULE_TYPE_OK && pmap_element.row_status == VAL_diffServPolicyMapElementStatus_active)
        {
            if( arg[2] != NULL )
            {
                if ( strcmp(arg[2], pmap_element.class_map_name) != 0 )
                    continue;
            }

            line_num = CLI_API_Show_One_Policy_Map ( &pmap_element, buff, sizeof(buff), line_num );

            if ( line_num == JUMP_OUT_MORE )
            {
                return CLI_NO_ERROR;
            }
            else if (line_num == EXIT_SESSION_MORE)
            {
                return CLI_EXIT_SESSION;
            }

        }

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
        {
            UI32_T  bundle_group_index;
            UI32_T  cnt;
            char    temp_buff[CLI_DEF_MAX_BUFSIZE];

            for (bundle_group_index = 0; bundle_group_index < SYS_ADPT_DIFFSERV_MAX_BUNDLE_GROUP_OF_POLICY_MAP; ++bundle_group_index)
            {
                if (TRUE != pmap.bundle_group[bundle_group_index].valid_status)
                {
                    continue;
                }

                SYSFUN_Snprintf(buff, sizeof(buff), " bundle %lu %lu",
                                pmap.bundle_group[bundle_group_index].index,
                                pmap.bundle_group[bundle_group_index].bundle_rate_kbps);
                buff[ sizeof(buff)-1 ] = '\0';

                CLI_DS_STRING_InitString(buff, sizeof(buff), &string);

                CLI_DS_PRIV_FLUSH_LINE(&string, line_num);

                for (cnt = 0; cnt < pmap.bundle_group[bundle_group_index].class_map_count; ++cnt)
                {
                    SYSFUN_Snprintf(temp_buff, sizeof(temp_buff), "%s%s",
                                    (0 == cnt) ? " " : ", ",
                                    pmap.bundle_group[bundle_group_index].class_map_name_list[cnt]);
                    temp_buff[ sizeof(temp_buff)-1 ] = '\0';
                    CLI_DS_STRING_StrCat(&string, temp_buff);
                    CLI_DS_PRIV_FLUSH_LINE(&string, line_num);
                }

                CLI_DS_STRING_StrCat(&string, "\r\n");
                PROCESS_MORE(buff);
            }
        }
#endif /* #if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE) */

        strcpy(buff, "\r\n");
        PROCESS_MORE(buff);

        if ( line_num == JUMP_OUT_MORE )
        {
            return CLI_NO_ERROR;
        }
        else if (line_num == EXIT_SESSION_MORE)
        {
            return CLI_EXIT_SESSION;
        }
    }

    if ( (arg[0] != NULL) && (found == 0) )
    {
        CLI_LIB_PrintStr_1(ERR_MSG_POLICY_MAP_NONEXISTED, arg [0]);
    }
#endif

   return CLI_NO_ERROR;
}

static UI32_T CLI_API_Show_One_Policy_Map (RULE_TYPE_UI_PolicyElement_T *pmap_element, char *buff, UI32_T buff_size, UI32_T line_num)
{
    CLI_DS_STRING_T     string;
    char                *meter_model = "";

    SYSFUN_Snprintf(buff, buff_size, " class %s\r\n", pmap_element -> class_map_name );
    buff[buff_size-1] = '\0';

    CLI_DS_STRING_InitString(buff, buff_size, &string);
    CLI_DS_PRIV_FLUSH_LINE(&string, line_num);
    PROCESS_MORE_FUNC(buff);

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)

    switch ( pmap_element->action_entry.class_action_type )
    {
        case CLASS_ACTION_SET_PRIORITY:
            SYSFUN_Snprintf(buff, buff_size, "  priority %lu\r\n", pmap_element->action_entry.class_action_value);
            buff[buff_size-1] = '\0';
            break;

        case CLASS_ACTION_INVALID:
        default:
            buff[0] = '\0';
            break;
    }

    CLI_DS_STRING_InitString(buff, buff_size, &string);
    CLI_DS_PRIV_FLUSH_LINE(&string, line_num);
    PROCESS_MORE_FUNC(buff);

#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

    switch  ( pmap_element ->action_entry.in_action_type )
    {
        case IN_ACTION_NEW_COS:
            SYSFUN_Snprintf(buff, buff_size, "  set CoS %lu\r\n", pmap_element->action_entry.in_action_value );
            buff[buff_size-1] = '\0';
            break;

        case IN_ACTION_NEW_IP_DSCP:
            SYSFUN_Snprintf(buff, buff_size, "  set IP DSCP %lu\r\n", pmap_element->action_entry.in_action_value );
            buff[buff_size-1] = '\0';
            break;

        case IN_ACTION_NEW_IP_PRECEDENCE:
            SYSFUN_Snprintf(buff, buff_size, "  set IP Precedence %lu\r\n", pmap_element->action_entry.in_action_value );
            buff[buff_size-1] = '\0';
            break;

        case IN_ACTION_NEW_PHB:
            SYSFUN_Snprintf(buff, buff_size, "  set PHB %lu\r\n", pmap_element->action_entry.in_action_value );
            buff[buff_size-1] = '\0';
            break;

        default:
            buff[0] = '\0';
            break;
    }

    CLI_DS_STRING_InitString(buff, buff_size, &string);
    CLI_DS_PRIV_FLUSH_LINE(&string, line_num);
    PROCESS_MORE_FUNC(buff);

    /* get meter */
    if (VAL_diffServMeterStatus_active == pmap_element ->meter_entry.row_status)
    {
        if(RULE_TYPE_METER_MODE_FLOW == pmap_element ->meter_entry.meter_model)
        {
            meter_model = "flow";
            SYSFUN_Snprintf(buff, buff_size, "  police %s %lu %lu",
                    meter_model,
                    pmap_element->meter_entry.rate,
                    pmap_element->meter_entry.burst_size);
            buff[buff_size-1] = '\0';
        }
        else if(RULE_TYPE_METER_MODE_TRTCM_COLOR_BLIND == pmap_element ->meter_entry.meter_model)
        {
            meter_model = "trtcm-color-blind";
            SYSFUN_Snprintf(buff, buff_size, "  police %s %lu %lu %lu %lu",
                    meter_model,
                    pmap_element->meter_entry.rate,
                    pmap_element->meter_entry.burst_size,
                    pmap_element->meter_entry.peak_rate,
                    pmap_element->meter_entry.peak_burst_size);
            buff[buff_size-1] = '\0';
        }
        else if(RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE == pmap_element ->meter_entry.meter_model)
        {
            meter_model = "trtcm-color-aware";
            SYSFUN_Snprintf(buff, buff_size, "  police %s %lu %lu %lu %lu",
                    meter_model,
                    pmap_element->meter_entry.rate,
                    pmap_element->meter_entry.burst_size,
                    pmap_element->meter_entry.peak_rate,
                    pmap_element->meter_entry.peak_burst_size );
            buff[buff_size-1] = '\0';
        }
        else if(RULE_TYPE_METER_MODE_SRTCM_COLOR_BLIND == pmap_element ->meter_entry.meter_model)
        {
            meter_model = "srtcm-color-blind";
            SYSFUN_Snprintf(buff, buff_size, "  police %s %lu %lu %lu",
                    meter_model,
                    pmap_element->meter_entry.rate,
                    pmap_element->meter_entry.burst_size,
                    pmap_element->meter_entry.peak_burst_size );
            buff[buff_size-1] = '\0';
        }
        else if(RULE_TYPE_METER_MODE_SRTCM_COLOR_AWARE == pmap_element ->meter_entry.meter_model)
        {
            meter_model = "srtcm-color-aware";
            SYSFUN_Snprintf(buff, buff_size, "  police %s %lu %lu %lu",
                    meter_model,
                    pmap_element->meter_entry.rate,
                    pmap_element->meter_entry.burst_size,
                    pmap_element->meter_entry.peak_burst_size );
            buff[buff_size-1] = '\0';
        }
        else if (RULE_TYPE_METER_MODE_RATE == pmap_element ->meter_entry.meter_model)
        {
            meter_model = "rate";
            SYSFUN_Snprintf(buff, buff_size, "  police %s %lu",
                            meter_model,
                            pmap_element->meter_entry.rate);
            buff[buff_size-1] = '\0';
        }
        else
        {
            buff[0] = '\0';
        }

        CLI_DS_STRING_InitString(buff, buff_size, &string);
        CLI_DS_PRIV_FLUSH_LINE(&string, line_num);

        /* get action */
        switch( pmap_element ->action_entry.confirm_action_type)
        {
            case CONFIRM_ACTION_TRANSMIT:
                CLI_DS_STRING_StrCat(&string, " conform-action transmit");
                break;

            case CONFIRM_ACTION_REMARK_DSCP_TRANSMIT:
                {
                    char dscp[5];

                    SYSFUN_Snprintf(dscp, sizeof(dscp), " %lu", pmap_element ->action_entry.confirm_action_value);
                    dscp[sizeof(dscp)-1] = '\0';

                    CLI_DS_STRING_StrCat(&string, " conform-action");
                    CLI_DS_STRING_StrCat(&string, dscp);
                }
                break;

            default:
                break;
        }

        CLI_DS_PRIV_FLUSH_LINE(&string, line_num);

        switch ( pmap_element ->action_entry.exceed_action_type )
        {
            case EXCEED_ACTION_DROP:
                CLI_DS_STRING_StrCat(&string, " exceed-action drop");
                break;

            case EXCEED_ACTION_REMARK_DSCP_TRANSMIT:
                {
                    char dscp[5];

                    SYSFUN_Snprintf(dscp, sizeof(dscp), " %lu", pmap_element ->action_entry.exceed_action_value);
                    dscp[sizeof(dscp)-1] = '\0';

                    CLI_DS_STRING_StrCat(&string, " exceed-action");
                    CLI_DS_STRING_StrCat(&string, dscp);
                }
                break;

            default:
                break;
        }

        CLI_DS_PRIV_FLUSH_LINE(&string, line_num);

        switch( pmap_element ->action_entry.violate_action_type )
        {
            case VIOLATE_ACTION_DROP:
                CLI_DS_STRING_StrCat(&string, " violate-action drop");
                break;

            case VIOLATE_ACTION_REMARK_DSCP_TRANSMIT:
                {
                    char dscp[5];

                    SYSFUN_Snprintf(dscp, sizeof(dscp), " %lu", pmap_element->action_entry.violate_action_value);
                    dscp[sizeof(dscp)-1] = '\0';

                    CLI_DS_STRING_StrCat(&string, " violate-action");
                    CLI_DS_STRING_StrCat(&string, dscp);
                }
                break;

            default:
                break;
        }

        CLI_DS_STRING_StrCat(&string, "\r\n");
        CLI_DS_PRIV_FLUSH_LINE(&string, line_num);
        PROCESS_MORE_FUNC(buff);
    }

    return line_num;
}

#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)
static UI32_T CLI_API_Pri_Show_Policy_Map_Hw_Counter(    
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    char *pmap_name, 
    char *cmap_name)
{
    UI32_T  line_num = 0;
    UI32_T pmap_id, cmap_id;
    RULE_TYPE_UI_PolicyElement_T    pmap_element;
    RULE_TYPE_CounterRec_T counter;
    char    buff    [CLI_DEF_MAX_BUFSIZE] = {0};
    BOOL_T found = FALSE;

    memset(&counter, 0, sizeof(RULE_TYPE_CounterRec_T));

    sprintf(buff, "Service-policy %s \r\n", pmap_name);
    PROCESS_MORE(buff);

    if (RULE_TYPE_OK != L4_PMGR_QoS_GetPolicyMapIdByName(pmap_name, &pmap_id))
    {
        CLI_LIB_PrintStr("Fail to get policy-map.\r\n");
        return CLI_NO_ERROR;
    }

    memset (&pmap_element, 0, sizeof(RULE_TYPE_UI_PolicyElement_T));
    while (L4_PMGR_QoS_GetNextElementFromPolicyMap (pmap_id, &pmap_element) == RULE_TYPE_OK && 
                                                           pmap_element.row_status == VAL_diffServPolicyMapElementStatus_active)
    {		
        if (cmap_name[0] != '\0') /*class*/
        {			
            if (strcmp(cmap_name, pmap_element.class_map_name) != 0)
            {
                continue;
            }
        }
        found = TRUE;

        if (RULE_TYPE_OK !=L4_PMGR_QoS_GetClassMapIdByName(pmap_element.class_map_name, &cmap_id))
        {
            CLI_LIB_PrintStr("Fail to get class-map.\r\n");
            return CLI_NO_ERROR;
        }

        if (RULE_TYPE_OK == L4_PMGR_QoS_GetDiffServPolicyMapHwCounter(
				ifindex, direction, pmap_id, cmap_id, &counter))
        {
             sprintf(buff, "  Class-map %s \r\n", pmap_element.class_map_name);
             sprintf(buff, "%s    Receive Packets: %10lu \r\n", buff, counter.receive_packets);
             sprintf(buff, "%s       Drop Packets: %10lu \r\n", buff, counter.drop_packets);

             PROCESS_MORE(buff);
        }		
    }

    if ((cmap_name[0] != '\0') && (found == FALSE))
    { 
        CLI_LIB_PrintStr_1(ERR_MSG_CLASS_MAP_NONEXISTED, cmap_name);
    }

    return CLI_NO_ERROR;
}
#endif /*#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)*/

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
static UI32_T
CLI_API_L4_DS_Show_One_Policy_Map_Interface(
    UI32_T *line_num_p,
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    char *pmap_name,
    char *cmap_name)
{
    UI32_T ret = CLI_NO_ERROR;
    UI32_T line_num = *line_num_p;

#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)
    if (pmap_name != NULL && cmap_name != NULL)
    {
        ret = CLI_API_Pri_Show_Policy_Map_Hw_Counter(ifindex, direction, pmap_name, cmap_name);
    }
    else
#endif /* SYS_CPNT_QOS_V2_COUNTER */
    {
        char   buff[CLI_DEF_MAX_BUFSIZE] = {0};
        char   policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH +1] = {0};
        char   interface_str[26] = {0};
        BOOL_T dump_interface = TRUE;
        UI32_T unit, port, trunk_id;
        SWCTRL_Lport_Type_T port_type;
        RULE_TYPE_InOutDirection_T dump_direction;

        ASSERT(pmap_name == NULL);
        ASSERT(cmap_name == NULL);
        
        port_type = SWCTRL_POM_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);
        switch (port_type)
        {
            case SWCTRL_LPORT_NORMAL_PORT:
            case SWCTRL_LPORT_TRUNK_PORT_MEMBER:
                snprintf(interface_str, sizeof(interface_str) - 1, "Interface ethernet %lu/%lu", unit, port);
                break;
            case SWCTRL_LPORT_TRUNK_PORT:
                snprintf(interface_str, sizeof(interface_str) - 1, "Interface port-channel %lu", trunk_id);
                break;
                
            default:
                return CLI_ERR_INTERNAL;
        }
        
        if (direction = RULE_TYPE_BOTH_DIRECTION)
        {
            direction = RULE_TYPE_INBOUND;
            dump_direction = RULE_TYPE_BOTH_DIRECTION;
        }
        else
        {
            dump_direction = direction + 1;
        }
        
        for (; direction < dump_direction; ++direction)
        {
            if (RULE_TYPE_OK == L4_PMGR_QoS_GetPolicyMapNameByPort(ifindex, direction, policy_map_name))
            {
                if (dump_interface == TRUE)
                {
                    dump_interface = FALSE;
                    sprintf(buff, "%s\r\n", interface_str);
                    PROCESS_MORE(buff);
                }

                sprintf(buff, "  service-policy %s %s\r\n",
                        (direction == RULE_TYPE_OUTBOUND) ? "output" : "input", policy_map_name);
                PROCESS_MORE(buff);
            }
        }
    }
    
    *line_num_p = line_num;
    return ret;
}
#endif /* (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV) */

/* show policy-map interface [<unit>/<port> {input|output} [class class-map-name] hardware counters]
 */
UI32_T CLI_API_Show_Policy_Map_Interface(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T  cli_ret = CLI_NO_ERROR;
#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    UI32_T  line_num = 0, arg_pos = 0;
    UI32_T  ifindex  = 0;
    UI32_T  verify_trunk_id, verify_unit, verify_port;
    RULE_TYPE_InOutDirection_T direction;
    CLI_API_EthStatus_T verify_ret;

    if (NULL == arg[arg_pos])
    {
        UI32_T max_port_num;
        
        direction = RULE_TYPE_BOTH_DIRECTION;

        for (verify_unit = 0; STKTPLG_OM_GetNextUnit(&verify_unit); )
        {
            max_port_num = SWCTRL_POM_UIGetUnitPortNumber(verify_unit);
            
            for (verify_port = 1; verify_port <= max_port_num; ++verify_port)
            {
                verify_ret = verify_ethernet(verify_unit, verify_port, &ifindex);
                
                if (CLI_API_ETH_OK != verify_ret)
                {
                    continue;
                }

                CLI_API_L4_DS_Show_One_Policy_Map_Interface(&line_num, ifindex, direction, NULL, NULL);
            }
        }
        
        return cli_ret;
    }
    
    switch (arg[arg_pos][0])
    {
        case 'p':
        case 'P':
            arg_pos++;
            
            if (NULL == arg[arg_pos])
            {
                return CLI_ERR_INTERNAL;
            }
            
            verify_trunk_id = atoi(arg[arg_pos]);
            
            if( (verify_ret = verify_trunk(verify_trunk_id, &ifindex)) != CLI_API_TRUNK_OK)
            {
                display_trunk_msg(verify_ret, verify_trunk_id);
                return CLI_NO_ERROR;
            }
            
            arg_pos++;
            break;
            
        case 'e':
        case 'E':
            arg_pos++;
            
            if (NULL == arg[arg_pos])
            {
                return CLI_ERR_INTERNAL;
            }

            verify_unit = atoi(arg[arg_pos]);
            verify_port = atoi(strchr(arg[arg_pos], '/') + 1);
            if((verify_ret = verify_ethernet(verify_unit, verify_port, &ifindex)) != CLI_API_ETH_OK)
            {
                display_ethernet_msg(verify_ret, verify_unit, verify_port);
                return CLI_NO_ERROR;
            }
            
            arg_pos++;
            break;
            
        default:
            verify_unit = atoi(arg[arg_pos]);
            verify_port = atoi(strchr(arg[arg_pos], '/') + 1);
            if( (verify_ret = verify_ethernet(verify_unit, verify_port, &ifindex)) != CLI_API_ETH_OK)
            {
                display_ethernet_msg(verify_ret, verify_unit, verify_port);
                return CLI_NO_ERROR;
            }
            
            arg_pos++;
            break;
    }

    direction = RULE_TYPE_BOTH_DIRECTION;
    if (NULL != arg[arg_pos])
    {
        switch (arg[arg_pos][0])
        {
            case 'i':
            case 'I':
                direction = RULE_TYPE_INBOUND;
                break;
            case 'o':
            case 'O':
                direction = RULE_TYPE_OUTBOUND;
                break;
                
            default:
                return CLI_ERR_INTERNAL;
        }
    }

#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)
    /* show policy-map interface <Unit>/<Port> {input|output} [class class-map-name] hardware counters
     */
    arg_pos ++;
    if (arg[arg_pos] != NULL)
    {
        char pmap_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH +1] = {0};
        char cmap_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH +1] = {0};
         /* class
          */
        if (arg[arg_pos][0] == 'c' || arg[arg_pos][0] == 'C')
        {
            if (CLI_DS_STRING_OK !=
                CLI_DS_STRING_StrNCpy(cmap_name, arg[arg_pos+1], SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH+1))
            {
                return CLI_ERR_INTERNAL;
            }
         }

        if (RULE_TYPE_OK != L4_PMGR_QoS_GetPolicyMapNameByPort(ifindex, direction, pmap_name))
        {
            return CLI_NO_ERROR;
        }

        cli_ret = CLI_API_L4_DS_Show_One_Policy_Map_Interface(&line_num, ifindex, direction, pmap_name, cmap_name);
    }
    else
#endif /* SYS_CPNT_QOS_V2_COUNTER */
    {
        cli_ret = CLI_API_L4_DS_Show_One_Policy_Map_Interface(&line_num, ifindex, direction, NULL, NULL);
    }

#endif /* (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV) */
   return cli_ret;
}

/** show policy-map control-plan [input]
 */
UI32_T CLI_API_Show_Policy_Map_Control_Plane(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
    UI32_T  line_num = 0;
    UI32_T  ifindex;

    char    buff    [CLI_DEF_MAX_BUFSIZE] = {0};
    char    pmap_name [SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH +1];

    RULE_TYPE_InOutDirection_T direction;
    UI32_T  arg_pos=0;

    if (NULL == arg[arg_pos])
    {
        return CLI_ERR_INTERNAL;
    }

    ifindex = SYS_ADPT_CPU_1_IF_INDEX_NUMBER;

    /** Bind policy map on control-plane input direction means
     * (phy) CPU port at egress direction
     */
    if ( (arg[arg_pos][0] == 'i') || (arg[arg_pos][0] == 'I') )
    {
        direction = RULE_TYPE_OUTBOUND;
    }
    else
    {
        return CLI_ERR_INTERNAL;
    }
   
    if ( L4_PMGR_QoS_GetPolicyMapNameByPort(ifindex, direction, pmap_name) != RULE_TYPE_OK )
    {
        return CLI_NO_ERROR;
    }

#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)
    arg_pos++;
    /*show policy-map control-plane input [class class-map-name] hardware counters*/
    if (arg[arg_pos] != NULL)
    {
        char    cmap_name [SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH +1] = {0};
	
        if (arg[arg_pos][0] == 'c' || arg[arg_pos][0] == 'C') /*class*/
        {
            if (CLI_DS_STRING_OK != CLI_DS_STRING_StrNCpy(cmap_name, arg[arg_pos+1], SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH+1))
            {
                return CLI_ERR_INTERNAL;
            }
        }

        return CLI_API_Pri_Show_Policy_Map_Hw_Counter(ifindex, direction, pmap_name, cmap_name);
    }
    else
#endif
        {
            sprintf(buff, "Service-policy %s \r\n", pmap_name );
            PROCESS_MORE( buff );
        }
#endif /* SYS_CPNT_QOS_V2_CPU_PORT */

   return CLI_NO_ERROR;
}


#if (SYS_CPNT_COS == TRUE)

UI32_T CLI_API_QOS_SetQosIfMappingMode(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_TRUST_MODE_SUPPORT_COS || SYS_CPNT_COS_TRUST_MODE_SUPPORT_DSCP \
    || SYS_CPNT_COS_TRUST_MODE_SUPPORT_PRECEDENCE)
    UI32_T lport;
    UI32_T mapping_mode;
    UI32_T i;

    UI32_T verify_unit = ctrl_P->CMenu.unit_id;
    UI32_T verify_port;
    CLI_API_EthStatus_T verify_ret;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_QOS_MAP_TRUSTMODE:

            if(arg[0]!=NULL)
            {
                if(arg[0][0] == 'c' || arg[0][0] == 'C')
                    mapping_mode = COS_MAPPING_MODE;
                else if(arg[0][0] == 'd' || arg[0][0] == 'D')
                    mapping_mode = DSCP_IF_MAPPING_MODE;
        else
                    mapping_mode = PRECEDENCE_MAPPING_MODE;
            }
            else
                return CLI_ERR_INTERNAL;
            break;

        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W4_NO_QOS_MAP_TRUSTMODE:
            mapping_mode = COS_MAPPING_MODE; /*default*/
            break;

        default:
            return CLI_ERR_INTERNAL;
    }

    for (i = 1 ; i <= ctrl_P->sys_info.max_port_number ; i++)
    {
        if (ctrl_P->CMenu.port_id_list[(UI32_T)((i-1)/8)]  & (1 << ( 7 - ((i-1)%8))) )
        {
            verify_port = i;

            verify_ret = verify_ethernet(verify_unit, verify_port, &lport);
            if (verify_ret != CLI_API_ETH_OK &&
                verify_ret != CLI_API_ETH_TRUNK_MEMBER)
            {
                display_ethernet_msg(verify_ret, verify_unit, verify_port);
                continue;
            }

            if (TRUE != L4_PMGR_QOS_SetPortPriorityTrustMode(lport, COS_TYPE_PRIORITY_USER, mapping_mode))
            {
#if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
#if (SYS_CPNT_EH == TRUE)
                    CLI_API_Show_Exception_Handeler_Msg();
#else
                    CLI_LIB_PrintStr_1("Failed to set QoS trust mode on ethernet %s\r\n", name);
#endif
                }
#else
#if (SYS_CPNT_EH == TRUE)
                CLI_API_Show_Exception_Handeler_Msg();
#else
                CLI_LIB_PrintStr_2("Failed to set QoS trust mode on ethernet %lu/%lu\r\n", verify_unit, verify_port);
#endif
#endif
            }
        }
    }
#endif /* #if (SYS_CPNT_COS_TRUST_MODE_SUPPORT_COS || SYS_CPNT_COS_TRUST_MODE_SUPPORT_DSCP \
            || SYS_CPNT_COS_TRUST_MODE_SUPPORT_PRECEDENCE) */
    return CLI_NO_ERROR;
}

#if (SYS_CPNT_COS_TRUST_MODE_SUPPORT_COS || SYS_CPNT_COS_TRUST_MODE_SUPPORT_DSCP \
    || SYS_CPNT_COS_TRUST_MODE_SUPPORT_PRECEDENCE)
static UI32_T Show_one_interface_cos_map_mode(UI32_T lport, UI32_T line_num)
{
    qos_if_trust_mode_t mode;
    char  *str[] =
    {
        " CoS mode",
        " IP-prec mode",
        " DSCP mode",
    };
    char   buff[CLI_DEF_MAX_BUFSIZE] = {0};
    if (TRUE == L4_PMGR_QOS_GetPortPriorityTrustMode(lport, COS_TYPE_PRIORITY_USER, (UI32_T *)&mode))
    {
         sprintf(buff, "  CoS Map mode:         %s\r\n", str[mode]);
         PROCESS_MORE_FUNC(buff);
    }
    else
    {
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr_1("Failed to get CoS port info, lport = %lu\r\n", lport);
#endif
        line_num += 1;
    }
    return line_num;
}
#endif /* #if (SYS_CPNT_COS_TRUST_MODE_SUPPORT_COS || SYS_CPNT_COS_TRUST_MODE_SUPPORT_DSCP \
            || SYS_CPNT_COS_TRUST_MODE_SUPPORT_PRECEDENCE) */

UI32_T CLI_API_QOS_ShowQosifMappingMode(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_TRUST_MODE_SUPPORT_COS || SYS_CPNT_COS_TRUST_MODE_SUPPORT_DSCP \
    || SYS_CPNT_COS_TRUST_MODE_SUPPORT_PRECEDENCE)
    UI32_T line_num = 0;

    if(arg[1]==NULL)
        return CLI_ERR_INTERNAL;

    switch(arg[0][0])
    {
        case 'e':
        case 'E':
        {
            UI32_T lport = 0;
            UI32_T verify_unit = atoi(arg[1]);
            UI32_T verify_port = atoi( strchr(arg[1], '/')+1 );
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
                if (!IF_PMGR_IfnameToIfindex(arg[1], &lport))
                {
                    CLI_LIB_PrintStr_1("%s does not exist\r\n",arg[1]);
                    return CLI_NO_ERROR;
                }
                SWCTRL_POM_LogicalPortToUserPort(lport, &verify_unit, &verify_port, &trunk_id);
            }
    #endif

            verify_ret = verify_ethernet(verify_unit, verify_port, &lport);

            switch(verify_ret)
            {
                case CLI_API_ETH_OK:
    #if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Information of %s\r\n", name);
                }
    #else
                    CLI_LIB_PrintStr_2("Information of Eth %lu/%lu\r\n", verify_unit, verify_port);
    #endif
                    line_num++;
                    Show_one_interface_cos_map_mode(lport, line_num);
                    break;

                case CLI_API_ETH_TRUNK_MEMBER:
    #if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Information of %s\r\n", name);
                }
    #else
                    CLI_LIB_PrintStr_2("Information of Eth %lu/%lu\r\n", verify_unit, verify_port);
    #endif
                    line_num++;
                    Show_one_interface_cos_map_mode(lport, line_num);
                    break;

                case CLI_API_ETH_NOT_PRESENT:
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    break;

                case CLI_API_ETH_UNKNOWN_PORT:
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    break;
            }
        }
            break;

        case 'p':
        case 'P':
        {
        #if 0
            UI32_T lport = 0;
            UI32_T verify_trunk_id = atoi(arg[1]);
            CLI_API_TrunkStatus_T verify_ret;

            if( (verify_ret = verify_trunk(verify_trunk_id, &lport)) != CLI_API_TRUNK_OK)
            {
                display_trunk_msg(verify_ret, verify_trunk_id);
                return CLI_NO_ERROR;
            }
            else
            {
                sprintf(buff, "Information of Trunk %lu\r\n", verify_trunk_id);
                PROCESS_MORE(buff);

                if((line_num = Show_one_interface_cos_map_mode(lport, line_num)) == JUMP_OUT_MORE)
                {
                    return CLI_NO_ERROR;
                }
                else if (line_num == EXIT_SESSION_MORE)
                {
                    return CLI_EXIT_SESSION;
                }

                break;
            }
        #endif
        }
            return CLI_ERR_CMD_NOT_IMPLEMENT;

        default:
            return CLI_ERR_INTERNAL;
    }
#endif /* #if (SYS_CPNT_COS_TRUST_MODE_SUPPORT_COS || SYS_CPNT_COS_TRUST_MODE_SUPPORT_DSCP \
            || SYS_CPNT_COS_TRUST_MODE_SUPPORT_PRECEDENCE) */
    return CLI_NO_ERROR;
}

#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP == TRUE)

/* [Global config mode]
 * qos map cos-dscp {phb color} from {cos cfi} [cos cfi]*
 * no qos map cos-dscp {cos cfi} [cos cfi]*
 */
UI32_T
CLI_API_QOS_SetIngressCos2Dscp_Global(
    UI16_T cmd_idx,
    char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == FALSE)
    typedef struct
    {
        UI32_T phb;
        UI32_T color;
    } INTERNAL_DSCP_T;

    const INTERNAL_DSCP_T default_cos_to_dscp_mapping[INGRESS_COS_TO_DSCP_MAPPING_ENTRY_NMB][INGRESS_COS_TO_DSCP_MAPPING_CFI] = COS_TYPE_DEFAULT_COS_TO_DSCP_MAP_ARRAY;

    UI32_T phb = 0, color = 0, cos = 0, cfi = 0;
    int arg_index, arg_num;
    BOOL_T is_restore;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_GLOBAL_CMD_W3_QOS_MAP_COSDSCP:
            is_restore = FALSE;
            break;

        case PRIVILEGE_CFG_GLOBAL_CMD_W4_NO_QOS_MAP_COSDSCP:
            is_restore = TRUE;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }

    arg_num = 0;
    while(arg[arg_num])
    {
        arg_num++;
    }

    arg_index = 0;
    if(!is_restore)
    {
        /* {phb color} from {cos cfi}
         */
        if (arg_num < 5)
        {
            return CLI_ERR_INTERNAL;
        }

        phb = atoi(arg[0]);
        if(phb < MIN_PHB_VAL || MAX_PHB_VAL < phb)
        {
            return CLI_ERR_INTERNAL;
        }

        color = atoi(arg[1]);
        if(color < MIN_COLOR_VAL || MAX_COLOR_VAL < color)
        {
            return CLI_ERR_INTERNAL;
        }

        if (color == 2)
        {
            CLI_LIB_PrintStr("drop precedence value can't be set to 2\r\n");
            return CLI_NO_ERROR;
        }

        arg_index = 3;
    }

    for (; (arg_index+1) < arg_num; arg_index+=2)
    {
        cos = atoi(arg[arg_index]);
        if(cos < MIN_COS_VAL || MAX_COS_VAL < cos)
        {
            return CLI_ERR_INTERNAL;
        }

        cfi = atoi(arg[arg_index+1]);
        if(cfi < MIN_CFI_VAL || MAX_CFI_VAL < cfi)
        {
            return CLI_ERR_INTERNAL;
        }

        if (is_restore)
        {
            phb = default_cos_to_dscp_mapping[cos][cfi].phb;
            color = default_cos_to_dscp_mapping[cos][cfi].color;
        }

        if (COS_TYPE_E_NONE !=
            L4_PMGR_QOS_SetCos2InternalDscp(COS_TYPE_PRIORITY_USER, cos, cfi, phb, color))
        {
             CLI_LIB_PrintStr("Failed to set QoS CoS to DSCP\r\n");
        }
    }
#endif /* #if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == FALSE) */

    return CLI_NO_ERROR;
}

/* [Internace ethernet config mode]
 * qos map cos-dscp {phb color} from {cos cfi} [cos cfi]*
 * no qos map cos-dscp {cos cfi} [cos cfi]*
 */
UI32_T CLI_API_QOS_SetIngressCos2Dscp(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE)
    typedef struct
    {
        UI32_T phb;
        UI32_T color;
    } INTERNAL_DSCP_T;

    const INTERNAL_DSCP_T default_cos_to_dscp_mapping[INGRESS_COS_TO_DSCP_MAPPING_ENTRY_NMB][INGRESS_COS_TO_DSCP_MAPPING_CFI] = COS_TYPE_DEFAULT_COS_TO_DSCP_MAP_ARRAY;

    int cos, cfi, i, j;
    int phb = 0, color = 0, arg_num = 0, num = 0;
    UI32_T verify_unit = ctrl_P->CMenu.unit_id;
    UI32_T verify_port,lport;
    CLI_API_EthStatus_T verify_ret;
    BOOL_T is_restore;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_QOS_MAP_COSDSCP:
                is_restore = FALSE;
            break;

        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W4_NO_QOS_MAP_COSDSCP:
                is_restore = TRUE;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }

    j = 0;
    if(!is_restore)
    {
        phb = atoi(arg[0]);
        if(phb < MIN_PHB_VAL || MAX_PHB_VAL < phb)
        {
            return CLI_ERR_INTERNAL;
        }

        color = atoi(arg[1]);
        if(color < MIN_COLOR_VAL || MAX_COLOR_VAL < color)
        {
            return CLI_ERR_INTERNAL;
        }

        if (color == 2)
        {
            CLI_LIB_PrintStr("drop precedence value can't be set to 2\r\n");
            return CLI_NO_ERROR;
        }
        j = 3;
    }

    while(arg[arg_num])
        arg_num++;

    for (num=j;num<arg_num;num=num+2)
    {
        cos = atoi(arg[num]);
        if(cos < MIN_COS_VAL || MAX_COS_VAL < cos)
        {
            return CLI_ERR_INTERNAL;
        }

        cfi = atoi(arg[num+1]);
        if(cfi < MIN_CFI_VAL || MAX_CFI_VAL < cfi)
        {
            return CLI_ERR_INTERNAL;
        }

        if (is_restore)
        {
            phb = default_cos_to_dscp_mapping[cos][cfi].phb;
            color = default_cos_to_dscp_mapping[cos][cfi].color;
        }

        for (i = 1 ; i <= ctrl_P->sys_info.max_port_number ; i++)
        {
            if (ctrl_P->CMenu.port_id_list[(UI32_T)((i-1)/8)]  & (1 << ( 7 - ((i-1)%8))) )
            {
                verify_port = i;

                verify_ret = verify_ethernet(verify_unit, verify_port, &lport);
                if (verify_ret != CLI_API_ETH_OK &&
                    verify_ret != CLI_API_ETH_TRUNK_MEMBER)
                {
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    continue;
                }

                if (COS_TYPE_E_NONE !=
                         L4_PMGR_QOS_SetPortCos2InternalDscp(lport, COS_TYPE_PRIORITY_USER, cos, cfi, phb, color))
                {
    #if (CLI_SUPPORT_PORT_NAME == 1)
                    {
                        UI8_T name[MAXSIZE_ifName+1] = {0};
                        CLI_LIB_Ifindex_To_Name(lport,name);
        #if (SYS_CPNT_EH == TRUE)
                        CLI_API_Show_Exception_Handeler_Msg();
        #else
                        CLI_LIB_PrintStr_1("Failed to set QoS CoS to DSCP on ethernet %s\r\n", name);
        #endif
                    }
    #else
        #if (SYS_CPNT_EH == TRUE)
                    CLI_API_Show_Exception_Handeler_Msg();
        #else
                    CLI_LIB_PrintStr_2("Failed to set QoS CoS to DSCP on ethernet %lu/%lu\r\n", verify_unit, verify_port);
        #endif
    #endif
                }
            }
        }
    }
#endif /* #if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE) */
    return CLI_NO_ERROR;
}

UI32_T CLI_API_QOS_SetIngressCos2Dscp_Pch(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE)
#if 0 /*2011.07.20 do not support trunk trust mode setting for now if support in the future will open this*/
    int  phb,color,cos,cfi,arg_num=0,j,num = 0;
    BOOL_T is_restore;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W3_QOS_MAP_COSDSCP:
                is_restore = FALSE;
            break;

        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W4_NO_QOS_MAP_COSDSCP:
                is_restore = TRUE;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }
    if(!is_restore)
    {
        phb = atoi(arg[0]);
    if(phb < MIN_PHB_VAL || phb > MAX_PHB_VAL)
            return CLI_ERR_INTERNAL;
    color = atoi(arg[1]);
    if(color < MIN_COLOR_VAL || color > MAX_COLOR_VAL)
            return CLI_ERR_INTERNAL;
        if (color == 2)
        {
            CLI_LIB_PrintStr("drop precedence value can't be set to 2\r\n");
            return CLI_NO_ERROR;
        }
    j = 3;
    }
    else
    {
        phb = 2;
    color = 2;
    j = 0;
    }
    while(arg[arg_num])
        arg_num++;
    for (num=j;num<arg_num;num=num+2)
    {
        cos = atoi(arg[num]);
    if(cos < MIN_COS_VAL || cos > MAX_COS_VAL)
            return CLI_ERR_INTERNAL;
    cfi = atoi(arg[num+1]);
    if(cfi < MIN_CFI_VAL || cfi > MAX_CFI_VAL)
            return CLI_ERR_INTERNAL;
        if (!L4_PMGR_QOS_SetIngressCos2Dscp(ctrl_P->CMenu.pchannel_id+SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER-1, cos, cfi, phb, color))
        {
#if (CLI_SUPPORT_PORT_NAME == 1)
            {
                UI8_T name[MAXSIZE_ifName+1] = {0};
                CLI_LIB_Ifindex_To_Name(lport,name);
#if (SYS_CPNT_EH == TRUE)
                CLI_API_Show_Exception_Handeler_Msg();
#else
                CLI_LIB_PrintStr_1("Failed to set QoS CoS to DSCP on ethernet %s\r\n", name);
#endif
            }
#else
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
#else
            CLI_LIB_PrintStr_1("Failed to set QoS CoS to DSCP on trunk %lu\r\n", ctrl_P->CMenu.pchannel_id);
#endif
#endif
         }
     }
#endif
#endif /* #if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE) */
     return CLI_NO_ERROR;
}

#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE)
static UI32_T Show_one_interface_cos_ingresscos2dscp(UI32_T lport,UI32_T line_num)
{
    UI32_T cos,cfi,phb,color;
    for (cos=0; cos<=MAX_COS_VAL; cos++)
    {
        CLI_LIB_PrintStr_1(" %lu           ",cos);
        for (cfi=0;cfi<=MAX_CFI_VAL;cfi++)
        {
            if (COS_TYPE_E_NONE ==
                L4_PMGR_QOS_GetPortCos2InternalDscp(lport, COS_TYPE_PRIORITY_USER, cos, cfi, &phb, &color))
            {
                 CLI_LIB_PrintStr_2(" (%lu,%lu)      ", phb,color);
            }
            else
            {
#if (SYS_CPNT_EH == TRUE)
                CLI_API_Show_Exception_Handeler_Msg();
#else
                CLI_LIB_PrintStr_1("Failed to get CoS port info, lport = %lu\r\n", lport);
#endif
                line_num += 1;
            }
            if (cfi == MAX_CFI_VAL)
                CLI_LIB_PrintStr("\r\n");
        }
    }

    return line_num;
}
#endif /* #if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE) */

UI32_T CLI_API_QOS_ShowIngressCos2Dscp(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE)
    UI32_T line_num = 0;

    if(arg[1]==NULL)
        return CLI_ERR_INTERNAL;

    switch(arg[0][0])
    {
        case 'e':
        case 'E':
        {
            UI32_T lport = 0;
            UI32_T verify_unit = atoi(arg[1]);
            UI32_T verify_port = atoi(strchr(arg[1], '/')+1 );
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
                if (!IF_PMGR_IfnameToIfindex(arg[1], &lport))
                {
                    CLI_LIB_PrintStr_1("%s does not exist\r\n",arg[1]);
                    return CLI_NO_ERROR;
                }
                SWCTRL_POM_LogicalPortToUserPort(lport, &verify_unit, &verify_port, &trunk_id);
            }
    #endif

            verify_ret = verify_ethernet(verify_unit, verify_port, &lport);

            switch(verify_ret)
            {
                case CLI_API_ETH_OK:
    #if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Information of %s\r\n", name);
                }
    #else
                    CLI_LIB_PrintStr_2("CoS Information of Eth %lu/%lu\r\n", verify_unit, verify_port);
    #endif
                    line_num++;
                    CLI_LIB_PrintStr(" CoS-DSCP map.(x,y),x: PHB,y: drop precedence:\r\n");
                    CLI_LIB_PrintStr(" CoS  : CFI  0          1\r\n");
                    CLI_LIB_PrintStr(" ---------------------------------\r\n");
                    Show_one_interface_cos_ingresscos2dscp(lport, line_num);
                    break;

                case CLI_API_ETH_TRUNK_MEMBER:
    #if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Information of %s\r\n", name);
                }
    #else
                    CLI_LIB_PrintStr_2("Information of Eth %lu/%lu\r\n", verify_unit, verify_port);
    #endif
                    line_num++;
                    CLI_LIB_PrintStr(" CoS-DSCP map.(x,y),x: PHB,y: drop precedence:\r\n");
                    CLI_LIB_PrintStr(" CoS  : CFI  0          1\r\n");
                    CLI_LIB_PrintStr(" ---------------------------------\r\n");
                    Show_one_interface_cos_ingresscos2dscp(lport, line_num);
                    break;

                case CLI_API_ETH_NOT_PRESENT:
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    break;

                case CLI_API_ETH_UNKNOWN_PORT:
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    break;
            }
        }
            break;

        case 'p':
        case 'P':
        {
        #if 0
            UI32_T lport = 0;
            UI32_T verify_trunk_id = atoi(arg[1]);
            CLI_API_TrunkStatus_T verify_ret;

            if( (verify_ret = verify_trunk(verify_trunk_id, &lport)) != CLI_API_TRUNK_OK)
            {
                display_trunk_msg(verify_ret, verify_trunk_id);
                return CLI_NO_ERROR;
            }
            else
            {
                CLI_LIB_PrintStr_1("CoS Information of Trunk %lu\r\n", verify_trunk_id);

                CLI_LIB_PrintStr(" CoS-DSCP map.(x,y),x: PHB,y: drop precedence:\r\n");
                CLI_LIB_PrintStr(" CoS  : CFI  0          1\r\n");
                CLI_LIB_PrintStr(" ---------------------------------\r\n");
                if((line_num = Show_one_interface_cos_ingresscos2dscp(lport, line_num)) == JUMP_OUT_MORE)
                {
                    return CLI_NO_ERROR;
                }
                else if (line_num == EXIT_SESSION_MORE)
                {
                    return CLI_EXIT_SESSION;
                }

                break;
            }
        #endif
        }
            return CLI_ERR_CMD_NOT_IMPLEMENT;

        default:
            return CLI_ERR_INTERNAL;
    }
#else
    UI32_T cos,cfi,phb,color;

    CLI_LIB_PrintStr(" CoS-DSCP map.(x,y),x: PHB,y: drop precedence:\r\n");
    CLI_LIB_PrintStr(" CoS  : CFI  0          1\r\n");
    CLI_LIB_PrintStr(" ---------------------------------\r\n");

    for (cos = 0; cos <= MAX_COS_VAL; cos++)
    {
        CLI_LIB_PrintStr_1(" %lu           ", cos);
        for (cfi = 0; cfi <= MAX_CFI_VAL; cfi++)
        {
            if (COS_TYPE_E_NONE !=
                L4_PMGR_QOS_GetCos2InternalDscp(COS_TYPE_PRIORITY_USER, cos, cfi, &phb, &color))
            {
                CLI_LIB_PrintStr("\r\n");
                CLI_LIB_PrintStr_2("Failed to get CoS-DSCP map, cos = %lu, cfi = %lu\r\n", cos, cfi);
                return CLI_ERR_INTERNAL;
            }

            CLI_LIB_PrintStr_2(" (%lu,%lu)      ", phb,color);

            if (cfi == MAX_CFI_VAL)
            {
                CLI_LIB_PrintStr("\r\n");
            }
        }
    }

#endif /* #if (SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT == TRUE) */
    return CLI_NO_ERROR;
}
#endif /* SYS_CPNT_COS_ING_COS_TO_INTER_DSCP */


#if (SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP == TRUE)
/* [Internace ethernet config mode]
 * qos map ip-prec-dscp {phb0 color0 phb1 color1 phb2 color2 phb3 color3 phb4 color4 phb5 color5
 *                       phb6 color6 phb7 color7}
 * no qos map ip-prec-dscp
 */
UI32_T CLI_API_QOS_SetIngressPre2Dscp(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT == TRUE)
    int  phb, color, i, arg_num = 0, num = 0;
    UI32_T pre;
    UI32_T verify_unit, verify_port, lport;
    CLI_API_EthStatus_T verify_ret;
    BOOL_T is_restore;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_QOS_MAP_IPPRECDSCP:
                is_restore = FALSE;
            break;

        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W4_NO_QOS_MAP_IPPRECDSCP:
                is_restore = TRUE;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }

    while(arg[arg_num])
    {
        arg_num++;
    }

    verify_unit = ctrl_P->CMenu.unit_id;
    for (i = 1 ; i <= ctrl_P->sys_info.max_port_number ; i++)
    {
        if (ctrl_P->CMenu.port_id_list[(UI32_T)((i-1)/8)]  & (1 << ( 7 - ((i-1)%8))) )
        {
            verify_port = i;

            verify_ret = verify_ethernet(verify_unit, verify_port, &lport);
            if (verify_ret != CLI_API_ETH_OK &&
                verify_ret != CLI_API_ETH_TRUNK_MEMBER)
            {
                display_ethernet_msg(verify_ret, verify_unit, verify_port);
                continue;
            }

            if(!is_restore)
            {
                for (num = 0; num < arg_num; num = num+2)
                {
                    pre = num/2;

                    phb = atoi(arg[num]);
                    if(phb < MIN_PHB_VAL || MAX_PHB_VAL < phb)
                    {
                        return CLI_ERR_INTERNAL;
                    }

                    color = atoi(arg[num+1]);
                    if(color < MIN_COLOR_VAL || MAX_COLOR_VAL < color)
                    {
                        return CLI_ERR_INTERNAL;
                    }

                    if (COS_TYPE_E_NONE !=
                        L4_PMGR_QOS_SetPortPre2InternalDscp(lport, COS_TYPE_PRIORITY_USER, pre, phb, color))
                    {
#if (SYS_CPNT_EH == TRUE)
                        CLI_API_Show_Exception_Handeler_Msg();
#else
    #if (CLI_SUPPORT_PORT_NAME == 1)
                        UI8_T name[MAXSIZE_ifName+1] = {0};

                        CLI_LIB_Ifindex_To_Name(lport,name);
                        CLI_LIB_PrintStr_1("Failed to set QoS IP-prec-DSCP on ethernet %s\r\n", name);

    #else
                        CLI_LIB_PrintStr_2("Failed to set QoS IP-prec-DSCP on ethernet %lu/%lu\r\n", verify_unit, verify_port);
    #endif /* CLI_SUPPORT_PORT_NAME */
#endif /* SYS_CPNT_EH */
                    }
                }
            }
            else
            {
                typedef struct
                {
                    UI32_T phb;
                    UI32_T color;
                } INTERNAL_DSCP_T;

                const INTERNAL_DSCP_T default_pre_to_dscp_mapping[INGRESS_PRE_TO_DSCP_MAPPING_ENTRY_NMB] = COS_TYPE_DEFAULT_PRE_TO_DSCP_MAP_ARRAY;

                COS_TYPE_ERROR_CODE_E result;

                for (pre = MIN_PRE_VAL; pre <= MAX_PRE_VAL; ++pre)
                {
                    phb = default_pre_to_dscp_mapping[pre].phb;
                    color = default_pre_to_dscp_mapping[pre].color;

                    result = L4_PMGR_QOS_SetPortPre2InternalDscp(lport, COS_TYPE_PRIORITY_USER, pre, phb, color);
                    if (COS_TYPE_E_NONE != result)
                    {
#if (SYS_CPNT_EH == TRUE)
                        CLI_API_Show_Exception_Handeler_Msg();
#else
    #if (CLI_SUPPORT_PORT_NAME == 1)
                        UI8_T name[MAXSIZE_ifName+1] = {0};

                        CLI_LIB_Ifindex_To_Name(lport,name);
                        CLI_LIB_PrintStr_1("Failed to set QoS IP-prec-DSCP on ethernet %s\r\n", name);

    #else
                        CLI_LIB_PrintStr_2("Failed to set QoS IP-prec-DSCP on ethernet %lu/%lu\r\n", verify_unit, verify_port);
    #endif /* CLI_SUPPORT_PORT_NAME */
#endif /* SYS_CPNT_EH */
                        break;
                    }
                }
            }
        }
    }
#endif /* #if (SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT == TRUE) */
    return CLI_NO_ERROR;
}

UI32_T CLI_API_QOS_SetIngressPre2Dscp_Pch(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT == TRUE)
#if 0
    int  phb,color,arg_num = 0,num = 0;
    BOOL_T is_restore;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W3_QOS_MAP_IPPRECDSCP:
                is_restore = FALSE;
            break;

        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W4_NO_QOS_MAP_IPPRECDSCP:
                is_restore = TRUE;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }
    while(arg[arg_num])
        arg_num++;
    if(!is_restore)
    {
        for (num=0;num<arg_num;num=num+2)
        {
            phb = atoi(arg[num]);
            if(phb < MIN_PHB_VAL || phb > MAX_PHB_VAL)
                return CLI_ERR_INTERNAL;
            color = atoi(arg[num+1]);
            if(color < MIN_COLOR_VAL || color > MAX_COLOR_VAL)
                return CLI_ERR_INTERNAL;

           if(!L4_PMGR_QOS_SetIngressPre2Dscp(ctrl_P->CMenu.pchannel_id+SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER-1, num/2, phb, color))
            {
#if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
#if (SYS_CPNT_EH == TRUE)
                    CLI_API_Show_Exception_Handeler_Msg();
#else
                    CLI_LIB_PrintStr_1("Failed to set QoS IP-prec-DSCP on ethernet %s\r\n", name);
#endif
                }
#else
#if (SYS_CPNT_EH == TRUE)
                CLI_API_Show_Exception_Handeler_Msg();
#else
                CLI_LIB_PrintStr_1("Failed to set QoS IP-prec-DSCP on trunk %lu\r\n",ctrl_P->CMenu.pchannel_id);
#endif
#endif

            }
        }
    }
    else
    {
        if(!L4_PMGR_QOS_ResetIngressPre2Dscp(ctrl_P->CMenu.pchannel_id+SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER-1))
        {
#if (CLI_SUPPORT_PORT_NAME == 1)
           {
            UI8_T name[MAXSIZE_ifName+1] = {0};
            CLI_LIB_Ifindex_To_Name(lport,name);
    #if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
    #else
            CLI_LIB_PrintStr_1("Failed to set QoS IP-prec-DSCP on ethernet %s\r\n", name);
    #endif
            }
#else
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
#else
            CLI_LIB_PrintStr_1("Failed to set QoS IP-prec-DSCP on trunk %lu\r\n", ctrl_P->CMenu.pchannel_id);
#endif
#endif
        }
    }
#endif /* if 0 */
#endif /*#if (SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT == TRUE) */
    return CLI_NO_ERROR;
}

#if (SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT == TRUE)
static UI32_T Show_one_interface_cos_ingresspre2dscp(UI32_T lport,UI32_T line_num)
{
    UI32_T phb, color, prec;
    UI8_T phb_ar[MAX_PRE_VAL + 1] = {0}, color_ar[MAX_PRE_VAL + 1] = {0};
    BOOL_T has_error = FALSE;

    for (prec = MIN_PRE_VAL; prec <= MAX_PRE_VAL; ++prec)
    {
        if (COS_TYPE_E_NONE !=
            L4_PMGR_QOS_GetPortPre2InternalDscp(lport, COS_TYPE_PRIORITY_USER, prec, &phb, &color))
        {
            has_error = TRUE;
            break;
        }
        phb_ar[prec]   = (UI8_T) phb;
        color_ar[prec] = (UI8_T) color;
    }

    if (has_error == TRUE)
    {
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr_1("Failed to get CoS port info, lport = %lu\r\n", lport);
#endif
        line_num += 1;
    }
    else
    {
        CLI_LIB_PrintStr(" IP-prec-DSCP map: \r\n");
        CLI_LIB_PrintStr(" IP-prec:         0     1     2     3     4     5     6     7 \r\n");
        CLI_LIB_PrintStr(" ------------------------------------------------------------ \r\n");
        CLI_LIB_PrintStr_4(" PHB:             %d     %d     %d     %d     ",phb_ar[0],phb_ar[1],phb_ar[2],phb_ar[3]);
        CLI_LIB_PrintStr_4("%d     %d     %d     %d\r\n",phb_ar[4],phb_ar[5],phb_ar[6],phb_ar[7]);
        CLI_LIB_PrintStr_4(" drop precedence: %d     %d     %d     %d     ",color_ar[0],color_ar[1],color_ar[2],color_ar[3]);
        CLI_LIB_PrintStr_4("%d     %d     %d     %d\r\n",color_ar[4],color_ar[5],color_ar[6],color_ar[7]);
    }

    return line_num;
}
#endif /* #if (SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT == TRUE) */

UI32_T CLI_API_QOS_ShowIngressPre2Dscp(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT == TRUE)
    UI32_T line_num = 0;

    if(arg[1]==NULL)
        return CLI_ERR_INTERNAL;

    switch(arg[0][0])
    {
        case 'e':
        case 'E':
        {
            UI32_T lport = 0;
            UI32_T verify_unit = atoi(arg[1]);
            UI32_T verify_port = atoi( strchr(arg[1], '/')+1 );
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
                if (!IF_PMGR_IfnameToIfindex(arg[1], &lport))
                {
                    CLI_LIB_PrintStr_1("%s does not exist\r\n",arg[1]);
                    return CLI_NO_ERROR;
                }
                SWCTRL_POM_LogicalPortToUserPort(lport, &verify_unit, &verify_port, &trunk_id);
            }
    #endif

            verify_ret = verify_ethernet(verify_unit, verify_port, &lport);

            switch(verify_ret)
            {
                case CLI_API_ETH_OK:
    #if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Information of %s\r\n", name);
                }
    #else
                    CLI_LIB_PrintStr_2("Information of Eth %lu/%lu\r\n", verify_unit, verify_port);
    #endif
                    line_num++;
                    Show_one_interface_cos_ingresspre2dscp(lport, line_num);
                    break;

                case CLI_API_ETH_TRUNK_MEMBER:
    #if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Information of %s\r\n", name);
                }
    #else
                    CLI_LIB_PrintStr_2("Information of Eth %lu/%lu\r\n", verify_unit, verify_port);
    #endif
                    line_num++;
                    Show_one_interface_cos_ingresspre2dscp(lport, line_num);
                    break;

                case CLI_API_ETH_NOT_PRESENT:
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    break;

                case CLI_API_ETH_UNKNOWN_PORT:
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    break;
            }
        }
            break;

        case 'p':
        case 'P':
        {
        #if 0
            UI32_T lport = 0;
            UI32_T verify_trunk_id = atoi(arg[1]);
            CLI_API_TrunkStatus_T verify_ret;

            if( (verify_ret = verify_trunk(verify_trunk_id, &lport)) != CLI_API_TRUNK_OK)
            {
                display_trunk_msg(verify_ret, verify_trunk_id);
                return CLI_NO_ERROR;
            }
            else
            {
                CLI_LIB_PrintStr_1("Information of Trunk %lu\r\n", verify_trunk_id);

                if((line_num = Show_one_interface_cos_ingresspre2dscp(lport, line_num)) == JUMP_OUT_MORE)
                {
                    return CLI_NO_ERROR;
                }
                else if (line_num == EXIT_SESSION_MORE)
                {
                    return CLI_EXIT_SESSION;
                }

                break;
            }
        #endif
        }
            return CLI_ERR_CMD_NOT_IMPLEMENT;

        default:
            return CLI_ERR_INTERNAL;
    }
#endif /* SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT */
    return CLI_NO_ERROR;
}
#endif /* (SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP == TRUE) */

#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP == TRUE)

/* [Global config mode]
 * qos map dscp-mutation {phb color} from {dscp} [dscp]*
 * no qos map dscp-mutation {dscp} [dscp]*
 */
UI32_T
CLI_API_QOS_SetIngressDscp2Dscp_Global(
    UI16_T cmd_idx,
    char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == FALSE)
    typedef struct
    {
        UI32_T phb;
        UI32_T color;
    } INTERNAL_DSCP_T;

    const INTERNAL_DSCP_T default_dscp_to_dscp_mapping[INGRESS_DSCP_TO_DSCP_MAPPING_ENTRY_NMB] = COS_TYPE_DEFAULT_DSCP_TO_DSCP_MAP_ARRAY;
    int    phb, color, dscp, i;
    UI32_T arg_index, arg_num;
    BOOL_T is_restore;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_GLOBAL_CMD_W3_QOS_MAP_DSCPMUTATION:
                is_restore = FALSE;
            break;

        case PRIVILEGE_CFG_GLOBAL_CMD_W4_NO_QOS_MAP_DSCPMUTATION:
                is_restore = TRUE;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }

    arg_index = 0;
    if(!is_restore)
    {
        phb = atoi(arg[0]);
        if(phb < MIN_PHB_VAL || MAX_PHB_VAL < phb)
        {
            return CLI_ERR_INTERNAL;
        }

        color = atoi(arg[1]);
        if(color < MIN_COLOR_VAL || MAX_COLOR_VAL < color)
        {
            return CLI_ERR_INTERNAL;
        }

        if (color == 2)
        {
            CLI_LIB_PrintStr("drop precedence value can't be set to 2\r\n");
            return CLI_NO_ERROR;
        }

        arg_index = 3;
    }

    arg_num = 0;
    while(arg[arg_num])
    {
        arg_num++;
    }

    for (; arg_index < arg_num; arg_index++)
    {
        dscp = atoi(arg[arg_index]);
        if(dscp < MIN_DSCP_VAL || MAX_DSCP_VAL < dscp)
        {
            return CLI_ERR_INTERNAL;
        }

        if (is_restore)
        {
            phb = default_dscp_to_dscp_mapping[dscp].phb;
            color = default_dscp_to_dscp_mapping[dscp].color;
        }

        if (COS_TYPE_E_NONE !=
            L4_PMGR_QOS_SetDscp2InternalDscp(COS_TYPE_PRIORITY_USER, dscp, phb, color))
        {
            CLI_LIB_PrintStr("Failed to set QoS DSCP to DSCP\r\n");
        }
    }
#endif /* #if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == FALSE) */
    return CLI_NO_ERROR;
}

/* [Internace ethernet config mode]
 * qos map dscp-mutation {phb color} from {dscp} [dscp]*
 * no qos map dscp-mutation {dscp} [dscp]*
 */
UI32_T CLI_API_QOS_SetIngressDscp2Dscp(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE)
    typedef struct
    {
        UI32_T phb;
        UI32_T color;
    } INTERNAL_DSCP_T;

    const INTERNAL_DSCP_T default_dscp_to_dscp_mapping[INGRESS_DSCP_TO_DSCP_MAPPING_ENTRY_NMB] = COS_TYPE_DEFAULT_DSCP_TO_DSCP_MAP_ARRAY;
    int dscp, i;
    int phb = 0, color = 0;
    UI32_T verify_unit, verify_port, lport, arg_index, arg_num;
    CLI_API_EthStatus_T verify_ret;
    BOOL_T is_restore;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_QOS_MAP_DSCPMUTATION:
                is_restore = FALSE;
            break;

        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W4_NO_QOS_MAP_DSCPMUTATION:
                is_restore = TRUE;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }

    arg_index = 0;
    if(!is_restore)
    {
        phb = atoi(arg[0]);
        if(phb < MIN_PHB_VAL || MAX_PHB_VAL < phb)
        {
            return CLI_ERR_INTERNAL;
        }

        color = atoi(arg[1]);
        if(color < MIN_COLOR_VAL || MAX_COLOR_VAL < color)
        {
            return CLI_ERR_INTERNAL;
        }

        if (color == 2)
        {
            CLI_LIB_PrintStr("drop precedence value can't be set to 2\r\n");
            return CLI_NO_ERROR;
        }

        arg_index = 3;
    }

    arg_num = 0;
    while(arg[arg_num])
    {
        arg_num++;
    }

    verify_unit = ctrl_P->CMenu.unit_id;
    for (; arg_index < arg_num; arg_index++)
    {
        dscp = atoi(arg[arg_index]);
        if(dscp < MIN_DSCP_VAL || MAX_DSCP_VAL < dscp)
        {
            return CLI_ERR_INTERNAL;
        }

        for (i = 1 ; i <= ctrl_P->sys_info.max_port_number ; i++)
        {
            if (ctrl_P->CMenu.port_id_list[(UI32_T)((i-1)/8)]  & (1 << ( 7 - ((i-1)%8))) )
            {
                verify_port = i;

                verify_ret = verify_ethernet(verify_unit, verify_port, &lport);
                if (verify_ret != CLI_API_ETH_OK &&
                    verify_ret != CLI_API_ETH_TRUNK_MEMBER)
                {
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    continue;
                }

                if (is_restore)
                {
                    phb = default_dscp_to_dscp_mapping[dscp].phb;
                    color = default_dscp_to_dscp_mapping[dscp].color;
                }

                if (COS_TYPE_E_NONE !=
                    L4_PMGR_QOS_SetPortDscp2InternalDscp(lport, COS_TYPE_PRIORITY_USER, dscp, phb, color))
                {
#if (CLI_SUPPORT_PORT_NAME == 1)
                    {
                        UI8_T name[MAXSIZE_ifName+1] = {0};
                        CLI_LIB_Ifindex_To_Name(lport,name);
    #if (SYS_CPNT_EH == TRUE)
                        CLI_API_Show_Exception_Handeler_Msg();
    #else
                        CLI_LIB_PrintStr_1("Failed to set QoS DSCP to DSCP on ethernet %s\r\n", name);
    #endif
                    }
#else
    #if (SYS_CPNT_EH == TRUE)
                    CLI_API_Show_Exception_Handeler_Msg();
    #else
                    CLI_LIB_PrintStr_2("Failed to set QoS DSCP to DSCP on ethernet %lu/%lu\r\n", verify_unit, verify_port);
    #endif
#endif
                }
            }
        }
    }
#endif /* #if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE) */
    return CLI_NO_ERROR;
}

/* [no] qos map phb-queue {queue} from {phb1|[phb2 phb3 ...]}
 */
UI32_T CLI_API_QOS_SetIngressDscp2Dscp_Pch(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE)
#if 0 /*2011.07.20 do not support trunk trust mode setting for now if support in the future will open this*/
    int  phb,color,dscp;
    UI32_T j,num,arg_num=0;
    BOOL_T is_restore;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W3_QOS_MAP_DSCPMUTATION:
                is_restore = FALSE;
            break;

        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W4_NO_QOS_MAP_DSCPMUTATION:
                is_restore = TRUE;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }
    if(!is_restore)
    {
        phb = atoi(arg[0]);
        if(phb < MIN_PHB_VAL || phb > MAX_PHB_VAL)
            return CLI_ERR_INTERNAL;
        color = atoi(arg[1]);
        if(color < MIN_COLOR_VAL || color > MAX_COLOR_VAL)
            return CLI_ERR_INTERNAL;
        if (color == 2)
        {
            CLI_LIB_PrintStr("drop precedence value can't be set to 2\r\n");
            return CLI_NO_ERROR;
        }
        j = 3;
    }
    else
    {
        phb = 8;
        color = 8;
        j=0;
    }
    while(arg[arg_num])
        arg_num++;
    for (num=j;num<arg_num;num++)
    {
        dscp = atoi(arg[num]);
    if(dscp < MIN_DSCP_VAL || dscp > MAX_DSCP_VAL)
        return CLI_ERR_INTERNAL;

        if (!L4_PMGR_QOS_SetIngressDscp2Dscp(ctrl_P->CMenu.pchannel_id+SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER-1, dscp, phb, color))
        {
#if (CLI_SUPPORT_PORT_NAME == 1)
            {
                UI8_T name[MAXSIZE_ifName+1] = {0};
                CLI_LIB_Ifindex_To_Name(lport,name);
#if (SYS_CPNT_EH == TRUE)
                CLI_API_Show_Exception_Handeler_Msg();
#else
                CLI_LIB_PrintStr_1("Failed to set QoS DSCP to DSCP on ethernet %s\r\n", name);
#endif
            }
#else
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
#else
            CLI_LIB_PrintStr_1("Failed to set QoS DSCP to DSCP on trunk %lu\r\n", ctrl_P->CMenu.pchannel_id);
#endif
#endif
        }
    }
#endif
#endif /* SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT */
    return CLI_NO_ERROR;
}

#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE)
static UI32_T Show_one_interface_cos_ingressdscp2dscp(UI32_T lport,UI32_T line_num)
{
    UI32_T dscp;
    UI32_T  phb,color;

    for(dscp = 0; dscp <= MAX_DSCP_VAL; dscp++)
    {
        if(L4_PMGR_QOS_GetIngressDscp2Dscp(lport, dscp, &phb, &color))
        {
            if ((dscp % 10) == 0)
            {
                CLI_LIB_PrintStr_1("\r\n %lu :    ",dscp/10);
            }
            CLI_LIB_PrintStr_2("(%lu,%lu) ", phb,color);
        }
    else
        {
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
#else
            CLI_LIB_PrintStr_1("Failed to get CoS port info, lport = %lu\r\n", lport);
#endif
            line_num += 1;
        }
    }
    return line_num;
}
#endif /* #if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE) */

UI32_T CLI_API_QOS_ShowIngressDscp2Dscp(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE)
    UI32_T line_num = 0;

    if(arg[1]==NULL)
        return CLI_ERR_INTERNAL;

    switch(arg[0][0])
    {
        case 'e':
        case 'E':
        {
            UI32_T lport = 0;
            UI32_T verify_unit = atoi(arg[1]);
            UI32_T verify_port = atoi( strchr(arg[1], '/')+1 );
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
                if (!IF_PMGR_IfnameToIfindex(arg[1], &lport))
                {
                    CLI_LIB_PrintStr_1("%s does not exist\r\n",arg[1]);
                    return CLI_NO_ERROR;
                }
                SWCTRL_POM_LogicalPortToUserPort(lport, &verify_unit, &verify_port, &trunk_id);
            }
    #endif

            verify_ret = verify_ethernet(verify_unit, verify_port, &lport);

            switch(verify_ret)
            {
                case CLI_API_ETH_OK:
    #if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Information of %s\r\n", name);
                }
    #else
                    CLI_LIB_PrintStr_2("Information of Eth %lu/%lu\r\n", verify_unit, verify_port);
    #endif
                    line_num++;
                    CLI_LIB_PrintStr(" DSCP mutation map.(x,y),x: PHB,y: drop precedence:\r\n");
                    CLI_LIB_PrintStr(" d1: d2 0     1     2     3     4     5     6     7     8     9\r\n");
                    CLI_LIB_PrintStr(" ------------------------------------------------------------------- ");
                    Show_one_interface_cos_ingressdscp2dscp(lport, line_num);
                    CLI_LIB_PrintStr("\r\n");
                    break;

                case CLI_API_ETH_TRUNK_MEMBER:
    #if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Information of %s\r\n", name);
                }
    #else
                    CLI_LIB_PrintStr_2("Information of Eth %lu/%lu\r\n", verify_unit, verify_port);
    #endif
                    line_num++;
                    CLI_LIB_PrintStr(" DSCP mutation map.(x,y),x: PHB,y: drop precedence:\r\n");
                    CLI_LIB_PrintStr(" d1: d2 0     1     2     3     4     5     6     7     8     9\r\n");
                    CLI_LIB_PrintStr(" ------------------------------------------------------------------- ");
                    Show_one_interface_cos_ingressdscp2dscp(lport, line_num);
                    CLI_LIB_PrintStr("\r\n");
                    break;

                case CLI_API_ETH_NOT_PRESENT:
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    break;

                case CLI_API_ETH_UNKNOWN_PORT:
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    break;
            }
        }
            break;

        case 'p':
        case 'P':
        {
        #if 0
            UI32_T lport = 0;
            UI32_T verify_trunk_id = atoi(arg[1]);
            CLI_API_TrunkStatus_T verify_ret;

            if( (verify_ret = verify_trunk(verify_trunk_id, &lport)) != CLI_API_TRUNK_OK)
            {
                display_trunk_msg(verify_ret, verify_trunk_id);
                return CLI_NO_ERROR;
            }
            else
            {
                CLI_LIB_PrintStr_1("Information of Trunk %lu\r\n", verify_trunk_id);
                CLI_LIB_PrintStr(" DSCP mutation map.(x,y),x: PHB,y: drop precedence:\r\n");
                CLI_LIB_PrintStr(" d1: d2 0     1     2     3     4     5     6     7     8     9\r\n");
                CLI_LIB_PrintStr(" ------------------------------------------------------------------- ");
                if((line_num = Show_one_interface_cos_ingressdscp2dscp(lport, line_num)) == JUMP_OUT_MORE)
                {
                    return CLI_NO_ERROR;
                }
                else if (line_num == EXIT_SESSION_MORE)
                {
                    return CLI_EXIT_SESSION;
                }

                break;
            }
        #endif
        }
            return CLI_ERR_CMD_NOT_IMPLEMENT;

        default:
            return CLI_ERR_INTERNAL;
    }
#else
    UI32_T dscp;
    UI32_T phb,color;

    CLI_LIB_PrintStr(" DSCP mutation map.(x,y),x: PHB,y: drop precedence:\r\n");
    CLI_LIB_PrintStr(" d1: d2 0     1     2     3     4     5     6     7     8     9\r\n");
    CLI_LIB_PrintStr(" ------------------------------------------------------------------- ");

    for (dscp = 0; dscp <= MAX_DSCP_VAL; dscp++)
    {
        if (COS_TYPE_E_NONE !=
            L4_PMGR_QOS_GetDscp2InternalDscp(COS_TYPE_PRIORITY_USER, dscp, &phb, &color))
        {
            CLI_LIB_PrintStr("\r\n");
            CLI_LIB_PrintStr("Failed to get DSCP mutation map\r\n");
            break;
        }

        if ((dscp % 10) == 0)
        {
            CLI_LIB_PrintStr_1("\r\n %lu :    ", dscp/10);
        }
        CLI_LIB_PrintStr_2("(%lu,%lu) ", phb, color);
    }

    CLI_LIB_PrintStr("\r\n");

#endif /* #if (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT == TRUE) */
    return CLI_NO_ERROR;
}
#endif /* (SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP == TRUE) */

#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE == TRUE)
/* [Global config mode]
 * show qos map phb-queue
 */
UI32_T CLI_API_QOS_ShowDscp2Queue(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT == FALSE)
    UI8_T  queue_ar[MAX_PHB_VAL + 1];
    UI32_T phb, queue;
    BOOL_T has_error = FALSE;

    for (phb = 0; phb <= MAX_PHB_VAL; ++phb)
    {
        if (COS_TYPE_E_NONE !=
            L4_PMGR_QOS_GetIngressPhb2Queue(COS_TYPE_PRIORITY_USER, phb, &queue))
        {
            has_error = TRUE;
            break;
        }

        queue_ar[phb] = (UI8_T) queue;
    }

    if (has_error == TRUE)
    {
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr("Failed to get CoS info of PHB to queue\r\n");
#endif /* #if (SYS_CPNT_EH == TRUE) */
    }
    else
    {
        CLI_LIB_PrintStr(" PHB Queue Map:\r\n");
        CLI_LIB_PrintStr(" PHB:       0     1     2     3     4     5     6     7 \r\n");
        CLI_LIB_PrintStr(" ------------------------------------------------------- \r\n");
        CLI_LIB_Printf(" Queue:%6d%6d%6d%6d%6d%6d%6d%6d\r\n",
            queue_ar[0], queue_ar[1], queue_ar[2], queue_ar[3],
            queue_ar[4], queue_ar[5], queue_ar[6], queue_ar[7]);
    }

#endif /* (SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT == FALSE) */
    return CLI_NO_ERROR;
}

#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT == TRUE)
static UI32_T Show_one_interface_cos_ingressdscp2queue(UI32_T lport,UI32_T line_num)
{
    UI8_T  queue_ar[MAX_PHB_VAL + 1];
    UI32_T phb, queue;
    BOOL_T has_error = FALSE;

    for (phb = 0; phb <= MAX_PHB_VAL; ++phb)
    {
        if (COS_TYPE_E_NONE !=
            L4_PMGR_QOS_GetPortIngressPhb2Queue(lport, COS_TYPE_PRIORITY_USER, phb, &queue))
        {
            has_error = TRUE;
            break;
        }

        queue_ar[phb] = (UI8_T) queue;
    }

    if (has_error == TRUE)
    {
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr_1("Failed to get CoS port info, lport = %lu\r\n", lport);
#endif
        line_num += 1;
    }
    else
    {
        CLI_LIB_PrintStr(" PHB Queue Map:\r\n");
        CLI_LIB_PrintStr(" PHB:       0     1     2     3     4     5     6     7 \r\n");
        CLI_LIB_PrintStr(" ------------------------------------------------------- \r\n");
        CLI_LIB_Printf(" Queue:%6d%6d%6d%6d%6d%6d%6d%6d\r\n",
            queue_ar[0], queue_ar[1], queue_ar[2], queue_ar[3],
            queue_ar[4], queue_ar[5], queue_ar[6], queue_ar[7]);
        line_num += 4;
    }

    return line_num;
}
#endif /* #if (SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT == TRUE) */

/* show qos map phb-queue interface { ethernet (unit)/(port) | port-channel (trunk) }
 */
UI32_T CLI_API_QOS_ShowDscp2Queue_Interface(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT == TRUE)
    UI32_T line_num = 0;

    if(arg[1]==NULL)
        return CLI_ERR_INTERNAL;

    switch(arg[0][0])
    {
        case 'e':
        case 'E':
        {
            UI32_T lport = 0;
            UI32_T verify_unit = atoi(arg[1]);
            UI32_T verify_port = atoi(strchr(arg[1], '/')+1 );
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
                if (!IF_PMGR_IfnameToIfindex(arg[1], &lport))
                {
                    CLI_LIB_PrintStr_1("%s does not exist\r\n",arg[1]);
                    return CLI_NO_ERROR;
                }
                SWCTRL_POM_LogicalPortToUserPort(lport, &verify_unit, &verify_port, &trunk_id);
            }
    #endif

            verify_ret = verify_ethernet(verify_unit, verify_port, &lport);

            switch(verify_ret)
            {
                case CLI_API_ETH_OK:
    #if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Information of %s\r\n", name);
                }
    #else
                    CLI_LIB_PrintStr_2("Information of Eth %lu/%lu\r\n", verify_unit, verify_port);
    #endif
                    line_num++;
                    Show_one_interface_cos_ingressdscp2queue(lport, line_num);
                    break;

                case CLI_API_ETH_TRUNK_MEMBER:
    #if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Information of %s\r\n", name);
                }
    #else
                    CLI_LIB_PrintStr_2("Information of Eth %lu/%lu\r\n", verify_unit, verify_port);
    #endif
                    line_num++;
                    Show_one_interface_cos_ingressdscp2queue(lport, line_num);
                    break;

                case CLI_API_ETH_NOT_PRESENT:
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    break;

                case CLI_API_ETH_UNKNOWN_PORT:
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    break;
            }
        }
            break;

        case 'p':
        case 'P':
        {
        #if 0
            UI32_T lport = 0;
            UI32_T verify_trunk_id = atoi(arg[1]);
            CLI_API_TrunkStatus_T verify_ret;

            if( (verify_ret = verify_trunk(verify_trunk_id, &lport)) != CLI_API_TRUNK_OK)
            {
                display_trunk_msg(verify_ret, verify_trunk_id);
                return CLI_NO_ERROR;
            }
            else
            {
                CLI_LIB_PrintStr_1("Information of Trunk %lu\r\n", verify_trunk_id);

                if((line_num = Show_one_interface_cos_ingressdscp2queue(lport, line_num)) == JUMP_OUT_MORE)
                {
                    return CLI_NO_ERROR;
                }
                else if (line_num == EXIT_SESSION_MORE)
                {
                    return CLI_EXIT_SESSION;
                }

                break;
            }
        #endif
        }
            return CLI_ERR_CMD_NOT_IMPLEMENT;

        default:
            return CLI_ERR_INTERNAL;
    }
#endif /* #if (SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT == TRUE) */
    return CLI_NO_ERROR;
}

/* [no] qos map phb-queue {queue} from {phb1| [phb2 phb3 ...]}
 */
UI32_T CLI_API_QOS_SetDscp2Queue(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT == FALSE)
    const UI32_T default_phb_to_queue_mapping[MAX_PHB_VAL + 1] = COS_TYPE_DEFAULT_PHB_TO_QUEUE_MAP_ARRAY;
    BOOL_T set_default = FALSE;
    int  queue = 0, phb = 0;
    UI32_T arg_total_number, arg_index, arg_phb_start;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_GLOBAL_CMD_W3_QOS_MAP_PHBQUEUE:
            queue = atoi(arg[0]);
            if(queue < MIN_QUEUE_VAL || queue > MAX_QUEUE_VAL)
            {
                return CLI_ERR_INTERNAL;
            }

            arg_phb_start = 2;
            break;

        case PRIVILEGE_CFG_GLOBAL_CMD_W4_NO_QOS_MAP_PHBQUEUE:
            set_default = TRUE;
            arg_phb_start= 0;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }

    arg_total_number = 0;
    while(arg[arg_total_number])
    {
        arg_total_number++;
    }

    for (arg_index = arg_phb_start; arg_index < arg_total_number; arg_index++)
    {
        phb = atoi(arg[arg_index]);
        if(phb < MIN_PHB_VAL || phb > MAX_PHB_VAL)
        {
            return CLI_ERR_INTERNAL;
        }

        if (set_default == TRUE)
        {
            queue = default_phb_to_queue_mapping[phb];
        }

        if (COS_TYPE_E_NONE != L4_PMGR_QOS_SetIngressPhb2Queue(
                                   COS_TYPE_PRIORITY_USER, phb, queue))
        {
    #if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
    #else
            CLI_LIB_PrintStr("Failed to set QoS DSCP to queue\r\n");
    #endif /* #if (SYS_CPNT_EH == TRUE) */
        }
    }
#endif /* #if (SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT == FALSE) */
    return CLI_NO_ERROR;
}

/* [no] qos map phb-queue {queue} from {phb1|[phb2 phb3 ...]}
 */
UI32_T CLI_API_QOS_SetDscp2Queue_Eth(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT == TRUE)
    const UI32_T default_phb_to_queue_mapping[MAX_PHB_VAL + 1] = COS_TYPE_DEFAULT_PHB_TO_QUEUE_MAP_ARRAY;
    BOOL_T set_default = FALSE;
    int  phb, queue = 0;
    UI32_T arg_total_number, arg_index, arg_phb_start;
    UI32_T unit , port, lport;
    CLI_API_EthStatus_T verify_ret;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_QOS_MAP_PHBQUEUE:
            queue = atoi(arg[0]);
            if(queue < MIN_QUEUE_VAL || queue > MAX_QUEUE_VAL)
            {
                return CLI_ERR_INTERNAL;
            }

            arg_phb_start = 2;
            break;

        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W4_NO_QOS_MAP_PHBQUEUE:
            set_default = TRUE;
            arg_phb_start= 0;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }

    arg_total_number = 0;
    while(arg[arg_total_number])
    {
        arg_total_number++;
    }

    unit = ctrl_P->CMenu.unit_id;

    for (arg_index = arg_phb_start; arg_index < arg_total_number; arg_index++)
    {
        phb = atoi(arg[arg_index]);
        if(phb < MIN_PHB_VAL || phb > MAX_PHB_VAL)
        {
            return CLI_ERR_INTERNAL;
        }

        if (set_default == TRUE)
        {
            queue = default_phb_to_queue_mapping[phb];
        }

        for (port = 1 ; port <= ctrl_P->sys_info.max_port_number ; port++)
        {
            if (ctrl_P->CMenu.port_id_list[((port-1)/8)] & (1 << ( 7 - ((port-1)%8))))
            {
                verify_ret = verify_ethernet(unit, port, &lport);
                if (verify_ret != CLI_API_ETH_OK &&
                    verify_ret != CLI_API_ETH_TRUNK_MEMBER)
                {
                    display_ethernet_msg(verify_ret, unit, port);
                    continue;
                }

                if (COS_TYPE_E_NONE != L4_PMGR_QOS_SetPortIngressPhb2Queue(
                                           lport, COS_TYPE_PRIORITY_USER, phb, queue))
                {

    #if (SYS_CPNT_EH == TRUE)
                    CLI_API_Show_Exception_Handeler_Msg();
    #else
        #if (CLI_SUPPORT_PORT_NAME == 1)
                    UI8_T name[MAXSIZE_ifName+1] = {0};

                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Failed to set QoS DSCP to queue on ethernet %s\r\n", name);
        #else
                    CLI_LIB_PrintStr_2("Failed to set QoS DSCP to queue on ethernet %lu/%lu\r\n", unit, port);
        #endif /* #if (CLI_SUPPORT_PORT_NAME == 1) */
    #endif /* #if (SYS_CPNT_EH == TRUE) */
                }
            }
        }
    }
#endif /* SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT */
    return CLI_NO_ERROR;
}

#endif /* (SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE == TRUE) */


UI32_T CLI_API_QOS_SetDscp2Color(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_COLOR == TRUE)
    int  phb,color,i;
    UI32_T verify_unit = ctrl_P->CMenu.unit_id;
    UI32_T verify_port,lport,num,arg_num=0,j;
    CLI_API_EthStatus_T verify_ret;
    BOOL_T is_restore;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_QOS_MAP_DEFAULTDROPPRECEDENCE:
                is_restore = FALSE;
            break;

        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W4_NO_QOS_MAP_DEFAULTDROPPRECEDENCE:
                is_restore = TRUE;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }
    if(!is_restore)
    {
        color = atoi(arg[0]);
    if(color < MIN_COLOR_VAL || color > MAX_COLOR_VAL)
        return CLI_ERR_INTERNAL;
        if (color == 2)
        {
            CLI_LIB_PrintStr("drop precedence value can't be set to 2\r\n");
            return CLI_NO_ERROR;
        }
    j = 2;
    }
    else
    {
    color = 4;
    j = 0;
    }
    while(arg[arg_num])
        arg_num++;
    for (num=j;num<arg_num;num++)
    {
        phb = atoi(arg[num]);
	    if(phb < MIN_PHB_VAL || phb > MAX_PHB_VAL)
	        return CLI_ERR_INTERNAL;

        for (i = 1 ; i <= ctrl_P->sys_info.max_port_number ; i++)
        {
            if (ctrl_P->CMenu.port_id_list[(UI32_T)((i-1)/8)]  & (1 << ( 7 - ((i-1)%8))) )
            {
                verify_port = i;

                verify_ret = verify_ethernet(verify_unit, verify_port, &lport);
                if (verify_ret != CLI_API_ETH_OK &&
                    verify_ret != CLI_API_ETH_TRUNK_MEMBER)
                {
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    continue;
                }

                if (!L4_PMGR_QOS_SetIngressDscp2Color(lport, phb, color))
                {
    #if (CLI_SUPPORT_PORT_NAME == 1)
                    {
                        UI8_T name[MAXSIZE_ifName+1] = {0};
                        CLI_LIB_Ifindex_To_Name(lport,name);
    #if (SYS_CPNT_EH == TRUE)
                        CLI_API_Show_Exception_Handeler_Msg();
    #else
                        CLI_LIB_PrintStr_1("Failed to set QoS DSCP to color on ethernet %s\r\n", name);
    #endif
                    }
    #else
    #if (SYS_CPNT_EH == TRUE)
                    CLI_API_Show_Exception_Handeler_Msg();
    #else
                    CLI_LIB_PrintStr_2("Failed to set QoS DSCP to color on ethernet %lu/%lu\r\n", verify_unit, verify_port);
    #endif
    #endif
                }
            }
        }
    }
#endif /* #if (SYS_CPNT_COS_ING_INTER_DSCP_TO_COLOR == TRUE) */
    return CLI_NO_ERROR;
}

#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_COLOR == TRUE)
static UI32_T Show_one_interface_cos_ingressdscp2color(UI32_T lport,UI32_T line_num)
{
    UI8_T  color[MAX_PHB_VAL + 1];

    if (L4_PMGR_QOS_GetIngressDscp2Color(lport,color))
    {
    CLI_LIB_PrintStr(" default-drop-precedence map:\r\n");
        CLI_LIB_PrintStr(" PHB:       0     1     2     3     4     5     6     7 \r\n");
        CLI_LIB_PrintStr(" ------------------------------------------------------- \r\n");
        CLI_LIB_PrintStr_4(" color:     %d     %d     %d     %d     ",color[0],color[1],color[2],color[3]);
        CLI_LIB_PrintStr_4("%d     %d     %d     %d\r\n",color[4],color[5],color[6],color[7]);
    }
    else
    {
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr_1("Failed to get CoS port info, lport = %lu\r\n", lport);
#endif
        line_num += 1;
    }

    return line_num;
}
#endif /* #if (SYS_CPNT_COS_ING_INTER_DSCP_TO_COLOR == TRUE) */

UI32_T CLI_API_QOS_ShowDscp2Color(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_COLOR == TRUE)
    UI32_T line_num = 0;

    if(arg[1]==NULL)
        return CLI_ERR_INTERNAL;

    switch(arg[0][0])
    {
        case 'e':
        case 'E':
        {
            UI32_T lport = 0;
            UI32_T verify_unit = atoi(arg[1]);
            UI32_T verify_port = atoi(strchr(arg[1], '/')+1 );
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
                if (!IF_PMGR_IfnameToIfindex(arg[1], &lport))
                {
                    CLI_LIB_PrintStr_1("%s does not exist\r\n",arg[1]);
                    return CLI_NO_ERROR;
                }
                SWCTRL_POM_LogicalPortToUserPort(lport, &verify_unit, &verify_port, &trunk_id);
            }
    #endif

            verify_ret = verify_ethernet(verify_unit, verify_port, &lport);

            switch(verify_ret)
            {
                case CLI_API_ETH_OK:
    #if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Information of %s\r\n", name);
                }
    #else
                    CLI_LIB_PrintStr_2("Information of Eth %lu/%lu\r\n", verify_unit, verify_port);
    #endif
                    line_num++;
                    Show_one_interface_cos_ingressdscp2color(lport, line_num);
                    break;

                case CLI_API_ETH_TRUNK_MEMBER:
    #if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Information of %s\r\n", name);
                }
    #else
                    CLI_LIB_PrintStr_2("Information of Eth %lu/%lu\r\n", verify_unit, verify_port);
    #endif
                    line_num++;
                    Show_one_interface_cos_ingressdscp2color(lport, line_num);
                    break;

                case CLI_API_ETH_NOT_PRESENT:
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    break;

                case CLI_API_ETH_UNKNOWN_PORT:
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    break;
            }
        }
            break;

        case 'p':
        case 'P':
        {
        #if 0
            UI32_T lport = 0;
            UI32_T verify_trunk_id = atoi(arg[1]);
            CLI_API_TrunkStatus_T verify_ret;

            if( (verify_ret = verify_trunk(verify_trunk_id, &lport)) != CLI_API_TRUNK_OK)
            {
                display_trunk_msg(verify_ret, verify_trunk_id);
                return CLI_NO_ERROR;
            }
            else
            {
                CLI_LIB_PrintStr_1("Information of Trunk %lu\r\n", verify_trunk_id);

                if((line_num = Show_one_interface_cos_ingressdscp2color(lport, line_num)) == JUMP_OUT_MORE)
                {
                    return CLI_NO_ERROR;
                }
                else if (line_num == EXIT_SESSION_MORE)
                {
                    return CLI_EXIT_SESSION;
                }

                break;
            }
        #endif
        }
            return CLI_ERR_CMD_NOT_IMPLEMENT;

        default:
            return CLI_ERR_INTERNAL;
    }
#endif /* #if (SYS_CPNT_COS_ING_INTER_DSCP_TO_COLOR == TRUE) */
    return CLI_NO_ERROR;
}

UI32_T CLI_API_QOS_SetEgressDscp2Cos(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_COS == TRUE)
    int  phb,color,cos,cfi,i;
    UI32_T verify_unit = ctrl_P->CMenu.unit_id;
    UI32_T verify_port,lport,num,arg_num = 0,j;
    CLI_API_EthStatus_T verify_ret;
    BOOL_T is_restore;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_QOS_MAP_DSCPCOS:
                is_restore = FALSE;
            break;

        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W4_NO_QOS_MAP_DSCPCOS:
                is_restore = TRUE;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }
    if(!is_restore)
    {
        cos = atoi(arg[0]);
        if(cos < MIN_COS_VAL || cos > MAX_COS_VAL)
            return CLI_ERR_INTERNAL;
        cfi = atoi(arg[1]);
        if(cfi < MIN_CFI_VAL || cfi > MAX_CFI_VAL)
            return CLI_ERR_INTERNAL;
        j = 3;
    }
    else
    {
        cos = 8;
        cfi = 8;
        j = 0;
    }
    while(arg[arg_num])
        arg_num++;
    for (num=j;num<arg_num;num=num+2)
    {
        phb = atoi(arg[num]);
    if(phb < MIN_PHB_VAL || phb > MAX_PHB_VAL)
        return CLI_ERR_INTERNAL;

        color = atoi(arg[num+1]);
    if(color < MIN_COLOR_VAL || color > MAX_COLOR_VAL)
        return CLI_ERR_INTERNAL;
        if (color == 2)
        {
            CLI_LIB_PrintStr("drop precedence value can't be set to 2\r\n");
            return CLI_NO_ERROR;
        }

        for (i = 1 ; i <= ctrl_P->sys_info.max_port_number ; i++)
        {
            if (ctrl_P->CMenu.port_id_list[(UI32_T)((i-1)/8)]  & (1 << ( 7 - ((i-1)%8))) )
            {
                verify_port = i;

                verify_ret = verify_ethernet(verify_unit, verify_port, &lport);
                if (verify_ret != CLI_API_ETH_OK &&
                    verify_ret != CLI_API_ETH_TRUNK_MEMBER)
                {
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    continue;
                }

                if (!L4_PMGR_QOS_SetIngressDscp2Cos(lport, phb, color,cos, cfi))
                {
    #if (CLI_SUPPORT_PORT_NAME == 1)
                    {
                        UI8_T name[MAXSIZE_ifName+1] = {0};
                        CLI_LIB_Ifindex_To_Name(lport,name);
    #if (SYS_CPNT_EH == TRUE)
                        CLI_API_Show_Exception_Handeler_Msg();
    #else
                        CLI_LIB_PrintStr_1("Failed to set QoS DSCP to CoS on ethernet %s\r\n", name);
    #endif
                    }
    #else
    #if (SYS_CPNT_EH == TRUE)
                    CLI_API_Show_Exception_Handeler_Msg();
    #else
                    CLI_LIB_PrintStr_2("Failed to set QoS DSCP to CoS on ethernet %lu/%lu\r\n", verify_unit, verify_port);
    #endif
    #endif
                }
            }
        }
    }
#endif /*#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_COLOR == TRUE) */
    return CLI_NO_ERROR;
}
static UI32_T Show_one_interface_cos_egressdscp2cos(UI32_T lport,UI32_T line_num)
{
    UI32_T cos,cfi,phb,color;

    for (phb=0; phb<=MAX_PHB_VAL; phb++)
    {
        CLI_LIB_PrintStr_1(" %lu  :                    ",phb);
        for (color=0;color<=MAX_COLOR_VAL;color++)
        {
            if (color == 2)
                continue;
            if (L4_PMGR_QOS_GetIngressDscp2Cos(lport,phb,color,&cos,&cfi))
            {
                 CLI_LIB_PrintStr_2(" (%lu,%lu)      ", cos,cfi);
            }
            else
            {
#if (SYS_CPNT_EH == TRUE)
                CLI_API_Show_Exception_Handeler_Msg();
#else
                CLI_LIB_PrintStr_1("Failed to get CoS port info, lport = %lu\r\n", lport);
#endif
                line_num += 1;
            }
            if (color == MAX_COLOR_VAL)
                CLI_LIB_PrintStr("\r\n");
        }
    }

    return line_num;
}
UI32_T CLI_API_QOS_ShowEgressDscp2Cos(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T line_num = 0;

    if(arg[1]==NULL)
        return CLI_ERR_INTERNAL;

    switch(arg[0][0])
    {
        case 'e':
        case 'E':
        {
            UI32_T lport = 0;
            UI32_T verify_unit = atoi(arg[1]);
            UI32_T verify_port = atoi( strchr(arg[1], '/')+1 );
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
                if (!IF_PMGR_IfnameToIfindex(arg[1], &lport))
                {
                    CLI_LIB_PrintStr_1("%s does not exist\r\n",arg[1]);
                    return CLI_NO_ERROR;
                }
                SWCTRL_POM_LogicalPortToUserPort(lport, &verify_unit, &verify_port, &trunk_id);
            }
    #endif

            verify_ret = verify_ethernet(verify_unit, verify_port, &lport);

            switch(verify_ret)
            {
                case CLI_API_ETH_OK:
    #if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Information of %s\r\n", name);
                }
    #else
                    CLI_LIB_PrintStr_2("Information of Eth %lu/%lu\r\n", verify_unit, verify_port);
    #endif
                    line_num++;
                    CLI_LIB_PrintStr(" DSCP-CoS map:\r\n");
                    CLI_LIB_PrintStr(" PHB:  drop precedence   0(green)   1(red)     3(yellow)\r\n");
                    CLI_LIB_PrintStr(" -------------------------------------------------------\r\n");
                    Show_one_interface_cos_egressdscp2cos(lport, line_num);
                    break;

                case CLI_API_ETH_TRUNK_MEMBER:
    #if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Information of %s\r\n", name);
                }
    #else
                    CLI_LIB_PrintStr_2("Information of Eth %lu/%lu\r\n", verify_unit, verify_port);
    #endif
                    line_num++;
                    CLI_LIB_PrintStr(" DSCP-CoS map:\r\n");
                    CLI_LIB_PrintStr(" PHB:  drop precedence   0(green)   1(red)     3(yellow)\r\n");
                    CLI_LIB_PrintStr(" -------------------------------------------------------\r\n");
                    Show_one_interface_cos_egressdscp2cos(lport, line_num);
                    break;

                case CLI_API_ETH_NOT_PRESENT:
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    break;

                case CLI_API_ETH_UNKNOWN_PORT:
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    break;
            }
        }
            break;

        case 'p':
        case 'P':
        {
        #if 0
            UI32_T lport = 0;
            UI32_T verify_trunk_id = atoi(arg[1]);
            CLI_API_TrunkStatus_T verify_ret;

            if( (verify_ret = verify_trunk(verify_trunk_id, &lport)) != CLI_API_TRUNK_OK)
            {
                display_trunk_msg(verify_ret, verify_trunk_id);
                return CLI_NO_ERROR;
            }
            else
            {
                CLI_LIB_PrintStr_1("Information of Trunk %lu\r\n", verify_trunk_id);

                CLI_LIB_PrintStr(" DSCP-CoS map:\r\n");
                CLI_LIB_PrintStr(" PHB:  drop precedence   0(green)   1(red)     3(yellow)\r\n");
                CLI_LIB_PrintStr(" -------------------------------------------------------\r\n");
                if((line_num = Show_one_interface_cos_egressdscp2cos(lport, line_num)) == JUMP_OUT_MORE)
                {
                    return CLI_NO_ERROR;
                }
                else if (line_num == EXIT_SESSION_MORE)
                {
                    return CLI_EXIT_SESSION;
                }

                break;
            }
        #endif
        }
            return CLI_ERR_CMD_NOT_IMPLEMENT;

        default:
            return CLI_ERR_INTERNAL;
    }

    return CLI_NO_ERROR;
}

#if (SYS_CPNT_COS_ING_IP_PORT_TO_INTER_DSCP == TRUE)
UI32_T CLI_API_QOS_SetTcpUdp2Dscp(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_IP_PORT_TO_INTER_DSCP_PER_PORT == TRUE)
    int  phb=0,color=0,i,port;
    UI32_T verify_unit = ctrl_P->CMenu.unit_id;
    UI32_T verify_port,lport;
    CLI_API_EthStatus_T verify_ret;
    int  protocol;
    BOOL_T is_restore;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_QOS_MAP_IPPORTDSCP:
                is_restore = FALSE;
            break;

        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W4_NO_QOS_MAP_IPPORTDSCP:
                is_restore = TRUE;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }

    if(arg[0][0]=='t' || arg[0][0]=='T')
        protocol = tcp_protocol;
    else
        protocol = udp_protocol;

    port = atoi(arg[1]);
    if(port < MIN_PORT_VAL || port > MAX_PORT_VAL)
        return CLI_ERR_INTERNAL;

    if(cmd_idx == PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_QOS_MAP_IPPORTDSCP)
    {
    phb = atoi(arg[3]);
    if(phb < MIN_PHB_VAL || phb > MAX_PHB_VAL)
        return CLI_ERR_INTERNAL;

    color = atoi(arg[4]);
    if(color < MIN_COLOR_VAL || color > MAX_COLOR_VAL)
        return CLI_ERR_INTERNAL;

    if (color == 2)
    {
        CLI_LIB_PrintStr("drop precedence value can't be set to 2\r\n");
        return CLI_NO_ERROR;
        }
    }

    for (i = 1 ; i <= ctrl_P->sys_info.max_port_number ; i++)
    {
        if (ctrl_P->CMenu.port_id_list[(UI32_T)((i-1)/8)]  & (1 << ( 7 - ((i-1)%8))) )
        {
            verify_port = i;

            verify_ret = verify_ethernet(verify_unit, verify_port, &lport);
            if (verify_ret != CLI_API_ETH_OK &&
                verify_ret != CLI_API_ETH_TRUNK_MEMBER)
            {
                display_ethernet_msg(verify_ret, verify_unit, verify_port);
                continue;
            }

            if (!L4_PMGR_QOS_SetTcpUdp2Dscp(lport,protocol,port,phb,color,is_restore))
            {
#if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
#if (SYS_CPNT_EH == TRUE)
                    CLI_API_Show_Exception_Handeler_Msg();
#else
                    CLI_LIB_PrintStr_1("Failed to set QoS IP-port-DSCP on ethernet %s\r\n", name);
#endif
                }
#else
#if (SYS_CPNT_EH == TRUE)
                CLI_API_Show_Exception_Handeler_Msg();
#else
                CLI_LIB_PrintStr_2("Failed to set QoS IP-port-DSCP on ethernet %lu/%lu\r\n", verify_unit, verify_port);
#endif
#endif
            }
        }
    }
#endif /* #if (SYS_CPNT_COS_ING_IP_PORT_TO_INTER_DSCP_PER_PORT == TRUE) */
    return CLI_NO_ERROR;
}

UI32_T CLI_API_QOS_SetTcpUdp2Dscp_Pch(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if 0 && (SYS_CPNT_COS_ING_IP_PORT_TO_INTER_DSCP_PER_PORT == TRUE)
    int  phb=0,color=0,port,protocol;
    BOOL_T is_restore;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W3_QOS_MAP_IPPORTDSCP:
                is_restore = FALSE;
            break;

        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W4_NO_QOS_MAP_IPPORTDSCP:
                is_restore = TRUE;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }

    if(arg[0][0]=='t' || arg[0][0]=='T')
        protocol = tcp_protocol;
    else
        protocol = udp_protocol;

    port = atoi(arg[1]);
    if(port < MIN_PORT_VAL || port > MAX_PORT_VAL)
        return CLI_ERR_INTERNAL;

    if(cmd_idx == PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W3_QOS_MAP_IPPORTDSCP)
    {
    phb = atoi(arg[3]);
    if(phb < MIN_PHB_VAL || phb > MAX_PHB_VAL)
        return CLI_ERR_INTERNAL;

    color = atoi(arg[4]);
    if(color < MIN_COLOR_VAL || color > MAX_COLOR_VAL)
        return CLI_ERR_INTERNAL;

    if (color == 2)
    {
        CLI_LIB_PrintStr("drop precedence value can't be set to 2\r\n");
        return CLI_NO_ERROR;
    }
    }

    if (!L4_PMGR_QOS_SetTcpUdp2Dscp(ctrl_P->CMenu.pchannel_id+SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER-1,protocol,port,phb,color,is_restore))
    {
#if (CLI_SUPPORT_PORT_NAME == 1)
        {
            UI8_T name[MAXSIZE_ifName+1] = {0};
            CLI_LIB_Ifindex_To_Name(lport,name);
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
#else
            CLI_LIB_PrintStr_1("Failed to set QoS IP-port-DSCP on trunk %s\r\n", name);
#endif
        }
#else
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr_1("Failed to set QoS IP-port-DSCP on trunk %lu\r\n", ctrl_P->CMenu.pchannel_id);
#endif
#endif

    }
#endif /*#if (SYS_CPNT_COS_ING_IP_PORT_TO_INTER_DSCP_PER_PORT == TRUE) */
    return CLI_NO_ERROR;

}

#if (SYS_CPNT_COS_ING_IP_PORT_TO_INTER_DSCP_PER_PORT == TRUE)
static UI32_T Show_one_interface_cos_TCPUDP2dscp(UI32_T lport,UI32_T line_num)
{
    UI32_T ret = 0;
    UI32_T protocol=0,port=0,phb=0,color=0,interface=0;

    for(;;)
    {
        ret = L4_PMGR_QOS_GetNextTCPUDP2Dscp(&interface,&protocol, &port ,&phb,&color);
        if(ret != TRUE)
        {
            break;
        }

        if(lport== interface)
        {
            if(protocol == tcp_protocol)
                {
                CLI_LIB_PrintStr_1(" (tcp,%-5lu)                       ",port);
                }
            else if (protocol == udp_protocol)
                {
                CLI_LIB_PrintStr_1(" (udp,%-5lu)                       ",port);
                }
            CLI_LIB_PrintStr_2("%-8lu%lu\r\n",phb,color);
        }
    }
    return line_num;
}
#endif /* #if (SYS_CPNT_COS_ING_IP_PORT_TO_INTER_DSCP_PER_PORT == TRUE) */

UI32_T CLI_API_QOS_ShowTcpUdp2Dscp(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_IP_PORT_TO_INTER_DSCP_PER_PORT == TRUE)
    UI32_T line_num = 0;

    if(arg[1]==NULL)
        return CLI_ERR_INTERNAL;

    switch(arg[0][0])
    {
        case 'e':
        case 'E':
        {
            UI32_T lport = 0;
            UI32_T verify_unit = atoi(arg[1]);
            UI32_T verify_port = atoi( strchr(arg[1], '/')+1 );
            CLI_API_EthStatus_T verify_ret;

    #if (CLI_SUPPORT_PORT_NAME == 1)
            if (isdigit(arg[1][0]))
            {
                verify_unit = atoi(arg[1]);
                verify_port = atoi)(strchr((char *arg[1],'/')+1);
            }
            else/*port name*/
            {
                UI32_T trunk_id = 0;
                if (!IF_PMGR_IfnameToIfindex(arg[1], &lport))
                {
                    CLI_LIB_PrintStr_1("%s does not exist\r\n",arg[1]);
                    return CLI_NO_ERROR;
                }
                SWCTRL_POM_LogicalPortToUserPort(lport, &verify_unit, &verify_port, &trunk_id);
            }
    #endif
            verify_ret = verify_ethernet(verify_unit, verify_port, &lport);
            switch(verify_ret)
            {
                case CLI_API_ETH_OK:
    #if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Information of %s\r\n", name);
                }
    #else
                    CLI_LIB_PrintStr_2("Information of Eth %lu/%lu\r\n", verify_unit, verify_port);
    #endif
                    line_num++;
                    CLI_LIB_PrintStr(" IP-port-DSCP map:\r\n");
                    CLI_LIB_PrintStr(" (IP protocol,destination port) :  PHB     drop precedence\r\n");
                    CLI_LIB_PrintStr(" -----------------------------------------------------------\r\n");
                    Show_one_interface_cos_TCPUDP2dscp(lport, line_num);
                    CLI_LIB_PrintStr("\r\n");
                    break;

                case CLI_API_ETH_TRUNK_MEMBER:
    #if (CLI_SUPPORT_PORT_NAME == 1)
                {
                    UI8_T name[MAXSIZE_ifName+1] = {0};
                    CLI_LIB_Ifindex_To_Name(lport,name);
                    CLI_LIB_PrintStr_1("Information of %s\r\n", name);
                }
    #else
                    CLI_LIB_PrintStr_2("Information of Eth %lu/%lu\r\n", verify_unit, verify_port);
    #endif
                    line_num++;
                    CLI_LIB_PrintStr(" IP-port-DSCP map:\r\n");
                    CLI_LIB_PrintStr(" (IP protocol,destination port) :  PHB     drop precedence\r\n");
                    CLI_LIB_PrintStr(" -----------------------------------------------------------\r\n");
                    Show_one_interface_cos_TCPUDP2dscp(lport, line_num);
                    CLI_LIB_PrintStr("\r\n");
                    break;

                case CLI_API_ETH_NOT_PRESENT:
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    break;

                case CLI_API_ETH_UNKNOWN_PORT:
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    break;
            }
        }
            break;

        case 'p':
        case 'P':
        {
        #if 0
            UI32_T lport = 0;
            UI32_T verify_trunk_id = atoi(arg[1]);
            CLI_API_TrunkStatus_T verify_ret;

            verify_ret = verify_trunk(verify_trunk_id, &lport);
            if((verify_ret == CLI_API_TRUNK_NOT_EXIST)||(verify_ret == CLI_API_TRUNK_INVALID_RANGE))
            {
                display_trunk_msg(verify_ret, verify_trunk_id);
                return CLI_NO_ERROR;
            }
            else
            {
                SWCTRL_POM_TrunkIDToLogicalPort(verify_trunk_id, &lport);

                CLI_LIB_PrintStr_1("Information of Trunk %lu\r\n", verify_trunk_id);

                CLI_LIB_PrintStr(" IP-port-DSCP map:\r\n");
                CLI_LIB_PrintStr(" (IP protocol,destination port) :  PHB     drop precedence\r\n");
                CLI_LIB_PrintStr(" -----------------------------------------------------------\r\n");
                if((line_num = Show_one_interface_cos_TCPUDP2dscp(lport, line_num)) == JUMP_OUT_MORE)
                {
                    return CLI_NO_ERROR;
                }
                else if (line_num == EXIT_SESSION_MORE)
                {
                    return CLI_EXIT_SESSION;
                }

                break;
            }
        #endif
        }
            return CLI_ERR_CMD_NOT_IMPLEMENT;

        default:
            return CLI_ERR_INTERNAL;
    }
#endif /* #if (SYS_CPNT_COS_ING_IP_PORT_TO_INTER_DSCP_PER_PORT == TRUE) */
   return CLI_NO_ERROR;
}
#endif /* SYS_CPNT_COS_ING_IP_PORT_TO_INTER_DSCP */

/*channel group*/

UI32_T CLI_API_QOS_SetQosIfMappingMode_Pch(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_TRUST_MODE_SUPPORT_COS || SYS_CPNT_COS_TRUST_MODE_SUPPORT_DSCP || SYS_CPNT_COS_TRUST_MODE_SUPPORT_PRECEDENCE)
#if 0 /*2011.07.08 do not support trunk trust mode setting for now if support in the future will open this*/
    UI32_T mapping_mode;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W3_QOS_MAP_TRUSTMODE:

            if(arg[0]!=NULL)
            {
                if(arg[0][0] == 'c' || arg[0][0] == 'C')
                    mapping_mode = COS_MAPPING_MODE;
                else if(arg[0][0] == 'd' || arg[0][0] == 'D')
                    mapping_mode = DSCP_IF_MAPPING_MODE;
        else
                    mapping_mode = PRECEDENCE_MAPPING_MODE;
            }
            else
                return CLI_ERR_INTERNAL;
            break;

        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W4_NO_QOS_MAP_TRUSTMODE:
            mapping_mode = COS_MAPPING_MODE; /*default*/
            break;

        default:
            return CLI_ERR_INTERNAL;
    }
    if (!L4_PMGR_QOS_SetQosIfMappingMode(ctrl_P->CMenu.pchannel_id+SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER-1, mapping_mode))
    {
#if (CLI_SUPPORT_PORT_NAME == 1)
        {
            UI8_T name[MAXSIZE_ifName+1] = {0};
            CLI_LIB_Ifindex_To_Name(lport,name);
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
#else
            CLI_LIB_PrintStr_1("Failed to set QoS trust mode on trunk %s\r\n", name);
#endif
        }
#else
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr_1("Failed to set QoS trust mode on trunk %lu\r\n", ctrl_P->CMenu.pchannel_id);
#endif
#endif
    }
#endif
#endif /* #if (SYS_CPNT_COS_TRUST_MODE_SUPPORT_COS || SYS_CPNT_COS_TRUST_MODE_SUPPORT_DSCP || SYS_CPNT_COS_TRUST_MODE_SUPPORT_PRECEDENCE) */
    return CLI_NO_ERROR;
}

UI32_T CLI_API_QOS_SetDscp2Queue_Pch(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT == TRUE)
#if 0 /*2011.07.20 do not support trunk trust mode setting for now if support in the future will open this*/
    int  queue,phb;
    UI32_T arg_num = 0,num,j;
    BOOL_T is_restore;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W3_QOS_MAP_PHBQUEUE:
                is_restore = FALSE;
            break;

        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W4_NO_QOS_MAP_PHBQUEUE:
                is_restore = TRUE;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }
    if(!is_restore)
    {
        queue = atoi(arg[0]);
    if(queue < MIN_QUEUE_VAL || queue > MAX_QUEUE_VAL)
        return CLI_ERR_INTERNAL;
    j = 2;
    }
    else
    {
        queue = 8;
    j = 0;
    }
    while(arg[arg_num])
        arg_num++;
    for (num=j;num<arg_num;num++)
    {
        phb = atoi(arg[num]);
    if(phb < MIN_PHB_VAL || phb > MAX_PHB_VAL)
        return CLI_ERR_INTERNAL;

        if (!L4_PMGR_QOS_SetIngressDscp2Queue(ctrl_P->CMenu.pchannel_id+SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER-1,phb,queue))
        {
#if (CLI_SUPPORT_PORT_NAME == 1)
            {
                UI8_T name[MAXSIZE_ifName+1] = {0};
                CLI_LIB_Ifindex_To_Name(lport,name);
#if (SYS_CPNT_EH == TRUE)
                CLI_API_Show_Exception_Handeler_Msg();
#else
                CLI_LIB_PrintStr_1("Failed to set QoS DSCP to queue on ethernet %s\r\n", name);
#endif
            }
#else
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
#else
            CLI_LIB_PrintStr_1("Failed to set QoS DSCP to queue on trunk %lu\r\n", ctrl_P->CMenu.pchannel_id);
#endif
#endif
        }
    }
#endif
#endif /* SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT */
    return CLI_NO_ERROR;
}


UI32_T CLI_API_QOS_SetDscp2Color_Pch(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if 0 && (SYS_CPNT_COS_ING_INTER_DSCP_TO_COLOR == TRUE)
    int  phb,color;
    UI32_T num,arg_num=0,j;
    BOOL_T is_restore;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W3_QOS_MAP_DEFAULTDROPPRECEDENCE:
                is_restore = FALSE;
            break;

        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W4_NO_QOS_MAP_DEFAULTDROPPRECEDENCE:
                is_restore = TRUE;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }
    if(!is_restore)
    {
        color = atoi(arg[0]);
    if(color < MIN_COLOR_VAL || color > MAX_COLOR_VAL)
        return CLI_ERR_INTERNAL;
        if (color == 2)
        {
            CLI_LIB_PrintStr("drop precedence value can't be set to 2\r\n");
            return CLI_NO_ERROR;
        }
    j = 2;
    }
    else
    {
    color = 4;
    j = 0;
    }
    while(arg[arg_num])
        arg_num++;
    for (num=j;num<arg_num;num++)
    {
        phb = atoi(arg[num]);
	    if(phb < MIN_PHB_VAL || phb > MAX_PHB_VAL)
	        return CLI_ERR_INTERNAL;

        if (!L4_PMGR_QOS_SetIngressDscp2Color(ctrl_P->CMenu.pchannel_id+SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER-1, phb, color))
        {
#if (CLI_SUPPORT_PORT_NAME == 1)
            {
                UI8_T name[MAXSIZE_ifName+1] = {0};
                CLI_LIB_Ifindex_To_Name(lport,name);
#if (SYS_CPNT_EH == TRUE)
                CLI_API_Show_Exception_Handeler_Msg();
#else
                CLI_LIB_PrintStr_1("Failed to set QoS DSCP to color on ethernet %s\r\n", name);
#endif
            }
#else
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
#else
            CLI_LIB_PrintStr_1("Failed to set QoS DSCP to color on trunk %lu\r\n", ctrl_P->CMenu.pchannel_id);
#endif
#endif

        }
    }
#endif /*#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_COLOR == TRUE) */
    return CLI_NO_ERROR;
}


UI32_T CLI_API_QOS_SetEgressDscp2Cos_Pch(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_COS == TRUE) && 0
    int  phb,color,cos,cfi;
    UI32_T num,arg_num = 0,j;
    BOOL_T is_restore;

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W3_QOS_MAP_DSCPCOS:
                is_restore = FALSE;
            break;

        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W4_NO_QOS_MAP_DSCPCOS:
                is_restore = TRUE;
            break;

        default:
            return CLI_ERR_INTERNAL;
    }
    if(!is_restore)
    {
        cos = atoi(arg[0]);
    if(cos < MIN_COS_VAL || cos > MAX_COS_VAL)
        return CLI_ERR_INTERNAL;
        cfi = atoi(arg[1]);
    if(cfi < MIN_CFI_VAL || cfi > MAX_CFI_VAL)
        return CLI_ERR_INTERNAL;
    j = 3;
    }
    else
    {
        cos = 8;
    cfi = 8;
    j = 0;
    }
    while(arg[arg_num])
        arg_num++;
    for (num=j;num<arg_num;num=num+2)
    {
        phb = atoi(arg[num]);
    if(phb < MIN_PHB_VAL || phb > MAX_PHB_VAL)
        return CLI_ERR_INTERNAL;
        color = atoi(arg[num+1]);
    if(color < MIN_COLOR_VAL || color > MAX_COLOR_VAL)
        return CLI_ERR_INTERNAL;
        if (color == 2)
        {
            CLI_LIB_PrintStr("drop precedence value can't be set to 2\r\n");
            return CLI_NO_ERROR;
        }

        if (!L4_PMGR_QOS_SetIngressDscp2Cos(ctrl_P->CMenu.pchannel_id+SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER-1, phb, color,cos, cfi))
        {
#if (CLI_SUPPORT_PORT_NAME == 1)
            {
                UI8_T name[MAXSIZE_ifName+1] = {0};
                CLI_LIB_Ifindex_To_Name(lport,name);
#if (SYS_CPNT_EH == TRUE)
                CLI_API_Show_Exception_Handeler_Msg();
#else
                CLI_LIB_PrintStr_1("Failed to set QoS DSCP to CoS on ethernet %s\r\n", name);
#endif
            }
#else
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
#else
            CLI_LIB_PrintStr_1("Failed to set QoS DSCP to CoS on trunk %lu\r\n", ctrl_P->CMenu.pchannel_id);
#endif
#endif
        }
    }
#endif /*#if (SYS_CPNT_COS_ING_INTER_DSCP_TO_COS == TRUE) */
    return CLI_NO_ERROR;
}

#endif /* SYS_CPNT_COS */
