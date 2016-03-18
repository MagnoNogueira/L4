/*
 *  netcfg_om_ip.h
 *  sysrsc
 *
 *  Created by yehjunying on 2011/4/28.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _NETCFG_OM_IP_H_
#define _NETCFG_OM_IP_H_

#include "sysrsc_mgr.h"

/* FUNCTION NAME : NETCFG_OM_IP_GetShMemInfo
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
void NETCFG_OM_IP_GetShMemInfo(SYSRSC_MGR_SEGID_T *segid_p, UI32_T *seglen_p);

/* FUNCTION NAME : NETCFG_OM_IP_InitateSystemResources
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
void NETCFG_OM_IP_InitateSystemResources(void);

/* FUNCTION NAME : NETCFG_OM_IP_AttachSystemResources
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
void NETCFG_OM_IP_AttachSystemResources(void);

#endif /* _NETCFG_OM_IP_H_ */
