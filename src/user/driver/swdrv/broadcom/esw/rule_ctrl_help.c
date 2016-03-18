/**************************************************

 RULE CTRL HELP FUNCTIONS

 **************************************************/

#define EACH_UNIT(unit)                     for (unit=0;    unit   < SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;        unit++)
#define EACH_PORT(port)                     for (port=0;    port   < SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT;         port++)
#define EACH_DEVICE(device)                 for (device=0;  device < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT;         device++)
#define EACH_GROUP(group)                   for (group=0;   group  < SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP;     group++)
#define EACH_RULE(rule)                     for (rule=0;    rule   < SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR;  rule++)

#if (255 < SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK)
error ("!! SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK MUST LESS THAN 255")
#endif

#if (255 < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT)
error ("!! SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT MUST LESS THAN 255")
#endif

#if (255 < SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP)
error ("!! SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP MUST LESS THAN 255")
#endif

#if (255 < SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR)
error ("!! SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR MUST LESS THAN 255")
#endif

static UI32_T RULE_CTRL_HELP_MakeDevRuleID(
        UI32_T unit,
        UI32_T device,
        UI32_T group,
        UI32_T rule)
{
    UI32_T res = 0;

    res += unit   << 24;
    res += device << 16;
    res += group  << 8;
    res += rule;

    return res;
}

BOOL_T RULE_CTRL_HELP_DecomposeDevRuleID(
        UI32_T dev_rule_id,
        UI32_T *unit_p,
        UI32_T *device_p,
        UI32_T *group_p,
        UI32_T *rule_p)
{
    UI32_T      unit;
    UI32_T      device;
    UI32_T      group;
    UI32_T      rule;

    unit    = (dev_rule_id >> 24) & 0xFF;
    device  = (dev_rule_id >> 16) & 0xFF;
    group   = (dev_rule_id >> 8)  & 0xFF;
    rule    = (dev_rule_id)       & 0xFF;

    if (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK < unit)
    {
        return FALSE;
    }

    if (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT < device)
    {
        return FALSE;
    }

    if (SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP < group)
    {
        return FALSE;
    }

    if (SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR < rule)
    {
        return FALSE;
    }

    *unit_p     = unit;
    *device_p   = device;
    *group_p    = group;
    *rule_p     = rule;
    return TRUE;
}

#define RULE_CTRL_MAX_NBR_PORT_PER_DEVICE   12


BOOL_T DEV_SWDRV_PMGR_Logical2PhyDevicePortID(
    UI32_T unit_id,
    UI32_T port,
    UI32_T *module_id,
    UI32_T *device_id,
    UI32_T *phy_port
    )
{
    if ((unit_id < 1 || SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK < unit_id)
        || (port < 1 || SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT < port))
    {
        return FALSE;
    }

    *module_id = 0;
    *device_id = ((port-1) / SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP);
    *phy_port  = ((port-1) % SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP);
    return TRUE;
}

BOOL_T RULE_CTRL_HELP_UserPortToPhyPort(UI32_T unit, UI32_T port, UI32_T *phy_port)
{
    UI32_T module_id;
    UI32_T device_id;

    if (FALSE == DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, phy_port))
    {
        return FALSE;
    }

    return TRUE;
}

typedef struct RULE_CTRL_USERPORT_
{
    UI32_T  unit;
    UI32_T  port;
} RULE_CTRL_USERPORT;

bcm_pbmp_t RULE_CTRL_HELP_MakePortBitmap(RULE_CTRL_USERPORT *uplist, UI32_T count)
{
    RULE_CTRL_USERPORT  *up;
    UI32_T              phy_port;
    bcm_pbmp_t          port_pbmp = {0};

    up = uplist;
    while (count --)
    {
        if (FALSE == RULE_CTRL_HELP_UserPortToPhyPort(up->unit, up->port, &phy_port))
        {
            printf("RULE_CTRL_HELP_UserPortToPhyPort failed with"
                   "unit=%lu port=%lu\n", up->unit, up->port);
            break;
        }

        BCM_PBMP_PORT_ADD(port_pbmp, phy_port);

        up ++;
    }

    return port_pbmp;
}

typedef struct
{
    BOOL_T              is_used;

    /* index key
     */
    UI32_T              unit;
    UI32_T              device;
    UI32_T              group;
    UI32_T              rule;

    UI32_T              dev_rule_id;
    DEVRM_AceEntry_T    ace_entry;
    DEVRM_MeterEntry_T  meter_entry;
    DEVRM_ActionEntry_T action_entries[RULE_CTRL_ISC_MAX_NBR_OF_ACTION];
} RULE_CTRL_RuleEntry_T;

static RULE_CTRL_RuleEntry_T rule_ctrl_rule_pool[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK]
                                                [SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT]
                                                [SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP]
                                                [SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR];

