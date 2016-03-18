#ifdef RUN_UNIT_TEST
/*-------------------------------------------------------------------------
 * FUNCTION NAME:  L4_Backdoor_UnitTest
 *-------------------------------------------------------------------------
 * PURPOSE  : unit test
 * INPUT    : none
 * OUTPUT   : none.
 * RETURN   : none
 * NOTES    : none.
 *-------------------------------------------------------------------------
 */
static void L4_Backdoor_UnitTest()
{
    /* Test function L4_Backdoor_StrCmpIgnoreCase2
     */
    {
        UI32_T len;

        len = L4_Backdoor_StrCmpIgnoreCase2("", "");
        if (!(len==0))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        len = L4_Backdoor_StrCmpIgnoreCase2("", "this is a book");
        if (!(len==0))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        len = L4_Backdoor_StrCmpIgnoreCase2(NULL, "this is a book");
        if (!(len==0))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        len = L4_Backdoor_StrCmpIgnoreCase2("t", "this is a book");
        if (!(len==1))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        len = L4_Backdoor_StrCmpIgnoreCase2("th", "this is a book");
        if (!(len==2))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        len = L4_Backdoor_StrCmpIgnoreCase2("TH", "this is a book");
        if (!(len==2))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        len = L4_Backdoor_StrCmpIgnoreCase2("tHis Is", "this is a book");
        if (!(len==7))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        len = L4_Backdoor_StrCmpIgnoreCase2("that", "this is a book");
        if (!(len==2))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        len = L4_Backdoor_StrCmpIgnoreCase2("this is a pen", "this is a book");
        if (!(len==10))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        len = L4_Backdoor_StrCmpIgnoreCase2("is", "this is a book");
        if (!(len==0))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        len = L4_Backdoor_StrCmpIgnoreCase2("book", "this is a book");
        if (!(len==0))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        len = L4_Backdoor_StrCmpIgnoreCase2("this?", "this is a book");
        if (!(len==4))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        len = L4_Backdoor_StrCmpIgnoreCase2("this*", "this is a book");
        if (!(len==4))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }
    }

    /* Test function L4_Backdoor_StrStrIgnoreCase
     */
    {
        char *p;

        p = L4_Backdoor_StrStrIgnoreCase("this is a book", "this");
        if (!(p))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }
        
        p = L4_Backdoor_StrStrIgnoreCase("this is a book", "th");
        if (!(p))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        p = L4_Backdoor_StrStrIgnoreCase("this is a book", "ht");
        if (!(!p))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        p = L4_Backdoor_StrStrIgnoreCase("this is a book", "th?");
        if (!( !p ))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        p = L4_Backdoor_StrStrIgnoreCase("this is a book", "th*");
        if (!( !p ))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }
    }

    /* Test function L4_Backdoor_GetNextConformString
     */
    {
        const char *strings[] =
        {
            "authentication",
            "auto-traffic-control",
            "ipv6",
            "ip",
            "ipv7",
        };

        L4_BACKDOOR_GetNextConformString_T cs = {0};
        UI32_T str_index;
        BOOL_T result;

        /*
         * L4_Backdoor_GetNextConformString
         */

        /* case 1
         *
         * L4_Backdoor_GetNextConformString will return all commands
         * with prefix "ip"
         *
         */
        cs.name = "ip";
        cs.conform = NULL;

        L4_Backdoor_GetNextConformString(&cs, strings, _countof(strings));
        if (! (cs.conform && 0==strcmp("ipv6", cs.conform) && 2 == cs.last_index) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextConformString(&cs, strings, _countof(strings));
        if (! (cs.conform && 0==strcmp("ip", cs.conform) && 3 == cs.last_index) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextConformString(&cs, strings, _countof(strings));
        if (! (cs.conform && 0==strcmp("ipv7", cs.conform) && 4 == cs.last_index) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextConformString(&cs, strings, _countof(strings));
        if (! (!cs.conform))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 2
         *
         * L4_Backdoor_GetNextCommand will return all strings
         * with prefix "ipv"
         *
         */
        cs.name = "ipv";
        cs.conform = NULL;

        L4_Backdoor_GetNextConformString(&cs, strings, _countof(strings));
        if (! (cs.conform && 0==strcmp("ipv6", cs.conform) && 2 == cs.last_index) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextConformString(&cs, strings, _countof(strings));
        if (! (cs.conform && 0==strcmp("ipv7", cs.conform) && 4 == cs.last_index) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextConformString(&cs, strings, _countof(strings));
        if (! (!cs.conform))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 3
         *
         * L4_Backdoor_GetNextCommand will return all strings
         * with prefix "ipv7"
         *
         */
        cs.name = "ipv7";
        cs.conform = NULL;

        L4_Backdoor_GetNextConformString(&cs, strings, _countof(strings));
        if (! (cs.conform && 0==strcmp("ipv7", cs.conform) && 4 == cs.last_index) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextConformString(&cs, strings, _countof(strings));
        if (! (!cs.conform))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /*
         * L4_Backdoor_GetBestConformString
         */

        /* case 1
         *
         * L4_Backdoor_GetBestConformString will return FALSE, because have more strings matched
         *
         */
        result = L4_Backdoor_GetBestConformString("aut", strings, _countof(strings), &str_index);
        if (! (FALSE == result) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 2
         *
         * L4_Backdoor_GetBestConformString will return "authentication" command
         *
         */
        result = L4_Backdoor_GetBestConformString("auth", strings, _countof(strings), &str_index);
        if (! (TRUE == result && 0==strcmp("authentication", strings[str_index])) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 3
         *
         * L4_Backdoor_GetBestConformString will return "ip" command, because input command name
         * match all characters with command "ip" even it also is a prefix of "ipv6" and
         * "ipv7".
         *
         */
        result = L4_Backdoor_GetBestConformString("ip", strings, _countof(strings), &str_index);
        if (! (0==strcmp("ip", strings[str_index])) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 4
         *
         * L4_Backdoor_GetBestConformString will return NULL, because "ipv" are the prefix
         * of strings "ipv6" and "ipv7"
         *
         */
        result = L4_Backdoor_GetBestConformString("ipv", strings, _countof(strings), &str_index);
        if (! (FALSE==result) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 5
         *
         * L4_Backdoor_GetBestConformString will return "ipv7"
         *
         */
        result = L4_Backdoor_GetBestConformString("ipv7", strings, _countof(strings), &str_index);
        if (! (0==strcmp("ipv7", strings[str_index])) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 6
         *
         * L4_Backdoor_GetBestConformString will return NULL, because no found a "ipp" command
         *
         */
        result = L4_Backdoor_GetBestConformString("ipp", strings, _countof(strings), &str_index);
        if (! (FALSE==result) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 7
         *
         * L4_Backdoor_GetBestConformString will return NULL, because '?' is illegal character
         *
         */
        result = L4_Backdoor_GetBestConformString("ip?", strings, _countof(strings), &str_index);
        if (! (FALSE==result) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

    }

    /* Test function L4_Backdoor_GetNextCommand and
     * L4_Backdoor_FindCommand
     */
    {
        L4_BACKDOOR_Command_T commands[] =
        {
            {
                "authentication",
                "authentication",
                NULL,
                0,
                {
                    {
                        PARAM_COUNT,
                        0
                    },
                }
            },
            {
                "auto-traffic-control",
                "auto-traffic-control",
                NULL,
                0,
                {
                    {
                        PARAM_COUNT,
                        0
                    },
                }
            },
            {
                "ipv6",
                "ipv6",
                NULL,
                0,
                {
                    {
                        PARAM_COUNT,
                        0
                    },
                }
            },
            {
                "ip",
                "ip",
                NULL,
                0,
                {
                    {
                        PARAM_COUNT,
                        0
                    },
                }
            },
            {
                "ipv7",
                "ipv7",
                NULL,
                0,
                {
                    {
                        PARAM_COUNT,
                        0
                    },
                }
            },
        };
        L4_Backdoor_GetNextCommand_T nc = {0};
        L4_BACKDOOR_Command_T        *command_p = NULL;

        /*
         * L4_Backdoor_GetNextCommand
         */

        /* case 1
         *
         * L4_Backdoor_GetNextCommand will return all commands
         * with prefix "ip"
         *
         */
        nc.name = "ip";

        L4_Backdoor_GetNextCommand(&nc, commands, _countof(commands));
        if (! (0==strcmp("ipv6", nc.command->name)) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextCommand(&nc, commands, _countof(commands));
        if (! (0==strcmp("ip", nc.command->name)) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextCommand(&nc, commands, _countof(commands));
        if (! (0==strcmp("ipv7", nc.command->name)) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextCommand(&nc, commands, _countof(commands));
        if (! (!nc.command))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 2
         *
         * L4_Backdoor_GetNextCommand will return all commands
         * with prefix "ipv"
         *
         */
        nc.name = "ipv";

        L4_Backdoor_GetNextCommand(&nc, commands, _countof(commands));
        if (! (0==strcmp("ipv6", nc.command->name)) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextCommand(&nc, commands, _countof(commands));
        if (! (0==strcmp("ipv7", nc.command->name)) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextCommand(&nc, commands, _countof(commands));
        if (! (!nc.command))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 3
         *
         * L4_Backdoor_GetNextCommand will return all commands
         * with prefix "ipv7"
         *
         */
        nc.name = "ipv7";

        L4_Backdoor_GetNextCommand(&nc, commands, _countof(commands));
        if (! (0==strcmp("ipv7", nc.command->name)) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextCommand(&nc, commands, _countof(commands));
        if (! (!nc.command))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 4
         * L4_Backdoor_GetNextCommand will return all commands if input "?"
         */
        nc.name = "?";
        L4_Backdoor_GetNextCommand(&nc, commands, _countof(commands));
        if (! (0==strcmp("authentication", nc.command->name)) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextCommand(&nc, commands, _countof(commands));
        if (! (0==strcmp("auto-traffic-control", nc.command->name)) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextCommand(&nc, commands, _countof(commands));
        if (! (0==strcmp("ipv6", nc.command->name)) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextCommand(&nc, commands, _countof(commands));
        if (! (0==strcmp("ip", nc.command->name)) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextCommand(&nc, commands, _countof(commands));
        if (! (0==strcmp("ipv7", nc.command->name)) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_Backdoor_GetNextCommand(&nc, commands, _countof(commands));
        if (! (!nc.command))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 4
         * L4_Backdoor_GetNextCommand will return nothing if input ""
         */
        nc.name = "";
        L4_Backdoor_GetNextCommand(&nc, commands, _countof(commands));
        if (! (!nc.command))
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /*
         * L4_Backdoor_FindCommand
         */

        /* case 1
         *
         * L4_Backdoor_FindCommand will return NULL, because have more commands matched
         *
         */
        command_p = L4_Backdoor_FindCommand("aut", commands, _countof(commands));
        if (! (NULL==command_p) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 2
         *
         * L4_Backdoor_FindCommand will return "authentication" command
         *
         */
        command_p = L4_Backdoor_FindCommand("auth", commands, _countof(commands));
        if (! (0==strcmp("authentication", command_p->name)) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 3
         *
         * L4_Backdoor_FindCommand will return "ip" command, because input command name
         * match all characters with command "ip" even it also is a prefix of "ipv6" and
         * "ipv7".
         *
         */
        command_p = L4_Backdoor_FindCommand("ip", commands, _countof(commands));
        if (! (0==strcmp("ip", command_p->name)) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 4
         *
         * L4_Backdoor_FindCommand will return NULL, because "ipv" are the prefix
         * of commands "ipv6" and "ipv7"
         *
         */
        command_p = L4_Backdoor_FindCommand("ipv", commands, _countof(commands));
        if (! (NULL==command_p) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 5
         *
         * L4_Backdoor_FindCommand will return "ipv7"
         *
         */
        command_p = L4_Backdoor_FindCommand("ipv7", commands, _countof(commands));
        if (! (0==strcmp("ipv7", command_p->name)) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 6
         *
         * L4_Backdoor_FindCommand will return NULL, because no found a "ipp" command
         *
         */
        command_p = L4_Backdoor_FindCommand("ipp", commands, _countof(commands));
        if (! (NULL==command_p) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        /* case 7
         *
         * L4_Backdoor_FindCommand will return NULL, because '?' is illegal character
         *
         */
        command_p = L4_Backdoor_FindCommand("ip?", commands, _countof(commands));
        if (! (NULL==command_p) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }
    }

    /* Test function L4_BACKDOOR_GetDefaultArgumentString
     */
    {
        L4_BACKDOOR_DefaultArgument_T   def_arg[PARAM_COUNT] = {0};
        UI32_T i;

        for (i=0; i<PARAM_COUNT; i++)
        {
            L4_BACKDOOR_GetDefaultArgument(i, &def_arg[i]);
        }

        /*
        L4_BACKDOOR_GetDefaultArgumentString(PARAM_UNIT, def_arg[PARAM_UNIT].value, _countof(def_arg[PARAM_UNIT].value));
        if (! (0==strcmp(def_arg[PARAM_UNIT].value, "1")) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_BACKDOOR_GetDefaultArgumentString(PARAM_PORT, def_arg[PARAM_PORT].value, _countof(def_arg[PARAM_PORT].value));
        if (! (0==strcmp(var, "1")) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_BACKDOOR_GetDefaultArgumentString(PARAM_UNIT, def_arg[PARAM_UNIT].value, _countof(def_arg[PARAM_UNIT].value));
        if (! (0==strcmp(var, "00-00-00-00-0C-01")) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_BACKDOOR_GetDefaultArgumentString(PARAM_DA, var, _countof(var));
        if (! (0==strcmp(var, "00-01-00-00-0C-01")) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_BACKDOOR_GetDefaultArgumentString(PARAM_VID, var, _countof(var));
        if (! (0==strcmp(var, "1")) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_BACKDOOR_GetDefaultArgumentString(PARAM_ETHTYPE, var, _countof(var));
        if (! (0==strcmp(var, "0x0800")) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }

        L4_BACKDOOR_GetDefaultArgumentString(PARAM_SRC_IP, var, _countof(var));
        if (! (0==strcmp(var, "192.168.1.1")) )
        {
            BACKDOOR_MGR_Printf("%s:%d failed\n", __FUNCTION__, __LINE__);
        }
        */
    }
}
#endif /* RUN_UNIT_TEST */
