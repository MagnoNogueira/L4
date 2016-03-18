/* ------------------------------------------------------------------------
 * FILE NAME - DEV_SWDRV_COMMON.H
 * ------------------------------------------------------------------------
 * ABSTRACT :
 * Purpose:
 *   to be used by dev_*.c
 * Note:
 * ------------------------------------------------------------------------
 *  History
 *
 *   Wakka             07/12/2012      new created
 *
 * ------------------------------------------------------------------------
 * Copyright(C)                                     Edge-Core Corp. , 2012
 * ------------------------------------------------------------------------
 */
#ifndef DEV_SWDRV_COMMON_H
#define DEV_SWDRV_COMMON_H

/* INCLUDE FILE DECLARATIONS
 */
/* Accton header files
 */
#include "sys_adpt.h"
#include "sys_dflt.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* for the maximum number of chip per unit
 */
#define LOCAL_DEV_MAX_NBR_OF_CHIP_PER_UNIT   1

/* only for debug and it will be remove in the future */
#if 1
#define RTK_DEV_DEBUG() {\
static UI32_T counter;\
    if (counter == 0){\
    counter ++;\
    printf("\n[Not Implemented !!][%s][%d]\n", __FUNCTION__, __LINE__);\
    }\
    return TRUE;\
}
#else
#define RTK_DEV_DEBUG()
#endif

#if 0 /*RTK-DB*/ 

/* for phy register
 */
#define DEV_SWDRV_DUMMY_PHY_MODEL   0

/* to identify QT2225 (a.k.a Marvell 98X22251) simpilify,
 * here give QT2225 a virtual model (MUST be different from Marvell PHYs)
 */
#define DEV_SWDRV_PHY_MODEL_QT2X25  (0x1 << 4)


/* MACRO FUNCTION DECLARATIONS
 */
#endif /*RTK-DB*/

/* convert local device id to array idx
 */
#define LOCAL_DEV_IDX(device_id) ((device_id) - dev_swdrv_local_first_dev_id)

#if 0 /*RTK-DB*/ 
/* wrapper for PHY access API
 */
#define cpssDxChPhyPortSmiRegisterWriteMask(devNum, portNum, phyReg, data, mask) ({ \
            GT_U16 _temp;       \
            cpssDxChPhyPortSmiRegisterRead((devNum), (portNum), (phyReg), &_temp);  \
            _temp &= ~ (mask);  \
            _temp |= (data);    \
            cpssDxChPhyPortSmiRegisterWrite((devNum), (portNum), (phyReg), _temp);  \
        })
#define cpssDxChPhyPortSmiRegisterReadByPage(devNum, portNum, pageReg, page, phyReg, dataPtr) \
            (cpssDxChPhyPortSmiRegisterWrite((devNum), (portNum), (pageReg), (page)), \
             cpssDxChPhyPortSmiRegisterRead((devNum), (portNum), (phyReg), (dataPtr)))
#define cpssDxChPhyPortSmiRegisterWriteByPage(devNum, portNum, pageReg, page, phyReg, data) \
            (cpssDxChPhyPortSmiRegisterWrite((devNum), (portNum), (pageReg), (page)), \
             cpssDxChPhyPortSmiRegisterWrite((devNum), (portNum), (phyReg), (data)))
#define cpssDxChPhyPortSmiRegisterWriteMaskByPage(devNum, portNum, pageReg, page, phyReg, data, mask) ({ \
            GT_U16 _temp;       \
            cpssDxChPhyPortSmiRegisterReadByPage((devNum), (portNum), (pageReg), (page), (phyReg), &_temp); \
            _temp &= ~ (mask);  \
            _temp |= (data);    \
            cpssDxChPhyPortSmiRegisterWrite((devNum), (portNum), (phyReg), _temp);  \
        })
#define cpssDxChPhyPortXsmiRegisterRead(devNum, portNum, phyDev, phyReg, dataPtr) \
        cpssDxChPhyPort10GSmiRegisterRead((devNum), (portNum), PRV_CPSS_PHY_XSMI_PORT_ADDR_MAC((devNum), (portNum)), GT_TRUE, (phyReg), (phyDev), (dataPtr))
#define cpssDxChPhyPortXsmiRegisterWrite(devNum, portNum, phyDev, phyReg, data) \
        cpssDxChPhyPort10GSmiRegisterWrite((devNum), (portNum), PRV_CPSS_PHY_XSMI_PORT_ADDR_MAC((devNum), (portNum)), GT_TRUE, (phyReg), (phyDev), (data))
#define cpssDxChPhyPortXsmiRegisterWriteMask(devNum, portNum, phyDev, phyReg, data, mask) ({ \
            GT_U16 _temp;       \
            cpssDxChPhyPort10GSmiRegisterRead((devNum), (portNum), PRV_CPSS_PHY_XSMI_PORT_ADDR_MAC((devNum), (portNum)), GT_TRUE, (phyReg), (phyDev), &_temp); \
            _temp &= ~ (mask);  \
            _temp |= (data);    \
            cpssDxChPhyPort10GSmiRegisterWrite((devNum), (portNum), PRV_CPSS_PHY_XSMI_PORT_ADDR_MAC((devNum), (portNum)), GT_TRUE, (phyReg), (phyDev), _temp); \
        })

/* For two deivces
 */
#define DEV_SWDRV_IS_DEV_CPU_ENABLED(devNum) \
    (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E ? \
        ((PRV_CPSS_PP_MAC(devNum)->devType & 0x20000) == 0) : \
        (devNum == 0))

/* EXPORTED VARIABLES DECLARATIONS
 */
extern UI32_T dev_swdrv_local_first_dev_id;
extern GT_U16 dev_swdrv_phy_id[SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT][SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP];



/* -------------------------------------------------------------------------
 * FUNCTION NAME - DEV_SWDRV_IsCascadingPort
 * -------------------------------------------------------------------------
 * PURPOSE : To check if (device_id, phy_port) is a cascading port or not
 * INPUT   : device_id  -- 
 *           phy_port   --
 * OUTPUT  : None
 * RETURN  : True: cascading port, FALSE: not cascading port
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRV_IsCascadingPort(UI32_T device_id, UI32_T phy_port);

GT_STATUS DEV_SWDRV_cpssDxChBrgVlanUnkUnregFilterSet(
    GT_U16 vlanId,
    CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT packetType,
    CPSS_PACKET_CMD_ENT cmd);

GT_STATUS DEV_SWDRV_cpssDxChBrgVlanIpMcRouteEnable(
    CPSS_IP_PROTOCOL_STACK_ENT protocol,
    GT_BOOL enable);

#endif /*RTK-DB*/


#define DEV_SWDRV_CPU_QUEUE_TO_INTERNAL_PRIORITY(queue) ( (queue == SYS_DFLT_PRI_7_TO_QUEUE) ? 7 \
                                                        : (queue == SYS_DFLT_PRI_6_TO_QUEUE) ? 6 \
                                                        : (queue == SYS_DFLT_PRI_5_TO_QUEUE) ? 5 \
                                                        : (queue == SYS_DFLT_PRI_4_TO_QUEUE) ? 4 \
                                                        : (queue == SYS_DFLT_PRI_3_TO_QUEUE) ? 3 \
                                                        : (queue == SYS_DFLT_PRI_2_TO_QUEUE) ? 0 \
                                                        : (queue == SYS_DFLT_PRI_1_TO_QUEUE) ? 2 \
                                                        : 1)


#endif /* DEV_SWDRV_COMMON_H */

