/*
 *  dos_om.c
 *  sysrsc
 *
 *  Created by yehjunying on 2011/4/28.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "dos_om.h"

/* FUNCTION NAME : dos_om_GetShMemInfo
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
void DOS_OM_GetShMemInfo(SYSRSC_MGR_SEGID_T *segid_p, UI32_T *seglen_p)
{
    *segid_p = SYSRSC_MGR_DOS_SHMEM_SEGID;
    *seglen_p = 100;
    return;
}

/* FUNCTION NAME : DOS_OM_InitiateSystemResources
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
void DOS_OM_InitiateSystemResources(void)
{
    return;
}

/* FUNCTION NAME : DOS_OM_AttachSystemResources
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
void DOS_OM_AttachSystemResources(void)
{
    return;
}
