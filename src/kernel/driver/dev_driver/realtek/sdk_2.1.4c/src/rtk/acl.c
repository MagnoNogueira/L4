#include "rtk/acl.h"
#include "assert.h"
#include "sys_adpt.h"
#include "sys_cpnt.h"
#include <string.h>


//#define  SYS_ADPT_DIFFSERV_MAX_NBR_OF_METER             256
//#define  SYS_ADPT_DIFFSERV_MAX_NBR_OF_ACTIONS           256


static acl_data_t acl_data[ACL_MAX_DEV_NUM];

int32
rtk_ut_acl_init()
{
    memset(&acl_data, 0, sizeof(acl_data));

    return RT_ERR_OK;
}

rtk_acl_entry_t *
rtk_ut_acl_getSdkRuleEntry(int32 unit, rtk_acl_phase_t phase, int32 index)
{
    int32 base = 0;

    if (ACL_MAX_DEV_NUM <= unit)
    {
        return NULL;
    }

    if (phase == ACL_PHASE_EGR_ACL)
        base = acl_data[unit].partition * ACL_MAX_SLICE_RULE_NUM;

    return &acl_data[unit].sdk_rule_entry[index+base];
}

int32
rtk_acl_blockLookupEnable_get(uint32 unit, uint32 block_idx, rtk_enable_t *pEnable)
{
    if (ACL_MAX_SLICE_NUM <= block_idx ||
        ACL_MAX_DEV_NUM <= unit)
        return RT_ERR_FAILED;

    *pEnable = acl_data[unit].block[block_idx].lookup.enable;

    return RT_ERR_OK;
}

int32
rtk_acl_blockLookupEnable_set(uint32 unit, uint32 block_idx, rtk_enable_t enable)
{
    if (ACL_MAX_SLICE_NUM <= block_idx ||
        ACL_MAX_DEV_NUM <= unit)
        return RT_ERR_FAILED;

    acl_data[unit].block[block_idx].lookup.enable = enable;

    return RT_ERR_OK;
}

int32
rtk_acl_blockPwrEnable_get(uint32 unit, uint32 block_idx, rtk_enable_t *pEnable)
{
    if (ACL_MAX_SLICE_NUM <= block_idx ||
        ACL_MAX_DEV_NUM <= unit)
        return RT_ERR_FAILED;

    *pEnable = acl_data[unit].block[block_idx].pwr.enable;

    return RT_ERR_OK;
}

int32
rtk_acl_blockPwrEnable_set(uint32 unit, uint32 block_idx, rtk_enable_t enable)
{
    if (ACL_MAX_SLICE_NUM <= block_idx ||
        ACL_MAX_DEV_NUM <= unit)
        return RT_ERR_FAILED;

    acl_data[unit].block[block_idx].pwr.enable = enable;

    return RT_ERR_OK;
}

int32
rtk_acl_rule_del(uint32 unit, rtk_acl_phase_t phase, rtk_acl_clear_t *pClrIdx)
{
    uint32 clear_start, clear_end;

    if (ACL_MAX_DEV_NUM <= unit)
    {
        return RT_ERR_FAILED;
    }

    clear_start = pClrIdx->start_idx;
    clear_end = pClrIdx->end_idx;

    if (phase == ACL_PHASE_EGR_ACL)
    {
        clear_start += (acl_data[unit].partition * ACL_MAX_SLICE_RULE_NUM);
        clear_end += (acl_data[unit].partition * ACL_MAX_SLICE_RULE_NUM);
    }

    memset(&acl_data[unit].sdk_rule_entry[clear_start], 0, sizeof(rtk_acl_entry_t) * (clear_end - clear_start + 1));

    return RT_ERR_OK;
}

int32
rtk_acl_rule_move(uint32 unit, rtk_acl_phase_t phase, rtk_acl_move_t *pData)
{
    rtk_acl_entry_t temp_entry[ACL_MAX_SLICE_NUM * ACL_MAX_SLICE_RULE_NUM];
    uint32 base = 0;


    if (ACL_MAX_DEV_NUM <= unit)
        return RT_ERR_FAILED;

    if (phase == ACL_PHASE_EGR_ACL)
        base = acl_data[unit].partition * ACL_MAX_SLICE_RULE_NUM;

    if ((base+ pData->move_to + pData->length) >= (ACL_MAX_SLICE_NUM * ACL_MAX_SLICE_RULE_NUM))
        return RT_ERR_FAILED;

    memcpy(temp_entry, &acl_data[unit].sdk_rule_entry[base+pData->move_from], sizeof(rtk_acl_entry_t) * pData->length);
    memset(&acl_data[unit].sdk_rule_entry[base+pData->move_from], 0, sizeof(rtk_acl_entry_t) * pData->length);
    memcpy(&acl_data[unit].sdk_rule_entry[base+pData->move_to], temp_entry, sizeof(rtk_acl_entry_t) * pData->length);

    return RT_ERR_OK;
}


