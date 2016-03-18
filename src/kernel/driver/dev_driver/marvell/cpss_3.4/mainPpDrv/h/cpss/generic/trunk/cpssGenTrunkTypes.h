/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenTrunkTypes.h
*
* DESCRIPTION:
*       API definitions for 802.3ad Link Aggregation (Trunk) facility
*
*
*       States and Modes :
*       A. Application can work in one of 2 modes:
*           1. use "high level" trunk manipulations set of APIs - Mode "HL"
*           2. use "low level" trunk HW tables/registers set of APIs - MODE "LL"
*
*       B. Using "high level" trunk manipulations set of APIs - "High level"
*           Mode.
*           In this mode the CPSS synchronize all the relevant trunk
*           tables/registers, implement some WA for trunks Errata.
*           1. Create trunk
*           2. Destroy trunk
*           3. Add/remove member to/from trunk
*           4. Enable/disable in trunk
*           5. Add/Remove port to/from "non-trunk" entry
*        C. Using "low level" trunk HW tables/registers set of APIs- "Low level"
*           Mode.
*           In this mode the CPSS allow direct access to trunk tables/registers.
*           In this mode the Application required to implement the
*           synchronization between the trunk t6ables/registers and to implement
*           the WA to some of the trunk Errata.
*
*           1. Set per port the trunkId
*           2. Set per trunk the trunk members , number of members
*           3. Set per trunk the Non-trunk local ports bitmap
*           4. Set per Index the designated local ports bitmap
*
*         D. Application responsibility is not to use a mix of using API from
*            those 2 modes
*            The only time that the Application can shift between the 2 modes,
*            is only when there are no trunks in the device.
*
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/

#ifndef __cpssGenTrunkTypesh
#define __cpssGenTrunkTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/
#include <cpss/generic/cpssTypes.h>

/* max number of members in a trunk */
#define CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS   8

/*
 * Typedef: struct CPSS_TRUNK_MEMBER_STC
 *
 * Description: A structure to hold the info on trunk member : device,port
 *
 * Fields:
 *    port - the port number
 *    device - the device number
 */
typedef struct{
    GT_U8   port;
    GT_U8   device;

}CPSS_TRUNK_MEMBER_STC;

/*
 * Typedef: enumeration CPSS_TRUNK_TYPE_ENT
 *
 * Description: An enumeration for the trunk type in the DB of the CPSS.
 *
 * values:
 *    CPSS_TRUNK_TYPE_FREE_E    - the trunk is empty .
 *              trunk at this stage supports ALL (high level) APIs.
 *    CPSS_TRUNK_TYPE_REGULAR_E - the trunk used as regular trunk.
 *              supports next (high level) APIs. (DXCH example) :
 *       cpssDxChTrunkMembersSet ,
 *       cpssDxChTrunkMemberAdd , cpssDxChTrunkMemberRemove,
 *       cpssDxChTrunkMemberEnable , cpssDxChTrunkMemberDisable
 *       cpssDxChTrunkDbEnabledMembersGet , cpssDxChTrunkDbDisabledMembersGet
 *       cpssDxChTrunkDesignatedMemberSet , cpssDxChTrunkDbDesignatedMemberGet
 *       and cpssDxChTrunkDbIsMemberOfTrunk
 *
 *    CPSS_TRUNK_TYPE_CASCADE_E - the trunk used as "cascade trunk" ('Local trunk').
 *              supports next (high level) APIs. (DXCH example) :
 *       cpssDxChTrunkCascadeTrunkPortsSet , cpssDxChTrunkCascadeTrunkPortsGet
 *       and cpssDxChTrunkDbIsMemberOfTrunk
 *
 */
typedef enum{
    CPSS_TRUNK_TYPE_FREE_E,
    CPSS_TRUNK_TYPE_REGULAR_E,
    CPSS_TRUNK_TYPE_CASCADE_E
}CPSS_TRUNK_TYPE_ENT;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenTrunkTypesh */

