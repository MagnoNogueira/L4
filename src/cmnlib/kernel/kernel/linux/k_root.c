/* Module Name: ROOT.C
 * Purpose:
 *  This module is in first task of system, bring up whole switching system.
 *  In VxWorks, the first task is usrRoot(), so this module is called by usrRoot().
 *  The bring up sequences will be:
 *      1. Initiate system resources for all the subsystems
 *      2. Create Inter_CSC relation
 *      3. Create tasks for all the subsystems
 *      4. Change and execute usrRoot() task as an Idel task.
 *
 * Notes:
 *  1. The bootstrap sequence MUST be lower layer first, highest layer last.
 *  2. If there is dependence between modules, must take care of the calling
 *     sequence. Missing sequence maybe make a disaster.
 *
 * History:
 *          Date      -- Modifier,        Reason
 *  V1.0  2001.06.22  -- William Chiang,  Creation
 *  V1.0  2001.09.17  -- Anderson Young,  Coding/Programing based on LVM+OOD
 *  V2.0  2002.09.05  -- Jason Hsue,      Add SNTP Task
 *        2002.11.18  -- Jason Hsue,      Add HRDRV and ACL
 *        2002.12.02  -- Jason Hsue,      Modify to support watch dog timer.
 *        2003.11.29  -- Zhong Qiyao,     Modify to add DbSync_Txt
 *        2005.08.16  -- Justin Jan,      Modify to add sysctrl_xor
 *        2006.06.23  -- Charlie Chen,    Add a new init phase: Create_InterCSC_Relation
 *
 * Copyright(C)      Accton Corporation, 1999, 2000, 2001, 2002
 */

/* INCLUDE FILE DECLARATIONS
 */
#include <linux/init.h>    /* for module_init(), module_exit() */

#include "sys_type.h"
#include "sys_bld.h"
#include "sys_cpnt.h"
#include "sys_adpt.h"

#include "k_sysfun.h"
#include "k_l_ipcmem.h"
#include "k_l_mm.h"

#include "k_phyaddr_access.h"
#if defined(SYS_CPNT_I2CDRV) && (SYS_CPNT_I2CDRV==TRUE)
#include "k_i2c.h"
#endif

#ifndef CMNLIB_DEBUG
#include "k_vlan_mgr.h"
#include "k_iml_mgr.h"
#if (SYS_CPNT_AMTRL3 == TRUE)
#include "k_amtrl3_mgr.h"
#endif

#if (SYS_CPNT_DYING_GASP == TRUE)
#include "k_dying_gasp.h"
#endif

extern I32_T VLAN_NET_proto_init(void);
extern void VLAN_NET_cleanup_module(void);
#endif

/* NAMING CONSTANT DECLARARTIONS
 */

/* TYPE DEFINITIONS
 */

/* MACRO DEFINITIONS
 */

/* LOCAL FUNCTIONS DECLARATIONS
 */
static int k_root_init(void);
static void k_root_cleanup(void);
static void ROOT_Initiate_System_Resources(void);
static void ROOT_Create_InterCSC_Relation(void);
static void ROOT_Cleanup_System_Resources(void);

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

/* LOCAL SUBPROGRAM SPECIFICATIONS
 */
/* PURPOSE: Module init function
 * INPUT:   None.
 * OUTPUT:  None.
 * RETURN:  None.
 * NOTES:   None.
 */
static int k_root_init(void)
{

    /* BODY
     */
    printk("\r\n<6>Initial Accton Module.\r\n");

    /* Initiate all system resources.
     *
     * Each subsystem shall
     *  1. initiate the required system resources, such as queues, events, semas, ..etc,
     *     for inter-subystem communication.
     *  2. register call-back functions for inter-subystem communication.
     */
    ROOT_Initiate_System_Resources();

    /* Create inter-CSC relationships.
     * Relashionships between CSCs are created through function pointer
     * registrations and callback functions. At this init phase, all operations
     * related to function pointer registrations will be processed.
     */
    ROOT_Create_InterCSC_Relation();

    return 0;
} /* End of root() */

/* PURPOSE: Module cleanup function
 * INPUT:   None.
 * OUTPUT:  None.
 * RETURN:  None.
 * NOTES:   None.
 */
static void k_root_cleanup(void)
{
    printk("\r\n<6>Cleanup Accton Module.\r\n");

    ROOT_Cleanup_System_Resources();
}

/* PURPOSE: This function initiates all system resources
 * INPUT:   None.
 * OUTPUT:  None.
 * RETURN:  None.
 * NOTES:   None.
 */
static void ROOT_Initiate_System_Resources(void)
{
    printk("\r\n<7>ROOT_Initiate_System_Resources\r\n");

#if (SYS_CPNT_DYING_GASP == TRUE)
    DYING_GASP_Init();
#endif

    K_L_IPCMEM_Initialize();
    K_L_MM_Init();
    SYSFUN_Init();

    PHYADDR_ACCESS_Init();
	PHYSICAL_ADDR_ACCESS_Init();	/* anzhen.zheng, 2/19/2008 */

    /* layer 2 */
#ifndef CMNLIB_DEBUG
    VLAN_NET_proto_init();
    VLAN_MGR_Init();

    /* layer 3 */
    IML_MGR_Init();
#if (SYS_CPNT_AMTRL3 == TRUE)
    K_AMTRL3_MR_Init();
#endif
#endif

} /* End of ROOT_Initiate_System_Resources() */

/* PURPOSE: Create inter-CSC relationships.
 * INPUT:   None.
 * OUTPUT:  None.
 * RETURN:  None.
 * NOTES:   None.
 */
static void ROOT_Create_InterCSC_Relation(void)
{
    printk("\r\n<7>ROOT_Create_InterCSC_Relation\r\n");

#if (SYS_CPNT_DYING_GASP == TRUE)
    DYING_GASP_Create_InterCSC_Relation();
#endif

    K_L_IPCMEM_Create_InterCSC_Relation();
    K_L_MM_Create_InterCSC_Relation();
    SYSFUN_Create_InterCSC_Relation();

    PHYADDR_ACCESS_Create_InterCSC_Relation();
    PHYSICAL_ADDR_ACCESS_Create_InterCSC_Relation();	/* anzhen.zheng, 2/19/2008 */
    VLAN_NET_Create_InterCSC_Relation();

    /* layer 2 */
#ifndef CMNLIB_DEBUG
    VLAN_MGR_Create_InterCSC_Relation();

    /* layer 3 */
    IML_MGR_Create_InterCSC_Relation();

#if (SYS_CPNT_AMTRL3 == TRUE)
    K_AMTRL3_MGR_Create_InterCSC_Relation();
#endif

    ROUTE_MGR_Create_InterCSC_Relation();

#endif

} /* End of ROOT_Create_InterCSC_Relation() */

/* PURPOSE: This function cleanup all system resources
 * INPUT:   None.
 * OUTPUT:  None.
 * RETURN:  None.
 * NOTES:   None.
 */
static void ROOT_Cleanup_System_Resources(void)
{
#ifndef CMNLIB_DEBUG
    VLAN_NET_cleanup_module();
#endif

    K_L_IPCMEM_Uninitialize();
}

/* KERNEL MODULE INIT/EXIT FUNCTION DECLARARTIONS
 */
module_init(k_root_init);
module_exit(k_root_cleanup);

