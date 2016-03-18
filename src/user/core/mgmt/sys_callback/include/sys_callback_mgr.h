/*
 *  sys_callback_mgr.h
 *  radius
 *
 *  Created by yehjunying on 2011/8/16.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __SYS_CALLBACK_MGR_H__
#define __SYS_CALLBACK_MGR_H__

#include "sys_type.h"
#include "sys_cpnt.h"
#include "sys_adpt.h"
#include "rule_type.h"

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
static UI8_T _change_list[SYS_ADPT_TIME_RANGE_TOTAL_NBR_OF_BYTE_FOR_ENTRY_LIST];
static UI8_T _status_list[SYS_ADPT_TIME_RANGE_TOTAL_NBR_OF_BYTE_FOR_ENTRY_LIST];
#endif

/*-----------------------------------------------------------------------------
 * FUNCTION NAME - SYS_CALLBACK_MGR_AnnounceAclDeleted
 *-----------------------------------------------------------------------------
 * PURPOSE : Call CallBack function when one ACL be deleted.
 * INPUT   :
 * OUTPUT  : None.
 *
 * RETURN  : TRUE  -- Callback messages have been sent successfully
 *           FALSE -- Fail to send some of the callback messages
 *
 * NOTES   : None.
 *-----------------------------------------------------------------------------
 */
static BOOL_T inline
SYS_CALLBACK_MGR_AnnounceAclDeleted(
        UI32_T      src_csc_id,
        const char  *acl_name,
        UI32_T      acl_type,
        UI8_T       port_list[SYS_ADPT_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST])
{
    memset(port_list, 0, RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST);
    return TRUE;
}

/*-----------------------------------------------------------------------------
 * FUNCTION NAME - SYS_CALLBACK_MGR_AnnouncePolicyMapDeleted
 *-----------------------------------------------------------------------------
 * PURPOSE : Call CallBack function when one ACL be deleted.
 * INPUT   :
 * OUTPUT  : None.
 *
 * RETURN  : TRUE  -- Callback messages have been sent successfully
 *           FALSE -- Fail to send some of the callback messages
 *
 * NOTES   : None.
 *-----------------------------------------------------------------------------
 */
static BOOL_T inline
SYS_CALLBACK_MGR_AnnouncePolicyMapDeleted(
        UI32_T      src_csc_id,
        const char  *policy_map_name,
        UI8_T       dynamic_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST])
{
    memset(dynamic_port_list, 0, RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST);
    return TRUE;
}

static BOOL_T inline
SYS_CALLBACK_MGR_TIME_RANGE_StatusChange(UI32_T csc_id, UI8_T * change_list, UI8_T *status_list)
{
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    memcpy(_change_list, change_list, SYS_ADPT_TIME_RANGE_TOTAL_NBR_OF_BYTE_FOR_ENTRY_LIST);
    memcpy(_status_list, status_list, SYS_ADPT_TIME_RANGE_TOTAL_NBR_OF_BYTE_FOR_ENTRY_LIST);
#endif
    return TRUE;
}


static BOOL_T inline
SYS_CALLBACK_MGR_L4_AddAclCallback(
                                      UI32_T src_csc_id,
                                      UI32_T acl_index,
                                      char *acl_name)
{
    return TRUE;
}

static BOOL_T inline
SYS_CALLBACK_MGR_L4_ModifyAclCallback(
                                      UI32_T src_csc_id,
                                      UI32_T acl_index,
                                      char *acl_name)
{
    return TRUE;
}

static BOOL_T inline
SYS_CALLBACK_MGR_L4_DeleteAclCallback(
    UI32_T src_csc_id,
    UI32_T acl_index,
   char *acl_name)
{
    return TRUE;
}

#endif
