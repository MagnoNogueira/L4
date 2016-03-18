
/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChNstPortIsolation.h
*
* DESCRIPTION:
*       CPSS DxCh NST Port Isolation Mechanism.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __cpssDxChNstPortIsolationh
#define __cpssDxChNstPortIsolationh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>


/*
 * typedef: enum CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT
 *
 * Description: Enumeration for L2 or/and L3 traffic type
 *
 * Enumerations:
 *    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E - Port isolation for L2
 *                                         packets (L2 Port Isolation table)
 *    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E - Port isolation for L3
 *                                         packets (L3 Port Isolation table)
 *
 */
typedef enum
{
   CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E,
   CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E
}CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT;


/*******************************************************************************
* cpssDxChNstPortIsolationEnableSet
*
* DESCRIPTION:
*       Function enables/disables the port isolation feature.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*       enable    - port isolation feature enable/disable
*                   GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);


/*******************************************************************************
* cpssDxChNstPortIsolationEnableGet
*
* DESCRIPTION:
*       Function gets enabled/disabled state of the port isolation feature.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       enablePtr - (pointer to) port isolation feature state
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - on NULL pointer 
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChNstPortIsolationTableEntrySet
*
* DESCRIPTION:
*       Function sets port isolation table entry. 
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking. If it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interface.
*                              Only portDev and Trunk are supported.
*       cpuPortMember        - port isolation for CPU Port
*                               GT_TRUE - member
*                               GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written to the
*                              L2/L3 PI table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface or 
*                                  localPortsMembersPtr
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may 
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationTableEntrySet
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_BOOL                                        cpuPortMember,                      
    IN CPSS_PORTS_BMP_STC                             *localPortsMembersPtr
);

/*******************************************************************************
* cpssDxChNstPortIsolationTableEntryGet
*
* DESCRIPTION:
*       Function gets port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking if it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interfaces
*                              Only portDev and Trunk are supported.
*
* OUTPUTS:
*       cpuPortMemberPtr     - (pointer to) port isolation for CPU Port 
*                                GT_TRUE - member
*                                GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written 
*                              to the L2/L3 PI table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may 
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationTableEntryGet
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    OUT GT_BOOL                                       *cpuPortMemberPtr,                      
    OUT CPSS_PORTS_BMP_STC                            *localPortsMembersPtr
);

/*******************************************************************************
* cpssDxChNstPortIsolationPortAdd
*
* DESCRIPTION:
*       Function adds single local port to port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Adding local port (may be also CPU port 63) to port isolation entry 
*       means that traffic which came from srcInterface and wish to egress
*       at the specified local port isn't blocked.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterface  - table index is calculated from source interface
*                       Only portDev and Trunk are supported.
*       portNum       - local port(include CPU port) to be added to bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface, portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationPortAdd
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_U8                                          portNum                      
);

/*******************************************************************************
* cpssDxChNstPortIsolationPortDelete
*
* DESCRIPTION:
*       Function deletes single local port to port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Deleting local port (may be also CPU port 63) to port isolation entry
*       means that traffic which came from srcInterface and wish to egress
*       at the specified local port is blocked.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterface  - table index is calculated from source interface 
*                       Only portDev and Trunk are supported.
*       portNum       - local port(include CPU port) to be deleted from bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface, portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In srcInterface parameter only portDev and Trunk are supported.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationPortDelete
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_U8                                          portNum                      
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChNstPortIsolationh */

