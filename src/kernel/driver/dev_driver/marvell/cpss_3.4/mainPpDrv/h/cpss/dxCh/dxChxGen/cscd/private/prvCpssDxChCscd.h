/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* prvCpssDxChCscd.h
*
* DESCRIPTION:
*       CPSS DXCH Cascading private API.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#ifndef __prvCpssDxChCscdh
#define __prvCpssDxChCscdh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*******************************************************************************
* prvCpssDxChCscdDevMapTableEntryHwFormatGet
*
* DESCRIPTION:
*       Get Cascade Device Map table entry in HW format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number
*       cascadeLinkPtr - (pointer to)A structure holding the cascade link type
*                      (port (0..63) or trunk(0..127)) and the link number
*                      leading to the target device.
*       srcPortTrunkHashEn - Relevant when (cascadeLinkPtr->linkType ==
*                       CPSS_CSCD_LINK_TYPE_TRUNK_E)
*                       Enabled to set the load balancing trunk hash for packets
*                       forwarded via an trunk uplink to be based on the
*                       packet’s source port, and not on the packets data.
*                       Indicates the type of uplink.
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E -
*                       Load balancing trunk hash is based on the ingress pipe
*                       hash mode as configured by function
*                       cpssDxChTrunkHashGeneralModeSet(...)
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E - Load
*                       balancing trunk hash for this cascade trunk interface is
*                       based on the packet’s source port, regardless of the
*                       ingress pipe hash mode
*                       NOTE : this parameter is relevant only to DXCH2
*                       and above devices
*
* OUTPUTS:
*       hwDataPtr       - Pointer to Device Map Table entry format data.  
*       numBitsPtr      - Pointer to number of bits in the entry.                           
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad trunk hash mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCscdDevMapTableEntryHwFormatGet
(
    IN  GT_U8                        devNum,
    IN  CPSS_CSCD_LINK_TYPE_STC      *cascadeLinkPtr,
    IN  CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT srcPortTrunkHashEn,
    OUT GT_U32                       *hwDataPtr,
    OUT GT_U32                       *numBitsPtr
);

/*******************************************************************************
* prvCpssDxChCscdDevMapTableEntryGet
*
* DESCRIPTION:
*            Get the cascade map table entry data from hardware format. 
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       hwData         - hardware format data
*
* OUTPUTS:
*       cascadeLinkPtr - (pointer to)A structure holding the cascade link type
*                      (port (0..63) or trunk(0..127)) and the link number
*                      leading to the target device.
*       srcPortTrunkHashEnPtr - Relevant when (cascadeLinkPtr->linkType ==
*                       CPSS_CSCD_LINK_TYPE_TRUNK_E)
*                       (pointer to) Enabled to set the load balancing trunk
*                       hash for packets forwarded via an trunk uplink to be
*                       based on the packet’s source port, and not on the
*                       packets data.
*                       Indicates the type of uplink.
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E -
*                       Load balancing trunk hash is based on the ingress pipe
*                       hash mode as configured by function
*                       cpssDxChTrunkHashGeneralModeSet(...)
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E - Load
*                       balancing trunk hash for this cascade trunk interface is
*                       based on the packet’s source port, regardless of the
*                       ingress pipe hash mode
*                       NOTE : this parameter is relevant only to DXCH2 and
*                       above devices
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCscdDevMapTableEntryGet 
(
    IN GT_U8                         devNum,
    IN GT_U32                        hwData, 
    OUT CPSS_CSCD_LINK_TYPE_STC      *cascadeLinkPtr,
    OUT CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT *srcPortTrunkHashEnPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChCscdh */

