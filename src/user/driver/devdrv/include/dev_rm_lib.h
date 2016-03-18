//
//  dev_rm_lib.h
//  l4_mgr
//
//  Created by yehjunying on 13/3/21.
//
//

#ifndef _DEV_RM_LIB_H_
#define _DEV_RM_LIB_H_

#include "sys_type.h"
#include "dev_rm.h"

#ifndef _countof
#define _countof(_Array) (sizeof(_Array)/sizeof(_Array[0]))
#endif

#ifndef ASSERT
#define ASSERT(eq)
#endif

void
DEVRM_LIB_RulePatternInit(
    DEVRM_AceEntry_T *rule_pattern_p
);

BOOL_T
DEVRM_LIB_RulePatternQualifyDestMacAddress(
    DEVRM_AceEntry_T *rule_pattern_p,
    const UI8_T *data_p,
    const UI8_T *mask_p
);

BOOL_T
DEVRM_LIB_RulePatternQualifySourceMacAddress(
    DEVRM_AceEntry_T *rule_pattern_p,
    const UI8_T *data_p,
    const UI8_T *mask_p
);

BOOL_T
DEVRM_LIB_RulePatternQualifyEtherType(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI16_T data,
    UI16_T mask
);

BOOL_T
DEVRM_LIB_RulePatternQualifyIp(
    DEVRM_AceEntry_T *rule_pattern_p
);

BOOL_T
DEVRM_LIB_RulePatternQualifyIpv6(
    DEVRM_AceEntry_T *rule_pattern_p
);

BOOL_T
DEVRM_LIB_RulePatternQualifyVlan(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI16_T data,
    UI16_T mask
);

BOOL_T
DEVRM_LIB_RulePatternQualifyCos(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI8_T data,
    UI8_T mask
);

BOOL_T
DEVRM_LIB_RulePatternQualifyIpProtocol(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI8_T data,
    UI8_T mask
);

BOOL_T
DEVRM_LIB_RulePatternQualifyIpDscp(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI8_T data,
    UI8_T mask
);

BOOL_T
DEVRM_LIB_RulePatternQualifyIpv6NextHeader(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI8_T data,
    UI8_T mask
);

BOOL_T
DEVRM_LIB_RulePatternQualifyIcmp(
    DEVRM_AceEntry_T *rule_pattern_p
);

BOOL_T
DEVRM_LIB_RulePatternQualifyUdp(
    DEVRM_AceEntry_T *rule_pattern_p
);

BOOL_T
DEVRM_LIB_RulePatternQualifyTcp(
    DEVRM_AceEntry_T *rule_pattern_p
);

BOOL_T
DEVRM_LIB_RulePatternQualifyIpDestAddress(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI32_T data,
    UI32_T mask
);

BOOL_T
DEVRM_LIB_RulePatternQualifyIpSourceAddress(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI32_T data,
    UI32_T mask
);

BOOL_T
DEVRM_LIB_RulePatternQualifyIpv6DestAddress(
    DEVRM_AceEntry_T *rule_pattern_p,
    const UI8_T *data_p,
    const UI8_T *mask_p
);

BOOL_T
DEVRM_LIB_RulePatternQualifyIpv6SourceAddress(
    DEVRM_AceEntry_T *rule_pattern_p,
    const UI8_T *data_p,
    const UI8_T *mask_p
);

BOOL_T
DEVRM_LIB_RulePatternQualifyIpFragment(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI8_T data,
    UI8_T mask
);

BOOL_T
DEVRM_LIB_RulePatternQualifyL4DestPort(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI16_T data,
    UI16_T mask
);

BOOL_T
DEVRM_LIB_RulePatternQualifyL4SourcePort(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI16_T data,
    UI16_T mask
);

BOOL_T
DEVRM_LIB_RulePatternQualifyTcpControlFlags(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI8_T data,
    UI8_T mask
);

BOOL_T
DEVRM_LIB_RulePatternQualifyIcmpType(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI8_T data,
    UI8_T mask
);

BOOL_T
DEVRM_LIB_RulePatternQualifyIcmpv6(
    DEVRM_AceEntry_T *rule_pattern_p
);

BOOL_T
DEVRM_LIB_RulePatternQualifyIcmpv6Type(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI8_T data,
    UI8_T mask
);

void
DEVRM_LIB_RulePatternDump(
    const DEVRM_AceEntry_T *rule_pattern_p,
    BOOL_T (*printf_fn)(char*, ...),
    UI32_T indent
);

static BOOL_T
DEVRM_LIB_ActionInit(
    DEVRM_ACTION_T *action_p)
{
    ASSERT(NULL != action_p);

    memset(action_p, 0, sizeof(*action_p));
    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionPacketTo(
    DEVRM_ACTION_T *action_p,
    DEVRM_PACKET_COMMAND_T command)
{
    ASSERT(NULL != action_p);

    action_p->pkt_cmd = command;

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionYellowPacketTo(
    DEVRM_ACTION_T *action_p,
    DEVRM_PACKET_COMMAND_T command)
{
    ASSERT(NULL != action_p);

    action_p->policer.yellow_pkt_cmd = command;

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionRedPacketTo(
    DEVRM_ACTION_T *action_p,
    DEVRM_PACKET_COMMAND_T command)
{
    ASSERT(NULL != action_p);

    action_p->policer.red_pkt_cmd = command;

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionModifyInnerPriority(
    DEVRM_ACTION_T *action_p,
    UI8_T priority)
{
    ASSERT(NULL != action_p);

    action_p->qos.modify_inner_priority = DEVRM_MODIFY_CMD_ENABLE;
    action_p->qos.inner_priority = priority;

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionModifyCpuQueue(
    DEVRM_ACTION_T *action_p,
    UI8_T queue)
{
    ASSERT(NULL != action_p);

    action_p->qos.modify_queue = DEVRM_MODIFY_CMD_ENABLE;
    action_p->qos.queue = queue;
    
    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionModifyCpuCode(
    DEVRM_ACTION_T *action_p,
    UI32_T cpu_code)
{
    ASSERT(NULL != action_p);

    action_p->cpu.modify_cpu_code = DEVRM_MODIFY_CMD_ENABLE;
    action_p->cpu.cpu_code = cpu_code;

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionModifyVlan(
    DEVRM_ACTION_T *action_p,
    DEVRM_MODIFY_VLAN_COMMAND_T command,
    UI16_T vid,
    BOOL_T add_outer_tag)
{
    ASSERT(NULL != action_p);
    ASSERT(DEVRM_MODIFY_VLAN_CMD_DISABLE == command ||
           DEVRM_MODIFY_VLAN_CMD_FOR_ALL == command ||
           DEVRM_MODIFY_VLAN_CMD_FOR_UNTAGGED_ONLY == command ||
           DEVRM_MODIFY_VLAN_CMD_FOR_TAGGED_ONLY == command);

    action_p->vlan.modify_vlan = command;
    action_p->vlan.vid = vid;
    action_p->vlan.add_outer_tag = add_outer_tag;

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionModifyPriority(
    DEVRM_ACTION_T *action_p,
    UI8_T priority)
{
    ASSERT(NULL != action_p);

    action_p->qos.modify_priority = DEVRM_MODIFY_CMD_ENABLE;
    action_p->qos.priority = priority & 7;

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionModifyIpDscp(
    DEVRM_ACTION_T *action_p,
    UI8_T dscp)
{
    ASSERT(NULL != action_p);

    action_p->qos.modify_dscp = DEVRM_MODIFY_CMD_ENABLE;
    action_p->qos.dscp = dscp & 63;

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionYellowPacketModifyIpDscp(
    DEVRM_ACTION_T *action_p,
    UI8_T dscp)
{
    ASSERT(NULL != action_p);

    action_p->policer.yellow_pkt_qos.modify_dscp = DEVRM_MODIFY_CMD_ENABLE;
    action_p->policer.yellow_pkt_qos.dscp = dscp & 63;

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionRedPacketModifyIpDscp(
    DEVRM_ACTION_T *action_p,
    UI8_T dscp)
{
    ASSERT(NULL != action_p);

    action_p->policer.red_pkt_qos.modify_dscp = DEVRM_MODIFY_CMD_ENABLE;
    action_p->policer.red_pkt_qos.dscp = dscp & 63;

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionModifyIpTos(
    DEVRM_ACTION_T *action_p,
    UI8_T tos)
{
    ASSERT(NULL != action_p);

    action_p->qos.modify_tos = DEVRM_MODIFY_CMD_ENABLE;
    action_p->qos.tos = tos;

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionEnableDropPrecedence(
    DEVRM_ACTION_T *action_p)
{
    ASSERT(NULL != action_p);

    action_p->qos.drop_precedence = DEVRM_MODIFY_CMD_ENABLE;

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionEnableYellowPacketDropPrecedence(
    DEVRM_ACTION_T *action_p)
{
    ASSERT(NULL != action_p);

    action_p->policer.yellow_pkt_qos.drop_precedence = DEVRM_MODIFY_CMD_ENABLE;

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionEnableRedPacketDropPrecedence(
    DEVRM_ACTION_T *action_p)
{
    ASSERT(NULL != action_p);

    action_p->policer.red_pkt_qos.drop_precedence = DEVRM_MODIFY_CMD_ENABLE;

    return TRUE;
}

#if defined(SYS_CPNT_QOS_DIFFSERV_POLICE_MODE) && (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
static BOOL_T
DEVRM_LIB_ActionMeterConfigRate(
    DEVRM_ACTION_T *action_p,
    UI32_T kbits_sec)
{
    ASSERT(NULL != action_p);

    action_p->policer.meter.mode = DEVRM_POLICER_METER_MODE_RATE;
    action_p->policer.meter.kbits_sec = kbits_sec;

    if (DEVRM_POLICER_MODE_DISABLE == action_p->policer.mode)
    {
        action_p->policer.mode = DEVRM_POLICER_MODE_METER_ONLY;
    }
    else if (DEVRM_POLICER_MODE_METER_ONLY == action_p->policer.mode)
    {
        action_p->policer.mode = DEVRM_POLICER_MODE_METER_AND_COUNTER;
    }
    
    return TRUE;
}
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

static BOOL_T
DEVRM_LIB_ActionMeterConfigType(
    DEVRM_ACTION_T *action_p,
    DEVRM_POLICER_METER_TYPE_T type)
{
    ASSERT(NULL != action_p);
    action_p->policer.meter.type = type;

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionMeterConfigFlow(
    DEVRM_ACTION_T *action_p,
    UI32_T kbits_sec,
    UI32_T kbytes_burst_size)
{
    ASSERT(NULL != action_p);

    action_p->policer.meter.mode = DEVRM_POLICER_METER_MODE_FLOW;
    action_p->policer.meter.kbits_sec = kbits_sec;
    action_p->policer.meter.kbytes_burst_size = kbytes_burst_size;

    if (DEVRM_POLICER_MODE_DISABLE == action_p->policer.mode)
    {
        action_p->policer.mode = DEVRM_POLICER_MODE_METER_ONLY;
    }
    else if (DEVRM_POLICER_MODE_METER_ONLY == action_p->policer.mode)
    {
        action_p->policer.mode = DEVRM_POLICER_MODE_METER_AND_COUNTER;
    }

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionMeterConfigSrTcm(
    DEVRM_ACTION_T *action_p,
    DEVRM_POLICER_METER_COLOR_MODE_T color_mode,
    UI32_T cir_kbits_sec,
    UI32_T cbs_kbytes,
    UI32_T ebs_kbytes)
{
    ASSERT(NULL != action_p);
    ASSERT(DEVRM_POLICER_METER_COLOR_AWARE == color_mode ||
           DEVRM_POLICER_METER_COLOR_BLIND == color_mode);

    if (DEVRM_POLICER_METER_COLOR_AWARE == color_mode)
    {
        action_p->policer.meter.mode = DEVRM_POLICER_METER_MODE_SRTCM_COLOR_AWARE;
    }
    else
    {
        action_p->policer.meter.mode = DEVRM_POLICER_METER_MODE_SRTCM_COLOR_BLIND;
    }

    action_p->policer.meter.kbits_sec = cir_kbits_sec;
    action_p->policer.meter.kbytes_burst_size = cbs_kbytes;
    action_p->policer.meter.peak_kbytes_burst_size = ebs_kbytes;

    if (DEVRM_POLICER_MODE_DISABLE == action_p->policer.mode)
    {
        action_p->policer.mode = DEVRM_POLICER_MODE_METER_ONLY;
    }
    else if (DEVRM_POLICER_MODE_METER_ONLY == action_p->policer.mode)
    {
        action_p->policer.mode = DEVRM_POLICER_MODE_METER_AND_COUNTER;
    }

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionMeterConfigTrTcm(
    DEVRM_ACTION_T *action_p,
    DEVRM_POLICER_METER_COLOR_MODE_T color_mode,
    UI32_T cir_kbits_sec,
    UI32_T cbs_kbytes,
    UI32_T pir_kbits_sec,
    UI32_T pbs_kbytes)
{
    ASSERT(NULL != action_p);
    ASSERT(DEVRM_POLICER_METER_COLOR_AWARE == color_mode ||
           DEVRM_POLICER_METER_COLOR_BLIND == color_mode);

    if (DEVRM_POLICER_METER_COLOR_AWARE == color_mode)
    {
        action_p->policer.meter.mode = DEVRM_POLICER_METER_MODE_TRTCM_COLOR_AWARE;
    }
    else
    {
        action_p->policer.meter.mode = DEVRM_POLICER_METER_MODE_TRTCM_COLOR_BLIND;
    }

    action_p->policer.meter.kbits_sec = cir_kbits_sec;
    action_p->policer.meter.kbytes_burst_size = cbs_kbytes;
    action_p->policer.meter.peak_kbits_sec = pir_kbits_sec;
    action_p->policer.meter.peak_kbytes_burst_size = pbs_kbytes;

    if (DEVRM_POLICER_MODE_DISABLE == action_p->policer.mode)
    {
        action_p->policer.mode = DEVRM_POLICER_MODE_METER_ONLY;
    }
    else if (DEVRM_POLICER_MODE_METER_ONLY == action_p->policer.mode)
    {
        action_p->policer.mode = DEVRM_POLICER_MODE_METER_AND_COUNTER;
    }

    return TRUE;
}

static BOOL_T
DEVRM_LIB_ActionCounterConfig(
    DEVRM_ACTION_T *action_p,
    DEVRM_POLICER_COUNTER_MODE_T counter_mode)
{
    ASSERT(NULL != action_p);
    ASSERT(DEVRM_POLICER_COUNTER_MODE_BYTE == counter_mode ||
           DEVRM_POLICER_COUNTER_MODE_PACKET == counter_mode);

    action_p->policer.counter_mode = counter_mode;

    if (DEVRM_POLICER_MODE_DISABLE == action_p->policer.mode)
    {
        action_p->policer.mode = DEVRM_POLICER_MODE_COUNTER_ONLY;
    }
    else if (DEVRM_POLICER_MODE_METER_ONLY == action_p->policer.mode)
    {
        action_p->policer.mode = DEVRM_POLICER_MODE_METER_AND_COUNTER;
    }

    return TRUE;
}

static void
DEVRM_LIB_ActionVlanDump(
    const DEVRM_ACTION_VLAN_T *vlan_p,
    BOOL_T (*printf_fn)(char*, ...),
    UI32_T indent,
    const char *prefix)
{
    char *modify_vlan_cmd_string[] =
    {
#define MODIFY_VLAN_CMD(vlanCmd) #vlanCmd,

        DEVRM_MODIFY_VLAN_COMMAND_TABLE

#undef MODIFY_VLAN_CMD
    };

    printf_fn("%*s" "+-o %s\r\n", indent, "", prefix);

    indent += 4;
    printf_fn("%*s" "{\r\n", indent, "");

    indent += 2;

    printf_fn("%*s"
              "modify_vlan = %s\r\n",
              indent, "",
              vlan_p->modify_vlan < _countof(modify_vlan_cmd_string) ?
              modify_vlan_cmd_string[vlan_p->modify_vlan] :
              "???");

    if (DEVRM_MODIFY_VLAN_CMD_DISABLE != vlan_p->modify_vlan)
    {
        printf_fn("%*s"
                  "vid = %hu\r\n",
                  indent, "",
                  vlan_p->vid);
        printf_fn("%*s"
                  "add_outer_tag = %s\r\n",
                  indent, "",
                  vlan_p->add_outer_tag ? "Yes" : "No");
    }

    indent -= 2;
    printf_fn("%*s" "}\r\n", indent, "");
}

static void
DEVRM_LIB_ActionCpuDump(
    const DEVRM_ACTION_CPU_T *cpu_p,
    BOOL_T (*printf_fn)(char*, ...),
    UI32_T indent,
    const char *prefix)
{
    char *modify_cmd_string[] =
    {
#define MODIFY_CMD(cmd) #cmd,

        DEVRM_MODIFY_COMMAND_TABLE

#undef MODIFY_CMD
    };

    printf_fn("%*s" "+-o %s\r\n", indent, "", prefix);

    indent += 4;
    printf_fn("%*s" "{\r\n", indent, "");

    indent += 2;

    printf_fn("%*s"
              "modify_cpu_code = %s\r\n",
              indent, "",
              cpu_p->modify_cpu_code < _countof(modify_cmd_string) ?
                    modify_cmd_string[cpu_p->modify_cpu_code] :
                    "???");

    if (DEVRM_MODIFY_CMD_ENABLE == cpu_p->modify_cpu_code)
    {
        printf_fn("%*s"
                  "cpu_code = %hu\r\n",
                  indent, "",
                  cpu_p->cpu_code);
    }

    indent -= 2;
    printf_fn("%*s" "}\r\n", indent, "");
}

static void
DEVM_LIB_ActionQosDump(
    const DEVRM_ACTION_QOS_T *qos_p,
    BOOL_T (*printf_fn)(char*, ...),
    UI32_T indent,
    const char *prefix)
{
    char *modify_cmd_string[] =
    {
#define MODIFY_CMD(cmd) #cmd,

        DEVRM_MODIFY_COMMAND_TABLE

#undef MODIFY_CMD
    };

    printf_fn("%*s" "+-o %s\r\n", indent, "", prefix);

    indent += 4;
    printf_fn("%*s" "{\r\n", indent, "");

    indent += 2;

    printf_fn("%*s"
              "modify_inner_priority = %s\r\n",
              indent, "",
              qos_p->modify_inner_priority < _countof(modify_cmd_string) ?
                    modify_cmd_string[qos_p->modify_inner_priority] :
                    "???");

    if (DEVRM_MODIFY_CMD_ENABLE == qos_p->modify_inner_priority)
    {
        printf_fn("%*s"
                  "inner_priority = %hu\r\n",
                  indent, "",
                  qos_p->inner_priority);
    }

    printf_fn("%*s"
              "modify_priority = %s\r\n",
              indent, "",
              qos_p->modify_priority < _countof(modify_cmd_string) ?
                    modify_cmd_string[qos_p->modify_priority] :
                    "???");

    if (DEVRM_MODIFY_CMD_ENABLE == qos_p->modify_priority)
    {
        printf_fn("%*s"
                  "priority = %hu\r\n",
                  indent, "",
                  qos_p->priority);
    }

    printf_fn("%*s"
              "modify_queue = %s\r\n",
              indent, "",
              qos_p->modify_queue < _countof(modify_cmd_string) ?
                    modify_cmd_string[qos_p->modify_queue] :
                    "???");

    if (DEVRM_MODIFY_CMD_ENABLE == qos_p->modify_queue)
    {
        printf_fn("%*s"
                  "queue = %hu\r\n",
                  indent, "",
                  qos_p->queue);
    }

    printf_fn("%*s"
              "modify_dscp = %s\r\n",
              indent, "",
              qos_p->modify_dscp < _countof(modify_cmd_string) ?
                    modify_cmd_string[qos_p->modify_dscp] :
                    "???");

    if (DEVRM_MODIFY_CMD_ENABLE == qos_p->modify_dscp)
    {
        printf_fn("%*s"
                  "dscp = %hu\r\n",
                  indent, "",
                  qos_p->dscp);
    }

    printf_fn("%*s"
              "modify_tos = %s\r\n",
              indent, "",
              qos_p->modify_tos < _countof(modify_cmd_string) ?
                    modify_cmd_string[qos_p->modify_tos] :
                    "???");

    if (DEVRM_MODIFY_CMD_ENABLE == qos_p->modify_tos)
    {
        printf_fn("%*s"
                  "tos = %hu\r\n",
                  indent, "",
                  qos_p->tos);
    }

    printf_fn("%*s"
              "drop_precedence = %s\r\n",
              indent, "",
              qos_p->drop_precedence < _countof(modify_cmd_string) ?
                    modify_cmd_string[qos_p->drop_precedence] :
                    "???");

    indent -= 2;
    printf_fn("%*s" "}\r\n", indent, "");
}

static void
DEVRM_LIB_ActionRedirectDump(
    const DEVRM_ACTION_REDIRECT_T *redirect_p,
    BOOL_T (*printf_fn)(char*, ...),
    UI32_T indent,
    const char *prefix)
{
    if (redirect_p->mode == DEVRM_REDIRECT_MODE_ROUTE)
    {
        printf_fn("%*s" "+-o %s\r\n", indent, "", prefix);

        indent += 4;
        printf_fn("%*s" "{\r\n", indent, "");

        indent += 2;

        printf_fn("%*s"
                  "mode = ROUTE\r\n",
                  indent, "");

        printf_fn("%*s"
                  "LTT index: row = %lu, column = %lu\r\n",
                  indent, "",
                  redirect_p->ltt_index.row,
                  redirect_p->ltt_index.column);

        indent -= 2;
        printf_fn("%*s" "}\r\n", indent, "");
    }
}

static void
DEVM_LIB_ActionPolicerMeterDump(
    const DEVRM_POLICER_METER_T *meter_p,
    BOOL_T (*printf_fn)(char*, ...),
    UI32_T indent,
    const char *prefix)
{
    char *meter_mode_string[] =
    {
#define METER_MODE(meterMode) #meterMode,

        DEVRM_POLICER_METER_MODE_TABLE

#undef METER_MODE
    };

    printf_fn("%*s" "+-o %s\r\n", indent, "", prefix);

    indent += 4;
    printf_fn("%*s" "{\r\n", indent, "");

    indent += 2;

    printf_fn("%*s"
              "mode = %s\r\n",
              indent, "",
              meter_p->mode < _countof(meter_mode_string) ?
                    meter_mode_string[meter_p->mode] :
                    "???");

    printf_fn("%*s"
              "kbits_sec = %lu\r\n",
              indent, "",
              meter_p->kbits_sec);

    printf_fn("%*s"
              "kbytes_burst_size = %lu\r\n",
              indent, "",
              meter_p->kbytes_burst_size);

    if (DEVRM_POLICER_METER_MODE_TRTCM_COLOR_BLIND == meter_p->mode ||
        DEVRM_POLICER_METER_MODE_TRTCM_COLOR_AWARE == meter_p->mode)
    {
        printf_fn("%*s"
                  "peak_kbits_sec = %lu\r\n",
                  indent, "",
                  meter_p->peak_kbits_sec);

        printf_fn("%*s"
                  "peak_kbytes_burst_size = %lu\r\n",
                  indent, "",
                  meter_p->peak_kbytes_burst_size);
    }

    indent -= 2;
    printf_fn("%*s" "}\r\n", indent, "");
}

static void
DEVM_LIB_ActionPolicerDump(
    const DEVRM_ACTION_POLICER_T *policer_p,
    BOOL_T (*printf_fn)(char*, ...),
    UI32_T indent,
    const char *prefix)
{
    char *pkt_cmd_string[] =
    {
#define PKT_CMD(pktCmd) #pktCmd,

        DEVRM_PACKET_COMMAND_TABLE

#undef PKT_CMD
    };

    char *policer_mode_string[] =
    {
#define POLICER_MODE(policerMode) #policerMode,

        DEVRM_POLICER_MODE_TABLE

#undef POLICER_MODE
    };

    printf_fn("%*s" "+-o %s\r\n", indent, "", prefix);

    indent += 4;
    printf_fn("%*s" "{\r\n", indent, "");

    indent += 2;

    printf_fn("%*s"
              "mode = %s\r\n",
              indent, "",
              policer_p->mode < _countof(policer_mode_string) ?
                    policer_mode_string[policer_p->mode] :
                    "???");

    if (DEVRM_POLICER_MODE_DISABLE != policer_p->mode)
    {
        DEVM_LIB_ActionPolicerMeterDump(&policer_p->meter, printf_fn, indent, "meter");

        printf_fn("%*s"
                  "yellow_pkt_cmd = %s\r\n",
                  indent, "",
                  policer_p->yellow_pkt_cmd < _countof(pkt_cmd_string) ?
                        pkt_cmd_string[policer_p->yellow_pkt_cmd] :
                        "???");

        DEVM_LIB_ActionQosDump(&policer_p->yellow_pkt_qos, printf_fn, indent, "yellow_pkt_qos");

        printf_fn("%*s"
                  "red_pkt_cmd = %s\r\n",
                  indent, "",
                  policer_p->red_pkt_cmd < _countof(pkt_cmd_string) ?
                        pkt_cmd_string[policer_p->red_pkt_cmd] :
                        "???");

        DEVM_LIB_ActionQosDump(&policer_p->red_pkt_qos, printf_fn, indent, "red_pkt_qos");
    }

    indent -= 2;
    printf_fn("%*s" "}\r\n", indent, "");
}

static void
DEVRM_LIB_ActionDump(
    const DEVRM_ACTION_T *action_p,
    BOOL_T (*printf_fn)(char*, ...),
    UI32_T indent)
{
    char *prefix = "action";

    ASSERT(NULL != action_p);

    char *pkt_cmd_string[] =
    {
#define PKT_CMD(pktCmd) #pktCmd,

        DEVRM_PACKET_COMMAND_TABLE

#undef PKT_CMD
    };

    printf_fn("%*s" "+-o %s\r\n", indent, "", prefix);

    indent += 4;
    printf_fn("%*s" "{\r\n", indent, "");

    indent += 2;

    if (NULL == action_p)
    {
        printf_fn("%*s" "nil\r\n", indent, "");
        return;
    }

    printf_fn("%*s"
              "pkt_cmd = %s\r\n",
              indent, "",
              action_p->pkt_cmd < _countof(pkt_cmd_string) ?
                    pkt_cmd_string[action_p->pkt_cmd] :
                    "???");

    DEVRM_LIB_ActionVlanDump(&action_p->vlan, printf_fn, indent, "vlan");
    DEVM_LIB_ActionQosDump(&action_p->qos, printf_fn, indent, "qos");
    DEVRM_LIB_ActionCpuDump(&action_p->cpu, printf_fn, indent, "cpu");
    DEVRM_LIB_ActionRedirectDump(&action_p->redirect, printf_fn, indent, "redirect");

    DEVM_LIB_ActionPolicerDump(&action_p->policer, printf_fn, indent, "policer");

    indent -= 2;
    printf_fn("%*s" "}\r\n", indent, "");
}

#endif /* _DEV_RM_LIB_H_ */
