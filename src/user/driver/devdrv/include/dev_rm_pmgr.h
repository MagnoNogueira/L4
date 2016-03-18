#ifndef _DEV_RM_PMGR_H_
#define _DEV_RM_PMGR_H_

#include "sys_type.h"

#ifdef MARVELL_CPSS
    #include <marvell/dev_rm.h>
#elif defined (BCM_ROBO_SUPPORT)
    #include "robo/dev_rm.h"
#elif defined REALTEK
    #include "realtek/dev_rm.h"
#else
    #include "esw/dev_rm.h"
#endif

enum
{
    DEVRM_PMGR_RESP_RESULT_FAIL,
};
#define DEVRM_PMGR_REQ_CMD_SIZE       sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->type)
#define DEVRM_PMGR_RESP_RESULT_SIZE   sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->type)

typedef struct
{
    union
    {
        UI32_T cmd;          /*cmd fnction id*/
        BOOL_T result_bool;  /*respond bool return*/
        UI32_T result_ui32;  /*respond ui32 return*/
        UI32_T result_i32;  /*respond i32 return*/
        UI64_T result_ui64;
    }type;

    union
    {
#ifndef MARVELL_CPSS
        union
        {
            struct
            {
                UI32_T device_id;
                DEVRM_GroupEntry_T group_entry;
            }req;
            struct
            {
                DEVRM_GroupEntry_T group_entry;
            }resp;
        }setgroup;

        union
        {
            struct
            {
                UI32_T device_id;
                UI32_T group_id;
                int priority;
            }req;
            struct
            {
            }resp;
        }set_group_priority;
#endif
        union
        {
            struct
            {
                UI32_T device_id;
                UI32_T group_id;
            }req;
            struct
            {
            }resp;
        }destroygroup;

        union
        {
            struct
            {
                UI32_T group_id;
                UI32_T vir_group_id;
                int    priority;
            }req;
            struct
            {
            }resp;
        }create_vir_group;

#ifdef MARVELL_CPSS
        union
        {
            struct
            {
                UI32_T device_id;
                UI32_T pcl_id;
                UI32_T vir_group_id;
                int    rule_pri;
                UI32_T dev_rule_id;
            }req;
            struct
            {
            }resp;
        }allocaterule;
#else
        union
        {
            struct
            {
                UI32_T device_id;
                UI32_T group_id;
                UI32_T vir_group_id;
                int    rule_pri;
            }req;
            struct
            {
                UI32_T dev_rule_id;
            }resp;
        }allocaterule;
#endif /* MARVELL_CPSS */

#if defined (MARVELL_CPSS) || defined (REALTEK)
        union
        {
            struct
            {
                UI32_T  device_id;
                UI32_T  group_id;
                UI32_T  dev_rule_id;
                DEVRM_AceEntry_T ace_entry;
                DEVRM_ACTION_T   action;
            }  req;
            struct
            {
            }resp;
        }setrule;
#else
        union
        {
            struct
            {
                UI32_T  device_id;
                UI32_T  group_id;
                UI32_T  dev_rule_id;
                UI32_T  action_entry_num;
                UI32_T  meter_entry_num;
                BOOL_T  counter_enable;
                DEVRM_AceEntry_T    ace_entry;
                DEVRM_MeterEntry_T  meter_entry;
                DEVRM_ActionEntry_T action_entry[32];
            }  req;
            struct
            {
            }resp;
        }setrule;
#endif /* MARVELL_CPSS */

#ifdef MARVELL_CPSS
#elif defined (REALTEK)
        union
        {
            struct
            {
                UI32_T  device_id;
                UI32_T  group_id;
                UI32_T  dev_rule_id;
                UI32_T  meter_entry_num;
                UI32_T  action_entry_num;
                DEVRM_AceEntry_T ace_entry;
                DEVRM_ACTION_T action;
            } req;

            struct
            {
            }resp;
        } updaterule;
#else
        union
        {
            struct
            {
                UI32_T  device_id;
                UI32_T  group_id;
                UI32_T  dev_rule_id;
                UI32_T  meter_entry_num;
                UI32_T  action_entry_num;
                DEVRM_AceEntry_T ace_entry;
                DEVRM_MeterEntry_T meter_entry;
                DEVRM_ActionEntry_T action_entry[32];
            } req;
            struct
            {
            }resp;
        }updaterule;
#endif /* MARVELL_CPSS */

        union
        {
            struct
            {
                UI32_T device_id;
                UI32_T vir_group_id;
                UI32_T dev_rule_id;
#ifdef MARVELL_CPSS
                UI32_T pcl_id;
#endif
            }req;
            struct
            {
            }resp;
        }destroyrule;

#ifdef MARVELL_CPSS
#elif defined REALTEK
        union
        {
            struct
            {
                DEVRM_POLICER_METER_T meter_entry;
                UI32_T device_id;
                UI32_T dev_rule_id;
                BOOL_T need_reinstalled;
            } req;
            struct
            {
            }resp;
        } create_meter;
#else
        union
        {
            struct
            {
                DEVRM_MeterEntry_T meter_entry;
                UI32_T device_id;
                UI32_T dev_rule_id;
                BOOL_T need_reinstalled;
            } req;
            struct
            {
            }resp;
        } create_meter;
#endif /* MARVELL_CPSS */

#ifdef MARVELL_CPSS
        union
        {
            struct
            {
                UI32_T  device_id;
                UI32_T  pcl_id;
                UI32_T  dev_rule_id;
                UI32_T  meter_entry_num;
                DEVRM_POLICER_METER_T dev_meter;
            }req;
            struct
            {
            }resp;
        } update_meter;
#elif defined (REALTEK)
        union
        {
            struct
            {
                UI32_T  device_id;
                UI32_T  dev_rule_id;
                UI32_T  meter_entry_num;
                DEVRM_POLICER_METER_T meter_entry;
            }req;
            struct
            {
            }resp;
        } update_meter;
#else
        union
        {
            struct
            {
                UI32_T  device_id;
                UI32_T  dev_rule_id;
                UI32_T  meter_entry_num;
                DEVRM_MeterEntry_T meter_entry;
            }req;
            struct
            {
            }resp;
        } update_meter;
#endif /* MARVELL_CPSS */

        union
        {
            struct
            {
                UI32_T device_id;
                UI32_T src_rule_id;
                UI32_T dst_rule_id;
            }req;
            struct
            {
            }resp;
        } share_meter;

        union
        {
            struct
            {
                UI32_T device_id;
                UI32_T dev_rule_id;
            }req;
            struct
            {
            }resp;
        }destroymeter;

#ifdef MARVELL_CPSS
#elif  defined (REALTEK)
        union
        {
            struct
            {
                UI32_T device_id;
                UI32_T dev_rule_id;
                UI32_T action_entry_num;
                DEVRM_ACTION_T action;
            }req;
            struct
            {
            }resp;
        } setaction;
#else
        union
        {
            struct
            {
                UI32_T device_id;
                UI32_T dev_rule_id;
                UI32_T action_entry_num;
                DEVRM_ActionEntry_T action_entry[5];
            }req;
            struct
            {
            }resp;
        }setaction;
#endif /* MARVELL_CPSS */

#ifdef MARVELL_CPSS
        union
        {
            struct
            {
                UI32_T device_id;
                UI32_T pcl_id;
                UI32_T dev_rule_id;
            }req;

            struct
            {
            }resp;
        } destroy_action;
#else
        union
        {
            struct
            {
                UI32_T device_id;
                UI32_T dev_rule_id;
            }req;

            struct
            {
            }resp;
        } destroy_action;
#endif /* MARVELL_CPSS */

        union
        {
            struct
            {
            }req;
            struct
            {
            }resp;
        }getnumberofchips;
        union
        {
            struct
            {
                UI32_T device_id;
            }req;
            struct
            {
            }resp;
        }checksoc;
        union
        {
            struct
            {

            }req;
            struct
            {
            }resp;
        }fieldinit;

#ifdef MARVELL_CPSS
        union
        {
            struct
            {
                DEVRM_PCL_LOOKUP_NUMBER_T lookup_num;
                UI32_T device_id;
                UI32_T pcl_id;
                UI32_T rule_type;
                BOOL_T is_ingress;
            }req;
            struct
            {
            }resp;
        } create_pcl;
#endif /* MARVELL_CPSS */

        union
        {
            struct
            {
                UI32_T  device_id;
                UI32_T  phy_port;
                UI32_T  pcl_id;
            }req;

            struct
            {
            }resp;
        } bind_pcl_to_port;

        union
        {
            struct
            {
                UI32_T  device_id;
                UI32_T  trunk_id;
                UI32_T  pcl_id;
            }req;

            struct
            {
            }resp;
        } bind_pcl_to_trunk_port;

        union
        {
            struct
            {
                UI32_T    aclId;
            }req;
            struct
            {
            }resp;
        }destroyacl;

#ifdef MARVELL_CPSS
        union
        {
            struct
            {
                UI32_T                  device_id;
                UI32_T                  pcl_id;
                UI32_T                  dev_rule_id;
            }req;

            struct
            {
                UI32_T                  green_octets;
                UI32_T                  yellow_octets;
                UI32_T                  red_octets;
            }resp;
        }get_counter;
#else
        union
        {
            struct
            {
                UI32_T                  device_id;
                UI32_T                  dev_rule_id;
            }req;

            struct
            {
                UI32_T                  counter;
            }resp;
        }get_counter;
#endif /* MARVELL_CPSS */

#ifdef MARVELL_CPSS
        union
        {
            struct
            {
                UI32_T                  device_id;
                UI32_T                  pcl_id;
                UI32_T                  dev_rule_id;
            }req;

            struct
            {
            }resp;
        } clear_counter;
#else
        union
        {
            struct
            {
                UI32_T                  device_id;
                UI32_T                  dev_rule_id;
            }req;

            struct
            {
            }resp;
        } clear_counter;
#endif /* MARVELL_CPSS */

        union
        {
            struct
            {
            }req;

            struct
            {
                DEVRM_DevInfo_T         device_info;
            }resp;
        }get_device_info;
    }data;
} DEVRM_PMGR_IPCMSG_T;