int32
rtk_acl_partition_set(uint32 unit, uint32 partition)
{
    if (ACL_MAX_DEV_NUM <= unit)
        return RT_ERR_FAILED;

    if (partition > ACL_MAX_SLICE_NUM)
        return RT_ERR_FAILED;

    acl_data[unit].partition = partition;
    return RT_ERR_OK;
}


int32
rtk_acl_templateSelector_set(
    uint32                  unit,
    uint32                  block_idx,
    rtk_acl_templateIdx_t   template_idx)
{
    return RT_ERR_OK;
}


static int32
rtk_acl_fillRuleEntryTemplae0(
    rtk_acl_entry_t *entry_p,
    uint8           *pData,
    uint8           *pMask)
{

    return RT_ERR_OK;
}

static int32
rtk_acl_fillRuleEntryTemplae1(
    rtk_acl_entry_t *entry_p,
    uint8           *pData,
    uint8           *pMask)
{
    
    return RT_ERR_OK;
}

static int32
rtk_acl_fillRuleEntryTemplae2(
    rtk_acl_entry_t *entry_p,
    uint8           *pData,
    uint8           *pMask)
{
    
    return RT_ERR_OK;
}

static int32
rtk_acl_fillRuleEntryTemplae3(
    rtk_acl_entry_t *entry_p,
    uint8           *pData,
    uint8           *pMask)
{

    return RT_ERR_OK;
}

static int32
rtk_acl_fillRuleEntryTemplae4(
    rtk_acl_entry_t *entry_p,
    uint8           *pData,
    uint8           *pMask)
{

    return RT_ERR_OK;
}

int32
rtk_acl_ruleEntryField_write(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    rtk_acl_entry_t *entry_p;

    if (ACL_MAX_DEV_NUM <= unit)
        return RT_ERR_FAILED;

    if (entry_idx >= ACL_MAX_SLICE_NUM * 128)
        return RT_ERR_FAILED;

    entry_p = &acl_data[unit].sdk_rule_entry[entry_idx];


    switch (type)
    {
        case USER_FIELD_TEMPLATE_ID:
            entry_p->fix.data.template_id = *pData;
            entry_p->fix.mask.template_id = *pMask;
            break;

        case USER_FIELD_FRAME_TYPE_L2:
            entry_p->fix.data.frame_type = *pData;
            entry_p->fix.mask.frame_type = *pMask;
            break;

        case USER_FIELD_OTAG_FMT:
            entry_p->fix.data.outer_untag_pritag = *pData;
            entry_p->fix.mask.outer_untag_pritag = *pMask;
            break;

        case USER_FIELD_OTAG_EXIST:
            entry_p->fix.data.outer_tag_pritag = *pData;
            entry_p->fix.mask.outer_tag_pritag = *pMask;
            break;

        case USER_FIELD_ITAG_FMT:
            entry_p->fix.data.inner_untag_pritag = *pData;
            entry_p->fix.mask.inner_untag_pritag = *pMask;
            break;

        case USER_FIELD_ITAG_EXIST:
            entry_p->fix.data.inner_tag_pritag = *pData;
            entry_p->fix.mask.inner_tag_pritag = *pMask;
            break;

        case USER_FIELD_FRAME_TYPE:
            entry_p->fix.data.frame_type = *pData;
            entry_p->fix.mask.frame_type = *pMask;
            break;

        case USER_FIELD_SWITCHMAC:
            entry_p->fix.data.switch_mac = *pData;
            entry_p->fix.mask.switch_mac = *pMask;
            break;

        default:
            switch (entry_p->fix.data.template_id)
            {
                case 0:
                    return rtk_acl_fillRuleEntryTemplae0(entry_p, pData, pMask);

                case 1:
                    return rtk_acl_fillRuleEntryTemplae1(entry_p, pData, pMask);

                case 2:
                    return rtk_acl_fillRuleEntryTemplae2(entry_p, pData, pMask);

                case 3:
                    return rtk_acl_fillRuleEntryTemplae3(entry_p, pData, pMask);

                case 4:
                    return rtk_acl_fillRuleEntryTemplae4(entry_p, pData, pMask);
                    
                default:
                    assert(0);
                    break;
            }
            break;
    }

    return RT_ERR_OK;
}


int32
rtk_acl_ruleValidate_set(
                         uint32              unit,
                         rtk_acl_phase_t     phase,
                         rtk_acl_id_t        entry_idx,
                         uint32              valid)
{
    uint32 base = 0;

    if (phase == ACL_PHASE_EGR_ACL)
        base += acl_data[unit].partition * ACL_MAX_SLICE_RULE_NUM;

    if ((base + entry_idx) >= ACL_MAX_SLICE_NUM * ACL_MAX_SLICE_RULE_NUM)
        assert(0);

    acl_data[unit].sdk_rule_entry[entry_idx+base].valid = valid;

    return RT_ERR_OK;
}

int32
rtk_acl_ruleAction_set(
                       uint32               unit,
                       rtk_acl_phase_t      phase,
                       rtk_acl_id_t         entry_idx,
                       rtk_acl_action_t     *pAction)
{
    uint32 base = 0;

    if (phase == ACL_PHASE_EGR_ACL)
        base += acl_data[unit].partition * ACL_MAX_SLICE_RULE_NUM;

    if ((base + entry_idx) >= ACL_MAX_SLICE_NUM * ACL_MAX_SLICE_RULE_NUM)
        assert(0);

    memcpy(&acl_data[unit].sdk_rule_entry[entry_idx+base].action,
           pAction,
           sizeof(rtk_acl_action_t));

    return RT_ERR_OK;
}

