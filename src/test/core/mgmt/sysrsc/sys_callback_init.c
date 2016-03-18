/*
 *  sys_callback_init.c
 *  sysrsc
 *
 *  Created by yehjunying on 2011/4/28.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "sys_callback_init.h"

/* FUNCTION NAME : SYS_CALLBACK_INIT_GetShMemInfo
 * PURPOSE:
 *          Get shared memory space information.
 *
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      segid_p
 *      seglen_p
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      None.
 */
void SYS_CALLBACK_INIT_GetShMemInfo(SYSRSC_MGR_SEGID_T *segid_p, UI32_T *seglen_p)
{
    *segid_p = SYSRSC_MGR_NETCFG_OM_IP_SHMEM_SEGID;
    *seglen_p = 100;
    return;
}

/* FUNCTION NAME : SYS_CALLBACK_INIT_InitateSystemResources
 * PURPOSE:
 *          Initialize system resources.
 *
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      None.
 */
void SYS_CALLBACK_INIT_InitiateSystemResources(void)
{
    return;
}

/* FUNCTION NAME : SYS_CALLBACK_INIT_AttachSystemResources
 * PURPOSE:
 *          Attach system resources.
 *
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      None.
 */
void SYS_CALLBACK_INIT_AttachSystemResources(void)
{
    return;
}
