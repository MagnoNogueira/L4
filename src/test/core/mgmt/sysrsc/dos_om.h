/*
 *  dos_om.h
 *  sysrsc
 *
 *  Created by yehjunying on 2011/4/28.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _DOS_OM_H_
#define _DOS_OM_H_

#include "sysrsc_mgr.h"

/* FUNCTION NAME : DOS_OM_GetShMemInfo
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
void DOS_OM_GetShMemInfo(SYSRSC_MGR_SEGID_T *segid_p, UI32_T *seglen_p);

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
void DOS_OM_InitiateSystemResources(void);

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
void DOS_OM_AttachSystemResources(void);

#endif /* _DOS_OM_H_ */
