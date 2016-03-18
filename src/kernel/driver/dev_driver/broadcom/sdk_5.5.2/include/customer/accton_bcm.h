/*
 * $Id: accton_bcm.h,v 1.0 2005/02/04 21:48:05 $
 * $Copyright: (c) 2000-2003, 2004 Broadcom Corp.
 * All Rights Reserved.$
 */

#ifndef _ACCTON_BCM_H_
#define _ACCTON_BCM_H_

#include <shared/portmode.h>
#include <shared/port.h>
#include <shared/phyconfig.h>

#include <bcm/types.h>

#include <bcm/port.h>

typedef enum bcm_mmu_config_e {
    MMU_CELLLMTCOS,		/* XGS Fabric Hercules1 */
    MMU_PKTLMTCOS,		/* XGS Fabric Hercules1 */
    MMU_CELLLMTCOS_LOWER,	/* XGS Fabric Hercules15 */
    MMU_CELLLMTCOS_UPPER,	/* XGS Fabric Hercules15 */
    MMU_PKTLMTCOS_LOWER,	/* XGS Fabric Hercules15 */
    MMU_PKTLMTCOS_UPPER,	/* XGS Fabric Hercules15 */
    HOLCOSPKTSETLIMIT,		/* XGS12, FireBolt */
    HOLCOSCELLSETLIMIT,		/* XGS12 */
    IBPCOSPKTSETLIMIT,		/* XGS12 */
    IBPDISCARDSETLIMIT,		/* XGS12, FireBolt */
    IBPCELLSETLIMIT,		/* XGS12, FireBolt */
    IBPPKTRESETLIMIT,		/* XGS12 */
    IBPCELLRESETLIMIT,          /* XGS12 */
    IBPPKTSETLIMIT,             /* FireBolt */
    E2EIBPPKTSETLIMIT,          /* FireBolt */
    E2EIBPCELLSETLIMIT,         /* FireBolt */
    E2EIBPDISCARDSETLIMIT       /* FireBolt */
} bcm_mmu_config_t;

/***************************************************************************
 * 	Accton-specific APIs for Hercules MC/IPMC Table Management         * 
 ***************************************************************************/
extern int accton_hercules_mc_port_set_all(int unit, int mcidx, pbmp_t pbmp);
extern int accton_hercules_ipmc_port_set_all(int unit, int mcidx, pbmp_t pbmp);

/***************************************************************************
 * 		Accton-specific APIs for Querying SOC/PHY Type             * 
 ***************************************************************************/
extern const char *acctom_bcm_soc_type_get(int unit);
extern char *accton_bcm_phy_type_get(int unit, soc_port_t port);

/***************************************************************************
 * 		Accton-specific APIs for Configuring MMU Settings          * 
 ***************************************************************************/
extern int 
accton_bcm_mmu_config_get(int unit, bcm_port_t port, bcm_mmu_config_t type, 
int cos_index, int *arg);
extern int 
accton_bcm_mmu_config_set(int unit, bcm_port_t port, bcm_mmu_config_t type, 
int cos_index, int arg);
extern int 
accton_bcm_fb_mmu_config_get(int unit, bcm_port_t port, bcm_mmu_config_t type, 
int cos_index, int *arg_set_limit, int *arg_threshold_select);
extern int 
accton_bcm_fb_mmu_config_set(int unit, bcm_port_t port, bcm_mmu_config_t type, 
int cos_index, int arg_set_limit, int arg_threshold_select);
#endif	/* !_ACCTON_BCM_H_ */