enum
{
    DEVRM_IPCCMD_CREATEGROUP,
    DEVRM_IPCCMD_SET_GROUP_QUALIFY_SET,
    DEVRM_IPCCMD_SET_GROUP_PRIORITY,
    DEVRM_IPCCMD_DESTROYGROUP,
    DEVRM_IPCCMD_CREATE_VIR_GROUP,
    DEVRM_IPCCMD_ALLOCATERULE,
    DEVRM_IPCCMD_SETRULE,
    DEVRM_IPCCMD_UPDATERULE,
    DEVRM_IPCCMD_DESTROYRULE,
    DEVRM_IPCCMD_CREATEMETER,
    DEVRM_IPCCMD_UPDATEMETER,
    DEVRM_IPCCMD_SHAREMETER,
    DEVRM_IPCCMD_DESTROYMETER,
    DEVRM_IPCCMD_SETACTION,
    DEVRM_IPCCMD_DESTROYACTION,
    DEVRM_IPCCMD_GETNUMBEROFCHIPS,
    DEVRM_IPCCMD_GET_DEVICE_INFOMATION,
    DEVRM_IPCCMD_CHECKSOC,
    DEVRM_IPCCMD_FIELDINIT,
    DEVRM_IPCCMD_CREATE_PCL,
    DEVRM_IPCCMD_BIND_PCL_TO_PORT,
    DEVRM_IPCCMD_UNBIND_PCL_TO_PORT,
    DEVRM_IPCCMD_BIND_PCL_TO_TRUNK_PORT,
    DEVRM_IPCCMD_UNBIND_PCL_TO_TRUNK_PORT,
    DEVRM_IPCCMD_GET_COUNTER,
    DEVRM_IPCCMD_CLEAR_COUNTER,
};

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_Initial
 *------------------------------------------------------------------------------
 * PURPOSE:  init ipc message queue
 * INPUT:    none
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
void DEVRM_PMGR_Init();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_FieldInit
 *------------------------------------------------------------------------------
 * FUNCTION: Initialize for clear the chip of the slave
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : None
 * NOTE    :
 *------------------------------------------------------------------------------
 */