int32
rtk_acl_ruleAction_get(
                       uint32               unit,
                       rtk_acl_phase_t      phase,
                       rtk_acl_id_t         entry_idx,
                       rtk_acl_action_t     *pAction)
{
    return RT_ERR_OK;
}

int32
rtk_acl_ruleOperation_set(
                          uint32                  unit,
                          rtk_acl_phase_t         phase,
                          rtk_acl_id_t            entry_idx,
                          rtk_acl_operation_t     *pOperation)
{
    if (pOperation->aggr_1 == ENABLED && (entry_idx%2 != 0))
        assert(0);

    if (pOperation->aggr_2 == ENABLED && (entry_idx%2 != 0))
        assert(0);

    return RT_ERR_OK;
}

#pragma mark - meter

int32
rtk_acl_meterMode_set(
                      uint32              unit,
                      uint32              idx,
                      rtk_acl_meterMode_t meterMode)
{
    return RT_ERR_OK;
}

int32
rtk_acl_meterBurstSize_set(
                           uint32                      unit,
                           rtk_acl_meterMode_t         meterMode,
                           rtk_acl_meterBurstSize_t    *pBurstSize)
{
    return RT_ERR_OK;
}

int32
rtk_acl_meterBurstSize_get(
                           uint32                      unit,
                           rtk_acl_meterMode_t         meterMode,
                           rtk_acl_meterBurstSize_t    *pBurstSize)
{
    return RT_ERR_OK;
}


int32
rtk_acl_meterEntry_set(
                       uint32                  unit,
                       uint32                  meterIdx,
                       rtk_acl_meterEntry_t    *pMeterEntry)
{
    return RT_ERR_OK;
}

int32
rtk_acl_meterEntry_get(
                       uint32                  unit,
                       uint32                  meterIdx,
                       rtk_acl_meterEntry_t    *pMeterEntry)
{
    return RT_ERR_OK;
}

#pragma mark - counter

int32
rtk_acl_stat_clearAll(uint32 unit)
{
    assert(unit == 0);
    memset(&acl_data[unit], 0, sizeof(acl_data[unit]));

    return RT_ERR_OK;
}

int32
rtk_acl_statPktCnt_get(uint32 unit, uint32 log_id, uint32 *pPkt_cnt)
{
    assert(unit == 0);
    assert(log_id < 256);

    *pPkt_cnt = acl_data[unit].counter.packets[log_id];

    return RT_ERR_OK;
}

int32
rtk_acl_statPktCnt_set(uint32 unit, uint32 log_id, uint32 Pkt_cnt)
{
    assert(unit == 0);
    assert(log_id < 256);

    acl_data[unit].counter.packets[log_id] = Pkt_cnt;

    return RT_ERR_OK;
}

int32
rtk_acl_statPktCnt_clear(uint32 unit, uint32 log_id)
{
    assert(unit == 0);
    assert(log_id < 256);

    acl_data[unit].counter.packets[log_id] = 0;

    return RT_ERR_OK;
}

int32
rtk_acl_statByteCnt_get(uint32 unit, uint32 log_id, uint64 *pByte_cnt)
{
    assert(unit == 0);
    assert(log_id < 256);

    *pByte_cnt = acl_data[unit].counter.bytes[log_id];
    return RT_ERR_OK;
}

int32
rtk_acl_statByteCnt_set(uint32 unit, uint32 log_id, uint64 Byte_cnt)
{
    assert(unit == 0);
    assert(log_id < 256);

    acl_data[unit].counter.bytes[log_id] = Byte_cnt;
    return RT_ERR_OK;
}

int32
rtk_acl_statByteCnt_clear(uint32 unit, uint32 log_id)
{
    assert(unit == 0);
    assert(log_id < 256);

    acl_data[unit].counter.bytes[log_id] = 0;
    return RT_ERR_OK;
}

int32
rtk_acl_ruleEntryFieldSize_get(uint32 unit, uint32 field, uint32 *size_p)
{
    assert(unit == 0);
#if (SYS_HWCFG_RTK_CHIP_FAMILY == SYS_HWCFG_RTK_CHIP_RTL8380)
    {
        *size_p = 4;
    }
#elif (SYS_HWCFG_RTK_CHIP_FAMILY == SYS_HWCFG_RTK_CHIP_RTL8390)
    {
        *size_p = 7;
    }
#else
    {
        return RT_ERR_INPUT;
    }
#endif

    return RT_ERR_OK;
}

int32
rtk_acl_templateFieldIntentVlanTag_set(uint32 unit, rtk_vlan_tagType_t tag_type)
{
    return RT_ERR_OK;
}

int32
rtk_acl_template_set(uint32 unit, uint32 template_id, rtk_acl_template_t *pTemplate)
{
    return RT_ERR_OK;
}