static BOOL_T RULE_CTRL_Allocate_Rule(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T group_id,
    UI32_T fun_type,
    int rule_pri,
    UI32_T *dev_rule_id
    )
{
    UI32_T i;

    unit -= 1;  /* 1-based */
    port -= 1;  /* 1-based */

    if (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK <= unit
        || SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT <= device_id
        || SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP <= group_id)
    {
        return FALSE;
    }

    for (i=0; i<SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR; i++)
    {
        if (!rule_ctrl_rule_pool[unit][device_id][group_id][i].is_used)
        {
            *dev_rule_id = rule_ctrl_rule_pool[unit][device_id][group_id][i].dev_rule_id;

            if (0 == unit)
            {
                if (FALSE == _DEVRM_AllocateRule(device_id, group_id, fun_type, rule_pri, dev_rule_id))
                {
                    return FALSE;
                }
            }

            rule_ctrl_rule_pool[unit][device_id][group_id][i].is_used = TRUE;
            return TRUE;
        }
    }

    return FALSE;
}

static BOOL_T RULE_CTRL_Destroy_Rule(
    UI32_T  unit,
    UI32_T  port,
    UI32_T  device_id,
    UI32_T  group_id,
    UI32_T  fun_type,
    UI32_T  dev_rule_id
    )
{
    unit -= 1;  /* 1-based */
    port -= 1;  /* 1-based */

    if (unit == 0)
    {
        if (FALSE == _DEVRM_DestroyRule(device_id, group_id, fun_type, dev_rule_id))
        {
            return FALSE;
        }
    }

    dev_rule_id &= 255;

    if (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK <= unit
        || SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT <= device_id
        || SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP <= group_id
        || SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR <= dev_rule_id)
    {
        return FALSE;
    }

    rule_ctrl_rule_pool[unit][device_id][group_id][dev_rule_id].is_used = FALSE;
    return TRUE;
}

static BOOL_T RULE_CTRL_Set_Rule(
    UI32_T              unit,
    UI32_T              port,
    UI32_T              device_id,
    UI32_T              group_id,
    UI32_T              dev_rule_id,
    DEVRM_AceEntry_T    *ace_entry_p,
    DEVRM_MeterEntry_T  *meter_entry_p,
    DEVRM_ActionEntry_T *action_entry_p
    )
{
    RULE_CTRL_RuleEntry_T   *entry_p;

    unit -= 1;  /* 1-based */
    port -= 1;  /* 1-based */

    dev_rule_id &= 255;

    if (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK <= unit
        || SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT <= device_id
        || SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP <= group_id
        || SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR <= dev_rule_id)
    {
        return FALSE;
    }

    entry_p = &rule_ctrl_rule_pool[unit][device_id][group_id][dev_rule_id];

    if (!rule_ctrl_rule_pool[unit][device_id][group_id][dev_rule_id].is_used)
    {
        return FALSE;
    }

    memcpy(&entry_p->ace_entry, ace_entry_p, sizeof(entry_p->ace_entry));

    memset(&entry_p->meter_entry, 0, sizeof(entry_p->meter_entry));
    if (meter_entry_p)
    {
        memcpy(&entry_p->meter_entry, meter_entry_p, sizeof(entry_p->meter_entry));
    }

    memset(&entry_p->action_entries, 0, sizeof(entry_p->action_entries));
    if (action_entry_p)
    {
        UI32_T i = 0;
        DEVRM_ActionEntry_T *p = &action_entry_p[i];
        while (p)
        {
            if (RULE_CTRL_ISC_MAX_NBR_OF_ACTION < i)
                return FALSE;

            memcpy(&entry_p->action_entries[i], p, sizeof(entry_p->action_entries[0]));
            i++;
            p = p->next_action;
        }
    }

    return TRUE;
}

static BOOL_T RULE_CTRL_GetDevInfo(
    UI32_T unit,
    RULE_CTRL_DevInfo_T *dev_info_p
    )
{
    memset(dev_info_p, 0, sizeof(*dev_info_p));

    dev_info_p->num_of_chips = DEVRM_PMGR_GetNumberOfChips();
    return TRUE;
}

static void RULE_CTRL_HELP_CleanRulePool()
{
    RULE_CTRL_RuleEntry_T   *re;
    UI32_T                  unit;
    UI32_T                  device;
    UI32_T                  group;
    UI32_T                  rule;

    EACH_UNIT(unit)
    {
        EACH_DEVICE(device)
        {
            EACH_GROUP(group)
            {
                EACH_RULE(rule)
                {
                    re = &rule_ctrl_rule_pool[unit][device][group][rule];

                    memset(re, 0, sizeof(*re));
                    re->is_used     = FALSE;
                    re->unit        = unit;
                    re->device      = device;
                    re->group       = group;
                    re->rule        = rule;
                    re->dev_rule_id =
                        RULE_CTRL_HELP_MakeDevRuleID(unit, device, group, rule);
                }
            }
        }
    }
}

