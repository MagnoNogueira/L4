/*
 *  sys_callback_init.h
 *  sysrsc
 *
 *  Created by yehjunying on 2011/4/28.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _SYS_CALLBACK_INIT_H_
#define _SYS_CALLBACK_INIT_H_

#include "sysrsc_mgr.h"

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
void SYS_CALLBACK_INIT_GetShMemInfo(SYSRSC_MGR_SEGID_T *segid_p, UI32_T *seglen_p);

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
void SYS_CALLBACK_INIT_InitiateSystemResources(void);

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
void SYS_CALLBACK_INIT_AttachSystemResources(void);

#endif /* _SYS_CALLBACK_INIT_H_ */
