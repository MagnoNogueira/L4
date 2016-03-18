/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgStp.h
*
* DESCRIPTION:
*       Spanning Tree protocol facility DxCh CPSS implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/
#ifndef __cpssDxChBrgStph
#define __cpssDxChBrgStph

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>

/* DxCh size of STG (Spanning Tree Group) entry in words */
/* NOTE:
    devices with up to 32 ports (cpu not included) use 2 words.
    devices with more then 32 ports (Lion 0..59) use 4 words.
*/
#define CPSS_DXCH_STG_ENTRY_SIZE_CNS    4

/*******************************************************************************
* cpssDxChBrgStpInit
*
* DESCRIPTION:
*       Initialize STP library.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       dev    - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpInit
(
    IN GT_U8    dev
);

/*******************************************************************************
* cpssDxChBrgStpStateSet
*
* DESCRIPTION:
*       Sets STP state of port belonging within an STP group.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       dev    - device number
*       port   - port number
*       stpId  - STG (Spanning Tree Group) index  (0..255)
*       state  - STP port state.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or port or stpId or state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpStateSet
(
    IN GT_U8                    dev,
    IN GT_U8                    port,
    IN GT_U16                   stpId,
    IN CPSS_STP_STATE_ENT       state
);

/*******************************************************************************
* cpssDxChBrgStpStateGet
*
* DESCRIPTION:
*       Gets STP state of port belonging within an STP group.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       dev         - device number
*       port        - port number
*       stpId       - STG (Spanning Tree Group) index  (0..255)
*
* OUTPUTS:
*       statePtr    - (pointer to) STP port state.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or port or stpId or state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpStateGet
(
    IN GT_U8                    dev,
    IN GT_U8                    port,
    IN GT_U16                   stpId,
    OUT CPSS_STP_STATE_ENT      *statePtr
);

/*******************************************************************************
* cpssDxChBrgStpEntryGet
*
* DESCRIPTION:
*       Gets the RAW STP entry from HW.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       dev    - device number
*       stpId  - STG (Spanning Tree Group) index  (0..255)
*
* OUTPUTS:
*       stpEntryWordArr     - (Pointer to) The raw stp entry to get.
*                         memory allocated by the caller.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or stpId
*       GT_BAD_PTR               - on NULL pointer parameter
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpEntryGet
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    OUT GT_U32      stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
);

/*******************************************************************************
* cpssDxChBrgStpEntryWrite
*
* DESCRIPTION:
*       Writes a raw STP entry to HW.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       dev             - device number
*       stpId           - STG (Spanning Tree Group) index  (0..255)
*       stpEntryWordArr     - (Pointer to) The raw stp entry to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or stpId
*       GT_BAD_PTR               - on NULL pointer parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpEntryWrite
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    IN  GT_U32      stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ____cpssDxChBrgStph */