static BOOL_T RULE_CTRL_HELP_GetRuleByDevRuleID(UI32_T dev_rule_id, RULE_CTRL_RuleEntry_T *re)
{
    UI32_T                  unit;
    UI32_T                  device;
    UI32_T                  group;
    UI32_T                  rule;

    if (FALSE == RULE_CTRL_HELP_DecomposeDevRuleID(dev_rule_id, &unit, &device, &group, &rule))
    {
        return FALSE;
    }

    if (!rule_ctrl_rule_pool[unit][device][group][rule].is_used)
    {
        return FALSE;
    }

    memcpy(re, &rule_ctrl_rule_pool[unit][device][group][rule], sizeof(*re));
    return TRUE;
}

static BOOL_T RULE_CTRL_HELP_GetNextRule(RULE_CTRL_RuleEntry_T *re)
{
    UI32_T                  unit;
    UI32_T                  device;
    UI32_T                  group;
    UI32_T                  rule;

    if (re->dev_rule_id == 0XFFFFFFFF)
    {
        EACH_UNIT(unit)
        {
            EACH_DEVICE(device)
            {
                EACH_GROUP(group)
                {
                    EACH_RULE(rule)
                    {
#if 0
                        if (FALSE ==RULE_CTRL_HELP_GetRuleByDevRuleID(
                                        RULE_CTRL_HELP_MakeDevRuleID(unit, device, group, rule),
                                        re))
                            continue;

                        return TRUE;
#else

                        if (!rule_ctrl_rule_pool[unit][device][group][rule].is_used)
                            continue;

                        memcpy(re, &rule_ctrl_rule_pool[unit][device][group][rule], sizeof(*re));
                        return TRUE;
#endif
                    }
                }
            }
        }
    }
    else
    {
        BOOL_T find_next = FALSE;

        EACH_UNIT(unit)
        {
            EACH_DEVICE(device)
            {
                EACH_GROUP(group)
                {
                    EACH_RULE(rule)
                    {
                        if (!rule_ctrl_rule_pool[unit][device][group][rule].is_used)
                            continue;

                        if (find_next == TRUE)
                        {
                            memcpy(re, &rule_ctrl_rule_pool[unit][device][group][rule], sizeof(*re));
                            return TRUE;
                        }

                        if (re->dev_rule_id == rule_ctrl_rule_pool[unit][device][group][rule].dev_rule_id)
                        {
                            find_next = TRUE;
                            continue;
                        }
                    }
                }
            }
        }
    }

    return FALSE;
}

static BOOL_T RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_FunctionType_T fun_type, RULE_CTRL_RuleEntry_T *re)
{
    UI32_T  group_id;
    int     rule_pri;

    RULE_CTRL_GetFunctionInfoByFunctionType(fun_type, &group_id, &rule_pri);

    while (TRUE == RULE_CTRL_HELP_GetNextRule(re))
    {
        if (re->group == group_id)
        {
            return TRUE;
        }
    }

    return FALSE;
}

static void RULE_CTRL_HELP_DumpRule()
{
    RULE_CTRL_RuleEntry_T   re;

    printf("RuleID Unit Device Group Rule InportBmp\n");

    re.dev_rule_id = 0XFFFFFFFF;
    while (TRUE == RULE_CTRL_HELP_GetNextRule(&re))
    {
        printf("%6lu %4d %6d %5d %4d %08lx %08lx",
            re.dev_rule_id,
            re.unit,
            re.device,
            re.group,
            re.rule,
            re.ace_entry.inports_data.pbits[0],
            re.ace_entry.inports_data.pbits[1]);

        printf("\n");
    }
}

static void RULE_CTRL_HELP_DumpRuleWithFunType(RULE_TYPE_FunctionType_T fun_type)
{
    RULE_CTRL_RuleEntry_T   re;

    printf("RuleID Unit Device Group Rule InportBmp\n");

    re.dev_rule_id = 0XFFFFFFFF;
    while (TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type, &re))
    {
        printf("%6lu %4d %6d %5d %4d %08lx %08lx",
            re.dev_rule_id,
            re.unit,
            re.device,
            re.group,
            re.rule,
            re.ace_entry.inports_data.pbits[0],
            re.ace_entry.inports_data.pbits[1]);

        printf("\n");
    }
}

static UI32_T RULE_CTRL_HELP_CountOfRule()
{
    UI32_T count = 0;
    RULE_CTRL_RuleEntry_T   re;

    re.dev_rule_id = 0xFFFFFFFF;
    while (TRUE == RULE_CTRL_HELP_GetNextRule(&re))
    {
        count ++;
    }

    return count;
}