void DEVRM_PMGR_FieldInit();

#ifndef MARVELL_CPSS
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_CreateGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Create a group entry.
 * INPUT:    device_id, group_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_CreateGroup(UI32_T device_id, DEVRM_GroupEntry_T *group_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_SetGroupQualifySet
 *------------------------------------------------------------------------------
 * PURPOSE:  Set qualify set of group
 * INPUT:    device_id, group_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_SetGroupQualifySet(UI32_T device_id, DEVRM_GroupEntry_T *group_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_SetGroupPriority
 *------------------------------------------------------------------------------
 * PURPOSE:  Set priority of group
 * INPUT:    device_id, group_id, priority
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_SetGroupPriority(UI32_T device_id, UI32_T group_id, int priority);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_DestroyGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a group entry.
 * INPUT:    device_id, group_index
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_DestroyGroup(UI32_T device_id,UI32_T group_id) ;

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_CreateVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Create virtual group.
 * INPUT:    group_id       - group ID
 *           vir_group_id   - virtual group ID (Unique per system).
 *           priority       - virtual group priority.
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_CreateVirtualGroup(
    UI32_T group_id,
    UI32_T vir_group_id,
    int priority);
#endif

/* rule */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_AllocateRule
 *------------------------------------------------------------------------------
 * PURPOSE  : Allocate rule resource
 * INPUT    : device_id, group_id, rule_pri
 * OUTPUT   : dev_rule_id (per chip unique),
 *
 * RETURN   : TRUE/FALSE.
 * NOTE     : 1, call DEVRM_PMGR_CreateGroup() first
 *            2, MUST allocate rule resource before call DEVRM_PMGR_SetRule()
 *------------------------------------------------------------------------------
 */
#ifdef MARVELL_CPSS
BOOL_T
DEVRM_PMGR_AllocateRule(
    UI32_T  device_id,
    UI32_T  pcl_id,
    UI32_T  vir_group_id,
    int     rule_pri,
    UI32_T  dev_rule_id);
#else
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_AllocateRule
 *------------------------------------------------------------------------------
 * PURPOSE  : Allocate rule resource
 * INPUT    : device_id     - device ID
 *            group_id      - group ID
 *            vir_group_id  - virtual group ID
 *            rule_pri      - rule priority (no used)
 * OUTPUT   : dev_rule_id (per chip unique),
 *
 * RETURN   : TRUE/FALSE.
 * NOTE     : 1, call DEVRM_PMGR_CreateGroup() first
 *            2, MUST allocate rule resource before call DEVRM_PMGR_SetRule()
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_AllocateRule(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T vir_group_id,
    int rule_pri,
    UI32_T *dev_rule_id);
#endif /* MARVELL_CPSS */

#if defined (MARVELL_CPSS) || defined (REALTEK)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_SetRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a rule entry.
 * INPUT:    device_id, group_id, dev_rule_id, rule_entry, meter_entry, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     DEVRM_Rule = class_element + meter + action
 *           pass meter_entry with NULL if this rule does not have meter
 *           pass action_entry with NULL if this rule does not have action
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_SetRule(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T    *ace_entry,
    const DEVRM_ACTION_T *dev_action_p
);
#else
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_SetRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a rule entry.
 * INPUT:    device_id, group_id, dev_rule_id, rule_entry, meter_entry, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     DEVRM_Rule = class_element + meter + action
 *           pass meter_entry with NULL if this rule does not have meter
 *           pass action_entry with NULL if this rule does not have action
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_SetRule(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry,
    DEVRM_MeterEntry_T *meter_entry,
    DEVRM_ActionEntry_T *action_entry,
    BOOL_T counter_enable
);
#endif /* MARVELL_CPSS */

#ifdef MARVELL_CPSS
#elif defined (REALTEK)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_UpdateRule
 *------------------------------------------------------------------------------
 * PURPOSE:  update a rule entry.
 * INPUT:    device_id, dev_rule_id, rule_entry, meter_entry, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     DEVRM_Rule = class_element + meter + action
 *           pass meter_entry with NULL if this rule does not have meter
 *           pass action_entry with NULL if this rule does not have action
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_UpdateRule(
    UI32_T device_id,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry,
    DEVRM_ACTION_T *action_p
);
#else
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_UpdateRule
 *------------------------------------------------------------------------------
 * PURPOSE:  update a rule entry.
 * INPUT:    device_id, dev_rule_id, rule_entry, meter_entry, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     DEVRM_Rule = class_element + meter + action
 *           pass meter_entry with NULL if this rule does not have meter
 *           pass action_entry with NULL if this rule does not have action
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_UpdateRule(
    UI32_T device_id,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry,
    DEVRM_MeterEntry_T *meter_entry,
    DEVRM_ActionEntry_T *action_entry
);
#endif /* MARVELL_CPSS */

#ifdef MARVELL_CPSS
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_DestroyRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a rule entry.
 * INPUT:    dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.If meter is shared, when delete it should check ..
 *------------------------------------------------------------------------------
 */

BOOL_T
DEVRM_PMGR_DestroyRule(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id
);

#else

BOOL_T DEVRM_PMGR_DestroyRule(
    UI32_T device_id,
    UI32_T vir_group_id,
    UI32_T dev_rule_id);

#endif /* MARVELL_CPSS */

/* meter */

#ifdef MARVELL_CPSS
#elif defined (REALTEK)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_CreateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  create a meter entry.
 * INPUT:    device_id, dev_rule_id, meter_entry,
 *           need_reinstalled (TRUE - rule will be reinstalled)
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_CreateMeter(UI32_T device_id,UI32_T dev_rule_id, DEVRM_POLICER_METER_T *meter_entry_p, BOOL_T need_reinstalled);
#else
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_CreateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  create a meter entry.
 * INPUT:    device_id, dev_rule_id, meter_entry,
 *           need_reinstalled (TRUE - rule will be reinstalled)
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_CreateMeter(UI32_T device_id,UI32_T dev_rule_id, DEVRM_MeterEntry_T *meter_entry, BOOL_T need_reinstalled);
#endif /* MARVELL_CPSS */

#ifdef MARVELL_CPSS
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_UpdateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  update a meter entry.
 * INPUT:    device_id, dev_rule_id, meter_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_UpdateMeter(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    const DEVRM_POLICER_METER_T *dev_meter_p
);
#elif defined (REALTEK)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_UpdateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  update a meter entry.
 * INPUT:    device_id, dev_rule_id, meter_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_UpdateMeter(
    UI32_T device_id,
    UI32_T dev_rule_id,
    const DEVRM_POLICER_METER_T *meter_entry_p
);
#else
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_UpdateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  update a meter entry.
 * INPUT:    device_id, dev_rule_id, meter_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_UpdateMeter(UI32_T device_id,UI32_T dev_rule_id, DEVRM_MeterEntry_T *meter_entry);
#endif /* MARVELL_CPSS */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_ShareMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  share a meter from src_rule_id to dst_rule_id
 * INPUT:    device_id, src_rule_id, dst_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_ShareMeter(UI32_T device_id, UI32_T src_rule_id, UI32_T dst_rule_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_DestroyMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a meter entry.
 * INPUT:    device_id, dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_DestroyMeter(UI32_T device_id, UI32_T dev_rule_id);

/* action */
#ifdef MARVELL_CPSS
#elif defined (REALTEK)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_SetAction
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a action entry.
 * INPUT:    device_id, dev_rule_id, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_SetAction(
    UI32_T device_id,
    UI32_T dev_rule_id,
    DEVRM_ACTION_T *action_p
);
#else
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_SetAction
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a action entry.
 * INPUT:    device_id, dev_rule_id, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_SetAction(UI32_T device_id, UI32_T dev_rule_id, DEVRM_ActionEntry_T *action_entry);
#endif /* MARVELL_CPSS */

#ifdef MARVELL_CPSS
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_DestroyAction
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a action entry.
 * INPUT:    device_id, dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_DestroyAction(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id
);
#else
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_DestroyAction
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a action entry.
 * INPUT:    device_id, dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_DestroyAction(UI32_T device_id, UI32_T dev_rule_id);
#endif /* MARVELL_CPSS */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_InitiateProcessResource
 *------------------------------------------------------------------------------
 * FUNCTION: Initialize for getting the ipc message queue
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : None
 * NOTE    :
 *------------------------------------------------------------------------------
 */
void DEVRM_PMGR_InitiateProcessResource(void) ;

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_GetNumberOfChips
 *------------------------------------------------------------------------------
 * PURPOSE  : Get soc_ndev
 * INPUT    : None
 * OUTPUT   : NONE.
 * RETURN   : number of chip.
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T DEVRM_PMGR_GetNumberOfChips();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_GetDeviceInfo
 *------------------------------------------------------------------------------
 * PURPOSE  : Get device information
 * INPUT    : None
 * OUTPUT   : num_of_chips
 *            port_bit_map.all_ports  (broadcom)
 *            port_bit_map.cpu_port   (broadcom)
 * RETURN   : TRUE/FALSE
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_GetDeviceInfo(
    DEVRM_DevInfo_T *dev_info_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_CheckSoc
 *------------------------------------------------------------------------------
 * PURPOSE  : check soc
 * INPUT    : None
 * OUTPUT   : NONE.
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_CheckSoc();

#ifdef MARVELL_CPSS
BOOL_T
DEVRM_PMGR_CreatePcl(
    UI32_T device_id,
    UI32_T pcl_id,
    BOOL_T is_ingress,
    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num,
    UI32_T rule_type
);
#endif /* MARVELL_CPSS */

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - DEVRM_PMGR_BindPclToPort
 *------------------------------------------------------------------------------
 * PURPOSE  : Bind PCL to port
 * INPUT    : device_id - device ID
 *            phy_port  - phy port
 *            pcl_id    - PCL ID
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_BindPclToPort(
    UI32_T device_id,
    UI32_T phy_port,
    UI32_T pcl_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - DEVRM_PMGR_BindPclToTrunkPort
 *------------------------------------------------------------------------------
 * PURPOSE  : Bind PCL to trunk port
 * INPUT    : device_id - device ID
 *            trunk_id  - trunk ID
 *            pcl_id    - PCL ID
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_BindPclToTrunkPort(
    UI32_T device_id,
    UI32_T trunk_id,
    UI32_T pcl_id
);

#ifdef MARVELL_CPSS
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_GetCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  get counter
 * INPUT:    aclId, logical_rule_id
 * OUTPUT:   green_octets_p, yellow_octets_p, red_octets_p
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_GetCounter(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    UI32_T *green_octets_p,
    UI32_T *yellow_octets_p,
    UI32_T *red_octets_p
);
#else
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_GetCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  get counter
 * INPUT:    aclId, logical_rule_id
 * OUTPUT:   green_octets_p, yellow_octets_p, red_octets_p
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_GetCounter(
    UI32_T device_id,
    UI32_T dev_rule_id,
    UI32_T *counter_p
);
#endif /* MARVELL_CPSS */

#ifdef MARVELL_CPSS
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_ClearCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  clear counter
 * INPUT:    aclId, logical_rule_id
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_ClearCounter(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id
);
#else
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_ClearCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  clear counter
 * INPUT:    aclId, logical_rule_id
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_ClearCounter(
    UI32_T device_id,
    UI32_T dev_rule_id
);
#endif /* MARVELL_CPSS */

#endif /* #ifndef _DEV_RM_PMGR_H_ */
